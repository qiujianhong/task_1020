#ifdef AREA_ANHUI

#include "CtsAll.h"
#include "area.h"
#include "iniparser.h"
//#include "mapmsg_split.h"
//��ַΪ00��ʾû�� �˿�RS485_PORT1

extern OOP_PERCEPTION_TERMINAL_LIST_T perceptionterminal;
uint16  GetVirtualPipePn(uint8 *pipeaddr,uint8 *termianladdr)
{
    uint16 i ,j;

    TASK_FMT_TRACE( 0, REC_LOG, "perceptionterminal.num is %d\n",perceptionterminal.num);
    for(i=0;i<perceptionterminal.num;i++)
    {
        TASK_BUF_TRACE( 0, REC_LOG, perceptionterminal.terminalinfo[i].terminaladdr,6);
        if(memcmp(termianladdr,perceptionterminal.terminalinfo[i].terminaladdr,6)==0)
        {
            break;
        }
    }
    if(i>=perceptionterminal.num)
    {
        return 0xFFFF;
    }
    TASK_FMT_TRACE( 0, REC_LOG, "perceptionterminal.terminalinfo[i].meterlist.num is %d\n",perceptionterminal.terminalinfo[i].meterlist.num);
    for(j=0;j<perceptionterminal.terminalinfo[i].meterlist.num;j++)
    {
        TASK_FMT_TRACE( 0, REC_LOG, "j is %d\n",j);
        TASK_BUF_TRACE( 0, REC_LOG, perceptionterminal.terminalinfo[i].meterlist.meterinfo[j].pipeaddr,6);
        if(memcmp(pipeaddr,perceptionterminal.terminalinfo[i].meterlist.meterinfo[j].pipeaddr,6)==0)
        {
           TASK_FMT_TRACE( 0, REC_LOG, "perceptionterminal.terminalinfo[i].meterlist.meterinfo[j].pn is %d\n",perceptionterminal.terminalinfo[i].meterlist.meterinfo[j].pn);
           return perceptionterminal.terminalinfo[i].meterlist.meterinfo[j].pn;
        }
    }
    return 0xFFFF;
}

BOOL  CmpVirtualPipeAddr(uint8 *pipeaddr,uint8 *termianladdr)
{
    uint16 i ,j;

    TASK_FMT_TRACE( 0, REC_LOG, "perceptionterminal.num is %d\n",perceptionterminal.num);
    for(i=0;i<perceptionterminal.num;i++)
    {
        TASK_BUF_TRACE( 0, REC_LOG, perceptionterminal.terminalinfo[i].terminaladdr,6);
        if(memcmp(termianladdr,perceptionterminal.terminalinfo[i].terminaladdr,6)==0)
        {
            break;
        }
    }
    if(i>=perceptionterminal.num)
    {
        return FALSE;
    }
    TASK_FMT_TRACE( 0, REC_LOG, "perceptionterminal.terminalinfo[i].meterlist.num is %d\n",perceptionterminal.terminalinfo[i].meterlist.num);
    for(j=0;j<perceptionterminal.terminalinfo[i].meterlist.num;j++)
    {
        TASK_FMT_TRACE( 0, REC_LOG, "j is %d\n",j);
        TASK_BUF_TRACE( 0, REC_LOG, perceptionterminal.terminalinfo[i].meterlist.meterinfo[j].pipeaddr,6);
        if(memcmp(pipeaddr,perceptionterminal.terminalinfo[i].meterlist.meterinfo[j].pipeaddr,6)==0)
        {
           TASK_FMT_TRACE( 0, REC_LOG, "perceptionterminal.terminalinfo[i].meterlist.meterinfo[j].pn is %d\n",perceptionterminal.terminalinfo[i].meterlist.meterinfo[j].pn);
           return   TRUE;
        }
    }
    return FALSE;
}


int  GetVirtualPipeAddr(uint16 pn,uint8 *MtrAddr)
{
    uint16 i ,j;

    for(i=0;i<perceptionterminal.num;i++)
    {    
        for(j=0;j<perceptionterminal.terminalinfo[i].meterlist.num;j++)
        {
            if(pn==perceptionterminal.terminalinfo[i].meterlist.meterinfo[j].pn)
            {
               memcpy_r(MtrAddr,perceptionterminal.terminalinfo[i].meterlist.meterinfo[j].pipeaddr,6);
                return 0;
            }
        }
    }
    if(i>=perceptionterminal.num)
        return -1;
    return 0;
}

