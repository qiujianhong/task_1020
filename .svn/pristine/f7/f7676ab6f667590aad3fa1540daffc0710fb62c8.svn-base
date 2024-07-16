/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved.
* @brief：   104消息模块头文件
* @date：    2020-3-4
* @history：
*********************************************************************
*/
#ifndef _PROTOCOL104_H
#define _PROTOCOL104_H

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


//按国网最新实施细则修改如下时间及计数201907
#define MAX_FRAMCOM_DELAY_T1   20  //发送帧未收到确认最大延时时间
#define IFRAMCOM_TIMES_MAX_K   12  //I帧未确认发送帧最大数量
#define IFRAMCOM_TIMES_MAX_W   8   //I帧未确认接收帧最大数量
#define MAX_FRAMCOM_DELAY_T2   10  //接收I帧最长给予确认的间隔时间
#define MAX_FRAMCOM_DELAY_T3   30  //联络检测时间间隔



#define YX_START_NUM       	0x0001 	//遥信∶信息对象地址范围为1H~1000H。
#define MAX_YX_NUM               4  //终端遥信数量，路数增加要考虑数据字典的增加
#define JDPROT1_START_NUM  	0x1001 	/*继电保护1∶信息对象地址范围为1001H~2000H，对应带CP56Time2a时标的继电保护设备事件、
									  带CP56Time2a时标的继电保护设备成组启动事件、带CP56Time2a时标的继电保护设备成组输出
									  电路信息*/
#define JDPROT2_START_NUM  	0x2001 	//继电保护2∶信息对象地址范围为2001H~3000H，为继电保护动作的故障电流区。
#define JDPROT3_START_NUM  	0x3001 	//继电保护3∶信息对象地址范围为3001H~4000H，为继电保护定值设定区。
#define YC_START_NUM       	0X4001 	//遥测∶信息对象地址范围为4001H~5000H。
#define MAX_YC_NUM             59   //遥测数据数量，路数增加要考虑数据字典的增加
#define PARA_START_NUM  	0x5001	//参数地址范围为5001H~6000H。
#define PARA_MAX_NUM        20
#define YK_START_NUM 		0x6001	//遥控、升降地址范围为6001H~6200H。
#define SET_START_NUM 		0x6201	//设定地址范围为6201H~6400H。
#define ENERGY_START_NUM 	0x6401	//电能累计量地址范围为6401H~6600H。
#define MAX_ENDATA_NUM      8  		//电量数据个数
#define STEP_START_NUM 		0x6601	//步位置信息地址范围为6601H~6700H。
#define TRSBIT_START_NUM 	0x6701	//向被控站传送二进制信息地址范围为6701H~6800H。
#define BCD_START_NUM 		0x6801	//BCD码地址范围为6801H~6900H。
#define RTUREMOT_START_NUM 	0x6901	//远动终端状态地址6901H。
#define FILE_START_NUM 		0x6902	//文件传送地址6902H~7000H
#define MAX_YK_NUM			2		//遥控路数，路数增加要考虑数据字典的增加
#define FAULT_MAX_NUM       40      //故障信息上报支持信息体最大个数
#define FALT_START_NO       0x0005  //故障信息上报信息体地址开始点号
#define FALT_START_ADDR     0x0101  //故障信息上报信息体地址开始点号

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
#define STEP_CONT_DEFAULT           0xaa//空闲
/****************************************************************/
/****************************************************************/
//IEC101、104规约电量总召应答过程进程定义
/***************************************************************/
#define ENDATA_STEP_SEND_START       0
#define ENDATA_STEP_SEND_DATA        1
#define ENDATA_STEP_SEND_DELAY       2
#define ENDATA_STEP_SEND_STOP        3
#define ALLDATA_STEP_SEND_DEFAULT   0xaa
/****************************************************************/

/****************************************************************/
//IEC104规约应用服务应答总召过程进程定义
/***************************************************************/
#define ALLDATA_STEP_SEND_YX        0
#define ALLDATA_STEP_SEND_YC        1
#define ALLDATA_STEP_SEND_STOP      2
#define ALLDATA_STEP_SEND_DEFAULT   0xaa
/****************************************************************/


