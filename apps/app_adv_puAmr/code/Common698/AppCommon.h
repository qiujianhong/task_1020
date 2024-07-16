/*-***************************************************************
* Copyright (c) 2006,南京新联电子仪器有限公司
*            All rights reserved.
* 程序名称：AppCommon.h
* 版本号：1.0
* 功能：对事件检测过程中用到的数据结构，调度函数进行声明
* 原始开发人及时间：
		董宁 2006年12月25日
* 修改人员、时间及修改内容:
* 		修改内容；补充几个关于帧结构的定义
* 		郑健锋 2006年1月19日
* 其他：
******************************************************************-*/
#ifndef _APPCOMMON_H
#define _APPCOMMON_H


#define DPRINT_PRTL_OOP(fmt...) \
    ({ if(getenv("DEBUG_OOP_PRTL") != NULL)  printf("OOPPRTL: "fmt);})

/*******************************************************************************
* 公共的部分，这部分定义一般不需要根据地区或硬件而改变
*******************************************************************************/
//--用来解决类型强制转换的问题--
#define chartoint16(p) 			((*((p) + 1) << 8) + *(p))
#define chartoint32(p) 			((*((p) + 3) << 24) + (*((p) + 2) << 16) + (*((p) + 1) << 8) + *(p))
#define chartoint64(p) 			(((int64)(*((p) + 7)) << 56) +((int64)(*((p) + 6)) << 48) +((int64)(*((p) + 5)) << 40) +((int64)(*((p) + 4)) << 32) +(*((p) + 3) << 24) + (*((p) + 2) << 16) + (*((p) + 1) << 8) + *(p))


#define CBTIME_DATA_FORMAT 		1 				//有关抄表时间的数据，当填无效值时，填0xee还是0x0；1填0xee，0填0x0
#define PasssWordCheck_EN 		1				//密码检测模块使能，1检测报文密码正确性，0报文密码检测直接通过
#define AB_TYPE_SWITCH  		1				//1a型触点，0b型触点；用于a、b型触点的遥信状态的切换，相关的id为SWITCH_XCH_STATU

//表示读取记录时记录的起始指针是首次读取成功的记录的序号
//结束指针是在成功读取记录的条件下遇到读取数记录失败时的记录的前一个记录
#define SUCCRECFLAG 			1

//用来提供给函数CheckDataValid使用
#define CHAR_FORMAT_0   		0x0
#define CHAR_FORMAT_EE  		0xee
#define CHAR_FORMAT_FF  		0xff

#define TEN_THOUSAND			10000			//电量相关的值放大10000倍后存储到数据字典中
#define POWER_STOP_TIME_LMT 	300 		    //上电事件判断上电与否需要的超时时间
/******************************************************************************
* 曲线数据的数据的读取时间的起始的选择，
* 示例1:
* 1小时一个点时，起始时间13:30,
* 取0表示13:00~14:00;
* 取1表示13:30~14:30--
* 示例2:
* 1小时一个点时，起始时间13:15,
* 取0表示13:00~14:00;
* 取1表示13:15~14:15--
*******************************************************************************/
#define GET_96_DATA_TIMESTD  	0 				//判断96点存储，读取数据选项

#define FIRST_DATA_TYPE   		1  				//重要事件
#define SECOND_DATA_TYPE  		2  				//一般事件

#define NEW_RECORD_TYPE   		0x00 			//新记录
#define OLD_RECORD_TYPE   		0x01 			//原有的记录
#define MUST_SEND_TYPE    		0x00 			//需要判断是否上报
#define NONE_SEND_TYPE    		0x02 			//不需要判断是否上报

//记录号
#define	ERC_NO_01	    		1 				//初始化及版本变更记录
#define	ERC_NO_02	    		2 				//终端参数丢失记录
#define	ERC_NO_03	    		3 				//备用
#define	ERC_NO_04	    		4 				//遥信变位记录
#define	ERC_NO_05	    		5 				//遥控跳闸记录
#define	ERC_NO_06	    		6 				//功控跳闸记录
#define	ERC_NO_07	    		7 				//电控跳闸记录
#define	ERC_NO_08	    		8 				//电能表参数变更记录
#define	ERC_NO_09	    		9 				//电流回路异常记录
#define	ERC_NO_10	    		10				//电压回路异常记录
#define	ERC_NO_11	    		11				//相序异常记录
#define	ERC_NO_12	    		12				//表时钟异常记录
#define	ERC_NO_13	    		13				//电能表故障记录
#define	ERC_NO_14	    		14				//终端停/上电事件记录
#define	ERC_NO_15	    		15				//谐波越限记录
#define	ERC_NO_16	    		16				//备用
#define	ERC_NO_17	    		17				//电压电流不平衡
#define	ERC_NO_18	    		18				//备用
#define	ERC_NO_19	    		19				//电压电流不平衡
#define	ERC_NO_20	    		20				//密码错误记录
#define	ERC_NO_21	    		21				//终端故障记录(485故障)
#define	ERC_NO_22	    		22				//有功总电能量差动越限
#define	ERC_NO_23	    		23				//电控告警事件
#define	ERC_NO_24	    		24				//电压越限记录
#define	ERC_NO_25	    		25				//电流越限告警
#define	ERC_NO_26	   	 		26				//视在功率越限告警
#define	ERC_NO_27	    		27				//电表示度下降告警
#define	ERC_NO_28	    		28				//电表电能量超差告警
#define	ERC_NO_29	    		29				//电表飞走告警
#define	ERC_NO_30	    		30				//电表停走告警
#define	ERC_NO_31				31				//485抄表失败事件记录

//698新增事件
#define	ERC_NO_32				32				//-终端与主站通信流量超门限事件记录
#define	ERC_NO_33				33				//-电能表运行状态字变位事件记录
#define	ERC_NO_34				34				//-CT异常事件记录
#define	ERC_NO_35				35				//-发现未知电表事件记录

#define ERC_NO_36				36				//控制输出回路开关接入状态量变位记录
#define ERC_NO_37				37				//电能表开表盖事件记录
#define ERC_NO_38				38				//电能表开端钮盒事件记录
#define ERC_NO_39				39				//补抄失败事件记录
#define ERC_NO_40				40				//磁场异常事件
#define ERC_NO_41				41				//终端对时事件
#define ERC_NO_42				42				//模块变更事件
#if LIAONING_FUN == 1 || ANHUI_FUN == 1
#define ERC_NO_51				51				//终端对电表校时事件
#endif
#if TMNHALT_METHOD ==1
#define ERC_NO_45				45				//电池失效事件
#endif

/*-记录长度，不包括记录号和长度值本身-*/
#define	ERC_NO_01_LEN			14
#define	ERC_NO_02_LEN			6
#define	ERC_NO_03_LEN			0
#define	ERC_NO_04_LEN			7
#define	ERC_NO_05_LEN			10
#define	ERC_NO_06_LEN			14
#define	ERC_NO_07_LEN			16
#define	ERC_NO_08_LEN			8
#define	ERC_NO_09_LEN			28
#define	ERC_NO_10_LEN			28
#define	ERC_NO_11_LEN			24
#define	ERC_NO_12_LEN			7
#define	ERC_NO_13_LEN			8
#if TMNHALT_METHOD ==1
#define	ERC_NO_14_LEN			11
#else
#define	ERC_NO_14_LEN			10
#endif

#define	ERC_NO_15_LEN			49
#if ANOLOGY_EN == 1
#define	ERC_NO_16_LEN			9
#else
#define	ERC_NO_16_LEN			0
#endif
#define	ERC_NO_17_LEN   		27
#define	ERC_NO_18_LEN   		16
#define	ERC_NO_19_LEN   		31
#define	ERC_NO_20_LEN   		22
#define	ERC_NO_21_LEN  		 	6
#define	ERC_NO_22_LEN  		 	21
#define	ERC_NO_23_LEN   		16
#define	ERC_NO_24_LEN   		14
#define	ERC_NO_25_LEN   		17
#define	ERC_NO_26_LEN   		14
#define	ERC_NO_27_LEN   		17
#define	ERC_NO_28_LEN   		18
#define	ERC_NO_29_LEN   		18
#define	ERC_NO_30_LEN   		13
#define	ERC_NO_31_LEN			21
#define	ERC_NO_32_LEN			13
#define	ERC_NO_33_LEN			35
#define	ERC_NO_34_LEN			8
#define	ERC_NO_35_LEN			0
#define ERC_NO_36_LEN			7
#define ERC_NO_37_LEN 		70
#define ERC_NO_38_LEN 		70
#define ERC_NO_39_LEN 		21
#define ERC_NO_40_LEN 		15
#define ERC_NO_41_LEN		14
#if TMNHALT_METHOD ==1
#define ERC_NO_42_LEN       0
#define ERC_NO_43_LEN       0
#define ERC_NO_44_LEN       0
#define ERC_NO_45_LEN       5				//电池失效事件
#define ERC_NO_46_LEN       0
#define ERC_NO_47_LEN       0
#define ERC_NO_48_LEN       0
#define ERC_NO_49_LEN       0
#define ERC_NO_50_LEN       0
#define ERC_NO_51_LEN       0
#define ERC_NO_52_LEN       0
#define ERC_NO_53_LEN       0
#define ERC_NO_54_LEN       0
#define ERC_NO_55_LEN       0
#define ERC_NO_56_LEN       0
#define ERC_NO_57_LEN       0
#define ERC_NO_58_LEN       0
#define ERC_NO_59_LEN       0
#define ERC_NO_60_LEN       0
#define ERC_NO_61_LEN       0
#define ERC_NO_62_LEN       0
#define ERC_NO_63_LEN       0
#if JIANGSU_JZQ_FUN == 1
#define ERC_NO_64_LEN       93
#else
#define ERC_NO_64_LEN       0
#endif
#endif

#define MAX_EVENT_NUM  			15  			//事件记录中断相事件外存后面可以存放的信息个数
#define MAX_DIFF_NUMBER 		4  				//差动越限事件检测支持的差动组个数