/*******************************************************************************
* ��������: Save698TerminalEventData 
* ��������://д0xFF100A00 �¼��ϱ�����
* �������:pipe �˿ں�
        DataBuf �¼�����
        DataLen ����
        Pn ����
        MainOad �¼���oad
        StartTime ����ʱ��
* �������: 
* �� �� ֵ: 
*******************************************************************************/
void Save698TerminalEventData(uint8 pipe,uint8* DataBuf,uint16 DataLen,uint16 Pn,OOP_OAD_U MainOad,time_t StartTime)
{

    uint8 i,j,k;
    uint16 index=0;
    uint8 CsdNum;
    uint8 RecordNum;
    uint16 rptLen = 0;
    //static uint16 terminalnum=10000;
    uint8 meternum;
    //�¼��ϱ�ʱ�ѹ̶���CSD�����ȥ
    uint8 rptBuff[2000] = {0x00};
    CsdNum = DataBuf[index++];
    OOP_PERCEPTION_TERMINAL_LIST_T tmpperceptionterminal;
    OOP_PERCEPTION_TERMINAL_INFO_T singalterminalinfo;

    memset((uint8*)&singalterminalinfo,0x00,sizeof(OOP_PERCEPTION_TERMINAL_INFO_T));
    //rptBuff[rptLen++] = 1;  //CSD����1 ��¼�Ͷ���������
    memcpy_r(&rptBuff[rptLen],(uint8*)&MainOad.value,4);
    rptLen+=4;
    rptBuff[rptLen++] = CsdNum; //  

    TASK_FMT_DEBUG( pipe, RELAY_LOG,"Save698TerminalEventData �������ϵ\n");
    TASK_BUF_DEBUG( pipe, RELAY_LOG,DataBuf, DataLen);
    for(i=0;i<CsdNum;i++)
    {
        if(DataBuf[index++]!=0)
            return;
        rptBuff[rptLen++] = 0x00;
        memcpy(&rptBuff[rptLen],&DataBuf[index],4);
        index+=4;
        rptLen+=4;
    }
    if(DataBuf[index++]==0)   //0Ϊ����DAR 1 ΪSEQUENCE OF A-RecordRow
        return;
    RecordNum = DataBuf[index++];
    if(RecordNum==0)
        return;
        
    rptBuff[rptLen++] = 1;//�������
    rptBuff[rptLen++] = RecordNum;//��¼����

    //��Ҫ��������ÿ����¼ ����Ҫ���ĸ����ϱ����Լ�ʱ���ǩ
    //uint8 tmpBuff[30]={0};
    uint16 indexbak = index;
    uint16 tmpindexbak = index;
    uint8 flag =0 ;
    //uint8 tempmetrenum=0;
    if(perceptionterminal.num == 0)
    {
        perceptionterminal.totalpn = 10000;
    }
    TASK_FMT_DEBUG( pipe, RELAY_LOG,"perceptionterminal.num is %d\n,perceptionterminal.totalpn is %d\n",perceptionterminal.num,perceptionterminal.totalpn);
    memcpy((uint8*)&tmpperceptionterminal,(uint8*)&perceptionterminal,sizeof(perceptionterminal));
    for(i=0;i<RecordNum;i++)
    {
        for(j=0;j<CsdNum;j++)
        {
            TASK_FMT_DEBUG( pipe, RELAY_LOG,"j is %d,CsdNum is %d\n",j,CsdNum);
            if(j ==CsdNum-2)
            {
                tmpindexbak = index;
                memcpy_r(tmpperceptionterminal.terminalinfo[tmpperceptionterminal.num].terminaladdr,&DataBuf[tmpindexbak+3],6);
                TASK_FMT_DEBUG( pipe, RELAY_LOG,"��֪�ն˵�ַ\n");
                TASK_BUF_DEBUG( pipe, RELAY_LOG,tmpperceptionterminal.terminalinfo[tmpperceptionterminal.num].terminaladdr, 6);
            }
            if(j ==CsdNum-1)
            {
                tmpindexbak = index;
                tmpindexbak = tmpindexbak+1;
                meternum = DataBuf[tmpindexbak++];
                TASK_FMT_DEBUG( pipe, RELAY_LOG,"meternum is %d\n",meternum);
                TASK_FMT_DEBUG( pipe, RELAY_LOG,"tmpperceptionterminal.terminalinfo[tmpperceptionterminal.num].meterlist.num is %d\n",tmpperceptionterminal.terminalinfo[tmpperceptionterminal.num].meterlist.num);
                for(k=0;k<meternum;k++)
                {
                    flag =0;
                    tmpindexbak = tmpindexbak+5;
                    //tempmetrenum = perceptionterminal.terminalinfo[perceptionterminal.num].meterlist.num;
                    memcpy(tmpperceptionterminal.terminalinfo[tmpperceptionterminal.num].meterlist.meterinfo[k].pipeaddr,&DataBuf[tmpindexbak],6); 
                    tmpindexbak = tmpindexbak + 9;
                    TASK_FMT_DEBUG( pipe, RELAY_LOG,"��%d����ͨ��\n",k);
                    TASK_BUF_DEBUG( pipe, RELAY_LOG,tmpperceptionterminal.terminalinfo[tmpperceptionterminal.num].meterlist.meterinfo[k].pipeaddr, 6);
                    memcpy(tmpperceptionterminal.terminalinfo[tmpperceptionterminal.num].meterlist.meterinfo[k].meteraddr,&DataBuf[tmpindexbak],6); 
                    TASK_BUF_DEBUG( pipe, RELAY_LOG,tmpperceptionterminal.terminalinfo[tmpperceptionterminal.num].meterlist.meterinfo[k].meteraddr, 6);
                    tmpindexbak = tmpindexbak + 7;
                    tmpperceptionterminal.terminalinfo[tmpperceptionterminal.num].meterlist.meterinfo[k].pn = DataBuf[tmpindexbak];
                    tmpindexbak = tmpindexbak + 1;
                    if(TRUE == CmpVirtualPipeAddr(tmpperceptionterminal.terminalinfo[tmpperceptionterminal.num].meterlist.meterinfo[k].pipeaddr,tmpperceptionterminal.terminalinfo[tmpperceptionterminal.num].terminaladdr))
                    {
                        TASK_FMT_DEBUG( pipe, RELAY_LOG,"�����µĸ�֪�ն˵ĵ�ַ\n");
                        continue;
                    }
                    flag =1;
                    tmpperceptionterminal.terminalinfo[tmpperceptionterminal.num].meterlist.meterinfo[k].pn = tmpperceptionterminal.totalpn;
                    tmpperceptionterminal.totalpn++;
                    TASK_FMT_DEBUG( pipe, RELAY_LOG,"tmpperceptionterminal.num is %d\n",tmpperceptionterminal.num);
                    TASK_FMT_DEBUG( pipe, RELAY_LOG,"tmpperceptionterminal.terminalinfo[tmpperceptionterminal.num].meterlist.meterinfo[k].pn is %d,k is %d\n",tmpperceptionterminal.terminalinfo[tmpperceptionterminal.num].meterlist.meterinfo[k].pn,k);
                    TASK_FMT_DEBUG( pipe, RELAY_LOG,"tmpperceptionterminal.terminalinfo[tmpperceptionterminal.num].meterlist.num is %d\n",tmpperceptionterminal.terminalinfo[tmpperceptionterminal.num].meterlist.num);
                    tmpperceptionterminal.terminalinfo[tmpperceptionterminal.num].meterlist.num++;
                }
                
                if(flag ==1)
                {
                    memcpy((uint8*)&singalterminalinfo,(uint8*)&tmpperceptionterminal.terminalinfo[tmpperceptionterminal.num],sizeof(OOP_PERCEPTION_TERMINAL_INFO_T));
                    tmpperceptionterminal.num++;
                }
            }
            taskmng_oop_next_read_data_analysis(DataBuf,&index,DataLen,DT_NULL,FALSE);
        }
    }
    //TASK_FMT_DEBUG( pipe, RELAY_LOG,"indexbak  %d index %d  DataLen %d \n",indexbak,index,DataLen);
    memcpy(&rptBuff[rptLen],&DataBuf[indexbak],index-indexbak);
    rptLen+=(index-indexbak);
    memcpy((uint8*)&perceptionterminal,(uint8*)&tmpperceptionterminal,sizeof(tmpperceptionterminal));
    if(flag ==1)
    {
        WritePerceptionTerminalData(pipe,(uint8*)&singalterminalinfo,sizeof(singalterminalinfo),PerceptionTerminalOad);
    }
    //����õ��¼�����д����������
    taskmng_event_report_data_write(pipe,rptBuff,rptLen,EventReportOad);
}

/*******************************************************************************
* ��������: Save698SepicialEventData 
* ��������://д0xFF100A00 �¼��ϱ�����
* �������:pipe �˿ں�
        DataBuf �¼�����
        DataLen ����
        Pn ����
        MainOad �¼���oad
        StartTime ����ʱ��
* �������: 
* �� �� ֵ: 
*******************************************************************************/
void Save698SepicialEventData(uint8 pipe,uint8* DataBuf,uint16 DataLen,uint16 Pn,OOP_OAD_U MainOad,time_t StartTime)
{

    uint16 rptLen = 0;

    uint8 rptBuff[2000];
  
    memcpy_r(&rptBuff[rptLen],(uint8*)&MainOad.value,4);
    rptLen+=4;
    memcpy(&rptBuff[rptLen],DataBuf,DataLen);
    rptLen = rptLen+DataLen;
    //����õ��¼�����д����������
    taskmng_event_report_data_write(pipe,rptBuff,rptLen,EventReportOad);
}

