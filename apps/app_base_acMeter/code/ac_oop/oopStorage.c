/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief：规约解析中记录型数据专用存储接口，依赖libStorage.so,共用句柄
* @date： 2019.12.03
*********************************************************************
*/

#include "baseCommon.h"
#include "db_api.h"
#include "commClass.h"
#include "oadInfoList.h"
#include "oopservice.h"
#include "oopStorage.h"
#include "ac_save.h"

extern OOP_OCTETVAR64_T    MAC; //校验码

/**
*********************************************************************
* @brief：     该记录是否是需要的
* @param[in]： inReadRecord  - 筛选需要的数据
               unit          - 单个数据记录单元
* @return：    =0            - 该记录是所需
               <0            - 该记录不是所需
*********************************************************************
*/
int record_data_is_valid(READ_RECORD_T inReadRecord, RECORD_UNIT_T unit)
{
    OOP_TI_T  ti = {0};
    uint32    div = 0;
    uint8     tmType;
    int32     ret = ERR_NORMAL;
    OOP_OAD_U oad;
    uint8     OIA;
    uint8     OIA1;

    ti = inReadRecord.ti;
    tmType = inReadRecord.cType;

    oad  = inReadRecord.recordOAD.road.oadMain;
    OIA  = (uint8)(oad.value >> 24); 
    OIA1 = (uint8)((OIA>>0x04)&0x0f);
    PRTL_FMT_LOGCC("record_data_is_valid inReadRecord.recordOAD.road.oadMain = 0x%08x\n", inReadRecord.recordOAD.road.oadMain.value);
    PRTL_FMT_LOGCC("record_data_is_valid inReadRecord.recordOAD.road.oadCols.nNum = %d\n", inReadRecord.recordOAD.road.oadCols.nNum);
    uint8 i = 0;
    uint8 j = 0;
    for (i = 0; i < inReadRecord.recordOAD.road.oadCols.nNum; i++)
    {
        PRTL_FMT_LOGCC("record_data_is_valid inReadRecord.recordOAD.road.oadCols.oad[%d] = 0x%08x\n", i, inReadRecord.recordOAD.road.oadCols.oad[i].value);
    }
    PRTL_FMT_LOGCC("record_data_is_valid inReadRecord.cType = %d\n", inReadRecord.cType);
    PRTL_FMT_LOGCC("record_data_is_valid unit.recordnum = %d\n", unit.recordnum);
    for (i = 0; i < unit.recordnum; i++)
    {
        PRTL_FMT_LOGCC("record_data_is_valid unit.record[%d].road.oadMain.value = 0x%08x\n", i, unit.record[i].road.oadMain.value);
        PRTL_FMT_LOGCC("record_data_is_valid unit.record[%d].road.oadMain.nNum = %d\n", i, unit.record[i].road.oadCols.nNum);
        for (j = 0; j < unit.record[i].road.oadCols.nNum; j++)
        {
            PRTL_FMT_LOGCC("record_data_is_valid unit.record[%d].road.oadCols.oad[%d].value = 0x%08x\n", i, j,  unit.record[i].road.oadCols.oad[j].value);
        }
        PRTL_BUF_LOGCC(unit.record[i].outDataBuf, unit.record[i].outDatalen, "record_data_is_valid unit.record[%d].outDataBuf:", i);
    }
    PRTL_FMT_LOGCC("record_data_is_valid inReadRecord.ti.unit = %d\n", inReadRecord.ti.unit);
    PRTL_FMT_LOGCC("record_data_is_valid inReadRecord.ti.value = %d\n", inReadRecord.ti.value);

    if (OIA1 == 0x03) //事件类
    {
        /*检测读出来的事件是否在刷选时间范围内*/
        switch (inReadRecord.cType)
        {
        case COL_TM_START:
            if ((unit.colStartTm >= inReadRecord.cStart) && (unit.colStartTm <= inReadRecord.cEnd))
            {
                ret = ERR_OK;
            }
            else
            {
                ret = ERR_O_RANGE;
            } 
            break;
        case COL_TM_END:
            if ((unit.colEndTm >= inReadRecord.cStart) && (unit.colEndTm <= inReadRecord.cEnd))
            {
                ret = ERR_OK;
            }
            else
            {
                ret = ERR_O_RANGE;
            } 
            break;
        case COL_TM_STORE:
            if ((unit.colStartTm >= inReadRecord.cStart) && (unit.colStoreTm <= inReadRecord.cEnd))
            {
                ret = ERR_OK;
            }
            else
            {
                ret = ERR_O_RANGE;
            } 
            break;        
        default:
            ret = ERR_O_RANGE;
            break;
        }
    }
    else if (OIA1 == 0x05)
    {
        /*检测读出来的数据是否在刷选时间点上*/
        switch (ti.unit)
        {
        case 0:
            div = ti.value;
            break;
        case 1:
            div = ti.value * 60;
            break;
        case 2:
            div = ti.value * 3600;
            break;
        case 3:
            div = ti.value * 24 * 3600;
            break;
        case 4:
            break;
        case 5:
            break;
        default:
            break;
        }

        if (div == 0)
        {
            return ERR_OK;
        }

        switch (tmType)
        {
        case COL_TM_START:
            // if (unit.colStartTm%div == 0)
            // {
            //     ret = ERR_OK;
            // }
            // break;
        case COL_TM_END:
            // if (unit.colEndTm%div == 0)
            // {
            //     ret = ERR_OK;
            // }
            // break;
        case COL_TM_STORE:
            if (ti.unit == 3)
            {
                //相对上日23点59分和相对当日0点0分 
                if ((unit.colStoreTm%div == 0) || (unit.colStoreTm%div == 86340))
                {
                    ret = ERR_OK;
                }
            }
            else if (unit.colStoreTm%div == 0)
            {
                ret = ERR_OK;
            }
            break;    
        default:
            break;
        }
    }
    else
    {
        ret = ERR_OK;
    }
    
    return ret;
}

#if DESC("终端事件记录读取函数", 0)
/**
*********************************************************************
* @brief：      标准事件记录单元,经RCSD筛选后的报文
* @param[in]： unit           - 单个数据记录单元
* @param[in]： oopRcsd        - 记录列选择描述符
* @param[out]：ppOutBuf       - 698格式报文
               outLen        - 报文长度
* @return：    =0             - 成功
               <0            - 错误码
*********************************************************************
*/
int event_tmn_std_unit2fmt698(OAD_INFO_T *pOadInfo, RECORD_UNIT_T unit, RELY_INFO_T *relyInfo, uint8 **ppOutBuf, uint32 *outLen)
{
    int32 nRet = ERR_OK;
    OOP_EVENT_T event;    
    uint32 i = 0 ,j = 0;
    OOP_OAD_U tmpOAD;
    uint32 offset = 0;
    uint32 tmpOffset = 0;
    uint8 *pOutBuf  = NULL;
    uint32 tmpOutLen = DATA_LEN_MAX; //TODO 暂时这么定
    OAD_INFO_T pstOdaInfo = {0};  
    OOP_RCSD_T oopRcsd = relyInfo->oopRcsd;

    memcpy(&event, unit.record[0].outDataBuf, sizeof(event));

    ACUDP_FMT_TRACE(" nIndex(%d)\
                    \r\n tmStart(%04d-%02d-%02d %02d:%02d:%02d)\
                    \r\n tmEnd  (%04d-%02d-%02d %02d:%02d:%02d)\n", 
                    event.nIndex, event.tmStart.year, event.tmStart.month, 
                    event.tmStart.day, event.tmStart.hour, event.tmStart.minute, event.tmStart.second,
                    event.tmEnd.year, event.tmEnd.month, 
                    event.tmEnd.day, event.tmEnd.hour, event.tmEnd.minute, event.tmEnd.second);

    pOutBuf = (uint8 *)malloc(DATA_LEN_MAX);//TODO 暂时这么定
    memset(pOutBuf, 0, DATA_LEN_MAX);


    //NORMAL方法读，或者RECORD读出所有列
    if((relyInfo->method == 1) || (oopRcsd.nNum == 0))
    {

        nRet = data_to_buf_evt_std(pOadInfo, pOutBuf, DATA_LEN_MAX, &offset, &event, sizeof(event));
        if(nRet != 0)
        {
            ACUDP_FMT_TRACE("data_to_buf_evt_std failed. oda(0x%08x), dar(%d)\n", pOadInfo->pTab->oad.value, nRet);
            return nRet;
        }

        if(relyInfo->method != 1) //非NORAML方法，去掉STRUCTER类型和长度
        {
            memmove(pOutBuf, pOutBuf + 2, offset - 2);
            *outLen = offset - 2;
        }
        else
        {
            *outLen = offset;
        }

        *ppOutBuf = pOutBuf;
        
        return nRet;
    }

    for(i = 0; i < oopRcsd.nNum; i++)
    {
        tmpOAD = oopRcsd.cols[i].oad;
        ACUDP_FMT_TRACE("[%d]rcsd.oad(0x%08x)\n",i,oopRcsd.cols[i].oad.value);

        if(!class_oadinfo_get(tmpOAD, &pstOdaInfo))
        {
            ACUDP_FMT_TRACE("class_oadinfo_get failed. oad(0x%08x)\n", tmpOAD.value);
            return ERR_NORMAL;
        }
        
        switch(tmpOAD.value)
        {
            case 0x20220200:   //事件记录序号
            {
                if(basic_data_to_buf(E_DOUBLE_LONG_UNSIGNED, pOutBuf, tmpOutLen-offset, &offset, &event.nIndex, sizeof(uint32)) < 0)
                {
                    nRet = ERR_NORMAL;
                    return nRet;
                }                
            }break;
            case 0x201E0200:   //事件发生时间
            {
                if(basic_data_to_buf(E_DATETIME_S, pOutBuf, tmpOutLen-offset, &offset, &event.tmStart, sizeof(OOP_DATETIME_S_T)) < 0)
                {
                    nRet = ERR_NORMAL;
                    return nRet;
                }
            }break;
            case 0x20200200:   //事件结束时间
            {
                if(event.tmEnd.year == 0) //TODO 姑且使用0作为不存在的判据
                {
                   pOutBuf[offset++] = DT_NULL; 
                }
                else
                {
                    if(basic_data_to_buf(E_DATETIME_S, pOutBuf, tmpOutLen-offset, &offset, &event.tmEnd, sizeof(OOP_DATETIME_S_T)) < 0)
                    {
                        nRet = ERR_NORMAL;
                        return nRet;
                    }  
                }
            }break;
            case 0x20240200:   //事件发生源
            {
                if(data_to_buf_evt_source(&pstOdaInfo, pOutBuf, tmpOutLen-offset, &offset, &event.source, sizeof(OOP_EVTSOURCE_T)) < 0)
                {
                    nRet = ERR_NORMAL;
                    return nRet;
                } 
            }break;            
            case 0x33000200:   //事件上报状态
            {
                if(data_to_buf_evt_report(&pstOdaInfo, pOutBuf, tmpOutLen-offset, &offset, &event.state, sizeof(OOP_REPORTS_T)) < 0)
                {
                    nRet = ERR_NORMAL;
                    return nRet;
                }             
            }break;

            default: break;
        }

        tmpOffset = 0;
        for(j = 0; j < event.data.oadNum; j++)
        {
            if(tmpOAD.value == event.data.cols.oad[j].value)
            {
                //nRet = normal_data2fmt698(icInfo, &offset, &event.data.buf[tmpOffset], event.data.idLen[j], pOutBuf, tmpOutLen-offset);
                nRet = data_to_buf_evt_oad(tmpOAD, pOutBuf, tmpOutLen-offset, &offset, &event.data.buf[tmpOffset], event.data.idLen[j]);

                break;
            }

            tmpOffset += event.data.idLen[j];
        }
   }
    
    *ppOutBuf = pOutBuf;
    *outLen = offset;

    return nRet;
}

