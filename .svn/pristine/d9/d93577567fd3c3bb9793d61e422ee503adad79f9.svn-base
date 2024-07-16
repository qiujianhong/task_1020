/*-
****************************************************************************************
* Copyright (c) 2011,南京新联电子股份有限责任公司
*            All rights reserved.
* 程序名称：newcb.c
* 版本号：1.0
* 功能：
-           打算将抄表模块进行优化，本文件存放新增的函数
目前优化的思路如下：
抄表主调度负责对抄表任务的生成以及各种优先级的任务的执行
各个驱动库负责具体抄表任务的执行
-           不过需要注意的是对各个抄表函数的处理：
-           目前对抄表函数进行了优化，主调度函数负责任务的调度以及任务的生成，各个抄表库负责抄表。
-           最大的修改是，一旦抄表驱动库开始一种抄表任务，函数就不再返回，直到所有数据项抄度完毕。
-           需要注意：由于抄表是阻塞式的抄表，所以在抄表的过程中需要一直维护信道状态和抄表任务
* 开发人：          顾君
* 开发时间：       2011年06月12日
* 修改者:
* 修改时间:
* 修改简要说明:
* 其他：
*****************************************************************************************
-*/


#include "CtsAll.h"
#include "task_msg.h"
#include "hal.h"
#include "uart.h"
#include "area.h"
#include "CtsPlc.h"

extern AmrThreadArg_T gAmrThreadArg[AMR_THREAD_NUM];
extern uint8 set_len_offset(uint16 eleLen, uint8 *indata);
extern uint8 patch_policy_flg[AMR_THREAD_NUM];

uint8 ComNo[10] = {PIPE0, PIPE1, PIPE2, PIPE3, PIPE4, PIPE5, PIPE6, PIPE7, PIPE8, PIPE9};

RS485_RS232_PARA Rs485ComNo[8];

//extern MODULE_INFO_T  gModuleInfo[5];      //5个模组的信息

extern uint8   gAmrThreadFlag[AMR_THREAD_NUM];

uint8 IsSendSn[8];

extern uint8 gdelay;

void pTaskRunInfoinit(uint8 pipe)
{
  int num;
  for(num=0;num<gAmrThreadArg[pipe].TaskNum;num++)
  {
        gAmrThreadArg[pipe].pTaskRunInfo[num].MeterIndex=0;
        gAmrThreadArg[pipe].pTaskRunInfo[num].CsdIndex=0;
        gAmrThreadArg[pipe].pTaskRunInfo[num].SubOadIndex=0;
  }
}





/**
*********************************************************************
* @brief:      实现  485抄表功能:包括抄表任务、方案的获取,以及抄读功能等；
* @param[in]：   pipe  线程号,485模块共8个线程,485的线程pipe从2开始到9
* @return：
*********************************************************************
*/
void cbtasknew_TaskCb(uint8 pipe)
{

    PIPESTATE pipestate = 0;                        //当前通道的状态
    int TaskFlag = 0;
    SCHEME_TASK_RUN_INFO CurTaskRunInfo;
    memset(&CurTaskRunInfo,0x00,sizeof(SCHEME_TASK_RUN_INFO));
    CTS_RUN_TASK_T   *RunTaskInfo=NULL;
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    uint8 ret;
    BOOLEAN NeedBc;
    CB_STATE status;
    uint32 sender;
    int RecvLen;
    uint8 RecvBuf[MAX_MSG_LEN];
    //先初始化一次随机数
    Init_ReadMtr(pipe);

    while(1)
    {
        gAmrThreadFlag[pipe] = 1;
//#ifdef PRODUCT_ECU
#if 0

        char devname[100]={0};
        if(pipe >= PLC_NUM+2 && pipe < PLC_NUM+6 && taskmng_uart_devname_get(pipe,devname)<0)
        {
            TASK_FMT_TRACE ( pipe,COMM_LOG, "未检测到485模块\n");
            taskmng_task_record_file_del_que_clear(pipe);
            taskmng_uart_release(pipe);
            sleep(3);
            continue;
        }

#endif
        //增加消息接收处理
        RecvLen = task_recv_msg(pAmrThreadArg->MsgSock, RecvBuf, MAX_MSG_LEN, &sender);

        if(RecvLen > 0)
        {

            TASK_MSG_t *    msg = (TASK_MSG_t *)RecvBuf;
            TASK_FMT_DEBUG(pipe, RELAY_LOG, "收到IPC消息 msg_id %04x RecvLen %d msg->msglen %d \n", msg->msg_id,
                 RecvLen, msg->msglen);
            TASK_BUF_DEBUG(pipe, RELAY_LOG, msg->msg, msg->msglen);

            switch(msg->msg_id)
            {
                case MSG_FILE_TRANSFER:
                    {
                        TASK_FMT_DEBUG(pipe, RELAY_LOG, "收到文件代发信息 \n");
                        taskmng_file_transfer_msg_amr(pipe, msg->msg, msg->msglen);
                    }
                    break;
                default:
                    break;
            }

        }

        if(pAmrThreadArg->IsUpdating == 1)
        {
            //485电表升级
            pAmrThreadArg->TotalState.FileTransfer = INNER_RTS_IDLE;
            taskmng_file_transfer_deal_485(pipe);
            pAmrThreadArg->IsUpdating = 0;
        }
        taskmng_data_init_check(pipe);
        taskmng_time_manage(pipe);
        //如果有广播校时要去执行广播校时
        if(pAmrThreadArg->BroadTiming.TimingType != 0)
        {
            TASK_FMT_TRACE ( pipe,RELAY_LOG, "在进行校时\n");
            if(pipe !=5)
            {
                Rs485BroadTiming(pipe);
                pAmrThreadArg->BroadTiming.TimingType = 0;
            }
            else
            {
                pAmrThreadArg->BroadTiming.TimingType = 0;
            }
        }
        taskmng_meter_time_485manager(pipe);
        taskmng_meter_time_485delay(pipe);
        //执行规约发来的代理和透传
        ExecMeter485Relay(pipe,&status,0xFF);
        pipestate = cbtaskOop_statecheck(pipe);
        switch(pipestate)
        {

            case PIPESTATE_POWEROFF:                //停电
            {
                sleep(4);
                continue;
            }
            case PIPESTATE_RUN_FLAG:
            {
                sleep(1);
                continue;
            }
            default://创建任务
            {
                break;
            }
        }

        NeedBc = FALSE;
        ret = GetHighPriorityTask(pipe,&RunTaskInfo,&CurTaskRunInfo);
        if(pAmrThreadArg->TaskModifyFlag != TASK_CFG_INITED)
        {
            continue;//任务没有初始化好，不进行抄表
        }
        if(ret==0)
        {
            NeedBc=GetNeedAgainTaskInfo(&CurTaskRunInfo,&RunTaskInfo,pipe);
            if(NeedBc==TRUE)
            {
                TASK_FMT_TRACE ( pipe,REC_LOG, "在进行补抄: RunTaskInfo->MeterIndex is %d\n", RunTaskInfo->MeterIndex);
            }
        }
        if((ret>0)||(NeedBc==TRUE))
        {
            if((RunTaskInfo->IsNewTurn ==1)||(NeedBc==TRUE))
            {
                taskmng_task_record_file_load(pipe,&CurTaskRunInfo,RunTaskInfo);
                TASK_FMT_TRACE ( pipe,REC_LOG, "任务 %d 开始执行\n", RunTaskInfo->TaskId);
                RunTaskInfo->IsNewTurn =0;
            }
            //如果是透明去执行透明方案，不走后面的正常抄表
            if(CurTaskRunInfo.SchemeType == TRANSPARENTPLAN)
            {
               Rs485TransDataReading(pipe,&CurTaskRunInfo,RunTaskInfo);
               continue;
            }
            if(CurTaskRunInfo.SchemeType ==ENCRYPTPLAN)
            {
                taskmng_data_reading_encrypt_485(pipe,&CurTaskRunInfo,RunTaskInfo);
                continue;
            }
            //当前有任务需要执行
            TaskFlag = 1;
            pAmrThreadArg->TmnAmrInfo.CurrentTaskId = RunTaskInfo->TaskId;

            if(CB_RET_END == cbTaskOop_MetAutkExec(pipe,&CurTaskRunInfo,RunTaskInfo))
            {
               RunTaskInfo->RunResult= SCHEME_FINISHED;
            }
        }
        else
        {
            TaskFlag=0;//当前无任务需要执行
        }

        if (TaskFlag == 0)
        {
            usleep(100000);
        }else
            usleep(2000);
        if(gdelay != 0)
        {
            usleep(gdelaytime);
        }
    }
}



/****************************************************************************
*函数名称：checkIsExist2022
*功能：检查下发的事件方案中是否有20220200事件记录号的OAD；

*输入参数：
*输出参数：
*返回：1 代表有事件记录号
       0 代表没有事件记录号
****************************************************************************/

int checkIsExist2022(SCHEME_TASK_RUN_INFO *pCbTask,OOP_OAD_U  EventMainOad,uint8 pipe)
{
 //   uint16          TaskId;                //任务编号
//    uint8           SchemeId;              //方案编号
//    char            CsdBuf[1024] = {0};
//    uint16          roadOffset = 0;
    uint8           roadNum = 0;
    uint8           i = 0;
    uint8           j = 0;
    uint32          len;
    int ret;
    OOP_OAD_U            MainOad;
    uint8           SoadNum = 0;
//  TASK_INFO               CsdTask;
    OOP_PLAN_EVENT_T EventPlanInfo;
    NOMAL_OAD_T NomalOad;

    NomalOad.oad.value = 0x60160200;
    NomalOad.logicId = 0;
    NomalOad.infoNum = pCbTask->SchemeId;
    ret = db_read_nomal(gAmrThreadArg[pipe].DBClientHandle, &NomalOad,sizeof(OOP_PLAN_EVENT_T), (uint8*)&EventPlanInfo, &len);

    if(ret != 0)
    {
        return 0;
    }
    roadNum=EventPlanInfo.events.nNum;
    if(roadNum == 0)
    {
        return 0;
    }
    for(i = 0; i < roadNum; i++)
    {

        MainOad.value=EventPlanInfo.events.cols[i].oadMain.value;
        SoadNum =EventPlanInfo.events.cols[i].oadCols.nNum;
        if(MainOad.nObjID == EventMainOad.nObjID && MainOad.attID == EventMainOad.attID)
        {
            for(j = 0; j < SoadNum; j++)
            {
                if(EventPlanInfo.events.cols[i].oadCols.oad[j].nObjID==0x2022)
                    return 1;

            }
        }
    }
    return 0;
}

BOOL ReadPowerState()
{
    return TRUE;
}

/****************************************************************************
名称： cbtaskOop_statecheck
功能：
-           抄表过程中，对通道各种状态的检查是否正常，为任务生成提供条件
-
处理流程：
-           略
输入参数：
-           pipe                当前通道号
-           pPipeCbTaskQueue    一个通道所有的任务集合
输出参数：
-           无
返回：
-           < 0 退出抄表流程
-           > 0 暂停抄表，等待重新启动
-           = 0 可以正常抄表
****************************************************************************/
PIPESTATE cbtaskOop_statecheck(uint8 pipe)//这个5s判一次
{

    //如果停电，不去抄表，处于等待状态
    if(ReadPowerState() == FALSE)
    {
       return PIPESTATE_POWEROFF;       //按照张工的说法目前只判断停电
    }
    //通道没有电表档案，认为是空转状态
    if(FALSE==taskmng_meter_check_from_pipe(pipe))
    {
        return PIPESTATE_RUN_FLAG;
    }
      return PIPESTATE_NORMAL;
}


/****************************************************************************
名称： cbtaskOop_drvcheck
功能：
-           检查当前信道状态
处理流程：
-           检查信道以下几个状态：
-               1、检查当前信道是否有中继命令，并且已经需要发送报文
-               2、查看当前抄表的测量点参数是否发生变化，如果发生变化，则终止抄表 cbtasknew_ReadScalePara
-               3、检查当前信道状态，查看当前信道是否是否需要暂停或者停止抄表 cbtasknew_statecheck
                4. 抄表控制判断
-           注意：1是实时检测，2、3是每5秒检测一次
输入参数：
-           taskid              当前任务号
-           lstCbMtrPara        当前抄表测量点的参数
-           pPipeCbTaskQueue    当前通道的所有抄表任务（一个是）
-           pLstTime            前一次检测的时间
输出参数：
-           无
返回：
-           CBDRIVER_STATE_PAUSE        暂停抄表
-           CBDRIVER_STATE_CONTINUE     继续抄表
-           CBDRIVER_STATE_FINISH       结束抄表，准备退出
****************************************************************************/
CB_STATE cbtaskOop_drvcheck(SCHEME_TASK_RUN_INFO *pCbTask,cbMtrPara *lstCbMtrPara, time_t *pLstTime,uint8 pipe)
{
    time_t      curTime = time(NULL);               //当前时钟

//  printf("cbtaskOop_drvcheck is BEGIN\n");
    //5秒钟检查一次信道状态
    if((curTime < *pLstTime) || ((curTime - *pLstTime) >= 5))
    {
        PIPESTATE   pipestate = PIPESTATE_NORMAL;       //当前通道的状态
//        cbMtrPara curCbMtrPara;                       //每次抄表的时候，读取测量点参数，用来进行比对测量点参数是否发生了变化
        *pLstTime = curTime;
        //检查当前信道状态，在信道正常的情况下生成抄表任务

        pipestate = cbtaskOop_statecheck(pipe);     //当前通道的状态

        //uint8 retAutkCreate=0;
        switch(pipestate)
        {



        case PIPESTATE_POWEROFF:                //停电
            return CB_STATE_FINISH;
        default:
        break;

        }
    }

    return CB_STATE_CONTINUE;
}
/****************************************************************************
名称： Deal_Cb_Result
功能：
-           检查当前通道的任务链表
处理流程：
-           本函数主要是检查当前运行的通道任务链表是否有效来判断后续抄表是否执行
输入参数：
-           TASK_RUN_INFO *pCbTask  一个通道所有的任务集合
-
输出参数：
-           无
返回：
-
****************************************************************************/
void Deal_Cb_Result(uint8 pipe,SCHEME_TASK_RUN_INFO *pCbTask,CB_RET    nCBRet,CTS_RUN_TASK_T  *RunTaskInfo)
{

    rtc_t cbCurBTime;
    uint8 Com_StatTime[6]= {0};

    uint8 CsdIndex;
    uint16 MeterIndex,pn,MsIndex;

    CsdIndex=RunTaskInfo->CsdIndex;
    MeterIndex=RunTaskInfo->MeterIndex;
    pn=pCbTask->MsInfo[MeterIndex].Sn;
    MsIndex=taskmng_ms_index_get(pipe,pn,RunTaskInfo->logicid);

    InterTaskReadRTC(&cbCurBTime);
    memcpy(Com_StatTime,&cbCurBTime,6);
    if (cbCurBTime.ww == 0x00)
        cbCurBTime.ww = 0x07;
    Com_StatTime[4] |= (cbCurBTime.ww<<5);

    TASK_FMT_TRACE( pipe, REC_LOG, "Deal_Cb_Result nCBRet is %d\n",nCBRet);
    if (CB_RET_RESTART != nCBRet)
    {
        if(CB_RET_OK == nCBRet)
        {


            if(CsdIndex>=1)
            {
                CsdIndex=CsdIndex-1;
            }
            uint16 MainOad = pCbTask->CsdTask.RoadTask[CsdIndex].MainOad.nObjID;

            TASK_FMT_TRACE( pipe, REC_LOG, "MainOad is %x\n",MainOad);

            if(MainOad == 0x5004 || MainOad == 0x5006 || MainOad == 0x5002 || MainOad == 0x5005)//日月曲线等冻结判断时标是否正确
            {
                if(taskmng_mtr_result_check(MeterIndex,RunTaskInfo->MtrResult)==TRUE)//日冻结抄读成功后累计成功个数
                {
                    if(patch_policy_flg[pipe] > 0 && pCbTask->MsInfo[MeterIndex].Prtl == PRTL_07)
                    {
                        taskmng_rs485_cache_buf_write_to_db(pipe,pn,0,&gAmrThreadArg[pipe].meterData[0]);
                    }
                    TASK_FMT_DEBUG( pipe, REC_LOG, "pn %d 抄完\n",pn);
                    taskmng_task_monitor_update(pipe,pCbTask->TaskId,pCbTask->logicid,UDTASK_SUCCESSNUM,NULL);//pCbTask->TaskMonitor.successNum ++;

                    RunTaskInfo->SucNum++;
                    taskmng_task_record_file_set(pipe,MsIndex,RunTaskInfo->TaskId,RunTaskInfo->logicid);
                }
            }
            else
            {

                taskmng_task_monitor_update(pipe,pCbTask->TaskId,pCbTask->logicid,UDTASK_SUCCESSNUM,NULL);//pCbTask->TaskMonitor.successNum ++;
                if(MainOad == 0 && pCbTask->MsInfo[MeterIndex].Prtl == PRTL_07)
                {
                    taskmng_rs485_cache_buf_write_to_db(pipe,pn,0,&gAmrThreadArg[pipe].meterData[0]);
                    taskmng_mtr_result_set(MeterIndex,RunTaskInfo->MtrResult);
                }

                if(taskmng_mtr_result_check(MeterIndex,RunTaskInfo->MtrResult)&&(MainOad ==0x0000))//抄读成功后累计成功个数
                {
                    TASK_FMT_DEBUG( pipe, REC_LOG, "pn %d 抄完,MsIndex is %d\n",pn,MsIndex);
                    RunTaskInfo->SucNum++;
                    taskmng_task_record_file_set(pipe,MsIndex,RunTaskInfo->TaskId,RunTaskInfo->logicid);
                }

                if(MainOad !=0x0000)
                {
                    RunTaskInfo->SucNum++;
                    //set_bit_value(RunTaskInfo->MtrResult,128,MsIndex);//事件抄读成功即不再进行补抄
                    taskmng_mtr_result_set(MeterIndex,RunTaskInfo->MtrResult);
                    taskmng_task_record_file_set(pipe,MsIndex,RunTaskInfo->TaskId,RunTaskInfo->logicid);
                }
            }
        }
        RunTaskInfo->MeterIndex++;
        RunTaskInfo->CbNum++;
        TASK_FMT_TRACE( pipe, REC_LOG, "结果RunTaskInfo->MeterIndex is %d，RunTaskInfo->CbNum is %d\n",RunTaskInfo->MeterIndex,RunTaskInfo->CbNum);
    }
    return;
}




/****************************************************************************
名称： Cheeck_Cb_Pipe
功能：
-           检查当前抄表通道的通信状态
处理流程：
-
输入参数：
-           S_COMM_INFO_MET *Comm_Info_Oop,uint8* pipe,cbMtrPara    *MtrPara,int32 *fd232
-
输出参数：
-           无
返回：
-           抄表状态
****************************************************************************/