/*******************************************************************************
* ��������: OopPatchTeminalAmrDataSave_Record
* ��������:�����洢��������Լ���¼�����ݽ���
* �������: pipe �˿ں�
            DataBuf              //���յ��ĵ������
            DataLen              //���յ��ĵ����
            MtrIndex             //���������Ϣ
            pTmp_NormalLink      //��������ڵ�
            RecvRunTask     //��������ʱ����
            StoreTime            //�洢ʱ��
* �������:
* �� �� ֵ:
*******************************************************************************/
uint16 OopPatchTeminalAmrDataSave_Record(uint8 Pipe,uint8 *DataBuf,uint16 DataLen,uint16 MtrIndex,SCHEME_TASK_RUN_INFO* pTmp_NormalLink)
{
    AmrThreadArg_T  *pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[Pipe];
    
    uint16           Dataoffset = 0;
    uint8           CsdNum = DataBuf[Dataoffset++];    //CSD��Ŀ
    //OOP_OAD_U            Oad[128];
    OOP_CSD_T    csd[32];
     uint8           i;
    uint8           j;
    uint8           k;
    uint8           RecordNum = 0;
    
    //uint16          Pn;
    uint8  pipeaddr[6];
    uint8  termianladdr[6];
    uint16 virtualpn;
    uint16          OadLen = 0;

    DateTimeBcd_t   StoreTm;
//    OadInfoMap   pMap;
//    OOP_OAD_U            OadTmp;
    OOP_OAD_U       MainOad;

    //TASK_FMT_TRACE(Pipe, REC_LOG,"698�����洢��¼������ SchemeType %d RoadIndex %d CsdNum %d\n",pTmp_NormalLink->SchemeType,CsdNum);
    //����RCSD��Ӧ��OAD��Ϣ
    for(i = 0; i < CsdNum; i++)
    {
        if(DataBuf[Dataoffset++] != 0)
        {
            csd[i].choice =1;
            memcpy_r((uint8*)&csd[i].road.oadMain.value,&DataBuf[Dataoffset],4);
            Dataoffset += 4;
            csd[i].road.oadCols.nNum = DataBuf[Dataoffset++];
            for(k = 0;k < csd[i].road.oadCols.nNum;k++)
            {
                memcpy_r((uint8*)&csd[i].road.oadCols.oad[k].value,&DataBuf[Dataoffset],4);
                Dataoffset += 4;
                 TASK_FMT_TRACE(Pipe, REC_LOG,"���ص�csd %d  oad %08x \n",i,csd[i].road.oadCols.oad[k].value);
            }           
        }
        else
        {           
            csd[i].choice =0;     
            memcpy_r((uint8*)&csd[i].oad.value,&DataBuf[Dataoffset],4);
            Dataoffset += 4;
            TASK_FMT_TRACE(Pipe, REC_LOG,"���ص�csd %d  oad %08x \n",i,csd[i].oad.value);
        }  
       
    }
    //������Ϣ [0] DAR
    if(DataBuf[Dataoffset++] == 0x0)
    {
        //printf("������Ϣ [0] DAR : %d  %d  %d\n",MtrIndex,pTmp_NormalLink->MsInfo[MtrIndex].CSDIndex,pTmp_NormalLink->MsInfo[MtrIndex].SigFrameNum);
        pAmrThreadArg->PatchMtrInfo[MtrIndex].CSDIndex += pAmrThreadArg->PatchMtrInfo[MtrIndex].SigFrameNum;
        return 0;
    }
    //MainOad = pTmp_NormalLink->CsdTask.RoadTask[0].MainOad;
    RecordNum = DataBuf[Dataoffset++];
    //TASK_FMT_TRACE(Pipe, REC_LOG,"mainoad %08x recorenum %d \n",MainOad.value,RecordNum);
    if(RecordNum == 0x0)
    {
        pAmrThreadArg->PatchMtrInfo[MtrIndex].CSDIndex += pAmrThreadArg->PatchMtrInfo[MtrIndex].SigFrameNum;
        return 0;
    }
    for(i = 0; i < RecordNum; i++)
    {
        for(j = 0; j < CsdNum; j++)
        {
            if(csd[j].choice == 1)
            {
                MainOad = csd[j].road.oadMain;   
                Dataoffset =Dataoffset +2;
                for(k=0;k < csd[j].road.oadCols.nNum;k++)
                {
                    if(pTmp_NormalLink->StoreTimeType == STRTMTYP_FRZTIME)
                    {
                        //����record ��ʱ���һ��OIΪʱ��
                        if(csd[j].road.oadCols.oad[k].nObjID == 0x2021)
                        {
                            //����record ��ʱ���һ��OIΪʱ��
                            if(csd[j].road.oadCols.oad[k].nObjID == 0x2021)
                            {
                                DateTimeHex_t        TmpDateTimeHex;
                                memset(&TmpDateTimeHex,0x0,sizeof(TmpDateTimeHex));
                                TASK_FMT_TRACE( Pipe, REC_LOG, "����ʱ��,Dataoffset is %d\n",Dataoffset);
                                TASK_BUF_TRACE( Pipe, REC_LOG, DataBuf+Dataoffset+1,6);
                                TmpDateTimeHex.year_h = DataBuf[Dataoffset+1];
                                TmpDateTimeHex.year_l = DataBuf[Dataoffset+2];
                                TmpDateTimeHex.month  = DataBuf[Dataoffset+3];
                                TmpDateTimeHex.day    = DataBuf[Dataoffset+4];
                                TmpDateTimeHex.hour   = DataBuf[Dataoffset+5];
                                TmpDateTimeHex.min    = DataBuf[Dataoffset+6];

                                if(DataBuf[Dataoffset+7] != 0xff)//������ظ�Ϊ0xff(������)
                                TmpDateTimeHex.sec    = DataBuf[Dataoffset+7];

                                DT_DateTimeHex2BCD(&TmpDateTimeHex,&StoreTm);
                            }

                        }
                    }
                    else
                    {
                        StoreTm = taskmng_patch_scheme_task_store_time_get(Pipe,MtrIndex,pTmp_NormalLink);
                    }
                    //TASK_FMT_TRACE(Pipe, REC_LOG, "��%d����¼��%d��������  \n",i,j);
                    TASK_FMT_TRACE(Pipe, REC_LOG,"StoreTm %02x/%02x/%02x %02x:%02x:%02x \n",StoreTm.year_l,StoreTm.month,StoreTm.day,StoreTm.hour,StoreTm.min,StoreTm.sec);

                    OadLen = OopPatchAmrData(Pipe,MtrIndex,MainOad,csd[j].road.oadCols.oad[k],DataBuf+Dataoffset,(DataLen-Dataoffset),pTmp_NormalLink,&StoreTm);
                    Dataoffset += OadLen;
                 }
            }
        else
        {

            if(csd[j].oad.value == 0x202A0200)
            {
                memcpy(pipeaddr,DataBuf+Dataoffset+3,6);
                TASK_FMT_TRACE( Pipe, REC_LOG, "csd %ld  Dataoffset is %d\n",j,Dataoffset);
                TASK_BUF_TRACE( Pipe, REC_LOG, DataBuf+Dataoffset,50);
                TASK_BUF_TRACE( Pipe, REC_LOG, termianladdr,6);
                TASK_BUF_TRACE( Pipe, REC_LOG, pipeaddr,6);
                virtualpn = GetVirtualPipePn(pipeaddr,termianladdr);
                TASK_FMT_TRACE( Pipe, REC_LOG, "virtualpn %ld  \n",virtualpn);
            }

            taskmng_oop_next_read_data_analysis(DataBuf,&Dataoffset,DataLen,DT_NULL,FALSE);
            //OadLen = OopMeterAmrData(Pipe,MtrIndex,MainOad,csd[j].oad,DataBuf+Dataoffset,(DataLen-Dataoffset),pTmp_NormalLink,RecvRunTask,&StoreTm);
            //Dataoffset += OadLen;
        }
        }
    }
    pAmrThreadArg->PatchMtrInfo[MtrIndex].CSDIndex += pAmrThreadArg->PatchMtrInfo[MtrIndex].SigFrameNum;

    return 1;
}



