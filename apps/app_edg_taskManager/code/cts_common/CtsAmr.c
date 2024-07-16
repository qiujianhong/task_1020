

#include "CtsAll.h"
#include "area.h"
#include "CtsPlc.h"

extern const MapOopTo645_NEW MapOopTo645_Min[];
extern const MapOopTo645_NEW MapOopTo645_MinOne[];
#if (defined AREA_JIANGXI) || (defined AREA_ANHUI)|| (defined AREA_JIANGSU)
extern OOP_PERCEPTION_TERMINAL_LIST_T perceptionterminal;
#endif
extern uint8 patch_policy_flg[AMR_THREAD_NUM];

/*******************************************************************************
* 函数名称: OopMeterAmrData
* 函数功能:存储面向对象规约表数据
* 输入参数:pipe 端口号
         MtrIndex 表索引
         MainOad 主oad
         Oad 子oad
         pRcvBuf 收到的数据buf
         RcvLen 收到的数据长度
         pTmp_NormalLink 任务参数
         RecvRunTask 任务运行时参数
         StoreTime 存储时间
         
* 输出参数:
* 返 回 值: 已解析存储的数据长度
*******************************************************************************/
uint16 OopMeterAmrData(uint8 Pipe,uint16 MtrIndex,OOP_OAD_U MainOad,OOP_OAD_U Oad, uint8* pRcvBuf, uint16 RcvLen, SCHEME_TASK_RUN_INFO* pTmp_NormalLink,CTS_RUN_TASK_T *RecvRunTask , DateTimeBcd_t *StoreTime)
{
    AmrThreadArg_T *pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[Pipe];
    uint16 offset =0 ;
    uint16 DataLen = 0;
    uint8 DataBuff[MAX_MSG_LEN]={0};
    uint16 Pn;
    uint8 logicid;
    DateTimeBcd_t  tmpTime;

    OadInfoMap pOadInfo;
    memset(&pOadInfo,0x00,sizeof(OadInfoMap));
    BOOL ret;
    uint8 pwrType;
    uint32 starttime,storetime;
    Pn = pAmrThreadArg->pMtrAmrInfo[MtrIndex].Pn;
    logicid = pAmrThreadArg->pMtrAmrInfo[MtrIndex].logicid;
#ifdef AREA_SHANDONG
    if(pAmrThreadArg->pMtrAmrInfo[MtrIndex].NILMModuleInfo == 0 && Oad.value == 0x40310200)
    {
        TASK_FMT_TRACE(Pipe,REC_LOG,"收到0x40310200回码\n");
        taskmng_meter_NILM_module_check(Pipe,pRcvBuf,RcvLen,&pAmrThreadArg->pMtrAmrInfo[MtrIndex].NILMModuleInfo,&pAmrThreadArg->pMtrAmrInfo[MtrIndex].logicid_5_info);
        return 0;
    }
#endif

    //根据oad获取对应的结构体信息
    ret = taskmng_oad_info_get( Oad,&pOadInfo);
    //TASK_FMT_DEBUG(Pipe,REC_LOG,"OopMeterAmrData OAD %08x ret %d \n",Oad.value,ret);
    if(ret==FALSE)
    {
        return 0;
    }
    if(pRcvBuf[0]==0) //如果为datanull 则无需解析
    {
        TASK_FMT_TRACE(Pipe,REC_LOG,"data为空 不解析\n");
        return 1;
    }
    pwrType=taskmng_mtr_conntype_get(Pn,logicid);
    Convert_Oop_Data_To_Struct(pRcvBuf,&offset,RcvLen,DataBuff,&pOadInfo,&DataLen,pwrType);
    //TASK_BUF_DEBUG(Pipe,REC_LOG,DataBuff,DataLen);
    memcpy_r((uint8*)&tmpTime,(uint8*)RecvRunTask->ThisTurnBeginTime,7);
    DT_DateTimeBCD2Time(&tmpTime,(time_t*)&starttime);     //开始时间
    if(StoreTime==NULL)
    {
        time_t storetm;
        OOP_TI_T delay;
        DT_DateTimeBCD2Time(&tmpTime,&storetm);
        delay.unit = pTmp_NormalLink->DelayTimeUnit;
        delay.value = pTmp_NormalLink->DelayTime;
        DT_TimeSubInv(&storetm, delay);
        tmpTime = taskmng_acq_store_time_get(pTmp_NormalLink->StoreTimeType,storetm);
        DT_DateTimeBCD2Time(&tmpTime,(time_t*)&storetime);
    }
    else
    {
        DT_DateTimeBCD2Time(StoreTime,(time_t*)&storetime);
        
        TASK_FMT_TRACE(Pipe, REC_LOG,"StoreTm 20%02x/%02x/%02x %02x:%02x:%02x \n",StoreTime->year_l,StoreTime->month,StoreTime->day,StoreTime->hour,StoreTime->min,StoreTime->sec);
    }
    //先将数据缓存
    taskmng_cache_buf_save(Pipe,Pn,MainOad,Oad,DataBuff,DataLen,starttime,storetime);


#ifdef AREA_JIANGXI
    //通过判断电表运行状态字来判断是否发生了逆向序
    //只要有一次发生逆向序，则置标志位
    if (MainOad.value == 0x50020200 && Oad.value == 0x20140200)
    {
        OOP_METWORD_T metWord;
        memcpy(&metWord, DataBuff, DataLen);
        if (metWord.metWord[6] & 0x03) //电表运行状态字7        第0位:电压逆相序 第1位:电流逆相序
            pAmrThreadArg->pMtrAmrInfo[MtrIndex].reversePhase = 1;
    }

    /* 判断零线电流是否大于0.5A */
    if (MainOad.value == 0x50020200 && Oad.value == 0x20010400)
    {
        int32   zeroCurrent;
        memcpy(&zeroCurrent, DataBuff, DataLen);
        if (zeroCurrent > 500)
            pAmrThreadArg->pMtrAmrInfo[MtrIndex].zeroCurrentFlag = 1;
    }
#endif

    return offset;
}

/*******************************************************************************
* 函数名称: taskmng_oop_meter_data_normal_list_save
* 函数功能:存储面向对象规约表实时数据
* 输入参数:     Pipe            端口号
            DataBuf              //接收到的电表数据
            DataLen              //接收到的电表长度
            MtrIndex             //电表索引信息
            pTmp_NormalLink      //方案任务节点
            StoreTime            //存储时间
            RecvRunTask     任务运行时参数
* 输出参数:
* 返 回 值:
*******************************************************************************/
uint16 taskmng_oop_meter_data_normal_save(uint8 Pipe,uint8 *DataBuf,uint16 DataLen,uint16 MtrIndex,SCHEME_TASK_RUN_INFO *pTmp_NormalLink,CTS_RUN_TASK_T *RecvRunTask )
{

    AmrThreadArg_T *pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[Pipe];
    //uint8           QuenueNum;  //OAD数量
    uint16           Dataoffset = 0;
    OOP_OAD_U            Oad;
    OOP_OAD_U       MainOad  ;
    uint16          OadLen = 0;
    //MainOad.value = 0x00000000;
    //分钟冻结任务如果配采集当前数据 还是需要存到分钟冻结里
    MainOad.value = pTmp_NormalLink->CsdTask.RoadTask[0].MainOad.value;
    uint16 Pn = pAmrThreadArg->pMtrAmrInfo[MtrIndex].Pn;//测量点号
    //uint8 oadNum = DataBuf[Dataoffset++];  //本次回复的OAD数量
    //uint8 i;
    TASK_FMT_TRACE(Pipe, REC_LOG, "OOP Save normal\n");
    //Dataoffset++;//OAD数量
    //while(Dataoffset < DataLen)//GET-Response长度不能低于5字节(OAD+DAR)
    //for(i=0;i<oadNum;i++)
    {
        if(Dataoffset + 4 > DataLen)//容错处理
            return 0;

        memcpy_r((uint8*)&Oad.value,&DataBuf[Dataoffset],4);
        Dataoffset += 4;
        //TASK_FMT_TRACE(Pipe, REC_LOG, "回复的oad %08x \n",Oad.value);
        //下移CSD索引信息
        if(pAmrThreadArg->pMtrAmrInfo[MtrIndex].IsNeedGetSafeMode == FALSE)
        {
            pAmrThreadArg->pMtrAmrInfo[MtrIndex].CSDIndex++;
        }

        if(DataBuf[Dataoffset++] != 0x01)//数据访问结果DAR
        {
            Dataoffset++;//DAR
            return 0;
        }

        //调用接口存数据
        //#ifdef AREA_HUNAN
       
        //TASK_FMT_TRACE(Pipe, REC_LOG, "回复的MainOad %08x \n",MainOad.value);
        //#endif
        OadLen = OopMeterAmrData(Pipe,MtrIndex,MainOad,Oad,DataBuf+Dataoffset,(DataLen-Dataoffset),pTmp_NormalLink,RecvRunTask,NULL);
        if(OadLen==0)
            return 0;//返回数据错误
        Dataoffset += OadLen;
    }
    if(DataBuf[Dataoffset++] ==1) //有跟随上报信息域
    {
        OOPFollowReport(Pipe,Pn,&DataBuf[Dataoffset],DataLen-Dataoffset);
    }
    return 1;
}

/*******************************************************************************
* 函数名称: taskmng_oop_meter_data_normal_list_save
* 函数功能:存储面向对象规约表实时数据
* 输入参数:     Pipe            端口号
            DataBuf              //接收到的电表数据
            DataLen              //接收到的电表长度
            MtrIndex             //电表索引信息
            pTmp_NormalLink      //方案任务节点
            StoreTime            //存储时间
            RecvRunTask     任务运行时参数
* 输出参数:
* 返 回 值:
*******************************************************************************/
uint16 taskmng_oop_meter_data_normal_list_save(uint8 Pipe,uint8 *DataBuf,uint16 DataLen,uint16 MtrIndex,SCHEME_TASK_RUN_INFO *pTmp_NormalLink,CTS_RUN_TASK_T *RecvRunTask )
{

    AmrThreadArg_T *pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[Pipe];
    //uint8           QuenueNum;  //OAD数量
    uint16           Dataoffset = 0;
    OOP_OAD_U            Oad;
    OOP_OAD_U       MainOad  ;
    uint16          OadLen = 0;
    //MainOad.value = 0x00000000;
    //分钟冻结任务如果配采集当前数据 还是需要存到分钟冻结里
    MainOad.value = pTmp_NormalLink->CsdTask.RoadTask[0].MainOad.value;
    uint16 Pn = pAmrThreadArg->pMtrAmrInfo[MtrIndex].Pn;//测量点号
    uint8 oadNum = DataBuf[Dataoffset++];  //本次回复的OAD数量
    uint8 i;
    TASK_FMT_TRACE(Pipe, REC_LOG, "OOP Save normal\n");
    //Dataoffset++;//OAD数量
    //while(Dataoffset < DataLen)//GET-Response长度不能低于5字节(OAD+DAR)
    for(i=0;i<oadNum;i++)
    {
        if(Dataoffset + 5 > DataLen)//容错处理
            break;

        memcpy_r((uint8*)&Oad.value,&DataBuf[Dataoffset],4);
        Dataoffset += 4;
        //TASK_FMT_TRACE(Pipe, REC_LOG, "回复的oad %08x \n",Oad.value);
        //下移CSD索引信息
        if(pAmrThreadArg->pMtrAmrInfo[MtrIndex].IsNeedGetSafeMode == FALSE)
        {
            pAmrThreadArg->pMtrAmrInfo[MtrIndex].CSDIndex++;
        }

        if(DataBuf[Dataoffset++] != 0x01)//数据访问结果DAR
        {
            Dataoffset++;//DAR
            continue;
        }

        //调用接口存数据
        //#ifdef AREA_HUNAN
       
        //TASK_FMT_TRACE(Pipe, REC_LOG, "回复的MainOad %08x \n",MainOad.value);
        //#endif
        OadLen = OopMeterAmrData(Pipe,MtrIndex,MainOad,Oad,DataBuf+Dataoffset,(DataLen-Dataoffset),pTmp_NormalLink,RecvRunTask,NULL);
        if(OadLen==0)
            return 0;//返回数据错误
        Dataoffset += OadLen;
    }
    if(DataBuf[Dataoffset++] ==1) //有跟随上报信息域
    {
        OOPFollowReport(Pipe,Pn,&DataBuf[Dataoffset],DataLen-Dataoffset);
    }
    return 1;
}



/*******************************************************************************
* 函数名称: 存储面向对象规约表记录型数据解析
* 函数功能:
* 输入参数: pipe 端口号
            DataBuf              //接收到的电表数据
            DataLen              //接收到的电表长度
            MtrIndex             //电表索引信息
            pTmp_NormalLink      //方案任务节点
            RecvRunTask     //任务运行时参数
            StoreTime            //存储时间
            flag   标记是否为应用分帧的解析，如果是应用分帧数据则csdindex不应该加
* 输出参数:
* 返 回 值:
*******************************************************************************/
uint16 taskmng_oop_meter_data_record_save(uint8 Pipe,uint8 *DataBuf,uint16 DataLen,uint16 MtrIndex,SCHEME_TASK_RUN_INFO* pTmp_NormalLink,CTS_RUN_TASK_T *RecvRunTask,DateTimeBcd_t *StoreTime,uint8 flag)
{
    AmrThreadArg_T  *pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[Pipe];
    uint16           Dataoffset = 0;
    uint8           CsdNum = DataBuf[Dataoffset++];    //CSD数目
    OOP_OAD_U            Oad[128];
    uint8           i;
    uint8           j;
    uint8           RecordNum = 0;
    uint16          OadLen = 0;
    uint8           ExistPosae = 0;
    DateTimeBcd_t   StoreTm;
    OadInfoMap      pMap;
    OOP_OAD_U            OadTmp;
    OOP_OAD_U       MainOad;
    uint8           RoadIndex=0;
    uint32          EventSeq;
    uint32          EventSeqReport;
    uint16          Pn = pAmrThreadArg->pMtrAmrInfo[MtrIndex].Pn;//测量点号
    uint8           is_happen_pre = FALSE;
    uint8           is_happen = FALSE;
    uint8           is_recover = FALSE;

    if(pTmp_NormalLink->SchemeType == EVENTPLAN)
    {
        RoadIndex = pAmrThreadArg->pMtrAmrInfo[MtrIndex].ROADIndex;
        
    }
    TASK_FMT_TRACE(Pipe, REC_LOG,"oop save record SchemeType %d RoadIndex %d CsdNum %d\n",pTmp_NormalLink->SchemeType,RoadIndex,CsdNum);
    //变量RCSD对应的OAD信息
    for(i = 0; i < CsdNum; i++)
    {
        if(DataBuf[Dataoffset++] != 0)
            return 0;

        //Oad[i].nObjID = DataBuf[Dataoffset]*0x100+DataBuf[Dataoffset+1];

        //Oad[i].attID    = DataBuf[Dataoffset+2];
        //Oad[i].nIndex = DataBuf[Dataoffset+3];
        memcpy_r((uint8*)&Oad[i].value,&DataBuf[Dataoffset],4);
        
        if(pTmp_NormalLink->CsdTask.RoadTask[RoadIndex].MainOad.nObjID==0x5004 && Oad[i].nObjID==0x0010)
            ExistPosae=1;
        Dataoffset += 4;
        //TASK_FMT_TRACE(Pipe, REC_LOG,"返回的csd %d  oad %08x \n",i,Oad[i].value);
    }

    //错误信息 [0] DAR
    if(DataBuf[Dataoffset++] == 0x0)
    {
        //printf("错误信息 [0] DAR : %d  %d  %d\n",MtrIndex,pTmp_NormalLink->MsInfo[MtrIndex].CSDIndex,pTmp_NormalLink->MsInfo[MtrIndex].SigFrameNum);
        if(pTmp_NormalLink->AcqInfo.Acqtype == ACQTYP_BC)
        {
            time_t  StartTime;
            StartTime = pAmrThreadArg->pMtrAmrInfo[MtrIndex].StartTime;
            if(pTmp_NormalLink->AcqInfo.TI.unit == TASK_CYCLE_FLAG_MIN)
                pAmrThreadArg->pMtrAmrInfo[MtrIndex].StartTime += pTmp_NormalLink->AcqInfo.TI.value * 60;
            else if (pTmp_NormalLink->AcqInfo.TI.unit == TASK_CYCLE_FLAG_HOUR)
                pAmrThreadArg->pMtrAmrInfo[MtrIndex].StartTime += pTmp_NormalLink->AcqInfo.TI.value * 3600;
            else if (pTmp_NormalLink->AcqInfo.TI.unit == TASK_CYCLE_FLAG_DAY)
                pAmrThreadArg->pMtrAmrInfo[MtrIndex].StartTime += pTmp_NormalLink->AcqInfo.TI.value * 3600 * 24;
            if(StartTime < RecvRunTask->StartTime)
            {
                TASK_FMT_DEBUG(0, REC_LOG, "startTime is %d, LastN is %u\n", pAmrThreadArg->pMtrAmrInfo[MtrIndex].StartTime, pTmp_NormalLink->AcqInfo.LastN);
                DateTimeBcd_t       tmpTime;
                DT_Time2DateTimeBCD(pAmrThreadArg->pMtrAmrInfo[MtrIndex].StartTime, &tmpTime);
                TASK_FMT_DEBUG(0, REC_LOG,"计算补抄开始时间:%02x%02x-%02x-%02x %02x:%02x:%02x\n", tmpTime.year_h, tmpTime.year_l,
                    tmpTime.month, tmpTime.day, tmpTime.hour, tmpTime.min, tmpTime.sec);
                pAmrThreadArg->pMtrAmrInfo[MtrIndex].CSDIndex = 0;
            }
            else
            {
                pAmrThreadArg->pMtrAmrInfo[MtrIndex].CSDIndex += pAmrThreadArg->pMtrAmrInfo[MtrIndex].SigFrameNum;
            }
        }
        else
        {
            pAmrThreadArg->pMtrAmrInfo[MtrIndex].CSDIndex += pAmrThreadArg->pMtrAmrInfo[MtrIndex].SigFrameNum;
        }
        return 0;
    }
    MainOad = pTmp_NormalLink->CsdTask.RoadTask[RoadIndex].MainOad;
    RecordNum = DataBuf[Dataoffset++];
    //TASK_FMT_TRACE(Pipe, REC_LOG,"--------------mainoad %08x recordnum %d -------------\n",MainOad.value,RecordNum);
#ifdef AREA_SHANDONG
    TASK_FMT_TRACE(Pipe, REC_LOG,"timeOffset=%ld\n", pAmrThreadArg->pMtrAmrInfo[MtrIndex].timeOffset);
#endif

    if (RecordNum == 0x0)
    {
#ifdef AREA_SHANDONG
        if (TaskMgrGetTimeOffset(Pipe, taskmng_task_period_get(pTmp_NormalLink->TaskId, pTmp_NormalLink->logicid), &pAmrThreadArg->pMtrAmrInfo[MtrIndex]) == 0)
        {
            return 0;
        }
#endif
        if(pTmp_NormalLink->AcqInfo.Acqtype == ACQTYP_BC)
        {
            time_t  StartTime;
            StartTime = pAmrThreadArg->pMtrAmrInfo[MtrIndex].StartTime;
            if(pTmp_NormalLink->AcqInfo.TI.unit == TASK_CYCLE_FLAG_MIN)
                pAmrThreadArg->pMtrAmrInfo[MtrIndex].StartTime += pTmp_NormalLink->AcqInfo.TI.value * 60;
            else if (pTmp_NormalLink->AcqInfo.TI.unit == TASK_CYCLE_FLAG_HOUR)
                pAmrThreadArg->pMtrAmrInfo[MtrIndex].StartTime += pTmp_NormalLink->AcqInfo.TI.value * 3600;
            else if (pTmp_NormalLink->AcqInfo.TI.unit == TASK_CYCLE_FLAG_DAY)
                pAmrThreadArg->pMtrAmrInfo[MtrIndex].StartTime += pTmp_NormalLink->AcqInfo.TI.value * 3600 * 24;
            if(StartTime < RecvRunTask->StartTime)
            {
                TASK_FMT_DEBUG(0, REC_LOG, "startTime is %d, LastN is %u\n", pAmrThreadArg->pMtrAmrInfo[MtrIndex].StartTime, pTmp_NormalLink->AcqInfo.LastN);
                DateTimeBcd_t       tmpTime;
                DT_Time2DateTimeBCD(pAmrThreadArg->pMtrAmrInfo[MtrIndex].StartTime, &tmpTime);
                TASK_FMT_DEBUG(0, REC_LOG,"计算补抄开始时间:%02x%02x-%02x-%02x %02x:%02x:%02x\n", tmpTime.year_h, tmpTime.year_l,
                    tmpTime.month, tmpTime.day, tmpTime.hour, tmpTime.min, tmpTime.sec);
                pAmrThreadArg->pMtrAmrInfo[MtrIndex].CSDIndex = 0;
            }
            else
            {
                pAmrThreadArg->pMtrAmrInfo[MtrIndex].CSDIndex += pAmrThreadArg->pMtrAmrInfo[MtrIndex].SigFrameNum;
            }
        }
        else
        {
            pAmrThreadArg->pMtrAmrInfo[MtrIndex].CSDIndex += pAmrThreadArg->pMtrAmrInfo[MtrIndex].SigFrameNum;
        }
        if(ExistPosae == 1)
        {
            pAmrThreadArg->pMtrAmrInfo[MtrIndex].CurNum=1;
            pAmrThreadArg->pMtrAmrInfo[MtrIndex].EndNum=0;
        }
        return 0;
    }
    //日冻结不为空，置标志不再补抄
    if(ExistPosae == 1)
    {
        pAmrThreadArg->pMtrAmrInfo[MtrIndex].EndNum=62;
    }

    for(i = 0; i < RecordNum; i++)//记录数目
    {
        //根据每条记录解析对应的OAD内容
        for(j = 0; j < CsdNum; j++)
        {
            OadTmp = Oad[j];
            OadTmp.attPro = 0;
            //taskmng_oad_info_get(OadTmp,&pMap);
            if(taskmng_oad_info_get(OadTmp,&pMap)==FALSE)
            {
                pAmrThreadArg->pMtrAmrInfo[MtrIndex].CSDIndex += pAmrThreadArg->pMtrAmrInfo[MtrIndex].SigFrameNum;
                return 0;
            }
            if(pTmp_NormalLink->SchemeType ==EVENTPLAN && Oad[j].value== 0x20220200)
            {
                if(DataBuf[Dataoffset]==DT_DOUBLE_LONG_UNSIGNED)
                    memcpy_r((uint8*)&EventSeq,&DataBuf[Dataoffset+1],4);
                TASK_FMT_TRACE( Pipe, REC_LOG, "事件序号 %ld \n",EventSeq);
                EventSeqReport = GetTaskReportSeq( Pipe, RecvRunTask->TaskId, RecvRunTask->logicid,MainOad, Pn);
                TASK_FMT_TRACE( Pipe, REC_LOG, "该事件上次抄读的记录序号为%ld  \n",EventSeqReport );
  
                //if(EventSeq == 0 || EventSeq == EventSeqReport)
                //事件序号0为有效的
                if( EventSeq == EventSeqReport)
                {
                    /* 电能表事件发生与恢复的序号相同，事件发生时事件结束时间为空 */
                    TASK_FMT_TRACE( Pipe, REC_LOG, "事件序号相同 \n");
                }
                else
                {
                    TASK_FMT_TRACE( Pipe, REC_LOG, "记录事件序号 \n");
                    SetTaskReportSeq( Pipe, RecvRunTask->TaskId,RecvRunTask->logicid, MainOad, Pn,EventSeq);
                }
            }
            if (pTmp_NormalLink->SchemeType ==EVENTPLAN && Oad[j].value== 0x20200200)
            {
                is_happen_pre = GetTaskReportEventState(Pipe, RecvRunTask->TaskId, RecvRunTask->logicid, MainOad, Pn);
                if (DataBuf[Dataoffset] == DT_NULL) /* 事件发生，没有结束时间 */
                {
                    if (!is_happen_pre)
                    {
                        SetTaskReportEventState(Pipe, RecvRunTask->TaskId, RecvRunTask->logicid, MainOad, Pn, TRUE);
                        is_happen = TRUE;
                        TASK_FMT_TRACE( Pipe, REC_LOG, "事件发生 \n");
                    }
                }
                else    /* 事件恢复 */
                {
                    if (is_happen_pre)
                    {
                        SetTaskReportEventState(Pipe, RecvRunTask->TaskId, RecvRunTask->logicid, MainOad, Pn, FALSE);
                        is_recover = TRUE;
                        TASK_FMT_TRACE( Pipe, REC_LOG, "事件恢复 \n");
                    }
                    else
                    {
                        TASK_FMT_TRACE( Pipe, REC_LOG, "事件恢复之前没有事件发生 \n");
                    }
                }
            }

            if(pTmp_NormalLink->StoreTimeType == STRTMTYP_FRZTIME || pTmp_NormalLink->AcqInfo.Acqtype == ACQTYP_BC)
            {
                //抄读record 的时候第一个OI为时标
                if(Oad[j].nObjID == 0x2021)
                {
                    DateTimeHex_t        TmpDateTimeHex;
                    memset(&TmpDateTimeHex,0x0,sizeof(TmpDateTimeHex));
                    TmpDateTimeHex.year_h = DataBuf[Dataoffset+1];
                    TmpDateTimeHex.year_l = DataBuf[Dataoffset+2];
                    TmpDateTimeHex.month  = DataBuf[Dataoffset+3];
                    TmpDateTimeHex.day    = DataBuf[Dataoffset+4];
                    TmpDateTimeHex.hour   = DataBuf[Dataoffset+5];
                    TmpDateTimeHex.min    = DataBuf[Dataoffset+6];

                    if(DataBuf[Dataoffset+7] != 0xff)//曲线秒回复为0xff(林洋电表)
                        TmpDateTimeHex.sec    = DataBuf[Dataoffset+7];

                    DT_DateTimeHex2BCD(&TmpDateTimeHex,&StoreTm);
                    //台体测试会出现回复的曲线冻结时标不对 所以加个判断
                    if(pTmp_NormalLink->CsdTask.RoadTask[RoadIndex].MainOad.nObjID==0x5002)
                    {
                        time_t timetmp;
                        DT_DateTimeHex2Time(&TmpDateTimeHex,&timetmp);
                        TASK_FMT_DEBUG(0, REC_LOG,"698回复帧解析时间:%02x%02x-%02x-%02x %02x:%02x:%02x\n", TmpDateTimeHex.year_h, TmpDateTimeHex.year_l,
                            TmpDateTimeHex.month, TmpDateTimeHex.day, TmpDateTimeHex.hour, TmpDateTimeHex.min, TmpDateTimeHex.sec);
                        if(timetmp < pAmrThreadArg->pMtrAmrInfo[MtrIndex].StartTime)
                        {
                            TASK_FMT_TRACE( Pipe, REC_LOG, "曲线收到的冻结时标不对 recv %ld StartTime %ld\n",timetmp,pAmrThreadArg->pMtrAmrInfo[MtrIndex].StartTime);
                            return 0;
                        }
                    }
                }
            }
            else
            {
                StoreTm = taskmng_scheme_task_store_time_get(Pipe,pTmp_NormalLink);
                //存储时间时，若一次抄回多条事件，以秒数区分各事件
                if(pTmp_NormalLink->SchemeType == EVENTPLAN)
                    StoreTm.sec = i;
            }
            //TASK_FMT_TRACE(Pipe, REC_LOG, "第%d条记录第%d个数据项\n",i,j);
           
            OadLen = OopMeterAmrData(Pipe,MtrIndex,MainOad,Oad[j],DataBuf+Dataoffset,(DataLen-Dataoffset),pTmp_NormalLink,RecvRunTask,&StoreTm);
            Dataoffset += OadLen;
        }
    }

    if (pTmp_NormalLink->SchemeType == EVENTPLAN && EventSeq == EventSeqReport && (!is_happen && !is_recover))
    {
        if(flag!=1)
        {
            pAmrThreadArg->pMtrAmrInfo[MtrIndex].CSDIndex += pAmrThreadArg->pMtrAmrInfo[MtrIndex].SigFrameNum;
        }
        return 0;
    }

    //TASK_FMT_TRACE( Pipe, REC_LOG, "pTmp_NormalLink->SchemeType is %d,pTmp_NormalLink->rptFlag is %d\n",pTmp_NormalLink->SchemeType,pTmp_NormalLink->rptFlag);
        //事件采集方案 如果需要上报 则组成6012格式写入数据中心事件上报表中
    if(pTmp_NormalLink->SchemeType ==EVENTPLAN && pTmp_NormalLink->rptFlag == TRUE )
    {
        TASK_FMT_TRACE( Pipe, REC_LOG, "该事件方案需要上报！\n");
        time_t StartTime;
        DateTimeBcd_t tmpTime;
        memcpy_r((uint8*)&tmpTime,(uint8*)RecvRunTask->ThisTurnBeginTime,7);
        DT_DateTimeBCD2Time(&tmpTime,(time_t*)&StartTime);
        #if (defined AREA_JIANGXI) || (defined AREA_ANHUI)||(defined AREA_JIANGSU)
        if(MainOad.value == 0x31300200) //感知终端下挂电能表变化事件
        {
            Save698TerminalEventData(Pipe,DataBuf,DataLen,Pn,MainOad,StartTime);
        }
        else
        {
            Save698EventData(Pipe, DataBuf, DataLen, Pn, MainOad, StartTime);
        }
        #else
        Save698EventData(Pipe, DataBuf, DataLen, Pn, MainOad, StartTime);
        #endif

    }
    if(DataBuf[Dataoffset++] ==1) //有跟随上报信息域
    {
        OOPFollowReport(Pipe,Pn,&DataBuf[Dataoffset],DataLen-Dataoffset);
    }
    if(flag!=1)
    {
        pAmrThreadArg->pMtrAmrInfo[MtrIndex].CSDIndex += pAmrThreadArg->pMtrAmrInfo[MtrIndex].SigFrameNum;
    }
    return 1;

}




