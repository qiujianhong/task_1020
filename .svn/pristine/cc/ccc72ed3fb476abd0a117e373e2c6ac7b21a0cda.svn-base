
#ifdef AREA_HUNAN

#include "baseCommon.h"
#include "oadInfoList.h"
#include "oopservice.h"
#include "commClass.h"
#include "oopStorage.h"
#include "area.h"
#include "hal.h"
#include "adc.h"
#include "power.h"
#include "../../Common698/AppCommon.h"
#include "../../event/Event.h"

//将版本信息以全局变量的形式写入各个地区的代码中，统一在 area.h 中引用
#ifdef ADV_PU_AMR
FACTORY_VER_TMP_T g_tVer = 
{
    "XXXX",      /**< 厂商代码           */
    "CM01",      /**< 软件版本号         */
    "220811",    /**< 软件版本日期       */
    "CC01",      /**< 硬件版本号         */
    "220621",    /**< 硬件版本日期       */
    "CM01-01",   /**< 厂家扩展信息       */
};
#elif ADV_ST_AMR
    #ifdef PRODUCT_ECU
    //能源控制器
    FACTORY_VER_TMP_T g_tVer = 
    {
        "XXXX",      /**< 厂商代码           */
        "EM01",      /**< 软件版本号         */
        "220619",    /**< 软件版本日期       */
        "ER01",      /**< 硬件版本号         */
        "220619",    /**< 硬件版本日期       */
        "EM01-01",   /**< 厂家扩展信息       */
    };
    #elif PRODUCT_ZCU_1
    //专变
    FACTORY_VER_TMP_T g_tVer = 
    {
        "XXXX",      /**< 厂商代码           */
        "1M01",      /**< 软件版本号         */
        "210207",    /**< 软件版本日期       */
        "Z101",      /**< 硬件版本号         */
        "200624",    /**< 硬件版本日期       */
        "1M01-01",   /**< 厂家扩展信息       */
    };
    #elif PRODUCT_ZCU_2
    //专变3型
    FACTORY_VER_TMP_T g_tVer = 
    {
        "XXXX",      /**< 厂商代码           */
        "2M01",      /**< 软件版本号         */
        "220715",    /**< 软件版本日期       */
        "Z301",      /**< 硬件版本号         */
        "220715",    /**< 硬件版本日期       */
        "2M01-01",   /**< 厂家扩展信息       */
    };
    #elif PRODUCT_ZCU_3
    //专变3型
    FACTORY_VER_TMP_T g_tVer = 
    {
        "XXXX",      /**< 厂商代码           */
        "3M01",      /**< 软件版本号         */
        "220716",    /**< 软件版本日期       */
        "Z301",      /**< 硬件版本号         */
        "220715",    /**< 硬件版本日期       */
        "3M01-01",   /**< 厂家扩展信息       */
    };
    #endif
#endif


#define USERTYPE_FRAME_BREAKER      212    //框架断路器 用户类型212 智能总开关
#define USERTYPE_LOW_BREAKER        213     //低压断路器 分支层
#define USERTYPE_METERBOX_BREAKER   220    //表箱断路器

#if DESC("停电处理",1)

uint16 g_val_a[OOP_MAX_POWERCUT_MET];     //停电时，电表A项电压

#endif

extern int IniEventCommInfo(EVENT_COMM_INFO *eventCommInfo);
extern int advance_send_pdsManager(uint16 label, uint16 iop, uint8* playload, uint16 length);

uint16 get_link_normal_data(int clientid,OOP_OAD_U oad,uint8* outbuf)
{
    int ret;
    //如果取终端数据 则取交采数据

    uint32 len=0;
    NOMAL_OAD_T NormalOad;
    NormalOad.logicId = 1;
    NormalOad.oad = oad;
    NormalOad.infoNum = 0;
    uint8 buf[1000];

    ret = db_read_nomal(clientid,&NormalOad,sizeof(buf),buf,&len);
    if(ret!=0)
    {
        EVENT_FMT_DEBUG("get_link_normal_data  oad=%08X error ret%d\n",  oad.value, ret);
        return 0;
    }
    memcpy(outbuf,buf,len);
    return len;
    
}
uint16 get_link_record_data(int clientid,uint8 logicid,uint16 infoNum,uint8* Addr,OOP_OAD_U oad,uint8* outbuf)
{
    int ret;
    //如果取终端数据 则取交采数据
    if(infoNum==0)
    {
        uint32 len=0;
        NOMAL_OAD_T NormalOad;
        NormalOad.logicId = 1;
        NormalOad.oad = oad;
        NormalOad.infoNum = 0;
        uint8 buf[1000];

        ret = db_read_nomal(clientid,&NormalOad,sizeof(buf),buf,&len);
        if(ret!=0)
        {
            EVENT_FMT_DEBUG("get_link_record_data infoNum=%u oad=%08X error ret%d\n", infoNum, oad.value, ret);
            return 0;
        }
        memcpy(outbuf,buf,len);
        return len;
    }
    READ_RECORD_T       startReadRecord;
    RECORD_UNIT_T       stepRecordOut;
    memset(&startReadRecord,0,sizeof(READ_RECORD_T));
    memset(&stepRecordOut,0,sizeof(RECORD_UNIT_T));
    startReadRecord.recordOAD.optional = 0;
    startReadRecord.recordOAD.road.oadCols.nNum = 1;
    startReadRecord.recordOAD.road.oadCols.oad[0] = oad;
    startReadRecord.recordOAD.logicId = logicid;
    startReadRecord.recordOAD.infoNum = infoNum;
    startReadRecord.cType = 0x00;
    startReadRecord.cStart = 0;
    startReadRecord.cEnd = time(NULL) + COMPENSATION_TIME;
    startReadRecord.sortType = 1;
    startReadRecord.MAC.nNum = 6;
    memcpy(startReadRecord.MAC.value, Addr, 6);
    ret = db_read_record(clientid, &startReadRecord, DATA_LEN_MAX, &stepRecordOut);
    if (ret != ERR_OK)
    {
        EVENT_FMT_DEBUG("get_link_record_data infoNum=%u oad=%08X error ret%d\n", infoNum, oad.value, ret);
        return 0;
    } 
    memcpy(outbuf,stepRecordOut.record[0].outDataBuf,stepRecordOut.record[0].outDatalen);
    return stepRecordOut.record[0].outDatalen;
}

/*********************************************************************
所属功能单元：
功能描述：线路开关变位事件 3360
输入参数：测量点号
输出参数：无
函数返回值：无
内部处理流程：
********************************************************************/
int CheckERC_3360(int clientid,uint8* databuf,uint16 datalen)
{

    //uint8 i;
    int ret;
    if(!event_3360.eventCommInfo.checkflag)
    {
        EVENT_FMT_LOGCC("CheckERC_3360 checkflag 0\n");
        return -1 ;
    }
    OOP_EVTSOURCE_T     eventSource; //事件发生源
	OOP_EVTBUF_T        eventData;   //事件关联数据
	memset(&eventSource, 0, sizeof(OOP_EVTSOURCE_T));
	memset(&eventData, 0, sizeof(OOP_EVTBUF_T));
    uint8 state; //开关分合状态 1 分 2合
    if(datalen!= sizeof(uint8))
    {
        EVENT_FMT_DEBUG("CheckERC_3360 datalen != 1\n");
        return -1 ;
    }
    state = databuf[0];
    if(state !=1 && state !=2)
    {
        EVENT_FMT_DEBUG("CheckERC_3360 state= %d 分合状态应该为1或者2 \n",state);
        return -1 ;
    }
    if(event_3360.breakstate==0)
    {
        event_3360.breakstate = state;
        EVENT_FMT_DEBUG("CheckERC_3360 state= %d  \n",state);
        return 0;
    }
    if(state ==event_3360.breakstate)
    {
        EVENT_FMT_DEBUG("CheckERC_3360 未发生变位事件 当前状态 %d  \n",state);
        return 0;
    }
    //发生变位 记录下目前状态 并产生事件
    event_3360.breakstate = state;
    //事件发生源 unsigned 备注：0为线路总开关，1、2、3…代表第n路分支线路开关
    //全部填0
	eventSource.choice = DT_UNSIGNED;
    eventSource.nValue = 0;
    //填默认关联对象属性
	eventData.cols.nNum = 1;
	eventData.oadNum = 1;
	//eventData.cols.oad[0].value = 0x40010200;
	eventData.cols.oad[0].value = 0x28000200;
//	eventData.cols.oad[2].value = 0x20000200;
//	eventData.cols.oad[3].value = 0x20010200;
//	eventData.cols.oad[4].value = 0x20010700;
//	eventData.idLen[0] = sizeof(OOP_OCTETVAR16_T);
	eventData.idLen[0] = sizeof(uint8);
//	eventData.idLen[2] = sizeof(OOP_WORD3V_T);
//	eventData.idLen[3] =sizeof(OOP_INT3V_T);
//	eventData.idLen[4] =sizeof(OOP_INT3V_T);
	
//	OOP_OCTETVAR16_T addr;
//	memset(&addr,0x00,sizeof(OOP_OCTETVAR16_T));
//	addr.nNum = 6;
//	memcpy(addr.value,oopMeter.basic.tsa.add,6);
//	memcpy(eventData.buf,&addr,eventData.idLen[0]);
    eventData.buf[0] = state;
	eventData.len+=eventData.idLen[0];
//	uint16 len;
//    for(i=1;i<5;i++)
//    {
//    	len = get_link_record_data( clientid, logicid,mtr_id, oopMeter.basic.tsa.add,eventData.cols.oad[i],eventData.buf+eventData.len); 
//        if(len == 0)
//        {
//            EVENT_FMT_DEBUG("获取关联对象0x%08x 数据失败 \n",eventData.cols.oad[i]);
//        }
//    	eventData.len+=eventData.idLen[i];
//	}
	ret = WriteEventToDB(clientid, 0, &event_3360.eventCommInfo, eventSource, eventData, 0);
    if (ret < ERR_OK)
	{
		EVENT_FMT_DEBUG("CheckERC_3360 记录失败返回%d\n", ret);
		return -1;
	}
	return 0;
}

/*********************************************************************
所属功能单元：事件处理模块提供应用层主调度使用
功能描述：从数据中心获取线路开关变位事件参数，写入全局变量中。
输入参数：
输出参数：
  event_3360   线路开关变位事件全局变量
函数返回值：
  0     成功
  <0    发生错误
内部处理流程：
    从数据中心获取线路开关变位事件参数，写入全局变量中。
********************************************************************/
int Init3360Para()
{

    memset(&event_3360, 0, sizeof(EVENT_3360_INFO));

    event_3360.eventCommInfo.eventOAD.nObjID= 0X3360;
    event_3360.eventCommInfo.eventOAD.value= 0X33600200;


    OOP_RCSD_T eventRCSD={
        10, 0,{
                {0,{{0X20220200},}},//事件记录序号
                {0,{{0X201E0200},}},//事件发生时间
                {0,{{0X20200200},}},//事件结束时间
                {0,{{0X20240200},}},//事件发生源
                {0,{{0X33000200},}},//通道上报状态

                {0,{{0x40010200},}},//通信地址
                {0,{{0x28000200},}},//线路开关分合状态
                {0,{{0x20000200},}},//电压
                {0,{{0x20010200},}},//电流
                {0,{{0x20010700},}},//漏电流
            }
    };

    event_3360.eventCommInfo.eventRCSD = eventRCSD;

    // 公共信息
    IniEventCommInfo(&event_3360.eventCommInfo);
 
    event_3360.initflag = 1;
    return ERR_OK;
}


