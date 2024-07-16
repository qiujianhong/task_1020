/*
*********************************************************************
* Copyright(C) 2022 南京新联电子股份有限公司
* All rights reserved.
* @brief：   福建规约
* @date：    2022-03-19
* @history：
*********************************************************************
*/


#include "baseCommon.h"
#include "oadInfoList.h"
#include "oopservice.h"
#include "commClass.h"
#include "oopStorage.h"
#include "oopVarUpdate.h"
#include "area.h"

#if DESC("福建特殊方法3召测", 1)
/**
*********************************************************************
* @brief：      通用参数获取
* @param[in]： pRSD          -  行选择
               pCSD          - 列选择
* @param[out]：pPlanTypeInfo - 目的数据
* @return：     =0            - 成功
               <0            - 错误码
*********************************************************************
*/
int32 general_param_get_node(OOP_PRSD_T *pOopRsd, OOP_RCSD_T *pOopRcsd, APDU_INFO_T *pApduInfo, 
    READ_INF_T *pInReadInf, RELY_INFO_T *pRelyInfo)
{
    int32 nRet = ERR_NORMAL;
    READ_INF_T   inReadInfo = {0};
    RELY_INFO_T    relyInfo = {0};

    switch (pOopRsd->choice)
    {
    case 0://方法0
    case 1://方法1
    case 2://方法2
    case 3://方法3
        {
            if (pOopRsd->choice == 0)
            {
                inReadInfo.startTask = 1;
                inReadInfo.endTask = 0xffff;
                inReadInfo.startNode = 1;
                inReadInfo.endNode = 0xffff;
            }       
            else if(pOopRsd->choice == 1)
            {
                if (pOopRsd->sel1.value.type == DT_LONG_UNSIGNED)
                {
                    inReadInfo.startTask = pOopRsd->sel1.value.wVal;
                    inReadInfo.endTask = pOopRsd->sel1.value.wVal;
                    inReadInfo.startNode = 1;
                    inReadInfo.endNode = 0xffff;
                }
                else
                {
                    PRTL_FMT_LOGCC("pOopRsd->sel1.value.type = [%d]\n", pOopRsd->sel1.value.type);
                }
            }
            else if(pOopRsd->choice == 2)//前闭后开
            {
                inReadInfo.startNode = 1;
                inReadInfo.endNode = 0xffff;
                
                if (pOopRsd->sel2.from.type == DT_LONG_UNSIGNED)
                {
                    inReadInfo.startTask = pOopRsd->sel2.from.wVal;
                }
                else
                {
                    PRTL_FMT_LOGCC("pOopRsd->sel1.value.type != DT_LONG_UNSIGNED\n");
                }

                if (pOopRsd->sel2.to.type == DT_LONG_UNSIGNED)
                {
                    inReadInfo.endTask = pOopRsd->sel2.to.wVal;
                }
                else
                {
                    PRTL_FMT_LOGCC("pOopRsd->sel2.from.type = [%d]\n", pOopRsd->sel2.from.type);
                }
            }
            else if(pOopRsd->choice == 3)
            {
                uint32 i = 0;
                
                if (pOopRsd->sel3.nNum != 2)
                {
                    PRTL_FMT_DEBUG("方法出错，数量不支持，pOopRsd->sel3.nNum = [%d]\n", pOopRsd->sel3.nNum);
                    nRet = ERR_NORMAL;
                    break;
                }
                
                for (i = 0; i < pOopRsd->sel3.nNum; i++)
                {
                    if ((pOopRsd->sel3.item[i].oad.value == 0x6e120201) || 
                        (pOopRsd->sel3.item[i].oad.value == 0x6e140201))
                    {
                        inReadInfo.startTask = pOopRsd->sel3.item[i].from.wVal;
                        inReadInfo.endTask   = pOopRsd->sel3.item[i].to.wVal;
                    }
                    else if (pOopRsd->sel3.item[i].oad.value == 0x6e140202)
                    {
                        inReadInfo.startNode = pOopRsd->sel3.item[i].from.wVal;
                        inReadInfo.endNode   = pOopRsd->sel3.item[i].to.wVal;
                    }
                    else if (pOopRsd->sel3.item[i].oad.value == 0x6e140203)
                    {
                        inReadInfo.ms.choice = 0x06;
                        inReadInfo.ms.msra.size = 0x01;
                        inReadInfo.ms.msra.region[0].nType = 0;
                        inReadInfo.ms.msra.region[0].start = pOopRsd->sel3.item[i].from.tsa;
                        inReadInfo.ms.msra.region[0].end = pOopRsd->sel3.item[i].to.tsa;
                    }
                    else
                    {
                        PRTL_FMT_DEBUG("存在不合法的筛选OAD[%d]\n", pOopRsd->sel3.item[i].oad.value);
                    }
                }
            }
            else
            {
                nRet =  ERR_NORMAL;
            }
            nRet = ERR_OK;
        }
        break;

    default:
        nRet = ERR_NORMAL;
        break;
    }

    if (nRet == ERR_OK)
    {
        relyInfo.oopRsd  = pOopRsd;
        relyInfo.oopRcsd = *pOopRcsd;
        relyInfo.bpBreakInfo = pApduInfo->bpBreakInfo;
        *pInReadInf = inReadInfo;
        *pRelyInfo     = relyInfo;
    }

    return nRet;
}

/**
*********************************************************************
* @brief：      通用参数获取
* @param[in]： pRSD          -  行选择
               pCSD          - 列选择
* @param[out]：pPlanTypeInfo - 目的数据
* @return：     =0            - 成功
               <0            - 错误码
*********************************************************************
*/
int32 general_param_get_acq(OOP_PRSD_T *pOopRsd, OOP_RCSD_T *pOopRcsd, APDU_INFO_T *pApduInfo, 
    READ_INF_T *pInReadInf, RELY_INFO_T *pRelyInfo)
{
    int32 nRet = ERR_NORMAL;
    READ_INF_T   inReadInfo = {0};
    RELY_INFO_T    relyInfo = {0};

    switch (pOopRsd->choice)
    {
    case 0://方法0
    case 1://方法1
    case 2://方法2
    case 3://方法3
        {
            if (pOopRsd->choice == 0)
            {
                inReadInfo.startTask = 1;
                inReadInfo.endTask = 0xffff;
                inReadInfo.startNode = 1;
                inReadInfo.endNode = 0xffff;
            }       
            else if(pOopRsd->choice == 1)
            {
                if (pOopRsd->sel1.value.type == DT_LONG_UNSIGNED)
                {
                    inReadInfo.startTask = pOopRsd->sel1.value.wVal;
                    inReadInfo.endTask = pOopRsd->sel1.value.wVal;
                    inReadInfo.startNode = 1;
                    inReadInfo.endNode = 0xffff;
                }
                else
                {
                    PRTL_FMT_LOGCC("pOopRsd->sel1.value.type = [%d]\n", pOopRsd->sel1.value.type);
                }
            }
            else if(pOopRsd->choice == 2)//前闭后开
            {
                inReadInfo.startNode = 1;
                inReadInfo.endNode = 0xffff;
                
                if (pOopRsd->sel2.from.type == DT_LONG_UNSIGNED)
                {
                    inReadInfo.startTask = pOopRsd->sel2.from.wVal;
                }
                else
                {
                    PRTL_FMT_LOGCC("pOopRsd->sel1.value.type != DT_LONG_UNSIGNED\n");
                }

                if (pOopRsd->sel2.to.type == DT_LONG_UNSIGNED)
                {
                    inReadInfo.endTask = pOopRsd->sel2.to.wVal;
                }
                else
                {
                    PRTL_FMT_LOGCC("pOopRsd->sel2.from.type = [%d]\n", pOopRsd->sel2.from.type);
                }
            }
            else if(pOopRsd->choice == 3)
            {
                uint32 i = 0;
                
                if (pOopRsd->sel3.nNum != 2)
                {
                    PRTL_FMT_DEBUG("方法出错，数量不支持，pOopRsd->sel3.nNum = [%d]\n", pOopRsd->sel3.nNum);
                    nRet = ERR_NORMAL;
                    break;
                }
                
                for (i = 0; i < pOopRsd->sel3.nNum; i++)
                {
                    if ((pOopRsd->sel3.item[i].oad.value == 0x6e120201) || 
                        (pOopRsd->sel3.item[i].oad.value == 0x6e210201))
                    {
                        inReadInfo.startTask = pOopRsd->sel3.item[i].from.wVal;
                        inReadInfo.endTask   = pOopRsd->sel3.item[i].to.wVal;
                    }
                    else if (pOopRsd->sel3.item[i].oad.value == 0x6e210202)
                    {
                        inReadInfo.startTime = pOopRsd->sel3.item[i].from.tmVal;
                        inReadInfo.endTime   = pOopRsd->sel3.item[i].to.tmVal;
                        inReadInfo.ti        = pOopRsd->sel3.item[i].span.ti;
                    }
                    else
                    {
                        PRTL_FMT_DEBUG("存在不合法的筛选OAD[%d]\n", pOopRsd->sel3.item[i].oad.value);
                    }
                }
            }
            else
            {
                nRet =  ERR_NORMAL;
            }
            nRet = ERR_OK;
        }
        break;

    default:
        nRet = ERR_NORMAL;
        break;
    }

    if (nRet == ERR_OK)
    {
        relyInfo.oopRsd  = pOopRsd;
        relyInfo.oopRcsd = *pOopRcsd;
        relyInfo.bpBreakInfo = pApduInfo->bpBreakInfo;
        *pInReadInf = inReadInfo;
        *pRelyInfo     = relyInfo;
    }

    return nRet;
}

/**
*********************************************************************
* @brief：      通用参数获取
* @param[in]： pRSD          -  行选择
               pCSD          - 列选择
* @param[out]：pPlanTypeInfo - 目的数据
* @return：     =0            - 成功
               <0            - 错误码
*********************************************************************
*/
int32 general_param_get_acq_status(OOP_PRSD_T *pOopRsd, OOP_RCSD_T *pOopRcsd, APDU_INFO_T *pApduInfo, 
    READ_INF_T *pInReadInf, RELY_INFO_T *pRelyInfo)
{
    int32 nRet = ERR_NORMAL;
    READ_INF_T   inReadInfo = {0};
    RELY_INFO_T    relyInfo = {0};

    switch (pOopRsd->choice)
    {
    case 0://方法0
    case 1://方法1
    case 2://方法2
    case 3://方法3
        {
            if (pOopRsd->choice == 0)
            {
                inReadInfo.startTask = 1;
                inReadInfo.endTask = 0xffff;
                inReadInfo.startNode = 1;
                inReadInfo.endNode = 0xffff;
            }       
            else if(pOopRsd->choice == 1)
            {
                if (pOopRsd->sel1.value.type == DT_LONG_UNSIGNED)
                {
                    inReadInfo.startTask = pOopRsd->sel1.value.wVal;
                    inReadInfo.endTask = pOopRsd->sel1.value.wVal;
                    inReadInfo.startNode = 1;
                    inReadInfo.endNode = 0xffff;
                }
                else if (pOopRsd->sel1.value.type == DT_TSA)
                {
                    inReadInfo.tsa = pOopRsd->sel1.value.tsa;
                }
                else
                {
                    PRTL_FMT_LOGCC("pOopRsd->sel1.value.type = [%d]\n", pOopRsd->sel1.value.type);
                }
            }
            else
            {
                nRet =  ERR_NORMAL;
            }
            nRet = ERR_OK;
        }
        break;

    default:
        nRet = ERR_NORMAL;
        break;
    }

    if (nRet == ERR_OK)
    {
        relyInfo.oopRsd  = pOopRsd;
        relyInfo.oopRcsd = *pOopRcsd;
        relyInfo.bpBreakInfo = pApduInfo->bpBreakInfo;
        *pInReadInf = inReadInfo;
        *pRelyInfo     = relyInfo;
    }

    return nRet;
}

/**
*********************************************************************
* @brief：      通用参数获取
* @param[in]： pRSD          -  行选择
               pCSD          - 列选择
* @param[out]：pPlanTypeInfo - 目的数据
* @return：     =0            - 成功
               <0            - 错误码
*********************************************************************
*/
int32 general_param_get_fj(OOP_PRSD_T *pOopRsd, OOP_RCSD_T *pOopRcsd, APDU_INFO_T *pApduInfo, 
    READ_INF_T *pInReadInf, RELY_INFO_T *pRelyInfo)
{
    int32 nRet = ERR_NORMAL;
    READ_INF_T   inReadInfo = {0};
    RELY_INFO_T    relyInfo = {0};

    switch (pOopRsd->choice)
    {
    case 0://方法0
    case 1://方法1
    case 2://方法2
    case 3://方法3
        {
            if (pOopRsd->choice == 0)
            {
                inReadInfo.startTask = 1;
                inReadInfo.endTask = 0xffff;
                inReadInfo.startNode = 1;
                inReadInfo.endNode = 0xffff;
            }       
            else if(pOopRsd->choice == 1)
            {
                if (pOopRsd->sel1.value.type == DT_LONG_UNSIGNED)
                {
                    inReadInfo.startTask = pOopRsd->sel1.value.wVal;
                    inReadInfo.endTask = pOopRsd->sel1.value.wVal;
                    inReadInfo.startNode = 1;
                    inReadInfo.endNode = 0xffff;
                }
                else if (pOopRsd->sel1.value.type == DT_OAD)
                {
                    inReadInfo.startNode = 1;
                    inReadInfo.endNode = 0xffff;
                    inReadInfo.startTask = pOopRsd->sel1.value.oad.value;
                    inReadInfo.endTask = pOopRsd->sel1.value.oad.value;
                }
                else if (pOopRsd->sel1.value.type == DT_TSA)
                {
                    inReadInfo.startNode = 0;
                    inReadInfo.endNode = 0xffff;
                    inReadInfo.startTask = 0;
                    inReadInfo.endTask = 0xffff;
                    inReadInfo.tsa = pOopRsd->sel1.value.tsa;
                }
                else
                {
                    PRTL_FMT_DEBUG("pOopRsd->sel1.value.type = [%d]\n", pOopRsd->sel1.value.type);
                }
            }
            else if(pOopRsd->choice == 2)//前闭后开
            {
                inReadInfo.startNode = 1;
                inReadInfo.endNode = 0xffff;
                
                if (pOopRsd->sel2.from.type == DT_LONG_UNSIGNED)
                {
                    inReadInfo.startTask = pOopRsd->sel2.from.wVal;
                }
                else if (pOopRsd->sel2.from.type == DT_OAD)
                {
                    inReadInfo.startTask = pOopRsd->sel2.from.oad.value;
                }
                else if (pOopRsd->sel2.from.type == DT_UNSIGNED)
                {
                    inReadInfo.startTask = pOopRsd->sel2.from.byVal;
                }
                else
                {
                    PRTL_FMT_DEBUG("pOopRsd->sel1.value.type != DT_LONG_UNSIGNED\n");
                }

                if (pOopRsd->sel2.to.type == DT_LONG_UNSIGNED)
                {
                    inReadInfo.endTask = pOopRsd->sel2.to.wVal;
                }
                else if (pOopRsd->sel2.to.type == DT_OAD)
                {
                    inReadInfo.endTask = pOopRsd->sel2.to.oad.value;
                }
                else if (pOopRsd->sel2.to.type == DT_UNSIGNED)
                {
                    inReadInfo.endTask = pOopRsd->sel2.to.byVal;
                }
                else
                {
                    PRTL_FMT_DEBUG("pOopRsd->sel2.from.type = [%d]\n", pOopRsd->sel2.from.type);
                }
            }
            else if(pOopRsd->choice == 3)
            {
                uint32 i = 0;
                
                if (pOopRsd->sel3.nNum != 2)
                {
                    PRTL_FMT_DEBUG("方法出错，数量不支持，pOopRsd->sel3.nNum = [%d]\n", pOopRsd->sel3.nNum);
                    nRet = ERR_NORMAL;
                    break;
                }
                
                for (i = 0; i < pOopRsd->sel3.nNum; i++)
                {
                    if ((pOopRsd->sel3.item[i].oad.value == 0x6e120201) || 
                        (pOopRsd->sel3.item[i].oad.value == 0x6e210201))
                    {
                        inReadInfo.startTask = pOopRsd->sel3.item[i].from.wVal;
                        inReadInfo.endTask   = pOopRsd->sel3.item[i].to.wVal;
                    }
                    else if (pOopRsd->sel3.item[i].oad.value == 0x6e210202)
                    {
                        inReadInfo.startTime = pOopRsd->sel3.item[i].from.tmVal;
                        inReadInfo.endTime   = pOopRsd->sel3.item[i].to.tmVal;
                    }
                    else
                    {
                        PRTL_FMT_DEBUG("存在不合法的筛选OAD[%d]\n", pOopRsd->sel3.item[i].oad.value);
                    }
                }
            }
            else
            {
                nRet =  ERR_NORMAL;
            }
            nRet = ERR_OK;
        }
        break;

    default:
        nRet = ERR_NORMAL;
        break;
    }

    if (nRet == ERR_OK)
    {
        relyInfo.oopRsd  = pOopRsd;
        relyInfo.oopRcsd = *pOopRcsd;
        relyInfo.bpBreakInfo = pApduInfo->bpBreakInfo;
        *pInReadInf = inReadInfo;
        *pRelyInfo     = relyInfo;
    }

    return nRet;
}