CB_STATE Cheeck_Cb_Pipe(SCHEME_TASK_RUN_INFO *pCbTask,S_COMM_INFO_MET* Comm_Info_Oop,cbMtrPara * MtrPara,uint8 pipe)
{

    //uint8 tmpFlag = 0xff;             //对当前TaskID处理结果的中间变量
    //Comm_Info_Oop->bCurMapIDIndex = 0;  //当前处理的pMap的序号

    //5秒钟检查一次信道状态

    //Comm_Info_Oop->MetStat = cbtaskOop_drvcheck(pCbTask, MtrPara,lstTime, pipe);
    if(Comm_Info_Oop->MetStat == CB_STATE_PAUSE)
    {

        Comm_Info_Oop->MetStat = CB_STATE_SEND;     //需要暂停任务，那么到下一次重新执行发送状态
        return Comm_Info_Oop->MetStat;
    }
    else if(Comm_Info_Oop->MetStat == CB_STATE_FINISH || Comm_Info_Oop->MetStat == CB_STATE_RESTART)
    {

        return Comm_Info_Oop->MetStat;                                      //任务停止或者重启，直接退出发送任务，进入到其他状态中
    }

    //在cbtasknew_drvcheck函数中，如果检测到有中继命令需要发送
    //可能会将485口关闭，所以这里需要重新打开
//    if(*fd232 <= 0)
//    {
//        Comm_Info_Oop->MetStat = CB_STATE_INIT;
//        return Comm_Info_Oop->MetStat;
//    }
    return Comm_Info_Oop->MetStat;
}

/****************************************************************************
名称： Cb_State_Init
功能：
-
处理流程：
-           对不同协议电表抄表时485通道进行通道初始化
输入参数：
-
-
输出参数：
-           无
返回：
-           无
****************************************************************************/
void Cb_State_Init(uint8 prtltype,uint8 pipe,S_COMM_INFO_MET* Comm_Info_Oop,cbMtrPara* MtrPara)
{
    Comm_Info_Oop->MetStat = CB_STATE_SEND;

    if(prtltype == PRTL_Dlms)
        Comm_Info_Oop->MetStat = CB_STATE_HANDSEND;
//    switch(prtltype)
//    {
//    case PRTL_OOP:
//        Cb_State_Init_Oop(Comm_Info_Oop,pipe,MtrPara);
//    break;
//    #if OS_METDLT645_EN == 1
//    case PRTL_97:
//        Cb_State_Init_645_97(Comm_Info_Oop,pipe,MtrPara);
//    break;
//    #endif
//    #if OS_METSTD2007_EN == 1
//    case PRTL_07:
//        Cb_State_Init_645_07(Comm_Info_Oop,pipe,MtrPara);
//    break;
//    #endif
//    case PRTL_Protector:
//        Cb_State_Init_Protector(Comm_Info_Oop,pipe,MtrPara);
//    break;
//    case PRTL_Cap:
//        Cb_State_Init_Cap(Comm_Info_Oop,pipe,MtrPara);
//    break;
//    case PRTL_Dlms:
//        Cb_State_Init_Dlms(Comm_Info_Oop,pipe,MtrPara);
//    break;
//    case PRTL_188:
//        Cb_State_Init_Std188(Comm_Info_Oop,pipe,MtrPara);
//    break;
//    default:
//    break;

 //   }
}

/****************************************************************************
名称： Cb_State_Hand_Send
功能：
-
处理流程：
-           对不同协议电表抄表时485通道进行通道初始化
输入参数：
-
-
输出参数：
-           无
返回：
-           无
****************************************************************************/
void Cb_State_Hand_Send(uint8 prtltype,SCHEME_TASK_RUN_INFO *pCbTask,CTS_RUN_TASK_T *RunTaskInfo, uint8 pipe, S_COMM_INFO_MET *Comm_Info_dlms,cbMtrPara *MtrPara)
{
    switch(prtltype)
    {
        case PRTL_Dlms:
            Meter_Dlms_Hnadsend(pCbTask,RunTaskInfo, pipe, Comm_Info_dlms,MtrPara);
        break;
        default:
        break;

    }
}

/****************************************************************************
名称： Cb_State_Hand_Send
功能：
-
处理流程：
-           对不同协议电表抄表时485通道进行通道初始化
输入参数：
-
-
输出参数：
-           无
返回：
-           无
****************************************************************************/
void Cb_State_Hand_Recv(uint8 prtltype,SCHEME_TASK_RUN_INFO *pCbTask,CTS_RUN_TASK_T *RunTaskInfo, uint8 pipe, S_COMM_INFO_MET *Comm_Info_dlms,cbMtrPara *MtrPara)
{
    switch(prtltype)
    {
        case PRTL_Dlms:
            Meter_Dlms_Hnadrecv(pCbTask,RunTaskInfo, pipe, Comm_Info_dlms,MtrPara);
        break;
        default:
        break;

    }
}

/****************************************************************************
名称： Cb_State_Send
功能：
-
处理流程：
-           485抄表发送抄表报文
输入参数：
-
-
输出参数：
-           无
返回：
-           无
****************************************************************************/
void Cb_State_Send(uint8 prtltype,SCHEME_TASK_RUN_INFO * pCbTask,CTS_RUN_TASK_T *RunTaskInfo,uint8 pipe,S_COMM_INFO_MET* Comm_Info_Oop,cbMtrPara* MtrPara,uint8 *DIIndex,uint8 *DINum,uint16 *DI97Buff,uint32 *DI07Buff)
{

    switch(prtltype)
    {
        case PRTL_OOP:
            Meter_Oop_Read_Item(pCbTask,RunTaskInfo, pipe, Comm_Info_Oop,MtrPara);
        break;
        #if OS_METDLT645_EN == 1
        case PRTL_97:
            Meter_645_97_Read_Item(pCbTask,RunTaskInfo,pipe,Comm_Info_Oop,MtrPara,DIIndex, DINum,DI97Buff);
        break;
        #endif
        case PRTL_07:
        {
            Meter_645_07_Read_Item(pCbTask,RunTaskInfo,pipe,Comm_Info_Oop,MtrPara,DIIndex, DINum,DI07Buff);
        }
        break;
        case PRTL_Protector:
        {
            Meter_Protector_Read_Item(pCbTask,RunTaskInfo,pipe,Comm_Info_Oop,MtrPara,DIIndex, DINum,DI07Buff);
        }
        break;
        case PRTL_Cap:
            Meter_Cap_Read_Item(pCbTask,RunTaskInfo,pipe,Comm_Info_Oop,MtrPara,DIIndex, DINum,DI07Buff);
        break;
        case PRTL_Dlms:
            Meter_Dlms_Send(pCbTask,RunTaskInfo,pipe, Comm_Info_Oop,MtrPara);
        break;
        case PRTL_188:
        {
            Meter_Std188_Read_Item_New(pCbTask,RunTaskInfo,pipe,Comm_Info_Oop,MtrPara,DIIndex, DINum,DI07Buff);
        }
        break;
        default:
        break;

    }
    return;
}
/****************************************************************************

名称： Cb_State_Recv
功能：
-
处理流程：
-           处理接收电表返回报文
输入参数：
-
-
输出参数：
-           无
返回：
-           无
****************************************************************************/
void Cb_State_Recv(uint8 prtltype,SCHEME_TASK_RUN_INFO *pCbTask,CTS_RUN_TASK_T *RunTaskInfo,uint8 pipe,S_COMM_INFO_MET *Comm_Info_Oop,cbMtrPara *MtrPara,uint8 *DIIndex,uint8 DINum,uint8 *CombinBuff,uint16 *CombinLen,uint16 *DI97Buff,uint32 *DI07Buff)

{
    switch(prtltype)
    {
    case PRTL_OOP:
        Meter_Oop_recvive_data(pCbTask,RunTaskInfo, pipe, Comm_Info_Oop,MtrPara);
    break;
    #if OS_METDLT645_EN == 1
    case PRTL_97:
        Meter_645_97_Recvive_Data(pCbTask,RunTaskInfo, pipe,Comm_Info_Oop,MtrPara,DIIndex,DINum,CombinBuff,CombinLen,DI97Buff);
    break;
    #endif
    #if OS_METSTD2007_EN == 1
    case PRTL_07:
        Meter_645_07_Recvive_Data(pCbTask,RunTaskInfo,pipe,Comm_Info_Oop,MtrPara,DIIndex,DINum,CombinBuff,CombinLen,DI07Buff);
    break;
    case PRTL_Protector:
        Protector_Recvive_Data(pCbTask,RunTaskInfo,pipe,Comm_Info_Oop,MtrPara,DIIndex,DINum,CombinBuff,CombinLen,DI07Buff);
    break;
    case PRTL_Cap:
        Meter_Cap_Recvive_Data(pCbTask,RunTaskInfo,pipe,Comm_Info_Oop,MtrPara,DIIndex,DINum,CombinBuff,CombinLen,DI07Buff);
    break;
    case PRTL_Dlms:
        Meter_Dlms_Recv(pCbTask,RunTaskInfo, pipe, Comm_Info_Oop,MtrPara);
    break;
    case PRTL_188:
        Meter_188_Recvive_Data_New(pCbTask,RunTaskInfo,pipe,Comm_Info_Oop,MtrPara,DIIndex,DINum,CombinBuff,CombinLen,DI07Buff);
    break;
    #endif
    default:
    break;

    }
    return;
}

/****************************************************************************
名称： Met485_AutkExec
功能：
-           电表驱动实现自动抄表
处理流程：
-           本函数主要根据规约类型来调用不用的抄表驱动库进行抄表，方法和原有函数类似
输入参数：
-           pPipeCbTaskQueue    一个通道所有的任务集合
-           taskid              执行抄表任务的任务号
输出参数：
-           无
返回：
-           无
****************************************************************************/
CB_RET Met485_AutkExec(uint8 pipe,SCHEME_TASK_RUN_INFO *pCbTask,CTS_RUN_TASK_T  *RunTaskInfo)
{


    uint8 prtltype;
//    time_t              lstTime = time(NULL);
//    int32               fd232 = 0;
    S_COMM_INFO_MET Comm_Info_Oop;
    cbMtrPara           MtrPara;
    uint8 DI07Index;
    uint8 DI07Num;
    uint8 CombinBuff[255];
    uint16 CombinLen;
    uint16 MeterIndex,MsIndex;
    uint16 pn;
    uint16 DI97Buff[10];
    uint32 DI07Buff[10];


    MeterIndex=RunTaskInfo->MeterIndex;

    prtltype = pCbTask->MsInfo[MeterIndex].Prtl;
    #ifdef AREA_SHANDONG
    TaskmngAdjustReadOad(pCbTask);
    #endif
    switch(prtltype)
    {
    case PRTL_OOP:
        Prepare_OOP_meter_read(pCbTask,pipe,RunTaskInfo,&Comm_Info_Oop,&MtrPara);
    break;
    #if OS_METDLT645_EN == 1
    case PRTL_97:
        Prepare_645_97_meter_read(pCbTask,pipe,RunTaskInfo,&Comm_Info_Oop,&MtrPara);
    break;
    #endif
    #if OS_METSTD2007_EN == 1
    case PRTL_07:
        Prepare_645_07_meter_read(pCbTask,pipe,RunTaskInfo,&Comm_Info_Oop,&MtrPara);
    break;
    #endif
    case PRTL_Protector:
        Prepare_Protector_meter_read(pCbTask,pipe,RunTaskInfo,&Comm_Info_Oop,&MtrPara);
    break;
    case PRTL_Cap:
        Prepare_Cap_read(pCbTask,pipe,RunTaskInfo,&Comm_Info_Oop,&MtrPara);
    break;
    case PRTL_Dlms:
        Prepare_dlms_meter_read(pCbTask,pipe,RunTaskInfo,&Comm_Info_Oop,&MtrPara);
    break;
    case PRTL_188:
        Prepare_std188_meter_read(pCbTask,pipe,RunTaskInfo,&Comm_Info_Oop,&MtrPara);
    break;

    default:
        {
            pn=pCbTask->MsInfo[MeterIndex].Sn;
            MsIndex=taskmng_ms_index_get(pipe,pn,RunTaskInfo->logicid);
            taskmng_mtr_result_set(MeterIndex,RunTaskInfo->MtrResult);
            taskmng_task_record_file_set(pipe,MsIndex,RunTaskInfo->TaskId,RunTaskInfo->logicid);
            return CB_RET_OK;
        }
    break;
    }
    while(1)
    {
      switch(Comm_Info_Oop.MetStat)
      {
        case CB_STATE_INIT:
             Cb_State_Init(prtltype,pipe,&Comm_Info_Oop,&MtrPara);
        break;
        case CB_STATE_HANDSEND:
             Meter_Dlms_Hnadsend(pCbTask,RunTaskInfo,pipe,&Comm_Info_Oop,&MtrPara);
        break;
        case CB_STATE_HANDRECV:
            Meter_Dlms_Hnadrecv(pCbTask,RunTaskInfo,pipe, &Comm_Info_Oop,&MtrPara);
         break;
        case CB_STATE_FRZSEND:
            Meter_Dlms_Frzsend(pCbTask,RunTaskInfo,pipe,&Comm_Info_Oop,&MtrPara);
            break;

        case CB_STATE_FRZRECV:
            Meter_Dlms_Frzrcv(pCbTask,RunTaskInfo,pipe, &Comm_Info_Oop,&MtrPara);
            break;
        case CB_STATE_SEND:
             Cb_State_Send(prtltype,pCbTask,RunTaskInfo,pipe,&Comm_Info_Oop,&MtrPara,&DI07Index,&DI07Num,DI97Buff,DI07Buff);
        break;

        case CB_STATE_RECV:                  //接收
             Cb_State_Recv(prtltype,pCbTask,RunTaskInfo,pipe,&Comm_Info_Oop,&MtrPara,&DI07Index,DI07Num,CombinBuff,&CombinLen,DI97Buff,DI07Buff);
        break;

        case CB_STATE_FINISH:         //抄表完成或失败，准备退出
        {

            if(Comm_Info_Oop.ComFrame==0)
            {
                Comm_Info_Oop.AutkStat = CB_RET_NULL;
            }
            else
            {
                if(Comm_Info_Oop.ErrFrame >= ERR_FRAME_MAX&&Comm_Info_Oop.ErrFrame >= Comm_Info_Oop.ComFrame)
                {
                    Comm_Info_Oop.AutkStat = CB_RET_FAIL;
                }
                else
                {
                    Comm_Info_Oop.AutkStat = CB_RET_OK;
                }
            }
            return Comm_Info_Oop.AutkStat;
        }
        break;
        case CB_STATE_RESTART:
        {
            Comm_Info_Oop.AutkStat = CB_RET_RESTART;

            return Comm_Info_Oop.AutkStat;;
        }
        break;

        default:
            break;

      }
        usleep(1000);     //释放一下CPU

    }
}
/****************************************************************************
名称： cbTaskOop_MetAutkExec
功能：
-           电表驱动实现自动抄表
处理流程：
-           本函数主要根据规约类型来调用不用的抄表驱动库进行抄表，方法和原有函数类似
输入参数：
-           pPipeCbTaskQueue    一个通道所有的任务集合
-           taskid              执行抄表任务的任务号
输出参数：
-           无
返回：
-           无
****************************************************************************/

CB_RET cbTaskOop_MetAutkExec(uint8 pipe,SCHEME_TASK_RUN_INFO *pCbTask,CTS_RUN_TASK_T  *RunTaskInfo)
{
    CB_RET nCBRet = CB_RET_FAIL;

    uint16 MeterIndex;
    //入口参数合法性检查处理
    if(pCbTask == NULL)
        return CB_RET_FAIL;

    MeterIndex=RunTaskInfo->MeterIndex;

    if(RunTaskInfo->NeedMtrNum==0 ||pCbTask->CsdTask.RoadNum==0)
    {
        return CB_RET_END;
    }

    while((RunTaskInfo->CbNum)<RunTaskInfo->NeedMtrNum)
    {
        MeterIndex=RunTaskInfo->MeterIndex;
        if(pCbTask->MsInfo[MeterIndex].Pipe == pipe)
        {
            if(taskmng_mtr_result_check(MeterIndex,RunTaskInfo->MtrResult)==FALSE)//修改的地方
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
        RunTaskInfo->MeterIndex++;
        if(RunTaskInfo->MeterIndex>=RunTaskInfo->TotalMtrNum)
        {
            TASK_FMT_TRACE ( pipe,REC_LOG, "没找到要抄的表 MeterIndex %d TotalMtrNum %d \n",RunTaskInfo->MeterIndex,RunTaskInfo->TotalMtrNum);
            break;
        }

    }

    if((RunTaskInfo->CbNum >= RunTaskInfo->NeedMtrNum)||(RunTaskInfo->MeterIndex>=RunTaskInfo->TotalMtrNum))
    {

          if(RunTaskInfo->SucNum>=RunTaskInfo->NeedMtrNum)
          {

            RunTaskInfo->NeedAgain=FALSE;
          }
          else
          {
            TASK_FMT_TRACE ( pipe,REC_LOG, "该任务需要补抄 TaskId is %d,RunTaskInfo->SucNum is %d,RunTaskInfo->NeedMtrNum is %d\n",pCbTask->TaskId,RunTaskInfo->SucNum,RunTaskInfo->NeedMtrNum);
            RunTaskInfo->NeedAgain=TRUE;
          }
          if((RunTaskInfo->NeedAgain==TRUE)&&(RunTaskInfo->IsAlreadyBc==TASKBC_DOING))
          {
            RunTaskInfo->IsAlreadyBc=TASKBC_FINISHED;
          }
          return CB_RET_END;
    }

    nCBRet=Met485_AutkExec(pipe,pCbTask,RunTaskInfo);
    if(nCBRet !=CB_RET_OK)
    {

    }
    Deal_Cb_Result(pipe,pCbTask,nCBRet,RunTaskInfo);

    return nCBRet;

}
/**
*********************************************************************
* @brief:      实现抄读主站发来的485的代理消息；
* @param[in]：  uint8 pipe  线程号
* @param[in]： CB_STATE *Cbstatus  485执行的是状态(是初始化状态还是、发送状态、接收状态等)
* @return：
*********************************************************************
*/

void ExecMeter485Relay(uint8 pipe,CB_STATE *Cbstatus,uint8 priority)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    int ret;
    uint16 conunt=0;
    uint8 flag=0;
    //穿透任务的产生
    while(1)
    {
        if(pAmrThreadArg->RelayTrans.isValid ==0)
        {
             while(1)
             {
                 ret = taskmng_relay_exist_check(pipe);
                 if(ret != -1)
                 {
                     break;
                 }
             }
            if(ret == 1)
            {
                *Cbstatus=CB_STATE_INIT;
                TASK_FMT_TRACE( pipe, RELAY_LOG, "产生穿透任务 ！\n");
                Meter485RelayTransDeal(pipe,priority);
                pAmrThreadArg->RelayTrans.isValid =0;
                conunt = 0;
                flag=1;
            }
            else
            {
                if((conunt<200)&&(flag==1))
                {
                    conunt++;
                    usleep(10000);
                    continue;
                }
                return;
            }

        }
        else
        {
            continue;
        }
    }
}

