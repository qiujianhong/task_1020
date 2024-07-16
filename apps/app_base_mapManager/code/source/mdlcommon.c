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

//�յ�����Ϣ������
INFO_DATA_LIST_T gInfoDataList_mdl = {0};                               //�յ�����Ϣ�������б�

InfoDataQueue_T InfoDataQueue;                                          //�Ѵ���mqtt��Ϣ֡��Ϣ
LockQueue_T     LockRecvBuf;                                            //mqtt���ջ�����
LockQueue_T     LockSendBuf[mdlnum];                                    //mqtt���ͻ�����

pthread_mutex_t ActiveInfoIndexLk;
uint16  ActInfIndex;

/*******************************************************************************
* ��������: AddInfoDataIntoList
* ��������: �����Ϣ������
* �������: ��Ϣmsg�ṹ��
* �������: ��
* �� �� ֵ: 
*******************************************************************************/
int8 AddInfoDataIntoList(INFO_DATA_T RecvInfoData)
{
    uint16 nAddIndex = 0;
    
    //���յ�������Ϣ�ݴ滺����
    if(gInfoDataList_mdl.InfoDataNum < MAX_INFO_DATA_NUM)
    {
        nAddIndex = (gInfoDataList_mdl.InfoDataReadIndex+gInfoDataList_mdl.InfoDataNum) % MAX_INFO_DATA_NUM;//ѭ��ȡ��ӵ�ַ�������±꣩
        //MDLMGR_FMT_DEBUG(mdlnum, "AddMsgIntoList()���֮ǰ...stInfoMsgDataList.InfoMsgDataNum=[%d]...begin\n", gInfoDataList_mdl.InfoDataNum);            
        
        gInfoDataList_mdl.InfoDataNum ++;
        gInfoDataList_mdl.InfoDataList[nAddIndex] = RecvInfoData;

        //MDLMGR_FMT_DEBUG(mdlnum, "AddMsgIntoList()���֮��...stInfoMsgDataList.InfoMsgDataNum=[%d]...end\n", gInfoDataList_mdl.InfoDataNum);
    }
    else
    {
        MDLMGR_FMT_DEBUG(mdlnum, "CreateTaskByInfo()...stInfoMsgDataList.InfoMsgDataNum ���ڵ��� MAX_MSG_INFO_NUM����ȴ�...\n");
        return -1;
    }

    return 1;
}

