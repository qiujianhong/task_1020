/*
*********************************************************************
* Copyright(C) 2019 �Ͼ��������ӹɷ����޹�˾
* All rights reserved. 
* @brief��   ͨ��ģ���̨ͨ��
* @date��    2022-07-29
* @history�� 
*********************************************************************
*/
#include "baseCommon.h"
#include "uart.h"
#include "ble.h"
#include "radio.h"
#include "comm.h"
#include "task_msg.h"
#include "task_id.h"
#include "msg_id.h"
#include "appmsg_split.h"
#include "sys_cfg.h"
#include "appcomm.h"
#include "oopVarUpdate.h"
#include "radio_comm.h"
#include "statlib.h"

/**********************************************************************
* @name      : comm_getcheck_radio
* @brief     ����ȡ����������Ƿ����仯
* @param[in] ��int sockdbfd      ͨ�ž��
               void *pCfg        ���ô洢��ַ
* @param[out]��void *pCfg        ���ô洢��ַ
               uint8 *pRestart   �߳��������
* @return    ��0-�ɹ�/������
* @Create    : ����
* @Date      ��2021-8-2
* @Update    :
**********************************************************************/
int comm_getcheck_radio(int sockdbfd, void *pCfg, uint8 *pRestart)
{
    /* Ŀǰ����Ҫ������ȡ */
    COMM_RADIO_HANDLE_T *ptHandle = (COMM_RADIO_HANDLE_T *)pCfg;
    int              ret = 0;
    uint32           readlen = 0;
    NOMAL_OAD_T      tOAD;
    OOP_OCTETVAR16_T tLocalAddr;
    
    memset(&tOAD, 0, sizeof(NOMAL_OAD_T));

    /* ��ȡ�ն˵�ַ */
    tOAD.oad.value = 0x40010200;
    memset(&tLocalAddr, 0, sizeof(OOP_OCTETVAR16_T));
    ret = db_read_nomal(sockdbfd, &tOAD, sizeof(OOP_OCTETVAR16_T), (uint8 *)&tLocalAddr, &readlen);
    if(0 == ret && readlen == sizeof(OOP_OCTETVAR16_T) && tLocalAddr.nNum > 0)
    {
        ptHandle->bGetCfg = TRUE;

        if (memcmp(&ptHandle->radioCfg.addr, &tLocalAddr, sizeof(tLocalAddr)) != 0)
        {
            memcpy(&ptHandle->radioCfg.addr, &tLocalAddr, sizeof(OOP_OCTETVAR16_T));
            /*ptHandle->radioCfg.areaNo.nNum = 2;
            ptHandle->radioCfg.areaNo.value[0] = tLocalAddr.value[0];
            ptHandle->radioCfg.areaNo.value[1] = tLocalAddr.value[1];
            ptHandle->radioCfg.devAddr.nNum = 3;
            ptHandle->radioCfg.devAddr.value[0] = ((0x0f << 0x04) + (tLocalAddr.value[3] & 0x0f));
            ptHandle->radioCfg.devAddr.value[1] = tLocalAddr.value[4];
            ptHandle->radioCfg.devAddr.value[2] = tLocalAddr.value[5];

            ptHandle.radioCfg.channel = 1;
            ptHandle.radioCfg.power = 5;
            ptHandle.radioCfg.noise = 5;
            ptHandle.radioCfg.mode = 34;
            ptHandle.radioCfg.offVaule = 0;*/
        
            ptHandle->btStatus = COMM_RADIO_STATUS_GETAPP;
            COMM_FMT_DEBUG("��ַ�仯�����õ�̨\n");
        }
    }
    else
    {
        ptHandle->bGetCfg = FALSE;
    }
    
    return 0;
}