/* **************************************************************/
//业务相关应用类型定义
#define APP_TYPE_NOMMAL           		0x00 //通用
#define APP_TYPE_FILE_ABOUT             0X01 //文件、参数相关
#define APP_TYPE_CTRL_SELECT_ACK       	0x02 //遥控选择确认等，需带终端随机数
#define APP_TYPE_PARA_ACK               0X03 //参数确认等
#define APP_TYPE_CTRL_SELECT			0X05 //遥控选择、遥控撤销
#define APP_TYPE_CTRL_RUN				0x07 //遥控执行
#define APP_TYPE_UPDATA				    0x08 //升级相关


//网关与终端双向认证应用类型定义
#define APP_TYPE_NET_RAND               0x20//网关发送随机数
#define APP_TYPE_MTU_AUTH               0x21//终端向网关认证申请
#define APP_TYPE_NET_AUTH               0x22//网关返回终端的认证结果，并向终端申请认证
#define APP_TYPE_AUTH_ACK               0x23//认证结果返回
#define APP_TYPE_NET_SERI_RES           0x24//网关申请终端及芯片序列号
#define APP_TYPE_NET_SERI_ACK           0x25//终端返回终端序列号及加密芯片序列号

//证书管理工具应用类型 定义
#define TOOL_TYPE_CHECK_RES_START       0x30//证书管理工具发起认证请求
#define TOOL_TYPE_CHECK_RES_START_ACK   0x31//终端响应证书管理工具的认证请求
#define TOOL_TYPE_CHECK_RES        		0x32//证书管理工具对终端认证请求的响应
#define TOOL_TYPE_CHECK_RES_ACK         0x33//终端向证书管理工具返回认证结果，应用类型0x33
#define TOOL_TYPE_KEY_VER_RES           0x34//获取终端密钥版本
#define TOOL_TYPE_KEY_VER_RES_ACK       0x35//终端返回密钥版本
#define TOOL_TYPE_MTU_ID_RES            0x36//获取终端序列号
#define TOOL_TYPE_MTU_ID_RES_ACK        0x37//终端返回终端序列号
#define TOOL_TYPE_MTU_SERI_RES          0x38//获取终端内置安全芯片序列号
#define TOOL_TYPE_MTU_SERI_RES_ACK      0x39//终端返回内置安全芯片的序列号
#define TOOL_TYPE_MTU_KEY_RES           0x3A//获取终端安全芯片公钥
#define TOOL_TYPE_MTU_KEY_RES_ACK       0x3B//终端返回公钥
#define TOOL_TYPE_CERT_FILE_RES         0x3C//证书管理工具将生成的证书请求文件发给终端
#define TOOL_TYPE_CERT_FILE_RES_ACK     0x3D//向证书管理工具返回签名结果
#define TOOL_TYPE_CERT_FILE_DOWN        0x3E//证书管理工具将配电应用CA证书、主站证书、网关证书、终端证书导入终端
#define TOOL_TYPE_CERT_FILE_DOWN_ACK    0x3F//终端返回证书导入结果
#define TOOL_TYPE_CERT_REBACK_RES       0x40//终端初始证书回写
#define TOOL_TYPE_CERT_REBACK_RES_ACK   0x41//终端返回初始证书回写结果
#define TOOL_TYPE_KEY_CERT_RES          0x42//证书管理工具导出终端公钥证书
#define TOOL_TYPE_KEY_CERT_SEND    		0x43//终端向证书管理工具返回终端证书
#define TOOL_TYPE_KEY_CERT_RES_ACK      0x44//证书管理工具向终端返回证书导出结果
#define TOOL_TYPE_KEY_CERT_SEND_ACK     0x45//分帧数据传输结果返回
#define TOOL_TYPE_MTU_KEY_REBACK        0x46//证书管理工具恢复终端密钥
#define TOOL_TYPE_MTU_KEY_REBACK_ACK    0x47//终端向证书管理工具返回密钥恢复结果
#define TOOL_TYPE_APP_DATA_RES          0x48//证书管理工具向终端发送配电数据
#define TOOL_TYPE_APP_DATA_RES_ACK      0x49//终端向证书管理工具发送配电数据



