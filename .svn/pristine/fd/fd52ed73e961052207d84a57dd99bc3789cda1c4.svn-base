/*-***************************************************************
* Copyright (c) 2006,南京新联电子仪器有限公司
*            All rights reserved.
* 程序名称：EventIO.h
* 版本号：1.0
* 功能：对事件检测过程中用到的数据结构，调度函数进行声明
* 原始开发人及时间：董宁 2006年12月25日
* 修改人员、时间及修改内容:无
* 其他：
******************************************************************-*/
#ifndef _EVENTIO_ZHI_H
#define _EVENTIO_ZHI_H

void Init_ac_data_lock(void);
int  update_ac_data(int clientid, uint8 logicId, uint16 infoNum, uint32 value);
int  GetInitEventInfo(int clientid, uint8         initType, uint8 logicId, uint16 classTag);
void GetMntAddr(OOP_OCTETVAR16_T tLocalAddr);

void event_init_zhj(int clientid);
void event_happen_proc_zhj(int clientid);//-事件生成接口-
int  event_check_faults_zhj(void);
int event_get_faults_zhj(uint16 *typeIndex, uint16 *groupIndex, uint16 *seqIndex, uint8 *pData);

#endif

