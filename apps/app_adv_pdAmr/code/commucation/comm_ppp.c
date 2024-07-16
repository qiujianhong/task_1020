/*
*********************************************************************
* Copyright(C) 2019 �Ͼ��������ӹɷ����޹�˾
* All rights reserved. 
* @brief��   ͨ��ģ������ͨ��
* @date��    2019-10-31
* @history�� 
*********************************************************************
*/

#include "baseCommon.h"
#include <net/if.h>
#include <error.h>
#include "msgUdp.h"
#include "storage.h"
#include <linux/tcp.h>
#include "task_msg.h"
#include "db_api.h"
#include "msg_id.h"
#include "task_id.h"

#include "common_base.h" 
#include "comm_ppp.h"

uint8    *g_PPPDiagFlag[2] = {NULL};
uint8     g_PPPdataBuf[2][BUF_LENGTH + 4] = {{0}, {0}};   /* ������ */

/**********************************************************************
* @name      : ppp_find_connectfd
* @brief     ����ȡconnect_fd
* @param[in] ��uint8 channle     ͨ��ID
               int connect_fd[]  ����б�
* @param[out]��uint8 *pIndex     �ڲ����
* @return    ��socket���
* @Create    : ����
* @Date      ��2020-3-7
* @Update    :
**********************************************************************/
int ppp_find_connectfd(uint8 channle, int connect_fd[], uint8 *pIndex)
{
    if(channle >= CHL_ETH_1 && channle < CHL_GPRS_1)
    {
        return -1;
    }
    
    if(channle >= CHL_GPRS_2)
    {
        channle -= CHL_GPRS_2;
    }
    else
    {
        channle -= CHL_GPRS_1;
    }
    if(channle < 2)
    {
        *pIndex = channle;
        return connect_fd[channle];
    }
    return -1;
}

/**********************************************************************
* @name      : ppp_get_channeloffset
* @brief     ����ȡ�ڲ�ͨ��ƫ��
* @param[in] ��uint8 channle    ͨ��ID
* @param[out]��
* @return    ��ƫ��
* @Create    : ����
* @Date      ��2020-8-29
* @Update    :
**********************************************************************/
int ppp_get_channeloffset(uint8 channle)
{
    int offset = -1;
    if(channle >= CHL_ETH_1 && channle < CHL_GPRS_1)
    {
        return offset;
    }
    
    if(channle >= CHL_GPRS_2)
    {
        channle -= CHL_GPRS_2;
    }
    else
    {
        channle -= CHL_GPRS_1;
    }
    offset = channle;
    return offset;
}

/**********************************************************************
* @name      : ppp_close_allfd
* @brief     ��ppp���ڹرվ��
* @param[in] ��COMM_PPP_HANDLE_T *ptHandle
* @param[out]��
* @return    ��
* @Create    : ����
* @Date      ��2020-3-7
* @Update    :
**********************************************************************/
void ppp_close_allfd(COMM_PPP_HANDLE_T *ptHandle)
{
    uint8 i;

    for(i = 0; i < ptHandle->masterNUM; i++)
    {
        ptHandle->ip[i] = 0;
        ptHandle->port[i] = 0;
        if(ptHandle->connect_fd[i] > 0)
        {
            close(ptHandle->connect_fd[i]);
            ptHandle->connect_fd[i] = -1;
        }
    }
    ptHandle->masterNUM = 0;
}

/**********************************************************************
* @name      : comm_ppp_send
* @brief     ��ppp���ڷ���Ϣ �Ƿ�������ʽ
* @param[in] ��uint8 ifnum                 ���0-1
               char *ifName                ��������
               COMM_PPP_HANDLE_T *ptHandle �߳̾��
* @param[out]��uint8 *pNextState           ��һ��״̬
* @return    �����ͳ���/-1������Ҫ�ر�ͨ��
* @Create    : ����
* @Date      ��2020-3-7
* @Update    : 2020-8-29
**********************************************************************/
int comm_ppp_send(uint8 ifnum, char *ifName, COMM_PPP_HANDLE_T *ptHandle, uint8 *pNextState)
{
    uint8 index;
    int   fdSocket = 0;
    uint8  channel = 0;
    uint8 protocol = -1;         /* �ڲ�Э�� */
    int    sendLen = 0;
    int     outlen = 0;
    uint16  dataLen = 0;
    uint8 dataBuf[200] = {0};   /* ������ */
    COMM_IPCMSG_T *ptIPCMsg = NULL;

    *pNextState = PPP_RECV_DATA;
    
    dataLen = comm_read_queue(ptHandle->ptSendList, g_PPPdataBuf[ifnum]);
    while(dataLen)
    {
        if(dataLen < 4)
        {
            COMM_FMT_TRACE("comm_ppp_send msg less then 4\n");
            goto cont;
        }
        
        channel = *g_PPPdataBuf[ifnum];
        protocol = comm_get_protocol(g_PPPdataBuf[ifnum]);
        fdSocket = ppp_find_connectfd(channel, ptHandle->connect_fd, &index);
        
        if(PROT_CLOSE_CHANNLE == protocol)
        {
            if(fdSocket > 0)
            {
                close(fdSocket);
                ptHandle->connect_fd[index] = -1;
            }
            /* �ر�ͨ�� */
            ptIPCMsg = (COMM_IPCMSG_T *)dataBuf;
            ptIPCMsg->id = channel;
            ptIPCMsg->protocol = PROT_CLOSE_CHANNLE;
            dataLen = sizeof(COMM_IPCMSG_T);
            if(0 != comm_write_queue(ptHandle->ptRecvList, dataBuf, dataLen))
            {
                COMM_FMT_TRACE("Msg Queue is full\n");
            }
            COMM_FMT_DEBUG("%s close channel %d\n", ifName, channel);
            goto cont;
        }
        if(PRO_DATA_TRANSMIT != protocol && PRO_LOGIN_FRAME != protocol && PRO_EXIT_LOGIN_FRAME != protocol
            && PRO_HEART_FRAME != protocol)
        {
            goto cont;  //������ʽ������
        }

        COMM_BUF_DEBUG(g_PPPdataBuf[ifnum] + 4, dataLen - 4, "%s channel %d send:", ifName, channel);

        if(fdSocket > 0)
        {
            sendLen = send(fdSocket, g_PPPdataBuf[ifnum]+4, dataLen-4, 0);
            if(sendLen > 0)
            {
                outlen += sendLen;
            }
            else
            {
                *pNextState = PPP_CONNECT_MASTER;
                close(fdSocket);
                ptHandle->connect_fd[index] = -1;
                /* �ر�ͨ�� */
                ptIPCMsg = (COMM_IPCMSG_T *)dataBuf;
                ptIPCMsg->id = channel;
                ptIPCMsg->protocol = PROT_CLOSE_CHANNLE;
                dataLen = sizeof(COMM_IPCMSG_T);
                if(0 != comm_write_queue(ptHandle->ptRecvList, dataBuf, dataLen))
                {
                    COMM_FMT_TRACE("Msg Queue is full\n");
                }
                COMM_FMT_DEBUG("%s close channel %d\n", ifName, channel);
            }
        }
cont:
        memset(g_PPPdataBuf[ifnum], 0, BUF_LENGTH + 4);
        dataLen = comm_read_queue(ptHandle->ptSendList, dataBuf);
    }
    return outlen;
}

