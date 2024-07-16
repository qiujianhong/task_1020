#if AREA_ZHEJIANG
#include "baseCommon.h"
#include "oadInfoList.h"
#include "oopservice.h"
#include "commClass.h"
#include "oopStorage.h"
#include "oopVarUpdate.h"
#include "area.h"

#ifdef ADV_PU_AMR
FACTORY_VER_TMP_T g_tVer = 
{
    "XXXX",      /**< 厂商代码           */
    "CZ01",      /**< 软件版本号         */
    "220817",    /**< 软件版本日期       */
    "CC01",      /**< 硬件版本号         */
    "220621",    /**< 硬件版本日期       */
    "CZ01-01",   /**< 厂家扩展信息       */
};
#elif ADV_ST_AMR
    #ifdef PRODUCT_ECU
    //能源控制器
    FACTORY_VER_TMP_T g_tVer = 
    {
        "XXXX",      /**< 厂商代码           */
        "EZ01",      /**< 软件版本号         */
        "220619",    /**< 软件版本日期       */
        "ER01",      /**< 硬件版本号         */
        "220619",    /**< 硬件版本日期       */
        "EZ01-01",   /**< 厂家扩展信息       */
    };
    #elif PRODUCT_ZCU_1
    //专变1型
    FACTORY_VER_TMP_T g_tVer = 
    {
        "XXXX",      /**< 厂商代码           */
        "1Z01",      /**< 软件版本号         */
        "220809",    /**< 软件版本日期       */
        "Z101",      /**< 硬件版本号         */
        "220727",    /**< 硬件版本日期       */
        "1Z01-02",   /**< 厂家扩展信息       */
    };
    #elif PRODUCT_ZCU_2
    //专变2型
    FACTORY_VER_TMP_T g_tVer = 
    {
        "XXXX",      /**< 厂商代码           */
        "2Z01",      /**< 软件版本号         */
        "220715",    /**< 软件版本日期       */
        "Z301",      /**< 硬件版本号         */
        "220715",    /**< 硬件版本日期       */
        "2A01-01",   /**< 厂家扩展信息       */
    };
    #elif PRODUCT_ZCU_3
    //专变3型
    FACTORY_VER_TMP_T g_tVer = 
    {
        "XXXX",      /**< 厂商代码           */
        "3Z01",      /**< 软件版本号         */
        "220716",    /**< 软件版本日期       */
        "Z301",      /**< 硬件版本号         */
        "220715",    /**< 硬件版本日期       */
        "3A01-01",   /**< 厂家扩展信息       */
    };
     #elif PRODUCT_ZCU_N
    //QT专变
    FACTORY_VER_TMP_T g_tVer = 
    {
        "XXXX",      /**< 厂商代码           */
        "NZ01",      /**< 软件版本号         */
        "210207",    /**< 软件版本日期       */
        "ZN01",      /**< 硬件版本号         */
        "200624",    /**< 硬件版本日期       */
        "NA01-01",   /**< 厂家扩展信息       */
    };
    #endif
#endif

// 普通参数，列出APDU，长度，数量
const uint8 setDefaultPara[][LOCAL_RECV_FRAM_MAXLEN] = 
{
    {0x06, 0x01, 0x00, 0x43, 0x00, 0x08, 0x00, 0x03, 0x01, 0x00},  //设置终端主动上报
    {0x06, 0x01, 0x01, 0x31, 0x11, 0x09, 0x00, 0x03, 0x01, 0x00},  //0x31110900
    {0x06, 0x01, 0x02, 0x31, 0x11, 0x08, 0x00, 0x16, 0x03, 0x00},  //0x31110800
    {0x06, 0x01, 0x01, 0x31, 0x14, 0x09, 0x00, 0x03, 0x01, 0x00},  //0x31140900
    {0x06, 0x01, 0x02, 0x31, 0x14, 0x08, 0x00, 0x16, 0x03, 0x00},  //0x31140800    
    {0x06, 0x01, 0x01, 0x31, 0x1B, 0x09, 0x00, 0x03, 0x01, 0x00},  //0x311B0900
    {0x06, 0x01, 0x02, 0x31, 0x1B, 0x08, 0x00, 0x16, 0x03, 0x00},  //0x311B0800
    {0x06, 0x01, 0x01, 0x30, 0x11, 0x09, 0x00, 0x03, 0x01, 0x00},  //0x30110900
    {0x06, 0x01, 0x02, 0x30, 0x11, 0x08, 0x00, 0x16, 0x03, 0x00},  //0x30110800


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
* @name      : set_act_by_protocol_anhui
* @brief     ：进程启动 统一接口
* @param[in] ：int sockdbfd    句柄
* @param[out]：
* @return    ：
**********************************************************************/
void set_act_by_protocol_zhejiang(int sockdbfd)
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
void app_setdefault_cfg_zhejiang(int sockdbfd)
{
    set_act_by_protocol_zhejiang(sockdbfd);

    return;
}

#define CBIR_READ_MOD       0       //0 从缓存中读 1 直接从数据中心读

extern OOP_DAR_E buf_to_data_basic_object(const OAD_INFO_T *pOadInfo, const void *srcBuf, uint32 srcLen, uint32 *pOffset, void *dstData, uint32 dstLen);
extern OOP_DAR_E buf_to_data_extended_object(const OAD_INFO_T *pOadInfo, const void *srcBuf, uint32 srcLen, uint32 *pOffset, void *dstData, uint32 dstLen);
extern int meter_data_if_clear(APDU_INFO_T *pApduInfo, OOP_METER_T *oldMeter, OOP_METER_T *curMeter);
extern int meter_data_clear(DB_CLIENT fd, uint16 infoNum);
extern OOP_DAR_E data_to_buf_basic_object(const OAD_INFO_T *pOadInfo, void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen);
extern OOP_DAR_E data_to_buf_extended_object(const OAD_INFO_T *pOadInfo, void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen);

/* 变量类OAD列表 OIA1 = 0x02 */
const OAD_TAB_T zhejiang_oadInfoList[] = 
{
    /*OAD          class    A1      A2    rsv     dataType                 len*/
    {{0x4E070200}, 8,       0x04,   0x0E, 0,      DT_STRUCTURE, sizeof(Circuit_Breaker_Identify_Result), E_NULL},            //拓扑识别关系
};

const uint32 zhejiang_oadInfoListLen = sizeof(zhejiang_oadInfoList)/sizeof(zhejiang_oadInfoList[0]);

/**
*********************************************************************
* @name：       class_zhejiang_oadinfo_get
* @brief：      查询ODA列表，得到OAD描述信息
* @param[in] ：OOP_OAD_U oad        查询OAD
* @param[out]：OAD_INFO_T *info     ODA信息
* @return    ：查询结果 TRUE-成功，FALSE-失败
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
int class_zhejiang_oadinfo_get(OOP_OAD_U oad, OAD_INFO_T *pstOdaInfo)
{
    uint8 OIA  = 0;
    uint8 OIA1 = 0;
    uint8 OIA2 = 0;
    OIA = (uint8)(oad.nObjID>>8);
    OIA1 = (uint8)((OIA>>4)&0x0f);
    OIA2 = (uint8)((OIA)&0x0f);
    OOP_OAD_U blockOAD = {0};
    uint32 size = 0;
    const OAD_TAB_T *pstOdaTab = NULL;

    //不为0才赋值，否则递归调用后会被清零
    if(oad.nIndex != 0)  
    {
        pstOdaInfo->nIndex = oad.nIndex;
    }
    

    pstOdaTab = zhejiang_oadInfoList; size = zhejiang_oadInfoListLen;

    if(pstOdaTab != NULL)
    {
        while(size--)
        {
            if(pstOdaTab->oad.value == oad.value)
            {
                pstOdaInfo->pTab = pstOdaTab;
                pstOdaInfo->subeData = pstOdaTab->eData;
                return TRUE;
            }
            else if((pstOdaTab->oad.nObjID == oad.nObjID) &&
                    (pstOdaTab->oad.attID == oad.attID) &&
                    (pstOdaTab->oad.nIndex == oad.nIndex)) 
            {
                PRTL_FMT_DEBUG("non-strict search oadInfoList. OAD(0x%08x), ATTPRO(%d), OIA1(%d), OIA2(%d)\n", oad.value, oad.attPro, OIA1, OIA2);
                pstOdaInfo->pTab = pstOdaTab;
                pstOdaInfo->subeData = pstOdaTab->eData;
                return TRUE;                
            }
                    
            pstOdaTab++;
        }
    }

    //如果索引不为0，查不中，再用0查一次
    if(oad.nIndex != 0)  
    {
        blockOAD.value = oad.value;
        blockOAD.nIndex = 0;
        
        PRTL_FMT_DEBUG("block search oadInfoList. OAD(0x%08x), OIA1(%d), OIA2(%d)\n", oad.value, OIA1, OIA2);
        return class_zhejiang_oadinfo_get(blockOAD, pstOdaInfo);
    }

    PRTL_FMT_DEBUG("search oadInfoList miss. OAD(0x%08x), OIA1(%d), OIA2(%d)\n", oad.value, OIA1, OIA2);

    return FALSE;
}

/* 自定义OMD列表 OIA1 = 0x0F OIA2 = 0x0F */
const OMD_INFO_T zhejiang_omdInfoList[] = 
{
    /*OMD          class   A1      A2    rsv     dataType                  len*/
};
const uint32 zhejiang_omdInfoListLen = sizeof(zhejiang_omdInfoList)/sizeof(zhejiang_omdInfoList[0]);

/**
*********************************************************************
* @name：       class_zhejiang_omdinfo_get
* @brief：      查询ODA列表，得到OAD描述信息
* @param[in] ：OOP_OAD_U oad        查询OAD
* @param[out]：OAD_INFO_T *info     ODA信息
* @return    ：查询结果 TRUE-成功，FALSE-失败
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
int class_zhejiang_omdinfo_get(OOP_OAD_U oad, OAD_INFO_T *pstOdaInfo)
{
    uint8 OIA  = 0;
    uint8 OIA1 = 0;
    uint8 OIA2 = 0;
    OIA = (uint8)(oad.nObjID>>8);
    OIA1 = (uint8)((OIA>>4)&0x0f);
    OIA2 = (uint8)((OIA)&0x0f);
    OOP_OAD_U blockOAD = {0};
    uint32 size = 0;
    const OMD_INFO_T *pstOdaTab = NULL;
     
    pstOdaTab = zhejiang_omdInfoList; size = zhejiang_omdInfoListLen;

    if(pstOdaTab != NULL)
    {
        while(size--)
        {
            if(pstOdaTab->omd.value == oad.value)
            {
                pstOdaInfo->pTab = (OAD_TAB_T *)pstOdaTab;
                return TRUE;
            }

            pstOdaTab++;
        }
    }

    //如果索引不为0，查不中，再用0查一次
    if(oad.nIndex != 0)  
    {
        blockOAD.value = oad.value;
        blockOAD.nIndex = 0;
        
        PRTL_FMT_DEBUG("block search oadInfoList. OAD(0x%08x), OIA1(%d), OIA2(%d)\n", oad.value, OIA1, OIA2);
        return class_zhejiang_omdinfo_get(blockOAD, pstOdaInfo);
    }

    PRTL_FMT_DEBUG("search omdInfoList miss. OAD(0x%08x), OIA1(%d), OIA2(%d)\n", oad.value, OIA1, OIA2);

    return FALSE;
}


OOP_DAR_E buf_to_data_annex_object_area(const OAD_INFO_T *pOadInfo, const void *srcBuf, uint32 srcLen, uint32 *pOffset, void *dstData, uint32 dstLen)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *pOffset;    
    OOP_METER_ANNEX_T		data;		/**< 附属信息	*/
    memset(&data,0x00,sizeof(OOP_METER_ANNEX_T));
    uint8 *srcBufTmp = (uint8 *)srcBuf;
    uint8 i,count;
    //结构类型
    if(srcBufTmp[sOffset++] != DT_ARRAY)
    {
        PRTL_BUF_DEBUG(srcBufTmp, srcLen, "check struct type failed. sOffset(%d)", sOffset);
        return DATA_TYPE_UNMATCHED;
    }
    count = srcBufTmp[sOffset++];
    if(count == 0)
    {
        *pOffset = sOffset;
        PRTL_BUF_DEBUG(srcBufTmp, srcLen, "annex_object count == 0 \n");
        return dar;
    }
    for(i=0;i<count;i++)
    {
        OOP_OAD_U oad;
        uint16 value;
        if(srcBufTmp[sOffset++] != DT_STRUCTURE)
        {
            PRTL_BUF_DEBUG(srcBufTmp, srcLen, "check struct type failed. sOffset(%d)", sOffset);
            return DATA_TYPE_UNMATCHED;
        }
        if(srcBufTmp[sOffset++] != 2)
        {
            PRTL_BUF_DEBUG(srcBufTmp, srcLen, "check struct num failed. sOffset(%d)", sOffset);
            return DATA_TYPE_UNMATCHED;
        }
        //OAD
        dar = basic_buf_to_data(E_OAD, srcBuf, srcLen, &sOffset, &oad, sizeof(OOP_OAD_U));
        if(dar != DATA_SUCCESS)
        {
            PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
            return DATA_TYPE_UNMATCHED;
        }
        dar = basic_buf_to_data(E_LONG_UNSIGNED, srcBuf, srcLen, &sOffset, &value, sizeof(uint16));
        if(dar != DATA_SUCCESS)
        {
            PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
            return DATA_TYPE_UNMATCHED;
        }
        if(oad.value == 0x6E010200||oad.value == 0x6E020200)
        {
            data.annexObj[data.nNum].oad = oad;
            data.annexObj[data.nNum].nLen = sizeof(uint16);
            memcpy(data.annexObj[data.nNum].value,&value,sizeof(uint16));
            data.nNum++;
        }
    }

    //避免每步都判断，放最后检查，但检查前可能已经越界
    if(sOffset > srcLen)
    {
        PRTL_FMT_DEBUG("cross the border. sOffset(%d), srcLen(%d)\n", sOffset, srcLen);
        return DATA_SCOPE_OF_ACCESS_VIOLATED;        
    }

    memcpy(dstData, &data, dstLen);

    *pOffset = sOffset;
  
    return dar;
}

