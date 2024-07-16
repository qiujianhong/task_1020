#ifndef _OOPALL_H_
#define _OOPALL_H_

#ifndef _DLL_IMPLICIT_
#define _DLL_IMPLICIT_
#endif

#if 0
#include    <stdio.h>       /*标准输入输出定义*/
#include    <stdlib.h>      /*标准函数库定义*/
#include    <unistd.h>      /*Unix 标准函数定义*/
#include    <sys/types.h>   /*系统类型定义*/ 
#include    <sys/stat.h>    /*系统状态定义*/
#include    <fcntl.h>       /*文件控制定义*/
#include    <termios.h>     /*PPSIX 终端控制定义*/
#include    <errno.h>       /*错误号定义*/
#include    <dlfcn.h>       /*动态库控制定义*/
#include    <string.h>      /*字符处理函数定义*/
#include    <sys/time.h>    /*时间函数*/
#include    <time.h>        /*时间函数*/
#include    <pthread.h>     /*线程函数库定义*/
#include    <sys/ipc.h>     /*信号量*/
#include    <signal.h>      /*系统信号定义*/
#include    <sys/file.h>    /*文件系统定义*/
#include    <semaphore.h>   /*信号量*/
#include    <sys/sem.h>
#include    <linux/rtc.h>
#include    <sys/shm.h>
#include    <sys/mman.h>
#include    <limits.h>
#include    <dirent.h>
#include    <stdarg.h>
#include    <assert.h>
#include    <libgen.h>
#include    <ctype.h>
#include    <getopt.h>
#include    <netinet/in.h>
#include    <arpa/inet.h>
#include    <sys/socket.h>
#include    <sys/wait.h>
#include    <sys/select.h>
#include    <sys/reboot.h>
#include    <sys/msg.h>
#include    <math.h>
#include    <sys/ioctl.h> 
#include    <sched.h>
#include    <asm/types.h>
#include    <linux/types.h>

#include    "dataType.h"
#include    "dataCommon.h"
#include    "oopType.h"
#include    "gpio.h"
#include    "mem.h"
#include    "para_id.h"
#include    "data_id.h"
#include    "oad_id.h"
#include    "ramxdata_id.h"
#include    "sys_cfg.h"
#endif

#include    "common.h"

//typedef  int time_t;
extern uint16 CRC16_Get(const uint8* buf, uint32 len);

//链路层数据结构定义
#define LINK_FRAME_LEN	 27	//STX(1)+LEN(2)+Ctrl(1)+A(1+16+1)+HCS(2)+FCS(2)+ETX(1)  链路层数据长度
#define MAX_LINK_LENGTH  2048+LINK_FRAME_LEN


#pragma pack(1)
typedef struct{
	union{
		uint8 af;//地址标识
		struct{
			uint8 af_len:4;//服务器地址长度 0~15分别标识长度1~16
			uint8 af_logicAddr:2;//保留
			uint8 af_typ:2;//地址类型
		};
	};
	uint8 sa[16];//服务端地址
	uint8 ca;//客户端地址
}LinkLayerAddr_t;

enum eUnit{
	YEAR=1,
	MONTH,
	WEEK,
	DAY,
	HOUR,
	MIN,
	SEC,
	ANGLE,					//角度 °
	DEGREE,					//摄氏度 ℃
	UCURRENCY,				//货币
	METER,					//米
	MperS,
	
	LITER = 19,
	KG,
	NEWTON,
	NPERM,
	PASCA,
	BAR,
	JOULE,

	W =27,
	KW,
	VA,
	KVA,
	VAR,
	KVAR,
	KWH,
	KVAH,
	KVARH,
	AMPERE,

	VOLT = 38,
	HZ = 47,
	IMPWH,
	IMPVARH,
	IMPVAH,
	PER,

	UBYTE = 254,
	NOUNIT= 255,
};
/*enum eAccessRight{
	NOACCESS,
	READONLY,
	WRITEONLY,
	READ_WRITE,

	ACCESS = 1,
};*/
typedef struct{
	int8 multi;
	uint8 unit;
}UnitMulti_t;

