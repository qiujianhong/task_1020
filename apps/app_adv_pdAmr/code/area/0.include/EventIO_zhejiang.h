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
#ifndef _EVENTIO_ZHI_H
#define _EVENTIO_ZHI_H

void Init_ac_data_lock(void);
int  update_ac_data(int clientid, uint8 logicId, uint16 infoNum, uint32 value);
int  GetInitEventInfo(int clientid, uint8         initType, uint8 logicId, uint16 classTag);
void GetMntAddr(OOP_OCTETVAR16_T tLocalAddr);

void event_init_zhj(int clientid);
void event_happen_proc_zhj(int clientid);//-�¼����ɽӿ�-
int  event_check_faults_zhj(void);
int event_get_faults_zhj(uint16 *typeIndex, uint16 *groupIndex, uint16 *seqIndex, uint8 *pData);

#endif

