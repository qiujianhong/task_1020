/*-***************************************************************
* Copyright (c) 2006,南京新联电子仪器有限公司
*            All rights reserved.
* 程序名称：Event.c
* 版本号：1.0
* 功能：实现事件的检测与主动上报功能
* 原始开发人及时间：董宁 2006年12月25日
* 修改人员、时间及修改内容:朴军海 2019年7月1日
            增加国网101、104协议所需的部分功能
* 其他：
******************************************************************-*/
#include "Event_zhejiang.h"
#include "EventFun_zhejiang.h"
#include "commfun.h"
#include "framecomm.h"
#include "msg_id.h"
#include "label_id.h"
#include "storage.h"
#include "oopVarUpdate.h"
#include "pw_protocol.h"
#include "pw_all.h"
#include "pw_protocol_zhj.h"
#include "pw_common_zhj.h"

/*-******************************************************************-*/
/*-                             全局变量                             -*/
/*-******************************************************************-*/
#define EventReportCount  28    //上报的事件总量,每一个事件一个文件，需要遍历所有时间文件
extern int ReadMetPara(int mtr_id, StoreMetStruct *InMetPara);


extern uint16 g_nConfigFaultNumZhj[MAX_EQUIP_TYPE_NUM][MAX_EQUIP_GROUP_NUM][MAX_EQUIP_COUNT_NUM];
extern FAULT_REPORT_CONFIG_ZHJ g_aFaultDataConfigZhj[FAULT_MAX_NUM_ZHJ];  //故障信息配置区
extern uint32         g_aFaultDataIndexZhj[MAX_EQUIP_TYPE_NUM][MAX_EQUIP_GROUP_NUM][MAX_EQUIP_COUNT_NUM];  //信号索引

//国网2019年2月发布最新规范
static const FAULT_REPORT_RAMINFO fault_report_ramoff[FAULT_MAX_NUM] = //目前终端只支持交采事件，电表事件不支持
{
    {//终端停电
         RTUSelfCheckAddr,	0,4
	},

	{//配电变压器停电
		 RTUSelfCheckAddr1,	0,4
	},

    {//A相电压越上限
		 GBVltUpAddr, 	    0,4,
	},

	{//B相电压越上限
		GBVltUpAddr,	    1,5,
	},

	{//C相电压越上限
		GBVltUpAddr,	    2,6
	},

    {//A相电流越上限
		GBCurrUpAddr,	    0,4
	},

	{//B相电流越上限
		GBCurrUpAddr,       1,5
	},

	{//C相电流越上限
		GBCurrUpAddr,       2,6
	},

    {//A相电压断相
		 GBLowvltAddr,	    0,4
	},

	{//B相电压断相
		 GBLowvltAddr,	    1,5
	},

	{//C相电压断相
		 GBLowvltAddr,	    2,6
	},
	
	{//A相电压缺相
		 GBLostvltAddr,		0,4
	},

	{//B相电压缺相
		 GBLostvltAddr,		1,5
	},

	{//C相电压缺相
		 GBLostvltAddr,		2,6
	},

	{//电流不平衡
		GBCurrUnbAddr, 	    0,4
	},

    {//负荷过载
		GBEnUpAddr, 	    0,4
	},

    {//相序异常(电压)
		GBVltAbnormalAddr,  0,4
	},

	{//相序异常(电流)
		GBCurrAbnormalAddr, 0,4
	},
    
	{//A相电流反极性
		GBPhaseReveAddr,	0,4
	},

	{//B相电流反极性
		GBPhaseReveAddr,	1,5
	},

	{//C相电流反极性
		GBPhaseReveAddr,	2,6
	},
	
	{//无功过补偿
		GBNoEnUpAddr,       0,4
	},
	
	{//无功欠补偿
		GBNoEnLowAddr,      0,4
	},

    {//零序电流偏大
		GBZeroCurrUpAddr,   0,4
	},
	
	{//电压不平衡
		GBVltUnbAddr,       0,4
	},
	
    {//A 相电压畸变率越限
		GBVltDisUpAddr,	    0,4
	},

	{//B相电压畸变率越限
		GBVltDisUpAddr,	    1,5
	},

	{//C相电压畸变率越限
		GBVltDisUpAddr,	    2,6
	},
	
	{//A相电流畸变率越限
		GBCurrDisUpAddr,	0,4
	},

	{//B相电流畸变率越限
		GBCurrDisUpAddr,	1,5
	},

	{//C相电流畸变率越限
		GBCurrDisUpAddr,	2,6
	},

    {//A相电压越下限
		 GBVltDownAddr, 	0,4,
	},

	{//B相电压越下限
		GBVltDownAddr,	    1,5,
	},

	{//C相电压越下限
		GBVltDownAddr,	    2,6
	},
	
};

static const uint32 fault_report_ramsize = sizeof(fault_report_ramoff) / sizeof(fault_report_ramoff[0]);
static const REPORT_DATA g_report_data_list[] =
{
    {0x00010010, &g_LatestACData.pdemand.demand[0].nValue},
    {0x00010015, &g_LatestACData.ndemand.demand[0].nValue},
    {0x00200001, &g_LatestACData.penergy.nValue[0]},
    {0x00200002, &g_LatestACData.penergy.nValue[1]},
    {0x00200003, &g_LatestACData.penergy.nValue[2]},
    {0x00200004, &g_LatestACData.penergy.nValue[3]},
    {0x00200005, &g_LatestACData.penergy.nValue[4]},
    {0x00200006, &g_LatestACData.nenergy.nValue[0]},
    {0x00200007, &g_LatestACData.nenergy.nValue[1]},
    {0x00200008, &g_LatestACData.nenergy.nValue[2]},
    {0x00200009, &g_LatestACData.nenergy.nValue[3]},
    {0x0020000a, &g_LatestACData.nenergy.nValue[4]},
    {0x00200015, &g_LatestACData.quadrant1.nValue[0]},
    {0x0020001a, &g_LatestACData.quadrant4.nValue[0]},
    {0x0020001f, &g_LatestACData.quadrant2.nValue[0]},
    {0x00200024, &g_LatestACData.quadrant3.nValue[0]},
    {0x00000001, (uint32*)&g_LatestACData.vol.nValue[0]},
    {0x00000002, (uint32*)&g_LatestACData.vol.nValue[1]},
    {0x00000003, (uint32*)&g_LatestACData.vol.nValue[2]},
    {0x00000004, (uint32*)&g_LatestACData.cur.nValue[0]},
    {0x00000005, (uint32*)&g_LatestACData.cur.nValue[1]},
    {0x00000006, (uint32*)&g_LatestACData.cur.nValue[2]},
    {0x0000001d, (uint32*)&g_LatestACData.z_cur},
    {0x00000181, (uint32*)&g_LatestACData.harmonicV[0].nValue[0]},
    {0x00000182, (uint32*)&g_LatestACData.harmonicV[1].nValue[0]},
    {0x00000183, (uint32*)&g_LatestACData.harmonicV[2].nValue[0]},
    {0x00000191, (uint32*)&g_LatestACData.harmonicC[0].nValue[0]},
    {0x00000192, (uint32*)&g_LatestACData.harmonicC[1].nValue[0]},
    {0x00000193, (uint32*)&g_LatestACData.harmonicC[2].nValue[0]}
};
static const uint32 g_report_data_num = sizeof(g_report_data_list) / sizeof(REPORT_DATA);


EVENT_REPORT_SEND faultsenddata[FAULT_MAX_NUM] = {{0}};


/**********************************************************************
* @name      : CleanEventStatusFlag
* @brief     ：初始化测量点事件标志
* @param[in] ：

* @param[out]：
* @return    ：
* @Create    : 梅安宁
* @Date      ：2020-02-24
* @Update    :
**********************************************************************/
void CleanEventStatusFlag()
{
    int i = 0;
    uint32 dwflag = 0;

    EVENT_FMT_TRACE("InitEventStatusFlag set 0!\n");

    for(i=0; i<MTR485_MAX_NUM; i++)
    {
        write_pdata_xram("^Event", (const char *)&dwflag, (EventStatusRecAddr+(i)*4), 4);
    }

    write_pdata_xram("^Event", (const char *)&dwflag, EventFlagRecAddr, 1);
    write_pdata_xram("^Event", (const char *)&dwflag, FluxOverTopAddr, 1);
}

/**********************************************************************
* @name      : GetInitEventInfo
* @brief     ：初始化事件
* @param[in] ：
* @param[out]：
* @return    ：
* @Create    : 梅安宁
* @Date      ：2020-02-14
* @Update    :
**********************************************************************/
int GetInitEventInfo(int clientid,uint8 initType, uint8 logicId,uint16 classTag)
{
    EVENT_FMT_DEBUG("Init event initType=%u, logicId=%u, classTag=%u!\n", initType, logicId, classTag);

    //初始化事件参数
    if(logicId == 7&&(initType == 1 || classTag == CLASS_DATA_EVENT || classTag == 0))
    {
        CleanEventStatusFlag();
        memset(&g_LatestACData, 0, sizeof(LatestACData));
    }

    return 0;
}

#if DESC("交采数据处理",1)

//最新交采数据
LatestACData g_LatestACData;

//普通型数据更新列表
EventUpdateDataInfo g_EventUpdateNormalDataList[] =
{/*     数据标识,   数据长度,     数据内容 */
    {0x00100200,    sizeof(OOP_ENERGY_T),            (uint8 *)&g_LatestACData.penergy},
    {0x00200200,    sizeof(OOP_ENERGY_T),            (uint8 *)&g_LatestACData.nenergy},
    {0x00500200,    sizeof(OOP_ENERGY_T),            (uint8 *)&g_LatestACData.quadrant1},
    {0x00600200,    sizeof(OOP_ENERGY_T),            (uint8 *)&g_LatestACData.quadrant2},
    {0x00700200,    sizeof(OOP_ENERGY_T),            (uint8 *)&g_LatestACData.quadrant3},
    {0x00800200,    sizeof(OOP_ENERGY_T),            (uint8 *)&g_LatestACData.quadrant4},
    {0x10100200,    sizeof(OOP_DEMAND_T),            (uint8 *)&g_LatestACData.pdemand},
    {0x10200200,    sizeof(OOP_DEMAND_T),            (uint8 *)&g_LatestACData.ndemand},
    {0x20000200,    sizeof(OOP_WORD3V_T),            (uint8 *)&g_LatestACData.vol},
    {0x20010200,    sizeof(OOP_INT3V_T),             (uint8 *)&g_LatestACData.cur},
    {0x20010600,    sizeof(int32),                   (uint8 *)&g_LatestACData.z_cur},
    {0x200a0200,    sizeof(OOP_LONG4V_T),            (uint8 *)&g_LatestACData.fpower},
    {0x20020200,    sizeof(OOP_WORD3V_T),            (uint8 *)&g_LatestACData.oopWord3V},
    {0x20030200,    sizeof(OOP_WORD3V_T),            (uint8 *)&g_LatestACData.oopWord3C},
    {0x20140200,    sizeof(OOP_METWORD_T),           (uint8 *)&g_LatestACData.meterRsw},
    {0x200d0200,    sizeof(OOP_HARMONIC_T),          (uint8 *)&g_LatestACData.harmonicV[0]},
    {0x200d0300,    sizeof(OOP_HARMONIC_T),          (uint8 *)&g_LatestACData.harmonicV[1]},
    {0x200d0400,    sizeof(OOP_HARMONIC_T),          (uint8 *)&g_LatestACData.harmonicV[2]},
    {0x200e0200,    sizeof(OOP_HARMONIC_T),          (uint8 *)&g_LatestACData.harmonicC[0]},
    {0x200e0300,    sizeof(OOP_HARMONIC_T),          (uint8 *)&g_LatestACData.harmonicC[1]},
    {0x200e0400,    sizeof(OOP_HARMONIC_T),          (uint8 *)&g_LatestACData.harmonicC[2]},
};

