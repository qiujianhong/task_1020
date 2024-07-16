/*
***************************************************************************
  Copyright (c) 2006-2007,南京新联电子仪器有限责任公司
             All rights reserved.
    程序名称:   mettype.h
    功    能：表类型定义
    开 发 人：尤斌  
    开发时间：2007-03-17
    
    版本历史:
            2007-03-17 ,v1.0 : New              
    
***************************************************************************
*/

#ifndef _METTYPE_H
#define _METTYPE_H

#include "dataType.h"


#if CH_AC485_CACHE_EN == 1
#define RELAY_MAX_LEN	2048
#else
#define RELAY_MAX_LEN	1200
#endif
// 抄表模块与应用层之间的通讯结构

#define PLC_EVENT_DETECT		10	//载波测量点事件检测

typedef struct _RELAY_IPC_PULS
{
	uint8 RelayIPCType;					//-中继任务类型（透明转发（0x1X）、中继命令（0x2X）、其他命令（0x3X））-
	uint8 RelayIPCTaskStat;				//-中继结果（0-失败 1-成功 0xff-超时）-
	uint8 RelayIPCPort;					//-端口号（字节）-
	uint8 RelayIPCMode;					//-端口波特率信息（字节）-
	uint32 RelayIPCOverTime;			//-中继超时时间（字节），698单位ms-
	uint8 RelayIPCPrtl;					//-规约类型(0-97规约、-07规约)-
    uint8 RelayIPCFrameSeq;				//-帧序号，主要是给抄读后续帧时使用-
	uint16 RelayIPCLen;					//-长度（字节698改为两个字节）
	uint8 RelayIPCData[RELAY_MAX_LEN];	//-中继内容-
}RELAY_IPC_PULS;

/*
由于目前对中继命令使用比较混乱，为了降低代码混乱程序，故对中继命令做整理
目前思路是将中继命令分为“透明转发”、“中继命令”、“其他命令”三种（对应），每种类型下面分若干小类
1、透明转发（0x10）
-	对于“透明转发”类命令，组帧由应用层负责，抄表模块（cbtask、PLC、AC）只负责转发工作
-	抄表模块原样返回抄表应答报文，不对应答报文进行解析
2、中继命令（0x20）
-	此类命令由抄表模块负责组帧、解码，并根据命令类型决定回复应用层的报文内容
3、其他命令（0x30）
-	根据具体命令决定功能功能
*/
#define RELAY_TRAN					0x10			//透明转发
#define RELAY_REPEAT				0x20			//中继命令
#define RELAY_OTHER					0x30			//其他命令
#define RELAY_EVENT                 0x40            //事件分类相关命令

#define RELAY_TRAN_F1				0x11			//数据转发F1
#define RELAY_TRAN_BATCH_AUTH		0x12			//批量认证任务
#define RELAY_TRAN_BATCH_COMP		0x13			//批量对时任务
//#define RELAY_TRAN_BROAD_TIME		0x14			//广播对时任务

#define RELAY_REPEAT_F9				0x21			//数据转发F9
#define RELAY_REPEAT_DATA1_POINT	0x22			//一类数据点抄
#define RELAY_REPEAT_BROAD_TIME		0x23			//广播对时任务

#define RELAY_OTHER_CB_START		0x31			//按键抄表，抄表模块（cbtask、PLC）收到报文后回复应答报文，并立即启动抄表
#define RELAY_OTHER_PLC_DELAY		0x32			//请求通讯延时，该命令仅供PLC通道下，批量对时任务时使用，用来“请求依通信延时修正通信数据”命令
#define RELAY_OTHER_AC_3761			0x33			//该命令仅供交采模块与appmain通讯使用，用来透明转发通过交采被抄口收发的376.1报文
#define RELAY_OTHER_GUI				0x34			//该命令仅供显示模块与appmain通讯使用，由于旧程序中显示与appmain交互命令没有遵守IPC结构体的规则
#if TMNHALT_METHOD ==1
#define RELAY_HALT              	0x35			//掉电记录中继
#endif

#if EVENT_2014_EN == 1
#define RELAY_REPEAT_F10            0x24            //该命令的数据格式和F9的格式是一致的，新增该标识的目的是为了方便appmain解码，appmain通过该标识就可以知道是穿透F9还是F10。

