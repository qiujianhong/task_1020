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

#define FIRST_DEFINE_METPARAM
 #include "../../include/CtsAll.h"

#if OS_METDLT645_EN == 1

#define DEBUG_STD //版本稳定后去掉此项

#ifdef DEBUG_STD
  #define STD_ENV  "metstd"
#endif

/*
*****************************************************************************************
*- 国标表命令ID宏定义下行 -
*****************************************************************************************
*/
#define STD_CMDID_READ          0x01    /*-读数据-*/
#define STD_CMDID_READNEXT      0x02    /*-读后续数据-*/
#define STD_CMDID_REREAD        0x03    /*-重读数据-*/
#define STD_CMDID_WRITE         0x04    /*-写数据-*/
#define STD_CMDID_SETTIME       0x08    /*-广播校时(YYMMDDhhmmss)-*/
#define STD_CMDID_SETADDR       0x0A    /*-写设备地址(用广播地址设置新地址)-*/
#define STD_CMDID_SETRATE       0x0C    /*-更改通信波特率-*/
#define STD_CMDID_MDFPSW        0x0F    /*-修改密码-*/
#define STD_CMDID_CLRMAXAD      0x10    /*-最大需量清零-*/

/*
*****************************************************************************************
*-从站返回帧情况-
*****************************************************************************************
*/
#define STD_CMDID_ONE           0x80    //-正常应答帧无后续帧-
#define STD_CMDID_NEXT          0xA0    //-正常应答帧有后续帧-
#define STD_CMDID_ERROR         0xC0    //-异常应答帧-

/*
*****************************************************************************************
*-其他-
*****************************************************************************************
*/
#define STD_FILL_CODE_DD        0xDD    /*-0xAA+0x33空数据或数据域结束充字符(大项没有)-*/
#define STD_FILL_CODE_EE        0xEE    /*-0xBB+0x33空数据或空字节填充字符(小项没有)-*/
//static uint8 STD_BROADCAST_ADDRESS[6] = {0x99, 0x99, 0x99, 0x99, 0x99, 0x99}; /*-广播地址-*/
#define STD_GAP_VALUE           0x33    /*-数据域DATA发送时+33H接收时-33H-*/
#define STD_AWAKE_CODE          0xFE    /*-发送信息前先发送1~4个字节FEH用于唤醒接收方-*/
#define STD_GAP_VALUE           0x33    /*-数据域DATA发送时+33H接收时-33H-*/
#define STD_ADDR_SITE           12

#define STD_SYNC                            0x68

void STD_MakeFrame(uint8 * pMetAddr, uint16 fraDI, uint8 cmdDI, uint8 * pBuf, uint8 * pSndLen, uint8 bStdType);
uint8 STD_CheckRspFrame(uint8 pipe, uint8 * pBuf, uint16 rcvLen, uint8 bStdType,S_COMM_INFO_MET* pComm_Info_97);
extern cbMetPara STD2007Para;
extern uint8 ComNo[10];


