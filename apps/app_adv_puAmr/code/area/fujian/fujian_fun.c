#include "baseCommon.h"
#include "oadInfoList.h"
#include "oopservice.h"
#include "commClass.h"
#include "oopStorage.h"
#include "area.h"
#include "appcomm.h"
#include "oopservice.h"
#include "storage.h"
#include "msg_id.h"
#include "EventComm.h"
#include "hal.h"
#include "adc.h"
#include "task_msg.h"

//���汾��Ϣ��ȫ�ֱ�������ʽд����������Ĵ����У�ͳһ�� area.h ������ 
#ifdef ADV_PU_AMR
FACTORY_VER_TMP_T g_tVer = 
{
    "XXXX",      /**< ���̴���           */
    "CF22",      /**< ����汾��         */
    "220812",    /**< ����汾����       */
    "CC01",      /**< Ӳ���汾��         */
    "220818",    /**< Ӳ���汾����       */
    "CF22-01",   /**< ������չ��Ϣ       */
};
#elif ADV_ST_AMR
    #ifdef PRODUCT_ECU
    //��Դ������
    FACTORY_VER_TMP_T g_tVer = 
    {
        "XXXX",      /**< ���̴���           */
        "EF01",      /**< ����汾��         */
        "220619",    /**< ����汾����       */
        "ER01",      /**< Ӳ���汾��         */
        "220619",    /**< Ӳ���汾����       */
        "EF01-01",   /**< ������չ��Ϣ       */
    };
    #elif PRODUCT_ZCU
    //ר��
    FACTORY_VER_TMP_T g_tVer = 
    {
        "XXXX",      /**< ���̴���           */
        "1F01",      /**< ����汾��         */
        "210207",    /**< ����汾����       */
        "Z101",      /**< Ӳ���汾��         */
        "200624",    /**< Ӳ���汾����       */
        "1F01-01",   /**< ������չ��Ϣ       */
    };
    #elif PRODUCT_ZCU_2
    //ר��3��
    FACTORY_VER_TMP_T g_tVer = 
    {
        "XXXX",      /**< ���̴���           */
        "2F01",      /**< ����汾��         */
        "220715",    /**< ����汾����       */
        "Z301",      /**< Ӳ���汾��         */
        "220715",    /**< Ӳ���汾����       */
        "2F01-01",   /**< ������չ��Ϣ       */
    };
    #elif PRODUCT_ZCU_3
    //ר��3��
    FACTORY_VER_TMP_T g_tVer = 
    {
        "XXXX",      /**< ���̴���           */
        "3F01",      /**< ����汾��         */
        "220716",    /**< ����汾����       */
        "Z301",      /**< Ӳ���汾��         */
        "220715",    /**< Ӳ���汾����       */
        "3F01-01",   /**< ������չ��Ϣ       */
    };
    #endif
#endif


/* ����Ļ��� */
OOP_TASK_LIST_FJ_T oopTaskList_fj;

/* �ϱ������Ļ��� */
OOP_PLAN_REPORT_LIST_FJ_T oopPlanReportList_fj;

#if DESC("Ĭ�ϲ�������",1)

// ��ͨ�������г�APDU�����ȣ�����
const uint8 setDefaultPara[][LOCAL_RECV_FRAM_MAXLEN] = 
{
    {0x06, 0x01, 0x00, 0x43, 0x00, 0x08, 0x00, 0x03, 0x01, 0x00},  //�����ն������ϱ�
    {0x06, 0x01, 0x01, 0x40, 0x00, 0x03, 0x00, 0x16, 0x01, 0x00},  //����Ĭ��УʱģʽΪ����Уʱ

    {0x06, 0x01, 0x01, 0x31, 0x00, 0x09, 0x00, 0x03, 0x01, 0x00},  //0x31000900 //�ն˳�ʼ�� ��Ч
    {0x06, 0x01, 0x02, 0x31, 0x00, 0x08, 0x00, 0x16, 0x03, 0x00},  //0x31000800 //�ն˳�ʼ�� �ϱ�    

    {0x06, 0x01, 0x01, 0x31, 0x14, 0x09, 0x00, 0x03, 0x01, 0x00},  //0x31140900 //�ն˶�ʱ ��Ч
    {0x06, 0x01, 0x02, 0x31, 0x14, 0x08, 0x00, 0x16, 0x03, 0x00},  //0x31140800 //�ն˶�ʱ �ϱ�   

    {0x06, 0x01, 0x01, 0x31, 0x01, 0x09, 0x00, 0x03, 0x01, 0x00},  //0x31010900 //�ն˰汾��� ��Ч
    {0x06, 0x01, 0x02, 0x31, 0x01, 0x08, 0x00, 0x16, 0x03, 0x00},  //0x31010800 //�ն˰汾��� �ϱ�

    {0x06, 0x01, 0x01, 0x31, 0x1E, 0x09, 0x00, 0x03, 0x01, 0x00},  //0x311E0900 //������� ��Ч

    {0x06, 0x01, 0x01, 0x31, 0x19, 0x09, 0x00, 0x03, 0x01, 0x00},  //0x31190900 //������·�쳣 ��Ч

    {0x06, 0x01, 0x01, 0x31, 0x05, 0x09, 0x00, 0x03, 0x01, 0x00},  //0x31050900 //���ܱ�ʱ�ӳ��� ��Ч
    {0x06, 0x01, 0x01, 0x31, 0x05, 0x08, 0x00, 0x16, 0x03, 0x00},  //0x31050800 //���ܱ�ʱ�ӳ��� �ϱ�

    {0x06, 0x01, 0x01, 0x31, 0x06, 0x09, 0x00, 0x03, 0x01, 0x00},  //0x31060900 //�ն�ͣ/�ϵ� ��Ч
    {0x06, 0x01, 0x02, 0x31, 0x06, 0x08, 0x00, 0x16, 0x03, 0x00},  //0x31060800 //�ն�ͣ/�ϵ� �ϱ�

    {0x06, 0x01, 0x01, 0x31, 0x07, 0x09, 0x00, 0x03, 0x01, 0x00},  //0x31070900 //ֱ��ģ����Խ���� ��Ч

    {0x06, 0x01, 0x01, 0x31, 0x08, 0x09, 0x00, 0x03, 0x01, 0x00},  //0x31080900 //ֱ��ģ����Խ���� ��Ч

    {0x06, 0x01, 0x01, 0x31, 0x09, 0x09, 0x00, 0x03, 0x01, 0x00},  //0x31090900 //�ն���Ϣ��֤���� ��Ч

    {0x06, 0x01, 0x01, 0x31, 0x0A, 0x09, 0x00, 0x03, 0x01, 0x00},  //0x310A0900 //�豸���� ��Ч

    {0x06, 0x01, 0x01, 0x31, 0x0B, 0x09, 0x00, 0x03, 0x01, 0x00},  //0x310B0900 //���ܱ�ʾ���½� ��Ч

    {0x06, 0x01, 0x01, 0x31, 0x0C, 0x09, 0x00, 0x03, 0x01, 0x00},  //0x310C0900 //���������� ��Ч

    {0x06, 0x01, 0x01, 0x31, 0x0D, 0x09, 0x00, 0x03, 0x01, 0x00},  //0x310D0900 //���ܱ���� ��Ч

    {0x06, 0x01, 0x01, 0x31, 0x0E, 0x09, 0x00, 0x03, 0x01, 0x00},  //0x310E0900 //���ܱ�ͣ�� ��Ч
    {0x06, 0x01, 0x01, 0x40, 0x00, 0x04, 0x00, 0x02, 0x05, 0x11, 0x0A, 0x11, 0x02, 0x11, 0x02, 0x11, 0x01, 0x11, 0x03, 0x00},  //40000400 ����Уʱ����

    {0x06, 0x01, 0x01, 0x31, 0x0F, 0x09, 0x00, 0x03, 0x01, 0x00},  //0x310F0900 //�ն˳���ʧ�� ��Ч

    {0x06, 0x01, 0x01, 0x31, 0x10, 0x09, 0x00, 0x03, 0x01, 0x00},  //0x31100900 //��ͨ���������� ��Ч

    {0x06, 0x01, 0x01, 0x31, 0x11, 0x09, 0x00, 0x03, 0x01, 0x00},  //0x31110900 //����δ֪���ܱ� ��Ч
	
    {0x06, 0x01, 0x02, 0x31, 0x18, 0x08, 0x00, 0x16, 0x03, 0x00},  //0x31180800 //�ն˱���¼� �ϱ�
    {0x06, 0x01, 0x01, 0x31, 0x18, 0x09, 0x00, 0x03, 0x01, 0x00},  //0x31180900 //�ն˱���¼� ��Ч

};
const uint16 setDefaultParaLen[] = 
{
    10, //��Ӧ�����һ��APDU����
    10, //��Ӧ����ڶ���APDU����
    10,
    10,
    10,
    10,
    10,
    10,
    10,
    10,
    10,
    10,
    10,
    10,
    10,
    10,
    10,
    10,
    10,
    10,
    10,
    10,
    20,
    10,
    10,
    10,
    10,
    10,
};

const uint32 setDefaultParaNum = sizeof(setDefaultPara)/LOCAL_RECV_FRAM_MAXLEN;

