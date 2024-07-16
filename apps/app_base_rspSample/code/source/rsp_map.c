/*
*********************************************************************
* Copyright(C) 2021 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   无线远程拨号模块主线程功能
* @date：    2022-3-20
* @history： 
*********************************************************************
*/
#include "baseCommon.h"
#include "hal.h"
#include "uart.h"
#include "oopType.h"
#include "appmsg_split.h"
#include "framecomm.h"
#include "task_base.h"
#include "mapmsg_split.h"

#include "rsp_mqtt.h"
#include "rsp_data.h"
#include "rsp_module.h"
#include "rsp_map.h"


/**********************************************************************
* @name      : rsp_mapmodel_choose
* @brief     ：功能模组筛选
* @param[in] ：RSP_DATA_T *pRspData    模组信息
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2022-6-17
* @Update    :
**********************************************************************/
void rsp_mapmodel_choose(RSP_DATA_T *pRspData)
{
    uint32 i = 0;
    uint32 j = 0;
    uint8  slot[2] = {0};
    uint8  num = 0;
    OOP_MODULE_INFO_NEW_T  *pModelInfo = &pRspData->modelInfo;
    
    if(FALSE == pRspData->bGetModel)
    {
        pRspData->tRspModel[0].slot = 0;
        pRspData->tRspModel[1].slot = 0;
        return;
    }

    for(i = 0; i < OOP_MAX_MODULE; i++)
    {
        if(0 == pModelInfo->item[i].nChNum)
        {
            continue;
        }
        for(j = 0; j < pModelInfo->item[i].nChNum; j++)
        {
            if(RSP_MAP_FUNC_YM == pModelInfo->item[i].chInfo[j].nFunc)
            {
                slot[num] = pModelInfo->item[i].nSlot;
                num++;
                break;
            }
        }
        if(num >= 2)
        {
            break;
        }
    }

    if(0 == num)
    {
        pRspData->tRspModel[0].slot = 0;
        pRspData->tRspModel[0].bWorkAgain = TRUE;
        pRspData->tRspModel[1].slot = 0;
        pRspData->tRspModel[1].bWorkAgain = TRUE;
        RSP_FMT_DEBUG("无遥信脉冲模组\n");
    }
    else if(1 == num)
    {
        if(pRspData->tRspModel[0].slot == slot[0])
        {
            if(0 != pRspData->tRspModel[1].slot)
            {
                RSP_FMT_DEBUG("遥信脉冲设备2模组拔出\n");
            }
            pRspData->tRspModel[1].slot = 0;
            pRspData->tRspModel[1].bWorkAgain = TRUE;
        }
        else if(pRspData->tRspModel[1].slot == slot[0])
        {
            if(0 != pRspData->tRspModel[0].slot)
            {
                RSP_FMT_DEBUG("遥信脉冲设备1模组拔出\n");
            }
            pRspData->tRspModel[0].slot = 0;
            pRspData->tRspModel[0].bWorkAgain = TRUE;
        }
        else
        {
            pRspData->tRspModel[0].slot = slot[0];
            pRspData->tRspModel[0].bWorkAgain = TRUE;
            pRspData->tRspModel[1].slot = 0;
            pRspData->tRspModel[1].bWorkAgain = TRUE;

            RSP_FMT_DEBUG("遥信脉冲设备1分配模组 %d\n", slot[0]);
        }
    }
    else if(2 == num)
    {
        if(pRspData->tRspModel[0].slot != slot[0])
        {
            RSP_FMT_DEBUG("遥信脉冲设备1分配模组 %d\n", slot[0]);
            pRspData->tRspModel[0].slot = slot[0];
            pRspData->tRspModel[0].bWorkAgain = TRUE;
        }
        if(pRspData->tRspModel[1].slot != slot[1])
        {
            RSP_FMT_DEBUG("遥信脉冲设备2分配模组 %d\n", slot[1]);
            pRspData->tRspModel[1].slot = slot[1];
            pRspData->tRspModel[1].bWorkAgain = TRUE;
        }
    }
}

