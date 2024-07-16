
#include "baseCommon.h"
#include "oadInfoList.h"
#include "oopservice.h"
#include "commClass.h"
#include "oopStorage.h"
#include "area.h"
#include "appcomm.h"
#include "oopservice.h"
#include "appmsg_struct.h"
#include "appmsg_split.h"
#include "statmain.h"
#include "msg_id.h"


//将版本信息以全局变量的形式写入各个地区的代码中，统一在 area.h 中引用
#ifdef ADV_PU_AMR
FACTORY_VER_TMP_T g_tVer = 
{
    "XXXX",      /**< 厂商代码           */
    "CU01",      /**< 软件版本号         */
    "220623",    /**< 软件版本日期       */
    "CC01",      /**< 硬件版本号         */
    "220621",    /**< 硬件版本日期       */
    "CU01-01",   /**< 厂家扩展信息       */
};
#elif ADV_ST_AMR
    #ifdef PRODUCT_ECU
    //能源控制器
    FACTORY_VER_TMP_T g_tVer = 
    {
        "XXXX",      /**< 厂商代码           */
        "EU01",      /**< 软件版本号         */
        "220619",    /**< 软件版本日期       */
        "ER01",      /**< 硬件版本号         */
        "220619",    /**< 硬件版本日期       */
        "EU01-01",   /**< 厂家扩展信息       */
    };
    #elif PRODUCT_ZCU_1
    //专变1型
    FACTORY_VER_TMP_T g_tVer = 
    {
        "XXXX",      /**< 厂商代码           */
        "1U01",      /**< 软件版本号         */
        "220809",    /**< 软件版本日期       */
        "Z101",      /**< 硬件版本号         */
        "220727",    /**< 硬件版本日期       */
        "1U01-03",   /**< 厂家扩展信息       */
    };
    #elif PRODUCT_ZCU_2
    //专变2型
    FACTORY_VER_TMP_T g_tVer = 
    {
        "XXXX",      /**< 厂商代码           */
        "2U01",      /**< 软件版本号         */
        "220715",    /**< 软件版本日期       */
        "Z301",      /**< 硬件版本号         */
        "220715",    /**< 硬件版本日期       */
        "2U01-01",   /**< 厂家扩展信息       */
    };
    #elif PRODUCT_ZCU_3
    //专变3型
    FACTORY_VER_TMP_T g_tVer = 
    {
        "XXXX",      /**< 厂商代码           */
        "3U01",      /**< 软件版本号         */
        "220716",    /**< 软件版本日期       */
        "Z301",      /**< 硬件版本号         */
        "220715",    /**< 硬件版本日期       */
        "3U01-01",   /**< 厂家扩展信息       */
    };
     #elif PRODUCT_ZCU_N
    //QT专变
    FACTORY_VER_TMP_T g_tVer = 
    {
        "XXXX",      /**< 厂商代码           */
        "NU01",      /**< 软件版本号         */
        "210207",    /**< 软件版本日期       */
        "ZN01",      /**< 硬件版本号         */
        "200624",    /**< 硬件版本日期       */
        "NU01-01",   /**< 厂家扩展信息       */
    };
    #endif
#endif


#if DESC("默认参数",1)

// 普通参数，列出APDU，长度，数量
const uint8 setDefaultPara[][LOCAL_RECV_FRAM_MAXLEN] = 
{
    {0x06, 0x01, 0x00, 0x43, 0x00, 0x08, 0x00, 0x03, 0x01, 0x00},  //设置终端主动上报
    {0x06, 0x01, 0x01, 0x31, 0x06, 0x09, 0x00, 0x03, 0x01, 0x00},  //0x31060900
    {0x06, 0x01, 0x02, 0x31, 0x06, 0x08, 0x00, 0x16, 0x03, 0x00},  //0x31060800
    {0x06, 0x01, 0x01, 0x31, 0x98, 0x09, 0x00, 0x03, 0x01, 0x00},  //0x31980900
    {0x06, 0x01, 0x02, 0x31, 0x98, 0x08, 0x00, 0x16, 0x03, 0x00},  //0x31980800
    {0x06, 0x01, 0x01, 0x31, 0x99, 0x09, 0x00, 0x03, 0x01, 0x00},  //0x31990900
    {0x06, 0x01, 0x02, 0x31, 0x99, 0x08, 0x00, 0x16, 0x01, 0x00},  //0x31990800
    {0x06, 0x01, 0x3d, 0x31, 0x06, 0x06, 0x00, 0x02, 0x02, 0x02, 0x04, 0x04, 0x08, 0xc0, 0x11, 0x00, 0x11, 
     0x05, 0x01, 0x00, 0x02, 0x06, 0x12, 0x00, 0x01, 0x12, 0x10, 0xe0, 0x12, 0x00, 0x0f, 0x12, 0x00, 0x05, 
     0x12, 0x05, 0x28, 0x12, 0x06, 0xe0, 0x00},                                                             //停上电参数
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
    41,
};
const uint32 setDefaultParaNum = sizeof(setDefaultPara)/LOCAL_RECV_FRAM_MAXLEN;