/****************************************************************************
*函数名称：Init_Relay_Cb_Pipe
*功能：执行代理和透传前初始化抄表通道;
*输入参数：
*输出参数：
*返回：
****************************************************************************/

void Init_Relay_Cb_Pipe(uint8 pipe,uint8* speed)
{
    OOP_TSA_T tsa;
    COMM_PARA Comm_Para_Dev;                                             /*串口参数*/
    //uint16 Speed=0;
    //uint8 i=0;
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    uint8 MsgIndex,Baud;
    memset((void *)&Comm_Para_Dev, 0, sizeof(COMM_PARA));

    switch(pAmrThreadArg->RelayTrans.IOP)
    {
        case 0x0010:
        {
            Baud=pAmrThreadArg->RelayTrans.comdcb.baud;
            //Speed=get_meter_read_speed(Baud);
        }
        break;
        case 0x0011:
        {
            switch(pAmrThreadArg->RelayTrans.proxychoice)
            {
                case 0x01:
                {
                    MsgIndex=pAmrThreadArg->RelayTrans.AmrIndex;
                    Baud=taskmng_baud_get_from_tsa(pAmrThreadArg->RelayTrans.AmrNormal[MsgIndex].Mtraddr,LOGIC_ID_YC);
                    //Speed=get_meter_read_speed(Baud);
                }
                break;
                case 0x02:
                {
                    memcpy_r(tsa.add,pAmrThreadArg->RelayTrans.MtrAddr,6);
                    Baud=taskmng_baud_get_from_tsa(tsa,LOGIC_ID_YC);
                    //Speed=get_meter_read_speed(Baud);
                }
                break;
                case 0x03:
				case 0x04:
				case 0x05:
				case 0x06:
                {
                    MsgIndex=pAmrThreadArg->RelayTrans.AmrIndex;
                    Baud=taskmng_baud_get_from_tsa(pAmrThreadArg->RelayTrans.METER_LIST[MsgIndex].Mtraddr,LOGIC_ID_YC);
                    //Speed=get_meter_read_speed(Baud);
                    //TASK_FMT_TRACE( pipe, RELAY_LOG,"Speed is %d\n",Speed);
                }
                break;
                case 0x07:
                {
                    Baud=pAmrThreadArg->RelayTrans.comdcb.baud;
                    //Speed=get_meter_read_speed(Baud);
                   // TASK_FMT_TRACE( pipe, RELAY_LOG,"Speed is %d\n",Speed);
                }
                break;
                default:
                break;
            }
        }
        break;
        case 0x0016:
        {
            //Speed=get_meter_read_speed(pAmrThreadArg->RelayTrans.comdcb.baud);
            Baud=pAmrThreadArg->RelayTrans.comdcb.baud;
        }
        break;

    }
    *speed = Baud;
//    for(i=0;i<2;i++)
//    {
//        if(rs485_RS232_Init(&Rs485ComNo[pipe-2],Speed)!=0)
//        {
//            rs485_RS232_UnInit(&Rs485ComNo[pipe-2]);//先临时添加
//        }
//        else
//        {
//            break;
//        }
//    }
//    if(i>=2)
//    {
//        pAmrThreadArg->TotalState.RelayTrans = INNER_RTS_END;//两次打开都失败,结束此次透传；
//    }
//    else
//    {
        pAmrThreadArg->TotalState.RelayTrans = INNER_RTS_PIPE_INIT;
    //}

}

