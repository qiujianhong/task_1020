#ifndef _LAYER_ID_H_
#define _LAYER_ID_H_

//===============================================================================================================
#if GUI_DISP_TYPE == 0  // 2型风格(无状态栏)
//===============================================================================================================
/************************************************************************/
/* 实时信息                                                           */
/************************************************************************/
#define REMOTE_CTRL_STATUS		0x00810000	// 遥控跳闸状态 
#define REMOTE_CTRL_STATUS_2	0x00800000	// 遥控合闸状态 

#define MON_CTRL_STATUS_1	0x00080100	// 月电控状态
#define MON_CTRL_STATUS_2	0x00080200	// 月电控状态
#define MON_CTRL_STATUS_3	0x00080300	// 月电控状态
#define MON_CTRL_STATUS_4	0x00080400	// 月电控状态
#define MON_CTRL_STATUS_5	0x00080500	// 月电控状态
#define MON_CTRL_STATUS_6	0x00080600	// 月电控状态
#define MON_CTRL_STATUS_7	0x00080700	// 月电控状态
#define MON_CTRL_STATUS_8	0x00080800	// 月电控状态

#define OVERFLOW_CTRL_STATUS_1	0x00820100	// 下浮控状态
#define OVERFLOW_CTRL_STATUS_2	0x00820200	// 下浮控状态
#define OVERFLOW_CTRL_STATUS_3	0x00820300	// 下浮控状态
#define OVERFLOW_CTRL_STATUS_4	0x00820400	// 下浮控状态
#define OVERFLOW_CTRL_STATUS_5	0x00820500	// 下浮控状态
#define OVERFLOW_CTRL_STATUS_6	0x00820600	// 下浮控状态
#define OVERFLOW_CTRL_STATUS_7	0x00820700	// 下浮控状态
#define OVERFLOW_CTRL_STATUS_8	0x00820800	// 下浮控状态


#define STOP_CTRL_STATUS_1	0x00870100	// 报停控状态
#define STOP_CTRL_STATUS_2	0x00870200	// 报停控状态
#define STOP_CTRL_STATUS_3	0x00870300	// 报停控状态
#define STOP_CTRL_STATUS_4	0x00870400	// 报停控状态
#define STOP_CTRL_STATUS_5	0x00870500	// 报停控状态
#define STOP_CTRL_STATUS_6	0x00870600	// 报停控状态
#define STOP_CTRL_STATUS_7	0x00870700	// 报停控状态
#define STOP_CTRL_STATUS_8	0x00870800	// 报停控状态

#define RESET_CTRL_STATUS_1	0x00830100	// 厂休控状态
#define RESET_CTRL_STATUS_2	0x00830200	// 厂休控状态
#define RESET_CTRL_STATUS_3	0x00830300	// 厂休控状态
#define RESET_CTRL_STATUS_4	0x00830400	// 厂休控状态
#define RESET_CTRL_STATUS_5	0x00830500	// 厂休控状态
#define RESET_CTRL_STATUS_6	0x00830600	// 厂休控状态
#define RESET_CTRL_STATUS_7	0x00830700	// 厂休控状态
#define RESET_CTRL_STATUS_8	0x00830800	// 厂休控状态

#define REMOTE_SWITCH_STATUS	0x00030000	// 遥信开关状态

#define TIME_CTRL_STATUS_1	0x00860100	// 时段控状态
#define TIME_CTRL_STATUS_2	0x00860200	// 时段控状态
#define TIME_CTRL_STATUS_3	0x00860300	// 时段控状态
#define TIME_CTRL_STATUS_4	0x00860400	// 时段控状态
#define TIME_CTRL_STATUS_5	0x00860500	// 时段控状态
#define TIME_CTRL_STATUS_6	0x00860600	// 时段控状态
#define TIME_CTRL_STATUS_7	0x00860700	// 时段控状态
#define TIME_CTRL_STATUS_8	0x00860800	// 时段控状态

#define BUY_CTRL_STATUS_1	0x00850100	// 购电控状态
#define BUY_CTRL_STATUS_2	0x00850200	// 购电控状态
#define BUY_CTRL_STATUS_3	0x00850300	// 购电控状态
#define BUY_CTRL_STATUS_4	0x00850400	// 购电控状态
#define BUY_CTRL_STATUS_5	0x00850500	// 购电控状态
#define BUY_CTRL_STATUS_6	0x00850600	// 购电控状态
#define BUY_CTRL_STATUS_7	0x00850700	// 购电控状态
#define BUY_CTRL_STATUS_8	0x00850800	// 购电控状态