/**********************************************************************
* @name      : get_temperature_param
* @brief     ：获取温度告警配置参数 33610600
* @param[in] ：int clientid    socket句柄
               uint8 logicId   逻辑地址
               uint16 infoNum  信息点号
               uint32 value    oad值
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 
* @Date      ：
* @Update    :
**********************************************************************/
int get_temperature_param(int clientid, uint8 logicId, uint16 infoNum, uint32 value)
{
    int              ret = 0;
    uint32           readlen = 0;
    NOMAL_OAD_T      tOAD;
    OOP_TemperatureWarningParam_T param;
    if(0x33610600 != value)
    {
        return -1;
    }
    memset(&tOAD, 0, sizeof(NOMAL_OAD_T));
    memset(&param,0,sizeof(OOP_TemperatureWarningParam_T));
    tOAD.oad.value = value;
    ret = db_read_nomal(clientid, &tOAD, sizeof(OOP_TemperatureWarningParam_T), (uint8 *)&param, &readlen);
    if(0 == ret && readlen == sizeof(OOP_TemperatureWarningParam_T))
    {
        memcpy(&event_3361.param,&param,sizeof(OOP_TemperatureWarningParam_T));
    }
    else
    {
        EVENT_FMT_DEBUG("get_temperature_param error \n");
    }
    return 0;
}
/*********************************************************************
所属功能单元：事件处理模块提供应用层主调度使用
功能描述：从数据中心获取环境温度告警事件参数，写入全局变量中。
输入参数：
输出参数：
  event_3361   环境温度告警事件全局变量
函数返回值：
  0     成功
  <0    发生错误
内部处理流程：
    从数据中心获取环境温度告警事件参数，写入全局变量中。
********************************************************************/
int Init3361Para()
{
    NOMAL_OAD_T nomalOAD;
    int ret;
    uint32 len;
    memset(&nomalOAD,0x00,sizeof(NOMAL_OAD_T));
    memset(&event_3361, 0, sizeof(EVENT_3361_INFO));

    event_3361.eventCommInfo.eventOAD.value= 0X33610200;

    OOP_RCSD_T eventRCSD={
        7, 0,{
                {0,{{0X20220200},}},//事件记录序号
                {0,{{0X201E0200},}},//事件发生时间
                {0,{{0X20200200},}},//事件结束时间
                {0,{{0X20240200},}},//事件发生源
                {0,{{0X33000200},}},//通道上报状态

                {0,{{0x26000200},}},//环境温度
                {0,{{0x20012200},}},//事件发生前电流
            }
    };

    event_3361.eventCommInfo.eventRCSD = eventRCSD;

    // 公共信息
    IniEventCommInfo(&event_3361.eventCommInfo);
     //获取 环境告警上下限
    nomalOAD.oad.value = 0x33610600;
    ret = db_read_nomal(gDbCenterClient, &nomalOAD, sizeof(OOP_TemperatureWarningParam_T), (uint8 *)&event_3361.param, &len);
    if (ret != ERR_OK || len == 0)
    {
        EVENT_FMT_DEBUG("nomal_data_read OAD=%8X error ret=%d,len=%u\n", nomalOAD.oad.value, ret, len);
    }

    EVENT_FMT_TRACE("event_3361.param.LowerLimit=%d \n", event_3361.param.LowerLimit);
    EVENT_FMT_TRACE("event_3361.param.UpperLimit =%d \n", event_3361.param.UpperLimit);
    ret = read_pdata_xram("^event_3361",(char*)event_3361.info,0,sizeof(EVENT_3361_METER)*EVENT_3361_RECORD_COUNT);
    if(ret!=0)
    {
        EVENT_FMT_TRACE("read_pdata_xram event_3361 error ret =%d \n", ret);
    }
    event_3361.initflag = 1;
    return ERR_OK;
}

int Init3362Para()
{
    int ret;
    memset(&event_3362, 0, sizeof(EVENT_3362_INFO));

    event_3362.eventCommInfo.eventOAD.value= 0X33620200;


    OOP_RCSD_T eventRCSD={
        7, 0,{
                {0,{{0X20220200},}},//事件记录序号
                {0,{{0X201E0200},}},//事件发生时间
                {0,{{0X20200200},}},//事件结束时间
                {0,{{0X20240200},}},//事件发生源
                {0,{{0X33000200},}},//通道上报状态

                {0,{{0x2B010200},}},//烟雾告警状态
                {0,{{0x20012200},}},//烟雾告警状态
            }
    };

    event_3362.eventCommInfo.eventRCSD = eventRCSD;

    // 公共信息
    IniEventCommInfo(&event_3362.eventCommInfo);

    ret = read_pdata_xram("^event",(char*)&event_3362.laststate,SmokeStateAddr,SmokeStateLen);
    {
        EVENT_FMT_TRACE("read_pdata_xram SmokeStateAddrerror ret =%d \n", ret);
    }
    event_3362.initflag = 1;
    return ERR_OK;
}

int Init3363Para()
{
    int ret;
    memset(&event_3363, 0, sizeof(EVENT_3362_INFO));

    event_3363.eventCommInfo.eventOAD.value= 0X33630200;


    OOP_RCSD_T eventRCSD={
        6, 0,{
                {0,{{0X20220200},}},//事件记录序号
                {0,{{0X201E0200},}},//事件发生时间
                {0,{{0X20200200},}},//事件结束时间
                {0,{{0X20240200},}},//事件发生源
                {0,{{0X33000200},}},//通道上报状态

                {0,{{0x2B020200},}},//浸水状态
            }
    };

    event_3363.eventCommInfo.eventRCSD = eventRCSD;

    // 公共信息
    IniEventCommInfo(&event_3363.eventCommInfo);
    ret = read_pdata_xram("^event",(char*)&event_3363.laststate,WaterStateAddr,WaterStateLen);
    {
        EVENT_FMT_TRACE("read_pdata_xram WaterStateAddr error ret =%d \n", ret);
    }
    event_3363.initflag = 1;
    return ERR_OK;
}


int CheckERC_3361(int clientid,uint8 logicid,uint16 mtr_id, OOP_METER_T oopMeter,uint8* databuf,uint16 datalen)
{

    uint8 i;
    int ret;
    EVENT_3361_METER *meterinfo=NULL;
    if(!event_3361.eventCommInfo.checkflag)
    {
        EVENT_FMT_DEBUG("CheckERC_3361 checkflag 0\n");
        return -1 ;
    }
    if(event_3361.param.LowerLimit==0&&event_3361.param.UpperLimit ==0)
    {
        EVENT_FMT_DEBUG("CheckERC_3361 告警上下限未配置\n");
        return -1 ;
    }
    int16 temperature ;
    uint8 flag = 0;
    OOP_LONG3V_T tem;
    memset(&tem,0x00,sizeof(OOP_LONG3V_T));
    
    if(datalen!=sizeof(OOP_LONG3V_T))
    {
        EVENT_FMT_DEBUG("CheckERC_3361 datalen %d != %d \n",datalen,sizeof(OOP_LONG3V_T));
        return -1 ;
    }
    memcpy(&tem,databuf,datalen);

    for(i=0;i<EVENT_3361_RECORD_COUNT;i++)
    {
        if(event_3361.info[i].isvalid==1&&mtr_id==event_3361.info[i].pn)
        {
            meterinfo = &event_3361.info[i];
            break;
        }
    }

    for(i=0;i<tem.nNum;i++)
    {
        if(0==get_bit_value((uint8*)&tem.rsv, 32,i))
            continue;
        temperature = tem.nValue[i];
        EVENT_FMT_LOGCC("CheckERC_3361 %d 当前温度 %d 下限 %d 上限 %d \n",i,temperature,event_3361.param.LowerLimit,event_3361.param.UpperLimit);
        //判断事件发生
        if(NULL==meterinfo)
        {
            if(temperature<event_3361.param.LowerLimit || temperature>event_3361.param.UpperLimit)
            {
                flag = 1;
                EVENT_FMT_DEBUG("CheckERC_3361 产生温度告警事件 \n");
                break;
            }
        }else
        {
            //判断事件恢复
            if(temperature>=event_3361.param.LowerLimit && temperature<=event_3361.param.UpperLimit)
            {
                flag = 1;
                EVENT_FMT_DEBUG("CheckERC_3361 温度告警事件恢复 \n");
                break;
            }
        }

    }
    if(flag == 0)
    {
        return -1;
    }

    OOP_EVTSOURCE_T     eventSource; //事件发生源
	OOP_EVTBUF_T        eventData;   //事件关联数据
	memset(&eventSource, 0, sizeof(OOP_EVTSOURCE_T));
	memset(&eventData, 0, sizeof(OOP_EVTBUF_T));
	eventSource.choice = DT_TSA;
	memcpy(&eventSource.add,&oopMeter.basic.tsa,sizeof(OOP_TSA_T));
	eventData.cols.nNum = 2;
	eventData.oadNum = 2;
	eventData.cols.oad[0].value = 0x26000200;
	eventData.idLen[0] = sizeof(OOP_LONG3V_T);
    memcpy(eventData.buf,&tem,sizeof(OOP_LONG3V_T));
    eventData.len+=eventData.idLen[0] ;
    eventData.cols.oad[1].value = 0x20012200;
	eventData.idLen[1] = sizeof(OOP_INT3V_T);
	uint16 len;
	OOP_OAD_U oad = {0x20010200};
    len = get_link_record_data( clientid, logicid,mtr_id, oopMeter.basic.tsa.add,oad,eventData.buf+eventData.len); 
    if(len==0)
    {
        EVENT_FMT_DEBUG("CheckERC_3361 获取事件发生前电流失败 \n");
    }
    eventData.len+=eventData.idLen[1] ;
	
    ret = WriteEventToDB(clientid, mtr_id, &event_3361.eventCommInfo, eventSource, eventData, 0);
    if (ret < ERR_OK)
	{
		EVENT_FMT_DEBUG("CheckERC_3361 记录失败返回%d\n", ret);
		return -1;
	}
	return 0;
}

int CheckERC_3362(int clientid,uint8* databuf,uint16 datalen)
{
    if(!event_3362.eventCommInfo.checkflag)
    {
        EVENT_FMT_DEBUG("CheckERC_3362 checkflag 0\n");
        return -1 ;
    }
    uint8 state;
    int ret;
    if(datalen!=1)
    {
        EVENT_FMT_DEBUG("CheckERC_3362 errordatalen %d \n",datalen);
        return -1 ;
    }
    state = databuf[0];
    if(state ==event_3362.laststate )
    {
        return -1 ;
    }

    uint8 *paddr = NULL;
    uint8 addrlen;
    OOP_EVTSOURCE_T     eventSource; //事件发生源
	OOP_EVTBUF_T        eventData;   //事件关联数据
	memset(&eventSource, 0, sizeof(OOP_EVTSOURCE_T));
	memset(&eventData, 0, sizeof(OOP_EVTBUF_T));
	eventSource.choice = DT_TSA;
	addrlen = app_get_localaddr(&paddr);
	if(addrlen!=0)
	{	
	    eventSource.add.len = addrlen-1;
	    memcpy(eventSource.add.add,paddr,addrlen);
	}

	eventData.cols.nNum = 2;
	eventData.oadNum = 2;
	eventData.cols.oad[0].value = 0x2B010200;
	eventData.idLen[0] = sizeof(uint8);
    memcpy(eventData.buf,&state,sizeof(uint8));
    eventData.len+=eventData.idLen[0] ;
    eventData.cols.oad[1].value = 0x20012200;
	eventData.idLen[1] = sizeof(OOP_INT3V_T);
	uint16 len;
	OOP_OAD_U oad = {0x20010200};
    len = get_link_normal_data( clientid, oad,eventData.buf+eventData.len); 
    if(len==0)
    {
        EVENT_FMT_DEBUG("CheckERC_3362 获取事件发生前电流失败\n");
    }
    eventData.len+=eventData.idLen[1] ;
    if(state==1)
    {
        ret = WriteEventToDB(clientid, 0, &event_3362.eventCommInfo, eventSource, eventData, 1);
    }else
    {
        ret = WriteEventToDB(clientid, 0, &event_3362.eventCommInfo, eventSource, eventData, 2);
    }
    event_3362.laststate = state;
    write_pdata_xram("^event",(char*)&event_3362.laststate,SmokeStateAddr,SmokeStateLen);
    if (ret < ERR_OK)
	{
		EVENT_FMT_DEBUG("CheckERC_3362 记录失败返回%d\n", ret);
		return -1;
	}
	return 0;
}

int CheckERC_3363(int clientid,uint8* databuf,uint16 datalen)
{
    if(!event_3363.eventCommInfo.checkflag)
    {
        EVENT_FMT_DEBUG("CheckERC_3363 checkflag 0\n");
        return -1 ;
    }
    uint8 state;
    int ret;
    if(datalen!=1)
    {
        EVENT_FMT_DEBUG("CheckERC_3363 datalen %d \n",datalen);
        return -1 ;
    }
    state = databuf[0];
    if(state ==event_3363.laststate )
    {
        return -1 ;
    }
    uint8 *paddr = NULL;
    uint8 addrlen;
    OOP_EVTSOURCE_T     eventSource; //事件发生源
	OOP_EVTBUF_T        eventData;   //事件关联数据
	memset(&eventSource, 0, sizeof(OOP_EVTSOURCE_T));
	memset(&eventData, 0, sizeof(OOP_EVTBUF_T));
	eventSource.choice = DT_TSA;
	addrlen = app_get_localaddr(&paddr);
	if(addrlen!=0)
	{	
	    eventSource.add.len = addrlen-1;
	    memcpy(eventSource.add.add,paddr,addrlen);
	}

	eventData.cols.nNum = 1;
	eventData.oadNum = 1;
	eventData.cols.oad[0].value = 0x2B020200;
	eventData.idLen[0] = sizeof(uint8);
    memcpy(eventData.buf,&state,sizeof(uint8));
    eventData.len+=eventData.idLen[0] ;

    if(state==1)
    {
        ret = WriteEventToDB(clientid, 0, &event_3363.eventCommInfo, eventSource, eventData, 1);
    }else
    {
        ret = WriteEventToDB(clientid, 0, &event_3363.eventCommInfo, eventSource, eventData, 2);
    }
    event_3363.laststate = state;
    write_pdata_xram("^event",(char*)&event_3363.laststate,WaterStateAddr,WaterStateLen);
    if (ret < ERR_OK)
	{
		EVENT_FMT_DEBUG("CheckERC_3363 记录失败返回%d\n", ret);
		return -1;
	}
	return 0;
}