/**********************************************************************
* @name      : comm_ppp_recv
* @brief     ��ppp��������Ϣ �Ƿ�������ʽ
* @param[in] ��uint8 ifnum                 ���0-1
               char *ifName                   ��������
               COMM_PPP_HANDLE_T *ptHandle    �߳�˽������
* @param[out]��uint8 *pNextState              ��һ��״̬
* @return    �����ݳ���/-1 �����쳣
* @Create    : ����
* @Date      ��2020-3-7
* @Update    : 2020-8-29
**********************************************************************/
int comm_ppp_recv(uint8 ifnum, char *ifName, COMM_PPP_HANDLE_T *ptHandle, uint8 *pNextState)
{
    uint8 i;
    int recvLen = 0;
    int outlen = 0;
    uint8   dataBuf[200] = {0};   /* ������ */
    uint16  bufLen = 0;
    struct timespec curtime = {0};
    COMM_IPCMSG_T  *ptIPCMsg = NULL;
    uint8   channel = 0;

    *pNextState = PPP_SEND_DATA;

    clock_gettime(CLOCK_MONOTONIC, &curtime);
    for(i = 0; i < ptHandle->masterNUM; i++)
    {
        if(ptHandle->connect_fd[i] <= 0)
        {
            if(curtime.tv_sec - ptHandle->connet_time[i] >= 5)
            {
                /* ���ߵ���վ, 5s����һ�� */
                *pNextState = PPP_CONNECT_MASTER;
            }
            continue;
        }
        
        recvLen = 0;
        recvLen = recv(ptHandle->connect_fd[i], g_PPPdataBuf[ifnum], BUF_LENGTH, 0);    /*socket�������ݽ���*/
        if(recvLen > 0)
        {
            outlen += recvLen;
            channel = (0 == ifnum ? CHL_GPRS_1 : CHL_GPRS_2) + i;
            comm_add_channel(channel, g_PPPdataBuf[ifnum], recvLen);
            bufLen = recvLen + 4;
            if(0 != comm_write_queue(ptHandle->ptRecvList, g_PPPdataBuf[ifnum], bufLen))
            {
                COMM_FMT_TRACE("Msg Queue is full\n");
            }
            COMM_BUF_DEBUG(g_PPPdataBuf[ifnum] + 4, recvLen - 4, "%s channel %d recv:", ifName, channel);
            ptHandle->connet_time[i] = (uint32)curtime.tv_sec;
        }
        if(recvLen <= 0 && (curtime.tv_sec - ptHandle->connet_time[i] > 10))
        {
            struct tcp_info info;
            int len = sizeof(info);
            getsockopt(ptHandle->connect_fd[i], IPPROTO_TCP, TCP_INFO, &info, (socklen_t *)&len);
            //printf("tcp state %d\n", info.tcpi_state);
            if(TCP_CLOSE_WAIT == info.tcpi_state || TCP_CLOSE == info.tcpi_state)
            {
                close(ptHandle->connect_fd[i]);
                ptHandle->connect_fd[i] = -1;
                /* �ر�ͨ�� */
                ptIPCMsg = (COMM_IPCMSG_T *)dataBuf;
                ptIPCMsg->id = (0 == ifnum ? CHL_GPRS_1 : CHL_GPRS_2) + i;
                ptIPCMsg->protocol = PROT_CLOSE_CHANNLE;
                bufLen = sizeof(COMM_IPCMSG_T);
                if(0 != comm_write_queue(ptHandle->ptRecvList, dataBuf, bufLen))
                {
                    COMM_FMT_TRACE("Msg Queue is full\n");
                }
                COMM_FMT_DEBUG("%s Server close the connect, channel %d\n", ifName, ptIPCMsg->id);
            }
            ptHandle->connet_time[i] = (uint32)curtime.tv_sec;
        }

    }
    
    return outlen;
}

/**********************************************************************
* @name      : comm_ppp_udpsend
* @brief     ��ppp���ڷ���Ϣ udp��ʽ
* @param[in] ��int fd                      socket���
               uint8 *pBuffer              ������
               uint16 bufLen               ��������С
               COMM_PPP_HANDLE_T *ptHandle �߳̾��
* @param[out]��
* @return    �����ͳ���/-1������Ҫ�ر�ͨ��
* @Create    : ����
* @Date      ��2020-8-29
* @Update    :
**********************************************************************/
int comm_ppp_udpsend(int fd, uint8 *pBuffer, uint16 bufLen, COMM_PPP_HANDLE_T *ptHandle)
{
    uint8   offset = -1;
    uint8 protocol = -1;         /* �ڲ�Э�� */
    int    sendLen = 0;
    struct sockaddr_in address;
    
    if(bufLen <= 4)
    {
        COMM_FMT_TRACE("comm_eth_udpsend msg less then 4\n");
        return 0;
    }

    protocol = comm_get_protocol(pBuffer);
    /*�����������ת���͡�����֡����½֡���˳���½֡, �����ݷ��ͳ�ȥ*/
    if(PRO_DATA_TRANSMIT != protocol && PRO_LOGIN_FRAME != protocol && PRO_EXIT_LOGIN_FRAME != protocol
        && PRO_HEART_FRAME != protocol)
    {
        return 0;  //������ʽ������
    }

    offset = ppp_get_channeloffset(*pBuffer);
    bzero(&address, sizeof(address));
    address.sin_family      = AF_INET;
    address.sin_addr.s_addr = htonl(ptHandle->ip[offset]);
    address.sin_port        = htons(ptHandle->port[offset]);
    sendLen = sendto(fd, pBuffer+4, bufLen-4, 0, (struct sockaddr *)&address, sizeof(address));
    if(sendLen < 0)
    {
        return -1;
    }
    return sendLen;
}

