#include "../include/mdlmgr.h"
#include "../include/mdlcommon.h"
#include "dataCommon.h"
#include "../include/Mqtt.h"
#include <hal.h>
#include "uart.h"
#include <power.h>
#include <time.h>
#include "framecomm.h"
#include "audio.h"

//收到的消息缓存区
INFO_DATA_LIST_T gInfoDataList_mdl = {0};                               //收到的消息缓存区列表

InfoDataQueue_T InfoDataQueue;                                          //已处理mqtt消息帧信息
LockQueue_T     LockRecvBuf;                                            //mqtt接收缓冲区
LockQueue_T     LockSendBuf[mdlnum];                                    //mqtt发送缓冲区

pthread_mutex_t ActiveInfoIndexLk;
uint16  ActInfIndex;

/*******************************************************************************
* 函数名称: AddInfoDataIntoList
* 函数功能: 添加消息至队列
* 输入参数: 消息msg结构体
* 输出参数: 无
* 返 回 值: 
*******************************************************************************/
int8 AddInfoDataIntoList(INFO_DATA_T RecvInfoData)
{
    uint16 nAddIndex = 0;
    
    //将收到的新消息暂存缓冲区
    if(gInfoDataList_mdl.InfoDataNum < MAX_INFO_DATA_NUM)
    {
        nAddIndex = (gInfoDataList_mdl.InfoDataReadIndex+gInfoDataList_mdl.InfoDataNum) % MAX_INFO_DATA_NUM;//循环取添加地址（数组下标）
        //MDLMGR_FMT_DEBUG(mdlnum, "AddMsgIntoList()添加之前...stInfoMsgDataList.InfoMsgDataNum=[%d]...begin\n", gInfoDataList_mdl.InfoDataNum);            
        
        gInfoDataList_mdl.InfoDataNum ++;
        gInfoDataList_mdl.InfoDataList[nAddIndex] = RecvInfoData;

        //MDLMGR_FMT_DEBUG(mdlnum, "AddMsgIntoList()添加之后...stInfoMsgDataList.InfoMsgDataNum=[%d]...end\n", gInfoDataList_mdl.InfoDataNum);
    }
    else
    {
        MDLMGR_FMT_DEBUG(mdlnum, "CreateTaskByInfo()...stInfoMsgDataList.InfoMsgDataNum 大于等于 MAX_MSG_INFO_NUM，请等待...\n");
        return -1;
    }

    return 1;
}

/*******************************************************************************
* 函数名称: GetInfoDataFromList
* 函数功能: 从队列中取出消息
* 输入参数: 
* 输出参数: 消息msg结构体
* 返 回 值: 
*******************************************************************************/
int8 GetInfoDataFromList(INFO_DATA_T *SendInfoData)
{
    //判断消息队列是否为空
    if(gInfoDataList_mdl.InfoDataNum > 0)
    {   
        //MDLMGR_FMT_DEBUG(mdlnum, "CreateTaskByInfo()取出之前...收到消息的数量=[%d]...begin\n", gInfoDataList_mdl.InfoDataNum);

        //按照先进先出的顺序取出消息
        *SendInfoData = gInfoDataList_mdl.InfoDataList[gInfoDataList_mdl.InfoDataReadIndex];
        gInfoDataList_mdl.InfoDataNum --;
        gInfoDataList_mdl.InfoDataReadIndex = (gInfoDataList_mdl.InfoDataReadIndex+1) % MAX_INFO_DATA_NUM;

        //MDLMGR_FMT_DEBUG(mdlnum, "CreateTaskByInfo()取出之后...收到消息的数量=[%d]...end\n", gInfoDataList_mdl.InfoDataNum);
        
    }
    else
    {
        //MDLMGR_FMT_DEBUG(mdlnum, "uint32 CreateTaskByInfo()...stInfoMsgDataList.InfoMsgDataNum 小于等于 0，请等待...\n");
        return -1;
    }

    return 1;
}

int32   MakeModeFrame(MDL_FRAME_INFO_T* frameInfo, uint8* pSendBuf)
{
    int dataIndex = 0;

    if(frameInfo == NULL || pSendBuf == NULL)
    {
        return -1;
    }
    //组报文帧
    pSendBuf[dataIndex++] = 0x68;
    memcpy(&pSendBuf[dataIndex],&frameInfo->dataLen,2);
    dataIndex += 2;
    pSendBuf[dataIndex++] = frameInfo->cntl.all;
    pSendBuf[dataIndex++] = frameInfo->seq;
    memcpy(&pSendBuf[dataIndex],frameInfo->pDataBuf,frameInfo->dataLen);
    dataIndex += frameInfo->dataLen;
    frameInfo->fcs = CRC16_Get(&pSendBuf[1], dataIndex - 1); 
    memcpy(&pSendBuf[dataIndex],&frameInfo->fcs,2);
    dataIndex += 2;
    pSendBuf[dataIndex++] = 0x16;

    return dataIndex;
}

int32 GetTimeNow(int32 usbNo, uint8* pData, uint8* dataLen)
{
    struct timeval  tv;
    struct tm*      pTime;
    MDL_DATE_TIME_T date_time;

    gettimeofday(&tv,NULL);
    pTime = localtime((const time_t*)&(tv.tv_sec));
    date_time.year          = (uint16)(pTime->tm_year + 1900);
    date_time.month         = (uint8)(pTime->tm_mon + 1);
    date_time.day_of_month  = (uint8)pTime->tm_mday;
    date_time.day_of_week   = (uint8)pTime->tm_wday;
    date_time.hour          = (uint8)pTime->tm_hour;
    date_time.minute        = (uint8)pTime->tm_min;
    date_time.second        = (uint8)pTime->tm_sec;
    date_time.millisec      = (uint16)(tv.tv_usec / 1000);
    MDLMGR_FMT_DEBUG(usbNo, "给模块设置时钟：%d-%2d-%2d 星期%d %02d:%02d:%02d:%04d\n",(int)date_time.year,(int)date_time.month,(int)date_time.day_of_month,
                                        (int)date_time.day_of_week,(int)date_time.hour,(int)date_time.minute,(int)date_time.second,(int)date_time.millisec);
    date_time.year          = htons(date_time.year);
    date_time.millisec      = htons(date_time.millisec);
    memcpy(pData,(uint8*)&date_time,sizeof(MDL_DATE_TIME_T));
    *dataLen = sizeof(MDL_DATE_TIME_T);
    return 1;
}

