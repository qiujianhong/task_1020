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

#define STD2007_SYNC                0x68



extern RS485_RS232_PARA Rs485ComNo[8];
extern uint8 ComNo[10];

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

void Prepare_std188_meter_read(SCHEME_TASK_RUN_INFO *pCbTask, uint8 pipe, CTS_RUN_TASK_T *RunTaskInfo, S_COMM_INFO_MET *Comm_Info_Oop,cbMtrPara *MtrPara)
{


    uint16 sn;
    uint16 MeterIndex;
    OOP_METER_BASIC_T MeterBasicInfo;
    OOP_METER_EXTENDED_T extend;
    //task_id=pCbTask->TaskId;
    MeterIndex=RunTaskInfo->MeterIndex;
    sn=pCbTask->MsInfo[MeterIndex].Sn;
    //读取测量点信息
    memset((void *)MtrPara, 0x0, sizeof(cbMtrPara));

    //读取测量点信息
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
*函数名称：Meter_645_07_Read_Item
*功能：  645_07抄读发送报文
*输入参数：
*输出参数：
*返回：
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
        *DI07Num = GetDI188(pCbTask->AcqInfo,&pCbTask->CsdTask.RoadTask[CsdIndex],SubOadIndex,Comm_Info_Oop->cbLPPara.endNum,DI07Buff,pCbTask,RunTaskInfo,pipe);//获取抄表的数据标识

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
            TASK_FMT_TRACE( pipe, REC_LOG, "发送失败 ret is %d\n",ret);
        }
        TASK_FMT_TRACE( pipe, REC_LOG, "测量点号 %d ,报文发送\n",ret);
        TASK_BUF_TRACE( pipe, REC_LOG, Comm_Info_Oop->pSndBuf, Comm_Info_Oop->SndLen);
        Comm_Info_Oop->MetStat = CB_STATE_RECV;         //发送成功，准备接收
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
    if(Comm_Info_Oop->MetStat != CB_STATE_RECV )//0xff  所有数据项已经抄完不再处于接收状态
    {
        Comm_Info_Oop->MetStat = CB_STATE_FINISH;       //默认为结束状态，只有在查找到合适的数据项后才转入接收状态，其他都算结束
    }
}


