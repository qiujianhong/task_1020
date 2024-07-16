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

//将版本信息以全局变量的形式写入各个地区的代码中，统一在 area.h 中引用 
#ifdef ADV_PU_AMR
FACTORY_VER_TMP_T g_tVer = 
{
    "XXXX",      /**< 厂商代码           */
    "CF22",      /**< 软件版本号         */
    "220812",    /**< 软件版本日期       */
    "CC01",      /**< 硬件版本号         */
    "220818",    /**< 硬件版本日期       */
    "CF22-01",   /**< 厂家扩展信息       */
};
#elif ADV_ST_AMR
    #ifdef PRODUCT_ECU
    //能源控制器
    FACTORY_VER_TMP_T g_tVer = 
    {
        "XXXX",      /**< 厂商代码           */
        "EF01",      /**< 软件版本号         */
        "220619",    /**< 软件版本日期       */
        "ER01",      /**< 硬件版本号         */
        "220619",    /**< 硬件版本日期       */
        "EF01-01",   /**< 厂家扩展信息       */
    };
    #elif PRODUCT_ZCU
    //专变
    FACTORY_VER_TMP_T g_tVer = 
    {
        "XXXX",      /**< 厂商代码           */
        "1F01",      /**< 软件版本号         */
        "210207",    /**< 软件版本日期       */
        "Z101",      /**< 硬件版本号         */
        "200624",    /**< 硬件版本日期       */
        "1F01-01",   /**< 厂家扩展信息       */
    };
    #elif PRODUCT_ZCU_2
    //专变3型
    FACTORY_VER_TMP_T g_tVer = 
    {
        "XXXX",      /**< 厂商代码           */
        "2F01",      /**< 软件版本号         */
        "220715",    /**< 软件版本日期       */
        "Z301",      /**< 硬件版本号         */
        "220715",    /**< 硬件版本日期       */
        "2F01-01",   /**< 厂家扩展信息       */
    };
    #elif PRODUCT_ZCU_3
    //专变3型
    FACTORY_VER_TMP_T g_tVer = 
    {
        "XXXX",      /**< 厂商代码           */
        "3F01",      /**< 软件版本号         */
        "220716",    /**< 软件版本日期       */
        "Z301",      /**< 硬件版本号         */
        "220715",    /**< 硬件版本日期       */
        "3F01-01",   /**< 厂家扩展信息       */
    };
    #endif
#endif


/* 任务的缓存 */
OOP_TASK_LIST_FJ_T oopTaskList_fj;

/* 上报方案的缓存 */
OOP_PLAN_REPORT_LIST_FJ_T oopPlanReportList_fj;

#if DESC("默认参数处理",1)

