#include "baseCommon.h"
#include "oadInfoList.h"
#include "oopservice.h"
#include "commClass.h"
#include "oopStorage.h"
#include "area.h"
#include "appcomm.h"
#include "oopservice.h"

//将版本信息以全局变量的形式写入各个地区的代码中，统一在 area.h 中引用
#ifdef ADV_PU_AMR
FACTORY_VER_TMP_T g_tVer = 
{
    "XXXX",      /**< 厂商代码           */
    "CC01",      /**< 软件版本号         */
    "220623",    /**< 软件版本日期       */
    "CC01",      /**< 硬件版本号         */
    "220621",    /**< 硬件版本日期       */
    "CA01-01",   /**< 厂家扩展信息       */
};
#elif ADV_ST_AMR
    #ifdef PRODUCT_ECU
    //能源控制器
    FACTORY_VER_TMP_T g_tVer = 
    {
        "XXXX",      /**< 厂商代码           */
        "EC01",      /**< 软件版本号         */
        "220619",    /**< 软件版本日期       */
        "ER01",      /**< 硬件版本号         */
        "220619",    /**< 硬件版本日期       */
        "EA01-01",   /**< 厂家扩展信息       */
    };
    #elif PRODUCT_ZCU_1
    //专变1型
    FACTORY_VER_TMP_T g_tVer = 
    {
        "XXXX",      /**< 厂商代码           */
        "OP01",      /**< 软件版本号, 重庆特殊要求     */
        "220822",    /**< 软件版本日期       */
        "OP01",      /**< 硬件版本号         */
        "220727",    /**< 硬件版本日期       */
        "1C01-01",   /**< 厂家扩展信息       */
    };
    #elif PRODUCT_ZCU_2
    //专变2型
    FACTORY_VER_TMP_T g_tVer = 
    {
        "XXXX",      /**< 厂商代码           */
        "2C01",      /**< 软件版本号         */
        "220715",    /**< 软件版本日期       */
        "Z301",      /**< 硬件版本号         */
        "220715",    /**< 硬件版本日期       */
        "2C01-01",   /**< 厂家扩展信息       */
    };
    #elif PRODUCT_ZCU_3
    //专变3型
    FACTORY_VER_TMP_T g_tVer = 
    {
        "XXXX",      /**< 厂商代码           */
        "3C01",      /**< 软件版本号         */
        "220716",    /**< 软件版本日期       */
        "Z301",      /**< 硬件版本号         */
        "220715",    /**< 硬件版本日期       */
        "3C01-01",   /**< 厂家扩展信息       */
    };
     #elif PRODUCT_ZCU_N
    //QT专变
    FACTORY_VER_TMP_T g_tVer = 
    {
        "XXXX",      /**< 厂商代码           */
        "QC01",      /**< 软件版本号         */
        "210207",    /**< 软件版本日期       */
        "ZN01",      /**< 硬件版本号         */
        "200624",    /**< 硬件版本日期       */
        "NC01-01",   /**< 厂家扩展信息       */
    };
    #endif
#endif


#ifdef ADV_PU_AMR
// 普通方案，列出APDU，长度，数量
const uint8 actDefaultNormalSch[][LOCAL_RECV_FRAM_MAXLEN] = 
{
};
const uint16 actDefaultNormalSchLen[] = 
{
};
const uint32 actDefaultNormalSchNum = sizeof(actDefaultNormalSch)/LOCAL_RECV_FRAM_MAXLEN;


// 事件方案，列出APDU，长度，数量
const uint8 actDefaultEventSch[][LOCAL_RECV_FRAM_MAXLEN] = 
{
};
const uint16 actDefaultEventSchLen[] = 
{
};
const uint32 actDefaultEventSchNum = sizeof(actDefaultEventSch)/LOCAL_RECV_FRAM_MAXLEN;

// 任务，列出APDU，长度，数量
const uint8 actDefaultTask[][LOCAL_RECV_FRAM_MAXLEN] = 
{
};
const uint16 actDefaultTaskLen[] = 
{
};
const uint32 actDefaultTaskNum = sizeof(actDefaultTask)/LOCAL_RECV_FRAM_MAXLEN;

#elif ADV_ST_AMR
// 普通方案，列出APDU，长度，数量
const uint8 actDefaultNormalSch[][LOCAL_RECV_FRAM_MAXLEN] = 
{
};
const uint16 actDefaultNormalSchLen[] = 
{
};
const uint32 actDefaultNormalSchNum = sizeof(actDefaultNormalSch)/LOCAL_RECV_FRAM_MAXLEN;

// 事件方案，列出APDU，长度，数量
const uint8 actDefaultEventSch[][LOCAL_RECV_FRAM_MAXLEN] = 
{  
};
const uint16 actDefaultEventSchLen[] = 
{
};
const uint32 actDefaultEventSchNum = sizeof(actDefaultEventSch)/LOCAL_RECV_FRAM_MAXLEN;


// 任务，列出APDU，长度，数量
const uint8 actDefaultTask[][LOCAL_RECV_FRAM_MAXLEN] = 
{
};
const uint16 actDefaultTaskLen[] = 
{
};

const uint32 actDefaultTaskNum = sizeof(actDefaultTask)/LOCAL_RECV_FRAM_MAXLEN;


// 上报方案，列出APDU，长度，数量
const uint8 actDefaultRptSch[][LOCAL_RECV_FRAM_MAXLEN] = 
{
};

const uint16 actDefaultRptSchLen[] = 
{
};
const uint32 actDefaultRptSchNum = sizeof(actDefaultRptSch)/LOCAL_RECV_FRAM_MAXLEN;