/**********************************************************************
* @name      : rsp_mapversion_msg
* @brief     ：获取管理器版本
* @param[in] ：MESSAGE_INFO_T *pInMsg 请求消息
               void *pArg             句柄
               void *pPublic          公共数据
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2022-6-17
* @Update    :
**********************************************************************/
int rsp_mapversion_msg(MESSAGE_INFO_T *pInMsg, void *pArg, void *pPublic)
{
    RSP_DATA_T      *pRspData = (RSP_DATA_T*)pPublic;
    
    if(NULL == pInMsg || NULL == pArg)
    {
        return -1;
    }
    
    if(NULL != strstr(pInMsg->souraddr, "mapManager"))
    {
        char             version[64] = {0};
        uint8 *temp = pInMsg->playload;
        uint8  len = *temp;
        temp++;
        memcpy(version, temp, MIN(len, 63));
        MQTTMSG_FMT_DEBUG("获取到模组管理器, 版本%s\n", version);
        pRspData->bMapReg = TRUE;
    }
    
    return 0;
}

/**********************************************************************
* @name      : rsp_map_event
* @brief     ：获取模组插拔事件
* @param[in] ：MESSAGE_INFO_T *pInMsg 请求消息
               void *pArg             句柄
               void *pPublic          公共数据
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2022-6-17
* @Update    :
**********************************************************************/
int rsp_map_event(MESSAGE_INFO_T *pInMsg, void *pArg, void *pPublic)
{
    RSP_DATA_T      *pRspData = (RSP_DATA_T*)pPublic;
    uint8           *temp = pInMsg->playload;
    uint16           offset = 0;
    OOP_MODULE_UINT_NEW_T   tmodel;
    int              no = 0;
    uint8            type = 0;
    char             strtemp[512] = {0};
    uint16           stroff = 0;
    uint32           i = 0;
    
    memset(&tmodel, 0, sizeof(OOP_MODULE_UINT_NEW_T));
    MEMCPY_R(&no, temp, 4);
    offset += 4;
    if(0 == no || no > 5)
    {
        return 0;
    }

    tmodel.nSlot = no;
    type = temp[offset++];
    if(0 == type)
    {
        tmodel.modType.nNum = temp[offset++];
        memcpy(tmodel.modType.value, temp + offset, tmodel.modType.nNum);
        offset += tmodel.modType.nNum;
        
        tmodel.modId.nNum = temp[offset++];
        memcpy(tmodel.modId.value, temp + offset, tmodel.modId.nNum);
        offset += tmodel.modId.nNum;
        
        tmodel.nSoftVer.nNum = temp[offset++];
        memcpy(tmodel.nSoftVer.value, temp + offset, tmodel.nSoftVer.nNum);
        offset += tmodel.nSoftVer.nNum;

        tmodel.softDate.nNum = temp[offset++];
        memcpy(tmodel.softDate.value, temp + offset, tmodel.softDate.nNum);
        offset += tmodel.softDate.nNum;

        tmodel.nHardVer.nNum = temp[offset++];
        memcpy(tmodel.nHardVer.value, temp + offset, tmodel.nHardVer.nNum);
        offset += tmodel.nHardVer.nNum;

        tmodel.hardDate.nNum = temp[offset++];
        memcpy(tmodel.hardDate.value, temp + offset, tmodel.hardDate.nNum);
        offset += tmodel.hardDate.nNum;

        tmodel.manuf.nNum = temp[offset++];
        memcpy(tmodel.manuf.value, temp + offset, tmodel.manuf.nNum);
        offset += tmodel.manuf.nNum;

        tmodel.nChNum = temp[offset++];
        for(i = 0; i < tmodel.nChNum; i++)
        {
            tmodel.chInfo[i].nType = temp[offset++];
            tmodel.chInfo[i].nFunc = temp[offset++];
            tmodel.chInfo[i].nDriveName.nNum = temp[offset++];
            memcpy(tmodel.chInfo[i].nDriveName.value, temp + offset, tmodel.chInfo[i].nDriveName.nNum);
            offset += tmodel.chInfo[i].nDriveName.nNum;

            stroff += sprintf(strtemp + stroff, "  通道%d, 类型%d, 功能%d\n", i, tmodel.chInfo[i].nType,
                                                                              tmodel.chInfo[i].nFunc);
        }
        RSP_FMT_DEBUG("模组%d插入, 型号: %s, ID: %s, 厂商: %s. 通道:\n%s", no,
                       tmodel.modType.value, tmodel.modId.value, tmodel.manuf.value, strtemp);
    }
    else
    {
        RSP_FMT_DEBUG("模组%d拔出\n", no);
    }

    /* 模组筛选 */
    if(TRUE == pRspData->bGetModel)
    {
        
        pRspData->modelInfo.item[no -1] = tmodel;
        rsp_mapmodel_choose(pRspData);
    }

    return 0;
}