typedef struct{
	uint8 attr_id;//属性或方法ID
	OOP_ATTRIBUTE_ACESS_E right;//访问权限 
}AccessRight_t;
#define ADDRTYPE_SINGLE           0   //单地址
#define ADDRTYPE_MULTICAST    2   //多播地址
#define ADDRTYPE_GROUP           2   //组地址
#define ADDRTYPE_BROADCAST   3   //广播地址
typedef union{
	uint8 ctrl;
	struct{
		uint8 ctrl_fun:3;//功能码
		uint8 ctrl_discode:1;   //扰码标志位
		uint8 ctrl_rev:1;       //保留
		uint8 ctrl_seg:1;//分帧标识 0:不分帧，1:分帧
		uint8 ctrl_prm:1;//启动标识
		uint8 ctrl_dir:1;//传输方向
	};
}LinkLayerCtrl_t;
#define CTRLFUN_PRECONNECT        1  //预连接管理
#define CTRLFUN_DATAEXCHANGE   3  //应用连接和数据交换管理
typedef struct{
	uint8 head;//帧起始符
	uint16 len;//帧长度
	LinkLayerCtrl_t ctrl;
	LinkLayerAddr_t a;
	uint16 hcs;//帧头校验
	uint8 *apdu;
	uint16 fcs;//帧校验
	uint8 tail;//帧结束符
}LinkLayerFrame_t;

typedef union{
	uint16 Flag;
	struct{
		uint16 frameNo     :12;//帧序号 0~4095循环使用
		uint16 rev         :2; //保留
		uint16 frameType   :2; //00:起始帧，01:最后帧，10:确认帧，11:中间帧
	};
}SegFlag_t;
#define SEGFLAG_FIRST_FRAME		0 //起始帧
#define SEGFLAG_LAST_FRAME		1 //最后帧
#define SEGFLAG_CONFRIM_FRAME	2 //确认帧
#define SEGFLAG_MID_FRAME		3 //中间帧


typedef struct strLinkLayer{
	uint8 pipe;//物理通道号
	OOP_TSA_T sa;//服务器地址
	uint8 sendBuff[MAX_LINK_LENGTH];
	uint8 resvBuff[MAX_LINK_LENGTH];
	uint16 sendLen;
	uint16 resvLen;
	uint8 *apdu;
	uint16 apduOffset;
	uint16 apduLen;
	time_t lastTime;//上一次正常通信时间
	uint16 frameCnt;//帧计数
	SegFlag_t sendSegflg;//发送分帧标识
	SegFlag_t recvSegflg;//接收分帧标识
	uint16 timeOut;//超时剩余时间， 单位s
	LinkLayerCtrl_t ctrl;//控制吗
	LinkLayerAddr_t a;
	struct strLinkLayer *next;
}LinkLayer_t;
#pragma pack()