//模组通信组帧
int32 MakeFrame(int32 usbNo, TRANS_INFO_T *pTransInfo,   uint8* pSendBuf)
{
    TRANS_INFO_T    *pRecvInfo;
    MDL_FRAME_INFO_T  frameinfo;
    char    dataBuf[512] = {0};
    int dwOffset = 0;
    UPDATE_INFO_T *pUpdateInfo;
    
    memset(&frameinfo,0x0,sizeof(MDL_FRAME_INFO_T));
    if(pSendBuf == NULL || pTransInfo == NULL)
    {
        return -1;
    }
    pRecvInfo = pTransInfo;
    frameinfo.seq = pRecvInfo->seq;
    frameinfo.cntl.all = 0x42;

    if(pRecvInfo->DTA == 0x00)          //只读
    {   
        frameinfo.frameFunc = GETREQUEST;
        frameinfo.dataLen = 0x03;
        //char databuf[3] = {0};
        //DTA = (DTA1<<0x03) | (DTA2 & 0x07);
        switch (pRecvInfo->DTB)
        {
            case 0x00:  //模块信息
            case 0x01:  //链路协商信息
            case 0x02:  //协议版本
            case 0x03:  //电源状态
            case 0x04:  //文件传输状态
                dataBuf[0] = frameinfo.frameFunc;
                dataBuf[1] = pRecvInfo->DTA;
                dataBuf[2] = pRecvInfo->DTB;
                memcpy(frameinfo.pDataBuf,dataBuf,frameinfo.dataLen);
                return MakeModeFrame(&frameinfo, pSendBuf);
                //break;
            default:
                MDLMGR_FMT_TRACE(usbNo, "MakeFrame DTB %d 错误\n",pRecvInfo->DTB);
                return -1;
                //break;
        }
    }
    else if(pRecvInfo->DTA == 0x01) //读写
    {
        if(pRecvInfo->DTB == 00)        //时钟
        {
            if(pRecvInfo->clkSetFlag)       //设置时钟
            {   
                frameinfo.frameFunc = SETREQUEST;
                dataBuf[0] = frameinfo.frameFunc;
                dataBuf[1] = pRecvInfo->DTA;
                dataBuf[2] = pRecvInfo->DTB;
                dataBuf[3] = DATE_TIME;
                GetTimeNow(usbNo, pRecvInfo->setData,(uint8*)&pRecvInfo->dataLen);
                if(pRecvInfo->dataLen != 0)
                {
                    memcpy(&dataBuf[4],pRecvInfo->setData,pRecvInfo->dataLen);
                    frameinfo.dataLen = 4 + pRecvInfo->dataLen;
                }
                else
                {
                    //add data_time now
                    //frameinfo.dataLen = 4 + sizeof(Date_time_t);
                }
            }
            else                //读取时钟
            {
                frameinfo.dataLen = 0x03;
                frameinfo.frameFunc = GETREQUEST;
                dataBuf[0] = frameinfo.frameFunc;
                dataBuf[1] = pRecvInfo->DTA;
                dataBuf[2] = pRecvInfo->DTB;
            }
            memcpy(frameinfo.pDataBuf,dataBuf,frameinfo.dataLen);
            return MakeModeFrame(&frameinfo, pSendBuf);

        }
    }
    else if(pRecvInfo->DTA == 0x02) //只写
    {
        frameinfo.frameFunc = SETREQUEST;
        switch (pRecvInfo->DTB)
        {
            case 0x00:      //硬件初始化
            case 0x01:      //参数初始化
            case 0x02:      //数据初始化
            
                dataBuf[0] = frameinfo.frameFunc;
                dataBuf[1] = pRecvInfo->DTA;
                dataBuf[2] = pRecvInfo->DTB;
                frameinfo.dataLen = 0x03;
                memcpy(frameinfo.pDataBuf,dataBuf,frameinfo.dataLen);
                return MakeModeFrame(&frameinfo, pSendBuf);
                //break;
                
            case 0x03:      //文件传输
                pUpdateInfo = (UPDATE_INFO_T*)pRecvInfo->setData;
                dataBuf[dwOffset++] = frameinfo.frameFunc;
                dataBuf[dwOffset++] = pRecvInfo->DTA;
                dataBuf[dwOffset++] = pRecvInfo->DTB;
                dataBuf[dwOffset++] = STRUCTURE;
                dataBuf[dwOffset++] = 0x04;
                dataBuf[dwOffset++] = UNSIGNED;
                dataBuf[dwOffset++] = pUpdateInfo->fileID;
                dataBuf[dwOffset++] = DOUBLE_LONG_UNSIGNED;
                memcpy_r(&dataBuf[dwOffset],&pUpdateInfo->packNo,4);
                dwOffset += 4;
                dataBuf[dwOffset++] = DOUBLE_LONG_UNSIGNED;
                memcpy_r(&dataBuf[dwOffset],&pUpdateInfo->packCnt,4);
                dwOffset += 4;
                dataBuf[dwOffset++] = OCTET_STRING;
                dataBuf[dwOffset++] = 0x82;             //长度为2字节
                dataBuf[dwOffset++] = (pUpdateInfo->packDataLen>>8) & 0xFF;
                dataBuf[dwOffset++] = pUpdateInfo->packDataLen &0xFF;
                memcpy(&dataBuf[dwOffset],&pUpdateInfo->packData,pUpdateInfo->packDataLen);
                dwOffset += pUpdateInfo->packDataLen;
                
                frameinfo.dataLen = dwOffset;
                memcpy(frameinfo.pDataBuf,dataBuf,frameinfo.dataLen);
                return MakeModeFrame(&frameinfo, pSendBuf);
                //break;

            default:

                break;
        }
    }

    if(pRecvInfo->DTA == 0x40)      //读遥信
    {
        frameinfo.frameFunc = GETREQUEST;
        frameinfo.dataLen = 0x03;
        switch (pRecvInfo->DTB)
        {
            case 0x00:      //遥信状态
            case 0x01:      //上一次脉冲计数和时间
            case 0x02:      //脉冲计数和时间
                dataBuf[0] = frameinfo.frameFunc;
                dataBuf[1] = pRecvInfo->DTA;
                dataBuf[2] = pRecvInfo->DTB;
                memcpy(frameinfo.pDataBuf,dataBuf,frameinfo.dataLen);
                return MakeModeFrame(&frameinfo, pSendBuf);
                //break;
            default:
                MDLMGR_FMT_TRACE(usbNo, "遥信读，DTB %d \n",pRecvInfo->DTB);
                break;
        }
        
    }
    else if( pRecvInfo->DTA == 0x41)        //读写遥信
    {
        if(pRecvInfo->clkSetFlag)           //表示设置
        {
            frameinfo.frameFunc = SETREQUEST;
            dataBuf[0] = frameinfo.frameFunc;
            dataBuf[1] = pRecvInfo->DTA;
            dataBuf[2] = pRecvInfo->DTB;
            switch (pRecvInfo->DTB)
            {
                case 0x00:      //遥信脉冲功能配置
                
                    dataBuf[3] = BIT_STRING;
                    memcpy(&dataBuf[4],pRecvInfo->setData,pRecvInfo->dataLen);
                    frameinfo.dataLen = 4 + pRecvInfo->dataLen;
                    memcpy(frameinfo.pDataBuf,dataBuf,frameinfo.dataLen);
                    return MakeModeFrame(&frameinfo, pSendBuf);
                    //break;
                    
                case 0x01:      //遥信防抖时间
                
                    dataBuf[3] = LONG_UNSIGNED;
                    memcpy(&dataBuf[4],pRecvInfo->setData,pRecvInfo->dataLen);
                    frameinfo.dataLen = 4 + pRecvInfo->dataLen;
                    memcpy(frameinfo.pDataBuf,dataBuf,frameinfo.dataLen);
                    return MakeModeFrame(&frameinfo, pSendBuf);
                    //break;
                    
                default:
                    break;
                
            }
        }
        else                
        {
            frameinfo.frameFunc = GETREQUEST;
            frameinfo.dataLen = 0x03;
            switch (pRecvInfo->DTB)
            {
                case 0x00:      //遥信功能配置读
                case 0x01:      //遥信防抖时间
                    dataBuf[0] = frameinfo.frameFunc;
                    dataBuf[1] = pRecvInfo->DTA;
                    dataBuf[2] = pRecvInfo->DTB;
                    memcpy(frameinfo.pDataBuf,dataBuf,frameinfo.dataLen);
                    return MakeModeFrame(&frameinfo, pSendBuf);
                    //break;
                default:
                    break;
            }
        }
    }

    if(pRecvInfo->DTA == 0x71)
    {
        if(pRecvInfo->clkSetFlag)       //设置
        {
            frameinfo.frameFunc = SETREQUEST;
            dataBuf[0] = frameinfo.frameFunc;
            dataBuf[1] = pRecvInfo->DTA;
            dataBuf[2] = pRecvInfo->DTB;
            switch(pRecvInfo->DTB)
            {
                case 0x00:              //继电器输出模式
                    dataBuf[3] = ENUM;
                    memcpy(&dataBuf[4],pRecvInfo->setData,pRecvInfo->dataLen);
                    frameinfo.dataLen = 4 + pRecvInfo->dataLen;
                    memcpy(frameinfo.pDataBuf,dataBuf,frameinfo.dataLen);
                    return MakeModeFrame(&frameinfo, pSendBuf);
                    //break;

                case 0x01:              //继电器输出脉冲宽度
                    dataBuf[3] = LONG_UNSIGNED;
                    memcpy(&dataBuf[4],pRecvInfo->setData,pRecvInfo->dataLen);
                    frameinfo.dataLen = 4 + pRecvInfo->dataLen;
                    memcpy(frameinfo.pDataBuf,dataBuf,frameinfo.dataLen);
                    return MakeModeFrame(&frameinfo, pSendBuf);
                    //break;

                default:
                    break;
            }
        }
        else
        {
            switch(pRecvInfo->DTB)
            {
                case 0x00:              //继电器输出模式 
                case 0x01:              //继电器输出脉冲宽度
                    frameinfo.frameFunc = GETREQUEST;
                    dataBuf[0] = frameinfo.frameFunc;
                    dataBuf[1] = pRecvInfo->DTA;
                    dataBuf[2] = pRecvInfo->DTB;
                    frameinfo.dataLen = 0x03;
                    memcpy(frameinfo.pDataBuf,dataBuf,frameinfo.dataLen);
                    return MakeModeFrame(&frameinfo, pSendBuf);
                    //break;
        
                default:
                    break;      
            }
        }
    }

    if(pRecvInfo->DTA == 0x80)      //模拟量采样
    {
        if(pRecvInfo->DTB == 0x00)
        {
            //
        }
    }

    return 1;
}