//声音告警索引
#define YXHZ_PHONE				0x01			//允许合闸
#define YKTZ_PHONE				0x02			//遥控跳闸
#define DLKYW_PHONE				0x03			//电量快用完，请速购电
#define DLYWYTZ_PHONE   		0x04			//电量用完，已跳闸
#define YCFHQXD_PHONE			0x05			//已超负荷，请限电
#define GKTZ_PHONE				0x06			//功控跳闸
#define YDKTR_PHONE				0x07			//月电控已投入
#define YDKJC_PHONE				0x08			//月电控已解除
#define GDKTR_PHONE				0x09			//购电控已投入
#define GDKJC_PHONE				0x0a			//购电控已解除
#define XFKTR_PHONE				0x0b			//当前功率下浮控已投入
#define XFKJC_PHONE				0x0c			//当前功率下浮控已解除
#define BTKTR_PHONE				0x0d			//报停控投入
#define BTKJC_PHONE				0x0e			//报停控解除
#define CXKTR_PHONE				0x0f			//厂休控已投入
#define CXKJC_PHONE				0x10			//厂休控已解除
#define SDKTR_PHONE				0x11			//时段控已投入
#define SDKJC_PHONE				0x12			//时段控已解除
#define YXCSYGG_PHONE			0x13			//运行参数已更改，请注意查看
#define ZXXX_PHONE				0x14			//最新信息，请查看
#define DUDUDU_PHONE			0x15			//嘟嘟嘟
#define DEFAULT_PHONE			0x18			//纯语音
#define STOPVOICE				0x0				//停止语音
#define CFKYI_PHONE    		 	0x19			//请用户及时结算电费

//-日志文件宏定义-
#define WRITE_LOG_EN 			1				//1、写日志；0、不写日志
#define FILE_MAX_NUM      		(1024*400)  	//日志信息最大长度  20K
//-日志文件保存的路径-
#define EVENT_LOG_FILE			"/mnt/log/libEvent.log"
#define PRTL_04_LOG_FILE		"/mnt/log/libptrl.log"
#define CONTROL_LOG_FILE		"/mnt/log/libControl.log"
#define AUTOTASK_LOG_FILE    	"/mnt/log/libAutoTask.log"
#define EN_REMAIN_APPMAIN_LOG_FILE	"/mnt/log/enremainappmain.log"

#define SECURITY_LOG_FILE		"/mnt/log/security.log"         // 安全报文加解密日志

#if DELIVER_LOG_EN == 1
#define EVENT_SETGET_LOG    	"/mnt/log/event_setget.log"
#define EVENT_DATA_LOG    		"/mnt/log/event_data.log"
#define CONTROL_DATA_LOG    	"/mnt/log/control_data.log"
#endif
#define JSBLACK_APP     		2   			//江苏交采
#define AC_SAMPINGMET   		2   			//交采电表
#define PLC_PRTL				31				//载波接口协议
#define MTR_Invalid       		0   			//该测量点没有配置
#define NULLMET					0x00			//该通道未接电表
#define Non485Met       		255   			//由单路脉冲或多路脉冲组成的混合型计量点，电压电流一般来自直流Ａ/Ｄ-

//链路口检测使用的宏定义
#define GPRS_TRY_CNT			2
#define GPRS_LOGIN_FLAG			0x11
#define GPRS_HEART_FLAG			0xaa

/*****************-通信驱动相关的子功能使能-**************/

#define Heart_Proj_EN			0			//心跳方案使能
#define Heart_ZJ_GB				0			//浙江,国标心跳选择,0为浙江规约,1为国标规约

#define LOGIN_INTERVAL 			30			//尝试登陆的间隔时间，10~20s
#if SHANGHAI_FUN == 1
#define HEART_INTERVAL 			300			//心跳间隔时间，s
#elif SHANXI_FUN == 1
#define HEART_INTERVAL 			600			//心跳间隔时间，s
#else
#define HEART_INTERVAL 			180			//心跳间隔时间，s
#endif
#define RESET_DELAY_TIME 		20			//复位命令从解析出命令到执行的延时

/******************************************************************************
*	通道允许收发数据的最大长度
******************************************************************************/
#if CH_AC485_CACHE_EN == 1
#define SFP_485_3761_LEN		2048
#else
#define SFP_485_3761_LEN		1024
#endif
#if HUNAN_FUN == 1
#define SFP_MAX_LEN				2048	 //湖南无线特殊处理(招测终端证书报文长)
#else
#define SFP_MAX_LEN				300			//无线数据不大于255
#endif
#define SFP_SMS_MAX_LEN       	200			//短信多帧最大帧长度
#define SFP_GPRS_GSM_MAX_LEN	1400		//GPRS_GSM数据不大于800
#if TD_SCDMA == 1
//本来程序里面是给CDMA使用的长度标志，但没有用到
//现在给TD_SCDMA使用
#define SFP_CDMA_MAX_LEN		1400		//TD_CDMA数据不大于4100
#else
#define SFP_CDMA_MAX_LEN		560			//CDMA数据不大于560
#endif
#define SFP_RADIO_MAX_LEN		255			//Radio数据不大于255
#define SFP_NET_MAX_LEN			1400		//网络传输不大于16383,一般取13000，现在暂取800-

//收发数据的最大长度的相关宏定义
#define CCH1_BUFF_SIZE_SND		SFP_GPRS_GSM_MAX_LEN
#define CCH1_BUFF_SIZE_RCV		2048

#if HUABEI_FUN
//应用分帧长度有自定义要求
#define APPLY_FRAME_CUSTOM      1
#define APPLY_FRAME_SIZE        1400 - SFP_FRA_DATA
#endif


//新疆应用分帧长度宏定义 之后可以合并到上面应用分帧长度自定义要求
#if XINJIANG_FUN
#define APPLY_FRAME_SIZE		1400 - SFP_FRA_DATA
#endif

// 最大可处理的APDU长度
#define APDU_BUFF_SIZE_MAX		4096


#define RELAY_485_3761			12	//485维护口发的3761帧

#define TMNRELAY 					1			//终端中级转发，不需要进行后续处理；
#define SUCCESS 					0			//程序处理中正常，需要进行后续处理；
#define PROCISOVER 					0			//程序处理结束，不需要进行后续处理；
#define ERROR 						-1			//程序处理中出现错误；
#define ERR_PW						-2			//-2  密码错误,
#define ERR_OVERTIME				-3			//-3  时间标签时间无效；
#define ERR_ADDR 					-4			//-4 地址域错误；

#define RELAY_ADDR_MAXNUM	 		16			//被转发终端中继转发地址最大个数

#define DELAYTORESET 				1			//复位操作需要的延时，原有的是800ms，暂取1s


//#if ESAM_ENCRYPT_EN_2012 == 1
// 2012版ESAM相关的宏定义
#define	NO_ESAM		0x0fffffff		// 终端不存在ESAM
//#endif

//链路状态
#if GB101_104_EN  
#define LINK_WAIT       -1      //未知或等待
#define LINK_SEND       0x00    //通讯
#endif

#define LINK_LOGIN		0x01	//登录
#define LINK_HEART		0x02	//心跳
#define LINK_QUIT		0x03	//退出登录
#define LINK_CHANGE		0x04	//状态切换

#if GB101_104_EN 
#define LINK_CLOSE      0x04    //关闭通道
#endif
#define INIT_SUCCESS	0		//初始化成功
#define INIT_DOING		1		//正在初始化
#define INIT_FAIL		2		//初始化失败
//需要发送心跳、登录帧的信道类型
#define CCH_NEED_LINK(CH_ID)	(((CH_ID) == SFP_CCH_GPRS) || ((CH_ID) == SFP_CCH_Ethernet) ||((CH_ID) == SFP_CCH_CDMA) || ((CH_ID) == TDSCDMA_ID))

/*-**************     自动任务相关的常量声明       ******************-*/
#define TASK_TYPE_1      			1        	//上报任务类型1
#define TASK_TYPE_2      			2        	//上报任务类型1
#define AUTOTASK_MAX_NUM     		64       	//终端支持的每种类型中最大任务总数
#define MAX_ID_NUM       			256       //-每个任务中支持的最大ID数量-
#define SEND_PARA_LENGTH 	((1+6+1+1)+4*MAX_ID_NUM + 2)       //-每个任务的参数长度:9(1+6+1+1)+4*256=73,再加上2bytes的校验码，一共75bytes-
#define SEND_PARA_Data_LENGTH (SEND_PARA_LENGTH-2)	//--每个任务的有效数据长度(即不包含校验码的2bytes)--
/*-**************    自动任务相关的常量声明    end    ******************-*/

/*--帧属性偏移地址--*/
#define COMMAND_SITE   				6    		//控制码-
#define A1_SITE        				7			//行政划码A1-
#define A2_SITE        				8			//行政划码A2-
#define B1_SITE        				9   		//终端地址B1-
#define B2_SITE        				10			//终端地址B2-

#define MSA_SITE       				11   		//主站地址-
#define DATASTART      				12   		//数据开始位置-
#define AFN_SITE       				12   		//应用层功能码-
#define SEQ_SITE       				13   		//帧序列域-
#define APP_DATA_SITE  				14   		//应用层数据开始位置，6(报文头)+1(控制域C)+5(地址域A)+1(应用层功能码AFN)+1(序列域SEQ)--
#define SFP_FRA_DATA				17			//(报文头1、长度2、控制域1、地址域8、头校验2、尾校验2、结束符1

#define SFP_FRA_FIXHEADLEN			6			//帧头固定长度-
#define SFP_FRA_FIXUSERLEN			2			//帧尾固定长度-

#define VF_BEGIN       				0x68 		//帧起始字符-
#define VF_END         				0x16		//帧结束字符-