typedef struct{
	OOP_OAD_U oad;
	uint8 rsdLen;
	uint8 *rsd;
	uint16 rcsdLen;
	uint8 *rcsd;
}GetRecord_t;
typedef enum{
	eOopComm_Success=0,//通信成功
	eOopComm_SegSend,//分帧发送
	eOopComm_SegRecv,//分帧接收
	eOopComm_GetNext,//读后续帧

	eOopComm_Fail = 0x10,//通信错误
	eOopComm_AppCacheOver,//应用层缓存溢出
	eOopComm_FrameNotIntergrity,//数据帧不完整
	eOopComm_AddrNotMatch,//地址不匹配
	eOopComm_OADNotMatch,//对象描述符不匹配
	eOopComm_FrameNoNotMatch,//帧序号不匹配
	eOopComm_ServerIDNotMatch,//服务ID部匹配
	eOopComm_SecurityNotMatch,//安全数据不匹配
	
	eOopComm_ConnectPswErr = 0x70,//密码错误
	eOopComm_ConnectSymKeyErr,//对称秘钥错误
	eOopComm_ConnectAsymKeyErr,//非对称秘钥错误
	eOopComm_ConnectSignaKeyErr,//签名秘钥错误
	eOopComm_ConnectNoSecData,//加密信息缺失
	eOopComm_ConnectRandLenErr,//随机数长度错误
	
	eOopComm_DarSuccess = 0x80,				//0成功
	eOopComm_DarHardWareFault,		//硬件失效
	eOopComm_DarTemporaryFailure,		//临时性失效
	eOopComm_DarDenied,				//读写拒绝
	eOopComm_DarObjectNoDefine,		//对象未定义
	eOopComm_DarClassIdErr,			//接口类不匹配
	eOopComm_DarClassIdNoDefine,		//对象不存在
	eOopComm_DarDataTypeErr,			//数据类型不匹配
	eOopComm_DarOver,				//越界
	eOopComm_DarDataBlockErr,			//数据块不可用 （9）
	eOopComm_DarSlicingCacel,			//分帧传输已取消 （10 ）
	eOopComm_DarSlicingDenied,			//不处于分帧传输状态 （11 ）
	eOopComm_DarBlockCacel,			//块写取消 （12 ）
	eOopComm_DarBlockDenied,			//不存在块写状态 （13 ）
	eOopComm_DarBlockErr,				//数据块序号无效 （14 ）
	eOopComm_DarPSWErr,					//密码错/未授权(15)
	eOopComm_DarCommBpsErr,				//通信波特率不能修改(16)
	eOopComm_DarTmZoneNumOver,			//年时区数超(17)
	eOopComm_DarTmSegNumOver,			//日时段数超(18)
	eOopComm_DarTriffNumOver,			//费率个数超(19)
	//eOopComm_Dar						//安全认证不匹配(20)
	//eOopComm_Dar						//重复充值(21)
	//eOopComm_Dar						//ESAM验证失败(22)
	eOopComm_DarAuthErr,				//认证错误(23)
	eOopComm_DarCustomErr,				//客户编号不匹配(24)
	//eOopComm_Dar,						//充值次数错误(25)
	//eOopComm_Dar						//购电超囤积(26)
	//eOopComm_Dar						//地址异常(27)
	//eOopComm_Dar						//对称解密错误(28)
	//eOopComm_Dar						//非对称解密错误(29)
	//eOopComm_Dar						//签名错误(30)
	//eOopComm_Dar						//密钥挂起(31)
	//eOopComm_Dar						//时间标签无效(32)
	eOopComm_DarOther=255,			//其它 （255255255）
}eOopCommRslt_t;

//服务层数据结构定义
#define MAX_APDU_LENGTH 4096
enum{
	SERVERID_LINKREQUEST = 1,
	SERVERID_CONNECTREQUEST,
	SERVERID_RELEASEREQUEST,
	SERVERID_GETREQUEST=5,
	SERVERID_SETREQUEST,
	SERVERID_ACTIONREQUEST,
	SERVERID_REPORTRESPONSE,
	SERVERID_PROXYREQUEST,
	SERVERID_SECURITYREQUEST=16,

	SERVERID_LINKRESPONSE = 129,
	SERVERID_CONNECTRESPONSE,
	SERVERID_RELEASERESPONSE,
	SERVERID_RELEASENOTIFICATION,
	SERVERID_GETRESPONSE,
	SERVERID_SETRESPONSE,
	SERVERID_ACTIONRESPONSE,
	SERVERID_REPORTREQUEST,
	SERVERID_PROXYRESPONSE,
	SERVERID_SECURITYRESPONSE=144,
};


typedef struct str_ApduList{
	uint16 frameNo;//帧序号
	uint16 apduLen;//APDU长度
	uint16 offSet;//偏移量
	uint8 apdu[MAX_APDU_LENGTH];
	//struct str_ApduList *next;
	struct str_ApduList *prior;
}ApduList_t;
typedef struct{
	uint8 connectId;//请求的应用连接的ID
	uint8 prtlVersion[2];//协议版本号
	uint8 prtlConformance[8];//协议一致性
	uint8 funcConformance[16];//功能一致性
	uint16 sendFrameLen;//发送帧长度
	uint16 recvFrameLen;//接收帧长度
	uint8 recvFrameNum;//接收帧个数
	uint16 maxApduSize;//最大可处理APDU尺寸
	uint8 passWordLen;
	uint8 passWord[8];
	uint8 randLen;
	uint8 rand[8];//随机数
}ConnectInfo_t;
typedef struct str_AppLayerList{
	uint8 pipe;
	OOP_TSA_T sa;
	uint8 connectId;//应用连接级别 0:预连接的应用连接，1~其他级别的应用连接
	time_t establishTime;//应用连接建立时间
	uint8 serverId;
	uint32 timeOut;//应用连接超时时间
	SecurityMode_t secMod;
	ConnectInfo_t serverConnectInfo;
	ConnectInfo_t clientConnectInfo;
	ApduList_t *sendApduList;//ApduList
	ApduList_t *recvApduList;//ApduList
	struct str_AppLayerList *next;
}AppLayerList_t;

