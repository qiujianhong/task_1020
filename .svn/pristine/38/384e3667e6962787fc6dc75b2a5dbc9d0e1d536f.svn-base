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
#include "oopType.h"
#include "appmsg_split.h"
#include "framecomm.h"
#include "task_base.h"
#include "mapmsg_split.h"

#include "rsp_mqtt.h"
#include "rsp_data.h"
#include "rsp_module.h"

#define RSP_UART_NAME          "/dev/ttyYM"
#define COPT_UART_NAME         "/dev/ttyCOPR"
#define RSP_UPDATE_FILENAME    "/data/app/rspSample/update_file.bin"

/**********************************************************************
* @name      : rsp_write_doorstatus
* @brief     ：遥信脉冲新写门节点状态
* @param[in] ：uint8 statsu
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2022-6-21
* @Update    :
**********************************************************************/
void rsp_write_doorstatus(uint8 statsu)
{
    int32 fd = 0;
    int32 iRet= 0;
    struct flock lock;

    fd = open("/tmp/dev/doorstatus", O_CREAT|O_WRONLY,0666);
    if(fd < 0)
    {
        RSP_FMT_DEBUG("打开设备/tmp/dev/doorstatus失败\n");
        return;
    }

    lock.l_type = F_WRLCK;
    lock.l_start = 0;
    lock.l_whence = SEEK_SET;
    lock.l_len = 0;
    lock.l_pid = getpid();
   
    iRet = fcntl(fd, F_SETLKW, &lock);
    if(iRet != 0)
    {
        RSP_FMT_DEBUG("Get id Return value of fcntl:%d\n",iRet);
    }
    
    iRet = write(fd, &statsu, 1);
    if(iRet != 1)
    {
        RSP_FMT_DEBUG("写入状态失败\n");
    }

    lock.l_type = F_UNLCK;
    iRet = fcntl(fd, F_SETLK, &lock);
    if (iRet < 0)
    {
        RSP_FMT_DEBUG("fcntl failed msg:%s\n", strerror(errno));
    }

    close(fd);
}

/**********************************************************************
* @name      : rsp_write_ctrlstatus
* @brief     ：写控制状态
* @param[in] ：uint8 num
               uint16 status
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2022-7-29
* @Update    :
**********************************************************************/
void rsp_write_ctrlstatus(uint8 num, uint16 status)
{
    int32 fd = 0;
    int32 iRet= 0;
    uint8 date[3];
    struct flock lock;

    fd = open("/tmp/dev/ctrlstatus", O_CREAT|O_WRONLY,0666);
    if(fd < 0)
    {
        RSP_FMT_DEBUG("打开设备/tmp/dev/ctrlstatus失败\n");
        return;
    }

    lock.l_type = F_WRLCK;
    lock.l_start = 0;
    lock.l_whence = SEEK_SET;
    lock.l_len = 0;
    lock.l_pid = getpid();
   
    iRet = fcntl(fd, F_SETLKW, &lock);
    if(iRet != 0)
    {
        RSP_FMT_DEBUG("Get id Return value of fcntl:%d\n",iRet);
    }
    
    date[0] = num;
    memcpy(date + 1, &status, 2);
    iRet = write(fd, date, 3);
    if(iRet != 1)
    {
        RSP_FMT_DEBUG("写入控制状态失败\n");
    }

    lock.l_type = F_UNLCK;
    iRet = fcntl(fd, F_SETLK, &lock);
    if (iRet < 0)
    {
        RSP_FMT_DEBUG("fcntl failed msg:%s\n", strerror(errno));
    }

    close(fd);
}


/**********************************************************************
* @name      : rsp_write_version
* @brief     ：写版本号
* @param[in] ：uint8 statsu
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2022-6-21
* @Update    :
**********************************************************************/
void rsp_write_version(char *version, uint16 len)
{
    int32 fd = 0;
    int32 iRet= 0;
    struct flock lock;

    fd = open("/data/app/rspSample/data/version", O_CREAT|O_WRONLY,0666);
    if(fd < 0)
    {
        RSP_FMT_DEBUG("打开设备/data/app/rspSample/data/version失败\n");
        return;
    }

    lock.l_type = F_WRLCK;
    lock.l_start = 0;
    lock.l_whence = SEEK_SET;
    lock.l_len = 0;
    lock.l_pid = getpid();
   
    iRet = fcntl(fd, F_SETLKW, &lock);
    if(iRet != 0)
    {
        RSP_FMT_DEBUG("Get id Return value of fcntl:%d\n",iRet);
    }
    
    iRet = write(fd, version, len);
    if(iRet != len)
    {
        RSP_FMT_DEBUG("写入状态失败\n");
    }

    lock.l_type = F_UNLCK;
    iRet = fcntl(fd, F_SETLK, &lock);
    if (iRet < 0)
    {
        RSP_FMT_DEBUG("fcntl failed msg:%s\n", strerror(errno));
    }

    close(fd);
}


