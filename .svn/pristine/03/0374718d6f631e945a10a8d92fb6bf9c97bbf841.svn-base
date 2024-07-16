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
#include "../../include/metstdprotector.h"




/*
*****************************************************************************************
*- 国标表命令ID宏定义下行 -
*****************************************************************************************
*/
#define STD2007_CMDID_READ          0x11    /*-读数据-*/
#define STD2007_CMDID_READNEXT      0x12    /*-读后续数据-*/
#define STD2007_CMDID_REREAD        0x13    /*-重读数据-*/
#define STD2007_CMDID_WRITE         0x14    /*-写数据-*/
#define STD2007_CMDID_SETTIME       0x08    /*-广播校时(YYMMDDhhmmss)-*/
#define STD2007_CMDID_SETADDR       0x0A    /*-写设备地址(用广播地址设置新地址)-*/
#define STD2007_CMDID_SETRATE       0x0C    /*-更改通信波特率-*/
#define STD2007_CMDID_MDFPSW        0x0F    /*-修改密码-*/
#define STD2007_CMDID_CLRMAXAD      0x10    /*-最大需量清零-*/

/*
*****************************************************************************************
*-其他-
*****************************************************************************************
*/
#define STD2007_FILL_CODE_DD        0xDD    /*-0xAA+0x33空数据或数据域结束充字符(大项没有)-*/
#define STD2007_FILL_CODE_EE        0xEE    /*-0xBB+0x33空数据或空字节填充字符(小项没有)-*/
//static uint8 STD_BROADCAST_ADDRESS[6] = {0x99, 0x99, 0x99, 0x99, 0x99, 0x99}; /*-广播地址-*/
#define STD2007_GAP_VALUE           0x33    /*-数据域DATA发送时+33H接收时-33H-*/
#define STD2007_ADDR_SITE           14

#define STD2007_SYNC                0x68
#define POINTDAY_BC_NUM             3 //日冻结数据补抄次数



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

void Prepare_Protector_meter_read(SCHEME_TASK_RUN_INFO *pCbTask, uint8 pipe, CTS_RUN_TASK_T *RunTaskInfo, S_COMM_INFO_MET *Comm_Info_Oop,cbMtrPara *MtrPara)
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
    //MtrPara->Speed=get_meter_read_speed(MeterBasicInfo.baud);
    MtrPara->Speed=MeterBasicInfo.baud;
    memcpy_r(MtrPara->MetAddr,MeterBasicInfo.tsa.add,6);
    memcpy((uint8 *)&MtrPara->MtrCommAddr,(uint8*)&MeterBasicInfo.tsa,sizeof(OOP_TSA_T));
    memset(Comm_Info_Oop,0,sizeof(S_COMM_INFO_MET));
 
    Comm_Info_Oop->pMtrPara = MtrPara;
    Comm_Info_Oop->MetStat = CB_STATE_INIT;
    //读取通信端口信息COM
    STD2007_LoadPrifle_Init(&Comm_Info_Oop->cbLPPara,pCbTask,RunTaskInfo);//有疑问的地方，算补抄的？？？？？？？？？
}