//request ，confirm 由发起端调用(一般由客户机发起，LINK服务由服务器发起)
//indication,response 由接收端调用(一般由服务机接收，LINK服务由客户机接收)
typedef void (*OopLinkRequest_t)(void) ;
typedef void (*OopLinkResponse_t)(void) ;
typedef struct{
	OopLinkRequest_t requst;
	OopLinkResponse_t confirm;
	OopLinkRequest_t indication;
	OopLinkResponse_t response;
}OopLink_t;


typedef void (*OopConnectRequest_t)(uint8 ,OOP_TSA_T*,uint8,uint32) ;
typedef eOopCommRslt_t (*OopConnectResponse_t)(AppLayerList_t *) ;
typedef struct{
	OopConnectRequest_t requst;
	OopConnectRequest_t indication;
	OopConnectResponse_t response;
	OopConnectResponse_t confirm;
}OopConnect_t;

typedef void (*OopReleaseRequest_t)(uint8 ,OOP_TSA_T*,uint8) ;
typedef eOopCommRslt_t (*OopReleaseResponse_t)(AppLayerList_t *) ;
typedef eOopCommRslt_t (*OopReleaseNotification_t)(AppLayerList_t *);
typedef struct{
	OopReleaseRequest_t requst;
	OopReleaseRequest_t indication;
	OopReleaseResponse_t response;
	OopReleaseResponse_t confirm;
	OopReleaseNotification_t Notification;
}OopRelease_t;

/*typedef void (*OopGetRequestNormal_t)(void) ;
typedef void (*OopGetResponseNormal_t)(void) ;

typedef struct{
	OopGetRequestNormal_t normal;
	OopGetRequestNormal_t normalList;
	OopGetRequestNormal_t record;
	OopGetRequestNormal_t recordList;
	OopGetRequestNormal_t next;
}OopGetRequest_t;

typedef struct{
	OopGetResponseNormal_t normal;
	OopGetResponseNormal_t normalList;
	OopGetResponseNormal_t record;
	OopGetResponseNormal_t recordList;
	OopGetResponseNormal_t next;
}OopGetResponse_t;*/


typedef void (*OopGetRequest_t)(uint8 ,OOP_TSA_T* ,uint8 ,uint8*,uint16);
typedef eOopCommRslt_t (*OopGetConfirm_t)(AppLayerList_t*);
typedef struct{
	OopGetRequest_t requst;
	OopGetRequest_t indication;
	OopGetConfirm_t response;
	OopGetConfirm_t confirm;
}OopGet_t;

typedef void (*OopSetRequest_t)(uint8 ,OOP_TSA_T* ,uint8 ,uint8*,uint16);
typedef eOopCommRslt_t (*OopSetConfirm_t)(AppLayerList_t*);
/*typedef struct{
	OopSetRequestNormal_t normal;
	OopSetRequestNormal_t normalList;
	OopSetRequestNormal_t thenGetNormalList;
}OopSetRequest_t;

typedef struct{
	OopSetResponseNormal_t normal;
	OopSetResponseNormal_t normalList;
	OopSetResponseNormal_t thenGetNormalList;
}OopSetResponse_t;*/
typedef struct{
	OopSetRequest_t requst;
	OopSetRequest_t indication;
	OopSetConfirm_t response;
	OopSetConfirm_t confirm;
}OopSet_t;


typedef void (*OopActionRequest_t)(uint8 ,OOP_TSA_T* ,uint8 ,uint8*,uint16);
typedef eOopCommRslt_t (*OopActionConfirm_t)(AppLayerList_t*);


/*typedef struct{
	OopActionRequestNormal_t normal;
	OopActionRequestNormal_t normalList;
	OopActionRequestNormal_t thenGetNormalList;
}OopActionRequest_t;

typedef struct{
	OopActionResponseNormal_t normal;
	OopActionResponseNormal_t normalList;
	OopActionResponseNormal_t thenGetNormalList;
}OopActionResponse_t;*/
typedef struct{
	OopActionRequest_t requst;
	OopActionRequest_t indication;
	OopActionConfirm_t response;
	OopActionConfirm_t confirm;
}OopAction_t;