/**********************************************************************
* @name      : rsp_mapall_msg
* @brief     ：获取模组所有信息
* @param[in] ：MESSAGE_INFO_T *pInMsg 请求消息
               void *pArg             句柄
               void *pPublic          公共数据
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2022-6-17
* @Update    :
**********************************************************************/
int rsp_mapall_msg(MESSAGE_INFO_T *pInMsg, void *pArg, void *pPublic)
{
    RSP_DATA_T      *pRspData = (RSP_DATA_T*)pPublic;
    uint8           *temp = pInMsg->playload;
    uint16           offset = 0;
    uint32           i = 0;
    uint32           j = 0;
    uint8            type = 0;
    OOP_MODULE_INFO_NEW_T   tmodelInfo;
    char             strtemp[512] = {0};
    uint16           stroff = 0;
    
    memset(&tmodelInfo, 0, sizeof(OOP_MODULE_INFO_NEW_T));

    tmodelInfo.nNum = temp[offset++];
    if(tmodelInfo.nNum > OOP_MAX_MODULE)
    {
        tmodelInfo.nNum = OOP_MAX_MODULE;
    }
    for(i = 0; i < tmodelInfo.nNum; i++)
    {
        tmodelInfo.item[i].nSlot = i + 1;
        type = temp[offset++];
        if(0 != type)
        {
            continue;
        }
        tmodelInfo.item[i].modType.nNum = temp[offset++];
        memcpy(tmodelInfo.item[i].modType.value, temp + offset, tmodelInfo.item[i].modType.nNum);
        offset += tmodelInfo.item[i].modType.nNum;
        
        tmodelInfo.item[i].modId.nNum = temp[offset++];
        memcpy(tmodelInfo.item[i].modId.value, temp + offset, tmodelInfo.item[i].modId.nNum);
        offset += tmodelInfo.item[i].modId.nNum;
        
        tmodelInfo.item[i].nSoftVer.nNum = temp[offset++];
        memcpy(tmodelInfo.item[i].nSoftVer.value, temp + offset, tmodelInfo.item[i].nSoftVer.nNum);
        offset += tmodelInfo.item[i].nSoftVer.nNum;

        tmodelInfo.item[i].softDate.nNum = temp[offset++];
        memcpy(tmodelInfo.item[i].softDate.value, temp + offset, tmodelInfo.item[i].softDate.nNum);
        offset += tmodelInfo.item[i].softDate.nNum;

        tmodelInfo.item[i].nHardVer.nNum = temp[offset++];
        memcpy(tmodelInfo.item[i].nHardVer.value, temp + offset, tmodelInfo.item[i].nHardVer.nNum);
        offset += tmodelInfo.item[i].nHardVer.nNum;

        tmodelInfo.item[i].hardDate.nNum = temp[offset++];
        memcpy(tmodelInfo.item[i].hardDate.value, temp + offset, tmodelInfo.item[i].hardDate.nNum);
        offset += tmodelInfo.item[i].hardDate.nNum;

        tmodelInfo.item[i].manuf.nNum = temp[offset++];
        memcpy(tmodelInfo.item[i].manuf.value, temp + offset, tmodelInfo.item[i].manuf.nNum);
        offset += tmodelInfo.item[i].manuf.nNum;

        tmodelInfo.item[i].nChNum = temp[offset++];
        for(j = 0; j < tmodelInfo.item[i].nChNum; j++)
        {
            tmodelInfo.item[i].chInfo[j].nType = temp[offset++];
            tmodelInfo.item[i].chInfo[j].nFunc = temp[offset++];
            tmodelInfo.item[i].chInfo[j].nDriveName.nNum = temp[offset++];
            memcpy(tmodelInfo.item[i].chInfo[j].nDriveName.value, temp + offset, tmodelInfo.item[i].chInfo[j].nDriveName.nNum);
            offset += tmodelInfo.item[i].chInfo[j].nDriveName.nNum;

            stroff += sprintf(strtemp + stroff, "  通道%d, 类型%d, 功能%d, 设备%s\n", j, tmodelInfo.item[i].chInfo[j].nType,
                                                                              tmodelInfo.item[i].chInfo[j].nFunc,
                                                                              tmodelInfo.item[i].chInfo[j].nDriveName.value);
        }
        if(FALSE == pRspData->bGetModel)
        {
            RSP_FMT_DEBUG("模组%d, 型号: %s, ID: %s, 厂商: %s. 通道:\n%s", tmodelInfo.item[i].nSlot,
                           tmodelInfo.item[i].modType.value, tmodelInfo.item[i].modId.value, tmodelInfo.item[i].manuf.value,
                           strtemp);
        }
        memset(strtemp, 0, 512);
        stroff = 0;
    }

    memcpy(&pRspData->modelInfo, &tmodelInfo, sizeof(OOP_MODULE_INFO_NEW_T));
    pRspData->bGetModel = TRUE;

    /* 模组筛选 */
    rsp_mapmodel_choose(pRspData);

    return 0;
}