/*******************************************************************************
* 函数名称: RelayCmdMeter485PrtlData
* 函数功能: 执行透传和代理的抄表发送
* 输入参数:
* 输出参数:
* 返 回 值:
*
*******************************************************************************/
int RelayCmdMeter485PrtlData ( uint8 pipe,uint8 speed,uint8 priority)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
  //  uint8 Payload[MAX_MSG_LEN] = {0};
  //  uint8 index=0;
    uint8 prtl = 0;
  //  uint8 log[MAX_MSG_LEN]={0};
  //  uint16 OverTime = 40;//超时时间统一40秒
    int ret;

    switch(pAmrThreadArg->RelayTrans.IOP)
    {
    case 0x0010:  //请求代理读取若干个服务器的若干个对象属性
    {

        uint16 DataLen=0;

        //通过从节点监控下发
        DataLen = pAmrThreadArg->RelayTrans.RelayLen;

        TASK_FMT_TRACE( pipe, RELAY_LOG, "透传组帧 DataLen is %d\n",DataLen);

        TASK_BUF_TRACE( pipe, RELAY_LOG,pAmrThreadArg->RelayTrans.RelayData,DataLen);
        if(DataLen>0)
        {

            ret=rs485_RS232_send(pipe,MAX_FRAME_OVERTIME,MAX_CHAR_OVERTIME,speed,PARITY_EVEN,pAmrThreadArg->RelayTrans.RelayData, pAmrThreadArg->RelayTrans.RelayLen);
            if(ret<0)
            {
                return -1;
            }
        }

    }
    break;

    case 0x0011:
    {
     switch(pAmrThreadArg->RelayTrans.proxychoice)
     {
        case 0x01:  //请求代理读取若干个服务器的若干个对象属性
        {
            DI_INFO_T DiInfo;
            uint8 DataBuff[1024]={0};
            uint16 DataLen;

            if(GetNext485RelayDI( pipe,&DiInfo,&prtl)>0)
            {
                //通过从节点监控下发
                DataLen = MakeRelay485Data_0010(pipe,DiInfo,prtl,DataBuff);

                TASK_FMT_TRACE( pipe, RELAY_LOG, "0011_01透传组帧 DataLen is %d\n",DataLen);

                TASK_BUF_TRACE( pipe, RELAY_LOG, DataBuff,DataLen);

                if(DataLen>0)
                {

                    ret=rs485_RS232_send(pipe,MAX_FRAME_OVERTIME,MAX_CHAR_OVERTIME,speed,PARITY_EVEN,DataBuff, DataLen);
                    if(ret<0)
                    {
                        return -1;
                    }
                }
            }
            else
            {
                return -1;
            }
        }
        break;
        case 0x02:
        {

            TASK_FMT_TRACE( pipe, RELAY_LOG, "pAmrThreadArg->RelayTrans.RelayLen is %d\n",pAmrThreadArg->RelayTrans.RelayLen);
            if(pAmrThreadArg->RelayTrans.RelayLen>0)
            {

               ret=rs485_RS232_send(pipe,MAX_FRAME_OVERTIME,MAX_CHAR_OVERTIME,speed,PARITY_EVEN,pAmrThreadArg->RelayTrans.RelayData, pAmrThreadArg->RelayTrans.RelayLen);
               TASK_BUF_TRACE(pipe, RELAY_LOG,pAmrThreadArg->RelayTrans.RelayData,pAmrThreadArg->RelayTrans.RelayLen);
               if(ret<0)
               {
                       return -1;
               }
            }
        }
        break;
        case 0x03:
        case 0x04:
        case 0x05:
        case 0x06:
        {

            TASK_FMT_TRACE( pipe, RELAY_LOG, "pAmrThreadArg->RelayTrans.RelayLen is %d,pAmrThreadArg->RelayTrans.proxychoice is %d\n",pAmrThreadArg->RelayTrans.RelayLen,pAmrThreadArg->RelayTrans.proxychoice);
            ret=GetNext485SetRelayData(pipe);
            if(ret<0)
           {
                   return -1;
           }
        }
        break;
        case 0x07:
        {

            uint16 DataLen=0;
            OOP_OAD_U oad;
            uint8 tmppipe;
            //通过从节点监控下发
            oad.value = pAmrThreadArg->RelayTrans.OadPort;
            DataLen = pAmrThreadArg->RelayTrans.RelayLen;
            tmppipe = taskmng_mtr_pipe_get(&oad);
            if(tmppipe !=pipe)
            {
                return -1;
            }
            TASK_FMT_TRACE( pipe, RELAY_LOG, "透传组帧 DataLen is %d\n",DataLen);

            TASK_BUF_TRACE( pipe, RELAY_LOG,pAmrThreadArg->RelayTrans.RelayData,DataLen);
            if(DataLen>0)
            {

                ret=rs485_RS232_send(pipe,MAX_FRAME_OVERTIME,MAX_CHAR_OVERTIME,speed,PARITY_EVEN,pAmrThreadArg->RelayTrans.RelayData, pAmrThreadArg->RelayTrans.RelayLen);
                if(ret<0)
                {
                    return -1;
                }
            }
        }
        break;
        case 0x50:
        {

            uint16 DataLen=0;
            OOP_OAD_U oad;
            uint8 tmppipe;
            FRAME_CONVERT_T frame;
            //通过从节点监控下发
            oad.value = pAmrThreadArg->RelayTrans.OadPort;
            DataLen = pAmrThreadArg->RelayTrans.RelayLen;
            tmppipe = taskmng_mtr_pipe_get(&oad);
            if(tmppipe !=pipe)
            {
                return -1;
            }
            if(priority >= pAmrThreadArg->RelayTrans.priority)
            {
                return -2;//用于福建的优先级判断
            }
            memcpy(frame.srcBuf,pAmrThreadArg->RelayTrans.RelayData,pAmrThreadArg->RelayTrans.RelayLen);
            frame.srcLen = pAmrThreadArg->RelayTrans.RelayLen;
            TASK_FMT_TRACE( pipe, RELAY_LOG, "透传组帧 DataLen is %d\n",DataLen);
            Convert_Request_Frame(pipe,&frame,pAmrThreadArg->RelayTrans.reqframeformat,&pAmrThreadArg->RelayTrans.tsa);
            TASK_BUF_TRACE( pipe, RELAY_LOG,pAmrThreadArg->RelayTrans.RelayData,DataLen);
            if(DataLen>0)
            {

                ret=rs485_RS232_send(pipe,MAX_FRAME_OVERTIME,MAX_CHAR_OVERTIME,speed,PARITY_EVEN,frame.dstBuf, frame.dstLen);
                if(ret<0)
                {
                    return -1;
                }
            }
        }
        break;
    default:
        break;

    }
    }
    break;
    default:
        return -1;

    }

    return 1;

}
/**
*********************************************************************
* @brief:      执行别的APP发给taskManager的代理消息；
* @param[in]：  uint8 pipe  线程号
* @return：
*********************************************************************
*/
int Meter485RelayTransDeal (uint8 pipe,uint8 priority)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    static time_t  TaskBeginTime[PLC_NUM+MAX_485_NUM];
    static time_t  BeginTime[PLC_NUM+MAX_485_NUM];
    static uint16  OverTime[PLC_NUM+MAX_485_NUM]; //超时时间
    int ret=0;
    int RecvLen=0;
    int tmprecvlen=0;

    uint8 RecvBuf[MAX_MSG_LEN]={0};
    uint8 count=0;
    MeterPrtlFmt_T  MeterPrtlData;
    char           apdu[2];

    uint16 index=0;
    uint8  tmpbuffer[MAX_MSG_LEN]={0};
    uint8  Byteoffset=0,meterindex=0;
    OOP_OAD_U oad;
    uint8 tmppipe;
    uint16 reallen=0;

    uint8 speed;

    if((pAmrThreadArg->RelayTrans.proxychoice==0x07)&&(pAmrThreadArg->RelayTrans.IOP == 0x0011))
    {
        oad.value = pAmrThreadArg->RelayTrans.OadPort;
        tmppipe = taskmng_mtr_pipe_get(&oad);
        if(tmppipe !=pipe)
        {
            TASK_FMT_TRACE( pipe, RELAY_LOG, "端口不匹配\n");
            pAmrThreadArg->RelayTrans.isValid = 0;
            return -1;
        }
    }


    Init_Relay_Cb_Pipe(pipe,&speed);

    while(1)
    {
        switch(pAmrThreadArg->TotalState.RelayTrans)
        {
        case INNER_RTS_PIPE_INIT:
        {
            TASK_FMT_TRACE( pipe, RELAY_LOG, "穿透任务初始化\n");

            OverTime[pipe] = 3;
            TaskBeginTime[pipe] = time(NULL);

            pAmrThreadArg->TotalState.RelayTrans = INNER_RTS_SEND;//进入组帧处理
        }
            break;
        case INNER_RTS_SEND:
        {
            ret =RelayCmdMeter485PrtlData (pipe,speed,priority);

            TASK_FMT_TRACE( pipe, RELAY_LOG,"透传 ret is %d\n",ret);
            if(ret>0)
            {
                BeginTime[pipe] = time(NULL);
                pAmrThreadArg->TotalState.RelayTrans = INNER_RTS_RECV;
            }else
            {

                TASK_FMT_TRACE( pipe, RELAY_LOG, "485透传结束\n");
                pAmrThreadArg->TotalState.RelayTrans = INNER_RTS_END;
            }
            TASK_FMT_TRACE( pipe, RELAY_LOG,"pAmrThreadArg->TotalState.RelayTrans is %d\n",pAmrThreadArg->TotalState.RelayTrans);
        }
            break;
        case INNER_RTS_RECV:
        {
            uint8 len,lenbuf[3];

            TASK_FMT_TRACE( pipe, RELAY_LOG,"开始接收透传数据\n");
            RecvLen = rs485_RS232_recv(pipe,RecvBuf, sizeof(RecvBuf));

            TASK_FMT_TRACE( pipe, RELAY_LOG,"RecvLen is %d\n",RecvLen);
            meterindex=pAmrThreadArg->RelayTrans.AmrIndex;
            TASK_FMT_TRACE( pipe, RELAY_LOG,"pAmrThreadArg->RelayTrans.METER_LIST[meterindex].VliadMeter is %d\n",pAmrThreadArg->RelayTrans.METER_LIST[meterindex].VliadMeter);
            TASK_BUF_TRACE(pipe, RELAY_LOG,RecvBuf, RecvLen);
            if((RecvLen>0)||((pAmrThreadArg->RelayTrans.METER_LIST[meterindex].VliadMeter==0)&&(pAmrThreadArg->RelayTrans.proxychoice != 0x02)&&(pAmrThreadArg->RelayTrans.proxychoice != 0x07)))
            {
                switch(pAmrThreadArg->RelayTrans.IOP)
                {
                    case 0x0010:
                    {
                        len=set_len_offset(RecvLen,lenbuf);
                        memcpy(pAmrThreadArg->RelayTrans.RelayData,lenbuf,len);
                        pAmrThreadArg->RelayTrans.RelayLen = RecvLen+len;
                        memcpy(pAmrThreadArg->RelayTrans.RelayData+len,RecvBuf,RecvLen);
                        TASK_FMT_TRACE( pipe, RELAY_LOG, "穿透收到数据 任务结束 \n");
                        taskmng_relay_up_msg_write( pipe, 0);
                        pAmrThreadArg->RelayTrans.RelayResult =1;
                        pAmrThreadArg->TotalState.RelayTrans = INNER_RTS_END;

                    }
                    break;
                    case 0x0011:
                    {
                        switch(pAmrThreadArg->RelayTrans.proxychoice)
                        {
                            case 0x01:
                            {
                                uint8 prtl;

                                taskmng_prtl_get_from_tsa(pAmrThreadArg->RelayTrans.AmrNormal[pAmrThreadArg->RelayTrans.AmrIndex].Mtraddr,LOGIC_ID_YC, &prtl);
                                ret = UnpackRelay485Data_0010(pipe,RecvBuf,RecvLen,&MeterPrtlData,apdu);
                                TASK_FMT_TRACE( pipe, RELAY_LOG,"0x0010收到回码 解帧 ret %d \n",ret);

                                if(ret <0 )
                                {
                                    pAmrThreadArg->TotalState.RelayTrans = INNER_RTS_RETRY;//台体测试,现有模块有可能只回复一个字节FF,这里面才尝试一次
                                    break;
                                }
                                if(prtl == PRTL_OOP)
                                {
                                    if(apdu[0]==0x85&&apdu[1]==0x02)
                                    {

                                        Oop485Relay_Save(pipe,MeterPrtlData.DataBuff,MeterPrtlData.DataLen);
                                    }
                                }
                                else
                                {
                                    if(ret == CHECK_RESULT_SUCCEED)
                                    {
                                        if(taskmng_645_relay_isvalid(pipe,prtl,MeterPrtlData.DataBuff,MeterPrtlData.CtrlCode,&MeterPrtlData.DataLen)>0)
                                        {
                                            TASK_FMT_TRACE(pipe, RELAY_LOG,"645验证成功 DataLen %d\n",MeterPrtlData.DataLen);

                                            TASK_BUF_TRACE( pipe, RELAY_LOG,MeterPrtlData.DataBuff, MeterPrtlData.DataLen);
                                            if(prtl == PRTL_97)
                                            {
                                                taskmng_645_relay_data_save(pipe,prtl,&MeterPrtlData.DataBuff[2], MeterPrtlData.DataLen-2);
                                            }else
                                            {
                                                taskmng_645_relay_data_save(pipe,prtl,&MeterPrtlData.DataBuff[4], MeterPrtlData.DataLen-4);
                                            }
                                        }
                                        pAmrThreadArg->RelayTrans.MapIndex++;
                                    }else
                                    {
                                        pAmrThreadArg->RelayTrans.MapIndex++;
                                    }
                                }
                                pAmrThreadArg->TotalState.RelayTrans = INNER_RTS_SEND;
                            }
                            break;
                            case 0x02:
                            {
                                AppLayerList_t* pAppLayer;

                                ret = LinkDataReceive(pipe, RecvBuf, RecvLen,NULL);
                                TASK_FMT_TRACE(pipe, RELAY_LOG,"LinkDataReceive 698解帧返回值 %d \n",ret);
                                if((ret >=eOopComm_Fail))//解帧失败直接返回
                                {
                                    pAmrThreadArg->TotalState.RelayTrans = INNER_RTS_END;
                                    break;
                                }
                                //获取电表地址
                                pAppLayer = FindAppLayer(pipe, NULL);
                                if(pAppLayer == NULL)
                                {
                                    pAmrThreadArg->TotalState.RelayTrans = INNER_RTS_END;
                                    break;
                                }

                                uint16 index=0;
                                uint8  tmpbuffer[MAX_MSG_LEN]={0};
                                uint8  Byteoffset=0;
                                pAmrThreadArg->RelayTrans.RelayData[index++] = 0x89;
                                pAmrThreadArg->RelayTrans.RelayData[index++] = 0x02;
                                pAmrThreadArg->RelayTrans.RelayData[index++] = pAmrThreadArg->RelayTrans.piid;
                                pAmrThreadArg->RelayTrans.RelayData[index++]=0x07;//tsa长度
                                pAmrThreadArg->RelayTrans.RelayData[index++]=5;//
                                memcpy_r(&pAmrThreadArg->RelayTrans.RelayData[index],pAmrThreadArg->RelayTrans.MtrAddr,6);
                                index+=6;

                                memcpy(&pAmrThreadArg->RelayTrans.RelayData[index],&pAppLayer->recvApduList->apdu[3],pAppLayer->recvApduList->apduLen-5);
                                index+=(pAppLayer->recvApduList->apduLen-5);
                                pAmrThreadArg->RelayTrans.RelayData[index++]=0x00;//跟随上报信息域
                                pAmrThreadArg->RelayTrans.RelayData[index++]=0x00;//时间标签
                                if(index<0x80)
                                {
                                     tmpbuffer[Byteoffset++]=index;
                                }else
                                {
                                     tmpbuffer[Byteoffset++] = 0x82;
                                     tmpbuffer[Byteoffset++] = (uint8)(index>>8) ;
                                     tmpbuffer[Byteoffset++] = (uint8)index ;
                                }
                                memcpy(tmpbuffer+Byteoffset,pAmrThreadArg->RelayTrans.RelayData,index);
                                index=index+Byteoffset;
                                memcpy(pAmrThreadArg->RelayTrans.RelayData,tmpbuffer,index);
                                pAmrThreadArg->RelayTrans.RelayLen = index;
                                TASK_FMT_TRACE(pipe, RELAY_LOG,"pAppLayer->recvApduList->apduLen is %d \n",pAppLayer->recvApduList->apduLen);
                                taskmng_relay_up_msg_write( pipe, 0);
                                pAmrThreadArg->RelayTrans.RelayResult =1;
                                pAmrThreadArg->TotalState.RelayTrans = INNER_RTS_END;
                            }
                            break;
                            case 0x03:
                            {
                                AppLayerList_t* pAppLayer;
                                uint8 i;
                                ret = LinkDataReceive(pipe, RecvBuf, RecvLen,NULL);
                                TASK_FMT_TRACE(pipe, RELAY_LOG,"LinkDataReceive 698解帧返回值 %d \n",ret);
                                if((ret >=eOopComm_Fail)&&(pAmrThreadArg->RelayTrans.METER_LIST[meterindex].VliadMeter !=0))//解帧失败直接返回
                                {
                                    pAmrThreadArg->TotalState.RelayTrans = INNER_RTS_END;
                                    break;
                                }
                                //获取电表地址
                                pAppLayer = FindAppLayer(pipe, NULL);
                                if(pAppLayer == NULL)
                                {
                                    pAmrThreadArg->TotalState.RelayTrans = INNER_RTS_END;
                                    break;
                                }

                                if(pAmrThreadArg->RelayTrans.AmrIndex==0)
                                {
                                    pAmrThreadArg->RelayTrans.RelayData[index++] = 0x89;
                                    pAmrThreadArg->RelayTrans.RelayData[index++] = pAmrThreadArg->RelayTrans.proxychoice;
                                    pAmrThreadArg->RelayTrans.RelayData[index++] = pAmrThreadArg->RelayTrans.piid;
                                    pAmrThreadArg->RelayTrans.RelayData[index++] = pAmrThreadArg->RelayTrans.TotalNum;
                                }
                                TASK_FMT_TRACE(pipe, RELAY_LOG,"pAmrThreadArg->RelayTrans.AmrIndex is %d,pAmrThreadArg->RelayTrans.TotalNum is %d\n",pAmrThreadArg->RelayTrans.AmrIndex,pAmrThreadArg->RelayTrans.TotalNum);
                                if(pAmrThreadArg->RelayTrans.AmrIndex<pAmrThreadArg->RelayTrans.TotalNum)
                                {
                                    meterindex=pAmrThreadArg->RelayTrans.AmrIndex;
                                    TASK_FMT_TRACE(pipe, RELAY_LOG,"pAmrThreadArg->RelayTrans.METER_LIST[meterindex].VliadMeter is %d\n",pAmrThreadArg->RelayTrans.METER_LIST[meterindex].VliadMeter);
                                    if(pAmrThreadArg->RelayTrans.METER_LIST[meterindex].VliadMeter==0)
                                    {
                                        pAmrThreadArg->RelayTrans.RelayData[index++]=0x07;//tsa长度
                                        pAmrThreadArg->RelayTrans.RelayData[index++]=5;//
                                        memcpy(&pAmrThreadArg->RelayTrans.RelayData[index],pAmrThreadArg->RelayTrans.METER_LIST[meterindex].Mtraddr.add,6);
                                        index+=6;
                                        pAmrThreadArg->RelayTrans.RelayData[index++] = pAmrThreadArg->RelayTrans.METER_LIST[meterindex].OadNum;
                                        for(i=0;i<pAmrThreadArg->RelayTrans.METER_LIST[meterindex].OadNum;i++)
                                        {
                                            memcpy_r(&pAmrThreadArg->RelayTrans.RelayData[index],&pAmrThreadArg->RelayTrans.METER_LIST[meterindex].SubOad[i],sizeof(OOP_OAD_U));
                                            index+=sizeof(OOP_OAD_U);
                                            pAmrThreadArg->RelayTrans.RelayData[index++]=0xFF;
                                        }
//                                        memcpy(&pAmrThreadArg->RelayTrans.RelayData[index],pAmrThreadArg->RelayTrans.METER_LIST[meterindex].relaydata,pAmrThreadArg->RelayTrans.METER_LIST[meterindex].relaydatalen);
//                                        index+=pAmrThreadArg->RelayTrans.METER_LIST[meterindex].relaydatalen;
//                                        pAmrThreadArg->RelayTrans.RelayData[index++]=0xFF;
                                        pAmrThreadArg->RelayTrans.AmrIndex++;
                                        if(pAmrThreadArg->RelayTrans.AmrIndex>=pAmrThreadArg->RelayTrans.TotalNum)
                                        {
                                        }
                                        else
                                        {
                                            pAmrThreadArg->TotalState.RelayTrans = INNER_RTS_SEND;
                                            break;
                                        }
                                    }
                                    else
                                    {
                                        pAmrThreadArg->RelayTrans.RelayData[index++]=0x07;//tsa长度
                                        pAmrThreadArg->RelayTrans.RelayData[index++]=5;//
                                        memcpy(&pAmrThreadArg->RelayTrans.RelayData[index],pAmrThreadArg->RelayTrans.METER_LIST[meterindex].Mtraddr.add,6);
                                        index+=6;

                                        memcpy(&pAmrThreadArg->RelayTrans.RelayData[index],&pAppLayer->recvApduList->apdu[3],pAppLayer->recvApduList->apduLen-5);
                                        index+=(pAppLayer->recvApduList->apduLen-5);
                                        pAmrThreadArg->RelayTrans.AmrIndex++;
                                        if(pAmrThreadArg->RelayTrans.AmrIndex>=pAmrThreadArg->RelayTrans.TotalNum)
                                        {
                                        }
                                        else
                                        {
                                            pAmrThreadArg->TotalState.RelayTrans = INNER_RTS_SEND;
                                            break;
                                        }

                                    }
                                }
                                pAmrThreadArg->RelayTrans.RelayData[index++]=0x00;//跟随上报信息域
                                pAmrThreadArg->RelayTrans.RelayData[index++]=0x00;//时间标签
                                if(index<0x80)
                                {
                                     tmpbuffer[Byteoffset++]=index;
                                }else
                                {
                                     tmpbuffer[Byteoffset++] = 0x82;
                                     tmpbuffer[Byteoffset++] = (uint8)(index>>8) ;
                                     tmpbuffer[Byteoffset++] = (uint8)index ;
                                }
                                memcpy(tmpbuffer+Byteoffset,pAmrThreadArg->RelayTrans.RelayData,index);
                                index=index+Byteoffset;
                                memcpy(pAmrThreadArg->RelayTrans.RelayData,tmpbuffer,index);
                                pAmrThreadArg->RelayTrans.RelayLen = index;
                                TASK_FMT_TRACE(pipe, RELAY_LOG,"pAppLayer->recvApduList->apduLen is %d \n",pAppLayer->recvApduList->apduLen);
                                taskmng_relay_up_msg_write( pipe, 0);
                                pAmrThreadArg->RelayTrans.RelayResult =1;
                                pAmrThreadArg->TotalState.RelayTrans = INNER_RTS_END;

                            }
                            break;
                            case 0x04:
                            {
                                AppLayerList_t* pAppLayer;
                                uint8 mn;

                                ret = LinkDataReceive(pipe, RecvBuf, RecvLen,NULL);
                                TASK_FMT_TRACE(pipe, RELAY_LOG,"LinkDataReceive 698解帧返回值 %d \n",ret);
                                if((ret >=eOopComm_Fail)&&(pAmrThreadArg->RelayTrans.METER_LIST[meterindex].VliadMeter !=0))//解帧失败直接返回
                                {
                                    pAmrThreadArg->TotalState.RelayTrans = INNER_RTS_END;
                                    break;
                                }
                                //获取电表地址
                                pAppLayer = FindAppLayer(pipe, NULL);
                                if(pAppLayer == NULL)
                                {
                                    pAmrThreadArg->TotalState.RelayTrans = INNER_RTS_END;
                                    break;
                                }

                                if(pAmrThreadArg->RelayTrans.AmrIndex==0)
                                {
                                    pAmrThreadArg->RelayTrans.RelayData[index++] = 0x89;
                                    pAmrThreadArg->RelayTrans.RelayData[index++] = pAmrThreadArg->RelayTrans.proxychoice;
                                    pAmrThreadArg->RelayTrans.RelayData[index++] = pAmrThreadArg->RelayTrans.piid;
                                    pAmrThreadArg->RelayTrans.RelayData[index++] = pAmrThreadArg->RelayTrans.TotalNum;
                                }
                                TASK_FMT_TRACE(pipe, RELAY_LOG,"pAmrThreadArg->RelayTrans.AmrIndex is %d,pAmrThreadArg->RelayTrans.TotalNum is %d\n",pAmrThreadArg->RelayTrans.AmrIndex,pAmrThreadArg->RelayTrans.TotalNum);
                                if(pAmrThreadArg->RelayTrans.AmrIndex<pAmrThreadArg->RelayTrans.TotalNum)
                                {
                                    meterindex=pAmrThreadArg->RelayTrans.AmrIndex;
                                    TASK_FMT_TRACE(pipe, RELAY_LOG,"pAmrThreadArg->RelayTrans.METER_LIST[meterindex].VliadMeter is %d\n",pAmrThreadArg->RelayTrans.METER_LIST[meterindex].VliadMeter);
                                    if(pAmrThreadArg->RelayTrans.METER_LIST[meterindex].VliadMeter==0)
                                    {
                                        pAmrThreadArg->RelayTrans.RelayData[index++]=0x07;//tsa长度
                                        pAmrThreadArg->RelayTrans.RelayData[index++]=5;//
                                        memcpy(&pAmrThreadArg->RelayTrans.RelayData[index],pAmrThreadArg->RelayTrans.METER_LIST[meterindex].Mtraddr.add,6);
                                        index+=6;
                                        pAmrThreadArg->RelayTrans.RelayData[index++]=pAmrThreadArg->RelayTrans.METER_LIST[meterindex].OadNum;
                                        for(mn=0;mn<pAmrThreadArg->RelayTrans.METER_LIST[meterindex].OadNum;mn++)
                                        {

                                            memcpy_r(pAmrThreadArg->RelayTrans.RelayData+index,(uint8*)&pAmrThreadArg->RelayTrans.METER_LIST[meterindex].SetOad[mn].value,4);
                                            index=index+4;
                                            pAmrThreadArg->RelayTrans.RelayData[index++]=0x21;

                                            memcpy_r(pAmrThreadArg->RelayTrans.RelayData+index,(uint8*)&pAmrThreadArg->RelayTrans.METER_LIST[meterindex].GetOad[mn].value,4);
                                            index=index+4;
                                            pAmrThreadArg->RelayTrans.RelayData[index++]=0x00;
                                            pAmrThreadArg->RelayTrans.RelayData[index++]=0x21;

                                        }
                                        pAmrThreadArg->RelayTrans.AmrIndex++;
                                        if(pAmrThreadArg->RelayTrans.AmrIndex>=pAmrThreadArg->RelayTrans.TotalNum)
                                        {
                                        }
                                        else
                                        {
                                            pAmrThreadArg->TotalState.RelayTrans = INNER_RTS_SEND;
                                            break;
                                        }
                                    }
                                    else
                                    {
                                        pAmrThreadArg->RelayTrans.RelayData[index++]=0x07;//tsa长度
                                        pAmrThreadArg->RelayTrans.RelayData[index++]=5;//
                                        memcpy(&pAmrThreadArg->RelayTrans.RelayData[index],pAmrThreadArg->RelayTrans.METER_LIST[meterindex].Mtraddr.add,6);
                                        index+=6;

                                        memcpy(&pAmrThreadArg->RelayTrans.RelayData[index],&pAppLayer->recvApduList->apdu[3],pAppLayer->recvApduList->apduLen-5);
                                        index+=(pAppLayer->recvApduList->apduLen-5);
                                        pAmrThreadArg->RelayTrans.AmrIndex++;
                                        if(pAmrThreadArg->RelayTrans.AmrIndex>=pAmrThreadArg->RelayTrans.TotalNum)
                                        {
                                        }
                                        else
                                        {
                                            pAmrThreadArg->TotalState.RelayTrans = INNER_RTS_SEND;
                                            break;
                                        }

                                    }
                                }
                                pAmrThreadArg->RelayTrans.RelayData[index++]=0x00;//跟随上报信息域
                                pAmrThreadArg->RelayTrans.RelayData[index++]=0x00;//时间标签
                                if(index<0x80)
                                {
                                     tmpbuffer[Byteoffset++]=index;
                                }else
                                {
                                     tmpbuffer[Byteoffset++] = 0x82;
                                     tmpbuffer[Byteoffset++] = (uint8)(index>>8) ;
                                     tmpbuffer[Byteoffset++] = (uint8)index ;
                                }
                                memcpy(tmpbuffer+Byteoffset,pAmrThreadArg->RelayTrans.RelayData,index);
                                index=index+Byteoffset;
                                memcpy(pAmrThreadArg->RelayTrans.RelayData,tmpbuffer,index);
                                pAmrThreadArg->RelayTrans.RelayLen = index;
                                TASK_FMT_TRACE(pipe, RELAY_LOG,"pAppLayer->recvApduList->apduLen is %d \n",pAppLayer->recvApduList->apduLen);
                                taskmng_relay_up_msg_write( pipe, 0);
                                pAmrThreadArg->RelayTrans.RelayResult =1;
                                pAmrThreadArg->TotalState.RelayTrans = INNER_RTS_END;

                            }
                            break;
                            case 0x05:
                            {
                                AppLayerList_t* pAppLayer;
                                uint8 i;
                                ret = LinkDataReceive(pipe, RecvBuf, RecvLen,NULL);
                                TASK_FMT_TRACE(pipe, RELAY_LOG,"LinkDataReceive 698解帧返回值 %d \n",ret);
                                if((ret >=eOopComm_Fail)&&(pAmrThreadArg->RelayTrans.METER_LIST[meterindex].VliadMeter !=0))//解帧失败直接返回
                                {
                                    pAmrThreadArg->TotalState.RelayTrans = INNER_RTS_END;
                                    break;
                                }
                                //获取电表地址
                                pAppLayer = FindAppLayer(pipe, NULL);
                                if(pAppLayer == NULL)
                                {
                                    pAmrThreadArg->TotalState.RelayTrans = INNER_RTS_END;
                                    break;
                                }

                                if(pAmrThreadArg->RelayTrans.AmrIndex==0)
                                {
                                    pAmrThreadArg->RelayTrans.RelayData[index++] = 0x89;
                                    pAmrThreadArg->RelayTrans.RelayData[index++] = pAmrThreadArg->RelayTrans.proxychoice;
                                    pAmrThreadArg->RelayTrans.RelayData[index++] = pAmrThreadArg->RelayTrans.piid;
                                    pAmrThreadArg->RelayTrans.RelayData[index++] = pAmrThreadArg->RelayTrans.TotalNum;
                                }
                                TASK_FMT_TRACE(pipe, RELAY_LOG,"pAmrThreadArg->RelayTrans.AmrIndex is %d,pAmrThreadArg->RelayTrans.TotalNum is %d\n",pAmrThreadArg->RelayTrans.AmrIndex,pAmrThreadArg->RelayTrans.TotalNum);
                                if(pAmrThreadArg->RelayTrans.AmrIndex<pAmrThreadArg->RelayTrans.TotalNum)
                                {
                                    meterindex=pAmrThreadArg->RelayTrans.AmrIndex;
                                    TASK_FMT_TRACE(pipe, RELAY_LOG,"pAmrThreadArg->RelayTrans.METER_LIST[meterindex].VliadMeter is %d\n",pAmrThreadArg->RelayTrans.METER_LIST[meterindex].VliadMeter);
                                    if(pAmrThreadArg->RelayTrans.METER_LIST[meterindex].VliadMeter==0)
                                    {
                                        pAmrThreadArg->RelayTrans.RelayData[index++]=0x07;//tsa长度
                                        pAmrThreadArg->RelayTrans.RelayData[index++]=5;//
                                        memcpy(&pAmrThreadArg->RelayTrans.RelayData[index],pAmrThreadArg->RelayTrans.METER_LIST[meterindex].Mtraddr.add,6);
                                        index+=6;
                                        pAmrThreadArg->RelayTrans.RelayData[index++]=pAmrThreadArg->RelayTrans.METER_LIST[meterindex].OadNum;
                                        for(i=0;i<pAmrThreadArg->RelayTrans.METER_LIST[meterindex].OadNum;i++)
                                        {
                                            memcpy_r(pAmrThreadArg->RelayTrans.RelayData+index,(uint8*)&pAmrThreadArg->RelayTrans.METER_LIST[meterindex].SetOad[i].value,4);
                                            index=index+4;
                                            pAmrThreadArg->RelayTrans.RelayData[index++]=0xFF;
                                            pAmrThreadArg->RelayTrans.RelayData[index++]=0x00;
                                        }
//                                        memcpy(&pAmrThreadArg->RelayTrans.RelayData[index],pAmrThreadArg->RelayTrans.METER_LIST[meterindex].relaydata,pAmrThreadArg->RelayTrans.METER_LIST[meterindex].relaydatalen);
//                                        index+=pAmrThreadArg->RelayTrans.METER_LIST[meterindex].relaydatalen;
//                                        pAmrThreadArg->RelayTrans.RelayData[index++]=0xFF;
//                                        pAmrThreadArg->RelayTrans.RelayData[index++]=0x00;
                                        pAmrThreadArg->RelayTrans.AmrIndex++;
                                        if(pAmrThreadArg->RelayTrans.AmrIndex>=pAmrThreadArg->RelayTrans.TotalNum)
                                        {
                                        }
                                        else
                                        {
                                            pAmrThreadArg->TotalState.RelayTrans = INNER_RTS_SEND;
                                            break;
                                        }
                                    }
                                    else
                                    {
                                        pAmrThreadArg->RelayTrans.RelayData[index++]=0x07;//tsa长度
                                        pAmrThreadArg->RelayTrans.RelayData[index++]=5;//
                                        memcpy(&pAmrThreadArg->RelayTrans.RelayData[index],pAmrThreadArg->RelayTrans.METER_LIST[meterindex].Mtraddr.add,6);
                                        index+=6;

                                        memcpy(&pAmrThreadArg->RelayTrans.RelayData[index],&pAppLayer->recvApduList->apdu[3],pAppLayer->recvApduList->apduLen-5);
                                        index+=(pAppLayer->recvApduList->apduLen-5);
                                        pAmrThreadArg->RelayTrans.AmrIndex++;
                                        if(pAmrThreadArg->RelayTrans.AmrIndex>=pAmrThreadArg->RelayTrans.TotalNum)
                                        {
                                        }
                                        else
                                        {
                                            pAmrThreadArg->TotalState.RelayTrans = INNER_RTS_SEND;
                                            break;
                                        }

                                    }
                                }
                                pAmrThreadArg->RelayTrans.RelayData[index++]=0x00;//跟随上报信息域
                                pAmrThreadArg->RelayTrans.RelayData[index++]=0x00;//时间标签
                                if(index<0x80)
                                {
                                     tmpbuffer[Byteoffset++]=index;
                                }else
                                {
                                     tmpbuffer[Byteoffset++] = 0x82;
                                     tmpbuffer[Byteoffset++] = (uint8)(index>>8) ;
                                     tmpbuffer[Byteoffset++] = (uint8)index ;
                                }
                                memcpy(tmpbuffer+Byteoffset,pAmrThreadArg->RelayTrans.RelayData,index);
                                index=index+Byteoffset;
                                memcpy(pAmrThreadArg->RelayTrans.RelayData,tmpbuffer,index);
                                pAmrThreadArg->RelayTrans.RelayLen = index;
                                TASK_FMT_TRACE(pipe, RELAY_LOG,"pAppLayer->recvApduList->apduLen is %d \n",pAppLayer->recvApduList->apduLen);
                                taskmng_relay_up_msg_write( pipe, 0);
                                pAmrThreadArg->RelayTrans.RelayResult =1;
                                pAmrThreadArg->TotalState.RelayTrans = INNER_RTS_END;

                            }
                            break;
                            case 0x06:
                            {
                                AppLayerList_t* pAppLayer;
                                uint8 mn;
                                TASK_FMT_TRACE(pipe, RELAY_LOG,"pAmrThreadArg->RelayTrans.proxychoice is %d \n",pAmrThreadArg->RelayTrans.proxychoice);
                                TASK_BUF_TRACE(pipe, RELAY_LOG,RecvBuf, RecvLen);
                                ret = LinkDataReceive(pipe, RecvBuf, RecvLen,NULL);
                                TASK_FMT_TRACE(pipe, RELAY_LOG,"LinkDataReceive 698解帧返回值 %d \n",ret);
                                if((ret >=eOopComm_Fail)&&(pAmrThreadArg->RelayTrans.METER_LIST[meterindex].VliadMeter !=0))//解帧失败直接返回
                                {
                                    pAmrThreadArg->TotalState.RelayTrans = INNER_RTS_END;
                                    break;
                                }
                                //获取电表地址
                                pAppLayer = FindAppLayer(pipe, NULL);
                                if(pAppLayer == NULL)
                                {
                                    pAmrThreadArg->TotalState.RelayTrans = INNER_RTS_END;
                                    break;
                                }

                                if(pAmrThreadArg->RelayTrans.AmrIndex==0)
                                {
                                    pAmrThreadArg->RelayTrans.RelayData[index++] = 0x89;
                                    pAmrThreadArg->RelayTrans.RelayData[index++] = pAmrThreadArg->RelayTrans.proxychoice;
                                    pAmrThreadArg->RelayTrans.RelayData[index++] = pAmrThreadArg->RelayTrans.piid;
                                    pAmrThreadArg->RelayTrans.RelayData[index++] = pAmrThreadArg->RelayTrans.TotalNum;
                                }
                                TASK_FMT_TRACE(pipe, RELAY_LOG,"pAmrThreadArg->RelayTrans.AmrIndex is %d,pAmrThreadArg->RelayTrans.TotalNum is %d\n",pAmrThreadArg->RelayTrans.AmrIndex,pAmrThreadArg->RelayTrans.TotalNum);
                                if(pAmrThreadArg->RelayTrans.AmrIndex<pAmrThreadArg->RelayTrans.TotalNum)
                                {
                                    meterindex=pAmrThreadArg->RelayTrans.AmrIndex;
                                    TASK_FMT_TRACE(pipe, RELAY_LOG,"pAmrThreadArg->RelayTrans.METER_LIST[meterindex].VliadMeter is %d\n",pAmrThreadArg->RelayTrans.METER_LIST[meterindex].VliadMeter);
                                    if(pAmrThreadArg->RelayTrans.METER_LIST[meterindex].VliadMeter==0)
                                    {
                                        pAmrThreadArg->RelayTrans.RelayData[index++]=0x07;//tsa长度
                                        pAmrThreadArg->RelayTrans.RelayData[index++]=5;//
                                        memcpy(&pAmrThreadArg->RelayTrans.RelayData[index],pAmrThreadArg->RelayTrans.METER_LIST[meterindex].Mtraddr.add,6);
                                        index+=6;
                                        pAmrThreadArg->RelayTrans.RelayData[index++]=pAmrThreadArg->RelayTrans.METER_LIST[meterindex].OadNum;
                                        for(mn=0;mn<pAmrThreadArg->RelayTrans.METER_LIST[meterindex].OadNum;mn++)
                                        {

                                            memcpy_r(pAmrThreadArg->RelayTrans.RelayData+index,(uint8*)&pAmrThreadArg->RelayTrans.METER_LIST[meterindex].SetOad[mn].value,4);
                                            index=index+4;
                                            pAmrThreadArg->RelayTrans.RelayData[index++]=0x21;
                                            pAmrThreadArg->RelayTrans.RelayData[index++]=0x00;
                                            memcpy_r(pAmrThreadArg->RelayTrans.RelayData+index,(uint8*)&pAmrThreadArg->RelayTrans.METER_LIST[meterindex].GetOad[mn].value,4);
                                            index=index+4;
                                            pAmrThreadArg->RelayTrans.RelayData[index++]=0x00;
                                            pAmrThreadArg->RelayTrans.RelayData[index++]=0x21;

                                        }
                                        pAmrThreadArg->RelayTrans.AmrIndex++;
                                        if(pAmrThreadArg->RelayTrans.AmrIndex>=pAmrThreadArg->RelayTrans.TotalNum)
                                        {
                                        }
                                        else
                                        {
                                            pAmrThreadArg->TotalState.RelayTrans = INNER_RTS_SEND;
                                            break;
                                        }
                                    }
                                    else
                                    {
                                        pAmrThreadArg->RelayTrans.RelayData[index++]=0x07;//tsa长度
                                        pAmrThreadArg->RelayTrans.RelayData[index++]=5;//
                                        memcpy(&pAmrThreadArg->RelayTrans.RelayData[index],pAmrThreadArg->RelayTrans.METER_LIST[meterindex].Mtraddr.add,6);
                                        index+=6;

                                        memcpy(&pAmrThreadArg->RelayTrans.RelayData[index],&pAppLayer->recvApduList->apdu[3],pAppLayer->recvApduList->apduLen-5);
                                        index+=(pAppLayer->recvApduList->apduLen-5);
                                        pAmrThreadArg->RelayTrans.AmrIndex++;
                                        if(pAmrThreadArg->RelayTrans.AmrIndex>=pAmrThreadArg->RelayTrans.TotalNum)
                                        {
                                        }
                                        else
                                        {
                                            pAmrThreadArg->TotalState.RelayTrans = INNER_RTS_SEND;
                                            break;
                                        }

                                    }
                                }
                                pAmrThreadArg->RelayTrans.RelayData[index++]=0x00;//跟随上报信息域
                                pAmrThreadArg->RelayTrans.RelayData[index++]=0x00;//时间标签
                                if(index<0x80)
                                {
                                     tmpbuffer[Byteoffset++]=index;
                                }else
                                {
                                     tmpbuffer[Byteoffset++] = 0x82;
                                     tmpbuffer[Byteoffset++] = (uint8)(index>>8) ;
                                     tmpbuffer[Byteoffset++] = (uint8)index ;
                                }
                                memcpy(tmpbuffer+Byteoffset,pAmrThreadArg->RelayTrans.RelayData,index);
                                index=index+Byteoffset;
                                memcpy(pAmrThreadArg->RelayTrans.RelayData,tmpbuffer,index);
                                pAmrThreadArg->RelayTrans.RelayLen = index;
                                TASK_FMT_TRACE(pipe, RELAY_LOG,"pAppLayer->recvApduList->apduLen is %d \n",pAppLayer->recvApduList->apduLen);
                                taskmng_relay_up_msg_write( pipe, 0);
                                pAmrThreadArg->RelayTrans.RelayResult =1;
                                pAmrThreadArg->TotalState.RelayTrans = INNER_RTS_END;

                            }
                            break;
                            case 0x07:
                            {
                                uint16 Offset=0;
                                uint8  *ptr=NULL;
                                TASK_FMT_TRACE(pipe, RELAY_LOG,"pAmrThreadArg->RelayTrans.frametype is %d \n",pAmrThreadArg->RelayTrans.frametype);
                                if(pAmrThreadArg->RelayTrans.frametype == PRTL_OOP)
                                {
                                    tmprecvlen=GetFrame(RecvBuf, RecvLen, &Offset);
                                }
                                else
                                {
                         #ifdef PRODUCT_ECU
                                    if(pAmrThreadArg->RelayTrans.frametype == PRTL_MBUS)
                                    {
                                        tmprecvlen = RecvLen;
                                    }else
                                        {
                         #endif
                                    if(pAmrThreadArg->RelayTrans.frametype == PRTL_07)
                                    {
                                        ptr = (uint8*)commfun_645_frame_get((char*)RecvBuf, 0x68, RecvLen,&reallen);//找到645报文的68头
                                        if(ptr == NULL)
                                        {
                                            RecvLen=0;
                                            pAmrThreadArg->TotalState.RelayTrans = INNER_RTS_END;
                                            break;
                                        }
                                        tmprecvlen = reallen;//纯645报文长度
                                        if(tmprecvlen <=0)
                                        {
                                            RecvLen=0;
                                            pAmrThreadArg->TotalState.RelayTrans = INNER_RTS_END;
                                            break;
                                        }
                                        memmove(RecvBuf, ptr, RecvLen);
                                    }
                                    else if(pAmrThreadArg->RelayTrans.frametype == PRTL_UNKNOWN)
                                    {
                                        tmprecvlen = RecvLen;
                                    }
                    #ifdef PRODUCT_ECU
                                    }
                    #endif
                                }
                                pAmrThreadArg->RelayTrans.RelayData[index++] = 0x89;
                                pAmrThreadArg->RelayTrans.RelayData[index++] = pAmrThreadArg->RelayTrans.proxychoice;
                                pAmrThreadArg->RelayTrans.RelayData[index++] = pAmrThreadArg->RelayTrans.piid;
                                //端口OAD
                                memcpy_r((uint8*)&pAmrThreadArg->RelayTrans.RelayData[index], (uint8*)&pAmrThreadArg->RelayTrans.OadPort, 4);
                                index =index +4;
                                //结果有数据
                                pAmrThreadArg->RelayTrans.RelayData[index++] = 0x01;
                                //数据octer_string
                                len=set_len_offset(tmprecvlen,lenbuf);
                                memcpy(pAmrThreadArg->RelayTrans.RelayData+index,lenbuf,len);
                                index = index +len;
                                memcpy(pAmrThreadArg->RelayTrans.RelayData+index,RecvBuf+Offset,tmprecvlen);
                                index = index + tmprecvlen;
                                pAmrThreadArg->RelayTrans.RelayData[index++]=0x00;//跟随上报信息域
                                pAmrThreadArg->RelayTrans.RelayData[index++]=0x00;//时间标签
                                if(index<0x80)
                                {
                                     tmpbuffer[Byteoffset++]=index;
                                }else
                                {
                                     tmpbuffer[Byteoffset++] = 0x82;
                                     tmpbuffer[Byteoffset++] = (uint8)(index>>8) ;
                                     tmpbuffer[Byteoffset++] = (uint8)index ;
                                }
                                memcpy(tmpbuffer+Byteoffset,pAmrThreadArg->RelayTrans.RelayData,index);
                                index=index+Byteoffset;
                                memcpy(pAmrThreadArg->RelayTrans.RelayData,tmpbuffer,index);
                                pAmrThreadArg->RelayTrans.RelayLen = index;
                                TASK_FMT_TRACE(pipe, RELAY_LOG,"pAmrThreadArg->RelayTrans.RelayLen is %d \n",pAmrThreadArg->RelayTrans.RelayLen);
                                taskmng_relay_up_msg_write( pipe, 0);
                                pAmrThreadArg->RelayTrans.RelayResult =1;
                                pAmrThreadArg->TotalState.RelayTrans = INNER_RTS_END;

                            }
                            break;
                            case 0x50:
                            {
                                uint16 Offset=0;
                                uint8  *ptr=NULL;
                                FRAME_CONVERT_T frame;
                                TASK_FMT_TRACE(pipe, RELAY_LOG,"pAmrThreadArg->RelayTrans.frametype is %d \n",pAmrThreadArg->RelayTrans.frametype);
                                if(pAmrThreadArg->RelayTrans.frametype == PRTL_OOP)
                                {
                                    tmprecvlen=GetFrame(RecvBuf, RecvLen, &Offset);
                                }
                                else
                                {
                                    ptr = (uint8*)commfun_645_frame_get((char*)RecvBuf, 0x68, RecvLen,&reallen);//找到645报文的68头
                                    if(ptr == NULL)
                                    {
                                        RecvLen=0;
                                        pAmrThreadArg->TotalState.RelayTrans = INNER_RTS_END;
                                        break;
                                    }
                                    tmprecvlen = reallen;//纯645报文长度
                                    if(tmprecvlen <=0)
                                    {
                                        RecvLen=0;
                                        pAmrThreadArg->TotalState.RelayTrans = INNER_RTS_END;
                                        break;
                                    }
                                    memmove(RecvBuf, ptr, RecvLen);
                                }
                                memcpy(frame.srcBuf,RecvBuf,RecvLen);
                                frame.srcLen = RecvLen;
                                Convert_Response_Frame(pipe,&frame,pAmrThreadArg->RelayTrans.resframeformat);
                                pAmrThreadArg->RelayTrans.RelayData[index++] = 0x89;
                                pAmrThreadArg->RelayTrans.RelayData[index++] = pAmrThreadArg->RelayTrans.proxychoice;
                                pAmrThreadArg->RelayTrans.RelayData[index++] = pAmrThreadArg->RelayTrans.piid;
                                //端口OAD
                                memcpy_r((uint8*)&pAmrThreadArg->RelayTrans.RelayData[index], (uint8*)&pAmrThreadArg->RelayTrans.OadPort, 4);
                                index =index +4;
                                //通信地址
                                pAmrThreadArg->RelayTrans.RelayData[index++] = pAmrThreadArg->RelayTrans.tsa.af;
                                memcpy((uint8*)&pAmrThreadArg->RelayTrans.RelayData[index],pAmrThreadArg->RelayTrans.tsa.add,pAmrThreadArg->RelayTrans.tsa.len+1);
                                index = index+pAmrThreadArg->RelayTrans.tsa.len+1;
                                //通信对象类型
                                pAmrThreadArg->RelayTrans.RelayData[index++] = pAmrThreadArg->RelayTrans.objiecttype;
                                //响应报文格式字
                                pAmrThreadArg->RelayTrans.RelayData[index++] = pAmrThreadArg->RelayTrans.resframeformat;
                                //结果有数据
                                pAmrThreadArg->RelayTrans.RelayData[index++] = 0x01;
                                //数据octer_string
                                len=set_len_offset(frame.dstLen,lenbuf);
                                memcpy(pAmrThreadArg->RelayTrans.RelayData+index,lenbuf,len);
                                index = index +len;
                                memcpy(pAmrThreadArg->RelayTrans.RelayData+index,frame.dstBuf,frame.dstLen);
                                index = index + frame.dstLen;
                                pAmrThreadArg->RelayTrans.RelayData[index++]=0x00;//跟随上报信息域
                                pAmrThreadArg->RelayTrans.RelayData[index++]=0x00;//时间标签
                                if(index<0x80)
                                {
                                     tmpbuffer[Byteoffset++]=index;
                                }else
                                {
                                     tmpbuffer[Byteoffset++] = 0x82;
                                     tmpbuffer[Byteoffset++] = (uint8)(index>>8) ;
                                     tmpbuffer[Byteoffset++] = (uint8)index ;
                                }
                                memcpy(tmpbuffer+Byteoffset,pAmrThreadArg->RelayTrans.RelayData,index);
                                index=index+Byteoffset;
                                memcpy(pAmrThreadArg->RelayTrans.RelayData,tmpbuffer,index);
                                pAmrThreadArg->RelayTrans.RelayLen = index;
                                TASK_FMT_TRACE(pipe, RELAY_LOG,"pAmrThreadArg->RelayTrans.RelayLen is %d \n",pAmrThreadArg->RelayTrans.RelayLen);
                                taskmng_relay_up_msg_write( pipe, 0);
                                pAmrThreadArg->RelayTrans.RelayResult =1;
                                pAmrThreadArg->TotalState.RelayTrans = INNER_RTS_END;

                            }
                            break;
                            default:
                                break;
                        }
                    }
                    break;
                    default:
                    break;

                }

            }
            else
            {
                TASK_FMT_TRACE(pipe, RELAY_LOG,"time(NULL) is %d ,BeginTime[pipe] is %d,OverTime[pipe] is %d\n",time(NULL),BeginTime[pipe],OverTime[pipe]);
                //if(((time(NULL)-BeginTime[pipe])>=OverTime[pipe])||(time(NULL)<BeginTime[pipe]))
                {
                    TASK_FMT_TRACE( pipe, RELAY_LOG, "穿透超过3s未收到回复 \n");

                    pAmrThreadArg->TotalState.RelayTrans = INNER_RTS_RETRY;
                }
            }
        }
            break;
        case INNER_RTS_RETRY:
        {
            switch(pAmrThreadArg->RelayTrans.IOP)
            {
             case 0x0010:
                {
                    TASK_FMT_TRACE( pipe, RELAY_LOG, "穿透超时 超时时间 %d 当前时间 %ld TaskBeginTime %ld \n",pAmrThreadArg->RelayTrans.OverTime,time(NULL),TaskBeginTime[pipe]);
                    if(((time(NULL)-TaskBeginTime[pipe])>pAmrThreadArg->RelayTrans.OverTime)||(time(NULL)<TaskBeginTime[pipe]))
                    {
                        TASK_FMT_TRACE( pipe, RELAY_LOG, "穿透超时 任务结束 \n");
                        taskmng_relay_up_msg_write( pipe, -1);
                        pAmrThreadArg->TotalState.RelayTrans = INNER_RTS_END;
                        break;
                    }
                    TASK_FMT_TRACE( pipe, RELAY_LOG, "count is  %d\n",count);
                    if(count<2)
                    {
                        count++;
                        pAmrThreadArg->TotalState.RelayTrans = INNER_RTS_SEND;
                    }
                    else
                    {
                        pAmrThreadArg->TotalState.RelayTrans = INNER_RTS_END;
                        count=0;
                    }
                    TASK_FMT_TRACE( pipe, RELAY_LOG, "pAmrThreadArg->TotalState.RelayTrans is  %d\n",pAmrThreadArg->TotalState.RelayTrans);
                }
            break;
            case 0x0011:
                {
                    switch(pAmrThreadArg->RelayTrans.proxychoice)
                    {
                            case 0x01:
                            {
                                uint8 MtrIndex = pAmrThreadArg->RelayTrans.AmrIndex;
                                TASK_FMT_TRACE( pipe, RELAY_LOG, "MtrIndex %d 发送时间 %ld 超时时间 %d \n",MtrIndex,pAmrThreadArg->RelayTrans.AmrNormal[MtrIndex].SendTime,pAmrThreadArg->RelayTrans.AmrNormal[MtrIndex].OverTime);
                                TASK_FMT_TRACE( pipe, RELAY_LOG, "timenow %ld TaskBeginTime %ld OverTime %d \n",time(NULL),TaskBeginTime[pipe],pAmrThreadArg->RelayTrans.OverTime);

                                if(((time(NULL)- pAmrThreadArg->RelayTrans.AmrNormal[MtrIndex].SendTime)<pAmrThreadArg->RelayTrans.AmrNormal[MtrIndex].OverTime)||(time(NULL)<pAmrThreadArg->RelayTrans.AmrNormal[MtrIndex].SendTime))
                                {
//                                    pAmrThreadArg->RelayTrans.AmrIndex++;
//                                    pAmrThreadArg->RelayTrans.OadIndex=0;
//                                    pAmrThreadArg->RelayTrans.MapIndex=0;
                                    TASK_FMT_TRACE( pipe, RELAY_LOG, "穿透重试次数 count is %d  \n",count);
                                    if(count<3)
                                    {
                                        count++;
//                                        if(count==2)
//                                        {
//                                             pAmrThreadArg->RelayTrans.OadIndex++;
//                                             pAmrThreadArg->RelayTrans.MapIndex=0;
//                                        }
                                        pAmrThreadArg->TotalState.RelayTrans = INNER_RTS_SEND;
                                    }
                                    else
                                    {
                                        pAmrThreadArg->TotalState.RelayTrans = INNER_RTS_END;
                                        count=0;
                                    }
                                    TASK_FMT_TRACE( pipe, RELAY_LOG, "该表穿透超时 AmrIndex %d  \n",pAmrThreadArg->RelayTrans.AmrIndex);
                                }
                                if(((time(NULL)-TaskBeginTime[pipe])>pAmrThreadArg->RelayTrans.OverTime)||(time(NULL)<TaskBeginTime[pipe]))
                                {
                                    TASK_FMT_TRACE( pipe, RELAY_LOG, "穿透超时 任务结束 \n");
                                    pAmrThreadArg->TotalState.RelayTrans = INNER_RTS_END;
                                }
                            }
                            break;
                            case 0x02:
                            case 0x03:
                            case 0x04:
                            case 0x05:
                            case 0x06:
                            {
                                TASK_FMT_TRACE( pipe, RELAY_LOG, "穿透超时 超时时间 %d 当前时间 %ld TaskBeginTime %ld \n",pAmrThreadArg->RelayTrans.OverTime,time(NULL),TaskBeginTime[pipe]);
                                if(((time(NULL)-TaskBeginTime[pipe])>pAmrThreadArg->RelayTrans.OverTime)||(time(NULL)<TaskBeginTime[pipe]))
                                {
                                    TASK_FMT_TRACE( pipe, RELAY_LOG, "穿透超时 任务结束 \n");
                                    taskmng_relay_up_msg_write( pipe, -1);

                                    pAmrThreadArg->TotalState.RelayTrans = INNER_RTS_END;
                                    count=0;

                                    break;
                                }
                                TASK_FMT_TRACE( pipe, RELAY_LOG, "重试次数  count is %d\n",count);
                                 if(count<1)
                                {
                                    count++;
                                    pAmrThreadArg->TotalState.RelayTrans = INNER_RTS_SEND;
                                }
                                else
                                {
                                    count=0;
                                    pAmrThreadArg->TotalState.RelayTrans = INNER_RTS_END;
                                }

                            }
                            break;
                            case 0x07:
                                {
                                    TASK_FMT_TRACE( pipe, RELAY_LOG, "穿透超时 超时时间 %d 当前时间 %ld TaskBeginTime %ld \n",pAmrThreadArg->RelayTrans.OverTime,time(NULL),TaskBeginTime[pipe]);
                                    if(((time(NULL)-TaskBeginTime[pipe])>pAmrThreadArg->RelayTrans.OverTime)||(time(NULL)<TaskBeginTime[pipe]))
                                    {
                                        TASK_FMT_TRACE( pipe, RELAY_LOG, "穿透超时 任务结束 \n");
                                        //taskmng_relay_up_msg_write( pipe, -1);
                                        pAmrThreadArg->TotalState.RelayTrans = INNER_RTS_END;
                                        break;
                                    }
                                    TASK_FMT_TRACE( pipe, RELAY_LOG, "count is  %d\n",count);
                                    if(count<2)
                                    {
                                        count++;
                                        pAmrThreadArg->TotalState.RelayTrans = INNER_RTS_SEND;
                                    }
                                    else
                                    {
                                        pAmrThreadArg->TotalState.RelayTrans = INNER_RTS_END;
                                        count=0;
                                    }
                                    TASK_FMT_TRACE( pipe, RELAY_LOG, "pAmrThreadArg->TotalState.RelayTrans is  %d\n",pAmrThreadArg->TotalState.RelayTrans);

                                }

                            break;
                            case 0x50:
                                {
                                    TASK_FMT_TRACE( pipe, RELAY_LOG, "穿透超时 超时时间 %d 当前时间 %ld TaskBeginTime %ld \n",pAmrThreadArg->RelayTrans.OverTime,time(NULL),TaskBeginTime[pipe]);
                                    if(((time(NULL)-TaskBeginTime[pipe])>pAmrThreadArg->RelayTrans.OverTime)||(time(NULL)<TaskBeginTime[pipe]))
                                    {
                                        TASK_FMT_TRACE( pipe, RELAY_LOG, "穿透超时 任务结束 \n");
                                        //taskmng_relay_up_msg_write( pipe, -1);
                                        pAmrThreadArg->TotalState.RelayTrans = INNER_RTS_END;
                                        break;
                                    }
                                    TASK_FMT_TRACE( pipe, RELAY_LOG, "count is  %d\n",count);
                                    if(count<2)
                                    {
                                        count++;
                                        pAmrThreadArg->TotalState.RelayTrans = INNER_RTS_SEND;
                                    }
                                    else
                                    {
                                        pAmrThreadArg->TotalState.RelayTrans = INNER_RTS_END;
                                        count=0;
                                    }
                                    TASK_FMT_TRACE( pipe, RELAY_LOG, "pAmrThreadArg->TotalState.RelayTrans is  %d\n",pAmrThreadArg->TotalState.RelayTrans);

                                }

                            break;
                            default:
                                break;
                        }
                    }
                break;
            default:
                break;
            }

        }
        break;
        case INNER_RTS_END:
        {
            count=0;
            switch(pAmrThreadArg->RelayTrans.IOP)
            {

            case 0x0011:
            {

                switch(pAmrThreadArg->RelayTrans.proxychoice)
                {
                    case 0x01:
                    {
                        //上报数据
                        taskmng_proxy_get_response_list_up_msg_write(pipe);
                    }
                    break;
                    case 0x02:
                    {
                        //上报数据
                       if(RecvLen==0)
                       {
                            uint16 index=0;
                            uint8  tmpbuffer[MAX_MSG_LEN]={0};
                            uint8  Byteoffset=0;
                            uint8  i=0;
                            pAmrThreadArg->RelayTrans.RelayData[index++] = 0x89;
                            pAmrThreadArg->RelayTrans.RelayData[index++] = 0x02;
                            pAmrThreadArg->RelayTrans.RelayData[index++] = pAmrThreadArg->RelayTrans.piid;
                            pAmrThreadArg->RelayTrans.RelayData[index++]=0x07;//tsa长度
                            pAmrThreadArg->RelayTrans.RelayData[index++]=5;//
                            memcpy_r(&pAmrThreadArg->RelayTrans.RelayData[index],pAmrThreadArg->RelayTrans.MtrAddr,6);
                            index+=6;

                            memcpy_r(&pAmrThreadArg->RelayTrans.RelayData[index],(uint8*)&pAmrThreadArg->RelayTrans.Road.MainOad.value,4);
                            index+=4;
                            pAmrThreadArg->RelayTrans.RelayData[index++]= pAmrThreadArg->RelayTrans.Road.SubOadNum;
                            for(i=0;i<pAmrThreadArg->RelayTrans.Road.SubOadNum;i++)
                            {
                                pAmrThreadArg->RelayTrans.RelayData[index++]=0x00;
                                memcpy_r((uint8*)&pAmrThreadArg->RelayTrans.RelayData[index],(uint8*)&pAmrThreadArg->RelayTrans.Road.OadListInfo[i].value,4);
                                index = index+4;
                                if(i==pAmrThreadArg->RelayTrans.Road.SubOadNum-1)
                                {
                                    pAmrThreadArg->RelayTrans.RelayData[index++]=0x21;
                                }
                                else
                                {
                                    pAmrThreadArg->RelayTrans.RelayData[index++]=0x00;
                                }

                            }
                            pAmrThreadArg->RelayTrans.RelayData[index++]=0x00;//跟随上报信息域
                            pAmrThreadArg->RelayTrans.RelayData[index++]=0x00;//时间标签
                            if(index<0x80)
                            {
                                 tmpbuffer[Byteoffset++]=index;
                            }else
                            {
                                 tmpbuffer[Byteoffset++] = 0x82;
                                 tmpbuffer[Byteoffset++] = (uint8)(index>>8) ;
                                 tmpbuffer[Byteoffset++] = (uint8)index ;
                            }
                            memcpy(tmpbuffer+Byteoffset,pAmrThreadArg->RelayTrans.RelayData,index);
                            index=index+Byteoffset;
                            memcpy(pAmrThreadArg->RelayTrans.RelayData,tmpbuffer,index);
                            pAmrThreadArg->RelayTrans.RelayLen = index;

                            taskmng_relay_up_msg_write( pipe, 0);
                            pAmrThreadArg->RelayTrans.RelayResult =1;

                       }
                    }
                    break;
                    case 0x03:
                    {
                        //上报数据
                       uint8 i;
                       if((RecvLen==0)&&(pAmrThreadArg->RelayTrans.METER_LIST[meterindex].VliadMeter==1))
                       {


                             if(pAmrThreadArg->RelayTrans.AmrIndex==0)
                            {
                                pAmrThreadArg->RelayTrans.RelayData[index++] = 0x89;
                                pAmrThreadArg->RelayTrans.RelayData[index++] = pAmrThreadArg->RelayTrans.proxychoice;
                                pAmrThreadArg->RelayTrans.RelayData[index++] = pAmrThreadArg->RelayTrans.piid;
                                pAmrThreadArg->RelayTrans.RelayData[index++] = pAmrThreadArg->RelayTrans.TotalNum;
                            }

                            if(pAmrThreadArg->RelayTrans.AmrIndex<pAmrThreadArg->RelayTrans.TotalNum)
                            {
                                meterindex=pAmrThreadArg->RelayTrans.AmrIndex;
                                TASK_FMT_TRACE(pipe, RELAY_LOG,"pAmrThreadArg->RelayTrans.METER_LIST[meterindex].VliadMeter is %d\n",pAmrThreadArg->RelayTrans.METER_LIST[meterindex].VliadMeter);
                                if(pAmrThreadArg->RelayTrans.METER_LIST[meterindex].VliadMeter==0)
                                {
                                    pAmrThreadArg->RelayTrans.RelayData[index++]=0x07;//tsa长度
                                    pAmrThreadArg->RelayTrans.RelayData[index++]=5;//
                                    memcpy(&pAmrThreadArg->RelayTrans.RelayData[index],pAmrThreadArg->RelayTrans.METER_LIST[meterindex].Mtraddr.add,6);
                                    index+=6;
                                    pAmrThreadArg->RelayTrans.RelayData[index++] = pAmrThreadArg->RelayTrans.METER_LIST[meterindex].OadNum;
                                    for(i=0;i<pAmrThreadArg->RelayTrans.METER_LIST[meterindex].OadNum;i++)
                                    {
                                        memcpy_r(&pAmrThreadArg->RelayTrans.RelayData[index],&pAmrThreadArg->RelayTrans.METER_LIST[meterindex].SubOad[i],sizeof(OOP_OAD_U));
                                        index+=sizeof(OOP_OAD_U);
                                        pAmrThreadArg->RelayTrans.RelayData[index++]=0xFF;
                                    }
                                    pAmrThreadArg->RelayTrans.AmrIndex++;
                                    if(pAmrThreadArg->RelayTrans.AmrIndex>=pAmrThreadArg->RelayTrans.TotalNum)
                                    {
                                    }
                                    else
                                    {
                                        pAmrThreadArg->TotalState.RelayTrans = INNER_RTS_SEND;
                                        break;
                                    }
                                }
                                else
                                {
                                    pAmrThreadArg->RelayTrans.RelayData[index++]=0x07;//tsa长度
                                    pAmrThreadArg->RelayTrans.RelayData[index++]=5;//
                                    memcpy(&pAmrThreadArg->RelayTrans.RelayData[index],pAmrThreadArg->RelayTrans.METER_LIST[meterindex].Mtraddr.add,6);
                                    index+=6;
                                    pAmrThreadArg->RelayTrans.RelayData[index++] = pAmrThreadArg->RelayTrans.METER_LIST[meterindex].OadNum;
                                    for(i=0;i<pAmrThreadArg->RelayTrans.METER_LIST[meterindex].OadNum;i++)
                                    {
                                        memcpy_r(&pAmrThreadArg->RelayTrans.RelayData[index],&pAmrThreadArg->RelayTrans.METER_LIST[meterindex].SubOad[i],sizeof(OOP_OAD_U));
                                        index+=sizeof(OOP_OAD_U);
                                        pAmrThreadArg->RelayTrans.RelayData[index++]=0x21;
                                    }
                                    pAmrThreadArg->RelayTrans.AmrIndex++;
                                    if(pAmrThreadArg->RelayTrans.AmrIndex>=pAmrThreadArg->RelayTrans.TotalNum)
                                    {
                                    }
                                    else
                                    {
                                        pAmrThreadArg->TotalState.RelayTrans = INNER_RTS_SEND;
                                        break;
                                    }

                                }
                            }
                            pAmrThreadArg->RelayTrans.RelayData[index++]=0x00;//跟随上报信息域
                            pAmrThreadArg->RelayTrans.RelayData[index++]=0x00;//时间标签
                            if(index<0x80)
                            {
                                 tmpbuffer[Byteoffset++]=index;
                            }else
                            {
                                 tmpbuffer[Byteoffset++] = 0x82;
                                 tmpbuffer[Byteoffset++] = (uint8)(index>>8) ;
                                 tmpbuffer[Byteoffset++] = (uint8)index ;
                            }
                            memcpy(tmpbuffer+Byteoffset,pAmrThreadArg->RelayTrans.RelayData,index);
                            index=index+Byteoffset;
                            memcpy(pAmrThreadArg->RelayTrans.RelayData,tmpbuffer,index);
                            pAmrThreadArg->RelayTrans.RelayLen = index;

                            taskmng_relay_up_msg_write( pipe, 0);
                            pAmrThreadArg->RelayTrans.RelayResult =1;


                        }
                    }
                    break;
                    case 0x04:
                    {
                        //上报数据
                       if((RecvLen==0)&&(pAmrThreadArg->RelayTrans.METER_LIST[meterindex].VliadMeter==1))
                       {

                                uint8 mn=0;

                                if(pAmrThreadArg->RelayTrans.AmrIndex==0)
                                {
                                    pAmrThreadArg->RelayTrans.RelayData[index++] = 0x89;
                                    pAmrThreadArg->RelayTrans.RelayData[index++] = pAmrThreadArg->RelayTrans.proxychoice;
                                    pAmrThreadArg->RelayTrans.RelayData[index++] = pAmrThreadArg->RelayTrans.piid;
                                    pAmrThreadArg->RelayTrans.RelayData[index++] = pAmrThreadArg->RelayTrans.TotalNum;
                                }
                                TASK_FMT_TRACE(pipe, RELAY_LOG,"pAmrThreadArg->RelayTrans.AmrIndex is %d,pAmrThreadArg->RelayTrans.TotalNum is %d\n",pAmrThreadArg->RelayTrans.AmrIndex,pAmrThreadArg->RelayTrans.TotalNum);
                                if(pAmrThreadArg->RelayTrans.AmrIndex<pAmrThreadArg->RelayTrans.TotalNum)
                                {
                                    meterindex=pAmrThreadArg->RelayTrans.AmrIndex;
                                    TASK_FMT_TRACE(pipe, RELAY_LOG,"pAmrThreadArg->RelayTrans.METER_LIST[meterindex].VliadMeter is %d\n",pAmrThreadArg->RelayTrans.METER_LIST[meterindex].VliadMeter);
                                    if(pAmrThreadArg->RelayTrans.METER_LIST[meterindex].VliadMeter==0)
                                    {
                                        pAmrThreadArg->RelayTrans.RelayData[index++]=0x07;//tsa长度
                                        pAmrThreadArg->RelayTrans.RelayData[index++]=5;//
                                        memcpy(&pAmrThreadArg->RelayTrans.RelayData[index],pAmrThreadArg->RelayTrans.METER_LIST[meterindex].Mtraddr.add,6);
                                        index+=6;
                                        pAmrThreadArg->RelayTrans.RelayData[index++]=pAmrThreadArg->RelayTrans.METER_LIST[meterindex].OadNum;
                                        for(mn=0;mn<pAmrThreadArg->RelayTrans.METER_LIST[meterindex].OadNum;mn++)
                                        {

                                            memcpy_r(pAmrThreadArg->RelayTrans.RelayData+index,(uint8*)&pAmrThreadArg->RelayTrans.METER_LIST[meterindex].SetOad[mn].value,4);
                                            index=index+4;
                                            pAmrThreadArg->RelayTrans.RelayData[index++]=0x21;

                                            memcpy_r(pAmrThreadArg->RelayTrans.RelayData+index,(uint8*)&pAmrThreadArg->RelayTrans.METER_LIST[meterindex].GetOad[mn].value,4);
                                            index=index+4;
                                            pAmrThreadArg->RelayTrans.RelayData[index++]=0x00;
                                            pAmrThreadArg->RelayTrans.RelayData[index++]=0x21;

                                        }

                                        pAmrThreadArg->RelayTrans.AmrIndex++;
                                        if(pAmrThreadArg->RelayTrans.AmrIndex>=pAmrThreadArg->RelayTrans.TotalNum)
                                        {
                                        }
                                        else
                                        {
                                            pAmrThreadArg->TotalState.RelayTrans = INNER_RTS_SEND;
                                            break;
                                        }
                                    }
                                    else
                                    {
                                        pAmrThreadArg->RelayTrans.RelayData[index++]=0x07;//tsa长度
                                        pAmrThreadArg->RelayTrans.RelayData[index++]=5;//
                                        memcpy(&pAmrThreadArg->RelayTrans.RelayData[index],pAmrThreadArg->RelayTrans.METER_LIST[meterindex].Mtraddr.add,6);
                                        index+=6;
                                        pAmrThreadArg->RelayTrans.RelayData[index++]=pAmrThreadArg->RelayTrans.METER_LIST[meterindex].OadNum;
                                        for(mn=0;mn<pAmrThreadArg->RelayTrans.METER_LIST[meterindex].OadNum;mn++)
                                        {

                                            memcpy_r(pAmrThreadArg->RelayTrans.RelayData+index,(uint8*)&pAmrThreadArg->RelayTrans.METER_LIST[meterindex].SetOad[mn].value,4);
                                            index=index+4;
                                            pAmrThreadArg->RelayTrans.RelayData[index++]=0x21;

                                            memcpy_r(pAmrThreadArg->RelayTrans.RelayData+index,(uint8*)&pAmrThreadArg->RelayTrans.METER_LIST[meterindex].GetOad[mn].value,4);
                                            index=index+4;
                                            pAmrThreadArg->RelayTrans.RelayData[index++]=0x00;
                                            pAmrThreadArg->RelayTrans.RelayData[index++]=0x21;

                                        }

                                        pAmrThreadArg->RelayTrans.AmrIndex++;
                                        if(pAmrThreadArg->RelayTrans.AmrIndex>=pAmrThreadArg->RelayTrans.TotalNum)
                                        {
                                        }
                                        else
                                        {
                                            pAmrThreadArg->TotalState.RelayTrans = INNER_RTS_SEND;
                                            break;
                                        }

                                    }
                                }
                                pAmrThreadArg->RelayTrans.RelayData[index++]=0x00;//跟随上报信息域
                                pAmrThreadArg->RelayTrans.RelayData[index++]=0x00;//时间标签
                                if(index<0x80)
                                {
                                     tmpbuffer[Byteoffset++]=index;
                                }else
                                {
                                     tmpbuffer[Byteoffset++] = 0x82;
                                     tmpbuffer[Byteoffset++] = (uint8)(index>>8) ;
                                     tmpbuffer[Byteoffset++] = (uint8)index ;
                                }
                                memcpy(tmpbuffer+Byteoffset,pAmrThreadArg->RelayTrans.RelayData,index);
                                index=index+Byteoffset;
                                memcpy(pAmrThreadArg->RelayTrans.RelayData,tmpbuffer,index);
                                pAmrThreadArg->RelayTrans.RelayLen = index;

                                taskmng_relay_up_msg_write( pipe, 0);
                                pAmrThreadArg->RelayTrans.RelayResult =1;



                        }
                    }
                    break;
                    case 0x05:
                    {
                               //上报数据
                            uint8 i;
                            if((RecvLen==0)&&(pAmrThreadArg->RelayTrans.METER_LIST[meterindex].VliadMeter==1))
                            {
                                TASK_FMT_TRACE(pipe, RELAY_LOG,"0x0011 00 05 未收到数据\n");
                                if(pAmrThreadArg->RelayTrans.AmrIndex==0)
                                {
                                    pAmrThreadArg->RelayTrans.RelayData[index++] = 0x89;
                                    pAmrThreadArg->RelayTrans.RelayData[index++] = pAmrThreadArg->RelayTrans.proxychoice;
                                    pAmrThreadArg->RelayTrans.RelayData[index++] = pAmrThreadArg->RelayTrans.piid;
                                    pAmrThreadArg->RelayTrans.RelayData[index++] = pAmrThreadArg->RelayTrans.TotalNum;
                                }
                                TASK_FMT_TRACE(pipe, RELAY_LOG,"pAmrThreadArg->RelayTrans.AmrIndex is %d,pAmrThreadArg->RelayTrans.TotalNum is %d\n",pAmrThreadArg->RelayTrans.AmrIndex,pAmrThreadArg->RelayTrans.TotalNum);
                                if(pAmrThreadArg->RelayTrans.AmrIndex<pAmrThreadArg->RelayTrans.TotalNum)
                                {
                                    meterindex=pAmrThreadArg->RelayTrans.AmrIndex;
                                    TASK_FMT_TRACE(pipe, RELAY_LOG,"pAmrThreadArg->RelayTrans.METER_LIST[meterindex].VliadMeter is %d\n",pAmrThreadArg->RelayTrans.METER_LIST[meterindex].VliadMeter);
                                    if(pAmrThreadArg->RelayTrans.METER_LIST[meterindex].VliadMeter==0)
                                    {
                                        pAmrThreadArg->RelayTrans.RelayData[index++]=0x07;//tsa长度
                                        pAmrThreadArg->RelayTrans.RelayData[index++]=5;//
                                        memcpy(&pAmrThreadArg->RelayTrans.RelayData[index],pAmrThreadArg->RelayTrans.METER_LIST[meterindex].Mtraddr.add,6);
                                        index+=6;
                                        pAmrThreadArg->RelayTrans.RelayData[index++]=pAmrThreadArg->RelayTrans.METER_LIST[meterindex].OadNum;
                                        for(i=0;i<pAmrThreadArg->RelayTrans.METER_LIST[meterindex].OadNum;i++)
                                        {
                                            memcpy_r(pAmrThreadArg->RelayTrans.RelayData+index,(uint8*)&pAmrThreadArg->RelayTrans.METER_LIST[meterindex].SetOad[i].value,4);
                                            index=index+4;
                                            pAmrThreadArg->RelayTrans.RelayData[index++]=0xFF;
                                            pAmrThreadArg->RelayTrans.RelayData[index++]=0x00;
                                        }
                                        pAmrThreadArg->RelayTrans.AmrIndex++;
                                        if(pAmrThreadArg->RelayTrans.AmrIndex>=pAmrThreadArg->RelayTrans.TotalNum)
                                        {
                                        }
                                        else
                                        {
                                            pAmrThreadArg->TotalState.RelayTrans = INNER_RTS_SEND;
                                            break;
                                        }
                                    }
                                    else
                                    {
                                        pAmrThreadArg->RelayTrans.RelayData[index++]=0x07;//tsa长度
                                        pAmrThreadArg->RelayTrans.RelayData[index++]=5;//
                                        memcpy(&pAmrThreadArg->RelayTrans.RelayData[index],pAmrThreadArg->RelayTrans.METER_LIST[meterindex].Mtraddr.add,6);
                                        index+=6;
                                        pAmrThreadArg->RelayTrans.RelayData[index++]=pAmrThreadArg->RelayTrans.METER_LIST[meterindex].OadNum;
                                        for(i=0;i<pAmrThreadArg->RelayTrans.METER_LIST[meterindex].OadNum;i++)
                                        {
                                            memcpy_r(pAmrThreadArg->RelayTrans.RelayData+index,(uint8*)&pAmrThreadArg->RelayTrans.METER_LIST[meterindex].SetOad[i].value,4);
                                            index=index+4;
                                            pAmrThreadArg->RelayTrans.RelayData[index++]=0x21;
                                            pAmrThreadArg->RelayTrans.RelayData[index++]=0x00;
                                        }
                                        pAmrThreadArg->RelayTrans.AmrIndex++;
                                        if(pAmrThreadArg->RelayTrans.AmrIndex>=pAmrThreadArg->RelayTrans.TotalNum)
                                        {
                                        }
                                        else
                                        {
                                            pAmrThreadArg->TotalState.RelayTrans = INNER_RTS_SEND;
                                            break;
                                        }

                                    }
                                }
                                pAmrThreadArg->RelayTrans.RelayData[index++]=0x00;//跟随上报信息域
                                pAmrThreadArg->RelayTrans.RelayData[index++]=0x00;//时间标签
                                if(index<0x80)
                                {
                                     tmpbuffer[Byteoffset++]=index;
                                }else
                                {
                                     tmpbuffer[Byteoffset++] = 0x82;
                                     tmpbuffer[Byteoffset++] = (uint8)(index>>8) ;
                                     tmpbuffer[Byteoffset++] = (uint8)index ;
                                }
                                memcpy(tmpbuffer+Byteoffset,pAmrThreadArg->RelayTrans.RelayData,index);
                                index=index+Byteoffset;
                                memcpy(pAmrThreadArg->RelayTrans.RelayData,tmpbuffer,index);
                                pAmrThreadArg->RelayTrans.RelayLen = index;

                                taskmng_relay_up_msg_write( pipe, 0);
                                pAmrThreadArg->RelayTrans.RelayResult =1;

                       }
                    }
                        break;
                    case 0x06:
                    {
                        //上报数据
                       if((RecvLen==0)&&(pAmrThreadArg->RelayTrans.METER_LIST[meterindex].VliadMeter==1))
                       {


                            uint8 mn;

                            if(pAmrThreadArg->RelayTrans.AmrIndex==0)
                            {
                                pAmrThreadArg->RelayTrans.RelayData[index++] = 0x89;
                                pAmrThreadArg->RelayTrans.RelayData[index++] = pAmrThreadArg->RelayTrans.proxychoice;
                                pAmrThreadArg->RelayTrans.RelayData[index++] = pAmrThreadArg->RelayTrans.piid;
                                pAmrThreadArg->RelayTrans.RelayData[index++] = pAmrThreadArg->RelayTrans.TotalNum;
                            }
                            TASK_FMT_TRACE(pipe, RELAY_LOG,"pAmrThreadArg->RelayTrans.AmrIndex is %d,pAmrThreadArg->RelayTrans.TotalNum is %d\n",pAmrThreadArg->RelayTrans.AmrIndex,pAmrThreadArg->RelayTrans.TotalNum);
                            if(pAmrThreadArg->RelayTrans.AmrIndex<pAmrThreadArg->RelayTrans.TotalNum)
                            {
                                meterindex=pAmrThreadArg->RelayTrans.AmrIndex;
                                TASK_FMT_TRACE(pipe, RELAY_LOG,"pAmrThreadArg->RelayTrans.METER_LIST[meterindex].VliadMeter is %d\n",pAmrThreadArg->RelayTrans.METER_LIST[meterindex].VliadMeter);
                                if(pAmrThreadArg->RelayTrans.METER_LIST[meterindex].VliadMeter==0)
                                {
                                    pAmrThreadArg->RelayTrans.RelayData[index++]=0x07;//tsa长度
                                    pAmrThreadArg->RelayTrans.RelayData[index++]=5;//
                                    memcpy(&pAmrThreadArg->RelayTrans.RelayData[index],pAmrThreadArg->RelayTrans.METER_LIST[meterindex].Mtraddr.add,6);
                                    index+=6;
                                    pAmrThreadArg->RelayTrans.RelayData[index++]=pAmrThreadArg->RelayTrans.METER_LIST[meterindex].OadNum;
                                        for(mn=0;mn<pAmrThreadArg->RelayTrans.METER_LIST[meterindex].OadNum;mn++)
                                        {

                                            memcpy_r(pAmrThreadArg->RelayTrans.RelayData+index,(uint8*)&pAmrThreadArg->RelayTrans.METER_LIST[meterindex].SetOad[mn].value,4);
                                            index=index+4;
                                            pAmrThreadArg->RelayTrans.RelayData[index++]=0x21;
                                            pAmrThreadArg->RelayTrans.RelayData[index++]=0x00;
                                            memcpy_r(pAmrThreadArg->RelayTrans.RelayData+index,(uint8*)&pAmrThreadArg->RelayTrans.METER_LIST[meterindex].GetOad[mn].value,4);
                                            index=index+4;
                                            pAmrThreadArg->RelayTrans.RelayData[index++]=0x00;
                                            pAmrThreadArg->RelayTrans.RelayData[index++]=0x21;

                                        }
                                    pAmrThreadArg->RelayTrans.AmrIndex++;
                                    if(pAmrThreadArg->RelayTrans.AmrIndex>=pAmrThreadArg->RelayTrans.TotalNum)
                                    {
                                    }
                                    else
                                    {
                                        pAmrThreadArg->TotalState.RelayTrans = INNER_RTS_SEND;
                                        break;
                                    }
                                }
                                else
                                {
                                    pAmrThreadArg->RelayTrans.RelayData[index++]=0x07;//tsa长度
                                    pAmrThreadArg->RelayTrans.RelayData[index++]=5;//
                                    memcpy(&pAmrThreadArg->RelayTrans.RelayData[index],pAmrThreadArg->RelayTrans.METER_LIST[meterindex].Mtraddr.add,6);
                                    index+=6;
                                    pAmrThreadArg->RelayTrans.RelayData[index++]=pAmrThreadArg->RelayTrans.METER_LIST[meterindex].OadNum;
                                    for(mn=0;mn<pAmrThreadArg->RelayTrans.METER_LIST[meterindex].OadNum;mn++)
                                    {

                                        memcpy_r(pAmrThreadArg->RelayTrans.RelayData+index,(uint8*)&pAmrThreadArg->RelayTrans.METER_LIST[meterindex].SetOad[mn].value,4);
                                        index=index+4;
                                        pAmrThreadArg->RelayTrans.RelayData[index++]=0x21;
                                        pAmrThreadArg->RelayTrans.RelayData[index++]=0x00;
                                        memcpy_r(pAmrThreadArg->RelayTrans.RelayData+index,(uint8*)&pAmrThreadArg->RelayTrans.METER_LIST[meterindex].GetOad[mn].value,4);
                                        index=index+4;
                                        pAmrThreadArg->RelayTrans.RelayData[index++]=0x00;
                                        pAmrThreadArg->RelayTrans.RelayData[index++]=0x21;

                                    }
                                    pAmrThreadArg->RelayTrans.AmrIndex++;
                                    if(pAmrThreadArg->RelayTrans.AmrIndex>=pAmrThreadArg->RelayTrans.TotalNum)
                                    {
                                    }
                                    else
                                    {
                                        pAmrThreadArg->TotalState.RelayTrans = INNER_RTS_SEND;
                                        break;
                                    }

                                }
                            }
                            pAmrThreadArg->RelayTrans.RelayData[index++]=0x00;//跟随上报信息域
                            pAmrThreadArg->RelayTrans.RelayData[index++]=0x00;//时间标签
                            if(index<0x80)
                            {
                                 tmpbuffer[Byteoffset++]=index;
                            }else
                            {
                                 tmpbuffer[Byteoffset++] = 0x82;
                                 tmpbuffer[Byteoffset++] = (uint8)(index>>8) ;
                                 tmpbuffer[Byteoffset++] = (uint8)index ;
                            }
                            memcpy(tmpbuffer+Byteoffset,pAmrThreadArg->RelayTrans.RelayData,index);
                            index=index+Byteoffset;
                            memcpy(pAmrThreadArg->RelayTrans.RelayData,tmpbuffer,index);
                            pAmrThreadArg->RelayTrans.RelayLen = index;

                            taskmng_relay_up_msg_write( pipe, 0);
                            pAmrThreadArg->RelayTrans.RelayResult =1;




                        }
                    }
                        break;
                    case 0x07:
                        {
                            OOP_OAD_U oad;
                            uint8 tmppipe=0;
                            oad.value = pAmrThreadArg->RelayTrans.OadPort;
                            tmppipe = taskmng_mtr_pipe_get(&oad);
                            TASK_FMT_TRACE( pipe, RELAY_LOG, "穿透收到数据 任务结束 tmppipe is %d,oad.value is %x\n",tmppipe,oad.value);
                            if(tmppipe !=pipe)
                            {
                                return -1;
                            }
                            if(RecvLen==0)
                            {
                                pAmrThreadArg->RelayTrans.RelayData[index++] = 0x89;
                                pAmrThreadArg->RelayTrans.RelayData[index++] = pAmrThreadArg->RelayTrans.proxychoice;
                                pAmrThreadArg->RelayTrans.RelayData[index++] = pAmrThreadArg->RelayTrans.piid;
                                //端口OAD
                                memcpy_r((uint8*)&pAmrThreadArg->RelayTrans.RelayData[index],(uint8*)&pAmrThreadArg->RelayTrans.OadPort, 4);
                                index =index +4;
                                //结果无数据，返回超时错误码
                                pAmrThreadArg->RelayTrans.RelayData[index++] = 0x00;
                                pAmrThreadArg->RelayTrans.RelayData[index++] = 0x21;
                                pAmrThreadArg->RelayTrans.RelayData[index++]=0x00;//跟随上报信息域
                                pAmrThreadArg->RelayTrans.RelayData[index++]=0x00;//时间标签
                                if(index<0x80)
                                {
                                     tmpbuffer[Byteoffset++]=index;
                                }else
                                {
                                     tmpbuffer[Byteoffset++] = 0x82;
                                     tmpbuffer[Byteoffset++] = (uint8)(index>>8) ;
                                     tmpbuffer[Byteoffset++] = (uint8)index ;
                                }
                                memcpy(tmpbuffer+Byteoffset,pAmrThreadArg->RelayTrans.RelayData,index);
                                index=index+Byteoffset;
                                memcpy(pAmrThreadArg->RelayTrans.RelayData,tmpbuffer,index);
                                pAmrThreadArg->RelayTrans.RelayLen = index;
                                TASK_FMT_TRACE( pipe, RELAY_LOG, "穿透收到数据 任务结束 \n");
                                taskmng_relay_up_msg_write( pipe, 0);
                                pAmrThreadArg->RelayTrans.RelayResult =1;
                            }
                        }
                    break;
                    case 0x50:
                        {
                            OOP_OAD_U oad;
                            uint8 tmppipe=0;
                            oad.value = pAmrThreadArg->RelayTrans.OadPort;
                            tmppipe = taskmng_mtr_pipe_get(&oad);
                            TASK_FMT_TRACE( pipe, RELAY_LOG, "穿透收到数据 任务结束 tmppipe is %d,oad.value is %x\n",tmppipe,oad.value);
                            if(tmppipe !=pipe)
                            {
                                return -1;
                            }
                            if(RecvLen==0)
                            {
                                pAmrThreadArg->RelayTrans.RelayData[index++] = 0x89;
                                pAmrThreadArg->RelayTrans.RelayData[index++] = pAmrThreadArg->RelayTrans.proxychoice;
                                pAmrThreadArg->RelayTrans.RelayData[index++] = pAmrThreadArg->RelayTrans.piid;
                                //端口OAD
                                memcpy_r((uint8*)&pAmrThreadArg->RelayTrans.RelayData[index],(uint8*)&pAmrThreadArg->RelayTrans.OadPort, 4);
                                index =index +4;
                                //通信地址
                                pAmrThreadArg->RelayTrans.RelayData[index++] = pAmrThreadArg->RelayTrans.tsa.af;
                                memcpy((uint8*)&pAmrThreadArg->RelayTrans.RelayData[index],pAmrThreadArg->RelayTrans.tsa.add,pAmrThreadArg->RelayTrans.tsa.len+1);
                                index = index+pAmrThreadArg->RelayTrans.tsa.len+1;
                                //通信对象类型
                                pAmrThreadArg->RelayTrans.RelayData[index++] = pAmrThreadArg->RelayTrans.objiecttype;
                                //响应报文格式字
                                pAmrThreadArg->RelayTrans.RelayData[index++] = pAmrThreadArg->RelayTrans.resframeformat;
                                //结果无数据，返回超时错误码
                                pAmrThreadArg->RelayTrans.RelayData[index++] = 0x00;
                                pAmrThreadArg->RelayTrans.RelayData[index++] = 0x21;
                                pAmrThreadArg->RelayTrans.RelayData[index++]=0x00;//跟随上报信息域
                                pAmrThreadArg->RelayTrans.RelayData[index++]=0x00;//时间标签
                                if(index<0x80)
                                {
                                     tmpbuffer[Byteoffset++]=index;
                                }else
                                {
                                     tmpbuffer[Byteoffset++] = 0x82;
                                     tmpbuffer[Byteoffset++] = (uint8)(index>>8) ;
                                     tmpbuffer[Byteoffset++] = (uint8)index ;
                                }
                                memcpy(tmpbuffer+Byteoffset,pAmrThreadArg->RelayTrans.RelayData,index);
                                index=index+Byteoffset;
                                memcpy(pAmrThreadArg->RelayTrans.RelayData,tmpbuffer,index);
                                pAmrThreadArg->RelayTrans.RelayLen = index;
                                TASK_FMT_TRACE( pipe, RELAY_LOG, "穿透收到数据 任务结束 \n");
                                taskmng_relay_up_msg_write( pipe, 0);
                                pAmrThreadArg->RelayTrans.RelayResult =1;
                            }
                        }
                    break;
                    default:
                        break;
                }
            }
                break;

            default:
                break;
            }
//            close(fd232);
            //rs485_RS232_UnInit(&Rs485ComNo[pipe-2]);
            pAmrThreadArg->RelayTrans.isValid = 0;
            memset(&pAmrThreadArg->RelayTrans,0x00,sizeof(RELAY_TRANS_T));
            pAmrThreadArg->TotalState.RelayTrans = INNER_RTS_IDLE;
            return 1;
        }
            break;
        default:
            break;
        }
    }
    return -1;
}