/****************************************************************************
*Meter_Std188_Read_Item_New
*功能：山东水表通过采集器抄读
*输入参数：
*输出参数：
*返回：
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
        *DI07Num = GetDI188New(pCbTask->AcqInfo,&pCbTask->CsdTask.RoadTask[CsdIndex],SubOadIndex,Comm_Info_Oop->cbLPPara.endNum,DI07Buff,pCbTask,RunTaskInfo,pipe);//获取抄表的数据标识

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
            TASK_FMT_TRACE( pipe, REC_LOG, "发送失败 ret is %d\n",ret);
        }
        TASK_FMT_TRACE( pipe, REC_LOG, "测量点号 %d ,报文发送\n",ret);
        TASK_BUF_TRACE( pipe, REC_LOG, Comm_Info_Oop->pSndBuf, Comm_Info_Oop->SndLen);
        Comm_Info_Oop->MetStat = CB_STATE_RECV;         //发送成功，准备接收
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
    if(Comm_Info_Oop->MetStat != CB_STATE_RECV )//0xff  所有数据项已经抄完不再处于接收状态
    {
        Comm_Info_Oop->MetStat = CB_STATE_FINISH;       //默认为结束状态，只有在查找到合适的数据项后才转入接收状态，其他都算结束
    }
}

int8 CJT188_CheckRspFrame(uint8 pipe, uint8 *pBuf, uint16 rcvLen, uint16 DI, S_COMM_INFO_MET* pCommInfo, uint16 *offset,SCHEME_TASK_RUN_INFO *pCbTask, CTS_RUN_TASK_T *RunTaskInfo)
{
    uint16  head_offset = 0; /* 数据索引 */
    uint8   CS = 0;
    uint8   i;
    uint8   data_len = 0;
    uint8   meter_addr[7]= {0};
    uint8  CtrlCode;
    uint8 user_type, meter_type;

    //用户类型
    OOP_METER_BASIC_T basicinfo;
    uint16 MeterIndex,pn;


    //用户类型
    MeterIndex=RunTaskInfo->MeterIndex;
    pn=pCbTask->MsInfo[MeterIndex].Sn;
    taskmng_mtr_basic_info_get(pn,RunTaskInfo->logicid,&basicinfo);

    user_type = basicinfo.userType;
    //表类型
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
        meter_type = 0x10;  //默认水表
    }

    memcpy_r(meter_addr, &pCommInfo->pMtrPara->MtrCommAddr.add[0], 7);

    head_offset = 0;
    while((head_offset + AMR_MT_PRTL_MIN_LEN_188 - 1) < rcvLen)
    {
        /* 1 判帧开始标志0x68和水表类型*/
        if ((pBuf[head_offset] != 0x68) || (pBuf[head_offset+1] != meter_type))
        {
            head_offset ++;
            continue;
        }
        /* 2 判帧长度 */
        if((rcvLen - head_offset) < AMR_MT_PRTL_MIN_LEN_188)
        {
            memset(pBuf, 0, rcvLen);
            return AMR_CHECK_RESULT_FAILUER_188;
        }

        data_len =  pBuf[head_offset + 10];          //188规约数据域长度 在帧开始标志后第10个字节

        /* 3 判结尾0x16 */
        if (pBuf[head_offset + data_len + 12] != 0x16)
        {
            head_offset ++;
            continue;
        }

        /* 4 判校验和 */
        for (i=0; i < (data_len + 11); i++)
        {
            CS += pBuf[head_offset+i];
        }
        if (CS != pBuf[head_offset + data_len + 11])
        {
            memset(pBuf, 0, rcvLen);
            return AMR_CHECK_RESULT_FAILUER_188;
        }

        /* 6 走到这里说明帧已经正确 */

        /* 6.1 刷新地址域 */
        if(memcmp(&pBuf[head_offset+2], &meter_addr[0], 7) != 0)
        {
            return AMR_CHECK_RESULT_FAILUER_188;
        }

        /* 6.2 刷新控制码*/
        CtrlCode = pBuf[head_offset+9];
        if ((CtrlCode & 0x80) == 0x00)
        {
            /* DataIndex + 数据长度 + 规约68到LEN的长度 + CS和16 */
            head_offset = head_offset + data_len + 12 + 1;
            CS = 0;
            continue;
        }

        /* 6.5 判断是否是 否认帧*/
        if (CtrlCode & 0x40)
        {
            memset(pBuf,0,rcvLen);
            return AMR_CHECK_RESULT_DENY_188;
        }

        /* 6.5 普通抄表任务判断发送和接受的帧标识是否相同 如果不同按此帧失败处理*/
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

    //用户类型
    OOP_METER_BASIC_T basicinfo;
    uint16 MeterIndex,pn;


    //用户类型
    MeterIndex=RunTaskInfo->MeterIndex;
    pn=pCbTask->MsInfo[MeterIndex].Sn;
    taskmng_mtr_basic_info_get(pn,RunTaskInfo->logicid,&basicinfo);
    //表类型
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
        meter_type = 0x10;  //默认水表
    }
    TASK_FMT_TRACE( pipe, REC_LOG,"表类型meter_type %d\n",meter_type);
    TASK_BUF_TRACE( pipe, REC_LOG,pBuf, rcvLen);
    if((0x10 == (meter_type&0xF0)) || (0x30 == (meter_type&0xF0)))	//水表
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
        *(profileBuff+15) = (tmp & 0x03);		//阀门状态
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
        *(profileBuff+36) = (tmp & 0x03);		//阀门状态
        *(profileBuff+37) = ((tmp>>2) & 0x01);  //电池电压
    }
    TASK_BUF_TRACE( pipe, REC_LOG,profileBuff, 100);
    //存储时标处理
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
                    TASK_FMT_TRACE( pipe, REC_LOG,"采集方式pCbTask->AcqInfo.Acqtype is %d\n",pCbTask->AcqInfo.Acqtype);
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
    uint8                tmpBufer[255];           //协议转换，暂时定255个字节
    
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

    //用户类型
    OOP_METER_BASIC_T basicinfo;
    uint16 MeterIndex,pn;

    memset(tmpBufer,0x00,255);
    memcpy(tmpBufer,pBuf,rcvLen);
    ReduceGap(tmpBufer, rcvLen, 0x33);
    //用户类型
    MeterIndex=RunTaskInfo->MeterIndex;
    pn=pCbTask->MsInfo[MeterIndex].Sn;
    taskmng_mtr_basic_info_get(pn,RunTaskInfo->logicid,&basicinfo);
    //表类型
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
        meter_type = 0x00;  //默认水表
    }
    TASK_FMT_TRACE( pipe, REC_LOG,"表类型meter_type %d\n",meter_type);
    TASK_BUF_TRACE( pipe, REC_LOG,tmpBufer, rcvLen);
    if((0x00 == (meter_type&0xF0)) || (0x30 == (meter_type&0xF0)))	//水表
    {
        //当前累计流量，单位m*m*m-BCD
        if(mainOAD.value==0x00000000)
        {
            //if(pBuf[6] == 0x2C)
            {
                memcpy(profileBuff,tmpBufer+7,4);
            }
            //结算日累计流量，，单位m*m*m-BCD
            //if(pBuf[11] == 0x2C)
            {
                memcpy(profileBuff+4,tmpBufer+12,4);
            }
            //累计工作时间
            memcpy(profileBuff+8,tmpBufer+37,3);
            if(check_is_all_ch(tmpBufer+37,0xEE,3)==TRUE)
            {
                memset(profileBuff+8,0x00,3);
            }
            //实时时间-年月日时分秒-BCD
            memcpy(profileBuff+11,tmpBufer+40,7);

            //状态ST
            //tmp = pBuf[17];
            *(profileBuff+15) = tmpBufer[47];		//阀门状态
            *(profileBuff+16) = tmpBufer[48];  //电池电压
        }
        else
        {
                        //if(pBuf[6] == 0x2C)
            {
                memcpy(profileBuff,tmpBufer+10,4);
            }
            //结算日累计流量，，单位m*m*m-BCD
            //if(pBuf[11] == 0x2C)
            {
                memcpy(profileBuff+4,tmpBufer+15,4);
            }
            //累计工作时间
            memcpy(profileBuff+8,tmpBufer+40,3);
            //实时时间-年月日时分秒-BCD
            memcpy(profileBuff+11,tmpBufer+43,7);

            //状态ST
            //tmp = pBuf[17];
            *(profileBuff+15) = tmpBufer[50];		//阀门状态
            *(profileBuff+16) = tmpBufer[51];  //电池电压

        }
        //XL_MemCpy(profileBuff+14,pBuf+16,2);
    }
    else if(0x50 == (meter_type&0xF0)) //热表----数据域中数据和单位的顺序还不好确定 ? 还有单位换算 ?
    {
        if(mainOAD.value ==0x000000)
        {
            //结算日热量-BCD
            memcpy(profileBuff,tmpBufer+7,4);

            //当前热量-BCD
            memcpy(profileBuff+4,tmpBufer+12,4);

            //热功率-BCD
            memcpy(profileBuff+8,tmpBufer+17,4);

            //流量-BCD
            memcpy(profileBuff+12,tmpBufer+22,4);

            //累计流量-BCD
            //if(pBuf[20] == 0x2C)
            {
                memcpy(profileBuff+16,tmpBufer+27,4);
            }
    //        else
    //        {
    //            memcpy(profileBuff+16,pBuf+20,4);
    //        }

            //供水温度-BCD
            memcpy(profileBuff+20,tmpBufer+31,3);

            //回水温度-BCD
            memcpy(profileBuff+23,tmpBufer+34,3);

            //累计工作时间-BCD
            memcpy(profileBuff+26,tmpBufer+37,3);

            //实时时间-年月日时分秒-BCD
            memcpy(profileBuff+29,tmpBufer+40,7);

            //状态ST
            //XL_MemCpy(profileBuff+35,pBuf+40,2);
            //tmp = pBuf[41];
            *(profileBuff+36) = tmpBufer[47];		//阀门状态
            *(profileBuff+37) = tmpBufer[48];  //电池电压
        }
        else
        {
            //结算日热量-BCD
            memcpy(profileBuff,tmpBufer+10,4);

            //当前热量-BCD
            memcpy(profileBuff+4,tmpBufer+15,4);

            //热功率-BCD
            memcpy(profileBuff+8,tmpBufer+20,4);

            //流量-BCD
            memcpy(profileBuff+12,tmpBufer+25,4);

            //累计流量-BCD
            //if(pBuf[20] == 0x2C)
            {
                memcpy(profileBuff+16,tmpBufer+30,4);
            }
    //        else
    //        {
    //            memcpy(profileBuff+16,pBuf+20,4);
    //        }

            //供水温度-BCD
            memcpy(profileBuff+20,tmpBufer+34,3);

            //回水温度-BCD
            memcpy(profileBuff+23,tmpBufer+37,3);

            //累计工作时间-BCD
            memcpy(profileBuff+26,tmpBufer+40,3);

            //实时时间-年月日时分秒-BCD
            memcpy(profileBuff+29,tmpBufer+43,7);

            //状态ST
            //XL_MemCpy(profileBuff+35,pBuf+40,2);
            //tmp = pBuf[41];
            *(profileBuff+36) = tmpBufer[50];		//阀门状态
            *(profileBuff+37) = tmpBufer[51];  //电池电压

        }
    }
    TASK_BUF_TRACE( pipe, REC_LOG,profileBuff, 100);
    //存储时标处理
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
                    TASK_FMT_TRACE( pipe, REC_LOG,"采集方式pCbTask->AcqInfo.Acqtype is %d\n",pCbTask->AcqInfo.Acqtype);
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
*名称：STD97_ToNextReadItem
*功能：获取下一个从OAD,或者获取下一个CSD
*输入参数：
*输出参数：
*处理：
*返回：无
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
////                    RunTaskInfo->SubOadIndex++;/* 转到下一个从OAD */
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
*函数名称：Deal_Receive_Result_645_07
*功能：处理  645_07抄读接收报文的结果
*输入参数：
*输出参数：
*返回：
****************************************************************************/