//将上面的几个变量填入下表，即可下发默认配置
const APDU_DEF_CFG_T apduDefCfgList[] = 
{
    /* 使用SET(0)还是ACT(1)服务 APDU                           APDULEN                          APDUNUM*/
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
* @name      : set_act_by_protocol_shaanxi
* @brief     ：进程启动 统一接口
* @param[in] ：int sockdbfd    句柄
* @param[out]：
* @return    ：
**********************************************************************/
void set_act_by_protocol_shaanxi(int sockdbfd)
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
* @name      : app_setdefault_cfg_shaanxi
* @brief     ：进程启动 设置地区默认参数
* @param[in] ：int sockdbfd    句柄
* @param[out]：
* @return    ：
**********************************************************************/
void app_setdefault_cfg_shaanxi(int sockdbfd)
{
    set_act_by_protocol_shaanxi(sockdbfd);

    return;
}
#endif


#if DESC("ICCID变更事件",1)
extern int IniEventCommInfo(EVENT_COMM_INFO *eventCommInfo);

/**********************************************************************
* @brief     ：ICCID 变更事件初始化
* @param[in] ：
* @param[out]：
* @return    ：
**********************************************************************/
int Init3199Para()
{

    memset(&event_3199, 0, sizeof(EVENT_3199_INFO));

    event_3199.eventCommInfo.eventOAD.nObjID= 0X3199;
    event_3199.eventCommInfo.eventOAD.value= 0X31990200;


    OOP_RCSD_T eventRCSD={
        7, 0,{
                {0,{{0X20220200},}},//事件记录序号
                {0,{{0X201E0200},}},//事件发生时间
                {0,{{0X20200200},}},//事件结束时间
                {0,{{0X20240200},}},//事件发生源
                {0,{{0X33000200},}},//通道上报状态

                {0,{{0x33990206},}},//变更前 ICCID 
                {0,{{0x33990207},}},//变更后 ICCID
            }
    };

    event_3199.eventCommInfo.eventRCSD = eventRCSD;

    // 公共信息
    IniEventCommInfo(&event_3199.eventCommInfo);

	//获取 ICCID
	evt_read_nomal_data(gDbCenterClient, 0, 0, 0x45000700, sizeof(OOP_VISIBLEVAR32_T), (uint8 *)&event_3199.iccidBef);
	//终端有ICCID信息，不是第一次插卡
	if(event_3199.iccidBef.nNum != 0)
	{
		event_3199.isFirst = 1;
	}
 
    return ERR_OK;
}

/**********************************************************************
* @brief     ：获取无线拨号ICCID信息
* @param[in] ： CHANNEL_CACHE_T *ptChannel   通道句柄
				
* @param[out]：pcurICCID	当前ICCID号
* @return    ：0成功, <0失败
**********************************************************************/
int get_ICCID_info(CHANNEL_CACHE_T *ptChannel, OOP_VISIBLEVAR32_T* pcurICCID)
{
	uint8  buf[480] = {0};
    uint16 msglen = 0;
    WIRE_DIAG_INFO_T tDiagInfo;

	uint8 no		= 0;       //模块id 1-2

	//无线通道1,  内部排号 0x10-0x29         无线通道2   内部排号 0x30-0x49
	if(ptChannel->id >= CHL_GPRS_1 && ptChannel->id < CHL_GSM_1)
	{
		no = 1;
	}
	else if(ptChannel->id >= CHL_GPRS_2 && ptChannel->id < CHL_GSM_2)
	{
		no = 2;
	}
	else
	{
		EVENT_FMT_DEBUG("0x%02x Is not wireless channel !\n", ptChannel->id);
		return -1;
	}

    msglen = appmain_get_ppp_diag(no, buf, 480);
    if(0 == msglen)
    {
        EVENT_FMT_DEBUG("getICCIDInfo ppp_diag failed, no[%u]\n", no);
        return -1;
    }
    memset(&tDiagInfo, 0, sizeof(WIRE_DIAG_INFO_T));
    appmsg_wirediag_split(buf, &tDiagInfo);


	if(tDiagInfo.simstatus != 1)
    {
        EVENT_FMT_DEBUG("simstatus is error\n");
        return -1;
    }

	pcurICCID->nNum = tDiagInfo.CCID.num>32? 32 : tDiagInfo.CCID.num;
	memcpy(pcurICCID->value, tDiagInfo.CCID.value, pcurICCID->nNum);
	return 0;
}

/**********************************************************************
* @brief     ：ICCID 变更事件
* @param[in] ：CHANNEL_CACHE_T *ptChannel   通道句柄
* @param[out]：
* @return    ：
**********************************************************************/
int CheckERC_3199(CHANNEL_CACHE_T *ptChannel)
{
    int ret;
	OOP_VISIBLEVAR32_T curICCID;

	EVENT_FMT_TRACE("CheckERC_3199 starts\n");
	
    if(!event_3199.eventCommInfo.checkflag)
    {
        EVENT_FMT_DEBUG("CheckERC_3199 checkflag 0\n");
        return -1;
    }

	//获取ICCID信息
	memset(&curICCID, 0, sizeof(OOP_VISIBLEVAR32_T));
	ret = get_ICCID_info(ptChannel, &curICCID);
	if(ret<0)
    {
        return -1;
    }

	//ICCID没有获取到,不判
	if(curICCID.nNum == 0)
    {
        return -1;
    }

	EVENT_FMT_TRACE("CheckERC_3199 curICCID: %s\n", curICCID.value);

	//第一次连接主站, 或数据初始化 不上报变更事件
	if(event_3199.isFirst == 0)
    {
    	event_3199.iccidBef.nNum = curICCID.nNum;
        memcpy(event_3199.iccidBef.value, curICCID.value, curICCID.nNum);
		event_3199.isFirst = 1;
		evt_write_nomal_data(ptChannel->clientid, 0, 0, 0x45000700, (uint8 *)&curICCID, sizeof(OOP_VISIBLEVAR32_T));
        return 0;
    }

    if(memcmp(event_3199.iccidBef.value, curICCID.value, 32) ==0)
    {
        //ICCID号未改变
        return 0;
    }
	else
	{
		EVENT_FMT_DEBUG("CheckERC_3199  iccidBef %s, curICCID: %s\n", event_3199.iccidBef.value, curICCID.value);
		event_3199.iccidCur.nNum = curICCID.nNum;
        memcpy(event_3199.iccidCur.value, curICCID.value, curICCID.nNum);
    }

	OOP_EVTSOURCE_T     eventSource; //事件发生源
	OOP_EVTBUF_T        eventData;   //事件关联数据
	memset(&eventSource, 0, sizeof(OOP_EVTSOURCE_T));
	memset(&eventData, 0, sizeof(OOP_EVTBUF_T));
	
    //事件发生源 NULL
	eventSource.choice = DT_NULL;
	
	ret = WriteEventToDB(ptChannel->clientid, 0, &event_3199.eventCommInfo, eventSource, eventData, 0);
    if (ret < ERR_OK)
	{
		EVENT_FMT_DEBUG("CheckERC_3199 记录失败返回%d\n", ret);
		return -1;
	}

	//修改上次ICCID号
	event_3199.iccidBef.nNum = curICCID.nNum;
    memcpy(event_3199.iccidBef.value, curICCID.value, curICCID.nNum);
	evt_write_nomal_data(ptChannel->clientid, 0, 0, 0x45000700, (uint8 *)&curICCID, sizeof(OOP_VISIBLEVAR32_T));
	return 0;
}

/**
*********************************************************************
* @brief：     数据中心格式转698报文，跨台区电能表事件 0x31120200
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_ICCID(const OAD_INFO_T *pOadInfo, void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32     offset = *pOffset;
    uint8 *dstBufTmp = (uint8 *)dstBuf;

    /*入参检查*/
    if ((pOadInfo == NULL) || (srcData == NULL) || (dstBuf == NULL))
    {
        PRTL_FMT_DEBUG("data_to_buf_refusenode param err\n");
        return ERR_PTR;
    }

    if (pOadInfo->pTab->eData != E_OOP_ICCIDLOG_T)
    {
        PRTL_FMT_DEBUG("check data type failed.\n");
        return DATA_TYPE_UNMATCHED;
    }

    OOP_ICCID_CHANGE_EVENT_T info;
    memset(&info,0x00,sizeof(OOP_ICCID_CHANGE_EVENT_T));

    if (srcLen != sizeof(OOP_ICCID_CHANGE_EVENT_T))
    {
        PRTL_FMT_DEBUG("check data len failed.\n");
        return DATA_TYPE_UNMATCHED;
    }

    memcpy(&info, srcData, srcLen);

    //结构数据类型
    dstBufTmp[offset++] = DT_STRUCTURE;

    //结构成员个数
    dstBufTmp[offset++] = 8 + info.data.oadNum;

    EVT_BASIC_T evtBasic;
    evtBasic.nIndex = info.nIndex;
    evtBasic.tmStart = info.tmStart;
    evtBasic.tmEnd = info.tmEnd;
    evtBasic.source = info.source;
    evtBasic.status = info.evtState;

    //*********************************************以下为事件记录单元的公共数据******************************************/
    dar = data_to_buf_evt_basic(pOadInfo, dstBufTmp, dstLen-offset, &offset, &evtBasic, sizeof(evtBasic));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("data_to_buf_evt_basic failed. dar(%d), offset(%d)\n", dar, offset);
        return DATA_TYPE_UNMATCHED;
    }

    //*********************************************以下为不同事件的特殊数据******************************************/

    //事件发生前ICCID
    dar = basic_data_to_buf(E_OOP_VISIBLEVAR32_T, dstBufTmp, dstLen-offset, &offset, &info.iccidBef, sizeof(OOP_VISIBLEVAR32_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), iccidBef(%s)\n", 
            dar, info.iccidBef.value);
        return dar;
    }
    
    //事件发生后ICCID
    dar = basic_data_to_buf(E_OOP_VISIBLEVAR32_T, dstBufTmp, dstLen-offset, &offset, &info.iccidCur, sizeof(OOP_VISIBLEVAR32_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), iccidCur(%s)\n", 
            dar, info.iccidCur.value);
        return dar;
    }

    *pOffset = offset;

    return dar;
}