int32 PlugEventInform(uint8 usbNo)
{   
    INFO_DATA_T infoDataTemp;
    char        payload[100];
    uint32      offset = 0;
    uint32      slotNo = usbNo + 1;

    #if PRTL_NEW_ENABEL == 1
    LINKRESPONSE_T* pLinkInfo = &moduleInfo_all[usbNo].linkInfo;
    MODULE_INF_T*   pModuleInfo = &moduleInfo_all[usbNo].moduleInfo;

    char   ver[256] = {0};
    uint8  datetemp[5] = {0};
    char   dateAscii[20] = {0};
    uint16 asclen = 0;
    int    i = 0;
    #endif

    memset(payload, 0x0, sizeof(payload));
    memset(&infoDataTemp,0x0,sizeof(INFO_DATA_T));
    plug_status[usbNo].time = time(NULL);          //获取变位上报时间
    
    infoDataTemp.MsgRPM = 1;                       //启动上报
    infoDataTemp.MsgPRIORITY = 0;   
    infoDataTemp.MsgIndex = globleSeq++;
    strcpy(infoDataTemp.DestAddr,appName);
    infoDataTemp.DestLen = strlen(appName);        //后面发送会调换源/目的
    #if 0
    strcpy(infoDataTemp.SourAddr, smiosName);
    infoDataTemp.SourLen = strlen(smiosName);
    #else
    strcpy(infoDataTemp.SourAddr, "Broadcast");
    infoDataTemp.SourLen = strlen("Broadcast");
    #endif

    infoDataTemp.IID_ID = 0x0004;       
    infoDataTemp.IOP_ID = 0x0001;       //模组插拔事件

    memcpy_r(payload, (uint8*)&slotNo, 4);
    offset += 4;
    
    if (plug_status[usbNo].plugStatus != ON_STATE)
    {
        payload[offset++] = 1;         //拔出
    }
    else
    {
        payload[offset++] = 0;         //插入

        #if PRTL_NEW_ENABEL == 1
        //组模块信息回码
        //模块设备型号
        payload[offset++] = pLinkInfo->modModeLen;
        memcpy(&payload[offset], pLinkInfo->modMode, pLinkInfo->modModeLen);      //模块类型从链路信息取
        offset += pLinkInfo->modModeLen;
        MDLMGR_FMT_DEBUG(mdlnum, "usb[%d]-上报插入事件 模块设备型号: %s\n", usbNo, pLinkInfo->modMode);

        //模块设备ID
        payload[offset++] = pModuleInfo->DevIDLen;
        memcpy(&payload[offset], pModuleInfo->DevID, pModuleInfo->DevIDLen);
        offset += pModuleInfo->DevIDLen;

        //软件版本，新协议
        if (pModuleInfo->type == MDL_PRTL_VER1)
        {
            sprintf(ver, "%s", pModuleInfo->softVer);
            payload[offset++] = strlen(ver) + 1;
            memcpy(&payload[offset], ver, strlen(ver) + 1);
            offset += strlen(ver) + 1;

            MDLMGR_FMT_DEBUG(mdlnum, "usb[%d]-上报插入事件 软件版本: %s\n", usbNo, pModuleInfo->softVer);
            MDLMGR_FMT_DEBUG(mdlnum, "usb[%d]-上报插入事件 软件发布日期 %s\n", usbNo, pModuleInfo->softDate);

            //软件发布日期
            asclen = strlen(pModuleInfo->softDate) + 1;
            payload[offset++] = asclen;
            memcpy(&payload[offset], pModuleInfo->softDate, asclen);
            offset += asclen;

            //硬件版本，新协议
            memset(ver, 0, sizeof(ver));
            sprintf(ver, "%s", pModuleInfo->hardVer);
            payload[offset++] = strlen(ver) + 1;
            memcpy(&payload[offset], ver, strlen(ver) + 1);
            offset += strlen(ver) + 1;

            MDLMGR_FMT_DEBUG(mdlnum, "usb[%d]-上报插入事件 硬件版本: %s\n", usbNo, pModuleInfo->hardVer);
            MDLMGR_FMT_DEBUG(mdlnum, "usb[%d]-上报插入事件 硬件发布日期 %s \n", usbNo, pModuleInfo->hardDate);

            //硬件发布日期
            asclen = strlen(pModuleInfo->hardDate) + 1;
            payload[offset++] = asclen;
            memcpy(&payload[offset], pModuleInfo->hardDate, asclen);
            offset += asclen;
        }
        else
        {
            sprintf(ver, "%d", pModuleInfo->softVersion);
            payload[offset++] = strlen(ver) + 1;
            memcpy(&payload[offset], ver, strlen(ver) + 1);
            offset += strlen(ver) + 1;

            MDLMGR_FMT_DEBUG(mdlnum, "usb[%d]-上报插入事件 软件版本: %04d-%02d-%02d-%02d\n", usbNo, pModuleInfo->date_soft.year,
            pModuleInfo->date_soft.month,pModuleInfo->date_soft.dayofmonth,pModuleInfo->date_soft.dayofweek);
            memcpy(datetemp, &pModuleInfo->date_soft, 5);
            datetemp[0] = pModuleInfo->date_soft.year/100;
            datetemp[1] = pModuleInfo->date_soft.year%100;
            BINtoASCII(datetemp, 5, dateAscii, &asclen);
            MDLMGR_FMT_DEBUG(mdlnum, "usb[%d]-上报插入事件 软件发布日期 %s\n", usbNo, dateAscii);

            //软件发布日期
            payload[offset++] = asclen;
            memcpy(&payload[offset],dateAscii,asclen);
            offset += asclen;

            //硬件版本，新协议
            memset(ver, 0, sizeof(ver));
            sprintf(ver, "%d", pModuleInfo->hardwVersion);
            payload[offset++] = strlen(ver) + 1;
            memcpy(&payload[offset], ver, strlen(ver) + 1);
            offset += strlen(ver) + 1;

            MDLMGR_FMT_DEBUG(mdlnum, "usb[%d]-上报插入事件 硬件版本: %04d-%02d-%02d-%02d\n", usbNo, pModuleInfo->date_hard.year,
                pModuleInfo->date_hard.month, pModuleInfo->date_hard.dayofmonth, pModuleInfo->date_hard.dayofweek);
            memcpy(datetemp,&pModuleInfo->date_hard, 5);
            datetemp[0] = pModuleInfo->date_hard.year/100;
            datetemp[1] = pModuleInfo->date_hard.year%100;
            BINtoASCII(datetemp, 5, dateAscii, &asclen);
            MDLMGR_FMT_DEBUG(mdlnum, "usb[%d]-上报插入事件 硬件发布日期 %s \n", usbNo, dateAscii);

            //硬件发布日期
            payload[offset++] = asclen;
            memcpy(&payload[offset], dateAscii, asclen);
            offset += asclen;
        }

        //厂商编码
        payload[offset++] = pModuleInfo->VendorCodeLen;
        memcpy(&payload[offset],pModuleInfo->VendorCode,pModuleInfo->VendorCodeLen);
        offset += pModuleInfo->VendorCodeLen;

        //通道配置
        payload[offset++] = pLinkInfo->virPipeCnt;
        for (i =0; i < pLinkInfo->virPipeCnt; i++ )
        {
            payload[offset++] = pLinkInfo->virturMode[i].interf;
            payload[offset++] = pLinkInfo->virturMode[i].funcConfig;
            payload[offset++] = strlen(pLinkInfo->virturMode[i].driveName) + 1;
            memcpy(&payload[offset], pLinkInfo->virturMode[i].driveName, strlen(pLinkInfo->virturMode[i].driveName) + 1);
            offset += strlen(pLinkInfo->virturMode[i].driveName) + 1;
        }
        #endif
    }
    
    memcpy(infoDataTemp.aPayloadData, payload, offset);
    infoDataTemp.nPayloadLength = offset;
    
    PlcWriteQueue(&LockRecvBuf, (uint8*)&infoDataTemp, sizeof(INFO_DATA_T));
    return 1;
}