/*******************************************************************************
* ��������: GetInfoDataFromList
* ��������: �Ӷ�����ȡ����Ϣ
* �������: 
* �������: ��Ϣmsg�ṹ��
* �� �� ֵ: 
*******************************************************************************/
int8 GetInfoDataFromList(INFO_DATA_T *SendInfoData)
{
    //�ж���Ϣ�����Ƿ�Ϊ��
    if(gInfoDataList_mdl.InfoDataNum > 0)
    {   
        //MDLMGR_FMT_DEBUG(mdlnum, "CreateTaskByInfo()ȡ��֮ǰ...�յ���Ϣ������=[%d]...begin\n", gInfoDataList_mdl.InfoDataNum);

        //�����Ƚ��ȳ���˳��ȡ����Ϣ
        *SendInfoData = gInfoDataList_mdl.InfoDataList[gInfoDataList_mdl.InfoDataReadIndex];
        gInfoDataList_mdl.InfoDataNum --;
        gInfoDataList_mdl.InfoDataReadIndex = (gInfoDataList_mdl.InfoDataReadIndex+1) % MAX_INFO_DATA_NUM;

        //MDLMGR_FMT_DEBUG(mdlnum, "CreateTaskByInfo()ȡ��֮��...�յ���Ϣ������=[%d]...end\n", gInfoDataList_mdl.InfoDataNum);
        
    }
    else
    {
        //MDLMGR_FMT_DEBUG(mdlnum, "uint32 CreateTaskByInfo()...stInfoMsgDataList.InfoMsgDataNum С�ڵ��� 0����ȴ�...\n");
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
    //�鱨��֡
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
    MDLMGR_FMT_DEBUG(usbNo, "��ģ������ʱ�ӣ�%d-%2d-%2d ����%d %02d:%02d:%02d:%04d\n",(int)date_time.year,(int)date_time.month,(int)date_time.day_of_month,
                                        (int)date_time.day_of_week,(int)date_time.hour,(int)date_time.minute,(int)date_time.second,(int)date_time.millisec);
    date_time.year          = htons(date_time.year);
    date_time.millisec      = htons(date_time.millisec);
    memcpy(pData,(uint8*)&date_time,sizeof(MDL_DATE_TIME_T));
    *dataLen = sizeof(MDL_DATE_TIME_T);
    return 1;
}

//ģ��ͨ����֡
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

    if(pRecvInfo->DTA == 0x00)          //ֻ��
    {   
        frameinfo.frameFunc = GETREQUEST;
        frameinfo.dataLen = 0x03;
        //char databuf[3] = {0};
        //DTA = (DTA1<<0x03) | (DTA2 & 0x07);
        switch (pRecvInfo->DTB)
        {
            case 0x00:  //ģ����Ϣ
            case 0x01:  //��·Э����Ϣ
            case 0x02:  //Э��汾
            case 0x03:  //��Դ״̬
            case 0x04:  //�ļ�����״̬
                dataBuf[0] = frameinfo.frameFunc;
                dataBuf[1] = pRecvInfo->DTA;
                dataBuf[2] = pRecvInfo->DTB;
                memcpy(frameinfo.pDataBuf,dataBuf,frameinfo.dataLen);
                return MakeModeFrame(&frameinfo, pSendBuf);
                //break;
            default:
                MDLMGR_FMT_TRACE(usbNo, "MakeFrame DTB %d ����\n",pRecvInfo->DTB);
                return -1;
                //break;
        }
    }
    else if(pRecvInfo->DTA == 0x01) //��д
    {
        if(pRecvInfo->DTB == 00)        //ʱ��
        {
            if(pRecvInfo->clkSetFlag)       //����ʱ��
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
            else                //��ȡʱ��
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
    else if(pRecvInfo->DTA == 0x02) //ֻд
    {
        frameinfo.frameFunc = SETREQUEST;
        switch (pRecvInfo->DTB)
        {
            case 0x00:      //Ӳ����ʼ��
            case 0x01:      //������ʼ��
            case 0x02:      //���ݳ�ʼ��
            
                dataBuf[0] = frameinfo.frameFunc;
                dataBuf[1] = pRecvInfo->DTA;
                dataBuf[2] = pRecvInfo->DTB;
                frameinfo.dataLen = 0x03;
                memcpy(frameinfo.pDataBuf,dataBuf,frameinfo.dataLen);
                return MakeModeFrame(&frameinfo, pSendBuf);
                //break;
                
            case 0x03:      //�ļ�����
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
                dataBuf[dwOffset++] = 0x82;             //����Ϊ2�ֽ�
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

    if(pRecvInfo->DTA == 0x40)      //��ң��
    {
        frameinfo.frameFunc = GETREQUEST;
        frameinfo.dataLen = 0x03;
        switch (pRecvInfo->DTB)
        {
            case 0x00:      //ң��״̬
            case 0x01:      //��һ�����������ʱ��
            case 0x02:      //���������ʱ��
                dataBuf[0] = frameinfo.frameFunc;
                dataBuf[1] = pRecvInfo->DTA;
                dataBuf[2] = pRecvInfo->DTB;
                memcpy(frameinfo.pDataBuf,dataBuf,frameinfo.dataLen);
                return MakeModeFrame(&frameinfo, pSendBuf);
                //break;
            default:
                MDLMGR_FMT_TRACE(usbNo, "ң�Ŷ���DTB %d \n",pRecvInfo->DTB);
                break;
        }
        
    }
    else if( pRecvInfo->DTA == 0x41)        //��дң��
    {
        if(pRecvInfo->clkSetFlag)           //��ʾ����
        {
            frameinfo.frameFunc = SETREQUEST;
            dataBuf[0] = frameinfo.frameFunc;
            dataBuf[1] = pRecvInfo->DTA;
            dataBuf[2] = pRecvInfo->DTB;
            switch (pRecvInfo->DTB)
            {
                case 0x00:      //ң�����幦������
                
                    dataBuf[3] = BIT_STRING;
                    memcpy(&dataBuf[4],pRecvInfo->setData,pRecvInfo->dataLen);
                    frameinfo.dataLen = 4 + pRecvInfo->dataLen;
                    memcpy(frameinfo.pDataBuf,dataBuf,frameinfo.dataLen);
                    return MakeModeFrame(&frameinfo, pSendBuf);
                    //break;
                    
                case 0x01:      //ң�ŷ���ʱ��
                
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
                case 0x00:      //ң�Ź������ö�
                case 0x01:      //ң�ŷ���ʱ��
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
        if(pRecvInfo->clkSetFlag)       //����
        {
            frameinfo.frameFunc = SETREQUEST;
            dataBuf[0] = frameinfo.frameFunc;
            dataBuf[1] = pRecvInfo->DTA;
            dataBuf[2] = pRecvInfo->DTB;
            switch(pRecvInfo->DTB)
            {
                case 0x00:              //�̵������ģʽ
                    dataBuf[3] = ENUM;
                    memcpy(&dataBuf[4],pRecvInfo->setData,pRecvInfo->dataLen);
                    frameinfo.dataLen = 4 + pRecvInfo->dataLen;
                    memcpy(frameinfo.pDataBuf,dataBuf,frameinfo.dataLen);
                    return MakeModeFrame(&frameinfo, pSendBuf);
                    //break;

                case 0x01:              //�̵������������
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
                case 0x00:              //�̵������ģʽ 
                case 0x01:              //�̵������������
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

    if(pRecvInfo->DTA == 0x80)      //ģ��������
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
    plug_status[usbNo].time = time(NULL);          //��ȡ��λ�ϱ�ʱ��
    
    infoDataTemp.MsgRPM = 1;                       //�����ϱ�
    infoDataTemp.MsgPRIORITY = 0;   
    infoDataTemp.MsgIndex = globleSeq++;
    strcpy(infoDataTemp.DestAddr,appName);
    infoDataTemp.DestLen = strlen(appName);        //���淢�ͻ����Դ/Ŀ��
    #if 0
    strcpy(infoDataTemp.SourAddr, smiosName);
    infoDataTemp.SourLen = strlen(smiosName);
    #else
    strcpy(infoDataTemp.SourAddr, "Broadcast");
    infoDataTemp.SourLen = strlen("Broadcast");
    #endif

    infoDataTemp.IID_ID = 0x0004;       
    infoDataTemp.IOP_ID = 0x0001;       //ģ�����¼�

    memcpy_r(payload, (uint8*)&slotNo, 4);
    offset += 4;
    
    if (plug_status[usbNo].plugStatus != ON_STATE)
    {
        payload[offset++] = 1;         //�γ�
    }
    else
    {
        payload[offset++] = 0;         //����

        #if PRTL_NEW_ENABEL == 1
        //��ģ����Ϣ����
        //ģ���豸�ͺ�
        payload[offset++] = pLinkInfo->modModeLen;
        memcpy(&payload[offset], pLinkInfo->modMode, pLinkInfo->modModeLen);      //ģ�����ʹ���·��Ϣȡ
        offset += pLinkInfo->modModeLen;
        MDLMGR_FMT_DEBUG(mdlnum, "usb[%d]-�ϱ������¼� ģ���豸�ͺ�: %s\n", usbNo, pLinkInfo->modMode);

        //ģ���豸ID
        payload[offset++] = pModuleInfo->DevIDLen;
        memcpy(&payload[offset], pModuleInfo->DevID, pModuleInfo->DevIDLen);
        offset += pModuleInfo->DevIDLen;

        //����汾����Э��
        if (pModuleInfo->type == MDL_PRTL_VER1)
        {
            sprintf(ver, "%s", pModuleInfo->softVer);
            payload[offset++] = strlen(ver) + 1;
            memcpy(&payload[offset], ver, strlen(ver) + 1);
            offset += strlen(ver) + 1;

            MDLMGR_FMT_DEBUG(mdlnum, "usb[%d]-�ϱ������¼� ����汾: %s\n", usbNo, pModuleInfo->softVer);
            MDLMGR_FMT_DEBUG(mdlnum, "usb[%d]-�ϱ������¼� ����������� %s\n", usbNo, pModuleInfo->softDate);

            //�����������
            asclen = strlen(pModuleInfo->softDate) + 1;
            payload[offset++] = asclen;
            memcpy(&payload[offset], pModuleInfo->softDate, asclen);
            offset += asclen;

            //Ӳ���汾����Э��
            memset(ver, 0, sizeof(ver));
            sprintf(ver, "%s", pModuleInfo->hardVer);
            payload[offset++] = strlen(ver) + 1;
            memcpy(&payload[offset], ver, strlen(ver) + 1);
            offset += strlen(ver) + 1;

            MDLMGR_FMT_DEBUG(mdlnum, "usb[%d]-�ϱ������¼� Ӳ���汾: %s\n", usbNo, pModuleInfo->hardVer);
            MDLMGR_FMT_DEBUG(mdlnum, "usb[%d]-�ϱ������¼� Ӳ���������� %s \n", usbNo, pModuleInfo->hardDate);

            //Ӳ����������
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

            MDLMGR_FMT_DEBUG(mdlnum, "usb[%d]-�ϱ������¼� ����汾: %04d-%02d-%02d-%02d\n", usbNo, pModuleInfo->date_soft.year,
            pModuleInfo->date_soft.month,pModuleInfo->date_soft.dayofmonth,pModuleInfo->date_soft.dayofweek);
            memcpy(datetemp, &pModuleInfo->date_soft, 5);
            datetemp[0] = pModuleInfo->date_soft.year/100;
            datetemp[1] = pModuleInfo->date_soft.year%100;
            BINtoASCII(datetemp, 5, dateAscii, &asclen);
            MDLMGR_FMT_DEBUG(mdlnum, "usb[%d]-�ϱ������¼� ����������� %s\n", usbNo, dateAscii);

            //�����������
            payload[offset++] = asclen;
            memcpy(&payload[offset],dateAscii,asclen);
            offset += asclen;

            //Ӳ���汾����Э��
            memset(ver, 0, sizeof(ver));
            sprintf(ver, "%d", pModuleInfo->hardwVersion);
            payload[offset++] = strlen(ver) + 1;
            memcpy(&payload[offset], ver, strlen(ver) + 1);
            offset += strlen(ver) + 1;

            MDLMGR_FMT_DEBUG(mdlnum, "usb[%d]-�ϱ������¼� Ӳ���汾: %04d-%02d-%02d-%02d\n", usbNo, pModuleInfo->date_hard.year,
                pModuleInfo->date_hard.month, pModuleInfo->date_hard.dayofmonth, pModuleInfo->date_hard.dayofweek);
            memcpy(datetemp,&pModuleInfo->date_hard, 5);
            datetemp[0] = pModuleInfo->date_hard.year/100;
            datetemp[1] = pModuleInfo->date_hard.year%100;
            BINtoASCII(datetemp, 5, dateAscii, &asclen);
            MDLMGR_FMT_DEBUG(mdlnum, "usb[%d]-�ϱ������¼� Ӳ���������� %s \n", usbNo, dateAscii);

            //Ӳ����������
            payload[offset++] = asclen;
            memcpy(&payload[offset], dateAscii, asclen);
            offset += asclen;
        }

        //���̱���
        payload[offset++] = pModuleInfo->VendorCodeLen;
        memcpy(&payload[offset],pModuleInfo->VendorCode,pModuleInfo->VendorCodeLen);
        offset += pModuleInfo->VendorCodeLen;

        //ͨ������
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
* @brief:        ˢ��״̬��
* @param[in]��   time    ��ʾʱ��
* @param[in]��   str     ��ʾ����
* @return��      NULL
*********************************************************************
*/
void refresh_status_bar(uint32 time, char *str)
{
    INFO_DATA_T infoDataTemp;
    char        payload[100];
    uint8       offset = 0;

    memset(payload, 0x0, sizeof(payload));
    memset(&infoDataTemp, 0x0, sizeof(INFO_DATA_T));

    //�����ϱ�
    infoDataTemp.MsgRPM = 1;        
    infoDataTemp.MsgPRIORITY = 0;   
    infoDataTemp.MsgIndex = globleSeq++;

    //���淢�ͻ����Դ/Ŀ��
    strcpy(infoDataTemp.DestAddr,appName);
    infoDataTemp.DestLen = strlen(appName);     
    strcpy(infoDataTemp.SourAddr, desktopName);
    infoDataTemp.SourLen = strlen(desktopName);

    //״̬������
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
* ��������: GetModuleInfo
* ��������: ��ѯ����ģ����Ϣ
* �������: 
* �������: 
* �� �� ֵ: 
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
            MDLMGR_FMT_DEBUG(mdlnum, "%s: ��Ϣ���� %d ����!\n",__FUNCTION__,offset + InfoSend.nPayloadLength);
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
* ��������: GetModuleInfo
* ��������: ��ѯģ����Ϣ
* �������: 
* �������: 
* �� �� ֵ: 
*******************************************************************************/
int GetModuleInfo(INFO_DATA_T info_data)
{
    uint32  slotNo = 0;
    INFO_DATA_T     InfoSend;
    MODULE_INF_T * pModInfo;
    
    memcpy_r((uint8*)&slotNo,info_data.aPayloadData,4);
    if(slotNo > 5 || slotNo < 1)
    {
        MDLMGR_FMT_DEBUG(mdlnum, "GetModuleInfo����ģ����Ϣ���к�usbNo %d ����\n",slotNo);
        return -1;
    }

    pModInfo = &moduleInfo_all[slotNo-1].moduleInfo;

    if (info_data.IOP_ID == 0x1010)
    {
        makeInfoData_new((uint8)(slotNo-1),pModInfo, &info_data,globleSeq++, &InfoSend);
    }
    else
    {
        //�ٲ�ģ����Ϣ���ȸĳ��¹淶��Ϣ
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
* ��������: GetModuleInfo
* ��������: ��ѯģ���λ��Ϣ
* �������: 
* �������: 
* �� �� ֵ: >= 0 �ɹ�
            <  0 ʧ��
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
            MDLMGR_FMT_DEBUG(mdlnum, "usb %d-û���豸�ڸò�λ�ϣ����Ƴ���:%d\n", i, slotInfo[i].modeTypeLen);
            continue;
        }

        MDLMGR_FMT_TRACE(mdlnum, "usb %d-�ͺ�����%s ��Ҫ��ѯ���ͺ�%s\n", i, slotInfo[i].modeType, &modtype[1]);
        if (info_data.nPayloadLength - 1 != modtype[0])
        {
            MDLMGR_FMT_DEBUG(mdlnum, "�յ��������ݳ�������!\n");
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
        MDLMGR_FMT_DEBUG(mdlnum, "δ���ҵ�ģ��%s�Ĳ�λ��Ϣ ԴAPP[%s]\n", &modtype[1], info_data.SourAddr);
    }
    else
    {
        MDLMGR_FMT_TRACE(mdlnum, "�ҵ�%d����λƥ��, ԴAPP[%s]\n",slotCnt, info_data.SourAddr);
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
* ��������: GetModuleInfo
* ��������: ��ѯģ��״̬
* �������: 
* �������: 
* �� �� ֵ: 
*******************************************************************************/
int GetModulePlugStatus(INFO_DATA_T info_data)
{
    int32           modeNo = 0;         //ģ�����
    INFO_DATA_T     infoDataTemp;           

    memcpy_r((uint8*)&modeNo, info_data.aPayloadData,4);
    
    if(modeNo < 1 || modeNo > 5)
    {
        MDLMGR_FMT_DEBUG(mdlnum, "ģ����� %d ���Ϸ�\n",modeNo);
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
    if(plug_status[modeNo-1].changeStatus == 1)         //�ϱ���ȡ����λ
    {
        plug_status[modeNo-1].changeStatus = 0;
    }
    PlcWriteQueue(&LockRecvBuf, (uint8*)&infoDataTemp, sizeof(INFO_DATA_T));
    return 1;
    
}

/*******************************************************************************
* ��������: GetModuleInfo
* ��������: ģ�鸴λ
* �������: 
* �������: 
* �� �� ֵ: 
*******************************************************************************/
int ResetModule(INFO_DATA_T info_data)      
{
    //������ʼ��
    uint32  modeNo = 0;
    TRANS_INFO_T    transInfo;
    
    memset(&transInfo,0x0,sizeof(TRANS_INFO_T));
    
    if(MDLWriteInfo(&InfoDataQueue, &info_data) < 0)
        return -1;
    //д�����Ͷ���
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
* ��������: ModulePowerOff
* ��������: ģ��ϵ���ϵ�
* �������: moduleNo����λ�š� OnorOff��0 �ϵ� 1 �ϵ硿
* �������: 
* �� �� ֵ: 0 �ɹ� ���� ʧ��
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
            MDLMGR_FMT_DEBUG(moduleNo - 1, "��λ[%d]�ϵ�ʧ��\n", moduleNo);
            return -1;
        }
    }
    else
    {
        if(dev_power->power_ply_vcc_out_set(dev_power, (uint8)moduleNo, POWER_OFF) < 0)
        {
            MDLMGR_FMT_DEBUG(moduleNo - 1, "��λ[%d]�ϵ�ʧ��\n", moduleNo);
            return -1;
        }
    }
    return 0;
}

/**********************************************************************
* @name      : mdl_ctrl_buzzer
* @brief     �����Ʒ�����
* @param[in] ��uint8 bOpen   �Ƿ��
* @param[out]��
* @return    ��
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
* @brief     �����Ʒ�����
* @param[in] ��delay   ��ʱ�ر�
* @param[out]��
* @return    ��
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
* ��������: GetModuleInfo
* ��������: ģ��ϵ�����
* �������: 
* �������: 
* �� �� ֵ: 
*******************************************************************************/
int RestartModule(INFO_DATA_T info_data)
{
    uint32          modeNo = 0;
    uint32          PowOffTime = 0;     //�ϵ�ʱ��
    TRANS_INFO_T    transInfo;
    INFO_DATA_T     infoDataTemp;
    uint8           result = 1;
    //uint8         i;
    
    memset(&transInfo,0x0,sizeof(TRANS_INFO_T));
    
//  if(MDLWriteInfo(&InfoDataQueue, &info_data) < 0)
//      return -1;
    //д�����Ͷ���
    //modCnt = info_data.aPayloadData[0];
    if(info_data.nPayloadLength != 8)
    {
        MDLMGR_FMT_DEBUG(mdlnum, "app���յ���ģ��������Ϣ���ݳ���=%d,�쳣\n",info_data.nPayloadLength);
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
        MDLMGR_FMT_DEBUG(mdlnum, "��λ[modeNo]��ʼ�ϵ磬�ϵ�ʱ��[%d]\n", modeNo, PowOffTime);
        if(ModulePowerOff(modeNo,0) < 0 )
        {
            MDLMGR_FMT_DEBUG(mdlnum, "ģ��[%d]����ʧ��!\n", modeNo);
            result = 0;             //ʧ��
        }
    }

    if(result == 0)
    {
        MDLMGR_FMT_DEBUG(mdlnum, "��λ[%d]�ϵ�ʧ��%02x\n", modeNo, result);
        memcpy(&infoDataTemp,&info_data,sizeof(INFO_DATA_T));
        infoDataTemp.MsgRPM = 0;
        infoDataTemp.nPayloadLength = 1;
        infoDataTemp.aPayloadData[0] = result;
        PlcWriteQueue(&LockRecvBuf, (uint8*)&infoDataTemp, sizeof(INFO_DATA_T));
    }
    else        //����ϵ������Ϣ���ϵ��ɹ����ٻ���
    {
        modPowerCtl[modeNo-1].offpowerflag = 1;
        modPowerCtl[modeNo-1].offpowertime = (time_t)PowOffTime;
        memcpy(&modPowerCtl[modeNo-1].infotemp,&info_data,sizeof(INFO_DATA_T));

        //�Ȼظ�ģ��������Ϣ
        memcpy(&infoDataTemp,&info_data,sizeof(INFO_DATA_T));
        infoDataTemp.MsgRPM = 0;
        infoDataTemp.nPayloadLength = 1;
        infoDataTemp.aPayloadData[0] = result;
        PlcWriteQueue(&LockRecvBuf, (uint8*)&infoDataTemp, sizeof(INFO_DATA_T));
    }

    return 1;
}

//APP�ļ���������
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
    MDLUP_FMT_DEBUG("FileSend���յ�ģ�����%d �������ļ�·��:%s, �ļ�������[%d], ׼������... \n",modeNo, fileName, nameLen);

    //���������߳�
    sprintf(fileSend,"%s_%d", MDL_UPDATE_FILE, modeNo - 1);
    sprintf(actBuffer,"cp %s %s", fileName, fileSend);
    MDLUP_FMT_DEBUG("���������ļ�Ϊ: %s\n", fileSend);
    if(system(actBuffer) < 0)
    {
        MDLUP_FMT_DEBUG("app·�������ļ�%sִ��ʧ��!\n",actBuffer);
        
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
* ��������: CreateTaskByInfo
* ��������: �����յ�����Ϣ��������
* �������: ��
* �������: ��
* �� �� ֵ: >0 �ɹ�
            <=0 ʧ��
*******************************************************************************/
int8 CreateTaskByInfo(void)
{
    int nGetRet = 0;
    
    INFO_DATA_T stInfoData;  //��Ϣ
    memset(&stInfoData, 0x00, sizeof(stInfoData));
    
    nGetRet = GetInfoDataFromList(&stInfoData);
    //printf("\nCreateTaskByInfo:nGetRet %d, stInfoData.IID_ID = %04x, stInfoData.IOP_ID = %04x\n",nGetRet,stInfoData.IID_ID,stInfoData.IOP_ID);
    if(nGetRet < 0)
    {
        return -1;
    }
    //ִ����Ϣ
    if(stInfoData.IID_ID != 0x0004 && stInfoData.IID_ID != 0x1155)
    {
        return -1;
    }
    
    if(stInfoData.IID_ID == 0x1155 && stInfoData.IOP_ID == 0x9090)
    {
        MDLMGR_FMT_DEBUG(mdlnum, "SrcAddr[%s]->DstAddr[%s]:��ѯ���̰汾��Ϣ\n", stInfoData.SourAddr, stInfoData.DestAddr);
        return GetVendorVerInfo(stInfoData);
    }
    else if(stInfoData.IID_ID == 0x0004)
    {
    switch (stInfoData.IOP_ID)
    {
        case 0x1010://��ѯģ����Ϣ(��Э��)
        case 0x0010://��ѯģ����Ϣ
        {
            MDLMGR_FMT_DEBUG(mdlnum, "SrcAddr[%s]->DstAddr[%s]:��ѯģ����Ϣ IOP[0x%04x]\n", stInfoData.SourAddr, 
                stInfoData.DestAddr, stInfoData.IOP_ID);
            return GetModuleInfo(stInfoData);
        }
        case 0x0011://��ѯģ���λ��Ϣ
        {
            MDLMGR_FMT_DEBUG(mdlnum, "SrcAddr[%s]->DstAddr[%s]:��ѯģ���λ��Ϣ IOP[0x%04x]\n", stInfoData.SourAddr, 
                stInfoData.DestAddr, stInfoData.IOP_ID);
            return GetModuleSlotInfo(stInfoData);
        }
        case 0x1012://��ѯģ����״̬(��Э��)
        case 0x0012://��ѯģ����״̬
        {
            MDLMGR_FMT_DEBUG(mdlnum, "SrcAddr[%s]->DstAddr[%s]:��ѯģ����״̬ IOP[0x%04x]\n", stInfoData.SourAddr, 
                stInfoData.DestAddr, stInfoData.IOP_ID);
            return GetModulePlugStatus(stInfoData);
        }
        case 0x0013://ģ�鸴λ
        {
            MDLMGR_FMT_DEBUG(mdlnum, "SrcAddr[%s]->DstAddr[%s]:ģ�鸴λ IOP[0x%04x]\n", stInfoData.SourAddr, 
                stInfoData.DestAddr, stInfoData.IOP_ID);
            return ResetModule(stInfoData);
        }
        case 0x0014://ģ������
        {
            MDLMGR_FMT_DEBUG(mdlnum, "SrcAddr[%s]->DstAddr[%s]:ģ������ IOP[0x%04x]\n", stInfoData.SourAddr, 
                stInfoData.DestAddr, stInfoData.IOP_ID);  
            return RestartModule(stInfoData);
        }
        case 0x0015://�ļ�����
        {
            MDLMGR_FMT_DEBUG(mdlnum, "SrcAddr[%s]->DstAddr[%s]:�ļ����� IOP[0x%04x]\n", stInfoData.SourAddr, 
                stInfoData.DestAddr, stInfoData.IOP_ID);
            return FileSend(stInfoData);
        }
        case 0x0016://��ѯ����ģ����Ϣ
        {
            MDLMGR_FMT_DEBUG(mdlnum, "SrcAddr[%s]->DstAddr[%s]:��ѯģ��������Ϣ IOP[0x%04x]\n", stInfoData.SourAddr, 
                stInfoData.DestAddr, stInfoData.IOP_ID);
            return GetModuleInfoAll(stInfoData);
        }
        default:
        {
            MDLMGR_FMT_DEBUG(mdlnum, "SrcAddr[%s]->DstAddr[%s]:IOP[0x%04x] IOP[0x%04x] ���ʹ���\n", stInfoData.SourAddr, 
                stInfoData.DestAddr, stInfoData.IID_ID, stInfoData.IOP_ID);
            return -1;
            break;
        }           
        
    }

    }

    return 1;
}

/*******************************************************************************
* ��������: MDLWriteInfo
* ��������: ���洦���е�mqtt֡��Ϣ
* ���������
* ���������
* �ڲ�����: �����в�������buf�е����ݷ�������У���������������������
* �� �� ֵ: >=   0                   �ɹ�
            <  0                ʧ��
*******************************************************************************/
int32 MDLWriteInfo(InfoDataQueue_T *pInfoDataQueue, INFO_DATA_T *pInfoData)
{
    int i;
    int ret = -1;
    //��������
    pthread_mutex_lock(&pInfoDataQueue->FrameLock);

    //�ж������Ƿ��пյ�λ��
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
    //�⻺����
    if(ret != 1)
    {
        MDLMGR_FMT_TRACE(mdlnum, "%s д��ʧ��!\n", __FUNCTION__);
    }
    pthread_mutex_unlock(&pInfoDataQueue->FrameLock);

    return ret;
}

/*******************************************************************************
* ��������: MDLReadInfo
* ��������: ƥ�䴦���е�mqtt��Ϣ
* ���������
* ���������
* �ڲ�����: �ж������Ƿ������ݣ�����Ӷ�����ȡ�����ݷ���buf�У�ÿִ��һ�Σ�ȡһ������
* �� �� ֵ: >= 0                 �ɹ�
            <  0              ʧ��
*******************************************************************************/
int32 MDLReadInfo(InfoDataQueue_T *pInfoDataQueue, uint8 frameIndex, INFO_DATA_T *pInfoData)
{
    int     i = -1;

    //��������
    pthread_mutex_lock(&pInfoDataQueue->FrameLock);

    //�������������ݣ���ȡ��buf�У��⿪��
    for(i = 0; i < MAX_INFO_QUEUE_CNT; i++)
    {
         if(*(pInfoDataQueue->flag + i) == 0)
            continue;
         
         if(frameIndex != (((pInfoDataQueue->InfoData+i)->MsgIndex)&0xff)   )   //ֻȡ�˵��ֽ�
            continue;
         
         memcpy(pInfoData,(pInfoDataQueue->InfoData+i),sizeof(INFO_DATA_T));
         memset(pInfoDataQueue->InfoData+i, 0x00,sizeof(InfoDataQueue_T));
         *(pInfoDataQueue->flag + i) = 0;                                   //�ñ�־λ��Ч
         MDLMGR_FMT_TRACE(mdlnum, "mqtt headinfo�����е�%d����\n",i);
         break;
    }

    if(i == MAX_INFO_QUEUE_CNT)
    {
        MDLMGR_FMT_TRACE(mdlnum, "mqtt headinfo ������,��ջ���\n");
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

//��ӡ������Ϣ��head+msg)
void PrintInfoDetail(MSG_INFO_T *stMsgInfoSend)
{
    uint32 IP;        //Ŀ�Ķ�ip
    uint16 port;
    
    uint8  MsgRPM;         /* ������־λ, 1-����, 0-������ */
    uint8  MsgPRIORITY;    /* ���ȼ� */
    uint16 MsgIndex;       /* ��Ϣ��� */
    uint16 MsgLabel;       /* ��Ϣ��ǩ */

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

    MDLMSG_FMT_TRACE("��ӡ��Ϣ����������Ϊ:\n");
    MDLMSG_FMT_TRACE("IP:%x, port:%d, MsgRPM:%d, MsgPRIORITY:%d, MsgIndex:%d, MsgLabel:%d, SourLen:%d, DestLen:%d, IID:%04x, IOP:%04x, MsgLen:%d. \n", 
        IP, port, MsgRPM, MsgPRIORITY,MsgIndex, MsgLabel, SourLen, DestLen, IID, IOP, MsgLen);
    MDLMSG_FMT_TRACE("SourAddr:%s\n", SourAddr);
    MDLMSG_FMT_TRACE("DestAddr:%s\n", DestAddr);
    MDLMSG_BUF_TRACE(MsgData, MsgLen, "MsgData:");
    MDLMSG_FMT_TRACE("��Ϣ���������ݴ�ӡ���. \n");
}

//����buffת��Ϊstring��ӡ����ӿո�
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
* ��������: PlcWriteQueue
* ��������: д����
* ���������plockqueue          ���������ṹָ��
            buf                 ��Ҫд����е�����
            buf_len             ���ݳ���
* �����������
* �ڲ�����: �����в�������buf�е����ݷ�������У���������������������
* �� �� ֵ: >0                  �ɹ�
            =0                  ʧ��
*******************************************************************************/
int32 PlcWriteQueue(LockQueue_T *pLockQueue, uint8 *buf, int32 buf_len)
{
    int32 flags = 0;

    //��������
    pthread_mutex_lock(&pLockQueue->QueueLock);
    
    //�ж������Ƿ��пյ�λ��
    if (pLockQueue->Queue[pLockQueue->QueueTail].QueueLen == 0)
    {   
        pLockQueue->Queue[pLockQueue->QueueTail].QueueLen = buf_len;
        memcpy(pLockQueue->Queue[pLockQueue->QueueTail].QueueBuf, buf, buf_len);
        flags = 1;
        pLockQueue->QueueTail = (pLockQueue->QueueTail + 1) % (MAX_QUEUE_CNT);
    }
    else
    {
        MDLMGR_FMT_TRACE(mdlnum, "PlcWriteQueue:������\n");
    }

    //�⻺����
    pthread_mutex_unlock(&pLockQueue->QueueLock);
    return flags;
}

/*******************************************************************************
* ��������: PlcWriteQueue
* ��������: ������
* ���������pLockQueue          ���������ṹָ��
*           buf                 �����ݻ��������������е�����ȡ������û�����
* ���������plockqueue          �����е�����
* �ڲ�����: �ж������Ƿ������ݣ�����Ӷ�����ȡ�����ݷ���buf�У�ÿִ��һ�Σ�ȡһ������
* �� �� ֵ: len                 ���������ݳ���
*******************************************************************************/
int32 PlcReadQueue(LockQueue_T *pLockQueue, uint8 *buf)
{
    int32 len = 0;
    uint8 *pdata_buf;

    //��������
    pthread_mutex_lock(&pLockQueue->QueueLock);

    //�������������ݣ���ȡ��buf�У��⿪��
    if ((pLockQueue->QueueHead != pLockQueue->QueueTail)
        || ((pLockQueue->QueueHead == pLockQueue->QueueTail)
            && (pLockQueue->Queue[pLockQueue->QueueHead].QueueLen != 0)))
    {
        pdata_buf = pLockQueue->Queue[pLockQueue->QueueHead].QueueBuf;
        len = pLockQueue->Queue[pLockQueue->QueueHead].QueueLen;

        //ȡ�����е�����
        memcpy(buf, pdata_buf, len);

        //������
        memset(pdata_buf, 0, len);
        pLockQueue->Queue[pLockQueue->QueueHead].QueueLen = 0;

        //����ͷ����ƶ�һλ
        pLockQueue->QueueHead= (pLockQueue->QueueHead + 1) % (MAX_QUEUE_CNT);
    }

    pthread_mutex_unlock(&pLockQueue->QueueLock);

    return len;
}

//����buffת��Ϊstring��ӡ��û�пո�
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
* ��������: GetInfoIndex
* ��������: ����������Ϣ��index
* �������: 
* �������: 
* �� �� ֵ: 
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

    //�������
    va_list ap;
    va_start(ap,fmt);


    vsnprintf(debug_msg,sizeof(debug_msg),fmt,ap);
    va_end(ap);

    //��¼��־ʱ��
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
* @brief     ����ʼ�����ڽӿ�
* @param[in] ��PULSE_RS232_PARA *pCommPara  ����ͨѶ����
* @param[out]��
* @return    ��0-�ɹ�/ʧ��
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
    
    // ���豸
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
* @brief     ��ȥ��ʼ�����ڽӿ�
* @param[in] ��PULSE_RS232_PARA *pCommPara  ����ͨѶ����
* @param[out]��
* @return    ��0-�ɹ�/ʧ��
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
* @brief     ���������ݽ��սӿ�
* @param[in] ��PULSE_RS232_PARA *pCommPara  ����ͨѶ����
               uint8* pBuffer               �������ݻ�����
               int nBufLen                  ���ͳ���
* @param[out]��
* @return    ��0-�ɹ�/������
* @Create    : ����
* @Date      ��2020-3-10
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
* @brief     ���������ݽ��սӿ�
* @param[in] ��PULSE_RS232_PARA *pCommPara  ����ͨѶ����
               int nBufLen                  �����ճ���
               int rsptime                  ���ݷ���ʱ��(20ms)
               int rcvtime                  �����ֽ�֮����ʱ��(20ms)
               uint8 endbyte                ��������û����0xff
* @param[out]��uint8* pBuffer               �������ݻ�����
* @return    �����յ����ݵĳ���
* @Create    : ����
* @Date      ��2020-3-10
* @Update    :
**********************************************************************/
int mdl_comm_recv(MDL_COMM_PARA *pCommPara, uint8* pBuffer, int nBufLen, int rsptime, int rcvtime, uint8 endbyte)
{
    int recv_len = 0;        /*���յ����ݳ���*/
    int data_len = 0;        /*���ڻ����������ݵĳ���*/
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
                return data_len; /*-�������ݳ�ʱ-*/
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