int event_tmn_ICCID(OAD_INFO_T *pOadInfo, RECORD_UNIT_T unit, RELY_INFO_T *relyInfo, uint8 **ppOutBuf, uint32 *outLen)
{
    int32 nRet = ERR_OK;
    OOP_ICCID_CHANGE_EVENT_T event;    
    uint32 i = 0 ,j = 0;
    OOP_OAD_U tmpOAD;
    uint32 offset = 0;
    uint8 *pOutBuf  = NULL;
    uint32 tmpOutLen = DATA_LEN_MAX; //TODO 暂时这么定
    OAD_INFO_T pstOdaInfo = {0};  
    OOP_RCSD_T oopRcsd = relyInfo->oopRcsd;
    //uint8      offlen = 0;
    //uint8      lenBuf[3] = {0};
    uint32 tmpOffset = 0;

    memcpy(&event, unit.record[0].outDataBuf, sizeof(event));

    pOutBuf = (uint8 *)malloc(DATA_LEN_MAX);//TODO 暂时这么定
    memset(pOutBuf, 0, DATA_LEN_MAX);

    //NORMAL方法读，或者RECORD读出所有列
    if((relyInfo->method == 1) || (oopRcsd.nNum == 0))
    {

        nRet = data_to_buf_ICCID(pOadInfo, pOutBuf, DATA_LEN_MAX, &offset, &event, sizeof(event));
        if(nRet != 0)
        {
            PRTL_FMT_DEBUG("data_to_buf_impeover failed. oda(0x%08x)\n", pOadInfo->pTab->oad.value);
            return nRet;
        }

        if(relyInfo->method != 1) //非NORAML方法，去掉STRUCTER类型和长度
        {
            memmove(pOutBuf, pOutBuf + 2, offset - 2);
            *outLen = offset - 2;
        }
        else
        {
            *outLen = offset;
        }

        *ppOutBuf = pOutBuf;
        
        return nRet;
    }

    for(i = 0; i < oopRcsd.nNum; i++)
    {
        tmpOAD = oopRcsd.cols[i].oad;

        if(!class_oadinfo_get(tmpOAD, &pstOdaInfo))
        {
            return ERR_NORMAL;
        }
        
        switch(tmpOAD.value)
        {
            case 0x20220200:   //事件记录序号
            {
                if(basic_data_to_buf(E_DOUBLE_LONG_UNSIGNED, pOutBuf, tmpOutLen-offset, &offset, &event.nIndex, sizeof(uint32)) < 0)
                {
                    nRet = ERR_NORMAL;
                    return nRet;
                } 
            }break;
            case 0x201E0200:   //事件发生时间
            {
                if(basic_data_to_buf(E_DATETIME_S, pOutBuf, tmpOutLen-offset, &offset, &event.tmStart, sizeof(OOP_DATETIME_S_T)) < 0)
                {
                    nRet = ERR_NORMAL;
                    return nRet;
                }
            }break;
            case 0x20200200:   //事件结束时间
            {
                if(event.tmEnd.year == 0) //姑且使用0作为不存在的判据
                {
                   pOutBuf[offset++] = DT_NULL; 
                }
                else
                {
                    if(basic_data_to_buf(E_DATETIME_S, pOutBuf, tmpOutLen-offset, &offset, &event.tmEnd, sizeof(OOP_DATETIME_S_T)) < 0)
                    {
                        nRet = ERR_NORMAL;
                        return nRet;
                    }  
                }           
            }break;
            case 0x20240200:   //事件发生源
            {
                if(data_to_buf_evt_source(&pstOdaInfo, pOutBuf, tmpOutLen-offset, &offset, &event.source, sizeof(OOP_EVTSOURCE_T)) < 0)
                {
                    nRet = ERR_NORMAL;
                    return nRet;
                } 
            }break;            
            case 0x33000200:   //事件上报状态
            {
                if(data_to_buf_evt_report(&pstOdaInfo, pOutBuf, tmpOutLen-offset, &offset, &event.evtState, sizeof(OOP_REPORTS_T)) < 0)
                {
                    nRet = ERR_NORMAL;
                    return nRet;
                }             
            }break;

            case 0x33990206:   //事件发生前ICCID
            {
                if(basic_data_to_buf(E_OOP_VISIBLEVAR32_T, pOutBuf, tmpOutLen-offset, &offset, &event.iccidBef, sizeof(OOP_VISIBLESTR20_T)) < 0)
                {
                    nRet = ERR_NORMAL;
                    return nRet;
                } 
            }break;

            case 0x33990207:   //事件发生后ICCID
            {
                if(basic_data_to_buf(E_OOP_VISIBLEVAR32_T, pOutBuf, tmpOutLen-offset, &offset, &event.iccidCur, sizeof(OOP_VISIBLESTR20_T)) < 0)
                {
                    nRet = ERR_NORMAL;
                    return nRet;
                } 
            }break;    
            default: break;
        }

        tmpOffset = 0;
        for(j = 0; j < event.data.oadNum; j++)
        {
            if(tmpOAD.value == event.data.cols.oad[j].value)
            {
                //nRet = normal_data2fmt698(icInfo, &offset, &event.data.buf[tmpOffset], event.data.idLen[j], pOutBuf, tmpOutLen-offset);
                nRet = data_to_buf_evt_oad(tmpOAD, pOutBuf, tmpOutLen-offset, &offset, &event.data.buf[tmpOffset], event.data.idLen[j]);

                break;
            }

            tmpOffset += event.data.idLen[j];
        }        
   }
    
    *ppOutBuf = pOutBuf;
    *outLen = offset;

    return nRet;
}