/**********************************************************************
* @name      : comm_ppp_udprecv
* @brief     ��ppp��������Ϣ udp��ʽ
* @param[in] ��int fd                      socket���
               uint16 bufLen               ��������С
               COMM_PPP_HANDLE_T *ptHandle �߳̾��
* @param[out]��uint8 *pBuffer              ������
               uint8 *pOffset              ͨ��ƫ��
* @return    �����ݳ���/-1 �����쳣
* @Create    : ����
* @Date      ��2020-8-29
* @Update    :
**********************************************************************/
int comm_ppp_udprecv(int fd, uint8 *pBuffer, uint16 bufLen, COMM_PPP_HANDLE_T *ptHandle, uint8 *pOffset)
{
    uint8 i;
    int recvLen = 0;
    int sin_len;
    uint32 ip = 0;
    uint16 port = 0;
    struct sockaddr_in sourceAddr;

    /* UDPģʽ */
    sin_len = sizeof(struct sockaddr);
    recvLen = recvfrom(fd, pBuffer, bufLen, 0, (struct sockaddr *)&sourceAddr, (socklen_t *)&sin_len);
    if(recvLen > 0)
    {
        ip = ntohl(sourceAddr.sin_addr.s_addr);
        port = ntohs(sourceAddr.sin_port);
        /* UDPģʽ �����վ */
        for(i = 0; i < ptHandle->masterNUM; i++)
        {
            if(ptHandle->ip[i] == ip && ptHandle->port[i] == port)
            {
                *pOffset = i;
                return recvLen;
            }
        }
    }
    return -1;
}

/**********************************************************************
* @name      : comm_timer_getDCM
* @brief     ��DCM��ѯ��Ϣ��ʱ��
* @param[in] ��int socketfd    task��Ϣ���
               void *pArg      ���
* @param[out]��
* @return    ��
* @Create    : 
* @Date      ��2020-8-29
* @Update    :
**********************************************************************/
void comm_timer_getDCM(int socketfd, void *pArg)
{
    int  ret = 0;
    uint8 id = 0;
    static uint8 s_getDCMFirst[2] = {TRUE, TRUE};
    COMM_PPP_HANDLE_T *ptHandle = (COMM_PPP_HANDLE_T *)pArg;
    struct timespec curtime = {0};

    if(socketfd <= 0 || NULL == ptHandle)
    {
        COMM_FMT_DEBUG("comm_timer_getDCM input null\n");
        return;
    }

    id = ptHandle->no + 1;

    if(TRUE == s_getDCMFirst[ptHandle->no])
    {
        g_PPPDiagFlag[ptHandle->no] = &ptHandle->bDiagOK;
    }

    clock_gettime(CLOCK_MONOTONIC, &curtime);

    /* �״�ִ����Ҫ��ѯһ�²���״̬ */
    if((TRUE == ptHandle->bDiagOK && FALSE == ptHandle->bGetDCM) || TRUE == s_getDCMFirst[ptHandle->no]
       || curtime.tv_sec > ptHandle->getApnTime + 120)
    {
        ret = task_send_labelmsg(socketfd, MSG_COMM_GETDIAL, id, &id, sizeof(uint8), PD_MSG_MONITOR);
        if(0 != ret)
        {
            COMM_FMT_DEBUG("Comm send get dial %d msg, ret %d\n", id, ret);
        }
        s_getDCMFirst[ptHandle->no] = FALSE;
    }
}

/**********************************************************************
* @name      : comm_ppp_dcmmsg
* @brief     ��DCM��ѯ��Ϣ����
* @param[in] ��void *playload    ��Ϣ����
               void *pArg        ˽������
               void *pQueueList  ������Ϣ�б�
* @param[out]��
* @return    ��0-�ɹ�/������
* @Create    : 
* @Date      ��2019-11-5
* @Update    : 2020-3-18
**********************************************************************/
int comm_ppp_dcmmsg(uint8 *playload, uint16 msglen, void *pArg)
{
    struct timespec curtime = {0};
    WIRE_DIAG_INFO_T tDiagInfo;
    COMM_PPP_HANDLE_T *ptHandle = (COMM_PPP_HANDLE_T *)pArg;

    if(NULL == playload || NULL == pArg)
    {
        COMM_FMT_DEBUG("comm_ppp_dcmmsg input null\n");
        return -1;
    }

    appmsg_wirediag_split(playload, &tDiagInfo);

    if(0 == tDiagInfo.diagstatus)
    {
        ptHandle->bDiagOK = TRUE;
    }
    else
    {
        ptHandle->bDiagOK = FALSE;
    }

    /* û��SIM������û������ */
    if(1 != tDiagInfo.simstatus || FALSE == ptHandle->bGetCfg)
    {
        ptHandle->getApnTime = curtime.tv_sec;
        return 0;
    }

    if(0 == tDiagInfo.APN.nNum)
    {
        COMM_FMT_DEBUG("PPP %d get apn is zero!\n", ptHandle->no);
        goto changeAPN;
    }

    COMM_FMT_DEBUG("PPP %d get apn is %s, operat %d!\n", ptHandle->no, tDiagInfo.APN.value, tDiagInfo.operat);

    clock_gettime(CLOCK_MONOTONIC, &curtime);

    /* ���APN�Ƿ���ϵ�ǰAPN */
    if(0 == strcmp(ptHandle->tGprsCfg.tOopGprs.apn.apn.value, tDiagInfo.APN.value))
    {
        ptHandle->bGetDCM = TRUE;
        ptHandle->getApnTime = curtime.tv_sec;
        return 0;
    }

changeAPN:
    /* ֧�ֲ�����apnʹ��Ĭ�ϵ�APN */
    if(0 == ptHandle->tGprsCfg.tOopGprs.apn.apn.nNum)
    {
        ptHandle->bGetDCM = TRUE;
        ptHandle->getApnTime = curtime.tv_sec;
        return 0;
    }

    ptHandle->bGetDCM = FALSE;
    if(curtime.tv_sec > ptHandle->getApnTime + 120)
    {
        /* 2minû����� ���л�APN */
        COMM_FMT_DEBUG("PPP %d no match apn %s, set to %s!\n", ptHandle->no, tDiagInfo.APN.value, 
                                                               ptHandle->tGprsCfg.tOopGprs.apn.apn.value);
        uint8  msgbuf[2048]; /* ����Ҫ��ʼ�� */
        uint16 msglen = 0;
        WIRE_DIAG_APN_T tDiagAPN;
        int ret = 0;

        tDiagAPN.id = ptHandle->no + 1;
        tDiagAPN.stack = 2;
        tDiagAPN.diagnum = 1;
        tDiagAPN.tDiagApn[0].operat = tDiagInfo.operat;
        memcpy(&(tDiagAPN.tDiagApn[0].apn), &(ptHandle->tGprsCfg.tOopGprs.apn.apn), sizeof(OOP_VISIBLEVAR_T));
        memcpy(&(tDiagAPN.tDiagApn[0].username), &(ptHandle->tGprsCfg.tOopGprs.apn.username), sizeof(OOP_VISIBLEVAR_T));
        memcpy(&(tDiagAPN.tDiagApn[0].pwd), &(ptHandle->tGprsCfg.tOopGprs.apn.pwd), sizeof(OOP_VISIBLEVAR_T));
        msglen = appmsg_wireapn_package(&tDiagAPN, msgbuf, 2048);
        if(msglen > 0)
        {
            ret = comm_send_label_msg(MSG_COMM_SETAPN, ptHandle->no + 1, msgbuf, msglen);
            COMM_FMT_DEBUG("Comm set dial %d apn %s msg, ret %d\n", ptHandle->no + 1, tDiagAPN.tDiagApn[0].apn.value, ret);
        }

        ptHandle->getApnTime = curtime.tv_sec;
    }
    return -1;
}