#define RELAY_AUTOEVENT             0x41            //事件主动上报（抄表模块接收到路由模块的主动上报帧后通过该标识通知appmain）
#define RELAY_FOLLOW_SEQ            0x42            //后续数据帧（抄表模块接收到后续帧命令后通过该标识通知appmain）
#define RELAY_READ_EVENTCOUNT       0x43            //appmain通知抄表模块抄读指定的事件发生次数，抄表模块发起抄读命令，将电表的回码进行解析并存入数据字典。
#define RELAY_READ_EVENTRECORD      0x44            //appmain通知抄表模块抄读指定事件上1~N的记录，该命令的数据格式和F9、F10的格式是一致的
#define RELAY_READ_AUTOEVENT        0x45            //appmain主动抄读电能表状态字0x04001501（抄表模块不解析，直接返回应答报文，数据格式和F9、F10的格式是一致的）
#define RELAY_REST_METERSTATUS      0x46            //复位电能表主动上报状态字，主要是用于等待确认帧后再抄读一下状态字
#define RELAY_READ_FOLLOW_FRAME     0x47            //读后续数据帧（抄表模块接收到后续帧命令后通过该标识通知appmain）
#if JIANG_SU_FUN == 1
#define RELAY_READ_DI_SET           0x48            //appmain通知抄表模块抄读采集器设置的状态字对应事件数据标识，抄表模块发起抄读命令，将电表的回码报给appmain。
#endif
#endif

#if LIAONING_TICK_EN == 1
#define RELAY_RDMETER_CLOCK          0x51           //抄读电表时钟，该命令格式类似于一类数据点
#define RELAY_SETMETER_CLOCK         0x52           //设置电表时钟
#endif

#if ZHEJIANG_FUN == 1
#define RELAY_TMN_RESET              0x61           //终端硬件复位
#endif

#if STD_698PRO_BETA == 1
#define RELAY_GET_NORMALLIST		 0x71			//读取若干个服务器的若干个对象属性
#define RELAY_GET_RECORD		 	 0x72			//读取一个服务器的一个记录型对象属性
#define RELAY_SET_NORMALLIST		 0x73			//设置若干个服务器的若干个对象属性
#define RELAY_SET_GETLIST		 	 0x74			//设置后读取若干个服务器的若干个对象属性
#define RELAY_ACTION_NORMALLIST		 0x75			//操作若干个服务器的若干个对象方法
#define RELAY_ACTION_GETLIST	 	 0x76			//操作后读取若干个服务器的若干个对象方法和属性
#define RELAY_TRANS_CMD		 	     0x77			//透明转发
#define RELAY_TRANS_F209		 	 0x78			//F209透明转发
#define RELAY_TRANS_F208		 	 0x79			//交采F208透明转发
#endif

//外部定义表类型
/*-不同的通讯规约修改该宏定义即可，类型为220以后的表示此地区没有这种表-*/
#if STD_698PRO_BETA == 1
#define OOP_PRTL     	235      //2面向对象规约电表
#endif
#if JIANG_SU_FUN == 1
#define NO_MET_APP      0       //-未接电表
#define DLT645_APP      1    	//-国标电表-
#define AC_SAMPINGMET   2    	//-交采电表-
#define JSBLACK_APP     2    	//-江苏交采-
#define FJ_APP          3    	//-福建规约-
#define LGR_APP_D3      4    	//-ZFB兰吉尔d型表三相三线-       
#define LGR_APP         5    	//-ZFB兰吉尔d型表三相四线-
#define LGR_D_MET_B3    6    	//-兰吉尔B型表三相三线-
#define LGR_D_MET       7    	//-兰吉尔B型表三相四线-    
#define ABB_APP         8    	//-ABB，(方表)-       
#define ABB_APP_S       9    	//-ABB(圆表)-
#define EDMI_APP		10	 	//-红相-
#define DLT645_FFEE_APP 11 //终端自适应是否支持数据块读取，四费率，顺序为总、峰、平、谷、尖；
#define DLT645_TFEE_APP 12 //电表不支持数据块读取，仅两费率；
#define WS20_APP        13   	//-威胜电表-    
#define DSSD25_APP      14   	//-浙江规约-
#define HETONG_OLD      15   	//-恒通I型(旧规约) -
#define HETONG_NEW      16   	//-恒通II型(新规约)-
#define HND_APP         17   	//-宁光电表(浩宁达自己规约)-
#define JIANGBEI        18   	//-江北电表-
#define WXABB_APP      	19   	//-德国ABB(ELSTER)，原宏定义为ELSTER_ABB-
#define LG9600_APP      20   	//-无锡新版LDS_D(9600波特)-
#define JSSIMULATE_APP	21	 	//-江苏模拟量-
#define SIMPLE_DLT_APP	22	 	//-单条抄国标-*/ 
#define DLMS_APP        23      //-DLMS规约-
#define CQINTELL_APP	24	 	//-重庆智能表-
#define LGR_B_MET		25   	//-特殊兰吉尔表-      
#define ZMC_APP         27      //-lds c表-
#define ELSTER_APP      28		//-ELSTER表-
#define DL2007_PLC_EVT  29      //13规范智能表，支持事件上报
#define DL2007_PRTL     30      //2007规约电表 
#define DL2007_PLC      31      //2007规约电表(载波)

