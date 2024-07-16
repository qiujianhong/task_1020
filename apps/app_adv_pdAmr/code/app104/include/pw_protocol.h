/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief： 功能描述
* @date： 2020-01-03
*********************************************************************
*/


#ifndef __PW_PROTOCOL_H__
#define __PW_PROTOCOL_H__

#include "pw_redef.h"

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_MSC_VER)
#pragma warning(disable:4201)
#endif

/*******************************************************************************
* 大端小端定义
*******************************************************************************/
#define CN_LITTLE_ENDIAN        ( 0 )     // 小端模式
#define CN_BIG_ENDIAN           ( 1 )     // 大端模式

#define CN_BYTE_ORDER   ( CN_LITTLE_ENDIAN )    // 定义为小端模式

#if( CN_BYTE_ORDER == CN_BIG_ENDIAN )// 大端模式
    #define  CN_BYTE_LO         ( 1 )     // 低字节
    #define  CN_BYTE_HI         ( 0 )     // 高字节

    #define  CN_WORD_LO         ( 1 )     // 低字
    #define  CN_WORD_HI         ( 0 )     // 高字

    #define  CN_DWB4_LL         ( 3 )     // 最低字节
    #define  CN_DWB4_LH         ( 2 )     // 次低字节
    #define  CN_DWB4_HL         ( 1 )     // 次高字节
    #define  CN_DWB4_HH         ( 0 )     // 最高字节

#else //==== CN_LITTLE_ENDIAN    缺省为小端模式
    #define  CN_BYTE_LO         ( 0 )     // 低字节
    #define  CN_BYTE_HI         ( 1 )     // 高字节

    #define  CN_WORD_LO         ( 0 )     // 低字
    #define  CN_WORD_HI         ( 1 )     // 高字

    #define  CN_DWB4_LL         ( 0 )     // 最低字节
    #define  CN_DWB4_LH         ( 1 )     // 次低字节
    #define  CN_DWB4_HL         ( 2 )     // 次高字节
    #define  CN_DWB4_HH         ( 3 )     // 最高字节
#endif

/* 国网最新实施细则点表默认区间定义
		状态量信息 		1-4000 
		模拟量信息 		4001-6000
		控制量信息 		6001-6200 
		电能量信息 		6401-6500 
		参数信息        8001-9000
*/

/*国网最新实施细则对应用传输的优先级排序定义
		1 初始化结束 
		2 总召唤的应答数据（初始化）
		3 遥控命令的应答报文 
		4 事件顺序记录（TCOS） 
		5 总召唤的应答数据（非初始化）
		6 故障事件 
		7 时钟同步的应答报文 
		8 变化遥测 
		9 复位进程 
		10 文件召唤 
		11 文件传输 
		12 电能量召唤
*/

/*国网实施细则对历史文件及目录命名的定义
	序号     文件类型                          	目录名定义                    文件名定义                  文件名示例                      备注 
	1              SOE事件记录                 	HISTORY/SOE              soe.xxx
	2              遥控操作记录               	HISTORY/CO               co.xxx 
	3              极值数据                  	HISTORY/EXV              exvYYYYMMDD.xxx        exv20160810.msg 
	4              定点记录数据               	HISTORY/FIXPT            fixptYYYYMMDD.xxx      fixpt20160810.msg 
	5              日冻结电能量               	HISTORY/FRZ              frzYYYYMMDD.xxx        frz20160810.msg            15分钟、日冻结共97个点 
	6              功率反向电能量冻结值               HISTORY/FLOWREV 	     flowrev.xxx 
	7              日志 					    HISTORY/ULOG             ulog.xxx                                          

	注：文件名区分大小写,xxx表示后缀名，MSG格式文件后缀名为.msg，XML格式文件后缀名为.xml
*/

//遥信点表实际配置个数
extern uint16 g_nConfigYxNum;

//遥测点表实际配置个数
extern uint16 g_nConfigYcNum;

//遥控点表实际配置个数
extern uint16 g_nConfigYkNum;

//电量点表实际配置个数
extern uint16 g_nConfigEnNum;

//故障事件点表实际配置个数
extern uint16 g_nConfigFaultNum;

//参数点表实际配置个数
extern uint16 g_nConfigParaNum;


//----------------------功能选择开关，以下4个功能开关需要根据用的要求，是否放开--------------
#define YC_CHANGE_SEND_EN           1        //-遥测数据变化上送功能-
#define EN_CYC_SEND_EN              0        //-突发送电量上送功能-
#define YC_CYC_SEND_EN              0        //-循环上送数据功能-
#define BACK_SCAN_EN                0        //-背景扫描功能-


//按国网最新实施细则修改如下时间及计数201907
#define MAX_FRAMCOM_DELAY_T1   20  //发送帧未收到确认最大延时时间
#define MAX_FRAMCOM_DELAY_T2   10  //接收I帧最长给予确认的间隔时间
#define MAX_FRAMCOM_DELAY_T3   30  //联络检测时间间隔
#define IFRAMCOM_TIMES_MAX_K   12  //I帧未确认发送帧最大数量
#define IFRAMCOM_TIMES_MAX_W   8   //I帧未确认接收帧最大数量


#define YX_START_ADDR       	0x0001 	//遥信∶信息对象地址范围为1H~1000H。
#define MAX_YX_NUM                 100  //终端遥信数量，路数增加要考虑数据字典的增加
#define JDPROT1_START_ADDR  	0x1001 	/*继电保护1∶信息对象地址范围为1001H~2000H，对应带CP56Time2a时标的继电保护设备事件、
									      带CP56Time2a时标的继电保护设备成组启动事件、带CP56Time2a时标的继电保护设备成组输出
									      电路信息*/
#define JDPROT2_START_ADDR  	0x2001 	//继电保护2∶信息对象地址范围为2001H~3000H，为继电保护动作的故障电流区。
#define JDPROT3_START_ADDR  	0x3001 	//继电保护3∶信息对象地址范围为3001H~4000H，为继电保护定值设定区。
#define YC_START_ADDR       	0X4001 	//遥测∶信息对象地址范围为4001H~5000H。
#define MAX_YC_NUM                1024  //遥测数据数量，路数增加要考虑数据字典的增加
#define YK_START_ADDR 	    	0x6001	//遥控、升降地址范围为6001H~6200H。
#define SET_START_ADDR 		    0x6201	//设定地址范围为6201H~6400H。
#define ENERGY_START_ADDR 	    0x6401	//电能累计量地址范围为6401H~6600H。
#define MAX_ENERGY_NUM        	   500	//电量数据个数
#define STEP_START_ADDR 		0x6601	//步位置信息地址范围为6601H~6700H。
#define TRSBIT_START_ADDR 	    0x6701	//向被控站传送二进制信息地址范围为6701H~6800H。
#define BCD_START_ADDR 		    0x6801	//BCD码地址范围为6801H~6900H。
#define RTUREMOT_START_ADDR 	0x6901	//远动终端状态地址6901H。
#define FILE_START_ADDR 		0x6902	//文件传送地址6902H~7000H 
#define MAX_YK_NUM			        50	//遥控路数，路数增加要考虑数据字典的增加
#define FALT_START_ADDR         0x0101  //故障信息上报信息体地址开始点号
#define FAULT_MAX_NUM               80 //故障信息上报支持信息体最大个数
#define PARA_START_ADDR     	0x8001	//参数地址范围为8001H~9000H。
#define PARA_MAX_NUM               200  //参数配置最大个数