/**********************************************************************
* @name      : comm_timer_radio_status
* @brief     ��״̬���л�
* @param[in] ��int socketfd    task��Ϣ���
               void *pArg      ���
* @param[out]��
* @return    ��
* @Create    : 
* @Date      ��2021-8-2
* @Update    :
**********************************************************************/
void comm_timer_radio_status(int socketfd, void *pArg)
{
    int                 ret = 0;
    COMM_RADIO_HANDLE_T *ptHandle = (COMM_RADIO_HANDLE_T *)pArg;

    if(socketfd <= 0 || NULL == pArg || FALSE == ptHandle->bGetCfg || 0 == *ptHandle->pIPCok)
    {
        return;
    }
    
    /* ������ز��� */
    switch(ptHandle->btStatus)
    {
        case COMM_RADIO_STATUS_GETAPP:
        {
            /* ��ѯAPP */
            ret = comm_send_label_msg(MSG_COMM_GETRADIOCFG, 1, NULL, 0);
            if(0 != ret)
            {
                COMM_FMT_DEBUG("Comm get radio app send failed %d\n", ret);
            }
            /* ��ն��� */
            comm_clean_queue(ptHandle->ptRecvList);
            comm_clean_queue(ptHandle->ptSendList);
            break;
        }
        case COMM_RADIO_STATUS_SETCFG:
        {
            uint8  msgbuf[128] = {0};
            uint16 msglen = 0;

            msglen = radio_config_package(&(ptHandle->radioCfg), msgbuf, 128);
            if(0 == msglen)
            {
                COMM_FMT_DEBUG("Comm package radio cfg msg failed\n");
                break;
            }
            /* �����²��� */
            ret = comm_send_label_msg(MSG_COMM_SETRADIOCFG, 1, msgbuf, msglen);
            if(0 != ret)
            {
                COMM_FMT_DEBUG("Comm set bt cfg send failed %d\n", ret);
            }
            break;
        }
        case COMM_RADIO_STATUS_PREPARE:
        {
            /* ֪ͨ�˿ڴ� */
            uint8 msgBuf[512] = {0};
            uint16 msgLen = 0;
            COMM_IPCMSG_T *ptIPCMsg = NULL;
            CHANNEL_INFO_T *ptChanInfo = NULL;
            
            msgLen = sizeof(COMM_IPCMSG_T) + sizeof(CHANNEL_INFO_T);
            ptIPCMsg = (COMM_IPCMSG_T *)msgBuf;
            ptChanInfo = (CHANNEL_INFO_T *)ptIPCMsg->msg;
            ptChanInfo->workmode = 2;
            ptChanInfo->medium = COMM_MEDIA_RADIO;
            ptChanInfo->connType = 0;
            ptChanInfo->bFirst = TRUE;
            ptChanInfo->heart = 0;
            ptIPCMsg->id = CHL_RADIO;
            ptIPCMsg->protocol = PROT_OPEN_CHANNLE;
            ptChanInfo->dailcnt = 3;
            ptChanInfo->dailtime = 60;
            ptChanInfo->ip = 0;
            ptChanInfo->port = 0;

            comm_write_queue(ptHandle->ptRecvList, msgBuf, msgLen);
            COMM_FMT_DEBUG("��̨ͨ��%d��\n", ptIPCMsg->id);
                
            ptHandle->btStatus = COMM_RADIO_STATUS_WORK;
            break;
        }
        case COMM_RADIO_STATUS_WORK:
        {
            break;
        }
        default:
        {
            ptHandle->btStatus = COMM_RADIO_STATUS_GETAPP;
            break;
        }
    }
}

/**********************************************************************
* @name      : comm_radio_getcfgmsg
* @brief     ��������ȡ
* @param[in] ��void *playload    ��Ϣ����
               uint16 msglen     ��Ϣ����
               void *pArg        ˽������
* @param[out]��
* @return    ��0-�ɹ�/������
* @Create    : 
* @Date      ��2021-8-2
* @Update    : 
**********************************************************************/
int comm_radio_getcfgmsg(uint8 *playload, uint16 msglen, uint16 label, void *pArg)
{
    COMM_RADIO_HANDLE_T *ptHandle = (COMM_RADIO_HANDLE_T *)pArg;
    RADIO_CONFIG_T tConfig;
    int ret = 0;

    if(NULL == playload || 0 == msglen || NULL == pArg)
    {
        COMM_FMT_DEBUG("comm_bt_getcfgmsg err input\n");
        return -1;
    }
    
    memset(&tConfig, 0, sizeof(RADIO_CONFIG_T));
    ret = radio_config_split(playload, msglen, &tConfig);
    if(0 != ret)
    {
        COMM_BUF_DEBUG(playload, msglen, "Comm get bt cfg error(%d)\n", ret);
        return ret;
    }
    
    //memcpy(&(ptHandle->radioCfg), &tConfig, sizeof(RADIO_CONFIG_T));
    //����ַ��Ĳ�����ʹ��Ĭ�ϲ���
    ptHandle->radioCfg.channel = tConfig.channel;
    ptHandle->radioCfg.power = tConfig.power;
    ptHandle->radioCfg.noise = tConfig.noise;
    #if 0
    ptHandle->radioCfg.mode = tConfig.mode;
    ptHandle->radioCfg.offVaule = tConfig.offVaule;
    #endif
    memcpy(&ptHandle->radioCfg.areaNo, &tConfig.areaNo, sizeof(tConfig.areaNo));
    memcpy(&ptHandle->radioCfg.devAddr, &tConfig.devAddr, sizeof(tConfig.devAddr));

    //����Ҫ���õ�ַ�����ˣ�����ʾ��������
    //memcpy(&ptHandle->radioCfg.addr, &tConfig.addr, sizeof(tConfig.addr));
    
    if (COMM_RADIO_STATUS_GETAPP != ptHandle->btStatus)
    {
        return 0;
    }

    COMM_FMT_DEBUG("��ȡ����̨��Ϣ:\n");
    COMM_BUF_DEBUG(tConfig.areaNo.value, tConfig.areaNo.nNum, "��������");
    COMM_BUF_DEBUG(tConfig.devAddr.value, tConfig.devAddr.nNum, "�豸��ַ");
    COMM_BUF_DEBUG(tConfig.addr.value, tConfig.addr.nNum, "�ն˵�ַ");
    #if 1
    COMM_FMT_DEBUG("Ƶ��[%d] ����%dms ���뼶��[%d]\n", tConfig.channel, tConfig.power, tConfig.noise);
    #endif

    if (memcmp(&ptHandle->radioCfg.addr, &tConfig.addr, sizeof(tConfig.addr)) == 0)
    {
        /* ��ַͬ�ն�, ֱ�Ӵ�ͨ�� */
        ptHandle->btStatus = COMM_RADIO_STATUS_PREPARE;
        COMM_BUF_DEBUG(ptHandle->radioCfg.addr.value, ptHandle->radioCfg.addr.nNum, "��ַͬ�ն�, ֱ�Ӵ�ͨ��:");
    }
    else
    {
        ptHandle->btStatus = COMM_RADIO_STATUS_SETCFG;
        COMM_BUF_DEBUG(ptHandle->radioCfg.addr.value, ptHandle->radioCfg.addr.nNum, "��ַ��ͬ�ն�, �����ò���:");
    }
    return 0;
}