/**********************************************************************
* @name      : comm_ppp_eventmsg
* @brief     ������ģ��֪ͨ��Ϣ����
* @param[in] ��void *playload    ��Ϣ����
               void *pArg        ˽������
               void *pQueueList  ������Ϣ�б�
* @param[out]��
* @return    ��0-�ɹ�/������
* @Create    : 
* @Date      ��2020-3-18
* @Update    : 
**********************************************************************/
int comm_ppp_eventmsg(uint8 *playload, uint16 msglen, void *pArg)
{
    uint8 port;
    uint8 status;

    if(NULL == playload)
    {
        COMM_FMT_DEBUG("comm_ppp_eventmsg input null\n");
        return -1;
    }

    port = *playload;
    status = *(playload + 1);
    if(0 == port || port > 2)
    {
        COMM_FMT_DEBUG("comm_ppp_eventmsg port %d err\n", port);
        return -1;
    }
    COMM_FMT_DEBUG("comm get diag event ppp%d status %d\n", port-1, status);

    if(0 == status)
    {
        if(g_PPPDiagFlag[port-1])
        {
            *(g_PPPDiagFlag[port-1]) = TRUE;
        }
    }
    else
    {
        if(g_PPPDiagFlag[port-1])
        {
            *(g_PPPDiagFlag[port-1]) = FALSE;
        }
    }
#ifdef AREA_ZHEJIANG
    /* �㽭����, sdk��ַ�ȹ̶����� ����ͨ�������·� */
    uint32 sdkip = 0x0a89fd73;
    if(2 == port)
    {
        if(0 == status)
        {
            comm_add_route_net("ppp1", sdkip);
            comm_add_route_net("ppp1", sdkip - 1);
        }
        else
        {
            comm_del_route_net("ppp1", sdkip);
            comm_del_route_net("ppp1", sdkip - 1);
        }
    }
#endif
    return 0;    
}

/**********************************************************************
* @name      : comm_getcheck_pppcfg
* @brief     ����ȡ�����ppp�����Ƿ����仯
* @param[in] ��uint8 ifuint      �ӿڱ��
               void *dbHandle    �������ľ��
               void *pCfg        ���ô洢��ַ
* @param[out]��uint8 *pRestart   �߳��������
* @return    ��0-�ɹ�/������
* @Create    : ����
* @Date      ��2020-8-29
* @Update    :
**********************************************************************/
int comm_getcheck_pppcfg(int sockdbfd, void *pCfg, uint8 *pRestart)
{
    uint8 i;
    int retGprs = 0;
    int retMaster = 0;
    uint32 readLen = 0;
    uint8  bChange = FALSE;
    uint8  bRestart = FALSE;
    NOMAL_OAD_T        tOAD;
    OOP_GPRSCONFIG_T   tGPRS;
    OOP_MASTERPARAMS_T tMaster;
    COMM_PPP_HANDLE_T *ptHandle = (COMM_PPP_HANDLE_T *)pCfg;
    uint8 ifuint = ptHandle->no;

    static uint8 s_pppcfg[2][2] = {{0}, {0}};  //�״λ�ȡʧ�ܴ�ӡ����ֹ��־����

    if(ifuint > 1 || sockdbfd <= 0 || NULL == pCfg)
    {
        return -1;
    }
    
    bzero(&tOAD, sizeof(NOMAL_OAD_T));
    tOAD.logicId = PD_LOGIC_ID;

#ifdef AREA_ZHEJIANG
    OOP_ZJ_CONNECT_T   tType;
    tOAD.oad.value = 0x45300200;
    tType.type = ZJ_CONNECT_NULL;
    db_read_nomal(sockdbfd, &tOAD, sizeof(OOP_ZJ_CONNECT_T), (uint8 *)&tType, &readLen);
    if(ZJ_CONNECT_PUBLIC == tType.type || ZJ_CONNECT_SPECIAL == tType.type)
    {
        ptHandle->bClose = FALSE;
    }
    else
    {
        ptHandle->bClose = TRUE;
    }
#endif

    tOAD.oad.value = (0 == ifuint) ? 0x45000200 : 0x45010200;
    memset(&tGPRS, 0, sizeof(OOP_GPRSCONFIG_T));
    retGprs = db_read_nomal(sockdbfd, &tOAD, sizeof(OOP_GPRSCONFIG_T), (uint8 *)&tGPRS, &readLen);
    if(0 == retGprs)
    {
        if(readLen != sizeof(OOP_GPRSCONFIG_T))
        {
            retGprs = -1;
        }
        else if(0 != memcmp(&(ptHandle->tGprsCfg.tOopGprs), &tGPRS, readLen))
        {
            bChange = TRUE;
        }
    }
    else if(FALSE == s_pppcfg[ifuint][0])
    {
        COMM_FMT_DEBUG("Read 0x%x failed, ret %d!\n", tOAD.oad.value, retGprs);
        s_pppcfg[ifuint][0] = TRUE;
    }

    readLen = 0;
    tOAD.oad.value = (0 == ifuint) ? 0x45000300 : 0x45010300;
    memset(&tMaster, 0, sizeof(OOP_MASTERPARAMS_T));
    retMaster = db_read_nomal(sockdbfd, &tOAD, sizeof(OOP_MASTERPARAMS_T), (uint8 *)&tMaster, &readLen);
    if(0 == retMaster)
    {
        if(readLen != sizeof(OOP_MASTERPARAMS_T))
        {
            retMaster = -1;
        }
        else if(0 != memcmp(&(ptHandle->tGprsCfg.tOopMaster), &tMaster, readLen))
        {
            bChange = TRUE;
        }
    }
    else if(FALSE == s_pppcfg[ifuint][1])
    {
        COMM_FMT_DEBUG("Read 0x%x failed, ret %d!\n", tOAD.oad.value, retMaster);
        s_pppcfg[ifuint][1] = TRUE;
    }

    if(TRUE == bChange)
    {
        sleep(10); //�ȴ�10s������
        /* �鿴�Ƿ���Ҫ���� */
        if(0 == retGprs)
        {
            if(ptHandle->tGprsCfg.tOopGprs.workmode != tGPRS.workmode || 
               ptHandle->tGprsCfg.tOopGprs.contype != tGPRS.contype ||
               ptHandle->tGprsCfg.tOopGprs.conmode != tGPRS.conmode ||
               0 != memcmp(&(ptHandle->tGprsCfg.tOopGprs.apn), &(tGPRS.apn), sizeof(OOP_APN_T)))
            {
                bRestart = TRUE;
            }
            else
            {
                ptHandle->bChangeParam = TRUE;
            }

            memcpy(&(ptHandle->tGprsCfg.tOopGprs), &tGPRS, sizeof(tGPRS));
        }
        if(0 == retMaster)
        {
            if(0 != memcmp(&(ptHandle->tGprsCfg.tOopMaster), &tMaster, sizeof(tMaster)))
            {
                bRestart = TRUE;
            }
            memcpy(&(ptHandle->tGprsCfg.tOopMaster), &tMaster, sizeof(tMaster));
        }
    }

    if(TRUE == bRestart)
    {
        COMM_FMT_DEBUG("PPP %d change configure, need to restart.\n", ifuint);
        /* ������ ����·�� */
        for(i = 0; i < ptHandle->masterNUM; i++)
        {
            if(0 == ptHandle->ip[i])
            {
                continue;
            }
            comm_del_route_net((0 == ifuint) ? "ppp0" : "ppp1", ptHandle->ip[i]);
        }

        if(TRUE == ptHandle->bGetCfg)
        {
            *ptHandle->pAppReStart = 1;
            sleep(2);
        }
        if(pRestart)
        {
            *pRestart = TRUE;
        }
        ptHandle->bGetDCM = FALSE;
    }

    if(0 == retGprs && 0 == retMaster)
    {
        ptHandle->bGetCfg = TRUE;
    }
    else
    {
        ptHandle->bGetCfg = FALSE;
    }

    return retGprs | retMaster;
}