/**
*********************************************************************
* @brief：      终端事件转换成698格式报文
* @param[in]： unit           - 单个数据记录单元
* @param[in]： oopRcsd        - 记录列选择描述符
* @param[out]：ppOutBuf       - 698格式报文
               outLen        - 报文长度
* @return：    =0             - 成功
               <0            - 错误码
*********************************************************************
*/
int event_tmn_unit2fmt698(RECORD_UNIT_T unit, RELY_INFO_T *relyInfo, uint8 **ppOutBuf, uint32 *outLen)
{
    int32     nRet = ERR_NORMAL;
    OAD_INFO_T *pOadInfo = NULL;

    if ((unit.record[0].road.oadCols.nNum == 0)&&(unit.record[0].road.oadMain.value == 0x0))
    {
        return ERR_OPTION;
    }

    //TODO 若查到OAD为 DT_ARRAY 则 pOadInfo++
    pOadInfo = (OAD_INFO_T *)relyInfo->pOadInfo;
   
    //列举所有的事件结构体
    switch(pOadInfo->pTab->eData)
    {
        case E_OOP_EVENT_T:        //标准事件记录单元
        {
            return event_tmn_std_unit2fmt698(pOadInfo, unit, relyInfo, ppOutBuf, outLen);
        }break;   
        
        default:
        {
            ACUDP_FMT_TRACE("event read failed data type is not support. type(%d)\n", pOadInfo->pTab->eData);
            return ERR_INVALID;   
        }break;
    }

    return nRet;
}
/**
*********************************************************************
* @brief：      读取全部记录(终端事件)
* @param[in]： inReadRecord  - 筛选需要的数据
               oopRcsd       - 列选择
               readRecNum    - 读记录数量，：0：读全部记录 1：读最近的一条记录
               maxBufLen     - 最大读取buf长度
* @param[out]：outRecordBuf  - 数据记录
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
int record_tmn_event_read(DB_CLIENT hUdp, READ_RECORD_T *inReadRecord, RELY_INFO_T *relyInfo, uint32 readRecNum, uint32 maxBufLen, RECORD_BUF_T *outRecordBuf)
{
    STEP_READ_RECORD_T stepReadRecord;
    RECORD_UNIT_T      *stepRecordOut = NULL;
    uint8              *pBufTmp = NULL;
    uint32             bufLenTmp = 0;
    uint32             offset = 0; //输出报文的偏移
    uint32             numOffset = 0; //写记录数的偏移
    uint32             bufAllLen = 0;
    uint32             bufAllLenLast = 0;
    BOOL               isBufFull = FALSE;
    uint32 outRecNumAll = 0;  //总的要输出的记录条数

    int32  ret    = ERR_NORMAL;
    uint32 outLen = 0;
    uint32 handle = 0;
    uint32 recNum = 0;
    uint32 outRecNum = 0; //实际输出的记录条数
    uint8  offlen = 0;
    uint8  lenBuf[3] = {0};
    int i = 0;

    /*入参检查*/
    if ((hUdp <= 0) || (outRecordBuf == NULL) || (outRecordBuf->recordBuf == NULL))
    {
        ACUDP_FMT_TRACE("record_event_tmn_read param err!,hudp(%d), outRecordBuf(%d), recordBuf(%d)\n",
            hUdp, outRecordBuf, outRecordBuf->recordBuf);
        return ERR_PTR;
    }
    
    ACUDP_FMT_TRACE("\r\ninReadRecord.recordOAD.logicId = %d, inReadRecord.recordOAD.infoNum = %d, inReadRecord.recordOAD.road.oadMain = 0x%08x\
                    \r\ninReadRecord.recordOAD.road.oadCols.nNum = %d \n",
          inReadRecord->recordOAD.logicId, inReadRecord->recordOAD.infoNum, 
          inReadRecord->recordOAD.road.oadMain, inReadRecord->recordOAD.road.oadCols.nNum);
    
    for(i = 0; i < inReadRecord->recordOAD.road.oadCols.nNum; i++)
    {
        ACUDP_FMT_TRACE("inReadRecord.recordOAD.road.oadCols.oad[%d] = %lu(0x%08x)\n", i, inReadRecord->recordOAD.road.oadCols.oad[i].value, inReadRecord->recordOAD.road.oadCols.oad[i].value);
    }

    for(i = 0; i < 6; i++)
    {
        ACUDP_FMT_TRACE("inReadRecord.MAC[%d] = %02x\n", i, inReadRecord->MAC.value[i]);
    }

    ACUDP_FMT_TRACE("inReadRecord.ti.unit = %d, inReadRecord.ti.value = %d\n", inReadRecord->ti.unit, inReadRecord->ti.value);

    ACUDP_FMT_TRACE("relyInfo->method(%d), relyInfo->pOadInfo->nIndex(%d), relyInfo->oopRcsd.nNum(%d)\n", relyInfo->method, relyInfo->pOadInfo->nIndex, relyInfo->oopRcsd.nNum);
    for(i = 0; i < relyInfo->oopRcsd.nNum; i++)
    {
        ACUDP_FMT_TRACE("relyInfo->oopRcsd.cols[%d].oad = %lu(0x%08x)\n", i, relyInfo->oopRcsd.cols[i].oad.value,  relyInfo->oopRcsd.cols[i].oad.value);
    }

    ACUDP_FMT_TRACE("relyInfo->oopRsd.choice(%d)\n", relyInfo->oopRsd->choice);
    
    switch(relyInfo->oopRsd->choice)
    {
        case 2: ACUDP_FMT_TRACE("sel2.oad = %lu(0x%08x)\n", relyInfo->oopRsd->sel2.oad.value, relyInfo->oopRsd->sel2.oad.value); 

        case 9: ACUDP_FMT_TRACE("sel9.nLast = %d\n", relyInfo->oopRsd->sel9.nLast); 
        
        default:break;
    }

    memset(&stepReadRecord, 0, sizeof(STEP_READ_RECORD_T));

    //断点处理
    if(relyInfo->bpBreakInfo->appSlicingFlag == 1)
    {
        //倒序，则截止到上次中断的时间点,前闭后开，上次结束的时间加1
        if(inReadRecord->sortType == DOWN_SORT)
        {
            inReadRecord->cEnd = relyInfo->bpBreakInfo->tmStartSeg + 1;    
        }
        else
        {
            inReadRecord->cStart = relyInfo->bpBreakInfo->tmStartSeg;
        }
    }

    ACUDP_FMT_TRACE("inReadRecord.tmType = %d, inReadRecord.tmStart = %d, inReadRecord.tmEnd = %d, inReadRecord.sortType = %d\n",
          inReadRecord->cType, inReadRecord->cStart, inReadRecord->cEnd, inReadRecord->sortType);
    
    /*开始读记录*/
    ret = db_read_record_start(hUdp, inReadRecord, &handle, &recNum);
    if (ret != ERR_OK)
    {
        ACUDP_FMT_TRACE("db_read_record_start failed. ret(%d), reNum(%d)\n", ret, recNum);
        return ERR_START_REC;
    }

    //MAC校验
    memcpy(&stepReadRecord.MAC, &inReadRecord->MAC, sizeof(OOP_OCTETVAR64_T));

    //实际能输出的记录条数，取数据中心读出的数据条数与主站需要的记录条数的小者,要不要报错？  
    outRecNumAll = MIN(recNum, readRecNum); 
    ACUDP_FMT_TRACE("outRecNumAll(%lu), MIN(recNum(%lu), readRecNum(%lu)\n", outRecNumAll, recNum, readRecNum);

    //记录型，或普通数据非分量，需要返回记录条数
    if(relyInfo->pOadInfo->nIndex == 0)
    {
        //普通非分量数据需要数组类型
        if(relyInfo->method == 1)
        {
            outRecordBuf->recordBuf[offset++] = DT_ARRAY;
        }

        numOffset = offset;  //记录数或者数组成员数的位置

        /*M 条记录 [1] SEQUENCE OF A-RecordRow*/
        outRecordBuf->recordBuf[offset++] = outRecNumAll;   //记录数先存实际条目 

        bufAllLen = offset;
    }

    //变量太大，会使栈空间溢出，破坏调用者的栈，所以改用堆分配
    stepRecordOut = (RECORD_UNIT_T*)malloc(sizeof(RECORD_UNIT_T));
    memset(stepRecordOut, 0, sizeof(RECORD_UNIT_T));
    
    for (i = 0; i < outRecNumAll; i++)
    {
        ret = db_read_record_step(hUdp, handle, &stepReadRecord, stepRecordOut);
        if ((ret != ERR_OK) || (stepRecordOut->recordnum == 0))
        {
            PRTL_BUF_DEBUG(stepReadRecord.MAC.value, 6, "db_read_record_step failed. ret(%d), outLen(%d), mainOAD(0x%08x)\n", 
                ret, outLen, inReadRecord->recordOAD.road.oadMain.value);
            continue;
        }

        //方法2：判断范围正确性
        if(relyInfo->oopRsd->choice == 2)
        {
            /* 判断是否是所需记录单元 */
            ret = record_data_is_valid(*inReadRecord, *stepRecordOut);
            if (ret != ERR_OK)
            {
                continue;
            }  
        }

        //方法9：读取上第n次记录
        if((relyInfo->oopRsd->choice == 9) && (i+1 != relyInfo->oopRsd->sel9.nLast))
        {
            continue;
        }
        
        /* 
        A-RecordRow∷=SEQUENCE
        {
         第 1 列数据 Data，
         第 2 列数据 Data，
         …
         第 N 列数据 Data
        }*/
        ret = event_tmn_unit2fmt698(*stepRecordOut, relyInfo, &pBufTmp, &bufLenTmp);
        if (ret != ERR_OK)
        {
            db_read_record_end(hUdp, handle);
            FREE_S(pBufTmp);  
            FREE_S(stepRecordOut);  
            return ERR_NORMAL;
        }

        bufAllLenLast = bufAllLen; //存储上次的长度

        bufAllLen += bufLenTmp;    //更新本次的长度

        if(bufAllLen <= maxBufLen)
        {
            //在长度累积大于maxBufLen后，这个拷贝会越界
            memcpy(&outRecordBuf->recordBuf[bufAllLenLast], pBufTmp, bufLenTmp);
        }
        else //超长，需要分帧，记下尚未处理的记录序号和尚未处理的时间，如果第一个就超长？
        {
            outRecordBuf->recNumSeg = outRecNum; //未处理的记录序号，同时表示已处理的记录数
            //方法4 5 6 7 8
            switch (inReadRecord->cType)
            {
                case COL_TM_START:
                    outRecordBuf->breakInfo.tmStartSeg = stepRecordOut->colStartTm;
                    break;
                case COL_TM_END:
                    outRecordBuf->breakInfo.tmStartSeg = stepRecordOut->colEndTm;
                    break;
                case COL_TM_STORE:
                    outRecordBuf->breakInfo.tmStartSeg = stepRecordOut->colStoreTm;
                    break;       
                default:
                    FREE_S(pBufTmp);
                    FREE_S(stepRecordOut);  
                    return ERR_PTR;
                break;
            }
            
            isBufFull = TRUE;
            outRecordBuf->breakInfo.appSlicingFlag = 1;

            break;
        }

        //正常跳出，为已经处理的总记录数，分帧跳出，为已经处理的记录数
        outRecNum++;

        FREE_S(pBufTmp);    
    }

    FREE_S(stepRecordOut);

    /*结束读记录*/
    db_read_record_end(hUdp, handle);

    if (!isBufFull)
    {
        outRecordBuf->bufLen = bufAllLen;
        //outRecordBuf->recordBuf[numOffset] = outRecNumAll;
        
        if (relyInfo->bpBreakInfo->appSlicingFlag == 0x01)
        {
            outRecordBuf->breakInfo.appSlicingFlag = 0x01;
            outRecordBuf->breakInfo.isEnd = 0x01;                          //末尾帧
            outRecordBuf->breakInfo.seq = relyInfo->bpBreakInfo->seq + 1;   //序号加1
        }
        else
        {
            /*清除断点信息*/
            memset(&outRecordBuf->breakInfo, 0, sizeof(BP_INFO_T));
        }
    }
    else
    {
        outRecordBuf->breakInfo.appSlicingFlag = 0x01;
        outRecordBuf->breakInfo.isEnd = 0x00;                              //不是末尾帧
        
        if (relyInfo->bpBreakInfo->appSlicingFlag == 0x00)
        {
            outRecordBuf->breakInfo.seq = BRK_SEQ_START;                   //首次序号
        }
        else
        {
            outRecordBuf->breakInfo.seq = relyInfo->bpBreakInfo->seq + 1;   //序号加1
        }
        
        outRecordBuf->bufLen = bufAllLenLast;
        //outRecordBuf->recordBuf[numOffset] = i; //跳出来前，已经处理的记录条数
    }

    //记录型，或普通数据非分量，需要返回记录条数
    if(relyInfo->pOadInfo->nIndex == 0)
    {
        //长度域超过后也要采用A-XDR编码
        if (outRecNum > 0x80)
        {
            offlen = set_len_offset(outRecNum, lenBuf);
            memmove(&outRecordBuf->recordBuf[numOffset+offlen], &outRecordBuf->recordBuf[numOffset+1], outRecordBuf->bufLen - numOffset - 1);
            memcpy(&outRecordBuf->recordBuf[numOffset], lenBuf, offlen);
            outRecordBuf->bufLen += (offlen - 1); //减掉原来预留的1字节长度
        }
        else
        {
            outRecordBuf->recordBuf[numOffset] = outRecNum;   //填写总记录数 
        }    
    }
    else if(outRecNum == 0) //非分量，且记录条数为0，则返回NULL
    {
        outRecordBuf->recordBuf[offset++] = DT_NULL;
        outRecordBuf->bufLen++;
    }
    
    ACUDP_FMT_TRACE("record_event_tmn_read result: outRecNum(%lu), appSlicingFlag(%d), isEnd(%d)\n", outRecNum, outRecordBuf->breakInfo.appSlicingFlag, outRecordBuf->breakInfo.isEnd);

    return ERR_OK;
}

