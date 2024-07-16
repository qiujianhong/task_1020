/*
***************************************************************************
  Copyright (c) 2006-2007,南京新联电子仪器有限责任公司
             All rights reserved.
  程序名称: commdef.h
  功    能：放一些公用的结构定义。比如在终端数据和配置参数中都使用终端地址结构的时候。
  开 发 人：gaoyf
  开发时间：2007-1
  
  版本历史:
  
    v1.0:
      2006-1-19, gaoyf : New file        
  
***************************************************************************
*/

#ifndef _COMMDEF_H_
#define _COMMDEF_H_

#define MAX_SEL_POINT	VIR_BEGIN-1

extern const char* g_pczChnlType[];	// 通道类型
extern const char* g_pczChnlName[];	// 通道名称
extern const char* g_pczMeasureBaud[];// 波特率字符串
extern const char* g_pczMeasureData[];// 数据位字符串
extern const char* g_pczMeasureStop[];// 停止位字符串
extern const char* g_pczMeasureParity[];// 奇偶校验位字符串
extern const char* g_pczMeasureFlag[];// 计量点状态
extern const char *g_pczGroup[];// 总加组序号字符串
extern unsigned g_nGroups;// 总加组最大个数(376.1规约支持8个总加组)
extern const char* g_pczDirect[];		// 方向标志
extern const char* g_pczOperator[];	// 运算符标志

extern const char* g_pczMeasureType[];// 测量点状态
extern char**g_pczMeasureChnl;//测量点通道
extern const char* g_pczMeasureMode[];//接线方式

extern const char* g_czCommType[];//通信类型
extern const char* g_czCommProtocol[];//通信协议
extern const char* g_czCommMode[];//通信模式
extern const char* g_pczMeterPort[];	//通道类型


/*--计量点类型定义--*/
#define MTR_485       1   /*-485表-*/
#define MTR_SIM       2   /*-模拟表-*/
#define MTR_PULSE     3   /*-脉冲-*/
#define MTR_VIR       4   /*-虚拟-*/
#define MTR_AC        5   /*-交采-*/
#define MTR_PLC       6   /*-载波-*/
#define MTR_PLCMUI    7   /*-载波多功能-*/


// 终端地址
typedef struct _TERMINAL_ADDR
{
    int     nIndex;     	// 当前索引
    int		nState;
    char    czAddr[20];		// 终端地址
    char    czCode[20];		// 行政区码
    char    czSn[21];		// 终端编号
    char    oopAddr[20];	// OOP终端地址
    uint8   ip[4];
}TERMINAL_ADDR;

#if COMMPARA_BAK_EN == 1
//备份专网
typedef struct _BACKUP_VIRNET
{
    int     nIndex;     	// 当前索引
    int     czIP_M;		// 主IP
    int     czPort_M;    // 主端口
    int     czIP_E;		// 备用IP 端口
    int     czPort_E;    // 备用端口
    char    czAPN[20];      // APN
    char    czName[40];		// 用户名
    char    czCode[40];     // 密码
}BACKUP_VIRNET;
#endif

// 用于通道配置，在初始化时读取所有值，在应用时保存值
typedef struct _TOTAL_CHNL_INFO
{
    int     nChnlType;	// 主通道类型
	int     nChnlType1;	// 通道1类型
	int     nChnlType2;	// 通道2类型

	int		nChnlMode;	// 主通道通信模式
	int		nChnlMode1;	// 通道1通信模式
	int		nChnlMode2;	// 通道2通信模式

	int		nIp1;
	int		nPort1;
	int		nIp2;
	int		nPort2;
	int		nIp3;
	int		nPort3;

	int		nGwIp;
	int		nGwPort;

	int		nProxyIp;
	int		nProxyPort;

	int		nNetType;
	int		nProtocolType;

	int		nPeriods;//心跳周期

	char	czAPN[40];
	char	czUsr[40];
	char	czPwd[40];

	int		nSleepTime;
	int		nHoldTime;
	int		nLoginInter;
	int		nLoginMaxUnit;

	char	czGroup1[20];
	char	czCode1[20];
	char	czGroup2[20];
	char	czCode2[20];
	char	czGroup3[20];
	char	czCode3[20];

	int     nBaudRate1;  // 波特率
    int     nData1;      // 数据位
    int     nStop1;      // 停止位
	BOOL    nParity1;    // 0:无  1:奇位 2偶位
	int     nBaudRate2;  // 波特率
    int     nData2;      // 数据位
    int     nStop2;      // 停止位
	BOOL    nParity2;    // 0:无  1:奇位 2偶位
	int     nBaudRate3;  // 波特率
    int     nData3;      // 数据位
    int     nStop3;      // 停止位
	BOOL    nParity3;    // 0:无  1:奇位 2偶位
	
	int		nDelay;		// 20ms的倍数
	uint8	nConnectAppMode; //连接应用方式

	#if WORK_MODE == 1
	uint8	work_type;		/*工作模式 0～2依次表示混合模式、客户机模式、服务器模式*/
	#endif

	BOOL	bSave;
}TOTAL_CHNL_INFO;