#endif

#if DESC("福建特殊ms解析", 1)
/**
*********************************************************************
* @brief：     判断表地址是否是群组档案
* @param[in]： addr     地址
* @return：    TRUE - 合法
*********************************************************************
*/
BOOL meter_addr_is_group(uint8 *addr)
{
    if (addr == NULL)
    {
        return ERR_PTR;
    }
    
    if ((addr[0] == 0x88) && 
        (addr[1] == 0x88) &&
        (addr[2] == 0x88) &&
        (addr[3] == 0x88))
    {
        return TRUE;
    }

    return FALSE;
}

/**
*********************************************************************
* @brief：     判断表地址是否合法
* @param[in]： addr     地址
* @return：    TRUE - 合法
*********************************************************************
*/
BOOL meter_addr_is_legally(uint64 *addr)
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
* @brief：     获取MS，有具体地址的集合按集合里的地址，全部用户地址只记录标记，其他还是从档案里读取，但
*             福建现场可能没有档案。
* @param[in]： inData     ms集合链表头
               offset     偏移
* @return：    ms集合链表头
*********************************************************************
*/
MS_T* mtr_list_get_fujian(DB_CLIENT hUdp, const uint8 *inData, int InDataLen, uint16* offset, BOOL isWithType)
{
    int32   ret = ERR_NORMAL;
    int		i = 0;
    int 	j = 0;
    uint16	k = 0;
    uint8	Choice = 0;
    MS_T*   pMtrHead = NULL;
    MS_T*   pMtrTail = pMtrHead;
    MS_T*   pNewMtr = pMtrHead;
    OOP_METER_T      oopMeter  = {0};

    if(isWithType)
    {
        if(inData[*offset] != 0x5c)
            return NULL;
        (*offset)++;
    }
    Choice = inData[(*offset)++];
    if(0 == Choice)
    {
        return NULL;
    }
    else
    {
        pNewMtr = pMtrHead;
        pMtrTail = pMtrHead;
        switch (Choice)
        {
        case 1:
            //全部用户地址，只记录标记，让数据中心处理
            return NULL;
            break;

        case 2://一组用户类型
        {
            uint8*	MtrTypeList = NULL;
            uint8	TypeNum = inData[(*offset)++];
            MtrTypeList = (uint8 *)malloc(TypeNum);
            for(j = 0; j < TypeNum; j++)
            {
                MtrTypeList[j] = inData[(*offset)++];
            }
            for(i = 1; i < NO_VIR_MTR_MAX_NUM + 1; i++)
            {
                ret = app_get_meter_list(i, &oopMeter);
                if (ret == ERR_OK)
                {
                    for(j = 0; j < TypeNum; j++)
                    {
                        if(oopMeter.basic.userType == MtrTypeList[j])
                        {
                            pNewMtr = (MS_T *)malloc(sizeof(MS_T));
                            memset(pNewMtr, 0, sizeof(MS_T));
                            pNewMtr->oopMeter = oopMeter;
                            pNewMtr->MtrNo    = i;
                            pNewMtr->next = NULL;
                            if(!pMtrHead)
                                pMtrHead = pNewMtr;
                            if(pMtrTail)
                                pMtrTail->next = pNewMtr;
                            pMtrTail = pNewMtr;
                            break;
                        }
                    }
                }
                else
                {
                    //PRTL_FMT_DEBUG("mtr_list_get db_read_nomal err i = %d\n", i);
                }
            }
            free(MtrTypeList);
            MtrTypeList = NULL;
        }
        break;
        case 3:
            //一组用户地址
        {
            uint8  mtrAddrLen = 0;
            uint8  mtrAddrNum = inData[(*offset)++];
            uint16 pn = 1;
            for(j = 0; j < mtrAddrNum; j++)
            {
                mtrAddrLen = inData[(*offset)++];

                //提取TSA信息
                oopMeter.basic.tsa.af = inData[(*offset)++];
                if (oopMeter.basic.tsa.len + 2 != mtrAddrLen)
                {
                    PRTL_FMT_DEBUG("TSA长度出错, oopMeter.basic.tsa.len[%d]. mtrAddrLen[%d]\n", 
                        oopMeter.basic.tsa.len, mtrAddrLen);
                    continue;
                }
                if (oopMeter.basic.tsa.flag == 0x01)
                {
                    oopMeter.basic.tsa.vxd = inData[(*offset)++];
                    memcpy(&oopMeter.basic.tsa.add[0], &inData[(*offset)], oopMeter.basic.tsa.len);
                    *offset += oopMeter.basic.tsa.len;
                    
                }
                else
                {
                     memcpy(&oopMeter.basic.tsa.add[0], &inData[(*offset)], oopMeter.basic.tsa.len + 1);
                    *offset += oopMeter.basic.tsa.len + 1;
                }

                //判断是否是群组档案
                if (meter_addr_is_group(&oopMeter.basic.tsa.add[0]))
                {
                    NOMAL_OAD_T nomalOad = {0};
                    uint32      len  = 0;
                    OOP_GROUPMETER_NEW_T  data = {0};
                    OOP_GROUPMETER_NEW_T  data1 = {0};
                    
                    nomalOad.infoNum = (uint8)(commfun_BcdToHex(oopMeter.basic.tsa.add[4]) >> 8) * 1000 + 
                                       (uint8)(commfun_BcdToHex(oopMeter.basic.tsa.add[4]) & 0xff) * 100 +
                                       (uint8)(commfun_BcdToHex(oopMeter.basic.tsa.add[5]) >> 8) * 10   + 
                                       (uint8)(commfun_BcdToHex(oopMeter.basic.tsa.add[5]) & 0xff);
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
                    
                    //群组档案
                    for (k = 0; k < data.num; k++)
                    {
                        oopMeter.basic.tsa.len = 6;
                        memcpy(&oopMeter.basic.tsa.add[0], &data.addr[k].add[0], 6);
                        
                        pNewMtr = (MS_T *)malloc(sizeof(MS_T));
                        memset(pNewMtr, 0, sizeof(MS_T));
                        pNewMtr->oopMeter = oopMeter;
                        pNewMtr->MtrNo    = pn ++;
                        pNewMtr->next     = NULL;
                        if(!pMtrHead)
                            pMtrHead = pNewMtr;
                        if(pMtrTail)
                            pMtrTail->next = pNewMtr;
                        pMtrTail = pNewMtr;
                    }

                    if (data1.num > 0)
                    {
                        for (k = 0; k < data1.num; k++)
                        {
                            oopMeter.basic.tsa.len = 6;
                            memcpy(&oopMeter.basic.tsa.add[0], &data1.addr[k].add[0], 6);
                            
                            pNewMtr = (MS_T *)malloc(sizeof(MS_T));
                            memset(pNewMtr, 0, sizeof(MS_T));
                            pNewMtr->oopMeter = oopMeter;
                            pNewMtr->MtrNo    = pn ++;
                            pNewMtr->next     = NULL;
                            if(!pMtrHead)
                                pMtrHead = pNewMtr;
                            if(pMtrTail)
                                pMtrTail->next = pNewMtr;
                            pMtrTail = pNewMtr;
                        }
                    }
                }
                else
                {
                    pNewMtr = (MS_T *)malloc(sizeof(MS_T));
                    memset(pNewMtr, 0, sizeof(MS_T));
                    pNewMtr->oopMeter = oopMeter;
                    pNewMtr->MtrNo    = pn ++;
                    pNewMtr->next     = NULL;
                    if(!pMtrHead)
                        pMtrHead = pNewMtr;
                    if(pMtrTail)
                        pMtrTail->next = pNewMtr;
                    pMtrTail = pNewMtr;
                }
            }
        }
        break;
        case 4:
            //一组用户序号
        {
            uint8	MtrNoCnt = inData[(*offset)++];
            int		No = 1;

            if (MtrNoCnt == 0x81)
            {
                MtrNoCnt = inData[(*offset)++];
            }
            else if (MtrNoCnt == 0x82)
            {
                MtrNoCnt = inData[(*offset)++];
                MtrNoCnt <<= 8;
                MtrNoCnt += inData[(*offset)++];
            }

            for(i = 0; i < MtrNoCnt; i++)
            {
                No  = (uint16)(inData[*offset]<<8) + (uint16)(inData[*offset + 1]);
                ret = app_get_meter_list(No, &oopMeter);
                if (ret == ERR_OK)
                {
                    pNewMtr = (MS_T *)malloc(sizeof(MS_T));
                    memset(pNewMtr, 0, sizeof(MS_T));
                    pNewMtr->oopMeter = oopMeter;
                    pNewMtr->MtrNo    = No;
                    pNewMtr->next = NULL;
                    if(!pMtrHead)
                        pMtrHead = pNewMtr;
                    if(pMtrTail)
                        pMtrTail->next = pNewMtr;
                    pMtrTail = pNewMtr;
                }
                else
                {
                    //PRTL_FMT_DEBUG("mtr_list_get db_read_nomal err i = %d\n", i);
                }
                (*offset) += 2;
            }
        }
        break;
        case 5:
            //一组用户类型区间
        {
            uint8	MtrType[256] = {0};
            uint8	UserType = 0, typeNum = 0;
            typeNum = GetMtrTypeRegion(inData, offset, MtrType);
            for(i = 1; i < NO_VIR_MTR_MAX_NUM + 1; i++)
            {
                ret = app_get_meter_list(i, &oopMeter);
                if (ret == ERR_OK)
                {
                    UserType = oopMeter.basic.userType;
                    for(j = 0; j < typeNum; j++)
                    {
                        if(UserType == MtrType[j])
                        {
                            pNewMtr = (MS_T *)malloc(sizeof(MS_T));
                            memset(pNewMtr, 0, sizeof(MS_T));
                            pNewMtr->oopMeter = oopMeter;
                            pNewMtr->MtrNo    = i;
                            pNewMtr->next = NULL;
                            if(!pMtrHead)
                                pMtrHead = pNewMtr;
                            if(pMtrTail)
                                pMtrTail->next = pNewMtr;
                            pMtrTail = pNewMtr;
                            break;
                        }
                    }
                }
                else
                {
                    //PRTL_FMT_DEBUG("mtr_list_get db_read_nomal err i = %d\n", i);
                }
            }
        }
        break;
        case 6:
            //一组用户地址区间
        {
            OOP_MSP_T ms;
            OOP_DAR_E dar = DATA_SUCCESS;
            uint16    sOffset = (*offset);
            uint16    pn = 1;

            uint64 startAddr = 0;
            uint64 endAddr = 0;
            
            ms.size = inData[sOffset++];
            ms.msra.size = ms.size;

            
            for(i = 0; i < ms.msra.size; i++)
            {
                ms.msra.region[i].nType = inData[sOffset++]; //区间类型
                
                dar = basic_tsa_fromfmt698(inData, InDataLen, (uint32 *)&sOffset, &ms.msra.region[i].start, sizeof(OOP_TSA_T));
                if(dar != DATA_SUCCESS)
                {
                    PRTL_BUF_DEBUG(inData, InDataLen, "basic_tsa_fromfmt698 failed. offset(%d)", sOffset);
                    return NULL;
                }  
                
                dar = basic_tsa_fromfmt698(inData, InDataLen, (uint32 *)&sOffset, &ms.msra.region[i].end, sizeof(OOP_TSA_T));
                if(dar != DATA_SUCCESS)
                {
                    PRTL_BUF_DEBUG(inData, InDataLen, "basic_tsa_fromfmt698 failed. offset(%d)", sOffset);
                    return NULL;
                }

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
                        oopMeter.basic.tsa = ms.msra.region[i].start;
                        if (ms.msra.region[i].start.flag == 1)
                        {
                            memcpy_r(&oopMeter.basic.tsa.add[0], &startAddr, ms.msra.region[i].start.len);
                        }
                        else
                        {
                            memcpy_r(&oopMeter.basic.tsa.add[0], &startAddr, ms.msra.region[i].start.len + 1);
                        }

                        //判断是否是群组档案
                        if (meter_addr_is_group(&oopMeter.basic.tsa.add[0]))
                        {
                            NOMAL_OAD_T nomalOad = {0};
                            uint32      len  = 0;
                            OOP_GROUPMETER_NEW_T  data = {0};
                            OOP_GROUPMETER_NEW_T  data1 = {0};
                            
                            nomalOad.infoNum = (uint8)(commfun_BcdToHex(oopMeter.basic.tsa.add[4]) >> 8) * 1000 + 
                                               (uint8)(commfun_BcdToHex(oopMeter.basic.tsa.add[4]) & 0xff) * 100 +
                                               (uint8)(commfun_BcdToHex(oopMeter.basic.tsa.add[5]) >> 8) * 10   + 
                                               (uint8)(commfun_BcdToHex(oopMeter.basic.tsa.add[5]) & 0xff);
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
                            
                            //群组档案
                            for (k = 0; k < data.num; k++)
                            {
                                oopMeter.basic.tsa.len = 6;
                                memcpy(&oopMeter.basic.tsa.add[0], &data.addr[k].add[0], 6);
                                
                                pNewMtr = (MS_T *)malloc(sizeof(MS_T));
                                memset(pNewMtr, 0, sizeof(MS_T));
                                pNewMtr->oopMeter = oopMeter;
                                pNewMtr->MtrNo    = pn ++;
                                pNewMtr->next     = NULL;
                                if(!pMtrHead)
                                    pMtrHead = pNewMtr;
                                if(pMtrTail)
                                    pMtrTail->next = pNewMtr;
                                pMtrTail = pNewMtr;
                            }
                        
                            if (data1.num > 0)
                            {
                                for (k = 0; k < data1.num; k++)
                                {
                                    oopMeter.basic.tsa.len = 6;
                                    memcpy(&oopMeter.basic.tsa.add[0], &data1.addr[k].add[0], 6);
                                    
                                    pNewMtr = (MS_T *)malloc(sizeof(MS_T));
                                    memset(pNewMtr, 0, sizeof(MS_T));
                                    pNewMtr->oopMeter = oopMeter;
                                    pNewMtr->MtrNo    = pn ++;
                                    pNewMtr->next     = NULL;
                                    if(!pMtrHead)
                                        pMtrHead = pNewMtr;
                                    if(pMtrTail)
                                        pMtrTail->next = pNewMtr;
                                    pMtrTail = pNewMtr;
                                }
                            }
                        }
                        else
                        {
                            pNewMtr = (MS_T *)malloc(sizeof(MS_T));
                            memset(pNewMtr, 0, sizeof(MS_T));
                            pNewMtr->oopMeter = oopMeter;
                            pNewMtr->MtrNo    = pn ++;
                            pNewMtr->next     = NULL;
                            if(!pMtrHead)
                                pMtrHead = pNewMtr;
                            if(pMtrTail)
                                pMtrTail->next = pNewMtr;
                            pMtrTail = pNewMtr;
                        }

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
                        if (!meter_addr_is_legally(&startAddr))
                        {
                            continue;
                        }
                        oopMeter.basic.tsa = ms.msra.region[i].start;
                        if (ms.msra.region[i].start.flag == 1)
                        {
                            memcpy_r(&oopMeter.basic.tsa.add[0], &startAddr, ms.msra.region[i].start.len);
                        }
                        else
                        {
                            memcpy_r(&oopMeter.basic.tsa.add[0], &startAddr, ms.msra.region[i].start.len + 1);
                        }

                        //判断是否是群组档案
                        if (meter_addr_is_group(&oopMeter.basic.tsa.add[0]))
                        {
                            NOMAL_OAD_T nomalOad = {0};
                            uint32      len  = 0;
                            OOP_GROUPMETER_NEW_T  data = {0};
                            OOP_GROUPMETER_NEW_T  data1 = {0};
                            
                            nomalOad.infoNum = (uint8)(commfun_BcdToHex(oopMeter.basic.tsa.add[4]) >> 8) * 1000 + 
                                               (uint8)(commfun_BcdToHex(oopMeter.basic.tsa.add[4]) & 0xff) * 100 +
                                               (uint8)(commfun_BcdToHex(oopMeter.basic.tsa.add[5]) >> 8) * 10   + 
                                               (uint8)(commfun_BcdToHex(oopMeter.basic.tsa.add[5]) & 0xff);
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
                            
                            //群组档案
                            for (k = 0; k < data.num; k++)
                            {
                                oopMeter.basic.tsa.len = 6;
                                memcpy(&oopMeter.basic.tsa.add[0], &data.addr[k].add[0], 6);
                                
                                pNewMtr = (MS_T *)malloc(sizeof(MS_T));
                                memset(pNewMtr, 0, sizeof(MS_T));
                                pNewMtr->oopMeter = oopMeter;
                                pNewMtr->MtrNo    = pn ++;
                                pNewMtr->next     = NULL;
                                if(!pMtrHead)
                                    pMtrHead = pNewMtr;
                                if(pMtrTail)
                                    pMtrTail->next = pNewMtr;
                                pMtrTail = pNewMtr;
                            }
                        
                            if (data1.num > 0)
                            {
                                for (k = 0; k < data1.num; k++)
                                {
                                    oopMeter.basic.tsa.len = 6;
                                    memcpy(&oopMeter.basic.tsa.add[0], &data1.addr[k].add[0], 6);
                                    
                                    pNewMtr = (MS_T *)malloc(sizeof(MS_T));
                                    memset(pNewMtr, 0, sizeof(MS_T));
                                    pNewMtr->oopMeter = oopMeter;
                                    pNewMtr->MtrNo    = pn ++;
                                    pNewMtr->next     = NULL;
                                    if(!pMtrHead)
                                        pMtrHead = pNewMtr;
                                    if(pMtrTail)
                                        pMtrTail->next = pNewMtr;
                                    pMtrTail = pNewMtr;
                                }
                            }
                        }
                        else
                        {
                            pNewMtr = (MS_T *)malloc(sizeof(MS_T));
                            memset(pNewMtr, 0, sizeof(MS_T));
                            pNewMtr->oopMeter = oopMeter;
                            pNewMtr->MtrNo    = pn ++;
                            pNewMtr->next     = NULL;
                            if(!pMtrHead)
                                pMtrHead = pNewMtr;
                            if(pMtrTail)
                                pMtrTail->next = pNewMtr;
                            pMtrTail = pNewMtr;
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
                        oopMeter.basic.tsa = ms.msra.region[i].start;
                        if (ms.msra.region[i].start.flag == 1)
                        {
                            memcpy_r(&oopMeter.basic.tsa.add[0], &startAddr, ms.msra.region[i].start.len);
                        }
                        else
                        {
                            memcpy_r(&oopMeter.basic.tsa.add[0], &startAddr, ms.msra.region[i].start.len + 1);
                        }

                        //判断是否是群组档案
                        if (meter_addr_is_group(&oopMeter.basic.tsa.add[0]))
                        {
                            NOMAL_OAD_T nomalOad = {0};
                            uint32      len  = 0;
                            OOP_GROUPMETER_NEW_T  data = {0};
                            OOP_GROUPMETER_NEW_T  data1 = {0};
                            
                            nomalOad.infoNum = (uint8)(commfun_BcdToHex(oopMeter.basic.tsa.add[4]) >> 8) * 1000 + 
                                               (uint8)(commfun_BcdToHex(oopMeter.basic.tsa.add[4]) & 0xff) * 100 +
                                               (uint8)(commfun_BcdToHex(oopMeter.basic.tsa.add[5]) >> 8) * 10   + 
                                               (uint8)(commfun_BcdToHex(oopMeter.basic.tsa.add[5]) & 0xff);
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
                            
                            //群组档案
                            for (k = 0; k < data.num; k++)
                            {
                                oopMeter.basic.tsa.len = 6;
                                memcpy(&oopMeter.basic.tsa.add[0], &data.addr[k].add[0], 6);
                                
                                pNewMtr = (MS_T *)malloc(sizeof(MS_T));
                                memset(pNewMtr, 0, sizeof(MS_T));
                                pNewMtr->oopMeter = oopMeter;
                                pNewMtr->MtrNo    = pn ++;
                                pNewMtr->next     = NULL;
                                if(!pMtrHead)
                                    pMtrHead = pNewMtr;
                                if(pMtrTail)
                                    pMtrTail->next = pNewMtr;
                                pMtrTail = pNewMtr;
                            }
                        
                            if (data1.num > 0)
                            {
                                for (k = 0; k < data1.num; k++)
                                {
                                    oopMeter.basic.tsa.len = 6;
                                    memcpy(&oopMeter.basic.tsa.add[0], &data1.addr[k].add[0], 6);
                                    
                                    pNewMtr = (MS_T *)malloc(sizeof(MS_T));
                                    memset(pNewMtr, 0, sizeof(MS_T));
                                    pNewMtr->oopMeter = oopMeter;
                                    pNewMtr->MtrNo    = pn ++;
                                    pNewMtr->next     = NULL;
                                    if(!pMtrHead)
                                        pMtrHead = pNewMtr;
                                    if(pMtrTail)
                                        pMtrTail->next = pNewMtr;
                                    pMtrTail = pNewMtr;
                                }
                            }
                        }
                        else
                        {
                            pNewMtr = (MS_T *)malloc(sizeof(MS_T));
                            memset(pNewMtr, 0, sizeof(MS_T));
                            pNewMtr->oopMeter = oopMeter;
                            pNewMtr->MtrNo    = pn ++;
                            pNewMtr->next     = NULL;
                            if(!pMtrHead)
                                pMtrHead = pNewMtr;
                            if(pMtrTail)
                                pMtrTail->next = pNewMtr;
                            pMtrTail = pNewMtr;
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
                        oopMeter.basic.tsa = ms.msra.region[i].start;
                        if (ms.msra.region[i].start.flag == 1)
                        {
                            memcpy_r(&oopMeter.basic.tsa.add[0], &startAddr, ms.msra.region[i].start.len);
                        }
                        else
                        {
                            memcpy_r(&oopMeter.basic.tsa.add[0], &startAddr, ms.msra.region[i].start.len + 1);
                        }

                        //判断是否是群组档案
                        if (meter_addr_is_group(&oopMeter.basic.tsa.add[0]))
                        {
                            NOMAL_OAD_T nomalOad = {0};
                            uint32      len  = 0;
                            OOP_GROUPMETER_NEW_T  data = {0};
                            OOP_GROUPMETER_NEW_T  data1 = {0};
                            
                            nomalOad.infoNum = (uint8)(commfun_BcdToHex(oopMeter.basic.tsa.add[4]) >> 8) * 1000 + 
                                               (uint8)(commfun_BcdToHex(oopMeter.basic.tsa.add[4]) & 0xff) * 100 +
                                               (uint8)(commfun_BcdToHex(oopMeter.basic.tsa.add[5]) >> 8) * 10   + 
                                               (uint8)(commfun_BcdToHex(oopMeter.basic.tsa.add[5]) & 0xff);
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
                            
                            //群组档案
                            for (k = 0; k < data.num; k++)
                            {
                                oopMeter.basic.tsa.len = 6;
                                memcpy(&oopMeter.basic.tsa.add[0], &data.addr[k].add[0], 6);
                                
                                pNewMtr = (MS_T *)malloc(sizeof(MS_T));
                                memset(pNewMtr, 0, sizeof(MS_T));
                                pNewMtr->oopMeter = oopMeter;
                                pNewMtr->MtrNo    = pn ++;
                                pNewMtr->next     = NULL;
                                if(!pMtrHead)
                                    pMtrHead = pNewMtr;
                                if(pMtrTail)
                                    pMtrTail->next = pNewMtr;
                                pMtrTail = pNewMtr;
                            }
                        
                            if (data1.num > 0)
                            {
                                for (k = 0; k < data1.num; k++)
                                {
                                    oopMeter.basic.tsa.len = 6;
                                    memcpy(&oopMeter.basic.tsa.add[0], &data1.addr[k].add[0], 6);
                                    
                                    pNewMtr = (MS_T *)malloc(sizeof(MS_T));
                                    memset(pNewMtr, 0, sizeof(MS_T));
                                    pNewMtr->oopMeter = oopMeter;
                                    pNewMtr->MtrNo    = pn ++;
                                    pNewMtr->next     = NULL;
                                    if(!pMtrHead)
                                        pMtrHead = pNewMtr;
                                    if(pMtrTail)
                                        pMtrTail->next = pNewMtr;
                                    pMtrTail = pNewMtr;
                                }
                            }
                        }
                        else
                        {
                            pNewMtr = (MS_T *)malloc(sizeof(MS_T));
                            memset(pNewMtr, 0, sizeof(MS_T));
                            pNewMtr->oopMeter = oopMeter;
                            pNewMtr->MtrNo    = pn ++;
                            pNewMtr->next     = NULL;
                            if(!pMtrHead)
                                pMtrHead = pNewMtr;
                            if(pMtrTail)
                                pMtrTail->next = pNewMtr;
                            pMtrTail = pNewMtr;
                        }
                    }
                    break;
                }
                default:
                    break;
                }
            }
            (*offset) = sOffset;
        }
        break;
        case 7:
            //一组序号区间
        {
            uint16 StartNo = 1, EndNo = 1;
            uint16 MetNum = 0;
            uint16 *Pn = malloc(2*NO_VIR_MTR_MAX_NUM);
            uint8  RegionNum = inData[(*offset)++];
            uint8  RegionType = 0;
            for(i = 0; i < RegionNum; i++)
            {
                RegionType = inData[(*offset)++];
                if(inData[(*offset)++] != LONG_UNSIGNED)
                {
                    free(Pn);
                    Pn = NULL;
                    return NULL;
                }
                memcpy_r(&StartNo, &inData[*offset], sizeof(uint16));
                (*offset) += 2;
                if(inData[(*offset)++] != LONG_UNSIGNED)
                {
                    free(Pn);
                    Pn = NULL;
                    return NULL;
                }
                memcpy_r(&EndNo, &inData[*offset], sizeof(uint16));
                (*offset) += 2;
                if(RegionType == 0 || RegionType == 3)
                    EndNo--;
                if(RegionType == 1 || RegionType == 3)
                    StartNo++;
                MergeMetSnRegion(Pn, &MetNum, StartNo, EndNo);
            }

            for(k = 0; k < MetNum; k++)
            {
                if(Pn[k] >=10000)
                {
                    
                    ret = GetVirtualPipePn(Pn[k], &oopMeter);
                    PRTL_FMT_DEBUG("感知终端虚拟通道 Pn[k] = %d,ret is %d\n", Pn[k],ret);
                }
                else
                {
                    ret = app_get_meter_list(Pn[k], &oopMeter);
                }
                if (ret == ERR_OK)
                {
                    pNewMtr = (MS_T *)malloc(sizeof(MS_T));
                    memset(pNewMtr, 0, sizeof(MS_T));
                    pNewMtr->oopMeter = oopMeter;
                    pNewMtr->MtrNo    = Pn[k];
                    pNewMtr->next = NULL;
                    if(!pMtrHead)
                        pMtrHead = pNewMtr;
                    if(pMtrTail)
                        pMtrTail->next = pNewMtr;
                    pMtrTail = pNewMtr;
                }
                else
                {
                    //PRTL_FMT_DEBUG("mtr_list_get db_read_nomal err i = %d\n", i);
                }
            }
            free(Pn);
            Pn = NULL;
        }
        break;
        //一组设备类型
        case 8:
        {
            uint16*	MtrTypeList = NULL;
            uint8	TypeNum = inData[(*offset)++];
            uint16  type = 0;
            MtrTypeList = (uint16 *)malloc(2*TypeNum);
            for (j = 0; j < TypeNum; j++)
            {
                MtrTypeList[j] = (uint16)(inData[*offset]<<8) + (uint16)(inData[*offset + 1]);
                (*offset) += 2;
            }
            for(i = 1; i < NO_VIR_MTR_MAX_NUM; i++)
            {
                ret = app_get_meter_list(i, &oopMeter);
                if (ret == ERR_OK)
                {
                    for(j = 0; j < TypeNum; j++)
                    {
                        if (oopMeter.annex.nNum == 0)
                        {
                            break;
                        }
                        for (k = 0; k < oopMeter.annex.nNum; k++)
                        {
                            if (oopMeter.annex.annexObj[k].nLen != sizeof(uint16) || oopMeter.annex.annexObj[k].oad.value != 0x6E010200)
                            {
                                continue;
                            }
                            memcpy(&type, oopMeter.annex.annexObj[k].value, sizeof(uint16));
                            if (type == MtrTypeList[j])
                            {
                                pNewMtr = (MS_T *)malloc(sizeof(MS_T));
                                memset(pNewMtr, 0, sizeof(MS_T));
                                pNewMtr->oopMeter = oopMeter;
                                pNewMtr->MtrNo    = i;
                                pNewMtr->next = NULL;
                                if(!pMtrHead)
                                    pMtrHead = pNewMtr;
                                if(pMtrTail)
                                    pMtrTail->next = pNewMtr;
                                pMtrTail = pNewMtr;
                                break;
                            }
                        }
                    }
                }
                else
                {
                    //PRTL_FMT_DEBUG("mtr_list_get db_read_nomal err i = %d\n", i);
                }
            }
            free(MtrTypeList);
            MtrTypeList = NULL;
        }
        break;
        default:
            break;
        }
    }
    return pMtrHead;
}