const uint8 setDefaultRptSche[][LOCAL_RECV_FRAM_MAXLEN] = 
{
    {0x07,0x01,0x01,0x6e,0x17,0x7f,0x00,0x01,0x01,0x02,0x08,0x11,0x01,0x01,0x02,0x51,0x45,0x10,0x00,0x00,0x51,0x45,0x00,0x00,0x00,0x11,0x0a,0x54,0x00,0x00,0x14,0x11,0x01,0x16,0x00,0x11,0x00,0x02,0x02,0x16,0x01,0x02,0x03,0x51,0x6e,0x15,0x02,0x00,0x60,0x01,0x00,0x6e,0x16,0x02,0x00,0x5a,0x00,0x01,0x07,0xe6,0x04,0x13,0x0c,0x22,0x29,0x02,0x00,0x18},
    {0x07,0x01,0x01,0x6e,0x17,0x7f,0x00,0x01,0x01,0x02,0x08,0x11,0x02,0x01,0x02,0x51,0x45,0x10,0x00,0x00,0x51,0x45,0x00,0x00,0x00,0x11,0x14,0x54,0x00,0x00,0x14,0x11,0x03,0x16,0x00,0x11,0x0f,0x02,0x02,0x16,0x01,0x02,0x03,0x51,0x6e,0x15,0x02,0x00,0x60,0x01,0x00,0x6e,0x16,0x02,0x00,0x5a,0x00,0x01,0x07,0xe6,0x04,0x13,0x0c,0x22,0x29,0x02,0x00,0x18},
    {0x07,0x01,0x01,0x6E,0x17,0x7F,0x00,0x01,0x01,0x02,0x08,0x11,0x03,0x01,0x02,0x51,0x45,0x10,0x00,0x00,0x51,0x45,0x00,0x00,0x00,0x11,0x1E,0x54,0x00,0x00,0x14,0x11,0x03,0x16,0x00,0x11,0xFF,0x02,0x02,0x16,0x01,0x02,0x03,0x51,0x6E,0x15,0x02,0x00,0x60,0x01,0x00,0x6E,0x16,0x02,0x00,0x5A,0x00,0x01,0x07,0xE6,0x04,0x13,0x0C,0x22,0x2A,0x02,0x00,0x18},
    {0x07,0x01,0x01,0x6E,0x17,0x7F,0x00,0x01,0x01,0x02,0x08,0x11,0x04,0x01,0x02,0x51,0x45,0x10,0x00,0x00,0x51,0x45,0x00,0x00,0x00,0x11,0x19,0x54,0x00,0x00,0x14,0x11,0x03,0x16,0x01,0x11,0x0F,0x02,0x02,0x16,0x01,0x02,0x03,0x51,0x6E,0x15,0x02,0x00,0x60,0x01,0x00,0x6E,0x16,0x02,0x00,0x5A,0x00,0x01,0x07,0xE6,0x04,0x13,0x0C,0x22,0x2A,0x02,0x00,0x18},
    {0x07,0x01,0x01,0x6E,0x17,0x7F,0x00,0x01,0x01,0x02,0x08,0x11,0x05,0x01,0x02,0x51,0x45,0x10,0x00,0x00,0x51,0x45,0x00,0x00,0x00,0x11,0x19,0x54,0x00,0x00,0x14,0x11,0x03,0x16,0x02,0x11,0x0F,0x02,0x02,0x16,0x01,0x02,0x03,0x51,0x6E,0x15,0x02,0x00,0x60,0x01,0x00,0x6E,0x16,0x02,0x00,0x5A,0x00,0x01,0x07,0xE6,0x04,0x13,0x0C,0x22,0x2A,0x02,0x00,0x18},
};

const uint16 setDefaultRptScheLen[] = 
{
    68,
    68,
    68,
    68,
    68,
};

const uint32 setDefaultRptScheNum = sizeof(setDefaultRptSche)/LOCAL_RECV_FRAM_MAXLEN;

//������ļ������������±������·�Ĭ������
const APDU_DEF_CFG_T apduDefCfgList[] = 
{
    /* ʹ��SET(0)����ACT(1)���� APDU                           APDULEN                          APDUNUM*/
    {1,                         (uint8 *)setDefaultRptSche,   (uint16 *)setDefaultRptScheLen,   (uint32 *)&setDefaultRptScheNum},
    {0,                         (uint8 *)setDefaultPara,      (uint16 *)setDefaultParaLen,      (uint32 *)&setDefaultParaNum},
    {0xFF,                      NULL,                          NULL,                            NULL},
};

/**********************************************************************
* @name      : set_by_protocol_eare
* @brief     ���������� ���õ���Ĭ�ϲ���
* @param[in] ��int sockdbfd    ���
* @param[out]��
* @return    ��
**********************************************************************/
void set_by_protocol_eare(int sockdbfd, uint8 *pSetDefaultApdu, uint16 *pSetDefaultLen, uint32 setDefaultNum)
{
    uint32           i = 0;
    CHANNEL_CACHE_T  *tChannel = NULL;
    int              msglen = 0;
    uint8            uNewApdu[LOCAL_RECV_FRAM_MAXLEN] = {0};
    uint8            apduBuf[LOCAL_RECV_FRAM_MAXLEN] = {0};

    if ((pSetDefaultApdu == NULL) || (pSetDefaultLen == NULL) || (setDefaultNum == 0))
    {
        PRTL_FMT_DEBUG("set_by_protocol_eare para err pSetDefaultApdu[%p] pSetDefaultLen[%p] setDefaultNum[%d]\n", pSetDefaultApdu, pSetDefaultLen, setDefaultNum);
        return;
    }

    tChannel = (CHANNEL_CACHE_T  *)malloc(sizeof(CHANNEL_CACHE_T));
    memset(tChannel, 0, sizeof(CHANNEL_CACHE_T));

    tChannel->clientid = sockdbfd;

    for (i = 0; i < setDefaultNum; i++)
    {
        memcpy(apduBuf, &pSetDefaultApdu[i*LOCAL_RECV_FRAM_MAXLEN], LOCAL_RECV_FRAM_MAXLEN);

        msglen = invoke_set_req(tChannel, &apduBuf[OOP_DATA_OFFSET], pSetDefaultLen[i] - OOP_DATA_OFFSET, &uNewApdu[OOP_DATA_OFFSET], LOCAL_RECV_FRAM_MAXLEN - OOP_DATA_OFFSET);
        if (msglen != 0)
        {
            if (uNewApdu[7] != 0)
            {
                PRTL_BUF_DEBUG(&apduBuf[OOP_DATA_OFFSET], msglen, "set_by_protocol_eare err[%d] output apdu:", uNewApdu[7]);
            }
            else
            {
                PRTL_BUF_TRACE(&uNewApdu[OOP_DATA_OFFSET], msglen, "set_by_protocol_eare ok output apdu:");
            }
            
        }
        else
        {
            PRTL_BUF_DEBUG(&apduBuf[0], pSetDefaultLen[i], "set_by_protocol_eare err input apdu:");
        } 
    }

    free(tChannel);
    tChannel = NULL;
    return;
}

/**********************************************************************
* @name      : actDefaultPara_anhui
* @brief     ���������� ���õ���Ĭ�ϲ���
* @param[in] ��int sockdbfd    ���
* @param[out]��
* @return    ��
**********************************************************************/
void act_by_protocol_eare(int sockdbfd, uint8 *pActDefaultApdu, uint16 *pActDefaultLen, uint32 actDefaultNum)
{
    uint32           i = 0;
    CHANNEL_CACHE_T  *tChannel = NULL;
    int              msglen = 0;
    uint8            uNewApdu[LOCAL_RECV_FRAM_MAXLEN] = {0};
    uint8            apduBuf[LOCAL_RECV_FRAM_MAXLEN] = {0};

    if ((pActDefaultApdu == NULL) || (pActDefaultLen == NULL) || (actDefaultNum == 0))
    {
        PRTL_FMT_DEBUG("act_by_protocol_eare para err pActDefaultApdu[%p] pActDefaultLen[%p] actDefaultNum[%d]\n", pActDefaultApdu, pActDefaultLen, actDefaultNum);
        return;
    }

    tChannel = (CHANNEL_CACHE_T  *)malloc(sizeof(CHANNEL_CACHE_T));
    memset(tChannel, 0, sizeof(CHANNEL_CACHE_T));

    tChannel->clientid = sockdbfd;

    for (i = 0; i < actDefaultNum; i++)
    {
        memcpy(apduBuf, &pActDefaultApdu[i*LOCAL_RECV_FRAM_MAXLEN], LOCAL_RECV_FRAM_MAXLEN);

        msglen = invoke_act_req(tChannel, &apduBuf[OOP_DATA_OFFSET], pActDefaultLen[i] - OOP_DATA_OFFSET, &uNewApdu[OOP_DATA_OFFSET], LOCAL_RECV_FRAM_MAXLEN - OOP_DATA_OFFSET);
        if (msglen != 0)
        {
            if (uNewApdu[7] != 0)
            {
                PRTL_BUF_DEBUG(&uNewApdu[OOP_DATA_OFFSET], msglen, "act_by_protocol_eare err[%d] output apdu:", uNewApdu[7]);
            }
            else
            {
                PRTL_BUF_TRACE(&uNewApdu[OOP_DATA_OFFSET], msglen, "act_by_protocol_eare ok output apdu:");
            }
        }
        else
        {
            PRTL_BUF_DEBUG(&apduBuf[0], pActDefaultLen[i], "act_by_protocol_eare err input apdu:");
        } 
    }

    free(tChannel);
    tChannel = NULL;
    return;
}

/**********************************************************************
* @name      : set_act_by_protocol_anhui
* @brief     ���������� ͳһ�ӿ�
* @param[in] ��int sockdbfd    ���
* @param[out]��
* @return    ��
**********************************************************************/
void set_act_by_protocol_fujian(int sockdbfd)
{
    uint32 i = 0;
    uint32 num = 0;

    for (i = 0; i < APDU_DEF_CFG_MAX; i++)
    {
        if (apduDefCfgList[i].ifSetOrAct == 0xff)
        {
            break;
        }
    }

    if (i == APDU_DEF_CFG_MAX)
    {
        PRTL_FMT_DEBUG("find no 0xFF\n");
        return;
    }

    num = i;

    for (i = 0; i < num; i++)
    {
        if (apduDefCfgList[i].ifSetOrAct == 0x00)
        {
            set_by_protocol_eare(sockdbfd, apduDefCfgList[i].apduBuf, apduDefCfgList[i].apduBufLen, *apduDefCfgList[i].apduNum);
        }
        else if(apduDefCfgList[i].ifSetOrAct == 0x01)
        {
            act_by_protocol_eare(sockdbfd, apduDefCfgList[i].apduBuf, apduDefCfgList[i].apduBufLen, *apduDefCfgList[i].apduNum);
        }
    }

    return;
}


/**********************************************************************
* @name      : app_setdefault_cfg_anhui
* @brief     ���������� ���õ���Ĭ�ϲ���
* @param[in] ��int sockdbfd    ���
* @param[out]��
* @return    ��
**********************************************************************/
void app_setdefault_cfg_fujian(int sockdbfd)
{
    set_act_by_protocol_fujian(sockdbfd);

    return;
}

#endif

#if DESC("͸���¼�����",1)

OOP_GATHER_E		gGather;							//�ⲿ�豸���ݲɼ�ģʽ
OOP_TRANS_EVENT_STATUS_T gTransEvtStatus;				//͸���¼���ǰ״̬

extern OOP_OCTETVAR16_T    tmnAddr;                            //�ն�ͨ�ŵ�ַ
extern int rpt_task_package(OOP_OAD_U *pChOAD);
extern void evt_report_state_update(uint8 *pevt_data, OOP_REPORTS_T state);
extern OOP_DAR_E record_response_make(OOP_OAD_U oad, OOP_RCSD_T rcsd, DATA_CONVERT_T *convert);
extern uint16 rpt_packet_make(uint8 *pApdu, uint16 apduLen, uint8 choice, uint8 cAddr, uint8 piid, uint8 *outBuf, uint16 outLen);
extern int evt_get_oop_struct_data(mtrnum_t mtr_id, EVENT_COMM_INFO *eventCommInfo, OOP_EVTSOURCE_T eventSource, OOP_EVTBUF_T eventData, uint32 max_len, uint8 *pout_buf, uint8 bType);
extern int rpt_event_write(int clientid, void* data, uint16 datalen);

/**********************************************************************
* @brief     ����ȡ�ⲿ�豸���ݲɼ�ģʽ
* @param[in] ��

* @param[out]��
* @return    ��
* @Create    : ÷����
* @Date      ��2022-03-22
* @Update    :
**********************************************************************/
OOP_GATHER_E get_tmn_gather()
{
    return gGather;
}

