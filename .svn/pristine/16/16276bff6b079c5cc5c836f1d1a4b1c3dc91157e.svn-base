/*-
****************************************************************************************
 * Copyright (c) 2004,南京新联电子仪器有限责任公司
 *            All rights reserved.
 * 程序名称：MetSTD.c
 * 版本号：1.0
 * 功能：该程序完成部颁电表规约的组帧、析帧及相关数据的存储功能
 * 开发人：尤斌
 * 开发时间：2006年12月22日
 * 修改者:
 * 修改时间:
 * 修改简要说明:
 * 其他：
*****************************************************************************************
-*/

 #include "../../include/CtsAll.h"
 #include "../../include/metstdcap.h"


extern uint8 ComNo[10];
extern uint8 LocalAddr[6];    //终端地址



/****************************************************************************
名称： Prepare_645_07_meter_read
功能：
-           为645_07规约的电表的抄读做准备，检查执行任务的测量的信息
处理流程：
-           
输入参数：
-           TASK_RUN_INFO *pCbTask, uint8* pipe, S_COMM_INFO_MET *Comm_Info_Oop,cbMtrPara *MtrPara
-       
输出参数：
-           无
返回：
-           无
****************************************************************************/

void Prepare_Cap_read(SCHEME_TASK_RUN_INFO *pCbTask, uint8 pipe, CTS_RUN_TASK_T *RunTaskInfo, S_COMM_INFO_MET *Comm_Info_Oop,cbMtrPara *MtrPara)
{


    uint16 sn;
    uint16 MeterIndex;
    OOP_METER_BASIC_T MeterBasicInfo;
    //task_id=pCbTask->TaskId;
    MeterIndex=RunTaskInfo->MeterIndex;
    sn=pCbTask->MsInfo[MeterIndex].Sn;
    //读取测量点信息
    memset((void *)MtrPara, 0x0, sizeof(cbMtrPara));

    //读取测量点信息
    taskmng_mtr_basic_info_get(sn,RunTaskInfo->logicid,&MeterBasicInfo);
    MtrPara->MtrNum=sn;
    //MtrPara->Speed=4800;
    MtrPara->Speed=4;
    memcpy_r(MtrPara->MetAddr,MeterBasicInfo.tsa.add,6);
    memcpy((uint8 *)&MtrPara->MtrCommAddr,(uint8*)&MeterBasicInfo.tsa,sizeof(OOP_TSA_T));
    memset(Comm_Info_Oop,0,sizeof(S_COMM_INFO_MET));
 
    Comm_Info_Oop->pMtrPara = MtrPara;
    Comm_Info_Oop->MetStat = CB_STATE_INIT;
    //读取通信端口信息COM
    //STD2007_LoadPrifle_Init(&Comm_Info_Oop->cbLPPara,pCbTask,RunTaskInfo);//有疑问的地方，算补抄的？？？？？？？？？
}

/****************************************************************************
*函数名称：Meter_645_07_Read_Item
*功能：  645_07抄读发送报文
*输入参数：
*输出参数：
*返回：
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
            TASK_FMT_TRACE( pipe, COMM_LOG, "发送失败 ret is %d\n",ret);
        }
        TASK_BUF_TRACE( pipe, COMM_LOG, Comm_Info_Oop->pSndBuf, Comm_Info_Oop->SndLen);
        Comm_Info_Oop->MetStat = CB_STATE_RECV;         //发送成功，准备接收
    }
    else
    {
        Comm_Info_Oop->MetStat = CB_STATE_FINISH;
    }
    if(Comm_Info_Oop->MetStat != CB_STATE_RECV )//0xff  所有数据项已经抄完不再处于接收状态
    {
        Comm_Info_Oop->MetStat = CB_STATE_FINISH;       //默认为结束状态，只有在查找到合适的数据项后才转入接收状态，其他都算结束
    }
}


/****************************************************************************
*函数名称：Deal_Receive_Result_645_07
*功能：处理  645_07抄读接收报文的结果
*输入参数：
*输出参数：
*返回：
****************************************************************************/