/**
*********************************************************************
* @brief:      实现  485抄表功能:包括抄表任务、方案的获取,以及抄读功能等；
* @param[in]：   pipe  线程号,485模块共8个线程,485的线程pipe从2开始到9
* @param[out]： SCHEME_TASK_RUN_INFO *CurTaskRunInfo  获取要抄读的任务信息
* @param[out]：  CTS_RUN_TASK_T **TaskInfo  获取方案抄读过程中的运行信息
* @return：1表示有周期任务要执行，0表示没有
*********************************************************************
*/


uint8 GetHighPriorityTask(uint8 pipe,CTS_RUN_TASK_T **TaskInfo,SCHEME_TASK_RUN_INFO *CurTaskRunInfo)
{
    int ret;
    taskmng_task_run_ergodic(pipe);

    ret=taskmng_task_runlist_check(pipe,TaskInfo);

    if(ret < 0)
    {
        return 0; //没有抄表任务需要执行
    }
    else
    {
        taskmng_task_info_get((*TaskInfo)->TaskId,(*TaskInfo)->logicid,CurTaskRunInfo);
        return 1;
    }
}
/*******************************************************************************
* 函数名称: taskmng_record_file_delete_pipe
* 函数功能:收到任务管理线程发送的任务变更消息，删除任务记录文件
* 输入参数:pipe  端口
        TaskId 任务id
* 输出参数:
* 返 回 值:
*******************************************************************************/
void taskmng_rs485_patch_file_delete(uint8 pipe,uint16 TaskId)
{
    AmrThreadArg_T  *pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    uint16 i=0,j;
    char  pfile[100];
    int ret;
    OopCrvAddReadTime_t OopCrvAddReadTime[OOP_CRV_ADDREAD_TASKNUM];
    OopCrvAddReadTime_t OopDayAddReadTime[OOP_DAY_ADDREAD_TASKNUM];

    memset(OopCrvAddReadTime,0,sizeof(OopCrvAddReadTime));
    memset(OopDayAddReadTime,0,sizeof(OopCrvAddReadTime));
    for(i=0;i<pAmrThreadArg->TmnAmrInfo.MtrCnt;i++)
    {
         sprintf(pfile,"%s/%s_%02d_MtrNo%02d",PrivateDir,PATCH_FILE,1,pAmrThreadArg->pMtrAmrInfo[i].Pn);
         ret=read_pdata_xram(pfile,(char *)&OopDayAddReadTime,0,sizeof(OopCrvAddReadTime_t)*OOP_DAY_ADDREAD_TASKNUM);
         if(ret >= 0)
         {
             for(j = 0; j < OOP_CRV_ADDREAD_TASKNUM; j++)
             {
                if(OopDayAddReadTime[j].TaskId == TaskId)
                {
                    OopDayAddReadTime[j].AddReadTime = 0;
                    write_pdata_xram(pfile,(char *)&OopDayAddReadTime,0,sizeof(OopCrvAddReadTime_t)*OOP_CRV_ADDREAD_TASKNUM);
                    TASK_FMT_TRACE( pipe, REC_LOG, "OopCrvAddReadTime[i].AddReadTime %ld \n",OopDayAddReadTime[i].AddReadTime);
                }
             }
         }

         sprintf(pfile,"%s/%s_%02d_MtrNo%02d",PrivateDir,PATCH_FILE,2,pAmrThreadArg->pMtrAmrInfo[i].Pn);
         ret=read_pdata_xram(pfile,(char *)&OopCrvAddReadTime,0,sizeof(OopCrvAddReadTime_t)*OOP_DAY_ADDREAD_TASKNUM);
         if(ret >= 0)
         {
             for(j = 0; j < OOP_DAY_ADDREAD_TASKNUM; j++)
             {
                if(OopCrvAddReadTime[j].TaskId == TaskId)
                {
                    OopCrvAddReadTime[j].AddReadTime = 0;
                    write_pdata_xram(pfile,(char *)&OopCrvAddReadTime,0,sizeof(OopCrvAddReadTime_t)*OOP_DAY_ADDREAD_TASKNUM);
                    TASK_FMT_TRACE( pipe, REC_LOG, "OopDayAddReadTime[i].AddReadTime %ld \n",OopCrvAddReadTime[i].AddReadTime);

                }
             }
         }

    }
    //write_pdata_xram(pfile,(char *)&OopDayAddReadTime,0,sizeof(OopCrvAddReadTime_t)*OOP_DAY_ADDREAD_TASKNUM);
    //write_pdata_xram(pfile,(char *)&OopCrvAddReadTime,0,sizeof(OopCrvAddReadTime_t)*OOP_CRV_ADDREAD_TASKNUM);
}