/****************************************************************************
*函数名称：Meter_645_07_Read_Item
*功能：  645_07抄读发送报文
*输入参数：
*输出参数：
*返回：
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
        *DI07Num = GetDI07_485(pCbTask->AcqInfo,&pCbTask->CsdTask.RoadTask[CsdIndex],SubOadIndex,Comm_Info_Oop->cbLPPara.endNum,DI07Buff,pCbTask,RunTaskInfo,pipe);//获取抄表的数据标识

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
            TASK_FMT_TRACE( pipe, COMM_LOG, "发送失败 ret is %d\n",ret);
        }
        TASK_BUF_TRACE( pipe, COMM_LOG, Comm_Info_Oop->pSndBuf, Comm_Info_Oop->SndLen);
        Comm_Info_Oop->MetStat = CB_STATE_RECV;         //发送成功，准备接收
        taskmng_task_monitor_update(pipe,pCbTask->TaskId,pCbTask->logicid,UDTASK_SENDFRMNUM,NULL);
        gAmrThreadArg[pipe].pMtrAmrInfo[tmpMsIndex].LastSendTime=time(NULL);
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

CB_STATE Deal_Receive_Result_Protector(S_COMM_INFO_MET *Comm_Info_Oop,SCHEME_TASK_RUN_INFO *pCbTask,CTS_RUN_TASK_T *RunTaskInfo,BOOL actResult,uint8 pipe)
{
    uint16 pn,tmpMsIndex,MeterIndex;
    
    MeterIndex=RunTaskInfo->MeterIndex;
    pn=pCbTask->MsInfo[MeterIndex].Sn;
    tmpMsIndex = taskmng_ms_index_get(pipe,pn,RunTaskInfo->logicid);


    eOopCommRslt_t ret = 0;
    if(actResult)                          //该帧交换正确
    {

        STD07_ToNextReadItem(FALSE,Comm_Info_Oop,pCbTask,RunTaskInfo);
        Comm_Info_Oop->ComFrame++;          //总帧数增加
    
        Comm_Info_Oop->ErrFrame = 0;        //失败帧数和错误帧数清零，准备重新计数
        Comm_Info_Oop->ErrTimes = 0;
        gAmrThreadArg[pipe].pMtrAmrInfo[tmpMsIndex].AllFailCnt = 0;
        gAmrThreadArg[pipe].pMtrAmrInfo[tmpMsIndex].LastResult = TRUE;
    }
    else                                            //该帧交换失败
    {
        TASK_FMT_TRACE( pipe, COMM_LOG,"ERR_FRAME_MAX is %d,ERR_TIMES_MAX is %d\n", ERR_FRAME_MAX,ERR_TIMES_MAX);
        if(Comm_Info_Oop->ErrTimes >= 0)        //同一帧的失败次数
        {

            if(Comm_Info_Oop->ErrFrame >= 10)      //失败次数过多，不再继续，准备退出
            {
                Comm_Info_Oop->ErrFrame = ERR_FRAME_MAX;
                Comm_Info_Oop->MetStat = CB_STATE_FINISH;
                return Comm_Info_Oop->MetStat;
            }
            else
            {
                if(0 == ret)                                                  //没有接收到数据
                {
                Comm_Info_Oop->ErrFrame++;                        //失败帧数增加
                Comm_Info_Oop->ComFrame++;                        //总帧数增加
                }

                Comm_Info_Oop->ErrTimes = 0;                         //重试次数清零
                STD07_ToNextReadItem(FALSE,Comm_Info_Oop,pCbTask,RunTaskInfo);//冻结类第一个数据项回否认，继续抄读，不切表
            }
            gAmrThreadArg[pipe].pMtrAmrInfo[tmpMsIndex].LastResult = FALSE;
            gAmrThreadArg[pipe].pMtrAmrInfo[tmpMsIndex].AllFailCnt++;
            gAmrThreadArg[pipe].pMtrAmrInfo[tmpMsIndex].LastFailTime=time(NULL);
        }
        else
        {
            if(ret==0)                                                           //没有接收数据
            {
                Comm_Info_Oop->ErrTimes++;                              //重发当前数据项
            }
            gAmrThreadArg[pipe].pMtrAmrInfo[tmpMsIndex].LastResult = FALSE;
            gAmrThreadArg[pipe].pMtrAmrInfo[tmpMsIndex].AllFailCnt++;
            gAmrThreadArg[pipe].pMtrAmrInfo[tmpMsIndex].LastFailTime=time(NULL);
        }
    }
    Comm_Info_Oop->MetStat = CB_STATE_SEND;                    //转入发送流程
    return Comm_Info_Oop->MetStat;
}



/****************************************************************************
*函数名称：Deal_Receive_Result_645_07
*功能：处理  645_07抄读接收报文的结果
*输入参数：
*输出参数：
*返回：
****************************************************************************/