/**********************************************************************
* @name      : WriteEventReportData
* @brief     ��д�ն��¼������ϱ�˽���ļ�
* @param[in] ��const char *filename   �¼��ϱ�˽���ļ�����
               OOP_OAD_U eventOAD     �¼�OAD
               uint8 *bDatabuf        �¼�����
               uint32 bufLen          �¼����ݳ���
* @param[out]��д���¼��ϱ�˽���ļ�
* @return    ��0 �ɹ��� <0 ʧ��
* @Create    : ÷����
* @Date      ��2020-01-11
* @Update    :
**********************************************************************/
int evt_packet_make(EVENT_COMM_INFO *eventCommInfo, DATA_CONVERT_T *pConvert)
{
    uint8 pEvtAPDU[1024] 	= {0};
	uint32 evtAPDULEN		= 0;
	uint8 cAddr				= 0;
    DATA_CONVERT_T temp;

    if(pConvert->srcBuf == NULL || pConvert->dstBuf ==NULL)
	{
		return -1;
	}
	
	//�ϱ������߰�ȫ���ϵͳ
    if(eventCommInfo->eventOAD.value == 0x31400200)
    {
        cAddr = 1;
    }

    temp.srcBuf = pConvert->srcBuf;
    temp.srcLen = pConvert->srcLen;
    temp.sOffset = pConvert->sOffset;
    
    temp.dOffset = &evtAPDULEN;
    temp.dstBuf = pEvtAPDU;
    temp.dstLen = 1024;

    //�����¼��ϱ�����
    record_response_make(eventCommInfo->eventOAD, eventCommInfo->eventRCSD, &temp);

    *pConvert->dOffset = rpt_packet_make(pEvtAPDU, evtAPDULEN, 1, cAddr, 0, pConvert->dstBuf, pConvert->dstLen);
    //����Ŀǰ��¼������
    
    return 0;
}

/**********************************************************************
* @name      : SetEventStat
* @brief     ���ն��¼���ǰֵ��¼
* @param[in] ��mtrnum_t          mtr_id            �������
               EVENT_COMM_INFO *eventCommInfo     �¼�������Ϣ
               OOP_EVTSOURCE_T  eventSource       �¼�����Դ
               uint8            bType             �¼�״̬ 0(ֻ��¼�¼����޷������ָ�����)��1������2�ָ�        
* @param[out]��   
* @return    ��0 �ɹ��� <0 ʧ��
* @Create    : ÷����
* @Date      ��2020-03-03
* @Update    :
**********************************************************************/
int evt_set_status_FJ(DB_CLIENT fd, OOP_TRANS_EVENT_STATUS_T *evtStatus)
{
    int             ret     = -1;
    NOMAL_OAD_T     nomalOAD;
    
    memset(&nomalOAD,0,sizeof(NOMAL_OAD_T));

    //�¼���ǰֵ��¼
    nomalOAD.oad.value = 0x6e310300;
    nomalOAD.classtag = CLASS_DATA_EVENT;
    ret = db_write_nomal(fd, &nomalOAD, (uint8 *)evtStatus, sizeof(OOP_TRANS_EVENT_STATUS_T));
    if (ret != ERR_OK)
    {
        EVENT_FMT_DEBUG("db_write_nomal oad[%08X]��¼�¼���ǰֵʧ�� ����%d\n", nomalOAD.oad.value, ret);
    }
    return ret;
}


/**********************************************************************
* @name      : evt_write_todb_FJ
* @brief     ���ն��¼�д�����ݿ�
* @param[in] ��DB_CLIENT fd            �������ľ��
               EVENT_COMM_INFO *eventCommInfo     �¼�������Ϣ
               uint8 *pevt_data       �¼�����
               uint32 data_len        �¼����ݳ���
               OOP_REPORTS_T state    �¼�ͨ���ϱ�״̬
* @param[out]��   
* @return    ��0 �ɹ��� <0 ʧ��
* @Create    : ÷����
* @Date      ��2021-2-4
* @Update    :
**********************************************************************/
int evt_write_todb_FJ(DB_CLIENT fd, EVENT_COMM_INFO *eventCommInfo, uint8 *pevt_data, uint32 data_len)
{  
    time_t              timep = 0;
    WRITE_RECORD_T      recInData;   
    
    memset(&recInData,0,sizeof(WRITE_RECORD_T));

    
    time(&timep); /*��ȡtime_t���͵ĵ�ǰʱ��*/
    recInData.recordOAD.classtag = CLASS_DATA_EVENT;
    recInData.recordOAD.road.oadCols.nNum = 1;
    recInData.recordOAD.road.oadCols.oad[0].value = 0x6e310200;
//    recInData.MAC = MAC;
    recInData.colStartTm = timep + COMPENSATION_TIME;
    recInData.colEndTm = timep + COMPENSATION_TIME;
    recInData.colStoreTm = gTransEvtStatus.lastRecvNo+1;

    recInData.inDatalen = data_len;
    memcpy(recInData.inDataBuf, pevt_data, data_len);

    return db_write_record(fd, &recInData, &eventCommInfo->storagenum);
    
}

extern int GetEventNumAndIndex(int clientid, uint32 eventOAD, uint16* nEventNum, uint32* nIndex);

/**********************************************************************
* @name      : evt_write_data_FJ
* @brief     ���ն��¼�д�����ݿ�
* @param[in] ��mtrnum_t          mtr_id            �������
               EVENT_COMM_INFO *eventCommInfo     �¼�������Ϣ
               OOP_EVTSOURCE_T  eventSource       �¼�����Դ
               OOP_EVTBUF_T     eventData         �¼�������������
               uint8            bType             �¼�״̬ 0(ֻ��¼�¼����޷������ָ�����)��1������2�ָ�        
* @param[out]��   
* @return    ��0 �ɹ��� <0 ʧ��
* @Create    : ÷����
* @Date      ��2022-03-22
* @Update    :
**********************************************************************/
int evt_write_data_FJ(DB_CLIENT fd, mtrnum_t mtr_id, EVENT_COMM_INFO *eventCommInfo, OOP_EVTSOURCE_T eventSource, OOP_EVTBUF_T eventData, uint8 bType)
{ 
    int                 ret                    		= 0;
//    time_t              timep                  		= 0;
    uint8               bDatabuf[DATA_LEN_MAX] 		= {0};
    uint32              bufLen                 		= 0;
	uint32				tmpOffset					= 0;
	DATA_CONVERT_T 		temp;
    OOP_REPORTS_T       state;
	OOP_TRANS_EVENT_T   transEvt;

    memset(&state,0,sizeof(OOP_REPORTS_T));
	memset(&transEvt,0,sizeof(OOP_TRANS_EVENT_T));

    //�ϱ�״̬
    state.nNum = rpt_task_package(&state.state[0].oad);

    EVENT_FMT_DEBUG("eventCommInfo->eventNum is %d,eventCommInfo->recordnum is %d\n", eventCommInfo->eventNum, eventCommInfo->recordnum);
    bufLen = evt_get_oop_struct_data(mtr_id, eventCommInfo, eventSource, eventData, DATA_LEN_MAX, bDatabuf, bType);
	if(bufLen == 0)
    {
        return ERR_O_RANGE;
    }

	evt_report_state_update(bDatabuf, state);

	//�����¼��ϱ�����
	temp.srcBuf = bDatabuf;
	temp.srcLen = bufLen;
	temp.sOffset = &tmpOffset;
	
	temp.dOffset = &transEvt.data.nNum;
	temp.dstBuf = transEvt.data.value;
	temp.dstLen = 2048;

	evt_packet_make(eventCommInfo, &temp);

	//͸���¼���������
	transEvt.curNum = gTransEvtStatus.recvSum+1;
	transEvt.selfStoreNo = gTransEvtStatus.lastRecvNo+1;
	current_tm_to_ooptm(&gTransEvtStatus.lastRecvTime);
	transEvt.reportTime = gTransEvtStatus.lastRecvTime;
	transEvt.add.len = tmnAddr.nNum-1;
	memcpy(transEvt.add.add, tmnAddr.value, tmnAddr.nNum);
	transEvt.oad.value = 0xf2e00200;
	transEvt.messType = OOP_MESS_FORMAT_698;

	ret = evt_write_todb_FJ(fd, eventCommInfo, (uint8 *)&transEvt, sizeof(OOP_TRANS_EVENT_T));
	if (ret != ERR_OK)
    {
        return ret;
    }
/*
    if(bType <= 1)//����
    {
        if(bType == 1) //���ܱ����
        {
            //��¼�洢��ź��¼���ʼʱ��
            write_pdata_xram("^Event", (char *)&eventCommInfo->storagenum, (eventCommInfo->EventStoragNumRecAddr+(mtr_id)*(4)), 4);
            time(&timep); //��ȡtime_t���͵ĵ�ǰʱ��
            write_pdata_xram("^Event", (char *)&timep, (eventCommInfo->EventStoragTmRecAddr+(mtr_id)*(4)), 4);

			//���ܱ�����¼���ͬһʱ�ڣ������ж���¼������ͻָ�����¼�����Ҫ�洢
			if(eventCommInfo->EventStoragIndexAddr != 0)
			{
				write_pdata_xram("^Event", (char *)&eventCommInfo->recordnum, (eventCommInfo->EventStoragIndexAddr+(mtr_id)*(4)), 4);
			}
        }

    }
*/
	gTransEvtStatus.lastRecvNo++;
	gTransEvtStatus.recvSum++;

    //�¼���ǰֵ��¼
    evt_set_status_FJ(fd, &gTransEvtStatus);

    //�ն��ϱ����ϱ��߳���ʵ��
    if(g_tnmEventReport == 1 && eventCommInfo->reportflag > EVENT_NO_REPORT)
    {
        //ֻ�ϱ��ָ��¼�
        if(bType <= 1 && eventCommInfo->reportflag == EVENT_RESTORE_REPORT)
            return ERR_OK;
            
        //ֻ�ϱ������¼�
        if(bType == 2 && eventCommInfo->reportflag == EVENT_HAPPENED_REPORT )
            return ERR_OK;
		
        rpt_event_write(fd, &transEvt, sizeof(OOP_TRANS_EVENT_T));
        EVENT_BUF_DEBUG(&transEvt, sizeof(OOP_TRANS_EVENT_T),"�ϱ����ݣ�");

    }
    return ERR_OK;
}

