/*
*********************************************************************
* Copyright(C) 2019 �Ͼ��������ӹɷ����޹�˾
* All rights reserved. 
* @brief��   ��Ӧ�õ��Ƚ���APDU����
* @date��    2019-12-5
* @history�� 
*********************************************************************
*/
#include "baseCommon.h"
#include "EventIO.h"
#include "appcomm.h"
#include "apducomm.h"
#include "oopservice.h"
#include "commClass.h"
#include "OopAll.h"
#include "AppCommon.h"
#include "framecomm.h"
#include "msg_id.h"
#include "appreport.h"
#include "area.h"
#if defined (AREA_JIANGSU) && defined (PRODUCT_ZCU_1)
#include "lcmtrans.h"
extern LC_UART_VDEV_T g_lcmdev;
#endif


APPLY_PROXY_ACK g_proxyApplyAck[MAX_CHANNEL_NUM] = {{FALSE, TRUE, 10}};

/**********************************************************************
* @name      : proxy_apply_ack_set
* @brief     ������ͨ�Ŵ����Ӧ������״̬֪ͨȷ��
* @param[in] ��ack   ȷ����Ϣ
* @return    ��������Ϣ����
* @Create    : ����
* @Date      ��2020-04-20
* @Update    :
**********************************************************************/
void proxy_apply_ack_set(uint8 id)
{
    int i = 0;

    //������Ϣ���ֲ���ͨ������ʱ�ĸ�ͨ�����ˣ��͸��ĸ�ͨ��ȷ��
    for(i = 0; i < MAX_CHANNEL_NUM; i++)
    {
        if(g_proxyApplyAck[i].bSend == TRUE)
        {
            g_proxyApplyAck[i].bSend = FALSE;
            g_proxyApplyAck[i].bAck = TRUE;
            g_proxyApplyAck[i].retry = 0;
        }
    }
}

/**********************************************************************
* @name      : proxy_apply_ack_set
* @brief     ��ͨ�Ŵ����Ӧ������״̬֪ͨȷ��
* @param[out]��ack   ȷ����Ϣ
* @return    ��������Ϣ����
* @Create    : ����
* @Date      ��2020-04-20
* @Update    :
**********************************************************************/
BOOLEAN proxy_apply_ack_get(uint8 id)
{
    return g_proxyApplyAck[id].bAck;
}

/**********************************************************************
* @name      : proxy_apply_ack_set
* @brief     ��ͨ�Ŵ����Ӧ������״̬֪ͨȷ��
* @param[out]��ack   ȷ����Ϣ
* @return    ��������Ϣ����
* @Create    : ����
* @Date      ��2020-04-20
* @Update    :
**********************************************************************/
void proxy_apply_retry_sub(uint8 id)
{
    if(g_proxyApplyAck[id].retry > 0)
    {
        g_proxyApplyAck[id].retry--;
    }

    if( g_proxyApplyAck[id].retry == 0)
    {
        g_proxyApplyAck[id].bSend = FALSE;
        g_proxyApplyAck[id].bAck = TRUE;        
    }
}

/**********************************************************************
* @name      : proxy_apply_ack_set
* @brief     ��ͨ�Ŵ����Ӧ������״̬֪ͨȷ��
* @param[out]��ack   ȷ����Ϣ
* @return    ��������Ϣ����
* @Create    : ����
* @Date      ��2020-04-20
* @Update    :
**********************************************************************/
uint8 proxy_apply_retry_get(uint8 id)
{
    return g_proxyApplyAck[id].retry;
}

/**********************************************************************
* @name      : proxy_connectInfo_notify
* @brief     ��ͨ�Ŵ����Ӧ������״̬֪ͨ
* @param[in] ��ack   ȷ����Ϣ
* @return    ��������Ϣ����
* @Create    : ����
* @Date      ��2020-04-20
* @Update    :
**********************************************************************/
void proxy_coninfo_notify(uint8 id)
{
    int ret = ERR_OK;
    uint8 msgdata[1400] = {0};
    uint16 msglen=0;
    APP_CONTEXT_t applyInfo;
    BOOLEAN connStatus = app_get_connect(id, &applyInfo);
    uint8 *pAddr = NULL;
    uint8 addrLen = 0;
        
    msgdata[msglen++] = connStatus;

    memcpy_r(&msgdata[msglen], &applyInfo.sendFrameLen, sizeof(uint16));
    msglen += sizeof(uint16);

    memcpy_r(&msgdata[msglen], &applyInfo.recvFrameLen, sizeof(uint16));
    msglen += sizeof(uint16);

    addrLen = app_get_localaddr(&pAddr);
    
    msgdata[msglen++] = addrLen;
    
    memcpy(&msgdata[msglen], pAddr, addrLen);
    msglen += addrLen;
       
    ret = appmain_send_msg(MSG_APPAMIN_APPLYSTATUS, 0, msgdata, msglen);
    if(ret != ERR_OK)
    {
        STAT_FMT_DEBUG("appmain_send_msg failed. ret(%d)\n",ret);
        return;
    }
}