/****************************************************************/
//IEC104规约应用服务应答后续过程进程定义                
/***************************************************************/
#define STEP_ALL_DATA_CALL_ALL      0//总召测
#define STEP_SOE_CHANGE_SEND       	1//soe变位上报
#define STEP_YC_CHANGE_SEND         2//遥测变化上报
#define STEP_ENN_DATA_CALL_ALL      3//电量总召测
#define STEP_PRESS_RESET            4//进程复位上报初始化结束
#define STEP_DATA_READ              5//读数据
#define STEP_TIMER_SYN              6//时钟同步
#define STEP_REMOTE_CTRL_ACK        7//遥控应答
#define STEP_PARA_RES_ACK0          8//参数上报后续处理
#define STEP_PARA_RES_ACK1          9//参数上报后续处理
#define STEP_NEW_PARA_CALL_ACK      10//国网新规范召测参数后续处理
#define STEP_IDEL_ACK_SEND          11//通讯空闲发送链路测试
#define STEP_FAULT_CHANGE_SEND      12//故障信息上报
#define STEP_TIMING_TASK_SEND       13//实时数据上报任务
#define STEP_FROZEN_TASK_SEND       14//数据周期冻结与上报
#define STEP_TASK_DATA_CALL         15//任务召测响应
#define STEP_POINT_DATA_CALL        16//点召测
#define STEP_ALL_PARA_CALL          17//所有参数召测
#define STEP_SOME_PARA_CALL         18//部分参数召测
#define STEP_CONT_DEFAULT           0xaa//空闲
/****************************************************************/ 

/****************************************************************/
//IEC101、104规约电量总召应答过程进程定义                
/***************************************************************/
#define ENDATA_STEP_SEND_AC          0  //交采数据
#define ENDATA_STEP_SEND_NO_TIME     1  //不带时标电表数据
#define ENDATA_STEP_SEND_TIME        2  //带时标电表数据
#define ENDATA_STEP_SEND_STOP        3
#define ENDATA_STEP_SEND_DEFAULT    0xaa
/****************************************************************/ 

/****************************************************************/
//IEC101、104规约参数召唤应答过程进程定义                
/***************************************************************/
#define PARA_DATA_STEP_SEND             0  //参数
#define PARA_DATA_STEP_SEND_DEFAULT     0xaa
/****************************************************************/ 

/****************************************************************/
//IEC101、104规约总召 遥测应答过程进程定义                
/***************************************************************/
#define ALLDATA_STEP_SEND_YX_SUB     0  //遥信
#define ALLDATA_STEP_SEND_NVA        1  //归一化值
#define ALLDATA_STEP_SEND_SVA        2  //标度化值
#define ALLDATA_STEP_SEND_R          3  //短浮点数
#define ALLDATA_STEP_SEND_R_T        4  //短浮点数 带时标
#define ALLDATA_STEP_SEND_R_EN       5  //短浮点数 电量
#define ALLDATA_STEP_SEND_STR        6  //比特串
#define ALLDATA_STEP_SEND_DEFAULT    0xaa
/****************************************************************/ 

/****************************************************************/
//IEC101、104规约变化遥测上报 过程进程定义                
/***************************************************************/
#define YC_CHANGE_STEP_SEND_NVA        0  //归一化值
#define YC_CHANGE_STEP_SEND_SVA        1  //标度化值
#define YC_CHANGE_STEP_SEND_R          2  //短浮点数
#define YC_CHANGE_STEP_SEND_R_T        3  //短浮点数 带时标
#define YC_CHANGE_STEP_SEND_R_EN       4  //短浮点数 电量
#define YC_CHANGE_STEP_SEND_STR        5  //比特串
#define YC_CHANGE_STEP_SEND_DEFAULT    0xaa
/****************************************************************/ 

/****************************************************************/
//IEC101、104规约周期冻结遥测上报 过程进程定义                
/***************************************************************/
#define YC_TIMING_STEP_SEND_NVA        0  //归一化值
#define YC_TIMING_STEP_SEND_SVA        1  //标度化值
#define YC_TIMING_STEP_SEND_R          2  //短浮点数
#define YC_TIMING_STEP_SEND_R_T        3  //短浮点数(带时标)
#define YC_TIMING_STEP_SEND_EN         4  //电量
#define YC_TIMING_STEP_SEND_STR        5  //比特串
#define YC_TIMING_STEP_SEND_DEFAULT    0xaa
/****************************************************************/ 

/****************************************************************/
//IEC104规约应用服务应答总召/点召/任务召测过程进程定义                
/***************************************************************/
#define ALLDATA_STEP_SEND_YX        0
#define ALLDATA_STEP_SEND_YC        1
#define ALLDATA_STEP_SEND_STOP      2
#define ALLDATA_STEP_SEND_DEFAULT   0xaa
/****************************************************************/ 

/****************************************************************/
//IEC104规约应用服务周期冻结上报进程定义                
/***************************************************************/
#define TIMING_STEP_SEND_HEAD      0
#define TIMING_STEP_SEND_YX        1
#define TIMING_STEP_SEND_YC        2
#define TIMING_STEP_SEND_END       3
#define TIMING_STEP_SEND_STOP      4
#define TIMING_STEP_SEND_DEFAULT   0xaa
/****************************************************************/ 


/* **************************************************************/
//业务相关应用类型定义
#define APP_TYPE_NOMMAL           		0x00 //通用
#define APP_TYPE_FILE_ABOUT             0X01 //文件、参数相关
#define APP_TYPE_CTRL_SELECT_ACK       	0x02 //遥控选择确认、参数预置确认等，需带终端随机数
#define APP_TYPE_PARA_ACK               0X03 //参数确认等
#define APP_TYPE_CTRL_SELECT			0X05 //遥控选择、遥控撤销
#define APP_TYPE_CTRL_RUN				0x07 //遥控执行
#define APP_TYPE_UPDATA				    0x08 //升级相关