/**
*********************************************************************
* @brief:        刷新状态栏
* @param[in]：   time    显示时长
* @param[in]：   str     显示内容
* @return：      NULL
*********************************************************************
*/
void refresh_status_bar(uint32 time, char *str)
{
    INFO_DATA_T infoDataTemp;
    char        payload[100];
    uint8       offset = 0;

    memset(payload, 0x0, sizeof(payload));
    memset(&infoDataTemp, 0x0, sizeof(INFO_DATA_T));

    //启动上报
    infoDataTemp.MsgRPM = 1;        
    infoDataTemp.MsgPRIORITY = 0;   
    infoDataTemp.MsgIndex = globleSeq++;

    //后面发送会调换源/目的
    strcpy(infoDataTemp.DestAddr,appName);
    infoDataTemp.DestLen = strlen(appName);     
    strcpy(infoDataTemp.SourAddr, desktopName);
    infoDataTemp.SourLen = strlen(desktopName);

    //状态栏更新
    infoDataTemp.IID_ID = 0x0007;       
    infoDataTemp.IOP_ID = 0x0012;

    memcpy_r(&payload[offset], (uint8*)&time, 4);
    offset += 4;

    if (strlen(str) > 127)
    {
        MDLMGR_FMT_DEBUG(mdlnum, "status bar msg too long\n");
        return;
    }

    payload[offset] = strlen(str);
    offset += 1;

    memcpy(&payload[offset], str, strlen(str) + 1);
    offset += strlen(str);    

    memcpy(infoDataTemp.aPayloadData, payload, offset);
    infoDataTemp.nPayloadLength = offset;

    PlcWriteQueue(&LockRecvBuf, (uint8*)&infoDataTemp, sizeof(INFO_DATA_T));
    return;
}

/*******************************************************************************
* 函数名称: GetModuleInfo
* 函数功能: 查询所有模组信息
* 输入参数: 
* 输出参数: 
* 返 回 值: 
*******************************************************************************/
int GetModuleInfoAll(INFO_DATA_T info_data)
{
    INFO_DATA_T     InfoSend;
    MODULE_INF_T * pModInfo;
    uint8   i;
    uint8   dataBuf[MAX_MSG_PAYLOAD_LEN];
    uint16  offset = 0;

    memset(dataBuf,0x0,sizeof(dataBuf));
    offset++;
    for (i=0; i < mdlnum; i++)
    {
        pModInfo = &moduleInfo_all[i].moduleInfo;
        #if PRTL_NEW_ENABEL == 1
        makeInfoData_new(i,pModInfo, &info_data, globleSeq++, &InfoSend);
        #else
        makeInfoData(i,pModInfo, &info_data, globleSeq++, &InfoSend);
        #endif
        if (offset + InfoSend.nPayloadLength >= MAX_MSG_PAYLOAD_LEN)
        {
            MDLMGR_FMT_DEBUG(mdlnum, "%s: 信息长度 %d 超出!\n",__FUNCTION__,offset + InfoSend.nPayloadLength);
            break;
        }
        memcpy(&dataBuf[offset],InfoSend.aPayloadData,InfoSend.nPayloadLength);
        offset += InfoSend.nPayloadLength;
    }
    dataBuf[0] = i;
    memcpy(InfoSend.aPayloadData,dataBuf,offset);
    InfoSend.nPayloadLength = offset;
    PlcWriteQueue(&LockRecvBuf, (uint8*)&InfoSend, sizeof(INFO_DATA_T));
    return 1;
}

/*******************************************************************************
* 函数名称: GetModuleInfo
* 函数功能: 查询模组信息
* 输入参数: 
* 输出参数: 
* 返 回 值: 
*******************************************************************************/
int GetModuleInfo(INFO_DATA_T info_data)
{
    uint32  slotNo = 0;
    INFO_DATA_T     InfoSend;
    MODULE_INF_T * pModInfo;
    
    memcpy_r((uint8*)&slotNo,info_data.aPayloadData,4);
    if(slotNo > 5 || slotNo < 1)
    {
        MDLMGR_FMT_DEBUG(mdlnum, "GetModuleInfo请求模组信息序列号usbNo %d 错误\n",slotNo);
        return -1;
    }

    pModInfo = &moduleInfo_all[slotNo-1].moduleInfo;

    if (info_data.IOP_ID == 0x1010)
    {
        makeInfoData_new((uint8)(slotNo-1),pModInfo, &info_data,globleSeq++, &InfoSend);
    }
    else
    {
        //召测模组信息，先改成新规范消息
        #if PRTL_NEW_ENABEL == 1
        makeInfoData_new((uint8)(slotNo-1),pModInfo, &info_data,globleSeq++, &InfoSend);
        #else
        makeInfoData((uint8)(slotNo-1),pModInfo, &info_data,globleSeq++, &InfoSend);
        #endif
    }
    
    PlcWriteQueue(&LockRecvBuf, (uint8*)&InfoSend, sizeof(INFO_DATA_T));

    MDLMGR_BUF_TRACE(mdlnum, (uint8*)&InfoSend, sizeof(MSG_INFO_T)+InfoSend.nPayloadLength, "InfoSend:");
    return 1;
}

/*******************************************************************************
* 函数名称: GetModuleInfo
* 函数功能: 查询模组槽位信息
* 输入参数: 
* 输出参数: 
* 返 回 值: >= 0 成功
            <  0 失败
*******************************************************************************/
int GetModuleSlotInfo(INFO_DATA_T info_data)
{
    int32           ret = -1;
    int32           slotIndex[5] = {0};
    uint8           slotCnt = 0;
    char            modtype[256] = {0};
    int             i = 0;
    INFO_DATA_T     infoDataTemp;

    memset(&infoDataTemp, 0x0, sizeof(INFO_DATA_T));
    memcpy(modtype, info_data.aPayloadData, info_data.nPayloadLength);
    for(i = 0; i < mdlnum; i++)
    {
        if (slotInfo[i].modeTypeLen <= 0 || slotInfo[i].modeTypeLen == 0xff) 
        {
            MDLMGR_FMT_DEBUG(mdlnum, "usb %d-没有设备在该槽位上，名称长度:%d\n", i, slotInfo[i].modeTypeLen);
            continue;
        }

        MDLMGR_FMT_TRACE(mdlnum, "usb %d-型号名称%s 需要查询的型号%s\n", i, slotInfo[i].modeType, &modtype[1]);
        if (info_data.nPayloadLength - 1 != modtype[0])
        {
            MDLMGR_FMT_DEBUG(mdlnum, "收到报文数据长度有误!\n");
            continue;
        }

        if (strstr(slotInfo[i].modeType,&modtype[1]) != NULL && slotInfo[i].modeType[0] == modtype[1])
        {
            slotIndex[slotCnt] = i+1;
            slotCnt++;
        }
    }

    if (slotCnt == 0)
    {
        MDLMGR_FMT_DEBUG(mdlnum, "未查找到模组%s的槽位信息 源APP[%s]\n", &modtype[1], info_data.SourAddr);
    }
    else
    {
        MDLMGR_FMT_TRACE(mdlnum, "找到%d个槽位匹配, 源APP[%s]\n",slotCnt, info_data.SourAddr);
        ret = ERR_OK;
    }
    
    memcpy(&infoDataTemp,&info_data,sizeof(INFO_DATA_T));
    infoDataTemp.MsgRPM = 0;
    infoDataTemp.nPayloadLength = sizeof(uint32)*slotCnt + 1;
    infoDataTemp.aPayloadData[0] = slotCnt;
    memcpy_r(&infoDataTemp.aPayloadData[1],slotIndex,infoDataTemp.nPayloadLength - 1);
    PlcWriteQueue(&LockRecvBuf, (uint8*)&infoDataTemp, sizeof(INFO_DATA_T));
    
    return ret;
}

/*******************************************************************************
* 函数名称: GetModuleInfo
* 函数功能: 查询模组状态
* 输入参数: 
* 输出参数: 
* 返 回 值: 
*******************************************************************************/
int GetModulePlugStatus(INFO_DATA_T info_data)
{
    int32           modeNo = 0;         //模组序号
    INFO_DATA_T     infoDataTemp;           

    memcpy_r((uint8*)&modeNo, info_data.aPayloadData,4);
    
    if(modeNo < 1 || modeNo > 5)
    {
        MDLMGR_FMT_DEBUG(mdlnum, "模组序号 %d 不合法\n",modeNo);
        return -1;
    }
    
    memcpy(&infoDataTemp,&info_data,sizeof(INFO_DATA_T));
    infoDataTemp.MsgRPM = 0;
    infoDataTemp.nPayloadLength = 2;

    if (info_data.IOP_ID == 0x1012)
    {
        infoDataTemp.aPayloadData[0] = ((plug_status[modeNo-1].plugStatus == ON_STATE) ? 0:1 );
    }
    else
    {
        #if PRTL_NEW_ENABEL == 1
        infoDataTemp.aPayloadData[0] = ((plug_status[modeNo-1].plugStatus == ON_STATE) ? 0:1 );
        #else
        infoDataTemp.aPayloadData[0] = ((plug_status[modeNo-1].plugStatus == ON_STATE) ? 1:0 );
        #endif
    }
    
    infoDataTemp.aPayloadData[1] = plug_status[modeNo-1].changeStatus;
    if(plug_status[modeNo-1].changeStatus == 1)         //上报后取消变位
    {
        plug_status[modeNo-1].changeStatus = 0;
    }
    PlcWriteQueue(&LockRecvBuf, (uint8*)&infoDataTemp, sizeof(INFO_DATA_T));
    return 1;
    
}