/**********************************************************************
* @name      : rsp_get_mapManager
* @brief     ：获取模组管理器
* @param[in] ：void *pPublic    公有数据
* @param[out]：void **ppPrivate 私有数据
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2022-6-17
* @Update    :
**********************************************************************/
void rsp_get_mapManager()
{
    int   ret = 0;
    ret =  rsp_send_mapmsg(0, MESSAGE_IID_COMMON, COMMON_IOP_VERSION, NULL, 0);
    MQTTMSG_FMT_TRACE("发送查询模组管理器版本, 结果%d\n", ret);
}

/**********************************************************************
* @name      : rsp_get_mapModule
* @brief     ：获取模组管理器
* @param[in] ：void *pPublic    公有数据
* @param[out]：void **ppPrivate 私有数据
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2022-6-17
* @Update    :
**********************************************************************/
void rsp_get_mapModule()
{
    int   ret = 0;
    ret = rsp_send_mapmsg(1, MESSAGE_IID_MODULE, MODULE_IOP_GET_ALL, NULL, 0);
    if(0 != ret)
    {
        MQTTMSG_FMT_TRACE("发送查询所有模组信息, 结果%d\n", ret);
    }
}

/**********************************************************************
* @name      : rsp_map_init
* @brief     ：模组管理器初始化
* @param[in] ：void *pPublic    公有数据
* @param[out]：void **ppPrivate 私有数据
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2022-6-17
* @Update    :
**********************************************************************/
int rsp_map_init(void *pPublic, void **ppPrivate)
{
    RSP_DATA_T      *pRspData = (RSP_DATA_T*)pPublic;

    if(FALSE == pRspData->bMAP)
    {
        while(1)
        {
            sleep(1);
        }
    }
    RSP_FMT_DEBUG("存在模组管理器\n");
    memset(&(pRspData->modelInfo), 0, sizeof(OOP_MODULE_INFO_NEW_T));
    *ppPrivate = &(pRspData->modelInfo);
    return 0;
}

/**********************************************************************
* @name      : rsp_map_proc
* @brief     ：模组管理器处理
* @param[in] ：void *pArg     线程句柄
               void *pPublic  公有数据
               void *pPrivate 私有数据
* @param[out]：
* @return    ：延时时间 us
* @Create    : 王津剑
* @Date      ：2022-6-17
* @Update    :
**********************************************************************/
int rsp_map_proc(void *pArg, void *pPublic, void *pPrivate)
{
    static uint32    gettime = 0;
    RSP_DATA_T      *pRspData = (RSP_DATA_T*)pPublic;
    struct timespec curtime = {0};

    /* 查询模组管理器 */
    if(FALSE == pRspData->bMapReg)
    {
        rsp_get_mapManager();
        sleep(10);
        return 10000;
    }

    clock_gettime(CLOCK_MONOTONIC, &curtime);

    /* 查询模组*/
    if(FALSE == pRspData->bGetModel || curtime.tv_sec - gettime >= 69)
    {
        rsp_get_mapModule();
        gettime = curtime.tv_sec;
        sleep(10);
        return 10000;
    }
    
    return 100000;
}