/****************************************************************/
//IEC101、104规约应用服务数据单元之类型标识                
/***************************************************************/
//          类型标识      序号		 定义
#define		M_SP_NA_1 	1		//单点信息                                       
#define		M_SP_TA_1 	2		//带时标的单点信息                               
#define		M_DP_NA_1  	3		//双点信息                                      
#define		M_DP_TA_1 	4		//带时标的双点信息                                
#define		M_ST_NA_1  	5		//步位置信息                                    
#define		M_ST_TA_1	6		//带时标的步位置信息                               
#define		M_BO_NA_1	7		//32比特串                                        
#define		M_BO_TA_1	8		//带时标的32比特串                                
#define		M_ME_NA_1	9		//测量值, 规一化值                                  
#define		M_ME_TA_1  	10		//测量值，带时标的规一化值                        
#define		M_ME_NB_1	11		//测量值, 标度化值                                 
#define		M_ME_TB_1  	12		//测量值, 带时标的标度化值   
#define		M_ME_NC_1  	13		//测量值, 短浮点数  
#define		M_ME_TC_1  	14		//测量值, 带时标的短浮点数                        
#define		M_IT_NA_1	15		//累计量                                            
#define		M_IT_TA_1  	16		//带时标的累计量                                  
#define		M_EP_TA_1  	17		//带时标的继电保护设备事件                        
#define		M_EP_TB_1  	18		//带时标的继电保护设备成组启动事件                
#define		M_EP_TC_1  	19		//带时标的继电保护设备成组输出电路信息            
#define		M_PS_NA_1  	20		//带变位检出的成组单点信息                        
#define		C_RE_NA_1  	20		//故障事件(浙江, 用于响应事件召测)
#define		M_ME_ND_1  	21		//测量值, 不带品质描述词的规一化值  
#define		M_SP_TB_1  	30		//带CP56Time2a时标的单点信息      
#define		M_DP_TB_1  	31		//带CP56Time2a时标的双点信息                      
#define		M_ST_TB_1  	32		//带CP56Time2a时标的步位置信息                    
#define		M_BO_TB_1	33		//带CP56Time2a时标的32比特串      
#define		M_ME_TD_1  	34		//带CP56Time2a时标的测量值, 规一化值   
#define		M_ME_TE_1 	35		//带CP56Time2a时标的测量值, 标度化值               
#define		M_ME_TF_1	36		//带CP56Time2a时标的测量值, 短浮点数                
#define		M_IT_TB_1	37		//带CP56Time2a时标的累计量                          
#define		M_EP_TD_1	38		//带CP56Time2a时标的继电保护设备事件                
#define		M_EP_TE_1	39		//带CP56Time2a时标的继电保护设备成组启动事件        
#define		M_EP_TF_1  	40		//带CP56Time2a时标的继电保护设备成组输出电路信息  
#define		M_VBO_TB_1  41      //带CP56Time2a时标的变长比特串(浙江)
#define		M_FT_NA_1   42      //国网新实施细则定义的故障信息上报类型
#define		C_SC_NA_1  	45		//单点命令                                        
#define		C_DC_NA_1  	46		//双点命令                                        
#define		C_RC_NA_1  	47		//步调节命令                                      
#define		C_SE_NA_1	48		//设定值命令, 规一化值   
#define		C_SE_NB_1	49		//设定值命令, 标度化值                             
#define		C_SE_NC_1	50		//设定值命令, 短浮点数      
#define		C_BO_NA_1	51		//32比特串      
#define		M_FT_NA_ZHJ_1	53		//故障事件信息(浙江)
#define     C_SC_TA_1   58      //带CP56Time2a时标的单点命令,104特有
#define     C_DC_TA_1   59      //带CP56Time2a时标的双点命令,104特有
#define     C_RC_TA_1   60      //带CP56Time2a时标的调节步命令,104特有
#define     C_SE_TA_1   61      //带CP56Time2a时标的设定值命令, 规一化值,104特有
#define     C_SE_TB_1   62      //带CP56Time2a时标的设定值命令, 标度化值,104特有
#define     C_SE_TC_1   63      //带CP56Time2a时标的设定值命令, 短浮点数,104特有
#define     C_BO_TA_1   64      //带CP56Time2a时标的32比特串,104特有
#define		M_EI_NA_1	70		//初始化结束                                       
#define		C_IC_NA_1  	100		//总召唤命令                                                                  
#define		C_CI_NA_1  	101		//计数量召唤命令                                 
#define		C_RD_NA_1   102		//读命令                                         
#define		C_CS_NA_1  	103		//时钟同步命令                                   
#define		C_TS_NA_1  	104		//测试命今                                       
#define		C_RP_NA_1  	105		//复位进程命令
#define		C_CD_NA_1  	106		//延时获得命今     
#define		C_TS_TA_1   107     //带CP56Time2a时标的测试命令,104特有
#define		P_ME_NA_1  	110		//测量值参数, 规一化值                             
#define		P_ME_NB_1  	111		//测量值参数, 标度化值                             
#define		P_ME_NC_1   112		//测量值参数, 短浮点数                                   
#define		P_AC_NA_1  	113		//参数激活                                         
#define		F_FR_NA_1  	120		//文件淮备就绪                                     
#define		F_SR_NA_1  	121		//节淮备就绪                                        
#define		F_SC_NA_1  	122		//召唤目录, 选择文件, 召唤文件，召唤节               
#define		F_LS_NA_1  	123		//最后的节,最后的段                                  
#define		F_AF_NA_1  	124		//认可文件,认可节                                    
#define		F_SG_NA_1	125		//段                                                 
#define		F_DR_TA_1	126		//目录                                               
#define		C_SR_NA_1   200     //国网最新实施细则参数切换类型标识
#define		C_RR_NA_1   201     //国网最新实施细则参数读定值区号类型标识
#define		C_RS_NA_1   202     //国网最新实施细则读单个或多个定值/参数类型标识
#define		C_WS_NA_1   203     //国网最新实施细则写单个或多个定值/参数类型标识
#define		C_TC_NA_ZHJ_1   204     //任务召唤命令(浙江)
#define		C_PC_NA_ZHJ_1   205     //点召唤命令(浙江)
#define		M_IT_NB_1   206     //国网最新实施细则累计量，短浮点数
#define		M_IT_TC_1   207     //国网最新实施细则带CP56Time2a时标的累计量，短浮点数
#define		C_EC_NA_ZHJ_1   208     //事件召唤命令(浙江)
#define		M_TK_NA_ZHJ_1   209     //任务上送(冻结)(浙江)
#define		F_SC_NA_2   210     //国网最新实施细则文件操作类型标识
#define		F_SR_NA_1_N 211     //国网最新实施细则软件升级启动/结束类型标识


//          传送原因                序号		定义
#define  	CAUSE_PERCYC		1		//周期、循环                                     
#define  	CAUSE_BACK        	2		//背景扫描
#define  	CAUSE_SPONT 		3		//突发(自发)  <山东定义为发送数据>
#define  	CAUSE_INIT 			4		//初始化
#define  	CAUSE_REQ 			5		//请求或者被请求
#define  	CAUSE_ACT			6		//激活
#define  	CAUSE_ACTCON		7		//激活确认
#define  	CAUSE_DEACT			8		//仃止激活                                        
#define  	CAUSE_DEACTCON		9		//仃止激活确认
#define  	CAUSE_ACTTERM		10		//激活终止
#define  	CAUSE_RETREM		11		//远方命令引起的返送信息
#define  	CAUSE_RETLOC		12		//当地命令引起的返送信息                            
#define  	CAUSE_FILE			13		//文件传输                                        
#define  	CAUSE_INTROGEN 		20		//响应站总召唤
#define     CAUSE_REQCOGEN      37      //响应计数量总召唤
#define  	ERROR_TYPE			44		//未知的类型标识
#define  	ERROR_CAUSE			45		//未知的传送原因
#define  	ERROR_ADDR			46		//未知的应用服务数据单元公共地址
#define  	ERROR_INFO			47		//未知的信息对象地址
#define  	CAUSE_PTCALLGEN			51		//响应点召唤(浙江)
#define  	CAUSE_TKCALLGEN			52		//响应任务召唤(浙江)
#define  	CAUSE_FTCALLGEN			53		//响应事件召唤(浙江)


/*******************************************************************************
* 104报文 定值参数设置 TLV数据类型表定义
*******************************************************************************/
#define TAG_NONE     0
#define TAG_BOOL     1
#define TAG_INT8     43
#define TAG_UINT8    32
#define TAG_INT16    33
#define TAG_UINT16   45
#define TAG_INT32    2
#define TAG_UINT32   35
#define TAG_INT64    36
#define TAG_UINT64   37
#define TAG_FLOAT32  38
#define TAG_FLOAT64  39
#define TAG_STRING   4
#define TAG_OCTSTRING   4
#define TAG_OCTSTRING_ZHJ   8

/*******************************************************************************
* 遥测/电量变化判定门槛值
*******************************************************************************/
//通用值
#define DEAD_VALUE_1             1    //门槛值为1
#define DEAD_VALUE_2             2    //门槛值为2
#define DEAD_VALUE_5             5    //门槛值为5
#define DEAD_VALUE_10           10    //门槛值为10
#define DEAD_VALUE_100         100    //门槛值为100
#define DEAD_VALUE_1000       1000    //门槛值为1000
#define DEAD_VALUE_10000     10000    //门槛值为10000