//地址类型标志
#define RTU_ADDRESS_UNBC       		-2			//不支持自定义广播-
#define RTU_ADDRESS_ERROR       	-1			//地址错误-
#define RTU_ADDRESS_SINGLE       	0			//单地址-
#define RTU_ADDRESS_WILD         	1 			//通配地址-
#define RTU_ADDRESS_TEAM         	2 			//组地址-
#define RTU_ADDRESS_BROADCAST    	3 			//广播地址-
#define RTU_ADDRESS_RELAY        	4      		//中继地址-
#if proAdvanceEN == 1
#define RTU_ADDRESS_SELFBROADCAST   5      //-自定义的广播地址(行政区码9999地址0000,回码与发码一致)
#endif

#define RTU_ADDRESS_SPEC_BROADCAST  7      // 特殊广播广播地址，行政区码FFFF地址FFFF，回码时以实际单址回码

//-确认/否认应答-
#define ANS_ASK						0x00		//全确认-
#define ANS_NAK						0xFF		//全否认-
#define ANS_ANK						0x55		//部分确认/否认-//

#define SFP_FEERATE_NUM				4       	//系统支持的费率数-
#define SFP_PnFn_NUM				8			//支持一个DI中Pn*Fn的最大个数-

//-链路层启动站控制码-
#if GB101_104_EN  
#define SFP_CMD_RsetFartrans_HS0    0           //复位远方链路
#endif
#define SFP_CMD_SendConfirm_HS1		1			//发送/确认	复位命令-
#if GB101_104_EN
#define SFP_CMD_SendConfirm_HS2     2			//测试帧
#define SFP_CMD_SendNoReply_HS3     3           //发送确认用户数据
#endif
#define SFP_CMD_SendNoReply_HS4		4			//发送/无回答	用户数据-
#define SFP_CMD_RequestRespond_HS9	9			//请求/响应帧	链路测试-
#define SFP_CMD_RequestRespond_HS10	10			//请求/响应帧	请求1级数据-
#define SFP_CMD_RequestRespond_HS11	11			//请求/响应帧	请求2级数据-

//-链路层从动站控制码-
#define SFP_CMD_Confirm_SS0			0			//确认		认可-
#if GB101_104_EN
#define SFP_CMD_Confirm_SS1			1			//确认		否认-
#endif
#define SFP_CMD_Respond_SS8			8			//响应帧	用户数据-
#define SFP_CMD_Respond_SS9			9			//响应帧	否认：无所召唤的数据-
#define SFP_CMD_Respond_SS11		11			//响应帧	链路状态-

//-应用层功能码AFN-
#define SFP_AFN_Respond				0x00		//确认/否认
#define SFP_AFN_Reset				0x01		//复位
#define SFP_AFN_CheckLink			0x02		//链路接口检测
#define SFP_AFN_RelayStation		0x03		//中继站命令
#define SFP_AFN_SetPara				0x04		//设置参数
#define SFP_AFN_SetCtrl				0x05		//控制命令
#define SFP_AFN_SetPW				0x06		//身份认证及密钥协商
#define SFP_AFN_GetLevelJoin		0x08		//请求被级联终端主动上报
#define SFP_AFN_GetTmnCfg			0x09		//请求终端配置
#define SFP_AFN_GetPara				0x0A		//查询参数
#define SFP_AFN_GetTask				0x0B		//任务数据查询
#define SFP_AFN_GetData1			0x0C		//请求1类数据（实时数据）
#define SFP_AFN_GetData2			0x0D		//请求2类数据（历史数据）
#define SFP_AFN_GetData3			0x0E		//请求3类数据（事件数据）
#define SFP_AFN_TransmitFile		0x0F		//文件传输
#define SFP_AFN_RelayData			0x10		//数据转发
#define SFP_AFN_ChannelTest			0x11		//信道误码测试应答

#define SFP_AFN_SetPara_Ext			0x22		//自定义任务设置参数
#define SFP_AFN_GetPara_Ext			0x23		//自定义任务查询参数


#define NOSECURITY                  0           // 无安全服务
#define NORMALSECURITY              1           // 明文加密
#define ENCRYPTIONSECURITY          2           // 密文加密


#define Max_PhyCh_Num   13

#if GB101_104_EN
#define PRO_DATA_TRANSMIT_APP     0     /*内部协议 - 数据中转传送*/
#define PRO_LOGIN_FRAME_APP       5     /*内部协议 - 登陆帧*/
#define PRO_EXIT_LOGIN_FRAME_APP  6     /*内部协议 - 退出登陆*/
#define PRO_HEART_FRAME_APP       7     /*内部协议 - 心跳帧*/
#define PROT_INIT_CHANNLE_APP     8     /*关闭通道*/
#endif

/*-******************************************************************-
 *-                             通用结构体                           -
 *-******************************************************************-*/

#define pt_char(vari)	(char *)&(vari)						/*-取数据指针char型-*/
#define pt_word(vari)	(uint16 *)&(vari)					/*-取数据指针word型-*/
#define pt_lword(vari)	(long *)&(vari)						/*-取数据指针lword型-*/
#define abs_long(vari)	((vari>=(long)0) ? (vari):-(vari))	/*-long型数据的绝对值-*/

typedef struct									// 年,月,日,时,分,秒,星期
{
	char	ss;									// 0~59 BCD
	char	nn;									// 0~59 BCD
	char	hh;									// 0~23 BCD
	char	dd;									// 1~31 BCD
	char	mm;									// 1~12 BCD
	char	yy;									// 00~99 BCD
	char	ww;									// 0~6 opposite Sun~Sat
}rtc;											//系统时钟结构体

/*-结构体声明-*/
struct rtc_hex
{
	uint8 ss;
  	uint8 nn;
  	uint8 hh;
  	uint8 dd;
  	uint8 mm;
  	uint8 yy;
};

typedef struct
{
    int	addr;
    int  len;
}T_OUT_XRAM;

typedef struct									//-记录存放数据结构-
{
	uint8 bERC;	     							//-记录号-
	uint8 bLen;		 							//-记录长度-
	uint8 bData[128];							//-记录数据内容-
}RECODE_SAVE_T;

typedef struct
{
  int8 min;
  int8 hour;
  int8 day;
  int8 month;
}Date_T;

typedef struct
{
	Date_T pt;									//-最近一次编程时间-
	Date_T ct;              					//-最近一次最大需量清零时间-
	int8 prg_num[2];        					//-编程次数-
	int8 clr_num[2];        					//-最大需量清零次数-
}PROG_INFO_AND_EV_T;

typedef union SFP_SEQ
{
	struct
	{
		uint8	seq:4;
		uint8	con:1;
		uint8	fin:1;
		uint8	fir:1;
		uint8	tpv:1;
	}__attribute__ ((packed)) bit;
	uint8	all;
}__attribute__ ((packed)) SFP_SEQ_U;	//-帧序号字节-

/****************************--帧结构--**********************************
-帧结构格式-
起始字符（68H）	   ↑			1B
长度L			固定长度		2B
长度L			的报文头		2B
起始字符（68H）	   ↓			1B
控制域C			控制域			1B
地址域A			地址域			5B
链路用户数据	应用层			nB
校验和CS		帧校验和		1B
结束字符（16H）					1B

-链路用户数据-
应用层功能码AFN					1B
帧序列域SEQ						1B
数据单元标识1					4B
数据单元1						nB
|								|
数据单元标识n					4B
数据单元n						nB
附加信息域AUX					2B+2B+6B

-附加信息域AUX-
密码://用于重要的下行报文中
PW0								1B
PW1								1B
+
事件计数器EC://用于具有重要事件告警状态的上行报文中
重要事件计数器EC1				1B
一般事件计数器EC2				1B
+
时间标签Tp:	//用于允许同时建立多个通信服务的链路传输和信道延时特性较差的传输中
启动帧帧序号计数器PFC	BIN		1B
启动帧发送时标			秒分时日4B
允许发送传输延时时间	BIN	min	1B
********************************************************************************/

/*************-长度L-*******************************
规约标识定义如下：
D1	D0	规约标识定义
0	0	禁用
0	1	本规约使用(04规约)
1	0	保留
1	1	保留

用户数据长度L1：由D2~D15组成，采用BIN编码，是控制域、地址域、链路用户数据（应用层）的字节总数。
——采用专用无线数传信道，长度L1不大于255；
——采用网络传输，长度L1不大于16383。
******************************************************/

typedef union SFP_LEN
{
	struct
	{
		uint16	sign:2;	//-规约标识-
		uint16	len:14;
	}__attribute__ ((packed)) bit;
	uint16 all;
}__attribute__ ((packed)) SFP_LEN_U;	//-长度码字节，广东规约中没有规约标识，保留是为了和国网兼容-

/******************************-控制域C-************************************************
	        D7	        D6  	    D5	        	D4				D3~D0
下行方向->	传输方向位	启动标志位	帧计数位FCB		帧计数有效位FCV	功能码
上行方向->	DIR			PRM			要求访问位ACD	保留			CMD
***************************************************************************************/
typedef union SFP_CMD
{
	struct
	{
		uint8	cmd:4;							//-控制域功能码-
		uint8	fcv:1;							//-帧计数有效位(下行)-
		uint8	fcb_acd:1;						//-要求访问位(上行方向)-
		uint8	prm:1;							//-启动标志位-
		uint8	dir:1;							//-传输方向位-
	}__attribute__ ((packed)) bit;
	uint8 all;
}__attribute__ ((packed)) SFP_CMD_U;	//-控制码字节-

/********************************-地址域A-******************************
B1
B2
-A2-//FFFFH为广播地址，0000H为无效地址
B1
B2
-A3-
D8	D7	D6	D5	D4	D3	D2	D1	D0
        主站地址MSA            |组地址标志0/1
主站启动的发送帧的MSA不能为0，其终端响应帧的MSA应与主站发送帧的MSA相同。
终端启动发送帧的MSA应为0，其主站响应帧的MSA也应为0。
*************************************************************************/
typedef union SFP_ADR
{
	struct
	{
		uint16	region:16;
		uint16	address:16;
		union
		{
			struct
			{
				uint8	gflg:1;
				uint8	msa:7;
			}__attribute__ ((packed)) bit;
			uint8 all;
		}__attribute__ ((packed)) msa_gflg;
	}__attribute__ ((packed)) bit;
	uint8 all[5];
}__attribute__ ((packed)) SFP_ADR_U;	//-地址码字节-

