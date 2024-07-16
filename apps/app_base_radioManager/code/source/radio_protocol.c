/*
*********************************************************************
* Copyright(C) 2021 �Ͼ��������ӹɷ����޹�˾
* All rights reserved. 
* @brief��   ��̨APP ������̨˽��Э�����
* @date��    2021-8-13
* @history�� 
*********************************************************************
*/
#include "baseCommon.h"
#include "framecomm.h"
#include "radio_data.h"
#include "radio_protocol.h"
#include "radio_uart.h"

#ifdef PRODUCT_CCU
#define RADIO_OPERATION_MAXTIME_MS  2000
#else
#define RADIO_OPERATION_MAXTIME_MS  800
#endif

/**********************************************************************
* @name      : radio_protocol_msg_ckeck
* @brief     ����̨Э�鱨�ĺϷ��Լ��(��68H��ʼ���)
* @param[in] ��uint8 *msg                ��������
               uint16 msglen             ���ĳ���
* @param[out]��
* @return    ��0-�ɹ�/������
* @Create    : ����
* @Date      ��2021-8-13
* @Update    :
**********************************************************************/
int radio_protocol_msg_ckeck(uint8 *msg, uint16 msglen)
{
    uint16 i = 0;
    uint16 len = 0;
    uint8  temp = 0;
    
    if(NULL == msg || msglen < RADIO_PROTOCOL_LEN_MIN)
    {
        return -1;
    }

    /* �����βֵ */
    if(0x68 != msg[0] || 0x16 != msg[msglen - 1])
    {
        return -2;
    }
    
    /* ��������򳤶��Ƿ�ƥ�� */
    memcpy(&len, msg + 1, 2);
    if(RADIO_PROTOCOL_LEN_MIN + len != msglen)
    {
        return -3;
    }

    /* ���֡��ʼ�� */
    if(0x68 != msg[RADIO_PROTOCOL_DATA_START])
    {
        return -4;
    }

    /* ���У�� */
    len = msglen - 2;
    for(i = 0; i < len; i++)
    {
        temp += msg[i];
    }
    if(temp != msg[len])
    {
        return -5;
    }
    
    return 0;
}

/**********************************************************************
* @name      : radio_protocol_split
* @brief     ����̨Э�鱨�Ľ���(��ʼ��68��ʼ)
* @param[in] ��uint8 *msg                ��������
               uint16 msglen             ���ĳ���
* @param[out]��RADIO_PRODATA_T **ppProData ���Ľ����ṹ��
* @return    ��0-�ɹ�/������
* @Create    : ����
* @Date      ��2021-8-13
* @Update    :
**********************************************************************/
int radio_protocol_split(uint8 *msg, uint16 msglen, RADIO_PRODATA_T **ppProData)
{
    int ret = 0;
    uint16 datalen = 0;
    uint8  temp = 0;
    RADIO_PRODATA_T *ptProData = NULL;

    if(NULL == ppProData)
    {
        return -1;
    }

    /* ��� */
    ret = radio_protocol_msg_ckeck(msg, msglen);
    if(0 != ret)
    {
        RADIO_BUF_DEBUG(msg, msglen, "��ʽ������");
        return ret;
    }
    
    /* ���ȼ��� */
    memcpy(&datalen, msg + 1, 2);

    /* �����ڴ� */
    ptProData = (RADIO_PRODATA_T*)malloc(datalen + sizeof(RADIO_PRODATA_T));
    if(NULL == ptProData)
    {
        return 1;
    }
    memset(ptProData, 0, datalen + sizeof(RADIO_PRODATA_T));
    ptProData->datalen = datalen;

    /* �������� */
    temp = msg[3];
    ptProData->type = (temp & 0x07);
    ptProData->bStatus = ((temp >> 6) & 0x01);
    ptProData->bAck = ((temp >> 7) & 0x01);

    /* �������� */
    memcpy(&(ptProData->cmd), msg + 4, 4);

    /* �����豸��ַ */
    MEMCPY_R(ptProData->mac, msg + 8, 6);
    
    /* �������� */
    memcpy(ptProData->data, msg + RADIO_PROTOCOL_DATA_START + 1, datalen);

    *ppProData = ptProData;
    return 0;
}