// 普通参数，列出APDU，长度，数量
const uint8 setDefaultPara[][LOCAL_RECV_FRAM_MAXLEN] = 
{
    {0x06, 0x01, 0x00, 0x43, 0x00, 0x08, 0x00, 0x03, 0x01, 0x00},  //设置终端主动上报
    {0x06, 0x01, 0x01, 0x40, 0x00, 0x03, 0x00, 0x16, 0x01, 0x00},  //设置默认校时模式为心跳校时

    {0x06, 0x01, 0x01, 0x31, 0x00, 0x09, 0x00, 0x03, 0x01, 0x00},  //0x31000900 //终端初始化 有效
    {0x06, 0x01, 0x02, 0x31, 0x00, 0x08, 0x00, 0x16, 0x03, 0x00},  //0x31000800 //终端初始化 上报    

    {0x06, 0x01, 0x01, 0x31, 0x14, 0x09, 0x00, 0x03, 0x01, 0x00},  //0x31140900 //终端对时 有效
    {0x06, 0x01, 0x02, 0x31, 0x14, 0x08, 0x00, 0x16, 0x03, 0x00},  //0x31140800 //终端对时 上报   

    {0x06, 0x01, 0x01, 0x31, 0x01, 0x09, 0x00, 0x03, 0x01, 0x00},  //0x31010900 //终端版本变更 有效
    {0x06, 0x01, 0x02, 0x31, 0x01, 0x08, 0x00, 0x16, 0x03, 0x00},  //0x31010800 //终端版本变更 上报

    {0x06, 0x01, 0x01, 0x31, 0x1E, 0x09, 0x00, 0x03, 0x01, 0x00},  //0x311E0900 //参数变更 有效

    {0x06, 0x01, 0x01, 0x31, 0x19, 0x09, 0x00, 0x03, 0x01, 0x00},  //0x31190900 //电流回路异常 有效

    {0x06, 0x01, 0x01, 0x31, 0x05, 0x09, 0x00, 0x03, 0x01, 0x00},  //0x31050900 //电能表时钟超差 有效
    {0x06, 0x01, 0x01, 0x31, 0x05, 0x08, 0x00, 0x16, 0x03, 0x00},  //0x31050800 //电能表时钟超差 上报

    {0x06, 0x01, 0x01, 0x31, 0x06, 0x09, 0x00, 0x03, 0x01, 0x00},  //0x31060900 //终端停/上电 有效
    {0x06, 0x01, 0x02, 0x31, 0x06, 0x08, 0x00, 0x16, 0x03, 0x00},  //0x31060800 //终端停/上电 上报

    {0x06, 0x01, 0x01, 0x31, 0x07, 0x09, 0x00, 0x03, 0x01, 0x00},  //0x31070900 //直流模拟量越上限 有效

    {0x06, 0x01, 0x01, 0x31, 0x08, 0x09, 0x00, 0x03, 0x01, 0x00},  //0x31080900 //直流模拟量越下限 有效

    {0x06, 0x01, 0x01, 0x31, 0x09, 0x09, 0x00, 0x03, 0x01, 0x00},  //0x31090900 //终端消息认证错误 有效

    {0x06, 0x01, 0x01, 0x31, 0x0A, 0x09, 0x00, 0x03, 0x01, 0x00},  //0x310A0900 //设备故障 有效

    {0x06, 0x01, 0x01, 0x31, 0x0B, 0x09, 0x00, 0x03, 0x01, 0x00},  //0x310B0900 //电能表示度下降 有效

    {0x06, 0x01, 0x01, 0x31, 0x0C, 0x09, 0x00, 0x03, 0x01, 0x00},  //0x310C0900 //电能量超差 有效

    {0x06, 0x01, 0x01, 0x31, 0x0D, 0x09, 0x00, 0x03, 0x01, 0x00},  //0x310D0900 //电能表飞走 有效

    {0x06, 0x01, 0x01, 0x31, 0x0E, 0x09, 0x00, 0x03, 0x01, 0x00},  //0x310E0900 //电能表停走 有效
    {0x06, 0x01, 0x01, 0x40, 0x00, 0x04, 0x00, 0x02, 0x05, 0x11, 0x0A, 0x11, 0x02, 0x11, 0x02, 0x11, 0x01, 0x11, 0x03, 0x00},  //40000400 心跳校时参数

    {0x06, 0x01, 0x01, 0x31, 0x0F, 0x09, 0x00, 0x03, 0x01, 0x00},  //0x310F0900 //终端抄表失败 有效

    {0x06, 0x01, 0x01, 0x31, 0x10, 0x09, 0x00, 0x03, 0x01, 0x00},  //0x31100900 //月通信流量超限 有效

    {0x06, 0x01, 0x01, 0x31, 0x11, 0x09, 0x00, 0x03, 0x01, 0x00},  //0x31110900 //发现未知电能表 有效
	
    {0x06, 0x01, 0x02, 0x31, 0x18, 0x08, 0x00, 0x16, 0x03, 0x00},  //0x31180800 //终端编程事件 上报
    {0x06, 0x01, 0x01, 0x31, 0x18, 0x09, 0x00, 0x03, 0x01, 0x00},  //0x31180900 //终端编程事件 有效

};
const uint16 setDefaultParaLen[] = 
{
    10, //对应上面第一条APDU长度
    10, //对应上面第二条APDU长度
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

//将上面的几个变量填入下表，即可下发默认配置
const APDU_DEF_CFG_T apduDefCfgList[] = 
{
    /* 使用SET(0)还是ACT(1)服务 APDU                           APDULEN                          APDUNUM*/
    {1,                         (uint8 *)setDefaultRptSche,   (uint16 *)setDefaultRptScheLen,   (uint32 *)&setDefaultRptScheNum},
    {0,                         (uint8 *)setDefaultPara,      (uint16 *)setDefaultParaLen,      (uint32 *)&setDefaultParaNum},
    {0xFF,                      NULL,                          NULL,                            NULL},
};

/**********************************************************************
* @name      : set_by_protocol_eare
* @brief     ：进程启动 设置地区默认参数
* @param[in] ：int sockdbfd    句柄
* @param[out]：
* @return    ：
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
* @brief     ：进程启动 设置地区默认参数
* @param[in] ：int sockdbfd    句柄
* @param[out]：
* @return    ：
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
* @brief     ：进程启动 统一接口
* @param[in] ：int sockdbfd    句柄
* @param[out]：
* @return    ：
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
* @brief     ：进程启动 设置地区默认参数
* @param[in] ：int sockdbfd    句柄
* @param[out]：
* @return    ：
**********************************************************************/
void app_setdefault_cfg_fujian(int sockdbfd)
{
    set_act_by_protocol_fujian(sockdbfd);

    return;
}

#endif

#if DESC("透明事件处理",1)

OOP_GATHER_E		gGather;							//外部设备数据采集模式
OOP_TRANS_EVENT_STATUS_T gTransEvtStatus;				//透明事件当前状态

extern OOP_OCTETVAR16_T    tmnAddr;                            //终端通信地址
extern int rpt_task_package(OOP_OAD_U *pChOAD);
extern void evt_report_state_update(uint8 *pevt_data, OOP_REPORTS_T state);
extern OOP_DAR_E record_response_make(OOP_OAD_U oad, OOP_RCSD_T rcsd, DATA_CONVERT_T *convert);
extern uint16 rpt_packet_make(uint8 *pApdu, uint16 apduLen, uint8 choice, uint8 cAddr, uint8 piid, uint8 *outBuf, uint16 outLen);
extern int evt_get_oop_struct_data(mtrnum_t mtr_id, EVENT_COMM_INFO *eventCommInfo, OOP_EVTSOURCE_T eventSource, OOP_EVTBUF_T eventData, uint32 max_len, uint8 *pout_buf, uint8 bType);
extern int rpt_event_write(int clientid, void* data, uint16 datalen);

/**********************************************************************
* @brief     ：获取外部设备数据采集模式
* @param[in] ：

* @param[out]：
* @return    ：
* @Create    : 梅安宁
* @Date      ：2022-03-22
* @Update    :
**********************************************************************/
OOP_GATHER_E get_tmn_gather()
{
    return gGather;
}

/**********************************************************************
* @name      : WriteEventReportData
* @brief     ：写终端事件主动上报私有文件
* @param[in] ：const char *filename   事件上报私有文件名称
               OOP_OAD_U eventOAD     事件OAD
               uint8 *bDatabuf        事件内容
               uint32 bufLen          事件内容长度
* @param[out]：写入事件上报私有文件
* @return    ：0 成功， <0 失败
* @Create    : 梅安宁
* @Date      ：2020-01-11
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
	
	//上报到在线安全监测系统
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

    //构造事件上报报文
    record_response_make(eventCommInfo->eventOAD, eventCommInfo->eventRCSD, &temp);

    *pConvert->dOffset = rpt_packet_make(pEvtAPDU, evtAPDULEN, 1, cAddr, 0, pConvert->dstBuf, pConvert->dstLen);
    //读出目前记录总条数
    
    return 0;
}

/**********************************************************************
* @name      : SetEventStat
* @brief     ：终端事件当前值记录
* @param[in] ：mtrnum_t          mtr_id            测量点号
               EVENT_COMM_INFO *eventCommInfo     事件基础信息
               OOP_EVTSOURCE_T  eventSource       事件发生源
               uint8            bType             事件状态 0(只记录事件，无发生，恢复区分)，1发生，2恢复        
* @param[out]：   
* @return    ：0 成功， <0 失败
* @Create    : 梅安宁
* @Date      ：2020-03-03
* @Update    :
**********************************************************************/
int evt_set_status_FJ(DB_CLIENT fd, OOP_TRANS_EVENT_STATUS_T *evtStatus)
{
    int             ret     = -1;
    NOMAL_OAD_T     nomalOAD;
    
    memset(&nomalOAD,0,sizeof(NOMAL_OAD_T));

    //事件当前值记录
    nomalOAD.oad.value = 0x6e310300;
    nomalOAD.classtag = CLASS_DATA_EVENT;
    ret = db_write_nomal(fd, &nomalOAD, (uint8 *)evtStatus, sizeof(OOP_TRANS_EVENT_STATUS_T));
    if (ret != ERR_OK)
    {
        EVENT_FMT_DEBUG("db_write_nomal oad[%08X]记录事件当前值失败 返回%d\n", nomalOAD.oad.value, ret);
    }
    return ret;
}


/**********************************************************************
* @name      : evt_write_todb_FJ
* @brief     ：终端事件写入数据库
* @param[in] ：DB_CLIENT fd            数据中心句柄
               EVENT_COMM_INFO *eventCommInfo     事件基础信息
               uint8 *pevt_data       事件数据
               uint32 data_len        事件数据长度
               OOP_REPORTS_T state    事件通道上报状态
* @param[out]：   
* @return    ：0 成功， <0 失败
* @Create    : 梅安宁
* @Date      ：2021-2-4
* @Update    :
**********************************************************************/
int evt_write_todb_FJ(DB_CLIENT fd, EVENT_COMM_INFO *eventCommInfo, uint8 *pevt_data, uint32 data_len)
{  
    time_t              timep = 0;
    WRITE_RECORD_T      recInData;   
    
    memset(&recInData,0,sizeof(WRITE_RECORD_T));

    
    time(&timep); /*获取time_t类型的当前时间*/
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
* @brief     ：终端事件写入数据库
* @param[in] ：mtrnum_t          mtr_id            测量点号
               EVENT_COMM_INFO *eventCommInfo     事件基础信息
               OOP_EVTSOURCE_T  eventSource       事件发生源
               OOP_EVTBUF_T     eventData         事件关联对象数据
               uint8            bType             事件状态 0(只记录事件，无发生，恢复区分)，1发生，2恢复        
* @param[out]：   
* @return    ：0 成功， <0 失败
* @Create    : 梅安宁
* @Date      ：2022-03-22
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

    //上报状态
    state.nNum = rpt_task_package(&state.state[0].oad);

    EVENT_FMT_DEBUG("eventCommInfo->eventNum is %d,eventCommInfo->recordnum is %d\n", eventCommInfo->eventNum, eventCommInfo->recordnum);
    bufLen = evt_get_oop_struct_data(mtr_id, eventCommInfo, eventSource, eventData, DATA_LEN_MAX, bDatabuf, bType);
	if(bufLen == 0)
    {
        return ERR_O_RANGE;
    }

	evt_report_state_update(bDatabuf, state);

	//构造事件上报报文
	temp.srcBuf = bDatabuf;
	temp.srcLen = bufLen;
	temp.sOffset = &tmpOffset;
	
	temp.dOffset = &transEvt.data.nNum;
	temp.dstBuf = transEvt.data.value;
	temp.dstLen = 2048;

	evt_packet_make(eventCommInfo, &temp);

	//透明事件其他内容
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
    if(bType <= 1)//发生
    {
        if(bType == 1) //电能表相关
        {
            //记录存储序号和事件开始时间
            write_pdata_xram("^Event", (char *)&eventCommInfo->storagenum, (eventCommInfo->EventStoragNumRecAddr+(mtr_id)*(4)), 4);
            time(&timep); //获取time_t类型的当前时间
            write_pdata_xram("^Event", (char *)&timep, (eventCommInfo->EventStoragTmRecAddr+(mtr_id)*(4)), 4);

			//电能表相关事件，同一时期，可以有多个事件发生和恢复，记录序号需要存储
			if(eventCommInfo->EventStoragIndexAddr != 0)
			{
				write_pdata_xram("^Event", (char *)&eventCommInfo->recordnum, (eventCommInfo->EventStoragIndexAddr+(mtr_id)*(4)), 4);
			}
        }

    }
*/
	gTransEvtStatus.lastRecvNo++;
	gTransEvtStatus.recvSum++;

    //事件当前值记录
    evt_set_status_FJ(fd, &gTransEvtStatus);

    //终端上报在上报线程中实现
    if(g_tnmEventReport == 1 && eventCommInfo->reportflag > EVENT_NO_REPORT)
    {
        //只上报恢复事件
        if(bType <= 1 && eventCommInfo->reportflag == EVENT_RESTORE_REPORT)
            return ERR_OK;
            
        //只上报发生事件
        if(bType == 2 && eventCommInfo->reportflag == EVENT_HAPPENED_REPORT )
            return ERR_OK;
		
        rpt_event_write(fd, &transEvt, sizeof(OOP_TRANS_EVENT_T));
        EVENT_BUF_DEBUG(&transEvt, sizeof(OOP_TRANS_EVENT_T),"上报数据：");

    }
    return ERR_OK;
}

extern DB_CLIENT g_rptUdpHandle; //TODO
/**********************************************************************
* @name      : rpt_mtr_event_msg
* @brief     ：福建电表事件上报
* @param[in] ：ack   确认消息
* @return    ：返回消息长度
* @Create    : 贺宁
* @Date      ：2020-04-20
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

    //端口号
    memcpy_r(&transEvt.oad, &revcbuf[offset], sizeof(uint32));
    offset += sizeof(uint32);

    //通信地址
    transEvt.add.len = 5;
    memcpy(transEvt.add.add, &revcbuf[offset], transEvt.add.len+1); 
    offset += transEvt.add.len + 1;

    //通信类型
    transEvt.commType = revcbuf[offset++];

    //数据帧
    transEvt.data.nNum = get_len_offset(&revcbuf[offset], &offlen);
    offset += offlen;

    if(transEvt.data.nNum == 0)
    {
        RPT_FMT_DEBUG("电表事件上报 无内容.\n");
        return;
    }
    
    memcpy(&transEvt.data.value[0], &revcbuf[offset], transEvt.data.nNum); 
    offset += transEvt.data.nNum;

    if(offset > msglen)
    {
        RPT_FMT_DEBUG("电表事件上报: offset(%d) > msglen(%d).\n", offset, msglen);
        return;
    }

    //获取事件数量和记录序号, 没弄明白这两者的区别，目前用一样的
    eventCommInfo.eventNum = gTransEvtStatus.recvSum;
    eventCommInfo.recordnum = gTransEvtStatus.lastRecvNo;

    transEvt.curNum = gTransEvtStatus.recvSum+1;
    transEvt.selfStoreNo = gTransEvtStatus.lastRecvNo+1;
    current_tm_to_ooptm(&gTransEvtStatus.lastRecvTime);
    transEvt.reportTime = gTransEvtStatus.lastRecvTime;

    ret = evt_write_todb_FJ(clientid, &eventCommInfo, (uint8 *)&transEvt, sizeof(OOP_TRANS_EVENT_T));
    if (ret != ERR_OK)
    {
        RPT_FMT_DEBUG("写透明事件记录集失败 返回%d\n", ret);
        return;
    }

    gTransEvtStatus.lastRecvNo++;
    gTransEvtStatus.recvSum++;

    //事件当前值记录
    evt_set_status_FJ(clientid, &gTransEvtStatus);
    
    if(g_tnmEventReport == 1)
    {
        rpt_event_write(clientid, &transEvt, sizeof(OOP_TRANS_EVENT_T));
    }

    return;
}

#endif
#if DESC("终端停电处理",1)
#define BATTERY_VOL			360		//电池电压3.6V

/** @brief 对象属性描述符    */
typedef union POWER_STATUS
{
    uint16          status;              
    struct
    {
        struct
        {
			uint8   bit7  : 1;         /**< 保留     */
			uint8   bit6  : 1;         /**< 保留     */
			uint8   bit5  : 1;         /**< 停电超级电容     */
			uint8   bit4  : 1;         /**< 时钟电池     */
			uint8   bit3  : 1;         /**< 停电工作电池     */
			uint8   bit12 : 2;         /**< 供电方式     */
			uint8   bit0  : 1;         /**< 保留    */
        };
        uint8       value2; 
    };
}POWER_STATUS_T;

/**********************************************************************
* @name      : get_battery_vol
* @brief     ：获取工作电池电压
* @param[in] ：  
* @param[out]：vol: 电压
* @return    ：1 成功， <0 失败
* @Create    : 梅安宁
* @Date      ：2020-07-27
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
* @brief     ：检查终端电池状态插拔状态
* @param[in] ：status 		1 有电池 ，0 电池被拔出
* @param[out]：
* @return    ：0 成功， <0 失败
* @Create    : 梅安宁
* @Date      ：2022-06-25
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

    /* 读掉电检测状态 */
    if(read(fd, &uData, sizeof(uData)) 
        != sizeof(uData))
    {
       iRet = ERR_IO;
       goto end;
    }

	*status = (uData>>8)&0x01;

	//实际调试发现，拔出为1，查入为0
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
* @brief     ：检查终端电源状态
* @param[in] ：int    clientid     数据中心句柄  
			   state: 电源状态，1有电，-1 掉电
* @param[out]：
* @return    ：1 成功， <0 失败
* @Create    : 梅安宁
* @Date      ：2020-07-27
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
	
	if(state == 1)		//主电源供电
	{
        if(pStatus->bit12 != 0)
		{
		    pStatus->bit12 = 0;
		    change  = 1;
		}
	}

	if(state == -1)		//电池供电
	{
        if(pStatus->bit12 != 2)
		{
		    pStatus->bit12 = 2;
		    change  = 1;
		}
	}

	if(vol < BATTERY_VOL)		//电池欠压
	{
        if(pStatus->bit3 != 1)
		{
		    pStatus->bit3 = 1;
		    change  = 1;
		}
	}
	
	if(vol >= BATTERY_VOL)		//电池正常
	{
        if(pStatus->bit3 != 0)
		{
		    pStatus->bit3 = 0;
		    change  = 1;
		}
	}

	if(change == 1)		//状态变更
	{
        evt_write_nomal_data(clientid, 0, 0, 0x4e010200, (uint8 *)&event_3106.powerStatus, sizeof(OOP_BITSTR16_T));
		EVENT_FMT_TRACE("CheckERC_3106 检测终端停电事件%02X%02X %04X \n", 
         event_3106.powerStatus.nValue[0], event_3106.powerStatus.nValue[1], pStatus->status);
	}
	
	return 1;
}