//主站与终端双向认证应用类型定义
#define APP_TYPE_HOST_RAND              0x50//主站发送随机数
#define APP_TYPE_CLIENT_AUTH            0x51//终端向主站认证申请
#define APP_TYPE_HOST_AUTH              0x52//主站返回终端的认证结果，并向终端申请认证
#define APP_TYPE_HOST_ACK               0x53//认证结果返回主站
#define APP_TYPE_HOST_SERI_RES          0x54//主站申请终端及芯片序列号
#define APP_TYPE_HOST_SERI_ACK          0x55//终端返回加密芯片序列号
#define APP_TYPE_HOST_CODE_RES          0x56//主站申请终端特征码
#define APP_TYPE_HOST_CODE_ACK          0x57//终端返回终端ID及芯片序列号
#define APP_TYPE_MANA_CODE_RES          0x58//主站通过管理通道申请终端特征码
#define APP_TYPE_MANA_CODE_ACK          0x59//终端返回终端ID及芯片序列号

//主站与终端密钥管理应用类型定义
#define APP_TYPE_KEY_VER_RES            0x60//主站向终端申请密钥版本
#define APP_TYPE_KEY_VER_ACK            0x61//终端返回密钥版本
#define APP_TYPE_KEY_CHANGE_RES         0x62//主站发起密钥更新
#define APP_TYPE_KEY_CHANGE_ACK         0x63//终端返回密钥更新结果
#define APP_TYPE_KEY_REBACK_RES         0x64//主站发起密钥恢复
#define APP_TYPE_KEY_REBACK_ACK         0x65//终端返回密钥恢复结果

//主站与终端证书管理应用类型定义
#define APP_TYPE_CERT_CHANGE_RES        0x70//主站发起证书更新
#define APP_TYPE_CERT_CHANGE_ACK        0x71//终端返回证书更新结果
#define APP_TYPE_CERT_DOWN_RES          0x72//主站发起证书下载
#define APP_TYPE_CERT_DOWN_ACK          0x73//终端返回证书下载结果
#define APP_TYPE_CERT_GET_RES        	0x74//主站获取终端证书
#define APP_TYPE_CERT_GET_ACK        	0x75//终端返回证书
#define APP_TYPE_CERT_GET_OVER        	0x76//主站返回证书提取结果


//业务错误代码 错误说明
#define BUSS_ERR_APP_TYPE   			0x9101//业务应用类型错误
#define BUSS_ERR_MESS_CHACK 			0x9102//报文验签失败
#define BUSS_ERR_MESS_DECI  			0x9103//报文解密失败
#define BUSS_ERR_RAND_CHECK 			0x9104//随机数验证失败
#define BUSS_ERR_TIME_CHECK 			0x9105//时间校验失败
#define BUSS_ERR_APP_SAFE   			0x9106//业务安全要求不合规，不符合报文安全规范要求如需要加密的报文未进行加密
#define BUSS_ERR_APP_FLOW   			0x9107//业务安全流程非法 如没有按照“ 先认证，后业务”处理
#define BUSS_ERR_POWER_LOW  			0x9108//权限不够
#define BUSS_ERR_NONE_KNOW  			0x9109//未知错误
#define BUSS_ERR_MESS_UNPACK   			0x9110//报文长度有误或解析失败
#define BUSS_ERR_MESS_CRC  				0x9111//CRC 错误
#define BUSS_ERR_MESS_MAC   			0x9112//MAC 错误

//安全扩展错误码
#define DASAFE_SUCCSESS 				0x9000//成功
#define DASAFE_ERR_CHACK 				0x9090//认证失败
#define DASAFE_ERR_KEY_CHANGE 			0x9091//密钥更新失败
#define DASAFE_ERR_KEY_REBACK  			0x9092//密钥恢复失败
#define DASAFE_ERR_CERT_IN				0x9093//证书导入失败
#define DASAFE_ERR_CERT_OUT 			0x9094//证书导出失败
#define DASAFE_ERR_CERT_GET 			0x9095//证书提取失败
#define DASAFE_ERR_MESS_RECV 			0x9096//分帧数据接收失败
#define DASAFE_ERR_CERT_DOWN 			0x9097//证书远程更新/下载失败

/**************************************************************/



