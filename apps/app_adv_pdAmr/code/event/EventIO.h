/*-***************************************************************
* Copyright (c) 2006,�Ͼ����������������޹�˾
*            All rights reserved.
* �������ƣ�EventIO.h
* �汾�ţ�1.0
* ���ܣ����¼����������õ������ݽṹ�����Ⱥ�����������
* ԭʼ�����˼�ʱ�䣺���� 2006��12��25��
* �޸���Ա��ʱ�估�޸�����:��
* ������
******************************************************************-*/
#ifndef _EVENTIO_H
#define _EVENTIO_H

void InitLatestMeterReadDataList(int clientid);
int  GetEventNomalParam(int clientid, uint8 logicId, uint16 infoNum, uint32 value);
int  GetInitEventInfo(int clientid, uint8         initType, uint8 logicId, uint16 classTag);
void GetMntAddr(OOP_OCTETVAR16_T tLocalAddr);

void event_init(void);
void event_happen_proc(void);//-�¼����ɽӿ�-
int  event_check_faults(void);
int  event_get_faults(uint8 infolen, uint8 * pData, uint8 bstartnum, uint8 * pNum);

#endif

