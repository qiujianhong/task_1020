/*
*********************************************************************
* Copyright(C) 2019 �Ͼ��������ӹɷ����޹�˾
* All rights reserved. 
* @brief��   ��Ӧ�õ��Ƚ����ⲿͨ���߳�
* @date��    2019-12-3
* @history�� 
*********************************************************************
*/

#include "baseCommon.h"
#include "task_msg.h"
#include "task_id.h"
#include "msg_id.h"

#include "appoutmsg.h"
#include "framecomm.h"
#include "label_id.h"
#include "storage.h"

pthread_mutex_t  g_fGuiMsg;
int g_GuiFd = 0;
int g_GuiSync = 0;

ProxyLockQueue ProxyQueue;
ProxyLockQueue ProxyrespondeQueue;


/**********************************************************************
* @name      : app_msg_entry
* @brief     �������ڲ�ͨ���߳�
* @param[in] ��void *arg
* @param[out]��
* @return    ��
* @Create    : ����
* @Date      ��2019-12-5
* @Update    :
**********************************************************************/
void *app_msg_entry(void *pArg)
{
    OUTMSG_THREAD_T *ptMsgHandle = (OUTMSG_THREAD_T*)pArg;
    int              clientid = *ptMsgHandle->pClientId;
    uint8            recvBuf[TASK_MSG_MAX_LEN] = {0};
    TASK_MSG_t      *pMsg = (TASK_MSG_t *)recvBuf;
    int              recvlen = 0;
    uint32           sender = 0;
    //int              ret = 0;
    
	//uint8            PIID=0;
	//uint16           label = 0;
	//ProxyResponse ProxyResponsedata;
	//uint8           SecMod;
	//FrameHeadInfo_t FrameHeadInfo;		//֡ͷ��Ϣ
    
    if(g_GuiFd <= 0)
    {
        g_GuiFd = task_msg_init(GUI_MSG);
        if(g_GuiFd <= 0)
        {
            GUI_FMT_DEBUG("GUI task msg init failed!\n");
            return 0;
        }
    }
    task_msg_settime(g_GuiFd, 0, 10000);  //����������ʱʱ��Ϊ0.01s

    if(g_GuiSync <= 0)
    {
        g_GuiSync = task_msg_init(GUI_SYNCMSG);
        if(g_GuiSync <= 0)
        {
            GUI_FMT_DEBUG("GUI task msg init failed!\n");
            return 0;
        }
        pthread_mutex_init(&g_fGuiMsg, NULL);
    }
    
    GUI_FMT_TRACE("GUI task client id %d, guiFd = %d\n", clientid, g_GuiFd);

    pthread_mutex_trylock(&g_fGuiMsg);
    pthread_mutex_unlock(&g_fGuiMsg);
    while(1)
    {
    	//usleep(20000);
    	//Proc_Proxy(&FrameHeadInfo, label, &SecMod, &PIID);
        *ptMsgHandle->pRunFlag = 0;
        pthread_mutex_lock(&g_fGuiMsg);
        recvlen = task_recv_msg(g_GuiFd, recvBuf, TASK_MSG_MAX_LEN, &sender);
        pthread_mutex_unlock(&g_fGuiMsg);
        if(recvlen > 0)
        {
//            for(i = 0; i < g_nAppMsgNum; i++)
//            {
//                if(pMsg->msg_id == g_AppOutMsg[i].msgid && NULL != g_AppOutMsg[i].func)
//                {
//                    ret = g_AppOutMsg[i].func(clientid, pMsg->label, pMsg->msg, pMsg->msglen);
//                    DPRINT("AppMain proc msg 0x%x: label %d, ret %d\n", pMsg->msg_id, pMsg->label, ret);
//                }
//            }
			GUI_BUF_TRACE(pMsg->msg, pMsg->msglen,"app msg recv msgid 0x%x:", pMsg->msg_id);
//			switch(pMsg->msg_id)
//			{
//				case MSG_APPAMIN_CTS_READ_LIST:
//				{
//				    if(pMsg->label == LABEL_HALT_EVENT)
//                  {
//                        ret = Pro_RelayHalt(pMsg->msg, pMsg->msglen);
//                        if(0 != ret)
//                        {
//                            GUI_FMT_DEBUG("Relay Halt msg error, ret %d\n", ret);
//                        }
//                        continue;
//                    }
//					//ProxyResponsedata.ServerID=0x01;
//				}
//				break;
//				case MSG_APPAMIN_CTS_READ_ONE:
//				{
//					//ProxyResponsedata.ServerID=0x02;
//				}
//				break;
//				case MSG_APPAMIN_CTS_SET_LIST:
//				{
//					ProxyResponsedata.ServerID=0x03;EVENT_FMT_DEBUG
//				}
//				break;
//				case MSG_APPAMIN_CTS_SETREAD_LIST:
//				{
//				    RecvReadMeterStatus(pMsg->msg, pMsg->msglen, pMsg->label);
//					ProxyResponsedata.ServerID=0x04;
//                    continue;
//				}
//				break;
//				case MSG_APPAMIN_CTS_ACT_LIST:
//				{
//					ProxyResponsedata.ServerID=0x05;
//				}
//				break;
//				case MSG_APPAMIN_CTS_ACTREAD_LIST:
//				{
//					ProxyResponsedata.ServerID=0x06;
//				}
//				break;
//				case MSG_APPAMIN_CTS_TRANSPORT:
//				{
//					ProxyResponsedata.ServerID=0x07;
//				}
//				break;
//				case MSG_APPAMIN_CTS_CARRIRT_TRANS:
//				{
//					ProxyResponsedata.ServerID=0x08;
//				}
//				break;
//				default:
//					continue;
//				break;
//			}
			//ProxyResponsedata.FrameHeadInfo= FrameHeadInfo;
			//ProxyResponsedata.SecMod = (SecurityMode_t)SecMod;
			//ProxyResponsedata.PIID = PIID;
			//ProxyResponsedata.ResponseApduLen=pMsg->msglen;
			//memcpy(ProxyResponsedata.ResponseApdu,pMsg->msg,pMsg->msglen);
            GUI_BUF_TRACE(pMsg->msg,pMsg->msglen,"ԭʼ��Ϣ����");
			//ProxyWriteQueue(&ProxyrespondeQueue,(uint8*)&ProxyResponsedata,sizeof(ProxyResponse));

            memset(recvBuf, 0, recvlen);
        }

        /* ������ʱ���� */

        usleep(10);
    }
    return 0;
}