// 通道信息结构
typedef struct _CHNL_INFO
{
    int     nIndex;     // 当前索引
	
    int     nChannel;
	int		nMode;
    int     nType;

	PLUGIN_INFO** ppDetail;
	PLUGIN_INFO*  pCurPlugin;
	int nPluginCnt;
	int nCurPlugin;
}CHNL_INFO;

#if MODULARIZATION_TMN_EN == 1
// 通道信息结构
typedef struct _PROTO_INFO
{
    int     nIndex;     // 当前索引
	
    int     nType;      // 规约类型

	//PLUGIN_INFO** ppDetail;
	//PLUGIN_INFO*  pCurPlugin;
	//int nPluginCnt;
	//int nCurPlugin;
}PROTO_INFO;
#endif

// 通道信息 GPRS
typedef struct _GPRS_INFO
{
	int		nNetType;
	int		nProtocolType;
	
	int		nIp;
    int     nPort;
    int     nGWIp;
    int     nGWPort;
    int     nPeriods;
	int     nProxyIp;
    int     nProxyPort;
	
    char    czAPN[40];
    int     nSel;			// 选中项
	#if WORK_MODE == 1
	uint8	work_type;		/*工作模式 0～2依次表示混合模式、客户机模式、服务器模式*/
	uint8 	nConnectAppMode;
	#endif
}GPRS_INFO;

// 通道信息 GPRS
typedef struct _GPRS_EXT
{
	int		nSleepTime;
	
	int		nHoldTime;
	int		nLoginInter;	//超时时间
	int		nLoginMaxUnit;	//重发次数

	char    czUsr[40];
    char    czPwd[40];
	
    int     nSel;       // 选中项
    
}GPRS_EXT;

// 通道信息 Ethernet
typedef struct _ETHERNET_INFO
{
	int		nIp;
    int     nPort;
    int     nGWIp;
    int     nGWPort;
	int     nProxyIp;
    int     nProxyPort;
	int		nProtocolType;

	#if WORK_MODE == 1
	uint8	work_type;		/*工作模式 0～2依次表示混合模式、客户机模式、服务器模式*/
	#endif

    int     nSel;       // 选中项
    
}ETHERNET_INFO;

#if GPRS_ETH_ALL_OLINE == 1
// 通道信息 Ethernet  g_ethCommPara
typedef struct _ETHCOMMPARA_INFO
{
	int		 nIp;            // 主用IP
    int      nPort;          // 主用端口号
	int		 nBackIp;        // 备用IP
    int      nBackPort;      // 备用端口号
    int      nGWIp;          // 网关IP
	int      nProxyIp;       
    int      nProxyPort;
	int		 nProtocolType;  
    int      nPeriods;       // 心跳周期
	uint8	 work_type;		// 工作模式 0～2依次表示混合模式、客户机模式、服务器模式
	uint8 	 nConnectAppMode;	//连接应用方式
    int		 nLoginInter;	//超时时间
	int		 nLoginMaxUnit;	//重发次数 	 
    int      nSel;           // 选中项
    
}ETHCOMMPARA_INFO;
#endif