#if DESC("模组协议处理", 1)

/**********************************************************************
* @name      : rsp_map_work_other
* @brief     ：遥信脉冲新设备其他工作
* @param[in] ：RSP_DEV_T  *ptHandle    线程句柄
               RSP_DATA_T *pRspData   进程数据
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-11-1
* @Update    :
**********************************************************************/
int rsp_map_work_other(RSP_DEV_T *ptHandle, RSP_DATA_T *pRspData)
{
    struct timespec  spectime = {0};  //绝对时间
    struct timeval   curtime = {0};
    struct timezone  tz;
    struct tm        systime;
    int     ret = 0;
    uint16  len = 0;
    uint8  *pStartData = NULL;
    uint8   bSend = FALSE;
    
    gettimeofday(&curtime, &tz); //获取系统时间
    FastSecondToDate(&curtime, &tz, &systime);

     /* 过日判断 设置时间 */
    if(systime.tm_mday != ptHandle->lasttime.tm_mday || systime.tm_mon != ptHandle->lasttime.tm_mon ||
       systime.tm_year != ptHandle->lasttime.tm_year)
    {
        ptHandle->bSetTime = TRUE;
        ptHandle->lasttime = systime;
    }

    /* 设置时间 */
    if(TRUE == ptHandle->bSetTime)
    {   
        ret = rsp_send_set_time(pRspData->uartfd, ptHandle->index++);
        ptHandle->bSetTime = FALSE;
        return ret;
    }

    clock_gettime(CLOCK_MONOTONIC, &spectime);
    /* 升级操作 */
    if(RSP_MAP_FILE == ptHandle->mapStatus)
    {
        if(NULL == ptHandle->fileData)
        {
            ptHandle->mapStatus = RSP_MAP_CHECK;
            return 0;
        }
        if(TRUE == ptHandle->bNeedSend)
        {
            bSend = TRUE;
        }
        else
        {
            /* 判断超时 */
            int64 timepass = 0;
            timepass = (spectime.tv_sec - ptHandle->mapTime) * 1000 + spectime.tv_nsec/1000000 - ptHandle->maptimems;
            if(timepass > 5000 || timepass < 0)
            {
                UPDATE_FMT_DEBUG("序号%d传输超时, 重试!\n", ptHandle->fileNO);
                bSend = TRUE;
            }
        }

        if(TRUE == bSend)
        {
            ptHandle->mapTime = spectime.tv_sec;
            ptHandle->maptimems = (uint16)(spectime.tv_nsec/1000000);
            ptHandle->bNeedSend = FALSE;
            
            /* 传输完成判断 */
            if(ptHandle->fileNO >= ptHandle->fileMAX)
            {
                free(ptHandle->fileData);
                ptHandle->fileData = NULL;
                ptHandle->filelen = 0;
                remove(ptHandle->updatefile);
                UPDATE_FMT_DEBUG("升级完成，删除升级文件!\n");
                ptHandle->mapStatus = RSP_MAP_LINK; //从头开始
                system("touch /data/app/rspSample/data/endupdate");
                system("rm -rf /data/app/rspSample/data/startupdate");

                sleep(2);
                ptHandle->workstauts = RSP_STATUS_GET_DEV;
            }
            else
            {
                ptHandle->fileindex = ptHandle->index++;
                /* 发送数据 */
                pStartData = ptHandle->fileData + ptHandle->fileNO * ptHandle->maxData;

                //最后一帧
                if(ptHandle->fileNO == (ptHandle->fileMAX -1))
                {
                    len = ptHandle->filelen - ptHandle->fileNO * ptHandle->maxData;
                }
                else
                {
                    len = ptHandle->maxData;
                }

                return rsp_send_set_file(pRspData->uartfd, ptHandle->fileindex, ptHandle->fileNO, ptHandle->fileMAX,
                         pStartData, len);
            }
        }
    }
    else if(spectime.tv_sec > ptHandle->mapTime + 2)
    {
        ptHandle->mapTime = spectime.tv_sec;
        
        /* 关闭文件内容 */
        if(ptHandle->fileData)
        {
            free(ptHandle->fileData);
            ptHandle->fileData = NULL;
        }
        switch(ptHandle->mapStatus)
        {
            case RSP_MAP_LINK:
            {
                return rsp_send_link_comm(pRspData->uartfd, ptHandle->index++);
            }
            case RSP_MAP_INFO:
            {
                return rsp_send_get_comm(pRspData->uartfd, ptHandle->index++, MAPMSG_DT_COMM_MODINFO);
            }
            case RSP_MAP_CHECK:
            {
                if(0 == access(ptHandle->updatefile, F_OK))
                {
                    FILE  *fd = NULL;
                    /* 打开文件句柄 */
                    sleep(2); //延时等待文件拷贝
                    fd = fopen(ptHandle->updatefile, "rb");
                    if(fd < 0)
                    {
                        UPDATE_FMT_DEBUG("打开%s失败, 原因: %s\n", ptHandle->updatefile, strerror(errno));
                        break;
                    }
                    
                    fseek(fd, 0, SEEK_END);
                    ptHandle->filelen = ftell(fd);
                    ptHandle->fileData = (uint8 *)malloc(ptHandle->filelen);
                    if(NULL == ptHandle->fileData)
                    {
                        UPDATE_FMT_DEBUG("%申请内存失败, 长度: %d\n", ptHandle->updatefile, ptHandle->filelen);
                        fclose(fd);
                        break;
                    }
                    fseek(fd, 0, SEEK_SET);
                    fread(ptHandle->fileData, 1, ptHandle->filelen, fd);
                    fclose(fd);

                    /* 计算最大数量 */
                    ptHandle->fileMAX = ptHandle->filelen / ptHandle->maxData;
                    if(0 != ptHandle->filelen % ptHandle->maxData)
                    {
                        ptHandle->fileMAX += 1;
                    }
                    ptHandle->fileNO = 0;
                    ptHandle->bNeedSend = TRUE;
                    ptHandle->mapStatus = RSP_MAP_FILE;

                    system("rm -rf /data/app/rspSample/data/endupdate");
                    sleep(1);
                    system("touch /data/app/rspSample/data/startupdate");
                    system("rm -rf /data/app/rspSample/data/version");
                }
                break;
            }
            default: ptHandle->mapStatus = RSP_MAP_LINK; break;
        }
    }
    return 0;
}