void Meter_Cap_Recvive_Data(SCHEME_TASK_RUN_INFO *pCbTask,CTS_RUN_TASK_T *RunTaskInfo, uint8 pipe, S_COMM_INFO_MET *Comm_Info_Oop,cbMtrPara *MtrPara,uint8 *DI07Index,uint8 DI07Num,uint8 *CombinBuff,uint16 *CombinLen,uint32 *DI07Buff)
{
    uint8               *ptr=NULL;
    
   

   
    OOP_OAD_U mainOAD;
    
    mainOAD.value = 0x00;

    memset(Comm_Info_Oop->pRcvBuf, 0x00, sizeof(Comm_Info_Oop->pRcvBuf));


    Comm_Info_Oop->RcvLen = rs485_RS232_recv(pipe,Comm_Info_Oop->pRcvBuf, sizeof(Comm_Info_Oop->pRcvBuf));
    
    TASK_BUF_TRACE( pipe, COMM_LOG,Comm_Info_Oop->pRcvBuf, Comm_Info_Oop->RcvLen);
    
    
    if(Comm_Info_Oop->RcvLen > 9)   //有效报文的最小长度为9个字节
    {       
        ptr = (uint8*)commfun_memchr_cap((char*)Comm_Info_Oop->pRcvBuf, 0x68, Comm_Info_Oop->RcvLen);//找到645报文的68头
    }
    TASK_FMT_TRACE( pipe, COMM_LOG, "ptr is %p,Comm_Info_Oop->pRcvBuf is %p\n",ptr,Comm_Info_Oop->pRcvBuf);
    if(Comm_Info_Oop->RcvLen <= 0 || ptr == NULL)
    {
        Comm_Info_Oop->MetStat =  CB_STATE_FINISH;
        return ;
    }
    else
    {
        Comm_Info_Oop->RcvLen -= (ptr - Comm_Info_Oop->pRcvBuf);//纯电容器报文长度
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
*模块编号?
*名称：根据接收到的报文，判断帧的有效性
*功能：部颁规约电表判断返回帧的有效性
*输入参数：通道号（pipe）、抄表任务结构体(*pTaskAllot)、接收到电表应答的数据缓冲区指针(*pBuf)、报文长度(rcvLen)
*输出参数：无
*处理： 检测首字节是否等于同步头，即0x68；
*       检测第七个字节是否也等于同步头，即0x68；
*   检测最后一个字节是否等于结束码（根据报文内的报文长度，截取到最后一个字节），即0x16；
*   检测报文的校验码是否等于最后第二个字节；
*   检测报文中的表地址是否等于pTaskAllot.MetAddr[]；
*       上面属于校验报文是否成功部分。
*   根据控制码（报文第8字节），判断从站是否正常应答，是否有后续帧。
*   如果控制码D6位为1，表示从站异常应答；
*   如果控制码D6位为0，表示从站正常应答；
*   如果控制码D5位为0，表示无后续帧；
*   如果控制码D5位为1，表示有后续帧；
*返回： -0校验失败-
*       -1校验成功无后续数据帧-
*       -2校验成功有后续数据帧-
*       -3校验成功正常应答帧-
*       -4校验成功异常应答帧-
*       -5校验成功错误应答帧-
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
*模块编号：7-9-1-1
*名称：组织抄表任务发送帧
*功能：部颁规约电表抄表组帧
*输入参数：电表地址pMetAddr、数据标识fraDI、命令标识cmdDI、帧存放缓冲区地址pBuf、帧长存放地址pSndLen
                  根据采集内容重新计算数据标识码，
*输出参数：组帧后的缓冲区、帧长


*处理：根据电表规约组帧
*返回：无
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
    memcpy(&tmpSnd[offset], SourAddr, 4); /*- 源地址 -*/
    offset=offset+4;
    memcpy(&tmpSnd[offset], DestAddr, 4); /*- 源地址 -*/
    offset=offset+4;
    tmpoffset1 = offset;//数据帧头的位置
    tmpSnd[offset++] = 0x68;
    tmpSnd[offset++] = S_Type;  /*- 源设备类型 -*/
    tmpoffset2 = offset++;    /*- 数据长度，整帧的数据长度 -*/
   
    tmpSnd[offset++] = CmdType;   /*- 命令类型 -*/
    tmpSnd[offset++] = Command;   /*- 命令类型 -*/
    if(DataLen !=0)
    {
        memcpy(&tmpSnd[offset],Data,DataLen);//数据域
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
名称： STDCAP_AUTK_Recv
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
        //数据转换

        //frame_printf(desbuffer,desLen);
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
        taskmng_mtr_result_set(MeterIndex,RunTaskInfo->MtrResult);
        taskmng_task_record_file_set(pipe,MsIndex,RunTaskInfo->TaskId,RunTaskInfo->logicid);


        //taskmng_meter_data_save(desbuffer,desLen,StoreInfo,pipe);   
        taskmng_cache_buf_save(pipe,pn,mainOAD,subOAD,desbuffer,desLen,colStartTm,longTm);

    }
    Comm_Info_Oop->mapindex++;//转换下一条命令
    TASK_FMT_TRACE ( pipe,COMM_LOG, "下一条命令 Comm_Info_Oop->mapindex %d,Comm_Info_Oop->SucOADNum is %d,pCbTask->CsdTask.RoadTask[0].SubOadNum is %d\n",Comm_Info_Oop->mapindex,Comm_Info_Oop->SucOADNum,pCbTask->CsdTask.RoadTask[0].SubOadNum);
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