/**********************************************************************
* @name      : comm_ppp0_start
* @brief     ������Ƿ���Ҫ����ppp0�߳�
* @param[in] ��
* @param[out]��
* @return    ��TRUE-����/FALSE-������
* @Create    : ����
* @Date      ��2019-11-1
* @Update    :
**********************************************************************/
BOOLEAN comm_ppp_start(void *pArg)
{
    COMM_PPP_HANDLE_T *ptHandle = (COMM_PPP_HANDLE_T *)pArg;

    if(NULL == pArg)
    {
        return FALSE;
    }
    
    if(NULL == pArg || 0 == *ptHandle->pIPCok || FALSE == ptHandle->bGetCfg || FALSE == ptHandle->bGetDCM 
       || FALSE == ptHandle->bDiagOK || TRUE == ptHandle->bClose || 0 == ptHandle->tGprsCfg.tOopMaster.nNum 
       || MODE_CLIENT != ptHandle->tGprsCfg.tOopGprs.workmode)
    {
        return FALSE;
    }

    return TRUE;
}

/**********************************************************************
* @name      : comm_ppp_tcpwork
* @brief     ��tcp��ʽ
* @param[in] ��COMM_PPP_HANDLE_T *ptHandle
* @param[out]��
* @return    ��
* @Create    : ����
* @Date      ��2020-8-29
* @Update    : 
**********************************************************************/
void comm_ppp_tcpwork(COMM_PPP_HANDLE_T *ptHandle)
{
    uint8  i;    
    uint8  statePPP = PPP_SEND_DATA; /* ״̬�� */
    struct timespec curtime = {0};
    struct timespec oldtime = {0};
    int         fdSocket = -1;
    uint8       dataBuf[200] = {0};  /* ������ */
    uint16      dataLen = 0;
    CHANNEL_INFO_T *ptChanInfo = 0;
    COMM_IPCMSG_T  *ptIPCMsg = NULL;
    char            ifName[24] = {0};
    uint8           ifunit = ptHandle->no;
    uint8           conCnt[OOP_MAX_HOST] = {0};

    uint32 applyRxFlow = 0;   /* Ӧ�ò����� */
    uint32 applyTxFlow = 0;   /* Ӧ�ò����� */

    OOP_GPRSCONFIG_T *ptPPPCfg   = &(ptHandle->tGprsCfg.tOopGprs);
    OOP_MASTERPARAMS_T *ptMaster = &(ptHandle->tGprsCfg.tOopMaster);

    if(NULL != ptHandle->pAppReStart)
    {
        *ptHandle->pAppReStart = 0;
    }

    *ptHandle->pRunFlag = 0;
    sprintf(ifName, "ppp%d", ifunit);

    /* ������վ */
    ptHandle->masterNUM = 0;
    if(MODE_ZB == ptPPPCfg->conmode)  //����ģʽ
    {
        if(ptHandle->masterStart >= ptMaster->nNum)
        {
            ptHandle->masterStart = 0;
        }
        for(i = ptHandle->masterStart; i < ptMaster->nNum; i++)
        {  //˳������
            *ptHandle->pRunFlag = 0;
            ptHandle->ip[0] = htonl(*((uint32 *)ptMaster->master[i].ip));
            ptHandle->port[0] = ptMaster->master[i].port;
            ptHandle->masterNUM++;
            comm_add_route_net(ifName, ptHandle->ip[0]);
            sleep(1);
            fdSocket = comm_connect_socket(ptHandle->ip[0], ptHandle->port[0]);
            if(fdSocket > 0)
            {
                ptHandle->connect_fd[0] = fdSocket;
                COMM_FMT_DEBUG("%s connect master 0x%x, port %d\n", ifName, ptHandle->ip[0], ptHandle->port[0]);
                break;
            }
            ptHandle->masterNUM--;
            comm_del_route_net(ifName, ptHandle->ip[0]);
        }
        if(i == ptMaster->nNum)
        {  //��ͷ��ʼ����
            for(i = 0; i < ptHandle->masterStart; i++)
            {
                *ptHandle->pRunFlag = 0;
                ptHandle->ip[0] = htonl(*((uint32 *)ptMaster->master[i].ip));
                ptHandle->port[0] = ptMaster->master[i].port;
                ptHandle->masterNUM++;
                comm_add_route_net(ifName, ptHandle->ip[0]);
                sleep(1);
                fdSocket = comm_connect_socket(ptHandle->ip[0], ptHandle->port[0]);
                if(fdSocket > 0)
                {
                    ptHandle->connect_fd[0] = fdSocket;
                    COMM_FMT_DEBUG("%s connect master 0x%x, port %d\n", ifName, ptHandle->ip[0], ptHandle->port[0]);
                    break;
                }
                ptHandle->masterNUM--;
                comm_del_route_net(ifName, ptHandle->ip[0]);    
            }
        }
    }
    else
    {
        for(i = 0; i < ptMaster->nNum; i++)
        {
            *ptHandle->pRunFlag = 0;
            ptHandle->ip[ptHandle->masterNUM] = htonl(*(uint32 *)ptMaster->master[i].ip);
            ptHandle->port[ptHandle->masterNUM] = ptMaster->master[i].port;
            comm_add_route_net(ifName, ptHandle->ip[ptHandle->masterNUM]);
            fdSocket = comm_connect_socket(ptHandle->ip[ptHandle->masterNUM], ptHandle->port[ptHandle->masterNUM]);
            usleep(100000);
            COMM_FMT_DEBUG("%s connect IP 0x%x, Port %d, fd %d\n", ifName, ptHandle->port[ptHandle->masterNUM],
                                                     ptHandle->port[ptHandle->masterNUM], fdSocket);
            if(fdSocket > 0)
            {
                ptHandle->connect_fd[ptHandle->masterNUM] = fdSocket;
            }
            ptHandle->connet_time[ptHandle->masterNUM] = (uint32)oldtime.tv_sec;
            ptHandle->masterNUM++;
        }
    }

    if(0 == ptHandle->masterNUM)
    {
        COMM_FMT_DEBUG("%s connect no master\n", ifName);
        return;
    }

    /* ֪ͨ��Ӧ�õ����߳�ͨ���� */
    dataLen = sizeof(COMM_IPCMSG_T) + sizeof(CHANNEL_INFO_T);
    ptIPCMsg = (COMM_IPCMSG_T *)dataBuf;
    ptIPCMsg->protocol = PROT_OPEN_CHANNLE;
    ptChanInfo = (CHANNEL_INFO_T *)ptIPCMsg->msg;
    ptChanInfo->workmode = MODE_CLIENT;
    ptChanInfo->medium = COMM_MEDIA_GPRS;
    ptChanInfo->heart = ptPPPCfg->heart;
    
    for(i = 0; i < ptHandle->masterNUM; i++)
    {
        if(ptHandle->connect_fd[i] > 0)
        {
            ptIPCMsg->id = (0 == ifunit ? CHL_GPRS_1 : CHL_GPRS_2) + i;
            ptChanInfo->ip = ptHandle->ip[i];
            ptChanInfo->port = ptHandle->port[i];
            comm_write_queue(ptHandle->ptRecvList, dataBuf, dataLen);
            COMM_FMT_DEBUG("%s open channel %d\n", ifName, ptIPCMsg->id);
        }
    } 
    
    clock_gettime(CLOCK_MONOTONIC, &oldtime);
    while(1)
    {
        *ptHandle->pRunFlag = 0;
        clock_gettime(CLOCK_MONOTONIC, &curtime);
        /*����*/
        if (curtime.tv_sec != oldtime.tv_sec)
        {
            memcpy(&oldtime, &curtime, sizeof(struct timespec));
            if(FALSE == ptHandle->bDiagOK || TRUE == *ptHandle->pAppReStart || TRUE == ptHandle->bClose 
               || FALSE == ptHandle->bGetDCM || FALSE == ptHandle->bGetCfg || 0 == ptMaster->nNum)
            {               
                COMM_FMT_DEBUG("%s no link or appmain restart or cfg change\n", ifName);
                statePPP = PPP_CLOSE_DEVICE;
            }
            if(TRUE == ptHandle->bChangeParam)
            {
                ptHandle->bChangeParam = FALSE;

                /* ���ñ仯���� */
                dataLen = sizeof(COMM_IPCMSG_T) + sizeof(CHANNEL_INFO_T);
                ptIPCMsg = (COMM_IPCMSG_T *)dataBuf;
                ptIPCMsg->protocol = PRO_PARA_MOD;
                ptChanInfo = (CHANNEL_INFO_T *)ptIPCMsg->msg;
                ptChanInfo->workmode = MODE_CLIENT;
                ptChanInfo->medium = COMM_MEDIA_GPRS;
                ptChanInfo->heart = ptPPPCfg->heart;
                
                for(i = 0; i < ptHandle->masterNUM; i++)
                {
                    if(ptHandle->connect_fd[i] > 0)
                    {
                        ptIPCMsg->id = (0 == ifunit ? CHL_GPRS_1 : CHL_GPRS_2) + i;
                        ptChanInfo->ip = ptHandle->ip[i];
                        ptChanInfo->port = ptHandle->port[i];
                        comm_write_queue(ptHandle->ptRecvList, dataBuf, dataLen);
                        COMM_FMT_DEBUG("%s change channel %d params\n", ifName, ptIPCMsg->id);
                    }
                } 
            }
        }
        
        switch(statePPP)
        {
            case PPP_CONNECT_MASTER:
            {
                for(i = 0; i < ptHandle->masterNUM; i++)
                {
                    if(ptHandle->connect_fd[i] > 0)
                    {
                        continue;
                    }
                    ptHandle->connect_fd[i] = comm_connect_socket(ptHandle->ip[i], ptHandle->port[i]);
                    if(ptHandle->connect_fd[i] > 0)
                    {
                        /* ֪ͨ��Ӧ�õ����߳�ͨ���� */
                        dataLen = sizeof(COMM_IPCMSG_T) + sizeof(CHANNEL_INFO_T);
                        ptIPCMsg = (COMM_IPCMSG_T *)dataBuf;
                        ptChanInfo = (CHANNEL_INFO_T *)ptIPCMsg->msg;
                        ptChanInfo->workmode = MODE_CLIENT;
                        ptChanInfo->medium = COMM_MEDIA_GPRS;
                        ptIPCMsg->id = (0 == ifunit ? CHL_ETH_1 : CHL_ETH_2) + i;
                        ptIPCMsg->protocol = PROT_OPEN_CHANNLE;
                        ptChanInfo->ip = ptHandle->ip[i];
                        ptChanInfo->port = ptHandle->port[i];
                        ptChanInfo->heart = ptPPPCfg->heart;
                        comm_write_queue(ptHandle->ptRecvList, dataBuf, dataLen);
                        COMM_FMT_DEBUG("%s open channel %d\n", ifName, ptIPCMsg->id);
                        conCnt[i] = 0;
                    }
                    else
                    {
                        conCnt[i] += 1;
                    }
                    usleep(10000);
                    ptHandle->connet_time[i] = curtime.tv_sec;
                }
                statePPP = PPP_CLOSE_DEVICE;
                for(i = 0; i < ptHandle->masterNUM; i++)
                {
                    if(ptHandle->connect_fd[i] > 0 || conCnt[i] < 5)
                    {
                        statePPP = PPP_SEND_DATA;
                    }
                }

                break;
            }
            case PPP_SEND_DATA:
            {
                applyTxFlow += comm_ppp_send(ifunit, ifName, ptHandle, &statePPP);
                break;
            }
            case PPP_RECV_DATA:
            {
                applyRxFlow += comm_ppp_recv(ifunit, ifName, ptHandle, &statePPP);
                break;
            }
            case PPP_CLOSE_DEVICE:
            {
                for(i = 0; i < ptHandle->masterNUM; i++)
                {
                    if(ptHandle->connect_fd[i] <= 0)
                    {
                        continue;
                    }
                    /* ֪ͨ��Ӧ�õ����߳�ͨ���ر� */
                    dataLen = sizeof(COMM_IPCMSG_T);
                    ptIPCMsg = (COMM_IPCMSG_T *)dataBuf;
                    ptIPCMsg->id = ((0 == ifunit) ? CHL_GPRS_1 : CHL_GPRS_2) + i;
                    ptIPCMsg->protocol = PROT_CLOSE_CHANNLE;
                    comm_write_queue(ptHandle->ptRecvList, dataBuf, dataLen);
                    COMM_FMT_DEBUG("%s close channel %d\n", ifName, ptIPCMsg->id);
                }
                ppp_close_allfd(ptHandle);
                return;
            }
            default:
            {
                statePPP = PPP_SEND_DATA;
                break;
            }
        }
        usleep(10000);
    }
    COMM_FMT_DEBUG("%s Send %lluB, Recv %lluB\n", ifName, applyTxFlow, applyRxFlow);
    return;
}