extern DB_CLIENT g_rptUdpHandle; //TODO
/**********************************************************************
* @name      : rpt_mtr_event_msg
* @brief     ����������¼��ϱ�
* @param[in] ��ack   ȷ����Ϣ
* @return    ��������Ϣ����
* @Create    : ����
* @Date      ��2020-04-20
* @Update    :
**********************************************************************/
void rpt_mtr_event_msg(int clientid, TASK_MSG_t *pMsg)
{
    int ret = 0;
    uint16 msglen = pMsg->msglen;
    uint8 *revcbuf = pMsg->msg;
    uint8  offset = 0;
    OOP_TRANS_EVENT_T transEvt = {0};
    uint8 offlen = 0;
    EVENT_COMM_INFO eventCommInfo;

    //�˿ں�
    memcpy_r(&transEvt.oad, &revcbuf[offset], sizeof(uint32));
    offset += sizeof(uint32);

    //ͨ�ŵ�ַ
    transEvt.add.len = 5;
    memcpy(transEvt.add.add, &revcbuf[offset], transEvt.add.len+1); 
    offset += transEvt.add.len + 1;

    //ͨ������
    transEvt.commType = revcbuf[offset++];

    //����֡
    transEvt.data.nNum = get_len_offset(&revcbuf[offset], &offlen);
    offset += offlen;

    if(transEvt.data.nNum == 0)
    {
        RPT_FMT_DEBUG("����¼��ϱ� ������.\n");
        return;
    }
    
    memcpy(&transEvt.data.value[0], &revcbuf[offset], transEvt.data.nNum); 
    offset += transEvt.data.nNum;

    if(offset > msglen)
    {
        RPT_FMT_DEBUG("����¼��ϱ�: offset(%d) > msglen(%d).\n", offset, msglen);
        return;
    }

    //��ȡ�¼������ͼ�¼���, ûŪ���������ߵ�����Ŀǰ��һ����
    eventCommInfo.eventNum = gTransEvtStatus.recvSum;
    eventCommInfo.recordnum = gTransEvtStatus.lastRecvNo;

    transEvt.curNum = gTransEvtStatus.recvSum+1;
    transEvt.selfStoreNo = gTransEvtStatus.lastRecvNo+1;
    current_tm_to_ooptm(&gTransEvtStatus.lastRecvTime);
    transEvt.reportTime = gTransEvtStatus.lastRecvTime;

    ret = evt_write_todb_FJ(clientid, &eventCommInfo, (uint8 *)&transEvt, sizeof(OOP_TRANS_EVENT_T));
    if (ret != ERR_OK)
    {
        RPT_FMT_DEBUG("д͸���¼���¼��ʧ�� ����%d\n", ret);
        return;
    }

    gTransEvtStatus.lastRecvNo++;
    gTransEvtStatus.recvSum++;

    //�¼���ǰֵ��¼
    evt_set_status_FJ(clientid, &gTransEvtStatus);
    
    if(g_tnmEventReport == 1)
    {
        rpt_event_write(clientid, &transEvt, sizeof(OOP_TRANS_EVENT_T));
    }

    return;
}

#endif
#if DESC("�ն�ͣ�紦��",1)
#define BATTERY_VOL			360		//��ص�ѹ3.6V

/** @brief ��������������    */
typedef union POWER_STATUS
{
    uint16          status;              
    struct
    {
        struct
        {
			uint8   bit7  : 1;         /**< ����     */
			uint8   bit6  : 1;         /**< ����     */
			uint8   bit5  : 1;         /**< ͣ�糬������     */
			uint8   bit4  : 1;         /**< ʱ�ӵ��     */
			uint8   bit3  : 1;         /**< ͣ�繤�����     */
			uint8   bit12 : 2;         /**< ���緽ʽ     */
			uint8   bit0  : 1;         /**< ����    */
        };
        uint8       value2; 
    };
}POWER_STATUS_T;

/**********************************************************************
* @name      : get_battery_vol
* @brief     ����ȡ������ص�ѹ
* @param[in] ��  
* @param[out]��vol: ��ѹ
* @return    ��1 �ɹ��� <0 ʧ��
* @Create    : ÷����
* @Date      ��2020-07-27
* @Update    :
**********************************************************************/
int get_battery_vol(uint16 *vol)
{

	int state = 0;
	
	ADC_DEVICE_T* dev = (ADC_DEVICE_T*) hal_device_get(HW_DEVICE_ID_ADC);
	if(NULL == dev)
	{
        EVENT_FMT_LOGCC("dev is null!\n");
        return -1;
	}
	
	if(dev->adc_volt_read(dev,EMADC_BATVOL_CHANNEL,&state) < 0)
	{
		EVENT_FMT_LOGCC("get power state failed!\n");
		return -1;
	}

	*vol = (uint16)state/10;

    EVENT_FMT_TRACE("get power state%d! \n", *vol);
    
	hal_device_release((HW_DEVICE *)dev);
	return 1;
}

/**********************************************************************
* @name      : get_battery_status
* @brief     ������ն˵��״̬���״̬
* @param[in] ��status 		1 �е�� ��0 ��ر��γ�
* @param[out]��
* @return    ��0 �ɹ��� <0 ʧ��
* @Create    : ÷����
* @Date      ��2022-06-25
* @Update    :
**********************************************************************/
int get_battery_status(uint8* status)
{
    int32 fd = -1;
    uint32 uData;
    int32 iRet = ERR_OK;
    
    /* open device file */
    fd = open("/dev/ym", O_RDWR);
    if(fd < 0)
    {
        EVENT_FMT_DEBUG("open get_battery_status file error!\n");
        return ERR_NORMAL;
    }

    /* ��������״̬ */
    if(read(fd, &uData, sizeof(uData)) 
        != sizeof(uData))
    {
       iRet = ERR_IO;
       goto end;
    }

	*status = (uData>>8)&0x01;

	//ʵ�ʵ��Է��֣��γ�Ϊ1������Ϊ0
	*status = (*status ==1) ? 0: 1;

	EVENT_FMT_TRACE(" get_battery_status [%x]!\n", uData);
	EVENT_FMT_TRACE(" get_battery_status [%x]!\n", *status);
end:
  //  printf("power.data = %d\n", data);
    close(fd);
    return iRet;    
}


/**********************************************************************
* @name      : check_power_status
* @brief     ������ն˵�Դ״̬
* @param[in] ��int    clientid     �������ľ��  
			   state: ��Դ״̬��1�е磬-1 ����
* @param[out]��
* @return    ��1 �ɹ��� <0 ʧ��
* @Create    : ÷����
* @Date      ��2020-07-27
* @Update    :
**********************************************************************/
int check_power_status(int clientid, int state)
{
	POWER_STATUS_T *pStatus;
	uint16 vol = 0;
	uint8 change = 0;

	get_battery_vol(&vol);

	pStatus = (POWER_STATUS_T *)event_3106.powerStatus.nValue;
//	memcpy(&tmpStatus.status, event_3106.powerStatus.nValue, sizeof(uint16));
	
	if(state == 1)		//����Դ����
	{
        if(pStatus->bit12 != 0)
		{
		    pStatus->bit12 = 0;
		    change  = 1;
		}
	}

	if(state == -1)		//��ع���
	{
        if(pStatus->bit12 != 2)
		{
		    pStatus->bit12 = 2;
		    change  = 1;
		}
	}

	if(vol < BATTERY_VOL)		//���Ƿѹ
	{
        if(pStatus->bit3 != 1)
		{
		    pStatus->bit3 = 1;
		    change  = 1;
		}
	}
	
	if(vol >= BATTERY_VOL)		//�������
	{
        if(pStatus->bit3 != 0)
		{
		    pStatus->bit3 = 0;
		    change  = 1;
		}
	}

	if(change == 1)		//״̬���
	{
        evt_write_nomal_data(clientid, 0, 0, 0x4e010200, (uint8 *)&event_3106.powerStatus, sizeof(OOP_BITSTR16_T));
		EVENT_FMT_TRACE("CheckERC_3106 ����ն�ͣ���¼�%02X%02X %04X \n", 
         event_3106.powerStatus.nValue[0], event_3106.powerStatus.nValue[1], pStatus->status);
	}
	
	return 1;
}

extern int PowerDownCheck(void);

/**********************************************************************
* @name      : check_power_status
* @brief     �����ͣ�繤�����״̬,�ն�����ʱ
* @param[in] ��int    clientid     �������ľ��  
			   state: ��Դ״̬��1�е磬0 ����
			   vol: ��ѹ
* @param[out]��
* @return    ��1 �ɹ��� <0 ʧ��
* @Create    : ÷����
* @Date      ��2020-07-27
* @Update    :
**********************************************************************/
int check_battery_status(int clientid)
{
	uint16 vol = 0;
	uint8 change = 0;

	get_battery_vol(&vol);

	if(event_3106.batteryStatus.voltage != vol)		//��ѹ�ı�
	{
		event_3106.batteryStatus.voltage = vol;
        change  = 1;
	}
	
	if(event_3106.tmFlag == 0xaa)		//��ص�����Ϊ0, ���ն�����ͣ�繤����ص������㣬3����δ����ֹͣ����
	{

		if (PowerDownCheck() < 180)		//�ն�ͣ��û��3���Ӿ��ϵ磬������
		{
			return 1;

		}
		else
		{
			if(event_3106.batteryStatus.capacity != 0)
			{
			    event_3106.batteryStatus.capacity = 0;
			    change  = 1;
			}
		}
        
	}
	else if(event_3106.tmFlag == 0x55)		//��ص�����Ϊ100
	{
        if(event_3106.batteryStatus.capacity != 100)
		{
		    event_3106.batteryStatus.capacity = 100;
		    change  = 1;
		}
	}
    
	if(change == 1)		//״̬���
	{
        evt_write_nomal_data(clientid, 0, 0, 0x4e020200, (uint8 *)&event_3106.batteryStatus, sizeof(OOP_BATTERY_STATUS_T));
	}
	return 1;
}


/**********************************************************************
* @name      : check_power_status
* @brief     �����ͣ�繤�����״̬,�ն�����ʱ
* @param[in] ��int    clientid     �������ľ��  
			   state: ��Դ״̬��1�е磬0 ����
			   vol: ��ѹ
* @param[out]��
* @return    ��1 �ɹ��� <0 ʧ��
* @Create    : ÷����
* @Date      ��2020-07-27
* @Update    :
**********************************************************************/
int check_battery_status_run(int clientid)
{
	int ret = 0;
	uint8 batStatus = 0;
	static uint8 lastBatStatus = 1;
	uint16 vol = 0;

	get_battery_vol(&vol);
	ret = get_battery_status(&batStatus);

	if(ret < 0)		//ʧ��
	{
		return ret;
	}

	if(batStatus == 0 && (event_3106.batteryStatus.capacity != 0 || event_3106.batteryStatus.voltage != 0 ))
	{
        event_3106.batteryStatus.capacity = 0;
		event_3106.batteryStatus.voltage = 0;
		evt_write_nomal_data(clientid, 0, 0, 0x4e020200, (uint8 *)&event_3106.batteryStatus, sizeof(OOP_BATTERY_STATUS_T));
	}

	if(batStatus == 1 && lastBatStatus == 0 )
	{
		event_3106.batteryStatus.voltage = vol;
		event_3106.batteryStatus.capacity = 100;
		evt_write_nomal_data(clientid, 0, 0, 0x4e020200, (uint8 *)&event_3106.batteryStatus, sizeof(OOP_BATTERY_STATUS_T));
	}

	lastBatStatus = batStatus;
	
	return 1;
}