/**********************************************************************
* @name      : appmain_send_msg
* @brief     ????APPAMIN��??????????????
* @param[in] ????uint32 msgid    ????????ID
               uint16 label    ????????label
               uint8 *pMsg     ????????????????
               uint16 msglen   ??????????????????��????
* @param[out]????
* @return    ????0-????????/??��??��????
* @Create    : ??????��????
* @Date      ????2020-1-2
* @Update    :
**********************************************************************/
int guiapp_send_msg(uint32 msgid, uint16 label, uint8 *pMsg, uint16 msglen)
{
    int    ret = 0;
    ret = task_send_labelmsg(g_GuiFd, msgid, label, pMsg, msglen, GUI_MSG_MONITOR);
    if(0 == ret)
    {
        GUI_FMT_TRACE("Send to 0x%x: msg 0x%x, label %d, ret %d\n", GUI_MSG_MONITOR, msgid, label, ret);
    }
    else
    {
        GUI_FMT_DEBUG("Send to 0x%x: msg 0x%x, label %d, ret %d\n", GUI_MSG_MONITOR, msgid, label, ret);
    }
    return ret;
}



/**********************************************************************
* @name      : gui_send_syncmsg
* @brief     ��GUI����ͬ����Ϣ��Ϣ, Ŀǰ������Ϣ
* @param[in] ��uint32 msgid    ��ϢID
               uint8 *sendbuf  ��������
               uint16 sendlen  ���ͳ���
               uint16 buflen   ���ջ����С
* @param[out]��uint8 *recvbuf  ��������
               uint16 *recvLen ���ճ���
* @return    ��0-�ɹ�/������
* @Create    : ����
* @Date      ��2020-3-19
* @Update    :
**********************************************************************/
int gui_send_syncmsg(uint32 msgid, uint8 *sendbuf, uint16 sendlen, uint8 *recvbuf, uint16 buflen, uint16 *recvLen)
{
    int    ret = 0;
    uint8  recvBuf[TASK_MSG_MAX_LEN] = {0};
    TASK_MSG_t *pMsg = (TASK_MSG_t *)recvBuf;

    pthread_mutex_lock(&g_fGuiMsg);
	GUI_FMT_TRACE("Fd = %d, msgid = %d, sendlen = %d\n", g_GuiFd, msgid, sendlen);
    ret = task_syncmsg_safe(g_GuiSync, msgid, sendbuf, sendlen, GUI_MSG_MONITOR, recvBuf, TASK_MSG_MAX_LEN, 2);
	GUI_FMT_TRACE("ret = %d\n", ret);
    pthread_mutex_unlock(&g_fGuiMsg);

    if(0 == ret)
    {
        if(buflen < pMsg->msglen)
        {
            return -100;
        }
        memcpy(recvbuf, pMsg->msg, pMsg->msglen);
        *recvLen = pMsg->msglen;
    }

    return ret;
}