extern int PowerDownCheck(void);

/**********************************************************************
* @name      : check_power_status
* @brief     ：检查停电工作电池状态,终端启动时
* @param[in] ：int    clientid     数据中心句柄  
			   state: 电源状态，1有电，0 掉电
			   vol: 电压
* @param[out]：
* @return    ：1 成功， <0 失败
* @Create    : 梅安宁
* @Date      ：2020-07-27
* @Update    :
**********************************************************************/
int check_battery_status(int clientid)
{
	uint16 vol = 0;
	uint8 change = 0;

	get_battery_vol(&vol);

	if(event_3106.batteryStatus.voltage != vol)		//电压改变
	{
		event_3106.batteryStatus.voltage = vol;
        change  = 1;
	}
	
	if(event_3106.tmFlag == 0xaa)		//电池电容设为0, 当终端由于停电工作电池电量不足，3分钟未到即停止工作
	{

		if (PowerDownCheck() < 180)		//终端停电没到3分钟就上电，不处理
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
	else if(event_3106.tmFlag == 0x55)		//电池电容设为100
	{
        if(event_3106.batteryStatus.capacity != 100)
		{
		    event_3106.batteryStatus.capacity = 100;
		    change  = 1;
		}
	}
    
	if(change == 1)		//状态变更
	{
        evt_write_nomal_data(clientid, 0, 0, 0x4e020200, (uint8 *)&event_3106.batteryStatus, sizeof(OOP_BATTERY_STATUS_T));
	}
	return 1;
}


/**********************************************************************
* @name      : check_power_status
* @brief     ：检查停电工作电池状态,终端运行时
* @param[in] ：int    clientid     数据中心句柄  
			   state: 电源状态，1有电，0 掉电
			   vol: 电压
* @param[out]：
* @return    ：1 成功， <0 失败
* @Create    : 梅安宁
* @Date      ：2020-07-27
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

	if(ret < 0)		//失败
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
* @brief     ：终端停电事件写入数据库，在终端停电一分钟后或上电立即记录上报
* @param[in] ：DB_CLIENT fd		数据库句柄      
			   uint32 tmnDownAddr 终端停电事件停电时间存储私有地址
* @param[out]：   
* @return    ：0 成功， <0 失败
* @Create    : 梅安宁
* @Date      ：2022-03-22
* @Update    :
**********************************************************************/
int evt3106_write_data_FJ(DB_CLIENT fd, uint32 tmnDownAddr)
{
	OOP_EVTSOURCE_T eventSource; //事件发生源
    OOP_EVTBUF_T    eventData;   //事件关联数据 
    
    memset(&eventSource,0,sizeof(OOP_EVTSOURCE_T));
    memset(&eventData,0,sizeof(OOP_EVTBUF_T));  

	int  ret = 0;

//	read_pdata_xram("^Event", (char *)&event_3106.tmnStarttime, tmnDownAddr, sizeof(OOP_DATETIME_S_T));

    //事件发生源
    eventSource.choice = DT_ENUM;
    eventSource.nValue = 0;             //停电

    //事件关联数据
    
	//当前判断为停电且未写过停电记录
	event_3106.eventFlag = 0x80;
	//写数据库
    ret = WriteEventToDB(fd, 0, &event_3106.eventCommInfo, eventSource, eventData, 0);
    if (ret < ERR_OK)
    {
        EVENT_FMT_DEBUG("CheckERC_3106 终端停电事件记录失败返回%d\n", ret);
        return ret;
    }
    EVENT_FMT_TRACE("CheckERC_3106 终端停电事件记录成功, storagenum=%u !\n", event_3106.eventCommInfo.storagenum);
	return ret;
}

#endif

#if DESC("任务参数变更" ,1)

/**********************************************************************
* @name      : task_list_lock_init
* @brief     ：
* @param[out]：
* @return    ：
* @Create    : 贺宁
* @Date      ：2020-1-10
* @Update    :
**********************************************************************/
void task_list_lock_init_fj(void)
{
    /* 初始化锁 */
    memset(&oopTaskList_fj, 0, sizeof(OOP_TASK_LIST_FJ_T));
    pthread_mutex_init(&oopTaskList_fj.mutex, NULL);
}

/**
*********************************************************************
* @name：      appdb_task_list_record_init
* @brief：     任务参数变更初始化(记录数据方式)
* @param[in] ：clientid  数据库句柄
* @param[out]：
* @return    ：
* @author    : 
* @Date      ：2019-12-6
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

    PRTL_FMT_DEBUG("加载福建全局透明任务方案.\n");
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

    //不需要附加校验
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

            //检查数据有效性
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

            //序号从1开始
            oopTaskList_fj.oopTask[oopTaskNum++] = oopTask;

        }
    }
    db_read_record_end(clientid, handle);
    
    PRTL_FMT_DEBUG("福建全局透明任务方案加载完成.\n");

    /* 记录任务数量 */
    oopTaskList_fj.oopTaskNum = oopTaskNum;

    PRTL_FMT_DEBUG("加载福建全局透明任务方案数: %d\n", oopTaskList_fj.oopTaskNum);

    return;
}