/**********************************************************************
* @name      : evt3106_write_data_FJ
* @brief     ���ն�ͣ���¼�д�����ݿ⣬���ն�ͣ��һ���Ӻ���ϵ�������¼�ϱ�
* @param[in] ��DB_CLIENT fd		���ݿ���      
			   uint32 tmnDownAddr �ն�ͣ���¼�ͣ��ʱ��洢˽�е�ַ
* @param[out]��   
* @return    ��0 �ɹ��� <0 ʧ��
* @Create    : ÷����
* @Date      ��2022-03-22
* @Update    :
**********************************************************************/
int evt3106_write_data_FJ(DB_CLIENT fd, uint32 tmnDownAddr)
{
	OOP_EVTSOURCE_T eventSource; //�¼�����Դ
    OOP_EVTBUF_T    eventData;   //�¼��������� 
    
    memset(&eventSource,0,sizeof(OOP_EVTSOURCE_T));
    memset(&eventData,0,sizeof(OOP_EVTBUF_T));  

	int  ret = 0;

//	read_pdata_xram("^Event", (char *)&event_3106.tmnStarttime, tmnDownAddr, sizeof(OOP_DATETIME_S_T));

    //�¼�����Դ
    eventSource.choice = DT_ENUM;
    eventSource.nValue = 0;             //ͣ��

    //�¼���������
    
	//��ǰ�ж�Ϊͣ����δд��ͣ���¼
	event_3106.eventFlag = 0x80;
	//д���ݿ�
    ret = WriteEventToDB(fd, 0, &event_3106.eventCommInfo, eventSource, eventData, 0);
    if (ret < ERR_OK)
    {
        EVENT_FMT_DEBUG("CheckERC_3106 �ն�ͣ���¼���¼ʧ�ܷ���%d\n", ret);
        return ret;
    }
    EVENT_FMT_TRACE("CheckERC_3106 �ն�ͣ���¼���¼�ɹ�, storagenum=%u !\n", event_3106.eventCommInfo.storagenum);
	return ret;
}

#endif

#if DESC("����������" ,1)

/**********************************************************************
* @name      : task_list_lock_init
* @brief     ��
* @param[out]��
* @return    ��
* @Create    : ����
* @Date      ��2020-1-10
* @Update    :
**********************************************************************/
void task_list_lock_init_fj(void)
{
    /* ��ʼ���� */
    memset(&oopTaskList_fj, 0, sizeof(OOP_TASK_LIST_FJ_T));
    pthread_mutex_init(&oopTaskList_fj.mutex, NULL);
}

/**
*********************************************************************
* @name��      appdb_task_list_record_init
* @brief��     ������������ʼ��(��¼���ݷ�ʽ)
* @param[in] ��clientid  ���ݿ���
* @param[out]��
* @return    ��
* @author    : 
* @Date      ��2019-12-6
*********************************************************************
*/
void appdb_task_list_record_init_fj(int clientid)
{
    int                ret = ERR_NORMAL;
    uint32             i = 0;
    uint32             oopTaskNum = 0;
    OOP_TRANS_TASK_T   oopTask = {0};
    READ_RECORD_T      inReadRecord;
    uint32             handle = 0;
    uint32             recNum = 0;
    STEP_READ_RECORD_T stepReadRecord;
    RECORD_UNIT_T      stepRecordOut;

    memset(&oopTaskList_fj, 0, sizeof(OOP_TASK_LIST_FJ_T));
    memset(&inReadRecord, 0, sizeof(READ_RECORD_T));
    memset(&stepReadRecord, 0, sizeof(STEP_READ_RECORD_T));
    memset(&stepRecordOut, 0, sizeof(RECORD_UNIT_T));

    PRTL_FMT_DEBUG("���ظ���ȫ��͸�����񷽰�.\n");
    inReadRecord.recordOAD.logicId = 0;
    inReadRecord.recordOAD.infoNum = 0xffff;
    inReadRecord.cType = COL_NO_INFO;
    inReadRecord.cStart = 1;
    inReadRecord.cEnd = 0xffff;
    inReadRecord.sortType = UP_SORT;
    inReadRecord.recordOAD.optional = 0;
    inReadRecord.recordOAD.road.oadMain.value = 0;
    inReadRecord.recordOAD.road.oadCols.nNum = 0x01;
    inReadRecord.recordOAD.road.oadCols.oad[0].value = 0x6E110200;

    //����Ҫ����У��
    stepReadRecord.MAC.nNum = 0;

    ret = db_read_record_start(clientid, &inReadRecord, &handle, &recNum);
    if ((ret != ERR_OK) || (recNum == 0))
    {
        PRTL_FMT_DEBUG("appdb_task_list_record_init db_read_record_start err! ret = %d, infoNum = %d recNum = %d\n", 
            ret, inReadRecord.recordOAD.infoNum, recNum);
    }
    else
    {
        PRTL_FMT_DEBUG("appdb_task_list_record_init db_read_record_start ok ret = %d, infoNum = %d recNum = %d\n", 
            ret, inReadRecord.recordOAD.infoNum, recNum);
        for (i = 0; i < recNum; i++)
        {
            ret = db_read_record_step(clientid, handle, &stepReadRecord, &stepRecordOut);
            if (ret != ERR_OK)
            {
                PRTL_FMT_DEBUG("appdb_task_list_record_init db_read_record_step err = %d!\n", ret);
                continue;
            }

            //���������Ч��
            if ((stepRecordOut.recordnum != 1) 
            || (stepRecordOut.record[0].road.oadCols.oad[0].value != 0x6E110200)
            || (stepRecordOut.record[0].outDatalen != sizeof(OOP_TRANS_TASK_T)))
            {
                PRTL_FMT_DEBUG("appdb_task_list_record_init data err !\n");
                PRTL_FMT_DEBUG("appdb_task_list_record_init stepRecordOut.recordnum = %d\n", stepRecordOut.recordnum);
                PRTL_FMT_DEBUG("appdb_task_list_record_init stepRecordOut.record[0].road.oadCols.oad[0] = 0x%08x\n", 
                    stepRecordOut.record[0].road.oadCols.oad[0].value);
                PRTL_FMT_DEBUG("appdb_task_list_record_init stepRecordOut.record[0].outDatalen = %d\n", 
                    stepRecordOut.record[0].outDatalen);
                PRTL_FMT_DEBUG("appdb_task_list_record_init sizeof(OOP_TRANS_TASK_T) = %d\n", 
                    sizeof(OOP_TRANS_TASK_T));
                continue;
            }

            memcpy(&oopTask, stepRecordOut.record[0].outDataBuf, stepRecordOut.record[0].outDatalen);
            PRTL_FMT_DEBUG("stepRecordOut.infoNum = %d\n", stepRecordOut.infoNum);
            PRTL_FMT_DEBUG("oopTask.id = %d\n", oopTask.taskid);
            PRTL_FMT_LOGCC("oopTask.planType = %d\n", oopTask.taskTypeData.taskType);
            PRTL_FMT_LOGCC("oopTask.planID = %d\n", oopTask.reportid);

            //��Ŵ�1��ʼ
            oopTaskList_fj.oopTask[oopTaskNum++] = oopTask;

        }
    }
    db_read_record_end(clientid, handle);
    
    PRTL_FMT_DEBUG("����ȫ��͸�����񷽰��������.\n");

    /* ��¼�������� */
    oopTaskList_fj.oopTaskNum = oopTaskNum;

    PRTL_FMT_DEBUG("���ظ���ȫ��͸�����񷽰���: %d\n", oopTaskList_fj.oopTaskNum);

    return;
}

/**********************************************************************
* @name      : app_get_task_list
* @brief     ���ӻ����л�ȡ��������
* @param[in] ��taskID  - �������          
* @param[out]: oopTask - ��������
* @return    : ������
**********************************************************************/
int app_get_task_list_fj(uint16 taskID, OOP_TRANS_TASK_T *oopTask)
{
    int32 ret = ERR_NORMAL;
    int i = 0;

    if(NULL == oopTask)
    {
        return ERR_PNULL;
    }

    /* ʹ�û��������м��� */
    pthread_mutex_lock(&oopTaskList_fj.mutex);

    for(i = 0; i < oopTaskList_fj.oopTaskNum; i++)
    {
        if(oopTaskList_fj.oopTask[i].taskid == taskID)
        {
            memcpy(oopTask, &oopTaskList_fj.oopTask[i], sizeof(OOP_TRANS_TASK_T));
            break;
        }
    }

    /* ʹ�û��������н��� */
    pthread_mutex_unlock(&oopTaskList_fj.mutex);

    if(i < oopTaskList_fj.oopTaskNum)
    {
        ret = ERR_OK;
    }
    else
    {
        ret = ERR_INVALID;
    }
 
    return ret;
}


/**********************************************************************
* @name      : app_get_meter_list
* @brief     ��ͨ������ID��ȡ��������
* @param[in] ��meterID        - �����ڲ����
* @param[out]��oopMeter - ����
* @return    : ������
* @Create    : ����
* @Date      ��2019-12-16
* @Update    :
**********************************************************************/
int app_get_task_list_fj_by_id(uint16 innerID, OOP_TRANS_TASK_T *oopTask)
{
    uint16 oopTaskNum = oopTaskList_fj.oopTaskNum;

    if((NULL == oopTask)||(innerID == 0xffff) || (innerID >= oopTaskNum))
    {
        return ERR_PNULL;
    }

    /* ʹ�û��������м��� */
    pthread_mutex_lock(&oopTaskList_fj.mutex);

    *oopTask = oopTaskList_fj.oopTask[innerID];

    /* ʹ�û��������н��� */
    pthread_mutex_unlock(&oopTaskList_fj.mutex);

    return ERR_OK;    
}

/**********************************************************************
* @name      : app_set_task_list
* @brief     �������µ���������д�뻺��
* @param[in] ��taskID  - �������          
               oopTask - ��������
* @return    ��������
**********************************************************************/
int app_set_task_list_fj(uint16 taskID, OOP_TRANS_TASK_T *oopTask)
{
    int ret = ERR_OK;
    int i = 0;
    if(NULL == oopTask)
    {
        return ERR_PNULL;
    }

    /* ʹ�û��������м��� */
    pthread_mutex_lock(&oopTaskList_fj.mutex);

    for(i = 0; i < oopTaskList_fj.oopTaskNum; i++)
    {
        if(oopTaskList_fj.oopTask[i].taskid == taskID)
        {
            memcpy(&oopTaskList_fj.oopTask[i], oopTask, sizeof(OOP_TRANS_TASK_T));
            PRTL_FMT_DEBUG("[%d]����͸�����񷽰�-%d\n", i,         oopTaskList_fj.oopTask[i].taskid);
            break;
        }
    }

    if(i == oopTaskList_fj.oopTaskNum)
    {
        if(oopTaskList_fj.oopTaskNum < 256)
        {
            memcpy(&oopTaskList_fj.oopTask[oopTaskList_fj.oopTaskNum++], oopTask, sizeof(OOP_TRANS_TASK_T));
            PRTL_FMT_DEBUG("[%d]����͸�����񷽰�-%d\n", i,         oopTaskList_fj.oopTask[i].taskid);
        }
        else
        {
            ret = ERR_NORESOURCE;
        }
    }

    /* ʹ�û��������н��� */
    pthread_mutex_unlock(&oopTaskList_fj.mutex);

    return ret;
}