/**********************************************************************
* @name      : analyse_prelinkrequest
* @brief     ��698��ԼԤ�����������
* @param[in] ��CHANNEL_CACHE_T *ptChannel   ͨ�����
               FRAME_HEAD_T *ptFrame        ����֡ͷ
               uint16 msgmaxlen             ������Ϣ��󳤶�
* @param[out]��uint8 *pOutMsg               ������Ϣ
               uint8 *pProto                ˽��Э��
* @return    ��������Ϣ����
* @Create    : ����
* @Date      ��2019-12-6
* @Update    :
**********************************************************************/
uint16 analyse_prelinkrequest(CHANNEL_CACHE_T *ptChannel, FRAME_HEAD_T *ptFrame, uint8 *pOutMsg, uint16 msgmaxlen,
                              uint8 *pProto)
{
    FRAME_HEAD_T tFrame;
    LinkRequest_t *pTRecvApdu = NULL;
    uint16         Heartbeat = 0;
    LinkResponse_t apdu;
    struct tm ptm;
    time_t now_t;
    uint8 *pAddr = NULL;
    uint8  len = 0;

    pTRecvApdu = (LinkRequest_t *)ptFrame->pApdu;
    Heartbeat = pTRecvApdu->beatPeriod[0] * 256 + pTRecvApdu->beatPeriod[1];
    switch(pTRecvApdu->type + 1)
    {
        case LINK_LOGIN:  //��¼����
        {
            /* ������С��Ӧ������Ȩ�� */
            app_connect_init(ptChannel->id);
                    
            /* �������Ȩ��Ӧ������ */
            if(MODE_MIX == ptChannel->tChannel.workmode || MODE_SERVER == ptChannel->tChannel.workmode)
            {
                ptChannel->linkState = LINK_LOGIN;
                ptChannel->tChannel.heart = Heartbeat;
                ptChannel->leftHB = Heartbeat;
            }
            *pProto = PRO_DATA_TRANSMIT;
            /* ��ͨ��ģ�鷢����վ��¼ȷ�� */
            send_emergency_ipcmsg(ptChannel->id, PROT_OPEN_ACK, &ptChannel->id, sizeof(uint8));
            break;
        }
        case LINK_HEART:  //��������
        {
            if(MODE_SERVER == ptChannel->tChannel.workmode)
            {
                ptChannel->linkState = LINK_HEART;
                ptChannel->tChannel.heart = Heartbeat;
                ptChannel->leftHB = Heartbeat;
                /* �յ���Ϣ�����������ʧ���� */
                ptChannel->lostHBCount = 0;
            }
            *pProto = PRO_DATA_TRANSMIT;
            break;
        }
        case LINK_QUIT:   //�˳���¼
        {
            *pProto = PRO_EXIT_LOGIN_FRAME;
            break;
        }
        default:
            return 0;
    }

    /* ��֯���ر��� */
    apdu.id = SERVERID_LINKRESPONSE;
    apdu.piid = pTRecvApdu->piid;//PIID
    apdu.result = 0x80;           //result ʱ�ӿ��ţ��ɹ�
    memcpy(&apdu.reqDateTime, &(pTRecvApdu->dateTime), sizeof(Date_Time_t));//����ʱ��

    now_t = time(NULL);
    LOCALTIME_R(&now_t, &ptm);
    apdu.recvDateTime.year_h = ((ptm.tm_year+1900)&0xFF00)>>8;
    apdu.recvDateTime.year_l = (uint8)(ptm.tm_year+1900);
    apdu.recvDateTime.month = (ptm.tm_mon+1);
    apdu.recvDateTime.day_of_month = (ptm.tm_mday);
    apdu.recvDateTime.day_of_week = (ptm.tm_wday);
    apdu.recvDateTime.hour = (ptm.tm_hour);
    apdu.recvDateTime.min = (ptm.tm_min);
    apdu.recvDateTime.sec = (ptm.tm_sec);
    apdu.recvDateTime.milliseconds_h = 0;
    apdu.recvDateTime.milliseconds_l = 0;

    apdu.respDateTime = apdu.recvDateTime;
    
    memcpy(&tFrame, ptFrame, sizeof(FRAME_HEAD_T));
    tFrame.dir = 1;
    tFrame.apduLen = sizeof(LinkResponse_t);
    tFrame.pApdu = (uint8 *)&apdu;
    len = app_get_localaddr(&pAddr);
    if(0 != len)
    {
        tFrame.sAddr.len = len;
        memcpy(tFrame.sAddr.addr, pAddr, len);
        tFrame.sAddr.type = 0;
    }
    tFrame.headlen = APUD_COUNT_FRAME_LEN(tFrame.sAddr.len, tFrame.sAddr.logicAddr);
    tFrame.msglen = tFrame.headlen + tFrame.apduLen + APP_698_CS_REGIN_SIZE;

    return app_frame_to_package(&tFrame, pOutMsg, msgmaxlen, TRUE);
}