/**
*********************************************************************
* @name：        basic_ms_fromfmt698
* @brief：       基本类型数据转换为698报文格式
* @param[in] ： srcType    源数据类型
*               srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer

* @return    ：无
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E apdu_ms_fromfmt698_fj(const void *srcBuf, uint32 srcLen, uint32 *pOffset, void *dstData, uint32 dstLen)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32 offset = *pOffset;    
    uint8 *srcBufTmp = (uint8*)srcBuf;
    OOP_MSPF_T ms;
    uint32 i = 0;    
    uint8 offlen = 0;      //数组数量长度
    uint32 num = 0;

    /*MS∷=CHOICE
     { 
         无表计              [0]，
         全部用户地址           [1]，
         一组用户类型           [2] SEQUENCE OF unsigned，
         一组用户地址           [3] SEQUENCE OF TSA，
         一组配置序号           [4] SEQUENCE OF long-unsigned，
         一组用户类型区间 [5] SEQUENCE OF Region，
         一组用户地址区间 [6] SEQUENCE OF Region，
         一组配置序号区间 [7] SEQUENCE OF Region
     }*/
     
    MEMZERO(&ms, sizeof(ms));

    ms.choice = srcBufTmp[offset++];

    switch(ms.choice)
    {
        case 0: break;//无表计 
        case 1: break;//所有电能表
        
        case 2:       //一组用户类型
        {
            ms.size = srcBufTmp[offset++];
            ms.mst.size = ms.size;
            for(i = 0; i < ms.msi.size; i++)
            {
                memcpy(&ms.mst.type[i], &srcBufTmp[offset], sizeof(uint8));    
                offset += sizeof(uint8);
            }
        }break;

        case 3:       //一组用户地址
        {
            ms.size = srcBufTmp[offset++];
            if (ms.size == 0x81)
            {
                ms.size = srcBufTmp[offset++];
            }
            else if (ms.size == 0x82)
            {
                ms.size = srcBufTmp[offset++];
                ms.size <<= 8;
                ms.size += srcBufTmp[offset++];
            }
            else if (ms.size > 0x82 || ms.size == 0x80)
            {
                return DATA_TYPE_UNMATCHED;
            }

            ms.msa.size = ms.size;
            for(i = 0; i < ms.msa.size; i++)
            {
                dar = apdu_tsa_fromfmt698(srcBuf, srcLen, &offset, &ms.msa.add[i], sizeof(OOP_TSA_T));
                if(dar != DATA_SUCCESS)
                {
                    PRTL_BUF_DEBUG(srcBufTmp, srcLen, "apdu_tsa_fromfmt698 failed. offset(%d)", offset);
                    return DATA_TYPE_UNMATCHED;           
                }                     
            }
        }break;   

        case 4:       //一组配置序号
        {
            //数组成员个数
            num = get_len_offset((void*)&srcBufTmp[offset], &offlen);
            offset += offlen;
            ms.size = num;
            ms.msi.size = ms.size;
            for(i = 0; i < ms.msi.size; i++)
            {
               memcpy_r(&ms.msi.idx[i], &srcBufTmp[offset], sizeof(uint16));
               offset += sizeof(uint16);               
            }

        }break;    

        case 5:       //一组用户类型区间
        {
            ms.size = srcBufTmp[offset++];
            ms.msrt.size = ms.size;
            for(i = 0; i < ms.msrt.size; i++)
            {
                ms.msrt.region[i].nType = srcBufTmp[offset++]; //区间类型

                dar = basic_buf_to_data(E_UNSIGNED,srcBuf, srcLen, &offset, &ms.msrt.region[i].start, sizeof(uint8));
                if(dar != DATA_SUCCESS)
                {
                    PRTL_BUF_DEBUG(srcBufTmp, srcLen, "basic_buf_to_data failed. offset(%d)", offset);
                    return DATA_TYPE_UNMATCHED;           
                }  

                dar = basic_buf_to_data(E_UNSIGNED, srcBuf, srcLen, &offset, &ms.msrt.region[i].end, sizeof(uint8));
                if(dar != DATA_SUCCESS)
                {
                    PRTL_BUF_DEBUG(srcBufTmp, srcLen, "basic_buf_to_data failed. offset(%d)", offset);
                    return DATA_TYPE_UNMATCHED;           
                }                
            }
        }break; 

        case 6:       //一组用户地址区间
        {
            ms.size = srcBufTmp[offset++];
            ms.msra.size = ms.size;
            for(i = 0; i < ms.msra.size; i++)
            {
                ms.msra.region[i].nType = srcBufTmp[offset++]; //区间类型
                
                dar = basic_tsa_fromfmt698(srcBuf, srcLen, &offset, &ms.msra.region[i].start, sizeof(OOP_TSA_T));
                if(dar != DATA_SUCCESS)
                {
                    PRTL_BUF_DEBUG(srcBufTmp, srcLen, "basic_tsa_fromfmt698 failed. offset(%d)", offset);
                    return DATA_TYPE_UNMATCHED;           
                }  
                
                dar = basic_tsa_fromfmt698(srcBuf, srcLen, &offset, &ms.msra.region[i].end, sizeof(OOP_TSA_T));
                if(dar != DATA_SUCCESS)
                {
                    PRTL_BUF_DEBUG(srcBufTmp, srcLen, "basic_tsa_fromfmt698 failed. offset(%d)", offset);
                    return DATA_TYPE_UNMATCHED;           
                }
            }
        }break;   

        case 7:       //一组配置序号区间
        {
            ms.size = srcBufTmp[offset++];
            ms.msri.size = ms.size;
            for(i = 0; i < ms.msra.size; i++)
            {
                ms.msri.region[i].nType = srcBufTmp[offset++]; //区间类型
                
                dar = basic_buf_to_data(E_LONG_UNSIGNED,srcBuf, srcLen, &offset, &ms.msri.region[i].start, sizeof(uint16));
                if(dar != DATA_SUCCESS)
                {
                    PRTL_BUF_DEBUG(srcBufTmp, srcLen, "basic_buf_to_data failed. offset(%d)", offset);
                    return DATA_TYPE_UNMATCHED;           
                }  
                
                dar = basic_buf_to_data(E_LONG_UNSIGNED, srcBuf, srcLen, &offset, &ms.msri.region[i].end, sizeof(uint16));
                if(dar != DATA_SUCCESS)
                {
                    PRTL_BUF_DEBUG(srcBufTmp, srcLen, "basic_buf_to_data failed. offset(%d)", offset);
                    return DATA_TYPE_UNMATCHED;           
                }
            }
        }break;          
#if AREA_ZHEJIANG
        case 8:       //一组配置序号
        {
            ms.size = srcBufTmp[offset++];
            ms.mdt.size = ms.size;
            for(i = 0; i < ms.mdt.size; i++)
            {
               memcpy_r(&ms.mdt.idx[i], &srcBufTmp[offset], sizeof(uint16));
               offset += sizeof(uint16);               
            }

        }break;    
#endif
        
    }

    memcpy(dstData, &ms, sizeof(ms));

    *pOffset = offset;

    return dar;     

}