uint16 g_EventUpdateNormalDataNum = sizeof(g_EventUpdateNormalDataList)/sizeof(EventUpdateDataInfo);

/**********************************************************************
* @name      : Init_ac_data_lock
* @brief     ：
* @param[out]：
* @return    ：
* @Create    : 梅安宁
* @Date      ：2020-11-19
* @Update    :
**********************************************************************/
void Init_ac_data_lock(void)
{
    static int firstFlag = 0;
    /* 初始化锁 */
    if(firstFlag == 0)
    {
        firstFlag = 1;
        
        pthread_mutex_init(&g_LatestACData.mutex, NULL);
    }
}

/**********************************************************************
* @name      : ac_data_init
* @brief     ：交采数据初始化
* @param[in] ：int clientid    数据中心句柄
* @param[out]：
* @return    ：
* @Create    : 梅安宁
* @Date      ：2020-11-19
* @Update    :
**********************************************************************/
int ac_data_init(int clientid)
{
    int                i = 0;
    int                ret = 0;
    uint32             readlen = 0;
    uint8              pDataBuf[1024] = {0};
    NOMAL_OAD_T        nomalOAD;
    
    memset(&g_LatestACData, 0, sizeof(LatestACData));
    
    for(i=0; i<g_EventUpdateNormalDataNum; i++)
    {
        memset(&nomalOAD, 0, sizeof(NOMAL_OAD_T));

        nomalOAD.logicId = 1;
        nomalOAD.oad.value = g_EventUpdateNormalDataList[i].oadValue;
        ret = db_read_nomal(clientid, &nomalOAD, 1024, pDataBuf, &readlen);
        if (ret != ERR_OK || readlen == 0)
        {
            EVENT_FMT_DEBUG("init ac data, OAD=%8X  error ret=%d, len=%u\n", nomalOAD.oad.value, ret, readlen);
            continue;
        }

        EVENT_FMT_TRACE("init ac data, OAD=%8X  OK !\n", nomalOAD.oad.value);
        if(g_EventUpdateNormalDataList[i].dateLen == readlen)
        {
            memcpy(g_EventUpdateNormalDataList[i].pBuf, pDataBuf, readlen);
        }
        else
        {
            EVENT_FMT_DEBUG("init ac data, OAD=%8X  readlen[%d]!=datalen[%u]\n", nomalOAD.oad.value, readlen, g_EventUpdateNormalDataList[i].dateLen);
        }
    }

    return 0;
}


/**********************************************************************
* @name      : update_ac_data
* @brief     ：交采数据实时变更
* @param[in] ：int clientid    socket句柄
               uint8 logicId   逻辑地址
               uint16 infoNum  信息点号
               uint32 value    oad值
* @param[out]：
* @return    ：
* @Create    : 梅安宁
* @Date      ：2020-11-19
* @Update    :
**********************************************************************/
int update_ac_data(int clientid, uint8 logicId, uint16 infoNum, uint32 value)
{
    int                i = 0;
    int                ret = 0;
    uint32             readlen = 0;
    uint8              pDataBuf[1024] = {0};
    NOMAL_OAD_T        nomalOAD;

    memset(&nomalOAD, 0, sizeof(NOMAL_OAD_T));

    nomalOAD.logicId = logicId;
    nomalOAD.infoNum = infoNum;
    nomalOAD.oad.value = value;
    ret = db_read_nomal(clientid, &nomalOAD, 1024, pDataBuf, &readlen);
    if (ret != ERR_OK || readlen == 0)
    {
        EVENT_FMT_DEBUG("Updata logicId=%u, infoNum=%u, OAD=%8X  error ret=%d, len=%u\n", logicId, infoNum, value, ret, readlen);
        return ret;
    }

    EVENT_FMT_TRACE("Updata logicId=%u, infoNum=%u, OAD=%8X  OK !\n", logicId, infoNum, value);

    for(i=0; i<g_EventUpdateNormalDataNum; i++)
    {
        if(g_EventUpdateNormalDataList[i].oadValue == value && g_EventUpdateNormalDataList[i].dateLen == readlen)
        {
            /* 使用互斥量进行加锁 */
            pthread_mutex_lock(&g_LatestACData.mutex);
            memcpy(g_EventUpdateNormalDataList[i].pBuf, pDataBuf, readlen);
            pthread_mutex_unlock(&g_LatestACData.mutex);

            return 0;
        }
    }

    return -1;
}

#endif

/**********************************************************************
* @name      : Uint16To32
* @brief     ：两字节拼接成四字节
* @param[in] ：uint16 front
               uint16 rear
* @param[out]：
* @return    ：拼接成四字节数据
* @Create    : 梅安宁
* @Date      ：2019-12-26
* @Update    :
**********************************************************************/
uint32 Uint16To32(uint16 front, uint16 rear)
{
    uint32 value = 0;
    value = (0xffffffff&front)<<16;
    value = value|rear;
    return value;
}

/*********************************************************************
所属功能单元：事件处理模块终端运行检测子模块
功能描述：
  完成终端初始化/版本变更、终端停/上电、遥信变位、总加组电量差动越限事件的检测功能。
输入参数：
  mtr_id  测量点号
输出参数：无
函数返回值：无
内部处理流程：
  顺序完成终端初始化/版本变更、终端停/上电、遥信变位、总加组电量差动越限事件的检测。
********************************************************************/
void RtuExam(int clientid)
{
    
    CheckERC_3106_zhj(clientid);        //终端停上电事件检测
    CheckSelfERC(clientid);         //配电变压器停上电检测
    
}

/*******************************************************************************
* 所属单元：事件处理模块用电现场检测子模块
* 功能描述：测量点告警处理接口。
* 输入参数：mtr_id  测量点号
* 输出参数：无
* 返 回 值：无
* 内部流程：根据当前告警配置信息，调用告警处理函数，处理告警数据。
*******************************************************************************/
void MetExam(int clientid)
{
    CheckERC_3119(clientid);                            //电流反极性检测
    CheckERC_3000(clientid);                            //断相检测
    CheckERC_3001(clientid);                            //电压越下限检测
    CheckERC_3002(clientid);                            //电压越上限检测
    CheckERC_3003(clientid);                            //缺相检测
    CheckERC_3005(clientid);                            //电流越限检测
    CheckERC_300F(clientid);                            //电压逆相序事件
    CheckERC_3010(clientid);                            //电流逆相序事件
    CheckERC_301D(clientid);                            //电压不平衡检测
    CheckERC_301E(clientid);                            //电流不平衡检测
    CheckERC_3031(clientid);                            //电压谐波总畸变率超限检测
    CheckERC_3032(clientid);                            //电流谐波总畸变率超限检测
    CheckERC_3033(clientid);                            //零序电流超限检测
    CheckERC_3034(clientid);                            //无功过补偿检测
    CheckERC_3035(clientid);                            //无功欠补偿检测
    CheckERC_3036(clientid);                            //负荷过载检测
    Check_ApparentPower(clientid);                      //视在功率超限检测
}

#if DESC("电压越限事件",1)
/**********************************************************************
* @name      : CheckERC_3001
* @brief     ：配电电压越下限事件
* @param[in] ：int clientid    数据中心句柄,预留
* @param[out]：
* @return    ：
* @Create    : 梅安宁
* @Date      ：2020-11-19
* @Update    :
**********************************************************************/
void CheckERC_3001(int clientid)
{
    /*- 数据内容                数据格式    字节数
        ERC=4                   BIN         1
        长度Le                  BIN         1
        变位时间：分时日月年    数据格式15  5
        状态变位                BS8         1
        变位后状态              BS8         1-*/
//    if (event_3002.eventCommInfo.checkflag != 1){
//        EVENT_FMT_DEBUG("未使能3002电压越限检测\n");
//        return;
//    }

    if (g_LatestACData.vol.nNum < 1) {
        EVENT_FMT_DEBUG("电压无效(3001)\n");
        return;
    }

    EVENT_FMT_TRACE("CheckERC_3001电压越下限恢复检测开始\n");
    VltSupassEvent(clientid, 4);
    EVENT_FMT_TRACE("CheckERC_3001电压越下限检测开始\n");
    VltSupassEvent(clientid, 3);
}


/**********************************************************************
* @name      : CheckERC_3002
* @brief     ：配电电压越限事件
* @param[in] ：int clientid    数据中心句柄,预留
* @param[out]：
* @return    ：
* @Create    : 梅安宁
* @Date      ：2020-11-19
* @Update    :
**********************************************************************/
void CheckERC_3002(int clientid)
{
    /*- 数据内容                数据格式    字节数
        ERC=4                   BIN         1
        长度Le                  BIN         1
        变位时间：分时日月年    数据格式15  5
        状态变位                BS8         1
        变位后状态              BS8         1-*/
//    if (event_3002.eventCommInfo.checkflag != 1){
//        EVENT_FMT_DEBUG("未使能3002电压越限检测\n");
//        return;
//    }

    if (g_LatestACData.vol.nNum < 1) {
        EVENT_FMT_DEBUG("电压无效(3002)\n");
        return;
    }

    EVENT_FMT_TRACE("CheckERC_3002电压越上限恢复检测开始\n");
    VltSupassEvent(clientid, 2);
    EVENT_FMT_TRACE("CheckERC_3002电压越上限检测开始\n");
    VltSupassEvent(clientid, 1);
}
#endif