/**********************************************************************
* @name      : comm_radio_setcfgmsg
* @brief     ����������
* @param[in] ��void *playload    ��Ϣ����
               uint16 msglen     ��Ϣ����
               void *pArg        ˽������
* @param[out]��
* @return    ��0-�ɹ�/������
* @Create    : 
* @Date      ��2021-8-2
* @Update    : 
**********************************************************************/
int comm_radio_setcfgmsg(uint8 *playload, uint16 msglen, uint16 label, void *pArg)
{
    COMM_RADIO_HANDLE_T *ptHandle = (COMM_RADIO_HANDLE_T *)pArg;
    RADIO_CONFIG_T *ptConfig = &(ptHandle->radioCfg);

    if(NULL == playload || 0 == msglen || NULL == pArg)
    {
        COMM_FMT_DEBUG("comm_radio_setcfgmsg err input\n");
        return -1;
    }

    COMM_FMT_DEBUG("���õ���̨��ַ����:\n");
    COMM_BUF_DEBUG(ptConfig->areaNo.value, ptConfig->areaNo.nNum, "��������");
    COMM_BUF_DEBUG(ptConfig->devAddr.value, ptConfig->devAddr.nNum, "�豸��ַ");
    COMM_BUF_DEBUG(ptConfig->addr.value, ptConfig->addr.nNum, "�ն˵�ַ");
    #if 0
    COMM_FMT_DEBUG("����%dms ���뼶��[%d], ������ʽ[%d], Ƶ��ƫֵ[%d]\n", 
                   ptConfig->power, ptConfig->noise, ptConfig->mode, ptConfig->offVaule);
    #endif

    if(COMM_RADIO_STATUS_SETCFG == ptHandle->btStatus)
    {
        ptHandle->btStatus = COMM_RADIO_STATUS_PREPARE;
    }
    return 0;  
}

/**********************************************************************
* @name      : comm_radio_recvmsg
* @brief     ��������Ϣ
* @param[in] ��void *playload    ��Ϣ����
               uint16 msglen     ��Ϣ����
               void *pArg        ˽������
* @param[out]��
* @return    ��0-�ɹ�/������
* @Create    : 
* @Date      ��2021-8-2
* @Update    : 
**********************************************************************/
int comm_radio_recvmsg(uint8 *playload, uint16 msglen, uint16 label, void *pArg)
{
    COMM_RADIO_HANDLE_T *ptHandle = (COMM_RADIO_HANDLE_T *)pArg;
    uint8  dataBuf[2048 + 8] = {0};
    uint16 dataLen = 0;
    int    ret = 0;
    RADIO_MSG_EVENT_T tMsg;
    if(NULL == playload || NULL == pArg)
    {
        COMM_FMT_DEBUG("comm_radio_recvmsg input null\n");
        return -1;
    }

    ret = radio_msg_event_split(playload, msglen, &tMsg);
    if(0 != ret)
    {
        COMM_BUF_DEBUG(playload, msglen, "Comm recv bt msg error(%d)\n", ret);
        return ret;
    }
    
    if(COMM_RADIO_STATUS_WORK != ptHandle->btStatus)
    {
        return 0;
    }
    
    memcpy(dataBuf, tMsg.msg, MIN(tMsg.len, 2048));
    dataLen = MIN(tMsg.len, 2048);

    comm_add_channel(CHL_RADIO, dataBuf, dataLen);

    if(0 != comm_write_queue(ptHandle->ptRecvList, dataBuf, dataLen + 4))
    {
        COMM_FMT_TRACE("Radio queue is full\n");
    }
    COMM_BUF_DEBUG(dataBuf + 4, dataLen, "��̨����");
    
    return 0;
}