/****************************************************************/
//IEC101、104规约应用服务数据单元之类型标识
/***************************************************************/
//          类型标识   序号		 定义
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
#define		M_FT_NA_1   42      //国网新实施细则定义的故障信息上报类型，pjhtest
#define		C_SC_NA_1  	45		//单点命令
#define		C_DC_NA_1  	46		//双点命令
#define		C_RC_NA_1  	47		//步调节命令
#define		C_SE_NA_1	48		//设定值命令, 规一化值
#define		C_SE_NB_1	49		//设定值命令, 标度化值
#define		C_SE_NC_1	50		//设定值命令, 短浮点数
#define		C_BO_NA_1	51		//32比特串
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
#define		C_SR_NA_1   200     //国网最新实施细则参数切换类型标识，pjhtest
#define		C_RR_NA_1   201     //国网最新实施细则参数读定值区号类型标识，pjhtest
#define		C_RS_NA_1   202     //国网最新实施细则读单个或多个定值/参数类型标识，pjhtest
#define		C_WS_NA_1   203     //国网最新实施细则写单个或多个定值/参数类型标识，pjhtest
#define		M_IT_NB_1   206     //国网最新实施细则累计量，短浮点数，pjhtest
#define		M_IT_TC_1   207     //国网最新实施细则带CP56Time2a时标的累计量，短浮点数，pjhtest
#define		F_SC_NA_2   210     //国网最新实施细则文件操作类型标识，pjhtest
#define		F_SR_NA_1_N 211     //国网最新实施细则软件升级启动/结束类型标识，pjhtest



//                             传送原因                             序号		定义
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

#define MAX_OAD_COLS_NUM (2)

// 面向对象值 转换系数方式
typedef enum
{
    E_OOP_TO_REAL=0,  //面向对象值 转换为 真实值
	E_REAL_TO_OOP,  //真实值 转换为 面向对象值
	E_DISPATCH_TO_REAL,  //调度值 转换为 真实值
	E_REAL_TO_DISPATCH,  //真实值 转换为 调度值
}CONVERT_TYPE_E;

// 遥信数据结构
//(数据字典中存放的位置加data_bit位决定了遥信量的位置
//state_bit位决定该数据是否要上送)
typedef struct
{
	uint16  objAddr;	 //遥信点号
	uint32	data_code;   //数据字典中存放的位置
	uint8  	data_bit;	 //当前数据存放的bit位
	uint8   state_bit;	 //遥信是否变位存放bit位
}YX_DATA_CONFIG;

//遥信上报数据结构
typedef struct
{
	uint16	objAddr;    //遥信点号
	uint8	state_sign; //当前状态是否发生变化标志1:变化；0:无变化
	uint8	state;		//状态
	uint8   btime[7];   //-数据发生变化的时间-
}YX_DATA_SEND;

// 遥测数据结构
//遥测配置信息结构体
typedef struct
{
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
	float32 fFullValue;  //满码值（用于归一化遥测值）
}YC_DATA_CONFIG;

//遥测实时数据结构体（用于上报）
typedef struct
{
	float32 fNewValue;    //实时数据
	float32 fHisValue;    //历史数据
	BOOL bSendFlag;       //是否上报
	BOOL bChangeFlag;     //是否变化
	BOOL bHisValidFlag;   //历史数据是否有效
	uint8   btime[7];     //-数据发生变化的时间-
}YC_DATA_SEND;

// 遥控数据结构
typedef struct
{
	uint16	objAddr;    //遥控点号
	uint32	data_code;  //数据字典中存放的位置
	uint8 	bSelect;	// 遥控路数<每一路均包含该路的--分--合-->
}YK_DATA_CONFIG;

//电量配置信息结构体
typedef struct
{
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
}EN_DATA_CONFIG;

typedef struct //遥测上报配置数据结构
{
	uint16 objAddr;         //-信息对象地址，可认为是GB376.1中的物理测量点+数据标识的绑定-
	uint32 data_code;		//-数据字典编码-
}APP_PARA_CONFIG;

//故障信息上报参数数据结构，上报按带时标的单点遥信上报
typedef struct
{
	uint16 nObjAddr;      //信息对象地址（加上起始地址的实际值）
	char   *szObjName;    //信息对象名称
	uint32 nSignalType;   //类型标识（故障信息）
	uint32 nFaultYxAddr;  //故障遥信地址
	uint8  nReportNum;      //上报遥测数据个数
	uint16 aReportAddr[12]; //上报遥测信息体地址(取遥测点表信息体地址的存放位置)
	uint32 nDbCenterOAD;  //数据中心查询标签（OAD）
	uint8  nItemIndex;    //分项序号（从0开始）
}FAULT_REPORT_CONFIG;