/*******************************************************************************
* 函数名称: GetModuleInfo
* 函数功能: 模组复位
* 输入参数: 
* 输出参数: 
* 返 回 值: 
*******************************************************************************/
int ResetModule(INFO_DATA_T info_data)      
{
    //参数初始化
    uint32  modeNo = 0;
    TRANS_INFO_T    transInfo;
    
    memset(&transInfo,0x0,sizeof(TRANS_INFO_T));
    
    if(MDLWriteInfo(&InfoDataQueue, &info_data) < 0)
        return -1;
    //写到发送队列
    memcpy_r((uint8*)&modeNo,info_data.aPayloadData,4);
    if(modeNo > 5 || modeNo < 1)
    {
        return -1;
    }
    transInfo.DTA = 0x02;
    transInfo.DTB = 0x00;
    transInfo.seq = info_data.MsgIndex & 0xff;
    transInfo.clkSetFlag = 1;
    PlcWriteQueue(&LockSendBuf[modeNo-1], (uint8*)&transInfo, sizeof(TRANS_INFO_T));
    return 1;

}


/*******************************************************************************
* 函数名称: ModulePowerOff
* 函数功能: 模组断电或上电
* 输入参数: moduleNo【槽位号】 OnorOff【0 断电 1 上电】
* 输出参数: 
* 返 回 值: 0 成功 其他 失败
*******************************************************************************/
int ModulePowerOff(int moduleNo, uint8 OnorOff)
{
    if(dev_power == NULL)
    {
        MDLMGR_FMT_DEBUG(moduleNo - 1, "dev_power is null\n");
        return -1;
    }
    if(OnorOff == 1)    //On power
    {
        if(dev_power->power_ply_vcc_out_set(dev_power, (uint8)moduleNo, POWER_ON) < 0)
        {
            MDLMGR_FMT_DEBUG(moduleNo - 1, "槽位[%d]上电失败\n", moduleNo);
            return -1;
        }
    }
    else
    {
        if(dev_power->power_ply_vcc_out_set(dev_power, (uint8)moduleNo, POWER_OFF) < 0)
        {
            MDLMGR_FMT_DEBUG(moduleNo - 1, "槽位[%d]断电失败\n", moduleNo);
            return -1;
        }
    }
    return 0;
}

/**********************************************************************
* @name      : mdl_ctrl_buzzer
* @brief     ：控制蜂鸣器
* @param[in] ：uint8 bOpen   是否打开
* @param[out]：
* @return    ：
* @Update    :
**********************************************************************/
void mdl_ctrl_buzzer(uint8 bOpen)
{
    AUDIO_DEVICE_T* dev = (AUDIO_DEVICE_T*)hal_device_get(HW_DEVICE_ID_AUDIO);
    if(NULL == dev)
    {
        MDLMGR_FMT_DEBUG(mdlnum, "Audio dev is null\n");
        return;
    }

    if(FALSE == bOpen)
    {
        dev->audio_set_buzzer_out(dev, 0, 1, 1);
    }
    else
    {
        dev->audio_set_buzzer_out(dev, 1, 1, 1); 
    }
    hal_device_release((HW_DEVICE *)dev);

    return;
}

/**********************************************************************
* @name      : mdl_open_buzzer
* @brief     ：控制蜂鸣器
* @param[in] ：delay   延时关闭
* @param[out]：
* @return    ：
* @Update    :
**********************************************************************/
void mdl_open_buzzer(uint32 delay)
{
    if (gBuzzerEn == TRUE)
    {
        mdl_ctrl_buzzer(TRUE);
        usleep(1000*delay);
        mdl_ctrl_buzzer(FALSE);
    }

    return;
}

/*******************************************************************************
* 函数名称: GetModuleInfo
* 函数功能: 模组断电重启
* 输入参数: 
* 输出参数: 
* 返 回 值: 
*******************************************************************************/
int RestartModule(INFO_DATA_T info_data)
{
    uint32          modeNo = 0;
    uint32          PowOffTime = 0;     //断电时长
    TRANS_INFO_T    transInfo;
    INFO_DATA_T     infoDataTemp;
    uint8           result = 1;
    //uint8         i;
    
    memset(&transInfo,0x0,sizeof(TRANS_INFO_T));
    
//  if(MDLWriteInfo(&InfoDataQueue, &info_data) < 0)
//      return -1;
    //写到发送队列
    //modCnt = info_data.aPayloadData[0];
    if(info_data.nPayloadLength != 8)
    {
        MDLMGR_FMT_DEBUG(mdlnum, "app接收到的模组重启消息数据长度=%d,异常\n",info_data.nPayloadLength);
        result = 0; 
    }
    else
    {
        memcpy_r((uint8*)&modeNo,&info_data.aPayloadData[0],4);
        if(modeNo > 5 || modeNo < 1)
        {
            MDLMGR_FMT_TRACE(mdlnum, "error modeNo = %d\n",modeNo);
            result = 0; 
        }
        memcpy_r((uint8*)&PowOffTime, &info_data.aPayloadData[4], 4);
        MDLMGR_FMT_DEBUG(mdlnum, "槽位[modeNo]开始断电，断电时间[%d]\n", modeNo, PowOffTime);
        if(ModulePowerOff(modeNo,0) < 0 )
        {
            MDLMGR_FMT_DEBUG(mdlnum, "模组[%d]重启失败!\n", modeNo);
            result = 0;             //失败
        }
    }

    if(result == 0)
    {
        MDLMGR_FMT_DEBUG(mdlnum, "槽位[%d]断电失败%02x\n", modeNo, result);
        memcpy(&infoDataTemp,&info_data,sizeof(INFO_DATA_T));
        infoDataTemp.MsgRPM = 0;
        infoDataTemp.nPayloadLength = 1;
        infoDataTemp.aPayloadData[0] = result;
        PlcWriteQueue(&LockRecvBuf, (uint8*)&infoDataTemp, sizeof(INFO_DATA_T));
    }
    else        //保存断电控制信息，上电后成功后再回码
    {
        modPowerCtl[modeNo-1].offpowerflag = 1;
        modPowerCtl[modeNo-1].offpowertime = (time_t)PowOffTime;
        memcpy(&modPowerCtl[modeNo-1].infotemp,&info_data,sizeof(INFO_DATA_T));

        //先回复模组重启消息
        memcpy(&infoDataTemp,&info_data,sizeof(INFO_DATA_T));
        infoDataTemp.MsgRPM = 0;
        infoDataTemp.nPayloadLength = 1;
        infoDataTemp.aPayloadData[0] = result;
        PlcWriteQueue(&LockRecvBuf, (uint8*)&infoDataTemp, sizeof(INFO_DATA_T));
    }

    return 1;
}

//APP文件传输设置
int FileSend(INFO_DATA_T info_data)
{
    uint32 modeNo = 0;
    TRANS_INFO_T    transInfo;
    INFO_DATA_T     infoDataTemp;
    
    char fileName[100] = {0};
    char actBuffer[200] = {0};
    uint8 nameLen = 0;
    char fileSend[100] = {0};

    memset(&transInfo,0x0,sizeof(transInfo));
    memset(&infoDataTemp,0x0,sizeof(infoDataTemp));

    memcpy_r((uint8*)&modeNo, info_data.aPayloadData,4);
    nameLen = info_data.aPayloadData[4];
    memcpy(fileName, &info_data.aPayloadData[5], nameLen);
    MDLUP_FMT_DEBUG("FileSend接收到模组序号%d 的升级文件路径:%s, 文件名长度[%d], 准备升级... \n",modeNo, fileName, nameLen);

    //启动升级线程
    sprintf(fileSend,"%s_%d", MDL_UPDATE_FILE, modeNo - 1);
    sprintf(actBuffer,"cp %s %s", fileName, fileSend);
    MDLUP_FMT_DEBUG("复制升级文件为: %s\n", fileSend);
    if(system(actBuffer) < 0)
    {
        MDLUP_FMT_DEBUG("app路径升级文件%s执行失败!\n",actBuffer);
        
        memcpy(&infoDataTemp,&info_data,sizeof(INFO_DATA_T));
        infoDataTemp.MsgRPM = 0;
        infoDataTemp.nPayloadLength = 1;
        infoDataTemp.aPayloadData[0] = 0;
        PlcWriteQueue(&LockRecvBuf, (uint8*)&infoDataTemp, sizeof(INFO_DATA_T));
        return -1;
    }

    memcpy(&infoDataTemp,&info_data,sizeof(INFO_DATA_T));
    infoDataTemp.MsgRPM = 0;
    infoDataTemp.nPayloadLength = 1;
    infoDataTemp.aPayloadData[0] = 1;
    PlcWriteQueue(&LockRecvBuf, (uint8*)&infoDataTemp, sizeof(INFO_DATA_T));
    return 1;
}