/**********************************************************************
* @name      : rsp_proc_uart_msg
* @brief     ：遥信脉冲设备工作
* @param[in] ：MAPMSG_INFO_T *pMapMsg 模组消息
               RSP_DEV_T  *ptHandle   线程句柄
               RSP_DATA_T *pRspData   进程数据
* @param[out]：
* @return    ：处理消息长度
* @Create    : 王津剑
* @Date      ：2022-6-16
* @Update    :
**********************************************************************/
uint16 rsp_proc_uart_msg(MAPMSG_INFO_T *pMapMsg, RSP_DEV_T *ptHandle, RSP_DATA_T *pRspData)
{
    int    ret = 0;
    uint8 *temp = NULL;
    uint8  type = 0;
    uint16 dt = 0;
    char  *dir[3] = {"成功", "拒绝读写", "信息类未定义"};
    
    if(MAPMSG_CTR_MSG != pMapMsg->cmt.cmd || pMapMsg->len < 3)
    {
        return pMapMsg->len + 8;
    }
    
    type = pMapMsg->apdu[0];

    /* 链路协商 */
    if(MAPMSG_APDU_LinkResponse == type)
    {
        if(0 == rsp_proc_msg_linkinfo(pMapMsg->apdu, pMapMsg->len, &ptHandle->linkInfo))
        {
            ptHandle->maxData = (MAX(ptHandle->linkInfo.maxrecv, 64) - 8 - 21)/4*4;
            ptHandle->maxData = MIN(ptHandle->maxData, 256);
            RSP_FMT_DEBUG("链路协商信息:\n协议版本号: %u, 最大发送长度: %u, 最大接收长度: %u, 文件传输长度: %u\n", 
                           ptHandle->linkInfo.version, ptHandle->linkInfo.maxSend, ptHandle->linkInfo.maxrecv,
                           ptHandle->maxData);
            ptHandle->mapStatus = RSP_MAP_INFO;
        }
        return pMapMsg->len + 8;
    }
    
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
#ifdef PRODUCT_CCU
                /* 集中器超过第2路最后第1路是门节点,写入临时文件中提供hal接口 */
                if(tRmtTemp.stateNum > 2)
                {
                    uint8 p;
                    uint8 doorstatus = 0;
                    for(p = 2; p < tRmtTemp.stateNum; p++)
                    {
                        if(1 == tRmtTemp.validFlag[p])
                        {
                            doorstatus = 1 - tRmtTemp.ST[p];
                        }
                        tRmtTemp.ST[p] = 0;
                        tRmtTemp.CT[p] = 0;
                        tRmtTemp.validFlag[p] = 0;
                        memset(&(tRmtTemp.changeTime[p]), 0, sizeof(OOP_DATETIME_T));
                    }
                    tRmtTemp.stateNum = 2;
                    tRmtTemp.changeNum = 2;
                    tRmtTemp.timeNum = 2;
                    if(doorstatus != ptHandle->doorStatus)
                    {
                        RSP_FMT_DEBUG("门节点变位\n");
                    }
                    ptHandle->doorStatus = doorstatus;
                    rsp_write_doorstatus(doorstatus);

                    if(tRmtTemp.ST[0] == ptHandle->curNode.ST[0] && tRmtTemp.ST[1] == ptHandle->curNode.ST[1]
                       && 0 == memcmp(&(tRmtTemp.changeTime[0]), &(ptHandle->curNode.chgTime[0]), sizeof(OOP_DATETIME_T))
                       && 0 == memcmp(&(tRmtTemp.changeTime[1]), &(ptHandle->curNode.chgTime[1]), sizeof(OOP_DATETIME_T)))
                    {
                        RSP_FMT_TRACE("无遥信变位\n");
                        break;
                    }
                }
#endif
                /* 记录变位信息 */
                PTHREAD_LOCK_SAFE(&ptHandle->flock);
                tNode.id = 0;
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
                if(MAPMSG_APDU_ReportNotification == type)
                {
                    /* 应答帧 */
                    rsp_send_report_resp(pRspData->uartfd, pMapMsg->cmt.fid, MAPMSG_DT_PULSE_STATUS);
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
                    RSP_FMT_DEBUG("设备1遥信状态%s, 数量 %d: \n%s", 
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
                RSP_FMT_DEBUG("设置遥信脉冲配置%s\n", *temp < 3 ? dir[*temp] : "失败, 其他原因");
            }
            break;
        }
        case MAPMSG_DT_PULSE_SETTIME:
        {
            if(MAPMSG_APDU_SetResponse == type && ptHandle->shakeindex == pMapMsg->cmt.fid)
            {
                ptHandle->shakeStatus = 0 == *temp ? 0 : 2;
                RSP_FMT_DEBUG("设置遥信防抖时间%s\n", *temp < 3 ? dir[*temp] : "失败, 其他原因");
            }
            break;
        }
        case MAPMSG_DT_COMM_TIMER:  //时钟设置
        {
            if(MAPMSG_APDU_SetResponse == type)
            {
                RSP_FMT_DEBUG("设置时间%s\n", *temp < 3 ? dir[*temp] : "失败, 其他原因");
            }
            else if(MAPMSG_APDU_GetRequest == type && MAPMSG_PRM_MODULE ==  pMapMsg->cmt.prm &&
                    MAPMSG_DIR_MODULE ==  pMapMsg->cmt.dir)
            {
                /* 模块主动请求 */
                RSP_FMT_DEBUG("模块请求时间(index %d)\n", pMapMsg->cmt.fid);
                rsp_send_res_time(pRspData->uartfd, pMapMsg->cmt.fid);
            }
            break;
        }
        case MAPMSG_DT_COMM_MODINFO: //模块信息
        {
            if(MAPMSG_APDU_GetResponse == type && MAPMSG_PRM_TERMINAL ==  pMapMsg->cmt.prm &&
               MAPMSG_DIR_MODULE ==  pMapMsg->cmt.dir)
            {
                if(0 == rsp_proc_msg_mapinfo(pMapMsg->apdu, pMapMsg->len, &ptHandle->mapInfo))
                {
                    char version[128] = {0};
                    
                    RSP_FMT_DEBUG("模块信息:\n型号: %s\n设备ID: %s\n厂商代码: %s, 软件版本号: %s, 软件日期: %s, "
                                  "硬件版本号: %s, 硬件日期: %s, 扩展信息: %s\n", ptHandle->mapInfo.modeltype,
                                  ptHandle->mapInfo.ID, ptHandle->mapInfo.manuNum, ptHandle->mapInfo.softversion,
                                  ptHandle->mapInfo.softdata, ptHandle->mapInfo.hwversion, ptHandle->mapInfo.hwdata,
                                  ptHandle->mapInfo.manuExten);
                    ptHandle->mapStatus = RSP_MAP_CHECK;

                    sprintf(version, "版本:%s 日期:%s",ptHandle->mapInfo.softversion, ptHandle->mapInfo.softdata);
                    rsp_write_version(version, strlen(version));
                }
            }
            break;
        }
        case MAPMSG_DT_COMM_FILETRAN:
        {
            if(MAPMSG_APDU_SetResponse == type && ptHandle->fileindex == pMapMsg->cmt.fid)
            {
                UPDATE_FMT_DEBUG("文件传输序号%d传输%s\n", ptHandle->fileNO, *temp < 3 ? dir[*temp] : "失败, 其他原因");
                if(0 == *temp)
                {
                    ptHandle->fileNO += 1;
                }
                ptHandle->bNeedSend = TRUE;
            }
            break;
        }
        case MAPMSG_DT_CTR_LOOP_STATUS:
        {
            uint16 status = 0xFFFF;
            uint8  num = 16;
            if(0 == rsp_proc_msg_ctrl(pMapMsg->apdu, pMapMsg->len, &status, &num))
            {
                if(status != ptHandle->switchStatus || num != ptHandle->switchnum)
                {
                    ptHandle->switchStatus = status;
                    ptHandle->switchnum = num;
                    rsp_write_ctrlstatus(num, status);
                }
            }
            break;
        }
        case MAPMSG_DT_485_CAN_STATUS:
        {
            if(MAPMSG_APDU_GetResponse == type && ptHandle->get485index == pMapMsg->cmt.fid)
            {
                ret = rsp_proc_msg_485can(pMapMsg->apdu, pMapMsg->len, &ptHandle->get485_can);
                ptHandle->get485Status = 0 == ret ? 0 : 2;
                RSP_FMT_DEBUG("485-2口工作模式%d\n", ptHandle->get485_can);
            }
            if(MAPMSG_APDU_SetResponse == type && ptHandle->set485index == pMapMsg->cmt.fid)
            {
                ptHandle->set485Status = 0 == *temp ? 0 : 2;
                RSP_FMT_DEBUG("设置485-2口工作模式%s\n", *temp < 3 ? dir[*temp] : "失败, 其他原因");
            }
            break;
        }
        default:
            break;
    }
    
    return pMapMsg->len + 8;
}
#endif