/****************************************************************************
名称： STD64507_AUTK_Recv
功能：
-           解析部颁规约电表返回报文
处理流程：
-           1、根据任务类型判定是存数据字典的任务还是主站配置的任务
-           2、
-           3、
输入参数：
-           pipe                通道号
-           pBuf                接收到的电表回码报文
-           rcvLen              接收的长度
-           mCbMtrPara          当前抄表的测量点的信息
-           pCbTaskID           抄表任务队列
-           bStdType            表规约类型
输出参数：
-           无
返回：
-           是否正常组帧
****************************************************************************/
uint16 STD188_PLC_AUTK_Recv(uint8 pipe,OOP_OAD_U mainOAD,OOP_OAD_U subOAD,uint8 *pBuf, uint16 rcvLen, uint16 Pn,uint16 MtrIndex, DateTimeBcd_t *StoreTime,SCHEME_TASK_RUN_INFO *pCbTask,CTS_RUN_TASK_T *RunTaskInfo)
{
    uint16               offSet=0;
    OadInfoMap       pOadInfo;
    const  MapOopTo645_NEW *pMapOopTo645;

    uint8                tmpBufer[255];           //协议转换，暂时定255个字节
    OOP_OAD_U                 findOAD;
    OOP_OAD_U                 tmpOad;

    //BOOL result=FALSE;
    DataSource sourcedata;
    uint8 desbuffer[2000];;
    uint16 desLen=0, task_id;
    OadStoreInfo StoreInfo;
    DateTimeBcd_t taskStoreTime;
    DateTimeBcd_t taskStartTime;
    time_t longTm;
    time_t colStartTm;
    //uint8 MtrAddr[6];
    uint16 MsIndex;
    OOP_METER_BASIC_T basicinfo;
    

    taskmng_mtr_basic_info_get(Pn,RunTaskInfo->logicid,&basicinfo);
    task_id=pCbTask->TaskId;

    findOAD = subOAD;
	MsIndex = taskmng_ms_index_get(pipe,Pn,RunTaskInfo->logicid);
    memset((uint8*)&StoreInfo,0x00,sizeof(OadStoreInfo));
    if(pCbTask->TaskId>255)
    {
        //内部任务处理
        //待增加

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

    //数据域-0x33
    //ReduceGap(tmpBufer, rcvLen, STD2007_GAP_VALUE);
    memcpy(sourcedata.sourcebuffer,tmpBufer,rcvLen);
    sourcedata.reclen=rcvLen;
    sourcedata.pwrType = basicinfo.pwrType;
    Convert_std188_To_Struct(sourcedata ,desbuffer,&desLen,pMapOopTo645);
    //数据转换

    //保存采集到数据
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
    taskmng_mtr_result_set(MtrIndex,RunTaskInfo->MtrResult);
    taskmng_task_record_file_set(pipe,MsIndex,RunTaskInfo->TaskId,RunTaskInfo->logicid);
    //taskmng_mtr_addr_get(pn,RunTaskInfo->logicid,MtrAddr);
    memcpy_r(StoreInfo.MAC.nValue,basicinfo.tsa.add,6);
    StoreInfo.recordOAD.infoNum=Pn;
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
    TASK_FMT_TRACE ( pipe,COMM_LOG, "r任务号:pCbTask->TaskId is %d\n",pCbTask->TaskId);
    TASK_BUF_TRACE ( pipe,COMM_LOG, desbuffer,desLen);
    taskmng_meter188_data_save(desbuffer,desLen,StoreInfo,pipe);   

    return offSet;
}


/*******************************************************************************
* 函数名称: 存储四表表记录型数据解析
* 函数功能:
* 输入参数: pipe 端口号
            DataBuf              //接收到的电表数据
            DataLen              //接收到的电表长度
            MtrIndex             //电表索引信息
            pTmp_NormalLink      //方案任务节点
            RecvRunTask     //任务运行时参数
            StoreTime            //存储时间
            flag   标记是否为应用分帧的解析，如果是应用分帧数据则csdindex不应该加
* 输出参数:
* 返 回 值:
*******************************************************************************/
uint16 taskmng_188_data_record_save(uint8 Pipe,uint8 *pBuf,uint16 DataLen,uint16 MtrIndex,SCHEME_TASK_RUN_INFO* pCbTask,CTS_RUN_TASK_T *RunTaskInfo)
{
    AmrThreadArg_T  *pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[Pipe];
    uint16          Pn = pAmrThreadArg->pMtrAmrInfo[MtrIndex].Pn;//测量点号

	OOP_METER_ANNEX_T       annex;
	uint8 meter_type,tmp,i=0;
	uint8 profileBuff[100];
	uint8           RoadIndex=0, Length = 0;
	OOP_TI_T ti;
    time_t oritime;
    OOP_OAD_U mainOAD, subOAD;
	DateTimeBcd_t storeTm = {0};
	DateTimeBcd_t tmpTime;
	DateTimeHex_t dateTimeHex;
	uint16 bias;
    uint8 testEnergy[20];
	uint16 oadvlaue;

	if(pCbTask->SchemeType == EVENTPLAN)
    {
        RoadIndex = pAmrThreadArg->pMtrAmrInfo[MtrIndex].ROADIndex;
        
    }

    annex =taskmng_mtr_annex_get(Pn,pAmrThreadArg->pMtrAmrInfo[MtrIndex].logicid);

	for(i=0;i<annex.nNum;i++)
	{
		if(annex.annexObj[i].oad.value == 0x40360200)
		{
			memcpy_r((uint8*)&oadvlaue, annex.annexObj[i].value,2);
			if(oadvlaue == 0x0310)
			{
				meter_type = 0x10;
			}
			else if(oadvlaue == 0x0310)
			{
				meter_type = 0x20;
			}
		}
	}
	if(meter_type==0)
	{
		meter_type = 0x10;  //默认水表
	}
	
	if((0x10 == (meter_type&0xF0)) || (0x30 == (meter_type&0xF0))) //水表
	   {
		   //当前累计流量，单位m*m*m-BCD
		   if(pBuf[0] == 0x2C)
		   {
			   memcpy(profileBuff,pBuf+1,4);
		   }
		   else
		   {
			   memcpy(profileBuff,pBuf,4);
		   }
	
		   //结算日累计流量，，单位m*m*m-BCD
		   if(pBuf[5] == 0x2C)
		   {
			   memcpy(profileBuff+4,pBuf+6,4);
		   }
		   else
		   {
			   memcpy(profileBuff+4,pBuf+5,4);
		   }
	
		   //实时时间-年月日时分秒-BCD
		   memcpy(profileBuff+8,pBuf+10,7);
	
		   //状态ST
		   tmp = pBuf[17];
		   *(profileBuff+15) = (tmp & 0x03);	   //阀门状态
		   *(profileBuff+16) = ((tmp>>2) & 0x01);  //电池电压
		   //XL_MemCpy(profileBuff+14,pBuf+16,2);
	   }
	   else if(0x20 == (meter_type&0xF0)) //热表----数据域中数据和单位的顺序还不好确定 ? 还有单位换算 ?
	   {
		   //结算日热量-BCD
		   memcpy(profileBuff,pBuf,4);
	
		   //当前热量-BCD
		   memcpy(profileBuff+4,pBuf+5,4);
	
		   //热功率-BCD
		   memcpy(profileBuff+8,pBuf+10,4);
	
		   //流量-BCD
		   memcpy(profileBuff+12,pBuf+15,4);
	
		   //累计流量-BCD
		   if(pBuf[20] == 0x2C)
		   {
			   memcpy(profileBuff+16,pBuf+21,4);
		   }
		   else
		   {
			   memcpy(profileBuff+16,pBuf+20,4);
		   }
	
		   //供水温度-BCD
		   memcpy(profileBuff+20,pBuf+25,3);
	
		   //回水温度-BCD
		   memcpy(profileBuff+23,pBuf+28,3);
	
		   //累计工作时间-BCD
		   memcpy(profileBuff+26,pBuf+31,3);
	
		   //实时时间-年月日时分秒-BCD
		   memcpy(profileBuff+29,pBuf+34,7);
	
		   //状态ST
		   //XL_MemCpy(profileBuff+35,pBuf+40,2);
		   tmp = pBuf[41];
		   *(profileBuff+36) = (tmp & 0x03);	   //阀门状态
		   *(profileBuff+37) = ((tmp>>2) & 0x01);  //电池电压
	   }
	       //存储时标处理
    memcpy_r((uint8*)&tmpTime,RunTaskInfo->ThisTurnBeginTime,7);
    DT_DateTimeBCD2Time(&tmpTime,&oritime);
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

        if(pCbTask->CsdTask.RoadTask[RoadIndex].MainOad.nObjID == 0x5003 ||
                pCbTask->CsdTask.RoadTask[RoadIndex].MainOad.nObjID == 0x5004 ||
                pCbTask->CsdTask.RoadTask[RoadIndex].MainOad.nObjID == 0x5005 ||
                pCbTask->CsdTask.RoadTask[RoadIndex].MainOad.nObjID == 0x5006)
        {
            storeTm = taskmng_acq_store_time_get(pCbTask->StoreTimeType, oritime);
        }
        else
        {
            DT_Time2DateTimeBCD(oritime, &storeTm);
        }
    }
   
//  
//    if(pCbTask->CsdTask.RoadTask[RoadIndex].MainOad.nObjID == 0)  //OAD
//    {
//        mainOAD.value = 0x00000000;
//        
//        subOAD = pCbTask->CsdTask.RoadTask[RoadIndex].OadListInfo[SubOadIndex];;
//
//        bias = CaclBiasByOad_188(meter_type, subOAD, &Length);
//        if(bias != 0xFFFF)
//        {
//            memset(testEnergy, 0xEE, sizeof(testEnergy));
//            memcpy(testEnergy, &profileBuff[bias], Length);
//            STD188_PLC_AUTK_Recv(Pipe, mainOAD, subOAD, testEnergy, sizeof(testEnergy), Pn,NULL, pCbTask,RunTaskInfo);
//        }
//    }
//    else //ROAD
    {       
        while(i < pCbTask->CsdTask.RoadTask[RoadIndex].SubOadNum)
        {
            mainOAD.value= pCbTask->CsdTask.RoadTask[RoadIndex].MainOad.value;
            subOAD = pCbTask->CsdTask.RoadTask[RoadIndex].OadListInfo[i];

            if(subOAD.value == 0x20210200)
            {
                i++;
                DT_DateTimeBCD2Hex(&storeTm,&dateTimeHex);
                memcpy(testEnergy,(uint8*)&dateTimeHex,sizeof(dateTimeHex));
                STD188_PLC_AUTK_Recv(Pipe, mainOAD, subOAD, testEnergy, sizeof(testEnergy),Pn,MtrIndex, NULL, pCbTask,RunTaskInfo);
                continue;
            }
            bias = CaclBiasByOad_188(meter_type, subOAD, &Length);
            if(bias != 0xFFFF)
            {
                memset(testEnergy, 0xEE, sizeof(testEnergy));
                memcpy(testEnergy, &profileBuff[bias], Length);

                if(pCbTask->AcqInfo.Acqtype == ACQTYP_NULL)
                {

                    STD188_PLC_AUTK_Recv(Pipe, mainOAD, subOAD, testEnergy, sizeof(testEnergy), Pn,MtrIndex,NULL, pCbTask,RunTaskInfo);
                }
                else
                {
                    STD188_PLC_AUTK_Recv(Pipe, mainOAD, subOAD, testEnergy, sizeof(testEnergy), Pn,MtrIndex,&storeTm, pCbTask,RunTaskInfo);
                }
            }

            i++;
        }
        
    }
 
	return 0;
}


/*******************************************************************************
* 函数名称: taskmng_645_meter_data_save_event
* 函数功能: 645表07规约事件存储
* 输入参数: pBuf          645回码数据
*           rcvLen        645回码长度
*           MtrIndex      电表索引
*           Tmp_AmrLink   任务信息
            RecvRunTask     任务运行时参数
* 输出参数: 无
* 返 回 值: FALSE 事件序号为0或者序号和上次抄的相同，无需再抄 TRUE 正常
*******************************************************************************/
BOOL taskmng_645_meter_data_save_event(uint8 Pipe,uint8 *pBuf, uint16 rcvLen,int MsIndex,SCHEME_TASK_RUN_INFO* pTmp_AmrLink,CTS_RUN_TASK_T *RecvRunTask )
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[Pipe];
    time_t                       StartTime,EndTime;
    DateTimeBcd_t                TmpTime;
    uint8                        RoadIndex;
    uint8                        CSDIndex;
    const EventMapOopTo645_t *pEventMapList;
    uint8                        SubOadNum;
    uint8                        i = 0;
    uint8                        DataLen;
    uint16                       bias = 0xFFFF;
    uint32              EventSeq;       //抄到的事件序号
    uint32              LastSeq;        //上次抄到的事件序号
    uint16 Pn;
    OOP_OAD_U       MainOad;
    memset(&MainOad,0x00,sizeof(OOP_OAD_U));
    memcpy_r((uint8*)&TmpTime,(uint8*)RecvRunTask->LastTurnBeginTime,7);
    printf("上轮启动时间:%02x%02x-%02x-%02x %02x:%02x:%02x\n", TmpTime.year_h, TmpTime.year_l,
           TmpTime.month, TmpTime.day, TmpTime.hour, TmpTime.min, TmpTime.sec);
    DT_DateTimeBCD2Time(&TmpTime,&StartTime);

    memcpy_r((uint8*)&TmpTime,(uint8*)RecvRunTask->ThisTurnBeginTime,7);
    printf("本轮启动时间:%02x%02x-%02x-%02x %02x:%02x:%02x\n", TmpTime.year_h, TmpTime.year_l,
           TmpTime.month, TmpTime.day, TmpTime.hour, TmpTime.min, TmpTime.sec);
    DT_DateTimeBCD2Time(&TmpTime,&EndTime);
    Pn = pAmrThreadArg->pMtrAmrInfo[MsIndex].Pn;
    RoadIndex = pAmrThreadArg->pMtrAmrInfo[MsIndex].ROADIndex;
    CSDIndex = pAmrThreadArg->pMtrAmrInfo[MsIndex].CSDIndex;
    MainOad = pTmp_AmrLink->CsdTask.RoadTask[RoadIndex].MainOad;
    pEventMapList = taskmng_event_map_get(MainOad);
    SubOadNum = pTmp_AmrLink->CsdTask.RoadTask[RoadIndex].SubOadNum;
    TASK_FMT_TRACE(Pipe, REC_LOG,"645事件存储 RoadIndex %d CSDIndex %d\n",RoadIndex,CSDIndex);
    if(pTmp_AmrLink->CsdTask.RoadTask[RoadIndex].OadListInfo[CSDIndex].value ==0x20220200)
    {
        EventSeq = commfun_BcdToHex(pBuf[2])*10000+commfun_BcdToHex(pBuf[1])*100+commfun_BcdToHex(pBuf[0]);
        
        LastSeq = GetTaskReportSeq( Pipe, RecvRunTask->TaskId,RecvRunTask->logicid, MainOad, Pn);
        TASK_FMT_TRACE(Pipe, REC_LOG,"抄到的事件序号 %d 上次存储的事件序号 %d \n",EventSeq,LastSeq);
        if(EventSeq==0 ||EventSeq==LastSeq)
        {
           return FALSE;
        }
        SetTaskReportSeq( Pipe,RecvRunTask->TaskId,RecvRunTask->logicid, MainOad, Pn, EventSeq);
        taskmng_645_meter_data_save( Pipe,pBuf, rcvLen, MsIndex, pTmp_AmrLink,RecvRunTask,NULL);
        return TRUE;
    }
    /********************事件数据标识单抄数据项的解析************************************/
    if(pEventMapList->DIAtr == DI_SINGL)
    {
        taskmng_645_meter_data_save( Pipe,pBuf, rcvLen, MsIndex, pTmp_AmrLink,RecvRunTask,NULL);
        return TRUE;
    }
     /********************事件数据标识块抄数据项的解析************************************/
    TASK_FMT_TRACE(Pipe, REC_LOG,"645事件快抄存储 RoadIndex %d CSDIndex %d\n",RoadIndex,CSDIndex);
    i = pAmrThreadArg->pMtrAmrInfo[MsIndex].CSDIndex;
    while(i < SubOadNum)
    {
        pAmrThreadArg->pMtrAmrInfo[MsIndex].CSDIndex = i;           //当前CSD索引

        //计算OAD对应的数据偏移信息
        bias = CacloffsByOad(Pipe,pTmp_AmrLink,MsIndex, &DataLen);
        //事件记录序号为单抄，不需要计算偏移
        if(pTmp_AmrLink->CsdTask.RoadTask[RoadIndex].OadListInfo[i].value == 0x20220200)
            bias = 0;
        if(bias == 0xFFFF)
        {
            i++;
            pAmrThreadArg->pMtrAmrInfo[MsIndex].MapIndex = 0;
            continue;
        }

        if((bias+DataLen) > rcvLen)
        {
            i++;
            pAmrThreadArg->pMtrAmrInfo[MsIndex].MapIndex = 0;
            continue;
        }

        taskmng_645_meter_data_save(Pipe,&pBuf[bias], DataLen,MsIndex, pTmp_AmrLink,RecvRunTask,NULL);
        i++;
    }
    return TRUE;
}
/*******************************************************************************
* 函数名称: taskmng_645_meter_data_save
* 函数功能: 对97、07表回码进行格式转换，填充存储函数接口进行存储
* 输入参数: pipe        端口号
            pBuf          645回码数据
*           rcvLen        645回码长度
*           MtrIndex      电表索引
*           Tmp_AmrLink   方案任务节点
*           StoreTime     数据存储时间
* 输出参数: 无
* 返 回 值: 无
*******************************************************************************/
void taskmng_645_meter_data_save(uint8 Pipe,uint8 *pBuf, uint16 rcvLen, int MsIndex, SCHEME_TASK_RUN_INFO* Tmp_AmrLink,CTS_RUN_TASK_T *RecvRunTask, DateTimeBcd_t *StoreTime)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[Pipe];
    uint16 Pn = pAmrThreadArg->pMtrAmrInfo[MsIndex].Pn;
    uint8 logicid =  pAmrThreadArg->pMtrAmrInfo[MsIndex].logicid;
    //uint8 prtl ;            
    OOP_OAD_U   MainOad,subOad;
    uint8   RoadIndex=0;
    uint8   CSDIndex=0;
    uint16  MapIndex=0;
    const MapOopTo645_NEW *pOadInfo=NULL;
    DataSource sourcedata;
    uint16 DataLen = 0;
    uint8 DataBuff[200]={0};
    //taskmng_mtr_prtl_get( Pn, logicid,&prtl);
    OadStoreInfo StoreInfo;
    memset(&StoreInfo,0x00,sizeof(OadStoreInfo));
    int ret;
    DateTimeBcd_t  tmpTime;
    const MapOopTo645_NEW*     Map;
    const EventMapOopTo645_t *pEventMapList;
    uint16  MapNum;
    OOP_METER_BASIC_T basicinfo;
    taskmng_mtr_basic_info_get( Pn, logicid,&basicinfo);

    //const MapOopTo645_NEW*     pMapOopTo645;
    if(Tmp_AmrLink->SchemeType==EVENTPLAN)
    {
        RoadIndex = pAmrThreadArg->pMtrAmrInfo[MsIndex].ROADIndex;
        MainOad = Tmp_AmrLink->CsdTask.RoadTask[RoadIndex].MainOad;
        CSDIndex = pAmrThreadArg->pMtrAmrInfo[MsIndex].CSDIndex;
        subOad = Tmp_AmrLink->CsdTask.RoadTask[RoadIndex].OadListInfo[CSDIndex];
        MapIndex = pAmrThreadArg->pMtrAmrInfo[MsIndex].MapIndex;
    }else
    {
        MainOad = Tmp_AmrLink->CsdTask.RoadTask[0].MainOad;
        CSDIndex = pAmrThreadArg->pMtrAmrInfo[MsIndex].CSDIndex;
        subOad = Tmp_AmrLink->CsdTask.RoadTask[0].OadListInfo[CSDIndex];
        MapIndex = pAmrThreadArg->pMtrAmrInfo[MsIndex].MapIndex;
    }

    //获取pOadInfo内容
    if(Tmp_AmrLink->SchemeType==EVENTPLAN)
    {
        pEventMapList = taskmng_event_map_get(MainOad);
        Map = pEventMapList->EventMap;
        pOadInfo = &Map[MapIndex];
    }else
    {
        Map = taskmng_map_get_from_mainoad(Pipe,MsIndex,Tmp_AmrLink,&MapNum);
        pOadInfo = &Map[MapIndex];
    }
    //TASK_FMT_TRACE(Pipe, REC_LOG,"taskmng_645_meter_data_save  \n");
    //TASK_FMT_TRACE(Pipe, REC_LOG,"mainoda 0x%08x CSDIndex %d  subOad 0x%08x mapindex %d \n",MainOad.value,CSDIndex,subOad.value,MapIndex);
   // TASK_FMT_TRACE(Pipe, REC_LOG,"poadinfo oad  0x%08x 07DI %08x  CovertSruct %d \n",pOadInfo->Oad.value,pOadInfo->DI07,pOadInfo->CovertSruct);
    
    ret =CacloffSpecial(Pipe,MsIndex,pOadInfo,MapIndex,pBuf,&rcvLen);
    if(ret<0)
        return;
    sourcedata.pwrType = basicinfo.pwrType;
    sourcedata.reclen = rcvLen;
    memcpy(sourcedata.sourcebuffer,pBuf,rcvLen);
   // TASK_FMT_TRACE(Pipe, REC_LOG,"rcvLen %d 抄表回复数据\n",rcvLen);

   // TASK_BUF_TRACE(Pipe, REC_LOG,pBuf, rcvLen);
    Convert_6457_07_To_Struct(sourcedata,DataBuff,&DataLen,pOadInfo);
   // TASK_FMT_TRACE(Pipe, REC_LOG,"Convert_6457_07_To_Struct  DataLen %d 转换后数据\n",DataLen);

   // TASK_BUF_TRACE(Pipe, REC_LOG,DataBuff, DataLen);

    StoreInfo.recordOAD.logicId=logicid;
    StoreInfo.recordOAD.infoNum = Pn;

    StoreInfo.recordOAD.road.oadMain=MainOad;
    StoreInfo.recordOAD.road.oadCols.nNum=1;
    StoreInfo.recordOAD.road.oadCols.oad[0]=subOad;
    StoreInfo.recordOAD.classtag = 5;
    taskmng_mtr_addr_get( Pn, logicid,(uint8*)StoreInfo.MAC.nValue);
    memcpy_r((uint8*)&tmpTime,(uint8*)RecvRunTask->ThisTurnBeginTime,7);
    DT_DateTimeBCD2Time(&tmpTime,(time_t*)&StoreInfo.colStartTm);     //开始时间

    if(StoreTime==NULL)
    {
        time_t storetm;
        OOP_TI_T delay;
        DT_DateTimeBCD2Time(&tmpTime,&storetm);
        delay.unit = Tmp_AmrLink->DelayTimeUnit;
        delay.value = Tmp_AmrLink->DelayTime;
        DT_TimeSubInv(&storetm, delay);
        tmpTime = taskmng_acq_store_time_get(Tmp_AmrLink->StoreTimeType,storetm);
        DT_DateTimeBCD2Time(&tmpTime,(time_t*)&StoreInfo.colStoreTm);
    }
    else
    {
        DT_DateTimeBCD2Time(StoreTime,(time_t*)&StoreInfo.colStoreTm);
        TASK_FMT_TRACE(Pipe, REC_LOG,"StoreTm 20%02x/%02x/%02x %02x:%02x:%02x \n",StoreTime->year_l,StoreTime->month,StoreTime->day,StoreTime->hour,StoreTime->min,StoreTime->sec);
    }
    StoreInfo.colEndTm = time(NULL);
    taskmng_meter_data_save(DataBuff,DataLen ,StoreInfo, Pipe );