typedef void (*OopReportRequestNormal_t)(void) ;
typedef void (*OopReportResponseNormal_t)(void) ;

typedef struct{
	OopReportRequestNormal_t normal;
	OopReportRequestNormal_t normalList;
	OopReportRequestNormal_t recordlList;
}OopReportRequest_t;

typedef struct{
	OopReportResponseNormal_t normal;
	OopReportResponseNormal_t normalList;
	OopReportResponseNormal_t recordlList;
}OopReportResponse_t;
typedef struct{
	OopReportRequest_t requst;
	OopReportRequest_t indication;
	OopReportResponse_t response;
	OopReportResponse_t confirm;
}OopReport_t;

typedef void (*OopProxyRequestNormal_t)(void) ;
typedef void (*OopProxyResponseNormal_t)(void) ;

typedef struct{
	OopProxyRequestNormal_t getList;
	OopProxyRequestNormal_t getRecord;
	OopProxyRequestNormal_t setList;
	OopProxyRequestNormal_t setThenGetList;
	OopProxyRequestNormal_t actionList;
	OopProxyRequestNormal_t actionThenGetList;
}OopProxyRequest_t;

typedef struct{
	OopProxyResponseNormal_t getList;
	OopProxyResponseNormal_t getRecord;
	OopProxyResponseNormal_t setList;
	OopProxyResponseNormal_t setThenGetList;
	OopProxyResponseNormal_t actionList;
	OopProxyResponseNormal_t actionThenGetList;
}OopProxyResponse_t;
typedef struct{
	OopProxyRequest_t requst;
	OopProxyRequest_t indication;
	OopProxyResponse_t response;
	OopProxyResponse_t confirm;
}OopProxy_t;
typedef struct{
	OopLink_t link;//链路服务
	OopConnect_t connect;//建立应用连接服务
	OopRelease_t release;//释放应用连接服务
	OopGet_t get;//get服务
	OopSet_t set;//set服务
	OopAction_t action;//action 服务
	OopReport_t report;//上报服务
	OopProxy_t proxy;//代理服务
}OopService_t;

extern void Client_Send_Apdu(uint8 pipe,LinkLayerAddr_t *a,uint8 ctrl,uint8 *apdu,uint16 apduLen);
extern void Server_Send_Apdu(uint8 ctrl,uint8 *apdu,uint16 apduLen);
extern eOopCommRslt_t Client_Resv_Apdu(LinkLayerAddr_t *a,LinkLayerCtrl_t ctrl,uint8 *apdu,uint16 apduLen);
extern uint16 LinkLayer_PackFrame(LinkLayerAddr_t *a,uint8*apdu,uint16 apduLen);
extern eOopCommRslt_t LinkDataReceive(uint8 pipe,const uint8 *Buffer, uint16 DataLength);
extern uint16 GetFrame(const uint8 *Buffer, uint16 DataLength, uint16 *OffSet);
extern eOopCommRslt_t LinkLayer_UnPackFrame(uint8 *inframe,uint16 inLen);
extern BOOLEAN Cmp_ServerAddr(LinkLayerAddr_t *sa,LinkLayerAddr_t*da);
extern LinkLayer_t* FindLinkLayer(uint8 pipe,uint8 *sa);
extern void DelLinkLayer(uint8 pipe,uint8 *sa);
extern void LinkLayerTimeOutDeal(uint8 pipe);

//get
extern void GetRequest(uint8 pipe,OOP_TSA_T *sa,uint8 nChoice,uint8 *inData,uint16 inLen);
extern eOopCommRslt_t GetConfirm(AppLayerList_t *appLayerList);
//set
extern void SetRequest(uint8 pipe,OOP_TSA_T *sa,uint8 nChoice,uint8 *inData,uint16 inLen);
extern eOopCommRslt_t SetConfirm(AppLayerList_t *appLayerList);

//action
extern void ActionRequest(uint8 pipe,OOP_TSA_T *sa,uint8 nChoice,uint8 *inData,uint16 inLen);
extern eOopCommRslt_t ActionConfirm(AppLayerList_t *appLayerList);

//connect
extern void ConnectRequest(uint8 pipe,OOP_TSA_T *sa,uint8 connectId,uint32 timeOut);
extern eOopCommRslt_t ConnectConfirm(AppLayerList_t *appLayerList);