/**********************************************************************
* @name      : rsp_model_proc_msg
* @brief     ：遥信脉冲模组消息解析
* @param[in] ：MAPMSG_INFO_T *pMapMsg 模组消息
               RSP_MODEL_T  *ptHandle 线程句柄
               RSP_DATA_T *pRspData   进程数据
* @param[out]：
* @return    ：处理消息长度
* @Create    : 王津剑
* @Date      ：2022-6-16
* @Update    :
**********************************************************************/
uint16 rsp_model_proc_msg(MAPMSG_INFO_T *pMapMsg, RSP_MODEL_T *ptHandle, RSP_DATA_T *pRspData)
{
    int    ret = 0;
    uint8 *temp = NULL;
    uint8  type = 0;
    uint16 dt = 0;
    char  *dir[3] = {"成功", "拒绝读写", "信息类未定义"};
    
    if(MAPMSG_CTR_MSG != pMapMsg->cmt.cmd || pMapMsg->len < 4)
    {
        return pMapMsg->len + 8;
    }
    
    type = pMapMsg->apdu[0];
    temp = pMapMsg->apdu + 1;
    MEMCPY_R(&dt, temp, sizeof(uint16));
    temp += sizeof(uint16);
    switch(dt)
    {
        case MAPMSG_DT_PULSE_STATUS:
        {
            RSP_RMT_D_T  tRmtTemp;
            RSP_REPORT_DATA_T tNode;
            uint8  i = 0;
            
            /* 遥信数据 */
            ret = rsp_proc_msg_rmt(pMapMsg->apdu, pMapMsg->len, &tRmtTemp);
            if(0 != ret)
            {
                RSP_BUF_DEBUG(pMapMsg->apdu, pMapMsg->len, "本体错误的APDU, type %d, DT %04X\n", type, dt);
            }
            else
            {
                /* 记录变位信息 */
                PTHREAD_LOCK_SAFE(&ptHandle->flock);
                tNode.id = ptHandle->no;
                tNode.sum = tRmtTemp.stateNum;
                memcpy(tNode.config, ptHandle->config, RSP_PORT_MAX);
                memcpy(tNode.ST, tRmtTemp.ST, RSP_PORT_MAX);
                memcpy(tNode.CT, tRmtTemp.CT, RSP_PORT_MAX);
                memcpy(tNode.chgTime, tRmtTemp.changeTime, RSP_PORT_MAX * sizeof(OOP_DATETIME_T));

                memcpy(ptHandle->storeState, tRmtTemp.ST, RSP_PORT_MAX);
                if(ptHandle->bFirstRun == TRUE)
                {
                    memcpy(&ptHandle->curNode, &tNode, sizeof(RSP_REPORT_DATA_T));
                }
                else if(MAPMSG_APDU_ReportNotification == type)
                {
                    memcpy(&ptHandle->oldNode, &ptHandle->lastNode, sizeof(RSP_REPORT_DATA_T));
                    memcpy(&ptHandle->lastNode, &ptHandle->curNode, sizeof(RSP_REPORT_DATA_T));
                    memcpy(&ptHandle->curNode, &tNode, sizeof(RSP_REPORT_DATA_T));
                }
                tNode.bReport = MAPMSG_APDU_ReportNotification == type ? TRUE : FALSE;
                PTHREAD_UNLOCK_SAFE(&ptHandle->flock);
                
                if(MAPMSG_APDU_ReportNotification == type || ptHandle->bFirstRun == TRUE)
                {
                    char tempstr[512] = {0};
                    uint16 offset = 0;
                    /* 汇总 */
                    for(i = 0; i < tRmtTemp.stateNum; i++)
                    {
                        offset += sprintf(tempstr + offset, "%d-%d, %04d-%02d-%02d %02d:%02d:%02d.%03d\n",
                                          tRmtTemp.ST[i], tRmtTemp.CT[i], RSP_PRINT_TIME(&(tRmtTemp.changeTime[i])));
                    }
                    RSP_FMT_DEBUG("设备%d遥信状态%s, 数量 %d: \n%s", ptHandle->no, 
                                   TRUE == tNode.bReport ? "上报" : "首次获取", tRmtTemp.stateNum, tempstr);
                    ptHandle->bFirstRun = FALSE;
                    tNode.bReport = TRUE;
                    rsp_write_queue(&pRspData->reportList, &tNode);
                }
            }
            break;
        }
        case MAPMSG_DT_PULSE_TOTALDATA:
        {
            RSP_PULSE_A_T  tPulseATemp;
            ret = rsp_proc_msg_pulasea(pMapMsg->apdu, pMapMsg->len, &tPulseATemp);
            if(0 == ret)
            {
                PTHREAD_LOCK_SAFE(&ptHandle->flock);
                memcpy(&ptHandle->tPulseAll, &tPulseATemp, sizeof(RSP_PULSE_A_T));
                PTHREAD_UNLOCK_SAFE(&ptHandle->flock);
            }
            else
            {
                RSP_BUF_DEBUG(pMapMsg->apdu, pMapMsg->len, "错误的APDU, type %d, DT %04X\n", type, dt);
            }
            break;
        }
        case MAPMSG_DT_PULSE_SET:
        {
            if(MAPMSG_APDU_SetResponse == type && ptHandle->configindex == pMapMsg->cmt.fid)
            {
                ptHandle->configStatus = 0 == *temp ? 0 : 2;
                RSP_FMT_DEBUG("%d设置遥信脉冲配置%s\n", ptHandle->no, *temp < 3 ? dir[*temp] : "失败, 其他原因");
            }
            break;
        }
        case MAPMSG_DT_PULSE_SETTIME:
        {
            if(MAPMSG_APDU_SetResponse == type && ptHandle->shakeindex == pMapMsg->cmt.fid)
            {
                ptHandle->shakeStatus = 0 == *temp ? 0 : 2;
                RSP_FMT_DEBUG("%d设置遥信防抖时间%s\n", ptHandle->no, *temp < 3 ? dir[*temp] : "失败, 其他原因");
            }
            break;
        }
        default:
            break;
    }
    
    return pMapMsg->len + 8;
}