#ifdef AREA_HUNAN
    if(MainOad.value == 0x50020200 && Pn>=800)
    {
        memset(&StoreInfo.recordOAD.road.oadMain,0x0,sizeof(OOP_OAD_U));
        taskmng_meter_data_save(DataBuff,DataLen ,StoreInfo, Pipe );
    }
#endif
#ifdef AREA_JIANGXI
    //通过判断电表运行状态字来判断是否发生了逆向序
    //只要有一次发生逆向序，则置标志位
    if (MainOad.value == 0x50020200 && subOad.value == 0x20140200)
    {
        OOP_METWORD_T metWord;
        memcpy(&metWord, DataBuff, DataLen);
        if (metWord.metWord[6] & 0x03) //电表运行状态字7        第0位:电压逆相序 第1位:电流逆相序
            pAmrThreadArg->pMtrAmrInfo[MsIndex].reversePhase = 1;
    }

    /* 判断零线电流是否大于0.5A */
    if (MainOad.value == 0x50020200 && subOad.value == 0x20010400)
    {
        int32   zeroCurrent;
        memcpy(&zeroCurrent, DataBuff, DataLen);
        if (zeroCurrent > 500)
            pAmrThreadArg->pMtrAmrInfo[MsIndex].zeroCurrentFlag = 1;
    }
#endif    

}

/*******************************************************************************
* 函数名称: taskmng_645_meter_data_save
* 函数功能: 对97、07表回码进行格式转换，填充存储函数接口进行存储
* 输入参数: pipe        端口号
            pBuf          645回码数据
*           rcvLen        645回码长度
*           MtrIndex      电表索引
*           Tmp_AmrLink   方案任务节点
*           StoreTime     数据存储时间
* 输出参数: 无
* 返 回 值: 无
*******************************************************************************/
void taskmng_645_meter_data_save_new(uint8 Pipe,uint16 mapindex,uint8 *pBuf, uint16 rcvLen, int MsIndex, SCHEME_TASK_RUN_INFO* Tmp_AmrLink,CTS_RUN_TASK_T *RecvRunTask, DateTimeBcd_t *StoreTime)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[Pipe];
    uint16 Pn = pAmrThreadArg->pMtrAmrInfo[MsIndex].Pn;
    uint8 logicid =  pAmrThreadArg->pMtrAmrInfo[MsIndex].logicid;
    //uint8 prtl ;            
    OOP_OAD_U   MainOad,subOad;
    uint8   RoadIndex=0;
    uint8   CSDIndex=0;
    //uint16  MapIndex=0;
    const MapOopTo645_NEW *pOadInfo=NULL;
    DataSource sourcedata;
    uint16 DataLen = 0;
    uint8 DataBuff[200]={0};
    OadStoreInfo StoreInfo;
    memset(&StoreInfo,0x00,sizeof(OadStoreInfo));

    int ret;
    DateTimeBcd_t  tmpTime;
    const MapOopTo645_NEW*     Map;
    const EventMapOopTo645_t *pEventMapList;
    uint16  MapNum;
    OOP_METER_BASIC_T basicinfo;
    uint32 starttime,storetime;
    taskmng_mtr_basic_info_get( Pn, logicid,&basicinfo);

    //const MapOopTo645_NEW*     pMapOopTo645;
    if(Tmp_AmrLink->SchemeType==EVENTPLAN)
    {
        RoadIndex = pAmrThreadArg->pMtrAmrInfo[MsIndex].ROADIndex;
        MainOad = Tmp_AmrLink->CsdTask.RoadTask[RoadIndex].MainOad;
        CSDIndex = pAmrThreadArg->pMtrAmrInfo[MsIndex].tmpcsdindex;
        subOad = Tmp_AmrLink->CsdTask.RoadTask[RoadIndex].OadListInfo[CSDIndex];

    }else
    {
        MainOad = Tmp_AmrLink->CsdTask.RoadTask[0].MainOad;
        CSDIndex = pAmrThreadArg->pMtrAmrInfo[MsIndex].tmpcsdindex;
        subOad = Tmp_AmrLink->CsdTask.RoadTask[0].OadListInfo[CSDIndex];
    }

    //获取pOadInfo内容
    if(Tmp_AmrLink->SchemeType==EVENTPLAN)
    {
        pEventMapList = taskmng_event_map_get(MainOad);
        Map = pEventMapList->EventMap;
        pOadInfo = &Map[mapindex];
    }else
    {
        Map = taskmng_map_get_from_mainoad(Pipe,MsIndex,Tmp_AmrLink,&MapNum);
        pOadInfo = &Map[mapindex];
    }
    //TASK_FMT_TRACE(Pipe, REC_LOG,"taskmng_645_meter_data_save  \n");
    //TASK_FMT_TRACE(Pipe, REC_LOG,"mainoda 0x%08x CSDIndex %d  subOad 0x%08x mapindex %d \n",MainOad.value,CSDIndex,subOad.value,MapIndex);
   // TASK_FMT_TRACE(Pipe, REC_LOG,"poadinfo oad  0x%08x 07DI %08x  CovertSruct %d \n",pOadInfo->Oad.value,pOadInfo->DI07,pOadInfo->CovertSruct);
    
    ret =CacloffSpecial(Pipe,MsIndex,pOadInfo,mapindex,pBuf,&rcvLen);
    if(ret<0)
        return;
    sourcedata.pwrType = basicinfo.pwrType;
    sourcedata.reclen = rcvLen;
    memcpy(sourcedata.sourcebuffer,pBuf,rcvLen);
    //TASK_FMT_TRACE(Pipe, REC_LOG,"rcvLen %d 抄表回复数据\n",rcvLen);
    //TASK_BUF_TRACE(Pipe, REC_LOG,pBuf, rcvLen);
    Convert_6457_07_To_Struct(sourcedata,DataBuff,&DataLen,pOadInfo);
    TASK_FMT_TRACE(Pipe, REC_LOG,"Convert_6457_07_To_Struct  DataLen %d 转换后数据\n",DataLen);
    TASK_BUF_TRACE(Pipe, REC_LOG,DataBuff, DataLen);

    memcpy_r((uint8*)&tmpTime,(uint8*)RecvRunTask->ThisTurnBeginTime,7);
    DT_DateTimeBCD2Time(&tmpTime,(time_t*)&starttime);     //开始时间

    if(StoreTime==NULL)
    {
        time_t storetm;
        OOP_TI_T delay;
        DT_DateTimeBCD2Time(&tmpTime,&storetm);
        delay.unit = Tmp_AmrLink->DelayTimeUnit;
        delay.value = Tmp_AmrLink->DelayTime;
        DT_TimeSubInv(&storetm, delay);
        tmpTime = taskmng_acq_store_time_get(Tmp_AmrLink->StoreTimeType,storetm);
        DT_DateTimeBCD2Time(&tmpTime,(time_t*)&storetime);
    }
    else
    {
        DT_DateTimeBCD2Time(StoreTime,(time_t*)&storetime);
        if(CSDIndex==0)
            TASK_FMT_TRACE(Pipe, REC_LOG,"StoreTm 20%02x/%02x/%02x %02x:%02x:%02x \n",StoreTime->year_l,StoreTime->month,StoreTime->day,StoreTime->hour,StoreTime->min,StoreTime->sec);
    }
    
    StoreInfo.recordOAD.logicId=logicid;
    StoreInfo.recordOAD.infoNum = Pn;

    StoreInfo.recordOAD.road.oadMain=MainOad;
    StoreInfo.recordOAD.road.oadCols.nNum=1;
    StoreInfo.recordOAD.road.oadCols.oad[0]=subOad;
    StoreInfo.recordOAD.classtag = 5;
    taskmng_mtr_addr_get( Pn, logicid,(uint8*)StoreInfo.MAC.nValue);
    memcpy_r((uint8*)&tmpTime,(uint8*)RecvRunTask->ThisTurnBeginTime,7);
    DT_DateTimeBCD2Time(&tmpTime,(time_t*)&StoreInfo.colStartTm);     //开始时间

    if(StoreTime==NULL)
    {
        time_t storetm;
        OOP_TI_T delay;
        DT_DateTimeBCD2Time(&tmpTime,&storetm);
        delay.unit = Tmp_AmrLink->DelayTimeUnit;
        delay.value = Tmp_AmrLink->DelayTime;
        DT_TimeSubInv(&storetm, delay);
        tmpTime = taskmng_acq_store_time_get(Tmp_AmrLink->StoreTimeType,storetm);
        DT_DateTimeBCD2Time(&tmpTime,(time_t*)&StoreInfo.colStoreTm);
    }
    else
    {
        DT_DateTimeBCD2Time(StoreTime,(time_t*)&StoreInfo.colStoreTm);
        TASK_FMT_TRACE(Pipe, REC_LOG,"StoreTm 20%02x/%02x/%02x %02x:%02x:%02x \n",StoreTime->year_l,StoreTime->month,StoreTime->day,StoreTime->hour,StoreTime->min,StoreTime->sec);
    }
    StoreInfo.colEndTm = time(NULL);
    //为了提高采集效率 645实时数据和日冻结任务都缓存起来最后一起存
    if(Tmp_AmrLink->AcqInfo.Acqtype ==ACQTYP_NULL || Tmp_AmrLink->CsdTask.RoadTask[0].MainOad.value == 0x50040200)
    {
        taskmng_cache_buf_save(Pipe,Pn,MainOad,subOad,DataBuff,DataLen,StoreInfo.colStartTm,StoreInfo.colStoreTm);
    }else
    {
        taskmng_meter_data_save(DataBuff,DataLen ,StoreInfo, Pipe );
    }
#ifdef AREA_HUNAN
    if(MainOad.value == 0x50020200 && Pn>=800)
    {
        memset(&StoreInfo.recordOAD.road.oadMain,0x0,sizeof(OOP_OAD_U));
        taskmng_meter_data_save(DataBuff,DataLen ,StoreInfo, Pipe );
    }
#endif
#ifdef AREA_JIANGXI
    //通过判断电表运行状态字来判断是否发生了逆向序
    //只要有一次发生逆向序，则置标志位
    if (MainOad.value == 0x50020200 && subOad.value == 0x20140200)
    {
        OOP_METWORD_T metWord;
        memcpy(&metWord, DataBuff, DataLen);
        if (metWord.metWord[6] & 0x03) //电表运行状态字7        第0位:电压逆相序 第1位:电流逆相序
            pAmrThreadArg->pMtrAmrInfo[MsIndex].reversePhase = 1;
    }

    /* 判断零线电流是否大于0.5A */
    if (MainOad.value == 0x50020200 && subOad.value == 0x20010400)
    {
        int32   zeroCurrent;
        memcpy(&zeroCurrent, DataBuff, DataLen);
        if (zeroCurrent > 500)
            pAmrThreadArg->pMtrAmrInfo[MsIndex].zeroCurrentFlag = 1;
    }
#endif    

}


/*******************************************************************************
* 函数名称: CacloffSpecial
* 函数功能: 645表时间需要特殊处理
 输入参数: pipe        端口号
        MsIndex      电表索引
        pOadInfo    面向对象和645数据项的转换结构
        MapIndex    645map索引
            pBuf          645回码数据
*           rcvLen        645回码长度

* 输出参数: 无
* 返 回 值: -1 回复的不全暂存在内存中 0 时间组合完毕，可以存储 
*******************************************************************************/
int CacloffSpecial(uint8 Pipe,uint16 MsIndex,const MapOopTo645_NEW *pOadInfo,uint16  MapIndex,uint8 *pBuf, uint16 *rcvLen)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[Pipe];
    const MapOopTo645_NEW * tmp =pOadInfo;
    uint8 TmpBuff[200] = {0};
    uint8 prtl;
    taskmng_mtr_prtl_get(pAmrThreadArg->pMtrAmrInfo[MsIndex].Pn,pAmrThreadArg->pMtrAmrInfo[MsIndex].logicid,&prtl);
    tmp++;
    if(pOadInfo->Oad.value == tmp->Oad.value)
    {
        if(prtl==PRTL_07)
        {
            if(pOadInfo->DI07!=tmp->DI07&&pOadInfo->DI07!=0xffffffff&&tmp->DI07!=0xffffffff)
            {
                memset(pAmrThreadArg->pMtrAmrInfo[MsIndex].SpecialTmpBuf,0x00,MAX_SPECIAL_TMP_LEN);
                memcpy(pAmrThreadArg->pMtrAmrInfo[MsIndex].SpecialTmpBuf,pBuf,*rcvLen);
                pAmrThreadArg->pMtrAmrInfo[MsIndex].SpecialTmpLen = *rcvLen;
                return -1;
            }
        }
        else if(prtl==PRTL_97)
        {
            if(pOadInfo->DI97!=tmp->DI97&&pOadInfo->DI97!=0xffff&&tmp->DI97!=0xffff)
            {
                memset(pAmrThreadArg->pMtrAmrInfo[MsIndex].SpecialTmpBuf,0x00,MAX_SPECIAL_TMP_LEN);
                memcpy(pAmrThreadArg->pMtrAmrInfo[MsIndex].SpecialTmpBuf,pBuf,*rcvLen);
                pAmrThreadArg->pMtrAmrInfo[MsIndex].SpecialTmpLen = *rcvLen;
                return -1;
            }
        }
    }
    if(MapIndex>0)
    {
        tmp =pOadInfo;
        tmp--;
        if(pOadInfo->Oad.value == tmp->Oad.value)
        {
            if(prtl==PRTL_07)
            {
                if(pOadInfo->DI07!=tmp->DI07&&pOadInfo->DI07!=0xffffffff&&tmp->DI07!=0xffffffff)
                {
                    memcpy(TmpBuff,pAmrThreadArg->pMtrAmrInfo[MsIndex].SpecialTmpBuf,pAmrThreadArg->pMtrAmrInfo[MsIndex].SpecialTmpLen );
                    memcpy(&TmpBuff[pAmrThreadArg->pMtrAmrInfo[MsIndex].SpecialTmpLen],pBuf,*rcvLen);
                    *rcvLen +=pAmrThreadArg->pMtrAmrInfo[MsIndex].SpecialTmpLen;
                    memcpy(pBuf,TmpBuff,*rcvLen);
                    
                }
            }
            else if(prtl==PRTL_97)
            {
                if(pOadInfo->DI97!=tmp->DI97&&pOadInfo->DI97!=0xffff&&tmp->DI97!=0xffff)
                {
                    memcpy(TmpBuff,pAmrThreadArg->pMtrAmrInfo[MsIndex].SpecialTmpBuf,pAmrThreadArg->pMtrAmrInfo[MsIndex].SpecialTmpLen );
                    memcpy(&TmpBuff[pAmrThreadArg->pMtrAmrInfo[MsIndex].SpecialTmpLen],pBuf,*rcvLen);
                    *rcvLen +=pAmrThreadArg->pMtrAmrInfo[MsIndex].SpecialTmpLen;
                    memcpy(pBuf,TmpBuff,*rcvLen);
                }
            }
        }
    }

    return 0;
}