#endif

#if DESC("冻结远程通信信道信息处理函数",1)
typedef struct tag_FROZEN_WIRELESS_INFO
{
	int16			 		csqvalue;       //信号强度
    OOP_4GMONITOR_INFO_T	monitorInfo;	//4G 下的监测相关信息
    int16        			ber;            //误码率
	OOP_OCTETSTR4_T			ci;				//小区编号
	uint8            		networkType;    //网络制式
}FROZEN_WIRELESS_INFO_T;

FROZEN_WIRELESS_INFO_T g_frozenWireless;

extern OOP_DATETIME_S_T   realstattime;     /*系统实时时间,全局时间变量*/

//冻结参数列表
EventUpdateDataInfo g_frozenwWirelessList[] = 
{/*     数据标识,   数据长度,     数据内容 */
    {0x45000900,    sizeof(int16),                  (uint8*)&g_frozenWireless.csqvalue},
    {0x45001600,    sizeof(OOP_4GMONITOR_INFO_T),   (uint8*)&g_frozenWireless.monitorInfo},
    {0x45001700,    sizeof(int16),                  (uint8*)&g_frozenWireless.ber},
    {0x45001800,    sizeof(OOP_OCTETSTR4_T),      	(uint8*)&g_frozenWireless.ci},
    {0x45001900,    sizeof(uint8),                  (uint8*)&g_frozenWireless.networkType},
};