//遥测类参数
#define DEAD_ZONE_I                (0.05)    //电流死区(0-0.3)。默认值0.05（ct变比600/5）；默认值0.01（ct变比600/1）
#define DEAD_ZONE_AC_U             (0.01)    //交流电压死区(0-0.3)
#define DEAD_ZONE_DC_U             (0.01)    //直流电压死区(0-0.3)
#define DEAD_ZONE_P                (0.05)    //功率死区(0-0.3)
#define DEAD_ZONE_FR               (0.005)   //频率死区(0-0.3)
#define DEAD_ZONE_PF               (0.02)    //功率因数死区(0-0.3)
#define FIXED_VALUE_PT1              (10.0)    //pt一次额定kV(0.1-30.0)
#define FIXED_VALUE_PT2              (220.0)   //pt二次额定V(0.1-400.0实际值)
#define FIXED_VALUE_PH_CT1           (600.0)   //相ct一次额定A(1.0-2000.0)
#define FIXED_VALUE_PH_CT2           (1.0)     //相ct二次额定A(1.0或5.0)
#define FIXED_VALUE_ZERO_CT1         (20.0)    //零序ct一次额定A(1.0-500.0)
#define FIXED_VALUE_ZERO_CT2         (1.0)     //零序ct二次额定A(1.0或5.0)

//越限类参数
#define FIXED_U_VALUE              (220.0)    //额定电压V
#define FIXED_I_VALUE              (1.5)      //额定电压A

#define LOW_U_LIMIT_VALUE          (0.9*FIXED_U_VALUE)        //低电压报警门限值V(0.1*FIXED_U_VALUE-2.0*FIXED_U_VALUE)
#define LOW_U_DELAY_TIME           (600.0)    //低电压报警延时s(0.0-10000.0)
#define OVER_U_LIMIT_VALUE         (1.1*FIXED_U_VALUE)        //过电压报警门限值V(0.1*FIXED_U_VALUE-2.0*FIXED_U_VALUE)
#define OVER_U_DELAY_TIME          (600.0)    //过电压报警延时s(0.0-10000.0)

#define HEAVY_LOAD_LIMIT_VALUE     (0.7*FIXED_I_VALUE)        //重载报警门限值A(0.1*FIXED_U_VALUE-2.0*FIXED_U_VALUE)
#define HEAVY_LOAD_DELAY_TIME      (3600.0)   //重载报警延时s(0.0-10000.0)
#define OVER_LOAD_LIMIT_VALUE      (1.0*FIXED_I_VALUE)        //过载报警门限值A(0.1*FIXED_U_VALUE-2.0*FIXED_U_VALUE)
#define OVER_LOAD_DELAY_TIME       (3600.0)   //过载报警延时s(0.0-10000.0)

//遥信类参数
#define YX_GET_TIME                (0.2)      //开入量采集防抖时间s(0.01-60.0)

//遥控类参数
#define OPEN_KEEP_TIME             (0.5)      //分闸输出脉冲保持时间s(0.01-50.0)
#define CLOSE_KEEP_TIME            (0.5)      //合闸输出脉冲保持时间s(0.01-50.0)


/*******************************************************************************
* 终端和计量点参数类型定义
*******************************************************************************/
#define TMN_RUN_PARA      0x10    /*--终端运行参数类型--*/
#define TMN_FUNC_PARA     0x20    /*--终端功能参数类型--*/
#define MTR_PARA_TYPE     0x30    /*--计量点参数类型--*/
#define TMN_PARA_EXTEND   0x40      //扩展终端参数
#define MTR_PARA_EXTEND   0x50      //扩展测量点参数

typedef enum
{
	DA_PARA_CURRENT_PORT,
	DA_PARA_ASPECT,
}FRAME_PARA_TYPE_E;

/****************************************************************/
//全局变量配置区
/***************************************************************/


/*******************************************************************************
* 104报文相关数组长度定义
*******************************************************************************/

#define TOTAL_FRAME_LEN_MAX  255  //一帧完整104报文最大长度

//104报文变化帧结构长度参数
#define CTRL_LEN  4  //4控制域
#define TI_LEN  1  //1类型
#define VSQ_LEN  1  //1可变
#define COT_LEN  2  //2原因
#define PUB_ADDR_LEN  2  //2公共地址
#define OBJ_ADDR_LEN  3  //3信息对象地址

#define APCI_LEN  6  //6APCI长度
#define UNIT_LABEL_LEN (TI_LEN + VSQ_LEN + COT_LEN + PUB_ADDR_LEN)  //ASDU数据单元标识UNIT长度
#define APCI_UNIT_LABEL_LEN (APCI_LEN + UNIT_LABEL_LEN)  //APCI+数据单元标识UNIT长度
#define CTRL_UNIT_LABEL_LEN (CTRL_LEN + UNIT_LABEL_LEN)  //控制域+数据单元标识UNIT长度


//255 - APCI长度 - ASDU数据单元标识UNIT长度
#define MAX_INFO_OBJ_DATA_LEN (255 - APCI_UNIT_LABEL_LEN)

/*******************************************************************************
* 数组长度定义
*******************************************************************************/
#define MAX_YK_BUFF_LEN (30)  //遥控报文长度
#define MAX_FAULT_SIGNAL_NUM (30)  //故障事件允许上报的最大数据个数

/*******************************************************************************
* 104参数定值对应面向对象结构体
*******************************************************************************/
//死区参数0x49010200
typedef struct tag_OOP_DEAD_ZONE
{
	int32			nI;  //电流死区
	uint32			nAcU;  //交流电压死区
	uint32			nDcU;  //直流电压死区
	int32			nPf;  //功率死区
	int16			nFr;  //频率死区
}OOP_DEAD_ZONE_T;

//PT参数0x49020200
typedef struct tag_OOP_PT
{
	uint16			nFir;  //一次额定
	uint16			nSec;  //二次额定
}OOP_PT_T;

//CT参数0x49030200
typedef struct tag_OOP_CT
{
	int32			nPhFir;  //相CT一次额定
	int32			nPhSec;  //相CT二次额定
	int32			nZeroFir;  //零序CT一次额定
	int32			nZeroSec;  //零序CT二次额定
}OOP_CT_T;

//低电压报警0x49040200
typedef struct tag_OOP_LOW_U
{
	uint32			nLimit;  //报警门限值
	uint16			nDelay;  //报警延时
}OOP_LOW_U_T;

//过电压报警0x49050200
typedef struct tag_OOP_OVER_U
{
	uint32			nLimit;  //报警门限值
	uint16			nDelay;  //报警延时
}OOP_OVER_U_T;

//重载报警0x49060200
typedef struct tag_OOP_HEAVY_LOAD
{
	int32			nLimit;  //报警门限值
	uint16			nDelay;  //报警延时
}OOP_HEAVY_LOAD_T;

//过载报警0x49070200
typedef struct tag_OOP_OVER_LOAD
{
	int32			nLimit;  //报警门限值
	uint16			nDelay;  //报警延时
}OOP_OVER_LOAD_T;

//开入量采集防抖动时间0x49080200
typedef struct tag_OOP_YX_GET_TIME
{
	uint32			nTime;  //报警门限值
}OOP_YX_GET_TIME_T;

//分闸输出脉冲保持时间0x49090200
typedef struct tag_OOP_PULSE_TIME
{
	uint32			nOpenTime;  //分闸输出脉冲保持时间
	uint32			nCloseTime;  //分闸输出脉冲保持时间
}OOP_PULSE_TIME_T;