/*******************************************************************************
* 函数名称: taskmng_645_meter_data_save_frz
* 函数功能: 处理97、07表的时标，供按冻结时标采集方式抄表使用
* 输入参数: pBuf          645回码数据
*           rcvLen        645回码长度
*           MtrIndex      电表索引
*           Tmp_AmrLink   方案任务节点
* 输出参数: 无
* 返 回 值: 无
*******************************************************************************/
uint16 taskmng_645_meter_data_save_frz(uint8 Pipe,uint8 *pBuf, uint16 rcvLen, int MsIndex, SCHEME_TASK_RUN_INFO* Tmp_AmrLink,CTS_RUN_TASK_T *RecvRunTask)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[Pipe];
    DateTimeBcd_t       storeTm;
    time_t              longTm;
    time_t              StartTime,endTime;
    DateTimeBcd_t       tmpTime;
    OOP_TI_T                ti;
    uint8               CSDIndex = pAmrThreadArg->pMtrAmrInfo[MsIndex].CSDIndex;
    
    if(Tmp_AmrLink->CsdTask.RoadTask[0].MainOad.nObjID== 0x5003 && taskmng_task_period_get(Tmp_AmrLink->TaskId,Tmp_AmrLink->logicid) < 86400L)
    {
        taskmng_645_meter_data_save(Pipe,pBuf, rcvLen,MsIndex, Tmp_AmrLink, RecvRunTask,NULL);
        return 0;
    }

    if(Tmp_AmrLink->CsdTask.RoadTask[0].MainOad.nObjID == 0x5002)
    {
#ifdef AREA_HUNAN
        if(taskmng_HPLC_module_curve_flag_get()==1 &&pAmrThreadArg->pMtrAmrInfo[MsIndex].stacurvestate ==1)
        {
            taskmng_645_meter_data_save_stacurve(Pipe,pBuf, rcvLen, MsIndex, Tmp_AmrLink, RecvRunTask);
            return 0 ;
        }
#endif
        uint16 Pn = pAmrThreadArg->pMtrAmrInfo[MsIndex].Pn;
#ifdef AREA_SHANDONG
        OOP_OAD_U suboad = Tmp_AmrLink->CsdTask.RoadTask[0].OadListInfo[CSDIndex];
        if(taskmng_energy_oad_check(suboad) == FALSE)
        {
            taskmng_645_meter_data_save(Pipe,pBuf, rcvLen, MsIndex, Tmp_AmrLink, RecvRunTask,NULL);
            return 0;
        }
#endif 
        if(taskmng_mtr_conntype_get(Pn,pAmrThreadArg->pMtrAmrInfo[MsIndex].logicid)==1)
        {
                        //判断flag
            if(pAmrThreadArg->pMtrAmrInfo[MsIndex].flag645 == 0)
            {
                taskmng_645_meter_data_save_curve(Pipe,pBuf, rcvLen, MsIndex, Tmp_AmrLink, RecvRunTask);
            }else
            {
                if(pAmrThreadArg->pMtrAmrInfo[MsIndex].flag645 == 1 ||taskmng_task_period_check(Tmp_AmrLink)==TRUE)
                {
                    taskmng_645_meter_data_save(Pipe,pBuf, rcvLen, MsIndex, Tmp_AmrLink, RecvRunTask,NULL);
                }else
                {
                    taskmng_645_meter_data_save_profile(Pipe,pBuf, rcvLen, MsIndex, Tmp_AmrLink,RecvRunTask);
                }
            }
//        
//            if(taskmng_task_period_check(Tmp_AmrLink)==TRUE)
//            {
//                taskmng_645_meter_data_save(Pipe,pBuf, rcvLen, MsIndex, Tmp_AmrLink, RecvRunTask,NULL);
//            }else
//            {
//                taskmng_645_meter_data_save_curve(Pipe,pBuf, rcvLen, MsIndex, Tmp_AmrLink, RecvRunTask);
//            }
        }else
        {
            if(pAmrThreadArg->pMtrAmrInfo[MsIndex].flag645 == 0)
            {
                taskmng_645_meter_data_save_profile(Pipe,pBuf, rcvLen, MsIndex, Tmp_AmrLink,RecvRunTask);
            }else
            {
                taskmng_645_meter_data_save(Pipe,pBuf, rcvLen, MsIndex, Tmp_AmrLink, RecvRunTask,NULL);
            }
        
//            if(taskmng_task_period_check(Tmp_AmrLink)==TRUE||taskmng_task_period_get(Tmp_AmrLink->TaskId,Tmp_AmrLink->logicid) < 86400L)
//            {
//                taskmng_645_meter_data_save(Pipe,pBuf, rcvLen, MsIndex, Tmp_AmrLink, RecvRunTask,NULL);
//            }else
//            {
//                taskmng_645_meter_data_save_profile(Pipe,pBuf, rcvLen, MsIndex, Tmp_AmrLink,RecvRunTask);
//            }
        }
        return 0;
    }

    if(Tmp_AmrLink->CsdTask.RoadTask[0].MainOad.nObjID == 0x5004&&pAmrThreadArg->pMtrAmrInfo[MsIndex].EndNum==0xff)
    {
        TASK_FMT_TRACE(Pipe, REC_LOG,"日冻结EndNum 0xff\n");
        taskmng_645_meter_data_save(Pipe,pBuf, rcvLen, MsIndex, Tmp_AmrLink, RecvRunTask,NULL);
        return 0;
    }
    
    
    memcpy_r((uint8*)&tmpTime,(uint8*)RecvRunTask->LastTurnBeginTime,7);
    //TASK_FMT_TRACE(Pipe, REC_LOG,"上轮启动时间:%02x%02x-%02x-%02x %02x:%02x:%02x\n", tmpTime.year_h, tmpTime.year_l,
           //tmpTime.month, tmpTime.day, tmpTime.hour, tmpTime.min, tmpTime.sec);
    DT_DateTimeBCD2Time(&tmpTime,&StartTime);
    memcpy_r((uint8*)&tmpTime,(uint8*)RecvRunTask->ThisTurnBeginTime,7);
    //TASK_FMT_TRACE(Pipe, REC_LOG,"本轮启动时间:%02x%02x-%02x-%02x %02x:%02x:%02x\n", tmpTime.year_h, tmpTime.year_l,
         //  tmpTime.month, tmpTime.day, tmpTime.hour, tmpTime.min, tmpTime.sec);
    DT_DateTimeBCD2Time(&tmpTime,&endTime);
    if(Tmp_AmrLink->AcqInfo.Acqtype == ACQTYP_FRZTM || Tmp_AmrLink->AcqInfo.Acqtype == ACQTYP_TI
            || Tmp_AmrLink->StoreTimeType == STRTMTYP_FRZTIME )
    {
        //按照冻结时标采集，需要判断数据是否在时间间隔范围内
        if(CSDIndex == 0)
        {
            if(Tmp_AmrLink->CsdTask.RoadTask[0].OadListInfo[CSDIndex].nObjID != 0x2021)//第一列必须是冻结时标
            {
                pAmrThreadArg->pMtrAmrInfo[MsIndex].EndNum=0;
                return 0;
            }

            if(Tmp_AmrLink->CsdTask.RoadTask[0].MainOad.nObjID == 0x5006 || Tmp_AmrLink->CsdTask.RoadTask[0].MainOad.nObjID == 0x5005)
            {
                memcpy_r(&storeTm.year_l,pBuf,4);
                storeTm.min = 0;
            }
            else
                memcpy_r(&storeTm.year_l,pBuf,5);

            storeTm.year_h = 0x20;
            storeTm.sec= 0x00;
            storeTm.min= 0x00;
            if(Tmp_AmrLink->CsdTask.RoadTask[0].MainOad.nObjID!= 0x5003)
                storeTm.hour= 0x00;
            // commfun_printstring((uint8*)&storeTm,7);//打印采集到的存储时间
            TASK_FMT_TRACE(Pipe, REC_LOG,"采集到的冻结时标 %02x年 %02x月%02x日 %02x %02x %02x \n",storeTm.year_l,storeTm.month,storeTm.day,storeTm.hour,storeTm.min,storeTm.sec);
            if(DT_DateTimeBCD2Time(&storeTm,&longTm)==FALSE)//应答的时间不合法
            {
                //不存储
                memset(&pAmrThreadArg->pMtrAmrInfo[MsIndex].storeTm, 0x0 ,sizeof(DateTimeBcd_t));
                pAmrThreadArg->pMtrAmrInfo[MsIndex].EndNum=0;
                return 0;
            }
            TASK_FMT_TRACE(Pipe, REC_LOG,"time_t 上报的时标 %ld StartTime %ld endTime %ld \n",longTm,StartTime,endTime);
            if(Tmp_AmrLink->AcqInfo.Acqtype == ACQTYP_FRZTM || Tmp_AmrLink->AcqInfo.Acqtype == ACQTYP_TI )
            {
                //小时冻结,只抄上次冻结时间在区间内-前闭后开
                if(Tmp_AmrLink->CsdTask.RoadTask[0].MainOad.nObjID == 0x5003)
                {
                    if(longTm < StartTime)
                    {
                        //冻结时标比起始时间还小，不需要继续搜索了
                        memset(&pAmrThreadArg->pMtrAmrInfo[MsIndex].storeTm, 0x0 ,sizeof(DateTimeBcd_t));
                        pAmrThreadArg->pMtrAmrInfo[MsIndex].EndNum=0;
                        return 0;
                    }
                    if(longTm >= endTime)
                    {
                        //不在采集的时间间隔内，不存储，继续搜索上一条记录
                        memset(&pAmrThreadArg->pMtrAmrInfo[MsIndex].storeTm, 0x0 ,sizeof(DateTimeBcd_t));
                        pAmrThreadArg->pMtrAmrInfo[MsIndex].EndNum=0;
                        return 0;
                    }
                }
                else
                {
                    //if(longTm <= StartTime)
                    //日冻结的时候starttime是上一天的0点 如果时标是上一天的0点也是不对的
                    if(longTm < StartTime || (Tmp_AmrLink->CsdTask.RoadTask[0].MainOad.nObjID == 0x5004 && longTm == StartTime))
                    {
                        //冻结时标比起始时间还小，不需要继续搜索了
                        memset(&pAmrThreadArg->pMtrAmrInfo[MsIndex].storeTm, 0x0 ,sizeof(DateTimeBcd_t));
                        pAmrThreadArg->pMtrAmrInfo[MsIndex].EndNum=0;
                        return 0;
                    }

                    if(longTm > endTime)
                    {
                        //不在采集的时间间隔内，不存储，继续搜索上一条记录
                        //时标不对直接用实时转 不再往前搜索
                        memset(&pAmrThreadArg->pMtrAmrInfo[MsIndex].storeTm, 0x0 ,sizeof(DateTimeBcd_t));
                        pAmrThreadArg->pMtrAmrInfo[MsIndex].EndNum=0;
                        return 0;
                    }
                }
            }
            pAmrThreadArg->pMtrAmrInfo[MsIndex].storeTm = storeTm;//保存存储时标
            if(Tmp_AmrLink->StoreTimeType!=STRTMTYP_FRZTIME)
            {
                pAmrThreadArg->pMtrAmrInfo[MsIndex].EndNum=pAmrThreadArg->pMtrAmrInfo[MsIndex].CurNum;
            }
        }
    }
    
    if(Tmp_AmrLink->StoreTimeType == STRTMTYP_FRZTIME)
    {
        storeTm =pAmrThreadArg->pMtrAmrInfo[MsIndex].storeTm;
    }
    else
    {
        uint8 tmpbuff[7];
        memcpy_r(tmpbuff,(uint8*)RecvRunTask->ThisTurnBeginTime,7);
        DT_DateTimeBCD2Time((DateTimeBcd_t*)tmpbuff,&longTm);
        //对于使用开始时间作为存储时间的方式，要减去启动延时
        ti.unit = Tmp_AmrLink->DelayTimeUnit;
        ti.value = Tmp_AmrLink->DelayTime;
        DT_TimeSubInv(&longTm,ti);
        storeTm = taskmng_acq_store_time_get(Tmp_AmrLink->StoreTimeType,longTm);
        pAmrThreadArg->pMtrAmrInfo[MsIndex].storeTm = storeTm;
    }

    TASK_FMT_TRACE(Pipe, REC_LOG,"storeTm 20%02x/%02x/%02x %02x:%02x:%02x \n",storeTm.year_l,storeTm.month,storeTm.day,storeTm.hour,storeTm.min,storeTm.sec);

    taskmng_645_meter_data_save(Pipe,pBuf, rcvLen, MsIndex, Tmp_AmrLink, RecvRunTask,&storeTm);

    return 0;
}
/*******************************************************************************
* 函数名称: taskmng_645_meter_data_save_frz
* 函数功能: 处理97、07表的时标，供按冻结时标采集方式抄表使用
* 输入参数: pBuf          645回码数据
*           rcvLen        645回码长度
*           MtrIndex      电表索引
*           Tmp_AmrLink   方案任务节点
* 输出参数: 无
* 返 回 值: 无
*******************************************************************************/
uint16 taskmng_645_meter_data_save_frz_new(uint8 Pipe,uint16 mapindex,uint8 *pBuf, uint16 rcvLen, int MsIndex, SCHEME_TASK_RUN_INFO* Tmp_AmrLink,CTS_RUN_TASK_T *RecvRunTask)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[Pipe];
    DateTimeBcd_t       storeTm;
    time_t              longTm;
    time_t              StartTime,endTime;
    DateTimeBcd_t       tmpTime;
    OOP_TI_T                ti;
    uint8               CSDIndex = pAmrThreadArg->pMtrAmrInfo[MsIndex].tmpcsdindex;
    
    if(Tmp_AmrLink->CsdTask.RoadTask[0].MainOad.nObjID== 0x5003 && taskmng_task_period_get(Tmp_AmrLink->TaskId,Tmp_AmrLink->logicid) < 86400L)
    {
        taskmng_645_meter_data_save_new(Pipe,mapindex,pBuf, rcvLen,MsIndex, Tmp_AmrLink, RecvRunTask,NULL);
        return 0;
    }

    if(Tmp_AmrLink->CsdTask.RoadTask[0].MainOad.nObjID == 0x5002)
    {
        
        uint16 Pn = pAmrThreadArg->pMtrAmrInfo[MsIndex].Pn;
#ifdef AREA_SHANDONG
        OOP_OAD_U suboad = Tmp_AmrLink->CsdTask.RoadTask[0].OadListInfo[CSDIndex];
        if(taskmng_energy_oad_check(suboad) == FALSE)
        {
            taskmng_645_meter_data_save_new(Pipe,mapindex,pBuf, rcvLen, MsIndex, Tmp_AmrLink, RecvRunTask,NULL);
            return 0;
        }
#endif  
        if(taskmng_mtr_conntype_get(Pn,pAmrThreadArg->pMtrAmrInfo[MsIndex].logicid)==1)
        {
                        //判断flag
            if(pAmrThreadArg->pMtrAmrInfo[MsIndex].flag645 == 0)
            {
                taskmng_645_meter_data_save_curve_new(Pipe,mapindex,pBuf, rcvLen, MsIndex, Tmp_AmrLink, RecvRunTask);
            }else
            {
                if(pAmrThreadArg->pMtrAmrInfo[MsIndex].flag645 == 1 ||taskmng_task_period_check(Tmp_AmrLink)==TRUE)
                {
                    taskmng_645_meter_data_save_new(Pipe,mapindex,pBuf, rcvLen, MsIndex, Tmp_AmrLink, RecvRunTask,NULL);
                }else
                {
                    taskmng_645_meter_data_save_profile(Pipe,pBuf, rcvLen, MsIndex, Tmp_AmrLink,RecvRunTask);
                }
            }

        }else
        {
            if(pAmrThreadArg->pMtrAmrInfo[MsIndex].flag645 == 0)
            {
                taskmng_645_meter_data_save_profile(Pipe,pBuf, rcvLen, MsIndex, Tmp_AmrLink,RecvRunTask);
            }else
            {
                taskmng_645_meter_data_save_new(Pipe,mapindex,pBuf, rcvLen, MsIndex, Tmp_AmrLink, RecvRunTask,NULL);
            }
        }
        return 0;
    }

    if(Tmp_AmrLink->CsdTask.RoadTask[0].MainOad.nObjID == 0x5004&&pAmrThreadArg->pMtrAmrInfo[MsIndex].EndNum==0xff)
    {
        //TASK_FMT_TRACE(Pipe, REC_LOG,"日冻结EndNum 0xff\n");
        taskmng_645_meter_data_save_new(Pipe,mapindex,pBuf, rcvLen, MsIndex, Tmp_AmrLink, RecvRunTask,NULL);
        return 0;
    }
    
    
    memcpy_r((uint8*)&tmpTime,(uint8*)RecvRunTask->LastTurnBeginTime,7);
    //TASK_FMT_TRACE(Pipe, REC_LOG,"上轮启动时间:%02x%02x-%02x-%02x %02x:%02x:%02x\n", tmpTime.year_h, tmpTime.year_l,
           //tmpTime.month, tmpTime.day, tmpTime.hour, tmpTime.min, tmpTime.sec);
    DT_DateTimeBCD2Time(&tmpTime,&StartTime);
    memcpy_r((uint8*)&tmpTime,(uint8*)RecvRunTask->ThisTurnBeginTime,7);
    //TASK_FMT_TRACE(Pipe, REC_LOG,"本轮启动时间:%02x%02x-%02x-%02x %02x:%02x:%02x\n", tmpTime.year_h, tmpTime.year_l,
         //  tmpTime.month, tmpTime.day, tmpTime.hour, tmpTime.min, tmpTime.sec);
    DT_DateTimeBCD2Time(&tmpTime,&endTime);
    if(Tmp_AmrLink->AcqInfo.Acqtype == ACQTYP_FRZTM || Tmp_AmrLink->AcqInfo.Acqtype == ACQTYP_TI
            || Tmp_AmrLink->StoreTimeType == STRTMTYP_FRZTIME )
    {
        //按照冻结时标采集，需要判断数据是否在时间间隔范围内
        if(CSDIndex == 0)
        {
            if(Tmp_AmrLink->CsdTask.RoadTask[0].OadListInfo[CSDIndex].nObjID != 0x2021)//第一列必须是冻结时标
            {
                pAmrThreadArg->pMtrAmrInfo[MsIndex].EndNum=0;
                return 0;
            }

            if(Tmp_AmrLink->CsdTask.RoadTask[0].MainOad.nObjID == 0x5006 || Tmp_AmrLink->CsdTask.RoadTask[0].MainOad.nObjID == 0x5005)
            {
                memcpy_r(&storeTm.year_l,pBuf,4);
                storeTm.min = 0;
            }
            else
                memcpy_r(&storeTm.year_l,pBuf,5);

            storeTm.year_h = 0x20;
            storeTm.sec= 0x00;
            storeTm.min= 0x00;
            if(Tmp_AmrLink->CsdTask.RoadTask[0].MainOad.nObjID!= 0x5003)
                storeTm.hour= 0x00;
            // commfun_printstring((uint8*)&storeTm,7);//打印采集到的存储时间
            TASK_FMT_TRACE(Pipe, REC_LOG,"采集到的冻结时标 %02x年 %02x月%02x日 %02x %02x %02x \n",storeTm.year_l,storeTm.month,storeTm.day,storeTm.hour,storeTm.min,storeTm.sec);
            if(DT_DateTimeBCD2Time(&storeTm,&longTm)==FALSE)//应答的时间不合法
            {
                //不存储
                memset(&pAmrThreadArg->pMtrAmrInfo[MsIndex].storeTm, 0x0 ,sizeof(DateTimeBcd_t));
                pAmrThreadArg->pMtrAmrInfo[MsIndex].EndNum=0;
                return 0;
            }
            TASK_FMT_TRACE(Pipe, REC_LOG,"time_t 上报的时标 %ld StartTime %ld endTime %ld \n",longTm,StartTime,endTime);
            if(Tmp_AmrLink->AcqInfo.Acqtype == ACQTYP_FRZTM || Tmp_AmrLink->AcqInfo.Acqtype == ACQTYP_TI )
            {
                //小时冻结,只抄上次冻结时间在区间内-前闭后开
                if(Tmp_AmrLink->CsdTask.RoadTask[0].MainOad.nObjID == 0x5003)
                {
                    if(longTm < StartTime)
                    {
                        //冻结时标比起始时间还小，不需要继续搜索了
                        memset(&pAmrThreadArg->pMtrAmrInfo[MsIndex].storeTm, 0x0 ,sizeof(DateTimeBcd_t));
                        pAmrThreadArg->pMtrAmrInfo[MsIndex].EndNum=0;
                        return 0;
                    }
                    if(longTm >= endTime)
                    {
                        //不在采集的时间间隔内，不存储，继续搜索上一条记录
                        memset(&pAmrThreadArg->pMtrAmrInfo[MsIndex].storeTm, 0x0 ,sizeof(DateTimeBcd_t));
                        pAmrThreadArg->pMtrAmrInfo[MsIndex].EndNum=0;
                        return 0;
                    }
                }
                else
                {
                    //if(longTm <= StartTime)
                    //日冻结的时候starttime是上一天的0点 如果时标是上一天的0点也是不对的
                    if(patch_policy_flg[Pipe] == 2 && Tmp_AmrLink->CsdTask.RoadTask[0].MainOad.nObjID == 0x5004)
                    {
                        TASK_FMT_TRACE ( Pipe,REC_LOG, "不判断应答时间是否合法：\n");
                        longTm = StartTime;
                    }
                    else
                    {
                        if(longTm < StartTime || (Tmp_AmrLink->CsdTask.RoadTask[0].MainOad.nObjID == 0x5004 && longTm == StartTime))
                        {
                            //冻结时标比起始时间还小，不需要继续搜索了
                            memset(&pAmrThreadArg->pMtrAmrInfo[MsIndex].storeTm, 0x0 ,sizeof(DateTimeBcd_t));
                            pAmrThreadArg->pMtrAmrInfo[MsIndex].EndNum=0;
                            return 0;
                        }
                        if(longTm > endTime)
                        {
                            //不在采集的时间间隔内，不存储，继续搜索上一条记录
                            //时标不对直接用实时转 不再往前搜索
                            memset(&pAmrThreadArg->pMtrAmrInfo[MsIndex].storeTm, 0x0 ,sizeof(DateTimeBcd_t));
                            pAmrThreadArg->pMtrAmrInfo[MsIndex].EndNum=0;
                            return 0;
                        }
                    }
                }
            }
            pAmrThreadArg->pMtrAmrInfo[MsIndex].storeTm = storeTm;//保存存储时标
            if(Tmp_AmrLink->StoreTimeType!=STRTMTYP_FRZTIME)
            {
                pAmrThreadArg->pMtrAmrInfo[MsIndex].EndNum=pAmrThreadArg->pMtrAmrInfo[MsIndex].CurNum;
            }
        }
    }
    
    if(Tmp_AmrLink->StoreTimeType == STRTMTYP_FRZTIME)
    {
        storeTm =pAmrThreadArg->pMtrAmrInfo[MsIndex].storeTm;
    }
    else
    {
        uint8 tmpbuff[7];
        memcpy_r(tmpbuff,(uint8*)RecvRunTask->ThisTurnBeginTime,7);
        DT_DateTimeBCD2Time((DateTimeBcd_t*)tmpbuff,&longTm);
        //对于使用开始时间作为存储时间的方式，要减去启动延时
        ti.unit = Tmp_AmrLink->DelayTimeUnit;
        ti.value = Tmp_AmrLink->DelayTime;
        DT_TimeSubInv(&longTm,ti);
        storeTm = taskmng_acq_store_time_get(Tmp_AmrLink->StoreTimeType,longTm);
        pAmrThreadArg->pMtrAmrInfo[MsIndex].storeTm = storeTm;
    }

    TASK_FMT_TRACE(Pipe, REC_LOG,"storeTm 20%02x/%02x/%02x %02x:%02x:%02x \n",storeTm.year_l,storeTm.month,storeTm.day,storeTm.hour,storeTm.min,storeTm.sec);

    taskmng_645_meter_data_save_new(Pipe,mapindex,pBuf, rcvLen, MsIndex, Tmp_AmrLink, RecvRunTask,&storeTm);

    return 0;
}

/****************************************************************************
*模块编号：
*名称：
*功能：根据OAD计算 OAD的数据在数据块中的偏移，供07表冻结，负荷曲线，事件记录使用
*      回码格式必须与列表格式一致，若不一致需将回码修正为列表格式再计算
*输入参数：pipe 端口号
        Tmp_AmrLink 任务参数
        MsIndex 电表索引
*输出参数：DataLen 数据在数据块中的偏移
*处理：
*返回：无
****************************************************************************/
uint16 CacloffsByOad(uint8 Pipe,SCHEME_TASK_RUN_INFO* Tmp_AmrLink,int MsIndex,uint8 *DataLen)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[Pipe];
    uint16                      offSet=0;
    uint8                       CSDIndex;
    OOP_OAD_U                   OAD;
    uint8                       OadTag;
    uint8                       RoadIndex;
    const MapOopTo645_NEW      *pMapOopTo645=NULL;
    const EventMapOopTo645_t  *pEventMapList=NULL;
    //OadInfoMap OadMap ;
    CSDIndex = pAmrThreadArg->pMtrAmrInfo[MsIndex].CSDIndex;
    RoadIndex = pAmrThreadArg->pMtrAmrInfo[MsIndex].ROADIndex;
    if(Tmp_AmrLink->SchemeType == EVENTPLAN)//如果方案类型为事件
    {
        //根据主OAD获取对应要搜索的列表
        pEventMapList = taskmng_event_map_get(Tmp_AmrLink->CsdTask.RoadTask[RoadIndex].MainOad);
        if(pEventMapList == NULL)
            return 0xFFFF;

        pMapOopTo645 = pEventMapList->EventMap;
        if(pMapOopTo645 == NULL)
            return 0xFFFF;

        OadTag = Tmp_AmrLink->CsdTask.RoadTask[RoadIndex].OadListInfo[CSDIndex].attPro;
        OAD = Tmp_AmrLink->CsdTask.RoadTask[RoadIndex].OadListInfo[CSDIndex];
        if(Tmp_AmrLink->CsdTask.RoadTask[RoadIndex].MainOad.nObjID== 0x300D
        &&  Tmp_AmrLink->CsdTask.RoadTask[RoadIndex].OadListInfo[CSDIndex].value ==0x20010200)
        {
            OAD.value = 0x20010201;
        }


    }
    else
    {
        if(Tmp_AmrLink->CsdTask.RoadTask[0].MainOad.nObjID == 0x5002)  //分钟曲线
            pMapOopTo645 = MapOopTo645_Min;
        else
            return 0xFFFF;

        OAD = Tmp_AmrLink->CsdTask.RoadTask[0].OadListInfo[CSDIndex];
        OadTag = Tmp_AmrLink->CsdTask.RoadTask[0].OadListInfo[CSDIndex].attPro;
    }
    TASK_FMT_TRACE(Pipe, REC_LOG,"CacloffsByOad MainOad 0x%08x OAD 0x%08x OadTag %d\n",Tmp_AmrLink->CsdTask.RoadTask[RoadIndex].MainOad,OAD,OadTag);
    
    pAmrThreadArg->pMtrAmrInfo[MsIndex].MapIndex = 0;
    
    while(pMapOopTo645->Oad.value!=0xFFFFFFFF)
    {
        if(OAD.value == pMapOopTo645->Oad.value && OadTag == pMapOopTo645->Oad.attPro)
        {
            *DataLen = pMapOopTo645->TotalDataLen;
            break;
        }
        else
        {
            if(taskmng_curve_DI_adapt(Pipe,Tmp_AmrLink, MsIndex,pMapOopTo645->Oad, pMapOopTo645) > 0)
            {
                *DataLen = pMapOopTo645->TotalDataLen;
                break;
            }
            if((pMapOopTo645->Oad.nIndex !=0)||(FALSE == Check_Data_Struct(pMapOopTo645->CovertSruct)))
                offSet += pMapOopTo645->DataLen;

        }
        pMapOopTo645++;
        pAmrThreadArg->pMtrAmrInfo[MsIndex].MapIndex ++;  //更新列表索引，供存储使用
    }
#ifdef AREA_HUNAN
        if(Tmp_AmrLink->CsdTask.RoadTask[RoadIndex].MainOad.nObjID== 0x3505
        && ( Tmp_AmrLink->CsdTask.RoadTask[RoadIndex].OadListInfo[CSDIndex].value ==0x33420207||
            Tmp_AmrLink->CsdTask.RoadTask[RoadIndex].OadListInfo[CSDIndex].value ==0x33420208))
        {
            offSet = 6;
        }
#endif
    TASK_FMT_TRACE(Pipe, REC_LOG,"offSet %d DataLen %d \n",offSet,*DataLen);
    if(pMapOopTo645->Oad.value==0xFFFFFFFF)
        return 0xFFFF;
    return offSet;
}