#endif
/****************************************************************************
*模块编号：
*名称：JudgeIfSpecialCombin
*功能：判断是否为需要特殊处理的OI
*输入参数：
*输出参数：
*处理：
*返回：无
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
*模块编号：
*名称：GetDI97
*功能：获取要抄读97表数据项
*输入参数：
*输出参数：
*处理：
*返回：无
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
                //只有冻结支持通过实时数据转存
                subOadIndex = csd->SubOadNum;//全部抄完了
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
*模块编号：7-9-1-1
*名称：组织抄表任务发送帧
*功能：部颁规约电表抄表组帧
*输入参数：电表地址pMetAddr、数据标识fraDI、命令标识cmdDI、帧存放缓冲区地址pBuf、帧长存放地址pSndLen
*输出参数：组帧后的缓冲区、帧长
           电表类型 bStdType   0-普通国标 1-重庆简易表

*处理：根据电表规约组帧
*返回：无
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
        memcpy(&tmpSnd[5], pMetAddr, 6); /*- 电表地址 -*/
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
        memcpy(&tmpSnd[5], pMetAddr, 6); /*- 电表地址 -*/
    }
    tmpSnd[11] = 0x68;

    if (fraDI == 0x0000)  //防窃电表
    {
        tmpSnd[12] = 0x05;
        tmpSnd[13] = 0x00;
        tmpSnd[14] = CalCs(&tmpSnd[4], 10);     /*- 校验和 -*/
        tmpSnd[15] = 0x16;
        memcpy(pBuf, tmpSnd, 16);               /*- 电表发送缓冲区pBuf-*/
        *pSndLen = 16;
        return;
    }
    /*-正常国标组帧-*/
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
    memcpy(&tmpSnd[14], (uint8 *)&fraDI, sizeof(uint16));   /*- 项目表识编码 -*/

    AddGap(&tmpSnd[14], 2, STD_GAP_VALUE);
    tmpSnd[16] = CalCs(&tmpSnd[4], 12);         /*- 校验和 -*/
    tmpSnd[17] = 0x16;
    tmpSnd[18] = 0x00;
    tmpSnd[19] = 0x00;

    if (bStdType == 1)                          //重庆简易表
    {
        memcpy(pBuf, &tmpSnd[4], 16);
        *pSndLen = 14;
    }
    else
    {
        memcpy(pBuf, tmpSnd, 20);               /*- 电表发送缓冲区pBuf*/
        *pSndLen = 18;                          /*-有的表不支持后面多两个0   2006-7-21 10:03-*/
    }
    return;
}

/*-
***************************************************************************
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
***************************************************************************
-*/