/************************************************************************/
/* 终端数据                                                             */
/************************************************************************/
#define TERM_SYS_RESET		0x00610001 // 系统重起
/************************************************************************/
/* 终端数据结束                                                         */
/************************************************************************/


/************************************************************************/
/* 控制参数                                                             */
/************************************************************************/
// 时段控参数
#define TIME_CTRL_PARA_1_1 		0x00280100
#define TIME_CTRL_PARA_2_1 		0x00280200
#define TIME_CTRL_PARA_3_1 		0x00280300
#define TIME_CTRL_PARA_4_1 		0x00280400
#define TIME_CTRL_PARA_5_1 		0x00280500
#define TIME_CTRL_PARA_6_1 		0x00280600
#define TIME_CTRL_PARA_7_1 		0x00280700
#define TIME_CTRL_PARA_8_1 		0x00280800 

// 下浮控参数
#define LOW_CTRL_PATA_1		0x00210100
#define LOW_CTRL_PATA_2		0x00210200
#define LOW_CTRL_PATA_3		0x00210300
#define LOW_CTRL_PATA_4		0x00210400
#define LOW_CTRL_PATA_5		0x00210500
#define LOW_CTRL_PATA_6		0x00210600
#define LOW_CTRL_PATA_7		0x00210700
#define LOW_CTRL_PATA_8		0x00210800

// 厂休控参数
#define RESET_CTRL_PATA_1	0x00220100
#define RESET_CTRL_PATA_2	0x00220200
#define RESET_CTRL_PATA_3	0x00220300
#define RESET_CTRL_PATA_4	0x00220400
#define RESET_CTRL_PATA_5	0x00220500
#define RESET_CTRL_PATA_6	0x00220600
#define RESET_CTRL_PATA_7	0x00220700
#define RESET_CTRL_PATA_8	0x00220800

// 月电控参数
#define MON_CTRL_PATA_1		0x00240100
#define MON_CTRL_PATA_2   	0x00240200
#define MON_CTRL_PATA_3   	0x00240300
#define MON_CTRL_PATA_4   	0x00240400
#define MON_CTRL_PATA_5		0x00240500
#define MON_CTRL_PATA_6   	0x00240600
#define MON_CTRL_PATA_7   	0x00240700
#define MON_CTRL_PATA_8   	0x00240800

// 报停控参数
#define STOP_CTRL_PATA_1  	0x00290100
#define STOP_CTRL_PATA_2  	0x00290200
#define STOP_CTRL_PATA_3  	0x00290300
#define STOP_CTRL_PATA_4  	0x00290400
#define STOP_CTRL_PATA_5  	0x00290500
#define STOP_CTRL_PATA_6  	0x00290600
#define STOP_CTRL_PATA_7  	0x00290700
#define STOP_CTRL_PATA_8  	0x00290800

// 购电控参数
#define BUY_CTRL_PATA_1	 0x00010100
#define BUY_CTRL_PATA_2  0x00010200
#define BUY_CTRL_PATA_3  0x00010300
#define BUY_CTRL_PATA_4  0x00010400
#define BUY_CTRL_PATA_5	 0x00010500
#define BUY_CTRL_PATA_6  0x00010600
#define BUY_CTRL_PATA_7  0x00010700
#define BUY_CTRL_PATA_8  0x00010800

#define PowerDown_PARA_1 0x99999999
#define PowerDown_PARA_2 0x99999999
#define PowerDown_PARA_3 0x99999999
#define PowerDown_PARA_4 0x99999999

#define CALLING_EXES_PARA  0x00260000 // 催费控参数
/************************************************************************/
/* 控制参数结束                                                         */
/************************************************************************/


/************************************************************************/
/* 告警页面库                                                           */
/************************************************************************/
#define ALARM_STATUS_1    	0x00900000    // 时段控8个轮次的告警状态
#define ALARM_STATUS_2    	0x00900001    // 厂休控8个轮次的告警状态
#define ALARM_STATUS_3    	0x00900002    // 实时控8个轮次的告警状态
#define ALARM_STATUS_4      0x00900003    // 营业报停控8个轮次的告警状态
#define ALARM_STATUS_5    	0x00900004    // 日电控8个轮次的告警状态
#define ALARM_STATUS_6    	0x00900005    // 月电控8个轮次的告警状态
#define ALARM_STATUS_7    	0x00900006    // 购电控8个轮次的告警状态
#define ALARM_STATUS_8      0x00900007    // 遥控8个轮次的告警状态
#define ALARM_STATUS_9      0x00900008    // 总控制8个轮次的告警状态