//蓄电池自动活化周期0x490A0200
typedef struct tag_OOP_AUTO_ACT_CYCLE
{
	uint32			nCycle;  //自动活化周期
	uint32			nTime;  //自动活化时刻
}OOP_AUTO_ACT_CYCLE_T;

//电池电压低报警阈值0x490B0200
typedef struct tag_OOP_BATT_LOW_U
{
	uint16			nLimitU;  //电池电压低报警阈值
}OOP_BATT_LOW_U_T;

//短路故障报警0x490C0200
typedef struct tag_OOP_SHORT_FAULT
{
	int32			nLimitI;  //突变电流阈值
	uint32			nTime;  //持续时间
}OOP_SHORT_FAULT_T;

//录波电场下降比例0x490E0200
typedef struct tag_OOP_RECORD_DROP
{
	uint16			nRatio;  //录波电场下降比例
}OOP_RECORD_DROP_T;

//永久故障0x490F0200
typedef struct tag_OOP_RESET_TPYE
{
	uint8			nType;  //0：上电复位，1：定时复位
}OOP_RESET_TPYE_T;

//永久故障0x490F0300
typedef struct tag_OOP_RESET_TIME
{
	uint32			nResetTime;  //上电复归时间
	uint32			nConfirmTime;  //确认时间
}OOP_RESET_TIME_T;

//负荷电流超限阈值0x49100200
typedef struct tag_OOP_LOAD_I_LIMIT
{
	int32			nLimit;  //上电复归时间
}OOP_LOAD_I_LIMIT_T;

//过电流报警阈值0x49110200
typedef struct tag_OOP_OVER_I_LIMIT
{
	int32			nLimit;  //上电复归时间
}OOP_OVER_I_LIMIT_T;

//相线路有电阈值0x49120200
typedef struct tag_OOP_PHASE_LINE_POWER
{
	uint16			nLimit;  //上电复归时间
}OOP_PHASE_LINE_POWER_T;

//故障指示灯自动复归0x49130200
typedef struct tag_OOP_FAULT_TUBE_RESET
{
	BOOL			bWork;  //是否投入
	uint32			nTime;  //复归时间
}OOP_FAULT_TUBE_RESET_T;

//故障遥信保持时间0x49140200
typedef struct tag_OOP_FAULT_YX_KEEP
{
	uint32			nTime;  //保持时间
}OOP_FAULT_YX_KEEP_T;

//过流参数（过流I段）0x49160200
//过流参数（过流II段）0x49160300
//过流参数（过流III段）0x49160400
//过流参数（零序过流）0x49160500
typedef struct tag_OOP_OVER_I_PARA
{
	BOOL			bWarnWork;  //告警投入
	BOOL			bExitWork;  //出口投入
	int32			nValue;  //定值
	uint32			nTime;  //时间
}OOP_OVER_I_PARA_T;



/*******************************************************************************
* 104报文枚举/结构体定义
*******************************************************************************/

//I帧发送序列号/接收序列号结构
typedef struct
{
	uint16	fir:1;			//-I帧类型标志-
	uint16	framenum:15;	//-帧序号-
}__attribute__ ((packed)) NUMBER_BIT_I;

//-通用发送序号
typedef union s_num_union
{
	NUMBER_BIT_I ibit;
	struct
	{
		uint16	fir:2;			//-S帧类型标志-
		uint16	framenum:14;	//-无效-
	}__attribute__ ((packed)) sbit;
    struct
	{
		uint16	fir:2;			//-U帧类型标志-
		uint16	staact:1;	    //-start act标志位-
		uint16	stacon:1;	    //-start con标志位-
		uint16	stoact:1;	    //-stop act标志位-
        uint16	stocon:1;	    //-stop con标志位-
        uint16	testact:1;	    //-test act标志位-
		uint16	testcon:1;	    //-test con标志位-
		uint16	framenum:8;	    //-无效-
	}__attribute__ ((packed)) ubit;
	uint8 crtltype12[2];
}__attribute__ ((packed)) S_NUMBER_BIT_UN;//-发送序号

//-通用接收序号
typedef union r_num_union
{
	NUMBER_BIT_I rbit; //I帧和S帧接收序号
	uint8  crtltype34[2];  //u帧为0
}__attribute__ ((packed)) R_NUMBER_BIT_UN;//-接收序号

//104规约帧数据结构
typedef struct
{
	uint8			nHead;		      //位置0，68报文头
	uint8           nLength;          //位置1，报文长度
    S_NUMBER_BIT_UN   	sCrtlType12;      //位置2，控制域1、2
	R_NUMBER_BIT_UN     sCrtlType34;      //位置4，控制域3、4
	uint8           nDataType;        //位置6，类型标识,1个字节
	uint8           nVSQ;		      //位置7，可变结构限定词,1个字节
	uint8           nTransCauseL;     //位置8，传送原因，2字节，这个是低字节
	uint8           nTransCauseH;     //位置9，传送原因，2字节，这个是高字节
	uint8           nPubAddrL;	      //位置10，公共地址，1字节，这个是低字节
	uint8           nPubAddrH;	      //位置11，公共地址，1字节，这个是高字节
    uint8           aObjData[MAX_INFO_OBJ_DATA_LEN];       //位置12，信息体
}__attribute__ ((packed)) TOTAL_104_FRAME_T;


//遥信配置信息结构体
typedef struct
{
    uint8  nObjAddrHB;    //信息对象地址（最高字节）
	uint16 nObjAddr;      //信息对象地址（加上起始地址的实际值）
	uint32 nObjAddrU32;   //信息对象地址（完整3个字节）
	char   *szObjName;    //信息对象名称
	uint32 nSignalType;   //类型标识
	uint32 nDbCenterOAD;  //数据中心查询标签（OAD）
	uint32 nOadColsNum;  //关联次OAD数量
	uint32 aOadCols[MAX_OAD_COLS_NUM];  //关联次OAD
	uint8  nItemIndex;    //分项序号（从0开始）
	uint32 nDealType;  //特殊处理类型(非直接从数据中心获取, 人工处理)
}YX_DATA_CONFIG;

//遥信实时数据结构体（用于上报）
typedef struct
{
	uint8  nNewValue;     //实时数据
	uint8  nHisValue;     //历史数据
	BOOL bSendFlag;       //是否上报
	BOOL bChangeFlag;     //是否变化
	BOOL bNewValidFlag;   //实时数据是否有效
	BOOL bHisValidFlag;   //历史数据是否有效
	uint8   btime[7];     //-数据发生变化的时间-
}YX_DATA_SEND;

//遥测配置信息结构体
typedef struct
{
    uint8  nObjAddrHB;    //信息对象地址（最高字节）
	uint16 nObjAddr;      //信息对象地址（加上起始地址的实际值）
	uint32 nObjAddrU32;   //信息对象地址（完整3个字节）
	char   *szObjName;    //信息对象名称
	uint32 nSignalType;   //类型标识
	int8 nReportRatio;    //当前数据上送主站的倍率
	uint16 nDeadZoneAddr; //变化死区值的对象地址
	float32 fDeadZoneValue; //变化死区值
	uint8 nLogicId; //逻辑地址，交采：1，电表：0
    uint16 nInfoNum; //信息点号  ，交采：0，电表：从1开始，当信息点号为0xFFFF时，表示全部信息点
	uint32 nDbCenterOAD;  //数据中心查询标签（主OAD）
	uint32 nOadColsNum;  //关联次OAD数量
	uint32 aOadCols[MAX_OAD_COLS_NUM];  //关联次OAD
	uint8  nItemIndex;    //分项序号（从0开始）
	float32 fFullValue;  //满码值（用于归一化遥测值）
	uint32 nDealType;  //特殊处理类型(非直接从数据中心获取, 人工处理)
	uint8 nArrLen;       //不需处理的数组参数长度(比特串)
}YC_DATA_CONFIG;