// 通道信息 短信
typedef struct _NOTE_INFO
{
    int     nIndex;     // 当前索引
    char    czGroup[20];
    char    czCode[20];
    int     nTime;
}NOTE_INFO;

// 通道信息 无线电 RS232
typedef struct _WIRELESS_INFO
{
    int     nIndex;     // 当前索引
	int		nType;		// 0:无线电 1：RS232
	
	int     nBaudRate;  // 波特率
    int     nData;      // 数据位
    int     nStop;      // 停止位
	BOOL    nParity;    // 0:无  1:奇位 2偶位

	int		nDelay;		// 20ms的倍数
	//#if WDASN_RADIO_EN == 1	
	int		nLoginInter;		//重拨间隔 2 BIN	
	int		nLoginMaxUnit;		//重拨次数 1 BIN	
	//#endif
}WIRELESS_INFO;

// 总加组配置
#define GROUP_OFFSET	VIR_BEGIN-1//NO_VIR_MTR_MAX_NUM	// 总加组相对于计量点的偏移量(当前为63)
typedef struct _GROUP_MAIN
{
    int     nIndex;     // 当前索引
	
	int		nCur;		// 当前总加组
	int		nState;		// 状态
}GROUP_MAIN;

typedef struct _GROUP_CONFIG_INFO
{
	char    TsaAddr[17]; //Tsa地址
    uint8   Direct;      //方向
    uint8   Op;          //累加
}GROUP_CONFIG_INFO;

// 总加组详细配置
typedef struct _GROUP_DETAIL
{
    int     nSel;       // 选中项
	int     nPage;      // 页号
	unsigned char nMeasureInfo[16][3];
	GROUP_CONFIG_INFO GroupConfInfo[16];
	char	sAddr[16][32];
	// 测量点号
	// 0 无效 1正 2 反
	// 0 无效 1+ 2-
	PLUGIN_INFO*	pConfig;
	
}GROUP_DETAIL;




// 测量点参数
typedef struct _MEASURE_MAIN
{
    int     nIndex;     // 当前索引
	
	PLUGIN_INFO**	ppDetail;	// 详细设置(485)
	PLUGIN_INFO*	pCurPlugin;
	int				nPluginCnt;
	int				nCurPlugin;
	
	int		nState;		// 测量点状态
	int		nType;		// 测量点类型
	
	int     nCT;        // CT变化
    int     nPT;        // PT变化
}MEASURE_MAIN;



// 485表参数

typedef struct _FIRST_485
{
	int     nMeaSeq;    // 电能表序号
    int     nProto;     // 协议
	int		nChnl;		// 通道
	int		nBaudRate;	// 波特率
    char    czAddr[20]; // 地址 
	int     nMode;     // 接线方式
    float   fV;         // 电压
	float	fA;         // 电流
	char 	password[10];

    int     nSel;       // 选中项
}FIRST_485;

typedef struct _TOTAL_MEASURE_INFO
{
	int		nState;		// 测量点状态
	int		nType;		// 测量点类型
	
	int     nCT;        // CT变化
    int     nPT;        // PT变化

	int     nMeaSeq;    // 电能表序号
	int     nProto;     // 协议
	int		nChnl;		// 通道
	int		nBaudRate;	// 波特率
    char    czAddr[20]; // 地址 
	BOOL	bChangeAddr;
	int     nMode;      // 接线方式
    float   fV;         // 电压
	float	fA;         // 电流
	int     nUsType;    //用户类型
	char    assetnum[20];// 资产号
	int		assetlen;
	char    bcdCollectorAddr[20];// 所属采集器地址(BCD码格式)
	int 	nRatenum;	//费率数
	char 	password[6];//密码

	char	nMeasureInfo[16][3];
	GROUP_CONFIG_INFO GroupConfInfo[16];
	BOOL	bChangeGroup;

	BOOL	bSave;
	BOOL    bClear;

}TOTAL_MEASURE_INFO;

void SwapAscii(char* pBuf, int nLen);
void TextOutStr(HDC hdc, int nLeft, int nTop, char* pczText);
void CurPointReturn(char* pNewStr, void* pPara);
double type2todoble(char* pBuf);
int type3toint(char* pBuf);
#endif//_LAYER_H_