/**********************************************************************
* @name      : get_smoke_state
* @brief     ：获取烟雾状态 2B010200
* @param[in] ：int clientid    socket句柄
               uint8 logicId   逻辑地址
               uint16 infoNum  信息点号
               uint32 value    oad值
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 
* @Date      ：
* @Update    :
**********************************************************************/
int get_smoke_state(int clientid, uint8 logicId, uint16 infoNum, uint32 value)
{
    int              ret = 0;
    uint32           readlen = 0;
    NOMAL_OAD_T      tOAD;
    uint8  state = 0;

    if(0x2B010200 != value)
    {
        return -1;
    }
    memset(&tOAD, 0, sizeof(NOMAL_OAD_T));

    tOAD.oad.value = value;
 
    ret = db_read_nomal(clientid, &tOAD, sizeof(uint8), &state, &readlen);
    if(0 == ret && readlen == sizeof(uint8))
    {
        CheckERC_3362( clientid,&state, readlen);
    }
    else
    {
        EVENT_FMT_DEBUG("get_smoke_state error \n");
    }
    return 0;
}

/**********************************************************************
* @name      : get_water_state
* @brief     ：获取水浸状态 2B020200
* @param[in] ：int clientid    socket句柄
               uint8 logicId   逻辑地址
               uint16 infoNum  信息点号
               uint32 value    oad值
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 
* @Date      ：
* @Update    :
**********************************************************************/
int get_water_state(int clientid, uint8 logicId, uint16 infoNum, uint32 value)
{
    int              ret = 0;
    uint32           readlen = 0;
    NOMAL_OAD_T      tOAD;
    uint8  state = 0;

    if(0x2B020200 != value)
    {
        return -1;
    }
    memset(&tOAD, 0, sizeof(NOMAL_OAD_T));

    tOAD.oad.value = value;
 
    ret = db_read_nomal(clientid, &tOAD, sizeof(uint8), &state, &readlen);
    if(0 == ret && readlen == sizeof(uint8))
    {
        CheckERC_3363( clientid,&state, readlen);
    }
    else
    {
        EVENT_FMT_DEBUG("get_water_state error \n");
    }
    return 0;
}

/**********************************************************************
* @name      : get_temperature
* @brief     ：获取终端温度 26000200
* @param[in] ：int clientid    socket句柄
               uint8 logicId   逻辑地址
               uint16 infoNum  信息点号
               uint32 value    oad值
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 
* @Date      ：
* @Update    :
**********************************************************************/
int get_temperature(int clientid, uint8 logicId, uint16 infoNum, uint32 value)
{
    int              ret = 0;
    uint32           readlen = 0;
    NOMAL_OAD_T      tOAD;
    OOP_LONG3V_T data;
    memset(&data,0x00,sizeof(OOP_LONG3V_T));

    if(0x26000200 != value)
    {
        return -1;
    }
    memset(&tOAD, 0, sizeof(NOMAL_OAD_T));

    tOAD.oad.value = value;
 
    ret = db_read_nomal(clientid, &tOAD, sizeof(OOP_LONG3V_T), (uint8*)&data, &readlen);
    if(0 == ret && readlen == sizeof(OOP_LONG3V_T))
    {
        OOP_METER_T temp;
        memset(&temp,0x00,sizeof(OOP_METER_T));
        uint8 addrlen;
        uint8 *paddr= NULL;
        addrlen = app_get_localaddr(&paddr);
        if(addrlen!=0)
        {
            temp.basic.tsa.len = addrlen-1;
            memcpy(temp.basic.tsa.add,paddr,addrlen);
        }
        //终端的温度告警事件发生源填终端地址
        CheckERC_3361( clientid,0,0,temp,(uint8*)&data,readlen);
    }
    else
    {
        EVENT_FMT_DEBUG("get_temperature error \n");
    }
    return 0;
}

/**********************************************************************
* @name      : get_switch_state
* @brief     ：获取开关变位状态 28000200
* @param[in] ：int clientid    socket句柄
               uint8 logicId   逻辑地址
               uint16 infoNum  信息点号
               uint32 value    oad值
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 
* @Date      ：
* @Update    :
**********************************************************************/
int get_switch_state(int clientid, uint8 logicId, uint16 infoNum, uint32 value)
{
    int              ret = 0;
    uint32           readlen = 0;
    NOMAL_OAD_T      tOAD;
    uint8 data = 0;

    if(0x28000200 != value)
    {
        return -1;
    }
    memset(&tOAD, 0, sizeof(NOMAL_OAD_T));

    tOAD.oad.value = value;
 
    ret = db_read_nomal(clientid, &tOAD, sizeof(uint8),&data, &readlen);
    if(0 == ret && readlen == sizeof(uint8))
    {

        //终端的温度告警事件发生源填终端地址
        CheckERC_3360( clientid,&data,readlen);
    }
    else
    {
        EVENT_FMT_DEBUG("get_switch_state error \n");
    }
    return 0;
}

#if DESC("规约处理",1)

/* 变量类OAD列表 OIA1 = 0x02 */
const OAD_TAB_T hunan_oadInfoList[] = 
{
    /*OAD          class    A1      A2    rsv     dataType                 len*/
    {{0x20A10200}, 3,       0x02,   0x00, 0,      DT_ARRAY,                sizeof(OOP_DWORD3V_T),   E_OOP_DWORD3V_T},          //线路阻抗值

    {{0x22120200}, 6,       0x02,   0x02, 0,      DT_UNSIGNED,             sizeof(uint8),           E_UNSIGNED},               //当前电池状态
    {{0x28000200}, 6,       0x02,   0x08, 0,      DT_ENUM,                 sizeof(uint8),           E_ENUM},                   //开关分合状态
    {{0x2B010200}, 6,       0x02,   0x0B, 0,      DT_ENUM,                 sizeof(uint8),           E_ENUM},                   //烟雾告警状态
    {{0x2B020200}, 6,       0x02,   0x0B, 0,      DT_ENUM,                 sizeof(uint8),           E_ENUM},                   //水浸状态

    {{0x2A000200}, 3,       0x02,   0x0A, 0,      DT_ARRAY,                sizeof(OOP_LONG3V_T),    E_OOP_LONG3V_T},           //变压器油温
    {{0x2A010200}, 3,       0x02,   0x0A, 0,      DT_ARRAY,                sizeof(OOP_LONG3V_T),    E_OOP_LONG3V_T},           //变压器外壳温度
    {{0x2A030200}, 3,       0x02,   0x0A, 0,      DT_ARRAY,                sizeof(OOP_LONG3V_T),    E_OOP_LONG3V_T},           //变压器抽头温度
    {{0x2A040200}, 3,       0x02,   0x0A, 0,      DT_ARRAY,                sizeof(OOP_LONG3V_T),    E_OOP_LONG3V_T},           //漏保开关温度
    {{0x2A050200}, 3,       0x02,   0x0A, 0,      DT_ARRAY,                sizeof(OOP_LONG3V_T),    E_OOP_LONG3V_T},           //分支接头温度
    {{0x2A060200}, 3,       0x02,   0x0A, 0,      DT_ARRAY,                sizeof(OOP_LONG3V_T),    E_OOP_LONG3V_T},           //表箱接头温度

    {{0x31f20200}, 7,       0x03,   0x01, 0,      DT_STRUCTURE,            sizeof(OOP_IMPE_OVER_T), E_OOP_IMPE_OVER_T},        //线路阻抗越限事件 - 事件记录表
    {{0x31f20300}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),      E_OOP_OADS_T},             //线路阻抗越限事件 - 关联对象属性表
    {{0x31f20400}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),          E_LONG_UNSIGNED},          //线路阻抗越限事件 - 当前记录数
    {{0x31f20500}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),          E_LONG_UNSIGNED},          //线路阻抗越限事件 - 最大记录数
    {{0x31f20600}, 7,       0x03,   0x01, 0,      DT_DOUBLE_LONG_UNSIGNED, sizeof(uint32),          E_OOP_EVTCFG_31F2_T},      //线路阻抗越限事件 - 配置参数
    {{0x31f20700}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),   E_OOP_EVTSTAT_T},          //线路阻抗越限事件 - 当前值记录表
    {{0x31f20800}, 7,       0x03,   0x01, 0,      DT_ENUM,                 sizeof(uint8),           E_UNSIGNED},               //线路阻抗越限事件 - 上报标识
    {{0x31f20900}, 7,       0x03,   0x01, 0,      DT_BOOL,                 sizeof(uint8),           E_UNSIGNED},               //线路阻抗越限事件 - 有效标识
    {{0x31f20A00}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),   E_OOP_EVTTIME_T},          //线路阻抗越限事件 - 时间状态记录表
    {{0x33f20200}, 8,		0x03,	0x01, 0,  	  DT_ENUM,				   sizeof(uint8),			E_ENUM},

    {{0x33600200}, 7,       0x03,   0x03, 0,      DT_STRUCTURE,            sizeof(OOP_EVENT_T),     E_OOP_EVENT_T},            //线路开关变位事件 - 事件记录表
    {{0x33600300}, 7,       0x03,   0x03, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),      E_OOP_OADS_T},             //线路开关变位事件 - 关联对象属性表
    {{0x33600400}, 7,       0x03,   0x03, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),          E_LONG_UNSIGNED},          //线路开关变位事件 - 当前记录数
    {{0x33600500}, 7,       0x03,   0x03, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),          E_LONG_UNSIGNED},          //线路开关变位事件 - 最大记录数
    {{0x33600700}, 7,       0x03,   0x03, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),   E_OOP_EVTSTAT_T},          //线路开关变位事件 - 当前值记录表
    {{0x33600800}, 7,       0x03,   0x03, 0,      DT_ENUM,                 sizeof(uint8),           E_UNSIGNED},               //线路开关变位事件 - 上报标识
    {{0x33600900}, 7,       0x03,   0x03, 0,      DT_BOOL,                 sizeof(uint8),           E_UNSIGNED},               //线路开关变位事件 - 有效标识
    {{0x33600A00}, 7,       0x03,   0x03, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),   E_OOP_EVTTIME_T},          //线路开关变位事件 - 时间状态记录表  
    
    {{0x33610200}, 7,       0x03,   0x03, 0,      DT_STRUCTURE,            sizeof(OOP_EVENT_T),     E_OOP_EVENT_T},            //环境温度告警事件 - 事件记录表
    {{0x33610300}, 7,       0x03,   0x03, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),      E_OOP_OADS_T},             //环境温度告警事件 - 关联对象属性表
    {{0x33610400}, 7,       0x03,   0x03, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),          E_LONG_UNSIGNED},          //环境温度告警事件 - 当前记录数
    {{0x33610500}, 7,       0x03,   0x03, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),          E_LONG_UNSIGNED},          //环境温度告警事件 - 最大记录数
    {{0x33610600}, 7,       0x03,   0x03, 0,      DT_STRUCTURE,            sizeof(OOP_TemperatureWarningParam_T), E_OOP_TemperatureWarningParam_T}, //环境温度告警事件 - 配置参数
    {{0x33610700}, 7,       0x03,   0x03, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),   E_OOP_EVTSTAT_T},          //环境温度告警事件 - 当前值记录表
    {{0x33610800}, 7,       0x03,   0x03, 0,      DT_ENUM,                 sizeof(uint8),           E_UNSIGNED},               //环境温度告警事件 - 上报标识
    {{0x33610900}, 7,       0x03,   0x03, 0,      DT_BOOL,                 sizeof(uint8),           E_UNSIGNED},               //环境温度告警事件 - 有效标识
    {{0x33610A00}, 7,       0x03,   0x03, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),   E_OOP_EVTTIME_T},          //环境温度告警事件 - 时间状态记录表
    
    {{0x33620200}, 7,       0x03,   0x03, 0,      DT_STRUCTURE,            sizeof(OOP_EVENT_T),     E_OOP_EVENT_T},            //环境湿度告警事件 - 事件记录表
    {{0x33620300}, 7,       0x03,   0x03, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),      E_OOP_OADS_T},             //环境湿度告警事件 - 关联对象属性表
    {{0x33620400}, 7,       0x03,   0x03, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),          E_LONG_UNSIGNED},          //环境湿度告警事件 - 当前记录数
    {{0x33620500}, 7,       0x03,   0x03, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),          E_LONG_UNSIGNED},          //环境湿度告警事件 - 最大记录数
    {{0x33620700}, 7,       0x03,   0x03, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),   E_OOP_EVTSTAT_T},          //环境湿度告警事件 - 当前值记录表
    {{0x33620800}, 7,       0x03,   0x03, 0,      DT_ENUM,                 sizeof(uint8),           E_UNSIGNED},               //环境湿度告警事件 - 上报标识
    {{0x33620900}, 7,       0x03,   0x03, 0,      DT_BOOL,                 sizeof(uint8),           E_UNSIGNED},               //环境湿度告警事件 - 有效标识
    {{0x33620A00}, 7,       0x03,   0x03, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),   E_OOP_EVTTIME_T},          //环境湿度告警事件 - 时间状态记录表  
    
    {{0x33630200}, 7,       0x03,   0x03, 0,      DT_STRUCTURE,            sizeof(OOP_EVENT_T),     E_OOP_EVENT_T},            //浸水告警事件 - 事件记录表
    {{0x33630300}, 7,       0x03,   0x03, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),      E_OOP_OADS_T},             //浸水告警事件 - 关联对象属性表
    {{0x33630400}, 7,       0x03,   0x03, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),          E_LONG_UNSIGNED},          //浸水告警事件 - 当前记录数
    {{0x33630500}, 7,       0x03,   0x03, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),          E_LONG_UNSIGNED},          //浸水告警事件 - 最大记录数
    {{0x33630700}, 7,       0x03,   0x03, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),   E_OOP_EVTSTAT_T},          //浸水告警事件 - 当前值记录表
    {{0x33630800}, 7,       0x03,   0x03, 0,      DT_ENUM,                 sizeof(uint8),           E_UNSIGNED},               //浸水告警事件 - 上报标识
    {{0x33630900}, 7,       0x03,   0x03, 0,      DT_BOOL,                 sizeof(uint8),           E_UNSIGNED},               //浸水告警事件 - 有效标识
    {{0x33630A00}, 7,       0x03,   0x03, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),   E_OOP_EVTTIME_T},          //浸水告警事件 - 时间状态记录表

    {{0x60F10200}, 11,      0x06,   0x00, 0,      DT_ARRAY,                sizeof(OOP_NodeTopology_T), E_OOP_NodeTopology_T},  //节点拓扑信息
    {{0x60E10200}, 8,       0x06,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_SENSOR_PARAM_T), E_OOP_SENSOR_PARAM_T},  //温湿度传感器
    {{0x60E20200}, 8,       0x06,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_SENSOR_PARAM_T), E_OOP_SENSOR_PARAM_T},  //温湿度传感器
    {{0x60E30200}, 8,       0x06,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_SENSOR_PARAM_T), E_OOP_SENSOR_PARAM_T},  //温湿度传感器
    {{0x60E40200}, 8,       0x06,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_SENSOR_PARAM_T), E_OOP_SENSOR_PARAM_T},  //温湿度传感器
};