/*******************************************************************************
* 函数名称:     crc_calc
* 函数功能:     电表校验未计算
* 输入参数：    databuf
                datalen
* 输出参数：
* 内部处理：
* 返 回 值:     NULL
*******************************************************************************/
uint8 crc_calc(char *databuf, uint8 datalen)
{
    uint8    start, i;
    uint8    crc = 0;

    start = 0;
    while (start < datalen)
    {
        if (databuf[start] == 0x68)
        {
            break;
        }
        start++;
    }

    for (i = start; i < datalen; i++)
    {
        crc += databuf[i];
    }

    return crc;
}
void PrintBuf(uint8 *buf, int len)
{
    int i;

    for (i = 0; i < len; i++)
    {
        DPRINT("%02x ", buf[i]);
    }

    if (len > 0)
    {
        DPRINT("\n");
    }
}

void ConverToOopTime(char *Srcuff, uint8 *desBuff)
{
    uint16 year = 0;
    if (!Srcuff || !desBuff)
    {
        return;
    }
    year = 2000 + commfun_BcdToHex(Srcuff[5]);
    desBuff[0] = year >> 8;
    desBuff[1] = year & 0xff;
    desBuff[2] = commfun_BcdToHex(Srcuff[4]);
    desBuff[3] = commfun_BcdToHex(Srcuff[3]);
    desBuff[4] = commfun_BcdToHex(Srcuff[2]);
    desBuff[5] = commfun_BcdToHex(Srcuff[1]);
    desBuff[6] = commfun_BcdToHex(Srcuff[0]);
    return;
}

/*******************************************************************************
* 函数名称: OopSpeedSwitchToMode
* 函数功能: 速度转换成模式字
* 输入参数:
* 输出参数:
* 内部处理:
* 返 回 值: mode
*******************************************************************************/
uint8 OopSpeedSwitchToMode(uint8 speed)
{
    uint8 mode = 0;

    switch (speed)
    {
    case 0x0B:
        mode = 0;
        break;

    case 0x2B:
        mode = 1;
        break;

    case 0x4B:
        mode = 2;
        break;

    case 0x6B:
        mode = 3;
        break;

    case 0x8B:
        mode = 4;
        break;

    case 0xAB:
        mode = 5;
        break;

    case 0xCB:
        mode = 6;
        break;

    case 0xEB:
        mode = 7;
        break;

    default:
        mode = 3;
        break;
    }

    return mode;
}

#if DESC("停上电事件",1)

/*******************************************************************************
* 函数名称: PowerDownCheck
* 函数功能: 终端的停上电检测,上次上电到当前时间的差值
* 输入参数: void
* 输出参数: 
* 内部处理:
* 返 回 值: 无
*******************************************************************************/
int PowerDownCheck_shj(uint8 btype)
{
    int len = -1;
    OOP_DATETIME_S_T last_time = {0};    //-停电时间-
    time_t nowSecond, lastSecond;

    len = read_pdata_xram("^Event", (char *)&last_time, ProgDownAddr+btype*sizeof(OOP_DATETIME_S_T), sizeof(OOP_DATETIME_S_T));
    if (len < 0)    //-读停电时间-
        return 0;

    if ((last_time.month == 0) || (last_time.month > 0x12) ||
        (last_time.day == 0) || (last_time.day > 0x31))
        return 0;

    OopDateTimeSToLinuxTime(&last_time, &lastSecond);

    //-比较60S,超过算上电-
    nowSecond = time(NULL);

    return ((nowSecond - lastSecond) > 180);
}

/*******************************************************************************
* 函数名称: CheckTmn_Halt
* 函数功能: 终端的停上电检测
* 输入参数: void
* 输出参数: < 0  //停电
            = 0  //新的上电记录
            > 0  //上电
* 内部处理:
* 返 回 值: 无
*******************************************************************************/
#define         COUNT_NUM         5        // 持续判断的次数
int CheckTmn_Halt_zhj(uint8 btype)
{
    int   bTemp = 0;                                    // 上电; 0:未上电
    OOP_DATETIME_S_T TmnTime = { 0 };                   //终端时间
    static OOP_DATETIME_S_T lastPowerOffTime[2];   //终端时间
    static int8 bfgPower = 1;                           //-电池状态标志-
    uint16 SetVol = 132;                                //停电电压限值
    uint16 ResetVol = 176;                              //上电电压限值
    //OOP_METER_PWRTYPE_E powerType = PWRTYPE_UNKNOWN;
    int fixVolt = 0;                                    // 额定电压
    static uint8 PowerOffCount[2] = { 0 };
    static uint8 PowerOnCount[2] ={ 0 };
    static uint8 WaitPowerOn[2] = { 0 };      /* 配电台体测试需要，台体上电后等待120S，然后60秒内等待上电事件 */
    static uint8 FirstRun[2] = {0x55};
    uint8 ishappen = 0;

    GetCurrentOopDateTimeS(&TmnTime);                   //-取实时时间-

    // 接线方式及额定电压，
    //powerType = PWRTYPE_3PHASE4;
    fixVolt = 2200;

    //取停电电压限值
//    SetVol = event_3106.eventParam.lParam.occVolt;
//    if (SetVol == 0xFFFF || SetVol == 0)
    {
        SetVol = fixVolt * 60 / 100;
    }

    //取上电电压限值
//    ResetVol = event_3106.eventParam.lParam.recVolt;
//    if (ResetVol == 0xFFFF || ResetVol == 0)
    {
        ResetVol = fixVolt * 80 / 100;
    }

    //读取停电告警发生标志
    if(read_pdata_xram("^Event", (char *)&ishappen, TmnPowerOffAddr+btype, 1) == 0 && ishappen == 0x55)
    {
        ishappen = 0x55;        //停电已经发生
        bfgPower = 0;
    }
    else
    {
        ishappen = 0xaa;        //停电未发生
        bfgPower = 1;
        write_pdata_xram("^Event", (const char *)&ishappen, TmnPowerOffAddr+btype, 1);
    }

    EVENT_FMT_TRACE("CheckERC_3106 检测终端停电时间 bfgPower = %d Volt[0] = %d Volt[1] = %d Volt[2] = %d SetVol = %d ResetVol = %d\n",
                    bfgPower, g_LatestACData.vol.nValue[0], g_LatestACData.vol.nValue[1], g_LatestACData.vol.nValue[2], SetVol, ResetVol);

    // 防错// 不会出现电池供电的情况下还有大于工作门限的电压
    if ((bfgPower < 0) && (g_LatestACData.vol.nValue[0] >= 900 || g_LatestACData.vol.nValue[1] >= 900 || g_LatestACData.vol.nValue[2] >= 900))
    {
        return 1;
    }

    if (btype == 0)
    {
        if (g_LatestACData.vol.nValue[0] == 0 && g_LatestACData.vol.nValue[1] == 0 && g_LatestACData.vol.nValue[2] == 0)
        {
            //多次，防止电压抖动
            if (PowerOffCount[0] < COUNT_NUM)
            {
                PowerOffCount[0]++;
            }
            PowerOnCount[0] = 0;
        }
        //不用电池供电，且当前电压大于上电限值，算上电
        else 
        {
            //多次，防止电压抖动
            if (PowerOnCount[0] < COUNT_NUM)
            {
                PowerOnCount[0]++;
            }

            if (WaitPowerOn[0] < 90)    /* 暂定等待90S */
            {
                WaitPowerOn[0]++;
            }
                
            PowerOffCount[0] = 0;
        }
    }
    else if (btype == 1 )

    {
        if (g_LatestACData.vol.nValue[0] == 0 && g_LatestACData.vol.nValue[1] == 0 && g_LatestACData.vol.nValue[2] == 0
                && g_LatestACData.cur.nValue[0] == 0 && g_LatestACData.cur.nValue[1] == 0 && g_LatestACData.cur.nValue[2] == 0)
        {
            //多次，防止电压抖动
            if (PowerOffCount[1] < COUNT_NUM)
            {
                PowerOffCount[1]++;
            }
            PowerOnCount[1] = 0;
        }
        //不用电池供电，且当前电压大于上电限值，算上电
        else if (g_LatestACData.vol.nValue[0] != 0 || g_LatestACData.vol.nValue[1] != 0 || g_LatestACData.vol.nValue[2] != 0
                    || g_LatestACData.cur.nValue[0] != 0 || g_LatestACData.cur.nValue[1] != 0 || g_LatestACData.cur.nValue[2] != 0 )

        {
            //多次，防止电压抖动
            if (PowerOnCount[1] < COUNT_NUM)
            {
                PowerOnCount[1]++;
            }

            if (WaitPowerOn[1] < 90)    /* 暂定等待90S */
            {
                WaitPowerOn[1]++;
            }
            
            PowerOffCount[1] = 0;
        }
    }
            

    //检测终端停电时间是否超过60S
    bTemp = PowerDownCheck_shj(btype);

    EVENT_FMT_TRACE("CheckERC_3106 检测终端停电时间bTemp=[%d]  PowerOffCount = %d PowerOnCount = %d flag = %02x\n",
       bTemp, PowerOffCount[btype], PowerOnCount[btype], ishappen);

    //当前停电且未记录过停电
    if (PowerOffCount[btype] >= COUNT_NUM && ishappen == 0xaa)
    {
        if (btype == 0)
        {
            SaveFileRecBfg(ERC_NO_14 | 0x80, 0x80, 0);//停电
        }
        else
        {
            SaveFileRecBfg(ERC_NO_14, 0x80, 0);//配电变压器停电
        }
        
        PowerOnCount[btype] = 0;
        //电池供电
        bfgPower = 0;
        //停电状态保存
        ishappen = 0x55;
        write_pdata_xram("^Event", (const char *)&ishappen, TmnPowerOffAddr+btype, 1);
        EVENT_FMT_TRACE("CheckERC_3106 检测终端停电时间超过%dS产生停上电事件\n", POWER_STOP_TIME_LMT);
        write_pdata_xram("^Event", (char *)&TmnTime, ProgDownAddr+btype*sizeof(OOP_DATETIME_S_T), sizeof(OOP_DATETIME_S_T));
        return -1;
    }
    //当前上电且未记录过上电，或第一次上电且停电时间超过限值
    else if ((WaitPowerOn[btype] >= 90) && ((PowerOnCount[btype] >= COUNT_NUM && ishappen == 0x55)
        || (bTemp == 1 && FirstRun[btype] == 0x55)))
    {
        if (btype == 0)
        {
            SaveFileRecBfg(ERC_NO_14 | 0x80, 0, 0);//上电
        }
        else
        {
            SaveFileRecBfg(ERC_NO_14, 0, 0);//配电变压器上电
        }
        PowerOffCount[btype] = 0;
        WaitPowerOn[btype] = 0;
        //正常供电
        bfgPower = 1;
        write_pdata_xram("^Event", (char *)&TmnTime,ProgDownAddr+btype*sizeof(OOP_DATETIME_S_T), sizeof(OOP_DATETIME_S_T));
        ishappen = 0xaa;
        write_pdata_xram("^Event", (const char *)&ishappen, TmnPowerOffAddr+btype, 1);

        return 0;
    }
    else if (bfgPower > 0)
    {
        //停电事件只要精确到秒，所以私有外村只记录到分钟即可，否则每秒都要写入，很浪费Nand写入次数
        if (lastPowerOffTime[btype].minute != TmnTime.minute || lastPowerOffTime[btype].hour != TmnTime.hour || lastPowerOffTime[btype].day != TmnTime.day ||
            lastPowerOffTime[btype].month != TmnTime.month || lastPowerOffTime[btype].year != TmnTime.year)
        {
            lastPowerOffTime[btype] = TmnTime;
            EVENT_FMT_DEBUG("终端停电时间%u-%u-%u %u:%u:%u \n", TmnTime.year, TmnTime.month, TmnTime.day, TmnTime.hour, TmnTime.minute, TmnTime.second);
            write_pdata_xram("^Event", (char *)&TmnTime, ProgDownAddr+btype*sizeof(OOP_DATETIME_S_T), sizeof(OOP_DATETIME_S_T));
        }
    }
    //终端在上电状态的情况下实时刷新终端时间
    //-从数据字典读取终端停电标志,这个标志是在selfcheck检测中维护
    //-从数据字典读取终端停电标志,这个标志是在selfcheck检测中维护的-

    FirstRun[btype] = 0xAA;
    return 1;
}