//release
extern void ReleaseRequest(uint8 pipe,OOP_TSA_T *sa,uint8 connectId);
extern eOopCommRslt_t ReleaseConfirm(AppLayerList_t *appLayerList);
extern eOopCommRslt_t ReleaseNotification(AppLayerList_t *appLayerList);

//Security
extern void SecurityRequest(uint8 pipe,SecurityMode_t SecMod,uint8 *inData,uint16 inLen);
extern eOopCommRslt_t SecurityConfirm(AppLayerList_t *appLayerList);

//服务层对外接口
extern const OopGet_t OopGet;
extern const OopSet_t OopSet;
extern const OopAction_t OopAction;
extern const OopConnect_t OopConnect;
extern const OopRelease_t OopRelease;
extern AppLayerList_t* FindAppLayer(uint8 pipe,OOP_TSA_T *sa);
extern void DelAppLayer(uint8 pipe,OOP_TSA_T *sa);
extern void AppLayerTimeOutDeal(uint8 pipe);
extern eOopCommRslt_t AppLayerConfirm(uint8 pipe,uint8*inData,uint16 inLen);

//协议数据编码解码等接口
extern void OopDataEncode(enum eDatatype dataType,uint16 dataLen,uint8 *pSrcBuff,uint8 *pDesBuff,uint16 *pOffset);
extern BOOL getNextReadData(const uint8 *inData,uint16 *offset,uint16 inDataLen,enum eDatatype DataType,BOOL bWithType);
extern uint8 getDataTypeLength(enum eDatatype dataType);


//TESAM

#define cla_offset			1
#define ins_offset			2
#define p1_offset			3
#define p2_offset			4
#define len1_offset			5
#define len2_offset			6
#define data_offset			7

#define TURN_OFF_TESAM   		0X6501		//power off
#define TURN_ON_TESAM   		0X6502		//power on
#define GET_POWER_STATUS 		0X6503		//get status

#define SET_CS_LOW              0X6504          	//Set CS/SSN = 0    enable CS
#define SET_CS_HIGH             0X6505          	//Set CS/SSN = 1    disable CS

#define CS_DELAY				50		//50 us
#define CHECK_STATUS_MAX_COUNT 	100000		//500 times
#define CHECK_STATUS_DELAY		200000	//200 ms  CHECK_STATUS_MAX_COUNT * CHECK_STATUS_DELAY = 3s

#define START_FLAG_VALUE		0x55

#define	COMM_MET_TYPE			0x11			// 表地址为12个A的通用表类型
#define	REAL_MET_TYPE			0x22			// 表地址为实际地址的表类型

#define    METVALID				0x55			// 表有效标志	
#define    OPERATE_PREPARE		0x33			// 表操作准备标志
#define    OPERATE_SUCCESS		0x55			// 表操作成功标志	
#define    OPERATE_DOING		0x66			// 表正在操作标志	

#define	NO_ESAM					0x0fffffff		// 终端不存在ESAM

#define TMNRELAY 					1			//终端中级转发，不需要进行后续处理；
#define SUCCESS 					0			//程序处理中正常，需要进行后续处理；
#define PROCISOVER 					0			//程序处理结束，不需要进行后续处理；
#define ERROR 						-1			//程序处理中出现错误；
#define ERR_PW						-2			//-2  密码错误,
#define ERR_OVERTIME				-3			//-3  时间标签时间无效；
#define ERR_ADDR 					-4			//-4 地址域错误；


#define ESAM_OK					0x01		// ESAM返回正确的结果
#define ESAM_FAIL				0x00		// ESAM返回一般性失败的结果
#define ERR_SIGN				0x11		//签名校验错误,
#define ERR_CRYPTOGRAPH			0x22		//密文校验错误,

typedef struct{
	//uint8 rslt[2];							//命令结果
	uint8 esamSerial[8];					//ESAM序列号
	uint8 esamVersion[4];					//ESAM版本号
	//uint8 cosVersion[4];					//COS版本号
	uint8 symKeyVersion[16];					//对称密钥版本
	uint8 masterCertVer[1];					//主站证书版本
	uint8 tmnCertVer[1];					//终端证书版本
	uint8 authTimeThr[4];					//会话时效阈值
	uint8 authTimeCnt[4];					//会话时效剩余时间
	uint8 asctrCnt[4];						//应用协商计数器
	uint8 asctrReportCnt[4];				//主动上报计数器
	uint8 agseq[4];							//广播通信序列号
	uint8 tmnCertSeriNo[16];				//终端证书序列号
	uint8 masterCertSeriNo[16];				//主站证书序列号
}TESAMInfo_t;