#define GATE_STATUS_1       0x00900009     // 时段控8个轮次的跳闸状态
#define GATE_STATUS_2       0x0090000A     // 厂休控8个轮次的跳闸状态
#define GATE_STATUS_3       0x0090000B     // 实时控8个轮次的跳闸状态
#define GATE_STATUS_4       0x0090000C     // 营业报停控8个轮次的跳闸状态
#define GATE_STATUS_5       0x0090000D     // 日电控8个轮次的跳闸状态
#define GATE_STATUS_6       0x0090000E     // 月电控8个轮次的跳闸状态
#define GATE_STATUS_7       0x0090000F     // 购电控8个轮次的跳闸状态
#define GATE_STATUS_8       0x00900010     // 遥控8个轮次的跳闸状态
#define GATE_STATUS_9       0x00800000     // 总控制8个轮次的跳闸状态
/************************************************************************/
/* 告警页面库结束                                                       */
/************************************************************************/


/************************************************************************/
/* 中文信息页面库                                                       */
/************************************************************************/
#define MSG_INFO_1      0x00700000       // 中文信息1 
#define MSG_INFO_2		0x00700001       // 中文信息2 
#define MSG_INFO_3		0x00700002       // 中文信息3 
#define MSG_INFO_4		0x00700003       // 中文信息4	
#define MSG_INFO_5		0x00700004       // 中文信息5	
#define MSG_INFO_6		0x00700005       // 中文信息6	
#define MSG_INFO_7		0x00700006       // 中文信息7	
#define MSG_INFO_8		0x00700007       // 中文信息8	
#define MSG_INFO_9		0x00700008       // 中文信息9	
#define MSG_INFO_10		0x00700009       // 中文信息10
/************************************************************************/
/* 中文信息页面库结束                                                   */
/************************************************************************/

#define TMN_CTRL_STAT 	0x00010101		//-终端当前的控制状态-
#define DNB_PARA		0x00430101		//-电能表参数-
#define PULSE_PARA		0x00400000		//-Kv Ki Kp-

//===============================================================================================================
#elif GUI_DISP_TYPE == 1  // 231风格(有状态栏)
//===============================================================================================================
/************************************************************************/
/* 实时信息                                                             */
/************************************************************************/
#define REMOTE_CTRL_STATUS		0x00020000	// 遥控跳闸状态 
#define REMOTE_CTRL_STATUS_2	0x00020000	// 遥控合闸状态,需要换为综合控制状态 

#define MON_CTRL_STATUS_1		0x00080100	// 月电控状态
#define MON_CTRL_STATUS_2		0x00080200	// 月电控状态
#define MON_CTRL_STATUS_3		0x00080300	// 月电控状态
#define MON_CTRL_STATUS_4		0x00080400	// 月电控状态

#define OVERFLOW_CTRL_STATUS_1	0x00040100	// 下浮控状态
#define OVERFLOW_CTRL_STATUS_2	0x00040200	// 下浮控状态
#define OVERFLOW_CTRL_STATUS_3	0x00040300	// 下浮控状态
#define OVERFLOW_CTRL_STATUS_4	0x00040400	// 下浮控状态

#define STOP_CTRL_STATUS_1		0x00050100	// 报停控状态
#define STOP_CTRL_STATUS_2		0x00050200	// 报停控状态
#define STOP_CTRL_STATUS_3		0x00050300	// 报停控状态
#define STOP_CTRL_STATUS_4		0x00050400	// 报停控状态

#define RESET_CTRL_STATUS_1		0x00060100	// 厂休控状态
#define RESET_CTRL_STATUS_2		0x00060200	// 厂休控状态
#define RESET_CTRL_STATUS_3		0x00060300	// 厂休控状态
#define RESET_CTRL_STATUS_4		0x00060400	// 厂休控状态

#define REMOTE_SWITCH_STATUS	0x000b0000	// 遥信开关状态

#define TIME_CTRL_STATUS_1		0x00030100	// 时段控状态
#define TIME_CTRL_STATUS_2		0x00030200	// 时段控状态
#define TIME_CTRL_STATUS_3		0x00030300	// 时段控状态
#define TIME_CTRL_STATUS_4		0x00030400	// 时段控状态

#define BUY_CTRL_STATUS_1		0x00090100	// 购电控状态
#define BUY_CTRL_STATUS_2		0x00090200	// 购电控状态
#define BUY_CTRL_STATUS_3		0x00090300	// 购电控状态
#define BUY_CTRL_STATUS_4		0x00090400	// 购电控状态

/************************************************************************/
/* 终端数据                                                             */
/************************************************************************/
#define TERM_SYS_RESET			0x00610001 // 系统重起
/************************************************************************/
/* 终端数据结束                                                         */
/************************************************************************/