//故障信息上报参数数据结构（实时），上报按带时标的单点遥信上报
typedef struct //故障信息上报参数数据结构，上报按带时标的单点遥信上报
{
	uint8       bfgchange;      //变位标志
	uint16 		objAddr;		//信息体地址
	uint8 		bstate;			//遥信状态
	uint8       btime[7];       //变位发生时间
	uint8       reportnum;      //上报遥测数据个数
	uint8       datatype;       //上报遥测数据类型
	uint16      reportdata[240]; //上报数据缓冲区
}FAULT_REPORT_SEND;

//104规约接收到I帧的帧序号
typedef struct
{
	uint16	fir:1;			//-I帧类型标志-
	uint16	framenum:15;	//-帧序号-
}__attribute__ ((packed)) RI_BIT;

typedef union SFP_R_NUM
{
	RI_BIT rbit;
	uint8 crtltype34[2];
}__attribute__ ((packed)) SFP_R_NUM_U;//-接收序号

typedef union SFP_S_NUM
{
	RI_BIT ibit;
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
}__attribute__ ((packed)) SFP_S_NUM_U;//-发送序号

//104规约帧数据结构
typedef struct
{
	uint8			head;		  //位置0，68报文头
	uint8           len;          //位置1，报文长度
    SFP_S_NUM_U   	sCrtlType12;  //位置2，控制域1、2
	SFP_R_NUM_U     sCrtlType34;  //位置4，控制域3、4
	uint8           bdatatype;    //位置6，类型标识,1个字节
	uint8           Vali;		  //位置7，可变结构限定词,1个字节
	uint8           CommCauseL;   //位置8，传送原因，2字节，这个是低字节
	uint8           CommCauseH;   //位置9，传送原因，2字节，这个是高字节
	uint8           PubAddrL;	  //位置10，公共地址，1字节，这个是低字节
	uint8           PubAddrH;	  //位置11，公共地址，1字节，这个是高字节
    uint8           *pdata;       //位置12，信息体
}__attribute__ ((packed)) S104FRAME_HEAD;


typedef union DA_SFP_S_NUM
{
    struct
	{
		uint16	base:8;			//-备用-   这个归约高字节在前
		uint16	keyport:3;	    //-密钥标识标志位-
		uint16	encry:1;	    //-加密标志位-
		uint16	none:2;	        //-备用-
		uint16	from:2;	    	//-相对终端连接对象-
	}__attribute__ ((packed)) ubit;
	uint8 commtype[2];
}__attribute__ ((packed)) DA_SFP_S_NUM_U;//-报文类型-

//配网加密规约帧数据结构
typedef struct
{
	uint8			headH;		  //0xEB报文头
	uint8           lenH;         //报文长度高字节
	uint8           lenL;         //报文长度低字节
	uint8			headL;		  //0xEB报文头
	DA_SFP_S_NUM_U  commtype;     //报文类型
	uint8           apptype;      //应用类型
}__attribute__ ((packed)) DASAFE_FRAME_HEAD;


//国网2019年2月发布最新规范
typedef struct   //参数存储数据结构
{
	uint16			bInfor;		  //信息体地址
	uint16          dParaID;      //参数存储ID
    uint8   	    bRunType;     //参数设置类型，0xaa可读写参数，0xbb表示只读类型
    uint8           bLen;         //参数长度
}__attribute__ ((packed)) PARA_INFO_CONFIG;

//end of 国网2019年2月发布最新规范

//方案档案状态
typedef enum
{
    TASK_CFG_UNINIT,              //未初始化
    TASK_CFG_MODIFY,              //修改
    TASK_CFG_MODIFY_DELAY,        //修改延时
    TASK_CFG_INITED,              //已初始化
    TASK_CFG_REBUILD,             //重建采样文件
    TASK_CFG_PATCH,               //创建补抄文件
} TASK_CFG_STATE;

#define MTR_MAX_NUM        	    2040				//最大测量点个数

//电表档案
typedef struct
{
    pthread_rwlock_t              rwLock;                     //读写锁 多线程访问
    TASK_CFG_STATE                FileState;                 //档案状态
    int                           ModifyDelaySec;            //方案配置修改延时
    time_t                        FunLastRunTime;            //方案上次修改时间
    uint16                        MeterCount;                //电表总个数
    OOP_METER_T                   MeterFile[MTR_MAX_NUM];    //每个电表的档案
}METER_FILE_T;

#endif