/**********************************************************************
* @name      : app_get_task_list
* @brief     ：从缓存中获取任务数据
* @param[in] ：taskID  - 任务序号          
* @param[out]: oopTask - 任务数据
* @return    : 错误码
**********************************************************************/
int app_get_task_list_fj(uint16 taskID, OOP_TRANS_TASK_T *oopTask)
{
    int32 ret = ERR_NORMAL;
    int i = 0;

    if(NULL == oopTask)
    {
        return ERR_PNULL;
    }

    /* 使用互斥量进行加锁 */
    pthread_mutex_lock(&oopTaskList_fj.mutex);

    for(i = 0; i < oopTaskList_fj.oopTaskNum; i++)
    {
        if(oopTaskList_fj.oopTask[i].taskid == taskID)
        {
            memcpy(oopTask, &oopTaskList_fj.oopTask[i], sizeof(OOP_TRANS_TASK_T));
            break;
        }
    }

    /* 使用互斥量进行解锁 */
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
* @brief     ：通过数组ID获取档案数据
* @param[in] ：meterID        - 档案内部编号
* @param[out]：oopMeter - 档案
* @return    : 错误码
* @Create    : 贺宁
* @Date      ：2019-12-16
* @Update    :
**********************************************************************/
int app_get_task_list_fj_by_id(uint16 innerID, OOP_TRANS_TASK_T *oopTask)
{
    uint16 oopTaskNum = oopTaskList_fj.oopTaskNum;

    if((NULL == oopTask)||(innerID == 0xffff) || (innerID >= oopTaskNum))
    {
        return ERR_PNULL;
    }

    /* 使用互斥量进行加锁 */
    pthread_mutex_lock(&oopTaskList_fj.mutex);

    *oopTask = oopTaskList_fj.oopTask[innerID];

    /* 使用互斥量进行解锁 */
    pthread_mutex_unlock(&oopTaskList_fj.mutex);

    return ERR_OK;    
}

/**********************************************************************
* @name      : app_set_task_list
* @brief     ：将更新的任务数据写入缓存
* @param[in] ：taskID  - 任务序号          
               oopTask - 任务数据
* @return    ：错误码
**********************************************************************/
int app_set_task_list_fj(uint16 taskID, OOP_TRANS_TASK_T *oopTask)
{
    int ret = ERR_OK;
    int i = 0;
    if(NULL == oopTask)
    {
        return ERR_PNULL;
    }

    /* 使用互斥量进行加锁 */
    pthread_mutex_lock(&oopTaskList_fj.mutex);

    for(i = 0; i < oopTaskList_fj.oopTaskNum; i++)
    {
        if(oopTaskList_fj.oopTask[i].taskid == taskID)
        {
            memcpy(&oopTaskList_fj.oopTask[i], oopTask, sizeof(OOP_TRANS_TASK_T));
            PRTL_FMT_DEBUG("[%d]更新透明任务方案-%d\n", i,         oopTaskList_fj.oopTask[i].taskid);
            break;
        }
    }

    if(i == oopTaskList_fj.oopTaskNum)
    {
        if(oopTaskList_fj.oopTaskNum < 256)
        {
            memcpy(&oopTaskList_fj.oopTask[oopTaskList_fj.oopTaskNum++], oopTask, sizeof(OOP_TRANS_TASK_T));
            PRTL_FMT_DEBUG("[%d]新增透明任务方案-%d\n", i,         oopTaskList_fj.oopTask[i].taskid);
        }
        else
        {
            ret = ERR_NORESOURCE;
        }
    }

    /* 使用互斥量进行解锁 */
    pthread_mutex_unlock(&oopTaskList_fj.mutex);

    return ret;
}

/**********************************************************************
* @name      : app_set_task_invalid
* @brief     ：设置任务数据无效
* @param[in] : taskID - 任务序号
* @param[out]：
* @return    ：错误码
**********************************************************************/
int app_set_task_invalid_fj(uint16 taskID)
{
    int i = 0;
    
    /* 使用互斥量进行加锁 */
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
            PRTL_FMT_DEBUG("[%d]删除透明任务方案-%d\n", i,         oopTaskList_fj.oopTask[i].taskid);
            break;
        }
    }

    /* 使用互斥量进行解锁 */
    pthread_mutex_unlock(&oopTaskList_fj.mutex);

    return ERR_OK;
}