#if OS_SMARTBREAKER_EN == 1
#define SMARTBREAKER_PRTL     33      //智能断路器规约
#endif

#define EMAIL_APP   	64	 	//-email表-       
#define FQD_APP         69      //-防窃电表- 
#define DLT645_TLLD     153  	//-三能防窃电表-   
#define ZZDB_APP        154     //-郑州负控,新增表(暂时自定义)-
#define EDMI_APP_Y		221	 	//-红相MK3-   
#define LGR_U_MET       222   	//-LDS U型表-
#define KELU_APP        223		//-科陆规约- 
#define ANHUI_MET       224    	//-安徽国标表-
#define DLT645_TJ       225    	//-天津表规约-
#define WEISHENG_645    226    	//-威胜645规约-
#define DONGFANG_645    227    	//-东方645规约-
#define HUALI_645       228    	//-华立645规约- 
#define WXABB_APP_4     229   	//-ELSTER,三相四线- 
#define ELSTER_APP_D3   230     //-ELSTER1700表三相三线-
#define JLB_APP         231   	//-吉林表-
#define DLT645CQJY_APP  234     //-重庆简易国标表-  
#elif HUBEI_FUN == 1

// 湖北规定
// 0//无效、1//DL/T645-1997规约、2//交流采集、3//预留、4//兰吉尔D表、
// 5//兰吉尔B表、6//鄂规（多功能）、7//浩宁达、8//ABB方表、9//ABB圆表、
// 10//预留、11//预留、12//威胜4.0、13//红相MK6、14//红相MK3、
// 15//鄂规（简易多功能）、16//兰吉尔U表、17//鄂规（普通电子式电能表）、
// 18//鄂规（居民复费率电能表）。30// DL/T645-2007。