const uint32 hunan_oadInfoListLen = sizeof(hunan_oadInfoList)/sizeof(hunan_oadInfoList[0]);

/**
*********************************************************************
* @name：       class_hunan_oadinfo_get
* @brief：      查询ODA列表，得到OAD描述信息
* @param[in] ：OOP_OAD_U oad        查询OAD
* @param[out]：OAD_INFO_T *info     ODA信息
* @return    ：查询结果 TRUE-成功，FALSE-失败
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
int class_hunan_oadinfo_get(OOP_OAD_U oad, OAD_INFO_T *pstOdaInfo)
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
    

    pstOdaTab = hunan_oadInfoList; size = hunan_oadInfoListLen;

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
        return class_hunan_oadinfo_get(blockOAD, pstOdaInfo);
    }

    PRTL_FMT_DEBUG("search oadInfoList miss. OAD(0x%08x), OIA1(%d), OIA2(%d)\n", oad.value, OIA1, OIA2);

    return FALSE;
}

/* 自定义OMD列表 OIA1 = 0x0F OIA2 = 0x0F */
const OMD_INFO_T hunan_omdInfoList[] = 
{
    /*OMD          class   A1      A2    rsv     dataType                  len*/
    {{0x60F17F00},   11,     0x06,   0x00,  0,     DT_LONG_UNSIGNED,         sizeof(uint16),           E_LONG_UNSIGNED},  //实时启动拓扑
};
const uint32 hunan_omdInfoListLen = sizeof(hunan_omdInfoList)/sizeof(hunan_omdInfoList[0]);

/**
*********************************************************************
* @name：       class_hunan_omdinfo_get
* @brief：      查询ODA列表，得到OAD描述信息
* @param[in] ：OOP_OAD_U oad        查询OAD
* @param[out]：OAD_INFO_T *info     ODA信息
* @return    ：查询结果 TRUE-成功，FALSE-失败
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
int class_hunan_omdinfo_get(OOP_OAD_U oad, OAD_INFO_T *pstOdaInfo)
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
     
    pstOdaTab = hunan_omdInfoList; size = hunan_omdInfoListLen;

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
        return class_hunan_omdinfo_get(blockOAD, pstOdaInfo);
    }

    PRTL_FMT_DEBUG("search omdInfoList miss. OAD(0x%08x), OIA1(%d), OIA2(%d)\n", oad.value, OIA1, OIA2);

    return FALSE;
}

/**
*********************************************************************
* @brief：      事件关联OAD，子结构体拼装报文（临时函数）
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
int32 data_to_buf_evt_oad_hunan(OOP_OAD_U oad, void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen)
{
    int32 nRet = ERR_NORMAL;
    uint8 *dstBufTmp = (uint8 *)dstBuf; 
    uint32 offset = *pOffset;
    uint8 i = 0;

    switch(oad.value)
    {
        case 0x43002302:   //事件发生前软件版本号
        case 0x43004302:   //事件发生后软件版本号
        {
            OOP_VERSION_T version;
            memcpy(&version.szSoftVer, srcData, srcLen);
            
            dstBufTmp[offset++] = DT_VISIBLE_STRING;
            dstBufTmp[offset++] = 4;
            memcpy(&dstBufTmp[offset], version.szSoftVer.value, 4);    
            offset += 4;      

        }break;
        
        case 0x60412200:  //事件发生前最近一次抄表成功时间
        case 0x201E4200:  //事件发生时间
        case 0x40002200:  //对时前时间
        case 0x40008200:  //对时后时间        
        {
            if(basic_data_to_buf(E_DATETIME_S, dstBufTmp, dstLen-offset, &offset, srcData, srcLen) < 0)
            {
                nRet = ERR_NORMAL;
                return nRet;
            }
        }break;
    
        case 0xF2034201:  //第1路状态变位发生后状态
        case 0xF2034202:  //第2路状态变位发生后状态
        case 0xF2034203:  //第3路状态变位发生后状态
        case 0xF2034204:  //第4路状态变位发生后状态   
        case 0xF2034205:  //第5路状态变位发生后状态
        case 0xF2034206:  //第6路状态变位发生后状态
        case 0xF2034207:  //第7路状态变位发生后状态
        case 0xF2034208:  //第8路状态变位发生后状态           
        {
            INPUTSTATE swstate;
            memcpy(&swstate, srcData, srcLen);
    
            dstBufTmp[offset++] = DT_STRUCTURE;
    
            //结构成员数
            dstBufTmp[offset++] = 2; 
            
            if(basic_data_to_buf(E_UNSIGNED, dstBufTmp, dstLen-offset, &offset, &swstate.st, sizeof(uint8)) < 0)
            {
                nRet = ERR_NORMAL;
                return nRet;
            }
            
            if(basic_data_to_buf(E_UNSIGNED, dstBufTmp, dstLen-offset, &offset, &swstate.cd, sizeof(uint8)) < 0)
            {
                nRet = ERR_NORMAL;
                return nRet;
            }
  
        }break;
    
        case 0x22004202:   //事件发生后，当月已发生通信流量
        case 0x31100601:   //月通信流量门限
        {
            if(basic_data_to_buf(E_DOUBLE_LONG_UNSIGNED, dstBufTmp, dstLen-offset, &offset, srcData, sizeof(uint32)) < 0)
            {
                nRet = ERR_NORMAL;
                return nRet;
            } 

        }break;
        case 0x20002201:   //事件发生前电压
        case 0x20002202:   //事件发生前电压
        case 0x20002203:   //事件发生前电压
        case 0x20004201:   //事件发生后电压
        case 0x20004202:   //事件发生后电压
        case 0x20004203:   //事件发生后电压
        case 0x20006201:   //事件结束前电压
        case 0x20006202:   //事件结束前电压
        case 0x20006203:   //事件结束前电压
        case 0x20008201:   //事件结束后电压
        case 0x20008202:   //事件结束后电压
        case 0x20008203:   //事件结束后电压
        {
                if(basic_data_to_buf(E_LONG_UNSIGNED, dstBufTmp, dstLen-offset, &offset, srcData, sizeof(uint16)) < 0)
                {
                    nRet = ERR_NORMAL;
                    return nRet;
                }
        }break;  
    
        case 0x20034200:   //事件发生时电压电流相角
        case 0x20000200:   //电压
        case 0x20002200:   //事件发生前电压
        case 0x20004200:   //事件发生后电压
        {
            OOP_WORD3V_T var; 
            memcpy(&var, srcData, srcLen);
    
            dstBufTmp[offset++] = DT_ARRAY;
    
            //数组成员个数
            dstBufTmp[offset++] = var.nNum;
    
            for(i = 0; i < var.nNum; i++)
            {
                if(basic_data_to_buf(E_LONG_UNSIGNED, dstBufTmp, dstLen-offset, &offset, &var.nValue[i], sizeof(uint16)) < 0)
                {
                    nRet = ERR_NORMAL;
                    return nRet;
                }
            }
        }break;  
    
        case 0x00102201:  //事件发生前的正向有功总电能
        case 0x00112201:  //事件发生前的A相正向有功电能 
        case 0x00122201:  //事件发生前的B相正向有功电能        
        case 0x00132201:  //事件发生前的C相正向有功电能        
        
        case 0x00104201:  //事件发生时的正向有功总电能
        
        case 0x00202201:  //事件发生前的反向有功总电能
        case 0x00212201:  //事件发生前的A相反向有功电能 
        case 0x00222201:  //事件发生前的B相反向有功电能        
        case 0x00232201:  //事件发生前的C相反向有功电能  
        
        case 0x00108201:  //事件结束后的正向有功总电能  
        case 0x00118201:  //事件结束后的A相正向有功电能 
        case 0x00128201:  //事件结束后的B相正向有功电能        
        case 0x00138201:  //事件结束后的C相正向有功电能     

        case 0x00208201:  //事件结束后的反向有功总电能  
        case 0x00218201:  //事件结束后的A相反向有功电能 
        case 0x00228201:  //事件结束后的B相反向有功电能        
        case 0x00238201:  //事件结束后的C相反向有功电能         
        {
            uint32 energys;
            
            if(srcLen != sizeof(uint32))
            {
                PRTL_FMT_DEBUG("srcLen check failed. srcLen(%d) != %d \n", srcLen, sizeof(uint32));
                return ERR_NORMAL;
            }
            
            memcpy(&energys, srcData, srcLen);  

            if(energys == 0) //TODO 姑且使用0作为不存在的判据
            {
                dstBufTmp[offset++] = DT_NULL;
            }
            else
            {
                if(basic_data_to_buf(E_DOUBLE_LONG_UNSIGNED, dstBufTmp, dstLen-offset, &offset, &energys, sizeof(uint32)) < 0)
                {
                    nRet = ERR_NORMAL;
                    return nRet;
                }
            }
        }break; 
        
        case 0x00302201:  //事件发生前的组合无功1总电能 
        case 0x00312201:  //事件发生前的A相组合无功1电能 
        case 0x00322201:  //事件发生前的B相组合无功1电能        
        case 0x00332201:  //事件发生前的C相组合无功1电能  
        
        case 0x00402201:  //事件发生前的组合无功2总电能  
        case 0x00412201:  //事件发生前的A相组合无功2电能 
        case 0x00422201:  //事件发生前的B相组合无功2电能        
        case 0x00432201:  //事件发生前的C相组合无功2电能  

        case 0x00308201:  //事件结束后的组合无功1总电能  
        case 0x00318201:  //事件结束后的A相组合无功1电能 
        case 0x00328201:  //事件结束后的B相组合无功1电能        
        case 0x00338201:  //事件结束后的C相组合无功1电能     

        case 0x00408201:  //事件结束后的组合无功2总电能  
        case 0x00418201:  //事件结束后的A相组合无功2电能 
        case 0x00428201:  //事件结束后的B相组合无功2电能        
        case 0x00438201:  //事件结束后的C相组合无功2电能         
        {
            int energys;

            if(srcLen != sizeof(int))
            {
                PRTL_FMT_DEBUG("srcLen check failed. srcLen(%d) != %d \n", srcLen, sizeof(int));
                return ERR_NORMAL;
            }
            
            memcpy(&energys, srcData, srcLen);  

            if(energys == 0) //TODO 姑且使用0作为不存在的判据
            {
                dstBufTmp[offset++] = DT_NULL;
            }
            else
            {
                if(basic_data_to_buf(E_DOUBLE_LONG, dstBufTmp, dstLen-offset, &offset, &energys, sizeof(int)) < 0)
                {
                    nRet = ERR_NORMAL;
                    return nRet;
                }
            }
        }break; 
        case 0x40010200: //通信地址
        {
            OOP_OCTETVAR16_T addr;
            if(srcLen != sizeof(OOP_OCTETVAR16_T))
            {
                PRTL_FMT_DEBUG("srcLen check failed. srcLen(%d) != %d \n", srcLen, sizeof(OOP_OCTETVAR16_T));
                return ERR_NORMAL;
            }
            memcpy(&addr,srcData, srcLen);
            if(addr.nNum == 0)
            {
                dstBufTmp[offset++] = DT_NULL;
            }else
            {
                if(basic_data_to_buf(E_OOP_OCTETVAR16_T, dstBufTmp, dstLen-offset, &offset, &addr, sizeof(OOP_OCTETVAR16_T)) < 0)
                {
                    nRet = ERR_NORMAL;
                    return nRet;
                }
            }
        }
            break;
        case 0x28000200://开关状态
        case 0x2b010200://烟雾告警状态
        case 0x2b020200://水浸状态
        {
            if(srcLen != sizeof(uint8))
            {
                PRTL_FMT_DEBUG("srcLen check failed. srcLen(%d) != %d \n", srcLen, sizeof(int));
                return ERR_NORMAL;
            }
            uint8 state ;
            memcpy(&state,srcData, srcLen);
            if(state == 0)
            {
                dstBufTmp[offset++] = DT_NULL;
            }else
            {
                if(basic_data_to_buf(E_ENUM, dstBufTmp, dstLen-offset, &offset, &state, sizeof(uint8)) < 0)
                {
                    nRet = ERR_NORMAL;
                    return nRet;
                }
 
            }
        }
            break;
        case 0x20012201:   //事件发生前电流
        case 0x20012202:   //事件发生前电流
        case 0x20012203:   //事件发生前电流
        case 0x20014201:   //事件发生后电流
        case 0x20014202:   //事件发生后电流
        case 0x20014203:   //事件发生后电流
        case 0x20016201:   //事件结束前电流
        case 0x20016202:   //事件结束前电流
        case 0x20016203:   //事件结束前电流
        case 0x20018201:   //事件结束后电流
        case 0x20018202:   //事件结束后电流
        case 0x20018203:   //事件结束后电流
        {
                if(basic_data_to_buf(E_DOUBLE_LONG, dstBufTmp, dstLen-offset, &offset, srcData, sizeof(int32)) < 0)
                {
                    nRet = ERR_NORMAL;
                    return nRet;
                }
        }break;  
        case 0x20010200:    //电流
        case 0x20012200:    //事件发生前电流     
        case 0x20014200:    //事件发生后电流        
        {
            OOP_INT3V_T value; 
            if(srcLen != sizeof(OOP_INT3V_T))
            {
                PRTL_FMT_DEBUG("srcLen check failed. srcLen(%d) != %d \n", srcLen, sizeof(int));
                return ERR_NORMAL;
            }

            memcpy(&value, srcData, srcLen);
    
            dstBufTmp[offset++] = DT_ARRAY;
    
            //数组成员个数
            dstBufTmp[offset++] = value.nNum;
    
            for(i = 0; i < value.nNum; i++)
            {
                if(0==get_bit_value((uint8*)&value.rsv,32,i))
                {
                    dstBufTmp[offset++] = DT_NULL;
                    continue;
                }
                            
                if(basic_data_to_buf(E_DOUBLE_LONG, dstBufTmp, dstLen-offset, &offset, &value.nValue[i], sizeof(int32)) < 0)
                {
                    nRet = ERR_NORMAL;
                    return nRet;
                }
            }
        }break;  

        case 0x20010500:    //剩余电流
        case 0x20012500:    //事件发生前的剩余电流
        {
            int value;
            
            if(srcLen != sizeof(int))
            {
                PRTL_FMT_DEBUG("srcLen check failed. srcLen(%d) != %d \n", srcLen, sizeof(int));
                return ERR_NORMAL;
            }
            
            memcpy(&value, srcData, srcLen);  

            if(basic_data_to_buf(E_DOUBLE_LONG, dstBufTmp, dstLen, &offset, &value, sizeof(int)) < 0)
            {
                nRet = ERR_NORMAL;
                return nRet;
            }
        }break; 
            
        case 0x26000200:    //环境温度
        {
            OOP_LONG3V_T value; 
            if(srcLen != sizeof(OOP_LONG3V_T))
            {
                PRTL_FMT_DEBUG("srcLen check failed. srcLen(%d) != %d \n", srcLen, sizeof(OOP_LONG3V_T));
                return ERR_NORMAL;
            }

            memcpy(&value, srcData, srcLen);
    
            dstBufTmp[offset++] = DT_ARRAY;
    
            //数组成员个数
            dstBufTmp[offset++] = value.nNum;
    
            for(i = 0; i < value.nNum; i++)
            {
                if(0==get_bit_value((uint8*)&value.rsv,32,i))
                {
                    dstBufTmp[offset++] = DT_NULL;
                    continue;
                }
                            
                if(basic_data_to_buf(E_LONG, dstBufTmp, dstLen-offset, &offset, &value.nValue[i], sizeof(int16)) < 0)
                {
                    nRet = ERR_NORMAL;
                    return nRet;
                }
            }
        }break;  
        default: nRet = ERR_NORMAL; break;
    }

    *pOffset = offset;
    
    return ERR_OK;
}

/**
*********************************************************************
* @brief：     从数据中心的数据转698格式报文(class3)
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/

    /*入参检查*/

    
    
    

        