/*******************************************************************************
* 函数名称: CheckERC_3106
* 函数功能: 停上电检测
* 输入参数: void
* 输出参数: NULL
* 内部处理:
根据新的用电信息采集系统停电事件采集要求，终端停/上电事件进行检测
判断逻辑步骤:
STEP-1:
终端停上电时间每秒检测:
    终端电压大于掉电限值，每个一段时间间隔(暂定为20S)刷新下当前时间。
    如果产生新的终端停电记录,那么终端实时产生一条停电记录，终端实时存储，进行上报.
函数返回。
    如果产生新的终端上电记录,那么终端实时产生一条上电记录，终端记录但是为，进入下一步处理.
    如果没有产生终端停上电记录,那么进入下一步处理。
STEP-2:
    终端根据停电数据采集标志决定是否需要穿透抄读掉电记录。
    依据下发的停电事件抄读时间间隔,如果是刚上电或者到抄读间隔进入穿透抄读掉电记录.
    根据穿透返回的电表掉电记录，依据判断电表的掉电记录。
   -

* 返 回 值: 无
*******************************************************************************/
void CheckERC_3106_zhj(int clientid)
{
    int TmnHaltRet = -1; //终端停上电返回
/*
    if (event_3106.eventCommInfo.checkflag != 1) {
        EVENT_FMT_DEBUG("未使能3106停电检测\n");
        return;
    }
*/
    //读取三相电压实时值

    if (g_LatestACData.vol.nNum < 1) {
        EVENT_FMT_DEBUG("电压无效(3106)\n");
        return;
    }

    uint32 ArrIdx = 0;//一维数组索引(起始)
    ArrIdx = g_aFaultDataIndexZhj[0][0][0];

    //每秒中进行判断终端是否产生新的停上电事件 -1:停电事件, 0:上电事件, 1:一直供电
    TmnHaltRet = CheckTmn_Halt_zhj(0);

	if(TmnHaltRet == -1)
	{
	    yx_event_signal_update_zhj(0, 0, 0, g_aFaultDataConfigZhj[ArrIdx+0].nFaultYxAddr,1);   //置停电事件标志
	}
	else if(TmnHaltRet == 0)
	{
		yx_event_signal_update_zhj(0, 0, 0, g_aFaultDataConfigZhj[ArrIdx+0].nFaultYxAddr,0);   //置上电事件标志
	}
	
}

/**********************************************************************
* @name      : CheckSelfERC
* @brief     ：配电变压器停上电检测
* @param[in] ：int clientid    数据中心句柄,预留
* @param[out]：
* @return    ：
* @Create    : 梅安宁
* @Date      ：2020-11-19
* @Update    :
**********************************************************************/
void CheckSelfERC(int clientid)
{
    int TmnHaltRet = -1; //终端停上电返回

    //读取三相电压实时值
    if (g_LatestACData.vol.nNum < 1) {
        EVENT_FMT_DEBUG("电压无效(CheckSelfERC)\n");
        return;
    }

    if (g_LatestACData.cur.nNum < 1) {
        EVENT_FMT_DEBUG("电流无效(CheckSelfERC)\n");
        return;
    }

    uint32 ArrIdx = 0;//一维数组索引(起始)
    ArrIdx = g_aFaultDataIndexZhj[1][0][0];

    //每秒中进行判断终端是否产生新的停上电事件 -1:停电事件, 0:上电事件, 1:一直供电
    TmnHaltRet = CheckTmn_Halt_zhj(1);

	if(TmnHaltRet == -1)
	{
	    yx_event_signal_update_zhj(1, 0, 0, g_aFaultDataConfigZhj[ArrIdx+0].nFaultYxAddr,1);   //置停电事件标志
	}
	else if(TmnHaltRet == 0)
	{
		yx_event_signal_update_zhj(1, 0, 0, g_aFaultDataConfigZhj[ArrIdx+0].nFaultYxAddr,0);   //置上电事件标志
	}

}

#endif

#if DESC("电压断相缺相事件",1)

/**********************************************************************
* @name      : CheckERC_3000
* @brief     电压断相事件
* @param[in] ：int clientid    数据中心句柄,预留
* @param[out]：
* @return    ：
* @Create    : 梅安宁
* @Date      ：2020-11-19
* @Update    :
**********************************************************************/
void  CheckERC_3000(int clientid)
{
    

    if (g_LatestACData.vol.nNum < 1 )
    {
        EVENT_FMT_DEBUG("电压无效返回(3000)\n");
        return;
    }

    EVENT_FMT_TRACE("电压断相恢复检测开始\n");
    VltBreakEvent(clientid, 4);
    EVENT_FMT_TRACE("电压断相检测开始\n");
    VltBreakEvent(clientid, 3);
}

/**********************************************************************
* @name      : CheckERC_3003
* @brief     电压缺相事件
* @param[in] ：int clientid    数据中心句柄,预留
* @param[out]：
* @return    ：
* @Create    : 梅安宁
* @Date      ：2020-11-19
* @Update    :
**********************************************************************/
void CheckERC_3003(int clientid)
{
    /*- 数据内容                                        数据格式    字节数
    ERC=12                                          BIN         1
    长度Le                                          BIN         1
    发生时间：分时日月年                            数据格式15  5
    D7：起/止标志   D6=0  D5~D0：mtr_id（测量点号） BIN         1-*/
//    if (event_3003.eventCommInfo.checkflag != 1) {
//        EVENT_FMT_DEBUG("未使能3003缺相检测\n");
//        return;
//    }

    if (g_LatestACData.vol.nNum < 1 && g_LatestACData.cur.nNum  < 1)
    {
        EVENT_FMT_DEBUG("电压电流无效返回(3003)\n");
        return;
    }

    EVENT_FMT_TRACE("电压缺相恢复检测开始\n");
    VltBreakEvent(clientid, 2);
    EVENT_FMT_TRACE("电压缺相检测开始\n");
    VltBreakEvent(clientid, 1);
}
#endif

#if DESC("电流越限事件",1)
/**********************************************************************
* @name      : CheckERC_3005
* @brief     ：配电电流越限事件
* @param[in] ：int clientid    数据中心句柄,预留
* @param[out]：
* @return    ：
* @Create    : 梅安宁
* @Date      ：2020-11-19
* @Update    :
**********************************************************************/
void CheckERC_3005(int clientid)
{
/*    if (event_3005.eventCommInfo.checkflag != 1) {
        EVENT_FMT_DEBUG("未使能3005电流越限检测\n");
        return;
    }
*/
    if (g_LatestACData.cur.nNum < 1)
    {
        EVENT_FMT_DEBUG("CheckERC_3005 无有效电流值(3005)\n");
        return;
    }


    EVENT_FMT_TRACE("CheckERC_3005电流越上限恢复检测开始\n");
    CurHighEvent(clientid, 2);
    EVENT_FMT_TRACE("CheckERC_3005电流越上限检测开始\n");
    CurHighEvent(clientid, 1);

}
#endif

#if DESC("电压电流逆相序事件",1)
/**********************************************************************
* @name      : CheckERC_300F
* @brief     ：配电电压电压逆相序事件
* @param[in] ：int clientid    数据中心句柄,预留
* @param[out]：
* @return    ：
* @Create    : 梅安宁
* @Date      ：2020-11-19
* @Update    :
**********************************************************************/
void CheckERC_300F(int clientid)
{
    /*- 数据内容                                        数据格式    字节数
    ERC=10                                          BIN         1
    长度Le                                          BIN         1
    发生时间：分时日月年                            数据格式15  5
    D7：起/止标志   D6=0    D5~D0：pn（测量点号）   BIN         1
    异常标志                                        BS8         1
    发生时的Ua/Uab                                  数据格式07  2
    发生时的Ub                                      数据格式07  2
    发生时的Uc/Ucb                                  数据格式07  2
    发生时的Ia                                      数据格式06  2
    发生时的Ib                                      数据格式06  2
    发生时的Ic                                      数据格式06  2
    发生时电能表正向有功总电能示值                  数据格式14  5-*/
/*
    if (event_300F.eventCommInfo.checkflag != 1) {
        EVENT_FMT_DEBUG("未使能300f相序异常检测\n");
        return;
    }
*/
    if (g_LatestACData.vol.nNum < 3) {
        EVENT_FMT_DEBUG("CheckERC_300F,无有效的电压相角\n");
        return;
    }

    EVENT_FMT_TRACE("CheckERC_300F,电压逆相检测开始\n");
    PhaseReverseEvent(clientid, 0);
}