/*******************************************************************************
* ��������: OopMeterAmrData
* ��������:�洢��������Լ������
* �������:pipe �˿ں�
         MtrIndex ������
         MainOad ��oad
         Oad ��oad
         pRcvBuf �յ�������buf
         RcvLen �յ������ݳ���
         pTmp_NormalLink �������
         RecvRunTask ��������ʱ����
         StoreTime �洢ʱ��
         
* �������:
* �� �� ֵ: �ѽ����洢�����ݳ���
*******************************************************************************/
uint16 OopTerminalAmrData(uint8 Pipe,uint16 MtrIndex,OOP_OAD_U MainOad,OOP_OAD_U Oad, uint8* pRcvBuf, uint16 RcvLen, SCHEME_TASK_RUN_INFO* pTmp_NormalLink,CTS_RUN_TASK_T *RecvRunTask , DateTimeBcd_t *StoreTime,uint16 pn)
{
    AmrThreadArg_T *pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[Pipe];
    uint16 offset =0 ;
    uint16 DataLen = 0;
    uint8 DataBuff[MAX_MSG_LEN]={0};
    uint16 Pn;
    uint8 logicid;
    DateTimeBcd_t  tmpTime;
    OadStoreInfo StoreInfo;
    memset(&StoreInfo,0x00,sizeof(OadStoreInfo));
    OadInfoMap pOadInfo;
    memset(&pOadInfo,0x00,sizeof(OadInfoMap));
    BOOL ret;
    uint8 pwrType;
 
    Pn = pn;
    logicid = pAmrThreadArg->pMtrAmrInfo[MtrIndex].logicid;
    if(pAmrThreadArg->pMtrAmrInfo[MtrIndex].IsNeedGetSafeMode == TRUE)
    {
        //����ESAM���к�
        if(Oad.nObjID==0x4002)
        {
            offset+=2;;
           
            uint8 MtrNo[8]={0};
            uint8 len = pRcvBuf[1];
            if(len>8)
                memcpy(MtrNo,pRcvBuf+2+len-8,8);
            else
                memcpy(MtrNo+8-len,pRcvBuf+2,len);
            TASK_FMT_TRACE(Pipe,REC_LOG,"����ESAM���к� %d 0x%02x%02x%02x%02x%02x%02x%02x%02x\n",Pn,MtrNo[0],MtrNo[1],
                MtrNo[2],MtrNo[3],MtrNo[4],MtrNo[5],MtrNo[6],MtrNo[7]);
            WriteOopMeterSecurityEsamSerialNo(Pipe,Pn,MtrNo);
            Set_ReadMtrNum(Pipe,MtrNo);
            offset+=len;
        }
        //�洢��ȫģʽ
        if(Oad.nObjID==0xF101)
        {
            offset++;
            if(pRcvBuf[1]!=1)
                pRcvBuf[1]=0;
            TASK_FMT_TRACE(Pipe,REC_LOG,"����洢��ȫģʽ %d ���� %d \n",Pn,pRcvBuf[1]);
            WriteOopMeterSecuritySecModEn(Pipe,Pn,pRcvBuf[1]);
            Set_MtrSecModEn(Pipe,pRcvBuf[1]);
            pAmrThreadArg->pMtrAmrInfo[MtrIndex].IsNeedGetSafeMode=FALSE;
            offset++;
        }
        return offset;
    }
    
    //����oad��ȡ��Ӧ�Ľṹ����Ϣ
    ret = taskmng_oad_info_get( Oad,&pOadInfo);
    TASK_FMT_TRACE(Pipe,REC_LOG,"OopMeterAmrData OAD %08x ret %d \n",Oad.value,ret);
    if(ret==FALSE)
    {
        return 0;
    }
    if(pRcvBuf[0]==0) //���Ϊdatanull ���������
    {
        TASK_FMT_TRACE(Pipe,REC_LOG,"dataΪ�� ������\n");
        return 1;
    }
    pwrType=taskmng_mtr_conntype_get(Pn,logicid);
    Convert_Oop_Data_To_Struct(pRcvBuf,&offset,RcvLen,DataBuff,&pOadInfo,&DataLen,pwrType);
    TASK_FMT_TRACE(Pipe, REC_LOG,"Convert_Oop_Data_To_Struct  DataLen %d ת�������� \n",DataLen);

    //TASK_BUF_TRACE(Pipe, REC_LOG,DataBuff, DataLen);
    
    StoreInfo.recordOAD.logicId=logicid;
    StoreInfo.recordOAD.infoNum = Pn;

    StoreInfo.recordOAD.road.oadMain=MainOad;
    StoreInfo.recordOAD.road.oadCols.nNum=1;
    StoreInfo.recordOAD.road.oadCols.oad[0]=Oad;
    StoreInfo.recordOAD.classtag = 5;
    if(Pn>=10000)
    {
        GetVirtualPipeAddr(Pn,(uint8*)StoreInfo.MAC.nValue);
    }
    else
    {
        taskmng_mtr_addr_get( Pn, logicid,(uint8*)StoreInfo.MAC.nValue);
    }
    memcpy_r((uint8*)&tmpTime,(uint8*)RecvRunTask->ThisTurnBeginTime,7);
    DT_DateTimeBCD2Time(&tmpTime,(time_t*)&StoreInfo.colStartTm);     //��ʼʱ��

    if(StoreTime==NULL)
    {
        time_t storetm;
        OOP_TI_T delay;
        DT_DateTimeBCD2Time(&tmpTime,&storetm);
        delay.unit = pTmp_NormalLink->DelayTimeUnit;
        delay.value = pTmp_NormalLink->DelayTime;
        DT_TimeSubInv(&storetm, delay);
        tmpTime = taskmng_acq_store_time_get(pTmp_NormalLink->StoreTimeType,storetm);
        DT_DateTimeBCD2Time(&tmpTime,(time_t*)&StoreInfo.colStoreTm);
    }
    else
    {
        DT_DateTimeBCD2Time(StoreTime,(time_t*)&StoreInfo.colStoreTm);
        
        TASK_FMT_TRACE(Pipe, REC_LOG,"StoreTm 20%02x/%02x/%02x %02x:%02x:%02x \n",StoreTime->year_l,StoreTime->month,StoreTime->day,StoreTime->hour,StoreTime->min,StoreTime->sec);
    }
    StoreInfo.colEndTm = time(NULL);
    taskmng_meter_data_save(DataBuff,DataLen ,StoreInfo, Pipe );

    return offset;
}

