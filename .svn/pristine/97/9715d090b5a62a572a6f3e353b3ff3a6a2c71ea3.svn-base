/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   遥脉计算
* @date：    2019-11-21
* @history： 
*********************************************************************
*/
#include "baseCommon.h"
#include "macro.h"

#include "calculate.h"
#include "framecomm.h"


/**********************************************************************
* @name      : pulse_get_rateNO
* @brief     ：获取当前费率号
* @param[in] ：OOP_DAYPERIOD_T *ptDayPeriod
               uint8 hours
               uint8 mins
* @param[out]：
* @return    ：费率号
* @Create    : 王津剑
* @Date      ：2019-11-21
* @Update    :
**********************************************************************/
uint8 pulse_get_rateNO(OOP_DAYPERIOD_T *ptDayPeriod, uint8 hours, uint8 mins)
{
    uint8 i;
    uint32 mintotal = 0;
    DAYPERIOD *ptPeriod = NULL;
    
    if(NULL == ptDayPeriod || hours >= 24 || mins >= 60)
    {
        return 0;
    }

    ptPeriod = &(ptDayPeriod->dPeriod[0]); //只需要取第0个
    if(0 == ptPeriod->nNum)
    {
        return EN_LEVEL; //默认费率号
    }
    else if(1 == ptPeriod->nNum)
    {
        return ptPeriod->period[0].rateNO;
    }
    
    mintotal = hours * 60 + mins;
    for(i = 0; i < ptPeriod->nNum - 1; i++)
    {
        if(ptPeriod->period[i].hour * 60 + ptPeriod->period[i].min <= mintotal &&
            mintotal < ptPeriod->period[i+1].hour * 60 + ptPeriod->period[i+1].min)
        {
            return ptPeriod->period[i].rateNO;
        }
    }
    return ptPeriod->period[i].rateNO;
}

/**********************************************************************
* @name      : cal_pls_powA
* @brief     ：采用脉冲功率算法A计算实时功率(算法A:利用一分钟内的脉冲数来计算)
               计算公式:P =3600*(10000×n)/(电表常数K×Δt)  
               单位:kw, 10000代表4位小数
* @param[in] ：uint32 plsNum  累计的脉冲数
               uint32 delta   累计时间
               uint32 metK    电表常数
* @param[out]：
* @return    ：1分钟功率
* @Create    : 王津剑
* @Date      ：2019-11-21
* @Update    :
**********************************************************************/
uint32 cal_pls_powA(uint32 plsNum, uint32 delta, uint32 metK)
{
	uint32 P;
	double f;
    //uint32 maxamend;
    uint32 newdeltime = 0;
	if(0 == metK || metK == 0xffff)
	{
		metK  = 5000;
	}
	if(metK <= 0 || plsNum <= 0)
	{
		P = 0;
		return P;
	}
	if(plsNum == 1)	/*一分钟内只有一个脉冲*/
	{
		delta = 60000;/* 1min的毫秒时间 */
	}
#if 0
    //统计方式计算 不能减去1
    else
    {
        plsNum--;
    }
#endif
    newdeltime = delta;
#if 0
    /* 统计时间修正 最大调整时间是MIN(1s, 四分之一个周期). 58-62s统计时间内进行调整 */
    maxamend = MIN(1000, delta/(4 * plsNum));
    if(delta > 60000)
    {
        if(delta <= 60000 + maxamend)
        {
            newdeltime = 60000;
        }
        else if(delta <= 62000)
        {
            newdeltime = delta - maxamend;
        }
    }
    else if(delta < 60000)
    {
        if(delta >= 60000 - maxamend)
        {
            newdeltime = 60000;
        }
        else if(delta > 58000)
        {
            newdeltime = delta + maxamend;
        }
    }
    PULSE_FMT_TRACE("Static time %u, amend time %u\n", delta, newdeltime);
#endif
	if((newdeltime > 0)&&(metK > 0))
	{
		f = (10000.0 / metK);
		f = (f / newdeltime) * 1000;
		P = (3600L) * plsNum * f;	/*时间转换为20ms*//*模组化平台不需要转换20ms */
	}
	else
		P = 0;
	return P;
}