/**********************************************************************
* @name      : CheckERC_3010
* @brief     ：配电电流逆相序事件
* @param[in] ：int clientid    数据中心句柄,预留
* @param[out]：
* @return    ：
* @Create    : 梅安宁
* @Date      ：2020-11-19
* @Update    :
**********************************************************************/
void CheckERC_3010(int clientid)
{
    /*- 数据内容                                        数据格式    字节数
    ERC=10                                          BIN         1
    长度Le                                          BIN         1
    发生时间：分时日月年                            数据格式15  5
    D7：起/止标志   D6=0    D5~D0：pn（测量点号）   BIN         1
    异常标志                                        BS8         1
    发生时的Ua/Uab                                  数据格式07  2
    发生时的Ub                                      数据格式07  2
    发生时的Uc/Ucb                                  数据格式07  2
    发生时的Ia                                      数据格式06  2
    发生时的Ib                                      数据格式06  2
    发生时的Ic                                      数据格式06  2
    发生时电能表正向有功总电能示值                  数据格式14  5-*/

    if (g_LatestACData.cur.nNum < 3) {
        EVENT_FMT_DEBUG("CheckERC_3010,无有效的电流\n");
        return;
    }

    EVENT_FMT_TRACE("CheckERC_3010,电流逆相检测开始\n");
    PhaseReverseEvent(clientid, 1);
}

#endif

#if DESC("电压电流不平衡事件",1)

/**********************************************************************
* @name      : CheckERC_301D
* @brief     ：电压不平衡事件
* @param[in] ：int clientid    数据中心句柄,预留
* @param[out]：
* @return    ：
* @Create    : 梅安宁
* @Date      ：2020-11-19
* @Update    :
**********************************************************************/
void CheckERC_301D(int clientid)
{
//    if (event_301D.eventCommInfo.checkflag != 1)
    {
//        EVENT_FMT_DEBUG("未使能301D电压不平衡检测\n");
//        return;
    }

    EVENT_FMT_TRACE("电压不平衡检测开始\n");
    if (g_LatestACData.vol.nNum < 1 )
    {
        EVENT_FMT_DEBUG("电压无效返回(CheckERC_301D)\n");
        return;
    }

    VltCurImbalanceEvent(clientid, 1);
}

/**********************************************************************
* @name      : CheckERC_301E
* @brief     ：电流不平衡事件
* @param[in] ：int clientid    数据中心句柄,预留
* @param[out]：
* @return    ：
* @Create    : 梅安宁
* @Date      ：2020-11-19
* @Update    :
**********************************************************************/
void CheckERC_301E(int clientid)
{
//    if (event_301E.eventCommInfo.checkflag != 1)
    {
//        EVENT_FMT_DEBUG("未使能301E电流不平衡检测\n");
//        return;
    }

    EVENT_FMT_TRACE("电流不平衡检测开始\n");
    if (g_LatestACData.cur.nNum < 1 )
    {
        EVENT_FMT_DEBUG("电流无效返回(CheckERC_301E)\n");
        return;
    }

    VltCurImbalanceEvent(clientid, 2);
}

#endif

#if DESC("电压电流畸变率越限事件",1)

/**********************************************************************
* @name      : CheckERC_3031
* @brief     ：电流畸变率越限事件
* @param[in] ：int clientid    数据中心句柄,预留
* @param[out]：
* @return    ：
* @Create    : 梅安宁
* @Date      ：2020-11-19
* @Update    :
**********************************************************************/
void CheckERC_3031(int clientid)
{
    EVENT_FMT_TRACE("电压畸变率越限检测开始\n");
    if (g_LatestACData.harmonicV[0].nNum < 1 && g_LatestACData.harmonicV[1].nNum < 1 &&g_LatestACData.harmonicV[2].nNum < 1  )
    {
        EVENT_FMT_DEBUG("电压谐波含有率无效返回(CheckERC_3031)\n");
        return;
    }

    VltCurHarmoniceEvent(clientid, 1);
}

/**********************************************************************
* @name      : CheckERC_3032
* @brief     ：电压畸变率越限事件
* @param[in] ：int clientid    数据中心句柄,预留
* @param[out]：
* @return    ：
* @Create    : 梅安宁
* @Date      ：2020-11-19
* @Update    :
**********************************************************************/
void CheckERC_3032(int clientid)
{
    EVENT_FMT_TRACE("电流畸变率越限检测开始\n");
    if (g_LatestACData.harmonicC[0].nNum < 1 && g_LatestACData.harmonicC[1].nNum < 1 &&g_LatestACData.harmonicC[2].nNum < 1  )
    {
        EVENT_FMT_DEBUG("电流谐波含有率无效返回(CheckERC_3031)\n");
        return;
    }

    VltCurHarmoniceEvent(clientid, 2);
}

#endif

#if DESC("电流反极性事件",1)

/**********************************************************************
* @name      : CheckERC_3119
* @brief     电流反极性事件
* @param[in] ：int clientid    数据中心句柄,预留
* @param[out]：
* @return    ：
* @Create    : 梅安宁
* @Date      ：2020-11-19
* @Update    :
**********************************************************************/
void CheckERC_3119(int clientid)
{
    /*- 数据内容                                    数据格式    字节数
    ERC=10                                          BIN         1
    长度Le                                          BIN         1
    发生时间：分时日月年                            数据格式15  5
    D7：起/止标志   D6=0    D5~D0：pn（测量点号）   BIN         1
    异常标志                                        BS8         1
    发生时的Ua/Uab                                  数据格式07  2
    发生时的Ub                                      数据格式07  2
    发生时的Uc/Ucb                                  数据格式07  2
    发生时的Ia                                      数据格式06  2
    发生时的Ib                                      数据格式06  2
    发生时的Ic                                      数据格式06  2
    发生时电能表正向有功总电能示值                  数据格式14  5-*/
//    if (event_3119.eventCommInfo.checkflag != 1) {
//        EVENT_FMT_DEBUG("未使能3119电流反极性检测\n");
//        return;
//    }

    if (g_LatestACData.oopWord3V.nNum < 3)
    {
        EVENT_FMT_DEBUG("电压相角无效\n");
        return;
    }

    if (g_LatestACData.oopWord3C.nNum < 3)
    {
        EVENT_FMT_DEBUG("电流相角无效\n");
        return;
    }

    CurReverseEvent(clientid,2);  //-反极性恢复检测-
    EVENT_FMT_TRACE("CheckERC_3119反极性恢复检测 complete\n");

    CurReverseEvent(clientid,1);  //-反极性发生检测-
    EVENT_FMT_TRACE("CheckERC_3119 反极性发生检测 complete\n\n\n");
}

#endif

#if DESC("零序电流偏大事件",1)

/**********************************************************************
* @name      : CheckERC_3119
* @brief     ：零序电流偏大事件
* @param[in] ：int clientid    数据中心句柄,预留
* @param[out]：
* @return    ：
* @Create    : 梅安宁
* @Date      ：2020-11-19
* @Update    :
**********************************************************************/
void CheckERC_3033(int clientid)
{
    uint8   lastTimesLmt1 = 1;                       //默认的电流越上限持续时间
    uint8   lastTimesLmt2 = 1;                       //默认的电流越上限恢复持续时间
    float32  cur_lmt= 0;                              //零序电流越上限值
    float32  cur_lmt_rev = 0;                         //零序电流越上上限恢复值
    static uint8 lastTimes[2];                       //持续时间 0 发生 1 恢复
    float32 tmpFloat;
    int64   tmpint;
    float32 Rated_current = 5000;
    int8   tmp_flag1 = 0, tmp_flag2 = 0;
    static int    dwERFlgTmp = 0;
    
    if (g_LatestACData.z_cur ==  APP_NONELWORD)
    {
        EVENT_FMT_DEBUG("零序电流无效\n");
        return;
    }

    //额定电流
    if (para_query_float32_zhj(1, 0, 0, 0x00008009, &tmpFloat) > 0 && tmpFloat > 0)
    {
        Rated_current = tmpFloat * 1000;
    }

    cur_lmt = Rated_current*0.5;
    cur_lmt_rev = Rated_current*0.5;

    if (para_query_float32_zhj(1, 0, 0, 0x0000802e, &tmpFloat) > 0 && tmpFloat > 0)
    {
        cur_lmt = tmpFloat*Rated_current;
    }

    if (para_query_float32_zhj(1, 0, 0, 0x0000802f, &tmpFloat) > 0 && tmpFloat > 0)
    {
        cur_lmt_rev = tmpFloat*Rated_current;
    }


    lastTimesLmt1 = 15;           //容错
    lastTimesLmt2 = 15;          //容错
    if (para_query_int64_zhj(1, 0, 0, 0x00008040, &tmpint) > 0 && tmpint > 0)
    {
        lastTimesLmt1 = tmpint * 60;
    }

    if (para_query_int64_zhj(1, 0, 0, 0x00008041, &tmpint) > 0 && tmpint > 0)
    {
        lastTimesLmt2 = tmpint * 60;
    }

    EVENT_FMT_TRACE("CheckERC_3033 z_cur[%d] 电流上限值=%d,持续时间=%d, 电流上限恢复值=%d    持续时间=%d\n", g_LatestACData.z_cur, cur_lmt,lastTimesLmt1,cur_lmt_rev,lastTimesLmt2);

    //-零序电流偏大判断-
    tmp_flag1 = ((float32)g_LatestACData.z_cur) > cur_lmt;
    tmp_flag2 = ((float32)g_LatestACData.z_cur) > cur_lmt_rev;
    EVENT_FMT_DEBUG("g_LatestACData.z_cur = %d, cur_lmt = %f, cur_lmt_rev = %f, dwERFlgTmp = %d\n", g_LatestACData.z_cur, cur_lmt, cur_lmt_rev, dwERFlgTmp);

    uint32 ArrIdx = 0;//一维数组索引(起始)
    ArrIdx = g_aFaultDataIndexZhj[1][0][0];

    if(tmp_flag1 && !dwERFlgTmp)            //发生了不平衡，并且以前没有记录过该事件
    {
        EVENT_FMT_DEBUG("lastTimes[0] = %d\n", lastTimes[0]);
        //如果满足相应的发生条件，将对应的时间进行累加
        if(lastTimes[0] >= lastTimesLmt1-1)
        {
            yx_event_signal_update_zhj(1, 0, 0, g_aFaultDataConfigZhj[ArrIdx+22].nFaultYxAddr,1);
            SaveFileRecBfg(ERC_NO_27,0x80,0);
            EVENT_FMT_DEBUG("零序电流越限发生\n");
            dwERFlgTmp = 1;
        }
        else
        {
            lastTimes[0]++;
        }
        lastTimes[1] = 0;
    }
    else if(!tmp_flag2 && dwERFlgTmp)    //没有发生不平衡，并且以前记录过该事件
    {
        //如果满足相应的发生条件，将对应的时间进行累加
        if(lastTimes[1] >= lastTimesLmt2-1)
        {
			yx_event_signal_update_zhj(1, 0, 0, g_aFaultDataConfigZhj[ArrIdx+22].nFaultYxAddr,0);
            SaveFileRecBfg(ERC_NO_27,0x00,0);
            EVENT_FMT_DEBUG("零序电流越限恢复\n");
            dwERFlgTmp = 0;
        }
        else
        {
            lastTimes[1]++;
        }
        lastTimes[0] = 0;
    }
    else
    {
        lastTimes[0] = 0;
        lastTimes[1] = 0;
    }

    return;
}

