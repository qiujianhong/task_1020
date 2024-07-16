/*-***************************************************************
* Copyright (c) 2006,�Ͼ����������������޹�˾
*            All rights reserved.
* �������ƣ�EventIO.c
* �汾�ţ�1.0
* ���ܣ��¼�ģ�����ӿڼ���ע�ắ��
* ԭʼ�����˼�ʱ�䣺���� 2006��12��25��
* �޸���Ա��ʱ�估�޸�����:��
* ������
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
�������ܵ�Ԫ���¼����ģ���ṩ������ʹ��
���ƣ��¼�����ʼ������
�����������¼����ĳ�ʼ��
�����������
�����������
��������ֵ����
�ڲ��������̣� �¼�����������ĳ�ʼ������ȫ�ֱ�����˽�����ݵĳ�ʼ��
********************************************************************/
void InitEvent(void)
{
    InitEventPara();

	return;
}

/**********************************************************************
* @brief     ���ն��¼����ӿ�
* @param[in] ��
* @param[out]��
* @return    ��
* @Create    : ÷����
* @Date      ��2020-12-09
* @Update    :
**********************************************************************/
void EventHappenProc(void)
{ 
	mtrnum_t bMtrId = MTR_ID_485;
	static uint32 	pscnt = 0;					//ʱ��
    static time_t   timep_sec = 0;
    static time_t   timep_min = 0;
    OOP_METER_T     oopMeter;
    time_t          timep_cur;
    time(&timep_cur); //��ȡ��1970������˶����룬����time_t���͵�timep_cur
    
	if (timep_sec != timep_cur)
	{
		//��������ն��¼��ļ��
		timep_sec = timep_cur;
 //       pscnt ++;

		EVENT_FMT_TRACE("�����¼�����ģ��\n");
		RtuExam();								//Tmn running examing
		EVENT_FMT_TRACE("�ն��¼��ļ�����\n");
	}

    pscnt = COMMFUN_ABS(timep_min,timep_cur);
    if(pscnt >= 60)    
    {
        memset(&oopMeter, 0, sizeof(OOP_METER_T));

        timep_min = timep_cur;
        pscnt = 0;
        //���ֽ��в������¼��ļ��
	    EVENT_FMT_TRACE("���ֽ��в������¼��ļ��\n");
		EVENT_FMT_TRACE("��������%d\n",bMtrId);
        
		while (bMtrId <= NO_VIR_MTR_MAX_NUM)
		{
			if (app_get_meter_list(bMtrId, &oopMeter) == 0)
			{
				EVENT_FMT_TRACE("������%d��Ч,���\n",bMtrId);
				MetExam(bMtrId, oopMeter);					//������澯�¼�����
			}
            bMtrId++;
		}

        CheckERC_310A(0);

		EVENT_FMT_TRACE("�������������=%d\n",bMtrId-1);
        EVENT_FMT_TRACE("�������¼��ļ�����\n");
	}

}

/**********************************************************************
* @name      : get_tmn_halt
* @brief     ����ȡ�ն�ͣ�ϵ�״̬
* @param[in] ��
* @param[out]��
* @return    ��0x55-ͣ�� 0xaa-�ϵ�
* @Create    : ÷����
* @Date      ��2020-02-11
* @Update    :
**********************************************************************/
uint8 get_tmn_halt()
{
    return event_3106.ishappen;
}


/**********************************************************************
* @name      : MntVersionCheck
* @brief     ���ն˰汾�ű�����
* @param[in] ��OOP_VERSION_T  tVersion ��ǰ�汾��
* @param[out]��event_3101.ishappen
* @return    ��
* @Create    : ÷����
* @Date      ��2020-02-11
* @Update    :
**********************************************************************/
void VersionCheck(OOP_VERSION_T      tVersion)
{
//    return MntVersionCheck(tVersion);
}