uint16 g_frozenwWirelessNum = sizeof(g_frozenwWirelessList)/sizeof(EventUpdateDataInfo);

extern void sta_last_time_init(OOP_TI_T freq, time_t *lastTime);
extern uint16 sta_frozen_record_write(const OOP_OAD_U mainOad, OOP_FROZENOBJ_T frzObjs, BOOLEAN bStart, time_t storeTime, const uint8 *dataBuf, const uint32 dataLen);
extern void sta_nexttime_update(OOP_TI_T ti, time_t lasttime, time_t *nextFrzTime);
extern int appmain_send_syncmsg(uint32 msgid, uint8 *sendbuf, uint16 sendlen, uint8 *recvbuf, uint16 buflen, uint16 *recvLen);

/**********************************************************************
* @brief     ：获取无线拨号网络制式
* @param[in] ：
				
* @param[out]：
* @return    ：0成功, <0失败
**********************************************************************/
int get_network_type()
{
	uint8  buf[480] = {0};
    uint16 msglen = 0;
    WIRE_DIAG_INFO_T tDiagInfo;

	uint8 no		= 0;       //模块id 1-2

	//模块1
	no = 1;

    msglen = appmain_get_ppp_diag(no, buf, 480);
    if(0 == msglen)
    {
        STAT_FMT_DEBUG("get_network_type ppp_diag failed, no[%u]\n", no);
        return -1;
    }
	
    memset(&tDiagInfo, 0, sizeof(WIRE_DIAG_INFO_T));
    appmsg_wirediag_split(buf, &tDiagInfo);
	if(tDiagInfo.simstatus != 1)
    {
//        STAT_FMT_DEBUG("simstatus is NULL\n");
//        return -1;
    }

	if(tDiagInfo.networkType == 255)
    {
        g_frozenWireless.networkType = 0;
    }
	else
    {
        g_frozenWireless.networkType = tDiagInfo.networkType +2;
    }

	return 0;
}