#define NO_MET_APP      0           //-未接电表
#define DLT645_APP      1    		//-国标电表-
#define AC_SAMPINGMET   2    		//-交采电表-
#define LGR_APP         4    		//-ZFB兰吉尔d型表三相四线-
#define LGR_D_MET       5    		//-兰吉尔B型表三相四线-    
#define HB_STD_APP		6			//鄂规（多功能）
#define HND_APP         7   		//-宁光电表(浩宁达自己规约)-
#define ABB_APP         8    		//-ABB，(方表)-
#define ABB_APP_S       9    		//-ABB(圆表)-
#define WS20_APP        12   		//-威胜电表-   
#define EDMI_APP		13	 		//-红相-
#define EDMI_APP_Y		14	 		//-红相MK3-
#define HB_JY_APP		15			//鄂规（简易多功能）
#define LGR_D_MET_B3    16    		//-兰吉尔B型表三相三线-    
#define HB_PT_APP		17			//鄂规（普通电子式电能表）
#define HB_FFL_APP		18			//鄂规（居民复费率电能表）
//#define DL2007_PRTL19	19   		//-DL/T645-2007-
#define DLT645_PRTL20	20			//辅助实现防窃电功能的数据转换模块。湖北要求电表规约号20
#define HB_MUL_STD_APP	21   		//湖北省多功能表规约
#define HB_JY_WP_APP	22   		//湖北省简易表规约(有分相电量)
#define HB_JY_NP_APP	23   		//湖北省简易表规约(无分相电量)
#define HB_JY_STD_APP	24   		//国标简易表规约（总电量）
#define DL2007_PRTL		30    		//-DL/T645-2007-
#define DL2007_PLC		31			//	
#define FJ_APP          12+20    	//-福建规约-
#define DSSD25_APP      14+20    	//-浙江规约-
#define HETONG_OLD      15+20    	//-恒通I型(旧规约) -
#define HETONG_NEW      16+20    	//-恒通II型(新规约)-
#define LGR_APP_D3      17+20     	//-ZFB兰吉尔d型表三相三线-    
#define JIANGBEI        18+20    	//-江北电表-
#define WXABB_APP      	19+20    	//-德国ABB(ELSTER)，原宏定义为ELSTER_ABB-
#define JLB_APP         20+20    	//-吉林表-
#define JSSIMULATE_APP	21+20 	 	//-江苏模拟量-
#define SIMPLE_DLT_APP	22+20 	 	//-单条抄国标-*/ 
#define DLMS_APP        23+20       //-DLMS规约-
#define CQINTELL_APP	24+20 	 	//-重庆智能表-
#define LGR_B_MET		25+20    	//-特殊兰吉尔表-      
#define ELSTER_APP      28+20 		//-ELSTER表-
#define ZMC_APP         29+20       //-lds c表-
#define EMAIL_APP   	64+20 	 	//-email表-       
#define FQD_APP         69+20       //-防窃电表- 
#define DLT645_TLLD     153  	//-三能防窃电表-   
#define ZZDB_APP        154     //-郑州负控,新增表(暂时自定义)-
#define LGR_U_MET       222   	//-LDS U型表-
#define KELU_APP        223		//-科陆规约- 
#define ANHUI_MET       224    	//-安徽国标表-
#define DLT645_TJ       225    	//-天津表规约-
#define WEISHENG_645    226    	//-威胜645规约-
#define DONGFANG_645    227    	//-东方645规约-
#define HUALI_645       228    	//-华立645规约- 
#define WXABB_APP_4     229   	//-ELSTER,三相四线- 
#define ELSTER_APP_D3   230     //-ELSTER1700表三相三线-
#define LG9600_APP      231   	//-无锡新版LDS_D(9600波特)-
#define DLT645CQJY_APP  234     //-重庆简易国标表-
#else
#define NO_MET_APP      0       //-未接电表
#define DLT645_APP      1    	//-国标电表-
#define AC_SAMPINGMET   2    	//-交采电表-
#define JSBLACK_APP     2    	//-江苏交采-
#define FJ_APP          3    	//-福建规约-
#define LGR_APP_D3      4    	//-ZFB兰吉尔d型表三相三线-       
#define LGR_APP         5    	//-ZFB兰吉尔d型表三相四线-
#define LGR_D_MET_B3    6    	//-兰吉尔B型表三相三线-
#define LGR_D_MET       7    	//-兰吉尔B型表三相四线-    
#define ABB_APP         8    	//-ABB，(方表)-       
#define ABB_APP_S       9    	//-ABB(圆表)-
#define EDMI_APP		10	 	//-红相-
#define WS20_APP        12   	//-威胜电表-    
#define DSSD25_APP      14   	//-浙江规约-
#define HETONG_OLD      15   	//-恒通I型(旧规约) -
#define HETONG_NEW      16   	//-恒通II型(新规约)-
#define HND_APP         17   	//-宁光电表(浩宁达自己规约)-
#define JIANGBEI        18   	//-江北电表-
#define WXABB_APP      	19   	//-德国ABB(ELSTER)，原宏定义为ELSTER_ABB-
#define LG9600_APP      20   	//-无锡新版LDS_D(9600波特)-
#define JSSIMULATE_APP	21	 	//-江苏模拟量-
#define SIMPLE_DLT_APP	22	 	//-单条抄国标-*/ 
#define DLMS_APP        23      //-DLMS规约-
#define CQINTELL_APP	24	 	//-重庆智能表-
#define LGR_B_MET		25   	//-特殊兰吉尔表-      
#define ELSTER_APP      28		//-ELSTER表-
#define ZMC_APP         29      //-lds c表-
#define DL2007_PRTL     30      //2007规约电表 
#define DL2007_PLC      31      //2007规约电表(载波)
#define EMAIL_APP   	64	 	//-email表-       
#define FQD_APP         69      //-防窃电表- 
#define DLT645_TLLD     153  	//-三能防窃电表-   
#define ZZDB_APP        154     //-郑州负控,新增表(暂时自定义)-
#define EDMI_APP_Y		221	 	//-红相MK3-   
#define LGR_U_MET       222   	//-LDS U型表-
#define KELU_APP        223		//-科陆规约- 
#define ANHUI_MET       224    	//-安徽国标表-
#define DLT645_TJ       225    	//-天津表规约-
#define WEISHENG_645    226    	//-威胜645规约-
#define DONGFANG_645    227    	//-东方645规约-
#define HUALI_645       228    	//-华立645规约- 
#define WXABB_APP_4     229   	//-ELSTER,三相四线- 
#define ELSTER_APP_D3   230     //-ELSTER1700表三相三线-
#define JLB_APP         231   	//-吉林表-
#define DLT645CQJY_APP  234     //-重庆简易国标表-    
#endif
////////////////////////////////////