/**********************************************************
* �������ƣ�����һ��msg
* ����������
* ����������
*
* ����ֵ��
**********************************************************/
BOOL GenProxyMSG(Proxyresquest *pProxyTask,uint32 *msgid,uint8 *outdata,uint16 *outlen)
{
    //uint16 offSet = 0;
    //uint8 i=0,j=0,num=0;
	//OOP_TSA_T tsa;
	//BOOL result=FALSE;
	uint8 lenBuf[3] = {0}; //������������
	uint8 offlen = 0;      //������������
	
    if(0 == pProxyTask->RequestApduLen)
	{
		return FALSE;
	}
        

	switch(pProxyTask->nChioce)
	{
	case 1:
	case 2:
	case 3:
	case 4:
	case 5:
	case 6:
		{
//			uint8 oadnum=0;
			*msgid=MSG_APPAMIN_CTS_READ_ONE;
//			offSet=offSet+3;
//			num=pProxyTask->pRequestApdu[offSet++]
//			for(i=0;i<num;i++)
//			{
//				offSet++;
//				tsa.af=pProxyTask->pRequestApdu[offSet++];
//				memcpy(tsa.add,pProxyTask->pRequestApdu+offSet,tsa.af);
//				result=IsExitByTsa(&tsa);
//				if(FALSE==result)
//				{
//					pProxyTask->state = PROXYST_RELAY_SUCCESS;
//				}
//				offSet=offSet+tsa.af;
//				oadnum=pProxyTask->pRequestApdu[offSet++];
//				offSet=offSet+4*oadnum;
//			}
			offlen = set_len_offset(pProxyTask->RequestApduLen, lenBuf);
			memcpy(outdata, lenBuf, offlen);

			*outlen=pProxyTask->RequestApduLen+offlen;
			memcpy(outdata+offlen,pProxyTask->pRequestApdu,*outlen);
			return TRUE;
		}
	break;
//	case 2:
//		{
////			uint8 oadnum=0;
//			*msgid=MSG_APPAMIN_CTS_READ_ONE;
////			offSet=offSet+4;
////			tsa.af=pProxyTask->pRequestApdu[offSet++];
////			memcpy(tsa.add,pProxyTask->pRequestApdu+offSet,tsa.af);
////			result=IsExitByTsa(&tsa);
////			if(FALSE==result)
////			{
////				pProxyTask->state = PROXYST_RELAY_SUCCESS;
////			}
//			*outlen=pProxyTask->RequestApduLen-2;
//			memcpy(outdata,pProxyTask->pRequestApdu+2,*outlen);
//			return TRUE;
//		}
//	break;
//	case 3:
//		{
//			
//			*msgid=MSG_APPAMIN_CTS_SET_LIST;
////			offSet=offSet+4;
////			tsa.af=pProxyTask->pRequestApdu[offSet++];
////			memcpy(tsa.add,pProxyTask->pRequestApdu+offSet,tsa.af);
////			offSet=offSet+tsa.af;
////			result=IsExitByTsa(&tsa);
////			if(FALSE==result)
////			{
////				pProxyTask->state = PROXYST_RELAY_SUCCESS;
////			}
////			offSet=offSet+2;
////			num=pProxyTask->pRequestApdu[offSet++];
////			for(i=0;i<num;i++)
////			{
////				apdu_next_position_get(pProxyTask->pRequestApdu, &offSet, uint16 pProxyTask->RequestApduLen, DT_OAD, FALSE);
////				apdu_next_position_get(pProxyTask->pRequestApdu, &offSet, uint16 pProxyTask->RequestApduLen, DT_NULL, FALSE);
////			}
//			*outlen=pProxyTask->RequestApduLen;
//			memcpy(outdata,pProxyTask->pRequestApdu,*outlen);
//			return TRUE;
//		}
//	break;
//	case 4:
//		{
//			
//			*msgid=MSG_APPAMIN_CTS_SETREAD_LIST;
////			offSet=offSet+4;
////			tsa.af=pProxyTask->pRequestApdu[offSet++];
////			memcpy(tsa.add,pProxyTask->pRequestApdu+offSet,tsa.af);
////			offSet=offSet+tsa.af;
////			result=IsExitByTsa(&tsa);
////			if(FALSE==result)
////			{
////				pProxyTask->state = PROXYST_RELAY_SUCCESS;
////			}
////			offSet=offSet+2;
////			num=pProxyTask->pRequestApdu[offSet++];
////			for(i=0;i<num;i++)
////			{
////				apdu_next_position_get(pProxyTask->pRequestApdu, &offSet, uint16 pProxyTask->RequestApduLen, DT_OAD, FALSE);
////				apdu_next_position_get(pProxyTask->pRequestApdu, &offSet, uint16 pProxyTask->RequestApduLen, DT_NULL, FALSE);
////				apdu_next_position_get(pProxyTask->pRequestApdu, &offSet, uint16 pProxyTask->RequestApduLen, DT_OAD, FALSE);
////				apdu_next_position_get(pProxyTask->pRequestApdu, &offSet, uint16 pProxyTask->RequestApduLen, DT_UNSIGNED, FALSE);
////			}
////			
//			*outlen=pProxyTask->RequestApduLen;
//			memcpy(outdata,pProxyTask->pRequestApdu,*outlen);
//			return TRUE;
//		}
//	break;
//	case 5:
//		{
//			
//			*msgid=MSG_APPAMIN_CTS_ACT_LIST;
////			offSet=offSet+4;
////			tsa.af=pProxyTask->pRequestApdu[offSet++];
////			memcpy(tsa.add,pProxyTask->pRequestApdu+offSet,tsa.af);
////			offSet=offSet+tsa.af;
////			result=IsExitByTsa(&tsa);
////			if(FALSE==result)
////			{
////				pProxyTask->state = PROXYST_RELAY_SUCCESS;
////			}
////			offSet=offSet+2;
////			num=pProxyTask->pRequestApdu[offSet++];
////			for(i=0;i<num;i++)
////			{
////				apdu_next_position_get(pProxyTask->pRequestApdu, &offSet, uint16 pProxyTask->RequestApduLen, DT_OAD, FALSE);
////				apdu_next_position_get(pProxyTask->pRequestApdu, &offSet, uint16 pProxyTask->RequestApduLen, DT_NULL, FALSE);
////			}
//			*outlen=pProxyTask->RequestApduLen;
//			memcpy(outdata,pProxyTask->pRequestApdu,*outlen);
//			return TRUE;
//		}
//	break;
//	case 6:
//		{
//			
//			*msgid=MSG_APPAMIN_CTS_ACTREAD_LIST;
////			offSet=offSet+4;
////			tsa.af=pProxyTask->pRequestApdu[offSet++];
////			memcpy(tsa.add,pProxyTask->pRequestApdu+offSet,tsa.af);
////			offSet=offSet+tsa.af;
////			result=IsExitByTsa(&tsa);
////			if(FALSE==result)
////			{
////				pProxyTask->state = PROXYST_RELAY_SUCCESS;
////			}
////			offSet=offSet+2;
////			num=pProxyTask->pRequestApdu[offSet++];
////			for(i=0;i<num;i++)
////			{
////				apdu_next_position_get(pProxyTask->pRequestApdu, &offSet, uint16 pProxyTask->RequestApduLen, DT_OAD, FALSE);
////				apdu_next_position_get(pProxyTask->pRequestApdu, &offSet, uint16 pProxyTask->RequestApduLen, DT_NULL, FALSE);
////				apdu_next_position_get(pProxyTask->pRequestApdu, &offSet, uint16 pProxyTask->RequestApduLen, DT_OAD, FALSE);
////				apdu_next_position_get(pProxyTask->pRequestApdu, &offSet, uint16 pProxyTask->RequestApduLen, DT_UNSIGNED, FALSE);
////			}
//			*outlen=pProxyTask->RequestApduLen;
//			memcpy(outdata,pProxyTask->pRequestApdu,*outlen);
//			return TRUE;
//		}
//	break;
	case 7:
	{
		uint8 offset=0;
		OOP_COMDCB_T comm;
		
		*msgid=MSG_APPAMIN_CTS_READ_LIST;
		memcpy(outdata+offset,pProxyTask->pRequestApdu+3,4);
		offset=offset+4;
		comm.baud=pProxyTask->pRequestApdu[7];
		comm.parity=pProxyTask->pRequestApdu[8];
		comm.databits=pProxyTask->pRequestApdu[9];
		comm.stopbits=pProxyTask->pRequestApdu[10];
		comm.flowctrl=pProxyTask->pRequestApdu[11];
		outdata[offset++]=SetComDCB(comm);
		memcpy(outdata+offset,pProxyTask->pRequestApdu+12,pProxyTask->RequestApduLen-12);
		offset=offset+pProxyTask->RequestApduLen-12;
		*outlen=offset;
		
		
		return TRUE;
	}
	break;
//	case 8:
//	{
//		
//		*msgid=MSG_APPAMIN_CTS_CARRIRT_TRANS;
//
//		*outlen=pProxyTask->RequestApduLen;
//		memcpy(outdata,pProxyTask->pRequestApdu,*outlen);
//		return TRUE;
//	}
//	break;
	default:


	return FALSE;
   	}
}