/************************************************************************/
/* 控制参数                                                             */
/************************************************************************/
// 时段控参数
// 控制组1
#define TIME_CTRL_PARA_1_1 		0x00200100
#define TIME_CTRL_PARA_2_1 		0x00200200
#define TIME_CTRL_PARA_3_1 		0x00200300
#define TIME_CTRL_PARA_4_1 		0x00200400                            	
                            	
// 下浮控参数               	
#define LOW_CTRL_PATA_1			0x00210100
#define LOW_CTRL_PATA_2			0x00210200
#define LOW_CTRL_PATA_3			0x00210300
#define LOW_CTRL_PATA_4			0x00210400

// 厂休控参数
#define RESET_CTRL_PATA_1		0x00220100
#define RESET_CTRL_PATA_2		0x00220200
#define RESET_CTRL_PATA_3		0x00220300
#define RESET_CTRL_PATA_4		0x00220400

// 月电控参数
#define MON_CTRL_PATA_1			0x00240100
#define MON_CTRL_PATA_2   		0x00240200
#define MON_CTRL_PATA_3   		0x00240300
#define MON_CTRL_PATA_4   		0x00240400

// 报停控参数
#define STOP_CTRL_PATA_1		0x00290100
#define STOP_CTRL_PATA_2  		0x00290200
#define STOP_CTRL_PATA_3  		0x00290300
#define STOP_CTRL_PATA_4  		0x00290400

// 购电控参数
#define BUY_CTRL_PATA_1	 		0x00250100
#define BUY_CTRL_PATA_2  		0x00250200
#define BUY_CTRL_PATA_3  		0x00250300
#define BUY_CTRL_PATA_4  		0x00250400

#define PowerDown_PARA_1        0x00210100
#define PowerDown_PARA_2        0x00210200
#define PowerDown_PARA_3        0x00210300
#define PowerDown_PARA_4        0x00210400

#define CALLING_EXES_PARA  		0x00260000 // 催费控参数
/************************************************************************/
/* 控制参数结束                                                         */
/************************************************************************/

/************************************************************************/
/* 告警页面库                                                           */
/************************************************************************/
#define ALARM_STATUS_1    		0x00900000    // 时段控8个轮次的告警状态
#define ALARM_STATUS_2    		0x00900001    // 厂休控8个轮次的告警状态
#define ALARM_STATUS_3    		0x00900002    // 实时控8个轮次的告警状态
#define ALARM_STATUS_4      	0x00900003    // 营业报停控8个轮次的告警状态
#define ALARM_STATUS_5    		0x00900004    // 日电控8个轮次的告警状态
#define ALARM_STATUS_6    		0x00900005    // 月电控8个轮次的告警状态
#define ALARM_STATUS_7    		0x00900006    // 购电控8个轮次的告警状态
#define ALARM_STATUS_8      	0x00900007    // 遥控8个轮次的告警状态
#define ALARM_STATUS_9      	0x00900008    // 总控制8个轮次的告警状态

#define GATE_STATUS_1       	0x00900009     // 时段控8个轮次的跳闸状态
#define GATE_STATUS_2       	0x0090000A     // 厂休控8个轮次的跳闸状态
#define GATE_STATUS_3       	0x0090000B     // 实时控8个轮次的跳闸状态
#define GATE_STATUS_4       	0x0090000C     // 营业报停控8个轮次的跳闸状态
#define GATE_STATUS_5       	0x0090000D     // 日电控8个轮次的跳闸状态
#define GATE_STATUS_6       	0x0090000E     // 月电控8个轮次的跳闸状态
#define GATE_STATUS_7       	0x0090000F     // 购电控8个轮次的跳闸状态
#define GATE_STATUS_8       	0x00900010     // 遥控8个轮次的跳闸状态
#define GATE_STATUS_9       	0x00900011     // 总控制8个轮次的跳闸状态
/************************************************************************/
/* 告警页面库结束                                                       */
/************************************************************************/


/************************************************************************/
/* 中文信息页面库                                                       */
/************************************************************************/
#define MSG_INFO_1    	0x00700000       // 中文信息1 
#define MSG_INFO_2		0x00700001       // 中文信息2 
#define MSG_INFO_3		0x00700002       // 中文信息3 
#define MSG_INFO_4		0x00700003       // 中文信息4	
#define MSG_INFO_5		0x00700004       // 中文信息5	
#define MSG_INFO_6		0x00700005       // 中文信息6	
#define MSG_INFO_7		0x00700006       // 中文信息7	
#define MSG_INFO_8		0x00700007       // 中文信息8	
#define MSG_INFO_9		0x00700008       // 中文信息9	
#define MSG_INFO_10		0x00700009       // 中文信息10
/************************************************************************/
/* 中文信息页面库结束                                                   */
/************************************************************************/