#endif

#if DESC("终端冻结记录读取函数",0)

/**
*********************************************************************
* @brief：      终端事件转换成698格式报文
* @param[in]： unit           - 单个数据记录单元
* @param[in]： oopRcsd        - 记录列选择描述符
* @param[out]：ppOutBuf       - 698格式报文
               outLen        - 报文长度
* @return：    =0             - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E sub_oad_unit2fmt698(RECORD_UNIT_T *unit, RELY_INFO_T *relyInfo, uint8 **ppOutBuf, uint32 *outLen)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    IC_OAD_INFO_T icInfo;
    DATA_CONVERT_T convert;
    uint32 sOffset = 0;
    uint32 dOffset = 0;
    uint8      *pOutBuf  = NULL;
    uint32 i= 0 ,j = 0;
    DateTimeHex_t oopTime_s;

    MEMZERO(&convert, sizeof(DATA_CONVERT_T));
    MEMZERO(&oopTime_s, sizeof(DateTimeHex_t));
   
    icInfo.choice = 0;
    pOutBuf = (uint8 *)malloc(DATA_LEN_MAX);//暂时这么定
    MEMZERO(pOutBuf, DATA_LEN_MAX);

    //普通方法还要返回结构成员和结构成员数
    if(relyInfo->method == 1)
    {
        pOutBuf[dOffset++] = DT_STRUCTURE;
        pOutBuf[dOffset++] = relyInfo->oopRcsd.nNum;
    }

    convert.dstBuf = pOutBuf;
    convert.dstLen = DATA_LEN_MAX;
    convert.sOffset = &sOffset;   
    convert.dOffset = &dOffset;

    //每次解析其中一个OAD
    for(i = 0; i < relyInfo->oopRcsd.nNum; i++)
    {
        sOffset = 0;
        
        for(j = 0; j < unit->recordnum; j++)
        {
            if(unit->record[j].road.oadCols.oad[0].value != relyInfo->oopRcsd.cols[i].oad.value)
            {
                continue;
            }

            icInfo.oad.value = unit->record[j].road.oadCols.oad[0].value;
            convert.srcBuf = unit->record[j].outDataBuf;
            convert.srcLen = unit->record[j].outDatalen;

            dar = normal_data_to_fmt698(&icInfo, &convert);
            if(dar != DATA_SUCCESS)
            {
                ACUDP_FMT_TRACE("[%d] normal_data_to_fmt698 failed. oad(0x%08x)\n", i, icInfo.oad.value);
                return dar;
            }

            break;
        }

        if(j == unit->recordnum)
        {
            pOutBuf[dOffset++] = DT_NULL;
            ACUDP_FMT_TRACE("[%d] not find objs, data is null. oad(0x%08x)\n", i, icInfo.oad.value);
        }

        PRTL_BUF_TRACE(pOutBuf, dOffset, "[%d] oadCols(0x%08x)\n", i, relyInfo->oopRcsd.cols[i].oad.value);
    }

    *ppOutBuf = pOutBuf;
    *outLen = dOffset;

    return dar;
}


/**
*********************************************************************
* @brief：      读取全部记录(终端冻结)
* @param[in]： inReadRecord  - 筛选需要的数据
               oopRcsd       - 列选择
               readRecNum    - 读记录数量，：0：读全部记录 1：读最近的一条记录
               maxBufLen     - 最大读取buf长度
* @param[out]：outRecordBuf  - 数据记录
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
int record_tmn_frozen_read(DB_CLIENT hUdp, READ_RECORD_T *inReadRecord, RELY_INFO_T *relyInfo, uint32 readRecNum, uint32 maxBufLen, RECORD_BUF_T *outRecordBuf)
{
    STEP_READ_RECORD_T stepReadRecord;
    RECORD_UNIT_T      stepRecordOut;
    uint8              *pBufTmp = NULL;
    uint32             bufLenTmp = 0;
    uint32             offset = 0; //输出报文的偏移
    uint32             numOffset = 0; //写记录数的偏移
    uint32             bufAllLen = 0;
    uint32             bufAllLenLast = 0;
    BOOL               isBufFull = FALSE;
    uint32 outRecNumAll = 0;  //总的要输出的记录条数

    int32  ret    = ERR_NORMAL;
    uint32 outLen = 0;
    uint32 handle = 0;
    uint32 recNum = 0;
    uint32 outRecNum = 0; //实际输出的记录条数
    uint8  offlen = 0;
    uint8  lenBuf[3] = {0};
    int i = 0;

    /*入参检查*/
    if ((hUdp <= 0) || (outRecordBuf == NULL) || (outRecordBuf->recordBuf == NULL))
    {
        ACUDP_FMT_TRACE("record_tmn_frozen_read param err!,hudp(%d), outRecordBuf(%d), recordBuf(%d)\n",
            hUdp, outRecordBuf, outRecordBuf->recordBuf);
        return ERR_PTR;
    }
    
    ACUDP_FMT_TRACE("\r\ninReadRecord.recordOAD.logicId = %d, inReadRecord.recordOAD.infoNum = %d, inReadRecord.recordOAD.road.oadMain = 0x%08x\
                    \r\ninReadRecord.recordOAD.road.oadCols.nNum = %d \n",
          inReadRecord->recordOAD.logicId, inReadRecord->recordOAD.infoNum, 
          inReadRecord->recordOAD.road.oadMain, inReadRecord->recordOAD.road.oadCols.nNum);
    
    for(i = 0; i < inReadRecord->recordOAD.road.oadCols.nNum; i++)
    {
        ACUDP_FMT_TRACE("inReadRecord.recordOAD.road.oadCols.oad[%d] = %lu(0x%08x)\n", i, inReadRecord->recordOAD.road.oadCols.oad[i].value, inReadRecord->recordOAD.road.oadCols.oad[i].value);
    }

    for(i = 0; i < 6; i++)
    {
        ACUDP_FMT_TRACE("inReadRecord.MAC[%d] = %02x\n", i, inReadRecord->MAC.value[i]);
    }

    ACUDP_FMT_TRACE("inReadRecord.ti.unit = %d, inReadRecord.ti.value = %d\n", inReadRecord->ti.unit, inReadRecord->ti.value);

    ACUDP_FMT_TRACE("relyInfo->method(%d), relyInfo->pOadInfo->nIndex(%d), relyInfo->oopRcsd.nNum(%d)\n", relyInfo->method, relyInfo->pOadInfo->nIndex, relyInfo->oopRcsd.nNum);
    for(i = 0; i < relyInfo->oopRcsd.nNum; i++)
    {
        ACUDP_FMT_TRACE("relyInfo->oopRcsd.cols[%d].oad = %lu(0x%08x)\n", i, relyInfo->oopRcsd.cols[i].oad.value,  relyInfo->oopRcsd.cols[i].oad.value);
    }

    ACUDP_FMT_TRACE("relyInfo->oopRsd.choice(%d)\n", relyInfo->oopRsd->choice);
    
    switch(relyInfo->oopRsd->choice)
    {
        case 2: ACUDP_FMT_TRACE("sel2.oad = %lu(0x%08x)\n", relyInfo->oopRsd->sel2.oad.value, relyInfo->oopRsd->sel2.oad.value); 

        case 9: ACUDP_FMT_TRACE("sel9.nLast = %d\n", relyInfo->oopRsd->sel9.nLast); 
        
        default:break;
    }

    memset(&stepReadRecord, 0, sizeof(STEP_READ_RECORD_T));
    memset(&stepRecordOut, 0, sizeof(RECORD_UNIT_T));

    //断点处理
    if(relyInfo->bpBreakInfo->appSlicingFlag == 1)
    {
        //倒序，则截止到上次中断的时间点,前闭后开，上次结束的时间加1
        if(inReadRecord->sortType == DOWN_SORT)
        {
            inReadRecord->cEnd = relyInfo->bpBreakInfo->tmStartSeg + 1;    
        }
        else
        {
            inReadRecord->cStart = relyInfo->bpBreakInfo->tmStartSeg;
        }
    }

    ACUDP_FMT_TRACE("inReadRecord.tmType = %d, inReadRecord.tmStart = %d, inReadRecord.tmEnd = %d, inReadRecord.sortType = %d\n",
          inReadRecord->cType, inReadRecord->cStart, inReadRecord->cEnd, inReadRecord->sortType);
    
    /*开始读记录*/
    ret = db_read_record_start(hUdp, inReadRecord, &handle, &recNum);
    if (ret != ERR_OK)
    {
        ACUDP_FMT_TRACE("db_read_record_start failed. ret(%d), reNum(%d)\n", ret, recNum);
        return ERR_START_REC;
    }

    //MAC校验
    memcpy(&stepReadRecord.MAC, &inReadRecord->MAC, sizeof(OOP_OCTETVAR64_T));

    //实际能输出的记录条数，取数据中心读出的数据条数与主站需要的记录条数的小者,要不要报错？  
    outRecNumAll = MIN(recNum, readRecNum); 
    ACUDP_FMT_TRACE("outRecNumAll(%lu), MIN(recNum(%lu), readRecNum(%lu)\n", outRecNumAll, recNum, readRecNum);

    //记录型，或普通数据非分量，需要返回记录条数
    if(relyInfo->pOadInfo->nIndex == 0)
    {
        //普通非分量数据需要数组类型
        if(relyInfo->method == 1)
        {
            outRecordBuf->recordBuf[offset++] = DT_ARRAY;
        }

        numOffset = offset;  //记录数或者数组成员数的位置

        /*M 条记录 [1] SEQUENCE OF A-RecordRow*/
        outRecordBuf->recordBuf[offset++] = outRecNumAll;   //记录数先存实际条目 

        bufAllLen = offset;
    }
    
    for (i = 0; i < outRecNumAll; i++)
    {
        ret = db_read_record_step(hUdp, handle, &stepReadRecord, &stepRecordOut);
        if ((ret < ERR_OK) || (stepRecordOut.recordnum == 0))
        {
            PRTL_BUF_DEBUG(stepReadRecord.MAC.value, 6, "db_read_record_step failed. ret(%d), outLen(%d), mainOAD(0x%08x)\n", 
                ret, outLen, inReadRecord->recordOAD.road.oadMain.value);
            continue;
        }

        //方法2：判断范围正确性
        if(relyInfo->oopRsd->choice == 2)
        {
            /* 判断是否是所需记录单元 */
            ret = record_data_is_valid(*inReadRecord, stepRecordOut);
            if (ret < ERR_OK)
            {
                continue;
            }  
        }

        //方法9：读取上第n次记录
        if((relyInfo->oopRsd->choice == 9) && (i+1 != relyInfo->oopRsd->sel9.nLast))
        {
            continue;
        }
        
        ret = sub_oad_unit2fmt698(&stepRecordOut, relyInfo, &pBufTmp, &bufLenTmp);
        if (ret != ERR_OK)
        {
            db_read_record_end(hUdp, handle);
            FREE_S(pBufTmp);             
            return ERR_NORMAL;
        }

        bufAllLenLast = bufAllLen; //存储上次的长度

        bufAllLen += bufLenTmp;    //更新本次的长度

        if(bufAllLen <= maxBufLen)
        {
            //在长度累积大于maxBufLen后，这个拷贝会越界
            memcpy(&outRecordBuf->recordBuf[bufAllLenLast], pBufTmp, bufLenTmp);
        }
        else //超长，需要分帧，记下尚未处理的记录序号和尚未处理的时间，如果第一个就超长？
        {
            outRecordBuf->recNumSeg = outRecNum; //未处理的记录序号，同时表示已处理的记录数
            //方法4 5 6 7 8
            switch (inReadRecord->cType)
            {
                case COL_TM_START:
                    outRecordBuf->breakInfo.tmStartSeg = stepRecordOut.colStartTm;
                    break;
                case COL_TM_END:
                    outRecordBuf->breakInfo.tmStartSeg = stepRecordOut.colEndTm;
                    break;
                case COL_TM_STORE:
                    outRecordBuf->breakInfo.tmStartSeg = stepRecordOut.colStoreTm;
                    break;       
                default:
                    FREE_S(pBufTmp);
                    return ERR_PTR;
                break;
            }
            
            isBufFull = TRUE;
            outRecordBuf->breakInfo.appSlicingFlag = 1;

            break;
        }

        //正常跳出，为已经处理的总记录数，分帧跳出，为已经处理的记录数
        outRecNum++;

        FREE_S(pBufTmp);    
    }

    /*结束读记录*/
    db_read_record_end(hUdp, handle);

    if (!isBufFull)
    {
        outRecordBuf->bufLen = bufAllLen;
        //outRecordBuf->recordBuf[numOffset] = outRecNumAll;
        
        if (relyInfo->bpBreakInfo->appSlicingFlag == 0x01)
        {
            outRecordBuf->breakInfo.appSlicingFlag = 0x01;
            outRecordBuf->breakInfo.isEnd = 0x01;                          //末尾帧
            outRecordBuf->breakInfo.seq = relyInfo->bpBreakInfo->seq + 1;   //序号加1
        }
        else
        {
            /*清除断点信息*/
            memset(&outRecordBuf->breakInfo, 0, sizeof(BP_INFO_T));
        }
    }
    else
    {
        outRecordBuf->breakInfo.appSlicingFlag = 0x01;
        outRecordBuf->breakInfo.isEnd = 0x00;                              //不是末尾帧
        
        if (relyInfo->bpBreakInfo->appSlicingFlag == 0x00)
        {
            outRecordBuf->breakInfo.seq = BRK_SEQ_START;                   //首次序号
        }
        else
        {
            outRecordBuf->breakInfo.seq = relyInfo->bpBreakInfo->seq + 1;   //序号加1
        }
        
        outRecordBuf->bufLen = bufAllLenLast;
        //outRecordBuf->recordBuf[numOffset] = i; //跳出来前，已经处理的记录条数
    }

    //记录型，或普通数据非分量，需要返回记录条数
    if(relyInfo->pOadInfo->nIndex == 0)
    {
        //长度域超过后也要采用A-XDR编码
        if (outRecNum > 0x80)
        {
            offlen = set_len_offset(outRecNum, lenBuf);
            memmove(&outRecordBuf->recordBuf[numOffset+offlen], &outRecordBuf->recordBuf[numOffset+1], outRecordBuf->bufLen - numOffset - 1);
            memcpy(&outRecordBuf->recordBuf[numOffset], lenBuf, offlen);
            outRecordBuf->bufLen += (offlen - 1); //减掉原来预留的1字节长度
        }
        else
        {
            outRecordBuf->recordBuf[numOffset] = outRecNum;   //填写总记录数 
        }    
    }
    else if(outRecNum == 0) //非分量，且记录条数为0，则返回NULL
    {
        outRecordBuf->recordBuf[offset++] = DT_NULL;
        outRecordBuf->bufLen++;
    }    
    
    ACUDP_FMT_TRACE("record_event_tmn_read result: outRecNum(%lu), appSlicingFlag(%d), isEnd(%d)\n", outRecNum, outRecordBuf->breakInfo.appSlicingFlag, outRecordBuf->breakInfo.isEnd);

    return ERR_OK;
}