/*******************************************************************************
* 函数名称: taskmng_curve_DI_adapt
* 函数功能: 抄读07表负荷曲线时，部分OAD若配置块抄，转换为单抄
*           会把每一条都找到并返回
* 输入参数: Tmp_AmrLink         方案任务节点
*           MsIndex             方案内电表索引
*           CurOAD              当前对照表中的OAD
* 输出参数: 无
* 返 回 值: <0                  没有需要转换的OAD
            >0                  有需要转换的OAD，允许抄读
*******************************************************************************/
int taskmng_curve_DI_adapt(uint8 Pipe,SCHEME_TASK_RUN_INFO* Tmp_AmrLink, int MsIndex, OOP_OAD_U OAD, const MapOopTo645_NEW* pMap)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[Pipe];
    uint16                   CSDIndex;

    CSDIndex = pAmrThreadArg->pMtrAmrInfo[MsIndex].CSDIndex;

    if(Tmp_AmrLink->CsdTask.RoadTask[0].MainOad.nObjID != 0x5002)
        return -1;
    if(pMap == MapOopTo645_MinOne)
    {
        if(Tmp_AmrLink->CsdTask.RoadTask[0].OadListInfo[CSDIndex].value == 0x00100200
            || Tmp_AmrLink->CsdTask.RoadTask[0].OadListInfo[CSDIndex].value == 0x00200200
            || Tmp_AmrLink->CsdTask.RoadTask[0].OadListInfo[CSDIndex].value == 0x00300200
            || Tmp_AmrLink->CsdTask.RoadTask[0].OadListInfo[CSDIndex].value == 0x00400200
            || Tmp_AmrLink->CsdTask.RoadTask[0].OadListInfo[CSDIndex].value == 0x00500200
            || Tmp_AmrLink->CsdTask.RoadTask[0].OadListInfo[CSDIndex].value == 0x00600200
            || Tmp_AmrLink->CsdTask.RoadTask[0].OadListInfo[CSDIndex].value == 0x00700200
            || Tmp_AmrLink->CsdTask.RoadTask[0].OadListInfo[CSDIndex].value == 0x00800200
            || Tmp_AmrLink->CsdTask.RoadTask[0].OadListInfo[CSDIndex].value == 0x20000200
            || Tmp_AmrLink->CsdTask.RoadTask[0].OadListInfo[CSDIndex].value == 0x20010200
            || Tmp_AmrLink->CsdTask.RoadTask[0].OadListInfo[CSDIndex].value == 0x20040200
            || Tmp_AmrLink->CsdTask.RoadTask[0].OadListInfo[CSDIndex].value == 0x200A0200)
        {
            if(OAD.value == (Tmp_AmrLink->CsdTask.RoadTask[0].OadListInfo[CSDIndex].value+1))  //是否与总OAD相同
            {
                return 1;
            }
        }
    }
    else if(pMap == MapOopTo645_Min)
    {
       if(Tmp_AmrLink->CsdTask.RoadTask[0].OadListInfo[CSDIndex].value == 0x00100200
            || Tmp_AmrLink->CsdTask.RoadTask[0].OadListInfo[CSDIndex].value == 0x00200200
            || Tmp_AmrLink->CsdTask.RoadTask[0].OadListInfo[CSDIndex].value == 0x00300200
            || Tmp_AmrLink->CsdTask.RoadTask[0].OadListInfo[CSDIndex].value == 0x00400200
            || Tmp_AmrLink->CsdTask.RoadTask[0].OadListInfo[CSDIndex].value == 0x00500200
            || Tmp_AmrLink->CsdTask.RoadTask[0].OadListInfo[CSDIndex].value == 0x00600200
            || Tmp_AmrLink->CsdTask.RoadTask[0].OadListInfo[CSDIndex].value == 0x00700200
            || Tmp_AmrLink->CsdTask.RoadTask[0].OadListInfo[CSDIndex].value == 0x00800200
            || Tmp_AmrLink->CsdTask.RoadTask[0].OadListInfo[CSDIndex].value == 0x20000200
            || Tmp_AmrLink->CsdTask.RoadTask[0].OadListInfo[CSDIndex].value == 0x20010200
            || Tmp_AmrLink->CsdTask.RoadTask[0].OadListInfo[CSDIndex].value == 0x20040200
            || Tmp_AmrLink->CsdTask.RoadTask[0].OadListInfo[CSDIndex].value == 0x200A0200)
        {
            if(OAD.value == (Tmp_AmrLink->CsdTask.RoadTask[0].OadListInfo[CSDIndex].value+1))  //是否与总OAD相同
            {
                return 1;
            }
        }
    }
    return -1;
}

int taskmng_curve_DI_adapt_new(SCHEME_TASK_RUN_INFO* Tmp_AmrLink,OOP_OAD_U suboad, OOP_OAD_U OAD, const MapOopTo645_NEW* pMap)
{


    if(Tmp_AmrLink->CsdTask.RoadTask[0].MainOad.nObjID != 0x5002)
        return -1;
    if(pMap == MapOopTo645_MinOne)
    {
        if(suboad.value == 0x00100200
            || suboad.value == 0x00200200
            || suboad.value == 0x00300200
            || suboad.value == 0x00400200
            || suboad.value == 0x00500200
            || suboad.value == 0x00600200
            || suboad.value == 0x00700200
            || suboad.value == 0x00800200
            || suboad.value == 0x20000200
            || suboad.value == 0x20010200
            || suboad.value == 0x20040200
            || suboad.value == 0x200A0200)
        {
            if(OAD.value == (suboad.value+1))  //是否与总OAD相同
            {
                return 1;
            }
        }
    }
    else if(pMap == MapOopTo645_Min)
    {
       if(suboad.value == 0x00100200
            || suboad.value == 0x00200200
            || suboad.value == 0x00300200
            || suboad.value == 0x00400200
            || suboad.value == 0x00500200
            || suboad.value == 0x00600200
            || suboad.value == 0x00700200
            || suboad.value == 0x00800200
            || suboad.value == 0x20000200
            || suboad.value == 0x20010200
            || suboad.value == 0x20040200
            || suboad.value == 0x200A0200)
        {
            if(OAD.value == (suboad.value+1))  //是否与总OAD相同
            {
                return 1;
            }
        }
    }
    return -1;
}



/*******************************************************************************
* 函数名称: taskmng_645_meter_data_save_curve
* 函数功能: 存储07表(14年单相0610xxxx)负荷曲线
* 输入参数:     pipe        端口号
            pBuf          645回码数据
*           rcvLen        645回码长度
*           MtrIndex      电表索引
*           Tmp_AmrLink   方案任务节点
            RecvRunTask     任务运行时参数
* 输出参数: 无
* 返 回 值: 无
*******************************************************************************/
void taskmng_645_meter_data_save_curve(uint8 Pipe,uint8 *pBuf, uint16 rcvLen, int MsIndex,SCHEME_TASK_RUN_INFO* Tmp_AmrLink,CTS_RUN_TASK_T *RecvRunTask)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[Pipe];
    uint8         offset=0;
    DateTimeBcd_t storeTm;
    uint8         PointNum;
    uint16        DataLen;
    time_t        TempTimeSec;
    OOP_TI_T          DelayTi;
    int           i;
    uint16 MapIndex = pAmrThreadArg->pMtrAmrInfo[MsIndex].MapIndex;
    time_t          startTime = 0;

    //如果没数据可能会回空 回码只有06100101这类四个字节 到本函数长度就为0
    //最少5个字节时间加2个字节数据
    if (rcvLen < 5 + 2)
    {
#ifdef AREA_SHANDONG
        if (TaskMgrGetTimeOffset(Pipe, taskmng_task_period_get(Tmp_AmrLink->TaskId, Tmp_AmrLink->logicid), &pAmrThreadArg->pMtrAmrInfo[MsIndex]) == 0)
        {
            return;
        }
#endif
        return ;
    }
    memcpy_r(&storeTm.year_l,pBuf,5);
    storeTm.year_h = 0x20;
    storeTm.sec = 0x00;
    if(DT_DateTimeBCD2Time(&storeTm, &TempTimeSec) == FALSE)
        return;
    //判断时间是否正确 如果不正确 则切换抄读方式
    //TASK_FMT_TRACE(Pipe, REC_LOG,"--------------TempTimeSec=%ld,StartTime=%ld -------------\n",TempTimeSec, pAmrThreadArg->pMtrAmrInfo[MsIndex].StartTime);
#ifdef AREA_SHANDONG
    TASK_FMT_TRACE(Pipe, REC_LOG,"timeOffset=%ld\n", pAmrThreadArg->pMtrAmrInfo[MsIndex].timeOffset);
    startTime = pAmrThreadArg->pMtrAmrInfo[MsIndex].StartTime - pAmrThreadArg->pMtrAmrInfo[MsIndex].timeOffset;
#else
    startTime = pAmrThreadArg->pMtrAmrInfo[MsIndex].StartTime;
#endif

    if (TempTimeSec < startTime)
    {
#ifdef AREA_SHANDONG
        if (TaskMgrGetTimeOffset(Pipe, taskmng_task_period_get(Tmp_AmrLink->TaskId, Tmp_AmrLink->logicid), &pAmrThreadArg->pMtrAmrInfo[MsIndex]) == 0)
        {
            return;
        }
#endif
        TASK_FMT_TRACE(Pipe, REC_LOG, " Recvtime %d starttime %d  \n",TempTimeSec,pAmrThreadArg->pMtrAmrInfo[MsIndex].StartTime);
#if CURRENT_TO_CURVE_ENABLE
        pAmrThreadArg->pMtrAmrInfo[MsIndex].flag645 = 1;
#endif
        return;
    }

    offset += 5;

    DataLen =MapOopTo645_MinOne[MapIndex].TotalDataLen;
    PointNum = (rcvLen-5)/DataLen;
    for(i=0;i<PointNum;i++)
    {
         //计算每个时间点的存储时标
        DelayTi.value = 15;
        DelayTi.unit = 1;  //07表负荷曲线间隔15分
        if(i != 0)
            DT_TimeAddInv(&TempTimeSec, DelayTi);

        //无效数据不存储
        if(pBuf[offset] == 0xff && pBuf[offset+1] == 0xff)
        {
            offset += DataLen;
            continue;
        }
        if(Tmp_AmrLink->AcqInfo.TI.unit == TASK_CYCLE_FLAG_HOUR
            || (Tmp_AmrLink->AcqInfo.TI.unit == TASK_CYCLE_FLAG_MIN && Tmp_AmrLink->AcqInfo.TI.value == 60))
        {
            if(i%4 != 0)
            {
                offset += DataLen;
                continue;
            }
        }
        DT_Time2DateTimeBCD(TempTimeSec, &storeTm);
        taskmng_645_meter_data_save(Pipe,&pBuf[offset], DataLen, MsIndex, Tmp_AmrLink, RecvRunTask,&storeTm);
        offset += DataLen;
    }
    return ;
}

/*******************************************************************************
* 函数名称: taskmng_645_meter_data_save_curve
* 函数功能: 存储07表(14年单相0610xxxx)负荷曲线
* 输入参数:     pipe        端口号
            pBuf          645回码数据
*           rcvLen        645回码长度
*           MtrIndex      电表索引
*           Tmp_AmrLink   方案任务节点
            RecvRunTask     任务运行时参数
* 输出参数: 无
* 返 回 值: 无
*******************************************************************************/
void taskmng_645_meter_data_save_curve_new(uint8 Pipe,uint16 MapIndex,uint8 *pBuf, uint16 rcvLen, int MsIndex,SCHEME_TASK_RUN_INFO* Tmp_AmrLink,CTS_RUN_TASK_T *RecvRunTask)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[Pipe];
    uint8         offset=0;
    DateTimeBcd_t storeTm;
    uint8         PointNum;
    uint16        DataLen;
    time_t        TempTimeSec;
    OOP_TI_T          DelayTi;
    int           i;
    //uint16 MapIndex = pAmrThreadArg->pMtrAmrInfo[MsIndex].MapIndex;
    time_t          startTime = 0;

    //如果没数据可能会回空 回码只有06100101这类四个字节 到本函数长度就为0
    //最少5个字节时间加2个字节数据
    if(rcvLen<5+2)
    {
#ifdef AREA_SHANDONG
        if (TaskMgrGetTimeOffset(Pipe, taskmng_task_period_get(Tmp_AmrLink->TaskId, Tmp_AmrLink->logicid), &pAmrThreadArg->pMtrAmrInfo[MsIndex]) == 0)
        {
            return;
        }
#endif
        return ;
    }
    memcpy_r(&storeTm.year_l,pBuf,5);
    storeTm.year_h = 0x20;
    storeTm.sec = 0x00;
    if(DT_DateTimeBCD2Time(&storeTm, &TempTimeSec) == FALSE)
    {
        TASK_FMT_TRACE(Pipe, REC_LOG,"回码时间格式错误\n");
        pAmrThreadArg->pMtrAmrInfo[MsIndex].flag645 = 1;
        return;
    }
    //判断时间是否正确 如果不正确 则切换抄读方式
    //TASK_FMT_TRACE(Pipe, REC_LOG,"--------------TempTimeSec=%ld,StartTime=%ld -------------\n",TempTimeSec, pAmrThreadArg->pMtrAmrInfo[MsIndex].StartTime);
#ifdef AREA_SHANDONG
    TASK_FMT_TRACE(Pipe, REC_LOG,"timeOffset=%ld\n", pAmrThreadArg->pMtrAmrInfo[MsIndex].timeOffset);
    startTime = pAmrThreadArg->pMtrAmrInfo[MsIndex].StartTime - pAmrThreadArg->pMtrAmrInfo[MsIndex].timeOffset;
#else
    startTime = pAmrThreadArg->pMtrAmrInfo[MsIndex].StartTime;
#endif

    if (TempTimeSec < startTime)
    {
#ifdef AREA_SHANDONG
        if (TaskMgrGetTimeOffset(Pipe, taskmng_task_period_get(Tmp_AmrLink->TaskId, Tmp_AmrLink->logicid), &pAmrThreadArg->pMtrAmrInfo[MsIndex]) == 0)
        {
            return;
        }
#endif
        TASK_FMT_TRACE(Pipe, REC_LOG, " Recvtime %d starttime %d  \n",TempTimeSec,pAmrThreadArg->pMtrAmrInfo[MsIndex].StartTime);
        pAmrThreadArg->pMtrAmrInfo[MsIndex].flag645 = 1;
        return;
    }

    offset += 5;

    DataLen =MapOopTo645_MinOne[MapIndex].TotalDataLen;
    PointNum = (rcvLen-5)/DataLen;
    for(i=0;i<PointNum;i++)
    {
         //计算每个时间点的存储时标
        DelayTi.value = 15;
        DelayTi.unit = 1;  //07表负荷曲线间隔15分
        if(i != 0)
            DT_TimeAddInv(&TempTimeSec, DelayTi);

        //无效数据不存储
        if(pBuf[offset] == 0xff && pBuf[offset+1] == 0xff)
        {
            offset += DataLen;
            continue;
        }
        if(Tmp_AmrLink->AcqInfo.TI.unit == TASK_CYCLE_FLAG_HOUR
            || (Tmp_AmrLink->AcqInfo.TI.unit == TASK_CYCLE_FLAG_MIN && Tmp_AmrLink->AcqInfo.TI.value == 60))
        {
            if(i%4 != 0)
            {
                offset += DataLen;
                continue;
            }
        }
        DT_Time2DateTimeBCD(TempTimeSec, &storeTm);
        taskmng_645_meter_data_save_new(Pipe,MapIndex,&pBuf[offset], DataLen, MsIndex, Tmp_AmrLink, RecvRunTask,&storeTm);
        offset += DataLen;
    }
    return ;
}



/*******************************************************************************
* 函数名称: taskmng_645_meter_data_save_profile
* 函数功能: 存储07表负荷曲线
* 输入参数:     pipe 端口号
            pBuf          645回码数据
*           rcvLen        645回码长度
*           MtrIndex      电表索引
*           Tmp_AmrLink   方案任务节点
            RecvRunTask     任务运行时参数
* 输出参数: 无
* 返 回 值: 无
*******************************************************************************/
void taskmng_645_meter_data_save_profile(uint8 Pipe,uint8 *pBuf, uint16 rcvLen, int MsIndex,SCHEME_TASK_RUN_INFO* Tmp_AmrLink,CTS_RUN_TASK_T *RecvRunTask)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[Pipe];
    uint8 offset=0;
    uint8 profileBuff[5+17+24+8+16+16+6];
    DateTimeBcd_t storeTm;
    uint8 i,subOadNum;
    uint8 DataLen;
    uint16 bias = 0xFFFF;
    time_t TempTimeSec;
    time_t startTime = 0;

    memset(profileBuff,0x00,sizeof(profileBuff));
    if(pBuf[0]!=0xA0 || pBuf[1]!=0xA0)
        return;
    offset += 3;
    memcpy(profileBuff,pBuf+offset,5);
    
    offset += 5;

    if(pBuf[offset]!=0xAA)
    {
        memcpy(profileBuff+5,pBuf+offset,17);
        offset += 17;
    }
    //功率
    if(pBuf[offset++]!=0xAA)
    {
        printf("功率偏移错误!\n");
        return;
    }
    if(pBuf[offset]!=0xAA)
    {
        memcpy(profileBuff+22,pBuf+offset,24);
        offset += 24;
    }
    //功率因数
    if(pBuf[offset++]!=0xAA)
    {
        printf("功率因数偏移错误!\n");
        return;
    }
    if(pBuf[offset]!=0xAA)
    {
        memcpy(profileBuff+46,pBuf+offset,8);
        offset += 8;
    }
    //有功电能
    if(pBuf[offset++]!=0xAA)
    {
        printf("有功电能偏移错误!\n");
        return;
    }
    if(pBuf[offset]!=0xAA)
    {
        memcpy(profileBuff+54,pBuf+offset,16);
        //commfun_printstring(profileBuff+54,16);
        offset += 16;
    }
    //无功电能
    if(pBuf[offset++]!=0xAA)
    {
        printf("无功电能偏移错误!\n");
        return;
    }
    if(pBuf[offset]!=0xAA)
    {
        memcpy(profileBuff+70,pBuf+offset,16);
        offset += 16;
    }
    //当前需量
    if(pBuf[offset++]!=0xAA)
    {
        printf("当前需量偏移错误!\n");
        return;
    }
    if(pBuf[offset]!=0xAA)
    {
        memcpy(profileBuff+86,pBuf+offset,6);
        offset += 6;
    }
    if(pBuf[offset++]!=0xAA)
    {
        printf("校验和偏移错误!\n");
        return;
    }
    //校验和判断
    offset++;
    //结束符判断
    if(pBuf[offset++]!=0xe5)
    {
        printf("结束符偏移错误!\n");
        return;
    }

    memcpy_r(&storeTm.year_l,profileBuff,5);

    storeTm.year_h = 0x20;
    storeTm.sec = 0x00;
    if(DT_DateTimeBCD2Time(&storeTm, &TempTimeSec) == FALSE)
        return;

    //判断时间是否正确 如果不正确 则切换抄读方式
    //TASK_FMT_TRACE(Pipe, REC_LOG,"--------------TempTimeSec=%ld,StartTime=%ld -------------\n",TempTimeSec, pAmrThreadArg->pMtrAmrInfo[MsIndex].StartTime);
#ifdef AREA_SHANDONG
    TASK_FMT_TRACE(Pipe, REC_LOG,"timeOffset=%ld\n", pAmrThreadArg->pMtrAmrInfo[MsIndex].timeOffset);
    startTime = pAmrThreadArg->pMtrAmrInfo[MsIndex].StartTime - pAmrThreadArg->pMtrAmrInfo[MsIndex].timeOffset;
#else
    startTime = pAmrThreadArg->pMtrAmrInfo[MsIndex].StartTime;
#endif

    if (TempTimeSec != startTime)
    {
#ifdef AREA_SHANDONG
        if (TaskMgrGetTimeOffset(Pipe, taskmng_task_period_get(Tmp_AmrLink->TaskId, Tmp_AmrLink->logicid), &pAmrThreadArg->pMtrAmrInfo[MsIndex]) == 0)
        {
            return;
        }
#endif
        TASK_FMT_TRACE(Pipe, REC_LOG, " Recvtime %d starttime %d  \n",TempTimeSec,pAmrThreadArg->pMtrAmrInfo[MsIndex].StartTime);
#if CURRENT_TO_CURVE_ENABLE
        pAmrThreadArg->pMtrAmrInfo[MsIndex].flag645 = 1;
#endif
        return;
    }

    subOadNum = Tmp_AmrLink->CsdTask.RoadTask[0].SubOadNum;
    if(subOadNum == 0)
        return;
    i=0;
    while(i<subOadNum)
    {
        pAmrThreadArg->pMtrAmrInfo[MsIndex].CSDIndex= i;           //当前CSD索引
        bias = CacloffsByOad(Pipe,Tmp_AmrLink,MsIndex, &DataLen);
        //printf("bias = %d,DataLen = %d\n",bias,DataLen);
        if((bias+DataLen) > 92 && bias!=0xFFFF)
            return;    //防止电表回码长度不够
        if(bias!=0xFFFF)
        {
            taskmng_645_meter_data_save(Pipe,&profileBuff[bias], DataLen,MsIndex, Tmp_AmrLink, RecvRunTask,&storeTm);
        }
        i++;
    }
}


/*******************************************************************************
* 函数名称: taskmng_oop_relay_data_normallist_save
* 函数功能://存储组帧穿透时回码
* 输入参数: pipe   端口号
        DataBuff    回码数据
        DataLen     回码长度

* 输出参数:
* 返 回 值: 0 解帧失败 1 存储成功
*******************************************************************************/
int taskmng_oop_relay_data_normallist_save(uint8 pipe,uint8 *DataBuff,uint16 DataLen)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    uint16 Dataoffset = 0;
    OOP_OAD_U            Oad;
    OOP_OAD_U       MainOad  ;
    uint16          OadLen = 0;
    MainOad.value = 0x00000000;
    uint8 MtrIndex = pAmrThreadArg->RelayTrans.AmrIndex;
    TASK_FMT_TRACE(pipe, RELAY_LOG, " 面向对象存储普通数据 MtrIndex %d IsNeedGetSafeMode %d \n",MtrIndex,pAmrThreadArg->RelayTrans.AmrNormal[MtrIndex].IsNeedGetSafeMode);
    Dataoffset++;//OAD数量
    if(pAmrThreadArg->RelayTrans.AmrNormal[MtrIndex].IsNeedGetSafeMode == FALSE)
    {
        pAmrThreadArg->RelayTrans.OadIndex+=pAmrThreadArg->RelayTrans.OopSendNum;
        TASK_FMT_TRACE(pipe, RELAY_LOG, " OadIndex %d \n",pAmrThreadArg->RelayTrans.OadIndex);
    }
    while(Dataoffset < DataLen)
    {
        if(Dataoffset +5 > DataLen)
            break;
        memcpy_r((uint8*)&Oad.value,&DataBuff[Dataoffset],4);
        Dataoffset += 4;
        TASK_FMT_TRACE(pipe, RELAY_LOG, "回复的oad %08x \n",Oad.value);
        
            
        if(DataBuff[Dataoffset++] != 0x01)//数据访问结果DAR
        {
            Dataoffset++;//DAR
            continue;
        }
        OadLen = OopRelay_SavaData(pipe,MtrIndex,MainOad,Oad,DataBuff+Dataoffset,DataLen-Dataoffset,NULL);
        if(OadLen==0)
            return 0;
        Dataoffset+=OadLen;
    }
    return 1;
}