typedef struct
{
	uint8 CLA;
	uint8 INS;
	uint8 P1;
	uint8 P2;
}SID_t;
/*
typedef enum{
	SecMod_NULL,							//无安全 明文
	SecMod_PlainMAC,						//明文+ MAC
	SecMod_Cipher,							//密文
	SecMod_CipherMAC,						//密文+ MAC
}SecurityMode_t;
*/
#define MAX_RAND_LEN			64
typedef struct{
	uint8 SecType;						//安全传输类型 0~4
	uint8 ClientRandLen;				//客户机下发的随机数长度
	uint8 ClientRand[MAX_RAND_LEN];		//客户机下发的随机数
	SID_t sid;
	uint8 attachData[2];				//附加数据 2字节
}SecurityData_t;


typedef struct{
	uint8 SecModEn;						//安全模式是否开启1:开启，其他未开启
	uint8 Rand[16];						//抄表随机数据
	uint8 EsamSerialNo[8];				//表ESAM序列号
}ReadMtrPara_t;

extern uint8 g_TESAMInfoBuff[2048];
extern uint16 g_TESAMInfoLen;

//extern int Open_TESAM(void) ;
extern int Init_Fd_Esam(void);
extern int8 NegotiateSession_Esam(uint8 *pInSessionInit,uint8 *pInSign, uint8 *pOutBuf, uint16 *OutbufLen);
extern int8 GetTEsam_Info(uint8 P2,uint8 *pOutBuf, uint16 *OutbufLen);
extern int8 TESAM_RcvSndDataAndCheck(uint8 *sndData, uint16 sndLen, uint8 *pOutBuf, uint16 *rcvLen, BOOL needReverse);
extern int8 GetDataMac(uint8 *pRand,uint8 *pPlainData,uint16 inDataLen,uint8 *pOutMAC);	
extern int8 GetRandAndDataMac(uint8 *pPlainData,uint16 inDataLen,uint8 *pOutMAC,uint8 *pOutRand);
extern int8 TEsamDataVerifi(SID_t sid,uint8 *pAttachData,uint8 *pInBuff,uint16 inDataLen,uint8 *pInMAC,uint8 *pOutBuff, uint16 *outDataLen);
extern int8 CheckDataMAC(SID_t sid,uint8 *pAttachData,uint8 *pInBuff,uint16 inDataLen,uint8 *pInMAC,uint8 *pOutBuff);
extern int8 ResponseFrameDeal(SecurityMode_t secMode,uint8 *pInBuff,uint16 inDataLen,uint8 *pOutBuff,uint16 *pOutDataLen);
extern int8 TESAMDataUpdate(SID_t sid,uint8 *pAttachData,uint8 attachLen,uint8 *pInBuff,uint16 inDataLen,uint8 *pInMAC);
extern int8 SetSecurityData(SecurityData_t SecData);

extern int8 Set_ReadMtrNum(uint8 pipe,uint8 *pInMtrNum);
extern int8 Set_MtrSecModEn(uint8 pipe,uint8 secModEn);
extern int8 Init_ReadMtr(uint8 pipe,uint8 *pInEsamSerialNo);
extern int8 Verify_ReadMtrMAC(uint8 pipe,SecurityMode_t secMod,uint8 *pPlainData,uint16 inDataLen,uint8 *pInMAC,uint16 *pOutDataLen);
extern int8 Get_ReadMtrRand(uint8 pipe,uint8 *pOutRand);
extern SecModWd_t GetDefaultSecMod(uint16 OI);
extern SecModWd_t GetMtrDefaultSecMod(uint16 OI);
extern SecModWd_t GeSecMod(uint16 OI);
extern SecModWd_t GetMtrSecMod(uint8 pipe,uint16 OI);
extern BOOL SaveTESAMInfo(void);
extern int WriteUpdateFile(const char *filename, uint8 *buf, unsigned offset, unsigned len);
extern int UpdateShowGui();
extern int WriteUpdateTar(char *buf, int offset, int len);

#endif
