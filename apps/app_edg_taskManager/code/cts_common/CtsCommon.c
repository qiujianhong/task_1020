

#include "CtsAll.h"
#include "devinfo.h"
#include "area.h"
#include "CtsACMeter.h"
#include "CtsPlc.h"


PastTimeFlag_T             gPastTimeFlag[AMR_THREAD_NUM];       //��ʱ���־ ���һ������������߳���
DateTime_T                 gCurBinTime[AMR_THREAD_NUM];         //��ǰBINʱ��
DateTime_T                 gCurBcdTime[AMR_THREAD_NUM];         //��ǰBCDʱ��
DateTime_T                 gPreBinTime[AMR_THREAD_NUM];         //�ϴ�BINʱ��
uint8                   gPreDI[MTR_MAX_NUM][4];          //�ϴ����ݱ�ʶ

PastTimeFlag_T             gPastTimeFlagTask;                           //��ʱ���־
DateTime_T                 gCurBinTimeTask;                             //��ǰBINʱ��
DateTime_T                 gCurBcdTimeTask;                             //��ǰBCDʱ��
DateTime_T                 gPreBinTimeTask;                             //�ϴ�BINʱ��
uint16                          MsgSeq = 0;      //��Ϣindex ���̹߳���
//MODULE_INFO_T              gModuleInfo[5];      //5��ģ�����Ϣ

#define MONITOR_PARALL_FUN  1      //����¼��ϱ���ʱ��͸����������Ƿ��ò�������ķ�ʽ   1��ʾ��003E�������� 0��ʾ������0038͸��


extern RS485_RS232_PARA Rs485ComNo[8];


uint8 gHPLCRouterState[PLC_NUM] = {0};  //·����ɱ�ʶ 0��ʾδ��� ���ܳ��� 1��ʾ·��ѧϰ��� ���Գ���
uint8 gHPLCSearchState[PLC_NUM] = {0};  //�ѱ���ɱ�ʶ 0��ʾδ��� ���ܳ��� 1��ʾ�ѱ���ɣ����ڽ���


uint8 HPLCChannel;          //����ز�Ƶ�����F2090900

/*̨��ʶ����ͣ���������־��F2090B00
bit0��̨��ʶ��ʹ�ܱ�־��0����ֹ��1������
bit1��̨������ʹ�ܱ�־��0����ֹ��1������
bit2��������֪ʹ�ܱ�־��0����ֹ��1������
bit3~bit7�����á�
*/
uint8 HPLCAcrossAreaFlag;
uint8 HPLCNodeRejectFlag;   //�ܾ��ڵ��ϱ�ʹ�ܱ�־F2091600

extern pthread_mutex_t         SeqLock;

extern uint8 set_len_offset(uint16 eleLen, uint8 *indata);

extern uint8 DataInitFlagMain;
extern uint16 get_len_offset(uint8 *indata,uint8 *bytes);
extern uint8 Rs485PipeConut;
#if (defined AREA_JIANGXI) || (defined AREA_ANHUI)||(defined AREA_JIANGSU)
extern OOP_PERCEPTION_TERMINAL_LIST_T perceptionterminal;
#endif
extern uint8 gReponsePIIDCAD;
extern time_t GetCrvStartTime(time_t OriStartTime, uint16 TaskId, uint16 MtrNo, uint8 PLC485Flag);
extern BOOL JudgeNeedPatch(uint16 TaskId, uint16 MtrNo,uint32 *starttime);


/*******************************************************************************
* ��������: GetMsgSeq
* ��������: ��ȡ��Ϣ���
* �������:
* �������:
* �� �� ֵ: ��Ϣ���
*******************************************************************************/
uint16 GetMsgSeq()
{
    uint16 seq =0 ;
    pthread_mutex_lock(&SeqLock);
    seq = MsgSeq++;
    pthread_mutex_unlock(&SeqLock);
    return seq;
}

// uint16 MakeRelay485Data_0010(uint8 pipe,DI_INFO_T DiInfo,uint8 prtl,uint8* DataBuff);
// int GetNext485RelayDI(uint8 pipe,DI_INFO_T* DiInfo,uint8* prtl);
 uint8 Get485OopRelayOad(uint8 pipe,uint8 MtrIndex,DI_INFO_T* DiInfo);

void WriteLoadRate(DB_CLIENT fd)
{

    OOP_INT4V_T  INT4V_T;
    uint32 Len;
    int ret;
    uint16 loadrate=0;
    OOP_OAD_U oad;
    int32 value=240;
    int32 tmpvalue=0;

    NOMAL_OAD_T NormalOad;
    NormalOad.logicId = 1;
    NormalOad.infoNum = 0;

    oad.value=0x40290200;
    NormalOad.oad = oad;
    ret = db_read_nomal(fd,&NormalOad,MAX_MSG_LEN,(uint8*)&value,&Len);
    if(ret!=0)
    {
        value =240;
    }
    if(value ==0)
    {
        value =240;
    }
    oad.value=0x20260200;
    NormalOad.oad = oad;
    memset((uint8*)&INT4V_T,0x00,sizeof(INT4V_T));
    ret = db_read_nomal(fd,&NormalOad,MAX_MSG_LEN,(uint8*)&INT4V_T,&Len);
    if(ret==0)
    {
        tmpvalue = INT4V_T.nValue[0]/10000;
        loadrate = tmpvalue/value;
    }
    oad.value=0x20280200;
    WRITE_RECORD_T Data;
    uint8 tmpaddr[6];
    uint16 pn=0;
    uint32  storeNo;
    memset(&Data,0x00,sizeof(WRITE_RECORD_T));
    Data.recordOAD.optional =1;
    Data.recordOAD.logicId = 0;

    Data.recordOAD.road.oadMain.value = 0;
    Data.recordOAD.road.oadCols.nNum =1;
    Data.recordOAD.road.oadCols.oad[0] = oad;
    Data.recordOAD.classtag = 5;

    //�洢ʱ��ŵ����洦��
    Data.colStoreTm = (time(NULL)/900)*900+ 8*60*60;
#if IS_NEED_ADD_8HOUR
    Data.colStartTm = time(NULL) + 8*60*60;
    //Data.colStoreTm = StoreTime +8*60*60 ;
    Data.colEndTm = time(NULL) + 8*60*60;
#else
    Data.colEndTm = time(NULL);
#endif

    Data.MAC.nNum = 6;
    //memcpy(Data.MAC.value,LocalAddr,6);
    taskmng_mtr_addr_pn_get_by_userType(0x06,LOGIC_ID_YC,tmpaddr,&pn);
    Data.recordOAD.infoNum = pn;
    memcpy_r(Data.MAC.value,tmpaddr,6);
    //taskmng_mtr_addr_get(TGThreadArg.Pn,Data.MAC.value);
    Data.inDatalen = 2;
    memcpy(Data.inDataBuf,(uint8*)&loadrate,2);

    ret = db_write_record(fd, &Data, &storeNo);
    if(ret!=0)
    {
        TASK_FMT_TRACE( -1,TG_LOG," �洢�ܼ�������ʧ�� \n");
    }
    return;

}


/*******************************************************************************
* ��������: taskmng_time_manage_task
* ��������: ��������̵߳�ʱ�������
* �������:
* �������:
* �� �� ֵ:
*******************************************************************************/
void taskmng_time_manage_task()
{
    gPastTimeFlagTask.Word = 0;                           //���ʱ���־

    taskmng_cur_bin_time_get(&gCurBinTimeTask);                //ˢ�µ�ǰHEX��ʱ��
    taskmng_cur_bcd_time_get(&gCurBcdTimeTask);              //ˢ�µ�ǰBCD��ʱ��


    //����
    if (gCurBinTimeTask.Month != gPreBinTimeTask.Month)
    {
        //ͬʱ�������¡����ա���ʱ�����֡������־
        gPastTimeFlagTask.BitSect.Month = TRUE;
        gPastTimeFlagTask.BitSect.Day = TRUE;
        gPastTimeFlagTask.BitSect.Hour = TRUE;
        gPastTimeFlagTask.BitSect.Minute = TRUE;
        gPastTimeFlagTask.BitSect.Second = TRUE;
    }
    //����
    if (gCurBinTimeTask.Day != gPreBinTimeTask.Day)
    {
        //ͬʱ�������ա���ʱ�����ֱ�־
        gPastTimeFlagTask.BitSect.Day = TRUE;
        gPastTimeFlagTask.BitSect.Hour = TRUE;
        gPastTimeFlagTask.BitSect.Minute = TRUE;
        gPastTimeFlagTask.BitSect.Second = TRUE;
    }
    //��ʱ
    else if (gCurBinTimeTask.Hour != gPreBinTimeTask.Hour)
    {
        //ͬʱ������ʱ�����ֱ�־
        gPastTimeFlagTask.BitSect.Hour = TRUE;
        gPastTimeFlagTask.BitSect.Minute = TRUE;
        gPastTimeFlagTask.BitSect.Second = TRUE;
    }
    //����
    else if (gCurBinTimeTask.Minute != gPreBinTimeTask.Minute)
    {
        //�������ֱ�־
        gPastTimeFlagTask.BitSect.Minute = TRUE;
        gPastTimeFlagTask.BitSect.Second = TRUE;
        #ifdef AREA_JIANGXI
        if(gCurBinTimeTask.Minute%15==0)
        {
            WriteLoadRate(gDBClientTask);
        }
        #endif
    }

    //����
    else if (gCurBinTimeTask.Second != gPreBinTimeTask.Second)
    {
        gPastTimeFlagTask.BitSect.Second = TRUE;
    }
    else
    {
        usleep(10000);
    }


    //���մ���
    if(gPastTimeFlagTask.BitSect.Day == TRUE)
    {

    }

    //��ʱ����
    if(gPastTimeFlagTask.BitSect.Hour == TRUE)
    {
    }

    if(gPastTimeFlagTask.BitSect.Second == TRUE)
    {


    }

    //���ִ���
    if(gPastTimeFlagTask.BitSect.Minute == TRUE)
    {

    }

    memcpy(&gPreBinTimeTask, &gCurBinTimeTask, sizeof(DateTime_T));;

}


/*******************************************************************************
* ��������: GetMtrPipe
* ��������: ���ݵ��oadת��Ϊ�˿ں� 0 1��Ӧ�ز� 2��9��Ӧ8·485
* �������:
            oad          //�����˿ڲ���


* �������: ��Ӧ�ڲ��˿�
* �� �� ֵ: �����������
*******************************************************************************/
uint8 taskmng_mtr_pipe_get(OOP_OAD_U *oad)
{
    if(oad->nObjID == 0xF209)
    {
        if(oad->nIndex == 0x01)
            return 0;
    }
    else if(oad->nObjID == 0xF201)
    {
        if(oad->nIndex == 0x01)
            return PLC_NUM;
        else if(oad->nIndex == 0x02)
            return PLC_NUM+1;
#ifdef PRODUCT_ECU
        else if(oad->nIndex == 0x03)
            return PLC_NUM+2;
        else if(oad->nIndex == 0x04)
            return PLC_NUM+3;
        else if(oad->nIndex == 0x05)
            return PLC_NUM+4;
        else if(oad->nIndex == 0x06)
            return PLC_NUM+5;
#endif
    }
#ifndef AREA_FUJIAN
    else if(oad->nObjID == 0xF208 && oad->attID == 0x02) //���ɼ����ݴ� 0200Ҳ֧��
    {
        return AMR_THREAD_NUM; //�����ñ�ʾ
    }
#endif
    else if(oad->value == 0xF20B0201)
    {
        return 1; //������1��ʾ
    }else if(oad->nObjID == 0xF221)
    {
#ifdef PRODUCT_ECU
        if(oad->nIndex == 0x01)
            return PLC_NUM+MAX_485_NUM-4;
        else if(oad->nIndex == 0x02)
            return PLC_NUM+MAX_485_NUM-3;
#else
        if(oad->nIndex == 0x01)
            return PLC_NUM+MAX_485_NUM-2;
        else if(oad->nIndex == 0x02)
            return PLC_NUM+MAX_485_NUM-1;
#endif
    }
#ifdef PRODUCT_ECU
    else if(oad->nObjID == 0xF220)
    {
        if(oad->nIndex == 0x01)
            return PLC_NUM+MAX_485_NUM-2;
        else if(oad->nIndex == 0x02)
            return PLC_NUM+MAX_485_NUM-1;
    }

#endif
#ifdef AREA_FUJIAN
    else if(oad->nObjID == 0xF2E1)
    {
        if(oad->nIndex == 0x01)
            return AMR_THREAD_NUM-2;
    }
    else if((oad->nObjID == 0xF2E0 || oad->nObjID == 0xF208) && oad->attID == 0x02) //���ɼ����ݴ� 0200Ҳ֧��
    {
        return AMR_THREAD_NUM-1; //������10��ʾ
    }

#endif
    return 0xff;
}


/*******************************************************************************
* ��������: taskmng_pipe_oad_get
* ��������: �������ڲ�ʹ�õĶ˿ں�ת��Ϊ698�˿ں�
* �������: pipe �����ڲ�ʹ�õĶ˿ں�
* �������:

* �� �� ֵ: 698�˿ں�
*******************************************************************************/
OOP_OAD_U  taskmng_pipe_oad_get(uint8 pipe)
{
    OOP_OAD_U oad;
    memset(&oad,0x00,sizeof(OOP_OAD_U));
    switch(pipe)
    {
        case 0:
            oad.value = 0xF2090201;
            break;
        case 1:
            oad.value = 0xF20B0201;
            break;
        case 2:
            oad.value = 0xF2010201;
            break;
        case 3:
            oad.value = 0xF2010202;
            break;
#ifdef PRODUCT_ECU
        case 4:
            oad.value = 0xF2010203;
            break;
        case 5:
            oad.value = 0xF2010204;
            break;
        case 6:
            oad.value = 0xF2010205;
            break;
        case 7:
            oad.value = 0xF2010206;
            break;
        case 8:
            oad.value = 0xF2210201;
            break;
        case 9:
            oad.value = 0xF2210202;
            break;
        case 10:
            oad.value = 0xF2200201;
            break;
        case 11:
            oad.value = 0xF2200202;
            break;

#else
        case 4:
            oad.value = 0xF2210201;
            break;
        case 5:
            oad.value = 0xF2210202;
            break;

#endif


#ifdef AREA_FUJIAN
        case (AMR_THREAD_NUM-2):
             oad.value = 0xF2E10201;
             break;
        case (AMR_THREAD_NUM-1):
             oad.value = 0xF2E00200;

#endif
        default:
            break;
    }
    return oad;
}



/*******************************************************************************
* ��������: �ж�Addr1�Ƿ��Addr2ƥ��
* �������:       Addr1          //���ܴ�ͨ����ĵ�ַ
            Addr2             //�����еĵ���ַ
* �������: ��
* �� �� ֵ: TRUE          //ƥ��
         FALSE        //��ƥ��
*******************************************************************************/
BOOL IsMtrAddrMatch(OOP_TSA_T *Addr1,OOP_TSA_T *Addr2)
{
    uint8 i;
    char str1[3]={0};
    char str2[3]={0};
    if(Addr1->len != Addr2->len)
        return FALSE;
    //����߼���ַ����ͬ ֱ�Ӳ�ƥ��
    if(Addr1->oldvxd!=Addr2->oldvxd || Addr1->vxd != Addr2->vxd)
        return FALSE;
    //�Ա����߼���ַ
    if(memcmp(Addr1->add,Addr2->add,Addr2->len+1)==0 && Addr1->oldvxd == Addr2->oldvxd &&
            Addr1->vxd == Addr2->vxd)
        return TRUE;

    for(i=0;i<Addr2->len+1;i++)
    {
        if(Addr1->add[i]!=Addr2->add[i])
        {
            sprintf(str1,"%02x",Addr1->add[i]);
            sprintf(str2,"%02x",Addr2->add[i]);
            if(str1[0]=='a'||str1[0]=='A')
            {
                str1[0]=str2[0];
            }
            if(str1[1]=='a'||str1[1]=='A')
            {
                str1[1]=str2[1];
            }
            if(memcmp(str1,str2,2)!=0)
                return FALSE;
        }
    }
    return TRUE;
}



/*******************************************************************************
* ��������: �ж������ĵ���Ƿ���ڵ�ǰ�б���
* �������: MetSn                   //������
            MsNum                   //���б���Ŀ
            pMsInfo                 //���������Ϣ
* �������: ��
* �� �� ֵ: 0      //�ظ�
            1      //���ظ�
*******************************************************************************/
int IsMtrSnInList(uint16 MtrSn,uint16 MsNum,MS_INFO *pMsInfo)
{
    uint16 i;

    for(i = 0; i < MsNum; i++)
    {
        if(MtrSn == pMsInfo[i].Sn)//�ж����к��Ƿ�һ��
            return 1;
    }

    return 0;
}
/*******************************************************************************
* ��������: ��TSA�ĵ�ַ���мӼ�����
* �������: TsaAddr                   //TSA��ַ
            Fun                       //����
* �������: ��
* �� �� ֵ: 0      //�ظ�
            1      //���ظ�
*******************************************************************************/
void TsaCalcfun(OOP_TSA_T *TsaAddr,int Fun)
{
    uint8   len = 0;
    char    TmpAddr[6] = {0};
    int64  tmpaddr = 0;

    len = TsaAddr->len+1;
    if(len>6)
        return;
    memcpy_r((uint8*)TmpAddr, TsaAddr->add, len);
    memcpy(&tmpaddr,(uint8*)TmpAddr,len);
    if(Fun == 1)
        tmpaddr +=1;
    if(Fun==-1)
    {
        if(tmpaddr>0)
            tmpaddr -= 1 ;
    }
    memcpy_r( TsaAddr->add, (uint8 *)&tmpaddr, len);
}







/**********************************************************************
���� GetTaskDelay
���� ��ȡ������ʱ û�м���
����  ��
����ֵ:������ʱ ��λ��
***********************************************************************/
uint32 GetTaskDelay(uint16 TaskId,uint8 logicid)
{
    uint32         dwPeriod;
    int            i;

    //��ȡ����ִ�����ڣ������ն���ɹ��ʼ�������ִ��ʱ��
    for(i=0; i<SchemeTaskCfgList.TaskNum; i++)
    {
        if(SchemeTaskCfgList.OopTaskCfg[i].TaskId == TaskId &&SchemeTaskCfgList.OopTaskCfg[i].logicid == logicid )
            break;
    }
    if(i == SchemeTaskCfgList.TaskNum)
    {
        return 0;   //�ݴ�
    }

    dwPeriod = SchemeTaskCfgList.OopTaskCfg[i].DelayTime;
    switch (SchemeTaskCfgList.OopTaskCfg[i].DelayTimeUnit)  //��ʱ��λ
    {
    case TASK_CYCLE_FLAG_SEC:       //��
        break;

    case TASK_CYCLE_FLAG_MIN:       //����
        dwPeriod *= 60L;
        break;

    case TASK_CYCLE_FLAG_HOUR:      //Сʱ
        dwPeriod *= 3600L;
        break;

    case TASK_CYCLE_FLAG_DAY:       //��
    case TASK_CYCLE_FLAG_MON:       //��
    case TASK_CYCLE_FLAG_YEAR:      //��
        dwPeriod = 86400L;
        break;

    default:
        return 0;
        break;
    }
    return dwPeriod;
}


/*******************************************************************************
* ��������: taskmng_past_time_deal_init
* ��������: ʱ��������ʼ��
* �������: ��
* �������: ��
* �� �� ֵ: ��
*******************************************************************************/
void taskmng_past_time_deal_init ( uint8 CommPipe )
{
    taskmng_cur_bin_time_get ( &gCurBinTime[CommPipe] );
    memcpy ( &gPreBinTime[CommPipe], &gCurBinTime[CommPipe], sizeof ( DateTime_T ) );
}
/*******************************************************************************
* ��������: taskmng_all_state_init
* ��������: ״̬�ֳ�ʼ��
* �������: ��
* �������: ��
* �� �� ֵ: ��
*******************************************************************************/
void taskmng_all_state_init ( uint8 CommPipe )
{
    AmrThreadArg_T* pAmrThreadArg;

    pAmrThreadArg = &gAmrThreadArg[CommPipe];

    memset ( &pAmrThreadArg->TotalState, 0x00, sizeof ( TOTAL_STATE_T ) );
    memset(&pAmrThreadArg->MeterSearch,0x00,sizeof(SEARCH_METER_T));
    memset(&pAmrThreadArg->RelayTrans,0x00,sizeof(RELAY_TRANS_T));
    memset(&pAmrThreadArg->EventReport,0x00,sizeof(EVENT_REPORT_T));
    pAmrThreadArg->RelayTrans.isValid=0;
    pAmrThreadArg->Isadjusttime = 0;
    pAmrThreadArg->pNeedAddMetHead=NULL;
    pAmrThreadArg->pNeedDeleteMetHead=NULL;
    pAmrThreadArg->TaskModifyFlag = TASK_CFG_UNINIT;
    pAmrThreadArg->MsgSock = task_msg_init(IPC_AMR+CommPipe);
    pAmrThreadArg->IsReportHPLCInfo = 0;
    pAmrThreadArg->HPLCInfoSendTime = 0;
    pAmrThreadArg->pReNetworkingAddHead = NULL;
    pAmrThreadArg->pReNetworkingDelHead = NULL;
    pAmrThreadArg->SendQueueFlag = 0;
    pAmrThreadArg->meterTimeManager = NULL;
    pAmrThreadArg->meterTimeDelay = NULL;
    task_msg_settime(pAmrThreadArg->MsgSock,0,1000);
    pAmrThreadArg->DBClientHandle = db_client_create(DB_AGENT_CTS,(DB_CLIENT_AMR+CommPipe));
    /* �������÷����� cpuռ����̫�� */
    /* task_msg_nonblocking_set(pAmrThreadArg->MsgSock); */

}

/*******************************************************************************
* ��������: taskmng_plc_module_check
* ��������: ��ѯ��Ӧ�˿��Ƿ�����ز�ģ��
* �������:
* �������: ��
* �� �� ֵ:  ����0 Ϊ��ģ�飬 -1Ϊû��
*******************************************************************************/
int taskmng_plc_module_check(uint8 pipe)
{

        return 0;

}


/*********************************
�������ܣ�    ȥ���ظ�������
����ԭ�ͣ�    Remove_RepeatPlcTask
����������
              TaskName            //���ӵ���������
��������ֵ��
*********************************/
int8 Remove_RepeatPlcTask ( uint8 CommPipe,MAIN_CYC_STATE_T TaskName )
{
    PLCTASK_QUEUE* PlcTask_RunLink = NULL;
    PLCTASK_QUEUE* pTmp = NULL;

    PlcTask_RunLink = gAmrThreadArg[CommPipe].PlcTask_RunLink;
    pTmp = PlcTask_RunLink;

    if ( PlcTask_RunLink == NULL )
    {
        return 0;
    }

    if ( TaskName == MS_RELAY_TRANS )
    {
        return 0;
    }
    //TASK_FMT_TRACE ( CommPipe,COMM_LOG, "��ǰ�������� %d PlcTaskDeal_Stat %d �������� %d \n",PlcTask_RunLink->TaskName,PlcTask_RunLink->PlcTaskDeal_Stat,TaskName );
    if ( PlcTask_RunLink->TaskName == TaskName ) //����˴������������뵱ǰ����ִ�е�����һ��
    {
        switch ( PlcTask_RunLink->PlcTaskDeal_Stat )
        {
            case TASK_INIT:
            case ROUTERCTRL_DEAL:
            case ROUTERCTRL_DELAY:
                return -1;
                break;
            case FUN_DEAL:
                if ( TaskName == MS_FILE_CMP||TaskName == MS_NORMAL_AMR ) //������ճ�����
                {
                    if ( PlcTask_RunLink->gInnerStat != NULL )
                    {
                        PlcTask_RunLink->gInnerStat = 0;//��ͷִ��
                    }

                    if ( TaskName == MS_NORMAL_AMR ) //������ճ�����������Ҫ����ִ��·�ɲ���
                    {
                        //PlcTask_RunLink->PlcTaskDeal_Stat = TASK_INIT;//����ִ��
                    }
                }
                else//����������Ҫ��������
                {
                    return -1;
                }
                break;

            case ROUTERRECOVER_DELAY:
            case ROUTERRECOVER_DEAL:
                PlcTask_RunLink->PlcTaskDeal_Stat = FUN_DEAL;//����ִ��
                break;
        }

        return -1;
    }

    while ( pTmp != NULL )
    {
        if ( pTmp->TaskName == TaskName )
        {
            if ( TaskName == MS_NORMAL_AMR )
            {
                if ( pTmp->RoutrtCtrl == RCS_RESTART ) //�����ʱ�в����ճ�����������ô·����Ҫ�ٴ�����
                {
                    pTmp->PlcTaskDeal_Stat = TASK_INIT;
                }
            }
            return -1;
        }
        pTmp = pTmp->next;
    }

    return 0;
}
/*********************************
�������ܣ�      ���ȼ������Ƚ����̲��뵽���������㷨
����ԭ�ͣ�      InsertPlcTaskQueue
����������      PLCTASK_QUEUE *PlcAddTask         //��Ҫ������ز�����
��������ֵ��
*********************************/
void InsertPlcTaskQueue ( uint8 CommPipe,PLCTASK_QUEUE* PlcAddTask )
{
    PLCTASK_QUEUE*  pTmpLink;
    PLCTASK_QUEUE*  PlcTask_RunLink = NULL;

    PlcTask_RunLink = gAmrThreadArg[CommPipe].PlcTask_RunLink;

    if ( PlcTask_RunLink == NULL ) //�����ǰ�������Ϊ��
    {
        PlcTask_RunLink = PlcAddTask;
        PlcAddTask->next = NULL;
        gAmrThreadArg[CommPipe].PlcTask_RunLink = PlcTask_RunLink;
        return ; //��ӳɹ�
    }

    if ( PlcAddTask->Priority == PlcTask_RunLink->Priority ) //����������ȼ���ͬ��ôֱ�����ڵ�ǰ������к�
    {

        pTmpLink              = PlcTask_RunLink->next;
        PlcTask_RunLink->next = PlcAddTask;
        PlcAddTask->next      = pTmpLink;
        gAmrThreadArg[CommPipe].PlcTask_RunLink = PlcTask_RunLink;
        return;
    }
    else if ( PlcAddTask->Priority < PlcTask_RunLink->Priority ) //���������ȼ����ڵ�ǰ��������ȼ�
    {

        if ( PlcTask_RunLink->PlcTaskDeal_Stat == ROUTERRECOVER_DELAY
                ||PlcTask_RunLink->PlcTaskDeal_Stat == ROUTERRECOVER_DEAL ) //�����ǰ�����ڻָ����߻ָ�ִ�й���ʱ
        {
            PlcTask_RunLink->next = SortPlcTaskQueue ( PlcTask_RunLink->next,PlcAddTask );
            gAmrThreadArg[CommPipe].PlcTask_RunLink = PlcTask_RunLink;
            return;
        }
        else
        {
            if ( PlcTask_RunLink->PlcTaskDeal_Stat == FUN_DEAL
                    || PlcTask_RunLink->PlcTaskDeal_Stat == ROUTERCTRL_DELAY ) //�����ǰ�Ǵ��ڹ��ܺ���ִ��
            {

                if ( PlcTask_RunLink->RoutrtCtrl == PlcAddTask->RoutrtCtrl )
                {
                    PlcAddTask->PlcTaskDeal_Stat = ROUTERCTRL_DELAY;//COMM_DEBUGֱ�ӽ�����ʱ���������ٽ���·�ɲ���
                    if ( PlcAddTask->DelaySec <= 5 )
                    {
                        gAmrThreadArg[CommPipe].TaskRunDelaySec = PlcAddTask->DelaySec;
                    }
                    else
                    {
                        gAmrThreadArg[CommPipe].TaskRunDelaySec = GetEnvValue ( "PLC_DELAY", PlcAddTask->DelaySec );
                    }
                }

            }
            //��λ��ǰ����,����ִ��

            RestPlcTaskStat ( PlcTask_RunLink );

            PlcAddTask->next = PlcTask_RunLink;
            PlcTask_RunLink = PlcAddTask;
            gAmrThreadArg[CommPipe].PlcTask_RunLink = PlcTask_RunLink;
            return;
        }
    }

    PlcTask_RunLink = SortPlcTaskQueue ( PlcTask_RunLink,PlcAddTask );
    gAmrThreadArg[CommPipe].PlcTask_RunLink = PlcTask_RunLink;
}


/*********************************
�������ܣ�      �ز��������ȼ�����
����ԭ�ͣ�      SortPlcTaskQueue
����������
                PLCTASK_QUEUE *pHeadTaskLink      //���������ͷ����
                PLCTASK_QUEUE *PlcAddTask         //��Ҫ������ز�����

��������ֵ��
*********************************/
PLCTASK_QUEUE* SortPlcTaskQueue ( PLCTASK_QUEUE* pHeadTaskLink,PLCTASK_QUEUE* PlcAddTask )
{
    PLCTASK_QUEUE* PTmpHeadTaskLink= pHeadTaskLink;//��ѯ����ʱʹ��
    PLCTASK_QUEUE* ScollLink = NULL;//��ѯ����ʱʹ��

    if ( PTmpHeadTaskLink == NULL )
    {
        PlcAddTask->next = NULL;
        return PlcAddTask;
    }

    if ( PlcAddTask->Priority < PTmpHeadTaskLink->Priority )
    {
        PlcAddTask->next = PTmpHeadTaskLink;
        return PlcAddTask;
    }


    while ( PTmpHeadTaskLink != NULL )
    {
        if ( PlcAddTask->Priority >= PTmpHeadTaskLink->Priority )
        {
            ScollLink = PTmpHeadTaskLink;
            PTmpHeadTaskLink = PTmpHeadTaskLink->next;
        }
        else//���ȼ��ȴ��������ȼ���
        {
            break;
        }

    }


    PlcAddTask->next = PTmpHeadTaskLink;
    ScollLink->next = PlcAddTask;

    return pHeadTaskLink;
}

/*******************************************************************************
* ��������:         FindNextTaskPrio
* ��������:         ��ȡ������ͬ���ȼ������һ������
* �������:         PLCTASK_QUEUE *PlcTaskQueue //�������
                    QueueNum                    //������г���
* �������:         int
* �� �� ֵ:         > 0  ��ӳɹ�
                    < 0  ���ʧ��

*******************************************************************************/
void RestPlcTaskStat ( PLCTASK_QUEUE* PlcCurTask )
{

    if ( ( PlcCurTask->gInnerStat ) != NULL )
    {
        * ( uint8* ) ( PlcCurTask->gInnerStat )  = 0;
    }

    if ( PlcCurTask->RoutrtCtrl != RCS_RESTART )
    {
        PlcCurTask->PlcTaskDeal_Stat = TASK_INIT;    //����ִ��
    }

}

/*******************************************************************************
* ��������: taskmng_frame_3762_unpack
* ��������: ��3762֡
* �������: FrameLen            ֡����
*           FrameBuff           ֡����
* �������: AppData             Ӧ�ò�����
* �� �� ֵ: CHECK_RESULT_SUCCEED
*           CHECK_RESULT_FAILUER
*           CHECK_RESULT_CONFIRM
*           CHECK_RESULT_DENY
*******************************************************************************/
int8 taskmng_frame_3762_unpack(uint16 FrameLen, uint8 *FrameBuff, AppData_T *AppData)
{

    uint16  FrameDataLen=0;                     //֡�����ܳ���
    uint16  DataIndex = 0;                      //ָ��ƫ��
    int     i = 0;
    uint8   CheckSum = 0;  //У���

    //����֡���ݳ���, ƫ�Ƶ���Ϣ��R
    memcpy(&FrameDataLen, &FrameBuff[1], 2);
    DataIndex = 4;

    //�жϱ��ĳ����Ƿ�Ϸ�
    if(FrameDataLen > FrameLen)
    {

        return CHECK_RESULT_FAILUER;
    }


    //������Ϣ��, ƫ�Ƶ���ַ��
    memcpy(AppData->RInfo, &FrameBuff[DataIndex], 6);
    DataIndex += 6;

    //�ж�ͨѶģ���ʶ��ƫ�Ƶ�Ӧ��������
    if (AppData->RInfo[0]& 0x04)  //���ڵ�ַ��
    {
        //ƫ�Ƶ�A1��A3���ӽڵ�������A2
        DataIndex += 12;
    }


    AppData->AFN = FrameBuff[DataIndex];
    for (i = 0; i < 8; i++)
    {
        if (FrameBuff[DataIndex+1] & (0x01 << i))
        {
            AppData->Fn = FrameBuff[DataIndex+2] * 8 + (i + 1);
            break;
        }
    }
    DataIndex += 3;

    if(FrameDataLen < (DataIndex + 2))
    {
        return CHECK_RESULT_FAILUER;
    }
    AppData->DataUnitLen = FrameDataLen - (DataIndex + 2);


    if(AppData->DataUnitLen > sizeof(AppData->DataUnitBuff))
    {
        return CHECK_RESULT_FAILUER;
    }
    memcpy(AppData->DataUnitBuff, &FrameBuff[DataIndex], AppData->DataUnitLen);


    //У��ͼ��

    for(i=3; i<FrameDataLen-2; i++)
    {
        CheckSum += FrameBuff[i];
    }

    if(CheckSum != FrameBuff[FrameDataLen-2])
    {
        return CHECK_RESULT_FAILUER;
    }

    //֡β���
    if(FrameBuff[FrameDataLen-1] != 0x16)
    {
        return CHECK_RESULT_FAILUER;
    }


    if (AppData->AFN == 0x00)
    {
        if (AppData->Fn == 1)                   //ȷ��
        {
            return CHECK_RESULT_CONFIRM;
        }
        else if (AppData->Fn == 2)              //����
        {
            return CHECK_RESULT_DENY;
        }
        else
        {
            return CHECK_RESULT_FAILUER;
        }
    }
    else
    {

        return CHECK_RESULT_SUCCEED;
    }


}



/*******************************************************************************
* ��������: ClearCommCbRecvBuff
* ��������: ���������ջ�������
* �������: ��
* �������: ��
* �� �� ֵ:  ��
*******************************************************************************/
void ClearCommCbRecvBuff(uint8 CommPipe)
{
    int     RecvLen = 0xff;                     //�������ݳ���
    uint8   RecvBuf[MAX_MSG_LEN];        //�������ݻ�����

    while (RecvLen > 0)
    {
        RecvLen = taskmng_plc_queue_read(pAmrRecvDown_Cb[CommPipe], RecvBuf);
    }
}


/*******************************************************************************
* ��������: taskmng_module_head_node_delete
* ��������: ɾ������
* �������:
*          pModuleMetAddrHead
           DeleteNum
* �������: ��
* �� �� ֵ: >0                  �ɹ�
*           <0                  ʧ��
*******************************************************************************/
MODULE_METADDRHEAD *taskmng_module_head_node_delete(MODULE_METADDRHEAD *pModuleMetAddrHead,uint16 DeleteNum)
{
    uint16              DeleteCnt = 0;

    MODULE_METADDRHEAD  *TmppModuleMetAddrHead = NULL;


    while(pModuleMetAddrHead != NULL)//�����Ϊ��
    {
        DeleteCnt++;
        TmppModuleMetAddrHead = pModuleMetAddrHead;

        if(pModuleMetAddrHead->PNext != NULL)
        {
            TmppModuleMetAddrHead = pModuleMetAddrHead->PNext;
        }
        else
            TmppModuleMetAddrHead = NULL;

        taskmng_free(__FILE__, __LINE__,pModuleMetAddrHead);

        pModuleMetAddrHead = TmppModuleMetAddrHead;

        if(DeleteCnt == DeleteNum)
            break;
    }

    return pModuleMetAddrHead;
}

void *taskmng_malloc(char const* file, int line, size_t size)
{
    //void *Ptr_Addr = NULL;

    return malloc(size);

}


void taskmng_free(char const* file, int line, void *p)
{
    free(p);
    p = NULL;
}



/*******************************************************************************
* ��������: taskmng_module_head_node_add
* ��������: ��ӽڵ���Ϣ����Ӧ������ͷ
* �������:
            RouterSn                   ��ӵ����
*           MtrAddr                    ����ַ
*           pModuleMetAddrHead         ��ӵ�����ͷ)
* �������:
* �� �� ֵ:
*******************************************************************************/
MODULE_METADDRHEAD *taskmng_module_head_node_add(uint16 RouterSn,uint8 *Addr,uint8 prtl,MODULE_METADDRHEAD *pModuleMetAddrHead)
{
    uint16  ModuleAddrNodeNum = 1;  //��Ҫ���ӻ���ɾ���Ĵӽڵ�����

    MODULE_METADDRHEAD  *Tmp_pModuleMetAddrHead = NULL;

    if(SerachNodeInLink(pModuleMetAddrHead,Addr) > 0)
    {
        return pModuleMetAddrHead;
    }

    Tmp_pModuleMetAddrHead = taskmng_malloc(__FILE__, __LINE__,sizeof(MODULE_METADDRHEAD));
    if(Tmp_pModuleMetAddrHead == NULL)
        return NULL;

    memcpy(Tmp_pModuleMetAddrHead->Module_MetAddr,Addr,6);

    Tmp_pModuleMetAddrHead->RouterSn = RouterSn;
    Tmp_pModuleMetAddrHead->Prtl = prtl;

    Tmp_pModuleMetAddrHead->PNext = NULL;

    if(pModuleMetAddrHead == NULL)
    {
        pModuleMetAddrHead = Tmp_pModuleMetAddrHead;
    }
    else
    {

        MODULE_METADDRHEAD *Start_ModuleAddr = NULL;

        Start_ModuleAddr = pModuleMetAddrHead;            //������ͷ���赱ǰ��ָ��

        while(Start_ModuleAddr->PNext != NULL)            //�ҵ���������һ��λ��
        {
            ModuleAddrNodeNum++;

            if(ModuleAddrNodeNum > MAX_AMR_COLLECT_CNT)
            {
                taskmng_free(__FILE__, __LINE__,Tmp_pModuleMetAddrHead);
                return NULL;
            }

            Start_ModuleAddr = Start_ModuleAddr->PNext;
        }
        Start_ModuleAddr->PNext = Tmp_pModuleMetAddrHead;
    }


    return pModuleMetAddrHead;
}

/*******************************************************************************
* ��������: SerachNodeInLink
* ��������: ��ѯ�˴������Ĵӽڵ���Ϣ���������Ƿ��Ѿ�����
* �������:
*           pModuleMetAddrHead         ԭ����
*           Addr                       ��Ҫ�����ĵ�ַ��Ϣ
* �������: ��
* �� �� ֵ: >0                  ����
*           <0                  ������
*******************************************************************************/
int SerachNodeInLink(MODULE_METADDRHEAD *pModuleMetAddrHead,uint8 *Addr)
{
    MODULE_METADDRHEAD *TmpModuleLink = NULL;

    if(pModuleMetAddrHead == NULL)
    {
        return -1;//������
    }

    TmpModuleLink = pModuleMetAddrHead;
    while(TmpModuleLink != NULL)
    {
        if(memcmp(TmpModuleLink->Module_MetAddr,Addr,6) == 0)
            return 1;//����

        TmpModuleLink = TmpModuleLink->PNext;
    }

    return -1;//������
}







/*******************************************************************************
* ��������: GetNeedMtrNum
* ��������: ��ȡÿ���߳���������Ҫ�����ı����
* �������: pipe �˿ں�
            taskindex ��������
* �������: ��
* �� �� ֵ:  ���߳���Ҫ�����ĵ�����
*******************************************************************************/
uint16 GetNeedMtrNum(uint8 pipe,uint8 taskindex)
{
    uint16 i;
    uint16 count=0;
    pthread_rwlock_rdlock(&TaskRWLock);
    for(i=0;i<SchemeTaskRunQueue[taskindex].TotalNum;i++)
    {
        if(pipe==SchemeTaskRunQueue[taskindex].MsInfo[i].Pipe)
            count++;
    }

    pthread_rwlock_unlock(&TaskRWLock);
    return count;

}




/*******************************************************************************
* ��������: TaskRunSort
* ��������: //��ÿ���߳��е�����ʱ������а������ȼ����������γ�����
* �������:

* �������: ��
* �� �� ֵ:
*******************************************************************************/
void TaskRunSort(uint8 Pipe)
{
    uint8 i;
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[Pipe];
    for(i = 0;i<pAmrThreadArg->TaskNum;i++)
    {
        pAmrThreadArg->pTaskHead = taskmng_scheme_task_link_add(&pAmrThreadArg->pTaskRunInfo[i],pAmrThreadArg->pTaskHead);
    }

    CTS_RUN_TASK_T *PTmpHeadTaskLink = NULL;//��ѯ����ʱʹ��
    PTmpHeadTaskLink = pAmrThreadArg->pTaskHead;
    TASK_FMT_TRACE(Pipe,REC_LOG,"�ճ������������   : \n");

    while(PTmpHeadTaskLink != NULL)
    {
        TASK_FMT_TRACE(Pipe,REC_LOG," TaskId %d \n",PTmpHeadTaskLink->TaskId);
        TASK_FMT_TRACE(Pipe,REC_LOG," NeedMtrNum %d  \n",PTmpHeadTaskLink->NeedMtrNum);
        TASK_FMT_TRACE(Pipe,REC_LOG," Priority  %d  \n",PTmpHeadTaskLink->Priority);
        PTmpHeadTaskLink = PTmpHeadTaskLink->next;
    }
}

/*******************************************************************************
* ��������: TaskRun_UpdateTimes
* ��������: ����ÿ���߳�������ִ���ִ�
* �������: Pipe  �˿ں�
          index  ��������
          TaskInfo ��������ʱ����
* �������: ��
* �� �� ֵ:
*******************************************************************************/
BOOL TaskRun_UpdateTimes(uint8 Pipe,uint8 index,CTS_RUN_TASK_T *TaskInfo)
{
    //rtc_t            RealTime;                     //��ǰϵͳʱ��ʱ��(RTC��ʽ)
    time_t           RealTime_t;                   //��ǰϵͳʱ���ת��������
    struct tm        tm_time;
    time_t           ThisTurnBeginTime_t;
    time_t           ThisTurnEndTime_t;
    time_t          NextTurnBeginTime_t;
    //rtc_t            TaskRunBeginTime;             //������ʼִ��ʱ��(RTC��ʽ)
    time_t           TaskRunBeginTime_t;           //������ʼִ��ת�������
    //rtc_t            TaskRunEndTime;               //��������ִ��ʱ��(RTC��ʽ)
    //time_t           TaskRunEndTime_t;             //��������ִ��ת�������
    OOP_TASK_CFG *pTaskCfg = &SchemeTaskCfgList.OopTaskCfg[index];
    uint32           dwPeriod = pTaskCfg->RunCycle;   //�ظ�����
    time_t           ProThisTurnTimeMark_t;
    time_t           ProThisTurnBeginTime_t;
    time_t           ProThisTurnEndTime_t;
    time_t           ProNextTurnBeginTime_t;
    time_t           ProLastTurnBeginTime_t;
    rtc_t            rtc_tmp;
    OOP_TI_T             Ti;
    //uint16           Year;

    //ϵͳ��ǰʱ��
    RealTime_t = time(NULL);
    LOCALTIME_R(&RealTime_t, &tm_time);

    memset(&rtc_tmp, 0, sizeof(rtc_t));
    memcpy(&rtc_tmp, TaskInfo->ThisTurnBeginTime, 6);
    ThisTurnBeginTime_t = commfun_rtc2time2(rtc_tmp);


    memset(&rtc_tmp, 0, sizeof(rtc_t));
    memcpy(&rtc_tmp, TaskInfo->ThisTurnEndTime, 6);
    ThisTurnEndTime_t = commfun_rtc2time2(rtc_tmp);

    //��ȥ����ʼʱ�����������ʱ
    Ti.unit = pTaskCfg->DelayTimeUnit;
    Ti.value = pTaskCfg->DelayTime;
    DT_TimeSubInv(&ThisTurnEndTime_t, Ti);
    DT_TimeSubInv(&ThisTurnBeginTime_t, Ti);
    //TASK_FMT_TRACE(Pipe,REC_LOG,"��ǰʱ���� is %d,���ֿ�ʼʱ�� is %d,���ֽ���ʱ�� is %d\n",RealTime_t,ThisTurnEndTime_t,ThisTurnBeginTime_t);
    if(FALSE==taskmng_task_in_period_check(index,RealTime_t))
    {
        return FALSE;
    }
    //TASK_FMT_TRACE(Pipe,REC_LOG,"��ǰʱ����11 is %d,���ֿ�ʼʱ�� is %d,���ֽ���ʱ�� is %d\n",RealTime_t,ThisTurnEndTime_t,ThisTurnBeginTime_t);
    if(RealTime_t >= ThisTurnBeginTime_t && RealTime_t < ThisTurnEndTime_t) //���ڱ����������
    {
        return FALSE;
    }
    //����ִ�� �Է�һ����ִ��̫���
    //TASK_FMT_TRACE(Pipe,REC_LOG,"Pipe is %d,gPastTimeFlag[Pipe].BitSect.Second is %d\n",Pipe,gPastTimeFlag[Pipe].BitSect.Second);
    if ( gPastTimeFlag[Pipe].BitSect.Second != TRUE )
        return FALSE;

    memset(&rtc_tmp, 0, sizeof(rtc_t));
    memcpy(&rtc_tmp, TaskInfo->NextTurnBeginTime, 6);
    NextTurnBeginTime_t = commfun_rtc2time2(rtc_tmp);
    if(RealTime_t>=ThisTurnEndTime_t && RealTime_t <NextTurnBeginTime_t)
    {
        //TASK_FMT_TRACE(Pipe,REC_LOG,"���� %d �����������ʱ�� ���������\n",TaskInfo->TaskId);
        TaskInfo->MinFrozenRebootCnt = 0;
        taskmng_task_finish(gAmrThreadArg[Pipe].DBClientHandle,TaskInfo->TaskId);
    }
    if(dwPeriod==0)
    {
        dwPeriod=1;//��Ϊ���ݻ�Ϊ0����Ĭ��һ��1
    }

    TASK_FMT_TRACE(Pipe,REC_LOG,"���� %d ���ڱ��� realtime %ld begin %ld end %ld \n",TaskInfo->TaskId,RealTime_t,ThisTurnBeginTime_t,ThisTurnEndTime_t);

    //����ʼʱ��

    DT_DateTimeShort2Time(&pTaskCfg->RunBeginTime,&TaskRunBeginTime_t);

    switch (pTaskCfg->RunCycleUnit) //ִ��Ƶ�ʵ�λ
    {
    case TASK_CYCLE_FLAG_SEC:       //����
        ProThisTurnTimeMark_t  = (RealTime_t/dwPeriod)*dwPeriod;//�������ڻ�ȡ��ǰ��ʱ���
        ProThisTurnBeginTime_t = ProThisTurnTimeMark_t + TaskRunBeginTime_t%dwPeriod;
        if(ProThisTurnBeginTime_t > RealTime_t)
            ProThisTurnBeginTime_t -= dwPeriod;
        ProThisTurnEndTime_t   = (ProThisTurnBeginTime_t + dwPeriod)-1;
        ProNextTurnBeginTime_t = ProThisTurnBeginTime_t + dwPeriod;
        ProLastTurnBeginTime_t = ProThisTurnBeginTime_t - dwPeriod;
        break;

    case TASK_CYCLE_FLAG_MIN:       //����
        dwPeriod *= 60L;
        ProThisTurnTimeMark_t  = (RealTime_t/dwPeriod)*dwPeriod;//�������ڻ�ȡ��ǰ��ʱ���
        ProThisTurnBeginTime_t = ProThisTurnTimeMark_t + TaskRunBeginTime_t%dwPeriod;
        if(ProThisTurnBeginTime_t > RealTime_t)
            ProThisTurnBeginTime_t -= dwPeriod;
        ProThisTurnEndTime_t   = (ProThisTurnBeginTime_t + dwPeriod)-1;
        ProNextTurnBeginTime_t = ProThisTurnBeginTime_t + dwPeriod;
        ProLastTurnBeginTime_t = ProThisTurnBeginTime_t - dwPeriod;
        break;

    case TASK_CYCLE_FLAG_HOUR:      //Сʱ
        dwPeriod *= 3600L;
        ProThisTurnTimeMark_t  = (RealTime_t/dwPeriod)*dwPeriod;
        ProThisTurnBeginTime_t = ProThisTurnTimeMark_t + TaskRunBeginTime_t%dwPeriod;
        if(ProThisTurnBeginTime_t > RealTime_t)
            ProThisTurnBeginTime_t -= dwPeriod;
        ProThisTurnEndTime_t   = (ProThisTurnBeginTime_t + dwPeriod)-60;
        ProNextTurnBeginTime_t = ProThisTurnBeginTime_t + dwPeriod;
        ProLastTurnBeginTime_t = ProThisTurnBeginTime_t - dwPeriod;
        break;

    case TASK_CYCLE_FLAG_DAY:       //��
        dwPeriod *= 86400L;
        ProThisTurnTimeMark_t  = (RealTime_t/dwPeriod)*dwPeriod;
        ProThisTurnBeginTime_t = ProThisTurnTimeMark_t + TaskRunBeginTime_t%dwPeriod;
        if(ProThisTurnBeginTime_t > RealTime_t)
            ProThisTurnBeginTime_t -= dwPeriod;
        ProThisTurnEndTime_t   = (ProThisTurnBeginTime_t + dwPeriod)-60;
        ProNextTurnBeginTime_t = ProThisTurnBeginTime_t + dwPeriod;
        ProLastTurnBeginTime_t = ProThisTurnBeginTime_t - dwPeriod;
        break;

    case TASK_CYCLE_FLAG_MON:       //��
    {
        time_t          tm_tmp = 0;
        OOP_DATETIME_S_T tmp;
        memset(&tmp,0x00,sizeof(OOP_DATETIME_S_T));

        //������ʼʱ����ո�Ϊ0
        tmp.year = pTaskCfg->RunBeginTime.year;
        tmp.month = pTaskCfg->RunBeginTime.month;
        tmp.day = 1;
        DT_DateTimeShort2Time(&tmp,&TaskRunBeginTime_t);


        Ti.unit  = pTaskCfg->RunCycleUnit;
        Ti.value = pTaskCfg->RunCycle;

        tm_tmp = TaskRunBeginTime_t;
        while(1)
        {
            DT_TimeAddInv(&tm_tmp, Ti);
            if(tm_tmp > RealTime_t)
            {
                break;
            }
        }
        ProThisTurnBeginTime_t = tm_tmp;
        DT_TimeSubInv(&ProThisTurnBeginTime_t, Ti);

        ProThisTurnEndTime_t = ProThisTurnBeginTime_t;
        DT_TimeAddInv(&ProThisTurnEndTime_t, Ti);
        ProThisTurnEndTime_t -= 60;

        ProNextTurnBeginTime_t = ProThisTurnBeginTime_t;
        DT_TimeAddInv(&ProNextTurnBeginTime_t, Ti);

        ProLastTurnBeginTime_t = ProThisTurnBeginTime_t;
        DT_TimeSubInv(&ProLastTurnBeginTime_t, Ti);
        break;
    }

    default:
        return 0;
    }
    //����ʱ����ȥ
    Ti.unit = pTaskCfg->DelayTimeUnit;
    Ti.value = pTaskCfg->DelayTime;


    DT_TimeAddInv(&ProThisTurnBeginTime_t, Ti);


    LOCALTIME_R(&ProThisTurnBeginTime_t, &tm_time);
    if(tm_time.tm_year<100)
        tm_time.tm_year = 100;
    TaskInfo->ThisTurnBeginTime[6] = 0x20;
    TaskInfo->ThisTurnBeginTime[5] = commfun_HexToBcd((uint8)(tm_time.tm_year - 100));  //��
    TaskInfo->ThisTurnBeginTime[4] = commfun_HexToBcd((uint8)(tm_time.tm_mon + 1));     //��
    TaskInfo->ThisTurnBeginTime[3] = commfun_HexToBcd((uint8)tm_time.tm_mday);          //��
    TaskInfo->ThisTurnBeginTime[2] = commfun_HexToBcd((uint8)tm_time.tm_hour);          //ʱ
    TaskInfo->ThisTurnBeginTime[1] = commfun_HexToBcd((uint8)tm_time.tm_min);           //��
    TaskInfo->ThisTurnBeginTime[0] = commfun_HexToBcd((uint8)tm_time.tm_sec);           //��

    Ti.unit = pTaskCfg->DelayTimeUnit;
    Ti.value = pTaskCfg->DelayTime;
    DT_TimeAddInv(&ProThisTurnEndTime_t, Ti);
    LOCALTIME_R(&ProThisTurnEndTime_t, &tm_time);
    if(tm_time.tm_year<100)
        tm_time.tm_year = 100;
    TaskInfo->ThisTurnEndTime[6] = 0x20;
    TaskInfo->ThisTurnEndTime[5] = commfun_HexToBcd((uint8)(tm_time.tm_year - 100));    //��
    TaskInfo->ThisTurnEndTime[4] = commfun_HexToBcd((uint8)(tm_time.tm_mon + 1));       //��
    TaskInfo->ThisTurnEndTime[3] = commfun_HexToBcd((uint8)tm_time.tm_mday);            //��
    TaskInfo->ThisTurnEndTime[2] = commfun_HexToBcd((uint8)tm_time.tm_hour);            //ʱ
    TaskInfo->ThisTurnEndTime[1] = commfun_HexToBcd((uint8)tm_time.tm_min);         //��
    TaskInfo->ThisTurnEndTime[0] = commfun_HexToBcd((uint8)tm_time.tm_sec);         //��

    LOCALTIME_R(&ProNextTurnBeginTime_t, &tm_time);
    if(tm_time.tm_year<100)
        tm_time.tm_year = 100;
    TaskInfo->NextTurnBeginTime[6] =0x20;
    TaskInfo->NextTurnBeginTime[5] = commfun_HexToBcd((uint8)(tm_time.tm_year - 100));  //��
    TaskInfo->NextTurnBeginTime[4] = commfun_HexToBcd((uint8)(tm_time.tm_mon + 1));     //��
    TaskInfo->NextTurnBeginTime[3] = commfun_HexToBcd((uint8)tm_time.tm_mday);          //��
    TaskInfo->NextTurnBeginTime[2] = commfun_HexToBcd((uint8)tm_time.tm_hour);          //ʱ
    TaskInfo->NextTurnBeginTime[1] = commfun_HexToBcd((uint8)tm_time.tm_min);           //��
    TaskInfo->NextTurnBeginTime[0] = commfun_HexToBcd((uint8)tm_time.tm_sec);           //��

    LOCALTIME_R(&ProLastTurnBeginTime_t, &tm_time);
    if(tm_time.tm_year<100)
        tm_time.tm_year = 100;
    TaskInfo->LastTurnBeginTime[6] =0x20;
    TaskInfo->LastTurnBeginTime[5] = commfun_HexToBcd((uint8)(tm_time.tm_year - 100));  //��
    TaskInfo->LastTurnBeginTime[4] = commfun_HexToBcd((uint8)(tm_time.tm_mon + 1));     //��
    TaskInfo->LastTurnBeginTime[3] = commfun_HexToBcd((uint8)tm_time.tm_mday);          //��
    TaskInfo->LastTurnBeginTime[2] = commfun_HexToBcd((uint8)tm_time.tm_hour);          //ʱ
    TaskInfo->LastTurnBeginTime[1] = commfun_HexToBcd((uint8)tm_time.tm_min);           //��
    TaskInfo->LastTurnBeginTime[0] = commfun_HexToBcd((uint8)tm_time.tm_sec);           //��
    taskmng_task_monitor_update(Pipe,TaskInfo->TaskId,TaskInfo->logicid,UDTASK_INIT,TaskInfo);

    return TRUE;
}


/*******************************************************************************
* ��������: TaskRun_UpdatePeriodStat
* ��������: �ж������Ƿ�������ʱ��
* �������: TaskInfo ��������ʱ����
            index  ��������
* �������: ��
* �� �� ֵ:
*******************************************************************************/
void TaskRun_UpdatePeriodStat(uint8 index,CTS_RUN_TASK_T *TaskInfo)
{
    rtc_t           rtc_tmp;
    time_t          RealTime_t;     //��ǰϵͳʱ���ת��������
    time_t          TaskRunBeginTime_t; //������ʼִ��ת�������
    time_t          TaskRunEndTime_t;   //��������ִ��ת�������
    time_t          ThisTurnBeginTime_t;
    time_t          ThisTurnEndTime_t;
    OOP_TI_T            Ti;
    //uint16          Year;
    OOP_TASK_CFG *pTaskCfg = &SchemeTaskCfgList.OopTaskCfg[index];

    RealTime_t = time(NULL);

    memset((char *)&rtc_tmp, 0x00, sizeof(rtc_t));
    memcpy((char *)&rtc_tmp, TaskInfo->ThisTurnBeginTime, 6);
    ThisTurnBeginTime_t = commfun_rtc2time2(rtc_tmp);

    memset((char *)&rtc_tmp, 0x00, sizeof(rtc_t));
    memcpy((char *)&rtc_tmp, TaskInfo->ThisTurnEndTime, 6);
    ThisTurnEndTime_t = commfun_rtc2time2(rtc_tmp);
    Ti.unit = pTaskCfg->DelayTimeUnit;
    Ti.value = pTaskCfg->DelayTime;
    DT_TimeSubInv(&ThisTurnEndTime_t, Ti);


    DT_DateTimeShort2Time(&pTaskCfg->RunBeginTime,&TaskRunBeginTime_t);

    uint8 flag = 0;
    //time_t���ֻ�ܱ�ʾ��2038��
    if(pTaskCfg->RunEndTime.year<2038)
    {
        DT_DateTimeShort2Time(&pTaskCfg->RunEndTime,&TaskRunEndTime_t);
        flag = 1;
    }

    if((RealTime_t < TaskRunBeginTime_t) ||(flag==1&& (RealTime_t > TaskRunEndTime_t)))
    {
        TaskInfo->ThisTurnPeriodStat = AMR_AFT_PERIOD;
        return ;
    }

    //�жϵ�ǰ�Ƿ��Ƿ��������ִ�����ʼ�Լ�����֮��
    if((RealTime_t >= ThisTurnBeginTime_t) && (RealTime_t < ThisTurnEndTime_t))
    {
        TaskInfo->ThisTurnPeriodStat = AMR_IN_PERIOD;

        //�������������Ƿ�������ʱ����
        taskmng_task_run_in_period(TaskInfo,pTaskCfg);
    }

    //�жϵ�ǰ�Ƿ��Ƿ��������ִν���֮��
    if(RealTime_t < ThisTurnBeginTime_t)
    {
        TaskInfo->ThisTurnPeriodStat = AMR_PRE_PERIOD;
    }

    //�жϵ�ǰ�Ƿ��Ƿ��������ִν���֮��
    if(RealTime_t > ThisTurnEndTime_t)
    {
        TaskInfo->ThisTurnPeriodStat = AMR_AFT_PERIOD;
    }
    //TASK_FMT_TRACE( -1, REC_LOG," TaskInfo->ThisTurnPeriodStat is %d,TaskRunBeginTime_t is %d,TaskRunEndTime_t is %d\n",TaskInfo->ThisTurnPeriodStat,TaskRunBeginTime_t,TaskRunEndTime_t);
}






/*******************************************************************************
* ��������: GetPriorityFromTaskID
* ��������: ��������id��ȡ�������ȼ�
* �������: Pipe  TaskId
* �������:
* �� �� ֵ: �������ȼ�
*******************************************************************************/
uint8 GetPriorityFromTaskID(uint8 Pipe,uint8 TaskId)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[Pipe];
    uint8 i;
    for(i = 0;i<pAmrThreadArg->TaskNum;i++)
    {
        if(TaskId == pAmrThreadArg->pTaskRunInfo[i].TaskId)
        {
            return pAmrThreadArg->pTaskRunInfo[i].Priority;
        }
    }
    return 0xff;
}




/*******************************************************************************
* ��������: GetNeedAgainTaskInfo
* ��������:
* �������: TaskId
* �������: TaskInfo
* �� �� ֵ:
*******************************************************************************/
BOOLEAN GetNeedAgainTaskInfo(SCHEME_TASK_RUN_INFO *TaskInfo,CTS_RUN_TASK_T **RunTask,uint8 Pipe)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[Pipe];
    uint8 TaskNum;
    TaskNum = pAmrThreadArg->TaskNum;
    uint8 i=0,j=0;
    BOOLEAN flag=FALSE;
    BOOLEAN HasTaskNeedBc=FALSE;


    for(j=0;j<TaskNum;j++)
    {

        if(pAmrThreadArg->pTaskRunInfo[j].NeedAgain == TRUE)
        {
            TASK_FMT_TRACE ( Pipe,COMM_LOG, "�жϲ���: pAmrThreadArg->pTaskRunInfo[j].IsAlreadyBc is %d,pAmrThreadArg->pTaskRunInfo[j].TaskId is %d\n", pAmrThreadArg->pTaskRunInfo[j].IsAlreadyBc,pAmrThreadArg->pTaskRunInfo[j].TaskId);
            if(pAmrThreadArg->pTaskRunInfo[j].IsAlreadyBc ==TASKBC_DOING)
            {
                *RunTask = &pAmrThreadArg->pTaskRunInfo[j];
                 taskmng_task_info_get((*RunTask)->TaskId,(*RunTask)->logicid,TaskInfo);
                return TRUE;
            }
            flag=TRUE;
            if(TASKBC_START==pAmrThreadArg->pTaskRunInfo[j].IsAlreadyBc)
            {
                HasTaskNeedBc=TRUE;
                //pAmrThreadArg->pTaskRunInfo[j].IsAlreadyBc=TRUE;
                *RunTask = &pAmrThreadArg->pTaskRunInfo[j];
                break;
            }

        }
    }
    if((j>=TaskNum)&&(TRUE==flag))
    {
        for(i=0;i<TaskNum;i++)
        {
            //if(pAmrThreadArg->pTaskRunInfo[i].NeedAgain == TRUE)
            //{
                pAmrThreadArg->pTaskRunInfo[i].IsAlreadyBc=TASKBC_START;
            //}
        }
    }

    if((HasTaskNeedBc==TRUE)&&(pAmrThreadArg->pTaskRunInfo[j].IsAlreadyBc !=TASKBC_DOING))
    {
        pAmrThreadArg->pTaskRunInfo[j].IsAlreadyBc =TASKBC_DOING;
        taskmng_task_info_get((*RunTask)->TaskId,(*RunTask)->logicid,TaskInfo);
        (*RunTask)->MeterIndex=0;
        (*RunTask)->CbNum=0;
        return TRUE;
    }
    return FALSE;
}





/*******************************************************************************
* ��������: GetAcNeedAgainTaskInfo
* ��������:
* �������: TaskId
* �������: TaskInfo
* �� �� ֵ:
*******************************************************************************/
BOOLEAN GetAcNeedAgainTaskInfo(SCHEME_TASK_RUN_INFO *TaskInfo,CTS_RUN_TASK_T **RunTask)
{

    uint8 TaskNum;
    TaskNum = TGThreadArg.TaskNum;
    uint8 i=0,j=0;
    BOOLEAN flag=FALSE;
    BOOLEAN HasTaskNeedBc=FALSE;


    for(j=0;j<TaskNum;j++)
    {

        if(TGThreadArg.pTaskRunInfo[j].NeedAgain == TRUE)
        {
            TASK_FMT_TRACE( -1,TG_LOG, "�жϲ���: TGThreadArg.pTaskRunInfo[j].IsAlreadyBc is %d,TGThreadArg.pTaskRunInfo[j].TaskId is %d\n", TGThreadArg.pTaskRunInfo[j].IsAlreadyBc,TGThreadArg.pTaskRunInfo[j].TaskId);
            if(TGThreadArg.pTaskRunInfo[j].IsAlreadyBc ==TASKBC_DOING)
            {
                *RunTask = &TGThreadArg.pTaskRunInfo[j];
                 taskmng_task_info_get((*RunTask)->TaskId,(*RunTask)->logicid,TaskInfo);
                return TRUE;
            }
            flag=TRUE;
            if(TASKBC_START==TGThreadArg.pTaskRunInfo[j].IsAlreadyBc)
            {
                HasTaskNeedBc=TRUE;
                //pAmrThreadArg->pTaskRunInfo[j].IsAlreadyBc=TRUE;
                *RunTask = &TGThreadArg.pTaskRunInfo[j];
                break;
            }

        }
    }
    if((j>=TaskNum)&&(TRUE==flag))
    {
        for(i=0;i<TaskNum;i++)
        {
            //if(pAmrThreadArg->pTaskRunInfo[i].NeedAgain == TRUE)
            //{
                TGThreadArg.pTaskRunInfo[i].IsAlreadyBc=TASKBC_START;
            //}
        }
    }

    if((HasTaskNeedBc==TRUE)&&(TGThreadArg.pTaskRunInfo[j].IsAlreadyBc !=TASKBC_DOING))
    {
        TGThreadArg.pTaskRunInfo[j].IsAlreadyBc =TASKBC_DOING;
        taskmng_task_info_get((*RunTask)->TaskId,(*RunTask)->logicid,TaskInfo);
        (*RunTask)->MeterIndex=0;
        (*RunTask)->CbNum=0;
        return TRUE;
    }
    return FALSE;
}

void GetTGRunTask(uint8 TaskId,CTS_RUN_TASK_T **RunTask)
{

    uint8 TaskNum;
    TaskNum = TGThreadArg.TaskNum;
    uint8 i;
    for(i=0;i<TaskNum;i++)
    {
        if(TGThreadArg.pTaskRunInfo[i].TaskId == TaskId)
        {
            *RunTask = &TGThreadArg.pTaskRunInfo[i];
            return;
        }
    }

}


/*******************************************************************************
* ��������: taskmng_oop_meter_security_para_read
* ��������: ��ȡ��������ȫģʽ��ز���
* �������:
* �������:
* �� �� ֵ:
*******************************************************************************/
int taskmng_oop_meter_security_para_read(uint8 Pipe,uint16 Pn,uint8 *MtrEsamSerialNo,uint8 *MtrSecModEn)
{
    //���ó���ȫģʽ��
    return 1;
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[Pipe];
    int ret;
    uint32 len;
    NOMAL_OAD_T NormalOad ;
    NormalOad.logicId = 0;
    NormalOad.infoNum = Pn;
    NormalOad.oad.value = 0xF1010200;

    ret = db_read_nomal(pAmrThreadArg->DBClientHandle, &NormalOad, 1, MtrSecModEn, &len);

    if(ret!=0||len!=1||*MtrSecModEn ==0xFF)
    {
        return -1;
    }
    NormalOad.oad.value = 0xF1000200;

    ret = db_read_nomal(pAmrThreadArg->DBClientHandle, &NormalOad, 8, MtrEsamSerialNo, &len);

    if(ret!=0||len!=8|| CheckFF(MtrEsamSerialNo,8) == TRUE)
    {
        return -1;
    }
    return 1;
}
/*******************************************************************************
* ��������: taskmng_oop_meter_security_para_read
* ��������: ��ȡ��������ȫģʽESAM���к�
* �������:
* �������:
* �� �� ֵ:
*******************************************************************************/
int ReadOopMeterSecurityEsamSerialNo(uint8 Pipe,uint16 Pn,uint8 *MtrEsamSerialNo)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[Pipe];
    int ret;
    uint32 len;
    NOMAL_OAD_T NormalOad ;
    NormalOad.logicId = 0;
    NormalOad.infoNum = Pn;
    NormalOad.oad.value = 0xF1000200;

    ret = db_read_nomal(pAmrThreadArg->DBClientHandle, &NormalOad, 8, MtrEsamSerialNo, &len);

    if(ret<0||len!=8|| CheckFF(MtrEsamSerialNo,8) == TRUE)
    {
        return -1;
    }
    return 1;
}
/*******************************************************************************
* ��������: taskmng_oop_meter_security_para_read
* ��������: ��ȡ��������ȫģʽESAM���к�
* �������:
* �������:
* �� �� ֵ:
*******************************************************************************/
int ReadOopMeterSecuritySecModEn(uint8 Pipe,uint16 Pn,uint8 *MtrSecModEn)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[Pipe];
    int ret;
    uint32 len;
    NOMAL_OAD_T NormalOad ;
    NormalOad.logicId = 0;
    NormalOad.infoNum = Pn;
    NormalOad.oad.value = 0xF1010200;

    ret = db_read_nomal(pAmrThreadArg->DBClientHandle, &NormalOad, 1, MtrSecModEn, &len);

    if(ret<0||len!=1||*MtrSecModEn ==0xFF)
    {
        return -1;
    }
    return 1;
}
/*******************************************************************************
* ��������: WriteOopMeterSecurityEsamSerialNo
* ��������: д��������ȫģʽ
* �������:
* �������:
* �� �� ֵ:
*******************************************************************************/
void WriteOopMeterSecurityEsamSerialNo(uint8 Pipe,uint16 Pn,uint8 *MtrEsamSerialNo)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[Pipe];

    NOMAL_OAD_T NormalOad ;
    NormalOad.logicId = 0;
    NormalOad.infoNum = Pn;
    NormalOad.oad.value = 0xF1000200;
    NormalOad.classtag = 3;

    db_write_nomal(pAmrThreadArg->DBClientHandle,&NormalOad,MtrEsamSerialNo,8);

}
/*******************************************************************************
* ��������: taskmng_oop_meter_security_para_read
* ��������: ��ȡ��������ȫģʽ����
* �������:
* �������:
* �� �� ֵ:
*******************************************************************************/
void WriteOopMeterSecuritySecModEn(uint8 Pipe,uint16 Pn,uint8 MtrSecModEn)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[Pipe];

    NOMAL_OAD_T NormalOad ;
    NormalOad.logicId = 0;
    NormalOad.infoNum = Pn;
    NormalOad.oad.value = 0xF1010200;
    NormalOad.classtag = 3;

    db_write_nomal(pAmrThreadArg->DBClientHandle,&NormalOad,&MtrSecModEn,1);

}



/*******************************************************************************
* ��������: taskmng_mtsd_188_get_request
* ��������: 188�ı���֡
* �������:
* �������:
* �� �� ֵ:
*******************************************************************************/
uint16 taskmng_mtsd_188_get_request( uint8 pipe,uint8 *FrameBuff,int mterindex)
{
    uint8   CS = 0;                       //У����
    uint8 tmpSnd[30];
    uint8 i;
    uint8 offset = 0;
    static uint8 ser = 0;
    uint8  meter_type=0;
    OOP_TSA_T                   TsaAddr;
    uint16 fraDI=0x901F;
    OOP_METER_ANNEX_T       annex;
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    uint16 oadvalue=0;

    TsaAddr = taskmng_tsa_get(pAmrThreadArg->pMtrAmrInfo[mterindex].Pn,pAmrThreadArg->pMtrAmrInfo[mterindex].logicid);
    annex =taskmng_mtr_annex_get(pAmrThreadArg->pMtrAmrInfo[mterindex].Pn,pAmrThreadArg->pMtrAmrInfo[mterindex].logicid);

    for(i=0;i<annex.nNum;i++)
    {
        if(annex.annexObj[i].oad.value == 0x40360200)
        {
            memcpy_r((uint8*)&oadvalue,annex.annexObj[i].value,2);
            if(oadvalue == 0x0310)
            {
                meter_type = 0x10;
            }
            else if(oadvalue == 0x0320)
            {
                meter_type = 0x20;
            }
        }
    }
    if(meter_type==0)
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
    tmpSnd[offset++] = TsaAddr.add[6];
    tmpSnd[offset++] = TsaAddr.add[5];
    tmpSnd[offset++] = TsaAddr.add[4];
    tmpSnd[offset++] = TsaAddr.add[3];
    tmpSnd[offset++] = TsaAddr.add[2];
    tmpSnd[offset++] = TsaAddr.add[1];
    tmpSnd[offset++] = TsaAddr.add[0];
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

    memcpy(FrameBuff, tmpSnd, offset);        /*- ����ͻ�����pBuf*/

    return  offset;
}



/*******************************************************************************
* ��������: taskmng_meter_frame_make
* ��������: ������֡
* �������: Pipe �˿ں�
         StartTime  ��ʼʱ��
         FrameSeqInfo ֡��Ϣ

* �������: DataBuff ��õĳ���֡
* �� �� ֵ: ����֡����
*******************************************************************************/
uint16 taskmng_meter_frame_make(uint8 Pipe,uint8 seqindex,time_t StartTime,FRAME_SEQ_INFO_T FrameSeqInfo,uint8 *DataBuff,uint16 taskid,uint8 logicid)
{

    MeterPrtlFmt_T  MeterPrtlData;                      //����Լ��ʽ����
    //uint8           MtrEsamSerialNo[8];                 //����������
   // uint8           MtrSecModEn;                        //��������ȫģʽ����
    uint32          DI07 = 0;
    //uint16          LastTaskId;
    int             MsIndex;
    uint8           SubOadNum;
    DateTimeBcd_t   proStartTime;
    uint16          len = 0;
#ifdef AREA_HUNAN
    len = taskmng_meter_frame_make_sta_curve( Pipe, seqindex, StartTime, FrameSeqInfo,DataBuff, taskid, logicid);
    if(len>0)
        return len;
#endif

    uint8 i,j;
    uint16 lentmp = 0;
    uint8 buf[100];
    MsIndex   = FrameSeqInfo.MsIndex;       //��ǰ�߳��еĲ���������
    SubOadNum = FrameSeqInfo.DiInfo.SubOadNum;
    //LastTaskId     = pAmrThreadArg->TmnAmrInfo.CurrentTaskId;//����ʱ�ķ�����



    MeterPrtlData.DelayFlag = 0;


    MeterPrtlData.PrtlMode = FrameSeqInfo.PrtlMode;      //ͨ�Ź�Լ
    //ˢ�²ɼ�����ַ�͵���ַ
    memcpy(MeterPrtlData.MtrAddr, FrameSeqInfo.MeterAddr, 6);
    memcpy(MeterPrtlData.AcqUnitAddr, FrameSeqInfo.AcqAddr, 6);
    MeterPrtlData.MtrType = FrameSeqInfo.Mettype;


    if(MeterPrtlData.PrtlMode == PRTL_OOP)//���������Э�����֡
    {
        #if (defined AREA_JIANGXI) || (defined AREA_ANHUI)||(defined AREA_JIANGSU)
        uint16          Pn;
        Pn = FrameSeqInfo.Pn;
        OOP_METER_BASIC_T MtrAddr;
        taskmng_mtr_basic_info_get(Pn,0,&MtrAddr);
        if(MtrAddr.userType == 125)
        {
            if((FrameSeqInfo.DiInfo.MainOad.value != 0x31300200)&&(FrameSeqInfo.DiInfo.MainOad.value != 0x30710200)&&(FrameSeqInfo.DiInfo.MainOad.value != 0x30910200)&&(FrameSeqInfo.DiInfo.MainOad.value != 0x30920200))
            {
                len = taskmng_oop_perception_term_get_request(Pipe,seqindex,StartTime,MsIndex,FrameSeqInfo.DiInfo.MainOad, SubOadNum,FrameSeqInfo.DiInfo.SubOad, &MeterPrtlData.DataLen, DataBuff);
            }
            else
            {
                len = taskmng_oop_meter_get_request(Pipe,seqindex,StartTime,MsIndex,FrameSeqInfo.DiInfo.MainOad,SubOadNum, FrameSeqInfo.DiInfo.SubOad, &MeterPrtlData.DataLen, DataBuff);
            }
        }
        else
        {
            len = taskmng_oop_meter_get_request(Pipe,seqindex,StartTime,MsIndex,FrameSeqInfo.DiInfo.MainOad,SubOadNum, FrameSeqInfo.DiInfo.SubOad, &MeterPrtlData.DataLen, DataBuff);
        }
        #else
        len = taskmng_oop_meter_get_request(Pipe,seqindex,StartTime,MsIndex,FrameSeqInfo.DiInfo.MainOad,SubOadNum, FrameSeqInfo.DiInfo.SubOad, &MeterPrtlData.DataLen, DataBuff);
        #endif
    }
    else if(MeterPrtlData.PrtlMode == PRTL_188)
    {
        len = taskmng_mtsd_188_get_request(Pipe,DataBuff,MsIndex);
    }
    else
    {
        for(i=0;i<FrameSeqInfo.DiInfo.SubOadNum;i++)
        {
            for(j=0;j<FrameSeqInfo.DiInfo.diinfo[i].dicount;j++)
            {
                if(MeterPrtlData.PrtlMode == PRTL_97)
                {
                    MeterPrtlData.CtrlCode = 0x01;
                    MeterPrtlData.DataLen = 2;
                    memcpy(MeterPrtlData.DataBuff, &FrameSeqInfo.DiInfo.diinfo[i].diinfo[j], MeterPrtlData.DataLen);
                }else
                {
                    MeterPrtlData.CtrlCode = 0x11;
                    memcpy(&DI07,&FrameSeqInfo.DiInfo.diinfo[i].diinfo[j],4);
                    if(DI07 == 0x06000001 || DI07 == 0x06010001 || DI07 == 0x06020001
                            || DI07 == 0x06030001 || DI07 == 0x06040001 || (DI07&0xffff0000) == 0x06100000)     //����㳭Сʱ�������ݿ�
                        MeterPrtlData.DataLen = 10;
                    else
                        MeterPrtlData.DataLen = 4;

                    memcpy(MeterPrtlData.DataBuff, &FrameSeqInfo.DiInfo.diinfo[i].diinfo[j],4);
                    if(DI07 == 0x06000001 || DI07 == 0x06010001 || DI07 == 0x06020001
                            || DI07 == 0x06030001 || DI07 == 0x06040001 || (DI07&0xffff0000) == 0x06100000)
                    {
                        MeterPrtlData.DataLen = 10;
                        MeterPrtlData.DataBuff[4] = 1;
                        if((DI07&0xffff0000) == 0x06100000)
                            MeterPrtlData.DataBuff[4] = taskmng_curve_point_get( taskid, logicid);  //һ�γ���8����
                        DT_Time2DateTimeBCD( StartTime, &proStartTime);
                        memcpy_r(&MeterPrtlData.DataBuff[5], &proStartTime.year_l, 5);
                    }
                }
                memset(buf,0x00,sizeof(buf));
                lentmp = taskmng_meter_prtl_645_pack(buf, MeterPrtlData);
                memcpy(DataBuff+len,buf,lentmp);
                len+=lentmp;
            }
        }

    }

    if(len > MAX_MSG_LEN)
        return 0;
    return len;
}

/*******************************************************************************
* ��������: taskmng_oop_meter_get_request
* ��������: ����������֡
* �������:
* �������:
* �� �� ֵ:
*******************************************************************************/
int taskmng_oop_meter_get_request(uint8 Pipe,uint8 seqindex, time_t StartTime,int MsIndex,OOP_OAD_U MainOad,uint8 SubOadNum,OOP_OAD_U *SubOad,uint16 *FrameLen, uint8 *FrameBuff)
{
    OOP_TSA_T                   TsaAddr;
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
    OadInfoMap              OadInfo;
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[Pipe];
    OOP_METER_BASIC_T MtrAddr;
    LastTaskId     = pAmrThreadArg->TmnAmrInfo.CurrentTaskId;//����ʱ�ķ�����
    AcqType_t  Acqtype =ACQTYP_NULL;
    uint8 choiceadd = 0;        //������� ����Ƿ��Ӽ������� ��get��ʱ��choice�Ӹ�100 ��get���������⴦�������ĳ�
    taskmng_task_info_get(LastTaskId,pAmrThreadArg->TmnAmrInfo.CurrentLogicid, &pTmp_NormalLink);
    if(pTmp_NormalLink.MinTaskFlag == 1)
        choiceadd = MIN_TASK_CHOICE_ADD;

    taskmng_mtr_basic_info_get(pAmrThreadArg->pMtrAmrInfo[MsIndex].Pn,pAmrThreadArg->pMtrAmrInfo[MsIndex].logicid,&MtrAddr);
    //��ȡ����������ַ
    TsaAddr = taskmng_tsa_get(pAmrThreadArg->pMtrAmrInfo[MsIndex].Pn,pAmrThreadArg->pMtrAmrInfo[MsIndex].logicid);
    //TASK_FMT_TRACE(Pipe,REC_LOG,"mainoad %08x    \n",MainOad.value);
//#ifdef AREA_HUNAN
#if defined(AREA_HUNAN) || defined(AREA_SHANDONG)
    //������ʶ��ģ���߼��豸����3
    //��г����ѹ������5
    uint8 flag = 0;
    for(i = 0; i < SubOadNum; i++)
    {
        if(SubOad[i].nObjID == 0x4800)
        {
            flag = 3;
            break;
        }
        if(SubOad[i].nObjID == 0x2046||SubOad[i].nObjID == 0x2047)
        {
            flag = 5;
            break;

        }
    }
    if(flag!=0)
    {
        //���߼���ַ�󳤶�Ҫ��һ
        TsaAddr.len++;
        TsaAddr.flag = 1;
        TsaAddr.vxd = flag;
    }
#endif
     Acqtype = pTmp_NormalLink.AcqInfo.Acqtype;
    //������OAD��Ӧ����Ż�ȡRSD������������������OAD
    if(MainOad.nObjID != 0x0000)
    {
        if(taskmng_oad_info_get(MainOad,&OadInfo)==FALSE)
            return 0;
        if(OadInfo.class ==9)
        {
            RsdOad.value = 0x20210200;

        }
        else if(OadInfo.class ==7 || OadInfo.class ==24)   //�¼���
        {
            RsdOad.value= 0x20220200;
        }
    }

    #ifdef AREA_HUNAN
    if(MainOad.nObjID  == 0x5002 && SubOadNum == 1 && SubOad[0].value == 0x20010400 &&
            taskmng_usertype_get_from_tsa(TsaAddr, LOGIC_ID_YC)!=LOT_METER_SPRDT)
    {
        if(pTmp_NormalLink.AcqInfo.Acqtype == ACQTYP_TI)
        {
            TASK_FMT_TRACE(Pipe,REC_LOG,"pTmp_NormalLink.AcqInfo.TI.value %08x    \n",pTmp_NormalLink.AcqInfo.TI.value);
            Acqtype = ACQTYP_NULL;
        }
    }
    #endif
#ifdef AREA_SHANDONG
    //ɽ���ǵ������������ʵʱ��
    if(((MainOad.nObjID>>12)&0x0f)  != 3  )
    {
        if(taskmng_energy_oad_check(SubOad[0])==FALSE)
        {
            TASK_FMT_TRACE(Pipe,REC_LOG,"ɽ���ǵ�����ʹ��ʵʱ���� \n");
            Acqtype = ACQTYP_NULL;
        }
        if(pAmrThreadArg->pMtrAmrInfo[MsIndex].NILMModuleInfo ==0 && SubOad[0].value == 0x40310200)
        {
            Acqtype = ACQTYP_NULL;
        }
    }
#endif
    //��ȫģʽ����OAD��ʽ���ж�ȡ
    //�ز���645֡ʱ���ղ���������ͨ���� �Է��з�֡ʱ���ı�����
    if(ACQTYP_NULL == Acqtype || (TRUE == pAmrThreadArg->pMtrAmrInfo[MsIndex].IsNeedGetSafeMode))//ʵʱ���ݲ���GetRequestNormalList
    {
        offset = 0;
        if(MtrAddr.userType == 127)
        {
            InDataBuf[offset++] = SubOad[i].nIndex;
            InDataBuf[offset++] = SubOad[i].attID;
            InDataBuf[offset++] = (uint8)(SubOad[i].nObjID);
            InDataBuf[offset++] = (uint8)(SubOad[i].nObjID >> 8);
        }
        else
        {
            for(i = 0; i < SubOadNum; i++)
            {
                InDataBuf[offset++] = (uint8)(SubOad[i].nObjID >> 8);
                InDataBuf[offset++] = (uint8)(SubOad[i].nObjID);
                InDataBuf[offset++] = SubOad[i].attID;
                InDataBuf[offset++] = SubOad[i].nIndex;
            }
        }

        //#ifdef AREA_JIANGSU
        if(MtrAddr.userType == 127)
        {
            OopGet.requst(oop_pipe_get(Pipe,seqindex),&TsaAddr,1+choiceadd,InDataBuf,offset);
        }
        else
        OopGet.requst(oop_pipe_get(Pipe,seqindex),&TsaAddr,2+choiceadd,InDataBuf,offset);
        //#endif
    }
    else//������ȡ����GetRequestRecord
    {
        if(ACQTYP_LASTN == Acqtype)
        {
            rsdLen = MakeRsdFrame(Pipe,9,rsd,&pTmp_NormalLink,RsdOad,MsIndex,StartTime);
        }
        else
        {
            rsdLen = MakeRsdFrame(Pipe,2,rsd,&pTmp_NormalLink,RsdOad,MsIndex,StartTime);
        }
        GetRecord.rsd = rsd;
        GetRecord.rsdLen = rsdLen;

        GetRecord.rcsd = InDataBuf;

        offset = 1;
        GetRecord.rcsd[offset++] = 0;
        GetRecord.rcsd[offset++] = (uint8)(RsdOad.nObjID >> 8);
        GetRecord.rcsd[offset++] = (uint8)(RsdOad.nObjID);
        GetRecord.rcsd[offset++] = RsdOad.attID;
        GetRecord.rcsd[offset++] = RsdOad.nIndex;

        for(i = 0; i < SubOadNum; i++)
        {
            if(SubOad[i].nObjID != RsdOad.nObjID)
            {
                GetRecord.rcsd[offset++] = 0;

                GetRecord.rcsd[offset++] = (uint8)(SubOad[i].nObjID >> 8);
                GetRecord.rcsd[offset++] = (uint8)(SubOad[i].nObjID);
                GetRecord.rcsd[offset++] = SubOad[i].attID;
                GetRecord.rcsd[offset++] = SubOad[i].nIndex;
            }
        }

        GetRecord.rcsd[0] = (offset-1)/5;
        GetRecord.rcsdLen = offset;
        GetRecord.oad = MainOad;


        OopGet.requst(oop_pipe_get(Pipe,seqindex),&TsaAddr,3+choiceadd,(uint8*)&GetRecord,sizeof(GetRecord_t));
    }

    pLinkLayer = FindLinkLayer(oop_pipe_get(Pipe,seqindex), NULL);

    if(pLinkLayer->sendLen>MAX_MSG_LEN)
        return 0;
    *FrameLen = pLinkLayer->sendLen;
    memcpy(FrameBuff,pLinkLayer->sendBuff,*FrameLen);

    return  *FrameLen;
}

/*******************************************************************************
* ��������: MakeRsdFrame
* ��������: ��¼ѡ��������RSD��֡
* �������:
* �������:
* �� �� ֵ:
*******************************************************************************/
int MakeRsdFrame(uint8 Pipe,uint8 Selector,uint8 *RsdBuf,SCHEME_TASK_RUN_INFO *pTmp_NormalLink,OOP_OAD_U RsdOad,int MsIndex,time_t StartTime)
{
    uint8                   RsdLen = 0;
    DateTimeBcd_t           TempTimeBcd;
    time_t                  TempTimeSec;
    OOP_TI_T                    DelayTi;
    CTS_RUN_TASK_T    *RunTask = NULL;
    taskmng_run_task_get( Pipe, pTmp_NormalLink->TaskId,pTmp_NormalLink->logicid, &RunTask);
    if(RunTask ==NULL)
        return 0;
    switch(Selector)
    {
    case 0://ѡ�񷽷�0
        break;

    case 1://ѡ�񷽷�1
        break;

    case 2://ѡ�񷽷�2
    {
        RsdBuf[RsdLen++] = 2;    //ѡ�񷽷�2

        //��������������  OAD
        RsdBuf[RsdLen++] = (uint8)(RsdOad.nObjID>> 8);
        RsdBuf[RsdLen++] = (uint8)(RsdOad.nObjID);
        RsdBuf[RsdLen++] = RsdOad.attID;
        RsdBuf[RsdLen++] = RsdOad.nIndex;

        RsdBuf[RsdLen++] = DATE_TIME_S;//��ʼʱ����������
        memcpy_r((uint8*)&TempTimeBcd, (uint8*)RunTask->LastTurnBeginTime, 7);//��ʼʱ��
        //TASK_FMT_TRACE(Pipe,REC_LOG,"MakeRsdFrame LastTurnBeginTime \n");
       // WriteTimeLog( Pipe,RunTask->LastTurnBeginTime);
        printf("MakeRsdFrame LastTurnBeginTime %02x%02x%02x%02x%02x%02x%02x\n",TempTimeBcd.year_h,TempTimeBcd.year_l,TempTimeBcd.month,TempTimeBcd.day,TempTimeBcd.hour,TempTimeBcd.min,TempTimeBcd.sec);
        DT_DateTimeBCD2Time(&TempTimeBcd, &TempTimeSec);
        //�����ඳ�᲻��Ҫ��1��
        if(pTmp_NormalLink->CsdTask.RoadTask[0].MainOad.nObjID < 0x5000 || pTmp_NormalLink->CsdTask.RoadTask[0].MainOad.nObjID > 0x5003)
            TempTimeSec = TempTimeSec+60;
        else
            TempTimeSec = StartTime;
        if(pTmp_NormalLink->AcqInfo.Acqtype == ACQTYP_BC)
            TempTimeSec = StartTime;
        DT_Time2DateTimeHex(TempTimeSec, (DateTimeHex_t*)(RsdBuf+RsdLen));
        RsdLen += 7;

        RsdBuf[RsdLen++] = DATE_TIME_S;//����ʱ����������
        memcpy_r((uint8*)&TempTimeBcd,(uint8*)RunTask->ThisTurnBeginTime,7);//����ʱ��
        //TASK_FMT_TRACE(Pipe,REC_LOG,"MakeRsdFrame ThisTurnBeginTime \n");
       // WriteTimeLog( Pipe,RunTask->ThisTurnBeginTime);

        printf("MakeRsdFrame ThisTurnBeginTime %02x%02x%02x%02x%02x%02x%02x\n",TempTimeBcd.year_h,TempTimeBcd.year_l,TempTimeBcd.month,TempTimeBcd.day,TempTimeBcd.hour,TempTimeBcd.min,TempTimeBcd.sec);

        DT_DateTimeBCD2Time(&TempTimeBcd,&TempTimeSec);
        DelayTi.value = pTmp_NormalLink->DelayTime;
        DelayTi.unit = pTmp_NormalLink->DelayTimeUnit;
        DT_TimeSubInv(&TempTimeSec, DelayTi);//��ȥ����ʼʱ�����������ʱ�����ǳ��������ʵ������ʱ���޹�
        //�����ඳ�᲻��Ҫ��1��
        if(pTmp_NormalLink->CsdTask.RoadTask[0].MainOad.nObjID< 0x5000 || pTmp_NormalLink->CsdTask.RoadTask[0].MainOad.nObjID > 0x5003)
            TempTimeSec = TempTimeSec+60;
        else
        {
            TempTimeSec = StartTime;
            DelayTi.value = pTmp_NormalLink->AcqInfo.TI.value;
            DelayTi.unit = pTmp_NormalLink->AcqInfo.TI.unit;
            DT_TimeAddInv(&TempTimeSec, DelayTi);//����һ��ʱ��������ֻ��1����
            TempTimeSec = TempTimeSec-1;  //���ֱ�ɸѡ������ǰ�պ�գ�-1��ǿ��Ϊǰ�պ�
        }
        if(pTmp_NormalLink->AcqInfo.Acqtype == ACQTYP_BC)
        {
            TempTimeSec = StartTime;
            DelayTi.value = pTmp_NormalLink->AcqInfo.TI.value;
            DelayTi.unit = pTmp_NormalLink->AcqInfo.TI.unit;
            DT_TimeAddInv(&TempTimeSec, DelayTi);//����һ��ʱ��������ֻ��1����
            /* ���������ڿ��� 240109 ����ģ����1495���ֽ����� ����ֻ�ܳ�һ���*/
            /* if( pTmp_NormalLink->CsdTask.RoadTask[0].MainOad.nObjID == 0x5004)
                DT_TimeAddInv(&TempTimeSec, DelayTi); */
            TempTimeSec = TempTimeSec-1;  //���ֱ�ɸѡ������ǰ�պ�գ�-1��ǿ��Ϊǰ�պ�
        }
        DT_Time2DateTimeHex(TempTimeSec, (DateTimeHex_t*)(RsdBuf+RsdLen));
        RsdLen += 7;
        if(pTmp_NormalLink->SchemeType == EVENTPLAN)
        {
            RsdBuf[RsdLen++] = 0;
        }
        else
        {
            //���ݼ��
            RsdBuf[RsdLen++] = eTI;
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
        }
    }
    break;

    case 3://ѡ�񷽷�3
        break;

    case 4://ѡ�񷽷�4
        break;

    case 5://ѡ�񷽷�5
        break;

    case 6://ѡ�񷽷�6
        break;

    case 7://ѡ�񷽷�7
        break;

    case 8://ѡ�񷽷�8
        break;

    case 9://ѡ�񷽷�9
    {
        RsdBuf[RsdLen++] = Selector;
        RsdBuf[RsdLen++] = pTmp_NormalLink->AcqInfo.LastN;
    }
    break;

    case 10:
        break;
    default:
        return 0;
    }

    return RsdLen;
}

uint16 GetMsInfo(OOP_MSP_T  *ms,uint8 *RsdBuf,uint16 RsdLen)
{
    uint16 i=0;
    uint16 tsalen=0;
    switch (ms->choice)
    {
        case 0:
            {
                RsdBuf[RsdLen++]=0x00;
            }
        break;
        case 1:
            {
                RsdBuf[RsdLen++]=0x01;
            }
        break;
        case 2:
            {
                RsdBuf[RsdLen++]=0x02;
                if(ms->mst.size<0x80)
                {
                    RsdBuf[RsdLen++]=ms->mst.size;
                }
                else
                {
                    RsdBuf[RsdLen++]=0x82;
                    RsdBuf[RsdLen++]=ms->mst.size>>8;
                    RsdBuf[RsdLen++]=ms->mst.size;

                }
                memcpy(RsdBuf+RsdLen,ms->mst.type,ms->mst.size);
                RsdLen=RsdLen+ms->mst.size;
            }
        break;
        case 3:
            {
                RsdBuf[RsdLen++]=0x03;
                if(ms->msa.size<0x80)
                {
                    RsdBuf[RsdLen++]=ms->msa.size;
                }
                else
                {
                    RsdBuf[RsdLen++]=0x82;
                    RsdBuf[RsdLen++]=ms->msa.size>>8;
                    RsdBuf[RsdLen++]=ms->msa.size;

                }
                for(i=0;i<ms->msa.size;i++)
                {
                    tsalen=ms->msa.add[i].af&0x0F;
                    if(ms->msa.add[i].flag == 0x01)
                    {
                        RsdBuf[RsdLen++]=tsalen+3;
                        RsdBuf[RsdLen++]=ms->msa.add[i].af;
                        RsdBuf[RsdLen++]=ms->msa.add[i].vxd;
                    }
                    else
                    {
                        RsdBuf[RsdLen++]=tsalen+2;
                        RsdBuf[RsdLen++]=ms->msa.add[i].af;
                    }
                    memcpy(RsdBuf+RsdLen,(uint8*)&ms->msa.add[i].add,tsalen+1);
                    RsdLen=RsdLen+(tsalen+1);
                }

            }
        break;
        case 4:
            {
                RsdBuf[RsdLen++]=0x04;
                if(ms->msi.size<0x80)
                {
                    RsdBuf[RsdLen++]=ms->msi.size;
                }
                else
                {
                    RsdBuf[RsdLen++]=0x82;
                    RsdBuf[RsdLen++]=ms->msi.size>>8;
                    RsdBuf[RsdLen++]=ms->msi.size;

                }
                for(i=0;i<ms->msi.size;i++)
                {
                    memcpy_r(RsdBuf+RsdLen,(uint8*)&(ms->msi.idx[i]),2);
                    RsdLen=RsdLen+2;
                }
            }

        break;
        case 5:
            {
                RsdBuf[RsdLen++]=0x05;
                if(ms->msi.size<0x80)
                {
                    RsdBuf[RsdLen++]=ms->msrt.size;
                }
                else
                {
                    RsdBuf[RsdLen++]=0x82;
                    RsdBuf[RsdLen++]=ms->msrt.size>>8;
                    RsdBuf[RsdLen++]=ms->msrt.size;
                }
                for(i=0;i<ms->msrt.size;i++)
                {
                    RsdBuf[RsdLen++]=ms->msrt.region[i].nType;
                    memcpy_r(RsdBuf+RsdLen,(uint8*)&ms->msrt.region[i].start,sizeof(ms->msrt.region[i].start));
                    RsdLen =RsdLen+sizeof(ms->msrt.region[i].start);
                    memcpy_r(RsdBuf+RsdLen,(uint8*)&ms->msrt.region[i].end,sizeof(ms->msrt.region[i].end));
                    RsdLen =RsdLen+sizeof(ms->msrt.region[i].end);
                }

            }
        break;
        case 6:
            {
                RsdBuf[RsdLen++]=0x06;
                if(ms->msra.size<0x80)
                {
                    RsdBuf[RsdLen++]=ms->msra.size;
                }
                else
                {
                    RsdBuf[RsdLen++]=0x82;
                    RsdBuf[RsdLen++]=ms->msra.size>>8;
                    RsdBuf[RsdLen++]=ms->msra.size;
                }

                for(i=0;i<ms->msra.size;i++)
                {
                    RsdBuf[RsdLen++]=ms->msra.region[i].nType;
                    tsalen=ms->msra.region[i].start.af&0x0F;
                    if(ms->msra.region[i].start.flag == 0x01)
                    {
                        RsdBuf[RsdLen++]=tsalen+3;
                        RsdBuf[RsdLen++]=ms->msra.region[i].start.af;
                        RsdBuf[RsdLen++]=ms->msra.region[i].start.vxd;
                    }
                    else
                    {
                        RsdBuf[RsdLen++]=tsalen+2;
                        RsdBuf[RsdLen++]=ms->msra.region[i].start.af;
                    }
                    memcpy(RsdBuf+RsdLen,(uint8*)&ms->msra.region[i].start.add,tsalen+1);
                    RsdLen=RsdLen+(tsalen+1);
                    tsalen=ms->msra.region[i].end.af&0x0F;
                    if(ms->msra.region[i].end.flag == 0x01)
                    {
                        RsdBuf[RsdLen++]=tsalen+3;
                        RsdBuf[RsdLen++]=ms->msra.region[i].end.af;
                        RsdBuf[RsdLen++]=ms->msra.region[i].end.vxd;
                    }
                    else
                    {
                        RsdBuf[RsdLen++]=tsalen+2;
                        RsdBuf[RsdLen++]=ms->msra.region[i].end.af;
                    }
                    memcpy(RsdBuf+RsdLen,(uint8*)&ms->msra.region[i].end.add,tsalen+1);
                    RsdLen=RsdLen+(tsalen+1);


                }
            }
        break;
        case 7:
        {
            RsdBuf[RsdLen++]=0x06;
            if(ms->msi.size<0x80)
            {
                RsdBuf[RsdLen++]=ms->msri.size;
            }
            else
            {
                RsdBuf[RsdLen++]=0x82;
                RsdBuf[RsdLen++]=ms->msri.size>>8;
                RsdBuf[RsdLen++]=ms->msri.size;
            }
            for(i=0;i<ms->msrt.size;i++)
            {
                RsdBuf[RsdLen++]=ms->msri.region[i].nType;
                memcpy_r(RsdBuf+RsdLen,(uint8*)&ms->msri.region[i].start,sizeof(ms->msri.region[i].start));
                RsdLen =RsdLen+sizeof(ms->msri.region[i].start);
                memcpy_r(RsdBuf+RsdLen,(uint8*)&ms->msri.region[i].end,sizeof(ms->msri.region[i].end));
                RsdLen =RsdLen+sizeof(ms->msri.region[i].end);

            }
        }
    break;

    }
    return RsdLen;
}

/*******************************************************************************
* ��������: MakeRsdFrame
* ��������: ��¼ѡ��������RSD��֡
* �������:
* �������:
* �� �� ֵ:
*******************************************************************************/
int MakeRsd(uint8 Pipe,uint8 Selector,uint8 *RsdBuf,SCHEME_TASK_RUN_INFO *pTmp_NormalLink,OOP_OAD_U RsdOad,int MsIndex,time_t StartTime,OOP_MSP_T   *ms)
{
    uint16                   RsdLen = 0;
    DateTimeBcd_t           TempTimeBcd;
    time_t                  TempTimeSec;
    OOP_TI_T                    DelayTi;
    CTS_RUN_TASK_T    *RunTask = NULL;

    taskmng_run_task_get( Pipe, pTmp_NormalLink->TaskId,pTmp_NormalLink->logicid, &RunTask);
    if(RunTask ==NULL)
        return 0;
    switch(Selector)
    {
    case 0://ѡ�񷽷�0
        break;

    case 1://ѡ�񷽷�1
        break;

    case 2://ѡ�񷽷�2
    {
        RsdBuf[RsdLen++] = 2;    //ѡ�񷽷�2

        //��������������  OAD
        RsdBuf[RsdLen++] = (uint8)(RsdOad.nObjID>> 8);
        RsdBuf[RsdLen++] = (uint8)(RsdOad.nObjID);
        RsdBuf[RsdLen++] = RsdOad.attID;
        RsdBuf[RsdLen++] = RsdOad.nIndex;

        RsdBuf[RsdLen++] = DATE_TIME_S;//��ʼʱ����������
        memcpy_r((uint8*)&TempTimeBcd, (uint8*)RunTask->LastTurnBeginTime, 7);//��ʼʱ��
        //TASK_FMT_TRACE(Pipe,REC_LOG,"MakeRsdFrame LastTurnBeginTime \n");
       // WriteTimeLog( Pipe,RunTask->LastTurnBeginTime);
        printf("MakeRsdFrame LastTurnBeginTime %02x%02x%02x%02x%02x%02x%02x\n",TempTimeBcd.year_h,TempTimeBcd.year_l,TempTimeBcd.month,TempTimeBcd.day,TempTimeBcd.hour,TempTimeBcd.min,TempTimeBcd.sec);
        DT_DateTimeBCD2Time(&TempTimeBcd, &TempTimeSec);
        //�����ඳ�᲻��Ҫ��1��
        if(pTmp_NormalLink->CsdTask.RoadTask[0].MainOad.nObjID < 0x5000 || pTmp_NormalLink->CsdTask.RoadTask[0].MainOad.nObjID > 0x5003)
            TempTimeSec = TempTimeSec+60;
        else
            TempTimeSec = StartTime;
        DT_Time2DateTimeHex(TempTimeSec, (DateTimeHex_t*)(RsdBuf+RsdLen));
        RsdLen += 7;

        RsdBuf[RsdLen++] = DATE_TIME_S;//����ʱ����������
        memcpy_r((uint8*)&TempTimeBcd,(uint8*)RunTask->ThisTurnBeginTime,7);//����ʱ��
        //TASK_FMT_TRACE(Pipe,REC_LOG,"MakeRsdFrame ThisTurnBeginTime \n");
       // WriteTimeLog( Pipe,RunTask->ThisTurnBeginTime);

        printf("MakeRsdFrame ThisTurnBeginTime %02x%02x%02x%02x%02x%02x%02x\n",TempTimeBcd.year_h,TempTimeBcd.year_l,TempTimeBcd.month,TempTimeBcd.day,TempTimeBcd.hour,TempTimeBcd.min,TempTimeBcd.sec);

        DT_DateTimeBCD2Time(&TempTimeBcd,&TempTimeSec);
        DelayTi.value = pTmp_NormalLink->DelayTime;
        DelayTi.unit = pTmp_NormalLink->DelayTimeUnit;
        DT_TimeSubInv(&TempTimeSec, DelayTi);//��ȥ����ʼʱ�����������ʱ�����ǳ��������ʵ������ʱ���޹�
        //�����ඳ�᲻��Ҫ��1��
        if(pTmp_NormalLink->CsdTask.RoadTask[0].MainOad.nObjID< 0x5000 || pTmp_NormalLink->CsdTask.RoadTask[0].MainOad.nObjID > 0x5003)
            TempTimeSec = TempTimeSec+60;
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
        if(pTmp_NormalLink->SchemeType == EVENTPLAN)
        {
            RsdBuf[RsdLen++] = 0;
        }
        else
        {
            //���ݼ��
            RsdBuf[RsdLen++] = eTI;
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
        }
    }
    break;

    case 3://ѡ�񷽷�3
        break;

    case 4://ѡ�񷽷�4
        {
            RsdBuf[RsdLen++] = 4;    //ѡ�񷽷�2
            memcpy_r((uint8*)&TempTimeBcd, (uint8*)RunTask->LastTurnBeginTime, 7);//��ʼʱ��

            printf("MakeRsdFrame LastTurnBeginTime %02x%02x%02x%02x%02x%02x%02x\n",TempTimeBcd.year_h,TempTimeBcd.year_l,TempTimeBcd.month,TempTimeBcd.day,TempTimeBcd.hour,TempTimeBcd.min,TempTimeBcd.sec);
            DT_DateTimeBCD2Time(&TempTimeBcd, &TempTimeSec);
            //�����ඳ�᲻��Ҫ��1��
            if(pTmp_NormalLink->CsdTask.RoadTask[0].MainOad.nObjID < 0x5000 || pTmp_NormalLink->CsdTask.RoadTask[0].MainOad.nObjID > 0x5003)
                TempTimeSec = TempTimeSec+60;
            else
                TempTimeSec = StartTime;
            DT_Time2DateTimeHex(TempTimeSec, (DateTimeHex_t*)(RsdBuf+RsdLen));
            RsdLen += 7;
        }
        RsdLen = GetMsInfo(ms,RsdBuf,RsdLen);
        break;
    case 5://ѡ�񷽷�5
        {
            RsdBuf[RsdLen++] = 5;    //ѡ�񷽷�2
            memcpy_r((uint8*)&TempTimeBcd, (uint8*)RunTask->LastTurnBeginTime, 7);//��ʼʱ��

            printf("MakeRsdFrame LastTurnBeginTime %02x%02x%02x%02x%02x%02x%02x\n",TempTimeBcd.year_h,TempTimeBcd.year_l,TempTimeBcd.month,TempTimeBcd.day,TempTimeBcd.hour,TempTimeBcd.min,TempTimeBcd.sec);
            DT_DateTimeBCD2Time(&TempTimeBcd, &TempTimeSec);
            //�����ඳ�᲻��Ҫ��1��
            if(pTmp_NormalLink->CsdTask.RoadTask[0].MainOad.nObjID < 0x5000 || pTmp_NormalLink->CsdTask.RoadTask[0].MainOad.nObjID > 0x5003)
                TempTimeSec = TempTimeSec+60;
            else
                TempTimeSec = StartTime;
            DT_Time2DateTimeHex(TempTimeSec, (DateTimeHex_t*)(RsdBuf+RsdLen));
            RsdLen += 7;

            RsdLen = GetMsInfo(ms,RsdBuf,RsdLen);

        }
        break;

    case 6://ѡ�񷽷�6
        break;

    case 7://ѡ�񷽷�7
        {
            RsdBuf[RsdLen++] = 7;    //ѡ�񷽷�2


            memcpy_r((uint8*)&TempTimeBcd, (uint8*)RunTask->LastTurnBeginTime, 7);//��ʼʱ��

            //�����ඳ�᲻��Ҫ��1��
            if(pTmp_NormalLink->CsdTask.RoadTask[0].MainOad.nObjID < 0x5000 || pTmp_NormalLink->CsdTask.RoadTask[0].MainOad.nObjID > 0x5003)
                TempTimeSec = TempTimeSec+60;
            else
                TempTimeSec = StartTime;
            DT_Time2DateTimeHex(TempTimeSec, (DateTimeHex_t*)(RsdBuf+RsdLen));
            RsdLen += 7;

            memcpy_r((uint8*)&TempTimeBcd,(uint8*)RunTask->ThisTurnBeginTime,7);//����ʱ��

            DT_DateTimeBCD2Time(&TempTimeBcd,&TempTimeSec);
            DelayTi.value = pTmp_NormalLink->DelayTime;
            DelayTi.unit = pTmp_NormalLink->DelayTimeUnit;
            DT_TimeSubInv(&TempTimeSec, DelayTi);//��ȥ����ʼʱ�����������ʱ�����ǳ��������ʵ������ʱ���޹�
            //�����ඳ�᲻��Ҫ��1��
            if(pTmp_NormalLink->CsdTask.RoadTask[0].MainOad.nObjID< 0x5000 || pTmp_NormalLink->CsdTask.RoadTask[0].MainOad.nObjID > 0x5003)
                TempTimeSec = TempTimeSec+60;
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
            if(pTmp_NormalLink->SchemeType == EVENTPLAN)
            {
                RsdBuf[RsdLen++] = 0;
            }
            else
            {
                //���ݼ��
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
            }

            RsdLen = GetMsInfo(ms,RsdBuf,RsdLen);
        }
        break;

    case 8://ѡ�񷽷�8
        break;

    case 9://ѡ�񷽷�9
    {
        RsdBuf[RsdLen++] = Selector;
        RsdBuf[RsdLen++] = pTmp_NormalLink->AcqInfo.LastN;
    }
    break;

    case 10:
    {
        RsdBuf[RsdLen++] = Selector;
        RsdBuf[RsdLen++] = pTmp_NormalLink->AcqInfo.LastN;
        RsdLen = GetMsInfo(ms,RsdBuf,RsdLen);
    }
        break;
    default:
        return 0;
    }

    return RsdLen;
}

/*******************************************************************************
* ��������: taskmng_meter_prtl_645_pack
* ��������: ����Լ��֡
* �������: MeterData           ����Լ����
* �������: PrtlData            ��֡������
* �� �� ֵ:  ֡����
*******************************************************************************/
int taskmng_meter_prtl_645_pack(uint8 *PrtlData, MeterPrtlFmt_T MeterData)
{
    uint8   CheckSum = 0;                       //У���
    int     i;
    uint8   CurIndex;                           //����

    PrtlData[0] = 0x68;
    memcpy(&PrtlData[1], MeterData.MtrAddr, 6);
    PrtlData[7] = 0x68;
    PrtlData[8] = MeterData.CtrlCode;
    PrtlData[9] = MeterData.DataLen;

    for (i=0; i < MeterData.DataLen; i++)
    {
        PrtlData[10+i] = MeterData.DataBuff[i] + 0x33;
    }
    CurIndex = MeterData.DataLen + 10;

    for (i = 0; i < CurIndex; i++)
    {
        CheckSum += PrtlData[i];
    }
    PrtlData[CurIndex] = CheckSum;
    PrtlData[CurIndex+1] = 0x16;

    CurIndex += 2;

    return CurIndex;
}




/*******************************************************************************
* ��������: taskmng_paral_meter_read_data_unpack
* ��������: �����е�645֡
* �������: AppData
* �������: MeterPrtlData
* �� �� ֵ: 0   ������֡(���ݳ��Ȳ�Ϊ0��)
            -1  ��
*******************************************************************************/

int taskmng_paral_meter_read_data_unpack(uint8 Pipe,uint8 seqindex, uint8 logicid,AMR_PLC_INFO_REPORT_T AmrReportInfo, MeterPrtlFmt_T *MeterPrtlData, uint16 *MtrIndex,char *apdu, FRAME_SEQ_INFO_T FrameSeqInfo)
{
    int             ret = -1;
    int             rettmp = -1;
    int16          index;
    AppLayerList_t* pAppLayer = NULL;
    //OOP_TSA_T tsa;
    uint8 framenum = 0;
    uint16 framelen = 0;
    if(AmrReportInfo.nDataBufLen==0)
        return -1;
    //645��֡����
    //TASK_BUF_DEBUG(Pipe, REC_LOG,AmrReportInfo.pDataBuf,AmrReportInfo.nDataBufLen);
    gReponsePIIDCAD = AmrReportInfo.pDataBuf[16];
    //TASK_FMT_DEBUG(Pipe, REC_LOG,"gReponsePIIDCAD is  %d \n",gReponsePIIDCAD);
    //tsa=taskmng_tsa_get(FrameSeqInfo.Pn,logicid);
    ret = taskmng_645_meter_data_unpack(AmrReportInfo.nDataBufLen, AmrReportInfo.pDataBuf
        , MeterPrtlData->MtrAddr, &MeterPrtlData->CtrlCode, &MeterPrtlData->DataLen, MeterPrtlData->DataBuff);
    //�������645֡��ʽʧ��,��ô����698.45��Э����н���
    if(CHECK_RESULT_FAILUER == ret)
    {
        ret = LinkDataReceive(oop_pipe_get(Pipe,seqindex), AmrReportInfo.pDataBuf, AmrReportInfo.nDataBufLen,NULL);
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
        MeterPrtlData->Pn=taskmng_pn_get_from_addr(MeterPrtlData->MtrAddr,0);
        //TASK_BUF_DEBUG(Pipe, REC_LOG,MeterPrtlData->MtrAddr,6);
        MeterPrtlData->DataLen = pAppLayer->recvApduList->apduLen-pAppLayer->recvApduList->offSet;
        memcpy(MeterPrtlData->DataBuff, pAppLayer->recvApduList->apdu+pAppLayer->recvApduList->offSet, MeterPrtlData->DataLen);
        memcpy(apdu,pAppLayer->recvApduList->apdu,2);
        //TASK_BUF_DEBUG(Pipe, REC_LOG,apdu,2);
        //TASK_BUF_DEBUG(Pipe, REC_LOG,MeterPrtlData->DataBuff,MeterPrtlData->DataLen);
        if((apdu[1]==0x01)&&(apdu[0]==0x85))
        {
            MeterPrtlData->DataLen = pAppLayer->recvApduList->apduLen-pAppLayer->recvApduList->offSet+4;
            memcpy(MeterPrtlData->DataBuff, pAppLayer->recvApduList->apdu+pAppLayer->recvApduList->offSet-4, MeterPrtlData->DataLen);

        }
        #if (defined AREA_JIANGXI) || (defined AREA_ANHUI)||(defined AREA_JIANGSU)
        OOP_OAD_U Tempoad;
        Tempoad.value=bin2_int32u(pAppLayer->recvApduList->apdu+3);
        if(Tempoad.value==0x60120300)
        {
            MeterPrtlData->Istasktable = 1;
        }
        else
        {
            MeterPrtlData->Istasktable = 0;
        }
        #endif
    }else
    {
        //����645���ĺ�������м��
        MeterPrtlData->frameinfo[framenum].result = ret;
        MeterPrtlData->frameinfo[framenum].len = MeterPrtlData->DataLen;
        memcpy(MeterPrtlData->frameinfo[framenum].buf,MeterPrtlData->DataBuff,MeterPrtlData->DataLen);
        framenum++;
        framelen = MeterPrtlData->DataLen+12;   //645�����ܳ���
        while(AmrReportInfo.nDataBufLen-framelen>0)
        {
            rettmp = taskmng_645_meter_data_unpack(AmrReportInfo.nDataBufLen-framelen,AmrReportInfo.pDataBuf+framelen,
                MeterPrtlData->MtrAddr, &MeterPrtlData->CtrlCode, &MeterPrtlData->DataLen, MeterPrtlData->DataBuff);
            if(rettmp != CHECK_RESULT_FAILUER)
            {
                MeterPrtlData->frameinfo[framenum].result = rettmp;
                MeterPrtlData->frameinfo[framenum].len = MeterPrtlData->DataLen;
                memcpy(MeterPrtlData->frameinfo[framenum].buf,MeterPrtlData->DataBuff,MeterPrtlData->DataLen);
                framenum++;
                framelen += MeterPrtlData->DataLen+12;
            }else
            {
                break;
            }
        }
        MeterPrtlData->framenum = framenum;

    }


    index = taskmng_ms_index_get( Pipe,AmrReportInfo.nMeterFilesID,logicid);

    if(index>=0)
        *MtrIndex = index;
    else
        return -1;
    MeterPrtlData->PrtlMode = AmrReportInfo.nProtocolType;


    return ret;
}





/*******************************************************************************
* ��������: taskmng_paral_meter_read_data_unpack
* ��������: �����е�645֡
* �������: AppData
* �������: MeterPrtlData
* �� �� ֵ: 0   ������֡(���ݳ��Ȳ�Ϊ0��)
            -1  ��
*******************************************************************************/

int taskmng_paral_caj_read_data_unpack(uint8 Pipe,uint8 seqindex, uint8 logicid,AMR_PLC_INFO_REPORT_T AmrReportInfo, MeterPrtlFmt_T *MeterPrtlData, uint16 *MtrIndex,char *apdu, FRAME_SEQ_INFO_T FrameSeqInfo)
{
    int             ret = -1;
    int16          index;
    AppLayerList_t* pAppLayer = NULL;
    uint8 MtrAddr[6];
    uint8 prtl;
    //OOP_TSA_T tsa;

    if(AmrReportInfo.nDataBufLen==0)
        return -1;
    //645��֡����
    TASK_BUF_DEBUG(Pipe, REC_LOG,AmrReportInfo.pDataBuf,AmrReportInfo.nDataBufLen);
    gReponsePIIDCAD = AmrReportInfo.pDataBuf[16];
    TASK_FMT_DEBUG(Pipe, REC_LOG,"gReponsePIIDCAD is  %d \n",gReponsePIIDCAD);
    //tsa=taskmng_tsa_get(FrameSeqInfo.Pn,logicid);
    ret = taskmng_645_meter_data_unpack(AmrReportInfo.nDataBufLen, AmrReportInfo.pDataBuf
        , MeterPrtlData->MtrAddr, &MeterPrtlData->CtrlCode, &MeterPrtlData->DataLen, MeterPrtlData->DataBuff);

    //�������645֡��ʽʧ��,��ô����698.45��Э����н���
    if(CHECK_RESULT_FAILUER == ret)
    {
        ret = LinkDataReceive(oop_pipe_get(Pipe,seqindex), AmrReportInfo.pDataBuf, AmrReportInfo.nDataBufLen,NULL);
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
        MeterPrtlData->Pn=taskmng_pn_get_from_addr(MeterPrtlData->MtrAddr,0);
        TASK_BUF_DEBUG(Pipe, REC_LOG,MeterPrtlData->MtrAddr,6);
        memcpy(apdu,pAppLayer->recvApduList->apdu,2);
        TASK_BUF_DEBUG(Pipe, REC_LOG,apdu,2);
        if((apdu[0]==0x88)&&(apdu[1]==0x02))
        {
            pAppLayer->recvApduList->offSet =0;
        }
        if((apdu[0]==0x88)&&(apdu[1]==0x06))
        {
            pAppLayer->recvApduList->offSet =0;
        }
        MeterPrtlData->DataLen = pAppLayer->recvApduList->apduLen-pAppLayer->recvApduList->offSet;
        memcpy(MeterPrtlData->DataBuff, pAppLayer->recvApduList->apdu+pAppLayer->recvApduList->offSet, MeterPrtlData->DataLen);
        TASK_BUF_DEBUG(Pipe, REC_LOG,MeterPrtlData->DataBuff,MeterPrtlData->DataLen);
        if((apdu[1]==0x01)&&(apdu[0]==0x85))
        {
            MeterPrtlData->DataLen = pAppLayer->recvApduList->apduLen-pAppLayer->recvApduList->offSet+4;
            memcpy(MeterPrtlData->DataBuff, pAppLayer->recvApduList->apdu+pAppLayer->recvApduList->offSet-4, MeterPrtlData->DataLen);

        }
        #if (defined AREA_JIANGXI) || (defined AREA_ANHUI)||(defined AREA_JIANGSU)
        OOP_OAD_U Tempoad;
        Tempoad.value=bin2_int32u(pAppLayer->recvApduList->apdu+3);
        if(Tempoad.value==0x60120300)
        {
            MeterPrtlData->Istasktable = 1;
        }
        else
        {
            MeterPrtlData->Istasktable = 0;
        }
        #endif
    }


    index = taskmng_caj_addr_prtl_get( AmrReportInfo.nMeterFilesID,MtrAddr,&prtl);

    if(index>=0)
        *MtrIndex = index;
    else
        return -1;
    MeterPrtlData->PrtlMode = prtl;


    return ret;
}



/*******************************************************************************
* ��������: taskmng_645_meter_data_unpack
* ��������: ����Լ��֡
* �������: FrameLen            ֡����
*           FrameBuff           ֡����
* �������: MtrAddr             ����ַ
*           DataLen             �����򳤶�
*           DataBuff            ����������
*           CtrlCode            ������
* �� �� ֵ: CHECK_RESULT_SUCCEED
*           CHECK_RESULT_FAILUER
*           CHECK_RESULT_CONFIRM
*           CHECK_RESULT_DENY
*******************************************************************************/
int8 taskmng_645_meter_data_unpack(uint16 FrameLen, uint8 *FrameBuff, uint8 *MtrAddr , uint8 *CtrlCode, uint16 *DataLen, uint8 *DataBuff)
{
    uint16  DataIndex = 0xff;                   //ָ��ƫ��
    uint8   CS = 0;
    uint16   i;//start Common Problem

    //����ƫ�ƣ���λ֡ͷ
    for (i = 0; i < FrameLen; i++)
    {
        if ((FrameBuff[i] == 0x68) && (FrameBuff[i+7] == 0x68))
        {
            DataIndex = i;
            break;
        }
    }

    //δ��λ��֡ͷ����Ϊʧ��
    if (DataIndex == 0xff)
    {
        return CHECK_RESULT_FAILUER;
    }

    //ˢ�µ�ַ��
    memcpy(MtrAddr, &FrameBuff[DataIndex+1], 6);

    //������
    *CtrlCode = FrameBuff[DataIndex+8];

    //ˢ�����ݳ���
    *DataLen = FrameBuff[DataIndex+9];

    //�ж�֡β
    if (FrameBuff[(DataIndex + *DataLen + 11)] != 0x16)
    {
        return CHECK_RESULT_FAILUER;
    }

    //�ж�У���
    for (i=0; i < (*DataLen + 10); i++)
    {
        CS += FrameBuff[DataIndex+i];
    }
    if (CS != FrameBuff[(DataIndex + *DataLen + 10)])
    {
        return CHECK_RESULT_FAILUER;
    }

    //ˢ�����������ݣ�ͬʱ����3�봦��
    for (i = 0; i < *DataLen; i++)
    {
        DataBuff[i] = FrameBuff[DataIndex+10+i] - 0x33;
    }


    //�ж��Ƿ��Ƿ���֡
    if (*CtrlCode & 0x40)
    {
        return CHECK_RESULT_DENY;
    }

    //�ж��Ƿ���ȷ��֡
    if (*DataLen == 0)
    {
        return CHECK_RESULT_CONFIRM;
    }
    if(*CtrlCode & 0x20) //�к���֡
    {
        AddEventReport(MtrAddr);
    }
    return CHECK_RESULT_SUCCEED;
}





/*******************************************************************************
* ��������: taskmng_645_meter_data_unpack
* ��������: ����Լ��֡
* �������: FrameLen            ֡����
*           FrameBuff           ֡����
* �������: MtrAddr             ����ַ
*           DataLen             �����򳤶�
*           DataBuff            ����������
*           CtrlCode            ������
* �� �� ֵ: CHECK_RESULT_SUCCEED
*           CHECK_RESULT_FAILUER
*           CHECK_RESULT_CONFIRM
*           CHECK_RESULT_DENY
*******************************************************************************/
int8 taskmng_188_meter_data_unpack(uint16 FrameLen, uint8 *FrameBuff, uint8 *MtrAddr , uint8 *CtrlCode, uint16 *DataLen, uint8 *DataBuff,uint16 DI)
{

    uint16  head_offset = 0; /* �������� */
    uint8   CS = 0;
    uint8   i;
    uint8   data_len = 0;


    while((head_offset + AMR_MT_PRTL_MIN_LEN_188 - 1) < FrameLen)
    {
        /* 1 ��֡��ʼ��־0x68��ˮ������*/
        if ((FrameBuff[head_offset] != 0x68))
        {
            head_offset ++;
            continue;
        }
        /* 2 ��֡���� */
        if((FrameLen - head_offset) < AMR_MT_PRTL_MIN_LEN_188)
        {
            //memset(FrameBuff, 0, FrameLen);
            return AMR_CHECK_RESULT_FAILUER_188;
        }

        data_len =  FrameBuff[head_offset + 10];         //188��Լ�����򳤶� ��֡��ʼ��־���10���ֽ�

        /* 3 �н�β0x16 */
        if (FrameBuff[head_offset + data_len + 12] != 0x16)
        {
            head_offset ++;
            continue;
        }

        /* 4 ��У��� */
        for (i=0; i < (data_len + 11); i++)
        {
            CS += FrameBuff[head_offset+i];
        }
        if (CS != FrameBuff[head_offset + data_len + 11])
        {
            //memset(FrameBuff, 0, FrameLen);
            return AMR_CHECK_RESULT_FAILUER_188;
        }

        /* 6 �ߵ�����˵��֡�Ѿ���ȷ */

        /* 6.1 ˢ�µ�ַ�� */
         //ˢ�µ�ַ��
        memcpy(MtrAddr, &FrameBuff[head_offset+2], 7);


        /* 6.2 ˢ�¿�����*/
        *CtrlCode = FrameBuff[head_offset+9];
        if((*CtrlCode & 0x80) == 0x00)
        {
            /* DataIndex + ���ݳ��� + ��Լ68��LEN�ĳ��� + CS��16 */
            head_offset = head_offset + data_len + 12 + 1;
            CS = 0;
            continue;
        }

        /* 6.5 �ж��Ƿ��� ����֡*/
        if(*CtrlCode & 0x40)
        {
            //memset(FrameBuff,0,FrameLen);
            return AMR_CHECK_RESULT_DENY_188;
        }

        /* 6.5 ��ͨ���������жϷ��ͺͽ��ܵ�֡��ʶ�Ƿ���ͬ �����ͬ����֡ʧ�ܴ���*/
        if(DI != *(uint16*)(FrameBuff+head_offset+11))
        {
            //memset(FrameBuff,0,FrameLen);
            return AMR_CHECK_RESULT_FAILUER_188;
        }

        memcpy(DataBuff,FrameBuff+head_offset,FrameLen-head_offset);
        *DataLen = FrameLen-head_offset;

        return AMR_CHECK_RESULT_SUCCEED_188;
    }

    return AMR_CHECK_RESULT_FAILUER_188;


}


/**********************************************************
���ƣ� bin2_int32u

��������:��1�ֽ�ת��4�ֽڣ�

��������ֵ:
��������ֵ:��
*************************************************************/

uint32 bin2_int32u(uint8 bin[4])
{
   int16 idx;
   uint32 val;

   val = 0;
   for(idx=0;idx<=3;idx++)
   {
       val<<=8;
       val += bin[idx];
   }
   return val;
}


/*******************************************************************************
* ��������: taskmng_mtr_result_check
* ��������: //���ÿ��������index��Ӧ�ı��Ƿ񳭶��ɹ�
* �������: index �������
        MtrResult  ��¼������������
* �������:
* �� �� ֵ: TRUE �ñ��ɹ�
        FALSE   �ñ�ʧ��
*******************************************************************************/
BOOL taskmng_mtr_result_check(uint16 index,uint8 *MtrResult)
{
    uint8 i,j;
    i = index/8;
    j = index%8;
    if(((MtrResult[i] >>j )& 1)==1)
        return TRUE;
    return FALSE;
}


/*******************************************************************************
* ��������: taskmng_mtr_result_check
* ��������: //���ÿ��������index��Ӧ�ı��Ƿ񳭶��ɹ�
* �������: index �������
        MtrResult  ��¼������������
* �������:
* �� �� ֵ: TRUE �ñ��ɹ�
        FALSE   �ñ�ʧ��
*******************************************************************************/
uint16 taskmng_mtr_meter_patch(uint8 datalen,uint8 *MtrResult)
{
    uint8 i,j;

    for(i=0;i<datalen;i++)
    {
        for(j=0;j<8;j++)
        {
            if((MtrResult[i]&(0x01>>j))==0x01)
            {
                return i*8+j;
            }
        }
    }
    return 0xFFFF;
}

/*******************************************************************************
* ��������: taskmng_mtr_result_set
* ��������: //��ָ������λ��Ϊ����ɹ�
* �������: index �������
        MtrResult  ��¼������������
* �������:
* �� �� ֵ:
*******************************************************************************/
void taskmng_mtr_result_set(uint16 index,uint8 *MtrResult)
{
    uint8 i,j;
    i = index/8;
    j = index%8;
    MtrResult[i]|=(1<<j);
}





/*******************************************************************************
* ��������: CreatPatchFile
* ��������: //���������ļ�
* �������: Pipe �˿ں�
        DayCnt Ϊ0��ʾ������ļ� 1��ʾ��һ��� �������
* �������:
* �� �� ֵ:
*******************************************************************************/
void CreatPatchFile(uint8 Pipe,uint8 DayCnt)
{

    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[Pipe];
    uint8 i;

    //pthread_rwlock_rdlock(&TaskRWLock);
    for(i=0;i<pAmrThreadArg->TaskNum;i++)
    {

        if(SchemeTaskRunQueue[i].SchemeType ==EVENTPLAN)  //�¼�������
            continue;

        //������������
        if(SchemeTaskRunQueue[i].CsdTask.RoadTask[0].MainOad.nObjID==0x5002
            ||SchemeTaskRunQueue[i].CsdTask.RoadTask[0].MainOad.nObjID==0x5003
            ||SchemeTaskRunQueue[i].CsdTask.RoadTask[0].MainOad.nObjID==0x5004)
        {
            CreatPatchFile_TaskId(Pipe,&SchemeTaskRunQueue[i],DayCnt);
        }
    }

    //pthread_rwlock_unlock(&TaskRWLock);
}


/*******************************************************************************
* ��������: ReOrderPatchFile
* ��������: //���²����ļ���ʱ��
* �������: Pipe �˿ں�

* �������:
* �� �� ֵ:
*******************************************************************************/
void ReOrderPatchFile(uint8 Pipe)
{
    char    pfile[100] = {0};
    char    pfiletmp[100]={0};
    char    pfiletmppath[100]={0};
    char    histime[3]={0};
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[Pipe];
    uint8 DayCnt = 0;
    int  i;
    int     Cnt;
    uint16 TaskId;
    DateTime_T RealBinTime;
    DateTime_T HistoryBinTime;

    taskmng_cur_bin_time_get(&RealBinTime);
    RealBinTime.Hour=0;
    RealBinTime.Minute =0;
    RealBinTime.Second =0;

    TASK_FMT_TRACE(Pipe, REC_LOG, "ReOrderPatchFile ���²����ļ���\n");
       //ÿ���������
    for(i=0;i<pAmrThreadArg->TaskNum;i++)
    {
        if(pAmrThreadArg->pTaskRunInfo[i].logicid!=LOGIC_ID_YC)
            continue;
        TaskId = pAmrThreadArg->pTaskRunInfo[i].TaskId;
        for(DayCnt =0;DayCnt<PATCH_MAX_DAY;DayCnt++)
        {
            memset(pfile,0x00,sizeof(pfile));
            memset(pfiletmppath,0x00,sizeof(pfiletmppath));
            sprintf(pfile,"%s/%s%03d_%02d_PIPE%02d",PrivateDir,PATCH_FILE,TaskId,DayCnt,Pipe);
            TASK_FMT_TRACE(Pipe, REC_LOG, "�����ļ� %s \n",pfile);
            if(access(pfile,R_OK)==0)
            {
                //sprintf(cmdbuf,"mv /mnt/data/private/%s%03d_%02d_PIPE%02d /mnt/data/private/%s%03d_%02d_PIPE%02d",PATCH_FILE,TaskId,DayCnt,Pipe,PATCH_FILE_TMP,TaskId,DayCnt,Pipe);
                sprintf(pfiletmppath,"%s/%s%03d_%02d_PIPE%02d",PrivateDir,PATCH_FILE_TMP,TaskId,DayCnt,Pipe);
                //system(cmdbuf);
                rename(pfile,pfiletmppath);
                TASK_FMT_TRACE(Pipe, REC_LOG, "�ļ����� ��ʱ���� \n");
            }
        }

        for(DayCnt=0;DayCnt<PATCH_MAX_DAY;DayCnt++)
        {
            memset(pfile,0x00,sizeof(pfile));
            memset(pfiletmp,0x00,sizeof(pfiletmp));
            memset(pfiletmppath,0x00,sizeof(pfiletmppath));
            sprintf(pfile,"%s/%s%03d_%02d_PIPE%02d",PrivateDir,PATCH_FILE_TMP,TaskId,DayCnt,Pipe);
            sprintf(pfiletmp,"%s/%s%03d_%02d_PIPE%02d",PrivateDir,PATCH_FILE_TMP,TaskId,DayCnt,Pipe);
            if(access(pfiletmp,R_OK)==0)
            {
                if(read_pdata_xram(pfile,(char*)histime,0,3)<0)
                {
                    remove(pfiletmp);
                    continue;
                }
                memset ( &HistoryBinTime,0x0,sizeof ( HistoryBinTime ) );
                HistoryBinTime.Year  = BcdToHex ( histime[2] );
                HistoryBinTime.Month = BcdToHex ( histime[1] );
                HistoryBinTime.Day   = BcdToHex ( histime[0] );
                Cnt = GetDistanceBinTimeByDay ( HistoryBinTime, RealBinTime );
                TASK_FMT_TRACE(Pipe, REC_LOG, "Cnt %d\n",Cnt);
                if(Cnt>=0 && Cnt<PATCH_MAX_DAY)
                {
                    sprintf(pfiletmppath,"%s/%s%03d_%02d_PIPE%02d",PrivateDir,PATCH_FILE,TaskId,Cnt,Pipe);
                    rename(pfiletmp,pfiletmppath);
                    TASK_FMT_TRACE(Pipe, REC_LOG, "%s ������Ϊ %s\n",pfiletmp,pfiletmppath);
                }else
                {
                    TASK_FMT_TRACE(Pipe, REC_LOG, "ɾ�������ļ� %s\n",pfiletmp);
                    remove(pfiletmp);
                }
            }
        }
    }

}



/*******************************************************************************
* ��������: patchfile_meter_check
* ��������: //��� �����ļ��е���Ƿ���ȷ
* �������: Pipe �˿ں�
       taskid �����
       daycnt �϶����ղ����ļ�
* �������:
* �� �� ֵ:
*******************************************************************************/
void patchfile_meter_check(uint8 pipe,uint8 daycnt,PATCH_TASK_T *PatchTask)
{
    char      pfile[100]={0};
    PATCH_TASK_T patchtasktmp;
    memset(&patchtasktmp,0x00,sizeof(PATCH_TASK_T));
    uint16 i,j,k;
    SCHEME_TASK_RUN_INFO TaskInfo;
    uint16 pn[MAX_AMR_COLLECT_CNT];
    uint16 meternum=0;

    memset(&TaskInfo,0x00,sizeof(SCHEME_TASK_RUN_INFO));
    taskmng_task_info_get(PatchTask->TaskId,LOGIC_ID_YC,&TaskInfo);
    sprintf(pfile,"%s/%s%03d_%02d_PIPE%02d",PrivateDir,PATCH_FILE,PatchTask->TaskId,daycnt,pipe);

    for(i=0;i<TaskInfo.TotalNum;i++)
    {
        if(pipe == TaskInfo.MsInfo[i].Pipe)
        {
            pn[meternum] = TaskInfo.MsInfo[i].Sn;
            meternum++;
        }
    }

    memcpy(patchtasktmp.Time,PatchTask->Time,3);
    memcpy(patchtasktmp.FileVersion,PatchTask->FileVersion,5);
    patchtasktmp.TaskId = PatchTask->TaskId;
    patchtasktmp.TimeNum = PatchTask->TimeNum;
    for(i=0;i<PatchTask->TimeNum;i++)
    {
        patchtasktmp.TimeInfo[i].PatchTime = PatchTask->TimeInfo[i].PatchTime;
        patchtasktmp.TimeInfo[i].MeterNum = meternum;
        memcpy(patchtasktmp.TimeInfo[i].Pn,pn,sizeof(pn));
        for(j=0;j<meternum;j++)
        {
            for(k=0;k<PatchTask->TimeInfo[i].MeterNum;k++)
            {
                if(patchtasktmp.TimeInfo[i].Pn[j] == PatchTask->TimeInfo[i].Pn[k])
                {
                    if(TRUE == taskmng_mtr_result_check(k,PatchTask->TimeInfo[i].flag))
                    {

                        taskmng_mtr_result_set(j, patchtasktmp.TimeInfo[i].flag);
                    }
                    break;
                }
            }
        }
    }
    write_pdata_xram(pfile,(char*)&patchtasktmp,0,sizeof(PATCH_TASK_T));
    TASK_FMT_TRACE(pipe, REC_LOG, "���²����ļ��ɹ�\n");
}


/*******************************************************************************
* ��������: CheckPatchFile
* ��������: //���ǰ���첹���ļ��Ƿ���� û���򴴽�
* �������: Pipe �˿ں�

* �������:
* �� �� ֵ:
*******************************************************************************/
void CheckPatchFile(uint8 Pipe)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[Pipe];
    char      pfile[100]={0};
    uint8 i;
    uint8 TaskId;
    uint8 DayCnt=0;
    TASK_FMT_TRACE(Pipe, REC_LOG, "CheckPatchFile   ��鲹���ļ��Ƿ����\n");
    //pthread_rwlock_rdlock(&TaskRWLock);
    for(i=0;i<pAmrThreadArg->TaskNum;i++)
    {
        if(pAmrThreadArg->pTaskRunInfo[i].logicid!=LOGIC_ID_YC)
            continue;
        if(SchemeTaskRunQueue[i].SchemeType ==EVENTPLAN)  //�¼�������
            continue;
        DayCnt = 0;
        while(DayCnt<PATCH_MAX_DAY)
        {

            TaskId = pAmrThreadArg->pTaskRunInfo[i].TaskId;
            memset(pfile,0x00,sizeof(pfile));
            sprintf(pfile,"%s/%s%03d_%02d_PIPE%02d",PrivateDir,PATCH_FILE,TaskId,DayCnt,Pipe);

            if(access(pfile,R_OK)==0)        //���������Ĳ����ļ���������һ��
            {
                TASK_FMT_TRACE(Pipe, REC_LOG, "�����ļ�%s ����\n",pfile);
                PATCH_TASK_T PatchTask;
                memset(&PatchTask,0x00,sizeof(PATCH_TASK_T));
                read_pdata_xram(pfile,(char*)&PatchTask,0,sizeof(PATCH_TASK_T));
                //��鲹���ļ��汾
                if(strcmp(PatchTask.FileVersion,PATCH_FILE_VERSION)!=0)
                {
                    TASK_FMT_TRACE(Pipe, REC_LOG, "�����ļ��汾���� ɾ��\n",PatchTask.Time[2],PatchTask.Time[1],PatchTask.Time[0],PatchTask.TimeNum);
                    remove(pfile);
                }else
                {
//                    TASK_FMT_TRACE(Pipe, REC_LOG, "����ʱ�� %02x %02x %02x timenum %d \n",PatchTask.Time[2],PatchTask.Time[1],PatchTask.Time[0],PatchTask.TimeNum);
//                    uint8 j;
//                    uint16 k;
//                    for(j=0;j<PatchTask.TimeNum;j++)
//                    {
//                        TASK_FMT_TRACE(Pipe, REC_LOG, "����ʱ��� %ld \n",PatchTask.TimeInfo[j].PatchTime);
//                        for(k=0;k<PatchTask.TimeInfo[j].MeterNum;k++)
//                        {
//                            TASK_FMT_TRACE(Pipe, REC_LOG, "pn %d flag  %d \n",PatchTask.TimeInfo[j].Pn[k],taskmng_mtr_result_check(k, PatchTask.TimeInfo[j].flag));
//
//                        }
//                    }
                    patchfile_meter_check(Pipe,DayCnt,&PatchTask);
                    DayCnt++;
                    continue;
                }
            }
            TASK_FMT_TRACE(Pipe, REC_LOG, "�����ļ�%s ������\n",pfile);
            //TASK_FMT_TRACE(Pipe, REC_LOG, "taskid %d tasktype %d mainoad %04x \n",TaskId,SchemeTaskRunQueue[i].SchemeType ,SchemeTaskRunQueue[i].CsdTask.RoadTask[0].MainOad.nObjID);

            //������������
            if(SchemeTaskRunQueue[i].CsdTask.RoadTask[0].MainOad.nObjID==0x5002
                ||SchemeTaskRunQueue[i].CsdTask.RoadTask[0].MainOad.nObjID==0x5003
                ||SchemeTaskRunQueue[i].CsdTask.RoadTask[0].MainOad.nObjID==0x5004)
            {
               CreatPatchFile_TaskId(Pipe,&SchemeTaskRunQueue[i],DayCnt);
            }
            DayCnt++;
        }
    }
    //pthread_rwlock_unlock(&TaskRWLock);

}

/*******************************************************************************
* ��������: CreatPatchFile_TaskId
* ��������: ���������ļ�
* �������: Pipe �˿ں�
       Tmp_AmrLink �������
       DayCnt  �ϼ����
* �������:
* �� �� ֵ:
*******************************************************************************/
void CreatPatchFile_TaskId(uint8 Pipe,SCHEME_TASK_RUN_INFO *Tmp_AmrLink,uint8 DayCnt)
{
    //AmrThreadArg_T* pAmrThreadArg;
    //pAmrThreadArg = &gAmrThreadArg[Pipe];
    //485��ʱ����Ҫ�����ļ�
    if(Pipe>=PLC_NUM)
    {
        return;
    }
    PATCH_TASK_T TaskInfo;
    char    pfile[100] = {0};

    uint8  day;
    uint16 i;
    uint16 MeterNum,TimeNum;
    DateTimeBcd_t tmpTime;

    time_t    tmpPatchTime;
    time_t    starttime;
    uint16 pn[MAX_AMR_COLLECT_CNT];
    sprintf(pfile,"%s/%s%03d_%02d_PIPE%02d",PrivateDir,PATCH_FILE,Tmp_AmrLink->TaskId,DayCnt,Pipe);
    //��ʵʱ��ʽ���� ������
    if(Tmp_AmrLink->AcqInfo.Acqtype == ACQTYP_NULL)
    {
        return;
    }
    if(GetTaskState(Tmp_AmrLink->TaskId,Tmp_AmrLink->logicid)!=1)
        return ;
    uint32 period = taskmng_task_period_get(Tmp_AmrLink->TaskId,Tmp_AmrLink->logicid);
     //��������С��15�������񲻲���
    if(period<15*60)
        return;
     TASK_FMT_TRACE(Pipe, REC_LOG, "���������ļ�%s ������\n",pfile);
    tmpPatchTime = GetDaycntTime( DayCnt);
    DT_Time2DateTimeBCD(tmpPatchTime,&tmpTime);
    day = tmpTime.day;
    TaskInfo.Time[0] = tmpTime.day;
    TaskInfo.Time[1] = tmpTime.month;
    TaskInfo.Time[2] = tmpTime.year_l;
    strcpy(TaskInfo.FileVersion,PATCH_FILE_VERSION);
    MeterNum = 0;

    //��ȡ��ǰ�����ڵ�ǰ�߳���Ҫ�����ĵ��
    for(i=0;i<Tmp_AmrLink->TotalNum;i++)
    {
        if(Pipe ==Tmp_AmrLink->MsInfo[i].Pipe )
        {
            pn[MeterNum] = Tmp_AmrLink->MsInfo[i].Sn;
            //MeterInfo[MeterNum].flag = 0;
            //TASK_FMT_TRACE(Pipe, REC_LOG, "MeterNum %d  pn %d \n",MeterNum,MeterInfo[MeterNum].Pn);
            MeterNum++;
        }
    }
    if(MeterNum == 0)
        return;
    TimeNum = 0;
    while(1)
    {

        DT_Time2DateTimeBCD(tmpPatchTime,&tmpTime);
        if(tmpTime.day!=day)
            break;
        starttime = tmpPatchTime -taskmng_task_period_get(Tmp_AmrLink->TaskId,Tmp_AmrLink->logicid);

        TaskInfo.TimeInfo[TimeNum].PatchTime = starttime;
        TaskInfo.TimeInfo[TimeNum].MeterNum = MeterNum;
        memcpy(TaskInfo.TimeInfo[TimeNum].Pn,pn,sizeof(pn));
        memset(TaskInfo.TimeInfo[TimeNum].flag,0x00,sizeof(TaskInfo.TimeInfo[TimeNum].flag));
       // TASK_FMT_TRACE(Pipe, REC_LOG, "TimeNum %d MeterNum %d  tmpPatchTime %ld \n",TimeNum,MeterNum,tmpPatchTime);
//        for(i=0;i<MeterNum;i++)
//        {
//            //memcpy(&TaskInfo.TimeInfo[TimeNum].MeterInfo[i],&MeterInfo[i],sizeof(PATCH_METER_T));
//            TaskInfo.TimeInfo[TimeNum].MeterInfo[i].Pn = MeterInfo[i].Pn;
//            TaskInfo.TimeInfo[TimeNum].MeterInfo[i].flag = MeterInfo[i].flag;
//
//        }
        TimeNum++;
        //DT_TimeAddInv(&tmpPatchTime,TI);
        tmpPatchTime+=period;
    }
    TaskInfo.TaskId = Tmp_AmrLink->TaskId;
    TaskInfo.TimeNum = TimeNum;
    int ret ;
    ret = write_pdata_xram(pfile,(char*)&TaskInfo,0,sizeof(PATCH_TASK_T));
    TASK_FMT_TRACE(Pipe, REC_LOG, "д�����ļ����� taskid %d ������ %02x %02x %02x TimeNum %d ret %d \n",TaskInfo.TaskId,TaskInfo.Time[0],TaskInfo.Time[1],TaskInfo.Time[2], TaskInfo.TimeNum,ret);

}

/*******************************************************************************
* ��������: get_task_run_frequence
* ��������: ��ȡ����ִ��Ƶ��
* �������: TaskId �����

* �������:  Ti ����ִ��Ƶ��
* �� �� ֵ:
*******************************************************************************/
BOOL get_task_run_frequence(uint16 TaskId,OOP_TI_T *Ti)
{
    uint16 i=0;
    pthread_rwlock_rdlock(&SchemeTaskCfgList.rwLock);
    for(i=0; i<SchemeTaskCfgList.TaskNum; i++)
    {
        if(SchemeTaskCfgList.OopTaskCfg[i].TaskId == TaskId)
        {
            Ti->unit=SchemeTaskCfgList.OopTaskCfg[i].RunCycleUnit;
            Ti->value=SchemeTaskCfgList.OopTaskCfg[i].RunCycle;
            pthread_rwlock_unlock(&SchemeTaskCfgList.rwLock);
            return TRUE;
        }

    }
    pthread_rwlock_unlock(&SchemeTaskCfgList.rwLock);
    return FALSE;
}

/*******************************************************************************
* ��������: taskmng_prtl_isvalid_check
* ��������: �жϵ���Э���Ƿ�֧��
* �������: Э���

* �������:
* �� �� ֵ: TRUE ��ʾ֧�ָù�Լ;FALSE��֧��
*******************************************************************************/
BOOL taskmng_prtl_isvalid_check(uint8 prtl)
{
    switch(prtl)
    {
        case PRTL_OOP:
        case PRTL_07:
        case PRTL_97:
        case PRTL_Cap:
        case PRTL_Protector:
        case PRTL_Dlms:
        case PRTL_188:
            return TRUE;
        default:
        return FALSE;
    }
}

/*******************************************************************************
* ��������: taskmng_698_addr_get
* ��������: ��ȡ����ַ
* �������: inframe ��������
          inLen ��Ϣ����
* �������:  Addr ����ַ
* �� �� ֵ:
*******************************************************************************/
void taskmng_698_addr_get(uint8 *inframe,uint16 inLen,uint8 *Addr)
{
    uint16 offset =0 ;
    OOP_TSA_T tsa;
    //head len
    offset+=3;
    //crtl

    offset+=1;
    //af
    tsa.af = inframe[offset];
    offset+=1;
    //sa
    memcpy(Addr,&inframe[offset],tsa.len+1);
}



/*******************************************************************************
* ��������: up_msg_response
* ��������: //���лظ�bool
* �������: MsgInfo ��Ϣ����
            result �ظ�����Ϣ
* �������:
* �� �� ֵ:
*******************************************************************************/
void up_msg_response(MESSAGE_INFO_T* MsgInfo ,uint8 result)
{
    MESSAGE_INFO_T* MsgSend=NULL;
    //uint8 result = TRUE;
    TASK_FMT_DEBUG(-1, UP_LOG, "�����лظ�����result %d \n",result);
    if(result!=0)
        result = 1;
    MsgSend = (MESSAGE_INFO_T*)malloc(sizeof(MESSAGE_INFO_T)+1);
    memset(MsgSend,0x00,sizeof(MESSAGE_INFO_T)+1);

    MsgSend->rpm = 0;
    MsgSend->index = MsgInfo->index;
    MsgSend->priority = MsgInfo->priority;
    MsgSend->label = MsgInfo->label;
    MsgSend->IID = MsgInfo->IID;
    MsgSend->IOP = MsgInfo->IOP;

    MsgSend->sourlen = MsgInfo->destlen;
    memcpy(MsgSend->souraddr,MsgInfo->destaddr,MsgSend->sourlen);
    MsgSend->destlen = MsgInfo->sourlen;
    memcpy(MsgSend->destaddr,MsgInfo->souraddr,MsgSend->destlen );
    MsgSend->msglen = 1;
    memcpy(MsgSend->playload,&result,1);


    taskmng_plc_queue_write(APPArg.SendBuf, (uint8*)MsgSend, sizeof(MESSAGE_INFO_T)+1);
    free(MsgSend);

}


/*******************************************************************************
* ��������: GetNextRelayDI
* ��������: ��ȡ0x0010��֡��͸Ҫ������������
* �������:
* �������: DiInfo
* �� �� ֵ:
*           1                  ������Ҫ����������
*           0                  ȫ������
*******************************************************************************/

int GetNext485RelayDI(uint8 pipe,DI_INFO_T* DiInfo,uint8* prtl)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    uint16 i;
    //uint8 prtl;
    uint8 Addr[6]={0};
    int ret;
    for(i = pAmrThreadArg->RelayTrans.AmrIndex;i<pAmrThreadArg->RelayTrans.AmrNum;i++)
    {

        taskmng_prtl_get_from_tsa(pAmrThreadArg->RelayTrans.AmrNormal[i].Mtraddr,LOGIC_ID_YC,prtl);
        memcpy_r(pAmrThreadArg->RelayTrans.MtrAddr,pAmrThreadArg->RelayTrans.AmrNormal[i].Mtraddr.add,6);

        memcpy_r(Addr,pAmrThreadArg->RelayTrans.AmrNormal[i].Mtraddr.add,6);
        TASK_FMT_DEBUG( pipe, RELAY_LOG, "taskmng_next_relay_DI_get ��ǰ���index %d prtl %d \n",i,*prtl);
        TASK_FMT_DEBUG( pipe, RELAY_LOG, "��ַ 0x%02x%02x%02x%02x%02x%02x \n",Addr[5],Addr[4],Addr[3],Addr[2],Addr[1],Addr[0]);
        if(*prtl == PRTL_NULL)
            continue;
        if(pAmrThreadArg->RelayTrans.OadIndex == 0&&pAmrThreadArg->RelayTrans.MapIndex == 0)
        {
            pAmrThreadArg->RelayTrans.AmrNormal[i].SendTime = time(NULL);
            TASK_FMT_DEBUG( pipe, RELAY_LOG, "�ñ��һ�η���ʱ�� %ld \n",pAmrThreadArg->RelayTrans.AmrNormal[i].SendTime);
        }
        if(*prtl == PRTL_OOP)
        {
            ret = Get485OopRelayOad(pipe,i,DiInfo);
            if(ret >0)
            {
                TASK_FMT_DEBUG( pipe, RELAY_LOG, "��������͸ SubOadNum %d \n",DiInfo->SubOadNum);
                TASK_FMT_DEBUG( pipe, RELAY_LOG, "��������͸ DI is %x\n",DiInfo->SubOad[0].value);
                return 1;
            }
            else
            {
                pAmrThreadArg->RelayTrans.AmrIndex++;
            }
        }
        else
        {
            ret = taskmng_645_relay_DI_get(pipe,i,*prtl,DiInfo->DI);
            if(ret>0)
            {
                TASK_FMT_DEBUG( pipe, RELAY_LOG, "645��͸ DI 0X%02x%02x%02x%02x \n",DiInfo->DI[3],DiInfo->DI[2],DiInfo->DI[1],DiInfo->DI[0]);
                return 1;
            }
            else
            {
                pAmrThreadArg->RelayTrans.AmrIndex++;
            }
        }
    }
    return 0;
}


/*******************************************************************************
* ��������: GetNext485SetRelayData
* ��������: ��ȡ0x0010��֡��͸Ҫ������������
* �������:
* �������: DiInfo
* �� �� ֵ:
*           1                  ������Ҫ����������
*           0                  ȫ������
*******************************************************************************/

int GetNext485SetRelayData(uint8 pipe)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    uint16 i;
    uint8 meterindex=0;
    //uint8 prtl;
    //uint8 Addr[6]={0};
    int ret;
    uint8 speed;
    TASK_FMT_DEBUG( pipe, RELAY_LOG, "pAmrThreadArg->RelayTrans.AmrIndex is %d\n",pAmrThreadArg->RelayTrans.AmrIndex);
    for(i = pAmrThreadArg->RelayTrans.AmrIndex;i<pAmrThreadArg->RelayTrans.TotalNum;i++)
    {
        meterindex=pAmrThreadArg->RelayTrans.AmrIndex;
        if(pAmrThreadArg->RelayTrans.METER_LIST[meterindex].VliadMeter==0)
        {
            return 1;
        }
        Init_Relay_Cb_Pipe(pipe,&speed);
        ret=rs485_RS232_send(pipe,MAX_FRAME_OVERTIME,MAX_CHAR_OVERTIME,speed,PARITY_EVEN,pAmrThreadArg->RelayTrans.METER_LIST[i].setaction, pAmrThreadArg->RelayTrans.METER_LIST[i].setlen);
        if(ret<0)
        {
            return -1;
        }
        return 1;
    }
    return -1;
}

void Init_Rs485_Pipe()
{
    memset((uint8*)Rs485ComNo,0x00,sizeof(RS485_RS232_PARA)*8);
}

/*******************************************************************************
* ��������: GetOopRelayOad
* ��������: ��ȡ��֡��͸����������Ҫ������Oad
* �������:Pipe
        MtrIndex ��͸�е������
* �������:DiInfo

* �� �� ֵ: 0    ��ǰ���ѳ������
    Oad����
*******************************************************************************/
uint8 Get485OopRelayOad(uint8 pipe,uint8 MtrIndex,DI_INFO_T* DiInfo)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    uint8 i=0;
    OadInfoMap OadMap ;
//    uint16               OadLen = 0;
    uint8                OadNum = 0;
//    uint16 Pn = taskmng_pn_get_from_tsa(pAmrThreadArg->RelayTrans.AmrNormal[MtrIndex].Mtraddr);
//    uint8 MtrEsamSerialNo[8]={0};
//    uint8 MtrSecModEn;
    TASK_FMT_DEBUG( pipe, RELAY_LOG, "GetOopRelayOad ��ǰoadindex %d  \n",pAmrThreadArg->RelayTrans.OadIndex );
      //�жϰ�ȫģʽ�Ƿ�����
//    if(ReadOopMeterSecurityEsamSerialNo( pipe, Pn, MtrEsamSerialNo)<0)
//    {
//      DiInfo->SubOad[0].nObjID = 0x4002;
//        DiInfo->SubOad[0].attID = 2;
//        DiInfo->SubOad[0].nIndex=0;
//        pAmrThreadArg->RelayTrans.AmrNormal[MtrIndex].IsNeedGetSafeMode = TRUE;
//        DiInfo->SubOadNum = 1;
//        return 2;
//    }
//  else
//  {
//      Set_ReadMtrNum(pipe,MtrEsamSerialNo);
//  }
//  if(ReadOopMeterSecuritySecModEn( pipe, Pn, &MtrSecModEn)<0)
//    {
//        DiInfo->SubOad[0].nObjID = 0xF101;
//        DiInfo->SubOad[0].attID = 2;
//        DiInfo->SubOad[0].nIndex=0;
//        pAmrThreadArg->RelayTrans.AmrNormal[MtrIndex].IsNeedGetSafeMode = TRUE;
//        DiInfo->SubOadNum = 1;
//        return 2;
//    }
//  else
//  {
//      //MtrSecModEn=0;//����ʹ��
//      pAmrThreadArg->RelayTrans.AmrNormal[MtrIndex].IsNeedGetSafeMode = FALSE;
//      Set_MtrSecModEn(pipe,MtrSecModEn);  //���ð�ȫģʽ
//  }
    if(pAmrThreadArg->RelayTrans.OadIndex >= pAmrThreadArg->RelayTrans.AmrNormal[MtrIndex].OadNum )
    {
        pAmrThreadArg->RelayTrans.OadIndex = 0;
        pAmrThreadArg->RelayTrans.MapIndex = 0;
        TASK_FMT_DEBUG( pipe, RELAY_LOG, "taskmng_oop_relay_oad_get ��ǰ���͸���  \n" );
        return 0;
    }
	pAmrThreadArg->RelayTrans.OopSendNum =0;
    for(i =pAmrThreadArg->RelayTrans.OadIndex ;i<pAmrThreadArg->RelayTrans.AmrNormal[MtrIndex].OadNum;i++ )
    {
    	if(OadNum>Rs485ReadNum)
    	{
    		break;
    	}
        if(taskmng_oad_info_get(pAmrThreadArg->RelayTrans.AmrNormal[MtrIndex].SubOad[i],&OadMap )==FALSE)
        {
        	OadNum++;
            continue;
        }
        TASK_FMT_DEBUG(pipe,RELAY_LOG,"taskmng_oop_relay_oad_get i %d oad %08x len %d \n",i,pAmrThreadArg->RelayTrans.AmrNormal[MtrIndex].SubOad[i].value,OadMap.dataLen);
        memcpy_r((uint8*)&DiInfo->SubOad[OadNum] , (uint8*)&pAmrThreadArg->RelayTrans.AmrNormal[MtrIndex].SubOad[i],4);
		OadNum++;
    }
    DiInfo->SubOadNum =OadNum ;
	pAmrThreadArg->RelayTrans.OopSendNum =pAmrThreadArg->RelayTrans.OopSendNum+OadNum;
    return 1;
}




/*******************************************************************************
* ��������: MakeRelay485Data_0010
* ��������: 485�����鳭��֡
* �������: pipe �˿ں�
        DiInfo ������
        prtl  ��Լ����
* �������:  DataBuff ��������
* �� �� ֵ:  ���ĳ���
*******************************************************************************/
uint16 MakeRelay485Data_0010(uint8 pipe,DI_INFO_T DiInfo,uint8 prtl,uint8* DataBuff)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    LinkLayer_t*            pLinkLayer = NULL;
    uint16 len = 0;
    //uint8           InDataBuf[255];
    //uint16          offset = 0;
//    uint8 i;
    MeterPrtlFmt_T  MeterPrtlData;                      //����Լ��ʽ����

    uint8 AmrIndex = pAmrThreadArg->RelayTrans.AmrIndex;
    MeterPrtlData.DelayFlag = 0;

    MeterPrtlData.PrtlMode = prtl;
    memcpy_r(MeterPrtlData.MtrAddr,pAmrThreadArg->RelayTrans.AmrNormal[AmrIndex].Mtraddr.add,6);

    if(prtl == PRTL_97)
    {
        MeterPrtlData.CtrlCode = 0x01;
        MeterPrtlData.DataLen = 2;
        memcpy(MeterPrtlData.DataBuff, DiInfo.DI, MeterPrtlData.DataLen);
        memcpy(pAmrThreadArg->RelayTrans.SendDI,DiInfo.DI,2);
    }
    else if(prtl == PRTL_07)
    {
        MeterPrtlData.CtrlCode = 0x11;
        MeterPrtlData.DataLen = 4;
        memcpy(MeterPrtlData.DataBuff, DiInfo.DI, MeterPrtlData.DataLen);
        memcpy(pAmrThreadArg->RelayTrans.SendDI,DiInfo.DI,4);
    }

    if(prtl == PRTL_OOP)
    {
        OopGet.requst(pipe,&pAmrThreadArg->RelayTrans.AmrNormal[AmrIndex].Mtraddr,2,(uint8*)&DiInfo.SubOad[0],4*DiInfo.SubOadNum);
        pLinkLayer = FindLinkLayer(pipe, NULL);
        len = pLinkLayer->sendLen;
        memcpy(DataBuff,pLinkLayer->sendBuff,len);
    }
    else
    {
        len = taskmng_meter_prtl_645_pack(DataBuff, MeterPrtlData);
    }
    return len;
}



/*******************************************************************************
* ��������: UnpackRelay485Data_0010
* ��������: //485�⴩͸����
* �������: pipe �˿ں�
        DataBuff ��������
        DataLen  ���ĳ���
* �������:  MeterPrtlData 645������������
        apdu 698��������apdu
* �� �� ֵ:  -1 ʧ�� �����ɹ�
*******************************************************************************/
int UnpackRelay485Data_0010(uint8 pipe,uint8* DataBuff,uint16 DataLen,MeterPrtlFmt_T*  MeterPrtlData,char* apdu)
{
    int ret = -1;
    AppLayerList_t* pAppLayer = NULL;
    ret = taskmng_645_meter_data_unpack(DataLen,DataBuff, MeterPrtlData->MtrAddr, &MeterPrtlData->CtrlCode, &MeterPrtlData->DataLen, MeterPrtlData->DataBuff);
    if(ret == CHECK_RESULT_FAILUER)
    {
        ret = LinkDataReceive(pipe, DataBuff, DataLen,NULL);
        TASK_FMT_DEBUG(pipe, RELAY_LOG,"LinkDataReceive 698��֡����ֵ %d \n",ret);
        if(ret >=eOopComm_Fail)//��֡ʧ��ֱ�ӷ���
            return -1;
        //��ȡ����ַ
        pAppLayer = FindAppLayer(pipe, NULL);
        if(pAppLayer == NULL)
            return -1;
        memcpy_r(MeterPrtlData->MtrAddr,pAppLayer->sa.add,6);

        MeterPrtlData->DataLen = pAppLayer->recvApduList->apduLen-pAppLayer->recvApduList->offSet;
        memcpy(MeterPrtlData->DataBuff, pAppLayer->recvApduList->apdu+pAppLayer->recvApduList->offSet, MeterPrtlData->DataLen);
        memcpy(apdu,pAppLayer->recvApduList->apdu,2);
    }
    return ret;
}





/*******************************************************************************
* ��������: PatchDataReading
* ��������:�������� ����һ���������в��� ��������ǰ �����������ȼ��� �������г������һ��
* �������: ��
* �������: ��
* �� �� ֵ: >0                  �ɹ�
*           <0                  ʧ��
*******************************************************************************/
int PatchDataReading(uint8 pipe)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    static int8 DayCnt[PLC_NUM] = {0};
    static int8 LastDayCnt[PLC_NUM] = {0};
    static CTS_RUN_TASK_T* tmptask[PLC_NUM] = {NULL};
    int ret;
    uint16 i,j;
    MESSAGE_INFO_T      *MsgInfo = NULL;
    int             RecvLen = 0;
    uint8           RecvBuf[MAX_MSG_LEN] = {0};
    uint8           PayloadBuf[MAX_MSG_LEN] = {0};        //payload������
    AMR_PLC_INFO_REPORT_T  AmrReportInfo;    //�ϱ��ĳ�������
    MeterPrtlFmt_T  MeterPrtlData;



    uint8           SendIndex =0;
    uint16          MtrIndex = 0;
    char            apdu[2];
    //uint16 TaskId;
    int             OverTimeCnt = 100;
    static SCHEME_TASK_RUN_INFO TaskInfo[PLC_NUM];
    static uint8 lastday[PLC_NUM];
    if(pAmrThreadArg->TaskModifyFlag!=TASK_CFG_INITED)
        return 1;//����δ��ʼ�� ����
    if((pAmrThreadArg->PatchTask.TaskId !=0) && (lastday[pipe]!=gCurBcdTime[pipe].Day))
    {
        lastday[pipe] = gCurBcdTime[pipe].Day;
        //������Ҫ���¼��ز����ļ�
        TASK_FMT_TRACE( pipe, REC_LOG, "���� �������� \n");

        pAmrThreadArg->TotalState.PatchAmr = RTS_IDLE;
        return 1;

    }
    if(pAmrThreadArg->SendQueueFlag>0)
    {

        //TASK_FMT_TRACE( pipe, REC_LOG, "patch recv only! \n");
        pAmrThreadArg->TotalState.PatchAmr = RTS_RECV;
    }
    lastday[pipe] = gCurBcdTime[pipe].Day;
    switch(pAmrThreadArg->TotalState.PatchAmr)
    {
    case RTS_IDLE:
    {
        memset(&pAmrThreadArg->PatchTask,0x00,sizeof(PATCH_TASK_T));
        memset(&pAmrThreadArg->PatchRunTask,0X00,sizeof(PATCH_TASK_RUN_T));
        memset(&pAmrThreadArg->AmrSendFrameInfo,0x00,sizeof(SendFrameQueue_T));
        memset(pAmrThreadArg->patchMeterData,0x00,sizeof(pAmrThreadArg->patchMeterData));
        pAmrThreadArg->AmrSendFrameInfo.usetype = 1;
        ret = GetPatchTaskInfo(pipe,(uint8)DayCnt[pipe],&tmptask[pipe]);
        if(ret>0)
        {
            pAmrThreadArg->PatchRunTask.TaskId = tmptask[pipe]->TaskId;
            pAmrThreadArg->PatchRunTask.NeedMtrNum = GetPatchNeedMtrNum(pAmrThreadArg->PatchTask.TimeInfo[0]);
            TASK_FMT_TRACE( pipe, REC_LOG, "�������� %d NeedMtrNum %d \n",pAmrThreadArg->PatchRunTask.TaskId,pAmrThreadArg->PatchRunTask.NeedMtrNum);

            memset(&TaskInfo[pipe],0x00,sizeof(SCHEME_TASK_RUN_INFO));
            uint16 taskid = pAmrThreadArg->PatchTask.TaskId;
            taskmng_task_info_get(taskid,LOGIC_ID_YC, &TaskInfo[pipe]);
            for(j=0;j<pAmrThreadArg->TmnAmrInfo.MtrCnt;j++)
            {
                pAmrThreadArg->PatchMtrInfo[j].CSDIndex = 0;
                pAmrThreadArg->PatchMtrInfo[j].SigFrameNum =0;
                pAmrThreadArg->PatchMtrInfo[j].MapIndex = 0;
                pAmrThreadArg->PatchMtrInfo[j].FailCnt =0;
                InitPatchTask(&TaskInfo[pipe],&pAmrThreadArg->PatchMtrInfo[j],&pAmrThreadArg->PatchRunTask,pAmrThreadArg->PatchTask.TimeInfo[0].PatchTime);
            }
            pAmrThreadArg->TotalState.PatchAmr = RTS_SEND;

        }else
        {
            DayCnt[pipe]++;
        }

        if(DayCnt[pipe]> PATCH_MAX_DAY-1)
            DayCnt[pipe] = 0;
    }
        break;
    case RTS_SEND:
    {
        pAmrThreadArg->AmrSendFrameInfo.usetype = 1;
        if(PatchAmrMakeFrame(pipe,DayCnt[pipe])==1)
        {
            TASK_FMT_TRACE( pipe, REC_LOG, "������ǰ������� DayCnt %d \n",DayCnt[pipe]);
            //WritePatchResult(pipe,pAmrThreadArg->PatchTask.TaskId,(uint8)DayCnt[pipe]);

            pAmrThreadArg->TotalState.PatchAmr = RTS_IDLE;
            //����ֱ��return-1 ������û�в��������ʱ���һֱˢ��־ ���ڲ�����������һ���ͣ���� һ����һ��
            if(DayCnt[pipe] == LastDayCnt[pipe])
            {
                return -1;
            }else
            {
                LastDayCnt[pipe] =  DayCnt[pipe];
                return 1;
            }
        }
        else
        {
            LastDayCnt[pipe] =  DayCnt[pipe];
            pAmrThreadArg->TotalState.PatchAmr = RTS_RECV;
        }
    }
        break;
    case RTS_RECV:
    {
        RecvLen = taskmng_plc_queue_read(pAmrRecvDown_Cb[pipe], RecvBuf);
        if(RecvLen>0)
        {
            MsgInfo = (MESSAGE_INFO_T*)RecvBuf;
            memcpy(PayloadBuf,MsgInfo->playload,MsgInfo->msglen);
            TASK_FMT_TRACE( pipe, REC_LOG, "patch recv  \n");

            TASK_BUF_TRACE( pipe, REC_LOG,MsgInfo->playload, MsgInfo->msglen);
            switch(MsgInfo->IOP)
            {
            case 0x003E:
            {
            //��������
            }
                break;
            case 0x0031:
            {
                //TASK_FMT_TRACE(pipe, REC_LOG,"@@@@@�յ������ϱ�����@@@@@@@@@@ \n");
                if(taskmng_amr_report_data_unpack(PayloadBuf,MsgInfo->msglen,&AmrReportInfo)==FALSE)
                    break;
                //TASK_FMT_TRACE(pipe, REC_LOG,"����Ϣ�ɹ� ��ȡ���Ͷ������� \n");
                if(taskmng_seq_is_valid(pipe,&SendIndex,AmrReportInfo.addr)==0)
                {
                    TASK_FMT_TRACE(pipe, REC_LOG,"taskmng_seq_is_valid δ�ҵ� \n");
                    break;
                }
                //�������������л�ʱ��������з��ͱ��� ����е��������� ��ʱ�յ��Ļ���ŵ������洦��
                //ֱ�ӽ��� �������Ͷ�����λ��Ч �����յ����е�ʱ����Զ��е�������������
                if(pAmrThreadArg->AmrSendFrameInfo.usetype != 1)
                {
                    TASK_FMT_TRACE(pipe, REC_LOG,"queue %d recv \n",SendIndex);
                    pAmrThreadArg->AmrSendFrameInfo.Queue[SendIndex].ValidFlag = 0;
                    if( pAmrThreadArg->AmrSendFrameInfo.QueueCnt> 0)
                        pAmrThreadArg->AmrSendFrameInfo.QueueCnt--;
                    break;
                }
                AmrReportInfo.nMeterFilesID = pAmrThreadArg->AmrSendFrameInfo.Queue[SendIndex].Pn;
                ret = taskmng_paral_meter_read_data_unpack(pipe,SendIndex,LOGIC_ID_YC,AmrReportInfo,&MeterPrtlData,&MtrIndex,apdu,pAmrThreadArg->AmrSendFrameInfo.Queue[SendIndex]);
                //TASK_FMT_TRACE(pipe, REC_LOG,"ret %d ���� %d prtl %d \n ",ret,AmrReportInfo.nMeterFilesID,MeterPrtlData.PrtlMode);
                if(ret<0)
                {
                    RefreshPatchPnState(pipe,SendIndex);
                    pAmrThreadArg->TotalState.PatchAmr = RTS_SEND;
                    break;
                }
                //pAmrThreadArg->PatchRunTask.TaskId;
                //TASK_FMT_TRACE( pipe, REC_LOG,"�յ��ϱ����� taskid %d \n",TaskId);
                if(AmrReportInfo.nProtocolType==PRTL_OOP && ret == eOopComm_SegRecv)
                {
                    TASK_FMT_TRACE( pipe, REC_LOG,"�յ���·��֡����\n");
                    pAmrThreadArg->AmrSendFrameInfo.Queue[SendIndex].NeedSendSeg=1;
                    pAmrThreadArg->TotalState.PatchAmr = RTS_SEND;
                    break;
                }
                if(MeterPrtlData.PrtlMode == PRTL_OOP)
                {
                    if(ret ==eOopComm_GetNext )
                    {
                        TASK_FMT_TRACE( pipe, REC_LOG,"�յ�Ӧ�÷�֡���� ��������\n");
                        pAmrThreadArg->AmrSendFrameInfo.Queue[SendIndex].NeedSendSeg=1;
                        OopPatchMeterAmrDataSave_Record(pipe,MeterPrtlData.DataBuff,MeterPrtlData.DataLen,MtrIndex,&TaskInfo[pipe],1);
                        pAmrThreadArg->TotalState.PatchAmr = RTS_SEND;
                        break;
                    }
                    //����ȫ�����ռ�¼��
                    #if (defined AREA_JIANGXI) || (defined AREA_ANHUI)||(defined AREA_JIANGSU)
                    if(MeterPrtlData.Istasktable == 1)
                    {
                        OopPatchTeminalAmrDataSave_Record(pipe,MeterPrtlData.DataBuff,MeterPrtlData.DataLen,MtrIndex,&TaskInfo[pipe]);
                    }
                    else
                    OopPatchMeterAmrDataSave_Record(pipe,MeterPrtlData.DataBuff,MeterPrtlData.DataLen,MtrIndex,&TaskInfo[pipe],0);
                    #else
                    OopPatchMeterAmrDataSave_Record(pipe,MeterPrtlData.DataBuff,MeterPrtlData.DataLen,MtrIndex,&TaskInfo[pipe],0);
                    #endif

                }else
                {
                    if(ret == CHECK_RESULT_SUCCEED)
                    {
                        if(JudgePatchMoveNextAmrTaskIndex(pipe,MtrIndex,&TaskInfo[pipe],MeterPrtlData.DataBuff,MeterPrtlData.CtrlCode,&MeterPrtlData.DataLen)>0)
                        {
                            TASK_FMT_TRACE(pipe, REC_LOG,"645��֤�ɹ� DataLen %d\n",MeterPrtlData.DataLen);

                            TASK_BUF_TRACE( pipe, REC_LOG,MeterPrtlData.DataBuff, MeterPrtlData.DataLen);
                            OopPatchDataSave9707_frz(pipe,&MeterPrtlData.DataBuff[4], MeterPrtlData.DataLen-4,MtrIndex,&TaskInfo[pipe]);
                            pAmrThreadArg->PatchMtrInfo[MtrIndex].MapIndex++;
                            JudgePatchFrz07MoveNext(pipe,&TaskInfo[pipe],MtrIndex,1);
                        }
                    }else
                    {
                        pAmrThreadArg->PatchMtrInfo[MtrIndex].MapIndex++;
                        JudgePatchFrz07MoveNext(pipe,&TaskInfo[pipe],MtrIndex,0);
                    }

                }
                pAmrThreadArg->PatchMtrInfo[MtrIndex].FailCnt=0;
                pAmrThreadArg->AmrSendFrameInfo.Queue[SendIndex].ValidFlag=0;
                if(pAmrThreadArg->AmrSendFrameInfo.QueueCnt>0)
                    pAmrThreadArg->AmrSendFrameInfo.QueueCnt--;
                pAmrThreadArg->TotalState.PatchAmr = RTS_SEND;
            }
                break;
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
                    flag = 1;
                    if((time(NULL)-pAmrThreadArg->AmrSendFrameInfo.Queue[i].CommBeginTime)>OverTimeCnt)
                    {
                        TASK_FMT_TRACE( pipe, REC_LOG, "Queue index %d ��ʱ PN %d \n",i,pAmrThreadArg->AmrSendFrameInfo.Queue[i].Pn);
                        RefreshPatchPnState(pipe,i);
                        pAmrThreadArg->TotalState.PatchAmr = RTS_SEND;
                    }
                }
            }
            //��������ķ��Ͷ�����û����Ч�ľ��л������� ��Ϊ������������һ������
            if(flag==0)
            {
                if(pAmrThreadArg->SendQueueFlag>0)
                {
                    TASK_FMT_TRACE( pipe, REC_LOG, "���β������� \n");
                    if(pAmrThreadArg->AmrSendFrameInfo.usetype != 1)
                        pAmrThreadArg->TotalState.PatchAmr = RTS_IDLE;
                    //�˴�����2���taskmng_plc_task_run�н����ж� ֱ���ٴδ���������������
                    return 2;
                }
                TASK_FMT_TRACE( pipe, REC_LOG, "�������Ͷ�����Ч �л������� \n");
                pAmrThreadArg->TotalState.PatchAmr = RTS_SEND;
            }
        }
    }

        break;
    default:
        break;
    }

    return -1;
}

/*******************************************************************************
* ��������: GetPatchTaskInfo
* ��������:��ȡ��ǰ��Ҫ����������
* �������: pipe daycnt
* �������: tmptask
* �� �� ֵ: >0                  ��ȡ���ղ�������
*           <0                  ��������ȫ������
*******************************************************************************/
int GetPatchTaskInfo(uint8 pipe,uint8 daycnt,CTS_RUN_TASK_T** patchtask)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    CTS_RUN_TASK_T* tmptask = *patchtask;
    if(tmptask == NULL)
        tmptask = pAmrThreadArg->pTaskHead;
    else
        tmptask = tmptask->next;
    char      pfile[100]={0};

    uint8 taskid;
    while(tmptask!=NULL)
    {
        taskid = tmptask->TaskId;
        memset(pfile,0x00,sizeof(pfile));
        sprintf(pfile,"%s/%s%03d_%02d_PIPE%02d",PrivateDir,PATCH_FILE,taskid,daycnt,pipe);
        if(access(pfile,R_OK)!=0 ||tmptask->NeedMtrNum==0||tmptask->TaskState!=1)
        {
            tmptask = tmptask->next;
            continue;
        }

        read_pdata_xram(pfile,(char*)&pAmrThreadArg->PatchTask,0,sizeof(PATCH_TASK_T));
        TASK_FMT_TRACE( pipe, REC_LOG, "��ȡ���������� taskid %d daycnt %d timenum %d NeedMtrNum %d��\n",taskid,daycnt,pAmrThreadArg->PatchTask.TimeNum,tmptask->NeedMtrNum);
        TASK_FMT_TRACE( pipe, REC_LOG, "ʱ�� %02x %02x %02x   \n",pAmrThreadArg->PatchTask.Time[2],pAmrThreadArg->PatchTask.Time[1],pAmrThreadArg->PatchTask.Time[0]);
//        uint8 i,j;
//        for(i=0;i<pAmrThreadArg->PatchTask.TimeNum;i++)
//        {
//            for(j=0;j<pAmrThreadArg->PatchTask.TimeInfo[i].MeterNum;j++)
//            {
//                TASK_FMT_TRACE( pipe, REC_LOG, "����ʱ��� %d pn %d flag %d \n",i,pAmrThreadArg->PatchTask.TimeInfo[i].MeterInfo[j].Pn,pAmrThreadArg->PatchTask.TimeInfo[i].MeterInfo[j].flag);
//            }
//        }
        *patchtask = tmptask;
       // TASK_FMT_TRACE( pipe, REC_LOG, "patchtask taskid %d NeedMtrNum %d ��\n",(*patchtask)->TaskId,(*patchtask)->NeedMtrNum);

        return 1;
    }
    *patchtask = tmptask;
    return -1;     //����ȫ������
}

/*******************************************************************************
* ��������: PatchAmrMakeFrame
* ��������: �����鳭��֡
* �������: pipe �˿ں�

* �������:
* �� �� ֵ:
*******************************************************************************/
int PatchAmrMakeFrame(uint8 pipe,int8 daycnt)
{
    uint8 i;
    int ret ;
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    SCHEME_TASK_RUN_INFO TaskInfo;
    int          MsIndex;    //��ǰ�߳��е�����
    DI_INFO_T    TmpDiInfo;
    uint16 Pn;
    uint8 prtl;
    uint8 MtrAddr[6];
    uint16       FrameLen = 0;
    uint8        FrameBuff[MAX_MSG_LEN];  //֡���ݻ�����
    uint8        PayLoad[500];
    OOP_OAD_U pipeOAD = taskmng_pipe_oad_get(pipe);
    MESSAGE_INFO_T    *MsgSend;
    uint16 index;
    memset(&TaskInfo,0x00,sizeof(SCHEME_TASK_RUN_INFO));
    uint16 taskid = pAmrThreadArg->PatchTask.TaskId;
    taskmng_task_info_get(taskid, LOGIC_ID_YC,&TaskInfo);
    int result = 0;
    for(i=0;i<PARALLEL_QUEUE_NUM;i++)
    {
        FrameLen = 0;
        if(pAmrThreadArg->AmrSendFrameInfo.Queue[i].ValidFlag==0)
        {
            ret = GetNextPatchDI(pipe,&TmpDiInfo,&MsIndex,&TaskInfo,daycnt);

            if(ret == SEARCH_SUCCESS)
            {
                Pn = pAmrThreadArg->PatchMtrInfo[MsIndex].Pn;
                taskmng_mtr_addr_prtl_get( Pn,LOGIC_ID_YC, MtrAddr, &prtl);
                pAmrThreadArg->AmrSendFrameInfo.QueueCnt++;
                pAmrThreadArg->AmrSendFrameInfo.Queue[i].ValidFlag =1;
                pAmrThreadArg->AmrSendFrameInfo.Queue[i].Pn = Pn;
                pAmrThreadArg->AmrSendFrameInfo.Queue[i].MsIndex = MsIndex;
                pAmrThreadArg->AmrSendFrameInfo.Queue[i].CommBeginTime = time(NULL);
                pAmrThreadArg->AmrSendFrameInfo.Queue[i].PrtlMode = prtl;
                memcpy(pAmrThreadArg->AmrSendFrameInfo.Queue[i].MeterAddr,MtrAddr,6);
                memcpy((void *)&pAmrThreadArg->AmrSendFrameInfo.Queue[i].DiInfo,(void*)&TmpDiInfo,sizeof(DI_INFO_T));
                TASK_FMT_TRACE( pipe, REC_LOG, "Pn %d MsIndex %d prtl %d StartTime %ld \n",Pn,MsIndex,prtl,pAmrThreadArg->PatchMtrInfo[MsIndex].StartTime);

                FrameLen = PatchMakeFrame(pipe,i,pAmrThreadArg->PatchMtrInfo[MsIndex].StartTime,pAmrThreadArg->AmrSendFrameInfo.Queue[i],FrameBuff,&TaskInfo);
                result = 0 ;
            }
            else if(ret == SEARCH_FINISH)
            {
                //TASK_FMT_TRACE( pipe, REC_LOG,"SEARCH_FINISH \n");
                result = 1 ;
                continue;
                //return 1;
            }else
            {
                //TASK_FMT_TRACE( pipe, REC_LOG,"û���ҵ�Ҫ���ı� \n");
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
                LinkLayer_t* pLinkLayer = FindLinkLayer(oop_pipe_get(pipe,i),NULL);
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
                TASK_FMT_TRACE( pipe, REC_LOG,"Pn %d ���ͷ�֡ȷ��֡ \n",Pn);
                result = 0 ;
            }
        }
        if(FrameLen>0)
        {
            TASK_FMT_TRACE( pipe, REC_LOG,"patch send\n");

            TASK_BUF_TRACE( pipe, REC_LOG,FrameBuff, FrameLen);
            memset(PayLoad,0x00,sizeof(PayLoad));
            index = 0;

            memcpy_r(&PayLoad[index],(uint8*)&pipeOAD.value,4);
            index+=4;
            index+=2;
            memcpy_r(&PayLoad[index],MtrAddr,6);
            index+=6;
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

            MsgSend = (MESSAGE_INFO_T*)malloc(sizeof(MESSAGE_INFO_T)+index);
            memset(MsgSend,0x00,sizeof(MESSAGE_INFO_T)+index);

            MsgSend->rpm = 1;
            MsgSend->index = GetMsgSeq();
            MsgSend->label = pipe;
            MsgSend->IID = 0x0003;
            MsgSend->IOP = 0x003E;
            MsgSend->priority = 1;
            MsgSend->msglen = index;
            MsgSend->sourlen = strlen(DownName);
            memcpy(MsgSend->souraddr,DownName,MsgSend->sourlen);
            MsgSend->destlen = strlen(CCOName);
            memcpy(MsgSend->destaddr,CCOName,MsgSend->destlen );
            memcpy(MsgSend->playload,PayLoad,index);

            taskmng_plc_queue_write(pAmrSendDown[pipe], (uint8*)MsgSend, sizeof(MESSAGE_INFO_T)+index);
            free(MsgSend);
        }
    }
    return result;
}
/*******************************************************************************
* ��������: GetNextPatchDI
* ��������:��ȡ��ǰ��Ҫ�����Ĳ�����ݱ�ʶ
* �������: pipe TaskInfo
* �������: DiInfo MsIndex
* �� �� ֵ: >0                  ������Ҫ����������
*           <0                  ȫ������
*******************************************************************************/
int GetNextPatchDI(uint8 pipe,DI_INFO_T* DiInfo, int *MsIndex,SCHEME_TASK_RUN_INFO *TaskInfo,int8 daycnt)
{
    uint16 Pn;
    int ret ;
    uint8 prtl;
    uint8 i;
    uint16 j;
    uint8 cnt = 0;
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    PATCH_TIME_T patchTime ;
    const MapOopTo645_NEW* pMap = NULL;
    uint16 pNum;
    OOP_TI_T delay;
    delay.unit = TaskInfo->DelayTimeUnit;
    delay.value = TaskInfo->DelayTime;
    time_t timetmp;
    i=pAmrThreadArg->PatchRunTask.TimeIndex;

    while(i<pAmrThreadArg->PatchTask.TimeNum)
    {
        patchTime = pAmrThreadArg->PatchTask.TimeInfo[i];//�������ڱ��߳���Ҫ�����ĵ��
        //ֻ��������ǰ����һ��
        //PatchTime�Ѿ�����һ�ֿ�ʼʱ�� û����ʱ��   ���ֻ��һ��ִ�м���ᵼ�±�������ûִ�о��Ȳ�����
        timetmp = patchTime.PatchTime;
        DT_TimeAddInv(&timetmp,delay);
        if(timetmp >= (time(NULL)-taskmng_task_period_get(TaskInfo->TaskId,TaskInfo->logicid)))
        {
            return SEARCH_FINISH;
        }
        while(1)
        {

            if(pAmrThreadArg->PatchRunTask.SucNum>=pAmrThreadArg->PatchRunTask.NeedMtrNum)
            {
                //TASK_FMT_TRACE( pipe, REC_LOG,"���� %d ʱ��� %ld ������� \n",pAmrThreadArg->PatchRunTask.TaskId,patchTime.PatchTime);
                break;
            }
            if(pAmrThreadArg->PatchRunTask.MeterIndex>=patchTime.MeterNum)
            {
                cnt++;
                pAmrThreadArg->PatchRunTask.MeterIndex=0;
            }

            if(cnt>=2)
            {
                return SEARCH_FAIL;
            }
            //Pn = patchTime.MeterInfo[pAmrThreadArg->PatchRunTask.MeterIndex].Pn;
            Pn = patchTime.Pn[pAmrThreadArg->PatchRunTask.MeterIndex];
            //TASK_FMT_TRACE( pipe, REC_LOG,"MeterIndex %d PN %d flag %d  \n",pAmrThreadArg->PatchRunTask.MeterIndex,Pn,patchTime.MeterInfo[pAmrThreadArg->PatchRunTask.MeterIndex].flag);
            //if(patchTime.MeterInfo[pAmrThreadArg->PatchRunTask.MeterIndex].flag ==1)
            if(taskmng_mtr_result_check(pAmrThreadArg->PatchRunTask.MeterIndex, patchTime.flag))
            {
                //TASK_FMT_TRACE( pipe, REC_LOG,"�ñ�ȫ������ ���貹�� PN %d \n",Pn);

                pAmrThreadArg->PatchRunTask.MeterIndex++;
                continue;
            }

            if(taskmng_mtr_in_queue_is_exist(pipe,Pn)>0)
            {
                pAmrThreadArg->PatchRunTask.MeterIndex++;
                continue;
            }

            if(taskmng_mtr_result_check(pAmrThreadArg->PatchRunTask.MeterIndex,pAmrThreadArg->PatchRunTask.MtrResult)==TRUE)
            {
                pAmrThreadArg->PatchRunTask.MeterIndex++;
                continue;
            }
            taskmng_mtr_prtl_get( Pn, LOGIC_ID_YC,&prtl);
            *MsIndex = taskmng_ms_index_get(pipe,Pn,LOGIC_ID_YC);
            if(*MsIndex == -1)
            {
                pAmrThreadArg->PatchRunTask.MeterIndex++;
                continue;
            }
            TASK_FMT_TRACE( pipe, REC_LOG,"PN %d MeterIndex %d  \n",Pn,pAmrThreadArg->PatchRunTask.MeterIndex);
            if(PRTL_07==prtl && CheckMapFromMainOAD(pipe,*MsIndex,TaskInfo,&pMap,&pNum)<1)
            {
                //TASK_FMT_TRACE( pipe, REC_LOG,"�ñ� %d ��ʵʱ��ʽ���� ������ \n",Pn);
                taskmng_mtr_result_set(pAmrThreadArg->PatchRunTask.MeterIndex,pAmrThreadArg->PatchRunTask.MtrResult);
                //pAmrThreadArg->PatchTask.TimeInfo[i].MeterInfo[pAmrThreadArg->PatchRunTask.MeterIndex].flag =1;
                taskmng_mtr_result_set(pAmrThreadArg->PatchRunTask.MeterIndex, pAmrThreadArg->PatchTask.TimeInfo[i].flag);
                pAmrThreadArg->PatchRunTask.MeterIndex++;
                pAmrThreadArg->PatchRunTask.SucNum++;
                continue;
            }
            if(pAmrThreadArg->PatchMtrInfo[*MsIndex].FailCnt>=MAX_METER_FAILURE_COUNT)
            {
                TASK_FMT_TRACE( pipe, REC_LOG,"�ñ�ʧ�ܴ��� %d ���ٳ��� PN %d \n",pAmrThreadArg->PatchMtrInfo[*MsIndex].FailCnt,Pn);
                taskmng_mtr_result_set(pAmrThreadArg->PatchRunTask.MeterIndex,pAmrThreadArg->PatchRunTask.MtrResult);
                pAmrThreadArg->PatchRunTask.MeterIndex++;
                pAmrThreadArg->PatchRunTask.SucNum++;
                continue;
            }
            if(prtl == PRTL_OOP)
            {
                DiInfo->SubOadNum = GetPatchOopOad(pipe,Pn,DiInfo,*MsIndex,TaskInfo);
                if(DiInfo->SubOadNum>0)
                {

                    return SEARCH_SUCCESS;
                }else
                {
                    taskmng_mtr_result_set(pAmrThreadArg->PatchRunTask.MeterIndex,pAmrThreadArg->PatchRunTask.MtrResult);
                    //pAmrThreadArg->PatchTask.TimeInfo[i].MeterInfo[pAmrThreadArg->PatchRunTask.MeterIndex].flag =1;
                    taskmng_mtr_result_set(pAmrThreadArg->PatchRunTask.MeterIndex, pAmrThreadArg->PatchTask.TimeInfo[i].flag);
                    if(pAmrThreadArg->PatchRunTask.SucNum>=pAmrThreadArg->PatchRunTask.NeedMtrNum)
                    {
                        break;
                    }
                    pAmrThreadArg->PatchRunTask.MeterIndex++;
                }
            }else
            {
                ret = SearchPatch645DI(pipe,*MsIndex,prtl,TaskInfo,DiInfo->DI);
                if(ret >0)
                {

                    return SEARCH_SUCCESS;
                }else if(ret ==0)
                {
                    taskmng_mtr_result_set(pAmrThreadArg->PatchRunTask.MeterIndex,pAmrThreadArg->PatchRunTask.MtrResult);
                    //pAmrThreadArg->PatchTask.TimeInfo[i].MeterInfo[pAmrThreadArg->PatchRunTask.MeterIndex].flag =1;
                    taskmng_mtr_result_set(pAmrThreadArg->PatchRunTask.MeterIndex, pAmrThreadArg->PatchTask.TimeInfo[i].flag);
                    if(pAmrThreadArg->PatchRunTask.SucNum>=pAmrThreadArg->PatchRunTask.NeedMtrNum)
                    {
                        break;
                    }
                    pAmrThreadArg->PatchRunTask.MeterIndex++;
                }
            }
        }
        if(pAmrThreadArg->PatchRunTask.NeedMtrNum>0)
        {
            WritePatchResult(pipe,pAmrThreadArg->PatchTask.TaskId,(uint8)daycnt,i,&pAmrThreadArg->PatchTask.TimeInfo[i]);
        }
        i++;
        if(i==pAmrThreadArg->PatchTask.TimeNum)
            return SEARCH_FINISH;
        //TASK_FMT_TRACE( pipe, REC_LOG,"���� %d i %d ��ʼ����һ��ʱ��� %ld  \n",pAmrThreadArg->PatchRunTask.TaskId,i,pAmrThreadArg->PatchTask.TimeInfo[i].PatchTime);

        pAmrThreadArg->PatchRunTask.TimeIndex = i;
        patchTime = pAmrThreadArg->PatchTask.TimeInfo[i];
        cnt =0;
        pAmrThreadArg->PatchRunTask.SucNum = 0;
        pAmrThreadArg->PatchRunTask.NeedMtrNum =GetPatchNeedMtrNum(patchTime);
        if(pAmrThreadArg->PatchRunTask.NeedMtrNum>0)
        {
            TASK_FMT_TRACE( pipe, REC_LOG,"���� %d i %d ��ʼ����һ��ʱ��� %ld NeedMtrNum %d \n",pAmrThreadArg->PatchRunTask.TaskId,i,pAmrThreadArg->PatchTask.TimeInfo[i].PatchTime,pAmrThreadArg->PatchRunTask.NeedMtrNum );
        }
        pAmrThreadArg->PatchRunTask.MeterIndex=0;
        memset(pAmrThreadArg->PatchRunTask.MtrResult,0x00,128);
        for(j=0;j<pAmrThreadArg->TmnAmrInfo.MtrCnt;j++)
        {
            pAmrThreadArg->PatchMtrInfo[j].CSDIndex = 0;
            pAmrThreadArg->PatchMtrInfo[j].SigFrameNum =0;
            pAmrThreadArg->PatchMtrInfo[j].MapIndex = 0;
            pAmrThreadArg->PatchMtrInfo[j].FailCnt =0;
            InitPatchTask(TaskInfo,&pAmrThreadArg->PatchMtrInfo[j],&pAmrThreadArg->PatchRunTask,patchTime.PatchTime);
        }
    }
    return SEARCH_FINISH;
}

/*******************************************************************************
* ��������: GetPatchOopOad
* ��������:���� ��ȡ����������Ҫ������Oad
* �������:Pipe TaskInfo CurTask
* �������:DiInfo
        MsIndex     ��ǰ�߳��еĵ������
* �� �� ֵ: 0    ��ǰ���ѳ������
*******************************************************************************/
uint8 GetPatchOopOad(uint8 Pipe,uint16 Pn,DI_INFO_T* DiInfo, int MsIndex,SCHEME_TASK_RUN_INFO *TaskInfo)
{
    uint16 i;
    uint16               OadLen = 0;
    uint8                OadNum = 0;
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[Pipe];
    OadInfoMap OadMap ;
    OOP_TI_T ti;
    uint8 datalen;

    TASK_FMT_TRACE(Pipe,REC_LOG,"GetPatchOopOad CSDIndex %d SubOadNum %d \n",pAmrThreadArg->PatchMtrInfo[MsIndex].CSDIndex,TaskInfo->CsdTask.RoadTask[0].SubOadNum);
    if(pAmrThreadArg->PatchMtrInfo[MsIndex].CSDIndex >=TaskInfo->CsdTask.RoadTask[0].SubOadNum)
    {
        if(TaskInfo->CsdTask.RoadTask[0].MainOad.nObjID == 0x5002||TaskInfo->CsdTask.RoadTask[0].MainOad.nObjID == 0x5003)
        {
            ti.unit = TaskInfo->AcqInfo.TI.unit;
            ti.value = TaskInfo->AcqInfo.TI.value;
            DT_TimeAddInv(&pAmrThreadArg->PatchMtrInfo[MsIndex].StartTime, ti);

            if(pAmrThreadArg->PatchMtrInfo[MsIndex].StartTime>=pAmrThreadArg->PatchMtrInfo[MsIndex].EndTime)
            {
                pAmrThreadArg->PatchMtrInfo[MsIndex].CSDIndex = 0xff;
                pAmrThreadArg->PatchRunTask.SucNum++;
                TASK_FMT_TRACE( Pipe, REC_LOG, " ��ǰ����������%d �������  \n",Pn );

                return 0;
            }else
            {
                pAmrThreadArg->PatchMtrInfo[MsIndex].CSDIndex = 0;
            }
        }
        else
        {
            pAmrThreadArg->PatchMtrInfo[MsIndex].CSDIndex = 0xff;
            pAmrThreadArg->PatchRunTask.SucNum++;
            TASK_FMT_TRACE( Pipe, REC_LOG, " ��ǰ����������%d �������  \n",Pn );

            return 0;
        }
    }
    for(i=pAmrThreadArg->PatchMtrInfo[MsIndex].CSDIndex;i<TaskInfo->CsdTask.RoadTask[0].SubOadNum;i++)
    {
        if(taskmng_oad_info_get(TaskInfo->CsdTask.RoadTask[0].OadListInfo[i],&OadMap)==FALSE)
        {
            pAmrThreadArg->PatchMtrInfo[MsIndex].CSDIndex++;
            continue;
        }
        //TASK_FMT_TRACE(Pipe,REC_LOG,"taskmng_oop_meter_oad_get i %d oad %08x len %d \n",i,TaskInfo->CsdTask.RoadTask[0].OadListInfo[i].value,OadMap.dataLen);
        if(TaskInfo->CsdTask.RoadTask[0].OadListInfo[i].nIndex==0)
            datalen = OadMap.dataLen;
        else
            datalen = OadMap.subLen;
        if(OadLen+datalen >MAX_OAD_RD_LEN&&OadLen!=0 )
            break;
        OadLen+=datalen;
        DiInfo->SubOad[OadNum] = TaskInfo->CsdTask.RoadTask[0].OadListInfo[i];
        OadNum++;
        if(TaskInfo->CsdTask.RoadTask[0].MainOad.nObjID == 0x5002)
        {
            if(OadNum>=8)
                break;
        }
    }
    DiInfo->MainOad = TaskInfo->CsdTask.RoadTask[0].MainOad;
    pAmrThreadArg->PatchMtrInfo[MsIndex].SigFrameNum = OadNum;
    return OadNum;
}

/*******************************************************************************
* ��������: SearchPatch645DI
* ��������: ������ת���б����ҵ���Ҫת�����ֵ�������ͬ����Ϣ�����ж�����Ϣʹ��ͬһ�ֵ�����
*           ���ÿһ�����ҵ�������
* �������: Tmp_AmrLink         ��������ڵ�
*           MtrIndex            ��������
*           PrtlMode            ����Լ
*           DI                  97/07���ݱ�ʶ
* �������:
*           DI                  �ҵ����ֵ�������Ӧ��97��07DI
* �� �� ֵ: =0                  �ò��ȫ������
            >0                  ���б��ҵ�ת����Ϣ
*******************************************************************************/
int SearchPatch645DI(uint8 Pipe,uint16 MsIndex,uint8 prtl,SCHEME_TASK_RUN_INFO *TaskInfo,uint8* DI)
{
    const   MapOopTo645_NEW*  pMap;
    uint16      MapNum;
    int16      i,j;
    uint32        DI07 = 0;
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[Pipe];
    //TASK_FMT_TRACE(Pipe, REC_LOG,"SearchPatch645DI mainoad 0x%08x  \n",TaskInfo->CsdTask.RoadTask[0].MainOad.value);
    pMap = taskmng_map_get_from_mainoad(Pipe,MsIndex,TaskInfo,&MapNum);
    if(pMap == NULL)
    {
        pAmrThreadArg->PatchMtrInfo[MsIndex].CSDIndex = 0xff;
        pAmrThreadArg->PatchMtrInfo[MsIndex].MapIndex = 0;
        pAmrThreadArg->PatchRunTask.SucNum++;  //�Ҳ���Ĭ�ϳɹ�

        return 0;
    }
    //TASK_FMT_TRACE(Pipe, REC_LOG,"SearchPatch645DI PN %d  CSDIndex %d MapIndex %d \n",pAmrThreadArg->PatchMtrInfo[MsIndex].Pn,pAmrThreadArg->PatchMtrInfo[MsIndex].CSDIndex,pAmrThreadArg->PatchMtrInfo[MsIndex].MapIndex);
    //TASK_FMT_TRACE(Pipe, REC_LOG,"taskid %d SubOadNum %d MapNum %d \n",TaskInfo->TaskId,TaskInfo->CsdTask.RoadTask[0].SubOadNum,MapNum);

    for(i=pAmrThreadArg->PatchMtrInfo[MsIndex].CSDIndex;i<TaskInfo->CsdTask.RoadTask[0].SubOadNum;i++)
    {
        for(j=pAmrThreadArg->PatchMtrInfo[MsIndex].MapIndex;j<MapNum;j++)
        {
            pAmrThreadArg->PatchMtrInfo[MsIndex].CSDIndex = i;
            if(pMap[j].Oad.value ==TaskInfo->CsdTask.RoadTask[0].OadListInfo[i].value|| CurvePatchDiAdapt(Pipe,TaskInfo, MsIndex,pMap[j].Oad, pMap) >0)
            {
                if(prtl == PRTL_07&&pMap[j].DI07==0xffffffff)
                    continue;
                if(j>1)
                {   //����698��07һ��һ���Ƕ�Ӧ97����� ����ֶ�����¼
                    if(prtl == PRTL_07 && pMap[j].DI07==pMap[j-1].DI07 )
                        continue;
                }
                pAmrThreadArg->PatchMtrInfo[MsIndex].MapIndex = j;
                if(prtl==PRTL_07)
                {
                    memcpy(&DI07,&pMap[j].DI07,4);
                    if(TaskInfo->CsdTask.RoadTask[0].MainOad.nObjID==0x5003||TaskInfo->CsdTask.RoadTask[0].MainOad.nObjID==0x5004)
                    {
                        TASK_FMT_TRACE(Pipe, REC_LOG,"DI07 %08X CurNum %d EndNum %d \n",DI07,pAmrThreadArg->PatchMtrInfo[MsIndex].CurNum,pAmrThreadArg->PatchMtrInfo[MsIndex].EndNum);

                        if((DI07 & 0xFF000000) == 0x05000000)
                        {
                            DI07 = (DI07&0xFFFFFF00)|pAmrThreadArg->PatchMtrInfo[MsIndex].CurNum;
                        }
                        if(pAmrThreadArg->PatchMtrInfo[MsIndex].CurNum>pAmrThreadArg->PatchMtrInfo[MsIndex].EndNum)
                        {

                            pAmrThreadArg->PatchMtrInfo[MsIndex].CSDIndex = 0xff;
                            pAmrThreadArg->PatchMtrInfo[MsIndex].MapIndex = 0;
                            pAmrThreadArg->PatchRunTask.SucNum++;
                            return 0;
                        }
                    }
                    memcpy(DI,&DI07,4);
                }
                pAmrThreadArg->PatchMtrInfo[MsIndex].MapIndex =j;
                pAmrThreadArg->PatchMtrInfo[MsIndex].CSDIndex =i;
                TASK_FMT_TRACE(Pipe, REC_LOG,"645�ҵ���ӦDI OAD 0X%08x ��ǰCSDIndex %d  MapIndex %d\n",TaskInfo->CsdTask.RoadTask[0].OadListInfo[i].value,i,j);

                return 1;
            }

        }
        pAmrThreadArg->PatchMtrInfo[MsIndex].MapIndex =0;
        if(i ==TaskInfo->CsdTask.RoadTask[0].SubOadNum-1 )
        {
            TASK_FMT_TRACE(Pipe, REC_LOG,"CurNum %d EndNum %d \n",pAmrThreadArg->PatchMtrInfo[MsIndex].CurNum,pAmrThreadArg->PatchMtrInfo[MsIndex].EndNum);

            if(TaskInfo->CsdTask.RoadTask[0].MainOad.nObjID!=0x0000&&TaskInfo->AcqInfo.Acqtype!=ACQTYP_NULL
            &&pAmrThreadArg->PatchMtrInfo[MsIndex].CurNum<pAmrThreadArg->PatchMtrInfo[MsIndex].EndNum
            &&pAmrThreadArg->PatchMtrInfo[MsIndex].EndNum!=0xff)
            {
                pAmrThreadArg->PatchMtrInfo[MsIndex].CSDIndex = 0;
                pAmrThreadArg->PatchMtrInfo[MsIndex].MapIndex = 0;
                pAmrThreadArg->PatchMtrInfo[MsIndex].CurNum++;
                memset((uint8*)&pAmrThreadArg->PatchMtrInfo[MsIndex].storeTm,0x00,sizeof(DateTimeBcd_t));
                i=-1;
            }
        }
    }
    pAmrThreadArg->PatchMtrInfo[MsIndex].CSDIndex = 0xff;
    pAmrThreadArg->PatchMtrInfo[MsIndex].MapIndex = 0;
    pAmrThreadArg->PatchRunTask.SucNum++;
    TASK_FMT_TRACE( Pipe, REC_LOG, " ��ǰ������%d �������  \n",pAmrThreadArg->PatchMtrInfo[MsIndex].Pn );
    return 0;

}



/*******************************************************************************
* ��������: IsNeedPatch
* ��������: //�жϸ������иñ��Ƿ���Ҫ����
* �������: Tmp_AmrLink �������
        pipe �˿ں�
            MSindex �������
* �������:
* �� �� ֵ:   1��Ҫ���� 0 ����Ҫ
*******************************************************************************/
//uint8 IsNeedPatch(uint8 Pipe,uint16 MsIndex,SCHEME_TASK_RUN_INFO* Tmp_AmrLink)
//{
//
//    uint8 PrtlMode = 0;
//    uint16 Pn;
//    OOP_OAD_U MainOAD;
//    AmrThreadArg_T* pAmrThreadArg;
//    pAmrThreadArg = &gAmrThreadArg[Pipe];
//    Pn = pAmrThreadArg->pMtrAmrInfo[MsIndex].Pn;
//    taskmng_mtr_prtl_get(Pn,pAmrThreadArg->pMtrAmrInfo[MsIndex].logicid,&PrtlMode);
//    memcpy((uint8*)&MainOAD,(uint8*)&Tmp_AmrLink->CsdTask.RoadTask[0].MainOad,sizeof(OOP_OAD_U));
//    int ret = 0 ;
//    if(Tmp_AmrLink->AcqInfo.Acqtype == ACQTYP_NULL || PrtlMode == PRTL_97)
//    {
//        return ret;
//    }
//    if(MainOAD.nObjID == 0x5004 && pAmrThreadArg->pMtrAmrInfo[MsIndex].EndNum == 0xff && PrtlMode == PRTL_07)
//    {
//        return ret;
//    }
//    switch(MainOAD.nObjID)
//    {
//    case 0x0000:  //ʵʱ����
//
//        ret = 0;
//        break;
//
//    case 0x5000:  //˲ʱ����
//
//        ret = 1;
//        break;
//
//    case 0x5002:  //���Ӷ���(��������)
//        //���ж��Ƿ��ǵ����
//        if(taskmng_mtr_conntype_get(Pn,pAmrThreadArg->pMtrAmrInfo[MsIndex].logicid)==1)
//        {
//            if(taskmng_task_period_check(Tmp_AmrLink)==TRUE)
//            {
//                ret = 0;
//            }else
//            {
//                ret = 1;
//            }
//        }else
//        {
//            if(taskmng_task_period_check(Tmp_AmrLink)==TRUE||taskmng_task_period_get(Tmp_AmrLink->TaskId,Tmp_AmrLink->logicid) < 86400L)
//            {
//                ret = 0;
//            }else
//            {
//                ret = 1;
//            }
//        }
//
//        break;
//
//    case 0x5003:  //Сʱ����
//        if(taskmng_task_period_get(Tmp_AmrLink->TaskId,Tmp_AmrLink->logicid) < 86400L)
//        {
//            ret = 0;
//        }
//        else      //���ڴ���1����ͨ���������߳���
//        {
//            ret = 1;
//        }
//        break;
//
//    case 0x5004:  //�ն���
//
//        ret = 1;
//        break;
//
//    case 0x5005:  //������
//    case 0x5006:  //�¶���
//
//        ret = 1;
//        break;
//
//    default:
//        return 0;
//    }
//    return ret;
//}


/*******************************************************************************
* ��������: InitPatchTask
* ��������: //��ʼ����������
* �������: pSchemeTaskRunInfo �������
        PatchMtr ���������Ϣ
        tmpTask ������������ʱ����
        PatchTime ����ʱ���
* �������:
* �� �� ֵ:
*******************************************************************************/
void InitPatchTask(SCHEME_TASK_RUN_INFO *pSchemeTaskRunInfo,PATCH_MTR_INFO_T* PatchMtr ,PATCH_TASK_RUN_T* tmpTask, time_t PatchTime)
{

    uint8 prtl;
    taskmng_mtr_prtl_get(PatchMtr->Pn,PatchMtr->logicid,&prtl);
//    OOP_TI_T             Ti;
//    Ti.unit = pSchemeTaskRunInfo->DelayTimeUnit;
//    Ti.value = pSchemeTaskRunInfo->DelayTime;
    if(prtl == PRTL_OOP)
    {
        if(pSchemeTaskRunInfo->CsdTask.RoadTask[0].MainOad.nObjID == 0x5002 || pSchemeTaskRunInfo->CsdTask.RoadTask[0].MainOad.nObjID== 0x5003)
        {

            //��ʱ����ת��Ϊ���ӵ�λ
            if(pSchemeTaskRunInfo->AcqInfo.TI.unit == 0)
            {
                pSchemeTaskRunInfo->AcqInfo.TI.unit = 1;
                pSchemeTaskRunInfo->AcqInfo.TI.value = pSchemeTaskRunInfo->AcqInfo.TI.value/60;
            }
            else if(pSchemeTaskRunInfo->AcqInfo.TI.unit == 2)
            {
                pSchemeTaskRunInfo->AcqInfo.TI.unit = 1;
                pSchemeTaskRunInfo->AcqInfo.TI.value = pSchemeTaskRunInfo->AcqInfo.TI.value*60;
            }
            else if(pSchemeTaskRunInfo->AcqInfo.TI.unit > 2)
            {
                pSchemeTaskRunInfo->AcqInfo.TI.value = 0;
            }
            //�쳣�����ʹ��Ĭ��ֵ
            if(pSchemeTaskRunInfo->AcqInfo.TI.value==0)
            {
                pSchemeTaskRunInfo->AcqInfo.TI.unit = 1;
                pSchemeTaskRunInfo->AcqInfo.TI.value = 15;//Ĭ��15����
            }
            PatchMtr->StartTime = PatchTime;
            PatchMtr->EndTime = PatchTime+taskmng_task_period_get(pSchemeTaskRunInfo->TaskId,pSchemeTaskRunInfo->logicid);
            //DT_TimeSubInv(&PatchMtr->EndTime,Ti);

        }
        else if(pSchemeTaskRunInfo->CsdTask.RoadTask[0].MainOad.nObjID == 0x5004)
        {
            PatchMtr->StartTime = PatchTime;
            PatchMtr->EndTime = PatchTime+taskmng_task_period_get(pSchemeTaskRunInfo->TaskId,pSchemeTaskRunInfo->logicid);
            //DT_TimeSubInv(&PatchMtr->EndTime,Ti);
        }
    }
    if(prtl == PRTL_07)
    {
        PatchMtr->CurNum = 1;
        PatchMtr->EndNum = 1;
        memset((uint8*)&PatchMtr->storeTm,0x0,sizeof(DateTimeBcd_t));
        switch(pSchemeTaskRunInfo->CsdTask.RoadTask[0].MainOad.nObjID)
        {
        case 0x5002:
            //��ʱ����ת��Ϊ���ӵ�λ
            if(pSchemeTaskRunInfo->AcqInfo.TI.unit == 0)
            {
                pSchemeTaskRunInfo->AcqInfo.TI.unit = 1;
                pSchemeTaskRunInfo->AcqInfo.TI.value = pSchemeTaskRunInfo->AcqInfo.TI.value/60;
            }
            else if(pSchemeTaskRunInfo->AcqInfo.TI.unit == 2)
            {
                pSchemeTaskRunInfo->AcqInfo.TI.unit = 1;
                pSchemeTaskRunInfo->AcqInfo.TI.value = pSchemeTaskRunInfo->AcqInfo.TI.value*60;
            }
            else if(pSchemeTaskRunInfo->AcqInfo.TI.unit > 2)
            {
                pSchemeTaskRunInfo->AcqInfo.TI.value = 0;
            }
            //�쳣�����ʹ��Ĭ��ֵ
            if(pSchemeTaskRunInfo->AcqInfo.TI.value==0)
            {
                pSchemeTaskRunInfo->AcqInfo.TI.unit = 1;
                pSchemeTaskRunInfo->AcqInfo.TI.value = 15;//Ĭ��15����
            }
            PatchMtr->CurNum = 0;
            PatchMtr->EndNum = 0;
            PatchMtr->StartTime = PatchTime;
            PatchMtr->EndTime = PatchTime+taskmng_task_period_get(pSchemeTaskRunInfo->TaskId,pSchemeTaskRunInfo->logicid);
            //DT_TimeSubInv(&PatchMtr->EndTime,Ti);
            break;
        case 0x5003:


            PatchMtr->EndNum = 1;
            PatchMtr->StartTime = PatchTime;
            PatchMtr->EndTime = PatchTime+taskmng_task_period_get(pSchemeTaskRunInfo->TaskId,pSchemeTaskRunInfo->logicid);
           // DT_TimeSubInv(&PatchMtr->EndTime,Ti);
            PatchMtr->CurNum = GetPatchCurNum(PatchMtr->EndTime,0x5003);
            break;
        case 0x5004:

            PatchMtr->EndNum = 3;
            PatchMtr->StartTime = PatchTime;
            PatchMtr->EndTime = PatchTime+taskmng_task_period_get(pSchemeTaskRunInfo->TaskId,pSchemeTaskRunInfo->logicid);
            //DT_TimeSubInv(&PatchMtr->EndTime,Ti);
            PatchMtr->CurNum = GetPatchCurNum(PatchMtr->EndTime,0x5004);
            break;

        default:
            break;
       }
       if(pSchemeTaskRunInfo->AcqInfo.Acqtype == ACQTYP_LASTN)
        {
            PatchMtr->CurNum = pSchemeTaskRunInfo->AcqInfo.LastN;
            if(PatchMtr->CurNum==0)
                PatchMtr->CurNum = 1;
            PatchMtr->EndNum = PatchMtr->CurNum;
        }

    }
}



/*******************************************************************************
* ��������: GetPatchCurNum
* ��������: //���㲹��ʱСʱ������ն�����Ҫ��ǰ���ٴ�
* �������:
        PatchTime ����ʱ���
        MainOI Сʱ���� �ն���
* �������:
* �� �� ֵ:
*******************************************************************************/
uint8 GetPatchCurNum(time_t patchtime,OI MainOI)
{
    uint8 num=0;
    time_t timenow = time(NULL);
    uint32 tmptime = timenow - patchtime;
    if(MainOI==0x5003)
    {
        num = tmptime/(3600)+1;
    }
    if(MainOI==0x5004)
    {
        num = tmptime/(86400)+1;
    }
    return num;
}

/*******************************************************************************
* ��������: PatchMakeFrame
* ��������: //�鲹������
* �������:

* �������:
* �� �� ֵ:
*******************************************************************************/
uint16 PatchMakeFrame(uint8 Pipe,uint16 seqindex,time_t StartTime,FRAME_SEQ_INFO_T FrameSeqInfo,uint8 *DataBuff,SCHEME_TASK_RUN_INFO *pTmp_NormalLink)
{

    MeterPrtlFmt_T  MeterPrtlData;                      //����Լ��ʽ����
    uint32          DI07 = 0;
    int             MsIndex;
    uint8           SubOadNum;
    DateTimeBcd_t   proStartTime;
    uint16          len = 0;

    MsIndex   = FrameSeqInfo.MsIndex;       //��ǰ�߳��еĲ���������
    SubOadNum = FrameSeqInfo.DiInfo.SubOadNum;



    MeterPrtlData.DelayFlag = 0;

    MeterPrtlData.PrtlMode = FrameSeqInfo.PrtlMode;      //ͨ�Ź�Լ
    //ˢ�²ɼ�����ַ�͵���ַ
    memcpy(MeterPrtlData.MtrAddr, FrameSeqInfo.MeterAddr, 6);
    memcpy(MeterPrtlData.AcqUnitAddr, FrameSeqInfo.AcqAddr, 6);
    MeterPrtlData.MtrType = FrameSeqInfo.Mettype;

    if(MeterPrtlData.PrtlMode == PRTL_07)
    {
        MeterPrtlData.CtrlCode = 0x11;
        memcpy(&DI07,FrameSeqInfo.DiInfo.DI,4);
        if(DI07 == 0x06000001 || DI07 == 0x06010001 || DI07 == 0x06020001
                || DI07 == 0x06030001 || DI07 == 0x06040001 || (DI07&0xffff0000) == 0x06100000)     //����㳭Сʱ�������ݿ�
            MeterPrtlData.DataLen = 10;
        else
            MeterPrtlData.DataLen = 4;

        memcpy(MeterPrtlData.DataBuff, FrameSeqInfo.DiInfo.DI, MeterPrtlData.DataLen);
        if(DI07 == 0x06000001 || DI07 == 0x06010001 || DI07 == 0x06020001
                || DI07 == 0x06030001 || DI07 == 0x06040001 || (DI07&0xffff0000) == 0x06100000)
        {
            MeterPrtlData.DataLen = 10;
            MeterPrtlData.DataBuff[4] = 1;
            if((DI07&0xffff0000) == 0x06100000)
                MeterPrtlData.DataBuff[4] = taskmng_curve_point_get(pTmp_NormalLink->TaskId,pTmp_NormalLink->logicid);  //һ�γ���8����
            DT_Time2DateTimeBCD( StartTime, &proStartTime);
            memcpy_r(&MeterPrtlData.DataBuff[5], &proStartTime.year_l, 5);
        }
    }

    if(MeterPrtlData.PrtlMode == PRTL_OOP)//���������Э�����֡
    {
        #if (defined AREA_JIANGXI) || (defined AREA_ANHUI)||(defined AREA_JIANGSU)
        uint16 pn;
        pn = FrameSeqInfo.Pn;
        OOP_METER_BASIC_T MtrAddr;
        taskmng_mtr_basic_info_get(pn,0,&MtrAddr);
        if(MtrAddr.userType == 125)
        {
            len = OopTermianlPatchGetRequst(Pipe,seqindex,StartTime,MsIndex,FrameSeqInfo.DiInfo.MainOad,SubOadNum, FrameSeqInfo.DiInfo.SubOad, &MeterPrtlData.DataLen, DataBuff,pTmp_NormalLink);
        }
        else
        {
            len = OopPatchGetRequst(Pipe,seqindex,StartTime,MsIndex,FrameSeqInfo.DiInfo.MainOad,SubOadNum, FrameSeqInfo.DiInfo.SubOad, &MeterPrtlData.DataLen, DataBuff,pTmp_NormalLink);
        }
        #else
        len = OopPatchGetRequst(Pipe,seqindex,StartTime,MsIndex,FrameSeqInfo.DiInfo.MainOad,SubOadNum, FrameSeqInfo.DiInfo.SubOad, &MeterPrtlData.DataLen, DataBuff,pTmp_NormalLink);
        #endif
    }
    else
    {
        len = taskmng_meter_prtl_645_pack(DataBuff, MeterPrtlData);
    }
    return len;
}

/*******************************************************************************
* ��������: OopPatchGetRequst
* ��������: //��698Э�鲹������
* �������:

* �������:
* �� �� ֵ:
*******************************************************************************/
int OopPatchGetRequst(uint8 Pipe,uint8 seqindex, time_t StartTime,int MsIndex,OOP_OAD_U MainOad,uint8 SubOadNum,OOP_OAD_U *SubOad,uint16 *FrameLen, uint8 *FrameBuff,SCHEME_TASK_RUN_INFO *pTmp_NormalLink)
{
    OOP_TSA_T                   TsaAddr;
    LinkLayer_t*            pLinkLayer = NULL;
    uint8                   rsd[30];
    uint8                   rsdLen=0;
    OOP_OAD_U                    RsdOad;
    GetRecord_t             GetRecord;
    uint8                    InDataBuf[255];
    uint16                  offset = 0;
    uint16                  i = 0;

    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[Pipe];

    TsaAddr = taskmng_tsa_get(pAmrThreadArg->PatchMtrInfo[MsIndex].Pn,pAmrThreadArg->PatchMtrInfo[MsIndex].logicid);
#ifdef AREA_HUNAN
    //������ʶ��ģ���߼��豸����3
    uint8 flag = 0;
    for(i = 0; i < SubOadNum; i++)
    {
        if(SubOad[i].nObjID == 0x4800)
        {
            flag = 1;
            break;
        }
    }
    if(flag==1)
    {
        TsaAddr.len++;
        TsaAddr.flag = 1;
        TsaAddr.vxd = 3;
    }
#endif
    RsdOad.value = 0x20210200;

    rsdLen = MakePatchRsdFrame(Pipe,rsd,pTmp_NormalLink,RsdOad,MsIndex,StartTime);
    GetRecord.rsd = rsd;
    GetRecord.rsdLen = rsdLen;

    GetRecord.rcsd = InDataBuf;

    offset = 1;
    GetRecord.rcsd[offset++] = 0;
    GetRecord.rcsd[offset++] = (uint8)(RsdOad.nObjID >> 8);
    GetRecord.rcsd[offset++] = (uint8)(RsdOad.nObjID);
    GetRecord.rcsd[offset++] = RsdOad.attID;
    GetRecord.rcsd[offset++] = RsdOad.nIndex;

    for(i = 0; i < SubOadNum; i++)
    {
        if(SubOad[i].nObjID != RsdOad.nObjID)
        {
            GetRecord.rcsd[offset++] = 0;

            GetRecord.rcsd[offset++] = (uint8)(SubOad[i].nObjID >> 8);
            GetRecord.rcsd[offset++] = (uint8)(SubOad[i].nObjID);
            GetRecord.rcsd[offset++] = SubOad[i].attID;
            GetRecord.rcsd[offset++] = SubOad[i].nIndex;
        }
    }

    GetRecord.rcsd[0] = (offset-1)/5;
    GetRecord.rcsdLen = offset;
    GetRecord.oad = MainOad;


    OopGet.requst(oop_pipe_get(Pipe, seqindex),&TsaAddr,3,(uint8*)&GetRecord,sizeof(GetRecord_t));
    pLinkLayer = FindLinkLayer(oop_pipe_get(Pipe, seqindex), NULL);
    *FrameLen = pLinkLayer->sendLen;
    memcpy(FrameBuff,pLinkLayer->sendBuff,*FrameLen);

    return  *FrameLen;
}


/*******************************************************************************
* ��������: OopPatchGetRequst
* ��������: //��698Э�鲹������
* �������:

* �������:
* �� �� ֵ:
*******************************************************************************/
int MakePatchRsdFrame(uint8 Pipe,uint8 *RsdBuf,SCHEME_TASK_RUN_INFO *pTmp_NormalLink,OOP_OAD_U RsdOad,int MsIndex,time_t StartTime)
{
    uint8                   RsdLen = 0;
    //DateTimeBcd_t           TempTimeBcd;
    time_t                  TempTimeSec;
    OOP_TI_T                    DelayTi;

    RsdBuf[RsdLen++] = 2;    //ѡ�񷽷�2

    //��������������  OAD
    RsdBuf[RsdLen++] = (uint8)(RsdOad.nObjID>> 8);
    RsdBuf[RsdLen++] = (uint8)(RsdOad.nObjID);
    RsdBuf[RsdLen++] = RsdOad.attID;
    RsdBuf[RsdLen++] = RsdOad.nIndex;

    RsdBuf[RsdLen++] = DATE_TIME_S;//��ʼʱ����������


    TempTimeSec = StartTime;
    if(pTmp_NormalLink->CsdTask.RoadTask[0].MainOad.nObjID < 0x5000 || pTmp_NormalLink->CsdTask.RoadTask[0].MainOad.nObjID > 0x5003)
        TempTimeSec = TempTimeSec+60;
    else
        TempTimeSec = StartTime;
    DT_Time2DateTimeHex(TempTimeSec, (DateTimeHex_t*)(RsdBuf+RsdLen));
    RsdLen += 7;

    RsdBuf[RsdLen++] = DATE_TIME_S;//����ʱ����������

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

    //���ݼ��
    RsdBuf[RsdLen++] = eTI;
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
    return RsdLen;
}

/*******************************************************************************
* ��������: RefreshPatchPnState
* ��������: ���²������״̬
* �������: Pipe �˿ں�
        SeqIndex ��������

* �������:
* �� �� ֵ:
*******************************************************************************/
void RefreshPatchPnState(uint8 Pipe,uint8 SeqIndex)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[Pipe];
    uint16 Pn = pAmrThreadArg->AmrSendFrameInfo.Queue[SeqIndex].Pn;
    uint16 MtrIndex = pAmrThreadArg->AmrSendFrameInfo.Queue[SeqIndex].MsIndex;

     pAmrThreadArg->AmrSendFrameInfo.Queue[SeqIndex].ValidFlag = 0;
    if( pAmrThreadArg->AmrSendFrameInfo.QueueCnt> 0)
        pAmrThreadArg->AmrSendFrameInfo.QueueCnt--;

    //ʧ�ܴ�����1
    if(pAmrThreadArg->PatchMtrInfo[MtrIndex].FailCnt < 0xff)
        pAmrThreadArg->PatchMtrInfo[MtrIndex].FailCnt++;

    printf("Sn = %d ����ʧ�ܴ���:%d\n",Pn,pAmrThreadArg->PatchMtrInfo[MtrIndex].FailCnt);

    return;
}

/*******************************************************************************
* ��������: JudgePatchMoveNextAmrTaskIndex
* ��������: ��֤645���������Ƿ���ȷ
* �������: Pipe �˿ں�
        MsIndex �������
        pTmp_NormalLink ������Ϣ
        pRecvDI  �յ���������
        CtrlCode ������
* �������:
* �� �� ֵ:   1 ��֤�ɹ� -1 ʧ��
*******************************************************************************/
int JudgePatchMoveNextAmrTaskIndex(uint8 Pipe,uint16 MsIndex,SCHEME_TASK_RUN_INFO* pTmp_NormalLink,uint8 *pRecvDI, char CtrlCode, uint16* DataLen)
{

    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[Pipe];
    const MapOopTo645_NEW *pMap = NULL;
    uint16                 MapNum = 0;
    uint16 Pn;
    uint8 prtl;

    uint16                 MapIndex;
    uint32                 SendDI = 0;
    uint32                 RecvDI = 0;

    Pn = pAmrThreadArg->PatchMtrInfo[MsIndex].Pn;
    taskmng_mtr_prtl_get(Pn,pAmrThreadArg->PatchMtrInfo[MsIndex].logicid,&prtl);
    pMap = taskmng_map_get_from_mainoad(Pipe,MsIndex,pTmp_NormalLink,&MapNum);
    if(pMap==NULL)
        return -1;
    MapIndex = pAmrThreadArg->PatchMtrInfo[MsIndex].MapIndex;
    if(prtl == PRTL_07)
    {
        if(pRecvDI != NULL)
            memcpy(&RecvDI,pRecvDI,4);
        memcpy(&SendDI,&pMap[MapIndex].DI07,4);
        if((SendDI & 0xfff00000) == 0x05000000)    //�����෢��ʱ���޸�DI
            SendDI = ((SendDI & 0xffffff00)|pAmrThreadArg->PatchMtrInfo[MsIndex].CurNum);
    }
    TASK_FMT_TRACE(Pipe, REC_LOG, "�յ���DI %08x SendDI %08x \n",RecvDI,SendDI);
     //��֤���͵����ݱ�ʶ����յ������ݱ�ʶ�Ƿ�һ��
    if(pRecvDI != NULL)
    {
        if((RecvDI != SendDI) || ((CtrlCode & 0x80) == 0))
            return -1;
    }
    else
    {
        printf("�յ�����֡ \n");
    }
    return 1;
}

/*******************************************************************************
* ��������: JudgePatchFrz07MoveNext
* ��������: ���ݽ��������һ��645������
* �������: Pipe �˿ں�
        MsIndex �������
        Tmp_AmrLink ������Ϣ
        Result  �������1�ɹ� 0ʧ��

* �������:
* �� �� ֵ:
*******************************************************************************/
void JudgePatchFrz07MoveNext(uint8 Pipe,SCHEME_TASK_RUN_INFO* Tmp_AmrLink,int MsIndex, uint8 Result)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[Pipe];

    time_t              endTime;
    uint16              Pn;
    uint8               PrtlMode = 0;

    Pn = pAmrThreadArg->PatchMtrInfo[MsIndex].Pn;
    taskmng_mtr_prtl_get(Pn,pAmrThreadArg->PatchMtrInfo[MsIndex].logicid,&PrtlMode);
    if(Tmp_AmrLink->CsdTask.RoadTask[0].MainOad.nObjID == 0x5003&& taskmng_task_period_get(Tmp_AmrLink->TaskId,Tmp_AmrLink->logicid) < 86400L)
        return;
    if(Tmp_AmrLink->CsdTask.RoadTask[0].MainOad.nObjID == 0x5004 && pAmrThreadArg->PatchMtrInfo[MsIndex].EndNum == 0xff)
        return;
    if(Tmp_AmrLink->CsdTask.RoadTask[0].MainOad.nObjID == 0x5002)
    {
        if(taskmng_mtr_conntype_get(Pn,pAmrThreadArg->PatchMtrInfo[MsIndex].logicid)==1)
        {
            if(pAmrThreadArg->pMtrAmrInfo[MsIndex].flag645==1 ||(pAmrThreadArg->pMtrAmrInfo[MsIndex].flag645!=0&& taskmng_task_period_check(Tmp_AmrLink)==TRUE))
            {
                return;
            }
    //        if(taskmng_task_period_check(Tmp_AmrLink)==TRUE)
    //        {
    //            return;
    //        }
        }else
        {
             if(pAmrThreadArg->pMtrAmrInfo[MsIndex].flag645!=0)
            {
                return ;
            }
    //        if(taskmng_task_period_check(Tmp_AmrLink)==TRUE||taskmng_task_period_get(Tmp_AmrLink->TaskId,Tmp_AmrLink->logicid) < 86400L)
    //        {
    //            return ;
    //        }
        }
    }
    if(Tmp_AmrLink->CsdTask.RoadTask[0].MainOad.nObjID!=0x0000)
    {
        if(Tmp_AmrLink->AcqInfo.Acqtype == ACQTYP_FRZTM||Tmp_AmrLink->AcqInfo.Acqtype ==ACQTYP_TI)
        {
            if(Tmp_AmrLink->CsdTask.RoadTask[0].MainOad.nObjID!=0x5002)
            {
                if(Result==0&&pAmrThreadArg->PatchMtrInfo[MsIndex].CSDIndex==0)
                {
                    //������ʱ��ط��ϣ����ٳ����������������
                    pAmrThreadArg->PatchMtrInfo[MsIndex].CSDIndex= 0xfe; //��һ���ϴ�Ĳ�����0xff�����������������������������
                    pAmrThreadArg->PatchMtrInfo[MsIndex].EndNum = 1;      //���ټ�������ʱ��
                    return;
                }
                if(pAmrThreadArg->PatchMtrInfo[MsIndex].storeTm.year_h==0)
                {
                    if(pAmrThreadArg->PatchMtrInfo[MsIndex].CurNum <= pAmrThreadArg->PatchMtrInfo[MsIndex].EndNum)
                    {
                        pAmrThreadArg->PatchMtrInfo[MsIndex].CSDIndex = 0;
                        pAmrThreadArg->PatchMtrInfo[MsIndex].MapIndex = 0;
                        pAmrThreadArg->PatchMtrInfo[MsIndex].CurNum++;//����������һ����¼
                    }
                }
                if(Tmp_AmrLink->CsdTask.RoadTask[0].MainOad.nObjID == 0x5004 && pAmrThreadArg->PatchMtrInfo[MsIndex].EndNum == 0
                        &&pAmrThreadArg->PatchMtrInfo[MsIndex].CurNum < 2)
                        pAmrThreadArg->PatchMtrInfo[MsIndex].EndNum = 0xff;
            }
            else
            {
                //�������ߴ���
                OOP_TI_T TI;
                TI.unit = Tmp_AmrLink->AcqInfo.TI.unit;
                TI.value = Tmp_AmrLink->AcqInfo.TI.value;

                //if(IsCnntStyleOne(Pn,pAmrThreadArg->PatchMtrInfo[MsIndex].logicid, Tmp_AmrLink) > 0)
                if(taskmng_mtr_conntype_get(Pn,pAmrThreadArg->pMtrAmrInfo[MsIndex].logicid)==1 && pAmrThreadArg->pMtrAmrInfo[MsIndex].flag645==0)
                {
                    pAmrThreadArg->PatchMtrInfo[MsIndex].CSDIndex++;    //������һ������
                    pAmrThreadArg->PatchMtrInfo[MsIndex].MapIndex = 0;
                    TASK_FMT_TRACE( Pipe, REC_LOG, "������������ csdindex %d mapindex %d \n",pAmrThreadArg->PatchMtrInfo[MsIndex].CSDIndex,pAmrThreadArg->PatchMtrInfo[MsIndex].MapIndex);

                    if(pAmrThreadArg->PatchMtrInfo[MsIndex].CSDIndex < Tmp_AmrLink->CsdTask.RoadTask[0].SubOadNum)
                        return;
                    TI.unit = TASK_CYCLE_FLAG_MIN;
                    TI.value = 15*taskmng_curve_point_get( Tmp_AmrLink->TaskId, Tmp_AmrLink->logicid);;    //һ��15min*8����
                }

                DT_TimeAddInv(&pAmrThreadArg->PatchMtrInfo[MsIndex].StartTime, TI);

                endTime = pAmrThreadArg->PatchMtrInfo[MsIndex].EndTime;

                //��ȥ����ʼʱ�����������ʱ
                TI.unit = Tmp_AmrLink->DelayTimeUnit;
                TI.value = Tmp_AmrLink->DelayTime;
                DT_TimeSubInv(&endTime, TI);


                if(pAmrThreadArg->PatchMtrInfo[MsIndex].StartTime < endTime)
                {
                    pAmrThreadArg->PatchMtrInfo[MsIndex].CSDIndex = 0;
                    pAmrThreadArg->PatchMtrInfo[MsIndex].MapIndex = 0;
                }
                else
                {
                    pAmrThreadArg->PatchMtrInfo[MsIndex].CSDIndex = 0xfe; //��һ���ϴ�Ĳ�����0xff�����������������������������
                    pAmrThreadArg->PatchMtrInfo[MsIndex].MapIndex = 0;
                }
            }
        }
    }

}


/*******************************************************************************
* ��������: WritePatchResult
* ��������: //����������ɺ�д�ز������
* �������: Pipe �˿ں�
            taskid �����

* �������:
* �� �� ֵ:
*******************************************************************************/
void WritePatchResult(uint8 pipe,uint16 taskid,uint8 DayCnt,uint8 timeindex,PATCH_TIME_T *meterinfo)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    char    pfile[100] = {0};
    sprintf(pfile,"%s/%s%03d_%02d_PIPE%02d",PrivateDir,PATCH_FILE,taskid,DayCnt,pipe);
//    uint16 i,j;
//    for(i=0;i<pAmrThreadArg->PatchTask.TimeNum;i++)
//    {
//        for(j=0;j<pAmrThreadArg->PatchTask.TimeInfo[i].MeterNum;j++)
//        {
//            TASK_FMT_TRACE( pipe, REC_LOG, "����ʱ��� %d pn %d flag %d \n",i,pAmrThreadArg->PatchTask.TimeInfo[i].MeterInfo[j].Pn,pAmrThreadArg->PatchTask.TimeInfo[i].MeterInfo[j].flag);
//        }
//    }
    if(access(pfile,R_OK)!=0)
    {
        return ;
    }

    PATCH_TASK_T PatchTask;
    memset(&PatchTask,0x00,sizeof(PATCH_TASK_T));
    read_pdata_xram(pfile,(char*)&PatchTask,0,sizeof(PATCH_TASK_T));
    memcpy(&PatchTask.TimeInfo[timeindex],meterinfo,sizeof(PATCH_TIME_T));

    write_pdata_xram(pfile,(char*)&pAmrThreadArg->PatchTask,0,sizeof(PATCH_TASK_T));

}



/*******************************************************************************
* ��������: GetPatchNeedMtrNum
* ��������: //��ȡ��Ҫ�����ĵ�����
* �������: Pipe �˿ں�
            taskid �����

* �������:
* �� �� ֵ:  ��Ҫ�����ĵ�����
*******************************************************************************/
uint16 GetPatchNeedMtrNum(PATCH_TIME_T PatchTime)
{
    uint16 i;
    uint16 num =0;
    for(i=0;i<PatchTime.MeterNum;i++)
    {
        if(taskmng_mtr_result_check(i,PatchTime.flag)==FALSE)
            num++;
    }
    return num;
}


/*******************************************************************************
* ��������: taskmng_up_msg_fun_deal
* ��������: //�����շ���Ϣ
* �������: IID ��ϢIID
        IOP ��ϢIOP
        len ���͵���Ϣ����
        PayLoad ��Ϣ����
        DestAddr ����Ŀ�Ķ�

* �������: RecvLoadLen �յ��������Ϣ���ݳ���
            RecvPayLoad �յ�����Ϣ����
* �� �� ֵ:  1 �ɹ� -1 ʧ��
*******************************************************************************/
int taskmng_up_msg_fun_deal(uint16 IID,uint16 IOP,uint16 len,uint8 *PayLoad,char* DestAddr,uint16 *RecvLoadLen,uint8 *RecvPayLoad)
{
    ACTIVE_COMM_STATE_T         Stat = ACT_COMM_IDLE;
    uint16                      SendLen = 0;
    static time_t               CommBeginTime;                      //ͨѶ��ʼʱ��
    uint8                       RecvBuf[MAX_MSG_LEN];
    int                         RecvLen;
    int8                       RecvResult =0 ;
    uint8                       RetryCnt=0;
    MESSAGE_INFO_T *MsgRecv = NULL;
    uint16 MsgIndex;
    while(1)
    {
        switch(Stat)
        {
        case ACT_COMM_IDLE:
            Stat = ACT_COMM_SEND;
            break;
        case ACT_COMM_SEND:
            SendLen = PackUpMsgData(IID,IOP,len,PayLoad,DestAddr,&MsgIndex);
            if(SendLen >0)
            {
                CommBeginTime= time(NULL);
                Stat = ACT_COMM_RECV;
            }
            break;
        case ACT_COMM_RECV:
            memset(RecvBuf,0x00,MAX_MSG_LEN);
            RecvLen = taskmng_plc_queue_read(APPArg.RecvBuf, RecvBuf);
            if(RecvLen>0)
            {
                TASK_FMT_DEBUG(-1, UP_LOG, "�յ��ظ� len =%d \n",RecvLen);

                MsgRecv = (MESSAGE_INFO_T *)RecvBuf;
                TASK_FMT_DEBUG(-1, UP_LOG, "IID 0x%04x IOP 0x%04x MsgIndex %d SendIndex %d \n",MsgRecv->IID,MsgRecv->IOP,MsgRecv->index,MsgIndex);
                if(IID==MsgRecv->IID && IOP==MsgRecv->IOP &&MsgIndex==MsgRecv->index )
                {
                    *RecvLoadLen = MsgRecv->msglen;
                    memcpy(RecvPayLoad,MsgRecv->playload,*RecvLoadLen);

                    TASK_BUF_TRACE(-1, UP_LOG, RecvPayLoad, *RecvLoadLen);
                    return 1;
                }else if(0x0000==MsgRecv->IID && 0x0014==MsgRecv->IOP)
                {
                    TASK_FMT_DEBUG(-1, UP_LOG, "�յ�00000014���������Ϣ \n");
                    return -1;
                }else
                {
                    RecvResult = -1;
                }
            }else
                RecvResult = -1;
            if(RecvResult == -1)
            {
                if(abs(time(NULL)-CommBeginTime)>MAX_MSG_DELAY_TIME)
                {
                    RetryCnt++;
                    Stat = ACT_COMM_RETRY;
                    TASK_FMT_DEBUG(-1, UP_LOG, "��ʱ����RetryCnt %d \n",RetryCnt);
                }
            }

            break;
        case ACT_COMM_RETRY:
            if(RetryCnt>3)
            {
                Stat = ACT_COMM_IDLE;
                return -1;
            }
            Stat = ACT_COMM_SEND;
            break;
        default:
            break;
        }
        usleep(20000);
    }
    return 1;
}

/*******************************************************************************
* ��������: PackUpMsgData
* ��������: ��������Ϣ
* �������:
* �������: ��
* �� �� ֵ: ����֡����
*******************************************************************************/
uint16 PackUpMsgData(uint16 IID,uint16 IOP,uint16 len,uint8 *PayLoad,char* DestAddr,uint16 *MsgIndex)
{

    MESSAGE_INFO_T *MsgSend = (MESSAGE_INFO_T*)malloc(sizeof(MESSAGE_INFO_T)+len);
    memset(MsgSend,0x00,sizeof(MESSAGE_INFO_T)+len);
    MsgSend->rpm = 1;
    MsgSend->priority = 1;
    MsgSend->index = GetMsgSeq();
    *MsgIndex = MsgSend->index ;
    MsgSend->label = 0;
    MsgSend->sourlen = strlen(APPNAME);
    memcpy(MsgSend->souraddr,APPNAME,MsgSend->sourlen);
    MsgSend->destlen = strlen(DestAddr);
    memcpy(MsgSend->destaddr,DestAddr,MsgSend->destlen );
    TASK_FMT_DEBUG(-1,UP_LOG, "SourLen %d SourAddr %s DestLen %d DestAddr %s \n",MsgSend->sourlen,MsgSend->souraddr,MsgSend->destlen,MsgSend->destaddr);
    MsgSend->IID = IID;
    MsgSend->IOP = IOP;
    MsgSend->msglen = len;
    memcpy(MsgSend->playload,PayLoad,len);
    TASK_FMT_DEBUG(-1, UP_LOG, "Send IID=0X%04x IOP=0x%04x MsgIndex %d \n",IID,IOP,MsgSend->index);

    TASK_BUF_DEBUG(-1, UP_LOG, PayLoad, len);
    taskmng_plc_queue_write ( APPArg.SendBuf, (uint8*)MsgSend, sizeof(MESSAGE_INFO_T)+len );
    free(MsgSend);
    return sizeof(MESSAGE_INFO_T)+len;

}

/*******************************************************************************
* ��������: DBSendMQTT
* ��������: //ͨ��mqtt���������ķ�������
* �������: label ��Ϣ��ǩ
        IOP ��ϢIOP
        payload ��Ϣ����
        len ���͵���Ϣ����

* �������: pindex ��Ϣ���
* �� �� ֵ:  >0�ɹ� -1 ʧ��
*******************************************************************************/
int DBSendMQTT(uint16 label,uint16 iop,uint8* payload,uint16 len,uint16* pindex)
{
    //uint8 log[MAX_MSG_LEN] = {0};
    MESSAGE_INFO_T *pMsgInfo = NULL;
    uint8 *buf = NULL;
    uint32 SendLen;
    uint16 msglen = 0;
    int ret;
    char topic[100];
    sprintf(topic,"%s/%s",APPNAME,DBCENTER_NAME);
    msglen = sizeof(MESSAGE_INFO_T) + len;
    pMsgInfo = (MESSAGE_INFO_T *)malloc(msglen);
    if(pMsgInfo == NULL)
    {
        return -1;
    }
    memset(pMsgInfo,0x00,msglen);
    pMsgInfo->rpm = 1;
    pMsgInfo->priority =1;
    *pindex = GetMsgSeq();
    pMsgInfo->index = *pindex;
    pMsgInfo->label = label;
    pMsgInfo->sourlen = strlen(APPNAME);
    memcpy(pMsgInfo->souraddr,APPNAME,pMsgInfo->sourlen);
    pMsgInfo->destlen = strlen(DBCENTER_NAME);
    memcpy(pMsgInfo->destaddr,DBCENTER_NAME,pMsgInfo->destlen);
    pMsgInfo->IID = 0X0005;
    pMsgInfo->IOP = iop;
    pMsgInfo->msglen = len;
    memcpy(pMsgInfo->playload,payload,len);

    SendLen = appmsg_package(pMsgInfo, &buf);
    if(SendLen == 0)
    {
        return -1;
    }
    //TASK_FMT_TRACE(-1,MQTT_LOG,"DBSendMQTT IOP %04X \n",iop);
   // TASK_BUF_TRACE(-1,MQTT_LOG,buf,SendLen);
    pthread_mutex_lock(&gMqttSendLock);
    ret = mqtt_topic_write(buf,SendLen,topic);
    pthread_mutex_unlock(&gMqttSendLock);
    free(buf);
    free(pMsgInfo);
    if(ret>0)
        return 0;
    else
        return ret;

}



/*******************************************************************************
* ��������: taskmng_local_addr_read
* ��������: //��ȡ�ն�ͨ�ŵ�ַ
* �������:
* �������: addr �ն˵�ַ
* �� �� ֵ:  0�ɹ� -1 ʧ��
*******************************************************************************/
int taskmng_local_addr_read(uint8 *addr)
{
    int   ret;
    uint32 len=0;
    NOMAL_OAD_T NormalOad;
    NormalOad.logicId = 0;
    NormalOad.oad.value = 0x40010200;
    NormalOad.infoNum = 0;
    OOP_OCTETVAR16_T LocalAddr;
    memset(&LocalAddr, 0, sizeof(OOP_OCTETVAR16_T));

    ret = db_read_nomal(gDBClientMain,&NormalOad,sizeof(OOP_OCTETVAR16_T),(uint8*)&LocalAddr,&len);

    if(ret !=0 ||len!=sizeof(OOP_OCTETVAR16_T))
        return -1 ;

    memcpy_r(addr,LocalAddr.value,6);
    TASK_FMT_TRACE(-1,UP_LOG,"��ȡ�ն˵�ַ 0x%02x%02x%02x%02x%02x%02x \n",addr[5],addr[4],addr[3],addr[2],addr[1],addr[0]);
    //��ccoRouter����һ��
    uint8 i;
    for(i=0;i<PLC_NUM;i++)
    {
        IsSetAddr[i]=0;
    }
    return 0;
}





/*******************************************************************************
* ��������: DBUpdateFun
* ��������: //�������������ϱ������ݸ����¼�
* �������: ptMsg ��Ϣ����
* �������:
* �� �� ֵ:
*******************************************************************************/
void DBUpdateFun(MESSAGE_INFO_T *ptMsg)
{
    int ret;
    uint8 payload[200] = {0};
    uint8 logicid;
    uint16 infonum;
    OOP_OAD_U oad;
    OOP_OAD_U MasterDI;
    uint16 index=0;
    uint16 classtag;
    DB_UPDATE_T dbupdate;
    memset(&dbupdate,0x00,sizeof(DB_UPDATE_T));
    memset(&MasterDI,0x00,sizeof(OOP_OAD_U));
    memcpy(payload,ptMsg->playload,ptMsg->msglen);
    logicid = payload[index++];
    memcpy_r((uint8*)&infonum,&payload[index],2);
    index+=2;
    memcpy_r((uint8*)&classtag,&payload[index],2);
    index+=2;
    //�������ʶ optional
    if(payload[index++]==0)
        return ;
    //���ĵĵ��������������¼� û����di ֻ��һ��di
#ifdef AREA_FUJIAN
    if(payload[index++]==1) //������DI
    {
        memcpy_r((uint8*)&MasterDI,&payload[index],4);
        index+=4;
    }
#else
    if(payload[index++]!=0)
        return ;
#endif

    if(payload[index++]!=1)
        return ;
    memcpy_r((uint8*)&oad,&payload[index],4);

    if(logicid!=LOGIC_ID_YC && logicid !=LOGIC_ID_PD)
        return ;
    dbupdate.logicid = logicid;
    dbupdate.infonum = infonum;
    dbupdate.oad = oad;
#ifdef AREA_FUJIAN
    dbupdate.MasterOad = MasterDI;
    //ȫ�־���������oad���0x6E130200 ��oad��ľ��������� ��Ϣ�����͸�����񷽰���
    if(MasterDI.value == 0x6E130200)
    {
        ret = taskmng_db_update_queue_write ( &pTaskDBUpdate, &dbupdate);
        TASK_FMT_DEBUG(-1,MQTT_LOG,"�յ������������¼� д����  ret %d  \n",ret);
        return;
    }
#endif
    switch(oad.value)
    {
    case 0x60000200:   //��������
    {
        TASK_FMT_DEBUG(-1,MQTT_LOG,"�յ����ݸ����¼� logicid  %d infonum %d oad %08x \n",logicid,infonum,oad.value);
        ret = taskmng_db_update_queue_write ( &pMeterFileUpdate, &dbupdate);
        TASK_FMT_DEBUG(-1,MQTT_LOG,"taskmng_db_update_queue_write  ret %d  \n",ret);
    }
        break;

#ifdef AREA_FUJIAN
    case 0x6E050200:
    {
        TASK_FMT_DEBUG(-1,MQTT_LOG,"�յ�Ⱥ�鵵�������¼� logicid  %d infonum %d oad %08x \n",logicid,infonum,oad.value);
        ret = taskmng_db_update_queue_write ( &pGropMeterFileUpdate, &dbupdate);
        TASK_FMT_DEBUG(-1,MQTT_LOG,"taskmng_db_update_queue_write  ret %d  \n",ret);
    }
    break;
#endif
    case 0x60020800:
    case 0x60020900:    //
    case 0x42040200:
    case 0x42040300:
    case 0xF2090600:
    case 0xF2090900:
    case 0xF2090B00:
    case 0xF2091600:
    case 0x60020200:
    case 0x60020500:
#ifdef AREA_HUNAN
    case 0x60E10200:
    case 0x60E20200:
    case 0x60E30200:
    case 0x60E40200:
    case 0xF2096300:
    case 0xF2096500:
#endif
#ifdef AREA_FUJIAN
    case 0x6E030300:
#endif
    /*240531 �յ��ն�ͣ�ϵ�����̴߳�������� ԭ����ÿ�������̵߳��߼�������  */
    case 0x31060200:
    {
        TASK_FMT_DEBUG(-1,MQTT_LOG,"�յ����ݸ����¼� logicid %d infonum %d oad %08x \n",logicid,infonum,oad.value);
        ret = task_send_msg(gMqttSock,MSG_DBUPDATE,(uint8*)&dbupdate,sizeof(DB_UPDATE_T),IPC_MAIN);

        TASK_FMT_DEBUG(-1,MQTT_LOG,"task_send_msg  ret %d  \n",ret);
    }
        break;
    case 0x60120200:    //�������
    case 0x60140200:        //��ͨ�ɼ�����
    case 0x60160200:
    case 0x60180200:
    case 0x601E0200:    //ת���ܷ���
#ifdef AREA_FUJIAN
    case 0x6E110200:    //ȫ��͸�����񷽰���
    case 0x6E130200:    //ȫ��͸����������
    case 0x6E11FFFF:    //��չ�ķ������ȼ�����
#endif
    {
        TASK_FMT_DEBUG(-1,MQTT_LOG,"�յ����ݸ����¼�logicid %d  infonum %d oad %08x \n",logicid,infonum,oad.value);
        //ret = task_send_msg(gMqttSock,MSG_DBUPDATE,(uint8*)&dbupdate,sizeof(DB_UPDATE_T),IPC_TASK);
        ret = taskmng_db_update_queue_write ( &pTaskDBUpdate, &dbupdate);
        TASK_FMT_DEBUG(-1,MQTT_LOG,"taskmng_db_update_queue_write  ret %d  \n",ret);
    }
        break;
#ifdef AREA_JIANGXI
    case 0x31060200:
    {
        // �����ն�ͣ�ϵ�
        uint16  pipe;
        TASK_FMT_DEBUG(-1,MQTT_LOG,"�յ��ն�ͣ�ϵ��¼� logicid %d infonum %d oad %08x \n",logicid,infonum,oad.value);

        for (pipe = 0; pipe < PLC_NUM; pipe++)
        {
            ret = task_send_msg(gMqttSock, MSG_TMN_POWER_ON_OFF, (uint8*)&dbupdate, sizeof(DB_UPDATE_T), IPC_AMR+pipe);
            TASK_FMT_DEBUG(-1,MQTT_LOG,"task_send_msg  ret %d  \n",ret);
        }
    }
        break;
#endif
    default:
        break;
    }

}






/*******************************************************************************
* ��������: SearchMeterUpFun
* ��������://�յ��ѱ��������������̴߳�����������
* �������: buf ��Ϣ����
            len ��Ϣ����
* �������:
* �� �� ֵ:
*******************************************************************************/
void SearchMeterUpFun(uint8* buf,uint16 len)
{
    uint16 index=0;
    uint8 pipe;
    int ret;
    OOP_OAD_U pipeOAD ;
    memset(&pipeOAD,0x00,sizeof(OOP_OAD_U));
    memcpy_r((uint8*)&pipeOAD.value,&buf[index],4);
    index+=4;
    pipe = taskmng_mtr_pipe_get(&pipeOAD);

    if(pipe!=0)
        return ;
    ret = taskmng_plc_queue_write(&gMeterSearchQueue, buf, len);
    //ret = task_send_msg(gMqttSock,MSG_SEARCH_METER,buf,len,IPC_AMR+pipe);
    TASK_FMT_TRACE(-1,MQTT_LOG,"�����ѱ���Ϣ ret %d pipe %d \n",ret,pipe);
}


/*******************************************************************************
* ��������: MsgErrorFunc
* ��������://�����յ��Ĵ�����Ϣ
* �������: MsgRecv �յ�����Ϣ
        MSGLen �յ�����Ϣ����
* �������:
* �� �� ֵ:
*******************************************************************************/
void MsgErrorFunc(MESSAGE_INFO_T *MsgRecv,int MSGLen)
{
    uint8 payload[200];
    uint16 index=0;
    uint16 IID,IOP;
    memcpy(payload,MsgRecv->playload,MsgRecv->msglen);

    memcpy_r((uint8*)&IID,&payload[index],2);
    index+=2;
    memcpy_r((uint8*)&IOP,&payload[index],2);
    index+=2;

    switch(IID)
    {
    case 0x0004:
    {
        if(IOP== 0X0010)
        {
            taskmng_plc_queue_write(APPArg.RecvBuf,(uint8*)MsgRecv, MSGLen );
        }
    }
        break;
    default:
        break;
    }

}





/*******************************************************************************
* ��������: CheckTaskRecordFile
* ��������://���ÿ�������˽���ļ��Ƿ���� ��û���򴴽� ˽���ļ��洢ÿ������ÿ���ĳ���״̬
* �������: pipe �˿ں�

* �������:
* �� �� ֵ:
*******************************************************************************/
void CheckTaskRecordFile(uint8 pipe)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    char      pfile[100]={0};
    uint8 i;
    uint8 TaskID;
    uint8 logicid;
    //TASK_FMT_TRACE(pipe, REC_LOG, "�����¼���� pAmrThreadArg->TaskNum is %d\n",pAmrThreadArg->TaskNum);
    for(i=0;i<pAmrThreadArg->TaskNum;i++)
    {
        TaskID = pAmrThreadArg->pTaskRunInfo[i].TaskId;
        logicid = pAmrThreadArg->pTaskRunInfo[i].logicid;
        memset(pfile,0x00,sizeof(pfile));
        sprintf(pfile,"%s/%s%03d_PIPE%02d_Logic%d",PrivateDir,TASK_RECORD_FILE,TaskID,pipe,logicid);

        if(access(pfile,R_OK)==0)        //���������ļ�¼�ļ���������һ��
        {
            TASK_FMT_DEBUG(pipe, REC_LOG, "�����¼�ļ�%s ����\n",pfile);
            char      pfile[100]={0};
            memset(pfile, 0x00, sizeof(pfile));
            sprintf(pfile, "%s/%s%03d_PIPE%02d_Logic%d", PrivateDir, TASK_RECORD_FILE, TaskID, pipe, logicid);
            Task_Record_T TaskRecord;
            memset(&TaskRecord, 0x00, sizeof(Task_Record_T));
            read_pdata_xram(pfile, (char*)&TaskRecord, 0, sizeof(Task_Record_T));
            //��������¼�ļ��汾
            if(strcmp(TaskRecord.FileVersion, TASK_RECORD_VERSION) != 0)
            {
                TASK_FMT_TRACE(pipe, REC_LOG, "�����¼�ļ��汾���� ɾ��\n");
                remove(pfile);
                CreateTaskRecordFile(pipe,TaskID,logicid);
            }
            else
            {
                //����������¼�ļ����� �Է����������Բ���
                CheckTaskRecordFileMeter(pipe,TaskID,logicid);
            }
            continue;
        }
        CreateTaskRecordFile(pipe,TaskID,logicid);

    }
}


/*******************************************************************************
* ��������: CheckTaskRecordFileMeter
* ��������://����������¼�ļ����� �Է����������Բ���
* �������: pipe �˿ں�
            TaskId �����
* �������:
* �� �� ֵ:
*******************************************************************************/
void CheckTaskRecordFileMeter(uint8 pipe,uint16 TaskId,uint8 logicid)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    SCHEME_TASK_RUN_INFO TaskInfo;
    memset(&TaskInfo,0x00,sizeof(SCHEME_TASK_RUN_INFO));
    uint16 i,j;
    char      pfile[100]={0};
    memset(pfile,0x00,sizeof(pfile));
    sprintf(pfile,"%s/%s%03d_PIPE%02d_Logic%d",PrivateDir,TASK_RECORD_FILE,TaskId,pipe,logicid);
    Task_Record_T TaskRecord,TaskRecordTmp;
    memset(&TaskRecord,0X00,sizeof(Task_Record_T));
    memset(&TaskRecordTmp,0X00,sizeof(Task_Record_T));
    read_pdata_xram(pfile, (char*)&TaskRecord,0, sizeof(Task_Record_T));
    TaskRecordTmp.TaskId = TaskRecord.TaskId;
    TaskRecordTmp.MeterCount = pAmrThreadArg->TmnAmrInfo.MtrCnt;
    strcpy(TaskRecordTmp.FileVersion, TASK_RECORD_VERSION);
    memcpy(TaskRecordTmp.ThisTurnBeginTime,TaskRecord.ThisTurnBeginTime,7);
    for(i=0;i<pAmrThreadArg->TmnAmrInfo.MtrCnt;i++)
    {
        if(pAmrThreadArg->pMtrAmrInfo[i].logicid == logicid)
        {
            TaskRecordTmp.MeterRecord[i].Pn = pAmrThreadArg->pMtrAmrInfo[i].Pn;
            for(j=0;j<TaskRecord.MeterCount;j++)
            {
                if(TaskRecordTmp.MeterRecord[i].Pn == TaskRecord.MeterRecord[j].Pn)
                {
                    TaskRecordTmp.MeterRecord[i].Result = TaskRecord.MeterRecord[j].Result;
                    memcpy(TaskRecordTmp.MeterRecord[i].EventReportSeq,TaskRecord.MeterRecord[j].EventReportSeq,sizeof(EVENT_REPORT_SEQ_T)*MAX_EVENT_ROAD_NUM);
                    break;
                }
            }
        }
    }
    taskmng_task_info_get( TaskId, logicid,&TaskInfo);
    if(TaskInfo.SchemeType==EVENTPLAN)
    {
        for(i = 0;i<TaskRecordTmp.MeterCount;i++)
        {
            //����¼��б�û��ֵ ����ֵ
            if(TaskRecordTmp.MeterRecord[i].EventReportSeq[0].EventMainOad.value == 0x00000000)
            {
                for(j = 0;j<TaskInfo.CsdTask.RoadNum;j++)
                {
                    TaskRecordTmp.MeterRecord[i].EventReportSeq[j].EventMainOad = TaskInfo.CsdTask.RoadTask[j].MainOad;
                }
            }
        }

    }
    write_pdata_xram(pfile,(char*)&TaskRecordTmp,0,sizeof(Task_Record_T));

}


/*******************************************************************************
* ��������: CreateTaskRecordFile
* ��������://���������¼�ļ�
* �������: pipe �˿ں�
            TaskId �����
* �������:
* �� �� ֵ:
*******************************************************************************/
void CreateTaskRecordFile(uint8 pipe,uint16 TaskId,uint8 logicid)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    uint16 i;
    char      pfile[100]={0};
    memset(pfile,0x00,sizeof(pfile));
    sprintf(pfile,"%s/%s%03d_PIPE%02d_Logic%d",PrivateDir,TASK_RECORD_FILE,TaskId,pipe,logicid);
    Task_Record_T TaskRecord;
    memset(&TaskRecord,0X00,sizeof(Task_Record_T));
    CTS_RUN_TASK_T *RunTask = NULL;
    SCHEME_TASK_RUN_INFO TaskInfo;
    memset(&TaskInfo,0x00,sizeof(SCHEME_TASK_RUN_INFO));
    EVENT_REPORT_SEQ_T EventReportSeq[MAX_EVENT_ROAD_NUM];
    memset(EventReportSeq,0x00,sizeof(EVENT_REPORT_SEQ_T)*MAX_EVENT_ROAD_NUM);
    taskmng_task_info_get( TaskId, logicid,&TaskInfo);
    taskmng_run_task_get( pipe,TaskId,logicid,&RunTask);
    TASK_FMT_DEBUG(pipe, REC_LOG,"TaskId %d RunTask->NeedMtrNum %d \n",TaskId,RunTask->NeedMtrNum);
    //�������û��Ҫ���ı� �Ͳ����������¼�ļ�
    if(RunTask->NeedMtrNum == 0)
        return;
    if(TaskInfo.SchemeType==EVENTPLAN)
    {
        for(i=0;i<TaskInfo.CsdTask.RoadNum;i++)
        {
            EventReportSeq[i].EventMainOad = TaskInfo.CsdTask.RoadTask[i].MainOad;
            EventReportSeq[i].ReportSeq = 0xffffffff; //698���¼����0��Ч
        }
    }

    TaskRecord.TaskId = TaskId;
    TaskRecord.MeterCount = pAmrThreadArg->TmnAmrInfo.MtrCnt;
    strcpy(TaskRecord.FileVersion, TASK_RECORD_VERSION);
    for(i=0;i<pAmrThreadArg->TmnAmrInfo.MtrCnt;i++)
    {
        TaskRecord.MeterRecord[i].Pn = pAmrThreadArg->pMtrAmrInfo[i].Pn;
        TaskRecord.MeterRecord[i].Result = 0;
        memcpy(TaskRecord.MeterRecord[i].EventReportSeq,EventReportSeq,sizeof(EVENT_REPORT_SEQ_T)*MAX_EVENT_ROAD_NUM);
    }
    TASK_FMT_DEBUG(pipe, REC_LOG,"���������¼�ļ�%s \n",pfile);
    write_pdata_xram(pfile,(char*)&TaskRecord,0,sizeof(Task_Record_T));
}





/*******************************************************************************
* ��������: GetTaskReportSeq
* ��������://�������ļ��ж�ȡ�ñ��Ӧ�¼������ϱ����
* �������: pipe �˿ں�
            TaskId �����
            MainOad �¼���OAD
            Pn ������
* �������:
* �� �� ֵ: �ñ���¼��ѳ������¼���� 698���¼����0����Ч�� ����û�оͷ���0xffffffff
*******************************************************************************/
uint32 GetTaskReportSeq(uint8 pipe,uint16 TaskId,uint8 logicid,OOP_OAD_U MainOad,uint16 Pn)
{
    Task_Record_T TaskRecord;
    memset(&TaskRecord,0x00,sizeof(Task_Record_T));
    uint16 i;
    uint8 j;
    int ret;
    char      pfile[100]={0};
    memset(pfile,0x00,sizeof(pfile));
    sprintf(pfile,"%s/%s%03d_PIPE%02d_Logic%d",PrivateDir,TASK_RECORD_FILE,TaskId,pipe,logicid);

    ret = read_pdata_xram(pfile, (char*)&TaskRecord,0, sizeof(Task_Record_T));
    //TASK_FMT_TRACE( pipe, COMM_LOG, "��ȡ�ļ����1111 ret %d\n", ret);
    if(ret!=0)
        return 0xffffffff;
    //TASK_FMT_TRACE( pipe, REC_LOG, "��ȡ�ļ� %s ret %d MeterCount %d  \n", pfile,ret,TaskRecord.MeterCount);
    for(i=0;i<TaskRecord.MeterCount;i++)
    {
        if(TaskRecord.MeterRecord[i].Pn == Pn)
        {
            for(j=0;j<MAX_EVENT_ROAD_NUM;j++)
            {
                if(TaskRecord.MeterRecord[i].EventReportSeq[j].EventMainOad.value == MainOad.value)
                    return TaskRecord.MeterRecord[i].EventReportSeq[j].ReportSeq;
            }
        }
    }
    return 0xffffffff;
}

/*******************************************************************************
* ��������: SetTaskReportSeq
* ��������:�������ļ������øñ��Ӧ�¼������
* �������: pipe �˿ں�
            TaskId �����
            MainOad �¼���OAD
            Pn ������
            seq �¼����
* �������:
* �� �� ֵ:
*******************************************************************************/
void SetTaskReportSeq(uint8 pipe,uint16 TaskId,uint8 logicid,OOP_OAD_U MainOad,uint16 Pn,uint32 seq)
{
    Task_Record_T TaskRecord;
    memset(&TaskRecord,0x00,sizeof(Task_Record_T));
    uint16 i;
    uint8 j;
    int ret;
    char      pfile[100]={0};
    memset(pfile,0x00,sizeof(pfile));
    sprintf(pfile,"%s/%s%03d_PIPE%02d_Logic%d",PrivateDir,TASK_RECORD_FILE,TaskId,pipe,logicid);

    ret = read_pdata_xram(pfile, (char*)&TaskRecord,0, sizeof(Task_Record_T));
    if(ret!=0)
    {
        TASK_FMT_TRACE( pipe, REC_LOG, "��ȡ�����¼�ļ�ʧ�� \n");
        return;
    }
    TASK_FMT_TRACE( pipe, REC_LOG, "SetTaskReportSeq Mainoad %08x pn %d seq %d   \n", MainOad.value,Pn,seq);
    TASK_FMT_TRACE( pipe, REC_LOG, "SetTaskReportSeq ��ȡ�ļ� %s ret %d MeterCount %d  \n", pfile,ret,TaskRecord.MeterCount);
    for(i=0;i<TaskRecord.MeterCount;i++)
    {
        if(TaskRecord.MeterRecord[i].Pn == Pn)
        {
            for(j=0;j<MAX_EVENT_ROAD_NUM;j++)
            {
                TASK_FMT_TRACE( pipe, REC_LOG, "j %d Mainoad %08x  \n", j,TaskRecord.MeterRecord[i].EventReportSeq[j].EventMainOad.value);
                if(TaskRecord.MeterRecord[i].EventReportSeq[j].EventMainOad.value == MainOad.value)
                {
                    TaskRecord.MeterRecord[i].EventReportSeq[j].ReportSeq = seq;
                    write_pdata_xram(pfile,(char*)&TaskRecord,0,sizeof(Task_Record_T));
                    TASK_FMT_TRACE( pipe, REC_LOG, "д���¼���� PN %d EventMainOad %08x seq %d   \n",Pn,MainOad.value,seq );
                    return;
                }
            }
        }
    }
    TASK_FMT_TRACE( pipe, REC_LOG, "д���¼����ʧ�� \n");

}

/*******************************************************************************
* ��������: GetTaskReportEventState
* ��������://�������ļ��ж�ȡ�ñ��Ӧ�¼��ķ���״̬
* �������: pipe �˿ں�
            TaskId �����
            MainOad �¼���OAD
            Pn ������
* �������:
* �� �� ֵ: �ñ���¼��ķ���״̬
*******************************************************************************/
uint8 GetTaskReportEventState(uint8 pipe, uint16 TaskId, uint8 logicid, OOP_OAD_U MainOad, uint16 Pn)
{
    Task_Record_T   TaskRecord;
    uint16          i;
    uint8           j;
    int             ret;
    char            pfile[100]={0};

    memset(&TaskRecord, 0x00, sizeof(Task_Record_T));
    memset(pfile, 0x00, sizeof(pfile));
    sprintf(pfile,"%s/%s%03d_PIPE%02d_Logic%d", PrivateDir, TASK_RECORD_FILE, TaskId, pipe, logicid);

    ret = read_pdata_xram(pfile, (char*)&TaskRecord, 0, sizeof(Task_Record_T));
    if(ret != 0)
        return 0;
    for(i=0; i<TaskRecord.MeterCount; i++)
    {
        if(TaskRecord.MeterRecord[i].Pn == Pn)
        {
            for(j=0; j<MAX_EVENT_ROAD_NUM; j++)
            {
                if(TaskRecord.MeterRecord[i].EventReportSeq[j].EventMainOad.value == MainOad.value)
                    return TaskRecord.MeterRecord[i].EventReportSeq[j].isHappen;
            }
        }
    }
    return 0;
}

/*******************************************************************************
* ��������: SetTaskReportEventState
* ��������:�������ļ������øñ��Ӧ�¼��ķ���״̬
* �������: pipe �˿ں�
            TaskId �����
            MainOad �¼���OAD
            Pn ������
            state �¼�����״̬
* �������:
* �� �� ֵ:
*******************************************************************************/
void SetTaskReportEventState(uint8 pipe, uint16 TaskId, uint8 logicid, OOP_OAD_U MainOad, uint16 Pn, uint8 state)
{
    Task_Record_T   TaskRecord;
    uint16          i;
    uint8           j;
    int             ret;
    char            pfile[100]={0};

    memset(&TaskRecord,0x00,sizeof(Task_Record_T));
    memset(pfile,0x00,sizeof(pfile));
    sprintf(pfile,"%s/%s%03d_PIPE%02d_Logic%d", PrivateDir, TASK_RECORD_FILE, TaskId, pipe, logicid);

    ret = read_pdata_xram(pfile, (char*)&TaskRecord, 0, sizeof(Task_Record_T));
    if(ret!=0)
        return;
    for(i=0;i<TaskRecord.MeterCount;i++)
    {
        if(TaskRecord.MeterRecord[i].Pn == Pn)
        {
            for(j=0;j<MAX_EVENT_ROAD_NUM;j++)
            {
                if(TaskRecord.MeterRecord[i].EventReportSeq[j].EventMainOad.value == MainOad.value)
                {
                    TaskRecord.MeterRecord[i].EventReportSeq[j].isHappen = state;
                    write_pdata_xram(pfile, (char*)&TaskRecord, 0, sizeof(Task_Record_T));
                    return;
                }
            }
        }
    }

}






/*******************************************************************************
* ��������: SetTGRecordFile
* ��������:�ܼ��鳭���ɹ����������¼�ļ�����Ϊ�ɹ�
* �������:
            TaskId ����id
* �������:
* �� �� ֵ:
*******************************************************************************/
void SetTGRecordFile(uint16 TaskId)
{
    Task_Record_T TaskRecord;
    char      pfile[100]={0};
    memset(pfile,0x00,sizeof(pfile));
    sprintf(pfile,"%s/%s%03d",PrivateDir,TASK_RECORD_FILE,TaskId);

    read_pdata_xram(pfile,(char*) &TaskRecord,0, sizeof(Task_Record_T));
    TaskRecord.MeterRecord[0].Result = 1;
    write_pdata_xram(pfile,(char*)&TaskRecord,0,sizeof(Task_Record_T));

}






/*******************************************************************************
* ��������: GetTaskState
* ��������://��ȡ����״̬
* �������: TaskId �����
* �������:
* �� �� ֵ: ����״̬
*******************************************************************************/
uint8 GetTaskState(uint16 TaskId,uint8 logicid)
{
    uint8 State = 0;
    int            i;
    pthread_rwlock_rdlock(&SchemeTaskCfgList.rwLock);

    for(i=0; i<SchemeTaskCfgList.TaskNum; i++)
    {
        if(SchemeTaskCfgList.OopTaskCfg[i].TaskId == TaskId && SchemeTaskCfgList.OopTaskCfg[i].logicid == logicid)
        {
            State = SchemeTaskCfgList.OopTaskCfg[i].TaskState;
            break;
        }
    }

    pthread_rwlock_unlock(&SchemeTaskCfgList.rwLock);
    return State;

}


/*******************************************************************************
* ��������: ErrorUpMsg
* ��������://���лظ�������Ϣ 0000 0014
* �������: MsgInfo ��Ϣ����
* �������:
* �� �� ֵ:
*******************************************************************************/
void ErrorUpMsg(MESSAGE_INFO_T* MsgInfo )
{
    MESSAGE_INFO_T* MsgSend=NULL;
    uint8 payload[20]={0};
    uint8 index=0;
    uint16 errorcode = 0;
    TASK_FMT_TRACE(-1, UP_LOG, "���лظ�������Ϣ \n");

    memcpy_r(&payload[index],(uint8*)&MsgInfo->IID,2);
    index+=2;
    memcpy_r(&payload[index],(uint8*)&MsgInfo->IOP,2);
    index+=2;
    memcpy_r(&payload[index],(uint8*)&errorcode,2);
    index+=2;

    MsgSend = (MESSAGE_INFO_T*)malloc(sizeof(MESSAGE_INFO_T)+index);
    memset(MsgSend,0x00,sizeof(MESSAGE_INFO_T)+index);

    MsgSend->rpm = 0;
    MsgSend->index = MsgInfo->index;
    MsgSend->priority = MsgInfo->priority;
    MsgSend->label = MsgInfo->label;
    MsgSend->IID = 0x0000;
    MsgSend->IOP = 0x0014;

    MsgSend->sourlen = MsgInfo->destlen;
    memcpy(MsgSend->souraddr,MsgInfo->destaddr,MsgSend->sourlen);
    MsgSend->destlen = MsgInfo->sourlen;
    memcpy(MsgSend->destaddr,MsgInfo->souraddr,MsgSend->destlen );


    memcpy(MsgSend->playload,payload,index);
    MsgSend->msglen = index;

    taskmng_plc_queue_write(APPArg.SendBuf, (uint8*)MsgSend, sizeof(MESSAGE_INFO_T)+index);
    free(MsgSend);
}




/*******************************************************************************
* ��������: MakeDateTimeS
* ��������://��timeתΪdate_time_s��ʽ����䵽buf��
* �������: time ʱ���

* �������: buf date_time_s��ʽ
* �� �� ֵ:
*******************************************************************************/
uint16 MakeDateTimeS(uint8* buf,time_t time)
{
    uint16 index=0;
    OOP_DATETIME_S_T datetimes;
    datetimes = Time_tToDataTimeS(time);
    memcpy_r(&buf[index],(uint8*)&datetimes.year,2);
    index+=2;
    buf[index++] = datetimes.month;
    buf[index++] = datetimes.day;
    buf[index++] = datetimes.hour;
    buf[index++] = datetimes.minute;
    buf[index++] = datetimes.second;
    return index;
}


/*******************************************************************************
* ��������: CheckMeterSearchResult
* ��������://����ѱ����Ƿ��ظ� ����ظ���д����������
* �������:pipe �˿ں�
        searchresult �ѱ���
* �������: infonum д�����������õ���Ϣ���
* �� �� ֵ: TRUE û���ظ��� FALSE���ظ���
*******************************************************************************/
BOOL CheckMeterSearchResult(uint8 pipe,OOP_SEARCHMET_RES_T searchresult,uint16 *infonum)
{

    uint16 i;
    int index = -1;
    uint8 tmpaddr[6];
    pthread_mutex_lock(&MeterSearchResult.lock);
    TASK_FMT_DEBUG( pipe, RELAY_LOG, "CheckMeterSearchResult infonum %d count %d \n",MeterSearchResult.infonum,MeterSearchResult.count);
    for(i=0;i<MeterSearchResult.count;i++)
    {
        memcpy_r(tmpaddr,searchresult.commAdd.add,6);
        if(searchresult.proto ==MeterSearchResult.result[i].prtl &&
            memcmp(tmpaddr,MeterSearchResult.result[i].Addr,6)==0)
        {
            if(pipe == MeterSearchResult.result[i].pipe)
            {
                pthread_mutex_unlock(&MeterSearchResult.lock);
                return FALSE;
            }else
            {
                //��¼��ͬ ���Ƕ˿ںŲ�ͬ
                index = i;
                *infonum = MeterSearchResult.result[i].infonum;
                break;
            }
        }
    }

    if(index==-1)
    {
        MeterSearchResult.infonum++;
        MeterSearchResult.result[MeterSearchResult.count].prtl = searchresult.proto;
        MeterSearchResult.result[MeterSearchResult.count].pipe = pipe;
        MeterSearchResult.result[MeterSearchResult.count].infonum = MeterSearchResult.infonum;
        memcpy_r(MeterSearchResult.result[MeterSearchResult.count].Addr,searchresult.commAdd.add,6);
        MeterSearchResult.count++;

        *infonum = MeterSearchResult.infonum;
    }
    TASK_FMT_DEBUG( pipe, RELAY_LOG, "CheckMeterSearchResult ��Ӽ�¼ infonum  %d count %d \n",MeterSearchResult.infonum,MeterSearchResult.count);

    pthread_mutex_unlock(&MeterSearchResult.lock);
    return TRUE;
}


/*******************************************************************************
* ��������: EventReportDeal
* ��������:�¼��ϱ�����
* �������:pipe �˿ں�

* �������:
* �� �� ֵ: 1 ������� -1 δ���
*******************************************************************************/
int EventReportDeal(uint8 pipe)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    static time_t  BeginTime[PLC_NUM];
    uint8 RecvBuf[MAX_MSG_LEN]={0};
    const uint16  OverTime = 30; //��ʱʱ��
    int RecvLen;
    int ret;
    MESSAGE_INFO_T *MsgInfo=NULL;
    uint8 Payload[MAX_MSG_LEN] = {0};

    uint8 monitorParallFlag = 0;
#if MONITOR_PARALL_FUN
    if(pAmrThreadArg->TmnAmrInfo.MtrCnt > 100)
        monitorParallFlag = 1;
#endif

    uint8 MtrIndex = pAmrThreadArg->EventReport.CurIndex;
    uint16 Pn = pAmrThreadArg->EventReport.MtrEvent[MtrIndex].Pn;
    if(Pn == 0)
    {
        TASK_FMT_DEBUG( pipe, RELAY_LOG, "�¼��ϱ�������Ч MtrIndex %d \n",MtrIndex);
        return 1;
    }
    if(FALSE==taskmng_meter_event_report_time_check())
    {
        usleep(100000);
        return -1;
    }
    //uint8 prtl = pAmrThreadArg->EventReport.MtrEvent[MtrIndex].prtl;
    //���ÿ���ϱ����¼�����200���򲻴���
    //if(pAmrThreadArg->EventReport.AllCount>200)
     //   return 1;
    switch(pAmrThreadArg->TotalState.EventReportState)
    {
    case INNER_RTS_IDLE:
    {
        pAmrThreadArg->TotalState.EventReportState = INNER_RTS_SEND;
    }
        break;
    case INNER_RTS_SEND:
    {
        ret = EventReportSendFrame(pipe,monitorParallFlag);
        if(ret ==1)
        {
            BeginTime[pipe] = time(NULL);
            pAmrThreadArg->TotalState.EventReportState = INNER_RTS_RECV;
        }else if(ret == 2)
        {
            break;
        }else
        {
            pAmrThreadArg->TotalState.EventReportState = INNER_RTS_IDLE;
            return 1;
        }
    }
        break;
    case INNER_RTS_RECV:
    {
        if(monitorParallFlag ==1)
            RecvLen = taskmng_plc_queue_read ( pAmrRecvDown_Cb[pipe], RecvBuf );
        else
            RecvLen = taskmng_plc_queue_read ( pAmrRecvDown[pipe], RecvBuf );

        if(RecvLen>0)
        {
            MsgInfo = (MESSAGE_INFO_T*)RecvBuf;
            memcpy(Payload,MsgInfo->playload,MsgInfo->msglen);
            TASK_FMT_DEBUG( pipe, RELAY_LOG, "�¼��ϱ��յ���Ϣ IID %04X IOP %04X index %d payload \n",MsgInfo->IID,MsgInfo->IOP,MsgInfo->index);

            TASK_BUF_DEBUG( pipe, RELAY_LOG,MsgInfo->playload, MsgInfo->msglen);


            if(monitorParallFlag == 1)
            {
                if(MsgInfo->IOP != 0x0031 || MsgInfo->index != pAmrThreadArg->EventReport.SendIndex)
                    break;

                //�Ա��µ�ַ�Ƿ��Ӧ
                OOP_TSA_T       Tsa = taskmng_tsa_get(Pn, LOGIC_ID_YC);
                uint8           addr[6] =
                {
                    0
                };
                uint16          index = 0;
                uint8           lenoffset;
                uint16          framelen = 0;

                index += 4;                                     //�˿ں�
                memcpy(addr, Payload + index, 6);
                index += 6;

                if(memcmp(addr, Tsa.add, 6) != 0)
                {
                    TASK_FMT_DEBUG(pipe, RELAY_LOG, "�յ��ĵ�ַ����Ӧ \n");
                    break;
                }

                index++;
                framelen = XADR_GetEleNum(Payload + index, &lenoffset);

                if(framelen == 0)
                {
                    pAmrThreadArg->TotalState.EventReportState = INNER_RTS_RETRY;
                    break;
                }

                EventReportRecvFun(pipe, Payload + index, framelen + lenoffset);
            }
            else
            {
                if(MsgInfo->IOP != 0x0038 || MsgInfo->index != pAmrThreadArg->EventReport.SendIndex)
                    break;

                if(MsgInfo->msglen <= 1)
                {
                    pAmrThreadArg->TotalState.EventReportState = INNER_RTS_RETRY;
                    break;
                }

                EventReportRecvFun(pipe, Payload, MsgInfo->msglen);
            }

            pAmrThreadArg->TotalState.EventReportState = INNER_RTS_SEND;
        }else
        {
            if(abs(time(NULL)-BeginTime[pipe])>OverTime)
            {
                TASK_FMT_DEBUG( pipe, RELAY_LOG, "�¼��ϱ����񳬹�30sδ�յ��ظ� \n");

                pAmrThreadArg->TotalState.EventReportState = INNER_RTS_RETRY;
            }
        }
    }
        break;
    case INNER_RTS_RETRY:
    {
        pAmrThreadArg->EventReport.MtrEvent[MtrIndex].FailCnt++;
        TASK_FMT_DEBUG( pipe, RELAY_LOG, "Pn %d ʧ�ܴ��� %d  \n",Pn,pAmrThreadArg->EventReport.MtrEvent[MtrIndex].FailCnt);
        pAmrThreadArg->TotalState.EventReportState = INNER_RTS_SEND;
    }
        break;
    default:
        break;
    }
    return -1;
}

/*******************************************************************************
* ��������: EventReportDeal
* ��������:�¼��ϱ�����
* �������:pipe �˿ں�

* �������:
* �� �� ֵ: 1 ������� -1 δ���
*******************************************************************************/
int RS485EventReportDeal(uint8 pipe)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    //static time_t  BeginTime[MAX_485_NUM];
    uint8 RecvBuf[MAX_MSG_LEN]={0};
    //const uint16  OverTime = 10; //��ʱʱ��
    int RecvLen;
    int ret;
//    MSG_INFO_T *MsgInfo=NULL;
//    uint8 Payload[MAX_MSG_LEN] = {0};

    uint8 MtrIndex = pAmrThreadArg->EventReport.CurIndex;
    uint16 Pn = pAmrThreadArg->EventReport.MtrEvent[MtrIndex].Pn;
    //uint8 prtl = pAmrThreadArg->EventReport.MtrEvent[MtrIndex].prtl;
    //���ÿ���ϱ����¼�����200���򲻴���
//    if(pAmrThreadArg->EventReport.AllCount>200)
//        return 1;
    while(1)
    {
        switch(pAmrThreadArg->TotalState.EventReportState)
        {
            case INNER_RTS_IDLE:
                {
                    pAmrThreadArg->TotalState.EventReportState = INNER_RTS_SEND;
                }
                break;
    
            case INNER_RTS_SEND:
                {
                    ret = Rs485EventReportSendFrame(pipe);
    
                    if(ret == 1)
                    {
                       // BeginTime[pipe] = time(NULL);
                        pAmrThreadArg->TotalState.EventReportState = INNER_RTS_RECV;
                    }
                    else if(ret == 2)
                    {
                        break;
                    }
                    else
                    {
                        pAmrThreadArg->TotalState.EventReportState = INNER_RTS_IDLE;
                        return 1;
                    }
                }
                break;
    
            case INNER_RTS_RECV:
                {
                    RecvLen = rs485_RS232_recv(pipe, RecvBuf, sizeof(RecvBuf));
    
                    if(RecvLen > 0)
                    {
                        Rs485EventReportRecvFun(pipe, RecvBuf, RecvLen);
                        pAmrThreadArg->TotalState.EventReportState = INNER_RTS_SEND;
                    }
                    else
                    {
                        //if(abs(time(NULL) -BeginTime[pipe] )> OverTime)
                        {
                            TASK_FMT_DEBUG(pipe, RELAY_LOG, "�¼��ϱ�����ʱ \n");
    
                            pAmrThreadArg->TotalState.EventReportState = INNER_RTS_RETRY;
                        }
                    }
                }
                break;
    
            case INNER_RTS_RETRY:
                {
                    pAmrThreadArg->EventReport.MtrEvent[MtrIndex].FailCnt++;
                    TASK_FMT_DEBUG(pipe, RELAY_LOG, "Pn %d ʧ�ܴ��� %d  \n", Pn,
                         pAmrThreadArg->EventReport.MtrEvent[MtrIndex].FailCnt);
                    pAmrThreadArg->TotalState.EventReportState = INNER_RTS_SEND;
                }
                break;
    
            default:
                break;
        }
    }
    
    return -1;
}
#ifdef AREA_ZHEJIANG
void Change_circuit_Breaker_Data_to_Struct(uint8* tmpbuf ,uint16 len,uint8 pipe)
{

    uint8 singalnum=0;
    uint8 offset=0;
    uint8 tmpBuff[30];
    uint8 i=0;
    Oop_Identify_Result IdentifyInformations;
    uint8 buf[256];
    uint8 codelen=0;

    memcpy(buf,tmpbuf,len);
    ReduceGap(buf, len, 0x33);

    TASK_FMT_DEBUG( pipe, RELAY_LOG,"Change_circuit_Breaker_Data_to_Struct \n");
    TASK_BUF_DEBUG( pipe, RELAY_LOG,buf, len);
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    uint8 MtrIndex = pAmrThreadArg->EventReport.CurIndex;
    uint16 Pn = pAmrThreadArg->EventReport.MtrEvent[MtrIndex].Pn;
    OOP_OAD_U OAD;

    memset(&IdentifyInformations,0x00,sizeof(Oop_Identify_Result));
    singalnum = buf[offset++];
    if(singalnum>4)
    {
        return;
    }
    IdentifyInformations.Identifynum=singalnum;
    //��������Ϣ����
    codelen = buf[offset++];
    for(i=0;i<singalnum;i++)
    {

        memset(tmpBuff,0x00,30);
        //������ΪBIN��ʽ 1d 15 11 1d 0a 14
        memcpy_r(tmpBuff,buf+offset,6);
        offset =offset +6;

        IdentifyInformations.IdentifyInformation[i].Suctime.year = 2000+tmpBuff[0];
        IdentifyInformations.IdentifyInformation[i].Suctime.month = tmpBuff[1];
        IdentifyInformations.IdentifyInformation[i].Suctime.day = tmpBuff[2];
        IdentifyInformations.IdentifyInformation[i].Suctime.hour = tmpBuff[3];
        IdentifyInformations.IdentifyInformation[i].Suctime.minute = tmpBuff[4];
        IdentifyInformations.IdentifyInformation[i].Suctime.second = tmpBuff[5];


        //IdentifyInformations.IdentifyInformation[i].signalstatus = buf[offset++];
        IdentifyInformations.IdentifyInformation[i].signalphase  = buf[offset++];

        memcpy(&IdentifyInformations.IdentifyInformation[i].Signalstrength1,buf+offset,4);
        offset = offset+4;

        memcpy(&IdentifyInformations.IdentifyInformation[i].Signalstrength2,buf+offset,4);
        offset = offset+4;

        //memcpy((uint8 *)&IdentifyInformations.IdentifyInformation[i].BaseStrength,buf+offset,4);
        //offset = offset+4;

        memcpy((uint8 *)&IdentifyInformations.IdentifyInformation[i].BackgroundNoise,buf+offset,4);
        offset = offset+4;
        //Ĭ��ΪAAE9 aa e9 00 00 puAmr�鱨��ʱ�ᷴ���� ������ߴ���

        memcpy_r(IdentifyInformations.IdentifyInformation[i].Signatureinformation.nValue,buf+offset,codelen);
        offset = offset+codelen;

        IdentifyInformations.IdentifyInformation[i].Extendedinformation.nNum = 12;
        memcpy(IdentifyInformations.IdentifyInformation[i].Extendedinformation.value,buf+offset,12);
        offset+=12;
    }
    OAD.value =0x4E070200;
    taskmng_normal_data_write_pipe( pipe, (uint8*)&IdentifyInformations, sizeof(IdentifyInformations), OAD, Pn, 0);

}
#endif
#ifdef AREA_JIANGXI
#if 0
void Change_circuit_Breaker_Data_to_Struct(uint8* tmpbuf ,uint16 len,uint8 pipe)
{

    uint8 singalnum=0;
    uint8 offset=0;
    uint8 tmpBuff[30];
    uint8 i=0;
    Oop_Identify_Result IdentifyInformations;
    uint8 buf[256];
    //uint8 codelen=0;

    memcpy(buf, tmpbuf, len);
    ReduceGap(buf, len, 0x33);

    TASK_FMT_DEBUG( pipe, RELAY_LOG,"Change_circuit_Breaker_Data_to_Struct \n");
    TASK_BUF_DEBUG( pipe, RELAY_LOG,buf, len);
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    uint8 MtrIndex = pAmrThreadArg->EventReport.CurIndex;
    uint16 Pn = pAmrThreadArg->EventReport.MtrEvent[MtrIndex].Pn;
    OOP_OAD_U OAD;

    memset(&IdentifyInformations,0x00,sizeof(Oop_Identify_Result));
    offset += 6; /* ��������Դ��ַ6���ֽ� */
    singalnum = buf[offset++];
    if(singalnum>4)
    {
        return;
    }
    IdentifyInformations.Identifynum=singalnum;
    //��������Ϣ����
    //codelen = buf[offset++];
    for(i = 0; i < singalnum; i++)
    {
        memset(tmpBuff,0x00,30);
        //������ΪBIN��ʽ 1d 15 11 1d 0a 14
        memcpy_r(tmpBuff, buf + offset, 6);
        offset = offset + 6;

        IdentifyInformations.IdentifyInformation[i].Suctime.year = 2000 + tmpBuff[0];
        IdentifyInformations.IdentifyInformation[i].Suctime.month = tmpBuff[1];
        IdentifyInformations.IdentifyInformation[i].Suctime.day = tmpBuff[2];
        IdentifyInformations.IdentifyInformation[i].Suctime.hour = tmpBuff[3];
        IdentifyInformations.IdentifyInformation[i].Suctime.minute = tmpBuff[4];
        IdentifyInformations.IdentifyInformation[i].Suctime.second = tmpBuff[5];

#if 0
        //IdentifyInformations.IdentifyInformation[i].signalstatus = buf[offset++];
        IdentifyInformations.IdentifyInformation[i].signalphase  = buf[offset++];

        memcpy(&IdentifyInformations.IdentifyInformation[i].Signalstrength1,buf+offset,4);
        offset = offset+4;

        memcpy(&IdentifyInformations.IdentifyInformation[i].Signalstrength2,buf+offset,4);
        offset = offset+4;

        //memcpy((uint8 *)&IdentifyInformations.IdentifyInformation[i].BaseStrength,buf+offset,4);
        //offset = offset+4;

        memcpy((uint8 *)&IdentifyInformations.IdentifyInformation[i].BackgroundNoise,buf+offset,4);
        offset = offset+4;
        //Ĭ��ΪAAE9 aa e9 00 00 puAmr�鱨��ʱ�ᷴ���� ������ߴ���

        memcpy_r(IdentifyInformations.IdentifyInformation[i].Signatureinformation.nValue,buf+offset,codelen);
        offset = offset+codelen;

        IdentifyInformations.IdentifyInformation[i].Extendedinformation.nNum = 12;
        memcpy(IdentifyInformations.IdentifyInformation[i].Extendedinformation.value,buf+offset,12);
        offset+=12;
#endif
    }
    OAD.value = 0x4E070200;
    taskmng_normal_data_write_pipe(pipe, (uint8 *)&IdentifyInformations, sizeof(IdentifyInformations), OAD, Pn, 0);
    TASK_FMT_DEBUG( pipe, RELAY_LOG,"Change_circuit_Breaker_Data_to_Struct WriteNormalData \n");
}
#endif
#endif

/*******************************************************************************
* ��������: reset_state_recv_check
* ��������://�յ���λ�����ϱ�״̬�ֻ����ж��Ƿ�����
* �������:pipe �˿ں�
        buf �յ��ı���
        len ���ĳ���
* �������:
* �� �� ֵ: 0 ����Ӧ�� -1 �쳣Ӧ��֡
*******************************************************************************/
int reset_state_recv_check(uint8 pipe,uint8* buf,uint16 len)
{
    int ret = -1;
    uint16 reallen=0;
    uint8 *prt =(uint8*)commfun_645_frame_get((char*)buf,0x68,len,&reallen);
    if(prt == NULL)
    {
        TASK_FMT_DEBUG( pipe, RELAY_LOG, "reset_state_recv_check����645����  \n");
        return ret;
    }
    if(prt[8]==0x94)
        ret = 0;
    return ret;
}
/*******************************************************************************
* ��������: get_state_recv_check
* ��������://������״̬�ֺ���д��� ���Ƿ��к����¼�
* �������:pipe �˿ں�
        buf �յ��ı���
        len ���ĳ���
* �������:
* �� �� ֵ: 0 ����Ӧ�� -1 �쳣Ӧ��֡
*******************************************************************************/
int get_state_recv_check(uint8 pipe,uint8* buf,uint16 len)
{
    int ret;
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    uint8 MtrIndex = pAmrThreadArg->EventReport.CurIndex;
    uint16 currentlen = pAmrThreadArg->EventReport.MtrEvent[MtrIndex].DataLen;
    uint16 reallen=0;
    uint8 data[300] = {0};  //��״̬�ֿ�ʼ��������
    uint8 datalen;         //�����򳤶�
    uint8  EventStateDI[4] = {0x34, 0x48, 0x33, 0x37}; //�����ϱ�״̬�ֵ�ID  0x04001501
    uint8 *prt =(uint8*)commfun_645_frame_get((char*)buf,0x68,len,&reallen);
    if(prt == NULL)
    {
        TASK_FMT_DEBUG( pipe, RELAY_LOG, "get_state_recv_check����645����  \n");
        return -1;
    }
    if(memcmp(&prt[10],EventStateDI,4)!=0)
    {
        TASK_FMT_DEBUG( pipe, RELAY_LOG, "����04001501  \n");
        return -1;
    }
    if(prt[9]<4+12) //di4 ״̬��12 ����AAH�ָ��� ״̬��ȫ0��ʱ�����û��AA
    {
        TASK_FMT_DEBUG( pipe, RELAY_LOG, "�����򳤶� %d  \n",prt[9]);
        return -1;
    }
    datalen = prt[9] -4; //��ȥ4���ֽ�DI
    memcpy(data,prt+14,datalen);
    Pro_ReduceMeterGap(data,datalen,0x33);
    ret = ReportEventStateFun(pipe,data,datalen,MtrIndex);
    if(ret == 0)
    {
        //��λ��Ϻ�û�������¼�
        pAmrThreadArg->EventReport.MtrEvent[MtrIndex].state = EVENT_ONEMTR_END;
        return 0;
    }
    if(ret ==1)
    {
        //��λ���������¼� ���ǲ���Ҫ�� ֱ�Ӵ���������

        pAmrThreadArg->EventReport.MtrEvent[MtrIndex].state = EVENT_RESET_STATE;
    }
    if(ret ==2)
    {
        //��λ���������¼� ������
        pAmrThreadArg->EventReport.MtrEvent[MtrIndex].state = EVENT_GET_EVENT;
        pAmrThreadArg->EventReport.MtrEvent[MtrIndex].CurIndex=0;
    }
    memcpy(pAmrThreadArg->EventReport.MtrEvent[MtrIndex].ReportState,data,12);
    //���µ�״̬��д���ϱ�����
    pAmrThreadArg->EventReport.MtrEvent[MtrIndex].DataBuf[currentlen] = reallen;
    pAmrThreadArg->EventReport.MtrEvent[MtrIndex].DataLen++;
    memcpy(&pAmrThreadArg->EventReport.MtrEvent[MtrIndex].DataBuf[currentlen+1],prt,reallen);
    pAmrThreadArg->EventReport.MtrEvent[MtrIndex].DataLen+=reallen;
    pAmrThreadArg->EventReport.MtrEvent[MtrIndex].DataBuf[4]++;
    return 0;
}

/*******************************************************************************
* ��������: EventReportRecvFun
* ��������://�¼��ϱ������յ��ı���
* �������:pipe �˿ں�
        buf �յ��ı���
        len ���ĳ���
* �������:
* �� �� ֵ: 1 ������� -1 δ���
*******************************************************************************/
int EventReportRecvFun(uint8 pipe,uint8* buf,uint16 len)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    uint8 MtrIndex = pAmrThreadArg->EventReport.CurIndex;
    uint16 Pn = pAmrThreadArg->EventReport.MtrEvent[MtrIndex].Pn;
    uint8 prtl = pAmrThreadArg->EventReport.MtrEvent[MtrIndex].prtl;
    int ret;
    AppLayerList_t* pAppLayer = NULL;
    uint16 newpn=0;

    uint8 RecvBuf[MAX_MSG_LEN]={0};
    uint16 RecvLen;

    uint16 index = 0;
    uint8 lenoffset;
    uint16 framelen = XADR_GetEleNum(buf, &lenoffset);

    switch(pAmrThreadArg->EventReport.MtrEvent[MtrIndex].state)
    {
    case EVENT_GET_STATE:
    {
        //�ֳ�������04001501�ط��ϵ� ������жϻ����һֱ��
        ret = get_state_recv_check( pipe,&buf[lenoffset],framelen);
        if(ret<0)
        {
            pAmrThreadArg->EventReport.MtrEvent[MtrIndex].FailCnt++;
        }
    }
        break;
    case EVENT_GET_EVENTLIST:
    {

        OOP_OAD_U RecvOad;
        uint16 SubOadcount;
        OOP_OAD_U SubOad[MAX_OAD_NUM];
        uint8 i,j;

        ret = LinkDataReceive(pipe, &buf[lenoffset], framelen,NULL);
        TASK_FMT_DEBUG(pipe, RELAY_LOG,"PN %d ��ȡ3320 LinkDataReceive 698��֡����ֵ %d \n",Pn,ret);
        if(ret >=eOopComm_Fail)//��֡ʧ��ֱ�ӷ���
            break;

        pAppLayer = FindAppLayer(pipe, NULL);
        if(pAppLayer == NULL)
            break;
        memset(RecvBuf,0x00,sizeof(RecvBuf));

        RecvLen = pAppLayer->recvApduList->apduLen-pAppLayer->recvApduList->offSet;
        memcpy(RecvBuf, pAppLayer->recvApduList->apdu+pAppLayer->recvApduList->offSet, RecvLen);

        TASK_FMT_DEBUG( pipe, RELAY_LOG,"3320�¼��б���Ϣ len %d RecvBuf \n",RecvLen);

        TASK_BUF_DEBUG( pipe, RELAY_LOG,RecvBuf, RecvLen);
        index = 0;

        if(RecvBuf[index++]==0) //1 DAR 1 DATA
        {
            TASK_FMT_DEBUG( pipe, RELAY_LOG,"���֧��3320 ��������\n");
            pAmrThreadArg->EventReport.MtrEvent[MtrIndex].state = EVENT_ONEMTR_END;
            break;
        }
        if(RecvBuf[index++] !=1 )//1����array
            break;
        uint8 OadCount = RecvBuf[index++] ;
        uint8 eventindex=0;
        for(i=0;i<OadCount;i++)
        {
            index++;//oad����81
            memcpy_r((uint8*)&RecvOad.value,&RecvBuf[index],4);
            index+=4;
            memset(SubOad,0x00,sizeof(OOP_OAD_U)*MAX_OAD_NUM);
            SubOadcount = GetEventSubOad(Pn,RecvOad,SubOad,&newpn);
            if(SubOadcount==0)
            {
                TASK_FMT_DEBUG( pipe, RELAY_LOG,"�¼� oad %08x δ�ҵ���Ӧ�ķ���\n",RecvOad.value);
                continue;
            }
            pAmrThreadArg->EventReport.MtrEvent[MtrIndex].EventInfo[eventindex].EventDI = RecvOad;
            pAmrThreadArg->EventReport.MtrEvent[MtrIndex].EventInfo[eventindex].SubOadNum = SubOadcount;
            memcpy(pAmrThreadArg->EventReport.MtrEvent[MtrIndex].EventInfo[eventindex].SubOad,SubOad,sizeof(OOP_OAD_U)*SubOadcount);
            eventindex++;
            TASK_FMT_DEBUG( pipe, RELAY_LOG,"3320�¼��б� count %d i %d mainoad %08x \n",OadCount,i,RecvOad.value);
            for(j=0;j<SubOadcount;j++)
            {
                TASK_FMT_DEBUG( pipe, RELAY_LOG,"suboad index %d  %08x \n",j,SubOad[j].value);
            }
        }
        if(eventindex==0)
        {
            TASK_FMT_DEBUG( pipe, RELAY_LOG,"û����Ҫ�������¼� \n");
            pAmrThreadArg->EventReport.MtrEvent[MtrIndex].state = EVENT_ONEMTR_END;
            break;
        }
        pAmrThreadArg->EventReport.MtrEvent[MtrIndex].EventCount = eventindex;
        pAmrThreadArg->EventReport.MtrEvent[MtrIndex].state = EVENT_GET_EVENT;
        return 1;
    }
        break;
    case EVENT_GET_EVENT:
    {
        if(prtl==PRTL_OOP || prtl == PRTL_20_IOT)
        {
            uint8 Eventindex = pAmrThreadArg->EventReport.MtrEvent[MtrIndex].CurIndex;
            OOP_OAD_U MainOad = pAmrThreadArg->EventReport.MtrEvent[MtrIndex].EventInfo[Eventindex].EventDI;
            time_t SendTime = pAmrThreadArg->EventReport.MtrEvent[MtrIndex].EventInfo[Eventindex].SendTime;
            ret = LinkDataReceive(pipe, &buf[lenoffset], framelen,NULL);
            TASK_FMT_DEBUG(pipe, RELAY_LOG,"LinkDataReceive 698��֡����ֵ %d \n",ret);
            if(ret >=eOopComm_Fail)//��֡ʧ��ֱ�ӷ���
            {
                pAmrThreadArg->EventReport.MtrEvent[MtrIndex].CurIndex++;
                break;
            }

            pAppLayer = FindAppLayer(pipe, NULL);
            if(pAppLayer == NULL)
                break;
            memset(RecvBuf,0x00,sizeof(RecvBuf));
            RecvLen = pAppLayer->recvApduList->apduLen-pAppLayer->recvApduList->offSet;
            memcpy(RecvBuf, pAppLayer->recvApduList->apdu+pAppLayer->recvApduList->offSet, RecvLen);

            TASK_FMT_DEBUG( pipe, RELAY_LOG,"�����¼��ظ� len %d RecvBuf \n",RecvLen);

            TASK_BUF_DEBUG( pipe, RELAY_LOG,RecvBuf, RecvLen);
            TASK_FMT_DEBUG(pipe, RELAY_LOG,"MainOad.value %x \n",MainOad.value);
#if (defined AREA_JIANGXI) || (defined AREA_ANHUI)
            if(MainOad.value == 0x31300200) //��֪�ն��¹ҵ��ܱ�仯�¼�
            {
                Save698TerminalEventData(pipe,RecvBuf,RecvLen,Pn,MainOad,SendTime);
            }
            else if((MainOad.value == 0x30600200)||(MainOad.value == 0x30610200))
            {
                MainOad.value = 0x60120300;
                Save698SepicialEventData(pipe,RecvBuf,RecvLen,Pn,MainOad,SendTime);
            }
            else
#endif
            {
                Save698EventData(pipe,RecvBuf,RecvLen,Pn,MainOad,SendTime);
            }
            pAmrThreadArg->EventReport.MtrEvent[MtrIndex].CurIndex++;
        }
        else
        {
#ifdef AREA_ZHEJIANG
            EVENT_INFO_T *eventinfo =NULL;
            uint8 eventindex = pAmrThreadArg->EventReport.MtrEvent[MtrIndex].CurIndex;
            eventinfo = &(pAmrThreadArg->EventReport.MtrEvent[MtrIndex].EventInfo[eventindex]);
            uint32 SendDI = eventinfo->AllDI[eventinfo->CurDIIndex] ;
            TASK_FMT_DEBUG( pipe, RELAY_LOG,"�����¼��ظ� SendDI is %d\n",SendDI);
            TASK_BUF_DEBUG( pipe, RELAY_LOG,buf, len);
            uint8 lenoff;
            XADR_GetEleNum(buf,&lenoff);
            if(SendDI==0x09080501)
            {
                Change_circuit_Breaker_Data_to_Struct(buf+lenoff+14,len-(lenoff+14+2),pipe);
            }
#endif
#ifdef AREA_JIANGXI
#if 0
            EVENT_INFO_T *eventinfo =NULL;
            uint8 eventindex = pAmrThreadArg->EventReport.MtrEvent[MtrIndex].CurIndex;
            eventinfo = &(pAmrThreadArg->EventReport.MtrEvent[MtrIndex].EventInfo[eventindex]);
            uint32 SendDI = eventinfo->AllDI[eventinfo->CurDIIndex] ;
            TASK_FMT_DEBUG( pipe, RELAY_LOG,"�����¼��ظ� SendDI is %d\n",SendDI);
            TASK_BUF_DEBUG( pipe, RELAY_LOG,buf, len);
            uint8 lenoff;
            XADR_GetEleNum(buf,&lenoff);
            TASK_FMT_DEBUG( pipe, RELAY_LOG,"Change_circuit_Breaker_Data_to_Struct SendDI=%u,di=%u,len=%u,lenoff=%u \n", SendDI, 0x09080601, len, lenoff);

            if (SendDI == 0x09080601)
            {
                if (len > (lenoff + 14 + 2) && len - (lenoff + 14 + 2) >= 13) /* ����Դ��ַ6���ֽ�+ʶ���ź�����1���ֽ�+ʶ���ź�ʱ��6���ֽ� */
                {
                    TASK_FMT_DEBUG( pipe, RELAY_LOG,"_Change_circuit_Breaker_Data_to_Struct_ in \n");
                    Change_circuit_Breaker_Data_to_Struct(buf + lenoff + 14, len - (lenoff + 14 + 2), pipe);
                }
            }
#endif
#endif
#ifdef AREA_HUNAN
            //������Ҫ����·���ϱ����¶ȸ澯�¼� ���������¼� բλ�仯�¼���֯��698�¼���Ԫ�ϱ�
            MeterPrtlFmt_T MeterPrtlData;
            memset(&MeterPrtlData,0x00,sizeof(MeterPrtlFmt_T));
            uint32 DI;
            ret = taskmng_645_meter_data_unpack(len,buf,MeterPrtlData.MtrAddr,&MeterPrtlData.CtrlCode,&MeterPrtlData.DataLen,MeterPrtlData.DataBuff);
            if(CHECK_RESULT_SUCCEED == ret)
            {
                memcpy(&DI,MeterPrtlData.DataBuff,4);
                if(0x03810207==DI)
                {
                    //�¶ȸ澯�¼�
                    event_temperature_seq(pipe,MeterPrtlData.DataBuff+4,MeterPrtlData.DataLen-4);
                }
                if(0x03950001==DI)
                {
                    //�¶ȸ澯�¼�
                    temperature_event_save(pipe,MeterPrtlData.DataBuff+4,MeterPrtlData.DataLen-4,Pn);
                }
                if(0x03810206==DI)
                {
                    //���������������� ��Ϊ�¼����
                    event_protect_seq(pipe,MeterPrtlData.DataBuff+4,MeterPrtlData.DataLen-4);
                }
                if(0x038E0001 == DI)
                {
                    event_protect_save(pipe,MeterPrtlData.DataBuff+4,MeterPrtlData.DataLen-4,Pn);
                }
                if(0x03810204==DI)
                {
                    event_switchchange_seq(pipe,MeterPrtlData.DataBuff+4,MeterPrtlData.DataLen-4);
                }
                if(0x03910001 == DI)
                {
                    event_switchchange_save(pipe,MeterPrtlData.DataBuff+4,MeterPrtlData.DataLen-4,Pn);
                }
            }

#endif
#ifdef AREA_SHANDONG
            GetRunStateEvent(pipe, &buf[lenoffset], framelen);
#endif
            uint16 currentlen = pAmrThreadArg->EventReport.MtrEvent[MtrIndex].DataLen;
            //OAD�����������
            if(currentlen == 4)
            {
                pAmrThreadArg->EventReport.MtrEvent[MtrIndex].DataLen++;
                pAmrThreadArg->EventReport.MtrEvent[MtrIndex].DataBuf[4] = 0;
                currentlen++;
            }
            memcpy(&(pAmrThreadArg->EventReport.MtrEvent[MtrIndex].DataBuf[currentlen]),buf,len);
            pAmrThreadArg->EventReport.MtrEvent[MtrIndex].DataLen+=len;
            pAmrThreadArg->EventReport.MtrEvent[MtrIndex].DataBuf[4]++;

            uint8 Eventindex = pAmrThreadArg->EventReport.MtrEvent[MtrIndex].CurIndex;
            pAmrThreadArg->EventReport.MtrEvent[MtrIndex].EventInfo[Eventindex].CurDIIndex++;

            TASK_FMT_DEBUG( pipe, RELAY_LOG,"645������ len %d Buf \n",pAmrThreadArg->EventReport.MtrEvent[MtrIndex].DataLen);

            TASK_BUF_DEBUG( pipe, RELAY_LOG,pAmrThreadArg->EventReport.MtrEvent[MtrIndex].DataBuf, pAmrThreadArg->EventReport.MtrEvent[MtrIndex].DataLen);

        }
    }
        break;
    case EVENT_RESET_STATE:
    {
        if(prtl == PRTL_20_IOT)
        {
            TASK_FMT_DEBUG( pipe, RELAY_LOG,"ȷ�ϵ��ܱ�����ϱ�״̬�ֻظ� \n");
            TASK_BUF_DEBUG( pipe, RELAY_LOG,&buf[lenoffset], framelen);
            ret = LinkDataReceive(pipe, &buf[lenoffset], framelen,NULL);
            TASK_FMT_DEBUG(pipe, RELAY_LOG,"LinkDataReceive 698��֡����ֵ %d \n",ret);
            if(ret >=eOopComm_Fail)//��֡ʧ��ֱ�ӷ���
            {
                break;
            }

            pAppLayer = FindAppLayer(pipe, NULL);
            if(pAppLayer == NULL)
                break;
            memset(RecvBuf,0x00,sizeof(RecvBuf));
            RecvLen = pAppLayer->recvApduList->apduLen-pAppLayer->recvApduList->offSet;
            memcpy(RecvBuf, pAppLayer->recvApduList->apdu+pAppLayer->recvApduList->offSet, RecvLen);
            if(RecvBuf[4] == 0)
            {
                TASK_FMT_DEBUG( pipe, RELAY_LOG,"ȷ�ϵ��ܱ�����ϱ�״̬�ֳɹ� \n");
            }
            pAmrThreadArg->EventReport.MtrEvent[MtrIndex].state = EVENT_ONEMTR_END;

            break;
        }
        //�յ������ж��Ƿ�ȷ��֡ ����ظ��쳣 ���ٸ�λһ��
        if(reset_state_recv_check(pipe,&buf[lenoffset],framelen)==0)
        {
            TASK_FMT_DEBUG( pipe, RELAY_LOG,"EVENT_RESET_STATE ok \n");
            pAmrThreadArg->EventReport.MtrEvent[MtrIndex].state=EVENT_GET_STATE;
        }

    }
        break;
    default:
        break;
    }
    return -1;
}

/*******************************************************************************
* ��������: EventReportRecvFun
* ��������://�¼��ϱ������յ��ı���
* �������:pipe �˿ں�
        buf �յ��ı���
        len ���ĳ���
* �������:
* �� �� ֵ: 1 ������� -1 δ���
*******************************************************************************/
int Rs485EventReportRecvFun(uint8 pipe,uint8* buf,uint16 len)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    uint8 MtrIndex = pAmrThreadArg->EventReport.CurIndex;
    uint16 Pn = pAmrThreadArg->EventReport.MtrEvent[MtrIndex].Pn;
    uint8 prtl = pAmrThreadArg->EventReport.MtrEvent[MtrIndex].prtl;
    int ret;
    AppLayerList_t* pAppLayer = NULL;
    uint16 newpn=0;

    uint8 RecvBuf[MAX_MSG_LEN]={0};
    uint16 RecvLen;

    uint16 index = 0;

    TASK_FMT_DEBUG(pipe, RELAY_LOG,"Rs485EventReportRecvFun \n");
    TASK_BUF_DEBUG(pipe, RELAY_LOG,buf,len);

    switch(pAmrThreadArg->EventReport.MtrEvent[MtrIndex].state)
    {
    case EVENT_GET_STATE:
    {
        //taskmng_event_report_amr_fun( pipe,buf,len);
        ret = get_state_recv_check( pipe,buf,len);
        if(ret<0)
        {
            pAmrThreadArg->EventReport.MtrEvent[MtrIndex].FailCnt++;
        }
    }
        break;
    case EVENT_GET_EVENTLIST:
    {

        OOP_OAD_U RecvOad;
        uint16 SubOadcount;
        OOP_OAD_U SubOad[MAX_OAD_NUM];
        uint8 i,j;

        ret = LinkDataReceive(pipe, buf, len,NULL);
        TASK_FMT_DEBUG(pipe, RELAY_LOG,"PN %d ��ȡ3320 LinkDataReceive 698��֡����ֵ %d \n",Pn,ret);
        if(ret >=eOopComm_Fail)//��֡ʧ��ֱ�ӷ���
            break;

        pAppLayer = FindAppLayer(pipe, NULL);
        if(pAppLayer == NULL)
            break;
        memset(RecvBuf,0x00,sizeof(RecvBuf));

        RecvLen = pAppLayer->recvApduList->apduLen-pAppLayer->recvApduList->offSet;
        memcpy(RecvBuf, pAppLayer->recvApduList->apdu+pAppLayer->recvApduList->offSet, RecvLen);

        TASK_FMT_DEBUG( pipe, RELAY_LOG,"3320�¼��б���Ϣ len %d RecvBuf \n",RecvLen);

        TASK_BUF_DEBUG( pipe, RELAY_LOG,RecvBuf, RecvLen);
        index = 0;

        if(RecvBuf[index++]==0) //1 DAR 1 DATA
        {
            TASK_FMT_DEBUG( pipe, RELAY_LOG,"���֧��3320 ��������\n");
            pAmrThreadArg->EventReport.MtrEvent[MtrIndex].state = EVENT_ONEMTR_END;
            break;
        }
        if(RecvBuf[index++] !=1 )//1����array
            break;
        uint8 OadCount = RecvBuf[index++] ;
        uint8 eventindex=0;
        for(i=0;i<OadCount;i++)
        {
            index++;//oad����81
            memcpy_r((uint8*)&RecvOad.value,&RecvBuf[index],4);
            index+=4;
            memset(SubOad,0x00,sizeof(OOP_OAD_U)*MAX_OAD_NUM);
            SubOadcount = GetEventSubOad(Pn,RecvOad,SubOad,&newpn);
            if(SubOadcount==0)
            {
                TASK_FMT_DEBUG( pipe, RELAY_LOG,"�¼� oad %08x δ�ҵ���Ӧ�ķ���\n",RecvOad.value);
                continue;
            }
            pAmrThreadArg->EventReport.MtrEvent[MtrIndex].EventInfo[eventindex].EventDI = RecvOad;
            pAmrThreadArg->EventReport.MtrEvent[MtrIndex].EventInfo[eventindex].SubOadNum = SubOadcount;
            memcpy(pAmrThreadArg->EventReport.MtrEvent[MtrIndex].EventInfo[eventindex].SubOad,SubOad,sizeof(OOP_OAD_U)*SubOadcount);
            eventindex++;
            TASK_FMT_DEBUG( pipe, RELAY_LOG,"3320�¼��б� count %d i %d mainoad %08x \n",OadCount,i,RecvOad.value);
            for(j=0;j<SubOadcount;j++)
            {
                TASK_FMT_DEBUG( pipe, RELAY_LOG,"suboad index %d  %08x \n",j,SubOad[j].value);
            }
        }
        if(eventindex==0)
        {
            TASK_FMT_DEBUG( pipe, RELAY_LOG,"û����Ҫ�������¼� \n");
            pAmrThreadArg->EventReport.MtrEvent[MtrIndex].state = EVENT_ONEMTR_END;
            break;
        }
        pAmrThreadArg->EventReport.MtrEvent[MtrIndex].EventCount = eventindex;
        pAmrThreadArg->EventReport.MtrEvent[MtrIndex].state = EVENT_GET_EVENT;
        return 1;
    }
        break;
    case EVENT_GET_EVENT:
    {
        if(prtl==PRTL_OOP)
        {
            uint8 Eventindex = pAmrThreadArg->EventReport.MtrEvent[MtrIndex].CurIndex;
            OOP_OAD_U MainOad = pAmrThreadArg->EventReport.MtrEvent[MtrIndex].EventInfo[Eventindex].EventDI;
            time_t SendTime = pAmrThreadArg->EventReport.MtrEvent[MtrIndex].EventInfo[Eventindex].SendTime;
            ret = LinkDataReceive(pipe, buf, len,NULL);
            TASK_FMT_DEBUG(pipe, RELAY_LOG,"LinkDataReceive 698��֡����ֵ %d \n",ret);
            if(ret >=eOopComm_Fail)//��֡ʧ��ֱ�ӷ���
                break;

            pAppLayer = FindAppLayer(pipe, NULL);
            if(pAppLayer == NULL)
                break;
            memset(RecvBuf,0x00,sizeof(RecvBuf));
            RecvLen = pAppLayer->recvApduList->apduLen-pAppLayer->recvApduList->offSet;
            memcpy(RecvBuf, pAppLayer->recvApduList->apdu+pAppLayer->recvApduList->offSet, RecvLen);

            TASK_FMT_DEBUG( pipe, RELAY_LOG,"�����¼��ظ� len %d RecvBuf \n",RecvLen);

            TASK_BUF_DEBUG( pipe, RELAY_LOG,RecvBuf, RecvLen);

            Save698EventData(pipe,RecvBuf,RecvLen,Pn,MainOad,SendTime);
            pAmrThreadArg->EventReport.MtrEvent[MtrIndex].CurIndex++;
        }
        else
        {
            uint16 currentlen = pAmrThreadArg->EventReport.MtrEvent[MtrIndex].DataLen;
            memcpy(&(pAmrThreadArg->EventReport.MtrEvent[MtrIndex].DataBuf[currentlen]),buf,len);
            pAmrThreadArg->EventReport.MtrEvent[MtrIndex].DataLen+=len;
            pAmrThreadArg->EventReport.MtrEvent[MtrIndex].DataBuf[4]++;
            uint8 Eventindex = pAmrThreadArg->EventReport.MtrEvent[MtrIndex].CurIndex;
            pAmrThreadArg->EventReport.MtrEvent[MtrIndex].EventInfo[Eventindex].CurDIIndex++;

            TASK_FMT_DEBUG( pipe, RELAY_LOG,"645������ len %d Buf \n",pAmrThreadArg->EventReport.MtrEvent[MtrIndex].DataLen);

            TASK_BUF_DEBUG( pipe, RELAY_LOG,pAmrThreadArg->EventReport.MtrEvent[MtrIndex].DataBuf, pAmrThreadArg->EventReport.MtrEvent[MtrIndex].DataLen);

        }
    }
        break;
    case EVENT_RESET_STATE:
    {
        pAmrThreadArg->EventReport.MtrEvent[MtrIndex].state=EVENT_GET_STATE;
    }
        break;
    default:
        break;
    }
    return -1;
}

/*******************************************************************************
* ��������: Save698EventData
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
void Save698EventData(uint8 pipe,uint8* DataBuf,uint16 DataLen,uint16 Pn,OOP_OAD_U MainOad,time_t StartTime)
{

    uint8 i,j;
    uint16 index=0;
    uint8 CsdNum;
    uint8 RecordNum;
    time_t        RealTime_t = time(NULL);
    //uint8 Addr[6]={0};
    OOP_TSA_T tsa;
    memset(&tsa,0x00,sizeof(OOP_TSA_T));
    uint16 rptLen = 25;
    //�¼��ϱ�ʱ�ѹ̶���CSD�����ȥ
    uint8 rptBuff[2000] = {0x60, 0x12, 0x03, 0x00, 0x05, 0x00, 0x20, 0x2A, 0x02, 0x00, 0x00,
        0x60, 0x40, 0x02, 0x00, 0x00, 0x60, 0x41, 0x02, 0x00, 0x00, 0x60, 0x42, 0x02, 0x00};
    CsdNum = DataBuf[index++];
    rptBuff[rptLen++] = 1;  //CSD����1 ��¼�Ͷ���������
    memcpy_r(&rptBuff[rptLen],(uint8*)&MainOad.value,4);
    rptLen+=4;
    rptBuff[rptLen++] = CsdNum; //

    for(i=0;i<CsdNum;i++)
    {
        if(DataBuf[index++]!=0)
            return;
        memcpy(&rptBuff[rptLen],&DataBuf[index],4);
        index+=4;
        rptLen+=4;
    }
    if(DataBuf[index++]==0)   //0Ϊ����DAR 1 ΪSEQUENCE OF A-RecordRow
        return;
    RecordNum = DataBuf[index++];
    if(RecordNum==0)
        return;

    rptBuff[rptLen++] = 1;
    rptBuff[rptLen++] = RecordNum;
    //��̶�������
    //��������ַ
    rptBuff[rptLen++] = DT_TSA;
    tsa = taskmng_tsa_get(Pn,LOGIC_ID_YC);
    rptBuff[rptLen++] = tsa.len+2;
    rptBuff[rptLen++] = tsa.af;
    if(tsa.flag == 1 )
        rptBuff[rptLen++] = tsa.vxd;
    //taskmng_mtr_addr_get( Pn,LOGIC_ID_YC,Addr);
    memcpy(&rptBuff[rptLen],tsa.add, 6);
    rptLen+=6;
    //����ʱ��
    rptBuff[rptLen++] = DT_DATETIME_S;
    DT_Time2DateTimeHex(StartTime,(DateTimeHex_t *)&rptBuff[rptLen]);
    rptLen+=7;
    //�ɹ�ʱ��
    rptBuff[rptLen++] = DT_DATETIME_S;
    DT_Time2DateTimeHex(RealTime_t,(DateTimeHex_t *)&rptBuff[rptLen]);
    rptLen+=7;
    //�洢ʱ��
    rptBuff[rptLen++] = DT_DATETIME_S;
    DT_Time2DateTimeHex(RealTime_t,(DateTimeHex_t *)&rptBuff[rptLen]);
    rptLen+=7;

    rptBuff[rptLen++] = 1;
    rptBuff[rptLen++] = CsdNum;
    //��Ҫ��������ÿ����¼ ����Ҫ���ĸ����ϱ����Լ�ʱ���ǩ
    //uint8 tmpBuff[30]={0};
    uint16 indexbak = index;
    for(i=0;i<RecordNum;i++)
    {
        for(j=0;j<CsdNum;j++)
        {
            taskmng_oop_next_read_data_analysis(DataBuf,&index,DataLen,DT_NULL,FALSE);
        }
    }
    //TASK_FMT_DEBUG( pipe, RELAY_LOG,"indexbak  %d index %d  DataLen %d \n",indexbak,index,DataLen);
    memcpy(&rptBuff[rptLen],&DataBuf[indexbak],index-indexbak);
    rptLen+=(index-indexbak);

    //����õ��¼�����д����������
    taskmng_event_report_data_write(pipe,rptBuff,rptLen,EventReportOad);
}

/*******************************************************************************
* ��������: event_report_data_save
* ��������://��ͨ��8802�����ϱ����¼�д����������
* �������:pipe �˿ں�
        DataBuf �¼����� 30 11 02 01 07 00 20 22 02 00 00 20 1e 02 00 00 20 20 02 00 00 20 24 02 00 00 33 00 02 00 00 00 10 22 01 00 00 20 22 01 01 01 06 00 00 00 02 1c 07 e5 03 1f 13 38 0a 1c 07 e5 04 01 09 1b 36 00 00 06 00 00 00 00 06 00 00 00 00
        buflen ����
        pn ���

* �������:
* �� �� ֵ: �����ı��ĳ���
*******************************************************************************/

uint16 event_report_data_save(uint8 pipe,uint8 *DataBuf,uint16 DataLen,uint16 Pn)
{
    uint8 i,j,k;
    uint16 index=0;
    uint8 CsdNum;
    uint8 RecordNum;
    time_t        RealTime_t = time(NULL);
    uint8 Addr[6]={0};
    uint16 rptLen = 25;
    //�¼��ϱ�ʱ�ѹ̶���CSD�����ȥ
    uint8 rptBuff[2000] = {0x60, 0x12, 0x03, 0x00, 0x05, 0x00, 0x20, 0x2A, 0x02, 0x00, 0x00,
        0x60, 0x40, 0x02, 0x00, 0x00, 0x60, 0x41, 0x02, 0x00, 0x00, 0x60, 0x42, 0x02, 0x00};

    rptBuff[rptLen++] = 1;  //CSD����1 ��¼�Ͷ���������
    TASK_FMT_DEBUG( pipe, RELAY_LOG,"event_report_data_save \n");
    TASK_BUF_DEBUG( pipe, RELAY_LOG,DataBuf, DataLen);

    memcpy(&rptBuff[rptLen],DataBuf,4);
    rptLen+=4;
    index+=4;
    CsdNum = DataBuf[index++];
    rptBuff[rptLen++] = CsdNum;
    for(i=0;i<CsdNum;i++)
    {
        if(DataBuf[index++]!=0)
            return index;
        memcpy(&rptBuff[rptLen],&DataBuf[index],4);
        index+=4;
        rptLen+=4;
    }
    if(DataBuf[index++]==0)   //0Ϊ����DAR 1 ΪSEQUENCE OF A-RecordRow
        return index;
    RecordNum = DataBuf[index++];
    if(RecordNum==0)
        return index;

    rptBuff[rptLen++] = 1;
    rptBuff[rptLen++] = RecordNum;
    for(j=0;j<RecordNum;j++)
    {
        //��̶�������
        //��������ַ
        rptBuff[rptLen++] = DT_TSA;
        rptBuff[rptLen++] = 7;
        rptBuff[rptLen++] = 5;
        taskmng_mtr_addr_get( Pn,LOGIC_ID_YC,Addr);
        memcpy_r(&rptBuff[rptLen],Addr, 6);
        rptLen+=6;
        //����ʱ��
        rptBuff[rptLen++] = DT_DATETIME_S;
        DT_Time2DateTimeHex(RealTime_t,(DateTimeHex_t *)&rptBuff[rptLen]);
        rptLen+=7;
        //�ɹ�ʱ��
        rptBuff[rptLen++] = DT_DATETIME_S;
        DT_Time2DateTimeHex(RealTime_t,(DateTimeHex_t *)&rptBuff[rptLen]);
        rptLen+=7;
        //�洢ʱ��
        rptBuff[rptLen++] = DT_DATETIME_S;
        DT_Time2DateTimeHex(RealTime_t,(DateTimeHex_t *)&rptBuff[rptLen]);
        rptLen+=7;

        rptBuff[rptLen++] = 1;
        rptBuff[rptLen++] = CsdNum;

        uint16 indexbak = index;
        for(k=0;k<CsdNum;k++)
        {
            taskmng_oop_next_read_data_analysis(DataBuf,&index,DataLen,DT_NULL,FALSE);
        }
        memcpy(&rptBuff[rptLen],&DataBuf[indexbak],index-indexbak);
        rptLen+=(index-indexbak);

    }
    OOP_OAD_U oad;
    memset(&oad,0,sizeof(oad));
    memcpy_r(&oad,DataBuf,4);
    if(oad.nObjID == 0x3011)
    {
        taskmng_meter_event_report_power_check(pipe,Pn,DataBuf+4,DataLen-4);
    }
    taskmng_event_report_data_write(pipe,rptBuff,rptLen,EventReportOad);
    return index;
}


/*******************************************************************************
* ��������: event_report_data_save
* ��������://��ͨ��8802�����ϱ����¼�д����������
* �������:pipe �˿ں�
        DataBuf �¼����� 30 11 02 01 07 00 20 22 02 00 00 20 1e 02 00 00 20 20 02 00 00 20 24 02 00 00 33 00 02 00 00 00 10 22 01 00 00 20 22 01 01 01 06 00 00 00 02 1c 07 e5 03 1f 13 38 0a 1c 07 e5 04 01 09 1b 36 00 00 06 00 00 00 00 06 00 00 00 00
        buflen ����
        pn ���

* �������:
* �� �� ֵ: �����ı��ĳ���
*******************************************************************************/

uint16 deal_reportNotificationlist_data_save(uint8 pipe,uint8 *DataBuf,uint16 DataLen,uint16 Pn,OOP_OAD_U RecvOad)
{
    uint8 j,k;
    uint16 index=0;
    time_t        RealTime_t = time(NULL);
    uint8 Addr[6]={0};
    uint16 rptLen = 25;
    //�¼��ϱ�ʱ�ѹ̶���CSD�����ȥ
    uint8 rptBuff[2000] = {0x60, 0x12, 0x03, 0x00, 0x05, 0x00, 0x20, 0x2A, 0x02, 0x00, 0x00,
        0x60, 0x40, 0x02, 0x00, 0x00, 0x60, 0x41, 0x02, 0x00, 0x00, 0x60, 0x42, 0x02, 0x00};

    rptBuff[rptLen++] = 0;  //CSD����1 ��¼�Ͷ���������
    TASK_FMT_DEBUG( pipe, RELAY_LOG,"deal_reportNotificationlist_data_save \n");
    TASK_BUF_DEBUG( pipe, RELAY_LOG,DataBuf, DataLen);

    memcpy_r(&rptBuff[rptLen],(uint8*)&RecvOad.value,4);
    rptLen+=4;

    if(DataBuf[index++]==0)   //0Ϊ����DAR 1 ΪSEQUENCE OF A-RecordRow
        return index;


    rptBuff[rptLen++] = 1;
    rptBuff[rptLen++] = 1;
    for(j=0;j<1;j++)
    {
        //��̶�������
        //��������ַ
        rptBuff[rptLen++] = DT_TSA;
        rptBuff[rptLen++] = 7;
        rptBuff[rptLen++] = 5;
        taskmng_mtr_addr_get( Pn,LOGIC_ID_YC,Addr);
        memcpy_r(&rptBuff[rptLen],Addr, 6);
        rptLen+=6;
        //����ʱ��
        rptBuff[rptLen++] = DT_DATETIME_S;
        DT_Time2DateTimeHex(RealTime_t,(DateTimeHex_t *)&rptBuff[rptLen]);
        rptLen+=7;
        //�ɹ�ʱ��
        rptBuff[rptLen++] = DT_DATETIME_S;
        DT_Time2DateTimeHex(RealTime_t,(DateTimeHex_t *)&rptBuff[rptLen]);
        rptLen+=7;
        //�洢ʱ��
        rptBuff[rptLen++] = DT_DATETIME_S;
        DT_Time2DateTimeHex(RealTime_t,(DateTimeHex_t *)&rptBuff[rptLen]);
        rptLen+=7;
        uint16 indexbak = index;
        for(k=0;k<1;k++)
        {
            taskmng_oop_next_read_data_analysis(DataBuf,&index,DataLen,DT_NULL,FALSE);
        }
        memcpy(&rptBuff[rptLen],&DataBuf[indexbak],index-indexbak);
        rptLen+=(index-indexbak);

    }
    taskmng_event_report_data_write(pipe,rptBuff,rptLen,EventReportOad);
    return index;
}

/*******************************************************************************
* ��������: taskmng_event_report_data_write
* ��������://����õĵ���ϱ��¼�����д����������
* �������:pipe �˿ں�
        DataBuf �¼�����
        DataLen ����
        Oad д�¼���oad

* �������:
* �� �� ֵ:
*******************************************************************************/
void taskmng_event_report_data_write(uint8 pipe,uint8* DataBuf,uint16 DataLen,OOP_OAD_U Oad)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];

    int ret;
    NOMAL_OAD_T NomalOad;
    NomalOad.logicId = 0;
    NomalOad.oad =Oad;
    NomalOad.classtag = 5;
    NomalOad.infoNum = taskmng_event_report_num_get(Oad);
    TASK_FMT_DEBUG( pipe, RELAY_LOG,"д�¼����ݵ��������� infonum %d len%d \n",NomalOad.infoNum,DataLen);

    TASK_BUF_DEBUG( pipe, RELAY_LOG,DataBuf, DataLen);
    ret = db_write_nomal(pAmrThreadArg->DBClientHandle,&NomalOad,DataBuf,DataLen);
    if(ret !=0)
    {
        TASK_FMT_DEBUG( pipe, RELAY_LOG,"�¼��ϱ�����д�����ݿ�ʧ�� \n");
    }
}



/*******************************************************************************
* ��������: ���Ӷ����8802�����ϱ�
* ��������://��ͨ��8802�����ϱ����¼�д����������
* �������:pipe �˿ں�
        DataBuf �¼����� 30 11 02 01 07 00 20 22 02 00 00 20 1e 02 00 00 20 20 02 00 00 20 24 02 00 00 33 00 02 00 00 00 10 22 01 00 00 20 22 01 01 01 06 00 00 00 02 1c 07 e5 03 1f 13 38 0a 1c 07 e5 04 01 09 1b 36 00 00 06 00 00 00 00 06 00 00 00 00
        buflen ����
        pn ���

* �������:
* �� �� ֵ: �����ı��ĳ���
*******************************************************************************/

uint16 data_report_data_save(uint8 pipe,uint8 *DataBuf,uint16 DataLen,uint16 Pn)
{
    uint8 i,j,k;
    uint16 index=6;
    uint8 CsdNum;
    uint8 RecordNum;
    time_t        RealTime_t = time(NULL);
    uint8 Addr[6]={0};
    uint16 rptLen = 25;
    OOP_OAD_U oad;
    uint8 frezeonindex=0;
    uint16 tmprptLen =0;
    //�¼��ϱ�ʱ�ѹ̶���CSD�����ȥ
    uint8 rptBuff[2000] = {0x60, 0x12, 0x03, 0x00, 0x05, 0x00, 0x20, 0x2A, 0x02, 0x00, 0x00,
        0x60, 0x40, 0x02, 0x00, 0x00, 0x60, 0x41, 0x02, 0x00, 0x00, 0x60, 0x42, 0x02, 0x00};

    rptBuff[rptLen++] = 1;  //CSD����1 ��¼�Ͷ���������
    TASK_FMT_DEBUG( pipe, RELAY_LOG,"event_report_data_save \n");
    TASK_BUF_DEBUG( pipe, RELAY_LOG,DataBuf, DataLen);

    memcpy(&rptBuff[rptLen],DataBuf+index,4);
    rptLen+=4;
    index+=4;
    CsdNum = DataBuf[index++];
    rptBuff[rptLen++] = CsdNum;
    for(i=0;i<CsdNum;i++)
    {
        memcpy(&rptBuff[rptLen],&DataBuf[index],4);
        memcpy_r((uint8*)&oad.value,&DataBuf[index],4);
        if(oad.value ==0x20210200)
        {

            frezeonindex = i;
        }
        index+=4;
        rptLen+=4;
    }
    if(DataBuf[index++]==0)   //0Ϊ����DAR 1 ΪSEQUENCE OF A-RecordRow
        return index;
    RecordNum = DataBuf[index++];

    if(RecordNum==0)
        return index;

    rptBuff[rptLen++] = 1;
    rptBuff[rptLen++] = RecordNum;
    for(j=0;j<RecordNum;j++)
    {
        //��̶�������
        //��������ַ
        rptBuff[rptLen++] = DT_TSA;
        rptBuff[rptLen++] = 7;
        rptBuff[rptLen++] = 5;
        taskmng_mtr_addr_get( Pn,LOGIC_ID_YC,Addr);
        memcpy_r(&rptBuff[rptLen],Addr, 6);
        rptLen+=6;
        //����ʱ��
        rptBuff[rptLen++] = DT_DATETIME_S;
        DT_Time2DateTimeHex(RealTime_t,(DateTimeHex_t *)&rptBuff[rptLen]);
        rptLen+=7;
        //�ɹ�ʱ��
        rptBuff[rptLen++] = DT_DATETIME_S;
        DT_Time2DateTimeHex(RealTime_t,(DateTimeHex_t *)&rptBuff[rptLen]);
        rptLen+=7;
        //�洢ʱ��
        tmprptLen = rptLen;
        rptBuff[rptLen++] = DT_DATETIME_S;
        DT_Time2DateTimeHex(RealTime_t,(DateTimeHex_t *)&rptBuff[rptLen]);
        rptLen+=7;

        //rptBuff[rptLen++] = 1;
        //rptBuff[rptLen++] = CsdNum;

        uint16 indexbak = index;
        for(k=0;k<CsdNum;k++)
        {
            if(k==frezeonindex)
            {
                memcpy(&rptBuff[tmprptLen],&DataBuf[index+2],8);
            }
            taskmng_oop_next_read_data_analysis(DataBuf,&index,DataLen,DT_NULL,FALSE);
        }
        memcpy(&rptBuff[rptLen],&DataBuf[indexbak],DataLen-indexbak-2);
        rptLen+=(DataLen-indexbak-2);

    }
    TASK_BUF_DEBUG( pipe, RELAY_LOG,rptBuff, rptLen);
    taskmng_event_report_data_write(pipe,rptBuff,rptLen,EventReportOad);
    return index;
}

/*******************************************************************************
* ��������: WriteEventReportData
* ��������://����õĵ���ϱ��¼�����д����������
* �������:pipe �˿ں�
        DataBuf �¼�����
        DataLen ����
        Oad д�¼���oad

* �������:
* �� �� ֵ:
*******************************************************************************/
void WritePerceptionTerminalData(uint8 pipe,uint8* DataBuf,uint16 DataLen,OOP_OAD_U Oad)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];

    int ret;
    NOMAL_OAD_T NomalOad;
    NomalOad.logicId = 0;
    NomalOad.oad =Oad;
    NomalOad.classtag = 3;
    NomalOad.infoNum = taskmng_event_report_num_get(Oad);
    TASK_FMT_DEBUG( pipe, RELAY_LOG,"д�¼����ݵ��������� infonum %d len%d \n",NomalOad.infoNum,sizeof(OOP_PERCEPTION_TERMINAL_LIST_T));

    //TASK_BUF_DEBUG( pipe, RELAY_LOG,DataBuf, DataLen);
    ret = db_write_nomal(pAmrThreadArg->DBClientHandle,&NomalOad,DataBuf,DataLen);
    if(ret !=0)
    {
        TASK_FMT_DEBUG( pipe, RELAY_LOG,"�¼��ϱ�����д�����ݿ�ʧ�� \n");
    }
}


/*******************************************************************************
* ��������: GetEventSubOad
* ��������://������oad��EventPlanList�в�ѯ�·������е���oad
* �������:MainOad ��oad

* �������: SubOad �����е���oad
* �� �� ֵ: ��oad����
*******************************************************************************/
int GetEventSubOad(uint16 pn,OOP_OAD_U MainOad,OOP_OAD_U* SubOad,uint16 *newpn)
{
    uint16 i,j,k;
    PLAN_EVENT_INTO_T *eventinfo = NULL;
    uint8 flag = 0;
    int suboadcount = 0;
    uint8 addr[6]={0};
    uint8 addrtmp[6];
    taskmng_mtr_addr_get(pn,LOGIC_ID_YC,addr);

    pthread_rwlock_wrlock(&EventPlanList.rwLock);
    for(i=0;i<EventPlanList.plancount;i++)
    {
        flag = 0;
        eventinfo = &EventPlanList.PlanEvent[i];
        //���жϱ��ڲ�����������ı�����
        for(j = 0;j<eventinfo->msNum;j++)
        {
            memset(addrtmp,0x00,6);
            taskmng_mtr_addr_get(eventinfo->msInfo[j].Sn,LOGIC_ID_YC,addrtmp);
            if(memcmp(addr,addrtmp,6)==0)
            //if(pn == eventinfo->msInfo[j].Sn)
            {
                flag = 1;
                *newpn = eventinfo->msInfo[j].Sn;
                break;
            }
        }
        //������ڱ����������������¼�oad
        if(flag == 0)
            continue;

        for(j=0;j<eventinfo->events.nNum ;j++)
        {
            if(MainOad.value == eventinfo->events.cols[j].oadMain.value)
            {
                suboadcount = eventinfo->events.cols[j].oadCols.nNum;
                for(k=0;k<suboadcount;k++)
                {
                    memcpy(&SubOad[k],&eventinfo->events.cols[j].oadCols.oad[k],sizeof(OOP_OAD_U));
                }
                pthread_rwlock_unlock(&EventPlanList.rwLock);
                return suboadcount;
            }

        }
    }
    pthread_rwlock_unlock(&EventPlanList.rwLock);
    //���û�����¼��ɼ����� �͸���Ĭ�ϵ�
    if(suboadcount == 0)
    {
        suboadcount = 4;
        memcpy(SubOad,g_report_301f_obj,sizeof(g_report_301f_obj));
    }
    return suboadcount;
}

/*******************************************************************************
* ��������: EventReportSendFrame
* ��������:��ȡ�¼��ϱ����͵ı���
* �������:pipe �˿ں�

* �������:
* �� �� ֵ: 1��ʾ�б��ķ��� 2��ʾһ������л�״̬ -1��ʾ���б���
*******************************************************************************/
int EventReportSendFrame(uint8 pipe,uint8 monitorParallFlag)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    uint8 MtrIndex = pAmrThreadArg->EventReport.CurIndex;
    uint16 Pn = pAmrThreadArg->EventReport.MtrEvent[MtrIndex].Pn;
    uint8 prtl = pAmrThreadArg->EventReport.MtrEvent[MtrIndex].prtl;
    if(pAmrThreadArg->EventReport.MtrEvent[MtrIndex].FailCnt>=3)
    {
        TASK_FMT_DEBUG( pipe, RELAY_LOG,"Pn %d ʧ�ܴ�������3�� ���ٳ���  \n",Pn);
        pAmrThreadArg->EventReport.MtrEvent[MtrIndex].state = EVENT_ONEMTR_END;
    }
    uint8 payload[200]={0};

    uint16 index=0;
    LinkLayer_t*   pLinkLayer = NULL;
    uint8 Framebuf[2000]={0};
    uint16 Framelen = 0;
    OOP_OAD_U pipeOAD = taskmng_pipe_oad_get(pipe);

    uint16 Overtime = MAX_MSG_DELAY_TIME;

    OOP_TSA_T Tsa = taskmng_tsa_get(Pn,LOGIC_ID_YC);
    uint8 i;
    //uint16 MsgIndex;
    switch(pAmrThreadArg->EventReport.MtrEvent[MtrIndex].state)
    {
    case EVENT_GET_STATE:
    {
        TASK_FMT_DEBUG( pipe, RELAY_LOG,"Pn %d ��ʼ��645״̬��\n",Pn);
//        if(pAmrThreadArg->EventReport.MtrEvent[MtrIndex].GetStateCount>3)
//        {
//            pAmrThreadArg->EventReport.MtrEvent[MtrIndex].state = EVENT_ONEMTR_END;
//            TASK_FMT_DEBUG( pipe, RELAY_LOG,"Pn %d ״̬�ֳ�̫��� ����  \n",Pn);
//            return 2;
//        }
        MeterPrtlFmt_T  MeterPrtlData;
        uint8 SendDi[] = {0x01,0x15,0x00,0x04};
        memset(&MeterPrtlData,0x00,sizeof(MeterPrtlFmt_T));
        MeterPrtlData.PrtlMode =PRTL_07;
        memcpy_r(MeterPrtlData.MtrAddr,Tsa.add,6);
        MeterPrtlData.CtrlCode = 0x11;
        memcpy(MeterPrtlData.DataBuff,SendDi,4);
        MeterPrtlData.DataLen = 4;
        Framelen = taskmng_meter_prtl_645_pack(Framebuf,MeterPrtlData);
        //pAmrThreadArg->EventReport.MtrEvent[MtrIndex].GetStateCount++;
    }
        break;
    case EVENT_GET_EVENTLIST:
    {

        OOP_OAD_U Oad;
        Oad.value = 0x33200200;
        OopGet.requst(pipe,&Tsa,1,(uint8*)&Oad.value,4);
        pLinkLayer = FindLinkLayer(pipe, NULL);
        Framelen = pLinkLayer->sendLen;
        memcpy(Framebuf,pLinkLayer->sendBuff,Framelen);
        TASK_FMT_DEBUG( pipe, RELAY_LOG,"Pn %d ��ʼ����3320  \n",Pn);
    }
        break;
    case EVENT_GET_EVENT:
    {
        uint8 eventindex = pAmrThreadArg->EventReport.MtrEvent[MtrIndex].CurIndex;
        TASK_FMT_DEBUG( pipe, RELAY_LOG,"eventindex %d EventCount %d \n ",eventindex,pAmrThreadArg->EventReport.MtrEvent[MtrIndex].EventCount);
        //���ж� �Է�ʱ�ӹ���֮�಻��Ҫ���������¼�û�и�λ״̬��
        if(eventindex>=pAmrThreadArg->EventReport.MtrEvent[MtrIndex].EventCount)
        {

            if(prtl == PRTL_OOP)
            {
                pAmrThreadArg->EventReport.MtrEvent[MtrIndex].state = EVENT_ONEMTR_END;
                return 2;
            }else
            {
                //if(eventindex == 0 ||pAmrThreadArg->EventReport.MtrEvent[MtrIndex].EventCount!=0)
                {
                    pAmrThreadArg->EventReport.MtrEvent[MtrIndex].state = EVENT_RESET_STATE;
                    return 2;
                }
            }
        }
//        if(pAmrThreadArg->EventReport.MtrEvent[MtrIndex].EventCount ==0)
//        {
//            pAmrThreadArg->EventReport.MtrEvent[MtrIndex].state = EVENT_ONEMTR_END;
//            TASK_FMT_DEBUG( pipe, RELAY_LOG,"Pn %d �����ϱ�״̬������  \n",Pn);
//            return 2;
//        }
        EVENT_INFO_T *eventinfo =NULL;
        eventinfo = &(pAmrThreadArg->EventReport.MtrEvent[MtrIndex].EventInfo[eventindex]);
        TASK_FMT_DEBUG( pipe, RELAY_LOG,"PN %d prtl %d �¼���ǰ����mainoad %08x SubOadNum %d  \n",Pn,prtl,eventinfo->EventDI.value,eventinfo->SubOadNum);
        if(prtl == PRTL_OOP || prtl == PRTL_20_IOT)
        {
            #if (defined AREA_JIANGXI) || (defined AREA_ANHUI)||(defined AREA_JIANGSU)
            if((eventinfo->EventDI.nObjID ==0x3060)||(eventinfo->EventDI.nObjID ==0x3061))
            {
                //��������ֱ��һ֡��һ���¼�
                uint8  rsd[] = {0x0A,0x01,0x01};
                uint8   InDataBuf[255];
                GetRecord_t getRecord;
                uint16 offset = 1;
                OOP_OAD_U MainOad;
                //OOP_OAD_U tempMainOad;
                uint16 tmpoffset;
                //OOP_OAD_U   RsdOad;
                //RsdOad.value= 0x20220200;//�¼���¼��űس�
                getRecord.rsd = rsd;
                getRecord.rsdLen = 3;
                getRecord.oad.value = 0x60120300;
                getRecord.rcsd = InDataBuf;
                MainOad = eventinfo->EventDI;

                offset = 1;
                getRecord.rcsd[offset++] = 0;
                getRecord.rcsd[offset++] = 0x60;
                getRecord.rcsd[offset++] = 0x40;
                getRecord.rcsd[offset++] = 0x02;
                getRecord.rcsd[offset++] = 0x00;

                getRecord.rcsd[offset++] = 0;
                getRecord.rcsd[offset++] = 0x60;
                getRecord.rcsd[offset++] = 0x41;
                getRecord.rcsd[offset++] = 0x02;
                getRecord.rcsd[offset++] = 0x00;

                getRecord.rcsd[offset++] = 0;
                getRecord.rcsd[offset++] = 0x60;
                getRecord.rcsd[offset++] = 0x42;
                getRecord.rcsd[offset++] = 0x02;
                getRecord.rcsd[offset++] = 0x00;

                getRecord.rcsd[offset++] = 0;
                getRecord.rcsd[offset++] = 0x20;
                getRecord.rcsd[offset++] = 0x2A;
                getRecord.rcsd[offset++] = 0x02;
                getRecord.rcsd[offset++] = 0x00;

                if(MainOad.nObjID !=0x0000)
                {
                    getRecord.rcsd[offset++] = 1;
                    getRecord.rcsd[offset++] = (uint8)(MainOad.nObjID >> 8);
                    getRecord.rcsd[offset++] = (uint8)(MainOad.nObjID);
                    getRecord.rcsd[offset++] = MainOad.attID;
                    getRecord.rcsd[offset++] = MainOad.nIndex;
                }
                tmpoffset = offset;
                getRecord.rcsd[offset++] = eventinfo->SubOadNum;
                for(i = 0; i < eventinfo->SubOadNum; i++)
                {

                    getRecord.rcsd[offset++] = (uint8)(eventinfo->SubOad[i].nObjID >> 8);
                    getRecord.rcsd[offset++] = (uint8)(eventinfo->SubOad[i].nObjID);
                    getRecord.rcsd[offset++] = eventinfo->SubOad[i].attID;
                    getRecord.rcsd[offset++] = eventinfo->SubOad[i].nIndex;

                }

                getRecord.rcsd[0] = (tmpoffset-1)/5;
                getRecord.rcsdLen = offset;
                OopGet.requst(pipe,&Tsa,3,(uint8*)&getRecord,sizeof(GetRecord_t));
                pLinkLayer = FindLinkLayer(pipe, NULL);
                Framelen = pLinkLayer->sendLen;
                memcpy(Framebuf,pLinkLayer->sendBuff,Framelen);
                eventinfo->SendTime = time(NULL);
                TASK_FMT_DEBUG( pipe, RELAY_LOG,"SendTime %ld \n",eventinfo->SendTime );
            }
            else
            {

                uint8  rsd[] = {0x09,0x01};
                uint8   InDataBuf[255];
                GetRecord_t getRecord;
                uint16 offset = 1;
                OOP_OAD_U   RsdOad;
                RsdOad.value= 0x20220200;//�¼���¼��űس�
                getRecord.rsd = rsd;
                getRecord.rsdLen = 2;
                getRecord.oad = eventinfo->EventDI;
                getRecord.rcsd = InDataBuf;
                getRecord.rcsd[offset++]=0;
                memcpy_r(&getRecord.rcsd[offset],(uint8*)&RsdOad,4);


                offset+=4;
                TASK_FMT_DEBUG( pipe, RELAY_LOG,"�¼���ǰ����mainoad %08x SubOadNum %d  \n",eventinfo->EventDI.value,eventinfo->SubOadNum);

                for(i=0;i<eventinfo->SubOadNum;i++)
                {
                    if(eventinfo->SubOad[i].nObjID!=RsdOad.nObjID)
                    {
                        getRecord.rcsd[offset++]=0;
                        memcpy_r(&getRecord.rcsd[offset],(uint8*)&eventinfo->SubOad[i].value,4);
                        offset+=4;
                        TASK_FMT_DEBUG( pipe, RELAY_LOG,"i %d  SubOad %08x \n",i,eventinfo->SubOad[i].value);

                    }
                }
                getRecord.rcsd[0] = (offset-1)/5;
                getRecord.rcsdLen = offset;
                OopGet.requst(pipe,&Tsa,3,(uint8*)&getRecord,sizeof(GetRecord_t));
                pLinkLayer = FindLinkLayer(pipe, NULL);
                Framelen = pLinkLayer->sendLen;
                memcpy(Framebuf,pLinkLayer->sendBuff,Framelen);
                eventinfo->SendTime = time(NULL);
                TASK_FMT_DEBUG( pipe, RELAY_LOG,"SendTime %ld \n",eventinfo->SendTime );

            }
            #else
            //��������ֱ��һ֡��һ���¼�
            uint8  rsd[] = {0x09,0x01};
            uint8   InDataBuf[255];
            GetRecord_t getRecord;
            uint16 offset = 1;
            OOP_OAD_U   RsdOad;
            RsdOad.value= 0x20220200;//�¼���¼��űس�
            getRecord.rsd = rsd;
            getRecord.rsdLen = 2;
            getRecord.oad = eventinfo->EventDI;
            getRecord.rcsd = InDataBuf;
            getRecord.rcsd[offset++]=0;
            memcpy_r(&getRecord.rcsd[offset],(uint8*)&RsdOad,4);


            offset+=4;
            TASK_FMT_DEBUG( pipe, RELAY_LOG,"�¼���ǰ����mainoad %08x SubOadNum %d  \n",eventinfo->EventDI.value,eventinfo->SubOadNum);

            for(i=0;i<eventinfo->SubOadNum;i++)
            {
                if(eventinfo->SubOad[i].nObjID!=RsdOad.nObjID)
                {
                    getRecord.rcsd[offset++]=0;
                    memcpy_r(&getRecord.rcsd[offset],(uint8*)&eventinfo->SubOad[i].value,4);
                    offset+=4;
                    TASK_FMT_DEBUG( pipe, RELAY_LOG,"i %d  SubOad %08x \n",i,eventinfo->SubOad[i].value);

                }
            }
            getRecord.rcsd[0] = (offset-1)/5;
            getRecord.rcsdLen = offset;
            //��������20150200���������¼���Ҫ���߼���ַ1
            if(prtl == PRTL_20_IOT && Tsa.oldvxd!=1)
            {
                Tsa.oldvxd = 1;
            }
#ifdef AREA_HUNAN
            if(eventinfo->EventDI.value ==0x305a0200 || eventinfo->EventDI.value == 0x305b0200)
            {
                Tsa.flag = 1;
                Tsa.vxd  = 2;
            }
#endif
            OopGet.requst(pipe,&Tsa,3,(uint8*)&getRecord,sizeof(GetRecord_t));
            pLinkLayer = FindLinkLayer(pipe, NULL);
            Framelen = pLinkLayer->sendLen;
            memcpy(Framebuf,pLinkLayer->sendBuff,Framelen);
            eventinfo->SendTime = time(NULL);
            TASK_FMT_DEBUG( pipe, RELAY_LOG,"SendTime %ld \n",eventinfo->SendTime );
            #endif
        }
        else
        {
            //645����Ҫ�ü�֡
            TASK_FMT_DEBUG( pipe, RELAY_LOG,"�¼���ǰ������������ CurDIIndex %d count %d  \n",eventinfo->CurDIIndex,eventinfo->count);

            if(eventinfo->CurDIIndex >=eventinfo->count)
            {
                TASK_FMT_DEBUG( pipe, RELAY_LOG,"��ǰ�¼�ȫ������ ����һ���¼� \n");
                pAmrThreadArg->EventReport.MtrEvent[MtrIndex].CurIndex++;
                return 2;
            }
            uint32 SendDI = eventinfo->AllDI[eventinfo->CurDIIndex] ;
            TASK_FMT_DEBUG( pipe, RELAY_LOG,"645������ %08x  \n",SendDI);

            MeterPrtlFmt_T  MeterPrtlData;
            memset(&MeterPrtlData,0x00,sizeof(MeterPrtlFmt_T));
            MeterPrtlData.PrtlMode =PRTL_07;
            memcpy_r(MeterPrtlData.MtrAddr,Tsa.add,6);
            MeterPrtlData.CtrlCode = 0x11;
            memcpy(MeterPrtlData.DataBuff,&SendDI,4);
            MeterPrtlData.DataLen = 4;
            Framelen = taskmng_meter_prtl_645_pack(Framebuf,MeterPrtlData);
            if(eventinfo->CurDIIndex ==0)
                eventinfo->SendTime = time(NULL);
        }

    }
        break;
    case EVENT_ONEMTR_END:
    {
        TASK_FMT_DEBUG( pipe, RELAY_LOG,"��ǰ���PN %d �¼����� Mtrindex %d \n",Pn,MtrIndex);
        if(prtl == PRTL_07)
        {
            TASK_FMT_DEBUG( pipe, RELAY_LOG,"645���¼��������������� datalen %d \n",pAmrThreadArg->EventReport.MtrEvent[MtrIndex].DataLen);
            taskmng_event_report_data_write( pipe, pAmrThreadArg->EventReport.MtrEvent[MtrIndex].DataBuf, pAmrThreadArg->EventReport.MtrEvent[MtrIndex].DataLen, EventReportOad645);

        }
        pAmrThreadArg->EventReport.MtrEvent[MtrIndex].Pn = 0;
        uint8 tmpIndex = MtrIndex;
        MtrIndex++;
        if(MtrIndex ==MAX_EVENT_MTR)
            MtrIndex = 0;
        while(MtrIndex<MAX_EVENT_MTR)
        {

            if(MtrIndex ==tmpIndex)
            {
                TASK_FMT_DEBUG( pipe, RELAY_LOG,"���е���¼�ȫ������ �������\n");
                pAmrThreadArg->EventReport.CurIndex = 0;
                return -1;
            }
            if(pAmrThreadArg->EventReport.MtrEvent[MtrIndex].Pn!=0)
            {
                pAmrThreadArg->EventReport.CurIndex = MtrIndex;
                TASK_FMT_DEBUG( pipe, RELAY_LOG,"����һ��� index %d Pn %d \n",MtrIndex,pAmrThreadArg->EventReport.MtrEvent[MtrIndex].Pn);
                return 2;
            }
            MtrIndex++;
            if(MtrIndex ==MAX_EVENT_MTR)
                MtrIndex = 0;
        }
        TASK_FMT_DEBUG( pipe, RELAY_LOG,"EVENT_ONEMTR_END MtrIndex %d \n",MtrIndex);
    }
        break;
    case EVENT_RESET_STATE:
    {
        //������������ȷ�ϵ��ܱ�����ϱ�״̬��
        if(prtl == PRTL_20_IOT)
        {
            uint8 buf[100] = {0};
            index = 0;
            buf[index++] = 0x20;
            buf[index++] = 0x15;
            buf[index++] = 0x7f;
            buf[index++] = 0x00;
            buf[index++] = DT_BIT_STRING;
            buf[index++] = 32;
            memcpy(&buf[index],pAmrThreadArg->EventReport.MtrEvent[MtrIndex].ReportState,4);
            index+=4;
            //��λʱ�߼���ַ��0
            Tsa.oldvxd = 0;
            OopAction.requst(pipe,&Tsa,1,buf,index);
            pLinkLayer = FindLinkLayer(pipe, NULL);
            if(pLinkLayer!=NULL)
            {
                Framelen = pLinkLayer->sendLen;
                memcpy(Framebuf,pLinkLayer->sendBuff,Framelen);
            }
            TASK_FMT_DEBUG( pipe, RELAY_LOG,"ȷ�ϵ��ܱ�����ϱ�״̬�� \n");

            break;
        }

        TASK_FMT_DEBUG( pipe, RELAY_LOG,"645����� ��λ�ϱ�״̬�� \n");
        if(pAmrThreadArg->EventReport.MtrEvent[MtrIndex].ResetStateCount>=3)
        {
            pAmrThreadArg->EventReport.MtrEvent[MtrIndex].state = EVENT_ONEMTR_END;
            TASK_FMT_DEBUG( pipe, RELAY_LOG,"Pn %d ��λ״̬��ʧ��̫���  \n",Pn);
            return 2;
        }
        MeterPrtlFmt_T  MeterPrtlData;
#ifdef AREA_ZHEJIANG
        uint8 Send[]={0x03,0x15,0x00,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
#else
        uint8 Send[]={0x03,0x15,0x00,0x04,0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

#endif
        //�����ϱ���״̬�ָ�λ
        for(i=0;i<12;i++)
        {
            Send[i+12]= ~(pAmrThreadArg->EventReport.MtrEvent[MtrIndex].ReportState[i]);
        }
        memset(&MeterPrtlData,0x00,sizeof(MeterPrtlFmt_T));
        MeterPrtlData.PrtlMode =PRTL_07;
        memcpy_r(MeterPrtlData.MtrAddr,Tsa.add,6);
        MeterPrtlData.CtrlCode = 0x14;
        memcpy(MeterPrtlData.DataBuff,Send,24);
        MeterPrtlData.DataLen =24;
        Framelen = taskmng_meter_prtl_645_pack(Framebuf,MeterPrtlData);
        pAmrThreadArg->EventReport.MtrEvent[MtrIndex].ResetStateCount++;
    }
        break;
    default:
    {
        pAmrThreadArg->EventReport.MtrEvent[MtrIndex].state = EVENT_ONEMTR_END;
        return 2;
    }
        break;
    }

    if(monitorParallFlag ==1)
    {
        //��003E��������ķ�ʽ��ȥ
        index = 0;
        memcpy_r(&payload[index],(uint8*)&pipeOAD.value,4);
        index+=4;
        index+=2;
        memcpy(&payload[index],Tsa.add,6);
        index+=6;
        if(Framelen<0x80)
        {
            payload[index++] = Framelen;
        }
        else
        {
            payload[index++] = 0x82;
            payload[index++] = (uint8)(Framelen>>8);
            payload[index++] = (uint8)Framelen;
        }
        memcpy(&payload[index],Framebuf,Framelen);
        index+=Framelen;

        taskmng_msg_data_pack(pipe,0x0003,0x003E,index,payload,&pAmrThreadArg->EventReport.SendIndex);
    }else
    {
        index = 0;
        memcpy_r(&payload[index],(uint8*)&pipeOAD.value,4);
        index+=4;
        if(prtl == PRTL_20_IOT )
            prtl = PRTL_OOP;
        payload[index++] = prtl;
        memcpy(&payload[index],Tsa.add,6);
        index+=6;
        index+=2;
        memcpy_r(&payload[index],(uint8*)&Overtime,2);
        index+=2;
        memcpy_r(&payload[index],(uint8*)&Overtime,2);
        index+=2;
        if(Framelen<0x80)
        {
            payload[index++] = Framelen;
        }
        else
        {
            payload[index++] = 0x82;
            payload[index++] = (uint8)(Framelen>>8);
            payload[index++] = (uint8)Framelen;
        }
        memcpy(&payload[index],Framebuf,Framelen);
        index+=Framelen;

        taskmng_msg_data_pack(pipe,0x0003,0x0038,index,payload,&pAmrThreadArg->EventReport.SendIndex);
    }


    TASK_FMT_DEBUG( pipe, RELAY_LOG,"����¼��ϱ����� PN %d state %d len %d sendindex %d\n",Pn,
        pAmrThreadArg->EventReport.MtrEvent[MtrIndex].state,index,pAmrThreadArg->EventReport.SendIndex);
    TASK_BUF_DEBUG( pipe, RELAY_LOG,payload, index);
    return 1;
}

/*******************************************************************************
* ��������: EventReportSendFrame
* ��������:��ȡ�¼��ϱ����͵ı���
* �������:pipe �˿ں�

* �������:
* �� �� ֵ: 1��ʾ�б��ķ��� 2��ʾһ������л�״̬ -1��ʾ���б���
*******************************************************************************/
int Rs485EventReportSendFrame(uint8 pipe)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    uint8 MtrIndex = pAmrThreadArg->EventReport.CurIndex;
    uint16 Pn = pAmrThreadArg->EventReport.MtrEvent[MtrIndex].Pn;
    uint8 prtl = pAmrThreadArg->EventReport.MtrEvent[MtrIndex].prtl;
    if(pAmrThreadArg->EventReport.MtrEvent[MtrIndex].FailCnt>=3)
    {
        TASK_FMT_DEBUG( pipe, RELAY_LOG,"Pn %d ʧ�ܴ�������3�� ���ٳ���  ",Pn);
        pAmrThreadArg->EventReport.MtrEvent[MtrIndex].state = EVENT_ONEMTR_END;
    }

    uint16 index=0;
    LinkLayer_t*   pLinkLayer = NULL;
    uint8 Framebuf[2000]={0};
    uint16 Framelen = 0;

    OOP_TSA_T Tsa = taskmng_tsa_get(Pn,LOGIC_ID_YC);
    uint8 i;
    //int ret;

    switch(pAmrThreadArg->EventReport.MtrEvent[MtrIndex].state)
    {
    case EVENT_GET_STATE:
    {
        TASK_FMT_DEBUG( pipe, RELAY_LOG,"Pn %d ��ʼ��485״̬��\n",Pn);
//        if(pAmrThreadArg->EventReport.MtrEvent[MtrIndex].GetStateCount>3)
//        {
//            pAmrThreadArg->EventReport.MtrEvent[MtrIndex].state = EVENT_ONEMTR_END;
//            TASK_FMT_DEBUG( pipe, RELAY_LOG,"Pn %d ״̬�ֳ�̫��� ����  \n",Pn);
//            return -1;
//        }
        MeterPrtlFmt_T  MeterPrtlData;
        uint8 SendDi[] = {0x01,0x15,0x00,0x04};
        memset(&MeterPrtlData,0x00,sizeof(MeterPrtlFmt_T));
        MeterPrtlData.PrtlMode =PRTL_07;
        memcpy_r(MeterPrtlData.MtrAddr,Tsa.add,6);
        MeterPrtlData.CtrlCode = 0x11;
        memcpy(MeterPrtlData.DataBuff,SendDi,4);
        MeterPrtlData.DataLen = 4;
        Framelen = taskmng_meter_prtl_645_pack(Framebuf,MeterPrtlData);
       // pAmrThreadArg->EventReport.MtrEvent[MtrIndex].GetStateCount++;
    }
        break;
    case EVENT_GET_EVENTLIST:
    {

        OOP_OAD_U Oad;
        Oad.value = 0x33200200;
        OopGet.requst(pipe,&Tsa,1,(uint8*)&Oad.value,4);
        pLinkLayer = FindLinkLayer(pipe, NULL);
        Framelen = pLinkLayer->sendLen;
        memcpy(Framebuf,pLinkLayer->sendBuff,Framelen);
        TASK_FMT_DEBUG( pipe, RELAY_LOG,"Pn %d ��ʼ����3320  \n",Pn);
    }
        break;
    case EVENT_GET_EVENT:
    {
        uint8 eventindex = pAmrThreadArg->EventReport.MtrEvent[MtrIndex].CurIndex;
        TASK_FMT_DEBUG( pipe, RELAY_LOG,"eventindex %d EventCount %d  \n",eventindex,pAmrThreadArg->EventReport.MtrEvent[MtrIndex].EventCount);
        if(pAmrThreadArg->EventReport.MtrEvent[MtrIndex].EventCount ==0)
        {
            pAmrThreadArg->EventReport.MtrEvent[MtrIndex].state = EVENT_ONEMTR_END;
            TASK_FMT_DEBUG( pipe, RELAY_LOG,"Pn %d �����ϱ�״̬������  \n",Pn);
            return 2;
        }
        if(eventindex>=pAmrThreadArg->EventReport.MtrEvent[MtrIndex].EventCount)
        {

            if(prtl == PRTL_OOP)
            {
                pAmrThreadArg->EventReport.MtrEvent[MtrIndex].state = EVENT_ONEMTR_END;
            }else
            {
                pAmrThreadArg->EventReport.MtrEvent[MtrIndex].state = EVENT_RESET_STATE;
            }
            return 2;
        }

        EVENT_INFO_T *eventinfo =NULL;
        eventinfo = &(pAmrThreadArg->EventReport.MtrEvent[MtrIndex].EventInfo[eventindex]);
        TASK_FMT_DEBUG( pipe, RELAY_LOG,"PN %d prtl %d �¼���ǰ����mainoad %08x SubOadNum %d  \n",Pn,prtl,eventinfo->EventDI.value,eventinfo->SubOadNum);
        if(prtl == PRTL_OOP)
        {
            //��������ֱ��һ֡��һ���¼�
            uint8  rsd[] = {0x09,0x01};
            uint8   InDataBuf[255];
            GetRecord_t getRecord;
            uint16 offset = 1;
            OOP_OAD_U   RsdOad;
            RsdOad.value= 0x20220200;//�¼���¼��űس�
            getRecord.rsd = rsd;
            getRecord.rsdLen = 2;
            getRecord.oad = eventinfo->EventDI;
            getRecord.rcsd = InDataBuf;
            getRecord.rcsd[offset++]=0;
            memcpy_r(&getRecord.rcsd[offset],(uint8*)&RsdOad,4);


            offset+=4;
            TASK_FMT_DEBUG( pipe, RELAY_LOG,"�¼���ǰ����mainoad %08x SubOadNum %d  \n",eventinfo->EventDI.value,eventinfo->SubOadNum);

            for(i=0;i<eventinfo->SubOadNum;i++)
            {
                if(eventinfo->SubOad[i].nObjID!=RsdOad.nObjID)
                {
                    getRecord.rcsd[offset++]=0;
                    memcpy_r(&getRecord.rcsd[offset],(uint8*)&eventinfo->SubOad[i].value,4);

                    offset+=4;
                    TASK_FMT_DEBUG( pipe, RELAY_LOG,"i %d  SubOad %08x \n",i,eventinfo->SubOad[i].value);

                }
            }
            getRecord.rcsd[0] = (offset-1)/5;
            getRecord.rcsdLen = offset;
            OopGet.requst(pipe,&Tsa,3,(uint8*)&getRecord,sizeof(GetRecord_t));
            pLinkLayer = FindLinkLayer(pipe, NULL);
            Framelen = pLinkLayer->sendLen;
            memcpy(Framebuf,pLinkLayer->sendBuff,Framelen);
            eventinfo->SendTime = time(NULL);
            TASK_FMT_DEBUG( pipe, RELAY_LOG,"SendTime %ld \n",eventinfo->SendTime );
        }
        else
        {
            //645����Ҫ�ü�֡
            TASK_FMT_DEBUG( pipe, RELAY_LOG,"�¼���ǰ������������ CurDIIndex %d count %d  \n",eventinfo->CurDIIndex,eventinfo->count);

            if(eventinfo->CurDIIndex >=eventinfo->count)
            {
                TASK_FMT_DEBUG( pipe, RELAY_LOG,"��ǰ�¼�ȫ������ ����һ���¼� \n");
                pAmrThreadArg->EventReport.MtrEvent[MtrIndex].CurIndex++;
                return 2;
            }
            uint32 SendDI = eventinfo->AllDI[eventinfo->CurDIIndex] ;
            MeterPrtlFmt_T  MeterPrtlData;
            memset(&MeterPrtlData,0x00,sizeof(MeterPrtlFmt_T));
            MeterPrtlData.PrtlMode =PRTL_07;
            memcpy_r(MeterPrtlData.MtrAddr,Tsa.add,6);
            MeterPrtlData.CtrlCode = 0x11;
            memcpy(MeterPrtlData.DataBuff,&SendDI,4);
            MeterPrtlData.DataLen = 4;
            Framelen = taskmng_meter_prtl_645_pack(Framebuf,MeterPrtlData);
            if(eventinfo->CurDIIndex ==0)
                eventinfo->SendTime = time(NULL);
        }

    }
        break;
    case EVENT_ONEMTR_END:
    {
        TASK_FMT_DEBUG( pipe, RELAY_LOG,"��ǰ���PN %d �¼����� Mtrindex %d \n",Pn,MtrIndex);
        if(prtl == PRTL_07)
        {
            TASK_FMT_DEBUG( pipe, RELAY_LOG,"645���¼��������������� datalen %d \n",pAmrThreadArg->EventReport.MtrEvent[MtrIndex].DataLen);
            taskmng_event_report_data_write( pipe, pAmrThreadArg->EventReport.MtrEvent[MtrIndex].DataBuf, pAmrThreadArg->EventReport.MtrEvent[MtrIndex].DataLen, EventReportOad645);

            TASK_BUF_DEBUG( pipe, RELAY_LOG,pAmrThreadArg->EventReport.MtrEvent[MtrIndex].DataBuf, pAmrThreadArg->EventReport.MtrEvent[MtrIndex].DataLen);
        }
        pAmrThreadArg->EventReport.MtrEvent[MtrIndex].Pn = 0;
        uint8 tmpIndex = MtrIndex;
        MtrIndex++;
        if(MtrIndex ==MAX_EVENT_MTR)
            MtrIndex = 0;
        while(MtrIndex<MAX_EVENT_MTR)
        {

            if(MtrIndex ==tmpIndex)
            {
                TASK_FMT_DEBUG( pipe, RELAY_LOG,"���е���¼�ȫ������ �������\n  ");
                return -1;
            }
            if(pAmrThreadArg->EventReport.MtrEvent[MtrIndex].Pn!=0)
            {
                pAmrThreadArg->EventReport.CurIndex = MtrIndex;
                TASK_FMT_DEBUG( pipe, RELAY_LOG,"����һ��� index %d Pn %d \n",MtrIndex,pAmrThreadArg->EventReport.MtrEvent[MtrIndex].Pn);
                return 1;
            }
            MtrIndex++;
            if(MtrIndex ==MAX_EVENT_MTR)
                MtrIndex = 0;
        }
        TASK_FMT_DEBUG( pipe, RELAY_LOG,"EVENT_ONEMTR_END MtrIndex %d \n",MtrIndex);
    }
        break;
    case EVENT_RESET_STATE:
    {
        TASK_FMT_DEBUG( pipe, RELAY_LOG,"645����� ��λ�ϱ�״̬�� \n",MtrIndex);
        if(pAmrThreadArg->EventReport.MtrEvent[MtrIndex].ResetStateCount>=3)
        {
            pAmrThreadArg->EventReport.MtrEvent[MtrIndex].state = EVENT_ONEMTR_END;
            TASK_FMT_DEBUG( pipe, RELAY_LOG,"Pn %d ��λ״̬��ʧ��̫���  \n",Pn);
            return 2;
        }
        MeterPrtlFmt_T  MeterPrtlData;
        uint8 Send[]={0x03,0x15,0x00,0x04,0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
        //�����ϱ���״̬�ָ�λ
        for(i=0;i<12;i++)
        {
            Send[i+12]= ~(pAmrThreadArg->EventReport.MtrEvent[MtrIndex].ReportState[i]);
        }
        memset(&MeterPrtlData,0x00,sizeof(MeterPrtlFmt_T));
        MeterPrtlData.PrtlMode =PRTL_07;
        memcpy_r(MeterPrtlData.MtrAddr,Tsa.add,6);
        MeterPrtlData.CtrlCode = 0x14;
        memcpy(MeterPrtlData.DataBuff,Send,24);
        MeterPrtlData.DataLen =24;
        Framelen = taskmng_meter_prtl_645_pack(Framebuf,MeterPrtlData);
        pAmrThreadArg->EventReport.MtrEvent[MtrIndex].ResetStateCount++;

    }
        break;
    default:
        break;
    }
    OOP_METER_BASIC_T MeterBasicInfo;
    taskmng_mtr_basic_info_get(Pn,LOGIC_ID_YC,&MeterBasicInfo);
    
    rs485_RS232_send(pipe,MAX_FRAME_OVERTIME,MAX_CHAR_OVERTIME,MeterBasicInfo.baud,PARITY_EVEN,Framebuf, Framelen);

    TASK_FMT_DEBUG( pipe, RELAY_LOG,"�¼��ϱ�����PN %d state %d len %d \n",Pn,pAmrThreadArg->EventReport.MtrEvent[MtrIndex].state,index);
    TASK_BUF_DEBUG( pipe, RELAY_LOG,Framebuf, Framelen);
    
    return 1;
}




/*******************************************************************************
* ��������: EventReportMqtt
* ��������://mqtt�߳����յ��ϱ��¼���Ĵ���
* �������:buf ��Ϣ����
        len ��Ϣ����

* �������:
* �� �� ֵ:
*******************************************************************************/
void EventReportMqtt(uint8* buf,uint16 len)
{
    int ret;
    uint16 index=0;
    uint8 DataBuf[2000]={0};
    uint16 DataLen;
    uint8 prtl;
    uint8 devicetype;
    uint16 pipe;
    uint8 lenoffset;
    OOP_OAD_U oad ;//ת���˿ڶ���������
    memcpy_r((uint8 *) &oad, &buf[index] , sizeof(OOP_OAD_U));
    pipe = taskmng_mtr_pipe_get(&oad);
    //TASK_BUF_TRACE(-1, RELAY_LOG,buf,len);
    //TASK_FMT_TRACE(-1, RELAY_LOG, "pipe is %d,oad.value is %x\n",pipe,oad.value);
    if(pipe>1)
        return;
    index+=4;
    devicetype = buf[index++]; //�豸���� 0 �ɼ��� 1 ���
    prtl = buf[index++];//00HΪ������01H ΪDL/T645��1997��02H ΪDL/T645��2007��03H����698.45,04H����ӽڵ�ͣ�������¼����ģ�05H����ܾ��ڵ���Ϣ�ϱ�����
    DataLen = XADR_GetEleNum(&buf[index],&lenoffset);
    index+=lenoffset;
    memcpy(DataBuf,&buf[index],DataLen);
    index+=DataLen;

    //�ݴ� �����Ϣ���Ȳ��Բ�����
    if(index+1>len)
        return;

    TASK_FMT_DEBUG(pipe, RELAY_LOG, "��Լ���� %d �豸���� %d ���ϱ��¼���������Ӧ�߳� datalen %d databuf \n",prtl,devicetype,DataLen);
    TASK_BUF_DEBUG(pipe, RELAY_LOG, DataBuf, DataLen);
    if(prtl == 4)
    {
        //ͣ�ϵ��¼��ϱ�
        if(devicetype == 0)
            ret = task_send_msg(gMqttSock,MSG_POWER_ON_OFF_COL,DataBuf,DataLen,IPC_AMR+pipe);
        else
            ret = task_send_msg(gMqttSock,MSG_POWER_ON_OFF_MET,DataBuf,DataLen,IPC_AMR+pipe);
    }else if(prtl == 5)
    {
        //�ڵ�ܾ���Ϣ�ϱ�
        ret = task_send_msg(gMqttSock,MSG_NODE_REJECT,DataBuf,DataLen,IPC_AMR+pipe);
    }else
    {
        //����¼��ϱ����߷Ǳ�̨������ϱ�
        ret = task_send_msg(gMqttSock,MSG_EVENT_REPORT,DataBuf,DataLen,IPC_AMR+pipe);
    }
    if(ret!=0)
        TASK_FMT_DEBUG(-1,MQTT_LOG,"���ϱ��¼����͸������̳߳��� ret %d \n",ret);

}




/*******************************************************************************
* ��������: ReportEventStateFun
* ��������:����645�����ϱ���״̬�� buf��12���ֽڵ�״̬�ֿ�ʼ Ȼ��AA ÿ���¼��ķ������� AA
* �������:pipe �˿ں�
        buf ��Ϣ����
        len ��Ϣ����
        index �������
* �������:
* �� �� ֵ: 0 ״̬��ȫ0 1 û����Ҫ�����¼�ֱ�Ӹ�λ״̬�� 2 ����Ҫ�����¼� -1 ���Ĳ��Ϸ�
*******************************************************************************/
int ReportEventStateFun(uint8 pipe,uint8* buf,uint16 len,uint16 index)
{
    uint8 i,j,k;
    uint8 EventCount=0;
    uint8 EventIndex = 0;
    uint8 tmpCount=0;
    uint8 SubEventCount=0;

    MTR_EVENT_REPORT_T *MtrEvent = NULL;
    MtrEvent = &gAmrThreadArg[pipe].EventReport.MtrEvent[index];
    uint8* pEventRecordNum = NULL;
    pEventRecordNum = buf+12; //��AA��ʼ 1���ô����һ���¼��Ĵ���
    for(i=0;i<96;i++)
    {
        if((buf[i/8] & (0x01<<(i%8)))!=0)
        {
            EventCount++;
            if(EventCount+12+2>len) //���ݳ��ȳ��� 12״̬�� �ټ�2AA
                return -1;
            const EventID_To_DI *pTmpEventIDToDI = NULL;
            pTmpEventIDToDI = GetEventIDToDIByIndex(i);

            if(pTmpEventIDToDI == NULL)
                continue;

            if(pEventRecordNum[EventCount] == 0)
            {
                continue;
            }else if(pEventRecordNum[EventCount] == 0xff)
            {
                tmpCount = 1;
            }else if(pEventRecordNum[EventCount] >10)
            {
                tmpCount = 10;
            }else
            {
                tmpCount =pEventRecordNum[EventCount] ;
            }
            if(pTmpEventIDToDI->DI_Frid[0]==0xffffffff)
            {
                MtrEvent->EventInfo[EventIndex].AllDI[SubEventCount++] = pTmpEventIDToDI->DI_EventCount;
            }else
            {
                MtrEvent->EventInfo[EventIndex].AllDI[SubEventCount++] = pTmpEventIDToDI->DI_EventCount;
                for(j=0;j<tmpCount;j++)
                {
                    //���ϼ����¼�
                    for(k=0;k<pTmpEventIDToDI->DINum;k++)
                    {
                        MtrEvent->EventInfo[EventIndex].AllDI[SubEventCount++] = pTmpEventIDToDI->DI_Frid[k]+0x00000001*j;
                    }
                }
            }
            MtrEvent->EventInfo[EventIndex].count = SubEventCount;
            EventIndex++;
            SubEventCount = 0;
        }
    }
    MtrEvent->EventCount = EventIndex;
    //ȫ0 ����û���¼�
    if(EventCount==0)
        return 0;

    //1��ʾ���¼����� ���ǲ���Ҫ�������¼�����
    if(MtrEvent->EventCount>0)
        return 2;
    else
        return 1;
}




/*******************************************************************************
* ��������: SetEventReportNum
* ��������:����Oad��Ӧ����Ϣ���
* �������:InfoNum ��Ϣ���
        Oad
* �������:
* �� �� ֵ:
*******************************************************************************/
void SetEventReportNum(uint16 InfoNum,OOP_OAD_U Oad)
{
    if(InfoNum ==0)
        InfoNum = 1;
    if(InfoNum>2000)
        InfoNum = 1;
    pthread_mutex_lock(&EventReportNumLock);
    if(Oad.value == EventReportOad.value)
        EventReportInfoNum = InfoNum;
    else if(Oad.value == EventReportOad645.value)
        EventReportInfoNum645 = InfoNum;
    else if(Oad.value == NodeRejectOad.value)
        NodeRejectNum = InfoNum;
    else if(Oad.value == PowerOnOffOad.value)
        PowerOnOffNum = InfoNum;

    else if(Oad.value == MinuteReportOad.value)
        {
        MinuteReportnfoNum = InfoNum;
        }

    #if (defined AREA_JIANGXI) || (defined AREA_ANHUI)
    else if(Oad.value == PerceptionTerminalOad.value)

        PerceptionTerminalInfoNum = InfoNum;
    #endif
    pthread_mutex_unlock(&EventReportNumLock);
}
/*******************************************************************************
* ��������: taskmng_event_report_num_get
* ��������:��ȡOad��Ӧ����Ϣ���
* �������:Oad

* �������:
* �� �� ֵ: ��Ϣ���
*******************************************************************************/
uint16 taskmng_event_report_num_get(OOP_OAD_U Oad)
{
    uint16 InfoNum = 0;
    pthread_mutex_lock(&EventReportNumLock);
    if(Oad.value ==EventReportOad.value )
    {
        if(EventReportInfoNum>2000)
            EventReportInfoNum = 1;
        InfoNum = EventReportInfoNum++;
    }else if(Oad.value == EventReportOad645.value)
    {
        if(EventReportInfoNum645>2000)
            EventReportInfoNum645 = 1;
        InfoNum = EventReportInfoNum645++;
    }else if(Oad.value == NodeRejectOad.value)
    {
        if(NodeRejectNum>2000)
            NodeRejectNum = 1;
        InfoNum = NodeRejectNum++;
    }else if(Oad.value == PowerOnOffOad.value)
    {
        if(PowerOnOffNum>2000)
            PowerOnOffNum = 1;
        InfoNum = PowerOnOffNum++;

    }
    else if(Oad.value == MinuteReportOad.value)
    {
        if(MinuteReportnfoNum>2000)
            MinuteReportnfoNum = 1;
        InfoNum = MinuteReportnfoNum++;
    }

    #if (defined AREA_JIANGXI) || (defined AREA_ANHUI)
    else if(Oad.value == PerceptionTerminalOad.value)
    {
        if(PerceptionTerminalInfoNum>2000)
            PerceptionTerminalInfoNum = 1;
        InfoNum = PerceptionTerminalInfoNum++;
    }
    #endif
    pthread_mutex_unlock(&EventReportNumLock);
    return InfoNum;
}
/*******************************************************************************
* ��������: GetCurrentEventReportNum
* ��������:��ȡOad��Ӧ����Ϣ���
* �������:Oad

* �������:
* �� �� ֵ: ��Ϣ���
*******************************************************************************/
uint16 GetCurrentEventReportNum(OOP_OAD_U Oad)
{
    uint16 InfoNum = 0;
    pthread_mutex_lock(&EventReportNumLock);
    if(Oad.value ==EventReportOad.value )
    {
        InfoNum = EventReportInfoNum;
    }else if(Oad.value == EventReportOad645.value)
    {
        InfoNum = EventReportInfoNum645;
    }else if(Oad.value == NodeRejectOad.value)
    {
        InfoNum = NodeRejectNum;
    }else if(Oad.value == PowerOnOffOad.value)
    {
        InfoNum = PowerOnOffNum;
    }


    #if (defined AREA_JIANGXI) || (defined AREA_ANHUI)
    else if(Oad.value == PerceptionTerminalOad.value)

    {
        InfoNum = PerceptionTerminalInfoNum;
    }
    #endif
    else if(Oad.value ==MinuteReportOad.value)
    {
        InfoNum = MinuteReportnfoNum;
    }



    pthread_mutex_unlock(&EventReportNumLock);
    return InfoNum;
}

/*******************************************************************************
* ��������: taskmng_event_report_num_load
* ��������://�ϵ�ʱ��ȡ���ݿ� ��ȡOad��Ӧ�������Ϣ���
* �������:

* �������:
* �� �� ֵ:
*******************************************************************************/
void taskmng_event_report_num_load(OOP_OAD_U Oad)
{
    int ret;
    uint32 handle;
    uint32 recNum;
    READ_RECORD_T ReadRecord;
    memset(&ReadRecord,0x00,sizeof(READ_RECORD_T));
    ReadRecord.recordOAD.logicId = 0;
    ReadRecord.recordOAD.infoNum = 0xffff;
    ReadRecord.recordOAD.road.oadMain.value = 0x00000000;
    ReadRecord.recordOAD.road.oadCols.nNum = 1;
    ReadRecord.recordOAD.road.oadCols.oad[0] = Oad;
    ReadRecord.cType = COL_NO_INFO;
    ReadRecord.cStart = 0;
    ReadRecord.cEnd = 65535;
    ReadRecord.sortType = DOWN_SORT;

    ret = db_read_record_start(gDBClientMain,&ReadRecord,&handle,&recNum);
    if(ret !=0 ||recNum==0 )
    {
        SetEventReportNum(1,Oad);
        return;
    }

    STEP_READ_RECORD_T StepRead;
    memset(&StepRead,0x00,sizeof(STEP_READ_RECORD_T));
    RECORD_UNIT_T Record;
    memset(&Record,0x00,sizeof(RECORD_UNIT_T));
    ret = db_read_record_step(gDBClientMain,handle,&StepRead,&Record);
    if(ret !=0)
    {
        SetEventReportNum(1,Oad);
        return;
    }
    SetEventReportNum(Record.infoNum+1,Oad);
    db_read_record_end(gDBClientMain, handle);
}


/*******************************************************************************
* ��������: AddEventReport
* ��������://645����ʱ��ʾ�к���֡
* �������:Addr ����ַ

* �������:
* �� �� ֵ:
*******************************************************************************/
void AddEventReport(uint8* Addr)
{
    uint8 pipe = taskmng_pipe_get_from_addr(Addr,LOGIC_ID_YC);
    if(pipe>1)
        return;
    uint8 Pn = taskmng_pn_get_from_addr(Addr,LOGIC_ID_YC);
    uint8 prtl;
    taskmng_prtl_get_from_addr(Addr,LOGIC_ID_YC,&prtl);
    uint8 i;
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    for(i=0;i<MAX_EVENT_MTR;i++)
    {
        if(pAmrThreadArg->EventReport.MtrEvent[i].Pn == 0)
        {
            memset(&pAmrThreadArg->EventReport.MtrEvent[i],0x00,sizeof(MTR_EVENT_REPORT_T));
            pAmrThreadArg->EventReport.MtrEvent[i].Pn =Pn;
            pAmrThreadArg->EventReport.MtrEvent[i].prtl =prtl;
            pAmrThreadArg->EventReport.MtrEvent[i].state = EVENT_GET_STATE;
            TASK_FMT_DEBUG( pipe, RELAY_LOG, "����¼��ϱ����� i %d pn %d \n",i,Pn);
            taskmng_plc_task_queue_add(pipe,MS_EVENT_REPORT);
            pAmrThreadArg->EventReport.AllCount++;
            break;
        }
    }

}

/*******************************************************************************
* ��������: DealEventReport
* ��������://645����ʱ��ʾ�к���֡
* �������:Addr ����ַ

* �������:
* �� �� ֵ:
*******************************************************************************/
void DealEventReport(uint8 pipe,uint8 prtl ,uint16 pn)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];

    memset(&pAmrThreadArg->EventReport.MtrEvent[0],0x00,sizeof(MTR_EVENT_REPORT_T));
    pAmrThreadArg->EventReport.MtrEvent[0].Pn =pn;
    pAmrThreadArg->EventReport.MtrEvent[0].prtl =prtl;
    pAmrThreadArg->EventReport.MtrEvent[0].state = EVENT_GET_STATE;
    pAmrThreadArg->EventReport.CurIndex = 0;
    pAmrThreadArg->TotalState.EventReportState = INNER_RTS_IDLE;
    TASK_FMT_DEBUG( pipe, RELAY_LOG, "���645_07�¼��ϱ�����\n");  
    RS485EventReportDeal(pipe);
}

/*******************************************************************************
* ��������: OOPFollowReport
* ��������://698�и����ϱ��� �����33200201���������������¼�
* �������:pipe �˿ں�
        Pn ����
        buf ��������
        len ���ĳ���

* �������:
* �� �� ֵ:
*******************************************************************************/
void OOPFollowReport(uint8 pipe,uint16 Pn,uint8* buf,uint16 len)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    uint16 newpn = 0;
    OOP_OAD_U oad;
    OOP_OAD_U SubOad[MAX_OAD_NUM];
    uint8 SubOadcount;
    memset(SubOad,0x00,sizeof(OOP_OAD_U)*MAX_OAD_NUM);
    uint8 i;
    if(len<1+1+4+1 +1+4) //�����ϱ���
        return;
    uint16 index = 0;
    if(buf[index++]!=1)    //���ɸ��������Լ����� [1] SEQUENCE OF A-ResultNormal
        return;
    index++;
    memcpy_r((uint8*)&oad,&buf[index],4);
    index+=4;
    if(oad.value!=0x33200201)
        return;
    if(buf[index++]==0)
        return;
    if(buf[index++]!=DT_OAD)
        return;
    memcpy_r((uint8*)&oad,&buf[index],4);
    index+=4;

    SubOadcount = GetEventSubOad(Pn,oad,SubOad,&newpn);
    if(SubOadcount==0)
    {
        TASK_FMT_DEBUG( pipe, RELAY_LOG,"�����ϱ����ϱ����¼� oad %08x δ�ҵ���Ӧ�ķ���\n",oad.value);
        return;
    }
    TASK_FMT_DEBUG( pipe, RELAY_LOG,"�����ϱ����ϱ����¼� oad %08x ׼������\n",oad.value);
    for(i=0;i<MAX_EVENT_MTR;i++)
    {
        TASK_FMT_DEBUG( pipe, RELAY_LOG,"i %d pn %d\n",i,pAmrThreadArg->EventReport.MtrEvent[i].Pn);
        if(pAmrThreadArg->EventReport.MtrEvent[i].Pn == 0)
        {
            memset(&pAmrThreadArg->EventReport.MtrEvent[i],0x00,sizeof(MTR_EVENT_REPORT_T));
            pAmrThreadArg->EventReport.MtrEvent[i].Pn =newpn;
            pAmrThreadArg->EventReport.MtrEvent[i].prtl =PRTL_OOP;
            pAmrThreadArg->EventReport.MtrEvent[i].state = EVENT_GET_EVENT;
            pAmrThreadArg->EventReport.MtrEvent[i].EventCount = 1;
            pAmrThreadArg->EventReport.MtrEvent[i].EventInfo[0].EventDI = oad;
            pAmrThreadArg->EventReport.MtrEvent[i].EventInfo[0].SubOadNum = SubOadcount;
            memcpy(pAmrThreadArg->EventReport.MtrEvent[i].EventInfo[0].SubOad,SubOad,sizeof(OOP_OAD_U)*SubOadcount);
            TASK_FMT_DEBUG( pipe, RELAY_LOG, "����¼��ϱ����� i %d  pn %d\n",i,newpn);
            taskmng_plc_task_queue_add(pipe,MS_EVENT_REPORT);
            pAmrThreadArg->EventReport.AllCount++;
            break;
        }
    }
}

/*******************************************************************************
* ��������: OOPFollowReport
* ��������://698�и����ϱ��� �����33200201���������������¼�
* �������:pipe �˿ں�
        Pn ����
        buf ��������
        len ���ĳ���

* �������:
* �� �� ֵ:
*******************************************************************************/
void OOPRS485FollowReport(uint8 pipe,uint16 Pn,uint8* buf,uint16 len)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    OOP_OAD_U oad;
    OOP_OAD_U SubOad[MAX_OAD_NUM];
    uint8 SubOadcount;
    memset(SubOad,0x00,sizeof(OOP_OAD_U)*MAX_OAD_NUM);
    uint16 newpn = 0;
    
    TASK_FMT_DEBUG( pipe, RELAY_LOG,"OOPRS485FollowReport \n");
    TASK_BUF_DEBUG( pipe, RELAY_LOG,buf,len);
    if(len<1+1+4+1 +1+4) //�����ϱ��� 
        return;
    uint16 index = 0;
    if(buf[index++]!=1)    //���ɸ��������Լ����� [1] SEQUENCE OF A-ResultNormal
        return;
    index++;
    memcpy_r((uint8*)&oad,&buf[index],4);
    index+=4;
    if(oad.value!=0x33200201)
        return;
    if(buf[index++]==0) 
        return;
    if(buf[index++]!=DT_OAD)
        return;
    memcpy_r((uint8*)&oad,&buf[index],4);
    index+=4;

    SubOadcount = GetEventSubOad(Pn,oad,SubOad,&newpn);
    if(SubOadcount==0)
    {
        TASK_FMT_DEBUG( pipe, RELAY_LOG,"�����ϱ����ϱ����¼� oad %08x δ�ҵ���Ӧ�ķ���\n",oad.value);
        return;
    }
    TASK_FMT_DEBUG( pipe, RELAY_LOG,"�����ϱ����ϱ����¼� oad %08x ׼������\n",oad.value);

   

    memset(&pAmrThreadArg->EventReport.MtrEvent[0],0x00,sizeof(MTR_EVENT_REPORT_T));
    pAmrThreadArg->EventReport.MtrEvent[0].Pn =newpn;
    pAmrThreadArg->EventReport.MtrEvent[0].prtl =PRTL_OOP;
    pAmrThreadArg->EventReport.MtrEvent[0].state = EVENT_GET_EVENT;
    pAmrThreadArg->EventReport.MtrEvent[0].EventCount = 1;
    pAmrThreadArg->EventReport.MtrEvent[0].EventInfo[0].EventDI = oad;
    pAmrThreadArg->EventReport.MtrEvent[0].EventInfo[0].SubOadNum = SubOadcount;
    memcpy(pAmrThreadArg->EventReport.MtrEvent[0].EventInfo[0].SubOad,SubOad,sizeof(OOP_OAD_U)*SubOadcount);
    pAmrThreadArg->EventReport.CurIndex = 0;
    pAmrThreadArg->TotalState.EventReportState = INNER_RTS_IDLE;
    RS485EventReportDeal(pipe);



}

/*******************************************************************************
* ��������: taskmng_normal_data_delete
* ��������:ɾ����ͨ����
* �������:infoNum ��Ϣ���
    Oad

* �������:
* �� �� ֵ:
*******************************************************************************/
void taskmng_normal_data_delete(uint16 infoNum,OOP_OAD_U Oad)
{
    RECORD_OAD_T RecordDel;
    RecordDel.logicId = 0;
    RecordDel.classtag = CLASS_DATA_NORMAL;
    RecordDel.infoNum = infoNum;
    RecordDel.road.oadMain.value = 0;
    RecordDel.road.oadCols.nNum= 1;
    RecordDel.road.oadCols.oad[0] = Oad;
    CLEAR_DATA_T clear;
    clear.recordOAD = RecordDel;
    clear.bClassTag = FALSE;
    int ret;
    ret = db_clear_oad_data(gDBClientMain,&clear);
    if(ret==0)
    {
        TASK_FMT_TRACE(-1, MQTT_LOG, "ɾ���ɹ� infonum %d oad %08x \n",infoNum,Oad.value);
    }else
    {
        TASK_FMT_TRACE(-1, MQTT_LOG, "ɾ��ʧ�� infonum %d oad %08x \n",infoNum,Oad.value);
    }

}



/*******************************************************************************
* ��������: HPLCInfoUpFun
* ��������:MQTT�߳��յ��ϱ���HPLC��Ϣ���д���
* �������:buf ��Ϣ����
        len ��Ϣ����

* �������:
* �� �� ֵ:
*******************************************************************************/
void HPLCInfoUpFun(uint8* buf,uint16 len)
{
    uint16 index=0;
    uint8 pipe;

    OOP_OAD_U pipeOAD ;
    memset(&pipeOAD,0x00,sizeof(OOP_OAD_U));
    memcpy_r((uint8*)&pipeOAD.value,&buf[index],4);
    index+=4;
    pipe = taskmng_mtr_pipe_get(&pipeOAD);

    if(pipe>1)
        return ;
    task_send_msg(gMqttSock,MSG_HPLC_INFO,buf,len,IPC_AMR+pipe);

}





/*******************************************************************************
* ��������: DeleteHPLCInfo
* ��������:ɾ����һ�ֲ�ѯ��HPLC��Ϣ
* �������:pipe �˿ں�

* �������:
* �� �� ֵ:
*******************************************************************************/
void DeleteHPLCInfo(uint8 pipe)
{
    OOP_OAD_U OAD;

    OAD.value = 0XF2090700;
    taskmng_normal_data_delete_pipe(pipe,0xffff,OAD);

    OAD.value = 0XF2090500;
    taskmng_normal_data_delete_pipe(pipe,0xffff,OAD);

    OAD.value = 0XF2090800;
    taskmng_normal_data_delete_pipe(pipe,0xffff,OAD);

    OAD.value = 0XF2090A00;
    taskmng_normal_data_delete_pipe(pipe,0xffff,OAD);

    OAD.value = 0XF2091400;
    taskmng_normal_data_delete_pipe(pipe,0,OAD);

    OAD.value = 0XF2090200;
    taskmng_normal_data_delete_pipe(pipe,0,OAD);

    OAD.value = 0XF2091E00;
    taskmng_normal_data_delete_pipe(pipe,0,OAD);

    OAD.value = 0XF2091F00;
    taskmng_normal_data_delete_pipe(pipe,0xffff,OAD);
}




/*******************************************************************************
* ��������: taskmng_task_monitor_update
* ��������://���������ز���
* �������:pipe �˿ں�
        TaskId �����
        cmd ��������
        indata ����������Ĳ���
* �������:
* �� �� ֵ:
*******************************************************************************/
void GetTaskRealBegiantime(uint8 pipe,uint16 taskID,uint8 logicid,CTS_RUN_TASK_T *indata)
{
    uint8 i;
    //int ret;
    OOP_ACQ_MONITOR_T* taskMonitorUnit = NULL;

    if(pipe!=AMR_THREAD_NUM)
    {
        AmrThreadArg_T* pAmrThreadArg;
        pAmrThreadArg = &gAmrThreadArg[pipe];
        if(pAmrThreadArg->TmnAmrInfo.MtrCnt == 0)
        {
            TASK_FMT_TRACE( pipe, REC_LOG, " ��ǰ�˿�û�е��������������ز���\n");
            return;
        }

    }
    pthread_mutex_lock(&TaskMonitor.MonitorLock);

    for(i=0;i<TaskMonitor.TaskCount;i++)
    {
        if(taskID == TaskMonitor.TaskMonitorInfo[i].id && logicid ==TaskMonitor.logicid[i] )
        {
            taskMonitorUnit = &TaskMonitor.TaskMonitorInfo[i];
            break;
        }
    }
    if(taskMonitorUnit==NULL)
    {
        pthread_mutex_unlock(&TaskMonitor.MonitorLock);
        if(pipe!=AMR_THREAD_NUM)
            TASK_FMT_TRACE( pipe, REC_LOG, "taskmng_task_monitor_update δ�ҵ���ص�Ԫ \n");
        return;
    }
    indata->ThisTurnRealBeginTime[6] = 0x20;
    indata->ThisTurnRealBeginTime[5] = commfun_HexToBcd((uint8)(taskMonitorUnit->startTime.year%100));  //��
    indata->ThisTurnRealBeginTime[4] = commfun_HexToBcd((uint8)(taskMonitorUnit->startTime.month));     //��
    indata->ThisTurnRealBeginTime[3] = commfun_HexToBcd((uint8)taskMonitorUnit->startTime.day);         //��
    indata->ThisTurnRealBeginTime[2] = commfun_HexToBcd((uint8)taskMonitorUnit->startTime.hour);            //ʱ
    indata->ThisTurnRealBeginTime[1] = commfun_HexToBcd((uint8)taskMonitorUnit->startTime.minute);          //��
    indata->ThisTurnRealBeginTime[0] = commfun_HexToBcd((uint8)taskMonitorUnit->startTime.second);          //��
    pthread_mutex_unlock(&TaskMonitor.MonitorLock);
}








/**
*********************************************************************
* @brief:      ʵ��  485͸�������������ܣ�
* @param[in]��   pipe  �̺߳�,485ģ�鹲8���߳�,485���߳�pipe��2��ʼ��9
* @param[in]��   SCHEME_TASK_RUN_INFO *TaskInfo  �������
* @param[in]��  CTS_RUN_TASK_T* RunTask ����ʱ������Ϣ
* @return��1 ȫ������ -1 δ����
*********************************************************************
*/
int Rs485TransDataReading(uint8 pipe,SCHEME_TASK_RUN_INFO *TaskInfo,CTS_RUN_TASK_T* RunTask)
{
    AmrThreadArg_T  *pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    uint16 i;
    int             RecvLen = 0;
    uint8           RecvBuf[MAX_MSG_LEN] = {0};
    //uint8           PayloadBuf[MAX_MSG_LEN] = {0};        //payload������
    //MSG_INFO_T* MsgInfo = NULL;
    //AMR_PLC_INFO_REPORT_T  AmrReportInfo;    //�ϱ��ĳ�������
    uint8           SendIndex=0;
    uint16 MtrIndex,pn;
    uint16 TaskId;
    //int             OverTimeCnt = 60;
    SCHEME_TASK_RUN_INFO RecvTaskInfo ;
    uint16 reallen=0;
    memset(&RecvTaskInfo,0x00,sizeof(SCHEME_TASK_RUN_INFO));
    pAmrThreadArg->TotalState.NormalAmr = RTS_IDLE;
    while(1)
    {
        switch(pAmrThreadArg->TotalState.NormalAmr)
        {
        case RTS_IDLE:
        {
            for(i=0;i<pAmrThreadArg->TmnAmrInfo.MtrCnt;i++)
            {
                pAmrThreadArg->pMtrAmrInfo[i].ROADIndex = 0;//͸��������ʹ����Ϊÿ������ķ������ĵ�����
                pAmrThreadArg->pMtrAmrInfo[i].BroadFailCnt=0;

            }
            //taskmng_task_record_file_load(pipe,TaskInfo,RunTask);
            TASK_FMT_TRACE( pipe, REC_LOG, "taskmng_data_reading_trans ��շ��Ͷ���\n");
            RunTask->StartTime = time(NULL);
            pAmrThreadArg->TotalState.NormalAmr = RTS_SEND;
        }
            break;
        case RTS_SEND:
        {
            if(Rs485TransFrameSend(pipe,TaskInfo,RunTask)==1)
            {
                TASK_FMT_TRACE( pipe, REC_LOG, "ȫ������ ͸����������%d ����&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&\n",TaskInfo->TaskId);
                pAmrThreadArg->TotalState.NormalAmr = RTS_IDLE;
                return 1;
            }
            pAmrThreadArg->TotalState.NormalAmr = RTS_RECV;
            TASK_FMT_TRACE(pipe, REC_LOG,"RunTask->TaskId11 is %d\n",RunTask->TaskId);
            taskmng_task_monitor_update(pipe,RunTask->TaskId,RunTask->logicid,UDTASK_SENDFRMNUM,NULL);
        }
            break;
        case RTS_RECV:
        {

            RecvLen = rs485_RS232_recv(pipe,RecvBuf, sizeof(RecvBuf));
            if(RecvLen > 0)
            {
                TASK_FMT_TRACE(pipe, REC_LOG,"@@@@@�յ������ϱ�����@@@@@@@@@@ \n");
                uint8 Prtl;
                uint8 *ptr = NULL;
                uint16 Offset=0;
                pn=TaskInfo->MsInfo[RunTask->MeterIndex].Sn;
                MtrIndex = taskmng_ms_index_get(pipe,pn,LOGIC_ID_YC);
                pAmrThreadArg->pMtrAmrInfo[MtrIndex].LastRecvTime = time(NULL);
                TaskId = pAmrThreadArg->pMtrAmrInfo[MtrIndex].LastTaskId;
                taskmng_mtr_prtl_get(pAmrThreadArg->pMtrAmrInfo[MtrIndex].Pn,LOGIC_ID_YC,&Prtl);
                if(Prtl == PRTL_OOP)
                {
                    RecvLen = GetFrame(RecvBuf, RecvLen, &Offset);
                    if(RecvLen == 0)
                    {
                        TASK_FMT_TRACE(pipe, REC_LOG,"oop��Լû���յ��Ϸ��ĳ�����\n");
                        pAmrThreadArg->pMtrAmrInfo[MtrIndex].BroadFailCnt = 0;
                        pAmrThreadArg->pMtrAmrInfo[MtrIndex].ROADIndex++;
                        pAmrThreadArg->AmrSendFrameInfo.Queue[SendIndex].ValidFlag=0;
                        if(pAmrThreadArg->AmrSendFrameInfo.QueueCnt>0)
                            pAmrThreadArg->AmrSendFrameInfo.QueueCnt--;
                        pAmrThreadArg->TotalState.NormalAmr = RTS_SEND;
                        break;
                    }

                }
                else
                {
                    ptr = (uint8*)commfun_645_frame_get((char*)RecvBuf, 0x68, RecvLen,&reallen);//�ҵ�645���ĵ�68ͷ
                    if(ptr == NULL)
                    {
                       TASK_FMT_TRACE(pipe, REC_LOG,"645��Լû���յ��Ϸ��ĳ�����\n");
                        pAmrThreadArg->pMtrAmrInfo[MtrIndex].BroadFailCnt = 0;
                        pAmrThreadArg->pMtrAmrInfo[MtrIndex].ROADIndex++;
                        pAmrThreadArg->AmrSendFrameInfo.Queue[SendIndex].ValidFlag=0;
                        if(pAmrThreadArg->AmrSendFrameInfo.QueueCnt>0)
                            pAmrThreadArg->AmrSendFrameInfo.QueueCnt--;
                        pAmrThreadArg->TotalState.NormalAmr = RTS_SEND;
                        break;
                    }
                    RecvLen = reallen;//��645���ĳ���
                    if(RecvLen <=0)
                    {
                        TASK_FMT_TRACE(pipe, REC_LOG,"645��Լû���յ��Ϸ��ĳ�����\n");
                        pAmrThreadArg->pMtrAmrInfo[MtrIndex].BroadFailCnt = 0;
                        pAmrThreadArg->pMtrAmrInfo[MtrIndex].ROADIndex++;
                        pAmrThreadArg->AmrSendFrameInfo.Queue[SendIndex].ValidFlag=0;
                        if(pAmrThreadArg->AmrSendFrameInfo.QueueCnt>0)
                            pAmrThreadArg->AmrSendFrameInfo.QueueCnt--;
                        pAmrThreadArg->TotalState.NormalAmr = RTS_SEND;
                        break;
                    }
                    memmove(RecvBuf, ptr, RecvLen);
                }
                TASK_FMT_TRACE(pipe, REC_LOG,"TaskId is %d,pAmrThreadArg->TmnAmrInfo.CurrentTaskId is %d\n",TaskId,pAmrThreadArg->TmnAmrInfo.CurrentTaskId);
                TASK_FMT_TRACE(pipe, REC_LOG,"RunTask->TaskId22 is %d\n",RunTask->TaskId);
                taskmng_task_monitor_update(pipe,RunTask->TaskId,RunTask->logicid,UDTASK_RECVFRMNUM,NULL);
                uint8 seq = pAmrThreadArg->AmrSendFrameInfo.Queue[SendIndex].FrameSeq;
                taskmng_trans_data_save(pipe,TaskInfo,RunTask,MtrIndex,seq,RecvBuf+Offset,RecvLen);
                pAmrThreadArg->pMtrAmrInfo[MtrIndex].BroadFailCnt = 0;
                pAmrThreadArg->pMtrAmrInfo[MtrIndex].ROADIndex++;
                pAmrThreadArg->AmrSendFrameInfo.Queue[SendIndex].ValidFlag=0;
                if(pAmrThreadArg->AmrSendFrameInfo.QueueCnt>0)
                    pAmrThreadArg->AmrSendFrameInfo.QueueCnt--;
                pAmrThreadArg->TotalState.NormalAmr = RTS_SEND;
            }
            else
            {
                for(i=0;i<1;i++)
                {
                    if(pAmrThreadArg->AmrSendFrameInfo.Queue[i].ValidFlag==1)
                    {
                        //if((time(NULL)-pAmrThreadArg->AmrSendFrameInfo.Queue[i].CommBeginTime)>OverTimeCnt)
                        {
                            TASK_FMT_TRACE( pipe, REC_LOG, "Queue index %d ��ʱ PN %d \n",i,pAmrThreadArg->AmrSendFrameInfo.Queue[i].Pn);
                            taskmng_pn_state_refresh( pipe, i);
                            pAmrThreadArg->TotalState.NormalAmr = RTS_SEND;
                        }
                    }

                }
            }
        }
            break;
        default:
            {
                pAmrThreadArg->TotalState.NormalAmr = RTS_IDLE;
                return -1;
            }
            break;
        }
    }

}


/*******************************************************************************
* ��������: Rs485TransFrameSend
* ��������://485͸�������ȡ��������
* �������:pipe �˿ں�
        TaskInfo �������
        RunTask ��������ʱ����
* �������:
* �� �� ֵ:1 ȫ������ -1 δ����
*******************************************************************************/
int Rs485TransFrameSend(uint8 pipe,SCHEME_TASK_RUN_INFO *TaskInfo,CTS_RUN_TASK_T* RunTask)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    uint8 i;
    int ret;
    int result;
    uint16 MsIndex;
    uint16 Pn;
    uint8 logicid;
    uint8 MtrAddr[6]={0};
    uint8 prtl;
    //uint8 PayLoad[1000]={0};
    //uint16 index=0;
    //OOP_OAD_U pipeOAD = taskmng_pipe_oad_get(pipe);
    uint16 FrameLen;
    uint8 frameindex;

    for(i=0;i<1;i++)
    {
        if(pAmrThreadArg->AmrSendFrameInfo.Queue[i].ValidFlag==0)
        {
            ret = taskmng_next_trans_buf_get(pipe,TaskInfo,RunTask,&MsIndex);
            if(ret == SEARCH_SUCCESS)
            {
                Pn = pAmrThreadArg->pMtrAmrInfo[MsIndex].Pn;
                logicid = pAmrThreadArg->pMtrAmrInfo[MsIndex].logicid;
                taskmng_mtr_addr_prtl_get( Pn, logicid,MtrAddr, &prtl);
                pAmrThreadArg->AmrSendFrameInfo.QueueCnt++;
                pAmrThreadArg->AmrSendFrameInfo.Queue[i].ValidFlag =1;
                pAmrThreadArg->AmrSendFrameInfo.Queue[i].Pn = Pn;
                pAmrThreadArg->AmrSendFrameInfo.Queue[i].MsIndex = MsIndex;
                pAmrThreadArg->AmrSendFrameInfo.Queue[i].CommBeginTime = time(NULL);
                pAmrThreadArg->pMtrAmrInfo[MsIndex].LastSendTime = time(NULL);//��ǰ������һ�η���ʱ��
                pAmrThreadArg->AmrSendFrameInfo.Queue[i].PrtlMode = prtl;
                memcpy(pAmrThreadArg->AmrSendFrameInfo.Queue[i].MeterAddr,MtrAddr,6);
                //��FrameSeq�洢͸��������ǰ���͵���������е�����
                pAmrThreadArg->AmrSendFrameInfo.Queue[i].FrameSeq = RunTask->MeterIndex;
                frameindex = pAmrThreadArg->pMtrAmrInfo[MsIndex].ROADIndex;
                FrameLen = TaskInfo->TransInfo[RunTask->MeterIndex].cmd.value[frameindex].data.nNum;
                TASK_FMT_TRACE( pipe, REC_LOG, "Pn %d MsIndex %d prtl %d frameindex %d CommBeginTime %ld \n",Pn,MsIndex,prtl,frameindex,time(NULL));
                TASK_BUF_TRACE(pipe, REC_LOG,TaskInfo->TransInfo[RunTask->MeterIndex].cmd.value[frameindex].data.value,FrameLen);

                OOP_METER_BASIC_T MeterBasicInfo;
                taskmng_mtr_basic_info_get(Pn,logicid,&MeterBasicInfo);

                result=rs485_RS232_send(pipe,MAX_FRAME_OVERTIME,MAX_CHAR_OVERTIME,MeterBasicInfo.baud,PARITY_EVEN,TaskInfo->TransInfo[RunTask->MeterIndex].cmd.value[frameindex].data.value, FrameLen);
                if(result<0)
                {
                    return 1;
                }


            }
            else if(ret == SEARCH_FINISH)
            {
                return 1;
            }else
            {
                return 0;
            }

        }
    }
    return 0;
}






/*******************************************************************************
* ��������: taskmng_register_app_check
* ��������:���ָ��APP�Ƿ�ע��ɹ�
* �������:AppName APP����
* �������:
* �� �� ֵ:TRUE ע��ɹ� FALSE ע��ʧ��
*******************************************************************************/
BOOL taskmng_register_app_check(char* AppName)
{
    int ret;
    uint16 IID = 0x0001;
    uint16 IOP = 0x0013;
    uint16 len = 0;
    uint8 PayLoad[200] = {0};
    uint16 RecvLen;
    uint8 RecvBuf[MAX_MSG_LEN]={0};
    char* DestAddr = "M-smiOS";

    memset(RecvBuf,0x00,MAX_MSG_LEN);
    PayLoad[len++] = strlen(AppName);
    memcpy(&PayLoad[len],AppName,strlen(AppName));
    len+=strlen(AppName);
    TASK_FMT_DEBUG( -1, UP_LOG, " ��ѯAPP %s ע����Ϣ  \n",AppName);
    ret = taskmng_up_msg_fun_deal(IID,IOP,len,PayLoad,DestAddr,&RecvLen,RecvBuf);
    if(ret>0)
    {
        TASK_FMT_DEBUG( -1, UP_LOG, " ��ѯAPP %s ע����Ϣ�ɹ���RecvLen %d  \n",AppName,RecvLen);
        TASK_BUF_DEBUG( -1, UP_LOG,RecvBuf,RecvLen);
        if(RecvLen>0)
        {
            char ResultName[100]={0};
            uint16 index = 0;
            uint8 namelen = RecvBuf[index++];
            memcpy(ResultName,&RecvBuf[index],namelen);
            TASK_FMT_DEBUG( -1, UP_LOG, " ��ѯ����APPname %s   \n",ResultName);
            if(strcmp(AppName,ResultName)==0)
                return TRUE;
        }
    }
    return FALSE;
}


void taskmng_net_topology_seq_reset()
{
    pthread_mutex_lock(&NetTopologySeqLock);
    NetTopologySeq = 1;
    pthread_mutex_unlock(&NetTopologySeqLock);
}

uint16 taskmng_net_topology_seq_get()
{
    uint16 ret;
    pthread_mutex_lock(&NetTopologySeqLock);
    ret = NetTopologySeq++;
    pthread_mutex_unlock(&NetTopologySeqLock);
    return ret;
}



int GetMsindexFromAddr(uint8 pipe,uint8 *Addr)
{
    uint16 Pn = taskmng_pn_get_from_addr(Addr,LOGIC_ID_YC);
    if(Pn != 0 )
    {
        return taskmng_ms_index_get(pipe, Pn,LOGIC_ID_YC);
    }
    return -1;
}

/*******************************************************************************
* ��������: SaveEventPowerOnOff
* ��������://�洢ͣ�ϵ��¼�
* �������:EventType 1 ͣ�� 0�ϵ�
* �������:
* �� �� ֵ:
*******************************************************************************/
void SaveEventPowerOnOff(uint8 pipe,uint8* Addr,uint8 EventType)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    OOP_OAD_U pipeoad = taskmng_pipe_oad_get(pipe);
    int msindex = -1;
    time_t powerofftime;
    time_t timenow = time(NULL);
    uint32 Eventseq = 0;

    uint8 rptBuff[2000] = {0x60, 0x12, 0x03, 0x00, 0x05, 0x00, 0x20, 0x2A, 0x02, 0x00, 0x00,
        0x60, 0x40, 0x02, 0x00, 0x00, 0x60, 0x41, 0x02, 0x00, 0x00, 0x60, 0x42, 0x02, 0x00,
        0x01, 0x30, 0x11, 0x02, 0x00, 0x05, 0x20, 0x1E, 0x02, 0x00, 0x20, 0x20, 0x02, 0x00,
        0x20, 0x22, 0x02, 0x00,0x20, 0x24, 0x02, 0x00,0x33, 0x00, 0x02, 0x00,};
    uint16 rptLen = 51;
    rptBuff[rptLen++] = 1;  //�������
    rptBuff[rptLen++] = 1;  //
    rptBuff[rptLen++] = DT_TSA;
    rptBuff[rptLen++] = 7;
    rptBuff[rptLen++] = 5;
    memcpy_r(&rptBuff[rptLen],Addr,6);
    rptLen+=6;
    msindex = GetMsindexFromAddr(pipe,Addr);
    if(msindex==-1)
    {
        TASK_FMT_DEBUG(pipe, RELAY_LOG, "�õ���ڵ����� �����д洢  \n");
        return;
    }
    rptBuff[rptLen++] = DT_DATETIME_S;
    DT_Time2DateTimeHex(timenow,(DateTimeHex_t *)&rptBuff[rptLen]);
    rptLen+=7;
    rptBuff[rptLen++] = DT_DATETIME_S;
    DT_Time2DateTimeHex(timenow,(DateTimeHex_t *)&rptBuff[rptLen]);
    rptLen+=7;
    rptBuff[rptLen++] = DT_DATETIME_S;
    DT_Time2DateTimeHex(timenow,(DateTimeHex_t *)&rptBuff[rptLen]);
    rptLen+=7;

    rptBuff[rptLen++] = 1;
    rptBuff[rptLen++] = 5;
    if(EventType==1)
    {
        rptBuff[rptLen++] = DT_DATETIME_S;
        DT_Time2DateTimeHex(timenow,(DateTimeHex_t *)&rptBuff[rptLen]);
        rptLen+=7;
        //��ͣ��ʱ��д���ڴ�
        if(pAmrThreadArg->pMtrAmrInfo[msindex].powerstate == 1)
        {
            TASK_FMT_DEBUG(pipe, RELAY_LOG, "�õ���ظ��ϱ�ͣ���¼� ֱ�ӹ��� \n");
            return;
        }
        pAmrThreadArg->pMtrAmrInfo[msindex].powerofftime = timenow;

        pAmrThreadArg->pMtrAmrInfo[msindex].powerstate = 1;
        Eventseq = pAmrThreadArg->pMtrAmrInfo[msindex].powereventseq;
        TASK_FMT_DEBUG(pipe, RELAY_LOG, "ͣ���¼���� %d \n",Eventseq);
        //ͣ�� �¼�����ʱ��ΪNULL
        rptBuff[rptLen++] = DT_NULL;
    }else
    {
        //ֻ��ͣ����ű��ϵ��¼�
        if(pAmrThreadArg->pMtrAmrInfo[msindex].powerstate!=1)
        {
            TASK_FMT_DEBUG(pipe, RELAY_LOG, "�õ��û��ͣ���¼� ���ϱ�  \n");
            return;
        }
        powerofftime = pAmrThreadArg->pMtrAmrInfo[msindex].powerofftime;
        Eventseq = pAmrThreadArg->pMtrAmrInfo[msindex].powereventseq;
        TASK_FMT_DEBUG(pipe, RELAY_LOG, "�ϵ��¼���� %d \n",Eventseq);
        pAmrThreadArg->pMtrAmrInfo[msindex].powerstate = 0;
        pAmrThreadArg->pMtrAmrInfo[msindex].powereventseq++;
        //����ϵ��¼��ġ�����ʱ�䡱ӦΪ����ͣ��ʱ�䣬����ն�û���յ��õ���ͣ���¼����򡰷���ʱ�䡱ӦΪ��
        if(powerofftime==0)
        {
            rptBuff[rptLen++] = DT_NULL;
        }else
        {

            rptBuff[rptLen++] = DT_DATETIME_S;
            DT_Time2DateTimeHex(powerofftime,(DateTimeHex_t *)&rptBuff[rptLen]);
            rptLen+=7;
        }
        //�ϵ� �¼�����ʱ����Ч
        rptBuff[rptLen++] = DT_DATETIME_S;
        DT_Time2DateTimeHex(timenow,(DateTimeHex_t *)&rptBuff[rptLen]);
        rptLen+=7;
    }
    rptBuff[rptLen++] = DT_DOUBLE_LONG_UNSIGNED;
    memcpy_r(&rptBuff[rptLen],(uint8*)&Eventseq,4);
    rptLen+=4;
    //�¼�����Դ
    rptBuff[rptLen++] = DT_OAD;
    memcpy_r(&rptBuff[rptLen],&pipeoad.value,4);
    rptLen+=4;
    //�¼��ϱ�״̬
    rptBuff[rptLen++] = DT_ARRAY;
    rptBuff[rptLen++] =1;
    rptBuff[rptLen++] = DT_STRUCTURE;
    rptBuff[rptLen++] = 2;
    rptBuff[rptLen++] = DT_OAD;
    memcpy_r(&rptBuff[rptLen],&pipeoad.value,4);
    rptLen+=4;
    rptBuff[rptLen++] = DT_UNSIGNED;
    if(EventType==1)
    {
        rptBuff[rptLen++] = 1;
    }else
    {
        rptBuff[rptLen++] = 5;
    }

    taskmng_event_report_data_write(pipe,rptBuff,rptLen,PowerOnOffOad);

}



/*******************************************************************************
* ��������: taskmng_search_meter_clean_load
* ��������://���������ļ���60020800 �ѱ�����־
* �������:
* �������:
* �� �� ֵ:
*******************************************************************************/
void taskmng_search_meter_clean_load()
{
    int   ret;
    uint32 Len;

    NOMAL_OAD_T NormalOad;
    NormalOad.logicId = 0;
    NormalOad.oad.value = 0x60020800;
    NormalOad.infoNum = 0;
    OOP_SEARCHPARAM_T clear;
    memset(&clear,0x00,sizeof(OOP_SEARCHPARAM_T));
    pthread_mutex_lock(&SearchMeterClean.lock);
    ret = db_read_nomal(gDBClientMain,&NormalOad,sizeof(OOP_SEARCHPARAM_T),(uint8*)&clear,&Len);
    if(ret!=0 || Len != sizeof(OOP_SEARCHPARAM_T))
    {
        TASK_FMT_TRACE(-1,UP_LOG, "����60020800ʧ�� ret %d  \n",ret);
        SearchMeterClean.isvalid = 0;
    }else
    {
        SearchMeterClean.isvalid = 1;
        memcpy(&SearchMeterClean.searchparam,&clear,sizeof(OOP_SEARCHPARAM_T));
    }
    pthread_mutex_unlock(&SearchMeterClean.lock);
}

/*******************************************************************************
* ��������: taskmng_search_meter_param_load
* ��������://���������ļ���60020900 �����ѱ��������
* �������:
* �������:
* �� �� ֵ:
*******************************************************************************/
void taskmng_search_meter_param_load(void)
{
    int   ret;
    uint32 Len;

    NOMAL_OAD_T NormalOad;
    NormalOad.logicId = 0;
    NormalOad.oad.value = 0x60020900;
    NormalOad.infoNum = 0;
    OOP_TIMINGSEARCH_T TimingSearch;
    memset(&TimingSearch,0x00,sizeof(OOP_TIMINGSEARCH_T));
    pthread_mutex_lock(&SearchMeterParam.Lock);
    ret = db_read_nomal(gDBClientMain,&NormalOad,sizeof(OOP_TIMINGSEARCH_T),(uint8*)&TimingSearch,&Len);
    if(ret!=0 || Len != sizeof(OOP_TIMINGSEARCH_T))
    {
        TASK_FMT_TRACE(-1,UP_LOG, "���������ѱ����ʧ�� ret %d  \n",ret);
        SearchMeterParam.isvalid = 0;
    }else
    {
        SearchMeterParam.isvalid = 1;
        memcpy(&SearchMeterParam.Param,&TimingSearch,sizeof(OOP_TIMINGSEARCH_T));
    }
    pthread_mutex_unlock(&SearchMeterParam.Lock);
}
#if (defined AREA_JIANGXI) || (defined AREA_ANHUI)||(defined AREA_JIANGSU)

/*******************************************************************************
* ��������: Loadtaskmng_perception_tmn_info_load
* ��������://���������ļ���60020900 �����ѱ��������
* �������:
* �������:
* �� �� ֵ:
*******************************************************************************/
void taskmng_perception_tmn_info_load()
{
    int   ret;
    uint32 Len;
    uint16 i;

    NOMAL_OAD_T NormalOad;
    NormalOad.logicId = 0;
    NormalOad.oad.value = 0xFF110E00;
    NormalOad.infoNum = 1;
    OOP_PERCEPTION_TERMINAL_INFO_T terminalinfo;
    uint8 num=0;

    memset((uint8*)&perceptionterminal,0x00,sizeof(OOP_PERCEPTION_TERMINAL_LIST_T));

    if(PerceptionTerminalInfoNum<1)
    {
        return;
    }
    for(i=0;i<PerceptionTerminalInfoNum;i++)
    {
        NormalOad.infoNum = i+1;
        ret = db_read_nomal(gDBClientMain,&NormalOad,sizeof(terminalinfo),(uint8*)&terminalinfo,&Len);
        if(ret!=0 || Len != sizeof(OOP_PERCEPTION_TERMINAL_INFO_T))
        {
            continue;
        }
        memcpy((uint8*)&perceptionterminal.terminalinfo[i],(uint8*)&terminalinfo,sizeof(terminalinfo));
    }
    perceptionterminal.num = PerceptionTerminalInfoNum;
    num = perceptionterminal.terminalinfo[PerceptionTerminalInfoNum-1].meterlist.num;
    if(num>=1)
    {
        perceptionterminal.totalpn = perceptionterminal.terminalinfo[PerceptionTerminalInfoNum-1].meterlist.meterinfo[num-1].pn;
        perceptionterminal.totalpn++;
    }
    return;
}

#endif
/*******************************************************************************
* ��������: SearchMeterCheck
* ��������:����Ƿ���������ѱ�
* �������:
* �������:
* �� �� ֵ:
*******************************************************************************/
void SearchMeterCheck(uint8 pipe)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    uint8 i;
    OOP_TIMINGSEARPARAM_T param;
    pthread_mutex_lock(&SearchMeterParam.Lock);
    if(SearchMeterParam.isvalid == 0)
    {
        pthread_mutex_unlock(&SearchMeterParam.Lock);
        return;
    }
    for(i=0;i<SearchMeterParam.Param.nNum;i++)
    {
        memset(&param,0x00,sizeof(OOP_TIMINGSEARPARAM_T));
        memcpy(&param,&SearchMeterParam.Param.period[i],sizeof(OOP_TIMINGSEARPARAM_T));
        if(param.start.hour ==gCurBinTime[pipe].Hour && param.start.minute ==gCurBinTime[pipe].Minute
            && param.start.second ==gCurBinTime[pipe].Second )
        {
            TASK_FMT_DEBUG(pipe, RELAY_LOG, "���������ѱ�ʱ�� hour %d minute %d second %d   \n",param.start.hour,
                param.start.minute,param.start.second);
            if(pAmrThreadArg->MeterSearch.IsInSearch==0)
            {
                pAmrThreadArg->IsNeedSearch = 2;
                pAmrThreadArg->MeterSearch.SearchTime = param.time;
                pthread_mutex_unlock(&SearchMeterParam.Lock);
                return;
            }
        }
        //��װ���ɲ���ʱ���ʱ���ѱ��м��ʱ�� ���е��ټӸ��ж�
    }
    pthread_mutex_unlock(&SearchMeterParam.Lock);
    return;
}


/*******************************************************************************
* ��������: DBInitFun
* ��������://������������ ���ݳ�ʼ���¼�
* �������:
* �������:
* �� �� ֵ:
*******************************************************************************/
void DBInitFun(uint8* buf,uint16 len)
{
    uint16 index = 0;
    uint8 i;
    uint8 choice;
    uint8 flag = 0;
    uint8 logicid;
    uint8 optional;
    uint16 dataclass;
    DB_UPDATE_T dbupdate;
    memset(&dbupdate,0x00,sizeof(DB_UPDATE_T));
    choice = buf[index++];
    TASK_FMT_DEBUG(-1,MQTT_LOG,"�յ����ݳ�ʼ���¼� \n");

    if(choice ==0 )
    {
        logicid = buf[index++];
        if(logicid!=0)
            return;
        optional = buf[index++];
        if(optional!=0)
        {
            memcpy_r((uint8*)&dataclass,&buf[index],2);
            index+=2;
            if(dataclass == 2)
                flag = 1;
            else if(dataclass ==3)
            {
                taskmng_meter_search_result_clear();
                taskmng_across_area_result_clear();
            }
        }else
        {
            flag = 2;
            taskmng_meter_search_result_clear();
            taskmng_across_area_result_clear();
        }
    }else
    {
        flag = 1;
    }
    //�ָ��������� �����������ʶΪ2�����ݳ�ʼ���Ŵ���
    if(flag == 1)
    {
        //ֱ�ӷ��͵�����ʼ�� ��ͬ�����������ʼ��
        dbupdate.logicid = 0xff;    //�˴�ʹ��0xff��ʾ�����߼��豸�ž�ɾ��
        dbupdate.infonum = 0xffff;
        dbupdate.oad.value = 0x60000200;
        task_send_msg(gMqttSock,MSG_DBUPDATE,(uint8*)&dbupdate,sizeof(DB_UPDATE_T),IPC_MAIN);

    }
    if(flag == 2)
    {
        for(i=0;i<AMR_THREAD_NUM+1;i++)
        {
            DataInitFlag[i]=1;
        }
        DataInitFlagMain = 1;
        ReloadTaskFlag = 1;
    }


}




/*******************************************************************************
* ��������: taskmng_sync_time_param_load
* ��������:���������Ļ�ȡ�㲥Уʱ��ز���
* �������:
* �������:
* �� �� ֵ:
*******************************************************************************/
void taskmng_sync_time_param_load()
{
    int   ret;
    uint32 Len;

    NOMAL_OAD_T NormalOad;
    NormalOad.logicId = 0;
    NormalOad.oad.value = 0x42040200;
    NormalOad.infoNum = 0;
    OOP_SYNCTIME_T SyncTime;
    memset(&SyncTime,0x00,sizeof(OOP_SYNCTIME_T));
    OOP_P2PSYNCTIME_T P2PSyncTime;
    memset(&P2PSyncTime,0x00,sizeof(OOP_P2PSYNCTIME_T));
    pthread_mutex_lock(&SyncTimingParam.Lock);
    ret = db_read_nomal(gDBClientMain,&NormalOad,sizeof(OOP_SYNCTIME_T),(uint8*)&SyncTime,&Len);
    if(ret!=0 || Len != sizeof(OOP_SYNCTIME_T))
    {
        TASK_FMT_DEBUG(-1,UP_LOG, "���ع㲥Уʱ����ʧ�� ret %d  \n",ret);
        SyncTimingParam.SyncTime.bValid = 0;
    }else
    {
        //240712 24�淶��УʱҪ�ܿ���������� ���������5������ �͸ĳ�5����
        if(SyncTime.time.minute < 5)
        {
            TASK_FMT_DEBUG(-1,UP_LOG, "�㲥Уʱ�����Ż� ԭ�� hour %d min %d sec %d  \n",SyncTime.time.hour,SyncTime.time.minute,SyncTime.time.second);
            SyncTime.time.minute = 5;
            SyncTime.time.second = 0;
        }
        if(SyncTime.time.minute >= 55)
        {
            TASK_FMT_DEBUG(-1,UP_LOG, "�㲥Уʱ�����Ż� ԭ�� hour %d min %d sec %d  \n",SyncTime.time.hour,SyncTime.time.minute,SyncTime.time.second);
            SyncTime.time.minute = 5;
            SyncTime.time.second = 0;
            if(SyncTime.time.hour >= 23)
            {
                SyncTime.time.hour = 0;
            }else
            {
                SyncTime.time.hour++;
            }
        }
        memcpy(&SyncTimingParam.SyncTime,&SyncTime,sizeof(OOP_SYNCTIME_T));
        TASK_FMT_DEBUG(-1,UP_LOG, "0x42040200�㲥Уʱ���� hour %d min %d sec %d  valid %d \n",SyncTime.time.hour,
            SyncTime.time.minute,SyncTime.time.second,SyncTime.bValid);
    }

    NormalOad.oad.value = 0x42040300;

    ret = db_read_nomal(gDBClientMain,&NormalOad,sizeof(OOP_P2PSYNCTIME_T),(uint8*)&P2PSyncTime,&Len);
    if(ret!=0 || Len != sizeof(OOP_P2PSYNCTIME_T))
    {
        TASK_FMT_DEBUG(-1,UP_LOG, "���ص���ַ�㲥Уʱ����ʧ�� ret %d  \n",ret);
        SyncTimingParam.P2PSycnTime.bValid = 0;
    }else
    {
        if(P2PSyncTime.time.hour == 0xFF)
        {
            SyncTimingParam.P2PSycnTime.bValid = 0;
            TASK_FMT_DEBUG(-1,UP_LOG, "����ַ�㲥Уʱ�����Ż�ʱ��ΪFF��Ϊ������Ч ԭ�� hour %d min %d sec %d  \n",P2PSyncTime.time.hour,P2PSyncTime.time.minute,P2PSyncTime.time.second);
        }
         //240712 24�淶��УʱҪ�ܿ���������� ���������5������ �͸ĳ�5����
        if(P2PSyncTime.time.minute < 5)
        {
            TASK_FMT_DEBUG(-1,UP_LOG, "����ַ�㲥Уʱ�����Ż� ԭ�� hour %d min %d sec %d  \n",P2PSyncTime.time.hour,P2PSyncTime.time.minute,P2PSyncTime.time.second);
            P2PSyncTime.time.minute = 5;
            P2PSyncTime.time.second = 0;
        }
        if(P2PSyncTime.time.minute >= 55)
        {
            TASK_FMT_DEBUG(-1,UP_LOG, "����ַ�㲥Уʱ�����Ż� ԭ�� hour %d min %d sec %d  \n",P2PSyncTime.time.hour,P2PSyncTime.time.minute,P2PSyncTime.time.second);
            P2PSyncTime.time.minute = 5;
            P2PSyncTime.time.second = 0;
            if(P2PSyncTime.time.hour >= 23)
            {
                P2PSyncTime.time.hour = 0;
            }else
            {
                P2PSyncTime.time.hour++;
            }
        }
        memcpy(&SyncTimingParam.P2PSycnTime,&P2PSyncTime,sizeof(OOP_P2PSYNCTIME_T));
        TASK_FMT_DEBUG(-1,UP_LOG, "0x42040300����ַ�㲥Уʱ���� hour %d min %d sec %d  valid %d offset %d \n",P2PSyncTime.time.hour,
            P2PSyncTime.time.minute,P2PSyncTime.time.second,P2PSyncTime.bValid,P2PSyncTime.offset);
    }
    pthread_mutex_unlock(&SyncTimingParam.Lock);
}

BOOL CheckBroadTimingisValid(uint8 type)
{
    BOOL ret = FALSE;
    pthread_mutex_lock(&SyncTimingParam.Lock);
    if(type==1)
    {
        if(SyncTimingParam.SyncTime.bValid == 1)
            ret = TRUE;
    }else
    {
        if(SyncTimingParam.P2PSycnTime.bValid ==1)
            ret =TRUE;
    }

    pthread_mutex_unlock(&SyncTimingParam.Lock);
    return ret;
}

/*******************************************************************************
* ��������: SyncTimingCheck
* ��������://����Ƿ���ڶ�ʱ�㲥Уʱ����
* �������:
* �������:
* �� �� ֵ:
*******************************************************************************/
void SyncTimingCheck(uint8 pipe)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    if(pAmrThreadArg->BroadTiming.TimingType !=0)
    {
        return;
    }
    pthread_mutex_lock(&SyncTimingParam.Lock);
    if(SyncTimingParam.SyncTime.bValid == 1)
    {

        if(SyncTimingParam.SyncTime.time.hour ==gCurBinTime[pipe].Hour &&SyncTimingParam.SyncTime.time.minute ==gCurBinTime[pipe].Minute
            &&(SyncTimingParam.SyncTime.time.second <=gCurBinTime[pipe].Second || pipe >=PLC_NUM)&&(pAmrThreadArg->Isadjusttime==0))
        {
            TASK_FMT_DEBUG(pipe, RELAY_LOG, "���˹㲥Уʱʱ�� hour %d minute %d second %d   \n",gCurBinTime[pipe].Hour,
                gCurBinTime[pipe].Minute,gCurBinTime[pipe].Second);
            pAmrThreadArg->BroadTiming.TimingType = 1;
            pAmrThreadArg->Isadjusttime=1;
            taskmng_plc_task_queue_add( pipe, MS_BROAD_TIMING);
        }
    }
    if(SyncTimingParam.P2PSycnTime.bValid ==1)
    {
        if(SyncTimingParam.P2PSycnTime.time.hour ==gCurBinTime[pipe].Hour &&SyncTimingParam.P2PSycnTime.time.minute ==gCurBinTime[pipe].Minute
            &&(SyncTimingParam.P2PSycnTime.time.second <=gCurBinTime[pipe].Second || pipe >=PLC_NUM)&&(pAmrThreadArg->Isadjusttime==0))
        {
            TASK_FMT_DEBUG(pipe, RELAY_LOG, "���˵���ַ�㲥Уʱʱ�� hour %d minute %d second %d   \n",gCurBinTime[pipe].Hour,
                gCurBinTime[pipe].Minute,gCurBinTime[pipe].Second);
            pAmrThreadArg->BroadTiming.TimingType = 2;
            pAmrThreadArg->Isadjusttime=1;
            pAmrThreadArg->BroadTiming.limit = SyncTimingParam.P2PSycnTime.offset;
            taskmng_plc_task_queue_add( pipe, MS_BROAD_TIMING);
        }
    }
    pthread_mutex_unlock(&SyncTimingParam.Lock);
}


/*******************************************************************************
* ��������: BroadTiming
* ��������://�㲥��ʱ
* �������:
* �������:
* �� �� ֵ:
*******************************************************************************/
int BroadTiming(uint8 pipe)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    static time_t  SendTime[PLC_NUM];
    uint8 RecvBuf[1000]={0};
    int RecvLen;
    int ret;
    MESSAGE_INFO_T* MsgInfo =NULL;
    if(CheckBroadTimingisValid(pAmrThreadArg->BroadTiming.TimingType)==FALSE)
    {
        TASK_FMT_DEBUG(pipe, RELAY_LOG, "�㲥Уʱ������Ч �������\n");
        pAmrThreadArg->TotalState.SyncTimeState = INNER_RTS_IDLE;
        pAmrThreadArg->BroadTiming.TimingType=0;
        return 1;
    }
    switch(pAmrThreadArg->TotalState.SyncTimeState)
    {
    //��ʼ̬
    case INNER_RTS_IDLE:
    {
        pAmrThreadArg->BroadTiming.MtrIndex = 0;
        pAmrThreadArg->BroadTiming.state = TMN_TO_METER_IDLE;
        pAmrThreadArg->BroadTiming.FailCnt = 0;
        TASK_FMT_DEBUG(pipe, RELAY_LOG, "�㲥��ʱ�����ʼ�� timingtype %d\n",pAmrThreadArg->BroadTiming.TimingType);
        if(pAmrThreadArg->BroadTiming.TimingType == 1)
        {
            pAmrThreadArg->BroadTiming.state = TMN_TO_METER_GET_DELAY;
            pAmrThreadArg->BroadTiming.BroadPrtl = PRTL_97;//��Լ1 2 3 ��ѯ
        }else if(pAmrThreadArg->BroadTiming.TimingType == 2)
        {
            pAmrThreadArg->BroadTiming.state = TMN_TO_METER_RDINIT_FIRST;
        }
        else
            return 1;
        pAmrThreadArg->TotalState.SyncTimeState = INNER_RTS_SEND;
    }
        break;
    case INNER_RTS_SEND:
    {
        ret =BroadTimingSend(pipe);
        if(ret ==1)
        {
            SendTime[pipe] = time(NULL);
            pAmrThreadArg->TotalState.SyncTimeState = INNER_RTS_RECV;
        }else if(ret==0)
        {
            TASK_FMT_DEBUG(pipe, RELAY_LOG, "�㲥Уʱ�������  \n");
            pAmrThreadArg->TotalState.SyncTimeState = INNER_RTS_IDLE;
            pAmrThreadArg->BroadTiming.TimingType = 0;
            return 1;
        }
    }
        break;
    case INNER_RTS_RECV:
    {
        RecvLen = taskmng_plc_queue_read ( pAmrRecvDown[pipe], RecvBuf );
        if(RecvLen>0)
        {
            MsgInfo = (MESSAGE_INFO_T*)RecvBuf;

            TASK_FMT_DEBUG( pipe, RELAY_LOG, "�㲥Уʱ�յ���Ϣ IID %04X IOP %04X MsgLen %d payload \n",MsgInfo->IID,MsgInfo->IOP,MsgInfo->msglen);
            TASK_FMT_DEBUG( pipe, RELAY_LOG,"recvindex %d  Sendindex %d \n",MsgInfo->index,pAmrThreadArg->BroadTiming.FrameIndex );
            if(MsgInfo->index!=pAmrThreadArg->BroadTiming.FrameIndex)
            {
                break;
            }
            if(MsgInfo->msglen ==0||(pAmrThreadArg->BroadTiming.state==TMN_TO_METER_RDINIT_FIRST &&MsgInfo->msglen <=1))
            {
                TASK_FMT_DEBUG(pipe, RELAY_LOG, "MsgInfo->MsgLen is %d\n",MsgInfo->msglen);
                pAmrThreadArg->TotalState.SyncTimeState = INNER_RTS_RETRY;
                break;
            }
            TASK_BUF_DEBUG( pipe, RELAY_LOG,MsgInfo->playload, MsgInfo->msglen);
            BroadTimingRecv(pipe,MsgInfo->playload, MsgInfo->msglen);
            pAmrThreadArg->TotalState.SyncTimeState = INNER_RTS_SEND;
        }
        else
        {
            if(time(NULL)-SendTime[pipe]>BROAD_TIMING_OVERTIME)
            {
                TASK_FMT_DEBUG(pipe, RELAY_LOG, "�㲥Уʱ%d�볬ʱ  \n",BROAD_TIMING_OVERTIME);
                pAmrThreadArg->TotalState.SyncTimeState = INNER_RTS_RETRY;
            }
        }
    }
        break;
    case INNER_RTS_RETRY:
    {
        pAmrThreadArg->BroadTiming.FailCnt++;
        if(pAmrThreadArg->BroadTiming.TimingType == 1)
        {
            if(pAmrThreadArg->BroadTiming.FailCnt>=3)
            {
                TASK_FMT_DEBUG(pipe, RELAY_LOG, "�㲥Уʱʧ�ܴ�������3�� ֱ���������  \n");
                pAmrThreadArg->TotalState.SyncTimeState = INNER_RTS_IDLE;
                pAmrThreadArg->BroadTiming.TimingType = 0;
                return 1;
            }
        }
        else
        {
            if(pAmrThreadArg->BroadTiming.FailCnt>3)
            {
                TASK_FMT_DEBUG(pipe, RELAY_LOG, "����ַ�㲥Уʱʧ�ܴ�������3�� ת��һ����� \n");
                pAmrThreadArg->BroadTiming.FailCnt = 0;
                pAmrThreadArg->BroadTiming.MtrIndex++;
                pAmrThreadArg->BroadTiming.state = TMN_TO_METER_RDINIT_FIRST;
            }
        }
        pAmrThreadArg->TotalState.SyncTimeState = INNER_RTS_SEND;
    }
        break;
    default:
        break;
    }
    return -1;
}


/**
*********************************************************************
* @brief:      ʵ��  485�㲥Уʱ�͵���Уʱ�Ĺ��ܣ�
* @param[in]��   pipe  �̺߳�,485ģ�鹲8���߳�,485���߳�pipe��2��ʼ��9
* @return��1 �㲥Уʱִ�гɹ� -1 �㲥Уʱִ��ʧ��
*********************************************************************
*/
int Rs485BroadTiming(uint8 pipe)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
//    static time_t  SendTime[PLC_NUM];
    uint8 RecvBuf[3000]={0};
    int RecvLen;
    int ret;

    pAmrThreadArg->TotalState.SyncTimeState = INNER_RTS_IDLE;
    while(1)
    {
        switch(pAmrThreadArg->TotalState.SyncTimeState)
        {
        //��ʼ̬
        case INNER_RTS_IDLE:
        {
            TASK_FMT_DEBUG(pipe, RELAY_LOG, "pAmrThreadArg->BroadTiming.TimingType is %d\n",pAmrThreadArg->BroadTiming.TimingType);
            //���û�е��� �Ͳ��㲥Уʱ
            if(pAmrThreadArg->TmnAmrInfo.MtrCnt == 0)
            {
                TASK_FMT_DEBUG(pipe, RELAY_LOG, "��ǰ�˿�û�е��� ���㲥Уʱ\n");
                return 1;
            }
            pAmrThreadArg->BroadTiming.MtrIndex = 0;
            pAmrThreadArg->BroadTiming.state = TMN_TO_METER_IDLE;
            pAmrThreadArg->BroadTiming.FailCnt = 0;
            if(pAmrThreadArg->BroadTiming.TimingType == 1)
            {
                pAmrThreadArg->BroadTiming.state = TMN_TO_METER_SETTIME_INIT;
                pAmrThreadArg->BroadTiming.BroadPrtl = PRTL_07;//��Լ1 2 3 ��ѯ
            }else if(pAmrThreadArg->BroadTiming.TimingType == 2)
            {
                pAmrThreadArg->BroadTiming.state = TMN_TO_METER_RDINIT_FIRST;
            }
            else
                return 1;
            pAmrThreadArg->TotalState.SyncTimeState = INNER_RTS_SEND;
        }
            break;
        case INNER_RTS_SEND:
        {
            ret =Rs485BroadTimingSend(pipe);
            if(ret ==1)
            {
                //SendTime[pipe] = time(NULL);
                pAmrThreadArg->TotalState.SyncTimeState = INNER_RTS_RECV;
            }else if(ret==0)
            {
                TASK_FMT_DEBUG(pipe, RELAY_LOG, "�㲥Уʱ�������  \n");
                pAmrThreadArg->TotalState.SyncTimeState = INNER_RTS_IDLE;
                pAmrThreadArg->BroadTiming.TimingType = 0;
                //rs485_RS232_UnInit(&Rs485ComNo[pipe-2]);
                return 1;
            }
        }
            break;
        case INNER_RTS_RECV:
        {
            RecvLen = rs485_RS232_recv(pipe,RecvBuf, sizeof(RecvBuf));
            if(RecvLen>0)
            {
                TASK_FMT_DEBUG(pipe, RELAY_LOG, "�յ���Ϣ \n");
                TASK_BUF_DEBUG( pipe, RELAY_LOG,RecvBuf, RecvLen);
                Rs485BroadTimingRecv(pipe,RecvBuf, RecvLen);
                pAmrThreadArg->TotalState.SyncTimeState = INNER_RTS_SEND;
            }
            else
            {
                if(pAmrThreadArg->BroadTiming.TimingType == 1)
                {
                    Rs485BroadTimingRecv(pipe,RecvBuf, RecvLen);
//                    pAmrThreadArg->BroadTiming.BroadPrtl++;
//                    pAmrThreadArg->BroadTiming.state = TMN_TO_METER_SETTIME_INIT;
                    pAmrThreadArg->TotalState.SyncTimeState = INNER_RTS_SEND;
                }
                else
                {
                    TASK_FMT_DEBUG(pipe, RELAY_LOG, "�㲥Уʱ%d�볬ʱ  \n",BROAD_TIMING_OVERTIME);
                    pAmrThreadArg->TotalState.SyncTimeState = INNER_RTS_RETRY;
                }
            }
        }
            break;
        case INNER_RTS_RETRY:
        {
            pAmrThreadArg->BroadTiming.FailCnt++;

            if(pAmrThreadArg->BroadTiming.FailCnt>3)
            {
                TASK_FMT_DEBUG(pipe, RELAY_LOG, "����ַ�㲥Уʱʧ�ܴ�������3�� ת��һ����� \n");
                pAmrThreadArg->BroadTiming.FailCnt = 0;
                pAmrThreadArg->BroadTiming.MtrIndex++;
                pAmrThreadArg->BroadTiming.state = TMN_TO_METER_RDINIT_FIRST;
            }

            pAmrThreadArg->TotalState.SyncTimeState = INNER_RTS_SEND;
        }
            break;
        default:
            break;
        }
    }
    return -1;
}
/*******************************************************************************
* ��������: BroadTimingRecv
* ��������://�㲥Уʱ�յ����봦��
* �������:
* �������:
* �� �� ֵ:
*******************************************************************************/
void BroadTimingRecv(uint8 pipe,uint8* buf,uint16 len)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    switch(pAmrThreadArg->BroadTiming.state)
    {
    //�յ��㲥��ʱ
    case TMN_TO_METER_GET_DELAY:
    {
        if(len!=2)
            return;
        if(pAmrThreadArg->BroadTiming.TimingType == 1)
        {
            memcpy_r((uint8*)&pAmrThreadArg->BroadTiming.BroadDelay,buf,2);
            TASK_FMT_DEBUG(pipe, RELAY_LOG, "��ȡ���㲥��ʱ %d  \n",pAmrThreadArg->BroadTiming.BroadDelay);
        }else
        {
            uint16 MtrIndex = pAmrThreadArg->BroadTiming.MtrIndex;
            memcpy_r((uint8*)&pAmrThreadArg->pMtrAmrInfo[MtrIndex].TimeDelay,buf,2);
            TASK_FMT_DEBUG(pipe, RELAY_LOG, "��ȡ�ӽڵ�����ʱ %d  \n",pAmrThreadArg->pMtrAmrInfo[MtrIndex].TimeDelay);
        }
        pAmrThreadArg->BroadTiming.state = TMN_TO_METER_SETTIME_INIT;
    }
        break;
    //�յ��㲥�Ļ���
    case TMN_TO_METER_SETTIME_INIT:
    {
        if(pAmrThreadArg->BroadTiming.TimingType == 1)
        {
            if(buf[0]==1)
            {
                //�������㲥���������û���������� ����о���Ҫ���߼���ַ1�ٹ㲥һ��
                if(pAmrThreadArg->BroadTiming.BroadPrtl == PRTL_OOP)
                {
                    if(TRUE == taskmng_iot_prtl_check(pipe))
                    {
                        pAmrThreadArg->BroadTiming.BroadPrtl = PRTL_20_IOT;
                        TASK_FMT_DEBUG(pipe, RELAY_LOG, "��������㲥Уʱ \n");
                        //�����㲥�������ʱ
                        sleep(5);
                        break;
                    }
                }
                if(pAmrThreadArg->BroadTiming.BroadPrtl == PRTL_20_IOT)
                {
                    pAmrThreadArg->BroadTiming.state = TMN_TO_METER_SETTIME_CIPHER;
                    TASK_FMT_DEBUG(pipe, RELAY_LOG, "�����������Ĺ㲥Уʱ \n");
                    //�����㲥�������ʱ
                    sleep(5);
                    break;
                }
                //һ�ֹ�Լ�㲥��ʱ�����һ����Լ
                pAmrThreadArg->BroadTiming.BroadPrtl++;
                pAmrThreadArg->BroadTiming.state = TMN_TO_METER_GET_DELAY;
            }
        }
        else
        {
            SaveMeterTimingEvent(pipe);
            pAmrThreadArg->BroadTiming.MtrIndex++;
            pAmrThreadArg->BroadTiming.FailCnt = 0;
            pAmrThreadArg->BroadTiming.state = TMN_TO_METER_RDINIT_FIRST;
        }
    }
        break;
    case TMN_TO_METER_SETTIME_CIPHER:
    {
        if(buf[0]==1)
        {

            pAmrThreadArg->BroadTiming.BroadPrtl++;
            pAmrThreadArg->BroadTiming.state = TMN_TO_METER_GET_DELAY;

        }

    }
        break;
    //�յ���ѯʱ�ӻ���
    case TMN_TO_METER_RDINIT_FIRST:
    {
        uint16 datalen;
        uint8 lenoffset;
        datalen = XADR_GetEleNum(buf, &lenoffset);
        if(datalen<=1)
            break;
        SaveMeterTime(pipe,&buf[lenoffset],datalen);
    }
        break;
    default:
        break;
    }
}

/*******************************************************************************
* ��������: BroadTimingRecv
* ��������://�㲥Уʱ�յ����봦��
* �������:
* �������:
* �� �� ֵ:
*******************************************************************************/
void Rs485BroadTimingRecv(uint8 pipe,uint8* buf,uint16 len)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    switch(pAmrThreadArg->BroadTiming.state)
    {
//    //�յ��㲥��ʱ
//    case TMN_TO_METER_GET_DELAY:
//    {
//        if(len!=2)
//            return;
//        if(pAmrThreadArg->BroadTiming.TimingType == 1)
//        {
//            memcpy_r((uint8*)&pAmrThreadArg->BroadTiming.BroadDelay,buf,2);
//            TASK_FMT_DEBUG(pipe, RELAY_LOG, "��ȡ���㲥��ʱ %d  \n",pAmrThreadArg->BroadTiming.BroadDelay);
//        }else
//        {
//            uint16 MtrIndex = pAmrThreadArg->BroadTiming.MtrIndex;
//            memcpy_r((uint8*)&pAmrThreadArg->pMtrAmrInfo[MtrIndex].TimeDelay,buf,2);
//            TASK_FMT_DEBUG(pipe, RELAY_LOG, "��ȡ�ӽڵ�����ʱ %d  \n",pAmrThreadArg->pMtrAmrInfo[MtrIndex].TimeDelay);
//        }
//        pAmrThreadArg->BroadTiming.state = TMN_TO_METER_SETTIME_INIT;
//    }
//        break;
    //�յ��㲥�Ļ���
    case TMN_TO_METER_SETTIME_INIT:
    {
        if(pAmrThreadArg->BroadTiming.TimingType == 1)
        {
            //if(buf[0]==1)
            {
                //�������㲥���������û���������� ����о���Ҫ���߼���ַ1�ٹ㲥һ��
                if(pAmrThreadArg->BroadTiming.BroadPrtl == PRTL_OOP)
                {
                    if(TRUE == taskmng_iot_prtl_check(pipe))
                    {
                        pAmrThreadArg->BroadTiming.BroadPrtl = PRTL_20_IOT;
                        TASK_FMT_DEBUG(pipe, RELAY_LOG, "��������㲥Уʱ \n");
                        //�����㲥�������ʱ
                        sleep(5);
                        break;
                    }
                }
                if(pAmrThreadArg->BroadTiming.BroadPrtl == PRTL_20_IOT)
                {
                    pAmrThreadArg->BroadTiming.state = TMN_TO_METER_SETTIME_CIPHER;
                    TASK_FMT_DEBUG(pipe, RELAY_LOG, "�����������Ĺ㲥Уʱ \n");

                    break;
                }
                //һ�ֹ�Լ�㲥��ʱ�����һ����Լ
                pAmrThreadArg->BroadTiming.BroadPrtl++;
                pAmrThreadArg->BroadTiming.state = TMN_TO_METER_SETTIME_INIT;
            }
        }
        else
        {
            SaveMeterTimingEvent(pipe);
            pAmrThreadArg->BroadTiming.MtrIndex++;
            pAmrThreadArg->BroadTiming.FailCnt = 0;
            pAmrThreadArg->BroadTiming.state = TMN_TO_METER_RDINIT_FIRST;
        }
    }
        break;
    //�յ���ѯʱ�ӻ���
    case TMN_TO_METER_RDINIT_FIRST:
    {
//        uint16 datalen;
//        uint8 lenoffset;
//        datalen = XADR_GetEleNum(buf, &lenoffset);
//        if(datalen<=1)
//            break;
        SaveMeterTime(pipe,buf,len);
    }
        break;
    case TMN_TO_METER_SETTIME_CIPHER:
    {
        //if(buf[0]==1)
        {

            pAmrThreadArg->BroadTiming.BroadPrtl++;
            pAmrThreadArg->BroadTiming.state = TMN_TO_METER_SETTIME_INIT;

        }

    }
        break;
    default:
        break;
    }
}
/*******************************************************************************
* ��������: SaveMeterTimingEvent
* ��������://������ʱ��Ϣ��FFA00100��oadд����������
* �������:
* �������:
* �� �� ֵ:
*******************************************************************************/
 void SaveMeterTimingEvent(uint8 pipe)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    time_t timenow = time(NULL);

    OOP_CHK_TIMEINFO_T Chktimeinfo;
    memset(&Chktimeinfo,0x00,sizeof(OOP_CHK_TIMEINFO_T));

    uint16 MtrIndex = pAmrThreadArg->BroadTiming.MtrIndex;
    uint16 Pn = pAmrThreadArg->pMtrAmrInfo[MtrIndex].Pn;
    uint8 Addr[6]={0};
    taskmng_mtr_addr_get( Pn, pAmrThreadArg->pMtrAmrInfo[MtrIndex].logicid ,Addr);

    Chktimeinfo.add.nNum = 6;
    memcpy_r(Chktimeinfo.add.value,Addr,6);
    DT_Time2DateTimeShort(timenow,&Chktimeinfo.chkTime);
    memcpy(&Chktimeinfo.beforechkTime,&pAmrThreadArg->pMtrAmrInfo[MtrIndex].MeterTime,sizeof(OOP_DATETIME_S_T));
    Chktimeinfo.error = (int16)pAmrThreadArg->pMtrAmrInfo[MtrIndex].TimeError;

    taskmng_normal_data_write_pipe( pipe, (uint8*)&Chktimeinfo, sizeof(OOP_CHK_TIMEINFO_T), MtrTimingOad, Pn,pAmrThreadArg->pMtrAmrInfo[MtrIndex].logicid);

    TASK_FMT_DEBUG(pipe, RELAY_LOG, "д����Уʱ��Ϣ Pn %d \n",Pn);

}



/*******************************************************************************
* ��������: SaveMeterTime
* ��������://�յ���ѯʱ�ӻ�����д���
* �������:
* �������:
* �� �� ֵ:
*******************************************************************************/
void SaveMeterTime(uint8 pipe,uint8* buf,uint16 len)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    uint16 MtrIndex = pAmrThreadArg->BroadTiming.MtrIndex;
    uint16 Pn = pAmrThreadArg->pMtrAmrInfo[MtrIndex].Pn;
    uint8 logicid = pAmrThreadArg->pMtrAmrInfo[MtrIndex].logicid;
    uint8 prtl;
    taskmng_mtr_prtl_get( Pn,logicid,&prtl);
    int ret;
    time_t metertime;
    switch(prtl)
    {
    case PRTL_07:
    case PRTL_97:
    {
        MeterPrtlFmt_T MeterPrtlData;
        memset(&MeterPrtlData,0x00,sizeof(MeterPrtlFmt_T));
        ret = taskmng_645_meter_data_unpack(len,buf,MeterPrtlData.MtrAddr,&MeterPrtlData.CtrlCode,&MeterPrtlData.DataLen,MeterPrtlData.DataBuff);
        if(ret ==CHECK_RESULT_SUCCEED)
        {
            //645�ճ���������
            if(pAmrThreadArg->pMtrAmrInfo[MtrIndex].GetTimeIndex == 0)
            {
                if(prtl == PRTL_97)
                {
                    //memcpy(pAmrThreadArg->pMtrAmrInfo[MtrIndex].MeterTime,&MeterPrtlData.DataBuff[2],4);
                    pAmrThreadArg->pMtrAmrInfo[MtrIndex].MeterTime.year = commfun_BcdToHex(MeterPrtlData.DataBuff[5])+2000;
                    pAmrThreadArg->pMtrAmrInfo[MtrIndex].MeterTime.month = commfun_BcdToHex(MeterPrtlData.DataBuff[4]);
                    pAmrThreadArg->pMtrAmrInfo[MtrIndex].MeterTime.day = commfun_BcdToHex(MeterPrtlData.DataBuff[3]);
                }else
                {
                    //memcpy(pAmrThreadArg->pMtrAmrInfo[MtrIndex].MeterTime,&MeterPrtlData.DataBuff[4],4);
                    pAmrThreadArg->pMtrAmrInfo[MtrIndex].MeterTime.year = commfun_BcdToHex(MeterPrtlData.DataBuff[7])+2000;
                    pAmrThreadArg->pMtrAmrInfo[MtrIndex].MeterTime.month = commfun_BcdToHex(MeterPrtlData.DataBuff[6]);
                    pAmrThreadArg->pMtrAmrInfo[MtrIndex].MeterTime.day = commfun_BcdToHex(MeterPrtlData.DataBuff[5]);
                }
                pAmrThreadArg->pMtrAmrInfo[MtrIndex].GetTimeIndex++;
            }else
            {
                if(prtl == PRTL_97)
                {
                    //memcpy(&pAmrThreadArg->pMtrAmrInfo[MtrIndex].MeterTime[4],&MeterPrtlData.DataBuff[2],3);
                    pAmrThreadArg->pMtrAmrInfo[MtrIndex].MeterTime.hour = commfun_BcdToHex(MeterPrtlData.DataBuff[4]);
                    pAmrThreadArg->pMtrAmrInfo[MtrIndex].MeterTime.minute = commfun_BcdToHex(MeterPrtlData.DataBuff[3]);
                    pAmrThreadArg->pMtrAmrInfo[MtrIndex].MeterTime.second = commfun_BcdToHex(MeterPrtlData.DataBuff[2]);
                }else
                {
                    //memcpy(&pAmrThreadArg->pMtrAmrInfo[MtrIndex].MeterTime[4],&MeterPrtlData.DataBuff[4],3);
                    pAmrThreadArg->pMtrAmrInfo[MtrIndex].MeterTime.hour = commfun_BcdToHex(MeterPrtlData.DataBuff[6]);
                    pAmrThreadArg->pMtrAmrInfo[MtrIndex].MeterTime.minute = commfun_BcdToHex(MeterPrtlData.DataBuff[5]);
                    pAmrThreadArg->pMtrAmrInfo[MtrIndex].MeterTime.second = commfun_BcdToHex(MeterPrtlData.DataBuff[4]);
                 }

                DT_DateTimeShort2Time(&pAmrThreadArg->pMtrAmrInfo[MtrIndex].MeterTime,&metertime);
                TASK_FMT_DEBUG(pipe, RELAY_LOG, "��ǰ���ʱ�� time_t %d \n",metertime);
                TASK_BUF_DEBUG(pipe, RELAY_LOG,&pAmrThreadArg->pMtrAmrInfo[MtrIndex].MeterTime,7);
                pAmrThreadArg->pMtrAmrInfo[MtrIndex].TimeError = abs(time(NULL)-metertime);
                //��ȡ���ʱ������ ������ȡ�ӽڵ�����ʱ
                if(pAmrThreadArg->pMtrAmrInfo[MtrIndex].TimeError <pAmrThreadArg->BroadTiming.limit )
                {
                    TASK_FMT_DEBUG(pipe, RELAY_LOG, "��ǰ��� %d ���ն�ʱ�� %d С����ֵ %d ����Уʱ \n",Pn,
                        pAmrThreadArg->pMtrAmrInfo[MtrIndex].TimeError,pAmrThreadArg->BroadTiming.limit);
                    pAmrThreadArg->BroadTiming.MtrIndex++;
                    pAmrThreadArg->BroadTiming.FailCnt = 0;
                    pAmrThreadArg->BroadTiming.state =TMN_TO_METER_RDINIT_FIRST;
                }else
                {
                    TASK_FMT_DEBUG(pipe, RELAY_LOG, "��ǰ��� %d ���ն�ʱ�� %d ������ֵ %d ��ҪУʱ \n",Pn,
                        pAmrThreadArg->pMtrAmrInfo[MtrIndex].TimeError,pAmrThreadArg->BroadTiming.limit);
                    //pAmrThreadArg->BroadTiming.state = TMN_TO_METER_GET_DELAY;
                    // ֱ������
                    pAmrThreadArg->BroadTiming.state = TMN_TO_METER_SETTIME_INIT;

                }
                pAmrThreadArg->pMtrAmrInfo[MtrIndex].GetTimeIndex = 0;
            }
        }
    }
        break;
    case PRTL_OOP:
    {
        AppLayerList_t *pAppLayer = NULL;
        uint8 databuf[100]={0};
        uint16 datalen;
        uint16 index=0;
        //OOP_DATETIME_S_T Datetimes;
        //memset(&Datetimes,0x00,sizeof(OOP_DATETIME_S_T));
        ret = LinkDataReceive(pipe,buf,len,NULL);
        TASK_FMT_DEBUG(pipe, RELAY_LOG,"LinkDataReceive ret %d \n",ret);
        if(ret >=eOopComm_Fail)//��֡ʧ��ֱ�ӷ���
        {
            pAmrThreadArg->BroadTiming.MtrIndex++;
            pAmrThreadArg->BroadTiming.FailCnt = 0;
            pAmrThreadArg->BroadTiming.state =TMN_TO_METER_RDINIT_FIRST;
            break ;
        }
        //��ȡ����ַ
        pAppLayer = FindAppLayer(pipe, NULL);
        if(pAppLayer == NULL)
            break;
        datalen = pAppLayer->recvApduList->apduLen-pAppLayer->recvApduList->offSet;
        memcpy(databuf, pAppLayer->recvApduList->apdu+pAppLayer->recvApduList->offSet, datalen);
        TASK_BUF_TRACE(pipe, RELAY_LOG,databuf,datalen);
        //databuf��oad����Ľ�����Ϳ�ʼ
        if(databuf[index++]!=1)
            break;
        if(databuf[index++]!=DT_DATETIME_S)
            break;
        memcpy_r((uint8*)&pAmrThreadArg->pMtrAmrInfo[MtrIndex].MeterTime.year,&databuf[index],2);
        index+=2;
        pAmrThreadArg->pMtrAmrInfo[MtrIndex].MeterTime.month = databuf[index++];
        pAmrThreadArg->pMtrAmrInfo[MtrIndex].MeterTime.day = databuf[index++];
        pAmrThreadArg->pMtrAmrInfo[MtrIndex].MeterTime.hour = databuf[index++];
        pAmrThreadArg->pMtrAmrInfo[MtrIndex].MeterTime.minute = databuf[index++];
        pAmrThreadArg->pMtrAmrInfo[MtrIndex].MeterTime.second = databuf[index++];
        DT_DateTimeShort2Time(&pAmrThreadArg->pMtrAmrInfo[MtrIndex].MeterTime,&metertime);
        pAmrThreadArg->pMtrAmrInfo[MtrIndex].TimeError = abs(time(NULL)-metertime);
        //��ȡ���ʱ������ ������ȡ�ӽڵ�����ʱ
        if(pAmrThreadArg->pMtrAmrInfo[MtrIndex].TimeError <pAmrThreadArg->BroadTiming.limit )
        {
            TASK_FMT_DEBUG(pipe, RELAY_LOG, "��ǰ��� %d ���ն�ʱ�� %d С����ֵ %d ����Уʱ \n",Pn,
                pAmrThreadArg->pMtrAmrInfo[MtrIndex].TimeError,pAmrThreadArg->BroadTiming.limit);
            pAmrThreadArg->BroadTiming.MtrIndex++;
            pAmrThreadArg->BroadTiming.FailCnt = 0;
            pAmrThreadArg->BroadTiming.state =TMN_TO_METER_RDINIT_FIRST;
        }else
        {
            TASK_FMT_DEBUG(pipe, RELAY_LOG, "��ǰ��� %d ���ն�ʱ�� %d ������ֵ %d ��ҪУʱ \n",Pn,
                pAmrThreadArg->pMtrAmrInfo[MtrIndex].TimeError,pAmrThreadArg->BroadTiming.limit);
            //pAmrThreadArg->BroadTiming.state = TMN_TO_METER_GET_DELAY;
            pAmrThreadArg->BroadTiming.state = TMN_TO_METER_SETTIME_INIT;
        }
    }
        break;
    default:
        break;
    }

}



/*******************************************************************************
* ��������: BroadTimingSend
* ��������://�㲥Уʱ����֡
* �������:
* �������:
* �� �� ֵ:  ����1 �����ݷ��� ����0 ������� ����2 �л�����״̬
*******************************************************************************/
int BroadTimingSend(uint8 pipe)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    switch(pAmrThreadArg->BroadTiming.state)
    {
    //��ȡ��ʱ
    case TMN_TO_METER_GET_DELAY:
    {
        if(pAmrThreadArg->BroadTiming.TimingType == 1)
        {
            if(GetBroadDelay(pipe)>0)
            {
                break;
            }else
            {
                TASK_FMT_DEBUG(pipe, RELAY_LOG, "�㲥Уʱ���й�Լȫ�������� ������� \n");
                return 0;
            }
        }
        else
        {
            if(GetMeterDelay(pipe)>0)
                break;
        }
    }
        break;
    case TMN_TO_METER_SETTIME_INIT:
    {
        if(pAmrThreadArg->BroadTiming.TimingType == 1)
        {
            if(SendBroadTiming(pipe)>0)
            {
                break;
            }
        }
        else
        {
            if(SetMeterTime(pipe)>0)
            {
                break;
            }
        }
    }
        break;
    case TMN_TO_METER_SETTIME_CIPHER:
    {
        SendBroadTimingCipher(pipe);

    }
        break;
    //��ѯ���ʱ��
    case TMN_TO_METER_RDINIT_FIRST:
    {
        if(GetMeterTime(pipe)>0)
        {
            break;
        }else
        {
            TASK_FMT_DEBUG(pipe, RELAY_LOG, "����ַ�㲥Уʱ���б�ȫ�������� ������� \n");
            return 0;
        }
    }
        break;
    default:
        return 0;
    }
    return 1;
}


/**
*********************************************************************
* @brief:      ʵ��  485�㲥Уʱ�͵���Уʱ�Ĺ��ܣ�
* @param[in]��   pipe  �̺߳�,485ģ�鹲8���߳�,485���߳�pipe��2��ʼ��9
* @return������1 �����ݷ��� ����0 ������� ����2 �л�����״̬
*********************************************************************
*/
int Rs485BroadTimingSend(uint8 pipe)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    switch(pAmrThreadArg->BroadTiming.state)
    {
    case TMN_TO_METER_SETTIME_INIT:
    {
        TASK_FMT_DEBUG(pipe, RELAY_LOG, "pAmrThreadArg->BroadTiming.TimingType is %d prtl %d \n",pAmrThreadArg->BroadTiming.TimingType,pAmrThreadArg->BroadTiming.BroadPrtl);
        if(pAmrThreadArg->BroadTiming.TimingType == 1)
        {
            if(Rs485SendBroadTiming(pipe)>0)
            {
                break;
            }
            else
            {
                return 0;
            }
        }
        else
        {
            if(Rs485SetMeterTime(pipe)>0)
            {
                break;
            }
             else
            {
                return 0;
            }
        }
    }
        break;
    //��ѯ���ʱ��
    case TMN_TO_METER_RDINIT_FIRST:
    {
        if(Rs485GetMeterTime(pipe)>0)
        {
            break;
        }else
        {
            TASK_FMT_DEBUG(pipe, RELAY_LOG, "����ַ�㲥Уʱ���б�ȫ�������� ������� \n");
            return 0;
        }
    }
        break;
    case TMN_TO_METER_SETTIME_CIPHER:
    {
        Rs485SendBroadTimingCipher(pipe);
        return 0;
    }
        break;
    default:
        return 0;
    }
    return 1;
}
/*******************************************************************************
* ��������: SetMeterTime
* ��������://ͨ���ӽڵ������õ��ʱ��
* �������:
* �������:
* �� �� ֵ:
*******************************************************************************/
int SetMeterTime(uint8 pipe)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    uint16 MtrIndex = pAmrThreadArg->BroadTiming.MtrIndex;
    uint16 Pn = pAmrThreadArg->pMtrAmrInfo[MtrIndex].Pn;
    uint8 logicid = pAmrThreadArg->pMtrAmrInfo[MtrIndex].logicid;
    uint8 prtl;
    uint8 Addr[6]={0};
    uint8 databuf[200]={0};
    uint16 datalen;
    taskmng_mtr_addr_prtl_get_new( Pn,logicid, Addr, & prtl);

    datalen = PackSetTimeFrame(pipe,databuf,prtl,0,pAmrThreadArg->pMtrAmrInfo[MtrIndex].TimeDelay);
    if(datalen>0)
    {
        TASK_FMT_DEBUG(pipe, RELAY_LOG, "���õ�� %d ʱ�� \n",Pn);
        //taskmng_router_ctrl_set ( pipe,RCS_PAUSE );
        if(prtl == PRTL_20_IOT)
            prtl = PRTL_OOP;
        SendFrame_0038(pipe,prtl,Addr,databuf,datalen,&pAmrThreadArg->BroadTiming.FrameIndex);
        return 1;
    }
    return 0;
}

/*******************************************************************************
* ��������: SetMeterTime
* ��������://ͨ���ӽڵ������õ��ʱ��
* �������:
* �������:
* �� �� ֵ:
*******************************************************************************/
int Rs485SetMeterTime(uint8 pipe)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    uint16 MtrIndex = pAmrThreadArg->BroadTiming.MtrIndex;
    uint16 Pn = pAmrThreadArg->pMtrAmrInfo[MtrIndex].Pn;
    uint8 logicid = pAmrThreadArg->pMtrAmrInfo[MtrIndex].logicid;
    uint8 prtl;
    uint8 Addr[6]={0};
    uint8 databuf[200]={0};
    uint16 datalen;
    uint8 speed;
    int ret=0;
    OOP_METER_BASIC_T MeterBasicInfo;
    taskmng_mtr_addr_prtl_get_new( Pn, logicid,Addr, & prtl);

    datalen = PackSetTimeFrame(pipe,databuf,prtl,0,0);
    switch(prtl)
    {
        case PRTL_97:
            {
                speed =2;
            }
        break;
        case PRTL_07:
             {
                speed =3;
            }
        break;
        case PRTL_OOP:
        case PRTL_20_IOT:
            {
                speed =6;
            }
        break;
        default:
                TASK_FMT_TRACE( pipe, RELAY_LOG, "prtl is %d\n",prtl);
                return 0;
        break;

    }
    TASK_FMT_TRACE( pipe, RELAY_LOG, "pAmrThreadArg->BroadTiming.TimingType is %d\n",pAmrThreadArg->BroadTiming.TimingType);
    if(pAmrThreadArg->BroadTiming.TimingType !=1)
    {
        MtrIndex = pAmrThreadArg->BroadTiming.MtrIndex;
        Pn = pAmrThreadArg->pMtrAmrInfo[MtrIndex].Pn;
        logicid = pAmrThreadArg->pMtrAmrInfo[MtrIndex].logicid;

        taskmng_mtr_basic_info_get(Pn,logicid,&MeterBasicInfo);
        speed = MeterBasicInfo.baud;
    }

    TASK_FMT_TRACE( pipe, RELAY_LOG, "����datalen is %d\n",datalen);
    if(datalen>0)
    {
        TASK_BUF_TRACE(pipe, RELAY_LOG, databuf, datalen);
        ret=rs485_RS232_send(pipe,MAX_FRAME_OVERTIME,MAX_CHAR_OVERTIME,speed,PARITY_EVEN,databuf, datalen);
        if(ret !=0)
        {
            TASK_FMT_TRACE( pipe, RELAY_LOG, "���ͽ�� ret is %d\n",ret);
        }
        return 1;
    }
    return 0;
}
/*******************************************************************************
* ��������: GetMeterDelay
* ��������://��ȡÿ�����Ĵӽڵ�����ʱ
* �������:
* �������:
* �� �� ֵ:
*******************************************************************************/
int GetMeterDelay(uint8 pipe)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    uint16 MtrIndex = pAmrThreadArg->BroadTiming.MtrIndex;
    uint16 Pn = pAmrThreadArg->pMtrAmrInfo[MtrIndex].Pn;
    uint8 logicid = pAmrThreadArg->pMtrAmrInfo[MtrIndex].logicid;
    uint8 prtl;
    uint8 Addr[6]={0};
    uint8 SendBuf[1000]={0};
    uint16 index=0;
    uint16 datalen;
    uint8 databuf[100]={0};
    OOP_OAD_U pipeOad = taskmng_pipe_oad_get( pipe);
    taskmng_mtr_addr_prtl_get( Pn,logicid, Addr, & prtl);

    datalen = PackSetTimeFrame(pipe,databuf,prtl,0,0);
    memcpy_r(&SendBuf[index],(uint8*)&pipeOad.value,4);
    index+=4;
    memcpy_r(&SendBuf[index],Addr,6);
    index+=6;
    SendBuf[index++] = prtl;
    if(datalen<0x80)
    {
        SendBuf[index++] = datalen;
    }
    else
    {
        SendBuf[index++] = 0x82;
        SendBuf[index++] = (uint8)(datalen>>8);
        SendBuf[index++] = (uint8)datalen;
    }
    memcpy(&SendBuf[index],databuf,datalen);
    index+=datalen;
    taskmng_msg_data_pack(pipe,0x0003,0x0037,index,SendBuf,&pAmrThreadArg->BroadTiming.FrameIndex);
    TASK_FMT_DEBUG(pipe, RELAY_LOG, "���ͻ�ȡ�ӽڵ�����ʱ���� MsgIndex %d  \n",pAmrThreadArg->BroadTiming.FrameIndex);
    TASK_BUF_DEBUG(pipe, RELAY_LOG,SendBuf,index);
    return 1;
}


/*******************************************************************************
* ��������: GetMeterTime
* ��������://��ѯ���ʱ��
* �������:
* �������:
* �� �� ֵ:
*******************************************************************************/
int GetMeterTime(uint8 pipe)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    uint16 MtrIndex;
    uint8 prtl;
    uint16 Pn;
    uint8 logicid;
    uint8 Addr[6];
    uint8 SendBuf[100]={0};
    int len = 0;
    MeterPrtlFmt_T    Meter;
    memset(&Meter,0x00,sizeof(MeterPrtlFmt_T));
    if(pAmrThreadArg->BroadTiming.MtrIndex>=pAmrThreadArg->TmnAmrInfo.MtrCnt)
        return 0;
    MtrIndex = pAmrThreadArg->BroadTiming.MtrIndex;
    Pn = pAmrThreadArg->pMtrAmrInfo[MtrIndex].Pn;
    logicid = pAmrThreadArg->pMtrAmrInfo[MtrIndex].logicid;
    taskmng_mtr_addr_prtl_get(Pn,logicid,Addr,&prtl);
    switch(prtl)
    {
    case PRTL_97:
    {
        uint16 DI;
        memcpy(Meter.MtrAddr,Addr,6);
        Meter.CtrlCode = 0x01;
        Meter.DataLen = 2;
        if(pAmrThreadArg->pMtrAmrInfo[MtrIndex].GetTimeIndex==0)
        {
            DI = 0XC010;
        }else
        {
            DI = 0xC011;
        }
        memcpy(Meter.DataBuff,&DI,2);
        len = taskmng_meter_prtl_645_pack(SendBuf,Meter);
    }
        break;
    case PRTL_07:
    {
        uint32 DI;
        memcpy(Meter.MtrAddr,Addr,6);
        Meter.CtrlCode = 0x11;
        Meter.DataLen = 4;
        if(pAmrThreadArg->pMtrAmrInfo[MtrIndex].GetTimeIndex==0)
        {
            DI = 0x04000101;
        }else
        {
            DI = 0x04000102;
        }
        memcpy(Meter.DataBuff,&DI,4);
        len = taskmng_meter_prtl_645_pack(SendBuf,Meter);
    }
        break;
    case PRTL_OOP:
    {
        OOP_OAD_U Oad = {0x40000200};
        LinkLayer_t *pLinkLayer = NULL;
        uint8 buf[100]={0};
        uint16 index=0;
        OOP_TSA_T Tsa;
        memset(&Tsa,0x00,sizeof(OOP_TSA_T));
        Tsa.len = 5;
        memcpy_r(Tsa.add,Addr,6);
        memcpy(buf,(uint8*)&Oad.value,4);
        index+=4;
        OopGet.requst(pipe,&Tsa,1,buf,index);
        pLinkLayer = FindLinkLayer(pipe, NULL);

        if(pLinkLayer!=NULL)
        {
            len = pLinkLayer->sendLen;
            memcpy(SendBuf,pLinkLayer->sendBuff,len);
        }
   }
        break;
    default:
        break;
    }
    if(len>0)
    {
        TASK_FMT_DEBUG(pipe, RELAY_LOG, "��ѯ��� %d ʱ�� \n",Pn);
        //taskmng_router_ctrl_set ( pipe,RCS_PAUSE );
        SendFrame_0038(pipe,prtl,Addr,SendBuf,len,&pAmrThreadArg->BroadTiming.FrameIndex);
    }
    return 1;
}

/*******************************************************************************
* ��������: GetMeterTime
* ��������://��ѯ���ʱ��
* �������:
* �������:
* �� �� ֵ:
*******************************************************************************/
int Rs485GetMeterTime(uint8 pipe)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    uint16 MtrIndex;
    uint8 prtl;
    uint16 Pn;
    uint8 logicid;
    uint8 Addr[6];
    uint8 SendBuf[100]={0};
    int len;
    int ret;
    MeterPrtlFmt_T    Meter;
    OOP_METER_BASIC_T MeterBasicInfo;
    //uint16 Speed;

    memset(&Meter,0x00,sizeof(MeterPrtlFmt_T));
    if(pAmrThreadArg->BroadTiming.MtrIndex>=pAmrThreadArg->TmnAmrInfo.MtrCnt)
        return 0;
    MtrIndex = pAmrThreadArg->BroadTiming.MtrIndex;
    Pn = pAmrThreadArg->pMtrAmrInfo[MtrIndex].Pn;
    logicid = pAmrThreadArg->pMtrAmrInfo[MtrIndex].logicid;


    taskmng_mtr_basic_info_get(Pn,logicid,&MeterBasicInfo);
    memcpy_r(Addr,MeterBasicInfo.tsa.add,6);
    prtl = MeterBasicInfo.protocol;
//    Speed=get_meter_read_speed(MeterBasicInfo.baud);
//    if(rs485_RS232_Init(&Rs485ComNo[pipe-2],Speed)!=0)
//    {
//        rs485_RS232_UnInit(&Rs485ComNo[pipe-2]);//����ʱ���
//        TASK_FMT_TRACE( pipe, COMM_LOG, "��ʼ��ʧ��,pipe is %d\n",pipe);
//        return 1;
//    }
    switch(prtl)
    {
    case PRTL_97:
    {
        uint16 DI;
        memcpy(Meter.MtrAddr,Addr,6);
        Meter.CtrlCode = 0x01;
        Meter.DataLen = 2;
        if(pAmrThreadArg->pMtrAmrInfo[MtrIndex].GetTimeIndex==0)
        {
            DI = 0XC010;
        }else
        {
            DI = 0xC011;
        }
        memcpy(Meter.DataBuff,&DI,2);
        len = taskmng_meter_prtl_645_pack(SendBuf,Meter);
    }
        break;
    case PRTL_07:
    {
        uint32 DI;
        memcpy(Meter.MtrAddr,Addr,6);
        Meter.CtrlCode = 0x11;
        Meter.DataLen = 4;
        if(pAmrThreadArg->pMtrAmrInfo[MtrIndex].GetTimeIndex==0)
        {
            DI = 0x04000101;
        }else
        {
            DI = 0x04000102;
        }
        memcpy(Meter.DataBuff,&DI,4);
        len = taskmng_meter_prtl_645_pack(SendBuf,Meter);
    }
        break;
    case PRTL_OOP:
    {
        OOP_OAD_U Oad = {0x40000200};
        LinkLayer_t *pLinkLayer = NULL;
        uint8 buf[100]={0};
        uint16 index=0;
        OOP_TSA_T Tsa;
        memset(&Tsa,0x00,sizeof(OOP_TSA_T));
        Tsa.len = 5;
        memcpy_r(Tsa.add,Addr,6);
        memcpy(buf,(uint8*)&Oad.value,4);
        index+=4;
        OopGet.requst(pipe,&Tsa,1,buf,index);
        pLinkLayer = FindLinkLayer(pipe, NULL);

        if(pLinkLayer!=NULL)
        {
            len = pLinkLayer->sendLen;
            memcpy(SendBuf,pLinkLayer->sendBuff,len);
        }
   }
        break;
    default:
        break;
    }
    if(len>0)
    {
        TASK_FMT_DEBUG(pipe, RELAY_LOG, "��ѯ��� %d ʱ��,len is %d\n",Pn,len);

        ret=rs485_RS232_send(pipe,MAX_FRAME_OVERTIME,MAX_CHAR_OVERTIME,MeterBasicInfo.baud,PARITY_EVEN,SendBuf, len);
        if(ret !=0)
        {
            TASK_FMT_TRACE( pipe, COMM_LOG, "���ͽ�� ret is %d\n",ret);
        }
    }
    return 1;
}

/*******************************************************************************
* ��������: SendFrame_0038
* ��������://��ӽڵ��ط���
* �������:
* �������:
* �� �� ֵ:
*******************************************************************************/
void SendFrame_0038(uint8 pipe,uint8 prtl,uint8 *Addr,uint8 *buf,uint16 len,uint16 *MsgIndex)
{
    uint8 payload[200]={0};
    uint16 index = 0;
    uint16 Overtime = BROAD_TIMING_OVERTIME;
    OOP_OAD_U pipeoad = taskmng_pipe_oad_get( pipe);
    memcpy_r(&payload[index],(uint8*)&pipeoad.value,4);
    index+=4;
    payload[index++] = prtl;
    memcpy_r(&payload[index],Addr,6);
    index+=6;
    index+=2;
    memcpy_r(&payload[index],(uint8*)&Overtime,2);
    index+=2;
    memcpy_r(&payload[index],(uint8*)&Overtime,2);
    index+=2;
    if(len<0x80)
    {
        payload[index++] = len;
    }
    else
    {
        payload[index++] = 0x82;
        payload[index++] = (uint8)(len>>8);
        payload[index++] = (uint8)len;
    }
    memcpy(&payload[index],buf,len);
    index+=len;
    taskmng_msg_data_pack(pipe,0x0003,0x0038,index,payload,MsgIndex);

    TASK_BUF_TRACE(pipe, RELAY_LOG,payload,index);
}


/*******************************************************************************
* ��������: SendBroadTiming
* ��������://���͹㲥Уʱ
* �������:
* �������:
* �� �� ֵ:
*******************************************************************************/
int SendBroadTiming(uint8 pipe)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    uint8 prtl ;
    uint8 buf[200]={0};
    uint16 len=0;

    prtl = pAmrThreadArg->BroadTiming.BroadPrtl;
    len = PackSetTimeFrame(pipe,buf,prtl,1,pAmrThreadArg->BroadTiming.BroadDelay);
    if(len>0)
    {
        if(prtl == PRTL_20_IOT)
            prtl = PRTL_OOP;
        taskmng_data_broad(pipe,buf,len,prtl,&pAmrThreadArg->BroadTiming.FrameIndex);
        return 1;
    }
    return 0;
}

/*******************************************************************************
* ��������: SendBroadTiming
* ��������://���͹㲥Уʱ
* �������:
* �������:
* �� �� ֵ:
*******************************************************************************/
int Rs485SendBroadTiming(uint8 pipe)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    uint8 prtl ;
    uint8 buf[200]={0};
    uint16 len=0;
    uint8 speed;
    int ret=0;
    uint8 i;
    prtl = pAmrThreadArg->BroadTiming.BroadPrtl;
    len = PackSetTimeFrame(pipe,buf,prtl,1,0);
    TASK_FMT_TRACE( pipe, RELAY_LOG, "Rs485SendBroadTiming prtl %d len %d\n",prtl,len);
//    switch(prtl)
//    {
//        case PRTL_97:
//            {
//                speed =2;
//            }
//        break;
//        case PRTL_07:
//             {
//                speed =3;
//            }
//        break;
//        case PRTL_OOP:
//            {
//                speed =6;
//            }
//        break;
//        case PRTL_20_IOT:
//            {
//                speed =10;
//            }
//        break;
//        default:
//                return 0;
//        break;
//
//    }
//    if(rs485_RS232_Init(&Rs485ComNo[pipe-2],speed)!=0)
//    {
//        rs485_RS232_UnInit(&Rs485ComNo[pipe-2]);//����ʱ���
//        TASK_FMT_TRACE( pipe, COMM_LOG, "��ʼ��ʧ��,pipe is %d\n",pipe);
//        return 0;
//    }

    if(len>0)
    {

        for(i=0;i<gSpeedList.num;i++)
        {
            len = PackSetTimeFrame(pipe,buf,prtl,1,0);
            if(len>0)
            {
                speed = gSpeedList.speed[i];
                TASK_FMT_TRACE( pipe, RELAY_LOG, "�㲥Уʱ���� speed %d\n",speed);
                TASK_BUF_TRACE(pipe, RELAY_LOG, buf, len);
                ret=rs485_RS232_send(pipe,2,MAX_CHAR_OVERTIME,speed,PARITY_EVEN,buf, len);
                if(ret !=0)
                {
                    TASK_FMT_TRACE( pipe, RELAY_LOG, "���ͽ�� ret is %d\n",ret);
                }
                sleep(3);
            }
        }
        return 1;
    }
    return 0;
}
/*******************************************************************************
* ��������: GetBroadDelay
* ��������://��ȡ�㲥��ʱ
* �������:
* �������:
* �� �� ֵ:  1�з��� 0 û��
*******************************************************************************/
int GetBroadDelay(uint8 pipe)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    uint8 prtl ;
    uint8 buf[200]={0};
    uint16 len=0;
    for(prtl =pAmrThreadArg->BroadTiming.BroadPrtl;prtl<=PRTL_OOP;prtl++ )
    {
        //�����ǰ������û�ж�Ӧ�Ĺ�Լ����Ҫ���й㲥
        if(taskmng_prtl_check(prtl)==FALSE)
            continue;
        len = PackSetTimeFrame(pipe,buf,prtl,1,0);
        if(len>0)
        {
            pAmrThreadArg->BroadTiming.BroadPrtl = prtl;
            taskmng_broad_delay_send(pipe,buf,len,prtl,&pAmrThreadArg->BroadTiming.FrameIndex);
            return 1;
        }
    }
    return 0;
}


/*******************************************************************************
* ��������: PackSetTimeFrame
* ��������://��װУʱ����
* �������:delay ����ʱ����Ҫ���ϵ���ʱ
        isBroad�Ƿ�㲥
* �������:
* �� �� ֵ:  1�з��� 0 û��
*******************************************************************************/
uint16 PackSetTimeFrame(uint8 pipe,uint8* outBuf,uint8 prtl,uint8 isBroad,uint16 delay)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    uint16 len=0;
    //����̨�� ��Уʱ����ʱֻ��50����ǰ
    struct timeval tv;
    struct  timezone tz;
    while(1)
    {
        gettimeofday(&tv, &tz);
        if(tv.tv_usec < 50000)
        {
            break;
        }
        usleep(10000);
    }
    time_t timenow = time(NULL);
    timenow+=delay;
    uint8 Addr[6];
    uint16 Pn;
    uint8 logicid;
    switch(prtl)
    {
    case PRTL_07:
    case PRTL_97:
    {
        rtc_t prtc;

        MeterPrtlFmt_T    Meter;
        memset(&Meter,0x00,sizeof(MeterPrtlFmt_T));
        if(isBroad==1)
        {
            memset(Addr,0x99,6);
        }else
        {
            Pn = pAmrThreadArg->pMtrAmrInfo[pAmrThreadArg->BroadTiming.MtrIndex].Pn;
            logicid = pAmrThreadArg->pMtrAmrInfo[pAmrThreadArg->BroadTiming.MtrIndex].logicid;
            taskmng_mtr_addr_get(Pn,logicid,Addr);
        }
        memcpy(Meter.MtrAddr,Addr,6);
        Meter.CtrlCode = 0x08 ;
        Meter.DataLen = 0x06;
        TimeT2RTC(timenow,&prtc);
        Meter.DataBuff[0] = prtc.ss;
        Meter.DataBuff[1] = prtc.nn;
        Meter.DataBuff[2] = prtc.hh;
        Meter.DataBuff[3] = prtc.dd;
        Meter.DataBuff[4] = prtc.mm;
        Meter.DataBuff[5] = prtc.yy;
        len = taskmng_meter_prtl_645_pack(outBuf,Meter);
    }
        break;
    case PRTL_OOP:
    {
        LinkLayer_t *pLinkLayer = NULL;
        uint8 buf[100]={0};
        uint16 index=0;
        OOP_TSA_T Tsa;
        memset(&Tsa,0x00,sizeof(OOP_TSA_T));
        DateTimeHex_t TimeHex;
        if(isBroad)
        {
            Tsa.af = 0xC0;
            Tsa.add[0] = 0xAA;
        }else
        {
            Pn = pAmrThreadArg->pMtrAmrInfo[pAmrThreadArg->BroadTiming.MtrIndex].Pn;
            logicid = pAmrThreadArg->pMtrAmrInfo[pAmrThreadArg->BroadTiming.MtrIndex].logicid;
            taskmng_mtr_addr_get(Pn,logicid,Addr);
            Tsa.len = 5;
            memcpy_r(Tsa.add,Addr,6);
        }
        DT_Time2DateTimeHex(timenow,&TimeHex);
        buf[index++] = 0x40;
        buf[index++] = 0x00;
        buf[index++] = 0x7f;
        buf[index++] = 0x00;
        buf[index++] = DT_DATETIME_S;
        memcpy(&buf[index],&TimeHex,7);
        index+=7;
        OopAction.requst(pipe,&Tsa,1,buf,index);
        pLinkLayer = FindLinkLayer(pipe, NULL);
        if(pLinkLayer!=NULL)
        {
            len = pLinkLayer->sendLen;
            memcpy(outBuf,pLinkLayer->sendBuff,len);
        }
   }
        break;
    case PRTL_20_IOT:
    {
        LinkLayer_t *pLinkLayer = NULL;
        uint8 buf[100]={0};
        uint16 index=0;
        OOP_TSA_T Tsa;
        memset(&Tsa,0x00,sizeof(OOP_TSA_T));
        DateTimeHex_t TimeHex;
        if(isBroad)
        {
            Tsa.af = 0xC0;
            Tsa.add[0] = 0xAA;
            Tsa.oldvxd = 1;
        }else
        {
            Pn = pAmrThreadArg->pMtrAmrInfo[pAmrThreadArg->BroadTiming.MtrIndex].Pn;
            logicid = pAmrThreadArg->pMtrAmrInfo[pAmrThreadArg->BroadTiming.MtrIndex].logicid;
            taskmng_mtr_addr_get(Pn,logicid,Addr);
            Tsa.len = 5;
            memcpy_r(Tsa.add,Addr,6);
            Tsa.oldvxd = 1;
        }
        DT_Time2DateTimeHex(timenow,&TimeHex);
        buf[index++] = 0x40;
        buf[index++] = 0x00;
        buf[index++] = 0x7f;
        buf[index++] = 0x00;
        buf[index++] = DT_DATETIME_S;
        memcpy(&buf[index],&TimeHex,7);
        index+=7;
        OopAction.requst(pipe,&Tsa,1,buf,index);
        pLinkLayer = FindLinkLayer(pipe, NULL);
        if(pLinkLayer!=NULL)
        {
            len = pLinkLayer->sendLen;
            memcpy(outBuf,pLinkLayer->sendBuff,len);
        }
   }
        break;
    default:
        break;
    }
    return len;
}




/*******************************************************************************
* ��������: taskmng_update_cycle_load
* ��������://���������ļ���0XF2090600 ��������
* �������:
* �������:
* �� �� ֵ:
*******************************************************************************/
void taskmng_update_cycle_load()
{
    int   ret;
    uint32 Len;

    NOMAL_OAD_T NormalOad;
    NormalOad.logicId = 0;
    NormalOad.oad.value = 0XF2090600;
    NormalOad.infoNum = 0;
    OOP_TI_T ti;
    memset(&ti,0x00,sizeof(OOP_TI_T));
    pthread_mutex_lock(&UpdateCycle.Lock);
    ret = db_read_nomal(gDBClientMain,&NormalOad,sizeof(OOP_TI_T),(uint8*)&ti,&Len);
    if(ret!=0 || Len != sizeof(OOP_TI_T))
    {
        TASK_FMT_DEBUG(-1,UP_LOG, "����0XF2090600��������ʧ�� ret %d  \n",ret);
        UpdateCycle.Ti = 1440; //���û��������Ĭ������Ϊһ��
        UpdateCycle.isSet = 0;
    }else
    {
        if(ti.value ==0)
        {
            UpdateCycle.Ti = 0;
        }else
        {
            switch(ti.unit)
            {
                case 0:
                    UpdateCycle.Ti = ti.value/60;
                    break;
                case 1:
                    UpdateCycle.Ti = ti.value;
                    break;
                case 2:
                    UpdateCycle.Ti = ti.value*60;
                    break;
                //���˸���λ���FF��ʾ�����ڲ�ѯ
                case 0xff:
                    UpdateCycle.Ti = 0;
                    break;
                default:
                    UpdateCycle.Ti = 1440;
                    break;
            }
        }
        UpdateCycle.isSet = 1;
        TASK_FMT_DEBUG(-1,UP_LOG, "����0XF2090600��������Ϊ %d  \n",UpdateCycle.Ti );
    }
    pthread_mutex_unlock(&UpdateCycle.Lock);
}


/*******************************************************************************
* ��������: taskmng_plc_pipe_set
* ��������:����˿�����
* �������:pipe  �˿�
        choice 0 ɾ�� 1 ���
* �������:
* �� �� ֵ:
*******************************************************************************/
void taskmng_plc_pipe_set(uint8 pipe,uint8 choice)
{
    uint16 IID = 0x0003;
    uint16 IOP = 0x0010;
    OOP_OAD_U pipeOAD = taskmng_pipe_oad_get(pipe);
    uint16 len=0;
    uint8 Buffer[10]={0};
    uint16 RecvLen;
    uint8 RecvBuf[MAX_MSG_LEN];
    memcpy_r(&Buffer[len],(uint8*)&pipeOAD.value,4);
    len+=4;
    Buffer[len++] = choice;
    TASK_FMT_DEBUG( pipe, COMM_LOG, "���ó���˿� \n");
    taskmng_msg_fun_deal(pipe,IID,IOP,len,Buffer,&RecvLen,RecvBuf);
    if(RecvLen==1 && RecvBuf[0]==1)
    {
        TASK_FMT_DEBUG( pipe, COMM_LOG, "����˿����óɹ� \n");
    }
}




/*******************************************************************************
* ��������: Appversion_Response
* ��������:�յ�iid 0000 IOP0013��ظ��汾��Ϣ
* �������:
* �������:
* �� �� ֵ:
*******************************************************************************/
void Appversion_Response(MESSAGE_INFO_T *MsgRecv )
{
    uint8 payload[100]={0};
    uint8 index = 0;
    //uint32 appversion = APP_VERSION;
    Date_Time_S publishtime;
    memset(&publishtime,0x00,sizeof(Date_Time_S));

    payload[index++] = strlen(APP_VERSION);
    memcpy(&payload[index],APP_VERSION,strlen(APP_VERSION));
    index+=strlen(APP_VERSION);
//    memcpy_r(&payload[index],(uint8*)&appversion,4);
//    index+=4;
    app_get_maketime(APP_PUBLISH_TIME,&publishtime);
    payload[index++]=publishtime.year_h;
    payload[index++]=publishtime.year_l;
    payload[index++]=publishtime.month;
    payload[index++]=publishtime.day;
    payload[index++]=publishtime.hour;
    payload[index++]=publishtime.min;
    payload[index++]=publishtime.sec;

    MESSAGE_INFO_T *MsgSend = (MESSAGE_INFO_T*)malloc(sizeof(MESSAGE_INFO_T)+index);
    memset(MsgSend,0x00,sizeof(MESSAGE_INFO_T)+index);
    MsgSend->rpm = 0;
    MsgSend->priority = MsgRecv->priority;
    MsgSend->index = MsgRecv->index;
    MsgSend->label = MsgRecv->label;
    MsgSend->sourlen = strlen(APPNAME);
    memcpy(MsgSend->souraddr,APPNAME,MsgSend->sourlen);
    MsgSend->destlen = MsgRecv->sourlen;
    memcpy(MsgSend->destaddr,MsgRecv->souraddr,MsgSend->destlen );
    TASK_FMT_DEBUG(-1,UP_LOG, "��ѯ���ذ汾��Ϣ SourAddr %s DestAddr %s \n",MsgSend->souraddr,MsgSend->destaddr);
    MsgSend->IID = MsgRecv->IID;
    MsgSend->IOP = MsgRecv->IOP;
    MsgSend->msglen = index;
    memcpy(MsgSend->playload,payload,index);
    TASK_FMT_DEBUG(-1, UP_LOG, "Send IID=0X%04x IOP=0x%04x MsgIndex %d \n",MsgSend->IID,MsgSend->IOP,MsgSend->index);

    TASK_BUF_DEBUG(-1, UP_LOG, payload, index);
    taskmng_plc_queue_write ( APPArg.SendBuf, (uint8*)MsgSend, sizeof(MESSAGE_INFO_T)+index );
    free(MsgSend);

}

/*******************************************************************************
* ��������: taskmng_oop_frame_broadtiming_check
* ��������:�ж�698֡�Ƿ�Ϊ�㲥Уʱ֡
* �������:buf ��������
         len ���ĳ���
* �������: settime ����ǹ㲥Уʱ֡��ΪУʱ��ʱ��
* �� �� ֵ:  TRUE FALSE
*******************************************************************************/
BOOL taskmng_oop_frame_broadtiming_check(uint8* buf,uint16 len,time_t *settime)
{
    uint16 offset =0 ;
    OOP_OMD_U omd;
    OOP_TSA_T tsa;
    DateTimeHex_t TimeHex;
    //head len
    offset+=3;
    //crtl

    offset+=1;
    //af
    tsa.af = buf[offset++];
    //co��ʾ�㲥��ַ
    if(tsa.af !=0xc0)
        return FALSE;
    //sa
    if(buf[offset++]!=0xAA)
    {
        return FALSE;
    }
    //ca

    offset++;
    //HCS
    offset +=2;
    //������ ��           [7]        ACTION-Request��
    uint8 requestchoice = buf[offset++];
    if(requestchoice!=SERVERID_ACTIONREQUEST &&requestchoice!=SERVERID_SETREQUEST)
    {
        return FALSE;
    }
    //�������һ�����󷽷�                        [1] ActionRequestNormal��
    if(buf[offset++]!=1)
    {
        return FALSE;
    }
    //PIID
    offset++;
    //omd
    memcpy_r((uint8*)&omd,&buf[offset],4);
    offset+=4;
    if(omd.value!=0x40007f00 && omd.value!=0x40000200)
    {
        return FALSE;
    }
    if(buf[offset++]!=DT_DATETIME_S)
    {
        return FALSE;
    }
    memcpy(&TimeHex,&buf[offset],7);
    DT_DateTimeHex2Time(&TimeHex,settime);
    return TRUE;
}






/*******************************************************************************
* ��������: taskmng_frame_3762_format_check
* ��������: ���3762֡��ʽ
* �������: FrameLen            ֡����
*           FrameBuff           ֡����

* �������: FrameEnd            ֡����ƫ��
* �� �� ֵ:  =0                  ��Ч֡
*           <0                  ��Ч֡
*******************************************************************************/
int8 taskmng_frame_3762_format_check(uint16 FrameLen, uint8 *FrameBuff, uint16 *FrameEnd)
{
    uint16  i = 0;
    uint16  DataIndex = 0;                      //ָ��ƫ��
    uint16  FrameDataLen=0;                     //֡�����ܳ���
    uint8   CheckSum = 0;                       //У���

    //����֡��ʼ��68H
    for (i = 0; i < FrameLen; i++)
    {
        if (FrameBuff[i] == 0x68)
        {
            break;
        }
        else
        {
            DataIndex++;
        }
    }

    //��Ч֡
    if (DataIndex >= FrameLen)
    {
        *FrameEnd = FrameLen;
        return -1;
    }

    //����֡���ݳ���
    memcpy(&FrameDataLen, &FrameBuff[DataIndex+1], 2);


    //�ݴ���
    if ((DataIndex + FrameDataLen) > FrameLen)
    {
        *FrameEnd = FrameLen;
        return -2;
    }

    //���֡β
    if (FrameBuff[DataIndex+FrameDataLen-1] != 0x16)
    {
        *FrameEnd = DataIndex+1;
        return -3;
    }

    //����͵ķ�ΧΪ������+�û�������
    for (i = 3; i < (DataIndex+FrameDataLen-2); i++)
    {
        CheckSum += FrameBuff[DataIndex+i];
    }
    if (FrameBuff[DataIndex+FrameDataLen-2] != CheckSum)
    {
        *FrameEnd = DataIndex+1;
        return -4;
    }

    *FrameEnd = DataIndex+FrameDataLen;

    return 0;
}

#if 0
/*******************************************************************************
* ��������: LoadHPLCChannel
* ��������://���������ļ���F2090900����ز�Ƶ�����
* �������:
* �������:
* �� �� ֵ:
*******************************************************************************/
void LoadHPLCChannel()
{
    int   ret;
    uint32 Len;
    uint8 i;
    NOMAL_OAD_T NormalOad;
    NormalOad.logicId = 0;
    NormalOad.oad.value = 0xF2090900;
    NormalOad.infoNum = 0;
    uint8 channel;
    ret = db_read_nomal(gDBClientMain,&NormalOad,1,&channel,&Len);
    if(ret!=0 || Len != 1)
    {
        TASK_FMT_DEBUG(-1,UP_LOG, "��ȡ����ز�Ƶ�����ʧ�� ret %d  \n",ret);

    }else
    {
        HPLCChannel = channel;
        for(i=0;i<PLC_NUM;i++)
        {
            ret = task_send_msg(gMainSock,MSG_HPLC_CHANNEL_CHANGE,&HPLCChannel,1,IPC_AMR+i);
            TASK_FMT_DEBUG(-1,UP_LOG, "֪ͨ�����߳����ÿ���ز�Ƶ�� %d ret %d \n",HPLCChannel,ret);
        }
    }
}
#endif
/*******************************************************************************
* ��������: taskmng_HPLC_across_area_flag_load
* ��������://���������ļ���F2090B00̨��ʶ����ͣ���
* �������:
* �������:
* �� �� ֵ:
*******************************************************************************/
void taskmng_HPLC_across_area_flag_load()
{
    int   ret;
    uint32 Len;
    uint8 i;
    NOMAL_OAD_T NormalOad;
    NormalOad.logicId = 0;
    NormalOad.oad.value = 0xF2090B00;
    NormalOad.infoNum = 0;
    uint8 flag;
    ret = db_read_nomal(gDBClientMain,&NormalOad,1,&flag,&Len);
    if(ret!=0 || Len != 1)
    {
        TASK_FMT_DEBUG(-1,UP_LOG, "��ȡ̨��ʶ����ͣ��� ret %d  \n",ret);

    }else
    {
        HPLCAcrossAreaFlag = flag;
        for(i=0;i<PLC_NUM;i++)
        {
            task_send_msg(gMainSock,MSG_HPLC_ACROSSAREA_CHANGE,&HPLCAcrossAreaFlag,1,IPC_AMR+i);
            TASK_FMT_DEBUG(-1,UP_LOG, "֪ͨ�����߳�����̨��ʶ����ͣ��� %d  \n",HPLCAcrossAreaFlag);
        }
    }
}

/*******************************************************************************
* ��������: taskmng_HPLC_node_reject_flag_load
* ��������://���������ļ���F2091600�ܾ��ڵ��ϱ�ʹ�ܱ�־
* �������:
* �������:
* �� �� ֵ:
*******************************************************************************/
void taskmng_HPLC_node_reject_flag_load()
{
    int   ret;
    uint32 Len;
    uint8 i;
    NOMAL_OAD_T NormalOad;
    NormalOad.logicId = 0;
    NormalOad.oad.value = 0xF2091600;
    NormalOad.infoNum = 0;
    uint8 flag;
    ret = db_read_nomal(gDBClientMain,&NormalOad,1,&flag,&Len);
    if(ret!=0 || Len != 1)
    {
        TASK_FMT_DEBUG(-1,UP_LOG, "��ȡ�ܾ��ڵ��ϱ�ʹ�ܱ�־ ret %d  \n",ret);

    }else
    {
        HPLCNodeRejectFlag= flag;
        for(i=0;i<PLC_NUM;i++)
        {
            task_send_msg(gMainSock,MSG_HPLC_NODEREJECT_CHANGE,&HPLCNodeRejectFlag,1,IPC_AMR+i);
            TASK_FMT_DEBUG(-1,UP_LOG, "֪ͨ�����߳����þܾ��ڵ��ϱ�ʹ�ܱ�־ %d  \n",HPLCNodeRejectFlag);
        }
    }
}

#if 0
/*******************************************************************************
* ��������: SetHPLCChannel
* ��������: //������ͨ��APP����HPLCƵ��
* �������: pipe �˿ں�
* �������:
* �� �� ֵ:
*******************************************************************************/
void SetHPLCChannel(uint8 pipe,uint8 channel)
{

    uint8 payload[20]={0};
    uint8 len = 0;
    OOP_OAD_U pipeoad = taskmng_pipe_oad_get( pipe);
    memcpy_r(&payload[len],(uint8*)&pipeoad.value,4);
    len+=4;
    payload[len++] = channel;
    uint16 recvlen;
    uint8 recvbuf[100]={0};
    taskmng_msg_fun_deal(pipe,0x0003,0x0052,len,payload,&recvlen,recvbuf);
    if(recvlen==1&&recvbuf[0]==1)
    {
        TASK_FMT_DEBUG(pipe,RELAY_LOG,"����HPLCƵ��ɹ� \n");
    }

}
#endif





/*******************************************************************************
* ��������: taskmng_search_meter_clear_param_get
* ��������: //��ȡ����ѱ���ѡ�� ���δ�����򷵻�-1
* �������:
* �������:
* �� �� ֵ:
*******************************************************************************/
int taskmng_search_meter_clear_param_get()
{
    int ret = -1;
    pthread_mutex_lock(&SearchMeterClean.lock);
    if(SearchMeterClean.isvalid ==1 )
    {
        ret = SearchMeterClean.searchparam.clearChoice;
    }
    pthread_mutex_unlock(&SearchMeterClean.lock);
    return ret;
}


/*******************************************************************************
* ��������: MeterSearchResultClear
* ��������: //����ڴ����ѱ���
* �������:
* �������:
* �� �� ֵ:
*******************************************************************************/
void taskmng_meter_search_result_clear(void)
{
    pthread_mutex_lock(&MeterSearchResult.lock);
    MeterSearchResult.count = 0;
    memset(MeterSearchResult.result,0x00,sizeof(METER_ADDR_AND_PRTL_T)*MAX_SEARCH_METER_RESULT);
    MeterSearchResult.infonum = 0;
    pthread_mutex_unlock(&MeterSearchResult.lock);
}

void taskmng_across_area_result_clear(void)
{
    pthread_mutex_lock(&AcrossAreaResult.Lock);
    AcrossAreaResult.count = 0;
    memset(AcrossAreaResult.result,0x00,sizeof(ACROSS_AERA_RES_T)*MAX_ACROSS_AREA_RESULT);
    AcrossAreaResult.infonum = 0;
    pthread_mutex_unlock(&AcrossAreaResult.Lock);
}

/*******************************************************************************
* ��������: taskmng_across_area_result_load
* ��������: //�ϵ�ʱ�������������еķǱ�̨�����ܱ��ϱ���Ϣ ��ȥ����
* �������:
* �������:
* �� �� ֵ:
*******************************************************************************/
void taskmng_across_area_result_load()
{
    int ret;
    READ_RECORD_T      inReadRecord;
    uint32             handle = 0;
    uint32             recNum = 0;
    int i;
    STEP_READ_RECORD_T stepReadRecord;
    RECORD_UNIT_T      stepRecordOut;
    OOP_ACROSSAREA_RES_T  Result;

    memset(&Result,0x00,sizeof(OOP_ACROSSAREA_RES_T));
    memset(&inReadRecord, 0, sizeof(READ_RECORD_T));
    memset(&stepReadRecord, 0, sizeof(STEP_READ_RECORD_T));
    memset(&stepRecordOut, 0, sizeof(RECORD_UNIT_T));

    TASK_FMT_DEBUG(-1,TASK_LOG, "��ʼ���طǱ�̨�����ܱ��ϱ���� \n");


    inReadRecord.recordOAD.logicId = 0;
    inReadRecord.recordOAD.infoNum = 0xffff;
    inReadRecord.cType = COL_NO_INFO;
    inReadRecord.cStart = 1;
    inReadRecord.cEnd = Search_MTR_MAX_NUM;
    //inReadRecord.cEnd = MTR_MAX_NUM;
    inReadRecord.sortType = UP_SORT;
    inReadRecord.recordOAD.road.oadMain.value = 0;
    inReadRecord.recordOAD.road.oadCols.nNum = 0x01;
    inReadRecord.recordOAD.road.oadCols.oad[0].value = 0x60020500;

    stepReadRecord.MAC.nNum = 0;
    pthread_mutex_lock(&AcrossAreaResult.Lock);
    ret = db_read_record_start(gDBClientMain, &inReadRecord, &handle, &recNum);
    if(ret!=0 || recNum==0)
    {
        TASK_FMT_DEBUG(-1,TASK_LOG, "��ʼ���Ǳ�̨�����ܱ��ϱ����ʧ�� ret  %d recNum %d \n",ret,recNum);
    }else
    {
        TASK_FMT_DEBUG(-1,TASK_LOG, "�Ǳ�̨�����ܱ��ϱ���� recNum %d \n",recNum);
        for(i=0;i<recNum;i++)
        {
            ret = db_read_record_step(gDBClientMain, handle, &stepReadRecord, &stepRecordOut);
            if(ret!=0)
            {
                TASK_FMT_DEBUG(-1,TASK_LOG, "�������Ǳ�̨�����ܱ��ϱ������¼ʧ�� ret  %d i %d \n",ret,i);
                continue;
            }
            if(stepRecordOut.recordnum!=1 || stepRecordOut.record[0].road.oadCols.oad[0].value!=0x60020500
                ||stepRecordOut.record[0].outDatalen!=sizeof(OOP_ACROSSAREA_RES_T))
            {
                TASK_FMT_DEBUG(-1,TASK_LOG, "������ȡ�ķǱ�̨�����ܱ��ϱ���������� \n");
                continue;
            }
            memcpy(&Result,stepRecordOut.record[0].outDataBuf,sizeof(OOP_ACROSSAREA_RES_T));
            if(stepRecordOut.infoNum>AcrossAreaResult.infonum)
            {
                AcrossAreaResult.infonum = stepRecordOut.infoNum;
            }
            memcpy_r(AcrossAreaResult.result[AcrossAreaResult.count].Addr,Result.commAdd.add,6);
            AcrossAreaResult.result[AcrossAreaResult.count].infonum = stepRecordOut.infoNum;
            AcrossAreaResult.count++;
            TASK_FMT_DEBUG(-1,TASK_LOG, "���صķǱ�̨�����ܱ��ϱ���¼ %d infonum %d  count %d ��ַ \n",i,stepRecordOut.infoNum,AcrossAreaResult.count);
            TASK_BUF_DEBUG(-1,TASK_LOG, Result.commAdd.add,6);
        }
    }
    db_read_record_end(gDBClientMain, handle);
    pthread_mutex_unlock(&AcrossAreaResult.Lock);
}

/*******************************************************************************
* ��������: CheckAcrossAreaResult
* ��������: //����ϱ��ķǱ�̨�������Ϣ�Ƿ��ظ�������ظ�����false���δ�ظ���д��AcrossAreaResult
* �������:
* �������: ���δ�ظ� �򷵻���Ϣ���
* �� �� ֵ:
*******************************************************************************/
BOOL CheckAcrossAreaResult(uint8 *Addr,uint16 *infonum)
{
    uint16 i;
    pthread_mutex_lock(&AcrossAreaResult.Lock);
    for(i=0;i<AcrossAreaResult.count;i++)
    {
        if(memcmp(AcrossAreaResult.result[i].Addr,Addr,6)==0)
        {
            pthread_mutex_unlock(&AcrossAreaResult.Lock);
            return FALSE;
        }
    }
    AcrossAreaResult.infonum++;
    memcpy(AcrossAreaResult.result[AcrossAreaResult.count].Addr,Addr,6);
    *infonum = AcrossAreaResult.infonum;
    AcrossAreaResult.result[AcrossAreaResult.count].infonum = *infonum;
    AcrossAreaResult.count++;
    pthread_mutex_unlock(&AcrossAreaResult.Lock);
    return TRUE;
}

/*******************************************************************************
* ��������: taskmng_across_area_result_change_fun
* ��������: //�յ�60020500���ݱ���¼�����ѯ�������ģ��͵�ǰ�ڴ����ݽ��бȽϣ���������ɾ��
* �������:
* �������: ���δ�ظ� �򷵻���Ϣ���
* �� �� ֵ:
*******************************************************************************/
void taskmng_across_area_result_change_fun(uint16 infonum)
{
    int ret;
    int i;
    int index = -1;
    OOP_ACROSSAREA_RES_T result;
    memset(&result,0x00,sizeof(OOP_ACROSSAREA_RES_T));
    TASK_FMT_DEBUG(-1,TASK_LOG, "�յ�60020500���ݱ���¼� infonum %d  \n",infonum);
    if(infonum == 0xffff)
    {
        pthread_mutex_lock(&AcrossAreaResult.Lock);
        AcrossAreaResult.count = 0;
        memset(AcrossAreaResult.result,0x00,sizeof(ACROSS_AERA_RES_T)*MAX_ACROSS_AREA_RESULT);
        AcrossAreaResult.infonum = 0;
        pthread_mutex_unlock(&AcrossAreaResult.Lock);
    }else
    {
        ret = taskmng_across_area_result_read_from_pn(infonum,&result);
        TASK_FMT_DEBUG(-1,TASK_LOG, "taskmng_across_area_result_read_from_pn ret %d  \n",ret);
        pthread_mutex_lock(&AcrossAreaResult.Lock);
        if(ret == -1)
        {
            //���ɾ��
            for(i=0;i<AcrossAreaResult.count;i++)
            {
                if(infonum == AcrossAreaResult.result[i].infonum)
                {
                    index = i;
                    break;
                }
            }
            if(index != -1)
            {
                for(i=index;i<AcrossAreaResult.count-1;i++)
                {
                    AcrossAreaResult.result[i] = AcrossAreaResult.result[i+1];
                }
                memset(&(AcrossAreaResult.result[AcrossAreaResult.count-1]),0,sizeof(ACROSS_AERA_RES_T));
                AcrossAreaResult.count--;
            }
        }else
        {
            //�忴�Ƿ��ڵ�ǰ�ڴ�����
            for(i=0;i<AcrossAreaResult.count;i++)
            {
                if(infonum ==AcrossAreaResult.result[i].infonum )
                {
                    index = i;
                    break;
                }
            }
            if(index == -1)
            {
                memcpy_r(AcrossAreaResult.result[AcrossAreaResult.count].Addr,result.commAdd.add,6);
                AcrossAreaResult.result[AcrossAreaResult.count].infonum = infonum;
                AcrossAreaResult.count++;
                if(infonum>AcrossAreaResult.infonum)
                {
                    AcrossAreaResult.infonum = infonum;
                }
            }
        }
        pthread_mutex_unlock(&AcrossAreaResult.Lock);
    }

}

/*******************************************************************************
* ��������: taskmng_across_area_result_read_from_pn
* ��������: ������Ϣ��Ŵ��������Ķ�ȡ�ѱ�̨�������
* �������:
            infonum

* �������: result
* �� �� ֵ:  0 �ɹ�

*******************************************************************************/
int taskmng_across_area_result_read_from_pn(uint16 infonum,OOP_ACROSSAREA_RES_T *result)
{
    int   ret;
    uint32 len=0;
    NOMAL_OAD_T NormalOad;
    NormalOad.logicId = 0;
    NormalOad.oad.value = 0x60020500;
    NormalOad.infoNum = infonum;

    ret = db_read_nomal(gDBClientMain,&NormalOad,sizeof(OOP_ACROSSAREA_RES_T),(uint8*)result,&len);

    if(ret !=0 ||len!=sizeof(OOP_ACROSSAREA_RES_T))
        return -1 ;

    return 0;
}

/*******************************************************************************
* ��������: taskmng_meter_search_result_read_from_pn
* ��������: ������Ϣ��Ŵ��������Ķ�ȡ�ѱ���
* �������:
            infonum

* �������: result
* �� �� ֵ:  0 �ɹ�

*******************************************************************************/
int taskmng_meter_search_result_read_from_pn(uint16 infonum,OOP_SEARCHMET_RES_T *result)
{
    int   ret;
    uint32 len=0;
    NOMAL_OAD_T NormalOad;
    NormalOad.logicId = 0;
    NormalOad.oad.value = 0x60020200;
    NormalOad.infoNum = infonum;

    ret = db_read_nomal(gDBClientMain,&NormalOad,sizeof(OOP_SEARCHMET_RES_T),(uint8*)result,&len);

    if(ret !=0 ||len!=sizeof(OOP_SEARCHMET_RES_T))
        return -1 ;

    return 0;
}

/*******************************************************************************
* ��������: taskmng_meter_search_result_change_fun
* ��������: //�յ�60020200���ݱ���¼�����ѯ�������ģ��͵�ǰ�ڴ����ݽ��бȽϣ���������ɾ��
* �������:
* �������: ���δ�ظ� �򷵻���Ϣ���
* �� �� ֵ:
*******************************************************************************/
void taskmng_meter_search_result_change_fun(uint16 infonum)
{
    int ret;
    int i;
    int index = -1;
    OOP_SEARCHMET_RES_T result;
    memset(&result,0x00,sizeof(OOP_SEARCHMET_RES_T));
    TASK_FMT_DEBUG(-1,TASK_LOG, "�յ�60020200�ѱ������ݱ���¼� infonum %d  \n",infonum);
    if(infonum == 0xffff)
    {
        taskmng_meter_search_result_clear();
    }else
    {
        ret = taskmng_meter_search_result_read_from_pn(infonum,&result);
        TASK_FMT_DEBUG(-1,TASK_LOG, "taskmng_meter_search_result_read_from_pn ret %d  \n",ret);
        pthread_mutex_lock(&MeterSearchResult.lock);
        if(ret == -1)
        {
            //���ɾ��
            for(i=0;i<MeterSearchResult.count;i++)
            {
                if(infonum == MeterSearchResult.result[i].infonum)
                {
                    index = i;
                    break;
                }
            }
            if(index != -1)
            {
                for(i=index;i<MeterSearchResult.count-1;i++)
                {
                    MeterSearchResult.result[i] = MeterSearchResult.result[i+1];
                }
                memset(&(MeterSearchResult.result[MeterSearchResult.count-1]),0,sizeof(METER_ADDR_AND_PRTL_T));
                MeterSearchResult.count--;
            }
        }else
        {
            //�鿴�Ƿ��ڵ�ǰ�ڴ�����
            for(i=0;i<MeterSearchResult.count;i++)
            {
                if(infonum ==MeterSearchResult.result[i].infonum )
                {
                    index = i;
                    break;
                }
            }
            if(index == -1)
            {
                memcpy_r(MeterSearchResult.result[MeterSearchResult.count].Addr,result.commAdd.add,6);
                MeterSearchResult.result[MeterSearchResult.count].prtl = result.proto;
                MeterSearchResult.result[MeterSearchResult.count].infonum = infonum;
                MeterSearchResult.count++;
                if(infonum>MeterSearchResult.infonum)
                {
                    MeterSearchResult.infonum = infonum;
                }
            }
        }
        pthread_mutex_unlock(&MeterSearchResult.lock);
    }

}






/**
*********************************************************************
* @brief�� get_app_id ��ȡϵͳ������appid
* @param[in]��
               argc      ��������
               argv[]    �����б�
* @param[out]��
               none        ����˵��
* @return�� ����appid����
*********************************************************************
*/
int get_app_id(uint8* appid,int maxlen)
{
    int appidlen;
    const char* appname = "taskManager";
    DEVINFO_DEVICE_T* dev = (DEVINFO_DEVICE_T*) hal_device_get(HW_DEVICE_ID_DEVINFO);
    if(NULL == dev)
    {
        appidlen = 24;
        memset(appid,0x00,appidlen);
        TASK_FMT_DEBUG(-1, UP_LOG, "devinfo hal_device_get error \n");

    }else
    {
        if(dev->devinfo_read_appid(dev,appname,(char*)appid,maxlen) < 0)
        {
            TASK_FMT_DEBUG(-1, UP_LOG, "devinfo devinfo_read_appid error \n");
            appidlen = 24;
            memset(appid,0x00,appidlen);
        }else
        {
            appidlen = 24;
        }

        hal_device_release((HW_DEVICE *)dev);
    }
    return appidlen;
}

/*******************************************************************************
* ��������: Appid_Response
* ��������:�յ�iid 0000 IOP0016��ظ�appid
* �������:
* �������:
* �� �� ֵ:
*******************************************************************************/
void Appid_Response(MESSAGE_INFO_T *MsgRecv )
{
    uint8 payload[100]={0};
    uint8 index = 0;
    int appidlen;
    uint8 appid[MAX_APPID_LEN]={0};
    appidlen = get_app_id(appid, MAX_APPID_LEN);
    payload[index++] = (uint8)appidlen;
    memcpy(&payload[index],appid,appidlen);
    index+=appidlen;

    MESSAGE_INFO_T *MsgSend = (MESSAGE_INFO_T*)malloc(sizeof(MESSAGE_INFO_T)+index);
    memset(MsgSend,0x00,sizeof(MESSAGE_INFO_T)+index);
    MsgSend->rpm = 0;
    MsgSend->priority = MsgRecv->priority;
    MsgSend->index = MsgRecv->index;
    MsgSend->label = MsgRecv->label;
    MsgSend->sourlen = strlen(APPNAME);
    memset(MsgSend->souraddr,0x00,sizeof(MsgSend->souraddr));
    memcpy(MsgSend->souraddr,APPNAME,MsgSend->sourlen);
    MsgSend->destlen = MsgRecv->sourlen;
    memset(MsgSend->destaddr,0x00,sizeof(MsgSend->destaddr));
    memcpy(MsgSend->destaddr,MsgRecv->souraddr,MsgSend->destlen );
    TASK_FMT_DEBUG(-1,UP_LOG, "��ѯ����APPID SourAddr %s DestAddr %s \n",MsgSend->souraddr,MsgSend->destaddr);
    MsgSend->IID = MsgRecv->IID;
    MsgSend->IOP = MsgRecv->IOP;
    MsgSend->msglen = index;
    memcpy(MsgSend->playload,payload,index);
    TASK_FMT_DEBUG(-1, UP_LOG, "Send IID=0X%04x IOP=0x%04x MsgIndex %d \n",MsgSend->IID,MsgSend->IOP,MsgSend->index);

    TASK_BUF_DEBUG(-1, UP_LOG, payload, index);
    taskmng_plc_queue_write ( APPArg.SendBuf, (uint8*)MsgSend, sizeof(MESSAGE_INFO_T)+index );
    free(MsgSend);

}




/*******************************************************************************
* ��������: taskmng_db_update_queue_read
* ��������: ������
* ���������pLockQueue          ���������ṹָ��
*           buf                 �����ݻ��������������е�����ȡ������û�����
* ���������plockqueue          �����е�����
* �ڲ�����: �ж������Ƿ������ݣ�����Ӷ�����ȡ�����ݷ���buf�У�ÿִ��һ�Σ�ȡһ������
* �� �� ֵ: len                 ���������ݳ���
*******************************************************************************/
int32 taskmng_db_update_queue_read(DBUPDATE_QUEUE_T *pLockQueue, DB_UPDATE_T *buf)
{
    int32 len = 0;
    //uint8 *pdata_buf;

    //��������
    pthread_mutex_lock(&pLockQueue->QueueLock);

    //�������������ݣ���ȡ��buf�У��⿪��
    if ((pLockQueue->QueueHead != pLockQueue->QueueTail)
        || ((pLockQueue->QueueHead == pLockQueue->QueueTail)
            && (pLockQueue->Queue[pLockQueue->QueueHead].isvalid != 0)))
    {
        //pdata_buf =&(pLockQueue->Queue[pLockQueue->QueueHead].dbupdate);
        len = sizeof(DB_UPDATE_T);

        //ȡ�����е�����
        memcpy(buf, &(pLockQueue->Queue[pLockQueue->QueueHead].dbupdate), len);

        //������
        memset(&(pLockQueue->Queue[pLockQueue->QueueHead].dbupdate), 0, len);
        pLockQueue->Queue[pLockQueue->QueueHead].isvalid = 0;

        //����ͷ����ƶ�һλ
        pLockQueue->QueueHead= (pLockQueue->QueueHead + 1) % (MAX_DBUPDATE_QUEUE_CNT);
    }

    pthread_mutex_unlock(&pLockQueue->QueueLock);

    return len;
}

/*******************************************************************************
* ��������: taskmng_db_update_queue_write
* ��������: д����
* ���������plockqueue          ���������ṹָ��
            buf                 ��Ҫд����е�����
            buf_len             ���ݳ���
* �����������
* �ڲ�����: �����в�������buf�е����ݷ�������У���������������������
* �� �� ֵ: >0                  �ɹ�
            =0                  ʧ��
*******************************************************************************/
int32 taskmng_db_update_queue_write(DBUPDATE_QUEUE_T *pLockQueue, DB_UPDATE_T *buf)
{
    int32 flags = 0;

    //��������
    pthread_mutex_lock(&pLockQueue->QueueLock);
    //�ж������Ƿ��пյ�λ��
    if (pLockQueue->Queue[pLockQueue->QueueTail].isvalid == 0)
    {
        pLockQueue->Queue[pLockQueue->QueueTail].isvalid = 1;
        memcpy(&(pLockQueue->Queue[pLockQueue->QueueTail].dbupdate), buf, sizeof(DB_UPDATE_T));
        flags = 1;
        pLockQueue->QueueTail = (pLockQueue->QueueTail + 1) % (MAX_DBUPDATE_QUEUE_CNT);
    }

    //�⻺����
    pthread_mutex_unlock(&pLockQueue->QueueLock);

    return flags;
}





/*******************************************************************************
* ��������: ReNetworking
* ��������: //��ѯ�߳��е�ÿ����� ���ĳ����һֱ����ʧ����ӵ�������
* �������: pipe �˿ں�

* �������:   pHead ��Ҫ���������ĵ������
* �� �� ֵ: 0 ����Ҫ�������� 1 ��Ҫ
*******************************************************************************/
int IsNeedReNetworking(uint8 pipe)
{
    AmrThreadArg_T  *pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    MtrAmrInfo_T *pMeterInfo = NULL;
    uint16 i;
    uint8 Addr[6]={0};
    uint8 prtl;
    uint8 flag = 0;
    for(i=0;i<pAmrThreadArg->TmnAmrInfo.MtrCnt;i++)
    {
        pMeterInfo = &pAmrThreadArg->pMtrAmrInfo[i];
        //TASK_FMT_TRACE( pipe, RELAY_LOG, "��� %d ��ʧ�ܴ��� %d ������������ %d LastRecvTime %ld \n",
                 //   pMeterInfo->Pn,pMeterInfo->AllFailCnt,pMeterInfo->ReNetworkingCount,pMeterInfo->LastRecvTime);
        if(pMeterInfo->AllFailCnt>=MAX_METER_ALL_FAILURE_COUNT &&time(NULL)-pMeterInfo->LastRecvTime>MAX_METER_ALL_FAILURE_TIME)
        {
            if(pMeterInfo->ReNetworkingCount<1&&time(NULL)-pMeterInfo->LastReNetwokingTime>MAX_METER_ALL_FAILURE_TIME)
            {
                TASK_FMT_TRACE( pipe, RELAY_LOG, "��� %d ��ʧ�ܴ��� %d ������������ %d  ��Ҫ��������\n",
                    pMeterInfo->Pn,pMeterInfo->AllFailCnt,pMeterInfo->ReNetworkingCount);
                taskmng_mtr_addr_prtl_get(pMeterInfo->Pn,LOGIC_ID_YC,Addr,&prtl);
                pAmrThreadArg->pReNetworkingDelHead= taskmng_module_head_node_add(pMeterInfo->Pn,Addr,prtl,pAmrThreadArg->pReNetworkingDelHead);
                pAmrThreadArg->pReNetworkingAddHead= taskmng_module_head_node_add(pMeterInfo->Pn,Addr,prtl,pAmrThreadArg->pReNetworkingAddHead);
                flag = 1;
                pMeterInfo->ReNetworkingCount++;
                pMeterInfo->LastReNetwokingTime = time(NULL);
            }
        }

    }
    if(flag == 1)
        return 1;
    else
        return 0;
}

/*******************************************************************************
* ��������: ReNetworking
* ��������: //��ѯ�߳��е�ÿ����� ���ĳ����һֱ����ʧ����ɾ��ccoRouter�ĵ������������ ������������
* �������: pipe �˿ں�

* �������:
* �� �� ֵ:
*******************************************************************************/
int ReNetworking(uint8 pipe)
{
    AmrThreadArg_T  *pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    int ret;
    switch(pAmrThreadArg->TotalState.ReNetworkingState)
    {
        //�ж��Ƿ���Ҫ��������
        case MFMS_IDLE:
        {
            if(IsNeedReNetworking(pipe)>0)
            {
                pAmrThreadArg->TotalState.ReNetworkingState = MFMS_FILE_DELETE;
            }else
            {
                TASK_FMT_TRACE( pipe, RELAY_LOG,"������������ \n" );
                return 1;
            }
        }
            break;
        //��ɾ������
        case MFMS_FILE_DELETE:
        {

            if(pAmrThreadArg->pReNetworkingDelHead == NULL)
            {
                pAmrThreadArg->TotalState.ReNetworkingState = MFMS_PAUSE;
                pAmrThreadArg->ReNetworkingDelay = 10;  //��ʱ10�����ӵ���
            }else
            {
                TASK_FMT_DEBUG( pipe, RELAY_LOG, "ReNetworking ����ɾ��  \n");
                ret = taskmng_router_info_delete(pipe,pAmrThreadArg->pReNetworkingDelHead,MODULEDEAL_CNT);
                if(ret>0)
                {
                    pAmrThreadArg->pReNetworkingDelHead = taskmng_module_head_node_delete(pAmrThreadArg->pReNetworkingDelHead, MODULEDEAL_CNT);
                }
            }
        }
            break;
        case MFMS_PAUSE:
        {
            if(taskmng_delay_deal(pipe,&pAmrThreadArg->ReNetworkingDelay)>0)
            {
                pAmrThreadArg->TotalState.ReNetworkingState = MFMS_FILE_ADD;
            }
        }
            break;
        //����ӵ���
        case MFMS_FILE_ADD:
        {

            if(pAmrThreadArg->pReNetworkingAddHead == NULL)
            {
                pAmrThreadArg->TotalState.ReNetworkingState = MFMS_IDLE;
                TASK_FMT_DEBUG( pipe, RELAY_LOG,  "ReNetworkingִ�н���  \n");
                return 1;
            }else
            {
                TASK_FMT_DEBUG( pipe, RELAY_LOG,  "ReNetworking �����·�  \n");
                ret = taskmng_slave_info_add(pipe, pAmrThreadArg->pReNetworkingAddHead, MODULEDEAL_CNT);
                if(ret>0)
                {
                    pAmrThreadArg->pReNetworkingAddHead = taskmng_module_head_node_delete(pAmrThreadArg->pReNetworkingAddHead, MODULEDEAL_CNT);
                }
            }
        }
            break;
        default:
            pAmrThreadArg->TotalState.ReNetworkingState = MFMS_IDLE;
            break;
    }
    return -1;
}



/**********************************************************************
���� taskmng_curve_point_get
���� ���㸺�����߳�������
����  ��
����ֵ: �������߳�������
***********************************************************************/
uint8 taskmng_curve_point_get(uint16 taskid,uint8 logicid)
{
    //Ĭ�ϳ�8����
    uint8 point=8;

    uint32 period = taskmng_task_period_get(taskid, logicid);
    if(period<(60*15))
    {
        return point;
    }
    point = period/(60*15);
    if(point>8)
        point = 8;
    return point;

}
/*******************************************************************************
* ��������: taskmng_record_file_delete_pipe
* ��������:�յ���������̷߳��͵���������Ϣ��ɾ�������¼�ļ�
* �������:pipe  �˿�
        TaskId ����id
* �������:
* �� �� ֵ:
*******************************************************************************/
void taskmng_rs485_patch_file_delete_by_pn(uint16 metrno)
{
    //AmrThreadArg_T  *pAmrThreadArg;
    //pAmrThreadArg = &gAmrThreadArg[pipe];
    //uint16 i=0,j;
    char  pfile[100];
    uint8 pipe;
    //int ret;
    OopCrvAddReadTime_t OopCrvAddReadTime[OOP_CRV_ADDREAD_TASKNUM];
    OopCrvAddReadTime_t OopDayAddReadTime[OOP_DAY_ADDREAD_TASKNUM];

    pipe= taskmng_pipe_get_by_pn(metrno);
    if((pipe<2)||(pipe==0xFF))
    {
        return;
    }
    memset(OopCrvAddReadTime,0,sizeof(OopCrvAddReadTime_t)*OOP_CRV_ADDREAD_TASKNUM);
    memset(OopDayAddReadTime,0,sizeof(OopCrvAddReadTime_t)*OOP_DAY_ADDREAD_TASKNUM);
    //for(i=0;i<pAmrThreadArg->TmnAmrInfo.MtrCnt;i++)
    {
         sprintf(pfile,"%s/%s_%02d_MtrNo%02d",PrivateDir,PATCH_FILE,1,metrno);
         //ret=read_pdata_xram(pfile,(char *)&OopDayAddReadTime,0,sizeof(OopCrvAddReadTime_t)*OOP_DAY_ADDREAD_TASKNUM);
        // if(ret >= 0)
         {
             //for(j = 0; j < OOP_CRV_ADDREAD_TASKNUM; j++)
             {
                //if(OopDayAddReadTime[j].TaskId == TaskId)
                {
                    //OopDayAddReadTime[j].AddReadTime = 0;
                    write_pdata_xram(pfile,(char *)&OopDayAddReadTime,0,sizeof(OopCrvAddReadTime_t)*OOP_CRV_ADDREAD_TASKNUM);
                    TASK_FMT_TRACE( pipe, REC_LOG, "ɾ��metrno %ld �ն�������\n",metrno);
                }
             }
         }

         sprintf(pfile,"%s/%s_%02d_MtrNo%02d",PrivateDir,PATCH_FILE,2,metrno);
         //ret=read_pdata_xram(pfile,(char *)&OopCrvAddReadTime,0,sizeof(OopCrvAddReadTime_t)*OOP_DAY_ADDREAD_TASKNUM);
         //if(ret >= 0)
         {
             //for(j = 0; j < OOP_DAY_ADDREAD_TASKNUM; j++)
             {
                //if(OopCrvAddReadTime[j].TaskId == TaskId)
                {
                    //OopCrvAddReadTime[j].AddReadTime = 0;
                    write_pdata_xram(pfile,(char *)&OopCrvAddReadTime,0,sizeof(OopCrvAddReadTime_t)*OOP_DAY_ADDREAD_TASKNUM);
                    TASK_FMT_TRACE( pipe, REC_LOG, "ɾ��metrno %ld �����߲�������\n",metrno);

                }
             }
         }

    }
    //write_pdata_xram(pfile,(char *)&OopDayAddReadTime,0,sizeof(OopCrvAddReadTime_t)*OOP_DAY_ADDREAD_TASKNUM);
    //write_pdata_xram(pfile,(char *)&OopCrvAddReadTime,0,sizeof(OopCrvAddReadTime_t)*OOP_CRV_ADDREAD_TASKNUM);
}


/*******************************************************************************
* ��������: taskmng_record_file_delete_pipe
* ��������:�յ���������̷߳��͵���������Ϣ��ɾ�������¼�ļ�
* �������:pipe  �˿�
        TaskId ����id
* �������:
* �� �� ֵ:
*******************************************************************************/
void taskmng_rs485_delete_all_patch_file(void)
{
    //AmrThreadArg_T  *pAmrThreadArg;
    //pAmrThreadArg = &gAmrThreadArg[pipe];
    uint16 i=0;
    char  pfile[100];
    uint8 pipe;
    //int ret;
    OopCrvAddReadTime_t OopCrvAddReadTime[OOP_CRV_ADDREAD_TASKNUM];
    OopCrvAddReadTime_t OopDayAddReadTime[OOP_DAY_ADDREAD_TASKNUM];

    memset(OopCrvAddReadTime,0,sizeof(OopCrvAddReadTime_t)*OOP_CRV_ADDREAD_TASKNUM);
    memset(OopDayAddReadTime,0,sizeof(OopCrvAddReadTime_t)*OOP_DAY_ADDREAD_TASKNUM);
    pthread_rwlock_rdlock(&MeterFileAll.rwLock);
    for(i=0;i<MeterFileAll.MeterCount;i++)
    {
        pipe = taskmng_mtr_pipe_get(&MeterFileAll.MeterFile[i].basic.port);
        if(pipe<2)
        {
            continue;
        }

        sprintf(pfile,"%s/%s_%02d_MtrNo%02d",PrivateDir,PATCH_FILE,1,MeterFileAll.MeterFile[i].nIndex);
        write_pdata_xram(pfile,(char *)&OopDayAddReadTime,0,sizeof(OopCrvAddReadTime_t)*OOP_DAY_ADDREAD_TASKNUM);
        TASK_FMT_TRACE( pipe, REC_LOG, "ɾ��metrno %ld �ն�������\n",MeterFileAll.MeterFile[i].nIndex);


        sprintf(pfile,"%s/%s_%02d_MtrNo%02d",PrivateDir,PATCH_FILE,2,MeterFileAll.MeterFile[i].nIndex);

        write_pdata_xram(pfile,(char *)&OopCrvAddReadTime,0,sizeof(OopCrvAddReadTime_t)*OOP_CRV_ADDREAD_TASKNUM);
        TASK_FMT_TRACE( pipe, REC_LOG, "ɾ��metrno %ld �����߲�������\n",MeterFileAll.MeterFile[i].nIndex);
    }
    pthread_rwlock_unlock(&MeterFileAll.rwLock);


}

/*******************************************************************************
* ��������: taskmng_energy_oad_check
* ��������: �ж�0ad�Ƿ����ڵ�������
* �������: oad Ҫ������������
* �������:
* �� �� ֵ: true ����
        FALSE ������
*******************************************************************************/

//BOOL taskmng_energy_oad_check(OOP_OAD_U oad)
//{
//    uint8 OIA1;
//    //����ʱ��ҲҪ�ö�������
//    OIA1=(oad.nObjID>>12)&0x0F;
//    if(OIA1 == 0)
//        return TRUE;
//    return FALSE;
//}
/*******************************************************************************
* ��������: PackSetTimeFrame_cipher
* ��������://��װУʱ���� �㲥����
* �������:delay ����ʱ����Ҫ���ϵ���ʱ
        isBroad�Ƿ�㲥
* �������:
* �� �� ֵ:  1�з��� 0 û��
*******************************************************************************/
uint16 PackSetTimeFrame_cipher(uint8 pipe,uint8* outBuf,uint16 delay)
{
    uint16 len=0;
    time_t timenow = time(NULL);
    timenow+=delay;

    LinkLayer_t *pLinkLayer = NULL;
    uint8 buf[100]={0};
    uint16 index=0;
    OOP_TSA_T Tsa;
    memset(&Tsa,0x00,sizeof(OOP_TSA_T));
    DateTimeHex_t TimeHex;

    Tsa.af = 0xC0;
    Tsa.add[0] = 0xAA;
    Tsa.oldvxd = 1;

    DT_Time2DateTimeHex(timenow,&TimeHex);
    buf[index++] = 0x40;
    buf[index++] = 0x00;
    buf[index++] = 0x7f;
    buf[index++] = 0x00;
    buf[index++] = DT_DATETIME_S;
    memcpy(&buf[index],&TimeHex,7);
    index+=7;
    OopAction.requst(pipe,&Tsa,4,buf,index);
    pLinkLayer = FindLinkLayer(pipe, NULL);
    if(pLinkLayer!=NULL)
    {
        len = pLinkLayer->sendLen;
        memcpy(outBuf,pLinkLayer->sendBuff,len);
    }
    TASK_FMT_TRACE( pipe, RELAY_LOG, "Уʱ���� %d\n",len);

    return len;
}


/*******************************************************************************
* ��������: SendBroadTimingCipher
* ��������://���͹㲥Уʱ ����
* �������:
* �������:
* �� �� ֵ:
*******************************************************************************/
int SendBroadTimingCipher(uint8 pipe)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    uint8 buf[200]={0};
    uint16 len=0;

    len = PackSetTimeFrame_cipher(pipe,buf,pAmrThreadArg->BroadTiming.BroadDelay);
    if(len>0)
    {

        taskmng_data_broad(pipe,buf,len,PRTL_OOP,&pAmrThreadArg->BroadTiming.FrameIndex);
        return 1;
    }
    return 0;
}
/*******************************************************************************
* ��������: SendBroadTimingCipher
* ��������://���͹㲥Уʱ ����
* �������:
* �������:
* �� �� ֵ:
*******************************************************************************/
int Rs485SendBroadTimingCipher(uint8 pipe)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    uint8 buf[200]={0};
    uint16 len=0;
    int ret;
    uint8 i,speed;
    len = PackSetTimeFrame_cipher(pipe,buf,pAmrThreadArg->BroadTiming.BroadDelay);
    if(len>0)
    {

        for(i=0;i<gSpeedList.num;i++)
        {
            speed = gSpeedList.speed[i];
            TASK_FMT_TRACE( pipe, RELAY_LOG, "�㲥Уʱ���� speed %d\n",speed);
            TASK_BUF_TRACE(pipe, RELAY_LOG, buf, len);
            ret=rs485_RS232_send(pipe,MAX_FRAME_OVERTIME,MAX_CHAR_OVERTIME,speed,PARITY_EVEN,buf, len);
            if(ret !=0)
            {
                TASK_FMT_TRACE( pipe, RELAY_LOG, "���ͽ�� ret is %d\n",ret);
            }
            sleep(3);
        }
//
//
//        ret=rs485_RS232_send(pipe,0,10,PARITY_EVEN,buf, len);
//        if(ret !=0)
//        {
//            TASK_FMT_TRACE( pipe, COMM_LOG, "���ͽ�� ret is %d\n",ret);
//        }
//        return 1;
    }
    return 0;
}

#ifdef PRODUCT_ECU
/*******************************************************************************
* ��������: taskmng_all_module_info_get
* ��������: ��ȡ����ģ����Ϣ
* �������:
* �������:
* �� �� ֵ:
*******************************************************************************/
void taskmng_all_module_info_get()
{
    int ret;
    uint16 IID = 0x0004;
    uint16 IOP = 0x0016;
    uint16 len = 0;
    uint8 PayLoad[200] = {0};
    uint16 RecvLen;
    uint8 RecvBuf[MAX_MSG_LEN]={0};
    char* DestAddr = "M-mapManager";
    uint16 index = 0;
    uint8 i,j;
    uint8 lenoffset = 0;
    uint16 lentmp;
    OOP_MODULE_INFO_NEW_T moduleinfo;
    uint8 num;
    OOP_MODULE_UINT_NEW_T *tmp = NULL;
    memset(&moduleinfo,0x00,sizeof(OOP_MODULE_INFO_NEW_T));
    ret = taskmng_up_msg_fun_deal(IID,IOP,len,PayLoad,DestAddr,&RecvLen,RecvBuf);
    if(ret >0)
    {
        num = RecvBuf[index++];
        for(i=0;i<num;i++)
        {
            if(RecvBuf[index++]!=0)
                continue;
            tmp = &moduleinfo.item[moduleinfo.nNum];
            tmp->nSlot = i+1;

            lentmp = get_len_offset(&RecvBuf[index], &lenoffset);
            index+=lenoffset;
            memcpy(tmp->modType.value,&RecvBuf[index],lentmp);
            tmp->modType.nNum = lentmp;
            index+=lentmp;

            lentmp = get_len_offset(&RecvBuf[index], &lenoffset);
            index+=lenoffset;
            memcpy(tmp->modId.value,&RecvBuf[index],lentmp);
            tmp->modId.nNum = lentmp;
            index+=lentmp;

            lentmp = get_len_offset(&RecvBuf[index], &lenoffset);
            index+=lenoffset;
            memcpy(tmp->nSoftVer.value,&RecvBuf[index],lentmp);
            tmp->nSoftVer.nNum = lentmp;
            index+=lentmp;

            lentmp = get_len_offset(&RecvBuf[index], &lenoffset);
            index+=lenoffset;
            memcpy(tmp->softDate.value,&RecvBuf[index],lentmp);
            tmp->softDate.nNum = lentmp;
            index+=lentmp;

            lentmp = get_len_offset(&RecvBuf[index], &lenoffset);
            index+=lenoffset;
            memcpy(tmp->nHardVer.value,&RecvBuf[index],lentmp);
            tmp->nHardVer.nNum = lentmp;
            index+=lentmp;

            lentmp = get_len_offset(&RecvBuf[index], &lenoffset);
            index+=lenoffset;
            memcpy(tmp->hardDate.value,&RecvBuf[index],lentmp);
            tmp->hardDate.nNum = lentmp;
            index+=lentmp;

            lentmp = get_len_offset(&RecvBuf[index], &lenoffset);
            index+=lenoffset;
            memcpy(tmp->manuf.value,&RecvBuf[index],lentmp);
            tmp->manuf.nNum = lentmp;
            index+=lentmp;

            tmp->nChNum = RecvBuf[index++];

            TASK_FMT_DEBUG(-1,UP_LOG,"��ȡ��ģ����Ϣ ��λ�� %d ģ���豸�ͺ� %s �豸ID %s \n",tmp->nSlot,tmp->modType.value,tmp->modId.value);
            TASK_FMT_DEBUG(-1,UP_LOG,"����汾 %s �������� %s \n",tmp->nSoftVer.value,tmp->softDate.value);
            TASK_FMT_DEBUG(-1,UP_LOG,"Ӳ���汾 %s �������� %s \n",tmp->nHardVer.value,tmp->hardDate.value);
            TASK_FMT_DEBUG(-1,UP_LOG,"���̱��� %s ͨ������ %d \n",tmp->manuf.value,tmp->nChNum);

            for(j=0;j<tmp->nChNum;j++)
            {
                tmp->chInfo[j].nType = RecvBuf[index++];
                tmp->chInfo[j].nFunc = RecvBuf[index++];
                lentmp = get_len_offset(&RecvBuf[index], &lenoffset);
                index+=lenoffset;
                memcpy(tmp->chInfo[j].nDriveName.value,&RecvBuf[index],lentmp);
                tmp->chInfo[j].nDriveName.nNum = lentmp;
                index+=lentmp;

                TASK_FMT_DEBUG(-1,UP_LOG,"ͨ�� %d �ӿ����� %d �������� %d �������� %s\n",j,tmp->chInfo[j].nType,tmp->chInfo[j].nFunc,tmp->chInfo[j].nDriveName.value);
            }
            moduleinfo.nNum++;
        }

        pthread_mutex_lock(&gModuleInfoLock);
        memcpy(&gModuleInfo,&moduleinfo,sizeof(OOP_MODULE_INFO_NEW_T));
        pthread_mutex_unlock(&gModuleInfoLock);

    }
    else
    {
        TASK_FMT_DEBUG(-1,UP_LOG,"��ȡģ����Ϣʧ�ܣ�\n");
    }

}

/*******************************************************************************
* ��������: ModuleEventFun
* ��������: //�յ�ģ�����¼���֪ͨ���߳�
* �������: buf ��Ϣ����
            len ��Ϣ����
* �������:
* �� �� ֵ:
*******************************************************************************/
void ModuleEventFun(uint8* buf,uint16 len)
{

    uint32 ModuleIndex=0;
    uint16 index=0;
    uint8 status;
    uint8 j;
    //ģ�����
    memcpy_r((uint8*)&ModuleIndex,&buf[index],4);
    index+=4;
    status = buf[index++];//���״̬ 0�� 1��
    TASK_FMT_DEBUG(-1,UP_LOG,"�յ�ģ�����¼� ģ����� %d status %d 0�� 1��\n",ModuleIndex,status);
    if(ModuleIndex<1||ModuleIndex>5)
        return ;

    if(status == 0)
    {
        //����ģ����Ϣ
        OOP_MODULE_UINT_NEW_T unit;
        uint8 lenoffset = 0;
        uint16 lentmp;

        memset(&unit,0x00,sizeof(OOP_MODULE_UINT_NEW_T));
        unit.nSlot = ModuleIndex;

        lentmp = get_len_offset(&buf[index], &lenoffset);
        index+=lenoffset;
        memcpy(unit.modType.value,&buf[index],lentmp);
        unit.modType.nNum = lentmp;
        index+=lentmp;

        lentmp = get_len_offset(&buf[index], &lenoffset);
        index+=lenoffset;
        memcpy(unit.modId.value,&buf[index],lentmp);
        unit.modId.nNum = lentmp;
        index+=lentmp;

        lentmp = get_len_offset(&buf[index], &lenoffset);
        index+=lenoffset;
        memcpy(unit.nSoftVer.value,&buf[index],lentmp);
        unit.nSoftVer.nNum = lentmp;
        index+=lentmp;

        lentmp = get_len_offset(&buf[index], &lenoffset);
        index+=lenoffset;
        memcpy(unit.softDate.value,&buf[index],lentmp);
        unit.softDate.nNum = lentmp;
        index+=lentmp;

        lentmp = get_len_offset(&buf[index], &lenoffset);
        index+=lenoffset;
        memcpy(unit.nHardVer.value,&buf[index],lentmp);
        unit.nHardVer.nNum = lentmp;
        index+=lentmp;

        lentmp = get_len_offset(&buf[index], &lenoffset);
        index+=lenoffset;
        memcpy(unit.hardDate.value,&buf[index],lentmp);
        unit.hardDate.nNum = lentmp;
        index+=lentmp;

        lentmp = get_len_offset(&buf[index], &lenoffset);
        index+=lenoffset;
        memcpy(unit.manuf.value,&buf[index],lentmp);
        unit.manuf.nNum = lentmp;
        index+=lentmp;

        unit.nChNum = buf[index++];

        TASK_FMT_DEBUG(-1,UP_LOG,"�����ģ����Ϣ ��λ�� %d ģ���豸�ͺ� %s �豸ID %s \n",ModuleIndex,unit.modType.value,unit.modId.value);
        TASK_FMT_DEBUG(-1,UP_LOG,"����汾 %s �������� %s \n",unit.nSoftVer.value,unit.softDate.value);
        TASK_FMT_DEBUG(-1,UP_LOG,"Ӳ���汾 %s �������� %s \n",unit.nHardVer.value,unit.hardDate.value);
        TASK_FMT_DEBUG(-1,UP_LOG,"���̱��� %s ͨ������ %d \n",unit.manuf.value,unit.nChNum);

        for(j=0;j<unit.nChNum;j++)
        {
            unit.chInfo[j].nType = buf[index++];
            unit.chInfo[j].nFunc = buf[index++];
            lentmp = get_len_offset(&buf[index], &lenoffset);
            index+=lenoffset;
            memcpy(unit.chInfo[j].nDriveName.value,&buf[index],lentmp);
            unit.chInfo[j].nDriveName.nNum = lentmp;
            index+=lentmp;

            TASK_FMT_DEBUG(-1,UP_LOG,"ͨ�� %d �ӿ����� %d �������� %d �������� %s\n",j,unit.chInfo[j].nType,unit.chInfo[j].nFunc,unit.chInfo[j].nDriveName.value);
        }
        pthread_mutex_lock(&gModuleInfoLock);
        if(gModuleInfo.nNum < OOP_MAX_MODULE)
        {
            memcpy(&gModuleInfo.item[gModuleInfo.nNum],&unit,sizeof(OOP_MODULE_UINT_NEW_T));
            gModuleInfo.nNum++;
        }
        pthread_mutex_unlock(&gModuleInfoLock);
    }
    else if(status == 1)
    {
        int flag = -1;
        pthread_mutex_lock(&gModuleInfoLock);
        for(j=0;j<gModuleInfo.nNum;j++)
        {
            if(ModuleIndex == gModuleInfo.item[j].nSlot)
            {
                flag = j;
                break;
            }
        }
        if(flag != -1)
        {
            memset(&gModuleInfo.item[flag],0x00,sizeof(OOP_MODULE_UINT_NEW_T));
            for(j =flag;j<gModuleInfo.nNum-1;j++)
            {
                memcpy(&gModuleInfo.item[j],&gModuleInfo.item[j+1],sizeof(OOP_MODULE_UINT_NEW_T));
            }
            memset(&gModuleInfo.item[gModuleInfo.nNum-1],0x00,sizeof(OOP_MODULE_UINT_NEW_T));
            gModuleInfo.nNum--;
        }
        pthread_mutex_unlock(&gModuleInfoLock);
    }

}
#endif

//����������Ԫ�ش��ڵ���2��������������
void right_shift(uint8 *buf,uint8 len)
{
    uint16 i=0;
    uint8 MSB=0x80;
    uint8 LSB =0x01;

    for(i=0;i<=len-1;i++)
    {
        buf[i]=buf[i]>>1;
        if((i<=len-2)&&(buf[i+1]&LSB))
        {
            buf[i]=buf[i]|MSB;
        }
    }
}

void UpdatePatchReadInfo(uint8 pipe,SCHEME_TASK_RUN_INFO *pCbTask,CTS_RUN_TASK_T  *RunTaskInfo)
{
    uint16 j=0;
    uint16 meternum=0;
    time_t startTime;
    OOP_TI_T delayTi;
    DateTimeBcd_t tmpTime;

    if(pCbTask->CsdTask.RoadTask[0].MainOad.nObjID !=0x5002)
    {
        return;
    }
    while(meternum<RunTaskInfo->NeedMtrNum)
    {
        if(pCbTask->MsInfo[j].Pipe == pipe)
        {
            meternum++;
            memcpy_r((uint8*)&tmpTime,(uint8*)RunTaskInfo->ThisTurnBeginTime,7);

            DT_DateTimeBCD2Time(&tmpTime,&startTime);
            delayTi.value = pCbTask->DelayTime;
            delayTi.unit = pCbTask->DelayTimeUnit;
            DT_TimeSubInv(&startTime, delayTi);
            GetCrvStartTime(startTime,pCbTask->TaskId,pCbTask->MsInfo[j].Sn,0);
        }
        j++;
    }
}

void GetPatch(uint8 pipe)
{
    AmrThreadArg_T* pAmrThreadArg;
    SCHEME_TASK_RUN_INFO TaskInfo;
    uint16 i=0;
    uint16 MeterIndex;
    SCHEME_TASK_RUN_INFO *pCbTask=NULL;
    CTS_RUN_TASK_T  *RunTaskInfo=NULL;

    pAmrThreadArg = &gAmrThreadArg[pipe];

    for(i=0;i<pAmrThreadArg->TaskNum;i++)
    {
        if(pAmrThreadArg->pTaskRunInfo[i].TaskState!=1||pAmrThreadArg->pTaskRunInfo[i].NeedMtrNum==0)
        {
            //TASK_FMT_TRACE( Pipe, REC_LOG,"���� %d ��ͣ�� ��������ִ� \n",pAmrThreadArg->pTaskRunInfo[i].TaskId);
            continue;
        }
        TASK_FMT_TRACE ( pipe,REC_LOG, "������11\n");
        memset(&TaskInfo,0x00,sizeof(SCHEME_TASK_RUN_INFO));
        taskmng_task_info_get(pAmrThreadArg->pTaskRunInfo[i].TaskId,pAmrThreadArg->pTaskRunInfo[i].logicid, &TaskInfo);
        if(TaskInfo.CsdTask.RoadTask[0].MainOad.value == 0x50020200)
        {
            TASK_FMT_TRACE ( pipe,REC_LOG, "�ҵ�����22\n");
            pAmrThreadArg->pTaskRunInfo[i].MeterIndex =0;
            pAmrThreadArg->pTaskRunInfo[i].CbNum =0;
            TASK_FMT_TRACE ( pipe,REC_LOG, "�ҵ�����33 is %d, is %d\n",pAmrThreadArg->pTaskRunInfo[i].CbNum,pAmrThreadArg->pTaskRunInfo[i].NeedMtrNum);
            while((pAmrThreadArg->pTaskRunInfo[i].CbNum)<pAmrThreadArg->pTaskRunInfo[i].NeedMtrNum)
            {
                MeterIndex=pAmrThreadArg->pTaskRunInfo[i].MeterIndex;
                if(TaskInfo.MsInfo[MeterIndex].Pipe == pipe)
                {
                    if(JudgeNeedPatch(pAmrThreadArg->pTaskRunInfo[i].TaskId,TaskInfo.MsInfo[MeterIndex].Sn,&pAmrThreadArg->pTaskRunInfo[i].patchtime)==TRUE)//�޸ĵĵط�
                    {

                        RunTaskInfo->CsdIndex=0;
                        RunTaskInfo->SubOadIndex=0;
                        TASK_FMT_TRACE ( pipe,REC_LOG, "�ҵ�Ҫ���ı� MeterIndex %d\n",MeterIndex);
                        break;
                    }
                    else
                    {
                        RunTaskInfo->CbNum++;
                    }
                }
                pAmrThreadArg->pTaskRunInfo[i].MeterIndex++;
                if(pAmrThreadArg->pTaskRunInfo[i].MeterIndex>=RunTaskInfo->TotalMtrNum)
                {
                    TASK_FMT_TRACE ( pipe,REC_LOG, "û�ҵ�Ҫ���ı� MeterIndex %d TotalMtrNum %d \n",RunTaskInfo->MeterIndex,RunTaskInfo->TotalMtrNum);
                    return;
                }

            }

        }
    }
    if(i>=pAmrThreadArg->TaskNum)
    {
        return;
    }
    pCbTask = &TaskInfo;
    RunTaskInfo = &pAmrThreadArg->pTaskRunInfo[i];
    RunTaskInfo->Ispatch=TRUE;
    Met485_AutkExec(pipe,pCbTask,RunTaskInfo);

}

struct timespec gMeterEventReportTime ;       //��¼���յ�����¼��ϱ������ʱ��
//��¼���յ�����¼��ϱ������ʱ��
void taskmng_meter_event_report_time_set()
{
    clock_gettime(CLOCK_MONOTONIC, &gMeterEventReportTime);
}
//����µ�ǰ�Ƿ����ִ���¼��ϱ��Ĵ������� 3������ִ�� ���¸�Ϊ5��
BOOL taskmng_meter_event_report_time_check()
{
    struct timespec timenow;
    clock_gettime(CLOCK_MONOTONIC, &timenow);

    if(timenow.tv_sec - gMeterEventReportTime.tv_sec > 5)
        return TRUE;

    return FALSE;
}

/*******************************************************************************
* ��������: taskmng_amr_status_get_mqtt
* ��������://0013 ��ȡ���ճ���״̬ mqtt�߳��յ���ֱ�ӻظ� ���ٸ������̴߳��� ԭ����ѯ��̫���ʱ���ѹ������ռ��
            ���º����͸�����ӵ���
* �������: pipe �˿ں�
        buf ��Ϣ����
        len ��Ϣ����
* �������:
* �� �� ֵ:
*******************************************************************************/
void taskmng_amr_status_get_mqtt(uint8 pipe,uint8* Addr,MESSAGE_INFO_T* MsgInfo)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    MESSAGE_INFO_T* MsgSend=NULL;
    uint8 payload[100] = {0};
    uint16 index=0;
    uint16 Pn;
    uint16 MsIndex;
    Pn = taskmng_pn_get_from_addr(Addr,LOGIC_ID_YC);
    MsIndex = taskmng_ms_index_get(pipe, Pn,LOGIC_ID_YC);

    payload[index++] = pAmrThreadArg->pMtrAmrInfo[MsIndex].LastResult;
    memcpy_r(&payload[index],(uint8*)&pAmrThreadArg->pMtrAmrInfo[MsIndex].AllFailCnt,2);
    index+=2;
    MakeDateTimeS(&payload[index],pAmrThreadArg->pMtrAmrInfo[MsIndex].LastSendTime);
    index+=7;
    MakeDateTimeS(&payload[index],pAmrThreadArg->pMtrAmrInfo[MsIndex].LastRecvTime);
    index+=7;
    MakeDateTimeS(&payload[index],pAmrThreadArg->pMtrAmrInfo[MsIndex].LastFailTime);
    index+=7;

    MsgSend = (MESSAGE_INFO_T*)malloc(sizeof(MESSAGE_INFO_T)+index);
    memset(MsgSend,0x00,sizeof(MESSAGE_INFO_T)+index);

    MsgSend->rpm = 0;
    MsgSend->index = MsgInfo->index;
    MsgSend->priority = MsgInfo->priority;
    MsgSend->label = MsgInfo->label;
    MsgSend->IID = MsgInfo->IID;
    MsgSend->IOP = MsgInfo->IOP;

    MsgSend->sourlen = MsgInfo->destlen;
    memcpy(MsgSend->souraddr,MsgInfo->destaddr,MsgSend->sourlen);
    MsgSend->destlen = MsgInfo->sourlen;
    memcpy(MsgSend->destaddr,MsgInfo->souraddr,MsgSend->destlen );

    MsgSend->msglen = index;
    memcpy(MsgSend->playload,payload,index);
    //TASK_FMT_DEBUG(pipe, RELAY_LOG,"���ճ���״̬\n");
    //TASK_BUF_DEBUG( pipe, RELAY_LOG,MsgSend->playload,MsgSend->msglen);
    taskmng_plc_queue_write(APPArg.SendBuf, (uint8*)MsgSend, sizeof(MESSAGE_INFO_T)+index);
    free(MsgSend);

}

#if DESC("�ն˴���״̬",1)
TMN_POWER_STATE_T gTmnPowerState;   //�ն˴���״̬

/**********************************************************
* �� �� �� : taskmng_power_state_init
* �������� : �ն˴���״̬��ʼ��
* ������� : ��
* ������� : ��
* �� �� ֵ : �ɹ� : 0
*            ʧ�� : -1
* �޸���ʷ :  2024/5/31
************************************************************/
int taskmng_power_state_init()
{
    time_t timenow = time(NULL);
    memset(&gTmnPowerState,0,sizeof(gTmnPowerState));
    gTmnPowerState.powerState = POWER_ON;
    /*���ϵ縳��ʱ��  */
    DT_Time2DateTimeShort(timenow,&gTmnPowerState.powerOnTime);
    return 0;
}

/**********************************************************
* �� �� �� : taskmng_power_event_deal
* �������� : �յ��ն�ͣ�ϵ��¼����ȡ��������
* ������� : infonum �յ�ͣ�ϵ��¼������
* ������� : ��
* �� �� ֵ : �ɹ� : 0
*            ʧ�� : -1
* �޸���ʷ :  2024/5/31
************************************************************/
int taskmng_power_event_deal(DB_CLIENT clientid,uint16 infonum)
{
    int ret = 0;
    READ_RECORD_T       readRecordIn;
    RECORD_UNIT_T       readRecordOut;
    OOP_POWERLOG_T data;
    time_t timenow = time(NULL);
    TASK_FMT_DEBUG(-1, UP_LOG, "��ȡ�ն�ͣ�ϵ��¼� \n");
    memset(&readRecordIn,0,sizeof(readRecordIn));
    memset(&readRecordOut,0,sizeof(readRecordOut));
    memset(&data,0,sizeof(OOP_POWERLOG_T));
    /* ��ȡ�ն�ͣ�ϵ��¼� */
    readRecordIn.recordOAD.optional = 0;
    readRecordIn.recordOAD.classtag = CLASS_DATA_EVENT;
    readRecordIn.recordOAD.road.oadMain.value = 0x00000000;
    readRecordIn.recordOAD.road.oadCols.nNum = 1;
    readRecordIn.recordOAD.road.oadCols.oad[0].value= 0x31060200;
    readRecordIn.recordOAD.logicId = 0;
    readRecordIn.recordOAD.infoNum = infonum;
    readRecordIn.cType = COL_TM_STORE;
    readRecordIn.cStart = 0;
    readRecordIn.cEnd = time(NULL)+1;
    readRecordIn.sortType = 1;
    readRecordIn.MAC.nNum = 6;
    memcpy_r(readRecordIn.MAC.value,LocalAddr, 6);

    ret = db_read_record(clientid, &readRecordIn, DATA_LEN_MAX, &readRecordOut);
    if(ret != 0 || readRecordOut.record[0].outDatalen != sizeof(OOP_POWERLOG_T))
    {
        TASK_FMT_DEBUG(-1, UP_LOG, "��ȡ�ն�ͣ�ϵ��¼�ʧ�� ret %d outlen %d\n",ret,readRecordOut.record[0].outDatalen);
        return -1;
    }
    memcpy(&data, readRecordOut.record[0].outDataBuf, sizeof(OOP_POWERLOG_T));
    if (data.source.choice == DT_ENUM)
    {
        if (data.source.nValue == 0)  //ͣ��
        {
            TASK_FMT_DEBUG(-1, UP_LOG, "�ն�ͣ�� \n");
            gTmnPowerState.powerState = POWER_OFF;
            DT_Time2DateTimeShort(timenow,&gTmnPowerState.powerOffTime);
            taskmng_meter_power_report_num_clear(POWER_ON);
        }
        else if (data.source.nValue == 1)  //�ϵ�
        {
            TASK_FMT_DEBUG(-1, UP_LOG, "�ն��ϵ� \n");
            gTmnPowerState.powerState = POWER_ON;
            DT_Time2DateTimeShort(timenow,&gTmnPowerState.powerOnTime);
            /* ͣ������ϵ� ��ͣ���¼����ϱ�������һ�� �Է�����ͣ�粻�ϱ��� */
            taskmng_meter_power_report_num_clear(POWER_OFF);
        }
    }
    return 0;
}


/**********************************************************
* �� �� �� : taskmng_power_state_get
* �������� : ��ȡ�ն˴���״̬
* ������� :
* ������� : TMN_POWER_STATE_T *state    �ն˴���״̬
* �� �� ֵ : ��
* �޸���ʷ :  2024/5/31
************************************************************/
void taskmng_power_state_get(TMN_POWER_STATE_T *state)
{
    if(NULL == state)
    {
        return;
    }
    memcpy(state,&gTmnPowerState,sizeof(TMN_POWER_STATE_T));
}
#endif
