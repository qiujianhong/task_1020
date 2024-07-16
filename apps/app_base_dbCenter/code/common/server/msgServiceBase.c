/*
*********************************************************************
* Copyright(C) 2019 �Ͼ��������ӹɷ����޹�˾
* All rights reserved.
* @brief�� ��Ϣ����ӿ�
* @date�� 2019-11-9
*********************************************************************
*/

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "baseConstant.h"
#include "baseType.h"
#include "msgServiceBase.h"
#include "zlog.h"

//��Ϣ����ӿ�
static sint32 (*queueMessageFp)(MSG_HANDER_OBJ_T *msgHanderObj);

/**
*********************************************************************
* @brief��                  ��Ϣ���к���
* @param[in]��
               msgHanderObj ��Ϣ��
* @param[out]��
               none
* @return�� -1 ����ʧ��
            0  ����ɹ�
*********************************************************************
*/
sint32 base_queue_message(MSG_HANDER_OBJ_T *msgHanderObj)
{
    if (NULL != queueMessageFp)
    {
        //��Ϣ����
        return queueMessageFp(msgHanderObj);
    }
    else
    {
        dzlog_fatal("queueMessageFp is NULL");
    }
    return -1;
}

/**
*********************************************************************
* @brief��                  ������Ϣ���лص�����
* @param[in]��
               fp           �ص�����ָ��
* @param[out]��
               none
* @return�� ����ֵ˵��
*********************************************************************
*/
void set_queue_message_fp(sint32 (*fp)(MSG_HANDER_OBJ_T *msgHanderObj))
{
    queueMessageFp = fp;
}
/*
#ifdef XL_TIME_TRACE_DUMP
void msg_life_cycle_dump(MSG_HANDER_OBJ_T *msgHanderObj)
{
    dzlog_debug("msg recvDone [%lu]:[%lu]", msgHanderObj->recvDone.tv_sec,
                msgHanderObj->recvDone.tv_usec);
    dzlog_debug("msg parseDone [%lu]:[%lu]", msgHanderObj->parseDone.tv_sec,
                msgHanderObj->parseDone.tv_usec);
    dzlog_debug("msg queueDone [%lu]:[%lu]", msgHanderObj->queueDone.tv_sec,
                msgHanderObj->queueDone.tv_usec);
    dzlog_debug("msg dequeueDone [%lu]:[%lu]", msgHanderObj->dequeueDone.tv_sec,
                msgHanderObj->dequeueDone.tv_usec);
    dzlog_debug("msg handerStart [%lu]:[%lu]", msgHanderObj->handerStart.tv_sec,
                msgHanderObj->handerStart.tv_usec);
    dzlog_debug("msg handerDone [%lu]:[%lu]", msgHanderObj->handerDone.tv_sec,
                msgHanderObj->handerDone.tv_usec);

    dzlog_debug("msg parseDone - recvDone [%lu]:[%lu]",
                msgHanderObj->parseDone.tv_sec - msgHanderObj->recvDone.tv_sec,
                msgHanderObj->parseDone.tv_usec -
                    msgHanderObj->recvDone.tv_usec);

    dzlog_debug("msg queueDone - parseDone [%lu]:[%lu]",
                msgHanderObj->queueDone.tv_sec - msgHanderObj->parseDone.tv_sec,
                msgHanderObj->queueDone.tv_usec -
                    msgHanderObj->parseDone.tv_usec);

    dzlog_debug(
        "msg dequeueDone - queueDone [%lu]:[%lu]",
        msgHanderObj->dequeueDone.tv_sec - msgHanderObj->queueDone.tv_sec,
        msgHanderObj->dequeueDone.tv_usec - msgHanderObj->queueDone.tv_usec);

    dzlog_debug(
        "msg handerStart - dequeueDone [%lu]:[%lu]",
        msgHanderObj->handerStart.tv_sec - msgHanderObj->dequeueDone.tv_sec,
        msgHanderObj->handerStart.tv_usec - msgHanderObj->dequeueDone.tv_usec);

    dzlog_debug(
        "msg handerDone - handerStart [%lu]:[%lu]",
        msgHanderObj->handerDone.tv_sec - msgHanderObj->handerStart.tv_sec,
        msgHanderObj->handerDone.tv_usec - msgHanderObj->handerStart.tv_usec);
}
#endif
*/