#if BEIJING_JZQ_EN == 1
typedef struct
{
	uint8	a1_en:1, a2_en:1, rtu_id_en:1, comm_en:1,
			rs232_en:1, rs_en:1, area_en:1, pulseconst_en:1,
			version_en:1,dummy_en:4;
	uint16 a1;				//行政区划码A1
	uint16 a2;				//终端地址A2
	char rtu_id[8];			//设备编号
	char comm_type;			//通讯类型	FF无效；01：短信；02：GPRS/CDMA；03：DTMF；04Ethernet； 05：红外；06：RS232；07：CSD； 08：Radio；09：短信唤醒(GPRS/CDMA);
	uint8 DataBit:2, CheckMode1:1, CheckMode2:1, StopBit:1, BPS:3;
	char rs_type;			//RS通信类型 =0  无RS ;   =1  RS11/15;  =2  RS9/15;
	char area_info;			//地区标识	0 江苏系统  1 南京系统  2 宜兴系统
	char pulseconst_info;	//脉冲常数标识 0 3200 1 6400
	char AreaVersion[4];    //北京集中器通用版本号
	char reserve_buf[4];	//预留的8个字节
	uint8 chksum;			//校验和
}__attribute__ ((packed)) pro_para;
#else
typedef struct
{
	uint8	a1_en:1, a2_en:1, rtu_id_en:1, comm_en:1,
			rs232_en:1, rs_en:1, area_en:1, pulseconst_en:1,
			dummy_en:4;
	uint16 a1;				//行政区划码A1
	uint16 a2;				//终端地址A2
	char rtu_id[8];			//设备编号
	char comm_type;			//通讯类型	FF无效；01：短信；02：GPRS/CDMA；03：DTMF；04Ethernet； 05：红外；06：RS232；07：CSD； 08：Radio；09：短信唤醒(GPRS/CDMA);
	uint8 DataBit:2, CheckMode1:1, CheckMode2:1, StopBit:1, BPS:3;
	char rs_type;			//RS通信类型 =0  无RS ;   =1  RS11/15;  =2  RS9/15;
	char area_info;			//地区标识	0 江苏系统  1 南京系统  2 宜兴系统
	char pulseconst_info;	//脉冲常数标识 0 3200 1 6400
	char reserve_buf[8];	//预留的8个字节
	uint8 chksum;			//校验和
}__attribute__ ((packed)) pro_para;
#endif


//电能表装置的配置参数结构体
typedef struct
{
	uint16  metsn;			//电能表/交流采样装置序号
	uint16 	msn;			//所属测量点号
	uint8 	port;			//通信速率及端口号
	uint8 	protcl;			//通信协议类型
	uint8	addr[6];		//通信地址
	uint8	password[6];	//通信密码
	uint8	bFeeNum;		//电能费率个数
	uint8	bDecNum;		//有功电能示值整数位及小数位个数
	uint8	chlAddr[6];		//所属采集器通信地址
	uint8	classID;		//用户大类号及用户小类号
}__attribute__((packed))Mtr_Set_Para;

//--各个模块需要共享的私有文件的宏定义,add by zjf--

//--规约解释和自动任务公用的一个私有文件--
#define PrtlAndTaskFile SPDATA_PRIFIX"prtl_task_privatefile"

//--参数设置状态私有文件--
#define ParaStateFile SPDATA_PRIFIX"ParastateFile"

/*******************************************************************************
* 系统各类型计量点数量定义
* 系统包括的总加组和运算组数量
*******************************************************************************/
#define DIFF_RULE_NUM		4					//差动规则数量

//终端支持的测量点的数量,前64个485和脉冲可以任意配置；后16个为总加组
//#define Mtr_Num_Max 		(NO_VIR_MTR_MAX_NUM + CtrlGrp_ROUTE + 1)

#define ANOLOGY_MAX_NUM    	8					//模拟量最大数量

#define MTR_ID_485			1					//485计量点号起始
//总加组号起始

#if AC_CALC_EN == 1   							//系统有内置交采时，485计量点号起始以及485计量点数目
#define MTR485_BEGIN	(MTR_ID_485+0)
#else/*--系统无内置交采时，485计量点号起始以及485计量点数目--*/
#define MTR485_BEGIN	MTR_ID_485
#endif

#define SIZE_DADT			(sizeof(DADT))
#define AFN_OK_DEF			0x01					//确认帧
#define AFN_ERROR_DEF		0x02					//否认帧
#define AFN_ERROR_ESAM		0x08					//硬件安全认证错误应答
#define PRTL3761			2
#define MAX_RELAY_NODE		20						//中继命令最多允许的数量
#define REST_PROTECT		1						//防止复位反复下发功能

#define PP_PRINT(x...)		printf(x)

//一下为中继F9、点抄、批量任务等操作时候各个信道超时时间
//为了适应不同表台软件需求，下列值需要根据实际情况调节
#define RELAY_OVERTIME_485	55	//485超时时间，需要在该值基础上除以1000，即1000代表1s
#define RELAY_OVERTIME_PLC	90	//PLC
#define RELAY_OVERTIME_AC	15	//AC
#define REALY_APP_WAIT		5		//为了便于抄表模块有足够超时时间，可以在超时基础上适当增加点延时

#if LINK_BREAKPOINT_EN == 1//链路数据分帧断点标识
#define MAX_TIME_SCALE_NUM      10
#endif
#define MAX_QUEUE_CNT 10

typedef struct DADT_
{
	uint8 DA1;		//信息点DA1
	uint8 DA2;		//信息点DA2
	uint8 DT1;		//信息类DT1
	uint8 DT2;		//信息类DT2
}DADT; //-数据单元标识-

typedef struct DaDtPnFn_
{
	uint8 DA1;		//信息点DA1
	uint8 DA2;		//信息点DA2
	uint8 DT1;		//信息类DT1
	uint8 DT2;		//信息类DT2
	mtrnum_t Pn;	//信息点Pn
	uint16 Fn;		//信息类Fn
}DaDtPnFn; //-数据单元标识 和 分解后的Pn、Fn-


typedef struct
{
	uint8			H1;			//68报文头
	SFP_LEN_U		Len1;		//第一个长度
	SFP_LEN_U		Len2;		//第二个长度
	uint8			H2;			//68报文头
	SFP_CMD_U		Cmd;		//控制码
	SFP_ADR_U		Addr;		//地址域
	uint8			AFN;		//应用层功能码
	SFP_SEQ_U		Seq;		//帧序列域
}__attribute__ ((packed)) FRAME_HEAD;	//报文头信息，从68开始到帧序号SEQ		Wait_Proc

typedef struct
{
	uint8 RecvPipe;			//下行报文的通道号
	char TpInfo[6];			//时间标签信息，如果下行报文中含有该信息，则回码的时候需要原样带回
	int8 AddrType;			//主站命令地址类型
}FRAME_INFO;		//-本帧报文的基本信息-

typedef struct DaDtPnFn_PRTL_
{
	uint8 DA1;			//信息点DA1
	uint8 DA2;			//信息点DA2
	uint8 DT1;
	uint8 DT2;

	mtrnum_t Pn;		//信息点Pn
	uint16 Fn;			//信息类Fn
	/***********************************************************/
	//应答报文的时候使用
	int8 UnpackResult;	//解码的结果 -5——一类数据点抄 -4——否认帧（F4 硬件安全认证错误应答） -3——继续对当前DADT解码 -2——无需应答	-1——否认		0——确认		>0 有应答报文
	uint8 AnswerAFN;	//应答的AFN
	uint16 OutLen;		//报文的长度
	uint8 *OutBuf;		//组好的应答报文
	/***********************************************************/
	struct DaDtPnFn_PRTL_* pNext;	//下一个对象
}DaDtPnFn_PRTL; //-数据单元标识 和 分解后的Pn、Fn-


typedef struct DaDtPack_
{
	uint16			DaDtPnFnCount;		//DaDtPnFn的数量
	DADT			DaDtInfo;			//DADT信息
	DaDtPnFn_PRTL	*pDaDtPnFn;			//首指针，指向每个DaDtPnFn组织的报文，注意这是一个链表，不是普通指针
	struct DaDtPack_ *pNext;			//指向下一个DaDt的报文
}DaDtPack;	//-记录一个完整的数据单元DaDt的组帧报文-

//一帧应答报文，包括68头至16所有的信息
typedef struct AnswerPack_
{
	uint8 RecvPipe;			//发送信道
	uint16 OutLen;			//应答报文的长度
	uint8 *OutBuf;			//组好的应答报文
	struct AnswerPack_* pNext;	//下一个对象
}AnswerPack; //-数据单元标识 和 分解后的Pn、Fn-

#define RELAY_NEW		1	//新点抄任务未处理
#define RELAY_WAIT		2	//点抄命令已发出,等待回码确认
#define	RELAY_OVERTIME	3	//中继超时
#define RELAY_SUCC 		4   //中继回码确认
typedef struct RelayPack_
{
	uint8	RelayType;			//中继类型 RELAY_DT RELAY_TK 等等
	uint8	RelayState;			//当前状态  未执行WAIT 等待回码
	time_t	RelayTimeStart;		//起始秒数
	uint32	RelayOverTime;		//超时秒数
	uint16	RelayDataLen;		//穿透报文长度
	uint8	RelayDataBuf[sizeof(RELAY_IPC_PULS)];	//穿透报文
}RelayPack;