/*******************************************************************************
* 函数名称: taskmng_data_reading_encrypt
* 函数功能://转加密任务抄读
* 输入参数:pipe 端口号
        TaskInfo 任务参数
        RunTask 任务运行时参数
* 输出参数:
* 返 回 值:1 全部抄完 -1 未抄完
*******************************************************************************/

int taskmng_data_reading_encrypt_485(uint8 pipe, SCHEME_TASK_RUN_INFO * TaskInfo, CTS_RUN_TASK_T * RunTask)
{
    AmrThreadArg_T * pAmrThreadArg;

    pAmrThreadArg = &gAmrThreadArg[pipe];
    static time_t   sSendTime[AMR_THREAD_NUM];      //每一帧发送的时间
    uint16          overtime = 6;                   //转加密任务用透传 485超时时间设为6秒

    int             ret;
    int             RecvLen = 0;
    uint8           RecvBuf[MAX_MSG_LEN] =
    {
        0
    };
    if(gEncryptPipe != -1 && gEncryptPipe != pipe)
    {
        TASK_FMT_TRACE( pipe, REC_LOG, "当前端口 %d 在执行转加密任务\n",gEncryptPipe);
        return 1;
    }else
    {
        gEncryptPipe = pipe;
        TASK_FMT_TRACE(pipe, REC_LOG, "gEncryptPipe %d\n",gEncryptPipe);
    }

    uint16          pn = TaskInfo->MsInfo[RunTask->MeterIndex].Sn; //转加密任务一个表一个表执行 用MeterIndex记录当前执行的表

    while(1)
    {
        switch(pAmrThreadArg->TotalState.NormalAmr)
        {
            case RTS_IDLE:
                {
                    taskmng_task_record_file_load(pipe, TaskInfo, RunTask);
                    TASK_FMT_TRACE(pipe, REC_LOG, "taskmng_data_reading_encrypt 转加密任务\n");
                    pAmrThreadArg->TotalState.NormalAmr = RTS_SEND;
                }
                break;

            case RTS_SEND:
                {
                    ret = taskmng_encrypt_send(pipe, TaskInfo, RunTask);
                    if( ret == 1)
                    {
                        TASK_FMT_TRACE(pipe, REC_LOG, "全部抄完 转加密方案任务%d \n", TaskInfo->TaskId);
                        pAmrThreadArg->TotalState.NormalAmr = RTS_IDLE;
                        RunTask->RunResult = SCHEME_FINISHED;
                        if(gEncryptPipe == pipe )
                        {
                            gEncryptPipe = -1;
                        }
                        return 1;
                    }
                    else if(ret == 2)
                    {
                        break;
                    }else
                    {
                        sSendTime[pipe] = time(NULL);
                    }

                    pAmrThreadArg->TotalState.NormalAmr = RTS_RECV;
                }
                break;

            case RTS_RECV:
                {
                    RecvLen = rs485_RS232_recv(pipe, RecvBuf, sizeof(RecvBuf));

                    if(RecvLen > 0)
                    {
                        TASK_FMT_TRACE(pipe, REC_LOG, "485收到消息\n");
                        TASK_BUF_TRACE(pipe, REC_LOG, RecvBuf, RecvLen);
                        ret = taskmng_encrypt_recv(pipe, RecvBuf, RecvLen, TaskInfo, RunTask);

                        if(ret < 0)
                        {
                            pAmrThreadArg->TotalState.NormalAmr = RTS_RETRY;
                            break;
                        }

                        pAmrThreadArg->TotalState.NormalAmr = RTS_SEND;

                    }
                    else
                    {
                        if(abs(time(NULL) -sSendTime[pipe]) > overtime)
                        {
                            TASK_FMT_TRACE(pipe, REC_LOG, "485接收超时\n");

                            pAmrThreadArg->TotalState.NormalAmr = RTS_RETRY;
                        }
                    }

                }
                break;

            case RTS_RETRY:
                {
                    RunTask->encryptFailCnt++;
                    pn = TaskInfo->MsInfo[RunTask->MeterIndex].Sn;
                    TASK_FMT_TRACE(pipe, REC_LOG, "电表 %d 失败次数 %d\n", pn, RunTask->encryptFailCnt);
                    pAmrThreadArg->TotalState.NormalAmr = RTS_SEND;

                }
                break;

            default:
                pAmrThreadArg->TotalState.NormalAmr = RTS_IDLE;
                break;
        }
    }

    return - 1;

}