#define TMN_CTRL_STAT 	0x000a0100		//-终端当前的控制状态-
#define DNB_PARA		0x00400100		//-电能表参数-
#define PULSE_PARA		0x00400100		//-Kv Ki Kp-


//===============================================================================================================
#elif GUI_DISP_TYPE == 2  // 01g风格(240x128)
//===============================================================================================================
#define REMOTE_CTRL_STATUS	0x00020000	// 遥控状态 
#define REMOTE_CTRL_STATUS_2	0x20800000	// 遥控合闸状态 

#define MON_CTRL_STATUS_1	0x00080100	// 月电控状态
#define MON_CTRL_STATUS_2	0x00080200	// 月电控状态
#define MON_CTRL_STATUS_3	0x00080300	// 月电控状态
#define MON_CTRL_STATUS_4	0x00080400	// 月电控状态

#define OVERFLOW_CTRL_STATUS_1	0x00040100	// 下浮控状态
#define OVERFLOW_CTRL_STATUS_2	0x00040200	// 下浮控状态
#define OVERFLOW_CTRL_STATUS_3	0x00040300	// 下浮控状态
#define OVERFLOW_CTRL_STATUS_4	0x00040400	// 下浮控状态

#define STOP_CTRL_STATUS_1	0x00050100	// 报停控状态
#define STOP_CTRL_STATUS_2	0x00050200	// 报停控状态
#define STOP_CTRL_STATUS_3	0x00050300	// 报停控状态
#define STOP_CTRL_STATUS_4	0x00050400	// 报停控状态

#define RESET_CTRL_STATUS_1	0x00060100	// 厂休控状态
#define RESET_CTRL_STATUS_2	0x00060200	// 厂休控状态
#define RESET_CTRL_STATUS_3	0x00060300	// 厂休控状态
#define RESET_CTRL_STATUS_4	0x00060400	// 厂休控状态

#define REMOTE_SWITCH_STATUS	0x20030000	// 遥信开关状态

#define TIME_CTRL_STATUS_1	0x00030100	// 时段控状态
#define TIME_CTRL_STATUS_2	0x00030200	// 时段控状态
#define TIME_CTRL_STATUS_3	0x00030300	// 时段控状态
#define TIME_CTRL_STATUS_4	0x00030400	// 时段控状态

#define BUY_CTRL_STATUS_1	0x00090100	// 购电控状态
#define BUY_CTRL_STATUS_2	0x00090200	// 购电控状态
#define BUY_CTRL_STATUS_3	0x00090300	// 购电控状态
#define BUY_CTRL_STATUS_4	0x00090400	// 购电控状态

/************************************************************************/
/* 终端数据                                                             */
/************************************************************************/
#define TERM_SYS_RESET		0x00610001 // 系统重起
/************************************************************************/
/* 终端数据结束                                                         */
/************************************************************************/


/************************************************************************/
/* 控制参数                                                             */
/************************************************************************/
// 时段控参数
// 第一套
#define TIME_CTRL_PARA_1_1 		0x20280100
#define TIME_CTRL_PARA_2_1 		0x20280200
#define TIME_CTRL_PARA_3_1 		0x20280300
#define TIME_CTRL_PARA_4_1 		0x20280400

#define LOW_CTRL_PATA_1	  0x20120100
#define LOW_CTRL_PATA_2   0x20120200
#define LOW_CTRL_PATA_3   0x20120300
#define LOW_CTRL_PATA_4   0x20120400

// 厂休控参数
#define RESET_CTRL_PATA_1	0x20110100
#define RESET_CTRL_PATA_2   0x20110200
#define RESET_CTRL_PATA_3   0x20110300
#define RESET_CTRL_PATA_4   0x20110400

// 月电控参数
#define MON_CTRL_PATA_1	  0x20240100
#define MON_CTRL_PATA_2   0x20240200
#define MON_CTRL_PATA_3   0x20240300
#define MON_CTRL_PATA_4   0x20240400

// 报停控参数
#define STOP_CTRL_PATA_1  0x20290100
#define STOP_CTRL_PATA_2  0x20290200
#define STOP_CTRL_PATA_3  0x20290300
#define STOP_CTRL_PATA_4  0x20290400

// 购电控参数
#define BUY_CTRL_PATA_1	 0x00010100
#define BUY_CTRL_PATA_2  0x00010200
#define BUY_CTRL_PATA_3  0x00010300
#define BUY_CTRL_PATA_4  0x00010400

#define PowerDown_PARA_1 0x20120100
#define PowerDown_PARA_2 0x20120200
#define PowerDown_PARA_3 0x20120300
#define PowerDown_PARA_4 0x20120400