/**********************************************************************
* @name      : analyse_prelinkresponse
* @brief     ��698��ԼԤ������Ӧ����
* @param[in] ��CHANNEL_CACHE_T *ptChannel   ͨ�����
               FRAME_HEAD_T *ptFrame        ����֡ͷ
               uint16 msgmaxlen             ������Ϣ��󳤶�
* @param[out]��uint8 *pOutMsg               ������Ϣ
               uint8 *pProto                ˽��Э��
* @return    ��������Ϣ����
* @Create    : ����
* @Date      ��2019-12-6
* @Update    :
**********************************************************************/
uint16 analyse_prelinkresponse(CHANNEL_CACHE_T *ptChannel, FRAME_HEAD_T *ptFrame, uint8 *pOutMsg, uint16 msgmaxlen, 
                             uint8 *pProto)
{
    uint16 msglen = 0;
    //LinkResponse_t *pTApdu = NULL;

    /* �յ���Ϣ�����������ʧ���� */
    ptChannel->lostHBCount = 0;

    /* ������ģʽ��ִ�� */
    if(MODE_SERVER == ptChannel->tChannel.workmode)
    {
        return 0;
    }
    else if(MODE_MIX == ptChannel->tChannel.workmode)
    {
        /* ������С��Ӧ������Ȩ�� */
        app_connect_init(ptChannel->id);
        
        ptChannel->leftHB = 0;
        ptChannel->linkState = LINK_NO;
        /* ��ͨ��ģ�鷢����վ��¼ȷ�� */
        send_emergency_ipcmsg(ptChannel->id, PROT_OPEN_ACK, &ptChannel->id, sizeof(uint8));
    }
    else if(MODE_CLIENT == ptChannel->tChannel.workmode)
    {
        if(LINK_LOGIN == ptChannel->linkState )
        {
            ptChannel->linkState = LINK_HEART;
            ptChannel->leftHB = CHANNEL_GET_HEART(ptChannel);
            ptChannel->lostHBCount = 0;
            ptChannel->lostHBtime = 0;

            /* ������С��Ӧ������Ȩ�� */
            app_connect_init(ptChannel->id);
            /* ��ͨ��ģ�鷢����վ��¼ȷ�� */
            send_emergency_ipcmsg(ptChannel->id, PROT_OPEN_ACK, &ptChannel->id, sizeof(uint8));

#ifdef AREA_SHAANXI			//����������ICCID����¼��ж�
			CheckERC_3199(ptChannel);
#endif
        }
        if(LINK_HEART == ptChannel->linkState)
        {
            ptChannel->leftHB = CHANNEL_GET_HEART(ptChannel);
            ptChannel->lostHBCount = 0;
            ptChannel->lostHBtime = 0;
            if(rpt_taskstate(-1,NULL) ==MASTER_TO_TMN_WAIT ||rpt_taskstate(-1,NULL) ==MASTER_TO_TMN_WAIT_SEC)
            {
                rpt_ansertime_deal(ptFrame->pApdu,ptFrame->apduLen);
            }
        }
        if(LINK_CHANGE == ptChannel->linkState)
        {
            /* �л�����¼ ��ֹ�ն�����ʧ�����µ�¼ */
            ptChannel->leftHB = 60;
            ptChannel->linkState = LINK_LOGIN;
        }

    }

    /* ����ʱ��� Todo ʲô����? */

    return msglen;
}

/**********************************************************************
* @name      : auth_symmetry_security
* @brief     ���Գƽ���
* @param[in] ��uint8 *cipher      ����
               uint8 cipherLen    ���ĳ���
               uint8 *sign        ǩ��
               uint16 signLen     ǩ������
* @param[out]��uint8 *pOutSession �ػ�
               uint8 * pSign      ����ǩ��
* @return    ����֤���
* @Create    : ����
* @Date      ��2019-12-16
* @Update    :
**********************************************************************/
uint8 auth_symmetry_security(uint8 *cipher, uint16 cipherLen ,uint8 *sign, uint16 signLen,
                             uint8 *pOutSession, uint8 * pSign)
{
	uint8 tmbuff[1000];
	uint16 outLen;

	if(32 != cipherLen && 64 != signLen)
	{
		return CONNECT_ERR_SYMMETRY;
    }
    
    if(ESAM_FAIL == NegotiateSession_Esam((uint8*)cipher,(uint8*)sign,tmbuff,&outLen))
    {
        return CONNECT_ERR_SYMMETRY;
    }
    memcpy(pOutSession, tmbuff, 48);
    memcpy(pSign, tmbuff + 64, 64);
	return 0;
}

/**********************************************************************
* @name      : auth_signature_security
* @brief     ������ǩ������
* @param[in] ��uint8 *cipher      ����
               uint8 cipherLen    ���ĳ���
               uint8 *sign        ǩ��
               uint16 signLen     ǩ������
* @param[out]��uint8 *pOutSession �ػ�
               uint8 * pSign      ����ǩ��
* @return    ����֤���
* @Create    : ����
* @Date      ��2019-12-16
* @Update    :
**********************************************************************/
uint8 auth_signature_security(uint8 *cipher, uint16 cipherLen ,uint8 *sign, uint16 signLen,
                             uint8 *pOutSession, uint8 * pSign)
{
	uint8 tmbuff[1000];
	uint16 outLen;
    
	if(32 != cipherLen && 64 != signLen)
	{
		return CONNECT_ERR_SIGNATURE;
    }

    if(ESAM_FAIL == NegotiateSession_Esam((uint8*)cipher,(uint8*)sign,tmbuff,&outLen))
    {
        return CONNECT_ERR_SIGNATURE;
	}
    memcpy(pOutSession, tmbuff, 48);
	memcpy(pSign, tmbuff + 48, 64);
	return 0;//��֤�ɹ�
}