/**********************************************************************
* @name      : comm_radio_start
* @brief     �������ж�
* @param[in] ��void *pArg
* @param[out]��
* @return    ��
* @Create    : ����
* @Date      ��2019-11-2
* @Update    :
**********************************************************************/
BOOLEAN comm_radio_start(void *pArg)
{
    COMM_RADIO_HANDLE_T *ptHandle = (COMM_RADIO_HANDLE_T *)pArg;

    if(0 == *ptHandle->pIPCok || COMM_RADIO_STATUS_WORK != ptHandle->btStatus)
    {
        return FALSE;
    }
    return TRUE;
}

/**********************************************************************
* @name      : comm_radio_entry
* @brief     ���߳����
* @param[in] ��void *pArg
* @param[out]��
* @return    ��
* @Create    : ����
* @Date      ��2019-11-2
* @Update    :
**********************************************************************/
void *comm_radio_entry(void *pArg)
{
    int        ret = 0;
    uint8      stateLocal = CHANNEL_STATE_INIT;  /* ����״̬ */
    RADIO_MSG_T  tMsgOut;
    uint8      msgBuf[SERIAL_BUF_SIZE + 128] = {0}; 
    uint32     msgLen = 0;
    uint8      sendBuf[SERIAL_BUF_SIZE + 128] = {0};
    uint32     sendLen = 0;
    uint8      recv[8] = {0};
    uint16     recvlen = 0;
    COMM_IPCMSG_T   *ptIPCMsg = NULL;
    COMM_RADIO_HANDLE_T *ptHandle = (COMM_RADIO_HANDLE_T *)pArg;
    
    if(NULL != ptHandle->pAppReStart)
    {
        *ptHandle->pAppReStart = 0;
    }

    COMM_FMT_DEBUG("��̨ͨ�Ŵ�\n");

    while(1)
    {
        *ptHandle->pRunFlag = 0;

        /* Appmain���� */
        if((NULL != ptHandle->pAppReStart && 1 == *ptHandle->pAppReStart) || COMM_RADIO_STATUS_WORK != ptHandle->btStatus)
        {
            /* ֪ͨ��Ӧ�õ����߳�ͨ���� ����Ĭ�Ͽ��� */
            msgLen = sizeof(COMM_IPCMSG_T);
            ptIPCMsg = (COMM_IPCMSG_T *)msgBuf;
            ptIPCMsg->id = CHL_RADIO;
            ptIPCMsg->protocol = PROT_CLOSE_CHANNLE;
            comm_write_queue(ptHandle->ptRecvList, msgBuf, msgLen);
            COMM_FMT_DEBUG("��̨ͨ��%d�ر�\n", ptIPCMsg->id);

            *ptHandle->pRunFlag = COMM_TRHEAD_FLAG_MUX;
            return 0;
        }
        
        switch(stateLocal)
        {
            case CHANNEL_STATE_INIT:
            {
                /* ֪ͨ��Ӧ�õ����߳�ͨ���� ����Ĭ�Ͽ��� */
                stateLocal = CHANNEL_STATE_SEND;
                break;
            }
            case CHANNEL_STATE_SEND:
            {
                sendLen = comm_read_queue(ptHandle->ptSendList, sendBuf);
                if (sendLen > 4)
                {
                    /* ǰ4���ֽ�ȥ�� */
                    memcpy(tMsgOut.msg, sendBuf + 4, sendLen - 4);
                    tMsgOut.len = sendLen - 4;
                    msgLen = radio_msg_package(&tMsgOut, msgBuf, SERIAL_BUF_SIZE + 128);
                    ret = comm_send_syncmsg(MSG_COMM_RADIOSENDMSG, msgBuf, msgLen, recv, 8, &recvlen);
                    if(0 != ret || recvlen != 1 || 0 == recv[0])
                    {
                        ret = -1;
                    }
                    COMM_BUF_DEBUG(tMsgOut.msg, tMsgOut.len, "��̨����%s",     0 == ret ? "" : "ʧ��");

                    memset(tMsgOut.msg, 0 , tMsgOut.len);
                    memset(msgBuf, 0 , msgLen);
                    memset(sendBuf, 0 , sendLen);
                }
                stateLocal = CHANNEL_STATE_SEND;
                break;
            }
            case CHANNEL_STATE_CLOSE:
            {
                stateLocal = CHANNEL_STATE_INIT;
                break;
            }
            default:
            {
                stateLocal = CHANNEL_STATE_INIT;
                break;
            }
        }
        usleep(10000);
    }
}