int GetVendorVerInfo(INFO_DATA_T info_data)
{
    char*   pAppName    ="mapManager";
    char*   pVersion    ="XL01" ;
    char*   pVendor     ="XLDZ" ;
    char*   pRealseData ="XL-2020-05-30";
    uint8   dataBuf[256] = {0};
    int     offset = 0;
    INFO_DATA_T     infoDataTemp;
    
    memset(&infoDataTemp,0x0,sizeof(INFO_DATA_T));
    
    dataBuf[offset++] = strlen(pAppName);
    memcpy(&dataBuf[offset],pAppName,strlen(pAppName));
    offset += strlen(pAppName);
    dataBuf[offset++] = strlen(pVersion);
    memcpy(&dataBuf[offset],pVersion,strlen(pVersion));
    offset += strlen(pVersion);
    dataBuf[offset++] = strlen(pVendor);
    memcpy(&dataBuf[offset],pVendor,strlen(pVendor));
    offset += strlen(pVendor);
    dataBuf[offset++] = strlen(pRealseData);
    memcpy(&dataBuf[offset],pRealseData,strlen(pRealseData));
    offset += strlen(pRealseData);

    memcpy(&infoDataTemp,&info_data,sizeof(INFO_DATA_T));
    infoDataTemp.MsgRPM = 0;
    infoDataTemp.nPayloadLength = offset;
    memcpy(infoDataTemp.aPayloadData,dataBuf,infoDataTemp.nPayloadLength);
    PlcWriteQueue(&LockRecvBuf, (uint8*)&infoDataTemp, sizeof(INFO_DATA_T));
    return 1;

}

/*******************************************************************************
* 函数名称: CreateTaskByInfo
* 函数功能: 根据收到的消息产生任务
* 输入参数: 无
* 输出参数: 无
* 返 回 值: >0 成功
            <=0 失败
*******************************************************************************/
int8 CreateTaskByInfo(void)
{
    int nGetRet = 0;
    
    INFO_DATA_T stInfoData;  //消息
    memset(&stInfoData, 0x00, sizeof(stInfoData));
    
    nGetRet = GetInfoDataFromList(&stInfoData);
    //printf("\nCreateTaskByInfo:nGetRet %d, stInfoData.IID_ID = %04x, stInfoData.IOP_ID = %04x\n",nGetRet,stInfoData.IID_ID,stInfoData.IOP_ID);
    if(nGetRet < 0)
    {
        return -1;
    }
    //执行消息
    if(stInfoData.IID_ID != 0x0004 && stInfoData.IID_ID != 0x1155)
    {
        return -1;
    }
    
    if(stInfoData.IID_ID == 0x1155 && stInfoData.IOP_ID == 0x9090)
    {
        MDLMGR_FMT_DEBUG(mdlnum, "SrcAddr[%s]->DstAddr[%s]:查询厂商版本信息\n", stInfoData.SourAddr, stInfoData.DestAddr);
        return GetVendorVerInfo(stInfoData);
    }
    else if(stInfoData.IID_ID == 0x0004)
    {
    switch (stInfoData.IOP_ID)
    {
        case 0x1010://查询模组信息(新协议)
        case 0x0010://查询模组信息
        {
            MDLMGR_FMT_DEBUG(mdlnum, "SrcAddr[%s]->DstAddr[%s]:查询模组信息 IOP[0x%04x]\n", stInfoData.SourAddr, 
                stInfoData.DestAddr, stInfoData.IOP_ID);
            return GetModuleInfo(stInfoData);
        }
        case 0x0011://查询模组槽位信息
        {
            MDLMGR_FMT_DEBUG(mdlnum, "SrcAddr[%s]->DstAddr[%s]:查询模组槽位信息 IOP[0x%04x]\n", stInfoData.SourAddr, 
                stInfoData.DestAddr, stInfoData.IOP_ID);
            return GetModuleSlotInfo(stInfoData);
        }
        case 0x1012://查询模组插拔状态(新协议)
        case 0x0012://查询模组插拔状态
        {
            MDLMGR_FMT_DEBUG(mdlnum, "SrcAddr[%s]->DstAddr[%s]:查询模组插拔状态 IOP[0x%04x]\n", stInfoData.SourAddr, 
                stInfoData.DestAddr, stInfoData.IOP_ID);
            return GetModulePlugStatus(stInfoData);
        }
        case 0x0013://模组复位
        {
            MDLMGR_FMT_DEBUG(mdlnum, "SrcAddr[%s]->DstAddr[%s]:模组复位 IOP[0x%04x]\n", stInfoData.SourAddr, 
                stInfoData.DestAddr, stInfoData.IOP_ID);
            return ResetModule(stInfoData);
        }
        case 0x0014://模组重启
        {
            MDLMGR_FMT_DEBUG(mdlnum, "SrcAddr[%s]->DstAddr[%s]:模组重启 IOP[0x%04x]\n", stInfoData.SourAddr, 
                stInfoData.DestAddr, stInfoData.IOP_ID);  
            return RestartModule(stInfoData);
        }
        case 0x0015://文件传输
        {
            MDLMGR_FMT_DEBUG(mdlnum, "SrcAddr[%s]->DstAddr[%s]:文件传输 IOP[0x%04x]\n", stInfoData.SourAddr, 
                stInfoData.DestAddr, stInfoData.IOP_ID);
            return FileSend(stInfoData);
        }
        case 0x0016://查询所有模组信息
        {
            MDLMGR_FMT_DEBUG(mdlnum, "SrcAddr[%s]->DstAddr[%s]:查询模组所有信息 IOP[0x%04x]\n", stInfoData.SourAddr, 
                stInfoData.DestAddr, stInfoData.IOP_ID);
            return GetModuleInfoAll(stInfoData);
        }
        default:
        {
            MDLMGR_FMT_DEBUG(mdlnum, "SrcAddr[%s]->DstAddr[%s]:IOP[0x%04x] IOP[0x%04x] 类型错误\n", stInfoData.SourAddr, 
                stInfoData.DestAddr, stInfoData.IID_ID, stInfoData.IOP_ID);
            return -1;
            break;
        }           
        
    }

    }

    return 1;
}

/*******************************************************************************
* 函数名称: MDLWriteInfo
* 函数功能: 保存处理中的mqtt帧信息
* 输入参数：
* 输出参数：
* 内部处理: 若队列不满，将buf中的数据放入队列中；若队列满，丢弃该数据
* 返 回 值: >=   0                   成功
            <  0                失败
*******************************************************************************/
int32 MDLWriteInfo(InfoDataQueue_T *pInfoDataQueue, INFO_DATA_T *pInfoData)
{
    int i;
    int ret = -1;
    //锁缓冲区
    pthread_mutex_lock(&pInfoDataQueue->FrameLock);

    //判队列中是否还有空的位置
    for(i = 0; i < MAX_INFO_QUEUE_CNT; i++)
    {
         if(*(pInfoDataQueue->flag + i) == 0)
         {
            memcpy((pInfoDataQueue->InfoData + i),pInfoData,sizeof(INFO_DATA_T));
            *(pInfoDataQueue->flag + i) = 1;
            ret = 1;
            break;
         }
    }
    //解缓冲区
    if(ret != 1)
    {
        MDLMGR_FMT_TRACE(mdlnum, "%s 写入失败!\n", __FUNCTION__);
    }
    pthread_mutex_unlock(&pInfoDataQueue->FrameLock);

    return ret;
}