void Protector_Recvive_Data(SCHEME_TASK_RUN_INFO *pCbTask,CTS_RUN_TASK_T *RunTaskInfo, uint8 pipe, S_COMM_INFO_MET *Comm_Info_Oop,cbMtrPara *MtrPara,uint8 *DI07Index,uint8 DI07Num,uint8 *CombinBuff,uint16 *CombinLen,uint32 *DI07Buff)
{
    uint8               *ptr=NULL;
    BOOL        actResult = TRUE;       //对回码处理的结果
    eOopCommRslt_t ret = 0;

    uint32              DI07 = 0;                                   //数据标识

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
    
    
    if(Comm_Info_Oop->RcvLen > 9)   //有效报文的最小长度为9个字节
    {
        gAmrThreadArg[pipe].pMtrAmrInfo[tmpMsIndex].LastRecvTime=time(NULL);
        taskmng_task_monitor_update(pipe,pCbTask->TaskId,pCbTask->logicid,UDTASK_RECVFRMNUM,NULL);
        ptr = (uint8*)commfun_645_frame_get((char*)Comm_Info_Oop->pRcvBuf, STD2007Para.Sync, Comm_Info_Oop->RcvLen,&reallen);//找到645报文的68头
    }
    if(Comm_Info_Oop->RcvLen <= 0 || ptr == NULL)
    {
        actResult = FALSE;              //电表没有回码 或者 回码格式非法
    }
    else
    {

        Comm_Info_Oop->RcvLen = reallen;//纯645报文长度

        memmove(Comm_Info_Oop->pRcvBuf, ptr, Comm_Info_Oop->RcvLen);
        ret = STD2007_CheckRspFrame(pipe, Comm_Info_Oop->pRcvBuf, (uint16)Comm_Info_Oop->RcvLen, 0,Comm_Info_Oop);

        if((ret == 1)||(ret == 2))    //有效,且有数据返回
        {
            #if 1
            Comm_Info_Oop->ErrFrame=0;
            OOP_OAD_U mainOad;
            if(DI07Num == 1)//不存在多个数据项,一帧抄回来
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
            else//需要多帧抄
            {
                DI07 = DI07Buff[*DI07Index];
                if(*DI07Index<DI07Num)//OAD 对应的07数据标识还没有通讯完
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
        else if(ret == 3 || ret == 5) //有有效数据，但值和期望的不一样
        {
            Comm_Info_Oop->ErrFrame=0;
            Comm_Info_Oop->ErrTimes = ERR_TIMES_MAX;        //按失败处理，不过不在重试，直接将重试次数置为最大值
            actResult = FALSE;
        }
        else if(ret == 4)                 //否认帧
        {
            Comm_Info_Oop->ErrFrame = 0;
            Comm_Info_Oop->ErrTimes = ERR_TIMES_MAX;             //如果是否认帧，则不再重试，直接将重试次数置为最大值
            actResult = FALSE;
        }
        else                                 //报文无效、错误
        {
            actResult = FALSE;
        }
    }
    Deal_Receive_Result_Protector(Comm_Info_Oop,pCbTask,RunTaskInfo,actResult,pipe);
}

/****************************************************************************
名称： Protector_AUTK_Recv
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

uint16 Protector_AUTK_Recv(uint8 pipe,OOP_OAD_U mainOAD,OOP_OAD_U subOAD,uint8 *pBuf, uint16 rcvLen, cbMtrPara mCbMtrPara, DateTimeBcd_t *StoreTime,SCHEME_TASK_RUN_INFO *pCbTask,CTS_RUN_TASK_T *RunTaskInfo)
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
        //冻结的变量处理
        if(subOAD.nObjID == 0x2004 || subOAD.nObjID == 0x2005)
        {
            if(subOAD.nObjID == 0x2005)
                offSet = rcvLen / 2;
        }
    }
    //数据域-0x33
    ReduceGap(tmpBufer, rcvLen, STD2007_GAP_VALUE);
    
    //数据转换
    memcpy(sourcedata.sourcebuffer,tmpBufer,rcvLen);
    sourcedata.reclen=rcvLen;
    Convert_Protector_To_Struct(sourcedata ,desbuffer,&desLen,pMapOopTo645);

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
    TASK_FMT_TRACE ( pipe,COMM_LOG, "r任务号:pCbTask->TaskId is %d\n",pCbTask->TaskId);
    TASK_FMT_TRACE ( pipe,COMM_LOG, "subOAD is %x\n",subOAD.value);
    TASK_BUF_TRACE( pipe,COMM_LOG, desbuffer,desLen);
    taskmng_meter_data_save(desbuffer,desLen,StoreInfo,pipe);   

    

    return offSet;
}

/****************************************************************************
名称： STD64507_AUTK_Recv_Profile
功能：
-           64507负荷曲线解析，并按照采集方案保存数据
处理流程：
?输入参数：
-           pipe                通道号
-           pBuf                接收到的电表回码报文
-           rcvLen              接收的长度
-           mCbMtrPara          当前抄表的测量点的信息
-           pCbTask         当前任务信息
输出参数：
-           无
返回：
-           0:数据解析错误
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
    //检查负荷曲线格式是否正确
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
        //电压电流
        //if(pBuf[offSet++]!=0xDD)
        //  break;

        if(pBuf[offSet]!=0xDD)
        {
            memcpy(profileBuff+5,pBuf+offSet,17);
            offSet += 17;
        }
        //功率
        if(pBuf[offSet++]!=0xDD)
            break;
        if(pBuf[offSet]!=0xDD)
        {
            memcpy(profileBuff+22,pBuf+offSet,24);
            offSet += 24;
        }
        //功率因数
        if(pBuf[offSet++]!=0xDD)
            break;
        if(pBuf[offSet]!=0xDD)
        {
            memcpy(profileBuff+46,pBuf+offSet,8);
            offSet += 8;
        }
        //有功电能
        if(pBuf[offSet++]!=0xDD)
            break;
        if(pBuf[offSet]!=0xDD)
        {
            memcpy(profileBuff+54,pBuf+offSet,16);
            //commfun_printstring(profileBuff+54,16);
            offSet += 16;
        }
        //无功电能
        if(pBuf[offSet++]!=0xDD)
            break;
        if(pBuf[offSet]!=0xDD)
        {
            memcpy(profileBuff+70,pBuf+offSet,16);
            offSet += 16;
        }
        //当前需量
        if(pBuf[offSet++]!=0xDD)
            break;
        if(pBuf[offSet]!=0xDD)
        {
            memcpy(profileBuff+86,pBuf+offSet,6);
            offSet += 6;
        }
        if(pBuf[offSet++]!=0xDD)
            break;
        //校验和判断
        offSet++;
        //结束符判断
        if(pBuf[offSet++]!=0x18)
            break;
        DateTimeBcd_t storeTm;
        memcpy_r(&storeTm.year_l,profileBuff,5);
        ReduceGap(&storeTm.year_l, 5, STD2007_GAP_VALUE);
        storeTm.year_h = 0x20;
        storeTm.sec = 0x00;

        #if(1 == OOP_CRV_ADDREAD_485) //更新曲线抄读起始时间
        UpdateCrvNewStartTime(oad.nObjID, &storeTm, pCbTask->TaskId, pCbTask->SchemeId, Pn, &pCbTask->AcqInfo);
        #endif

        uint8 i,subOadNum;
        subOadNum = pCbTask->CsdTask.RoadTask[CsdIndex].SubOadNum;
        if(subOadNum == 0)
            break;
        i=0;
        TASK_FMT_TRACE ( pipe,REC_LOG, "负荷记录抄读subOadNum is %d\n",subOadNum);
        TASK_BUF_TRACE(pipe,REC_LOG,pBuf, rcvLen);
        while(i<subOadNum)
        {
            uint16 bias = CaclBiasByOad(oad,pCbTask->CsdTask.RoadTask[CsdIndex].OadListInfo[i]);
            TASK_FMT_TRACE ( pipe,REC_LOG, "在06000001中的bias is %d,oad.value is %x,pCbTask->CsdTask.RoadTask[CsdIndex].OadListInfo[i].value is %x\n",bias,oad.value,pCbTask->CsdTask.RoadTask[CsdIndex].OadListInfo[i].value);
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
*函数名称：STD64507_AUTK_Recv_Frz
*功能：处理  645_07抄读返回的冻结数据
*输入参数：
*输出参数：
*返回：
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
        //按照冻结时标采集，需要判断数据是否在时间间隔范围内
        if(RunTaskInfo->SubOadIndex==0)
        {
//          printf("STD64507_AUTK_Recv_Frz is BEGIN1111\n");
            if(pCbTask->CsdTask.RoadTask[CsIndex].OadListInfo[SubOadIndex].nObjID != 0x2021)//第一列必须是冻结时标
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
            //commfun_printstring((uint8*)&storeTm,7);//打印采集到的存储时间
            if(DT_DateTimeBCD2Time(&storeTm,&longTm)==FALSE)//应答的时间不合法
            {
                //不存储
//                printf("STD64507_AUTK_Recv_Frz is BEGIN4444\n");
                pComm_Info->cbLPPara.endNum = 0;
                return 0;
            }
            if(pCbTask->AcqInfo.Acqtype == ACQTYP_FRZTM || pCbTask->AcqInfo.Acqtype == ACQTYP_TI)
            {
                //使用前闭后闭，参数初始化决定开
                if(pCbTask->CsdTask.RoadTask[CsIndex].MainOad.nObjID == 0x5003)
                {
                    if(longTm < pComm_Info->cbLPPara.startTime)
                    {
                        //冻结时标比起始时间还小，不需要继续搜索了
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
                        //冻结时标比起始时间还小，不需要继续搜索了
                        pComm_Info->cbLPPara.endNum = 0;
                        #if(1 == OOP_DAY_ADDREAD_485)
                        if(pCbTask->CsdTask.RoadTask[CsIndex].MainOad.nObjID == 0x5004)
                        {
//                          printf("STD64507_AUTK_Recv_Frz is BEGIN6666\n");
                            pComm_Info->cbLPPara.storeTm = storeTm;//保存存储时标
                        }
                        #else
                        if(pCbTask->CsdTask.RoadTask[CsIndex].MainOad.nObjID == 0x5004)
                        {
                        //    pCbTask->pCurMtr->needCBagain = TRUE;//需要补抄
//                          printf("STD64507_AUTK_Recv_Frz is BEGIN7777\n");
                        }
                        #endif
                        return 0;
                    }
                }
                if(longTm > pComm_Info->cbLPPara.endTime)
                {
                    //不在采集的时间间隔类，不存储，继续搜索上一条记录
                    return 0;
                }
                if(pCbTask->CsdTask.RoadTask[CsIndex].MainOad.nObjID == 0x5004)
                {
                    #if(0 == OOP_DAY_ADDREAD_485)
               //     pCbTask->pCurMtr->needCBagain = FALSE;//抄成功不需要补抄,先注释掉
                    pComm_Info->cbLPPara.endNum = 0;//不在搜索
                    #endif
                }
            }
            else if(pCbTask->AcqInfo.Acqtype == ACQTYP_LASTN)//上N次的不比较时间
            {
//                printf("按采集上N次采集，不补抄\r\n");
           //     pCbTask->pCurMtr->needCBagain = FALSE;//抄成功不需要补抄,先注释掉
                pComm_Info->cbLPPara.endNum = 0;//不在搜索
            }

            pComm_Info->cbLPPara.storeTm = storeTm;//保存存储时标
            if(pCbTask->StoreTimeType != STRTMTYP_FRZTIME)//如果不是按照冻结时标存储，则不搜索了
            {
                pComm_Info->cbLPPara.endNum = 0;
            }
        }
    }
    else
    {
        if(pCbTask->CsdTask.RoadTask[CsIndex].MainOad.nObjID == 0x5004)
        {
          //  pCbTask->pCurMtr->needCBagain = FALSE;//抄成功不需要补抄
        }
    }
//  printf("pCbTask->StoreTimeType is %d\n",pCbTask->StoreTimeType);
    if(pCbTask->StoreTimeType == STRTMTYP_FRZTIME)
    {
        #if D_485_698_RTTOPOINT == 1
        //if(IsRealDataCopyToPointData())
        {
            if(pCbTask->CsdTask.RoadTask[CsIndex].MainOad.nObjID == 0x5004 && pComm_Info->cbLPPara.endNum == 0xff && pComm_Info->cbLPPara.storeTm.year_h == 0) //日冻结实时转存,把存储时间修改为任务启动时间-延时时间
            {
                time_t StoreTm;
                OOP_TI_T delayTi;
                DateTimeBcd_t tmptime;
                memcpy_r((uint8*)&tmptime,RunTaskInfo->ThisTurnBeginTime,7);//采集启动时间
                DT_DateTimeBCD2Time(&tmptime,&StoreTm);
                delayTi.unit = pCbTask->DelayTimeUnit;
                delayTi.value = pCbTask->DelayTime;
                DT_TimeSubInv(&StoreTm, delayTi);
                pComm_Info->cbLPPara.storeTm = taskmng_acq_store_time_get(pCbTask->StoreTimeType,StoreTm);
             //   pCbTask->pCurMtr->needCBagain = FALSE;//抄成功不需要补抄
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
          //  pCbTask->pCurMtr->needCBagain = FALSE;//抄成功不需要补抄
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
名称： STD64507_AUTK_Recv_Event
功能：
-           64507事件解析，并按照采集方案保存数据
处理流程：
?输入参数：
-           pipe                通道号
-           pBuf                接收到的电表回码报文
-           rcvLen              接收的长度
-           mCbMtrPara          当前抄表的测量点的信息
-           pCbTask         当前任务信息
输出参数：
-           无
返回：
-           0:数据解析错误
****************************************************************************/
uint16 STDProtector_AUTK_Recv_Event(uint8 pipe,OOP_OAD_U oad,uint8 *pBuf, uint16 rcvLen, cbMtrPara mCbMtrPara, S_COMM_INFO_MET *pComm_Info,SCHEME_TASK_RUN_INFO *pCbTask,uint8 *DI07Index,uint8 DI07Num,CTS_RUN_TASK_T *RunTaskInfo)
{
    uint8 i =0,subOadNum;
    uint16 bias;
    DateTimeBcd_t storeTm;
    time_t longTm;


    uint16 CsdIndex;            
    uint16 SubOadIndex;
    uint16 MeterIndex,pn;  //当前抄表电表索引 

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
        //按照冻结时标采集，需要判断数据是否在时间间隔范围内
        if(pCbTask->CsdTask.RoadTask[CsdIndex].OadListInfo[0].nObjID != 0x201E)//第一列必须是事件发生时间
        {
            pComm_Info->cbLPPara.endNum = 0;
            return 0;
        }
        if(*DI07Index == 0)
        {
            bias = CaclBiasByOad(oad,pCbTask->CsdTask.RoadTask[CsdIndex].OadListInfo[0]);//获取事件发生时间的偏移位置
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
            if(DT_DateTimeBCD2Time(&storeTm,&longTm)==FALSE)//应答的时间不合法
            {
                //不存储
                pComm_Info->cbLPPara.endNum = 0;
                return 0;
            }
            pComm_Info->cbLPPara.storeTm = storeTm;//保存存储时标
            //前闭后开，参数初始化处不用处理
            if(longTm < pComm_Info->cbLPPara.startTime)
            {
                //冻结时标比起始时间还小，不需要继续搜索了
                pComm_Info->cbLPPara.endNum = 0;
                return 0;
            }
            if(longTm >= pComm_Info->cbLPPara.endTime)
            {
                //不在采集的时间间隔类，不存储，继续搜索上一条记录
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
        else if(*DI07Index == 1)//事件发生序号
        {
            bias = 14;
            subOad.nObjID = 0x2022;
            subOad.attID = 0x02;
            subOad.nIndex = 0;
           
            if(DT_DateTimeBCD2Time(&pComm_Info->cbLPPara.storeTm,&longTm)==FALSE)//应答的时间不合法
            {
                //不存储
                pComm_Info->cbLPPara.endNum = 0;
                return 0;
            }
            //前闭后开，参数初始化处不用处理
            if(longTm < pComm_Info->cbLPPara.startTime)
            {
                //冻结时标比起始时间还小，不需要继续搜索了
                pComm_Info->cbLPPara.endNum = 0;
                return 0;
            }
            if(longTm >= pComm_Info->cbLPPara.endTime)
            {
                //不在采集的时间间隔类，不存储，继续搜索上一条记录
                return 0;
            }
            STD64507_AUTK_Recv(pipe,oad,subOad,&pBuf[bias],rcvLen-bias,mCbMtrPara,NULL,pCbTask,RunTaskInfo);
        }
    }

    if(pCbTask->AcqInfo.Acqtype == ACQTYP_LASTN)
    {

        

        if(pCbTask->CsdTask.RoadTask[CsdIndex].OadListInfo[0].nObjID != 0x2022)//第一列必须是事件记录序号
        {
            pComm_Info->cbLPPara.endNum = 0;
            return 0;
        }

        if(*DI07Index == 0)                  //第一帧抄读事件记录序号
        {
            uint32      LastRecNum = 0;                 //上一次抄读事件记录序号
            uint32      RecvRecNum = 0;                 //本次回复事件记录序号
            uint8       EvtCnt[3] = {0};

                        
            if(FALSE == FindEvtRecNoIDByOad(oad))
            {

                TASK_BUF_TRACE( pipe, COMM_LOG, "未找到对应oad is %x\n",oad.value);

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

            TASK_FMT_TRACE( pipe, COMM_LOG, "上次抄读的事件序号LastRecNum is %d,RecvRecNum is %d\n",LastRecNum,RecvRecNum);

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
        else if(*DI07Index == 1)             //事件记录内容
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
                TASK_FMT_TRACE( pipe, REC_LOG, " 一个事件抄完 该方案需要上报  \n" );
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



