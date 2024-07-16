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
    "XXXX",      /**< 厂商代码          */
    "CR01",      /**< 软件版本号         */
    "220821",    /**< 软件版本日期        */
    "CC01",      /**< 硬件版本号         */
    "220619",    /**< 硬件版本日期        */
    "CR01-15",   /**< 厂家扩展信息        */
};
#elif ADV_ST_AMR
    #ifdef PRODUCT_ECU
    //能源控制器
    FACTORY_VER_TMP_T g_tVer = 
    {
        "XXXX",      /**< 厂商代码          */
        "ER01",      /**< 软件版本号         */
        "220825",    /**< 软件版本日期        */
        "EE01",      /**< 硬件版本号         */
        "220619",    /**< 硬件版本日期        */
        "ER01-11",   /**< 厂家扩展信息        */
    };
    #elif PRODUCT_ZCU
    //专变
    FACTORY_VER_TMP_T g_tVer = 
    {
        "XXXX",      /**< 厂商代码          */
        "ZR01",      /**< 软件版本号         */
        "210207",    /**< 软件版本日期        */
        "ZZ01",      /**< 硬件版本号         */
        "200624",    /**< 硬件版本日期        */
        "ZR01-01",   /**< 厂家扩展信息        */
    };
    #endif
#endif

// 普通参数，列出APDU，长度，数量
const uint8 setDefaultPara[][LOCAL_RECV_FRAM_MAXLEN] = 
{
    {0x06, 0x01, 0x00, 0x43, 0x00, 0x08, 0x00, 0x03, 0x01, 0x00},  //设置终端主动上报
    {0x06, 0x01, 0x00, 0x43, 0x00, 0x09, 0x00, 0x03, 0x01, 0x00}, //43000900
    {0x06, 0x01, 0x01, 0x31, 0x01, 0x09, 0x00, 0x03, 0x01, 0x00},  //0x31010900
    {0x06, 0x01, 0x02, 0x31, 0x01, 0x08, 0x00, 0x16, 0x03, 0x00},  //0x31010800
    {0x06, 0x01, 0x01, 0x31, 0x04, 0x09, 0x00, 0x03, 0x01, 0x00},  //0x31040900
    {0x06, 0x01, 0x02, 0x31, 0x04, 0x08, 0x00, 0x16, 0x03, 0x00},  //0x31040800
    {0x06, 0x01, 0x01, 0x31, 0x05, 0x09, 0x00, 0x03, 0x01, 0x00},  //0x31050900
    {0x06, 0x01, 0x02, 0x31, 0x05, 0x08, 0x00, 0x16, 0x03, 0x00},  //0x31050800
    {0x06, 0x01, 0x01, 0x31, 0x06, 0x09, 0x00, 0x03, 0x01, 0x00},  //0x31060900
    {0x06, 0x01, 0x02, 0x31, 0x06, 0x08, 0x00, 0x16, 0x03, 0x00},  //0x31060800
    {0x06, 0x01, 0x26, 0x31, 0x06, 0x06, 0x00, 0x02, 0x02, 0x02, 0x04, 0x04, 0x08, 0xC0, 0x11, 0x01, 0x11, 
     0x05, 0x01, 0x00, 0x02, 0x06, 0x12, 0x00, 0x01, 0x12, 0x10, 0xe0, 0x12, 0x00, 0x05, 0x12, 0x00, 0x01, 
     0x12, 0x05, 0x28, 0x12, 0x06, 0xe0, 0x00},        //0x31060600
    {0x06, 0x01, 0x01, 0x31, 0x0B, 0x09, 0x00, 0x03, 0x01, 0x00},  //0x310B0900
    {0x06, 0x01, 0x02, 0x31, 0x0B, 0x08, 0x00, 0x16, 0x03, 0x00},  //0x310B0800
    {0x06, 0x01, 0x01, 0x31, 0x0D, 0x09, 0x00, 0x03, 0x01, 0x00},  //0x310D0900
    {0x06, 0x01, 0x02, 0x31, 0x0D, 0x08, 0x00, 0x16, 0x03, 0x00},  //0x310D0800
    {0x06, 0x01, 0x01, 0x31, 0x0E, 0x09, 0x00, 0x03, 0x01, 0x00},  //0x310E0900
    {0x06, 0x01, 0x02, 0x31, 0x0E, 0x08, 0x00, 0x16, 0x03, 0x00},  //0x310E0800
    {0x06, 0x01, 0x01, 0x31, 0x0F, 0x09, 0x00, 0x03, 0x01, 0x00},  //0x310F0900
    {0x06, 0x01, 0x02, 0x31, 0x0F, 0x08, 0x00, 0x16, 0x03, 0x00},  //0x310F0800
    {0x06, 0x01, 0x01, 0x31, 0x11, 0x09, 0x00, 0x03, 0x01, 0x00},  //0x31110900
    {0x06, 0x01, 0x02, 0x31, 0x11, 0x08, 0x00, 0x16, 0x03, 0x00},  //0x31110800
    {0x06, 0x01, 0x01, 0x31, 0x14, 0x09, 0x00, 0x03, 0x01, 0x00},  //0x31140900
    {0x06, 0x01, 0x02, 0x30, 0x03, 0x0b, 0x00, 0x16, 0x03, 0x00},  //0x30030B00
    {0x06, 0x01, 0x01, 0x30, 0x03, 0x0c, 0x00, 0x03, 0x01, 0x00},  //0x30030C00
    {0x06, 0x01, 0x26, 0x30, 0x03, 0x05, 0x00, 0x02, 0x03, 0x12,0x05,0x28,0x05,0x00,0x00,0x00,0x4b,0x11,
        0x3c,0x00},                                 //0x30300500
    {0x06, 0x01, 0x02, 0x30, 0x0F, 0x08, 0x00, 0x16, 0x03, 0x00},  //0x300F0800
    {0x06, 0x01, 0x01, 0x30, 0x0F, 0x09, 0x00, 0x03, 0x01, 0x00},  //0x300F0900
    {0x06, 0x01, 0x00, 0x42, 0x04, 0x02, 0x00, 0x02, 0x02, 0x1b, 0x10, 0x32, 0x00, 0x03, 0x01, 0x00 },  //0x300F0900
                     
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
    41,
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
    16,
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
void set_act_by_protocol_shandong(int sockdbfd)
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
void app_setdefault_cfg_shandong(int sockdbfd)
{
    set_act_by_protocol_shandong(sockdbfd);

    return;
}
/**********************************************************************
* @name      : rpt_road_compare
* @brief     比较上报方案和普通采集方案的road是否一样
* @param[in] ：
* @param[out]：
* @return    ：
**********************************************************************/
BOOL rpt_road_compare(OOP_ROAD_T *road1, OOP_ROAD_T *road2)
{
    if(road1->oadMain.value!=road2->oadMain.value)
        return FALSE;
    uint8 i,j;
    uint8 flag = 0;
    for(i=0;i<road1->oadCols.nNum;i++)
    {
        //有的时候普通方案不会配冻结时标
        if(road1->oadCols.oad[i].value == 0x20210200)
            continue;
        for(j=0;j<road2->oadCols.nNum;j++)
        {
            if(road1->oadCols.oad[i].value==road2->oadCols.oad[j].value)
            {
                flag = 1;
                break;
            }

        }
        if(flag ==0)
        {
            RPT_FMT_DEBUG("oad %08x 不在采集方案中\n",road1->oadCols.oad[i].value);
            return FALSE;
        }else
        {
            flag = 0;
        }
    }
    return TRUE;
}

/**********************************************************************
* @name      : rtp_plan_normal_taskid_get
* @brief     ：根据上报方案的内容找到对应的抄表任务 主要判断执行频率 抄读数据项
* @param[in] ：rptContext 上报方案的内容
* @param[out]：
* @return    ：对应的普通采集任务id
**********************************************************************/
uint16 rpt_plan_normal_taskid_get(uint16 value,uint8 unit,REPORT_CONTEXT *rptContext)
{
    OOP_TASK_T task;
    OOP_PLAN_NORMAL_T plan;
    uint16 tmpId = 0;
    uint8 i;
    int ret;
    while(tmpId++ < 255)
    {
        ret = app_get_task_list(tmpId, &task);
        if(ret<0)
            continue;
        if(task.planType!=1 ||task.state!=1)
            continue;
        //判断执行频率是否一样
        if(task.acqFreq.unit!=unit&&task.acqFreq.value!=value)
            continue;
        ret = app_get_plan_normal_list(task.planID,&plan);
        if(ret<0)
            continue;
        //再判断抄读数据项
        if(rptContext->choice ==0)
        {
            if(plan.recdCol.csd[0].choice==0 && plan.recdCol.csd[0].oad.value ==rptContext->oad.value)
            {
                return tmpId;
            }
        }else
        {
            for(i=0;i<rptContext->record.tRCSD.nNum;i++)
            {
                if(rptContext->record.tRCSD.cols[i].choice==1)
                {
                    if(rpt_road_compare(&rptContext->record.tRCSD.cols[i].road,&plan.recdCol.csd[0].road)==TRUE )
                    {
                        return tmpId;
                    }
                }

            }
        }
    }
    return 0;
}

/**********************************************************************
* @name      : rpt_normal_task_finish_check
* @brief     ：定时读取任务对应的60340200判断任务是否执行完毕 一分钟读一次
* @param[in] client 数据中心句柄 
        taskid 对应的任务id
* @param[out]：
* @return    ：任务是否抄完
**********************************************************************/
BOOL rpt_normal_task_finish_check(DB_CLIENT client,uint16 taskid,time_t *lasttime)
{
    if(taskid == 0)
        return FALSE;
    
    time_t timenow = time(NULL);
    OOP_ACQ_MONITOR_T taskinfo;
    memset(&taskinfo,0x00,sizeof(OOP_ACQ_MONITOR_T));
    uint32 len;
    int ret;
    NOMAL_OAD_T normaload;
    memset(&normaload,0x00,sizeof(NOMAL_OAD_T));
    normaload.infoNum = taskid;
    normaload.oad.value = 0x60340200;
    if(timenow-*lasttime>=60)
    {
        *lasttime = timenow;
        ret = db_read_nomal(client, &normaload, sizeof(OOP_ACQ_MONITOR_T), (uint8*)&taskinfo, &len);
        RPT_FMT_DEBUG("rpt_normal_task_finish_check taskid %d ret %d taskstate %d \n",taskid,ret,taskinfo.taskState);
        if(ret!=0||len!=sizeof(OOP_ACQ_MONITOR_T))
        {
            return FALSE;
        }
        if(taskinfo.taskState==2)
            return TRUE;
    }
    return FALSE;
}
