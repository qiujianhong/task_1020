

#include "CtsAll.h"
#include "devinfo.h"
#include "area.h"
#include "CtsACMeter.h"
#include "CtsPlc.h"


PastTimeFlag_T             gPastTimeFlag[AMR_THREAD_NUM];       //过时间标志 最后一个给任务管理线程用
DateTime_T                 gCurBinTime[AMR_THREAD_NUM];         //当前BIN时间
DateTime_T                 gCurBcdTime[AMR_THREAD_NUM];         //当前BCD时间
DateTime_T                 gPreBinTime[AMR_THREAD_NUM];         //上次BIN时间
uint8                   gPreDI[MTR_MAX_NUM][4];          //上次数据标识

PastTimeFlag_T             gPastTimeFlagTask;                           //过时间标志
DateTime_T                 gCurBinTimeTask;                             //当前BIN时间
DateTime_T                 gCurBcdTimeTask;                             //当前BCD时间
DateTime_T                 gPreBinTimeTask;                             //上次BIN时间
uint16                          MsgSeq = 0;      //消息index 各线程共用
//MODULE_INFO_T              gModuleInfo[5];      //5个模组的信息

#define MONITOR_PARALL_FUN  1      //电表事件上报的时候透传电表数据是否用并发抄表的方式   1表示用003E并发抄表 0表示还是用0038透传


extern RS485_RS232_PARA Rs485ComNo[8];


uint8 gHPLCRouterState[PLC_NUM] = {0};  //路由完成标识 0表示未完成 不能抄表 1表示路由学习完成 可以抄表
uint8 gHPLCSearchState[PLC_NUM] = {0};  //搜表完成标识 0表示未完成 不能抄表 1表示搜表完成，用于江苏


uint8 HPLCChannel;          //宽带载波频段序号F2090900

/*台区识别启停标记启动标志：F2090B00
bit0：台区识别使能标志（0：禁止；1：允许）
bit1：台区改切使能标志（0：禁止；1：允许）
bit2：离网感知使能标志（0：禁止；1：允许）
bit3~bit7：备用。
*/
uint8 HPLCAcrossAreaFlag;
uint8 HPLCNodeRejectFlag;   //拒绝节点上报使能标志F2091600

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
* 函数名称: GetMsgSeq
* 函数功能: 获取消息序号
* 输入参数:
* 输出参数:
* 返 回 值: 消息序号
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

    //存储时标放到外面处理
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
        TASK_FMT_TRACE( -1,TG_LOG," 存储总加组数据失败 \n");
    }
    return;

}


/*******************************************************************************
* 函数名称: taskmng_time_manage_task
* 函数功能: 任务管理线程的时间管理函数
* 输入参数:
* 输出参数:
* 返 回 值:
*******************************************************************************/
void taskmng_time_manage_task()
{
    gPastTimeFlagTask.Word = 0;                           //清除时间标志

    taskmng_cur_bin_time_get(&gCurBinTimeTask);                //刷新当前HEX码时间
    taskmng_cur_bcd_time_get(&gCurBcdTimeTask);              //刷新当前BCD码时间


    //过月
    if (gCurBinTimeTask.Month != gPreBinTimeTask.Month)
    {
        //同时产生过月、过日、过时、过分、过秒标志
        gPastTimeFlagTask.BitSect.Month = TRUE;
        gPastTimeFlagTask.BitSect.Day = TRUE;
        gPastTimeFlagTask.BitSect.Hour = TRUE;
        gPastTimeFlagTask.BitSect.Minute = TRUE;
        gPastTimeFlagTask.BitSect.Second = TRUE;
    }
    //过日
    if (gCurBinTimeTask.Day != gPreBinTimeTask.Day)
    {
        //同时产生过日、过时、过分标志
        gPastTimeFlagTask.BitSect.Day = TRUE;
        gPastTimeFlagTask.BitSect.Hour = TRUE;
        gPastTimeFlagTask.BitSect.Minute = TRUE;
        gPastTimeFlagTask.BitSect.Second = TRUE;
    }
    //过时
    else if (gCurBinTimeTask.Hour != gPreBinTimeTask.Hour)
    {
        //同时产生过时、过分标志
        gPastTimeFlagTask.BitSect.Hour = TRUE;
        gPastTimeFlagTask.BitSect.Minute = TRUE;
        gPastTimeFlagTask.BitSect.Second = TRUE;
    }
    //过分
    else if (gCurBinTimeTask.Minute != gPreBinTimeTask.Minute)
    {
        //产生过分标志
        gPastTimeFlagTask.BitSect.Minute = TRUE;
        gPastTimeFlagTask.BitSect.Second = TRUE;
        #ifdef AREA_JIANGXI
        if(gCurBinTimeTask.Minute%15==0)
        {
            WriteLoadRate(gDBClientTask);
        }
        #endif
    }

    //过秒
    else if (gCurBinTimeTask.Second != gPreBinTimeTask.Second)
    {
        gPastTimeFlagTask.BitSect.Second = TRUE;
    }
    else
    {
        usleep(10000);
    }


    //过日处理
    if(gPastTimeFlagTask.BitSect.Day == TRUE)
    {

    }

    //过时处理
    if(gPastTimeFlagTask.BitSect.Hour == TRUE)
    {
    }

    if(gPastTimeFlagTask.BitSect.Second == TRUE)
    {


    }

    //过分处理
    if(gPastTimeFlagTask.BitSect.Minute == TRUE)
    {

    }

    memcpy(&gPreBinTimeTask, &gCurBinTimeTask, sizeof(DateTime_T));;

}


/*******************************************************************************
* 函数名称: GetMtrPipe
* 函数功能: 根据电表oad转换为端口号 0 1对应载波 2到9对应8路485
* 输入参数:
            oad          //档案端口参数


* 输出参数: 对应内部端口
* 返 回 值: 抄读电表数量
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
    else if(oad->nObjID == 0xF208 && oad->attID == 0x02) //交采加上容错 0200也支持
    {
        return AMR_THREAD_NUM; //交采用表示
    }
#endif
    else if(oad->value == 0xF20B0201)
    {
        return 1; //蓝牙用1表示
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
    else if((oad->nObjID == 0xF2E0 || oad->nObjID == 0xF208) && oad->attID == 0x02) //交采加上容错 0200也支持
    {
        return AMR_THREAD_NUM-1; //交采用10表示
    }

#endif
    return 0xff;
}


/*******************************************************************************
* 函数名称: taskmng_pipe_oad_get
* 函数功能: 将程序内部使用的端口号转换为698端口号
* 输入参数: pipe 程序内部使用的端口号
* 输出参数:

* 返 回 值: 698端口号
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
* 函数功能: 判断Addr1是否和Addr2匹配
* 输入参数:       Addr1          //可能带通配符的地址
            Addr2             //档案中的电表地址
* 输出参数: 无
* 返 回 值: TRUE          //匹配
         FALSE        //不匹配
*******************************************************************************/
BOOL IsMtrAddrMatch(OOP_TSA_T *Addr1,OOP_TSA_T *Addr2)
{
    uint8 i;
    char str1[3]={0};
    char str2[3]={0};
    if(Addr1->len != Addr2->len)
        return FALSE;
    //如果逻辑地址不相同 直接不匹配
    if(Addr1->oldvxd!=Addr2->oldvxd || Addr1->vxd != Addr2->vxd)
        return FALSE;
    //对比下逻辑地址
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
* 函数功能: 判断新增的电表是否存在当前列表中
* 输入参数: MetSn                   //电表序号
            MsNum                   //表列表数目
            pMsInfo                 //抄读电表信息
* 输出参数: 无
* 返 回 值: 0      //重复
            1      //不重复
*******************************************************************************/
int IsMtrSnInList(uint16 MtrSn,uint16 MsNum,MS_INFO *pMsInfo)
{
    uint16 i;

    for(i = 0; i < MsNum; i++)
    {
        if(MtrSn == pMsInfo[i].Sn)//判断序列号是否一致
            return 1;
    }

    return 0;
}
/*******************************************************************************
* 函数功能: 对TSA的地址进行加减操作
* 输入参数: TsaAddr                   //TSA地址
            Fun                       //功能
* 输出参数: 无
* 返 回 值: 0      //重复
            1      //不重复
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
名称 GetTaskDelay
功能 获取任务延时 没有加锁
参数  无
返回值:任务延时 单位秒
***********************************************************************/
uint32 GetTaskDelay(uint16 TaskId,uint8 logicid)
{
    uint32         dwPeriod;
    int            i;

    //获取任务执行周期，根据日冻结成功率计算允许执行时间
    for(i=0; i<SchemeTaskCfgList.TaskNum; i++)
    {
        if(SchemeTaskCfgList.OopTaskCfg[i].TaskId == TaskId &&SchemeTaskCfgList.OopTaskCfg[i].logicid == logicid )
            break;
    }
    if(i == SchemeTaskCfgList.TaskNum)
    {
        return 0;   //容错
    }

    dwPeriod = SchemeTaskCfgList.OopTaskCfg[i].DelayTime;
    switch (SchemeTaskCfgList.OopTaskCfg[i].DelayTimeUnit)  //延时单位
    {
    case TASK_CYCLE_FLAG_SEC:       //秒
        break;

    case TASK_CYCLE_FLAG_MIN:       //分钟
        dwPeriod *= 60L;
        break;

    case TASK_CYCLE_FLAG_HOUR:      //小时
        dwPeriod *= 3600L;
        break;

    case TASK_CYCLE_FLAG_DAY:       //日
    case TASK_CYCLE_FLAG_MON:       //月
    case TASK_CYCLE_FLAG_YEAR:      //月
        dwPeriod = 86400L;
        break;

    default:
        return 0;
        break;
    }
    return dwPeriod;
}


/*******************************************************************************
* 函数名称: taskmng_past_time_deal_init
* 函数功能: 时间过处理初始化
* 输入参数: 无
* 输出参数: 无
* 返 回 值: 无
*******************************************************************************/
void taskmng_past_time_deal_init ( uint8 CommPipe )
{
    taskmng_cur_bin_time_get ( &gCurBinTime[CommPipe] );
    memcpy ( &gPreBinTime[CommPipe], &gCurBinTime[CommPipe], sizeof ( DateTime_T ) );
}
/*******************************************************************************
* 函数名称: taskmng_all_state_init
* 函数功能: 状态字初始化
* 输入参数: 无
* 输出参数: 无
* 返 回 值: 无
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
    /* 不能设置非阻塞 cpu占用率太高 */
    /* task_msg_nonblocking_set(pAmrThreadArg->MsgSock); */

}

/*******************************************************************************
* 函数名称: taskmng_plc_module_check
* 函数功能: 查询对应端口是否插入载波模块
* 输入参数:
* 输出参数: 无
* 返 回 值:  返回0 为有模块， -1为没有
*******************************************************************************/
int taskmng_plc_module_check(uint8 pipe)
{

        return 0;

}