/*******************************************************************************
* 函数名称: OopRelay_SaveNormalList
* 函数功能://485存储组帧穿透时回码
* 输入参数: pipe   端口号
        DataBuff    回码数据
        DataLen     回码长度

* 输出参数:
* 返 回 值:0 解帧失败 1 存储成功
*******************************************************************************/
int Oop485Relay_Save(uint8 pipe,uint8 *DataBuff,uint16 DataLen)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    uint16 Dataoffset = 0;
    OOP_OAD_U            Oad;
    OOP_OAD_U       MainOad  ;
    uint16          OadLen = 0;
    MainOad.value = 0x00000000;
    uint8 MtrIndex = pAmrThreadArg->RelayTrans.AmrIndex;
	uint8 oadnum = DataBuff[Dataoffset++];
	uint8 i=0;
    TASK_FMT_TRACE(pipe, RELAY_LOG, " 面向对象存储普通数据 MtrIndex %d IsNeedGetSafeMode %d \n",MtrIndex,pAmrThreadArg->RelayTrans.AmrNormal[MtrIndex].IsNeedGetSafeMode);
  
    //if(pAmrThreadArg->RelayTrans.AmrNormal[MtrIndex].IsNeedGetSafeMode == FALSE)
    {
        pAmrThreadArg->RelayTrans.OadIndex=pAmrThreadArg->RelayTrans.OadIndex+pAmrThreadArg->RelayTrans.OopSendNum;
        TASK_FMT_TRACE(pipe, RELAY_LOG, " pAmrThreadArg->RelayTrans.OopSendNum is %d \n",pAmrThreadArg->RelayTrans.OopSendNum);
    }
    while(i < oadnum)
    {
        if(Dataoffset +5 > DataLen)
            break;
		i++;
        memcpy_r((uint8*)&Oad.value,&DataBuff[Dataoffset],4);
        Dataoffset += 4;
        TASK_FMT_TRACE(pipe, RELAY_LOG, "回复的oad %08x \n",Oad.value);
        
            
        if(DataBuff[Dataoffset++] != 0x01)//数据访问结果DAR
        {
            Dataoffset++;//DAR
            continue;
        }
        OadLen = OopRelay_SavaData(pipe,MtrIndex,MainOad,Oad,DataBuff+Dataoffset,DataLen-Dataoffset,NULL);
        if(OadLen==0)
            return 0;
        Dataoffset+=OadLen;
    }
    return 1;
}
/*******************************************************************************
* 函数名称: OopRelay_SavaData
* 函数功能://存储组帧穿透数据
* 输入参数: pipe    端口号
        MtrIndex    电表索引
        MainOad     主oad
        Oad         子oad
        pRcvBuf     回码数据
        RcvLen      回码长度
        StoreTime   存储时标

* 输出参数:
* 返 回 值: 0 解析失败 >0 存储完数据的偏移
*******************************************************************************/
uint16 OopRelay_SavaData(uint8 pipe,uint8 MtrIndex,OOP_OAD_U MainOad,OOP_OAD_U Oad, uint8* pRcvBuf, uint16 RcvLen,DateTimeBcd_t *StoreTime)
{
    AmrThreadArg_T *pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    uint16 offset =0 ;
    uint16 DataLen = 0;
    uint8 DataBuff[MAX_MSG_LEN]={0};
    uint16 Pn;
    
    OadStoreInfo StoreInfo;
    memset(&StoreInfo,0x00,sizeof(OadStoreInfo));
    OadInfoMap pOadInfo;
    memset(&pOadInfo,0x00,sizeof(OadInfoMap));
    BOOL ret;
    uint8 pwrType;


    Pn = taskmng_pn_get_from_tsa(pAmrThreadArg->RelayTrans.AmrNormal[MtrIndex].Mtraddr,LOGIC_ID_YC);

    ret = taskmng_oad_info_get( Oad,&pOadInfo);
    TASK_FMT_TRACE(pipe,RELAY_LOG,"OopRelay_SavaData OAD %08x ret %d \n",Oad.value,ret);
    if(ret == FALSE)
    {
        return 0;
    }
    if(pRcvBuf[0]==0) //如果为datanull 则无需解析
    {
        TASK_FMT_TRACE(pipe,RELAY_LOG,"data为空 不解析\n");
        return 1;
    }
    pwrType=taskmng_mtr_conntype_get(Pn,LOGIC_ID_YC);
    Convert_Oop_Data_To_Struct(pRcvBuf,&offset,RcvLen,DataBuff,&pOadInfo,&DataLen,pwrType);
    TASK_FMT_TRACE(pipe,RELAY_LOG,"Convert_Oop_Data_To_Struct  DataLen %d 转换后数据 \n",DataLen);

    TASK_BUF_TRACE(pipe,RELAY_LOG,DataBuff, DataLen);
    //组帧穿透数据按照逻辑设备号100来存 取出后会删除
    StoreInfo.recordOAD.logicId=100;
    StoreInfo.recordOAD.infoNum = Pn;

    StoreInfo.recordOAD.road.oadMain=MainOad;
    StoreInfo.recordOAD.road.oadCols.nNum=1;
    StoreInfo.recordOAD.road.oadCols.oad[0]=Oad;
    StoreInfo.recordOAD.classtag = 5;
    
    taskmng_mtr_addr_get( Pn,LOGIC_ID_YC, (uint8*)StoreInfo.MAC.nValue);
    //开始时间和结束时间都取当前时间
    StoreInfo.colStartTm = StoreInfo.colEndTm = time(NULL);
    if(StoreTime == NULL)
    {
        StoreInfo.colStoreTm = time(NULL);
    }
    else
        DT_DateTimeBCD2Time(StoreTime,(time_t*)&StoreInfo.colStoreTm);
   
    taskmng_meter_data_save(DataBuff,DataLen ,StoreInfo, pipe );

    return offset;
}

/*******************************************************************************
* 函数名称: taskmng_645_relay_data_save
* 函数功能://存储组帧穿透645数据
* 输入参数: pipe   端口号
        prtl    规约类型
        pRcvBuf    回码数据
        RcvLen     回码长度

* 输出参数:
* 返 回 值:
*******************************************************************************/
void taskmng_645_relay_data_save(uint8 pipe,uint8 prtl,uint8* pRcvBuf, uint16 RcvLen)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    int ret;
    DataSource sourcedata;

    uint16 DataLen = 0;
    uint8 DataBuff[200]={0};
    OadStoreInfo StoreInfo;
    memset(&StoreInfo,0x00,sizeof(OadStoreInfo));
    uint8 MtrIndex = pAmrThreadArg->RelayTrans.AmrIndex;
    uint16 MapIndex = pAmrThreadArg->RelayTrans.MapIndex;
    uint16 Pn = taskmng_pn_get_from_tsa(pAmrThreadArg->RelayTrans.AmrNormal[MtrIndex].Mtraddr,LOGIC_ID_YC);
    const MapOopTo645_NEW *pOadInfo = &MapOopTo645_RT[MapIndex];

    ret = CacloffSpecialRelay(pipe,prtl,pOadInfo,MapIndex,pRcvBuf,&RcvLen);
    if(ret<0)
        return ;
    sourcedata.reclen = RcvLen;
    memcpy(sourcedata.sourcebuffer,pRcvBuf,RcvLen);
    TASK_FMT_TRACE(pipe, RELAY_LOG,"rcvLen %d 组帧穿透回复数据\n",RcvLen);

    TASK_BUF_TRACE(pipe, RELAY_LOG,pRcvBuf, RcvLen);
    if(prtl == PRTL_97)
        Convert_6457_97_To_Struct(sourcedata,DataBuff,&DataLen,pOadInfo);
    else
        Convert_6457_07_To_Struct(sourcedata,DataBuff,&DataLen,pOadInfo);
    //TASK_FMT_TRACE(pipe, RELAY_LOG,"Convert_6457_07_To_Struct  DataLen %d 转换后数据",DataLen);
    
    
   // TASK_BUF_TRACE(pipe, RELAY_LOG,DataBuff, DataLen);

    StoreInfo.recordOAD.logicId=100;
    StoreInfo.recordOAD.infoNum = Pn;

    StoreInfo.recordOAD.road.oadMain.value=0x00000000;
    StoreInfo.recordOAD.road.oadCols.nNum=1;
    StoreInfo.recordOAD.road.oadCols.oad[0]= pOadInfo->Oad;
    StoreInfo.recordOAD.classtag = 5;
    taskmng_mtr_addr_get( Pn, LOGIC_ID_YC,(uint8*)StoreInfo.MAC.nValue);
    //开始时间和结束时间都取当前时间
    StoreInfo.colStartTm = StoreInfo.colEndTm =StoreInfo.colStoreTm = time(NULL);
    
    taskmng_meter_data_save(DataBuff,DataLen ,StoreInfo, pipe );

    
}
/*******************************************************************************
* 函数名称: CacloffSpecialRelay
* 函数功能: 穿透645表时间需要特殊处理
 输入参数: pipe        端口号
        MsIndex      电表索引
        pOadInfo    面向对象和645数据项的转换结构
        MapIndex    645map索引
            pBuf          645回码数据
*           rcvLen        645回码长度

* 输出参数: 无
* 返 回 值: -1 回复的不全暂存在内存中 0 时间组合完毕，可以存储 
*******************************************************************************/
int CacloffSpecialRelay(uint8 Pipe,uint8 prtl,const MapOopTo645_NEW *pOadInfo,uint16  MapIndex,uint8 *pBuf, uint16 *rcvLen)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[Pipe];
    const MapOopTo645_NEW * tmp =pOadInfo;
    uint8 TmpBuff[200] = {0};

    tmp++;
    if(pOadInfo->Oad.value == tmp->Oad.value)
    {
        if(prtl==PRTL_07)
        {
            if(pOadInfo->DI07!=tmp->DI07&&pOadInfo->DI07!=0xffffffff&&tmp->DI07!=0xffffffff)
            {
                memset(pAmrThreadArg->RelayTrans.SpecialTmpBuf,0x00,MAX_SPECIAL_TMP_LEN);
                memcpy(pAmrThreadArg->RelayTrans.SpecialTmpBuf,pBuf,*rcvLen);
                pAmrThreadArg->RelayTrans.SpecialTmpLen = *rcvLen;
                return -1;
            }
        }
        else if(prtl==PRTL_97)
        {
            if(pOadInfo->DI97!=tmp->DI97&&pOadInfo->DI97!=0xffff&&tmp->DI97!=0xffff)
            {
                memset(pAmrThreadArg->RelayTrans.SpecialTmpBuf,0x00,MAX_SPECIAL_TMP_LEN);
                memcpy(pAmrThreadArg->RelayTrans.SpecialTmpBuf,pBuf,*rcvLen);
                pAmrThreadArg->RelayTrans.SpecialTmpLen = *rcvLen;
                return -1;
            }
        }
    }
    if(MapIndex>0)
    {
        tmp =pOadInfo;
        tmp--;
        if(pOadInfo->Oad.value == tmp->Oad.value)
        {
            if(prtl==PRTL_07)
            {
                if(pOadInfo->DI07!=tmp->DI07&&pOadInfo->DI07!=0xffffffff&&tmp->DI07!=0xffffffff)
                {
                    memcpy(TmpBuff,pAmrThreadArg->RelayTrans.SpecialTmpBuf,pAmrThreadArg->RelayTrans.SpecialTmpLen );
                    memcpy(&TmpBuff[pAmrThreadArg->RelayTrans.SpecialTmpLen],pBuf,*rcvLen);
                    *rcvLen +=pAmrThreadArg->RelayTrans.SpecialTmpLen;
                    memcpy(pBuf,TmpBuff,*rcvLen);
                    
                }
            }
            else if(prtl==PRTL_97)
            {
                if(pOadInfo->DI97!=tmp->DI97&&pOadInfo->DI97!=0xffff&&tmp->DI97!=0xffff)
                {
                    memcpy(TmpBuff,pAmrThreadArg->RelayTrans.SpecialTmpBuf,pAmrThreadArg->RelayTrans.SpecialTmpLen );
                    memcpy(&TmpBuff[pAmrThreadArg->RelayTrans.SpecialTmpLen],pBuf,*rcvLen);
                    *rcvLen +=pAmrThreadArg->RelayTrans.SpecialTmpLen;
                    memcpy(pBuf,TmpBuff,*rcvLen);
                }
            }
        }
    }

    return 0;
}

/*******************************************************************************
* 函数名称: OopPatchMeterAmrDataSave_Record
* 函数功能:补抄存储面向对象规约表记录型数据解析
* 输入参数: pipe 端口号
            DataBuf              //接收到的电表数据
            DataLen              //接收到的电表长度
            MtrIndex             //电表索引信息
            pTmp_NormalLink      //方案任务节点
            RecvRunTask     //任务运行时参数
            StoreTime            //存储时间
* 输出参数:
* 返 回 值:
*******************************************************************************/
uint16 OopPatchMeterAmrDataSave_Record(uint8 Pipe,uint8 *DataBuf,uint16 DataLen,uint16 MtrIndex,SCHEME_TASK_RUN_INFO* pTmp_NormalLink,uint8 flag)
{
    AmrThreadArg_T  *pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[Pipe];
    
    uint16           Dataoffset = 0;
    uint8           CsdNum = DataBuf[Dataoffset++];    //CSD数目
    OOP_OAD_U            Oad[128];
    uint8           i;
    uint8           j;
    uint8           RecordNum = 0;
    
    //uint16          Pn;
    uint16          OadLen = 0;

    DateTimeBcd_t   StoreTm;
    OadInfoMap   pMap;
    OOP_OAD_U            OadTmp;
    OOP_OAD_U       MainOad;

    //TASK_FMT_TRACE(Pipe, REC_LOG,"698补抄存储记录型数据 SchemeType %d RoadIndex %d CsdNum %d\n",pTmp_NormalLink->SchemeType,CsdNum);
    //变量RCSD对应的OAD信息
    for(i = 0; i < CsdNum; i++)
    {
        if(DataBuf[Dataoffset++] != 0)
            return 0;
        memcpy_r((uint8*)&Oad[i].value,&DataBuf[Dataoffset],4);
        Dataoffset += 4;
        //TASK_FMT_TRACE(Pipe, REC_LOG,"返回的csd %d  oad %08x \n",i,Oad[i].value);
    }
    //错误信息 [0] DAR
    if(DataBuf[Dataoffset++] == 0x0)
    {
        //printf("错误信息 [0] DAR : %d  %d  %d\n",MtrIndex,pTmp_NormalLink->MsInfo[MtrIndex].CSDIndex,pTmp_NormalLink->MsInfo[MtrIndex].SigFrameNum);
        pAmrThreadArg->PatchMtrInfo[MtrIndex].CSDIndex += pAmrThreadArg->PatchMtrInfo[MtrIndex].SigFrameNum;
        return 0;
    }
    MainOad = pTmp_NormalLink->CsdTask.RoadTask[0].MainOad;
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
            OadTmp = Oad[j];
            OadTmp.attPro = 0;

            if(taskmng_oad_info_get(OadTmp,&pMap)==FALSE)
            {
                pAmrThreadArg->PatchMtrInfo[MtrIndex].CSDIndex += pAmrThreadArg->PatchMtrInfo[MtrIndex].SigFrameNum;
                return 0;
            }
            
            if(pTmp_NormalLink->StoreTimeType == STRTMTYP_FRZTIME)
            {
                //抄读record 的时候第一个OI为时标
                if(Oad[j].nObjID == 0x2021)
                {
                    DateTimeHex_t        TmpDateTimeHex;
                    memset(&TmpDateTimeHex,0x0,sizeof(TmpDateTimeHex));
                    TmpDateTimeHex.year_h = DataBuf[Dataoffset+1];
                    TmpDateTimeHex.year_l = DataBuf[Dataoffset+2];
                    TmpDateTimeHex.month  = DataBuf[Dataoffset+3];
                    TmpDateTimeHex.day    = DataBuf[Dataoffset+4];
                    TmpDateTimeHex.hour   = DataBuf[Dataoffset+5];
                    TmpDateTimeHex.min    = DataBuf[Dataoffset+6];

                    if(DataBuf[Dataoffset+7] != 0xff)//曲线秒回复为0xff(林洋电表)
                        TmpDateTimeHex.sec    = DataBuf[Dataoffset+7];

                    DT_DateTimeHex2BCD(&TmpDateTimeHex,&StoreTm);

                }
            }
            else
            {
                StoreTm = taskmng_patch_scheme_task_store_time_get(Pipe,MtrIndex,pTmp_NormalLink);
            }
            //TASK_FMT_TRACE(Pipe, REC_LOG, "第%d条记录第%d个数据项  \n",i,j);
            TASK_FMT_TRACE(Pipe, REC_LOG,"StoreTm %02x/%02x/%02x %02x:%02x:%02x \n",StoreTm.year_l,StoreTm.month,StoreTm.day,StoreTm.hour,StoreTm.min,StoreTm.sec);

            OadLen = OopPatchAmrData(Pipe,MtrIndex,MainOad,Oad[j],DataBuf+Dataoffset,(DataLen-Dataoffset),pTmp_NormalLink,&StoreTm);
            Dataoffset += OadLen;
        }
    }
    if(flag!=1)
        pAmrThreadArg->PatchMtrInfo[MtrIndex].CSDIndex += pAmrThreadArg->PatchMtrInfo[MtrIndex].SigFrameNum;

    return 1;
}

/*******************************************************************************
* 函数名称: OopPatchAmrData
* 函数功能:补抄存储698数据
* 输入参数: pipe 端口号
         MtrIndex 表索引
         MainOad 主oad
         Oad 子oad
         pRcvBuf 收到的数据buf
         RcvLen 收到的数据长度
         pTmp_NormalLink 任务参数
         RecvRunTask 任务运行时参数
         StoreTime 存储时间

* 输出参数:
* 返 回 值:已解析存储的数据长度
*******************************************************************************/
uint16 OopPatchAmrData(uint8 Pipe,uint16 MtrIndex,OOP_OAD_U MainOad,OOP_OAD_U Oad, uint8* pRcvBuf, uint16 RcvLen, SCHEME_TASK_RUN_INFO* pTmp_NormalLink, DateTimeBcd_t *StoreTime)
{
    AmrThreadArg_T *pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[Pipe];
    uint16 offset =0 ;
    uint16 DataLen = 0;
    uint8 DataBuff[MAX_MSG_LEN]={0};
    uint16 Pn;
    uint8 logicid;
    OadStoreInfo StoreInfo;
    memset(&StoreInfo,0x00,sizeof(OadStoreInfo));
    OadInfoMap pOadInfo;
    memset(&pOadInfo,0x00,sizeof(OadInfoMap));
    BOOL ret;
    uint8 pwrType;

    Pn = pAmrThreadArg->PatchMtrInfo[MtrIndex].Pn;
    logicid = pAmrThreadArg->PatchMtrInfo[MtrIndex].logicid;
     //根据oad获取对应的结构体信息
    ret = taskmng_oad_info_get( Oad,&pOadInfo);
    //TASK_FMT_TRACE(Pipe,REC_LOG,"OopMeterAmrData OAD %08x ret %d \n",Oad.value,ret);
    if(ret==FALSE)
    {
        return 0;
    }
    if(pRcvBuf[0]==0) //如果为datanull 则无需解析
    {
        TASK_FMT_TRACE(Pipe,REC_LOG,"data为空 不解析\n");
        return 1;
    }
    pwrType=taskmng_mtr_conntype_get(Pn,logicid);
    Convert_Oop_Data_To_Struct(pRcvBuf,&offset,RcvLen,DataBuff,&pOadInfo,&DataLen,pwrType);
    //TASK_FMT_TRACE(Pipe, REC_LOG,"Convert_Oop_Data_To_Struct  DataLen %d 转换后数据 \n",DataLen);

    //TASK_BUF_TRACE(Pipe, REC_LOG,DataBuff, DataLen);

    StoreInfo.recordOAD.logicId=logicid;
    StoreInfo.recordOAD.infoNum = Pn;
    StoreInfo.recordOAD.road.oadMain=MainOad;
    StoreInfo.recordOAD.road.oadCols.nNum=1;
    StoreInfo.recordOAD.road.oadCols.oad[0]=Oad;
    StoreInfo.recordOAD.classtag = 5;
    taskmng_mtr_addr_get( Pn,logicid, (uint8*)StoreInfo.MAC.nValue);

    StoreInfo.colStartTm =pAmrThreadArg->PatchMtrInfo[MtrIndex].EndTime ;     //开始时间
    //加上任务开始时间里包含的延时
    OOP_TI_T TI;
    TI.unit = pTmp_NormalLink->DelayTimeUnit;
    TI.value = pTmp_NormalLink->DelayTime;
    DT_TimeAddInv((time_t*)&StoreInfo.colStartTm, TI);
    DT_DateTimeBCD2Time(StoreTime,(time_t*)&StoreInfo.colStoreTm);
    StoreInfo.colEndTm = time(NULL);
    taskmng_meter_data_save(DataBuff,DataLen ,StoreInfo, Pipe );

    return offset;
}