#define CALLING_EXES_PARA  0x00260000 // 催费控参数
/************************************************************************/
/* 控制参数结束                                                         */
/************************************************************************/


/************************************************************************/
/* 告警页面库                                                           */
/************************************************************************/
#define ALARM_STATUS_1    	0x00900000    // 时段控8个轮次的告警状态
#define ALARM_STATUS_2    	0x00900001    // 厂休控8个轮次的告警状态
#define ALARM_STATUS_3    	0x00900002    // 实时控8个轮次的告警状态
#define ALARM_STATUS_4      0x00900003    // 营业报停控8个轮次的告警状态
#define ALARM_STATUS_5    	0x00900004    // 日电控8个轮次的告警状态
#define ALARM_STATUS_6    	0x00900005    // 月电控8个轮次的告警状态
#define ALARM_STATUS_7    	0x00900006    // 购电控8个轮次的告警状态
#define ALARM_STATUS_8      0x00900007    // 遥控8个轮次的告警状态
#define ALARM_STATUS_9      0x00900008    // 总控制8个轮次的告警状态

#define GATE_STATUS_1       0x00900009     // 时段控8个轮次的跳闸状态
#define GATE_STATUS_2       0x0090000A     // 厂休控8个轮次的跳闸状态
#define GATE_STATUS_3       0x0090000B     // 实时控8个轮次的跳闸状态
#define GATE_STATUS_4       0x0090000C     // 营业报停控8个轮次的跳闸状态
#define GATE_STATUS_5       0x0090000D     // 日电控8个轮次的跳闸状态
#define GATE_STATUS_6       0x0090000E     // 月电控8个轮次的跳闸状态
#define GATE_STATUS_7       0x0090000F     // 购电控8个轮次的跳闸状态
#define GATE_STATUS_8       0x00900010     // 遥控8个轮次的跳闸状态
#define GATE_STATUS_9       0x00900011     // 总控制8个轮次的跳闸状态
/************************************************************************/
/* 告警页面库结束                                                       */
/************************************************************************/


/************************************************************************/
/* 中文信息页面库                                                       */
/************************************************************************/
#define MSG_INFO_1      0x20400000       // 中文信息1 
#define MSG_INFO_2		0x20400001       // 中文信息2 
#define MSG_INFO_3		0x20400002       // 中文信息3 
#define MSG_INFO_4		0x20400003       // 中文信息4	
#define MSG_INFO_5		0x20400004       // 中文信息5	
#define MSG_INFO_6		0x20400005       // 中文信息6	
#define MSG_INFO_7		0x20400006       // 中文信息7	
#define MSG_INFO_8		0x20400007       // 中文信息8	
#define MSG_INFO_9		0x20400008       // 中文信息9	
#define MSG_INFO_10		0x20400009       // 中文信息10
/************************************************************************/
/* 中文信息页面库结束                                                   */
/************************************************************************/

#define TMN_CTRL_STAT 	0x00010101		//-终端当前的控制状态-
#define DNB_PARA		0x20140000		//-电能表参数-
#define PULSE_PARA		0x20130000		//-Kv Ki Kp-

#elif GUI_DISP_TYPE == 3  // 01g风格(128x64)
//===============================================================================================================
#define REMOTE_CTRL_STATUS		0x00810000	// 遥控跳闸状态 
#define REMOTE_CTRL_STATUS_2	0x00800000	// 遥控合闸状态 

#define MON_CTRL_STATUS_1	0x00080100	// 月电控状态
#define MON_CTRL_STATUS_2	0x00080200	// 月电控状态
#define MON_CTRL_STATUS_3	0x00080300	// 月电控状态
#define MON_CTRL_STATUS_4	0x00080400	// 月电控状态
#define MON_CTRL_STATUS_5	0x00080500	// 月电控状态
#define MON_CTRL_STATUS_6	0x00080600	// 月电控状态
#define MON_CTRL_STATUS_7	0x00080700	// 月电控状态
#define MON_CTRL_STATUS_8	0x00080800	// 月电控状态

#define OVERFLOW_CTRL_STATUS_1	0x00820100	// 下浮控状态
#define OVERFLOW_CTRL_STATUS_2	0x00820200	// 下浮控状态
#define OVERFLOW_CTRL_STATUS_3	0x00820300	// 下浮控状态
#define OVERFLOW_CTRL_STATUS_4	0x00820400	// 下浮控状态
#define OVERFLOW_CTRL_STATUS_5	0x00820500	// 下浮控状态
#define OVERFLOW_CTRL_STATUS_6	0x00820600	// 下浮控状态
#define OVERFLOW_CTRL_STATUS_7	0x00820700	// 下浮控状态
#define OVERFLOW_CTRL_STATUS_8	0x00820800	// 下浮控状态