#if DESC("终端本地遥信脉冲操作", 1)

/**********************************************************************
* @name      : rsp_dev_work
* @brief     ：遥信脉冲设备工作
* @param[in] ：RSP_DEV_T  *ptHandle    线程句柄
               RSP_DATA_T *pRspData   进程数据
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-7-26
* @Update    :
**********************************************************************/
int rsp_uart_work(RSP_DEV_T *ptHandle, RSP_DATA_T *pRspData)
{
    struct timeval   curtime = {0};
    struct timezone  tz;
    OOP_DATETIME_T   tOopTime;
    int     ret = 0;
    MAPMSG_INFO_T *pMapMsg = NULL;
    uint16  offset = 0;
    uint8  *temp = NULL;
    uint16  len = 0;
    static uint32 s_getCtrltime = 0;
    
    gettimeofday(&curtime, &tz); //获取系统时间
    rsp_get_curtime(&tOopTime);

    /* 查询脉冲, 6s查询一次发现异常快速恢复 */
    if((ptHandle->gettime.year != tOopTime.year || ptHandle->gettime.month != tOopTime.month ||
        ptHandle->gettime.mday != tOopTime.mday || ptHandle->gettime.hour != tOopTime.hour ||
        ptHandle->gettime.minute != tOopTime.minute || 0 == tOopTime.second % 5) && FALSE == ptHandle->bFirstGet)
    {
        ptHandle->gettime = tOopTime;
        if((ptHandle->pulsesum > 0 && 0 == ptHandle->bShare) ||
           1 == ptHandle->config[0] || 1 == ptHandle->config[1] || 1 == ptHandle->config[2] || 1 == ptHandle->config[3]||
           1 == ptHandle->config[4] || 1 == ptHandle->config[5] || 1 == ptHandle->config[6] || 1 == ptHandle->config[7])
        {
            ret = rsp_send_get_comm(pRspData->uartfd, ptHandle->index++, MAPMSG_DT_PULSE_TOTALDATA);
            if(0 != ret)
            {
                return ret;
            }
            goto recv;
        }
        goto recv;
    }

    /* 查询遥信状态, 过分0s不采集 */
    if((curtime.tv_sec != ptHandle->timeflag.tv_sec || TRUE == ptHandle->bFirstGet) && 
       0 != tOopTime.second && 0 != ptHandle->sum)
    {
        ptHandle->timeflag = curtime;
        ret = rsp_send_get_comm(pRspData->uartfd, ptHandle->index++, MAPMSG_DT_PULSE_STATUS);
        if(0 != ret)
        {
            return ret;
        }
        ptHandle->bFirstGet = FALSE;
        goto recv;
    }

    /* 如果没有遥信，就要结束 */
    if(0 == ptHandle->sum)
    {
        ptHandle->bFirstGet = FALSE;
    }
    
    /* 设置功能 */
    if(1 == ptHandle->configStatus)
    {
        ptHandle->configindex = ptHandle->index++;
        PTHREAD_LOCK_SAFE(&ptHandle->flock);
        ret = rsp_send_set_config(pRspData->uartfd, ptHandle->configindex, ptHandle->config, RSP_PORT_MAX);
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
        ret = rsp_send_set_shake(pRspData->uartfd, ptHandle->shakeindex, ptHandle->shakeTime);
        PTHREAD_UNLOCK_SAFE(&ptHandle->flock);
        ptHandle->shakeStatus = 0 == ret ? 3 : 2;
        if(0 != ret)
        {
            return ret;
        }
        goto recv;
    }

    /* 对时 文件升级 */
    ret = rsp_map_work_other(ptHandle, pRspData);
    if(0 != ret)
    {
        if(0 != ret)
        {
            return ret;
        }
    }

    /* 读取遥控状态 */
    if(1 == pRspData->bGetCtrl && s_getCtrltime != curtime.tv_sec)
    {
        s_getCtrltime = curtime.tv_sec;
        rsp_send_get_comm(pRspData->uartfd, ptHandle->index++, MAPMSG_DT_CTR_LOOP_STATUS);
    }

recv:
    /* 接收处理 */
    ret = rsp_uart_recv(pRspData->uartfd, ptHandle->recv + ptHandle->recvlen, RSP_BUF_LENGTH - ptHandle->recvlen, 20, 10);
    if(ret > 0)
    {
        ptHandle->recvlen += ret;
        if(ptHandle->recvlen > RSP_BUF_LENGTH)
        {
            ptHandle->recvlen = RSP_BUF_LENGTH;
        }
        RSP_BUF_TRACE(ptHandle->recv, ptHandle->recvlen, "本体接收报文");
        temp = ptHandle->recv;
        while(offset < ptHandle->recvlen)
        {
            if(FRAMESTART == temp[offset])
            {
                ret = mapmsg_split(temp + offset, ptHandle->recvlen - offset, &pMapMsg);
                if(ret == 0 && NULL != pMapMsg)
                {
                    offset += rsp_proc_uart_msg(pMapMsg, ptHandle, pRspData);
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
* @name      : rsp_copt_work
* @brief     ：遥信脉冲设备工作
* @param[in] ：RSP_DEV_T  *ptHandle    线程句柄
               RSP_DATA_T *pRspData   进程数据
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2022-6-16
* @Update    :
**********************************************************************/
int rsp_copt_work(RSP_DEV_T *ptHandle, RSP_DATA_T *pRspData)
{
    int     ret = 0;
    MAPMSG_INFO_T *pMapMsg = NULL;
    uint16  offset = 0;
    uint8  *temp = NULL;
    uint16  len = 0;

    /* 对时 文件升级 */
    ret = rsp_map_work_other(ptHandle, pRspData);
    if(0 != ret)
    {
        if(0 != ret)
        {
            return ret;
        }
    }

    /* 485CAN查询 */
    if(1 == ptHandle->get485Status)
    {
        ptHandle->get485index = ptHandle->index++;
        ret = rsp_send_get_comm(pRspData->uartfd, ptHandle->get485index, MAPMSG_DT_485_CAN_STATUS);
        ptHandle->get485Status = 0 == ret ? 3 : 2;
        if(0 != ret)
        {
            return ret;
        }
        goto recv;
    }

    /* 485CAN设置 */
    if(1 == ptHandle->set485Status)
    {
        ptHandle->set485index = ptHandle->index++;
        ret = rsp_send_set_485can(pRspData->uartfd, ptHandle->set485index, ptHandle->set485_can);
        ptHandle->set485Status = 0 == ret ? 3 : 2;
        if(0 != ret)
        {
            return ret;
        }
        goto recv;
    }

recv:
    /* 接收处理 */
    ret = rsp_uart_recv(pRspData->uartfd, ptHandle->recv + ptHandle->recvlen, RSP_BUF_LENGTH - ptHandle->recvlen, 20, 10);
    if(ret > 0)
    {
        ptHandle->recvlen += ret;
        if(ptHandle->recvlen > RSP_BUF_LENGTH)
        {
            ptHandle->recvlen = RSP_BUF_LENGTH;
        }
        RSP_BUF_TRACE(ptHandle->recv, ptHandle->recvlen, "本体接收报文");
        temp = ptHandle->recv;
        while(offset < ptHandle->recvlen)
        {
            if(FRAMESTART == temp[offset])
            {
                ret = mapmsg_split(temp + offset, ptHandle->recvlen - offset, &pMapMsg);
                if(ret == 0 && NULL != pMapMsg)
                {
                    offset += rsp_proc_uart_msg(pMapMsg, ptHandle, pRspData);
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
* @param[in] ：void *pArg     线程句柄
               void *pPublic  公有数据
               void *pPrivate 私有数据
* @param[out]：
* @return    ：延时时间 us
* @Create    : 王津剑
* @Date      ：2022-3-20
* @Update    :
**********************************************************************/
int rsp_ym_proc(void *pArg, void *pPublic, void *pPrivate)
{
    RSP_DATA_T *pRspData = (RSP_DATA_T*)pPublic;
    RSP_DEV_T *ptHandle = (RSP_DEV_T*)pPrivate;

    if(pRspData->uartfd > 0 && (RSP_STATUS_WORK != ptHandle->workstauts || 0 != access(RSP_UART_NAME, F_OK)))
    {
        rsp_uart_close(pRspData->uartfd);
        pRspData->uartfd = -1;
        RSP_FMT_DEBUG("设备1异常, 关闭句柄\n");
    }
    
    switch(ptHandle->workstauts)
    {
        case RSP_STATUS_GET_DEV:
        {
            if(0 == access(RSP_UART_NAME, F_OK))
            {
                RSP_FMT_DEBUG("终端本地存在, 开始工作\n");
                ptHandle->workstauts = RSP_STATUS_INIT;
            }
            //sleep(2);
            break;
        }
        case RSP_STATUS_INIT:
        {
            /* 打开串口 */
            memset(ptHandle->recv, 0, RSP_BUF_LENGTH);
            ptHandle->recvlen = 0;
            
            pRspData->uartfd = rsp_uart_open(RSP_UART_NAME);
            if(pRspData->uartfd < 0)
            {
                RSP_FMT_DEBUG("设备1 %s 打开失败\n", RSP_UART_NAME);
                ptHandle->workstauts = RSP_STATUS_GET_DEV;
                break;
            }
            if(0 != rsp_uart_set(pRspData->uartfd, 115200, 8, 1, 'N'))
            {
                RSP_FMT_DEBUG("设备1 %s 设置参数失败\n", RSP_UART_NAME);
                ptHandle->workstauts = RSP_STATUS_GET_DEV;
            }
            else
            {
                RSP_FMT_DEBUG("设备1 %s 打开成功\n", RSP_UART_NAME);
                ptHandle->workstauts = RSP_STATUS_WORK;
                ptHandle->configStatus = 1;
                ptHandle->shakeStatus = 1;
                ptHandle->mapStatus = RSP_MAP_LINK;
                ptHandle->bFirstRun = TRUE;
                ptHandle->bFirstGet = TRUE;
            }
            break;
        }
        case RSP_STATUS_WORK:
        {
            if(0 != rsp_uart_work(ptHandle, pRspData))
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
    return 5000;
}

/**********************************************************************
* @name      : rsp_dev_proc
* @brief     ：遥信设备检查
* @param[in] ：void *pArg     线程句柄
               void *pPublic  公有数据
               void *pPrivate 私有数据
* @param[out]：
* @return    ：延时时间 us
* @Create    : 王津剑
* @Date      ：2022-3-20
* @Update    :
**********************************************************************/
int rsp_copt_proc(void *pArg, void *pPublic, void *pPrivate)
{
    RSP_DATA_T *pRspData = (RSP_DATA_T*)pPublic;
    RSP_DEV_T *ptHandle = (RSP_DEV_T*)pPrivate;

    if(pRspData->uartfd > 0 && (RSP_STATUS_WORK != ptHandle->workstauts || 0 != access(COPT_UART_NAME, F_OK)))
    {
        rsp_uart_close(pRspData->uartfd);
        pRspData->uartfd = -1;
        RSP_FMT_DEBUG("设备1异常, 关闭句柄\n");
    }
    
    switch(ptHandle->workstauts)
    {
        case RSP_STATUS_GET_DEV:
        {
            if(0 == access(COPT_UART_NAME, F_OK))
            {
                RSP_FMT_DEBUG("终端维护口存在, 开始工作\n");
                ptHandle->workstauts = RSP_STATUS_INIT;
            }
            sleep(2);
            break;
        }
        case RSP_STATUS_INIT:
        {
            /* 打开串口 */
            memset(ptHandle->recv, 0, RSP_BUF_LENGTH);
            ptHandle->recvlen = 0;
            
            pRspData->uartfd = rsp_uart_open(COPT_UART_NAME);
            if(pRspData->uartfd < 0)
            {
                RSP_FMT_DEBUG("设备1 %s 打开失败\n", COPT_UART_NAME);
                ptHandle->workstauts = RSP_STATUS_GET_DEV;
                break;
            }
            if(0 != rsp_uart_set(pRspData->uartfd, 115200, 8, 1, 'N'))
            {
                RSP_FMT_DEBUG("设备1 %s 设置参数失败\n", COPT_UART_NAME);
                ptHandle->workstauts = RSP_STATUS_GET_DEV;
            }
            else
            {
                RSP_FMT_DEBUG("设备1 %s 打开成功\n", COPT_UART_NAME);
                ptHandle->workstauts = RSP_STATUS_WORK;
                ptHandle->mapStatus = RSP_MAP_LINK;
            }
            break;
        }
        case RSP_STATUS_WORK:
        {
            if(0 != rsp_copt_work(ptHandle, pRspData))
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
    return 5000;
}

#endif

/**********************************************************************
* @name      : rsp_dev_init
* @brief     ：遥信设备初始化
* @param[in] ：void *pPublic    公有数据
* @param[out]：void **ppPrivate 私有数据
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2022-3-20
* @Update    :
**********************************************************************/
int rsp_uart_init(void *pPublic, void **ppPrivate)
{
    RSP_DATA_T      *pRspData = (RSP_DATA_T*)pPublic;

    RSP_FMT_DEBUG("UART设备采集终端本地\n");
    system("rm -rf /data/app/rspSample/data/endupdate");
    system("rm -rf /data/app/rspSample/data/startupdate");
    system("rm -rf /data/app/rspSample/data/version");
                
    pRspData->uartfd = -1;
    if(FALSE == pRspData->bIIC)
    {
        pRspData->tRspDev.bFirstRun = TRUE;
    }
    memcpy(pRspData->tRspDev.updatefile, RSP_UPDATE_FILENAME, strlen(RSP_UPDATE_FILENAME));

    pRspData->tRspDev.switchnum = 0;
    pRspData->tRspDev.switchStatus = 0xFFFF;
    *ppPrivate = &(pRspData->tRspDev);
    return 0;
}

/**********************************************************************
* @name      : rsp_dev_proc
* @brief     ：遥信设备检查
* @param[in] ：void *pArg     线程句柄
               void *pPublic  公有数据
               void *pPrivate 私有数据
* @param[out]：
* @return    ：延时时间 us
* @Create    : 王津剑
* @Date      ：2022-3-20
* @Update    :
**********************************************************************/
int rsp_uart_proc(void *pArg, void *pPublic, void *pPrivate)
{
    RSP_DATA_T *pRspData = (RSP_DATA_T*)pPublic;

    if(TRUE == pRspData->bIIC)
    {
        /* IIC设备执行升级和对时任务 */
        rsp_copt_proc(pArg, pPublic, pPrivate);
    }
    else
    {
        rsp_ym_proc(pArg, pPublic, pPrivate);
    }
    return 5000;
}