/**
*********************************************************************
* @brief：     从数据中心的数据转698格式报文(class3)
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_class3_oop_temp(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *dsdata)
{
    int32 nRet = ERR_OK;
    const OAD_INFO_T *pOadInfoTmp = NULL;
    uint32     offset = *dsdata->dOffset;
    uint8      offlen = 0;
    uint8      lenBuf[3] = {0};
    uint32     i = 0;
    pOadInfoTmp = pOadInfo;
    uint8 *srcDataTmp = (uint8 *)dsdata->srcBuf;
    uint8 *dstBufTmp = (uint8 *)dsdata->dstBuf;
    OOP_LONG3V_T data;

    /*入参检查*/
    if ((pOadInfoTmp == NULL) || (dsdata->srcBuf == NULL) || (dsdata->dstBuf == NULL) || (dsdata->sOffset == NULL))
    {
        PRTL_FMT_DEBUG("data_to_buf_class2 error, ERR_PTR\n");
        return ERR_PTR;
    }

    MEMZERO(&data, sizeof(data));
    
    if (dsdata->srcLen != sizeof(OOP_LONG3V_T))
    {
        PRTL_FMT_DEBUG("data_to_buf_class2 error, ERR_OPTION\n");
        return ERR_OPTION;
    }
    
    memcpy(&data, srcDataTmp, dsdata->srcLen);
    offlen = set_len_offset(3, lenBuf);
    dstBufTmp[offset] = pOadInfoTmp->pTab->dataType;
    offset ++;
    
    memcpy(&dstBufTmp[offset], lenBuf, offlen); 
    offset += offlen;

    for (i = 0; i < 3; i++)
    {
        if(0==get_bit_value((uint8*)&data.rsv,32,i))
        {
            dstBufTmp[offset++] = DT_NULL;
            continue;
        }
        
        if(basic_data_to_buf(E_LONG, dstBufTmp, dsdata->dstLen-offset, &offset, &data.nValue[i], sizeof(int32)) < 0)
        {
            nRet = ERR_NORMAL;
            return nRet;
        }
    }

    *dsdata->dOffset = offset;

    return nRet;
}

/**
*********************************************************************
* @brief：      温度越限告警事件
*               
* @param[in]:   srcBuf     源数据区698报文
*               srcLen     源数据长度
*               dstLen     目的长度，通常是变量的长度
*
* @param[out]： pOffset     解出该数据后的源数据区偏移
*               dstData    目的数据区，通常是变量的地址
* @return：      =0            - 成功
*               <0            - 错误码
*********************************************************************
*/
int32 data_to_buf_evt_cfg3361(const OAD_INFO_T *pOdaInfo, void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen)
{
    int32 nRet = ERR_NORMAL;
    uint32  offset = *pOffset;
    OOP_TemperatureWarningParam_T evtcfg;
    uint8 *dstBufTmp = (uint8 *)dstBuf;
    
    memcpy(&evtcfg, srcData, srcLen);

    //结构类型
    dstBufTmp[offset++] = DT_STRUCTURE;
    
    //结构成员数
    dstBufTmp[offset++] = 2;

    //温度下限
    if(basic_data_to_buf(E_LONG, dstBufTmp, dstLen-offset, &offset, &evtcfg.LowerLimit, sizeof(uint32)) < 0)
    {
        nRet = ERR_NORMAL;
        return nRet;
    }

    //温度上限
    if(basic_data_to_buf(E_LONG, dstBufTmp, dstLen-offset, &offset, &evtcfg.UpperLimit, sizeof(uint32)) < 0)
    {
        nRet = ERR_NORMAL;
        return nRet;
    }

    *pOffset = offset;
     
    return ERR_OK;
}

/**
*********************************************************************
* @brief：      阻抗越限告警事件
*               
* @param[in]:   srcBuf     源数据区698报文
*               srcLen     源数据长度
*               dstLen     目的长度，通常是变量的长度
*
* @param[out]： pOffset     解出该数据后的源数据区偏移
*               dstData    目的数据区，通常是变量的地址
* @return：      =0            - 成功
*               <0            - 错误码
*********************************************************************
*/
int32 data_to_buf_evt_cfg31f2(const OAD_INFO_T *pOdaInfo, void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen)
{
    int32 nRet = ERR_NORMAL;
    uint32  offset = *pOffset;
    OOP_EVTCFG_31F2_T evtcfg;
    uint8 *dstBufTmp = (uint8 *)dstBuf;
    
    memcpy(&evtcfg, srcData, srcLen);

    //结构类型
    dstBufTmp[offset++] = DT_STRUCTURE;
    
    //结构成员数
    dstBufTmp[offset++] = 1;

    //阻抗越限值
    if(basic_data_to_buf(E_DOUBLE_LONG_UNSIGNED, dstBufTmp, dstLen-offset, &offset, &evtcfg.impeLimit, sizeof(uint32)) < 0)
    {
        nRet = ERR_NORMAL;
        return nRet;
    }

    *pOffset = offset;
     
    return ERR_OK;
}

/**
*********************************************************************
* @brief：      冻结关联对象属性表
* @param[in]： ppOadInfo     - 数据表指针
               srcBuf       - 源数据
               srcLen       - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen       - 目的数据长度
               sOffset      - 转换后源数据区的偏移
               dOffset      - 转换后目的数据区的偏移
* @return：     =0           - 成功
              !=0           - 错误码
* @author    : 贺宁
* @Date      ：2019-12-6                 
*********************************************************************
*/
OOP_DAR_E data_to_buf_node_topology(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_NodeTopologyInfo_T data;
    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    memcpy(&data, &srcBuf[sOffset], sizeof(data));
    sOffset += sizeof(data);

    //数组类型

    //数组成员个数

        //结构类型
        dstBuf[dOffset++] = DT_STRUCTURE;

        //结构成员数
        dstBuf[dOffset++] = 4;
                
        //本节点设备地址
    dar = basic_data_to_buf(E_TSA, dstBuf, dstLen, &dOffset, &data.NodeDevAddr, sizeof(OOP_TSA_T));
        if(dar != DATA_SUCCESS)
        {
        PRTL_BUF_DEBUG(data.NodeDevAddr.add, OOP_TSA_SIZE-1, "basic_data_to_buf failed. dar(%d)\n",dar);
            return DATA_TYPE_UNMATCHED;
        }
        
        //本节点相位信息
    dar = basic_data_to_buf(E_ENUM, dstBuf, dstLen, &dOffset, &data.NodePhaseInfo, sizeof(uint8));
        if(dar != DATA_SUCCESS)
        {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d),oad(0x%08x)\n",dar, data.NodePhaseInfo);
            return DATA_TYPE_UNMATCHED;
        }
        //父节点设备地址
    dar = basic_data_to_buf(E_TSA, dstBuf, dstLen, &dOffset, &data.ParentNodeDevAddr, sizeof(OOP_TSA_T));
        if(dar != DATA_SUCCESS)
        {
        PRTL_BUF_DEBUG(data.ParentNodeDevAddr.add, OOP_TSA_SIZE-1, "basic_data_to_buf failed. dar(%d)\n", dar);
            return DATA_TYPE_UNMATCHED;
        }   

        //父节点线路计量地址
    dar = basic_data_to_buf(E_TSA, dstBuf, dstLen, &dOffset, &data.ParentNodeLineAddr, sizeof(OOP_TSA_T));
        if(dar != DATA_SUCCESS)
        {
        PRTL_BUF_DEBUG(data.ParentNodeLineAddr.add, OOP_TSA_SIZE-1, "basic_data_to_buf failed. dar(%d)\n", dar);
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
* @brief：       线路阻抗值
* @param[in]:   srcBuf     源数据区698报文
*               srcLen     源数据长度
*               dstLen     目的长度，通常是变量的长度
*
* @param[out] ：pOffset     解出该数据后的源数据区偏移
*               dstData    目的数据区，通常是变量的地址
* @return：      =0            - 成功
*               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_evt_impe(const OAD_INFO_T *pOdaInfo, void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  offset = *pOffset;
    OOP_DWORD3V_T data;
    uint8 *dstBufTmp = (uint8 *)dstBuf;
    uint32 i = 0;
    
    memcpy(&data, srcData, srcLen);

    //数组类型
    dstBufTmp[offset++] = DT_ARRAY;
    
    //数据成员数
    dstBufTmp[offset++] = 3;

    for(i = 0; i < 3; i++)
    {
        dar = basic_data_to_buf(E_DOUBLE_LONG_UNSIGNED, dstBufTmp, dstLen, &offset, &data.nValue[i], sizeof(uint32));
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("basic_data_to_buf failed. [%d] dar(%d), data(%d)\n", i, dar, data.nValue[i]);
            return DATA_TYPE_UNMATCHED;
        }
    }
    
    *pOffset = offset;
     
    return dar;
}

