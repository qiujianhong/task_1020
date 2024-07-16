/*
*********************************************************************
* @file    class10.c
* @brief： class10处理
* @author：
* @date：   2019.12.06
* @copyright 2019 南京新联电子股份有限公司 All rights reserved.
*********************************************************************
*/

#include "baseCommon.h"
#include "oadInfoList.h"
#include "oopservice.h"
#include "commClass.h"
#include "oopStorage.h"

//
// class10
//

/**
*********************************************************************
* @brief：     CSD_T -- > OOP_RCSD
* @param[in]： pCSD          - 列选择
* @param[out]：OOP_RCSD      - 列选择
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
int32 oop_rcsd_get(CSD_T *pCSD, OOP_RCSD_T *oopRcsd)
{
    OOP_RCSD_T oopRcsdTmp = {0};
    uint32 num = 0;
    uint32 i   = 0;
    uint32 j   = 0;
    CSD_T *pCsdTmp = pCSD;

    if ((pCSD == NULL) || (oopRcsd == NULL))
    {
        PRTL_FMT_DEBUG("oop_rcsd_get input para null pCSD[%p] oopRcsd[%p]\n", pCSD, oopRcsd);
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
        PRTL_FMT_DEBUG("oop_rcsd_get num err num[%d]\n", num);
        return ERR_OPTION;
    }

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

    *oopRcsd = oopRcsdTmp;

    return ERR_OK;
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
int32 read_record_param_get(RSD_T *pRSD, CSD_T *pCSD, APDU_INFO_T *pApduInfo, READ_RECORD_T *pInReadRecord, RELY_INFO_T *pRelyInfo, uint32 *pReadRecNum)
{
    int32 nRet = ERR_NORMAL;
    uint16 mainOI = 0;
    PlanTypeInfo_t planTypeInfo = {0};
    OadQueryInfo_t oadQueryInfo = {0};
    READ_RECORD_T  inReadRecord;
    RELY_INFO_T    relyInfo = {0};
    uint32         readRecNum = 0;
    BOOL qxTest    = FALSE;                 //配合电科院台体测试曲线数据召读
    uint32 startTm = 0;
    uint32 endTm   = 0;
    uint32 i       = 0;
    CSD_T *pCSDTmp = pCSD;
    uint32 oadNum  = 0;

    memset(&inReadRecord, 0, sizeof(READ_RECORD_T));
    planTypeInfo.planType = NORMALPLAN;
    memset(&oadQueryInfo.sOad, 0, sizeof(OOP_OAD_U));

    while(pCSDTmp)
    {
        if (pCSDTmp->CSDType == 1)
        {
            mainOI = pCSDTmp->Road.MainOad.nObjID;
            if (((mainOI >= 0x5000) && (mainOI <= 0x500B))
            ||(mainOI == 0x5010) || (mainOI == 0x5011))
            {
                planTypeInfo.planType = NORMALPLAN;

                /*填写主OAD和数量*/
                inReadRecord.recordOAD.classtag = 1;
                inReadRecord.recordOAD.road.oadMain.value = pCSDTmp->Road.MainOad.value;
            }
            //else if ((mainOI >= 0x3000) && (mainOI <= 0x302F))
            else if ((mainOI & 0x3000) == 0x3000)    
            {
                planTypeInfo.planType = EVENTPLAN;

                /*填写主OAD和数量*/
                inReadRecord.recordOAD.classtag = 1;
                inReadRecord.recordOAD.road.oadMain.value = pCSDTmp->Road.MainOad.value;
            }
            else
            {
                return ERR_NORMAL;
            }
            planTypeInfo.planMainOad = pCSDTmp->Road.MainOad;

            /*填写子OAD和数量, 注意，可能有多个ROAD的情况(主OAD需相同，如果主OAD不同也不支持)*/
            oadNum = pCSDTmp->Road.SubOadNum;
            for (i = 0; i < pCSDTmp->Road.SubOadNum; i++)
            {
                /*数据中心都是总存分取，索引都是0*/
                inReadRecord.recordOAD.road.oadCols.oad[inReadRecord.recordOAD.road.oadCols.nNum + i].value = pCSDTmp->Road.SubOad[i].value;
                inReadRecord.recordOAD.road.oadCols.oad[inReadRecord.recordOAD.road.oadCols.nNum + i].nIndex = 0;
            }
            inReadRecord.recordOAD.road.oadCols.nNum += oadNum;
        }
        else
        {
            //if ((pCSDTmp->Oad.nObjID != 0x6040) && (pCSDTmp->Oad.nObjID != 0x6041) && (pCSDTmp->Oad.nObjID != 0x2021)
            //    && (pCSDTmp->Oad.nObjID != 0x6042) && (pCSDTmp->Oad.nObjID != 0x4001) && (pCSDTmp->Oad.nObjID != 0x202A))
            if ((pCSDTmp->Oad.nObjID != 0x6040) && (pCSDTmp->Oad.nObjID != 0x6041) && (pCSDTmp->Oad.nObjID != 0x2021)
                && (pCSDTmp->Oad.nObjID != 0x6042) && (pCSDTmp->Oad.nObjID != 0x202A))    
            {
                //单OAD时为实时数据，主OAD就填单OAD 数据中心都是总存分取，索引都是0
                inReadRecord.recordOAD.classtag = 0;
                inReadRecord.recordOAD.road.oadMain.value = 0x00;
                inReadRecord.recordOAD.road.oadCols.oad[oadNum].value = pCSDTmp->Oad.value;
                if((pCSDTmp->Oad.nObjID != 0x400C) && (pCSDTmp->Oad.nObjID != 0x401E))
                    inReadRecord.recordOAD.road.oadCols.oad[oadNum].nIndex = 0;
                oadNum ++;
                inReadRecord.recordOAD.road.oadCols.nNum = oadNum;

                //单OAD时为实时数据，一般就读最近一条记录，此要求注释掉
                #if 0
                readRecNum = 1;
                #endif
            }
        }
        pCSDTmp = pCSDTmp->next;
    }

    //因为oopType.h中宏的限制，目前关联OAD最大数量是OOP_MAX_OADS
    if (inReadRecord.recordOAD.road.oadCols.nNum > JS_OOP_MAX_OADS)
    {
        return DATA_SCOPE_OF_ACCESS_VIOLATED;
    }

    /*填写逻辑地址*/
    inReadRecord.recordOAD.logicId = 0;

    /*填写信息点*/
    inReadRecord.recordOAD.infoNum = 0;

    switch (pRSD->selId)
    {
    case 4://指定电能表集合、指定采集启动时间
    case 5://指定电能表集合、指定采集存储时间
    case 6://指定电能表集合、指定采集启动时间区间内连续间隔值
    case 7://指定电能表集合、指定采集存储时间区间内连续间隔值
    case 8://指定电能表集合、指定采集成功时间区间内连续间隔值
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
                PRTL_FMT_LOGCC("read_record_param_get startTm:%s time[s]:%d\n",strT, startTm);
                strftime(strT, sizeof(strT), "%Y-%m-%d %H:%M:%S", localtime((time_t*)&endTm));
                PRTL_FMT_LOGCC("read_record_param_get endTm:%s time[s]:%d\n",strT, endTm);

                // 注：+8个小时(28800)转成格林时间
                startTm += COMPENSATION_TIME;
                endTm += COMPENSATION_TIME;
            }
            

            if(pRSD->selId == 5)//指定电能表集合、指定采集存储时间。
            {
                // if(planTypeInfo.planMainOad.nObjID == 0x5002) ????
                // {
                    inReadRecord.cStart = startTm;
                    inReadRecord.cEnd   = startTm;
                    inReadRecord.cType  = COL_TM_STORE;
                    qxTest = TRUE;
                // }
            }
            else if(pRSD->selId == 4)//Selector4为指定电能表集合、指定采集启动时间
            {
                // if(planTypeInfo.planMainOad.nObjID == 0x5002) ???
                // {
                    inReadRecord.cStart = startTm;
                    inReadRecord.cEnd   = startTm;
                    inReadRecord.cType  = COL_TM_START;
                    qxTest = TRUE;
                // }
            }
            else if(pRSD->selId == 6)//指定电能表集合、指定采集启动时间区间内连续间隔
            {
                inReadRecord.cStart = startTm;
                inReadRecord.cEnd   = endTm;
                inReadRecord.cType  = COL_TM_START;
                inReadRecord.ti      = pRSD->Ti;
            }
            else if(pRSD->selId == 7)//指定电能表集合、指定采集启动时间区间内连续间隔
            {
                inReadRecord.cStart = startTm;
                inReadRecord.cEnd   = endTm;
                inReadRecord.cType  = COL_TM_STORE;
                inReadRecord.ti      = pRSD->Ti;
                //如果是单OAD的实时数据，按采集起始时间筛选
                if (inReadRecord.recordOAD.road.oadMain.value == 0x00)
                {
                    inReadRecord.cType  = COL_TM_START;

                    //此处也注释掉
                    #if 0
                    inReadRecord.cEnd   = endTm + 1;
                    #endif
                }
            }
            else if(pRSD->selId == 8)//指定电能表集合、指定采集成功时间区间内连续间隔值
            {
                inReadRecord.cStart = startTm;
                inReadRecord.cEnd   = endTm;
                inReadRecord.cType  = COL_TM_END;
                inReadRecord.ti      = pRSD->Ti;
            }
            else
            {
                nRet =  DATA_READ_WRITE_DENIED;
            }
            nRet = ERR_OK;
        }
        break;
        case 10:
        {
            // 注：+8个小时(28800)转成格林时间
            time_t  RealTime_t = time(NULL) + COMPENSATION_TIME;
            inReadRecord.cStart = 0;
            inReadRecord.cEnd   = RealTime_t;
            inReadRecord.cType  = COL_TM_STORE;
            readRecNum           = pRSD->LastN;

            //防止从0开始筛选，可能造成数据中心压力很大，开始时间根据数据类型往前推部分时间。
            if ((inReadRecord.recordOAD.road.oadMain.value == 0x50020200) ||
                (inReadRecord.recordOAD.road.oadMain.value == 0x00))
            {
                //3天的秒数(259200 = 3*24*60*60)
                if (RealTime_t > 259200)
                {
                    inReadRecord.cStart = RealTime_t - 259200;
                }
            }
            else if (inReadRecord.recordOAD.road.oadMain.value == 0x50030200)
            {
                //7天的秒数(604800 = 7*24*60*60)
                if (RealTime_t > 604800)
                {
                    inReadRecord.cStart = RealTime_t - 604800;
                }
            }
            else if (inReadRecord.recordOAD.road.oadMain.value == 0x50040200)
            {
                //31天的秒数(2678400 = 31*24*60*60)
                if (RealTime_t > 2678400)
                {
                    inReadRecord.cStart = RealTime_t - 2678400;
                }
            }
            else if ((inReadRecord.recordOAD.road.oadMain.value == 0x50050200) ||
                     (inReadRecord.recordOAD.road.oadMain.value == 0x50060200))
            {
                //一年的秒数(32140800 = 12*31*24*60*60)
                if (RealTime_t > 32140800)
                {
                    inReadRecord.cStart = RealTime_t - 32140800;
                }
            }
            
            #ifdef AREA_SHANDONG
            if(pApduInfo->isreport == 1)
            {
                inReadRecord.ti = pApduInfo->runTi;
                
                //山东只上报一个周期内的数据
                long longtime=0;
                if(inReadRecord.ti.unit>5)
                    nRet = DATA_OBJECT_CLASS_INCONSISTENT;
                if(inReadRecord.ti.value == 0xFFFF)
                    nRet = DATA_OBJECT_CLASS_INCONSISTENT;
                
                if(inReadRecord.ti.unit<4)
                {

                    if(inReadRecord.ti.unit==0)
                        longtime = inReadRecord.ti.value;
                    else if(inReadRecord.ti.unit==1)
                        longtime = (long)inReadRecord.ti.value*60;
                    else if(inReadRecord.ti.unit==2)
                        longtime = (long)inReadRecord.ti.value*3600;
                    else
                        longtime = (long)inReadRecord.ti.value*86400;
                    if(RealTime_t<longtime)
                        nRet = DATA_OBJECT_CLASS_INCONSISTENT;
                    inReadRecord.cStart = RealTime_t - longtime;
                }
            }
            
            #endif
            nRet = ERR_OK;
        }
        break;
    default:
        nRet = DATA_READ_WRITE_DENIED;
        break;
    }

    /*pCSD -- > oopRcsd*/
    pCSDTmp = pCSD;
    if (oop_rcsd_get(pCSDTmp, &relyInfo.oopRcsd) != ERR_OK)
    {
        nRet = ERR_OPTION;
    }

    if (nRet == ERR_OK)
    {
        
        relyInfo.bpBreakInfo = pApduInfo->bpBreakInfo;
        relyInfo.ms    = pRSD->Ms;
        *pInReadRecord = inReadRecord;
        *pReadRecNum   = readRecNum;
        *pRelyInfo     = relyInfo;
    }

    PRTL_FMT_LOGCC("planType%d, qText(%d)\n",planTypeInfo.planType, qxTest);

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
int32 read_record_transparent_param_get(RSD_T *pRSD, CSD_T *pCSD, APDU_INFO_T *pApduInfo, READ_RECORD_T *pInReadRecord, RELY_INFO_T *pRelyInfo, uint32 *pReadRecNum)
{
    int32 nRet = ERR_NORMAL;
    
    PlanTypeInfo_t planTypeInfo = {0};
    OadQueryInfo_t oadQueryInfo = {0};
    READ_RECORD_T  inReadRecord;
    RELY_INFO_T    relyInfo = {0};
    uint32         readRecNum = 0;
    BOOL qxTest    = FALSE;                 //配合电科院台体测试曲线数据召读
    uint32 startTm = 0;
    uint32 endTm   = 0;
    uint32 i       = 0;
    CSD_T *pCSDTmp = pCSD;
    uint32 oadNum  = 0;

    memset(&inReadRecord, 0, sizeof(READ_RECORD_T));
    planTypeInfo.planType = NORMALPLAN;
    memset(&oadQueryInfo.sOad, 0, sizeof(OOP_OAD_U));

    /*填写主OAD和数量*/
    inReadRecord.recordOAD.classtag = 1;
    inReadRecord.recordOAD.road.oadMain.value = pCSDTmp->Road.MainOad.value;


    /*填写子OAD和数量*/
    oadNum = pCSDTmp->Road.SubOadNum;
    for (i = 0; i < pCSDTmp->Road.SubOadNum; i++)
    {

        inReadRecord.recordOAD.road.oadCols.oad[i].value = pCSDTmp->Road.SubOad[i].value;
    }
    inReadRecord.recordOAD.road.oadCols.nNum = oadNum;

    /*填写逻辑地址*/
    inReadRecord.recordOAD.logicId = 0;

    /*填写信息点*/
    inReadRecord.recordOAD.infoNum = 0;

    switch (pRSD->selId)
    {
    case 4://指定电能表集合、指定采集启动时间
    case 5://指定电能表集合、指定采集存储时间
    case 6://指定电能表集合、指定采集启动时间区间内连续间隔值
    case 7://指定电能表集合、指定采集存储时间区间内连续间隔值
    case 8://指定电能表集合、指定采集成功时间区间内连续间隔值
        {
            if (!DT_DateTimeHex2Time((DateTimeHex_t*)pRSD->StartData, (time_t*)&startTm) || 
               (!DT_DateTimeHex2Time((DateTimeHex_t*)pRSD->EndData, (time_t*)&endTm)))
            {
                return DATA_READ_WRITE_DENIED;
            }
            else
            {
                char strT[128];
                strftime(strT, sizeof(strT), "%Y-%m-%d %H:%M:%S", localtime((time_t*)&startTm));
                PRTL_FMT_LOGCC("read_record_param_get startTm:%s time[s]:%d\n",strT, startTm);
                strftime(strT, sizeof(strT), "%Y-%m-%d %H:%M:%S", localtime((time_t*)&endTm));
                PRTL_FMT_LOGCC("read_record_param_get endTm:%s time[s]:%d\n",strT, endTm);

                // 注：+8个小时(28800)转成格林时间
                startTm += COMPENSATION_TIME;
                endTm += COMPENSATION_TIME;
            }
            

            if(pRSD->selId == 5)//指定电能表集合、指定采集存储时间。
            {
                // if(planTypeInfo.planMainOad.nObjID == 0x5002) ????
                // {
                    inReadRecord.cStart = startTm;
                    inReadRecord.cEnd   = startTm;
                    inReadRecord.cType  = COL_TM_STORE;
                    qxTest = TRUE;
                // }
            }
            else if(pRSD->selId == 4)//Selector4为指定电能表集合、指定采集启动时间
            {
                // if(planTypeInfo.planMainOad.nObjID == 0x5002) ???
                // {
                    inReadRecord.cStart = startTm;
                    inReadRecord.cEnd   = startTm;
                    inReadRecord.cType  = COL_TM_START;
                    qxTest = TRUE;
                // }
            }
            else if(pRSD->selId == 6)//指定电能表集合、指定采集启动时间区间内连续间隔
            {
                inReadRecord.cStart = startTm;
                inReadRecord.cEnd   = endTm;
                inReadRecord.cType  = COL_TM_START;
                inReadRecord.ti      = pRSD->Ti;
            }
            else if(pRSD->selId == 7)//指定电能表集合、指定采集启动时间区间内连续间隔
            {
                inReadRecord.cStart = startTm;
                inReadRecord.cEnd   = endTm;
                inReadRecord.cType  = COL_TM_STORE;
                inReadRecord.ti      = pRSD->Ti;

                //如果是单OAD的实时数据，按采集起始时间刷新
                if (inReadRecord.recordOAD.road.oadMain.value == 0x00)
                {
                    inReadRecord.cType  = COL_TM_START;
                    inReadRecord.cEnd   = endTm + 1;
                }

            }
            else if(pRSD->selId == 8)//指定电能表集合、指定采集成功时间区间内连续间隔值
            {
                inReadRecord.cStart = startTm;
                inReadRecord.cEnd   = endTm;
                inReadRecord.cType  = COL_TM_END;
                inReadRecord.ti      = pRSD->Ti;
            }
            else
            {
                nRet =  DATA_READ_WRITE_DENIED;
            }
            nRet = ERR_OK;
        }
        break;
        case 10:
        {
            // 注：+8个小时(28800)转成格林时间
            time_t  RealTime_t = time(NULL) + COMPENSATION_TIME;
            inReadRecord.cStart = 0;
            inReadRecord.cEnd   = RealTime_t;
            inReadRecord.cType  = COL_TM_STORE;
            readRecNum           = pRSD->LastN;
            nRet = ERR_OK;
        }
        break;
    default:
        nRet = DATA_READ_WRITE_DENIED;
        break;
    }

    /*pCSD -- > oopRcsd*/
    pCSDTmp = pCSD;
    if (oop_rcsd_get(pCSDTmp, &relyInfo.oopRcsd) != ERR_OK)
    {
        nRet = ERR_OPTION;
    }

    if (nRet == ERR_OK)
    {
        
        relyInfo.bpBreakInfo = pApduInfo->bpBreakInfo;
        relyInfo.ms    = pRSD->Ms;
        *pInReadRecord = inReadRecord;
        *pReadRecNum   = readRecNum;
        *pRelyInfo     = relyInfo;
    }

    PRTL_FMT_LOGCC("planType%d, qText(%d)\n",planTypeInfo.planType, qxTest);

    return nRet;
}