/*******************************************************************************
* ��������: �洢��������Լ���¼�����ݽ���
* ��������:
* �������: pipe �˿ں�
            DataBuf              //���յ��ĵ������
            DataLen              //���յ��ĵ����
            MtrIndex             //���������Ϣ
            pTmp_NormalLink      //��������ڵ�
            RecvRunTask     //��������ʱ����
            StoreTime            //�洢ʱ��
* �������:
* �� �� ֵ:
*******************************************************************************/
uint16 taskmng_oop_meter_data_tasktable_save(uint8 Pipe,uint8 *DataBuf,uint16 DataLen,uint16 MtrIndex,SCHEME_TASK_RUN_INFO* pTmp_NormalLink,CTS_RUN_TASK_T *RecvRunTask,DateTimeBcd_t *StoreTime)
{
    AmrThreadArg_T  *pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[Pipe];
    uint16           Dataoffset = 0;
    uint8           CsdNum = DataBuf[Dataoffset++];    //CSD��Ŀ
    //OOP_OAD_U            Oad[128];
    OOP_CSD_T    csd[32];
    uint8           i;
    uint8           j;
    uint8           k;
    uint8           RecordNum = 0;
    
    //uint16          Pn;
    uint16          OadLen = 0;
    uint8           ExistPosae = 0;
    DateTimeBcd_t   StoreTm;
//    OadInfoMap   pMap;
//    OOP_OAD_U            OadTmp;
    OOP_OAD_U       MainOad;
    uint8           RoadIndex=0;
    uint8  pipeaddr[6];
    uint8  termianladdr[6];
    uint16 virtualpn;
    uint32 EventSeq;
    uint32 EventSeqReport;
    uint16 Pn = pAmrThreadArg->pMtrAmrInfo[MtrIndex].Pn;//�������

    taskmng_mtr_addr_get(Pn,0,termianladdr);
    virtualpn = Pn;
    if(pTmp_NormalLink->SchemeType == EVENTPLAN)
    {
        RoadIndex = pAmrThreadArg->pMtrAmrInfo[MtrIndex].ROADIndex;
        
    }
    TASK_FMT_TRACE(Pipe, REC_LOG,"698�洢��¼������ SchemeType %d RoadIndex %d CsdNum %d\n",pTmp_NormalLink->SchemeType,RoadIndex,CsdNum);
    TASK_BUF_TRACE( Pipe, REC_LOG, DataBuf,50);
    //����RCSD��Ӧ��OAD��Ϣ
    for(i = 0; i < CsdNum; i++)
    {
        if(DataBuf[Dataoffset++] != 0)
        {
            csd[i].choice =1;
            memcpy_r((uint8*)&csd[i].road.oadMain.value,&DataBuf[Dataoffset],4);
            Dataoffset += 4;
            csd[i].road.oadCols.nNum = DataBuf[Dataoffset++];
            for(k = 0;k < csd[i].road.oadCols.nNum;k++)
            {
                memcpy_r((uint8*)&csd[i].road.oadCols.oad[k].value,&DataBuf[Dataoffset],4);
                Dataoffset += 4;
                if(pTmp_NormalLink->CsdTask.RoadTask[RoadIndex].MainOad.nObjID==0x5004 && csd[i].road.oadCols.oad[k].nObjID==0x0010)
                ExistPosae=1;
                 TASK_FMT_TRACE(Pipe, REC_LOG,"���ص�csd %d  oad %08x \n",i,csd[i].road.oadCols.oad[k].value);
            }           
        }
        else
        {           
            csd[i].choice =0;     
            memcpy_r((uint8*)&csd[i].oad.value,&DataBuf[Dataoffset],4);
            Dataoffset += 4;
            TASK_FMT_TRACE(Pipe, REC_LOG,"���ص�csd %d  oad %08x \n",i,csd[i].oad.value);
        }  
       
    }

    //������Ϣ [0] DAR
    if(DataBuf[Dataoffset++] == 0x0)
    {
        //printf("������Ϣ [0] DAR : %d  %d  %d\n",MtrIndex,pTmp_NormalLink->MsInfo[MtrIndex].CSDIndex,pTmp_NormalLink->MsInfo[MtrIndex].SigFrameNum);
        pAmrThreadArg->pMtrAmrInfo[MtrIndex].CSDIndex += pAmrThreadArg->pMtrAmrInfo[MtrIndex].SigFrameNum;
        return 0;
    }
    //MainOad = pTmp_NormalLink->CsdTask.RoadTask[RoadIndex].MainOad;
    RecordNum = DataBuf[Dataoffset++];
    TASK_FMT_TRACE(Pipe, REC_LOG,"mainoad %08x recordnum %d \n",MainOad.value,RecordNum);
    
    if(RecordNum == 0x0)
    {
        pAmrThreadArg->pMtrAmrInfo[MtrIndex].CSDIndex += pAmrThreadArg->pMtrAmrInfo[MtrIndex].SigFrameNum;
        if(ExistPosae == 1)
        {
            pAmrThreadArg->pMtrAmrInfo[MtrIndex].CurNum=1;
            pAmrThreadArg->pMtrAmrInfo[MtrIndex].EndNum=0;
        }
        return 0;
    }
    //�ն��᲻Ϊ�գ��ñ�־���ٲ���
    if(ExistPosae == 1)
    {
        pAmrThreadArg->pMtrAmrInfo[MtrIndex].EndNum=62;
    }

    for(i = 0; i < RecordNum; i++)//��¼��Ŀ
    {
        //����ÿ����¼������Ӧ��OAD����
        for(j = 0; j < CsdNum; j++)
        {
            if(csd[j].choice == 1)
            {
                MainOad = csd[j].road.oadMain;   
                Dataoffset =Dataoffset +2;
                for(k=0;k < csd[j].road.oadCols.nNum;k++)
                {
    //                OadTmp = csd[j].road.oadCols.oad[k];
    //                OadTmp.attPro = 0;
    //                TASK_FMT_TRACE( Pipe, REC_LOG, "csd[j].choice is %d\n",csd[j].choice);
    //                if(taskmng_oad_info_get(OadTmp,&pMap)==FALSE)
    //                {
    //                    pAmrThreadArg->pMtrAmrInfo[MtrIndex].CSDIndex += pAmrThreadArg->pMtrAmrInfo[MtrIndex].SigFrameNum;
    //                    return 0;
    //                }
                    if(pTmp_NormalLink->SchemeType ==EVENTPLAN && csd[j].road.oadCols.oad[k].value== 0x20220200)
                    {
                        if(DataBuf[Dataoffset]==DT_DOUBLE_LONG_UNSIGNED)
                            memcpy_r((uint8*)&EventSeq,&DataBuf[Dataoffset+1],4);
                        TASK_FMT_TRACE( Pipe, REC_LOG, "�¼���� %ld \n",EventSeq);
                        EventSeqReport = GetTaskReportSeq( Pipe, RecvRunTask->TaskId, RecvRunTask->logicid,MainOad, Pn);
                        TASK_FMT_TRACE( Pipe, REC_LOG, "���¼��ϴγ����ļ�¼���Ϊ%ld  \n",EventSeqReport );
                        if(EventSeq==EventSeqReport)
                        {
                            pAmrThreadArg->pMtrAmrInfo[MtrIndex].CSDIndex += pAmrThreadArg->pMtrAmrInfo[MtrIndex].SigFrameNum;
                            return 0;
                        }
                        else
                        {
                            SetTaskReportSeq( Pipe, RecvRunTask->TaskId,RecvRunTask->logicid, MainOad, Pn,EventSeq);
                        }
                    }
                    TASK_FMT_TRACE( Pipe, REC_LOG, "pTmp_NormalLink->StoreTimeType is %d\n",pTmp_NormalLink->StoreTimeType);
                    if(pTmp_NormalLink->StoreTimeType == STRTMTYP_FRZTIME)
                    {
                        //����record ��ʱ���һ��OIΪʱ��
                        if(csd[j].road.oadCols.oad[k].nObjID == 0x2021)
                        {
                            DateTimeHex_t        TmpDateTimeHex;
                            memset(&TmpDateTimeHex,0x0,sizeof(TmpDateTimeHex));
                            TASK_FMT_TRACE( Pipe, REC_LOG, "����ʱ��,Dataoffset is %d\n",Dataoffset);
                            TASK_BUF_TRACE( Pipe, REC_LOG, DataBuf+Dataoffset+1,6);
                            TmpDateTimeHex.year_h = DataBuf[Dataoffset+1];
                            TmpDateTimeHex.year_l = DataBuf[Dataoffset+2];
                            TmpDateTimeHex.month  = DataBuf[Dataoffset+3];
                            TmpDateTimeHex.day    = DataBuf[Dataoffset+4];
                            TmpDateTimeHex.hour   = DataBuf[Dataoffset+5];
                            TmpDateTimeHex.min    = DataBuf[Dataoffset+6];

                            if(DataBuf[Dataoffset+7] != 0xff)//������ظ�Ϊ0xff(������)
                                TmpDateTimeHex.sec    = DataBuf[Dataoffset+7];

                            DT_DateTimeHex2BCD(&TmpDateTimeHex,&StoreTm);

                        }
                    }
                    else
                    {
                        StoreTm = taskmng_scheme_task_store_time_get(Pipe,pTmp_NormalLink);
                        //�洢ʱ��ʱ����һ�γ��ض����¼������������ָ��¼�
                        if(pTmp_NormalLink->SchemeType == EVENTPLAN)
                            StoreTm.sec = i;
                    }
                    TASK_FMT_TRACE(Pipe, REC_LOG, "��%d����¼��%d�������� �洢ʱ�� \n",i,j);
                    WriteTimeLog(Pipe, (char *) &StoreTm);
                    if(DataLen<=Dataoffset)
                    {
                        pAmrThreadArg->pMtrAmrInfo[MtrIndex].CSDIndex += pAmrThreadArg->pMtrAmrInfo[MtrIndex].SigFrameNum;
                        return 1;
                    }
                    OadLen = OopTerminalAmrData(Pipe,MtrIndex,MainOad,csd[j].road.oadCols.oad[k],DataBuf+Dataoffset,(DataLen-Dataoffset),pTmp_NormalLink,RecvRunTask,&StoreTm,virtualpn);
                    Dataoffset += OadLen;
                    }
                }
                else
                {
                    //MainOad.value = 0;
                    if(csd[j].oad.value == 0x202A0200)
                    {
                        memcpy(pipeaddr,DataBuf+Dataoffset+3,6);
                        TASK_FMT_TRACE( Pipe, REC_LOG, "csd %ld  Dataoffset is %d\n",j,Dataoffset);
                        TASK_BUF_TRACE( Pipe, REC_LOG, DataBuf+Dataoffset,50);
                        TASK_BUF_TRACE( Pipe, REC_LOG, termianladdr,6);
                        TASK_BUF_TRACE( Pipe, REC_LOG, pipeaddr,6);
                        virtualpn = GetVirtualPipePn(pipeaddr,termianladdr);
                        if(virtualpn == 0xFFFF)
                        {
                            virtualpn = Pn;
                        }
                        TASK_FMT_TRACE( Pipe, REC_LOG, "virtualpn %ld  \n",virtualpn);
                    }

                    taskmng_oop_next_read_data_analysis(DataBuf,&Dataoffset,DataLen,DT_NULL,FALSE);
                    //OadLen = OopMeterAmrData(Pipe,MtrIndex,MainOad,csd[j].oad,DataBuf+Dataoffset,(DataLen-Dataoffset),pTmp_NormalLink,RecvRunTask,&StoreTm);
                    //Dataoffset += OadLen;
                }
        }
    }
        //�¼��ɼ����� �����Ҫ�ϱ� �����6012��ʽд�����������¼��ϱ�����
    if(pTmp_NormalLink->SchemeType ==EVENTPLAN && pTmp_NormalLink->rptFlag == TRUE )
    {
        TASK_FMT_TRACE( Pipe, REC_LOG, "���¼�������Ҫ�ϱ���\n");
        time_t StartTime;
        DateTimeBcd_t tmpTime;
        memcpy_r((uint8*)&tmpTime,(uint8*)RecvRunTask->ThisTurnBeginTime,7);
        DT_DateTimeBCD2Time(&tmpTime,(time_t*)&StartTime);

        Save698EventData(Pipe, DataBuf, DataLen, Pn, MainOad, StartTime);

    }
    if(DataBuf[Dataoffset++] ==1) //�и����ϱ���Ϣ��
    {
        OOPFollowReport(Pipe,Pn,&DataBuf[Dataoffset],DataLen-Dataoffset);
    }
    pAmrThreadArg->pMtrAmrInfo[MtrIndex].CSDIndex += pAmrThreadArg->pMtrAmrInfo[MtrIndex].SigFrameNum;

    return 1;

}


