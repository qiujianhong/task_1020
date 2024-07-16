/*
***************************************************************************
  Copyright (c) 2006-2007,�Ͼ��������������������ι�˾
             All rights reserved.
	��������:	store_custom.h
	��    ��:	�洢����ͷ�ļ�
	�� �� �ˣ�	
	����ʱ�䣺2006-11
	
	�汾��ʷ:
			2006-11-17 ,v1.0 : New				
	
***************************************************************************
*/

#ifndef _STORE_CUSTOM_H
#define _STORE_CUSTOM_H

#include "sys_cfg.h"

/*************-��ʷ������ض���-************/
#if JIANGXI_FUN==1
#define RECORD_DAYS                 365          /*--�����ݴ洢����--*/
#else
#define RECORD_DAYS                 62          /*--�����ݴ洢����--*/
#endif

#define RECORD_MONTHS               24          /*--��ʷ�����ݴ洢����--*/
#define RECORD_CURVE                31          /*--�������ݴ洢����--*/

#define REC_DAY_COPYS               5           /*--�ռ�¼���ݱ�����Ŀ(���5��)--*/
#define REC_MON_COPYS               5           /*--�¼�¼���ݱ�����Ŀ(���5��)--*/

#define MTR_CURVE_INTERVAL          15                              /*--�����������ܶ�(��)--*/
#define MTR_CURVE_DOTS              (1440/MTR_CURVE_INTERVAL)       /*--���������ߵ���--*/
#define PLC_CURVE_INTERVAL          60                              /*--�ز������������ܶȣ��֣�--*/
#define PLC_CURVE_DOTS              (1440/PLC_CURVE_INTERVAL)       /*--�ز����������ߵ���--*/
#define MTR_MINCURVE_INTERVAL       1                               /*--��������������ܶȣ��֣�--*/
#define MTR_MINCURVE_DOTS           (1440/MTR_MINCURVE_INTERVAL)    /*--������������ߵ���--*/
#define TMN_CURVE_INTERVAL          15                              /*--�ն������ܶ�(��)--*/
#define TMN_CURVE_DOTS              (1440/TMN_CURVE_INTERVAL)       /*--�ն����ߵ���--*/
#define TMN_MINCURVE_INTERVAL       5                               /*--�ն˷��������ܶȣ��֣�--*/
#define TMN_MINCURVE_DOTS           (1440/TMN_MINCURVE_INTERVAL)    /*--�ն˷������ߵ���--*/

#define MAX_CB_DATA_NUM		13		/*-�����Ա���ĳ�����-*/

#define REAL_DATA_DOTS		    10		/*-ʵʱ���ݵı������ȵ���-*/
#define PARATABLEMAXNUM 	    1		/*-��󷽰�������ĸ���-*/
#define EVENTFILEMAXNUM 	    15		/*-�¼��洢�ļ�������-*/
#define DAYDATAFILEMAXNUM 	    62		/*-�����ݴ洢�ļ�������-*/
#define MONATAFILEMAXNUM 	    12		/*-�����ݴ洢�ļ�������-*/
#define CURVEDATAFILEMAXNUM 	288*10	/*-�����ݴ洢�ļ�������-*/

#endif		//_STORE_CUSTOM_H