#endif

#if DESC("视在功率越限",1)
/**********************************************************************
* @name      : Check_ApparentPower
* @brief     ：视在功率越限
* @param[in] ：int clientid    数据中心句柄,预留
* @param[out]：
* @return    ：
* @Create    : 郭磊
* @Date      ：2021-6-21
* @Update    :
**********************************************************************/
void Check_ApparentPower(int clientid)
{
    float32 rated_current = 5000;
    float32 rated_volt = 2200;
    float32 tmpFloat;
    float32 cur_lmt;
    float32 volt_lmt;
    static int happenFlag = 0;
    static int rcvFlag = 0;
    static uint8 lastHappenTimes = 0;
    static uint8 lastRecvTimes = 0;

    //额定电流
    if (para_query_float32_zhj(1, 0, 0, 0x00008009, &tmpFloat) > 0 && tmpFloat > 0)
    {
        rated_current = tmpFloat * 1000;
    }

    //额定电压
    if (para_query_float32_zhj(1, 0, 0, 0x00008008, &tmpFloat) > 0 && tmpFloat > 0)
    {
        rated_volt = tmpFloat * 10;
    }

    cur_lmt = rated_current * 1.15;
    volt_lmt = rated_volt * 1.15;

    EVENT_FMT_DEBUG("cur_lmt = %f, volt_lmt = %f\n", cur_lmt, volt_lmt);
    EVENT_FMT_DEBUG("g_LatestACData.cur.nValue[0] = %d, g_LatestACData.cur.nValue[1] = %d, g_LatestACData.cur.nValue[2] = %d\n", 
        g_LatestACData.cur.nValue[0], g_LatestACData.cur.nValue[1], g_LatestACData.cur.nValue[2]);
    EVENT_FMT_DEBUG("g_LatestACData.vol.nValue[0] = %d, g_LatestACData.vol.nValue[1] = %d, g_LatestACData.vol.nValue[2] = %d\n", 
        g_LatestACData.vol.nValue[0], g_LatestACData.vol.nValue[1], g_LatestACData.vol.nValue[2]);

    if ((g_LatestACData.cur.nValue[0] > cur_lmt && g_LatestACData.vol.nValue[0] > volt_lmt) ||
       (g_LatestACData.cur.nValue[1] > cur_lmt && g_LatestACData.vol.nValue[1] > volt_lmt) ||
       (g_LatestACData.cur.nValue[2] > cur_lmt && g_LatestACData.vol.nValue[2] > volt_lmt))  /* 发生 */
    {
        EVENT_FMT_DEBUG("lastHappenTimes = %d\n", lastHappenTimes);
        if (!happenFlag && lastHappenTimes > 59)
        {
            yx_event_signal_update_zhj(1, 0, 0, 0x100018 ,1);
            EVENT_FMT_DEBUG("视在功率越限发生\n");
            happenFlag = 1;
            rcvFlag = 0;
        }
        else if (lastHappenTimes <= 59)
        {
            lastHappenTimes++;
        }
        lastRecvTimes = 0;
    }
    else /* 恢复 */
    {
        if (happenFlag)
        {
            EVENT_FMT_DEBUG("lastRecvTimes = %d\n", lastRecvTimes);
            if (!rcvFlag && lastRecvTimes > 59)
            {
                yx_event_signal_update_zhj(1, 0, 0, 0x100018, 0);
                EVENT_FMT_DEBUG("视在功率越限恢复\n");
                rcvFlag = 1;
                happenFlag = 0;
            }
            else if (lastRecvTimes <= 59)
                lastRecvTimes++;
        }
        lastHappenTimes = 0;
        
    }
}
#endif

#if DESC("无功补偿事件",1)

/**********************************************************************
* @name      : CheckERC_3034
* @brief     ：无功过补偿事件
* @param[in] ：int clientid    数据中心句柄,预留
* @param[out]：
* @return    ：
* @Create    : 梅安宁
* @Date      ：2020-11-19
* @Update    :
**********************************************************************/
void CheckERC_3034(int clientid)
{
    EVENT_FMT_TRACE("无功过补偿事件检测开始\n");
    if (g_LatestACData.fpower.nNum < 1 )
    {
        EVENT_FMT_DEBUG("功率因数无效返回(CheckERC_3034)\n");
        return;
    }

    repower_comp_event(clientid, 1);
}

/**********************************************************************
* @name      : CheckERC_3035
* @brief     ：无功欠补偿事件
* @param[in] ：int clientid    数据中心句柄,预留
* @param[out]：
* @return    ：
* @Create    : 梅安宁
* @Date      ：2020-11-19
* @Update    :
**********************************************************************/
void CheckERC_3035(int clientid)
{
    EVENT_FMT_TRACE("无功欠补偿事件检测开始\n");
    if (g_LatestACData.fpower.nNum < 1 )
    {
        EVENT_FMT_DEBUG("功率因数无效返回(CheckERC_3035)\n");
        return;
    }

    repower_comp_event(clientid, 2);
}

#endif


#if DESC("负荷过载事件",1)

/**********************************************************************
* @name      : CheckERC_3036
* @brief     ：负荷过载事件
* @param[in] ：int clientid    数据中心句柄,预留
* @param[out]：
* @return    ：
* @Create    : 梅安宁
* @Date      ：2020-11-19
* @Update    :
**********************************************************************/
void CheckERC_3036(int clientid)
{
    EVENT_FMT_TRACE("负荷过载检测开始\n");
    if (g_LatestACData.fpower.nNum < 1 )
    {
        EVENT_FMT_DEBUG("功率因数无效返回(CheckERC_3036)\n");
        return;
    }

    over_load_event(clientid);
}

#endif


/*********************************************************************
所属功能单元：
功能描述：清应用层外存数据
输入参数：无
输出参数：无
函数返回值：无
内部处理流程：
********************************************************************/
void ClearAppXData(void)
{
    uint8 bData[sizeof(RECODE_SAVE_T) * 256];

    memset(bData, 0, sizeof(RECODE_SAVE_T) * 256);
    write_pdata_xram("^Event", (const char *)bData, PowerDownAddr, PowerDownRecLen);
    write_pdata_xram("^Event", (const char *)bData, ProgDownAddr, ProgDownRecLen);
    write_pdata_xram("^Event", (const char *)bData, EventPhaseRecAddr, EventPhaseRecLen);
    write_pdata_xram("^Event", (const char *)bData, LostvltAddr, LostvltLen);
    write_pdata_xram("^Event", (const char *)bData, BreakphaseAddr, BreakphaseLen);
    write_pdata_xram("^Event", (const char *)bData, CurReverseRecAddr, CurReverseRecLen);
    write_pdata_xram("^Event", (const char *)bData, PhaseAbnormalAddr, PhaseAbnormalLen);
    write_pdata_xram("^Event", (const char *)bData, PhaseRvsLastTimesAddr, PhaseRvsLastTimesLen);
#if 1

#else
    write_pdata_xram("^Event", (char *)bData, MetBMagneticAbNormalAddr, MetBMagneticAbNormalLen);
#endif
    write_pdata_xram("^Event", (char *)bData, TimeSettingAddr, TimeSettingLen);


}

/*********************************************************************
所属功能单元：
功能描述：清应用层外存数据
输入参数：无
输出参数：无
函数返回值：无
内部处理流程：
********************************************************************/
void ClearAppYData(void)
{
    uint8 bData[(VIR_BEGIN) * 4 * sizeof(uint64)];

    memset(bData, 0, (VIR_BEGIN) * 4 * sizeof(uint64));
    write_pdata_xram("^Event", (const char *)bData, EventStatusRecAddr, EventStatusRecLen);
}
//<Start Common Problem>
/*********************************************************************
所属功能单元：
功能描述：清应用层外存数据
输入参数：无
输出参数：无
函数返回值：无
内部处理流程：
********************************************************************/
void ClearAppZData(void)
{
    uint8 bData[256];
    memset(bData, 0, sizeof(bData));
    write_pdata_xram("^Event", (const char *)bData, ProgDownAddr, ProgDownRecLen);
}

/*********************************************************************
所属功能单元：用电监测提供给规约解释
功能描述：当重新设置表之后，清空表外存历史数据给
                事件检测。
输入参数：无
输出参数：无
函数返回值：无
内部处理流程：清空设置过后测量点的外存历史数据
********************************************************************/
void ClearMtrHisD(mtrnum_t Mtrid)
{
    uint8 bData[4];
    uint8 bData1[32];

    memset(bData, 0, 4);
    memset(bData1, APP_NONECHAR, 32);

    if (Mtrid > MTR485_MAX_NUM)    //只记录前64个测量点的事件，清外存时也只需清前64个测量点的数据.
    {
        return;
    }

    write_pdata_xram("^Event", (char *)bData, (CurReverseRecAddr + (Mtrid * sizeof(uint8))), 1);
    write_pdata_xram("^Event", (char *)bData, (EventStatusRecAddr + (Mtrid * sizeof(uint32))), sizeof(uint32));
    write_pdata_xram("^Event", (char *)bData, (LostvltAddr + (Mtrid * sizeof(uint16))), sizeof(uint16));
    write_pdata_xram("^Event", (char *)bData, (BreakphaseAddr + (Mtrid * sizeof(uint32))), sizeof(uint32));
    write_pdata_xram("^Event", (char *)bData, (PhaseAbnormalAddr + (Mtrid * 4)), 4);
    write_pdata_xram("^Event", (char *)bData, (PhaseRvsLastTimesAddr + (Mtrid * 2)), 2);
}