/*********************************
函数功能：    去除重复任务函数
函数原型：    Remove_RepeatPlcTask
函数参数：
              TaskName            //增加的任务名称
函数返回值：
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
    //TASK_FMT_TRACE ( CommPipe,COMM_LOG, "当前运行任务 %d PlcTaskDeal_Stat %d 新增任务 %d \n",PlcTask_RunLink->TaskName,PlcTask_RunLink->PlcTaskDeal_Stat,TaskName );
    if ( PlcTask_RunLink->TaskName == TaskName ) //如果此次新增的任务与当前正在执行的任务一致
    {
        switch ( PlcTask_RunLink->PlcTaskDeal_Stat )
        {
            case TASK_INIT:
            case ROUTERCTRL_DEAL:
            case ROUTERCTRL_DELAY:
                return -1;
                break;
            case FUN_DEAL:
                if ( TaskName == MS_FILE_CMP||TaskName == MS_NORMAL_AMR ) //如果是日常抄表
                {
                    if ( PlcTask_RunLink->gInnerStat != NULL )
                    {
                        PlcTask_RunLink->gInnerStat = 0;//重头执行
                    }

                    if ( TaskName == MS_NORMAL_AMR ) //如果是日常抄读任务需要重新执行路由操作
                    {
                        //PlcTask_RunLink->PlcTaskDeal_Stat = TASK_INIT;//重新执行
                    }
                }
                else//其他任务不需要重新启动
                {
                    return -1;
                }
                break;

            case ROUTERRECOVER_DELAY:
            case ROUTERRECOVER_DEAL:
                PlcTask_RunLink->PlcTaskDeal_Stat = FUN_DEAL;//重新执行
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
                if ( pTmp->RoutrtCtrl == RCS_RESTART ) //如果此时有产生日常抄读任务那么路由需要再次重启
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
函数功能：      优先级法调度将进程插入到就绪队列算法
函数原型：      InsertPlcTaskQueue
函数参数：      PLCTASK_QUEUE *PlcAddTask         //需要插入的载波任务
函数返回值：
*********************************/
void InsertPlcTaskQueue ( uint8 CommPipe,PLCTASK_QUEUE* PlcAddTask )
{
    PLCTASK_QUEUE*  pTmpLink;
    PLCTASK_QUEUE*  PlcTask_RunLink = NULL;

    PlcTask_RunLink = gAmrThreadArg[CommPipe].PlcTask_RunLink;

    if ( PlcTask_RunLink == NULL ) //如果当前任务队列为空
    {
        PlcTask_RunLink = PlcAddTask;
        PlcAddTask->next = NULL;
        gAmrThreadArg[CommPipe].PlcTask_RunLink = PlcTask_RunLink;
        return ; //添加成功
    }

    if ( PlcAddTask->Priority == PlcTask_RunLink->Priority ) //如果任务优先级相同那么直接置于当前任务队列后
    {

        pTmpLink              = PlcTask_RunLink->next;
        PlcTask_RunLink->next = PlcAddTask;
        PlcAddTask->next      = pTmpLink;
        gAmrThreadArg[CommPipe].PlcTask_RunLink = PlcTask_RunLink;
        return;
    }
    else if ( PlcAddTask->Priority < PlcTask_RunLink->Priority ) //新任务优先级大于当前任务的优先级
    {

        if ( PlcTask_RunLink->PlcTaskDeal_Stat == ROUTERRECOVER_DELAY
                ||PlcTask_RunLink->PlcTaskDeal_Stat == ROUTERRECOVER_DEAL ) //如果当前任务处于恢复或者恢复执行过程时
        {
            PlcTask_RunLink->next = SortPlcTaskQueue ( PlcTask_RunLink->next,PlcAddTask );
            gAmrThreadArg[CommPipe].PlcTask_RunLink = PlcTask_RunLink;
            return;
        }
        else
        {
            if ( PlcTask_RunLink->PlcTaskDeal_Stat == FUN_DEAL
                    || PlcTask_RunLink->PlcTaskDeal_Stat == ROUTERCTRL_DELAY ) //如果当前是处于功能函数执行
            {

                if ( PlcTask_RunLink->RoutrtCtrl == PlcAddTask->RoutrtCtrl )
                {
                    PlcAddTask->PlcTaskDeal_Stat = ROUTERCTRL_DELAY;//COMM_DEBUG直接进行延时处理无须再进行路由操作
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
            //复位当前任务,重新执行

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
函数功能：      载波任务优先级排序
函数原型：      SortPlcTaskQueue
函数参数：
                PLCTASK_QUEUE *pHeadTaskLink      //任务链表的头队列
                PLCTASK_QUEUE *PlcAddTask         //需要插入的载波任务

函数返回值：
*********************************/
PLCTASK_QUEUE* SortPlcTaskQueue ( PLCTASK_QUEUE* pHeadTaskLink,PLCTASK_QUEUE* PlcAddTask )
{
    PLCTASK_QUEUE* PTmpHeadTaskLink= pHeadTaskLink;//轮询链表时使用
    PLCTASK_QUEUE* ScollLink = NULL;//轮询链表时使用

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
        else//优先级比此任务优先级低
        {
            break;
        }

    }


    PlcAddTask->next = PTmpHeadTaskLink;
    ScollLink->next = PlcAddTask;

    return pHeadTaskLink;
}

/*******************************************************************************
* 函数名称:         FindNextTaskPrio
* 函数功能:         获取链表相同优先级的最后一个任务
* 输入参数:         PLCTASK_QUEUE *PlcTaskQueue //任务队列
                    QueueNum                    //任务队列长度
* 输出参数:         int
* 返 回 值:         > 0  添加成功
                    < 0  添加失败

*******************************************************************************/
void RestPlcTaskStat ( PLCTASK_QUEUE* PlcCurTask )
{

    if ( ( PlcCurTask->gInnerStat ) != NULL )
    {
        * ( uint8* ) ( PlcCurTask->gInnerStat )  = 0;
    }

    if ( PlcCurTask->RoutrtCtrl != RCS_RESTART )
    {
        PlcCurTask->PlcTaskDeal_Stat = TASK_INIT;    //重新执行
    }

}

/*******************************************************************************
* 函数名称: taskmng_frame_3762_unpack
* 函数功能: 解3762帧
* 输入参数: FrameLen            帧长度
*           FrameBuff           帧数据
* 输出参数: AppData             应用层数据
* 返 回 值: CHECK_RESULT_SUCCEED
*           CHECK_RESULT_FAILUER
*           CHECK_RESULT_CONFIRM
*           CHECK_RESULT_DENY
*******************************************************************************/
int8 taskmng_frame_3762_unpack(uint16 FrameLen, uint8 *FrameBuff, AppData_T *AppData)
{

    uint16  FrameDataLen=0;                     //帧数据总长度
    uint16  DataIndex = 0;                      //指针偏移
    int     i = 0;
    uint8   CheckSum = 0;  //校验和

    //计算帧数据长度, 偏移到信息域R
    memcpy(&FrameDataLen, &FrameBuff[1], 2);
    DataIndex = 4;

    //判断报文长度是否合法
    if(FrameDataLen > FrameLen)
    {

        return CHECK_RESULT_FAILUER;
    }


    //解析信息域, 偏移到地址域
    memcpy(AppData->RInfo, &FrameBuff[DataIndex], 6);
    DataIndex += 6;

    //判断通讯模块标识，偏移到应用数据域
    if (AppData->RInfo[0]& 0x04)  //存在地址域
    {
        //偏移掉A1和A3，从节点上行无A2
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


    //校验和检查

    for(i=3; i<FrameDataLen-2; i++)
    {
        CheckSum += FrameBuff[i];
    }

    if(CheckSum != FrameBuff[FrameDataLen-2])
    {
        return CHECK_RESULT_FAILUER;
    }

    //帧尾检查
    if(FrameBuff[FrameDataLen-1] != 0x16)
    {
        return CHECK_RESULT_FAILUER;
    }


    if (AppData->AFN == 0x00)
    {
        if (AppData->Fn == 1)                   //确认
        {
            return CHECK_RESULT_CONFIRM;
        }
        else if (AppData->Fn == 2)              //否认
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
* 函数名称: ClearCommCbRecvBuff
* 函数功能: 清除抄表接收缓冲数据
* 输入参数: 无
* 输出参数: 无
* 返 回 值:  无
*******************************************************************************/
void ClearCommCbRecvBuff(uint8 CommPipe)
{
    int     RecvLen = 0xff;                     //接收数据长度
    uint8   RecvBuf[MAX_MSG_LEN];        //接收数据缓冲区

    while (RecvLen > 0)
    {
        RecvLen = taskmng_plc_queue_read(pAmrRecvDown_Cb[CommPipe], RecvBuf);
    }
}


/*******************************************************************************
* 函数名称: taskmng_module_head_node_delete
* 函数功能: 删除链表
* 输入参数:
*          pModuleMetAddrHead
           DeleteNum
* 输出参数: 无
* 返 回 值: >0                  成功
*           <0                  失败
*******************************************************************************/
MODULE_METADDRHEAD *taskmng_module_head_node_delete(MODULE_METADDRHEAD *pModuleMetAddrHead,uint16 DeleteNum)
{
    uint16              DeleteCnt = 0;

    MODULE_METADDRHEAD  *TmppModuleMetAddrHead = NULL;


    while(pModuleMetAddrHead != NULL)//如果不为空
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
* 函数名称: taskmng_module_head_node_add
* 函数功能: 添加节点信息到相应的链表头
* 输入参数:
            RouterSn                   添加的序号
*           MtrAddr                    电表地址
*           pModuleMetAddrHead         添加的链表头)
* 输出参数:
* 返 回 值:
*******************************************************************************/
MODULE_METADDRHEAD *taskmng_module_head_node_add(uint16 RouterSn,uint8 *Addr,uint8 prtl,MODULE_METADDRHEAD *pModuleMetAddrHead)
{
    uint16  ModuleAddrNodeNum = 1;  //需要增加或者删除的从节点数量

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

        Start_ModuleAddr = pModuleMetAddrHead;            //将链表头赋予当前的指针

        while(Start_ModuleAddr->PNext != NULL)            //找到链表的最后一个位置
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
* 函数名称: SerachNodeInLink
* 函数功能: 查询此次新增的从节点信息在链表里是否已经存在
* 输入参数:
*           pModuleMetAddrHead         原链表
*           Addr                       需要新增的地址信息
* 输出参数: 无
* 返 回 值: >0                  存在
*           <0                  不存在
*******************************************************************************/
int SerachNodeInLink(MODULE_METADDRHEAD *pModuleMetAddrHead,uint8 *Addr)
{
    MODULE_METADDRHEAD *TmpModuleLink = NULL;

    if(pModuleMetAddrHead == NULL)
    {
        return -1;//不存在
    }

    TmpModuleLink = pModuleMetAddrHead;
    while(TmpModuleLink != NULL)
    {
        if(memcmp(TmpModuleLink->Module_MetAddr,Addr,6) == 0)
            return 1;//存在

        TmpModuleLink = TmpModuleLink->PNext;
    }

    return -1;//不存在
}







/*******************************************************************************
* 函数名称: GetNeedMtrNum
* 函数功能: 获取每个线程中任务需要抄读的表个数
* 输入参数: pipe 端口号
            taskindex 任务索引
* 输出参数: 无
* 返 回 值:  本线程需要抄读的电表个数
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
* 函数名称: TaskRunSort
* 函数功能: //将每个线程中的运行时任务队列按照优先级进行排序形成链表
* 输入参数:

* 输出参数: 无
* 返 回 值:
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

    CTS_RUN_TASK_T *PTmpHeadTaskLink = NULL;//轮询链表时使用
    PTmpHeadTaskLink = pAmrThreadArg->pTaskHead;
    TASK_FMT_TRACE(Pipe,REC_LOG,"日常抄表任务队列   : \n");

    while(PTmpHeadTaskLink != NULL)
    {
        TASK_FMT_TRACE(Pipe,REC_LOG," TaskId %d \n",PTmpHeadTaskLink->TaskId);
        TASK_FMT_TRACE(Pipe,REC_LOG," NeedMtrNum %d  \n",PTmpHeadTaskLink->NeedMtrNum);
        TASK_FMT_TRACE(Pipe,REC_LOG," Priority  %d  \n",PTmpHeadTaskLink->Priority);
        PTmpHeadTaskLink = PTmpHeadTaskLink->next;
    }
}

/*******************************************************************************
* 函数名称: TaskRun_UpdateTimes
* 函数功能: 计算每个线程中任务执行轮次
* 输入参数: Pipe  端口号
          index  任务索引
          TaskInfo 任务运行时参数
* 输出参数: 无
* 返 回 值:
*******************************************************************************/
BOOL TaskRun_UpdateTimes(uint8 Pipe,uint8 index,CTS_RUN_TASK_T *TaskInfo)
{
    //rtc_t            RealTime;                     //当前系统时间时间(RTC格式)
    time_t           RealTime_t;                   //当前系统时间的转换的秒数
    struct tm        tm_time;
    time_t           ThisTurnBeginTime_t;
    time_t           ThisTurnEndTime_t;
    time_t          NextTurnBeginTime_t;
    //rtc_t            TaskRunBeginTime;             //方案开始执行时间(RTC格式)
    time_t           TaskRunBeginTime_t;           //方案开始执行转换后的秒
    //rtc_t            TaskRunEndTime;               //方案结束执行时间(RTC格式)
    //time_t           TaskRunEndTime_t;             //方案结束执行转换后的秒
    OOP_TASK_CFG *pTaskCfg = &SchemeTaskCfgList.OopTaskCfg[index];
    uint32           dwPeriod = pTaskCfg->RunCycle;   //重复周期
    time_t           ProThisTurnTimeMark_t;
    time_t           ProThisTurnBeginTime_t;
    time_t           ProThisTurnEndTime_t;
    time_t           ProNextTurnBeginTime_t;
    time_t           ProLastTurnBeginTime_t;
    rtc_t            rtc_tmp;
    OOP_TI_T             Ti;
    //uint16           Year;

    //系统当前时间
    RealTime_t = time(NULL);
    LOCALTIME_R(&RealTime_t, &tm_time);

    memset(&rtc_tmp, 0, sizeof(rtc_t));
    memcpy(&rtc_tmp, TaskInfo->ThisTurnBeginTime, 6);
    ThisTurnBeginTime_t = commfun_rtc2time2(rtc_tmp);


    memset(&rtc_tmp, 0, sizeof(rtc_t));
    memcpy(&rtc_tmp, TaskInfo->ThisTurnEndTime, 6);
    ThisTurnEndTime_t = commfun_rtc2time2(rtc_tmp);

    //减去任务开始时间里包含的延时
    Ti.unit = pTaskCfg->DelayTimeUnit;
    Ti.value = pTaskCfg->DelayTime;
    DT_TimeSubInv(&ThisTurnEndTime_t, Ti);
    DT_TimeSubInv(&ThisTurnBeginTime_t, Ti);
    //TASK_FMT_TRACE(Pipe,REC_LOG,"当前时间是 is %d,本轮开始时间 is %d,本轮结束时间 is %d\n",RealTime_t,ThisTurnEndTime_t,ThisTurnBeginTime_t);
    if(FALSE==taskmng_task_in_period_check(index,RealTime_t))
    {
        return FALSE;
    }
    //TASK_FMT_TRACE(Pipe,REC_LOG,"当前时间是11 is %d,本轮开始时间 is %d,本轮结束时间 is %d\n",RealTime_t,ThisTurnEndTime_t,ThisTurnBeginTime_t);
    if(RealTime_t >= ThisTurnBeginTime_t && RealTime_t < ThisTurnEndTime_t) //还在本轮无需更新
    {
        return FALSE;
    }
    //过秒执行 以防一秒内执行太多次
    //TASK_FMT_TRACE(Pipe,REC_LOG,"Pipe is %d,gPastTimeFlag[Pipe].BitSect.Second is %d\n",Pipe,gPastTimeFlag[Pipe].BitSect.Second);
    if ( gPastTimeFlag[Pipe].BitSect.Second != TRUE )
        return FALSE;

    memset(&rtc_tmp, 0, sizeof(rtc_t));
    memcpy(&rtc_tmp, TaskInfo->NextTurnBeginTime, 6);
    NextTurnBeginTime_t = commfun_rtc2time2(rtc_tmp);
    if(RealTime_t>=ThisTurnEndTime_t && RealTime_t <NextTurnBeginTime_t)
    {
        //TASK_FMT_TRACE(Pipe,REC_LOG,"任务 %d 到达任务结束时间 置任务结束\n",TaskInfo->TaskId);
        TaskInfo->MinFrozenRebootCnt = 0;
        taskmng_task_finish(gAmrThreadArg[Pipe].DBClientHandle,TaskInfo->TaskId);
    }
    if(dwPeriod==0)
    {
        dwPeriod=1;//因为数据会为0，先默认一个1
    }

    TASK_FMT_TRACE(Pipe,REC_LOG,"任务 %d 不在本轮 realtime %ld begin %ld end %ld \n",TaskInfo->TaskId,RealTime_t,ThisTurnBeginTime_t,ThisTurnEndTime_t);

    //任务开始时间

    DT_DateTimeShort2Time(&pTaskCfg->RunBeginTime,&TaskRunBeginTime_t);

    switch (pTaskCfg->RunCycleUnit) //执行频率单位
    {
    case TASK_CYCLE_FLAG_SEC:       //分钟
        ProThisTurnTimeMark_t  = (RealTime_t/dwPeriod)*dwPeriod;//根据周期获取当前的时标点
        ProThisTurnBeginTime_t = ProThisTurnTimeMark_t + TaskRunBeginTime_t%dwPeriod;
        if(ProThisTurnBeginTime_t > RealTime_t)
            ProThisTurnBeginTime_t -= dwPeriod;
        ProThisTurnEndTime_t   = (ProThisTurnBeginTime_t + dwPeriod)-1;
        ProNextTurnBeginTime_t = ProThisTurnBeginTime_t + dwPeriod;
        ProLastTurnBeginTime_t = ProThisTurnBeginTime_t - dwPeriod;
        break;

    case TASK_CYCLE_FLAG_MIN:       //分钟
        dwPeriod *= 60L;
        ProThisTurnTimeMark_t  = (RealTime_t/dwPeriod)*dwPeriod;//根据周期获取当前的时标点
        ProThisTurnBeginTime_t = ProThisTurnTimeMark_t + TaskRunBeginTime_t%dwPeriod;
        if(ProThisTurnBeginTime_t > RealTime_t)
            ProThisTurnBeginTime_t -= dwPeriod;
        ProThisTurnEndTime_t   = (ProThisTurnBeginTime_t + dwPeriod)-1;
        ProNextTurnBeginTime_t = ProThisTurnBeginTime_t + dwPeriod;
        ProLastTurnBeginTime_t = ProThisTurnBeginTime_t - dwPeriod;
        break;

    case TASK_CYCLE_FLAG_HOUR:      //小时
        dwPeriod *= 3600L;
        ProThisTurnTimeMark_t  = (RealTime_t/dwPeriod)*dwPeriod;
        ProThisTurnBeginTime_t = ProThisTurnTimeMark_t + TaskRunBeginTime_t%dwPeriod;
        if(ProThisTurnBeginTime_t > RealTime_t)
            ProThisTurnBeginTime_t -= dwPeriod;
        ProThisTurnEndTime_t   = (ProThisTurnBeginTime_t + dwPeriod)-60;
        ProNextTurnBeginTime_t = ProThisTurnBeginTime_t + dwPeriod;
        ProLastTurnBeginTime_t = ProThisTurnBeginTime_t - dwPeriod;
        break;

    case TASK_CYCLE_FLAG_DAY:       //日
        dwPeriod *= 86400L;
        ProThisTurnTimeMark_t  = (RealTime_t/dwPeriod)*dwPeriod;
        ProThisTurnBeginTime_t = ProThisTurnTimeMark_t + TaskRunBeginTime_t%dwPeriod;
        if(ProThisTurnBeginTime_t > RealTime_t)
            ProThisTurnBeginTime_t -= dwPeriod;
        ProThisTurnEndTime_t   = (ProThisTurnBeginTime_t + dwPeriod)-60;
        ProNextTurnBeginTime_t = ProThisTurnBeginTime_t + dwPeriod;
        ProLastTurnBeginTime_t = ProThisTurnBeginTime_t - dwPeriod;
        break;

    case TASK_CYCLE_FLAG_MON:       //月
    {
        time_t          tm_tmp = 0;
        OOP_DATETIME_S_T tmp;
        memset(&tmp,0x00,sizeof(OOP_DATETIME_S_T));

        //将任务开始时间的日改为0
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
    //把延时加上去
    Ti.unit = pTaskCfg->DelayTimeUnit;
    Ti.value = pTaskCfg->DelayTime;


    DT_TimeAddInv(&ProThisTurnBeginTime_t, Ti);


    LOCALTIME_R(&ProThisTurnBeginTime_t, &tm_time);
    if(tm_time.tm_year<100)
        tm_time.tm_year = 100;
    TaskInfo->ThisTurnBeginTime[6] = 0x20;
    TaskInfo->ThisTurnBeginTime[5] = commfun_HexToBcd((uint8)(tm_time.tm_year - 100));  //年
    TaskInfo->ThisTurnBeginTime[4] = commfun_HexToBcd((uint8)(tm_time.tm_mon + 1));     //月
    TaskInfo->ThisTurnBeginTime[3] = commfun_HexToBcd((uint8)tm_time.tm_mday);          //日
    TaskInfo->ThisTurnBeginTime[2] = commfun_HexToBcd((uint8)tm_time.tm_hour);          //时
    TaskInfo->ThisTurnBeginTime[1] = commfun_HexToBcd((uint8)tm_time.tm_min);           //分
    TaskInfo->ThisTurnBeginTime[0] = commfun_HexToBcd((uint8)tm_time.tm_sec);           //分

    Ti.unit = pTaskCfg->DelayTimeUnit;
    Ti.value = pTaskCfg->DelayTime;
    DT_TimeAddInv(&ProThisTurnEndTime_t, Ti);
    LOCALTIME_R(&ProThisTurnEndTime_t, &tm_time);
    if(tm_time.tm_year<100)
        tm_time.tm_year = 100;
    TaskInfo->ThisTurnEndTime[6] = 0x20;
    TaskInfo->ThisTurnEndTime[5] = commfun_HexToBcd((uint8)(tm_time.tm_year - 100));    //年
    TaskInfo->ThisTurnEndTime[4] = commfun_HexToBcd((uint8)(tm_time.tm_mon + 1));       //月
    TaskInfo->ThisTurnEndTime[3] = commfun_HexToBcd((uint8)tm_time.tm_mday);            //日
    TaskInfo->ThisTurnEndTime[2] = commfun_HexToBcd((uint8)tm_time.tm_hour);            //时
    TaskInfo->ThisTurnEndTime[1] = commfun_HexToBcd((uint8)tm_time.tm_min);         //分
    TaskInfo->ThisTurnEndTime[0] = commfun_HexToBcd((uint8)tm_time.tm_sec);         //分

    LOCALTIME_R(&ProNextTurnBeginTime_t, &tm_time);
    if(tm_time.tm_year<100)
        tm_time.tm_year = 100;
    TaskInfo->NextTurnBeginTime[6] =0x20;
    TaskInfo->NextTurnBeginTime[5] = commfun_HexToBcd((uint8)(tm_time.tm_year - 100));  //年
    TaskInfo->NextTurnBeginTime[4] = commfun_HexToBcd((uint8)(tm_time.tm_mon + 1));     //月
    TaskInfo->NextTurnBeginTime[3] = commfun_HexToBcd((uint8)tm_time.tm_mday);          //日
    TaskInfo->NextTurnBeginTime[2] = commfun_HexToBcd((uint8)tm_time.tm_hour);          //时
    TaskInfo->NextTurnBeginTime[1] = commfun_HexToBcd((uint8)tm_time.tm_min);           //分
    TaskInfo->NextTurnBeginTime[0] = commfun_HexToBcd((uint8)tm_time.tm_sec);           //分

    LOCALTIME_R(&ProLastTurnBeginTime_t, &tm_time);
    if(tm_time.tm_year<100)
        tm_time.tm_year = 100;
    TaskInfo->LastTurnBeginTime[6] =0x20;
    TaskInfo->LastTurnBeginTime[5] = commfun_HexToBcd((uint8)(tm_time.tm_year - 100));  //年
    TaskInfo->LastTurnBeginTime[4] = commfun_HexToBcd((uint8)(tm_time.tm_mon + 1));     //月
    TaskInfo->LastTurnBeginTime[3] = commfun_HexToBcd((uint8)tm_time.tm_mday);          //日
    TaskInfo->LastTurnBeginTime[2] = commfun_HexToBcd((uint8)tm_time.tm_hour);          //时
    TaskInfo->LastTurnBeginTime[1] = commfun_HexToBcd((uint8)tm_time.tm_min);           //分
    TaskInfo->LastTurnBeginTime[0] = commfun_HexToBcd((uint8)tm_time.tm_sec);           //分
    taskmng_task_monitor_update(Pipe,TaskInfo->TaskId,TaskInfo->logicid,UDTASK_INIT,TaskInfo);

    return TRUE;
}


/*******************************************************************************
* 函数名称: TaskRun_UpdatePeriodStat
* 函数功能: 判断任务是否在运行时段
* 输入参数: TaskInfo 任务运行时参数
            index  任务索引
* 输出参数: 无
* 返 回 值:
*******************************************************************************/
void TaskRun_UpdatePeriodStat(uint8 index,CTS_RUN_TASK_T *TaskInfo)
{
    rtc_t           rtc_tmp;
    time_t          RealTime_t;     //当前系统时间的转换的秒数
    time_t          TaskRunBeginTime_t; //方案开始执行转换后的秒
    time_t          TaskRunEndTime_t;   //方案结束执行转换后的秒
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
    //time_t最多只能表示到2038年
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

    //判断当前是否是否在任务本轮次任务开始以及结束之间
    if((RealTime_t >= ThisTurnBeginTime_t) && (RealTime_t < ThisTurnEndTime_t))
    {
        TaskInfo->ThisTurnPeriodStat = AMR_IN_PERIOD;

        //方案任务允许是否在允许时段内
        taskmng_task_run_in_period(TaskInfo,pTaskCfg);
    }

    //判断当前是否是否在任务本轮次结束之后
    if(RealTime_t < ThisTurnBeginTime_t)
    {
        TaskInfo->ThisTurnPeriodStat = AMR_PRE_PERIOD;
    }

    //判断当前是否是否在任务本轮次结束之后
    if(RealTime_t > ThisTurnEndTime_t)
    {
        TaskInfo->ThisTurnPeriodStat = AMR_AFT_PERIOD;
    }
    //TASK_FMT_TRACE( -1, REC_LOG," TaskInfo->ThisTurnPeriodStat is %d,TaskRunBeginTime_t is %d,TaskRunEndTime_t is %d\n",TaskInfo->ThisTurnPeriodStat,TaskRunBeginTime_t,TaskRunEndTime_t);
}






/*******************************************************************************
* 函数名称: GetPriorityFromTaskID
* 函数功能: 估计任务id获取任务优先级
* 输入参数: Pipe  TaskId
* 输出参数:
* 返 回 值: 任务优先级
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
* 函数名称: GetNeedAgainTaskInfo
* 函数功能:
* 输入参数: TaskId
* 输出参数: TaskInfo
* 返 回 值:
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
            TASK_FMT_TRACE ( Pipe,COMM_LOG, "判断补抄: pAmrThreadArg->pTaskRunInfo[j].IsAlreadyBc is %d,pAmrThreadArg->pTaskRunInfo[j].TaskId is %d\n", pAmrThreadArg->pTaskRunInfo[j].IsAlreadyBc,pAmrThreadArg->pTaskRunInfo[j].TaskId);
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
* 函数名称: GetAcNeedAgainTaskInfo
* 函数功能:
* 输入参数: TaskId
* 输出参数: TaskInfo
* 返 回 值:
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
            TASK_FMT_TRACE( -1,TG_LOG, "判断补抄: TGThreadArg.pTaskRunInfo[j].IsAlreadyBc is %d,TGThreadArg.pTaskRunInfo[j].TaskId is %d\n", TGThreadArg.pTaskRunInfo[j].IsAlreadyBc,TGThreadArg.pTaskRunInfo[j].TaskId);
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
* 函数名称: taskmng_oop_meter_security_para_read
* 函数功能: 读取面向对象表安全模式相关参数
* 输入参数:
* 输出参数:
* 返 回 值:
*******************************************************************************/
int taskmng_oop_meter_security_para_read(uint8 Pipe,uint16 Pn,uint8 *MtrEsamSerialNo,uint8 *MtrSecModEn)
{
    //不用抄安全模式了
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
* 函数名称: taskmng_oop_meter_security_para_read
* 函数功能: 读取面向对象表安全模式ESAM序列号
* 输入参数:
* 输出参数:
* 返 回 值:
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
* 函数名称: taskmng_oop_meter_security_para_read
* 函数功能: 读取面向对象表安全模式ESAM序列号
* 输入参数:
* 输出参数:
* 返 回 值:
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
* 函数名称: WriteOopMeterSecurityEsamSerialNo
* 函数功能: 写面向对象表安全模式
* 输入参数:
* 输出参数:
* 返 回 值:
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
* 函数名称: taskmng_oop_meter_security_para_read
* 函数功能: 读取面向对象表安全模式开关
* 输入参数:
* 输出参数:
* 返 回 值:
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
* 函数名称: taskmng_mtsd_188_get_request
* 函数功能: 188四表组帧
* 输入参数:
* 输出参数:
* 返 回 值:
*******************************************************************************/
uint16 taskmng_mtsd_188_get_request( uint8 pipe,uint8 *FrameBuff,int mterindex)
{
    uint8   CS = 0;                       //校验码
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
        meter_type = 0x10;  //默认水表
    }


    memset(tmpSnd, 0x00, sizeof(tmpSnd));
    for(i = 0; i < 4; i++)
        tmpSnd[i] = 0xFE;

    offset = 4;

    tmpSnd[offset++] = 0x68;
    //表类型
    tmpSnd[offset++] = meter_type;
    /*- 表地址 -*/
    tmpSnd[offset++] = TsaAddr.add[6];
    tmpSnd[offset++] = TsaAddr.add[5];
    tmpSnd[offset++] = TsaAddr.add[4];
    tmpSnd[offset++] = TsaAddr.add[3];
    tmpSnd[offset++] = TsaAddr.add[2];
    tmpSnd[offset++] = TsaAddr.add[1];
    tmpSnd[offset++] = TsaAddr.add[0];
    tmpSnd[offset++] = 0x01;              //功能码
    tmpSnd[offset++] = 0x03;           //数据域长度
    tmpSnd[offset++] = (uint8)(fraDI&0x00FF);            //数据标识
    tmpSnd[offset++] = (uint8)(fraDI>>8);
    //序列号
    ser = ser%256 + 1;
    tmpSnd[offset++] = ser;
    //校验和
    for (i = 4; i < offset; i++)
    {
        CS += tmpSnd[i];
    }
    tmpSnd[offset++] = CS;
    tmpSnd[offset++] = 0x16;

    memcpy(FrameBuff, tmpSnd, offset);        /*- 电表发送缓冲区pBuf*/

    return  offset;
}



/*******************************************************************************
* 函数名称: taskmng_meter_frame_make
* 函数功能: 抄表组帧
* 输入参数: Pipe 端口号
         StartTime  开始时间
         FrameSeqInfo 帧信息

* 输出参数: DataBuff 组好的抄表帧
* 返 回 值: 抄表帧长度
*******************************************************************************/
uint16 taskmng_meter_frame_make(uint8 Pipe,uint8 seqindex,time_t StartTime,FRAME_SEQ_INFO_T FrameSeqInfo,uint8 *DataBuff,uint16 taskid,uint8 logicid)
{

    MeterPrtlFmt_T  MeterPrtlData;                      //电表规约格式数据
    //uint8           MtrEsamSerialNo[8];                 //面向对象表表号
   // uint8           MtrSecModEn;                        //面向对象表安全模式开关
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
    MsIndex   = FrameSeqInfo.MsIndex;       //当前线程中的测量点索引
    SubOadNum = FrameSeqInfo.DiInfo.SubOadNum;
    //LastTaskId     = pAmrThreadArg->TmnAmrInfo.CurrentTaskId;//发送时的方案号



    MeterPrtlData.DelayFlag = 0;


    MeterPrtlData.PrtlMode = FrameSeqInfo.PrtlMode;      //通信规约
    //刷新采集器地址和电表地址
    memcpy(MeterPrtlData.MtrAddr, FrameSeqInfo.MeterAddr, 6);
    memcpy(MeterPrtlData.AcqUnitAddr, FrameSeqInfo.AcqAddr, 6);
    MeterPrtlData.MtrType = FrameSeqInfo.Mettype;


    if(MeterPrtlData.PrtlMode == PRTL_OOP)//面向对象电表协议的组帧
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
                            || DI07 == 0x06030001 || DI07 == 0x06040001 || (DI07&0xffff0000) == 0x06100000)     //如果点抄小时冻结数据块
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
                            MeterPrtlData.DataBuff[4] = taskmng_curve_point_get( taskid, logicid);  //一次抄读8个点
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
* 函数名称: taskmng_oop_meter_get_request
* 函数功能: 面向对象表组帧
* 输入参数:
* 输出参数:
* 返 回 值:
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
    LastTaskId     = pAmrThreadArg->TmnAmrInfo.CurrentTaskId;//发送时的方案号
    AcqType_t  Acqtype =ACQTYP_NULL;
    uint8 choiceadd = 0;        //搞个适配 如果是分钟级的任务 则get的时候choice加个100 在get函数里特殊处理用明文抄
    taskmng_task_info_get(LastTaskId,pAmrThreadArg->TmnAmrInfo.CurrentLogicid, &pTmp_NormalLink);
    if(pTmp_NormalLink.MinTaskFlag == 1)
        choiceadd = MIN_TASK_CHOICE_ADD;

    taskmng_mtr_basic_info_get(pAmrThreadArg->pMtrAmrInfo[MsIndex].Pn,pAmrThreadArg->pMtrAmrInfo[MsIndex].logicid,&MtrAddr);
    //读取面向对象电表地址
    TsaAddr = taskmng_tsa_get(pAmrThreadArg->pMtrAmrInfo[MsIndex].Pn,pAmrThreadArg->pMtrAmrInfo[MsIndex].logicid);
    //TASK_FMT_TRACE(Pipe,REC_LOG,"mainoad %08x    \n",MainOad.value);
//#ifdef AREA_HUNAN
#if defined(AREA_HUNAN) || defined(AREA_SHANDONG)
    //抄负荷识别模块逻辑设备号用3
    //抄谐波电压电流用5
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
        //有逻辑地址后长度要加一
        TsaAddr.len++;
        TsaAddr.flag = 1;
        TsaAddr.vxd = flag;
    }
#endif
     Acqtype = pTmp_NormalLink.AcqInfo.Acqtype;
    //根据主OAD对应的类号获取RSD方法对象属性描述符OAD
    if(MainOad.nObjID != 0x0000)
    {
        if(taskmng_oad_info_get(MainOad,&OadInfo)==FALSE)
            return 0;
        if(OadInfo.class ==9)
        {
            RsdOad.value = 0x20210200;

        }
        else if(OadInfo.class ==7 || OadInfo.class ==24)   //事件类
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
    //山东非电量类的数据用实时抄
    if(((MainOad.nObjID>>12)&0x0f)  != 3  )
    {
        if(taskmng_energy_oad_check(SubOad[0])==FALSE)
        {
            TASK_FMT_TRACE(Pipe,REC_LOG,"山东非电量类使用实时抄读 \n");
            Acqtype = ACQTYP_NULL;
        }
        if(pAmrThreadArg->pMtrAmrInfo[MsIndex].NILMModuleInfo ==0 && SubOad[0].value == 0x40310200)
        {
            Acqtype = ACQTYP_NULL;
        }
    }
#endif
    //安全模式按照OAD方式进行读取
    //载波组645帧时按照并发数分配通道号 以防有分帧时报文被覆盖
    if(ACQTYP_NULL == Acqtype || (TRUE == pAmrThreadArg->pMtrAmrInfo[MsIndex].IsNeedGetSafeMode))//实时数据采用GetRequestNormalList
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
    else//其他读取采用GetRequestRecord
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
* 函数名称: MakeRsdFrame
* 函数功能: 记录选择描述符RSD组帧
* 输入参数:
* 输出参数:
* 返 回 值:
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
    case 0://选择方法0
        break;

    case 1://选择方法1
        break;

    case 2://选择方法2
    {
        RsdBuf[RsdLen++] = 2;    //选择方法2

        //对象属性描述符  OAD
        RsdBuf[RsdLen++] = (uint8)(RsdOad.nObjID>> 8);
        RsdBuf[RsdLen++] = (uint8)(RsdOad.nObjID);
        RsdBuf[RsdLen++] = RsdOad.attID;
        RsdBuf[RsdLen++] = RsdOad.nIndex;

        RsdBuf[RsdLen++] = DATE_TIME_S;//起始时间数据类型
        memcpy_r((uint8*)&TempTimeBcd, (uint8*)RunTask->LastTurnBeginTime, 7);//起始时间
        //TASK_FMT_TRACE(Pipe,REC_LOG,"MakeRsdFrame LastTurnBeginTime \n");
       // WriteTimeLog( Pipe,RunTask->LastTurnBeginTime);
        printf("MakeRsdFrame LastTurnBeginTime %02x%02x%02x%02x%02x%02x%02x\n",TempTimeBcd.year_h,TempTimeBcd.year_l,TempTimeBcd.month,TempTimeBcd.day,TempTimeBcd.hour,TempTimeBcd.min,TempTimeBcd.sec);
        DT_DateTimeBCD2Time(&TempTimeBcd, &TempTimeSec);
        //曲线类冻结不需要加1秒
        if(pTmp_NormalLink->CsdTask.RoadTask[0].MainOad.nObjID < 0x5000 || pTmp_NormalLink->CsdTask.RoadTask[0].MainOad.nObjID > 0x5003)
            TempTimeSec = TempTimeSec+60;
        else
            TempTimeSec = StartTime;
        if(pTmp_NormalLink->AcqInfo.Acqtype == ACQTYP_BC)
            TempTimeSec = StartTime;
        DT_Time2DateTimeHex(TempTimeSec, (DateTimeHex_t*)(RsdBuf+RsdLen));
        RsdLen += 7;

        RsdBuf[RsdLen++] = DATE_TIME_S;//结束时间数据类型
        memcpy_r((uint8*)&TempTimeBcd,(uint8*)RunTask->ThisTurnBeginTime,7);//结束时间
        //TASK_FMT_TRACE(Pipe,REC_LOG,"MakeRsdFrame ThisTurnBeginTime \n");
       // WriteTimeLog( Pipe,RunTask->ThisTurnBeginTime);

        printf("MakeRsdFrame ThisTurnBeginTime %02x%02x%02x%02x%02x%02x%02x\n",TempTimeBcd.year_h,TempTimeBcd.year_l,TempTimeBcd.month,TempTimeBcd.day,TempTimeBcd.hour,TempTimeBcd.min,TempTimeBcd.sec);

        DT_DateTimeBCD2Time(&TempTimeBcd,&TempTimeSec);
        DelayTi.value = pTmp_NormalLink->DelayTime;
        DelayTi.unit = pTmp_NormalLink->DelayTimeUnit;
        DT_TimeSubInv(&TempTimeSec, DelayTi);//减去任务开始时间里包含的延时，这是抄表和任务实际启动时间无关
        //曲线类冻结不需要加1秒
        if(pTmp_NormalLink->CsdTask.RoadTask[0].MainOad.nObjID< 0x5000 || pTmp_NormalLink->CsdTask.RoadTask[0].MainOad.nObjID > 0x5003)
            TempTimeSec = TempTimeSec+60;
        else
        {
            TempTimeSec = StartTime;
            DelayTi.value = pTmp_NormalLink->AcqInfo.TI.value;
            DelayTi.unit = pTmp_NormalLink->AcqInfo.TI.unit;
            DT_TimeAddInv(&TempTimeSec, DelayTi);//加上一个时标间隔，即只抄1个点
            TempTimeSec = TempTimeSec-1;  //部分表筛选方法是前闭后闭，-1秒强制为前闭后开
        }
        if(pTmp_NormalLink->AcqInfo.Acqtype == ACQTYP_BC)
        {
            TempTimeSec = StartTime;
            DelayTi.value = pTmp_NormalLink->AcqInfo.TI.value;
            DelayTi.unit = pTmp_NormalLink->AcqInfo.TI.unit;
            DT_TimeAddInv(&TempTimeSec, DelayTi);//加上一个时标间隔，即只抄1个点
            /* 抄两个周期看下 240109 虚拟模块收1495个字节误码 还是只能抄一天的*/
            /* if( pTmp_NormalLink->CsdTask.RoadTask[0].MainOad.nObjID == 0x5004)
                DT_TimeAddInv(&TempTimeSec, DelayTi); */
            TempTimeSec = TempTimeSec-1;  //部分表筛选方法是前闭后闭，-1秒强制为前闭后开
        }
        DT_Time2DateTimeHex(TempTimeSec, (DateTimeHex_t*)(RsdBuf+RsdLen));
        RsdLen += 7;
        if(pTmp_NormalLink->SchemeType == EVENTPLAN)
        {
            RsdBuf[RsdLen++] = 0;
        }
        else
        {
            //数据间隔
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

    case 3://选择方法3
        break;

    case 4://选择方法4
        break;

    case 5://选择方法5
        break;

    case 6://选择方法6
        break;

    case 7://选择方法7
        break;

    case 8://选择方法8
        break;

    case 9://选择方法9
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
* 函数名称: MakeRsdFrame
* 函数功能: 记录选择描述符RSD组帧
* 输入参数:
* 输出参数:
* 返 回 值:
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
    case 0://选择方法0
        break;

    case 1://选择方法1
        break;

    case 2://选择方法2
    {
        RsdBuf[RsdLen++] = 2;    //选择方法2

        //对象属性描述符  OAD
        RsdBuf[RsdLen++] = (uint8)(RsdOad.nObjID>> 8);
        RsdBuf[RsdLen++] = (uint8)(RsdOad.nObjID);
        RsdBuf[RsdLen++] = RsdOad.attID;
        RsdBuf[RsdLen++] = RsdOad.nIndex;

        RsdBuf[RsdLen++] = DATE_TIME_S;//起始时间数据类型
        memcpy_r((uint8*)&TempTimeBcd, (uint8*)RunTask->LastTurnBeginTime, 7);//起始时间
        //TASK_FMT_TRACE(Pipe,REC_LOG,"MakeRsdFrame LastTurnBeginTime \n");
       // WriteTimeLog( Pipe,RunTask->LastTurnBeginTime);
        printf("MakeRsdFrame LastTurnBeginTime %02x%02x%02x%02x%02x%02x%02x\n",TempTimeBcd.year_h,TempTimeBcd.year_l,TempTimeBcd.month,TempTimeBcd.day,TempTimeBcd.hour,TempTimeBcd.min,TempTimeBcd.sec);
        DT_DateTimeBCD2Time(&TempTimeBcd, &TempTimeSec);
        //曲线类冻结不需要加1秒
        if(pTmp_NormalLink->CsdTask.RoadTask[0].MainOad.nObjID < 0x5000 || pTmp_NormalLink->CsdTask.RoadTask[0].MainOad.nObjID > 0x5003)
            TempTimeSec = TempTimeSec+60;
        else
            TempTimeSec = StartTime;
        DT_Time2DateTimeHex(TempTimeSec, (DateTimeHex_t*)(RsdBuf+RsdLen));
        RsdLen += 7;

        RsdBuf[RsdLen++] = DATE_TIME_S;//结束时间数据类型
        memcpy_r((uint8*)&TempTimeBcd,(uint8*)RunTask->ThisTurnBeginTime,7);//结束时间
        //TASK_FMT_TRACE(Pipe,REC_LOG,"MakeRsdFrame ThisTurnBeginTime \n");
       // WriteTimeLog( Pipe,RunTask->ThisTurnBeginTime);

        printf("MakeRsdFrame ThisTurnBeginTime %02x%02x%02x%02x%02x%02x%02x\n",TempTimeBcd.year_h,TempTimeBcd.year_l,TempTimeBcd.month,TempTimeBcd.day,TempTimeBcd.hour,TempTimeBcd.min,TempTimeBcd.sec);

        DT_DateTimeBCD2Time(&TempTimeBcd,&TempTimeSec);
        DelayTi.value = pTmp_NormalLink->DelayTime;
        DelayTi.unit = pTmp_NormalLink->DelayTimeUnit;
        DT_TimeSubInv(&TempTimeSec, DelayTi);//减去任务开始时间里包含的延时，这是抄表和任务实际启动时间无关
        //曲线类冻结不需要加1秒
        if(pTmp_NormalLink->CsdTask.RoadTask[0].MainOad.nObjID< 0x5000 || pTmp_NormalLink->CsdTask.RoadTask[0].MainOad.nObjID > 0x5003)
            TempTimeSec = TempTimeSec+60;
        else
        {
            TempTimeSec = StartTime;
            DelayTi.value = pTmp_NormalLink->AcqInfo.TI.value;
            DelayTi.unit = pTmp_NormalLink->AcqInfo.TI.unit;
            DT_TimeAddInv(&TempTimeSec, DelayTi);//加上一个时标间隔，即只抄1个点
            TempTimeSec = TempTimeSec-1;  //部分表筛选方法是前闭后闭，-1秒强制为前闭后开
        }
        DT_Time2DateTimeHex(TempTimeSec, (DateTimeHex_t*)(RsdBuf+RsdLen));
        RsdLen += 7;
        if(pTmp_NormalLink->SchemeType == EVENTPLAN)
        {
            RsdBuf[RsdLen++] = 0;
        }
        else
        {
            //数据间隔
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

    case 3://选择方法3
        break;

    case 4://选择方法4
        {
            RsdBuf[RsdLen++] = 4;    //选择方法2
            memcpy_r((uint8*)&TempTimeBcd, (uint8*)RunTask->LastTurnBeginTime, 7);//起始时间

            printf("MakeRsdFrame LastTurnBeginTime %02x%02x%02x%02x%02x%02x%02x\n",TempTimeBcd.year_h,TempTimeBcd.year_l,TempTimeBcd.month,TempTimeBcd.day,TempTimeBcd.hour,TempTimeBcd.min,TempTimeBcd.sec);
            DT_DateTimeBCD2Time(&TempTimeBcd, &TempTimeSec);
            //曲线类冻结不需要加1秒
            if(pTmp_NormalLink->CsdTask.RoadTask[0].MainOad.nObjID < 0x5000 || pTmp_NormalLink->CsdTask.RoadTask[0].MainOad.nObjID > 0x5003)
                TempTimeSec = TempTimeSec+60;
            else
                TempTimeSec = StartTime;
            DT_Time2DateTimeHex(TempTimeSec, (DateTimeHex_t*)(RsdBuf+RsdLen));
            RsdLen += 7;
        }
        RsdLen = GetMsInfo(ms,RsdBuf,RsdLen);
        break;
    case 5://选择方法5
        {
            RsdBuf[RsdLen++] = 5;    //选择方法2
            memcpy_r((uint8*)&TempTimeBcd, (uint8*)RunTask->LastTurnBeginTime, 7);//起始时间

            printf("MakeRsdFrame LastTurnBeginTime %02x%02x%02x%02x%02x%02x%02x\n",TempTimeBcd.year_h,TempTimeBcd.year_l,TempTimeBcd.month,TempTimeBcd.day,TempTimeBcd.hour,TempTimeBcd.min,TempTimeBcd.sec);
            DT_DateTimeBCD2Time(&TempTimeBcd, &TempTimeSec);
            //曲线类冻结不需要加1秒
            if(pTmp_NormalLink->CsdTask.RoadTask[0].MainOad.nObjID < 0x5000 || pTmp_NormalLink->CsdTask.RoadTask[0].MainOad.nObjID > 0x5003)
                TempTimeSec = TempTimeSec+60;
            else
                TempTimeSec = StartTime;
            DT_Time2DateTimeHex(TempTimeSec, (DateTimeHex_t*)(RsdBuf+RsdLen));
            RsdLen += 7;

            RsdLen = GetMsInfo(ms,RsdBuf,RsdLen);

        }
        break;

    case 6://选择方法6
        break;

    case 7://选择方法7
        {
            RsdBuf[RsdLen++] = 7;    //选择方法2


            memcpy_r((uint8*)&TempTimeBcd, (uint8*)RunTask->LastTurnBeginTime, 7);//起始时间

            //曲线类冻结不需要加1秒
            if(pTmp_NormalLink->CsdTask.RoadTask[0].MainOad.nObjID < 0x5000 || pTmp_NormalLink->CsdTask.RoadTask[0].MainOad.nObjID > 0x5003)
                TempTimeSec = TempTimeSec+60;
            else
                TempTimeSec = StartTime;
            DT_Time2DateTimeHex(TempTimeSec, (DateTimeHex_t*)(RsdBuf+RsdLen));
            RsdLen += 7;

            memcpy_r((uint8*)&TempTimeBcd,(uint8*)RunTask->ThisTurnBeginTime,7);//结束时间

            DT_DateTimeBCD2Time(&TempTimeBcd,&TempTimeSec);
            DelayTi.value = pTmp_NormalLink->DelayTime;
            DelayTi.unit = pTmp_NormalLink->DelayTimeUnit;
            DT_TimeSubInv(&TempTimeSec, DelayTi);//减去任务开始时间里包含的延时，这是抄表和任务实际启动时间无关
            //曲线类冻结不需要加1秒
            if(pTmp_NormalLink->CsdTask.RoadTask[0].MainOad.nObjID< 0x5000 || pTmp_NormalLink->CsdTask.RoadTask[0].MainOad.nObjID > 0x5003)
                TempTimeSec = TempTimeSec+60;
            else
            {
                TempTimeSec = StartTime;
                DelayTi.value = pTmp_NormalLink->AcqInfo.TI.value;
                DelayTi.unit = pTmp_NormalLink->AcqInfo.TI.unit;
                DT_TimeAddInv(&TempTimeSec, DelayTi);//加上一个时标间隔，即只抄1个点
                TempTimeSec = TempTimeSec-1;  //部分表筛选方法是前闭后闭，-1秒强制为前闭后开
            }
            DT_Time2DateTimeHex(TempTimeSec, (DateTimeHex_t*)(RsdBuf+RsdLen));
            RsdLen += 7;
            if(pTmp_NormalLink->SchemeType == EVENTPLAN)
            {
                RsdBuf[RsdLen++] = 0;
            }
            else
            {
                //数据间隔
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

    case 8://选择方法8
        break;

    case 9://选择方法9
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
* 函数名称: taskmng_meter_prtl_645_pack
* 函数功能: 电表规约组帧
* 输入参数: MeterData           电表规约数据
* 输出参数: PrtlData            组帧后数据
* 返 回 值:  帧长度
*******************************************************************************/
int taskmng_meter_prtl_645_pack(uint8 *PrtlData, MeterPrtlFmt_T MeterData)
{
    uint8   CheckSum = 0;                       //校验和
    int     i;
    uint8   CurIndex;                           //索引

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
* 函数名称: taskmng_paral_meter_read_data_unpack
* 函数功能: 解上行的645帧
* 输入参数: AppData
* 输出参数: MeterPrtlData
* 返 回 值: 0   含有用帧(数据长度不为0的)
            -1  无
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
    //645解帧处理
    //TASK_BUF_DEBUG(Pipe, REC_LOG,AmrReportInfo.pDataBuf,AmrReportInfo.nDataBufLen);
    gReponsePIIDCAD = AmrReportInfo.pDataBuf[16];
    //TASK_FMT_DEBUG(Pipe, REC_LOG,"gReponsePIIDCAD is  %d \n",gReponsePIIDCAD);
    //tsa=taskmng_tsa_get(FrameSeqInfo.Pn,logicid);
    ret = taskmng_645_meter_data_unpack(AmrReportInfo.nDataBufLen, AmrReportInfo.pDataBuf
        , MeterPrtlData->MtrAddr, &MeterPrtlData->CtrlCode, &MeterPrtlData->DataLen, MeterPrtlData->DataBuff);
    //如果查找645帧格式失败,那么按照698.45表协议进行解析
    if(CHECK_RESULT_FAILUER == ret)
    {
        ret = LinkDataReceive(oop_pipe_get(Pipe,seqindex), AmrReportInfo.pDataBuf, AmrReportInfo.nDataBufLen,NULL);
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
        //检测出645报文后继续进行检测
        MeterPrtlData->frameinfo[framenum].result = ret;
        MeterPrtlData->frameinfo[framenum].len = MeterPrtlData->DataLen;
        memcpy(MeterPrtlData->frameinfo[framenum].buf,MeterPrtlData->DataBuff,MeterPrtlData->DataLen);
        framenum++;
        framelen = MeterPrtlData->DataLen+12;   //645报文总长度
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
* 函数名称: taskmng_paral_meter_read_data_unpack
* 函数功能: 解上行的645帧
* 输入参数: AppData
* 输出参数: MeterPrtlData
* 返 回 值: 0   含有用帧(数据长度不为0的)
            -1  无
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
    //645解帧处理
    TASK_BUF_DEBUG(Pipe, REC_LOG,AmrReportInfo.pDataBuf,AmrReportInfo.nDataBufLen);
    gReponsePIIDCAD = AmrReportInfo.pDataBuf[16];
    TASK_FMT_DEBUG(Pipe, REC_LOG,"gReponsePIIDCAD is  %d \n",gReponsePIIDCAD);
    //tsa=taskmng_tsa_get(FrameSeqInfo.Pn,logicid);
    ret = taskmng_645_meter_data_unpack(AmrReportInfo.nDataBufLen, AmrReportInfo.pDataBuf
        , MeterPrtlData->MtrAddr, &MeterPrtlData->CtrlCode, &MeterPrtlData->DataLen, MeterPrtlData->DataBuff);

    //如果查找645帧格式失败,那么按照698.45表协议进行解析
    if(CHECK_RESULT_FAILUER == ret)
    {
        ret = LinkDataReceive(oop_pipe_get(Pipe,seqindex), AmrReportInfo.pDataBuf, AmrReportInfo.nDataBufLen,NULL);
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
* 函数名称: taskmng_645_meter_data_unpack
* 函数功能: 电表规约解帧
* 输入参数: FrameLen            帧长度
*           FrameBuff           帧数据
* 输出参数: MtrAddr             电表地址
*           DataLen             数据域长度
*           DataBuff            数据域内容
*           CtrlCode            控制码
* 返 回 值: CHECK_RESULT_SUCCEED
*           CHECK_RESULT_FAILUER
*           CHECK_RESULT_CONFIRM
*           CHECK_RESULT_DENY
*******************************************************************************/
int8 taskmng_645_meter_data_unpack(uint16 FrameLen, uint8 *FrameBuff, uint8 *MtrAddr , uint8 *CtrlCode, uint16 *DataLen, uint8 *DataBuff)
{
    uint16  DataIndex = 0xff;                   //指针偏移
    uint8   CS = 0;
    uint16   i;//start Common Problem

    //滑动偏移，定位帧头
    for (i = 0; i < FrameLen; i++)
    {
        if ((FrameBuff[i] == 0x68) && (FrameBuff[i+7] == 0x68))
        {
            DataIndex = i;
            break;
        }
    }

    //未定位到帧头，判为失败
    if (DataIndex == 0xff)
    {
        return CHECK_RESULT_FAILUER;
    }

    //刷新地址域
    memcpy(MtrAddr, &FrameBuff[DataIndex+1], 6);

    //控制码
    *CtrlCode = FrameBuff[DataIndex+8];

    //刷新数据长度
    *DataLen = FrameBuff[DataIndex+9];

    //判断帧尾
    if (FrameBuff[(DataIndex + *DataLen + 11)] != 0x16)
    {
        return CHECK_RESULT_FAILUER;
    }

    //判断校验和
    for (i=0; i < (*DataLen + 10); i++)
    {
        CS += FrameBuff[DataIndex+i];
    }
    if (CS != FrameBuff[(DataIndex + *DataLen + 10)])
    {
        return CHECK_RESULT_FAILUER;
    }

    //刷新数据域内容，同时做余3码处理
    for (i = 0; i < *DataLen; i++)
    {
        DataBuff[i] = FrameBuff[DataIndex+10+i] - 0x33;
    }


    //判断是否是否认帧
    if (*CtrlCode & 0x40)
    {
        return CHECK_RESULT_DENY;
    }

    //判断是否是确认帧
    if (*DataLen == 0)
    {
        return CHECK_RESULT_CONFIRM;
    }
    if(*CtrlCode & 0x20) //有后续帧
    {
        AddEventReport(MtrAddr);
    }
    return CHECK_RESULT_SUCCEED;
}





/*******************************************************************************
* 函数名称: taskmng_645_meter_data_unpack
* 函数功能: 电表规约解帧
* 输入参数: FrameLen            帧长度
*           FrameBuff           帧数据
* 输出参数: MtrAddr             电表地址
*           DataLen             数据域长度
*           DataBuff            数据域内容
*           CtrlCode            控制码
* 返 回 值: CHECK_RESULT_SUCCEED
*           CHECK_RESULT_FAILUER
*           CHECK_RESULT_CONFIRM
*           CHECK_RESULT_DENY
*******************************************************************************/
int8 taskmng_188_meter_data_unpack(uint16 FrameLen, uint8 *FrameBuff, uint8 *MtrAddr , uint8 *CtrlCode, uint16 *DataLen, uint8 *DataBuff,uint16 DI)
{

    uint16  head_offset = 0; /* 数据索引 */
    uint8   CS = 0;
    uint8   i;
    uint8   data_len = 0;


    while((head_offset + AMR_MT_PRTL_MIN_LEN_188 - 1) < FrameLen)
    {
        /* 1 判帧开始标志0x68和水表类型*/
        if ((FrameBuff[head_offset] != 0x68))
        {
            head_offset ++;
            continue;
        }
        /* 2 判帧长度 */
        if((FrameLen - head_offset) < AMR_MT_PRTL_MIN_LEN_188)
        {
            //memset(FrameBuff, 0, FrameLen);
            return AMR_CHECK_RESULT_FAILUER_188;
        }

        data_len =  FrameBuff[head_offset + 10];         //188规约数据域长度 在帧开始标志后第10个字节

        /* 3 判结尾0x16 */
        if (FrameBuff[head_offset + data_len + 12] != 0x16)
        {
            head_offset ++;
            continue;
        }

        /* 4 判校验和 */
        for (i=0; i < (data_len + 11); i++)
        {
            CS += FrameBuff[head_offset+i];
        }
        if (CS != FrameBuff[head_offset + data_len + 11])
        {
            //memset(FrameBuff, 0, FrameLen);
            return AMR_CHECK_RESULT_FAILUER_188;
        }

        /* 6 走到这里说明帧已经正确 */

        /* 6.1 刷新地址域 */
         //刷新地址域
        memcpy(MtrAddr, &FrameBuff[head_offset+2], 7);


        /* 6.2 刷新控制码*/
        *CtrlCode = FrameBuff[head_offset+9];
        if((*CtrlCode & 0x80) == 0x00)
        {
            /* DataIndex + 数据长度 + 规约68到LEN的长度 + CS和16 */
            head_offset = head_offset + data_len + 12 + 1;
            CS = 0;
            continue;
        }

        /* 6.5 判断是否是 否认帧*/
        if(*CtrlCode & 0x40)
        {
            //memset(FrameBuff,0,FrameLen);
            return AMR_CHECK_RESULT_DENY_188;
        }

        /* 6.5 普通抄表任务判断发送和接受的帧标识是否相同 如果不同按此帧失败处理*/
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
名称： bin2_int32u

函数功能:将1字节转成4字节；

函数输入值:
函数返回值:无
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
* 函数名称: taskmng_mtr_result_check
* 函数功能: //检查每个任务中index对应的表是否抄读成功
* 输入参数: index 电表索引
        MtrResult  记录电表抄读结果数据
* 输出参数:
* 返 回 值: TRUE 该表抄成功
        FALSE   该表抄失败
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
* 函数名称: taskmng_mtr_result_check
* 函数功能: //检查每个任务中index对应的表是否抄读成功
* 输入参数: index 电表索引
        MtrResult  记录电表抄读结果数据
* 输出参数:
* 返 回 值: TRUE 该表抄成功
        FALSE   该表抄失败
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
* 函数名称: taskmng_mtr_result_set
* 函数功能: //置指定索引位置为抄表成功
* 输入参数: index 电表索引
        MtrResult  记录电表抄读结果数据
* 输出参数:
* 返 回 值:
*******************************************************************************/
void taskmng_mtr_result_set(uint16 index,uint8 *MtrResult)
{
    uint8 i,j;
    i = index/8;
    j = index%8;
    MtrResult[i]|=(1<<j);
}





/*******************************************************************************
* 函数名称: CreatPatchFile
* 函数功能: //创建补抄文件
* 输入参数: Pipe 端口号
        DayCnt 为0表示当天的文件 1表示上一天的 最多三天
* 输出参数:
* 返 回 值:
*******************************************************************************/
void CreatPatchFile(uint8 Pipe,uint8 DayCnt)
{

    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[Pipe];
    uint8 i;

    //pthread_rwlock_rdlock(&TaskRWLock);
    for(i=0;i<pAmrThreadArg->TaskNum;i++)
    {

        if(SchemeTaskRunQueue[i].SchemeType ==EVENTPLAN)  //事件不补抄
            continue;

        //补抄冻结数据
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
* 函数名称: ReOrderPatchFile
* 函数功能: //更新补抄文件中时标
* 输入参数: Pipe 端口号

* 输出参数:
* 返 回 值:
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

    TASK_FMT_TRACE(Pipe, REC_LOG, "ReOrderPatchFile 更新补抄文件！\n");
       //每个任务更新
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
            TASK_FMT_TRACE(Pipe, REC_LOG, "补抄文件 %s \n",pfile);
            if(access(pfile,R_OK)==0)
            {
                //sprintf(cmdbuf,"mv /mnt/data/private/%s%03d_%02d_PIPE%02d /mnt/data/private/%s%03d_%02d_PIPE%02d",PATCH_FILE,TaskId,DayCnt,Pipe,PATCH_FILE_TMP,TaskId,DayCnt,Pipe);
                sprintf(pfiletmppath,"%s/%s%03d_%02d_PIPE%02d",PrivateDir,PATCH_FILE_TMP,TaskId,DayCnt,Pipe);
                //system(cmdbuf);
                rename(pfile,pfiletmppath);
                TASK_FMT_TRACE(Pipe, REC_LOG, "文件存在 暂时改名 \n");
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
                    TASK_FMT_TRACE(Pipe, REC_LOG, "%s 重命名为 %s\n",pfiletmp,pfiletmppath);
                }else
                {
                    TASK_FMT_TRACE(Pipe, REC_LOG, "删除补抄文件 %s\n",pfiletmp);
                    remove(pfiletmp);
                }
            }
        }
    }

}



/*******************************************************************************
* 函数名称: patchfile_meter_check
* 函数功能: //检查 补抄文件中电表是否正确
* 输入参数: Pipe 端口号
       taskid 任务号
       daycnt 上多少日补抄文件
* 输出参数:
* 返 回 值:
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
    TASK_FMT_TRACE(pipe, REC_LOG, "更新补抄文件成功\n");
}


/*******************************************************************************
* 函数名称: CheckPatchFile
* 函数功能: //检查前三天补抄文件是否存在 没有则创建
* 输入参数: Pipe 端口号

* 输出参数:
* 返 回 值:
*******************************************************************************/
void CheckPatchFile(uint8 Pipe)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[Pipe];
    char      pfile[100]={0};
    uint8 i;
    uint8 TaskId;
    uint8 DayCnt=0;
    TASK_FMT_TRACE(Pipe, REC_LOG, "CheckPatchFile   检查补抄文件是否存在\n");
    //pthread_rwlock_rdlock(&TaskRWLock);
    for(i=0;i<pAmrThreadArg->TaskNum;i++)
    {
        if(pAmrThreadArg->pTaskRunInfo[i].logicid!=LOGIC_ID_YC)
            continue;
        if(SchemeTaskRunQueue[i].SchemeType ==EVENTPLAN)  //事件不补抄
            continue;
        DayCnt = 0;
        while(DayCnt<PATCH_MAX_DAY)
        {

            TaskId = pAmrThreadArg->pTaskRunInfo[i].TaskId;
            memset(pfile,0x00,sizeof(pfile));
            sprintf(pfile,"%s/%s%03d_%02d_PIPE%02d",PrivateDir,PATCH_FILE,TaskId,DayCnt,Pipe);

            if(access(pfile,R_OK)==0)        //如果该任务的补抄文件存在则下一个
            {
                TASK_FMT_TRACE(Pipe, REC_LOG, "补抄文件%s 存在\n",pfile);
                PATCH_TASK_T PatchTask;
                memset(&PatchTask,0x00,sizeof(PATCH_TASK_T));
                read_pdata_xram(pfile,(char*)&PatchTask,0,sizeof(PATCH_TASK_T));
                //检查补抄文件版本
                if(strcmp(PatchTask.FileVersion,PATCH_FILE_VERSION)!=0)
                {
                    TASK_FMT_TRACE(Pipe, REC_LOG, "补抄文件版本不对 删除\n",PatchTask.Time[2],PatchTask.Time[1],PatchTask.Time[0],PatchTask.TimeNum);
                    remove(pfile);
                }else
                {
//                    TASK_FMT_TRACE(Pipe, REC_LOG, "补抄时间 %02x %02x %02x timenum %d \n",PatchTask.Time[2],PatchTask.Time[1],PatchTask.Time[0],PatchTask.TimeNum);
//                    uint8 j;
//                    uint16 k;
//                    for(j=0;j<PatchTask.TimeNum;j++)
//                    {
//                        TASK_FMT_TRACE(Pipe, REC_LOG, "补抄时间点 %ld \n",PatchTask.TimeInfo[j].PatchTime);
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
            TASK_FMT_TRACE(Pipe, REC_LOG, "补抄文件%s 不存在\n",pfile);
            //TASK_FMT_TRACE(Pipe, REC_LOG, "taskid %d tasktype %d mainoad %04x \n",TaskId,SchemeTaskRunQueue[i].SchemeType ,SchemeTaskRunQueue[i].CsdTask.RoadTask[0].MainOad.nObjID);

            //补抄冻结数据
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
* 函数名称: CreatPatchFile_TaskId
* 函数功能: 创建补抄文件
* 输入参数: Pipe 端口号
       Tmp_AmrLink 任务参数
       DayCnt  上几天的
* 输出参数:
* 返 回 值:
*******************************************************************************/
void CreatPatchFile_TaskId(uint8 Pipe,SCHEME_TASK_RUN_INFO *Tmp_AmrLink,uint8 DayCnt)
{
    //AmrThreadArg_T* pAmrThreadArg;
    //pAmrThreadArg = &gAmrThreadArg[Pipe];
    //485暂时不需要补抄文件
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
    //以实时方式抄读 不补抄
    if(Tmp_AmrLink->AcqInfo.Acqtype == ACQTYP_NULL)
    {
        return;
    }
    if(GetTaskState(Tmp_AmrLink->TaskId,Tmp_AmrLink->logicid)!=1)
        return ;
    uint32 period = taskmng_task_period_get(Tmp_AmrLink->TaskId,Tmp_AmrLink->logicid);
     //运行周期小于15分钟任务不补抄
    if(period<15*60)
        return;
     TASK_FMT_TRACE(Pipe, REC_LOG, "创建补抄文件%s ！！！\n",pfile);
    tmpPatchTime = GetDaycntTime( DayCnt);
    DT_Time2DateTimeBCD(tmpPatchTime,&tmpTime);
    day = tmpTime.day;
    TaskInfo.Time[0] = tmpTime.day;
    TaskInfo.Time[1] = tmpTime.month;
    TaskInfo.Time[2] = tmpTime.year_l;
    strcpy(TaskInfo.FileVersion,PATCH_FILE_VERSION);
    MeterNum = 0;

    //获取当前任务在当前线程需要抄读的电表
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
    TASK_FMT_TRACE(Pipe, REC_LOG, "写补抄文件内容 taskid %d 年月日 %02x %02x %02x TimeNum %d ret %d \n",TaskInfo.TaskId,TaskInfo.Time[0],TaskInfo.Time[1],TaskInfo.Time[2], TaskInfo.TimeNum,ret);

}

/*******************************************************************************
* 函数名称: get_task_run_frequence
* 函数功能: 获取任务执行频率
* 输入参数: TaskId 任务号

* 输出参数:  Ti 任务执行频率
* 返 回 值:
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
* 函数名称: taskmng_prtl_isvalid_check
* 函数功能: 判断档案协议是否支持
* 输入参数: 协议号

* 输出参数:
* 返 回 值: TRUE 表示支持该规约;FALSE不支持
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
* 函数名称: taskmng_698_addr_get
* 函数功能: 获取电表地址
* 输入参数: inframe 报文内容
          inLen 消息长度
* 输出参数:  Addr 电表地址
* 返 回 值:
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
* 函数名称: up_msg_response
* 函数功能: //上行回复bool
* 输入参数: MsgInfo 消息内容
            result 回复的消息
* 输出参数:
* 返 回 值:
*******************************************************************************/
void up_msg_response(MESSAGE_INFO_T* MsgInfo ,uint8 result)
{
    MESSAGE_INFO_T* MsgSend=NULL;
    //uint8 result = TRUE;
    TASK_FMT_DEBUG(-1, UP_LOG, "组上行回复报文result %d \n",result);
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
* 函数名称: GetNextRelayDI
* 函数功能: 获取0x0010组帧穿透要抄读的数据项
* 输入参数:
* 输出参数: DiInfo
* 返 回 值:
*           1                  存在需要抄读的数据
*           0                  全部抄完
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
        TASK_FMT_DEBUG( pipe, RELAY_LOG, "taskmng_next_relay_DI_get 当前电表index %d prtl %d \n",i,*prtl);
        TASK_FMT_DEBUG( pipe, RELAY_LOG, "地址 0x%02x%02x%02x%02x%02x%02x \n",Addr[5],Addr[4],Addr[3],Addr[2],Addr[1],Addr[0]);
        if(*prtl == PRTL_NULL)
            continue;
        if(pAmrThreadArg->RelayTrans.OadIndex == 0&&pAmrThreadArg->RelayTrans.MapIndex == 0)
        {
            pAmrThreadArg->RelayTrans.AmrNormal[i].SendTime = time(NULL);
            TASK_FMT_DEBUG( pipe, RELAY_LOG, "该表第一次发送时间 %ld \n",pAmrThreadArg->RelayTrans.AmrNormal[i].SendTime);
        }
        if(*prtl == PRTL_OOP)
        {
            ret = Get485OopRelayOad(pipe,i,DiInfo);
            if(ret >0)
            {
                TASK_FMT_DEBUG( pipe, RELAY_LOG, "面向对象表穿透 SubOadNum %d \n",DiInfo->SubOadNum);
                TASK_FMT_DEBUG( pipe, RELAY_LOG, "面向对象表穿透 DI is %x\n",DiInfo->SubOad[0].value);
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
                TASK_FMT_DEBUG( pipe, RELAY_LOG, "645表穿透 DI 0X%02x%02x%02x%02x \n",DiInfo->DI[3],DiInfo->DI[2],DiInfo->DI[1],DiInfo->DI[0]);
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
* 函数名称: GetNext485SetRelayData
* 函数功能: 获取0x0010组帧穿透要抄读的数据项
* 输入参数:
* 输出参数: DiInfo
* 返 回 值:
*           1                  存在需要抄读的数据
*           0                  全部抄完
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
* 函数名称: GetOopRelayOad
* 函数功能: 获取组帧穿透面向对象表需要抄读的Oad
* 输入参数:Pipe
        MtrIndex 穿透中电表索引
* 输出参数:DiInfo

* 返 回 值: 0    当前表已抄读完成
    Oad个数
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
    TASK_FMT_DEBUG( pipe, RELAY_LOG, "GetOopRelayOad 当前oadindex %d  \n",pAmrThreadArg->RelayTrans.OadIndex );
      //判断安全模式是否启用
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
//      //MtrSecModEn=0;//测试使用
//      pAmrThreadArg->RelayTrans.AmrNormal[MtrIndex].IsNeedGetSafeMode = FALSE;
//      Set_MtrSecModEn(pipe,MtrSecModEn);  //设置安全模式
//  }
    if(pAmrThreadArg->RelayTrans.OadIndex >= pAmrThreadArg->RelayTrans.AmrNormal[MtrIndex].OadNum )
    {
        pAmrThreadArg->RelayTrans.OadIndex = 0;
        pAmrThreadArg->RelayTrans.MapIndex = 0;
        TASK_FMT_DEBUG( pipe, RELAY_LOG, "taskmng_oop_relay_oad_get 当前电表穿透完成  \n" );
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
* 函数名称: MakeRelay485Data_0010
* 函数功能: 485代理组抄表帧
* 输入参数: pipe 端口号
        DiInfo 数据项
        prtl  规约类型
* 输出参数:  DataBuff 报文内容
* 返 回 值:  报文长度
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
    MeterPrtlFmt_T  MeterPrtlData;                      //电表规约格式数据

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
* 函数名称: UnpackRelay485Data_0010
* 函数功能: //485解穿透回码
* 输入参数: pipe 端口号
        DataBuff 报文内容
        DataLen  报文长度
* 输出参数:  MeterPrtlData 645表解出来的内容
        apdu 698表解出来的apdu
* 返 回 值:  -1 失败 其他成功
*******************************************************************************/
int UnpackRelay485Data_0010(uint8 pipe,uint8* DataBuff,uint16 DataLen,MeterPrtlFmt_T*  MeterPrtlData,char* apdu)
{
    int ret = -1;
    AppLayerList_t* pAppLayer = NULL;
    ret = taskmng_645_meter_data_unpack(DataLen,DataBuff, MeterPrtlData->MtrAddr, &MeterPrtlData->CtrlCode, &MeterPrtlData->DataLen, MeterPrtlData->DataBuff);
    if(ret == CHECK_RESULT_FAILUER)
    {
        ret = LinkDataReceive(pipe, DataBuff, DataLen,NULL);
        TASK_FMT_DEBUG(pipe, RELAY_LOG,"LinkDataReceive 698解帧返回值 %d \n",ret);
        if(ret >=eOopComm_Fail)//解帧失败直接返回
            return -1;
        //获取电表地址
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
* 函数名称: PatchDataReading
* 函数功能:补抄任务 按照一天的任务进行补抄 优先三天前 按照任务优先级抄 当天所有抄完后抄下一天
* 输入参数: 无
* 输出参数: 无
* 返 回 值: >0                  成功
*           <0                  失败
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
    uint8           PayloadBuf[MAX_MSG_LEN] = {0};        //payload缓冲区
    AMR_PLC_INFO_REPORT_T  AmrReportInfo;    //上报的抄读数据
    MeterPrtlFmt_T  MeterPrtlData;



    uint8           SendIndex =0;
    uint16          MtrIndex = 0;
    char            apdu[2];
    //uint16 TaskId;
    int             OverTimeCnt = 100;
    static SCHEME_TASK_RUN_INFO TaskInfo[PLC_NUM];
    static uint8 lastday[PLC_NUM];
    if(pAmrThreadArg->TaskModifyFlag!=TASK_CFG_INITED)
        return 1;//任务未初始化 结束
    if((pAmrThreadArg->PatchTask.TaskId !=0) && (lastday[pipe]!=gCurBcdTime[pipe].Day))
    {
        lastday[pipe] = gCurBcdTime[pipe].Day;
        //过日需要重新加载补抄文件
        TASK_FMT_TRACE( pipe, REC_LOG, "过日 补抄结束 \n");

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
            TASK_FMT_TRACE( pipe, REC_LOG, "补抄任务 %d NeedMtrNum %d \n",pAmrThreadArg->PatchRunTask.TaskId,pAmrThreadArg->PatchRunTask.NeedMtrNum);

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
            TASK_FMT_TRACE( pipe, REC_LOG, "补抄当前任务结束 DayCnt %d \n",DayCnt[pipe]);
            //WritePatchResult(pipe,pAmrThreadArg->PatchTask.TaskId,(uint8)DayCnt[pipe]);

            pAmrThreadArg->TotalState.PatchAmr = RTS_IDLE;
            //不能直接return-1 这样在没有补抄任务的时候会一直刷日志 现在补抄到了另外一天就停下来 一分钟一次
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
            //不做处理
            }
                break;
            case 0x0031:
            {
                //TASK_FMT_TRACE(pipe, REC_LOG,"@@@@@收到抄表上报数据@@@@@@@@@@ \n");
                if(taskmng_amr_report_data_unpack(PayloadBuf,MsgInfo->msglen,&AmrReportInfo)==FALSE)
                    break;
                //TASK_FMT_TRACE(pipe, REC_LOG,"解消息成功 获取发送队列索引 \n");
                if(taskmng_seq_is_valid(pipe,&SendIndex,AmrReportInfo.addr)==0)
                {
                    TASK_FMT_TRACE(pipe, REC_LOG,"taskmng_seq_is_valid 未找到 \n");
                    break;
                }
                //正常抄表任务切换时如果队列有发送报文 则会切到补抄任务 此时收到的回码放到这里面处理
                //直接接收 并将发送队列置位无效 这样收到所有的时候会自动切到正常抄表任务
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
                //TASK_FMT_TRACE(pipe, REC_LOG,"ret %d 电表号 %d prtl %d \n ",ret,AmrReportInfo.nMeterFilesID,MeterPrtlData.PrtlMode);
                if(ret<0)
                {
                    RefreshPatchPnState(pipe,SendIndex);
                    pAmrThreadArg->TotalState.PatchAmr = RTS_SEND;
                    break;
                }
                //pAmrThreadArg->PatchRunTask.TaskId;
                //TASK_FMT_TRACE( pipe, REC_LOG,"收到上报数据 taskid %d \n",TaskId);
                if(AmrReportInfo.nProtocolType==PRTL_OOP && ret == eOopComm_SegRecv)
                {
                    TASK_FMT_TRACE( pipe, REC_LOG,"收到链路分帧数据\n");
                    pAmrThreadArg->AmrSendFrameInfo.Queue[SendIndex].NeedSendSeg=1;
                    pAmrThreadArg->TotalState.PatchAmr = RTS_SEND;
                    break;
                }
                if(MeterPrtlData.PrtlMode == PRTL_OOP)
                {
                    if(ret ==eOopComm_GetNext )
                    {
                        TASK_FMT_TRACE( pipe, REC_LOG,"收到应用分帧数据 继续解析\n");
                        pAmrThreadArg->AmrSendFrameInfo.Queue[SendIndex].NeedSendSeg=1;
                        OopPatchMeterAmrDataSave_Record(pipe,MeterPrtlData.DataBuff,MeterPrtlData.DataLen,MtrIndex,&TaskInfo[pipe],1);
                        pAmrThreadArg->TotalState.PatchAmr = RTS_SEND;
                        break;
                    }
                    //补抄全部按照记录型
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
                            TASK_FMT_TRACE(pipe, REC_LOG,"645验证成功 DataLen %d\n",MeterPrtlData.DataLen);

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
                        TASK_FMT_TRACE( pipe, REC_LOG, "Queue index %d 超时 PN %d \n",i,pAmrThreadArg->AmrSendFrameInfo.Queue[i].Pn);
                        RefreshPatchPnState(pipe,i);
                        pAmrThreadArg->TotalState.PatchAmr = RTS_SEND;
                    }
                }
            }
            //如果补抄的发送队列里没有有效的就切换到发送 因为和正常抄表共用一个队列
            if(flag==0)
            {
                if(pAmrThreadArg->SendQueueFlag>0)
                {
                    TASK_FMT_TRACE( pipe, REC_LOG, "本次补抄结束 \n");
                    if(pAmrThreadArg->AmrSendFrameInfo.usetype != 1)
                        pAmrThreadArg->TotalState.PatchAmr = RTS_IDLE;
                    //此处返回2配合taskmng_plc_task_run中进行判断 直接再次创建正常抄表任务
                    return 2;
                }
                TASK_FMT_TRACE( pipe, REC_LOG, "补抄发送队列无效 切换到发送 \n");
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
* 函数名称: GetPatchTaskInfo
* 函数功能:获取当前需要补抄的任务
* 输入参数: pipe daycnt
* 输出参数: tmptask
* 返 回 值: >0                  获取当日补抄任务
*           <0                  该日任务全部补抄
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
        TASK_FMT_TRACE( pipe, REC_LOG, "获取到补抄任务 taskid %d daycnt %d timenum %d NeedMtrNum %d！\n",taskid,daycnt,pAmrThreadArg->PatchTask.TimeNum,tmptask->NeedMtrNum);
        TASK_FMT_TRACE( pipe, REC_LOG, "时间 %02x %02x %02x   \n",pAmrThreadArg->PatchTask.Time[2],pAmrThreadArg->PatchTask.Time[1],pAmrThreadArg->PatchTask.Time[0]);
//        uint8 i,j;
//        for(i=0;i<pAmrThreadArg->PatchTask.TimeNum;i++)
//        {
//            for(j=0;j<pAmrThreadArg->PatchTask.TimeInfo[i].MeterNum;j++)
//            {
//                TASK_FMT_TRACE( pipe, REC_LOG, "补抄时间点 %d pn %d flag %d \n",i,pAmrThreadArg->PatchTask.TimeInfo[i].MeterInfo[j].Pn,pAmrThreadArg->PatchTask.TimeInfo[i].MeterInfo[j].flag);
//            }
//        }
        *patchtask = tmptask;
       // TASK_FMT_TRACE( pipe, REC_LOG, "patchtask taskid %d NeedMtrNum %d ！\n",(*patchtask)->TaskId,(*patchtask)->NeedMtrNum);

        return 1;
    }
    *patchtask = tmptask;
    return -1;     //当日全补抄过
}

/*******************************************************************************
* 函数名称: PatchAmrMakeFrame
* 函数功能: 补抄组抄表帧
* 输入参数: pipe 端口号

* 输出参数:
* 返 回 值:
*******************************************************************************/
int PatchAmrMakeFrame(uint8 pipe,int8 daycnt)
{
    uint8 i;
    int ret ;
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    SCHEME_TASK_RUN_INFO TaskInfo;
    int          MsIndex;    //当前线程中的索引
    DI_INFO_T    TmpDiInfo;
    uint16 Pn;
    uint8 prtl;
    uint8 MtrAddr[6];
    uint16       FrameLen = 0;
    uint8        FrameBuff[MAX_MSG_LEN];  //帧数据缓冲区
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
                //TASK_FMT_TRACE( pipe, REC_LOG,"没有找到要抄的表 \n");
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
                TASK_FMT_TRACE( pipe, REC_LOG,"Pn %d 发送分帧确认帧 \n",Pn);
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
            index+=FrameLen;          //主动抄表PayLoad长度

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
* 函数名称: GetNextPatchDI
* 函数功能:获取当前需要补抄的测点数据标识
* 输入参数: pipe TaskInfo
* 输出参数: DiInfo MsIndex
* 返 回 值: >0                  存在需要抄读的数据
*           <0                  全部抄完
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
        patchTime = pAmrThreadArg->PatchTask.TimeInfo[i];//该任务在本线程需要抄读的电表
        //只补抄到当前的上一轮
        //PatchTime已经是上一轮开始时间 没带延时的   如果只减一个执行间隔会导致本轮任务还没执行就先补抄了
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
                //TASK_FMT_TRACE( pipe, REC_LOG,"任务 %d 时间点 %ld 补抄完成 \n",pAmrThreadArg->PatchRunTask.TaskId,patchTime.PatchTime);
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
                //TASK_FMT_TRACE( pipe, REC_LOG,"该表全部抄完 无需补抄 PN %d \n",Pn);

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
                //TASK_FMT_TRACE( pipe, REC_LOG,"该表 %d 以实时方式抄读 不补抄 \n",Pn);
                taskmng_mtr_result_set(pAmrThreadArg->PatchRunTask.MeterIndex,pAmrThreadArg->PatchRunTask.MtrResult);
                //pAmrThreadArg->PatchTask.TimeInfo[i].MeterInfo[pAmrThreadArg->PatchRunTask.MeterIndex].flag =1;
                taskmng_mtr_result_set(pAmrThreadArg->PatchRunTask.MeterIndex, pAmrThreadArg->PatchTask.TimeInfo[i].flag);
                pAmrThreadArg->PatchRunTask.MeterIndex++;
                pAmrThreadArg->PatchRunTask.SucNum++;
                continue;
            }
            if(pAmrThreadArg->PatchMtrInfo[*MsIndex].FailCnt>=MAX_METER_FAILURE_COUNT)
            {
                TASK_FMT_TRACE( pipe, REC_LOG,"该表失败次数 %d 不再抄读 PN %d \n",pAmrThreadArg->PatchMtrInfo[*MsIndex].FailCnt,Pn);
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
        //TASK_FMT_TRACE( pipe, REC_LOG,"任务 %d i %d 开始抄下一个时间点 %ld  \n",pAmrThreadArg->PatchRunTask.TaskId,i,pAmrThreadArg->PatchTask.TimeInfo[i].PatchTime);

        pAmrThreadArg->PatchRunTask.TimeIndex = i;
        patchTime = pAmrThreadArg->PatchTask.TimeInfo[i];
        cnt =0;
        pAmrThreadArg->PatchRunTask.SucNum = 0;
        pAmrThreadArg->PatchRunTask.NeedMtrNum =GetPatchNeedMtrNum(patchTime);
        if(pAmrThreadArg->PatchRunTask.NeedMtrNum>0)
        {
            TASK_FMT_TRACE( pipe, REC_LOG,"任务 %d i %d 开始抄下一个时间点 %ld NeedMtrNum %d \n",pAmrThreadArg->PatchRunTask.TaskId,i,pAmrThreadArg->PatchTask.TimeInfo[i].PatchTime,pAmrThreadArg->PatchRunTask.NeedMtrNum );
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
* 函数名称: GetPatchOopOad
* 函数功能:补抄 获取面向对象表需要抄读的Oad
* 输入参数:Pipe TaskInfo CurTask
* 输出参数:DiInfo
        MsIndex     当前线程中的电表索引
* 返 回 值: 0    当前表已抄读完成
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
                TASK_FMT_TRACE( Pipe, REC_LOG, " 当前补抄任务电表%d 抄读完成  \n",Pn );

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
            TASK_FMT_TRACE( Pipe, REC_LOG, " 当前补抄任务电表%d 抄读完成  \n",Pn );

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
* 函数名称: SearchPatch645DI
* 函数功能: 补抄在转换列表里找到与要转换的字典索引相同的信息。若有多条信息使用同一字典索引
*           会把每一条都找到并返回
* 输入参数: Tmp_AmrLink         方案任务节点
*           MtrIndex            抄表索引
*           PrtlMode            电表规约
*           DI                  97/07数据标识
* 输出参数:
*           DI                  找到的字典索引对应的97或07DI
* 返 回 值: =0                  该测点全部抄完
            >0                  在列表找到转换信息
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
        pAmrThreadArg->PatchRunTask.SucNum++;  //找不到默认成功

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
                {   //存在698和07一对一但是对应97多个的 会出现多条记录
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
                TASK_FMT_TRACE(Pipe, REC_LOG,"645找到对应DI OAD 0X%08x 当前CSDIndex %d  MapIndex %d\n",TaskInfo->CsdTask.RoadTask[0].OadListInfo[i].value,i,j);

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
    TASK_FMT_TRACE( Pipe, REC_LOG, " 当前任务电表%d 补抄完成  \n",pAmrThreadArg->PatchMtrInfo[MsIndex].Pn );
    return 0;

}



/*******************************************************************************
* 函数名称: IsNeedPatch
* 函数功能: //判断该任务中该表是否需要补抄
* 输入参数: Tmp_AmrLink 任务参数
        pipe 端口号
            MSindex 电表索引
* 输出参数:
* 返 回 值:   1需要补抄 0 不需要
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
//    case 0x0000:  //实时数据
//
//        ret = 0;
//        break;
//
//    case 0x5000:  //瞬时冻结
//
//        ret = 1;
//        break;
//
//    case 0x5002:  //分钟冻结(负荷曲线)
//        //先判断是否是单相表
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
//    case 0x5003:  //小时冻结
//        if(taskmng_task_period_get(Tmp_AmrLink->TaskId,Tmp_AmrLink->logicid) < 86400L)
//        {
//            ret = 0;
//        }
//        else      //周期大于1天仍通过负荷曲线抄读
//        {
//            ret = 1;
//        }
//        break;
//
//    case 0x5004:  //日冻结
//
//        ret = 1;
//        break;
//
//    case 0x5005:  //结算日
//    case 0x5006:  //月冻结
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
* 函数名称: InitPatchTask
* 函数功能: //初始化补抄任务
* 输入参数: pSchemeTaskRunInfo 任务参数
        PatchMtr 补抄电表信息
        tmpTask 补抄任务运行时参数
        PatchTime 补抄时间点
* 输出参数:
* 返 回 值:
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

            //将时标间隔转化为分钟单位
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
            //异常情况下使用默认值
            if(pSchemeTaskRunInfo->AcqInfo.TI.value==0)
            {
                pSchemeTaskRunInfo->AcqInfo.TI.unit = 1;
                pSchemeTaskRunInfo->AcqInfo.TI.value = 15;//默认15分钟
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
            //将时标间隔转化为分钟单位
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
            //异常情况下使用默认值
            if(pSchemeTaskRunInfo->AcqInfo.TI.value==0)
            {
                pSchemeTaskRunInfo->AcqInfo.TI.unit = 1;
                pSchemeTaskRunInfo->AcqInfo.TI.value = 15;//默认15分钟
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
* 函数名称: GetPatchCurNum
* 函数功能: //计算补抄时小时冻结和日冻结需要抄前多少次
* 输入参数:
        PatchTime 补抄时间点
        MainOI 小时冻结 日冻结
* 输出参数:
* 返 回 值:
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
* 函数名称: PatchMakeFrame
* 函数功能: //组补抄报文
* 输入参数:

* 输出参数:
* 返 回 值:
*******************************************************************************/
uint16 PatchMakeFrame(uint8 Pipe,uint16 seqindex,time_t StartTime,FRAME_SEQ_INFO_T FrameSeqInfo,uint8 *DataBuff,SCHEME_TASK_RUN_INFO *pTmp_NormalLink)
{

    MeterPrtlFmt_T  MeterPrtlData;                      //电表规约格式数据
    uint32          DI07 = 0;
    int             MsIndex;
    uint8           SubOadNum;
    DateTimeBcd_t   proStartTime;
    uint16          len = 0;

    MsIndex   = FrameSeqInfo.MsIndex;       //当前线程中的测量点索引
    SubOadNum = FrameSeqInfo.DiInfo.SubOadNum;



    MeterPrtlData.DelayFlag = 0;

    MeterPrtlData.PrtlMode = FrameSeqInfo.PrtlMode;      //通信规约
    //刷新采集器地址和电表地址
    memcpy(MeterPrtlData.MtrAddr, FrameSeqInfo.MeterAddr, 6);
    memcpy(MeterPrtlData.AcqUnitAddr, FrameSeqInfo.AcqAddr, 6);
    MeterPrtlData.MtrType = FrameSeqInfo.Mettype;

    if(MeterPrtlData.PrtlMode == PRTL_07)
    {
        MeterPrtlData.CtrlCode = 0x11;
        memcpy(&DI07,FrameSeqInfo.DiInfo.DI,4);
        if(DI07 == 0x06000001 || DI07 == 0x06010001 || DI07 == 0x06020001
                || DI07 == 0x06030001 || DI07 == 0x06040001 || (DI07&0xffff0000) == 0x06100000)     //如果点抄小时冻结数据块
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
                MeterPrtlData.DataBuff[4] = taskmng_curve_point_get(pTmp_NormalLink->TaskId,pTmp_NormalLink->logicid);  //一次抄读8个点
            DT_Time2DateTimeBCD( StartTime, &proStartTime);
            memcpy_r(&MeterPrtlData.DataBuff[5], &proStartTime.year_l, 5);
        }
    }

    if(MeterPrtlData.PrtlMode == PRTL_OOP)//面向对象电表协议的组帧
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
* 函数名称: OopPatchGetRequst
* 函数功能: //组698协议补抄报文
* 输入参数:

* 输出参数:
* 返 回 值:
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
    //抄负荷识别模块逻辑设备号用3
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
* 函数名称: OopPatchGetRequst
* 函数功能: //组698协议补抄报文
* 输入参数:

* 输出参数:
* 返 回 值:
*******************************************************************************/
int MakePatchRsdFrame(uint8 Pipe,uint8 *RsdBuf,SCHEME_TASK_RUN_INFO *pTmp_NormalLink,OOP_OAD_U RsdOad,int MsIndex,time_t StartTime)
{
    uint8                   RsdLen = 0;
    //DateTimeBcd_t           TempTimeBcd;
    time_t                  TempTimeSec;
    OOP_TI_T                    DelayTi;

    RsdBuf[RsdLen++] = 2;    //选择方法2

    //对象属性描述符  OAD
    RsdBuf[RsdLen++] = (uint8)(RsdOad.nObjID>> 8);
    RsdBuf[RsdLen++] = (uint8)(RsdOad.nObjID);
    RsdBuf[RsdLen++] = RsdOad.attID;
    RsdBuf[RsdLen++] = RsdOad.nIndex;

    RsdBuf[RsdLen++] = DATE_TIME_S;//起始时间数据类型


    TempTimeSec = StartTime;
    if(pTmp_NormalLink->CsdTask.RoadTask[0].MainOad.nObjID < 0x5000 || pTmp_NormalLink->CsdTask.RoadTask[0].MainOad.nObjID > 0x5003)
        TempTimeSec = TempTimeSec+60;
    else
        TempTimeSec = StartTime;
    DT_Time2DateTimeHex(TempTimeSec, (DateTimeHex_t*)(RsdBuf+RsdLen));
    RsdLen += 7;

    RsdBuf[RsdLen++] = DATE_TIME_S;//结束时间数据类型

    if(pTmp_NormalLink->CsdTask.RoadTask[0].MainOad.nObjID == 0x5004)
        TempTimeSec = TempTimeSec+taskmng_task_period_get(pTmp_NormalLink->TaskId,pTmp_NormalLink->logicid);
    else
    {
        TempTimeSec = StartTime;
        DelayTi.value = pTmp_NormalLink->AcqInfo.TI.value;
        DelayTi.unit = pTmp_NormalLink->AcqInfo.TI.unit;
        DT_TimeAddInv(&TempTimeSec, DelayTi);//加上一个时标间隔，即只抄1个点
        TempTimeSec = TempTimeSec-1;  //部分表筛选方法是前闭后闭，-1秒强制为前闭后开
    }
    DT_Time2DateTimeHex(TempTimeSec, (DateTimeHex_t*)(RsdBuf+RsdLen));
    RsdLen += 7;

    //数据间隔
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
* 函数名称: RefreshPatchPnState
* 函数功能: 更新补抄电表状态
* 输入参数: Pipe 端口号
        SeqIndex 发送索引

* 输出参数:
* 返 回 值:
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

    //失败次数加1
    if(pAmrThreadArg->PatchMtrInfo[MtrIndex].FailCnt < 0xff)
        pAmrThreadArg->PatchMtrInfo[MtrIndex].FailCnt++;

    printf("Sn = %d 抄表失败次数:%d\n",Pn,pAmrThreadArg->PatchMtrInfo[MtrIndex].FailCnt);

    return;
}

/*******************************************************************************
* 函数名称: JudgePatchMoveNextAmrTaskIndex
* 函数功能: 验证645补抄报文是否正确
* 输入参数: Pipe 端口号
        MsIndex 电表索引
        pTmp_NormalLink 任务信息
        pRecvDI  收到的数据项
        CtrlCode 控制码
* 输出参数:
* 返 回 值:   1 验证成功 -1 失败
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
        if((SendDI & 0xfff00000) == 0x05000000)    //冻结类发送时会修改DI
            SendDI = ((SendDI & 0xffffff00)|pAmrThreadArg->PatchMtrInfo[MsIndex].CurNum);
    }
    TASK_FMT_TRACE(Pipe, REC_LOG, "收到的DI %08x SendDI %08x \n",RecvDI,SendDI);
     //验证发送的数据标识与接收到的数据标识是否一致
    if(pRecvDI != NULL)
    {
        if((RecvDI != SendDI) || ((CtrlCode & 0x80) == 0))
            return -1;
    }
    else
    {
        printf("收到否认帧 \n");
    }
    return 1;
}

/*******************************************************************************
* 函数名称: JudgePatchFrz07MoveNext
* 函数功能: 根据结果进入下一个645补抄项
* 输入参数: Pipe 端口号
        MsIndex 电表索引
        Tmp_AmrLink 任务信息
        Result  补抄结果1成功 0失败

* 输出参数:
* 返 回 值:
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
                    //若抄读时标回否认，则不再抄读该任务后续数据
                    pAmrThreadArg->PatchMtrInfo[MsIndex].CSDIndex= 0xfe; //给一个较大的不等于0xff的索引，进入正常的任务完成流程
                    pAmrThreadArg->PatchMtrInfo[MsIndex].EndNum = 1;      //不再继续搜索时标
                    return;
                }
                if(pAmrThreadArg->PatchMtrInfo[MsIndex].storeTm.year_h==0)
                {
                    if(pAmrThreadArg->PatchMtrInfo[MsIndex].CurNum <= pAmrThreadArg->PatchMtrInfo[MsIndex].EndNum)
                    {
                        pAmrThreadArg->PatchMtrInfo[MsIndex].CSDIndex = 0;
                        pAmrThreadArg->PatchMtrInfo[MsIndex].MapIndex = 0;
                        pAmrThreadArg->PatchMtrInfo[MsIndex].CurNum++;//继续搜索下一条记录
                    }
                }
                if(Tmp_AmrLink->CsdTask.RoadTask[0].MainOad.nObjID == 0x5004 && pAmrThreadArg->PatchMtrInfo[MsIndex].EndNum == 0
                        &&pAmrThreadArg->PatchMtrInfo[MsIndex].CurNum < 2)
                        pAmrThreadArg->PatchMtrInfo[MsIndex].EndNum = 0xff;
            }
            else
            {
                //负荷曲线处理
                OOP_TI_T TI;
                TI.unit = Tmp_AmrLink->AcqInfo.TI.unit;
                TI.value = Tmp_AmrLink->AcqInfo.TI.value;

                //if(IsCnntStyleOne(Pn,pAmrThreadArg->PatchMtrInfo[MsIndex].logicid, Tmp_AmrLink) > 0)
                if(taskmng_mtr_conntype_get(Pn,pAmrThreadArg->pMtrAmrInfo[MsIndex].logicid)==1 && pAmrThreadArg->pMtrAmrInfo[MsIndex].flag645==0)
                {
                    pAmrThreadArg->PatchMtrInfo[MsIndex].CSDIndex++;    //抄读下一个曲线
                    pAmrThreadArg->PatchMtrInfo[MsIndex].MapIndex = 0;
                    TASK_FMT_TRACE( Pipe, REC_LOG, "补抄负荷曲线 csdindex %d mapindex %d \n",pAmrThreadArg->PatchMtrInfo[MsIndex].CSDIndex,pAmrThreadArg->PatchMtrInfo[MsIndex].MapIndex);

                    if(pAmrThreadArg->PatchMtrInfo[MsIndex].CSDIndex < Tmp_AmrLink->CsdTask.RoadTask[0].SubOadNum)
                        return;
                    TI.unit = TASK_CYCLE_FLAG_MIN;
                    TI.value = 15*taskmng_curve_point_get( Tmp_AmrLink->TaskId, Tmp_AmrLink->logicid);;    //一次15min*8个点
                }

                DT_TimeAddInv(&pAmrThreadArg->PatchMtrInfo[MsIndex].StartTime, TI);

                endTime = pAmrThreadArg->PatchMtrInfo[MsIndex].EndTime;

                //减去任务开始时间里包含的延时
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
                    pAmrThreadArg->PatchMtrInfo[MsIndex].CSDIndex = 0xfe; //给一个较大的不等于0xff的索引，进入正常的任务完成流程
                    pAmrThreadArg->PatchMtrInfo[MsIndex].MapIndex = 0;
                }
            }
        }
    }

}


/*******************************************************************************
* 函数名称: WritePatchResult
* 函数功能: //补抄任务完成后写回补抄结果
* 输入参数: Pipe 端口号
            taskid 任务号

* 输出参数:
* 返 回 值:
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
//            TASK_FMT_TRACE( pipe, REC_LOG, "补抄时间点 %d pn %d flag %d \n",i,pAmrThreadArg->PatchTask.TimeInfo[i].MeterInfo[j].Pn,pAmrThreadArg->PatchTask.TimeInfo[i].MeterInfo[j].flag);
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
* 函数名称: GetPatchNeedMtrNum
* 函数功能: //获取需要补抄的电表个数
* 输入参数: Pipe 端口号
            taskid 任务号

* 输出参数:
* 返 回 值:  需要补抄的电表个数
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
* 函数名称: taskmng_up_msg_fun_deal
* 函数功能: //上行收发消息
* 输入参数: IID 消息IID
        IOP 消息IOP
        len 发送的消息长度
        PayLoad 消息内容
        DestAddr 发送目的端

* 输出参数: RecvLoadLen 收到回码的消息内容长度
            RecvPayLoad 收到的消息内容
* 返 回 值:  1 成功 -1 失败
*******************************************************************************/
int taskmng_up_msg_fun_deal(uint16 IID,uint16 IOP,uint16 len,uint8 *PayLoad,char* DestAddr,uint16 *RecvLoadLen,uint8 *RecvPayLoad)
{
    ACTIVE_COMM_STATE_T         Stat = ACT_COMM_IDLE;
    uint16                      SendLen = 0;
    static time_t               CommBeginTime;                      //通讯开始时间
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
                TASK_FMT_DEBUG(-1, UP_LOG, "收到回复 len =%d \n",RecvLen);

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
                    TASK_FMT_DEBUG(-1, UP_LOG, "收到00000014回码错误信息 \n");
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
                    TASK_FMT_DEBUG(-1, UP_LOG, "超时重试RetryCnt %d \n",RetryCnt);
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
* 函数名称: PackUpMsgData
* 函数功能: 组上行消息
* 输入参数:
* 输出参数: 无
* 返 回 值: 数据帧长度
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
* 函数名称: DBSendMQTT
* 函数功能: //通过mqtt向数据中心发送数据
* 输入参数: label 消息标签
        IOP 消息IOP
        payload 消息内容
        len 发送的消息长度

* 输出参数: pindex 消息序号
* 返 回 值:  >0成功 -1 失败
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
* 函数名称: taskmng_local_addr_read
* 函数功能: //获取终端通信地址
* 输入参数:
* 输出参数: addr 终端地址
* 返 回 值:  0成功 -1 失败
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
    TASK_FMT_TRACE(-1,UP_LOG,"获取终端地址 0x%02x%02x%02x%02x%02x%02x \n",addr[5],addr[4],addr[3],addr[2],addr[1],addr[0]);
    //给ccoRouter重设一遍
    uint8 i;
    for(i=0;i<PLC_NUM;i++)
    {
        IsSetAddr[i]=0;
    }
    return 0;
}





/*******************************************************************************
* 函数名称: DBUpdateFun
* 函数功能: //处理数据中心上报的数据更新事件
* 输入参数: ptMsg 消息内容
* 输出参数:
* 返 回 值:
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
    //数据项标识 optional
    if(payload[index++]==0)
        return ;
    //关心的档案方案任务变更事件 没有主di 只有一个di
#ifdef AREA_FUJIAN
    if(payload[index++]==1) //存在主DI
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
    //全局具体任务主oad存的0x6E130200 次oad存的具体任务编号 信息点号是透明任务方案号
    if(MasterDI.value == 0x6E130200)
    {
        ret = taskmng_db_update_queue_write ( &pTaskDBUpdate, &dbupdate);
        TASK_FMT_DEBUG(-1,MQTT_LOG,"收到具体任务变更事件 写队列  ret %d  \n",ret);
        return;
    }
#endif
    switch(oad.value)
    {
    case 0x60000200:   //档案更新
    {
        TASK_FMT_DEBUG(-1,MQTT_LOG,"收到数据更新事件 logicid  %d infonum %d oad %08x \n",logicid,infonum,oad.value);
        ret = taskmng_db_update_queue_write ( &pMeterFileUpdate, &dbupdate);
        TASK_FMT_DEBUG(-1,MQTT_LOG,"taskmng_db_update_queue_write  ret %d  \n",ret);
    }
        break;

#ifdef AREA_FUJIAN
    case 0x6E050200:
    {
        TASK_FMT_DEBUG(-1,MQTT_LOG,"收到群组档案更新事件 logicid  %d infonum %d oad %08x \n",logicid,infonum,oad.value);
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
    /*240531 收到终端停上电后主线程处理就行了 原来给每个抄表线程的逻辑不合理  */
    case 0x31060200:
    {
        TASK_FMT_DEBUG(-1,MQTT_LOG,"收到数据更新事件 logicid %d infonum %d oad %08x \n",logicid,infonum,oad.value);
        ret = task_send_msg(gMqttSock,MSG_DBUPDATE,(uint8*)&dbupdate,sizeof(DB_UPDATE_T),IPC_MAIN);

        TASK_FMT_DEBUG(-1,MQTT_LOG,"task_send_msg  ret %d  \n",ret);
    }
        break;
    case 0x60120200:    //任务更新
    case 0x60140200:        //普通采集方案
    case 0x60160200:
    case 0x60180200:
    case 0x601E0200:    //转加密方案
#ifdef AREA_FUJIAN
    case 0x6E110200:    //全局透明任务方案集
    case 0x6E130200:    //全局透明具体任务集
    case 0x6E11FFFF:    //扩展的方案优先级数据
#endif
    {
        TASK_FMT_DEBUG(-1,MQTT_LOG,"收到数据更新事件logicid %d  infonum %d oad %08x \n",logicid,infonum,oad.value);
        //ret = task_send_msg(gMqttSock,MSG_DBUPDATE,(uint8*)&dbupdate,sizeof(DB_UPDATE_T),IPC_TASK);
        ret = taskmng_db_update_queue_write ( &pTaskDBUpdate, &dbupdate);
        TASK_FMT_DEBUG(-1,MQTT_LOG,"taskmng_db_update_queue_write  ret %d  \n",ret);
    }
        break;
#ifdef AREA_JIANGXI
    case 0x31060200:
    {
        // 发生终端停上电
        uint16  pipe;
        TASK_FMT_DEBUG(-1,MQTT_LOG,"收到终端停上电事件 logicid %d infonum %d oad %08x \n",logicid,infonum,oad.value);

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
* 函数名称: SearchMeterUpFun
* 函数功能://收到搜表结果后发送至抄表线程存入数据中心
* 输入参数: buf 消息内容
            len 消息长度
* 输出参数:
* 返 回 值:
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
    TASK_FMT_TRACE(-1,MQTT_LOG,"发送搜表消息 ret %d pipe %d \n",ret,pipe);
}


/*******************************************************************************
* 函数名称: MsgErrorFunc
* 函数功能://处理收到的错误信息
* 输入参数: MsgRecv 收到的消息
        MSGLen 收到的消息长度
* 输出参数:
* 返 回 值:
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
* 函数名称: CheckTaskRecordFile
* 函数功能://检查每个任务的私有文件是否存在 如没有则创建 私有文件存储每个任务每块表的抄读状态
* 输入参数: pipe 端口号

* 输出参数:
* 返 回 值:
*******************************************************************************/
void CheckTaskRecordFile(uint8 pipe)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    char      pfile[100]={0};
    uint8 i;
    uint8 TaskID;
    uint8 logicid;
    //TASK_FMT_TRACE(pipe, REC_LOG, "任务记录个数 pAmrThreadArg->TaskNum is %d\n",pAmrThreadArg->TaskNum);
    for(i=0;i<pAmrThreadArg->TaskNum;i++)
    {
        TaskID = pAmrThreadArg->pTaskRunInfo[i].TaskId;
        logicid = pAmrThreadArg->pTaskRunInfo[i].logicid;
        memset(pfile,0x00,sizeof(pfile));
        sprintf(pfile,"%s/%s%03d_PIPE%02d_Logic%d",PrivateDir,TASK_RECORD_FILE,TaskID,pipe,logicid);

        if(access(pfile,R_OK)==0)        //如果该任务的记录文件存在则下一个
        {
            TASK_FMT_DEBUG(pipe, REC_LOG, "任务记录文件%s 存在\n",pfile);
            char      pfile[100]={0};
            memset(pfile, 0x00, sizeof(pfile));
            sprintf(pfile, "%s/%s%03d_PIPE%02d_Logic%d", PrivateDir, TASK_RECORD_FILE, TaskID, pipe, logicid);
            Task_Record_T TaskRecord;
            memset(&TaskRecord, 0x00, sizeof(Task_Record_T));
            read_pdata_xram(pfile, (char*)&TaskRecord, 0, sizeof(Task_Record_T));
            //检查任务记录文件版本
            if(strcmp(TaskRecord.FileVersion, TASK_RECORD_VERSION) != 0)
            {
                TASK_FMT_TRACE(pipe, REC_LOG, "任务记录文件版本不对 删除\n");
                remove(pfile);
                CreateTaskRecordFile(pipe,TaskID,logicid);
            }
            else
            {
                //检查下任务记录文件内容 以防档案变更后对不上
                CheckTaskRecordFileMeter(pipe,TaskID,logicid);
            }
            continue;
        }
        CreateTaskRecordFile(pipe,TaskID,logicid);

    }
}


/*******************************************************************************
* 函数名称: CheckTaskRecordFileMeter
* 函数功能://检查下任务记录文件内容 以防档案变更后对不上
* 输入参数: pipe 端口号
            TaskId 任务号
* 输出参数:
* 返 回 值:
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
            //如果事件列表没赋值 给赋值
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
* 函数名称: CreateTaskRecordFile
* 函数功能://创建任务记录文件
* 输入参数: pipe 端口号
            TaskId 任务号
* 输出参数:
* 返 回 值:
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
    //如果任务没有要抄的表 就不创建任务记录文件
    if(RunTask->NeedMtrNum == 0)
        return;
    if(TaskInfo.SchemeType==EVENTPLAN)
    {
        for(i=0;i<TaskInfo.CsdTask.RoadNum;i++)
        {
            EventReportSeq[i].EventMainOad = TaskInfo.CsdTask.RoadTask[i].MainOad;
            EventReportSeq[i].ReportSeq = 0xffffffff; //698表事件序号0有效
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
    TASK_FMT_DEBUG(pipe, REC_LOG,"创建任务记录文件%s \n",pfile);
    write_pdata_xram(pfile,(char*)&TaskRecord,0,sizeof(Task_Record_T));
}





/*******************************************************************************
* 函数名称: GetTaskReportSeq
* 函数功能://从任务文件中读取该表对应事件的已上报序号
* 输入参数: pipe 端口号
            TaskId 任务号
            MainOad 事件主OAD
            Pn 电表序号
* 输出参数:
* 返 回 值: 该表该事件已抄读的事件序号 698表事件序号0是有效的 所以没有就返回0xffffffff
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
    //TASK_FMT_TRACE( pipe, COMM_LOG, "读取文件结果1111 ret %d\n", ret);
    if(ret!=0)
        return 0xffffffff;
    //TASK_FMT_TRACE( pipe, REC_LOG, "读取文件 %s ret %d MeterCount %d  \n", pfile,ret,TaskRecord.MeterCount);
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
* 函数名称: SetTaskReportSeq
* 函数功能:在任务文件中设置该表对应事件的序号
* 输入参数: pipe 端口号
            TaskId 任务号
            MainOad 事件主OAD
            Pn 电表序号
            seq 事件序号
* 输出参数:
* 返 回 值:
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
        TASK_FMT_TRACE( pipe, REC_LOG, "读取任务记录文件失败 \n");
        return;
    }
    TASK_FMT_TRACE( pipe, REC_LOG, "SetTaskReportSeq Mainoad %08x pn %d seq %d   \n", MainOad.value,Pn,seq);
    TASK_FMT_TRACE( pipe, REC_LOG, "SetTaskReportSeq 读取文件 %s ret %d MeterCount %d  \n", pfile,ret,TaskRecord.MeterCount);
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
                    TASK_FMT_TRACE( pipe, REC_LOG, "写入事件序号 PN %d EventMainOad %08x seq %d   \n",Pn,MainOad.value,seq );
                    return;
                }
            }
        }
    }
    TASK_FMT_TRACE( pipe, REC_LOG, "写入事件序号失败 \n");

}

/*******************************************************************************
* 函数名称: GetTaskReportEventState
* 函数功能://从任务文件中读取该表对应事件的发生状态
* 输入参数: pipe 端口号
            TaskId 任务号
            MainOad 事件主OAD
            Pn 电表序号
* 输出参数:
* 返 回 值: 该表该事件的发生状态
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
* 函数名称: SetTaskReportEventState
* 函数功能:在任务文件中设置该表对应事件的发生状态
* 输入参数: pipe 端口号
            TaskId 任务号
            MainOad 事件主OAD
            Pn 电表序号
            state 事件发生状态
* 输出参数:
* 返 回 值:
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
* 函数名称: SetTGRecordFile
* 函数功能:总加组抄读成功后在任务记录文件中置为成功
* 输入参数:
            TaskId 任务id
* 输出参数:
* 返 回 值:
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
* 函数名称: GetTaskState
* 函数功能://获取任务状态
* 输入参数: TaskId 任务号
* 输出参数:
* 返 回 值: 任务状态
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
* 函数名称: ErrorUpMsg
* 函数功能://上行回复错误信息 0000 0014
* 输入参数: MsgInfo 消息内容
* 输出参数:
* 返 回 值:
*******************************************************************************/
void ErrorUpMsg(MESSAGE_INFO_T* MsgInfo )
{
    MESSAGE_INFO_T* MsgSend=NULL;
    uint8 payload[20]={0};
    uint8 index=0;
    uint16 errorcode = 0;
    TASK_FMT_TRACE(-1, UP_LOG, "上行回复错误信息 \n");

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
* 函数名称: MakeDateTimeS
* 函数功能://将time转为date_time_s格式并填充到buf中
* 输入参数: time 时间戳

* 输出参数: buf date_time_s格式
* 返 回 值:
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
* 函数名称: CheckMeterSearchResult
* 函数功能://检查搜表结果是否重复 如果重复则不写入数据中心
* 输入参数:pipe 端口号
        searchresult 搜表结果
* 输出参数: infonum 写入数据中心用的信息点号
* 返 回 值: TRUE 没有重复的 FALSE有重复的
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
                //记录相同 但是端口号不同
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
    TASK_FMT_DEBUG( pipe, RELAY_LOG, "CheckMeterSearchResult 添加记录 infonum  %d count %d \n",MeterSearchResult.infonum,MeterSearchResult.count);

    pthread_mutex_unlock(&MeterSearchResult.lock);
    return TRUE;
}


/*******************************************************************************
* 函数名称: EventReportDeal
* 函数功能:事件上报处理
* 输入参数:pipe 端口号

* 输出参数:
* 返 回 值: 1 处理完成 -1 未完成
*******************************************************************************/
int EventReportDeal(uint8 pipe)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    static time_t  BeginTime[PLC_NUM];
    uint8 RecvBuf[MAX_MSG_LEN]={0};
    const uint16  OverTime = 30; //超时时间
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
        TASK_FMT_DEBUG( pipe, RELAY_LOG, "事件上报参数无效 MtrIndex %d \n",MtrIndex);
        return 1;
    }
    if(FALSE==taskmng_meter_event_report_time_check())
    {
        usleep(100000);
        return -1;
    }
    //uint8 prtl = pAmrThreadArg->EventReport.MtrEvent[MtrIndex].prtl;
    //如果每天上报的事件超过200个则不处理
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
            TASK_FMT_DEBUG( pipe, RELAY_LOG, "事件上报收到消息 IID %04X IOP %04X index %d payload \n",MsgInfo->IID,MsgInfo->IOP,MsgInfo->index);

            TASK_BUF_DEBUG( pipe, RELAY_LOG,MsgInfo->playload, MsgInfo->msglen);


            if(monitorParallFlag == 1)
            {
                if(MsgInfo->IOP != 0x0031 || MsgInfo->index != pAmrThreadArg->EventReport.SendIndex)
                    break;

                //对比下地址是否对应
                OOP_TSA_T       Tsa = taskmng_tsa_get(Pn, LOGIC_ID_YC);
                uint8           addr[6] =
                {
                    0
                };
                uint16          index = 0;
                uint8           lenoffset;
                uint16          framelen = 0;

                index += 4;                                     //端口号
                memcpy(addr, Payload + index, 6);
                index += 6;

                if(memcmp(addr, Tsa.add, 6) != 0)
                {
                    TASK_FMT_DEBUG(pipe, RELAY_LOG, "收到的地址不对应 \n");
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
                TASK_FMT_DEBUG( pipe, RELAY_LOG, "事件上报任务超过30s未收到回复 \n");

                pAmrThreadArg->TotalState.EventReportState = INNER_RTS_RETRY;
            }
        }
    }
        break;
    case INNER_RTS_RETRY:
    {
        pAmrThreadArg->EventReport.MtrEvent[MtrIndex].FailCnt++;
        TASK_FMT_DEBUG( pipe, RELAY_LOG, "Pn %d 失败次数 %d  \n",Pn,pAmrThreadArg->EventReport.MtrEvent[MtrIndex].FailCnt);
        pAmrThreadArg->TotalState.EventReportState = INNER_RTS_SEND;
    }
        break;
    default:
        break;
    }
    return -1;
}

/*******************************************************************************
* 函数名称: EventReportDeal
* 函数功能:事件上报处理
* 输入参数:pipe 端口号

* 输出参数:
* 返 回 值: 1 处理完成 -1 未完成
*******************************************************************************/
int RS485EventReportDeal(uint8 pipe)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    //static time_t  BeginTime[MAX_485_NUM];
    uint8 RecvBuf[MAX_MSG_LEN]={0};
    //const uint16  OverTime = 10; //超时时间
    int RecvLen;
    int ret;
//    MSG_INFO_T *MsgInfo=NULL;
//    uint8 Payload[MAX_MSG_LEN] = {0};

    uint8 MtrIndex = pAmrThreadArg->EventReport.CurIndex;
    uint16 Pn = pAmrThreadArg->EventReport.MtrEvent[MtrIndex].Pn;
    //uint8 prtl = pAmrThreadArg->EventReport.MtrEvent[MtrIndex].prtl;
    //如果每天上报的事件超过200个则不处理
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
                            TASK_FMT_DEBUG(pipe, RELAY_LOG, "事件上报任务超时 \n");
    
                            pAmrThreadArg->TotalState.EventReportState = INNER_RTS_RETRY;
                        }
                    }
                }
                break;
    
            case INNER_RTS_RETRY:
                {
                    pAmrThreadArg->EventReport.MtrEvent[MtrIndex].FailCnt++;
                    TASK_FMT_DEBUG(pipe, RELAY_LOG, "Pn %d 失败次数 %d  \n", Pn,
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
    //特征码信息长度
    codelen = buf[offset++];
    for(i=0;i<singalnum;i++)
    {

        memset(tmpBuff,0x00,30);
        //年月日为BIN格式 1d 15 11 1d 0a 14
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
        //默认为AAE9 aa e9 00 00 puAmr组报文时会反过来 所以这边存大端

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
    offset += 6; /* 跳过发生源地址6个字节 */
    singalnum = buf[offset++];
    if(singalnum>4)
    {
        return;
    }
    IdentifyInformations.Identifynum=singalnum;
    //特征码信息长度
    //codelen = buf[offset++];
    for(i = 0; i < singalnum; i++)
    {
        memset(tmpBuff,0x00,30);
        //年月日为BIN格式 1d 15 11 1d 0a 14
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
        //默认为AAE9 aa e9 00 00 puAmr组报文时会反过来 所以这边存大端

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
* 函数名称: reset_state_recv_check
* 函数功能://收到复位主动上报状态字回码判断是否正常
* 输入参数:pipe 端口号
        buf 收到的报文
        len 报文长度
* 输出参数:
* 返 回 值: 0 正常应答 -1 异常应答帧
*******************************************************************************/
int reset_state_recv_check(uint8 pipe,uint8* buf,uint16 len)
{
    int ret = -1;
    uint16 reallen=0;
    uint8 *prt =(uint8*)commfun_645_frame_get((char*)buf,0x68,len,&reallen);
    if(prt == NULL)
    {
        TASK_FMT_DEBUG( pipe, RELAY_LOG, "reset_state_recv_check不是645报文  \n");
        return ret;
    }
    if(prt[8]==0x94)
        ret = 0;
    return ret;
}
/*******************************************************************************
* 函数名称: get_state_recv_check
* 函数功能://主动抄状态字后进行处理 看是否有后续事件
* 输入参数:pipe 端口号
        buf 收到的报文
        len 报文长度
* 输出参数:
* 返 回 值: 0 正常应答 -1 异常应答帧
*******************************************************************************/
int get_state_recv_check(uint8 pipe,uint8* buf,uint16 len)
{
    int ret;
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    uint8 MtrIndex = pAmrThreadArg->EventReport.CurIndex;
    uint16 currentlen = pAmrThreadArg->EventReport.MtrEvent[MtrIndex].DataLen;
    uint16 reallen=0;
    uint8 data[300] = {0};  //从状态字开始的数据域
    uint8 datalen;         //数据域长度
    uint8  EventStateDI[4] = {0x34, 0x48, 0x33, 0x37}; //主动上报状态字的ID  0x04001501
    uint8 *prt =(uint8*)commfun_645_frame_get((char*)buf,0x68,len,&reallen);
    if(prt == NULL)
    {
        TASK_FMT_DEBUG( pipe, RELAY_LOG, "get_state_recv_check不是645报文  \n");
        return -1;
    }
    if(memcmp(&prt[10],EventStateDI,4)!=0)
    {
        TASK_FMT_DEBUG( pipe, RELAY_LOG, "不是04001501  \n");
        return -1;
    }
    if(prt[9]<4+12) //di4 状态字12 两个AAH分隔符 状态字全0的时候可能没有AA
    {
        TASK_FMT_DEBUG( pipe, RELAY_LOG, "数据域长度 %d  \n",prt[9]);
        return -1;
    }
    datalen = prt[9] -4; //减去4个字节DI
    memcpy(data,prt+14,datalen);
    Pro_ReduceMeterGap(data,datalen,0x33);
    ret = ReportEventStateFun(pipe,data,datalen,MtrIndex);
    if(ret == 0)
    {
        //复位完毕后没有新增事件
        pAmrThreadArg->EventReport.MtrEvent[MtrIndex].state = EVENT_ONEMTR_END;
        return 0;
    }
    if(ret ==1)
    {
        //复位后有新增事件 但是不需要抄 直接存下来即可

        pAmrThreadArg->EventReport.MtrEvent[MtrIndex].state = EVENT_RESET_STATE;
    }
    if(ret ==2)
    {
        //复位后有新增事件 继续抄
        pAmrThreadArg->EventReport.MtrEvent[MtrIndex].state = EVENT_GET_EVENT;
        pAmrThreadArg->EventReport.MtrEvent[MtrIndex].CurIndex=0;
    }
    memcpy(pAmrThreadArg->EventReport.MtrEvent[MtrIndex].ReportState,data,12);
    //将新的状态字写入上报队列
    pAmrThreadArg->EventReport.MtrEvent[MtrIndex].DataBuf[currentlen] = reallen;
    pAmrThreadArg->EventReport.MtrEvent[MtrIndex].DataLen++;
    memcpy(&pAmrThreadArg->EventReport.MtrEvent[MtrIndex].DataBuf[currentlen+1],prt,reallen);
    pAmrThreadArg->EventReport.MtrEvent[MtrIndex].DataLen+=reallen;
    pAmrThreadArg->EventReport.MtrEvent[MtrIndex].DataBuf[4]++;
    return 0;
}

/*******************************************************************************
* 函数名称: EventReportRecvFun
* 函数功能://事件上报处理收到的报文
* 输入参数:pipe 端口号
        buf 收到的报文
        len 报文长度
* 输出参数:
* 返 回 值: 1 处理完成 -1 未完成
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
        //现场遇到抄04001501回否认的 如果不判断回码会一直抄
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
        TASK_FMT_DEBUG(pipe, RELAY_LOG,"PN %d 获取3320 LinkDataReceive 698解帧返回值 %d \n",Pn,ret);
        if(ret >=eOopComm_Fail)//解帧失败直接返回
            break;

        pAppLayer = FindAppLayer(pipe, NULL);
        if(pAppLayer == NULL)
            break;
        memset(RecvBuf,0x00,sizeof(RecvBuf));

        RecvLen = pAppLayer->recvApduList->apduLen-pAppLayer->recvApduList->offSet;
        memcpy(RecvBuf, pAppLayer->recvApduList->apdu+pAppLayer->recvApduList->offSet, RecvLen);

        TASK_FMT_DEBUG( pipe, RELAY_LOG,"3320事件列表信息 len %d RecvBuf \n",RecvLen);

        TASK_BUF_DEBUG( pipe, RELAY_LOG,RecvBuf, RecvLen);
        index = 0;

        if(RecvBuf[index++]==0) //1 DAR 1 DATA
        {
            TASK_FMT_DEBUG( pipe, RELAY_LOG,"电表不支持3320 放弃抄读\n");
            pAmrThreadArg->EventReport.MtrEvent[MtrIndex].state = EVENT_ONEMTR_END;
            break;
        }
        if(RecvBuf[index++] !=1 )//1代表array
            break;
        uint8 OadCount = RecvBuf[index++] ;
        uint8 eventindex=0;
        for(i=0;i<OadCount;i++)
        {
            index++;//oad类型81
            memcpy_r((uint8*)&RecvOad.value,&RecvBuf[index],4);
            index+=4;
            memset(SubOad,0x00,sizeof(OOP_OAD_U)*MAX_OAD_NUM);
            SubOadcount = GetEventSubOad(Pn,RecvOad,SubOad,&newpn);
            if(SubOadcount==0)
            {
                TASK_FMT_DEBUG( pipe, RELAY_LOG,"事件 oad %08x 未找到对应的方案\n",RecvOad.value);
                continue;
            }
            pAmrThreadArg->EventReport.MtrEvent[MtrIndex].EventInfo[eventindex].EventDI = RecvOad;
            pAmrThreadArg->EventReport.MtrEvent[MtrIndex].EventInfo[eventindex].SubOadNum = SubOadcount;
            memcpy(pAmrThreadArg->EventReport.MtrEvent[MtrIndex].EventInfo[eventindex].SubOad,SubOad,sizeof(OOP_OAD_U)*SubOadcount);
            eventindex++;
            TASK_FMT_DEBUG( pipe, RELAY_LOG,"3320事件列表 count %d i %d mainoad %08x \n",OadCount,i,RecvOad.value);
            for(j=0;j<SubOadcount;j++)
            {
                TASK_FMT_DEBUG( pipe, RELAY_LOG,"suboad index %d  %08x \n",j,SubOad[j].value);
            }
        }
        if(eventindex==0)
        {
            TASK_FMT_DEBUG( pipe, RELAY_LOG,"没有需要抄读的事件 \n");
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
            TASK_FMT_DEBUG(pipe, RELAY_LOG,"LinkDataReceive 698解帧返回值 %d \n",ret);
            if(ret >=eOopComm_Fail)//解帧失败直接返回
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

            TASK_FMT_DEBUG( pipe, RELAY_LOG,"抄读事件回复 len %d RecvBuf \n",RecvLen);

            TASK_BUF_DEBUG( pipe, RELAY_LOG,RecvBuf, RecvLen);
            TASK_FMT_DEBUG(pipe, RELAY_LOG,"MainOad.value %x \n",MainOad.value);
#if (defined AREA_JIANGXI) || (defined AREA_ANHUI)
            if(MainOad.value == 0x31300200) //感知终端下挂电能表变化事件
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
            TASK_FMT_DEBUG( pipe, RELAY_LOG,"抄读事件回复 SendDI is %d\n",SendDI);
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
            TASK_FMT_DEBUG( pipe, RELAY_LOG,"抄读事件回复 SendDI is %d\n",SendDI);
            TASK_BUF_DEBUG( pipe, RELAY_LOG,buf, len);
            uint8 lenoff;
            XADR_GetEleNum(buf,&lenoff);
            TASK_FMT_DEBUG( pipe, RELAY_LOG,"Change_circuit_Breaker_Data_to_Struct SendDI=%u,di=%u,len=%u,lenoff=%u \n", SendDI, 0x09080601, len, lenoff);

            if (SendDI == 0x09080601)
            {
                if (len > (lenoff + 14 + 2) && len - (lenoff + 14 + 2) >= 13) /* 发生源地址6个字节+识别信号数量1个字节+识别信号时间6个字节 */
                {
                    TASK_FMT_DEBUG( pipe, RELAY_LOG,"_Change_circuit_Breaker_Data_to_Struct_ in \n");
                    Change_circuit_Breaker_Data_to_Struct(buf + lenoff + 14, len - (lenoff + 14 + 2), pipe);
                }
            }
#endif
#endif
#ifdef AREA_HUNAN
            //湖南需要将断路器上报的温度告警事件 保护动作事件 闸位变化事件组织成698事件单元上报
            MeterPrtlFmt_T MeterPrtlData;
            memset(&MeterPrtlData,0x00,sizeof(MeterPrtlFmt_T));
            uint32 DI;
            ret = taskmng_645_meter_data_unpack(len,buf,MeterPrtlData.MtrAddr,&MeterPrtlData.CtrlCode,&MeterPrtlData.DataLen,MeterPrtlData.DataBuff);
            if(CHECK_RESULT_SUCCEED == ret)
            {
                memcpy(&DI,MeterPrtlData.DataBuff,4);
                if(0x03810207==DI)
                {
                    //温度告警事件
                    event_temperature_seq(pipe,MeterPrtlData.DataBuff+4,MeterPrtlData.DataLen-4);
                }
                if(0x03950001==DI)
                {
                    //温度告警事件
                    temperature_event_save(pipe,MeterPrtlData.DataBuff+4,MeterPrtlData.DataLen-4,Pn);
                }
                if(0x03810206==DI)
                {
                    //保护动作新增次数 作为事件序号
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
            //OAD后面填报文条数
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

            TASK_FMT_DEBUG( pipe, RELAY_LOG,"645总数据 len %d Buf \n",pAmrThreadArg->EventReport.MtrEvent[MtrIndex].DataLen);

            TASK_BUF_DEBUG( pipe, RELAY_LOG,pAmrThreadArg->EventReport.MtrEvent[MtrIndex].DataBuf, pAmrThreadArg->EventReport.MtrEvent[MtrIndex].DataLen);

        }
    }
        break;
    case EVENT_RESET_STATE:
    {
        if(prtl == PRTL_20_IOT)
        {
            TASK_FMT_DEBUG( pipe, RELAY_LOG,"确认电能表跟随上报状态字回复 \n");
            TASK_BUF_DEBUG( pipe, RELAY_LOG,&buf[lenoffset], framelen);
            ret = LinkDataReceive(pipe, &buf[lenoffset], framelen,NULL);
            TASK_FMT_DEBUG(pipe, RELAY_LOG,"LinkDataReceive 698解帧返回值 %d \n",ret);
            if(ret >=eOopComm_Fail)//解帧失败直接返回
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
                TASK_FMT_DEBUG( pipe, RELAY_LOG,"确认电能表跟随上报状态字成功 \n");
            }
            pAmrThreadArg->EventReport.MtrEvent[MtrIndex].state = EVENT_ONEMTR_END;

            break;
        }
        //收到回码判断是否确认帧 如果回复异常 则再复位一次
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
* 函数名称: EventReportRecvFun
* 函数功能://事件上报处理收到的报文
* 输入参数:pipe 端口号
        buf 收到的报文
        len 报文长度
* 输出参数:
* 返 回 值: 1 处理完成 -1 未完成
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
        TASK_FMT_DEBUG(pipe, RELAY_LOG,"PN %d 获取3320 LinkDataReceive 698解帧返回值 %d \n",Pn,ret);
        if(ret >=eOopComm_Fail)//解帧失败直接返回
            break;

        pAppLayer = FindAppLayer(pipe, NULL);
        if(pAppLayer == NULL)
            break;
        memset(RecvBuf,0x00,sizeof(RecvBuf));

        RecvLen = pAppLayer->recvApduList->apduLen-pAppLayer->recvApduList->offSet;
        memcpy(RecvBuf, pAppLayer->recvApduList->apdu+pAppLayer->recvApduList->offSet, RecvLen);

        TASK_FMT_DEBUG( pipe, RELAY_LOG,"3320事件列表信息 len %d RecvBuf \n",RecvLen);

        TASK_BUF_DEBUG( pipe, RELAY_LOG,RecvBuf, RecvLen);
        index = 0;

        if(RecvBuf[index++]==0) //1 DAR 1 DATA
        {
            TASK_FMT_DEBUG( pipe, RELAY_LOG,"电表不支持3320 放弃抄读\n");
            pAmrThreadArg->EventReport.MtrEvent[MtrIndex].state = EVENT_ONEMTR_END;
            break;
        }
        if(RecvBuf[index++] !=1 )//1代表array
            break;
        uint8 OadCount = RecvBuf[index++] ;
        uint8 eventindex=0;
        for(i=0;i<OadCount;i++)
        {
            index++;//oad类型81
            memcpy_r((uint8*)&RecvOad.value,&RecvBuf[index],4);
            index+=4;
            memset(SubOad,0x00,sizeof(OOP_OAD_U)*MAX_OAD_NUM);
            SubOadcount = GetEventSubOad(Pn,RecvOad,SubOad,&newpn);
            if(SubOadcount==0)
            {
                TASK_FMT_DEBUG( pipe, RELAY_LOG,"事件 oad %08x 未找到对应的方案\n",RecvOad.value);
                continue;
            }
            pAmrThreadArg->EventReport.MtrEvent[MtrIndex].EventInfo[eventindex].EventDI = RecvOad;
            pAmrThreadArg->EventReport.MtrEvent[MtrIndex].EventInfo[eventindex].SubOadNum = SubOadcount;
            memcpy(pAmrThreadArg->EventReport.MtrEvent[MtrIndex].EventInfo[eventindex].SubOad,SubOad,sizeof(OOP_OAD_U)*SubOadcount);
            eventindex++;
            TASK_FMT_DEBUG( pipe, RELAY_LOG,"3320事件列表 count %d i %d mainoad %08x \n",OadCount,i,RecvOad.value);
            for(j=0;j<SubOadcount;j++)
            {
                TASK_FMT_DEBUG( pipe, RELAY_LOG,"suboad index %d  %08x \n",j,SubOad[j].value);
            }
        }
        if(eventindex==0)
        {
            TASK_FMT_DEBUG( pipe, RELAY_LOG,"没有需要抄读的事件 \n");
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
            TASK_FMT_DEBUG(pipe, RELAY_LOG,"LinkDataReceive 698解帧返回值 %d \n",ret);
            if(ret >=eOopComm_Fail)//解帧失败直接返回
                break;

            pAppLayer = FindAppLayer(pipe, NULL);
            if(pAppLayer == NULL)
                break;
            memset(RecvBuf,0x00,sizeof(RecvBuf));
            RecvLen = pAppLayer->recvApduList->apduLen-pAppLayer->recvApduList->offSet;
            memcpy(RecvBuf, pAppLayer->recvApduList->apdu+pAppLayer->recvApduList->offSet, RecvLen);

            TASK_FMT_DEBUG( pipe, RELAY_LOG,"抄读事件回复 len %d RecvBuf \n",RecvLen);

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

            TASK_FMT_DEBUG( pipe, RELAY_LOG,"645总数据 len %d Buf \n",pAmrThreadArg->EventReport.MtrEvent[MtrIndex].DataLen);

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
* 函数名称: Save698EventData
* 函数功能://写0xFF100A00 事件上报数据
* 输入参数:pipe 端口号
        DataBuf 事件内容
        DataLen 长度
        Pn 电表号
        MainOad 事件主oad
        StartTime 启动时标
* 输出参数:
* 返 回 值:
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
    //事件上报时把固定列CSD先填进去
    uint8 rptBuff[2000] = {0x60, 0x12, 0x03, 0x00, 0x05, 0x00, 0x20, 0x2A, 0x02, 0x00, 0x00,
        0x60, 0x40, 0x02, 0x00, 0x00, 0x60, 0x41, 0x02, 0x00, 0x00, 0x60, 0x42, 0x02, 0x00};
    CsdNum = DataBuf[index++];
    rptBuff[rptLen++] = 1;  //CSD类型1 记录型对象描述符
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
    if(DataBuf[index++]==0)   //0为错误DAR 1 为SEQUENCE OF A-RecordRow
        return;
    RecordNum = DataBuf[index++];
    if(RecordNum==0)
        return;

    rptBuff[rptLen++] = 1;
    rptBuff[rptLen++] = RecordNum;
    //填固定列数据
    //服务器地址
    rptBuff[rptLen++] = DT_TSA;
    tsa = taskmng_tsa_get(Pn,LOGIC_ID_YC);
    rptBuff[rptLen++] = tsa.len+2;
    rptBuff[rptLen++] = tsa.af;
    if(tsa.flag == 1 )
        rptBuff[rptLen++] = tsa.vxd;
    //taskmng_mtr_addr_get( Pn,LOGIC_ID_YC,Addr);
    memcpy(&rptBuff[rptLen],tsa.add, 6);
    rptLen+=6;
    //启动时标
    rptBuff[rptLen++] = DT_DATETIME_S;
    DT_Time2DateTimeHex(StartTime,(DateTimeHex_t *)&rptBuff[rptLen]);
    rptLen+=7;
    //成功时标
    rptBuff[rptLen++] = DT_DATETIME_S;
    DT_Time2DateTimeHex(RealTime_t,(DateTimeHex_t *)&rptBuff[rptLen]);
    rptLen+=7;
    //存储时标
    rptBuff[rptLen++] = DT_DATETIME_S;
    DT_Time2DateTimeHex(RealTime_t,(DateTimeHex_t *)&rptBuff[rptLen]);
    rptLen+=7;

    rptBuff[rptLen++] = 1;
    rptBuff[rptLen++] = CsdNum;
    //需要继续解析每条记录 不需要最后的跟随上报域以及时间标签
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

    //将组好的事件内容写进数据中心
    taskmng_event_report_data_write(pipe,rptBuff,rptLen,EventReportOad);
}

/*******************************************************************************
* 函数名称: event_report_data_save
* 函数功能://将通过8802主动上报的事件写入数据中心
* 输入参数:pipe 端口号
        DataBuf 事件内容 30 11 02 01 07 00 20 22 02 00 00 20 1e 02 00 00 20 20 02 00 00 20 24 02 00 00 33 00 02 00 00 00 10 22 01 00 00 20 22 01 01 01 06 00 00 00 02 1c 07 e5 03 1f 13 38 0a 1c 07 e5 04 01 09 1b 36 00 00 06 00 00 00 00 06 00 00 00 00
        buflen 长度
        pn 电表

* 输出参数:
* 返 回 值: 解析的报文长度
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
    //事件上报时把固定列CSD先填进去
    uint8 rptBuff[2000] = {0x60, 0x12, 0x03, 0x00, 0x05, 0x00, 0x20, 0x2A, 0x02, 0x00, 0x00,
        0x60, 0x40, 0x02, 0x00, 0x00, 0x60, 0x41, 0x02, 0x00, 0x00, 0x60, 0x42, 0x02, 0x00};

    rptBuff[rptLen++] = 1;  //CSD类型1 记录型对象描述符
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
    if(DataBuf[index++]==0)   //0为错误DAR 1 为SEQUENCE OF A-RecordRow
        return index;
    RecordNum = DataBuf[index++];
    if(RecordNum==0)
        return index;

    rptBuff[rptLen++] = 1;
    rptBuff[rptLen++] = RecordNum;
    for(j=0;j<RecordNum;j++)
    {
        //填固定列数据
        //服务器地址
        rptBuff[rptLen++] = DT_TSA;
        rptBuff[rptLen++] = 7;
        rptBuff[rptLen++] = 5;
        taskmng_mtr_addr_get( Pn,LOGIC_ID_YC,Addr);
        memcpy_r(&rptBuff[rptLen],Addr, 6);
        rptLen+=6;
        //启动时标
        rptBuff[rptLen++] = DT_DATETIME_S;
        DT_Time2DateTimeHex(RealTime_t,(DateTimeHex_t *)&rptBuff[rptLen]);
        rptLen+=7;
        //成功时标
        rptBuff[rptLen++] = DT_DATETIME_S;
        DT_Time2DateTimeHex(RealTime_t,(DateTimeHex_t *)&rptBuff[rptLen]);
        rptLen+=7;
        //存储时标
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
* 函数名称: event_report_data_save
* 函数功能://将通过8802主动上报的事件写入数据中心
* 输入参数:pipe 端口号
        DataBuf 事件内容 30 11 02 01 07 00 20 22 02 00 00 20 1e 02 00 00 20 20 02 00 00 20 24 02 00 00 33 00 02 00 00 00 10 22 01 00 00 20 22 01 01 01 06 00 00 00 02 1c 07 e5 03 1f 13 38 0a 1c 07 e5 04 01 09 1b 36 00 00 06 00 00 00 00 06 00 00 00 00
        buflen 长度
        pn 电表

* 输出参数:
* 返 回 值: 解析的报文长度
*******************************************************************************/

uint16 deal_reportNotificationlist_data_save(uint8 pipe,uint8 *DataBuf,uint16 DataLen,uint16 Pn,OOP_OAD_U RecvOad)
{
    uint8 j,k;
    uint16 index=0;
    time_t        RealTime_t = time(NULL);
    uint8 Addr[6]={0};
    uint16 rptLen = 25;
    //事件上报时把固定列CSD先填进去
    uint8 rptBuff[2000] = {0x60, 0x12, 0x03, 0x00, 0x05, 0x00, 0x20, 0x2A, 0x02, 0x00, 0x00,
        0x60, 0x40, 0x02, 0x00, 0x00, 0x60, 0x41, 0x02, 0x00, 0x00, 0x60, 0x42, 0x02, 0x00};

    rptBuff[rptLen++] = 0;  //CSD类型1 记录型对象描述符
    TASK_FMT_DEBUG( pipe, RELAY_LOG,"deal_reportNotificationlist_data_save \n");
    TASK_BUF_DEBUG( pipe, RELAY_LOG,DataBuf, DataLen);

    memcpy_r(&rptBuff[rptLen],(uint8*)&RecvOad.value,4);
    rptLen+=4;

    if(DataBuf[index++]==0)   //0为错误DAR 1 为SEQUENCE OF A-RecordRow
        return index;


    rptBuff[rptLen++] = 1;
    rptBuff[rptLen++] = 1;
    for(j=0;j<1;j++)
    {
        //填固定列数据
        //服务器地址
        rptBuff[rptLen++] = DT_TSA;
        rptBuff[rptLen++] = 7;
        rptBuff[rptLen++] = 5;
        taskmng_mtr_addr_get( Pn,LOGIC_ID_YC,Addr);
        memcpy_r(&rptBuff[rptLen],Addr, 6);
        rptLen+=6;
        //启动时标
        rptBuff[rptLen++] = DT_DATETIME_S;
        DT_Time2DateTimeHex(RealTime_t,(DateTimeHex_t *)&rptBuff[rptLen]);
        rptLen+=7;
        //成功时标
        rptBuff[rptLen++] = DT_DATETIME_S;
        DT_Time2DateTimeHex(RealTime_t,(DateTimeHex_t *)&rptBuff[rptLen]);
        rptLen+=7;
        //存储时标
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
* 函数名称: taskmng_event_report_data_write
* 函数功能://将组好的电表上报事件内容写入数据中心
* 输入参数:pipe 端口号
        DataBuf 事件内容
        DataLen 长度
        Oad 写事件的oad

* 输出参数:
* 返 回 值:
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
    TASK_FMT_DEBUG( pipe, RELAY_LOG,"写事件内容到数据中心 infonum %d len%d \n",NomalOad.infoNum,DataLen);

    TASK_BUF_DEBUG( pipe, RELAY_LOG,DataBuf, DataLen);
    ret = db_write_nomal(pAmrThreadArg->DBClientHandle,&NomalOad,DataBuf,DataLen);
    if(ret !=0)
    {
        TASK_FMT_DEBUG( pipe, RELAY_LOG,"事件上报数据写入数据库失败 \n");
    }
}



/*******************************************************************************
* 函数名称: 分钟冻结的8802数据上报
* 函数功能://将通过8802主动上报的事件写入数据中心
* 输入参数:pipe 端口号
        DataBuf 事件内容 30 11 02 01 07 00 20 22 02 00 00 20 1e 02 00 00 20 20 02 00 00 20 24 02 00 00 33 00 02 00 00 00 10 22 01 00 00 20 22 01 01 01 06 00 00 00 02 1c 07 e5 03 1f 13 38 0a 1c 07 e5 04 01 09 1b 36 00 00 06 00 00 00 00 06 00 00 00 00
        buflen 长度
        pn 电表

* 输出参数:
* 返 回 值: 解析的报文长度
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
    //事件上报时把固定列CSD先填进去
    uint8 rptBuff[2000] = {0x60, 0x12, 0x03, 0x00, 0x05, 0x00, 0x20, 0x2A, 0x02, 0x00, 0x00,
        0x60, 0x40, 0x02, 0x00, 0x00, 0x60, 0x41, 0x02, 0x00, 0x00, 0x60, 0x42, 0x02, 0x00};

    rptBuff[rptLen++] = 1;  //CSD类型1 记录型对象描述符
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
    if(DataBuf[index++]==0)   //0为错误DAR 1 为SEQUENCE OF A-RecordRow
        return index;
    RecordNum = DataBuf[index++];

    if(RecordNum==0)
        return index;

    rptBuff[rptLen++] = 1;
    rptBuff[rptLen++] = RecordNum;
    for(j=0;j<RecordNum;j++)
    {
        //填固定列数据
        //服务器地址
        rptBuff[rptLen++] = DT_TSA;
        rptBuff[rptLen++] = 7;
        rptBuff[rptLen++] = 5;
        taskmng_mtr_addr_get( Pn,LOGIC_ID_YC,Addr);
        memcpy_r(&rptBuff[rptLen],Addr, 6);
        rptLen+=6;
        //启动时标
        rptBuff[rptLen++] = DT_DATETIME_S;
        DT_Time2DateTimeHex(RealTime_t,(DateTimeHex_t *)&rptBuff[rptLen]);
        rptLen+=7;
        //成功时标
        rptBuff[rptLen++] = DT_DATETIME_S;
        DT_Time2DateTimeHex(RealTime_t,(DateTimeHex_t *)&rptBuff[rptLen]);
        rptLen+=7;
        //存储时标
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
* 函数名称: WriteEventReportData
* 函数功能://将组好的电表上报事件内容写入数据中心
* 输入参数:pipe 端口号
        DataBuf 事件内容
        DataLen 长度
        Oad 写事件的oad

* 输出参数:
* 返 回 值:
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
    TASK_FMT_DEBUG( pipe, RELAY_LOG,"写事件内容到数据中心 infonum %d len%d \n",NomalOad.infoNum,sizeof(OOP_PERCEPTION_TERMINAL_LIST_T));

    //TASK_BUF_DEBUG( pipe, RELAY_LOG,DataBuf, DataLen);
    ret = db_write_nomal(pAmrThreadArg->DBClientHandle,&NomalOad,DataBuf,DataLen);
    if(ret !=0)
    {
        TASK_FMT_DEBUG( pipe, RELAY_LOG,"事件上报数据写入数据库失败 \n");
    }
}


/*******************************************************************************
* 函数名称: GetEventSubOad
* 函数功能://根据主oad到EventPlanList中查询下发方案中的子oad
* 输入参数:MainOad 主oad

* 输出参数: SubOad 方案中的子oad
* 返 回 值: 子oad个数
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
        //先判断表在不在这个方案的表集合中
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
        //如果不在表集合中则无需再找事件oad
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
    //如果没有配事件采集方案 就给个默认的
    if(suboadcount == 0)
    {
        suboadcount = 4;
        memcpy(SubOad,g_report_301f_obj,sizeof(g_report_301f_obj));
    }
    return suboadcount;
}

/*******************************************************************************
* 函数名称: EventReportSendFrame
* 函数功能:获取事件上报发送的报文
* 输入参数:pipe 端口号

* 输出参数:
* 返 回 值: 1表示有报文发送 2表示一块表抄完切换状态 -1表示所有表抄完
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
        TASK_FMT_DEBUG( pipe, RELAY_LOG,"Pn %d 失败次数超过3次 不再抄读  \n",Pn);
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
        TASK_FMT_DEBUG( pipe, RELAY_LOG,"Pn %d 开始抄645状态字\n",Pn);
//        if(pAmrThreadArg->EventReport.MtrEvent[MtrIndex].GetStateCount>3)
//        {
//            pAmrThreadArg->EventReport.MtrEvent[MtrIndex].state = EVENT_ONEMTR_END;
//            TASK_FMT_DEBUG( pipe, RELAY_LOG,"Pn %d 状态字抄太多次 结束  \n",Pn);
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
        TASK_FMT_DEBUG( pipe, RELAY_LOG,"Pn %d 开始抄读3320  \n",Pn);
    }
        break;
    case EVENT_GET_EVENT:
    {
        uint8 eventindex = pAmrThreadArg->EventReport.MtrEvent[MtrIndex].CurIndex;
        TASK_FMT_DEBUG( pipe, RELAY_LOG,"eventindex %d EventCount %d \n ",eventindex,pAmrThreadArg->EventReport.MtrEvent[MtrIndex].EventCount);
        //先判断 以防时钟故障之类不需要继续抄的事件没有复位状态字
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
//            TASK_FMT_DEBUG( pipe, RELAY_LOG,"Pn %d 所有上报状态字清零  \n",Pn);
//            return 2;
//        }
        EVENT_INFO_T *eventinfo =NULL;
        eventinfo = &(pAmrThreadArg->EventReport.MtrEvent[MtrIndex].EventInfo[eventindex]);
        TASK_FMT_DEBUG( pipe, RELAY_LOG,"PN %d prtl %d 事件当前抄到mainoad %08x SubOadNum %d  \n",Pn,prtl,eventinfo->EventDI.value,eventinfo->SubOadNum);
        if(prtl == PRTL_OOP || prtl == PRTL_20_IOT)
        {
            #if (defined AREA_JIANGXI) || (defined AREA_ANHUI)||(defined AREA_JIANGSU)
            if((eventinfo->EventDI.nObjID ==0x3060)||(eventinfo->EventDI.nObjID ==0x3061))
            {
                //面向对象表直接一帧抄一个事件
                uint8  rsd[] = {0x0A,0x01,0x01};
                uint8   InDataBuf[255];
                GetRecord_t getRecord;
                uint16 offset = 1;
                OOP_OAD_U MainOad;
                //OOP_OAD_U tempMainOad;
                uint16 tmpoffset;
                //OOP_OAD_U   RsdOad;
                //RsdOad.value= 0x20220200;//事件记录序号必抄
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
                RsdOad.value= 0x20220200;//事件记录序号必抄
                getRecord.rsd = rsd;
                getRecord.rsdLen = 2;
                getRecord.oad = eventinfo->EventDI;
                getRecord.rcsd = InDataBuf;
                getRecord.rcsd[offset++]=0;
                memcpy_r(&getRecord.rcsd[offset],(uint8*)&RsdOad,4);


                offset+=4;
                TASK_FMT_DEBUG( pipe, RELAY_LOG,"事件当前抄到mainoad %08x SubOadNum %d  \n",eventinfo->EventDI.value,eventinfo->SubOadNum);

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
            //面向对象表直接一帧抄一个事件
            uint8  rsd[] = {0x09,0x01};
            uint8   InDataBuf[255];
            GetRecord_t getRecord;
            uint16 offset = 1;
            OOP_OAD_U   RsdOad;
            RsdOad.value= 0x20220200;//事件记录序号必抄
            getRecord.rsd = rsd;
            getRecord.rsdLen = 2;
            getRecord.oad = eventinfo->EventDI;
            getRecord.rcsd = InDataBuf;
            getRecord.rcsd[offset++]=0;
            memcpy_r(&getRecord.rcsd[offset],(uint8*)&RsdOad,4);


            offset+=4;
            TASK_FMT_DEBUG( pipe, RELAY_LOG,"事件当前抄到mainoad %08x SubOadNum %d  \n",eventinfo->EventDI.value,eventinfo->SubOadNum);

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
            //物联网表抄20150200报上来的事件需要用逻辑地址1
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
            //645表需要好几帧
            TASK_FMT_DEBUG( pipe, RELAY_LOG,"事件当前抄到的数据项 CurDIIndex %d count %d  \n",eventinfo->CurDIIndex,eventinfo->count);

            if(eventinfo->CurDIIndex >=eventinfo->count)
            {
                TASK_FMT_DEBUG( pipe, RELAY_LOG,"当前事件全部抄完 抄下一个事件 \n");
                pAmrThreadArg->EventReport.MtrEvent[MtrIndex].CurIndex++;
                return 2;
            }
            uint32 SendDI = eventinfo->AllDI[eventinfo->CurDIIndex] ;
            TASK_FMT_DEBUG( pipe, RELAY_LOG,"645数据项 %08x  \n",SendDI);

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
        TASK_FMT_DEBUG( pipe, RELAY_LOG,"当前电表PN %d 事件抄完 Mtrindex %d \n",Pn,MtrIndex);
        if(prtl == PRTL_07)
        {
            TASK_FMT_DEBUG( pipe, RELAY_LOG,"645表事件抄完后存数据中心 datalen %d \n",pAmrThreadArg->EventReport.MtrEvent[MtrIndex].DataLen);
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
                TASK_FMT_DEBUG( pipe, RELAY_LOG,"所有电表事件全部抄完 任务结束\n");
                pAmrThreadArg->EventReport.CurIndex = 0;
                return -1;
            }
            if(pAmrThreadArg->EventReport.MtrEvent[MtrIndex].Pn!=0)
            {
                pAmrThreadArg->EventReport.CurIndex = MtrIndex;
                TASK_FMT_DEBUG( pipe, RELAY_LOG,"抄下一块表 index %d Pn %d \n",MtrIndex,pAmrThreadArg->EventReport.MtrEvent[MtrIndex].Pn);
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
        //向物联网表发送确认电能表跟随上报状态字
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
            //复位时逻辑地址用0
            Tsa.oldvxd = 0;
            OopAction.requst(pipe,&Tsa,1,buf,index);
            pLinkLayer = FindLinkLayer(pipe, NULL);
            if(pLinkLayer!=NULL)
            {
                Framelen = pLinkLayer->sendLen;
                memcpy(Framebuf,pLinkLayer->sendBuff,Framelen);
            }
            TASK_FMT_DEBUG( pipe, RELAY_LOG,"确认电能表跟随上报状态字 \n");

            break;
        }

        TASK_FMT_DEBUG( pipe, RELAY_LOG,"645电表抄完 复位上报状态字 \n");
        if(pAmrThreadArg->EventReport.MtrEvent[MtrIndex].ResetStateCount>=3)
        {
            pAmrThreadArg->EventReport.MtrEvent[MtrIndex].state = EVENT_ONEMTR_END;
            TASK_FMT_DEBUG( pipe, RELAY_LOG,"Pn %d 复位状态字失败太多次  \n",Pn);
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
        //根据上报的状态字复位
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
        //用003E并发抄表的方式下去
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


    TASK_FMT_DEBUG( pipe, RELAY_LOG,"电表事件上报发送 PN %d state %d len %d sendindex %d\n",Pn,
        pAmrThreadArg->EventReport.MtrEvent[MtrIndex].state,index,pAmrThreadArg->EventReport.SendIndex);
    TASK_BUF_DEBUG( pipe, RELAY_LOG,payload, index);
    return 1;
}

/*******************************************************************************
* 函数名称: EventReportSendFrame
* 函数功能:获取事件上报发送的报文
* 输入参数:pipe 端口号

* 输出参数:
* 返 回 值: 1表示有报文发送 2表示一块表抄完切换状态 -1表示所有表抄完
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
        TASK_FMT_DEBUG( pipe, RELAY_LOG,"Pn %d 失败次数超过3次 不再抄读  ",Pn);
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
        TASK_FMT_DEBUG( pipe, RELAY_LOG,"Pn %d 开始抄485状态字\n",Pn);
//        if(pAmrThreadArg->EventReport.MtrEvent[MtrIndex].GetStateCount>3)
//        {
//            pAmrThreadArg->EventReport.MtrEvent[MtrIndex].state = EVENT_ONEMTR_END;
//            TASK_FMT_DEBUG( pipe, RELAY_LOG,"Pn %d 状态字抄太多次 结束  \n",Pn);
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
        TASK_FMT_DEBUG( pipe, RELAY_LOG,"Pn %d 开始抄读3320  \n",Pn);
    }
        break;
    case EVENT_GET_EVENT:
    {
        uint8 eventindex = pAmrThreadArg->EventReport.MtrEvent[MtrIndex].CurIndex;
        TASK_FMT_DEBUG( pipe, RELAY_LOG,"eventindex %d EventCount %d  \n",eventindex,pAmrThreadArg->EventReport.MtrEvent[MtrIndex].EventCount);
        if(pAmrThreadArg->EventReport.MtrEvent[MtrIndex].EventCount ==0)
        {
            pAmrThreadArg->EventReport.MtrEvent[MtrIndex].state = EVENT_ONEMTR_END;
            TASK_FMT_DEBUG( pipe, RELAY_LOG,"Pn %d 所有上报状态字清零  \n",Pn);
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
        TASK_FMT_DEBUG( pipe, RELAY_LOG,"PN %d prtl %d 事件当前抄到mainoad %08x SubOadNum %d  \n",Pn,prtl,eventinfo->EventDI.value,eventinfo->SubOadNum);
        if(prtl == PRTL_OOP)
        {
            //面向对象表直接一帧抄一个事件
            uint8  rsd[] = {0x09,0x01};
            uint8   InDataBuf[255];
            GetRecord_t getRecord;
            uint16 offset = 1;
            OOP_OAD_U   RsdOad;
            RsdOad.value= 0x20220200;//事件记录序号必抄
            getRecord.rsd = rsd;
            getRecord.rsdLen = 2;
            getRecord.oad = eventinfo->EventDI;
            getRecord.rcsd = InDataBuf;
            getRecord.rcsd[offset++]=0;
            memcpy_r(&getRecord.rcsd[offset],(uint8*)&RsdOad,4);


            offset+=4;
            TASK_FMT_DEBUG( pipe, RELAY_LOG,"事件当前抄到mainoad %08x SubOadNum %d  \n",eventinfo->EventDI.value,eventinfo->SubOadNum);

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
            //645表需要好几帧
            TASK_FMT_DEBUG( pipe, RELAY_LOG,"事件当前抄到的数据项 CurDIIndex %d count %d  \n",eventinfo->CurDIIndex,eventinfo->count);

            if(eventinfo->CurDIIndex >=eventinfo->count)
            {
                TASK_FMT_DEBUG( pipe, RELAY_LOG,"当前事件全部抄完 抄下一个事件 \n");
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
        TASK_FMT_DEBUG( pipe, RELAY_LOG,"当前电表PN %d 事件抄完 Mtrindex %d \n",Pn,MtrIndex);
        if(prtl == PRTL_07)
        {
            TASK_FMT_DEBUG( pipe, RELAY_LOG,"645表事件抄完后存数据中心 datalen %d \n",pAmrThreadArg->EventReport.MtrEvent[MtrIndex].DataLen);
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
                TASK_FMT_DEBUG( pipe, RELAY_LOG,"所有电表事件全部抄完 任务结束\n  ");
                return -1;
            }
            if(pAmrThreadArg->EventReport.MtrEvent[MtrIndex].Pn!=0)
            {
                pAmrThreadArg->EventReport.CurIndex = MtrIndex;
                TASK_FMT_DEBUG( pipe, RELAY_LOG,"抄下一块表 index %d Pn %d \n",MtrIndex,pAmrThreadArg->EventReport.MtrEvent[MtrIndex].Pn);
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
        TASK_FMT_DEBUG( pipe, RELAY_LOG,"645电表抄完 复位上报状态字 \n",MtrIndex);
        if(pAmrThreadArg->EventReport.MtrEvent[MtrIndex].ResetStateCount>=3)
        {
            pAmrThreadArg->EventReport.MtrEvent[MtrIndex].state = EVENT_ONEMTR_END;
            TASK_FMT_DEBUG( pipe, RELAY_LOG,"Pn %d 复位状态字失败太多次  \n",Pn);
            return 2;
        }
        MeterPrtlFmt_T  MeterPrtlData;
        uint8 Send[]={0x03,0x15,0x00,0x04,0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
        //根据上报的状态字复位
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

    TASK_FMT_DEBUG( pipe, RELAY_LOG,"事件上报发送PN %d state %d len %d \n",Pn,pAmrThreadArg->EventReport.MtrEvent[MtrIndex].state,index);
    TASK_BUF_DEBUG( pipe, RELAY_LOG,Framebuf, Framelen);
    
    return 1;
}




/*******************************************************************************
* 函数名称: EventReportMqtt
* 函数功能://mqtt线程中收到上报事件后的处理
* 输入参数:buf 消息内容
        len 消息长度

* 输出参数:
* 返 回 值:
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
    OOP_OAD_U oad ;//转发端口对象描述符
    memcpy_r((uint8 *) &oad, &buf[index] , sizeof(OOP_OAD_U));
    pipe = taskmng_mtr_pipe_get(&oad);
    //TASK_BUF_TRACE(-1, RELAY_LOG,buf,len);
    //TASK_FMT_TRACE(-1, RELAY_LOG, "pipe is %d,oad.value is %x\n",pipe,oad.value);
    if(pipe>1)
        return;
    index+=4;
    devicetype = buf[index++]; //设备类型 0 采集器 1 电表
    prtl = buf[index++];//00H为保留；01H 为DL/T645—1997；02H 为DL/T645—2007；03H代表698.45,04H代表从节点停、复电事件报文，05H代表拒绝节点信息上报报文
    DataLen = XADR_GetEleNum(&buf[index],&lenoffset);
    index+=lenoffset;
    memcpy(DataBuf,&buf[index],DataLen);
    index+=DataLen;

    //容错 如果消息长度不对不处理
    if(index+1>len)
        return;

    TASK_FMT_DEBUG(pipe, RELAY_LOG, "规约类型 %d 设备类型 %d 将上报事件发送至对应线程 datalen %d databuf \n",prtl,devicetype,DataLen);
    TASK_BUF_DEBUG(pipe, RELAY_LOG, DataBuf, DataLen);
    if(prtl == 4)
    {
        //停上电事件上报
        if(devicetype == 0)
            ret = task_send_msg(gMqttSock,MSG_POWER_ON_OFF_COL,DataBuf,DataLen,IPC_AMR+pipe);
        else
            ret = task_send_msg(gMqttSock,MSG_POWER_ON_OFF_MET,DataBuf,DataLen,IPC_AMR+pipe);
    }else if(prtl == 5)
    {
        //节点拒绝信息上报
        ret = task_send_msg(gMqttSock,MSG_NODE_REJECT,DataBuf,DataLen,IPC_AMR+pipe);
    }else
    {
        //电表事件上报或者非本台区电表上报
        ret = task_send_msg(gMqttSock,MSG_EVENT_REPORT,DataBuf,DataLen,IPC_AMR+pipe);
    }
    if(ret!=0)
        TASK_FMT_DEBUG(-1,MQTT_LOG,"将上报事件发送给抄表线程出错 ret %d \n",ret);

}




/*******************************************************************************
* 函数名称: ReportEventStateFun
* 函数功能:处理645主动上报的状态字 buf由12个字节的状态字开始 然后AA 每个事件的发生次数 AA
* 输入参数:pipe 端口号
        buf 消息内容
        len 消息长度
        index 电表索引
* 输出参数:
* 返 回 值: 0 状态字全0 1 没有需要抄的事件直接复位状态字 2 有需要抄的事件 -1 报文不合法
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
    pEventRecordNum = buf+12; //从AA开始 1正好代表第一个事件的次数
    for(i=0;i<96;i++)
    {
        if((buf[i/8] & (0x01<<(i%8)))!=0)
        {
            EventCount++;
            if(EventCount+12+2>len) //数据长度出错 12状态字 再加2AA
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
                    //抄上几次事件
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
    //全0 代表没有事件
    if(EventCount==0)
        return 0;

    //1表示有事件发生 但是不需要抄具体事件内容
    if(MtrEvent->EventCount>0)
        return 2;
    else
        return 1;
}




/*******************************************************************************
* 函数名称: SetEventReportNum
* 函数功能:设置Oad对应的信息点号
* 输入参数:InfoNum 信息点号
        Oad
* 输出参数:
* 返 回 值:
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
* 函数名称: taskmng_event_report_num_get
* 函数功能:获取Oad对应的信息点号
* 输入参数:Oad

* 输出参数:
* 返 回 值: 信息点号
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
* 函数名称: GetCurrentEventReportNum
* 函数功能:获取Oad对应的信息点号
* 输入参数:Oad

* 输出参数:
* 返 回 值: 信息点号
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
* 函数名称: taskmng_event_report_num_load
* 函数功能://上电时读取数据库 获取Oad对应的最大信息点号
* 输入参数:

* 输出参数:
* 返 回 值:
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
* 函数名称: AddEventReport
* 函数功能://645抄表时显示有后续帧
* 输入参数:Addr 电表地址

* 输出参数:
* 返 回 值:
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
            TASK_FMT_DEBUG( pipe, RELAY_LOG, "添加事件上报任务 i %d pn %d \n",i,Pn);
            taskmng_plc_task_queue_add(pipe,MS_EVENT_REPORT);
            pAmrThreadArg->EventReport.AllCount++;
            break;
        }
    }

}

/*******************************************************************************
* 函数名称: DealEventReport
* 函数功能://645抄表时显示有后续帧
* 输入参数:Addr 电表地址

* 输出参数:
* 返 回 值:
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
    TASK_FMT_DEBUG( pipe, RELAY_LOG, "添加645_07事件上报任务\n");  
    RS485EventReportDeal(pipe);
}

/*******************************************************************************
* 函数名称: OOPFollowReport
* 函数功能://698有跟随上报域 如果是33200201则主动抄产生的事件
* 输入参数:pipe 端口号
        Pn 电表号
        buf 报文内容
        len 报文长度

* 输出参数:
* 返 回 值:
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
    if(len<1+1+4+1 +1+4) //跟随上报域
        return;
    uint16 index = 0;
    if(buf[index++]!=1)    //若干个对象属性及数据 [1] SEQUENCE OF A-ResultNormal
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
        TASK_FMT_DEBUG( pipe, RELAY_LOG,"跟随上报域上报的事件 oad %08x 未找到对应的方案\n",oad.value);
        return;
    }
    TASK_FMT_DEBUG( pipe, RELAY_LOG,"跟随上报域上报的事件 oad %08x 准备抄读\n",oad.value);
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
            TASK_FMT_DEBUG( pipe, RELAY_LOG, "添加事件上报任务 i %d  pn %d\n",i,newpn);
            taskmng_plc_task_queue_add(pipe,MS_EVENT_REPORT);
            pAmrThreadArg->EventReport.AllCount++;
            break;
        }
    }
}

/*******************************************************************************
* 函数名称: OOPFollowReport
* 函数功能://698有跟随上报域 如果是33200201则主动抄产生的事件
* 输入参数:pipe 端口号
        Pn 电表号
        buf 报文内容
        len 报文长度

* 输出参数:
* 返 回 值:
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
    if(len<1+1+4+1 +1+4) //跟随上报域 
        return;
    uint16 index = 0;
    if(buf[index++]!=1)    //若干个对象属性及数据 [1] SEQUENCE OF A-ResultNormal
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
        TASK_FMT_DEBUG( pipe, RELAY_LOG,"跟随上报域上报的事件 oad %08x 未找到对应的方案\n",oad.value);
        return;
    }
    TASK_FMT_DEBUG( pipe, RELAY_LOG,"跟随上报域上报的事件 oad %08x 准备抄读\n",oad.value);

   

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
* 函数名称: taskmng_normal_data_delete
* 函数功能:删除普通数据
* 输入参数:infoNum 信息点号
    Oad

* 输出参数:
* 返 回 值:
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
        TASK_FMT_TRACE(-1, MQTT_LOG, "删除成功 infonum %d oad %08x \n",infoNum,Oad.value);
    }else
    {
        TASK_FMT_TRACE(-1, MQTT_LOG, "删除失败 infonum %d oad %08x \n",infoNum,Oad.value);
    }

}



/*******************************************************************************
* 函数名称: HPLCInfoUpFun
* 函数功能:MQTT线程收到上报的HPLC信息进行处理
* 输入参数:buf 消息内容
        len 消息长度

* 输出参数:
* 返 回 值:
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
* 函数名称: DeleteHPLCInfo
* 函数功能:删除上一轮查询的HPLC信息
* 输入参数:pipe 端口号

* 输出参数:
* 返 回 值:
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
* 函数名称: taskmng_task_monitor_update
* 函数功能://更新任务监控参数
* 输入参数:pipe 端口号
        TaskId 任务号
        cmd 更新命令
        indata 更新命令带的参数
* 输出参数:
* 返 回 值:
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
            TASK_FMT_TRACE( pipe, REC_LOG, " 当前端口没有电表，无需更新任务监控参数\n");
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
            TASK_FMT_TRACE( pipe, REC_LOG, "taskmng_task_monitor_update 未找到监控单元 \n");
        return;
    }
    indata->ThisTurnRealBeginTime[6] = 0x20;
    indata->ThisTurnRealBeginTime[5] = commfun_HexToBcd((uint8)(taskMonitorUnit->startTime.year%100));  //年
    indata->ThisTurnRealBeginTime[4] = commfun_HexToBcd((uint8)(taskMonitorUnit->startTime.month));     //月
    indata->ThisTurnRealBeginTime[3] = commfun_HexToBcd((uint8)taskMonitorUnit->startTime.day);         //日
    indata->ThisTurnRealBeginTime[2] = commfun_HexToBcd((uint8)taskMonitorUnit->startTime.hour);            //时
    indata->ThisTurnRealBeginTime[1] = commfun_HexToBcd((uint8)taskMonitorUnit->startTime.minute);          //分
    indata->ThisTurnRealBeginTime[0] = commfun_HexToBcd((uint8)taskMonitorUnit->startTime.second);          //分
    pthread_mutex_unlock(&TaskMonitor.MonitorLock);
}








/**
*********************************************************************
* @brief:      实现  485透明方案抄读功能；
* @param[in]：   pipe  线程号,485模块共8个线程,485的线程pipe从2开始到9
* @param[in]：   SCHEME_TASK_RUN_INFO *TaskInfo  任务参数
* @param[in]：  CTS_RUN_TASK_T* RunTask 运行时任务信息
* @return：1 全部抄完 -1 未抄完
*********************************************************************
*/
int Rs485TransDataReading(uint8 pipe,SCHEME_TASK_RUN_INFO *TaskInfo,CTS_RUN_TASK_T* RunTask)
{
    AmrThreadArg_T  *pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    uint16 i;
    int             RecvLen = 0;
    uint8           RecvBuf[MAX_MSG_LEN] = {0};
    //uint8           PayloadBuf[MAX_MSG_LEN] = {0};        //payload缓冲区
    //MSG_INFO_T* MsgInfo = NULL;
    //AMR_PLC_INFO_REPORT_T  AmrReportInfo;    //上报的抄读数据
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
                pAmrThreadArg->pMtrAmrInfo[i].ROADIndex = 0;//透明任务中使用作为每块表抄到的方案报文的索引
                pAmrThreadArg->pMtrAmrInfo[i].BroadFailCnt=0;

            }
            //taskmng_task_record_file_load(pipe,TaskInfo,RunTask);
            TASK_FMT_TRACE( pipe, REC_LOG, "taskmng_data_reading_trans 清空发送队列\n");
            RunTask->StartTime = time(NULL);
            pAmrThreadArg->TotalState.NormalAmr = RTS_SEND;
        }
            break;
        case RTS_SEND:
        {
            if(Rs485TransFrameSend(pipe,TaskInfo,RunTask)==1)
            {
                TASK_FMT_TRACE( pipe, REC_LOG, "全部抄完 透明方案任务%d 结束&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&\n",TaskInfo->TaskId);
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
                TASK_FMT_TRACE(pipe, REC_LOG,"@@@@@收到抄表上报数据@@@@@@@@@@ \n");
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
                        TASK_FMT_TRACE(pipe, REC_LOG,"oop规约没有收到合法的抄表报文\n");
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
                    ptr = (uint8*)commfun_645_frame_get((char*)RecvBuf, 0x68, RecvLen,&reallen);//找到645报文的68头
                    if(ptr == NULL)
                    {
                       TASK_FMT_TRACE(pipe, REC_LOG,"645规约没有收到合法的抄表报文\n");
                        pAmrThreadArg->pMtrAmrInfo[MtrIndex].BroadFailCnt = 0;
                        pAmrThreadArg->pMtrAmrInfo[MtrIndex].ROADIndex++;
                        pAmrThreadArg->AmrSendFrameInfo.Queue[SendIndex].ValidFlag=0;
                        if(pAmrThreadArg->AmrSendFrameInfo.QueueCnt>0)
                            pAmrThreadArg->AmrSendFrameInfo.QueueCnt--;
                        pAmrThreadArg->TotalState.NormalAmr = RTS_SEND;
                        break;
                    }
                    RecvLen = reallen;//纯645报文长度
                    if(RecvLen <=0)
                    {
                        TASK_FMT_TRACE(pipe, REC_LOG,"645规约没有收到合法的抄表报文\n");
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
                            TASK_FMT_TRACE( pipe, REC_LOG, "Queue index %d 超时 PN %d \n",i,pAmrThreadArg->AmrSendFrameInfo.Queue[i].Pn);
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
* 函数名称: Rs485TransFrameSend
* 函数功能://485透明任务获取发送数据
* 输入参数:pipe 端口号
        TaskInfo 任务参数
        RunTask 任务运行时参数
* 输出参数:
* 返 回 值:1 全部抄完 -1 未抄完
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
                pAmrThreadArg->pMtrAmrInfo[MsIndex].LastSendTime = time(NULL);//当前电表最近一次发送时间
                pAmrThreadArg->AmrSendFrameInfo.Queue[i].PrtlMode = prtl;
                memcpy(pAmrThreadArg->AmrSendFrameInfo.Queue[i].MeterAddr,MtrAddr,6);
                //用FrameSeq存储透明方案当前发送电表在任务中的索引
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
* 函数名称: taskmng_register_app_check
* 函数功能:检查指定APP是否注册成功
* 输入参数:AppName APP名称
* 输出参数:
* 返 回 值:TRUE 注册成功 FALSE 注册失败
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
    TASK_FMT_DEBUG( -1, UP_LOG, " 查询APP %s 注册信息  \n",AppName);
    ret = taskmng_up_msg_fun_deal(IID,IOP,len,PayLoad,DestAddr,&RecvLen,RecvBuf);
    if(ret>0)
    {
        TASK_FMT_DEBUG( -1, UP_LOG, " 查询APP %s 注册信息成功！RecvLen %d  \n",AppName,RecvLen);
        TASK_BUF_DEBUG( -1, UP_LOG,RecvBuf,RecvLen);
        if(RecvLen>0)
        {
            char ResultName[100]={0};
            uint16 index = 0;
            uint8 namelen = RecvBuf[index++];
            memcpy(ResultName,&RecvBuf[index],namelen);
            TASK_FMT_DEBUG( -1, UP_LOG, " 查询到的APPname %s   \n",ResultName);
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
* 函数名称: SaveEventPowerOnOff
* 函数功能://存储停上电事件
* 输入参数:EventType 1 停电 0上电
* 输出参数:
* 返 回 值:
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
    rptBuff[rptLen++] = 1;  //结果类型
    rptBuff[rptLen++] = 1;  //
    rptBuff[rptLen++] = DT_TSA;
    rptBuff[rptLen++] = 7;
    rptBuff[rptLen++] = 5;
    memcpy_r(&rptBuff[rptLen],Addr,6);
    rptLen+=6;
    msindex = GetMsindexFromAddr(pipe,Addr);
    if(msindex==-1)
    {
        TASK_FMT_DEBUG(pipe, RELAY_LOG, "该电表不在档案中 不进行存储  \n");
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
        //将停电时间写入内存
        if(pAmrThreadArg->pMtrAmrInfo[msindex].powerstate == 1)
        {
            TASK_FMT_DEBUG(pipe, RELAY_LOG, "该电表重复上报停电事件 直接过滤 \n");
            return;
        }
        pAmrThreadArg->pMtrAmrInfo[msindex].powerofftime = timenow;

        pAmrThreadArg->pMtrAmrInfo[msindex].powerstate = 1;
        Eventseq = pAmrThreadArg->pMtrAmrInfo[msindex].powereventseq;
        TASK_FMT_DEBUG(pipe, RELAY_LOG, "停电事件序号 %d \n",Eventseq);
        //停电 事件结束时间为NULL
        rptBuff[rptLen++] = DT_NULL;
    }else
    {
        //只有停过电才报上电事件
        if(pAmrThreadArg->pMtrAmrInfo[msindex].powerstate!=1)
        {
            TASK_FMT_DEBUG(pipe, RELAY_LOG, "该电表没有停电事件 不上报  \n");
            return;
        }
        powerofftime = pAmrThreadArg->pMtrAmrInfo[msindex].powerofftime;
        Eventseq = pAmrThreadArg->pMtrAmrInfo[msindex].powereventseq;
        TASK_FMT_DEBUG(pipe, RELAY_LOG, "上电事件序号 %d \n",Eventseq);
        pAmrThreadArg->pMtrAmrInfo[msindex].powerstate = 0;
        pAmrThreadArg->pMtrAmrInfo[msindex].powereventseq++;
        //电表上电事件的“发生时间”应为电表的停电时间，如果终端没有收到该电表的停电事件，则“发生时间”应为空
        if(powerofftime==0)
        {
            rptBuff[rptLen++] = DT_NULL;
        }else
        {

            rptBuff[rptLen++] = DT_DATETIME_S;
            DT_Time2DateTimeHex(powerofftime,(DateTimeHex_t *)&rptBuff[rptLen]);
            rptLen+=7;
        }
        //上电 事件结束时间有效
        rptBuff[rptLen++] = DT_DATETIME_S;
        DT_Time2DateTimeHex(timenow,(DateTimeHex_t *)&rptBuff[rptLen]);
        rptLen+=7;
    }
    rptBuff[rptLen++] = DT_DOUBLE_LONG_UNSIGNED;
    memcpy_r(&rptBuff[rptLen],(uint8*)&Eventseq,4);
    rptLen+=4;
    //事件发生源
    rptBuff[rptLen++] = DT_OAD;
    memcpy_r(&rptBuff[rptLen],&pipeoad.value,4);
    rptLen+=4;
    //事件上报状态
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
* 函数名称: taskmng_search_meter_clean_load
* 函数功能://从数据中心加载60020800 搜表结果标志
* 输入参数:
* 输出参数:
* 返 回 值:
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
        TASK_FMT_TRACE(-1,UP_LOG, "加载60020800失败 ret %d  \n",ret);
        SearchMeterClean.isvalid = 0;
    }else
    {
        SearchMeterClean.isvalid = 1;
        memcpy(&SearchMeterClean.searchparam,&clear,sizeof(OOP_SEARCHPARAM_T));
    }
    pthread_mutex_unlock(&SearchMeterClean.lock);
}

/*******************************************************************************
* 函数名称: taskmng_search_meter_param_load
* 函数功能://从数据中心加载60020900 周期搜表参数配置
* 输入参数:
* 输出参数:
* 返 回 值:
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
        TASK_FMT_TRACE(-1,UP_LOG, "加载周期搜表参数失败 ret %d  \n",ret);
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
* 函数名称: Loadtaskmng_perception_tmn_info_load
* 函数功能://从数据中心加载60020900 周期搜表参数配置
* 输入参数:
* 输出参数:
* 返 回 值:
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
* 函数名称: SearchMeterCheck
* 函数功能:检查是否存在周期搜表
* 输入参数:
* 输出参数:
* 返 回 值:
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
            TASK_FMT_DEBUG(pipe, RELAY_LOG, "到了周期搜表时间 hour %d minute %d second %d   \n",param.start.hour,
                param.start.minute,param.start.second);
            if(pAmrThreadArg->MeterSearch.IsInSearch==0)
            {
                pAmrThreadArg->IsNeedSearch = 2;
                pAmrThreadArg->MeterSearch.SearchTime = param.time;
                pthread_mutex_unlock(&SearchMeterParam.Lock);
                return;
            }
        }
        //即装即采测试时会对时到搜表中间的时间 所有得再加个判断
    }
    pthread_mutex_unlock(&SearchMeterParam.Lock);
    return;
}


/*******************************************************************************
* 函数名称: DBInitFun
* 函数功能://处理数据中心 数据初始化事件
* 输入参数:
* 输出参数:
* 返 回 值:
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
    TASK_FMT_DEBUG(-1,MQTT_LOG,"收到数据初始化事件 \n");

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
    //恢复出厂设置 或者数据类标识为2的数据初始化才处理
    if(flag == 1)
    {
        //直接发送档案初始化 可同步进行任务初始化
        dbupdate.logicid = 0xff;    //此处使用0xff表示所有逻辑设备号均删除
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
* 函数名称: taskmng_sync_time_param_load
* 函数功能:从数据中心获取广播校时相关参数
* 输入参数:
* 输出参数:
* 返 回 值:
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
        TASK_FMT_DEBUG(-1,UP_LOG, "加载广播校时参数失败 ret %d  \n",ret);
        SyncTimingParam.SyncTime.bValid = 0;
    }else
    {
        //240712 24规范中校时要避开整点五分钟 所以如果是5分钟内 就改成5分钟
        if(SyncTime.time.minute < 5)
        {
            TASK_FMT_DEBUG(-1,UP_LOG, "广播校时参数优化 原来 hour %d min %d sec %d  \n",SyncTime.time.hour,SyncTime.time.minute,SyncTime.time.second);
            SyncTime.time.minute = 5;
            SyncTime.time.second = 0;
        }
        if(SyncTime.time.minute >= 55)
        {
            TASK_FMT_DEBUG(-1,UP_LOG, "广播校时参数优化 原来 hour %d min %d sec %d  \n",SyncTime.time.hour,SyncTime.time.minute,SyncTime.time.second);
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
        TASK_FMT_DEBUG(-1,UP_LOG, "0x42040200广播校时参数 hour %d min %d sec %d  valid %d \n",SyncTime.time.hour,
            SyncTime.time.minute,SyncTime.time.second,SyncTime.bValid);
    }

    NormalOad.oad.value = 0x42040300;

    ret = db_read_nomal(gDBClientMain,&NormalOad,sizeof(OOP_P2PSYNCTIME_T),(uint8*)&P2PSyncTime,&Len);
    if(ret!=0 || Len != sizeof(OOP_P2PSYNCTIME_T))
    {
        TASK_FMT_DEBUG(-1,UP_LOG, "加载单地址广播校时参数失败 ret %d  \n",ret);
        SyncTimingParam.P2PSycnTime.bValid = 0;
    }else
    {
        if(P2PSyncTime.time.hour == 0xFF)
        {
            SyncTimingParam.P2PSycnTime.bValid = 0;
            TASK_FMT_DEBUG(-1,UP_LOG, "单地址广播校时参数优化时间为FF认为参数无效 原来 hour %d min %d sec %d  \n",P2PSyncTime.time.hour,P2PSyncTime.time.minute,P2PSyncTime.time.second);
        }
         //240712 24规范中校时要避开整点五分钟 所以如果是5分钟内 就改成5分钟
        if(P2PSyncTime.time.minute < 5)
        {
            TASK_FMT_DEBUG(-1,UP_LOG, "单地址广播校时参数优化 原来 hour %d min %d sec %d  \n",P2PSyncTime.time.hour,P2PSyncTime.time.minute,P2PSyncTime.time.second);
            P2PSyncTime.time.minute = 5;
            P2PSyncTime.time.second = 0;
        }
        if(P2PSyncTime.time.minute >= 55)
        {
            TASK_FMT_DEBUG(-1,UP_LOG, "单地址广播校时参数优化 原来 hour %d min %d sec %d  \n",P2PSyncTime.time.hour,P2PSyncTime.time.minute,P2PSyncTime.time.second);
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
        TASK_FMT_DEBUG(-1,UP_LOG, "0x42040300单地址广播校时参数 hour %d min %d sec %d  valid %d offset %d \n",P2PSyncTime.time.hour,
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
* 函数名称: SyncTimingCheck
* 函数功能://检查是否存在定时广播校时任务
* 输入参数:
* 输出参数:
* 返 回 值:
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
            TASK_FMT_DEBUG(pipe, RELAY_LOG, "到了广播校时时间 hour %d minute %d second %d   \n",gCurBinTime[pipe].Hour,
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
            TASK_FMT_DEBUG(pipe, RELAY_LOG, "到了单地址广播校时时间 hour %d minute %d second %d   \n",gCurBinTime[pipe].Hour,
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
* 函数名称: BroadTiming
* 函数功能://广播对时
* 输入参数:
* 输出参数:
* 返 回 值:
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
        TASK_FMT_DEBUG(pipe, RELAY_LOG, "广播校时参数无效 任务结束\n");
        pAmrThreadArg->TotalState.SyncTimeState = INNER_RTS_IDLE;
        pAmrThreadArg->BroadTiming.TimingType=0;
        return 1;
    }
    switch(pAmrThreadArg->TotalState.SyncTimeState)
    {
    //初始态
    case INNER_RTS_IDLE:
    {
        pAmrThreadArg->BroadTiming.MtrIndex = 0;
        pAmrThreadArg->BroadTiming.state = TMN_TO_METER_IDLE;
        pAmrThreadArg->BroadTiming.FailCnt = 0;
        TASK_FMT_DEBUG(pipe, RELAY_LOG, "广播对时任务初始化 timingtype %d\n",pAmrThreadArg->BroadTiming.TimingType);
        if(pAmrThreadArg->BroadTiming.TimingType == 1)
        {
            pAmrThreadArg->BroadTiming.state = TMN_TO_METER_GET_DELAY;
            pAmrThreadArg->BroadTiming.BroadPrtl = PRTL_97;//规约1 2 3 轮询
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
            TASK_FMT_DEBUG(pipe, RELAY_LOG, "广播校时任务结束  \n");
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

            TASK_FMT_DEBUG( pipe, RELAY_LOG, "广播校时收到消息 IID %04X IOP %04X MsgLen %d payload \n",MsgInfo->IID,MsgInfo->IOP,MsgInfo->msglen);
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
                TASK_FMT_DEBUG(pipe, RELAY_LOG, "广播校时%d秒超时  \n",BROAD_TIMING_OVERTIME);
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
                TASK_FMT_DEBUG(pipe, RELAY_LOG, "广播校时失败次数超过3次 直接任务结束  \n");
                pAmrThreadArg->TotalState.SyncTimeState = INNER_RTS_IDLE;
                pAmrThreadArg->BroadTiming.TimingType = 0;
                return 1;
            }
        }
        else
        {
            if(pAmrThreadArg->BroadTiming.FailCnt>3)
            {
                TASK_FMT_DEBUG(pipe, RELAY_LOG, "单地址广播校时失败次数超过3次 转下一个电表 \n");
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
* @brief:      实现  485广播校时和单表校时的功能；
* @param[in]：   pipe  线程号,485模块共8个线程,485的线程pipe从2开始到9
* @return：1 广播校时执行成功 -1 广播校时执行失败
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
        //初始态
        case INNER_RTS_IDLE:
        {
            TASK_FMT_DEBUG(pipe, RELAY_LOG, "pAmrThreadArg->BroadTiming.TimingType is %d\n",pAmrThreadArg->BroadTiming.TimingType);
            //如果没有档案 就不广播校时
            if(pAmrThreadArg->TmnAmrInfo.MtrCnt == 0)
            {
                TASK_FMT_DEBUG(pipe, RELAY_LOG, "当前端口没有档案 不广播校时\n");
                return 1;
            }
            pAmrThreadArg->BroadTiming.MtrIndex = 0;
            pAmrThreadArg->BroadTiming.state = TMN_TO_METER_IDLE;
            pAmrThreadArg->BroadTiming.FailCnt = 0;
            if(pAmrThreadArg->BroadTiming.TimingType == 1)
            {
                pAmrThreadArg->BroadTiming.state = TMN_TO_METER_SETTIME_INIT;
                pAmrThreadArg->BroadTiming.BroadPrtl = PRTL_07;//规约1 2 3 轮询
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
                TASK_FMT_DEBUG(pipe, RELAY_LOG, "广播校时任务结束  \n");
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
                TASK_FMT_DEBUG(pipe, RELAY_LOG, "收到消息 \n");
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
                    TASK_FMT_DEBUG(pipe, RELAY_LOG, "广播校时%d秒超时  \n",BROAD_TIMING_OVERTIME);
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
                TASK_FMT_DEBUG(pipe, RELAY_LOG, "单地址广播校时失败次数超过3次 转下一个电表 \n");
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
* 函数名称: BroadTimingRecv
* 函数功能://广播校时收到回码处理
* 输入参数:
* 输出参数:
* 返 回 值:
*******************************************************************************/
void BroadTimingRecv(uint8 pipe,uint8* buf,uint16 len)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    switch(pAmrThreadArg->BroadTiming.state)
    {
    //收到广播延时
    case TMN_TO_METER_GET_DELAY:
    {
        if(len!=2)
            return;
        if(pAmrThreadArg->BroadTiming.TimingType == 1)
        {
            memcpy_r((uint8*)&pAmrThreadArg->BroadTiming.BroadDelay,buf,2);
            TASK_FMT_DEBUG(pipe, RELAY_LOG, "获取到广播延时 %d  \n",pAmrThreadArg->BroadTiming.BroadDelay);
        }else
        {
            uint16 MtrIndex = pAmrThreadArg->BroadTiming.MtrIndex;
            memcpy_r((uint8*)&pAmrThreadArg->pMtrAmrInfo[MtrIndex].TimeDelay,buf,2);
            TASK_FMT_DEBUG(pipe, RELAY_LOG, "获取从节点监控延时 %d  \n",pAmrThreadArg->pMtrAmrInfo[MtrIndex].TimeDelay);
        }
        pAmrThreadArg->BroadTiming.state = TMN_TO_METER_SETTIME_INIT;
    }
        break;
    //收到广播的回码
    case TMN_TO_METER_SETTIME_INIT:
    {
        if(pAmrThreadArg->BroadTiming.TimingType == 1)
        {
            if(buf[0]==1)
            {
                //面向对象广播完后检查下有没有物联网表 如果有就需要用逻辑地址1再广播一遍
                if(pAmrThreadArg->BroadTiming.BroadPrtl == PRTL_OOP)
                {
                    if(TRUE == taskmng_iot_prtl_check(pipe))
                    {
                        pAmrThreadArg->BroadTiming.BroadPrtl = PRTL_20_IOT;
                        TASK_FMT_DEBUG(pipe, RELAY_LOG, "物联网表广播校时 \n");
                        //两个广播间加上延时
                        sleep(5);
                        break;
                    }
                }
                if(pAmrThreadArg->BroadTiming.BroadPrtl == PRTL_20_IOT)
                {
                    pAmrThreadArg->BroadTiming.state = TMN_TO_METER_SETTIME_CIPHER;
                    TASK_FMT_DEBUG(pipe, RELAY_LOG, "物联网表密文广播校时 \n");
                    //两个广播间加上延时
                    sleep(5);
                    break;
                }
                //一种规约广播对时完后下一个规约
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
    //收到查询时钟回码
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
* 函数名称: BroadTimingRecv
* 函数功能://广播校时收到回码处理
* 输入参数:
* 输出参数:
* 返 回 值:
*******************************************************************************/
void Rs485BroadTimingRecv(uint8 pipe,uint8* buf,uint16 len)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    switch(pAmrThreadArg->BroadTiming.state)
    {
//    //收到广播延时
//    case TMN_TO_METER_GET_DELAY:
//    {
//        if(len!=2)
//            return;
//        if(pAmrThreadArg->BroadTiming.TimingType == 1)
//        {
//            memcpy_r((uint8*)&pAmrThreadArg->BroadTiming.BroadDelay,buf,2);
//            TASK_FMT_DEBUG(pipe, RELAY_LOG, "获取到广播延时 %d  \n",pAmrThreadArg->BroadTiming.BroadDelay);
//        }else
//        {
//            uint16 MtrIndex = pAmrThreadArg->BroadTiming.MtrIndex;
//            memcpy_r((uint8*)&pAmrThreadArg->pMtrAmrInfo[MtrIndex].TimeDelay,buf,2);
//            TASK_FMT_DEBUG(pipe, RELAY_LOG, "获取从节点监控延时 %d  \n",pAmrThreadArg->pMtrAmrInfo[MtrIndex].TimeDelay);
//        }
//        pAmrThreadArg->BroadTiming.state = TMN_TO_METER_SETTIME_INIT;
//    }
//        break;
    //收到广播的回码
    case TMN_TO_METER_SETTIME_INIT:
    {
        if(pAmrThreadArg->BroadTiming.TimingType == 1)
        {
            //if(buf[0]==1)
            {
                //面向对象广播完后检查下有没有物联网表 如果有就需要用逻辑地址1再广播一遍
                if(pAmrThreadArg->BroadTiming.BroadPrtl == PRTL_OOP)
                {
                    if(TRUE == taskmng_iot_prtl_check(pipe))
                    {
                        pAmrThreadArg->BroadTiming.BroadPrtl = PRTL_20_IOT;
                        TASK_FMT_DEBUG(pipe, RELAY_LOG, "物联网表广播校时 \n");
                        //两个广播间加上延时
                        sleep(5);
                        break;
                    }
                }
                if(pAmrThreadArg->BroadTiming.BroadPrtl == PRTL_20_IOT)
                {
                    pAmrThreadArg->BroadTiming.state = TMN_TO_METER_SETTIME_CIPHER;
                    TASK_FMT_DEBUG(pipe, RELAY_LOG, "物联网表密文广播校时 \n");

                    break;
                }
                //一种规约广播对时完后下一个规约
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
    //收到查询时钟回码
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
* 函数名称: SaveMeterTimingEvent
* 函数功能://将电表对时信息以FFA00100的oad写入数据中心
* 输入参数:
* 输出参数:
* 返 回 值:
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

    TASK_FMT_DEBUG(pipe, RELAY_LOG, "写入电表校时信息 Pn %d \n",Pn);

}



/*******************************************************************************
* 函数名称: SaveMeterTime
* 函数功能://收到查询时钟回码进行处理
* 输入参数:
* 输出参数:
* 返 回 值:
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
            //645刚抄了年月日
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
                TASK_FMT_DEBUG(pipe, RELAY_LOG, "当前电表时钟 time_t %d \n",metertime);
                TASK_BUF_DEBUG(pipe, RELAY_LOG,&pAmrThreadArg->pMtrAmrInfo[MtrIndex].MeterTime,7);
                pAmrThreadArg->pMtrAmrInfo[MtrIndex].TimeError = abs(time(NULL)-metertime);
                //获取电表时间误差后 继续获取从节点监控延时
                if(pAmrThreadArg->pMtrAmrInfo[MtrIndex].TimeError <pAmrThreadArg->BroadTiming.limit )
                {
                    TASK_FMT_DEBUG(pipe, RELAY_LOG, "当前电表 %d 和终端时差 %d 小于阈值 %d 无需校时 \n",Pn,
                        pAmrThreadArg->pMtrAmrInfo[MtrIndex].TimeError,pAmrThreadArg->BroadTiming.limit);
                    pAmrThreadArg->BroadTiming.MtrIndex++;
                    pAmrThreadArg->BroadTiming.FailCnt = 0;
                    pAmrThreadArg->BroadTiming.state =TMN_TO_METER_RDINIT_FIRST;
                }else
                {
                    TASK_FMT_DEBUG(pipe, RELAY_LOG, "当前电表 %d 和终端时差 %d 大于阈值 %d 需要校时 \n",Pn,
                        pAmrThreadArg->pMtrAmrInfo[MtrIndex].TimeError,pAmrThreadArg->BroadTiming.limit);
                    //pAmrThreadArg->BroadTiming.state = TMN_TO_METER_GET_DELAY;
                    // 直接设置
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
        if(ret >=eOopComm_Fail)//解帧失败直接返回
        {
            pAmrThreadArg->BroadTiming.MtrIndex++;
            pAmrThreadArg->BroadTiming.FailCnt = 0;
            pAmrThreadArg->BroadTiming.state =TMN_TO_METER_RDINIT_FIRST;
            break ;
        }
        //获取电表地址
        pAppLayer = FindAppLayer(pipe, NULL);
        if(pAppLayer == NULL)
            break;
        datalen = pAppLayer->recvApduList->apduLen-pAppLayer->recvApduList->offSet;
        memcpy(databuf, pAppLayer->recvApduList->apdu+pAppLayer->recvApduList->offSet, datalen);
        TASK_BUF_TRACE(pipe, RELAY_LOG,databuf,datalen);
        //databuf从oad后面的结果类型开始
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
        //获取电表时间误差后 继续获取从节点监控延时
        if(pAmrThreadArg->pMtrAmrInfo[MtrIndex].TimeError <pAmrThreadArg->BroadTiming.limit )
        {
            TASK_FMT_DEBUG(pipe, RELAY_LOG, "当前电表 %d 和终端时差 %d 小于阈值 %d 无需校时 \n",Pn,
                pAmrThreadArg->pMtrAmrInfo[MtrIndex].TimeError,pAmrThreadArg->BroadTiming.limit);
            pAmrThreadArg->BroadTiming.MtrIndex++;
            pAmrThreadArg->BroadTiming.FailCnt = 0;
            pAmrThreadArg->BroadTiming.state =TMN_TO_METER_RDINIT_FIRST;
        }else
        {
            TASK_FMT_DEBUG(pipe, RELAY_LOG, "当前电表 %d 和终端时差 %d 大于阈值 %d 需要校时 \n",Pn,
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
* 函数名称: BroadTimingSend
* 函数功能://广播校时发送帧
* 输入参数:
* 输出参数:
* 返 回 值:  返回1 有数据发送 返回0 任务结束 返回2 切换发送状态
*******************************************************************************/
int BroadTimingSend(uint8 pipe)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    switch(pAmrThreadArg->BroadTiming.state)
    {
    //获取延时
    case TMN_TO_METER_GET_DELAY:
    {
        if(pAmrThreadArg->BroadTiming.TimingType == 1)
        {
            if(GetBroadDelay(pipe)>0)
            {
                break;
            }else
            {
                TASK_FMT_DEBUG(pipe, RELAY_LOG, "广播校时所有规约全部发送完 任务结束 \n");
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
    //查询电表时钟
    case TMN_TO_METER_RDINIT_FIRST:
    {
        if(GetMeterTime(pipe)>0)
        {
            break;
        }else
        {
            TASK_FMT_DEBUG(pipe, RELAY_LOG, "单地址广播校时所有表全部发送完 任务结束 \n");
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
* @brief:      实现  485广播校时和单表校时的功能；
* @param[in]：   pipe  线程号,485模块共8个线程,485的线程pipe从2开始到9
* @return：返回1 有数据发送 返回0 任务结束 返回2 切换发送状态
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
    //查询电表时钟
    case TMN_TO_METER_RDINIT_FIRST:
    {
        if(Rs485GetMeterTime(pipe)>0)
        {
            break;
        }else
        {
            TASK_FMT_DEBUG(pipe, RELAY_LOG, "单地址广播校时所有表全部发送完 任务结束 \n");
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
* 函数名称: SetMeterTime
* 函数功能://通过从节点监控设置电表时间
* 输入参数:
* 输出参数:
* 返 回 值:
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
        TASK_FMT_DEBUG(pipe, RELAY_LOG, "设置电表 %d 时钟 \n",Pn);
        //taskmng_router_ctrl_set ( pipe,RCS_PAUSE );
        if(prtl == PRTL_20_IOT)
            prtl = PRTL_OOP;
        SendFrame_0038(pipe,prtl,Addr,databuf,datalen,&pAmrThreadArg->BroadTiming.FrameIndex);
        return 1;
    }
    return 0;
}

/*******************************************************************************
* 函数名称: SetMeterTime
* 函数功能://通过从节点监控设置电表时间
* 输入参数:
* 输出参数:
* 返 回 值:
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

    TASK_FMT_TRACE( pipe, RELAY_LOG, "发送datalen is %d\n",datalen);
    if(datalen>0)
    {
        TASK_BUF_TRACE(pipe, RELAY_LOG, databuf, datalen);
        ret=rs485_RS232_send(pipe,MAX_FRAME_OVERTIME,MAX_CHAR_OVERTIME,speed,PARITY_EVEN,databuf, datalen);
        if(ret !=0)
        {
            TASK_FMT_TRACE( pipe, RELAY_LOG, "发送结果 ret is %d\n",ret);
        }
        return 1;
    }
    return 0;
}
/*******************************************************************************
* 函数名称: GetMeterDelay
* 函数功能://获取每个电表的从节点监控延时
* 输入参数:
* 输出参数:
* 返 回 值:
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
    TASK_FMT_DEBUG(pipe, RELAY_LOG, "发送获取从节点监控延时命令 MsgIndex %d  \n",pAmrThreadArg->BroadTiming.FrameIndex);
    TASK_BUF_DEBUG(pipe, RELAY_LOG,SendBuf,index);
    return 1;
}


/*******************************************************************************
* 函数名称: GetMeterTime
* 函数功能://查询电表时钟
* 输入参数:
* 输出参数:
* 返 回 值:
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
        TASK_FMT_DEBUG(pipe, RELAY_LOG, "查询电表 %d 时钟 \n",Pn);
        //taskmng_router_ctrl_set ( pipe,RCS_PAUSE );
        SendFrame_0038(pipe,prtl,Addr,SendBuf,len,&pAmrThreadArg->BroadTiming.FrameIndex);
    }
    return 1;
}

/*******************************************************************************
* 函数名称: GetMeterTime
* 函数功能://查询电表时钟
* 输入参数:
* 输出参数:
* 返 回 值:
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
//        rs485_RS232_UnInit(&Rs485ComNo[pipe-2]);//先临时添加
//        TASK_FMT_TRACE( pipe, COMM_LOG, "初始化失败,pipe is %d\n",pipe);
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
        TASK_FMT_DEBUG(pipe, RELAY_LOG, "查询电表 %d 时钟,len is %d\n",Pn,len);

        ret=rs485_RS232_send(pipe,MAX_FRAME_OVERTIME,MAX_CHAR_OVERTIME,MeterBasicInfo.baud,PARITY_EVEN,SendBuf, len);
        if(ret !=0)
        {
            TASK_FMT_TRACE( pipe, COMM_LOG, "发送结果 ret is %d\n",ret);
        }
    }
    return 1;
}

/*******************************************************************************
* 函数名称: SendFrame_0038
* 函数功能://组从节点监控发送
* 输入参数:
* 输出参数:
* 返 回 值:
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
* 函数名称: SendBroadTiming
* 函数功能://发送广播校时
* 输入参数:
* 输出参数:
* 返 回 值:
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
* 函数名称: SendBroadTiming
* 函数功能://发送广播校时
* 输入参数:
* 输出参数:
* 返 回 值:
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
//        rs485_RS232_UnInit(&Rs485ComNo[pipe-2]);//先临时添加
//        TASK_FMT_TRACE( pipe, COMM_LOG, "初始化失败,pipe is %d\n",pipe);
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
                TASK_FMT_TRACE( pipe, RELAY_LOG, "广播校时发送 speed %d\n",speed);
                TASK_BUF_TRACE(pipe, RELAY_LOG, buf, len);
                ret=rs485_RS232_send(pipe,2,MAX_CHAR_OVERTIME,speed,PARITY_EVEN,buf, len);
                if(ret !=0)
                {
                    TASK_FMT_TRACE( pipe, RELAY_LOG, "发送结果 ret is %d\n",ret);
                }
                sleep(3);
            }
        }
        return 1;
    }
    return 0;
}
/*******************************************************************************
* 函数名称: GetBroadDelay
* 函数功能://获取广播延时
* 输入参数:
* 输出参数:
* 返 回 值:  1有发送 0 没有
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
        //如果当前档案中没有对应的规约则不需要进行广播
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
* 函数名称: PackSetTimeFrame
* 函数功能://组装校时报文
* 输入参数:delay 设置时间需要加上的延时
        isBroad是否广播
* 输出参数:
* 返 回 值:  1有发送 0 没有
*******************************************************************************/
uint16 PackSetTimeFrame(uint8 pipe,uint8* outBuf,uint8 prtl,uint8 isBroad,uint16 delay)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    uint16 len=0;
    //适配台体 组校时报文时只在50毫秒前
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
* 函数名称: taskmng_update_cycle_load
* 函数功能://从数据中心加载0XF2090600 更新周期
* 输入参数:
* 输出参数:
* 返 回 值:
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
        TASK_FMT_DEBUG(-1,UP_LOG, "加载0XF2090600更新周期失败 ret %d  \n",ret);
        UpdateCycle.Ti = 1440; //如果没有设置则默认周期为一天
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
                //加了个单位变成FF表示不定期查询
                case 0xff:
                    UpdateCycle.Ti = 0;
                    break;
                default:
                    UpdateCycle.Ti = 1440;
                    break;
            }
        }
        UpdateCycle.isSet = 1;
        TASK_FMT_DEBUG(-1,UP_LOG, "加载0XF2090600更新周期为 %d  \n",UpdateCycle.Ti );
    }
    pthread_mutex_unlock(&UpdateCycle.Lock);
}


/*******************************************************************************
* 函数名称: taskmng_plc_pipe_set
* 函数功能:抄表端口配置
* 输入参数:pipe  端口
        choice 0 删除 1 添加
* 输出参数:
* 返 回 值:
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
    TASK_FMT_DEBUG( pipe, COMM_LOG, "配置抄表端口 \n");
    taskmng_msg_fun_deal(pipe,IID,IOP,len,Buffer,&RecvLen,RecvBuf);
    if(RecvLen==1 && RecvBuf[0]==1)
    {
        TASK_FMT_DEBUG( pipe, COMM_LOG, "抄表端口配置成功 \n");
    }
}




/*******************************************************************************
* 函数名称: Appversion_Response
* 函数功能:收到iid 0000 IOP0013后回复版本信息
* 输入参数:
* 输出参数:
* 返 回 值:
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
    TASK_FMT_DEBUG(-1,UP_LOG, "查询返回版本信息 SourAddr %s DestAddr %s \n",MsgSend->souraddr,MsgSend->destaddr);
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
* 函数名称: taskmng_oop_frame_broadtiming_check
* 函数功能:判断698帧是否为广播校时帧
* 输入参数:buf 报文内容
         len 报文长度
* 输出参数: settime 如果是广播校时帧则为校时的时间
* 返 回 值:  TRUE FALSE
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
    //co表示广播地址
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
    //操作请 求           [7]        ACTION-Request，
    uint8 requestchoice = buf[offset++];
    if(requestchoice!=SERVERID_ACTIONREQUEST &&requestchoice!=SERVERID_SETREQUEST)
    {
        return FALSE;
    }
    //请求操作一个对象方法                        [1] ActionRequestNormal，
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
* 函数名称: taskmng_frame_3762_format_check
* 函数功能: 检查3762帧格式
* 输入参数: FrameLen            帧长度
*           FrameBuff           帧数据

* 输出参数: FrameEnd            帧结束偏移
* 返 回 值:  =0                  有效帧
*           <0                  无效帧
*******************************************************************************/
int8 taskmng_frame_3762_format_check(uint16 FrameLen, uint8 *FrameBuff, uint16 *FrameEnd)
{
    uint16  i = 0;
    uint16  DataIndex = 0;                      //指针偏移
    uint16  FrameDataLen=0;                     //帧数据总长度
    uint8   CheckSum = 0;                       //校验和

    //查找帧起始符68H
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

    //无效帧
    if (DataIndex >= FrameLen)
    {
        *FrameEnd = FrameLen;
        return -1;
    }

    //计算帧数据长度
    memcpy(&FrameDataLen, &FrameBuff[DataIndex+1], 2);


    //容错处理
    if ((DataIndex + FrameDataLen) > FrameLen)
    {
        *FrameEnd = FrameLen;
        return -2;
    }

    //检测帧尾
    if (FrameBuff[DataIndex+FrameDataLen-1] != 0x16)
    {
        *FrameEnd = DataIndex+1;
        return -3;
    }

    //检验和的范围为控制域+用户数据区
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
* 函数名称: LoadHPLCChannel
* 函数功能://从数据中心加载F2090900宽带载波频段序号
* 输入参数:
* 输出参数:
* 返 回 值:
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
        TASK_FMT_DEBUG(-1,UP_LOG, "读取宽带载波频段序号失败 ret %d  \n",ret);

    }else
    {
        HPLCChannel = channel;
        for(i=0;i<PLC_NUM;i++)
        {
            ret = task_send_msg(gMainSock,MSG_HPLC_CHANNEL_CHANGE,&HPLCChannel,1,IPC_AMR+i);
            TASK_FMT_DEBUG(-1,UP_LOG, "通知抄表线程设置宽带载波频段 %d ret %d \n",HPLCChannel,ret);
        }
    }
}
#endif
/*******************************************************************************
* 函数名称: taskmng_HPLC_across_area_flag_load
* 函数功能://从数据中心加载F2090B00台区识别启停标记
* 输入参数:
* 输出参数:
* 返 回 值:
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
        TASK_FMT_DEBUG(-1,UP_LOG, "读取台区识别启停标记 ret %d  \n",ret);

    }else
    {
        HPLCAcrossAreaFlag = flag;
        for(i=0;i<PLC_NUM;i++)
        {
            task_send_msg(gMainSock,MSG_HPLC_ACROSSAREA_CHANGE,&HPLCAcrossAreaFlag,1,IPC_AMR+i);
            TASK_FMT_DEBUG(-1,UP_LOG, "通知抄表线程设置台区识别启停标记 %d  \n",HPLCAcrossAreaFlag);
        }
    }
}

/*******************************************************************************
* 函数名称: taskmng_HPLC_node_reject_flag_load
* 函数功能://从数据中心加载F2091600拒绝节点上报使能标志
* 输入参数:
* 输出参数:
* 返 回 值:
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
        TASK_FMT_DEBUG(-1,UP_LOG, "读取拒绝节点上报使能标志 ret %d  \n",ret);

    }else
    {
        HPLCNodeRejectFlag= flag;
        for(i=0;i<PLC_NUM;i++)
        {
            task_send_msg(gMainSock,MSG_HPLC_NODEREJECT_CHANGE,&HPLCNodeRejectFlag,1,IPC_AMR+i);
            TASK_FMT_DEBUG(-1,UP_LOG, "通知抄表线程设置拒绝节点上报使能标志 %d  \n",HPLCNodeRejectFlag);
        }
    }
}

#if 0
/*******************************************************************************
* 函数名称: SetHPLCChannel
* 函数功能: //给本地通信APP设置HPLC频点
* 输入参数: pipe 端口号
* 输出参数:
* 返 回 值:
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
        TASK_FMT_DEBUG(pipe,RELAY_LOG,"设置HPLC频点成功 \n");
    }

}
#endif





/*******************************************************************************
* 函数名称: taskmng_search_meter_clear_param_get
* 函数功能: //获取清空搜表结果选项 如果未设置则返回-1
* 输入参数:
* 输出参数:
* 返 回 值:
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
* 函数名称: MeterSearchResultClear
* 函数功能: //清空内存中搜表结果
* 输入参数:
* 输出参数:
* 返 回 值:
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
* 函数名称: taskmng_across_area_result_load
* 函数功能: //上电时加载数据中心中的非本台区电能表上报信息 做去重用
* 输入参数:
* 输出参数:
* 返 回 值:
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

    TASK_FMT_DEBUG(-1,TASK_LOG, "开始加载非本台区电能表上报结果 \n");


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
        TASK_FMT_DEBUG(-1,TASK_LOG, "开始读非本台区电能表上报结果失败 ret  %d recNum %d \n",ret,recNum);
    }else
    {
        TASK_FMT_DEBUG(-1,TASK_LOG, "非本台区电能表上报结果 recNum %d \n",recNum);
        for(i=0;i<recNum;i++)
        {
            ret = db_read_record_step(gDBClientMain, handle, &stepReadRecord, &stepRecordOut);
            if(ret!=0)
            {
                TASK_FMT_DEBUG(-1,TASK_LOG, "单步读非本台区电能表上报结果记录失败 ret  %d i %d \n",ret,i);
                continue;
            }
            if(stepRecordOut.recordnum!=1 || stepRecordOut.record[0].road.oadCols.oad[0].value!=0x60020500
                ||stepRecordOut.record[0].outDatalen!=sizeof(OOP_ACROSSAREA_RES_T))
            {
                TASK_FMT_DEBUG(-1,TASK_LOG, "单步读取的非本台区电能表上报结果有问题 \n");
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
            TASK_FMT_DEBUG(-1,TASK_LOG, "加载的非本台区电能表上报记录 %d infonum %d  count %d 地址 \n",i,stepRecordOut.infoNum,AcrossAreaResult.count);
            TASK_BUF_DEBUG(-1,TASK_LOG, Result.commAdd.add,6);
        }
    }
    db_read_record_end(gDBClientMain, handle);
    pthread_mutex_unlock(&AcrossAreaResult.Lock);
}

/*******************************************************************************
* 函数名称: CheckAcrossAreaResult
* 函数功能: //检测上报的非本台区电表信息是否重复，如果重复返回false如果未重复则写入AcrossAreaResult
* 输入参数:
* 输出参数: 如果未重复 则返回信息点号
* 返 回 值:
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
* 函数名称: taskmng_across_area_result_change_fun
* 函数功能: //收到60020500数据变更事件，查询数据中心，和当前内存数据进行比较，新增或者删除
* 输入参数:
* 输出参数: 如果未重复 则返回信息点号
* 返 回 值:
*******************************************************************************/
void taskmng_across_area_result_change_fun(uint16 infonum)
{
    int ret;
    int i;
    int index = -1;
    OOP_ACROSSAREA_RES_T result;
    memset(&result,0x00,sizeof(OOP_ACROSSAREA_RES_T));
    TASK_FMT_DEBUG(-1,TASK_LOG, "收到60020500数据变更事件 infonum %d  \n",infonum);
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
            //结果删除
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
            //插看是否在当前内存中有
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
* 函数名称: taskmng_across_area_result_read_from_pn
* 函数功能: 根据信息点号从数据中心读取费本台区电表结果
* 输入参数:
            infonum

* 输出参数: result
* 返 回 值:  0 成功

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
* 函数名称: taskmng_meter_search_result_read_from_pn
* 函数功能: 根据信息点号从数据中心读取搜表结果
* 输入参数:
            infonum

* 输出参数: result
* 返 回 值:  0 成功

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
* 函数名称: taskmng_meter_search_result_change_fun
* 函数功能: //收到60020200数据变更事件，查询数据中心，和当前内存数据进行比较，新增或者删除
* 输入参数:
* 输出参数: 如果未重复 则返回信息点号
* 返 回 值:
*******************************************************************************/
void taskmng_meter_search_result_change_fun(uint16 infonum)
{
    int ret;
    int i;
    int index = -1;
    OOP_SEARCHMET_RES_T result;
    memset(&result,0x00,sizeof(OOP_SEARCHMET_RES_T));
    TASK_FMT_DEBUG(-1,TASK_LOG, "收到60020200搜表结果数据变更事件 infonum %d  \n",infonum);
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
            //结果删除
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
            //查看是否在当前内存中有
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
* @brief： get_app_id 获取系统管理器appid
* @param[in]：
               argc      参数个数
               argv[]    参数列表
* @param[out]：
               none        出参说明
* @return： 返回appid长度
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
* 函数名称: Appid_Response
* 函数功能:收到iid 0000 IOP0016后回复appid
* 输入参数:
* 输出参数:
* 返 回 值:
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
    TASK_FMT_DEBUG(-1,UP_LOG, "查询返回APPID SourAddr %s DestAddr %s \n",MsgSend->souraddr,MsgSend->destaddr);
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
* 函数名称: taskmng_db_update_queue_read
* 函数功能: 读队列
* 输入参数：pLockQueue          锁缓冲区结构指针
*           buf                 读数据缓冲区，将队列中的数据取出放入该缓冲区
* 输出参数：plockqueue          队列中的数据
* 内部处理: 判队列中是否有数据，有则从队列中取出数据放入buf中，每执行一次，取一个数据
* 返 回 值: len                 读到的数据长度
*******************************************************************************/
int32 taskmng_db_update_queue_read(DBUPDATE_QUEUE_T *pLockQueue, DB_UPDATE_T *buf)
{
    int32 len = 0;
    //uint8 *pdata_buf;

    //锁缓冲区
    pthread_mutex_lock(&pLockQueue->QueueLock);

    //若队列中有数据，则取到buf中，解开锁
    if ((pLockQueue->QueueHead != pLockQueue->QueueTail)
        || ((pLockQueue->QueueHead == pLockQueue->QueueTail)
            && (pLockQueue->Queue[pLockQueue->QueueHead].isvalid != 0)))
    {
        //pdata_buf =&(pLockQueue->Queue[pLockQueue->QueueHead].dbupdate);
        len = sizeof(DB_UPDATE_T);

        //取队列中的数据
        memcpy(buf, &(pLockQueue->Queue[pLockQueue->QueueHead].dbupdate), len);

        //清数据
        memset(&(pLockQueue->Queue[pLockQueue->QueueHead].dbupdate), 0, len);
        pLockQueue->Queue[pLockQueue->QueueHead].isvalid = 0;

        //队列头向后移动一位
        pLockQueue->QueueHead= (pLockQueue->QueueHead + 1) % (MAX_DBUPDATE_QUEUE_CNT);
    }

    pthread_mutex_unlock(&pLockQueue->QueueLock);

    return len;
}

/*******************************************************************************
* 函数名称: taskmng_db_update_queue_write
* 函数功能: 写队列
* 输入参数：plockqueue          锁缓冲区结构指针
            buf                 需要写入队列的数据
            buf_len             数据长度
* 输出参数：无
* 内部处理: 若队列不满，将buf中的数据放入队列中；若队列满，丢弃该数据
* 返 回 值: >0                  成功
            =0                  失败
*******************************************************************************/
int32 taskmng_db_update_queue_write(DBUPDATE_QUEUE_T *pLockQueue, DB_UPDATE_T *buf)
{
    int32 flags = 0;

    //锁缓冲区
    pthread_mutex_lock(&pLockQueue->QueueLock);
    //判队列中是否还有空的位置
    if (pLockQueue->Queue[pLockQueue->QueueTail].isvalid == 0)
    {
        pLockQueue->Queue[pLockQueue->QueueTail].isvalid = 1;
        memcpy(&(pLockQueue->Queue[pLockQueue->QueueTail].dbupdate), buf, sizeof(DB_UPDATE_T));
        flags = 1;
        pLockQueue->QueueTail = (pLockQueue->QueueTail + 1) % (MAX_DBUPDATE_QUEUE_CNT);
    }

    //解缓冲区
    pthread_mutex_unlock(&pLockQueue->QueueLock);

    return flags;
}





/*******************************************************************************
* 函数名称: ReNetworking
* 函数功能: //轮询线程中的每个电表 如果某个表一直抄表失败添加到链表中
* 输入参数: pipe 端口号

* 输出参数:   pHead 需要重新组网的电表链表
* 返 回 值: 0 不需要重新组网 1 需要
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
        //TASK_FMT_TRACE( pipe, RELAY_LOG, "电表 %d 总失败次数 %d 重新组网次数 %d LastRecvTime %ld \n",
                 //   pMeterInfo->Pn,pMeterInfo->AllFailCnt,pMeterInfo->ReNetworkingCount,pMeterInfo->LastRecvTime);
        if(pMeterInfo->AllFailCnt>=MAX_METER_ALL_FAILURE_COUNT &&time(NULL)-pMeterInfo->LastRecvTime>MAX_METER_ALL_FAILURE_TIME)
        {
            if(pMeterInfo->ReNetworkingCount<1&&time(NULL)-pMeterInfo->LastReNetwokingTime>MAX_METER_ALL_FAILURE_TIME)
            {
                TASK_FMT_TRACE( pipe, RELAY_LOG, "电表 %d 总失败次数 %d 重新组网次数 %d  需要重新组网\n",
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
* 函数名称: ReNetworking
* 函数功能: //轮询线程中的每个电表 如果某个表一直抄表失败则删除ccoRouter的档案再重新添加 触发重新组网
* 输入参数: pipe 端口号

* 输出参数:
* 返 回 值:
*******************************************************************************/
int ReNetworking(uint8 pipe)
{
    AmrThreadArg_T  *pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    int ret;
    switch(pAmrThreadArg->TotalState.ReNetworkingState)
    {
        //判断是否需要重新组网
        case MFMS_IDLE:
        {
            if(IsNeedReNetworking(pipe)>0)
            {
                pAmrThreadArg->TotalState.ReNetworkingState = MFMS_FILE_DELETE;
            }else
            {
                TASK_FMT_TRACE( pipe, RELAY_LOG,"无需重新组网 \n" );
                return 1;
            }
        }
            break;
        //先删除档案
        case MFMS_FILE_DELETE:
        {

            if(pAmrThreadArg->pReNetworkingDelHead == NULL)
            {
                pAmrThreadArg->TotalState.ReNetworkingState = MFMS_PAUSE;
                pAmrThreadArg->ReNetworkingDelay = 10;  //延时10秒后添加档案
            }else
            {
                TASK_FMT_DEBUG( pipe, RELAY_LOG, "ReNetworking 档案删除  \n");
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
        //再添加档案
        case MFMS_FILE_ADD:
        {

            if(pAmrThreadArg->pReNetworkingAddHead == NULL)
            {
                pAmrThreadArg->TotalState.ReNetworkingState = MFMS_IDLE;
                TASK_FMT_DEBUG( pipe, RELAY_LOG,  "ReNetworking执行结束  \n");
                return 1;
            }else
            {
                TASK_FMT_DEBUG( pipe, RELAY_LOG,  "ReNetworking 档案下发  \n");
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
名称 taskmng_curve_point_get
功能 计算负荷曲线抄读点数
参数  无
返回值: 负荷曲线抄读点数
***********************************************************************/
uint8 taskmng_curve_point_get(uint16 taskid,uint8 logicid)
{
    //默认抄8个点
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
* 函数名称: taskmng_record_file_delete_pipe
* 函数功能:收到任务管理线程发送的任务变更消息，删除任务记录文件
* 输入参数:pipe  端口
        TaskId 任务id
* 输出参数:
* 返 回 值:
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
                    TASK_FMT_TRACE( pipe, REC_LOG, "删除metrno %ld 日冻结数据\n",metrno);
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
                    TASK_FMT_TRACE( pipe, REC_LOG, "删除metrno %ld 的曲线补抄数据\n",metrno);

                }
             }
         }

    }
    //write_pdata_xram(pfile,(char *)&OopDayAddReadTime,0,sizeof(OopCrvAddReadTime_t)*OOP_DAY_ADDREAD_TASKNUM);
    //write_pdata_xram(pfile,(char *)&OopCrvAddReadTime,0,sizeof(OopCrvAddReadTime_t)*OOP_CRV_ADDREAD_TASKNUM);
}


/*******************************************************************************
* 函数名称: taskmng_record_file_delete_pipe
* 函数功能:收到任务管理线程发送的任务变更消息，删除任务记录文件
* 输入参数:pipe  端口
        TaskId 任务id
* 输出参数:
* 返 回 值:
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
        TASK_FMT_TRACE( pipe, REC_LOG, "删除metrno %ld 日冻结数据\n",MeterFileAll.MeterFile[i].nIndex);


        sprintf(pfile,"%s/%s_%02d_MtrNo%02d",PrivateDir,PATCH_FILE,2,MeterFileAll.MeterFile[i].nIndex);

        write_pdata_xram(pfile,(char *)&OopCrvAddReadTime,0,sizeof(OopCrvAddReadTime_t)*OOP_CRV_ADDREAD_TASKNUM);
        TASK_FMT_TRACE( pipe, REC_LOG, "删除metrno %ld 的曲线补抄数据\n",MeterFileAll.MeterFile[i].nIndex);
    }
    pthread_rwlock_unlock(&MeterFileAll.rwLock);


}

/*******************************************************************************
* 函数名称: taskmng_energy_oad_check
* 函数功能: 判断0ad是否属于电能量类
* 输入参数: oad 要抄读的数据项
* 输出参数:
* 返 回 值: true 属于
        FALSE 不属于
*******************************************************************************/

//BOOL taskmng_energy_oad_check(OOP_OAD_U oad)
//{
//    uint8 OIA1;
//    //冻结时标也要用冻结来抄
//    OIA1=(oad.nObjID>>12)&0x0F;
//    if(OIA1 == 0)
//        return TRUE;
//    return FALSE;
//}
/*******************************************************************************
* 函数名称: PackSetTimeFrame_cipher
* 函数功能://组装校时报文 广播密文
* 输入参数:delay 设置时间需要加上的延时
        isBroad是否广播
* 输出参数:
* 返 回 值:  1有发送 0 没有
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
    TASK_FMT_TRACE( pipe, RELAY_LOG, "校时密文 %d\n",len);

    return len;
}


/*******************************************************************************
* 函数名称: SendBroadTimingCipher
* 函数功能://发送广播校时 密文
* 输入参数:
* 输出参数:
* 返 回 值:
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
* 函数名称: SendBroadTimingCipher
* 函数功能://发送广播校时 密文
* 输入参数:
* 输出参数:
* 返 回 值:
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
            TASK_FMT_TRACE( pipe, RELAY_LOG, "广播校时发送 speed %d\n",speed);
            TASK_BUF_TRACE(pipe, RELAY_LOG, buf, len);
            ret=rs485_RS232_send(pipe,MAX_FRAME_OVERTIME,MAX_CHAR_OVERTIME,speed,PARITY_EVEN,buf, len);
            if(ret !=0)
            {
                TASK_FMT_TRACE( pipe, RELAY_LOG, "发送结果 ret is %d\n",ret);
            }
            sleep(3);
        }
//
//
//        ret=rs485_RS232_send(pipe,0,10,PARITY_EVEN,buf, len);
//        if(ret !=0)
//        {
//            TASK_FMT_TRACE( pipe, COMM_LOG, "发送结果 ret is %d\n",ret);
//        }
//        return 1;
    }
    return 0;
}

#ifdef PRODUCT_ECU
/*******************************************************************************
* 函数名称: taskmng_all_module_info_get
* 函数功能: 获取所有模组信息
* 输入参数:
* 输出参数:
* 返 回 值:
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

            TASK_FMT_DEBUG(-1,UP_LOG,"获取到模组信息 槽位号 %d 模块设备型号 %s 设备ID %s \n",tmp->nSlot,tmp->modType.value,tmp->modId.value);
            TASK_FMT_DEBUG(-1,UP_LOG,"软件版本 %s 发布日期 %s \n",tmp->nSoftVer.value,tmp->softDate.value);
            TASK_FMT_DEBUG(-1,UP_LOG,"硬件版本 %s 发布日期 %s \n",tmp->nHardVer.value,tmp->hardDate.value);
            TASK_FMT_DEBUG(-1,UP_LOG,"厂商编码 %s 通道个数 %d \n",tmp->manuf.value,tmp->nChNum);

            for(j=0;j<tmp->nChNum;j++)
            {
                tmp->chInfo[j].nType = RecvBuf[index++];
                tmp->chInfo[j].nFunc = RecvBuf[index++];
                lentmp = get_len_offset(&RecvBuf[index], &lenoffset);
                index+=lenoffset;
                memcpy(tmp->chInfo[j].nDriveName.value,&RecvBuf[index],lentmp);
                tmp->chInfo[j].nDriveName.nNum = lentmp;
                index+=lentmp;

                TASK_FMT_DEBUG(-1,UP_LOG,"通道 %d 接口类型 %d 功能配置 %d 驱动名称 %s\n",j,tmp->chInfo[j].nType,tmp->chInfo[j].nFunc,tmp->chInfo[j].nDriveName.value);
            }
            moduleinfo.nNum++;
        }

        pthread_mutex_lock(&gModuleInfoLock);
        memcpy(&gModuleInfo,&moduleinfo,sizeof(OOP_MODULE_INFO_NEW_T));
        pthread_mutex_unlock(&gModuleInfoLock);

    }
    else
    {
        TASK_FMT_DEBUG(-1,UP_LOG,"获取模组信息失败！\n");
    }

}

/*******************************************************************************
* 函数名称: ModuleEventFun
* 函数功能: //收到模组插拔事件后通知主线程
* 输入参数: buf 消息内容
            len 消息长度
* 输出参数:
* 返 回 值:
*******************************************************************************/
void ModuleEventFun(uint8* buf,uint16 len)
{

    uint32 ModuleIndex=0;
    uint16 index=0;
    uint8 status;
    uint8 j;
    //模组序号
    memcpy_r((uint8*)&ModuleIndex,&buf[index],4);
    index+=4;
    status = buf[index++];//插拔状态 0插 1拔
    TASK_FMT_DEBUG(-1,UP_LOG,"收到模块插拔事件 模组序号 %d status %d 0插 1拔\n",ModuleIndex,status);
    if(ModuleIndex<1||ModuleIndex>5)
        return ;

    if(status == 0)
    {
        //解析模组信息
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

        TASK_FMT_DEBUG(-1,UP_LOG,"插入的模组信息 槽位号 %d 模块设备型号 %s 设备ID %s \n",ModuleIndex,unit.modType.value,unit.modId.value);
        TASK_FMT_DEBUG(-1,UP_LOG,"软件版本 %s 发布日期 %s \n",unit.nSoftVer.value,unit.softDate.value);
        TASK_FMT_DEBUG(-1,UP_LOG,"硬件版本 %s 发布日期 %s \n",unit.nHardVer.value,unit.hardDate.value);
        TASK_FMT_DEBUG(-1,UP_LOG,"厂商编码 %s 通道个数 %d \n",unit.manuf.value,unit.nChNum);

        for(j=0;j<unit.nChNum;j++)
        {
            unit.chInfo[j].nType = buf[index++];
            unit.chInfo[j].nFunc = buf[index++];
            lentmp = get_len_offset(&buf[index], &lenoffset);
            index+=lenoffset;
            memcpy(unit.chInfo[j].nDriveName.value,&buf[index],lentmp);
            unit.chInfo[j].nDriveName.nNum = lentmp;
            index+=lentmp;

            TASK_FMT_DEBUG(-1,UP_LOG,"通道 %d 接口类型 %d 功能配置 %d 驱动名称 %s\n",j,unit.chInfo[j].nType,unit.chInfo[j].nFunc,unit.chInfo[j].nDriveName.value);
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

//适用于数据元素大于等于2个数组整体右移
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
            //TASK_FMT_TRACE( Pipe, REC_LOG,"任务 %d 已停用 无需更新轮次 \n",pAmrThreadArg->pTaskRunInfo[i].TaskId);
            continue;
        }
        TASK_FMT_TRACE ( pipe,REC_LOG, "找任务11\n");
        memset(&TaskInfo,0x00,sizeof(SCHEME_TASK_RUN_INFO));
        taskmng_task_info_get(pAmrThreadArg->pTaskRunInfo[i].TaskId,pAmrThreadArg->pTaskRunInfo[i].logicid, &TaskInfo);
        if(TaskInfo.CsdTask.RoadTask[0].MainOad.value == 0x50020200)
        {
            TASK_FMT_TRACE ( pipe,REC_LOG, "找到任务22\n");
            pAmrThreadArg->pTaskRunInfo[i].MeterIndex =0;
            pAmrThreadArg->pTaskRunInfo[i].CbNum =0;
            TASK_FMT_TRACE ( pipe,REC_LOG, "找到任务33 is %d, is %d\n",pAmrThreadArg->pTaskRunInfo[i].CbNum,pAmrThreadArg->pTaskRunInfo[i].NeedMtrNum);
            while((pAmrThreadArg->pTaskRunInfo[i].CbNum)<pAmrThreadArg->pTaskRunInfo[i].NeedMtrNum)
            {
                MeterIndex=pAmrThreadArg->pTaskRunInfo[i].MeterIndex;
                if(TaskInfo.MsInfo[MeterIndex].Pipe == pipe)
                {
                    if(JudgeNeedPatch(pAmrThreadArg->pTaskRunInfo[i].TaskId,TaskInfo.MsInfo[MeterIndex].Sn,&pAmrThreadArg->pTaskRunInfo[i].patchtime)==TRUE)//修改的地方
                    {

                        RunTaskInfo->CsdIndex=0;
                        RunTaskInfo->SubOadIndex=0;
                        TASK_FMT_TRACE ( pipe,REC_LOG, "找到要抄的表 MeterIndex %d\n",MeterIndex);
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
                    TASK_FMT_TRACE ( pipe,REC_LOG, "没找到要抄的表 MeterIndex %d TotalMtrNum %d \n",RunTaskInfo->MeterIndex,RunTaskInfo->TotalMtrNum);
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

struct timespec gMeterEventReportTime ;       //记录下收到电表事件上报的最后时间
//记录下收到电表事件上报的最后时间
void taskmng_meter_event_report_time_set()
{
    clock_gettime(CLOCK_MONOTONIC, &gMeterEventReportTime);
}
//检查下当前是否可以执行事件上报的处理任务 3秒后可以执行 最新改为5秒
BOOL taskmng_meter_event_report_time_check()
{
    struct timespec timenow;
    clock_gettime(CLOCK_MONOTONIC, &timenow);

    if(timenow.tv_sec - gMeterEventReportTime.tv_sec > 5)
        return TRUE;

    return FALSE;
}

/*******************************************************************************
* 函数名称: taskmng_amr_status_get_mqtt
* 函数功能://0013 获取当日抄读状态 mqtt线程收到后直接回复 不再给抄表线程处理 原来查询的太多的时候会把共享队列占满
            导致后面的透传被扔掉了
* 输入参数: pipe 端口号
        buf 消息内容
        len 消息长度
* 输出参数:
* 返 回 值:
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
    //TASK_FMT_DEBUG(pipe, RELAY_LOG,"当日抄读状态\n");
    //TASK_BUF_DEBUG( pipe, RELAY_LOG,MsgSend->playload,MsgSend->msglen);
    taskmng_plc_queue_write(APPArg.SendBuf, (uint8*)MsgSend, sizeof(MESSAGE_INFO_T)+index);
    free(MsgSend);

}

#if DESC("终端带电状态",1)
TMN_POWER_STATE_T gTmnPowerState;   //终端带电状态

/**********************************************************
* 函 数 名 : taskmng_power_state_init
* 函数功能 : 终端带电状态初始化
* 输入参数 : 无
* 输出参数 : 无
* 返 回 值 : 成功 : 0
*            失败 : -1
* 修改历史 :  2024/5/31
************************************************************/
int taskmng_power_state_init()
{
    time_t timenow = time(NULL);
    memset(&gTmnPowerState,0,sizeof(gTmnPowerState));
    gTmnPowerState.powerState = POWER_ON;
    /*给上电赋个时间  */
    DT_Time2DateTimeShort(timenow,&gTmnPowerState.powerOnTime);
    return 0;
}

/**********************************************************
* 函 数 名 : taskmng_power_event_deal
* 函数功能 : 收到终端停上电事件后读取数据中心
* 输入参数 : infonum 收到停上电事件的序号
* 输出参数 : 无
* 返 回 值 : 成功 : 0
*            失败 : -1
* 修改历史 :  2024/5/31
************************************************************/
int taskmng_power_event_deal(DB_CLIENT clientid,uint16 infonum)
{
    int ret = 0;
    READ_RECORD_T       readRecordIn;
    RECORD_UNIT_T       readRecordOut;
    OOP_POWERLOG_T data;
    time_t timenow = time(NULL);
    TASK_FMT_DEBUG(-1, UP_LOG, "读取终端停上电事件 \n");
    memset(&readRecordIn,0,sizeof(readRecordIn));
    memset(&readRecordOut,0,sizeof(readRecordOut));
    memset(&data,0,sizeof(OOP_POWERLOG_T));
    /* 读取终端停上电事件 */
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
        TASK_FMT_DEBUG(-1, UP_LOG, "读取终端停上电事件失败 ret %d outlen %d\n",ret,readRecordOut.record[0].outDatalen);
        return -1;
    }
    memcpy(&data, readRecordOut.record[0].outDataBuf, sizeof(OOP_POWERLOG_T));
    if (data.source.choice == DT_ENUM)
    {
        if (data.source.nValue == 0)  //停电
        {
            TASK_FMT_DEBUG(-1, UP_LOG, "终端停电 \n");
            gTmnPowerState.powerState = POWER_OFF;
            DT_Time2DateTimeShort(timenow,&gTmnPowerState.powerOffTime);
            taskmng_meter_power_report_num_clear(POWER_ON);
        }
        else if (data.source.nValue == 1)  //上电
        {
            TASK_FMT_DEBUG(-1, UP_LOG, "终端上电 \n");
            gTmnPowerState.powerState = POWER_ON;
            DT_Time2DateTimeShort(timenow,&gTmnPowerState.powerOnTime);
            /* 停电后又上电 把停电事件的上报个数清一下 以防后面停电不上报了 */
            taskmng_meter_power_report_num_clear(POWER_OFF);
        }
    }
    return 0;
}


/**********************************************************
* 函 数 名 : taskmng_power_state_get
* 函数功能 : 获取终端带电状态
* 输入参数 :
* 输出参数 : TMN_POWER_STATE_T *state    终端带电状态
* 返 回 值 : 无
* 修改历史 :  2024/5/31
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