uint8 STD_CheckRspFrame(uint8 pipe, uint8 *pBuf, uint16 rcvLen, uint8 bStdType,S_COMM_INFO_MET* pComm_Info_97)
{
    uint8   frameCbAddr[6]={0};
    uint8   frameCs;
    uint8   frameLen;

    frameLen = 0x0c + pBuf[9];
    if(frameLen > rcvLen)       //长度过长
        return 0;

    memcpy(frameCbAddr, pComm_Info_97->pMtrPara->MetAddr, 6);           /*- 电表地址 -*/

    uint8   LocalCnt = 0;                // 与本表地址相同个数
    int     i = 0;

    //比较返回地址
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
        && (pBuf[7] == pBuf[0])                         /*-北京系统电表地址补0或AA可变-*/
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
            if((pBuf[8] & 0xF0) == 0xC0)                /*-异常应答帧-*/
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
            if(frameLen >= 0x0C)
            {
                if(0x80 == (pBuf[8] & 0xF0))        /*-无后续数据帧-*/
                {
                    return 1;
                }
                else
                {
                    if(0xA0 == (pBuf[8] & 0xF0))    /*-有后续数据帧-*/
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
名称： Prepare_645_97_meter_read
功能：
-           为645_97规约的电表的抄读做准备，检查执行任务的测量的信息
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


void Prepare_645_97_meter_read(SCHEME_TASK_RUN_INFO *pCbTask, uint8 pipe,CTS_RUN_TASK_T   *RunTaskInfo, S_COMM_INFO_MET *Comm_Info_Oop,cbMtrPara *MtrPara)
{
    uint16 sn;
    uint16 MeterIndex;
    OOP_METER_BASIC_T MeterBasicInfo;
    
    MeterIndex=RunTaskInfo->MeterIndex;
    sn=pCbTask->MsInfo[MeterIndex].Sn;
    //读取测量点信息
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
名称： Meter_645_97_Read_Item
功能： 实现645——97规约电表的抄表报文的发送

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

void Meter_645_97_Read_Item(SCHEME_TASK_RUN_INFO *pCbTask,CTS_RUN_TASK_T *RunTaskInfo, uint8 pipe, S_COMM_INFO_MET *Comm_Info_Oop,cbMtrPara *MtrPara,uint8 *DI97Index,uint8 *DI97Num,uint16 *DI97Buff)
{

    uint16              DI97=0xFFFF;    //数据标识

 
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
            *DI97Num = GetDI97(pCbTask->AcqInfo,&pCbTask->CsdTask.RoadTask[CsdIndex],SubOadIndex,DI97Buff);//获取抄表的数据标识

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
名称： Deal_Receive_Result_645_97
功能： 处理645——97规约电表的抄表报文的接收结果

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

CB_STATE Deal_Receive_Result_645_97(uint8 pipe,S_COMM_INFO_MET *Comm_Info_Oop,SCHEME_TASK_RUN_INFO *pCbTask,CTS_RUN_TASK_T *RunTaskInfo,BOOL actResult,uint8   *DI97Index)
{
    uint16 pn,tmpMsIndex,MeterIndex;
    
    MeterIndex=RunTaskInfo->MeterIndex;
    pn=pCbTask->MsInfo[MeterIndex].Sn;
    tmpMsIndex = taskmng_ms_index_get(pipe,pn,RunTaskInfo->logicid);
    eOopCommRslt_t ret = 0;
    if(actResult)                          //该帧交换正确
    {
        STD97_ToNextReadItem(FALSE,Comm_Info_Oop,pCbTask,RunTaskInfo,DI97Index);
        Comm_Info_Oop->ComFrame++;          //总帧数增加
        taskmng_task_monitor_update(pipe,pCbTask->TaskId,pCbTask->logicid,UDTASK_RECVFRMNUM,NULL);//pCbTask->TaskMonitor.recvFrameNum ++;
        Comm_Info_Oop->ErrFrame = 0;        //失败帧数和错误帧数清零，准备重新计数
        Comm_Info_Oop->ErrTimes = 0;
        gAmrThreadArg[pipe].pMtrAmrInfo[tmpMsIndex].LastResult = TRUE;
        gAmrThreadArg[pipe].pMtrAmrInfo[tmpMsIndex].AllFailCnt = 0;
    }
    else                                            //该帧交换失败
    {

        if(Comm_Info_Oop->ErrTimes >= ERR_TIMES_MAX)        //同一帧的失败次数
        {
            if(Comm_Info_Oop->ErrFrame >= ERR_FRAME_MAX)      //失败次数过多，不再继续，准备退出
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
                STD97_ToNextReadItem(FALSE,Comm_Info_Oop,pCbTask,RunTaskInfo,DI97Index);
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
名称： Meter_645_97_Recvive_Data
功能： 实现645——97规约电表的抄表报文的接收

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

void Meter_645_97_Recvive_Data(SCHEME_TASK_RUN_INFO *pCbTask,CTS_RUN_TASK_T *RunTaskInfo, uint8 pipe, S_COMM_INFO_MET *Comm_Info_Oop,cbMtrPara *MtrPara,uint8 *DI97Index,uint8 DI97Num,uint8 *CombinBuff,uint16 *CombinLen,uint16 *DI97Buff)
{
    BOOL        actResult = TRUE;       //对回码处理的结果
    eOopCommRslt_t ret = 0;
    uint16              DI97=0xFFFF;                                    //数据标识
    uint8               *ptr=NULL;                                  //应答数据帧起始指针

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
    if(Comm_Info_Oop->RcvLen > 9)   //有效报文的最小长度为9个字节
    {
        ptr = (uint8*)commfun_645_frame_get((char*)Comm_Info_Oop->pRcvBuf, STD2007Para.Sync, Comm_Info_Oop->RcvLen,&reallen);
    }
    if(Comm_Info_Oop->RcvLen <= 0 || ptr == NULL)
    {
        actResult = FALSE;              //电表没有回码 或者 回码格式非法
    }
    else
    {
        Comm_Info_Oop->RcvLen = reallen;
        memmove(Comm_Info_Oop->pRcvBuf, ptr, Comm_Info_Oop->RcvLen);
        ret = STD_CheckRspFrame(pipe, Comm_Info_Oop->pRcvBuf, (uint16)Comm_Info_Oop->RcvLen, 0,Comm_Info_Oop);
        if((ret == 1)||(ret == 2))    //有效,且有数据返回
        {
            OOP_OAD_U mainOad;
            Comm_Info_Oop->ErrFrame=0;
            DI97 = DI97Buff[*DI97Index];

            if(*DI97Index<DI97Num)//OAD 对应的97数据标识还没有通讯完
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

    Deal_Receive_Result_645_97(pipe,Comm_Info_Oop,pCbTask,RunTaskInfo,actResult,DI97Index);
}
/****************************************************************************
*模块编号：
*名称：IsGroupByDI97
*功能：判断97的的数据标识是否在同一组中
*输入参数：DI
*输出参数：
*处理：
*返回：
    FALSE:不在同一组
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
*模块编号：
*名称：STD1997_FrameCombin
*功能：1个OAD对应两个不同的97数据项的,需要把两帧数据拼凑到一起
*输入参数：DI
*输出参数：
*处理：
*返回：
    FALSE:不在同一组
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
    //根据从OAD 查找645对应表

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
        //97表的需量有两个数据项A01F和B01F,需要两次数据项内容的合并，在抄回来BO1F后,需要便宜A01F所占的字节数
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
        //97表需量走到这说明是在抄读B01F,B01F的长度块是20，分是4；
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
名称： STD64597_AUTK_Recv
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
uint16 STD64597_AUTK_Recv(uint8 pipe,OOP_OAD_U mainOAD,OOP_OAD_U subOAD,uint8 *pBuf, uint16 rcvLen, cbMtrPara mCbMtrPara, SCHEME_TASK_RUN_INFO *pCbTask,CTS_RUN_TASK_T *RunTaskInfo)
{
    uint16               offSet=0;
    OadInfoMap       pOadInfo;
    const MapOopTo645_NEW  *pMapOopTo645;
    DateTimeBcd_t taskStoreTime;  // 采集启动时间
    
    DateTimeBcd_t taskStartTime;   // 采集存储时间
    OadStoreInfo StoreInfo;
    time_t storeTm;
    time_t colStartTm;
    uint8                tmpBufer[255];           //协议转换，暂时定255个字节
    DataSource sourcedata;
    uint8 desbuffer[2000];
    uint16 desLen,MsIndex;
    uint16 MeterIndex,pn;


    memset((uint8*)&StoreInfo,0x00,sizeof(OadStoreInfo));
    MeterIndex=RunTaskInfo->MeterIndex;
    pn=pCbTask->MsInfo[MeterIndex].Sn;
    MsIndex=taskmng_ms_index_get(pipe,pn,RunTaskInfo->logicid);
    if(pCbTask->TaskId>255)
    {//内部任务处理
        //待增加

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
    //97电表状态字
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
        memcpy_r((uint8*)&taskStartTime,(uint8*)RunTaskInfo->ThisTurnBeginTime,7);//采集启动时间
        DT_DateTimeBCD2Time(&taskStartTime,&colStartTm);
    DT_DateTimeBCD2Time(&taskStartTime,&storeTm);
    if(pCbTask->StoreTimeType == STRTMTYP_NULL || pCbTask->StoreTimeType == STRTMTYP_STARTTM || pCbTask->StoreTimeType == STRTMTYP_FRZTIME)//97表没有冻结时标，所以存任务启动时间
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
    TASK_FMT_TRACE ( pipe,COMM_LOG, "任务号:pCbTask->TaskId is %d\n",pCbTask->TaskId);
    taskmng_meter_data_save(desbuffer,desLen,StoreInfo,pipe);

    return offSet;


}
/****************************************************************************
*模块编号：
*名称：STD97_ToNextReadItem
*功能：切换645——07规约电表的抄读数据项
*输入参数：
*输出参数：
*处理：
*返回：无
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
                        RunTaskInfo->SubOadIndex++;//转到下一个从OAD
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