/*******************************************************************************
* 函数名称: OopPatchAmrData
* 函数功能:补抄存储645冻结数据
* 输入参数:     pipe 端口号
            pBuf          645回码数据
*           rcvLen        645回码长度
*           MtrIndex      电表索引
*           Tmp_AmrLink   方案任务节点

* 输出参数:
* 返 回 值:
*******************************************************************************/
uint16 OopPatchDataSave9707_frz(uint8 Pipe,uint8 *pBuf, uint16 rcvLen, int MsIndex, SCHEME_TASK_RUN_INFO* Tmp_AmrLink)
{

    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[Pipe];
    DateTimeBcd_t       storeTm;
    time_t              longTm;
    time_t              StartTime,endTime;
 
    uint8               CSDIndex = pAmrThreadArg->PatchMtrInfo[MsIndex].CSDIndex;

    if(Tmp_AmrLink->CsdTask.RoadTask[0].MainOad.nObjID == 0x5002)
    {
        
        uint16 Pn = pAmrThreadArg->PatchMtrInfo[MsIndex].Pn;
        if(taskmng_mtr_conntype_get(Pn,pAmrThreadArg->PatchMtrInfo[MsIndex].logicid)==1)
        {
            OopPatchDataSave_curve(Pipe,pBuf, rcvLen, MsIndex, Tmp_AmrLink);
        }else
        {
            OopPatchDataSave_profile(Pipe,pBuf, rcvLen, MsIndex, Tmp_AmrLink);
        }
        return 0;
    }


    TASK_FMT_TRACE(Pipe, REC_LOG,"645冻结数据处理\n");

    StartTime = pAmrThreadArg->PatchMtrInfo[MsIndex].StartTime;
    endTime = pAmrThreadArg->PatchMtrInfo[MsIndex].EndTime;
    
    if(Tmp_AmrLink->AcqInfo.Acqtype == ACQTYP_FRZTM || Tmp_AmrLink->AcqInfo.Acqtype == ACQTYP_TI
            || Tmp_AmrLink->StoreTimeType == STRTMTYP_FRZTIME )
    {
        //按照冻结时标采集，需要判断数据是否在时间间隔范围内
        if(CSDIndex == 0)
        {
            if(Tmp_AmrLink->CsdTask.RoadTask[0].OadListInfo[CSDIndex].nObjID != 0x2021)//第一列必须是冻结时标
            {
                pAmrThreadArg->PatchMtrInfo[MsIndex].EndNum=0;
                return 0;
            }

            if(Tmp_AmrLink->CsdTask.RoadTask[0].MainOad.nObjID == 0x5006 || Tmp_AmrLink->CsdTask.RoadTask[0].MainOad.nObjID == 0x5005)
            {
                memcpy_r(&storeTm.year_l,pBuf,4);
                storeTm.min = 0;
            }
            else
                memcpy_r(&storeTm.year_l,pBuf,5);

            storeTm.year_h = 0x20;
            storeTm.sec= 0x00;
            storeTm.min= 0x00;
            if(Tmp_AmrLink->CsdTask.RoadTask[0].MainOad.nObjID!= 0x5003)
                storeTm.hour= 0x00;
            // commfun_printstring((uint8*)&storeTm,7);//打印采集到的存储时间
            TASK_FMT_TRACE(Pipe, REC_LOG,"采集到的冻结时标 %02x/%02x/%02x %02x:%02x:%02x\n",storeTm.year_l,storeTm.month,
                        storeTm.day,storeTm.hour,storeTm.min,storeTm.sec);

            if(DT_DateTimeBCD2Time(&storeTm,&longTm)==FALSE)//应答的时间不合法
            {
                //不存储
                memset(&pAmrThreadArg->PatchMtrInfo[MsIndex].storeTm, 0x0 ,sizeof(DateTimeBcd_t));
                pAmrThreadArg->PatchMtrInfo[MsIndex].EndNum=0;
                return 0;
            }
            TASK_FMT_TRACE(Pipe, REC_LOG,"time_t 上报的时标 %ld StartTime %ld endTime %ld \n",longTm,StartTime,endTime);
            if(Tmp_AmrLink->AcqInfo.Acqtype == ACQTYP_FRZTM || Tmp_AmrLink->AcqInfo.Acqtype == ACQTYP_TI )
            {
                //小时冻结,只抄上次冻结时间在区间内-前闭后开
                if(Tmp_AmrLink->CsdTask.RoadTask[0].MainOad.nObjID == 0x5003)
                {
                    if(longTm < StartTime)
                    {
                        //冻结时标比起始时间还小，不需要继续搜索了
                        memset(&pAmrThreadArg->PatchMtrInfo[MsIndex].storeTm, 0x0 ,sizeof(DateTimeBcd_t));
                        pAmrThreadArg->PatchMtrInfo[MsIndex].EndNum=0;
                        return 0;
                    }
                    if(longTm >= endTime)
                    {
                        //不在采集的时间间隔内，不存储，继续搜索上一条记录
                        memset(&pAmrThreadArg->PatchMtrInfo[MsIndex].storeTm, 0x0 ,sizeof(DateTimeBcd_t));
                        return 0;
                    }
                }
                else
                {
                    if(longTm <= StartTime)
                    {
                        //冻结时标比起始时间还小，不需要继续搜索了
                        memset(&pAmrThreadArg->PatchMtrInfo[MsIndex].storeTm, 0x0 ,sizeof(DateTimeBcd_t));
                        pAmrThreadArg->PatchMtrInfo[MsIndex].EndNum=0;
                        return 0;
                    }
                    if(longTm > endTime)
                    {
                        //不在采集的时间间隔内，不存储，继续搜索上一条记录
                        memset(&pAmrThreadArg->PatchMtrInfo[MsIndex].storeTm, 0x0 ,sizeof(DateTimeBcd_t));
                        return 0;
                    }
                }
            }
            pAmrThreadArg->PatchMtrInfo[MsIndex].storeTm = storeTm;//保存存储时标
            if(Tmp_AmrLink->StoreTimeType!=STRTMTYP_FRZTIME)
            {
                pAmrThreadArg->PatchMtrInfo[MsIndex].EndNum=pAmrThreadArg->PatchMtrInfo[MsIndex].CurNum;
            }
        }
    }
    
    if(Tmp_AmrLink->StoreTimeType == STRTMTYP_FRZTIME)
    {
        storeTm =pAmrThreadArg->PatchMtrInfo[MsIndex].storeTm;
    }
    else
    {
        storeTm = taskmng_acq_store_time_get(Tmp_AmrLink->StoreTimeType,pAmrThreadArg->PatchMtrInfo[MsIndex].EndTime);
        pAmrThreadArg->PatchMtrInfo[MsIndex].storeTm = storeTm;
    }
    TASK_FMT_TRACE(Pipe, REC_LOG,"存储时标 %02x/%02x/%02x %02x:%02x:%02x\n",storeTm.year_l,storeTm.month,
                        storeTm.day,storeTm.hour,storeTm.min,storeTm.sec);

    OopPatchDataSave9707(Pipe,pBuf, rcvLen, MsIndex, Tmp_AmrLink,&storeTm);

    return 0;
}
/*******************************************************************************
* 函数名称: OopPatchAmrData
* 函数功能:补抄存储645数据
* 输入参数: pipe        端口号
            pBuf          645回码数据
*           rcvLen        645回码长度
*           MtrIndex      电表索引
*           Tmp_AmrLink   方案任务节点
*           StoreTime     数据存储时间

* 输出参数:
* 返 回 值:
*******************************************************************************/
void OopPatchDataSave9707(uint8 Pipe,uint8 *pBuf, uint16 rcvLen, int MsIndex, SCHEME_TASK_RUN_INFO* Tmp_AmrLink, DateTimeBcd_t *StoreTime)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[Pipe];
    uint16 Pn = pAmrThreadArg->PatchMtrInfo[MsIndex].Pn;
    uint8 logicid = pAmrThreadArg->PatchMtrInfo[MsIndex].logicid;
    uint8 prtl ;            
    OOP_OAD_U   MainOad,subOad;
    
    uint8   CSDIndex=0;
    uint16  MapIndex=0;
    const MapOopTo645_NEW *pOadInfo=NULL;
    DataSource sourcedata;
    uint16 DataLen = 0;
    uint8 DataBuff[200]={0};
    taskmng_mtr_prtl_get( Pn,logicid,&prtl);
    OadStoreInfo StoreInfo;
    memset(&StoreInfo,0x00,sizeof(OadStoreInfo));
    int ret;
    
    const MapOopTo645_NEW*     Map;
    uint16  MapNum;
    OOP_METER_BASIC_T basicinfo;
    
    taskmng_mtr_basic_info_get( Pn, logicid,&basicinfo);

    MainOad = Tmp_AmrLink->CsdTask.RoadTask[0].MainOad;
    CSDIndex = pAmrThreadArg->PatchMtrInfo[MsIndex].CSDIndex;
    subOad = Tmp_AmrLink->CsdTask.RoadTask[0].OadListInfo[CSDIndex];
    MapIndex = pAmrThreadArg->PatchMtrInfo[MsIndex].MapIndex;

    Map = taskmng_map_get_from_mainoad(Pipe,MsIndex,Tmp_AmrLink,&MapNum);
    pOadInfo = &Map[MapIndex];

    //TASK_FMT_TRACE(Pipe, REC_LOG,"taskmng_645_meter_data_save  \n");
   // TASK_FMT_TRACE(Pipe, REC_LOG,"mainoda 0x%08x CSDIndex %d  subOad 0x%08x mapindex %d \n",MainOad.value,CSDIndex,subOad.value,MapIndex);
    //TASK_FMT_TRACE(Pipe, REC_LOG,"poadinfo oad  0x%08x 07DI %08x  CovertSruct %d \n",pOadInfo->Oad.value,pOadInfo->DI07,pOadInfo->CovertSruct);
    
    ret =CacloffSpecialPatch(Pipe,MsIndex,pOadInfo,MapIndex,pBuf,&rcvLen);
    if(ret<0)
        return;
    sourcedata.reclen = rcvLen;
    sourcedata.pwrType = basicinfo.pwrType;
    memcpy(sourcedata.sourcebuffer,pBuf,rcvLen);
    
    //TASK_FMT_TRACE(Pipe, REC_LOG,"rcvLen %d 抄表回复数据 \n",rcvLen);

   // TASK_BUF_TRACE(Pipe, REC_LOG,pBuf, rcvLen);
    Convert_6457_07_To_Struct(sourcedata,DataBuff,&DataLen,pOadInfo);
    //TASK_FMT_TRACE(Pipe, REC_LOG,"Convert_6457_07_To_Struct  DataLen %d 转换后数据\n",DataLen);

   // TASK_BUF_TRACE(Pipe, REC_LOG,DataBuff, DataLen);

    StoreInfo.recordOAD.logicId=logicid;
    StoreInfo.recordOAD.infoNum = Pn;

    StoreInfo.recordOAD.road.oadMain=MainOad;
    StoreInfo.recordOAD.road.oadCols.nNum=1;
    StoreInfo.recordOAD.road.oadCols.oad[0]=subOad;
    StoreInfo.recordOAD.classtag = 5;
    taskmng_mtr_addr_get( Pn,logicid, (uint8*)StoreInfo.MAC.nValue);
    StoreInfo.colStartTm = pAmrThreadArg->PatchMtrInfo[MsIndex].EndTime;
    OOP_TI_T delay;
    delay.unit = Tmp_AmrLink->DelayTimeUnit;
    delay.value = Tmp_AmrLink->DelayTime;
    DT_TimeAddInv((time_t*)&StoreInfo.colStartTm, delay);
    DateTimeBcd_t storetm;
    if(StoreTime==NULL)
    {
        storetm= taskmng_acq_store_time_get(Tmp_AmrLink->StoreTimeType,pAmrThreadArg->PatchMtrInfo[MsIndex].EndTime);
        DT_DateTimeBCD2Time(&storetm,(time_t*)&StoreInfo.colStoreTm);
    }
    else
        DT_DateTimeBCD2Time(StoreTime,(time_t*)&StoreInfo.colStoreTm);
    StoreInfo.colEndTm = time(NULL);
    taskmng_meter_data_save(DataBuff,DataLen ,StoreInfo, Pipe );
    
}

/*******************************************************************************
* 函数名称: CacloffSpecialPatch
* 函数功能: 补抄645表时间需要特殊处理
 输入参数: pipe        端口号
        MsIndex      电表索引
        pOadInfo    面向对象和645数据项的转换结构
        MapIndex    645map索引
            pBuf          645回码数据
*           rcvLen        645回码长度

* 输出参数: 无
* 返 回 值: -1 回复的不全暂存在内存中 0 时间组合完毕，可以存储 
*******************************************************************************/
int CacloffSpecialPatch(uint8 Pipe,uint16 MsIndex,const MapOopTo645_NEW *pOadInfo,uint16  MapIndex,uint8 *pBuf, uint16 *rcvLen)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[Pipe];
    const MapOopTo645_NEW * tmp =pOadInfo;
    uint8 TmpBuff[200] = {0};
    uint8 prtl;
    taskmng_mtr_prtl_get(pAmrThreadArg->PatchMtrInfo[MsIndex].Pn,pAmrThreadArg->PatchMtrInfo[MsIndex].logicid,&prtl);
    tmp++;
    if(pOadInfo->Oad.value == tmp->Oad.value)
    {
        if(prtl==PRTL_07)
        {
            if(pOadInfo->DI07!=tmp->DI07&&pOadInfo->DI07!=0xffffffff&&tmp->DI07!=0xffffffff)
            {
                memset(pAmrThreadArg->PatchMtrInfo[MsIndex].SpecialTmpBuf,0x00,MAX_SPECIAL_TMP_LEN);
                memcpy(pAmrThreadArg->PatchMtrInfo[MsIndex].SpecialTmpBuf,pBuf,*rcvLen);
                pAmrThreadArg->PatchMtrInfo[MsIndex].SpecialTmpLen = *rcvLen;
                return -1;
            }
        }
    }
    if(MapIndex>0)
    {
        tmp =pOadInfo;
        tmp--;
        if(pOadInfo->Oad.value == tmp->Oad.value)
        {
            if(prtl==PRTL_07)
            {
                if(pOadInfo->DI07!=tmp->DI07&&pOadInfo->DI07!=0xffffffff&&tmp->DI07!=0xffffffff)
                {
                    memcpy(TmpBuff,pAmrThreadArg->PatchMtrInfo[MsIndex].SpecialTmpBuf,pAmrThreadArg->PatchMtrInfo[MsIndex].SpecialTmpLen );
                    memcpy(&TmpBuff[pAmrThreadArg->PatchMtrInfo[MsIndex].SpecialTmpLen],pBuf,*rcvLen);
                    *rcvLen +=pAmrThreadArg->PatchMtrInfo[MsIndex].SpecialTmpLen;
                    memcpy(pBuf,TmpBuff,*rcvLen);
                    
                }
            }
        }
    }

    return 0;
}
/*******************************************************************************
* 函数名称: OopPatchDataSave_curve
* 函数功能: 补抄存储07表(14年单相0610xxxx)负荷曲线
* 输入参数:     pipe        端口号
            pBuf          645回码数据
*           rcvLen        645回码长度
*           MtrIndex      电表索引
*           Tmp_AmrLink   方案任务节点
            RecvRunTask     任务运行时参数
* 输出参数: 无
* 返 回 值: 无
*******************************************************************************/
void OopPatchDataSave_curve(uint8 Pipe,uint8 *pBuf, uint16 rcvLen, int MsIndex,SCHEME_TASK_RUN_INFO* Tmp_AmrLink)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[Pipe];
    uint8         offset=0;
    DateTimeBcd_t storeTm;
    uint8         PointNum;
    uint16        DataLen;
    time_t        TempTimeSec;
    OOP_TI_T          DelayTi;
    int           i;
    uint16 MapIndex = pAmrThreadArg->PatchMtrInfo[MsIndex].MapIndex;
    if(rcvLen<5+2)
        return ;
    memcpy_r(&storeTm.year_l,pBuf,5);
    storeTm.year_h = 0x20;
    storeTm.sec = 0x00;
    if(DT_DateTimeBCD2Time(&storeTm, &TempTimeSec) == FALSE)
        return;

    offset += 5;

    DataLen =MapOopTo645_MinOne[MapIndex].TotalDataLen;
    PointNum = (rcvLen-5)/DataLen;
    for(i=0;i<PointNum;i++)
    {
         //计算每个时间点的存储时标
        DelayTi.value = 15;
        DelayTi.unit = 1;  //07表负荷曲线间隔15分
        if(i != 0)
            DT_TimeAddInv(&TempTimeSec, DelayTi);

        //无效数据不存储
        if(pBuf[offset] == 0xff && pBuf[offset+1] == 0xff)
        {
            offset += DataLen;
            continue;
        }
        if(Tmp_AmrLink->AcqInfo.TI.unit == TASK_CYCLE_FLAG_HOUR
            || (Tmp_AmrLink->AcqInfo.TI.unit == TASK_CYCLE_FLAG_MIN && Tmp_AmrLink->AcqInfo.TI.value == 60))
        {
            if(i%4 != 0)
            {
                offset += DataLen;
                continue;
            }
        }
        DT_Time2DateTimeBCD(TempTimeSec, &storeTm);
        TASK_FMT_TRACE(Pipe, REC_LOG,"存储时标时标 %02x年 %02x月%02x日 %02x:%02x:%02x \n",storeTm.year_l,storeTm.month,storeTm.day,storeTm.hour,storeTm.min,storeTm.sec);

        OopPatchDataSave9707(Pipe,&pBuf[offset], DataLen, MsIndex, Tmp_AmrLink,&storeTm);
        offset += DataLen;
    }
    return ;
}
/*******************************************************************************
* 函数名称: OopPatchDataSave_profile
* 函数功能:补抄存储07表负荷曲线
* 输入参数:     pipe 端口号
            pBuf          645回码数据
*           rcvLen        645回码长度
*           MtrIndex      电表索引
*           Tmp_AmrLink   方案任务节点
            RecvRunTask     任务运行时参数
* 输出参数: 无
* 返 回 值: 无
*******************************************************************************/
void OopPatchDataSave_profile(uint8 Pipe,uint8 *pBuf, uint16 rcvLen, int MsIndex,SCHEME_TASK_RUN_INFO* Tmp_AmrLink)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[Pipe];
    uint8 offset=0;
    uint8 profileBuff[5+17+24+8+16+16+6];
    DateTimeBcd_t storeTm;
    uint8 i,subOadNum;
    uint8 DataLen;
    uint16 bias = 0xFFFF;

    memset(profileBuff,0x00,sizeof(profileBuff));
    if(pBuf[0]!=0xA0 || pBuf[1]!=0xA0)
        return;
    offset += 3;
    memcpy(profileBuff,pBuf+offset,5);
    offset += 5;

    if(pBuf[offset]!=0xAA)
    {
        memcpy(profileBuff+5,pBuf+offset,17);
        offset += 17;
    }
    //功率
    if(pBuf[offset++]!=0xAA)
    {
        printf("功率偏移错误!\n");
        return;
    }
    if(pBuf[offset]!=0xAA)
    {
        memcpy(profileBuff+22,pBuf+offset,24);
        offset += 24;
    }
    //功率因数
    if(pBuf[offset++]!=0xAA)
    {
        printf("功率因数偏移错误!\n");
        return;
    }
    if(pBuf[offset]!=0xAA)
    {
        memcpy(profileBuff+46,pBuf+offset,8);
        offset += 8;
    }
    //有功电能
    if(pBuf[offset++]!=0xAA)
    {
        printf("有功电能偏移错误!\n");
        return;
    }
    if(pBuf[offset]!=0xAA)
    {
        memcpy(profileBuff+54,pBuf+offset,16);
        //commfun_printstring(profileBuff+54,16);
        offset += 16;
    }
    //无功电能
    if(pBuf[offset++]!=0xAA)
    {
        printf("无功电能偏移错误!\n");
        return;
    }
    if(pBuf[offset]!=0xAA)
    {
        memcpy(profileBuff+70,pBuf+offset,16);
        offset += 16;
    }
    //当前需量
    if(pBuf[offset++]!=0xAA)
    {
        printf("当前需量偏移错误!\n");
        return;
    }
    if(pBuf[offset]!=0xAA)
    {
        memcpy(profileBuff+86,pBuf+offset,6);
        offset += 6;
    }
    if(pBuf[offset++]!=0xAA)
    {
        printf("校验和偏移错误!\n");
        return;
    }
    //校验和判断
    offset++;
    //结束符判断
    if(pBuf[offset++]!=0xe5)
    {
        printf("结束符偏移错误!\n");
        return;
    }

    memcpy_r(&storeTm.year_l,profileBuff,5);

    storeTm.year_h = 0x20;
    storeTm.sec = 0x00;
    subOadNum = Tmp_AmrLink->CsdTask.RoadTask[0].SubOadNum;
    if(subOadNum == 0)
        return;
    i=0;
    while(i<subOadNum)
    {
        pAmrThreadArg->PatchMtrInfo[MsIndex].CSDIndex= i;           //当前CSD索引
        bias = CaclPatchoffsByOad(Pipe,Tmp_AmrLink,MsIndex, &DataLen);
        //printf("bias = %d,DataLen = %d\n",bias,DataLen);
        if((bias+DataLen) > 92 && bias!=0xFFFF)
            return;    //防止电表回码长度不够
        if(bias!=0xFFFF)
        {
            OopPatchDataSave9707(Pipe,&profileBuff[bias], DataLen,MsIndex, Tmp_AmrLink,&storeTm);
        }
        i++;
    }
}
/****************************************************************************
*模块编号：
*名称：
*功能：补抄根据OAD计算 OAD的数据在数据块中的偏移，供07表冻结，负荷曲线，事件记录使用
*      回码格式必须与列表格式一致，若不一致需将回码修正为列表格式再计算
*输入参数：pipe 端口号
        Tmp_AmrLink 任务参数
        MsIndex 电表索引
*输出参数：DataLen 数据在数据块中的偏移
*处理：
*返回：无
****************************************************************************/
uint16 CaclPatchoffsByOad(uint8 Pipe,SCHEME_TASK_RUN_INFO* Tmp_AmrLink,int MsIndex,uint8 *DataLen)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[Pipe];
    uint16                      offSet=0;
    uint8                       CSDIndex;
    OOP_OAD_U                   OAD;
    uint8                       OadTag;

    const MapOopTo645_NEW      *pMapOopTo645=NULL;

    CSDIndex = pAmrThreadArg->PatchMtrInfo[MsIndex].CSDIndex;
    
    
    if(Tmp_AmrLink->CsdTask.RoadTask[0].MainOad.nObjID == 0x5002)  //分钟曲线
        pMapOopTo645 = MapOopTo645_Min;
    else
        return 0xFFFF;

    OAD = Tmp_AmrLink->CsdTask.RoadTask[0].OadListInfo[CSDIndex];
    OadTag = Tmp_AmrLink->CsdTask.RoadTask[0].OadListInfo[CSDIndex].attPro;
    
    TASK_FMT_TRACE(Pipe, REC_LOG,"CacloffsByOad MainOad 0x%08x OAD 0x%08x OadTag %d\n",Tmp_AmrLink->CsdTask.RoadTask[0].MainOad,OAD,OadTag);
    
    pAmrThreadArg->PatchMtrInfo[MsIndex].MapIndex = 0;
    
    while(pMapOopTo645->Oad.value!=0xFFFFFFFF)
    {
        if(OAD.value == pMapOopTo645->Oad.value && OadTag == pMapOopTo645->Oad.attPro)
        {
            *DataLen = pMapOopTo645->TotalDataLen;
            break;
        }
        else
        {
            if(CurvePatchDiAdapt(Pipe,Tmp_AmrLink, MsIndex,pMapOopTo645->Oad, pMapOopTo645) > 0)
            {
                *DataLen = pMapOopTo645->TotalDataLen;
                break;
            }
            if((pMapOopTo645->Oad.nIndex !=0)||(FALSE == Check_Data_Struct(pMapOopTo645->CovertSruct)))
                offSet += pMapOopTo645->DataLen;

        }
        pMapOopTo645++;
        pAmrThreadArg->PatchMtrInfo[MsIndex].MapIndex ++;  //更新列表索引，供存储使用
    }
    TASK_FMT_TRACE(Pipe, REC_LOG,"offSet %d DataLen %d \n",offSet,*DataLen);
    
    if(pMapOopTo645->Oad.value==0xFFFFFFFF)
        return 0xFFFF;
    return offSet;
}
/*******************************************************************************
* 函数名称: CurvePatchDiAdapt
* 函数功能: 补抄抄读07表负荷曲线时，部分OAD若配置块抄，转换为单抄
*           会把每一条都找到并返回
* 输入参数: Tmp_AmrLink         方案任务节点
*           MsIndex             方案内电表索引
*           CurOAD              当前对照表中的OAD
* 输出参数: 无
* 返 回 值: <0                  没有需要转换的OAD
            >0                  有需要转换的OAD，允许抄读
*******************************************************************************/
int CurvePatchDiAdapt(uint8 Pipe,SCHEME_TASK_RUN_INFO* Tmp_AmrLink, int MsIndex, OOP_OAD_U OAD, const MapOopTo645_NEW* pMap)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[Pipe];
    uint16                   CSDIndex;

    CSDIndex = pAmrThreadArg->PatchMtrInfo[MsIndex].CSDIndex;

    if(Tmp_AmrLink->CsdTask.RoadTask[0].MainOad.nObjID != 0x5002)
        return -1;
    if(pMap == MapOopTo645_MinOne)
    {
        if(Tmp_AmrLink->CsdTask.RoadTask[0].OadListInfo[CSDIndex].value == 0x00100200
            || Tmp_AmrLink->CsdTask.RoadTask[0].OadListInfo[CSDIndex].value == 0x00200200
            || Tmp_AmrLink->CsdTask.RoadTask[0].OadListInfo[CSDIndex].value == 0x00300200
            || Tmp_AmrLink->CsdTask.RoadTask[0].OadListInfo[CSDIndex].value == 0x00400200
            || Tmp_AmrLink->CsdTask.RoadTask[0].OadListInfo[CSDIndex].value == 0x00500200
            || Tmp_AmrLink->CsdTask.RoadTask[0].OadListInfo[CSDIndex].value == 0x00600200
            || Tmp_AmrLink->CsdTask.RoadTask[0].OadListInfo[CSDIndex].value == 0x00700200
            || Tmp_AmrLink->CsdTask.RoadTask[0].OadListInfo[CSDIndex].value == 0x00800200
            || Tmp_AmrLink->CsdTask.RoadTask[0].OadListInfo[CSDIndex].value == 0x20000200
            || Tmp_AmrLink->CsdTask.RoadTask[0].OadListInfo[CSDIndex].value == 0x20010200
            || Tmp_AmrLink->CsdTask.RoadTask[0].OadListInfo[CSDIndex].value == 0x20040200
            || Tmp_AmrLink->CsdTask.RoadTask[0].OadListInfo[CSDIndex].value == 0x200A0200)
        {
            if(OAD.value == (Tmp_AmrLink->CsdTask.RoadTask[0].OadListInfo[CSDIndex].value+1))  //是否与总OAD相同
            {
                return 1;
            }
        }
    }
    else if(pMap == MapOopTo645_Min)
    {
       if(Tmp_AmrLink->CsdTask.RoadTask[0].OadListInfo[CSDIndex].value == 0x00100200
            || Tmp_AmrLink->CsdTask.RoadTask[0].OadListInfo[CSDIndex].value == 0x00200200
            || Tmp_AmrLink->CsdTask.RoadTask[0].OadListInfo[CSDIndex].value == 0x00300200
            || Tmp_AmrLink->CsdTask.RoadTask[0].OadListInfo[CSDIndex].value == 0x00400200
            || Tmp_AmrLink->CsdTask.RoadTask[0].OadListInfo[CSDIndex].value == 0x00500200
            || Tmp_AmrLink->CsdTask.RoadTask[0].OadListInfo[CSDIndex].value == 0x00600200
            || Tmp_AmrLink->CsdTask.RoadTask[0].OadListInfo[CSDIndex].value == 0x00700200
            || Tmp_AmrLink->CsdTask.RoadTask[0].OadListInfo[CSDIndex].value == 0x00800200
            || Tmp_AmrLink->CsdTask.RoadTask[0].OadListInfo[CSDIndex].value == 0x20000200
            || Tmp_AmrLink->CsdTask.RoadTask[0].OadListInfo[CSDIndex].value == 0x20010200
            || Tmp_AmrLink->CsdTask.RoadTask[0].OadListInfo[CSDIndex].value == 0x20040200
            || Tmp_AmrLink->CsdTask.RoadTask[0].OadListInfo[CSDIndex].value == 0x200A0200)
        {
            if(OAD.value == (Tmp_AmrLink->CsdTask.RoadTask[0].OadListInfo[CSDIndex].value+1))  //是否与总OAD相同
            {
                return 1;
            }
        }
    }
    return -1;
}