/**********************************************************************
* @name      : appdb_get_task_list
* @brief     ：任务列表到缓存
* @param[in] ：int clientid    socket句柄
               uint8 logicId   逻辑地址
               uint16 infoNum  信息点号
               uint32 value    oad值
* @param[out]：
* @return    ：0-成功/错误码
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

    PRTL_FMT_TRACE("透明任务方案更新 logicId(%d) infoNum(%d) value(0x%x)\n", logicId, infoNum, value);

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

#if DESC("上报方案参数变更" ,1)

/**********************************************************************
* @name      : plan_report_list_lock_init
* @brief     ：
* @param[out]：
* @return    ：
* @Create    : 贺宁
* @Date      ：2020-1-10
* @Update    :
**********************************************************************/
void plan_report_list_lock_init_fj(void)
{
    /* 初始化锁 */
    memset(&oopPlanReportList_fj, 0, sizeof(OOP_PLAN_REPORT_LIST_FJ_T));
    pthread_mutex_init(&oopPlanReportList_fj.mutex, NULL);

    memset(&oopReportTri, 0, sizeof(OOP_REPORT_TRIGGER_T));
    pthread_mutex_init(&oopReportTri.mutex, NULL);

    memset(&oopEventRptTri, 0, sizeof(OOP_EVTRPT_TRIGGER_T));
    pthread_mutex_init(&oopEventRptTri.mutex, NULL);
}