/**
*********************************************************************
* @brief：      温度越限告警事件 
*             
* @param[in]:   srcBuf     源数据区698报文
*               srcLen     源数据长度
*               dstLen     目的长度，通常是变量的长度
*
* @param[out]： pOffset     解出该数据后的源数据区偏移
*               dstData    目的数据区，通常是变量的地址
* @return：      =0            - 成功
*               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E buf_to_data_evt_cfg3361(const OAD_INFO_T *pOadInfo, const void *srcBuf, uint32 srcLen, uint32 *pOffset, void *dstData, uint32 dstLen)
{
    OOP_DAR_E  dar = DATA_SUCCESS; 
    uint32  offset = *pOffset;
    OOP_TemperatureWarningParam_T evtcfg;
    uint8 *srcBufTmp = (uint8 *)srcBuf;

     //结构类型
     if(srcBufTmp[offset++] != DT_STRUCTURE)
     {
         PRTL_BUF_DEBUG(srcBufTmp, srcLen, "check struct type failed. offset(%d)", offset);
         return DATA_TYPE_UNMATCHED;
     }
    
     //结构成员数
     if(srcBufTmp[offset++] != 2)
     {
         PRTL_BUF_DEBUG(srcBufTmp, srcLen, "check struct num failed. offset(%d)", offset);
         return DATA_TYPE_UNMATCHED;
     }

    //温度下限
    dar = basic_buf_to_data(E_LONG, srcBufTmp, srcLen-offset, &offset, &evtcfg.LowerLimit, sizeof(int16));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBufTmp, srcLen, "basic_buf_to_data failed. dar(%d), offset(%d)\n", dar, offset);
        return DATA_TYPE_UNMATCHED;
    }

    //温度上限
    dar = basic_buf_to_data(E_LONG, srcBufTmp, srcLen-offset, &offset, &evtcfg.UpperLimit, sizeof(int16));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBufTmp, srcLen, "basic_buf_to_data failed. dar(%d), offset(%d)\n", dar, offset);
        return DATA_TYPE_UNMATCHED;
    }    

    memcpy(dstData, &evtcfg, dstLen);
    *pOffset = offset;
     
    return dar;
}

/**
*********************************************************************
* @brief：      阻抗越限告警事件 
*             
* @param[in]:   srcBuf     源数据区698报文
*               srcLen     源数据长度
*               dstLen     目的长度，通常是变量的长度
*
* @param[out]： pOffset     解出该数据后的源数据区偏移
*               dstData    目的数据区，通常是变量的地址
* @return：      =0            - 成功
*               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E buf_to_data_evt_cfg31f2(const OAD_INFO_T *pOadInfo, const void *srcBuf, uint32 srcLen, uint32 *pOffset, void *dstData, uint32 dstLen)
{
    OOP_DAR_E  dar = DATA_SUCCESS; 
    uint32  offset = *pOffset;
    OOP_EVTCFG_31F2_T evtcfg;
    uint8 *srcBufTmp = (uint8 *)srcBuf;

    //结构类型
    if(srcBufTmp[offset++] != DT_STRUCTURE)
    {
        PRTL_BUF_DEBUG(srcBufTmp, srcLen, "check struct type failed. offset(%d)", offset);
        return DATA_TYPE_UNMATCHED;
    }

    //结构成员数
    if(srcBufTmp[offset++] != 1)
    {
        PRTL_BUF_DEBUG(srcBufTmp, srcLen, "check struct num failed. offset(%d)", offset);
        return DATA_TYPE_UNMATCHED;
    }

    //阻抗超限值
    dar = basic_buf_to_data(E_DOUBLE_LONG_UNSIGNED, srcBufTmp, srcLen-offset, &offset, &evtcfg.impeLimit, sizeof(uint32));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBufTmp, srcLen, "basic_buf_to_data failed. dar(%d), offset(%d)\n", dar, offset);
        return DATA_TYPE_UNMATCHED;
    }

    memcpy(dstData, &evtcfg, dstLen);
    *pOffset = offset;

    return dar;
}

/**
*********************************************************************
* @name：       通讯流量超限事件配置参数
* @brief：      class7的SET服务
* @param[in] ：pOdaInfo        OAD信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
* @return    ：输出报文实际长度
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E evt_cfg3361_set(const OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 inLen, uint32 *offset)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    NOMAL_OAD_T normal = {0};
    OOP_TemperatureWarningParam_T evtcfg;

    //从报文中提取数据
    dar = buf_to_data_evt_cfg3361(pOadInfo, inData, inLen, offset, &evtcfg, sizeof(OOP_TemperatureWarningParam_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("buf_to_data_evt_cfg3110 failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    //写入数据
    normal.oad.value = pOadInfo->pTab->oad.value;
    normal.classtag = CLASS_DATA_INIT;

    ret = db_write_nomal(pApduInfo->hUdp, &normal, (uint8*)&evtcfg, sizeof(OOP_TemperatureWarningParam_T));
    if(ret != ERR_OK)
    {
        PRTL_FMT_DEBUG("evt_cfg3361 db_write_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
            ret, normal.logicId, normal.infoNum, normal.oad.value);
        return DATA_OTHER_REASON;
    }

    return dar;
}

/**
*********************************************************************
* @name：       阻抗超限事件配置参数
* @brief：      class7的SET服务
* @param[in] ：pOdaInfo        OAD信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
* @return    ：输出报文实际长度
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E evt_cfg31f2_set(const OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 inLen, uint32 *offset)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    NOMAL_OAD_T normal = {0};
    OOP_EVTCFG_31F2_T evtcfg;

    //从报文中提取数据
    dar = buf_to_data_evt_cfg31f2(pOadInfo, inData, inLen, offset, &evtcfg, sizeof(OOP_EVTCFG_31F2_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("buf_to_data_evt_cfg3110 failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    //写入数据
    normal.oad.value = pOadInfo->pTab->oad.value;
    normal.classtag = CLASS_DATA_INIT;

    ret = db_write_nomal(pApduInfo->hUdp, &normal, (uint8*)&evtcfg, sizeof(OOP_EVTCFG_31F2_T));
    if(ret != ERR_OK)
    {
        PRTL_FMT_DEBUG("evt_cfg3361 db_write_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
            ret, normal.logicId, normal.infoNum, normal.oad.value);
        return DATA_OTHER_REASON;
    }

    return dar;
}

/**
*********************************************************************
* @name：       温度超限告警事件配置参数
* @brief：      class7的SET服务
* @param[in] ：pOdaInfo        OAD信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
* @return    ：输出报文实际长度
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E evt_cfg3361_get(const OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;    
    NOMAL_OAD_T normal = {0};   
    OOP_TemperatureWarningParam_T evtcfg;
    uint32 len = 0;
    uint32 offset = 0;

    normal.oad.value = pOadInfo->pTab->oad.value;
    normal.logicId = 0;
    normal.infoNum = pOadInfo->pTab->oad.nIndex;

    memset(&evtcfg, 0, sizeof(OOP_EVTCFG_3110_T));

    ret = db_read_nomal(pApduInfo->hUdp, &normal, sizeof(OOP_TemperatureWarningParam_T), (uint8*)&evtcfg, &len);
    if(ret != ERR_OK)
    {
        PRTL_FMT_DEBUG("db_read_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
            ret, normal.logicId, normal.infoNum, normal.oad.value);
        return oop_null_data_res(pOadInfo,  pApduInfo, inData, InDataLen, outData, outDataMax);
    }

    memcpy(&outData->pResultData[offset], inData, sizeof(OOP_OAD_U));
    offset += sizeof(OOP_OAD_U);

    outData->pResultData[offset++] = 1;

    dar = data_to_buf_evt_cfg3361(pOadInfo, outData->pResultData, outDataMax, &offset, &evtcfg, len);
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("data_to_buf_evt_asobjattr failed. dar(%d), oad(0x%08x)", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    outData->resultDataLen = offset;

    return dar;
}

/**
*********************************************************************
* @name：       阻抗超限告警事件配置参数
* @brief：      class7的SET服务
* @param[in] ：pOdaInfo        OAD信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
* @return    ：输出报文实际长度
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E evt_cfg31f2_get(const OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;    
    NOMAL_OAD_T normal = {0};   
    OOP_EVTCFG_31F2_T evtcfg;
    uint32 len = 0;
    uint32 offset = 0;

    normal.oad.value = pOadInfo->pTab->oad.value;
    normal.logicId = 0;
    normal.infoNum = pOadInfo->pTab->oad.nIndex;

    memset(&evtcfg, 0, sizeof(OOP_EVTCFG_31F2_T));

    ret = db_read_nomal(pApduInfo->hUdp, &normal, sizeof(OOP_EVTCFG_31F2_T), (uint8*)&evtcfg, &len);
    if(ret != ERR_OK)
    {
        PRTL_FMT_DEBUG("db_read_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
            ret, normal.logicId, normal.infoNum, normal.oad.value);
        return oop_null_data_res(pOadInfo,  pApduInfo, inData, InDataLen, outData, outDataMax);
    }

    memcpy(&outData->pResultData[offset], inData, sizeof(OOP_OAD_U));
    offset += sizeof(OOP_OAD_U);

    outData->pResultData[offset++] = 1;

    dar = data_to_buf_evt_cfg31f2(pOadInfo, outData->pResultData, outDataMax, &offset, &evtcfg, len);
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("data_to_buf_evt_asobjattr failed. dar(%d), oad(0x%08x)", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    outData->resultDataLen = offset;

    return dar;
}

/**
*********************************************************************
* @name：       class11_NodeTopology_get
* @brief：      获取节点拓扑信息
* @param[in] ：pOdaInfo        OAD信息
               inData         输入报文
               inLen          输入报文长度
* @return    ：输出报文实际长度
* @author    : 贺宁
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E class11_nodetopology_get(const OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    DATA_CONVERT_T convert;
    uint32 sOffset = 0;
    uint32 dOffset = 0;
    uint32 offset  = 0;
    uint32             i = 0;
    READ_RECORD_T      inReadRecord;
    uint32             handle = 0;
    uint32             recNum = 0;
    STEP_READ_RECORD_T stepReadRecord;
    RECORD_UNIT_T      stepRecordOut;
    OOP_NodeTopologyInfo_T data;
    uint32    realNum = 0;    //数组由于分帧导致的每帧实际成员数
    uint32    numOffset = 0;  //数组成员数在报文中的位置
    uint32    bufAllLen = 0;
    BP_INFO_T breakInfo = {0};                                                //断点信息
    uint32    maxBufLen = 0;
    BOOLEAN   isBufFull = FALSE;

    maxBufLen = pApduInfo->connectionInfo.ApplSplitLen - cal_excludeLen(pApduInfo->connectionInfo);
    memset(&breakInfo, 0, sizeof(BP_INFO_T));
    memset(&data, 0, sizeof(data));
    MEMZERO(&convert, sizeof(convert)); 

    memset(&inReadRecord, 0, sizeof(READ_RECORD_T));
    memset(&stepReadRecord, 0, sizeof(STEP_READ_RECORD_T));
    memset(&stepRecordOut, 0, sizeof(RECORD_UNIT_T));
    //读出数据
    PRTL_FMT_DEBUG("read node topology start\n");
    inReadRecord.recordOAD.logicId = 0;

    if(pOadInfo->nIndex == 0)
    {
        inReadRecord.recordOAD.infoNum = 0xffff;
    }
    else
    {
        inReadRecord.recordOAD.infoNum = pOadInfo->nIndex;
    }
    
    inReadRecord.cType = COL_NO_INFO;
    inReadRecord.cStart = 0;
    inReadRecord.cEnd = NO_VIR_MTR_MAX_NUM;
    inReadRecord.sortType = UP_SORT;
    inReadRecord.recordOAD.optional = 0;
    inReadRecord.recordOAD.road.oadMain.value = 0;
    inReadRecord.recordOAD.road.oadCols.nNum = 0x01;
    inReadRecord.recordOAD.road.oadCols.oad[0].value = pOadInfo->pTab->oad.value;
    stepReadRecord.MAC.nNum = 0;
    if(pApduInfo->bpBreakInfo->appSlicingFlag == 1)
    {
        inReadRecord.cStart = pApduInfo->bpBreakInfo->tmStartSeg;    
    }
    memcpy(&outData->pResultData[offset], inData, sizeof(OOP_OAD_U));   //拷贝OAD
    offset += sizeof(OOP_OAD_U);
    outData->pResultData[offset ++] = 0x01;                             //结果
    if(pOadInfo->nIndex == 0)
    {
        outData->pResultData[offset ++] = DT_ARRAY;                         //类型
        numOffset = offset;                                                 //数量偏移
        outData->pResultData[offset ++] = 0x00;                             //数量，先填0
    }
    bufAllLen = offset;
    ret = db_read_record_start(pApduInfo->hUdp, &inReadRecord, &handle, &recNum);
    if ((ret != ERR_OK) || (recNum == 0))
    {
        PRTL_FMT_DEBUG("class11_nodetopology_get db_read_record_start err! ret = %d, infoNum = %d recNum = %d\n", 
            ret, inReadRecord.recordOAD.infoNum, recNum);
    }
    else
    {
        PRTL_FMT_DEBUG("class11_nodetopology_get db_read_record_start ok ret = %d, infoNum = %d recNum = %d\n", 
            ret, inReadRecord.recordOAD.infoNum, recNum);
        
        for (i = 0; i < recNum; i++)
        {
            ret = db_read_record_step(pApduInfo->hUdp, handle, &stepReadRecord, &stepRecordOut);
            if(ret != ERR_OK)
            {
                PRTL_FMT_DEBUG("class11_nodetopology_get db_read_record_step err = %d!\n", ret);
                continue;
                //数据中心出错返回NULL
            }

            //OAD与返回数据
            if ((stepRecordOut.recordnum != 1) 
            || (stepRecordOut.record[0].road.oadCols.oad[0].value != pOadInfo->pTab->oad.value)
            || (stepRecordOut.record[0].outDatalen != sizeof(OOP_NodeTopologyInfo_T)))
            {
                PRTL_FMT_DEBUG("class11_nodetopology_get data err !\n");
                PRTL_FMT_DEBUG("class11_nodetopology_get stepRecordOut.recordnum = %d\n", stepRecordOut.recordnum);
                PRTL_FMT_DEBUG("class11_nodetopology_get stepRecordOut.record[0].road.oadCols.oad[0] = 0x%08x\n", 
                    stepRecordOut.record[0].road.oadCols.oad[0].value);
                PRTL_FMT_DEBUG("class11_nodetopology_get stepRecordOut.record[0].outDatalen = %d\n", 
                    stepRecordOut.record[0].outDatalen);
                PRTL_FMT_DEBUG("class11_nodetopology_get sizeof(OOP_METER_T) = %d\n", 
                    sizeof(OOP_METER_T));
                continue;
            }

            memcpy(&data, stepRecordOut.record[0].outDataBuf, stepRecordOut.record[0].outDatalen);
            PRTL_FMT_DEBUG("stepRecordOut.infoNum = %d\n", stepRecordOut.infoNum);

            //转换成报文数据
            convert.srcBuf = &data;
            convert.srcLen = sizeof(data);
            convert.sOffset = &sOffset;

            convert.dstBuf = &outData->pResultData[offset];
            convert.dstLen = outDataMax - offset;
            convert.dOffset = &dOffset;

            breakInfo.tmStartSeg = stepRecordOut.infoNum;
            dar = data_to_buf_node_topology(pOadInfo, &convert);
            if(dar == DATA_SUCCESS)
            {                  
                bufAllLen += dOffset;
                if (bufAllLen <= maxBufLen)
                {
                    memcpy(&outData->pResultData[offset], convert.dstBuf, dOffset);
                    offset += dOffset;
                    outData->resultDataLen = offset;
                    PRTL_FMT_DEBUG(convert.dstBuf, dOffset, "第%d个物理节点:", i);
                    realNum ++;
                    dOffset = 0;
                    sOffset = 0;
                }
                else
                {
                    dOffset = 0;
                    sOffset = 0;
                    isBufFull = TRUE;
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
    }
    db_read_record_end(pApduInfo->hUdp, handle);
    PRTL_FMT_DEBUG("read node topology end.\n");
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
    memcpy(pApduInfo->bpBreakInfo, &breakInfo, sizeof(BP_INFO_T));

    if(pOadInfo->nIndex == 0)
    {
        outData->pResultData[numOffset] = realNum;   //重新记数量
    }

    outData->resultDataLen = offset;

    return dar;
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
OOP_DAR_E data_to_buf_impeover(const OAD_INFO_T *pOadInfo, void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen)
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

    if (pOadInfo->pTab->eData != E_OOP_IMPE_OVER_T)
    {
        PRTL_FMT_DEBUG("check data type failed.\n");
        return DATA_TYPE_UNMATCHED;
    }

    OOP_IMPE_OVER_T info;
    memset(&info,0x00,sizeof(OOP_IMPE_OVER_T));

    if (srcLen != sizeof(OOP_IMPE_OVER_T))
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

    //事件发生前阻抗值
    dar = data_to_buf_evt_impe(pOadInfo, dstBufTmp, dstLen-offset, &offset, &info.impeBef, sizeof(OOP_DWORD3V_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), offset(%d)\n", dar, offset);
        return DATA_TYPE_UNMATCHED;
    }
    
    //事件发生后阻抗值
    dar = data_to_buf_evt_impe(pOadInfo, dstBufTmp, dstLen-offset, &offset, &info.impeAft, sizeof(OOP_DWORD3V_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), offset(%d)\n", dar, offset);
        return DATA_TYPE_UNMATCHED;
    }

    //事件发生时相位
    dar = basic_data_to_buf(E_ENUM, dstBufTmp, dstLen-offset, &offset, &info.curPhase, sizeof(uint8));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), offset(%d)\n", dar, offset);
        return DATA_TYPE_UNMATCHED;
    }

    //关联对象属性数据
    dar = data_to_buf_evt_coload(pOadInfo, dstBufTmp, dstLen-offset, &offset, &info.data, sizeof(info.data));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("data_to_buf_evt_coload failed. dar(%d), offset(%d)\n", dar, offset);
        return DATA_TYPE_UNMATCHED;
    }

    *pOffset = offset;

    return dar;
}

int event_tmn_impeover(OAD_INFO_T *pOadInfo, RECORD_UNIT_T unit, RELY_INFO_T *relyInfo, uint8 **ppOutBuf, uint32 *outLen)
{
    int32 nRet = ERR_OK;
    OOP_IMPE_OVER_T event;    
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

        nRet = data_to_buf_impeover(pOadInfo, pOutBuf, DATA_LEN_MAX, &offset, &event, sizeof(event));
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

            case 0x20a12200:   //事件发生前阻抗值
            {
                if(data_to_buf_evt_impe(&pstOdaInfo, pOutBuf, tmpOutLen-offset, &offset, &event.impeBef, sizeof(OOP_DWORD3V_T)) < 0)
                {
                    nRet = ERR_NORMAL;
                    return nRet;
                } 
            }break;

            case 0x20a14200:   //事件发生后阻抗值
            {
                if(data_to_buf_evt_impe(&pstOdaInfo, pOutBuf, tmpOutLen-offset, &offset, &event.impeAft, sizeof(OOP_DWORD3V_T)) < 0)
                {
                    nRet = ERR_NORMAL;
                    return nRet;
                } 
            }break;    

             case 0x33f20208:   //事件发生时相位
            {
                if(basic_data_to_buf(E_ENUM, pOutBuf, tmpOutLen-offset, &offset, &event.curPhase, sizeof(uint8)) < 0)
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

/**********************************************************************
* @name      : start_topology_to_pdsManager
* @brief     ：向台区状态管理APP发送启动拓扑
* @param[in] ：
* @return    ：
* @Date      ：2020-3-18
* @Update    :
**********************************************************************/
int start_topology_to_pdsManager(uint16 time)
{
    uint8  sendbuf[100] = {0};
    uint16 offset = 0;

    sendbuf[offset++] = (uint8)((time >> 8) &0xff);
    sendbuf[offset++] = (uint8)(time & 0xff);

    return advance_send_pdsManager(0, PDS_IOP_TOPOLOGY_START, sendbuf, offset);
}

