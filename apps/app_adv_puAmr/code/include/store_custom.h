/*
***************************************************************************
  Copyright (c) 2006-2007,南京新联电子仪器有限责任公司
             All rights reserved.
	程序名称:	store_custom.h
	功    能:	存储定制头文件
	开 发 人：	
	开发时间：2006-11
	
	版本历史:
			2006-11-17 ,v1.0 : New				
	
***************************************************************************
*/

#ifndef _STORE_CUSTOM_H
#define _STORE_CUSTOM_H

#include "sys_cfg.h"

/*************-历史数据相关定义-************/
#if JIANGXI_FUN==1
#define RECORD_DAYS                 365          /*--日数据存储天数--*/
#else
#define RECORD_DAYS                 62          /*--日数据存储天数--*/
#endif

#define RECORD_MONTHS               24          /*--历史月数据存储月数--*/
#define RECORD_CURVE                31          /*--曲线数据存储天数--*/

#define REC_DAY_COPYS               5           /*--日记录数据保存数目(最近5条)--*/
#define REC_MON_COPYS               5           /*--月记录数据保存数目(最近5条)--*/

#define MTR_CURVE_INTERVAL          15                              /*--计量点曲线密度(分)--*/
#define MTR_CURVE_DOTS              (1440/MTR_CURVE_INTERVAL)       /*--计量点曲线点数--*/
#define PLC_CURVE_INTERVAL          60                              /*--载波计量点曲线密度（分）--*/
#define PLC_CURVE_DOTS              (1440/PLC_CURVE_INTERVAL)       /*--载波计量点曲线点数--*/
#define MTR_MINCURVE_INTERVAL       1                               /*--计量点分钟曲线密度（分）--*/
#define MTR_MINCURVE_DOTS           (1440/MTR_MINCURVE_INTERVAL)    /*--计量点分钟曲线点数--*/
#define TMN_CURVE_INTERVAL          15                              /*--终端曲线密度(分)--*/
#define TMN_CURVE_DOTS              (1440/TMN_CURVE_INTERVAL)       /*--终端曲线点数--*/
#define TMN_MINCURVE_INTERVAL       5                               /*--终端分钟曲线密度（分）--*/
#define TMN_MINCURVE_DOTS           (1440/TMN_MINCURVE_INTERVAL)    /*--终端分钟曲线点数--*/

#define MAX_CB_DATA_NUM		13		/*-最多可以保存的抄表日-*/

#define REAL_DATA_DOTS		    10		/*-实时数据的保存的深度点数-*/
#define PARATABLEMAXNUM 	    1		/*-最大方案参数表的个数-*/
#define EVENTFILEMAXNUM 	    15		/*-事件存储文件最大个数-*/
#define DAYDATAFILEMAXNUM 	    62		/*-日数据存储文件最大个数-*/
#define MONATAFILEMAXNUM 	    12		/*-月数据存储文件最大个数-*/
#define CURVEDATAFILEMAXNUM 	288*10	/*-日数据存储文件最大个数-*/

#endif		//_STORE_CUSTOM_H