/**
*********************************************************************
* @name：      appdb_plan_report_list_record_init
* @brief：     上报方案参数变更初始化(记录数据方式)
* @param[in] ：clientid  数据库句柄
* @param[out]：
* @return    ：
* @author    : 
* @Date      ：2019-12-6
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

    PRTL_FMT_DEBUG("加载福建主动上报方案.\n");
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

    //不需要附加校验
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

            //检查数据有效性
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

            //序号从1开始
            oopPlanReportList_fj.oopPlanReport[oopPlanReportNum++] = oopPlanReport;

        }
    }
    db_read_record_end(clientid, handle);
    
    PRTL_FMT_DEBUG("福建主动上报方案加载完成.\n");

    /* 记录任务数量 */
    oopPlanReportList_fj.oopPlanReportNum = oopPlanReportNum;

    PRTL_FMT_DEBUG("加载福建主动上报方案数: %d\n", oopPlanReportList_fj.oopPlanReportNum);

    return;
}


/**********************************************************************
* @name      : app_get_plan_report_list
* @brief     ：从缓存中获取上报方案数据
* @param[in] ：plan_reportID  - 上报方案序号          
* @param[out]: oopPlanReport - 上报方案数据
* @return    : 错误码
**********************************************************************/
int app_get_plan_report_list_fj(uint8 planID, OOP_TRANS_REPORT_T *oopPlanReport)
{
    int32 ret = ERR_NORMAL;
    int i = 0;

    if(NULL == oopPlanReport)
    {
        return ERR_PNULL;
    }

    /* 使用互斥量进行加锁 */
    pthread_mutex_lock(&oopPlanReportList_fj.mutex);

    for(i = 0; i < oopPlanReportList_fj.oopPlanReportNum; i++)
    {
        if(oopPlanReportList_fj.oopPlanReport[i].reportid == planID)
        {
            memcpy(oopPlanReport, &oopPlanReportList_fj.oopPlanReport[i], sizeof(OOP_TRANS_REPORT_T));
            break;
        }
    }

    /* 使用互斥量进行解锁 */
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
* @brief     ：将更新的上报方案数据写入缓存
* @param[in] ：plan_reportID  - 上报方案序号          
               oopPlanReport - 上报方案数据
* @return    ：错误码
**********************************************************************/
int app_set_plan_report_list_fj(uint8 planID, OOP_TRANS_REPORT_T *oopPlanReport)
{
    int ret = ERR_OK;
    int i = 0;
    if(NULL == oopPlanReport)
    {
        return ERR_PNULL;
    }

    /* 使用互斥量进行加锁 */
    pthread_mutex_lock(&oopPlanReportList_fj.mutex);

    for(i = 0; i < oopPlanReportList_fj.oopPlanReportNum; i++)
    {
        if(oopPlanReportList_fj.oopPlanReport[i].reportid == planID)
        {
            memcpy(&oopPlanReportList_fj.oopPlanReport[i], oopPlanReport, sizeof(OOP_TRANS_REPORT_T));
            PRTL_FMT_DEBUG("[%d]更新上报方案-%d\n", i,   oopPlanReportList_fj.oopPlanReport[i].reportid);
            break;
        }
    }

    if(i == oopPlanReportList_fj.oopPlanReportNum)
    {
        if(oopPlanReportList_fj.oopPlanReportNum < 256)
        {
            memcpy(&oopPlanReportList_fj.oopPlanReport[oopPlanReportList_fj.oopPlanReportNum++], oopPlanReport, sizeof(OOP_TRANS_REPORT_T));
            PRTL_FMT_DEBUG("[%d]新增上报方案-%d\n", i,   oopPlanReportList_fj.oopPlanReport[i].reportid);
        }
        else
        {
            ret = ERR_NORESOURCE;
        }
    }

    /* 使用互斥量进行解锁 */
    pthread_mutex_unlock(&oopPlanReportList_fj.mutex);

    return ret;
}


/**********************************************************************
* @name      : app_set_plan_report_invalid
* @brief     ：设置上报方案数据无效
* @param[in] : planID - 上报方案序号
* @param[out]：
* @return    ：错误码
**********************************************************************/
int app_set_plan_report_invalid_fj(uint8 planID)
{
    int i = 0;
    
    /* 使用互斥量进行加锁 */
    pthread_mutex_lock(&oopPlanReportList_fj.mutex);

    for(i = 0; i < oopPlanReportList_fj.oopPlanReportNum; i++)
    {
        if(oopPlanReportList_fj.oopPlanReport[i].reportid == planID)
        {
            PRTL_FMT_DEBUG("[%d]删除上报方案-%d\n", i,   oopPlanReportList_fj.oopPlanReport[i].reportid);
            if(i < oopPlanReportList_fj.oopPlanReportNum-1)
            {
                memmove(&oopPlanReportList_fj.oopPlanReport[i], &oopPlanReportList_fj.oopPlanReport[i+1], sizeof(OOP_TRANS_REPORT_T));
            }

            oopPlanReportList_fj.oopPlanReportNum--;
            break;
        }
    }

    /* 使用互斥量进行解锁 */
    pthread_mutex_unlock(&oopPlanReportList_fj.mutex);

    return ERR_OK;
}

/**********************************************************************
* @name      : appdb_get_plan_report_list
* @brief     ：上报方案列表到缓存
* @param[in] ：int clientid    socket句柄
               uint8 logicId   逻辑地址
               uint16 infoNum  信息点号
               uint32 value    oad值
* @param[out]：
* @return    ：0-成功/错误码
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

#if DESC("补报接口" , 1)

//补报结构体
OOP_REPORT_TRIGGER_T oopReportTri = {0};

/**
*********************************************************************
* @brief：     触发补报
* @param[in]：rptInf   补报信息
* @return：    none
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
    
    /* 使用互斥量进行加锁 */
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

    /* 使用互斥量进行解锁 */
    pthread_mutex_unlock(&oopReportTri.mutex);
    
    return;
}