#define STOP_CTRL_STATUS_1	0x00870100	// 报停控状态
#define STOP_CTRL_STATUS_2	0x00870200	// 报停控状态
#define STOP_CTRL_STATUS_3	0x00870300	// 报停控状态
#define STOP_CTRL_STATUS_4	0x00870400	// 报停控状态
#define STOP_CTRL_STATUS_5	0x00870500	// 报停控状态
#define STOP_CTRL_STATUS_6	0x00870600	// 报停控状态
#define STOP_CTRL_STATUS_7	0x00870700	// 报停控状态
#define STOP_CTRL_STATUS_8	0x00870800	// 报停控状态

#define RESET_CTRL_STATUS_1	0x00830100	// 厂休控状态
#define RESET_CTRL_STATUS_2	0x00830200	// 厂休控状态
#define RESET_CTRL_STATUS_3	0x00830300	// 厂休控状态
#define RESET_CTRL_STATUS_4	0x00830400	// 厂休控状态
#define RESET_CTRL_STATUS_5	0x00830500	// 厂休控状态
#define RESET_CTRL_STATUS_6	0x00830600	// 厂休控状态
#define RESET_CTRL_STATUS_7	0x00830700	// 厂休控状态
#define RESET_CTRL_STATUS_8	0x00830800	// 厂休控状态

#define REMOTE_SWITCH_STATUS	0x00030000	// 遥信开关状态

#define TIME_CTRL_STATUS_1	0x00860100	// 时段控状态
#define TIME_CTRL_STATUS_2	0x00860200	// 时段控状态
#define TIME_CTRL_STATUS_3	0x00860300	// 时段控状态
#define TIME_CTRL_STATUS_4	0x00860400	// 时段控状态
#define TIME_CTRL_STATUS_5	0x00860500	// 时段控状态
#define TIME_CTRL_STATUS_6	0x00860600	// 时段控状态
#define TIME_CTRL_STATUS_7	0x00860700	// 时段控状态
#define TIME_CTRL_STATUS_8	0x00860800	// 时段控状态

#define BUY_CTRL_STATUS_1	0x00850100	// 购电控状态
#define BUY_CTRL_STATUS_2	0x00850200	// 购电控状态
#define BUY_CTRL_STATUS_3	0x00850300	// 购电控状态
#define BUY_CTRL_STATUS_4	0x00850400	// 购电控状态
#define BUY_CTRL_STATUS_5	0x00850500	// 购电控状态
#define BUY_CTRL_STATUS_6	0x00850600	// 购电控状态
#define BUY_CTRL_STATUS_7	0x00850700	// 购电控状态
#define BUY_CTRL_STATUS_8	0x00850800	// 购电控状态

/************************************************************************/
/* 终端数据                                                             */
/************************************************************************/
#define TERM_SYS_RESET		0x00610001 // 系统重起
/************************************************************************/
/* 终端数据结束                                                         */
/************************************************************************/


/************************************************************************/
/* 控制参数                                                             */
/************************************************************************/
// 时段控参数
#define TIME_CTRL_PARA_1_1 		0x00280100
#define TIME_CTRL_PARA_2_1 		0x00280200
#define TIME_CTRL_PARA_3_1 		0x00280300
#define TIME_CTRL_PARA_4_1 		0x00280400
#define TIME_CTRL_PARA_5_1 		0x00280500
#define TIME_CTRL_PARA_6_1 		0x00280600
#define TIME_CTRL_PARA_7_1 		0x00280700
#define TIME_CTRL_PARA_8_1 		0x00280800 

// 下浮控参数
#define LOW_CTRL_PATA_1		0x00210100
#define LOW_CTRL_PATA_2		0x00210200
#define LOW_CTRL_PATA_3		0x00210300
#define LOW_CTRL_PATA_4		0x00210400
#define LOW_CTRL_PATA_5		0x00210500
#define LOW_CTRL_PATA_6		0x00210600
#define LOW_CTRL_PATA_7		0x00210700
#define LOW_CTRL_PATA_8		0x00210800

// 厂休控参数
#define RESET_CTRL_PATA_1	0x00220100
#define RESET_CTRL_PATA_2	0x00220200
#define RESET_CTRL_PATA_3	0x00220300
#define RESET_CTRL_PATA_4	0x00220400
#define RESET_CTRL_PATA_5	0x00220500
#define RESET_CTRL_PATA_6	0x00220600
#define RESET_CTRL_PATA_7	0x00220700
#define RESET_CTRL_PATA_8	0x00220800