#endif

/**
*********************************************************************
* @brief：     根据rsd判断读的结果是不是唯一
* @param[in]： oopRsd   - 行刷选
* @param[out]：
* @return：    TRUE  唯一
               FALSE 非唯一
*********************************************************************
*/
BOOL is_result_only(OOP_PRSD_T oopRsd)
{
    BOOL ret = FALSE;
    time_t timeFrom = 0;
    time_t timeTo = 0;

    switch (oopRsd.choice)
    {
    case 1:
        {
            if (oopRsd.sel1.oad.value == 0x20210200)
            {
                if (oopRsd.sel1.value.type == DT_DATETIME_S)
                {
                    ret = TRUE;
                }
            }
        }
    case 2:
        {
            if (oopRsd.sel2.oad.value == 0x20210200)
            {
                if ((oopRsd.sel2.from.type == DT_DATETIME_S) && 
                    (oopRsd.sel2.to.type == DT_DATETIME_S))
                {
                    if ((oopRsd.sel2.span.type == DT_TI) &&
                        (oopRsd.sel2.span.ti.unit == TI_DAY) &&
                        (oopRsd.sel2.span.ti.value == 1))
                    {
                        DT_DateTimeShort2Time(&oopRsd.sel2.from.tmVal, &timeFrom);
                        DT_DateTimeShort2Time(&oopRsd.sel2.to.tmVal, &timeTo);

                        if ((timeTo - timeFrom)/(24*60*60) == 1)
                        {
                            ret = TRUE;
                        }
                    }
                    else if (oopRsd.sel2.span.type == DT_NULL)
                    {
                        ret = TRUE;
                    } 
                }
            }
        }
        break;
    case 9:
        {
            ret = TRUE;
        }
        break;
    default:
        break;
    }

    return ret;
}