/**********************************************************************
* @name      : get_report_trigger_rptinf
* @brief     ：获取触发信息，读完后，被清除。
* @param[in] ：
* @param[out]：pRptInf 触发信息
* @return    ：错误码
**********************************************************************/
int get_report_trigger(OOP_RPT_INF_T *pRptInf)
{
    int i = 0;

    if (pRptInf == NULL)
    {
        PRTL_FMT_DEBUG("parm null pRptInf[%p]\n", pRptInf);
        return ERR_PTR;
    }

    /* 使用互斥量进行加锁 */
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

    /* 使用互斥量进行解锁 */
    pthread_mutex_unlock(&oopReportTri.mutex);

    return ERR_OK;
}

//事件补报
OOP_EVTRPT_TRIGGER_T oopEventRptTri = {0};

/**********************************************************************
* @name      : set_event_rpt_trigger
* @brief     ：触发事件补报
* @param[in] ：none
* @param[out]：none
* @return    ：NULL
**********************************************************************/
void set_event_rpt_trigger(void)
{
    /* 使用互斥量进行加锁 */
    pthread_mutex_lock(&oopEventRptTri.mutex);

    oopEventRptTri.triNum++;

    /* 使用互斥量进行解锁 */
    pthread_mutex_unlock(&oopEventRptTri.mutex);
    
    return;
}