/*******************************************************************************
* 函数名称: taskmng_cache_buf_save
* 函数功能: 将数据缓存，带一块表的数据抄完后一起存储
* 输入参数: pn 信息点号
        mainoad 主oad
        suboad 子oad
        databuf 数据缓存
        datalen 数据长度
* 输出参数: 
* 返 回 值: 
*******************************************************************************/
void taskmng_cache_buf_save(uint8 pipe,uint16 pn,OOP_OAD_U mainoad,OOP_OAD_U suboad,uint8 *databuf,uint16 datalen,uint32 starttime,uint32   storeTm)
{
    AmrThreadArg_T *pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    OOP_OAD_U saveoad = suboad;
    DATA_CACHE_T *data =NULL;
    OadInfoMap pOadInfo;
    memset(&pOadInfo,0x00,sizeof(OadInfoMap));
    uint8 mergebuf[DATA_LEN_MAX]={0};
    uint16 mergelen=0;
    int index = -1;
    uint8 i;

    for(i = 0;i<PARALLEL_QUEUE_NUM;i++)
    {
        if(pAmrThreadArg->meterData[i].isValid ==1&&pAmrThreadArg->meterData[i].pn == pn)
        {
            index = i;
            break;
        }
    }
    //如果第一次写入 则添加一个
    if(index==-1)
    {
        for(i=0;i<PARALLEL_QUEUE_NUM;i++)
        {
            if(pAmrThreadArg->meterData[i].isValid ==0)
            {
                index = i;
                pAmrThreadArg->meterData[i].isValid = 1;
                pAmrThreadArg->meterData[i].pn = pn;
                pAmrThreadArg->meterData[i].mainOad = mainoad;
                pAmrThreadArg->meterData[i].starttime = starttime;
                pAmrThreadArg->meterData[i].storeTm = storeTm;
                break;
            }
        }
    }
    if(index == -1)
    {
        TASK_FMT_TRACE(pipe, REC_LOG,"找不到缓存队列\n");
        for(i=0;i<PARALLEL_QUEUE_NUM;i++)
        {
            TASK_FMT_TRACE(pipe, REC_LOG,"i %d Pn %d \n",i,pAmrThreadArg->meterData[i].pn);
        }
        return ;
    }
    data = &(pAmrThreadArg->meterData[index]);
    uint8 subnum = data->subnum;
    if(TRUE == taskmng_oad_info_get( suboad,&pOadInfo) &&(0x20210200==suboad.value || TRUE==taskmng_need_merge_check(&pOadInfo)))
    {
        saveoad.nIndex = 0;
        for(i=0;i<subnum;i++)
        {
            if(data->subOad[i].value == saveoad.value)
            {
                //TASK_FMT_TRACE ( pipe,REC_LOG, "merge oad is %8x",data->subOad[i].value);
                //找到上次写过的数据
                if(TRUE == RecordMerge(databuf, data->dataBuf[i].databuf,mergebuf,&mergelen,& pOadInfo))
                {
                    memcpy(data->dataBuf[i].databuf,mergebuf,mergelen);
                }
                return;
            }
        }
    }
    //新增的
    if(subnum>=JS_OOP_MAX_OADS)
    {
        TASK_FMT_TRACE(pipe, REC_LOG,"suboad too more \n");
        return ;
    }
    data->subOad[subnum] = saveoad;
    if(pOadInfo.CovertSruct == E_OOP_VOLTQR_T)
    {
        OOP_VOLTSTAT_T record;
        memset(&record,0x00,sizeof(OOP_VOLTSTAT_T));
        if(suboad.nIndex ==1)
        {
            memcpy(&record.day,databuf,datalen);
        }else
        {
            memcpy(&record.mon,databuf,datalen);
        }
        memcpy(data->dataBuf[subnum].databuf,&record,sizeof(OOP_VOLTSTAT_T));
        data->dataBuf[subnum].datalen = sizeof(OOP_VOLTSTAT_T);
    }else
    {
        memcpy(data->dataBuf[subnum].databuf,databuf,datalen);
        data->dataBuf[subnum].datalen = datalen;
    }
    //TASK_FMT_TRACE(pipe, REC_LOG,"缓存数据 oad %08x subnum %d\n",saveoad,subnum);
    //TASK_BUF_TRACE(pipe, REC_LOG,data->dataBuf[subnum].databuf,data->dataBuf[subnum].datalen);
    data->subnum++;
}


/*******************************************************************************
* 函数名称: taskmng_cache_buf_save
* 函数功能: 将数据缓存，带一块表的数据抄完后一起存储
* 输入参数: pn 信息点号
        mainoad 主oad
        suboad 子oad
        databuf 数据缓存
        datalen 数据长度
* 输出参数: 
* 返 回 值: 
*******************************************************************************/
void taskmng_rs485_cache_buf_save(uint8 pipe,uint16 pn,OOP_OAD_U mainoad,OOP_OAD_U suboad,uint8 *databuf,uint16 datalen,uint32 starttime,uint32   storeTm,DATA_CACHE_T  *meterData)
{
    OOP_OAD_U saveoad = suboad;
    DATA_CACHE_T *data =NULL;
    OadInfoMap pOadInfo;
    memset(&pOadInfo,0x00,sizeof(OadInfoMap));
    uint8 mergebuf[DATA_LEN_MAX]={0};
    uint16 mergelen=0;
    uint8 i;

    data = meterData;
    uint8 subnum = data->subnum;

    data->mainOad.value = mainoad.value;
    data->starttime = starttime;
    data->storeTm = storeTm;
    if(TRUE == taskmng_oad_info_get( suboad,&pOadInfo) &&(0x20210200==suboad.value || TRUE==taskmng_need_merge_check(&pOadInfo)))
    {
        saveoad.nIndex = 0;
        for(i=0;i<subnum;i++)
        {
            if(data->subOad[i].value == saveoad.value)
            {
                TASK_FMT_TRACE ( pipe,REC_LOG, "merge oad is %8x",data->subOad[i].value);
                //找到上次写过的数据
                if(TRUE == RecordMerge(databuf, data->dataBuf[i].databuf,mergebuf,&mergelen,& pOadInfo))
                {
                    memcpy(data->dataBuf[i].databuf,mergebuf,mergelen);
                }
                return;
            }
        }
    }
    
    //新增的
    if(subnum>=JS_OOP_MAX_OADS)
    {
        TASK_FMT_TRACE(pipe, REC_LOG,"suboad too more \n");
        return ;
    }
    data->subOad[subnum] = saveoad;
    if(pOadInfo.CovertSruct == E_OOP_VOLTQR_T)
    {
        OOP_VOLTSTAT_T record;
        memset(&record,0x00,sizeof(OOP_VOLTSTAT_T));
        if(suboad.nIndex ==1)
        {
            memcpy(&record.day,databuf,datalen);
        }else
        {
            memcpy(&record.mon,databuf,datalen);
        }
        memcpy(data->dataBuf[subnum].databuf,&record,sizeof(OOP_VOLTSTAT_T));
        data->dataBuf[subnum].datalen = sizeof(OOP_VOLTSTAT_T);
    }else
    {
        memcpy(data->dataBuf[subnum].databuf,databuf,datalen);
        data->dataBuf[subnum].datalen = datalen;
    }
    data->subnum++;
}

/*******************************************************************************
* 函数名称: taskmng_cache_buf_write_to_db
* 函数功能: 一块表的数据抄完后一起存储
* 输入参数: pn 信息点号
       
* 输出参数: 
* 返 回 值: 
*******************************************************************************/
void taskmng_cache_buf_write_to_db(uint8 pipe,uint16 Pn,uint8 logicid)
{
    uint16 i;
    int index = -1;
    OadInfoMap pOadInfo;
    memset(&pOadInfo,0x00,sizeof(OadInfoMap));
    AmrThreadArg_T *pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    for(i=0;i<PARALLEL_QUEUE_NUM;i++)
    {
        if(pAmrThreadArg->meterData[i].isValid ==1&&pAmrThreadArg->meterData[i].pn == Pn)
        {
            index = i;
            break;
        }
    }
    if(index == -1)
    {
        TASK_FMT_TRACE(pipe, REC_LOG,"未找到该表数据 Pn %d\n",Pn);
        return;
    }
    TASK_FMT_TRACE ( pipe,REC_LOG, "taskmng_cache_buf_write_to_db pn  %d\n",Pn);
    //组织存储报文
    DATA_CACHE_T *data = &(pAmrThreadArg->meterData[index]);
    WRITE_RECORDS_T record;
    uint32 storeno;
    int ret;
    uint8 addr[6]={0};
    memset(&record,0x00,sizeof(WRITE_RECORDS_T));
    record.recordOAD.logicId = logicid;
    record.recordOAD.infoNum = Pn;
    record.recordOAD.road.oadMain = data->mainOad;
    
    for(i=0;i<data->subnum;i++)
    {
        if(data->subOad[i].value == 0x40160200 || data->subOad[i].value == 0x40170200)//需要分合总，但是缓存中也没有总的数据，去db中查一下
        {
            TASK_FMT_TRACE ( pipe,REC_LOG, "DBmerge oad is %8x\n",data->subOad[i].value);
            OadStoreInfo StoreInfo= {0};
            uint8 addr[6]={0};

            taskmng_mtr_addr_get(Pn, 0, addr);
            memcpy(StoreInfo.MAC.nValue,addr,6);
            
            StoreInfo.recordOAD.infoNum=Pn;
            StoreInfo.recordOAD.logicId=0;

            StoreInfo.recordOAD.road.oadMain=data->mainOad;
            StoreInfo.recordOAD.road.oadCols.nNum=1;
            StoreInfo.recordOAD.road.oadCols.oad[0]=data->subOad[i];
            StoreInfo.recordOAD.road.oadCols.rsv=0;

            StoreInfo.recordOAD.classtag = 5;
            StoreInfo.colStartTm=data->starttime;
            StoreInfo.colStoreTm=data->storeTm;
            StoreInfo.colEndTm=time(NULL);
            taskmng_leak_data_save(data->dataBuf[i].databuf,data->dataBuf[i].datalen,StoreInfo,pipe,1);
            //TASK_FMT_DEBUG(pipe,REC_LOG,"OopMeterAmrData OAD %08x \n",data->subOad[i].value);
            //TASK_BUF_DEBUG(pipe,REC_LOG,data->dataBuf[i].databuf,data->dataBuf[i].datalen);
        }
        record.recordOAD.road.oadCols.oad[i] = data->subOad[i];
        record.datas[i].len = data->dataBuf[i].datalen;
        memcpy(record.datas[i].data,data->dataBuf[i].databuf,data->dataBuf[i].datalen);

        //TASK_FMT_DEBUG(pipe,REC_LOG,"OopMeterAmrData OAD %08x \n",data->subOad[i].value);
        //TASK_BUF_DEBUG(pipe,REC_LOG,data->dataBuf[i].databuf,data->dataBuf[i].datalen);
        //加点打印 统计下时钟超差的表
        if(data->mainOad.value ==0 && data->subOad[i].value == 0x40000200)
        {
            OOP_DATETIME_S_T datetimes;
            time_t tmptime = 0;
            memset(&datetimes,0,sizeof(datetimes));
            memcpy(&datetimes,data->dataBuf[i].databuf,data->dataBuf[i].datalen);
            DT_DateTimeShort2Time(&datetimes,&tmptime);
            if(abs(time(NULL) - tmptime) > 300)
            {
                taskmng_time_error_num_add();
                TASK_FMT_DEBUG(pipe,REC_LOG,"电表 %d 时钟超差 总个数 %d \n",Pn,taskmng_time_error_num_get());
            }
        }
    }
    record.recordOAD.road.oadCols.nNum = i;
    record.recordOAD.classtag = 5;
    record.colStartTm = data->starttime;
    record.colStoreTm = data->storeTm;
    record.colEndTm = time(NULL);
#if IS_NEED_ADD_8HOUR
    record.colStoreTm+=8*60*60;
    record.colStartTm+=8*60*60;
    record.colEndTm+=8*60*60;
#endif

#ifdef PRODUCT_CCU
    if(patch_policy_flg[pipe] == 1)
    {
        time_t  RealTime_t ;
        //系统当前时间
        RealTime_t = time(NULL);
        OOP_DATETIME_S_T    curTime = {0};
        DT_Time2DateTimeShort(RealTime_t, &curTime);

        if(curTime.minute < 22 && (record.colEndTm - record.colStartTm) > 70)
            record.colEndTm-=70;
        else if (curTime.minute >= 22 && (record.colEndTm - record.colStartTm) > 40)
            record.colEndTm-=40;
    }
#endif

#ifdef ANHUI
    if(Pn>=10000)
    {
        GetVirtualPipeAddr(Pn,addr);
    }
    else
    {
        taskmng_mtr_addr_get( Pn, logicid,addr);
    }
#else
    taskmng_mtr_addr_get( Pn, logicid,addr);
#endif
    record.MAC.nNum = 6;
    memcpy_r(record.MAC.value,addr,6);
    ret = db_write_records(gAmrThreadArg[pipe].DBClientHandle,&record,&storeno);
    if(ret!=0)
    {
        TASK_FMT_TRACE ( pipe,REC_LOG, "taskmng_cache_buf_write_to_db error ret  %d\n",ret);
    }
    TASK_FMT_TRACE ( pipe,REC_LOG, "taskmng_cache_buf_write_to_db pn  ok\n",Pn);
    //上报完清空
    memset(data,0x00,sizeof(DATA_CACHE_T));
}


/*******************************************************************************
* 函数名称: taskmng_cache_buf_write_to_db
* 函数功能: 一块表的数据抄完后一起存储
* 输入参数: pn 信息点号
       
* 输出参数: 
* 返 回 值: 
*******************************************************************************/
void taskmng_rs485_cache_buf_write_to_db(uint8 pipe,uint16 Pn,uint8 logicid,DATA_CACHE_T *meterData)
{
    uint16 i;
    OadInfoMap pOadInfo;
    memset(&pOadInfo,0x00,sizeof(OadInfoMap));
    
    TASK_FMT_TRACE ( pipe,REC_LOG, "taskmng_cache_buf_write_to_db pn  %d\n",Pn);
    //组织存储报文
    DATA_CACHE_T *data = meterData;
    WRITE_RECORDS_T record;
    uint32 storeno;
    int ret;
    uint8 addr[6]={0};
    memset(&record,0x00,sizeof(WRITE_RECORDS_T));
    record.recordOAD.logicId = logicid;
    record.recordOAD.infoNum = Pn;
    record.recordOAD.road.oadMain = data->mainOad;
    
    for(i=0;i<data->subnum;i++)
    {
        if(data->subOad[i].value == 0x40160200 || data->subOad[i].value == 0x40170200)//需要分合总，但是缓存中也没有总的数据，去db中查一下
        {
            TASK_FMT_TRACE ( pipe,REC_LOG, "DBmerge oad is %8x\n",data->subOad[i].value);
            OadStoreInfo StoreInfo= {0};
            uint8 addr[6]={0};

            taskmng_mtr_addr_get(Pn, 0, addr);
            memcpy(StoreInfo.MAC.nValue,addr,6);
            
            StoreInfo.recordOAD.infoNum=Pn;
            StoreInfo.recordOAD.logicId=0;

            StoreInfo.recordOAD.road.oadMain=data->mainOad;
            StoreInfo.recordOAD.road.oadCols.nNum=1;
            StoreInfo.recordOAD.road.oadCols.oad[0]=data->subOad[i];
            StoreInfo.recordOAD.road.oadCols.rsv=0;

            StoreInfo.recordOAD.classtag = 5;
            StoreInfo.colStartTm=data->starttime;
            StoreInfo.colStoreTm=data->storeTm;
            StoreInfo.colEndTm=time(NULL);
            taskmng_leak_data_save(data->dataBuf[i].databuf,data->dataBuf[i].datalen,StoreInfo,pipe,1);
            //TASK_FMT_DEBUG(pipe,REC_LOG,"OopMeterAmrData OAD %08x \n",data->subOad[i].value);
            //TASK_BUF_DEBUG(pipe,REC_LOG,data->dataBuf[i].databuf,data->dataBuf[i].datalen);
        }
        record.recordOAD.road.oadCols.oad[i] = data->subOad[i];
        record.datas[i].len = data->dataBuf[i].datalen;
        memcpy(record.datas[i].data,data->dataBuf[i].databuf,data->dataBuf[i].datalen);
        //加点打印 统计下时钟超差的表
        if(data->mainOad.value ==0 && data->subOad[i].value == 0x40000200)
        {
            OOP_DATETIME_S_T datetimes;
            time_t tmptime = 0;
            memset(&datetimes,0,sizeof(datetimes));
            memcpy(&datetimes,data->dataBuf[i].databuf,data->dataBuf[i].datalen);
            DT_DateTimeShort2Time(&datetimes,&tmptime);
            if(abs(time(NULL) - tmptime) > 300)
            {
                TASK_FMT_DEBUG(pipe,REC_LOG,"电表 %d 时钟超差 \n",Pn);
            }
        }
    }
    record.recordOAD.road.oadCols.nNum = i;
    record.recordOAD.classtag = 5;
    record.colStartTm = data->starttime;
    record.colStoreTm = data->storeTm;
    record.colEndTm = time(NULL);
#if IS_NEED_ADD_8HOUR
    record.colStoreTm+=8*60*60;
    record.colStartTm+=8*60*60;
    record.colEndTm+=8*60*60;
#endif

#ifndef PRODUCT_CCU
    if(patch_policy_flg[pipe] == 1)
    {
        time_t  RealTime_t ;
        //系统当前时间
        RealTime_t = time(NULL);
        OOP_DATETIME_S_T    curTime = {0};
        DT_Time2DateTimeShort(RealTime_t, &curTime);

#ifdef PRODUCT_ECU
        if(curTime.minute < 16 && (record.colEndTm - record.colStartTm) > 120)
            record.colEndTm-=120;
#else
        if(curTime.minute < 16 && (record.colEndTm - record.colStartTm) > 70)
            record.colEndTm-=70;
#endif
        else if (curTime.minute >= 16 && (record.colEndTm - record.colStartTm) > 10)
            record.colEndTm-=10;
    }
#endif
    TASK_FMT_TRACE(pipe, REC_LOG, "record.colStoreTm is %d\n", record.colStoreTm);
    DateTimeBcd_t       tmpTime;
    DT_Time2DateTimeBCD(record.colStoreTm, &tmpTime);

    TASK_FMT_TRACE(pipe, REC_LOG,"taskStoreTime:%02x%02x-%02x-%02x %02x:%02x:%02x\n", tmpTime.year_h, tmpTime.year_l,
        tmpTime.month, tmpTime.day, tmpTime.hour, tmpTime.min, tmpTime.sec);

#ifdef ANHUI
    if(Pn>=10000)
    {
        GetVirtualPipeAddr(Pn,addr);
    }
    else
    {
        taskmng_mtr_addr_get( Pn, logicid,addr);
    }
#else
    taskmng_mtr_addr_get( Pn, logicid,addr);
#endif
    record.MAC.nNum = 6;
    memcpy_r(record.MAC.value,addr,6);
    ret = db_write_records(gAmrThreadArg[pipe].DBClientHandle,&record,&storeno);
    if(ret!=0)
    {
        TASK_FMT_TRACE ( pipe,REC_LOG, "taskmng_cache_buf_write_to_db error ret  %d\n",ret);
    }
    TASK_FMT_TRACE ( pipe,REC_LOG, "taskmng_cache_buf_write_to_db pn  ok\n",Pn);
    //上报完清空
    memset(data,0x00,sizeof(DATA_CACHE_T));
}

/*******************************************************************************
* 函数名称: taskmng_patch_cache_buf_save
* 函数功能: 将数据缓存，带一块表的数据抄完后一起存储 补抄用
* 输入参数: pn 信息点号
        mainoad 主oad
        suboad 子oad
        databuf 数据缓存
        datalen 数据长度
* 输出参数: 
* 返 回 值: 
*******************************************************************************/
void taskmng_patch_cache_buf_save(uint8 pipe,uint16 pn,OOP_OAD_U mainoad,OOP_OAD_U suboad,uint8 *databuf,uint16 datalen,uint32 starttime,uint32   storeTm)
{
    AmrThreadArg_T *pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    OOP_OAD_U saveoad = suboad;
    DATA_CACHE_T *data =NULL;
    OadInfoMap pOadInfo;
    memset(&pOadInfo,0x00,sizeof(OadInfoMap));
    uint8 mergebuf[DATA_LEN_MAX]={0};
    uint16 mergelen=0;
    int index = -1;
    uint8 i;
    for(i = 0;i<PARALLEL_QUEUE_NUM;i++)
    {
        if(pAmrThreadArg->patchMeterData[i].isValid ==1&&pAmrThreadArg->patchMeterData[i].pn == pn)
        {
            index = i;
            break;
        }
    }
    //如果第一次写入 则添加一个
    if(index==-1)
    {
        for(i=0;i<PARALLEL_QUEUE_NUM;i++)
        {
            if(pAmrThreadArg->patchMeterData[i].isValid ==0)
            {
                index = i;
                pAmrThreadArg->patchMeterData[i].isValid = 1;
                pAmrThreadArg->patchMeterData[i].pn = pn;
                pAmrThreadArg->patchMeterData[i].mainOad = mainoad;
                pAmrThreadArg->patchMeterData[i].starttime = starttime;
                pAmrThreadArg->patchMeterData[i].storeTm = storeTm;
                break;
            }
        }
    }
    if(index == -1)
    {
        TASK_FMT_TRACE(pipe, REC_LOG,"找不到缓存队列\n");
        return ;
    }
    data = &(pAmrThreadArg->patchMeterData[index]);
    uint8 subnum = data->subnum;
    if(TRUE == taskmng_oad_info_get( suboad,&pOadInfo) &&(0x20210200==suboad.value || TRUE==taskmng_need_merge_check(&pOadInfo)))
    {
        saveoad.nIndex = 0;
        for(i=0;i<subnum;i++)
        {
            if(data->subOad[i].value == saveoad.value)
            {
                //找到上次写过的数据
                if(TRUE == RecordMerge(databuf, data->dataBuf[i].databuf,mergebuf,&mergelen,& pOadInfo))
                {
                    memcpy(data->dataBuf[i].databuf,mergebuf,mergelen);
                }
                return;
            }
        }

    }
    //新增的
    if(subnum>=OOP_MAX_OADS)
    {
        TASK_FMT_TRACE(pipe, REC_LOG,"suboad too more \n");
        return ;
    }
    data->subOad[subnum] = saveoad;
    if(pOadInfo.CovertSruct == E_OOP_VOLTQR_T)
    {
        OOP_VOLTSTAT_T record;
        memset(&record,0x00,sizeof(OOP_VOLTSTAT_T));
        if(suboad.nIndex ==1)
        {
            memcpy(&record.day,databuf,datalen);
        }else
        {
            memcpy(&record.mon,databuf,datalen);
        }
        memcpy(data->dataBuf[subnum].databuf,&record,sizeof(OOP_VOLTSTAT_T));
        data->dataBuf[subnum].datalen = sizeof(OOP_VOLTSTAT_T);
    }else
    {
        memcpy(data->dataBuf[subnum].databuf,databuf,datalen);
        data->dataBuf[subnum].datalen = datalen;
    }
    TASK_FMT_TRACE(pipe, REC_LOG,"缓存数据 oad %08x subnum %d\n",saveoad,subnum);
    TASK_BUF_TRACE(pipe, REC_LOG,data->dataBuf[subnum].databuf,data->dataBuf[subnum].datalen);
    data->subnum++;
}

/*******************************************************************************
* 函数名称: taskmng_patch_cache_buf_write_to_db
* 函数功能: 一块表的数据抄完后一起存储 补抄
* 输入参数: pn 信息点号
       
* 输出参数: 
* 返 回 值: 
*******************************************************************************/
void taskmng_patch_cache_buf_write_to_db(uint8 pipe,uint16 Pn,uint8 logicid)
{
    uint16 i;
    int index = -1;
    AmrThreadArg_T *pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    for(i=0;i<PARALLEL_QUEUE_NUM;i++)
    {
        if(pAmrThreadArg->patchMeterData[i].isValid ==1&&pAmrThreadArg->patchMeterData[i].pn == Pn)
        {
            index = i;
            break;
        }
    }
    if(index == -1)
    {
        TASK_FMT_TRACE(pipe, REC_LOG,"未找到该表数据 Pn %d\n",Pn);
        return;
    }
    TASK_FMT_TRACE ( pipe,REC_LOG, "taskmng_cache_buf_write_to_db pn  %d\n",Pn);
    //组织存储报文
    DATA_CACHE_T *data = &(pAmrThreadArg->patchMeterData[index]);
    WRITE_RECORDS_T record;
    uint32 storeno;
    int ret;
    uint8 addr[6]={0};
    memset(&record,0x00,sizeof(WRITE_RECORDS_T));
    record.recordOAD.logicId = logicid;
    record.recordOAD.infoNum = Pn;
    record.recordOAD.road.oadMain = data->mainOad;
    record.recordOAD.road.oadCols.nNum = data->subnum;
    for(i=0;i<data->subnum;i++)
    {
        record.recordOAD.road.oadCols.oad[i] = data->subOad[i];
        record.datas[i].len = data->dataBuf[i].datalen;
        memcpy(record.datas[i].data,data->dataBuf[i].databuf,data->dataBuf[i].datalen);
    }
    record.recordOAD.classtag = 5;
    record.colStartTm = data->starttime;
    record.colStoreTm = data->storeTm;
    record.colEndTm = time(NULL);
#if IS_NEED_ADD_8HOUR
    record.colStoreTm+=8*60*60;
    record.colStartTm+=8*60*60;
    record.colEndTm+=8*60*60;
#endif
    taskmng_mtr_addr_get( Pn, logicid,addr);
    record.MAC.nNum = 6;
    memcpy_r(record.MAC.value,addr,6);
    ret = db_write_records(gAmrThreadArg[pipe].DBClientHandle,&record,&storeno);
    if(ret!=0)
    {
        TASK_FMT_TRACE ( pipe,REC_LOG, "taskmng_patch_cache_buf_write_to_db error ret  %d\n",ret);
    }
    TASK_FMT_TRACE ( pipe,REC_LOG, "taskmng_patch_cache_buf_write_to_db pn  ok\n",Pn);
    //上报完清空
    memset(data,0x00,sizeof(DATA_CACHE_T));
}



