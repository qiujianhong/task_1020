/*
*********************************************************************
* Copyright(C) 2019 �Ͼ��������ӹɷ����޹�˾
* All rights reserved. 
* @brief��   ң������
* @date��    2019-11-21
* @history�� 
*********************************************************************
*/
#include "baseCommon.h"
#include "macro.h"

#include "calculate.h"
#include "framecomm.h"


/**********************************************************************
* @name      : pulse_get_rateNO
* @brief     ����ȡ��ǰ���ʺ�
* @param[in] ��OOP_DAYPERIOD_T *ptDayPeriod
               uint8 hours
               uint8 mins
* @param[out]��
* @return    �����ʺ�
* @Create    : ����
* @Date      ��2019-11-21
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

    ptPeriod = &(ptDayPeriod->dPeriod[0]); //ֻ��Ҫȡ��0��
    if(0 == ptPeriod->nNum)
    {
        return EN_LEVEL; //Ĭ�Ϸ��ʺ�
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
* @brief     ���������幦���㷨A����ʵʱ����(�㷨A:����һ�����ڵ�������������)
               ���㹫ʽ:P =3600*(10000��n)/(�����K����t)  
               ��λ:kw, 10000����4λС��
* @param[in] ��uint32 plsNum  �ۼƵ�������
               uint32 delta   �ۼ�ʱ��
               uint32 metK    �����
* @param[out]��
* @return    ��1���ӹ���
* @Create    : ����
* @Date      ��2019-11-21
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
	if(plsNum == 1)	/*һ������ֻ��һ������*/
	{
		delta = 60000;/* 1min�ĺ���ʱ�� */
	}
#if 0
    //ͳ�Ʒ�ʽ���� ���ܼ�ȥ1
    else
    {
        plsNum--;
    }
#endif
    newdeltime = delta;
#if 0
    /* ͳ��ʱ������ ������ʱ����MIN(1s, �ķ�֮һ������). 58-62sͳ��ʱ���ڽ��е��� */
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
		P = (3600L) * plsNum * f;	/*ʱ��ת��Ϊ20ms*//*ģ�黯ƽ̨����Ҫת��20ms */
	}
	else
		P = 0;
	return P;
}

/**********************************************************************
* @name      : cal_pls_en
* @brief     �������������
* @param[in] ��uint32 plsNum     ������
               uint32 *pleftNum �ϴμ�������
               uint32 metK      �����
* @param[out]��uint32 *pleftNum û�в����������
* @return    ������en ��λ 0.1W
* @Create    : ����
* @Date      ��2019-11-21
* @Update    :
* @Drief     :����en = ����������Pls  + �ϴμ���������/ �����K����10000 ȡ��
              ����rem= ����������Pls  + �ϴμ���������/ �����K����10000 ȡ��
              �˴�10000����4λС��
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
		en  =(uint32)(plsNum*10000+*pleftNum)/metK;       /*--�������--*/
		*pleftNum = (uint32)((plsNum*10000+*pleftNum)%metK);	    /*--ʣ������ȡ��--*/

//		en = (uint64)((plsNum + *rem) * 10000L / metK);       /*�������*/
//		*rem = (uint16)(((plsNum + *rem) * 10000L % metK) / 10000L);/*ʣ������ȡ��*/
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
* @brief     �����ʵ�������(�㷨A:����һ�����ڵ�������������) ���㹦�ʵ���
* @param[in] ��uint32 count     ������
               uint32 timems    ����ʱ��ms 
               uint32* pleftNum ʣ���������
               uint32 K         �����
* @param[out]��uint32* pleftNum ʣ���������
               uint32 *pEn      ����
* @return    ������
* @Create    : ����
* @Date      ��2019-11-21
* @Update    :
**********************************************************************/
uint32 pulse_Acal_pow_en(uint32 count, uint32 timems, uint32 K, uint32* pleftNum, uint32 *pEn)
{
    uint32 			p = 0;     //����

    if(0 != count)  //ͨ��������
    {
		p = cal_pls_powA(count, timems, K); 	  	/*���㹦��,�����㷨A*/
    }
    
    *pEn = cal_pls_en(count, pleftNum, K);
	
    return p;
}

/**********************************************************************
* @name      : cal_pls_dm
* @brief     ����������
* @param[in] ��uint32 count     ������
               uint32 timems    ����ʱ��ms 
               uint32* pleftNum ʣ���������
* @param[out]��uint32* pleftNum ʣ���������
               uint32 *pEn      ����
* @return    ������
* @Create    : ����
* @Date      ��2019-11-21
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