/**********************************************************************
* @name      : radio_protocol_package
* @brief     ����̨Э�鱨�ķ�װ(ǰ�ĸ��ֽ���FEǰ����)
* @param[in] ��uint8 frametype           ֡����
               uint32 cmd                ����
               uint8 *mac                Ŀ�ĵ�ַ
               uint8 *pData              ��������
               uint16 datalen            ���ݳ���
* @param[out]��uint8 **ppFrame           ��������
* @return    ���������ݳ���
* @Create    : ����
* @Date      ��2021-8-14
* @Update    :
**********************************************************************/
uint16 radio_protocol_package(uint8 frametype, uint32 cmd, uint8 *mac, uint8 *pData, uint16 datalen, uint8 **ppFrame)
{
    uint16 len = 0;
    uint16 offset = 0;
    uint16 i = 0;
    uint8  temp = 0;
    uint8 *pMsg = 0;
    
    *ppFrame = NULL;
    /* �������ݴ�С10K�� */
    if((NULL == pData && 0 != datalen) || NULL == ppFrame || datalen > 1024 * 10)
    {
        return 0;
    }

    len = RADIO_PROTOCOL_LEN_MIN + datalen + 4;
    pMsg = (uint8*)malloc(len);
    if(NULL == pMsg)
    {
        return 0;
    }

    pMsg[offset++] = 0xFE;
    pMsg[offset++] = 0xFE;
    pMsg[offset++] = 0xFE;
    pMsg[offset++] = 0xFE;

    pMsg[offset++] = 0x68;
    
    memcpy(pMsg + offset, &datalen, 2);
    offset += 2;
    
    pMsg[offset++] = frametype;
    
    memcpy(pMsg + offset, &cmd, 4);
    offset += 4;
    
    MEMCPY_R(pMsg + offset, mac, 6);
    offset += 6;
    
    memset(pMsg + offset, 0, 4);
    offset += 4;
    
    pMsg[offset++] = 0x68;

    if(datalen > 0)
    {
        memcpy(pMsg + offset, pData, datalen);
        offset += datalen;
    }
    
    for(i = 4; i < offset; i++)
    {
        temp += pMsg[i];
    }
    pMsg[offset++] = temp;
    pMsg[offset++] = 0x16;

    *ppFrame = pMsg;
    return offset;
}