#define MAX_MET_NUM       	40
#define DLT645              1 //国标表
#define LGR                 2 //LGR表
#define WS20                3 //威胜表
#define ABB                 4 //ABB表
#define EDMI                5 //红相表
#define JSBLACK             6 //江苏交采表
#define HND                 7 //浩宁达表
#define HT                  8 //恒通表
#define NZJC		        9 //内置交采表
#define WXABB              10 //无锡ABB表
#define DSSD25             11 //浙江规约表
#define DLT645TJ           12 //天津规约
#define FQD                13 //防窃电表
#define DLT645JY           14 //简易国标
#define JLB                15 //吉林规约表
#define JSSIMULATE         16 //江苏模拟表
#define DLT645_2007		   17	//-DL/T645-2007-
#define DLT645_0           18 //三能防窃电表
#define FJ                 19 //福建电表
#define CBH                20 //抄表盒
#define DLMS               21 //DLMS
#define ZMC                22 //兰吉尔C表
#define	DIRECTSIM		   23 //直流模拟量
#define HR				   24 //谐波
#define LG9600			   25 //9600bps兰吉尔D表
#define ELSTER             26 //福建ELSTER表
#define EMAIL              27 //email表
#define DLT645CQJY         28 //重庆简易国标
#define ZZDB               29 //郑州规约表
#define CQINTELL           30 //重庆表 
#define ABB_S              31 //ABB圆表
#define FJ_KN              32 //福建漳州科能表
#define LGR_B              33 //兰吉尔B表三相三线/三相四线
#define	HT_NEW		   	   34 //恒通表(新规约)
#define DL2007_PLC_MET     35 //2007规约电表 
#define ACTARIS            36 //法国ACTARIS
#if HUBEI_FUN == 1
#define HB_STD			   37	//鄂规（多功能）
#define HB_JY			   38	//鄂规（简易多功能）
#define HB_PT			   39	//鄂规（普通电子式电能表）
#define HB_FFL			   40	//鄂规（居民复费率电能表）
#define FQDMK			   41	//防窃电模块:辅助实现防窃电功能的数据转换模块。注:湖北要求对应 DLT645_PRTL20
#define HB_MUL_STD         43   //湖北省多功能表规约
#define HB_JY_WP           44   //湖北省简易表规约(有分相电量)
#define HB_JY_NP           45   //湖北省简易表规约(无分相电量)
#endif

#define DLT645DC           42 //单抄国标表

#if JIANG_SU_FUN == 1
#define DLT645_FFEE        43 //终端自适应是否支持数据块读取，四费率，顺序为总、峰、平、谷、尖；
#define DLT645_TFEE        44 //电表不支持数据块读取，仅两费率；
#define DLT645_2007_EVT    45   

#if  OS_SMARTBREAKER_EN == 1
#define SMARTBREAKER        46      //智能断路器规约
#endif

#endif

#if STD_698PRO_BETA == 1
#define DLTOOP				47 //面向对象规约电能表
#define DLT188				48 //面向对象规约定义的188规约表
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _METTYPE_CONTRAST
{
    uint8   nMettypeID;     // 外部定义表类型
    char*   czMettype;      // 表类型字符串，不要太长，控制在5汉字以内
}METTYPE_CONTRAST;

extern METTYPE_CONTRAST g_MettypeContrast[];
extern uint32 g_nMettypeCnt;
uint8 MtTyp_To_IdMtTyp(uint8 MtTyp);
char* get_met_type_str(uint32 type);

#ifdef __cplusplus
}
#endif
#endif