/**********************************************************************
* @name      : app_set_task_invalid
* @brief     ����������������Ч
* @param[in] : taskID - �������
* @param[out]��
* @return    ��������
**********************************************************************/
int app_set_task_invalid_fj(uint16 taskID)
{
    int i = 0;
    
    /* ʹ�û��������м��� */
    pthread_mutex_lock(&oopTaskList_fj.mutex);

    for(i = 0; i < oopTaskList_fj.oopTaskNum; i++)
    {
        if(oopTaskList_fj.oopTask[i].taskid == taskID)
        {
            if(i < oopTaskList_fj.oopTaskNum-1)
            {
                memmove(&oopTaskList_fj.oopTask[i], &oopTaskList_fj.oopTask[i+1], sizeof(OOP_TRANS_TASK_T));
            }

            oopTaskList_fj.oopTaskNum--;
            PRTL_FMT_DEBUG("[%d]ɾ��͸�����񷽰�-%d\n", i,         oopTaskList_fj.oopTask[i].taskid);
            break;
        }
    }

    /* ʹ�û��������н��� */
    pthread_mutex_unlock(&oopTaskList_fj.mutex);

    return ERR_OK;
}

/**********************************************************************
* @name      : appdb_get_task_list
* @brief     �������б�����
* @param[in] ��int clientid    socket���
               uint8 logicId   �߼���ַ
               uint16 infoNum  ��Ϣ���
               uint32 value    oadֵ
* @param[out]��
* @return    ��0-�ɹ�/������
**********************************************************************/
int appdb_get_task_list_fj(int clientid, uint8 logicId, uint16 infoNum, uint32 value)
{
    int                ret = 0;
    uint32             readlen = 0;
    NOMAL_OAD_T        tOAD = {0};
    OOP_TRANS_TASK_T         oopTask = {0};

    if(0x6E110200 != value)
    {
        return -1;
    }

    tOAD.logicId   = logicId;
    tOAD.infoNum   = infoNum;
    tOAD.oad.value = value;

    PRTL_FMT_TRACE("͸�����񷽰����� logicId(%d) infoNum(%d) value(0x%x)\n", logicId, infoNum, value);

    if (infoNum == 0xffff)
    {
        oopTaskList_fj.oopTaskNum = 0;    
        return 0;
    }

    ret = db_read_nomal(clientid, &tOAD, sizeof(OOP_TRANS_TASK_T), (uint8 *)&oopTask, &readlen);
    if(0 != ret || readlen != sizeof(OOP_TRANS_TASK_T))
    {
        app_set_task_invalid_fj(infoNum);
        PRTL_FMT_DEBUG("db_read_nomal failed and app_set_task_invalid. ret(%d), infoNum(%d), readlen(%d), explen(%d)\n", ret, infoNum, readlen, sizeof(OOP_TRANS_TASK_T));
        return ERR_NORMAL;
    }

    PRTL_FMT_TRACE("oopTask.taskID = %d\n", oopTask.taskid);
    PRTL_FMT_TRACE("oopTask.acqFreq.value = %d\n", oopTask.cycle.value);
    PRTL_FMT_TRACE("oopTask.acqFreq.unit = %d\n", oopTask.cycle.unit);
    PRTL_FMT_TRACE("oopTask.planType = %d\n", oopTask.taskTypeData.taskType);
    PRTL_FMT_TRACE("oopTask.planID = %d\n", oopTask.reportid);
    PRTL_FMT_TRACE("oopTask.startTime = %d\n", oopTask.startTime);
    PRTL_FMT_TRACE("oopTask.endTime = %d\n", oopTask.endTime);
    
    ret = app_set_task_list_fj(infoNum, &oopTask);
    if (ret != ERR_OK)
    {
        app_set_task_invalid_fj(infoNum);
        PRTL_FMT_DEBUG("app_set_task_list failed. ret(%d), infoNum(%d)\n", ret, infoNum);
        return ERR_NORMAL;
    }
    
    return 0;
}

#endif

#if DESC("�ϱ������������" ,1)

/**********************************************************************
* @name      : plan_report_list_lock_init
* @brief     ��
* @param[out]��
* @return    ��
* @Create    : ����
* @Date      ��2020-1-10
* @Update    :
**********************************************************************/
void plan_report_list_lock_init_fj(void)
{
    /* ��ʼ���� */
    memset(&oopPlanReportList_fj, 0, sizeof(OOP_PLAN_REPORT_LIST_FJ_T));
    pthread_mutex_init(&oopPlanReportList_fj.mutex, NULL);

    memset(&oopReportTri, 0, sizeof(OOP_REPORT_TRIGGER_T));
    pthread_mutex_init(&oopReportTri.mutex, NULL);

    memset(&oopEventRptTri, 0, sizeof(OOP_EVTRPT_TRIGGER_T));
    pthread_mutex_init(&oopEventRptTri.mutex, NULL);
}

/**
*********************************************************************
* @name��      appdb_plan_report_list_record_init
* @brief��     �ϱ��������������ʼ��(��¼���ݷ�ʽ)
* @param[in] ��clientid  ���ݿ���
* @param[out]��
* @return    ��
* @author    : 
* @Date      ��2019-12-6
*********************************************************************
*/
void appdb_plan_report_list_record_init_fj(int clientid)
{
    int                ret = ERR_NORMAL;
    uint32             i = 0;
    uint32             oopPlanReportNum = 0;
    OOP_TRANS_REPORT_T  oopPlanReport = {0};
    READ_RECORD_T      inReadRecord;
    uint32             handle = 0;
    uint32             recNum = 0;
    STEP_READ_RECORD_T stepReadRecord;
    RECORD_UNIT_T      stepRecordOut;

    memset(&oopPlanReportList_fj, 0, sizeof(OOP_PLAN_REPORT_LIST_FJ_T));
    memset(&inReadRecord, 0, sizeof(READ_RECORD_T));
    memset(&stepReadRecord, 0, sizeof(STEP_READ_RECORD_T));
    memset(&stepRecordOut, 0, sizeof(RECORD_UNIT_T));

    PRTL_FMT_DEBUG("���ظ��������ϱ�����.\n");
    inReadRecord.recordOAD.logicId = 0;
    inReadRecord.recordOAD.infoNum = 0xffff;
    inReadRecord.cType = COL_NO_INFO;
    inReadRecord.cStart = 1;
    inReadRecord.cEnd = 256;
    inReadRecord.sortType = UP_SORT;
    inReadRecord.recordOAD.optional = 0;
    inReadRecord.recordOAD.road.oadMain.value = 0;
    inReadRecord.recordOAD.road.oadCols.nNum = 0x01;
    inReadRecord.recordOAD.road.oadCols.oad[0].value = 0x6E170200;

    //����Ҫ����У��
    stepReadRecord.MAC.nNum = 0;

    ret = db_read_record_start(clientid, &inReadRecord, &handle, &recNum);
    if ((ret != ERR_OK) || (recNum == 0))
    {
        PRTL_FMT_DEBUG("appdb_plan_report_list_record_init db_read_record_start err! ret = %d, infoNum = %d recNum = %d\n", 
            ret, inReadRecord.recordOAD.infoNum, recNum);
    }
    else
    {
        PRTL_FMT_DEBUG("appdb_plan_report_list_record_init db_read_record_start ok ret = %d, infoNum = %d recNum = %d\n", 
            ret, inReadRecord.recordOAD.infoNum, recNum);
        for (i = 0; i < recNum; i++)
        {
            ret = db_read_record_step(clientid, handle, &stepReadRecord, &stepRecordOut);
            if (ret != ERR_OK)
            {
                PRTL_FMT_DEBUG("appdb_plan_report_list_record_init db_read_record_step err = %d!\n", ret);
                continue;
            }

            //���������Ч��
            if ((stepRecordOut.recordnum != 1) 
            || (stepRecordOut.record[0].road.oadCols.oad[0].value != 0x6E170200)
            || (stepRecordOut.record[0].outDatalen != sizeof(OOP_TRANS_REPORT_T)))
            {
                PRTL_FMT_DEBUG("appdb_plan_report_list_record_init data err !\n");
                PRTL_FMT_DEBUG("appdb_plan_report_list_record_init stepRecordOut.recordnum = %d\n", stepRecordOut.recordnum);
                PRTL_FMT_DEBUG("appdb_plan_report_list_record_init stepRecordOut.record[0].road.oadCols.oad[0] = 0x%08x\n", 
                    stepRecordOut.record[0].road.oadCols.oad[0].value);
                PRTL_FMT_DEBUG("appdb_plan_report_list_record_init stepRecordOut.record[0].outDatalen = %d\n", 
                    stepRecordOut.record[0].outDatalen);
                PRTL_FMT_DEBUG("appdb_plan_report_list_record_init sizeof(OOP_TRANS_REPORT_T) = %d\n", 
                    sizeof(OOP_TRANS_REPORT_T));
                continue;
            }

            memcpy(&oopPlanReport, stepRecordOut.record[0].outDataBuf, stepRecordOut.record[0].outDatalen);
            PRTL_FMT_DEBUG("stepRecordOut.infoNum = %d\n", stepRecordOut.infoNum);
            PRTL_FMT_DEBUG("oopPlanReport.planID = %d\n", oopPlanReport.reportid);
            PRTL_FMT_DEBUG("oopPlanReport.retry = %d\n", oopPlanReport.retry);
            PRTL_FMT_DEBUG("oopPlanReport.timeout.value = %d\n", oopPlanReport.timeout.value);
            PRTL_FMT_DEBUG("oopPlanReport.timeout.unit = %d\n", oopPlanReport.timeout.unit);

            //��Ŵ�1��ʼ
            oopPlanReportList_fj.oopPlanReport[oopPlanReportNum++] = oopPlanReport;

        }
    }
    db_read_record_end(clientid, handle);
    
    PRTL_FMT_DEBUG("���������ϱ������������.\n");

    /* ��¼�������� */
    oopPlanReportList_fj.oopPlanReportNum = oopPlanReportNum;

    PRTL_FMT_DEBUG("���ظ��������ϱ�������: %d\n", oopPlanReportList_fj.oopPlanReportNum);

    return;
}