#if DESC("事件记录读取函数",1)

/**
*********************************************************************
* @brief：      标准事件记录单元,经RCSD筛选后的报文
* @param[in]： unit           - 单个数据记录单元
* @param[in]： oopRcsd        - 记录列选择描述符
* @param[out]：ppOutBuf       - 698格式报文
               outLen        - 报文长度
* @return：    =0             - 成功
               <0            - 错误码
*********************************************************************
*/
int event_std_unit2fmt698(RECORD_INFO_T *recordInfo, RECORD_UNIT_T *unit, BUFFER_INFO_T *outRecord)
{
    int32 nRet = ERR_OK;
    OOP_EVENT_T event;    
    uint32 i = 0 ,j = 0;
    OOP_OAD_U tmpOAD;
    uint32 offset  = *outRecord->offset;  //输出报文的偏移
    uint32 tmpOffset = 0;
    OAD_INFO_T pstOdaInfo = {0};  
    OOP_RCSD_T oopRcsd = recordInfo->oopRcsd;
    uint8 *outBuffer = (uint8*)outRecord->buffer;

    memcpy(&event, unit->record[0].outDataBuf, sizeof(event));

    ACUDP_FMT_TRACE(" nIndex(%d)\
                    \r\n tmStart(%04d-%02d-%02d %02d:%02d:%02d)\
                    \r\n tmEnd  (%04d-%02d-%02d %02d:%02d:%02d)\n", 
                    event.nIndex, event.tmStart.year, event.tmStart.month, 
                    event.tmStart.day, event.tmStart.hour, event.tmStart.minute, event.tmStart.second,
                    event.tmEnd.year, event.tmEnd.month, 
                    event.tmEnd.day, event.tmEnd.hour, event.tmEnd.minute, event.tmEnd.second);

    //NORMAL方法读，或者RECORD读出所有列
    if((recordInfo->append.bNormal == TRUE) || (oopRcsd.nNum == 0))
    {

        nRet = data_to_buf_evt_std(recordInfo->append.pOadInfo, outRecord->buffer, outRecord->bufLen, &offset, &event, sizeof(event));
        if(nRet != 0)
        {
            ACUDP_FMT_TRACE("data_to_buf_evt_std failed. oda(0x%08x), dar(%d)\n", recordInfo->append.pOadInfo->pTab->oad.value, nRet);
            return nRet;
        }

        if(recordInfo->append.bNormal != TRUE) //非NORAML方法，去掉STRUCTER类型和长度
        {
            memmove(outRecord->buffer, outRecord->buffer + 2, offset - 2);
            *outRecord->offset = offset - 2;
        }
        else
        {
            *outRecord->offset = offset;
        }
        
        return nRet;
    }

    for(i = 0; i < oopRcsd.nNum; i++)
    {
        tmpOAD = oopRcsd.cols[i].oad;
        ACUDP_FMT_TRACE("[%d]rcsd.oad(0x%08x)\n",i,oopRcsd.cols[i].oad.value);

        if(!class_oadinfo_get(tmpOAD, &pstOdaInfo))
        {
            ACUDP_FMT_TRACE("class_oadinfo_get failed. oad(0x%08x)\n", tmpOAD.value);
            return ERR_NORMAL;
        }
        
        switch(tmpOAD.value)
        {
            case 0x20220200:   //事件记录序号
            {
                if(basic_data_to_buf(E_DOUBLE_LONG_UNSIGNED, outRecord->buffer, outRecord->bufLen-offset, &offset, &event.nIndex, sizeof(uint32)) < 0)
                {
                    nRet = ERR_NORMAL;
                    return nRet;
                }                
            }break;
            case 0x201E0200:   //事件发生时间
            {
                if(basic_data_to_buf(E_DATETIME_S, outRecord->buffer, outRecord->bufLen-offset, &offset, &event.tmStart, sizeof(OOP_DATETIME_S_T)) < 0)
                {
                    nRet = ERR_NORMAL;
                    return nRet;
                }
            }break;
            case 0x20200200:   //事件结束时间
            {
                if(event.tmEnd.year == 0) //TODO 姑且使用0作为不存在的判据
                {
                   outBuffer[offset++] = DT_NULL; 
                }
                else
                {
                    if(basic_data_to_buf(E_DATETIME_S, outRecord->buffer, outRecord->bufLen-offset, &offset, &event.tmEnd, sizeof(OOP_DATETIME_S_T)) < 0)
                    {
                        nRet = ERR_NORMAL;
                        return nRet;
                    }  
                }
            }break;
            case 0x20240200:   //事件发生源
            {
                if(data_to_buf_evt_source(&pstOdaInfo, outRecord->buffer, outRecord->bufLen-offset, &offset, &event.source, sizeof(OOP_EVTSOURCE_T)) < 0)
                {
                    nRet = ERR_NORMAL;
                    return nRet;
                } 
            }break;            
            case 0x33000200:   //事件上报状态
            {
                if(data_to_buf_evt_report(&pstOdaInfo, outRecord->buffer, outRecord->bufLen-offset, &offset, &event.state, sizeof(OOP_REPORTS_T)) < 0)
                {
                    nRet = ERR_NORMAL;
                    return nRet;
                }             
            }break;

            default: break;
        }

        tmpOffset = 0;
        for(j = 0; j < event.data.oadNum; j++)
        {
            if(tmpOAD.value == event.data.cols.oad[j].value)
            {
                //nRet = normal_data2fmt698(icInfo, &offset, &event.data.buf[tmpOffset], event.data.idLen[j], pOutBuf, tmpOutLen-offset);
                nRet = data_to_buf_evt_oad(tmpOAD, outRecord->buffer, outRecord->bufLen-offset, &offset, &event.data.buf[tmpOffset], event.data.idLen[j]);

                break;
            }

            tmpOffset += event.data.idLen[j];
        }
   }
    
    *outRecord->offset = offset;

    return nRet;
}

/**
*********************************************************************
* @brief：      终端事件转换成698格式报文
* @param[in]： unit           - 单个数据记录单元
* @param[in]： oopRcsd        - 记录列选择描述符
* @param[out]：ppOutBuf       - 698格式报文
               outLen        - 报文长度
* @return：    =0             - 成功
               <0            - 错误码
*********************************************************************
*/
int event_unit2fmt698(RECORD_INFO_T *recordInfo, RECORD_UNIT_T *unit, BUFFER_INFO_T *outRecord)
{
    int32     nRet = ERR_NORMAL;
    OAD_INFO_T *pOadInfo = NULL;

    if ((unit->record[0].road.oadCols.nNum == 0)&&(unit->record[0].road.oadMain.value == 0x0))
    {
        return ERR_OPTION;
    }

    //TODO 若查到OAD为 DT_ARRAY 则 pOadInfo++
    pOadInfo = (OAD_INFO_T *)recordInfo->append.pOadInfo;
   
    //列举所有的事件结构体
    switch(pOadInfo->pTab->eData)
    {
        case E_OOP_EVENT_T:        //标准事件记录单元
        {
            return event_std_unit2fmt698(recordInfo, unit, outRecord);
        }break;   
        
        default:
        {
            ACUDP_FMT_TRACE("event read failed data type is not support. type(%d)\n", pOadInfo->pTab->eData);
            return ERR_INVALID;   
        }break;
    }

    return nRet;
}

/**
*********************************************************************
* @brief：      获取读事件记录参数
* @param[in]： recordInfo     - 转换前的参数
* @param[out]: readRecord    - 转换后的参数
* @return：    =0             - 成功
              <0             - 错误码
*********************************************************************
*/
int event_param_get(RECORD_INFO_T *recordInfo, READ_RECORD_T *readRecord, uint32 *stepNum)
{
    readRecord->MAC = MAC;
    readRecord->recordOAD.infoNum = recordInfo->append.infoNum;
    readRecord->recordOAD.logicId = 1;

    //事件主OAD为0，次OAD为事件，读出为事件整体，需要根据RSCD筛选
    readRecord->recordOAD.road.oadMain.value = 0;
    readRecord->recordOAD.road.oadCols.nNum = 1;
    readRecord->recordOAD.road.oadCols.oad[0].value = recordInfo->oopOad.value;

    switch(recordInfo->oopRsd.choice)
    {
        case 2:
        {
            //对于事件来讲，筛选的OAD只支持冻结时间
            if((recordInfo->oopRsd.sel2.oad.value == 0x201E0200)||(recordInfo->oopRsd.sel2.oad.value == 0x20200200))
            {
                readRecord->sortType = UP_SORT;
                
                //方法OAD目前支持持时标类OAD
                if(recordInfo->oopRsd.sel2.oad.value == 0x201E0200) //事件发生时间
                {
                    readRecord->cType = COL_TM_START;
                }
                else if(recordInfo->oopRsd.sel2.oad.value == 0x20200200) //事件结束时间
                {
                    readRecord->cType = COL_TM_END;
                }

                if(recordInfo->oopRsd.sel2.from.type != DT_DATETIME_S)
                {
                    ACUDP_FMT_TRACE("not support. type = %d\n", recordInfo->oopRsd.sel2.from.type);
                    return ERR_TYPE;    
                }

                DT_DateTimeShort2Time(&recordInfo->oopRsd.sel2.from.tmVal, (time_t*)&readRecord->cStart);
                // 注：+8个小时(28800)转成格林时间
                readRecord->cStart += COMPENSATION_TIME;

                if(recordInfo->oopRsd.sel2.to.type != DT_DATETIME_S)
                {
                    ACUDP_FMT_TRACE("not support. type = %d\n", recordInfo->oopRsd.sel2.to.type);
                    return ERR_TYPE;    
                }

                DT_DateTimeShort2Time(&recordInfo->oopRsd.sel2.to.tmVal, (time_t*)&readRecord->cEnd);
                // 注：+8个小时(28800)转成格林时间
                readRecord->cEnd += COMPENSATION_TIME;

                //RSD的范围是前闭后开区间，即[起始值，结束值）
                if(readRecord->cStart >= readRecord->cEnd)
                {
                    ACUDP_FMT_TRACE("not support. region[%d, %d)\n", readRecord->cStart, readRecord->cEnd);
                    return ERR_O_RANGE;
                }

                if((recordInfo->oopRsd.sel2.span.type != DT_TI)&&(recordInfo->oopRsd.sel2.span.type != DT_NULL))
                {
                    ACUDP_FMT_TRACE("not support. type = %d\n", recordInfo->oopRsd.sel2.span.type);
                    return ERR_TYPE;    
                }

                memcpy_s(&readRecord->ti, sizeof(OOP_TI_T), &recordInfo->oopRsd.sel2.span.ti, sizeof(OOP_TI_T));

                *stepNum = 1;
            }
            else
            {
                ACUDP_FMT_TRACE("not support. oad = %lu(0x%08x)\n", recordInfo->oopRsd.sel2.oad.value, recordInfo->oopRsd.sel2.oad.value);
                return ERR_O_RANGE;
            }
        }break;        
 
        case 9:
        {
            readRecord->cType = COL_TM_STORE;
            readRecord->sortType = DOWN_SORT;
            readRecord->cStart = 0;
            readRecord->cEnd = time(NULL) + 1 + COMPENSATION_TIME;
        }break;
    }

    return ERR_OK;
}