#define RELAY_HEAD_NEW		1	//新点抄任务未处理
#define RELAY_HEAD_WAIT		2	//点抄命令已发出,等待回码确认
#define	RELAY_HEAD_OVERTIME	3	//中继超时
#define RELAY_HEAD_SUCCESS 	4   //中继回码确认
#define RELAY_HEAD_RELACE 	5   //释放链表

typedef struct RelayPackHead_
{
	uint8		RelayHeadPipe;				//下发当前穿透抄报文所在的通道号
	uint8		RelayHeadState;				//当前穿透的状态
	uint8		RelayHeadDataBuf[CCH1_BUFF_SIZE_RCV];	//原始穿透报文
	uint16		RelayHeadDataLen;			//原始穿透报文长度
	RelayPack	*pACRelayPack;				//下发给交采的穿透报文
	RelayPack	*p485RelayPack;				//下发给485的穿透报文（程序暂时不支持多路485口同时中继）
	RelayPack	*pPLCRelayPack;				//下发给PLC的穿透报文
	struct RelayPackHead_ *pNext;			//指向下一帧报文的中继信息
}RelayPackHead;		//-一帧报文的中继命令信息，该报文可以是主站下发的F129一类数据穿透命令，也可以是中继命令F1、F9-



typedef enum
{
    eGetRequestNormal = 1,
    eGetRequestNormalList,
    eGetRequestRecord,
    eGetRequestRecordList,
    eGetRequestNext,
    #if GET_OPTIMIZATION_EN == 1
    eGetRequestConsistentRecord = 13,
    #endif
    #if GET_SIMPLIFYRECORD_EN == 1
    eGetRequestSimplifyRecord = 23,
    #endif
} GetRequestType_E;                 //GET请求的类型

typedef enum
{
    eGetResponseNormal = 1,
    eGetResponseNormalList,
    eGetResponseRecord,
    eGetResponseRecordList,
    eGetResponseNext,
    #if GET_OPTIMIZATION_EN == 1
    eGetResponseConsistentRecord = 13,
    #endif
    #if GET_SIMPLIFYRECORD_EN == 1
    eGetResponseSimplifyRecord = 23,
    #endif
} GetResponseType_E;                //GET请求响应的类型

typedef enum
{
    eSetRequestNormal = 1,
    eSetRequestNormalList,
    eSetThenGetRequestNormalList,
} SetRequestType_E;                 //SET请求的类型

typedef enum
{
    eSetResponseNormal = 1,
    eSetResponseNormalList,
    eSetThenGetResponseNormalList,
} SetResponseType_E;                //SET请求响应的类型

typedef enum
{
    eActionRequestNormal = 1,
    eActionRequestNormalList,
    eActionThenGetRequestNormalList,
} ActionRequestType_E;             //Action请求的类型

typedef enum
{
    eActionResponseNormal = 1,
    eActionResponseNormalList,
    eActionThenGetResponseNormalList,
} ActionResponseType_E;            //Action请求响应的类型

typedef enum
{
    eProxyGetRequestList = 1,
    eProxyGetRequestRecord,
    eProxySetRequestList,
    eProxySetThenGetRequestList,
    eProxyActionRequestList,
    eProxyActionThenGetRequestList,
    eProxyTransCommandRequest,
} ProxyGetRequestType_E;           //Proxy请求的类型

typedef enum
{
    eProxyGetResponseList = 1,
    eProxyGetResponseRecord,
    eProxySetResponseList,
    eProxySetThenGetResponseList,
    eProxyActionResponseList,
    eProxyActionThenGetResponseList,
    eProxyTransCommandResponse,
} ProxyResponseType_E;             //Proxy请求响应的类型


typedef struct{
	uint8 id;//服务ID
	uint8 piid;
	uint8 type;
	uint8 beatPeriod[2];
	Date_Time_t dateTime;
}__attribute__ ((packed)) LinkRequest_t;

typedef struct{
	uint8 id;//服务ID
	uint8 piid;
	uint8 result;
	Date_Time_t reqDateTime;//请求时间
	Date_Time_t recvDateTime;//接收时间
	Date_Time_t respDateTime;//应答时间
}__attribute__ ((packed)) LinkResponse_t;



typedef struct{
	uint8 id;//服务ID
	uint8 piid;
	DateTimeHex_t establishTime;
	DateTimeHex_t curTime;
}__attribute__ ((packed)) ConnectNotification_t;

#if LINK_BREAKPOINT_EN == 1//链路数据分帧断点标识
typedef struct{
    uint8         appSlicingFlag;    //应用分帧标识
    OOP_OAD_U          serviceOAD;        //记录型数据的主oad(如60120300)
    uint8         seviceapdu[3];     //读取型记录的服务类型
    uint16        seq;               //应用分帧序号
    uint16        metsn;             //60120300记录的测量点信息
    DateTimeBcd_t dateTimeTag;       //60120300记录的读数据的时标
    uint8         theLastNTimes;     //60120300记录的剩余未报的上n次
    uint8         ms_cnt;            //60120300单个记录的时间点信息
}__attribute__ ((packed)) LinkBreakPointInfo_t;
#endif

typedef struct ResultData_t_
{
	uint16			        resultDataLen;		// 数据结果长度
	SecurityMode_t          securityMode;       // 加密模式
	uint8                   linkSlicingFlag;    // 0xAA链路层分帧标志，表明此单元是链路层分帧的片段
	uint16                  slicingSeq;         // 链路层分帧相应的序号
	#if LINK_BREAKPOINT_EN == 1
	LinkBreakPointInfo_t    linkbpinfo;
	#endif
	uint8 			        *pResultData;		// 数据结果数据区指针
	struct ResultData_t_    *pNext;			    // 指向下一个数据结果
	struct ResultData_t_    *pFront;			// 指向前一个数据结果
	#if GET_OPTIMIZATION_EN == 1 || GET_SIMPLIFYRECORD_EN == 1
	uint8                   startTime[7];
	#endif
}ResultData_t;

typedef struct FrameData_t_
{
	uint16			        frameDataLen;		// 数据帧长度
	uint8                   pipe;               // 通道号
	uint8                   frameType;          // 帧类型，起始帧，中间帧，结束帧
	uint16                  slicingSeq;         // 链路层分帧相应的序号
	uint8 			        *pFrameData;		// 数据帧数据区指针
	struct FrameData_t_     *pNext;			    // 指向下一个数据结果
	struct FrameData_t_     *pFront;			// 指向前一个数据结果
}FrameData_t;

typedef enum
{
	PROXYST_NEW,//新的代理请求
	PROXYST_RELAY_NEW,//生成一个中继
	PROXYST_RELAY_DEAL,//中继处理中
	PROXYST_RELAY_SUCCESS,//中继处理结束或中继超时
	PROXYST_RELAY_OVERTIMR,
	PROXYST_OVERTIME,//代理超时
	PROXYST_SUCCESS,//代理结束
	PROXYST_RELEASE,//释放代理
}PROXY_STATE; 		/*-穿透抄任务状态-*/

//typedef struct FrameHeadInfo_t_
//{
//    uint8 pipe;//报文通道号
//	LinkLayerCtrl_t ctrl;//控制码
//	OOP_TSA_T tsa;
//}FrameHeadInfo_t;
//
//typedef struct 
//{
//	uint8 PIID;
//	uint8 ServerID;
//	uint8 nChioce;
//	SecurityMode_t SecMod;
//	FrameHeadInfo_t FrameHeadInfo;		//帧头信息
//	PROXY_STATE state;					//穿透抄任务状态
//	time_t RelayStartTime;					//中继开始时间
//	time_t SendTime;						//发送的起始时间
//	uint16 TimeOut;							//任务超时时间
//	uint16 RequestApduLen;					//代理请求的APDU长
//	uint8  pRequestApdu[1400];					//任务请求数据
//
//}Proxyresquest;
//
//typedef struct 
//{
//	uint8 PIID;
//	SecurityMode_t SecMod;
//	FrameHeadInfo_t FrameHeadInfo;		//帧头信息
//	PROXY_STATE state;					//穿透抄任务状态
//	time_t RelayStartTime;					//中继开始时间
//	time_t SendTime;						//发送的起始时间
//	uint16 TimeOut;							//任务超时时间
//	uint16 ResponseApduLen;					//代理请求的APDU长
//	uint8  ResponseApdu[1400];					//任务请求数据
//	uint8 ServerID;
//
//}ProxyResponse;
//
//
//typedef struct
//{
//    uint16  Queue_Len;                          //队列尾
//    uint8*  Queue_Data;               //通讯数据队列
//}Proxy_Queue; 
//
//typedef struct
//{
//	pthread_mutex_t QueueLock;                  //接收缓冲区互斥锁
//    int16  QueueHead;                          //队列头
//    int16  QueueTail;                          //队列尾
//    Proxy_Queue Queue[MAX_QUEUE_CNT];               //通讯数据队列
//}ProxyLockQueue; 

typedef struct{
	uint16 prtlVersion;//协议版本号
	uint16 recvFrameLen;//服务端接收帧长度
	uint16 sendFrameLen;//服务端发送帧长度
	uint16 recvApduLen;//服务端Apdu长度
	uint8 prtlConformance[8];//协议一致性
	uint8 funcConformance[16];//功能一致性
	uint8 recvWindowSize;		//接收窗口大小
	uint32 timeOut;//静态超时时间
}AppContextInfo_t;//应用连接环境信息

typedef struct
{
	uint8 facCode[4];
	uint8 swVer[4];
	uint8 swDate[6];
	uint8 hwVer[4];
	uint8 hwDate[6];
	uint8 exInfo[8];
}FacVer_t;


typedef struct ApduFrameNeedInfo_t_
{
	uint8                   inServerID;         // 服务ID  GET/SET/ACTION等
	uint8                   outServerID;        // 服务ID  GET/SET/ACTION等
	uint16			        choiceVal;		    // 服务中的子项
	uint8                   ppidVal;            // PPID号
	uint16                  blockNo_l;          // 链路层分帧的块序号
	uint16                  blockNo_a;          // 应用层分帧的块序号
}ApduFrameNeedInfo_t;	                    // APDU组帧需要的信息