//遥测变长比特串最大长度
#define MAX_YC_STRING_LEN  100

//遥测实时数据结构体（用于上报）
typedef struct
{
	float32 fNewValue;    //实时数据
	float32 fHisValue;    //历史数据
	uint8  nNewArrValueNum;     //实时比特串数据 个数
	uint8  nHisArrValueNum;     //历史比特串数据 个数
	uint8  aNewArrValue[MAX_YC_STRING_LEN];     //实时比特串数据
	uint8  aHisArrValue[MAX_YC_STRING_LEN];     //历史比特串数据
	BOOL bSendFlag;       //是否上报
	BOOL bChangeFlag;     //是否变化
	BOOL bNewValidFlag;   //实时数据是否有效
	BOOL bHisValidFlag;   //历史数据是否有效
	uint8   btime[7];     //-数据发生变化的时间-
}YC_DATA_SEND;

//电量配置信息结构体
typedef struct
{
    uint8  nObjAddrHB;    //信息对象地址（最高字节）
	uint16 nObjAddr;      //信息对象地址（加上起始地址的实际值）
	char   *szObjName;    //信息对象名称
	uint32 nSignalType;   //类型标识
	int8 nReportRatio;    //当前数据上送主站的倍率
	uint16 nDeadZoneAddr; //变化死区值的对象地址
	float32 fDeadZoneValue; //变化死区值
	uint8 nLogicId; //逻辑地址，交采：1，电表：0
    uint16 nInfoNum; //信息点号  ，交采：0，电表：从1开始，当信息点号为0xFFFF时，表示全部信息点
	uint32 nDbCenterOAD;  //数据中心查询标签（主OAD）
	uint32 nOadColsNum;  //关联次OAD数量
	uint32 aOadCols[MAX_OAD_COLS_NUM];  //关联次OAD
	uint8  nItemIndex;    //分项序号（从0开始）
	uint32 nDealType;  //特殊处理类型(非直接从数据中心获取, 人工处理)
}EN_DATA_CONFIG;

//电量实时数据结构体（用于上报）
typedef struct
{
	float32 fNewValue;    //实时数据
	float32 fHisValue;    //历史数据
	BOOL bSendFlag;       //是否上报
	BOOL bChangeFlag;     //是否变化
	BOOL bHisValidFlag;   //历史数据是否有效
	uint8   btime[7];     //-数据发生变化的时间-
}EN_DATA_SEND;



//故障信息上报参数数据结构，上报按带时标的单点遥信上报
typedef struct 
{
    uint8  nObjAddrHB;    //信息对象地址（最高字节）
	uint16 nObjAddr;      //信息对象地址（加上起始地址的实际值）
	uint32 nObjAddrU32;   //信息对象地址（完整3个字节）
	char   *szObjName;    //信息对象名称
	uint8  nEuipType;     //设备类型（用以确定遥信的所属）
	uint32 nSignalType;   //类型标识（故障信息）
	uint32 nFaultYxAddr;  //故障遥信地址(复用事件的判断逻辑)
	uint8  nRptDataSrc;    //上报数据来源（来自哪个设备）
	uint8  nReportNum;      //上报数据个数(总个数)
	uint8  nReportYxNum;      //上报数据个数(遥信个数)
	uint8  nReportYcNum;      //上报数据个数(不带时标 遥测个数)
	uint8  nReportYcTiNum;      //上报数据个数(带时标 遥测个数)
	uint8  nReportEnNum;      //上报数据个数(电能量个数)
	uint8  nReportStrNum;      //上报数据个数(比特串个数)
	uint8  nReportParaNum;      //上报数据个数(参数个数 仅用于参数变更事件)
	uint16 aReportAddr[MAX_FAULT_SIGNAL_NUM]; //上报信息体地址(取点表信息体地址的存放位置)
	uint32 aReportAddrU32[MAX_FAULT_SIGNAL_NUM]; //上报信息体地址(取点表信息体地址的存放位置)
	uint32 nDbCenterOAD;  //数据中心查询标签（OAD）
	uint8  nItemIndex;    //分项序号（从0开始）
}FAULT_REPORT_CONFIG;

//故障信息上报参数数据结构（实时），上报按带时标的单点遥信上报
typedef struct 
{
	BOOL bSendFlag;       //是否上报（事件）
	uint8  nNewValue;     //实时数据（遥信）
	uint8  nHisValue;     //历史数据（遥信）
	BOOL bChangeFlag;     //是否变位（遥信）
	uint8 btime[7];       //变位发生时间（遥信）
	uint8  nYxType;       //遥信数据类型（遥信）
	uint8  nReportNum;      //上报遥测数据个数
	uint8  nReportType;     //上报遥测数据类型(归一化值或短浮点数)
	uint8  aReportData[255]; //上报数据缓冲区(按序存放故障时刻的遥测点号+数值)
}FAULT_REPORT_SEND;


//遥控步骤
#define CTRL_SELECT 0  //-遥控预置-
#define CTRL_RUN    1  //-遥控执行-
#define CTRL_STOP   2  //-遥控撤销-

//参数设置步骤
#define PARA_SELECT 0  //-参数预置-
#define PARA_RUN    1  //-参数固化-
#define PARA_STOP   2  //-参数预置撤销-

//遥控数据结构
typedef struct
{
    uint8  nObjAddrHB;    //信息对象地址（最高字节）
	uint16 nObjAddr;      //信息对象地址（加上起始地址的实际值）
	char   *szObjName;    //信息对象名称
	uint32 nSignalType;   //类型标识
	uint32 nDbCenterOAD;  //数据中心查询标签（OAD）
	uint8  nItemIndex;    //分项序号（从0开始）
	uint8  nSelect;	      //遥控路数<每一路均包含该路的--分--合-->
}YK_DATA_CONFIG;

//遥控实时数据结构体（用于上报）
typedef struct
{
	uint8  nNewValue;     //实时数据
	uint8  nHisValue;     //历史数据
	BOOL bSendFlag;       //是否上报
	BOOL bChangeFlag;     //是否变化
	BOOL bHisValidFlag;   //历史数据是否有效
	uint8   btime[7];     //-数据发生变化的时间-
}YK_DATA_SEND;


//远程参数读写-参数特征标识PI结构体
typedef union para_iden_union
{
	uint8 nParaIden;
	struct
	{
		uint8	cont:1;			//后续状态位
		uint8	res:5;			//保留RES=RESERVE
		uint8	cr:1;			//CR 未用/取消预置
		uint8	se:1;			//S/E 固化/预置
	}__attribute__ ((packed)) unParaIden;
}__attribute__ ((packed)) PARA_IDENTIFY_UN;

//定值区数量
#define MAX_FIXED_AREA_NUM  2

//定值区特征标识结构体
typedef struct
{
	uint16 nFixedAreaID;  //定值区号SN
	PARA_IDENTIFY_UN unParaIdentify;  //参数特征标识
	uint8  nSetState;  //0x55已预置, 0xaa已固化/撤销
	uint8  aSetPara[255];  //参数设置报文(首字节为标志0x55/0xaa, 次字节为个数, 后面地址+tag+长度+值)
}__attribute__ ((packed)) FIXED_AREA_IDEN_T;

//定值参数数据长度（考虑字符串 最长64字节 以\0结尾）
#define MAX_FIXED_VALUE_LEN  64