/**********************************************************************
* @name      : app_get_plan_report_list
* @brief     ���ӻ����л�ȡ�ϱ���������
* @param[in] ��plan_reportID  - �ϱ��������          
* @param[out]: oopPlanReport - �ϱ���������
* @return    : ������
**********************************************************************/
int app_get_plan_report_list_fj(uint8 planID, OOP_TRANS_REPORT_T *oopPlanReport)
{
    int32 ret = ERR_NORMAL;
    int i = 0;

    if(NULL == oopPlanReport)
    {
        return ERR_PNULL;
    }

    /* ʹ�û��������м��� */
    pthread_mutex_lock(&oopPlanReportList_fj.mutex);

    for(i = 0; i < oopPlanReportList_fj.oopPlanReportNum; i++)
    {
        if(oopPlanReportList_fj.oopPlanReport[i].reportid == planID)
        {
            memcpy(oopPlanReport, &oopPlanReportList_fj.oopPlanReport[i], sizeof(OOP_TRANS_REPORT_T));
            break;
        }
    }

    /* ʹ�û��������н��� */
    pthread_mutex_unlock(&oopPlanReportList_fj.mutex);

    if(i < oopPlanReportList_fj.oopPlanReportNum)
    {
        ret = ERR_OK;
    }
    else
    {
        ret = ERR_INVALID;
    }

    return ret;
}

/**********************************************************************
* @name      : app_set_plan_report_list
* @brief     �������µ��ϱ���������д�뻺��
* @param[in] ��plan_reportID  - �ϱ��������          
               oopPlanReport - �ϱ���������
* @return    ��������
**********************************************************************/
int app_set_plan_report_list_fj(uint8 planID, OOP_TRANS_REPORT_T *oopPlanReport)
{
    int ret = ERR_OK;
    int i = 0;
    if(NULL == oopPlanReport)
    {
        return ERR_PNULL;
    }

    /* ʹ�û��������м��� */
    pthread_mutex_lock(&oopPlanReportList_fj.mutex);

    for(i = 0; i < oopPlanReportList_fj.oopPlanReportNum; i++)
    {
        if(oopPlanReportList_fj.oopPlanReport[i].reportid == planID)
        {
            memcpy(&oopPlanReportList_fj.oopPlanReport[i], oopPlanReport, sizeof(OOP_TRANS_REPORT_T));
            PRTL_FMT_DEBUG("[%d]�����ϱ�����-%d\n", i,   oopPlanReportList_fj.oopPlanReport[i].reportid);
            break;
        }
    }

    if(i == oopPlanReportList_fj.oopPlanReportNum)
    {
        if(oopPlanReportList_fj.oopPlanReportNum < 256)
        {
            memcpy(&oopPlanReportList_fj.oopPlanReport[oopPlanReportList_fj.oopPlanReportNum++], oopPlanReport, sizeof(OOP_TRANS_REPORT_T));
            PRTL_FMT_DEBUG("[%d]�����ϱ�����-%d\n", i,   oopPlanReportList_fj.oopPlanReport[i].reportid);
        }
        else
        {
            ret = ERR_NORESOURCE;
        }
    }

    /* ʹ�û��������н��� */
    pthread_mutex_unlock(&oopPlanReportList_fj.mutex);

    return ret;
}


/**********************************************************************
* @name      : app_set_plan_report_invalid
* @brief     �������ϱ�����������Ч
* @param[in] : planID - �ϱ��������
* @param[out]��
* @return    ��������
**********************************************************************/
int app_set_plan_report_invalid_fj(uint8 planID)
{
    int i = 0;
    
    /* ʹ�û��������м��� */
    pthread_mutex_lock(&oopPlanReportList_fj.mutex);

    for(i = 0; i < oopPlanReportList_fj.oopPlanReportNum; i++)
    {
        if(oopPlanReportList_fj.oopPlanReport[i].reportid == planID)
        {
            PRTL_FMT_DEBUG("[%d]ɾ���ϱ�����-%d\n", i,   oopPlanReportList_fj.oopPlanReport[i].reportid);
            if(i < oopPlanReportList_fj.oopPlanReportNum-1)
            {
                memmove(&oopPlanReportList_fj.oopPlanReport[i], &oopPlanReportList_fj.oopPlanReport[i+1], sizeof(OOP_TRANS_REPORT_T));
            }

            oopPlanReportList_fj.oopPlanReportNum--;
            break;
        }
    }

    /* ʹ�û��������н��� */
    pthread_mutex_unlock(&oopPlanReportList_fj.mutex);

    return ERR_OK;
}

/**********************************************************************
* @name      : appdb_get_plan_report_list
* @brief     ���ϱ������б�����
* @param[in] ��int clientid    socket���
               uint8 logicId   �߼���ַ
               uint16 infoNum  ��Ϣ���
               uint32 value    oadֵ
* @param[out]��
* @return    ��0-�ɹ�/������
**********************************************************************/
int appdb_get_plan_report_list_fj(int clientid, uint8 logicId, uint16 infoNum, uint32 value)
{
    int                ret = 0;
    uint32             readlen = 0;
    NOMAL_OAD_T        tOAD = {0};
    OOP_TRANS_REPORT_T         oopPlanReport = {0};
    uint32 i = 0;
    uint32 j = 0;

    if(0x6E170200 != value)
    {
        return -1;
    }

    tOAD.logicId   = logicId;
    tOAD.infoNum   = infoNum;
    tOAD.oad.value = value;

    PRTL_FMT_TRACE("appdb_get_plan_report_list logicId(%d) infoNum(%d) value(0x%x) running.....\n", logicId, infoNum, value);

    if (infoNum == 0xffff)
    {
        oopPlanReportList_fj.oopPlanReportNum = 0;   
        return 0;
    }

    ret = db_read_nomal(clientid, &tOAD, sizeof(OOP_TRANS_REPORT_T), (uint8 *)&oopPlanReport, &readlen);
    if(0 != ret || readlen != sizeof(OOP_TRANS_REPORT_T))
    {
        app_set_plan_report_invalid_fj(infoNum);
        PRTL_FMT_DEBUG("db_read_nomal failed. ret(%d), oad(0x%08x), infoNum(%d), readlen(%d), explen(%d)\n", ret, tOAD.oad.value, infoNum, readlen, sizeof(OOP_TRANS_REPORT_T));
        return ERR_NORMAL;
    }

    PRTL_FMT_TRACE("planID = %d\n", oopPlanReport.reportid);
    PRTL_FMT_TRACE("timeout.unit = %d\n", oopPlanReport.timeout.unit);
    PRTL_FMT_TRACE("timeout.value = %d\n", oopPlanReport.timeout.value);
    PRTL_FMT_TRACE("retry = %d\n", oopPlanReport.retry);
    PRTL_FMT_TRACE("data.choice = %d\n", oopPlanReport.data.choice);

    if(oopPlanReport.data.choice == 0)
    {
        PRTL_FMT_TRACE("data.oad = 0x%08x\n", oopPlanReport.data.oad.value);
    }
    else
    {
        //OAD
        PRTL_FMT_TRACE("data.record.oad = %d\n", oopPlanReport.data.record.oad.value);

        //RCSD
        for(i = 0; i < oopPlanReport.data.record.tRCSD.nNum; i++)
        {
            PRTL_FMT_TRACE("data.record.tRCSD.cols[%d].choice = %d\n", i , oopPlanReport.data.record.tRCSD.cols[i].choice);
            if(oopPlanReport.data.record.tRCSD.cols[i].choice == 0)
            {
                PRTL_FMT_TRACE("data.record.tRCSD.cols[%d].oad = 0x%08x\n", i, oopPlanReport.data.record.tRCSD.cols[i].oad.value); 
            }
            else
            {
                PRTL_FMT_TRACE(".data.record.tRCSD.cols[%d].road.oadMain = 0x%08x\n", j, oopPlanReport.data.record.tRCSD.cols[i].road.oadMain.value);
                for(j = 0; j < oopPlanReport.data.record.tRCSD.cols[i].road.oadCols.nNum; j++)
                {
                    PRTL_FMT_TRACE(".data.record.tRCSD.cols[%d].road.oadCols.oad[%d] = 0x%08x\n", i, j, oopPlanReport.data.record.tRCSD.cols[i].road.oadCols.oad[j].value);
                }
            }
        }

        //RSD
        PRTL_FMT_TRACE("data.record.tRSD.choice = %d\n", oopPlanReport.data.record.tRSD.choice);
    }
    
    ret = app_set_plan_report_list_fj(infoNum, &oopPlanReport);
    if (ret != ERR_OK)
    {
        app_set_plan_report_invalid_fj(infoNum);
        PRTL_FMT_DEBUG("app_set_plan_report_list failed. ret(%d), infoNum(%d)\n", ret, infoNum);
        return ERR_NORMAL;
    }
    
    return 0;
}

#endif

#if DESC("�����ӿ�" , 1)

//�����ṹ��
OOP_REPORT_TRIGGER_T oopReportTri = {0};

/**
*********************************************************************
* @brief��     ��������
* @param[in]��rptInf   ������Ϣ
* @return��    none
*********************************************************************
*/
void set_report_trigger(OOP_RPT_INF_T *pRptInf)
{
    int i = 0;
    
    if (pRptInf == NULL)
    {
        PRTL_FMT_DEBUG("parm null pRptInf[%p]\n", pRptInf);
        return;
    }
    
    /* ʹ�û��������м��� */
    pthread_mutex_lock(&oopReportTri.mutex);

    for(i = 0; i < pRptInf->num; i++)
    {
        if((oopReportTri.wtptr+1)%256 == oopReportTri.rdptr)
        {
            oopReportTri.rdptr = (oopReportTri.rdptr+1)%256;
        }
 
        oopReportTri.taskId[oopReportTri.wtptr] = pRptInf->taskId[i];
        oopReportTri.wtptr = (oopReportTri.wtptr+1)%256;
    }

    /* ʹ�û��������н��� */
    pthread_mutex_unlock(&oopReportTri.mutex);
    
    return;
}


/**********************************************************************
* @name      : get_report_trigger_rptinf
* @brief     ����ȡ������Ϣ������󣬱������
* @param[in] ��
* @param[out]��pRptInf ������Ϣ
* @return    ��������
**********************************************************************/
int get_report_trigger(OOP_RPT_INF_T *pRptInf)
{
    int i = 0;

    if (pRptInf == NULL)
    {
        PRTL_FMT_DEBUG("parm null pRptInf[%p]\n", pRptInf);
        return ERR_PTR;
    }

    /* ʹ�û��������м��� */
    pthread_mutex_lock(&oopReportTri.mutex);

    for(i = 0; i < 256; i++)
    {
        if(oopReportTri.wtptr == oopReportTri.rdptr)
        {
            break;
        }
        
        pRptInf->taskId[pRptInf->num++] = oopReportTri.taskId[oopReportTri.rdptr];
        oopReportTri.rdptr = (oopReportTri.rdptr+1)%256;
    }

    /* ʹ�û��������н��� */
    pthread_mutex_unlock(&oopReportTri.mutex);

    return ERR_OK;
}

//�¼�����
OOP_EVTRPT_TRIGGER_T oopEventRptTri = {0};