/**********************************************************************
* @name      : radio_set_oper
* @brief     ����̨���ò���
* @param[in] ��int fd                    �豸���
               uint32 cmd                ����
               uint8 *pSend              ��������
               uint16 sendlen            ���ͳ���
               uint16 buflen             ���ջ�������
* @param[out]��RADIO_PRODATA_T **pProRet   ���ý��
               uint8 *recvbuf            ���ս��(�����ϱ���͸����Ϣ)
* @return    ��>=0 recvbuf��������; < 0 ������
* @Create    : ����
* @Date      ��2021-8-14
* @Update    :
**********************************************************************/
int radio_set_operate_one(int fd, uint32 cmd, uint8 *pSend, uint16 sendlen, RADIO_PRODATA_T **pProRet,
                       uint8 *recvbuf, uint16 buflen)
{
    int    ret = 0;
    uint16 msglen = 0;
    uint8 *pMsg = NULL;
    uint8  MAC[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    uint8  buf[10240] = {0};  //10K�����ڲ����ջ���
    int    retlen = 0;
    int    start = 0;
    uint16 offset = 0;
    RADIO_PRODATA_T *pProData = NULL;
    
    *pProRet = NULL;
    
    if(fd < 0 || 0 == cmd || NULL == pSend || 0 == sendlen || NULL == pProRet || NULL == recvbuf || 0 == buflen)
    {
        RADIO_FMT_DEBUG("��̨������δ���, cmd 0x%08X\n", cmd);
        return 0;
    }

    msglen = radio_protocol_package(RADIO_PRO_SET_BT, cmd, MAC, pSend, sendlen, &pMsg);
    if(NULL == pMsg)
    {
        RADIO_BUF_DEBUG(pSend, sendlen, "������֡ʧ��, cmd 0x%08X", cmd);
        return 0;
    }
    
    /* ������Ϣ */
    ret = radio_uart_send(fd, pMsg, msglen);
    RADIO_BUF_DEBUG(pMsg, msglen, "���ڷ���(cmd = 0x%08X)%s", cmd, 0 == ret ? "�ɹ�" : "ʧ��");
    free(pMsg);
    if(0 != ret)
    {
        return -1;
    }

    /* ���ڽ��� */
    retlen = radio_uart_recv(fd, buf, 10240, RADIO_OPERATION_MAXTIME_MS, 200);
    if(retlen <= 0)
    {
        RADIO_FMT_DEBUG("���ճ�ʱ\n", cmd);
        return 0;
    }
    RADIO_BUF_DEBUG(buf, retlen, "���ڽ���");

    pMsg = recvbuf;
    /* �жϽ������� */
    while(start < retlen)
    {
        if(0x68 == buf[start])
        {
            memcpy(&msglen, buf + start + 1, 2);
            if(start + RADIO_PROTOCOL_LEN_MIN + msglen <= retlen)
            {
                ret = radio_protocol_split(buf + start, RADIO_PROTOCOL_LEN_MIN + msglen, &pProData);
                if (NULL != pProData)
                {
                    if(cmd == pProData->cmd && RADIO_PRO_SET_BT == pProData->type &&
                       1 == pProData->bAck && 1 == pProData->bStatus)
                    {
                        break;
                    }
                    else
                    {
                        free(pProData);
                        pProData = NULL;
                    }
                }
            }
        }
        
        /* ���� */
        if(offset < buflen)
        {
            recvbuf[offset++] = buf[start];
        }
        start++;
    }

    if (NULL == pProData)
    {
        RADIO_FMT_DEBUG("���Ľ������󣬴�����[%d]\n", ret);
        return -2;
    }

    *pProRet = pProData;

    /* ʣ�����ݿ��� */
    if(pProData)
    {
        start += RADIO_PROTOCOL_LEN_MIN + pProData->datalen;
        if(start < retlen && offset < buflen)
        {
            msglen = MIN(buflen - offset, retlen - start);
            memcpy(recvbuf + offset, buf + start, msglen);
            offset += msglen;
        }
    }
    return offset;
}

/**********************************************************************
* @name      : radio_set_oper
* @brief     ����̨���ò���
* @param[in] ��int fd                    �豸���
               uint32 cmd                ����
               uint8 *pSend              ��������
               uint16 sendlen            ���ͳ���
               uint16 buflen             ���ջ�������
* @param[out]��RADIO_PRODATA_T **pProRet   ���ý��
               uint8 *recvbuf            ���ս��(�����ϱ���͸����Ϣ)
* @return    ��>=0 recvbuf��������; < 0 ������
* @Create    : ����
* @Date      ��2021-8-14
* @Update    :
**********************************************************************/
int radio_set_operate(int fd, uint32 cmd, uint8 *pSend, uint16 sendlen, RADIO_PRODATA_T **pProRet,
                       uint8 *recvbuf, uint16 buflen)
{
    uint32 i = 0;
    int    len = -1;

    while (i < 2)
    {
        len = radio_set_operate_one(fd, cmd, pSend, sendlen, pProRet, recvbuf, buflen);
        if (len > 0)
        {
            break;
        }
        else
        {
            i++;
            usleep(10000);
        }
    }

    return len;
}

/**********************************************************************
* @name      : radio_get_operate
* @brief     ����̨���ò���
* @param[in] ��int fd                    �豸���
               uint32 cmd                ����
               uint8 *pSend              ��������
               uint16 sendlen            ���ͳ���
               uint16 buflen             ���ջ�������
* @param[out]��RADIO_PRODATA_T **pProRet   ���ý��
               uint8 *recvbuf            ���ս��(�����ϱ���͸����Ϣ)
* @return    ��>=0 recvbuf��������; < 0 ������
* @Create    : ����
* @Date      ��2021-8-14
* @Update    :
**********************************************************************/
int radio_get_operate_one(int fd, uint32 cmd, uint8 *pSend, uint16 sendlen, RADIO_PRODATA_T **pProRet, 
                       uint8 *recvbuf, uint16 buflen)
{
    int    ret = 0;
    uint16 msglen = 0;
    uint8 *pMsg = NULL;
    uint8  MAC[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    uint8  buf[10240] = {0};  //10K�����ڲ����ջ���
    int    retlen = 0;
    int    start = 0;
    uint16 offset = 0;
    RADIO_PRODATA_T *pProData = NULL;
    
    *pProRet = NULL;
    
    if(fd < 0 || 0 == cmd || (NULL == pSend && 0 != sendlen)  || NULL == pProRet || NULL == recvbuf || 0 == buflen)
    {
        RADIO_FMT_DEBUG("��̨��ȡ��δ���, cmd 0x%08X\n", cmd);
        return 0;
    }

    msglen = radio_protocol_package(RADIO_PRO_GET_BT, cmd, MAC, pSend, sendlen, &pMsg);
    if(NULL == pMsg)
    {
        RADIO_BUF_DEBUG(pSend, sendlen, "������֡ʧ��, cmd 0x%08X", cmd);
        return 0;
    }
    
    /* ������Ϣ */
    ret = radio_uart_send(fd, pMsg, msglen);
    RADIO_BUF_DEBUG(pMsg, msglen, "���ڷ���(cmd = 0x%08X)%s", cmd, 0 == ret ? "�ɹ�" : "ʧ��");
    free(pMsg);
    if(0 != ret)
    {
        return -1;
    }

    /* ���ڽ��� */
    retlen = radio_uart_recv(fd, buf, 10240, RADIO_OPERATION_MAXTIME_MS, 200);
    if(retlen <= 0)
    {
        RADIO_FMT_DEBUG("���ճ�ʱ\n", cmd);
        return 0;
    }
    RADIO_BUF_DEBUG(buf, retlen, "���ڽ���");

    pMsg = recvbuf;
    /* �жϽ������� */
    while(start < retlen)
    {
        if(0x68 == buf[start])
        {
            memcpy(&msglen, buf + start + 1, 2);
            if(start + RADIO_PROTOCOL_LEN_MIN + msglen <= retlen)
            {
                ret = radio_protocol_split(buf + start, RADIO_PROTOCOL_LEN_MIN + msglen, &pProData);
                if(NULL != pProData)
                {
                    if(cmd == pProData->cmd && RADIO_PRO_GET_BT == pProData->type && 1 == pProData->bAck)
                    {
                        break;
                    }
                    else
                    {
                        free(pProData);
                        pProData = NULL;
                    }
                }
            }
        }
        
        /* ���� */
        if(offset < buflen)
        {
            recvbuf[offset++] = buf[start];
        }
        start++;
    }
    
    if (NULL == pProData)
    {
        RADIO_FMT_DEBUG("���Ľ������󣬴�����[%d]\n", ret);
        return -2;
    }

    *pProRet = pProData;

    /* ʣ�����ݿ��� */
    if(pProData)
    {
        start += RADIO_PROTOCOL_LEN_MIN + pProData->datalen;
        if(start < retlen && offset < buflen)
        {
            msglen = MIN(buflen - offset, retlen - start);
            memcpy(recvbuf + offset, buf + start, msglen);
            offset += msglen;
        }
    }
    return offset;
}

/**********************************************************************
* @name      : radio_get_operate
* @brief     ����̨���ò���
* @param[in] ��int fd                    �豸���
               uint32 cmd                ����
               uint8 *pSend              ��������
               uint16 sendlen            ���ͳ���
               uint16 buflen             ���ջ�������
* @param[out]��RADIO_PRODATA_T **pProRet   ���ý��
               uint8 *recvbuf            ���ս��(�����ϱ���͸����Ϣ)
* @return    ��>=0 recvbuf��������; < 0 ������
* @Create    : ����
* @Date      ��2021-8-14
* @Update    :
**********************************************************************/
int radio_get_operate(int fd, uint32 cmd, uint8 *pSend, uint16 sendlen, RADIO_PRODATA_T **pProRet, 
                       uint8 *recvbuf, uint16 buflen)
{
    uint32 i = 0;
    int    len = -1;

    while (i < 2)
    {
        len = radio_get_operate_one(fd, cmd, pSend, sendlen, pProRet, recvbuf, buflen);
        if (len > 0)
        {
            break;
        }
        else
        {
            i++;
            usleep(10000);
        }
    }

    return len;
}

/**********************************************************************
* @name      : radio_tran_send
* @brief     ����̨͸����Ϣ
* @param[in] ��int fd                    �豸���
               uint8 *MAC                �Զ˵�ַ
               uint8 *pSend              ��������
               uint16 sendlen            ���ͳ���
* @param[out]��
* @return    ��0-�ɹ�/������
* @Create    : ����
* @Date      ��2021-8-14
* @Update    :
**********************************************************************/
int radio_tran_send(int fd, uint8 *MAC, uint8 *pSend, uint16 sendlen)
{
    int    ret = 0;
    uint16 msglen = 0;
    uint8 *pMsg = NULL;
    
    if(fd < 0 || 0 == MAC || NULL == pSend || 0 == sendlen)
    {
        RADIO_FMT_DEBUG("��̨͸����δ���\n");
        return 0;
    }

    msglen = radio_protocol_package(RADIO_PRO_TO_OUT, 0, MAC, pSend, sendlen, &pMsg);
    if(NULL == pMsg)
    {
        RADIO_BUF_DEBUG(pSend, sendlen, "������֡ʧ��, ͸��");
        return 0;
    }
    
    /* ������Ϣ */
    ret = radio_uart_send(fd, pMsg, msglen);
    RADIO_BUF_DEBUG(pMsg, msglen, "���ڷ���(͸��%02X:%02X:%02X:%02X:%02X:%02X)%s", MAC[0], MAC[1], MAC[2], MAC[3], MAC[4],
                                MAC[5], 0 == ret ? "�ɹ�" : "ʧ��");
    free(pMsg);
    return ret;
}

/**********************************************************************
* @name      : radio_report_response
* @brief     ����̨��Ӧ�����ϱ�
* @param[in] ��int fd                    �豸���
               uint32 cmd                ����
               uint8 *pSend              ��������
               uint16 sendlen            ���ͳ���
* @param[out]��
* @return    ��0-�ɹ�/������
* @Create    : ����
* @Date      ��2021-8-14
* @Update    :
**********************************************************************/
int radio_report_response(int fd, uint32 cmd, uint8 *pSend, uint16 sendlen)
{
    int    ret = 0;
    uint16 msglen = 0;
    uint8  type = RADIO_PRO_REPORT;
    uint8  MAC[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    uint8 *pMsg = NULL;
    
    if(fd < 0 || NULL == pSend || 0 == sendlen)
    {
        RADIO_FMT_DEBUG("��̨�ϱ�Ӧ����δ���\n");
        return 0;
    }

    msglen = radio_protocol_package(type + 0xC0, cmd, MAC, pSend, sendlen, &pMsg);
    if(NULL == pMsg)
    {
        RADIO_BUF_DEBUG(pSend, sendlen, "������֡ʧ��, �ϱ�Ӧ��");
        return 0;
    }
    
    /* ������Ϣ */
    ret = radio_uart_send(fd, pMsg, msglen);
    RADIO_BUF_DEBUG(pMsg, msglen, "���ڷ���(�ϱ�Ӧ��)%s", 0 == ret ? "�ɹ�" : "ʧ��");
    free(pMsg);
    return ret;

}