/**********************************************************************
* @name      : analyse_set_clientInfo
* @brief     �����ÿͻ����·���Ӧ�û�����������Э��
* @param[in] ��OOP_ASSOCIATION_CONTEXT_T *ptContext ����������
               ConnectRequest_t *pRequest           Ӧ������������Ϣ
* @param[out]��APP_CONTEXT_t *ptConnect             Ӧ����������
* @return    ��
* @Create    : ����
* @Date      ��2019-12-16
* @Update    :
**********************************************************************/
void analyse_set_clientInfo(OOP_ASSOCIATION_CONTEXT_T *ptContext, ConnectRequest_t *pRequest, APP_CONTEXT_t *ptConnect)
{
	uint8 i;

    ptConnect->prtlVersion = ptContext->VER;
    ptConnect->recvFrameLen = ptContext->nRecvSize;
    ptConnect->sendFrameLen = MIN(pRequest->clientRFML, ptContext->nSendSize);
    ptConnect->recvApduLen  = MIN(pRequest->clientPFML, ptContext->nMaxPDUSize);
    ptConnect->recvWindowSize = 1;

    ptConnect->ApplSplitLen = MAX(ptConnect->sendFrameLen, app_apdu_default_len());

	for(i = 0;i < 8; i++)
	{
        ptConnect->prtlConformance[i] = ptContext->PCB[i] | pRequest->prtlConf[i];
	}
	for(i = 0;i < 16; i++)
	{
		ptConnect->funcConformance[i] = ptContext->FCB[i] | pRequest->funcConf[i];
	}
    
	ptConnect->securityMode = *(pRequest->mechains);
    ptConnect->timeOut = pRequest->timeOut;
    
    ptConnect->estalishTime = time(NULL);
}