#if 0
typedef struct SelectTimeTagInfo_t_
{
    DateTimeBcd_t   startTime;                    // 起始时间
    DateTimeBcd_t   endTime;                      // 结束时间
    OOP_TI_T            ti;                           // 时间间隔
    BOOL            firstStartTimeFlag;           // 起始点的第一个点的标志
    uint8           theLastNTimes;                // 上第n次
}SelectTimeTagInfo_t;	                      // 时标信息


typedef struct _TimeFileStruct {
	unsigned int time;
	char chFileName[10];
}TimeFileStruct;
#endif

#if GET_DATA1_RELAY_EN == 1

#define DATA1_RELAY_NEW			1	//新点抄任务未处理
#define DATA1_RELAY_WAIT		2	//点抄命令已发出,等待回码确认
#define	DATA1_RELAY_OVERTIME	3	//点抄超时
#define DATA1_RELAY_PACK 		4   //开始组帧回码

typedef struct Data1RelayHead_
{
	//以下变量在首次解码时候赋值，供中继执行和回码时候使用
	FRAME_HEAD		Data1FrameHead;			//帧头信息
	FRAME_INFO		Data1FrameInfo;			//报文中其他信息
	uint8			Data1TaskFlag;			//任务类型
	DaDtPack		*pData1DaDtPack;		//解码的中间报文
	//以下变量主要在中继过程中使用
	uint8			Data1RelayState;		//当前穿透的状态
	time_t			Data1RelayTimeStart;	//起始秒数
	uint32			Data1RelayOverTime;		//超时秒数
	struct Data1RelayHead_ *pNext;

}Data1RelayHead;		//-一帧报文的中继命令信息，该报文可以是主站下发的F129一类数据穿透命令，也可以是中继命令F1、F9-
#endif

#if EVENT_2014_EN == 1
#define BReadTask_NEW                   1           //新批量抄读任务未处理
#define BReadTask_WAIT                  2           //批量抄读任务命令已发出,等待回码确认
#define BReadTask_OVERTIME              3           //批量抄读任务超时
#define BReadTask_READEVENT             4           //读取终端的事件记录
#define BReadTask_PACK                  5           //开始组帧回码
#define BReadTask_RELACE                8           //释放链表
#define BReadTask_READMETERSTATUS_AGAIN     9       //抄读完成后再读一下电表状态字

typedef struct BReadDI0DI1Pack_     //单个中继信息
{
    char        RelayResult;                //中继处理结果  0xAA:成功，0x00:未发送，0x01-超时，0x03-确认，0x04-否认，0x05-转发接收数据，其他:失败或者超时
    uint8       RelayRetryCount;            //重试次数
    char        DI0DI1Info[4];              //645数据标识信息
    uint8       DIDealFlag;                 //数据标识处理信息（0-允许处理（默认） 1-单抄的事件记录标识 2-事件无关且已经抄读完毕的数据标识 10-重复标识 11-完整事件记录标识 12-未到抄读周期）BIN 1
    uint8       followFramFlag;             //后续帧标识    0x55为后续帧,其它非后续帧
    uint8       followFramSeq;              //后续帧序号
    uint32      EventCountDI;               //所属的数据记录（如果是事件分相记录，则需要记录当前记录对应的事件发生次数的标识）
    uint16      ReadDI0DI1RelayLen;         //中继回复报文长度
    char        ReadDI0DI1RelayBuf[300];    //中继回码的内容
    uint32      LastEventCount;             //前一次事件发生的次数（周期抄读时候使用）
    uint32      curEventCount;              //本次次事件发生的次数（周期抄读时候使用）
}BReadDI0DI1Pack;   //-记录一个完整数据标识的中继信息-

typedef struct BReadTask_
{
    FRAME_HEAD          BatchReadTaskFrameHead;         //帧头信息
    FRAME_INFO          BatchReadTaskFrameInfo;         //报文中其他信息

    uint8               BatchPackDataBuf[CCH1_BUFF_SIZE_RCV];	//上送至主站的报文
    uint16              BatchPackDataLen;			    //上送至主站的报文长度

    uint8               BatchReadDataBuf[1 + 1 + 6 * 32 + 6 + 1];	//原始穿透报文（从“转发中继级数n”到“数据标识类型”，与F9的内容相同）
    uint16              BatchReadDataLen;               //原始穿透报文长度

    uint8               BatchReadTaskMetCom;            //终端通信端口号
    uint8               BReadTaskMtrSeries[6];          //转发目标地址
    uint8               EventStateLenAndBuf[255];      // 主动上报状态字及长度，长度+数据标识+数据,暂存,抄读完成后清除
                                                        // 非全0才清除,否则不清除
    uint8               BatchReadTaskMetPrtl;           //转发直接抄读的数据标识类型：0表示符合DL/T 645—1997，1表示符合DL/T 645—2007
    uint16              BatchReadTaskNum;               //数据标识个数m

    uint8               BatchReadTaskFlag;              //任务类型
    uint8               BatchReadTaskState;             //当前穿透的状态
    uint16              BatchReadTaskDealIndex;         //本次处理的数据标识序号
    time_t              BatchReadTaskTimeStart;         //起始秒数
    uint32              BatchReadTaskOverTime;          //超时秒数
    mtrnum_t            BatchReadTaskMtrNum;            //本次下发的测量点号
    BReadDI0DI1Pack     *pBatchReadTaskDI0DI1Pack;      //每个数据标识的中继信息
    #if JIANG_SU_FUN == 1
    uint32              BatchReadDISet07[8];            //07的状态字对应数据标识
    uint16              BatchReadDISet97[8];            //97的状态字对应数据标识
    uint8               BatchReadStatOnOff[2];          //读取的事件开关状态:字节1为07，字节2为97
    uint8               BatchReadAcqAddr[6];            //上报事件的采集器地址
    #endif
    struct BReadTask_   *pNext;                         //指向下一帧批量任务的中继信息
}BReadTask;     //-“转发主站直接对电能表的批量抄读数据命令F10”的任务头-


#define BREADREPORT_NEW         0       //新任务
#define BREADREPORT_REPORT      1       //上报至主站
#define BREADREPORT_CONFIRM     2       //等待主站确认
#define BREADREPORT_RELACE      8       //释放
typedef struct BReadReport_
{
    uint8               BReportState;               //当前上报的状态
    FRAME_HEAD          BReportFrameHead;           //帧头信息
    FRAME_INFO          BReportFrameInfo;           //报文中其他信息
    uint8               BPackDataBuf[CCH1_BUFF_SIZE_RCV];	//上送至主站的报文
    uint16              BPackDataLen;			            //上送至主站的报文长度
    time_t              BReportTimeStart;           //起始秒数
    uint32              BReportOverTime;            //超时秒数
    uint8               BReportRetryCount;          //重试次数
    struct BReadReport_   *pNext;                           //指向下一帧批量任务的中继信息
}BReadReport;     //-“转发主站直接对电能表的批量抄读数据命令F10”上报队列的任务头-

int BatchRead_Confirm(uint8 ipc_type, uint8 relay_type, uint8 *PIpcBuf, int IpcBufLen);              //抄表模块应答appmain批量抄读的处理
int16 Event2014_MtrEventReport(uint8 ipc_type, uint8 relay_type, uint8 *PIpcBuf, int IpcBufLen);    //电能表主动上报状态字的处理
int16 Event2014_MtrFollowSeq(uint8 ipc_type, uint8 relay_type, uint8 *PIpcBuf, int IpcBufLen);
int16 Event2014_RouterReport(uint8 ipc_type, uint8 relay_type, uint8 *PIpcBuf, int IpcBufLen);          //路由模块主动上报帧的处理
int16 Event2014_DealRestConfirm(uint8 relayPort, uint8 *pIpcBuf, int ipcBufLen);      // 复位完主动上报状态字后接收到确认帧的处理
#endif

#define BATCHRELAY_EN		1			//批量对时功能
#if BATCHRELAY_EN == 1

#define MAX_BATCHRELAY_NODE		2048		//最多允许的批量任务数量
#define MAX_BATCHDATA			128			//任务数据的最大长度
#define BATCHTASK_AUTH			0x00		//身份认证任务
#define BATCHTASK_COMP			0x01		//电表对时任务
#define BATCHTASK_RETRY_TOTAL			3			//执行失败的情况下，重试的次数

typedef enum
{
	//以下为身份认证任务相关状态
	BATCH_AUTH_NEW = 0x30,			//新任务
	BATCH_AUTH_WAIT,				//发送身份认证报文后，等待电能表回码
	BATCH_AUTH_RESEND,				//重新发送
	BATCH_AUTH_FAIL,				//身份认证失败
	BATCH_AUTH_SUCCESS,				//身份认证成功

	//以下为对时任务相关状态
	BATCH_COMP_NEW = 0x50,			//新的对时任务
	BATCH_COMP_DELAY_REQ,			//请求载波信道延时
	BATCH_COMP_DELAY_WAIT,			//请求载波信道延时（等待PLC回码）
	BATCH_COMP_DELAY_RESEND,		//请求载波信道延时（重新发送）
	BATCH_COMP_DEAL,				//对电表进行对时操作
	BATCH_COMP_DEAL_WAIT,			//发送对时操作报文后，等待电能表回码
	BATCH_COMP_DEAL_RESEND,			//重新发送
	BATCH_COMP_FAIL,				//对时任务失败
	BATCH_COMP_SUCCESS,				//对时任务成功
	BATCH_COMP_FINISH,				//对时任务完成
}enumTaskState;