/**********************************************************************
* @name      : rsp_model_work
* @brief     ：遥信脉冲模组工作
* @param[in] ：RSP_DEV_T  *ptHandle    线程句柄
               RSP_DATA_T *pRspData   进程数据
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-7-26
* @Update    :
**********************************************************************/
int rsp_model_work(RSP_MODEL_T *ptHandle, RSP_DATA_T *pRspData)
{
    struct timeval   curtime = {0};
    struct timezone  tz;
    struct tm        systime;
    int     ret = 0;
    MAPMSG_INFO_T *pMapMsg = NULL;
    uint16  offset = 0;
    uint8  *temp = NULL;
    uint16  len = 0;
    
    gettimeofday(&curtime, &tz); //获取系统时间
    FastSecondToDate(&curtime, &tz, &systime);

    /* 查询脉冲 */
    if(curtime.tv_sec != ptHandle->timeflag.tv_sec)
    {
        ptHandle->timeflag.tv_sec = curtime.tv_sec;
        ptHandle->timeflag.tv_usec = 800000;
        if(TRUE == ptHandle->bSet)
        {
            if(1 == ptHandle->config[0] || 1 == ptHandle->config[1] || 1 == ptHandle->config[2] || 1 == ptHandle->config[3]||
               1 == ptHandle->config[4] || 1 == ptHandle->config[5] || 1 == ptHandle->config[6] || 1 == ptHandle->config[7])
            {
                ret = rsp_send_get_comm(ptHandle->fd, ptHandle->index++, MAPMSG_DT_PULSE_TOTALDATA);
                if(0 != ret)
                {
                    return ret;
                }
                goto recv;
            }
            goto recv;
        }
    }

    /* 查询遥信状态 */
    if(curtime.tv_usec >= ptHandle->timeflag.tv_usec && 0 != ptHandle->sum)
    {
        ptHandle->timeflag.tv_usec = 9000000;
        ret = rsp_send_get_comm(ptHandle->fd, ptHandle->index++, MAPMSG_DT_PULSE_STATUS);
        if(0 != ret)
        {
            return ret;
        }
        goto recv;
    }

    /* 设置功能 */
    if(1 == ptHandle->configStatus && TRUE == ptHandle->bSet)
    {
        ptHandle->configindex = ptHandle->index++;
        PTHREAD_LOCK_SAFE(&ptHandle->flock);
        ret = rsp_send_set_config(ptHandle->fd, ptHandle->configindex, ptHandle->config, RSP_PORT_MAX);
        PTHREAD_UNLOCK_SAFE(&ptHandle->flock);
        ptHandle->configStatus = 0 == ret ? 3 : 2;
        if(0 != ret)
        {
            return ret;
        }
        goto recv;
    }

    /* 遥信防抖时间设置 */
    if(1 == ptHandle->shakeStatus)
    {
        ptHandle->shakeindex = ptHandle->index++;
        PTHREAD_LOCK_SAFE(&ptHandle->flock);
        ret = rsp_send_set_shake(ptHandle->fd, ptHandle->shakeindex, ptHandle->shakeTime);
        PTHREAD_UNLOCK_SAFE(&ptHandle->flock);
        ptHandle->shakeStatus = 0 == ret ? 3 : 2;
        if(0 != ret)
        {
            return ret;
        }
        goto recv;
    }

recv:
    /* 接收处理 */
    ret = rsp_uart_recv(ptHandle->fd, ptHandle->recv + ptHandle->recvlen, RSP_BUF_LENGTH - ptHandle->recvlen, 20, 10);
    if(ret > 0)
    {
        ptHandle->recvlen += ret;
        if(ptHandle->recvlen > RSP_BUF_LENGTH)
        {
            ptHandle->recvlen = RSP_BUF_LENGTH;
        }
        RSP_BUF_TRACE(ptHandle->recv, ptHandle->recvlen, "模组接收报文");
        temp = ptHandle->recv;
        while(offset < ptHandle->recvlen)
        {
            if(FRAMESTART == temp[offset])
            {
                ret = mapmsg_split(temp + offset, ptHandle->recvlen - offset, &pMapMsg);
                if(ret == 0 && NULL != pMapMsg)
                {
                    offset += rsp_model_proc_msg(pMapMsg, ptHandle, pRspData);
                    offset--;
                    free(pMapMsg);
                    pMapMsg = NULL;
                }
                else if(offset + 4 < ptHandle->recvlen)
                {
                    /* 非全消息判断 消息长度小于256 */
                    memcpy(&len, temp + offset + 1, 2);
                    if(len <= 256 && offset + 8 + len > ptHandle->recvlen)
                    {
                        break;
                    }
                }
            }
            offset++;
        }
        /* 队列清理 */
        if(offset >= ptHandle->recvlen)
        {
            memset(ptHandle->recv, 0, ptHandle->recvlen);
            ptHandle->recvlen = 0;
        }
        else if(offset > 0)
        {
            memmove(ptHandle->recv, ptHandle->recv + offset, ptHandle->recvlen - offset);
            ptHandle->recvlen -= offset;
            memset(ptHandle->recv + ptHandle->recvlen, 0, RSP_BUF_LENGTH - ptHandle->recvlen);
        }
    }
    return 0;
}