/*******************************************************************************
* 函数名称: MDLReadInfo
* 函数功能: 匹配处理中的mqtt信息
* 输入参数：
* 输出参数：
* 内部处理: 判队列中是否有数据，有则从队列中取出数据放入buf中，每执行一次，取一个数据
* 返 回 值: >= 0                 成功
            <  0              失败
*******************************************************************************/
int32 MDLReadInfo(InfoDataQueue_T *pInfoDataQueue, uint8 frameIndex, INFO_DATA_T *pInfoData)
{
    int     i = -1;

    //锁缓冲区
    pthread_mutex_lock(&pInfoDataQueue->FrameLock);

    //若队列中有数据，则取到buf中，解开锁
    for(i = 0; i < MAX_INFO_QUEUE_CNT; i++)
    {
         if(*(pInfoDataQueue->flag + i) == 0)
            continue;
         
         if(frameIndex != (((pInfoDataQueue->InfoData+i)->MsgIndex)&0xff)   )   //只取了低字节
            continue;
         
         memcpy(pInfoData,(pInfoDataQueue->InfoData+i),sizeof(INFO_DATA_T));
         memset(pInfoDataQueue->InfoData+i, 0x00,sizeof(InfoDataQueue_T));
         *(pInfoDataQueue->flag + i) = 0;                                   //置标志位无效
         MDLMGR_FMT_TRACE(mdlnum, "mqtt headinfo缓冲中第%d满足\n",i);
         break;
    }

    if(i == MAX_INFO_QUEUE_CNT)
    {
        MDLMGR_FMT_TRACE(mdlnum, "mqtt headinfo 缓冲满,清空缓冲\n");
        memset(pInfoDataQueue->InfoData,0x0,sizeof(INFO_DATA_T)*MAX_INFO_QUEUE_CNT);
        memset(pInfoDataQueue->flag,0x0,sizeof(pInfoDataQueue->flag));
        pthread_mutex_unlock(&pInfoDataQueue->FrameLock);
        return -1;
    }
    
    pthread_mutex_unlock(&pInfoDataQueue->FrameLock);
    return i;
}

int GetHeadInfo(INFO_DATA_T InfoMsgData, MSG_INFO_T *ptMsgInfo)
{   
    //head
    ptMsgInfo->IP = InfoMsgData.IP;
    ptMsgInfo->port = InfoMsgData.port;
    ptMsgInfo->MsgRPM = InfoMsgData.MsgRPM;
    ptMsgInfo->MsgPRIORITY = InfoMsgData.MsgPRIORITY;       
    ptMsgInfo->MsgIndex = InfoMsgData.MsgIndex;
    ptMsgInfo->MsgLabel = InfoMsgData.MsgLabel;
    ptMsgInfo->SourLen = InfoMsgData.DestLen;
    memcpy(ptMsgInfo->SourAddr, InfoMsgData.DestAddr, ptMsgInfo->SourLen);
    ptMsgInfo->DestLen = InfoMsgData.SourLen;
    memcpy(ptMsgInfo->DestAddr, InfoMsgData.SourAddr, ptMsgInfo->DestLen);

    //msg
    ptMsgInfo->IID = InfoMsgData.IID_ID;
    ptMsgInfo->IOP = InfoMsgData.IOP_ID;
    ptMsgInfo->MsgLen = (uint32)InfoMsgData.nPayloadLength;
    memcpy(ptMsgInfo->MsgData, InfoMsgData.aPayloadData, ptMsgInfo->MsgLen);
    
    return 1;
}

//打印完整消息（head+msg)
void PrintInfoDetail(MSG_INFO_T *stMsgInfoSend)
{
    uint32 IP;        //目的端ip
    uint16 port;
    
    uint8  MsgRPM;         /* 启动标志位, 1-启动, 0-非启动 */
    uint8  MsgPRIORITY;    /* 优先级 */
    uint16 MsgIndex;       /* 消息序号 */
    uint16 MsgLabel;       /* 消息标签 */

    uint8  SourLen;
    uint8  DestLen;
    char   SourAddr[128] = {0};
    char   DestAddr[128] = {0};

    uint16 IID;
    uint16 IOP;

    uint32 MsgLen; 
    uint8  MsgData[65535];

    IP = stMsgInfoSend->IP;
    port = stMsgInfoSend->port;

    MsgRPM = stMsgInfoSend->MsgRPM;
    MsgPRIORITY = stMsgInfoSend->MsgPRIORITY;
    MsgIndex = stMsgInfoSend->MsgIndex;
    MsgLabel = stMsgInfoSend->MsgLabel;
    SourLen = stMsgInfoSend->SourLen;
    DestLen = stMsgInfoSend->DestLen;
    memcpy(SourAddr, stMsgInfoSend->SourAddr, SourLen);
    memcpy(DestAddr, stMsgInfoSend->DestAddr, DestLen);
    IID = stMsgInfoSend->IID;
    IOP = stMsgInfoSend->IOP;
    MsgLen = stMsgInfoSend->MsgLen;
    memcpy(MsgData, stMsgInfoSend->MsgData, MsgLen);

    MDLMSG_FMT_TRACE("打印消息的完整内容为:\n");
    MDLMSG_FMT_TRACE("IP:%x, port:%d, MsgRPM:%d, MsgPRIORITY:%d, MsgIndex:%d, MsgLabel:%d, SourLen:%d, DestLen:%d, IID:%04x, IOP:%04x, MsgLen:%d. \n", 
        IP, port, MsgRPM, MsgPRIORITY,MsgIndex, MsgLabel, SourLen, DestLen, IID, IOP, MsgLen);
    MDLMSG_FMT_TRACE("SourAddr:%s\n", SourAddr);
    MDLMSG_FMT_TRACE("DestAddr:%s\n", DestAddr);
    MDLMSG_BUF_TRACE(MsgData, MsgLen, "MsgData:");
    MDLMSG_FMT_TRACE("消息的完整内容打印完毕. \n");
}

//报文buff转换为string打印，添加空格
void CharArrToString(char *buf, int len, int maxlen, char *comm_log)
{
    int i;
    char *p;
  
    p = comm_log;
    
    if(len <= 0)
    {
        return;
    }
        
    if (len < maxlen)
    {
        for (i=0; i<len; i++)
        {   
            sprintf(p+3*i, "%02x ", buf[i]);
        }
    }
    else
    {
        for (i=0; i<maxlen; i++)
        {   
            sprintf(p+3*i, "%02x ", buf[i]);
        }
    }
}

/*******************************************************************************
* 函数名称: PlcWriteQueue
* 函数功能: 写队列
* 输入参数：plockqueue          锁缓冲区结构指针
            buf                 需要写入队列的数据
            buf_len             数据长度
* 输出参数：无
* 内部处理: 若队列不满，将buf中的数据放入队列中；若队列满，丢弃该数据
* 返 回 值: >0                  成功
            =0                  失败
*******************************************************************************/
int32 PlcWriteQueue(LockQueue_T *pLockQueue, uint8 *buf, int32 buf_len)
{
    int32 flags = 0;

    //锁缓冲区
    pthread_mutex_lock(&pLockQueue->QueueLock);
    
    //判队列中是否还有空的位置
    if (pLockQueue->Queue[pLockQueue->QueueTail].QueueLen == 0)
    {   
        pLockQueue->Queue[pLockQueue->QueueTail].QueueLen = buf_len;
        memcpy(pLockQueue->Queue[pLockQueue->QueueTail].QueueBuf, buf, buf_len);
        flags = 1;
        pLockQueue->QueueTail = (pLockQueue->QueueTail + 1) % (MAX_QUEUE_CNT);
    }
    else
    {
        MDLMGR_FMT_TRACE(mdlnum, "PlcWriteQueue:队列满\n");
    }

    //解缓冲区
    pthread_mutex_unlock(&pLockQueue->QueueLock);
    return flags;
}

/*******************************************************************************
* 函数名称: PlcWriteQueue
* 函数功能: 读队列
* 输入参数：pLockQueue          锁缓冲区结构指针
*           buf                 读数据缓冲区，将队列中的数据取出放入该缓冲区
* 输出参数：plockqueue          队列中的数据
* 内部处理: 判队列中是否有数据，有则从队列中取出数据放入buf中，每执行一次，取一个数据
* 返 回 值: len                 读到的数据长度
*******************************************************************************/
int32 PlcReadQueue(LockQueue_T *pLockQueue, uint8 *buf)
{
    int32 len = 0;
    uint8 *pdata_buf;

    //锁缓冲区
    pthread_mutex_lock(&pLockQueue->QueueLock);

    //若队列中有数据，则取到buf中，解开锁
    if ((pLockQueue->QueueHead != pLockQueue->QueueTail)
        || ((pLockQueue->QueueHead == pLockQueue->QueueTail)
            && (pLockQueue->Queue[pLockQueue->QueueHead].QueueLen != 0)))
    {
        pdata_buf = pLockQueue->Queue[pLockQueue->QueueHead].QueueBuf;
        len = pLockQueue->Queue[pLockQueue->QueueHead].QueueLen;

        //取队列中的数据
        memcpy(buf, pdata_buf, len);

        //清数据
        memset(pdata_buf, 0, len);
        pLockQueue->Queue[pLockQueue->QueueHead].QueueLen = 0;

        //队列头向后移动一位
        pLockQueue->QueueHead= (pLockQueue->QueueHead + 1) % (MAX_QUEUE_CNT);
    }

    pthread_mutex_unlock(&pLockQueue->QueueLock);

    return len;
}