/**********************************************************
* �������ƣ�����������
* ����������
* ����������
*
* ����ֵ��  Ӧ������
**********************************************************/
int Proc_Proxy(FrameHeadInfo_t *FrameHeadInfo,uint16 label, uint8 *SecMod, uint8 *PIID)
{
	uint8 msgdata[1400];
	uint16 msglen=0;
//	uint16 label=0;
	uint32 msgid=0;
	Proxyresquest pProxyTaskLink;
	uint16 datalen=0;
	BOOL rslt=FALSE;

   	datalen=ProxyReadQueue(&ProxyQueue,(uint8*)&pProxyTaskLink);
	if(datalen<=0)
	{
		return 0;
	}
	GUI_BUF_TRACE((uint8*)&pProxyTaskLink,sizeof(Proxyresquest),"Proc_Proxy");
	*FrameHeadInfo=pProxyTaskLink.FrameHeadInfo;
	*SecMod=pProxyTaskLink.SecMod;
	*PIID= pProxyTaskLink.PIID;
//    switch(pProxyTaskLink->state)
//    {
//	    case PROXYST_RELAY_NEW://����һ���м�
//	    {
//	        BOOL rslt;
//
//	        if (pProxyTaskLink->ServerID == SERVERID_ACTIONREQUEST)
//	        {
//	//            rslt = GenF209ProxyIPC(pProxyTaskHead);
//	        }
//	        else
//	        {
//	            rslt=GenProxyMSG(&pProxyTaskLink,&msgid,msgdata,&msglen);
//	        }
//
//	        if(rslt==FALSE)//�������֮���ͷŴ���
//	        {
//	            pProxyTaskLink->state = PROXYST_RELEASE;
//	        }
//	        else
//	        {
//	        	appmain_send_msg(msgid,label,msgdata,msglen);
//	        }
//	    }
//		break;
//		case PROXYST_RELAY_DEAL://����һ���м�
//		{
//			 time_t secTime;
//	        secTime = time(NULL);
//	        
//	        if ((secTime < pProxyTaskLink->SendTime) || (secTime -pProxyTaskLink->SendTime > (pProxyTaskLink->TimeOut+15)))  //������ʱ�¼� ���� ��ʱ
//	        {
//	            pProxyTaskLink->state = PROXYST_OVERTIME;
//	        }
//	        if ((secTime < pProxyTaskLink->RelayStartTime) )  //������ʱ�¼� ���� ��ʱ
//	        {
//	            pProxyTaskLink->state = PROXYST_RELAY_OVERTIMR;
//	        }
//		}
//		break;
//    }
	GUI_FMT_TRACE("Proc_Proxy1111\n");

	rslt = GenProxyMSG(&pProxyTaskLink,&msgid,msgdata,&msglen);
    if(rslt==FALSE)//�������֮���ͷŴ���
    {
//        pProxyTaskLink->state = PROXYST_RELEASE;
    }
    else
    {
    	GUI_FMT_TRACE("msgid is %x\n",msgid);
    	GUI_BUF_TRACE(msgdata,msglen,"msgdata���");
    	guiapp_send_msg(msgid, label, msgdata, msglen);
    }
    return 0;
}