//定值参数设定涉及的数据类型联合体
typedef union
{
	uint8  arrayV[MAX_FIXED_VALUE_LEN];           //数据value
	BOOL   boolV;
	int8   int8V;
	uint8  uint8V;
	int16  int16V;
	uint16 uint16V;
	int32  int32V;
	uint32 uint32V;
	int64  int64V;
	uint64 uint64V;
	float32 float32V;
	float64 float64V;
}VALUE_TPYE_UN;

//定值区数量
#define MAX_ARR_VALUE_LEN  260

//定值参数设定结构体（主要用于报文）
typedef struct
{
    BOOL bModifiable;     //可修改的
    uint8  nObjAddrHB;    //信息对象地址（最高字节）
	uint16 nObjAddr;      //信息对象地址
	uint32 nObjAddrU32;   //信息对象地址（完整3个字节）
	uint8  res;           //信息对象地址（高字节保留）
	uint8  nTagType;      //Tag类型
	uint8  nDataLen;      //数据长度
	VALUE_TPYE_UN unValue;//数据value
	BOOL   bValid;        //数据value是否有效
	float  fDefaultValue; //默认值
	int64  nDefaultValueI64; //默认值(整型)
	float  fMinValue;     //最小值
	float  fMaxValue;     //最大值
	uint32 nDealType;     //特殊处理类型(非直接从数据中心获取, 人工处理)
	uint8  nUpdateFlag;   //是否更新已有的私有文件内容
	uint16 nUpdateArea;   //更新的定值区号对应参数
	uint16 nArrLen;       //不需处理的数组参数长度
	uint8  aArrValue[MAX_ARR_VALUE_LEN];     //不需处理的数组参数
}PARA_SET_VALUE_T;

//参数配置一个OAD最多包含104信息体个数
#define MAX_OOP_ADDR_NUM  5

//104报文 与 面向对象 对应关系
typedef struct{
	OOP_OAD_U  Oad;          //OAD
	int8   DesScaler;        //0AD对应的换算单位
	uint32  DI07;	         //07协议数据标识
	uint8   DataLen;	     //645 每个子元素的长度
	uint8   TotalDataLen;	 //645 总长度
	uint16  DI97;	         //97协议数据标识
    DATASTRUCT  CovertSruct; //要转成的结构体类型
    int8   Scaler;           //645小数点位数
    uint16  Addr[MAX_OOP_ADDR_NUM];  //关联的104信息体个数
    int8    InfoSca[MAX_OOP_ADDR_NUM]; //关联的104信息体小数点位数
}Map104ToOop;

// 面向对象值 转换系数方式
typedef enum
{   
    E_OOP_TO_REAL=0,  //面向对象值 转换为 真实值
	E_REAL_TO_OOP,  //真实值 转换为 面向对象值
	E_DISPATCH_TO_REAL,  //调度值 转换为 真实值
	E_REAL_TO_DISPATCH,  //真实值 转换为 调度值
}CONVERT_TYPE_E;


// 104任务执行流程全局变量
typedef struct
{
	uint8 nTaskStep;  //任务流程
    uint8 nDataCallStep;  //一级数据上报
    uint16 nStartNum;
	uint8 nSubDataCallSetp;  //二级数据上报
	uint16 nSubStartNum;
    uint8 bFirstRun;
    uint16 nEquipTypeIndex;  //设备类型(数组下标)
    uint16 nEquipGroupIndex;  //设备组别(数组下标)
    uint16 nEquipNoIndex;  //设备序号(数组下标)
    uint16 nEqTaskNoIndex;  //数据冻结与上报任务序号(数组下标)
    uint16 nTaskPubAddrIndex;  //数据冻结与上报任务的公共地址(数组下标)
    uint16 nEqSaveNoIndex;  //历史冻结数据的存储序号(数组下标)
    uint8 nFinishFlag;  //结束标志
    uint8 nLastDataCallStep;  //一级数据上报(备份)
    uint8 nLastSubDataCallSetp;  //二级数据上报(备份)

    uint8 aTempBuff[TOTAL_FRAME_LEN_MAX];//缓存报文 最大255
    uint8 nTempBuffLen;//缓存报文长度 最大255
}TASK_RUN_STEP_T;

// 上报任务对应通道号
typedef struct
{
	uint8 nAllDataCallPipe;  //总召唤通道号
	uint8 nPointCallPipe;  //点召唤通道号
	uint8 nTaskCallPipe;  //任务召唤通道号
    uint8 nEnDataCallPipe;  //电能量召唤通道号
    uint8 nProcessResetPipe;  //进程复位通道号
    uint8 nTimeSynPipe;  //时钟同步通道号
    uint8 nRemoteCtrlAckPipe;  //应答遥控结束通道号
}TASK_REPORT_PIPE_T;



/*******************************************************************************
* 信号配置与发送变量声明
*******************************************************************************/
extern YX_DATA_CONFIG g_aYxDataConfig[MAX_YX_NUM];
extern YX_DATA_SEND   g_aYxDataSend[MAX_YX_NUM];  //遥信信号存储区
extern YC_DATA_CONFIG g_aYcDataConfig[MAX_YC_NUM];
extern YC_DATA_SEND   g_aYcDataSend[MAX_YC_NUM];  //遥测数据存储区
extern EN_DATA_CONFIG g_aEnergyDataConfig[MAX_ENERGY_NUM];
extern EN_DATA_SEND   g_aEnergyDataSend[MAX_ENERGY_NUM];  //电量数据存储区
extern YK_DATA_CONFIG g_aYkDataConfig[MAX_YK_NUM];
extern YK_DATA_SEND   g_aYkDataSend[MAX_YK_NUM];  //遥控数据存储区
extern FIXED_AREA_IDEN_T g_aFixedValueArea[MAX_FIXED_AREA_NUM];
extern PARA_SET_VALUE_T g_aParaSetValue[MAX_FIXED_AREA_NUM][PARA_MAX_NUM];  //定值参数配置
extern PARA_SET_VALUE_T g_aParaValueInit[PARA_MAX_NUM];
extern const Map104ToOop g_aMap104ToOopRT[];
extern const Map645ToOop g_aMap645ToOopRT[];
extern FAULT_REPORT_CONFIG g_aFaultDataConfig[FAULT_MAX_NUM];
extern FAULT_REPORT_SEND g_aFaultDataSend[FAULT_MAX_NUM];  //故障数据存储区

/****************************************************************/
//需要通信管理线程刷新的全局变量
/***************************************************************/
extern uint8  g_nNetWorkFlag;  //链路连接成功标志

/****************************************************************/
//全局变量配置区
/***************************************************************/
extern uint16 g_nPublicAddr;  //终端公共地址
extern uint8 g_nGlobalPipe;  //全局通道序号(记录最近一次的收发通道号，主要用于变化遥测、变化遥信和故障事件主动上报)
BOOL g_bGlobalInitFlag;  //全局初始化完成标志

extern BOOL g_bEnergyCycSendFlag;  //-突发送电量上送功能-
extern BOOL g_bYcCycSendFlag;  //-循环上送数据功能-
extern BOOL g_bBackScanSendFlag;  //-背景扫描功能-

