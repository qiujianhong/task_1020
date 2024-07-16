/*-***************************************************************
* Copyright (c) 2006,南京新联电子仪器有限公司
*            All rights reserved.
* 程序名称：EventIO.c
* 版本号：1.0
* 功能：事件模块对外接口集和注册函数
* 原始开发人及时间：董宁 2006年12月25日
* 修改人员、时间及修改内容:无
* 其他：
******************************************************************-*/
#include "OopAll.h"
#include "task_id.h"
#include "framecomm.h"
#include "db_api.h"
#include "AppCommon.h"


#include "EventComm.h"
#include "Event.h"
#include "EventFun.h"
#include "oad_id.h"
#include "oopVarUpdate.h"
#include "commClass.h"

//#include "commuprot.h"
#include "appcomm.h"

/*********************************************************************
所属功能单元：事件检测模块提供主调度使用
名称：事件检测初始化函数
功能描述：事件检测的初始化
输入参数：无
输出参数：无
函数返回值：无
内部处理流程： 事件检测主函数的初始化，对全局变量和私有数据的初始化
********************************************************************/
void InitEvent(void)
{
    InitEventPara();

	return;
}

/**********************************************************************
* @brief     ：终端事件检测接口
* @param[in] ：
* @param[out]：
* @return    ：
* @Create    : 梅安宁
* @Date      ：2020-12-09
* @Update    :
**********************************************************************/
void EventHappenProc(void)
{ 
	mtrnum_t bMtrId = MTR_ID_485;
	static uint32 	pscnt = 0;					//时差
    static time_t   timep_sec = 0;
    static time_t   timep_min = 0;
    OOP_METER_T     oopMeter;
    time_t          timep_cur;
    time(&timep_cur); //获取从1970至今过了多少秒，存入time_t类型的timep_cur
    
	if (timep_sec != timep_cur)
	{
		//过秒进行终端事件的检测
		timep_sec = timep_cur;
 //       pscnt ++;

		EVENT_FMT_TRACE("进入事件生成模块\n");
		RtuExam();								//Tmn running examing
		EVENT_FMT_TRACE("终端事件的检测完毕\n");
	}

    pscnt = COMMFUN_ABS(timep_min,timep_cur);
    if(pscnt >= 60)    
    {
        memset(&oopMeter, 0, sizeof(OOP_METER_T));

        timep_min = timep_cur;
        pscnt = 0;
        //过分进行测量点事件的检测
	    EVENT_FMT_TRACE("过分进行测量点事件的检测\n");
		EVENT_FMT_TRACE("检测测量点%d\n",bMtrId);
        
		while (bMtrId <= NO_VIR_MTR_MAX_NUM)
		{
			if (app_get_meter_list(bMtrId, &oopMeter) == 0)
			{
				EVENT_FMT_TRACE("测量点%d有效,检测\n",bMtrId);
				MetExam(bMtrId, oopMeter);					//测量点告警事件处理
			}
            bMtrId++;
		}

        CheckERC_310A(0);

		EVENT_FMT_TRACE("测量点号在这里=%d\n",bMtrId-1);
        EVENT_FMT_TRACE("测量点事件的检测完毕\n");
	}

}

/**********************************************************************
* @name      : get_tmn_halt
* @brief     ：获取终端停上电状态
* @param[in] ：
* @param[out]：
* @return    ：0x55-停电 0xaa-上电
* @Create    : 梅安宁
* @Date      ：2020-02-11
* @Update    :
**********************************************************************/
uint8 get_tmn_halt()
{
    return event_3106.ishappen;
}


/**********************************************************************
* @name      : MntVersionCheck
* @brief     ：终端版本号变更检查
* @param[in] ：OOP_VERSION_T  tVersion 当前版本号
* @param[out]：event_3101.ishappen
* @return    ：
* @Create    : 梅安宁
* @Date      ：2020-02-11
* @Update    :
**********************************************************************/
void VersionCheck(OOP_VERSION_T      tVersion)
{
//    return MntVersionCheck(tVersion);
}