/**
*********************************************************************
* @brief：      读取事件记录
* @param[in]： clientfd       - 数据中心句柄
               recordInfo    - 读记录信息
* @param[out]：outRecord      - 输出记录信息
* @return：    =0             - 成功
              <0             - 错误码
*********************************************************************
*/
int record_event_read(DB_CLIENT clientfd, RECORD_INFO_T *recordInfo, BUFFER_INFO_T *outRecord)
{
    int ret = 0;
    STEP_READ_RECORD_T stepRead;
    RECORD_UNIT_T      stepOut;
    READ_RECORD_T readRecord;
    uint32 stepHandle = 0;  //单步读句柄，最多4轮  
    uint32 recNum     = 0;  //记录条数   
    uint32 outRecNumAll = 0;  //总的要输出的记录条数
    uint32 ctrlRecNum = 0xffffffff;
    uint32 stepNum = 0; //单步读轮数    
    uint32 offset     = *outRecord->offset;  //输出报文的偏移
    uint32 numOffset  = 0;  //写记录数的偏移    
    uint32 i          = 0;
    uint32 outRecNum  = 0; //实际输出的记录条数
    uint32             bufAllLen = 0;
    uint32             bufAllLenLast = 0;
    BOOLEAN               isBufFull = FALSE;
    uint8  offlen = 0;
    uint8  lenBuf[3] = {0};
    uint8 *outBuffer = (uint8*)outRecord->buffer;
    BUFFER_INFO_T toutBuffer;
    uint32 tOffset = 0;
    uint8 tmpBuf[DATA_LEN_MAX] = {0};

    /*入参检查*/
    if((recordInfo == NULL) || (outRecord == NULL))
    {
        ACUDP_FMT_TRACE("record_stat_read param check failed. recordInfo(%d), outBuffer(%d)\n", recordInfo, outRecord);
        return ERR_PTR;
    }

    MEMZERO(&readRecord, sizeof(readRecord));
    MEMZERO(&stepRead, sizeof(stepRead));
    MEMZERO(&stepOut, sizeof(stepOut));
    MEMZERO(&toutBuffer, sizeof(toutBuffer));

    //获取数据中心读取参数
    InitEventPara();
    ret = event_param_get(recordInfo, &readRecord, &stepNum);
    if(ret == ERR_O_RANGE) //超出范围的OAD
    {
        ctrlRecNum = 0;
    }
    else if(ret != ERR_OK)
    {
        return DATA_TYPE_UNMATCHED;
    }

    ACUDP_FMT_TRACE("\r\nreadRecord.recordOAD.logicId = %d, readRecord.recordOAD.infoNum = %d, readRecord.recordOAD.road.oadMain = 0x%08x\
                    \r\nreadRecord.recordOAD.road.oadCols.nNum = %d \n",
                    readRecord.recordOAD.logicId, readRecord.recordOAD.infoNum, 
                    readRecord.recordOAD.road.oadMain, readRecord.recordOAD.road.oadCols.nNum);
    
    for(i = 0; i < readRecord.recordOAD.road.oadCols.nNum; i++)
    {
        ACUDP_FMT_TRACE("readRecord.recordOAD.road.oadCols.oad[%d] = %lu(0x%08x)\n", i, readRecord.recordOAD.road.oadCols.oad[i].value, readRecord.recordOAD.road.oadCols.oad[i].value);
    }

    //如果有断点，由于是倒序，则截止到上次中断的时间点，前闭后开，上次结束的时间的加1
    if((recordInfo->append.bpInfo != NULL)&&(recordInfo->append.bpInfo->appSlicingFlag == 1))
    {
        readRecord.cEnd = recordInfo->append.bpInfo->tmStartSeg + 1;    
    }

    ACUDP_FMT_TRACE("readRecord.ti.unit = %d, readRecord.ti.value = %d, readRecord.cType = %d, readRecord.tmStart = %d, readRecord.tmEnd = %d, readRecord.sortType = %d\n", 
        readRecord.ti.unit, readRecord.ti.value,
        readRecord.cType, readRecord.cStart, readRecord.cEnd, readRecord.sortType);

    /*开始读记录*/
    ret = db_read_record_start(clientfd, &readRecord, &stepHandle, &recNum);
    if (ret != ERR_OK)
    {
        ACUDP_FMT_TRACE("db_read_record_start failed. ret(%d), reNum(%d)\n", ret, recNum);
        return ERR_START_REC;
    }

    outRecNumAll = MIN(recNum, ctrlRecNum); 
    ACUDP_FMT_TRACE("outRecNumAll(%lu), MIN(recNum(%lu), ctrlRecNum(%lu)\n", outRecNumAll, recNum, ctrlRecNum);

    //MAC校验
    memcpy(&stepRead.MAC, &readRecord.MAC, sizeof(OOP_OCTETVAR64_T));

    for(i = 0; i < 6; i++)
    {
        ACUDP_FMT_TRACE("stepRead.MAC[%d] = %02x\n", i, stepRead.MAC.value[i]);
    }

    numOffset = offset;  //记录数或者数组成员数的位置
    
    /*M 条记录 [1] SEQUENCE OF A-RecordRow*/
    outBuffer[offset++] = outRecNumAll;

    // 如果outRecNumAll为0，且结果具有唯一性，每个CSD对应填NULL
    if ((outRecNumAll == 0) && (is_result_only(recordInfo->oopRsd)))
    {
        ACUDP_FMT_TRACE("outRecNumAll(%lu), and the result is only\n", outRecNumAll);
        outRecNum = 1;
        for (i = 0; i < recordInfo->oopRcsd.nNum; i ++)
        {
            outBuffer[offset++] = 0;
        }
    }
    
    bufAllLen = *outRecord->offset = offset;

    //每读一条存到一个临时buffer,便于分帧
    toutBuffer.buffer = tmpBuf;
    toutBuffer.offset = &tOffset;
    toutBuffer.bufLen = DATA_LEN_MAX;

    //由于统计的特殊性，由于1条记录，所以只支持方法9，上第1条
    for(i = 0; i < outRecNumAll; i++)
    {
        ret = db_read_record_step(clientfd, stepHandle, &stepRead, &stepOut);
        if ((ret != ERR_OK) || (stepOut.recordnum == 0))
        {
            PRTL_BUF_DEBUG(stepRead.MAC.value, 6, "db_read_record_step failed. ret(%d), recordnum(%d), mainOAD(0x%08x)\n", 
                ret, stepOut.recordnum, readRecord.recordOAD.road.oadMain.value);
            continue;
        }

        //方法2：判断范围正确性
        if(recordInfo->oopRsd.choice == 2)
        {
            /* 判断是否是所需记录单元 */
            ret = record_data_is_valid(readRecord, stepOut);
            if (ret != ERR_OK)
            {
                continue;
            }  
        }

        //方法9：读取上第n次记录
        if((recordInfo->oopRsd.choice == 9) && (i+1 != recordInfo->oopRsd.sel9.nLast))
        {
            continue;
        }

        /* 
        A-RecordRow∷=SEQUENCE
        {
         第 1 列数据 Data，
         第 2 列数据 Data，
         …
         第 N 列数据 Data
        }*/        

        bufAllLenLast = bufAllLen; //存储上次的长度
        tOffset = 0;
        ret = event_unit2fmt698(recordInfo, &stepOut, &toutBuffer);
        if(ret != ERR_OK)
        {
            db_read_record_end(clientfd, stepHandle);          
            return ret;
        }
        
        bufAllLen = bufAllLenLast + tOffset;

        //超长，需要分帧，记下尚未处理的记录序号和尚未处理的时间，如果第一个就超长？
        if(bufAllLen <= outRecord->bufLen)
        {
            memcpy(&outBuffer[bufAllLenLast], toutBuffer.buffer, tOffset);
        }
        else
        {
            isBufFull = TRUE;
    
            if(recordInfo->append.bpInfo != NULL)
            {
                recordInfo->append.bpInfo->recNumSeg = outRecNum; //未处理的记录序号，同时表示已处理的记录数
                //方法4 5 6 7 8
                switch (readRecord.cType)
                {
                    case COL_TM_START:
                        recordInfo->append.bpInfo->tmStartSeg = stepOut.colStartTm;
                        break;
                    case COL_TM_END:
                        recordInfo->append.bpInfo->tmStartSeg = stepOut.colEndTm;
                        break;
                    case COL_TM_STORE:
                        recordInfo->append.bpInfo->tmStartSeg = stepOut.colStoreTm;
                        break;       
                    default:
                        return ERR_PTR;
                    break;
                }            
                recordInfo->append.bpInfo->appSlicingFlag = 1;
            }
            
            break;
        }
    
        outRecNum++;
    }
    
    /*结束读记录*/
    db_read_record_end(clientfd, stepHandle);
    
    if (!isBufFull)
    {
        *outRecord->offset = bufAllLen;
    
        if(recordInfo->append.bpInfo != NULL)
        {
            if (recordInfo->append.bpInfo->appSlicingFlag == 0x01)
            {
                recordInfo->append.bpInfo->appSlicingFlag = 0x01;
                recordInfo->append.bpInfo->isEnd = 0x01;                          //末尾帧
                recordInfo->append.bpInfo->seq++;   //序号加1
            }
            else
            {
                /*清除断点信息*/
                memset(recordInfo->append.bpInfo, 0, sizeof(BP_INFO_T));
            }
        }
    }
    else
    {
        *outRecord->offset = bufAllLenLast;
        
        if(recordInfo->append.bpInfo != NULL)
        {
            if(recordInfo->append.bpInfo->appSlicingFlag == 0x00)
            {
                recordInfo->append.bpInfo->seq = BRK_SEQ_START;                   //首次序号
            }
            else
            {
                recordInfo->append.bpInfo->seq++;   //序号加1
            }
            
            recordInfo->append.bpInfo->appSlicingFlag = 0x01;
            recordInfo->append.bpInfo->isEnd = 0x00;                              //不是末尾帧
        }
    }

    //长度域超过后也要采用A-XDR编码
    if (outRecNum > 0x80)
    {
        offlen = set_len_offset(outRecNum, lenBuf);
        memmove(&((uint8*)outRecord->buffer)[numOffset+offlen], &((uint8*)outRecord->buffer)[numOffset+1], *outRecord->offset - numOffset - 1);
        memcpy(&((uint8*)outRecord->buffer)[numOffset], lenBuf, offlen);
        *outRecord->offset += (offlen - 1); //减掉原来预留的1字节长度
    }
    else
    {
        //填写总记录数 
        ((uint8*)outRecord->buffer)[numOffset] = outRecNum;   
    }      

    return ERR_OK;
}