//͸��ת��һ���ֽڵ�ͨѶ��ת��ΪOOP_COMDCB_T
uint8 SetComDCB(OOP_COMDCB_T comm)
{
    //uint8 commbyte=0;
	uint8	baud = 0;			/**< ������				*/
	uint8	parity = 0;		/**< У��λ				*/
	uint8	databits = 0;		/**< ����λ				*/
	uint8	stopbits = 0;		/**< ֹͣλ				*/
	//uint8	flowctrl;		/**< ����				*/

	switch(comm.baud)
	{
		case 0:
			baud=0x00;
			break;
		case 1:
			baud=0x20;
			break;
		case 2:
			baud=0x40;
			break;
		case 3:
			baud=0x60;
			break;
		case 4:
			baud=0x80;
			break;
		case 5:
			baud=0xA0;
			break;
		case 6:
			baud=0xC0;
			break;
		case 7:
			baud=0xE0;
			break;
		default:
			break;

	}
	switch(comm.parity)
	{
		case 0:
			parity=0x00;
			break;
		case 1:
			parity=0x06;
			break;
		case 2:
			parity=0x04;
			break;
		default:
			break;

	}
	switch(comm.databits)
	{
		case 5:
			databits=0x00;
			break;
		case 6:
			databits=0x01;
			break;
		case 7:
			databits=0x02;
			break;
		case 8:
			databits=0x03;
			break;
		default:
			break;

	}
	switch(comm.stopbits)
	{
		case 1:
			stopbits=0x00;
			break;
		case 2:
			stopbits=0x10;
			break;
		default:
			break;
	}
    return baud|parity|databits|stopbits;
}