//报文buff转换为string打印，没有空格
void ComposeBuf(uint8 *buf, int len, uint8 *comm_log)
{
    int i;
    char *p;
  
    p = (char*)comm_log;
  
    if (len < 1500)
    {
        for (i=0; i<len; i++)
        {   
            sprintf(p+3*i, "%02x ", buf[i]);   
        }
    }
    else
    {
        for (i=0; i<1500; i++)
        {   
            sprintf(p+3*i, "%02x ", buf[i]);
        }
    }
}


/*******************************************************************************
* 函数名称: GetInfoIndex
* 函数功能: 主动上送消息的index
* 输入参数: 
* 输出参数: 
* 返 回 值: 
*******************************************************************************/
uint16 GetInfoIndex()
{
    uint16 index =0 ;
    pthread_mutex_lock(&ActiveInfoIndexLk);
    index = ActInfIndex++;
    pthread_mutex_unlock(&ActiveInfoIndexLk);
    return index;
}

uint8  BINtoASCII(uint8* InBuf, uint16 len, char* OutBuf, uint16* outlen)
{
    int i;
    char temp[2000] = {0};
    uint16 offset = 0;
    
    
    if(InBuf == NULL || OutBuf == NULL)
    {
        return -1;
    }

    for (i = 0; i< len; i++)
    {
        if(InBuf[i] > 99)
        {
            temp[offset] = InBuf[i]/100 + 0x30; 
            temp[offset+1] = (InBuf[i]/10)%10 + 0x30;
            temp[offset+2] = InBuf[i]%10 + 0x30;
            offset += 3;
        }
        else
        {
            temp[offset] = InBuf[i]/10 + 0x30;
            temp[offset+1] = InBuf[i]%10 + 0x30;
            offset += 2;
        }
    }
    memcpy(OutBuf,temp,offset);
    *outlen = offset;
    return 1;
}

int GetPlcFileSize(const char *filename)
{
    int     fd = -1;
    int     filesize = -1;
    struct  stat    sta;

    if((fd = open(filename,O_RDONLY)) < 0)
    {
        return -1;
    }

    if(fstat(fd,&sta) < 0)
    {
        return -1;
    }
    filesize = sta.st_size;

    close(fd);

    return filesize;
}

void WritePlcFile(const char *filepath,const char *string, int MaxSize)
{
    char pathsub[256];
    char cmd[256];

    if(GetPlcFileSize(filepath) > MaxSize)
    {
        memset(pathsub,0x0,sizeof(pathsub));
        strcat(pathsub,filepath);
        strcat(pathsub,".sub");
        remove(pathsub);
        rename(filepath,pathsub);
        sprintf(cmd,"mv -f %s %s",pathsub,"/data/log/");
        system(cmd);
    }

    FILE *logFile = fopen(filepath,"at");
    if(logFile)
    {
        fprintf(logFile,"%s",string);
        fclose(logFile);
    }
}

void writePlclog(const char* filepath, const char *fmt, ...)
{
    char    debug_msg[5000];
    int     len;
    struct  timeval tp;
    struct  tm  *p_tm;
    char    debug_buf[5200];

    //处理参数
    va_list ap;
    va_start(ap,fmt);


    vsnprintf(debug_msg,sizeof(debug_msg),fmt,ap);
    va_end(ap);

    //记录日志时间
    gettimeofday(&tp,NULL);
    p_tm = localtime(&(tp.tv_sec));
    sprintf(debug_buf, "%d %02d/%02d %02d:%02d:%02d:%ld ", p_tm->tm_year+1900
        , p_tm->tm_mon+1, p_tm->tm_mday, p_tm->tm_hour, p_tm->tm_min, p_tm->tm_sec,tp.tv_usec);

    strcpy(debug_buf+strlen(debug_buf),debug_msg);
    len = strlen(debug_buf);
    *(debug_buf+len) = '\n';
    *(debug_buf+len+1) = 0x00;
    WritePlcFile(filepath,debug_buf,LOG_MAX_SIZE);
}

/**********************************************************************
* @name      : mdl_comm_init
* @brief     ：初始化串口接口
* @param[in] ：PULSE_RS232_PARA *pCommPara  本地通讯参数
* @param[out]：
* @return    ：0-成功/失败
* @Update    :
**********************************************************************/
int mdl_comm_init(MDL_COMM_PARA *pCommPara)
{
    int32 ret = -1;
    UART_DEVICE_T* dev;
    dev = (UART_DEVICE_T*)hal_device_get(pCommPara->deviceName);
    if(NULL == dev)
    {
        return -1;    
    }
    
    // 打开设备
    ret = dev->uart_param_set(dev, 9600, 8, 1, PARITY_NO);
    if (ret != 0)
    {
        hal_device_release((HW_DEVICE *)dev);
        return -2; 
    }
    pCommPara->pDev = dev;
    return 0;
}


/**********************************************************************
* @name      : mdl_comm_uninit
* @brief     ：去初始化串口接口
* @param[in] ：PULSE_RS232_PARA *pCommPara  本地通讯参数
* @param[out]：
* @return    ：0-成功/失败
* @Update    :
**********************************************************************/
void mdl_comm_uninit(MDL_COMM_PARA *pCommPara)
{
    if(NULL == pCommPara || NULL == pCommPara->pDev)
    {
        return;
    }
    
    hal_device_release((HW_DEVICE *)pCommPara->pDev);
    pCommPara->pDev = NULL;
}


/**********************************************************************
* @name      : pulse_RS232_send
* @brief     ：串口数据接收接口
* @param[in] ：PULSE_RS232_PARA *pCommPara  本地通讯参数
               uint8* pBuffer               发送数据缓冲区
               int nBufLen                  发送长度
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2020-3-10
* @Update    :
**********************************************************************/
int mdl_comm_send(MDL_COMM_PARA *pCommPara, uint8* pBuffer, int nBufLen)
{
    UART_DEVICE_T* dev = (UART_DEVICE_T*)pCommPara->pDev;
    if (dev == NULL)
    {
        return -2;
    }

    if(0 <= dev->uart_data_send(dev, pBuffer, nBufLen))
    {
        return 0;
    }
    return -1;
}

/**********************************************************************
* @name      : pulse_RS232_recv
* @brief     ：串口数据接收接口
* @param[in] ：PULSE_RS232_PARA *pCommPara  本地通讯参数
               int nBufLen                  最大接收长度
               int rsptime                  数据返回时间(20ms)
               int rcvtime                  数据字节之间间隔时间(20ms)
               uint8 endbyte                结束符，没有填0xff
* @param[out]：uint8* pBuffer               接收数据缓冲区
* @return    ：接收到数据的长度
* @Create    : 王津剑
* @Date      ：2020-3-10
* @Update    :
**********************************************************************/
int mdl_comm_recv(MDL_COMM_PARA *pCommPara, uint8* pBuffer, int nBufLen, int rsptime, int rcvtime, uint8 endbyte)
{
    int recv_len = 0;        /*接收到数据长度*/
    int data_len = 0;        /*串口缓冲区中数据的长度*/
    int count = 0;
    fd_set set;
    struct timeval timeout;
    int n = 0;
    int fd = 0;

    UART_DEVICE_T* dev = (UART_DEVICE_T*)pCommPara->pDev;
    if (dev == NULL)
    {
        return 0;
    }

    recv_len = 0;
    data_len = 0;
    
    fd = dev->uart_get_handle(dev);
    if(fd <= 0)
    {
        return 0;
    }

    while (1)
    {
        FD_ZERO(&set);
        FD_SET(fd, &set);
        timeout.tv_sec = 0;
        timeout.tv_usec = 20000;
        do 
        {
            n = select(fd + 1, &set, NULL, NULL, &timeout);
        }while(n < 0);

        if (n > 0 && FD_ISSET(fd, &set))
        {
            recv_len = dev->uart_data_recv(dev,pBuffer+data_len, nBufLen-data_len);
        }
        else
            recv_len = 0;

        if (recv_len > 0)    
        {
            data_len += recv_len;
            count = 0;

            if(endbyte != 0xff)
            {
                if(memchr(pBuffer+data_len-recv_len, endbyte, recv_len) != NULL) 
                {
                    usleep(20000);
                    return data_len;                    
                }
            }
        }
        else
        {
            if((data_len == 0)&&(count>rsptime)) 
            {
                usleep(20000);
                return data_len; /*-接收数据超时-*/
            }

            if((data_len != 0)&&((count>rcvtime)||(data_len>nBufLen)))
            {
                usleep(20000);
                return data_len;
            }
        }
        count++;
    }
}