#endif

#if DESC("冻结记录读取函数",1)

/**
*********************************************************************
* @brief：      终端事件转换成698格式报文
* @param[in]： unit           - 单个数据记录单元
* @param[in]： oopRcsd        - 记录列选择描述符
* @param[out]：ppOutBuf       - 698格式报文
               outLen        - 报文长度
* @return：    =0             - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E frozen_unit2fmt698(RECORD_INFO_T *recordInfo, RECORD_UNIT_T *unit, BUFFER_INFO_T *outRecord)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    IC_OAD_INFO_T icInfo;
    DATA_CONVERT_T convert;
    uint32 sOffset = 0;
    uint32 dOffset = *outRecord->offset;
    uint32 i = 0 ,j = 0;
    
    MEMZERO(&convert, sizeof(DATA_CONVERT_T));
    
    convert.dstBuf = outRecord->buffer;
    convert.dstLen = outRecord->bufLen;
    convert.sOffset = &sOffset;   
    convert.dOffset = &dOffset;

    icInfo.choice = 0;

    //每次解析其中一个OAD
    for(i = 0; i < recordInfo->oopRcsd.nNum; i++)
    {
        sOffset = 0;
        
        for(j = 0; j < unit->recordnum; j++)
        {
            if(unit->record[j].road.oadCols.oad[0].value != recordInfo->oopRcsd.cols[i].oad.value)
            {
                continue;
            }

            icInfo.oad.value = unit->record[j].road.oadCols.oad[0].value;
            convert.srcBuf = unit->record[j].outDataBuf;
            convert.srcLen = unit->record[j].outDatalen;

            dar = normal_data_to_fmt698(&icInfo, &convert);
            if(dar != DATA_SUCCESS)
            {
                ACUDP_FMT_TRACE("[%d] normal_data_to_fmt698 failed. oad(0x%08x)\n", i, icInfo.oad.value);
                return dar;
            }

            break;
        }

        if(j == unit->recordnum)
        {
            ((uint8*)(outRecord->buffer))[dOffset++] = DT_NULL;
            ACUDP_FMT_TRACE("[%d] not find objs, data is null. oad(0x%08x)\n", i, icInfo.oad.value);
        }

        PRTL_BUF_TRACE(outRecord->buffer, dOffset, "[%d] oadCols(0x%08x)\n", i, recordInfo->oopRcsd.cols[i].oad.value);
    }

    *outRecord->offset = dOffset;

    return dar;
}


/**
*********************************************************************
* @brief：      获取读冻结记录参数
* @param[in]： recordInfo     - 转换前的参数
* @param[out]: readRecord    - 转换后的参数
* @return：    =0             - 成功
              <0             - 错误码
*********************************************************************
*/
int frozen_param_get(RECORD_INFO_T *recordInfo, READ_RECORD_T *readRecord, uint32 *stepNum)
{
    uint32 i = 0, j = 0;

    readRecord->MAC = MAC;
    readRecord->recordOAD.infoNum = recordInfo->append.infoNum;
    readRecord->recordOAD.logicId = 1;
    readRecord->recordOAD.road.oadMain.value = recordInfo->oopOad.value;
    readRecord->recordOAD.road.oadCols.nNum = recordInfo->oopRcsd.nNum;

    //默认读冻结记录序号和数据冻结时间
    for(i = 0; i < recordInfo->oopRcsd.nNum; i++)
    {
        if(recordInfo->oopRcsd.cols[i].oad.value == 0x20230200)
        {
            break;
        }
    }

    if(i == recordInfo->oopRcsd.nNum)
    {
        readRecord->recordOAD.road.oadCols.oad[j++].value = 0x20230200;
        readRecord->recordOAD.road.oadCols.nNum++;
    }

    for(i = 0; i < recordInfo->oopRcsd.nNum; i++)
    {
        if(recordInfo->oopRcsd.cols[i].oad.value == 0x20210200)
        {
            break;
        }
    }

    if(i == recordInfo->oopRcsd.nNum)
    {    
        readRecord->recordOAD.road.oadCols.oad[j++].value = 0x20210200;
        readRecord->recordOAD.road.oadCols.nNum++;
    }

    //关联OAD不存在记录型
    for(i = j; i <  readRecord->recordOAD.road.oadCols.nNum; i++)
    {
        readRecord->recordOAD.road.oadCols.oad[i].value = recordInfo->oopRcsd.cols[i-j].oad.value;
        //readRecord->recordOAD.road.oadCols.oad[i].nIndex = 0;
    }

    switch(recordInfo->oopRsd.choice)
    {
        case 0: //[0] NULL
        break;

        case 1:
        {
            if(recordInfo->oopRsd.sel1.oad.value == 0x20210200) //对于冻结来讲，筛选的OAD只支持冻结时间
            {

                if(recordInfo->oopRsd.sel1.value.type != DT_DATETIME_S)
                {
                    ACUDP_FMT_TRACE("not support. type = %d\n", recordInfo->oopRsd.sel1.value.type);
                    return ERR_TYPE;    
                }
                
                readRecord->cType = COL_TM_STORE;
                readRecord->sortType = DOWN_SORT;

                DT_DateTimeShort2Time(&recordInfo->oopRsd.sel1.value.tmVal, (time_t*)&readRecord->cStart);
                // 注：+8个小时(28800)转成格林时间
                readRecord->cStart += COMPENSATION_TIME;
                readRecord->cEnd = readRecord[0].cStart+1;
            }
            else
            {
                ACUDP_FMT_TRACE("not support. oad = %lu(0x%08x)\n", recordInfo->oopRsd.sel1.oad.value, recordInfo->oopRsd.sel1.oad.value);
                return ERR_O_RANGE;
            }

            *stepNum = 1;
        }break;

        case 2:
        {
            if(recordInfo->oopRsd.sel2.oad.value == 0x20210200) //对于冻结来讲，筛选的OAD只支持冻结时间
            {
                readRecord->cType = COL_TM_STORE;
                readRecord->sortType = DOWN_SORT;

                if(recordInfo->oopRsd.sel2.from.type != DT_DATETIME_S)
                {
                    ACUDP_FMT_TRACE("not support. type = %d\n", recordInfo->oopRsd.sel2.from.type);
                    return ERR_TYPE;    
                }

                DT_DateTimeShort2Time(&recordInfo->oopRsd.sel2.from.tmVal, (time_t*)&readRecord->cStart);
                // 注：+8个小时(28800)转成格林时间
                readRecord->cStart += COMPENSATION_TIME;

                if(recordInfo->oopRsd.sel2.to.type != DT_DATETIME_S)
                {
                    ACUDP_FMT_TRACE("not support. type = %d\n", recordInfo->oopRsd.sel2.to.type);
                    return ERR_TYPE;    
                }

                DT_DateTimeShort2Time(&recordInfo->oopRsd.sel2.to.tmVal, (time_t*)&readRecord->cEnd);
                // 注：+8个小时(28800)转成格林时间
                readRecord->cEnd += COMPENSATION_TIME;

                //RSD的范围是前闭后开区间，即[起始值，结束值）
                if(readRecord->cStart >= readRecord->cEnd)
                {
                    ACUDP_FMT_TRACE("not support. region[%d, %d)\n", readRecord->cStart, readRecord->cEnd);
                    return ERR_O_RANGE;
                }

                if((recordInfo->oopRsd.sel2.span.type != DT_TI)&&(recordInfo->oopRsd.sel2.span.type != DT_NULL))
                {
                    ACUDP_FMT_TRACE("not support. type = %d\n", recordInfo->oopRsd.sel2.span.type);
                    return ERR_TYPE;    
                }

                memcpy_s(&readRecord->ti, sizeof(OOP_TI_T), &recordInfo->oopRsd.sel2.span.ti, sizeof(OOP_TI_T));

                *stepNum = 1;
            }
            else
            {
                ACUDP_FMT_TRACE("not support. oad = %lu(0x%08x)\n", recordInfo->oopRsd.sel2.oad.value, recordInfo->oopRsd.sel2.oad.value);
                return ERR_O_RANGE;
            }
        }break;        

        case 3:
        {
 #if 0           
            for(i = 0; i < recordInfo->oopRsd.sel3.nNum; i++)
            {
                if(recordInfo->oopRsd.sel3.item[i].oad.value == 0x20210200) //对于冻结来讲，筛选的OAD只支持冻结时间
                {
                    readRecord[i].cType = COL_TM_STORE;
                    readRecord[i].sortType = DOWN_SORT;
            
                    if(recordInfo->oopRsd.sel3.item[i].from.type != DT_DATETIME_S)
                    {
                        ACUDP_FMT_TRACE("[%d] not support. type = %d\n", i, recordInfo->oopRsd.sel3.item[i].from.type);
                        return ERR_TYPE;    
                    }
            
                    DT_DateTimeShort2Time(&recordInfo->oopRsd.sel3.item[i].from.tmVal, &readRecord[i].cStart);
            
                    if(recordInfo->oopRsd.sel3.item[i].to.type != DT_DATETIME_S)
                    {
                        ACUDP_FMT_TRACE("[%d] not support. type = %d\n", i, recordInfo->oopRsd.sel3.item[i].to.type);
                        return ERR_TYPE;    
                    }
            
                    DT_DateTimeShort2Time(&recordInfo->oopRsd.sel3.item[i].to.tmVal, &readRecord[i].cEnd);  
            
                    if((recordInfo->oopRsd.sel2.span.type != DT_TI)&&(recordInfo->oopRsd.sel2.span.type != DT_NULL))
                    {
                        ACUDP_FMT_TRACE("not support. type = %d\n", recordInfo->oopRsd.sel3.item[i].span.type);
                        return ERR_TYPE;    
                    }
            
                    memcpy_s(&readRecord[i].ti, sizeof(OOP_TI_T), &recordInfo->oopRsd.sel3.item[i].span.ti, sizeof(OOP_TI_T));
                }
                else
                {
                    ACUDP_FMT_TRACE("[%d] not support. oad = %lu(0x%08x)\n", i, recordInfo->oopRsd.sel3.item[i].oad.value, recordInfo->oopRsd.sel3.item[i].oad.value);
                    return ERR_O_RANGE;
                }
            }
 #endif
        }break;  
 
        case 9:
        {
            readRecord->cType = COL_TM_STORE;
            readRecord->sortType = DOWN_SORT;
            readRecord->cStart = 0;
            readRecord->cEnd = time(NULL) + 1 + COMPENSATION_TIME;
        }break;
    }

    return ERR_OK;
}