/*******************************************************************************
* 函数名称: taskmng_file_transfer_deal_485
* 函数功能:485电表升级
* 输入参数:pipe 端口号

* 输出参数:
* 返 回 值:
*******************************************************************************/
int taskmng_file_transfer_deal_485(uint8 pipe)
{
    AmrThreadArg_T * pAmrThreadArg;

    pAmrThreadArg = &gAmrThreadArg[pipe];
    UPDATE_INFO_T * updateinfo = &pAmrThreadArg->UpdateInfo;
    FILE_UPDATE_ENCRYPT_INFO_T *encryptinfo = &pAmrThreadArg->UpdateEncryptInfo;
    int             ret;
    static time_t   BeginTime[AMR_THREAD_NUM];
    uint8           RecvBuf[MAX_MSG_LEN] =
    {
        0
    };
    int             RecvLen;
    while(1)
    {
        switch(pAmrThreadArg->TotalState.FileTransfer)
        {
            case INNER_RTS_IDLE:
                {
                    memset(encryptinfo,0x00,sizeof(FILE_UPDATE_ENCRYPT_INFO_T));
                    //如果第一个地址是组地址 则要用广播发 每一帧限定长度
                    //485不需要限制
//                    if(updateinfo->MeterInfo[0].AddrFlag == ADDR_GROUP)
//                    {
//                        if(updateinfo->FileInfo.nBlockSize > 200)
//                            updateinfo->FileInfo.nBlockSize = 200;
//                    }
                    //计算需要发送多少帧
                    uint16 tmp = updateinfo->FileInfo.file.nSize%updateinfo->FileInfo.nBlockSize;
                    if(tmp>0)
                    {
                        updateinfo->FrameCount = updateinfo->FileInfo.file.nSize/updateinfo->FileInfo.nBlockSize+1;
                    }else
                    {
                        updateinfo->FrameCount = updateinfo->FileInfo.file.nSize/updateinfo->FileInfo.nBlockSize;
                    }
                    TASK_FMT_DEBUG( pipe, RELAY_LOG, "文件大小 %d 每帧 %d 总帧数%d \n",updateinfo->FileInfo.file.nSize,updateinfo->FileInfo.nBlockSize,updateinfo->FrameCount);
                    pAmrThreadArg->TotalState.FileTransfer = INNER_RTS_SEND;
                    updateinfo->FileState = FILE_INIT;

                }
                break;

            case INNER_RTS_SEND:
                {
                    ret = taskmng_file_transfer_send(pipe);

                    if(ret == 1)
                    {
                        BeginTime[pipe] = time(NULL);
                        pAmrThreadArg->TotalState.FileTransfer = INNER_RTS_RECV;

                    }
                    else if(ret == 0)
                    {
                        pAmrThreadArg->TotalState.FileTransfer = INNER_RTS_IDLE;
                        pAmrThreadArg->IsUpdating = 0;
                        return 1;
                    }
                }
                break;

            case INNER_RTS_RECV:
                {
                    RecvLen =rs485_RS232_recv(pipe,RecvBuf,sizeof(RecvBuf));

                    if(RecvLen > 0)
                    {

                        TASK_FMT_DEBUG(pipe, RELAY_LOG, "文件代发收到消息  \n");

                        TASK_BUF_DEBUG(pipe, RELAY_LOG,RecvBuf, RecvLen);
                        ret = taskmng_file_transfer_recv(pipe, RecvBuf, RecvLen);

                        if(ret == 0)
                            pAmrThreadArg->TotalState.FileTransfer = INNER_RTS_SEND;
                        else
                            pAmrThreadArg->TotalState.FileTransfer = INNER_RTS_RETRY;
                    }
                    else
                    {
                        if(updateinfo->isBroad == 1)
                        {
                            //广播不需要回码 手动给结果赋值
                            RecvLen = 1;
                            RecvBuf[0] = 1;
                            ret = taskmng_file_transfer_recv(pipe, RecvBuf, RecvLen);
                            if(ret == 0)
                                pAmrThreadArg->TotalState.FileTransfer = INNER_RTS_SEND;
                            else
                                pAmrThreadArg->TotalState.FileTransfer = INNER_RTS_RETRY;
                            break;
                        }
                        if(time(NULL) -BeginTime[pipe] > 6)
                        {
                            TASK_FMT_DEBUG(pipe, RELAY_LOG, "电表索引 %d 接收超时 \n",
                                 updateinfo->MeterIndex);

                            pAmrThreadArg->TotalState.FileTransfer = INNER_RTS_RETRY;
                        }
                    }

                }
                break;

            case INNER_RTS_RETRY:
                {
                    if(updateinfo->FileState >= FILE_ENCRYPT_INIT )
                    {
                        encryptinfo->failcnt ++;
                        TASK_FMT_DEBUG( pipe, RELAY_LOG, "转加密文件升级 失败次数 %d \n",encryptinfo->failcnt);
                    }else
                    {
                        updateinfo->MeterInfo[updateinfo->MeterIndex].FainCnt++;
                        TASK_FMT_DEBUG( pipe, RELAY_LOG, "电表索引 %d    失败次数 %d \n",updateinfo->MeterIndex,updateinfo->MeterInfo[updateinfo->MeterIndex].FainCnt);
                    }
                    pAmrThreadArg->TotalState.FileTransfer = INNER_RTS_SEND;

                }
                break;

            default:
                break;
        }
    }

    return - 1;
}