/*******************************************************************************
* ��������: taskmng_oop_perception_term_get_request
* ��������: ����������֡
* �������:
* �������:
* �� �� ֵ:
*******************************************************************************/
int taskmng_oop_perception_term_get_request(uint8 Pipe,uint8 seqindex, time_t StartTime,int MsIndex,OOP_OAD_U MainOad,uint8 SubOadNum,OOP_OAD_U *SubOad,uint16 *FrameLen, uint8 *FrameBuff)
{
    OOP_TSA_T                    TsaAddr;
    LinkLayer_t*            pLinkLayer = NULL;
    SCHEME_TASK_RUN_INFO    pTmp_NormalLink;
    uint16                  LastTaskId;
    uint8                   rsd[30];
    uint8                   rsdLen=0;
    OOP_OAD_U                    RsdOad;
    //uint16                  OadId;
    GetRecord_t             GetRecord;
    uint8                    InDataBuf[255];
    uint16                  offset = 0;
    uint16                  i = 0;
    //OadInfoMap              OadInfo;
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[Pipe];
    LastTaskId     = pAmrThreadArg->TmnAmrInfo.CurrentTaskId;//����ʱ�ķ�����
    AcqType_t  Acqtype =ACQTYP_NULL;
    taskmng_task_info_get(LastTaskId,pAmrThreadArg->TmnAmrInfo.CurrentLogicid, &pTmp_NormalLink);

    OOP_MSP_T    ms;
    OOP_OAD_U tempMainOad;
    uint16 tmpoffset;
    OOP_OAD_U                    tempRsdOad;
    OadInfoMap OadInfo;
    BOOL frezon =FALSE;

    tempMainOad.value = 0x60120300;
    ms.choice = 1;
    //��ȡ����������ַ
    TsaAddr = taskmng_tsa_get(pAmrThreadArg->pMtrAmrInfo[MsIndex].Pn,pAmrThreadArg->pMtrAmrInfo[MsIndex].logicid);
    //TASK_FMT_TRACE(Pipe,REC_LOG,"mainoad %08x    \n",MainOad.value);
     Acqtype = pTmp_NormalLink.AcqInfo.Acqtype;
    //������OAD��Ӧ����Ż�ȡRSD������������������OAD

    //�ز���645֡ʱ���ղ���������ͨ���� �Է��з�֡ʱ���ı�����
    if(ACQTYP_NULL == Acqtype || (TRUE == pAmrThreadArg->pMtrAmrInfo[MsIndex].IsNeedGetSafeMode))//ʵʱ���ݲ���GetRequestNormalList
    {
        offset = 0;
        for(i = 0; i < SubOadNum; i++)
        {
            InDataBuf[offset++] = (uint8)(SubOad[i].nObjID >> 8);
            InDataBuf[offset++] = (uint8)(SubOad[i].nObjID);
            InDataBuf[offset++] = SubOad[i].attID;
            InDataBuf[offset++] = SubOad[i].nIndex;
        }
        OopGet.requst(oop_pipe_get(Pipe,seqindex),&TsaAddr,2,InDataBuf,offset);
    }
    else//������ȡ����GetRequestRecord
    {
        if(ACQTYP_LASTN == Acqtype)
        {
            rsdLen = MakeRsd(Pipe,10,rsd,&pTmp_NormalLink,RsdOad,MsIndex,StartTime,&ms);
        }
        else
        {
            rsdLen = MakeRsd(Pipe,7,rsd,&pTmp_NormalLink,RsdOad,MsIndex,StartTime,&ms);
        }
        GetRecord.rsd = rsd;
        GetRecord.rsdLen = rsdLen;

        GetRecord.rcsd = InDataBuf;

        offset = 1;
        
        GetRecord.rcsd[offset++] = 0;
        GetRecord.rcsd[offset++] = 0x60;
        GetRecord.rcsd[offset++] = 0x40;
        GetRecord.rcsd[offset++] = 0x02;
        GetRecord.rcsd[offset++] = 0x00;

        GetRecord.rcsd[offset++] = 0;
        GetRecord.rcsd[offset++] = 0x60;
        GetRecord.rcsd[offset++] = 0x41;
        GetRecord.rcsd[offset++] = 0x02;
        GetRecord.rcsd[offset++] = 0x00;

        GetRecord.rcsd[offset++] = 0;
        GetRecord.rcsd[offset++] = 0x60;
        GetRecord.rcsd[offset++] = 0x42;
        GetRecord.rcsd[offset++] = 0x02;
        GetRecord.rcsd[offset++] = 0x00;

        GetRecord.rcsd[offset++] = 0;
        GetRecord.rcsd[offset++] = 0x20;
        GetRecord.rcsd[offset++] = 0x2A;
        GetRecord.rcsd[offset++] = 0x02;
        GetRecord.rcsd[offset++] = 0x00;
        
        if(MainOad.nObjID !=0x0000)
        {
            GetRecord.rcsd[offset++] = 1;
            GetRecord.rcsd[offset++] = (uint8)(MainOad.nObjID >> 8);
            GetRecord.rcsd[offset++] = (uint8)(MainOad.nObjID);
            GetRecord.rcsd[offset++] = MainOad.attID;
            GetRecord.rcsd[offset++] = MainOad.nIndex;
           if(taskmng_oad_info_get(MainOad,&OadInfo)==FALSE)
                return 0;
            if(OadInfo.class ==9)
            {
                tempRsdOad.value = 0x20210200;

            }
            else 
            {
                tempRsdOad.value = 0x20220200;
            }
        }

        tmpoffset = offset;
        GetRecord.rcsd[offset++] = SubOadNum;
        for(i = 0; i < SubOadNum; i++)
        {
            if(SubOad[i].nObjID == tempRsdOad.nObjID)
            {
                frezon =TRUE;
            }
        }
        if(frezon ==TRUE)
        {
            for(i = 0; i < SubOadNum; i++)
            {

                GetRecord.rcsd[offset++] = (uint8)(SubOad[i].nObjID >> 8);
                GetRecord.rcsd[offset++] = (uint8)(SubOad[i].nObjID);
                GetRecord.rcsd[offset++] = SubOad[i].attID;
                GetRecord.rcsd[offset++] = SubOad[i].nIndex;

            }
        }
        else
        {
            GetRecord.rcsd[tmpoffset] = SubOadNum+1;
            
            GetRecord.rcsd[offset++] = (uint8)(tempRsdOad.nObjID >> 8);
            GetRecord.rcsd[offset++] = (uint8)(tempRsdOad.nObjID);
            GetRecord.rcsd[offset++] = tempRsdOad.attID;
            GetRecord.rcsd[offset++] = tempRsdOad.nIndex;
            for(i = 0; i < SubOadNum; i++)
            {

                GetRecord.rcsd[offset++] = (uint8)(SubOad[i].nObjID >> 8);
                GetRecord.rcsd[offset++] = (uint8)(SubOad[i].nObjID);
                GetRecord.rcsd[offset++] = SubOad[i].attID;
                GetRecord.rcsd[offset++] = SubOad[i].nIndex;

            }
        }

        GetRecord.rcsd[0] = (tmpoffset-1)/5;
        GetRecord.rcsdLen = offset;
        GetRecord.oad = tempMainOad;

       
        OopGet.requst(oop_pipe_get(Pipe,seqindex),&TsaAddr,3,(uint8*)&GetRecord,sizeof(GetRecord_t));
    }
    
    pLinkLayer = FindLinkLayer(oop_pipe_get(Pipe,seqindex), NULL);

    if(pLinkLayer->sendLen>MAX_MSG_LEN)
        return 0;
    *FrameLen = pLinkLayer->sendLen;
    memcpy(FrameBuff,pLinkLayer->sendBuff,*FrameLen);

    return     *FrameLen;
}