typedef struct
{
	mtrnum_t		TaskMtrNum;			//任务对应的测量点
	uint8			TaskMtrAddr[6];		//任务对应的测量点地址
	time_t			TaskTimeStart;		//任务开始的时间(以数字表示)
	uint8			TaskMtrPort;		//测量点对应的端口号
	uint8			TaskMtrMode;		//测量点对应的波特率参数
	uint16			TaskTimeout;		//发送穿透报文后等待的超时时间

	//身份认证时候电表返回的数据
	char			TaskRand2[4];		//随机数2（身份认证的回码）
	char			TaskEsamID[8];		//ESAM序列号（身份认证的回码）

	//对时时候，ESAM返回的任务密文
	char			TaskMtrDI0DI1[4];	//数据标识4字节
	char			TaskMtrCipherAndMac[16 + 4];	//对时命令的密文 + 4字节MAC

	//对于载波测量点，对时前需要请求信道延时
	uint16			TaskTimeDelay;		//信道延时，单位秒
}BatchTaskProc;		//执行一个任务时候，需要使用到的中间变量

typedef struct BatchTask_
{
	uint64			BatchTaskIndex;						//批量任务的序号（该需要一直累加，只有在转加密授权的时候才归0）
	uint16			BatchTaskFormat;					//任务格式
	uint8			BatchTaskType;						//批量任务类型 BATCHTASK_AUTH BATCHTASK_COMP 等等
	uint8			BatchTaskFailCount;					//当前任务执行失败的次数（对于电能表回否认或者不回码，需要进行重试）
	uint16			BatchTaskInfoDataLen;				//批量任务数据长度
	uint8			BatchTaskInfoData[MAX_BATCHDATA];	//批量任务数据
	uint8			BatchTaskMtrSeries[8];				//表号
	uint8			BatchTaskInfoMtrPw[32];				//表的密钥密文
	enumTaskState	BatchTaskState;						//当前批量任务的状态
	uint8			BatchTaskSendBuf[255];				//发送给电表的报文（备份起来，在重发的时候使用）
	uint8			BatchTaskSendLen;					//报文长度
	rtc_t			BatchTaskCreateTime;				//下发任务的时间
	struct BatchTask_ *pNext;							//指向下一帧批量任务的中继信息
}BatchTask;		//-批量电表任务的任务头-

#define BATCHTASK_AUTH_FILE		"batchtask_auth_file"		//存放主站下发的身份认证参数
#define BATCHTASK_COMP_FILE		"batchtask_comp_file"		//存放主站下发的批量对时参数
#define BATCHTASK_TOTAL_LEN		sizeof(uint64)				//批量任务总数量，在文件中偏移从0开始
#define BATCHTASK_ADDR(taskindex)	(8 + (taskindex) * (sizeof(BatchTask)))			//查询某个批量任务参数在文件中的偏移位置

int BatchTaskAuth_Confirm(uint8 *PIpcBuf, int IpcBufLen);
int BatchTaskComp_Confirm(uint8 *PIpcBuf, int IpcBufLen);
#endif

typedef struct
{
    uint8 listNum;                  // 个数
    uint8 *dataBuf;                 // 数据
    uint16 dataLen;                 // 数据长度
}F209RelatData_t;	                // F209穿透的数据结构体

typedef enum
{
    ACQTYP_NULL=0,				//采集当前数据
    ACQTYP_LASTN,				//采集上第N
    ACQTYP_FRZTM,				//冻结时标采集
    ACQTYP_TI,					//按时标间隔采集
    ACQTYP_BC,					//补抄
}AcqType_t;

typedef struct
{
    uint8     planId;
    uint16    storDepth;
    AcqType_t acqType;
    uint8     acqValue;
    OOP_TI_T      ti;
#if HENAN_FUN == 1 || GANSU_FUN == 1||ANHUI_FUN == 1 || SICHUAN_FUN ==1 || HUBEI_FUN == 1
    uint8     MSType;
#endif
    uint8     userType[255];
    uint8     userNum;
    OOP_OAD_U      mainOad;
    uint16    subOadNum;
    const OOP_OAD_U *oadList;
    uint8     storType;
}__attribute__ ((packed))NORPLAN_PARA_INFO_T;       //默认参数

typedef struct
{
    uint8 oadNum;
    const OOP_OAD_U *defOad;
}PLAN_DEFOAD_INFO_T;

typedef struct
{
    uint16 EvtOI;                   //事件标识
    uint8  ReportEn;                //上报使能
} EVENT_DEFAULT_TYPE;

typedef struct
{
    OOP_TI_T runCycle;
    uint8 plantype;
    uint8 plan_id;
    DateTimeHex_t taskStartTime;
    DateTimeHex_t taskEndTime;
    OOP_TI_T delayTime;
    uint8 priority;
    uint8 state;
    uint16 Pre_RunScript;
    uint16 Over_RunScript;
    uint8 TimePeriodType;
    uint8 TimePeriordNum;
    uint8 TimePeriod[4];

}__attribute__ ((packed))DefTask_Para_INFO_T;

typedef struct
{
    uint8      planId;
    OOP_TI_T       Delay;//上报响应超时时间
    uint8      MaxReportCount;// 1字节,最大上报次数
    OOP_OAD_U       mainOad;
    OOP_TI_T       Ti;  //数据间隔
    uint8      userType[255];
    uint8      userNum;
}__attribute__ ((packed))REPPLAN_PARA_INFO_T;

//#if LIAONING_TICK_EN == 1
#define printt(x...) ({if (getenv("DEBUG_TICK") != NULL)  printf(x);})

typedef enum
{
    MASTER_TO_TMN_IDLE = 0,
    MASTER_TO_TMN_NEW,                  //终端请求时钟新任务(准备发出心跳)
    MASTER_TO_TMN_WAIT,                 //终端请求时钟等待接收状态(等待心跳回码)
    MASTER_TO_TMN_OVERTIME,             //终端请求时钟等待超时状态
    MASTER_TO_TMN_PRECESE_SUCC,         //终端请求时钟接收完成

    MASTER_TO_TMN_NEW_SEC,              //终端再次请求时钟新任务
    MASTER_TO_TMN_WAIT_SEC,             //终端再次请求时钟等待接收状态
    MASTER_TO_TMN_OVERTIME_SEC,         //终端再次请求时钟等待超时状态
    MASTER_TO_TMN_PRECESE_SUCC_SEC,     //终端再次请求时钟接收完成

    MASTER_TO_TMN_PRECESE_NEXTWAIT,     //终端下次请求时钟
} MASTER_TO_TMN_STAT; //主站对终端的对时任务状态

typedef enum
{
    TMN_TO_METER_IDLE = 0,
    TMN_TO_METER_NEW,
    TMN_TO_METER_RDINIT_FIRST,         //终端首次对电表查询时钟初始化
    TMN_TO_METER_RDWAIT_FIRST,         //终端首次对电表查询时钟的等待
    TMN_TO_METER_FIRST_OVERTIME,       //终端首次对电表查询时钟超时等待
    TMN_TO_METER_FIRST_SUC,            //终端首次对电表查询时钟成功标志

    TMN_TO_METER_SETTIME_INIT,         //终端对电表设置时钟的初始化
    TMN_TO_METER_SETTIME_WAIT,         //终端对电表设置时钟的等待
    TMN_TO_METER_SETTIME_OVERTIME,     //终端对电表设置时钟的超时
    TMN_TO_METER_SETTIME_SUC,          //终端对电表设置时钟的成功

    TMN_TO_METER_RDINIT_SEC,           //终端再次对电表查询时钟初始化
    TMN_TO_METER_RDWAIT_SEC,           //终端再次对电表查询时钟的等待
    TMN_TO_METER_SEC_OVERTIME,         //终端再次对电表查询时钟超时等待
    TMN_TO_METER_SEC_SUC,              //终端再次对电表查询时钟成功标志

} TMN_TO_METER_STAT; //终端对电表对时任务状态

typedef enum
{
    NULL_TASK = 0,                   //无当前任务
    MASTER_TO_TMN_PRECESE_TASK,      //主站对终端的精确对时任务
    TMN_TO_METER_TASK,               //终端对电表对时任务
} TIMETASKTYPE; //对时任务类型

typedef struct
{
    uint8     MasterToTmn_Mode;                // 1字节: 对时模式
    uint8     MasterToTmn_HeartRate;           // 1字节：最近心跳时间总个数
    char      MasterToTmn_MaxEliminatedNum;    // 1字节：最大值剔除个数
    char      MasterToTmn_MinEliminatedNum;    // 1字节：最小值剔除个数
    uint8     MasterToTmn_HaltdelayTime;       // 1字节：通讯延时阈值（单位：秒）
    uint8     MasterToTmn_LeastEffectiveNum;   // 1字节：最少有效个数
} __attribute__ ((packed)) MASTER_TO_TMN_PARA_STRUCT; //终端精确对时相关参数

typedef struct
{
    uint8     TmnToMeter_TimeFlag;  // 1字节: 终端是否启动电表校时模式
    uint8     TmnToMeter_Mode;      // 1字节: 对时模式 2:终端广播校时 3:终端单地址广播校时参数
    uint8     TmnToMeter_Start[3];  // 3字节：终端广播校时启动时间
    char      TmnToMeter_Limit;     // 1字节：时钟误差阈值(单位：秒)
} __attribute__ ((packed)) TMN_TO_METER_PARA_STRUCT; //电表对时参数

typedef struct
{
    TIMETASKTYPE    TimeTaskType;   //对时任务类型(上电简单对时任务 精确对时任务  终端给电表对时任务)
    uint8           TaskSucBit;     //当天任务完成标志(上电简单对时任务 精确对时任务  终端给电表对时任务)
} TIMETASK_STRUCT;
//#endif

#if SAL_PRTL == 1
typedef struct
{
    uint8   func        :4;	//功能码
    uint8   flag        :1;	//主站触发协商标志
    uint8   subf        :1;	//分帧标志
    uint8   PRM         :1;	//启动标志位
    uint8   DIR         :1;	//传输方向位
} __attribute__ ((packed)) SAL_FC_WORD_B;