/**********************************************************************
* @name      : analyse_prelinkrequest
* @brief     ��698��Լ����Ӧ�������������
* @param[in] ��CHANNEL_CACHE_T *ptChannel   ͨ�����
               FRAME_HEAD_T *ptFrame        ����֡ͷ
               uint16 msgmaxlen             ������Ϣ��󳤶�
* @param[out]��uint8 *pOutMsg               ������Ϣ
* @return    ��������Ϣ����
* @Create    : ����
* @Date      ��2019-12-16
* @Update    :
**********************************************************************/
uint16 analyse_connectrequest(CHANNEL_CACHE_T *ptChannel, FRAME_HEAD_T *ptFrame, uint8 *pOutMsg, uint16 msgmaxlen)
{
    uint8 rslt = CONNECT_PERMISSION;
    ConnectRequest_t *pRequest = NULL;
    uint8 SecurityLevel = 0;
    OOP_ASSOCIATION_CONTEXT_T *ptContext = NULL;
    uint16 cipherLen = 0;
    uint16 signLen = 0;
    uint8 *cipher = NULL;
    uint8 *sign = NULL;
    uint8 offset = 0;
    uint8 *temp = NULL;
    uint16 sessionLen = 0;
    uint8 uSessionData[48] = {0};
    uint8 uSignInfo[64] = {0};
    APP_CONTEXT_t tConnect;
    uint8 timeTag = 0;
    uint8 timeTagBuff[10] = {0};
    
    FRAME_HEAD_T tFrame;
    
    Factory_Version_t *ptVersion = NULL;
    uint16             apduLen = 0;
    uint16             msglen = 0;
    ConnectResponse_t *pResponse = NULL;
    uint8             *pAddr = NULL;
    uint8              len = 0;

    pRequest = (ConnectRequest_t *)ptFrame->pApdu;
    /* 0 �ֽ���ת�� */
    pRequest->version = ntohs(pRequest->version);
    pRequest->clientSFML = ntohs(pRequest->clientSFML);
    pRequest->clientRFML = ntohs(pRequest->clientRFML);
    pRequest->clientPFML = ntohs(pRequest->clientPFML);
    pRequest->timeOut = ntohl(pRequest->timeOut);

    /* 1 ���汾��Ϣ */
    ptContext = (OOP_ASSOCIATION_CONTEXT_T *)app_get_context();
    if(NULL == ptContext || pRequest->version > ptContext->VER)
    {
        rslt = CONNECT_ERR_VERSION;
        temp = pRequest->mechains + 1;
        goto end;
    }

    /* 2 ������֤��Ϣ */
    SecurityLevel = *pRequest->mechains;
    switch(SecurityLevel)
    {
        case NULL_SECURITY_MODE:  //��������
        {
            temp = pRequest->mechains + 1;
            break;
        }
        case PWD_SECURITY_MODE:   //һ������
        {
            cipherLen = get_len_offset(pRequest->mechains + 1, &offset);
            cipher = pRequest->mechains + 1 + offset;

            if(FALSE == app_check_password(pRequest->mechains + 1, *pRequest->mechains))
            {
                rslt = CONNECT_ERR_PASSWORD;
            }
            temp = pRequest->mechains + 1 + 1 + *pRequest->mechains;
            break;
        }
        case SYM_SECURITY_MODE:   //�ԳƼ���
        {
            cipherLen = get_len_offset(pRequest->mechains + 1, &offset);
            cipher = pRequest->mechains + 1 + offset;
            
            temp = cipher + cipherLen;
            signLen = get_len_offset(temp, &offset);
            sign = temp + offset;
            
            rslt = auth_symmetry_security(cipher, cipherLen, sign, signLen, uSessionData, uSignInfo);
            temp += offset + signLen;
            break;
        }
        case SIG_SECURITY_MODE:   //����ǩ��
        {
            cipherLen = get_len_offset(pRequest->mechains + 1, &offset);
            cipher = pRequest->mechains + 1 + offset;

            temp = cipher + cipherLen;
            signLen = get_len_offset(temp, &offset);
            sign = temp + offset;

            rslt = auth_signature_security(cipher, cipherLen, sign, signLen, uSessionData, uSignInfo);
            temp += offset + signLen;
            break;
        }
        default:
        {
            rslt = CONNECT_ERR_OTHER;
            break;
        }
    }
    
    if(CONNECT_PERMISSION != rslt)
    {                
        //�����ն���Ϣ��֤�����¼��ӿ�
        PRTL_BUF_DEBUG(cipher, cipherLen, "generates a message authentication error event rslt = [%d] cipher:", rslt);
        CheckERC_3109(ptChannel->clientid, cipher, cipherLen);

        /* 3 ����ͨ��Ӧ������ */
        memset(&tConnect, 0, sizeof(APP_CONTEXT_t));
        analyse_set_clientInfo(ptContext, pRequest, &tConnect);
        goto end;
    }

    /* 3 ����ͨ��Ӧ������ */
    memset(&tConnect, 0, sizeof(APP_CONTEXT_t));
    analyse_set_clientInfo(ptContext, pRequest, &tConnect);

    app_set_connect(ptChannel->id, TRUE, &tConnect);
end:
    /* 4 ���췵�ر���APDU */
    timeTag = *temp;
    if(timeTag > 0)
    {
        memcpy(timeTagBuff, temp+1, 10);
    }
    apduLen = sizeof(ConnectResponse_t) + 1 + 1 + ((0 == timeTag) ? 1 : 11); //��֤������Ϣ+FollowReport+TimeTag
    if(SecurityLevel >= SYM_SECURITY_MODE && 0 == rslt)
    {
        /* ����֤������Ϣ */
        sessionLen = sizeof(uSessionData);
        signLen = sizeof(uSignInfo);
        apduLen += 1 + sessionLen + 1 + signLen;
    }
    pResponse = (ConnectResponse_t *)malloc(apduLen);
    if(NULL == pResponse)
    {
        APP698_FMT_DEBUG("Channel %d analyse_connectrequest malloc %d failed\n", ptChannel->id, apduLen);
        return 0;
    }
    memset(pResponse, 0, apduLen);

    pResponse->id = SERVERID_CONNECTRESPONSE;
    pResponse->piid_acd = pRequest->piid;
    
    /* Ӧ������ʱ�����°汾��Ϣ */
    appdb_get_version(ptChannel->clientid, 0, 0, 0x43000300);
    ptVersion = (Factory_Version_t *)app_get_version();
    memcpy(&(pResponse->factoryVersion), ptVersion, sizeof(Factory_Version_t));
    pResponse->version = htons(ptContext->VER);
    memcpy(pResponse->prtlConf, tConnect.prtlConformance, 8);
    memcpy(pResponse->funcConf, tConnect.funcConformance, 16);
    pResponse->serverSFML = htons(tConnect.sendFrameLen);
    pResponse->serverRFML = htons(tConnect.recvFrameLen);
    pResponse->serverWS = tConnect.recvWindowSize;
    pResponse->serverPFML = htons(tConnect.recvApduLen);
    pResponse->timeOut = htonl(tConnect.timeOut);
    pResponse->response.result = rslt;
    
    temp = (uint8 *)pResponse + sizeof(ConnectResponse_t);
    offset = 0;
    if(SecurityLevel >= SYM_SECURITY_MODE && 0 == rslt)
    {
        temp[offset++] = 0x01;//����֤������Ϣ
        
        temp[offset++] = sessionLen;
        memcpy(temp + offset, uSessionData, sessionLen);
        offset += sessionLen;
        
        temp[offset++] = signLen;
        memcpy(temp + offset, uSignInfo, signLen);
        offset += signLen;
    }
    else
    {
        temp[offset++] = 0x00;//û����֤������Ϣ
    }
    temp[offset++] = 0x00;//�����ϱ�
    #if 0
    temp[offset++] = timeTag;//ʱ���ǩ
    if(timeTag > 0)
    {
        memcpy(temp + offset, timeTagBuff, 10);
    }
    #else
    if(timeTag > 0)
    {
        apduLen -= 10;
    }
    
    //����һ�������ʱ�ӱ�ǩ
    temp[offset++] = 0x00;
    #endif

    /* 5 ���췵�ر��� */
    memcpy(&tFrame, ptFrame, sizeof(FRAME_HEAD_T));
    tFrame.dir = 1;
    len = app_get_localaddr(&pAddr);
    if(0 != len)
    {
        tFrame.sAddr.len = len;
        memcpy(tFrame.sAddr.addr, pAddr, len);
        tFrame.sAddr.type = 0;
    }
    tFrame.pApdu = (uint8 *)pResponse;
    tFrame.apduLen = apduLen;
    tFrame.headlen = APUD_COUNT_FRAME_LEN(tFrame.sAddr.len, tFrame.sAddr.logicAddr);
    tFrame.msglen = tFrame.headlen + tFrame.apduLen + APP_698_CS_REGIN_SIZE;

    msglen = app_frame_to_package(&tFrame, pOutMsg, msgmaxlen, TRUE);
    free(pResponse);
    return msglen;
}