/**********************************************************************
* @name      : cal_pls_en
* @brief     ：脉冲电量计算
* @param[in] ：uint32 plsNum     脉冲数
               uint32 *pleftNum 上次计算余数
               uint32 metK      电表常数
* @param[out]：uint32 *pleftNum 没有参与计算余数
* @return    ：电量en 单位 0.1W
* @Create    : 王津剑
* @Date      ：2019-11-21
* @Update    :
* @Drief     :电量en = （（脉冲数Pls  + 上次计算余数）/ 电表常数K）×10000 取整
              余数rem= （（脉冲数Pls  + 上次计算余数）/ 电表常数K）×10000 取余
              此处10000代表4位小数
**********************************************************************/
uint32 cal_pls_en(uint32 plsNum, uint32 *pleftNum, uint32 metK)
{
	uint32 	en = 0;

	if(0 == metK || metK == 0xffff)
	{
		metK = 5000;
	}
	if(metK > 0)
	{
		en  =(uint32)(plsNum*10000+*pleftNum)/metK;       /*--计算电量--*/
		*pleftNum = (uint32)((plsNum*10000+*pleftNum)%metK);	    /*--剩余脉冲取整--*/

//		en = (uint64)((plsNum + *rem) * 10000L / metK);       /*计算电量*/
//		*rem = (uint16)(((plsNum + *rem) * 10000L % metK) / 10000L);/*剩余脉冲取整*/
	}
	else
	{
		*pleftNum = 0;
	   	en = 0;
	}
	return en;
}


/**********************************************************************
* @name      : pulse_cal_pow_en
* @brief     ：功率电量计算(算法A:利用一分钟内的脉冲数来计算) 计算功率电量
* @param[in] ：uint32 count     脉冲数
               uint32 timems    持续时间ms 
               uint32* pleftNum 剩余计算余数
               uint32 K         电表常数
* @param[out]：uint32* pleftNum 剩余计算余数
               uint32 *pEn      电量
* @return    ：功率
* @Create    : 王津剑
* @Date      ：2019-11-21
* @Update    :
**********************************************************************/
uint32 pulse_Acal_pow_en(uint32 count, uint32 timems, uint32 K, uint32* pleftNum, uint32 *pEn)
{
    uint32 			p = 0;     //功率

    if(0 != count)  //通道无数据
    {
		p = cal_pls_powA(count, timems, K); 	  	/*计算功率,采用算法A*/
    }
    
    *pEn = cal_pls_en(count, pleftNum, K);
	
    return p;
}

/**********************************************************************
* @name      : cal_pls_dm
* @brief     ：需量计算
* @param[in] ：uint32 count     脉冲数
               uint32 timems    持续时间ms 
               uint32* pleftNum 剩余计算余数
* @param[out]：uint32* pleftNum 剩余计算余数
               uint32 *pEn      电量
* @return    ：功率
* @Create    : 王津剑
* @Date      ：2019-11-21
* @Update    :
**********************************************************************/
uint32 cal_pls_dm(uint32 curPower, uint8 *pNum, uint32 powerList[])
{
    uint8 i;
    uint64 PowerTotal = 0;
    
	if(DM_POWER_MAX == *pNum)
	{
        memmove(&(powerList[0]), &(powerList[01]), (DM_POWER_MAX-1)*sizeof(uint32));
    }
    else
    {
        *pNum += 1;
    }
    powerList[*pNum-1] = curPower;

    for(i = 0; i < *pNum; i++)
    {
        PowerTotal += powerList[i];
    }
    
    return (uint32)(PowerTotal/(*pNum));
}