// 月电控参数
#define MON_CTRL_PATA_1		0x00240100
#define MON_CTRL_PATA_2   	0x00240200
#define MON_CTRL_PATA_3   	0x00240300
#define MON_CTRL_PATA_4   	0x00240400
#define MON_CTRL_PATA_5		0x00240500
#define MON_CTRL_PATA_6   	0x00240600
#define MON_CTRL_PATA_7   	0x00240700
#define MON_CTRL_PATA_8   	0x00240800

// 报停控参数
#define STOP_CTRL_PATA_1  	0x00290100
#define STOP_CTRL_PATA_2  	0x00290200
#define STOP_CTRL_PATA_3  	0x00290300
#define STOP_CTRL_PATA_4  	0x00290400
#define STOP_CTRL_PATA_5  	0x00290500
#define STOP_CTRL_PATA_6  	0x00290600
#define STOP_CTRL_PATA_7  	0x00290700
#define STOP_CTRL_PATA_8  	0x00290800

// 购电控参数
#define BUY_CTRL_PATA_1	 0x00010100
#define BUY_CTRL_PATA_2  0x00010200
#define BUY_CTRL_PATA_3  0x00010300
#define BUY_CTRL_PATA_4  0x00010400
#define BUY_CTRL_PATA_5	 0x00010500
#define BUY_CTRL_PATA_6  0x00010600
#define BUY_CTRL_PATA_7  0x00010700
#define BUY_CTRL_PATA_8  0x00010800

#define PowerDown_PARA_1 0x00210100
#define PowerDown_PARA_2 0x00210200
#define PowerDown_PARA_3 0x00210300
#define PowerDown_PARA_4 0x00210400

#define CALLING_EXES_PARA  0x00260000 // 催费控参数
/************************************************************************/
/* 控制参数结束                                                         */
/************************************************************************/


/************************************************************************/
/* 告警页面库                                                           */
/************************************************************************/
#define ALARM_STATUS_1    	0x00900000    // 时段控8个轮次的告警状态
#define ALARM_STATUS_2    	0x00900001    // 厂休控8个轮次的告警状态
#define ALARM_STATUS_3    	0x00900002    // 实时控8个轮次的告警状态
#define ALARM_STATUS_4      0x00900003    // 营业报停控8个轮次的告警状态
#define ALARM_STATUS_5    	0x00900004    // 日电控8个轮次的告警状态
#define ALARM_STATUS_6    	0x00900005    // 月电控8个轮次的告警状态
#define ALARM_STATUS_7    	0x00900006    // 购电控8个轮次的告警状态
#define ALARM_STATUS_8      0x00900007    // 遥控8个轮次的告警状态
#define ALARM_STATUS_9      0x00900008    // 总控制8个轮次的告警状态

#define GATE_STATUS_1       0x00900009     // 时段控8个轮次的跳闸状态
#define GATE_STATUS_2       0x0090000A     // 厂休控8个轮次的跳闸状态
#define GATE_STATUS_3       0x0090000B     // 实时控8个轮次的跳闸状态
#define GATE_STATUS_4       0x0090000C     // 营业报停控8个轮次的跳闸状态
#define GATE_STATUS_5       0x0090000D     // 日电控8个轮次的跳闸状态
#define GATE_STATUS_6       0x0090000E     // 月电控8个轮次的跳闸状态
#define GATE_STATUS_7       0x0090000F     // 购电控8个轮次的跳闸状态
#define GATE_STATUS_8       0x00900010     // 遥控8个轮次的跳闸状态
#define GATE_STATUS_9       0x00800000     // 总控制8个轮次的跳闸状态
/************************************************************************/
/* 告警页面库结束                                                       */
/************************************************************************/


/************************************************************************/
/* 中文信息页面库                                                       */
/************************************************************************/
#define MSG_INFO_1      0x00700000       // 中文信息1 
#define MSG_INFO_2		0x00700001       // 中文信息2 
#define MSG_INFO_3		0x00700002       // 中文信息3 
#define MSG_INFO_4		0x00700003       // 中文信息4	
#define MSG_INFO_5		0x00700004       // 中文信息5	
#define MSG_INFO_6		0x00700005       // 中文信息6	
#define MSG_INFO_7		0x00700006       // 中文信息7	
#define MSG_INFO_8		0x00700007       // 中文信息8	
#define MSG_INFO_9		0x00700008       // 中文信息9	
#define MSG_INFO_10		0x00700009       // 中文信息10
/************************************************************************/
/* 中文信息页面库结束                                                   */
/************************************************************************/

#define TMN_CTRL_STAT 	0x00010101		//-终端当前的控制状态-
#define DNB_PARA		0x00430101		//-电能表参数-
#define PULSE_PARA		0x00400000		//-Kv Ki Kp-

#endif // GUI_DISP_TYPE
#endif
