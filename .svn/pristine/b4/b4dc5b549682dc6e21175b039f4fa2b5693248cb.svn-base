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
#define _DLL_IMPLICIT_
#define FIRST_DEFINE_METPARAM
 #include "../../include/CtsAll.h"
 #include "../../include/metstd2007.h"

#if OS_METSTD2007_EN == 1

#define DEBUG_STD2007 //版本稳定后去掉此项

#ifdef DEBUG_STD2007
#define STD2007_ENV  "metstd2007"
#endif

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
uint8 STD2007_CheckRspFrame(uint8 pipe, /*cbTaskAllot_T * pTaskAllot,*/ uint8 * pBuf, uint16 rcvLen, uint8 bStdType,S_COMM_INFO_MET* pCommInfo);
void FormatToStd2007(/*uint8 pipe, uint8 srcDataType,*/ uint16 eDataId, dataFormat * pDF, uint8 bStdType,uint8 TaskType,mtrnum_t MetSn);
uint16 GetBlockIndex(const S_CbIDInfo* pInfo);
void cbtasknew_DataSave_07Ext(mtrnum_t  metSn);
extern uint8 patch_policy_flg[AMR_THREAD_NUM];



cbMetPara STD2007Para =
{
    JFPG,/*-表征费率顺序-*/
    #if SHANGHAI_FUN == 1
    4800,/*波特率*/
    #else
    2400,/*波特率*/
    #endif
    8,   /*数据位, 4-8 */
    'e', /*校验 'n'  'e'  'o'  's' = None,Odd,Even,Space*/
    1,   /*停止位 0,1,2 = 1, 1.5, 2*/
    0x68,/*-同步头--*/
    0xff,/*结束字节1*/
    0xFF,/*结束字节2*/
    0x09,/*长度1    */
    0xFF,/*长度2    */
    12,  /*附加长度,*/
    100, /*--电表反应时间:命令码发出后，收到电表返回的第一个字节的时间--*/
    100, /*--命令发送时间:发送命令报文的时间--*/
    10,  /*--字节之间的间隔时间--*/
    0,   /*-给有握手过程的电表使用,握手帧总数-*/
    0    /*-给有握手过程的电表使用,读取数据帧总数-*/
};

/****************************************************************************
*模块编号：7-9-1-1
*名称：组织抄表任务发送帧
*功能：部颁规约电表抄表组帧
*输入参数：电表地址pMetAddr、数据标识fraDI、命令标识cmdDI、帧存放缓冲区地址pBuf、帧长存放地址pSndLen
*输出参数：组帧后的缓冲区、帧长
           电表类型 bStdType   0-普通国标 1-重庆简易表

*处理：根据电表规约组帧
*返回：无
****************************************************************************/
void STD2007_MakeFrame(uint8 *pMetAddr, uint32 fraDI, uint8 cmdDI, uint8 *pBuf, uint8 *pSndLen, uint8 bStdType)
{
    uint8 tmpSnd[30];
    uint8 i;

    memset(tmpSnd, 0x00, sizeof(tmpSnd));
    for(i = 0; i < 4; i++)
        tmpSnd[i] = 0xFE;
    tmpSnd[4] = 0x68;
    memcpy(&tmpSnd[5], pMetAddr, 6); /*- 电表地址 -*/
    tmpSnd[11] = 0x68;
    /*-正常国标组帧-*/
    tmpSnd[12] = cmdDI;//控制码

    switch(cmdDI)
    {
    case STD2007_CMDID_READ:
    case STD2007_CMDID_READNEXT:
        tmpSnd[13] = 0x04;
        break;
    case STD2007_CMDID_REREAD:
        tmpSnd[13] = 0x00;
        break;
    default:
        tmpSnd[13] = 0x04;
    }

    switch(fraDI)
    {
    case 0x06000001:        //给定时间曲线数据
    {
        rtc_t cbCurBTime;

        tmpSnd[13] = 0x0a;          //长度
        memcpy(&tmpSnd[14], (uint8 *)&fraDI, sizeof(uint32));   /*- 项目表识编码 -*/
        tmpSnd[18] = 0x01;          //块数

        //时间,取整15分钟的时间
        InterTaskReadRTC(&cbCurBTime);
        if(cbCurBTime.nn <= 0x14)
            cbCurBTime.nn = 0;
        else if(cbCurBTime.nn >=0x15 && cbCurBTime.nn <= 0x29)
            cbCurBTime.nn = 0x15;
        else if(cbCurBTime.nn >=0x30 && cbCurBTime.nn <= 0x44)
            cbCurBTime.nn = 0x30;
        else if(cbCurBTime.nn >=0x45 && cbCurBTime.nn <= 0x59)
            cbCurBTime.nn = 0x45;
        else
            cbCurBTime.nn = 0;
        memcpy(&tmpSnd[19], &cbCurBTime.nn, 5);

        if (getenv(STD2007_ENV))
            printf("=============曲线抄表时间: %02x %02x %02x %02x %02x ===============\n", tmpSnd[19], tmpSnd[20], tmpSnd[21], tmpSnd[22], tmpSnd[23]);

        AddGap(&tmpSnd[14], tmpSnd[13], STD2007_GAP_VALUE);
        tmpSnd[24] = CalCs(&tmpSnd[4], 20);                 /*- 校验和 -*/
        tmpSnd[25] = 0x16;
        tmpSnd[26] = 0x00;
        tmpSnd[27] = 0x00;

        memcpy(pBuf, tmpSnd, 26);               /*- 电表发送缓冲区pBuf*/
        *pSndLen = 26; /*-有的表不支持后面多两个0   2006-7-21 10:03-*/

    }
    break;
    default:
    {
        memcpy(&tmpSnd[14], (uint8 *)&fraDI, sizeof(uint32));   /*- 项目表识编码 -*/
        AddGap(&tmpSnd[14], 4, STD2007_GAP_VALUE);
        tmpSnd[18] = CalCs(&tmpSnd[4], 14);                 /*- 校验和 -*/
        tmpSnd[19] = 0x16;
        tmpSnd[20] = 0x00;
        tmpSnd[21] = 0x00;

        memcpy(pBuf, tmpSnd, 20);               /*- 电表发送缓冲区pBuf*/
        *pSndLen = 20; /*-有的表不支持后面多两个0   2006-7-21 10:03-*/
        break;
    }

    }

    return;
}
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

void Prepare_645_07_meter_read(SCHEME_TASK_RUN_INFO *pCbTask, uint8 pipe, CTS_RUN_TASK_T *RunTaskInfo, S_COMM_INFO_MET *Comm_Info_Oop,cbMtrPara *MtrPara)
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

void Meter_645_07_Read_Item(SCHEME_TASK_RUN_INFO *pCbTask, CTS_RUN_TASK_T *RunTaskInfo, uint8 pipe, S_COMM_INFO_MET *Comm_Info_Oop,cbMtrPara *MtrPara,uint8 *DI07Index,uint8 *DI07Num,uint32 *DI07Buff)
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
            TASK_FMT_TRACE( pipe, REC_LOG, "发送失败 ret is %d\n",ret);
        }
        TASK_FMT_TRACE( pipe, REC_LOG, "测量点号 %d ,报文发送 speed %d \n",pn,MtrPara->Speed);
        TASK_BUF_TRACE( pipe, REC_LOG, Comm_Info_Oop->pSndBuf, Comm_Info_Oop->SndLen);
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