extern uint8  g_bNetTestFlag;   //链路测试使能
extern uint8  g_bAckRecvFlag;   //104归约链路接收应答s帧发送标志
extern uint8  g_bInitEndFlag;   // 初始化结束标志
extern uint8  g_bDataSendFlag;   //全局可以发送数据的标志
extern int8   g_nIFrameT1Count;	 /*帧未收到确认回答的最大延时时间*/
extern uint8  g_nIFrameT2Count;	 //i帧应答确认超时时间
extern uint8  g_nIFrameT3Count;	 //链路测试最大间隔时间
extern uint8  g_nNoneActIFramTimes;   /*发送I帧未收到确认帧的次数*/
extern uint16 g_nIFramRecvTimes;	 /*收到I帧的帧数量*/ 	
extern uint16 g_nIFramSendTimes;	 /*I帧发送帧数量*/	
extern uint16 g_nRcvIFramSendNum;	 /*收到帧的发送帧序号*/	
extern uint16 g_nRcvFramRcvNum;	 /*收到帧的接收帧序号*/	
extern uint8  g_bAllDataCallFlag;   //总召唤启动标志	
extern uint8  g_bPointCallFlag;   //点召唤启动标志	
extern uint8  g_bTaskCallFlag;   //任务召唤启动标志	
extern uint8  g_bAllParaCallFlag;   //所有参数召唤启动标志
extern uint8  g_bSomeParaCallFlag;   //部分参数召唤启动标志
extern uint8  g_bYCYXChangeSendFlag;   /*遥测数据变化上报使能标志,在连接成功且上报过总召唤后打开*/
extern uint8  g_bTimerSynFlag;   /*已时钟同步的标志*/
extern uint8  g_nElecDataCallFlag;   //电能召唤标志
extern uint8  g_nFrameErrTimes;   //用于判断104归约帧序号错误的次数，如果大于3，关闭通道，小于3发链路复位
extern uint16 g_nDataReadFlagOrAddr;   /*读数据使能标志,如果有读数据，置为数据信息体地址*/
extern uint8  g_nRtuResetFlag;   /*终端复位标志，1表示全复位，清soe队列及遥测数据，2表示只清soe事件队列
                                                       				4表示终端停电复位，8表示终端本地复位，最高位如果置位，表示主站下发复位后，未复位终端的状态*/

extern uint32  g_nParaDataAddr; //参数定值召唤保存召唤的数据的信息体地址
extern uint8   g_nParaDataStep;//参数上送标志 1，启动上送，2，主站收到上送，3，上送结束 ，0无上送

extern uint8  g_nControlState;     //遥控全局变量，0x55预置，0xaa执行
extern uint8  g_nRemoteSetFlag;       //遥控预置标志
extern uint8  g_nRemoteSetDelay;       //遥控预置延时时间，不允许超过60秒
extern uint8  g_aCtrlReSendBuff[MAX_YK_BUFF_LEN];     //存储遥控下发数据，第一个字节为标志，第二个字节开始存放数据帧-

extern uint8  g_nParaFixedFlag;       //参数固化标志

extern uint8   g_nParaGetFlag;       //101、104规约参数读取后续处理使能 ，国网新实施细则 
extern uint16  g_nParaSetFlag;       //101、104规约参数设置后续处理使能 ，国网新实施细则 
extern uint8   g_nParaPortChangeFlag;//101、104规约参数区号切换后续处理使能 ，国网新实施细则 
extern uint8   g_nParaPortReadFlag;  //101、104规约参数区号读取后续处理使能 ，国网新实施细则

extern BOOL    g_bYxChanged;    //遥信变化，需要上送
extern BOOL    g_bYcChanged;    //遥测变化，需要上送
extern BOOL    g_bEnergyChanged;    //电量变化，需要上送
extern BOOL    g_bFaultChanged;    //事件变化，需要上送

extern uint8   g_nFixedValueArea;    //定值区号
extern BOOL    g_bFixedValueChanged;    //定值参数改变，需要同步至数据中心

extern TASK_RUN_STEP_T g_sTaskRunStep;  //104任务执行流程全局变量
extern TASK_REPORT_PIPE_T g_sTaskReportPipe;  //上报任务对应通道号

/****************************************************************/
//全局函数声明
/***************************************************************/
extern int s_frame_data_pack(uint8 *pOutBuf);
extern int u_frame_data_pack(uint8 bTpye, uint8 *pOutBuf);
extern int i_frame_data_pack(uint8 nTI, uint8 nVSQ, uint16 nCOT, uint8 *pInObjBuff, uint8 nInObjLen, uint8 *pOutBuf);
extern uint8 time_check(uint8 *ptime);
extern uint8 control_operate(uint8 infolen, uint8 *pdata, uint8 btype);
extern uint8 para_set_operate(uint8 infonum, uint8 *pdata, uint8 infolen, uint8 btype);
extern int para_data_write(uint16 nFixArea, PARA_SET_VALUE_T sParaSetValue);
extern int para_data_read(uint16 nFixArea, PARA_SET_VALUE_T *sParaSetValue);
extern int i_frame_data_unpack(uint16 nFrameLen, uint8 *pFrameBuff, uint8 *pOutBuf, uint8 *pAppType);
extern int frame_data_check(uint16 nFrameLen, uint8 *pFrameBuff);
extern int frame_data_unpack(uint16 nFrameLen, uint8 *pFrameBuff, uint8 *pOutBuf, uint8 *pAppType);
extern void energy_cyc_send();
extern void yc_cyc_send();
extern void back_scan_send();
extern void timing_active_send();
extern void yk_state_cyc_check();
extern void t1_t2_t3_cyc_check();
extern void yc_report_data_clear();
extern void yx_report_soe_clear();
extern int report_task_create();
extern uint8 yx_total_get(uint8 bAdulen, uint8 *pData ,uint16 bStartNum,uint8 *pGetNum);
extern float32 ratio_convert(int8 label, CONVERT_TYPE_E type);
extern uint8 yc_total_nva_get(uint8 bAdulen, uint8 *pData ,uint16 *pStartNum,uint8 *pGetNum);
extern uint8 yc_total_sva_get(uint8 bAdulen, uint8 *pData ,uint16 *pStartNum,uint8 *pGetNum);
extern uint8 yc_total_r_get(uint8 bAdulen, uint8 *pData ,uint16 *pStartNum,uint8 *pGetNum);
extern int yc_total_get(uint8 *pPipeIndex, uint8 *pReportBuf, uint8 *pAppType);
extern uint8 yx_change_get(uint8 bAdulen, uint8 *pData ,uint16 *pStartNum,uint8 *pGetNum);
extern uint8 yc_change_nva_get(uint8 bAdulen, uint8 *pData ,uint16 *pStartNum,uint8 *pGetNum);
extern uint8 yc_change_sva_get(uint8 bAdulen, uint8 *pData ,uint16 *pStartNum,uint8 *pGetNum);
extern uint8 yc_change_r_get(uint8 bAdulen, uint8 *pData ,uint16 *pStartNum,uint8 *pGetNum);
extern int yc_change_get(uint8 *pPipeIndex, uint8 *pReportBuf, uint8 *pAppType);
extern uint8 en_ac_data_get(uint8 bAdulen, uint8 *pData ,uint16 *pStartNum,uint8 *pGetNum);
extern uint8 en_without_time_get(uint8 bAdulen, uint8 *pData ,uint16 *pStartNum,uint8 *pGetNum);
extern uint8 en_with_time_get(uint8 bAdulen, uint8 *pData ,uint16 *pStartNum,uint8 *pGetNum);
extern int en_total_get(uint8 *pPipeIndex, uint8 *pReportBuf, uint8 *pAppType);
extern uint8 fault_change_get(uint8 bAdulen, uint8 *pData ,uint16 *pStartNum,uint8 *pGetNum);
extern int report_task_run(uint8 *pPipeIndex, uint8 *pReportBuf, uint8 *pAppType);
extern void frame_data_print(TOTAL_104_FRAME_T stTotal104Frame);
extern void log_frame_data_save(TOTAL_104_FRAME_T stTotal104Frame, char *path);



#ifdef __cplusplus
}
#endif

#endif // __PW_PROTOCOL_H__