/**
*********************************************************************
* @name：        basic_ms_fromfmt698_fj
* @brief：       基本类型数据转换为698报文格式
* @param[in] ： srcType    源数据类型
*               srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer

* @return    ：无
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E basic_ms_fromfmt698_fj(const void *srcBuf, uint32 srcLen, uint32 *pOffset, void *dstData, uint32 dstLen)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32 offset = *pOffset;    
    uint8 *srcBufTmp = (uint8*)srcBuf;
    OOP_MSPF_T ms;
     
    MEMZERO(&ms, sizeof(ms));

    //类型
    if(srcBufTmp[offset++] != DT_MS)
    {
        PRTL_BUF_DEBUG(srcBufTmp, srcLen, "check csd type failed. offset(%d)", offset);
        return DATA_TYPE_UNMATCHED;
    }
    
    dar = apdu_ms_fromfmt698_fj(srcBuf, srcLen, &offset, &ms, sizeof(ms));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBufTmp, srcLen, "apdu_csd_fromfmt698 failed. dar(%d), offset(%d)", dar, offset);
        return DATA_TYPE_UNMATCHED;           
    }  
 
    memcpy(dstData, &ms, sizeof(ms));

    *pOffset = offset;

    return dar;
}

/**
*********************************************************************
* @name：        basic_ms_fromfmt698
* @brief：       基本类型数据转换为698报文格式
* @param[in] ： srcType    源数据类型
*               srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer

* @return    ：无
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E apdu_msp_tofmt698_fj(void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32 offset = *pOffset;    
    uint8 *srcDataTmp = (uint8*)srcData;
    uint8 *dstBufTmp = (uint8*)dstBuf;
    OOP_MSPF_T ms;
    uint32 i = 0;    
    uint8 offlen = 0;      //数组数量长度
    uint8 lenBuf[3] = {0};

    /*MS∷=CHOICE
     { 
         无表计              [0]，
         全部用户地址           [1]，
         一组用户类型           [2] SEQUENCE OF unsigned，
         一组用户地址           [3] SEQUENCE OF TSA，
         一组配置序号           [4] SEQUENCE OF long-unsigned，
         一组用户类型区间 [5] SEQUENCE OF Region，
         一组用户地址区间 [6] SEQUENCE OF Region，
         一组配置序号区间 [7] SEQUENCE OF Region
     }*/
     
    memcpy(&ms, srcDataTmp, sizeof(ms));

    dstBufTmp[offset++] = ms.choice;

    switch(ms.choice)
    {
        case 0: break; //无表计
        case 1: break; //全部可采集的表计  
        case 2:        //一组用户类型 
        {
            dstBufTmp[offset++] = (uint8)ms.mst.size; //SEQUENCE个数
            
            memcpy(&dstBufTmp[offset], ms.mst.type, ms.mst.size);
            offset += ms.mst.size;
        }break;
        case 3:        //一组用户地址 
        {
            if (ms.msa.size >= 0x80)
            {
                dstBufTmp[offset++] = 0x82;
                dstBufTmp[offset++] = (uint8)(ms.msa.size >> 8);
                dstBufTmp[offset++] = (uint8)ms.msa.size;
            }
            else
            {
                dstBufTmp[offset++] = (uint8)ms.msa.size; //SEQUENCE个数
            }
            
            for(i = 0; i < ms.msa.size; i++)
            {
                dar = apdu_tsa_tofmt698(dstBuf, dstLen, &offset, &ms.msa.add[i], sizeof(OOP_TSA_T));
                if(dar != DATA_SUCCESS)
                {
                    PRTL_FMT_DEBUG("apdu_tsa_tofmt698 failed. offset(%d)", offset);
                    return DATA_TYPE_UNMATCHED;           
                } 
            }
        }break;
        case 4:        //一组配置序号
        {
            offlen = set_len_offset(ms.msi.size, lenBuf);
            memcpy(&dstBufTmp[offset], lenBuf, offlen);//SEQUENCE个数
            offset += offlen;

            for(i = 0; i < ms.msi.size; i++)
            {
                memcpy_r(&dstBufTmp[offset], (uint8*)&ms.msi.idx[i], sizeof(uint16));
                offset += sizeof(uint16);
            }
        }break;
        case 5:        //一组用户类型区间 
        {
            dstBufTmp[offset++] = (uint8)ms.msrt.size; //SEQUENCE个数
            
            for(i = 0; i < ms.msrt.size; i++)
            {
                dstBufTmp[offset++] = ms.msrt.region[i].nType; //区间类型
                dstBufTmp[offset++] = DT_UNSIGNED;             //Data choice
                dstBufTmp[offset++] = ms.msrt.region[i].start; //起始值
                dstBufTmp[offset++] = DT_UNSIGNED;             //Data choice
                dstBufTmp[offset++] = ms.msrt.region[i].end;   //结束值             
            }
        }break;   
        case 6:        //一组用户地址区间 
        {
            dstBufTmp[offset++] = (uint8)ms.msra.size; //SEQUENCE个数
            
            for(i = 0; i < ms.msra.size; i++)
            {
                dstBufTmp[offset++] = ms.msra.region[i].nType;   //区间类型
                dar = basic_tsa_tofmt698(dstBufTmp, dstLen, &offset, &ms.msra.region[i].start, sizeof(OOP_TSA_T));
                if(dar != DATA_SUCCESS)
                {
                    PRTL_FMT_DEBUG("basic_tsa_tofmt698 failed. offset(%d)", offset);
                    return DATA_TYPE_UNMATCHED;           
                }

                dar = basic_tsa_tofmt698(dstBufTmp, dstLen, &offset, &ms.msra.region[i].end, sizeof(OOP_TSA_T));
                if(dar != DATA_SUCCESS)
                {
                    PRTL_FMT_DEBUG("basic_tsa_tofmt698 failed. offset(%d)", offset);
                    return DATA_TYPE_UNMATCHED;           
                }
            }
        }break;  
        case 7:        //一组配置序号区间 
        {
            dstBufTmp[offset++] = (uint8)ms.msri.size; //SEQUENCE个数
            
            for(i = 0; i < ms.msri.size; i++)
            {
                dstBufTmp[offset++] = ms.msri.region[i].nType;   //区间类型

                dstBufTmp[offset++] = DT_LONG_UNSIGNED;               //Data choice

                memcpy_r(&dstBufTmp[offset], (uint8*)&ms.msri.region[i].start, sizeof(uint16));//起始值
                offset += sizeof(uint16); 

                dstBufTmp[offset++] = DT_LONG_UNSIGNED;               //Data choice

                memcpy_r(&dstBufTmp[offset], (uint8*)&ms.msri.region[i].end, sizeof(uint16));//结束值
                offset += sizeof(uint16); 
            }
        }break; 
#if AREA_ZHEJIANG
        case 8:        //一组配置序号
        {
            dstBufTmp[offset++] = (uint8)ms.mdt.size; //SEQUENCE个数

            for(i = 0; i < ms.mdt.size; i++)
            {
                memcpy_r(&dstBufTmp[offset], (uint8*)&ms.mdt.idx[i], sizeof(uint16));
                offset += sizeof(uint16);
            }
        }break;
#endif
        default:break;
    }

    *pOffset = offset;

    return dar;     
}