typedef union
{
    SAL_FC_WORD_B      BitSect;
    uint8  			   byte;
} __attribute__ ((packed)) SAL_FC_BYTE;

typedef struct
{
    uint8   ESAMmode        :4;	//加密算法 0:明文 1:CBC密文
    uint8   reserve         :4;	//备用
    uint8   var             :8;	//SAL协议版本
} __attribute__ ((packed)) SAL_SV_B;

typedef union
{
    SAL_SV_B           BitSect;
    uint16              Word;
} __attribute__ ((packed)) SAL_SV_WORD;

typedef struct
{
	uint8   PrtlVer        :5; //应用层协议版本
	uint8   reserve        :3; //保留
	uint8   AddrClass      :3; //地址类型
	uint8   DevClass       :5; //设备类型
} __attribute__ ((packed)) SAL_DAT_B;
typedef union
{
	SAL_DAT_B      bitsect;
	uint16           word;
} __attribute__ ((packed)) SAL_DAT_WORD;

typedef struct _DA_t
{
	union
	{
		struct
		{
			uint8 termlen    :4;      //行政地址+逻辑地址长度
			uint8 mslen      :3;      //主站地址长度
			uint8 reserve    :1;      //保留
		}DAL_B;
		uint8 byte;
	}DAL;                               //设备地址长度
	uint8          DAC[8];                //设备地址
}__attribute__ ((packed)) DA_t;

typedef struct _SA_t
{
	union
	{
		struct
		{
			uint8 len:5;                 //源地址长度
			uint8 portLen:3;                //端口长度
		}SAL_B;
		uint8 byte;
	}SAL;
	uint8          SAC[16];               //设备地址
}__attribute__ ((packed)) SA_t;

typedef struct _TA_t
{
	union
	{
		struct
		{
			uint8 len:5;                 //源地址长度
			uint8 portLen:3;                //端口长度
		}TAL_B;
		uint8 byte;
	}TAL;
	uint8          TAC[16];              //设备地址
}__attribute__ ((packed)) TA_t;

typedef struct _CI_t
{
	union 
	{
		struct
		{
			uint8 len:4;                    //信道信息长度
			uint8 type:4;                   //通信信道类型
		}CIL_B;
		uint8 byte;
	}CIL;
	uint8          CIC[15];                 // 通道信息
}__attribute__ ((packed)) CI_t;

typedef struct _TP_t
{
	uint8 yyH;
	uint8 yyL;
	uint8 mon;
	uint8 day;
	uint8 hour;
	uint8 min;
	uint8 sec;
}__attribute__ ((packed)) TP_t;

typedef struct _GA_t
{
	uint8          GAL;                    // 网关长度
	uint8          GAC[6];                 // 网关信息
}__attribute__ ((packed)) GA_t;

typedef struct _CD_t
{
	SAL_FC_BYTE    FC;                      //功能码
	SAL_SV_WORD    SV;                      //SSAL协议版本和加密算法
	SAL_DAT_WORD   DAT;                     //设备地址类型
	DA_t           DA;                      //设备地址
	SA_t           SA;                      //源地址
	TA_t           TA;                      //目标地址
	CI_t           CI;                      //信道信息
	TP_t           TP;                      //时间标签
	GA_t           GA;                      //网关信息	
}__attribute__ ((packed)) CD_t;

typedef struct _SALHeadInfo_t
{
	uint8          bStartword;               // 起始字符
	uint16         L;                        // SAL长度 
	uint16         SEQ;                      // 帧序号
	uint16          C;                       // 控制码
	CD_t           CD;                       // 控制数据
	uint16         HCS;                      // 帧头校验
	uint16         RevInfo;                  // 返回信息 应答时有
	uint16         len698;                   // 下行数据区长度，回码时复用为回码数据区长度
}__attribute__ ((packed)) SALHeadInfo_t;    // SAL帧头数据结构体  

typedef struct _SALSubFrame_t
{
	uint8          bStartword;               // 起始字符
	uint16         L;                        // SAL长度 
	uint16         SEQ;                      // 帧序号
	uint16          C;                       // 控制码
	CD_t           CD;                       // 控制数据
	uint16         HCS;                      // 帧头校验
	uint16         RevInfo;                  // 返回信息 应答时有
	uint16         len698;                   // 下行数据区长度，回码时复用为回码数据区长度
	uint8 *        salbuf;                   //数据内容缓冲区
	struct _SALSubFrame_t *next;
}__attribute__ ((packed)) SALSubFrame_t;   //分帧信息结构 

//终端返回错误码
//安全认证错误（Base：0x1000）
#define SAL_RECVINFO_ERROR_TERMINAL_DECIPHERING 0x1001       //终端解密错误
#define SAL_RECVINFO_ERROR_TERMINAL_SIGNCHECK   0x1002       //终端验签失败
#define SAL_RECVINFO_ERROR_TERMINAL_MAC         0x1003       //终端MAC校验失败
#define SAL_RECVINFO_ERROR_TERMINAL_SN          0x1004       //会话计数器错误
#define SAL_RECVINFO_ERROR_GW_DECIPHERING       0x1005       //网关解密错误
#define SAL_RECVINFO_ERROR_GW_SIGNCHECK         0x1006       //网关验签失败
#define SAL_RECVINFO_ERROR_GW_MAC               0x1007       //网关MAC校验失败
#define SAL_RECVINFO_ERROR_GW_PASSWD            0x1008       //网关密码单元故障
#define SAL_RECVINFO_ERROR_LINK_PASSWD          0x1009       //链路设备密码单元故障
//帧校验错误(Base：0x2000)
#define SAL_RECVINFO_ERROR_DATA					0x2000       //帧解析错误(终端维护)
#define SAL_RECVINFO_ERROR_SAL_SV               0x2001       //协议版本错误
#define SAL_RECVINFO_ERROR_DES_MISMATCHING      0x2002       //加解密算法标志不匹配
#define SAL_RECVINFO_ERROR_DEVICE_TYPE          0x2003       //设备类型无法识别
#define SAL_RECVINFO_ERROR_CONTROL              0x2004       //控制域无法识别
#define SAL_RECVINFO_ERROR_DIR                  0x2005       //传输方向位错误
#define SAL_RECVINFO_ERROR_APDU_LEN_ABMORMAL    0x2006       //数据域长度异常（小于4字节）
#define SAL_RECVINFO_ERROR_APDU_LEN             0x2007       //数据域长度不匹配
//链路错误(Base：0x3000)
#define SAL_RECVINFO_ERROR_NO_TARGET_NDOE       0x3001       //目标节点不存在
#define SAL_RECVINFO_ERROR_NO_SESSION_LINK      0x3002       //当前会话链路未建立
#define SAL_RECVINFO_ERROR_SEND_INFO            0x3003       //报文发送失败
#define SAL_RECVINFO_ERROR_CI                   0x3004       //信道错误
#define SAL_RECVINFO_ERROR_SESSION_NEGOTIATION  0x3005       //当前链路会话协商失败

// 最大可处理的APDU长度
#define APDU_BUFF_SIZE_MAX		4096

#define SAL_SUBFRAME                    0x7FFFFFF0  //分帧
#define SAL_SUBFRAME_START              0x7FFFFFF1  //分帧起始帧
#define SAL_SUBFRAME_MID                0x7FFFFFF3  //分帧中间帧
#define SAL_SUBFRAME_END                0x7FFFFFF2  //分帧结束帧
#define SAL_SUBFRAME_GW_ANSWER          0x7FFFFFF4  //网关对终端上行分帧的应答
#define SAL_FRAM_CHECK_FAIL 			-3			//SAL协议解帧失败
#define SAL_PROCESS_FAIL 				-2			//SAL回码处理过程出错，回否认
#define SAL_LINK_FRAME 					-1			//SAL链路管理帧,不需回码?
#define SAL_NONE_698_FRAME 				0			//纯SAL帧，不含698内容，直接回码
#define SAL_PROCESS_SUCCESS             1           //解帧成功

//控制码对应位
#define SAL_C_FC               (1<<15)     //功能码
#define SAL_C_SV               (1<<14)     //协议版本
#define SAL_C_DAT              (1<<13)     //设备类型
#define SAL_C_DA               (1<<12)     //设备地址
#define SAL_C_SA               (1<<11)     //原地址
#define SAL_C_TA               (1<<10)     //目的地址
#define SAL_C_CI               (1<<9)      //通道信息
#define SAL_C_TP               (1<<8)      //时间标签
#define SAL_C_GA               (1<<7)      //网关地址
#endif

#if TMNHALT_METHOD ==1
//int Pro_RelayHalt(uint8 *PIpcBuf, int IpcBufLen);
#endif

extern int ToIPCUpdateFTP( uint8 *pBuf, int nBufLen );
int PrtlPro_CreatFrameHead(FRAME_HEAD *pFrameHead);			//获取主动上报的报文头信息
int PrtlPro_Makeframe(FRAME_HEAD FrameHead, FRAME_INFO FrameInfo, char *pInBuf, uint16 InbufLen, char *pOutBuf, uint16 *OutbufLen);
int GetNextTimeTag(SelectTimeTagInfo_t STInfo, PlanTypeInfo_t planTypeInfo, OadQueryInfo_t oadQueryInfo, DateTimeBcd_t *dateTimeTag);
#if GPRS_ETH_ALL_OLINE == 1
extern int PrtlPro_Link(uint8 pipe, int bProcFlag, char *pOutBuf, uint16 *OutbufLen);
#else
extern int PrtlPro_Link(int bProcFlag, char *pOutBuf, uint16 *OutbufLen);
#endif

extern int16 Event2014_MtrEventCountRead(void);
extern int16 BatchRead_TaskProc(uint8 *pipe, uint8 *pOutBuf, uint16 *pOutBufLen);
extern int16 BatchReport_TaskProc(uint8 *pipe, uint8 *pOutBuf, uint16 *pOutBufLen);

#endif