/*
********************************************************************
所属功能单元：GB101规约处理模块
功能描述    ：遥测数据转换，将数据字典的数据转换为104规约的数据格式
输入参数    ：Objaddr-数据信息体地址

输出参数    ：*pdata-返回数据缓冲区

函数返回值    ：无
作者        ：丁俊杰
创建日期    ：2014年4月3日
修改者    ：
修改说明    ：
修改日期    ：
*******************************************************************/
//-这个函数要根据数据字典类型、存储倍率和上送倍率进行分类 2016-06-
void TransYaoChe(uint16 Objaddr,float *pData)
{
    int i =0;

    for(i=0;i<MAX_YC_NUM;i++)
    {
        if(g_aYcDataConfig[i].nObjAddr == Objaddr)
        {
            if(g_aYcDataConfig[i].nSignalType == M_ME_NA_1)
            {
                //归一化数值
                float32 fRatio = ratio_convert(g_aYcDataConfig[i].nReportRatio, E_REAL_TO_DISPATCH);
                float32 fConvertValue = g_aYcDataSend[i].fNewValue*fRatio;
                float32 fNvaValue = fConvertValue/g_aYcDataConfig[i].fFullValue;  //实际值除以满码值
                int16   nValueNVA = 0;  //转换之后的归一化值

                //溢出检查
                if(fNvaValue > (1-pow(2, -15)))
                {
                    fNvaValue = 1-pow(2, -15);
                }
                else if(fNvaValue < (-1+pow(2, -15)))
                {
                    fNvaValue = -1+pow(2, -15);
                }

                if(fNvaValue >= 0)
                {
                    nValueNVA = (int16)(fNvaValue*pow(2, 15));
                }
                else
                {
                    nValueNVA = (int16)(fNvaValue*pow(2, 15)*(-1));
                    nValueNVA = (int16)(nValueNVA*(-1));
                }

                memcpy((void *)pData, (void *)&nValueNVA, sizeof(int16));

                break;
            }

            if(g_aYcDataConfig[i].nSignalType == M_ME_NB_1)
            {
                //标度化数值
                float32 fRatio = ratio_convert(g_aYcDataConfig[i].nReportRatio, E_REAL_TO_DISPATCH);
                float32 fConvertValue = g_aYcDataSend[i].fNewValue*fRatio;
                int16 nValueSVA;  //转换之后的标度化值

                //溢出检查
                if(fConvertValue > 32767)
                {
                    nValueSVA = 32767;
                }
                else if(fConvertValue < (-32768))
                {
                    nValueSVA = (-32768);
                }
                else
                {
                    nValueSVA = (int16)fConvertValue;
                }

                memcpy((void *)pData, (void *)&nValueSVA, sizeof(int16));

                break;
            }

            if(g_aYcDataConfig[i].nSignalType == M_ME_NC_1)
            {
                //浮点型数值
                float32 fRatio = ratio_convert(g_aYcDataConfig[i].nReportRatio, E_REAL_TO_DISPATCH);
                float32 fConvertValue = g_aYcDataSend[i].fNewValue*fRatio;

                memcpy((void *)pData, (void *)&fConvertValue, sizeof(float32));

                break;
            }
        }
    }

    return ;
}

/**********************************************************************
* @name      : event_happen_proc
* @brief     ：事件发生检查
* @param[in] ：
* @param[out]：
* @return    ：
* @Create    : 陈德昶
* @Date      ：2020-03-12
* @Update    :

**********************************************************************/
void event_init_zhj(int clientid)
{
    int ret = 0;
    
    if ((ret = reg_pdata_xram(ADV_APP_NAME, "^Event")) != 0)
    {
        EVENT_FMT_DEBUG("event_init reg error1 ret=%d\n", ret);
    }

    if ((ret = reg_pdata_xram(ADV_APP_NAME, "^EventS104")) != 0)
    {
        EVENT_FMT_DEBUG("event_init reg error2 ret=%d\n", ret);
    }

    ac_data_init(clientid);
    
//    InitEventPara(g_sDataCenterClient);
}

/**********************************************************************
* @name      : event_happen_proc_new
* @brief     ：事件发生检查
* @param[in] ：
* @param[out]：
* @return    ：
* @Create    : 李银银
* @Date      ：2020-06-09
* @Update    :

**********************************************************************/
void event_happen_proc_zhj(int clientid)
{
    static time_t   timep_sec;
    time_t          timep_cur;
    time(&timep_cur); //获取从1970至今过了多少秒，存入time_t类型的timep_cur

    if (timep_sec != timep_cur)
    {
        //过秒进行终端事件的检测
        timep_sec = timep_cur;

        EVENT_FMT_TRACE("进入终端事件生成模块\n");
        RtuExam(clientid);                                //Tmn running examing
        EVENT_FMT_TRACE("终端事件的检测完毕\n");

		EVENT_FMT_TRACE("进入交采事件生成模块\n");
        MetExam(clientid);                         //Tmn running examing
        EVENT_FMT_TRACE("交采事件的检测完毕\n");
    }
}


/**********************************************************************
* @name      : tnm_event_check_faults
* @brief     ：检查是否有终端事件记录,并且缓存事件记录
* @param[in] ：
* @param[out]：
* @return    ：0没有记录，1有记录
* @Create    : 梅安宁
* @Date      ：2020-11-26
* @Update    :

**********************************************************************/
int get_report_data(uint32 report_add, uint32 *preport_data)
{
    int i = 0;
    *preport_data = 0;
    for(i=0; i<g_report_data_num; i++)
    {
        if(g_report_data_list[i].report_addr == report_add)
        {
            *preport_data = *g_report_data_list[i].preport_data;
            break;
        }
    }

    return 0;
}


/**********************************************************************
* @name      : tnm_event_check_faults
* @brief     ：检查是否有终端事件记录,并且缓存事件记录
* @param[in] ：
* @param[out]：
* @return    ：0没有记录，1有记录
* @Create    : 梅安宁
* @Date      ：2020-11-26
* @Update    :

**********************************************************************/
int tnm_event_check_faults(void)
{
    uint8 i ,j ,bno,bsit,bfgchange= 0;
    uint32 id = 0;
    uint8  bdata[32] = {0};
    uint8  rtn = 0;
    uint16 dtemp = 0;
    struct timeval   c_time;
    OOP_DATETIME_S_T SystemTime;

    if (g_nConfigFaultNumZhj[0][0][0] < 1 || g_nConfigFaultNumZhj[0][0][0] > FAULT_MAX_NUM)
    {
        EVENT_FMT_DEBUG("event_check_faults, 配置数量%u错误\n", g_nConfigFaultNumZhj[0][0][0]);

        return 0;
    }

    uint32 ArrIdx = 0;//一维数组索引(起始)
    ArrIdx = g_aFaultDataIndexZhj[0][0][0];
    
    //只检查终端停上电事件
    if (g_aFaultDataConfigZhj[ArrIdx+0].nObjAddrU32 != FALT_START_ADDR )
    {
        EVENT_FMT_DEBUG("tnm_event_check_faults, 终端停上电事件配置错误, nObjAddr:%04X\n", g_aFaultDataConfigZhj[ArrIdx+0].nObjAddrU32);

        return 0;
    }

    id   = 0;
    i = 0;
    ReadRamXData(fault_report_ramoff[id].data_code, (char *)bdata, 9);//读取遥信存放数据字典
    bno  =    fault_report_ramoff[id].state_bit/8;//变位信息
    bsit =    fault_report_ramoff[id].state_bit%8;
    bfgchange = (bdata[bno]>>bsit)&0x01;

    EVENT_FMT_TRACE("event_check_faults, ID=%u bit:%u, state:%02x\n", id, fault_report_ramoff[id].state_bit, bdata[0]);

    if(bfgchange)
    {
        rtn = 1;

        //填写数据
        faultsenddata[i].bfgchange = 1;//变位标志
        
        if((bdata[bno]>>fault_report_ramoff[id].data_bit)&0x01) //发生
        {
            faultsenddata[i].objAddr = g_aFaultDataConfigZhj[ArrIdx+0].nObjAddrU32&0xffff;
        }
        else// 恢复
        {
            faultsenddata[i].objAddr = (g_aFaultDataConfigZhj[ArrIdx+0].nObjAddrU32+0x80)&0xffff;
        }

        EVENT_FMT_TRACE("event_check_faults, ID=%u bit:%u, state:%u\n", id, fault_report_ramoff[id].state_bit, bdata[bno]>>fault_report_ramoff[id].data_bit);
            
        bdata[bno] &= ~(0x01<<bsit);//清变位标志
        bdata[bno] &= ~(0x01<<fault_report_ramoff[id].data_bit); //发生标志
        WriteRamXData(fault_report_ramoff[id].data_code, (char*)bdata, 9);

        //变位发生时间填写
        GetCurrentOopDateTimeS(&SystemTime); //-取系统时钟-

        gettimeofday(&c_time, NULL);
        dtemp = ((uint16)SystemTime.second)*1000 + c_time.tv_usec/1000;
        faultsenddata[i].btime[0] =  dtemp&0xff;
        faultsenddata[i].btime[1] =  dtemp>>8;
        faultsenddata[i].btime[2] =  SystemTime.minute&0x3f;//高两位保留
        faultsenddata[i].btime[3] =  SystemTime.hour&0x1f;
        faultsenddata[i].btime[4] =  SystemTime.day&0x1f;

        if(SystemTime.week == 0)
            faultsenddata[i].btime[4] |= (7<<5);
        else
            faultsenddata[i].btime[4] |= (SystemTime.week<<5);

        faultsenddata[i].btime[5] =  SystemTime.month&0x0f;
        faultsenddata[i].btime[6] =  (SystemTime.year%100)&0x7f;

        faultsenddata[i].tel_num = g_aFaultDataConfigZhj[ArrIdx+0].nReportYcNum; //遥测数据个数

        faultsenddata[i].tel_ti_num = g_aFaultDataConfigZhj[ArrIdx+0].nReportYcTiNum; //遥测数据个数 带时标

        for(j=0;j<faultsenddata[i].tel_num;j++)
        {
            faultsenddata[i].tel_data[j].objAddr = g_aFaultDataConfigZhj[ArrIdx+0].aReportAddrU32[j];
            get_report_data(faultsenddata[i].tel_data[j].objAddr, &faultsenddata[i].tel_data[j].objdata);
        }

        for(j=faultsenddata[i].tel_num;j<faultsenddata[i].tel_num+faultsenddata[i].tel_ti_num;j++)
        {
            faultsenddata[i].tel_data[j].objAddr = g_aFaultDataConfigZhj[ArrIdx+0].aReportAddrU32[j];
            get_report_data(faultsenddata[i].tel_data[j].objAddr, &faultsenddata[i].tel_data[j].objdata);
        }
        
        faultsenddata[i].energy_num = g_aFaultDataConfigZhj[ArrIdx+0].nReportEnNum; //电能量数据个数
        for(j=0;j<faultsenddata[i].energy_num;j++)
        {
            faultsenddata[i].energy_data[j].objAddr = g_aFaultDataConfigZhj[ArrIdx+0].aReportAddrU32[faultsenddata[i].tel_num+faultsenddata[i].tel_ti_num+j];
            get_report_data(faultsenddata[i].energy_data[j].objAddr, &faultsenddata[i].energy_data[faultsenddata[i].tel_num+faultsenddata[i].tel_ti_num+j].objdata);

        }
    }

    return rtn;
}