/*******************************************************************************
* 函数名称: taskmng_task_run_info_reset_485
* 函数功能: 485特殊处理函数
* 输入参数: TaskInfo 任务运行时参数

* 输出参数: 无
* 返 回 值:
*******************************************************************************/
void taskmng_task_run_info_reset_485(CTS_RUN_TASK_T *TaskInfo,SCHEME_TASK_RUN_INFO *TaskRunInfo)
{
    DateTimeBcd_t tmpTime;
    uint8        tmpmin;
    OOP_TI_T          TI;
    //485特殊处理 如果上一轮没抄完 下一轮直接从没抄到的开始抄 五分钟任务
    if(TaskInfo->NeedMtrNum>350 && TaskInfo->SucNum<TaskInfo->NeedMtrNum &&TaskInfo->MeterIndex < TaskInfo->TotalMtrNum && TaskRunInfo->FiveMinTaskFlag ==1)
    {
        TASK_FMT_DEBUG( 2, REC_LOG, "电表索引 %d \n",TaskInfo->MeterIndex);
    }else
    {
        TaskInfo->MeterIndex =0;
    }
    TaskInfo->SucNum = 0;
    TaskInfo->MinFrozenFailCnt = 0;
    TaskInfo->DayFrozenFailCnt = 0;
    TaskInfo->MonFrozenFailCnt = 0;

    TaskInfo->CsdIndex=0;
    TaskInfo->SubOadIndex=0;
    TaskInfo->CbNum = 0;
    TaskInfo->IsAlreadyBc=TASKBC_START;
    TaskInfo->RunResult = SCHEME_UNDONE;
    TaskInfo->ThisTurnPeriodStat = AMR_PRE_PERIOD;
    TaskInfo->IsNewTurn = 1;
    memset(TaskInfo->MtrResult,0x00,128);
    TaskInfo->encryptFailCnt = 0;
    TaskInfo->encryptStatus = ENCRYPT_INIT;
    //memset(TaskInfo->CbResult,0xFF,128);//修改的地方
    //memset(TaskInfo->TransResult,0x00,sizeof(OOP_TRANSRES_T)*OOP_MAX_TRANS_INFO);
    memcpy_r((uint8*)&tmpTime,(uint8*)TaskInfo->LastTurnBeginTime,7);
    tmpTime.sec=0;
    //取整，若起始时间的分钟不是15的整数倍，从最近的下一个15分钟开始
    DT_DateTimeBCD2Time(&tmpTime,&TaskInfo->StartTime);
    tmpmin = commfun_BcdToHex(tmpTime.min);
    if(tmpmin%15 != 0)
    {
        tmpmin = (tmpmin/15)*15;
        tmpTime.min = commfun_HexToBcd(tmpmin);
        DT_DateTimeBCD2Time(&tmpTime,&TaskInfo->StartTime);
        //去掉不能整除的分钟后早于了任务开始时间，需要加上一个15分钟
        TI.unit = 1;
        TI.value = 15;
        DT_TimeAddInv(&TaskInfo->StartTime, TI);
    }
}