/**
*********************************************************************
* @name：        basic_ms_tofmt698
* @brief：       基本类型数据转换为698报文格式
* @param[in] ： srcType    源数据类型
*               srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer

* @return    ：无
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E basic_ms_tofmt698_fj(void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32 offset = *pOffset;    
    uint8 *dstBufTmp = (uint8*)dstBuf;
    OOP_MSPF_T ms;  
     
    memcpy(&ms, srcData, srcLen);

    //MS类型
    dstBufTmp[offset++] = DT_MS;
    
    dar = apdu_msp_tofmt698_fj(dstBuf, dstLen, &offset, srcData, srcLen);
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(dstBuf, dstLen, "apdu_csd_tofmt698 failed. dar(%d), offset(%d)", dar, offset);
        return DATA_TYPE_UNMATCHED;           
    } 

    *pOffset = offset;

    return dar;
}

#endif


#if DESC("福建特殊方法解析", 1) 
/**
*********************************************************************
* @brief：     获取RSD
* @param[in]： inData      原始报文数据
* @param[in]： InDataLen   报文长度
* @param[out]：pRSD        筛选方法
* @return：    offset      解完后偏移
*********************************************************************
*/
uint16 rsd_get_fujian(DB_CLIENT hUdp, const uint8* inData, int InDataLen, RSD_T* pRSD)
{
    uint16 tempLen = InDataLen;
    uint16 offset = 0;
    uint16 i = 0;
    uint16 nodeidNum = 0;
    pRSD->selId = inData[offset++];

    switch (pRSD->selId)
    {
    case 50:
        pRSD->LastN = (((uint16)inData[offset] << 8) + (uint16)(inData[offset + 1]));
        offset += 2;
        pRSD->recNum = inData[offset++];
        return offset;
        break;
    case 51:
        pRSD->transTaskid = ((uint16)(inData[offset] << 8) + (uint16)(inData[offset + 1]));
        offset += 2;
        tempLen -= 2;
        pRSD->StartData = (uint8*)malloc(7);
        pRSD->EndData = (uint8*)malloc(7);
        if (tempLen < 7)
        {
            PRTL_FMT_DEBUG("RSD解析错误，格式错误\n");
            return 0;
        }
        else
        {
            memcpy(pRSD->StartData, &inData[offset], 7);
            memcpy(pRSD->EndData, &inData[offset], 7);
            offset += 7;
        }
        pRSD->Ti.unit = 0xff;
        pRSD->Ti.value = 0;

        //是否筛选所有地址
        if (inData[offset] == 0x01)
        {
            pRSD->isAllmeter = 0x01;
            offset ++;
        }
        else
        {
            pRSD->Ms = mtr_list_get_fujian(hUdp, inData, InDataLen, &offset, FALSE);
            if (!pRSD->Ms)
            {
                PRTL_FMT_DEBUG("RSD解析错误，找不到MS集合\n");
                return 0;
            }
        }
        return offset;
        break;
    case 52:
        pRSD->transTaskid = ((uint16)(inData[offset] << 8) + (uint16)(inData[offset + 1]));
        offset += 2;
        tempLen -= 2;
        nodeidNum = inData[offset++];
        tempLen --;
        if (nodeidNum == 0x81)
        {
            nodeidNum = inData[offset++];
            tempLen --;
        }
        else if (nodeidNum == 0x82)
        {
            nodeidNum = inData[offset++];
            nodeidNum <<= 8;
            nodeidNum += inData[offset++];
            tempLen -= 2;
        }
        pRSD->nodeidNum = nodeidNum;
        if (nodeidNum > NODE_ID_NUM_MAX)
        {
            PRTL_FMT_DEBUG("RSD解析错误，数量过大\n");
            return 0;
        }
        for (i = 0; i < nodeidNum; i++)
        {
            pRSD->nodeid[i] = ((uint16)(inData[offset] << 8) + (uint16)(inData[offset + 1]));
            offset += 2;
            tempLen -= 2;
        }
        pRSD->StartData = (uint8*)malloc(7);
        pRSD->EndData = (uint8*)malloc(7);
        if (tempLen < 7)
        {
            PRTL_FMT_DEBUG("RSD解析错误，格式错误\n");
            return 0;
        }
        else
        {
            memcpy(pRSD->StartData, &inData[offset], 7);
            memcpy(pRSD->EndData, &inData[offset], 7);
            offset += 7;
        }
        pRSD->Ti.unit = 0xff;
        pRSD->Ti.value = 0;
        return offset;
        break;
    case 53:
        pRSD->transTaskid = ((uint16)(inData[offset] << 8) + (uint16)(inData[offset + 1]));
        offset += 2;
        tempLen -= 2;
        pRSD->StartData = (uint8*)malloc(7);
        pRSD->EndData = (uint8*)malloc(7);
        if (tempLen < 7)
        {
            PRTL_FMT_DEBUG("RSD解析错误，格式错误\n");
            return 0;
        }
        else
        {
            memcpy(pRSD->StartData, &inData[offset], 7);
            offset += 7;
            tempLen -= 7;
        }

        if (tempLen < 7)
        {
            PRTL_FMT_DEBUG("RSD解析错误，格式错误\n");
            return 0;
        }
        else
        {
            memcpy(pRSD->EndData, &inData[offset], 7);
            offset += 7;
            tempLen -= 7;
        }

        pRSD->Ti.unit = 0xff;
        pRSD->Ti.value = 0;

        if (tempLen <= 0)
        {
            PRTL_FMT_DEBUG("RSD解析错误，格式错误\n");
            return 0;
        }
        else
        {
            //是否筛选所有地址
            if (inData[offset] == 0x01)
            {
                pRSD->isAllmeter = 0x01;
                offset ++;
            }
            else
            {
                pRSD->Ms = mtr_list_get_fujian(hUdp, inData, InDataLen, &offset, FALSE);
                if (!pRSD->Ms)
                {
                    PRTL_FMT_DEBUG("RSD解析错误，找不到MS集合\n");
                    return 0;
                }
                else
                {
                    return offset;
                }
            }
        }
        return offset;
        break;
    case 54:
        pRSD->transTaskid = ((uint16)(inData[offset] << 8) + (uint16)(inData[offset + 1]));
        offset += 2;
        tempLen -= 2;
        pRSD->LastN = (((uint16)inData[offset] << 8) + (uint16)(inData[offset + 1]));
        offset += 2;
        tempLen -= 2;
        pRSD->recNum = inData[offset++];
        tempLen--;
        if (tempLen <= 0)
        {
            PRTL_FMT_DEBUG("RSD解析错误，格式错误\n");
            return 0;
        }
        else
        {
            //是否筛选所有地址
            if (inData[offset] == 0x01)
            {
                pRSD->isAllmeter = 0x01;
                offset ++;
            }
            else
            {
                pRSD->Ms = mtr_list_get_fujian(hUdp, inData, InDataLen, &offset, FALSE);
                if (!pRSD->Ms)
                {
                    PRTL_FMT_DEBUG("RSD解析错误，找不到MS集合\n");
                    return 0;
                }
                else
                {
                    return offset;
                }
            }
        }
        return offset;
        break;
    default:
        return 0;
    }

    return 0;
}

#endif
/**
*********************************************************************
* @brief：      获取透明任务方案信息
* @param[in]： taskId          -方案号
* @param[out]：task             方案内容
* @return：     =0              成功
               <0              错误码
*********************************************************************
*/
int transparent_task_sch_read(DB_CLIENT            hUdp, uint16 taskId, OOP_TRANS_TASK_T *data)
{
    NOMAL_OAD_T normal = {0};
    uint32      len  = 0;
    int         ret = ERR_OK;

    OOP_TRANS_TASK_T task;

    if (data == NULL)
    {
        return ERR_PTR;
    }

    memset(&task, 0, sizeof(task));
    memset(&normal, 0, sizeof(normal));
    normal.oad.value = 0x6e110200;
    normal.infoNum = taskId;
    ret = db_read_nomal(hUdp, &normal, sizeof(task), (uint8 *)&task, &len);
    if ((ret != ERR_OK) || (len != sizeof(task)))
    {
        PRTL_FMT_DEBUG("读任务方案失败. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x) len(%d) sizeof(task) = %d\n",
            ret, normal.logicId, normal.infoNum, normal.oad.value, len, sizeof(task));
        return ERR_NOTEXIST;
    }
    else
    {
        memcpy(data, &task, sizeof(task));
    }
    
    return ERR_OK;
}

/**
*********************************************************************
* @brief：      ms集合中是否包含tsa
* @param[in]： tsa          -   表地址
               ms              具体任务中的表集合
* @param[out]：
* @return：    TRUE             包含
              FASLE            不包含
*********************************************************************
*/
BOOL ms_is_include_tsa(OOP_TSA_T *tsa, OOP_MSC_T *ms)
{
    uint32 i = 0;
    
    if ((tsa == NULL) || (ms == NULL))
    {
        return FALSE;
    }

    if (ms->choice != 3)
    {
        PRTL_FMT_DEBUG("暂时不支持该类型 choice[%d]\n", ms->choice);
        return FALSE;
    }

    for (i = 0; i < ms->msa.size; i++)
    {
        //有相同的表地址，则跳出
        if (memcmp(tsa, &ms->msa.add[i], sizeof(OOP_TSA_T)) == 0)
        {
            return TRUE;
        }
    }

    return FALSE;
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
int db_read_start_virtualData_by_tas(DB_CLIENT              hUdp, OOP_TSA_T tsa, uint16 taskId, OOP_TRANS_TASK_T *task, 
    uint32 *handle, uint32 *recNum)
{
    LINK_TRANS_NODE_T *linkNode = NULL;
    LINK_TRANS_NODE_T *linkNodeHead = NULL;
    
    OOP_TRANS_TASK_RESULT_T data;
    OOP_TRANS_TASK_RESULT_T *p = NULL;
    uint32                  num = 0;
    OOP_TRANS_TASK_NODE_T   oopNode;

    //该表是否配置具体任务
    BOOL bTsaSetCfg = FALSE;
    
    if ((task == NULL) || (handle == NULL) )
    {
        return ERR_PTR;
    }

    memset(&data, 0, sizeof(data));
    memset(&oopNode, 0, sizeof(oopNode));

    //广播任务，回空数据即可
    if (task->taskTypeData.taskType == 1)
    {
        PRTL_FMT_DEBUG("任务类别[%d] 广播任务 回空数据即可\n", task->taskTypeData.taskType);
        return ERR_NOCONFIG;
    }

    linkNode = get_node_link_stat(taskId, 0, hUdp);
    if (linkNode != NULL)
    {
        linkNodeHead = linkNode;
    }
    else
    {
        PRTL_FMT_TRACE("任务方案[%d]下不存在具体任务\n", taskId);
        return ERR_NOTEXIST;
    }

    while (linkNode)
    {
        oopNode = linkNode->oopNode;
        num++;
        linkNode = linkNode->next;
    }
    linkNode = linkNodeHead;

    p = (OOP_TRANS_TASK_RESULT_T *)malloc(num * sizeof(OOP_TRANS_TASK_RESULT_T));
    num = 0;

    while (linkNode)
    {
        oopNode = linkNode->oopNode;
        if (ms_is_include_tsa(&tsa, &oopNode.ms))
        {
            bTsaSetCfg = TRUE;
            
            PRTL_BUF_DEBUG(&tsa.add[0], 6, "表地址:");
            PRTL_FMT_DEBUG("表地址存在具体任务[%d]中，应该返回任务未执行 方案号[%d]\n", oopNode.nodeid, taskId);
            
            data.transMsg.errCode = 12; //任务未执行
            data.nodeid = oopNode.nodeid;
            data.transTaskid = taskId;
            data.commType = task->commType;
            data.readTime.year = 2099;
            data.readTime.month = 12;
            data.readTime.day = 31;
            data.readTime.hour = 23;
            data.readTime.minute = 59;
            data.readTime.second = 59;

            p[num] = data;
            num ++;
        }
        linkNode = linkNode->next;
    }
    link_node_destroy(linkNodeHead);

    if (!bTsaSetCfg)
    {
        PRTL_FMT_DEBUG("表地址不存在具体任务中，应该返回任务未配置 方案号[%d]\n", taskId);
        data.transMsg.errCode = 13; //任务未配置
        data.transTaskid = taskId;
        data.commType = task->commType;
        data.readTime.year = 2099;
        data.readTime.month = 12;
        data.readTime.day = 31;
        data.readTime.hour = 23;
        data.readTime.minute = 59;
        data.readTime.second = 59;
        p[0] = data;
        num = 1;
    }

    *handle = (uint32)p;
    *recNum = num;

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
int db_read_step_virtualData_by_tas(DB_CLIENT              hUdp, uint32 handle, uint32 index, RECORD_UNIT_T *stepRecordOut)
{
    if ((stepRecordOut == NULL) || (handle == 0))
    {
        return ERR_PTR;
    }
        
    OOP_TRANS_TASK_RESULT_T *p = (OOP_TRANS_TASK_RESULT_T *)handle;
    OOP_TRANS_TASK_RESULT_T data = p[index];

    stepRecordOut->infoNum = data.transTaskid;
    stepRecordOut->recordnum = 1;
    stepRecordOut->record[0].road.oadMain.value = 0x6e150200;
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
int db_read_end_virtualData_by_tas(DB_CLIENT hUdp, uint32 *handle)
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
* @brief：      组织虚拟数据
* @param[in]： taskId          -方案号
               data            方案内容
* @param[out]：stepRecordOut    虚拟数据内容
* @return：     =0              成功
               <0              错误码
*********************************************************************
*/
int db_read_record_virtualData_by_node(DB_CLIENT hUdp, uint16 nodeId, uint16 taskId, OOP_TRANS_TASK_T *task, 
    RECORD_UNIT_T *stepRecordOut)
{
    LINK_TRANS_NODE_T *linkNode = NULL;
    LINK_TRANS_NODE_T *linkNodeHead = NULL;
    
    OOP_TRANS_TASK_RESULT_T data;
    OOP_TRANS_TASK_NODE_T   oopNode;
    
    if ((task == NULL) || (stepRecordOut == NULL))
    {
        return ERR_PTR;
    }

    memset(&data, 0, sizeof(data));
    memset(&oopNode, 0, sizeof(oopNode));

    //广播任务，回空数据即可
    if (task->taskTypeData.taskType == 1)
    {
        PRTL_FMT_DEBUG("任务类别[%d] 广播任务 回空数据即可\n", task->taskTypeData.taskType);
        return ERR_NOCONFIG;
    }

    linkNode = get_node_link_stat(taskId, nodeId, hUdp);
    if (linkNode != NULL)
    {
        linkNodeHead = linkNode;
        oopNode = linkNode->oopNode;
        
        PRTL_FMT_DEBUG("福建规约，存在具体任务，应该返回任务未执行\n");
        data.transMsg.errCode = 12; //任务未执行
        data.nodeid = oopNode.nodeid;
        link_node_destroy(linkNodeHead);
    }
    else
    {
        PRTL_FMT_DEBUG("福建规约，不存在具体任务，应该返回任务未配置\n");
        
        data.transMsg.errCode = 13; //任务未配置
    }

    data.transTaskid = taskId;
    data.commType = task->commType;
    data.readTime.year = 2099;
    data.readTime.month = 12;
    data.readTime.day = 31;
    data.readTime.hour = 23;
    data.readTime.minute = 59;
    data.readTime.second = 59;

    stepRecordOut->infoNum = taskId;
    stepRecordOut->recordnum = 1;
    stepRecordOut->record[0].road.oadMain.value = 0x6e150200;
    stepRecordOut->record[0].outDatalen = sizeof(data);
    memcpy(stepRecordOut->record[0].outDataBuf, &data, sizeof(data));

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
int32 read_transparent_results_param_get(RSD_T* pRSD, CSD_T* pCSD, APDU_INFO_T* pApduInfo, READ_RECORD_T* pInReadRecord, 
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
                if (pCSDTmp->Oad.nObjID != 0x6E16)
                {
                    PRTL_FMT_DEBUG("福建规约不存在RCSD中不存在OI是0x6E16的情况\n");
                    return ERR_TYPE;
                }
            }
        }
        pCSDTmp = pCSDTmp->next;
    }

    inReadRecord.recordOAD.classtag = 0;
    inReadRecord.recordOAD.road.oadMain.value = 0x6E150200;
    inReadRecord.recordOAD.road.oadCols.oad[0].value = 0;
    inReadRecord.recordOAD.road.oadCols.nNum = 1;

    //因为oopType.h中宏的限制，目前关联OAD最大数量是OOP_MAX_OADS，对福建来说，有点多余
    if (inReadRecord.recordOAD.road.oadCols.nNum > OOP_MAX_OADS)
    {
        return DATA_SCOPE_OF_ACCESS_VIOLATED;
    }

    /*填写逻辑地址*/
    inReadRecord.recordOAD.logicId = 0;

    /*填写信息点*/
    inReadRecord.recordOAD.infoNum = pRSD->transTaskid;

    switch (pRSD->selId)
    {
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
            relyInfo.LastN = pRSD->LastN;
            relyInfo.recNum = pRSD->recNum;
        }
        nRet = ERR_OK;
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
        relyInfo.ms = pRSD->Ms;
        relyInfo.isAllmeter = pRSD->isAllmeter;
        *pInReadRecord = inReadRecord;
        *pRelyInfo = relyInfo;
    }

    return nRet;
}

