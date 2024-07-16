/*
*********************************************************************
* @file    class11.c
* @brief： class11处理
* @author：
* @date：   2019.12.06
* @copyright 2019 南京新联电子股份有限公司 All rights reserved.
*********************************************************************
*/
#include "hal.h"

#include "baseCommon.h"
#include "oadInfoList.h"
#include "oopservice.h"
#include "commClass.h"
#include "oopStorage.h"
#include "msg_id.h"
#include "area.h"
#include "AutoTask.h"
#include "devinfo.h"

#if DESC("组群档案相关读取", 1)
/**
*********************************************************************
* @brief：      销毁链表
* @param[in]： 集合链表头
* @return：     集合链表头
*********************************************************************
*/
LINK_GROUPMETER_ALL_T* link_group_destroy(LINK_GROUPMETER_ALL_T* pGroupHead)
{
    LINK_GROUPMETER_ALL_T *tempGroup = NULL;
    LINK_GROUPMETER_ALL_T *pGroup = pGroupHead;
    while (pGroup)
    {
        tempGroup = pGroup->next;
        free(pGroup);
        pGroup = tempGroup;
    }
    pGroupHead = NULL;
    return pGroupHead;
}

/**
*********************************************************************
* @name：       get_meter_link_stat
* @brief：      获取任务方案
* @param[in] ：hUdp        数据中心句柄
* @param[out]: none
* @return    ：配置信息链表
* @author    :
* @Date      ：2019-12-6
*********************************************************************
*/
LINK_GROUPMETER_ALL_T *get_meter_link_stat(DB_CLIENT hUdp)
{
    int                 i = 0;
    int                 j = 0;
    int                 ret = ERR_OK;
    uint32              handle = 0;
    uint32              recNum = 0;
    uint16              oopGroupNum = 0;
    
    //链表相关变量
    LINK_GROUPMETER_ALL_T* pMtrHead      = NULL;
    LINK_GROUPMETER_ALL_T* pMtrTail      = pMtrHead;
    LINK_GROUPMETER_ALL_T* pNewMtr       = pMtrHead;

    OOP_GROUPMETER_NEW_T  oopGroup;
    OOP_GROUPMETER_NEW_T  oopGroup2;
    OOP_GROUPMETER_ALL_T  oopGroupall;
    READ_RECORD_T         inReadRecord;
    STEP_READ_RECORD_T    stepReadRecord;
    RECORD_UNIT_T         stepRecordOut;

    NOMAL_OAD_T normal = {0};
    uint32      len  = 0;

    memset(&stepReadRecord, 0, sizeof(stepReadRecord));
    memset(&stepRecordOut, 0, sizeof(stepRecordOut));
    memset(&inReadRecord, 0, sizeof(inReadRecord));
    memset(&oopGroup, 0, sizeof(oopGroup));
    memset(&oopGroup2, 0, sizeof(oopGroup2));
    memset(&oopGroupall, 0, sizeof(oopGroupall));

    //获取所有任务方案
    inReadRecord.recordOAD.infoNum = 0xffff;
    inReadRecord.cType = COL_NO_INFO;
    inReadRecord.cStart = 0;
    inReadRecord.cEnd = 0xffff;
    inReadRecord.sortType = UP_SORT;
    inReadRecord.recordOAD.optional = 0;
    inReadRecord.recordOAD.road.oadMain.value = 0;
    inReadRecord.recordOAD.road.oadCols.nNum = 0x01;
    inReadRecord.recordOAD.road.oadCols.oad[0].value = 0x6e050200;

    ret = db_read_record_start(hUdp, &inReadRecord, &handle, &recNum);
    if ((ret != ERR_OK) || (recNum == 0))
    {
        PRTL_FMT_DEBUG("群组档案开始读失败 ret = %d, infoNum = %d recNum = %d\n",
            ret, inReadRecord.recordOAD.infoNum, recNum);

        db_read_record_end(hUdp, handle);
        return NULL;
    }

    for (i = 0; i < recNum; i++)
    {
        ret = db_read_record_step(hUdp, handle, &stepReadRecord, &stepRecordOut);
        if (ret != ERR_OK)
        {
            PRTL_FMT_DEBUG("群组档案单步读成功 db_read_record_step err = %d!\n", ret);
            continue;
        }

        if ((stepRecordOut.recordnum != 1)
            || (stepRecordOut.record[0].road.oadCols.oad[0].value != 0x6e050200)
            || (stepRecordOut.record[0].outDatalen != sizeof(OOP_GROUPMETER_NEW_T)))
        {
            PRTL_FMT_DEBUG("读群组档案失败!\n");
            PRTL_FMT_DEBUG("读群组档案 stepRecordOut.recordnum = %d\n", stepRecordOut.recordnum);
            PRTL_FMT_DEBUG("读群组档案 stepRecordOut.record[0].road.oadCols.oad[0] = 0x%08x\n",
                stepRecordOut.record[0].road.oadCols.oad[0].value);
            PRTL_FMT_DEBUG("读群组档案 stepRecordOut.record[0].outDatalen = %d\n",
                stepRecordOut.record[0].outDatalen);
            PRTL_FMT_DEBUG("读群组档案 sizeof(OOP_GROUPMETER_NEW_T) = %d\n",
                sizeof(OOP_GROUPMETER_NEW_T));
            continue;
        }

        memcpy(&oopGroup, stepRecordOut.record[0].outDataBuf, stepRecordOut.record[0].outDatalen);
        PRTL_FMT_DEBUG("读到群组档案[%d]\n", stepRecordOut.infoNum);

        normal.oad.value = 0x6e05ffff;
        normal.infoNum = stepRecordOut.infoNum;
        ret = db_read_nomal(hUdp, &normal, sizeof(oopGroup), (uint8*)&oopGroup2, &len);
        if((ret != ERR_OK) || (len == 0))
        {
            PRTL_FMT_DEBUG("读普通数据失败. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                ret, normal.logicId, normal.infoNum, normal.oad.value);
        }

        //2个结构体合并
        oopGroupall.choice = oopGroup.choice;
        oopGroupall.groupAddr = oopGroup.groupAddr;
        for (j = 0; j < oopGroup.num; j++)
        {
            memcpy(&oopGroupall.addr[j].add[0], &oopGroup.addr[j].add[0], 6);
        }
        if (oopGroup2.num > 0)
        {
            for (j = 0; j < oopGroup2.num; j++)
            {
                memcpy(&oopGroupall.addr[oopGroup.num + j].add[0], &oopGroup2.addr[j].add[0], 6);
            }
        }
        oopGroupall.num = oopGroup.num + oopGroup2.num;

        pNewMtr = (LINK_GROUPMETER_ALL_T *)malloc(sizeof(LINK_GROUPMETER_ALL_T));
        memset(pNewMtr, 0, sizeof(LINK_GROUPMETER_ALL_T));
        pNewMtr->oopGroup = oopGroupall;
        pNewMtr->no   = oopGroupNum ++;
        pNewMtr->next = NULL;
        if(!pMtrHead)
            pMtrHead = pNewMtr;
        if(pMtrTail)
            pMtrTail->next = pNewMtr;
        pMtrTail = pNewMtr;
    }

    db_read_record_end(hUdp, handle);
    
    return pMtrHead;
}

/**
*********************************************************************
* @brief：     判断表地址是否合法
* @param[in]： addr     地址
* @return：    TRUE - 合法
*********************************************************************
*/
BOOL meteraddr_is_legally(uint64 *addr)
{
    uint8 i = 0;
    uint8 add[OOP_TSA_SIZE] = { 0 };

    memcpy(add, addr, sizeof(uint64));

    for (i = 0; i < sizeof(uint64); i++)
    {
        if (add[i] > 0x99)
        {
            return FALSE;
        }
    }
    return TRUE;
}

/**
*********************************************************************
* @name：      ms_from_groupmeter_new
* @brief：     
* @param[in] ：srcdata    组群档案
               dstdata   转换成ms集合形式的群组档案
* @param[out]：
* @return    ：TRUE - 有相同的档案 FALSE -没有相同档案
* @author    :
* @Date      ：
*********************************************************************
*/
int groupmeter_new_to_old(OOP_GROUP_METER_T *dstdata, OOP_GROUPMETER_NEW_T *srcdata1, OOP_GROUPMETER_NEW_T *srcdata2)
{
    uint32 i = 0;
    
    if ((dstdata == NULL) || (srcdata1 == NULL) || (srcdata2 == NULL))
    {
        return ERR_PTR;
    }

    if (srcdata1->choice != 3 && srcdata1->choice != 6)
    {
        PRTL_FMT_DEBUG("档案转换出错 choice1[%d] choice2[%d]\n", srcdata1->choice, srcdata1->choice);
        return ERR_TYPE;
    }

    if (srcdata1->num == 0)
    {
        PRTL_FMT_DEBUG("档案转换出错 srcdata1->num[%d]\n", srcdata1->num);
        return ERR_TYPE;
    }

    dstdata->groupAdd = srcdata1->groupAddr;
    dstdata->ms.choice = srcdata1->choice;
    switch (srcdata1->choice)
    {
    case 3:
    {
        dstdata->ms.msa.size = srcdata1->num;
        for (i = 0; i < srcdata1->num; i++)
        {
            dstdata->ms.msa.add[i].len = 5;
            memcpy(&dstdata->ms.msa.add[i].add[0], &srcdata1->addr[i].add[0], 6);
        }

        if (srcdata2->num > 0)
        {
            dstdata->ms.msa.size = srcdata1->num + srcdata2->num;
            for (i = 0; i < srcdata2->num; i++)
            {
                dstdata->ms.msa.add[srcdata1->num + i].len = 5;
                memcpy(&dstdata->ms.msa.add[srcdata1->num + i].add[0], &srcdata2->addr[i].add[0], 6);
            }
        }
        break;
    }
    case 6:
    {
        PRTL_FMT_DEBUG("暂时只考虑地址类似3 srcdata->choice[%d]\n", srcdata1->choice);
        break;
    }
    default:
        break;
    }

    return ERR_OK;
}

/**
*********************************************************************
* @brief：      读记录型方案任务
* @param[in]： inReadRecord  - 筛选需要的数据
               oopRcsd       - 列选择
               readRecNum    - 读记录数量，：0：读全部记录 1：读最近的一条记录
               maxBufLen     - 最大读取buf长度
* @param[out]：outRecordBuf  -  数据记录
* @return：     =0            - 成功
               <0            - 错误码
*********************************************************************
*/
int record_group_meter_read_by_tsa(DB_CLIENT hUdp, const READ_INF_T inReadNor, RELY_INFO_T relyInfo, uint32 maxBufLen, RECORD_BUF_T *outRecordBuf)
{
    int       result = DATA_OTHER_REASON;
    uint32    offset = 0;
    uint32    i = 0;
    int32     ret = ERR_NORMAL;
    OOP_DAR_E nRet = DATA_OTHER_REASON;
    uint32    bufAllLen = 0;
    BP_INFO_T breakInfo = {0};                                             //断点信息
    BOOL      isBufFull = FALSE;
    OAD_INFO_T *pOadInfo = relyInfo.pOadInfo;

    /*清除断点信息, 需要注意应用分帧的未帧*/
    memset(&breakInfo, 0, sizeof(BP_INFO_T));
   
    DATA_CONVERT_T   convert;
    OOP_GROUP_METER_T  dstdata;
    OOP_GROUPMETER_NEW_T  data;
    OOP_GROUPMETER_NEW_T  data2;

    uint32 sOffset = 0;
    uint32 dOffset = 0;
    uint32 oopGroupNum = 0;
    uint32 oopGroupOffset = 0;

    READ_RECORD_T      inReadRecord;
    STEP_READ_RECORD_T stepReadRecord;
    RECORD_UNIT_T      stepRecordOut;
    uint32             handle = 0;
    uint32             recNum = 0;

    NOMAL_OAD_T normal = {0};
    uint32      len  = 0;

    MEMZERO(&convert, sizeof(convert));
    MEMZERO(&inReadRecord, sizeof(inReadRecord));

    oopGroupOffset = offset;                                         //档案数量偏移
    outRecordBuf->recordBuf[offset ++] = 0x00;                          //数量，先填0
    bufAllLen = offset;

    inReadRecord.recordOAD.logicId = 0;
    inReadRecord.recordOAD.infoNum = 0xffff;
    inReadRecord.cType = COL_NO_INFO;
    inReadRecord.cStart = inReadNor.startTask;
    inReadRecord.cEnd = inReadNor.endTask;
    inReadRecord.sortType = UP_SORT;
    inReadRecord.recordOAD.optional = 0;
    inReadRecord.recordOAD.road.oadMain.value = 0;
    inReadRecord.recordOAD.road.oadCols.nNum = 0x01;
    inReadRecord.recordOAD.road.oadCols.oad[0].value = pOadInfo->pTab->oad.value;

    stepReadRecord.MAC.nNum = 0;

    if(relyInfo.bpBreakInfo->appSlicingFlag == 1)
    {
        inReadRecord.cStart = relyInfo.bpBreakInfo->index;
    }

    ret = db_read_record_start(hUdp, &inReadRecord, &handle, &recNum);
    if ((ret != ERR_OK) || (recNum == 0))
    {
        PRTL_FMT_DEBUG("开始读出错. 错误码[%d], infoNum[%d] recNum[%d] oad[0x%08x]\n", 
            ret, inReadRecord.recordOAD.infoNum, recNum, pOadInfo->pTab->oad.value);

        db_read_record_end(hUdp, handle);
        return ret;
    }
    else
    {
        PRTL_FMT_DEBUG("开始读成功. 返回码[%d], infoNum[%d] recNum[%d] oad[0x%08x]\n", 
            ret, inReadRecord.recordOAD.infoNum, recNum, pOadInfo->pTab->oad.value);
    }

    for (i = 0; i < recNum; i++)
    {
        ret = db_read_record_step(hUdp, handle, &stepReadRecord, &stepRecordOut);
        if(ret != ERR_OK)
        {
            PRTL_FMT_DEBUG("单步读出错 错误码[%d]\n", ret);
            continue;
        }

        //OAD与返回数据
        if ((stepRecordOut.recordnum != 1) 
        || (stepRecordOut.record[0].road.oadCols.oad[0].value != pOadInfo->pTab->oad.value)
        || (stepRecordOut.record[0].outDatalen != sizeof(data)))
        {
            PRTL_FMT_DEBUG("单步读到的数据错误!\n");
            PRTL_FMT_DEBUG("单步读数据 stepRecordOut.recordnum = %d\n", stepRecordOut.recordnum);
            PRTL_FMT_DEBUG("单步读数据 stepRecordOut.record[0].road.oadCols.oad[0] = 0x%08x\n", 
                stepRecordOut.record[0].road.oadCols.oad[0].value);
            PRTL_FMT_DEBUG("单步读数据 stepRecordOut.record[0].outDatalen = %d\n", 
                stepRecordOut.record[0].outDatalen);
            PRTL_FMT_DEBUG("单步读数据 sizeof(OOP_GROUPMETER_NEW_T) = %d\n", 
                sizeof(data));
            continue;
        }

        memcpy(&data, stepRecordOut.record[0].outDataBuf, stepRecordOut.record[0].outDatalen);
        PRTL_FMT_DEBUG("stepRecordOut.infoNum = %d\n", stepRecordOut.infoNum);

        normal.oad.value = 0x6e05ffff;
        normal.infoNum = stepRecordOut.infoNum;
        ret = db_read_nomal(hUdp, &normal, sizeof(data2), (uint8*)&data2, &len);
        if((ret != ERR_OK) || (len == 0))
        {
            PRTL_FMT_DEBUG("读普通数据失败. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                ret, normal.logicId, normal.infoNum, normal.oad.value);
        }

        ret = groupmeter_new_to_old(&dstdata, &data, &data2);
        if (ret != ERR_OK)
        {
            PRTL_FMT_DEBUG("群组档案数据转换失败 ret = %d\n", ret);
            continue;
        }

        if (inReadNor.tsa.len != 0)
        {
            if (memcmp(&inReadNor.tsa, &dstdata.groupAdd, sizeof(OOP_TSA_T)) != 0)
            {
                continue;
            }
        }

        //转换成报文数据
        convert.srcBuf = &dstdata;
        convert.srcLen = sizeof(dstdata);
        convert.sOffset = &sOffset;

        convert.dstBuf = &outRecordBuf->recordBuf[offset];
        convert.dstLen = maxBufLen - offset;
        convert.dOffset = &dOffset;

        //记录断点信息
        breakInfo.index = stepRecordOut.infoNum;

        nRet = data_to_buf_group_meter(pOadInfo, &convert);  //入参
        if (nRet == DATA_SUCCESS)
        {
            bufAllLen += dOffset;
            if (bufAllLen <= maxBufLen)
            {
                memcpy(&outRecordBuf->recordBuf[offset], convert.dstBuf, dOffset);
                offset += dOffset;
                outRecordBuf->bufLen = offset;
                PRTL_BUF_LOGCC(convert.dstBuf, dOffset, "第%d个群组档案报文:", i);
                oopGroupNum ++;
                if (bufAllLen + dOffset >= maxBufLen) //虽然本次没有超最大长度，但下次会
                {
                    isBufFull = TRUE;
                    PRTL_FMT_DEBUG("虽然本次没有超最大长度，但下次会 isBufFull = [%d]\n", isBufFull);
                }
                dOffset = 0;
                sOffset = 0;
            }
            else
            {
                dOffset = 0;
                sOffset = 0;
                isBufFull = TRUE;
                PRTL_FMT_DEBUG("达到最大报文长度 isBufFull = [%d]\n", isBufFull);
            }
        }
        else
        {
            continue;
        }

        if ((isBufFull) && (offset > 0))
        {
            break;
        }
    }

    db_read_record_end(hUdp, handle);

    if (!isBufFull)
    {
        if (relyInfo.bpBreakInfo->appSlicingFlag == 0x01)
        {
            breakInfo.appSlicingFlag = 0x01;
            breakInfo.isEnd = 0x01;                           //末尾帧
            breakInfo.seq = relyInfo.bpBreakInfo->seq + 1;    //序号加1
            breakInfo.objPre = 0x01;                          //对象属性
        }
        else
        {
            /*清除断点信息*/
            memset(&breakInfo, 0, sizeof(BP_INFO_T));
        }
    }
    else
    {
        breakInfo.reqType = 0x03;
        breakInfo.appSlicingFlag = 0x01;
        breakInfo.isEnd = 0x00;                              //不是末尾帧
        breakInfo.objPre = 0x01;                             //对象属性
        if (relyInfo.bpBreakInfo->appSlicingFlag == 0x00)
        {
            breakInfo.seq = BRK_SEQ_START;                   //首次序号
        }
        else
        {
            breakInfo.seq = relyInfo.bpBreakInfo->seq + 1;  //序号加1
        }
    }

    //刷新断点信息
    memcpy(&outRecordBuf->breakInfo, &breakInfo, sizeof(BP_INFO_T));

    PRTL_BUF_LOGCC(outRecordBuf->recordBuf, offset, "整个报文:");
    PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->appSlicingFlag = %d\n", outRecordBuf->breakInfo.appSlicingFlag);
    PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->index = %d\n", outRecordBuf->breakInfo.index);
    PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->isEnd = %d\n", outRecordBuf->breakInfo.isEnd);
    PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->objPre = %d\n", outRecordBuf->breakInfo.objPre);
    PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->seq = %d\n", outRecordBuf->breakInfo.seq);
    PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->reqType = %d\n", outRecordBuf->breakInfo.reqType);

    result = DATA_SUCCESS;
    outRecordBuf->recordBuf[oopGroupOffset] = oopGroupNum;   //重新记档案数量
    outRecordBuf->bufLen = offset;

    return result;
}

/**
*********************************************************************
* @brief：      读记录型方案任务
* @param[in]： inReadRecord  - 筛选需要的数据
               oopRcsd       - 列选择
               readRecNum    - 读记录数量，：0：读全部记录 1：读最近的一条记录
               maxBufLen     - 最大读取buf长度
* @param[out]：outRecordBuf  -  数据记录
* @return：     =0            - 成功
               <0            - 错误码
*********************************************************************
*/
int record_group_meter_read_by_port(DB_CLIENT hUdp, const READ_INF_T inReadNor, RELY_INFO_T relyInfo, uint32 maxBufLen, RECORD_BUF_T *outRecordBuf)
{
    int       result = DATA_OTHER_REASON;
    uint32    offset = 0;
    uint32    i = 0;
    int32     ret = ERR_NORMAL;
    OOP_DAR_E nRet = DATA_OTHER_REASON;
    uint32    bufAllLen = 0;
    BP_INFO_T breakInfo = {0};                                         //断点信息
    BOOL      isBufFull = FALSE;
    OAD_INFO_T *pOadInfo = relyInfo.pOadInfo;

    /*清除断点信息, 需要注意应用分帧的未帧*/
    memset(&breakInfo, 0, sizeof(BP_INFO_T));
   
    DATA_CONVERT_T   convert;
    OOP_GROUP_METER_T  dstdata;
    OOP_GROUPMETER_NEW_T  data;
    OOP_GROUPMETER_NEW_T  data2;

    uint32 sOffset = 0;
    uint32 dOffset = 0;
    uint32 oopGroupNum = 0;
    uint32 oopGroupNumOffset = 0;

    READ_RECORD_T      inReadRecord;
    STEP_READ_RECORD_T stepReadRecord;
    RECORD_UNIT_T      stepRecordOut;
    uint32             handle = 0;
    uint32             recNum = 0;

    NOMAL_OAD_T normal = {0};
    uint32      len  = 0;

    MEMZERO(&convert, sizeof(convert));
    MEMZERO(&inReadRecord, sizeof(inReadRecord));

    oopGroupNumOffset = offset;                                         //数量偏移
    outRecordBuf->recordBuf[offset ++] = 0x00;                          //数量，先填0
    bufAllLen = offset;

    inReadRecord.recordOAD.logicId = 0;
    inReadRecord.recordOAD.infoNum = 0xffff;
    inReadRecord.cType = COL_NO_INFO;
    inReadRecord.cStart = 0;
    inReadRecord.cEnd = 0xffff;
    inReadRecord.sortType = UP_SORT;
    inReadRecord.recordOAD.optional = 0;
    inReadRecord.recordOAD.road.oadMain.value = 0;
    inReadRecord.recordOAD.road.oadCols.nNum = 0x01;
    inReadRecord.recordOAD.road.oadCols.oad[0].value = pOadInfo->pTab->oad.value;

    stepReadRecord.MAC.nNum = 0;

    if(relyInfo.bpBreakInfo->appSlicingFlag == 1)
    {
        inReadRecord.cStart = relyInfo.bpBreakInfo->index;
    }

    ret = db_read_record_start(hUdp, &inReadRecord, &handle, &recNum);
    if ((ret != ERR_OK) || (recNum == 0))
    {
        PRTL_FMT_DEBUG("开始读出错 错误码[%d], infoNum[%d] recNum[%d] oad[0x%08x]\n", 
            ret, inReadRecord.recordOAD.infoNum, recNum, pOadInfo->pTab->oad.value);

        db_read_record_end(hUdp, handle);
        return ret;
    }
    else
    {
        PRTL_FMT_DEBUG("db_read_record_start err! ret = %d, infoNum = %d recNum = %d\n", 
            ret, inReadRecord.recordOAD.infoNum, recNum);
    }

    for (i = 0; i < recNum; i++)
    {
        ret = db_read_record_step(hUdp, handle, &stepReadRecord, &stepRecordOut);
        if(ret != ERR_OK)
        {
            PRTL_FMT_DEBUG("db_read_record_step err = %d!\n", ret);
            continue;
        }

        //OAD与返回数据
        if ((stepRecordOut.recordnum != 1) 
        || (stepRecordOut.record[0].road.oadCols.oad[0].value != pOadInfo->pTab->oad.value)
        || (stepRecordOut.record[0].outDatalen != sizeof(data)))
        {
            PRTL_FMT_DEBUG("单步读到的数据错误!\n");
            PRTL_FMT_DEBUG("单步读数据 stepRecordOut.recordnum = %d\n", stepRecordOut.recordnum);
            PRTL_FMT_DEBUG("单步读数据 stepRecordOut.record[0].road.oadCols.oad[0] = 0x%08x\n", 
                stepRecordOut.record[0].road.oadCols.oad[0].value);
            PRTL_FMT_DEBUG("单步读数据 stepRecordOut.record[0].outDatalen = %d\n", 
                stepRecordOut.record[0].outDatalen);
            PRTL_FMT_DEBUG("单步读数据 sizeof(OOP_GROUPMETER_NEW_T) = %d\n", 
                sizeof(data));
            continue;
        }

        memcpy(&data, stepRecordOut.record[0].outDataBuf, stepRecordOut.record[0].outDatalen);

        //.....筛选方式，暂时省略
        
        PRTL_FMT_DEBUG("stepRecordOut.infoNum = %d\n", stepRecordOut.infoNum);
        normal.oad.value = 0x6e05ffff;
        normal.infoNum = stepRecordOut.infoNum;
        ret = db_read_nomal(hUdp, &normal, sizeof(data2), (uint8*)&data2, &len);
        if((ret != ERR_OK) || (len == 0))
        {
            PRTL_FMT_DEBUG("读普通数据失败. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                ret, normal.logicId, normal.infoNum, normal.oad.value);
        }

        ret = groupmeter_new_to_old(&dstdata, &data, &data2);
        if (ret != ERR_OK)
        {
            PRTL_FMT_DEBUG("群组档案数据转换失败 ret = %d\n", ret);
            continue;
        }

        //转换成报文数据
        convert.srcBuf = &dstdata;
        convert.srcLen = sizeof(dstdata);
        convert.sOffset = &sOffset;

        convert.dstBuf = &outRecordBuf->recordBuf[offset];
        convert.dstLen = maxBufLen - offset;
        convert.dOffset = &dOffset;

        //记录断点信息
        breakInfo.tmStartSeg = stepRecordOut.infoNum;

        nRet = data_to_buf_group_meter(pOadInfo, &convert);  //入参
        if (nRet == DATA_SUCCESS)
        {
            bufAllLen += dOffset;
            if (bufAllLen <= maxBufLen)
            {
                memcpy(&outRecordBuf->recordBuf[offset], convert.dstBuf, dOffset);
                offset += dOffset;
                outRecordBuf->bufLen = offset;
                PRTL_BUF_LOGCC(convert.dstBuf, dOffset, "第%d个群组档案报文:", i);
                oopGroupNum ++;
                if (bufAllLen + dOffset >= maxBufLen) //虽然本次没有超最大长度，但下次会
                {
                    isBufFull = TRUE;
                    PRTL_FMT_DEBUG("虽然本次没有超最大长度，但下次会 isBufFull = [%d]\n", isBufFull);
                }
                dOffset = 0;
                sOffset = 0;
            }
            else
            {
                dOffset = 0;
                sOffset = 0;
                isBufFull = TRUE;
                PRTL_FMT_DEBUG("达到最大报文长度 isBufFull = [%d]\n", isBufFull);
            }
        }
        else
        {
            continue;
        }

        if ((isBufFull) && (offset > 0))
        {
            break;
        }
    }

    db_read_record_end(hUdp, handle);

    if (!isBufFull)
    {
        if (relyInfo.bpBreakInfo->appSlicingFlag == 0x01)
        {
            breakInfo.appSlicingFlag = 0x01;
            breakInfo.isEnd = 0x01;                           //末尾帧
            breakInfo.seq = relyInfo.bpBreakInfo->seq + 1;    //序号加1
            breakInfo.objPre = 0x01;                          //对象属性
        }
        else
        {
            /*清除断点信息*/
            memset(&breakInfo, 0, sizeof(BP_INFO_T));
        }
    }
    else
    {
        breakInfo.reqType = 0x03;
        breakInfo.appSlicingFlag = 0x01;
        breakInfo.isEnd = 0x00;                              //不是末尾帧
        breakInfo.objPre = 0x01;                             //对象属性
        if (relyInfo.bpBreakInfo->appSlicingFlag == 0x00)
        {
            breakInfo.seq = BRK_SEQ_START;                   //首次序号
        }
        else
        {
            breakInfo.seq = relyInfo.bpBreakInfo->seq + 1;  //序号加1
        }
    }

    //刷新断点信息
    memcpy(&outRecordBuf->breakInfo, &breakInfo, sizeof(BP_INFO_T));

    PRTL_BUF_LOGCC(outRecordBuf->recordBuf, offset, "整个报文:");
    PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->appSlicingFlag = %d\n", outRecordBuf->breakInfo.appSlicingFlag);
    PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->index = %d\n", outRecordBuf->breakInfo.index);
    PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->isEnd = %d\n", outRecordBuf->breakInfo.isEnd);
    PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->objPre = %d\n", outRecordBuf->breakInfo.objPre);
    PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->seq = %d\n", outRecordBuf->breakInfo.seq);
    PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->reqType = %d\n", outRecordBuf->breakInfo.reqType);

    result = DATA_SUCCESS;
    outRecordBuf->recordBuf[oopGroupNumOffset] = oopGroupNum;   //重新记档案数量
    outRecordBuf->bufLen = offset;

    return result;
}
/**
*********************************************************************
* @brief：      读记录型方案任务
* @param[in]： inReadRecord  - 筛选需要的数据
               oopRcsd       - 列选择
               readRecNum    - 读记录数量，：0：读全部记录 1：读最近的一条记录
               maxBufLen     - 最大读取buf长度
* @param[out]：outRecordBuf  -  数据记录
* @return：     =0            - 成功
               <0            - 错误码
*********************************************************************
*/
int record_group_meter_read(DB_CLIENT hUdp, const READ_INF_T inReadNor, RELY_INFO_T relyInfo, uint32 maxBufLen, 
RECORD_BUF_T *outRecordBuf)
{
    if ((relyInfo.oopRsd->choice == 1) && (relyInfo.oopRsd->sel1.value.type == DT_OAD))
    {
        //return record_group_meter_read_by_port(hUdp, inReadMeter, relyInfo, maxBufLen, outRecordBuf);
    }
    else
    {
        return record_group_meter_read_by_tsa(hUdp, inReadNor, relyInfo, maxBufLen, outRecordBuf);
    }

    return ERR_OK;
}

/**
*********************************************************************
* @name：      class11_invoke_get_oop_meter_record
* @brief：     class11的GET服务
* @param[in] ：pOdaInfo        OAD信息
               pBpInfo        应用分帧断点信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
               outDataMax     输出报文缓冲区最大长度

* @param[out]：outData     输出报文
* @return    ：OOP_DAR_E
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E class11_invoke_get_group_meter_all_rec(OAD_INFO_T *pOdaInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    uint16 i = 0;
    uint16 j = 0;
    OOP_DAR_E  nRet = DATA_OTHER_REASON;
    uint32 offset = 4;
    uint16 csdStart = 0, csdLen = 0;
    uint16 outOffset = 0;
    uint16 inDataLenTmp = InDataLen;

    OOP_PRSD_T oopRsd;
    OOP_RCSD_T oopRcsd;
    uint16     headLen = 0;
    READ_INF_T inReadNor;
    RELY_INFO_T  relyInfo;
    RECORD_BUF_T outRecordBuf;

    /* 清0处理 */
    MEMZERO(&oopRsd, sizeof(oopRsd));
    MEMZERO(&oopRcsd, sizeof(oopRcsd));
    MEMZERO(&inReadNor, sizeof(inReadNor));
    MEMZERO(&relyInfo, sizeof(relyInfo));
    MEMZERO(&outRecordBuf, sizeof(outRecordBuf));

    outRecordBuf.recordBuf = (uint8 *)malloc(pApduInfo->connectionInfo.ApplSplitLen);
    memset(outRecordBuf.recordBuf, 0xee, pApduInfo->connectionInfo.ApplSplitLen);

    /*处理RSD*/
    nRet = apdu_rsd_fromfmt698(inData, inDataLenTmp, &offset, &oopRsd, sizeof(oopRsd));
    PRTL_FMT_LOGCC("oopRsd.choice = %d\n", oopRsd.choice);
    PRTL_FMT_LOGCC("offset = %d\n", offset);
    if (nRet != DATA_SUCCESS)
    {
        PRTL_FMT_TRACE("RSD解析错误\n");

        /*应对电科院协议一致性测试*/
        uint16 testOffset = 4;
        uint16 inCsdOff = 0, inCsdLen = 0;
        apdu_next_dpos_get(inData, &testOffset, InDataLen, DT_RSD, TRUE);
        inCsdOff = testOffset;
        apdu_next_dpos_get(inData, &testOffset, InDataLen, DT_RCSD, TRUE);
        inCsdLen = testOffset - inCsdOff;
        memcpy(outData->pResultData, inData, 4);
        memcpy(outData->pResultData + 4, inData + inCsdOff, inCsdLen);
        outData->resultDataLen = 4 + inCsdLen;

        outData->pResultData[outData->resultDataLen++] = 1;                   //后面优化下，outData->resultDataLen与下面统一
        if (apdu_rcsd_fromfmt698((uint8 *)inData, InDataLen, &offset, &oopRcsd, sizeof(oopRcsd)) != DATA_SUCCESS)
        {
            outData->pResultData[outData->resultDataLen++] = 0;

        }
        else
        {
            outData->pResultData[outData->resultDataLen++] = 1;
            for (i = 0; i < oopRcsd.nNum; i++)
            {
                outData->pResultData[outData->resultDataLen++] = NULL_DATA;
            }
        }

        free(outRecordBuf.recordBuf);
        outRecordBuf.recordBuf = NULL;
        return DATA_SUCCESS;
    }
    else
    {
        inDataLenTmp -= offset;  //除去RSD和OAD后的长度
    }

    /*处理RCSD*/
    csdStart = offset;
    if (apdu_rcsd_fromfmt698(inData, InDataLen, &offset, &oopRcsd, sizeof(oopRcsd)) != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("CSD解析错误!\n");
        free(outRecordBuf.recordBuf);
        outRecordBuf.recordBuf = NULL;
        return DATA_TYPE_UNMATCHED;
    }

    /*填写OAD+RCSD+记录结果1 并记下偏移headLen*/
    outData->resultDataLen = 0;
    csdLen = offset-csdStart;
    memcpy(outData->pResultData, inData, 4);
    outOffset = 4;

    if (csdLen == 1)
    {
        //如果rcsd为空，填充OAD6e060200
        outData->pResultData[outOffset ++] = 0x01;
        outData->pResultData[outOffset ++] = 0x00;
        outData->pResultData[outOffset ++] = 0x6e;
        outData->pResultData[outOffset ++] = 0x06;
        outData->pResultData[outOffset ++] = 0x02;
        outData->pResultData[outOffset ++] = 0x00;
    }
    else
    {
        memcpy(&outData->pResultData[outOffset], &inData[csdStart], csdLen);
        outOffset += csdLen;
    }
    outData->pResultData[outOffset++] = 1;
    outData->resultDataLen += outOffset;
    headLen = outOffset;
    PRTL_BUF_LOGCC(&outData->pResultData[0], headLen, "填写OAD+RCSD+记录结果1 并记下偏移headLen head:");

    /*记录型档案接口提供输入参数*/
    if (general_param_get_fj(&oopRsd, &oopRcsd, pApduInfo, &inReadNor, &relyInfo) != ERR_OK)
    {
        free(outRecordBuf.recordBuf);
        outRecordBuf.recordBuf = NULL;
        return DATA_READ_WRITE_DENIED;
    }

    /* 依赖信息中存下OAD列表的指针 */
    relyInfo.pOadInfo = pOdaInfo;

    PRTL_FMT_LOGCC("****************************************************\n");
    PRTL_FMT_LOGCC("inReadMeter.startTask = %d\n", inReadNor.startTask);
    PRTL_FMT_LOGCC("inReadMeter.endTask = %d\n", inReadNor.endTask);
    PRTL_FMT_LOGCC("relyInfo.oopRcsd.nNum = %d\n", relyInfo.oopRcsd.nNum);
    for(i = 0; i < relyInfo.oopRcsd.nNum; i++)
    {
        if (relyInfo.oopRcsd.cols[i].choice == 0)
        {
            PRTL_FMT_LOGCC("choice = %d relyInfo.oopRcsd.cols[%d].oad.value = 0x%08x\n", i, relyInfo.oopRcsd.cols[i].choice, 
                                                                                      relyInfo.oopRcsd.cols[i].oad.value);
        }
        else if(relyInfo.oopRcsd.cols[i].choice == 1)
        {
            PRTL_FMT_LOGCC("choice = %d relyInfo.oopRcsd.cols[%d].road.oadMain.value = 0x%08x\n", i, relyInfo.oopRcsd.cols[i].choice, 
                                                                                              relyInfo.oopRcsd.cols[i].road.oadMain.value);
            for (j = 0; j < relyInfo.oopRcsd.cols[i].road.oadCols.nNum; j++)
            {
                PRTL_FMT_LOGCC("choice = %d relyInfo.oopRcsd.cols[%d].road.oadCols.oad[%d].value = 0x%08x\n", i, j, relyInfo.oopRcsd.cols[i].choice, 
                                                                                                          relyInfo.oopRcsd.cols[i].road.oadCols.oad[j].value);
            }
        }
    }
    PRTL_FMT_LOGCC("****************************************************\n");

    /*读记录档案接口，可以读出apdu数据域，可返回断点信息*/
    if (record_group_meter_read(pApduInfo->hUdp, inReadNor, relyInfo, pApduInfo->connectionInfo.ApplSplitLen - headLen - cal_excludeLen(pApduInfo->connectionInfo), &outRecordBuf))
    {
        nRet = DATA_READ_WRITE_DENIED;
    }
    else
    {
        memcpy(&outData->pResultData[headLen], outRecordBuf.recordBuf, outRecordBuf.bufLen);
        outData->resultDataLen += outRecordBuf.bufLen;

        if (outRecordBuf.breakInfo.appSlicingFlag == 1)
        {
            /*记下断点信息*/
            PRTL_FMT_LOGCC("记下断点信息\n");
            outRecordBuf.breakInfo.objPre = 0x02;   //对象属性，记录型数据
            memcpy(pApduInfo->bpBreakInfo, &outRecordBuf.breakInfo, sizeof(BP_INFO_T));
        }
        else
        {
            /*清除断点信息*/
            PRTL_FMT_LOGCC("清除断点信息\n");
            memset(pApduInfo->bpBreakInfo, 0, sizeof(BP_INFO_T));
        }
        
        nRet = DATA_SUCCESS;
    }
    
    free(outRecordBuf.recordBuf);
    outRecordBuf.recordBuf = NULL;
    return DATA_SUCCESS;
}

/**
*********************************************************************
* @name：      class11_invoke_get_group_meter_all_nor
* @brief：     class11的GET服务
* @param[in] ：pOdaInfo        OAD信息
               pBpInfo        应用分帧断点信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
               outDataMax     输出报文缓冲区最大长度

* @param[out]：outData     输出报文
* @return    ：OOP_DAR_E
* @author    :
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E class11_invoke_get_group_meter_all_nor(OAD_INFO_T* pOadInfo, APDU_INFO_T* pApduInfo, const uint8* inData, uint16 inLen, RESULT_DATA_T* outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    DATA_CONVERT_T convert;
    uint32 sOffset = 0;
    uint32 dOffset = 0;
    uint32 offset = 0;
    uint32             i = 0;
    READ_RECORD_T      inReadRecord;
    uint32             handle = 0;
    uint32             recNum = 0;
    STEP_READ_RECORD_T stepReadRecord;
    RECORD_UNIT_T      stepRecordOut;
    OOP_GROUP_METER_T  dstdata;
    OOP_GROUPMETER_NEW_T  data;
    OOP_GROUPMETER_NEW_T  data2;
    uint32    realNum = 0;    //数组由于分帧导致的每帧实际成员数
    uint32    numOffset = 0;  //数组成员数在报文中的位置
    uint32    bufAllLen = 0;
    BP_INFO_T breakInfo = { 0 };                                                //断点信息
    uint32    maxBufLen = 0;
    BOOLEAN   isBufFull = FALSE;

    NOMAL_OAD_T normal = {0};
    uint32      len  = 0;

    maxBufLen = pApduInfo->connectionInfo.ApplSplitLen - cal_excludeLen(pApduInfo->connectionInfo);
    memset(&breakInfo, 0, sizeof(BP_INFO_T));
    memset(&data, 0, sizeof(data));
    memset(&data2, 0, sizeof(data2));
    memset(&dstdata, 0, sizeof(dstdata));
    MEMZERO(&convert, sizeof(convert));

    memset(&inReadRecord, 0, sizeof(READ_RECORD_T));
    memset(&stepReadRecord, 0, sizeof(STEP_READ_RECORD_T));
    memset(&stepRecordOut, 0, sizeof(RECORD_UNIT_T));
    //读出数据
    PRTL_FMT_DEBUG("read group meter start\n");
    inReadRecord.recordOAD.logicId = 0;

    if (pOadInfo->nIndex == 0)
    {
        inReadRecord.recordOAD.infoNum = 0xffff;
    }
    else
    {
        inReadRecord.recordOAD.infoNum = pOadInfo->nIndex;
    }

    inReadRecord.cType = COL_NO_INFO;
    inReadRecord.cStart = 0;
    inReadRecord.cEnd = 0xffff;
    inReadRecord.sortType = UP_SORT;
    inReadRecord.recordOAD.optional = 0;
    inReadRecord.recordOAD.road.oadMain.value = 0;
    inReadRecord.recordOAD.road.oadCols.nNum = 0x01;
    inReadRecord.recordOAD.road.oadCols.oad[0].value = pOadInfo->pTab->oad.value;
    stepReadRecord.MAC.nNum = 0;
    if (pApduInfo->bpBreakInfo->appSlicingFlag == 1)
    {
        inReadRecord.cStart = pApduInfo->bpBreakInfo->tmStartSeg;
    }
    memcpy(&outData->pResultData[offset], inData, sizeof(OOP_OAD_U));   //拷贝OAD
    offset += sizeof(OOP_OAD_U);
    outData->pResultData[offset++] = 0x01;                              //结果
    if (pOadInfo->nIndex == 0)
    {
        outData->pResultData[offset++] = DT_ARRAY;                         //类型
        numOffset = offset;                                                //数量偏移
        outData->pResultData[offset++] = 0x00;                             //数量，先填0
    }
    bufAllLen = offset;
    ret = db_read_record_start(pApduInfo->hUdp, &inReadRecord, &handle, &recNum);
    if ((ret != ERR_OK) || (recNum == 0))
    {
        PRTL_FMT_DEBUG("群组档案开始读失败 ret = %d, infoNum = %d recNum = %d\n",
            ret, inReadRecord.recordOAD.infoNum, recNum);
    }
    else
    {
        PRTL_FMT_DEBUG("群组档案开始读成功 ret = %d, infoNum = %d recNum = %d\n",
            ret, inReadRecord.recordOAD.infoNum, recNum);

        for (i = 0; i < recNum; i++)
        {
            ret = db_read_record_step(pApduInfo->hUdp, handle, &stepReadRecord, &stepRecordOut);
            if (ret != ERR_OK)
            {
                PRTL_FMT_DEBUG("群组档案单步读失败 db_read_record_step err = %d!\n", ret);
                continue;
                //数据中心出错返回NULL
            }

            //OAD与返回数据
            if ((stepRecordOut.recordnum != 1)
                || (stepRecordOut.record[0].road.oadCols.oad[0].value != pOadInfo->pTab->oad.value)
                || (stepRecordOut.record[0].outDatalen != sizeof(OOP_GROUPMETER_NEW_T)))
            {
                PRTL_FMT_DEBUG("读群组档案失败!\n");
                PRTL_FMT_DEBUG("读群组档案 stepRecordOut.recordnum = %d\n", stepRecordOut.recordnum);
                PRTL_FMT_DEBUG("读群组档案 stepRecordOut.record[0].road.oadCols.oad[0] = 0x%08x\n",
                    stepRecordOut.record[0].road.oadCols.oad[0].value);
                PRTL_FMT_DEBUG("读群组档案 stepRecordOut.record[0].outDatalen = %d\n",
                    stepRecordOut.record[0].outDatalen);
                PRTL_FMT_DEBUG("读群组档案 sizeof(OOP_GROUPMETER_NEW_T) = %d\n",
                    sizeof(OOP_GROUPMETER_NEW_T));
                continue;
            }

            memcpy(&data, stepRecordOut.record[0].outDataBuf, stepRecordOut.record[0].outDatalen);
            PRTL_FMT_DEBUG("stepRecordOut.infoNum = %d\n", stepRecordOut.infoNum);

            normal.oad.value = 0x6e05ffff;
            normal.infoNum = stepRecordOut.infoNum;
            ret = db_read_nomal(pApduInfo->hUdp, &normal, sizeof(data2), (uint8*)&data2, &len);
            if((ret != ERR_OK) || (len == 0))
            {
                PRTL_FMT_DEBUG("读普通数据失败. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                    ret, normal.logicId, normal.infoNum, normal.oad.value);
            }

            ret = groupmeter_new_to_old(&dstdata, &data, &data2);
            if (ret != ERR_OK)
            {
                PRTL_FMT_DEBUG("群组档案数据转换失败 ret = %d\n", ret);
                continue;
            }

            //转换成报文数据
            convert.srcBuf = &dstdata;
            convert.srcLen = sizeof(dstdata);
            convert.sOffset = &sOffset;

            convert.dstBuf = &outData->pResultData[offset];
            convert.dstLen = outDataMax - offset;
            convert.dOffset = &dOffset;

            breakInfo.tmStartSeg = stepRecordOut.infoNum;
            dar = data_to_buf_group_meter(pOadInfo, &convert);
            if (dar == DATA_SUCCESS)
            {
                bufAllLen += dOffset;
                if (bufAllLen <= maxBufLen)
                {
                    memcpy(&outData->pResultData[offset], convert.dstBuf, dOffset);
                    offset += dOffset;
                    outData->resultDataLen = offset;
                    PRTL_BUF_DEBUG(convert.dstBuf, dOffset, "第%d个群组档案:", i);
                    realNum++;
                    dOffset = 0;
                    sOffset = 0;
                }
                else
                {
                    dOffset = 0;
                    sOffset = 0;
                    isBufFull = TRUE;
                }
            }
            else
            {
                continue;
            }
            if ((isBufFull) && (offset > 0))
            {
                break;
            }
        }
    }
    db_read_record_end(pApduInfo->hUdp, handle);
    PRTL_FMT_DEBUG("read group meter end.\n");
    if (!isBufFull)
    {
        if (pApduInfo->bpBreakInfo->appSlicingFlag == 0x01)
        {
            breakInfo.appSlicingFlag = 0x01;
            breakInfo.isEnd = 0x01;                           //末尾帧
            breakInfo.seq = pApduInfo->bpBreakInfo->seq + 1;  //序号加1
            breakInfo.objPre = 0x01;                          //对象属性
        }
        else
        {
            memset(&breakInfo, 0, sizeof(BP_INFO_T));
        }
    }
    else
    {
        breakInfo.reqType = pApduInfo->reqType;
        breakInfo.appSlicingFlag = 0x01;
        breakInfo.isEnd = 0x00;                              //不是末尾帧
        breakInfo.objPre = 0x01;                             //对象属性
        if (pApduInfo->bpBreakInfo->appSlicingFlag == 0x00)
        {
            breakInfo.seq = BRK_SEQ_START;                   //首次序号
        }
        else
        {
            breakInfo.seq = pApduInfo->bpBreakInfo->seq + 1;  //序号加1
        }
    }
    memcpy(pApduInfo->bpBreakInfo, &breakInfo, sizeof(BP_INFO_T));

    if (pOadInfo->nIndex == 0)
    {
        outData->pResultData[numOffset] = realNum;   //重新记数量
    }

    outData->resultDataLen = offset;

    return dar;
}

/**
*********************************************************************
* @name：      class11_invoke_get_group_meter_all_nor
* @brief：     class11的GET服务
* @param[in] ：pOdaInfo        OAD信息
               pBpInfo        应用分帧断点信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
               outDataMax     输出报文缓冲区最大长度

* @param[out]：outData     输出报文
* @return    ：OOP_DAR_E
* @author    :
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E class11_invoke_get_group_meter_num_nor(OAD_INFO_T* pOadInfo, APDU_INFO_T* pApduInfo, const uint8* inData, uint16 inLen, RESULT_DATA_T* outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    DATA_CONVERT_T convert;
    uint32 sOffset = 0;
    uint32 dOffset = 0;
    uint32 offset = 0;
    uint32             i = 0;
    READ_RECORD_T      inReadRecord;
    uint32             handle = 0;
    uint32             recNum = 0;
    STEP_READ_RECORD_T stepReadRecord;
    RECORD_UNIT_T      stepRecordOut;
    OOP_GROUP_NUM_T       groupNumInfo;
    OOP_GROUPMETER_NEW_T  data;
    OOP_GROUPMETER_NEW_T  data2;
    uint32    realNum = 0;    //数组由于分帧导致的每帧实际成员数
    uint32    numOffset = 0;  //数组成员数在报文中的位置
    uint32    bufAllLen = 0;
    BP_INFO_T breakInfo = { 0 };                                                //断点信息
    uint32    maxBufLen = 0;
    BOOLEAN   isBufFull = FALSE;

    NOMAL_OAD_T normal = {0};
    uint32      len = 0;

    maxBufLen = pApduInfo->connectionInfo.ApplSplitLen - cal_excludeLen(pApduInfo->connectionInfo);
    memset(&breakInfo, 0, sizeof(BP_INFO_T));
    memset(&data, 0, sizeof(data));
    memset(&data2, 0, sizeof(data2));
    memset(&groupNumInfo, 0, sizeof(groupNumInfo));
    MEMZERO(&convert, sizeof(convert));

    memset(&inReadRecord, 0, sizeof(READ_RECORD_T));
    memset(&stepReadRecord, 0, sizeof(STEP_READ_RECORD_T));
    memset(&stepRecordOut, 0, sizeof(RECORD_UNIT_T));

    inReadRecord.recordOAD.logicId = 0;

    if (pOadInfo->nIndex == 0)
    {
        inReadRecord.recordOAD.infoNum = 0xffff;
    }
    else
    {
        inReadRecord.recordOAD.infoNum = pOadInfo->nIndex;
    }

    inReadRecord.cType = COL_NO_INFO;
    inReadRecord.cStart = 0;
    inReadRecord.cEnd = 0xffff;
    inReadRecord.sortType = UP_SORT;
    inReadRecord.recordOAD.optional = 0;
    inReadRecord.recordOAD.road.oadMain.value = 0;
    inReadRecord.recordOAD.road.oadCols.nNum = 0x01;
    inReadRecord.recordOAD.road.oadCols.oad[0].value = 0x6e050200;
    stepReadRecord.MAC.nNum = 0;
    if (pApduInfo->bpBreakInfo->appSlicingFlag == 1)
    {
        inReadRecord.cStart = pApduInfo->bpBreakInfo->tmStartSeg;
    }
    memcpy(&outData->pResultData[offset], inData, sizeof(OOP_OAD_U));   //拷贝OAD
    offset += sizeof(OOP_OAD_U);
    outData->pResultData[offset++] = 0x01;                              //结果
    if (pOadInfo->nIndex == 0)
    {
        outData->pResultData[offset++] = DT_ARRAY;                         //类型
        numOffset = offset;                                                //数量偏移
        outData->pResultData[offset++] = 0x00;                             //数量，先填0
    }
    bufAllLen = offset;
    ret = db_read_record_start(pApduInfo->hUdp, &inReadRecord, &handle, &recNum);
    if ((ret != ERR_OK) || (recNum == 0))
    {
        PRTL_FMT_DEBUG("群组档案开始读失败 ret = %d, infoNum = %d recNum = %d\n",
            ret, inReadRecord.recordOAD.infoNum, recNum);
    }
    else
    {
        PRTL_FMT_DEBUG("群组档案开始读成功 ret = %d, infoNum = %d recNum = %d\n",
            ret, inReadRecord.recordOAD.infoNum, recNum);

        for (i = 0; i < recNum; i++)
        {
            ret = db_read_record_step(pApduInfo->hUdp, handle, &stepReadRecord, &stepRecordOut);
            if (ret != ERR_OK)
            {
                PRTL_FMT_DEBUG("群组档案单步读失败 db_read_record_step err = %d!\n", ret);
                continue;
                //数据中心出错返回NULL
            }

            //OAD与返回数据
            if ((stepRecordOut.recordnum != 1)
                || (stepRecordOut.record[0].road.oadCols.oad[0].value != 0x6e050200)
                || (stepRecordOut.record[0].outDatalen != sizeof(OOP_GROUPMETER_NEW_T)))
            {
                PRTL_FMT_DEBUG("读群组档案失败!\n");
                PRTL_FMT_DEBUG("读群组档案 stepRecordOut.recordnum = %d\n", stepRecordOut.recordnum);
                PRTL_FMT_DEBUG("读群组档案 stepRecordOut.record[0].road.oadCols.oad[0] = 0x%08x\n",
                    stepRecordOut.record[0].road.oadCols.oad[0].value);
                PRTL_FMT_DEBUG("读群组档案 stepRecordOut.record[0].outDatalen = %d\n",
                    stepRecordOut.record[0].outDatalen);
                PRTL_FMT_DEBUG("读群组档案 sizeof(OOP_GROUPMETER_NEW_T) = %d\n",
                    sizeof(OOP_GROUPMETER_NEW_T));
                continue;
            }

            memcpy(&data, stepRecordOut.record[0].outDataBuf, stepRecordOut.record[0].outDatalen);
            PRTL_FMT_DEBUG("stepRecordOut.infoNum = %d\n", stepRecordOut.infoNum);

            normal.oad.value = 0x6e05ffff;
            normal.infoNum = stepRecordOut.infoNum;
            ret = db_read_nomal(pApduInfo->hUdp, &normal, sizeof(data2), (uint8*)&data2, &len);
            if((ret != ERR_OK) || (len == 0))
            {
                PRTL_FMT_DEBUG("读普通数据失败. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                    ret, normal.logicId, normal.infoNum, normal.oad.value);
            }

            groupNumInfo.groupAdd = data.groupAddr;
            groupNumInfo.num = data.num + data2.num;


            //转换成报文数据
            convert.srcBuf = &groupNumInfo;
            convert.srcLen = sizeof(groupNumInfo);
            convert.sOffset = &sOffset;

            convert.dstBuf = &outData->pResultData[offset];
            convert.dstLen = outDataMax - offset;
            convert.dOffset = &dOffset;

            breakInfo.tmStartSeg = stepRecordOut.infoNum;
            dar = data_to_buf_group_meter_num(pOadInfo, &convert);
            if (dar == DATA_SUCCESS)
            {
                bufAllLen += dOffset;
                if (bufAllLen <= maxBufLen)
                {
                    memcpy(&outData->pResultData[offset], convert.dstBuf, dOffset);
                    offset += dOffset;
                    outData->resultDataLen = offset;
                    PRTL_BUF_DEBUG(convert.dstBuf, dOffset, "第%d个群组档案数量报文:", i);
                    realNum++;
                    dOffset = 0;
                    sOffset = 0;
                }
                else
                {
                    dOffset = 0;
                    sOffset = 0;
                    isBufFull = TRUE;
                }
            }
            else
            {
                continue;
            }
            if ((isBufFull) && (offset > 0))
            {
                break;
            }
        }
    }
    db_read_record_end(pApduInfo->hUdp, handle);
    if (!isBufFull)
    {
        if (pApduInfo->bpBreakInfo->appSlicingFlag == 0x01)
        {
            breakInfo.appSlicingFlag = 0x01;
            breakInfo.isEnd = 0x01;                           //末尾帧
            breakInfo.seq = pApduInfo->bpBreakInfo->seq + 1;  //序号加1
            breakInfo.objPre = 0x01;                          //对象属性
        }
        else
        {
            memset(&breakInfo, 0, sizeof(BP_INFO_T));
        }
    }
    else
    {
        breakInfo.reqType = pApduInfo->reqType;
        breakInfo.appSlicingFlag = 0x01;
        breakInfo.isEnd = 0x00;                              //不是末尾帧
        breakInfo.objPre = 0x01;                             //对象属性
        if (pApduInfo->bpBreakInfo->appSlicingFlag == 0x00)
        {
            breakInfo.seq = BRK_SEQ_START;                   //首次序号
        }
        else
        {
            breakInfo.seq = pApduInfo->bpBreakInfo->seq + 1;  //序号加1
        }
    }
    memcpy(pApduInfo->bpBreakInfo, &breakInfo, sizeof(BP_INFO_T));

    if (pOadInfo->nIndex == 0)
    {
        outData->pResultData[numOffset] = realNum;   //重新记数量
    }

    outData->resultDataLen = offset;

    return dar;

}

/**
*********************************************************************
* @name：      class11_invoke_get_group_meter_all
* @brief：     class11的GET服务
* @param[in] ：pOdaInfo        OAD信息
               pBpInfo        应用分帧断点信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
               outDataMax     输出报文缓冲区最大长度

* @param[out]：outData     输出报文
* @return    ：OOP_DAR_E
* @author    :
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E class11_invoke_get_group_meter_all(OAD_INFO_T* pOadInfo, APDU_INFO_T* pApduInfo, const uint8* inData, 
    uint16 inLen, RESULT_DATA_T* outData, uint16 outDataMax)
{
    //如果是应用分帧状态，将apdu[1]修改成起始帧方式
    if (pApduInfo->bpBreakInfo->appSlicingFlag == 1)
    {
        pApduInfo->reqType = pApduInfo->bpBreakInfo->reqType;
    }

    switch (pApduInfo->reqType)
    {
    case 0x01:
    case 0x02:
        return class11_invoke_get_group_meter_all_nor(pOadInfo, pApduInfo, inData, inLen, outData, outDataMax);
        break;
    case 0x03:
    case 0x04:
        return class11_invoke_get_group_meter_all_rec(pOadInfo, pApduInfo, inData, inLen, outData, outDataMax);
        break;
    default:
        break;
    }

    return DATA_OBJECT_UNDEFINED;
}

/**
*********************************************************************
* @name：      class11_invoke_get_group_meter_num
* @brief：     class11的GET服务
* @param[in] ：pOdaInfo        OAD信息
               pBpInfo        应用分帧断点信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
               outDataMax     输出报文缓冲区最大长度

* @param[out]：outData     输出报文
* @return    ：OOP_DAR_E
* @author    :
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E class11_invoke_get_group_meter_num(OAD_INFO_T* pOadInfo, APDU_INFO_T* pApduInfo, const uint8* inData, 
    uint16 inLen, RESULT_DATA_T* outData, uint16 outDataMax)
{
    //如果是应用分帧状态，将apdu[1]修改成起始帧方式
    if (pApduInfo->bpBreakInfo->appSlicingFlag == 1)
    {
        pApduInfo->reqType = pApduInfo->bpBreakInfo->reqType;
    }

    switch (pApduInfo->reqType)
    {
    case 0x01:
    case 0x02:
        return class11_invoke_get_group_meter_num_nor(pOadInfo, pApduInfo, inData, inLen, outData, outDataMax);
        break;
    case 0x03:
    case 0x04:
        //return class11_invoke_get_group_meter_num_rec(pOadInfo, pApduInfo, inData, inLen, outData, outDataMax);
        break;
    default:
        break;
    }

    return DATA_OBJECT_UNDEFINED;
}
#endif

#if DESC("组群档案相关操作", 1)
/**
*********************************************************************
* @name：      set_default_group_meter_fj
* @brief：     设置默认群组档案
* @param[in] ：addr      - 终端地址
               clientid - 数据中心句柄
* @param[out]：
* @return    ：
* @author    : 
* @Date      ：2022-3-29
*********************************************************************
*/
void set_default_group_meter_fj(int clientid, OOP_OCTETVAR16_T *addr)
{
    int32       ret = 0;
    NOMAL_OAD_T nomalOad = {0};
    OOP_GROUPMETER_NEW_T dataNEW;
    OOP_GROUPMETER_NEW_T dataNEWtmp;
    OOP_OCTETVAR16_T     tLocalAddr;

    uint32           readlen = 0;
    NOMAL_OAD_T      tOAD;

    if (addr == NULL)
    {
        memset(&tOAD, 0, sizeof(NOMAL_OAD_T));

        tOAD.oad.value = 0x40010200;
        memset(&tLocalAddr, 0, sizeof(OOP_OCTETVAR16_T));
        ret = db_read_nomal(clientid, &tOAD, sizeof(OOP_OCTETVAR16_T), (uint8 *)&tLocalAddr, &readlen);
        if (0 != ret || readlen != sizeof(OOP_OCTETVAR16_T))
        {
            PRTL_FMT_DEBUG("默认群组地址时读终端地址失败 错误码%d\n", ret);
            return;
        }
    }
    else
    {
        tLocalAddr = *addr;
    }

    //填写群组档案0
    memset(&dataNEW, 0, sizeof(dataNEW));
    dataNEW.groupAddr.len = 0x05;
    dataNEW.groupAddr.add[0] = 0x88;
    dataNEW.groupAddr.add[1] = 0x88;
    dataNEW.groupAddr.add[2] = 0x88;
    dataNEW.groupAddr.add[3] = 0x88;
    dataNEW.groupAddr.add[4] = 0x00;
    dataNEW.groupAddr.add[5] = 0x00;
    dataNEW.choice = 3;
    dataNEW.num = 1;
    memcpy(&dataNEW.addr[0].add, tLocalAddr.value, 6);

    memset(&tOAD, 0, sizeof(NOMAL_OAD_T));
    tOAD.infoNum = 0;
    tOAD.oad.value = 0x6e050200;
    memset(&dataNEWtmp, 0, sizeof(dataNEWtmp));
    ret = db_read_nomal(clientid, &tOAD, sizeof(dataNEWtmp), (uint8 *)&dataNEWtmp, &readlen);
    if (0 != ret || readlen != sizeof(dataNEWtmp))
    {
        PRTL_FMT_DEBUG("默认群组地址时读终端地址失败 错误码%d\n", ret);
    }
    
    if (memcmp(&dataNEW, &dataNEWtmp, sizeof(dataNEW)) != 0)
    {
        PRTL_FMT_DEBUG("群组档案存入数据中心的信息点为%d\n", nomalOad.infoNum);
        nomalOad.logicId = 0x00;
        nomalOad.infoNum = 0;
        nomalOad.oad.value = 0x6e050200;
        nomalOad.classtag = CLASS_DATA_UNINIT;
        ret = db_write_nomal(clientid, &nomalOad, (uint8*)&dataNEW, sizeof(dataNEW));
        if (ret != ERR_OK)
        {
            PRTL_FMT_DEBUG("写入数据中心失败. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                ret, nomalOad.logicId, nomalOad.infoNum, nomalOad.oad.value);
        }
    }

    //填写群组档案1
    memset(&dataNEW, 0, sizeof(dataNEW));
    dataNEW.groupAddr.len = 0x05;
    dataNEW.groupAddr.add[0] = 0x88;
    dataNEW.groupAddr.add[1] = 0x88;
    dataNEW.groupAddr.add[2] = 0x88;
    dataNEW.groupAddr.add[3] = 0x88;
    dataNEW.groupAddr.add[4] = 0x00;
    dataNEW.groupAddr.add[5] = 0x01;
    dataNEW.choice = 3;
    dataNEW.num = 1;
    memset(&dataNEW.addr[0].add, 0x99, 6);

    memset(&tOAD, 0, sizeof(NOMAL_OAD_T));
    tOAD.infoNum = 1;
    tOAD.oad.value = 0x6e050200;
    memset(&dataNEWtmp, 0, sizeof(dataNEWtmp));
    ret = db_read_nomal(clientid, &tOAD, sizeof(dataNEWtmp), (uint8 *)&dataNEWtmp, &readlen);
    if (0 != ret || readlen != sizeof(dataNEWtmp))
    {
        PRTL_FMT_DEBUG("默认群组地址时读终端地址失败 错误码%d\n", ret);
    }
    
    if (memcmp(&dataNEW, &dataNEWtmp, sizeof(dataNEW)) != 0)
    {
        PRTL_FMT_DEBUG("群组档案存入数据中心的信息点为%d\n", nomalOad.infoNum);
        nomalOad.logicId = 0x00;
        nomalOad.infoNum = 1;
        nomalOad.oad.value = 0x6e050200;
        nomalOad.classtag = CLASS_DATA_UNINIT;
        ret = db_write_nomal(clientid, &nomalOad, (uint8*)&dataNEW, sizeof(dataNEW));
        if (ret != ERR_OK)
        {
            PRTL_FMT_DEBUG("写入数据中心失败. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                ret, nomalOad.logicId, nomalOad.infoNum, nomalOad.oad.value);
        }
    }

    return;
}

/**
*********************************************************************
* @name：      groupmeter_new_from_ms_choice3
* @brief：     群组档案2个结构体转换(记录数据方式)
* @param[in] ：
* @param[out]：
* @return    ：
* @author    : 
* @Date      ：2022-3-29
*********************************************************************
*/
int groupmeter_new_from_ms_choice3(OOP_GROUPMETER_NEW_T *pGroupMeterNEW1, OOP_GROUPMETER_NEW_T *pGroupMeterNEW2, 
    OOP_GROUP_METER_T *pGroupMs)
{
    uint32 i = 0;
    
    OOP_MSPF_T *ms = &pGroupMs->ms;

    pGroupMeterNEW1->groupAddr = pGroupMs->groupAdd;
    pGroupMeterNEW2->groupAddr = pGroupMs->groupAdd;
    pGroupMeterNEW1->choice = 3;
    pGroupMeterNEW2->choice = 3;

    if (ms->msa.size > 2*GROUPMETER_NUM)
    {
        PRTL_FMT_DEBUG("群组档案数量超限 ms->msa.size[%d] > %d", ms->msa.size, 2*GROUPMETER_NUM);
        return ERR_ADDR;
    }

    for (i = 0; i < ms->msa.size; i++)
    {
        if (i < GROUPMETER_NUM)
        {
            memcpy(&pGroupMeterNEW1->addr[i].add[0], &ms->msa.add[i].add[0], sizeof(OOP_ADDR_T));
            pGroupMeterNEW1->num ++;
        }
        else
        {
            memcpy(&pGroupMeterNEW2->addr[i].add[0], &ms->msa.add[i].add[0], sizeof(OOP_ADDR_T));
            pGroupMeterNEW2->num ++;
        }
    }

    return ERR_OK;
}



/**
*********************************************************************
* @name：      groupmeter_new_from_ms_choice6
* @brief：     群组档案2个结构体转换(记录数据方式)
* @param[in] ：clientid  数据库句柄
* @param[out]：
* @return    ：
* @author    : 
* @Date      ：2022-3-29
*********************************************************************
*/
int groupmeter_new_from_ms_choice6(OOP_GROUPMETER_NEW_T *pGroupMeterNEW1, OOP_GROUPMETER_NEW_T *pGroupMeterNEW2, OOP_GROUP_METER_T *pGroupMs)
{
    uint32 i = 0;
    uint64 startAddr = 0;
    uint64 endAddr = 0;
    uint32 groupMeterIndex = 0;
    
    OOP_MSPF_T *ms = &pGroupMs->ms;
    
    pGroupMeterNEW1->groupAddr = pGroupMs->groupAdd;
    pGroupMeterNEW2->groupAddr = pGroupMs->groupAdd;
    pGroupMeterNEW1->choice = 6;
    pGroupMeterNEW2->choice = 6;

    for (i = 0; i < ms->msra.size; i++)
    {
        if ((ms->msra.region[i].start.len != 5) || (ms->msra.region[i].end.len != 5))
        {
            PRTL_FMT_DEBUG("ms集合中地址长度异常 start.len[%d] end.len[%d]", ms->msra.region[i].start.len, 
                ms->msra.region[i].end.len);
        }
        
        if (ms->msra.region[i].start.flag == 1)
        {
            memcpy_r(&startAddr, &ms->msra.region[i].start.add[0], ms->msra.region[i].start.len);
        }
        else
        {
            memcpy_r(&startAddr, &ms->msra.region[i].start.add[0], ms->msra.region[i].start.len + 1);
        }
        
        if (ms->msra.region[i].end.flag == 1)
        {
            memcpy_r(&endAddr, &ms->msra.region[i].end.add[0], ms->msra.region[i].end.len);
        }
        else
        {
            memcpy_r(&endAddr, &ms->msra.region[i].end.add[0], ms->msra.region[i].end.len + 1);
        }
                
        switch (ms->msra.region[i].nType)
        {
        case 0:
        {
            //前闭后开
            while (startAddr < endAddr)
            {
                //判断表地址是否合法
                if (!meteraddr_is_legally(&startAddr))
                {
                    startAddr ++;
                    continue;
                }

                if (groupMeterIndex > 2*GROUPMETER_NUM - 1)
                {
                    PRTL_FMT_DEBUG("群组档案数量超限 groupMeterIndex[%d] > %d", groupMeterIndex, 2*GROUP_METER_NUM);
                    break;
                }

                if (groupMeterIndex < GROUPMETER_NUM)
                {
                    memcpy_r(&pGroupMeterNEW1->addr[groupMeterIndex].add[0], &startAddr, 6);
                    pGroupMeterNEW1->num ++;
                }
                else
                {
                    memcpy_r(&pGroupMeterNEW2->addr[groupMeterIndex - GROUPMETER_NUM].add[0], &startAddr, 6);
                    pGroupMeterNEW2->num ++;
                }
                
                groupMeterIndex ++;
                
                startAddr ++;
            }
            break;
        }
        case 1:
        {
            //前开后闭
            while (startAddr <= endAddr)
            {
                startAddr ++;
                
                //判断表地址是否合法
                if (!meteraddr_is_legally(&startAddr))
                {
                    continue;
                }

                if (groupMeterIndex > 2*GROUP_METER_NUM - 1)
                {
                    PRTL_FMT_DEBUG("群组档案数量超限 groupMeterIndex[%d] > %d", groupMeterIndex, GROUP_METER_NUM);
                    break;
                }
                                
                if (groupMeterIndex < GROUPMETER_NUM)
                {
                    memcpy_r(&pGroupMeterNEW1->addr[groupMeterIndex].add[0], &startAddr, 6);
                    pGroupMeterNEW1->num ++;
                }
                else
                {
                    memcpy_r(&pGroupMeterNEW2->addr[groupMeterIndex - GROUPMETER_NUM].add[0], &startAddr, 6);
                    pGroupMeterNEW2->num ++;
                }

                groupMeterIndex ++;
            }
            break;
        }
        case 2:
        {
            //前闭后闭
            while (startAddr <= endAddr)
            {
                //判断表地址是否合法
                if (!meteraddr_is_legally(&startAddr))
                {
                    startAddr ++;
                    continue;
                }

                if (groupMeterIndex > 2*GROUP_METER_NUM - 1)
                {
                    PRTL_FMT_DEBUG("群组档案数量超限 groupMeterIndex[%d] > %d", groupMeterIndex, GROUP_METER_NUM);
                    break;
                }
                                
                if (groupMeterIndex < GROUPMETER_NUM)
                {
                    memcpy_r(&pGroupMeterNEW1->addr[groupMeterIndex].add[0], &startAddr, 6);
                    pGroupMeterNEW1->num ++;
                }
                else
                {
                    memcpy_r(&pGroupMeterNEW2->addr[groupMeterIndex - GROUPMETER_NUM].add[0], &startAddr, 6);
                    pGroupMeterNEW2->num ++;
                }

                groupMeterIndex ++;

                startAddr ++;
            }
            break;
        }
        case 3:
        {
            //前开后开
            while (startAddr < endAddr)
            {
                startAddr ++;
                
                //判断表地址是否合法
                if (!meteraddr_is_legally(&startAddr))
                {
                    continue;
                }

                if (groupMeterIndex > 2*GROUP_METER_NUM - 1)
                {
                    PRTL_FMT_DEBUG("群组档案数量超限 groupMeterIndex[%d] > %d", groupMeterIndex, GROUP_METER_NUM);
                    break;
                }
                                
                if (groupMeterIndex < GROUPMETER_NUM)
                {
                    memcpy_r(&pGroupMeterNEW1->addr[groupMeterIndex].add[0], &startAddr, 6);
                    pGroupMeterNEW1->num ++;
                }
                else
                {
                    memcpy_r(&pGroupMeterNEW2->addr[groupMeterIndex - GROUPMETER_NUM].add[0], &startAddr, 6);
                    pGroupMeterNEW2->num ++;
                }

                groupMeterIndex ++;
                
            }
            break;
        }
        default:
            break;
        }
    }

    return ERR_OK;
}

/**
*********************************************************************
* @name：      groupmeter_new_info_from_ms
* @brief：     群组档案2个结构体转换(记录数据方式)
* @param[in] ：
* @param[out]：
* @return    ：
* @author    : 
* @Date      ：2022-3-29
*********************************************************************
*/
int groupmeter_new_info_from_ms(OOP_GROUPMETER_NEW_T *pGroupMeterNEW1, OOP_GROUPMETER_NEW_T *pGroupMeterNEW2, 
    OOP_GROUP_METER_T *pGroupMs)
{
    switch (pGroupMs->ms.choice)
    {
    case 3:
    {
        return groupmeter_new_from_ms_choice3(pGroupMeterNEW1, pGroupMeterNEW2, pGroupMs);
        break;
    }
    case 6:
    {
        return groupmeter_new_from_ms_choice6(pGroupMeterNEW1, pGroupMeterNEW2, pGroupMs);
        break;
    }
    default:
        break;
    }

    return ERR_NORMAL;
}

/**
*********************************************************************
* @name：       group_meter_read
* @brief：      读群组档案(转换成大结构体)
* @param[in] ：infoNum     信息点
* @param[out]：data        群组档案(转换成大结构体)
* @return    ：错误码
* @author    : 
* @Date      ：2022-3-29
*********************************************************************
*/
int group_meter_read(DB_CLIENT fd, uint16 infoNum, OOP_GROUP_METER_T *data)
{
    int ret = ERR_OK;
    
    NOMAL_OAD_T normal = {0};
    uint32      len = 0;

    OOP_GROUPMETER_NEW_T  data1;
    OOP_GROUPMETER_NEW_T  data2;
    OOP_GROUP_METER_T     dataBig;

    if (data == NULL)
    {
        return ERR_PTR;
    }

    memset(&data1, 0, sizeof(data1));
    memset(&data2, 0, sizeof(data2));
    memset(&dataBig, 0, sizeof(dataBig));

    normal.oad.value = 0x6e050200;
    normal.infoNum = infoNum;

    ret = db_read_nomal(fd, &normal, sizeof(data1), (uint8*)&data1, &len);
    if ((ret != ERR_OK) || (len != sizeof(data1)))
    {
        PRTL_FMT_DEBUG("读主群组档案失败. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
            ret, normal.logicId, normal.infoNum, normal.oad.value);
        return ERR_IFACE;
    }

    normal.oad.value = 0x6e05ffff;

    ret = db_read_nomal(fd, &normal, sizeof(data2), (uint8*)&data2, &len);
    if ((ret != ERR_OK) || (len != sizeof(data2)))
    {
        PRTL_FMT_DEBUG("读次群组档案失败. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
            ret, normal.logicId, normal.infoNum, normal.oad.value);
    }

    ret = groupmeter_new_to_old(&dataBig, &data1, &data2);
    if (ret != ERR_OK)
    {
        PRTL_FMT_DEBUG("群组档案数据转换失败 ret = %d\n", ret);
        return ERR_UNCOMPATIBLE;
    }

    memcpy(data, &dataBig, sizeof(dataBig));
    
    return ERR_OK;
}

/**
*********************************************************************
* @name：       group_meter_write
* @brief：      写群组档案(转换成大结构体)
* @param[in] ：infoNum     信息点
* @param[out]：data        群组档案(转换成大结构体)
* @return    ：错误码
* @author    : 
* @Date      ：2022-3-29
*********************************************************************
*/
int group_meter_write(DB_CLIENT fd, OOP_GROUP_METER_T *data)
{
    int         ret = ERR_NORMAL;
    NOMAL_OAD_T nomalOad = { 0 };

    OOP_GROUPMETER_NEW_T dataNEW1;
    OOP_GROUPMETER_NEW_T dataNEW2;

    if (data == NULL)
    {
        return ERR_PTR;
    }

    memset(&dataNEW1, 0, sizeof(dataNEW1));
    memset(&dataNEW2, 0, sizeof(dataNEW2));

    ret = groupmeter_new_info_from_ms(&dataNEW1, &dataNEW2, data);
    if (ret != ERR_OK)
    {
        PRTL_FMT_DEBUG("群组档案数据转换失败，ret = %d data.ms.choice[%d]\n", ret, data->ms.choice);
        return ERR_UNCOMPATIBLE;
    }

    nomalOad.logicId = 0x00;
    nomalOad.infoNum = (uint8)(commfun_BcdToHex(data->groupAdd.add[4]) >> 8) * 1000 + (uint8)(commfun_BcdToHex(data->groupAdd.add[4]) & 0xff) * 100 +
                       (uint8)(commfun_BcdToHex(data->groupAdd.add[5]) >> 8) * 10   + (uint8)(commfun_BcdToHex(data->groupAdd.add[5]) & 0xff);
    nomalOad.oad.value = 0x6e050200;
    nomalOad.classtag = CLASS_DATA_INIT;

    if (dataNEW1.num > 0)
    {
        nomalOad.oad.value = 0x6e050200;
        ret = db_write_nomal(fd, &nomalOad, (uint8*)&dataNEW1, sizeof(dataNEW1));
        if (ret != ERR_OK)
        {
            PRTL_FMT_DEBUG("写入数据中心失败. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                ret, nomalOad.logicId, nomalOad.infoNum, nomalOad.oad.value);
            return DATA_READ_WRITE_DENIED;
        }
    }
    
    if (dataNEW2.num > 0)
    {
        nomalOad.oad.value = 0x6e05ffff;
        ret = db_write_nomal(fd, &nomalOad, (uint8*)&dataNEW2, sizeof(dataNEW2));
        if (ret != ERR_OK)
        {
            PRTL_FMT_DEBUG("写入数据中心失败. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                ret, nomalOad.logicId, nomalOad.infoNum, nomalOad.oad.value);
            return DATA_READ_WRITE_DENIED;
        }
    }

    return ERR_OK;
}

/**
*********************************************************************
* @name：       trans_node_ms_f_add
* @brief：      ms集合的添加处理(ms3 = ms1 + ms2)
* @param[in] ：ms 电表集合
* @param[out]: ms 电表集合
* @return    ：错误码
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
int trans_node_ms_f_add(OOP_MSPF_T *ms1, OOP_MSPF_T *ms2, OOP_MSPF_T *ms3)
{
    uint32 i = 0;
    uint32 j = 0;
    uint32 addConnt = 0;
    OOP_MSPF_T ms;
    OOP_TSA_T tsa;
    
    if ((ms1 == NULL) || (ms2 == NULL) || (ms3 == NULL))
    {
        return ERR_PTR;
    }

    memset(&tsa, 0, sizeof(tsa));
    memset(&ms, 0, sizeof(ms));

    ms.choice = 3;

    if ((ms1->choice != 3) || (ms2->choice != 3))
    {
        PRTL_FMT_DEBUG("暂时不支持该方法 choice1[%d] choice[%d]\n", ms1->choice, ms2->choice);
        return ERR_TYPE;
    }

    for (i = 0; i < ms2->msa.size; i++)
    {
        tsa = ms2->msa.add[i];
        
        for (j = 0; j < ms1->msa.size; j++)
        {
            //有相同的表地址，则跳出
            if (memcmp(&tsa, &ms1->msa.add[j], sizeof(OOP_TSA_T)) == 0)
            {
                break;
            }
        }

        if (j == ms1->msa.size)
        {
            ms.msa.add[addConnt] = tsa;
            addConnt ++;
            ms.msa.size ++;
            PRTL_BUF_TRACE(tsa.add, tsa.len + 1, "发现需要添加的表地址:");
        }
    }

    if (addConnt > 0)
    {
        if (ms3->msa.size + addConnt > OOP_MAX_MSPARAM_F)
        {
            PRTL_FMT_DEBUG("ms集合无法存入太多的表 cur[%d] max[%d]\n", ms3->msa.size + addConnt, OOP_MAX_MSPARAM_F);
            return ERR_INVALID;
        }
        
        memcpy(ms3, ms1, sizeof(OOP_MSPF_T));
        for (i = 0; i < addConnt; i++)
        {
            ms3->msa.add[ms3->msa.size + i] = ms.msa.add[i];
        }
        ms3->msa.size += addConnt;
    }
    else
    {
        PRTL_FMT_DEBUG("ms2中表地址均存在于ms1中，无需添加 addConnt[%d] ms.msa.size[%d]\n", addConnt, ms.msa.size);
        return ERR_INVALID;
    }

    return ERR_OK;
}

/**
*********************************************************************
* @name：       trans_node_ms_f_del
* @brief：      ms集合的添加处理(ms3 = ms1 - ms2)
* @param[in] ：ms 电表集合
* @param[out]: ms 电表集合
* @return    ：错误码
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
int trans_node_ms_f_del(OOP_MSPF_T *ms1, OOP_MSPF_T *ms2, OOP_MSPF_T *ms3)
{
    uint32 i = 0;
    uint32 j = 0;
    uint32 count = 0;
    OOP_MSPF_T ms;
    OOP_TSA_T tsa;
    
    if ((ms1 == NULL) || (ms2 == NULL) || (ms3 == NULL))
    {
        return ERR_PTR;
    }

    memset(&tsa, 0, sizeof(tsa));
    memset(&ms, 0, sizeof(ms));

    ms.choice = 3;

    if ((ms1->choice != 3) || (ms2->choice != 3))
    {
        PRTL_FMT_DEBUG("暂时不支持该方法 choice1[%d] choice[%d]\n", ms1->choice, ms2->choice);
        return ERR_TYPE;
    }

    for (i = 0; i < ms1->msa.size; i++)
    {
        tsa = ms1->msa.add[i];
        
        for (j = 0; j < ms2->msa.size; j++)
        {
            //有相同的表地址，则跳出
            if (memcmp(&tsa, &ms2->msa.add[j], sizeof(OOP_TSA_T)) == 0)
            {
                PRTL_BUF_TRACE(tsa.add, tsa.len + 1, "发现需要删除的表地址:");
                break;
            }
        }

        //将不需要删除的表存入新的ms
        if (j == ms2->msa.size)
        {
            ms.msa.add[count] = tsa;
            count ++;
            ms.msa.size ++;
        }

    }

    if (count == 0)
    {
        PRTL_FMT_DEBUG("ms1中表地址被清空 count[%d]\n", ms.msa.size);
    }
    else if (count == ms1->msa.size)
    {
        PRTL_FMT_DEBUG("ms1中表地址均没有被删除 count[%d]\n", ms.msa.size);
        memcpy(ms3, &ms, sizeof(OOP_MSPF_T));
        return ERR_INVALID;
    }
    else
    {
        PRTL_FMT_DEBUG("ms1中表地址部分被删除 count[%d]\n", ms.msa.size);
        memcpy(ms3, &ms, sizeof(OOP_MSPF_T));
    }

    return ERR_OK;
}

/**
*********************************************************************
* @name：      class11_invoke_add_group_meter
* @brief：     class11的ACTION服务
* @param[in] ：pOdaInfo        OAD信息
               pBpInfo        应用分帧断点信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
               outDataMax     输出报文缓冲区最大长度

* @param[out]：outData     输出报文
* @return    ：OOP_DAR_E
* @author    :
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E class11_invoke_add_group_meter(OAD_INFO_T* pOadInfo, APDU_INFO_T* pApduInfo, const uint8* inData, 
    uint16 inLen, RESULT_DATA_T* outData, uint16 outDataMax)
{
    uint16 i = 0;
    int    ret = ERR_OK;
    uint32 sOffset = 4;
    uint32 dOffset = 0;
    uint8  num = 0;
    uint16 infoNum = 0;

    OOP_DAR_E   dar = DATA_OTHER_REASON;

    OOP_GROUP_METER_T    data;
    OOP_GROUP_METER_T    dataGet;
    OOP_MSPF_T           ms;
    DATA_CONVERT_T       convert;

    memset(&data, 0, sizeof(data));
    memset(&dataGet, 0, sizeof(dataGet));
    memset(&ms, 0, sizeof(ms));
    memset(&convert, 0, sizeof(convert));

    /*类型不存在*/
    if ((pOadInfo == NULL) || (pApduInfo == NULL) || (inData == NULL) || (outData == NULL))
    {
        return DATA_READ_WRITE_DENIED;
    }

    convert.srcBuf = (void*)inData;
    convert.srcLen = inLen;
    convert.sOffset = &sOffset;

    convert.dstBuf = &data;
    convert.dstLen = sizeof(data);
    convert.dOffset = &dOffset;

    switch (pOadInfo->pTab->oad.value)
    {
    case 0x6e057F00:
    {
        if (inData[sOffset++] != DT_ARRAY)
        {
            return ERR_OPTION;
        }

        num = inData[sOffset++];
        if (num == 0x81)
        {
            num = inData[sOffset++];
        }
        else if (num == 0x82)
        {
            num = inData[sOffset++];
            num <<= 8;
            num += inData[sOffset++];
        }
        else if (num > 0x82 || num == 0x80)
        {
            return DATA_TYPE_UNMATCHED;
        }

        PRTL_FMT_DEBUG("增加群组档案，数量为%d\n", num);
        for (i = 0; i < num; i++)
        {
            dar = buf_to_data_group_meter(pOadInfo, &convert);
            if (dar != DATA_SUCCESS)
            {
                return dar;
            }
            else
            {
                dOffset = 0;
            }

            PRTL_BUF_DEBUG(&data.groupAdd.add[0], 6, "群组档案地址");

            infoNum = (uint8)(commfun_BcdToHex(data.groupAdd.add[4]) >> 8) * 1000 + 
                      (uint8)(commfun_BcdToHex(data.groupAdd.add[4]) & 0xff) * 100 +
                      (uint8)(commfun_BcdToHex(data.groupAdd.add[5]) >> 8) * 10   + 
                      (uint8)(commfun_BcdToHex(data.groupAdd.add[5]) & 0xff);

            PRTL_FMT_DEBUG("群组档案存入数据中心的信息点为%d\n", infoNum);

            ret = group_meter_read(pApduInfo->hUdp, infoNum, &dataGet);
            if (ret != ERR_OK)
            {
                ret = group_meter_write(pApduInfo->hUdp, &data);
                if (ret != ERR_OK)
                {
                    PRTL_FMT_DEBUG("群组档案存入数据中心失败 错误码[%d] 信息点[%d]\n", ret, infoNum);
                    return DATA_READ_WRITE_DENIED;
                }
            }
            else
            {
                PRTL_FMT_DEBUG("存在相同群组档案，需要合并\n");
                ret = trans_node_ms_f_add(&dataGet.ms, &data.ms, &ms);
                if (ret != ERR_OK)
                {
                    PRTL_FMT_DEBUG("ms集合的添加处理错误 错误码[%d]\n", ret);
                }
                else
                {
                    data.ms = ms;
                }

                ret = group_meter_write(pApduInfo->hUdp, &data);
                if (ret != ERR_OK)
                {
                    PRTL_FMT_DEBUG("群组档案存入数据中心失败 错误码[%d] 信息点[%d]\n", ret, infoNum);
                    return DATA_READ_WRITE_DENIED;
                }
            }
        }
    }
    break;
    default:
        break;
    }

    dar = DATA_SUCCESS;
    memcpy_r((uint8*)outData->pResultData, (uint8*)&pOadInfo->pTab->oad.value, 4); 
    outData->pResultData[4] = DATA_SUCCESS;
    outData->pResultData[5] = 0x00;                          //DATA:NULL
    outData->resultDataLen = 0x06;

    return dar;
}


/**
*********************************************************************
* @name：      class11_invoke_del_group_meter
* @brief：     class11的ACTION服务
* @param[in] ：pOdaInfo        OAD信息
               pBpInfo        应用分帧断点信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
               outDataMax     输出报文缓冲区最大长度

* @param[out]：outData     输出报文
* @return    ：OOP_DAR_E
* @author    :
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E class11_invoke_del_group_meter(OAD_INFO_T* pOadInfo, APDU_INFO_T* pApduInfo, const uint8* inData, 
    uint16 inLen, RESULT_DATA_T* outData, uint16 outDataMax)
{
    uint16 i = 0;
    int    ret = ERR_OK;
    uint32 sOffset = 4;
    uint32 dOffset = 0;
    uint8  num = 0;
    uint16 infoNum = 0;

    CLEAR_DATA_T RecordDel;
    OOP_DAR_E   dar = DATA_OTHER_REASON;

    OOP_GROUP_METER_T  data;
    OOP_GROUP_METER_T  dataGet;
    OOP_MSPF_T         ms;
    DATA_CONVERT_T     convert;

    memset(&data, 0, sizeof(data));
    memset(&dataGet, 0, sizeof(dataGet));
    memset(&ms, 0, sizeof(ms));
    memset(&convert, 0, sizeof(convert));
    memset(&RecordDel, 0, sizeof(RecordDel));

    /*类型不存在*/
    if ((pOadInfo == NULL) || (pApduInfo == NULL) || (inData == NULL) || (outData == NULL))
    {
        return DATA_READ_WRITE_DENIED;
    }

    convert.srcBuf = (void*)inData;
    convert.srcLen = inLen;
    convert.sOffset = &sOffset;

    convert.dstBuf = &data;
    convert.dstLen = sizeof(data);
    convert.dOffset = &dOffset;

    switch (pOadInfo->pTab->oad.value)
    {
    case 0x6e058000:
    {
        if (inData[sOffset++] != DT_ARRAY)
        {
            return ERR_OPTION;
        }

        num = inData[sOffset++];
        if (num == 0x81)
        {
            num = inData[sOffset++];
        }
        else if (num == 0x82)
        {
            num = inData[sOffset++];
            num <<= 8;
            num += inData[sOffset++];
        }
        else if (num > 0x82 || num == 0x80)
        {
            return DATA_TYPE_UNMATCHED;
        }

        PRTL_FMT_DEBUG("删除群组档案，数量为%d\n", num);
        for (i = 0; i < num; i++)
        {
            dar = buf_to_data_group_meter(pOadInfo, &convert);
            if (dar != DATA_SUCCESS)
            {
                return dar;
            }
            else
            {
                dOffset = 0;
            }

            PRTL_BUF_DEBUG(&data.groupAdd.add[0], 6, "要删除的群组档案地址");

            infoNum = (uint8)(commfun_BcdToHex(data.groupAdd.add[4]) >> 8) * 1000 + 
                      (uint8)(commfun_BcdToHex(data.groupAdd.add[4]) & 0xff) * 100 +
                      (uint8)(commfun_BcdToHex(data.groupAdd.add[5]) >> 8) * 10   + 
                      (uint8)(commfun_BcdToHex(data.groupAdd.add[5]) & 0xff);
            
            PRTL_FMT_DEBUG("要删除群组档案存入数据中心的信息点为%d\n", infoNum);

            ret = group_meter_read(pApduInfo->hUdp, infoNum, &dataGet);
            if (ret != ERR_OK)
            {
                PRTL_FMT_DEBUG("读群组档案失败，无需删除群组档案 错误码[%d] 信息点[%d]\n", ret, infoNum);
            }
            else
            {
                PRTL_FMT_DEBUG("存在相同群组档案，需要删除处理\n");
                ret = trans_node_ms_f_del(&dataGet.ms, &data.ms, &ms);
                if (ret != ERR_OK)
                {
                    PRTL_FMT_DEBUG("ms集合的添加处理错误 错误码[%d]\n", ret);
                }
                else
                {
                    data.ms = ms;
                }

                if (ms.size == 0)
                {
                    RecordDel.recordOAD.logicId = 0;
                    RecordDel.recordOAD.infoNum = infoNum;
                    RecordDel.recordOAD.optional = 0;
                    RecordDel.recordOAD.road.oadMain.value = 0;
                    RecordDel.recordOAD.road.oadCols.nNum = 1;
                    RecordDel.recordOAD.road.oadCols.oad[0].value = 0x6e050200;
                    RecordDel.bClassTag = FALSE;
                    
                    ret = db_clear_oad_data(pApduInfo->hUdp, &RecordDel);
                    if (ret != ERR_OK)
                    {
                        PRTL_FMT_DEBUG("数据删除失败 ret[%d]\n", ret);
                    }
                    
                    RecordDel.recordOAD.road.oadCols.oad[0].value = 0x6e05ffff;
                    ret = db_clear_oad_data(pApduInfo->hUdp, &RecordDel);
                    if (ret != ERR_OK)
                    {
                        PRTL_FMT_DEBUG("数据删除失败 ret[%d]\n", ret);
                    }
                }
                else
                {
                    ret = group_meter_write(pApduInfo->hUdp, &data);
                    if (ret != ERR_OK)
                    {
                        PRTL_FMT_DEBUG("群组档案存入数据中心失败 错误码[%d] 信息点[%d]\n", ret, infoNum);
                        return DATA_READ_WRITE_DENIED;
                    }
                }
            }
        }
    }
    break;
    default:
        break;
    }

    dar = DATA_SUCCESS;
    memcpy_r((uint8*)outData->pResultData, (uint8*)&pOadInfo->pTab->oad.value, 4); 
    outData->pResultData[4] = DATA_SUCCESS;
    outData->pResultData[5] = 0x00;                          //DATA:NULL
    outData->resultDataLen = 0x06;

    return dar;
}

/**
*********************************************************************
* @name：      class11_invoke_clr_group_meter
* @brief：     class11的ACTION服务
* @param[in] ：pOdaInfo        OAD信息
               pBpInfo        应用分帧断点信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
               outDataMax     输出报文缓冲区最大长度

* @param[out]：outData     输出报文
* @return    ：OOP_DAR_E
* @author    :
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E class11_invoke_clr_group_meter(OAD_INFO_T* pOadInfo, APDU_INFO_T* pApduInfo, const uint8* inData, 
    uint16 inLen, RESULT_DATA_T* outData, uint16 outDataMax)
{
    int    ret = ERR_OK;

    CLEAR_DATA_T RecordDel;
    OOP_DAR_E   dar = DATA_OTHER_REASON;

    memset(&RecordDel, 0, sizeof(RecordDel));

    /*类型不存在*/
    if ((pOadInfo == NULL) || (pApduInfo == NULL) || (inData == NULL) || (outData == NULL))
    {
        return DATA_READ_WRITE_DENIED;
    }

    switch (pOadInfo->pTab->oad.value)
    {
    case 0x6e058200:
    {
        PRTL_FMT_DEBUG("清空群组档案\n");

        RecordDel.recordOAD.logicId = 0;
        RecordDel.recordOAD.infoNum = 0xffff;
        RecordDel.recordOAD.optional = 0;
        RecordDel.recordOAD.road.oadMain.value = 0;
        RecordDel.recordOAD.road.oadCols.nNum = 1;
        RecordDel.recordOAD.road.oadCols.oad[0].value = 0x6e050200;
        RecordDel.bClassTag = FALSE;

        ret = db_clear_oad_data(pApduInfo->hUdp, &RecordDel);
        if (ret != ERR_OK)
        {
            PRTL_FMT_LOGCC("清空群组档案失败 ret[%d]\n", ret);
        }

        RecordDel.recordOAD.road.oadCols.oad[0].value = 0x6e05ffff;
        ret = db_clear_oad_data(pApduInfo->hUdp, &RecordDel);
        if (ret != ERR_OK)
        {
            PRTL_FMT_LOGCC("清空群组档案失败 ret[%d]\n", ret);
        }

        set_default_group_meter_fj(pApduInfo->hUdp, NULL);
    }
    break;
    default:
        break;
    }

    dar = DATA_SUCCESS;
    memcpy_r((uint8*)outData->pResultData, (uint8*)&pOadInfo->pTab->oad.value, 4); 
    outData->pResultData[4] = DATA_SUCCESS;
    outData->pResultData[5] = 0x00;                          //DATA:NULL
    outData->resultDataLen = 0x06;

    return dar;
}

/**
*********************************************************************
* @name：      class11_memcmp_group_meter_n
* @brief：     
* @param[in] ：group_meter 组群档案
               ms          要比较的档案
* @param[out]：
* @return    ：TRUE - 有相同的档案 FALSE -没有相同档案
* @author    :
* @Date      ：
*********************************************************************
*/
BOOL class11_memcmp_group_meter_n(OOP_MSPF_T *group_meter, OOP_MSPF_T *ms)
{
    BOOL ret = FALSE;

    int i, j;

    if (group_meter->choice == 3)
    {
        for (i = 0; i < group_meter->msa.size; i++)
        {
            if (ms->choice == 3)
            {
                for (j = 0; j < ms->msa.size; j++)
                {
                    if (memcmp(&group_meter->msa.add[i].add[0], &ms->msa.add[j].add[0], 6) == 0)
                    {
                        ret = TRUE;
                        break;
                    }
                }
            }
            else if (ms->choice == 6)
            {
                for (j = 0; j < ms->msra.size; j++)
                {
                    if ((memcmp(&ms->msra.region[j].start.add[0], &group_meter->msa.add[i].add[0], 6) < 0) && 
                        (memcmp(&ms->msra.region[j].end.add[0], &group_meter->msa.add[i].add[0], 6) > 0))
                    {
                        ret = TRUE;
                        break;
                    }
                }
 
            }
            else
            {
                PRTL_FMT_DEBUG("比较组群档案是否要删除，但待比较的档案的ms类型不支持 choice[%d]\n", ms->choice);
                ret = FALSE;
                break;
            }
        }
    }
    else if (group_meter->choice == 6)
    {
        for (i = 0; i < group_meter->msa.size; i++)
        {
            if (ms->choice == 3)
            {
                for (j = 0; j < ms->msa.size; j++)
                {
                    if ((memcmp(&group_meter->msra.region[i].start.add[0], &ms->msa.add[j].add[0], 6) < 0) &&
                        (memcmp(&group_meter->msra.region[i].end.add[0], &ms->msa.add[j].add[0], 6) > 0))
                    {
                        ret = TRUE;
                        break;
                    }
                }
            }
            else if (ms->choice == 6)
            {
                for (j = 0; j < ms->msa.size; j++)
                {
                    PRTL_FMT_DEBUG("比较组群档案是否要删除， 比较复杂，暂时不考虑，choice[%d]\n", ms->choice);
                    ret = FALSE;
                    break;
                }
            }
            else
            {
                PRTL_FMT_DEBUG("比较组群档案是否要删除，但待比较的档案的ms类型不支持 choice[%d]\n", ms->choice);
                ret = FALSE;
                break;
            }
        }
    }
    else
    {
        PRTL_FMT_DEBUG("比较组群档案是否要删除，但组群档案的ms类型不支持 choice[%d]\n", group_meter->choice);
        ret = FALSE;
    }

    return ret;
}

/**
*********************************************************************
* @name：      class11_memcmp_group_meter
* @brief：     
* @param[in] ：group_meter 组群档案
               ms          要比较的档案
* @param[out]：
* @return    ：TRUE - 有相同的档案 FALSE -没有相同档案
* @author    :
* @Date      ：
*********************************************************************
*/
BOOL class11_memcmp_group_meter(OOP_MSC_T *group_meter, OOP_MSP_T *ms)
{
    BOOL ret = FALSE;

    int i, j;

    if (group_meter->choice == 3)
    {
        for (i = 0; i < group_meter->msa.size; i++)
        {
            if (ms->choice == 3)
            {
                for (j = 0; j < ms->msa.size; j++)
                {
                    if (memcmp(&group_meter->msa.add[i].add[0], &ms->msa.add[j].add[0], 6) == 0)
                    {
                        ret = TRUE;
                        break;
                    }
                }
            }
            else if (ms->choice == 6)
            {
                for (j = 0; j < ms->msra.size; j++)
                {
                    if ((memcmp(&ms->msra.region[j].start.add[0], &group_meter->msa.add[i].add[0], 6) < 0) && 
                        (memcmp(&ms->msra.region[j].end.add[0], &group_meter->msa.add[i].add[0], 6) > 0))
                    {
                        ret = TRUE;
                        break;
                    }
                }
 
            }
            else
            {
                PRTL_FMT_DEBUG("比较组群档案是否要删除，但待比较的档案的ms类型不支持 choice[%d]\n", ms->choice);
                ret = FALSE;
                break;
            }
        }
    }
    else if (group_meter->choice == 6)
    {
        for (i = 0; i < group_meter->msa.size; i++)
        {
            if (ms->choice == 3)
            {
                for (j = 0; j < ms->msa.size; j++)
                {
                    if ((memcmp(&group_meter->msra.region[i].start.add[0], &ms->msa.add[j].add[0], 6) < 0) &&
                        (memcmp(&group_meter->msra.region[i].end.add[0], &ms->msa.add[j].add[0], 6) > 0))
                    {
                        ret = TRUE;
                        break;
                    }
                }
            }
            else if (ms->choice == 6)
            {
                for (j = 0; j < ms->msa.size; j++)
                {
                    PRTL_FMT_DEBUG("比较组群档案是否要删除， 比较复杂，暂时不考虑，choice[%d]\n", ms->choice);
                    ret = FALSE;
                    break;
                }
            }
            else
            {
                PRTL_FMT_DEBUG("比较组群档案是否要删除，但待比较的档案的ms类型不支持 choice[%d]\n", ms->choice);
                ret = FALSE;
                break;
            }
        }
    }
    else
    {
        PRTL_FMT_DEBUG("比较组群档案是否要删除，但组群档案的ms类型不支持 choice[%d]\n", group_meter->choice);
        ret = FALSE;
    }

    return ret;
}

/**
*********************************************************************
* @name：      class11_invoke_del_group_meter_by_ms
* @brief：     class11的ACTION服务
* @param[in] ：pOdaInfo        OAD信息
               pBpInfo        应用分帧断点信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
               outDataMax     输出报文缓冲区最大长度

* @param[out]：outData     输出报文
* @return    ：OOP_DAR_E
* @author    :
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E class11_invoke_del_group_meter_by_ms(OAD_INFO_T* pOadInfo, APDU_INFO_T* pApduInfo, const uint8* inData, 
    uint16 inLen, RESULT_DATA_T* outData, uint16 outDataMax)
{
    int    ret = ERR_OK;
    int    i = 0;
    uint32 offset = 0;
    CLEAR_DATA_T delData;
    NOMAL_OAD_T normal = {0};
    uint32      len = 0;
    
    OOP_GROUPMETER_NEW_T data;
    OOP_GROUPMETER_NEW_T data2;

    READ_RECORD_T  inReadRecord;
    uint32         handle = 0;
    uint32         recNum = 0;
    OOP_GROUP_METER_T dstData;
    OOP_MSPF_T     ms;
    OOP_MSPF_T     ms1;
    OOP_DAR_E      dar = DATA_SUCCESS;

    STEP_READ_RECORD_T stepReadRecord;
    RECORD_UNIT_T      stepRecordOut;

    uint8          *srcBuf = (uint8*)inData;
    uint32         srcLen  = inLen;
    uint32         sOffset = 4;

    memset(&ms, 0, sizeof(ms));
    memset(&ms1, 0, sizeof(ms1));
    memset(&dstData, 0, sizeof(dstData));
    memset(&inReadRecord, 0, sizeof(inReadRecord));
    memset(&data, 0, sizeof(data));
    memset(&data2, 0, sizeof(data2));
    memset(&delData, 0, sizeof(delData));
    memset(&stepReadRecord, 0, sizeof(STEP_READ_RECORD_T));
    memset(&stepRecordOut, 0, sizeof(RECORD_UNIT_T));

    switch (pOadInfo->pTab->oad.value)
    {
    case 0x6e058300:
    {
        dar = basic_buf_to_data(E_MS_F, srcBuf, srcLen, &sOffset, &ms, sizeof(ms));
        if (dar != DATA_SUCCESS)
        {
            return dar;
        }
        break;
    }
    default:
        break;
    }

    inReadRecord.recordOAD.infoNum = 0xffff;
    inReadRecord.cType = COL_NO_INFO;
    inReadRecord.cStart = 2;
    inReadRecord.cEnd = 0xffff;
    inReadRecord.sortType = UP_SORT;
    inReadRecord.recordOAD.optional = 0;
    inReadRecord.recordOAD.road.oadMain.value = 0;
    inReadRecord.recordOAD.road.oadCols.nNum = 0x01;
    inReadRecord.recordOAD.road.oadCols.oad[0].value = 0x6e050200;
    stepReadRecord.MAC.nNum = 0;

    ret = db_read_record_start(pApduInfo->hUdp, &inReadRecord, &handle, &recNum);
    if ((ret != ERR_OK) || (recNum == 0))
    {
        PRTL_FMT_DEBUG("读所有群组档案失败 ret = %d, infoNum = %d recNum = %d\n",
            ret, inReadRecord.recordOAD.infoNum, recNum);

        db_read_record_end(pApduInfo->hUdp, handle);

        return DATA_DATA_BLOCK_UNAVAILABLE;
    }

    for (i = 0; i < recNum; i++)
    {
        ret = db_read_record_step(pApduInfo->hUdp, handle, &stepReadRecord, &stepRecordOut);
        if (ret != ERR_OK)
        {
            PRTL_FMT_DEBUG("读所有群组档案单步失败err = %d!\n", ret);
            continue;
        }

        //OAD与返回数据
        if ((stepRecordOut.recordnum != 1)
            || (stepRecordOut.record[0].road.oadCols.oad[0].value != 0x6e050200)
            || (stepRecordOut.record[0].outDatalen != sizeof(OOP_GROUPMETER_NEW_T)))
        {
            PRTL_FMT_DEBUG("群组档案数据错误!\n");
            PRTL_FMT_DEBUG("群组档案数据 stepRecordOut.recordnum = %d\n", stepRecordOut.recordnum);
            PRTL_FMT_DEBUG("群组档案数据 stepRecordOut.record[0].road.oadCols.oad[0] = 0x%08x\n",
                stepRecordOut.record[0].road.oadCols.oad[0].value);
            PRTL_FMT_DEBUG("群组档案数据 stepRecordOut.record[0].outDatalen = %d\n",
                stepRecordOut.record[0].outDatalen);
            PRTL_FMT_DEBUG("群组档案数据 sizeof(OOP_GROUPMETER_NEW_T) = %d\n",
                sizeof(OOP_GROUPMETER_NEW_T));
            continue;
        }

        memcpy(&data, stepRecordOut.record[0].outDataBuf, stepRecordOut.record[0].outDatalen);
        PRTL_FMT_DEBUG("群组档案数据 stepRecordOut.infoNum = %d\n", stepRecordOut.infoNum);

        normal.oad.value = 0x6e05ffff;
        normal.infoNum = stepRecordOut.infoNum;
        ret = db_read_nomal(pApduInfo->hUdp, &normal, pOadInfo->pTab->dataLen, (uint8*)&data2, &len);
        if((ret != ERR_OK) || (len == 0))
        {
            PRTL_FMT_DEBUG("读普通数据失败. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                ret, normal.logicId, normal.infoNum, normal.oad.value);
        }

        ret = groupmeter_new_to_old(&dstData, &data, &data2);
        if (ret != ERR_OK)
        {
            PRTL_FMT_DEBUG("群组档案数据转换失败 ret = %d\n", ret);
            continue;
        }

        PRTL_FMT_DEBUG("群组档案[%d]，需要删除处理\n", normal.infoNum);
        ret = trans_node_ms_f_del(&dstData.ms, &ms, &ms1);
        if (ret != ERR_OK)
        {
            PRTL_FMT_DEBUG("ms集合的添加处理错误 错误码[%d]\n", ret);
        }
        else
        {
            dstData.ms = ms1;
        }

        //即使群组档案下的档案全部删除，也要保留群组档案，与方法128的删除逻辑不一样。
        /*if (ms1.size == 0)
        {
            delData.recordOAD.logicId = 0x00;
            delData.recordOAD.optional = 0;
            delData.recordOAD.road.oadMain.value = 0x00;
            delData.recordOAD.road.oadCols.nNum = 1;
            delData.recordOAD.road.oadCols.oad[0].value = 0x6e050200;
            delData.bClassTag = FALSE;

            delData.recordOAD.infoNum = stepRecordOut.infoNum;

            ret = db_clear_oad_data(pApduInfo->hUdp, &delData);
            if (ret != ERR_OK)
            {
                PRTL_FMT_DEBUG("删除序号群组档案数据%d err = %d\n", stepRecordOut.infoNum, ret);
                return DATA_READ_WRITE_DENIED;
            }

            delData.recordOAD.road.oadCols.oad[0].value = 0x6e05ffff;
            ret = db_clear_oad_data(pApduInfo->hUdp, &delData);
            if (ret != ERR_OK)
            {
                PRTL_FMT_DEBUG("删除序号群组档案数据%d err = %d\n", stepRecordOut.infoNum, ret);
                return DATA_READ_WRITE_DENIED;
            }
        }
        else*/
        {
            ret = group_meter_write(pApduInfo->hUdp, &dstData);
            if (ret != ERR_OK)
            {
                PRTL_FMT_DEBUG("群组档案存入数据中心失败 错误码[%d] 信息点[%d]\n", ret, stepRecordOut.infoNum);
                return DATA_READ_WRITE_DENIED;
            }
        }
    }

    memcpy(&outData->pResultData[offset], inData, sizeof(OOP_OAD_U));  //OMD
    offset += sizeof(OOP_OAD_U);
    outData->pResultData[offset++] = 0x00;                            //DAR type
    outData->pResultData[offset++] = 0x00;                            //DATA:NULL
    outData->resultDataLen = offset;

    db_read_record_end(pApduInfo->hUdp, handle);

    return dar;
}
#endif

#if DESC("全局透明方法任务读取", 1)
/**
*********************************************************************
* @brief：      读记录型方案任务
* @param[in]： inReadRecord  - 筛选需要的数据
               oopRcsd       - 列选择
               readRecNum    - 读记录数量，：0：读全部记录 1：读最近的一条记录
               maxBufLen     - 最大读取buf长度
* @param[out]：outRecordBuf  -  数据记录
* @return：     =0            - 成功
               <0            - 错误码
*********************************************************************
*/
int record_trans_task_read_by_task(DB_CLIENT hUdp, const READ_METER_T inReadMeter, RELY_INFO_T relyInfo, uint32 maxBufLen, RECORD_BUF_T *outRecordBuf)
{
    int       result = DATA_OTHER_REASON;
    uint32    offset = 0;
    uint32    i = 0;
    int32     ret = ERR_NORMAL;
    OOP_DAR_E nRet = DATA_OTHER_REASON;
    uint32    bufAllLen = 0;
    BP_INFO_T breakInfo = {0};                                         //断点信息
    BOOL      isBufFull = FALSE;
    OAD_INFO_T *pOadInfo = relyInfo.pOadInfo;

    /*清除断点信息, 需要注意应用分帧的未帧*/
    memset(&breakInfo, 0, sizeof(BP_INFO_T));
   
    DATA_CONVERT_T   convert;
    OOP_TRANS_TASK_T data = {0};                                        //每个任务方案信息
    uint32 sOffset = 0;
    uint32 dOffset = 0;
    uint32 oopMeterNum = 0;
    uint32 oopMeterNumOffset = 0;

    READ_RECORD_T      inReadRecord;
    STEP_READ_RECORD_T stepReadRecord;
    RECORD_UNIT_T      stepRecordOut;
    uint32             handle = 0;
    uint32             recNum = 0;

    MEMZERO(&convert, sizeof(convert));
    MEMZERO(&inReadRecord, sizeof(inReadRecord));

    oopMeterNumOffset = offset;                                         //档案数量偏移
    outRecordBuf->recordBuf[offset ++] = 0x00;                          //数量，先填0
    bufAllLen = offset;

    inReadRecord.recordOAD.logicId = 0;
    inReadRecord.recordOAD.infoNum = 0xffff;
    inReadRecord.cType = COL_NO_INFO;
    inReadRecord.cStart = inReadMeter.startInfoNum;
    inReadRecord.cEnd = inReadMeter.endInfoNum;
    inReadRecord.sortType = UP_SORT;
    inReadRecord.recordOAD.optional = 0;
    inReadRecord.recordOAD.road.oadMain.value = 0;
    inReadRecord.recordOAD.road.oadCols.nNum = 0x01;
    inReadRecord.recordOAD.road.oadCols.oad[0].value = pOadInfo->pTab->oad.value;

    stepReadRecord.MAC.nNum = 0;

    if(relyInfo.bpBreakInfo->appSlicingFlag == 1)
    {
        inReadRecord.cStart = relyInfo.bpBreakInfo->index;
    }

    ret = db_read_record_start(hUdp, &inReadRecord, &handle, &recNum);
    if ((ret != ERR_OK) || (recNum == 0))
    {
        PRTL_FMT_DEBUG("开始读出错. 错误码[%d], infoNum[%d] recNum[%d] oad[0x%08x]\n", 
            ret, inReadRecord.recordOAD.infoNum, recNum, pOadInfo->pTab->oad.value);

        db_read_record_end(hUdp, handle);
        return ret;
    }

    for (i = 0; i < recNum; i++)
    {
        ret = db_read_record_step(hUdp, handle, &stepReadRecord, &stepRecordOut);
        if(ret != ERR_OK)
        {
            PRTL_FMT_DEBUG("单步读出错 错误码[%d]\n", ret);
            continue;
        }

        //OAD与返回数据
        if ((stepRecordOut.recordnum != 1) 
        || (stepRecordOut.record[0].road.oadCols.oad[0].value != pOadInfo->pTab->oad.value)
        || (stepRecordOut.record[0].outDatalen != sizeof(data)))
        {
            PRTL_FMT_DEBUG("单步读到的数据错误!\n");
            PRTL_FMT_DEBUG("单步读数据 stepRecordOut.recordnum = %d\n", stepRecordOut.recordnum);
            PRTL_FMT_DEBUG("单步读数据 stepRecordOut.record[0].road.oadCols.oad[0] = 0x%08x\n", 
                stepRecordOut.record[0].road.oadCols.oad[0].value);
            PRTL_FMT_DEBUG("单步读数据 stepRecordOut.record[0].outDatalen = %d\n", 
                stepRecordOut.record[0].outDatalen);
            PRTL_FMT_DEBUG("单步读数据 sizeof(OOP_TRANS_TASK_T) = %d\n", 
                sizeof(data));
            continue;
        }

        memcpy(&data, stepRecordOut.record[0].outDataBuf, stepRecordOut.record[0].outDatalen);
        PRTL_FMT_LOGCC("找到任务方案序号为%d\n", data.taskid);

        //转换成报文数据
        convert.srcBuf = &data;
        convert.srcLen = sizeof(data);
        convert.sOffset = &sOffset;

        convert.dstBuf = &outRecordBuf->recordBuf[offset];
        convert.dstLen = maxBufLen - offset;
        convert.dOffset = &dOffset;

        //记录断点信息
        breakInfo.index = data.taskid;

        nRet = data_to_buf_trans_task(pOadInfo, &convert);  //入参
        if (nRet == DATA_SUCCESS)
        {
            bufAllLen += dOffset;
            if (bufAllLen <= maxBufLen)
            {
                memcpy(&outRecordBuf->recordBuf[offset], convert.dstBuf, dOffset);
                offset += dOffset;
                outRecordBuf->bufLen = offset;
                PRTL_BUF_LOGCC(convert.dstBuf, dOffset, "第%d个任务方案报文:", i);
                oopMeterNum ++;
                if (bufAllLen + dOffset >= maxBufLen) //虽然本次没有超最大长度，但下次会
                {
                    isBufFull = TRUE;
                    PRTL_FMT_DEBUG("虽然本次没有超最大长度，但下次会 isBufFull = [%d]\n", isBufFull);
                }
                dOffset = 0;
                sOffset = 0;
            }
            else
            {
                dOffset = 0;
                sOffset = 0;
                isBufFull = TRUE;
                PRTL_FMT_DEBUG("达到最大报文长度 isBufFull = [%d]\n", isBufFull);
            }
        }
        else
        {
            continue;
        }

        if ((isBufFull) && (offset > 0))
        {
            break;
        }
    }

    db_read_record_end(hUdp, handle);

    if (!isBufFull)
    {
        if (relyInfo.bpBreakInfo->appSlicingFlag == 0x01)
        {
            breakInfo.appSlicingFlag = 0x01;
            breakInfo.isEnd = 0x01;                           //末尾帧
            breakInfo.seq = relyInfo.bpBreakInfo->seq + 1;    //序号加1
            breakInfo.objPre = 0x01;                          //对象属性
        }
        else
        {
            /*清除断点信息*/
            memset(&breakInfo, 0, sizeof(BP_INFO_T));
        }
    }
    else
    {
        breakInfo.reqType = 0x03;
        breakInfo.appSlicingFlag = 0x01;
        breakInfo.isEnd = 0x00;                              //不是末尾帧
        breakInfo.objPre = 0x01;                             //对象属性
        if (relyInfo.bpBreakInfo->appSlicingFlag == 0x00)
        {
            breakInfo.seq = BRK_SEQ_START;                   //首次序号
        }
        else
        {
            breakInfo.seq = relyInfo.bpBreakInfo->seq + 1;  //序号加1
        }
    }

    //刷新断点信息
    memcpy(&outRecordBuf->breakInfo, &breakInfo, sizeof(BP_INFO_T));

    PRTL_BUF_LOGCC(outRecordBuf->recordBuf, offset, "整个报文:");
    PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->appSlicingFlag = %d\n", outRecordBuf->breakInfo.appSlicingFlag);
    PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->index = %d\n", outRecordBuf->breakInfo.index);
    PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->isEnd = %d\n", outRecordBuf->breakInfo.isEnd);
    PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->objPre = %d\n", outRecordBuf->breakInfo.objPre);
    PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->seq = %d\n", outRecordBuf->breakInfo.seq);
    PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->reqType = %d\n", outRecordBuf->breakInfo.reqType);

    result = DATA_SUCCESS;
    outRecordBuf->recordBuf[oopMeterNumOffset] = oopMeterNum;   //重新记档案数量
    outRecordBuf->bufLen = offset;

    return result;
}

/**
*********************************************************************
* @brief：      读记录型方案任务
* @param[in]： inReadRecord  - 筛选需要的数据
               oopRcsd       - 列选择
               readRecNum    - 读记录数量，：0：读全部记录 1：读最近的一条记录
               maxBufLen     - 最大读取buf长度
* @param[out]：outRecordBuf  -  数据记录
* @return：     =0            - 成功
               <0            - 错误码
*********************************************************************
*/
int record_trans_task_read_by_port(DB_CLIENT hUdp, const READ_METER_T inReadMeter, RELY_INFO_T relyInfo, uint32 maxBufLen, RECORD_BUF_T *outRecordBuf)
{
    int       result = DATA_OTHER_REASON;
    uint32    offset = 0;
    uint32    i = 0;
    int32     ret = ERR_NORMAL;
    OOP_DAR_E nRet = DATA_OTHER_REASON;
    uint32    bufAllLen = 0;
    BP_INFO_T breakInfo = {0};                                         //断点信息
    BOOL      isBufFull = FALSE;
    OAD_INFO_T *pOadInfo = relyInfo.pOadInfo;

    /*清除断点信息, 需要注意应用分帧的未帧*/
    memset(&breakInfo, 0, sizeof(BP_INFO_T));
   
    DATA_CONVERT_T   convert;
    OOP_TRANS_TASK_T data = {0};                                        //每个任务方案信息
    uint32 sOffset = 0;
    uint32 dOffset = 0;
    uint32 oopMeterNum = 0;
    uint32 oopMeterNumOffset = 0;

    READ_RECORD_T      inReadRecord;
    STEP_READ_RECORD_T stepReadRecord;
    RECORD_UNIT_T      stepRecordOut;
    uint32             handle = 0;
    uint32             recNum = 0;

    MEMZERO(&convert, sizeof(convert));
    MEMZERO(&inReadRecord, sizeof(inReadRecord));

    oopMeterNumOffset = offset;                                         //档案数量偏移
    outRecordBuf->recordBuf[offset ++] = 0x00;                          //数量，先填0
    bufAllLen = offset;

    inReadRecord.recordOAD.logicId = 0;
    inReadRecord.recordOAD.infoNum = 0xffff;
    inReadRecord.cType = COL_NO_INFO;
    inReadRecord.cStart = 0;
    inReadRecord.cEnd = 0xffff;
    inReadRecord.sortType = UP_SORT;
    inReadRecord.recordOAD.optional = 0;
    inReadRecord.recordOAD.road.oadMain.value = 0;
    inReadRecord.recordOAD.road.oadCols.nNum = 0x01;
    inReadRecord.recordOAD.road.oadCols.oad[0].value = pOadInfo->pTab->oad.value;

    stepReadRecord.MAC.nNum = 0;

    if(relyInfo.bpBreakInfo->appSlicingFlag == 1)
    {
        inReadRecord.cStart = relyInfo.bpBreakInfo->index;
    }

    ret = db_read_record_start(hUdp, &inReadRecord, &handle, &recNum);
    if ((ret != ERR_OK) || (recNum == 0))
    {
        PRTL_FMT_DEBUG("开始读出错 错误码[%d], infoNum[%d] recNum[%d] oad[0x%08x]\n", 
            ret, inReadRecord.recordOAD.infoNum, recNum, pOadInfo->pTab->oad.value);

        db_read_record_end(hUdp, handle);
        return ret;
    }
    else
    {
        PRTL_FMT_DEBUG("db_read_record_start err! ret = %d, infoNum = %d recNum = %d\n", 
            ret, inReadRecord.recordOAD.infoNum, recNum);
    }

    for (i = 0; i < recNum; i++)
    {
        ret = db_read_record_step(hUdp, handle, &stepReadRecord, &stepRecordOut);
        if(ret != ERR_OK)
        {
            PRTL_FMT_DEBUG("db_read_record_step err = %d!\n", ret);
            continue;
        }

        //OAD与返回数据
        if ((stepRecordOut.recordnum != 1) 
        || (stepRecordOut.record[0].road.oadCols.oad[0].value != pOadInfo->pTab->oad.value)
        || (stepRecordOut.record[0].outDatalen != sizeof(data)))
        {
            PRTL_FMT_DEBUG("单步读到的数据错误!\n");
            PRTL_FMT_DEBUG("单步读数据 stepRecordOut.recordnum = %d\n", stepRecordOut.recordnum);
            PRTL_FMT_DEBUG("单步读数据 stepRecordOut.record[0].road.oadCols.oad[0] = 0x%08x\n", 
                stepRecordOut.record[0].road.oadCols.oad[0].value);
            PRTL_FMT_DEBUG("单步读数据 stepRecordOut.record[0].outDatalen = %d\n", 
                stepRecordOut.record[0].outDatalen);
            PRTL_FMT_DEBUG("单步读数据 sizeof(OOP_TRANS_TASK_T) = %d\n", 
                sizeof(data));
            continue;
        }

        memcpy(&data, stepRecordOut.record[0].outDataBuf, stepRecordOut.record[0].outDatalen);

        if (data.oad.value != inReadMeter.startInfoNum)
        {
            continue;
        }
        
        PRTL_FMT_LOGCC("找到任务方案序号为%d\n", data.taskid);

        //转换成报文数据
        convert.srcBuf = &data;
        convert.srcLen = sizeof(data);
        convert.sOffset = &sOffset;

        convert.dstBuf = &outRecordBuf->recordBuf[offset];
        convert.dstLen = maxBufLen - offset;
        convert.dOffset = &dOffset;

        //记录断点信息
        breakInfo.index = data.taskid;

        nRet = data_to_buf_trans_task(pOadInfo, &convert);  //入参
        if (nRet == DATA_SUCCESS)
        {
            bufAllLen += dOffset;
            if (bufAllLen <= maxBufLen)
            {
                memcpy(&outRecordBuf->recordBuf[offset], convert.dstBuf, dOffset);
                offset += dOffset;
                outRecordBuf->bufLen = offset;
                PRTL_BUF_LOGCC(convert.dstBuf, dOffset, "第%d个任务方案报文:", i);
                oopMeterNum ++;
                if (bufAllLen + dOffset >= maxBufLen) //虽然本次没有超最大长度，但下次会
                {
                    isBufFull = TRUE;
                    PRTL_FMT_DEBUG("虽然本次没有超最大长度，但下次会 isBufFull = [%d]\n", isBufFull);
                }
                dOffset = 0;
                sOffset = 0;
            }
            else
            {
                dOffset = 0;
                sOffset = 0;
                isBufFull = TRUE;
                PRTL_FMT_DEBUG("达到最大报文长度 isBufFull = [%d]\n", isBufFull);
            }
        }
        else
        {
            continue;
        }

        if ((isBufFull) && (offset > 0))
        {
            break;
        }
    }

    db_read_record_end(hUdp, handle);

    if (!isBufFull)
    {
        if (relyInfo.bpBreakInfo->appSlicingFlag == 0x01)
        {
            breakInfo.appSlicingFlag = 0x01;
            breakInfo.isEnd = 0x01;                           //末尾帧
            breakInfo.seq = relyInfo.bpBreakInfo->seq + 1;    //序号加1
            breakInfo.objPre = 0x01;                          //对象属性
        }
        else
        {
            /*清除断点信息*/
            memset(&breakInfo, 0, sizeof(BP_INFO_T));
        }
    }
    else
    {
        breakInfo.reqType = 0x03;
        breakInfo.appSlicingFlag = 0x01;
        breakInfo.isEnd = 0x00;                              //不是末尾帧
        breakInfo.objPre = 0x01;                             //对象属性
        if (relyInfo.bpBreakInfo->appSlicingFlag == 0x00)
        {
            breakInfo.seq = BRK_SEQ_START;                   //首次序号
        }
        else
        {
            breakInfo.seq = relyInfo.bpBreakInfo->seq + 1;  //序号加1
        }
    }

    //刷新断点信息
    memcpy(&outRecordBuf->breakInfo, &breakInfo, sizeof(BP_INFO_T));

    PRTL_BUF_LOGCC(outRecordBuf->recordBuf, offset, "整个报文:");
    PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->appSlicingFlag = %d\n", outRecordBuf->breakInfo.appSlicingFlag);
    PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->index = %d\n", outRecordBuf->breakInfo.index);
    PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->isEnd = %d\n", outRecordBuf->breakInfo.isEnd);
    PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->objPre = %d\n", outRecordBuf->breakInfo.objPre);
    PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->seq = %d\n", outRecordBuf->breakInfo.seq);
    PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->reqType = %d\n", outRecordBuf->breakInfo.reqType);

    result = DATA_SUCCESS;
    outRecordBuf->recordBuf[oopMeterNumOffset] = oopMeterNum;   //重新记档案数量
    outRecordBuf->bufLen = offset;

    return result;
}
/**
*********************************************************************
* @brief：      读记录型方案任务
* @param[in]： inReadRecord  - 筛选需要的数据
               oopRcsd       - 列选择
               readRecNum    - 读记录数量，：0：读全部记录 1：读最近的一条记录
               maxBufLen     - 最大读取buf长度
* @param[out]：outRecordBuf  -  数据记录
* @return：     =0            - 成功
               <0            - 错误码
*********************************************************************
*/
int record_trans_task_read(DB_CLIENT hUdp, const READ_METER_T inReadMeter, RELY_INFO_T relyInfo, uint32 maxBufLen, 
RECORD_BUF_T *outRecordBuf)
{
    if ((relyInfo.oopRsd->choice == 1) && (relyInfo.oopRsd->sel1.value.type == DT_OAD))
    {
        return record_trans_task_read_by_port(hUdp, inReadMeter, relyInfo, maxBufLen, outRecordBuf);
    }
    else
    {
        return record_trans_task_read_by_task(hUdp, inReadMeter, relyInfo, maxBufLen, outRecordBuf);
    }

    return ERR_OK;
}

/**
*********************************************************************
* @name：      class11_invoke_get_oop_meter_record
* @brief：     class11的GET服务
* @param[in] ：pOdaInfo        OAD信息
               pBpInfo        应用分帧断点信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
               outDataMax     输出报文缓冲区最大长度

* @param[out]：outData     输出报文
* @return    ：OOP_DAR_E
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E class11_invoke_get_transparent_task_all_rec(OAD_INFO_T *pOdaInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    uint16 i = 0;
    uint16 j = 0;
    OOP_DAR_E  nRet = DATA_OTHER_REASON;
    uint32 offset = 4;
    uint16 csdStart = 0, csdLen = 0;
    uint16 outOffset = 0;
    uint16 inDataLenTmp = InDataLen;

    OOP_PRSD_T oopRsd;
    OOP_RCSD_T oopRcsd;
    uint16     headLen = 0;
    READ_METER_T inReadMeter;
    RELY_INFO_T  relyInfo;
    RECORD_BUF_T outRecordBuf;

    /* 清0处理 */
    MEMZERO(&oopRsd, sizeof(oopRsd));
    MEMZERO(&oopRcsd, sizeof(oopRcsd));
    MEMZERO(&inReadMeter, sizeof(inReadMeter));
    MEMZERO(&relyInfo, sizeof(relyInfo));
    MEMZERO(&outRecordBuf, sizeof(outRecordBuf));

    outRecordBuf.recordBuf = (uint8 *)malloc(pApduInfo->connectionInfo.ApplSplitLen);
    memset(outRecordBuf.recordBuf, 0xee, pApduInfo->connectionInfo.ApplSplitLen);

    /*处理RSD*/
    nRet = apdu_rsd_fromfmt698(inData, inDataLenTmp, &offset, &oopRsd, sizeof(oopRsd));
    PRTL_FMT_LOGCC("oopRsd.choice = %d\n", oopRsd.choice);
    PRTL_FMT_LOGCC("oopRsd.sel1.oad.value = 0x%08x\n", oopRsd.sel1.oad.value);
    PRTL_FMT_LOGCC("oopRsd.sel1.value.wVal = %d\n", oopRsd.sel1.value.wVal);
    PRTL_FMT_LOGCC("offset = %d\n", offset);
    if (nRet != DATA_SUCCESS)
    {
        PRTL_FMT_TRACE("RSD解析错误\n");

        /*应对电科院协议一致性测试*/
        uint16 testOffset = 4;
        uint16 inCsdOff = 0, inCsdLen = 0;
        apdu_next_dpos_get(inData, &testOffset, InDataLen, DT_RSD, TRUE);
        inCsdOff = testOffset;
        apdu_next_dpos_get(inData, &testOffset, InDataLen, DT_RCSD, TRUE);
        inCsdLen = testOffset - inCsdOff;
        memcpy(outData->pResultData, inData, 4);
        memcpy(outData->pResultData + 4, inData + inCsdOff, inCsdLen);
        outData->resultDataLen = 4 + inCsdLen;

        outData->pResultData[outData->resultDataLen++] = 1;                   //后面优化下，outData->resultDataLen与下面统一
        if (apdu_rcsd_fromfmt698((uint8 *)inData, InDataLen, &offset, &oopRcsd, sizeof(oopRcsd)) != DATA_SUCCESS)
        {
            outData->pResultData[outData->resultDataLen++] = 0;

        }
        else
        {
            outData->pResultData[outData->resultDataLen++] = 1;
            for (i = 0; i < oopRcsd.nNum; i++)
            {
                outData->pResultData[outData->resultDataLen++] = NULL_DATA;
            }
        }

        free(outRecordBuf.recordBuf);
        outRecordBuf.recordBuf = NULL;
        return DATA_SUCCESS;
    }
    else
    {
        inDataLenTmp -= offset;  //除去RSD和OAD后的长度
    }

    /*处理RCSD*/
    csdStart = offset;
    if (apdu_rcsd_fromfmt698(inData, InDataLen, &offset, &oopRcsd, sizeof(oopRcsd)) != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("CSD解析错误!\n");
        free(outRecordBuf.recordBuf);
        outRecordBuf.recordBuf = NULL;
        return DATA_TYPE_UNMATCHED;
    }

    /*填写OAD+RCSD+记录结果1 并记下偏移headLen*/
    outData->resultDataLen = 0;
    csdLen = offset-csdStart;
    memcpy(outData->pResultData, inData, 4);
    outOffset = 4;

    if (csdLen == 1)
    {
        //如果rcsd为空，填充OAD6e120200
        outData->pResultData[outOffset ++] = 0x01;
        outData->pResultData[outOffset ++] = 0x00;
        outData->pResultData[outOffset ++] = 0x6e;
        outData->pResultData[outOffset ++] = 0x12;
        outData->pResultData[outOffset ++] = 0x02;
        outData->pResultData[outOffset ++] = 0x00;
    }
    else
    {
        memcpy(&outData->pResultData[outOffset], &inData[csdStart], csdLen);
        outOffset += csdLen;
    }
    outData->pResultData[outOffset++] = 1;
    outData->resultDataLen += outOffset;
    headLen = outOffset;
    PRTL_BUF_LOGCC(&outData->pResultData[0], headLen, "填写OAD+RCSD+记录结果1 并记下偏移headLen head:");

    /*记录型档案接口提供输入参数*/
    if (general_param_get(&oopRsd, &oopRcsd, pApduInfo, &inReadMeter, &relyInfo) != ERR_OK)
    {
        free(outRecordBuf.recordBuf);
        outRecordBuf.recordBuf = NULL;
        return DATA_READ_WRITE_DENIED;
    }

    /* 依赖信息中存下OAD列表的指针 */
    relyInfo.pOadInfo = pOdaInfo;

    PRTL_FMT_LOGCC("****************************************************\n");
    PRTL_FMT_LOGCC("inReadMeter.startInfoNum = %d\n", inReadMeter.startInfoNum);
    PRTL_FMT_LOGCC("inReadMeter.endInfoNum = %d\n", inReadMeter.endInfoNum);
    PRTL_FMT_LOGCC("relyInfo.oopRcsd.nNum = %d\n", relyInfo.oopRcsd.nNum);
    for(i = 0; i < relyInfo.oopRcsd.nNum; i++)
    {
        if (relyInfo.oopRcsd.cols[i].choice == 0)
        {
            PRTL_FMT_LOGCC("choice = %d relyInfo.oopRcsd.cols[%d].oad.value = 0x%08x\n", i, relyInfo.oopRcsd.cols[i].choice, 
                                                                                      relyInfo.oopRcsd.cols[i].oad.value);
        }
        else if(relyInfo.oopRcsd.cols[i].choice == 1)
        {
            PRTL_FMT_LOGCC("choice = %d relyInfo.oopRcsd.cols[%d].road.oadMain.value = 0x%08x\n", i, relyInfo.oopRcsd.cols[i].choice, 
                                                                                              relyInfo.oopRcsd.cols[i].road.oadMain.value);
            for (j = 0; j < relyInfo.oopRcsd.cols[i].road.oadCols.nNum; j++)
            {
                PRTL_FMT_LOGCC("choice = %d relyInfo.oopRcsd.cols[%d].road.oadCols.oad[%d].value = 0x%08x\n", i, j, relyInfo.oopRcsd.cols[i].choice, 
                                                                                                          relyInfo.oopRcsd.cols[i].road.oadCols.oad[j].value);
            }
        }
    }
    PRTL_FMT_LOGCC("****************************************************\n");

    /*读记录档案接口，可以读出apdu数据域，可返回断点信息*/
    if (record_trans_task_read(pApduInfo->hUdp, inReadMeter, relyInfo, pApduInfo->connectionInfo.ApplSplitLen - headLen - cal_excludeLen(pApduInfo->connectionInfo), &outRecordBuf))
    {
        nRet = DATA_READ_WRITE_DENIED;
    }
    else
    {
        memcpy(&outData->pResultData[headLen], outRecordBuf.recordBuf, outRecordBuf.bufLen);
        outData->resultDataLen += outRecordBuf.bufLen;

        if (outRecordBuf.breakInfo.appSlicingFlag == 1)
        {
            /*记下断点信息*/
            PRTL_FMT_LOGCC("记下断点信息\n");
            outRecordBuf.breakInfo.objPre = 0x02;   //对象属性，记录型数据
            memcpy(pApduInfo->bpBreakInfo, &outRecordBuf.breakInfo, sizeof(BP_INFO_T));
        }
        else
        {
            /*清除断点信息*/
            PRTL_FMT_LOGCC("清除断点信息\n");
            memset(pApduInfo->bpBreakInfo, 0, sizeof(BP_INFO_T));
        }
        
        nRet = DATA_SUCCESS;
    }
    
    free(outRecordBuf.recordBuf);
    outRecordBuf.recordBuf = NULL;
    return DATA_SUCCESS;
}

/**
*********************************************************************
* @name：      class11_invoke_get_transparent_task_all_nor
* @brief：     class11的GET服务
* @param[in] ：pOdaInfo        OAD信息
               pBpInfo        应用分帧断点信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
               outDataMax     输出报文缓冲区最大长度

* @param[out]：outData     输出报文
* @return    ：OOP_DAR_E
* @author    :
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E class11_invoke_get_transparent_task_all_nor(OAD_INFO_T* pOadInfo, APDU_INFO_T* pApduInfo, const uint8* inData, uint16 inLen, RESULT_DATA_T* outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    DATA_CONVERT_T convert;
    uint32 sOffset = 0;
    uint32 dOffset = 0;
    uint32 offset = 0;
    uint32             i = 0;
    READ_RECORD_T      inReadRecord;
    uint32             handle = 0;
    uint32             recNum = 0;
    STEP_READ_RECORD_T stepReadRecord;
    RECORD_UNIT_T      stepRecordOut;
    OOP_TRANS_TASK_T   data;
    uint32    realNum = 0;    //数组由于分帧导致的每帧实际成员数
    uint32    numOffset = 0;  //数组成员数在报文中的位置
    uint32    bufAllLen = 0;
    BP_INFO_T breakInfo = { 0 };                                                //断点信息
    uint32    maxBufLen = 0;
    BOOLEAN   isBufFull = FALSE;

    maxBufLen = pApduInfo->connectionInfo.ApplSplitLen - cal_excludeLen(pApduInfo->connectionInfo);
    memset(&breakInfo, 0, sizeof(BP_INFO_T));
    memset(&data, 0, sizeof(data));
    MEMZERO(&convert, sizeof(convert));

    memset(&inReadRecord, 0, sizeof(READ_RECORD_T));
    memset(&stepReadRecord, 0, sizeof(STEP_READ_RECORD_T));
    memset(&stepRecordOut, 0, sizeof(RECORD_UNIT_T));
    //读出数据
    PRTL_FMT_DEBUG("read trans task start\n");
    inReadRecord.recordOAD.logicId = 0;

    if (pOadInfo->nIndex == 0)
    {
        inReadRecord.recordOAD.infoNum = 0xffff;
}
    else
    {
        inReadRecord.recordOAD.infoNum = pOadInfo->nIndex;
    }

    inReadRecord.cType = COL_NO_INFO;
    inReadRecord.cStart = 0;
    inReadRecord.cEnd = 0xffff;
    inReadRecord.sortType = UP_SORT;
    inReadRecord.recordOAD.optional = 0;
    inReadRecord.recordOAD.road.oadMain.value = 0;
    inReadRecord.recordOAD.road.oadCols.nNum = 0x01;
    inReadRecord.recordOAD.road.oadCols.oad[0].value = pOadInfo->pTab->oad.value;
    stepReadRecord.MAC.nNum = 0;
    if (pApduInfo->bpBreakInfo->appSlicingFlag == 1)
    {
        inReadRecord.cStart = pApduInfo->bpBreakInfo->tmStartSeg;
    }
    memcpy(&outData->pResultData[offset], inData, sizeof(OOP_OAD_U));   //拷贝OAD
    offset += sizeof(OOP_OAD_U);
    outData->pResultData[offset++] = 0x01;                              //结果
    if (pOadInfo->nIndex == 0)
    {
        outData->pResultData[offset++] = DT_ARRAY;                         //类型
        numOffset = offset;                                                //数量偏移
        outData->pResultData[offset++] = 0x00;                             //数量，先填0
    }
    bufAllLen = offset;
    ret = db_read_record_start(pApduInfo->hUdp, &inReadRecord, &handle, &recNum);
    if ((ret != ERR_OK) || (recNum == 0))
    {
        PRTL_FMT_DEBUG("任务方案开始读失败 ret = %d, infoNum = %d recNum = %d\n",
            ret, inReadRecord.recordOAD.infoNum, recNum);
    }
    else
    {
        PRTL_FMT_DEBUG("任务方案开始读成功 ret = %d, infoNum = %d recNum = %d\n",
            ret, inReadRecord.recordOAD.infoNum, recNum);

        for (i = 0; i < recNum; i++)
        {
            ret = db_read_record_step(pApduInfo->hUdp, handle, &stepReadRecord, &stepRecordOut);
            if (ret != ERR_OK)
            {
                PRTL_FMT_DEBUG("任务方案单步读成功 db_read_record_step err = %d!\n", ret);
                continue;
                //数据中心出错返回NULL
            }

            //OAD与返回数据
            if ((stepRecordOut.recordnum != 1)
                || (stepRecordOut.record[0].road.oadCols.oad[0].value != pOadInfo->pTab->oad.value)
                || (stepRecordOut.record[0].outDatalen != sizeof(OOP_TRANS_TASK_T)))
            {
                PRTL_FMT_DEBUG("读任务方案失败!\n");
                PRTL_FMT_DEBUG("读任务方案 stepRecordOut.recordnum = %d\n", stepRecordOut.recordnum);
                PRTL_FMT_DEBUG("读任务方案 stepRecordOut.record[0].road.oadCols.oad[0] = 0x%08x\n",
                    stepRecordOut.record[0].road.oadCols.oad[0].value);
                PRTL_FMT_DEBUG("读任务方案 stepRecordOut.record[0].outDatalen = %d\n",
                    stepRecordOut.record[0].outDatalen);
                PRTL_FMT_DEBUG("读任务方案 sizeof(OOP_TRANS_TASK_T) = %d\n",
                    sizeof(OOP_TRANS_TASK_T));
                continue;
            }

            memcpy(&data, stepRecordOut.record[0].outDataBuf, stepRecordOut.record[0].outDatalen);
            PRTL_FMT_DEBUG("stepRecordOut.infoNum = %d\n", stepRecordOut.infoNum);

            //转换成报文数据
            convert.srcBuf = &data;
            convert.srcLen = sizeof(data);
            convert.sOffset = &sOffset;

            convert.dstBuf = &outData->pResultData[offset];
            convert.dstLen = outDataMax - offset;
            convert.dOffset = &dOffset;

            breakInfo.tmStartSeg = stepRecordOut.infoNum;
            dar = data_to_buf_trans_task(pOadInfo, &convert);
            if (dar == DATA_SUCCESS)
            {
                bufAllLen += dOffset;
                if (bufAllLen <= maxBufLen)
                {
                    memcpy(&outData->pResultData[offset], convert.dstBuf, dOffset);
                    offset += dOffset;
                    outData->resultDataLen = offset;
                    PRTL_BUF_DEBUG(convert.dstBuf, dOffset, "第%d个任务方案:", i);
                    realNum++;
                    dOffset = 0;
                    sOffset = 0;
                }
                else
                {
                    dOffset = 0;
                    sOffset = 0;
                    isBufFull = TRUE;
                }
            }
            else
            {
                continue;
            }
            if ((isBufFull) && (offset > 0))
            {
                break;
            }
        }
    }
    db_read_record_end(pApduInfo->hUdp, handle);
    PRTL_FMT_DEBUG("read trans task end.\n");
    if (!isBufFull)
    {
        if (pApduInfo->bpBreakInfo->appSlicingFlag == 0x01)
        {
            breakInfo.appSlicingFlag = 0x01;
            breakInfo.isEnd = 0x01;                           //末尾帧
            breakInfo.seq = pApduInfo->bpBreakInfo->seq + 1;  //序号加1
            breakInfo.objPre = 0x01;                          //对象属性
        }
        else
        {
            memset(&breakInfo, 0, sizeof(BP_INFO_T));
        }
    }
    else
    {
        breakInfo.reqType = pApduInfo->reqType;
        breakInfo.appSlicingFlag = 0x01;
        breakInfo.isEnd = 0x00;                              //不是末尾帧
        breakInfo.objPre = 0x01;                             //对象属性
        if (pApduInfo->bpBreakInfo->appSlicingFlag == 0x00)
        {
            breakInfo.seq = BRK_SEQ_START;                   //首次序号
        }
        else
        {
            breakInfo.seq = pApduInfo->bpBreakInfo->seq + 1;  //序号加1
        }
    }
    memcpy(pApduInfo->bpBreakInfo, &breakInfo, sizeof(BP_INFO_T));

    if (pOadInfo->nIndex == 0)
    {
        outData->pResultData[numOffset] = realNum;   //重新记数量
    }

    outData->resultDataLen = offset;

    return dar;
}

/**
*********************************************************************
* @name：      class11_invoke_get_transparent_task_all
* @brief：     class11的GET服务
* @param[in] ：pOdaInfo        OAD信息
               pBpInfo        应用分帧断点信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
               outDataMax     输出报文缓冲区最大长度

* @param[out]：outData     输出报文
* @return    ：OOP_DAR_E
* @author    :
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E class11_invoke_get_transparent_task_all(OAD_INFO_T* pOadInfo, APDU_INFO_T* pApduInfo, const uint8* inData, 
    uint16 inLen, RESULT_DATA_T* outData, uint16 outDataMax)
{
    //如果是应用分帧状态，将apdu[1]修改成起始帧方式
    if (pApduInfo->bpBreakInfo->appSlicingFlag == 1)
    {
        pApduInfo->reqType = pApduInfo->bpBreakInfo->reqType;
    }

    switch (pApduInfo->reqType)
    {
    case 0x01:
    case 0x02:
        return class11_invoke_get_transparent_task_all_nor(pOadInfo, pApduInfo, inData, inLen, outData, outDataMax);
        break;
    case 0x03:
    case 0x04:
        return class11_invoke_get_transparent_task_all_rec(pOadInfo, pApduInfo, inData, inLen, outData, outDataMax);
        break;
    default:
        break;
    }

    return DATA_OBJECT_UNDEFINED;
}

/**
*********************************************************************
* @brief：      销毁链表
* @param[in]： 集合链表头
* @return：     集合链表头
*********************************************************************
*/
LINK_CONFIG_STAT_T* link_config_stat_destroy(LINK_CONFIG_STAT_T* pConfigHead)
{
    LINK_CONFIG_STAT_T *tempConfig = NULL;
    LINK_CONFIG_STAT_T *pConfig = pConfigHead;
    while (pConfig)
    {
        tempConfig = pConfig->next;
        free(pConfig);
        pConfig = tempConfig;
    }
    pConfigHead = NULL;
    return pConfigHead;
}

/**
*********************************************************************
* @name：       get_task_meter_num_all
* @brief：      获取任务方案下所有表数量
* @param[in] ：hUdp        数据中心句柄
               inform      信息点
* @param[out]: none
* @return    ：配置信息链表
* @author    :
* @Date      ：2019-12-6
*********************************************************************
*/
uint16 get_task_meter_num_all(DB_CLIENT hUdp, uint16 infoNum)
{
    int           ret = ERR_NORMAL;
    READ_RECORD_T readRecord = {0};
    RECORD_UNIT_T recordUnit = {0};
    
    OOP_TRANS_TASK_RLUESTATE_T data = {0};

    readRecord.recordOAD.logicId = 0;
    readRecord.recordOAD.infoNum = infoNum;
    readRecord.cType = COL_NO_STORE;
    readRecord.cStart = 0;
    readRecord.cEnd = 0xffffffff;
    readRecord.sortType = DOWN_SORT;
    readRecord.recordOAD.optional = 0;
    readRecord.recordOAD.road.oadCols.nNum = 1;
    readRecord.recordOAD.road.oadCols.oad[0].value = 0x6e200200;
    readRecord.MAC.nNum = 0;

    ret = db_read_record(hUdp, &readRecord, DATA_LEN_MAX, &recordUnit);
    if (ret != ERR_OK)
    {
        PRTL_FMT_DEBUG("读记录数据失败. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n", ret, readRecord.recordOAD.logicId, 
            readRecord.recordOAD.infoNum, readRecord.recordOAD.road.oadCols.oad[0].value);
        
        return 0;
    }

    if (sizeof(data) != recordUnit.record[0].outDatalen)
    {
        PRTL_FMT_DEBUG("数据中心的数据长度不一致. sizeof(data) = %d  outDatalen = %d\n", sizeof(data), 
            recordUnit.record[0].outDatalen);

        return 0;
    }

    memcpy(&data, recordUnit.record[0].outDataBuf, sizeof(data));

    return data.paramSum;
}

/**
*********************************************************************
* @name：       get_task_meter_num_all
* @brief：      获取任务方案下所有表数量
* @param[in] ：hUdp        数据中心句柄
               inform      信息点
               isDeGroup   是否分解群组档案
* @param[out]: none
* @return    ：配置信息链表
* @author    :
* @Date      ：2019-12-6
*********************************************************************
*/
uint16 get_meter_num_from_node(DB_CLIENT hUdp, OOP_TRANS_TASK_NODE_T *node, BOOL isDeGroup)
{
    int    ret = ERR_NORMAL;
    uint32 i = 0;
    uint16 num = 0;

    uint64 startAddr = 0;
    uint64 endAddr = 0;

    if (node == NULL)
    {
        PRTL_FMT_DEBUG("输入空地址参数，node[%p]\n", node);
        return 0;
    }

    OOP_MSC_T ms = node->ms;
    
    if (ms.choice == 3)
    {
        if (isDeGroup)
        {
            for (i = 0; i < ms.msa.size; i++)
            {
                //判断是否是群组档案
                if (meter_addr_is_group(&ms.msa.add[i].add[0]))
                {
                    NOMAL_OAD_T nomalOad = {0};
                    uint32      len  = 0;
                    OOP_GROUPMETER_NEW_T  data = {0};
                    OOP_GROUPMETER_NEW_T  data1 = {0};
                    
                    nomalOad.infoNum = (uint8)(commfun_BcdToHex(ms.msa.add[i].add[4]) >> 8) * 1000 + 
                                       (uint8)(commfun_BcdToHex(ms.msa.add[i].add[4]) & 0xff) * 100 +
                                       (uint8)(commfun_BcdToHex(ms.msa.add[i].add[5]) >> 8) * 10   + 
                                       (uint8)(commfun_BcdToHex(ms.msa.add[i].add[5]) & 0xff);
                    nomalOad.oad.value = 0x6e050200;
                
                    ret = db_read_nomal(hUdp, &nomalOad, sizeof(data), (uint8*)&data, &len);
                    if((ret != ERR_OK) || (len == 0))
                    {
                        PRTL_FMT_DEBUG("读普通数据失败. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                            ret, nomalOad.logicId, nomalOad.infoNum, nomalOad.oad.value);
                    }
                
                    nomalOad.oad.value = 0x6e05ffff;
                
                    ret = db_read_nomal(hUdp, &nomalOad, sizeof(data1), (uint8*)&data1, &len);
                    if((ret != ERR_OK) || (len == 0))
                    {
                        PRTL_FMT_DEBUG("读普通数据失败. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                            ret, nomalOad.logicId, nomalOad.infoNum, nomalOad.oad.value);
                    }

                    num += (data.num + data1.num);
                }
                else
                {
                    num ++;
                }
            }
        }
        else
        {
            num = ms.msa.size;
        }
    }
    else if (ms.choice == 6)
    {
        for(i = 0; i < ms.msra.size; i++)
        {
            if (ms.msra.region[i].start.flag == 1)
            {
                memcpy_r(&startAddr, &ms.msra.region[i].start.add[0], ms.msra.region[i].start.len);
            }
            else
            {
                memcpy_r(&startAddr, &ms.msra.region[i].start.add[0], ms.msra.region[i].start.len + 1);
            }
            
            if (ms.msra.region[i].end.flag == 1)
            {
                memcpy_r(&endAddr, &ms.msra.region[i].end.add[0], ms.msra.region[i].end.len);
            }
            else
            {
                memcpy_r(&endAddr, &ms.msra.region[i].end.add[0], ms.msra.region[i].end.len + 1);
            }

            switch (ms.msra.region[i].nType)
            {
            case 0:
            {
                //前闭后开
                while (startAddr < endAddr)
                {
                    //判断表地址是否合法
                    if (!meter_addr_is_legally(&startAddr))
                    {
                        startAddr++;
                        continue;
                    }

                    if (isDeGroup)
                    {
                        OOP_TSA_T tsa = {0};

                        tsa = ms.msra.region[i].start;
                        if (ms.msra.region[i].start.flag == 1)
                        {
                            memcpy_r(&tsa.add[0], &startAddr, ms.msra.region[i].start.len);
                        }
                        else
                        {
                            memcpy_r(&tsa.add[0], &startAddr, ms.msra.region[i].start.len + 1);
                        }
                        
                        //判断是否是群组档案
                        if (meter_addr_is_group(&tsa.add[0]))
                        {
                            NOMAL_OAD_T nomalOad = {0};
                            uint32      len  = 0;
                            OOP_GROUPMETER_NEW_T  data = {0};
                            OOP_GROUPMETER_NEW_T  data1 = {0};
                            
                            nomalOad.infoNum = (uint8)(commfun_BcdToHex(tsa.add[4]) >> 8) * 1000 + 
                                               (uint8)(commfun_BcdToHex(tsa.add[4]) & 0xff) * 100 +
                                               (uint8)(commfun_BcdToHex(tsa.add[5]) >> 8) * 10   + 
                                               (uint8)(commfun_BcdToHex(tsa.add[5]) & 0xff);
                            nomalOad.oad.value = 0x6e050200;
                        
                            ret = db_read_nomal(hUdp, &nomalOad, sizeof(data), (uint8*)&data, &len);
                            if((ret != ERR_OK) || (len == 0))
                            {
                                PRTL_FMT_DEBUG("读普通数据失败. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                                    ret, nomalOad.logicId, nomalOad.infoNum, nomalOad.oad.value);
                            }
                        
                            nomalOad.oad.value = 0x6e05ffff;
                        
                            ret = db_read_nomal(hUdp, &nomalOad, sizeof(data1), (uint8*)&data1, &len);
                            if((ret != ERR_OK) || (len == 0))
                            {
                                PRTL_FMT_DEBUG("读普通数据失败. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                                    ret, nomalOad.logicId, nomalOad.infoNum, nomalOad.oad.value);
                            }
                        
                            num += (data.num + data1.num);
                        }
                        else
                        {
                            num ++;
                        }
                    }
                    else
                    {
                        num ++;
                    }
                    startAddr++;
                }
                break;
            }
            case 1:
            {
                //前开后闭
                while (startAddr <= endAddr)
                {
                    startAddr ++;

                    //判断表地址是否合法
                    if (!meter_addr_is_legally(&startAddr))
                    {
                        continue;
                    }

                    if (isDeGroup)
                    {
                        OOP_TSA_T tsa = {0};

                        tsa = ms.msra.region[i].start;
                        if (ms.msra.region[i].start.flag == 1)
                        {
                            memcpy_r(&tsa.add[0], &startAddr, ms.msra.region[i].start.len);
                        }
                        else
                        {
                            memcpy_r(&tsa.add[0], &startAddr, ms.msra.region[i].start.len + 1);
                        }
                        
                        //判断是否是群组档案
                        if (meter_addr_is_group(&tsa.add[0]))
                        {
                            NOMAL_OAD_T nomalOad = {0};
                            uint32      len  = 0;
                            OOP_GROUPMETER_NEW_T  data = {0};
                            OOP_GROUPMETER_NEW_T  data1 = {0};
                            
                            nomalOad.infoNum = (uint8)(commfun_BcdToHex(tsa.add[4]) >> 8) * 1000 + 
                                               (uint8)(commfun_BcdToHex(tsa.add[4]) & 0xff) * 100 +
                                               (uint8)(commfun_BcdToHex(tsa.add[5]) >> 8) * 10   + 
                                               (uint8)(commfun_BcdToHex(tsa.add[5]) & 0xff);
                            nomalOad.oad.value = 0x6e050200;
                        
                            ret = db_read_nomal(hUdp, &nomalOad, sizeof(data), (uint8*)&data, &len);
                            if((ret != ERR_OK) || (len == 0))
                            {
                                PRTL_FMT_DEBUG("读普通数据失败. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                                    ret, nomalOad.logicId, nomalOad.infoNum, nomalOad.oad.value);
                            }
                        
                            nomalOad.oad.value = 0x6e05ffff;
                        
                            ret = db_read_nomal(hUdp, &nomalOad, sizeof(data1), (uint8*)&data1, &len);
                            if((ret != ERR_OK) || (len == 0))
                            {
                                PRTL_FMT_DEBUG("读普通数据失败. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                                    ret, nomalOad.logicId, nomalOad.infoNum, nomalOad.oad.value);
                            }
                        
                            num += (data.num + data1.num);
                        }
                        else
                        {
                            num ++;
                        }
                    }
                    else
                    {
                        num ++;
                    }
                }
                break;
            }
            case 2:
            {
                //前闭后闭
                while (startAddr <= endAddr)
                {
                    //判断表地址是否合法
                    if (!meter_addr_is_legally(&startAddr))
                    {
                        startAddr++;
                        continue;
                    }

                    if (isDeGroup)
                    {
                        OOP_TSA_T tsa = {0};

                        tsa = ms.msra.region[i].start;
                        if (ms.msra.region[i].start.flag == 1)
                        {
                            memcpy_r(&tsa.add[0], &startAddr, ms.msra.region[i].start.len);
                        }
                        else
                        {
                            memcpy_r(&tsa.add[0], &startAddr, ms.msra.region[i].start.len + 1);
                        }
                        
                        //判断是否是群组档案
                        if (meter_addr_is_group(&tsa.add[0]))
                        {
                            NOMAL_OAD_T nomalOad = {0};
                            uint32      len  = 0;
                            OOP_GROUPMETER_NEW_T  data = {0};
                            OOP_GROUPMETER_NEW_T  data1 = {0};
                            
                            nomalOad.infoNum = (uint8)(commfun_BcdToHex(tsa.add[4]) >> 8) * 1000 + 
                                               (uint8)(commfun_BcdToHex(tsa.add[4]) & 0xff) * 100 +
                                               (uint8)(commfun_BcdToHex(tsa.add[5]) >> 8) * 10   + 
                                               (uint8)(commfun_BcdToHex(tsa.add[5]) & 0xff);
                            nomalOad.oad.value = 0x6e050200;
                        
                            ret = db_read_nomal(hUdp, &nomalOad, sizeof(data), (uint8*)&data, &len);
                            if((ret != ERR_OK) || (len == 0))
                            {
                                PRTL_FMT_DEBUG("读普通数据失败. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                                    ret, nomalOad.logicId, nomalOad.infoNum, nomalOad.oad.value);
                            }
                        
                            nomalOad.oad.value = 0x6e05ffff;
                        
                            ret = db_read_nomal(hUdp, &nomalOad, sizeof(data1), (uint8*)&data1, &len);
                            if((ret != ERR_OK) || (len == 0))
                            {
                                PRTL_FMT_DEBUG("读普通数据失败. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                                    ret, nomalOad.logicId, nomalOad.infoNum, nomalOad.oad.value);
                            }
                        
                            num += (data.num + data1.num);
                        }
                        else
                        {
                            num ++;
                        }
                    }
                    else
                    {
                        num ++;
                    }
                    startAddr++;
                }
                break;
            }
            case 3:
            {
                //前开后开
                while (startAddr < endAddr)
                {
                    startAddr++;

                    //判断表地址是否合法
                    if (!meter_addr_is_legally(&startAddr))
                    {
                        continue;
                    }

                    if (isDeGroup)
                    {
                        OOP_TSA_T tsa = {0};

                        tsa = ms.msra.region[i].start;
                        if (ms.msra.region[i].start.flag == 1)
                        {
                            memcpy_r(&tsa.add[0], &startAddr, ms.msra.region[i].start.len);
                        }
                        else
                        {
                            memcpy_r(&tsa.add[0], &startAddr, ms.msra.region[i].start.len + 1);
                        }
                        
                        //判断是否是群组档案
                        if (meter_addr_is_group(&tsa.add[0]))
                        {
                            NOMAL_OAD_T nomalOad = {0};
                            uint32      len  = 0;
                            OOP_GROUPMETER_NEW_T  data = {0};
                            OOP_GROUPMETER_NEW_T  data1 = {0};
                            
                            nomalOad.infoNum = (uint8)(commfun_BcdToHex(tsa.add[4]) >> 8) * 1000 + 
                                               (uint8)(commfun_BcdToHex(tsa.add[4]) & 0xff) * 100 +
                                               (uint8)(commfun_BcdToHex(tsa.add[5]) >> 8) * 10   + 
                                               (uint8)(commfun_BcdToHex(tsa.add[5]) & 0xff);
                            nomalOad.oad.value = 0x6e050200;
                        
                            ret = db_read_nomal(hUdp, &nomalOad, sizeof(data), (uint8*)&data, &len);
                            if((ret != ERR_OK) || (len == 0))
                            {
                                PRTL_FMT_DEBUG("读普通数据失败. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                                    ret, nomalOad.logicId, nomalOad.infoNum, nomalOad.oad.value);
                            }
                        
                            nomalOad.oad.value = 0x6e05ffff;
                        
                            ret = db_read_nomal(hUdp, &nomalOad, sizeof(data1), (uint8*)&data1, &len);
                            if((ret != ERR_OK) || (len == 0))
                            {
                                PRTL_FMT_DEBUG("读普通数据失败. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                                    ret, nomalOad.logicId, nomalOad.infoNum, nomalOad.oad.value);
                            }
                        
                            num += (data.num + data1.num);
                        }
                        else
                        {
                            num ++;
                        }
                    }
                    else
                    {
                        num ++;
                    }
                }
                break;
            }
            default:
                break;
            }
        }
    }
    else
    {
        PRTL_FMT_DEBUG("ms类型不支持 choice[%d]\n", ms.choice);
    }

    return num;
}

/**
*********************************************************************
* @name：       get_task_config_stat
* @brief：      获取任务方案配置统计
* @param[in] ：hUdp        数据中心句柄
* @param[out]: none
* @return    ：配置信息链表
* @author    :
* @Date      ：2019-12-6
*********************************************************************
*/
LINK_CONFIG_STAT_T *get_task_config_stat(DB_CLIENT hUdp)
{
    int                 i = 0;
    int                 j = 0;
    int                 ret = ERR_OK;
    uint32              handle = 0;
    uint32              recNum = 0;
    uint32              handleNode = 0;
    uint32              recNumNode = 0;
    uint16              oopConfigStatNum = 0;
    
    //链表相关变量
    LINK_CONFIG_STAT_T* pMtrHead      = NULL;
    LINK_CONFIG_STAT_T* pMtrTail      = pMtrHead;
    LINK_CONFIG_STAT_T* pNewMtr       = pMtrHead;
    OOP_CONFIG_STAT_T   oopConfigStat = {0};

    OOP_TRANS_TASK_T      oopTask;
    OOP_TRANS_TASK_NODE_T oopNode;
    READ_RECORD_T         inReadRecord;
    STEP_READ_RECORD_T    stepReadRecord;
    RECORD_UNIT_T         stepRecordOut;
    READ_RECORD_T         inReadRecordNode;
    STEP_READ_RECORD_T    stepReadRecordNode;
    RECORD_UNIT_T         stepRecordOutNode;

    memset(&stepReadRecord, 0, sizeof(stepReadRecord));
    memset(&stepRecordOut, 0, sizeof(stepRecordOut));
    memset(&inReadRecord, 0, sizeof(inReadRecord));
    memset(&stepReadRecordNode, 0, sizeof(stepReadRecordNode));
    memset(&stepRecordOutNode, 0, sizeof(stepRecordOutNode));
    memset(&inReadRecordNode, 0, sizeof(inReadRecordNode));
    memset(&oopTask, 0, sizeof(oopTask));
    memset(&oopNode, 0, sizeof(oopNode));

    //获取所有任务方案
    inReadRecord.recordOAD.infoNum = 0xffff;
    inReadRecord.cType = COL_NO_INFO;
    inReadRecord.cStart = 0;
    inReadRecord.cEnd = 0xffff;
    inReadRecord.sortType = UP_SORT;
    inReadRecord.recordOAD.optional = 0;
    inReadRecord.recordOAD.road.oadMain.value = 0;
    inReadRecord.recordOAD.road.oadCols.nNum = 0x01;
    inReadRecord.recordOAD.road.oadCols.oad[0].value = 0x6e110200;

    ret = db_read_record_start(hUdp, &inReadRecord, &handle, &recNum);
    if ((ret != ERR_OK) || (recNum == 0))
    {
        PRTL_FMT_DEBUG("任务方案开始读失败 ret = %d, infoNum = %d recNum = %d\n",
            ret, inReadRecord.recordOAD.infoNum, recNum);

        db_read_record_end(hUdp, handle);
        return NULL;
    }
    else
    {
        PRTL_FMT_TRACE("任务方案开始读成功 ret = %d, infoNum = %d recNum = %d\n",
            ret, inReadRecord.recordOAD.infoNum, recNum);
    }

    for (i = 0; i < recNum; i++)
    {
        ret = db_read_record_step(hUdp, handle, &stepReadRecord, &stepRecordOut);
        if (ret != ERR_OK)
        {
            PRTL_FMT_DEBUG("[%d]任务方案单步读失败  错误码[%d]!\n", i, ret);
            continue;
        }

        if ((stepRecordOut.recordnum != 1)
            || (stepRecordOut.record[0].road.oadCols.oad[0].value != 0x6e110200)
            || (stepRecordOut.record[0].outDatalen != sizeof(OOP_TRANS_TASK_T)))
        {
            PRTL_FMT_DEBUG("读任务方案失败!\n");
            PRTL_FMT_DEBUG("读任务方案 stepRecordOut.recordnum = %d\n", stepRecordOut.recordnum);
            PRTL_FMT_DEBUG("读任务方案 stepRecordOut.record[0].road.oadCols.oad[0] = 0x%08x\n",
                stepRecordOut.record[0].road.oadCols.oad[0].value);
            PRTL_FMT_DEBUG("读任务方案 stepRecordOut.record[0].outDatalen = %d\n",
                stepRecordOut.record[0].outDatalen);
            PRTL_FMT_DEBUG("读任务方案 sizeof(OOP_TRANS_TASK_T) = %d\n",
                sizeof(OOP_TRANS_TASK_T));
            continue;
        }

        memcpy(&oopTask, stepRecordOut.record[0].outDataBuf, stepRecordOut.record[0].outDatalen);
        PRTL_FMT_DEBUG("stepRecordOut.infoNum = %d\n", stepRecordOut.infoNum);

        memset(&oopConfigStat, 0, sizeof(oopConfigStat));

        oopConfigStat.taskId = oopTask.taskid;
        oopConfigStat.port = oopTask.oad;
        oopConfigStat.priority = oopTask.priority;
        oopConfigStat.isOnly = oopTask.bmonopoly;

        //加载某个任务方案下所有具体任务
        inReadRecordNode.recordOAD.infoNum = stepRecordOut.infoNum;
        inReadRecordNode.cType = COL_NOT;
        inReadRecordNode.cStart = 0;
        inReadRecordNode.cEnd = 0xffff;
        inReadRecordNode.sortType = UP_SORT;
        inReadRecordNode.recordOAD.road.oadMain.value = 0x6e130200;
        inReadRecordNode.recordOAD.road.oadCols.nNum = 0x00;
        inReadRecordNode.bTable = 1;
        inReadRecordNode.table.nNum = 0;
        inReadRecordNode.MAC.nNum = 0;

        ret = db_read_record_start(hUdp, &inReadRecordNode, &handleNode, &recNumNode);
        if ((ret != ERR_OK) || (recNumNode == 0))
        {
            PRTL_FMT_DEBUG("具体任务开始读失败 ret = %d, infoNum = %d recNumNode = %d\n",
                ret, inReadRecordNode.recordOAD.infoNum, recNumNode);
        }
        else
        {
            PRTL_FMT_TRACE("具体任务开始读成功 ret = %d, infoNum = %d recNumNode = %d\n",
                ret, inReadRecordNode.recordOAD.infoNum, recNumNode);
        }

        //任务方案统计
        oopConfigStat.nodeNum = 0;

        uint16 nodeMinId = 0;
        uint16 nodeMaxId = 0;
        
        for (j = 0; j < recNumNode; j++)
        {
            ret = db_read_record_step(hUdp, handleNode, &stepReadRecordNode, &stepRecordOutNode);
            if (ret != ERR_OK)
            {
                PRTL_FMT_DEBUG("具体任务单步读成功 db_read_record_step err = %d!\n", ret);
                continue;
            }

            memcpy(&oopNode, stepRecordOutNode.record[0].outDataBuf, stepRecordOutNode.record[0].outDatalen);

            if (nodeMinId == 0)
            {
                nodeMinId = oopNode.nodeid;
            }
            else if (oopNode.nodeid < nodeMinId)
            {
                nodeMinId = oopNode.nodeid;
            }

            if (nodeMaxId == 0)
            {
                nodeMaxId = oopNode.nodeid;
            }
            else if (oopNode.nodeid >  nodeMaxId)
            {
                nodeMaxId = oopNode.nodeid;
            }

            oopConfigStat.nodeNum += get_meter_num_from_node(hUdp, &oopNode, FALSE);
        }

        oopConfigStat.nodeMinId = nodeMinId;
        oopConfigStat.nodeMaxId = nodeMaxId;

        pNewMtr = (LINK_CONFIG_STAT_T *)malloc(sizeof(LINK_CONFIG_STAT_T));
        memset(pNewMtr, 0, sizeof(LINK_CONFIG_STAT_T));
        pNewMtr->oopConfigStat = oopConfigStat;
        pNewMtr->no   = oopConfigStatNum ++;
        pNewMtr->next = NULL;
        if(!pMtrHead)
            pMtrHead = pNewMtr;
        if(pMtrTail)
            pMtrTail->next = pNewMtr;
        pMtrTail = pNewMtr;
    }

    db_read_record_end(hUdp, handle);
    db_read_record_end(hUdp, handleNode);
    
    return pMtrHead;
}

/**
*********************************************************************
* @brief：      读记录型方案任务
* @param[in]： inReadRecord  - 筛选需要的数据
               oopRcsd       - 列选择
               readRecNum    - 读记录数量，：0：读全部记录 1：读最近的一条记录
               maxBufLen     - 最大读取buf长度
* @param[out]：outRecordBuf  -  数据记录
* @return：     =0            - 成功
               <0            - 错误码
*********************************************************************
*/
int record_trans_config_read_by_task(DB_CLIENT hUdp, const READ_INF_T inReadNor, RELY_INFO_T relyInfo, uint32 maxBufLen, RECORD_BUF_T *outRecordBuf)
{
    int       result = DATA_OTHER_REASON;
    uint32    offset = 0;
    OOP_DAR_E nRet = DATA_OTHER_REASON;
    uint32    bufAllLen = 0;
    BP_INFO_T breakInfo = {0};                                         //断点信息
    BOOL      isBufFull = FALSE;
    OAD_INFO_T *pOadInfo = relyInfo.pOadInfo;

    LINK_CONFIG_STAT_T *pLinkConfig = NULL;
    LINK_CONFIG_STAT_T *pLinkConfigHead = NULL;

    /*清除断点信息, 需要注意应用分帧的未帧*/
    memset(&breakInfo, 0, sizeof(BP_INFO_T));
   
    DATA_CONVERT_T   convert;
    OOP_CONFIG_STAT_T data = {0};                                        //每个任务方案信息
    uint32 sOffset = 0;
    uint32 dOffset = 0;
    uint32 oopConfigNum = 0;
    uint32 oopConfigNumOffset = 0;

    MEMZERO(&data, sizeof(data));
    MEMZERO(&convert, sizeof(convert));

    oopConfigNumOffset = offset;                                         //档案数量偏移
    outRecordBuf->recordBuf[offset ++] = 0x00;                          //数量，先填0
    bufAllLen = offset;

    pLinkConfig = get_task_config_stat(hUdp);
    if (pLinkConfig == NULL)
    {
        return DATA_TYPE_UNMATCHED;
    }
    else
    {
        pLinkConfigHead = pLinkConfig;
    }

    while (pLinkConfig)
    {
        if (relyInfo.oopRsd->choice == 1)
        {
            if (pLinkConfig->oopConfigStat.taskId != inReadNor.startTask)
            {
                pLinkConfig = pLinkConfig->next;
                continue;
            }
        }
        else if (relyInfo.oopRsd->choice == 2)
        {
            if ((pLinkConfig->oopConfigStat.taskId < inReadNor.startTask) || 
                (pLinkConfig->oopConfigStat.taskId >= inReadNor.endTask))
            {
                pLinkConfig = pLinkConfig->next;
                continue;
            }
        }
        else
        {
            PRTL_FMT_LOGCC("暂时不支持方法%d\n", relyInfo.oopRsd->choice);
            break;
        }

        if ((relyInfo.bpBreakInfo->index > 0) && (pLinkConfig->oopConfigStat.taskId < relyInfo.bpBreakInfo->index))
        {
            pLinkConfig = pLinkConfig->next;
            continue;
        }
        else
        {
            relyInfo.bpBreakInfo->index = 0;
        }

        data = pLinkConfig->oopConfigStat;

        PRTL_FMT_LOGCC("找到任务方案统计为%d\n", data.taskId);

        //转换成报文数据
        convert.srcBuf = &data;
        convert.srcLen = sizeof(data);
        convert.sOffset = &sOffset;

        convert.dstBuf = &outRecordBuf->recordBuf[offset];
        convert.dstLen = maxBufLen - offset;
        convert.dOffset = &dOffset;

        //记录断点信息
        breakInfo.index = data.taskId;

        nRet = data_to_buf_config_stat(pOadInfo, &convert);  //入参
        if (nRet == DATA_SUCCESS)
        {
            bufAllLen += dOffset;
            if (bufAllLen <= maxBufLen)
            {
                memcpy(&outRecordBuf->recordBuf[offset], convert.dstBuf, dOffset);
                offset += dOffset;
                outRecordBuf->bufLen = offset;
                PRTL_BUF_LOGCC(convert.dstBuf, dOffset, "第%d个任务方案统计报文:", data.taskId);
                oopConfigNum ++;
                if (bufAllLen + dOffset >= maxBufLen) //虽然本次没有超最大长度，但下次会
                {
                    isBufFull = TRUE;
                    PRTL_FMT_DEBUG("虽然本次没有超最大长度，但下次会 isBufFull = [%d]\n", isBufFull);
                }
                dOffset = 0;
                sOffset = 0;
            }
            else
            {
                dOffset = 0;
                sOffset = 0;
                isBufFull = TRUE;
                PRTL_FMT_DEBUG("达到最大报文长度 isBufFull = [%d]\n", isBufFull);
            }
        }
        else
        {
            pLinkConfig = pLinkConfig->next;
            continue;
        }

        if ((isBufFull) && (offset > 0))
        {
            break;
        }

        pLinkConfig = pLinkConfig->next;
    }

    link_config_stat_destroy(pLinkConfigHead);

    if (!isBufFull)
    {
        if (relyInfo.bpBreakInfo->appSlicingFlag == 0x01)
        {
            breakInfo.appSlicingFlag = 0x01;
            breakInfo.isEnd = 0x01;                           //末尾帧
            breakInfo.seq = relyInfo.bpBreakInfo->seq + 1;    //序号加1
            breakInfo.objPre = 0x01;                          //对象属性
        }
        else
        {
            /*清除断点信息*/
            memset(&breakInfo, 0, sizeof(BP_INFO_T));
        }
    }
    else
    {
        breakInfo.reqType = 0x03;
        breakInfo.appSlicingFlag = 0x01;
        breakInfo.isEnd = 0x00;                              //不是末尾帧
        breakInfo.objPre = 0x01;                             //对象属性
        if (relyInfo.bpBreakInfo->appSlicingFlag == 0x00)
        {
            breakInfo.seq = BRK_SEQ_START;                   //首次序号
        }
        else
        {
            breakInfo.seq = relyInfo.bpBreakInfo->seq + 1;  //序号加1
        }
    }

    //刷新断点信息
    memcpy(&outRecordBuf->breakInfo, &breakInfo, sizeof(BP_INFO_T));

    PRTL_BUF_LOGCC(outRecordBuf->recordBuf, offset, "整个报文:");
    PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->appSlicingFlag = %d\n", outRecordBuf->breakInfo.appSlicingFlag);
    PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->index = %d\n", outRecordBuf->breakInfo.index);
    PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->isEnd = %d\n", outRecordBuf->breakInfo.isEnd);
    PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->objPre = %d\n", outRecordBuf->breakInfo.objPre);
    PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->seq = %d\n", outRecordBuf->breakInfo.seq);
    PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->reqType = %d\n", outRecordBuf->breakInfo.reqType);

    result = DATA_SUCCESS;
    outRecordBuf->recordBuf[oopConfigNumOffset] = oopConfigNum;   //重新记数量
    outRecordBuf->bufLen = offset;

    return result;
}

/**
*********************************************************************
* @brief：      读记录型方案任务
* @param[in]： inReadRecord  - 筛选需要的数据
               oopRcsd       - 列选择
               readRecNum    - 读记录数量，：0：读全部记录 1：读最近的一条记录
               maxBufLen     - 最大读取buf长度
* @param[out]：outRecordBuf  -  数据记录
* @return：     =0            - 成功
               <0            - 错误码
*********************************************************************
*/
int record_trans_config_read_by_port(DB_CLIENT hUdp, const READ_INF_T inReadNor, RELY_INFO_T relyInfo, uint32 maxBufLen, RECORD_BUF_T *outRecordBuf)
{
    int       result = DATA_OTHER_REASON;
    uint32    offset = 0;
    OOP_DAR_E nRet = DATA_OTHER_REASON;
    uint32    bufAllLen = 0;
    BP_INFO_T breakInfo = {0};                                         //断点信息
    BOOL      isBufFull = FALSE;
    OAD_INFO_T *pOadInfo = relyInfo.pOadInfo;

    LINK_CONFIG_STAT_T *pLinkConfig = NULL;
    LINK_CONFIG_STAT_T *pLinkConfigHead = NULL;

    /*清除断点信息, 需要注意应用分帧的未帧*/
    memset(&breakInfo, 0, sizeof(BP_INFO_T));
   
    DATA_CONVERT_T   convert;
    OOP_CONFIG_STAT_T data = {0};                                        //每个任务方案信息
    uint32 sOffset = 0;
    uint32 dOffset = 0;
    uint32 oopConfigNum = 0;
    uint32 oopConfigNumOffset = 0;

    MEMZERO(&data, sizeof(data));
    MEMZERO(&convert, sizeof(convert));

    oopConfigNumOffset = offset;                                         //档案数量偏移
    outRecordBuf->recordBuf[offset ++] = 0x00;                          //数量，先填0
    bufAllLen = offset;

    pLinkConfig = get_task_config_stat(hUdp);
    if (pLinkConfig == NULL)
    {
        return DATA_TYPE_UNMATCHED;
    }
    else
    {
        pLinkConfigHead = pLinkConfig;
    }

    while (pLinkConfig)
    {
        if (relyInfo.oopRsd->choice == 1)
        {
            if (pLinkConfig->oopConfigStat.port.value != inReadNor.startTask)
            {
                pLinkConfig = pLinkConfig->next;
                continue;
            }
        }
        else if (relyInfo.oopRsd->choice == 2)
        {
            if ((pLinkConfig->oopConfigStat.port.value < inReadNor.startTask) || 
                (pLinkConfig->oopConfigStat.port.value >= inReadNor.endTask))
            {
                pLinkConfig = pLinkConfig->next;
                continue;
            }
        }
        else
        {
            PRTL_FMT_LOGCC("暂时不支持方法%d\n", relyInfo.oopRsd->choice);
            break;
        }


        if ((relyInfo.bpBreakInfo->index > 0) && (pLinkConfig->oopConfigStat.taskId < relyInfo.bpBreakInfo->index))
        {
            pLinkConfig = pLinkConfig->next;
            continue;
        }
        else
        {
            relyInfo.bpBreakInfo->index = 0;
        }

        data = pLinkConfig->oopConfigStat;

        PRTL_FMT_LOGCC("找到任务方案统计为%d\n", data.taskId);

        //转换成报文数据
        convert.srcBuf = &data;
        convert.srcLen = sizeof(data);
        convert.sOffset = &sOffset;

        convert.dstBuf = &outRecordBuf->recordBuf[offset];
        convert.dstLen = maxBufLen - offset;
        convert.dOffset = &dOffset;

        //记录断点信息
        breakInfo.index = data.taskId;

        nRet = data_to_buf_config_stat(pOadInfo, &convert);  //入参
        if (nRet == DATA_SUCCESS)
        {
            bufAllLen += dOffset;
            if (bufAllLen <= maxBufLen)
            {
                memcpy(&outRecordBuf->recordBuf[offset], convert.dstBuf, dOffset);
                offset += dOffset;
                outRecordBuf->bufLen = offset;
                PRTL_BUF_LOGCC(convert.dstBuf, dOffset, "第%d个任务方案统计报文:", data.taskId);
                oopConfigNum ++;
                if (bufAllLen + dOffset >= maxBufLen) //虽然本次没有超最大长度，但下次会
                {
                    isBufFull = TRUE;
                    PRTL_FMT_DEBUG("虽然本次没有超最大长度，但下次会 isBufFull = [%d]\n", isBufFull);
                }
                dOffset = 0;
                sOffset = 0;
            }
            else
            {
                dOffset = 0;
                sOffset = 0;
                isBufFull = TRUE;
                PRTL_FMT_DEBUG("达到最大报文长度 isBufFull = [%d]\n", isBufFull);
            }
        }
        else
        {
            pLinkConfig = pLinkConfig->next;
            continue;
        }

        if ((isBufFull) && (offset > 0))
        {
            break;
        }

        pLinkConfig = pLinkConfig->next;
    }

    link_config_stat_destroy(pLinkConfigHead);

    if (!isBufFull)
    {
        if (relyInfo.bpBreakInfo->appSlicingFlag == 0x01)
        {
            breakInfo.appSlicingFlag = 0x01;
            breakInfo.isEnd = 0x01;                           //末尾帧
            breakInfo.seq = relyInfo.bpBreakInfo->seq + 1;    //序号加1
            breakInfo.objPre = 0x01;                          //对象属性
        }
        else
        {
            /*清除断点信息*/
            memset(&breakInfo, 0, sizeof(BP_INFO_T));
        }
    }
    else
    {
        breakInfo.reqType = 0x03;
        breakInfo.appSlicingFlag = 0x01;
        breakInfo.isEnd = 0x00;                              //不是末尾帧
        breakInfo.objPre = 0x01;                             //对象属性
        if (relyInfo.bpBreakInfo->appSlicingFlag == 0x00)
        {
            breakInfo.seq = BRK_SEQ_START;                   //首次序号
        }
        else
        {
            breakInfo.seq = relyInfo.bpBreakInfo->seq + 1;  //序号加1
        }
    }

    //刷新断点信息
    memcpy(&outRecordBuf->breakInfo, &breakInfo, sizeof(BP_INFO_T));

    PRTL_BUF_LOGCC(outRecordBuf->recordBuf, offset, "整个报文:");
    PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->appSlicingFlag = %d\n", outRecordBuf->breakInfo.appSlicingFlag);
    PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->index = %d\n", outRecordBuf->breakInfo.index);
    PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->isEnd = %d\n", outRecordBuf->breakInfo.isEnd);
    PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->objPre = %d\n", outRecordBuf->breakInfo.objPre);
    PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->seq = %d\n", outRecordBuf->breakInfo.seq);
    PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->reqType = %d\n", outRecordBuf->breakInfo.reqType);

    result = DATA_SUCCESS;
    outRecordBuf->recordBuf[oopConfigNumOffset] = oopConfigNum;   //重新记数量
    outRecordBuf->bufLen = offset;

    return result;
}


/**
*********************************************************************
* @brief：      读记录型方案任务
* @param[in]： inReadRecord  - 筛选需要的数据
               oopRcsd       - 列选择
               readRecNum    - 读记录数量，：0：读全部记录 1：读最近的一条记录
               maxBufLen     - 最大读取buf长度
* @param[out]：outRecordBuf  -  数据记录
* @return：     =0            - 成功
               <0            - 错误码
*********************************************************************
*/
int record_trans_config_read(DB_CLIENT hUdp, const READ_INF_T inReadMeter, RELY_INFO_T relyInfo, uint32 maxBufLen, 
RECORD_BUF_T *outRecordBuf)
{
    if (relyInfo.oopRsd->sel1.value.type == DT_OAD)
    {
        return record_trans_config_read_by_port(hUdp, inReadMeter, relyInfo, maxBufLen, outRecordBuf);
    }
    else
    {
        return record_trans_config_read_by_task(hUdp, inReadMeter, relyInfo, maxBufLen, outRecordBuf);
    }

    return ERR_OK;
}


/**
*********************************************************************
* @name：      class11_invoke_get_oop_meter_record
* @brief：     class11的GET服务
* @param[in] ：pOdaInfo        OAD信息
               pBpInfo        应用分帧断点信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
               outDataMax     输出报文缓冲区最大长度

* @param[out]：outData     输出报文
* @return    ：OOP_DAR_E
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E class11_invoke_get_config_stat_rec(OAD_INFO_T *pOdaInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    uint16 i = 0;
    uint16 j = 0;
    OOP_DAR_E  nRet = DATA_OTHER_REASON;
    uint32 offset = 4;
    uint16 csdStart = 0, csdLen = 0;
    uint16 outOffset = 0;
    uint16 inDataLenTmp = InDataLen;

    OOP_PRSD_T oopRsd;
    OOP_RCSD_T oopRcsd;
    uint16     headLen = 0;
    READ_INF_T inReadNor;
    RELY_INFO_T  relyInfo;
    RECORD_BUF_T outRecordBuf;

    /* 清0处理 */
    MEMZERO(&oopRsd, sizeof(oopRsd));
    MEMZERO(&oopRcsd, sizeof(oopRcsd));
    MEMZERO(&inReadNor, sizeof(inReadNor));
    MEMZERO(&relyInfo, sizeof(relyInfo));
    MEMZERO(&outRecordBuf, sizeof(outRecordBuf));

    outRecordBuf.recordBuf = (uint8 *)malloc(pApduInfo->connectionInfo.ApplSplitLen);
    memset(outRecordBuf.recordBuf, 0xee, pApduInfo->connectionInfo.ApplSplitLen);

    /*处理RSD*/
    nRet = apdu_rsd_fromfmt698(inData, inDataLenTmp, &offset, &oopRsd, sizeof(oopRsd));
    PRTL_FMT_LOGCC("oopRsd.choice = %d\n", oopRsd.choice);
    if (nRet != DATA_SUCCESS)
    {
        PRTL_FMT_TRACE("RSD解析错误 nRet[%d]\n", nRet);

        /*应对电科院协议一致性测试*/
        uint16 testOffset = 4;
        uint16 inCsdOff = 0, inCsdLen = 0;
        apdu_next_dpos_get(inData, &testOffset, InDataLen, DT_RSD, TRUE);
        inCsdOff = testOffset;
        apdu_next_dpos_get(inData, &testOffset, InDataLen, DT_RCSD, TRUE);
        inCsdLen = testOffset - inCsdOff;
        memcpy(outData->pResultData, inData, 4);
        memcpy(outData->pResultData + 4, inData + inCsdOff, inCsdLen);
        outData->resultDataLen = 4 + inCsdLen;

        outData->pResultData[outData->resultDataLen++] = 1;                   //后面优化下，outData->resultDataLen与下面统一
        if (apdu_rcsd_fromfmt698((uint8 *)inData, InDataLen, &offset, &oopRcsd, sizeof(oopRcsd)) != DATA_SUCCESS)
        {
            outData->pResultData[outData->resultDataLen++] = 0;

        }
        else
        {
            outData->pResultData[outData->resultDataLen++] = 1;
            for (i = 0; i < oopRcsd.nNum; i++)
            {
                outData->pResultData[outData->resultDataLen++] = NULL_DATA;
            }
        }

        free(outRecordBuf.recordBuf);
        outRecordBuf.recordBuf = NULL;
        return DATA_SUCCESS;
    }
    else
    {
        inDataLenTmp -= offset;  //除去RSD和OAD后的长度
    }

    /*处理RCSD*/
    csdStart = offset;
    if (apdu_rcsd_fromfmt698(inData, InDataLen, &offset, &oopRcsd, sizeof(oopRcsd)) != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("CSD解析错误! offset[%d]\n", offset);
        free(outRecordBuf.recordBuf);
        outRecordBuf.recordBuf = NULL;
        return DATA_TYPE_UNMATCHED;
    }

    /*填写OAD+RCSD+记录结果1 并记下偏移headLen*/
    outData->resultDataLen = 0;
    csdLen = offset-csdStart;
    memcpy(outData->pResultData, inData, 4);
    outOffset = 4;

    if (csdLen == 1)
    {
        //如果rcsd为空，填充OAD6e120300
        outData->pResultData[outOffset ++] = 0x01;
        outData->pResultData[outOffset ++] = 0x00;
        outData->pResultData[outOffset ++] = 0x6e;
        outData->pResultData[outOffset ++] = 0x12;
        outData->pResultData[outOffset ++] = 0x03;
        outData->pResultData[outOffset ++] = 0x00;
    }
    else
    {
        memcpy(&outData->pResultData[outOffset], &inData[csdStart], csdLen);
        outOffset += csdLen;
    }
    outData->pResultData[outOffset++] = 1;
    outData->resultDataLen += outOffset;
    headLen = outOffset;
    PRTL_BUF_LOGCC(&outData->pResultData[0], headLen, "填写OAD+RCSD+记录结果1 并记下偏移headLen head:");

    /*记录型档案接口提供输入参数*/
    if (general_param_get_fj(&oopRsd, &oopRcsd, pApduInfo, &inReadNor, &relyInfo) != ERR_OK)
    {
        free(outRecordBuf.recordBuf);
        outRecordBuf.recordBuf = NULL;
        return DATA_READ_WRITE_DENIED;
    }

    /* 依赖信息中存下OAD列表的指针 */
    relyInfo.pOadInfo = pOdaInfo;

    PRTL_FMT_LOGCC("****************************************************\n");
    PRTL_FMT_LOGCC("inReadMeter.startInfoNum = %d\n", inReadNor.startTask);
    PRTL_FMT_LOGCC("inReadMeter.endInfoNum = %d\n", inReadNor.endTask);
    PRTL_FMT_LOGCC("relyInfo.oopRcsd.nNum = %d\n", relyInfo.oopRcsd.nNum);
    for(i = 0; i < relyInfo.oopRcsd.nNum; i++)
    {
        if (relyInfo.oopRcsd.cols[i].choice == 0)
        {
            PRTL_FMT_LOGCC("choice = %d relyInfo.oopRcsd.cols[%d].oad.value = 0x%08x\n", i, relyInfo.oopRcsd.cols[i].choice, 
                                                                                      relyInfo.oopRcsd.cols[i].oad.value);
        }
        else if(relyInfo.oopRcsd.cols[i].choice == 1)
        {
            PRTL_FMT_LOGCC("choice = %d relyInfo.oopRcsd.cols[%d].road.oadMain.value = 0x%08x\n", i, relyInfo.oopRcsd.cols[i].choice, 
                                                                                              relyInfo.oopRcsd.cols[i].road.oadMain.value);
            for (j = 0; j < relyInfo.oopRcsd.cols[i].road.oadCols.nNum; j++)
            {
                PRTL_FMT_LOGCC("choice = %d relyInfo.oopRcsd.cols[%d].road.oadCols.oad[%d].value = 0x%08x\n", i, j, relyInfo.oopRcsd.cols[i].choice, 
                                                                                                          relyInfo.oopRcsd.cols[i].road.oadCols.oad[j].value);
            }
        }
    }
    PRTL_FMT_LOGCC("****************************************************\n");

    /*读记录档案接口，可以读出apdu数据域，可返回断点信息*/
    if (record_trans_config_read(pApduInfo->hUdp, inReadNor, relyInfo, pApduInfo->connectionInfo.ApplSplitLen - headLen - cal_excludeLen(pApduInfo->connectionInfo), &outRecordBuf))
    {
        nRet = DATA_READ_WRITE_DENIED;
    }
    else
    {
        memcpy(&outData->pResultData[headLen], outRecordBuf.recordBuf, outRecordBuf.bufLen);
        outData->resultDataLen += outRecordBuf.bufLen;

        if (outRecordBuf.breakInfo.appSlicingFlag == 1)
        {
            /*记下断点信息*/
            PRTL_FMT_LOGCC("记下断点信息\n");
            outRecordBuf.breakInfo.objPre = 0x02;   //对象属性，记录型数据
            memcpy(pApduInfo->bpBreakInfo, &outRecordBuf.breakInfo, sizeof(BP_INFO_T));
        }
        else
        {
            /*清除断点信息*/
            PRTL_FMT_LOGCC("清除断点信息\n");
            memset(pApduInfo->bpBreakInfo, 0, sizeof(BP_INFO_T));
        }
        
        nRet = DATA_SUCCESS;
    }
    
    free(outRecordBuf.recordBuf);
    outRecordBuf.recordBuf = NULL;
    return DATA_SUCCESS;
}


/**
*********************************************************************
* @name：      class11_invoke_get_transparent_task_all_nor
* @brief：     class11的GET服务
* @param[in] ：pOdaInfo        OAD信息
               pBpInfo        应用分帧断点信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
               outDataMax     输出报文缓冲区最大长度

* @param[out]：outData     输出报文
* @return    ：OOP_DAR_E
* @author    :
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E class11_invoke_get_config_stat_nor(OAD_INFO_T* pOadInfo, APDU_INFO_T* pApduInfo, const uint8* inData, uint16 inLen, RESULT_DATA_T* outData, uint16 outDataMax)
{
    int result = DATA_OTHER_REASON;
    uint32 i = 0;
    uint32 offset = 0;
    
    uint32 sOffset = 0;
    uint32 dOffset = 0;
    
    OOP_CONFIG_STAT_T configInfo = {0};
    DATA_CONVERT_T   convert = {0};
    uint32    bufAllLen = 0;
    BP_INFO_T breakInfo = {0};                                          //断点信息
    uint32    maxBufLen = 0;
    BOOL      isBufFull = FALSE;

    LINK_CONFIG_STAT_T *pLinkConfig = NULL;
    LINK_CONFIG_STAT_T *pLinkConfigHead = NULL;
    
    maxBufLen = pApduInfo->connectionInfo.ApplSplitLen - cal_excludeLen(pApduInfo->connectionInfo);

    /*清除断点信息, 需要注意应用分帧的未帧*/
    memset(&breakInfo, 0, sizeof(BP_INFO_T));
   
    uint8  configNum = 0;
    uint32 configNumOffset = 0;
    memcpy(&outData->pResultData[offset], inData, sizeof(OOP_OAD_U));   //拷贝OAD
    offset += sizeof(OOP_OAD_U);
    outData->pResultData[offset ++] = 0x01;                             //结果
    outData->pResultData[offset ++] = DT_ARRAY;                         //类型
    configNumOffset = offset;                                           //任务数量偏移
    outData->pResultData[offset ++] = 0x00;                             //数量，先填0
    bufAllLen = offset;

    pLinkConfig = get_task_config_stat(pApduInfo->hUdp);
    if (pLinkConfig == NULL)
    {
        return DATA_TYPE_UNMATCHED;
    }
    else
    {
        pLinkConfigHead = pLinkConfig;
    }
    
    while (pLinkConfig)
    { 
        if (slicing_is_comtinue(pApduInfo->bpBreakInfo, i++))
        {
            pLinkConfig = pLinkConfig->next;
            continue;
        }
        
        memset((uint8*)&configInfo, 0x00, sizeof(OOP_CONFIG_STAT_T));

        configInfo = pLinkConfig->oopConfigStat;
        
        //转换成报文数据
        
        convert.srcBuf = &configInfo;
        convert.srcLen = sizeof(configInfo);
        convert.sOffset = &sOffset;

        convert.dstBuf = &outData->pResultData[offset];
        convert.dstLen = outDataMax - offset;
        convert.dOffset = &dOffset;

        //记录断点信息
        breakInfo.index = pLinkConfig->no;
        
        result = data_to_buf_config_stat(pOadInfo, &convert);
        if(result == ERR_OK)
        {
            bufAllLen += dOffset;
            if (bufAllLen <= maxBufLen)
            {
                memcpy(&outData->pResultData[offset], convert.dstBuf, dOffset);
                offset += dOffset;
                outData->resultDataLen = offset;
                PRTL_BUF_LOGCC(convert.dstBuf, dOffset, "第%d个任务方案统计信息报文:", i);
                configNum ++;
                dOffset = 0;
                sOffset = 0;
            }
            else
            {
                dOffset = 0;
                sOffset = 0;
                isBufFull = TRUE;
                PRTL_FMT_DEBUG("读任务方案统计信息产生断点信息 isBufFull = %d\n", isBufFull);
            }
        }
        else
        {
            pLinkConfig = pLinkConfig->next;
            continue;
        }
        if ((isBufFull) && (offset > 0))
        {
            break;
        }

        pLinkConfig = pLinkConfig->next;
    }
    if (!isBufFull)
    {
        if (pApduInfo->bpBreakInfo->appSlicingFlag == 0x01)
        {
            breakInfo.appSlicingFlag = 0x01;
            breakInfo.isEnd = 0x01;                           //末尾帧
            breakInfo.seq = pApduInfo->bpBreakInfo->seq + 1;  //序号加1
            breakInfo.objPre = 0x01;                          //对象属性
        }
        else
        {
            /*清除断点信息*/
            memset(&breakInfo, 0, sizeof(BP_INFO_T));
        }
    }
    else
    {
        breakInfo.reqType = pApduInfo->reqType;
        breakInfo.appSlicingFlag = 0x01;
        breakInfo.isEnd = 0x00;                              //不是末尾帧
        breakInfo.objPre = 0x01;                             //对象属性
        if (pApduInfo->bpBreakInfo->appSlicingFlag == 0x00)
        {
            breakInfo.seq = BRK_SEQ_START;                   //首次序号
        }
        else
        {
            breakInfo.seq = pApduInfo->bpBreakInfo->seq + 1;  //序号加1
        }
    }
    //刷新断点信息
    memcpy(pApduInfo->bpBreakInfo, &breakInfo, sizeof(BP_INFO_T));

    PRTL_BUF_LOGCC(outData->pResultData, offset, "整个任务方案统计信息报文:");
    PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->appSlicingFlag = %d\n", pApduInfo->bpBreakInfo->appSlicingFlag);
    PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->index = %d\n", pApduInfo->bpBreakInfo->index);
    PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->isEnd = %d\n", pApduInfo->bpBreakInfo->isEnd);
    PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->objPre = %d\n", pApduInfo->bpBreakInfo->objPre);
    PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->seq = %d\n", pApduInfo->bpBreakInfo->seq);
    PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->reqType = %d\n", pApduInfo->bpBreakInfo->reqType);

    if (configNum > 0)
    {
        outData->pResultData[configNumOffset] = configNum;   //重新记档案数量
        outData->resultDataLen = offset;
        result = DATA_SUCCESS;
    }
    else if(configNum == 0)
    {
        memcpy_r(outData->pResultData,(uint8*)&pOadInfo->pTab->oad.value,4);
        outData->pResultData[4]=DT_ARRAY;
        outData->pResultData[5]=0x01;
        outData->pResultData[6]=0x00;
        outData->resultDataLen=7;
        result = DATA_SUCCESS;
    }
    else
    {
        result = DATA_SUCCESS;
    }
    
    link_config_stat_destroy(pLinkConfigHead);

    return result;

}
/**
*********************************************************************
* @name：      class11_invoke_get_transparent_task_all
* @brief：     class11的GET服务
* @param[in] ：pOdaInfo        OAD信息
               pBpInfo        应用分帧断点信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
               outDataMax     输出报文缓冲区最大长度

* @param[out]：outData     输出报文
* @return    ：OOP_DAR_E
* @author    :
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E class11_invoke_get_config_stat(OAD_INFO_T* pOadInfo, APDU_INFO_T* pApduInfo, const uint8* inData, 
    uint16 inLen, RESULT_DATA_T* outData, uint16 outDataMax)
{
    //如果是应用分帧状态，将apdu[1]修改成起始帧方式
    if (pApduInfo->bpBreakInfo->appSlicingFlag == 1)
    {
        pApduInfo->reqType = pApduInfo->bpBreakInfo->reqType;
    }

    switch (pApduInfo->reqType)
    {
    case 0x01:
    case 0x02:
        return class11_invoke_get_config_stat_nor(pOadInfo, pApduInfo, inData, inLen, outData, outDataMax);
        break;
    case 0x03:
    case 0x04:
        return class11_invoke_get_config_stat_rec(pOadInfo, pApduInfo, inData, inLen, outData, outDataMax);
        break;
    default:
        break;
    }

    return DATA_OBJECT_UNDEFINED;
}
#endif

#if DESC("全局透明方法任务操作", 1)
/**
*********************************************************************
* @brief：      清除指定任务方案的数据
* @param[in]： fd  数据中心句柄
* @param[in]： infoNum   信息点
* @return：     =0, 成功 <0 失败
*********************************************************************
*/
int task_data_clear(DB_CLIENT fd, uint16 infoNum)
{
    int ret = ERR_OK;
    CLEAR_DATA_T clear;

    MEMZERO(&clear, sizeof(clear));

    clear.bClassTag = TRUE;
    clear.recordOAD.infoNum = infoNum;
    clear.recordOAD.classtag = CLASS_DATA_COLLEC;
    
    ret = db_clear_oad_data(fd, &clear);

    PRTL_FMT_DEBUG("meter data clear. ret(%d), infoNum(%d), classtag(%d)\n", 
        ret, clear.recordOAD.infoNum, clear.recordOAD.classtag);     

    return ret;
}

/**
*********************************************************************
* @brief：      清除指定任务方案的具体任务
* @param[in]： fd  数据中心句柄
* @param[in]： infoNum   信息点
* @return：     =0, 成功 <0 失败
*********************************************************************
*/
int task_node_clear(DB_CLIENT fd, uint16 infoNum)
{
    int ret = ERR_OK;
    CLEAR_DATA_T delData;

    MEMZERO(&delData, sizeof(delData));

    delData.recordOAD.logicId = 0x00;
    delData.recordOAD.optional = 0;
    delData.recordOAD.road.oadMain.value = 0x6e130200;
    delData.recordOAD.road.oadCols.nNum = 1;
    delData.recordOAD.road.oadCols.oad[0].value = 0xffffffff;
    delData.bClassTag = FALSE;
    
    delData.recordOAD.infoNum = infoNum;
    delData.bFilter = 1;
    delData.filterType = 255;
    
    ret = db_clear_oad_data(fd, &delData);
    if (ret != ERR_OK)
    {
        PRTL_FMT_LOGCC("删除具体任务%d err = %d taskId[%d] nodeid[%x]\n", ret, infoNum, 0xffffffff);
    }

    return ret;
}

/**
*********************************************************************
* @brief：      清除指定任务方案的采集状态
* @param[in]： fd  数据中心句柄
* @param[in]： infoNum   信息点
* @return：     =0, 成功 <0 失败
*********************************************************************
*/
int task_trans_acq_clear(DB_CLIENT fd, uint16 infoNum)
{
    int ret = ERR_OK;
    CLEAR_DATA_T RecordDel;

    MEMZERO(&RecordDel, sizeof(RecordDel));

    RecordDel.recordOAD.logicId = 0;
    RecordDel.recordOAD.infoNum = infoNum;
    RecordDel.recordOAD.optional = 0;
    RecordDel.recordOAD.road.oadMain.value = 0;
    RecordDel.recordOAD.road.oadCols.nNum = 1;
    RecordDel.recordOAD.road.oadCols.oad[0].value = 0x6e200200;
    RecordDel.bClassTag = FALSE;
    
    ret = db_clear_oad_data(fd, &RecordDel);
    if (ret != ERR_OK)
    {
        PRTL_FMT_DEBUG("数据删除失败 ret[%d]\n", ret);
    }

    return ret;
}

/**
*********************************************************************
* @name：      class11_invoke_add_transparent_task
* @brief：     class11的ACTION服务
* @param[in] ：pOdaInfo        OAD信息
               pBpInfo        应用分帧断点信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
               outDataMax     输出报文缓冲区最大长度

* @param[out]：outData     输出报文
* @return    ：OOP_DAR_E
* @author    :
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E class11_invoke_add_transparent_task(OAD_INFO_T* pOadInfo, APDU_INFO_T* pApduInfo, const uint8* inData, 
    uint16 inLen, RESULT_DATA_T* outData, uint16 outDataMax)
{
    uint16 i = 0;
    int    ret = ERR_OK;
    uint32 sOffset = 4;
    uint32 dOffset = 0;
    uint8  num = 0;

    NOMAL_OAD_T nomalOad = { 0 };
    OOP_DAR_E   dar = DATA_OTHER_REASON;

    OOP_TRANS_TASK_T   data;
    OOP_TRANS_TASK_T   dataLast;
    DATA_CONVERT_T     convert;
    uint32             len = 0;

    memset(&data, 0, sizeof(data));
    memset(&dataLast, 0, sizeof(dataLast));
    memset(&convert, 0, sizeof(convert));

    /*类型不存在*/
    if ((pOadInfo == NULL) || (pApduInfo == NULL) || (inData == NULL) || (outData == NULL))
    {
        return DATA_READ_WRITE_DENIED;
    }

    convert.srcBuf = (void*)inData;
    convert.srcLen = inLen;
    convert.sOffset = &sOffset;

    convert.dstBuf = &data;
    convert.dstLen = sizeof(data);
    convert.dOffset = &dOffset;

    switch (pOadInfo->pTab->oad.value)
    {
    case 0x6e117F00:
    {
        if (inData[sOffset++] != DT_ARRAY)
        {
            return ERR_OPTION;
        }

        num = inData[sOffset++];

        PRTL_FMT_DEBUG("增加方案任务，数量为%d\n", num);
        for (i = 0; i < num; i++)
        {
            dar = buf_to_data_trans_task(pOadInfo, &convert);
            if (dar != DATA_SUCCESS)
            {
                return dar;
            }
            else
            {
                dOffset = 0;
            }

            PRTL_FMT_DEBUG("需要增加任务方案号:%d\n", data.taskid);

            nomalOad.logicId = 0x00;
            nomalOad.infoNum = data.taskid;
            nomalOad.oad.value = 0x6e110200;
            nomalOad.classtag = CLASS_DATA_INIT;

            ret = db_read_nomal(pApduInfo->hUdp, &nomalOad, sizeof(dataLast), (uint8*)&dataLast, &len);
            if ((ret != ERR_OK) || (len != sizeof(dataLast)))
            {
                PRTL_FMT_DEBUG("读取数据中心失败. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                    ret, nomalOad.logicId, nomalOad.infoNum, nomalOad.oad.value);
            }
            else
            {
                if (memcmp(&dataLast, &data, sizeof(OOP_TRANS_TASK_T)) == 0)
                {
                    PRTL_FMT_DEBUG("数据中心中存有一样的任务方案:%d, 无需重新下发\n", data.taskid);
                    continue;
                }
            }

            if ((data.oad.value  != 0xf2010200) && 
                (data.oad.value  != 0xf2010201) &&
                (data.oad.value  != 0xf2010202) &&
                (data.oad.value  != 0xf2090200) &&
                (data.oad.value  != 0xf2090201) &&
                (data.oad.value  != 0xf2E10200) &&
                (data.oad.value  != 0xf2E10201) &&
                (data.oad.nObjID != 0xf2E0)     &&
                (data.oad.nObjID != 0xf208))
            {
                PRTL_FMT_DEBUG("下发的端口不支持 oad[0x%08x]\n", data.oad.value);
                return DATA_READ_WRITE_DENIED;
            }

            ret = db_write_nomal(pApduInfo->hUdp, &nomalOad, (uint8*)&data, sizeof(data));
            if (ret != ERR_OK)
            {
                PRTL_FMT_DEBUG("写入数据中心失败. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                    ret, nomalOad.logicId, nomalOad.infoNum, nomalOad.oad.value);
                return DATA_READ_WRITE_DENIED;
            }

            //删除相关数据
            ret = task_data_clear(pApduInfo->hUdp, data.taskid);
            if(ret != ERR_OK)
            {
                PRTL_FMT_DEBUG("meter data clear failed. ret(%d), omd(0x%08x)\n", ret, pOadInfo->pTab->oad.value);
                return DATA_READ_WRITE_DENIED;
            }
        }

        memcpy_r((uint8*)outData->pResultData, (uint8*)&pOadInfo->pTab->oad.value, 4);
        outData->pResultData[4] = DATA_SUCCESS;
        outData->pResultData[5] = 0x00;                          //DATA:NULL
        outData->resultDataLen = 0x06;

        dar = DATA_SUCCESS;
    }
    break;
    default:
        break;
    }

    return dar;
}

/**
*********************************************************************
* @name：      class11_invoke_del_transparent_task
* @brief：     class11的ACTION服务
* @param[in] ：pOdaInfo        OAD信息
               pBpInfo        应用分帧断点信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
               outDataMax     输出报文缓冲区最大长度

* @param[out]：outData     输出报文
* @return    ：OOP_DAR_E
* @author    :
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E class11_invoke_del_transparent_task(OAD_INFO_T* pOadInfo, APDU_INFO_T* pApduInfo, const uint8* inData, 
    uint16 inLen, RESULT_DATA_T* outData, uint16 outDataMax)
{
    uint16 i = 0;
    int    ret = ERR_OK;
    uint32 sOffset = 4;
    uint8  num = 0;

    CLEAR_DATA_T RecordDel;
    OOP_DAR_E   dar = DATA_OTHER_REASON;

    uint16   taskid = 0;;

    memset(&taskid, 0, sizeof(taskid));
    memset(&RecordDel, 0, sizeof(RecordDel));

    /*类型不存在*/
    if ((pOadInfo == NULL) || (pApduInfo == NULL) || (inData == NULL) || (outData == NULL))
    {
        return DATA_READ_WRITE_DENIED;
    }

    switch (pOadInfo->pTab->oad.value)
    {
    case 0x6e118000:
    {
        if (inData[sOffset++] != DT_ARRAY)
        {
            return ERR_OPTION;
        }

        num = inData[sOffset++];
        if (num == 0x81)
        {
            num = inData[sOffset++];
        }
        else if (num == 0x82)
        {
            num = inData[sOffset++];
            num <<= 8;
            num += inData[sOffset++];
        }
        else if(num > 0x82||num == 0x80)
        {
            return DATA_TYPE_UNMATCHED;
        }

        PRTL_FMT_DEBUG("删除任务方案，数量为%d\n", num);
        for (i = 0; i < num; i++)
        {
            dar = basic_buf_to_data(E_LONG_UNSIGNED, inData, inLen, &sOffset, &taskid, sizeof(taskid));
            if (dar != DATA_SUCCESS)
            {
                PRTL_BUF_DEBUG(inData, inLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
                return DATA_TYPE_UNMATCHED;
            }

            PRTL_FMT_DEBUG("需要删除任务方案号:%d\n", taskid);

            RecordDel.recordOAD.logicId = 0;
            RecordDel.recordOAD.infoNum = taskid;
            RecordDel.recordOAD.optional = 0;
            RecordDel.recordOAD.road.oadMain.value = 0;
            RecordDel.recordOAD.road.oadCols.nNum = 1;
            RecordDel.recordOAD.road.oadCols.oad[0].value = 0x6e110200;
            RecordDel.bClassTag = FALSE;

            PRTL_FMT_DEBUG("要删除任务方案存入数据中心的信息点为%d\n", RecordDel.recordOAD.infoNum);

            ret = db_clear_oad_data(pApduInfo->hUdp, &RecordDel);
            if (ret != ERR_OK)
            {
                PRTL_FMT_DEBUG("数据删除失败 ret[%d]\n", ret);
            }

            //删除相关数据
            ret = task_data_clear(pApduInfo->hUdp, taskid);
            if (ret != ERR_OK)
            {
                PRTL_FMT_DEBUG("删除相关数据 失败. ret(%d), omd(0x%08x)\n", ret, pOadInfo->pTab->oad.value);
                return DATA_READ_WRITE_DENIED;
            }

            //删除相关具体任务
            ret = task_node_clear(pApduInfo->hUdp, taskid);
            if (ret != ERR_OK)
            {
                PRTL_FMT_DEBUG("删除相关具体任务 失败. ret(%d), omd(0x%08x)\n", ret, pOadInfo->pTab->oad.value);
                return DATA_READ_WRITE_DENIED;
            }

            //删除相关任务状态
            ret = task_trans_acq_clear(pApduInfo->hUdp, taskid);
            if (ret != ERR_OK)
            {
                PRTL_FMT_DEBUG("删除相关任务状态 失败. ret(%d), omd(0x%08x)\n", ret, pOadInfo->pTab->oad.value);
                return DATA_READ_WRITE_DENIED;
            }
        }

        memcpy_r((uint8*)outData->pResultData, (uint8*)&pOadInfo->pTab->oad.value, 4);
        if (ret == ERR_OK)
        {
            dar = DATA_SUCCESS;
            outData->pResultData[4] = DATA_SUCCESS;
            outData->pResultData[5] = 0x00;                          //DATA:NULL
            outData->resultDataLen = 0x06;
        }
        else
        {
            dar = DATA_READ_WRITE_DENIED;
            outData->pResultData[4] = DATA_READ_WRITE_DENIED;
            outData->resultDataLen = 0x05;
        }
    }
    break;
    default:
        break;
    }

    return dar;
}

/**
*********************************************************************
* @name：      class11_invoke_clr_transparent_task
* @brief：     class11的ACTION服务
* @param[in] ：pOdaInfo        OAD信息
               pBpInfo        应用分帧断点信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
               outDataMax     输出报文缓冲区最大长度

* @param[out]：outData     输出报文
* @return    ：OOP_DAR_E
* @author    :
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E class11_invoke_clr_transparent_task(OAD_INFO_T* pOadInfo, APDU_INFO_T* pApduInfo, const uint8* inData, 
    uint16 inLen, RESULT_DATA_T* outData, uint16 outDataMax)
{
    int    ret = ERR_OK;

    CLEAR_DATA_T RecordDel;
    OOP_DAR_E   dar = DATA_OTHER_REASON;

    memset(&RecordDel, 0, sizeof(RecordDel));

    /*类型不存在*/
    if ((pOadInfo == NULL) || (pApduInfo == NULL) || (inData == NULL) || (outData == NULL))
    {
        return DATA_READ_WRITE_DENIED;
    }

    switch (pOadInfo->pTab->oad.value)
    {
    case 0x6e118100:
    {
        PRTL_FMT_DEBUG("清空任务方案\n");

        RecordDel.recordOAD.logicId = 0;
        RecordDel.recordOAD.infoNum = 0xffff;
        RecordDel.recordOAD.optional = 0;
        RecordDel.recordOAD.road.oadMain.value = 0;
        RecordDel.recordOAD.road.oadCols.nNum = 1;
        RecordDel.recordOAD.road.oadCols.oad[0].value = 0x6e110200;
        RecordDel.bClassTag = FALSE;

        ret = db_clear_oad_data(pApduInfo->hUdp, &RecordDel);
        if (ret != ERR_OK)
        {
            PRTL_FMT_LOGCC("清空任务方案失败 ret[%d]\n", ret);
        }

        RecordDel.recordOAD.road.oadCols.oad[0].value = 0x6e11ffff;

        ret = db_clear_oad_data(pApduInfo->hUdp, &RecordDel);
        if (ret != ERR_OK)
        {
            PRTL_FMT_LOGCC("清空任务方案失败 ret[%d]\n", ret);
        }

        //删除相关数据
        ret = task_data_clear(pApduInfo->hUdp, 0xffff);
        if (ret != ERR_OK)
        {
            PRTL_FMT_DEBUG("删除相关数据 失败. ret(%d), omd(0x%08x)\n", ret, pOadInfo->pTab->oad.value);
            return DATA_READ_WRITE_DENIED;
        }

        //删除相关具体任务
        ret = task_node_clear(pApduInfo->hUdp, 0xffff);
        if (ret != ERR_OK)
        {
            PRTL_FMT_DEBUG("删除相关具体任务 失败. ret(%d), omd(0x%08x)\n", ret, pOadInfo->pTab->oad.value);
            return DATA_READ_WRITE_DENIED;
        }

        //删除相关任务状态
        ret = task_trans_acq_clear(pApduInfo->hUdp, 0xffff);
        if (ret != ERR_OK)
        {
            PRTL_FMT_DEBUG("删除相关任务状态 失败. ret(%d), omd(0x%08x)\n", ret, pOadInfo->pTab->oad.value);
            return DATA_READ_WRITE_DENIED;
        }

        memcpy_r((uint8*)outData->pResultData, (uint8*)&pOadInfo->pTab->oad.value, 4);
        if (ret == ERR_OK)
        {
            dar = DATA_SUCCESS;
            outData->pResultData[4] = DATA_SUCCESS;
            outData->pResultData[5] = 0x00;                          //DATA:NULL
            outData->resultDataLen = 0x06;
        }
        else
        {
            dar = DATA_READ_WRITE_DENIED;
            outData->pResultData[4] = DATA_READ_WRITE_DENIED;
            outData->resultDataLen = 0x05;
        }
    }
    break;
    default:
        break;
    }

    return dar;
}

/**
*********************************************************************
* @name：      class11_invoke_add_transparent_task
* @brief：     class11的ACTION服务
* @param[in] ：pOdaInfo        OAD信息
               pBpInfo        应用分帧断点信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
               outDataMax     输出报文缓冲区最大长度

* @param[out]：outData     输出报文
* @return    ：OOP_DAR_E
* @author    :
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E class11_invoke_upd_transparent_task(OAD_INFO_T* pOadInfo, APDU_INFO_T* pApduInfo, const uint8* inData, 
    uint16 inLen, RESULT_DATA_T* outData, uint16 outDataMax)
{
    uint16 taskId = 0;
    uint8  priority = 0;
    int    ret = ERR_OK;
    uint32 sOffset = 4;
    uint32 len = 0;
    uint32 num = 0;
    uint32 i = 0;

    NOMAL_OAD_T nomalOad = { 0 };
    OOP_DAR_E   dar = DATA_OTHER_REASON;

    CLEAR_DATA_T RecordDel;
    memset(&RecordDel, 0, sizeof(RecordDel));

    OOP_TRANS_TASK_T data;
    memset(&data, 0, sizeof(data));

    OOP_TRANS_UPDATE_T update;
    OOP_TRANS_UPDATE_T updateGet;
    memset(&update, 0, sizeof(update));
    memset(&updateGet, 0, sizeof(updateGet));

    /*类型不存在*/
    if ((pOadInfo == NULL) || (pApduInfo == NULL) || (inData == NULL) || (outData == NULL))
    {
        return DATA_READ_WRITE_DENIED;
    }

    switch (pOadInfo->pTab->oad.value)
    {
    case 0x6e118200:
    {
        if (inData[sOffset++] != DT_ARRAY)
        {
            return ERR_TYPE;
        }

        num = inData[sOffset++];
        if (num == 0x81)
        {
            num = inData[sOffset++];
        }
        else if (num == 0x82)
        {
            num = inData[sOffset++];
            num <<= 8;
            num += inData[sOffset++];
        }
        else if (num > 0x82 || num == 0x80)
        {
            return DATA_TYPE_UNMATCHED;
        }

        for (i = 0; i < num; i++)
        {
            if (inData[sOffset++] != DT_STRUCTURE)
            {
                return ERR_TYPE;
            }
            
            if (inData[sOffset++] != 2)
            {
                return ERR_OPTION;
            }
            
            dar = basic_buf_to_data(E_LONG_UNSIGNED, inData, inLen, &sOffset, &taskId, sizeof(taskId));
            if (dar != DATA_SUCCESS)
            {
                PRTL_BUF_DEBUG(inData, inLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
                return DATA_TYPE_UNMATCHED;
            }
            
            //执行优先级
            dar = basic_buf_to_data(E_UNSIGNED, inData, inLen, &sOffset, &priority, sizeof(priority));
            if (dar != DATA_SUCCESS)
            {
                PRTL_BUF_DEBUG(inData, inLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
                return DATA_TYPE_UNMATCHED;
            }
            
            PRTL_FMT_DEBUG("更新方案任务，taskId为%d\n", taskId);
            PRTL_FMT_DEBUG("更新方案任务，priority为%d\n", priority);

            if (priority == 0)
            {
                RecordDel.recordOAD.logicId = 0;
                RecordDel.recordOAD.infoNum = taskId;
                RecordDel.recordOAD.optional = 0;
                RecordDel.recordOAD.road.oadMain.value = 0;
                RecordDel.recordOAD.road.oadCols.nNum = 1;
                RecordDel.recordOAD.road.oadCols.oad[0].value = 0x6e110200;
                RecordDel.bClassTag = FALSE;
                PRTL_FMT_DEBUG("要删除任务方案存入数据中心的信息点为%d\n", RecordDel.recordOAD.infoNum);
                
                ret = db_clear_oad_data(pApduInfo->hUdp, &RecordDel);
                if (ret != ERR_OK)
                {
                    PRTL_FMT_DEBUG("数据删除失败 ret[%d]\n", ret);
                }

                RecordDel.recordOAD.road.oadCols.oad[0].value = 0x6e11ffff;
                
                ret = db_clear_oad_data(pApduInfo->hUdp, &RecordDel);
                if (ret != ERR_OK)
                {
                    PRTL_FMT_DEBUG("数据删除失败 ret[%d]\n", ret);
                }
                
            }
            else if (priority == 255)
            {
                update.transTaskid = taskId;
                update.priority = priority;
                memset(&nomalOad, 0, sizeof(nomalOad));
                nomalOad.logicId = 0x00;
                nomalOad.infoNum = update.transTaskid;
                nomalOad.oad.value = 0x6e11ffff;
                nomalOad.classtag = CLASS_DATA_INIT;

                ret = db_read_nomal(pApduInfo->hUdp, &nomalOad, sizeof(updateGet), (uint8*)&updateGet, &len);
                if ((ret != ERR_OK) || (len == 0))
                {
                    PRTL_FMT_DEBUG("读数据中心失败. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                        ret, nomalOad.logicId, nomalOad.infoNum, nomalOad.oad.value);
                }

                if (updateGet.priority != 254)
                {
                    ret = db_write_nomal(pApduInfo->hUdp, &nomalOad, (uint8*)&update, sizeof(update));
                    if (ret != ERR_OK)
                    {
                        PRTL_FMT_DEBUG("写入数据中心失败. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                            ret, nomalOad.logicId, nomalOad.infoNum, nomalOad.oad.value);
                        return DATA_READ_WRITE_DENIED;
                    }
                }
                else
                {
                    PRTL_FMT_DEBUG("更新状态之前是254(暂停)，255不写数据中心\n");
                }
            }
            else 
            {
                nomalOad.logicId = 0x00;
                nomalOad.infoNum = taskId;
                nomalOad.oad.value = 0x6e110200;
                nomalOad.classtag = CLASS_DATA_INIT;
                
                ret = db_read_nomal(pApduInfo->hUdp, &nomalOad, sizeof(data), (uint8*)&data, &len);
                if ((ret != ERR_OK) || (len == 0))
                {
                    PRTL_FMT_DEBUG("读数据中心失败. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                        ret, nomalOad.logicId, nomalOad.infoNum, nomalOad.oad.value);
                    return DATA_DATA_BLOCK_UNAVAILABLE;
                }
                
                if (data.taskid != taskId)
                {
                    PRTL_FMT_DEBUG("读出来的数据不是要更新的 data.taskid[%d]\n", data.taskid);
                    return DATA_DATA_BLOCK_UNAVAILABLE;
                }
                
                data.priority = priority;
                
                memset(&nomalOad, 0, sizeof(nomalOad));
                nomalOad.logicId = 0x00;
                nomalOad.infoNum = data.taskid;
                nomalOad.oad.value = 0x6e110200;
                nomalOad.classtag = CLASS_DATA_INIT;
                
                ret = db_write_nomal(pApduInfo->hUdp, &nomalOad, (uint8*)&data, sizeof(data));
                if (ret != ERR_OK)
                {
                    PRTL_FMT_DEBUG("写入数据中心失败. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                        ret, nomalOad.logicId, nomalOad.infoNum, nomalOad.oad.value);
                    return DATA_READ_WRITE_DENIED;
                }

                update.transTaskid = taskId;
                update.priority = priority;
                memset(&nomalOad, 0, sizeof(nomalOad));
                nomalOad.logicId = 0x00;
                nomalOad.infoNum = update.transTaskid;
                nomalOad.oad.value = 0x6e11ffff;
                nomalOad.classtag = CLASS_DATA_INIT;
                
                ret = db_write_nomal(pApduInfo->hUdp, &nomalOad, (uint8*)&update, sizeof(update));
                if (ret != ERR_OK)
                {
                    PRTL_FMT_DEBUG("写入数据中心失败. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                        ret, nomalOad.logicId, nomalOad.infoNum, nomalOad.oad.value);
                    return DATA_READ_WRITE_DENIED;
                }
            }
        }

        memcpy_r((uint8*)outData->pResultData, (uint8*)&pOadInfo->pTab->oad.value, 4);
        if (ret == ERR_OK)
        {
            dar = DATA_SUCCESS;
            outData->pResultData[4] = DATA_SUCCESS;
            outData->pResultData[5] = 0x00;                          //DATA:NULL
            outData->resultDataLen = 0x06;
        }
        else
        {
            dar = DATA_READ_WRITE_DENIED;
            outData->pResultData[4] = DATA_READ_WRITE_DENIED;
            outData->resultDataLen = 0x05;
        }
    }
    break;
    default:
        break;
    }

    return dar;
}

#endif

#if DESC("全局透明具体任务读取", 1)
/**
*********************************************************************
* @brief：      销毁链表
* @param[in]： 集合链表头
* @return：     集合链表头
*********************************************************************
*/
LINK_TRANS_TASK_T* link_task_destroy(LINK_TRANS_TASK_T* pTaskHead)
{
    LINK_TRANS_TASK_T *tempTask = NULL;
    LINK_TRANS_TASK_T *pTask = pTaskHead;
    while (pTask)
    {
        tempTask = pTask->next;
        free(pTask);
        pTask = tempTask;
    }
    pTaskHead = NULL;
    return pTaskHead;
}

/**
*********************************************************************
* @name：       get_task_link_stat
* @brief：      获取任务方案
* @param[in] ：hUdp        数据中心句柄
* @param[out]: none
* @return    ：配置信息链表
* @author    :
* @Date      ：2019-12-6
*********************************************************************
*/
LINK_TRANS_TASK_T *get_task_link_stat(DB_CLIENT hUdp)
{
    int                 i = 0;
    int                 ret = ERR_OK;
    uint32              handle = 0;
    uint32              recNum = 0;
    uint16              oopTaskNum = 0;
    
    //链表相关变量
    LINK_TRANS_TASK_T* pMtrHead      = NULL;
    LINK_TRANS_TASK_T* pMtrTail      = pMtrHead;
    LINK_TRANS_TASK_T* pNewMtr       = pMtrHead;

    OOP_TRANS_TASK_T      oopTask;
    READ_RECORD_T         inReadRecord;
    STEP_READ_RECORD_T    stepReadRecord;
    RECORD_UNIT_T         stepRecordOut;

    memset(&stepReadRecord, 0, sizeof(stepReadRecord));
    memset(&stepRecordOut, 0, sizeof(stepRecordOut));
    memset(&inReadRecord, 0, sizeof(inReadRecord));
    memset(&oopTask, 0, sizeof(oopTask));

    //获取所有任务方案
    inReadRecord.recordOAD.infoNum = 0xffff;
    inReadRecord.cType = COL_NO_INFO;
    inReadRecord.cStart = 0;
    inReadRecord.cEnd = 0xffff;
    inReadRecord.sortType = UP_SORT;
    inReadRecord.recordOAD.optional = 0;
    inReadRecord.recordOAD.road.oadMain.value = 0;
    inReadRecord.recordOAD.road.oadCols.nNum = 0x01;
    inReadRecord.recordOAD.road.oadCols.oad[0].value = 0x6e110200;

    ret = db_read_record_start(hUdp, &inReadRecord, &handle, &recNum);
    if ((ret != ERR_OK) || (recNum == 0))
    {
        PRTL_FMT_DEBUG("任务方案开始读失败 ret = %d, infoNum = %d recNum = %d\n",
            ret, inReadRecord.recordOAD.infoNum, recNum);

        db_read_record_end(hUdp, handle);
        return NULL;
    }

    for (i = 0; i < recNum; i++)
    {
        ret = db_read_record_step(hUdp, handle, &stepReadRecord, &stepRecordOut);
        if (ret != ERR_OK)
        {
            PRTL_FMT_DEBUG("任务方案单步读成功 db_read_record_step err = %d!\n", ret);
            continue;
        }

        if ((stepRecordOut.recordnum != 1)
            || (stepRecordOut.record[0].road.oadCols.oad[0].value != 0x6e110200)
            || (stepRecordOut.record[0].outDatalen != sizeof(OOP_TRANS_TASK_T)))
        {
            PRTL_FMT_DEBUG("读任务方案失败!\n");
            PRTL_FMT_DEBUG("读任务方案 stepRecordOut.recordnum = %d\n", stepRecordOut.recordnum);
            PRTL_FMT_DEBUG("读任务方案 stepRecordOut.record[0].road.oadCols.oad[0] = 0x%08x\n",
                stepRecordOut.record[0].road.oadCols.oad[0].value);
            PRTL_FMT_DEBUG("读任务方案 stepRecordOut.record[0].outDatalen = %d\n",
                stepRecordOut.record[0].outDatalen);
            PRTL_FMT_DEBUG("读任务方案 sizeof(OOP_TRANS_TASK_T) = %d\n",
                sizeof(OOP_TRANS_TASK_T));
            continue;
        }

        memcpy(&oopTask, stepRecordOut.record[0].outDataBuf, stepRecordOut.record[0].outDatalen);
        PRTL_FMT_DEBUG("读到任务方案[%d]\n", stepRecordOut.infoNum);


        pNewMtr = (LINK_TRANS_TASK_T *)malloc(sizeof(LINK_TRANS_TASK_T));
        memset(pNewMtr, 0, sizeof(LINK_TRANS_TASK_T));
        pNewMtr->oopTask = oopTask;
        pNewMtr->no   = oopTaskNum ++;
        pNewMtr->next = NULL;
        if(!pMtrHead)
            pMtrHead = pNewMtr;
        if(pMtrTail)
            pMtrTail->next = pNewMtr;
        pMtrTail = pNewMtr;
    }

    db_read_record_end(hUdp, handle);
    
    return pMtrHead;
}

/**
*********************************************************************
* @brief：      读记录型具体任务
* @param[in]： inReadRecord  - 筛选需要的数据
               oopRcsd       - 列选择
               readRecNum    - 读记录数量，：0：读全部记录 1：读最近的一条记录
               maxBufLen     - 最大读取buf长度
* @param[out]：outRecordBuf  -  数据记录
* @return：     =0            - 成功
               <0            - 错误码
*********************************************************************
*/
int record_trans_node_read_by_node(DB_CLIENT hUdp, READ_INF_T inReadInf, RELY_INFO_T relyInfo, uint32 maxBufLen, RECORD_BUF_T *outRecordBuf)
{
    int       result = DATA_OTHER_REASON;
    uint32    offset = 0;
    uint32    i = 0;
    uint32    k = 0;
    int32     ret = ERR_NORMAL;
    OOP_DAR_E nRet = DATA_OTHER_REASON;
    uint32    bufAllLen = 0;
    BP_INFO_T breakInfo = {0};                                         //断点信息
    BOOL      isBufFull = FALSE;
    OAD_INFO_T *pOadInfo = relyInfo.pOadInfo;

    /*清除断点信息, 需要注意应用分帧的未帧*/
    memset(&breakInfo, 0, sizeof(BP_INFO_T));
   
    DATA_CONVERT_T        convert;
    OOP_TRANS_TASK_NODE_T data = {0};                                        //每个任务方案信息
    uint32 sOffset = 0;
    uint32 dOffset = 0;
    uint32 oopNodeNum = 0;
    uint32 oopNodeNumOffset = 0;

    READ_RECORD_T      inReadRecord;
    STEP_READ_RECORD_T stepReadRecord;
    RECORD_UNIT_T      stepRecordOut;
    uint32             handle = 0;
    uint32             recNum = 0;

    uint32             node_cnt = 0;

    MEMZERO(&convert, sizeof(convert));
    MEMZERO(&inReadRecord, sizeof(inReadRecord));

    oopNodeNumOffset = offset;                                         //档案数量偏移
    outRecordBuf->recordBuf[offset ++] = 0x00;                          //数量，先填0
    bufAllLen = offset;

    if(relyInfo.bpBreakInfo->appSlicingFlag == 1)
    {
        inReadInf.startTask = relyInfo.bpBreakInfo->index;
        node_cnt = relyInfo.bpBreakInfo->node_cnt;
    }

    for (i = inReadInf.startTask; i < inReadInf.endTask; i++)
    {
        inReadRecord.recordOAD.logicId = 0;
        inReadRecord.recordOAD.infoNum = i;
        inReadRecord.cType = COL_NOT;
        inReadRecord.cStart = 0;
        inReadRecord.cEnd = 0xffff;
        inReadRecord.sortType = UP_SORT;
        inReadRecord.recordOAD.road.oadMain.value = 0x6e130200;
        inReadRecord.recordOAD.road.oadCols.nNum = 0;
        inReadRecord.recordOAD.road.oadCols.oad[0].value = 0;
        inReadRecord.bTable = 1;
        inReadRecord.table.nNum = 0;

        stepReadRecord.MAC.nNum = 0;

        ret = db_read_record_start(hUdp, &inReadRecord, &handle, &recNum);
        if ((ret != ERR_OK) || (recNum == 0))
        {
            PRTL_FMT_DEBUG("db_read_record_start err! ret = %d, infoNum = %d recNum = %d node = %d\n", 
                ret, inReadRecord.recordOAD.infoNum, recNum, 0);
        
            db_read_record_end(hUdp, handle);
            continue;
        }

        PRTL_FMT_DEBUG("开始读成功, recNum[%d], task[%d]\n", recNum, i);

        for (k = 0; k < recNum; k++)
        {
            ret = db_read_record_step(hUdp, handle, &stepReadRecord, &stepRecordOut);
            if(ret != ERR_OK)
            {
                PRTL_FMT_DEBUG("db_read_record_step err = %d!\n", ret);
                continue;
            }

            if (k < node_cnt)
            {
                PRTL_FMT_LOGCC("跳过断点前已经处理过的单步信息 %d < index index = %d!\n", k, node_cnt);
                continue;
            }
            else
            {
                node_cnt = 0;
            }

            //OAD与返回数据
            if ((stepRecordOut.recordnum != 1) 
            || (stepRecordOut.record[0].road.oadMain.value != pOadInfo->pTab->oad.value)
            || (stepRecordOut.record[0].outDatalen != sizeof(data)))
            {
                PRTL_FMT_DEBUG("单步读到的数据错误!\n");
                PRTL_FMT_DEBUG("单步读数据 stepRecordOut.recordnum = %d\n", stepRecordOut.recordnum);
                PRTL_FMT_DEBUG("单步读数据 stepRecordOut.record[0].road.oadMain.value = 0x%08x\n", 
                    stepRecordOut.record[0].road.oadMain.value);
                PRTL_FMT_DEBUG("单步读数据 stepRecordOut.record[0].outDatalen = %d\n", 
                    stepRecordOut.record[0].outDatalen);
                PRTL_FMT_DEBUG("单步读数据 sizeof(OOP_TRANS_TASK_NODE_T) = %d\n", 
                    sizeof(data));
                continue;
            }

            memcpy(&data, stepRecordOut.record[0].outDataBuf, stepRecordOut.record[0].outDatalen);
            PRTL_FMT_LOGCC("找到具体任务序号为%d\n", data.nodeid);

            //转换成报文数据
            convert.srcBuf = &data;
            convert.srcLen = sizeof(data);
            convert.sOffset = &sOffset;

            convert.dstBuf = &outRecordBuf->recordBuf[offset];
            convert.dstLen = maxBufLen - offset;
            convert.dOffset = &dOffset;

            //记录断点信息
            breakInfo.index = i;
            breakInfo.node_cnt = k;

            nRet = data_to_buf_trans_task_node(pOadInfo, &convert);  //入参
            if (nRet == DATA_SUCCESS)
            {
                bufAllLen += dOffset;
                if (bufAllLen <= maxBufLen)
                {
                    memcpy(&outRecordBuf->recordBuf[offset], convert.dstBuf, dOffset);
                    offset += dOffset;
                    outRecordBuf->bufLen = offset;
                    PRTL_BUF_LOGCC(convert.dstBuf, dOffset, "第%d个具体任务报文:", i);
                    oopNodeNum ++;
                    if (bufAllLen + dOffset >= maxBufLen) //虽然本次没有超最大长度，但下次会
                    {
                        isBufFull = TRUE;
                        PRTL_FMT_DEBUG("虽然本次没有超最大长度，但下次会 isBufFull = [%d]\n", isBufFull);
                    }
                    dOffset = 0;
                    sOffset = 0;
                }
                else
                {
                    dOffset = 0;
                    sOffset = 0;
                    isBufFull = TRUE;
                    PRTL_FMT_DEBUG("达到最大报文长度 isBufFull = [%d]\n", isBufFull);
                }
            }
            else
            {
                continue;
            }

            if ((isBufFull) && (offset > 0))
            {
                break;
            }
        }

        //说明具体任务一次读完，具体任务断点应该要清空
        if (k == recNum)
        {
            breakInfo.node_cnt = 0;
        }
        
        if ((isBufFull) && (offset > 0))
        {
            break;
        }
    }

    db_read_record_end(hUdp, handle);

    if (!isBufFull)
    {
        if (relyInfo.bpBreakInfo->appSlicingFlag == 0x01)
        {
            breakInfo.appSlicingFlag = 0x01;
            breakInfo.isEnd = 0x01;                           //末尾帧
            breakInfo.seq = relyInfo.bpBreakInfo->seq + 1;    //序号加1
            breakInfo.objPre = 0x01;                          //对象属性
        }
        else
        {
            /*清除断点信息*/
            memset(&breakInfo, 0, sizeof(BP_INFO_T));
        }
    }
    else
    {
        breakInfo.reqType = 0x03;
        breakInfo.appSlicingFlag = 0x01;
        breakInfo.isEnd = 0x00;                              //不是末尾帧
        breakInfo.objPre = 0x01;                             //对象属性
        if (relyInfo.bpBreakInfo->appSlicingFlag == 0x00)
        {
            breakInfo.seq = BRK_SEQ_START;                   //首次序号
        }
        else
        {
            breakInfo.seq = relyInfo.bpBreakInfo->seq + 1;  //序号加1
        }
    }

    //刷新断点信息
    memcpy(&outRecordBuf->breakInfo, &breakInfo, sizeof(BP_INFO_T));

    PRTL_BUF_LOGCC(outRecordBuf->recordBuf, offset, "整个报文:");
    PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->appSlicingFlag = %d\n", outRecordBuf->breakInfo.appSlicingFlag);
    PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->index = %d\n", outRecordBuf->breakInfo.index);
    PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->isEnd = %d\n", outRecordBuf->breakInfo.isEnd);
    PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->objPre = %d\n", outRecordBuf->breakInfo.objPre);
    PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->seq = %d\n", outRecordBuf->breakInfo.seq);
    PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->reqType = %d\n", outRecordBuf->breakInfo.reqType);

    result = DATA_SUCCESS;
    outRecordBuf->recordBuf[oopNodeNumOffset] = oopNodeNum;   //重新记档案数量
    outRecordBuf->bufLen = offset;

    return result;
}

/**
*********************************************************************
* @brief：      读记录型方案任务
* @param[in]： inReadRecord  - 筛选需要的数据
               oopRcsd       - 列选择
               readRecNum    - 读记录数量，：0：读全部记录 1：读最近的一条记录
               maxBufLen     - 最大读取buf长度
* @param[out]：outRecordBuf  -  数据记录
* @return：     =0            - 成功
               <0            - 错误码
*********************************************************************
*/
int record_trans_node_read_by_ms(DB_CLIENT hUdp, READ_INF_T inReadInf, RELY_INFO_T relyInfo, uint32 maxBufLen, RECORD_BUF_T *outRecordBuf)
{
    int       result = DATA_OTHER_REASON;
    uint32    offset = 0;
    uint32    i = 0;
    uint32    j = 0;
    int32     ret = ERR_NORMAL;
    OOP_DAR_E nRet = DATA_OTHER_REASON;
    uint32    bufAllLen = 0;
    BP_INFO_T breakInfo = {0};                                         //断点信息
    BOOL      isBufFull = FALSE;
    OAD_INFO_T *pOadInfo = relyInfo.pOadInfo;

    /*清除断点信息, 需要注意应用分帧的未帧*/
    memset(&breakInfo, 0, sizeof(BP_INFO_T));
   
    DATA_CONVERT_T        convert;
    OOP_TRANS_TASK_NODE_T data = {0};                                  //每个任务方案信息
    uint32 sOffset = 0;
    uint32 dOffset = 0;
    uint32 oopMeterNum = 0;
    uint32 oopMeterNumOffset = 0;
    uint32 node_cnt = 0;

    READ_RECORD_T      inReadRecord;
    STEP_READ_RECORD_T stepReadRecord;
    RECORD_UNIT_T      stepRecordOut;
    uint32             handle = 0;
    uint32             recNum = 0;

    MEMZERO(&convert, sizeof(convert));
    MEMZERO(&inReadRecord, sizeof(inReadRecord));

    oopMeterNumOffset = offset;                                         //档案数量偏移
    outRecordBuf->recordBuf[offset ++] = 0x00;                          //数量，先填0
    bufAllLen = offset;

    
    if(relyInfo.bpBreakInfo->appSlicingFlag == 1)
    {
        inReadInf.startTask = relyInfo.bpBreakInfo->index;
        node_cnt = relyInfo.bpBreakInfo->node_cnt;
    }

    for (i = inReadInf.startTask; i < inReadInf.endTask; i++)
    {
        inReadRecord.recordOAD.logicId = 0;
        inReadRecord.recordOAD.infoNum = i;
        inReadRecord.cType = COL_NOT;
        inReadRecord.cStart = 0;
        inReadRecord.cEnd = 0xffff;
        inReadRecord.sortType = UP_SORT;
        inReadRecord.recordOAD.road.oadMain.value = 0x6e130200;
        inReadRecord.recordOAD.road.oadCols.nNum = 0x0;

        stepReadRecord.MAC.nNum = 0;

        ret = db_read_record_start(hUdp, &inReadRecord, &handle, &recNum);
        if ((ret != ERR_OK) || (recNum == 0))
        {
            PRTL_FMT_DEBUG("db_read_record_start err! ret = %d, infoNum = %d recNum = %d\n", 
                ret, inReadRecord.recordOAD.infoNum, recNum);

            db_read_record_end(hUdp, handle);
            return ret;
        }

        for (j = 0; j < recNum; j++)
        {
            ret = db_read_record_step(hUdp, handle, &stepReadRecord, &stepRecordOut);
            if(ret != ERR_OK)
            {
                PRTL_FMT_DEBUG("db_read_record_step err = %d!\n", ret);
                continue;
            }

            if (j < node_cnt)
            {
                continue;
            }
            else
            {
                node_cnt = 0;
            }

            //OAD与返回数据
            if ((stepRecordOut.recordnum != 1) 
            || (stepRecordOut.record[0].road.oadCols.oad[0].value != pOadInfo->pTab->oad.value)
            || (stepRecordOut.record[0].outDatalen != sizeof(data)))
            {
                PRTL_FMT_DEBUG("单步读到的数据错误!\n");
                PRTL_FMT_DEBUG("单步读数据 stepRecordOut.recordnum = %d\n", stepRecordOut.recordnum);
                PRTL_FMT_DEBUG("单步读数据 stepRecordOut.record[0].road.oadMain.value = 0x%08x\n", 
                    stepRecordOut.record[0].road.oadMain.value);
                PRTL_FMT_DEBUG("单步读数据 stepRecordOut.record[0].outDatalen = %d\n", 
                    stepRecordOut.record[0].outDatalen);
                PRTL_FMT_DEBUG("单步读数据 sizeof(OOP_TRANS_TASK_NODE_T) = %d\n", 
                    sizeof(data));
                continue;
            }

            memcpy(&data, stepRecordOut.record[0].outDataBuf, stepRecordOut.record[0].outDatalen);

            //档案不符合筛选条件
            if (!class11_memcmp_group_meter(&data.ms, &inReadInf.ms))
            {
                continue;
            }
            
            PRTL_FMT_LOGCC("找到具体任务序号为%d\n", data.nodeid);

            //转换成报文数据
            convert.srcBuf = &data;
            convert.srcLen = sizeof(data);
            convert.sOffset = &sOffset;

            convert.dstBuf = &outRecordBuf->recordBuf[offset];
            convert.dstLen = maxBufLen - offset;
            convert.dOffset = &dOffset;

            //记录断点信息
            breakInfo.index = i;
            breakInfo.node_cnt = j;

            nRet = data_to_buf_trans_task_node(pOadInfo, &convert);  //入参
            if (nRet == DATA_SUCCESS)
            {
                bufAllLen += dOffset;
                if (bufAllLen <= maxBufLen)
                {
                    memcpy(&outRecordBuf->recordBuf[offset], convert.dstBuf, dOffset);
                    offset += dOffset;
                    outRecordBuf->bufLen = offset;
                    PRTL_BUF_LOGCC(convert.dstBuf, dOffset, "第%d个具体任务报文:", i);
                    oopMeterNum ++;
                    if (bufAllLen + dOffset >= maxBufLen) //虽然本次没有超最大长度，但下次会
                    {
                        isBufFull = TRUE;
                        PRTL_FMT_DEBUG("虽然本次没有超最大长度，但下次会 isBufFull = [%d]\n", isBufFull);
                    }
                    dOffset = 0;
                    sOffset = 0;
                }
                else
                {
                    dOffset = 0;
                    sOffset = 0;
                    isBufFull = TRUE;
                    PRTL_FMT_DEBUG("达到最大报文长度 isBufFull = [%d]\n", isBufFull);
                }
            }
            else
            {
                continue;
            }

            if ((isBufFull) && (offset > 0))
            {
                break;
            }
        }
    }

    db_read_record_end(hUdp, handle);
    
    if (!isBufFull)
    {
        if (relyInfo.bpBreakInfo->appSlicingFlag == 0x01)
        {
            breakInfo.appSlicingFlag = 0x01;
            breakInfo.isEnd = 0x01;                           //末尾帧
            breakInfo.seq = relyInfo.bpBreakInfo->seq + 1;    //序号加1
            breakInfo.objPre = 0x01;                          //对象属性
        }
        else
        {
            /*清除断点信息*/
            memset(&breakInfo, 0, sizeof(BP_INFO_T));
        }
    }
    else
    {
        breakInfo.reqType = 0x03;
        breakInfo.appSlicingFlag = 0x01;
        breakInfo.isEnd = 0x00;                              //不是末尾帧
        breakInfo.objPre = 0x01;                             //对象属性
        if (relyInfo.bpBreakInfo->appSlicingFlag == 0x00)
        {
            breakInfo.seq = BRK_SEQ_START;                   //首次序号
        }
        else
        {
            breakInfo.seq = relyInfo.bpBreakInfo->seq + 1;  //序号加1
        }
    }

    //刷新断点信息
    memcpy(&outRecordBuf->breakInfo, &breakInfo, sizeof(BP_INFO_T));

    PRTL_BUF_LOGCC(outRecordBuf->recordBuf, offset, "整个报文:");
    PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->appSlicingFlag = %d\n", outRecordBuf->breakInfo.appSlicingFlag);
    PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->index = %d\n", outRecordBuf->breakInfo.index);
    PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->isEnd = %d\n", outRecordBuf->breakInfo.isEnd);
    PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->objPre = %d\n", outRecordBuf->breakInfo.objPre);
    PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->seq = %d\n", outRecordBuf->breakInfo.seq);
    PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->reqType = %d\n", outRecordBuf->breakInfo.reqType);

    result = DATA_SUCCESS;
    outRecordBuf->recordBuf[oopMeterNumOffset] = oopMeterNum;   //重新记档案数量
    outRecordBuf->bufLen = offset;

    return result;
}


/**
*********************************************************************
* @brief：      读记录型方案任务
* @param[in]： inReadRecord  - 筛选需要的数据
               oopRcsd       - 列选择
               readRecNum    - 读记录数量，：0：读全部记录 1：读最近的一条记录
               maxBufLen     - 最大读取buf长度
* @param[out]：outRecordBuf  -  数据记录
* @return：     =0            - 成功
               <0            - 错误码
*********************************************************************
*/
int record_trans_node_read(DB_CLIENT hUdp, READ_INF_T inReadInf, RELY_INFO_T relyInfo, uint32 maxBufLen, 
    RECORD_BUF_T *outRecordBuf)
{
    if (inReadInf.startNode > 0)
    {
        //按具体任务筛选
        return record_trans_node_read_by_node(hUdp, inReadInf, relyInfo, maxBufLen, outRecordBuf);
    }
    else if (inReadInf.ms.choice == 0x06)
    {
        //按ms集合筛选
        return record_trans_node_read_by_ms(hUdp, inReadInf, relyInfo, maxBufLen, outRecordBuf);
    }
    return ERR_OK;
}


/**
*********************************************************************
* @name：      class11_invoke_get_transparent_node_all_nor
* @brief：     class11的GET服务
* @param[in] ：pOdaInfo        OAD信息
               pBpInfo        应用分帧断点信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
               outDataMax     输出报文缓冲区最大长度

* @param[out]：outData     输出报文
* @return    ：OOP_DAR_E
* @author    :
* @Date      ：2019-12-6
*********************************************************************
*/
#if 0
OOP_DAR_E class11_invoke_get_transparent_node_all_nor(OAD_INFO_T* pOadInfo, APDU_INFO_T* pApduInfo, 
    const uint8* inData, uint16 inLen, RESULT_DATA_T* outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    DATA_CONVERT_T convert;
    uint32 sOffset = 0;
    uint32 dOffset = 0;
    uint32 offset = 0;
    uint32             i = 0;
    READ_RECORD_T      inReadRecord;
    uint32             handle = 0;
    uint32             recNum = 0;
    STEP_READ_RECORD_T stepReadRecord;
    RECORD_UNIT_T      stepRecordOut;
    OOP_TRANS_TASK_NODE_T   data;
    uint32    realNum = 0;    //数组由于分帧导致的每帧实际成员数
    uint32    numOffset = 0;  //数组成员数在报文中的位置
    uint32    bufAllLen = 0;
    BP_INFO_T breakInfo = { 0 };                                                //断点信息
    uint32    maxBufLen = 0;
    BOOLEAN   isBufFull = FALSE;

    uint32    oopTaskIndex = 0;
    uint32    oopNodeIndex = 0;

    LINK_TRANS_TASK_T *oopTaskLink = NULL;
    LINK_TRANS_TASK_T *oopTaskLinkHead = NULL;

    maxBufLen = pApduInfo->connectionInfo.ApplSplitLen - cal_excludeLen(pApduInfo->connectionInfo);
    memset(&breakInfo, 0, sizeof(BP_INFO_T));
    memset(&data, 0, sizeof(data));
    MEMZERO(&convert, sizeof(convert));

    memset(&inReadRecord, 0, sizeof(READ_RECORD_T));
    memset(&stepReadRecord, 0, sizeof(STEP_READ_RECORD_T));
    memset(&stepRecordOut, 0, sizeof(RECORD_UNIT_T));
    //读出数据
    inReadRecord.recordOAD.logicId = 0;

    oopTaskLink = get_task_link_stat(pApduInfo->hUdp);
    if (oopTaskLink == NULL)
    {
        return DATA_READ_WRITE_DENIED;
    }
    else
    {
        oopTaskLinkHead = oopTaskLink;
    }

    if (pApduInfo->bpBreakInfo->appSlicingFlag == 1)
    {
        oopTaskIndex = pApduInfo->bpBreakInfo->recNumSeg;
        oopNodeIndex = pApduInfo->bpBreakInfo->index;
    }

    while (oopTaskLink)
    {
        if (oopTaskIndex > oopTaskLink->no)
        {
            oopTaskLink = oopTaskLink->next;
            continue;
        }
        else
        {
            oopTaskIndex = 0;
        }
        
        inReadRecord.recordOAD.infoNum = oopTaskLink->oopTask.taskid;
        inReadRecord.cType = COL_NOT;
        inReadRecord.cStart = 0;
        inReadRecord.cEnd = 0xffff;
        inReadRecord.sortType = UP_SORT;
        inReadRecord.recordOAD.road.oadMain.value = 0x6e130200;
        inReadRecord.recordOAD.road.oadCols.nNum = 0;
        inReadRecord.bTable = 1;
        inReadRecord.table.nNum = 0;
        stepReadRecord.MAC.nNum = 0;

        memcpy(&outData->pResultData[offset], inData, sizeof(OOP_OAD_U));   //拷贝OAD
        offset += sizeof(OOP_OAD_U);
        outData->pResultData[offset++] = 0x01;                              //结果

        outData->pResultData[offset++] = DT_ARRAY;                          //类型
        numOffset = offset;                                                 //数量偏移
        outData->pResultData[offset++] = 0x00;                              //数量，先填0

        bufAllLen = offset;
        ret = db_read_record_start(pApduInfo->hUdp, &inReadRecord, &handle, &recNum);
        if ((ret != ERR_OK) || (recNum == 0))
        {
            PRTL_FMT_DEBUG("具体任务开始读失败 ret = %d, infoNum = %d recNum = %d\n",
                ret, inReadRecord.recordOAD.infoNum, recNum);
        }
        else
        {
            PRTL_FMT_DEBUG("具体任务开始读成功 ret = %d, infoNum = %d recNum = %d\n",
                ret, inReadRecord.recordOAD.infoNum, recNum);

            for (i = 0; i < recNum; i++)
            {
                if (i < oopNodeIndex)
                {
                    continue;
                }
                
                ret = db_read_record_step(pApduInfo->hUdp, handle, &stepReadRecord, &stepRecordOut);
                if (ret != ERR_OK)
                {
                    PRTL_FMT_DEBUG("具体任务单步读失败 db_read_record_step err = %d!\n", ret);
                    continue;
                    //数据中心出错返回NULL
                }

                //OAD与返回数据
                if ((stepRecordOut.recordnum != 1)
                    || (stepRecordOut.record[0].road.oadMain.value != pOadInfo->pTab->oad.value)
                    || (stepRecordOut.record[0].outDatalen != sizeof(OOP_TRANS_TASK_NODE_T)))
                {
                    PRTL_FMT_DEBUG("读具体任务失败!\n");
                    PRTL_FMT_DEBUG("读具体任务 stepRecordOut.recordnum = %d\n", stepRecordOut.recordnum);
                    PRTL_FMT_DEBUG("读具体任务 stepRecordOut.record[0].road.oadMain.value = 0x%08x\n",
                        stepRecordOut.record[0].road.oadMain.value);
                    PRTL_FMT_DEBUG("读具体任务 stepRecordOut.record[0].outDatalen = %d\n",
                        stepRecordOut.record[0].outDatalen);
                    PRTL_FMT_DEBUG("读具体任务 sizeof(OOP_TRANS_TASK_NODE_T) = %d\n",
                        sizeof(OOP_TRANS_TASK_NODE_T));
                    continue;
                }

                memcpy(&data, stepRecordOut.record[0].outDataBuf, stepRecordOut.record[0].outDatalen);
                PRTL_FMT_DEBUG("stepRecordOut.infoNum = %d\n", stepRecordOut.infoNum);

                //转换成报文数据
                convert.srcBuf = &data;
                convert.srcLen = sizeof(data);
                convert.sOffset = &sOffset;

                convert.dstBuf = &outData->pResultData[offset];
                convert.dstLen = outDataMax - offset;
                convert.dOffset = &dOffset;

                //实时记录断点
                breakInfo.recNumSeg = oopTaskLink->no;
                breakInfo.index = i;
                
                dar = data_to_buf_trans_task_node(pOadInfo, &convert);
                if (dar == DATA_SUCCESS)
                {
                    bufAllLen += dOffset;
                    if (bufAllLen <= maxBufLen)
                    {
                        memcpy(&outData->pResultData[offset], convert.dstBuf, dOffset);
                        offset += dOffset;
                        outData->resultDataLen = offset;
                        PRTL_BUF_DEBUG(convert.dstBuf, dOffset, "第%d个任务方案:", i);
                        realNum++;
                        dOffset = 0;
                        sOffset = 0;
                    }
                    else
                    {
                        dOffset = 0;
                        sOffset = 0;
                        isBufFull = TRUE;
                    }
                }
                else
                {
                    continue;
                }
                if ((isBufFull) && (offset > 0))
                {
                    break;
                }
            }
        }
        
        if ((isBufFull) && (offset > 0))
        {
            break;
        }
        
        db_read_record_end(pApduInfo->hUdp, handle);

        oopTaskLink = oopTaskLink->next;
    }
    
    if (!isBufFull)
    {
        if (pApduInfo->bpBreakInfo->appSlicingFlag == 0x01)
        {
            breakInfo.appSlicingFlag = 0x01;
            breakInfo.isEnd = 0x01;                           //末尾帧
            breakInfo.seq = pApduInfo->bpBreakInfo->seq + 1;  //序号加1
            breakInfo.objPre = 0x01;                          //对象属性
        }
        else
        {
            memset(&breakInfo, 0, sizeof(BP_INFO_T));
        }
    }
    else
    {
        breakInfo.reqType = pApduInfo->reqType;
        breakInfo.appSlicingFlag = 0x01;
        breakInfo.isEnd = 0x00;                              //不是末尾帧
        breakInfo.objPre = 0x01;                             //对象属性
        if (pApduInfo->bpBreakInfo->appSlicingFlag == 0x00)
        {
            breakInfo.seq = BRK_SEQ_START;                   //首次序号
        }
        else
        {
            breakInfo.seq = pApduInfo->bpBreakInfo->seq + 1;  //序号加1
        }
    }
    memcpy(pApduInfo->bpBreakInfo, &breakInfo, sizeof(BP_INFO_T));

    if (pOadInfo->nIndex == 0)
    {
        outData->pResultData[numOffset] = realNum;   //重新记数量
    }

    outData->resultDataLen = offset;

    link_task_destroy(oopTaskLinkHead);

    return dar;
}
#else
OOP_DAR_E class11_invoke_get_transparent_node_all_nor(OAD_INFO_T* pOadInfo, APDU_INFO_T* pApduInfo, 
    const uint8* inData, uint16 inLen, RESULT_DATA_T* outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    DATA_CONVERT_T convert;
    uint32 sOffset = 0;
    uint32 dOffset = 0;
    uint32 offset = 0;
    uint32             i = 0;
    READ_RECORD_T      inReadRecord;
    uint32             handle = 0;
    uint32             recNum = 0;
    STEP_READ_RECORD_T stepReadRecord;
    RECORD_UNIT_T      stepRecordOut;
    OOP_TRANS_TASK_NODE_T   data;
    uint32    realNum = 0;    //数组由于分帧导致的每帧实际成员数
    uint32    numOffset = 0;  //数组成员数在报文中的位置
    uint32    bufAllLen = 0;
    BP_INFO_T breakInfo = { 0 };                                                //断点信息
    uint32    maxBufLen = 0;
    BOOLEAN   isBufFull = FALSE;

    uint32    oopNodeIndex = 0;

    maxBufLen = pApduInfo->connectionInfo.ApplSplitLen - cal_excludeLen(pApduInfo->connectionInfo);
    memset(&breakInfo, 0, sizeof(BP_INFO_T));
    memset(&data, 0, sizeof(data));
    MEMZERO(&convert, sizeof(convert));

    memset(&inReadRecord, 0, sizeof(READ_RECORD_T));
    memset(&stepReadRecord, 0, sizeof(STEP_READ_RECORD_T));
    memset(&stepRecordOut, 0, sizeof(RECORD_UNIT_T));
    //读出数据
    inReadRecord.recordOAD.logicId = 0;

    if (pApduInfo->bpBreakInfo->appSlicingFlag == 1)
    {
        oopNodeIndex = pApduInfo->bpBreakInfo->index;
    }
    
    inReadRecord.recordOAD.infoNum = 0xffff;
    inReadRecord.cType = COL_NO_INFO;
    inReadRecord.cStart = 0;
    inReadRecord.cEnd = 0xffff;
    inReadRecord.sortType = UP_SORT;
    inReadRecord.recordOAD.road.oadMain.value = 0x6e130200;
    inReadRecord.recordOAD.road.oadCols.nNum = 0;
    inReadRecord.bTable = 1;
    inReadRecord.table.nNum = 0;
    stepReadRecord.MAC.nNum = 0;

    memcpy(&outData->pResultData[offset], inData, sizeof(OOP_OAD_U));   //拷贝OAD
    offset += sizeof(OOP_OAD_U);
    outData->pResultData[offset++] = 0x01;                              //结果

    outData->pResultData[offset++] = DT_ARRAY;                          //类型
    numOffset = offset;                                                 //数量偏移
    outData->pResultData[offset++] = 0x00;                              //数量，先填0

    bufAllLen = offset;
    ret = db_read_record_start(pApduInfo->hUdp, &inReadRecord, &handle, &recNum);
    if ((ret != ERR_OK) || (recNum == 0))
    {
        PRTL_FMT_DEBUG("具体任务开始读失败 ret = %d, infoNum = %d recNum = %d\n",
            ret, inReadRecord.recordOAD.infoNum, recNum);
    }
    else
    {
        PRTL_FMT_DEBUG("具体任务开始读成功 ret = %d, infoNum = %d recNum = %d\n",
            ret, inReadRecord.recordOAD.infoNum, recNum);

        for (i = 0; i < recNum; i++)
        {
            ret = db_read_record_step(pApduInfo->hUdp, handle, &stepReadRecord, &stepRecordOut);
            if (ret != ERR_OK)
            {
                PRTL_FMT_DEBUG("具体任务单步读失败 db_read_record_step err = %d!\n", ret);
                continue;
                //数据中心出错返回NULL
            }

            if (i < oopNodeIndex)
            {
                continue;
            }

            //OAD与返回数据
            if ((stepRecordOut.recordnum != 1)
                || (stepRecordOut.record[0].road.oadMain.value != pOadInfo->pTab->oad.value)
                || (stepRecordOut.record[0].outDatalen != sizeof(OOP_TRANS_TASK_NODE_T)))
            {
                PRTL_FMT_DEBUG("读具体任务失败!\n");
                PRTL_FMT_DEBUG("读具体任务 stepRecordOut.recordnum = %d\n", stepRecordOut.recordnum);
                PRTL_FMT_DEBUG("读具体任务 stepRecordOut.record[0].road.oadMain.value = 0x%08x\n",
                    stepRecordOut.record[0].road.oadMain.value);
                PRTL_FMT_DEBUG("读具体任务 stepRecordOut.record[0].outDatalen = %d\n",
                    stepRecordOut.record[0].outDatalen);
                PRTL_FMT_DEBUG("读具体任务 sizeof(OOP_TRANS_TASK_NODE_T) = %d\n",
                    sizeof(OOP_TRANS_TASK_NODE_T));
                continue;
            }

            memcpy(&data, stepRecordOut.record[0].outDataBuf, stepRecordOut.record[0].outDatalen);
            PRTL_FMT_DEBUG("stepRecordOut.infoNum = %d\n", stepRecordOut.infoNum);

            //转换成报文数据
            convert.srcBuf = &data;
            convert.srcLen = sizeof(data);
            convert.sOffset = &sOffset;

            convert.dstBuf = &outData->pResultData[offset];
            convert.dstLen = outDataMax - offset;
            convert.dOffset = &dOffset;

            //实时记录断点
            breakInfo.index = i;
            
            dar = data_to_buf_trans_task_node(pOadInfo, &convert);
            if (dar == DATA_SUCCESS)
            {
                bufAllLen += dOffset;
                if (bufAllLen <= maxBufLen)
                {
                    memcpy(&outData->pResultData[offset], convert.dstBuf, dOffset);
                    offset += dOffset;
                    outData->resultDataLen = offset;
                    PRTL_BUF_DEBUG(convert.dstBuf, dOffset, "第%d个具体任务:", i);
                    realNum++;
                    dOffset = 0;
                    sOffset = 0;
                }
                else
                {
                    dOffset = 0;
                    sOffset = 0;
                    isBufFull = TRUE;
                }
            }
            else
            {
                continue;
            }
            if ((isBufFull) && (offset > 0))
            {
                break;
            }
        }
    }
    
    db_read_record_end(pApduInfo->hUdp, handle);
    
    if (!isBufFull)
    {
        if (pApduInfo->bpBreakInfo->appSlicingFlag == 0x01)
        {
            breakInfo.appSlicingFlag = 0x01;
            breakInfo.isEnd = 0x01;                           //末尾帧
            breakInfo.seq = pApduInfo->bpBreakInfo->seq + 1;  //序号加1
            breakInfo.objPre = 0x01;                          //对象属性
        }
        else
        {
            memset(&breakInfo, 0, sizeof(BP_INFO_T));
        }
    }
    else
    {
        breakInfo.reqType = pApduInfo->reqType;
        breakInfo.appSlicingFlag = 0x01;
        breakInfo.isEnd = 0x00;                              //不是末尾帧
        breakInfo.objPre = 0x01;                             //对象属性
        if (pApduInfo->bpBreakInfo->appSlicingFlag == 0x00)
        {
            breakInfo.seq = BRK_SEQ_START;                   //首次序号
        }
        else
        {
            breakInfo.seq = pApduInfo->bpBreakInfo->seq + 1;  //序号加1
        }
    }
    memcpy(pApduInfo->bpBreakInfo, &breakInfo, sizeof(BP_INFO_T));

    if (pOadInfo->nIndex == 0)
    {
        outData->pResultData[numOffset] = realNum;   //重新记数量
    }

    outData->resultDataLen = offset;

    return dar;
}
#endif

/**
*********************************************************************
* @name：      class11_invoke_get_oop_meter_record
* @brief：     class11的GET服务
* @param[in] ：pOdaInfo        OAD信息
               pBpInfo        应用分帧断点信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
               outDataMax     输出报文缓冲区最大长度

* @param[out]：outData     输出报文
* @return    ：OOP_DAR_E
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E class11_invoke_get_transparent_node_all_rec(OAD_INFO_T *pOdaInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    uint16 i = 0;
    uint16 j = 0;
    OOP_DAR_E  nRet = DATA_OTHER_REASON;
    uint32 offset = 4;
    uint16 csdStart = 0, csdLen = 0;
    uint16 outOffset = 0;
    uint16 inDataLenTmp = InDataLen;

    OOP_PRSD_T oopRsd;
    OOP_RCSD_T oopRcsd;
    uint16     headLen = 0;
    READ_INF_T inReadInf;
    RELY_INFO_T  relyInfo;
    RECORD_BUF_T outRecordBuf;

    /* 清0处理 */
    MEMZERO(&oopRsd, sizeof(oopRsd));
    MEMZERO(&oopRcsd, sizeof(oopRcsd));
    MEMZERO(&inReadInf, sizeof(inReadInf));
    MEMZERO(&relyInfo, sizeof(relyInfo));
    MEMZERO(&outRecordBuf, sizeof(outRecordBuf));

    outRecordBuf.recordBuf = (uint8 *)malloc(pApduInfo->connectionInfo.ApplSplitLen);
    memset(outRecordBuf.recordBuf, 0xee, pApduInfo->connectionInfo.ApplSplitLen);

    /*处理RSD*/
    nRet = apdu_rsd_fromfmt698(inData, inDataLenTmp, &offset, &oopRsd, sizeof(oopRsd));
    PRTL_FMT_LOGCC("oopRsd.choice = %d\n", oopRsd.choice);
    PRTL_FMT_LOGCC("oopRsd.sel1.oad.value = 0x%08x\n", oopRsd.sel1.oad.value);
    PRTL_FMT_LOGCC("oopRsd.sel1.value.wVal = %d\n", oopRsd.sel1.value.wVal);
    PRTL_FMT_LOGCC("offset = %d\n", offset);
    if (nRet != DATA_SUCCESS)
    {
        PRTL_FMT_TRACE("RSD解析错误\n");

        /*应对电科院协议一致性测试*/
        uint16 testOffset = 4;
        uint16 inCsdOff = 0, inCsdLen = 0;
        apdu_next_dpos_get(inData, &testOffset, InDataLen, DT_RSD, TRUE);
        inCsdOff = testOffset;
        apdu_next_dpos_get(inData, &testOffset, InDataLen, DT_RCSD, TRUE);
        inCsdLen = testOffset - inCsdOff;
        memcpy(outData->pResultData, inData, 4);
        memcpy(outData->pResultData + 4, inData + inCsdOff, inCsdLen);
        outData->resultDataLen = 4 + inCsdLen;

        outData->pResultData[outData->resultDataLen++] = 1;                   //后面优化下，outData->resultDataLen与下面统一
        if (apdu_rcsd_fromfmt698((uint8 *)inData, InDataLen, &offset, &oopRcsd, sizeof(oopRcsd)) != DATA_SUCCESS)
        {
            outData->pResultData[outData->resultDataLen++] = 0;

        }
        else
        {
            outData->pResultData[outData->resultDataLen++] = 1;
            for (i = 0; i < oopRcsd.nNum; i++)
            {
                outData->pResultData[outData->resultDataLen++] = NULL_DATA;
            }
        }

        free(outRecordBuf.recordBuf);
        outRecordBuf.recordBuf = NULL;
        return DATA_SUCCESS;
    }
    else
    {
        inDataLenTmp -= offset;  //除去RSD和OAD后的长度
    }

    /*处理RCSD*/
    csdStart = offset;
    if (apdu_rcsd_fromfmt698(inData, InDataLen, &offset, &oopRcsd, sizeof(oopRcsd)) != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("CSD解析错误!\n");
        free(outRecordBuf.recordBuf);
        outRecordBuf.recordBuf = NULL;
        return DATA_TYPE_UNMATCHED;
    }

    /*填写OAD+RCSD+记录结果1 并记下偏移headLen*/
    outData->resultDataLen = 0;
    csdLen = offset-csdStart;
    memcpy(outData->pResultData, inData, 4);
    outOffset = 4;

    if (csdLen == 1)
    {
        //如果rcsd为空，填充OAD6e120200
        outData->pResultData[outOffset ++] = 0x01;
        outData->pResultData[outOffset ++] = 0x00;
        outData->pResultData[outOffset ++] = 0x6e;
        outData->pResultData[outOffset ++] = 0x14;
        outData->pResultData[outOffset ++] = 0x02;
        outData->pResultData[outOffset ++] = 0x00;
    }
    else
    {
        memcpy(&outData->pResultData[outOffset], &inData[csdStart], csdLen);
        outOffset += csdLen;
    }
    outData->pResultData[outOffset++] = 1;
    outData->resultDataLen += outOffset;
    headLen = outOffset;
    PRTL_BUF_LOGCC(&outData->pResultData[0], headLen, "填写OAD+RCSD+记录结果1 并记下偏移headLen head:");

    /*记录型档案接口提供输入参数*/
    if (general_param_get_node(&oopRsd, &oopRcsd, pApduInfo, &inReadInf, &relyInfo) != ERR_OK)
    {
        free(outRecordBuf.recordBuf);
        outRecordBuf.recordBuf = NULL;
        return DATA_READ_WRITE_DENIED;
    }

    /* 依赖信息中存下OAD列表的指针 */
    relyInfo.pOadInfo = pOdaInfo;

    PRTL_FMT_LOGCC("****************************************************\n");
    PRTL_FMT_LOGCC("inReadMeter.startInfoNum = %d\n", inReadInf.startTask);
    PRTL_FMT_LOGCC("inReadMeter.endInfoNum = %d\n", inReadInf.endTask);
    PRTL_FMT_LOGCC("relyInfo.oopRcsd.nNum = %d\n", relyInfo.oopRcsd.nNum);
    for(i = 0; i < relyInfo.oopRcsd.nNum; i++)
    {
        if (relyInfo.oopRcsd.cols[i].choice == 0)
        {
            PRTL_FMT_LOGCC("choice = %d relyInfo.oopRcsd.cols[%d].oad.value = 0x%08x\n", i, relyInfo.oopRcsd.cols[i].choice, 
                                                                                      relyInfo.oopRcsd.cols[i].oad.value);
        }
        else if(relyInfo.oopRcsd.cols[i].choice == 1)
        {
            PRTL_FMT_LOGCC("choice = %d relyInfo.oopRcsd.cols[%d].road.oadMain.value = 0x%08x\n", i, relyInfo.oopRcsd.cols[i].choice, 
                                                                                              relyInfo.oopRcsd.cols[i].road.oadMain.value);
            for (j = 0; j < relyInfo.oopRcsd.cols[i].road.oadCols.nNum; j++)
            {
                PRTL_FMT_LOGCC("choice = %d relyInfo.oopRcsd.cols[%d].road.oadCols.oad[%d].value = 0x%08x\n", i, j, relyInfo.oopRcsd.cols[i].choice, 
                                                                                                          relyInfo.oopRcsd.cols[i].road.oadCols.oad[j].value);
            }
        }
    }
    PRTL_FMT_LOGCC("****************************************************\n");

    /*读记录档案接口，可以读出apdu数据域，可返回断点信息*/
    if (record_trans_node_read(pApduInfo->hUdp, inReadInf, relyInfo, pApduInfo->connectionInfo.ApplSplitLen - headLen - cal_excludeLen(pApduInfo->connectionInfo), &outRecordBuf))
    {
        nRet = DATA_READ_WRITE_DENIED;
    }
    else
    {
        memcpy(&outData->pResultData[headLen], outRecordBuf.recordBuf, outRecordBuf.bufLen);
        outData->resultDataLen += outRecordBuf.bufLen;

        if (outRecordBuf.breakInfo.appSlicingFlag == 1)
        {
            /*记下断点信息*/
            PRTL_FMT_LOGCC("记下断点信息\n");
            outRecordBuf.breakInfo.objPre = 0x02;   //对象属性，记录型数据
            memcpy(pApduInfo->bpBreakInfo, &outRecordBuf.breakInfo, sizeof(BP_INFO_T));
        }
        else
        {
            /*清除断点信息*/
            PRTL_FMT_LOGCC("清除断点信息\n");
            memset(pApduInfo->bpBreakInfo, 0, sizeof(BP_INFO_T));
        }
        
        nRet = DATA_SUCCESS;
    }
    
    free(outRecordBuf.recordBuf);
    outRecordBuf.recordBuf = NULL;
    return DATA_SUCCESS;
}


/**
*********************************************************************
* @name：      class11_invoke_get_transparent_node_all
* @brief：     class11的GET服务
* @param[in] ：pOdaInfo        OAD信息
               pBpInfo        应用分帧断点信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
               outDataMax     输出报文缓冲区最大长度

* @param[out]：outData     输出报文
* @return    ：OOP_DAR_E
* @author    :
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E class11_invoke_get_transparent_node_all(OAD_INFO_T* pOadInfo, APDU_INFO_T* pApduInfo, const uint8* inData,
    uint16 inLen, RESULT_DATA_T* outData, uint16 outDataMax)
{
    //如果是应用分帧状态，将apdu[1]修改成起始帧方式
    if (pApduInfo->bpBreakInfo->appSlicingFlag == 1)
    {
        pApduInfo->reqType = pApduInfo->bpBreakInfo->reqType;
    }

    switch (pApduInfo->reqType)
    {
    case 0x01:
    case 0x02:
        return class11_invoke_get_transparent_node_all_nor(pOadInfo, pApduInfo, inData, inLen, outData, outDataMax);
        break;
    case 0x03:
    case 0x04:
        return class11_invoke_get_transparent_node_all_rec(pOadInfo, pApduInfo, inData, inLen, outData, outDataMax);
        break;
    default:
        break;
    }

    return DATA_OBJECT_UNDEFINED;
}
#endif

#if DESC("全局透明具体任务操作", 1)
/**
*********************************************************************
* @brief：      销毁链表
* @param[in]： 集合链表头
* @return：     集合链表头
*********************************************************************
*/
LINK_TRANS_NODE_T* link_node_destroy(LINK_TRANS_NODE_T* pNodeHead)
{
    LINK_TRANS_NODE_T *tempNode = NULL;
    LINK_TRANS_NODE_T *pNode = pNodeHead;
    while (pNode)
    {
        tempNode = pNode->next;
        free(pNode);
        pNode = tempNode;
    }
    pNodeHead = NULL;
    return pNodeHead;
}

/**
*********************************************************************
* @name：       get_node_link_stat
* @brief：      获取具体任务
* @param[in] ：hUdp        数据中心句柄
               taskId.....任务方案，FFFF表示所有任务方案
               nodeId.....具体任务  0   表示所有具体任务
* @param[out]: none
* @return    ：配置信息链表
* @author    :
* @Date      ：2019-12-6
*********************************************************************
*/
LINK_TRANS_NODE_T *get_node_link_stat(uint16 taskId, uint16 nodeId, DB_CLIENT hUdp)
{
    int                 i = 0;
    int                 ret = ERR_OK;
    uint32              handle = 0;
    uint32              recNum = 0;
    uint16              oopNodeNum = 0;
    
    //链表相关变量
    LINK_TRANS_NODE_T* pMtrHead      = NULL;
    LINK_TRANS_NODE_T* pMtrTail      = pMtrHead;
    LINK_TRANS_NODE_T* pNewMtr       = pMtrHead;

    OOP_TRANS_TASK_NODE_T oopNode;
    READ_RECORD_T         inReadRecord;
    STEP_READ_RECORD_T    stepReadRecord;
    RECORD_UNIT_T         stepRecordOut;

    memset(&stepReadRecord, 0, sizeof(stepReadRecord));
    memset(&stepRecordOut, 0, sizeof(stepRecordOut));
    memset(&inReadRecord, 0, sizeof(inReadRecord));
    memset(&oopNode, 0, sizeof(oopNode));

    //获取所有任务方案
    inReadRecord.recordOAD.infoNum = taskId;
    inReadRecord.cType = COL_NOT;
    inReadRecord.cStart = 0;
    inReadRecord.cEnd = 0xffff;
    inReadRecord.sortType = UP_SORT;
    inReadRecord.recordOAD.optional = 0;
    inReadRecord.recordOAD.road.oadMain.value = 0x6e130200;
    inReadRecord.recordOAD.road.oadCols.nNum = 0;
    inReadRecord.bTable = 1;
    inReadRecord.table.nNum = 0;

    //次DI为0，表示所有具体任务，否则筛选具体某个任务
    if (nodeId != 0)
    {
        inReadRecord.recordOAD.road.oadCols.nNum = 1;
        inReadRecord.recordOAD.road.oadCols.oad[0].value = nodeId;
    }

    ret = db_read_record_start(hUdp, &inReadRecord, &handle, &recNum);
    if ((ret != ERR_OK) || (recNum == 0))
    {
        PRTL_FMT_DEBUG("具体任务开始读失败 ret = %d, infoNum = %d recNum = %d\n",
            ret, inReadRecord.recordOAD.infoNum, recNum);

        db_read_record_end(hUdp, handle);
        return NULL;
    }

    for (i = 0; i < recNum; i++)
    {
        ret = db_read_record_step(hUdp, handle, &stepReadRecord, &stepRecordOut);
        if (ret != ERR_OK)
        {
            PRTL_FMT_DEBUG("具体任务单步读失败 db_read_record_step err = %d!\n", ret);
            continue;
        }

        if ((stepRecordOut.recordnum != 1)
            || (stepRecordOut.record[0].road.oadMain.value != 0x6e130200)
            || (stepRecordOut.record[0].outDatalen != sizeof(OOP_TRANS_TASK_NODE_T)))
        {
            PRTL_FMT_DEBUG("读具体任务失败!\n");
            PRTL_FMT_DEBUG("读具体任务 stepRecordOut.recordnum = %d\n", stepRecordOut.recordnum);
            PRTL_FMT_DEBUG("读具体任务 stepRecordOut.record[0].road.oadCols.oad[0] = 0x%08x\n",
                stepRecordOut.record[0].road.oadCols.oad[0].value);
            PRTL_FMT_DEBUG("读具体任务 stepRecordOut.record[0].outDatalen = %d\n",
                stepRecordOut.record[0].outDatalen);
            PRTL_FMT_DEBUG("读具体任务 sizeof(OOP_TRANS_TASK_NODE_T) = %d\n",
                sizeof(OOP_TRANS_TASK_NODE_T));
            continue;
        }

        memcpy(&oopNode, stepRecordOut.record[0].outDataBuf, stepRecordOut.record[0].outDatalen);
        PRTL_FMT_DEBUG("stepRecordOut.infoNum = %d\n", stepRecordOut.infoNum);


        pNewMtr = (LINK_TRANS_NODE_T *)malloc(sizeof(LINK_TRANS_NODE_T));
        memset(pNewMtr, 0, sizeof(LINK_TRANS_NODE_T));
        pNewMtr->oopNode = oopNode;
        pNewMtr->no   = oopNodeNum ++;
        pNewMtr->next = NULL;
        if(!pMtrHead)
            pMtrHead = pNewMtr;
        if(pMtrTail)
            pMtrTail->next = pNewMtr;
        pMtrTail = pNewMtr;
    }

    db_read_record_end(hUdp, handle);
    
    return pMtrHead;
}

/**
*********************************************************************
* @name：       trans_node_ms_add
* @brief：      ms集合的添加处理(ms3 = ms1 + ms2)
* @param[in] ：ms 电表集合
* @param[out]: ms 电表集合
* @return    ：错误码
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
int trans_node_ms_add(OOP_MSC_T *ms1, OOP_MSC_T *ms2, OOP_MSC_T *ms3)
{
    uint32 i = 0;
    uint32 j = 0;
    uint32 addConnt = 0;
    OOP_MSP_T ms;
    OOP_TSA_T tsa;
    
    if ((ms1 == NULL) || (ms2 == NULL) || (ms3 == NULL))
    {
        return ERR_PTR;
    }

    memset(&tsa, 0, sizeof(tsa));
    memset(&ms, 0, sizeof(ms));

    ms.choice = 3;

    if ((ms1->choice != 3) || (ms2->choice != 3))
    {
        PRTL_FMT_DEBUG("暂时不支持该方法 choice1[%d] choice[%d]\n", ms1->choice, ms2->choice);
        return ERR_TYPE;
    }

    for (i = 0; i < ms2->msa.size; i++)
    {
        tsa = ms2->msa.add[i];
        
        for (j = 0; j < ms1->msa.size; j++)
        {
            //有相同的表地址，则跳出
            if (memcmp(&tsa, &ms1->msa.add[j], sizeof(OOP_TSA_T)) == 0)
            {
                break;
            }
        }

        if (j == ms1->msa.size)
        {
            ms.msa.add[addConnt] = tsa;
            addConnt ++;
            ms.msa.size ++;
            PRTL_BUF_TRACE(tsa.add, tsa.len + 1, "发现需要添加的表地址:");
        }
    }

    if (addConnt > 0)
    {
        memcpy(ms3, ms1, sizeof(OOP_MSC_T));

        if (ms3->msa.size + addConnt > OOP_MAX_MSREAD)
        {
            PRTL_FMT_DEBUG("ms集合无法存入太多的表 cur[%d] max[%d]\n", ms3->msa.size + addConnt, OOP_MAX_MSREAD);
            return ERR_INVALID;
        }

        for (i = 0; i < addConnt; i++)
        {
            ms3->msa.add[ms3->msa.size + i] = ms.msa.add[i];
        }
        ms3->msa.size += addConnt;
    }
    else
    {
        PRTL_FMT_DEBUG("ms2中表地址均存在于ms1中，无需添加 addConnt[%d] ms.msa.size[%d]\n", addConnt, ms.msa.size);
        return ERR_INVALID;
    }

    return ERR_OK;
}

/**
*********************************************************************
* @name：       trans_node_ms_del
* @brief：      ms集合的添加处理(ms3 = ms1 - ms2)
* @param[in] ：ms 电表集合
* @param[out]: ms 电表集合
* @return    ：错误码
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
int trans_node_ms_del(OOP_MSC_T *ms1, OOP_MSC_T *ms2, OOP_MSC_T *ms3)
{
    uint32 i = 0;
    uint32 j = 0;
    uint32 count = 0;
    OOP_MSP_T ms;
    OOP_TSA_T tsa;
    
    if ((ms1 == NULL) || (ms2 == NULL) || (ms3 == NULL))
    {
        return ERR_PTR;
    }

    memset(&tsa, 0, sizeof(tsa));
    memset(&ms, 0, sizeof(ms));

    ms.choice = 3;

    if ((ms1->choice != 3) || (ms2->choice != 3))
    {
        PRTL_FMT_DEBUG("暂时不支持该方法 choice1[%d] choice[%d]\n", ms1->choice, ms2->choice);
        return ERR_TYPE;
    }

    for (i = 0; i < ms1->msa.size; i++)
    {
        tsa = ms1->msa.add[i];
        
        for (j = 0; j < ms2->msa.size; j++)
        {
            //有相同的表地址，则跳出
            if (memcmp(&tsa, &ms2->msa.add[j], sizeof(OOP_TSA_T)) == 0)
            {
                PRTL_BUF_TRACE(tsa.add, tsa.len + 1, "发现需要删除的表地址:");
                break;
            }
        }

        //将不需要删除的表存入新的ms
        if (j == ms2->msa.size)
        {
            ms.msa.add[count] = tsa;
            count ++;
            ms.msa.size ++;
        }

    }

    if (count == 0)
    {
        PRTL_FMT_DEBUG("ms1中表地址被清空 count[%d]\n", ms.msa.size);
    }
    else if (count == ms1->msa.size)
    {
        PRTL_FMT_DEBUG("ms1中表地址均没有被删除 count[%d]\n", ms.msa.size);
        memcpy(ms3, &ms, sizeof(OOP_MSP_T));
        return ERR_INVALID;
    }
    else
    {
        PRTL_FMT_DEBUG("ms1中表地址部分被删除 count[%d]\n", ms.msa.size);
        memcpy(ms3, &ms, sizeof(OOP_MSP_T));
    }

    return ERR_OK;
}


/**
*********************************************************************
* @name：       class11_invoke_add_transparent_task_node
* @brief：      class11的ACTION服务
* @param[in] ：pOdaInfo        OAD信息
               pBpInfo        应用分帧断点信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
               outDataMax     输出报文缓冲区最大长度

* @param[out]：outData     输出报文
* @return    ：OOP_DAR_E
* @author    :
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E class11_invoke_add_transparent_node(OAD_INFO_T* pOadInfo, APDU_INFO_T* pApduInfo, const uint8* inData, 
    uint16 inLen, RESULT_DATA_T* outData, uint16 outDataMax)
{
    uint16 i = 0;
    int    ret = ERR_OK;
    uint32 sOffset = 4;
    uint32 dOffset = 0;
    uint8  num = 0;

    uint32         storageNo = 0;
    WRITE_RECORD_T recInData = {0};
    OOP_DAR_E      dar = DATA_OTHER_REASON;

    
    LINK_TRANS_NODE_T *linkNode = NULL;
    LINK_TRANS_NODE_T *linkNodeHead = NULL;

    OOP_TRANS_TASK_NODE_T data;
    OOP_TRANS_TASK_NODE_T dataLast;
    DATA_CONVERT_T        convert;
    OOP_TRANS_TASK_T      task;

    NOMAL_OAD_T normal = {0};
    uint32      len  = 0;

    OOP_MSC_T   ms;

    memset(&task, 0, sizeof(task));
    memset(&data, 0, sizeof(data));
    memset(&dataLast, 0, sizeof(dataLast));
    memset(&convert, 0, sizeof(convert));
    memset(&normal, 0, sizeof(normal));
    memset(&ms, 0, sizeof(ms));

    /*类型不存在*/
    if ((pOadInfo == NULL) || (pApduInfo == NULL) || (inData == NULL) || (outData == NULL))
    {
        return DATA_READ_WRITE_DENIED;
    }

    convert.srcBuf = (void*)inData;
    convert.srcLen = inLen;
    convert.sOffset = &sOffset;

    convert.dstBuf = &data;
    convert.dstLen = sizeof(data);
    convert.dOffset = &dOffset;

    switch (pOadInfo->pTab->oad.value)
    {
    case 0x6e137F00:
    {
        if (inData[sOffset++] != DT_ARRAY)
        {
            return ERR_OPTION;
        }

        num = inData[sOffset++];

        PRTL_FMT_DEBUG("增加具体任务，数量为%d\n", num);
        for (i = 0; i < num; i++)
        {
            dar = buf_to_data_trans_task_node(pOadInfo, &convert);
            if (dar != DATA_SUCCESS)
            {
                return dar;
            }
            else
            {
                dOffset = 0;
            }

            PRTL_FMT_DEBUG("需要增加任务方案号:%d\n", data.transTaskid);
            PRTL_FMT_DEBUG("需要增加具体任务号:%d\n", data.nodeid);

            //没有配置任务方案时，若配置具体任务应回对象不存在
            normal.oad.value = 0x6e110200;
            normal.infoNum = data.transTaskid;
            ret = db_read_nomal(pApduInfo->hUdp, &normal, sizeof(task), (uint8 *)&task, &len);
            if ((ret != ERR_OK) || (len != sizeof(task)))
            {
                PRTL_FMT_DEBUG("读任务方案失败. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x) len(%d) sizeof(task) = %d\n",
                    ret, normal.logicId, normal.infoNum, normal.oad.value, len, sizeof(task));
                return DATA_READ_WRITE_DENIED;
            }

            linkNode = get_node_link_stat(data.transTaskid, data.nodeid, pApduInfo->hUdp);
            if (linkNode != NULL)
            {
                linkNodeHead = linkNode;
                dataLast = linkNode->oopNode;
                if (memcmp(&data, &dataLast, sizeof(OOP_TRANS_TASK_NODE_T)) == 0)
                {
                    PRTL_FMT_DEBUG("任务方案号已经存在无需增加 transTaskid[%d] nodeid[%d]\n", data.transTaskid, 
                        data.nodeid);
                    link_node_destroy(linkNodeHead);
                    continue;
                }
                link_node_destroy(linkNodeHead);
            }

            ret = trans_node_ms_add(&dataLast.ms, &data.ms, &ms);
            if (ret != ERR_OK)
            {
                PRTL_FMT_DEBUG("ms集合的添加处理错误 错误码[%d]\n", ret);
            }
            else
            {
                data.ms = ms;
            }

            recInData.recordOAD.logicId = 0;
            recInData.recordOAD.infoNum = data.transTaskid;
            recInData.recordOAD.classtag = CLASS_DATA_INIT;
            recInData.recordOAD.road.oadMain.value = 0x6e130200;
            recInData.recordOAD.road.oadCols.nNum = 1;
            recInData.recordOAD.road.oadCols.oad[0].value = data.nodeid;
            memcpy(recInData.inDataBuf, &data, sizeof(OOP_TRANS_TASK_NODE_T));
            recInData.inDatalen = sizeof(OOP_TRANS_TASK_NODE_T);
            
            ret = db_write_record(pApduInfo->hUdp, &recInData, &storageNo);
            if (ret != ERR_OK)
            {
                PRTL_FMT_DEBUG("写入数据中心失败. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                    ret, recInData.recordOAD.logicId, recInData.recordOAD.infoNum, recInData.recordOAD.road.oadMain.value);
                return DATA_READ_WRITE_DENIED;
            }
        }

        memcpy_r((uint8*)outData->pResultData, (uint8*)&pOadInfo->pTab->oad.value, 4);
        if (ret == ERR_OK)
        {
            dar = DATA_SUCCESS;
            outData->pResultData[4] = DATA_SUCCESS;
            outData->pResultData[5] = 0x00;                          //DATA:NULL
            outData->resultDataLen = 0x06;
        }
        else
        {
            dar = DATA_READ_WRITE_DENIED;
            outData->pResultData[4] = DATA_READ_WRITE_DENIED;
            outData->resultDataLen = 0x05;
        }
    }
    break;
    default:
        break;
    }

    return dar;
}

/**
*********************************************************************
* @name：      class11_invoke_del_transparent_node_by_ms
* @brief：     class11的ACTION服务
* @param[in] ：pOdaInfo        OAD信息
               pBpInfo        应用分帧断点信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
               outDataMax     输出报文缓冲区最大长度

* @param[out]：outData     输出报文
* @return    ：OOP_DAR_E
* @author    :
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E class11_invoke_del_transparent_node_by_ms(OAD_INFO_T* pOadInfo, APDU_INFO_T* pApduInfo, const uint8* inData, 
    uint16 inLen, RESULT_DATA_T* outData, uint16 outDataMax)
{
    int    i = 0;
    int    ret = ERR_OK;
    uint32 offset = 0;
    CLEAR_DATA_T delData;

    uint16         taskId = 0;
    OOP_MSC_T      ms;
    uint16         num = 0;
    OOP_DAR_E      dar = DATA_SUCCESS;

    uint8          *srcBuf = (uint8*)inData;
    uint32         srcLen  = inLen;
    uint32         sOffset = 4;

    LINK_TRANS_NODE_T *linkNode = NULL;
    LINK_TRANS_NODE_T *linkNodeHead = NULL;

    OOP_MSC_T      ms1;
    OOP_TRANS_TASK_NODE_T data;

    uint32         storageNo = 0;
    WRITE_RECORD_T recInData = {0};

    memset(&ms, 0, sizeof(ms));
    memset(&ms1, 0, sizeof(ms1));
    memset(&delData, 0, sizeof(delData));
    memset(&data, 0, sizeof(data));

    switch (pOadInfo->pTab->oad.value)
    {
    case 0x6e138000:
    {
        if (srcBuf[sOffset ++] != DT_ARRAY)
        {
            return DATA_TYPE_UNMATCHED;
        }

        num = srcBuf[sOffset ++];
        if (num == 0x81)
        {
            num = srcBuf[sOffset ++];
        }
        else if (num == 0x82)
        {
            num = srcBuf[sOffset++];
            num <<= 8;
            num += srcBuf[sOffset++];
        }
        else if(num > 0x82||num == 0x80)
        {
            return DATA_TYPE_UNMATCHED;
        }

        for (i = 0; i < num; i++)
        {
            if (srcBuf[sOffset ++] != DT_STRUCTURE)
            {
                return DATA_TYPE_UNMATCHED;
            }

            if (srcBuf[sOffset ++] != 2)
            {
                return DATA_TYPE_UNMATCHED;
            }

            dar = basic_buf_to_data(E_LONG_UNSIGNED, srcBuf, srcLen, &sOffset, &taskId, sizeof(taskId));
            if (dar != DATA_SUCCESS)
            {
                return dar;
            }
            
            dar = basic_buf_to_data(E_MS, srcBuf, srcLen, &sOffset, &ms, sizeof(ms));
            if (dar != DATA_SUCCESS)
            {
                return dar;
            }

            linkNode = get_node_link_stat(taskId, 0, pApduInfo->hUdp);
            if (linkNode == NULL)
            {
                return DATA_DATA_BLOCK_UNAVAILABLE;
            }
            else
            {
                linkNodeHead = linkNode;
            }

            dar = DATA_READ_WRITE_DENIED;

            while (linkNode)
            {
                PRTL_FMT_TRACE("要删除的具体任务的taskId[%d] ms.choice[%d]\n", taskId, ms.choice);

                data = linkNode->oopNode;

                ret = trans_node_ms_del(&data.ms, &ms, &ms1);
                if (ret != ERR_OK)
                {
                    PRTL_FMT_DEBUG("ms集合的删除处理错误 错误码[%d]\n", ret);
                    linkNode = linkNode->next;
                    continue;
                }
                else
                {
                    data.ms = ms1;
                }

                recInData.recordOAD.logicId = 0;
                recInData.recordOAD.infoNum = data.transTaskid;
                recInData.recordOAD.classtag = CLASS_DATA_INIT;
                recInData.recordOAD.road.oadMain.value = 0x6e130200;
                recInData.recordOAD.road.oadCols.nNum = 1;
                recInData.recordOAD.road.oadCols.oad[0].value = data.nodeid;
                memcpy(recInData.inDataBuf, &data, sizeof(OOP_TRANS_TASK_NODE_T));
                recInData.inDatalen = sizeof(OOP_TRANS_TASK_NODE_T);
                
                ret = db_write_record(pApduInfo->hUdp, &recInData, &storageNo);
                if (ret != ERR_OK)
                {
                    PRTL_FMT_DEBUG("写入数据中心失败. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                        ret, recInData.recordOAD.logicId, recInData.recordOAD.infoNum, recInData.recordOAD.road.oadMain.value);
                }

                dar = DATA_SUCCESS;

                //此处理解错误
                /*if (class11_memcmp_group_meter(&linkNode->oopNode.ms, &ms))
                {
                    delData.recordOAD.logicId = 0x00;
                    delData.recordOAD.optional = 0;
                    delData.recordOAD.road.oadMain.value = 0x6e130200;
                    delData.recordOAD.road.oadCols.nNum = 1;
                    delData.recordOAD.road.oadCols.oad[0].value = linkNode->oopNode.nodeid;
                    delData.bClassTag = FALSE;

                    delData.recordOAD.infoNum = taskId;

                    ret = db_clear_oad_data(pApduInfo->hUdp, &delData);
                    if (ret != ERR_OK)
                    {
                        PRTL_FMT_DEBUG("删除具体任务失败 err = %d taskId[%d] nodeid[%d]\n", ret, taskId, 
                            linkNode->oopNode.nodeid);
                        link_node_destroy(linkNodeHead);
                        return DATA_READ_WRITE_DENIED;
                    }
                    else
                    {
                        PRTL_FMT_TRACE("删除具体任务成功 ret = %d taskId[%d] nodeid[%d]\n", ret, taskId, 
                            linkNode->oopNode.nodeid);
                    }

                    dar = DATA_SUCCESS;
                }*/
                linkNode = linkNode->next;
            }
            link_node_destroy(linkNodeHead);
        }
        
        break;
    }
    default:
        break;
    }

    if (dar != DATA_SUCCESS)
    {
        return dar;
    }

    memcpy(&outData->pResultData[offset], inData, sizeof(OOP_OAD_U));  //OMD
    offset += sizeof(OOP_OAD_U);
    outData->pResultData[offset++] = 0x00;                            //DAR type
    outData->pResultData[offset++] = 0x00;                            //DATA:NULL
    outData->resultDataLen = offset;

    return dar;
}

/**
*********************************************************************
* @name：      class11_invoke_del_transparent_node_by_task
* @brief：     class11的ACTION服务
* @param[in] ：pOdaInfo        OAD信息
               pBpInfo        应用分帧断点信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
               outDataMax     输出报文缓冲区最大长度

* @param[out]：outData     输出报文
* @return    ：OOP_DAR_E
* @author    :
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E class11_invoke_del_transparent_node_by_task(OAD_INFO_T* pOadInfo, APDU_INFO_T* pApduInfo, const uint8* inData, 
    uint16 inLen, RESULT_DATA_T* outData, uint16 outDataMax)
{
    int    ret = ERR_OK;
    int    i = 0;
    uint32 offset = 0;
    
    CLEAR_DATA_T   delData;
    uint16         taskIdNum = 0;
    uint16         taskId = 0;
    OOP_DAR_E      dar = DATA_SUCCESS;

    uint8          *srcBuf = (uint8*)inData;
    uint32         srcLen  = inLen;
    uint32         sOffset = 4;

    memset(&delData, 0, sizeof(delData));

    switch (pOadInfo->pTab->oad.value)
    {
    case 0x6e138100:
    {
        if (srcBuf[sOffset ++] != DT_ARRAY)
        {
            return DATA_TYPE_UNMATCHED;
        }

        taskIdNum = srcBuf[sOffset ++];
        if (taskIdNum == 0x81)
        {
            taskIdNum = srcBuf[sOffset ++];
        }
        else if(taskIdNum == 0x82)
        {
            taskIdNum = srcBuf[sOffset++];
            taskIdNum <<= 8;
            taskIdNum += srcBuf[sOffset++];
        }
        else if(taskIdNum > 0x82||taskIdNum == 0x80)
        {
            return DATA_TYPE_UNMATCHED;
        }

        for (i = 0; i < taskIdNum; i++)
        {
            dar = basic_buf_to_data(E_LONG_UNSIGNED, srcBuf, srcLen, &sOffset, &taskId, sizeof(taskId));
            if (dar != DATA_SUCCESS)
            {
                return dar;
            }

            delData.recordOAD.logicId = 0x00;
            delData.recordOAD.optional = 0;
            delData.recordOAD.road.oadMain.value = 0x6e130200;
            delData.recordOAD.road.oadCols.nNum = 1;
            delData.recordOAD.road.oadCols.oad[0].value = 0xffffffff;
            delData.bClassTag = FALSE;

            delData.recordOAD.infoNum = taskId;
            delData.bFilter = 1;
            delData.filterType = 255;

            ret = db_clear_oad_data(pApduInfo->hUdp, &delData);
            if (ret != ERR_OK)
            {
                PRTL_FMT_LOGCC("删除具体任务%d err = %d taskId[%d] nodeid[%d]\n", ret, taskId, 0);
            }
        }
        break;
    }
    default:
        break;
    }

    memcpy(&outData->pResultData[offset], inData, sizeof(OOP_OAD_U));  //OMD
    offset += sizeof(OOP_OAD_U);
    outData->pResultData[offset++] = 0x00;                            //DAR type
    outData->pResultData[offset++] = 0x00;                            //DATA:NULL
    outData->resultDataLen = offset;

    return dar;
}

/**
*********************************************************************
* @name：      class11_invoke_clr_transparent_node
* @brief：     class11的ACTION服务
* @param[in] ：pOdaInfo        OAD信息
               pBpInfo        应用分帧断点信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
               outDataMax     输出报文缓冲区最大长度

* @param[out]：outData     输出报文
* @return    ：OOP_DAR_E
* @author    :
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E class11_invoke_clr_transparent_node(OAD_INFO_T* pOadInfo, APDU_INFO_T* pApduInfo, const uint8* inData, 
    uint16 inLen, RESULT_DATA_T* outData, uint16 outDataMax)
{
    int    ret = ERR_OK;
    uint32 offset = 0;
    CLEAR_DATA_T delData;

    OOP_DAR_E    dar = DATA_SUCCESS;

    memset(&delData, 0, sizeof(delData));

    delData.recordOAD.logicId = 0x00;
    delData.recordOAD.optional = 0;
    delData.recordOAD.road.oadMain.value = 0x6e130200;
    delData.recordOAD.road.oadCols.nNum = 1;
    delData.recordOAD.road.oadCols.oad[0].value = 0xffffffff;
    delData.bClassTag = FALSE;

    delData.recordOAD.infoNum = 0xffff;
    delData.bFilter = 1;
    delData.filterType = 255;

    ret = db_clear_oad_data(pApduInfo->hUdp, &delData);
    if (ret != ERR_OK)
    {
        PRTL_FMT_LOGCC("删除具体任务%d err = %d taskId[%d] nodeid[%d]\n", ret, 0xffff, 0);
        return DATA_READ_WRITE_DENIED;
    }

    memcpy(&outData->pResultData[offset], inData, sizeof(OOP_OAD_U));  //OMD
    offset += sizeof(OOP_OAD_U);
    outData->pResultData[offset++] = 0x00;                            //DAR type
    outData->pResultData[offset++] = 0x00;                            //DATA:NULL
    outData->resultDataLen = offset;

    return dar;
}

/**
*********************************************************************
* @name：      class11_invoke_del_transparent_node_by_ms
* @brief：     class11的ACTION服务
* @param[in] ：pOdaInfo        OAD信息
               pBpInfo        应用分帧断点信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
               outDataMax     输出报文缓冲区最大长度

* @param[out]：outData     输出报文
* @return    ：OOP_DAR_E
* @author    :
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E class11_invoke_del_node_by_ms_without_task(OAD_INFO_T* pOadInfo, APDU_INFO_T* pApduInfo, const uint8* inData, 
    uint16 inLen, RESULT_DATA_T* outData, uint16 outDataMax)
{
    int    ret = ERR_OK;
    uint32 offset = 0;
    CLEAR_DATA_T delData;

    OOP_MSC_T      ms;
    OOP_DAR_E      dar = DATA_SUCCESS;

    uint8          *srcBuf = (uint8*)inData;
    uint32         srcLen  = inLen;
    uint32         sOffset = 4;

    LINK_TRANS_NODE_T *linkNode = NULL;
    LINK_TRANS_NODE_T *linkNodeHead = NULL;

    OOP_MSC_T      ms1;
    OOP_TRANS_TASK_NODE_T data;

    uint32         storageNo = 0;
    WRITE_RECORD_T recInData = {0};

    memset(&ms, 0, sizeof(ms));
    memset(&ms1, 0, sizeof(ms1));
    memset(&delData, 0, sizeof(delData));
    memset(&data, 0, sizeof(data));

    switch (pOadInfo->pTab->oad.value)
    {
    case 0x6e138300:
    {
        dar = basic_buf_to_data(E_MS, srcBuf, srcLen, &sOffset, &ms, sizeof(ms));
        if (dar != DATA_SUCCESS)
        {
            return dar;
        }
        break;
    }
    default:
        break;
    }

    linkNode = get_node_link_stat(0xffff, 0, pApduInfo->hUdp);
    if (linkNode == NULL)
    {
        return DATA_DATA_BLOCK_UNAVAILABLE;
    }
    else
    {
        linkNodeHead = linkNode;
    }

    dar = DATA_READ_WRITE_DENIED;

    while (linkNode)
    {
        PRTL_FMT_TRACE("要删除的具体任务的ms.choice[%d]\n", ms.choice);
        
        data = linkNode->oopNode;
        
        ret = trans_node_ms_del(&data.ms, &ms, &ms1);
        if (ret != ERR_OK)
        {
            PRTL_FMT_DEBUG("ms集合的删除处理错误 错误码[%d]\n", ret);
            linkNode = linkNode->next;
            continue;
        }
        else
        {
            data.ms = ms1;
        }
        
        recInData.recordOAD.logicId = 0;
        recInData.recordOAD.infoNum = data.transTaskid;
        recInData.recordOAD.classtag = CLASS_DATA_INIT;
        recInData.recordOAD.road.oadMain.value = 0x6e130200;
        recInData.recordOAD.road.oadCols.nNum = 1;
        recInData.recordOAD.road.oadCols.oad[0].value = data.nodeid;
        memcpy(recInData.inDataBuf, &data, sizeof(OOP_TRANS_TASK_NODE_T));
        recInData.inDatalen = sizeof(OOP_TRANS_TASK_NODE_T);
        
        ret = db_write_record(pApduInfo->hUdp, &recInData, &storageNo);
        if (ret != ERR_OK)
        {
            PRTL_FMT_DEBUG("写入数据中心失败. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                ret, recInData.recordOAD.logicId, recInData.recordOAD.infoNum, recInData.recordOAD.road.oadMain.value);
        }
        
        dar = DATA_SUCCESS;
        
        //此处理解错误
        /*if (class11_memcmp_group_meter(&linkNode->oopNode.ms, &ms))
        {
            delData.recordOAD.logicId = 0x00;
            delData.recordOAD.optional = 0;
            delData.recordOAD.road.oadMain.value = 0x6e130200;
            delData.recordOAD.road.oadCols.nNum = 1;
            delData.recordOAD.road.oadCols.oad[0].value = linkNode->oopNode.nodeid;
            delData.bClassTag = FALSE;

            delData.recordOAD.infoNum = linkNode->oopNode.transTaskid;

            ret = db_clear_oad_data(pApduInfo->hUdp, &delData);
            if (ret != ERR_OK)
            {
                PRTL_FMT_LOGCC("删除具体任务%d err = %d taskId[%d] nodeid[%d]\n", ret, 
                    linkNode->oopNode.transTaskid, linkNode->oopNode.nodeid);
                link_node_destroy(linkNodeHead);
                return DATA_READ_WRITE_DENIED;
            }

            dar = DATA_SUCCESS;
        }*/
        linkNode = linkNode->next;
    }

    link_node_destroy(linkNodeHead);

    if (dar != DATA_SUCCESS)
    {
        return dar;
    }

    memcpy(&outData->pResultData[offset], inData, sizeof(OOP_OAD_U));  //OMD
    offset += sizeof(OOP_OAD_U);
    outData->pResultData[offset++] = 0x00;                            //DAR type
    outData->pResultData[offset++] = 0x00;                            //DATA:NULL
    outData->resultDataLen = offset;

    
    return dar;
}

#endif

#if DESC("福建上报方案读取", 1)
/**
*********************************************************************
* @brief：      读记录型方案任务
* @param[in]： inReadRecord  - 筛选需要的数据
               oopRcsd       - 列选择
               readRecNum    - 读记录数量，：0：读全部记录 1：读最近的一条记录
               maxBufLen     - 最大读取buf长度
* @param[out]：outRecordBuf  -  数据记录
* @return：     =0            - 成功
               <0            - 错误码
*********************************************************************
*/
int record_trans_report_plan_by_task(DB_CLIENT hUdp, const READ_INF_T inReadNor, RELY_INFO_T relyInfo, uint32 maxBufLen, RECORD_BUF_T *outRecordBuf)
{
    int       result = DATA_OTHER_REASON;
    uint32    offset = 0;
    uint32    i = 0;
    int32     ret = ERR_NORMAL;
    OOP_DAR_E nRet = DATA_OTHER_REASON;
    uint32    bufAllLen = 0;
    BP_INFO_T breakInfo = {0};                                         //断点信息
    BOOL      isBufFull = FALSE;
    OAD_INFO_T *pOadInfo = relyInfo.pOadInfo;

    /*清除断点信息, 需要注意应用分帧的未帧*/
    memset(&breakInfo, 0, sizeof(BP_INFO_T));
   
    DATA_CONVERT_T   convert;
    OOP_TRANS_REPORT_T data = {0};                                        //每个任务方案信息
    uint32 sOffset = 0;
    uint32 dOffset = 0;
    uint32 oopRptNum = 0;
    uint32 oopRptNumOffset = 0;

    READ_RECORD_T      inReadRecord;
    STEP_READ_RECORD_T stepReadRecord;
    RECORD_UNIT_T      stepRecordOut;
    uint32             handle = 0;
    uint32             recNum = 0;

    MEMZERO(&convert, sizeof(convert));
    MEMZERO(&inReadRecord, sizeof(inReadRecord));

    oopRptNumOffset = offset;                                         //档案数量偏移
    outRecordBuf->recordBuf[offset ++] = 0x00;                          //数量，先填0
    bufAllLen = offset;

    inReadRecord.recordOAD.logicId = 0;
    inReadRecord.recordOAD.infoNum = 0xffff;
    inReadRecord.cType = COL_NO_INFO;
    inReadRecord.cStart = inReadNor.startTask;
    inReadRecord.cEnd = inReadNor.endTask;
    inReadRecord.sortType = UP_SORT;
    inReadRecord.recordOAD.optional = 0;
    inReadRecord.recordOAD.road.oadMain.value = 0;
    inReadRecord.recordOAD.road.oadCols.nNum = 0x01;
    inReadRecord.recordOAD.road.oadCols.oad[0].value = pOadInfo->pTab->oad.value;

    stepReadRecord.MAC.nNum = 0;

    if(relyInfo.bpBreakInfo->appSlicingFlag == 1)
    {
        inReadRecord.cStart = relyInfo.bpBreakInfo->index;
    }

    ret = db_read_record_start(hUdp, &inReadRecord, &handle, &recNum);
    if ((ret != ERR_OK) || (recNum == 0))
    {
        PRTL_FMT_DEBUG("开始读出错. 错误码[%d], infoNum[%d] recNum[%d] oad[0x%08x]\n", 
            ret, inReadRecord.recordOAD.infoNum, recNum, pOadInfo->pTab->oad.value);

        db_read_record_end(hUdp, handle);
        return ret;
    }
    else
    {
        PRTL_FMT_DEBUG("开始读成功. 返回码[%d], infoNum[%d] recNum[%d] oad[0x%08x]\n", 
            ret, inReadRecord.recordOAD.infoNum, recNum, pOadInfo->pTab->oad.value);
    }

    for (i = 0; i < recNum; i++)
    {
        ret = db_read_record_step(hUdp, handle, &stepReadRecord, &stepRecordOut);
        if(ret != ERR_OK)
        {
            PRTL_FMT_DEBUG("单步读出错 错误码[%d]\n", ret);
            continue;
        }

        //OAD与返回数据
        if ((stepRecordOut.recordnum != 1) 
        || (stepRecordOut.record[0].road.oadCols.oad[0].value != pOadInfo->pTab->oad.value)
        || (stepRecordOut.record[0].outDatalen != sizeof(data)))
        {
            PRTL_FMT_DEBUG("单步读到的数据错误!\n");
            PRTL_FMT_DEBUG("单步读数据 stepRecordOut.recordnum = %d\n", stepRecordOut.recordnum);
            PRTL_FMT_DEBUG("单步读数据 stepRecordOut.record[0].road.oadCols.oad[0] = 0x%08x\n", 
                stepRecordOut.record[0].road.oadCols.oad[0].value);
            PRTL_FMT_DEBUG("单步读数据 stepRecordOut.record[0].outDatalen = %d\n", 
                stepRecordOut.record[0].outDatalen);
            PRTL_FMT_DEBUG("单步读数据 sizeof(OOP_TRANS_REPORT_T) = %d\n", 
                sizeof(data));
            continue;
        }

        memcpy(&data, stepRecordOut.record[0].outDataBuf, stepRecordOut.record[0].outDatalen);
        PRTL_FMT_DEBUG("找到上报方案序号为%d\n", data.reportid);

        //转换成报文数据
        convert.srcBuf = &data;
        convert.srcLen = sizeof(data);
        convert.sOffset = &sOffset;

        convert.dstBuf = &outRecordBuf->recordBuf[offset];
        convert.dstLen = maxBufLen - offset;
        convert.dOffset = &dOffset;

        //记录断点信息
        breakInfo.index = data.reportid;

        nRet = data_to_buf_report_plan_fujian(pOadInfo, &convert);  //入参
        if (nRet == DATA_SUCCESS)
        {
            bufAllLen += dOffset;
            if (bufAllLen <= maxBufLen)
            {
                memcpy(&outRecordBuf->recordBuf[offset], convert.dstBuf, dOffset);
                offset += dOffset;
                outRecordBuf->bufLen = offset;
                PRTL_BUF_LOGCC(convert.dstBuf, dOffset, "第%d个任务方案报文:", i);
                oopRptNum ++;
                if (bufAllLen + dOffset >= maxBufLen) //虽然本次没有超最大长度，但下次会
                {
                    isBufFull = TRUE;
                    PRTL_FMT_DEBUG("虽然本次没有超最大长度，但下次会 isBufFull = [%d]\n", isBufFull);
                }
                dOffset = 0;
                sOffset = 0;
            }
            else
            {
                dOffset = 0;
                sOffset = 0;
                isBufFull = TRUE;
                PRTL_FMT_DEBUG("达到最大报文长度 isBufFull = [%d]\n", isBufFull);
            }
        }
        else
        {
            continue;
        }

        if ((isBufFull) && (offset > 0))
        {
            break;
        }
    }

    db_read_record_end(hUdp, handle);

    if (!isBufFull)
    {
        if (relyInfo.bpBreakInfo->appSlicingFlag == 0x01)
        {
            breakInfo.appSlicingFlag = 0x01;
            breakInfo.isEnd = 0x01;                           //末尾帧
            breakInfo.seq = relyInfo.bpBreakInfo->seq + 1;    //序号加1
            breakInfo.objPre = 0x01;                          //对象属性
        }
        else
        {
            /*清除断点信息*/
            memset(&breakInfo, 0, sizeof(BP_INFO_T));
        }
    }
    else
    {
        breakInfo.reqType = 0x03;
        breakInfo.appSlicingFlag = 0x01;
        breakInfo.isEnd = 0x00;                              //不是末尾帧
        breakInfo.objPre = 0x01;                             //对象属性
        if (relyInfo.bpBreakInfo->appSlicingFlag == 0x00)
        {
            breakInfo.seq = BRK_SEQ_START;                   //首次序号
        }
        else
        {
            breakInfo.seq = relyInfo.bpBreakInfo->seq + 1;  //序号加1
        }
    }

    //刷新断点信息
    memcpy(&outRecordBuf->breakInfo, &breakInfo, sizeof(BP_INFO_T));

    PRTL_BUF_LOGCC(outRecordBuf->recordBuf, offset, "整个报文:");
    PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->appSlicingFlag = %d\n", outRecordBuf->breakInfo.appSlicingFlag);
    PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->index = %d\n", outRecordBuf->breakInfo.index);
    PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->isEnd = %d\n", outRecordBuf->breakInfo.isEnd);
    PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->objPre = %d\n", outRecordBuf->breakInfo.objPre);
    PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->seq = %d\n", outRecordBuf->breakInfo.seq);
    PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->reqType = %d\n", outRecordBuf->breakInfo.reqType);

    result = DATA_SUCCESS;
    outRecordBuf->recordBuf[oopRptNumOffset] = oopRptNum;   //重新记档案数量
    outRecordBuf->bufLen = offset;

    return result;
}

/**
*********************************************************************
* @brief：      读记录型方案任务
* @param[in]： inReadRecord  - 筛选需要的数据
               oopRcsd       - 列选择
               readRecNum    - 读记录数量，：0：读全部记录 1：读最近的一条记录
               maxBufLen     - 最大读取buf长度
* @param[out]：outRecordBuf  -  数据记录
* @return：     =0            - 成功
               <0            - 错误码
*********************************************************************
*/
int record_trans_report_plan_by_port(DB_CLIENT hUdp, const READ_INF_T inReadNor, RELY_INFO_T relyInfo, uint32 maxBufLen, RECORD_BUF_T *outRecordBuf)
{
    int       result = DATA_OTHER_REASON;
    uint32    offset = 0;
    uint32    i = 0;
    int32     ret = ERR_NORMAL;
    OOP_DAR_E nRet = DATA_OTHER_REASON;
    uint32    bufAllLen = 0;
    BP_INFO_T breakInfo = {0};                                         //断点信息
    BOOL      isBufFull = FALSE;
    OAD_INFO_T *pOadInfo = relyInfo.pOadInfo;

    /*清除断点信息, 需要注意应用分帧的未帧*/
    memset(&breakInfo, 0, sizeof(BP_INFO_T));
   
    DATA_CONVERT_T   convert;
    OOP_TRANS_TASK_T data = {0};                                        //每个任务方案信息
    uint32 sOffset = 0;
    uint32 dOffset = 0;
    uint32 oopMeterNum = 0;
    uint32 oopMeterNumOffset = 0;

    READ_RECORD_T      inReadRecord;
    STEP_READ_RECORD_T stepReadRecord;
    RECORD_UNIT_T      stepRecordOut;
    uint32             handle = 0;
    uint32             recNum = 0;

    MEMZERO(&convert, sizeof(convert));
    MEMZERO(&inReadRecord, sizeof(inReadRecord));

    oopMeterNumOffset = offset;                                         //档案数量偏移
    outRecordBuf->recordBuf[offset ++] = 0x00;                          //数量，先填0
    bufAllLen = offset;

    inReadRecord.recordOAD.logicId = 0;
    inReadRecord.recordOAD.infoNum = 0xffff;
    inReadRecord.cType = COL_NO_INFO;
    inReadRecord.cStart = 0;
    inReadRecord.cEnd = 0xffff;
    inReadRecord.sortType = UP_SORT;
    inReadRecord.recordOAD.optional = 0;
    inReadRecord.recordOAD.road.oadMain.value = 0;
    inReadRecord.recordOAD.road.oadCols.nNum = 0x01;
    inReadRecord.recordOAD.road.oadCols.oad[0].value = pOadInfo->pTab->oad.value;

    stepReadRecord.MAC.nNum = 0;

    if(relyInfo.bpBreakInfo->appSlicingFlag == 1)
    {
        inReadRecord.cStart = relyInfo.bpBreakInfo->index;
    }

    ret = db_read_record_start(hUdp, &inReadRecord, &handle, &recNum);
    if ((ret != ERR_OK) || (recNum == 0))
    {
        PRTL_FMT_DEBUG("开始读出错 错误码[%d], infoNum[%d] recNum[%d] oad[0x%08x]\n", 
            ret, inReadRecord.recordOAD.infoNum, recNum, pOadInfo->pTab->oad.value);

        db_read_record_end(hUdp, handle);
        return ret;
    }
    else
    {
        PRTL_FMT_DEBUG("db_read_record_start err! ret = %d, infoNum = %d recNum = %d\n", 
            ret, inReadRecord.recordOAD.infoNum, recNum);
    }

    for (i = 0; i < recNum; i++)
    {
        ret = db_read_record_step(hUdp, handle, &stepReadRecord, &stepRecordOut);
        if(ret != ERR_OK)
        {
            PRTL_FMT_DEBUG("db_read_record_step err = %d!\n", ret);
            continue;
        }

        //OAD与返回数据
        if ((stepRecordOut.recordnum != 1) 
        || (stepRecordOut.record[0].road.oadCols.oad[0].value != pOadInfo->pTab->oad.value)
        || (stepRecordOut.record[0].outDatalen != sizeof(data)))
        {
            PRTL_FMT_DEBUG("单步读到的数据错误!\n");
            PRTL_FMT_DEBUG("单步读数据 stepRecordOut.recordnum = %d\n", stepRecordOut.recordnum);
            PRTL_FMT_DEBUG("单步读数据 stepRecordOut.record[0].road.oadCols.oad[0] = 0x%08x\n", 
                stepRecordOut.record[0].road.oadCols.oad[0].value);
            PRTL_FMT_DEBUG("单步读数据 stepRecordOut.record[0].outDatalen = %d\n", 
                stepRecordOut.record[0].outDatalen);
            PRTL_FMT_DEBUG("单步读数据 sizeof(OOP_TRANS_TASK_T) = %d\n", 
                sizeof(data));
            continue;
        }

        memcpy(&data, stepRecordOut.record[0].outDataBuf, stepRecordOut.record[0].outDatalen);

        if (data.oad.value != inReadNor.startTask)
        {
            continue;
        }
        
        PRTL_FMT_LOGCC("找到任务方案序号为%d\n", data.taskid);

        //转换成报文数据
        convert.srcBuf = &data;
        convert.srcLen = sizeof(data);
        convert.sOffset = &sOffset;

        convert.dstBuf = &outRecordBuf->recordBuf[offset];
        convert.dstLen = maxBufLen - offset;
        convert.dOffset = &dOffset;

        //记录断点信息
        breakInfo.index = data.taskid;

        nRet = data_to_buf_trans_task(pOadInfo, &convert);  //入参
        if (nRet == DATA_SUCCESS)
        {
            bufAllLen += dOffset;
            if (bufAllLen <= maxBufLen)
            {
                memcpy(&outRecordBuf->recordBuf[offset], convert.dstBuf, dOffset);
                offset += dOffset;
                outRecordBuf->bufLen = offset;
                PRTL_BUF_LOGCC(convert.dstBuf, dOffset, "第%d个任务方案报文:", i);
                oopMeterNum ++;
                if (bufAllLen + dOffset >= maxBufLen) //虽然本次没有超最大长度，但下次会
                {
                    isBufFull = TRUE;
                    PRTL_FMT_DEBUG("虽然本次没有超最大长度，但下次会 isBufFull = [%d]\n", isBufFull);
                }
                dOffset = 0;
                sOffset = 0;
            }
            else
            {
                dOffset = 0;
                sOffset = 0;
                isBufFull = TRUE;
                PRTL_FMT_DEBUG("达到最大报文长度 isBufFull = [%d]\n", isBufFull);
            }
        }
        else
        {
            continue;
        }

        if ((isBufFull) && (offset > 0))
        {
            break;
        }
    }

    db_read_record_end(hUdp, handle);

    if (!isBufFull)
    {
        if (relyInfo.bpBreakInfo->appSlicingFlag == 0x01)
        {
            breakInfo.appSlicingFlag = 0x01;
            breakInfo.isEnd = 0x01;                           //末尾帧
            breakInfo.seq = relyInfo.bpBreakInfo->seq + 1;    //序号加1
            breakInfo.objPre = 0x01;                          //对象属性
        }
        else
        {
            /*清除断点信息*/
            memset(&breakInfo, 0, sizeof(BP_INFO_T));
        }
    }
    else
    {
        breakInfo.reqType = 0x03;
        breakInfo.appSlicingFlag = 0x01;
        breakInfo.isEnd = 0x00;                              //不是末尾帧
        breakInfo.objPre = 0x01;                             //对象属性
        if (relyInfo.bpBreakInfo->appSlicingFlag == 0x00)
        {
            breakInfo.seq = BRK_SEQ_START;                   //首次序号
        }
        else
        {
            breakInfo.seq = relyInfo.bpBreakInfo->seq + 1;  //序号加1
        }
    }

    //刷新断点信息
    memcpy(&outRecordBuf->breakInfo, &breakInfo, sizeof(BP_INFO_T));

    PRTL_BUF_LOGCC(outRecordBuf->recordBuf, offset, "整个报文:");
    PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->appSlicingFlag = %d\n", outRecordBuf->breakInfo.appSlicingFlag);
    PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->index = %d\n", outRecordBuf->breakInfo.index);
    PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->isEnd = %d\n", outRecordBuf->breakInfo.isEnd);
    PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->objPre = %d\n", outRecordBuf->breakInfo.objPre);
    PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->seq = %d\n", outRecordBuf->breakInfo.seq);
    PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->reqType = %d\n", outRecordBuf->breakInfo.reqType);

    result = DATA_SUCCESS;
    outRecordBuf->recordBuf[oopMeterNumOffset] = oopMeterNum;   //重新记档案数量
    outRecordBuf->bufLen = offset;

    return result;
}
/**
*********************************************************************
* @brief：      读记录型方案任务
* @param[in]： inReadRecord  - 筛选需要的数据
               oopRcsd       - 列选择
               readRecNum    - 读记录数量，：0：读全部记录 1：读最近的一条记录
               maxBufLen     - 最大读取buf长度
* @param[out]：outRecordBuf  -  数据记录
* @return：     =0            - 成功
               <0            - 错误码
*********************************************************************
*/
int record_report_plan_fujian_read(DB_CLIENT hUdp, const READ_INF_T inReadNor, RELY_INFO_T relyInfo, uint32 maxBufLen, 
RECORD_BUF_T *outRecordBuf)
{
    if ((relyInfo.oopRsd->choice == 1) && (relyInfo.oopRsd->sel1.value.type == DT_OAD))
    {
        //return record_trans_report_plan_by_port(hUdp, inReadMeter, relyInfo, maxBufLen, outRecordBuf);
    }
    else
    {
        return record_trans_report_plan_by_task(hUdp, inReadNor, relyInfo, maxBufLen, outRecordBuf);
    }

    return ERR_OK;
}

/**
*********************************************************************
* @name：      class11_invoke_get_oop_meter_record
* @brief：     class11的GET服务
* @param[in] ：pOdaInfo        OAD信息
               pBpInfo        应用分帧断点信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
               outDataMax     输出报文缓冲区最大长度

* @param[out]：outData     输出报文
* @return    ：OOP_DAR_E
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E class11_invoke_get_report_plan_fujian_rec(OAD_INFO_T *pOdaInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    uint16 i = 0;
    uint16 j = 0;
    OOP_DAR_E  nRet = DATA_OTHER_REASON;
    uint32 offset = 4;
    uint16 csdStart = 0, csdLen = 0;
    uint16 outOffset = 0;
    uint16 inDataLenTmp = InDataLen;

    OOP_PRSD_T oopRsd;
    OOP_RCSD_T oopRcsd;
    uint16     headLen = 0;
    READ_INF_T inReadNor;
    RELY_INFO_T  relyInfo;
    RECORD_BUF_T outRecordBuf;

    /* 清0处理 */
    MEMZERO(&oopRsd, sizeof(oopRsd));
    MEMZERO(&oopRcsd, sizeof(oopRcsd));
    MEMZERO(&inReadNor, sizeof(inReadNor));
    MEMZERO(&relyInfo, sizeof(relyInfo));
    MEMZERO(&outRecordBuf, sizeof(outRecordBuf));

    outRecordBuf.recordBuf = (uint8 *)malloc(pApduInfo->connectionInfo.ApplSplitLen);
    memset(outRecordBuf.recordBuf, 0xee, pApduInfo->connectionInfo.ApplSplitLen);

    /*处理RSD*/
    nRet = apdu_rsd_fromfmt698(inData, inDataLenTmp, &offset, &oopRsd, sizeof(oopRsd));
    PRTL_FMT_LOGCC("oopRsd.choice = %d\n", oopRsd.choice);
    PRTL_FMT_LOGCC("offset = %d\n", offset);
    if (nRet != DATA_SUCCESS)
    {
        PRTL_FMT_TRACE("RSD解析错误\n");

        /*应对电科院协议一致性测试*/
        uint16 testOffset = 4;
        uint16 inCsdOff = 0, inCsdLen = 0;
        apdu_next_dpos_get(inData, &testOffset, InDataLen, DT_RSD, TRUE);
        inCsdOff = testOffset;
        apdu_next_dpos_get(inData, &testOffset, InDataLen, DT_RCSD, TRUE);
        inCsdLen = testOffset - inCsdOff;
        memcpy(outData->pResultData, inData, 4);
        memcpy(outData->pResultData + 4, inData + inCsdOff, inCsdLen);
        outData->resultDataLen = 4 + inCsdLen;

        outData->pResultData[outData->resultDataLen++] = 1;                   //后面优化下，outData->resultDataLen与下面统一
        if (apdu_rcsd_fromfmt698((uint8 *)inData, InDataLen, &offset, &oopRcsd, sizeof(oopRcsd)) != DATA_SUCCESS)
        {
            outData->pResultData[outData->resultDataLen++] = 0;

        }
        else
        {
            outData->pResultData[outData->resultDataLen++] = 1;
            for (i = 0; i < oopRcsd.nNum; i++)
            {
                outData->pResultData[outData->resultDataLen++] = NULL_DATA;
            }
        }

        free(outRecordBuf.recordBuf);
        outRecordBuf.recordBuf = NULL;
        return DATA_SUCCESS;
    }
    else
    {
        inDataLenTmp -= offset;  //除去RSD和OAD后的长度
    }

    /*处理RCSD*/
    csdStart = offset;
    if (apdu_rcsd_fromfmt698(inData, InDataLen, &offset, &oopRcsd, sizeof(oopRcsd)) != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("CSD解析错误!\n");
        free(outRecordBuf.recordBuf);
        outRecordBuf.recordBuf = NULL;
        return DATA_TYPE_UNMATCHED;
    }

    /*填写OAD+RCSD+记录结果1 并记下偏移headLen*/
    outData->resultDataLen = 0;
    csdLen = offset-csdStart;
    memcpy(outData->pResultData, inData, 4);
    outOffset = 4;

    if (csdLen == 1)
    {
        //如果rcsd为空，填充OAD6e120200
        outData->pResultData[outOffset ++] = 0x01;
        outData->pResultData[outOffset ++] = 0x00;
        outData->pResultData[outOffset ++] = 0x6e;
        outData->pResultData[outOffset ++] = 0x18;
        outData->pResultData[outOffset ++] = 0x02;
        outData->pResultData[outOffset ++] = 0x00;
    }
    else
    {
        memcpy(&outData->pResultData[outOffset], &inData[csdStart], csdLen);
        outOffset += csdLen;
    }
    outData->pResultData[outOffset++] = 1;
    outData->resultDataLen += outOffset;
    headLen = outOffset;
    PRTL_BUF_LOGCC(&outData->pResultData[0], headLen, "填写OAD+RCSD+记录结果1 并记下偏移headLen head:");

    /*记录型档案接口提供输入参数*/
    if (general_param_get_fj(&oopRsd, &oopRcsd, pApduInfo, &inReadNor, &relyInfo) != ERR_OK)
    {
        free(outRecordBuf.recordBuf);
        outRecordBuf.recordBuf = NULL;
        return DATA_READ_WRITE_DENIED;
    }

    /* 依赖信息中存下OAD列表的指针 */
    relyInfo.pOadInfo = pOdaInfo;

    PRTL_FMT_LOGCC("****************************************************\n");
    PRTL_FMT_LOGCC("inReadMeter.startTask = %d\n", inReadNor.startTask);
    PRTL_FMT_LOGCC("inReadMeter.endTask = %d\n", inReadNor.endTask);
    PRTL_FMT_LOGCC("relyInfo.oopRcsd.nNum = %d\n", relyInfo.oopRcsd.nNum);
    for(i = 0; i < relyInfo.oopRcsd.nNum; i++)
    {
        if (relyInfo.oopRcsd.cols[i].choice == 0)
        {
            PRTL_FMT_LOGCC("choice = %d relyInfo.oopRcsd.cols[%d].oad.value = 0x%08x\n", i, relyInfo.oopRcsd.cols[i].choice, 
                                                                                      relyInfo.oopRcsd.cols[i].oad.value);
        }
        else if(relyInfo.oopRcsd.cols[i].choice == 1)
        {
            PRTL_FMT_LOGCC("choice = %d relyInfo.oopRcsd.cols[%d].road.oadMain.value = 0x%08x\n", i, relyInfo.oopRcsd.cols[i].choice, 
                                                                                              relyInfo.oopRcsd.cols[i].road.oadMain.value);
            for (j = 0; j < relyInfo.oopRcsd.cols[i].road.oadCols.nNum; j++)
            {
                PRTL_FMT_LOGCC("choice = %d relyInfo.oopRcsd.cols[%d].road.oadCols.oad[%d].value = 0x%08x\n", i, j, relyInfo.oopRcsd.cols[i].choice, 
                                                                                                          relyInfo.oopRcsd.cols[i].road.oadCols.oad[j].value);
            }
        }
    }
    PRTL_FMT_LOGCC("****************************************************\n");

    /*读记录档案接口，可以读出apdu数据域，可返回断点信息*/
    if (record_report_plan_fujian_read(pApduInfo->hUdp, inReadNor, relyInfo, pApduInfo->connectionInfo.ApplSplitLen - headLen - cal_excludeLen(pApduInfo->connectionInfo), &outRecordBuf))
    {
        nRet = DATA_READ_WRITE_DENIED;
    }
    else
    {
        memcpy(&outData->pResultData[headLen], outRecordBuf.recordBuf, outRecordBuf.bufLen);
        outData->resultDataLen += outRecordBuf.bufLen;

        if (outRecordBuf.breakInfo.appSlicingFlag == 1)
        {
            /*记下断点信息*/
            PRTL_FMT_LOGCC("记下断点信息\n");
            outRecordBuf.breakInfo.objPre = 0x02;   //对象属性，记录型数据
            memcpy(pApduInfo->bpBreakInfo, &outRecordBuf.breakInfo, sizeof(BP_INFO_T));
        }
        else
        {
            /*清除断点信息*/
            PRTL_FMT_LOGCC("清除断点信息\n");
            memset(pApduInfo->bpBreakInfo, 0, sizeof(BP_INFO_T));
        }
        
        nRet = DATA_SUCCESS;
    }
    
    free(outRecordBuf.recordBuf);
    outRecordBuf.recordBuf = NULL;
    return DATA_SUCCESS;
}

/**
*********************************************************************
* @name：       class11_invoke_get_report_plan_fujian_nor
* @brief：      class11的GET服务
* @param[in] ：pOdaInfo        OAD信息
               pBpInfo        应用分帧断点信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
               outDataMax     输出报文缓冲区最大长度

* @param[out]：outData     输出报文
* @return    ：OOP_DAR_E
* @author    :
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E class11_invoke_get_report_plan_fujian_nor(OAD_INFO_T* pOadInfo, APDU_INFO_T* pApduInfo, const uint8* inData, uint16 inLen, RESULT_DATA_T* outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    DATA_CONVERT_T convert;
    uint32 sOffset = 0;
    uint32 dOffset = 0;
    uint32 offset = 0;
    uint32             i = 0;
    READ_RECORD_T      inReadRecord;
    uint32             handle = 0;
    uint32             recNum = 0;
    STEP_READ_RECORD_T stepReadRecord;
    RECORD_UNIT_T      stepRecordOut;
    OOP_TRANS_REPORT_T   data;
    uint32    realNum = 0;    //数组由于分帧导致的每帧实际成员数
    uint32    numOffset = 0;  //数组成员数在报文中的位置
    uint32    bufAllLen = 0;
    BP_INFO_T breakInfo = { 0 };                                                //断点信息
    uint32    maxBufLen = 0;
    BOOLEAN   isBufFull = FALSE;

    maxBufLen = pApduInfo->connectionInfo.ApplSplitLen - cal_excludeLen(pApduInfo->connectionInfo);
    memset(&breakInfo, 0, sizeof(BP_INFO_T));
    memset(&data, 0, sizeof(data));
    MEMZERO(&convert, sizeof(convert));

    memset(&inReadRecord, 0, sizeof(READ_RECORD_T));
    memset(&stepReadRecord, 0, sizeof(STEP_READ_RECORD_T));
    memset(&stepRecordOut, 0, sizeof(RECORD_UNIT_T));

    //读出数据
    inReadRecord.recordOAD.logicId = 0;

    if (pOadInfo->nIndex == 0)
    {
        inReadRecord.recordOAD.infoNum = 0xffff;
    }
    else
    {
        inReadRecord.recordOAD.infoNum = pOadInfo->nIndex;
    }

    inReadRecord.cType = COL_NO_INFO;
    inReadRecord.cStart = 0;
    inReadRecord.cEnd = NO_VIR_MTR_MAX_NUM;
    inReadRecord.sortType = UP_SORT;
    inReadRecord.recordOAD.optional = 0;
    inReadRecord.recordOAD.road.oadMain.value = 0;
    inReadRecord.recordOAD.road.oadCols.nNum = 0x01;
    inReadRecord.recordOAD.road.oadCols.oad[0].value = pOadInfo->pTab->oad.value;
    stepReadRecord.MAC.nNum = 0;
    if (pApduInfo->bpBreakInfo->appSlicingFlag == 1)
    {
        inReadRecord.cStart = pApduInfo->bpBreakInfo->tmStartSeg;
}
    memcpy(&outData->pResultData[offset], inData, sizeof(OOP_OAD_U));   //拷贝OAD
    offset += sizeof(OOP_OAD_U);
    outData->pResultData[offset++] = 0x01;                              //结果
    if (pOadInfo->nIndex == 0)
    {
        outData->pResultData[offset++] = DT_ARRAY;                         //类型
        numOffset = offset;                                                //数量偏移
        outData->pResultData[offset++] = 0x00;                             //数量，先填0
    }
    bufAllLen = offset;
    ret = db_read_record_start(pApduInfo->hUdp, &inReadRecord, &handle, &recNum);
    if ((ret != ERR_OK) || (recNum == 0))
    {
        PRTL_FMT_DEBUG("福建上报方案开始读失败 ret = %d, infoNum = %d recNum = %d\n",
            ret, inReadRecord.recordOAD.infoNum, recNum);
    }
    else
    {
        PRTL_FMT_DEBUG("福建上报方案开始读成功 ret = %d, infoNum = %d recNum = %d\n",
            ret, inReadRecord.recordOAD.infoNum, recNum);

        for (i = 0; i < recNum; i++)
        {
            ret = db_read_record_step(pApduInfo->hUdp, handle, &stepReadRecord, &stepRecordOut);
            if (ret != ERR_OK)
            {
                PRTL_FMT_DEBUG("福建上报方案单步读失败 db_read_record_step err = %d!\n", ret);
                continue;
                //数据中心出错返回NULL
            }

            //OAD与返回数据
            if ((stepRecordOut.recordnum != 1)
                || (stepRecordOut.record[0].road.oadCols.oad[0].value != pOadInfo->pTab->oad.value)
                || (stepRecordOut.record[0].outDatalen != sizeof(OOP_TRANS_REPORT_T)))
            {
                PRTL_FMT_DEBUG("读福建上报方案失败!\n");
                PRTL_FMT_DEBUG("读福建上报方案 stepRecordOut.recordnum = %d\n", stepRecordOut.recordnum);
                PRTL_FMT_DEBUG("读福建上报方案 stepRecordOut.record[0].road.oadCols.oad[0] = 0x%08x\n",
                    stepRecordOut.record[0].road.oadCols.oad[0].value);
                PRTL_FMT_DEBUG("读福建上报方案 stepRecordOut.record[0].outDatalen = %d\n",
                    stepRecordOut.record[0].outDatalen);
                PRTL_FMT_DEBUG("读福建上报方案 sizeof(OOP_TRANS_REPORT_T) = %d\n",
                    sizeof(OOP_TRANS_REPORT_T));
                continue;
            }

            memcpy(&data, stepRecordOut.record[0].outDataBuf, stepRecordOut.record[0].outDatalen);
            PRTL_FMT_DEBUG("读到福建上报方案[%d]\n", stepRecordOut.infoNum);

            //转换成报文数据
            convert.srcBuf = &data;
            convert.srcLen = sizeof(data);
            convert.sOffset = &sOffset;

            convert.dstBuf = &outData->pResultData[offset];
            convert.dstLen = outDataMax - offset;
            convert.dOffset = &dOffset;

            breakInfo.tmStartSeg = stepRecordOut.infoNum;
            dar = data_to_buf_report_plan_fujian(pOadInfo, &convert);
            if (dar == DATA_SUCCESS)
            {
                bufAllLen += dOffset;
                if (bufAllLen <= maxBufLen)
                {
                    memcpy(&outData->pResultData[offset], convert.dstBuf, dOffset);
                    offset += dOffset;
                    outData->resultDataLen = offset;
                    PRTL_BUF_DEBUG(convert.dstBuf, dOffset, "第%d个福建上报方案:", i);
                    realNum++;
                    dOffset = 0;
                    sOffset = 0;
                }
                else
                {
                    dOffset = 0;
                    sOffset = 0;
                    isBufFull = TRUE;
                }
            }
            else
            {
                continue;
            }
            if ((isBufFull) && (offset > 0))
            {
                break;
            }
        }
    }
    db_read_record_end(pApduInfo->hUdp, handle);
    if (!isBufFull)
    {
        if (pApduInfo->bpBreakInfo->appSlicingFlag == 0x01)
        {
            breakInfo.appSlicingFlag = 0x01;
            breakInfo.isEnd = 0x01;                           //末尾帧
            breakInfo.seq = pApduInfo->bpBreakInfo->seq + 1;  //序号加1
            breakInfo.objPre = 0x01;                          //对象属性
        }
        else
        {
            memset(&breakInfo, 0, sizeof(BP_INFO_T));
        }
    }
    else
    {
        breakInfo.reqType = pApduInfo->reqType;
        breakInfo.appSlicingFlag = 0x01;
        breakInfo.isEnd = 0x00;                              //不是末尾帧
        breakInfo.objPre = 0x01;                             //对象属性
        if (pApduInfo->bpBreakInfo->appSlicingFlag == 0x00)
        {
            breakInfo.seq = BRK_SEQ_START;                   //首次序号
        }
        else
        {
            breakInfo.seq = pApduInfo->bpBreakInfo->seq + 1;  //序号加1
        }
    }
    memcpy(pApduInfo->bpBreakInfo, &breakInfo, sizeof(BP_INFO_T));

    if (pOadInfo->nIndex == 0)
    {
        outData->pResultData[numOffset] = realNum;   //重新记数量
    }

    outData->resultDataLen = offset;

    return dar;
}
#endif

#if DESC("采集状态集合", 1)
/**
*********************************************************************
* @name：      class11_invoke_get_transparent_node_all_nor
* @brief：     class11的GET服务
* @param[in] ：pOdaInfo        OAD信息
               pBpInfo        应用分帧断点信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
               outDataMax     输出报文缓冲区最大长度

* @param[out]：outData     输出报文
* @return    ：OOP_DAR_E
* @author    :
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E class11_invoke_get_acq_status_all_nor(OAD_INFO_T* pOadInfo, APDU_INFO_T* pApduInfo, const uint8* inData, uint16 inLen, RESULT_DATA_T* outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    DATA_CONVERT_T convert;
    uint32 sOffset = 0;
    uint32 dOffset = 0;
    uint32 offset = 0;
    uint32             i = 0;
    READ_RECORD_T      inReadRecord;
    uint32             handle = 0;
    uint32             recNum = 0;
    STEP_READ_RECORD_T stepReadRecord;
    RECORD_UNIT_T      stepRecordOut;
    OOP_TRANS_ACQ_RLUESTATE_T   data;
    uint32    realNum = 0;    //数组由于分帧导致的每帧实际成员数
    uint32    numOffset = 0;  //数组成员数在报文中的位置
    uint32    bufAllLen = 0;
    BP_INFO_T breakInfo = { 0 };                                                //断点信息
    uint32    maxBufLen = 0;
    BOOLEAN   isBufFull = FALSE;

    maxBufLen = pApduInfo->connectionInfo.ApplSplitLen - cal_excludeLen(pApduInfo->connectionInfo);
    memset(&breakInfo, 0, sizeof(BP_INFO_T));
    memset(&data, 0, sizeof(data));
    MEMZERO(&convert, sizeof(convert));

    memset(&inReadRecord, 0, sizeof(READ_RECORD_T));
    memset(&stepReadRecord, 0, sizeof(STEP_READ_RECORD_T));
    memset(&stepRecordOut, 0, sizeof(RECORD_UNIT_T));
    //读出数据
    inReadRecord.recordOAD.logicId = 0;

    if (pOadInfo->nIndex == 0)
    {
        inReadRecord.recordOAD.infoNum = 0xffff;
    }
    else
    {
        inReadRecord.recordOAD.infoNum = pOadInfo->nIndex;
    }

    inReadRecord.cType = COL_NO_INFO;
    inReadRecord.cStart = 0;
    inReadRecord.cEnd = NO_VIR_MTR_MAX_NUM;
    inReadRecord.sortType = UP_SORT;
    inReadRecord.recordOAD.optional = 0;
    inReadRecord.recordOAD.road.oadMain.value = 0;
    inReadRecord.recordOAD.road.oadCols.nNum = 0x01;
    inReadRecord.recordOAD.road.oadCols.oad[0].value = pOadInfo->pTab->oad.value;
    stepReadRecord.MAC.nNum = 0;
    if (pApduInfo->bpBreakInfo->appSlicingFlag == 1)
    {
        inReadRecord.cStart = pApduInfo->bpBreakInfo->tmStartSeg;
    }
    memcpy(&outData->pResultData[offset], inData, sizeof(OOP_OAD_U));   //拷贝OAD
    offset += sizeof(OOP_OAD_U);
    outData->pResultData[offset++] = 0x01;                              //结果
    if (pOadInfo->nIndex == 0)
    {
        outData->pResultData[offset++] = DT_ARRAY;                         //类型
        numOffset = offset;                                                //数量偏移
        outData->pResultData[offset++] = 0x00;                             //数量，先填0
    }
    bufAllLen = offset;
    ret = db_read_record_start(pApduInfo->hUdp, &inReadRecord, &handle, &recNum);
    if ((ret != ERR_OK) || (recNum == 0))
    {
        PRTL_FMT_DEBUG("采集状态开始读失败 ret = %d, infoNum = %d recNum = %d\n",
            ret, inReadRecord.recordOAD.infoNum, recNum);
    }
    else
    {
        PRTL_FMT_DEBUG("采集状态开始读成功 ret = %d, infoNum = %d recNum = %d\n",
            ret, inReadRecord.recordOAD.infoNum, recNum);

        for (i = 0; i < recNum; i++)
        {
            ret = db_read_record_step(pApduInfo->hUdp, handle, &stepReadRecord, &stepRecordOut);
            if (ret != ERR_OK)
            {
                PRTL_FMT_DEBUG("采集状态单步读失败 db_read_record_step err = %d!\n", ret);
                continue;
                //数据中心出错返回NULL
            }

            //OAD与返回数据
            if ((stepRecordOut.recordnum != 1)
                || (stepRecordOut.record[0].road.oadCols.oad[0].value != pOadInfo->pTab->oad.value)
                || (stepRecordOut.record[0].outDatalen != sizeof(OOP_TRANS_ACQ_RLUESTATE_T)))
            {
                PRTL_FMT_DEBUG("读采集状态失败!\n");
                PRTL_FMT_DEBUG("读采集状态 stepRecordOut.recordnum = %d\n", stepRecordOut.recordnum);
                PRTL_FMT_DEBUG("读采集状态 stepRecordOut.record[0].road.oadCols.oad[0] = 0x%08x\n",
                    stepRecordOut.record[0].road.oadCols.oad[0].value);
                PRTL_FMT_DEBUG("读采集状态 stepRecordOut.record[0].outDatalen = %d\n",
                    stepRecordOut.record[0].outDatalen);
                PRTL_FMT_DEBUG("读采集状态 sizeof(OOP_TRANS_ACQ_RLUESTATE_T) = %d\n",
                    sizeof(OOP_TRANS_ACQ_RLUESTATE_T));
                continue;
            }

            memcpy(&data, stepRecordOut.record[0].outDataBuf, stepRecordOut.record[0].outDatalen);
            PRTL_FMT_DEBUG("stepRecordOut.infoNum = %d\n", stepRecordOut.infoNum);

            //转换成报文数据
            convert.srcBuf = &data;
            convert.srcLen = sizeof(data);
            convert.sOffset = &sOffset;

            convert.dstBuf = &outData->pResultData[offset];
            convert.dstLen = outDataMax - offset;
            convert.dOffset = &dOffset;

            breakInfo.tmStartSeg = stepRecordOut.infoNum;
            dar = data_to_buf_acq_status(pOadInfo, &convert);
            if (dar == DATA_SUCCESS)
            {
                bufAllLen += dOffset;
                if (bufAllLen <= maxBufLen)
                {
                    memcpy(&outData->pResultData[offset], convert.dstBuf, dOffset);
                    offset += dOffset;
                    outData->resultDataLen = offset;
                    PRTL_BUF_DEBUG(convert.dstBuf, dOffset, "第%d个采集状态:", i);
                    realNum++;
                    dOffset = 0;
                    sOffset = 0;
                }
                else
                {
                    dOffset = 0;
                    sOffset = 0;
                    isBufFull = TRUE;
                }
            }
            else
            {
                continue;
            }
            if ((isBufFull) && (offset > 0))
            {
                break;
            }
        }
    }
    db_read_record_end(pApduInfo->hUdp, handle);
    if (!isBufFull)
    {
        if (pApduInfo->bpBreakInfo->appSlicingFlag == 0x01)
        {
            breakInfo.appSlicingFlag = 0x01;
            breakInfo.isEnd = 0x01;                           //末尾帧
            breakInfo.seq = pApduInfo->bpBreakInfo->seq + 1;  //序号加1
            breakInfo.objPre = 0x01;                          //对象属性
        }
        else
        {
            memset(&breakInfo, 0, sizeof(BP_INFO_T));
        }
    }
    else
    {
        breakInfo.reqType = pApduInfo->reqType;
        breakInfo.appSlicingFlag = 0x01;
        breakInfo.isEnd = 0x00;                              //不是末尾帧
        breakInfo.objPre = 0x01;                             //对象属性
        if (pApduInfo->bpBreakInfo->appSlicingFlag == 0x00)
        {
            breakInfo.seq = BRK_SEQ_START;                   //首次序号
        }
        else
        {
            breakInfo.seq = pApduInfo->bpBreakInfo->seq + 1;  //序号加1
        }
    }
    memcpy(pApduInfo->bpBreakInfo, &breakInfo, sizeof(BP_INFO_T));

    if (pOadInfo->nIndex == 0)
    {
        outData->pResultData[numOffset] = realNum;   //重新记数量
    }

    outData->resultDataLen = offset;

    return dar;
}

/**
*********************************************************************
* @brief：      读记录型方案任务
* @param[in]： inReadRecord  - 筛选需要的数据
               oopRcsd       - 列选择
               readRecNum    - 读记录数量，：0：读全部记录 1：读最近的一条记录
               maxBufLen     - 最大读取buf长度
* @param[out]：outRecordBuf  -  数据记录
* @return：     =0            - 成功
               <0            - 错误码
*********************************************************************
*/
int record_acq_status_read_by_tsa(DB_CLIENT hUdp, const READ_INF_T inReadNor, RELY_INFO_T relyInfo, uint32 maxBufLen, RECORD_BUF_T *outRecordBuf)
{
    int       result = DATA_OTHER_REASON;
    uint32    offset = 0;
    uint32    i = 0;
    int32     ret = ERR_NORMAL;
    OOP_DAR_E nRet = DATA_OTHER_REASON;
    uint32    bufAllLen = 0;
    BP_INFO_T breakInfo = {0};                                         //断点信息
    BOOL      isBufFull = FALSE;
    OAD_INFO_T *pOadInfo = relyInfo.pOadInfo;

    /*清除断点信息, 需要注意应用分帧的未帧*/
    memset(&breakInfo, 0, sizeof(BP_INFO_T));
   
    DATA_CONVERT_T   convert;
    OOP_TRANS_ACQ_RLUESTATE_T data = {0};                              //每个采集状态信息
    uint32 sOffset = 0;
    uint32 dOffset = 0;
    uint32 oopAcqNum = 0;
    uint32 oopAcqNumOffset = 0;

    READ_RECORD_T      inReadRecord;
    STEP_READ_RECORD_T stepReadRecord;
    RECORD_UNIT_T      stepRecordOut;
    uint32             handle = 0;
    uint32             recNum = 0;

    MEMZERO(&convert, sizeof(convert));
    MEMZERO(&inReadRecord, sizeof(inReadRecord));

    oopAcqNumOffset = offset;                                           //数量偏移
    outRecordBuf->recordBuf[offset ++] = 0x00;                          //数量，先填0
    bufAllLen = offset;

    inReadRecord.recordOAD.logicId = 0;
    inReadRecord.recordOAD.infoNum = 0xffff;
    inReadRecord.cType = COL_NO_INFO;
    inReadRecord.cStart = 0;
    inReadRecord.cEnd = 0xffff;
    inReadRecord.sortType = UP_SORT;
    inReadRecord.recordOAD.optional = 0;
    inReadRecord.recordOAD.road.oadMain.value = 0;
    inReadRecord.recordOAD.road.oadCols.nNum = 0x01;
    inReadRecord.recordOAD.road.oadCols.oad[0].value = pOadInfo->pTab->oad.value;

    stepReadRecord.MAC.nNum = 0;

    ret = db_read_record_start(hUdp, &inReadRecord, &handle, &recNum);
    if ((ret != ERR_OK) || (recNum == 0))
    {
        PRTL_FMT_DEBUG("开始读出错 错误码[%d], infoNum[%d] recNum[%d] oad[0x%08x]\n", 
            ret, inReadRecord.recordOAD.infoNum, recNum, pOadInfo->pTab->oad.value);

        db_read_record_end(hUdp, handle);
        return ret;
    }
    else
    {
        PRTL_FMT_DEBUG("开始读成功[%d], infoNum[%d] recNum[%d] oad[0x%08x]\n", 
            ret, inReadRecord.recordOAD.infoNum, recNum, pOadInfo->pTab->oad.value);
    }

    for (i = 0; i < recNum; i++)
    {
        ret = db_read_record_step(hUdp, handle, &stepReadRecord, &stepRecordOut);
        if(ret != ERR_OK)
        {
            PRTL_FMT_DEBUG("单步读失败 错误码[%d] infoNum[%d] recNum[%d]\n", ret, 
                inReadRecord.recordOAD.infoNum, recNum);
            continue;
        }

        if (i < relyInfo.bpBreakInfo->index)
        {
            continue;
        }
        else
        {
            relyInfo.bpBreakInfo->index = 0;
        }

        //OAD与返回数据
        if ((stepRecordOut.recordnum != 1) 
        || (stepRecordOut.record[0].road.oadCols.oad[0].value != pOadInfo->pTab->oad.value)
        || (stepRecordOut.record[0].outDatalen != sizeof(data)))
        {
            PRTL_FMT_DEBUG("单步读到的数据错误!\n");
            PRTL_FMT_DEBUG("单步读数据 stepRecordOut.recordnum = %d\n", stepRecordOut.recordnum);
            PRTL_FMT_DEBUG("单步读数据 stepRecordOut.record[0].road.oadCols.oad[0] = 0x%08x\n", 
                stepRecordOut.record[0].road.oadCols.oad[0].value);
            PRTL_FMT_DEBUG("单步读数据 stepRecordOut.record[0].outDatalen = %d\n", 
                stepRecordOut.record[0].outDatalen);
            PRTL_FMT_DEBUG("单步读数据 sizeof(OOP_TRANS_ACQ_RLUESTATE_T) = %d\n", 
                sizeof(data));
            continue;
        }

        memcpy(&data, stepRecordOut.record[0].outDataBuf, stepRecordOut.record[0].outDatalen);

        if (memcmp(&data.add, &inReadNor.tsa, sizeof(OOP_TSA_T)) != 0)
        {
            continue;
        }
        
        PRTL_BUF_LOGCC(data.add.add, 6, "找到一个采集状态：%d\n");

        //转换成报文数据
        convert.srcBuf = &data;
        convert.srcLen = sizeof(data);
        convert.sOffset = &sOffset;

        convert.dstBuf = &outRecordBuf->recordBuf[offset];
        convert.dstLen = maxBufLen - offset;
        convert.dOffset = &dOffset;

        //记录断点信息
        breakInfo.index = i;

        nRet = data_to_buf_acq_status(pOadInfo, &convert);  //入参
        if (nRet == DATA_SUCCESS)
        {
            bufAllLen += dOffset;
            if (bufAllLen <= maxBufLen)
            {
                memcpy(&outRecordBuf->recordBuf[offset], convert.dstBuf, dOffset);
                offset += dOffset;
                outRecordBuf->bufLen = offset;
                PRTL_BUF_LOGCC(convert.dstBuf, dOffset, "第%d个采集状态报文:", i);
                oopAcqNum ++;
                if (bufAllLen + dOffset >= maxBufLen) //虽然本次没有超最大长度，但下次会
                {
                    isBufFull = TRUE;
                    PRTL_FMT_DEBUG("虽然本次没有超最大长度，但下次会 isBufFull = [%d]\n", isBufFull);
                }
                dOffset = 0;
                sOffset = 0;
            }
            else
            {
                dOffset = 0;
                sOffset = 0;
                isBufFull = TRUE;
                PRTL_FMT_DEBUG("达到最大报文长度 isBufFull = [%d]\n", isBufFull);
            }
        }
        else
        {
            continue;
        }

        if ((isBufFull) && (offset > 0))
        {
            break;
        }
    }

    db_read_record_end(hUdp, handle);

    if (!isBufFull)
    {
        if (relyInfo.bpBreakInfo->appSlicingFlag == 0x01)
        {
            breakInfo.appSlicingFlag = 0x01;
            breakInfo.isEnd = 0x01;                           //末尾帧
            breakInfo.seq = relyInfo.bpBreakInfo->seq + 1;    //序号加1
            breakInfo.objPre = 0x01;                          //对象属性
        }
        else
        {
            /*清除断点信息*/
            memset(&breakInfo, 0, sizeof(BP_INFO_T));
        }
    }
    else
    {
        breakInfo.reqType = 0x03;
        breakInfo.appSlicingFlag = 0x01;
        breakInfo.isEnd = 0x00;                              //不是末尾帧
        breakInfo.objPre = 0x01;                             //对象属性
        if (relyInfo.bpBreakInfo->appSlicingFlag == 0x00)
        {
            breakInfo.seq = BRK_SEQ_START;                   //首次序号
        }
        else
        {
            breakInfo.seq = relyInfo.bpBreakInfo->seq + 1;  //序号加1
        }
    }

    //刷新断点信息
    memcpy(&outRecordBuf->breakInfo, &breakInfo, sizeof(BP_INFO_T));

    PRTL_BUF_LOGCC(outRecordBuf->recordBuf, offset, "整个报文:");
    PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->appSlicingFlag = %d\n", outRecordBuf->breakInfo.appSlicingFlag);
    PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->index = %d\n", outRecordBuf->breakInfo.index);
    PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->isEnd = %d\n", outRecordBuf->breakInfo.isEnd);
    PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->objPre = %d\n", outRecordBuf->breakInfo.objPre);
    PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->seq = %d\n", outRecordBuf->breakInfo.seq);
    PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->reqType = %d\n", outRecordBuf->breakInfo.reqType);

    result = DATA_SUCCESS;
    outRecordBuf->recordBuf[oopAcqNumOffset] = oopAcqNum;   //重新记档案数量
    outRecordBuf->bufLen = offset;

    return result;
}


/**
*********************************************************************
* @brief：      读记录型方案任务
* @param[in]： inReadRecord  - 筛选需要的数据
               oopRcsd       - 列选择
               readRecNum    - 读记录数量，：0：读全部记录 1：读最近的一条记录
               maxBufLen     - 最大读取buf长度
* @param[out]：outRecordBuf  -  数据记录
* @return：     =0            - 成功
               <0            - 错误码
*********************************************************************
*/
int record_acq_status_read(DB_CLIENT hUdp, const READ_INF_T inReadNor, RELY_INFO_T relyInfo, uint32 maxBufLen, 
RECORD_BUF_T *outRecordBuf)
{
    if ((relyInfo.oopRsd->choice == 1) && (relyInfo.oopRsd->sel1.value.type == DT_OAD))
    {
        //return record_acq_status_read_by_port(hUdp, inReadMeter, relyInfo, maxBufLen, outRecordBuf);
    }
    else
    {
        return record_acq_status_read_by_tsa(hUdp, inReadNor, relyInfo, maxBufLen, outRecordBuf);
    }

    return ERR_OK;
}

/**
*********************************************************************
* @name：      class11_invoke_get_acq_status_all_rec
* @brief：     class11的GET服务
* @param[in] ：pOdaInfo        OAD信息
               pBpInfo        应用分帧断点信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
               outDataMax     输出报文缓冲区最大长度

* @param[out]：outData     输出报文
* @return    ：OOP_DAR_E
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E class11_invoke_get_acq_status_all_rec(OAD_INFO_T *pOdaInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    uint16 i = 0;
    uint16 j = 0;
    OOP_DAR_E  nRet = DATA_OTHER_REASON;
    uint32 offset = 4;
    uint16 csdStart = 0, csdLen = 0;
    uint16 outOffset = 0;
    uint16 inDataLenTmp = InDataLen;

    OOP_PRSD_T oopRsd;
    OOP_RCSD_T oopRcsd;
    uint16     headLen = 0;
    READ_INF_T    inReadNor;
    RELY_INFO_T   relyInfo;
    RECORD_BUF_T  outRecordBuf;

    /* 清0处理 */
    MEMZERO(&oopRsd, sizeof(oopRsd));
    MEMZERO(&oopRcsd, sizeof(oopRcsd));
    MEMZERO(&inReadNor, sizeof(inReadNor));
    MEMZERO(&relyInfo, sizeof(relyInfo));
    MEMZERO(&outRecordBuf, sizeof(outRecordBuf));

    outRecordBuf.recordBuf = (uint8 *)malloc(pApduInfo->connectionInfo.ApplSplitLen);
    memset(outRecordBuf.recordBuf, 0xee, pApduInfo->connectionInfo.ApplSplitLen);

    /*处理RSD*/
    nRet = apdu_rsd_fromfmt698(inData, inDataLenTmp, &offset, &oopRsd, sizeof(oopRsd));
    PRTL_FMT_LOGCC("oopRsd.choice = %d\n", oopRsd.choice);
    PRTL_FMT_LOGCC("oopRsd.sel1.oad.value = 0x%08x\n", oopRsd.sel1.oad.value);
    PRTL_FMT_LOGCC("oopRsd.sel1.value.wVal = %d\n", oopRsd.sel1.value.wVal);
    PRTL_FMT_LOGCC("offset = %d\n", offset);
    if (nRet != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("RSD解析错误 错误码[nRet]\n", nRet);

        /*应对电科院协议一致性测试*/
        uint16 testOffset = 4;
        uint16 inCsdOff = 0, inCsdLen = 0;
        apdu_next_dpos_get(inData, &testOffset, InDataLen, DT_RSD, TRUE);
        inCsdOff = testOffset;
        apdu_next_dpos_get(inData, &testOffset, InDataLen, DT_RCSD, TRUE);
        inCsdLen = testOffset - inCsdOff;
        memcpy(outData->pResultData, inData, 4);
        memcpy(outData->pResultData + 4, inData + inCsdOff, inCsdLen);
        outData->resultDataLen = 4 + inCsdLen;

        outData->pResultData[outData->resultDataLen++] = 1;                   //后面优化下，outData->resultDataLen与下面统一
        if (apdu_rcsd_fromfmt698((uint8 *)inData, InDataLen, &offset, &oopRcsd, sizeof(oopRcsd)) != DATA_SUCCESS)
        {
            outData->pResultData[outData->resultDataLen++] = 0;

        }
        else
        {
            outData->pResultData[outData->resultDataLen++] = 1;
            for (i = 0; i < oopRcsd.nNum; i++)
            {
                outData->pResultData[outData->resultDataLen++] = NULL_DATA;
            }
        }

        free(outRecordBuf.recordBuf);
        outRecordBuf.recordBuf = NULL;
        return DATA_SUCCESS;
    }
    else
    {
        inDataLenTmp -= offset;  //除去RSD和OAD后的长度
    }

    /*处理RCSD*/
    csdStart = offset;
    if (apdu_rcsd_fromfmt698(inData, InDataLen, &offset, &oopRcsd, sizeof(oopRcsd)) != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("CSD解析错误! offset[%d]\n", offset);
        free(outRecordBuf.recordBuf);
        outRecordBuf.recordBuf = NULL;
        return DATA_TYPE_UNMATCHED;
    }

    /*填写OAD+RCSD+记录结果1 并记下偏移headLen*/
    outData->resultDataLen = 0;
    csdLen = offset-csdStart;
    memcpy(outData->pResultData, inData, 4);
    outOffset = 4;

    if (csdLen == 1)
    {
        //如果rcsd为空，填充OAD6e240200
        outData->pResultData[outOffset ++] = 0x01;
        outData->pResultData[outOffset ++] = 0x00;
        outData->pResultData[outOffset ++] = 0x6e;
        outData->pResultData[outOffset ++] = 0x24;
        outData->pResultData[outOffset ++] = 0x02;
        outData->pResultData[outOffset ++] = 0x00;
    }
    else
    {
        memcpy(&outData->pResultData[outOffset], &inData[csdStart], csdLen);
        outOffset += csdLen;
    }
    outData->pResultData[outOffset++] = 1;
    outData->resultDataLen += outOffset;
    headLen = outOffset;
    PRTL_BUF_LOGCC(&outData->pResultData[0], headLen, "填写OAD+RCSD+记录结果1 并记下偏移headLen head:");

    /*记录型档案接口提供输入参数*/
    if (general_param_get_acq_status(&oopRsd, &oopRcsd, pApduInfo, &inReadNor, &relyInfo) != ERR_OK)
    {
        free(outRecordBuf.recordBuf);
        outRecordBuf.recordBuf = NULL;
        return DATA_READ_WRITE_DENIED;
    }

    /* 依赖信息中存下OAD列表的指针 */
    relyInfo.pOadInfo = pOdaInfo;

    PRTL_FMT_LOGCC("****************************************************\n");
    PRTL_BUF_LOGCC(&inReadNor.tsa.add[0], 6, "筛选的通信地址:");
    PRTL_FMT_LOGCC("relyInfo.oopRcsd.nNum = %d\n", relyInfo.oopRcsd.nNum);
    for(i = 0; i < relyInfo.oopRcsd.nNum; i++)
    {
        if (relyInfo.oopRcsd.cols[i].choice == 0)
        {
            PRTL_FMT_LOGCC("choice = %d relyInfo.oopRcsd.cols[%d].oad.value = 0x%08x\n", i, relyInfo.oopRcsd.cols[i].choice, 
                                                                                      relyInfo.oopRcsd.cols[i].oad.value);
        }
        else if(relyInfo.oopRcsd.cols[i].choice == 1)
        {
            PRTL_FMT_LOGCC("choice = %d relyInfo.oopRcsd.cols[%d].road.oadMain.value = 0x%08x\n", i, relyInfo.oopRcsd.cols[i].choice, 
                                                                                              relyInfo.oopRcsd.cols[i].road.oadMain.value);
            for (j = 0; j < relyInfo.oopRcsd.cols[i].road.oadCols.nNum; j++)
            {
                PRTL_FMT_LOGCC("choice = %d relyInfo.oopRcsd.cols[%d].road.oadCols.oad[%d].value = 0x%08x\n", i, j, relyInfo.oopRcsd.cols[i].choice, 
                                                                                                          relyInfo.oopRcsd.cols[i].road.oadCols.oad[j].value);
            }
        }
    }
    PRTL_FMT_LOGCC("****************************************************\n");

    /*读记录档案接口，可以读出apdu数据域，可返回断点信息*/
    if (record_acq_status_read(pApduInfo->hUdp, inReadNor, relyInfo, pApduInfo->connectionInfo.ApplSplitLen - headLen - cal_excludeLen(pApduInfo->connectionInfo), &outRecordBuf))
    {
        nRet = DATA_READ_WRITE_DENIED;
    }
    else
    {
        memcpy(&outData->pResultData[headLen], outRecordBuf.recordBuf, outRecordBuf.bufLen);
        outData->resultDataLen += outRecordBuf.bufLen;

        if (outRecordBuf.breakInfo.appSlicingFlag == 1)
        {
            /*记下断点信息*/
            PRTL_FMT_LOGCC("记下断点信息\n");
            outRecordBuf.breakInfo.objPre = 0x02;   //对象属性，记录型数据
            memcpy(pApduInfo->bpBreakInfo, &outRecordBuf.breakInfo, sizeof(BP_INFO_T));
        }
        else
        {
            /*清除断点信息*/
            PRTL_FMT_LOGCC("清除断点信息\n");
            memset(pApduInfo->bpBreakInfo, 0, sizeof(BP_INFO_T));
        }
        
        nRet = DATA_SUCCESS;
    }
    
    free(outRecordBuf.recordBuf);
    outRecordBuf.recordBuf = NULL;
    return DATA_SUCCESS;
}


/**
*********************************************************************
* @name：      class11_invoke_get_transparent_node_all
* @brief：     class11的GET服务
* @param[in] ：pOdaInfo        OAD信息
               pBpInfo        应用分帧断点信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
               outDataMax     输出报文缓冲区最大长度

* @param[out]：outData     输出报文
* @return    ：OOP_DAR_E
* @author    :
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E class11_invoke_get_acq_status_all(OAD_INFO_T* pOadInfo, APDU_INFO_T* pApduInfo, const uint8* inData,
    uint16 inLen, RESULT_DATA_T* outData, uint16 outDataMax)
{
    //如果是应用分帧状态，将apdu[1]修改成起始帧方式
    if (pApduInfo->bpBreakInfo->appSlicingFlag == 1)
    {
        pApduInfo->reqType = pApduInfo->bpBreakInfo->reqType;
    }

    switch (pApduInfo->reqType)
    {
    case 0x01:
    case 0x02:
        return class11_invoke_get_acq_status_all_nor(pOadInfo, pApduInfo, inData, inLen, outData, outDataMax);
        break;
    case 0x03:
    case 0x04:
        return class11_invoke_get_acq_status_all_rec(pOadInfo, pApduInfo, inData, inLen, outData, outDataMax);
        break;
    default:
        break;
    }

    return DATA_OBJECT_UNDEFINED;
}

#endif

#if DESC("读取透明事件内容", 1)
/**
*********************************************************************
* @brief：      CSD_T -- > OOP_RCSD
* @param[in]： pCSD          - 列选择
* @param[out]：OOP_RCSD      - 列选择
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
int32 oop_rcsd_get_event(CSD_T *pCSD, OOP_RCSD_T *oopRcsd)
{
    OOP_RCSD_T oopRcsdTmp = {0};
    uint32 num = 0;
    uint32 i   = 0;
    uint32 j   = 0;
    CSD_T *pCsdTmp = pCSD;

    if (oopRcsd == NULL)
    {
        PRTL_FMT_DEBUG("oop_rcsd_get input para null oopRcsd[%p]\n", oopRcsd);
        return ERR_PTR;
    }

    /*先获取rcsd数量*/
    while(pCsdTmp)
    {
        num ++;
        pCsdTmp = pCsdTmp->next;
    }

    if (num == 0)
    {
        PRTL_FMT_DEBUG("空RCSD，虚拟6e320200\n");
        oopRcsdTmp.nNum = 1;
        oopRcsdTmp.cols[0].choice = 0;
        oopRcsdTmp.cols[0].oad.value = 0x6e320200;
    }
    else
    {
        pCsdTmp = pCSD;
        oopRcsdTmp.nNum = num;
        PRTL_FMT_LOGCC("oop_rcsd_get num = %d\n", num);
        for (i = 0; i < num; i++)
        {
            PRTL_FMT_LOGCC("oop_rcsd_get pCsdTmp->CSDType = %d\n", pCsdTmp->CSDType);
            if (pCsdTmp->CSDType == 0)
            {
                oopRcsdTmp.cols[i].choice = 0;
                oopRcsdTmp.cols[i].oad = pCsdTmp->Oad;
                PRTL_FMT_LOGCC("choice = %d oopRcsdTmp.cols[%d].oad = 0x%08X\n", oopRcsdTmp.cols[i].choice, i, pCsdTmp->Oad.value);
            }
            else if(pCsdTmp->CSDType == 1)
            {
                oopRcsdTmp.cols[i].choice = 1;
                oopRcsdTmp.cols[i].road.oadMain = pCsdTmp->Road.MainOad;
                oopRcsdTmp.cols[i].road.oadCols.nNum = pCsdTmp->Road.SubOadNum;
                PRTL_FMT_LOGCC("choice = %d oopRcsdTmp.cols[%d].road.oadMain = 0x%08x\n", oopRcsdTmp.cols[i].choice, i, oopRcsdTmp.cols[i].road.oadMain);
                PRTL_FMT_LOGCC("choice = %d oopRcsdTmp.cols[%d].road.oadCols.nNum = %d\n", oopRcsdTmp.cols[i].choice, i, oopRcsdTmp.cols[i].road.oadCols.nNum);

                if (oopRcsdTmp.cols[i].road.oadCols.nNum == 0)
                {
                    PRTL_FMT_DEBUG("oop_rcsd_get err oopRcsdTmp.cols[%d].road.oadCols.nNum = [%d]\n", i, oopRcsdTmp.cols[i].road.oadCols.nNum);
                    return ERR_OPTION;
                }

                for (j = 0; j < oopRcsdTmp.cols[i].road.oadCols.nNum; j++)
                {
                    oopRcsdTmp.cols[i].road.oadCols.oad[j] = pCsdTmp->Road.SubOad[j];
                    PRTL_FMT_LOGCC("choice = %d oopRcsdTmp.cols[%d].road.oadCols.oad[%d] = 0x%08x\n", oopRcsdTmp.cols[i].choice, i, j, oopRcsdTmp.cols[i].road.oadCols.oad[j]);
                }
            }
            pCsdTmp = pCsdTmp->next;
        }
    }

    *oopRcsd = oopRcsdTmp;

    return ERR_OK;
}

/**
*********************************************************************
* @brief：      data_to_buf_report_event_fujian
* @param[in]： ppOadInfo     - 数据表指针
               srcBuf       - 源数据
               srcLen       - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen       - 目的数据长度
               sOffset      - 转换后源数据区的偏移
               dOffset      - 转换后目的数据区的偏移
* @return：     =0           - 成功
              !=0           - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_event_status(const OAD_INFO_T* pOadInfo, DATA_CONVERT_T* convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_TRANS_EVENT_STATUS_T data;
    uint8* srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8* dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    memcpy(&data, &srcBuf[sOffset], sizeof(data));
    sOffset += sizeof(data);

    //结构类型
    dstBuf[dOffset++] = DT_STRUCTURE;

    //结构成员数
    dstBuf[dOffset++] = 3;

    //最近接收事件存储时标
    dar = basic_data_to_buf(E_DATETIME_S, dstBuf, dstLen, &dOffset, &data.lastRecvTime, sizeof(data.lastRecvTime));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), data.lastRecvTime.hour(%d)\n", dar, data.lastRecvTime.hour);
        return DATA_TYPE_UNMATCHED;
    }

    //最近接收事件存储序号
    dar = basic_data_to_buf(E_LONG_UNSIGNED, dstBuf, dstLen, &dOffset, &data.lastRecvNo, sizeof(data.lastRecvNo));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), data.lastRecvNo(%d)\n", dar, data.lastRecvNo);
        return DATA_TYPE_UNMATCHED;
    }

    //已接收确认报文条数
    dar = basic_data_to_buf(E_LONG_UNSIGNED, dstBuf, dstLen, &dOffset, &data.recvSum, sizeof(data.recvSum));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), recvSum(%d)\n", dar, data.recvSum);
        return DATA_TYPE_UNMATCHED;
    }

    //避免每步都判断，放最后检查，但检查前可能已经越界
    if ((sOffset > srcLen) || (dOffset > dstLen))
    {
        PRTL_FMT_DEBUG("cross the border. sOffset(%d), srcLen(%d), dOffset(%d), dstLen(%d)\n", sOffset, srcLen, dOffset, dstLen);
        return DATA_SCOPE_OF_ACCESS_VIOLATED;
    }

    *convert->dOffset = dOffset;
    *convert->sOffset = sOffset;

    return dar;
}

/**
*********************************************************************
* @brief：     该记是否是需要的
* @param[in]： inReadRecord  - 筛选需要的数据
               unit          - 单个数据记录单元
* @return：    =0            - 该记录是所需
               <0            - 该记录不是所需
*********************************************************************
*/
int record_event_is_valid_trans(READ_RECORD_T inReadRecord, RECORD_UNIT_T unit)
{
    int32     ret = ERR_NORMAL;

    PRTL_FMT_LOGCC("******************打印数据中心数据*******************************\n");
    PRTL_FMT_LOGCC("单步读筛选OAD inReadRecord.recordOAD.road.oadMain = 0x%08x\n", inReadRecord.recordOAD.road.oadMain.value);
    PRTL_FMT_LOGCC("单步读筛选OAD inReadRecord.recordOAD.road.oadCols.nNum = %d\n", inReadRecord.recordOAD.road.oadCols.nNum);
    uint8 i = 0;
    uint8 j = 0;
    for (i = 0; i < inReadRecord.recordOAD.road.oadCols.nNum; i++)
    {
        PRTL_FMT_LOGCC("单步读筛选OAD inReadRecord.recordOAD.road.oadCols.oad[%d] = 0x%08x\n", i, inReadRecord.recordOAD.road.oadCols.oad[i].value);
    }
    switch (inReadRecord.cType)
    {
    case COL_TM_START: {PRTL_FMT_LOGCC("单步读筛选方式 inReadRecord.cType:%s\n", "采集起始时间");     break; }
    case COL_TM_END:   {PRTL_FMT_LOGCC("单步读筛选方式 inReadRecord.cType:%s\n", "采集结束时间");     break; }
    case COL_TM_STORE: {PRTL_FMT_LOGCC("单步读筛选方式 inReadRecord.cType:%s\n", "采集存储时间");     break; }
    case COL_NO_STORE: {PRTL_FMT_LOGCC("单步读筛选方式 inReadRecord.cType:%s\n", "数据存储序号");     break; }
    case COL_NO_INFO:  {PRTL_FMT_LOGCC("单步读筛选方式 inReadRecord.cType:%s\n", "信息点号");         break; }
    case COL_NOT:      {PRTL_FMT_LOGCC("单步读筛选方式 inReadRecord.cType:%s\n", "不筛选");           break; }
    default:           {PRTL_FMT_LOGCC("单步读筛选方式 inReadRecord.cType:%s\n", "不正确的筛选类型");  break; }
    }
    PRTL_FMT_TIME("单步读结果 unit.colStartTm", unit.colStartTm);
    PRTL_FMT_LOGCC("单步读结果 unit.colStoreTm[%d]\n", unit.colStoreTm);
    PRTL_FMT_TIME("单步读结果 unit.colEndTm", unit.colEndTm);
    PRTL_FMT_LOGCC("单步读结果 unit.recordnum = %d\n", unit.recordnum);
    for (i = 0; i < unit.recordnum; i++)
    {
        PRTL_FMT_LOGCC("单步读结果 unit.record[%d].road.oadMain.value = 0x%08x\n", i, unit.record[i].road.oadMain.value);
        PRTL_FMT_LOGCC("单步读结果 unit.record[%d].road.oadMain.nNum = %d\n", i, unit.record[i].road.oadCols.nNum);
        for (j = 0; j < unit.record[i].road.oadCols.nNum; j++)
        {
            PRTL_FMT_LOGCC("单步读结果 unit.record[%d].road.oadCols.oad[%d].value = 0x%08x\n", i, j, unit.record[i].road.oadCols.oad[j].value);
        }
        PRTL_BUF_LOGCC(unit.record[i].outDataBuf, unit.record[i].outDatalen, "单步读结果 unit.record[%d].outDataBuf:", i);
    }
    PRTL_FMT_LOGCC("******************打印数据中心数据结束***************************\n");

    //检测数据中心内容是否符合要求
    for (i = 0; i < unit.recordnum; i++)
    {
        if ((unit.record[i].road.oadCols.oad[0].value != 0x6E310200) || 
            (unit.record[i].outDatalen != sizeof(OOP_TRANS_EVENT_T)))
        {
            PRTL_FMT_DEBUG("单步读结果不符合规约要求 unit.record[%d].road.oadCols.oad[0].value = 0x%08x unit.record[%d].outDatalen = %d\n", 
                i, unit.record[i].road.oadCols.oad[0].value, i, unit.record[i].outDatalen);
            ret = ERR_ITEM;
            break;
        }
    }

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

    return ret;
}

/**
*********************************************************************
* @brief：     电表数据转换成698格式报文
               默认每次单步出来的数据是个完整的记录（同一个测量点，相同时间），不需要再拼接
* @param[in]： unit          - 单个数据记录单元
               oopRcsd       - 列刷选
* @param[out]：ppOutBuf      - 698格式报文
               outLen        - 报文长度
               pOadNum       - OAD数量
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
int transparent_event_to_698buf_malloc(RECORD_UNIT_T unit, OOP_RCSD_T oopRcsd, uint16* pRecNum, uint32 index, uint8** ppOutBuf, uint32* outLen)
{
    OOP_DAR_E      dar;
    int32          nRet = ERR_NORMAL;
    OAD_INFO_T     pOadInfo = { 0 };
    uint8          *pOutBuf = NULL;
    IC_OAD_INFO_T  icInfo = { 0 };
    DATA_CONVERT_T convert = { 0 };
    uint32         dOffset = 0;
    uint32         sOffset = 0;
    uint32         offset = 0;
    uint32         i = 0;
    uint32         j = 0;
    DateTimeHex_t  oopTime_s;
    uint32         tmpTime = 0;

    OOP_TRANS_EVENT_T data;

    memset(&data, 0, sizeof(data));

    /* 入参检查 */
    if ((unit.recordnum == 0) || (outLen == NULL) || (pRecNum == NULL))
    {
        PRTL_FMT_DEBUG("福建规约，组报文接口参数错误. unit.recordnum[%d] recNum[%p], outLen[%d]\n", unit.recordnum, pRecNum, outLen);
        return ERR_OPTION;
    }

    /* 此接口只做做单OAD数据 */
    for (i = 0; i < oopRcsd.nNum; i++)
    {
        if (oopRcsd.cols[i].choice == 1)
        {
            PRTL_FMT_DEBUG("福建只做做单OAD数据 oopRcsd.cols[%d].choice[%d] err\n", i, oopRcsd.cols[i].choice);
            return ERR_OPTION;
        }
    }

    pOutBuf = (uint8*)malloc(DATA_LEN_MAX);//暂时这么定

    for (j = 0; j < unit.recordnum; j++)
    {
        //此接口每次这组织一个记录的报文
        if (j != index)
        {
            continue;
        }

        //参数检查
        if ((unit.record[j].road.oadCols.nNum == 0) || (unit.record[j].road.oadMain.value != 0x0))
        {
            PRTL_FMT_DEBUG("福建规约透明结果中不应该存在主次OAD unit.record[%d]\n", j);
            free(pOutBuf);
            pOutBuf = NULL;
            return ERR_OPTION;
        }

        memcpy(&data, unit.record[j].outDataBuf, sizeof(data));
        data.curNum = *pRecNum;
        PRTL_FMT_DEBUG("事件记录数按实际数量重新回调 data.curNum[%d]\n", data.curNum);

        for (i = 0; i < oopRcsd.nNum; i++)
        {
            memset((uint8*)&pOadInfo, 0x00, sizeof(OAD_INFO_T));
            memset((uint8*)&icInfo, 0x00, sizeof(IC_OAD_INFO_T));
            if (oopRcsd.cols[i].oad.value == 0x60400200)
            {
                PRTL_FMT_TIME("采集启动时标", unit.colStartTm);
                // 注：-8个小时(28800)转成北京时间
                tmpTime = unit.colStartTm - COMPENSATION_TIME;
                if (!DT_Time2DateTimeHex(tmpTime, &oopTime_s))
                {
                    PRTL_FMT_DEBUG("组织采集启动时标错误!\n");
                }
                pOutBuf[offset++] = DT_DATETIME_S;
                memcpy(&pOutBuf[offset], &oopTime_s, sizeof(DateTimeHex_t));
                offset += sizeof(DateTimeHex_t);
                continue;
            }
            else if (oopRcsd.cols[i].oad.value == 0x60410200)
            {
                PRTL_FMT_TIME("采集成功时标", unit.colEndTm);
                // 注：-8个小时(28800)转成北京时间
                tmpTime = unit.colEndTm - COMPENSATION_TIME;
                if (!DT_Time2DateTimeHex(tmpTime, &oopTime_s))
                {
                    PRTL_FMT_DEBUG("组织采集成功时标错误!\n");
                }
                pOutBuf[offset++] = DT_DATETIME_S;
                memcpy(&pOutBuf[offset], &oopTime_s, sizeof(DateTimeHex_t));
                offset += sizeof(DateTimeHex_t);
                continue;
            }
            else if (oopRcsd.cols[i].oad.value == 0x60420200 || oopRcsd.cols[i].oad.value == 0x20210200)
            {
                PRTL_FMT_TIME("采集存储时标", unit.colStoreTm);
                // 注：-8个小时(28800)转成北京时间
                tmpTime = unit.colStoreTm - COMPENSATION_TIME;
                if (!DT_Time2DateTimeHex(tmpTime, &oopTime_s))
                {
                    PRTL_FMT_DEBUG("组织采集存储时标失败!\n");
                }
                pOutBuf[offset++] = DT_DATETIME_S;
                memcpy(&pOutBuf[offset], &oopTime_s, sizeof(DateTimeHex_t));
                offset += sizeof(DateTimeHex_t);
                continue;
            }
            else if (oopRcsd.cols[i].oad.value == 0x40010200 || oopRcsd.cols[i].oad.value == 0x202A0200)
            {
                pOutBuf[offset++] = DT_TSA;
                pOutBuf[offset++] = 0x07;
                pOutBuf[offset++] = 0x05;
                memcpy(&pOutBuf[offset], &data.add.add[0], sizeof(OOP_OCTETSTR6_T));
                offset += sizeof(OOP_OCTETSTR6_T);
                continue;
            }
            else if (!class_oadinfo_get(oopRcsd.cols[i].oad, &pOadInfo))
            {
                PRTL_FMT_DEBUG("RCSD中不存在这个oad[0x%08x]\n", oopRcsd.cols[i].oad.value);
                pOutBuf[offset++] = DT_NULL;
                continue;
            }

            icInfo.choice = 1;
            icInfo.pstOdaInfo = &pOadInfo;

            convert.dstBuf = &(pOutBuf[offset]);
            convert.dstLen = DATA_LEN_MAX - offset;
            convert.dOffset = &dOffset;

            switch (pOadInfo.nIndex)
            {
            case 0:
                convert.srcBuf = &data;
                convert.srcLen = sizeof(data);
                break;
            case 1:
                convert.srcBuf = &data.curNum;
                convert.srcLen = sizeof(data.curNum);
                break;
            case 2:
                convert.srcBuf = &data.selfStoreNo;
                convert.srcLen = sizeof(data.selfStoreNo);
                break;
            case 3:
                convert.srcBuf = &data.reportTime;
                convert.srcLen = sizeof(data.reportTime);
                break;
            case 4:
                convert.srcBuf = &data.add;
                convert.srcLen = sizeof(data.add);
                break;
            case 5:
                convert.srcBuf = &data.commType;
                convert.srcLen = sizeof(data.commType);
                break;
            case 6:
                convert.srcBuf = &data.oad;
                convert.srcLen = sizeof(data.oad);
                break;
            case 7:
                convert.srcBuf = &data.messType;
                convert.srcLen = sizeof(data.messType);
                break;
            case 8:
                convert.srcBuf = &data.data;
                convert.srcLen = sizeof(data.data);
                break;
            default:
                break;
            }

            convert.sOffset = &sOffset;

            //组织698格式报文
            dar = normal_data_to_fmt698(&icInfo, &convert);
            if (dar != DATA_SUCCESS)
            {
                PRTL_FMT_DEBUG("数据转换报文失败 dar[%d]\n", dar);
                pOutBuf[offset++] = DT_NULL;
                sOffset = 0;
                dOffset = 0;
                nRet = ERR_OK;
            }
            else
            {
                PRTL_BUF_LOGCC(convert.dstBuf, dOffset, "OAD[0x%08x]转换偏移offset[%d]，数据转换报文结果:", oopRcsd.cols[i].oad.value, offset);
                offset += dOffset;
                sOffset = 0;
                dOffset = 0;
                nRet = ERR_OK;
            }
        }
    }

    *ppOutBuf = pOutBuf;
    *outLen = offset;

    return nRet;
}
/**
*********************************************************************
* @brief：     获取基本方案信息
* @param[in]： pRSD          - 行选择
               pCSD          - 列选择
* @param[out]：pPlanTypeInfo - 目的数据
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
int32 read_transparent_event_param_get(RSD_T* pRSD, CSD_T* pCSD, APDU_INFO_T* pApduInfo, READ_RECORD_T* pInReadRecord, 
    RELY_INFO_T* pRelyInfo)
{
    int32          nRet = ERR_NORMAL;
    READ_RECORD_T  inReadRecord;
    RELY_INFO_T    relyInfo;
    CSD_T          *pCSDTmp = pCSD;
    uint32         startTm = 0;
    uint32         endTm = 0;
    memset(&inReadRecord, 0, sizeof(READ_RECORD_T));
    memset(&relyInfo, 0, sizeof(RELY_INFO_T));

    while (pCSDTmp)
    {
        if (pCSDTmp->CSDType == 1)
        {
            PRTL_FMT_DEBUG("福建规约不存在RCSD中存在ROAD的情况\n");
            return ERR_TYPE;
        }
        else
        {
            if ((pCSDTmp->Oad.nObjID != 0x6040) && (pCSDTmp->Oad.nObjID != 0x6041) && (pCSDTmp->Oad.nObjID != 0x2021)
                && (pCSDTmp->Oad.nObjID != 0x6042) && (pCSDTmp->Oad.nObjID != 0x4001) && (pCSDTmp->Oad.nObjID != 0x202A))
            {
                if (pCSDTmp->Oad.nObjID != 0x6E32)
                {
                    PRTL_FMT_DEBUG("福建规约不存在RCSD中不存在OI是0x6E32的情况\n");
                    return ERR_TYPE;
                }
            }
        }
        pCSDTmp = pCSDTmp->next;
    }

    inReadRecord.recordOAD.classtag = 0;
    inReadRecord.recordOAD.road.oadMain.value = 0;
    inReadRecord.recordOAD.road.oadCols.oad[0].value = 0x6E310200;
    inReadRecord.recordOAD.road.oadCols.nNum = 1;

    //因为oopType.h中宏的限制，目前关联OAD最大数量是OOP_MAX_OADS，对福建来说，有点多余
    if (inReadRecord.recordOAD.road.oadCols.nNum > OOP_MAX_OADS)
    {
        return DATA_SCOPE_OF_ACCESS_VIOLATED;
    }

    /*填写逻辑地址*/
    inReadRecord.recordOAD.logicId = 0;

    /*填写信息点*/
    inReadRecord.recordOAD.infoNum = 0;

    switch (pRSD->selId)
    {
    case 50:
        {
            //福建透明事件存储时，将存储序号存入存储时间中
            inReadRecord.cStart = 0;
            inReadRecord.cEnd = 0xffff + 1;
            inReadRecord.cType = COL_TM_STORE;

            #if 1
            relyInfo.LastN = pRSD->LastN;;
            relyInfo.recNum = pRSD->recNum;
            #else
            //福建要求升序排序，下面方法废弃
            //福建规约，透明事件倒序排列后，要往上查事件，从第N数M个
            //代码设计是往下查事件 这里需要做个转换，转成往下数
            if (pRSD->LastN < pRSD->recNum)
            {
                relyInfo.LastN = 1;
                relyInfo.recNum = pRSD->LastN;
            }
            else
            {
                relyInfo.LastN = pRSD->LastN - pRSD->recNum + 1;
                relyInfo.recNum = pRSD->recNum;
            }
            #endif
            
            nRet = ERR_OK;
            break;
        }
    case 51:
    case 52:
    case 53:
        {
            if (!DT_DateTimeHex2Time((DateTimeHex_t*)pRSD->StartData, (time_t*)&startTm) ||
                (!DT_DateTimeHex2Time((DateTimeHex_t*)pRSD->EndData, (time_t*)&endTm)))
            {
                return DATA_DATA_BLOCK_UNAVAILABLE;
            }
            else
            {
                char strT[128];
                strftime(strT, sizeof(strT), "%Y-%m-%d %H:%M:%S", localtime((time_t*)&startTm));
                PRTL_FMT_LOGCC("read_record_param_get startTm:%s time[s]:%d\n", strT, startTm);
                strftime(strT, sizeof(strT), "%Y-%m-%d %H:%M:%S", localtime((time_t*)&endTm));
                PRTL_FMT_LOGCC("read_record_param_get endTm:%s time[s]:%d\n", strT, endTm);

                // 注：+8个小时(28800)转成格林时间
                startTm += COMPENSATION_TIME;
                endTm += COMPENSATION_TIME;
            }

            if (pRSD->selId == 51)
            {
                inReadRecord.cStart = startTm;
                inReadRecord.cEnd = startTm;
                inReadRecord.cType = COL_TM_STORE;
            }
            else if (pRSD->selId == 52)
            {
                inReadRecord.cStart = startTm;
                inReadRecord.cEnd = startTm;
                inReadRecord.cType = COL_TM_STORE;
                relyInfo.nodeidNum = pRSD->nodeidNum;
                memcpy(&relyInfo.nodeid[0], &pRSD->nodeid[0], NODE_ID_NUM_MAX * sizeof(uint16));
            }
            else if (pRSD->selId == 53)
            {
                inReadRecord.cStart = startTm;
                inReadRecord.cEnd = endTm;
                inReadRecord.cType = COL_TM_STORE;
            }
        }
        nRet = ERR_OK;
        break;
    case 54:
        {
            // 注：+8个小时(28800)转成格林时间
            time_t  RealTime_t = time(NULL) + COMPENSATION_TIME;
            inReadRecord.cStart = 0;
            inReadRecord.cEnd = RealTime_t;
            inReadRecord.cType = COL_TM_STORE;
            
            //福建规约，透明数据倒序排列后，要往上查数据，从第N数M个
            //代码设计是往下查数据 这里需要做个转换，转成往下数
            if (pRSD->LastN < pRSD->recNum)
            {
                relyInfo.LastN = 1;
                relyInfo.recNum = pRSD->LastN;
            }
            else
            {
                relyInfo.LastN = pRSD->LastN - pRSD->recNum + 1;
                relyInfo.recNum = pRSD->recNum;
            }
        }
        nRet = ERR_OK;
        break;
    default:
        nRet = DATA_READ_WRITE_DENIED;
        break;
    }

    /*pCSD -- > oopRcsd*/
    pCSDTmp = pCSD;
    if (oop_rcsd_get_event(pCSDTmp, &relyInfo.oopRcsd) != ERR_OK)
    {
        nRet = ERR_OPTION;
    }

    if (nRet == ERR_OK)
    {
        relyInfo.bpBreakInfo = pApduInfo->bpBreakInfo;
        relyInfo.ms = pRSD->Ms;
        relyInfo.isAllmeter = pRSD->isAllmeter;
        *pInReadRecord = inReadRecord;
        *pRelyInfo = relyInfo;
    }

    return nRet;
}

/**
*********************************************************************
* @brief：     读透明结果集合
* @param[in]： hUdp          - 数据中心句柄
               inReadRecord  - 筛选需要的数据
               relyInfo      - 依赖数据
               readRecNum    - 读记录数量，：0：读全部记录 n：读最近的n条记录
               maxBufLen     - 最大读取buf长度
* @param[out]：outRecordBuf  - 数据记录
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
int record_transparent_event_read(DB_CLIENT hUdp, READ_RECORD_T inReadRecord, RELY_INFO_T relyInfo,
    uint32 maxBufLen, RECORD_BUF_T* outRecordBuf)
{
    READ_RECORD_T      inReadRecordTmp;
    STEP_READ_RECORD_T stepReadRecord;
    RECORD_UNIT_T      stepRecordOut;
    uint8              *pBufTmp = NULL;
    uint32             bufLenTmp = 0;
    uint32             offset = 0;

    uint8              *recordBufTmp = NULL;
    uint32             bufAllLen = 0;
    uint32             bufAllLenLast = 0;
    BOOL               isBufFull = FALSE;
    BOOL               isMaxRec = FALSE; 
    
    OOP_RCSD_T         oopRcsdtmp;

    int32  ret = ERR_NORMAL;
    uint32 handle = 0;
    uint32 recNum = 0;
    uint32 oopRecNumPn = 0;       //单个记录数索引
    uint32 oopRecNumAll = 0;      //总记录表中记录条数
    uint32 len = 0;
    uint8  offlen = 0;
    uint8  lenBuf[3] = { 0 };
    uint16 index = 0;
    uint16 curNum = 0;
    uint32 maxStorageNo = 0;

    memset(&inReadRecordTmp, 0, sizeof(inReadRecordTmp));
    memset(&oopRcsdtmp, 0, sizeof(oopRcsdtmp));

    //入参检查
    if ((hUdp <= 0) || (outRecordBuf == NULL) || (outRecordBuf->recordBuf == NULL))
    {
        PRTL_FMT_DEBUG("福建功能，读透明结果入参错误 hUdp[%d] outRecordBuf[%d] outRecordBuf->recordBuf[%d]!\n",
            hUdp, outRecordBuf, outRecordBuf->recordBuf);
        return ERR_PTR;
    }
    else
    {
        PRTL_FMT_LOGCC("福建功能，读透明结果入参成功 maxBufLen[%d]\n", maxBufLen);
    }

    if ((relyInfo.LastN == 0) || (relyInfo.recNum == 0))
    {
        PRTL_FMT_DEBUG("福建功能，读透明结果接口调用错误 relyInfo.LastN[%d] relyInfo.recNum\n", relyInfo.LastN, 
            relyInfo.recNum);
        return ERR_PTR;
    }

    ret = get_trans_event_status(hUdp, NULL, &curNum, &maxStorageNo);
    if (ret != ERR_OK)
    {
        PRTL_FMT_DEBUG("福建规约，获取事件当前计数器失败 ret[%d] infoNum[%d] curNum[%d] maxStorageNo[%d]\n",
            ret, inReadRecordTmp.recordOAD.infoNum, curNum, maxStorageNo);
        return ERR_EXECFAIL;
    }

    //临时变量，备份作用
    inReadRecordTmp = inReadRecord;
    oopRcsdtmp = relyInfo.oopRcsd;

    //正序从数据中心取数据，福建要求存储序号从小到大排序
    inReadRecordTmp.sortType = UP_SORT;

    //清除断点信息, 需要注意应用分帧的未帧
    memset(&outRecordBuf->breakInfo, 0, sizeof(BP_INFO_T));

    //申请一份内存，注意释放
    recordBufTmp = (uint8*)malloc(RECORD_LEN_MAX);
    memset(recordBufTmp, 0, RECORD_LEN_MAX);

    //记录数先存0，OAD+RCSD+1 已经在上层接口中完成 
    offset = 0;
    recordBufTmp[offset++] = 0x00;

    //如果数据已经回滚，则需要补充回滚前的数据，存储序号从1开始,最大65535(0xffff)
    if (maxStorageNo < relyInfo.LastN)
    {
        inReadRecord.cStart = (0xffff + 1) - (relyInfo.LastN - maxStorageNo) + 1;
        inReadRecord.cEnd = maxStorageNo + 1;
    }
    else
    {
        inReadRecord.cStart = maxStorageNo - relyInfo.LastN + 1;
        inReadRecord.cEnd = maxStorageNo + 1;
    }

    //福建规约中MAC表地址用于筛选数据，代替以前的信息点，0表示所有表
    inReadRecordTmp.table.nNum = 0x00;
    memset(&inReadRecordTmp.table.value[0], 0, 6);
    inReadRecordTmp.bTable = 0x01;

    //信息点选0
    inReadRecordTmp.recordOAD.infoNum = 0;

    //次OAD填透明事件OAD
    inReadRecordTmp.recordOAD.road.oadCols.oad[0].value = 0x6e310200;

    /*先读回滚数据 ，再读正常数据*/
    /*开始读记录*/
    int i = 0;
    int k = 0;
    
    //针对福建特殊开始读接口，MAC填空
    stepReadRecord.MAC.nNum = 0x06;
    memset(&stepReadRecord.MAC.value[0], 0, 6);
    PRTL_BUF_LOGCC(&stepReadRecord.MAC.value[0], 6, "单步读的MAC校验: ");

    if (maxStorageNo < relyInfo.LastN)
    {
        PRTL_FMT_DEBUG("先读回滚前数据 start[%d] end[%d]\n", inReadRecord.cStart, 0xffff + 1);
        
        inReadRecordTmp.cStart = inReadRecord.cStart;
        inReadRecordTmp.cEnd = 0xffff + 1;

        //开始读需要使用福建特殊接口
        ret = db_read_record_start(hUdp, &inReadRecordTmp, &handle, &recNum);
        if ((ret != ERR_OK) || (recNum == 0))
        {
            PRTL_FMT_DEBUG("福建规约，开始读记录出错(回滚数据) ret[%d] infoNum[%d] recNum[%d]\n",
                ret, inReadRecordTmp.recordOAD.infoNum, recNum);

            if (handle != 0)
            {
                /*结束读记录*/
                db_read_record_end(hUdp, handle);
                handle = 0;
            }

            //将数据置0，然后面流程正常结束
            recNum = 0;
        }
        else
        {
            PRTL_FMT_DEBUG("福建规约，开始读记录成功(回滚数据) ret[%d] infoNum[%d] recNum[%d]\n",
                ret, inReadRecordTmp.recordOAD.infoNum, recNum);
        }

        if (recNum > (inReadRecordTmp.cEnd - inReadRecord.cStart))
        {
            PRTL_FMT_DEBUG("透明事件数量[%d] 超过回滚的数量[%d], 理论上不应该有这种情况\n", recNum, 
                (inReadRecordTmp.cEnd - inReadRecord.cStart));
            index = recNum - (inReadRecordTmp.cEnd - inReadRecord.cStart);
        }
                
        for (i = 0; i < recNum; i++)
        {
            ret = db_read_record_step(hUdp, handle, &stepReadRecord, &stepRecordOut);
            if (ret != ERR_OK)
            {
                PRTL_FMT_DEBUG("福建规约，单步读出错(回滚数据)err[%d]\n", ret);
                continue;
            }

            //超过存储的数据不要
            if (i < index)
            {
                continue;
            }
            else
            {
                index = 0;
            }

            /* 判断是否是所需记录单元 */
            ret = record_event_is_valid_trans(inReadRecordTmp, stepRecordOut);
            if (ret < ERR_OK)
            {
                continue;
            }

            for (k = 0; k < stepRecordOut.recordnum; k++)
            {
                ret = transparent_event_to_698buf_malloc(stepRecordOut, oopRcsdtmp, &curNum, k, &pBufTmp, &bufLenTmp);
                if (ret < ERR_OK)
                {
                    db_read_record_end(hUdp, handle);
                    PRTL_FMT_DEBUG("福建规约，组织单步报文错误(回滚数据) err[%d]!\n", ret);
                    handle = 0;
                    free(recordBufTmp);
                    recordBufTmp = NULL;
                    return ERR_NORMAL;
                }
                else
                {
                    //跳过已经处理过的事件
                    if (oopRecNumPn < relyInfo.bpBreakInfo->recNumSeg)
                    {
                        oopRecNumPn ++;
                        continue;
                    }
                    else
                    {
                        relyInfo.bpBreakInfo->recNumSeg = 0;
                    }
                    
                    if ((relyInfo.LastN > 0) && (relyInfo.recNum > 0))
                    {
                        if (oopRecNumPn >= relyInfo.recNum)
                        {
                            //达到该测量点的最大记录数，跳转到下一个测量点
                            oopRecNumPn = 0;
                            offset = 0;
                            isMaxRec = TRUE;
                            free(pBufTmp);
                            pBufTmp = NULL;
                            break;
                        }
                    }

                    /* 拷贝数据域内容到recordBufTmp */
                    memcpy(&recordBufTmp[offset], pBufTmp, bufLenTmp);
                    offset += bufLenTmp;

                    bufAllLenLast = bufAllLen;
                    bufAllLen += offset;

                    // 方法50实时记下断点信息
                    outRecordBuf->breakInfo.recNumSeg = oopRecNumPn;

                    len = offset;

                    if (bufAllLen <= maxBufLen)
                    {
                        memcpy(&outRecordBuf->recordBuf[bufAllLenLast], recordBufTmp, offset);
                        memset(&recordBufTmp[0], 0x00, RECORD_LEN_MAX);
                        oopRecNumPn ++;
                        oopRecNumAll++;
                        offset = 0;

                        //打印一个单步完整记录报文信息
                        PRTL_FMT_LOGCC("*****************打印一个单步完整记录信息*********************\n");
                        PRTL_FMT_LOGCC("stepRecordOut.infoNum = %d\n", stepRecordOut.infoNum);
                        PRTL_FMT_LOGCC("oopRecNumAll = %d\n", oopRecNumAll);
                        PRTL_FMT_LOGCC("bufAllLenLast = %d\n", bufAllLenLast);
                        PRTL_FMT_LOGCC("bufAllLen = %d\n", bufAllLen);
                        PRTL_FMT_LOGCC("len = %d\n", len);
                        PRTL_BUF_LOGCC(&outRecordBuf->recordBuf[bufAllLenLast], len, "outRecordBuf->recordBuf[%d]:", bufAllLenLast);
                        PRTL_FMT_LOGCC("*****************打印一个单步完整记录信息结束********************\n");
                    }
                    else
                    {
                        memset(&recordBufTmp[0], 0x00, RECORD_LEN_MAX);
                        offset = 0;
                        isBufFull = TRUE;

                        PRTL_FMT_DEBUG("读透明结果集合达到最大长度(回滚数据) isBufFull[%d] bufAllLen[%d] maxBufLen[%d]\n", isBufFull,
                            bufAllLen, maxBufLen);

                    }

                    free(pBufTmp);
                    pBufTmp = NULL;
                }

                //如果达到最大长度，跳出循环
                if ((isBufFull) && (bufAllLenLast > 0))
                {
                    break;
                }
            }

            //达到最大记录数
            if (isMaxRec)
            {
                break;
            }

            //如果达到最大长度，跳出循环
            if ((isBufFull) && (bufAllLenLast > 0))
            {
                break;
            }
        }
    }

    //读完回滚数据，再读正常数据或者直接读正常数据
    if ((!isMaxRec) && (!isBufFull || bufAllLenLast == 0))
    {
        if (maxStorageNo < relyInfo.LastN)
        {
            inReadRecordTmp.cStart = 0;
            inReadRecordTmp.cEnd = inReadRecord.cEnd;
        }
        else
        {
            inReadRecordTmp.cStart = inReadRecord.cStart;
            inReadRecordTmp.cEnd = inReadRecord.cEnd;
        }

        PRTL_FMT_DEBUG("读正常数据区间 start[%d] end[%d]\n", inReadRecordTmp.cStart, inReadRecordTmp.cEnd);

        //开始读需要使用福建特殊接口
        ret = db_read_record_start(hUdp, &inReadRecordTmp, &handle, &recNum);
        if ((ret != ERR_OK) || (recNum == 0))
        {
            PRTL_FMT_DEBUG("福建规约，开始读记录出错 ret[%d] infoNum[%d] recNum[%d]\n",
                ret, inReadRecordTmp.recordOAD.infoNum, recNum);

            if (handle != 0)
            {
                /*结束读记录*/
                db_read_record_end(hUdp, handle);
                free(recordBufTmp);
                recordBufTmp = NULL;
                handle = 0;
            }
            
            return ERR_ACCESS;
        }
        else
        {
            PRTL_FMT_DEBUG("福建规约，开始读记录成功 ret[%d] infoNum[%d] recNum[%d]\n",
                ret, inReadRecordTmp.recordOAD.infoNum, recNum);
        }

        if (recNum > (inReadRecord.cEnd - inReadRecord.cStart))
        {
            PRTL_FMT_DEBUG("透明事件数量[%d] 超过正常的数量[%d], 理论上不应该有这种情况\n", recNum, 
                (inReadRecord.cEnd - inReadRecord.cStart));
            index = recNum - (inReadRecord.cEnd - inReadRecord.cStart);
        }
                
        for (i = 0; i < recNum; i++)
        {
            ret = db_read_record_step(hUdp, handle, &stepReadRecord, &stepRecordOut);
            if (ret != ERR_OK)
            {
                PRTL_FMT_DEBUG("福建规约，单步读出错err[%d]\n", ret);
                continue;
            }

            //超过存储的数据不要
            if (i < index)
            {
                continue;
            }
            else
            {
                index = 0;
            }

            /* 判断是否是所需记录单元 */
            ret = record_event_is_valid_trans(inReadRecordTmp, stepRecordOut);
            if (ret < ERR_OK)
            {
                continue;
            }

            for (k = 0; k < stepRecordOut.recordnum; k++)
            {
                ret = transparent_event_to_698buf_malloc(stepRecordOut, oopRcsdtmp, &curNum, k, &pBufTmp, &bufLenTmp);
                if (ret < ERR_OK)
                {
                    db_read_record_end(hUdp, handle);
                    PRTL_FMT_DEBUG("福建规约，组织单步报文错误 err[%d]!\n", ret);
                    free(recordBufTmp);
                    recordBufTmp = NULL;
                    return ERR_NORMAL;
                }
                else
                {
                    //跳过已经处理过的事件
                    if (oopRecNumPn < relyInfo.bpBreakInfo->recNumSeg)
                    {
                        oopRecNumPn ++;
                        continue;
                    }
                    else
                    {
                        relyInfo.bpBreakInfo->recNumSeg = 0;
                    }
                    
                    if ((relyInfo.LastN > 0) && (relyInfo.recNum > 0))
                    {
                        if (oopRecNumPn >= relyInfo.recNum)
                        {
                            //达到该测量点的最大记录数，跳转到下一个测量点
                            oopRecNumPn = 0;
                            offset = 0;
                            isMaxRec = TRUE;
                            free(pBufTmp);
                            pBufTmp = NULL;
                            break;
                        }
                    }

                    /* 拷贝数据域内容到recordBufTmp */
                    memcpy(&recordBufTmp[offset], pBufTmp, bufLenTmp);
                    offset += bufLenTmp;

                    bufAllLenLast = bufAllLen;
                    bufAllLen += offset;

                    // 方法50实时记下断点信息
                    outRecordBuf->breakInfo.recNumSeg = oopRecNumPn;

                    len = offset;

                    if (bufAllLen <= maxBufLen)
                    {
                        memcpy(&outRecordBuf->recordBuf[bufAllLenLast], recordBufTmp, offset);
                        memset(&recordBufTmp[0], 0x00, RECORD_LEN_MAX);
                        oopRecNumPn ++;
                        oopRecNumAll++;
                        offset = 0;

                        //打印一个单步完整记录报文信息
                        PRTL_FMT_LOGCC("*****************打印一个单步完整记录信息*********************\n");
                        PRTL_FMT_LOGCC("stepRecordOut.infoNum = %d\n", stepRecordOut.infoNum);
                        PRTL_FMT_LOGCC("oopRecNumAll = %d\n", oopRecNumAll);
                        PRTL_FMT_LOGCC("bufAllLenLast = %d\n", bufAllLenLast);
                        PRTL_FMT_LOGCC("bufAllLen = %d\n", bufAllLen);
                        PRTL_FMT_LOGCC("len = %d\n", len);
                        PRTL_BUF_LOGCC(&outRecordBuf->recordBuf[bufAllLenLast], len, "outRecordBuf->recordBuf[%d]:", bufAllLenLast);
                        PRTL_FMT_LOGCC("*****************打印一个单步完整记录信息结束********************\n");
                    }
                    else
                    {
                        memset(&recordBufTmp[0], 0x00, RECORD_LEN_MAX);
                        offset = 0;
                        isBufFull = TRUE;

                        PRTL_FMT_LOGCC("读透明结果集合达到最大长度 isBufFull[%d] bufAllLen[%d] maxBufLen[%d]\n", isBufFull,
                            bufAllLen, maxBufLen);

                    }

                    free(pBufTmp);
                    pBufTmp = NULL;
                }

                //如果达到最大长度，跳出循环
                if ((isBufFull) && (bufAllLenLast > 0))
                {
                    break;
                }
            }

            //达到最大记录数
            if (isMaxRec)
            {
                isMaxRec = FALSE;
                break;
            }

            //如果达到最大长度，跳出循环
            if ((isBufFull) && (bufAllLenLast > 0))
            {
                break;
            }
        }
    }

    if (!isBufFull)
    {
        outRecordBuf->bufLen = bufAllLen;

        if (relyInfo.bpBreakInfo->appSlicingFlag == 0x01)
        {
            outRecordBuf->breakInfo.reqType = 0x03;
            outRecordBuf->breakInfo.appSlicingFlag = 0x01;
            outRecordBuf->breakInfo.isEnd = 0x01;                          //末尾帧
            outRecordBuf->breakInfo.seq = relyInfo.bpBreakInfo->seq + 1;   //序号加1
        }
        else
        {
            /*清除断点信息*/
            memset(&outRecordBuf->breakInfo, 0, sizeof(BP_INFO_T));
        }
    }
    else
    {
        outRecordBuf->breakInfo.reqType = 0x03;
        outRecordBuf->breakInfo.appSlicingFlag = 0x01;
        outRecordBuf->breakInfo.isEnd = 0x00;                              //不是末尾帧
        if (relyInfo.bpBreakInfo->appSlicingFlag == 0x00)
        {
            outRecordBuf->breakInfo.seq = BRK_SEQ_START;                   //首次序号
        }
        else
        {
            outRecordBuf->breakInfo.seq = relyInfo.bpBreakInfo->seq + 1;   //序号加1
        }
        outRecordBuf->bufLen = bufAllLenLast;                               //填写返回报文长度
    }

    /*结束读记录*/
    db_read_record_end(hUdp, handle);

    /* 没有找到有效数据 满足协议一致性要求*/
    if (oopRecNumAll == 0)
    {
        if (relyInfo.bpBreakInfo->appSlicingFlag == 0x01)
        {
            outRecordBuf->breakInfo.reqType = 0x03;
            outRecordBuf->breakInfo.appSlicingFlag = 0x01;
            outRecordBuf->breakInfo.isEnd = 0x01;                          //末尾帧
            outRecordBuf->breakInfo.seq = relyInfo.bpBreakInfo->seq + 1;   //序号加1
        }

        outRecordBuf->bufLen = 1;
        PRTL_FMT_DEBUG("没有发现有效数据报文oopRecNumAll[0]\n");
    }

    if (oopRecNumAll > 0x80)
    {
        offlen = set_len_offset(oopRecNumAll, lenBuf);
        memmove(&outRecordBuf->recordBuf[offlen], &outRecordBuf->recordBuf[1], outRecordBuf->bufLen - 1);
        memcpy(&outRecordBuf->recordBuf[0], lenBuf, offlen);
        outRecordBuf->bufLen += (offlen - 1);
    }
    else
    {
        outRecordBuf->recordBuf[0] = oopRecNumAll;   //填写总记录数 
    }

    free(recordBufTmp);
    recordBufTmp = NULL;

    return ERR_OK;
}

/**
*********************************************************************
* @name：       class11_invoke_get_transparent_event_rec
* @brief：      class10的GET服务
* @param[in] ：pOdaInfo        OAD信息
               pApduInfo      一些必要信息，包括应用分帧断点信息
               inData         输入报文，不含OAD
               InDataLen      输入报文长度
               outDataMax     输出报文缓冲区最大长度
* @param[out]：outData     输出报文
* @return    ：OOP_DAR_E
* @author    :
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E class11_invoke_get_transparent_event_rec(OAD_INFO_T* pOadInfo, APDU_INFO_T* pApduInfo, const uint8* inData, 
    uint16 InDataLen, RESULT_DATA_T* outData, uint16 outDataMax)
{
    uint16 i = 0;
    uint16 j = 0;

    OOP_DAR_E  dar = DATA_OTHER_REASON;
    int32      nRet = ERR_NORMAL;
    uint16     offset = 4;
    uint16     csdStart = 0, csdLen = 0;
    uint16     outOffset = 0;
    uint16     inDataLenTmp = InDataLen;

    READ_RECORD_T inReadRecord;
    RELY_INFO_T   relyInfo;
    uint16        headLen = 0;
    RECORD_BUF_T  outRecordBuf = { 0 };

    memset(&inReadRecord, 0, sizeof(inReadRecord));
    memset(&relyInfo, 0, sizeof(relyInfo));

    outRecordBuf.recordBuf = (uint8*)malloc(pApduInfo->connectionInfo.ApplSplitLen);
    memset(outRecordBuf.recordBuf, 0xee, pApduInfo->connectionInfo.ApplSplitLen);

    //参数判断
    if (pOadInfo->pTab->oad.value != 0x6E310200)
    {
        PRTL_FMT_DEBUG("福建透明结果召测参数有误 主OAD[0x%08x]\n", pOadInfo->pTab->oad.value);
        return DATA_OBJECT_UNAVAILABLE;
    }

    //RSD和RSCD解析相关变量
    CSD_T *pCSD = NULL, *pCSDHead = NULL;
    RSD_T *pRSD = (RSD_T*)malloc(sizeof(RSD_T));
    pRSD->StartData = NULL;
    pRSD->EndData = NULL;
    MS_T *ms = NULL;

    //RSD解析相关
    memset(pRSD, 0, sizeof(RSD_T)); //清0
    offset = rsd_get(pApduInfo->hUdp, (uint8*)(inData + offset), inDataLenTmp - 4, pRSD);
    if (offset == 0)
    {
        free(outRecordBuf.recordBuf);
        outRecordBuf.recordBuf = NULL;
        if (pRSD->StartData != NULL)
        {
            free(pRSD->StartData);
        }
        if (pRSD->EndData != NULL)
        {
            free(pRSD->EndData);
        }
        if (pRSD->Ms != NULL)
        {
            ms_destroy(pRSD->Ms);
        }
        free(pRSD);
        pRSD = NULL;

        /*应对电科院协议一致性测试*/
        uint16 testOffset = 4;
        uint16 inCsdOff = 0, inCsdLen = 0;
        apdu_next_dpos_get(inData, &testOffset, InDataLen, DT_RSD, TRUE);
        inCsdOff = testOffset;
        apdu_next_dpos_get(inData, &testOffset, InDataLen, DT_RCSD, TRUE);
        inCsdLen = testOffset - inCsdOff;
        memcpy(outData->pResultData, inData, 4);
        memcpy(outData->pResultData + 4, inData + inCsdOff, inCsdLen);
        outData->resultDataLen = 4 + inCsdLen;

        outData->pResultData[outData->resultDataLen++] = 1;                   //后面优化下，outData->resultDataLen与下面统一
        pCSDHead = rcsd_get((uint8*)inData, InDataLen, &offset, FALSE);
        if (!pCSDHead || !(pCSDHead->next))
        {
            outData->pResultData[outData->resultDataLen++] = 0;
            rcsd_destroy(pCSDHead);
        }
        else
        {
            outData->pResultData[outData->resultDataLen++] = 1;
            pCSD = pCSDHead->next;
            while (pCSD)
            {
                outData->pResultData[outData->resultDataLen++] = NULL_DATA;
                pCSD = pCSD->next;
            }
            rcsd_destroy(pCSDHead);
        }

        /*空数据标记，供上报使用*/
        pApduInfo->isDateNull = 1;

        free(outRecordBuf.recordBuf);
        outRecordBuf.recordBuf = NULL;
        return DATA_SUCCESS;
    }
    else
    {
        offset += 4;             //加上原本OAD长度
        inDataLenTmp -= offset;  //除去RSD和OAD后的长度
        ms = pRSD->Ms;
    }

    if (pRSD != NULL)
    {
        if (pRSD->selId == 0)
        {
            //如果是方法0，就回所有表的上一次日冻结电能
            pRSD->selId = 54;
            pRSD->transTaskid = 1;
            pRSD->LastN = 1;
            pRSD->recNum = 1;
            pRSD->isAllmeter = 0x01;
        }
    }

    //RSCD解析相关
    csdStart = offset;
    pCSDHead = rcsd_get((uint8*)inData, InDataLen, &offset, FALSE);

    if (!pCSDHead)
    {
        PRTL_FMT_DEBUG("CSD解析错误!\n");
        ms_destroy(pRSD->Ms);
        free(pRSD->StartData);
        free(pRSD->EndData);
        free(pRSD);
        free(outRecordBuf.recordBuf);
        outRecordBuf.recordBuf = NULL;
        return DATA_TYPE_UNMATCHED;
    }

    pCSD = pCSDHead->next;      //头指针没用实际的RCSD
    if (!pCSD)
    {
        PRTL_FMT_DEBUG("头指针没用实际的RCSD, 需要虚拟0x6e320200内容\n");
    }

    /*填写OAD+RCSD+记录结果1 并记下偏移headLen*/
    outData->resultDataLen = 0;
    csdLen = offset - csdStart;
    memcpy(outData->pResultData, inData, 4);
    outOffset = 4;

    if (csdLen == 1)
    {
        //如果rcsd为空，填充OAD6e320200
        outData->pResultData[outOffset ++] = 0x01;
        outData->pResultData[outOffset ++] = 0x00;
        outData->pResultData[outOffset ++] = 0x6e;
        outData->pResultData[outOffset ++] = 0x32;
        outData->pResultData[outOffset ++] = 0x02;
        outData->pResultData[outOffset ++] = 0x00;
    }
    else
    {
        memcpy(&outData->pResultData[outOffset], &inData[csdStart], csdLen);
        outOffset += csdLen;
    }
    outData->pResultData[outOffset++] = 1;
    outData->resultDataLen += outOffset;
    headLen = outOffset;
    PRTL_BUF_LOGCC(&outData->pResultData[0], headLen, "填写OAD+RCSD+记录结果[1] 并记下偏移headLen head:");

    /*给读透明结果集口提供输入参数*/
    pCSD = pCSDHead->next;
    nRet = read_transparent_event_param_get(pRSD, pCSD, pApduInfo, &inReadRecord, &relyInfo);
    if (nRet != ERR_OK)
    {
        PRTL_FMT_DEBUG("给读透明结果集口提供输入参数失败 错误码[%d]\n", nRet);
        rcsd_destroy(pCSDHead);
        if (((pRSD->selId >= 4) && (pRSD->selId <= 8)) || ((pRSD->selId >= 51) && (pRSD->selId <= 53)))
        {
            free(pRSD->StartData);
            free(pRSD->EndData);
        }
        ms_destroy(pRSD->Ms);
        free(pRSD);
        free(outRecordBuf.recordBuf);
        outRecordBuf.recordBuf = NULL;
        return DATA_OBJECT_CLASS_INCONSISTENT;
    }

    PRTL_FMT_LOGCC("**************打印读透明结果集口提供输入参数*********************\n");
    switch (inReadRecord.cType)
    {
    case COL_TM_START: {PRTL_FMT_LOGCC("inReadRecord.cType:%s\n", "采集起始时间");     break; }
    case COL_TM_END:   {PRTL_FMT_LOGCC("inReadRecord.cType:%s\n", "采集结束时间");     break; }
    case COL_TM_STORE: {PRTL_FMT_LOGCC("inReadRecord.cType:%s\n", "采集存储时间");     break; }
    case COL_NO_STORE: {PRTL_FMT_LOGCC("inReadRecord.cType:%s\n", "数据存储序号");     break; }
    case COL_NO_INFO:  {PRTL_FMT_LOGCC("inReadRecord.cType:%s\n", "信息点号");         break; }
    case COL_NOT:      {PRTL_FMT_LOGCC("inReadRecord.cType:%s\n", "不筛选");           break; }
    default:           {PRTL_FMT_LOGCC("inReadRecord.cType:%s\n", "不正确的筛选类型");  break; }
    }
    PRTL_FMT_LOGCC("inReadRecord.cStart:%d\n", inReadRecord.cStart);
    PRTL_FMT_LOGCC("inReadRecord.cEnd:%d\n", inReadRecord.cEnd);
    PRTL_FMT_LOGCC("inReadRecord.recordOAD.oadMain = 0x%08x\n", inReadRecord.recordOAD.road.oadMain.value);
    PRTL_FMT_LOGCC("inReadRecord.recordOAD.road.oadCols.nNum = %d\n", inReadRecord.recordOAD.road.oadCols.nNum);
    for (i = 0; i < inReadRecord.recordOAD.road.oadCols.nNum; i++)
    {
        PRTL_FMT_LOGCC("inReadRecord.recordOAD.road.oad[%d] = 0x%08x\n", i, inReadRecord.recordOAD.road.oadCols.oad[i]);
    }
    PRTL_FMT_LOGCC("relyInfo.oopRcsd.nNum = %d\n", relyInfo.oopRcsd.nNum);
    for (i = 0; i < relyInfo.oopRcsd.nNum; i++)
    {
        if (relyInfo.oopRcsd.cols[i].choice == 0)
        {
            PRTL_FMT_LOGCC("choice = %d relyInfo.oopRcsd.cols[%d].oad.value = 0x%08x\n", relyInfo.oopRcsd.cols[i].choice, i,
                relyInfo.oopRcsd.cols[i].oad.value);
        }
        else if (relyInfo.oopRcsd.cols[i].choice == 1)
        {
            PRTL_FMT_LOGCC("choice = %d relyInfo.oopRcsd.cols[%d].road.oadMain.value = 0x%08x\n", relyInfo.oopRcsd.cols[i].choice, i,
                relyInfo.oopRcsd.cols[i].road.oadMain.value);
            for (j = 0; j < relyInfo.oopRcsd.cols[i].road.oadCols.nNum; j++)
            {
                PRTL_FMT_LOGCC("choice = %d relyInfo.oopRcsd.cols[%d].road.oadCols.oad[%d].value = 0x%08x\n", relyInfo.oopRcsd.cols[i].choice, i, j,
                    relyInfo.oopRcsd.cols[i].road.oadCols.oad[j].value);
            }
        }
    }
    ms = relyInfo.ms;
    while (ms)
    {
        PRTL_FMT_LOGCC("ms->MtrNo = %d\n", ms->MtrNo);
        PRTL_FMT_LOGCC("ms->next = %p\n", ms->next);
        PRTL_BUF_LOGCC(&ms->oopMeter.basic.tsa.add[0], 6, "ms->oopMeter.basic.tsa.add[0]:");
        ms = ms->next;
    }
    PRTL_FMT_LOGCC("**************打印读透明结果集口提供输入参数结束******************\n");

    /*读任务记录表接口，可以读出apdu数据域，可返回断点信息*/
    nRet = record_transparent_event_read(pApduInfo->hUdp, inReadRecord, relyInfo, 
        pApduInfo->connectionInfo.ApplSplitLen - headLen - cal_excludeLen(pApduInfo->connectionInfo), &outRecordBuf);
    if (nRet != ERR_OK)
    {
        /*清除断点信息*/
        PRTL_FMT_DEBUG("读事件记录表接口错误 错误码[%d]\n", nRet);
        memset(pApduInfo->bpBreakInfo, 0, sizeof(BP_INFO_T));
        dar = DATA_READ_WRITE_DENIED;
    }
    else
    {
        memcpy(&outData->pResultData[headLen], outRecordBuf.recordBuf, outRecordBuf.bufLen);
        outData->resultDataLen += outRecordBuf.bufLen;

        if (outRecordBuf.bufLen == 1)
        {
            /*空数据标记，供上报使用*/
            pApduInfo->isDateNull = 1;
        }

        if (outRecordBuf.breakInfo.appSlicingFlag == 1)
        {
            /*记下断点信息*/
            PRTL_FMT_LOGCC("记下断点信息\n");
            outRecordBuf.breakInfo.objPre = 0x02;   //对象属性，60120300属于记录对象
            memcpy(pApduInfo->bpBreakInfo, &outRecordBuf.breakInfo, sizeof(BP_INFO_T));
        }
        else
        {
            /*清除断点信息*/
            PRTL_FMT_LOGCC("清除断点信息\n");
            memset(pApduInfo->bpBreakInfo, 0, sizeof(BP_INFO_T));
        }

        dar = DATA_SUCCESS;
    }

    rcsd_destroy(pCSDHead);
    if (((pRSD->selId >= 4) && (pRSD->selId <= 8)) || ((pRSD->selId >= 51) && (pRSD->selId <= 53)))
    {
        free(pRSD->StartData);
        free(pRSD->EndData);
    }
    ms_destroy(pRSD->Ms);
    free(pRSD);
    free(outRecordBuf.recordBuf);
    outRecordBuf.recordBuf = NULL;

    return dar;
}
    
/**
*********************************************************************
* @name：       class11_invoke_get_acq_status
* @brief：      class10的GET服务
* @param[in] ：pOdaInfo        OAD信息
               pApduInfo      一些必要信息，包括应用分帧断点信息
               inData         输入报文，不含OAD
               InDataLen      输入报文长度
               outDataMax     输出报文缓冲区最大长度
* @param[out]：outData     输出报文
* @return    ：OOP_DAR_E
* @author    :
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E class11_invoke_get_transparent_event_res(OAD_INFO_T* pOadInfo, APDU_INFO_T* pApduInfo, const uint8* inData,
    uint16 InDataLen, RESULT_DATA_T* outData, uint16 outDataMax)
{
    if (pOadInfo->pTab->oad.value != 0x6e310200)
    {
        PRTL_FMT_DEBUG("读透明事件集合错误，主OAD选择错误 oad[0x%08x]", pOadInfo->pTab->oad.value);
        return DATA_OBJECT_UNDEFINED;
    }
        
    //如果是应用分帧状态，将apdu[1]修改成起始帧方式
    if (pApduInfo->bpBreakInfo->appSlicingFlag == 1)
    {
        pApduInfo->reqType = pApduInfo->bpBreakInfo->reqType;
    }

    switch (pApduInfo->reqType)
    {
    case 0x01:
    case 0x02:
        //return class11_invoke_get_transparent_event_nor(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break;
    case 0x03:
    case 0x04:
        return class11_invoke_get_transparent_event_rec(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break;
    default:
        break;
    }

    return DATA_OBJECT_UNDEFINED;

}

/**
*********************************************************************
* @name：       get_task_meter_num_all
* @brief：      获取任务方案下所有表数量
* @param[in] ：hUdp        数据中心句柄
、             inform      信息点
* @param[out]: none
* @return    ：配置信息链表
* @author    :
* @Date      ：2019-12-6
*********************************************************************
*/
int get_trans_event_status(DB_CLIENT hUdp, OOP_TRANS_EVENT_STATUS_T *pStatus, uint16 *pCurNum, 
    uint32 *pMaxStorageNo)
{
    int           ret = ERR_NORMAL;
    READ_RECORD_T readRecord = {0};
    RECORD_UNIT_T recordUnit = {0};
    NOMAL_OAD_T   normal = {0};
    uint32        len = 0;
    OOP_TRANS_EVENT_T data = {0};
    OOP_TRANS_EVENT_STATUS_T status = {0};
    
    uint16 confirmNum = 0;

    //读安全模式选择
    normal.oad.value = 0x6E310300;
    ret = db_read_nomal(hUdp, &normal, sizeof(OOP_TRANS_EVENT_STATUS_T), (uint8*)&status, &len);
    if ((ret != ERR_OK) || (len == 0))
    {
        PRTL_FMT_DEBUG("读普通数据出错. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
            ret, normal.logicId, normal.infoNum, normal.oad.value);
        return ERR_IO;
    }

    readRecord.recordOAD.logicId = 0;
    readRecord.recordOAD.infoNum = 0;
    readRecord.cType = COL_TM_STORE;
    readRecord.cStart = status.lastRecvNo;
    readRecord.cEnd = status.lastRecvNo + 1;
    readRecord.sortType = DOWN_SORT;
    readRecord.recordOAD.optional = 0;
    readRecord.recordOAD.road.oadCols.nNum = 1;
    readRecord.recordOAD.road.oadCols.oad[0].value = 0x6e310200;
    readRecord.MAC.nNum = 0;
    
    //福建规约中MAC表地址用于筛选数据，代替以前的信息点，0表示所有表
    readRecord.table.nNum = 0x00;
    memset(&readRecord.table.value[0], 0, 6);
    readRecord.bTable = 0x01;

    ret = db_read_record(hUdp, &readRecord, DATA_LEN_MAX, &recordUnit);
    if (ret != ERR_OK)
    {
        PRTL_FMT_DEBUG("读记录数据失败. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n", ret, readRecord.recordOAD.logicId, 
            readRecord.recordOAD.infoNum, readRecord.recordOAD.road.oadCols.oad[0].value);
        
        return ERR_IFACE;
    }

    if (sizeof(data) != recordUnit.record[0].outDatalen)
    {
        PRTL_FMT_DEBUG("数据中心的数据长度不一致. sizeof(data) = %d  outDatalen = %d\n", sizeof(data), 
            recordUnit.record[0].outDatalen);

        return ERR_IFACE;
    }

    memcpy(&data, recordUnit.record[0].outDataBuf, sizeof(data));

    ret = rpt_evtstat_read(&confirmNum);
    if (ret != 0)
    {
        PRTL_FMT_TRACE("读已接收确认报文条数错误 错误码[%d]\n", ret);
        confirmNum = 0;
    }

    PRTL_FMT_DEBUG("最近透明事件的计数器:%d\n", data.curNum);
    PRTL_FMT_DEBUG("最近透明事件存储序号:%d\n", data.selfStoreNo);
    PRTL_FMT_DEBUG("读已接收确认报文条数:%d\n", confirmNum);
    PRTL_FMT_DEBUG("整体透明事件存储序号:%d\n", status.lastRecvNo);

    DT_Time2DateTimeShort(recordUnit.colStartTm, &status.lastRecvTime);
    #if 0
    status.lastRecvNo = data.selfStoreNo;
    #endif
    
    status.recvSum = confirmNum;

    if (pStatus != NULL)
    {
        *pStatus = status;
    }

    if (pCurNum != NULL)
    {
        *pCurNum = data.curNum;
    }

    if (pMaxStorageNo != NULL)
    {
        *pMaxStorageNo = status.lastRecvNo;
    }

    return ERR_OK;
}

/**
*********************************************************************
* @name：      class11_invoke_get_transparent_event_status
* @brief：     class8的SET服务
* @param[in] ：pOdaInfo        OAD信息
               pBpInfo        应用分帧断点信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
               outDataMax     输出报文缓冲区最大长度

* @param[out]：outData     输出报文
* @return    ：OOP_DAR_E
* @author    :
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E class11_invoke_get_transparent_event_status(OAD_INFO_T* pOadInfo, APDU_INFO_T* pApduInfo, const uint8* inData, uint16 InDataLen,
    RESULT_DATA_T* outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    DATA_CONVERT_T convert;
    uint32 sOffset = 0;
    uint32 offset = 0;

    MEMZERO(&convert, sizeof(convert));

    //读出数据
    switch (pOadInfo->pTab->oad.value)
    {
    case 0x6e310300:
    {
        OOP_TRANS_EVENT_STATUS_T dataGet = {0};
        
        ret = get_trans_event_status(pApduInfo->hUdp, &dataGet, NULL, NULL);
        if(ret !=ERR_OK)
        {
            memcpy(outData->pResultData, inData, sizeof(OOP_OAD_U));
            offset += sizeof(OOP_OAD_U);
            outData->pResultData[offset++] = 0x01;
            outData->pResultData[offset++] = 0x00;
            outData->resultDataLen += offset;
            return DATA_SUCCESS;
        }

        //转换成报文数据
        convert.srcBuf = &dataGet;
        convert.srcLen = sizeof(dataGet);
        convert.sOffset = &sOffset;

        convert.dstBuf = &outData->pResultData[5];
        convert.dstLen = outDataMax;
        convert.dOffset = &outData->resultDataLen;
    }
    break;

    default:
        break;
    }

            /* 数据和报文转换 */
    dar = data_to_buf_event_status(pOadInfo, &convert);
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("data_to_buf_event_status failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }


    /* 填写报文开头处的OAD和数据结果0x01*/
    memcpy(outData->pResultData, inData, sizeof(OOP_OAD_U));
    offset += sizeof(OOP_OAD_U);
    outData->pResultData[offset++] = 0x01;
    outData->resultDataLen += offset;

    return dar;
}

#endif

#if DESC("操作透明事件内容", 1)
/**
*********************************************************************
* @name：      class11_invoke_clr_transparent_event
* @brief：     class11的ACTION服务
* @param[in] ：pOdaInfo        OAD信息
               pBpInfo        应用分帧断点信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
               outDataMax     输出报文缓冲区最大长度

* @param[out]：outData     输出报文
* @return    ：OOP_DAR_E
* @author    :
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E class11_invoke_clr_transparent_event(OAD_INFO_T* pOadInfo, APDU_INFO_T* pApduInfo, const uint8* inData, 
    uint16 inLen, RESULT_DATA_T* outData, uint16 outDataMax)
{
    int    ret = ERR_OK;

    CLEAR_DATA_T RecordDel;
    OOP_DAR_E   dar = DATA_OTHER_REASON;

    memset(&RecordDel, 0, sizeof(RecordDel));

    /*类型不存在*/
    if ((pOadInfo == NULL) || (pApduInfo == NULL) || (inData == NULL) || (outData == NULL))
    {
        return DATA_READ_WRITE_DENIED;
    }

    switch (pOadInfo->pTab->oad.value)
    {
    case 0x6e317f00:
    {
        PRTL_FMT_DEBUG("清空事件记录\n");

        RecordDel.recordOAD.logicId = 0;
        RecordDel.recordOAD.infoNum = 0xffff;
        RecordDel.recordOAD.optional = 0;
        RecordDel.recordOAD.road.oadMain.value = 0;
        RecordDel.recordOAD.road.oadCols.nNum = 1;
        RecordDel.recordOAD.road.oadCols.oad[0].value = 0x6e310200;
        RecordDel.bClassTag = FALSE;

        ret = db_clear_oad_data(pApduInfo->hUdp, &RecordDel);
        if (ret != ERR_OK)
        {
            PRTL_FMT_LOGCC("清空事件记录失败 ret[%d]\n", ret);
        }
       
        RecordDel.recordOAD.road.oadCols.oad[0].value = 0x6e310300;

        ret = db_clear_oad_data(pApduInfo->hUdp, &RecordDel);
        if (ret != ERR_OK)
        {
            PRTL_FMT_LOGCC("清空事件当前状态失败 ret[%d]\n", ret);
        }
        memset(&gTransEvtStatus, 0, sizeof(OOP_TRANS_EVENT_STATUS_T));
        rpt_evtstat_clear();
        memcpy_r((uint8*)outData->pResultData, (uint8*)&pOadInfo->pTab->oad.value, 4);
        if (ret == ERR_OK)
        {
            dar = DATA_SUCCESS;
            outData->pResultData[4] = DATA_SUCCESS;
            outData->pResultData[5] = 0x00;                          //DATA:NULL
            outData->resultDataLen = 0x06;
        }
        else
        {
            dar = DATA_READ_WRITE_DENIED;
        }
    }
    break;
    default:
        break;
    }

    return dar;
}

/**
*********************************************************************
* @name：      class11_invoke_tri_transparent_report
* @brief：     class11的ACTION服务
* @param[in] ：pOdaInfo        OAD信息
               pBpInfo        应用分帧断点信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
               outDataMax     输出报文缓冲区最大长度

* @param[out]：outData     输出报文
* @return    ：OOP_DAR_E
* @author    :
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E class11_invoke_tri_transparent_event(OAD_INFO_T* pOadInfo, APDU_INFO_T* pApduInfo, const uint8* inData, 
    uint16 inLen, RESULT_DATA_T* outData, uint16 outDataMax)
{
    OOP_DAR_E   dar = DATA_OTHER_REASON;

    /*类型不存在*/
    if ((pOadInfo == NULL) || (pApduInfo == NULL) || (inData == NULL) || (outData == NULL))
    {
        return DATA_READ_WRITE_DENIED;
    }

    switch (pOadInfo->pTab->oad.value)
    {
    case 0x6e318000:
    {
        PRTL_FMT_DEBUG("开始补报事件\n");

        set_event_rpt_trigger();

        memcpy_r((uint8*)outData->pResultData, (uint8*)&pOadInfo->pTab->oad.value, 4);

        outData->pResultData[4] = DATA_SUCCESS;
        outData->pResultData[5] = 0x00;
        outData->resultDataLen = 0x06;

        dar = DATA_SUCCESS;
    }
    break;
    default:
        break;
    }

    return dar;
}

#endif

#if DESC("上报方案操作", 1)
/**
*********************************************************************
* @name：      class11_invoke_add_transparent_report
* @brief：     class11的ACTION服务
* @param[in] ：pOdaInfo        OAD信息
               pBpInfo        应用分帧断点信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
               outDataMax     输出报文缓冲区最大长度

* @param[out]：outData     输出报文
* @return    ：OOP_DAR_E
* @author    :
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E class11_invoke_add_transparent_report(OAD_INFO_T* pOadInfo, APDU_INFO_T* pApduInfo, const uint8* inData, 
    uint16 inLen, RESULT_DATA_T* outData, uint16 outDataMax)
{
    uint16 i = 0;
    int    ret = ERR_OK;
    uint32 sOffset = 4;
    uint32 dOffset = 0;
    uint8  num = 0;

    NOMAL_OAD_T nomalOad = { 0 };
    OOP_DAR_E   dar = DATA_OTHER_REASON;

    OOP_TRANS_REPORT_T data;
    DATA_CONVERT_T     convert;

    memset(&data, 0, sizeof(data));
    memset(&convert, 0, sizeof(convert));

    /*类型不存在*/
    if ((pOadInfo == NULL) || (pApduInfo == NULL) || (inData == NULL) || (outData == NULL))
    {
        return DATA_READ_WRITE_DENIED;
    }

    convert.srcBuf = (void*)inData;
    convert.srcLen = inLen;
    convert.sOffset = &sOffset;

    convert.dstBuf = &data;
    convert.dstLen = sizeof(data);
    convert.dOffset = &dOffset;

    switch (pOadInfo->pTab->oad.value)
    {
    case 0x6e177F00:
    {
        if (inData[sOffset++] != DT_ARRAY)
        {
            return ERR_OPTION;
        }

        num = inData[sOffset++];

        PRTL_FMT_DEBUG("增加上报任务，数量为%d\n", num);
        for (i = 0; i < num; i++)
        {
            dar = buf_to_data_report_plan_fujian(pOadInfo, &convert);
            if (dar != DATA_SUCCESS)
            {
                return dar;
            }
            else
            {
                dOffset = 0;
            }

            PRTL_FMT_DEBUG("需要增加上报方案号:%d\n", data.reportid);

            nomalOad.logicId = 0x00;
            nomalOad.infoNum = data.reportid;
            nomalOad.oad.value = 0x6e170200;
            nomalOad.classtag = CLASS_DATA_INIT;

            ret = db_write_nomal(pApduInfo->hUdp, &nomalOad, (uint8*)&data, sizeof(data));
            if (ret != ERR_OK)
            {
                PRTL_FMT_DEBUG("写入数据中心失败. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                    ret, nomalOad.logicId, nomalOad.infoNum, nomalOad.oad.value);
                return DATA_READ_WRITE_DENIED;
            }
        }

        memcpy_r((uint8*)outData->pResultData, (uint8*)&pOadInfo->pTab->oad.value, 4);
        if (ret == ERR_OK)
        {
            dar = DATA_SUCCESS;
            outData->pResultData[4] = DATA_SUCCESS;
            outData->pResultData[5] = 0x00;                          //DATA:NULL
            outData->resultDataLen = 0x06;
        }
        else
        {
            dar = DATA_READ_WRITE_DENIED;
        }
    }
    break;
    default:
        break;
    }

    return dar;
}

/**
*********************************************************************
* @name：      class11_invoke_del_transparent_report
* @brief：     class11的ACTION服务
* @param[in] ：pOdaInfo        OAD信息
               pBpInfo        应用分帧断点信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
               outDataMax     输出报文缓冲区最大长度

* @param[out]：outData     输出报文
* @return    ：OOP_DAR_E
* @author    :
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E class11_invoke_del_transparent_report(OAD_INFO_T* pOadInfo, APDU_INFO_T* pApduInfo, const uint8* inData, 
    uint16 inLen, RESULT_DATA_T* outData, uint16 outDataMax)
{
    uint16 i = 0;
    int    ret = ERR_OK;
    uint8  num = 0;
    uint8  reportId = 0;;

    CLEAR_DATA_T RecordDel;
    OOP_DAR_E    dar = DATA_OTHER_REASON;

    uint8       *srcBuf = (uint8*)inData;
    uint32       srcLen  = inLen;
    uint32       sOffset = 4;

    memset(&RecordDel, 0, sizeof(RecordDel));

    /*类型不存在*/
    if ((pOadInfo == NULL) || (pApduInfo == NULL) || (inData == NULL) || (outData == NULL))
    {
        return DATA_READ_WRITE_DENIED;
    }

    switch (pOadInfo->pTab->oad.value)
    {
    case 0x6e178000:
    {
        if (inData[sOffset++] != DT_ARRAY)
        {
            return ERR_OPTION;
        }

        num = inData[sOffset++];
        if (num == 0x81)
        {
            num = inData[sOffset++];
        }
        else if (num == 0x82)
        {
            num = inData[sOffset++];
            num <<= 8;
            num += inData[sOffset++];
        }

        PRTL_FMT_DEBUG("删除上报方案，数量为%d\n", num);
        for (i = 0; i < num; i++)
        {
            
            dar = basic_buf_to_data(E_UNSIGNED, srcBuf, srcLen, &sOffset, &reportId, sizeof(reportId));
            if (dar != DATA_SUCCESS)
            {
                return dar;
            }
            //直接删除的时候可以把2删掉
            //if(reportId != 2)
            //{            
                PRTL_FMT_DEBUG("需要删除的上报方案号:%d\n", reportId);

                RecordDel.recordOAD.logicId = 0;
                RecordDel.recordOAD.infoNum = reportId;
                RecordDel.recordOAD.optional = 0;
                RecordDel.recordOAD.road.oadMain.value = 0;
                RecordDel.recordOAD.road.oadCols.nNum = 1;
                RecordDel.recordOAD.road.oadCols.oad[0].value = 0x6e170200;
                RecordDel.bClassTag = FALSE;

                PRTL_FMT_DEBUG("要删除任务方案存入数据中心的信息点为%d\n", RecordDel.recordOAD.infoNum);

                ret = db_clear_oad_data(pApduInfo->hUdp, &RecordDel);
                if (ret != ERR_OK)
                {
                    PRTL_FMT_DEBUG("数据删除失败 ret[%d]\n", ret);
                }
//            }
//            else
//            {
//                PRTL_FMT_DEBUG("上报方案%d不予删除\n", reportId);
//            }
        }

        memcpy_r((uint8*)outData->pResultData, (uint8*)&pOadInfo->pTab->oad.value, 4);
        if (ret == ERR_OK)
        {
            dar = DATA_SUCCESS;
            outData->pResultData[4] = DATA_SUCCESS;
            outData->pResultData[5] = 0x00;                          //DATA:NULL
            outData->resultDataLen = 0x06;
        }
        else
        {
            dar = DATA_READ_WRITE_DENIED;
        }
    }
    break;
    default:
        break;
    }

    return dar;
}

/**
*********************************************************************
* @name：      class11_invoke_clr_transparent_report
* @brief：     class11的ACTION服务
* @param[in] ：pOdaInfo        OAD信息
               pBpInfo        应用分帧断点信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
               outDataMax     输出报文缓冲区最大长度

* @param[out]：outData     输出报文
* @return    ：OOP_DAR_E
* @author    :
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E class11_invoke_clr_transparent_report(OAD_INFO_T* pOadInfo, APDU_INFO_T* pApduInfo, const uint8* inData, 
    uint16 inLen, RESULT_DATA_T* outData, uint16 outDataMax)
{
    int    ret = ERR_OK;
    uint16 i = 0;
    CLEAR_DATA_T RecordDel;
    OOP_DAR_E   dar = DATA_OTHER_REASON;
    OOP_PLAN_REPORT_LIST_FJ_T tmpPlanList = {0};

    memset(&RecordDel, 0, sizeof(RecordDel));
    memcpy(&tmpPlanList, &oopPlanReportList_fj, sizeof(OOP_PLAN_REPORT_LIST_FJ_T));
    
    /*类型不存在*/
    if ((pOadInfo == NULL) || (pApduInfo == NULL) || (inData == NULL) || (outData == NULL))
    {
        return DATA_READ_WRITE_DENIED;
    }

    switch (pOadInfo->pTab->oad.value)
    {
    case 0x6e178100:
    {
        PRTL_FMT_DEBUG("清空上报方案,其中方案2不予清空\n");

        for(i = 0; i < tmpPlanList.oopPlanReportNum; i++)
        {
            if(tmpPlanList.oopPlanReport[i].reportid == 2)
            {
                continue;
            }
            
            RecordDel.recordOAD.logicId = 0;
            RecordDel.recordOAD.infoNum = tmpPlanList.oopPlanReport[i].reportid;
            RecordDel.recordOAD.optional = 0;
            RecordDel.recordOAD.road.oadMain.value = 0;
            RecordDel.recordOAD.road.oadCols.nNum = 1;
            RecordDel.recordOAD.road.oadCols.oad[0].value = 0x6e170200;
            RecordDel.bClassTag = FALSE;

            ret = db_clear_oad_data(pApduInfo->hUdp, &RecordDel);
            if (ret != ERR_OK)
            {
                PRTL_FMT_LOGCC("清空上报方案失败 ret[%d]\n", ret);
            }
        }
        
        memcpy_r((uint8*)outData->pResultData, (uint8*)&pOadInfo->pTab->oad.value, 4);
        if (ret == ERR_OK)
        {
            dar = DATA_SUCCESS;
            outData->pResultData[4] = DATA_SUCCESS;
            outData->pResultData[5] = 0x00;                          //DATA:NULL
            outData->resultDataLen = 0x06;
        }
        else
        {
            dar = DATA_READ_WRITE_DENIED;
        }
    }
    break;
    default:
        break;
    }

    return dar;
}

/**
*********************************************************************
* @name：      class11_invoke_tri_transparent_report
* @brief：     class11的ACTION服务
* @param[in] ：pOdaInfo        OAD信息
               pBpInfo        应用分帧断点信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
               outDataMax     输出报文缓冲区最大长度

* @param[out]：outData     输出报文
* @return    ：OOP_DAR_E
* @author    :
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E class11_invoke_tri_transparent_report(OAD_INFO_T* pOadInfo, APDU_INFO_T* pApduInfo, const uint8* inData, 
    uint16 inLen, RESULT_DATA_T* outData, uint16 outDataMax)
{
    uint8  num = 0;
    uint32 i = 0;

    OOP_DAR_E   dar = DATA_OTHER_REASON;

    uint8      *srcBuf  = (uint8*)inData;
    uint32      srcLen  = inLen;
    uint32      sOffset = 4;

    OOP_RPT_INF_T rptInf = {0};
    uint16        taskId = 0;

    memset(&rptInf, 0, sizeof(rptInf));

    /*类型不存在*/
    if ((pOadInfo == NULL) || (pApduInfo == NULL) || (inData == NULL) || (outData == NULL))
    {
        return DATA_READ_WRITE_DENIED;
    }

    switch (pOadInfo->pTab->oad.value)
    {
    case 0x6e178400:
    {
        if (inData[sOffset++] != DT_ARRAY)
        {
            return ERR_OPTION;
        }

        num = inData[sOffset++];
        if (num == 0x81)
        {
            num = inData[sOffset++];
        }
        else if (num == 0x82)
        {
            num = inData[sOffset++];
            num <<= 8;
            num += inData[sOffset++];
        }

        rptInf.num = num;
        for (i = 0; i < num; i++)
        {
            
            dar = basic_buf_to_data(E_LONG_UNSIGNED, srcBuf, srcLen, &sOffset, &taskId, sizeof(taskId));
            if (dar != DATA_SUCCESS)
            {
                return dar;
            }
            rptInf.taskId[i] = taskId;
        }
        
        PRTL_FMT_DEBUG("开始补报 补报数量[%d]\n", rptInf.num);

        set_report_trigger(&rptInf);

        memcpy_r((uint8*)outData->pResultData, (uint8*)&pOadInfo->pTab->oad.value, 4);

        dar = DATA_SUCCESS;
        outData->pResultData[4] = DATA_SUCCESS;
        outData->pResultData[5] = 0x00;
        outData->resultDataLen = 0x06;
    }
    break;
    default:
        break;
    }

    return dar;
}


#endif

#if DESC("透明方案采集状态集合", 1)
/**
*********************************************************************
* @name：       get_meter_num_from_task
* @brief：      获取表数量
* @param[in] ：hUdp        数据中心句柄
               taskId.....任务方案，FFFF表示所有任务方案
               isDeGroup  是否分解群组档案
* @param[out]: none
* @return    ：获取具体任务数量
* @author    :
* @Date      ：2019-12-6
*********************************************************************
*/
uint16 get_meter_num_from_task(uint16 taskId, DB_CLIENT hUdp, BOOL isDeGroup)
{
    int                 i = 0;
    int                 ret = ERR_OK;
    uint32              handle = 0;
    uint32              recNum = 0;
    uint32              num = 0;
    
    OOP_TRANS_TASK_NODE_T oopNode;
    READ_RECORD_T         inReadRecord;
    STEP_READ_RECORD_T    stepReadRecord;
    RECORD_UNIT_T         stepRecordOut;

    memset(&oopNode, 0, sizeof(oopNode));
    memset(&stepReadRecord, 0, sizeof(stepReadRecord));
    memset(&stepRecordOut, 0, sizeof(stepRecordOut));
    memset(&inReadRecord, 0, sizeof(inReadRecord));

    //获取所有任务方案
    inReadRecord.recordOAD.infoNum = taskId;
    inReadRecord.cType = COL_NOT;
    inReadRecord.cStart = 0;
    inReadRecord.cEnd = 0xffff;
    inReadRecord.sortType = UP_SORT;
    inReadRecord.recordOAD.optional = 0;
    inReadRecord.recordOAD.road.oadMain.value = 0x6e130200;
    inReadRecord.recordOAD.road.oadCols.nNum = 0;
    inReadRecord.bTable = 1;
    inReadRecord.table.nNum = 0;

    ret = db_read_record_start(hUdp, &inReadRecord, &handle, &recNum);
    if ((ret != ERR_OK) || (recNum == 0))
    {
        PRTL_FMT_DEBUG("获取具体任务数量失败 ret = %d, infoNum = %d recNum = %d\n",
            ret, inReadRecord.recordOAD.infoNum, recNum);

        db_read_record_end(hUdp, handle);
        return 0;
    }

    for (i = 0; i < recNum; i++)
    {
        ret = db_read_record_step(hUdp, handle, &stepReadRecord, &stepRecordOut);
        if (ret != ERR_OK)
        {
            PRTL_FMT_DEBUG("具体任务单步读成功 db_read_record_step err = %d!\n", ret);
            continue;
        }

        memcpy(&oopNode, stepRecordOut.record[0].outDataBuf, stepRecordOut.record[0].outDatalen);
        
        num += get_meter_num_from_node(hUdp, &oopNode, isDeGroup);
    }

    db_read_record_end(hUdp, handle);

    return num;
}

/**
*********************************************************************
* @name：       get_node_num
* @brief：      获取具体任务数量
* @param[in] ：hUdp        数据中心句柄
               taskId.....任务方案，FFFF表示所有任务方案
* @param[out]: none
* @return    ：获取具体任务数量
* @author    :
* @Date      ：2019-12-6
*********************************************************************
*/
uint16 get_task_node_num(uint16 taskId, DB_CLIENT hUdp)
{
    int                 ret = ERR_OK;
    uint32              handle = 0;
    uint32              recNum = 0;

    READ_RECORD_T         inReadRecord;
    STEP_READ_RECORD_T    stepReadRecord;
    RECORD_UNIT_T         stepRecordOut;

    memset(&stepReadRecord, 0, sizeof(stepReadRecord));
    memset(&stepRecordOut, 0, sizeof(stepRecordOut));
    memset(&inReadRecord, 0, sizeof(inReadRecord));

    //获取所有任务方案
    inReadRecord.recordOAD.infoNum = taskId;
    inReadRecord.cType = COL_NOT;
    inReadRecord.cStart = 0;
    inReadRecord.cEnd = 0xffff;
    inReadRecord.sortType = UP_SORT;
    inReadRecord.recordOAD.optional = 0;
    inReadRecord.recordOAD.road.oadMain.value = 0x6e130200;
    inReadRecord.recordOAD.road.oadCols.nNum = 0;
    inReadRecord.bTable = 1;
    inReadRecord.table.nNum = 0;

    ret = db_read_record_start(hUdp, &inReadRecord, &handle, &recNum);
    if ((ret != ERR_OK) || (recNum == 0))
    {
        PRTL_FMT_DEBUG("获取具体任务数量失败 ret = %d, infoNum = %d recNum = %d\n",
            ret, inReadRecord.recordOAD.infoNum, recNum);

        db_read_record_end(hUdp, handle);
        return 0;
    }

    db_read_record_end(hUdp, handle);

    return recNum;
}

/**
*********************************************************************
* @brief：      组织虚拟数据(模仿数据中心接口 开始读)
* @param[in]： taskId           方案号
               data            方案内容
* @param[out]：handle           虚拟数据内容句柄
               recNum          数量
* @return：     =0              成功
               <0              错误码
*********************************************************************
*/
int db_read_start_virtualAcq(DB_CLIENT              hUdp, READ_INF_T *inReadInf, uint16 taskId, OOP_TRANS_TASK_T *task, 
    uint32 *handle, uint32 *recNum)
{
    OOP_TRANS_TASK_RLUESTATE_T data;
    OOP_TRANS_TASK_RLUESTATE_T *p = NULL;
    uint32                     num = 0;
    
    if ((inReadInf == NULL) || (handle == NULL) )
    {
        return ERR_PTR;
    }

    memset(&data, 0, sizeof(data));

    //广播任务，回空数据即可
    if (task->taskTypeData.taskType == 1)
    {
        PRTL_FMT_DEBUG("任务类别[%d] 广播任务 回空数据即可\n", task->taskTypeData.taskType);
        return ERR_NOCONFIG;
    }

    p = (OOP_TRANS_TASK_RLUESTATE_T *)malloc(sizeof(OOP_TRANS_TASK_RLUESTATE_T));
    num = get_meter_num_from_task(taskId, hUdp, TRUE);

    PRTL_FMT_DEBUG("任务方案没有在对应的时间执行过 方案号[%d] 任务配置总数[%d]\n", taskId, num);
    
    data.transTaskid = taskId;
    data.storeTime = inReadInf->startTime;
    data.firststartTime.year = 2099;
    data.firststartTime.month = 12;
    data.firststartTime.day = 31;
    data.firststartTime.hour = 23;
    data.firststartTime.minute = 59;
    data.firststartTime.second = 59;
    data.laststartTime.year = 2099;
    data.laststartTime.month = 12;
    data.laststartTime.day = 31;
    data.laststartTime.hour = 23;
    data.laststartTime.minute = 59;
    data.laststartTime.second = 59;
    data.paramSum = num;
    data.storenum = 0;

    p[0] = data;

    *handle = (uint32)p;
    *recNum = 1;

    return ERR_OK;
}

/**
*********************************************************************
* @brief：      组织虚拟数据(模仿数据中心接口 单步读)
* @param[in]： taskId           方案号
               data            方案内容
               handle          虚拟数据内容句柄
               index           索引
* @param[out]：stepRecordOut    单步读结果
* @return：     =0              成功
               <0              错误码
*********************************************************************
*/
int db_read_step_virtualAcq(DB_CLIENT              hUdp, uint32 handle, uint32 index, RECORD_UNIT_T *stepRecordOut)
{
    if ((stepRecordOut == NULL) || (handle == 0))
    {
        return ERR_PTR;
    }
        
    OOP_TRANS_TASK_RLUESTATE_T *p = (OOP_TRANS_TASK_RLUESTATE_T *)handle;
    OOP_TRANS_TASK_RLUESTATE_T data = p[index];

    stepRecordOut->infoNum = data.transTaskid;
    stepRecordOut->recordnum = 1;
    stepRecordOut->record[0].road.oadCols.oad[0].value = 0x6e200200;
    stepRecordOut->record[0].outDatalen = sizeof(data);
    memcpy(stepRecordOut->record[0].outDataBuf, &data, sizeof(data));

    return ERR_OK;
}

/**
*********************************************************************
* @brief：      组织虚拟数据(模仿数据中心接口 结束读)
* @param[in]： hUdp          -  数据中心句柄
* @param[out]：handle           虚拟数据内容句柄
* @return：     =0              成功
               <0              错误码
*********************************************************************
*/
int db_read_end_virtualAcq(DB_CLIENT hUdp, uint32 *handle)
{
    uint32 h  = *handle;
    
    if (*handle == 0)
        return ERR_PTR;
    
    free((void *)h);

    *handle = 0;
    
    return ERR_OK;
}

/**
*********************************************************************
* @name：       get_task_store_num
* @brief：      获取任务方案存储深度
* @param[in] ：hUdp         数据中心句柄
               inform      信息点
* @param[out]: none
* @return    ：配置信息链表
* @author    :
* @Date      ：2019-12-6
*********************************************************************
*/
uint16 get_task_store_num(DB_CLIENT hUdp, uint16 infoNum)
{
    OOP_TRANS_TASK_T task;
    
    NOMAL_OAD_T normal = {0};
    uint32      len  = 0;
    int         ret = ERR_OK;
    
    normal.oad.value = 0x6e110200;
    normal.infoNum = infoNum;
    ret = db_read_nomal(hUdp, &normal, sizeof(task), (uint8 *)&task, &len);
    if ((ret != ERR_OK) || (len != sizeof(task)))
    {
        PRTL_FMT_DEBUG("读任务方案失败. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x) len(%d) sizeof(task) = %d\n",
            ret, normal.logicId, normal.infoNum, normal.oad.value, len, sizeof(task));
        return 0;
    }

    //存储深度为0时，也要召测到一条采集状态
    if (task.storeNum == 0)
    {
        return 1;
    }

    return task.storeNum;
}

/**
*********************************************************************
* @brief：      该记是否是需要的
* @param[in]： 
* @return：    =0            - 该记录是所需
               <0            - 该记录不是所需
*********************************************************************
*/
int record_trans_acq_is_valid(uint32 storageTime, uint32 startTime, uint32 endTime, OOP_TI_T ti)
{
    uint32    div = 0;
    int32     ret = 0;
    
    if ((storageTime < startTime) || (storageTime >= endTime))
    {
        PRTL_FMT_TRACE("时间不符合要求 startTime[%d] storageTime[%d] endTime[%d]\n", startTime, storageTime, endTime);
        return ERR_O_RANGE;
    }

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

    //透明任务采集状态不需要判断存储周期
    /*if (storageTime % div == 0)
    {
        ret = ERR_OK;
    }
    else
    {
        ret = ERR_OPTION;
    }*/
    
    return ret;
}

/**
*********************************************************************
* @brief：      读记录型方案任务
* @param[in]： inReadRecord  - 筛选需要的数据
               oopRcsd       - 列选择
               readRecNum    - 读记录数量，：0：读全部记录 1：读最近的一条记录
               maxBufLen     - 最大读取buf长度
* @param[out]：outRecordBuf  -  数据记录
* @return：     =0            - 成功
               <0            - 错误码
*********************************************************************
*/
int record_trans_acq_read_bak(DB_CLIENT hUdp, READ_INF_T inReadInf, RELY_INFO_T relyInfo, uint32 maxBufLen, RECORD_BUF_T *outRecordBuf)
{
    uint32    offset = 0;
    uint32    i = 0;
    uint32    j = 0;
    int32     ret = ERR_NORMAL;
    OOP_DAR_E nRet = DATA_OTHER_REASON;
    uint32    bufAllLen = 0;
    BP_INFO_T breakInfo = {0};                                         //断点信息
    BOOL      isBufFull = FALSE;
    OAD_INFO_T *pOadInfo = relyInfo.pOadInfo;

    /*清除断点信息, 需要注意应用分帧的未帧*/
    memset(&breakInfo, 0, sizeof(BP_INFO_T));
   
    DATA_CONVERT_T             convert;
    OOP_TRANS_TASK_RLUESTATE_T data = {0};
    uint32 sOffset = 0;
    uint32 dOffset = 0;
    uint32 acqNum = 0;
    uint32 acqNumOffset = 0;

    READ_RECORD_T      inReadRecord;
    STEP_READ_RECORD_T stepReadRecord;
    RECORD_UNIT_T      stepRecordOut;
    uint32             handle = 0;
    uint32             recNum = 0;

    uint32 startTime = 0;
    uint32 endTime = 0;
    uint32 storageTime = 0;
    OOP_TI_T ti = inReadInf.ti;

    MEMZERO(&convert, sizeof(convert));
    MEMZERO(&inReadRecord, sizeof(inReadRecord));

    acqNumOffset = offset;                                             //数量偏移
    outRecordBuf->recordBuf[offset ++] = 0x00;                          //数量，先填0
    bufAllLen = offset;

    ret = DT_DateTimeShort2Time(&inReadInf.startTime, (time_t *)&startTime);
    if (!ret)
    {
        PRTL_FMT_DEBUG("时间筛选参数转换出错! date: %d/%d/%d time: %d-%d-%d\n", inReadInf.startTime.year, inReadInf.startTime.month, 
            inReadInf.startTime.day, inReadInf.startTime.hour, inReadInf.startTime.minute, inReadInf.startTime.second);
        return ERR_PTR;
    }

    ret = DT_DateTimeShort2Time(&inReadInf.endTime, (time_t *)&endTime);
    if (!ret)
    {
        PRTL_FMT_DEBUG("时间筛选参数转换出错! date: %d/%d/%d time: %d-%d-%d\n", inReadInf.endTime.year, inReadInf.endTime.month, 
            inReadInf.endTime.day, inReadInf.endTime.hour, inReadInf.endTime.minute, inReadInf.endTime.second);
        return ERR_PTR;
    }

    inReadRecord.recordOAD.logicId = 0;
    inReadRecord.recordOAD.infoNum = 0xffff;
    inReadRecord.cType = COL_NO_INFO;
    inReadRecord.cStart = 0;
    inReadRecord.cEnd = 0xffff;
    inReadRecord.sortType = UP_SORT;
    inReadRecord.recordOAD.road.oadMain.value = 0;
    inReadRecord.recordOAD.road.oadCols.nNum = 1;
    inReadRecord.recordOAD.road.oadCols.oad[0].value = 0x6e200200;

    if (inReadInf.startTask != 0 || inReadInf.endTask != 0)
    {
        inReadRecord.cStart = inReadInf.startTask;
        inReadRecord.cEnd = inReadInf.endTask;
    }
    
    if(relyInfo.bpBreakInfo->appSlicingFlag == 1)
    {
        inReadRecord.cStart = relyInfo.bpBreakInfo->index;
    }

    stepReadRecord.MAC.nNum = 0;

    ret = db_read_record_start(hUdp, &inReadRecord, &handle, &recNum);
    if ((ret != ERR_OK) || (recNum == 0))
    {
        PRTL_FMT_LOGCC("福建规约，开始读记录错误 ret[%d] infoNum[%d] recNum[%d] start[%d] end[%d]\n",
            ret, inReadRecord.recordOAD.infoNum, recNum, inReadRecord.cStart, inReadRecord.cEnd);

        db_read_record_end(hUdp, handle);
        return ret;
    }
    else
    {
        PRTL_FMT_LOGCC("福建规约，开始读记录成功 ret[%d] infoNum[%d] recNum[%d] start[%d] end[%d]\n",
            ret, inReadRecord.recordOAD.infoNum, recNum, inReadRecord.cStart, inReadRecord.cEnd);
    }

    for (j = 0; j < recNum; j++)
    {
        ret = db_read_record_step(hUdp, handle, &stepReadRecord, &stepRecordOut);
        if(ret != ERR_OK)
        {
            PRTL_FMT_DEBUG("单步读失败 错误码%d!\n", ret);
            continue;
        }

        //OAD与返回数据
        if ((stepRecordOut.recordnum != 1) 
        || (stepRecordOut.record[0].road.oadCols.oad[0].value != pOadInfo->pTab->oad.value)
        || (stepRecordOut.record[0].outDatalen != sizeof(data)))
        {
            PRTL_FMT_DEBUG("单步读到的数据错误!\n");
            PRTL_FMT_DEBUG("单步读数据 stepRecordOut.recordnum = %d\n", stepRecordOut.recordnum);
            PRTL_FMT_DEBUG("单步读数据 stepRecordOut.record[0].road.oadCols.oad[0] = 0x%08x\n", 
                stepRecordOut.record[0].road.oadCols.oad[0].value);
            PRTL_FMT_DEBUG("单步读数据 stepRecordOut.record[0].outDatalen = %d\n", 
                stepRecordOut.record[0].outDatalen);
            PRTL_FMT_DEBUG("单步读数据 sizeof(OOP_TRANS_TASK_RLUESTATE_T) = %d\n", 
                sizeof(data));
            continue;
        }

        memcpy(&data, stepRecordOut.record[0].outDataBuf, stepRecordOut.record[0].outDatalen);
        ret = DT_DateTimeShort2Time(&data.storeTime, (time_t *)&storageTime);
        if (!ret)
        {
            PRTL_FMT_DEBUG("时间筛选参数转换出错[%d]! date: %d/%d/%d time: %d-%d-%d\n", ret, data.storeTime.year, data.storeTime.month, 
                data.storeTime.day, data.storeTime.hour, data.storeTime.minute, data.storeTime.second);
            continue;
        }

        //不符合筛选条件
        ret = record_trans_acq_is_valid(storageTime, startTime, endTime, ti);
        if (ret != ERR_OK)
        {
            PRTL_FMT_TRACE("时间筛选不符合筛选条件[%d]! date: %d/%d/%d time: %d-%d-%d\n", ret, data.storeTime.year, data.storeTime.month, 
                data.storeTime.day, data.storeTime.hour, data.storeTime.minute, data.storeTime.second);
            continue;
        }
        
        PRTL_FMT_LOGCC("找到采集状态序号为%d\n", data.transTaskid);

        //转换成报文数据
        convert.srcBuf = &data;
        convert.srcLen = sizeof(data);
        convert.sOffset = &sOffset;

        convert.dstBuf = &outRecordBuf->recordBuf[offset];
        convert.dstLen = maxBufLen - offset;
        convert.dOffset = &dOffset;

        //记录断点信息
        breakInfo.index = data.transTaskid;

        nRet = data_to_buf_transparent_acq_status(pOadInfo, &convert);  //入参
        if (nRet == DATA_SUCCESS)
        {
            bufAllLen += dOffset;
            if (bufAllLen <= maxBufLen)
            {
                memcpy(&outRecordBuf->recordBuf[offset], convert.dstBuf, dOffset);
                offset += dOffset;
                outRecordBuf->bufLen = offset;
                PRTL_BUF_LOGCC(convert.dstBuf, dOffset, "第%d个具体采集状态报文:", i);
                acqNum ++;
                if (bufAllLen + dOffset >= maxBufLen) //虽然本次没有超最大长度，但下次会
                {
                    isBufFull = TRUE;
                    PRTL_FMT_DEBUG("虽然本次没有超最大长度，但下次会 isBufFull = [%d]\n", isBufFull);
                }
                dOffset = 0;
                sOffset = 0;
            }
            else
            {
                dOffset = 0;
                sOffset = 0;
                isBufFull = TRUE;
                PRTL_FMT_DEBUG("达到最大报文长度 isBufFull = [%d]\n", isBufFull);
            }
        }
        else
        {
            continue;
        }

        if ((isBufFull) && (offset > 0))
        {
            break;
        }
    }

    db_read_record_end(hUdp, handle);
    
    if (!isBufFull)
    {
        if (relyInfo.bpBreakInfo->appSlicingFlag == 0x01)
        {
            breakInfo.appSlicingFlag = 0x01;
            breakInfo.isEnd = 0x01;                           //末尾帧
            breakInfo.seq = relyInfo.bpBreakInfo->seq + 1;    //序号加1
            breakInfo.objPre = 0x01;                          //对象属性
        }
        else
        {
            /*清除断点信息*/
            memset(&breakInfo, 0, sizeof(BP_INFO_T));
        }
    }
    else
    {
        breakInfo.reqType = 0x03;
        breakInfo.appSlicingFlag = 0x01;
        breakInfo.isEnd = 0x00;                              //不是末尾帧
        breakInfo.objPre = 0x01;                             //对象属性
        if (relyInfo.bpBreakInfo->appSlicingFlag == 0x00)
        {
            breakInfo.seq = BRK_SEQ_START;                   //首次序号
        }
        else
        {
            breakInfo.seq = relyInfo.bpBreakInfo->seq + 1;  //序号加1
        }
    }

    //刷新断点信息
    memcpy(&outRecordBuf->breakInfo, &breakInfo, sizeof(BP_INFO_T));

    PRTL_BUF_LOGCC(outRecordBuf->recordBuf, offset, "整个报文:");
    PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->appSlicingFlag = %d\n", outRecordBuf->breakInfo.appSlicingFlag);
    PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->index = %d\n", outRecordBuf->breakInfo.index);
    PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->isEnd = %d\n", outRecordBuf->breakInfo.isEnd);
    PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->objPre = %d\n", outRecordBuf->breakInfo.objPre);
    PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->seq = %d\n", outRecordBuf->breakInfo.seq);
    PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->reqType = %d\n", outRecordBuf->breakInfo.reqType);

    outRecordBuf->recordBuf[acqNumOffset] = acqNum;
    outRecordBuf->bufLen = offset;

    return DATA_SUCCESS;
}

/**
*********************************************************************
* @brief：      读记录型方案任务
* @param[in]： inReadRecord  - 筛选需要的数据
               oopRcsd       - 列选择
               readRecNum    - 读记录数量，：0：读全部记录 1：读最近的一条记录
               maxBufLen     - 最大读取buf长度
* @param[out]：outRecordBuf  -  数据记录
* @return：     =0            - 成功
               <0            - 错误码
*********************************************************************
*/
int record_trans_acq_read(DB_CLIENT hUdp, READ_INF_T inReadInf, RELY_INFO_T relyInfo, uint32 maxBufLen, RECORD_BUF_T *outRecordBuf)
{
    uint32    offset = 0;
    uint32    k = 0;
    uint32    j = 0;
    int32     ret = ERR_NORMAL;
    OOP_DAR_E nRet = DATA_OTHER_REASON;
    uint32    bufAllLen = 0;
    BP_INFO_T breakInfo = {0};                                         //断点信息
    BOOL      isBufFull = FALSE;
    OAD_INFO_T *pOadInfo = relyInfo.pOadInfo;

    /*清除断点信息, 需要注意应用分帧的未帧*/
    memset(&breakInfo, 0, sizeof(BP_INFO_T));
   
    DATA_CONVERT_T             convert;
    OOP_TRANS_TASK_RLUESTATE_T data = {0};
    uint32 sOffset = 0;
    uint32 dOffset = 0;
    uint32 acqNum = 0;
    uint32 acqNumOffset = 0;

    OOP_TRANS_TASK_T   task;
    BOOL               bTaskExist = TRUE;
    BOOL               bVirtualData = FALSE;

    READ_RECORD_T      inReadRecordTmp;
    READ_RECORD_T      inReadRecord;
    STEP_READ_RECORD_T stepReadRecord;
    RECORD_UNIT_T      stepRecordOut;
    uint32             handle = 0;
    uint32             hVirData = 0;
    uint32             recNum = 0;
    uint32             oopRecNumPn = 0;
    uint16             storeNum = 0;
    uint16             index = 0;

    uint32 startTime = 0;
    uint32 endTime = 0;
    uint32 storageTime = 0;
    OOP_TI_T ti = inReadInf.ti;

    MEMZERO(&convert, sizeof(convert));
    MEMZERO(&inReadRecord, sizeof(inReadRecord));
    memset(&inReadRecordTmp, 0, sizeof(inReadRecordTmp));
    memset(&task, 0, sizeof(task));

    acqNumOffset = offset;                                             //数量偏移
    outRecordBuf->recordBuf[offset ++] = 0x00;                          //数量，先填0
    bufAllLen = offset;

    ret = DT_DateTimeShort2Time(&inReadInf.startTime, (time_t *)&startTime);
    if (!ret)
    {
        PRTL_FMT_DEBUG("时间筛选参数转换出错! date: %d/%d/%d time: %d-%d-%d\n", inReadInf.startTime.year, inReadInf.startTime.month, 
            inReadInf.startTime.day, inReadInf.startTime.hour, inReadInf.startTime.minute, inReadInf.startTime.second);
        return ERR_PTR;
    }

    ret = DT_DateTimeShort2Time(&inReadInf.endTime, (time_t *)&endTime);
    if (!ret)
    {
        PRTL_FMT_DEBUG("时间筛选参数转换出错! date: %d/%d/%d time: %d-%d-%d\n", inReadInf.endTime.year, inReadInf.endTime.month, 
            inReadInf.endTime.day, inReadInf.endTime.hour, inReadInf.endTime.minute, inReadInf.endTime.second);
        return ERR_PTR;
    }

    //是否是方法3
    if ((inReadInf.endTask == 0) || (endTime == 0))
    {
        PRTL_FMT_DEBUG("筛选参数出错 inReadInf.endTask[%d] endTime[%d]\n", inReadInf.endTask, endTime);
        return ERR_PTR;
    }

    inReadRecord.cStart = startTime;
    inReadRecord.cEnd = endTime;

    //临时变量，备份作用
    inReadRecordTmp = inReadRecord;

    if (inReadInf.startTask == inReadInf.endTask)
    {
        inReadInf.endTask += 1;
    }
    
    for (k = inReadInf.startTask; k < inReadInf.endTask; k++)
    {
        inReadRecordTmp.recordOAD.logicId = 0;
        inReadRecordTmp.recordOAD.infoNum = k;
        inReadRecordTmp.cType = COL_TM_STORE;
        inReadRecordTmp.cStart = inReadRecord.cStart;
        inReadRecordTmp.cEnd = inReadRecord.cEnd;
        inReadRecordTmp.sortType = UP_SORT;
        inReadRecordTmp.recordOAD.road.oadMain.value = 0;
        inReadRecordTmp.recordOAD.road.oadCols.nNum = 1;
        inReadRecordTmp.recordOAD.road.oadCols.oad[0].value = 0x6e200200;
        
        inReadRecordTmp.MAC.nNum = 0;

        oopRecNumPn = 0;
        if (slicing_is_comtinue_task_fujian(relyInfo.bpBreakInfo, k, &inReadRecordTmp, &oopRecNumPn))
        {
            continue;
        }

        storeNum = get_task_store_num(hUdp, k);
        if (storeNum == 0)
        {
            PRTL_FMT_DEBUG("该任务方案[%d]存储深度为[%d]，需检查任务配置\n", k, storeNum);
            continue;
        }
        
        ret = db_read_record_start(hUdp, &inReadRecordTmp, &handle, &recNum);
        if (ret != ERR_OK)
        {
            PRTL_FMT_DEBUG("福建规约，开始读记录错误 ret[%d] infoNum[%d] recNum[%d] start[%d] end[%d]\n",
                ret, inReadRecordTmp.recordOAD.infoNum, recNum, inReadRecordTmp.cStart, inReadRecordTmp.cEnd);
        
            if (handle != 0)
            {
                /*结束读记录*/
                db_read_record_end(hUdp, handle);
                handle = 0;
            }
            continue;
        }
        else if (recNum == 0)
        {
            //1、没有配置对应任务方案的不提供该信息。
            //2、若任务方案没有在对应的时间执行过，则只回复一条记录（采集存储时标填请求的时标起始值，
            //   终端应能统计配置总数量，采集成功数量填0，其他没有的数据填0，开始时间、最后执行时间
            //   填2099年12月31日23：59：59）；若任务方案尚未配置具体任务，则配置总数量填0，其他信息
            //   参照任务尚未执行的情况。
            PRTL_FMT_DEBUG("福建规约，开始读记录为空 ret[%d] infoNum[%d] recNum[%d]\n",
                ret, inReadRecordTmp.recordOAD.infoNum, recNum);

            //读透明任务方案内容，置方案是否存在标记
            ret = transparent_task_sch_read(hUdp, k, &task);
            if (ret != ERR_OK)
            {
                bTaskExist = FALSE;
            }
            else
            {
                bTaskExist = TRUE;
            }
        
            if (bTaskExist)
            {
                //存在任务方案，需要按上述注释需求，虚拟数据
                ret = db_read_start_virtualAcq(hUdp, &inReadInf, k, &task, &hVirData, &recNum);
                PRTL_FMT_DEBUG("福建规约，开始读虚拟记录 ret[%d] infoNum[%d] recNum[%d]\n",
                    ret, inReadRecordTmp.recordOAD.infoNum, recNum);
                if (ret != ERR_OK || recNum == 0)
                {
                    db_read_end_virtualAcq(hUdp, &hVirData);
                    if (handle != 0)
                    {
                        /*结束读记录*/
                        db_read_record_end(hUdp, handle);
                        handle = 0;
                    }
                    continue;
                }
                else
                {
                    bVirtualData = TRUE;
                }
            }
            else
            {
                if (handle != 0)
                {
                    /*结束读记录*/
                    db_read_record_end(hUdp, handle);
                    handle = 0;
                }
                continue;
            }
        }
        else
        {
            PRTL_FMT_DEBUG("福建规约，开始读记录成功 ret[%d] infoNum[%d] recNum[%d] start[%d] end[%d]\n",
                ret, inReadRecordTmp.recordOAD.infoNum, recNum, inReadRecordTmp.cStart, inReadRecordTmp.cEnd);
        }

        if (recNum > storeNum)
        {
            index = recNum - storeNum;
        }
        
        for (j = 0; j < recNum; j++)
        {
            if (bVirtualData == TRUE)
            {
                memset(&stepRecordOut, 0, sizeof(stepRecordOut));
                ret = db_read_step_virtualAcq(hUdp, hVirData, j, &stepRecordOut);
                if (ret != ERR_OK)
                {
                    PRTL_FMT_DEBUG("福建规约，组织虚拟数据出错err[%d]\n", ret);
                    continue;
                }
            }
            else
            {
                ret = db_read_record_step(hUdp, handle, &stepReadRecord, &stepRecordOut);
                if(ret != ERR_OK)
                {
                    PRTL_FMT_DEBUG("单步读失败 错误码%d!\n", ret);
                    continue;
                }
            }

            //超过存储深度的数据不要
            if (j < index)
            {
                continue;
            }
            else
            {
                index = 0;
            }
        
            //OAD与返回数据
            if ((stepRecordOut.recordnum != 1) 
            || (stepRecordOut.record[0].road.oadCols.oad[0].value != pOadInfo->pTab->oad.value)
            || (stepRecordOut.record[0].outDatalen != sizeof(data)))
            {
                PRTL_FMT_DEBUG("单步读到的数据错误!\n");
                PRTL_FMT_DEBUG("单步读数据 stepRecordOut.recordnum = %d\n", stepRecordOut.recordnum);
                PRTL_FMT_DEBUG("单步读数据 stepRecordOut.record[0].road.oadCols.oad[0] = 0x%08x\n", 
                    stepRecordOut.record[0].road.oadCols.oad[0].value);
                PRTL_FMT_DEBUG("单步读数据 stepRecordOut.record[0].outDatalen = %d\n", 
                    stepRecordOut.record[0].outDatalen);
                PRTL_FMT_DEBUG("单步读数据 sizeof(OOP_TRANS_TASK_RLUESTATE_T) = %d\n", 
                    sizeof(data));
                continue;
            }
        
            memcpy(&data, stepRecordOut.record[0].outDataBuf, stepRecordOut.record[0].outDatalen);
            ret = DT_DateTimeShort2Time(&data.storeTime, (time_t *)&storageTime);
            if (!ret)
            {
                PRTL_FMT_DEBUG("时间筛选参数转换出错[%d]! date: %d/%d/%d time: %d-%d-%d\n", ret, data.storeTime.year, data.storeTime.month, 
                    data.storeTime.day, data.storeTime.hour, data.storeTime.minute, data.storeTime.second);
                continue;
            }
        
            //不符合筛选条件
            ret = record_trans_acq_is_valid(storageTime, startTime, endTime, ti);
            if (ret != ERR_OK)
            {
                PRTL_FMT_TRACE("时间筛选不符合筛选条件[%d]! date: %d/%d/%d time: %d-%d-%d\n", ret, data.storeTime.year, data.storeTime.month, 
                    data.storeTime.day, data.storeTime.hour, data.storeTime.minute, data.storeTime.second);
                continue;
            }
            
            PRTL_FMT_LOGCC("找到采集状态序号为%d\n", data.transTaskid);
        
            //转换成报文数据
            convert.srcBuf = &data;
            convert.srcLen = sizeof(data);
            convert.sOffset = &sOffset;
        
            convert.dstBuf = &outRecordBuf->recordBuf[offset];
            convert.dstLen = maxBufLen - offset;
            convert.dOffset = &dOffset;
        
            //记录断点信息
            breakInfo.metsn = data.transTaskid;
            breakInfo.tmStartSeg = stepRecordOut.colStoreTm;
            oopRecNumPn ++;
            /*breakInfo.recNumSeg = oopRecNumPn ++;
            if (oopRecNumPn > storeNum)
            {
                //达到最大记录数，跳转到下一个任务
                oopRecNumPn = 0;
                break;
            }*/
        
            nRet = data_to_buf_transparent_acq_status(pOadInfo, &convert);  //入参
            if (nRet == DATA_SUCCESS)
            {
                bufAllLen += dOffset;
                if (bufAllLen <= maxBufLen)
                {
                    if (bufAllLen + dOffset >= maxBufLen) //虽然本次没有超最大长度，但下次会
                    {
                        isBufFull = TRUE;
                        PRTL_FMT_DEBUG("虽然本次没有超最大长度，但下次会 isBufFull = [%d]\n", isBufFull);
                    }
                    else
                    {
                        memcpy(&outRecordBuf->recordBuf[offset], convert.dstBuf, dOffset);
                        offset += dOffset;
                        outRecordBuf->bufLen = offset;
                        PRTL_BUF_LOGCC(convert.dstBuf, dOffset, "第%d个具体采集状态报文:", oopRecNumPn);
                        acqNum ++;
                    }
                    dOffset = 0;
                    sOffset = 0;
                }
                else
                {
                    dOffset = 0;
                    sOffset = 0;
                    isBufFull = TRUE;
                    PRTL_FMT_DEBUG("达到最大报文长度 isBufFull = [%d]\n", isBufFull);
                }
            }
            else
            {
                continue;
            }
        
            if ((isBufFull) && (offset > 0))
            {
                break;
            }
        }

        if ((isBufFull) && (offset > 0))
        {
            break;
        }

        db_read_record_end(hUdp, handle);
        db_read_end_virtualAcq(hUdp, &hVirData);

        if (bVirtualData)
        {
            bVirtualData = FALSE;
        }
    }
    
    if (!isBufFull)
    {
        if (relyInfo.bpBreakInfo->appSlicingFlag == 0x01)
        {
            breakInfo.appSlicingFlag = 0x01;
            breakInfo.isEnd = 0x01;                           //末尾帧
            breakInfo.seq = relyInfo.bpBreakInfo->seq + 1;    //序号加1
            breakInfo.objPre = 0x01;                          //对象属性
        }
        else
        {
            /*清除断点信息*/
            memset(&breakInfo, 0, sizeof(BP_INFO_T));
        }
    }
    else
    {
        breakInfo.reqType = 0x03;
        breakInfo.appSlicingFlag = 0x01;
        breakInfo.isEnd = 0x00;                              //不是末尾帧
        breakInfo.objPre = 0x01;                             //对象属性
        if (relyInfo.bpBreakInfo->appSlicingFlag == 0x00)
        {
            breakInfo.seq = BRK_SEQ_START;                   //首次序号
        }
        else
        {
            breakInfo.seq = relyInfo.bpBreakInfo->seq + 1;  //序号加1
        }
    }

    //刷新断点信息
    memcpy(&outRecordBuf->breakInfo, &breakInfo, sizeof(BP_INFO_T));

    PRTL_BUF_LOGCC(outRecordBuf->recordBuf, offset, "整个报文:");
    PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->appSlicingFlag = %d\n", outRecordBuf->breakInfo.appSlicingFlag);
    PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->index = %d\n", outRecordBuf->breakInfo.index);
    PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->isEnd = %d\n", outRecordBuf->breakInfo.isEnd);
    PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->objPre = %d\n", outRecordBuf->breakInfo.objPre);
    PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->seq = %d\n", outRecordBuf->breakInfo.seq);
    PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->reqType = %d\n", outRecordBuf->breakInfo.reqType);

    outRecordBuf->recordBuf[acqNumOffset] = acqNum;
    outRecordBuf->bufLen = offset;

    return DATA_SUCCESS;
}

/**
*********************************************************************
* @name：      class11_invoke_get_oop_meter_record
* @brief：     class11的GET服务
* @param[in] ：pOdaInfo        OAD信息
               pBpInfo        应用分帧断点信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
               outDataMax     输出报文缓冲区最大长度

* @param[out]：outData     输出报文
* @return    ：OOP_DAR_E
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E class11_invoke_get_transparent_acq_status_rec(OAD_INFO_T *pOdaInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    uint16 i = 0;
    uint16 j = 0;
    OOP_DAR_E  nRet = DATA_OTHER_REASON;
    uint32 offset = 4;
    uint16 csdStart = 0, csdLen = 0;
    uint16 outOffset = 0;
    uint16 inDataLenTmp = InDataLen;

    OOP_PRSD_T oopRsd;
    OOP_RCSD_T oopRcsd;
    uint16     headLen = 0;
    READ_INF_T inReadInf;
    RELY_INFO_T  relyInfo;
    RECORD_BUF_T outRecordBuf;

    /* 清0处理 */
    MEMZERO(&oopRsd, sizeof(oopRsd));
    MEMZERO(&oopRcsd, sizeof(oopRcsd));
    MEMZERO(&inReadInf, sizeof(inReadInf));
    MEMZERO(&relyInfo, sizeof(relyInfo));
    MEMZERO(&outRecordBuf, sizeof(outRecordBuf));

    outRecordBuf.recordBuf = (uint8 *)malloc(pApduInfo->connectionInfo.ApplSplitLen);
    memset(outRecordBuf.recordBuf, 0xee, pApduInfo->connectionInfo.ApplSplitLen);

    /*处理RSD*/
    nRet = apdu_rsd_fromfmt698(inData, inDataLenTmp, &offset, &oopRsd, sizeof(oopRsd));
    PRTL_FMT_LOGCC("oopRsd.choice = %d\n", oopRsd.choice);
    if (nRet != DATA_SUCCESS)
    {
        PRTL_FMT_TRACE("RSD解析错误 nRet[%d]\n", nRet);

        /*应对电科院协议一致性测试*/
        uint16 testOffset = 4;
        uint16 inCsdOff = 0, inCsdLen = 0;
        apdu_next_dpos_get(inData, &testOffset, InDataLen, DT_RSD, TRUE);
        inCsdOff = testOffset;
        apdu_next_dpos_get(inData, &testOffset, InDataLen, DT_RCSD, TRUE);
        inCsdLen = testOffset - inCsdOff;
        memcpy(outData->pResultData, inData, 4);
        memcpy(outData->pResultData + 4, inData + inCsdOff, inCsdLen);
        outData->resultDataLen = 4 + inCsdLen;

        outData->pResultData[outData->resultDataLen++] = 1;                   //后面优化下，outData->resultDataLen与下面统一
        if (apdu_rcsd_fromfmt698((uint8 *)inData, InDataLen, &offset, &oopRcsd, sizeof(oopRcsd)) != DATA_SUCCESS)
        {
            outData->pResultData[outData->resultDataLen++] = 0;

        }
        else
        {
            outData->pResultData[outData->resultDataLen++] = 1;
            for (i = 0; i < oopRcsd.nNum; i++)
            {
                outData->pResultData[outData->resultDataLen++] = NULL_DATA;
            }
        }

        free(outRecordBuf.recordBuf);
        outRecordBuf.recordBuf = NULL;
        return DATA_SUCCESS;
    }
    else
    {
        inDataLenTmp -= offset;  //除去RSD和OAD后的长度
    }

    /*处理RCSD*/
    csdStart = offset;
    if (apdu_rcsd_fromfmt698(inData, InDataLen, &offset, &oopRcsd, sizeof(oopRcsd)) != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("CSD解析错误! offset[%d]\n", offset);
        free(outRecordBuf.recordBuf);
        outRecordBuf.recordBuf = NULL;
        return DATA_TYPE_UNMATCHED;
    }

    /*填写OAD+RCSD+记录结果1 并记下偏移headLen*/
    outData->resultDataLen = 0;
    csdLen = offset-csdStart;
    memcpy(outData->pResultData, inData, 4);
    outOffset = 4;

    if (csdLen == 1)
    {
        //如果rcsd为空，填充OAD6e210200
        outData->pResultData[outOffset ++] = 0x01;
        outData->pResultData[outOffset ++] = 0x00;
        outData->pResultData[outOffset ++] = 0x6e;
        outData->pResultData[outOffset ++] = 0x21;
        outData->pResultData[outOffset ++] = 0x02;
        outData->pResultData[outOffset ++] = 0x00;
    }
    else
    {
        memcpy(&outData->pResultData[outOffset], &inData[csdStart], csdLen);
        outOffset += csdLen;
    }
    outData->pResultData[outOffset++] = 1;
    outData->resultDataLen += outOffset;
    headLen = outOffset;
    PRTL_BUF_LOGCC(&outData->pResultData[0], headLen, "填写OAD+RCSD+记录结果1 并记下偏移headLen head:");

    /*记录型档案接口提供输入参数*/
    if (general_param_get_acq(&oopRsd, &oopRcsd, pApduInfo, &inReadInf, &relyInfo) != ERR_OK)
    {
        free(outRecordBuf.recordBuf);
        outRecordBuf.recordBuf = NULL;
        return DATA_READ_WRITE_DENIED;
    }

    /* 依赖信息中存下OAD列表的指针 */
    relyInfo.pOadInfo = pOdaInfo;

    PRTL_FMT_LOGCC("****************************************************\n");
    PRTL_FMT_LOGCC("inReadMeter.startInfoNum = %d\n", inReadInf.startTask);
    PRTL_FMT_LOGCC("inReadMeter.endInfoNum = %d\n", inReadInf.endTask);
    PRTL_FMT_LOGCC("relyInfo.oopRcsd.nNum = %d\n", relyInfo.oopRcsd.nNum);
    for(i = 0; i < relyInfo.oopRcsd.nNum; i++)
    {
        if (relyInfo.oopRcsd.cols[i].choice == 0)
        {
            PRTL_FMT_LOGCC("choice = %d relyInfo.oopRcsd.cols[%d].oad.value = 0x%08x\n", i, relyInfo.oopRcsd.cols[i].choice, 
                                                                                      relyInfo.oopRcsd.cols[i].oad.value);
        }
        else if(relyInfo.oopRcsd.cols[i].choice == 1)
        {
            PRTL_FMT_LOGCC("choice = %d relyInfo.oopRcsd.cols[%d].road.oadMain.value = 0x%08x\n", i, relyInfo.oopRcsd.cols[i].choice, 
                                                                                              relyInfo.oopRcsd.cols[i].road.oadMain.value);
            for (j = 0; j < relyInfo.oopRcsd.cols[i].road.oadCols.nNum; j++)
            {
                PRTL_FMT_LOGCC("choice = %d relyInfo.oopRcsd.cols[%d].road.oadCols.oad[%d].value = 0x%08x\n", i, j, relyInfo.oopRcsd.cols[i].choice, 
                                                                                                          relyInfo.oopRcsd.cols[i].road.oadCols.oad[j].value);
            }
        }
    }
    PRTL_FMT_LOGCC("****************************************************\n");

    /*读记录档案接口，可以读出apdu数据域，可返回断点信息*/
    if (record_trans_acq_read(pApduInfo->hUdp, inReadInf, relyInfo, pApduInfo->connectionInfo.ApplSplitLen - headLen - cal_excludeLen(pApduInfo->connectionInfo), &outRecordBuf))
    {
        nRet = DATA_READ_WRITE_DENIED;
    }
    else
    {
        memcpy(&outData->pResultData[headLen], outRecordBuf.recordBuf, outRecordBuf.bufLen);
        outData->resultDataLen += outRecordBuf.bufLen;

        if (outRecordBuf.breakInfo.appSlicingFlag == 1)
        {
            /*记下断点信息*/
            PRTL_FMT_LOGCC("记下断点信息\n");
            outRecordBuf.breakInfo.objPre = 0x02;   //对象属性，记录型数据
            memcpy(pApduInfo->bpBreakInfo, &outRecordBuf.breakInfo, sizeof(BP_INFO_T));
        }
        else
        {
            /*清除断点信息*/
            PRTL_FMT_LOGCC("清除断点信息\n");
            memset(pApduInfo->bpBreakInfo, 0, sizeof(BP_INFO_T));
        }
        
        nRet = DATA_SUCCESS;
    }
    
    free(outRecordBuf.recordBuf);
    outRecordBuf.recordBuf = NULL;
    return DATA_SUCCESS;
}

#endif

#if DESC("对外接口", 1)

/**
*********************************************************************
* @name：      class11_invoke_get_plan_fujian
* @brief：     class11的GET服务
* @param[in] ：pOdaInfo        OAD信息
               pBpInfo        应用分帧断点信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
               outDataMax     输出报文缓冲区最大长度

* @param[out]：outData     输出报文
* @return    ：OOP_DAR_E
* @author    :
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E class11_invoke_get_report_plan_fujian(OAD_INFO_T* pOadInfo, APDU_INFO_T* pApduInfo, const uint8* inData, uint16 inLen, RESULT_DATA_T* outData, uint16 outDataMax)
{
    //如果是应用分帧状态，将apdu[1]修改成起始帧方式
    if (pApduInfo->bpBreakInfo->appSlicingFlag == 1)
    {
        pApduInfo->reqType = pApduInfo->bpBreakInfo->reqType;
    }

    switch (pApduInfo->reqType)
    {
    case 0x01:
    case 0x02:
        return class11_invoke_get_report_plan_fujian_nor(pOadInfo, pApduInfo, inData, inLen, outData, outDataMax);
        break;
    case 0x03:
    case 0x04:
        return class11_invoke_get_report_plan_fujian_rec(pOadInfo, pApduInfo, inData, inLen, outData, outDataMax);
        break;
    default:
        break;
    }

    return DATA_OBJECT_UNDEFINED;
}

/**
*********************************************************************
* @name：       class11_invoke_get_transparent_task_node
* @brief：      class10的GET服务
* @param[in] ：pOdaInfo        OAD信息
               pApduInfo      一些必要信息，包括应用分帧断点信息
               inData         输入报文，不含OAD
               InDataLen      输入报文长度
               outDataMax     输出报文缓冲区最大长度
* @param[out]：outData     输出报文
* @return    ：OOP_DAR_E
* @author    :
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E class11_invoke_get_transparent_task_node(OAD_INFO_T* pOadInfo, APDU_INFO_T* pApduInfo, const uint8* inData,
    uint16 InDataLen, RESULT_DATA_T* outData, uint16 outDataMax)
{
    switch (pOadInfo->pTab->oad.value)
    {
    case 0x6e130200:
        return class11_invoke_get_transparent_node_all(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break;
    default:
        break;
    }
    return DATA_OBJECT_UNDEFINED;
}
/**
*********************************************************************
* @name：      class11_invoke_get_group_meter
* @brief：     class11的GET服务
* @param[in] ：pOdaInfo        OAD信息
               pBpInfo        应用分帧断点信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
               outDataMax     输出报文缓冲区最大长度

* @param[out]：outData     输出报文
* @return    ：OOP_DAR_E
* @author    :
* @Date      ：2022-03-19
*********************************************************************
*/
OOP_DAR_E class11_invoke_get_group_meter(OAD_INFO_T* pOadInfo, APDU_INFO_T* pApduInfo, const uint8* inData,
    uint16 inLen, RESULT_DATA_T* outData, uint16 outDataMax)
{
    switch (pOadInfo->pTab->oad.value)
    {
    case 0x6e050200:
        return class11_invoke_get_group_meter_all(pOadInfo, pApduInfo, inData, inLen, outData, outDataMax);
        break;
    case 0x6e050300:
        return class11_invoke_get_group_meter_num(pOadInfo, pApduInfo, inData, inLen, outData, outDataMax);
        break;
    default:
        break;
    }
    return DATA_OBJECT_UNDEFINED;
}

/**
*********************************************************************
* @name：       class10_invoke_get_task_record
* @brief：      class10的GET服务
* @param[in] ：pOdaInfo        OAD信息
               pApduInfo      一些必要信息，包括应用分帧断点信息
               inData         输入报文，不含OAD
               InDataLen      输入报文长度
               outDataMax     输出报文缓冲区最大长度
* @param[out]：outData     输出报文
* @return    ：OOP_DAR_E
* @author    :
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E class11_invoke_get_transparent_task(OAD_INFO_T* pOadInfo, APDU_INFO_T* pApduInfo, const uint8* inData,
    uint16 InDataLen, RESULT_DATA_T* outData, uint16 outDataMax)
{
    switch (pOadInfo->pTab->oad.value)
    {
    case 0x6e110200:
        return class11_invoke_get_transparent_task_all(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break;
    case 0x6e110300:
        return class11_invoke_get_config_stat(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break;
    default:
        break;
    }
    return DATA_OBJECT_UNDEFINED;
}

/**
*********************************************************************
* @name：       class11_invoke_get_acq_status
* @brief：      class10的GET服务
* @param[in] ：pOdaInfo        OAD信息
               pApduInfo      一些必要信息，包括应用分帧断点信息
               inData         输入报文，不含OAD
               InDataLen      输入报文长度
               outDataMax     输出报文缓冲区最大长度
* @param[out]：outData     输出报文
* @return    ：OOP_DAR_E
* @author    :
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E class11_invoke_get_acq_status(OAD_INFO_T* pOadInfo, APDU_INFO_T* pApduInfo, const uint8* inData,
    uint16 InDataLen, RESULT_DATA_T* outData, uint16 outDataMax)
{
    switch (pOadInfo->pTab->oad.value)
    {
    case 0x6e230200:
        return class11_invoke_get_acq_status_all(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break;
    default:
        break;
    }
    return DATA_OBJECT_UNDEFINED;
}

/**
*********************************************************************
* @name：       class11_invoke_get_acq_status
* @brief：      class10的GET服务
* @param[in] ：pOdaInfo        OAD信息
               pApduInfo      一些必要信息，包括应用分帧断点信息
               inData         输入报文，不含OAD
               InDataLen      输入报文长度
               outDataMax     输出报文缓冲区最大长度
* @param[out]：outData     输出报文
* @return    ：OOP_DAR_E
* @author    :
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E class11_invoke_get_transparent_event(OAD_INFO_T* pOadInfo, APDU_INFO_T* pApduInfo, const uint8* inData,
    uint16 InDataLen, RESULT_DATA_T* outData, uint16 outDataMax)
{
    switch (pOadInfo->pTab->oad.value)
    {
    case 0x6e310200:
        return class11_invoke_get_transparent_event_res(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break;
    case 0x6e310300:
        return class11_invoke_get_transparent_event_status(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break;
    default:
        break;
    }
    return DATA_OBJECT_UNDEFINED;
}


/**
*********************************************************************
* @name：       class11_invoke_get_transparent_acq_status
* @brief：      class10的GET服务
* @param[in] ：pOdaInfo        OAD信息
               pApduInfo      一些必要信息，包括应用分帧断点信息
               inData         输入报文，不含OAD
               InDataLen      输入报文长度
               outDataMax     输出报文缓冲区最大长度
* @param[out]：outData     输出报文
* @return    ：OOP_DAR_E
* @author    :
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E class11_invoke_get_transparent_acq_status(OAD_INFO_T* pOadInfo, APDU_INFO_T* pApduInfo, const uint8* inData,
    uint16 InDataLen, RESULT_DATA_T* outData, uint16 outDataMax)
{
    if (pOadInfo->pTab->oad.value != 0x6e200200)
    {
        PRTL_FMT_DEBUG("读采集状态集合错误，主OAD选择错误 oad[0x%08x]", pOadInfo->pTab->oad.value);
        return DATA_OBJECT_UNDEFINED;
    }
        
    //如果是应用分帧状态，将apdu[1]修改成起始帧方式
    if (pApduInfo->bpBreakInfo->appSlicingFlag == 1)
    {
        pApduInfo->reqType = pApduInfo->bpBreakInfo->reqType;
    }

    switch (pApduInfo->reqType)
    {
    case 0x01:
    case 0x02:
        //return class11_invoke_get_transparent_event_nor(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break;
    case 0x03:
    case 0x04:
        return class11_invoke_get_transparent_acq_status_rec(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break;
    default:
        break;
    }

    return DATA_OBJECT_UNDEFINED;

}

/**
*********************************************************************
* @name：      class11_invoke_act_group_meter
* @brief：     class11的ACTION服务
* @param[in] ：pOdaInfo        OAD信息
               pBpInfo        应用分帧断点信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
               outDataMax     输出报文缓冲区最大长度

* @param[out]：outData     输出报文
* @return    ：OOP_DAR_E
* @author    :
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E class11_invoke_act_transparent_task(OAD_INFO_T* pOadInfo, APDU_INFO_T* pApduInfo, const uint8* inData, 
    uint16 inLen, RESULT_DATA_T* outData, uint16 outDataMax)
{
    switch (pOadInfo->pTab->oad.value)
    {
    case 0x6e117f00:
        return class11_invoke_add_transparent_task(pOadInfo, pApduInfo, inData, inLen, outData, outDataMax);
        break;
    case 0x6e118000:
        return class11_invoke_del_transparent_task(pOadInfo, pApduInfo, inData, inLen, outData, outDataMax);
        break;
    case 0x6e118100:
        return class11_invoke_clr_transparent_task(pOadInfo, pApduInfo, inData, inLen, outData, outDataMax);
        break;
    case 0x6e118200:
        return class11_invoke_upd_transparent_task(pOadInfo, pApduInfo, inData, inLen, outData, outDataMax);
        break;
    default:
        break;
    }
    return DATA_OBJECT_UNDEFINED;
}

/**
*********************************************************************
* @name：      class11_invoke_act_group_meter
* @brief：     class11的ACTION服务
* @param[in] ：pOdaInfo        OAD信息
               pBpInfo        应用分帧断点信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
               outDataMax     输出报文缓冲区最大长度

* @param[out]：outData     输出报文
* @return    ：OOP_DAR_E
* @author    :
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E class11_invoke_act_group_meter(OAD_INFO_T* pOadInfo, APDU_INFO_T* pApduInfo, const uint8* inData, 
    uint16 inLen, RESULT_DATA_T* outData, uint16 outDataMax)
{
    switch (pOadInfo->pTab->oad.value)
    {
    case 0x6e057f00:
        return class11_invoke_add_group_meter(pOadInfo, pApduInfo, inData, inLen, outData, outDataMax);
        break;
    case 0x6e058000:
        return class11_invoke_del_group_meter(pOadInfo, pApduInfo, inData, inLen, outData, outDataMax);
        break;
    case 0x6e058200:
        return class11_invoke_clr_group_meter(pOadInfo, pApduInfo, inData, inLen, outData, outDataMax);
        break;
    case 0x6e058300:
        return class11_invoke_del_group_meter_by_ms(pOadInfo, pApduInfo, inData, inLen, outData, outDataMax);
        break;
    default:
        break;
    }
    return DATA_OBJECT_UNDEFINED;
}

/**
*********************************************************************
* @name：       class11_invoke_act_transparent_task_node
* @brief：      class11的ACTION服务
* @param[in] ：pOdaInfo        OAD信息
               pBpInfo        应用分帧断点信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
               outDataMax     输出报文缓冲区最大长度

* @param[out]：outData     输出报文
* @return    ：OOP_DAR_E
* @author    :
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E class11_invoke_act_transparent_task_node(OAD_INFO_T* pOadInfo, APDU_INFO_T* pApduInfo, const uint8* inData, 
    uint16 inLen, RESULT_DATA_T* outData, uint16 outDataMax)
{
    switch (pOadInfo->pTab->oad.value)
    {
    case 0x6e137f00:
        return class11_invoke_add_transparent_node(pOadInfo, pApduInfo, inData, inLen, outData, outDataMax);
        break;
    case 0x6e138000:
        return class11_invoke_del_transparent_node_by_ms(pOadInfo, pApduInfo, inData, inLen, outData, outDataMax);
        break;
    case 0x6e138100:
        return class11_invoke_del_transparent_node_by_task(pOadInfo, pApduInfo, inData, inLen, outData, outDataMax);
        break;
    case 0x6e138200:
        return class11_invoke_clr_transparent_node(pOadInfo, pApduInfo, inData, inLen, outData, outDataMax);
        break;
    case 0x6e138300:
        return class11_invoke_del_node_by_ms_without_task(pOadInfo, pApduInfo, inData, inLen, outData, outDataMax);
        break;
    default:
        break;
    }
    return DATA_OBJECT_UNDEFINED;
}

/**
*********************************************************************
* @name：       class11_invoke_act_transparent_task_node
* @brief：      class11的ACTION服务
* @param[in] ：pOdaInfo        OAD信息
               pBpInfo        应用分帧断点信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
               outDataMax     输出报文缓冲区最大长度

* @param[out]：outData     输出报文
* @return    ：OOP_DAR_E
* @author    :
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E class11_invoke_act_transparent_report(OAD_INFO_T* pOadInfo, APDU_INFO_T* pApduInfo, const uint8* inData, 
    uint16 inLen, RESULT_DATA_T* outData, uint16 outDataMax)
{
    switch (pOadInfo->pTab->oad.value)
    {
    case 0x6e177f00:
        return class11_invoke_add_transparent_report(pOadInfo, pApduInfo, inData, inLen, outData, outDataMax);
        break;
    case 0x6e178000:
        return class11_invoke_del_transparent_report(pOadInfo, pApduInfo, inData, inLen, outData, outDataMax);
        break;
    case 0x6e178100:
        return class11_invoke_clr_transparent_report(pOadInfo, pApduInfo, inData, inLen, outData, outDataMax);
        break;
    case 0x6e178400:
        return class11_invoke_tri_transparent_report(pOadInfo, pApduInfo, inData, inLen, outData, outDataMax);
        break;
    default:
        break;
    }
    return DATA_OBJECT_UNDEFINED;
}

/**
*********************************************************************
* @name：       class11_invoke_act_transparent_task_node
* @brief：      class11的ACTION服务
* @param[in] ：pOdaInfo        OAD信息
               pBpInfo        应用分帧断点信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
               outDataMax     输出报文缓冲区最大长度

* @param[out]：outData     输出报文
* @return    ：OOP_DAR_E
* @author    :
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E class11_invoke_act_transparent_event(OAD_INFO_T* pOadInfo, APDU_INFO_T* pApduInfo, const uint8* inData, 
    uint16 inLen, RESULT_DATA_T* outData, uint16 outDataMax)
{
    switch (pOadInfo->pTab->oad.value)
    {
    case 0x6e317f00:
        return class11_invoke_clr_transparent_event(pOadInfo, pApduInfo, inData, inLen, outData, outDataMax);
        break;
    case 0x6e318000:
        return class11_invoke_tri_transparent_event(pOadInfo, pApduInfo, inData, inLen, outData, outDataMax);
        break;
    default:
        break;
    }
    return DATA_OBJECT_UNDEFINED;
}

#endif