OOP_DAR_E area_buf_to_data_oop_meter(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_METER_T data;
    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    MEMZERO(&data, sizeof(data));
    
    //结构类型
    if(srcBuf[sOffset++] != DT_STRUCTURE)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "check struct type failed. sOffset(%d)", sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //结构成员数
    if(srcBuf[sOffset++] != 4)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "check struct num failed. sOffset(%d)", sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //配置序号
    dar = basic_buf_to_data(E_LONG_UNSIGNED, srcBuf, srcLen, &sOffset, &data.nIndex, sizeof(data.nIndex));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //基本信息
    dar = buf_to_data_basic_object(pOadInfo, srcBuf, srcLen, &sOffset, &data.basic, sizeof(data.basic));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d)\n", dar);
        return DATA_TYPE_UNMATCHED;
    }

    //扩展信息
    dar = buf_to_data_extended_object(pOadInfo, srcBuf, srcLen, &sOffset, &data.extend, sizeof(data.extend));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d)\n", dar);
        return DATA_TYPE_UNMATCHED;
    }

    //附属信息
    dar = buf_to_data_annex_object_area(pOadInfo, srcBuf, srcLen, &sOffset, &data.annex, sizeof(data.annex));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d)\n", dar);
        return DATA_TYPE_UNMATCHED;
    }

    memcpy(&dstBuf[dOffset], &data, sizeof(data));
    dOffset += sizeof(data);

    //避免每步都判断，放最后检查，但检查前可能已经越界
    if((sOffset > srcLen) || (dOffset > dstLen))
    {
        PRTL_FMT_DEBUG("cross the border. sOffset(%d), srcLen(%d), dOffset(%d), dstLen(%d)\n", sOffset, srcLen, dOffset, dstLen);
        return DATA_SCOPE_OF_ACCESS_VIOLATED;        
    }

    *convert->dOffset = dOffset;
    *convert->sOffset = sOffset;
     
    return dar;
}
/**
*********************************************************************
* @brief：     从698格式报文转数据中心的数据(档案信息)
* @param[in]： ppOadInfo     - 数据表指针
               srcBuf       - 源数据
               srcLen       - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen       - 目的数据长度
               sOffset      - 转换后源数据区的偏移
               dOffset      - 转换后目的数据区的偏移
* @return：     =0           - 成功
              !=0           - 错误码
*********************************************************************
*/
OOP_DAR_E area_buf_to_data_oop_meter_extend_object(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOPM_METER_130_T data;
    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    MEMZERO(&data, sizeof(data));
    
    //结构类型
    if(srcBuf[sOffset++] != DT_STRUCTURE)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "check struct type failed. sOffset(%d)", sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //结构成员数
    if(srcBuf[sOffset++] != 3)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "check struct num failed. sOffset(%d)", sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //配置序号
    dar = basic_buf_to_data(E_LONG_UNSIGNED, srcBuf, srcLen, &sOffset, &data.index, sizeof(data.index));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //扩展信息
    dar = buf_to_data_extended_object(pOadInfo, srcBuf, srcLen, &sOffset, &data.ext, sizeof(data.ext));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d)\n", dar);
        return DATA_TYPE_UNMATCHED;
    }

    //附属信息
    dar = buf_to_data_annex_object_area(pOadInfo, srcBuf, srcLen, &sOffset, &data.annex, sizeof(data.annex));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d)\n", dar);
        return DATA_TYPE_UNMATCHED;
    }


    memcpy(&dstBuf[dOffset], &data, sizeof(data));
    dOffset += sizeof(data);

    //避免每步都判断，放最后检查，但检查前可能已经越界
    if((sOffset > srcLen) || (dOffset > dstLen))
    {
        PRTL_FMT_DEBUG("cross the border. sOffset(%d), srcLen(%d), dOffset(%d), dstLen(%d)\n", sOffset, srcLen, dOffset, dstLen);
        return DATA_SCOPE_OF_ACCESS_VIOLATED;        
    }

    *convert->dOffset = dOffset;
    *convert->sOffset = sOffset;
     
    return dar;
}


OOP_DAR_E area_buf_to_data_oop_meter_tsa_annex(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOPM_METER_135_T data;
    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    MEMZERO(&data, sizeof(data));
    
    //结构类型
    if(srcBuf[sOffset++] != DT_STRUCTURE)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "check struct type failed. sOffset(%d)", sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //结构成员数
    if(srcBuf[sOffset++] != 2)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "check struct num failed. sOffset(%d)", sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //配置序号
    dar = basic_buf_to_data(E_TSA, srcBuf, srcLen, &sOffset, &data.add, sizeof(data.add));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //附属信息
    dar = buf_to_data_annex_object_area(pOadInfo, srcBuf, srcLen, &sOffset, &data.annex, sizeof(data.annex));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d)\n", dar);
        return DATA_TYPE_UNMATCHED;
    }


    memcpy(&dstBuf[dOffset], &data, sizeof(data));
    dOffset += sizeof(data);

    //避免每步都判断，放最后检查，但检查前可能已经越界
    if((sOffset > srcLen) || (dOffset > dstLen))
    {
        PRTL_FMT_DEBUG("cross the border. sOffset(%d), srcLen(%d), dOffset(%d), dstLen(%d)\n", sOffset, srcLen, dOffset, dstLen);
        return DATA_SCOPE_OF_ACCESS_VIOLATED;        
    }

    *convert->dOffset = dOffset;
    *convert->sOffset = sOffset;
     
    return dar;
}