#if DESC("AT指令透传",1)

/**********************************************************************
* @brief     ：读取+NETDMSG: +NETDMSG: "460+11", 0x7757, 0x77E9B30, 219, "CDMA1X and LTE", LTE BAND 3,
1825, -58, 11, 0x0, 0x0, 0x0, -848, -83, 159 
* @param[in] ：buf  AT命令行
* @param[out]：pInfo		注册网络信息
* @return    ：
* @Create    : 梅安宁
* @Date      ：2022-2-28
* @Update    :
**********************************************************************/
int at_get_netdmsg(char *buf, FROZEN_WIRELESS_INFO_T *pInfo)
{
	int i			= 0;
    char *ptemp		= NULL;
	char *ptemp2	= NULL;
	int  nValue		= 0;

	//获取前，置成默认值0XEE
	int16 csqvalue	= pInfo->csqvalue;       //信号强度
	int16 ber		= pInfo->ber;            //误码率
	memset(pInfo, 0XEE, sizeof(FROZEN_WIRELESS_INFO_T));
	pInfo->csqvalue = csqvalue;
	pInfo->ber		= ber;
    
    if (buf == NULL || NULL == strstr(buf, "+NETDMSG:"))
	{
    	return -1;
	}

	ptemp2 = strstr(buf, "+NETDMSG:");
	for(i=0; i<14; i++)
	{
		ptemp = strstr(ptemp2, ",");
	    if (ptemp == NULL)
	    {
	        break;
	    }
		else
        {
        	ptemp += 1;		//去掉“,”
        	
			switch(i)
			{
				case 1:		//小区编号
					nValue = strtol(ptemp, &ptemp2, 16);
					memcpy(pInfo->ci.nValue, &nValue, 4);
					break;
				case 3:		//网络制式
					if (strstr(ptemp2, "NONE") != NULL)
				    {
				        pInfo->networkType = 0;
				    }
					break;
				case 5:		//当前频点
					pInfo->monitorInfo.arfcn = strtoul(ptemp, &ptemp2, 10);
					break;
				case 11:	//接收功率
					pInfo->monitorInfo.rsrp = strtol(ptemp, &ptemp2, 10);
					break;
				case 12:	//接收质量
					pInfo->monitorInfo.rsrq = strtol(ptemp, &ptemp2, 10);
					break;
				case 13:	//信噪比
					pInfo->monitorInfo.simr = strtol(ptemp, &ptemp2, 10);
					break;
			}
			ptemp2 = ptemp;		

        }
	}
    
    return 0;
}