/**
*********************************************************************
* @name：      class11_invoke_act_topology
* @brief：     启动拓扑
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
OOP_DAR_E class11_invoke_act_topology(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    uint8 data[BUF_LEN_MAX] = {0};
    DATA_CONVERT_T convert;
    uint32 sOffset = 4;
    uint32 dOffset = 0;
    uint32 offset  = 0;

    MEMZERO(&data, sizeof(data)); 
    MEMZERO(&convert, sizeof(convert)); 

    convert.srcBuf = (void *)inData;
    convert.srcLen = InDataLen;
    convert.sOffset = &sOffset;

    convert.dstBuf = &data;
    convert.dstLen = sizeof(data);
    convert.dOffset = &dOffset;  

    //从报文中提取数据
    dar = buf_to_data_class11_act(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(&inData[sOffset], InDataLen-sOffset, "buf_to_data_class11_set failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    //写入数据
    switch (pOadInfo->pTab->oad.value)
    {
    case 0x60F17F00:
        {
            uint16 data = 0;
            memcpy(&data, convert.dstBuf, dOffset);
            PRTL_FMT_LOGCC("class11_invoke_act_searchmeter_start : data = %d\n", data);

            ret = start_topology_to_pdsManager(data);
            if (ret != ERR_OK)
            {
                PRTL_FMT_DEBUG("start_topology_to_pdsManager err\n");
                return FALSE;
            }
            PRTL_FMT_DEBUG("node topology to taskManager ok\n");
        }
        break;
    
    default:
        return DATA_OBJECT_UNAVAILABLE;
        break;
    }
    
    memcpy(&outData->pResultData[offset], inData, sizeof(OOP_OAD_U));  //OMD
    offset += sizeof(OOP_OAD_U);
    outData->pResultData[offset ++] = 0x00;                            //DAR type
    outData->pResultData[offset ++] = 0x00;                            //DATA:NULL
    outData->resultDataLen = offset;

    return dar;
}

/**********************************************************************
* @name      : get_power_state
* @brief     ：获取电源状态
* @param[in] ：  
* @param[out]：state: 电源状态，1有电，0 掉电
* @return    ：0 成功， <0 失败
* @Create    : 贺宁
* @Date      ：2020-07-27
* @Update    :
**********************************************************************/
int get_tmn_power_state(int *state)
{
    int ret = ERR_OK;
    
    POWER_DEVICE_T* dev = (POWER_DEVICE_T*) hal_device_get(HW_DEVICE_ID_POWER);
    if(NULL == dev)
    {
        PRTL_FMT_DEBUG("dev is null!\n");
        return ERR_PNULL;
    }

    ret |= dev->power_power_state_get(dev,POWER_MAIN,state);
    ret |= dev->power_power_state_get(dev,POWER_MAIN,state);
    ret |= dev->power_power_state_get(dev,POWER_MAIN,state);
    if(ret < 0)
    {
        PRTL_FMT_DEBUG("get power state failed, ret(%d).\n", ret);
        return ret;
    }

    
    PRTL_FMT_TRACE("get power state %d! \n", *state);
    
    hal_device_release((HW_DEVICE *)dev);
    
    return ret;
}