/**********************************************************************
* @name      : rsp_dev_proc
* @brief     ：遥信设备检查
* @param[in] ：RSP_MODEL_T *ptHandle   线程句柄
               RSP_DATA_T *pRspData    公共数据
* @param[out]：
* @return    ：错误码
* @Create    : 王津剑
* @Date      ：2022-3-20
* @Update    :
**********************************************************************/
int rsp_model_status(RSP_MODEL_T *ptHandle, RSP_DATA_T *pRspData)
{
    switch(ptHandle->workstauts)
    {
        case RSP_STATUS_GET_DEV:
        {
            UART_DEVICE_T* dev = NULL;
            char           devName[32] = {0};

            if(ptHandle->pUart)
            {
                hal_device_release((HW_DEVICE *)ptHandle->pUart);
                ptHandle->pUart = NULL;
            }
            
            sprintf(devName, "%s%d_%d", UART_ECU_DEVICE_ID_STR, ptHandle->slot, ptHandle->channel);
            dev = (UART_DEVICE_T*)hal_device_get(devName);
            if(NULL == dev)
            {
                RSP_FMT_DEBUG("设备 %d 打开%s失败\n", ptHandle->no, devName);
                ptHandle->workstauts = RSP_STATUS_GET_DEV;
                sleep(10);
            }
            else
            {
                ptHandle->pUart = dev;
                RSP_FMT_DEBUG("设备 %d 打开%s成功\n", ptHandle->no, devName);
                ptHandle->workstauts = RSP_STATUS_INIT;
            }
            
            break;
        }
        case RSP_STATUS_INIT:
        {
            UART_DEVICE_T* dev = (UART_DEVICE_T*)ptHandle->pUart;
            char           devName[32] = {0};

            sprintf(devName, "%s%d_%d", UART_ECU_DEVICE_ID_STR, ptHandle->slot, ptHandle->channel);
            
            if(0 == dev->uart_param_set(dev, 115200, 8, 1, PARITY_NO))
            {
                RSP_FMT_DEBUG("设备 %d 设置%s成功\n", ptHandle->no, devName);
                memset(ptHandle->recv, 0, RSP_BUF_LENGTH);
                ptHandle->recvlen = 0;
                ptHandle->workstauts = RSP_STATUS_WORK;
                ptHandle->configStatus = 1;
                ptHandle->shakeStatus = 1;
                ptHandle->fd = dev->uart_get_handle(dev);
                ptHandle->bFirstRun = TRUE;
            }
            else
            {
                RSP_FMT_DEBUG("设备 %d 设置%s失败\n", ptHandle->no, devName);
                ptHandle->workstauts = RSP_STATUS_GET_DEV;
                hal_device_release((HW_DEVICE *)ptHandle->pUart);
                ptHandle->pUart = NULL;
                sleep(1);
            }
            break;
        }
        case RSP_STATUS_WORK:
        {
            if(0 != rsp_model_work(ptHandle, pRspData))
            {
                ptHandle->workstauts = RSP_STATUS_GET_DEV;
            }
            break;
        }
        default:
        {
            ptHandle->workstauts = RSP_STATUS_GET_DEV;
            break;
        }
    }
    return 0;
}