/**********************************************************************
* @name      : event_check_faults_zhj
* @brief     ：检查是否有事件记录,并且缓存事件记录
* @param[in] ：
* @param[out]：
* @return    ：0没有记录，1有记录
* @Create    : 梅安宁
* @Date      ：2020-11-26
* @Update    :

**********************************************************************/
int event_check_faults_zhj(void)
{
    uint8 i ,j ,bno,bsit,bfgchange= 0;
    uint32 id = 0;
    uint8  bdata[32] = {0};
    uint8  rtn = 0;
    uint16 dtemp = 0;
    struct timeval   c_time;
    OOP_DATETIME_S_T SystemTime;

    rtn = tnm_event_check_faults();     //终端停上电
    
    if (g_nConfigFaultNumZhj[1][0][0] < 1 || g_nConfigFaultNumZhj[1][0][0] > FAULT_MAX_NUM)
    {
        EVENT_FMT_DEBUG("event_check_fault, 配置数量%u错误\n", g_nConfigFaultNumZhj[1][0][0]);

        return 0;
    }

    uint32 ArrIdx = 0;//一维数组索引(起始)
    ArrIdx = g_aFaultDataIndexZhj[1][0][0];

    for(i=0;i<g_nConfigFaultNumZhj[1][0][0];i++)
    {
        if (g_aFaultDataConfigZhj[ArrIdx+i].nObjAddrU32 < FALT_START_ADDR || g_aFaultDataConfigZhj[ArrIdx+i].nObjAddrU32 >= FALT_START_ADDR+fault_report_ramsize)
        {
            EVENT_FMT_DEBUG("event_check_fault, 第%u配置错误, nObjAddr:%04X\n", i, g_aFaultDataConfigZhj[ArrIdx+i].nObjAddrU32);

            continue;
        }

        if (g_aFaultDataConfigZhj[ArrIdx+i].nObjAddrU32 > 0x00000122)
        {
            break;
        }

        id   = g_aFaultDataConfigZhj[ArrIdx+i].nObjAddrU32 - FALT_START_ADDR;
        ReadRamXData(fault_report_ramoff[id].data_code, (char *)bdata, 9);//读取遥信存放数据字典
        bno  =    fault_report_ramoff[id].state_bit/8;//变位信息
        bsit =    fault_report_ramoff[id].state_bit%8;
        bfgchange = (bdata[bno]>>bsit)&0x01;

        EVENT_FMT_TRACE("event_check_fault, ID=%u bit:%u, state:%02x\n", id, fault_report_ramoff[id].state_bit, bdata[0]);

        if(bfgchange)
        {
            rtn = 1;

            //填写数据
            faultsenddata[id].bfgchange = 1;//变位标志
            if((bdata[bno]>>fault_report_ramoff[id].data_bit)&0x01) //发生
            {
                faultsenddata[id].objAddr = g_aFaultDataConfigZhj[ArrIdx+i].nObjAddrU32&0xffff;
            }
            else// 恢复
            {
                faultsenddata[id].objAddr = (g_aFaultDataConfigZhj[ArrIdx+i].nObjAddrU32+0x80)&0xffff;
            }

            EVENT_FMT_TRACE("event_check_fault, ID=%u bit:%u, state:%u\n", id, fault_report_ramoff[id].state_bit, bdata[bno]>>fault_report_ramoff[id].data_bit);
            
            bdata[bno] &= ~(0x01<<bsit);//清变位标志
            bdata[bno] &= ~(0x01<<fault_report_ramoff[id].data_bit);
            WriteRamXData(fault_report_ramoff[id].data_code, (char*)bdata, 9);

            //变位发生时间填写
            GetCurrentOopDateTimeS(&SystemTime); //-取系统时钟-

            gettimeofday(&c_time, NULL);
            dtemp = ((uint16)SystemTime.second)*1000 + c_time.tv_usec/1000;
            faultsenddata[id].btime[0] =  dtemp&0xff;
            faultsenddata[id].btime[1] =  dtemp>>8;
            faultsenddata[id].btime[2] =  SystemTime.minute&0x3f;//高两位保留
            faultsenddata[id].btime[3] =  SystemTime.hour&0x1f;
            faultsenddata[id].btime[4] =  SystemTime.day&0x1f;

            if(SystemTime.week == 0)
                faultsenddata[id].btime[4] |= (7<<5);
            else
                faultsenddata[id].btime[4] |= (SystemTime.week<<5);

            faultsenddata[id].btime[5] =  SystemTime.month&0x0f;
            faultsenddata[id].btime[6] =  (SystemTime.year%100)&0x7f;
            

            faultsenddata[id].tel_num = g_aFaultDataConfigZhj[ArrIdx+i].nReportYcNum; //遥测数据个数
            for(j=0;j<faultsenddata[id].tel_num;j++)
            {
                faultsenddata[id].tel_data[j].objAddr = g_aFaultDataConfigZhj[ArrIdx+i].aReportAddrU32[j];
                get_report_data(faultsenddata[id].tel_data[j].objAddr, &faultsenddata[id].tel_data[j].objdata);
            }

            faultsenddata[id].tel_ti_num = g_aFaultDataConfigZhj[ArrIdx+i].nReportYcTiNum; //遥测数据个数 带时标
            for(j=faultsenddata[id].tel_num;j<faultsenddata[id].tel_num+faultsenddata[id].tel_ti_num;j++)
            {
                faultsenddata[id].tel_data[j].objAddr = g_aFaultDataConfigZhj[ArrIdx+i].aReportAddrU32[j];
                get_report_data(faultsenddata[id].tel_data[j].objAddr, &faultsenddata[id].tel_data[j].objdata);
            }
            
            faultsenddata[id].energy_num = g_aFaultDataConfigZhj[ArrIdx+i].nReportEnNum; //电能量数据个数
            for(j=0;j<faultsenddata[id].energy_num;j++)
            {
                faultsenddata[id].energy_data[j].objAddr = g_aFaultDataConfigZhj[ArrIdx+i].aReportAddrU32[j+faultsenddata[id].tel_num+faultsenddata[id].tel_ti_num];
                get_report_data(faultsenddata[id].energy_data[j].objAddr, &faultsenddata[id].energy_data[j+faultsenddata[id].tel_num+faultsenddata[id].tel_ti_num].objdata);

            }

        }
    }

    return rtn;
}


/**********************************************************************
* @name      : event_get_faults_zhj
* @brief     ：读取故障事件记录
* @param[in] ：
* @param[out]：typeIndex         设备类型数组下标, 从0开始
               groupIndex    设备组别数组下标, 从0开始
               seqIndex      组内偏移地址数组下标, 从0开始
               pData 读取数据缓存 
* @return    ：读取的数据长度
* @Create    : 梅安宁
* @Date      ：2020-11-26
* @Update    :

**********************************************************************/
int event_get_faults_zhj(uint16 *typeIndex, uint16 *groupIndex, uint16 *seqIndex, uint8 *pData)
{
    uint8 i = 0;
    uint8 k = 0;
    uint8 len = 0;
    uint8 index = 0;

    *groupIndex = 0;
    *seqIndex = 0;
    
    for(i=0;i<FAULT_MAX_NUM;i++)
    {
        if(faultsenddata[i].bfgchange ==  1)
        {
            if(i == 0)
            {
                *typeIndex = 0;
            }
            else
            {
                *typeIndex = 1;
            }
            
            faultsenddata[i].bfgchange = 0;

            pData[index++]  =  faultsenddata[i].objAddr&0xff;//事件编码
            pData[index++]  =  faultsenddata[i].objAddr>>8;
           
            for(k=0;k<7;k++)//事件发生时刻
            {
                pData[index++] = faultsenddata[i].btime[k];
            }

            pData[index++] = 0;     //遥信个数0
            
            pData[index++] = faultsenddata[i].tel_num;       //遥测个数（不带时标）
            for(k=0;k<faultsenddata[i].tel_num;k++)
            {
                //信息对象地址
                pData[index++] = faultsenddata[i].tel_data[k].objAddr&0xff;
                pData[index++] = (faultsenddata[i].tel_data[k].objAddr>>8) & 0xff;
                pData[index++] = (faultsenddata[i].tel_data[k].objAddr>>16) & 0xff;

                //事件发生时刻数据
                memcpy((void *)&pData[index],(void *)&faultsenddata[i].tel_data[k].objdata,4);
                index += 4;

                pData[index++] = 0;     //品质描述词 QDS
            }
            
            pData[index++] = faultsenddata[i].tel_ti_num;       //遥测个数（带时标）
            for(k=faultsenddata[i].tel_num;k<faultsenddata[i].tel_num+faultsenddata[i].tel_ti_num;k++)
            {
                //信息对象地址
                pData[index++] = faultsenddata[i].tel_data[k].objAddr&0xff;
                pData[index++] = (faultsenddata[i].tel_data[k].objAddr>>8) & 0xff;
                pData[index++] = (faultsenddata[i].tel_data[k].objAddr>>16) & 0xff;

                //事件发生时刻数据
                memcpy((void *)&pData[index],(void *)&faultsenddata[i].tel_data[k].objdata,4);
                index += 4;

                pData[index++] = 0;     //品质描述词 QDS
                
                for(k=0;k<7;k++)//事件发生时刻
                {
                    pData[index++] = faultsenddata[i].btime[k];
                }
            }
            
            pData[index++] = faultsenddata[i].energy_num;       //电能量个数
            for(k=0;k<faultsenddata[i].energy_num;k++)
            {
                //信息对象地址
                pData[index++] = faultsenddata[i].energy_data[k].objAddr&0xff;
                pData[index++] = (faultsenddata[i].energy_data[k].objAddr>>8) & 0xff;
                pData[index++] = (faultsenddata[i].energy_data[k].objAddr>>16) & 0xff;

                //事件发生时刻数据
                memcpy((void *)&pData[index],(void *)&faultsenddata[i].energy_data[k].objdata,4);
                index += 4;

                pData[index++] = 0;     //品质描述词 QDS
            }

            pData[index++] = 0;     //比特串个数
            pData[index++] = 0;     //参数个数

            EVENT_FMT_TRACE("objAddr%04x\n", faultsenddata[i].objAddr);
            EVENT_BUF_TRACE(pData, index,"buf");
            break;
        }
    }

    len = index;

    return len;
}