/*******************************************************************************
* ��������: ProxyWriteQueue
* ��������: д����
* ���������plockqueue          ���������ṹָ��
            buf                 ��Ҫд����е�����
            buf_len             ���ݳ���
* �����������
* �ڲ�����: �����в�������buf�е����ݷ�������У���������������������
* �� �� ֵ: >0                  �ɹ�
            =0                  ʧ��
*******************************************************************************/
int32 ProxyWriteQueue(ProxyLockQueue *ProxyQueue, uint8 *buf, int32 buf_len)
{
    int32 flags = 0;
	//printx("*PlcWriteQueue() pLockQueue:%p, &pLockQueue->QueueLock:%p, buf_len:%d \n", pLockQueue, &pLockQueue->QueueLock, buf_len);
	//PrintBuf(buf, buf_len);

    //��������
    pthread_mutex_lock(&ProxyQueue->QueueLock);
	GUI_FMT_LOGCC("ProxyQueue.QueueTail is %d\n",ProxyQueue->QueueTail);
	GUI_FMT_LOGCC("buf_len is %d\n",buf_len);
	GUI_FMT_LOGCC("ProxyQueue.Queue[ProxyQueue.QueueTail].Queue_Len is %d\n",ProxyQueue->Queue[ProxyQueue->QueueTail].Queue_Len);
    //�ж������Ƿ��пյ�λ��
    if(ProxyQueue->Queue[ProxyQueue->QueueTail].Queue_Len == 0)
    {
        ProxyQueue->Queue[ProxyQueue->QueueTail].Queue_Len = buf_len;
        memcpy((uint8*)ProxyQueue->Queue[ProxyQueue->QueueTail].Queue_Data, buf, buf_len);
        flags = 1;
        ProxyQueue->QueueTail = (ProxyQueue->QueueTail + 1) % (10);
    }
	GUI_FMT_LOGCC("ProxyQueue.QueueTail11 is %d\n",ProxyQueue->QueueTail);
    //�⻺����
    pthread_mutex_unlock(&ProxyQueue->QueueLock);

    return flags;
}