/**********************************************************************
* @name      : rsp_model1_init
* @brief     ：模组初始化
* @param[in] ：void *pPublic    公有数据
* @param[out]：void **ppPrivate 私有数据
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2022-6-17
* @Update    :
**********************************************************************/
int rsp_model1_init(void *pPublic, void **ppPrivate)
{
    RSP_DATA_T      *pRspData = (RSP_DATA_T*)pPublic;

    while(FALSE == pRspData->bMapReg)
    {
        sleep(1);
    }

    *ppPrivate = &(pRspData->tRspModel[0]);
    return 0;
}

/**********************************************************************
* @name      : rsp_model2_init
* @brief     ：模组初始化
* @param[in] ：void *pPublic    公有数据
* @param[out]：void **ppPrivate 私有数据
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2022-6-17
* @Update    :
**********************************************************************/
int rsp_model2_init(void *pPublic, void **ppPrivate)
{
    RSP_DATA_T      *pRspData = (RSP_DATA_T*)pPublic;

    while(FALSE == pRspData->bMapReg)
    {
        sleep(1);
    }

    *ppPrivate = &(pRspData->tRspModel[1]);
    return 0;
}


/**********************************************************************
* @name      : rsp_model_proc
* @brief     ：模组处理
* @param[in] ：void *pArg     线程句柄
               void *pPublic  公有数据
               void *pPrivate 私有数据
* @param[out]：
* @return    ：延时时间 us
* @Create    : 王津剑
* @Date      ：2022-6-17
* @Update    :
**********************************************************************/
int rsp_model_proc(void *pArg, void *pPublic, void *pPrivate)
{
    RSP_MODEL_T *pHandle = (RSP_MODEL_T *)pPrivate;
    RSP_DATA_T  *pRspData = (RSP_DATA_T*)pPublic;
    uint8        i = 0;
    int          ret = 0;
    
    if(0 != pHandle->slot)
    {
        pHandle->bSet = TRUE;
        /* 判断是否控制模块 */
        for(i = 0; i < pRspData->modelInfo.item[pHandle->slot - 1].nChNum; i++)
        {
            if(RSP_MAP_FUNC_CTRL == pRspData->modelInfo.item[pHandle->slot - 1].chInfo[i].nFunc)
            {
                pHandle->bSet = FALSE;
                memset(pHandle->config, 0, RSP_PORT_MAX);
            }
            if(RSP_MAP_FUNC_YM == pRspData->modelInfo.item[pHandle->slot - 1].chInfo[i].nFunc)
            {
                pHandle->channel = i;
            }
        }
        pHandle->bWorkAgain = FALSE;
        pHandle->workstauts = RSP_STATUS_GET_DEV;

        while(1)
        {
            if(0 == pHandle->slot || 0 == pHandle->channel || TRUE == pHandle->bWorkAgain)
            {
                RSP_FMT_DEBUG("设备 %d 异常, 退出\n", pHandle->no, pHandle->slot, pHandle->channel);
                break;
            }
            ret = rsp_model_status(pHandle, pRspData);
            if(0 != ret)
            {
                break;
            }
            usleep(5000);
        }
        if(pHandle->pUart)
        {
            RSP_FMT_DEBUG("设备 %d 释放句柄\n", pHandle->no);
            hal_device_release((HW_DEVICE *)pHandle->pUart);
            pHandle->pUart = NULL;
        }
    }

    return 1000000;
}