/**********************************************************************
* @name      : get_event_rpt_trigger_state
* @brief     ：获取事件触发状态
* @param[in] ：NULL
* @param[out]：NULL
* @return    ：TRUE - 已经触发; FALSE - 没有触发
**********************************************************************/
uint8 get_event_rpt_trigger(void)
{
    uint8 triNum = 0;

    /* 使用互斥量进行加锁 */
    pthread_mutex_lock(&oopEventRptTri.mutex);

    triNum = oopEventRptTri.triNum;

    oopEventRptTri.triNum = 0;

    /* 使用互斥量进行解锁 */
    pthread_mutex_unlock(&oopEventRptTri.mutex);
    
    return triNum;
}

#endif

#if DESC("终端本体功能扩展内容组织", 1)
extern void *appmsg_memcpy_r(void *dest, void *src, int count);
/**********************************************************************
* @name      : appmsg_wireinfo_split
* @brief     ：解析无线拨号 查询获取当前信号信息
* @param[in] ：uint8 *playload           消息
			   uint16 len				消息长度
* @param[out]：OOP_SINGAL_INFO_T *pOutInfo 当前信号信息
* @return    ：0-成功/错误码
* @Create    : 
* @Date      ：2020-1-19
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

	if(len != 1+4+4+4+2+2)		//OPTIONAL + 基站号 + 小区号 + 频点号 + 接收功率 + 发射功率
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
* @brief     ：无线拨号查询获取当前信号信息
* @param[in] ：uint8 no       模块id 1-2
               uint16 buflen  缓存大小
* @param[out]：uint8* recvbuf 缓存
* @return    ：消息长度
* @Date      ：2020-5-29
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
* @brief     ：发送终端上电消息给taskManager
* @param[in] ：
* @param[out]：
* @return    ：
* @Date      ：2022-6-27
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

#if DESC("写保护剩余时间控制", 1)
uint8 g_writeProtectLeft = 255;
pthread_mutex_t g_fwriteProteclock;

/**********************************************************************
* @name      : area_write_protect_init
* @brief     ：写保护剩余时间初始化
* @param[in] ：DB_CLIENT fd        数据中心客户端
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2022-04-13
* @Update    :
**********************************************************************/
void area_write_protect_init(DB_CLIENT fd)
{
    int             ret = -1;
    uint32          datalen = 0;
    NOMAL_OAD_T     nomalOAD;
    
    memset(&nomalOAD,0,sizeof(NOMAL_OAD_T));

    pthread_mutex_init(&g_fwriteProteclock, NULL);
    //事件当前值记录
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
* @brief     ：写保护剩余时间减少
* @param[in] ：DB_CLIENT fd        数据中心客户端
               uint8 type         类型: 0-决绝值, 1-减少至
               uint8 reducemin    减少时间(分钟)
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2022-04-13
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
* @brief     ：写保护剩余时间维护
* @param[in] ：DB_CLIENT fd        数据中心客户端
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2022-04-13
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

    /* 时间计算 */
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
* @brief     ：写保护判断(设置)
* @param[in] ：uint8 *pMsgOAD     报文中OAD部分
* @param[out]：
* @return    ：TRUE-保护/FALSE-不保护
* @Create    : 王津剑
* @Date      ：2022-04-13
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
* @brief     ：写保护判断(操作)
* @param[in] ：uint8 *pMsgOAD     报文中OAD部分
* @param[out]：
* @return    ：TRUE-保护/FALSE-不保护
* @Create    : 王津剑
* @Date      ：2022-04-13
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