/**********************************************************************
* @brief     ：读取+NETDMSG: +CSQEX: 0,87,90,99
* @param[in] ：buf  AT命令行
* @param[out]：pInfo		网络信号信息
* @return    ：
* @Create    : 梅安宁
* @Date      ：2022-2-28
* @Update    :
**********************************************************************/
int at_get_csqex(char *buf, FROZEN_WIRELESS_INFO_T *pInfo)
{
	int i			= 0;
    char *ptemp		= NULL;
	char *ptemp2	= NULL;

	//获取前，置成默认值0XEE
	pInfo->csqvalue = 0XEEEE;
	pInfo->ber		= 0XEEEE;
    
    if (buf == NULL || NULL == strstr(buf, "+CSQEX:"))
	{
    	return -1;
	}

	ptemp2 = strstr(buf, "+CSQEX:");
	for(i=0; i<3; i++)
	{
		ptemp = strstr(ptemp2, ",");
	    if (ptemp == NULL)
	    {
	        break;
	    }
		else
        {
        	ptemp += 1;		//去掉“,”
			switch(i)
			{
				case 0:		//信号强度
					pInfo->csqvalue = strtol(ptemp, &ptemp2, 10);
					break;
				case 2:		//误码率
					pInfo->ber = strtol(ptemp, &ptemp2, 10);
					break;
			}
			ptemp2 = ptemp;		

        }
	}
    
    return 0;
}


/**********************************************************************
* @brief     ：获取当前注册网络信息netdmsg
* @param[in] ：		
* @param[out]：
* @return    ：0成功, <0失败
**********************************************************************/
int app_get_netdmsg()
{
	int    ret 				= 0;
	uint8  recvBuf[1024] 	= {0};
	char   sendBuf[32] 		= {0};
    uint16 recvLen 			= 0;

	sendBuf[0] = 10;
	memcpy(sendBuf+1, "AT+NETDMSG", 10);

    ret = appmain_send_syncmsg(MSG_APPMAIN_TRANSPORT, (uint8*)sendBuf, strlen(sendBuf), recvBuf, sizeof(recvBuf), &recvLen);
    if (ret != ERR_OK || 0 == recvLen)
    {
        STAT_FMT_DEBUG("app_get_netdmsg err! ret = %d, recvLen = %d\n", ret, recvLen);
        return ret;
    }

	STAT_FMT_DEBUG("app_get_netdmsg:%s", recvBuf+2);

    //小区编码等解析 TODO
    at_get_netdmsg((char*)recvBuf+1, &g_frozenWireless);

	//网络制式
	get_network_type();

	return 0;
}