/**
*********************************************************************
* @brief：     应用分帧情况下判断是否继续抄读该测量点
* @param[in]： bpInfo      断点信息
* @param[in]： metsn       测量点号
* @param[out]：
* @return：    0           继续抄读
               1           抄读下一个电表信息
*********************************************************************
*/
uint8 slicing_is_comtinue_ms_fujian(BP_INFO_T* bpInfo, uint16 metsn, READ_RECORD_T* inReadRecord, 
    uint32* oopRecNumPn, uint32 *index)
{
    int nRet = 0;

    if (bpInfo == NULL)
        return 0;//继续抄读

    *index = 0;
    
    if (bpInfo->appSlicingFlag != 0)
    {
        if (bpInfo->metsn != 0)
        {
            if (metsn != bpInfo->metsn)
            {
                nRet = 1;//抄读下一个电表信息
            }
            else
            {
                /*查到断点，将断点信息填入 如果bpInfo->recNumSeg>0时，则是按方法1刷选，起始时间填0，结束时间填断点时间*/
                *oopRecNumPn = bpInfo->recNumSeg;

                if (bpInfo->recNumSeg > 0)
                {
                    inReadRecord->cStart = 0;
                    inReadRecord->cEnd = bpInfo->tmStartSeg;
                }
                else
                {
                    inReadRecord->cStart = bpInfo->tmStartSeg;
                }

                *index = bpInfo->index;

                bpInfo->metsn = 0;
                nRet = 0;
            }
        }
    }

    return nRet;
}

/**
*********************************************************************
* @brief：     应用分帧情况下判断是否继续抄读该任务
* @param[in]： bpInfo      断点信息
* @param[in]： metsn       测量点号
* @param[out]：
* @return：    0           继续抄读
               1           抄读下一个电表信息
*********************************************************************
*/
uint8 slicing_is_comtinue_task_fujian(BP_INFO_T* bpInfo, uint16 metsn, READ_RECORD_T* inReadRecord, uint32* oopRecNumPn)
{
    int nRet = 0;

    if (bpInfo == NULL)
        return 0;//继续抄读

    if (bpInfo->appSlicingFlag != 0)
    {
        if (bpInfo->metsn != 0)
        {
            if (metsn != bpInfo->metsn)
            {
                nRet = 1;//抄读下一个任务
            }
            else
            {
                *oopRecNumPn = bpInfo->recNumSeg;

                if (bpInfo->recNumSeg > 0)
                {
                    inReadRecord->cEnd = bpInfo->tmStartSeg + 1;
                }
                else
                {
                    inReadRecord->cStart = bpInfo->tmStartSeg;
                }

                bpInfo->metsn = 0;
                nRet = 0;
            }
        }
    }

    return nRet;
}