/**
*********************************************************************
* @name：      class11_invoke_act_oop_meter
* @brief：     class11的ACTION服务
* @param[in] ：pOdaInfo        OAD信息
               pBpInfo        应用分帧断点信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
               outDataMax     输出报文缓冲区最大长度

* @param[out]：outData     输出报文
* @return    ：OOP_DAR_E
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E area_class11_invoke_act_oop_meter(OAD_INFO_T *pOdaInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 inLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    uint16 i = 0;
    uint32 offset = 0;
    int result = DATA_OTHER_REASON;
    int ret = ERR_NORMAL;
    OOP_DAR_E nRet = DATA_OTHER_REASON;
    NOMAL_OAD_T nomalOad = {0};
    OOP_METER_T oopMeter;
    OOP_METER_T oldOopMeter;
    DATA_CONVERT_T convert;
    uint32 sOffset = 4;
    uint32 dOffset = 0;
    uint32 len = 0;
    int meterret = ERR_OK;

    /*类型不存在*/
    if ((pOdaInfo == NULL) || (pApduInfo == NULL) || (inData == NULL) || (outData == NULL))
    {
        return DATA_READ_WRITE_DENIED;
    }

    MEMZERO(&oopMeter, sizeof(OOP_METER_T));
    
    convert.srcBuf = (void*)inData;
    convert.srcLen = inLen;
    convert.sOffset = &sOffset;

    convert.dstBuf = &oopMeter;
    convert.dstLen = sizeof(oopMeter);
    convert.dOffset = &dOffset;  

    PRTL_FMT_LOGCC("pOdaInfo->pTab->oad.value is %x\n",pOdaInfo->pTab->oad.value);
    switch (pOdaInfo->pTab->oad.value)
    {
        case 0x60007F00:
        {
            /*下发一个档案单元*/
            PRTL_FMT_LOGCC("class11_invoke_act 下发一个档案单元\n");
            nRet = area_buf_to_data_oop_meter(pOdaInfo, &convert);

            if (nRet != DATA_SUCCESS)
            {
                return DATA_READ_WRITE_DENIED;
            }
            else
            {
                dOffset = 0;
            }

            //检测档案数据是否有效
            if ((oopMeter.basic.baud > 10 && oopMeter.basic.baud != 255) ||
                (oopMeter.basic.protocol > 4) ||
                (oopMeter.basic.pwrType > 3))
            {
                return DATA_SCOPE_OF_ACCESS_VIOLATED;
            }

            MEMZERO(&oldOopMeter, sizeof(OOP_METER_T));
            
            nomalOad.logicId = 0x00;
            nomalOad.infoNum = oopMeter.nIndex;
            nomalOad.oad.value = 0x60000200;
            nomalOad.classtag = CLASS_DATA_INIT;

            //先读出原电表信息
            meterret = app_get_meter_list(nomalOad.infoNum, &oldOopMeter);
            if(meterret != ERR_OK)
            {
                PRTL_FMT_TRACE("get old meter list. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x), inMaxLen(%d), outLen\n",
                    meterret, nomalOad.logicId, nomalOad.infoNum, nomalOad.oad.value, sizeof(OOP_METER_T), len);
            }
            
            ret = db_write_nomal(pApduInfo->hUdp, &nomalOad, (uint8*)&oopMeter, sizeof(OOP_METER_T));
            if (ret != ERR_OK)
            {
                PRTL_FMT_DEBUG("class11_invoke_act_oop_meter db_write_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                    ret, nomalOad.logicId, nomalOad.infoNum, nomalOad.oad.value);
                return DATA_READ_WRITE_DENIED;
            }

            //删档案，或通信地址，端口，规约类型，用户类型发生变化，则清电表数据
            if(meterret == ERR_OK)
            {
                ret = meter_data_if_clear(pApduInfo, &oldOopMeter, &oopMeter);
                if(ret != ERR_OK)
                {
                    PRTL_FMT_DEBUG("meter data clear failed. ret(%d), omd(0x%08x)\n", ret, pOdaInfo->pTab->oad.value);
                    return DATA_READ_WRITE_DENIED;
                }
            }
            
            result = DATA_SUCCESS;                                             //填写成功标识
            offset = 0;
            memcpy(&outData->pResultData[offset], inData, sizeof(OOP_OAD_U));  //OMD
            offset += sizeof(OOP_OAD_U);
            outData->pResultData[offset ++] = 0x00;                            //DAR type
            outData->pResultData[offset ++] = 0x00;                            //DATA:NULL
            outData->resultDataLen = offset;
        }
        break;
        case 0x60008000:
        {
            /*下发多个档案单元*/
            uint32      oopMeterNum;

            PRTL_FMT_LOGCC("class11_invoke_act 下发多个档案单元\n");

            if(inData[sOffset ++] != DT_ARRAY)
            {
                return ERR_OPTION;
            }
            oopMeterNum = inData[sOffset++];
            PRTL_FMT_LOGCC("oopMeterNum is %d\n",oopMeterNum);
            for(i = 0; i < oopMeterNum; i++)
            {

                nRet = area_buf_to_data_oop_meter(pOdaInfo, &convert);

                if (nRet != DATA_SUCCESS)
                {
                    PRTL_FMT_LOGCC("buf_to_data_oop_meter err = %d\n", ret);
                    return DATA_READ_WRITE_DENIED;
                }
                else
                {
                    dOffset = 0;
                }

                //检测档案数据是否有效
                if ((oopMeter.basic.baud > 10 && oopMeter.basic.baud != 255) ||
                    (oopMeter.basic.protocol > 4) ||
                    (oopMeter.basic.pwrType > 3))
                {
                    return DATA_SCOPE_OF_ACCESS_VIOLATED;
                }

                PRTL_FMT_LOGCC("oopMeter.nIndex = %d\n", oopMeter.nIndex);
                PRTL_FMT_LOGCC("oopMeter.basic.protocol = %d\n", oopMeter.basic.protocol);
                PRTL_BUF_LOGCC(oopMeter.basic.tsa.add, oopMeter.basic.tsa.len + 1, "地址:");

                MEMZERO(&oldOopMeter, sizeof(OOP_METER_T));

                nomalOad.logicId = 0x00;
                nomalOad.infoNum = oopMeter.nIndex;
                nomalOad.oad.value = 0x60000200;
                nomalOad.classtag = CLASS_DATA_INIT;
                
                //先读出原电表信息
                meterret = app_get_meter_list(nomalOad.infoNum, &oldOopMeter);
                if(meterret != ERR_OK)
                {
                    PRTL_FMT_TRACE("get old meter list. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                        meterret, nomalOad.logicId, nomalOad.infoNum, nomalOad.oad.value);
                }

                ret = db_write_nomal(pApduInfo->hUdp, &nomalOad, (uint8*)&oopMeter, sizeof(OOP_METER_T));
                if (ret != ERR_OK)
                {
                    PRTL_FMT_DEBUG("class11_invoke_act_oop_meter db_write_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                        ret, nomalOad.logicId, nomalOad.infoNum, nomalOad.oad.value);
                    return DATA_READ_WRITE_DENIED;
                }

                //删档案，或通信地址，端口，规约类型，用户类型发生变化，则清电表数据
                if(meterret == ERR_OK)
                {
                    ret = meter_data_if_clear(pApduInfo, &oldOopMeter, &oopMeter);
                    if(ret != ERR_OK)
                    {
                        PRTL_FMT_DEBUG("meter data clear failed. ret(%d), omd(0x%08x)\n", ret, pOdaInfo->pTab->oad.value);
                        return DATA_READ_WRITE_DENIED;
                    }
                }
            }

            result = DATA_SUCCESS;                                             //填写成功标识

            offset = 0;
            memcpy(&outData->pResultData[offset], inData, sizeof(OOP_OAD_U));  //OMD
            offset += sizeof(OOP_OAD_U);
            outData->pResultData[offset ++] = 0x00;                            //DAR type
            outData->pResultData[offset ++] = 0x00;                            //DATA:NULL
            outData->resultDataLen = offset;
        }
        break;
        case 0x60008100:
        {
            OOPM_METER_129_T data;

            MEMZERO(&data, sizeof(data));

            convert.srcBuf = (void*)inData;
            convert.srcLen = inLen;
            convert.sOffset = &sOffset;

            convert.dstBuf = &data;
            convert.dstLen = sizeof(data);
            convert.dOffset = &dOffset; 

            /*更新档案单元（129方法）*/
            PRTL_FMT_LOGCC("class11_invoke_act 更新一个档案单元（方法129）\n");
            nRet = buf_to_data_oop_meter_basic_object(pOdaInfo, &convert);
            if (nRet != DATA_SUCCESS)
            {
                return DATA_READ_WRITE_DENIED;
            }
            else
            {
                dOffset = 0;
            }

            MEMZERO(&oldOopMeter, sizeof(OOP_METER_T));
            
            nomalOad.logicId = 0x00;
            nomalOad.infoNum = data.index;
            nomalOad.oad.value = 0x60000200;
            nomalOad.classtag = CLASS_DATA_INIT;

            if (data.index > NO_VIR_MTR_MAX_NUM)
            {
                return DATA_SCOPE_OF_ACCESS_VIOLATED;
            }

            //先读出原电表信息
            meterret = app_get_meter_list(nomalOad.infoNum, &oldOopMeter);
            if(meterret != ERR_OK)
            {
                PRTL_FMT_TRACE("get old meter list. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x), inMaxLen(%d), outLen\n",
                    meterret, nomalOad.logicId, nomalOad.infoNum, nomalOad.oad.value, sizeof(OOP_METER_T), len);
            }

            oopMeter = oldOopMeter;
            oopMeter.basic = data.basic;
            
            ret = db_write_nomal(pApduInfo->hUdp, &nomalOad, (uint8*)&oopMeter, sizeof(OOP_METER_T));
            if (ret != ERR_OK)
            {
                PRTL_FMT_DEBUG("class11_invoke_act_oop_meter db_write_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                    ret, nomalOad.logicId, nomalOad.infoNum, nomalOad.oad.value);
                return DATA_READ_WRITE_DENIED;
            }

            //删档案，或通信地址，端口，规约类型，用户类型发生变化，则清电表数据
            if(meterret == ERR_OK)
            {
                ret = meter_data_if_clear(pApduInfo, &oldOopMeter, &oopMeter);
                if(ret != ERR_OK)
                {
                    PRTL_FMT_DEBUG("meter data clear failed. ret(%d), omd(0x%08x)\n", ret, pOdaInfo->pTab->oad.value);
                    return DATA_READ_WRITE_DENIED;
                }
            }
            
            result = DATA_SUCCESS;                                             //填写成功标识
            offset = 0;
            memcpy(&outData->pResultData[offset], inData, sizeof(OOP_OAD_U));  //OMD
            offset += sizeof(OOP_OAD_U);
            outData->pResultData[offset ++] = 0x00;                            //DAR type
            outData->pResultData[offset ++] = 0x00;                            //DATA:NULL
            outData->resultDataLen = offset;
        }
        break;
        case 0x60008200:
        {
            OOPM_METER_130_T data;

            MEMZERO(&data, sizeof(data));

            convert.srcBuf = (void*)inData;
            convert.srcLen = inLen;
            convert.sOffset = &sOffset;

            convert.dstBuf = &data;
            convert.dstLen = sizeof(data);
            convert.dOffset = &dOffset; 

            /*更新档案单元（130方法）*/
            PRTL_FMT_LOGCC("class11_invoke_act 更新一个档案单元（方法130）\n");

            nRet = area_buf_to_data_oop_meter_extend_object(pOdaInfo, &convert);

            if (nRet != DATA_SUCCESS)
            {
                return DATA_READ_WRITE_DENIED;
            }
            else
            {
                dOffset = 0;
            }

            MEMZERO(&oldOopMeter, sizeof(OOP_METER_T));
            
            nomalOad.logicId = 0x00;
            nomalOad.infoNum = data.index;
            nomalOad.oad.value = 0x60000200;
            nomalOad.classtag = CLASS_DATA_INIT;

            if (data.index > NO_VIR_MTR_MAX_NUM)
            {
                return DATA_SCOPE_OF_ACCESS_VIOLATED;
            }

            //先读出原电表信息
            meterret = app_get_meter_list(nomalOad.infoNum, &oldOopMeter);
            if(meterret != ERR_OK)
            {
                PRTL_FMT_TRACE("get old meter list. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x), inMaxLen(%d), outLen\n",
                    meterret, nomalOad.logicId, nomalOad.infoNum, nomalOad.oad.value, sizeof(OOP_METER_T), len);
            }

            oopMeter = oldOopMeter;
            oopMeter.extend = data.ext;
            oopMeter.annex = data.annex;
            
            ret = db_write_nomal(pApduInfo->hUdp, &nomalOad, (uint8*)&oopMeter, sizeof(OOP_METER_T));
            if (ret != ERR_OK)
            {
                PRTL_FMT_DEBUG("class11_invoke_act_oop_meter db_write_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                    ret, nomalOad.logicId, nomalOad.infoNum, nomalOad.oad.value);
                return DATA_READ_WRITE_DENIED;
            }

            //删档案，或通信地址，端口，规约类型，用户类型发生变化，则清电表数据
            if(meterret == ERR_OK)
            {
                ret = meter_data_if_clear(pApduInfo, &oldOopMeter, &oopMeter);
                if(ret != ERR_OK)
                {
                    PRTL_FMT_DEBUG("meter data clear failed. ret(%d), omd(0x%08x)\n", ret, pOdaInfo->pTab->oad.value);
                    return DATA_READ_WRITE_DENIED;
                }
            }
            
            result = DATA_SUCCESS;                                             //填写成功标识
            offset = 0;
            memcpy(&outData->pResultData[offset], inData, sizeof(OOP_OAD_U));  //OMD
            offset += sizeof(OOP_OAD_U);
            outData->pResultData[offset ++] = 0x00;                            //DAR type
            outData->pResultData[offset ++] = 0x00;                            //DATA:NULL
            outData->resultDataLen = offset;
        }
        break;
        case 0x60008600:
        {
            /*清空档案*/
            CLEAR_DATA_T clearData;

            MEMZERO(&clearData, sizeof(clearData));
            PRTL_FMT_LOGCC("class11_invoke_act 清空档案\n");

            if(inData[sOffset ++] != DT_NULL)
            {
                return DATA_TYPE_UNMATCHED;
            }

            /*清空档案操作*/
            clearData.recordOAD.logicId = 0x00;
            clearData.recordOAD.optional = 0;
            clearData.recordOAD.road.oadMain.value = 0x00;
            clearData.recordOAD.road.oadCols.nNum = 1;
            clearData.recordOAD.road.oadCols.oad[0].value = 0x60000200;
			clearData.recordOAD.classtag = CLASS_DATA_INIT;
            clearData.bClassTag = FALSE;
			
            clearData.recordOAD.infoNum = 0xffff;
            ret = db_clear_oad_data(pApduInfo->hUdp, &clearData);
            if (ret != ERR_OK)
            {
                PRTL_FMT_LOGCC("删除档案全部失败 err = %d\n", ret);
            }
            else
            {
                PRTL_FMT_LOGCC("删除档案全部成功\n");
            }

            ret = meter_data_clear(pApduInfo->hUdp, clearData.recordOAD.infoNum);
            if(ret != ERR_OK)
            {
                PRTL_FMT_DEBUG("meter data clear failed. ret(%d), omd(0x%08x)\n", ret, pOdaInfo->pTab->oad.value);
                return DATA_READ_WRITE_DENIED;
            }

            result = DATA_SUCCESS;                                             //填写成功标识

            offset = 0;
            memcpy(&outData->pResultData[offset], inData, sizeof(OOP_OAD_U));  //OMD
            offset += sizeof(OOP_OAD_U);
            outData->pResultData[offset ++] = 0x00;                            //DAR type
            outData->pResultData[offset ++] = 0x00;                            //DATA:NULL
            outData->resultDataLen = offset;
        }
        break;
        case 0x60008300:
        {
            /*删除一个档案*/
            CLEAR_DATA_T delData;
            uint16  data = 0;

            MEMZERO(&delData, sizeof(delData));

            convert.dstBuf = &data;
            convert.dstLen = sizeof(data);
            convert.dOffset = &dOffset;  
            PRTL_FMT_LOGCC("class11_invoke_act 删除一个档案\n");
            nRet = buf_to_data_del_one_meter(pOdaInfo, &convert);
            if (nRet != DATA_SUCCESS)
            {
                return DATA_READ_WRITE_DENIED;
            }
            else
            {
                dOffset = 0;
            }

            PRTL_FMT_LOGCC("删除序号为%d的档案\n", data);

            /*清空档案操作*/
            delData.recordOAD.logicId = 0x00;
            delData.recordOAD.optional = 0;
			delData.recordOAD.classtag = CLASS_DATA_INIT;
            delData.recordOAD.road.oadMain.value = 0x00;
            delData.recordOAD.road.oadCols.nNum = 1;
            delData.recordOAD.road.oadCols.oad[0].value = 0x60000200;
            delData.bClassTag = FALSE;

            delData.recordOAD.infoNum = data;
            ret = db_clear_oad_data(pApduInfo->hUdp, &delData);
            if (ret != ERR_OK)
            {
                PRTL_FMT_LOGCC("删除序号档案%d err = %d\n", i, ret);
                return DATA_READ_WRITE_DENIED;
            }

            ret = meter_data_clear(pApduInfo->hUdp, delData.recordOAD.infoNum);
            if(ret != ERR_OK)
            {
                PRTL_FMT_DEBUG("meter data clear failed. ret(%d), omd(0x%08x)\n", ret, pOdaInfo->pTab->oad.value);
                return DATA_READ_WRITE_DENIED;
            }

            result = DATA_SUCCESS;                                             //填写成功标识

            offset = 0;
            memcpy(&outData->pResultData[offset], inData, sizeof(OOP_OAD_U));  //OMD
            offset += sizeof(OOP_OAD_U);
            outData->pResultData[offset ++] = 0x00;                            //DAR type
            outData->pResultData[offset ++] = 0x00;                            //DATA:NULL
            outData->resultDataLen = offset;
        }
        break;
        case 0x60008400:
        {
            /*删除档案，通过基本信息*/
            CLEAR_DATA_T delData;
            OOP_METER_BASIC_T  data;

            MEMZERO(&delData, sizeof(delData));
            MEMZERO(&data, sizeof(data));

            convert.dstBuf = &data;
            convert.dstLen = sizeof(data);
            convert.dOffset = &dOffset;  
            PRTL_FMT_LOGCC("class11_invoke_act 删除档案act132\n");
            nRet = buf_to_data_del_one_meter_by_basic(pOdaInfo, &convert);
            if (nRet != DATA_SUCCESS)
            {
                return DATA_READ_WRITE_DENIED;
            }
            else
            {
                dOffset = 0;
            }

            /*清空档案操作*/
            delData.recordOAD.logicId = 0x00;
            delData.recordOAD.optional = 0;
			delData.recordOAD.classtag = CLASS_DATA_INIT;
            delData.recordOAD.road.oadMain.value = 0x00;
            delData.recordOAD.road.oadCols.nNum = 1;
            delData.recordOAD.road.oadCols.oad[0].value = 0x60000200;
            delData.bClassTag = FALSE;

            for (i = 1; i <= NO_VIR_MTR_MAX_NUM; i++)
            {
                delData.recordOAD.infoNum = i;

                //先读出原电表信息
                meterret = app_get_meter_list(i, &oldOopMeter);
                if(meterret != ERR_OK)
                {
                    PRTL_FMT_TRACE("get old meter list. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                        meterret, nomalOad.logicId, nomalOad.infoNum, nomalOad.oad.value);
                    continue;
                }

                if (memcmp(&data, &oldOopMeter.basic, sizeof(data)) == 0)
                {
                    PRTL_FMT_LOGCC("删除序号为%d的档案\n", i);
                }
                else
                {
                    continue;
                }
                

                ret = db_clear_oad_data(pApduInfo->hUdp, &delData);
                if (ret != ERR_OK)
                {
                    PRTL_FMT_LOGCC("删除序号档案%d err = %d\n", i, ret);
                    return DATA_READ_WRITE_DENIED;
                }

                ret = meter_data_clear(pApduInfo->hUdp, delData.recordOAD.infoNum);
                if(ret != ERR_OK)
                {
                    PRTL_FMT_DEBUG("meter data clear failed. ret(%d), omd(0x%08x)\n", ret, pOdaInfo->pTab->oad.value);
                    return DATA_READ_WRITE_DENIED;
                }
                
            }

            result = DATA_SUCCESS;                                             //填写成功标识

            offset = 0;
            memcpy(&outData->pResultData[offset], inData, sizeof(OOP_OAD_U));  //OMD
            offset += sizeof(OOP_OAD_U);
            outData->pResultData[offset ++] = 0x00;                            //DAR type
            outData->pResultData[offset ++] = 0x00;                            //DATA:NULL
            outData->resultDataLen = offset;
        }
        break;
        case 0x60008500:
        {
            /*删除档案，通过基本信息*/
            CLEAR_DATA_T delData;
            OOP_METER_M133_T  data;

            MEMZERO(&delData, sizeof(delData));
            MEMZERO(&data, sizeof(data));

            convert.dstBuf = &data;
            convert.dstLen = sizeof(data);
            convert.dOffset = &dOffset;  
            PRTL_FMT_LOGCC("class11_invoke_act 删除档案act133\n");
            nRet = buf_to_data_del_one_meter_by_act133(pOdaInfo, &convert);
            if (nRet != DATA_SUCCESS)
            {
                return DATA_READ_WRITE_DENIED;
            }
            else
            {
                dOffset = 0;
            }

            /*清空档案操作*/
            delData.recordOAD.logicId = 0x00;
            delData.recordOAD.optional = 0;
			delData.recordOAD.classtag = CLASS_DATA_INIT;
            delData.recordOAD.road.oadMain.value = 0x00;
            delData.recordOAD.road.oadCols.nNum = 1;
            delData.recordOAD.road.oadCols.oad[0].value = 0x60000200;
            delData.bClassTag = FALSE;

            for (i = 1; i <= NO_VIR_MTR_MAX_NUM; i++)
            {
                delData.recordOAD.infoNum = i;

                //先读出原电表信息
                meterret = app_get_meter_list(i, &oldOopMeter);
                if(meterret != ERR_OK)
                {
                    PRTL_FMT_TRACE("get old meter list. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                        meterret, nomalOad.logicId, i, nomalOad.oad.value);
                    continue;
                }

                if ((memcmp(&data.add, &oldOopMeter.basic.tsa, sizeof(data.add)) == 0) && 
                   (memcmp(&data.port, &oldOopMeter.basic.port, sizeof(data.port)) == 0))
                {
                    PRTL_FMT_LOGCC("删除序号为%d的档案\n", i);
                }
                else
                {
                    continue;
                }
                
                ret = db_clear_oad_data(pApduInfo->hUdp, &delData);
                if (ret != ERR_OK)
                {
                    PRTL_FMT_LOGCC("删除序号档案%d err = %d\n", i, ret);
                    return DATA_READ_WRITE_DENIED;
                }

                ret = meter_data_clear(pApduInfo->hUdp, delData.recordOAD.infoNum);
                if(ret != ERR_OK)
                {
                    PRTL_FMT_DEBUG("meter data clear failed. ret(%d), omd(0x%08x)\n", ret, pOdaInfo->pTab->oad.value);
                    return DATA_READ_WRITE_DENIED;
                }
                
            }

            result = DATA_SUCCESS;                                             //填写成功标识

            offset = 0;
            memcpy(&outData->pResultData[offset], inData, sizeof(OOP_OAD_U));  //OMD
            offset += sizeof(OOP_OAD_U);
            outData->pResultData[offset ++] = 0x00;                            //DAR type
            outData->pResultData[offset ++] = 0x00;                            //DATA:NULL
            outData->resultDataLen = offset;
        }
        break;
        case 0x60008700:
        {
            OOPM_METER_135_T data;
            MEMZERO(&data, sizeof(data));
            convert.dstBuf = &data;
            convert.dstLen = sizeof(data);
            convert.dOffset = &dOffset;  
            PRTL_FMT_LOGCC("class11_invoke_act 更新档案act135\n");
            nRet = area_buf_to_data_oop_meter_tsa_annex(pOdaInfo, &convert);
            if (nRet != DATA_SUCCESS)
            {
                return DATA_READ_WRITE_DENIED;
            }
            else
            {
                dOffset = 0;
            }

            MEMZERO(&oldOopMeter, sizeof(OOP_METER_T));

            //先读出原电表信息
            meterret = app_get_meter_list_by_tsa(data.add, &oldOopMeter);
            if(meterret != ERR_OK)
            {
                PRTL_FMT_TRACE("get old meter list. error\n");
                return DATA_READ_WRITE_DENIED;
            }

            nomalOad.logicId = 0x00;
            nomalOad.infoNum = oldOopMeter.nIndex;
            nomalOad.oad.value = 0x60000200;
            nomalOad.classtag = CLASS_DATA_INIT;

            oopMeter = oldOopMeter;
            oopMeter.annex = data.annex;
            
            ret = db_write_nomal(pApduInfo->hUdp, &nomalOad, (uint8*)&oopMeter, sizeof(OOP_METER_T));
            if (ret != ERR_OK)
            {
                PRTL_FMT_DEBUG("class11_invoke_act_oop_meter db_write_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                    ret, nomalOad.logicId, nomalOad.infoNum, nomalOad.oad.value);
                return DATA_READ_WRITE_DENIED;
            }

            //删档案，或通信地址，端口，规约类型，用户类型发生变化，则清电表数据
            if(meterret == ERR_OK)
            {
                ret = meter_data_if_clear(pApduInfo, &oldOopMeter, &oopMeter);
                if(ret != ERR_OK)
                {
                    PRTL_FMT_DEBUG("meter data clear failed. ret(%d), omd(0x%08x)\n", ret, pOdaInfo->pTab->oad.value);
                    return DATA_READ_WRITE_DENIED;
                }
            }
            
            result = DATA_SUCCESS;                                             //填写成功标识
            offset = 0;
            memcpy(&outData->pResultData[offset], inData, sizeof(OOP_OAD_U));  //OMD
            offset += sizeof(OOP_OAD_U);
            outData->pResultData[offset ++] = 0x00;                            //DAR type
            outData->pResultData[offset ++] = 0x00;                            //DATA:NULL
            outData->resultDataLen = offset;
        }
            break;
    }
    return result;
}
/**
*********************************************************************
* @brief：     从数据中心的数据转698格式报文(档案扩展信息)
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E area_data_to_buf_annex_object(const OAD_INFO_T *pOadInfo, void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen)
{   
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32     offset = *pOffset;
    OOP_METER_ANNEX_T data;
    uint8 *dstBufTmp = (uint8 *)dstBuf;
    uint8 i;
    memcpy(&data, srcData, srcLen);

    //结构类型
    dstBufTmp[offset++] = DT_ARRAY;

    //结构成员个数
    dstBufTmp[offset++] = data.nNum;    

    for(i=0;i<data.nNum;i++)
    {
        dstBufTmp[offset++] = DT_STRUCTURE;
        dstBufTmp[offset++] = 2;
        dar = basic_data_to_buf(E_OAD, dstBufTmp, dstLen-offset, &offset, &data.annexObj[i].oad , sizeof(data.annexObj[i].oad));
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d)\n", dar);
            return dar;
        } 
        dar = basic_data_to_buf(E_LONG_UNSIGNED, dstBufTmp, dstLen-offset, &offset, data.annexObj[i].value , sizeof(data.annexObj[i].oad));
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d)\n", dar);
            return dar;
        } 
    }

    *pOffset = offset;

    return ERR_OK;
}
/**
*********************************************************************
* @brief：     从数据中心的数据转698格式报文(采集档案单元)
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E area_data_to_buf_oop_meter(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_METER_T data;
    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    memcpy(&data, &srcBuf[sOffset], sizeof(data));
    sOffset += sizeof(data);

    //结构类型
    dstBuf[dOffset++] = DT_STRUCTURE;

    //结构成员数
    dstBuf[dOffset++] = 4;

    //配置序号
    dar = basic_data_to_buf(E_LONG_UNSIGNED, dstBuf, dstLen, &dOffset, &data.nIndex, sizeof(data.nIndex));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d)\n", dar);
        return DATA_TYPE_UNMATCHED;
    }

    //基本信息
    dar = data_to_buf_basic_object(pOadInfo, dstBuf, dstLen, &dOffset, &data.basic, sizeof(data.basic));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d)\n", dar);
        return DATA_TYPE_UNMATCHED;
    }

    //扩展信息
    dar = data_to_buf_extended_object(pOadInfo, dstBuf, dstLen, &dOffset, &data.extend, sizeof(data.extend));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d)\n", dar);
        return DATA_TYPE_UNMATCHED;
    }

    //附属信息
    dar = area_data_to_buf_annex_object(pOadInfo, dstBuf, dstLen, &dOffset, &data.annex, sizeof(data.annex));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d)\n", dar);
        return DATA_TYPE_UNMATCHED;
    }

    //避免每步都判断，放最后检查，但检查前可能已经越界
    if((sOffset > srcLen) || (dOffset > dstLen))
    {
        PRTL_FMT_DEBUG("cross the border. sOffset(%d), srcLen(%d), dOffset(%d), dstLen(%d)\n", sOffset, srcLen, dOffset, dstLen);
        return DATA_SCOPE_OF_ACCESS_VIOLATED;        
    }

    *convert->dOffset = dOffset;
    *convert->sOffset = sOffset;
     
    return dar; 
}

/**
*********************************************************************
* @brief：     从数据中心的数据转698格式报文(拓扑识别关系)
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_cbir_result(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    Circuit_Breaker_Identify_Result data;
    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    memcpy(&data, &srcBuf[sOffset], sizeof(data));
    sOffset += sizeof(data);

    //结构类型
    dstBuf[dOffset++] = DT_STRUCTURE;

    //结构成员数
    dstBuf[dOffset++] = 7;

    //识别成功时间
    dar = basic_data_to_buf(E_DATETIME_S, dstBuf, dstLen, &dOffset, &data.Suctime, sizeof(data.Suctime));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d)\n", dar);
        return DATA_TYPE_UNMATCHED;
    }

    //识别信号相位
    dar = basic_data_to_buf(E_ENUM, dstBuf, dstLen, &dOffset, &data.signalphase, sizeof(data.signalphase));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d)\n", dar);
        return DATA_TYPE_UNMATCHED;
    }

    //识别信号强度1
    dar = basic_data_to_buf(E_DOUBLE_LONG, dstBuf, dstLen, &dOffset, &data.Signalstrength1, sizeof(data.Signalstrength1));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d)\n", dar);
        return DATA_TYPE_UNMATCHED;
    }

    //识别信号强度2
    dar = basic_data_to_buf(E_DOUBLE_LONG, dstBuf, dstLen, &dOffset, &data.Signalstrength2, sizeof(data.Signalstrength2));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d)\n", dar);
        return DATA_TYPE_UNMATCHED;
    }

    //背景噪声
    dar = basic_data_to_buf(E_DOUBLE_LONG, dstBuf, dstLen, &dOffset, &data.BackgroundNoise, sizeof(data.BackgroundNoise));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d)\n", dar);
        return DATA_TYPE_UNMATCHED;
    }

    //特征码信息
    dar = basic_data_to_buf(E_OOP_BITSTR16_T, dstBuf, dstLen, &dOffset, &data.Signatureinformation, sizeof(data.Signatureinformation));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d)\n", dar);
        return DATA_TYPE_UNMATCHED;
    }

    //扩展信息
    dar = basic_data_to_buf(E_OOP_OCTETVAR16_T, dstBuf, dstLen, &dOffset, &data.Extendedinformation, sizeof(data.Extendedinformation));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d)\n", dar);
        return DATA_TYPE_UNMATCHED;
    }

    //避免每步都判断，放最后检查，但检查前可能已经越界
    if((sOffset > srcLen) || (dOffset > dstLen))
    {
        PRTL_FMT_DEBUG("cross the border. sOffset(%d), srcLen(%d), dOffset(%d), dstLen(%d)\n", sOffset, srcLen, dOffset, dstLen);
        return DATA_SCOPE_OF_ACCESS_VIOLATED;        
    }

    *convert->dOffset = dOffset;
    *convert->sOffset = sOffset;
     
    return dar; 
}


/**
*********************************************************************
* @brief：     record_cbir_result_read
* @param[in]： inReadRecord  - 筛选需要的数据
               oopRcsd       - 列选择
               readRecNum    - 读记录数量，：0：读全部记录 1：读最近的一条记录
               maxBufLen     - 最大读取buf长度
* @param[out]：outRecordBuf  - 数据记录
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
int record_cbir_result_read(DB_CLIENT hUdp, const READ_NOR_BP_T inReadNor, RELY_INFO_T relyInfo, uint32 maxBufLen, RECORD_BUF_T *outRecordBuf)
{
    int       result = DATA_OTHER_REASON;
    uint32    offset = 0;
    uint32    i = 0;
    int32     ret = ERR_NORMAL;
    OOP_DAR_E nRet = DATA_OTHER_REASON;
    uint32    bufAllLen = 0;
    BP_INFO_T breakInfo = {0};                                                //断点信息
    BOOL      isBufFull = FALSE;
    OAD_INFO_T *pOadInfo = relyInfo.pOadInfo;

    /*清除断点信息, 需要注意应用分帧的未帧*/
    memset(&breakInfo, 0, sizeof(BP_INFO_T));
   
    switch (pOadInfo->pTab->oad.value)
    {
        case 0x4E070200:
        {
            DATA_CONVERT_T convert;
            Circuit_Breaker_Identify_Result cbirResult;                                        //每个电表的档案信息
            uint32 sOffset = 0;
            uint32 dOffset = 0;
            uint32 cbirResultNum = 0;
            uint32 cbirResultNumOffset = 0;

            MEMZERO(&convert, sizeof(convert));
            MEMZERO(&cbirResult, sizeof(cbirResult));

            #if CBIR_READ_MOD == 1
            uint32      len = 0;
            NOMAL_OAD_T nomalOad;
            MEMZERO(&nomalOad, sizeof(nomalOad));
            nomalOad.logicId = 0x00;
            nomalOad.oad.value = 0x4E080200;
            #endif

            PRTL_FMT_LOGCC("获取指定拓扑识别关系\n");

            cbirResultNumOffset = offset;                                     //数量偏移
            outRecordBuf->recordBuf[offset ++] = 0x00;                        //数量，先填0
            bufAllLen = offset;
            for (i = inReadNor.startInfoNum; i <= inReadNor.endInfoNum; i++)
            {
                //有断点时，找到上次执行到的i
                // PRTL_FMT_LOGCC("slicing_is_comtinue before bpBreakInfo->appSlicingFlag = %d\n", relyInfo.bpBreakInfo.appSlicingFlag);
                // PRTL_FMT_LOGCC("slicing_is_comtinue before bpBreakInfo->index = %d\n", relyInfo.bpBreakInfo.index);
                // PRTL_FMT_LOGCC("slicing_is_comtinue before bpBreakInfo->isEnd = %d\n", relyInfo.bpBreakInfo.isEnd);
                // PRTL_FMT_LOGCC("slicing_is_comtinue before bpBreakInfo->objPre = %d\n", relyInfo.bpBreakInfo.objPre);
                // PRTL_FMT_LOGCC("slicing_is_comtinue before bpBreakInfo->seq = %d\n", relyInfo.bpBreakInfo.seq);
                if (slicing_is_comtinue(relyInfo.bpBreakInfo, i))
                {
                    continue;
                }
                //PRTL_FMT_LOGCC("slicing_is_comtinue after i = %d\n", i);

                #if CBIR_READ_MOD == 1
                nomalOad.infoNum = i;
                ret = db_read_nomal(hUdp, &nomalOad, sizeof(Circuit_Breaker_Identify_Result), (uint8*)&cbirResult, &len);
                if ((ret < ERR_OK) || (len != sizeof(Circuit_Breaker_Identify_Result)))
                #else
                ret = app_get_cbir_result_list(i, &cbirResult);
                if (ret < ERR_OK)
                #endif
                {
                    continue;
                }

                PRTL_FMT_LOGCC("找到找到拓扑识别关系结果 序号为%d\n", i);

                //转换成报文数据
                convert.srcBuf = &cbirResult;
                convert.srcLen = sizeof(cbirResult);
                convert.sOffset = &sOffset;

                convert.dstBuf = &outRecordBuf->recordBuf[offset];
                convert.dstLen = maxBufLen - offset;
                convert.dOffset = &dOffset;

                //记录断点信息
                breakInfo.index = i;

                nRet = data_to_buf_cbir_result(pOadInfo, &convert);  //入参
                if (nRet == DATA_SUCCESS)
                {
                    bufAllLen += dOffset;
                    if (bufAllLen <= maxBufLen)
                    {
                        memcpy(&outRecordBuf->recordBuf[offset], convert.dstBuf, dOffset);
                        offset += dOffset;
                        outRecordBuf->bufLen = offset;
                        PRTL_BUF_LOGCC(convert.dstBuf, dOffset, "第%d个搜表报文:", i);
                        cbirResultNum ++;
                        dOffset = 0;
                        sOffset = 0;
                    }
                    else
                    {
                        dOffset = 0;
                        sOffset = 0;
                        isBufFull = TRUE;
                        PRTL_FMT_LOGCC("record_searchmeter_res_all_read isBufFull == TRUE isBufFull = %d\n", isBufFull);
                    }
                }
                else
                {
                    continue;
                }

                if ((isBufFull) && (offset > 0))
                {
                    break;
                }
            }

            if (!isBufFull)
            {
                if (relyInfo.bpBreakInfo->appSlicingFlag == 0x01)
                {
                    breakInfo.appSlicingFlag = 0x01;
                    breakInfo.isEnd = 0x01;                           //末尾帧
                    breakInfo.seq = relyInfo.bpBreakInfo->seq + 1;    //序号加1
                    breakInfo.objPre = 0x01;                          //对象属性
                }
                else
                {
                    /*清除断点信息*/
                    memset(&breakInfo, 0, sizeof(BP_INFO_T));
                }
            }
            else
            {
                breakInfo.reqType = 0x03;
                breakInfo.appSlicingFlag = 0x01;
                breakInfo.isEnd = 0x00;                              //不是末尾帧
                breakInfo.objPre = 0x01;                             //对象属性
                if (relyInfo.bpBreakInfo->appSlicingFlag == 0x00)
                {
                    breakInfo.seq = BRK_SEQ_START;                   //首次序号
                }
                else
                {
                    breakInfo.seq = relyInfo.bpBreakInfo->seq + 1;  //序号加1
                }
            }

            //刷新断点信息
            memcpy(&outRecordBuf->breakInfo, &breakInfo, sizeof(BP_INFO_T));

            PRTL_BUF_LOGCC(outRecordBuf->recordBuf, offset, "整个拓扑识别关系报文:");
            PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->appSlicingFlag = %d\n", outRecordBuf->breakInfo.appSlicingFlag);
            PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->index = %d\n", outRecordBuf->breakInfo.index);
            PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->isEnd = %d\n", outRecordBuf->breakInfo.isEnd);
            PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->objPre = %d\n", outRecordBuf->breakInfo.objPre);
            PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->seq = %d\n", outRecordBuf->breakInfo.seq);
            PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->reqType = %d\n", outRecordBuf->breakInfo.reqType);

            result = DATA_SUCCESS;
            outRecordBuf->recordBuf[cbirResultNumOffset] = cbirResultNum;   //重新记数量
            outRecordBuf->bufLen = offset;
        }
        break;

        default:
        {
            return DATA_READ_WRITE_DENIED;
        }
        break;
    }

    return result;
}

/**
*********************************************************************
* @name：      class11_invoke_get_cbir_result_nor
* @brief：     class11的GET服务
* @param[in] ：pOdaInfo        OAD信息
               pBpInfo        应用分帧断点信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
               outDataMax     输出报文缓冲区最大长度

* @param[out]：outData     输出报文
* @return    ：OOP_DAR_E
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E class8_invoke_get_cbir_result_nor(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 inLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int       result = DATA_OTHER_REASON;
    uint32    offset = 0;
    uint16    i = 0;
    int32     ret = ERR_NORMAL;
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32    bufAllLen = 0;
    BP_INFO_T breakInfo = {0};                                                //断点信息
    uint32    maxBufLen = 0;
    BOOL      isBufFull = FALSE;

    maxBufLen = pApduInfo->connectionInfo.ApplSplitLen - cal_excludeLen(pApduInfo->connectionInfo);

    /*清除断点信息, 需要注意应用分帧的未帧*/
    memset(&breakInfo, 0, sizeof(BP_INFO_T));
   
    switch (pOadInfo->pTab->oad.value)
    {
        case 0x4E070200:
        {
            uint32 sOffset = 0;
            uint32 dOffset = 0;
            Circuit_Breaker_Identify_Result data;
            DATA_CONVERT_T convert;
            uint32 cbirResultNum = 0;
            uint32 cbirResultNumOffset = 0;

            MEMZERO(&data, sizeof(data));
            MEMZERO(&convert, sizeof(convert));

            #if CBIR_READ_MOD == 1
            uint32 len = 0;
            NOMAL_OAD_T normal;
            MEMZERO(&normal, sizeof(normal));
            nomalOad.logicId = 0x00;
            normal.oad.value = 0x4E080200;
            #endif

            PRTL_FMT_LOGCC("获取所有拓扑识别关系结果\n");

            //获取搜表结果数量，暂时不用，先直接搜索2040个
            // if (oop_searchmeter_res_num(pApduInfo->hUdp, &cbirResultNum) < ERR_OK)
            // {
            //  return DATA_DATA_BLOCK_UNAVAILABLE;
            // }

            memcpy(&outData->pResultData[offset], inData, sizeof(OOP_OAD_U));   //拷贝OAD
            offset += sizeof(OOP_OAD_U);
            outData->pResultData[offset ++] = 0x01;                             //结果
            outData->pResultData[offset ++] = DT_ARRAY;                         //类型
            cbirResultNumOffset = offset;                                    //档案数量偏移
            outData->pResultData[offset ++] = 0x00;                             //数量，先填0
            bufAllLen = offset;
            for (i = 1; i <= CBIR_MAX_NUM; i++)
            {
                //有断点时，找到上次执行到的i
                // PRTL_FMT_LOGCC("slicing_is_comtinue_oop_meter before bpBreakInfo->appSlicingFlag = %d\n", pApduInfo->bpBreakInfo->appSlicingFlag);
                // PRTL_FMT_LOGCC("slicing_is_comtinue_oop_meter before bpBreakInfo->index = %d\n", pApduInfo->bpBreakInfo->index);
                // PRTL_FMT_LOGCC("slicing_is_comtinue_oop_meter before bpBreakInfo->isEnd = %d\n", pApduInfo->bpBreakInfo->isEnd);
                // PRTL_FMT_LOGCC("slicing_is_comtinue_oop_meter before bpBreakInfo->objPre = %d\n", pApduInfo->bpBreakInfo->objPre);
                // PRTL_FMT_LOGCC("slicing_is_comtinue_oop_meter before bpBreakInfo->seq = %d\n", pApduInfo->bpBreakInfo->seq);
                if (slicing_is_comtinue(pApduInfo->bpBreakInfo, i))
                {
                    continue;
                }
                //PRTL_FMT_LOGCC("slicing_is_comtinue_oop_meter after i = %d\n", i);

                #if CBIR_READ_MOD == 1
                nomalOad.infoNum = i;
                ret = db_read_nomal(pApduInfo->hUdp, &normal, sizeof(data), (uint8*)&data, &len);
                if ((ret < ERR_OK) || (len != sizeof(Circuit_Breaker_Identify_Result)))
                #else
                ret = app_get_cbir_result_list(i, &data);
                if (ret < ERR_OK)
                #endif
                {
                    continue;
                }
                PRTL_FMT_LOGCC("找到找到拓扑识别关系结果 序号为%d\n", i);

                //转换成报文数据
                convert.srcBuf = &data;
                convert.srcLen = sizeof(data);
                convert.sOffset = &sOffset;
                
                convert.dstBuf = &outData->pResultData[offset];
                convert.dstLen = outDataMax - offset;
                convert.dOffset = &dOffset;

                //记录断点信息
                breakInfo.index = i;

                dar = data_to_buf_cbir_result(pOadInfo, &convert);
                if (dar == DATA_SUCCESS)
                {
                    bufAllLen += dOffset;
                    if (bufAllLen <= maxBufLen)
                    {
                        memcpy(&outData->pResultData[offset], convert.dstBuf, dOffset);
                        offset += dOffset;
                        outData->resultDataLen = offset;
                        PRTL_BUF_LOGCC(convert.dstBuf, dOffset, "第%d个拓扑识别关系报文:", i);
                        cbirResultNum ++;
                        dOffset = 0;
                        sOffset = 0;
                    }
                    else
                    {
                        dOffset = 0;
                        sOffset = 0;
                        isBufFull = TRUE;
                        PRTL_FMT_LOGCC("class8_invoke_get_cbir_result_nor isBufFull == TRUE isBufFull = %d\n", isBufFull);
                    }
                }
                else
                {
                    continue;
                }

                if ((isBufFull) && (offset > 0))
                {
                    break;
                }
            }

            if (!isBufFull)
            {
                if (pApduInfo->bpBreakInfo->appSlicingFlag == 0x01)
                {
                    breakInfo.appSlicingFlag = 0x01;
                    breakInfo.isEnd = 0x01;                           //末尾帧
                    breakInfo.seq = pApduInfo->bpBreakInfo->seq + 1;  //序号加1
                    breakInfo.objPre = 0x01;                          //对象属性
                }
                else
                {
                    /*清除断点信息*/
                    memset(&breakInfo, 0, sizeof(BP_INFO_T));
                }
            }
            else
            {
                breakInfo.reqType = pApduInfo->reqType;
                breakInfo.appSlicingFlag = 0x01;
                breakInfo.isEnd = 0x00;                              //不是末尾帧
                breakInfo.objPre = 0x01;                             //对象属性
                if (pApduInfo->bpBreakInfo->appSlicingFlag == 0x00)
                {
                    breakInfo.seq = BRK_SEQ_START;                   //首次序号
                }
                else
                {
                    breakInfo.seq = pApduInfo->bpBreakInfo->seq + 1;  //序号加1
                }
            }

            //刷新断点信息
            memcpy(pApduInfo->bpBreakInfo, &breakInfo, sizeof(BP_INFO_T));

            PRTL_BUF_LOGCC(outData->pResultData, offset, "整个拓扑识别报文:");
            PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->appSlicingFlag = %d\n", pApduInfo->bpBreakInfo->appSlicingFlag);
            PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->index = %d\n", pApduInfo->bpBreakInfo->index);
            PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->isEnd = %d\n", pApduInfo->bpBreakInfo->isEnd);
            PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->objPre = %d\n", pApduInfo->bpBreakInfo->objPre);
            PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->seq = %d\n", pApduInfo->bpBreakInfo->seq);
            PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->reqType = %d\n", pApduInfo->bpBreakInfo->reqType);

            if (cbirResultNum > 0)
            {
                result = DATA_SUCCESS;
                outData->pResultData[cbirResultNumOffset] = cbirResultNum;   //重新记数量
                outData->resultDataLen = offset;
            }
            else
            {
                result = DATA_SUCCESS;
                outData->pResultData[cbirResultNumOffset] = 0;   //重新记档案数量
                outData->resultDataLen = offset;
            }
        }
        break;

        default:
        {
            return DATA_READ_WRITE_DENIED;
        }
        break;
    }
    return result;
}

/**
*********************************************************************
* @name：      class11_invoke_get_cbir_result_rec
* @brief：     class11的GET服务
* @param[in] ：pOdaInfo        OAD信息
               pBpInfo        应用分帧断点信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
               outDataMax     输出报文缓冲区最大长度

* @param[out]：outData     输出报文
* @return    ：OOP_DAR_E
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E class8_invoke_get_cbir_result_rec(OAD_INFO_T *pOdaInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    uint16 i = 0;
    uint16 j = 0;
    OOP_DAR_E  nRet = DATA_OTHER_REASON;
    uint32 offset = 4;
    uint16 csdStart = 0, csdLen = 0;
    uint16 outOffset = 0;
    uint16 inDataLenTmp = InDataLen;

    /* 类型不存在 */
    if (pOdaInfo->pTab->oad.nObjID != 0x4E07)
    {
        return DATA_OBJECT_UNAVAILABLE;
    }

    switch (pOdaInfo->pTab->oad.attID)
    {
    case 2:
        {
            OOP_PRSD_T oopRsd;
            OOP_RCSD_T oopRcsd;
            uint16 headLen = 0;
            READ_NOR_BP_T inReadNor;
            RELY_INFO_T  relyInfo;
            RECORD_BUF_T outRecordBuf;

            /* 清0处理 */
            MEMZERO(&oopRsd, sizeof(oopRsd));
            MEMZERO(&oopRcsd, sizeof(oopRcsd));
            MEMZERO(&inReadNor, sizeof(inReadNor));
            MEMZERO(&relyInfo, sizeof(relyInfo));
            MEMZERO(&outRecordBuf, sizeof(outRecordBuf));

            outRecordBuf.recordBuf = (uint8 *)malloc(pApduInfo->connectionInfo.ApplSplitLen);
            memset(outRecordBuf.recordBuf, 0xee, pApduInfo->connectionInfo.ApplSplitLen);

            /*处理RSD*/
            nRet = apdu_rsd_fromfmt698(inData, inDataLenTmp, &offset, &oopRsd, sizeof(oopRsd));
            PRTL_FMT_LOGCC("oopRsd.choice = %d\n", oopRsd.choice);
            PRTL_FMT_LOGCC("oopRsd.sel1.oad.value = 0x%08x\n", oopRsd.sel1.oad.value);
            PRTL_FMT_LOGCC("oopRsd.sel1.value.wVal = %d\n", oopRsd.sel1.value.wVal);
            PRTL_FMT_LOGCC("offset = %d\n", offset);
            if (nRet != DATA_SUCCESS)
            {
                PRTL_FMT_TRACE("RSD解析错误\n");

                /*应对电科院协议一致性测试*/
                uint16 testOffset = 4;
                uint16 inCsdOff = 0, inCsdLen = 0;
                apdu_next_dpos_get(inData, &testOffset, InDataLen, DT_RSD, TRUE);
                inCsdOff = testOffset;
                apdu_next_dpos_get(inData, &testOffset, InDataLen, DT_RCSD, TRUE);
                inCsdLen = testOffset - inCsdOff;
                memcpy(outData->pResultData, inData, 4);
                memcpy(outData->pResultData + 4, inData + inCsdOff, inCsdLen);
                outData->resultDataLen = 4 + inCsdLen;

                outData->pResultData[outData->resultDataLen++] = 1;                   //后面优化下，outData->resultDataLen与下面统一
                if (apdu_rcsd_fromfmt698((uint8 *)inData, InDataLen, &offset, &oopRcsd, sizeof(oopRcsd)) != DATA_SUCCESS)
                {
                    outData->pResultData[outData->resultDataLen++] = 0;

                }
                else
                {
                    outData->pResultData[outData->resultDataLen++] = 1;
                    for (i = 0; i < oopRcsd.nNum; i++)
                    {
                        outData->pResultData[outData->resultDataLen++] = NULL_DATA;
                    }
                }

                free(outRecordBuf.recordBuf);
                outRecordBuf.recordBuf = NULL;
                return DATA_SUCCESS;
            }
            else
            {
                inDataLenTmp -= offset;  //除去RSD和OAD后的长度
            }

            /*处理RCSD*/
            csdStart = offset;
            if (apdu_rcsd_fromfmt698(inData, InDataLen, &offset, &oopRcsd, sizeof(oopRcsd)) != DATA_SUCCESS)
            {
                PRTL_FMT_TRACE("CSD解析错误!\n");
                free(outRecordBuf.recordBuf);
                outRecordBuf.recordBuf = NULL;
                return DATA_TYPE_UNMATCHED;
            }

            /*填写OAD+RCSD+记录结果1 并记下偏移headLen*/
            outData->resultDataLen = 0;
            csdLen = offset-csdStart;
            memcpy(outData->pResultData, inData, 4);
            outOffset = 4;

            if (csdLen == 1)
            {
                //如果rcsd为空，填充OAD60010200
                outData->pResultData[outOffset ++] = 0x01;
                outData->pResultData[outOffset ++] = 0x00;
                outData->pResultData[outOffset ++] = 0x4E;
                outData->pResultData[outOffset ++] = 0x08;
                outData->pResultData[outOffset ++] = 0x02;
                outData->pResultData[outOffset ++] = 0x00;
            }
            else
            {
                memcpy(&outData->pResultData[outOffset], &inData[csdStart], csdLen);
                outOffset += csdLen;
            }
            outData->pResultData[outOffset++] = 1;
            outData->resultDataLen += outOffset;
            headLen = outOffset;
            PRTL_BUF_LOGCC(&outData->pResultData[0], headLen, "填写OAD+RCSD+记录结果1 并记下偏移headLen head:");

            /*记录型档案接口提供输入参数*/
            if (read_nor_param_get(&oopRsd, &oopRcsd, pApduInfo, &inReadNor, &relyInfo) != ERR_OK)
            {
                free(outRecordBuf.recordBuf);
                outRecordBuf.recordBuf = NULL;
                return DATA_READ_WRITE_DENIED;
            }

            /* 依赖信息中存下OAD列表的指针 */
            relyInfo.pOadInfo = pOdaInfo;

            PRTL_FMT_LOGCC("****************************************************\n");
            PRTL_FMT_LOGCC("inReadNor.startInfoNum = %d\n", inReadNor.startInfoNum);
            PRTL_FMT_LOGCC("inReadNor.endInfoNum = %d\n", inReadNor.endInfoNum);
            PRTL_FMT_LOGCC("relyInfo.oopRcsd.nNum = %d\n", relyInfo.oopRcsd.nNum);
            for(i = 0; i < relyInfo.oopRcsd.nNum; i++)
            {
                if (relyInfo.oopRcsd.cols[i].choice == 0)
                {
                    PRTL_FMT_LOGCC("choice = %d relyInfo.oopRcsd.cols[%d].oad.value = 0x%08x\n", i, relyInfo.oopRcsd.cols[i].choice, 
                                                                                              relyInfo.oopRcsd.cols[i].oad.value);
                }
                else if(relyInfo.oopRcsd.cols[i].choice == 1)
                {
                    PRTL_FMT_LOGCC("choice = %d relyInfo.oopRcsd.cols[%d].road.oadMain.value = 0x%08x\n", i, relyInfo.oopRcsd.cols[i].choice, 
                                                                                                      relyInfo.oopRcsd.cols[i].road.oadMain.value);
                    for (j = 0; j < relyInfo.oopRcsd.cols[i].road.oadCols.nNum; j++)
                    {
                        PRTL_FMT_LOGCC("choice = %d relyInfo.oopRcsd.cols[%d].road.oadCols.oad[%d].value = 0x%08x\n", i, j, relyInfo.oopRcsd.cols[i].choice, 
                                                                                                                  relyInfo.oopRcsd.cols[i].road.oadCols.oad[j].value);
                    }
                }
            }
            PRTL_FMT_LOGCC("****************************************************\n");

            /*读记录接口，可以读出apdu数据域，可返回断点信息*/
            if (record_cbir_result_read(pApduInfo->hUdp, inReadNor, relyInfo, pApduInfo->connectionInfo.ApplSplitLen - headLen - cal_excludeLen(pApduInfo->connectionInfo), &outRecordBuf))
            {
                nRet = DATA_READ_WRITE_DENIED;
            }
            else
            {
                memcpy(&outData->pResultData[headLen], outRecordBuf.recordBuf, outRecordBuf.bufLen);
                outData->resultDataLen += outRecordBuf.bufLen;

                if (outRecordBuf.breakInfo.appSlicingFlag == 1)
                {
                    /*记下断点信息*/
                    PRTL_FMT_LOGCC("记下断点信息\n");
                    outRecordBuf.breakInfo.objPre = 0x02;   //对象属性，记录型数据
                    memcpy(pApduInfo->bpBreakInfo, &outRecordBuf.breakInfo, sizeof(BP_INFO_T));
                }
                else
                {
                    /*清除断点信息*/
                    PRTL_FMT_LOGCC("清除断点信息\n");
                    memset(pApduInfo->bpBreakInfo, 0, sizeof(BP_INFO_T));
                }
                
                nRet = DATA_SUCCESS;
            }
            
            free(outRecordBuf.recordBuf);
            outRecordBuf.recordBuf = NULL;
            return DATA_SUCCESS;
        }
        break;
    default:
        {
            return DATA_OBJECT_UNAVAILABLE;
        }
        break;
    }

    return nRet;
}

/**
*********************************************************************
* @name：      class11_invoke_get_cbir_result
* @brief：     class11的GET服务
* @param[in] ：pOdaInfo        OAD信息
               pBpInfo        应用分帧断点信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
               outDataMax     输出报文缓冲区最大长度

* @param[out]：outData     输出报文
* @return    ：OOP_DAR_E
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E class8_invoke_get_cbir_result(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 inLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    //如果是应用分帧状态，将apdu[1]修改成起始帧方式
    if (pApduInfo->bpBreakInfo->appSlicingFlag == 1)
    {
        pApduInfo->reqType = pApduInfo->bpBreakInfo->reqType;
    }

    switch (pApduInfo->reqType)
    {
    case 0x01:
    case 0x02:
    case 0x06:
        return class8_invoke_get_cbir_result_nor(pOadInfo, pApduInfo, inData, inLen, outData, outDataMax);
        break;
    case 0x03:
    case 0x04:
        return class8_invoke_get_cbir_result_rec(pOadInfo, pApduInfo, inData, inLen, outData, outDataMax);
        break;  
    default:
        break;
    }

    return DATA_OBJECT_UNDEFINED;
}

#endif