CB_STATE Deal_Receive_Result_645_07(S_COMM_INFO_MET *Comm_Info_Oop,SCHEME_TASK_RUN_INFO *pCbTask,CTS_RUN_TASK_T *RunTaskInfo,BOOL actResult,uint8 pipe)
{
    uint16 pn,tmpMsIndex,MeterIndex;

    MeterIndex=RunTaskInfo->MeterIndex;
    pn=pCbTask->MsInfo[MeterIndex].Sn;
    tmpMsIndex = taskmng_ms_index_get(pipe,pn,RunTaskInfo->logicid);


    eOopCommRslt_t ret = 0;
    if(actResult)                          //该帧交换正确
    {
//      printf("Deal_Receive_Result_645_07 is BEGIN1111\n");
        STD07_ToNextReadItem(FALSE,Comm_Info_Oop,pCbTask,RunTaskInfo);
        Comm_Info_Oop->ComFrame++;          //总帧数增加

        Comm_Info_Oop->ErrFrame = 0;        //失败帧数和错误帧数清零，准备重新计数
        Comm_Info_Oop->ErrTimes = 0;
        gAmrThreadArg[pipe].pMtrAmrInfo[tmpMsIndex].AllFailCnt =0 ;
        gAmrThreadArg[pipe].pMtrAmrInfo[tmpMsIndex].LastResult = TRUE;
    }
    else                                            //该帧交换失败
    {
        TASK_FMT_TRACE( pipe, COMM_LOG,"ERR_FRAME_MAX is %d,ERR_TIMES_MAX is %d\n", ERR_FRAME_MAX,ERR_TIMES_MAX);
        if(Comm_Info_Oop->ErrTimes >= (patch_policy_flg[pipe] == 1 ? 0 : ERR_TIMES_MAX))        //同一帧的失败次数
        {

            if(Comm_Info_Oop->ErrFrame >= (patch_policy_flg[pipe] == 1 ? 0 : ERR_FRAME_MAX))      //失败次数过多，不再继续，准备退出
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

CB_STATE Deal_Receive_Result_Protector_645_07(S_COMM_INFO_MET *Comm_Info_Oop,SCHEME_TASK_RUN_INFO *pCbTask,CTS_RUN_TASK_T *RunTaskInfo,BOOL actResult,uint8 pipe)
{
    uint16 pn,tmpMsIndex,MeterIndex;

    MeterIndex=RunTaskInfo->MeterIndex;
    pn=pCbTask->MsInfo[MeterIndex].Sn;
    tmpMsIndex = taskmng_ms_index_get(pipe,pn,RunTaskInfo->logicid);


    eOopCommRslt_t ret = 0;
    if(actResult)                          //该帧交换正确
    {
//      printf("Deal_Receive_Result_645_07 is BEGIN1111\n");
        STD07_ToNextReadItem(FALSE,Comm_Info_Oop,pCbTask,RunTaskInfo);
        Comm_Info_Oop->ComFrame++;          //总帧数增加

        Comm_Info_Oop->ErrFrame = 0;        //失败帧数和错误帧数清零，准备重新计数
        Comm_Info_Oop->ErrTimes = 0;
        gAmrThreadArg[pipe].pMtrAmrInfo[tmpMsIndex].AllFailCnt =0;
        gAmrThreadArg[pipe].pMtrAmrInfo[tmpMsIndex].LastResult = TRUE;
    }
    else                                            //该帧交换失败
    {

        if(Comm_Info_Oop->ErrTimes >= 0)        //同一帧的失败次数
        {

            if(Comm_Info_Oop->ErrFrame >= 10)      //失败次数过多，不再继续，准备退出
            {
                Comm_Info_Oop->ErrFrame = 10;
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

void Meter_645_07_Recvive_Data(SCHEME_TASK_RUN_INFO *pCbTask,CTS_RUN_TASK_T *RunTaskInfo, uint8 pipe, S_COMM_INFO_MET *Comm_Info_Oop,cbMtrPara *MtrPara,uint8 *DI07Index,uint8 DI07Num,uint8 *CombinBuff,uint16 *CombinLen,uint32 *DI07Buff)
{
    uint8               *ptr=NULL;
    BOOL        actResult = TRUE;       //对回码处理的结果
    eOopCommRslt_t ret = 0;

    uint32              DI07 = 0;                                   //数据标识

    uint8 CsdIndex,SubOadIndex;
    uint16 MeterIndex,pn,tmpMsIndex;
    uint16 reallen=0;
    //uint16 result=0;

    MeterIndex=RunTaskInfo->MeterIndex;
    pn=pCbTask->MsInfo[MeterIndex].Sn;
    tmpMsIndex = taskmng_ms_index_get(pipe,pn,RunTaskInfo->logicid);

    SubOadIndex=RunTaskInfo->SubOadIndex;
    CsdIndex=RunTaskInfo->CsdIndex;

    memset(Comm_Info_Oop->pRcvBuf, 0x00, sizeof(Comm_Info_Oop->pRcvBuf));


    Comm_Info_Oop->RcvLen = rs485_RS232_recv(pipe,Comm_Info_Oop->pRcvBuf, sizeof(Comm_Info_Oop->pRcvBuf));
    TASK_FMT_TRACE( pipe, REC_LOG,"pn %d 收到报文 \n",pn);
    TASK_BUF_TRACE( pipe, REC_LOG,Comm_Info_Oop->pRcvBuf, Comm_Info_Oop->RcvLen);


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
        if(Comm_Info_Oop->RcvLen>255)
        {
            Comm_Info_Oop->RcvLen = 255;
        }
        memmove(Comm_Info_Oop->pRcvBuf, ptr, Comm_Info_Oop->RcvLen);
        ret = STD2007_CheckRspFrame(pipe, Comm_Info_Oop->pRcvBuf, (uint16)Comm_Info_Oop->RcvLen, 0,Comm_Info_Oop);

        if((ret == 1)||(ret == 2))    //有效,且有数据返回
        {
            #if 1
            Comm_Info_Oop->ErrFrame=0;
            OOP_OAD_U mainOad;
            TASK_FMT_TRACE( pipe, REC_LOG,"DI07Num %u  \n",DI07Num);
            if(DI07Num == 1)//不存在多个数据项,一帧抄回来
            {
                if(pCbTask->CsdTask.RoadTask[CsdIndex].MainOad.nObjID==0)
                {
                    memset(&mainOad,0x00,sizeof(OOP_OAD_U));
                    STD64507_AUTK_Recv(pipe,mainOad,pCbTask->CsdTask.RoadTask[CsdIndex].OadListInfo[SubOadIndex],Comm_Info_Oop->pRcvBuf+14, (uint16)Comm_Info_Oop->RcvLen-14-2, *MtrPara,NULL, pCbTask,RunTaskInfo);
                }
                else
                {
                    mainOad = pCbTask->CsdTask.RoadTask[CsdIndex].MainOad;
                    if(pCbTask->AcqInfo.Acqtype == ACQTYP_NULL)
                    {
                        STD64507_AUTK_Recv(pipe,mainOad,pCbTask->CsdTask.RoadTask[CsdIndex].OadListInfo[SubOadIndex],Comm_Info_Oop->pRcvBuf+14, (uint16)Comm_Info_Oop->RcvLen-14-2, *MtrPara, NULL,pCbTask,RunTaskInfo);
                    }
                    else
                    {
                        if(mainOad.nObjID == 0x5002)
                        {
                            {
                                STD64507_AUTK_Recv_Profile(pipe,mainOad,Comm_Info_Oop->pRcvBuf, (uint16)Comm_Info_Oop->RcvLen-2, *MtrPara, pCbTask,RunTaskInfo);
                                //if(result==2)
                                //{
                                    //pCbTask->AcqInfo.Acqtype = ACQTYP_NULL;
                                    //Comm_Info_Oop->MetStat = CB_STATE_SEND;
                                //}
                            }
                        }
                        else if((mainOad.nObjID & 0xF000)== 0x3000)
                            STD64507_AUTK_Recv_Event(pipe,mainOad,Comm_Info_Oop->pRcvBuf, (uint16)Comm_Info_Oop->RcvLen-2, *MtrPara, Comm_Info_Oop,pCbTask,DI07Index,DI07Num,RunTaskInfo);
                        else
                            STD64507_AUTK_Recv_Frz(pipe,mainOad,Comm_Info_Oop->pRcvBuf, (uint16)Comm_Info_Oop->RcvLen-2, *MtrPara, Comm_Info_Oop,pCbTask,RunTaskInfo);
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
                        STD64507_AUTK_Recv_Event(pipe,mainOad,Comm_Info_Oop->pRcvBuf, (uint16)Comm_Info_Oop->RcvLen-2, *MtrPara, Comm_Info_Oop,pCbTask,DI07Index,DI07Num,RunTaskInfo);
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
                            STD64507_AUTK_Recv(pipe,mainOad,pCbTask->CsdTask.RoadTask[CsdIndex].OadListInfo[SubOadIndex],CombinBuff, *CombinLen, *MtrPara,NULL, pCbTask,RunTaskInfo);
                        }
                        else
                        {
                            if(mainOad.nObjID == 0x5002)
                                STD64507_AUTK_Recv_Profile(pipe,mainOad,CombinBuff, *CombinLen, *MtrPara, pCbTask,RunTaskInfo);
                            else if((mainOad.nObjID & 0xF000)== 0x3000)
                            {
                            }
                            else
                                STD64507_AUTK_Recv_Frz(pipe,mainOad,CombinBuff, *CombinLen, *MtrPara, Comm_Info_Oop,pCbTask,RunTaskInfo);
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
            if(ret==2)
            {
                DealEventReport(pipe,pCbTask->MsInfo[MeterIndex].Prtl,pn);

           }
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
    Deal_Receive_Result_645_07(Comm_Info_Oop,pCbTask,RunTaskInfo,actResult,pipe);
}

/****************************************************************************
*函数名称：Deal_Receive_Result_645_07
*功能：处理  645_07抄读接收报文的结果
*输入参数：
*输出参数：
*返回：
****************************************************************************/

void Protector_645_07_Recvive_Data(SCHEME_TASK_RUN_INFO *pCbTask,CTS_RUN_TASK_T *RunTaskInfo, uint8 pipe, S_COMM_INFO_MET *Comm_Info_Oop,cbMtrPara *MtrPara,uint8 *DI07Index,uint8 DI07Num,uint8 *CombinBuff,uint16 *CombinLen,uint32 *DI07Buff)
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
                                STD64507_AUTK_Recv_Profile(pipe,mainOad,Comm_Info_Oop->pRcvBuf, (uint16)Comm_Info_Oop->RcvLen-2, *MtrPara, pCbTask,RunTaskInfo);
                            }
                        }
                        else if((mainOad.nObjID & 0xF000)== 0x3000)
                            STD64507_AUTK_Recv_Event(pipe,mainOad,Comm_Info_Oop->pRcvBuf, (uint16)Comm_Info_Oop->RcvLen-2, *MtrPara, Comm_Info_Oop,pCbTask,DI07Index,DI07Num,RunTaskInfo);
                        else
                            STD64507_AUTK_Recv_Frz(pipe,mainOad,Comm_Info_Oop->pRcvBuf, (uint16)Comm_Info_Oop->RcvLen-2, *MtrPara, Comm_Info_Oop,pCbTask,RunTaskInfo);
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
                        STD64507_AUTK_Recv_Event(pipe,mainOad,Comm_Info_Oop->pRcvBuf, (uint16)Comm_Info_Oop->RcvLen-2, *MtrPara, Comm_Info_Oop,pCbTask,DI07Index,DI07Num,RunTaskInfo);
                        else
                        STD2007_FrameCombin(mainOad,pCbTask->CsdTask.RoadTask[CsdIndex].OadListInfo[SubOadIndex],DI07,*DI07Index,Comm_Info_Oop->pRcvBuf, (uint16)Comm_Info_Oop->RcvLen, CombinBuff, CombinLen);
                    }
                    (*DI07Index)++;
                    if(*DI07Index>=DI07Num)
                    {
                        *DI07Index = 0;
                        if(pCbTask->CsdTask.RoadTask[CsdIndex].MainOad.nObjID==0x0000 || pCbTask->AcqInfo.Acqtype == ACQTYP_NULL)
                        {
                            memset(&mainOad,0x00,sizeof(OOP_OAD_U));
                            Protector_AUTK_Recv(pipe,mainOad,pCbTask->CsdTask.RoadTask[CsdIndex].OadListInfo[SubOadIndex],CombinBuff, *CombinLen, *MtrPara,NULL, pCbTask,RunTaskInfo);
                        }
                        else
                        {
                            if(mainOad.nObjID == 0x5002)
                                STD64507_AUTK_Recv_Profile(pipe,mainOad,CombinBuff, *CombinLen, *MtrPara, pCbTask,RunTaskInfo);
                            else if((mainOad.nObjID & 0xF000)== 0x3000)
                            {
                            }
                            else
                                STD64507_AUTK_Recv_Frz(pipe,mainOad,CombinBuff, *CombinLen, *MtrPara, Comm_Info_Oop,pCbTask,RunTaskInfo);
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
            //if(ret==2)
            //{
                //DealEventReport(pipe,pCbTask->MsInfo[MeterIndex].Prtl,pn);

           //}
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
    Deal_Receive_Result_Protector_645_07(Comm_Info_Oop,pCbTask,RunTaskInfo,actResult,pipe);
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

uint8 STD2007_CheckRspFrame(uint8 pipe, uint8 *pBuf, uint16 rcvLen, uint8 bStdType,S_COMM_INFO_MET* pCommInfo)
{
    uint8   frameCs;
    uint16  frameLen;
    //比较返回地址
    if(memcmp(pCommInfo->pMtrPara->MetAddr, &pBuf[1], 6) != 0)
        return 0;

    frameLen = 0x0c + pBuf[9];
    frameCs = CalCs(pBuf, frameLen - 2);

    if((pBuf[0] == STD2007_SYNC)
            && (pBuf[7] == pBuf[0]) /*-北京系统电表地址补0或AA可变-*/
            && (pBuf[frameLen - 2] == frameCs)
            && (pBuf[frameLen - 1] == 0x16))
    {
        switch(frameLen)
        {
        case 0x0C:
            if((pBuf[8] == 0x8A) && (pBuf[9] == 0x00)) /*-写设备地址正常应答帧-*/
            {
                return 3;
            }
            else
            {
                return 0;
            }
        case 0x0D:
            if((pBuf[8] & 0xF0) == 0xD0)        /*-异常应答帧-*/
            {
                return 4;
            }
            else
            {
                if((pBuf[8] == 0x8C) && (pBuf[10] != 0xFF)) /*-更改波特率确认应答帧-*/
                {
                    return 3;
                }
                else
                {
                    if((pBuf[8] == 0x8C) && (pBuf[10] == 0xFF)) /*-更改波特率错误应答帧-*/
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
                if(0x90 == (pBuf[8] & 0xF0))        /*-无后续数据帧-*/
                {
                    return 1;
                }
                else
                {
                    if(0xB0 == (pBuf[8] & 0xF0))    /*-有后续数据帧-*/
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

#if TASK_EN_CURVE == 1 || TASK_EN_MIN60 == 1
uint8 cbtasknew_STD2007_AUTK_Recv_Curve(uint8 pipe, uint8 *pBuf, uint16 rcvLen, cbMtrPara mCbMtrPara, uint8 bStdType, S_cbTaskQueue* pCurTaskQueue, enum_cbtask_id taskid,uint8 BCNum)
{
    const S_CbIDInfo *pCbTaskID = pCurTaskQueue->pQueueTaskID;
    uint32          FraDI = 0xffffffffL;
    uint8           idNum = 0;                      /*-表示一帧报文能够读取的数据ID数量-*/
    uint8           fraOffs = STD2007_ADDR_SITE;    /*-数据项在返回数据帧中的偏移地址-*/
    uint8           i = 0, j = 0;
    uint16          offset = 0;
    BOOL            blnFrameCheck = FALSE;      //报文格式校验是否正确
    dataFormat      CurveData[30];
    uint16          idOffMap[30];
    uint16          index = 0;
    time_struct     point_time;                 //曲线回码时间

    //回码中的数据标识,进行数据存放，而不是根据组帧时的数据标识
    memcpy((void*)&FraDI, (void*)&pBuf[10], sizeof(uint32));
    rcvLen = 0x0C + pBuf[9];            //有效报文长度
    ReduceGap(&pBuf[10],  pBuf[9], STD2007_GAP_VALUE);      //减0x33

    //查找当前数据项对应的数据项列表
    memset(idOffMap, 0, sizeof(idOffMap));
    memset(CurveData, 0, sizeof(CurveData));
    memset(&point_time, 0, sizeof(point_time));

    offset = 0;
    while(cbtasknew_STD2007_FRADI_MAP[offset].bDataId != 0xffff)
    {
        if(FraDI == cbtasknew_STD2007_FRADI_MAP[offset].FraDI)
        {
            idOffMap[idNum] = offset;
            CurveData[idNum].DataId = cbtasknew_STD2007_FRADI_MAP[offset].bDataId;
            CurveData[idNum].DataId2 = 0xFFFF;
            CurveData[idNum].type = cbtasknew_STD2007_FRADI_MAP[offset].type;
            CurveData[idNum].DataNum = cbtasknew_STD2007_FRADI_MAP[offset].dataNum;
            CurveData[idNum].SrcDataLen = cbtasknew_STD2007_FRADI_MAP[offset].srcDataLen;
            CurveData[idNum].SrcDecLen = cbtasknew_STD2007_FRADI_MAP[offset].srcDecLen;
            CurveData[idNum].Rate = cbtasknew_STD2007_FRADI_MAP[offset].rate;
            CurveData[idNum].srcTodes = cbtasknew_STD2007_FRADI_MAP[offset].srcTodes;
            if(++idNum >= cbtasknew_STD2007_FRADI_MAP[offset].DINum)
                break;
        }
        offset++;
    }

    if(0x06000001 == FraDI)
    {
        //检查报文合法性
        if(pBuf[fraOffs] == 0xA0 && pBuf[fraOffs + 1] == 0xA0)      //负荷记录起始码：A0H，A0H（或E0H，E0H：本数据块不正确），2字节
        {
            //一下判断代码暂时屏蔽，防止和电表判断规则不一致，导致无法抄表 by gujun 2013-06-17
            //uint8 bCurveCS = 0;                           //负荷记录校验码
            //uint8 bCurveDateLen = 0;                      //负荷记录字节数
            //bCurveDateLen = pBuf[fraOffs + 2];            //负荷记录字节数
            //for(i = 0; i < (bCurveDateLen + 2 + 1); i++)
            //  bCurveCS += pBuf[fraOffs + i];
            //if(bCurveCS == pBuf[bCurveDateLen + 2 + 1] && pBuf[bCurveDateLen + 2 + 1 + 1] == 0xE5)            //负荷记录累加校验码正确  负荷记录结束码正确
            {
                blnFrameCheck = TRUE;
            }
        }

        if(blnFrameCheck == TRUE)       //报文合法，非法报文不需要处理
        {
            //负荷记录存储时间：    年、月、日、时、分, 5字节
            fraOffs += 3;
            memcpy(&point_time.minute, &pBuf[fraOffs], 5);
            WriteMtrXData(mCbMtrPara.MtrNum, RT_DATA|WF_RAMONLY, POINT_BC_TIME_ID, (char*)&point_time);
            if (getenv(STD2007_ENV))
                printf("0x06000001  曲线回码时间: %02X-%02X-%02X %02X:%02X %x\n", pBuf[fraOffs + 4], pBuf[fraOffs + 3], pBuf[fraOffs + 2], pBuf[fraOffs + 1], pBuf[fraOffs + 0],mCbMtrPara.MtrNum);

            fraOffs += 5;
            for(i = 0; i < idNum; i++)      /*块抄的数据项个数*/
            {
                uint8 bBlockNum = cbtasknew_STD2007_FRADI_MAP[idOffMap[i]].BlockNum;        //两个分隔符（0xAA）之间的ID数量
                if (getenv(STD2007_ENV))
                    printf("i = %d  bBlockNum = %d      bOffSign = %d\n", i, cbtasknew_STD2007_FRADI_MAP[idOffMap[i]].BlockNum, cbtasknew_STD2007_FRADI_MAP[idOffMap[i]].bOffSign);

                index = 0;
                if(bBlockNum != 0 && pBuf[fraOffs] == 0xAA)     //块分隔码：            AAH，1字节
                {
                    for(j = 0; j < bBlockNum; j++)
                    {
                        index = 0;
                        if(cbtasknew_IsInTaskId(pipe, CurveData[i + j].type, CurveData[i + j].DataId, pCbTaskID, &index, taskid))
                            cbtasknew_DataToXBBF(AUTKTAG_FAIL, pipe, index, 0, &CurveData[i + j], taskid);      /*清除相应缓冲区数据,加上四费率的偏移量*/
                    }
                    i += (bBlockNum - 1);
                    fraOffs += 1;
                    continue;
                }
                else
                {
                    if(cbtasknew_IsInTaskId(pipe, CurveData[i].type, CurveData[i].DataId, pCbTaskID, &index, taskid))
                    {
                        CurveData[i].pSrcBuf = &pBuf[fraOffs];
                        FormatToStd2007(CurveData[i].DataId, &CurveData[i], bStdType, pCurTaskQueue->QueueTaskType, mCbMtrPara.MtrNum);
                        cbtasknew_DataToXBBF(AUTKTAG_SUCCESS, pipe, index, 0, &CurveData[i], taskid);       /*清除相应缓冲区数据,加上四费率的偏移量*/
                    }
                    fraOffs += CurveData[i].SrcDataLen;
                }

                if(cbtasknew_STD2007_FRADI_MAP[idOffMap[i]].bOffSign == 1)          //后面是否有分隔符 目前用于曲线数据
                {
                    if(pBuf[fraOffs] == 0xAA)       //块分隔码：            AAH，1字节
                        fraOffs += 1;
                    else
                        break;      //如果不是0xAA的话，表示解码完毕，或者解码出错
                }
            }
        }
    }
    else if(0x0504FF01 == FraDI)
    {
        //负荷记录存储时间：    年、月、日、时、分, 5字节
        memcpy(&point_time.minute, &pBuf[fraOffs], 5);
        WriteMtrXData(mCbMtrPara.MtrNum, RT_DATA|WF_RAMONLY, POINT_BC_TIME_ID, (char*)&point_time);
        if (getenv(STD2007_ENV))
            printf("0x0504FF01  曲线回码时间: %02X-%02X-%02X %02X:%02X\n", pBuf[fraOffs + 4], pBuf[fraOffs + 3], pBuf[fraOffs + 2], pBuf[fraOffs + 1], pBuf[fraOffs + 0]);

        fraOffs += 5;
        for(i = 0; i < idNum; i++)      /*块抄的数据项个数*/
        {
            if(cbtasknew_IsInTaskId(pipe, CurveData[i].type, CurveData[i].DataId, pCbTaskID, &index, taskid))
            {
                CurveData[i].pSrcBuf = &pBuf[fraOffs];
                FormatToStd2007(CurveData[i].DataId, &CurveData[i], bStdType, pCurTaskQueue->QueueTaskType, mCbMtrPara.MtrNum);
                cbtasknew_DataToXBBF(AUTKTAG_SUCCESS, pipe, index, 0, &CurveData[i], taskid);       /*清除相应缓冲区数据,加上四费率的偏移量*/
            }
            fraOffs += CurveData[i].SrcDataLen;
        }
    }


    return TRUE;
}
#endif



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
void STD2007_MakeFrameEx(uint8 *pMetAddr, uint32 fraDI, uint8 cmdDI, uint8 *pBuf, uint8 *pSndLen, S_COMM_INFO_MET *pComm_Info,SCHEME_TASK_RUN_INFO *pCbTask, CTS_RUN_TASK_T *RunTaskInfo)
{
    uint8 tmpSnd[30];
    uint8 i;
    time_t startTm = pComm_Info->cbLPPara.startTime;
    uint8 CsdIndex;

    CsdIndex=RunTaskInfo->CsdIndex;
    memset(tmpSnd, 0x00, sizeof(tmpSnd));
    for(i = 0; i < 4; i++)
        tmpSnd[i] = 0xFE;
    tmpSnd[4] = 0x68;
    memcpy(&tmpSnd[5], pMetAddr, 6); /*- 电表地址 -*/
    tmpSnd[11] = 0x68;
    /*-正常国标组帧-*/
    tmpSnd[12] = cmdDI;//控制码

    switch(cmdDI)
    {
    case STD2007_CMDID_READ:
    case STD2007_CMDID_READNEXT:
        tmpSnd[13] = 0x04;
        break;
    case STD2007_CMDID_REREAD:
        tmpSnd[13] = 0x00;
        break;
    default:
        tmpSnd[13] = 0x04;
    }
    if(pCbTask->CsdTask.RoadTask[CsdIndex].MainOad.nObjID==0 || pCbTask->AcqInfo.Acqtype == ACQTYP_NULL)//OAD
    {
        //不需要处理
    }
    else//ROAD
    {
        if(pCbTask->CsdTask.RoadTask[CsdIndex].MainOad.nObjID != 0x5002)//分钟曲线
        {
            if(pCbTask->SchemeType != EVENTPLAN)
            {
                if(pCbTask->CsdTask.RoadTask[CsdIndex].MainOad.nObjID == 0x5004)
                {
                    if((fraDI &0xffff0000) == 0x05060000)
                        fraDI = (fraDI&0xFFFFFF00)|pComm_Info->cbLPPara.startNum;//modify the last n times
                }
                else
                    fraDI = (fraDI&0xFFFFFF00)|pComm_Info->cbLPPara.startNum;//modify the last n times
            }
        }
    }
    switch(fraDI)//assume DI is 00fe0000
    {
    case 0x06000001:        //给定时间曲线数据
    {
        DateTimeBcd_t cbCurBTime;

        tmpSnd[13] = 0x0a;          //长度
        memcpy(&tmpSnd[14], (uint8 *)&fraDI, sizeof(uint32));   /*- 项目表识编码 -*/
        tmpSnd[18] = 0x01;          //块数

        DT_Time2DateTimeBCD(startTm,&cbCurBTime);
        memcpy_r(tmpSnd+19,&cbCurBTime.year_l,5);
        AddGap(&tmpSnd[14], tmpSnd[13], STD2007_GAP_VALUE);
        tmpSnd[24] = CalCs(&tmpSnd[4], 20);                 /*- 校验和 -*/
        tmpSnd[25] = 0x16;
        tmpSnd[26] = 0x00;
        tmpSnd[27] = 0x00;

        memcpy(pBuf, tmpSnd, 26);               /*- 电表发送缓冲区pBuf*/
        *pSndLen = 26; /*-有的表不支持后面多两个0   2006-7-21 10:03-*/

    }
    break;
    default:
    {
        memcpy(&tmpSnd[14], (uint8 *)&fraDI, sizeof(uint32));   /*- 项目表识编码 -*/
        AddGap(&tmpSnd[14], 4, STD2007_GAP_VALUE);
        tmpSnd[18] = CalCs(&tmpSnd[4], 14);                 /*- 校验和 -*/
        tmpSnd[19] = 0x16;
        tmpSnd[20] = 0x00;
        tmpSnd[21] = 0x00;

        memcpy(pBuf, tmpSnd, 20);               /*- 电表发送缓冲区pBuf*/
        *pSndLen = 20; /*-有的表不支持后面多两个0   2006-7-21 10:03-*/
        break;
    }

    }

    return;
}
/****************************************************************************
*函数名称：STD2007_LoadPrifle_Init
*功能：  645_07抄读的任务开始时间处理
*输入参数：
*输出参数：
*返回：
****************************************************************************/

void STD2007_LoadPrifle_Init(cbLPPara_t *pCbLpPara ,SCHEME_TASK_RUN_INFO *pCbTask,CTS_RUN_TASK_T   *RunTaskInfo)
{
    if(pCbTask->CsdTask.RoadTask[0].MainOad.value !=0x00000000)//
    {
        DateTimeBcd_t tmpTime;
        OOP_TI_T ti;

        #if((1 == OOP_CRV_ADDREAD_485)||(1 == OOP_DAY_ADDREAD_485))
        time_t OriStartTime = 0;
        #endif

        #ifdef AREA_SHANDONG
        OOP_TI_T delayTi;
		OOP_TI_T TaskTi;
		uint16 CsdIndex;
		uint16 taskid;

		taskid=pCbTask->TaskId;
		CsdIndex=RunTaskInfo->CsdIndex;
		 if((pCbTask->CsdTask.RoadTask[CsdIndex].MainOad.nObjID==0x5002)
    	||(pCbTask->CsdTask.RoadTask[CsdIndex].MainOad.nObjID==0x5003))//分钟曲线或小时曲线
		{
			get_task_run_frequence(taskid,&TaskTi);
			if((TaskTi.unit == pCbTask->AcqInfo.TI.unit)&&(TaskTi.value = pCbTask->AcqInfo.TI.value))
			{
		        memcpy_r((uint8*)&tmpTime,(uint8*)RunTaskInfo->ThisTurnBeginTime,7);
		        DT_DateTimeBCD2Time(&tmpTime,&pCbLpPara->startTime);
		        delayTi.value = pCbTask->DelayTime;
		        delayTi.unit = pCbTask->DelayTimeUnit;
		        DT_TimeSubInv(&pCbLpPara->startTime, delayTi);
		        memcpy_r((uint8*)&tmpTime,(uint8*)RunTaskInfo->ThisTurnEndTime,7);
		        DT_DateTimeBCD2Time(&tmpTime,&pCbLpPara->endTime);
			}
			else
			{
				memcpy_r((uint8*)&tmpTime,(uint8*)RunTaskInfo->LastTurnBeginTime,7);

		        DT_DateTimeBCD2Time(&tmpTime,&pCbLpPara->startTime);
		        memcpy_r((uint8*)&tmpTime,(uint8*)RunTaskInfo->ThisTurnBeginTime,7);

		        DT_DateTimeBCD2Time(&tmpTime,&pCbLpPara->endTime);
			}
    	}
		else
		{
				memcpy_r((uint8*)&tmpTime,(uint8*)RunTaskInfo->LastTurnBeginTime,7);

		        DT_DateTimeBCD2Time(&tmpTime,&pCbLpPara->startTime);
		        memcpy_r((uint8*)&tmpTime,(uint8*)RunTaskInfo->ThisTurnBeginTime,7);

		        DT_DateTimeBCD2Time(&tmpTime,&pCbLpPara->endTime);

		}
        #else
        memcpy_r((uint8*)&tmpTime,(uint8*)RunTaskInfo->LastTurnBeginTime,7);

        DT_DateTimeBCD2Time(&tmpTime,&pCbLpPara->startTime);
        memcpy_r((uint8*)&tmpTime,(uint8*)RunTaskInfo->ThisTurnBeginTime,7);

        DT_DateTimeBCD2Time(&tmpTime,&pCbLpPara->endTime);
        #endif
        ti.unit = pCbTask->DelayTimeUnit;
        ti.value = pCbTask->DelayTime;
        DT_TimeSubInv(&pCbLpPara->endTime, ti);
        pCbLpPara->tmInv.unit = pCbTask->AcqInfo.TI.unit;
        pCbLpPara->tmInv.value= pCbTask->AcqInfo.TI.value;
        //
        pCbLpPara->startNum = 1;
        pCbLpPara->endNum = 0;//不支持
        memset((uint8*)&pCbLpPara->storeTm,0x00,sizeof(pCbLpPara->storeTm));
        switch(pCbTask->CsdTask.RoadTask[0].MainOad.nObjID)
        {
        case 0x5000:
            pCbLpPara->endNum = 3;
            break;
        case 0x5001:
            pCbLpPara->startTime = time(NULL);
            break;
        case 0x5002:
            if(pCbLpPara->tmInv.value==0)
            {
                pCbLpPara->tmInv.unit = 1;
                pCbLpPara->tmInv.value = 15;//默认15分钟
            }
            pCbLpPara->endTime = pCbLpPara->endTime-1;
            #if(1 == OOP_CRV_ADDREAD_485)  //获取485曲线补抄的起始时间

            OriStartTime = pCbLpPara->startTime;
            pCbLpPara->startTime = GetCrvNewStartTime(OriStartTime, pCbTask->TaskId, pCbTask->SchemeId, pCbTask->MsInfo[RunTaskInfo->MeterIndex].Sn, 0);
            #endif
            break;
        case 0x5003:
            pCbLpPara->endNum = 48;//
            if(pCbLpPara->tmInv.value==0)
            {
                pCbLpPara->tmInv.unit = 1;
                pCbLpPara->tmInv.value = 30;//默认30分钟
            }
            pCbLpPara->endTime = pCbLpPara->endTime-1;
            break;
        case 0x5004:
            pCbLpPara->endNum = 3;//
            printf("pCbLpPara->startTime is %ld\n",pCbLpPara->startTime);
            if(pCbLpPara->tmInv.value==0)
            {
                pCbLpPara->tmInv.unit = 3;
                pCbLpPara->tmInv.value = 1;//默认1天
            }
            if((pCbTask->AcqInfo.Acqtype == ACQTYP_LASTN)&&(pCbTask->StoreTimeType == STRTMTYP_FRZTIME))
            {
                pCbTask->StoreTimeType = STRTMTYP_DAYSTART;
            }
            #if(1 == OOP_DAY_ADDREAD_485)  //获取485日冻结补抄的起始时间
            OriStartTime = pCbLpPara->startTime;
            printf("!!!!!!!!!pCbLpPara->startTime is %ld\n",pCbLpPara->startTime);
            pCbLpPara->startTime = GetDayNewStartTime(OriStartTime, pCbTask->TaskId, pCbTask->SchemeId, pCbTask->MsInfo[pCbTask->MeterIndex].Sn, 0);
            #endif
            break;
        case 0x5005:
        case 0x5006:
            pCbLpPara->endNum = 12;//
            if(pCbLpPara->tmInv.value==0)
            {
                pCbLpPara->tmInv.unit = 4;
                pCbLpPara->tmInv.value = 1;//默认1月
            }
            if((pCbTask->StoreTimeType == STRTMTYP_FRZTIME)||(pCbTask->StoreTimeType == STRTMTYP_STARTTM))
            {
                pCbTask->StoreTimeType = STRTMTYP_MONTHSTART;
            }
            if(pCbTask->AcqInfo.Acqtype == ACQTYP_FRZTM)
            {
                pCbTask->AcqInfo.Acqtype = ACQTYP_LASTN;
                pCbTask->AcqInfo.LastN = 1;
            }
            break;
        case 0x5007://暂定为定时冻结
            pCbLpPara->endNum = 60;
            break;
        case 0x5008:
        case 0x5009:
        case 0x500A:
        case 0x500B:
        case 0x5011:
            pCbLpPara->endNum = 2;
            break;
        default:
            pCbLpPara->endNum = 0;
            break;

        }
        if(pCbTask->AcqInfo.Acqtype == ACQTYP_LASTN)
        {
            pCbLpPara->startNum = pCbTask->AcqInfo.LastN;
            if(pCbLpPara->startNum==0)
                pCbLpPara->startNum = 1;
            pCbLpPara->endNum = pCbLpPara->startNum;
            if(pCbTask->CsdTask.RoadTask[0].OadListInfo[RunTaskInfo->CsdIndex].nObjID==0x5002)
            {
                //特殊处理默认按15分钟间隔
                pCbLpPara->tmInv.unit = 1;
                pCbLpPara->tmInv.value = 15;//默认15分钟
                while(pCbLpPara->startNum>1)
                {
                    DT_TimeSubInv(&pCbLpPara->endTime,pCbLpPara->tmInv);
                    pCbLpPara->startNum--;
                }
                pCbLpPara->startTime = pCbLpPara->endTime;

            }
        }
        else if(pCbTask->AcqInfo.Acqtype == ACQTYP_NULL)
        {
            pCbLpPara->startNum = 0;
            pCbLpPara->endNum = 0;
            pCbLpPara->startTime = time(NULL);//只采集一次
        }

    }
    else
    {
        pCbLpPara->startTime = time(NULL);
        pCbLpPara->tmInv.value = 15;
    }
}

/****************************************************************************
*模块编号：
*名称：STD2007_FrameCombin
*功能：1个OAD对应多个07数据项,需要把这些数据项对应的数据拼凑一起然后处理
*输入参数：
*输出参数：
*处理：
*返回：无
****************************************************************************/
BOOL STD2007_FrameCombin(OOP_OAD_U mainOad,OOP_OAD_U suboad,uint32 DI07,uint8 DI07Index,uint8 *inData,uint8 inLen,uint8 *outData,uint16 *outLen)
{
    uint16               offSet=14;

    const MapOopTo645_NEW *pMapOopTo645;

    uint16               i;
    uint16               desOffset=14;
    uint8                ItemLen=0;
    inLen = inData[9]+0x0A;
    //根据从OAD 查找645对应表
    if(mainOad.nObjID == 0x0000)
        desOffset = 0;

    pMapOopTo645 = Find645InfoPtrByOad(mainOad,suboad);//
    if(pMapOopTo645==NULL)
    {
        return FALSE;
    }

    for(i=0; i<DI07Index; i++)
    {
        desOffset += pMapOopTo645->TotalDataLen;
        pMapOopTo645+=1;
    }
    if(outData!=NULL)
    {
        ItemLen = pMapOopTo645->TotalDataLen;
        memcpy(outData+desOffset,inData+offSet,ItemLen);

        desOffset+=ItemLen;
    }
    if(outLen!=NULL)
        *outLen = desOffset;

    return TRUE;
}
/*-
***************************************************************************
*模块编号：
*名称：数据区数据减0x33
*功能：数据区数据减0x33
*输入参数：数据缓冲区首地址(*pRecvBuf)、数据字节数(offs)、需要减去的数值(gapValue)
*输出参数：处理后的缓冲区
*处理：将数据缓冲区中offs个字节减去gapValue
*返回：无
***************************************************************************
-*/
void ReduceGap(uint8 *pRecvBuf, uint8 offs, uint8  gapValue)
{
    uint8 i;

    if(offs == 0) return;

    for(i = 0; i < offs; i++)
    {
        *(pRecvBuf + i) -= gapValue;
    }
}



/****************************************************************************
名称： setStdInfo_07
功能：
-           设置解析的目标格式
处理流程：
-           1、根据OI设置特殊格式
输入参数：
-           StdData             数据格式
-           pMapOopTo645        645配置表信息
-           rcvLen              接收的长度
输出参数：
-           无
返回：
-           是否正常组帧
****************************************************************************/
/*void setStdInfo_07(dataFormat *StdData,MapOopTo645_t pMapOopTo645,OadInfoList_t *pOadInfo,uint8 *pBuf,uint16 rcvLen,uint16 MainOI)
{
    uint8 tmpBuff[12];
    StdData->SrcDataLen = pMapOopTo645.DataLen;//源数据长度
    StdData->scaler = pMapOopTo645.Scaler;//源数据换算
    StdData->SrcDataType = pMapOopTo645.DataType;//源数据类型

    StdData->desDataLen = pOadInfo->dataLen;//目标数据长度
    StdData->scaler -= pMapOopTo645.DesScaler;//目标换算 = 源换算-目标换算
    StdData->DesDataType = pMapOopTo645->DesDataType; //目标数据类型;
    if((MainOI&0xf000) == 0x3000)  //事件中的时间不进行处理
        return;
    switch(pOadInfo->oad.OI)//一些拼帧需要特殊处理
    {
    case 0x4000:
        tmpBuff[1] = *(pBuf+4);
        tmpBuff[2] = *(pBuf+5);
        tmpBuff[3] = *(pBuf+6);
        tmpBuff[4] = *(pBuf+1);
        tmpBuff[5] = *(pBuf+2);
        tmpBuff[6] = *(pBuf+3);
        memcpy(pBuf,tmpBuff,rcvLen);
        StdData->SrcDataLen = rcvLen;
        StdData->SrcDataType = e645DATETIME_S;
        break;
    default:
        break;
    }
    return;
}*/
/****************************************************************************
名称： STD64507_AUTK_Recv_One
功能：
-           64507单相表负荷曲线解析，并按照采集方案保存数据
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
uint16 STD64507_AUTK_Recv_One(uint8 pipe,OOP_OAD_U oad,uint8 *pBuf, uint16 rcvLen, cbMtrPara mCbMtrPara, S_COMM_INFO_MET *pComm_Info,SCHEME_TASK_RUN_INFO *pCbTask,CTS_RUN_TASK_T *RunTaskInfo)
{
    uint8 offSet=14;
    uint16 CsdIndex;
    uint16 SubOadIndex;
    DateTimeBcd_t storeTm;
    memcpy_r(&storeTm.year_l,&pBuf[offSet],5);
    ReduceGap(&storeTm.year_l, 5, STD2007_GAP_VALUE);
    storeTm.year_h = 0x20;
    storeTm.sec = 0x00;
    CsdIndex=RunTaskInfo->CsdIndex;
    SubOadIndex=RunTaskInfo->SubOadIndex;
    //存储冻结时标
    if(pCbTask->CsdTask.RoadTask[CsdIndex].OadListInfo[0].nObjID == 0x2021)
        STD64507_AUTK_Recv(pipe,oad,pCbTask->CsdTask.RoadTask[CsdIndex].OadListInfo[0],&pBuf[offSet],rcvLen-offSet,mCbMtrPara,&storeTm,pCbTask,RunTaskInfo);
    //存储数据
    offSet += 5;
    STD64507_AUTK_Recv(pipe,oad,pCbTask->CsdTask.RoadTask[CsdIndex].OadListInfo[SubOadIndex],&pBuf[offSet],rcvLen-offSet,mCbMtrPara,&storeTm,pCbTask,RunTaskInfo);
    return 0;
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
uint16 STD64507_AUTK_Recv(uint8 pipe,OOP_OAD_U mainOAD,OOP_OAD_U subOAD,uint8 *pBuf, uint16 rcvLen, cbMtrPara mCbMtrPara, DateTimeBcd_t *StoreTime,SCHEME_TASK_RUN_INFO *pCbTask,CTS_RUN_TASK_T *RunTaskInfo)
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
    memcpy(sourcedata.sourcebuffer,tmpBufer,rcvLen);
    sourcedata.reclen=rcvLen;
    sourcedata.pwrType = basicinfo.pwrType;
    Convert_6457_07_To_Struct(sourcedata ,desbuffer,&desLen,pMapOopTo645);
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
    TASK_FMT_TRACE ( pipe,REC_LOG, "存储数据 colStoreTm %d\n",StoreInfo.colStoreTm);
    TASK_BUF_TRACE ( pipe,REC_LOG, desbuffer,desLen);
    //如果是实时数据 就缓存起来一起存
    if(mainOAD.value == 0 || patch_policy_flg[pipe] > 0)
    {
        TASK_FMT_TRACE ( pipe,REC_LOG, "缓存数据\n");
        taskmng_rs485_cache_buf_save(pipe,pn,mainOAD,subOAD,desbuffer,desLen,colStartTm,longTm,&gAmrThreadArg[pipe].meterData[0]);
    }else
    {
        taskmng_meter_data_save(desbuffer,desLen,StoreInfo,pipe);
    }
        /*240609 存储07表停电事件的时候判一下结束时间  */
    if(mainOAD.nObjID == 0x3011 && subOAD.nObjID == 0x2020 && desLen == sizeof(OOP_DATETIME_S_T))
    {
        OOP_DATETIME_S_T powerontime;
        memcpy(&powerontime,desbuffer,desLen);
        time_t starttime,endtime,tmptime;
        uint8 tmpbuff[7] = {0};
        uint8 addr[6]={0};
        memcpy_r(tmpbuff, (uint8 *) RunTaskInfo->LastTurnBeginTime, 7);
        DT_DateTimeBCD2Time((DateTimeBcd_t *) tmpbuff, &starttime);
        memcpy_r(tmpbuff, (uint8 *) RunTaskInfo->ThisTurnBeginTime, 7);
        DT_DateTimeBCD2Time((DateTimeBcd_t *) tmpbuff, &endtime);
        DT_DateTimeShort2Time(&powerontime,&tmptime);
        TASK_FMT_TRACE(pipe, REC_LOG, "StartTime is %d,EndTime is %d,tmpTime is %d\n", starttime, endtime, tmptime);
        //240711 台体给的上电时间正好是0点0分 和698改动一样 改为当前时间
        //if(tmptime >starttime && tmptime < endtime)
        if(tmptime >starttime && tmptime < time(NULL))
        {
            TASK_FMT_TRACE(pipe, REC_LOG, "pn %d 新发生上电\n", pn);
            memcpy_r(addr,basicinfo.tsa.add,6);
            taskmng_meter_time_add(pipe,pn,basicinfo.protocol,addr);
        }
    }

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
uint16 STD64507_AUTK_Recv_Profile(uint8 pipe,OOP_OAD_U oad,uint8 *pBuf, uint16 rcvLen, cbMtrPara mCbMtrPara, SCHEME_TASK_RUN_INFO *pCbTask,CTS_RUN_TASK_T *RunTaskInfo)
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
        #ifdef AREA_SHANDONG
        if(pCbTask->Isenergyoad ==TRUE)
        {
            subOadNum = pCbTask->energynum;
        }
		else
		{
			if(pCbTask->energynum ==0)
			{
				subOadNum = 1;
			}
		}
        #endif
        while(i<subOadNum)
        {
            uint16 bias = CaclBiasByOad(oad,pCbTask->CsdTask.RoadTask[CsdIndex].OadListInfo[i]);
            //TASK_FMT_TRACE ( pipe,REC_LOG, "在06000001中的bias is %d,oad.value is %x,pCbTask->CsdTask.RoadTask[CsdIndex].OadListInfo[i].value is %x\n",bias,oad.value,pCbTask->CsdTask.RoadTask[CsdIndex].OadListInfo[i].value);
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

#if(1 == OOP_DAY_ADDREAD_485)
//根据电表的日冻结时标确定是否继续补抄
/*int IsNeedPatchDayMet07ByFrzTime(DateTimeBcd_t bfrztm,DateTimeBcd_t storeTm,cbMtrPara mCbMtrPara, S_COMM_INFO_MET *pComm_Info,SCHEME_TASK_RUN_INFO *pCbTask)
{
    int              ret;
    uint16           innerOffset = 0;
    OadQueryInfo_t   oadQueryInfo;
    PlanTypeInfo_t   planTypeInfo;
    ReadOutData_t    readOutData;
    uint8            OutDataBuf[1024];
    time_t           lststart = 0;
    time_t           nowstart = 0;
    time_t           frztm = 0;
    DateTimeBcd_t    tmpTime;

    DT_DateTimeBCD2Time(&bfrztm,&frztm);
    memcpy_r((uint8*)&tmpTime,pCbTask->LastTurnBeginTime,7);
    DT_DateTimeBCD2Time(&tmpTime,&lststart);
    nowstart = pComm_Info->cbLPPara.endTime;

    memset(&oadQueryInfo,0x0,sizeof(oadQueryInfo));
    memset(&readOutData,0x0,sizeof(readOutData));
    if(pComm_Info->subOADIndex == 0)
    {
        if(frztm > lststart && frztm <= nowstart)
            pCbTask->pCurMtr->needCBagain = FALSE;//抄成功不需要补抄

        if((pComm_Info->subOADIndex + 1) >= pCbTask->pCurCSDNode->Road.SubOadNum)
            return 0;

        oadQueryInfo.metSn = mCbMtrPara.MtrNum;
        oadQueryInfo.sOad = pCbTask->pCurCSDNode->Road.SubOad[pComm_Info->subOADIndex+1];
        oadQueryInfo.queryTime = storeTm;
        oadQueryInfo.needReadTimeTag = TRUE;
        memcpy(&planTypeInfo.planMainOad,&pCbTask->pCurCSDNode->Road.MainOad,sizeof(SOAD));
        planTypeInfo.planType = NORMALPLAN;
        readOutData.OutDataBuf = OutDataBuf;
        //根据存储的时标点判断是否需要继续补抄
        ret = ReadPlanMtrXData(planTypeInfo, oadQueryInfo, &readOutData);
        if(ret > 0 && taskmng_oop_next_read_data_get(readOutData.OutDataBuf, &innerOffset, ret, NULL_DATA, FALSE))
        {
            pComm_Info->subOADIndex = pCbTask->pCurCSDNode->Road.SubOadNum;//不需要进行补抄
        }
    }
    return 0;
}*/
#endif
/****************************************************************************
*函数名称：STD64507_AUTK_Recv_Frz
*功能：处理  645_07抄读返回的冻结数据
*输入参数：
*输出参数：
*返回：
****************************************************************************/

uint16 STD64507_AUTK_Recv_Frz(uint8 pipe,OOP_OAD_U oad,uint8 *pBuf, uint16 rcvLen, cbMtrPara mCbMtrPara, S_COMM_INFO_MET *pComm_Info,SCHEME_TASK_RUN_INFO *pCbTask,CTS_RUN_TASK_T *RunTaskInfo)
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

        //按照冻结时标采集，需要判断数据是否在时间间隔范围内
        if(RunTaskInfo->SubOadIndex==0)
        {

            if(pCbTask->CsdTask.RoadTask[CsIndex].OadListInfo[SubOadIndex].nObjID != 0x2021)//第一列必须是冻结时标
            {

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
            storeTm.min= 0x00;

            TASK_FMT_TRACE ( pipe,REC_LOG, "patch_policy_flg: %u \n",patch_policy_flg[pipe]);
            TASK_FMT_TRACE ( pipe,REC_LOG, "pCbTask->CsdTask.RoadTask[CsIndex].MainOad.nObjID: %04x\n", pCbTask->CsdTask.RoadTask[CsIndex].MainOad.nObjID);
            if(patch_policy_flg[pipe] == 1 && pCbTask->CsdTask.RoadTask[CsIndex].MainOad.nObjID == 0x5004)
            {
                TASK_FMT_TRACE ( pipe,REC_LOG, "不判断应答时间是否合法：\n");
                time_t tmp_starttime = pComm_Info->cbLPPara.startTime + 86400;
                DT_Time2DateTimeBCD(tmp_starttime,&storeTm);
            }
            else if(patch_policy_flg[pipe] == 2 && pCbTask->CsdTask.RoadTask[CsIndex].MainOad.nObjID == 0x5004)
            {
                TASK_FMT_TRACE ( pipe,REC_LOG, "不判断应答时间是否合法：\n");
                longTm = pComm_Info->cbLPPara.startTime;
                //DT_Time2DateTimeBCD(tmp_starttime,&storeTm);
            }
            else
            {

                if(DT_DateTimeBCD2Time(&storeTm,&longTm)==FALSE)//应答的时间不合法
                {
                    //不存储
                    pComm_Info->cbLPPara.endNum = 0;
                   return 0;
                }
            }

            //commfun_printstring((uint8*)&storeTm,7);//打印采集到的存储时间
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
                    if(patch_policy_flg[pipe] == 0)
                    {
                    if(longTm <= pComm_Info->cbLPPara.startTime)
                    {

                        //冻结时标比起始时间还小，不需要继续搜索了
                        #ifdef AREA_HUNAN
                        if(pCbTask->CsdTask.RoadTask[CsIndex].MainOad.nObjID == 0x5004)
                        {
                            pCbTask->AcqInfo.Acqtype = ACQTYP_NULL;//冻结时标比起始时间还小,直接转实时
                        }
                        #endif
                        pComm_Info->cbLPPara.endNum = 0;
                        #if(1 == OOP_DAY_ADDREAD_485)
                        if(pCbTask->CsdTask.RoadTask[CsIndex].MainOad.nObjID == 0x5004)
                        {
                            pComm_Info->cbLPPara.storeTm = storeTm;//保存存储时标
                        }
                        #else
                        if(pCbTask->CsdTask.RoadTask[CsIndex].MainOad.nObjID == 0x5004)
                        {

                        }
                        #endif
                        return 0;
                    }
                    }
                }

                if(patch_policy_flg[pipe] == 0)
                {
                if(longTm > pComm_Info->cbLPPara.endTime)
                {
                    //不在采集的时间间隔类，不存储，继续搜索上一条记录
                    return 0;
                }
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

            }
        }
        #endif
        storeTm = pComm_Info->cbLPPara.storeTm;

        TASK_FMT_TRACE(pipe, REC_LOG,"存储时间:%02x%02x-%02x-%02x %02x:%02x:%02x\n", storeTm.year_h, storeTm.year_l,
             storeTm.month, storeTm.day, storeTm.hour, storeTm.min, storeTm.sec);
    }
    else
    {
        uint8 tmpbuff[7];
        memcpy_r(tmpbuff,(uint8*)RunTaskInfo->ThisTurnBeginTime,7);
        DT_DateTimeBCD2Time((DateTimeBcd_t*)tmpbuff,&longTm);
        storeTm = taskmng_acq_store_time_get(pCbTask->StoreTimeType,longTm);
        TASK_FMT_TRACE(pipe, REC_LOG,"存储时间:%02x%02x-%02x-%02x %02x:%02x:%02x\n", storeTm.year_h, storeTm.year_l,
             storeTm.month, storeTm.day, storeTm.hour, storeTm.min, storeTm.sec);

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
    TASK_FMT_TRACE ( pipe,REC_LOG, "存储数据\n");
    STD64507_AUTK_Recv(pipe,oad,pCbTask->CsdTask.RoadTask[CsIndex].OadListInfo[SubOadIndex],&pBuf[offSet],rcvLen-offSet,mCbMtrPara,&storeTm,pCbTask,RunTaskInfo);

    #if(1 == OOP_DAY_ADDREAD_485)
    if(pCbTask->CsdTask.RoadTask[CsIndex].MainOad.nObjID == 0x5004)
    {

       // IsNeedPatchDayMet07ByFrzTime(pComm_Info->cbLPPara.storeTm,storeTm,mCbMtrPara,pComm_Info,pCbTask);
    }
    #endif
    return 0;
}

//检测是否存在发生时间
//int checkIsExist201E(SCHEME_TASK_RUN_INFO *pCbTask,OOP_OAD_U  EventMainOad)
//{
//    uint16            TaskId;                  //任务编号
//    uint8         SchemeId;                //方案编号
//    char            CsdBuf[1024] = {0};
//    uint16            roadOffset = 0;
//    uint8         roadNum = 0;
//    uint8         i = 0;
//    uint8         j = 0;
//    int             Len;
//    OOP_OAD_U           MainOad;
//    uint8         SoadNum = 0;
//
////    TaskId = pCbTask->TaskId;
////    SchemeId = pCbTask->SchemeId;
/////*     Len = ReadProParaExt(MTR_EVT_PARA, CsdBuf, SchemeId, 0, 1);
////    if(Len < 0)
////    {
////        return 0;
////    }
////    if(CsdBuf[roadOffset++] != ARRAY)
////    {
////        return 0;
////    }
////
////    roadNum = CsdBuf[roadOffset++];
////    for(i = 0; i < roadNum; i++)
////    {
////        if(CsdBuf[roadOffset++] != eROAD)
////        {
////            return 0;
////        }
////
////        MainOad.OI = (uint16)(CsdBuf[roadOffset]<<8) + CsdBuf[roadOffset+1];
////        roadOffset += 2;
////        MainOad.attID = CsdBuf[roadOffset++];
////        MainOad.nIndex = CsdBuf[roadOffset++];
////        SoadNum = CsdBuf[roadOffset++];
////        if(MainOad.nObjID == EventMainOad.OI && MainOad.attID == EventMainOad.attID)
////        {
////            for(j = 0; j < SoadNum; j++)
////            {
////                if(CsdBuf[roadOffset] == 0x20 && CsdBuf[roadOffset+1] == 0x1E)
////                    return 1;
////                roadOffset += 4;
////            }
////        }
////        else
////        {
////            roadOffset += 4*SoadNum;
////        }
////
////    }
//    return 0;
//}

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
uint16 STD64507_AUTK_Recv_Event(uint8 pipe,OOP_OAD_U oad,uint8 *pBuf, uint16 rcvLen, cbMtrPara mCbMtrPara, S_COMM_INFO_MET *pComm_Info,SCHEME_TASK_RUN_INFO *pCbTask,uint8 *DI07Index,uint8 DI07Num,CTS_RUN_TASK_T *RunTaskInfo)
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
                STD64507_AUTK_Recv(pipe,oad,subOad,&pBuf[bias],rcvLen-bias,mCbMtrPara,NULL,pCbTask,RunTaskInfo);
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

                    STD64507_AUTK_Recv(pipe,oad,pCbTask->CsdTask.RoadTask[CsdIndex].OadListInfo[i],&pBuf[bias],rcvLen-bias,mCbMtrPara,NULL,pCbTask,RunTaskInfo);
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
uint16 STDPROTECTOR_AUTK_Recv_Event(uint8 pipe,OOP_OAD_U oad,uint8 *pBuf, uint16 rcvLen, cbMtrPara mCbMtrPara, S_COMM_INFO_MET *pComm_Info,SCHEME_TASK_RUN_INFO *pCbTask,uint8 *DI07Index,uint8 DI07Num,CTS_RUN_TASK_T *RunTaskInfo)
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
                    Protector_AUTK_Recv(pipe,oad,pCbTask->CsdTask.RoadTask[CsdIndex].OadListInfo[SubOadIndex],&pBuf[bias],rcvLen-bias,mCbMtrPara,NULL,pCbTask,RunTaskInfo);
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
            Protector_AUTK_Recv(pipe,oad,subOad,&pBuf[bias],rcvLen-bias,mCbMtrPara,NULL,pCbTask,RunTaskInfo);
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
                STD64507_AUTK_Recv(pipe,oad,subOad,&pBuf[bias],rcvLen-bias,mCbMtrPara,NULL,pCbTask,RunTaskInfo);
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

                    STD64507_AUTK_Recv(pipe,oad,pCbTask->CsdTask.RoadTask[CsdIndex].OadListInfo[i],&pBuf[bias],rcvLen-bias,mCbMtrPara,NULL,pCbTask,RunTaskInfo);
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


/****************************************************************************
*模块编号：
*名称：STD07_ToNextReadItem
*功能：切换07表抄读数据项
*输入参数：
*输出参数：
*处理：
*返回：无
****************************************************************************/
void  STD07_ToNextReadItem(BOOL NextCSDFlg,S_COMM_INFO_MET *pComm_Info,SCHEME_TASK_RUN_INFO *pCbTask,CTS_RUN_TASK_T *RunTaskInfo)
{
    uint16 *CsdIndex,SubOadIndex;

    SubOadIndex=RunTaskInfo->SubOadIndex;

    CsdIndex=&RunTaskInfo->CsdIndex;

    if(pCbTask==NULL || *CsdIndex>=pCbTask->CsdTask.RoadNum)
        return;
    if(NextCSDFlg==FALSE)
    {
        if(pCbTask->CsdTask.RoadTask[*CsdIndex].MainOad.nObjID > 0)
        {
            if(pCbTask->AcqInfo.Acqtype == ACQTYP_FRZTM || pCbTask->AcqInfo.Acqtype == ACQTYP_TI)
            {
                if(pCbTask->CsdTask.RoadTask[*CsdIndex].MainOad.nObjID!=0x5002)
                {
                    //负荷曲线处理方法不一样
                    if(pComm_Info->cbLPPara.storeTm.year_h==0)//未找到存储时间
                    {

                        //继续查找下一条记录
                        if(pCbTask->CsdTask.RoadTask[*CsdIndex].MainOad.nObjID ==0x5005 ||
                            pCbTask->CsdTask.RoadTask[*CsdIndex].MainOad.nObjID ==0x5006)//月冻结无冻结时标，不抄冻结时标,继续往下抄读
                        {
                            if((SubOadIndex+1)<pCbTask->CsdTask.RoadTask[*CsdIndex].SubOadNum)
                            {
                                RunTaskInfo->SubOadIndex++;//转到下一个从OAD
                                pComm_Info->MetStat = CB_STATE_SEND;
                                return;
                            }
                        }
                        else
                        {

                            if(pComm_Info->cbLPPara.startNum<=pComm_Info->cbLPPara.endNum)
                            {
                                //还没有搜索完，继续搜索
                                RunTaskInfo->SubOadIndex = 0;
                                pComm_Info->cbLPPara.startNum++;//继续搜索下一条记录
                                pComm_Info->MetStat = CB_STATE_SEND;
                                return;
                            }
                            #if D_485_698_RTTOPOINT == 1
                            //if(IsRealDataCopyToPointData())
                            {
                                //时标不对用实时数据转存
                                if(pCbTask->CsdTask.RoadTask[*CsdIndex].MainOad.nObjID  == 0x5004 && pComm_Info->cbLPPara.endNum == 0
                                    && pComm_Info->cbLPPara.startNum < 2)
                                {
                                    pComm_Info->cbLPPara.endNum = 0xff; //置为0xff，表示开始实时转存
                                    pComm_Info->subOADIndex++;//转到下一个从OAD
                                    pComm_Info->MetStat = CB_STATE_SEND;
                                    return;
                                }
                            }
                            #endif
                        }
                    }
                    else
                    {

                        //搜索到正确的时标了，继续抄读后续的列
                        if((pCbTask->CsdTask.RoadTask[*CsdIndex].MainOad.nObjID & 0xF000) != 0x3000)
                        {
                            #if(1 == OOP_DAY_ADDREAD_485)
                            if(pCbTask->CsdTask.RoadTask[*CsdIndex].MainOad.nObjID == 0x5004)
                            {
                                time_t  frzTime = 0;
                                DT_DateTimeBCD2Time(&pComm_Info->cbLPPara.storeTm,&frzTime);
                                if((SubOadIndex+1) >= pCbTask->CsdTask.RoadTask[*CsdIndex].SubOadNum)
                                {
                                    if(frzTime > pComm_Info->cbLPPara.startTime)
                                    {
                                        //还没有搜索完，继续搜索
                                        RunTaskInfo->SubOadIndex = 0;
                                        pComm_Info->cbLPPara.startNum++;//继续搜索下一条记录
                                        pComm_Info->MetStat = CB_STATE_SEND;
                                        return;
                                    }
                                }
                                else
                                {
                                    if(frzTime <= pComm_Info->cbLPPara.startTime)
                                    {
                                        RunTaskInfo->CsdIndex++;
                                        if(*CsdIndex<pCbTask->CsdTask.RoadNum)
                                        {
                                            RunTaskInfo->SubOadIndex = 0;
                                            if(pCbTask->CsdTask.RoadTask[*CsdIndex].MainOad.nObjID!= 0x0000)
                                                STD2007_LoadPrifle_Init(&pComm_Info->cbLPPara,pCbTask,RunTaskInfo);
                                        }
                                        pComm_Info->MetStat = CB_STATE_SEND;
                                        return;
                                    }
                                }
                            }
                            #endif

                            //645协议事件所有列通过一个数据标识应答，只需要读一次
                            if((RunTaskInfo->SubOadIndex)<pCbTask->CsdTask.RoadTask[*CsdIndex].SubOadNum)
                            {
                                RunTaskInfo->SubOadIndex++;//转到下一个从OAD
                                pComm_Info->MetStat = CB_STATE_SEND;
                                return;
                            }
                            #if D_485_698_RTTOPOINT == 1
                            //if(IsRealDataCopyToPointData())
                            {
                                if(pCbTask->CsdTask.RoadTask[*CsdIndex].MainOad.nObjID  == 0x5004 && pComm_Info->cbLPPara.endNum == 0xff)
                                {
                                    pComm_Info->cbLPPara.endNum = 0;
                                }
                            }
                            #endif
                        }
                        if(pCbTask->CsdTask.RoadTask[*CsdIndex].MainOad.nObjID  != 0x5006) //月冻结不抄下一个点
                        {

                            if(pComm_Info->cbLPPara.startNum<=pComm_Info->cbLPPara.endNum)
                            {
                                RunTaskInfo->SubOadIndex = 0;
                                pComm_Info->cbLPPara.startNum++;//继续搜索下一条记录
                                memset((uint8*)&pComm_Info->cbLPPara.storeTm,0x00,sizeof(pComm_Info->cbLPPara.storeTm));
                                pComm_Info->MetStat = CB_STATE_SEND;
                                return;
                            }
                        }
                    }
                }
                else
                {
                    //负荷曲线处理
                    DT_TimeAddInv(&pComm_Info->cbLPPara.startTime,pComm_Info->cbLPPara.tmInv);
                    if(pComm_Info->cbLPPara.startTime<pComm_Info->cbLPPara.endTime)
                    {
                        pComm_Info->MetStat = CB_STATE_SEND;
                        return;
                    }
                }
            }
            else
            {
                if((pCbTask->CsdTask.RoadTask[*CsdIndex].MainOad.nObjID & 0xF000) != 0x3000)
                {
                    //645协议事件所有列通过一个数据标识应答，只需要读一次

                    if(pCbTask->CsdTask.RoadTask[*CsdIndex].MainOad.nObjID==0x5002
                            &&pCbTask->AcqInfo.Acqtype!=ACQTYP_NULL)
                    {
                        //分钟曲线需要判断所有的时间点是否都已抄完
                        DT_TimeAddInv(&pComm_Info->cbLPPara.startTime,pComm_Info->cbLPPara.tmInv);
                        if(pComm_Info->cbLPPara.startTime<pComm_Info->cbLPPara.endTime)
                        {
                            pComm_Info->MetStat = CB_STATE_SEND;
                            return;
                        }
                    }
                    else
                    {
                        if((RunTaskInfo->SubOadIndex)<pCbTask->CsdTask.RoadTask[*CsdIndex].SubOadNum)
                        {
                            RunTaskInfo->SubOadIndex++;//转到下一个从OAD
                            pComm_Info->MetStat = CB_STATE_SEND;
                            return;
                        }
                    }
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
        #ifdef AREA_SHANDONG
		OOP_OAD_U Suboad =pCbTask->CsdTask.RoadTask[*CsdIndex].OadListInfo[SubOadIndex];

        if((pCbTask->CsdTask.RoadTask[*CsdIndex].MainOad.nObjID==0x5002)||(pCbTask->CsdTask.RoadTask[*CsdIndex].MainOad.nObjID==0x5003))
        {
            if((pCbTask->energynum ==0)&&(Suboad.value == 0x20210200))
        	{
				RunTaskInfo->SubOadIndex=RunTaskInfo->SubOadIndex+1;
        	}
			else
			{
            	RunTaskInfo->SubOadIndex = RunTaskInfo->SubOadIndex+pCbTask->energynum;
			}
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
        #else
        RunTaskInfo->CsdIndex++;
        #endif
    }
    if((*CsdIndex<pCbTask->CsdTask.RoadNum)&&(pCbTask->CsdTask.RoadTask[*CsdIndex].MainOad.nObjID==0x3000))
    {   if(pCbTask->CsdTask.RoadTask[*CsdIndex].MainOad.nObjID !=0)
        {
            RunTaskInfo->SubOadIndex = 0;
        }
        if(pCbTask->CsdTask.RoadTask[*CsdIndex].MainOad.nObjID!=0x0000)
        {
            STD2007_LoadPrifle_Init(&pComm_Info->cbLPPara,pCbTask,RunTaskInfo);
        }
    }
    pComm_Info->MetStat = CB_STATE_SEND;
}


#endif