/**
*********************************************************************
* @brief：     应用分帧情况下判断是否继续抄读该测量点
* @param[in]： bpInfo      断点信息
* @param[in]： metsn       测量点号
* @param[out]：
* @return：    0           继续抄读
               1           抄读下一个电表信息
*********************************************************************
*/
uint8 slicing_is_comtinue_node_fujian(BP_INFO_T* bpInfo, uint16 node_cnt)
{
    int nRet = 0;

    if (bpInfo == NULL)
        return 0;//继续抄读

    if (bpInfo->appSlicingFlag != 0)
    {
        if (bpInfo->node_cnt != 0)
        {
            if (node_cnt != bpInfo->node_cnt)
            {
                nRet = 1;//抄读下一个电表信息
            }
            else
            {
                /*查到断点*/
                bpInfo->node_cnt = 0;
                nRet = 0;
            }
        }
    }

    return nRet;
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
int record_data_is_valid_trans(READ_RECORD_T inReadRecord, RECORD_UNIT_T unit)
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
    PRTL_FMT_TIME("单步读结果 unit.colStoreTm", unit.colStoreTm);
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
        if ((unit.record[i].road.oadMain.value != 0x6E150200) || 
            (unit.record[i].outDatalen != sizeof(OOP_TRANS_TASK_RESULT_T)))
        {
            PRTL_FMT_DEBUG("单步读结果不符合规约要求 unit.record[%d].road.oadMain.value = 0x%08x unit.record[%d].outDatalen = %d\n", 
                i, unit.record[i].road.oadMain.value, i, unit.record[i].outDatalen);
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
int transparent_results_to_698buf_malloc(RECORD_UNIT_T unit, OOP_RCSD_T oopRcsd, MS_T* ms, uint32 index, 
    uint8** ppOutBuf, uint32* outLen)
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

    OOP_TRANS_TASK_RESULT_T data;

    memset(&data, 0, sizeof(data));

    /* 入参检查 */
    if ((unit.recordnum == 0) || (outLen == NULL))
    {
        PRTL_FMT_DEBUG("福建规约，组报文接口参数错误. unit.recordnum[%d] ms[%p], outLen[%d]\n", unit.recordnum, ms, outLen);
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
        if (unit.record[j].road.oadMain.value != 0x6e150200)
        {
            PRTL_FMT_DEBUG("福建规约透明结果中不应该存在主次OAD unit.record[%d]\n", j);
            free(pOutBuf);
            pOutBuf = NULL;
            return ERR_OPTION;
        }

        memcpy(&data, unit.record[j].outDataBuf, sizeof(data));

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

                if (ms == NULL)
                {
                    memcpy(&pOutBuf[offset], &data.groupAdd.add[0], sizeof(OOP_OCTETSTR6_T));
                }
                else
                {
                    memcpy(&pOutBuf[offset], &ms->oopMeter.basic.tsa.add[0], sizeof(OOP_OCTETSTR6_T));
                }
                
                offset += sizeof(OOP_OCTETSTR6_T);
                continue;
            }
            else if (!class_oadinfo_get(oopRcsd.cols[i].oad, &pOadInfo))
            {
                PRTL_FMT_DEBUG("RCSD中不应该存在这个oad[0x%08x]\n", oopRcsd.cols[i].oad.value);
                pOutBuf[offset++] = DT_NULL;
                continue;
            }
            else if (pOadInfo.nIndex > 11)
            {
                PRTL_FMT_DEBUG("索引超过最大范围oad[0x%08x] nIndex[%d]\n", oopRcsd.cols[i].oad.value, 
                    pOadInfo.nIndex);
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
                convert.srcBuf = unit.record[j].outDataBuf;
                convert.srcLen = unit.record[j].outDatalen;
                break;
            case 1:
                convert.srcBuf = &data.transTaskid;
                convert.srcLen = sizeof(data.transTaskid);
                break;
            case 2:
                convert.srcBuf = &data.nodeid;
                convert.srcLen = sizeof(data.nodeid);
                break;
            case 3:
                convert.srcBuf = &data.groupAdd;
                convert.srcLen = sizeof(data.groupAdd);
                break;
            case 4:
                convert.srcBuf = &data.commType;
                convert.srcLen = sizeof(data.commType);
                break;
            case 5:
                convert.srcBuf = &data.storeTime;
                convert.srcLen = sizeof(data.storeTime);
                break;
            case 6:
                convert.srcBuf = &data.readTime;
                convert.srcLen = sizeof(data.readTime);
                break;
            case 7:
                convert.srcBuf = &data.storeNO;
                convert.srcLen = sizeof(data.storeNO);
                break;
            case 8:
                convert.srcBuf = &data.retryTurn;
                convert.srcLen = sizeof(data.retryTurn);
                break;
            case 9:
                convert.srcBuf = &data.result;
                convert.srcLen = sizeof(data.result);
                break;
            case 10:
                convert.srcBuf = &data.resultMessType;
                convert.srcLen = sizeof(data.resultMessType);
                break;
            case 11:
                convert.srcBuf = &data.transMsg;
                convert.srcLen = sizeof(data.transMsg);
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
* @brief：      任务结果空处理
* @param[in]： hUdp          -  数据中心句柄
               taskId          任务方案
* @param[out]：buf              目的数据
               bufLen          目的数据长度
* @return：     =0              成功
               <0              错误码
*********************************************************************
*/
int trans_result_null_proc(DB_CLIENT hUdp, uint16 taskId, RELY_INFO_T relyInfo, uint8 *buf, uint32 *bufLen)
{
    OOP_TRANS_TASK_T task;
    
    NOMAL_OAD_T normal = {0};
    uint32      len  = 0;
    int         ret = ERR_OK;

    LINK_TRANS_NODE_T *linkNode = NULL;
    LINK_TRANS_NODE_T *linkNodeHead = NULL;

    RECORD_UNIT_T      stepRecordOut;
    OOP_TRANS_TASK_RESULT_T data;

    uint8     *pBufTmp    = NULL;
    uint32     bufLenTmp  = 0;
    OOP_RCSD_T oopRcsdtmp = relyInfo.oopRcsd;

    if (buf == NULL || bufLen == NULL)
    {
        return ERR_PTR;
    }

    memset(&task, 0, sizeof(task));
    memset(&normal, 0, sizeof(normal));
    memset(&stepRecordOut, 0, sizeof(stepRecordOut));
    memset(&data, 0, sizeof(data));
    
    normal.oad.value = 0x6e110200;
    normal.infoNum = taskId;
    ret = db_read_nomal(hUdp, &normal, sizeof(task), (uint8 *)&task, &len);
    if ((ret != ERR_OK) || (len != sizeof(task)))
    {
        PRTL_FMT_DEBUG("读任务方案失败. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x) len(%d) sizeof(task) = %d\n",
            ret, normal.logicId, normal.infoNum, normal.oad.value, len, sizeof(task));
        return ERR_NOTEXIST;
    }

    //广播任务，回空数据即可
    if (task.taskTypeData.taskType == 1)
    {
        PRTL_FMT_DEBUG("任务类别[%d] 广播任务 回空数据即可\n", task.taskTypeData.taskType);
        return ERR_NOCONFIG;
    }

    linkNode = get_node_link_stat(taskId, 0, hUdp);
    if (linkNode != NULL)
    {
        linkNodeHead = linkNode;
        link_node_destroy(linkNodeHead);
        PRTL_FMT_DEBUG("福建规约，存在具体任务，应该返回任务未执行\n");
        data.transMsg.errCode = 12; //任务未执行
    }
    else
    {
        PRTL_FMT_DEBUG("福建规约，不存在具体任务，应该返回任务未配置\n");
        data.transMsg.errCode = 13; //任务未配置
    }

    data.transTaskid = taskId;
    data.commType = task.commType;
    data.readTime.year = 2099;
    data.readTime.month = 12;
    data.readTime.day = 31;
    data.readTime.hour = 23;
    data.readTime.minute = 59;
    data.readTime.second = 59;

    stepRecordOut.infoNum = taskId;
    stepRecordOut.recordnum = 1;
    stepRecordOut.record[0].road.oadMain.value = 0x6e150200;
    stepRecordOut.record[0].outDatalen = sizeof(data);
    memcpy(stepRecordOut.record[0].outDataBuf, &data, sizeof(data));

    ret = transparent_results_to_698buf_malloc(stepRecordOut, oopRcsdtmp, NULL, 0, &pBufTmp, &bufLenTmp);
    if (ret < ERR_OK)
    {
        PRTL_FMT_DEBUG("福建规约，组织报文错误 err[%d]!\n", ret);
        return ERR_NORMAL;
    }
    else
    {
        //结果数量1
        buf[0] = 1;

        //结果内容
        memcpy(&buf[1], pBufTmp, bufLenTmp);

        //结果数据长度
        *bufLen = bufLenTmp + 1;
        
        free(pBufTmp);
        pBufTmp = NULL;
    }

    return ERR_OK;
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
int record_transparent_results_read_ms(DB_CLIENT hUdp, const READ_RECORD_T inReadRecord, RELY_INFO_T relyInfo, 
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
    MS_T               *ms = NULL;

    OOP_TRANS_TASK_T   task;
    BOOL               bTaskExist = TRUE;
    BOOL               bVirtualData = FALSE;

    int32  ret = ERR_NORMAL;
    uint32 handle = 0;
    uint32 hVirData = 0;
    uint32 recNum = 0;
    uint32 oopRecNumPn = 0;       //单个表的记录数索引，一般用于方法54，这里用不着
    uint32 oopRecNumAll = 0;      //总记录表中记录条数
    uint32 len = 0;
    uint8  offlen = 0;
    uint8  lenBuf[3] = { 0 };
    uint32 index = 0;

    memset(&inReadRecordTmp, 0, sizeof(inReadRecordTmp));
    memset(&oopRcsdtmp, 0, sizeof(oopRcsdtmp));
    memset(&task, 0, sizeof(task));

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

    if (relyInfo.nodeidNum > 0)
    {
        PRTL_FMT_DEBUG("福建功能，读透明结果接口调用错误 nodeidNum[%d]\n", relyInfo.nodeidNum);
        return ERR_PTR;
    }

    if ((relyInfo.LastN == 0) && (relyInfo.recNum > 0))
    {
        PRTL_FMT_DEBUG("福建功能，读透明结果接口调用错误 relyInfo.LastN[%d] relyInfo.recNum\n", relyInfo.LastN, 
            relyInfo.recNum);
        return ERR_PTR;
    }

    //临时变量，备份作用
    inReadRecordTmp = inReadRecord;
    oopRcsdtmp = relyInfo.oopRcsd;

    if ((relyInfo.LastN > 0) && (relyInfo.recNum > 0))
    {
        //倒序从数据中心取数据
        inReadRecordTmp.sortType = 0x01;
    }
    else
    {
        //正序从数据中心取数据
        inReadRecordTmp.sortType = 0x00;
    }

    //清除断点信息, 需要注意应用分帧的未帧
    memset(&outRecordBuf->breakInfo, 0, sizeof(BP_INFO_T));

    //取ms集合
    ms = relyInfo.ms;

    //申请一份内存，注意释放
    recordBufTmp = (uint8*)malloc(RECORD_LEN_MAX);
    memset(recordBufTmp, 0, RECORD_LEN_MAX);

    //记录数先存0，OAD+RCSD+1 已经在上层接口中完成 
    offset = 0;
    recordBufTmp[offset++] = 0x00;

    //读透明任务方案内容，置方案是否存在标记
    ret = transparent_task_sch_read(hUdp, inReadRecord.recordOAD.infoNum, &task);
    if (ret != ERR_OK)
    {
        bTaskExist = FALSE;
    }

    while (ms)
    {
        //判断如果是应该分帧，跳过已经处理过的测量点，填入新的起始时间
        inReadRecordTmp.cStart = inReadRecord.cStart;
        inReadRecordTmp.cEnd = inReadRecord.cEnd;

        //福建规约中MAC表地址用于筛选数据，代替以前的信息点
        inReadRecordTmp.table.nNum = 0x06;
        memcpy(&inReadRecordTmp.table.value[0], &ms->oopMeter.basic.tsa.add[0], 6);
        inReadRecordTmp.bTable = 0x01;
        

        //信息点选用福建规约中的任务方案号
        inReadRecordTmp.recordOAD.infoNum = inReadRecord.recordOAD.infoNum;

        //次OAD填具体任务号，0标识所有任务
        inReadRecordTmp.recordOAD.road.oadCols.nNum = 0;

        oopRecNumPn = 0;
        index = 0;
        if (slicing_is_comtinue_ms_fujian(relyInfo.bpBreakInfo, ms->MtrNo, &inReadRecordTmp, &oopRecNumPn, &index))
        {
            ms = ms->next;
            continue;
        }

        /*开始读记录*/
        //开始读需要使用福建特殊接口
        ret = db_read_record_start(hUdp, &inReadRecordTmp, &handle, &recNum);
        if (ret != ERR_OK)
        {
            PRTL_FMT_DEBUG("福建规约，开始读记录出错 ret[%d] infoNum[%d] recNum[%d]\n",
                ret, inReadRecordTmp.recordOAD.infoNum, recNum);

            ms = ms->next;
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
            //3、当召测的对象有任务信息没有执行记录时，ERR-CODE内容填“任务未执行”，终端抄表时
            //   间填2099年12月31日23：59：59，召测时有提供的信息填提供的信息，其他未提供的信息填0。
            //4、当召测的对象无对应具体任务信息时，ERR-CODE填“任务未配置”，终端抄表时间填2099年12月
            //   31日23：59：59，召测时有提供的信息填提供的信息，其他未提供的信息填0。
            //5、未配置任务方案时，按无数据响应。
            PRTL_FMT_DEBUG("福建规约，开始读记录为空 ret[%d] infoNum[%d] recNum[%d]\n",
                ret, inReadRecordTmp.recordOAD.infoNum, recNum);
            if (bTaskExist)
            {
                //存在任务方案，需要按上述注释需求，虚拟数据
                ret = db_read_start_virtualData_by_tas(hUdp, ms->oopMeter.basic.tsa, inReadRecord.recordOAD.infoNum, 
                    &task, &hVirData, &recNum);
                PRTL_FMT_DEBUG("福建规约，开始读虚拟记录 ret[%d] infoNum[%d] recNum[%d]\n",
                    ret, inReadRecordTmp.recordOAD.infoNum, recNum);
                if (ret != ERR_OK || recNum == 0)
                {
                    db_read_end_virtualData_by_tas(hUdp, &hVirData);
                    
                    ms = ms->next;
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
                ms = ms->next;
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
            PRTL_FMT_LOGCC("福建规约，开始读记录成功 ret[%d] infoNum[%d] recNum[%d]\n",
                ret, inReadRecordTmp.recordOAD.infoNum, recNum);
        }

        /*循环单步读操作*/
        int i = 0;
        int k = 0;
        stepReadRecord.MAC.nNum = 0x06;
        memcpy(&stepReadRecord.MAC.value[0], &ms->oopMeter.basic.tsa.add[0], 6);
        PRTL_BUF_LOGCC(&stepReadRecord.MAC.value[0], 6, "单步读的MAC校验: ");
        for (i = 0; i < recNum; i++)
        {
            //虚拟数据和数据中心读到的数据，分开处理，虚拟数据不需要检查数据有效性
            if (bVirtualData == TRUE)
            {
                memset(&stepRecordOut, 0, sizeof(stepRecordOut));
                ret = db_read_step_virtualData_by_tas(hUdp, hVirData, i, &stepRecordOut);
                if (ret != ERR_OK)
                {
                    PRTL_FMT_DEBUG("福建规约，组织虚拟数据出错err[%d]\n", ret);
                    continue;
                }

                //断点处理
                if (i < index)
                {
                    continue;
                }
                else
                {
                    index = 0;
                }
            }
            else
            {
                ret = db_read_record_step(hUdp, handle, &stepReadRecord, &stepRecordOut);
                if (ret != ERR_OK)
                {
                    PRTL_FMT_DEBUG("福建规约，单步读出错err[%d]\n", ret);
                    continue;
                }

                /* 判断是否是所需记录单元 */
                ret = record_data_is_valid_trans(inReadRecordTmp, stepRecordOut);
                if (ret < ERR_OK)
                {
                    continue;
                }
            }

            for (k = 0; k < stepRecordOut.recordnum; k++)
            {
                ret = transparent_results_to_698buf_malloc(stepRecordOut, oopRcsdtmp, ms, k, &pBufTmp, &bufLenTmp);
                if (ret < ERR_OK)
                {
                    db_read_record_end(hUdp, handle);
                    db_read_end_virtualData_by_tas(hUdp, &hVirData);
                    PRTL_FMT_DEBUG("福建规约，组织单步报文错误 err[%d]!\n", ret);
                    free(recordBufTmp);
                    recordBufTmp = NULL;
                    return ERR_NORMAL;
                }
                else
                {
                    //方法54
                    if ((relyInfo.LastN > 0) && (relyInfo.recNum > 0))
                    {
                        if (oopRecNumPn >= (relyInfo.LastN + relyInfo.recNum - 1))
                        {
                            //方法54，达到该测量点的最大记录数，跳转到下一个测量点
                            oopRecNumPn = 0;
                            offset = 0;
                            isMaxRec = TRUE;
                            break;
                        }
                        else if (oopRecNumPn < relyInfo.LastN - 1)
                        {
                            oopRecNumPn ++;
                            free(pBufTmp);
                            pBufTmp = NULL;
                            continue;
                        }
                    }

                    /* 拷贝数据域内容到recordBufTmp */
                    memcpy(&recordBufTmp[offset], pBufTmp, bufLenTmp);
                    offset += bufLenTmp;

                    bufAllLenLast = bufAllLen;
                    bufAllLen += offset;

                    //方法51 53 54 实时记下断点信息
                    switch (inReadRecord.cType)
                    {
                    case COL_TM_STORE:
                        outRecordBuf->breakInfo.tmStartSeg = stepRecordOut.colStoreTm;
                        break;
                    default:
                        PRTL_FMT_DEBUG("record_task_table_read inReadRecord.tmType err!\n");
                        db_read_record_end(hUdp, handle);
                        db_read_end_virtualData_by_tas(hUdp, &hVirData);
                        free(pBufTmp);
                        pBufTmp = NULL;
                        free(recordBufTmp);
                        recordBufTmp = NULL;
                        return ERR_PTR;
                        break;
                    }

                    // 方法54实时记下断点信息
                    if ((relyInfo.LastN > 0) && (relyInfo.recNum > 0))
                    {
                        outRecordBuf->breakInfo.recNumSeg = oopRecNumPn;
                    }
                    else
                    {
                        outRecordBuf->breakInfo.recNumSeg = 0;
                    }

                    outRecordBuf->breakInfo.index = i;

                    len = offset;

                    if (bufAllLen <= maxBufLen)
                    {
                        memcpy(&outRecordBuf->recordBuf[bufAllLenLast], recordBufTmp, offset);
                        memset(&recordBufTmp[0], 0x00, RECORD_LEN_MAX);
                        oopRecNumPn ++;
                        oopRecNumAll ++;
                        offset = 0;

                        //打印一个单步完整记录报文信息
                        PRTL_FMT_LOGCC("*****************打印一个单步完整记录信息*********************\n");
                        PRTL_FMT_LOGCC("stepRecordOut.infoNum = %d\n", stepRecordOut.infoNum);
                        PRTL_FMT_LOGCC("oopRecNumPn = %d\n", oopRecNumPn);
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

            //方法54，达到该测量点的最大记录数，跳转到下一个测量点
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

        if (bVirtualData)
        {
            bVirtualData = FALSE;
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
            outRecordBuf->breakInfo.metsn = ms->MtrNo;
            memcpy(&outRecordBuf->breakInfo.MAC, &ms->oopMeter.basic.tsa.add[0], sizeof(OOP_OCTETSTR6_T));
            outRecordBuf->bufLen = bufAllLenLast;                               //填写返回报文长度
            break;
        }

        /*结束读记录*/
        db_read_record_end(hUdp, handle);
        db_read_end_virtualData_by_tas(hUdp, &hVirData);
        ms = ms->next;
    }

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
int record_transparent_results_read_all_ms(DB_CLIENT hUdp, const READ_RECORD_T inReadRecord, RELY_INFO_T relyInfo, 
    uint32 maxBufLen, RECORD_BUF_T* outRecordBuf)
{
    READ_RECORD_T      inReadRecordTmp;
    STEP_READ_RECORD_T stepReadRecord;
    RECORD_UNIT_T      stepRecordOut;
    uint8* pBufTmp = NULL;
    uint32             bufLenTmp = 0;
    uint32             offset = 0;

    uint8* recordBufTmp = NULL;
    uint32             bufAllLen = 0;
    uint32             bufAllLenLast = 0;
    BOOL               isBufFull = FALSE;
    BOOL               isMaxRec = FALSE; 
    
    OOP_RCSD_T         oopRcsdtmp;

    //获取具体方案号
    OOP_TRANS_TASK_NODE_T data;

    LINK_TRANS_NODE_T *linkNode = NULL;
    LINK_TRANS_NODE_T *linkNodeHead = NULL;

    int32  ret = ERR_NORMAL;
    uint32 handle = 0;
    uint32 recNum = 0;
    uint32 oopRecNumPn = 0;       //单个表的记录数索引，一般用于方法54，这里用不着
    uint32 oopRecNumAll = 0;      //总记录表中记录条数
    uint32 len = 0;
    uint8  offlen = 0;
    uint8  lenBuf[3] = { 0 };
    uint32 index = 0;             //断点时，记录下处理到哪个点，更加细化的断点。

    memset(&inReadRecordTmp, 0, sizeof(inReadRecordTmp));
    memset(&oopRcsdtmp, 0, sizeof(oopRcsdtmp));
    memset(&data, 0, sizeof(data));

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

    if (relyInfo.nodeidNum > 0)
    {
        PRTL_FMT_DEBUG("福建功能，读透明结果接口调用错误 nodeidNum[%d]\n", relyInfo.nodeidNum);
        return ERR_PTR;
    }

    if ((relyInfo.LastN == 0) && (relyInfo.recNum > 0))
    {
        PRTL_FMT_DEBUG("福建功能，读透明结果接口调用错误 relyInfo.LastN[%d] relyInfo.recNum\n", relyInfo.LastN, 
            relyInfo.recNum);
        return ERR_PTR;
    }

    //临时变量，备份作用
    inReadRecordTmp = inReadRecord;
    oopRcsdtmp = relyInfo.oopRcsd;

    if ((relyInfo.LastN > 0) && (relyInfo.recNum > 0))
    {
        //倒序从数据中心取数据
        inReadRecordTmp.sortType = 0x01;
    }
    else
    {
        //正序从数据中心取数据
        inReadRecordTmp.sortType = 0x00;
    }

    //清除断点信息, 需要注意应用分帧的未帧
    memset(&outRecordBuf->breakInfo, 0, sizeof(BP_INFO_T));

    //申请一份内存，注意释放
    recordBufTmp = (uint8*)malloc(RECORD_LEN_MAX);
    memset(recordBufTmp, 0, RECORD_LEN_MAX);

    //记录数先存0，OAD+RCSD+1 已经在上层接口中完成 
    offset = 0;
    recordBufTmp[offset++] = 0x00;

    linkNode = get_node_link_stat(inReadRecord.recordOAD.infoNum, 0, hUdp);
    if (linkNode == NULL)
    {
        //不需要返回错误码，空数据就行，后面需要组织特殊报文
        PRTL_FMT_DEBUG("查找具体任务失败 taskID[%d]\n", inReadRecord.recordOAD.infoNum);
        //return DATA_DATA_BLOCK_UNAVAILABLE;
    }
    else
    {
        linkNodeHead = linkNode;
    }

    while (linkNode)
    {
        //判断如果是应该分帧，跳过已经处理过的测量点，填入新的起始时间
        inReadRecordTmp.cStart = inReadRecord.cStart;
        inReadRecordTmp.cEnd = inReadRecord.cEnd;

        //福建规约中MAC表地址用于筛选数据，代替以前的信息点，0表示所有表
        inReadRecordTmp.table.nNum = 0x00;
        memset(&inReadRecordTmp.table.value[0], 0, 6);
        inReadRecordTmp.bTable = 0x01;
        
        //信息点选用福建规约中的任务方案号
        inReadRecordTmp.recordOAD.infoNum = inReadRecord.recordOAD.infoNum;

        oopRecNumPn = 0;
        index = 0;
        if (slicing_is_comtinue_ms_fujian(relyInfo.bpBreakInfo, linkNode->no, &inReadRecordTmp, &oopRecNumPn, &index))
        {
            linkNode = linkNode->next;
            continue;
        }

        data = linkNode->oopNode;

        //填写具体任务号
        inReadRecordTmp.recordOAD.road.oadCols.oad[0].value = data.nodeid;

        /*开始读记录，根据具体任务筛选，读所有MS数据*/
        ret = db_read_record_start(hUdp, &inReadRecordTmp, &handle, &recNum);
        if ((ret != ERR_OK) || (recNum == 0))
        {
            PRTL_FMT_DEBUG("福建规约，开始读记录出错 ret[%d] infoNum[%d] recNum[%d]\n",
                ret, inReadRecordTmp.recordOAD.infoNum, recNum);
            
            linkNode = linkNode->next;
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
            PRTL_FMT_LOGCC("福建规约，开始读记录成功 ret[%d] infoNum[%d] recNum[%d]\n",
                ret, inReadRecordTmp.recordOAD.infoNum, recNum);
        }

        /*循环单步读操作*/
        int i = 0;
        int k = 0;

        //针对福建特殊开始读接口，MAC填空
        stepReadRecord.MAC.nNum = 0x06;
        memset(&stepReadRecord.MAC.value[0], 0, 6);
        PRTL_BUF_LOGCC(&stepReadRecord.MAC.value[0], 6, "单步读的MAC校验: ");
        for (i = 0; i < recNum; i++)
        {
            ret = db_read_record_step(hUdp, handle, &stepReadRecord, &stepRecordOut);
            if (ret != ERR_OK)
            {
                PRTL_FMT_DEBUG("福建规约，单步读出错err[%d]\n", ret);
                continue;
            }

            /* 跳过断点前已经处理过的单步信息 */
            if (i < index)
            {
                PRTL_FMT_LOGCC("跳过断点前已经处理过的单步信息 %d < index index = %d!\n", i, index);
                continue;
            }
            else
            {
                index = 0;
            }

            /* 判断是否是所需记录单元 */
            ret = record_data_is_valid_trans(inReadRecordTmp, stepRecordOut);
            if (ret < ERR_OK)
            {
                continue;
            }

            for (k = 0; k < stepRecordOut.recordnum; k++)
            {
                ret = transparent_results_to_698buf_malloc(stepRecordOut, oopRcsdtmp, NULL, k, &pBufTmp, &bufLenTmp);
                if (ret < ERR_OK)
                {
                    db_read_record_end(hUdp, handle);
                    link_node_destroy(linkNodeHead);
                    PRTL_FMT_DEBUG("福建规约，组织单步报文错误 err[%d]!\n", ret);
                    free(recordBufTmp);
                    recordBufTmp = NULL;
                    return ERR_NORMAL;
                }
                else
                {
                    // 方法54实时记下断点信息
                    if ((relyInfo.LastN > 0) && (relyInfo.recNum > 0))
                    {
                        if (oopRecNumPn >= (relyInfo.LastN + relyInfo.recNum - 1))
                        {
                            //方法54，达到该测量点的最大记录数，跳转到下一个测量点
                            oopRecNumPn = 0;
                            offset = 0;
                            isMaxRec = TRUE;
                            break;
                        }
                        else if (oopRecNumPn < relyInfo.LastN - 1)
                        {
                            oopRecNumPn ++;
                            free(pBufTmp);
                            pBufTmp = NULL;
                            continue;
                        }
                    }

                    /* 拷贝数据域内容到recordBufTmp */
                    memcpy(&recordBufTmp[offset], pBufTmp, bufLenTmp);
                    offset += bufLenTmp;

                    bufAllLenLast = bufAllLen;
                    bufAllLen += offset;

                    //方法51 53 54 实时记下断点信息
                    switch (inReadRecord.cType)
                    {
                    case COL_TM_STORE:
                        outRecordBuf->breakInfo.tmStartSeg = stepRecordOut.colStoreTm;
                        break;
                    default:
                        PRTL_FMT_DEBUG("record_task_table_read inReadRecord.tmType err!\n");
                        db_read_record_end(hUdp, handle);
                        link_node_destroy(linkNodeHead);
                        free(pBufTmp);
                        pBufTmp = NULL;
                        free(recordBufTmp);
                        recordBufTmp = NULL;
                        return ERR_PTR;
                        break;
                    }

                    // 方法54实时记下断点信息
                    if ((relyInfo.LastN > 0) && (relyInfo.recNum > 0))
                    {
                        outRecordBuf->breakInfo.recNumSeg = oopRecNumPn;
                    }
                    else
                    {
                        outRecordBuf->breakInfo.recNumSeg = 0;
                    }
                    
                    outRecordBuf->breakInfo.index = i;

                    len = offset;

                    if (bufAllLen <= maxBufLen)
                    {
                        memcpy(&outRecordBuf->recordBuf[bufAllLenLast], recordBufTmp, offset);
                        memset(&recordBufTmp[0], 0x00, RECORD_LEN_MAX);
                        oopRecNumPn ++;
                        oopRecNumAll ++;
                        offset = 0;

                        //打印一个单步完整记录报文信息
                        PRTL_FMT_LOGCC("*****************打印一个单步完整记录信息*********************\n");
                        PRTL_FMT_LOGCC("stepRecordOut.infoNum = %d\n", stepRecordOut.infoNum);
                        PRTL_FMT_LOGCC("oopRecNumPn = %d\n", oopRecNumPn);
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

            //如果达到最大长度，跳出循环
            if ((isBufFull) && (bufAllLenLast > 0))
            {
                break;
            }

            //方法54，达到该测量点的最大记录数，跳转到下一个测量点
            if (isMaxRec)
            {
                isMaxRec = FALSE;
                break;
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
            outRecordBuf->breakInfo.metsn = linkNode->no;
            outRecordBuf->bufLen = bufAllLenLast;                               //填写返回报文长度
            break;
        }

        /*结束读记录*/
        db_read_record_end(hUdp, handle);
        linkNode = linkNode->next;
    }
    link_node_destroy(linkNodeHead);

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
int record_transparent_results_read_node(DB_CLIENT hUdp, const READ_RECORD_T inReadRecord, RELY_INFO_T relyInfo,
    uint32 maxBufLen, RECORD_BUF_T* outRecordBuf)
{
    READ_RECORD_T      inReadRecordTmp;
    STEP_READ_RECORD_T stepReadRecord;
    RECORD_UNIT_T      stepRecordOut;
    uint8* pBufTmp = NULL;
    uint32             bufLenTmp = 0;
    uint32             offset = 0;

    uint8* recordBufTmp = NULL;
    uint32             bufAllLen = 0;
    uint32             bufAllLenLast = 0;
    BOOL               isBufFull = FALSE;

    OOP_RCSD_T         oopRcsdtmp;

    OOP_TRANS_TASK_T   task;
    BOOL               bTaskExist = TRUE;
    BOOL               bVirtualData = FALSE;

    int32  j = 0;
    int32  ret = ERR_NORMAL;
    uint32 handle = 0;
    uint32 recNum = 0;
    uint32 oopRecNumAll = 0;      //总记录表中记录条数
    uint32 len = 0;
    uint8  offlen = 0;
    uint8  lenBuf[3] = { 0 };
    uint32 index = 0;              //断点时，记录下处理到哪个点，更加细化的断点。

    memset(&inReadRecordTmp, 0, sizeof(inReadRecordTmp));
    memset(&oopRcsdtmp, 0, sizeof(oopRcsdtmp));
    memset(&task, 0, sizeof(task));

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

    if (relyInfo.nodeidNum == 0)
    {
        PRTL_FMT_DEBUG("福建功能，读透明结果接口调用错误 nodeidNum[%d]\n", relyInfo.nodeidNum);
        return ERR_PTR;
    }

    //临时变量，备份作用
    inReadRecordTmp = inReadRecord;
    oopRcsdtmp = relyInfo.oopRcsd;

    //正序从数据中心取数据
    inReadRecordTmp.sortType = 0x00;

    if (relyInfo.bpBreakInfo->appSlicingFlag == 1)
    {
        index = relyInfo.bpBreakInfo->index;
    }

    //清除断点信息, 需要注意应用分帧的未帧
    memset(&outRecordBuf->breakInfo, 0, sizeof(BP_INFO_T));

    //申请一份内存，注意释放
    recordBufTmp = (uint8*)malloc(RECORD_LEN_MAX);
    memset(recordBufTmp, 0, RECORD_LEN_MAX);

    //记录数先存0，OAD+RCSD+1 已经在上层接口中完成 
    offset = 0;
    recordBufTmp[offset++] = 0x00;

    //读透明任务方案内容，置方案是否存在标记
    ret = transparent_task_sch_read(hUdp, inReadRecord.recordOAD.infoNum, &task);
    if (ret != ERR_OK)
    {
        bTaskExist = FALSE;
    }

    for (j = 0; j < relyInfo.nodeidNum; j++)
    {
        //判断如果是应该分帧，跳过已经处理过的测量点，填入新的起始时间
        inReadRecordTmp.cStart = inReadRecord.cStart;
        inReadRecordTmp.cEnd = inReadRecord.cEnd;

        //福建规约中MAC表地址用于筛选数据，代替以前的信息点，0表示所有表
        inReadRecordTmp.table.nNum = 0x00;
        memset(&inReadRecordTmp.table.value[0], 0, 6);
        inReadRecordTmp.bTable = 0x01;

        //信息点选用福建规约中的任务方案号
        inReadRecordTmp.recordOAD.infoNum = inReadRecord.recordOAD.infoNum;

        //次OAD填具体任务号
        inReadRecordTmp.recordOAD.road.oadCols.oad[0].value = relyInfo.nodeid[j];

        if (slicing_is_comtinue_node_fujian(relyInfo.bpBreakInfo, j))
        {
            continue;
        }

        /*开始读记录*/
        //开始读需要使用福建特殊接口
        ret = db_read_record_start(hUdp, &inReadRecordTmp, &handle, &recNum);
        if (ret != ERR_OK)
        {
            PRTL_FMT_DEBUG("福建规约，开始读记录出错 ret[%d] infoNum[%d] nodeId[%d] recNum[%d]\n",
                ret, inReadRecordTmp.recordOAD.infoNum, inReadRecordTmp.recordOAD.road.oadCols.oad[0].value, recNum);

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
            //3、当召测的对象有任务信息没有执行记录时，ERR-CODE内容填“任务未执行”，终端抄表时
            //   间填2099年12月31日23：59：59，召测时有提供的信息填提供的信息，其他未提供的信息填0。
            //4、当召测的对象无对应具体任务信息时，ERR-CODE填“任务未配置”，终端抄表时间填2099年12月
            //   31日23：59：59，召测时有提供的信息填提供的信息，其他未提供的信息填0。
            //5、未配置任务方案时，按无数据响应。
            PRTL_FMT_DEBUG("福建规约，开始读记录为空 ret[%d] infoNum[%d] recNum[%d]\n",
                ret, inReadRecordTmp.recordOAD.infoNum, recNum);
            if (bTaskExist)
            {
                //存在任务方案，需要按上述注释需求，虚拟数据
                recNum = 1;
                bVirtualData = TRUE;
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
            PRTL_FMT_LOGCC("福建规约，开始读记录成功 ret[%d] infoNum[%d] nodeId[%d] recNum[%d]\n",
                ret, inReadRecordTmp.recordOAD.infoNum, inReadRecordTmp.recordOAD.road.oadCols.oad[0].value, recNum);
        }

        /*循环单步读操作*/
        int i = 0;
        int k = 0;

        //针对福建特殊开始读接口，MAC填空
        stepReadRecord.MAC.nNum = 0x06;
        memset(&stepReadRecord.MAC.value[0], 0, 6);
        PRTL_BUF_LOGCC(&stepReadRecord.MAC.value[0], 6, "单步读的MAC校验: ");
        for (i = 0; i < recNum; i++)
        {
            //虚拟数据和数据中心读到的数据，分开处理，虚拟数据不需要检查数据有效性
            if (recNum == 1 && bVirtualData == TRUE)
            {
                bVirtualData = FALSE;
                memset(&stepRecordOut, 0, sizeof(stepRecordOut));
                ret = db_read_record_virtualData_by_node(hUdp, relyInfo.nodeid[j], inReadRecord.recordOAD.infoNum, &task, 
                    &stepRecordOut);
                if (ret != ERR_OK)
                {
                    PRTL_FMT_DEBUG("福建规约，组织虚拟数据出错err[%d]\n", ret);
                    continue;
                }
            }
            else
            {
                ret = db_read_record_step(hUdp, handle, &stepReadRecord, &stepRecordOut);
                if (ret != ERR_OK)
                {
                    PRTL_FMT_DEBUG("福建规约，单步读出错err[%d]\n", ret);
                    continue;
                }

                /* 跳过断点前已经处理过的单步信息 */
                if (i < index)
                {
                    PRTL_FMT_LOGCC("跳过断点前已经处理过的单步信息 %d < index index = %d!\n", i, index);
                    continue;
                }
                else
                {
                    index = 0;
                }

                /* 判断是否是所需记录单元 */
                ret = record_data_is_valid_trans(inReadRecordTmp, stepRecordOut);
                if (ret < ERR_OK)
                {
                    continue;
                }
            }

            for (k = 0; k < stepRecordOut.recordnum; k++)
            {
                ret = transparent_results_to_698buf_malloc(stepRecordOut, oopRcsdtmp, NULL, k, &pBufTmp, &bufLenTmp);
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

                    /* 拷贝数据域内容到recordBufTmp */
                    memcpy(&recordBufTmp[offset], pBufTmp, bufLenTmp);
                    offset += bufLenTmp;

                    bufAllLenLast = bufAllLen;
                    bufAllLen += offset;

                    // 方法52实时记下断点信息
                    outRecordBuf->breakInfo.node_cnt = j;
                    outRecordBuf->breakInfo.index = i;

                    len = offset;

                    if (bufAllLen <= maxBufLen)
                    {
                        memcpy(&outRecordBuf->recordBuf[bufAllLenLast], recordBufTmp, offset);
                        memset(&recordBufTmp[0], 0x00, RECORD_LEN_MAX);
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

            //如果达到最大长度，跳出循环
            if ((isBufFull) && (bufAllLenLast > 0))
            {
                break;
            }
        }

        //如果某个具体任务(node)，完整读出来，index断点应为0。
        if (i == recNum)
        {
            outRecordBuf->breakInfo.index = 0;
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
            break;
        }

        /*结束读记录*/
        db_read_record_end(hUdp, handle);
    }

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
int record_transparent_results_read(DB_CLIENT hUdp, const READ_RECORD_T inReadRecord, RELY_INFO_T relyInfo, uint32 maxBufLen, 
    RECORD_BUF_T* outRecordBuf)
{
    if (relyInfo.nodeidNum > 0)
    {
        //福建规约，带具体任务的筛选方法，方法52
        return record_transparent_results_read_node(hUdp, inReadRecord, relyInfo, maxBufLen, outRecordBuf);
    }
    else
    {
        if (relyInfo.isAllmeter != 1)
        {
            //福建规约，带MS集合的筛选方法，方法51、53、54
            return record_transparent_results_read_ms(hUdp, inReadRecord, relyInfo, maxBufLen, outRecordBuf);
        }
        else
        {
            //福建规约，带具体任务的筛选方法，方法51、53、54，全部地址
            return record_transparent_results_read_all_ms(hUdp, inReadRecord, relyInfo, maxBufLen, outRecordBuf);
        }
    }

    return ERR_NORMAL;
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
OOP_DAR_E class11_invoke_get_transparent_results_rec(OAD_INFO_T* pOadInfo, APDU_INFO_T* pApduInfo, const uint8* inData, 
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
    if (pOadInfo->pTab->oad.value != 0x6E150200)
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
        PRTL_FMT_DEBUG("头指针没用实际的RCSD\n");
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
    csdLen = offset - csdStart;
    memcpy(outData->pResultData, inData, 4);
    outOffset = 4;
    memcpy(&outData->pResultData[outOffset], &inData[csdStart], csdLen);
    outOffset += csdLen;
    outData->pResultData[outOffset++] = 1;
    outData->resultDataLen += outOffset;
    headLen = outOffset;
    PRTL_BUF_LOGCC(&outData->pResultData[0], headLen, "填写OAD+RCSD+记录结果[1] 并记下偏移headLen head:");

    /*给读透明结果集口提供输入参数*/
    pCSD = pCSDHead->next;
    nRet = read_transparent_results_param_get(pRSD, pCSD, pApduInfo, &inReadRecord, &relyInfo);
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
    char strT[128] = {0};
    uint32 tmpTime = inReadRecord.cStart - COMPENSATION_TIME;
    strftime(strT, sizeof(strT), "%Y-%m-%d %H:%M:%S", localtime((time_t*)&tmpTime));
    PRTL_FMT_LOGCC("inReadRecord.cStart:%s time[s]:%d\n", strT, inReadRecord.cStart);
    tmpTime = inReadRecord.cEnd - COMPENSATION_TIME;
    strftime(strT, sizeof(strT), "%Y-%m-%d %H:%M:%S", localtime((time_t*)&tmpTime));
    PRTL_FMT_LOGCC("inReadRecord.cEnd:%s time[s]:%d\n", strT, inReadRecord.cEnd);
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
    if (record_transparent_results_read(pApduInfo->hUdp, inReadRecord, relyInfo, 
        pApduInfo->connectionInfo.ApplSplitLen - headLen - cal_excludeLen(pApduInfo->connectionInfo), &outRecordBuf))
    {
        /*清除断点信息*/
        PRTL_FMT_DEBUG("record_task_table_read err[%d]\n", DATA_READ_WRITE_DENIED);
        memset(pApduInfo->bpBreakInfo, 0, sizeof(BP_INFO_T));
        dar = DATA_READ_WRITE_DENIED;
    }
    else
    {
        if (outRecordBuf.bufLen == 1)
        {
            //存在任务方案，但未配置具体任务，需要组织特殊报文
            nRet = trans_result_null_proc(pApduInfo->hUdp, inReadRecord.recordOAD.infoNum, relyInfo, outRecordBuf.recordBuf, 
                &outRecordBuf.bufLen);
            if (nRet != ERR_OK || outRecordBuf.bufLen == 1)
            {
                /*空数据标记，供上报使用*/
                pApduInfo->isDateNull = 1;
            }
        }
                
        memcpy(&outData->pResultData[headLen], outRecordBuf.recordBuf, outRecordBuf.bufLen);
        outData->resultDataLen += outRecordBuf.bufLen;

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
* @name：      class11_invoke_get_transparent_results
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
OOP_DAR_E class11_invoke_get_transparent_results(OAD_INFO_T* pOadInfo, APDU_INFO_T* pApduInfo, const uint8* inData, 
    uint16 inLen, RESULT_DATA_T* outData, uint16 outDataMax)
{
    if (pOadInfo->pTab->oad.value != 0x6e150200)
    {
        PRTL_FMT_DEBUG("读透明结果集合错误，主OAD选择错误 oad[0x%08x]", pOadInfo->pTab->oad.value);
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
        //return class11_invoke_get_transparent_results_nor(pOadInfo, pApduInfo, inData, inLen, outData, outDataMax);
        break;
    case 0x03:
    case 0x04:
        return class11_invoke_get_transparent_results_rec(pOadInfo, pApduInfo, inData, inLen, outData, outDataMax);
        break;
    default:
        break;
    }

    return DATA_OBJECT_UNDEFINED;
}