/**
*********************************************************************
* @brief：      读取冻结记录
* @param[in]： clientfd       - 数据中心句柄
               recordInfo    - 读记录信息
* @param[out]：outRecord      - 输出记录信息
* @return：    =0             - 成功
              <0             - 错误码
*********************************************************************
*/
int record_frozen_read(DB_CLIENT clientfd, RECORD_INFO_T *recordInfo, BUFFER_INFO_T *outRecord)
{
    int ret = 0;
    STEP_READ_RECORD_T stepRead;
    RECORD_UNIT_T      stepOut;
    READ_RECORD_T readRecord;
    uint32 stepHandle = 0;  //单步读句柄，最多4轮  
    uint32 recNum     = 0;  //记录条数   
    uint32 outRecNumAll = 0;  //总的要输出的记录条数
    uint32 ctrlRecNum = 0xffffffff;
    uint32 stepNum = 0; //单步读轮数    
    uint32 offset     = *outRecord->offset;  //输出报文的偏移
    uint32 numOffset  = 0;  //写记录数的偏移    
    uint32 i          = 0;
    uint32 outRecNum  = 0; //实际输出的记录条数
    uint32             bufAllLen = 0;
    uint32             bufAllLenLast = 0;
    BOOLEAN               isBufFull = FALSE;
    uint8  offlen = 0;
    uint8  lenBuf[3] = {0};
    uint8 *outBuffer = (uint8*)outRecord->buffer;
    BUFFER_INFO_T toutBuffer;
    uint32 tOffset = 0;
    uint8 tmpBuf[DATA_LEN_MAX] = {0};

    /*入参检查*/
    if((recordInfo == NULL) || (outRecord == NULL))
    {
        ACUDP_FMT_TRACE("record_stat_read param check failed. recordInfo(%d), outBuffer(%d)\n", recordInfo, outRecord);
        return ERR_PTR;
    }

    MEMZERO(&readRecord, sizeof(readRecord));
    MEMZERO(&stepRead, sizeof(stepRead));
    MEMZERO(&stepOut, sizeof(stepOut));
    MEMZERO(&toutBuffer, sizeof(toutBuffer));

    //获取数据中心读取参数
    ret = frozen_param_get(recordInfo, &readRecord, &stepNum);
    if(ret == ERR_O_RANGE) //超出范围的OAD
    {
        ctrlRecNum = 0;
    }
    else if(ret != ERR_OK)
    {
        return DATA_TYPE_UNMATCHED;
    }

    ACUDP_FMT_TRACE("\r\nreadRecord.recordOAD.logicId = %d, readRecord.recordOAD.infoNum = %d, readRecord.recordOAD.road.oadMain = 0x%08x\
                    \r\nreadRecord.recordOAD.road.oadCols.nNum = %d \n",
                    readRecord.recordOAD.logicId, readRecord.recordOAD.infoNum, 
                    readRecord.recordOAD.road.oadMain, readRecord.recordOAD.road.oadCols.nNum);
    
    for(i = 0; i < readRecord.recordOAD.road.oadCols.nNum; i++)
    {
        ACUDP_FMT_TRACE("readRecord.recordOAD.road.oadCols.oad[%d] = %lu(0x%08x)\n", i, readRecord.recordOAD.road.oadCols.oad[i].value, readRecord.recordOAD.road.oadCols.oad[i].value);
    }

    //如果有断点，由于是倒序，则截止到上次中断的时间点，前闭后开，上次结束的时间的加1
    if((recordInfo->append.bpInfo != NULL)&&(recordInfo->append.bpInfo->appSlicingFlag == 1))
    {
        readRecord.cEnd = recordInfo->append.bpInfo->tmStartSeg + 1;    
    }

    ACUDP_FMT_TRACE("readRecord.ti.unit = %d, readRecord.ti.value = %d, readRecord.cType = %d, readRecord.tmStart = %d, readRecord.tmEnd = %d, readRecord.sortType = %d\n", 
        readRecord.ti.unit, readRecord.ti.value,
        readRecord.cType, readRecord.cStart, readRecord.cEnd, readRecord.sortType);

    /*开始读记录*/
    ret = db_read_record_start(clientfd, &readRecord, &stepHandle, &recNum);
    if (ret != ERR_OK)
    {
        ACUDP_FMT_TRACE("db_read_record_start failed. ret(%d), reNum(%d)\n", ret, recNum);
        return ERR_START_REC;
    }

    outRecNumAll = MIN(recNum, ctrlRecNum); 
    ACUDP_FMT_TRACE("outRecNumAll(%lu), MIN(recNum(%lu), ctrlRecNum(%lu)\n", outRecNumAll, recNum, ctrlRecNum);

    //MAC校验
    memcpy(&stepRead.MAC, &readRecord.MAC, sizeof(OOP_OCTETVAR64_T));

    for(i = 0; i < 6; i++)
    {
        ACUDP_FMT_TRACE("stepRead.MAC[%d] = %02x\n", i, stepRead.MAC.value[i]);
    }

    numOffset = offset;  //记录数或者数组成员数的位置
    
    /*M 条记录 [1] SEQUENCE OF A-RecordRow*/
    outBuffer[offset++] = outRecNumAll;

    // 如果outRecNumAll为0，且结果具有唯一性，每个CSD对应填NULL
    if ((outRecNumAll == 0) && (is_result_only(recordInfo->oopRsd)))
    {
        ACUDP_FMT_TRACE("outRecNumAll(%lu), and the result is only\n", outRecNumAll);
        outRecNum = 1;
        for (i = 0; i < recordInfo->oopRcsd.nNum; i ++)
        {
            outBuffer[offset++] = 0;
        }
    }
    
    bufAllLen = *outRecord->offset = offset;

    //每读一条存到一个临时buffer,便于分帧
    toutBuffer.buffer = tmpBuf;
    toutBuffer.offset = &tOffset;
    toutBuffer.bufLen = DATA_LEN_MAX;

    //由于统计的特殊性，由于1条记录，所以只支持方法9，上第1条
    for(i = 0; i < outRecNumAll; i++)
    {
        ret = db_read_record_step(clientfd, stepHandle, &stepRead, &stepOut);
        if ((ret != ERR_OK) || (stepOut.recordnum == 0))
        {
            PRTL_BUF_DEBUG(stepRead.MAC.value, 6, "db_read_record_step failed. ret(%d), recordnum(%d), mainOAD(0x%08x)\n", 
                ret, stepOut.recordnum, readRecord.recordOAD.road.oadMain.value);
            continue;
        }

        //方法2：判断范围正确性
        if(recordInfo->oopRsd.choice == 2)
        {
            /* 判断是否是所需记录单元 */
            ret = record_data_is_valid(readRecord, stepOut);
            if (ret != ERR_OK)
            {
                continue;
            }  
        }

        //方法9：读取上第n次记录
        if((recordInfo->oopRsd.choice == 9) && (i+1 != recordInfo->oopRsd.sel9.nLast))
        {
            continue;
        }

        /* 
        A-RecordRow∷=SEQUENCE
        {
         第 1 列数据 Data，
         第 2 列数据 Data，
         …
         第 N 列数据 Data
        }*/        

        bufAllLenLast = bufAllLen; //存储上次的长度
        tOffset = 0;
        ret = frozen_unit2fmt698(recordInfo, &stepOut, &toutBuffer);
        if(ret != ERR_OK)
        {
            db_read_record_end(clientfd, stepHandle);          
            return ret;
        }
        
        bufAllLen = bufAllLenLast + tOffset;

        //超长，需要分帧，记下尚未处理的记录序号和尚未处理的时间，如果第一个就超长？
        if(bufAllLen <= outRecord->bufLen)
        {
            memcpy(&outBuffer[bufAllLenLast], toutBuffer.buffer, tOffset);
        }
        else
        {
            isBufFull = TRUE;

            if(recordInfo->append.bpInfo != NULL)
            {
                recordInfo->append.bpInfo->recNumSeg = outRecNum; //未处理的记录序号，同时表示已处理的记录数
                //方法4 5 6 7 8
                switch (readRecord.cType)
                {
                    case COL_TM_START:
                        recordInfo->append.bpInfo->tmStartSeg = stepOut.colStartTm;
                        break;
                    case COL_TM_END:
                        recordInfo->append.bpInfo->tmStartSeg = stepOut.colEndTm;
                        break;
                    case COL_TM_STORE:
                        recordInfo->append.bpInfo->tmStartSeg = stepOut.colStoreTm;
                        break;       
                    default:
                        return ERR_PTR;
                    break;
                }            
                recordInfo->append.bpInfo->appSlicingFlag = 1;
            }
            
            break;
        }

        outRecNum++;
    }

    /*结束读记录*/
    db_read_record_end(clientfd, stepHandle);

    if (!isBufFull)
    {
        *outRecord->offset = bufAllLen;

        if(recordInfo->append.bpInfo != NULL)
        {
            if (recordInfo->append.bpInfo->appSlicingFlag == 0x01)
            {
                recordInfo->append.bpInfo->appSlicingFlag = 0x01;
                recordInfo->append.bpInfo->isEnd = 0x01;                          //末尾帧
                recordInfo->append.bpInfo->seq++;   //序号加1
            }
            else
            {
                /*清除断点信息*/
                memset(recordInfo->append.bpInfo, 0, sizeof(BP_INFO_T));
            }
        }
    }
    else
    {
        *outRecord->offset = bufAllLenLast;
        
        if(recordInfo->append.bpInfo != NULL)
        {
            if(recordInfo->append.bpInfo->appSlicingFlag == 0x00)
            {
                recordInfo->append.bpInfo->seq = BRK_SEQ_START;                   //首次序号
            }
            else
            {
                recordInfo->append.bpInfo->seq++;   //序号加1
            }
            
            recordInfo->append.bpInfo->appSlicingFlag = 0x01;
            recordInfo->append.bpInfo->isEnd = 0x00;                              //不是末尾帧
        }
    }

    //长度域超过后也要采用A-XDR编码
    if (outRecNum > 0x80)
    {
        offlen = set_len_offset(outRecNum, lenBuf);
        memmove(&((uint8*)outRecord->buffer)[numOffset+offlen], &((uint8*)outRecord->buffer)[numOffset+1], *outRecord->offset - numOffset - 1);
        memcpy(&((uint8*)outRecord->buffer)[numOffset], lenBuf, offlen);
        *outRecord->offset += (offlen - 1); //减掉原来预留的1字节长度
    }
    else
    {
        //填写总记录数 
        ((uint8*)outRecord->buffer)[numOffset] = outRecNum;   
    }      

    return ERR_OK;
}

#endif