/**********************************************************************
* @name      : analyse_releaserequest
* @brief     ��698��Լ�Ͽ�Ӧ�������������
* @param[in] ��CHANNEL_CACHE_T *ptChannel   ͨ�����
               FRAME_HEAD_T *ptFrame        ����֡ͷ
               uint16 msgmaxlen             ������Ϣ��󳤶�
* @param[out]��uint8 *pOutMsg               ������Ϣ
* @return    ��������Ϣ����
* @Create    : ����
* @Date      ��2019-12-16
* @Update    :
**********************************************************************/
uint16 analyse_releaserequest(CHANNEL_CACHE_T *ptChannel, FRAME_HEAD_T *ptFrame, uint8 *pOutMsg, uint16 msgmaxlen)
{
    FRAME_HEAD_T tFrame;
    uint16       apduLen = 0;
    uint8        uApdu[20] = {0};
    uint16       msglen = 0;
    uint8       *pAddr = NULL;
    uint8        len = 0;

    app_set_connect(ptChannel->id, FALSE, NULL);

    uApdu[apduLen++] = SERVERID_RELEASERESPONSE;
    uApdu[apduLen++] = ptFrame->pApdu[1];
    uApdu[apduLen++] = 0x00;   //���
    uApdu[apduLen++] = 0x00;   //flowwreport
    uApdu[apduLen++] = ptFrame->pApdu[2]; //TimeTag
    if(ptFrame->pApdu[2] > 0)
    {
        memcpy(uApdu + apduLen, &(ptFrame->pApdu[3]), 10);
        apduLen += 0;
    }

    memcpy(&tFrame, ptFrame, sizeof(FRAME_HEAD_T));
    tFrame.dir = 1;
    len = app_get_localaddr(&pAddr);
    if(0 != len)
    {
        tFrame.sAddr.len = len;
        memcpy(tFrame.sAddr.addr, pAddr, len);
        tFrame.sAddr.type = 0;
    }
    tFrame.pApdu = uApdu;
    tFrame.apduLen = apduLen;
    tFrame.headlen = APUD_COUNT_FRAME_LEN(tFrame.sAddr.len, tFrame.sAddr.logicAddr);
    tFrame.msglen = tFrame.headlen + tFrame.apduLen + APP_698_CS_REGIN_SIZE;
    
    msglen = app_frame_to_package(&tFrame, pOutMsg, msgmaxlen, TRUE);
    return msglen;
}

/****************************************************************************
*ģ���ţ�
*���ƣ�
*���ܣ�
*���������
*���������
*����
*���أ�0:�ɹ���1:��ʱ���ǩ��-2:֡��������-3��ʽ����-4 ʱ��ʧЧ
****************************************************************************/
int timetag_check(const uint8* inData,uint16 IndataLen)
{
    uint16 offSet = 0;
    
    if(inData[offSet]==0x00)
    {
        return 1;
    }

    if(inData[offSet] != 1)
    {
        return -3;
    }

    offSet++;
    
    if(IndataLen-offSet <10)
    {
        return -2;
    }
    
    DateTimeHex_t Tag_Time;
    OOP_TI_T Tag_TI;
    memcpy((uint8*)&Tag_Time,inData+offSet,7);

    offSet +=7;
    Tag_TI.unit = inData[offSet++];
    Tag_TI.value = inData[offSet++];
    Tag_TI.value = (Tag_TI.value<<8)+inData[offSet++];
    
    if(Tag_TI.value == 0)
    {
        return 0;
    }
    
    time_t LinuxTagTimeStart, LinuxTagTimeEnd;
    if(FALSE == DT_DateTimeHex2Time(&Tag_Time,&LinuxTagTimeEnd))
    {
        return -3;
    }
    
    LinuxTagTimeStart = LinuxTagTimeEnd;
    DT_TimeAddInv(&LinuxTagTimeEnd,Tag_TI);
    DT_TimeSubInv(&LinuxTagTimeStart,Tag_TI);
    
    if((time(NULL) < LinuxTagTimeStart) || (time(NULL) > LinuxTagTimeEnd))
    {
        return -4;
    }
    
    return 0;
}

/**********************************************************************
* @name      : apdu_timetag_check
* @brief     ��ʱ�������
* @param[in] ��CHANNEL_CACHE_T *ptChannel   ͨ�����
               FRAME_HEAD_T *ptFrame        ����֡ͷ
               uint16 msgmaxlen             ������Ϣ��󳤶�
* @param[out]��uint8 *pOutMsg               ������Ϣ
* @return    ��������Ϣ����
* @Create    : ����
* @Date      ��2020-03-27
* @Update    :
**********************************************************************/
BOOLEAN apdu_timetag_check(CHANNEL_CACHE_T *ptChannel, FRAME_HEAD_T *ptFrame, uint8 svrID, uint8 reqType)
{
    int ret = -1;
    uint16 offset = 0;

    PRTL_FMT_TRACE("parse apdu: service[%d], request[%d].\n", svrID, reqType);

    // ��¼������û��ʱ���ǩ�����ж��¸�ʽ�Ƿ���ȷ
    if (svrID == OOP_LINK_RESPONSE)
    {
        ret = serial_chkapdu(svrID, reqType, ptFrame->pApdu+1, ptFrame->apduLen-1);
        if(ret < 0)
        {
            PRTL_FMT_DEBUG("apdu format error service[%d], request[%d]. ret[%d].\n", svrID, reqType, ret);
            return FALSE;
        }
        return TRUE;
    }

    if(reqType == 0)
    {
        return FALSE;
    }
    
#if GET_OPTIMIZATION_EN == 1 || GET_SIMPLIFYRECORD_EN == 1
    if((svrID == OOP_GET_REQUEST) && (reqType > 5 && reqType != 13 && reqType != 23))
#else
    if((svrID == OOP_GET_REQUEST) && (reqType > 5))
#endif
    {
        PRTL_FMT_DEBUG("apdu format error service[%d], request[%d].\n", svrID, reqType);
        return FALSE;
    }

    if((svrID == OOP_SET_REQUEST) && (reqType > 3))
    {
        PRTL_FMT_DEBUG("apdu format error service[%d], request[%d].\n", svrID, reqType);
        return FALSE;
    }

    if((svrID == OOP_ACTION_REQUEST) && (reqType > 3))
    {
        PRTL_FMT_DEBUG("apdu format error service[%d], request[%d].\n", svrID, reqType);
        return FALSE;
    }

    ret = serial_chkapdu(svrID, reqType, ptFrame->pApdu+2, ptFrame->apduLen-2);
    if(ret < 0)
    {
        PRTL_FMT_DEBUG("apdu format error service[%d], request[%d]. ret[%d].\n", svrID, reqType, ret);
        return FALSE;
    }

    offset = ret;
    offset += 2;
    
    //�ж�ʱ���ǩ�Ƿ���ȷ
    ret = timetag_check(&ptFrame->pApdu[offset], ptFrame->apduLen-offset);
    if(ret == 1)//��ʱ���ǩ
    {
        ptChannel->tContext.timeTag.tagFlag = 0;
    }
    else if (ret == 0)//��ʱ���ǩ��û��ʱ
    {
        ptChannel->tContext.timeTag.isTimeOut = FALSE;
        ptChannel->tContext.timeTag.tagFlag = ptFrame->pApdu[offset];
        memcpy(ptChannel->tContext.timeTag.tagBuff, &ptFrame->pApdu[offset+1], 10);
    }
    else if (ret == -4)//��ʱ���ǩ���Ѿ���ʱ
    {
        ptChannel->tContext.timeTag.isTimeOut = TRUE;
        ptChannel->tContext.timeTag.tagFlag = ptFrame->pApdu[offset];
        memcpy(ptChannel->tContext.timeTag.tagBuff, &ptFrame->pApdu[offset+1], 10);
    }
    else
    {
        PRTL_BUF_DEBUG(&ptFrame->pApdu[offset], ptFrame->apduLen-offset, "apdu format error service[%d], request[%d]. ret[%d]:\n", svrID, reqType, ret);
        return FALSE;
    }
    
    return TRUE;
}