/**
*********************************************************************
* @brief：     从数据中心的数据转698格式报文(class10)
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_class10(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    return 0;
}

OOP_DAR_E buf_to_data_class10(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    return DATA_SUCCESS;
}

/**
*********************************************************************
* @name：       class10_invoke_get
* @brief：      class10的GET服务
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
OOP_DAR_E class10_invoke_get_task_list_nor(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    uint16 i = 0;
    OOP_DAR_E  nRet = DATA_OTHER_REASON;
    uint32 offset = 0;

    int result = ERR_NORMAL,ret;
    uint32 sOffset = 0;
    uint32 dOffset = 0,len;
    OOP_TASK_T TASK_T;
    NOMAL_OAD_T NormalOad ;
    DATA_CONVERT_T convert;
    uint32    bufAllLen = 0;
    BP_INFO_T breakInfo = {0};                                                //断点信息
    uint32    maxBufLen = 0;
    BOOL      isBufFull = FALSE;
    
    maxBufLen = pApduInfo->connectionInfo.ApplSplitLen - cal_excludeLen(pApduInfo->connectionInfo);

    /*清除断点信息, 需要注意应用分帧的未帧*/
    memset(&breakInfo, 0, sizeof(BP_INFO_T));


    /*类型不存在*/
    if (pOadInfo->pTab->oad.nObjID != 0x6012)
    {
        return DATA_OBJECT_UNAVAILABLE;
    }

    switch (pOadInfo->pTab->oad.attID)
    {
    case 2:
        {
            printf("召测任务数据\n");
            if(pOadInfo->nIndex==0x00)
            {
                uint8 tasknum = 0;
                uint32 oopTaskNumOffset = 0;
                memcpy(&outData->pResultData[offset], inData, sizeof(OOP_OAD_U));   //拷贝OAD
                offset += sizeof(OOP_OAD_U);
                outData->pResultData[offset ++] = 0x01;                             //结果
                outData->pResultData[offset ++] = DT_ARRAY;                         //类型
                oopTaskNumOffset = offset;                                         //任务数量偏移
                outData->pResultData[offset ++] = 0x00;                             //数量，先填0
                bufAllLen = offset;
                for(i=1;i<=255;i++)
                { 

                    if (slicing_is_comtinue_task(pApduInfo->bpBreakInfo, i))
                    {
                        continue;
                    }
                    NormalOad.logicId = 0;
                    NormalOad.infoNum = i;
                    NormalOad.oad.value=0x60120200;

                    memset((uint8*)&TASK_T,0x00,sizeof(OOP_TASK_T));
                    #if TASK_READ_MOD == 1
                    ret=db_read_nomal(pApduInfo->hUdp, &NormalOad,sizeof(OOP_TASK_T), (uint8*)&TASK_T, &len);
                    if ((ret != ERR_OK) || (len != sizeof(OOP_TASK_T)))
                    #else
                    ret = app_get_task_list(i, &TASK_T);
                    if (ret != ERR_OK)
                    #endif
                    {
                        continue;
                    }

                    //转换成报文数据
                    convert.srcBuf = &TASK_T;
                    convert.srcLen = sizeof(OOP_TASK_T);
                    convert.sOffset = &sOffset;

                    convert.dstBuf = &outData->pResultData[offset];
                    convert.dstLen = outDataMax - offset;
                    convert.dOffset = &dOffset;
                    
                    result = data_to_buf_task_config_uint(pOadInfo, &convert);

                    //记录断点信息
                    breakInfo.index = i;
                    
                    if(result == ERR_OK)
                    {
                        bufAllLen += dOffset;
                        if (bufAllLen <= maxBufLen)
                        {
                            memcpy(&outData->pResultData[offset], convert.dstBuf, dOffset);
                            offset += dOffset;
                            outData->resultDataLen = offset;
                            PRTL_BUF_LOGCC(convert.dstBuf, dOffset, "第%d个任务报文:", i);
                            tasknum ++;
                            dOffset = 0;
                            sOffset = 0;
                        }
                        else
                        {
                            dOffset = 0;
                            sOffset = 0;
                            isBufFull = TRUE;
                            PRTL_FMT_LOGCC("class10_invoke_get_task_list_nor isBufFull == TRUE isBufFull = %d\n", isBufFull);
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

                PRTL_BUF_LOGCC(outData->pResultData, offset, "整个任务报文:");
                PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->appSlicingFlag = %d\n", pApduInfo->bpBreakInfo->appSlicingFlag);
                PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->index = %d\n", pApduInfo->bpBreakInfo->index);
                PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->isEnd = %d\n", pApduInfo->bpBreakInfo->isEnd);
                PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->objPre = %d\n", pApduInfo->bpBreakInfo->objPre);
                PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->seq = %d\n", pApduInfo->bpBreakInfo->seq);
                PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->reqType = %d\n", pApduInfo->bpBreakInfo->reqType);

                if (tasknum > 0)
                {
                    outData->pResultData[oopTaskNumOffset] = tasknum;   //重新记档案数量
                    outData->resultDataLen = offset;
                    nRet = DATA_SUCCESS;
                }
                else if(tasknum == 0)
                {
                    memcpy_r(outData->pResultData,(uint8*)&pOadInfo->pTab->oad.value,4);
                    outData->pResultData[4]=DT_ARRAY;
                    outData->pResultData[5]=0x01;
                    outData->pResultData[6]=0x00;
                    outData->resultDataLen=7;
                    nRet = DATA_SUCCESS;
                }
                else
                {
                    nRet = DATA_SUCCESS;
                }
                
            }
            else
            {
                    NormalOad.logicId = 0;
                    NormalOad.infoNum = pOadInfo->nIndex;
                    NormalOad.oad.value=0x60120200;
                    convert.dstBuf = &outData->pResultData[5];
                    convert.dstLen = outDataMax - 5;
                    convert.dOffset = &dOffset;
                    memset((uint8*)&TASK_T,0x00,sizeof(OOP_TASK_T));
                    #if TASK_READ_MOD == 1
                    ret=db_read_nomal(pApduInfo->hUdp, &NormalOad,sizeof(OOP_TASK_T), (uint8*)&TASK_T, &len);
                    if ((ret != ERR_OK) || (len != sizeof(OOP_TASK_T)))
                    #else
                    ret = app_get_task_list(i, &TASK_T);
                    if (ret != ERR_OK)
                    #endif
                    {
                        return DATA_OBJECT_UNAVAILABLE;
                    }

                    //转换成报文数据
                    convert.srcBuf = &TASK_T;
                    convert.srcLen = sizeof(OOP_TASK_T);
                    convert.sOffset = &sOffset;

                    convert.dstBuf = &outData->pResultData[offset];
                    convert.dstLen = outDataMax - offset;
                    convert.dOffset = &dOffset;
                    
                    result = data_to_buf_task_config_uint(pOadInfo, &convert);
                    nRet=data_to_buf_task_config_uint(pOadInfo, &convert);
                    if(nRet == ERR_OK)
                    {
                        memcpy(outData->pResultData,inData,4);
                        outData->pResultData[4]=0x01;
                        outData->resultDataLen=dOffset+5;
                    }
                    else
                    {
                        memcpy(outData->pResultData,inData,4);
                        outData->pResultData[4]=0x00;
                        outData->pResultData[5]=DATA_OBJECT_UNAVAILABLE;
                        outData->resultDataLen=6;
                    }
            }
        }
        break;

    default:
        {
            return DATA_OBJECT_UNAVAILABLE;
        }
        break;
    }

    return nRet;
}

/**
*********************************************************************
* @name：      class10_invoke_get_task_list_rec
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
OOP_DAR_E class10_invoke_get_task_list_rec(OAD_INFO_T *pOdaInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    uint16 i = 0;
    uint16 j = 0;
    OOP_DAR_E  nRet = DATA_OTHER_REASON;
    uint32 offset = 4;
    uint16 csdStart = 0, csdLen = 0;
    uint16 outOffset = 0;
    uint16 inDataLenTmp = InDataLen;

    /* 类型不存在 */
    if (pOdaInfo->pTab->oad.nObjID != 0x6012)
    {
        return DATA_OBJECT_UNAVAILABLE;
    }

    switch (pOdaInfo->pTab->oad.attID)
    {
    case 2:
        {
            OOP_PRSD_T oopRsd;
            OOP_RCSD_T oopRcsd;
            uint16 headLen = 0;
            READ_NOR_BP_T inReadNor;
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
                PRTL_FMT_TRACE("CSD解析错误!\n");
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
                //如果rcsd为空，填充OAD60010200
                outData->pResultData[outOffset ++] = 0x01;
                outData->pResultData[outOffset ++] = 0x00;
                outData->pResultData[outOffset ++] = 0x60;
                outData->pResultData[outOffset ++] = 0x13;
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
            if (read_nor_param_get(&oopRsd, &oopRcsd, pApduInfo, &inReadNor, &relyInfo) != ERR_OK)
            {
                free(outRecordBuf.recordBuf);
                outRecordBuf.recordBuf = NULL;
                return DATA_READ_WRITE_DENIED;
            }

            /* 依赖信息中存下OAD列表的指针 */
            relyInfo.pOadInfo = pOdaInfo;

            PRTL_FMT_LOGCC("****************************************************\n");
            PRTL_FMT_LOGCC("inReadNor.startInfoNum = %d\n", inReadNor.startInfoNum);
            PRTL_FMT_LOGCC("inReadNor.endInfoNum = %d\n", inReadNor.endInfoNum);
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
            if (record_task_read(pApduInfo->hUdp, inReadNor, relyInfo, pApduInfo->connectionInfo.ApplSplitLen - headLen - cal_excludeLen(pApduInfo->connectionInfo), &outRecordBuf))
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
        break;
    default:
        {
            return DATA_OBJECT_UNAVAILABLE;
        }
        break;
    }

    return nRet;
}

/**
*********************************************************************
* @name：      class11_invoke_get_oop_meter
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
OOP_DAR_E class10_invoke_get_task_list(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 inLen, RESULT_DATA_T *outData, uint16 outDataMax)
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
    case 0x06:
        return class10_invoke_get_task_list_nor(pOadInfo, pApduInfo, inData, inLen, outData, outDataMax);
        break;
    case 0x03:
    case 0x04:
        return class10_invoke_get_task_list_rec(pOadInfo, pApduInfo, inData, inLen, outData, outDataMax);
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
OOP_DAR_E class10_invoke_get_task_record(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    uint16 i = 0;
    uint16 j = 0;
    OOP_DAR_E  nRet = DATA_OTHER_REASON;
    uint16 offset = 4;
    uint16 csdStart = 0, csdLen = 0;
    uint16 outOffset = 0;
    uint16 inDataLenTmp = InDataLen;
    READ_RECORD_T inReadRecord;
    RELY_INFO_T   relyInfo;
    uint32        readRecNum;

    /* 类型不存在 */
    if (pOadInfo->pTab->oad.nObjID != 0x6012)
    {
        return DATA_OBJECT_UNAVAILABLE;
    }

    switch (pOadInfo->pTab->oad.attID)
    {
    case 3:
        {
            CSD_T *pCSD = NULL, *pCSDHead = NULL;
            RSD_T *pRSD = (RSD_T *)malloc(sizeof(RSD_T));
            pRSD->StartData = NULL;
            pRSD->EndData = NULL;
            MS_T *ms = NULL;
            uint16 headLen = 0;
            RECORD_BUF_T outRecordBuf = {0};

            outRecordBuf.recordBuf = (uint8 *)malloc(pApduInfo->connectionInfo.ApplSplitLen);
            memset(outRecordBuf.recordBuf, 0xee, pApduInfo->connectionInfo.ApplSplitLen);
            outRecordBuf.tmEndSeg = 0;

            /*处理RSD*/
            memset(pRSD, 0, sizeof(RSD_T)); //清0
            offset = rsd_get(pApduInfo->hUdp, (uint8 *)(inData + offset), inDataLenTmp - 4, pRSD);
            if (offset == 0)
            {
                PRTL_FMT_DEBUG("RSD解析错误\n");
                free(outRecordBuf.recordBuf);
                outRecordBuf.recordBuf = NULL;
                if (pRSD->StartData != NULL)
                {
                    free(pRSD->StartData);
                }
                if (pRSD->EndData!= NULL)
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
                pCSDHead = rcsd_get((uint8 *)inData, InDataLen, &offset, FALSE);
                if (!pCSDHead || !(pCSDHead->next))
                {
                    outData->pResultData[outData->resultDataLen++] = 0;
                    rcsd_destroy(pCSDHead);
                }
                else
                {
                    outData->pResultData[outData->resultDataLen++] = 1;
                    pCSD = pCSDHead->next;
                    while(pCSD)
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
                    //return enumDarDataTypeErr;
                    //如果是方法10，就回所有表的上一次日冻结电能
                    uint16 mmoffset = 0;
                    uint8 msssbuff[10] = {0};
                    msssbuff[0] = 1;
                    pRSD->selId = 10;
                    pRSD->LastN = 1;
                    pRSD->Ms = mtr_list_get(pApduInfo->hUdp, msssbuff, &mmoffset, FALSE);
                    if (!pRSD->Ms)
                    {
                        free(pRSD);
                        free(outRecordBuf.recordBuf);
                        outRecordBuf.recordBuf = NULL;
                        return DATA_TYPE_UNMATCHED;
                    }
                    ms = pRSD->Ms;
                }
            }

            /*处理RCSD*/
            csdStart = offset;
            pCSDHead = rcsd_get((uint8 *)inData, InDataLen, &offset, FALSE);

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
                PRTL_FMT_DEBUG("无CSD\n");
                free(pRSD->StartData);
                free(pRSD->EndData);
                ms_destroy(pRSD->Ms);
                free(pRSD);
                free(pCSDHead);
                pCSDHead = NULL;
                free(outRecordBuf.recordBuf);
                outRecordBuf.recordBuf = NULL;
                return DATA_READ_WRITE_DENIED;
            }

            /*填写OAD+RCSD+记录结果1 并记下偏移headLen*/
            outData->resultDataLen = 0;
            csdLen = offset-csdStart;
            memcpy(outData->pResultData, inData, 4);
            outOffset = 4;
            memcpy(&outData->pResultData[outOffset], &inData[csdStart], csdLen);
            outOffset += csdLen;
            outData->pResultData[outOffset++] = 1;
            outData->resultDataLen += outOffset;
            headLen = outOffset;
            PRTL_BUF_LOGCC(&outData->pResultData[0], headLen, "填写OAD+RCSD+记录结果1 并记下偏移headLen head:");

            /*给读任务记录表接口提供输入参数*/
            pCSD = pCSDHead->next;
            nRet = read_record_param_get(pRSD, pCSD, pApduInfo, &inReadRecord, &relyInfo, &readRecNum);
            if (nRet != ERR_OK)
            {
                PRTL_FMT_DEBUG("read_record_param_get err[%d]\n", nRet);
                rcsd_destroy(pCSDHead);
                if ((pRSD->selId >= 4) && (pRSD->selId <= 8))
                {
                    free(pRSD->StartData);
                    free(pRSD->EndData);
                }
                ms_destroy(pRSD->Ms);
                free(pRSD);
                free(outRecordBuf.recordBuf);
                outRecordBuf.recordBuf = NULL;
                return nRet;
            }
            
            PRTL_FMT_LOGCC("**************打印任务记录表接口提供输入参数*****************\n");
            char strT[128];
            switch(inReadRecord.cType)
            {
                case COL_TM_START: {PRTL_FMT_LOGCC("CHOOSE_TYPE_E:%s\n", "COL_TM_START");     break;}
                case COL_TM_END:   {PRTL_FMT_LOGCC("CHOOSE_TYPE_E:%s\n", "COL_TM_END");       break;}
                case COL_TM_STORE: {PRTL_FMT_LOGCC("CHOOSE_TYPE_E:%s\n", "COL_TM_STORE");     break;}
                case COL_NO_STORE: {PRTL_FMT_LOGCC("CHOOSE_TYPE_E:%s\n", "COL_NO_STORE");     break;}
                case COL_NO_INFO:  {PRTL_FMT_LOGCC("CHOOSE_TYPE_E:%s\n", "COL_NO_INFO");      break;}
                case COL_NOT:      {PRTL_FMT_LOGCC("CHOOSE_TYPE_E:%s\n", "COL_NOT");          break;}
                default:           {PRTL_FMT_LOGCC("CHOOSE_TYPE_E:%s\n", "NO CHOOSE_TYPE_E"); break;}
            }
            uint32 tmpTime = inReadRecord.cStart - COMPENSATION_TIME;
            strftime(strT, sizeof(strT), "%Y-%m-%d %H:%M:%S", localtime((time_t*)&tmpTime));
            PRTL_FMT_LOGCC("inReadRecord.cStart:%s time[s]:%d\n",strT, inReadRecord.cStart);
            tmpTime = inReadRecord.cEnd - COMPENSATION_TIME;
            strftime(strT, sizeof(strT), "%Y-%m-%d %H:%M:%S", localtime((time_t*)&tmpTime));
            PRTL_FMT_LOGCC("inReadRecord.cEnd:%s time[s]:%d\n",strT, inReadRecord.cEnd);
            PRTL_FMT_LOGCC("inReadRecord.ti.unit = %d\n", inReadRecord.ti.unit);
            PRTL_FMT_LOGCC("inReadRecord.ti.value = %d\n", inReadRecord.ti.value);
            PRTL_FMT_LOGCC("readRecNum = %d\n", readRecNum);
            PRTL_BUF_LOGCC(&inReadRecord.MAC, 6, "inReadRecord.MAC");
            PRTL_FMT_LOGCC("inReadRecord.recordOAD.logicId = %d\n", inReadRecord.recordOAD.logicId);
            PRTL_FMT_LOGCC("inReadRecord.recordOAD.infoNum = %d\n", inReadRecord.recordOAD.infoNum);
            PRTL_FMT_LOGCC("inReadRecord.recordOAD.oadMain = 0x%08x\n", inReadRecord.recordOAD.road.oadMain.value);
            PRTL_FMT_LOGCC("inReadRecord.recordOAD.road.oadCols.nNum = %d\n", inReadRecord.recordOAD.road.oadCols.nNum);
            for(i = 0; i < inReadRecord.recordOAD.road.oadCols.nNum; i++)
            {
                PRTL_FMT_LOGCC("inReadRecord.recordOAD.road.oad[%d] = 0x%08x\n", i, inReadRecord.recordOAD.road.oadCols.oad[i]);
            }
            
            PRTL_FMT_LOGCC("relyInfo.oopRcsd.nNum = %d\n", relyInfo.oopRcsd.nNum);
            for(i = 0; i < relyInfo.oopRcsd.nNum; i++)
            {
                if (relyInfo.oopRcsd.cols[i].choice == 0)
                {
                    PRTL_FMT_LOGCC("choice = %d relyInfo.oopRcsd.cols[%d].oad.value = 0x%08x\n", relyInfo.oopRcsd.cols[i].choice, i,  
                                                                                              relyInfo.oopRcsd.cols[i].oad.value);
                }
                else if(relyInfo.oopRcsd.cols[i].choice == 1)
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
            PRTL_FMT_LOGCC("**************打印任务记录表接口提供输入参数结束******************\n");

            /*读任务记录表接口，可以读出apdu数据域，可返回断点信息*/
            if (record_task_table_read(pApduInfo->hUdp, inReadRecord, relyInfo, readRecNum, pApduInfo->connectionInfo.ApplSplitLen - headLen - cal_excludeLen(pApduInfo->connectionInfo), &outRecordBuf))
            {
                /*清除断点信息*/
                PRTL_FMT_DEBUG("record_task_table_read err[%d]\n", DATA_READ_WRITE_DENIED);
                memset(pApduInfo->bpBreakInfo, 0, sizeof(BP_INFO_T));
                nRet = DATA_READ_WRITE_DENIED;
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
                
                nRet = DATA_SUCCESS;
            }
            
            rcsd_destroy(pCSDHead);
            if ((pRSD->selId >= 4) && (pRSD->selId <= 8))
            {
                free(pRSD->StartData);
                free(pRSD->EndData);
            }
            ms_destroy(pRSD->Ms);
            free(pRSD);
            free(outRecordBuf.recordBuf);
            outRecordBuf.recordBuf = NULL;
        }
        break;
    default:
        {
            return DATA_OBJECT_UNAVAILABLE;
        }
        break;
    }

    return nRet;
}


#if DESC("class10对外接口函数",1)

/**
*********************************************************************
* @name：       class10_invoke_set
* @brief：      class10的SET服务
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
OOP_DAR_E class10_invoke_set(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 inLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    uint16 outLen = 0;

    return outLen;
}

/**
*********************************************************************
* @name：       class10_invoke_get
* @brief：      class10的GET服务
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
OOP_DAR_E class10_invoke_get(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    switch (pOadInfo->pTab->oad.value)
    {
    case 0x60120200:
        return class10_invoke_get_task_list(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break;

    case 0x60120300:
        return class10_invoke_get_task_record(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break; 

    default:
        break;
    }
    return DATA_OBJECT_UNAVAILABLE;
}

/**
*********************************************************************
* @name：       class10_invoke_act
* @brief：      class10的ACTION服务
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
OOP_DAR_E class10_invoke_act(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint32 inLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    uint16 i;
    uint32 offset = 4;
    uint8 task_num=0;
    int result = DATA_OTHER_REASON;
    int ret = ERR_NORMAL;

    /*类型不存在*/
    printf("pOadInfo->pTab->oad.nObjID is %x\n",pOadInfo->pTab->oad.nObjID);
    if (pOadInfo->pTab->oad.nObjID != 0x6012)
    {
        return DATA_OBJECT_UNAVAILABLE;
    }
    switch (pOadInfo->pTab->oad.value)
    {
        case 0x60127F00:
        {
            printf("class10_invoke_act offset is %d\n",offset);
            if(inData[offset++]!=DT_ARRAY)
            {
                return ERR_OPTION;
            }
            task_num=inData[offset++];
            printf("task_num is %d\n",task_num);
            for(i=0;i<task_num;i++)
            {
                ret = buf_to_data_class8_task_config_uint(pOadInfo, inData, (uint16)inLen, &offset,pApduInfo->hUdp);
            }
            memcpy((uint8*)&outData->pResultData[0], (uint8*)&inData[0], 4);
            if(ret == ERR_OK)
            {
                result = DATA_SUCCESS;
                outData->pResultData[4] = DATA_SUCCESS;                  //DAR type
                outData->pResultData[5] = 0x00;                          //DATA:NULL
                outData->resultDataLen = 0x06;                    
            }
            else
            {
                result = DATA_READ_WRITE_DENIED;
                outData->pResultData[4] = DATA_READ_WRITE_DENIED;
                outData->resultDataLen=0x05;
            }
        }
        break;
        case 0x60128000:
        {
            uint8 task_id=0;
            CLEAR_DATA_T RecordDel;

            MEMZERO(&RecordDel, sizeof(RecordDel));

            if(inData[offset++]!=DT_ARRAY)
            {
                return ERR_OPTION;
            }

            task_num=inData[offset++];
            for(i=0;i<task_num;i++)
            {
                offset++;
                task_id=inData[offset++];

                //删除存储的临时数据
                RecordDel.recordOAD.logicId = 0;
                RecordDel.recordOAD.infoNum = task_id;
                RecordDel.recordOAD.classtag = CLASS_DATA_INIT;
                RecordDel.recordOAD.road.oadMain.value = 0;
                RecordDel.recordOAD.road.oadCols.nNum = 1;
                RecordDel.recordOAD.road.oadCols.oad[0].value = 0x60120200;
                RecordDel.bClassTag = FALSE;

                ret = db_clear_oad_data(pApduInfo->hUdp,&RecordDel);
                if(ret==0)
                {
                    PRTL_FMT_LOGCC("数据删除成功！\n");
                }else
                {
                    PRTL_FMT_DEBUG("数据删除失败[%d]！\n", task_id);
                }
    
            }
            memcpy((uint8*)&outData->pResultData[0], (uint8*)&inData[0], 4);
            if(ret == ERR_OK)
            {
                result = DATA_SUCCESS;
                outData->pResultData[4] = DATA_SUCCESS;                  //DAR type
                outData->pResultData[5] = 0x00;                          //DATA:NULL
                outData->resultDataLen = 0x06;                    
            }
            else
            {
                result = DATA_READ_WRITE_DENIED;
                outData->pResultData[4] = DATA_READ_WRITE_DENIED;
                outData->resultDataLen=0x05;
            }
        }
        break;
        case 0x60128100:
        {

            CLEAR_DATA_T RecordDel;

            MEMZERO(&RecordDel, sizeof(RecordDel));

            ret = ERR_OK;
            for(i = 0; i < 256; i++)
            {
                //判断任务是否有效
                if (!oopTaskList.isVaild[i])
                {
                    continue;
                }

                //删除存储的临时数据
                RecordDel.recordOAD.logicId = 0;
                RecordDel.recordOAD.infoNum = i;
                RecordDel.recordOAD.classtag = CLASS_DATA_INIT;
                RecordDel.recordOAD.road.oadMain.value = 0;
                RecordDel.recordOAD.road.oadCols.nNum = 1;
                RecordDel.recordOAD.road.oadCols.oad[0].value = 0x60120200;
                RecordDel.bClassTag = FALSE;

                ret = db_clear_oad_data(pApduInfo->hUdp,&RecordDel);
                if(ret==0)
                {
                    PRTL_FMT_LOGCC("数据删除成功！\n");
                }else
                {
                    PRTL_FMT_DEBUG("数据删除失败[%d]！！！\n", i);
                }
    
            }

            memcpy((uint8*)&outData->pResultData[0], (uint8*)&inData[0], 4);
            if(ret == ERR_OK)
            {
                result = DATA_SUCCESS;
                outData->pResultData[4] = DATA_SUCCESS;                  //DAR type
                outData->pResultData[5] = 0x00;                          //DATA:NULL
                outData->resultDataLen = 0x06;                    
            }
            else
            {
                result = DATA_READ_WRITE_DENIED;
                outData->pResultData[4] = DATA_READ_WRITE_DENIED;
                outData->resultDataLen=0x05;
            }
        }
        break;
        case 0x60128200:
        {
            ret =buf_to_data_class8_update_task_config_uint(pOadInfo, inData, (uint16)inLen, &offset,pApduInfo->hUdp);
            if(ret == ERR_OK)
            {
                result = DATA_SUCCESS;
                outData->pResultData[4] = DATA_SUCCESS;                  //DAR type
                outData->pResultData[5] = 0x00;                          //DATA:NULL
                outData->resultDataLen = 0x06;                    
            }
            else
            {
                result = DATA_READ_WRITE_DENIED;
                outData->pResultData[4] = DATA_READ_WRITE_DENIED;
                outData->resultDataLen=0x05;
            }
        }
        break;

    }
    
    
    return result;
}


#endif