/**********************************************************************
* @name      : set_event_rpt_trigger
* @brief     �������¼�����
* @param[in] ��none
* @param[out]��none
* @return    ��NULL
**********************************************************************/
void set_event_rpt_trigger(void)
{
    /* ʹ�û��������м��� */
    pthread_mutex_lock(&oopEventRptTri.mutex);

    oopEventRptTri.triNum++;

    /* ʹ�û��������н��� */
    pthread_mutex_unlock(&oopEventRptTri.mutex);
    
    return;
}

/**********************************************************************
* @name      : get_event_rpt_trigger_state
* @brief     ����ȡ�¼�����״̬
* @param[in] ��NULL
* @param[out]��NULL
* @return    ��TRUE - �Ѿ�����; FALSE - û�д���
**********************************************************************/
uint8 get_event_rpt_trigger(void)
{
    uint8 triNum = 0;

    /* ʹ�û��������м��� */
    pthread_mutex_lock(&oopEventRptTri.mutex);

    triNum = oopEventRptTri.triNum;

    oopEventRptTri.triNum = 0;

    /* ʹ�û��������н��� */
    pthread_mutex_unlock(&oopEventRptTri.mutex);
    
    return triNum;
}

#endif

#if DESC("�ն˱��幦����չ������֯", 1)
extern void *appmsg_memcpy_r(void *dest, void *src, int count);
/**********************************************************************
* @name      : appmsg_wireinfo_split
* @brief     ���������߲��� ��ѯ��ȡ��ǰ�ź���Ϣ
* @param[in] ��uint8 *playload           ��Ϣ
			   uint16 len				��Ϣ����
* @param[out]��OOP_SINGAL_INFO_T *pOutInfo ��ǰ�ź���Ϣ
* @return    ��0-�ɹ�/������
* @Create    : 
* @Date      ��2020-1-19
* @Update    :
**********************************************************************/
int appmsg_wireinfo_split_singal(uint8 *playload, uint16 len, OOP_SINGAL_INFO_T *pOutInfo)
{
    uint8 *pTemp = playload;
    uint16 offset = 0;
	uint32 tmp32 = 0;
	int16  tmp16 = 0;
    
    if(NULL == playload || NULL == pOutInfo)
    {
        return -1;
    }

	if(playload[0] == 0)		//OPTIONAL
    {
        return -1;
    }

	if(len != 1+4+4+4+2+2)		//OPTIONAL + ��վ�� + С���� + Ƶ��� + ���չ��� + ���书��
    {
        return -1;
    }

    offset = 1;
    appmsg_memcpy_r(&tmp32, pTemp + offset, sizeof(uint32));
    offset += sizeof(uint32);
	pOutInfo->lac = (uint16)tmp32;

	appmsg_memcpy_r(&tmp32, pTemp + offset, sizeof(uint32));
    offset += sizeof(uint32);
	pOutInfo->cell_id = (uint16)tmp32;

	appmsg_memcpy_r(&tmp32, pTemp + offset, sizeof(uint32));
    offset += sizeof(uint32);
	pOutInfo->arfcn = (uint16)tmp32;

	appmsg_memcpy_r(&tmp16, pTemp + offset, sizeof(int16));
    offset += sizeof(int16);
	pOutInfo->rx_level = tmp16;

	appmsg_memcpy_r(&tmp16, pTemp + offset, sizeof(int16));
    offset += sizeof(int16);
	pOutInfo->power = tmp16;

    return 0;
}

/**********************************************************************
* @name      : appmain_get_ppp_mode
* @brief     �����߲��Ų�ѯ��ȡ��ǰ�ź���Ϣ
* @param[in] ��uint8 no       ģ��id 1-2
               uint16 buflen  �����С
* @param[out]��uint8* recvbuf ����
* @return    ����Ϣ����
* @Date      ��2020-5-29
* @Update    :
**********************************************************************/
uint16 appmain_get_singal_info(uint8 no, uint8* recvbuf, uint16 buflen)
{
    uint16 recLen = 0;
    int    ret = 0;
	uint8 rBuf[2048] = {0};
	
	if(buflen < sizeof(OOP_SINGAL_INFO_T))
	{
		return 0;
	}
	
	OOP_SINGAL_INFO_T *pData = (OOP_SINGAL_INFO_T *)recvbuf;

    ret = appmain_send_syncmsg(MSG_APPMAIN_GETSINGLEINFO, &no, sizeof(no), rBuf, 2048, &recLen);
    if (ret != ERR_OK || 0 == recLen)
    {
        PRTL_FMT_DEBUG("appmain_get_singal_info err! ret = %d, recLen = %d\n", ret, recLen);
        return 0;
    }
	
    ret = appmsg_wireinfo_split_singal(rBuf, recLen, pData);
    if (ret != 0)
    {
        PRTL_FMT_DEBUG("appmsg_wireinfo_split_singal err! ret = %d, recLen = %d\n", ret, recLen);
        return 0;
    }
	
    return sizeof(OOP_SINGAL_INFO_T);
}

/**********************************************************************
* @name      : appmain_send_powerup_evnet
* @brief     �������ն��ϵ���Ϣ��taskManager
* @param[in] ��
* @param[out]��
* @return    ��
* @Date      ��2022-6-27
* @Update    :
**********************************************************************/
uint16 appmain_send_powerup_evnet(void)
{
    uint16 recLen = 0;
    int    ret = 0;
	uint8 rBuf[2048] = {0};

    ret = appmain_send_syncmsg(MSG_APPAMIN_POWERUP_EVT, NULL, 0, rBuf, 2048, &recLen);
    if (ret != ERR_OK || 0 == recLen)
    {
        PRTL_FMT_DEBUG("appmain_send_powerup_evnet err! ret = %d, recLen = %d\n", ret, recLen);
        return 0;
    }
	
    return recLen;
}

#endif

#if DESC("д����ʣ��ʱ�����", 1)
uint8 g_writeProtectLeft = 255;
pthread_mutex_t g_fwriteProteclock;

/**********************************************************************
* @name      : area_write_protect_init
* @brief     ��д����ʣ��ʱ���ʼ��
* @param[in] ��DB_CLIENT fd        �������Ŀͻ���
* @param[out]��
* @return    ��
* @Create    : ����
* @Date      ��2022-04-13
* @Update    :
**********************************************************************/
void area_write_protect_init(DB_CLIENT fd)
{
    int             ret = -1;
    uint32          datalen = 0;
    NOMAL_OAD_T     nomalOAD;
    
    memset(&nomalOAD,0,sizeof(NOMAL_OAD_T));

    pthread_mutex_init(&g_fwriteProteclock, NULL);
    //�¼���ǰֵ��¼
    nomalOAD.oad.value = 0x6e000300;
    ret = db_read_nomal(fd, &nomalOAD, 1, &g_writeProtectLeft, &datalen);
    if(0 != ret || 1 != datalen)
    {
        g_writeProtectLeft = 255;
        APP698_FMT_DEBUG("FUJIAN write protect left time used default\n");
    }
    APP698_FMT_DEBUG("FUJIAN write protect left time %dmin\n", g_writeProtectLeft);
}

/**********************************************************************
* @name      : area_write_protect_reduce
* @brief     ��д����ʣ��ʱ�����
* @param[in] ��DB_CLIENT fd        �������Ŀͻ���
               uint8 type         ����: 0-����ֵ, 1-������
               uint8 reducemin    ����ʱ��(����)
* @param[out]��
* @return    ��
* @Create    : ����
* @Date      ��2022-04-13
* @Update    :
**********************************************************************/
int area_write_protect_reduce(DB_CLIENT fd, uint8 type, uint8 reducemin)
{
    int   ret = 0;
    NOMAL_OAD_T normal;

    if(1 == type && (0 == reducemin || 255 == g_writeProtectLeft || 0 == g_writeProtectLeft))
    {
        return 0;
    }
    
    PTHREAD_LOCK_SAFE(&g_fwriteProteclock);
    if(0 == type)
    {
        g_writeProtectLeft = reducemin;
    }
    else if(255 != g_writeProtectLeft)
    {
        if(g_writeProtectLeft >= reducemin)
        {
            g_writeProtectLeft -= reducemin;
        }
        else
        {
            g_writeProtectLeft = 0;
        }
    }
    memset(&normal, 0, sizeof(NOMAL_OAD_T));
    normal.oad.value = 0x6e000300;
    normal.classtag = CLASS_DATA_INIT;
    ret = db_write_nomal(fd, &normal, &g_writeProtectLeft, sizeof(uint8));
    PTHREAD_UNLOCK_SAFE(&g_fwriteProteclock);

    return ret;
}

/**********************************************************************
* @name      : area_write_protect_leftproc
* @brief     ��д����ʣ��ʱ��ά��
* @param[in] ��DB_CLIENT fd        �������Ŀͻ���
* @param[out]��
* @return    ��
* @Create    : ����
* @Date      ��2022-04-13
* @Update    :
**********************************************************************/
void area_write_protect_leftproc(DB_CLIENT fd)
{
    static uint8 s_first_protect = TRUE;
    static time_t  s_last_protect_t = 0;
    static uint32  s_last_pass_min = 0;
    struct timespec curtime = {0};
    uint32  pass_min = 0;
    int     ret = 0;
    
    clock_gettime(CLOCK_MONOTONIC, &curtime);
    if(TRUE == s_first_protect)
    {
        s_last_protect_t = curtime.tv_sec;
        s_last_pass_min = 0;
        s_first_protect = FALSE;
        return;
    }

    /* ʱ����� */
    pass_min = (curtime.tv_sec - s_last_protect_t)/60;
    if(pass_min > s_last_pass_min)
    {
        ret = area_write_protect_reduce(fd, 1, pass_min - s_last_pass_min);
        if(0 != ret)
        {
            APP698_FMT_DEBUG("FUJIAN reduce protect failed, ret %d\n", ret);
        }
    }
    s_last_pass_min = pass_min;
}

/**********************************************************************
* @name      : area_check_set_protect
* @brief     ��д�����ж�(����)
* @param[in] ��uint8 *pMsgOAD     ������OAD����
* @param[out]��
* @return    ��TRUE-����/FALSE-������
* @Create    : ����
* @Date      ��2022-04-13
* @Update    :
**********************************************************************/
uint8 area_check_set_protect(uint32 uOAD)
{
    if(g_writeProtectLeft > 0 || 0x6e000300 == uOAD)
    {
        return FALSE;
    }

    return TRUE;
}

/**********************************************************************
* @name      : area_check_act_protect
* @brief     ��д�����ж�(����)
* @param[in] ��uint8 *pMsgOAD     ������OAD����
* @param[out]��
* @return    ��TRUE-����/FALSE-������
* @Create    : ����
* @Date      ��2022-04-13
* @Update    :
**********************************************************************/
uint8 area_check_act_protect(uint32 uOAD)
{
    if(g_writeProtectLeft > 0 || 0x6e178400 == uOAD || 0x6e318000 == uOAD)
    {
        return FALSE;
    }

    return TRUE;
}


#endif