/**********************************************************************
* @name      : protocol_analyze_channel
* @brief     ��698��Լ����
* @param[in] ��CHANNEL_CACHE_T *ptChannel   ͨ�����
               FRAME_HEAD_T *ptFrame        ����֡ͷ
               uint16 msgmaxlen             ������Ϣ��󳤶�
* @param[out]��uint8 *pOutMsg               ������Ϣ
               uint8 *pProto                ˽��Э��
               uint8 *pBreakFlag            Ӧ�÷�֡���
* @return    ��������Ϣ����
* @Create    : ����
* @Date      ��2019-12-5
* @Update    :
**********************************************************************/
uint16 protocol_analyze_apdu(CHANNEL_CACHE_T *ptChannel, FRAME_HEAD_T *ptFrame, uint8 *pOutMsg, uint16 msgmaxlen, 
                             uint8 *pProto, uint8 *pBreakFlag)
{
    uint16 msglen = 0;
    uint8 inServerID = 0;
    uint8 reqType = 0;
    uint8 linkState = ptChannel->linkState;
    OOP_OAD_U OAD = {0};
    uint8 oadnum=0;
    memcpy_r(&OAD, ptFrame->pApdu+3, sizeof(OOP_OAD_U));

    inServerID = ptFrame->pApdu[0];
    reqType    = ptFrame->pApdu[1];
    *pProto = 0;
    //���°�������߼���ַ1 ֱ�Ӷ��ն�48��oad
    //�߼���ַ1�����0502���� �ж��¶������ ����40����ֱ�ӹ�Լ�ظ� ��͸������
//    if(1 == ptFrame->sAddr.logicAddr && inServerID == SERVERID_GETREQUEST && reqType == 2)
//    {
//        oadnum = ptFrame->pApdu[3];
//        PRTL_FMT_DEBUG("�߼���ַ1 0502 oadnum %d\n", oadnum);
//    }
    /* �����ַ���ʿ��� F20B����������������ն˻ظ�*/
    if (((TRUE == ptFrame->NoEsamMac) || (1 == ptFrame->sAddr.logicAddr && oadnum < 16) || (TRUE == ptChannel->private[4])) && 
        (SERVERID_LINKRESPONSE != inServerID) &&
        (0xF20B != OAD.nObjID))
    {
        if (SERVERID_GETREQUEST != inServerID && 
           SERVERID_SETREQUEST != inServerID &&
           SERVERID_CONNECTREQUEST != inServerID &&
           SERVERID_ACTIONREQUEST != inServerID)
        {
            PRTL_FMT_DEBUG("�������͵�ַ��֧�ִ˲��� Serverid 0x%x\n", inServerID);
            return 0;
        }
        
        //ʹ��͸���ķ�ʽ����APDU������APP����ʱͳһ���ն����
        #if 1
        if ((0x40000200 == OAD.value) && (SERVERID_SETREQUEST == inServerID))
        #else
        if (0)
        #endif
        {
            PRTL_FMT_DEBUG("���ɵװ��ʱ��ת���ն����\n");
        }
        #if defined (PRODUCT_ZCU_1)
        else if (0x302A == OAD.nObjID)
        {
            PRTL_FMT_DEBUG("���ɴų���Ϣ���������ն�\n");
        }
        #endif
        else
        {
            msglen = analyse_puAmr_acMeter(ptChannel, ptFrame, pOutMsg, msgmaxlen, pProto);

            //��ɺ����˽������
            memset_s(ptChannel->private, sizeof(ptChannel->private), 0, sizeof(ptChannel->private));

            return msglen;
        }
    }

#if defined (AREA_JIANGSU) && defined (PRODUCT_ZCU_1)
    if(2 == ptFrame->sAddr.logicAddr) //�߼���ַ2�ı���͸����Ѳ����
    {
        uint8  tranbuf[1024] = {0};
        uint16 transLen = 0;
        transLen = app_frame_to_package(ptFrame, tranbuf, transLen, TRUE);
        
        if(g_lcmdev.fd > 0)
        {
            uart_write(&g_lcmdev, tranbuf, transLen);
            return 0;
        }
    }

    //һЩ�ض�OAD͸����Ѳ����
    if((OAD.nObjID==0x4040) || (OAD.nObjID==0x4041) || (OAD.nObjID==0x2050) || (OAD.nObjID==0x3120))
    {
        uint8  tranbuf[1024] = {0};
        uint16 transLen = 0;
        transLen = app_frame_to_package(ptFrame, tranbuf, transLen, TRUE);
        
        if(g_lcmdev.fd > 0)
        {
            uart_write(&g_lcmdev, tranbuf, transLen);
            return 0;
        }
    }

    //һЩ�ض�OAD�ַ���Ѳ����
    if((OAD.nObjID==0x4300) || (OAD.nObjID==0x4001) || (OAD.value==0x40000200))
    {
        uint8  tranbuf[1024] = {0};
        uint16 transLen = 0;
        transLen = app_frame_to_package(ptFrame, tranbuf, transLen, TRUE);
        
        if(g_lcmdev.fd > 0)
        {
            uart_write(&g_lcmdev, tranbuf, transLen);
        }
    }    
#endif

    if(apdu_timetag_check(ptChannel, ptFrame, inServerID, reqType) == FALSE)
    {
        //TODO �쳣֡����
    }

    //���߼���ַ����ͨ����˽��������
    ptChannel->private[0] = ptFrame->sAddr.logicAddr;

    //��reqType����ͨ����˽��������
    ptChannel->private[1] = reqType;

    switch(inServerID)
    {
        /* case�еĴ��뾡������һ������ */
        case SERVERID_LINKREQUEST:// Ԥ�������� ��վ����������ʱʹ��
        {
            msglen = analyse_prelinkrequest(ptChannel, ptFrame, pOutMsg, msgmaxlen, pProto);
            break;
        }
        case SERVERID_LINKRESPONSE:// Ԥ������Ӧ֡
        {
            msglen = analyse_prelinkresponse(ptChannel, ptFrame, pOutMsg, msgmaxlen, pProto);
            break;
        }
        case SERVERID_CONNECTREQUEST:// Ӧ��������Ӧ֡
        {
            msglen = analyse_connectrequest(ptChannel, ptFrame, pOutMsg, msgmaxlen);
            break;
        }
        case SERVERID_RELEASEREQUEST:// �Ͽ�Ӧ����������
        {
            msglen = analyse_releaserequest(ptChannel, ptFrame, pOutMsg, msgmaxlen);
            break;
        }
        case SERVERID_GETREQUEST:// ��ȡ����
        {
            msglen = get_request_analyse(ptChannel, ptFrame, pOutMsg, msgmaxlen, pBreakFlag);
            break;
        }
        case SERVERID_SETREQUEST:// ���÷���
        {
            msglen = set_request_analyse(ptChannel, ptFrame, pOutMsg, msgmaxlen, pBreakFlag);
            break;
        }
        case SERVERID_ACTIONREQUEST:// ��������
        {
            msglen = action_request_analyse(ptChannel, ptFrame, pOutMsg, msgmaxlen, pBreakFlag);
            break;
        }
        case SERVERID_REPORTRESPONSE:// �ϱ�����
        {
            msglen = report_response_analyse(ptChannel, ptFrame, pOutMsg, msgmaxlen, pBreakFlag);
            break;
        }
        case SERVERID_PROXYREQUEST:
        {
            ProxyRequest_ClientApdu(ptChannel, ptFrame, pOutMsg, msgmaxlen, pBreakFlag);
            break;  
        }
        
        /* �Լ��� */
        default:
        {
            return 0;
        }
    }

    //��ɺ����˽������
    memset_s(ptChannel->private, sizeof(ptChannel->private), 0, sizeof(ptChannel->private));

    /* ͨ�Ŵ������£�֪ͨͨ�Ŵ���APPЭ�̽��*/
    if(app_comm_proxy_isuse())
    {
        //����Ӧ��¼֡ʱ����վ������/�Ͽ�Ӧ������ʱ, ���Ȩ��Ӧ�����ӹر�ʱ��֪ͨӦ������״̬
        if(((inServerID == SERVERID_LINKRESPONSE)&&(linkState == LINK_LOGIN)) 
            || (inServerID == SERVERID_CONNECTREQUEST) || (inServerID == SERVERID_RELEASEREQUEST)||(ptChannel->bConnect == FALSE)) 
        {
            if((ptChannel->bOpen == TRUE) && (ptChannel->tChannel.workmode == MODE_CLIENT) 
                && ((ptChannel->tChannel.medium == COMM_MEDIA_GPRS)||(ptChannel->tChannel.medium == COMM_MEDIA_ETH)))
            {
                proxy_coninfo_notify(ptChannel->id);
                
                //��δ�յ�ȷ�ϣ�����10��
                g_proxyApplyAck[ptChannel->id].bSend = TRUE;
                g_proxyApplyAck[ptChannel->id].bAck = FALSE;
                g_proxyApplyAck[ptChannel->id].retry = 10;
            }
        }
    }

    return msglen;
}