/**********************************************************************
* @name      : comm_ppp_udpwork
* @brief     ��udp��ʽ
* @param[in] ��COMM_PPP_HANDLE_T *ptHandle
* @param[out]��
* @return    ��
* @Create    : ����
* @Date      ��2020-8-29
* @Update    : 
**********************************************************************/
void comm_ppp_udpwork(COMM_PPP_HANDLE_T *ptHandle)
{
    uint8  i;    
    uint8  stateEth = PPP_SEND_DATA; /* ״̬�� */
    struct timespec curtime = {0};
    struct timespec oldtime = {0};
    int         fdSocket = -1;
    uint8       dataBuf[200] = {0};  /* ������ */
    uint16      dataLen = 0;
    CHANNEL_INFO_T *ptChanInfo = 0;
    COMM_IPCMSG_T  *ptIPCMsg = NULL;
    char            ifName[24] = {0};
    uint8           ifunit = ptHandle->no;

    uint32 applyRxFlow = 0;   /* Ӧ�ò����� */
    uint32 applyTxFlow = 0;   /* Ӧ�ò����� */
    int    sendRecvLen = 0;

    OOP_GPRSCONFIG_T *ptPPPCfg   = &(ptHandle->tGprsCfg.tOopGprs);
    OOP_MASTERPARAMS_T *ptMaster = &(ptHandle->tGprsCfg.tOopMaster);

    if(NULL != ptHandle->pAppReStart)
    {
        *ptHandle->pAppReStart = 0;
    }

    *ptHandle->pRunFlag = 0;
    sprintf(ifName, "ppp%d", ifunit);

    /* ��վ���� */
    ptHandle->masterNUM = 0;
    if(MODE_ZB == ptPPPCfg->conmode)  //����ģʽ
    {
        ptHandle->masterNUM = 1;
        for(i = 0; i < ptMaster->nNum; i++)
        {
            *ptHandle->pRunFlag = 0;
            ptHandle->ip[0] = ntohl(*((uint32 *)ptMaster->master[i].ip));
            ptHandle->port[0] = ptMaster->master[i].port;
            comm_add_route_net(ifName, ptHandle->ip[0]);
            
            fdSocket = comm_init_udp(ifName);
            if(fdSocket > 0)
            {
                COMM_FMT_DEBUG("%s connect master 0x%x, port %d\n", ifName, ptHandle->ip[0], ptHandle->port[0]);
                break;
            }
            usleep(100000);
            comm_del_route_net(ifName, ptHandle->ip[0]);    
        }
    }
    else  //�����ӷ�ʽ
    {
        for(i = 0; i < ptMaster->nNum; i++)
        {
            ptHandle->masterNUM++;
            *ptHandle->pRunFlag = 0;
            ptHandle->ip[i] = ntohl(*((uint32 *)ptMaster->master[i].ip));
            ptHandle->port[i] = ptMaster->master[i].port;
            comm_add_route_net(ifName, ptHandle->ip[i]);
        }
        fdSocket = comm_init_udp(ifName);
        usleep(100000);
    }

    if(fdSocket <= 0)
    {
        COMM_FMT_DEBUG("%s Create socekt failed\n", ifName);
        return;
    }
    ptHandle->connect_fd[0] = fdSocket;

    /* ֪ͨ��Ӧ�õ����߳�ͨ���� */
    dataLen = sizeof(COMM_IPCMSG_T) + sizeof(CHANNEL_INFO_T);
    ptIPCMsg = (COMM_IPCMSG_T *)dataBuf;
    ptIPCMsg->protocol = PROT_OPEN_CHANNLE;
    ptChanInfo = (CHANNEL_INFO_T *)ptIPCMsg->msg;
    ptChanInfo->workmode = MODE_CLIENT;
    ptChanInfo->medium = COMM_MEDIA_GPRS;
    ptChanInfo->heart = ptPPPCfg->heart;
    
    for(i = 0; i < ptHandle->masterNUM; i++)
    {
        if(ptHandle->connect_fd[i] > 0)
        {
            ptIPCMsg->id = (0 == ifunit ? CHL_GPRS_1 : CHL_GPRS_2) + i;
            ptChanInfo->ip = ptHandle->ip[i];
            ptChanInfo->port = ptHandle->port[i];
            comm_write_queue(ptHandle->ptRecvList, dataBuf, dataLen);
            COMM_FMT_DEBUG("%s open channel %d\n", ifName, ptIPCMsg->id);
        }
    } 

    clock_gettime(CLOCK_MONOTONIC, &oldtime);
    while(1)
    {
        *ptHandle->pRunFlag = 0;

        /*����*/
        if (curtime.tv_sec != oldtime.tv_sec)
        {
            memcpy(&oldtime, &curtime, sizeof(struct timespec));
            if(FALSE == ptHandle->bDiagOK || TRUE == *ptHandle->pAppReStart || TRUE == ptHandle->bClose 
               || FALSE == ptHandle->bGetDCM || FALSE == ptHandle->bGetCfg || 0 == ptMaster->nNum)
            {               
                COMM_FMT_DEBUG("%s no link or appmain restart or cfg change\n", ifName);
                stateEth = PPP_CLOSE_DEVICE;
            }
            if(TRUE == ptHandle->bChangeParam)
            {
                ptHandle->bChangeParam = FALSE;

                /* ���ñ仯���� */
                dataLen = sizeof(COMM_IPCMSG_T) + sizeof(CHANNEL_INFO_T);
                ptIPCMsg = (COMM_IPCMSG_T *)dataBuf;
                ptIPCMsg->protocol = PRO_PARA_MOD;
                ptChanInfo = (CHANNEL_INFO_T *)ptIPCMsg->msg;
                ptChanInfo->workmode = MODE_CLIENT;
                ptChanInfo->medium = COMM_MEDIA_GPRS;
                ptChanInfo->heart = ptPPPCfg->heart;
                
                for(i = 0; i < ptHandle->masterNUM; i++)
                {
                    ptIPCMsg->id = (0 == ifunit ? CHL_GPRS_1 : CHL_GPRS_2) + i;
                    ptChanInfo->ip = ptHandle->ip[i];
                    ptChanInfo->port = ptHandle->port[i];
                    comm_write_queue(ptHandle->ptRecvList, dataBuf, dataLen);
                    COMM_FMT_DEBUG("%s change channel %d params\n", ifName, ptIPCMsg->id);
                } 
            }
        }

        switch(stateEth)
        {
            case PPP_SEND_DATA:
            {
                dataLen = (uint16)comm_read_queue(ptHandle->ptSendList, g_PPPdataBuf[ifunit]);
                if(dataLen > 0)
                {
                    sendRecvLen = comm_ppp_udpsend(fdSocket, g_PPPdataBuf[ifunit], dataLen, ptHandle);
                    COMM_BUF_DEBUG(dataBuf, dataLen, "%s send(%d):", ifName, sendRecvLen);
                    if(sendRecvLen <0)
                    {
                        stateEth = PPP_CLOSE_DEVICE;
                        break;
                    }
                    applyTxFlow += sendRecvLen;
                    memset(dataBuf, 0, BUF_LENGTH);
                }
                stateEth = PPP_RECV_DATA;
                break;
            }
            case PPP_RECV_DATA:
            {
                sendRecvLen = comm_ppp_udprecv(fdSocket, g_PPPdataBuf[ifunit], BUF_LENGTH, ptHandle, &i);
                if(sendRecvLen > 0)  //д����ܶ���
                {
                    dataLen = (uint16)sendRecvLen;
                    applyRxFlow += dataLen;
                    /*�����ݼ��ϻỰЭ��ͷ��*/
                    comm_add_channel(((0 == ifunit) ? CHL_GPRS_1 : CHL_GPRS_2) + i, dataBuf, dataLen);
                    dataLen += 4;
                    comm_write_queue(ptHandle->ptRecvList, dataBuf, dataLen);
                    COMM_BUF_DEBUG(dataBuf, dataLen, "%s recv:", ifName);
                    memset(dataBuf, 0, BUF_LENGTH);
                }
                stateEth = PPP_SEND_DATA;
                break;
            }
            case PPP_CLOSE_DEVICE:
            {
                dataLen = sizeof(COMM_IPCMSG_T);
                ptIPCMsg = (COMM_IPCMSG_T *)dataBuf;
                ptIPCMsg->protocol = PROT_CLOSE_CHANNLE;
                for(i = 0; i < ptHandle->masterNUM; i++)
                {
                    ptIPCMsg->id = (0 == ifunit ? CHL_GPRS_1 : CHL_GPRS_2) + i;
                    comm_write_queue(ptHandle->ptRecvList, dataBuf, dataLen);
                    COMM_FMT_DEBUG("%s change channel %d params\n", ifName, ptIPCMsg->id);
                    comm_del_route_net(ifName, ptHandle->ip[i]);  
                } 
                ppp_close_allfd(ptHandle);
                return;
            }
            default:
            {
                stateEth = PPP_SEND_DATA;
                break;
            }
        }
        usleep(10000);
    }

}

/**********************************************************************
* @name      : comm_ppp_entry
* @brief     ��ppp�˿���Ϣ�շ��߳�
* @param[in] ��void *arg
* @param[out]��
* @return    ��
* @Create    : ����
* @Date      ��2019-10-31
* @Update    :
**********************************************************************/
void *comm_ppp_entry(void *pArg)
{
    COMM_PPP_HANDLE_T *ptHandle = (COMM_PPP_HANDLE_T *)pArg;
    COMM_PPPCFG_T     *ptPPPCfg = NULL;

    if(NULL == pArg)
    {
        return 0;
    }

    /* ���ͷ��ϴ�û�������ͷŵľ�� */
    ppp_close_allfd(ptHandle);

    ptPPPCfg = &ptHandle->tGprsCfg;
    if(TYPE_UDP == ptPPPCfg->tOopGprs.contype)
    {
        comm_ppp_udpwork(ptHandle);
    }
    else if(TYPE_TCP == ptPPPCfg->tOopGprs.contype)
    {
        comm_ppp_tcpwork(ptHandle);
    }
    
    return 0;
}