/*******************************************************************************
* ��������: OopPatchGetRequst 
* ��������: //��698Э�鲹������
* �������: 

* �������:  
* �� �� ֵ:   
*******************************************************************************/
int OopTermianlPatchGetRequst(uint8 Pipe,uint8 seqindex, time_t StartTime,int MsIndex,OOP_OAD_U MainOad,uint8 SubOadNum,OOP_OAD_U *SubOad,uint16 *FrameLen, uint8 *FrameBuff,SCHEME_TASK_RUN_INFO *pTmp_NormalLink)
{
    OOP_TSA_T                    TsaAddr;
    LinkLayer_t*            pLinkLayer = NULL;
    uint8                   rsd[30];
    uint8                   rsdLen=0;
    OOP_OAD_U                    RsdOad;
    GetRecord_t             GetRecord;
    uint8                    InDataBuf[255];
    uint16                  offset = 0;
    uint16                  i = 0;
    OOP_OAD_U tempMainOad;
    uint16 tmpoffset;

    tempMainOad.value = 0x60120300;

    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[Pipe];

    TsaAddr = taskmng_tsa_get(pAmrThreadArg->PatchMtrInfo[MsIndex].Pn,pAmrThreadArg->PatchMtrInfo[MsIndex].logicid);
   
    rsdLen = MakeTerminalPatchRsdFrame(Pipe,rsd,pTmp_NormalLink,RsdOad,MsIndex,StartTime);
    GetRecord.rsd = rsd;
    GetRecord.rsdLen = rsdLen;

    GetRecord.rcsd = InDataBuf;

    offset = 1;
    
    GetRecord.rcsd[offset++] = 0;
    GetRecord.rcsd[offset++] = 0x60;
    GetRecord.rcsd[offset++] = 0x40;
    GetRecord.rcsd[offset++] = 0x02;
    GetRecord.rcsd[offset++] = 0x00;
    
    GetRecord.rcsd[offset++] = 0;
    GetRecord.rcsd[offset++] = 0x60;
    GetRecord.rcsd[offset++] = 0x41;
    GetRecord.rcsd[offset++] = 0x02;
    GetRecord.rcsd[offset++] = 0x00;
    
    GetRecord.rcsd[offset++] = 0;
    GetRecord.rcsd[offset++] = 0x60;
    GetRecord.rcsd[offset++] = 0x42;
    GetRecord.rcsd[offset++] = 0x02;
    GetRecord.rcsd[offset++] = 0x00;
    
    GetRecord.rcsd[offset++] = 0;
    GetRecord.rcsd[offset++] = 0x20;
    GetRecord.rcsd[offset++] = 0x2A;
    GetRecord.rcsd[offset++] = 0x02;
    GetRecord.rcsd[offset++] = 0x00;
    
    if(MainOad.nObjID !=0x0000)
    {
        GetRecord.rcsd[offset++] = 1;
        GetRecord.rcsd[offset++] = (uint8)(MainOad.nObjID >> 8);
        GetRecord.rcsd[offset++] = (uint8)(MainOad.nObjID);
        GetRecord.rcsd[offset++] = MainOad.attID;
        GetRecord.rcsd[offset++] = MainOad.nIndex;
    }
    
    tmpoffset = offset;
    GetRecord.rcsd[offset++] = SubOadNum;
    for(i = 0; i < SubOadNum; i++)
    {
    
        GetRecord.rcsd[offset++] = (uint8)(SubOad[i].nObjID >> 8);
        GetRecord.rcsd[offset++] = (uint8)(SubOad[i].nObjID);
        GetRecord.rcsd[offset++] = SubOad[i].attID;
        GetRecord.rcsd[offset++] = SubOad[i].nIndex;
    
    }
    
    GetRecord.rcsd[0] = (tmpoffset-1)/5;
    GetRecord.rcsdLen = offset;
    GetRecord.oad = tempMainOad;
    
    
    OopGet.requst(oop_pipe_get(Pipe,seqindex),&TsaAddr,3,(uint8*)&GetRecord,sizeof(GetRecord_t));

    pLinkLayer = FindLinkLayer(oop_pipe_get(Pipe, seqindex), NULL);
    *FrameLen = pLinkLayer->sendLen;
    memcpy(FrameBuff,pLinkLayer->sendBuff,*FrameLen);

    return     *FrameLen;
}