/**********************************************************************
* @brief     ：获取网络信号信息csqex
* @param[in] ： 
* @param[out]：
* @return    ：0成功, <0失败
**********************************************************************/
int app_get_csqex()
{
	int    ret 				= 0;
	uint8  recvBuf[1024] 	= {0};
	char   sendBuf[32] 		= {0};
    uint16 recvLen 			= 0;

	sendBuf[0] = 8;
	memcpy(sendBuf+1, "AT+CSQEX", 8);

    ret = appmain_send_syncmsg(MSG_APPMAIN_TRANSPORT, (uint8*)sendBuf, strlen(sendBuf), recvBuf, sizeof(recvBuf), &recvLen);
    if (ret != ERR_OK || 0 == recvLen)
    {
        STAT_FMT_DEBUG("app_get_csqex err! ret = %d, recvLen = %d\n", ret, recvLen);
        return ret;
    }

	STAT_FMT_DEBUG("app_get_csqex:%s", recvBuf+2);

    //信号强度、误码率解析 TODO
    at_get_csqex((char*)recvBuf+1, &g_frozenWireless);

	return 0;
}


#endif

/***********************************************************************
* @brief：      执行冻结
* @param[in]： pTmpNode 冻结参数
* @param[out]：NA
* @return：     time_t 下一次冻结时间
**********************************************************************/
void sta_frozen_wireless_execute(time_t storeTime, FrzRunParaList_t *pTmpNode)
{
    int ret = ERR_OK;
    uint16 j;
    OOP_FROZENOBJ_T frzObjs;
    BOOLEAN bStart = TRUE;

    STAT_FMT_DEBUG("---------frozen execute start--------\n");
	memset(&g_frozenWireless, 0xEE, sizeof(g_frozenWireless));

	app_get_netdmsg();
	app_get_csqex();
	
    frzObjs.nPeriod = pTmpNode->frzRunPara.period;
    frzObjs.nDepth= pTmpNode->frzRunPara.storeDepth;

    /*遍历每个关联对象属性*/
    for(j = 0; j < g_frozenwWirelessNum; j++)
    {
        frzObjs.oad.value = g_frozenwWirelessList[j].oadValue;

        /* 写一个记录型数据 db_write_record*/
        ret = sta_frozen_record_write(pTmpNode->frzOAD, frzObjs, bStart, storeTime, g_frozenwWirelessList[j].pBuf, g_frozenwWirelessList[j].dateLen);
        if(ret != 0)
        {
            STAT_FMT_DEBUG("sta_frozen_record_write failed. ret(%d), mainOAD(0x%08x), subOAD(0x%08x)\n", ret, pTmpNode->frzOAD.value, frzObjs.oad.value);
            return;   
        }

        bStart = FALSE;
    }

    STAT_FMT_TRACE("---------frozen execute end--------\n");

    return;
}

/***********************************************************************
* @brief：      冻结远程通信信道信息处理
* @param[in]： NA
* @param[out]：NA
* @return：     void
**********************************************************************/
void sta_frozen_wireless_proc()
{
    FrzRunParaList_t frozenWirelessNode;   //冻结参数链表
    time_t nextFrzTime;        //时间为秒数，方便转化为协议要求的时间格式
    time_t curTime;
    OOP_TI_T time_interval;
    DT_DateTimeShort2Time(&realstattime, &curTime);

    /*初始化冻结参数链表*/
	time_interval.unit = TI_MIN;
    time_interval.value = 5;		//五分钟冻结一次
	frozenWirelessNode.frzOAD.value = 0x50020200;
	frozenWirelessNode.frzRunPara.period = time_interval.value;
	frozenWirelessNode.frzRunPara.storeDepth = 12*24*3; //存储三天数据
	frozenWirelessNode.frzRunPara.oadNum = 5;

	sta_last_time_init(time_interval, &frozenWirelessNode.frzRunPara.lastFrzTm);

    //冻结时间
    sta_nexttime_update(time_interval, frozenWirelessNode.frzRunPara.lastFrzTm, &nextFrzTime);

    /* 满足冻结条件，执行冻结 */
    if((nextFrzTime!=0) && (nextFrzTime<=curTime))
    {
        sta_frozen_wireless_execute(nextFrzTime, &frozenWirelessNode);
        frozenWirelessNode.frzRunPara.lastFrzTm = nextFrzTime;
    }
            
    return;
}

#endif