void Meter_188_Recvive_Data(SCHEME_TASK_RUN_INFO *pCbTask,CTS_RUN_TASK_T *RunTaskInfo, uint8 pipe, S_COMM_INFO_MET *Comm_Info_Oop,cbMtrPara *MtrPara,uint8 *DI07Index,uint8 DI07Num,uint8 *CombinBuff,uint16 *CombinLen,uint32 *DI07Buff)
{
    //uint8               *ptr=NULL;
    BOOL        actResult = TRUE;       //对回码处理的结果
    eOopCommRslt_t ret = 0;

    //uint32              DI07 = 0;                                   //数据标识

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
    TASK_FMT_TRACE( pipe, REC_LOG,"pn %d 收到报文 \n",pn);
    TASK_BUF_TRACE( pipe, REC_LOG,Comm_Info_Oop->pRcvBuf, Comm_Info_Oop->RcvLen);
    
    if(Comm_Info_Oop->RcvLen < AMR_MT_PRTL_MIN_LEN_188) // || ptr == NULL)
    {
        actResult = FALSE;              //电表没有回码 或者 回码格式非法
        ret = AMR_CHECK_RESULT_FAILUER_188;
        TASK_FMT_TRACE( pipe, REC_LOG,"pn 回码异常 \n",pn);
    }
    else
    {
        ret = CJT188_CheckRspFrame(pipe, Comm_Info_Oop->pRcvBuf, (uint16)Comm_Info_Oop->RcvLen, DI07Buff[*DI07Index], Comm_Info_Oop, &frameoffset,pCbTask,RunTaskInfo);
        TASK_FMT_TRACE( pipe, REC_LOG,"解析结果 ret %d\n",ret);
        if(ret == AMR_CHECK_RESULT_SUCCEED_188)    //有效,且有数据返回
        {
            Comm_Info_Oop->ErrFrame=0;
            CJT188_AUTK_Recv_Deal(pipe,Comm_Info_Oop->pRcvBuf+frameoffset+14,Comm_Info_Oop->RcvLen-frameoffset-14-2,*MtrPara,Comm_Info_Oop,pCbTask,RunTaskInfo);
            actResult = TRUE;
        }
        else if(ret == AMR_CHECK_RESULT_FAILUER_188) //有有效数据，但值和期望的不一样
        {
            Comm_Info_Oop->ErrFrame=0;
            Comm_Info_Oop->ErrTimes = ERR_TIMES_MAX;		//按失败处理，不过不在重试，直接将重试次数置为最大值
            actResult = FALSE;
        }
        else if(ret == AMR_CHECK_RESULT_DENY_188)                 //否认帧
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

if(actResult)                          //该帧交换正确
{
    CJT188_ToNextReadItem(FALSE,Comm_Info_Oop,pCbTask,RunTaskInfo);
    Comm_Info_Oop->ComFrame++;          //总帧数增加
    
    Comm_Info_Oop->ErrFrame = 0;        //失败帧数和错误帧数清零，准备重新计数
    Comm_Info_Oop->ErrTimes = 0;                    

}
else                                            //该帧交换失败
{

    if(Comm_Info_Oop->ErrTimes >= ERR_TIMES_MAX)        //同一帧的失败次数
    {
        if(Comm_Info_Oop->ErrFrame >= ERR_FRAME_MAX)	  //失败次数过多，不再继续，准备退出
        {
            Comm_Info_Oop->ErrFrame = ERR_FRAME_MAX;
            Comm_Info_Oop->MetStat = CB_STATE_FINISH;
            return;
        }
        else
        {
            if(AMR_CHECK_RESULT_FAILUER_188 == ret)                                                  //没有接收到数据
            {
                Comm_Info_Oop->ErrFrame++;                        //失败帧数增加
                Comm_Info_Oop->ComFrame++;                        //总帧数增加
            }

            Comm_Info_Oop->ErrTimes = 0;                         //重试次数清零
            CJT188_ToNextReadItem(FALSE,Comm_Info_Oop,pCbTask,RunTaskInfo);//冻结类第一个数据项回否认，继续抄读，不切表
        }
    }
    else
    {
        if(ret==AMR_CHECK_RESULT_FAILUER_188)                                                           //没有接收数据
        {
            Comm_Info_Oop->ErrTimes++;                              //重发当前数据项
        }
    }
}
    Comm_Info_Oop->MetStat = CB_STATE_SEND;                    //转入发送流程
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

uint8 STD2007_CheckRspFrame_Ex(uint8 pipe, uint8 *pBuf, uint16 rcvLen, uint8 bStdType,S_COMM_INFO_MET* pCommInfo)
{
    uint8   frameCs;
    uint16  frameLen;
    //比较返回地址
    
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
/****************************************************************************
*函数名称：Deal_Receive_Result_645_07
*功能：处理  645_07抄读接收报文的结果
*输入参数：
*输出参数：
*返回：
****************************************************************************/

CB_STATE Deal_Receive_Result_188(S_COMM_INFO_MET *Comm_Info_Oop,SCHEME_TASK_RUN_INFO *pCbTask,CTS_RUN_TASK_T *RunTaskInfo,BOOL actResult,uint8 pipe)
{
    uint16 pn,tmpMsIndex,MeterIndex;
    
    MeterIndex=RunTaskInfo->MeterIndex;
    pn=pCbTask->MsInfo[MeterIndex].Sn;
    tmpMsIndex = taskmng_ms_index_get(pipe,pn,RunTaskInfo->logicid);


    eOopCommRslt_t ret = 0;
    if(actResult)                          //该帧交换正确
    {
//      printf("Deal_Receive_Result_645_07 is BEGIN1111\n");
        CJT188_ToNextReadItem(FALSE,Comm_Info_Oop,pCbTask,RunTaskInfo);
        Comm_Info_Oop->ComFrame++;          //总帧数增加
    
        Comm_Info_Oop->ErrFrame = 0;        //失败帧数和错误帧数清零，准备重新计数
        Comm_Info_Oop->ErrTimes = 0;
        gAmrThreadArg[pipe].pMtrAmrInfo[tmpMsIndex].AllFailCnt =0 ;
        gAmrThreadArg[pipe].pMtrAmrInfo[tmpMsIndex].LastResult = TRUE;
    }
    else                                            //该帧交换失败
    {
        TASK_FMT_TRACE( pipe, COMM_LOG,"ERR_FRAME_MAX is %d,ERR_TIMES_MAX is %d\n", ERR_FRAME_MAX,ERR_TIMES_MAX);
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
                CJT188_ToNextReadItem(FALSE,Comm_Info_Oop,pCbTask,RunTaskInfo);//冻结类第一个数据项回否认，继续抄读，不切表
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

void Meter_188_Recvive_Data_New(SCHEME_TASK_RUN_INFO *pCbTask,CTS_RUN_TASK_T *RunTaskInfo, uint8 pipe, S_COMM_INFO_MET *Comm_Info_Oop,cbMtrPara *MtrPara,uint8 *DI07Index,uint8 DI07Num,uint8 *CombinBuff,uint16 *CombinLen,uint32 *DI07Buff)
{
    //uint8               *ptr=NULL;
    BOOL        actResult = TRUE;       //对回码处理的结果
    eOopCommRslt_t ret = 0;

    //uint32              DI07 = 0;                                   //数据标识

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
    TASK_FMT_TRACE( pipe, REC_LOG,"pn %d 收到报文 \n",pn);
    TASK_BUF_TRACE( pipe, REC_LOG,Comm_Info_Oop->pRcvBuf, Comm_Info_Oop->RcvLen);
    
    if(Comm_Info_Oop->RcvLen > 9)   //有效报文的最小长度为9个字节
    {
       // gAmrThreadArg[pipe].pMtrAmrInfo[tmpMsIndex].LastRecvTime=time(NULL);
        taskmng_task_monitor_update(pipe,pCbTask->TaskId,pCbTask->logicid,UDTASK_RECVFRMNUM,NULL);
        ptr = (uint8*)commfun_645_frame_get((char*)Comm_Info_Oop->pRcvBuf, 0x68, Comm_Info_Oop->RcvLen,&reallen);//找到645报文的68头
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
        ret = STD2007_CheckRspFrame_Ex(pipe, Comm_Info_Oop->pRcvBuf, (uint16)Comm_Info_Oop->RcvLen, 0,Comm_Info_Oop);
        TASK_FMT_TRACE( pipe, REC_LOG,"解析结果 ret %d\n",ret);
         if((ret == 1)||(ret == 2))  //有效,且有数据返回
        {
            Comm_Info_Oop->ErrFrame=0;
            CJT188_AUTK_Recv_Deal_New(pipe,Comm_Info_Oop->pRcvBuf+frameoffset+14,Comm_Info_Oop->RcvLen-frameoffset-14-2,*MtrPara,Comm_Info_Oop,pCbTask,RunTaskInfo);
            actResult = TRUE;
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
Deal_Receive_Result_188(Comm_Info_Oop,pCbTask,RunTaskInfo,actResult,pipe);

//if(actResult)                          //该帧交换正确
//{
//    CJT188_ToNextReadItem(FALSE,Comm_Info_Oop,pCbTask,RunTaskInfo);
//    Comm_Info_Oop->ComFrame++;          //总帧数增加
//    
//    Comm_Info_Oop->ErrFrame = 0;        //失败帧数和错误帧数清零，准备重新计数
//    Comm_Info_Oop->ErrTimes = 0;                    
//
//}
//else                                            //该帧交换失败
//{
//
//    if(Comm_Info_Oop->ErrTimes >= ERR_TIMES_MAX)        //同一帧的失败次数
//    {
//        if(Comm_Info_Oop->ErrFrame >= ERR_FRAME_MAX)	  //失败次数过多，不再继续，准备退出
//        {
//            Comm_Info_Oop->ErrFrame = ERR_FRAME_MAX;
//            Comm_Info_Oop->MetStat = CB_STATE_FINISH;
//            return;
//        }
//        else
//        {
//            if(AMR_CHECK_RESULT_FAILUER_188 == ret)                                                  //没有接收到数据
//            {
//                Comm_Info_Oop->ErrFrame++;                        //失败帧数增加
//                Comm_Info_Oop->ComFrame++;                        //总帧数增加
//            }
//
//            Comm_Info_Oop->ErrTimes = 0;                         //重试次数清零
//            CJT188_ToNextReadItem(FALSE,Comm_Info_Oop,pCbTask,RunTaskInfo);//冻结类第一个数据项回否认，继续抄读，不切表
//        }
//    }
//    else
//    {
//        if(ret==AMR_CHECK_RESULT_FAILUER_188)                                                           //没有接收数据
//        {
//            Comm_Info_Oop->ErrTimes++;                              //重发当前数据项
//        }
//    }
//}
//    Comm_Info_Oop->MetStat = CB_STATE_SEND;                    //转入发送流程
//    return;
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
void STD188_MakeFrameEx(uint8 *pMetAddr, uint32 fraDI, uint8 cmdDI, uint8 *pBuf, uint8 *pSndLen, S_COMM_INFO_MET *pComm_Info,SCHEME_TASK_RUN_INFO *pCbTask, CTS_RUN_TASK_T *RunTaskInfo)
{
    uint8   CS = 0;                       //校验码
    uint8 tmpSnd[30];
    uint8 i;
    uint8 offset = 0;
    static uint8 ser = 0;
    uint8 user_type, meter_type;
    OOP_METER_BASIC_T basicinfo;
    uint16 MeterIndex,pn;


    //用户类型
    MeterIndex=RunTaskInfo->MeterIndex;
    pn=pCbTask->MsInfo[MeterIndex].Sn;
    taskmng_mtr_basic_info_get(pn,RunTaskInfo->logicid,&basicinfo);

    user_type = basicinfo.userType;
    //表类型
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
    tmpSnd[offset++] = pComm_Info->pMtrPara->MtrCommAddr.add[6];
    tmpSnd[offset++] = pComm_Info->pMtrPara->MtrCommAddr.add[5];
    tmpSnd[offset++] = pComm_Info->pMtrPara->MtrCommAddr.add[4];
    tmpSnd[offset++] = pComm_Info->pMtrPara->MtrCommAddr.add[3];
    tmpSnd[offset++] = pComm_Info->pMtrPara->MtrCommAddr.add[2];
    tmpSnd[offset++] = pComm_Info->pMtrPara->MtrCommAddr.add[1];
    tmpSnd[offset++] = pComm_Info->pMtrPara->MtrCommAddr.add[0];
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

    memcpy(pBuf, tmpSnd, offset);        /*- 电表发送缓冲区pBuf*/
    *pSndLen = offset;

    return;

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
void STD188_MakeFrameEx_New(uint8 *pMetAddr, uint32 fraDI, uint8 cmdDI, uint8 *pBuf, uint8 *pSndLen, S_COMM_INFO_MET *pComm_Info,SCHEME_TASK_RUN_INFO *pCbTask, CTS_RUN_TASK_T *RunTaskInfo)
{
    uint8   CS = 0;                       //校验码
    uint8 tmpSnd[30];
    uint8 i;
    uint8 offset = 0;
//    static uint8 ser = 0;
    uint8 user_type, meter_type;
    OOP_METER_BASIC_T basicinfo;
    uint16 MeterIndex,pn;


    //用户类型
    MeterIndex=RunTaskInfo->MeterIndex;
    pn=pCbTask->MsInfo[MeterIndex].Sn;
    taskmng_mtr_basic_info_get(pn,RunTaskInfo->logicid,&basicinfo);

    user_type = basicinfo.userType;
    //表类型
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
        meter_type = 0x00;  //默认水表
    }

    memset(tmpSnd, 0x00, sizeof(tmpSnd));
    for(i = 0; i < 4; i++)
        tmpSnd[i] = 0xFE;

    offset = 4;

    tmpSnd[offset++] = 0x68;
    /*- 采集器地址 -*/
    tmpSnd[offset++] = pComm_Info->pMtrPara->CjqCommAddr.add[3];
    tmpSnd[offset++] = pComm_Info->pMtrPara->CjqCommAddr.add[2];
    tmpSnd[offset++] = pComm_Info->pMtrPara->CjqCommAddr.add[1];
    tmpSnd[offset++] = pComm_Info->pMtrPara->CjqCommAddr.add[0];
    tmpSnd[offset++] = pComm_Info->pMtrPara->CjqCommAddr.add[4];
    tmpSnd[offset++] = pComm_Info->pMtrPara->CjqCommAddr.add[5];
    tmpSnd[offset++] = 0x68;
    tmpSnd[offset++] = 0x11;              //功能码
    tmpSnd[offset++] = 0x0c;           //数据域长度
    memcpy((uint8*)&tmpSnd[offset],(uint8*)&fraDI,4);//数据标识
    offset =offset+4;
    //表类型
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
        tmpSnd[i]=tmpSnd[i]+0x33;//加33
    }
    //校验和
    for (i = 4; i < offset; i++)
    {
        CS += tmpSnd[i];
    }
    tmpSnd[offset++] = CS;
    tmpSnd[offset++] = 0x16;

    memcpy(pBuf, tmpSnd, offset);        /*- 电表发送缓冲区pBuf*/
    *pSndLen = offset;

    return;

}

/****************************************************************************
*模块编号：
*名称：
*功能：根据OAD计算 OAD的数据在数据块中的偏移，供07表负荷曲线，事件记录使用
*输入参数：主OAD，从OAD
*输出参数：数据在数据块中的偏移
*处理：
*返回：无
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
        case 0x25000200:		//累计水（热）流量
            offSet = 16;
            break;

        case 0x25020200:        //累计热量
            offSet = 4;
            break;

        case 0x25030200:		//热功率
            offSet = 8;
            break;

        case 0x25040200:		//累计工作时间
            offSet = 26;
            break;

        case 0x25050200:		//水温
        case 0x25050201:		//供水温度
            offSet = 20;
            break;

        case 0x25050202:		//回水温度
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

        if(subOad.value == pMapOopTo645->Oad.value) //事件参数设为0，表格中为1也可
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
*模块编号：
*名称：
*功能：根据OAD计算 OAD的数据在数据块中的偏移，供07表负荷曲线，事件记录使用
*输入参数：主OAD，从OAD
*输出参数：数据在数据块中的偏移
*处理：
*返回：无
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
        case 0x25000200:		//累计水（热）流量
            offSet = 16;
            break;

        case 0x25020200:        //累计热量
            offSet = 4;
            break;

        case 0x25030200:		//热功率
            offSet = 8;
            break;

        case 0x25040200:		//累计工作时间
            offSet = 26;
            break;

        case 0x25050200:		//水温
        case 0x25050201:		//供水温度
            offSet = 20;
            break;

        case 0x25050202:		//回水温度
            offSet = 23;
            break;

        case 0x25060200:        //状态
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

        if(subOad.value == pMapOopTo645->Oad.value) //事件参数设为0，表格中为1也可
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
uint16 STD188_AUTK_Recv(uint8 pipe,OOP_OAD_U mainOAD,OOP_OAD_U subOAD,uint8 *pBuf, uint16 rcvLen, cbMtrPara mCbMtrPara, DateTimeBcd_t *StoreTime,SCHEME_TASK_RUN_INFO *pCbTask,CTS_RUN_TASK_T *RunTaskInfo)
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
    TASK_FMT_TRACE ( pipe,COMM_LOG, "r任务号:pCbTask->TaskId is %d\n",pCbTask->TaskId);
    TASK_BUF_TRACE ( pipe,COMM_LOG, desbuffer,desLen);
    taskmng_meter188_data_save(desbuffer,desLen,StoreInfo,pipe);   

    return offSet;
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
uint16 STD188_AUTK_Recv_New(uint8 pipe,OOP_OAD_U mainOAD,OOP_OAD_U subOAD,uint8 *pBuf, uint16 rcvLen, cbMtrPara mCbMtrPara, DateTimeBcd_t *StoreTime,SCHEME_TASK_RUN_INFO *pCbTask,CTS_RUN_TASK_T *RunTaskInfo)
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

    //数据域-0x33
    //ReduceGap(tmpBufer, rcvLen, STD2007_GAP_VALUE);
    memcpy(sourcedata.sourcebuffer,tmpBufer,rcvLen);
    sourcedata.reclen=rcvLen;
    sourcedata.pwrType = basicinfo.pwrType;
    TASK_BUF_TRACE ( pipe,REC_LOG, sourcedata.sourcebuffer,rcvLen);
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
    TASK_FMT_TRACE ( pipe,COMM_LOG, "r任务号:pCbTask->TaskId is %d\n",pCbTask->TaskId);
    TASK_BUF_TRACE ( pipe,COMM_LOG, desbuffer,desLen);
    taskmng_meter188_data_save(desbuffer,desLen,StoreInfo,pipe);   

    return offSet;
}