/*******************************************************************************
* ��������: OopPatchGetRequst 
* ��������: //��698Э�鲹������
* �������: 

* �������:  
* �� �� ֵ:   
*******************************************************************************/
int MakeTerminalPatchRsdFrame(uint8 Pipe,uint8 *RsdBuf,SCHEME_TASK_RUN_INFO *pTmp_NormalLink,OOP_OAD_U RsdOad,int MsIndex,time_t StartTime)
{
    uint8                   RsdLen = 0;
    //DateTimeBcd_t           TempTimeBcd;
    time_t                  TempTimeSec;
    OOP_TI_T                    DelayTi;

    RsdBuf[RsdLen++] = 7;    //ѡ�񷽷�2




    TempTimeSec = StartTime;
    if(pTmp_NormalLink->CsdTask.RoadTask[0].MainOad.nObjID < 0x5000 || pTmp_NormalLink->CsdTask.RoadTask[0].MainOad.nObjID > 0x5003)
        TempTimeSec = TempTimeSec+60;
    else
        TempTimeSec = StartTime;
    DT_Time2DateTimeHex(TempTimeSec, (DateTimeHex_t*)(RsdBuf+RsdLen));
    RsdLen += 7;

    if(pTmp_NormalLink->CsdTask.RoadTask[0].MainOad.nObjID == 0x5004)
        TempTimeSec = TempTimeSec+taskmng_task_period_get(pTmp_NormalLink->TaskId,pTmp_NormalLink->logicid);
    else
    {
        TempTimeSec = StartTime;
        DelayTi.value = pTmp_NormalLink->AcqInfo.TI.value;
        DelayTi.unit = pTmp_NormalLink->AcqInfo.TI.unit;
        DT_TimeAddInv(&TempTimeSec, DelayTi);//����һ��ʱ��������ֻ��1����
        TempTimeSec = TempTimeSec-1;  //���ֱ�ɸѡ������ǰ�պ�գ�-1��ǿ��Ϊǰ�պ�
    }
    DT_Time2DateTimeHex(TempTimeSec, (DateTimeHex_t*)(RsdBuf+RsdLen));
    RsdLen += 7;

    if(pTmp_NormalLink->AcqInfo.Acqtype == ACQTYP_FRZTM)
    {
        if(pTmp_NormalLink->CsdTask.RoadTask[0].MainOad.nObjID == 0x5004)
            pTmp_NormalLink->AcqInfo.TI.unit = TASK_CYCLE_FLAG_DAY;
        else if(pTmp_NormalLink->CsdTask.RoadTask[0].MainOad.nObjID == 0x5006)
            pTmp_NormalLink->AcqInfo.TI.unit = TASK_CYCLE_FLAG_MON;
        else if(pTmp_NormalLink->CsdTask.RoadTask[0].MainOad.nObjID== 0x5003)
            pTmp_NormalLink->AcqInfo.TI.unit = TASK_CYCLE_FLAG_HOUR;
        else if(pTmp_NormalLink->CsdTask.RoadTask[0].MainOad.nObjID == 0x5002)
            pTmp_NormalLink->AcqInfo.TI.unit = TASK_CYCLE_FLAG_MIN;
        pTmp_NormalLink->AcqInfo.TI.value = 0;
    }
    RsdBuf[RsdLen++] = pTmp_NormalLink->AcqInfo.TI.unit;
    RsdBuf[RsdLen++] = (uint8)(pTmp_NormalLink->AcqInfo.TI.value >> 8);
    RsdBuf[RsdLen++] = (uint8)pTmp_NormalLink->AcqInfo.TI.value;
    RsdBuf[RsdLen++] = 0x01;//MS����Ĭ��ȫ������ַ;
    return RsdLen;
}


/*******************************************************************************
* ��������: taskmng_perception_tmn_info_clear 
* ��������: //����ڴ��и�֪�ն���Ϣ
* �������: 
* �������: 
* �� �� ֵ: 
*******************************************************************************/
void taskmng_perception_tmn_info_clear()
{
    PerceptionTerminalInfoNum = 1;
    //pthread_mutex_lock(&perceptionterminal.lock);
    perceptionterminal.num = 0;
    memset((uint8*)&perceptionterminal,0x00,sizeof(OOP_PERCEPTION_TERMINAL_LIST_T));
    //pthread_mutex_unlock(&perceptionterminal.lock);
}


#endif