/*******************************************************************************
* ��������: ProxyReadQueue
* ��������: ������
* ���������pLockQueue          ���������ṹָ��
*           buf                 �����ݻ��������������е�����ȡ������û�����
* ���������plockqueue          �����е�����
* �ڲ�����: �ж������Ƿ������ݣ�����Ӷ�����ȡ�����ݷ���buf�У�ÿִ��һ�Σ�ȡһ������
* �� �� ֵ: len                 ���������ݳ���
*******************************************************************************/
int32 ProxyReadQueue(ProxyLockQueue *ProxyQueue, uint8 *buf)
{
    int32 len = 0;
    uint8 *pdata_buf;

    //��������
    pthread_mutex_lock(&ProxyQueue->QueueLock);  

    //�������������ݣ���ȡ��buf�У��⿪��
    if ((ProxyQueue->QueueHead != ProxyQueue->QueueTail)
        || ((ProxyQueue->QueueHead == ProxyQueue->QueueTail)
            && (ProxyQueue->Queue[ProxyQueue->QueueHead].Queue_Len != 0)))
    {
        pdata_buf = ProxyQueue->Queue[ProxyQueue->QueueHead].Queue_Data;  
        len = ProxyQueue->Queue[ProxyQueue->QueueHead].Queue_Len;
        
        //ȡ�����е�����
        memcpy(buf, pdata_buf, len); 

        //������
        memset(pdata_buf, 0, len);   
        ProxyQueue->Queue[ProxyQueue->QueueHead].Queue_Len = 0;

        //����ͷ����ƶ�һλ
        ProxyQueue->QueueHead= (ProxyQueue->QueueHead + 1) % (10); 
		GUI_FMT_LOGCC("ProxyQueue.QueueTail is %d\n",ProxyQueue->QueueHead);
		GUI_FMT_LOGCC("ProxyQueue.QueueTail is %d\n",ProxyQueue->QueueTail);
		GUI_FMT_LOGCC("ProxyQueue->Queue[ProxyQueue->QueueHead].Queue_Len is %d\n",ProxyQueue->Queue[ProxyQueue->QueueHead].Queue_Len);
		GUI_FMT_LOGCC("len is %d\n",len);
    }
    pthread_mutex_unlock(&ProxyQueue->QueueLock);
    
    return len;
}