/**
*********************************************************************
* @brief：      设置校时模式
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
OOP_DAR_E buf_to_data_class8_sensor_param(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_SENSOR_PARAM_T data;
    uint8 i = 0;
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
    
    //端口
    dar = basic_buf_to_data(E_OAD, srcBuf, srcLen, &sOffset, &data.port, sizeof(data.port));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }
    
    //地址
    dar = basic_buf_to_data(E_OOP_OCTETVAR16_T, srcBuf, srcLen, &sOffset, &data.addr, sizeof(data.addr));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
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
* @name：      class8_invoke_set_sensor_param
* @brief：     设置传感器参数
* @param[in] ：pOdaInfo        OAD信息
               pBpInfo        应用分帧断点信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
               outDataMax     输出报文缓冲区最大长度

* @param[out]：outData     输出报文
* @return    ：OOP_DAR_E
* @author    : 
* @Date      ：
*********************************************************************
*/
OOP_DAR_E class8_invoke_set_sensor_param(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint8 data[BUF_LEN_MAX] = {0};
    DATA_CONVERT_T convert;
    uint32 sOffset = 4;
    uint32 dOffset = 0;
    uint32 offset  = 0;
    NOMAL_OAD_T normal = {0};
    int ret;
    MEMZERO(&data, sizeof(data)); 
    MEMZERO(&convert, sizeof(convert)); 

    convert.srcBuf = (uint8*)inData;
    convert.srcLen = InDataLen;
    convert.sOffset = &sOffset;

    convert.dstBuf = &data;
    convert.dstLen = sizeof(data);
    convert.dOffset = &dOffset;  

    //从报文中提取数据
    dar = buf_to_data_class8_set(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(&inData[sOffset], InDataLen-sOffset, "buf_to_data_class8_set failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    normal.oad.value = pOadInfo->pTab->oad.value;

    //写入数据
    switch (pOadInfo->pTab->oad.value)
    {
        case 0x60E10200:   //温湿度传感器
        case 0x60E20200:   //烟雾传感器
        case 0x60E30200:   //水浸传感器
        case 0x60E40200:   //变压器桩头温度传感器
        {
            OOP_SENSOR_PARAM_T data;
            memcpy(&data, convert.dstBuf, dOffset);
            normal.classtag = CLASS_DATA_NORMAL;
            PRTL_FMT_LOGCC("class8_invoke_set_sensor_param : port 0x%08x addrlen %d  \n", data.port.value,data.addr.nNum);
            PRTL_BUF_LOGCC(data.addr.value, data.addr.nNum, "addr: ");

            ret = db_write_nomal(pApduInfo->hUdp, &normal, (uint8*)&data, sizeof(data));
            if(ret != ERR_OK)
            {
                PRTL_FMT_DEBUG("db_write_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                    ret, normal.logicId, normal.infoNum, normal.oad.value);
                return DATA_OTHER_REASON;
            }
        }
        break;
    
    default:
        return DATA_OBJECT_UNAVAILABLE;
        break;
    }
    
    memcpy(&outData->pResultData[offset], inData, sizeof(OOP_OAD_U));  //OMD
    offset += sizeof(OOP_OAD_U);
    outData->pResultData[offset ++] = 0x00;                            //DAR type
    outData->pResultData[offset ++] = 0x00;                            //DATA:NULL
    outData->resultDataLen = offset;

    return dar;
}

/**
*********************************************************************
* @name：      class8_invoke_set_oia_60
* @brief：     class8的SET服务
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
OOP_DAR_E class8_invoke_set_oia_60(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    switch (pOadInfo->pTab->oad.nObjID)
    {
        case 0x60E1:
        case 0x60E2:
        case 0x60E3:
        case 0x60E4:
        {
            switch (pOadInfo->pTab->oad.attID)
            {
            case 0x02:
                {
                    return class8_invoke_set_sensor_param(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
                }
                break;

            default:
                break;
            }
        }
        break;
   
    default:
        return DATA_OBJECT_UNAVAILABLE;
        break;
    }
    return DATA_OBJECT_UNAVAILABLE;
}
/**
*********************************************************************
* @name：      class8_invoke_get_time_precise
* @brief：     class8的GET服务
* @param[in] ：pOdaInfo       OAD信息
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
OOP_DAR_E class8_invoke_get_sensor_param(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    NOMAL_OAD_T normal = {0};
    DATA_CONVERT_T convert;
    uint32 sOffset = 0;
    uint32 offset  = 0;
    uint32 len = 0;  

    MEMZERO(&convert, sizeof(convert)); 

    //读出数据
    switch (pOadInfo->pTab->oad.value)
    {
        case 0x60E10200:   //温湿度传感器
        case 0x60E20200:   //烟雾传感器
        case 0x60E30200:   //水浸传感器
        case 0x60E40200:   //变压器桩头温度传感器
        {
            OOP_SENSOR_PARAM_T data ;
            MEMZERO(&data, sizeof(data));
            normal.oad.value = pOadInfo->pTab->oad.value;
            ret = db_read_nomal(pApduInfo->hUdp, &normal, pOadInfo->pTab->dataLen, (uint8*)&data, &len);
            if((ret != ERR_OK) || (len == 0))
            {
                PRTL_FMT_DEBUG("db_read_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                    ret, normal.logicId, normal.infoNum, normal.oad.value);
            }

            PRTL_FMT_LOGCC("class8_invoke_get_sensor_param  : port 0x%08x addrlen %d  \n", data.port.value,data.addr.nNum);
            PRTL_BUF_LOGCC(data.addr.value, data.addr.nNum, "addr: ");

            //转换成报文数据
            convert.srcBuf = &data;
            convert.srcLen = sizeof(data);
            convert.sOffset = &sOffset;
            
            convert.dstBuf = &outData->pResultData[5];
            convert.dstLen = outDataMax;
            convert.dOffset = &outData->resultDataLen;
        }
        break;
    
    default:
        break;
    }
    
    /* 数据和报文转换 */
    dar = data_to_buf_class8(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("data_to_buf_class8 failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    /* 填写报文开头处的OAD和数据结果0x01*/
    memcpy(outData->pResultData, inData, sizeof(OOP_OAD_U));
    offset += sizeof(OOP_OAD_U);
    outData->pResultData[offset ++] = 0x01;
    outData->resultDataLen += offset;

    return dar;
}
/**
*********************************************************************
* @name：      class8_invoke_get_oia_40
* @brief：     class8的GET服务
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
OOP_DAR_E class8_invoke_get_oia_60(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    switch (pOadInfo->pTab->oad.nObjID)
    {
        case 0x60E1:
        case 0x60E2:
        case 0x60E3:
        case 0x60E4:
        {
            switch (pOadInfo->pTab->oad.attID)
            {
            case 0x02:
                {
                    return class8_invoke_get_sensor_param(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
                }
                break;    
            default:
                break;
            }
        }
        break;


    default:
        return DATA_OBJECT_UNAVAILABLE;
    }
    return DATA_OBJECT_UNAVAILABLE;
}
/**
*********************************************************************
* @brief：      传感器参数
* @param[in] ： srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_class8_sensor_param(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_SENSOR_PARAM_T data ;
    MEMZERO(&data, sizeof(data));
    uint8 i = 0;
    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    memcpy(&data, &srcBuf[sOffset], sizeof(data));
    sOffset += sizeof(data);

    //结构类型
    dstBuf[dOffset++] = DT_STRUCTURE;

    //结构成员数
    dstBuf[dOffset++] = 2;
            
    //端口
    dar = basic_data_to_buf(E_OAD, dstBuf, dstLen, &dOffset, &data.port, sizeof(data.port));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. [%d] dar(%d),\n", i, dar);
        return DATA_TYPE_UNMATCHED;
    }
    
    //地址
    dar = basic_data_to_buf(E_OOP_OCTETVAR16_T, dstBuf, dstLen, &dOffset, &data.addr, sizeof(data.addr));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. [%d] dar(%d)\n", i, dar);
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
#endif
/**
*********************************************************************
* @brief：      字符串解析
* @param[in] ： src 源字符串
*             delim 分隔符

* @param[out] ：dest     分割后的字符串
*             num   分割后的个数
* @return：   
*********************************************************************
*/
void str_split(char *src,const char *delim,char dest[][100],int *num)
{
    char* p = src;
    char* tmp;
    int i =0;
    tmp = strsep(&p,delim);
    while(tmp!=NULL)
    {
        strcpy(dest[i],tmp);
        tmp = strsep(&p,delim);
        i++;
    }
    *num = i;
}


//将字符串形式的ip转换为数组 失败返回false
BOOL commfun_IP_unpack(char* ipin, uint8* ipout)
{
    if (ipin == NULL || strlen(ipin) == 0)
        return FALSE;
    char* p1[4];
    char czBuf[20];
    strcpy(czBuf, ipin);

    char* pTmp = p1[0] = czBuf;
    int nIndex = 1;
    while (*pTmp != '\0')
    {
        if (*pTmp == '.')
        {
            *pTmp = '\0';
            pTmp++;
            if (*pTmp == '\0')
            {
                break;
            }
            else
            {
                if (*pTmp > '9' || *pTmp < '0')
                    return FALSE;
                if (nIndex > 4)
                    return FALSE;
                p1[nIndex] = pTmp;
                nIndex++;
            }
        }
        else if (*pTmp > '9' || *pTmp < '0')
        {
            return FALSE;
        }
        pTmp++;
    }
    if (nIndex != 4)
    {
        return FALSE;
    }
    ipout[0] = atoi(p1[0]);
    ipout[1] = atoi(p1[1]);
    ipout[2] = atoi(p1[2]);
    ipout[3] = atoi(p1[3]);
    return TRUE;
}
/**
*********************************************************************
* @brief：      湖南解析收到的短信 
①远程硬件初始化功能，即终端收到指定短信命令后自动硬件初始化； 
命令格式：cssdl 0  (中间有空格) 
②TD-LTE / FDD-LTE/GPRS远程主站切换功能，即终端收到指定短信命令后，更改原终端的通讯参数，并重新连接主站，完成主站切换的功能。
命令格式：cssdl 1 IP 参数:端口参数:APN 参数 
命令举例：cssdl 1 001.000.000.001:4000:cssdl.hn(中间有空格) 
③CDMA远程主站切换功能，即终端收到指定短信命令后，更改原终端的 CDMA 通讯参数，并重新连接主站。
命令格式：cssdl 2 IP 参数:端口参数:虚拟网用户名：虚拟网密码 
命令举例：cssdl 2 001.000.000.001:4000:cs@dl.vpdn.hn:hn123456(中间有空格)

* @param[in] ： buf 收到的短信
            buflen 长度

* @param[out] ：
* @return：    =0            - 成功
               <0        - 错误码
*********************************************************************
*/
int sms_msg_unpack(COMM_SMS_HANDLE_T *ptHandle,uint8 *buf,uint16 buflen)
{
    const char* smshead = "cssdl ";
    uint8 offset = 0;
    char str[50][100];
    int ret;
    int num = 0;
    memset(str,0x00,sizeof(str));
    uint8 ip[4] = {0};
    uint16 port;
    uint32 len;
    OOP_MASTERPARAMS_T masterparam;
    memset(&masterparam,0x00,sizeof(OOP_MASTERPARAMS_T));
    OOP_GPRSCONFIG_T gprsconfig;
    memset(&gprsconfig,0x00,sizeof(OOP_GPRSCONFIG_T));
    
    NOMAL_OAD_T NormalOad ;
    NormalOad.logicId = 0;
    NormalOad.infoNum = 0;
    NormalOad.classtag = CLASS_DATA_UNINIT;
    //最少得是cssdl 0  
    if(buflen<strlen(smshead)+1)
        return -1;
    if(strncmp(smshead,(const char*)buf,strlen(smshead))!=0)
    {
        COMM_FMT_DEBUG("hunan cssdl 未识别到 \n");
        return -1;
    }
    offset+=strlen(smshead);
    uint8 cmd = atoi((char*)&buf[offset]);
    switch(cmd)
    {
        //重启
        case 0:
        {
            COMM_FMT_DEBUG("hunan tmn reboot ! \n");
            set_reset_flag(ptHandle->dbClient, TMN_REBOOT);
            return 0;
        }
            break;
        case 1:
        {
            offset+=2;
            str_split((char*)&buf[offset], ":",str, &num);
            if(num!=3)
            {
                COMM_FMT_DEBUG("param error !\n");
                return -1;
            }
            //ip
            if(FALSE == commfun_IP_unpack(str[0],ip))
            {
                COMM_FMT_DEBUG("IP error !\n");
                return -1;
            }
            //port
            port = atoi(str[1]);
            masterparam.nNum = 1;
            masterparam.master[0].port = port;
            memcpy(masterparam.master[0].ip,ip,4);
            NormalOad.oad.value = 0x45000300;
            ret = db_write_nomal(ptHandle->dbClient, &NormalOad,(uint8*)&masterparam,sizeof(OOP_MASTERPARAMS_T));
            if(ret!=0)
            {
                COMM_FMT_DEBUG("IP port write error %d \n",ret);
            }
            //APN
            NormalOad.oad.value = 0x45000200;
            ret = db_read_nomal(ptHandle->dbClient, &NormalOad,sizeof(OOP_GPRSCONFIG_T), (uint8*)&gprsconfig,&len);
            if(ret!=0 || len!=sizeof(OOP_GPRSCONFIG_T))
            {
                COMM_FMT_DEBUG("read gprsconfig error %d \n",ret);
            }
            gprsconfig.apn.apn.nNum = strlen(str[2]);
            memcpy(gprsconfig.apn.apn.value,str[2],strlen(str[2]));
            ret = db_write_nomal(ptHandle->dbClient, &NormalOad,(uint8*)&gprsconfig,sizeof(OOP_GPRSCONFIG_T));
            if(ret!=0)
            {
                COMM_FMT_DEBUG("gprsconfigwrite error %d \n",ret);
            }
            COMM_FMT_DEBUG("sms ip %d.%d.%d.%d port %d apn %s  \n",ip[0],ip[1],ip[2],ip[3],port,str[2]);
            return 0;
        }
            break;
        case 2:
        {
            offset+=2;
            str_split((char*)&buf[offset], ":",str, &num);
            if(num!=4)
            {
                COMM_FMT_DEBUG("param error !\n");
                return -1;
            }
            //ip
            if(FALSE == commfun_IP_unpack(str[0],ip))
            {
                COMM_FMT_DEBUG("IP error !\n");
                return -1;
            }
            //port
            port = atoi(str[1]);
            masterparam.nNum = 1;
            masterparam.master[0].port = port;
            memcpy(masterparam.master[0].ip,ip,4);
            NormalOad.oad.value = 0x45000300;
            ret = db_write_nomal(ptHandle->dbClient, &NormalOad,(uint8*)&masterparam,sizeof(OOP_MASTERPARAMS_T));
            if(ret!=0)
            {
                COMM_FMT_DEBUG("IP port write error %d \n",ret);
            }
            //用户名密码
            NormalOad.oad.value = 0x45000200;
            ret = db_read_nomal(ptHandle->dbClient, &NormalOad,sizeof(OOP_GPRSCONFIG_T), (uint8*)&gprsconfig,&len);
            if(ret!=0 || len!=sizeof(OOP_GPRSCONFIG_T))
            {
                COMM_FMT_DEBUG("read gprsconfig error %d \n",ret);
            }
            gprsconfig.apn.username.nNum = strlen(str[2]);
            memcpy(gprsconfig.apn.username.value,str[2],strlen(str[2]));
            gprsconfig.apn.pwd.nNum = strlen(str[3]);
            memcpy(gprsconfig.apn.pwd.value,str[3],strlen(str[3]));
            ret = db_write_nomal(ptHandle->dbClient, &NormalOad,(uint8*)&gprsconfig,sizeof(OOP_GPRSCONFIG_T));
            if(ret!=0)
            {
                COMM_FMT_DEBUG("gprsconfigwrite error %d \n",ret);
            }
            COMM_FMT_DEBUG("sms ip %d.%d.%d.%d port %d apnuser %s pwd %s \n",ip[0],ip[1],ip[2],ip[3],port,str[2],str[3]);
            return 0;
        }
            break;
        default:
            break;
    }
    return -1;
}


#endif