const uint8 actDefaultPara[][LOCAL_RECV_FRAM_MAXLEN] = 
{
};
const uint16 actDefaultParaLen[] =
{
};
const uint32 actDefaultParaNum = sizeof(actDefaultPara)/LOCAL_RECV_FRAM_MAXLEN;

const GRPS_PARAM_T gGprsParam[] =
{
    {0,0,{10,101,1,190},35200,{10,101,1,189},35200,"jsepc.js","",""},
    {0,2,{10,101,1,190},35200,{10,101,1,189},35200,"jsepc4g.js","",""},
    {2,2,{8,0,0,11},    35200,{8,0,0,10},    35200,"jssdlgs.wxcb.jsapn","",""},
    {1,2,{9,0,1,5},     35200,{9,0,1,6},     35200,"private.vpdn","jsepc@jsepc.vpdn.js","jsepc123"},
};

/**********************************************************************
* @name      : apn_group_default_set
* @brief     ：江苏设置默认的主站参数 如果多网络参数没有配置 则设置到多网络参数中
* @param[in] ：
* @param[out]：apnGroup  多网络参数
* @return    ：
**********************************************************************/

void apn_group_default_set(OOP_APNGROUP_T *apnGroup)
{
    uint8 i,num;
    OOP_APNGROUP_T data;
    memset(&data,0x00,sizeof(OOP_APNGROUP_T));

    num = sizeof(gGprsParam)/sizeof(GRPS_PARAM_T);
    for(i = 0;i<num;i++)
    {
        data.net[i].factory = gGprsParam[i].factory;
        data.net[i].network = gGprsParam[i].network;
        data.net[i].apn.apn.nNum = strlen(gGprsParam[i].apn);
        strcpy(data.net[i].apn.apn.value,gGprsParam[i].apn);
        data.net[i].apn.username.nNum = strlen(gGprsParam[i].username);
        strcpy(data.net[i].apn.username.value,gGprsParam[i].username);
        data.net[i].apn.pwd.nNum = strlen(gGprsParam[i].password);
        strcpy(data.net[i].apn.pwd.value,gGprsParam[i].password);
        data.net[i].auth = 0xff;    //自适应
        data.net[i].master.nNum = 2;
        memcpy(data.net[i].master.master[0].ip,gGprsParam[i].ip1,4);
        data.net[i].master.master[0].port  = gGprsParam[i].port1;
        memcpy(data.net[i].master.master[1].ip,gGprsParam[i].ip2,4);
        data.net[i].master.master[1].port  = gGprsParam[i].port2;
    }
    data.nNum = num;
    if(apnGroup)
    {
        memcpy(apnGroup,&data,sizeof(OOP_APNGROUP_T));
    }
}

#endif

//将上面的几个变量填入下表，即可下发默认配置
const APDU_DEF_CFG_T apduDefCfgList[] = 
{
    /* 使用SET(0)还是ACT(1)服务 APDU                           APDULEN                          APDUNUM*/
    {1,                         (uint8 *)actDefaultNormalSch, (uint16 *)actDefaultNormalSchLen, (uint32 *)&actDefaultNormalSchNum},
    {1,                         (uint8 *)actDefaultEventSch,  (uint16 *)actDefaultEventSchLen,  (uint32 *)&actDefaultEventSchNum},
    {1,                         (uint8 *)actDefaultTask,      (uint16 *)actDefaultTaskLen,      (uint32 *)&actDefaultTaskNum},
#ifdef ADV_ST_AMR
    {0,                         (uint8 *)actDefaultPara,      (uint16 *)actDefaultParaLen,      (uint32 *)&actDefaultParaNum},
#endif
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
void set_act_by_protocol_chongqing(int sockdbfd)
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
* @name      : app_set_default_acfile_chongqing
* @brief     ：江苏设置默认的交采档案
* @param[in] ：int sockdbfd    句柄
* @param[out]：
* @return    ：
**********************************************************************/
void app_set_default_acfile_chongqing(int sockdbfd )
{
    NOMAL_OAD_T NomalOad;
    OOP_METER_T gMeterFile;
    uint8 *paddr= NULL;
    uint8 addrlen;
    int ret;
    memset(&NomalOad,0x00,sizeof(NOMAL_OAD_T));
    NomalOad.logicId = 0;
    NomalOad.oad.value =0x60000200;
    NomalOad.classtag = CLASS_DATA_INIT;
    NomalOad.infoNum = 1;
    
    memset(&gMeterFile,0x00,sizeof(OOP_METER_T));
    gMeterFile.nIndex = 1;
    gMeterFile.basic.port.value= 0xf2080201;
    gMeterFile.basic.protocol = 3;
    gMeterFile.basic.pwrType =3;
    gMeterFile.basic.userType = 98;
    addrlen = app_get_localaddr(&paddr);
    if(addrlen!=0)
    {
        gMeterFile.basic.tsa.len = addrlen-1;
        memcpy(gMeterFile.basic.tsa.add,paddr,addrlen);
    }
    PRTL_BUF_DEBUG(paddr,addrlen, "写入默认交采档案 \n");
    ret = db_write_nomal(sockdbfd,&NomalOad,(uint8*)&gMeterFile,sizeof(gMeterFile));
    if(ret !=0)
    {
        PRTL_FMT_DEBUG( "写入默认交采档案失败 \n");
    }
}

/**********************************************************************
* @name      : app_setdefault_cfg_anhui
* @brief     ：进程启动 设置地区默认参数
* @param[in] ：int sockdbfd    句柄
* @param[out]：
* @return    ：
**********************************************************************/
void app_setdefault_cfg_chongqing(int sockdbfd)
{
    set_act_by_protocol_chongqing(sockdbfd);
    app_set_default_acfile_chongqing(sockdbfd);
    return;
}

