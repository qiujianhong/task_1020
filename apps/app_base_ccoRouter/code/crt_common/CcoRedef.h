/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief： 功能描述
* @date： 2019-10-25
*********************************************************************
*/


#ifndef __CCO_REDEF_H__
#define __CCO_REDEF_H__

#include "CcoAll.h"

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_MSC_VER)
#pragma warning(disable:4201)
#endif

/*******************************************************************************
* 任务调度APP线程数量配置 *********************------<测试修改>------**********
*******************************************************************************/

#define MAX_ALLOWED_CTS_THREAD_NUM    2               //任务调度CTS下行最大允许线程数量
#define MAX_CTS_THREAD_NUM    2               //任务调度CTS下行实际运行线程数量

/*******************************************************************************
* 本地抄表模块线程数量配置 *********************------<测试修改>------**********
*******************************************************************************/

#define MAX_ALLOWED_DOWN_THREAD_NUM     2               //CCO下行最大允许线程数量
#define MAX_DOWN_THREAD_NUM             1               //CCO下行实际运行线程数量




/*******************************************************************************
* 通信参数配置
*******************************************************************************/
#define MAX_QUEUE_CNT          20              //最大队列数

#define MAX_HID_SEND_LEN        65              //载波HID最大发送长度
#define MAX_HID_RECV_LEN        64              //载波HID最大接收长度

#define MAX_COMM_DELAY_TIME     6               //最大接收延时等待时间
#define MAX_NORMAL_AMR_INFO_CNT     10            //并行最大抄表个数

#define MAX_INFO_DATA_NUM      500             //消息MSG缓存列表

#define MAX_MODULE_NUM     5       //模组的最大数量
#define MAX_CHANNEL_NUM     5      //每个模组最多有5个通道 0默认管理通道
#define MAX_MODULE_DELAY_TIME  6   //查询模组消息最大超时时间
#define CCO_MODULE_DEVNAME  "uart_acm"

/*
 ********************************************************************
 *                                                                  *
 * CCO上行消息数据类型定义                                          *
 *                                                                  *
 ********************************************************************
*/

#define MAX_QUEUE_BUFF_LEN                 5120                      //上行、下行缓冲区长度
#define MAX_MSG_PAYLOAD_LEN                2040                      //消息MSG的payload最大长度
#define MAX_DEAL_BUFF_LEN                  1500                      //数据透传、从节点监控的内含原始报文长度
#define MAX_UINT8_LEN                      255                       //uint8对应最大长度

#define MAX_SEARCH_METER_FILES_NUM         1000                      //上报搜表数据表计档案最大数
#define MAX_NEIGHBOR_NODE_NET_NUM          1000                      //邻居节点网络标识号最大个数
#define MAX_NET_LEVEL_NUM                  1000                      //网络层级最大个数
#define MAX_NEXT_NODE_NUM                  1000                      //下接节点最大个数
#define MAX_AMR_COLLECT_CNT                2040                      //最大抄表汇总个数
#define MAX_FILES_SUMMARY_NUM              2040                      //档案数量最大个数

#define MAX_ACTIVE_AMR_INFO_NUM            1000                      //主动抄表报文最大个数
#define MAX_DATA_TRANSMIT_INFO_NUM         1000                      //数据透传报文最大个数
#define MAX_SLAVE_MONITOR_INFO_NUM         1000                      //从节点监控报文最大个数

/*******************************************************************************
* 每次读取 删除 添加 路由的数目
*******************************************************************************/
#define MODULEDEAL_CNT                            10

/*******************************************************************************
* 搜表的设备标识
*******************************************************************************/
#define DEV_COLLECTOR          0x00           //采集器
#define DEV_METER              0x01           //电表

/*******************************************************************************
* 端口号配置
*******************************************************************************/
#define PORT_F2090201          0xf2090201           //第1个本地模块端口号
#define PORT_F2090202          0xf2090202           //第2个本地模块端口号

#define CCO_PRODUCT_MODULE          1       /* 有模组管理器 */
#define CCO_PRODUCT_NO_MODULE       2       /* 没有模组管理器 */


/* 考虑字节对齐, 没有严格按照报文格式, 收到的消息解析成如下格式 */
typedef struct tagInfoData
{
    uint32 IP;        //目的端ip
    uint16 port;
    uint8  res[2];
    
    uint8  MsgRPM;         /* 启动标志位, 1-启动, 0-非启动 */
    uint8  MsgPRIORITY;    /* 优先级 */
    uint16 MsgIndex;       /* 消息序号 */
    uint16 MsgLabel;       /* 消息标签 */

    uint8  SourLen;
    uint8  DestLen;
    char   SourAddr[128];
    char   DestAddr[128];

    uint16 IID_ID;
    uint16 IOP_ID;

    uint32 nPayloadLength; 
    uint8  aPayloadData[MAX_MSG_PAYLOAD_LEN];
}INFO_DATA_T;


//消息HEAD
typedef struct tagInfoDataHead
{
    uint32 IP;        //目的端ip
    uint16 port;
    uint8  res[2];
    
    uint8  MsgRPM;         /* 启动标志位, 1-启动, 0-非启动 */
    uint8  MsgPRIORITY;    /* 优先级 */
    uint16 MsgIndex;       /* 消息序号 */
    uint16 MsgLabel;       /* 消息标签 */

    uint8  SourLen;
    uint8  DestLen;
    char   SourAddr[128];
    char   DestAddr[128];
}INFO_DATA_HEAD_T;


//消息列表缓存区
typedef  struct
{
    int16 InfoDataNum;  //消息列表缓存区的数量
    int16 InfoDataReadIndex;  //获取索引（数组下标，从0开始）
    INFO_DATA_T InfoDataList[MAX_INFO_DATA_NUM];  //消息MSG列表缓存区
} INFO_DATA_LIST_T;  //消息列表缓存区

//抄表端口修改类型
typedef enum
{
    AMR_PORT_MODIFY_DELETE = 0, //删除
    AMR_PORT_MODIFY_ADD,  //添加
}AMR_PORT_MODIFY_E;  

//抄表端口配置
typedef struct
{
    uint32   nPortID;  //端口号
    AMR_PORT_MODIFY_E emPortModify;  //抄表端口修改类型
}AMR_PORT_CFG_T;   


//档案比对时的具体参数
typedef enum
{
    CMP_FILES_PORT_ID = 0, //端口号
    //CMP_FILES_ID,  //档案序号
    CMP_FILES_ADDR,  //档案地址
    CMP_FILES_MAC,  //档案MAC
    CMP_FILES_PROTOCAL,  //协议类型
    //CMP_FILES_COMM,  //通信参数
    CMP_FILES_ADDR_PROTOCAL,  //同时比对地址和规约
}AMR_FILES_CMP_PARA_E;  

//电能表档案
typedef struct
{
    //uint16   nMeterFilesID; //档案序号
    uint8   aMeterFilesAddr[6];  //档案地址
    uint8   aMeterFilesMac[6];  //档案MAC
    uint8   nProtocolType;  //协议类型
    //uint8   nCommPara;  //通信参数
}AMR_METER_FILES_T;  

//档案信息
typedef struct
{
    uint32   nPortID;   //端口号
    AMR_METER_FILES_T stAmrMeterFiles;  //电能表档案
}AMR_METER_FILES_INFO_T;  

//档案信息汇总表
typedef struct
{
    uint16   nMeterFilesNum;  //档案数量
    AMR_METER_FILES_INFO_T stMeterFilesData[MAX_FILES_SUMMARY_NUM]; //电能表档案
}AMR_METER_FILES_LIST_T; 

typedef struct tagCCO_COM_DCB_T
{
    uint8   baud; /* 波特率 */
    uint8   ctrl; /* 控制位 */
}CCO_COM_DCB_T;

//请求抄表（被动抄表）
typedef struct
{
    uint32   nPortID;  //端口号
    uint16   nMeterFilesID; //档案序号
    CCO_COM_DCB_T comDcb;  //通信控制字
    uint8   aPlcCommModelSymbol[6];  //载波通信模式字
    uint8   nMeterAddr[6];  //表计地址
}AMR_METER_REQUEST_READ_T;  

//抄读信息
typedef struct
{
    uint8   nRelayRouterLevel;  //中继路由级数
    uint8   nChannelIdentifier;  //信道标识
    uint8   nCarrierReadPhase;  //载波抄读相位
    uint8   nCarrierSignalQuality;  //载波信号品质
    uint8   nCarrierSymbol;  //载波标志
    uint8   aReserveForOtherUse[3];  //保留  
}AMR_PLC_READING_INFO_T;  

//上报抄表数据
typedef struct
{
    uint32   nPortID;  //端口号
    uint16   nMeterFilesID; //档案序号
    uint8   nProtocolType;  //协议类型
    uint16  nDataBufLen;    //数据帧长度
    uint8   pDataBuf[MAX_DEAL_BUFF_LEN];    //数据帧
    uint8   nReadingInfoFlag;  //是否含有抄读信息标志, 0x00:无, 0x01:有
    AMR_PLC_READING_INFO_T stPlcReadingInfo;  //抄读信息
}AMR_PLC_INFO_REPORT_T;  

//上报事件数据
typedef struct
{
    uint32   nPortID;  //端口号
    uint16   nMeterFilesID; //档案序号
    uint8   nProtocolType;  //协议类型
    uint16  nDataBufLen;    //数据帧长度
    uint8   pDataBuf[MAX_DEAL_BUFF_LEN];    //数据帧
    uint8   nReadingInfoFlag;  //是否含有抄读信息标志, 0x00:无, 0x01:有
    AMR_PLC_READING_INFO_T stPlcReadingInfo;  //抄读信息
}AMR_EVENT_DATA_REPORT_T;  


//数据透传
typedef struct
{
    uint32   nPortID;  //端口号
    uint8   nCommSymbol;  //通讯字
    uint16   nFrameTimeover; //帧超时时间
    uint16   nCharTimeover; //字符超时时间
    uint16  nTransmitDataLen;   //数据帧长度
    uint8   pTransmitDataBuf[MAX_DEAL_BUFF_LEN];    //发送数据内容
}AMR_DATA_TRANSMIT_T;  

//数据透传信息的集合
typedef struct
{
    pthread_mutex_t DataTransmitLock;                   //数据透传信息缓冲区互斥锁
    uint16   nDataTransmitNum;  //数据透传信息条数
    int16    DataTransmitIndex;  //获取索引（数组下标，从0开始）
    AMR_DATA_TRANSMIT_T aDataTransmitList[MAX_DATA_TRANSMIT_INFO_NUM];  //数据透传信息info集合
    INFO_DATA_HEAD_T    aInfoDataHead[MAX_DATA_TRANSMIT_INFO_NUM];  //数据透传信息info原始HEAD
}AMR_DATA_TRANSMIT_LIST_T; 


//启动搜表
typedef struct
{
    uint32   nPortID;  //端口号
    int32    nStartTime;  //开始时间
    int32    nContinuedTime;  //持续时间
    uint8   nSlaveNodeReSendCount;  //从节点重发次数
    uint8   nRandWaitTimeSlice;  //随机等待时间片个数
}AMR_METER_START_SEARCH_T;  

//是否启动搜表
typedef struct
{
    BOOL bStartSearch;  //是否启动搜索
    AMR_METER_START_SEARCH_T stAmrMeterStartSearch;  //搜表信息
}WHETHER_START_SEARCH_METER_T;  


//上报搜表数据
typedef struct
{
    uint32   nPortID;  //端口号
    uint8   nReadingInfoFlag;  //是否含有抄读信息标志, 0x00:无, 0x01:有
    AMR_PLC_READING_INFO_T stPlcReadingInfo;  //载波传输附加信息
    uint16  nAmrMeterFilesNum;  //表计档案数量
    AMR_METER_FILES_T stAmrMeterFiles[MAX_SEARCH_METER_FILES_NUM];  //表计档案
}AMR_METER_SEARCH_REPORT_T; 

//获取从节点监控延时
typedef struct
{
    uint32   nPortID;  //端口号
    uint8    aSlaveNodeAddr[6]; //载波从节点地址
    uint8    nProtocolType; //规约类型
    uint16   nDataBufLen;  //报文长度
    uint8    aDataBuf[MAX_DEAL_BUFF_LEN];   //报文内容
}AMR_SLAVE_NODE_DELAY_TIME_T; 

//获取从节点监控延时的集合
typedef struct
{
    pthread_mutex_t SlaveMonitorTimeLock;  //从节点监控延时信息缓冲区互斥锁
    uint16   nSlaveMonitorTimeNum;  //从节点监控延时信息条数
    int16    SlaveMonitorTimeIndex;  //获取索引（数组下标，从0开始）
    AMR_SLAVE_NODE_DELAY_TIME_T aSlaveMonitorTimeList[MAX_SLAVE_MONITOR_INFO_NUM];  //从节点监控延时信息info集合
    INFO_DATA_HEAD_T    aInfoDataHead[MAX_SLAVE_MONITOR_INFO_NUM];  //从节点监控延时信息info原始HEAD
}AMR_SLAVE_MONITOR_TIME_LIST_T; 

//从节点监控
typedef struct
{
    uint32   nPortID;  //端口号
    uint8   nProtocolType;  //规约类型
    uint8   aSlaveNodeAddr[6];  //载波从节点地址
    CCO_COM_DCB_T comDcb;  //通信控制字
    uint16   nFrameTimeover; //帧超时时间
    uint16   nByteTimeover; //字节超时时间
    uint16   nDataBufLen;  //报文长度
    uint8    aDataBuf[MAX_DEAL_BUFF_LEN];   //报文内容
}AMR_SLAVE_NODE_MONITOR_T; 

//从节点监控信息的集合
typedef struct
{
    pthread_mutex_t SlaveMonitorLock;                   //从节点监控信息缓冲区互斥锁
    uint16   nSlaveMonitorNum;  //从节点监控信息条数
    int16    SlaveMonitorIndex;  //获取索引（数组下标，从0开始）
    AMR_SLAVE_NODE_MONITOR_T aSlaveMonitorList[MAX_SLAVE_MONITOR_INFO_NUM];  //从节点监控信息info集合
    INFO_DATA_HEAD_T    aInfoDataHead[MAX_SLAVE_MONITOR_INFO_NUM];  //从节点监控信息info原始HEAD
}AMR_SLAVE_MONITOR_LIST_T; 


//获取广播延时
typedef struct
{
    uint32   nPortID;  //端口号
    uint8   nProtocolType;  //规约类型
    uint16  nDataBufLen;    //数据帧长度
    uint8   aDataBuf[MAX_UINT8_LEN];    //数据帧
}AMR_GET_BROADCAST_DELAY_T; 

//是否获取广播延时
typedef struct
{
    BOOL bGetBroadcastDelay;  //是否获取广播延时
    INFO_DATA_HEAD_T InfoDataHead;  //消息原始HEAD
    AMR_GET_BROADCAST_DELAY_T stAmrGetBroadcastDelay;  //获取广播延时
}WHETHER_GET_BROADCAST_DELAY_T; 


//广播
typedef struct
{
    uint32   nPortID;  //端口号
    uint8   nProtocolType;  //规约类型
    CCO_COM_DCB_T comDcb;  //通信控制字
    uint16  nDataBufLen;    //数据帧长度
    uint8   aDataBuf[MAX_DEAL_BUFF_LEN];    //数据帧
}AMR_DO_BROADCAST_T; 

//是否广播
typedef struct
{
    BOOL bDoBroadcast;  //是否获取广播延时
    AMR_DO_BROADCAST_T stAmrDoBroadcast;  //广播
}WHETHER_DO_BROADCAST_T;


//当前抄表模式
typedef enum
{
    AMR_MODE_ACTIVE = 0, //主动抄表
    AMR_MODE_PASSIVE,  //被动抄表
}AMR_CURRENT_MODE_E;  

//主动抄表
typedef struct
{
    uint32   nPortID;  //端口号
    //uint16   nMeterFilesID; //档案序号
    CCO_COM_DCB_T comDcb;  //通信控制字
    uint8    nMeterAddr[6]; //表计地址
    uint16   nDataBufLen;  //报文长度
    uint8    aDataBuf[MAX_DEAL_BUFF_LEN];   //报文内容
}AMR_MODE_ACTIVE_T; 

//主动抄表任务的集合
typedef struct
{
    pthread_mutex_t ActiveAmrInfoLock;                  //抄表信息缓冲区互斥锁
    uint16   nActiveAmrInfoNum;  //抄表报文条数
    int16    ActiveAmrInfoIndex;  //获取索引（数组下标，从0开始）
    AMR_MODE_ACTIVE_T aActiveAmrInfoList[MAX_ACTIVE_AMR_INFO_NUM];  //主动抄表info集合
}AMR_ACTIVE_INFO_LIST_T; 


//查询档案
typedef struct
{
    uint32   nPortID;  //端口号
    uint16   nFilesStartID; //档案序号起始
    uint16   nFilesCount; //查询档案数
}AMR_FILES_QUERY_T; 

//查询HPLC信息
typedef struct
{
    uint32   nPortID;  //端口号
    uint8   nInfoType;  //信息类型
}AMR_HPLC_INFO_QUERY_T; 

//节点网络拓扑信息
typedef struct
{
    uint8   nNodeAddr[6];  //节点地址
    uint8   nNodeTopologyInfo[5];  //节点拓扑信息
}AMR_HPLC_TOPOGOGY_INFO_T; 

//宽带载波芯片信息
typedef struct
{
    uint16   nNodeID; //节点序号
    uint8   nNodeAddr[6];  //节点地址
    uint8   nNodeEquipType;  //节点设备类型
    uint8   nNodeChipInfo[24];  //节点芯片ID信息
    uint8   nNodeChipSoftVer[2];  //节点芯片软件版本
}AMR_HPLC_CHIP_INFO_T; 

//多网络信息
typedef struct
{
    uint8   nNeighborNodeNum;  //邻居节点个数
    uint8   nNodeNetSymbol[3];  //本节点网络标识号
    uint8   nNodeMasterNodeAddr[6];  //本节点主节点地址
    uint8   nNeighborNodeNetSymbol[MAX_NEIGHBOR_NODE_NET_NUM][3];  //邻居节点网络标识号
}AMR_HPLC_NETS_INFO_T; 

//节点相线信息
typedef struct
{
    uint16   nNodeID; //节点序号
    uint8   nNodeAddr[6];  //节点地址
    uint8   nNodePhaseInfo[2];  //节点相位信息
}AMR_HPLC_PHASE_INFO_T; 

//主节点模块ID信息
typedef struct
{
    uint8   nModuleFacSymbol[2];  //模块厂商代码
    uint8   nModuleIdLength;  //模块ID长度
    uint8   nModuleIdFormat;  //模块ID格式
    uint8   nModuleID;  //模块ID
}AMR_HPLC_MASTER_ID_INFO_T; 

//从节点模块ID信息
typedef struct
{
    uint16   nNodeID; //从节点序号
    uint8   nNodeAddr[6];  //从节点地址
    uint8   nNodeType;  //从节点类型
    uint8   nModuleFacSymbol[2];  //从节点模块厂商代码
    uint8   nModuleIdLength;  //从节点模块ID长度
    uint8   nModuleIdFormat;  //从节点模块ID格式
    uint8   nModuleID;  //从节点模块ID
}AMR_HPLC_SLAVE_ID_INFO_T; 

//层级信息
typedef struct
{
    uint16   nBelongLevel; //所在层级
    uint8   nNodeNumOfLevel[2];  //此层级节点个数
}AMR_HPLC_LEVEL_INFO_T; 

//网络基本信息
typedef struct
{
    uint8   nNetNodeCount[2];  //网络节点总数
    uint8   nOnlineNodeCount[2];  //在线站点个数
    uint8   nNetStartNetPeriod[2];  //网络启动组网时长
    uint8   nNetWorkLinkPeriod[2];  //组网时长
    uint8   nBeaconCycle;  //信标周期
    uint8   nRouterCycle[2];  //路由周期
    uint8   nTopoChangeTimes[2];  //拓扑变更次数
    AMR_HPLC_LEVEL_INFO_T  stLevelInfo[MAX_NET_LEVEL_NUM];  //层级信息
}AMR_HPLC_BASE_ID_INFO_T; 

//下接节点信息
typedef struct
{
    uint8   nNodeAddr[6];  //下接从节点通信地址
    uint8   nCommProtocolType;  //下接从节点通信协议类型
    uint8   nNodeEquipType;  //下接从节点设备类型
}AMR_HPLC_NEXT_NODE_INFO_T; 

//网络节点信息
typedef struct
{
    uint16   nNodeID; //序号
    uint8   nNodeAddr[6];  //节点地址
    uint8   nNodeTopologyInfo[5];  //节点拓扑信息
    uint8   nNetInfo;  //网络信息
    uint8   nNodeEquipType;  //设备类型
    uint8   nPhaseInfo;  //相位信息
    uint8   nAgentChangeTimes[2];  //代理变更次数
    uint8   nStaOfflineTimes[2];  //站点离线次数
    uint8   nStaOfflinePeriod[4];  //站点离线时长
    uint8   nStaOfflinePeriodMax[4];  //站点离线最大时长
    uint8   nUpCommSuccessRate[4];  //上行通信成功率
    uint8   nDownCommSuccessRate[4];  //下行通信成功率
    uint8   nMainVerID[3];  //主版本号
    uint8   nSecondaryVerID[2];  //次版本号
    uint8   nNextTurnInfo[2];  //下一跳信息
    uint8   nChannelType[2];  //信道类型
    uint8   nProtocolType;  //规约类型
    uint8   nTransformAreaState;  //台区状态
    uint8   nTransformAreaAddr[6];  //台区号地址
    AMR_HPLC_NEXT_NODE_INFO_T  stNextNodeInfo[MAX_NEXT_NODE_NUM];
}AMR_HPLC_NET_NODE_INFO_T; 

//上报HPLC查询信息类型
typedef enum
{
    AMR_HPLC_TOPOLOGY_INFO = 1,  //网络拓扑信息   [1] SEQUENCE OF AMR_HPLC_TOPOLOGYINFO，
    AMR_HPLC_CHIP_INFO,  //宽带载波芯片信息 [2] SEQUENCE OF AMR_HPLC_CHIPINFO，
    AMR_HPLC_NETS_INFO,  //多网络信息          [3] AMR_HPLC_NETSINFO，
    AMR_HPLC_PHASE_INFO,  //节点相线信息      [4] SEQUENCE OF AMR_HPLC_PHASEINFO，
    AMR_HPLC_NET_SCALE,  //网络规模             [5] long-unsigned，
    AMR_HPLC_MASTER_ID,  //主节点模块ID信息  [6] SEQUENCE OF AMR_HPLC_MASTERID，
    AMR_HPLC_SLAVE_ID,  //从节点ID信息     [7] SEQUENCE OF AMR_HPLC_SLAVEID，
    AMR_HPLC_BASE_INFO,  //网络基本信息   [8] AMR_HPLC_BASEINFO，
    AMR_HPLC_NET_NODE_INFO,  //网络节点信息   [9] SEQUENCE OF AMR_HPLC_NETNODEINFO
}AMR_HPLC_QUERY_INFO_TYPE_E;  


//上报HPLC查询信息
typedef struct
{
    uint32   nPortID;  //端口号
    uint16   nStartID; //起始序号
    AMR_HPLC_QUERY_INFO_TYPE_E  emQueryInfoType;  //上报HPLC查询信息类型
    uint8   *pDataBuf;  //数据
}AMR_HPLC_QUERY_INFO_REPORT_T; 

//HPLC频点设置
typedef struct
{
    uint32   nPortID;  //端口号
    uint8   nFrequencyPot;  //频点
}AMR_HPLC_FREQUENCY_POT_SET_T; 

//HPLC台区识别
typedef struct
{
    uint32   nPortID;  //端口号
    uint8   nSwitch;  //开关
}AMR_HPLC_TRANSFORM_AREA_TELL_T; 

//HPLC离网感知
typedef struct
{
    uint32   nPortID;  //端口号
    uint8   nSwitch;  //开关
}AMR_HPLC_OFFLINE_SENSE_T; 

//查询抄表状态
typedef struct
{
    uint32   nLastRecvTime;  //最近一次接收时间
    int32    nModuleAmrState; //模块抄表状态 1：抄表中；0：未抄表
    int32    nModuleSearchState; //模块搜表状态 1：搜表中；0：未搜表
    int32    nAutoAmrState; //自动抄表状态
    uint16   nClearRouterTimes; //清除路由次数
    uint16   nDataInitTimes; //数据初始化次数
    uint16   nParaInitTimes; //参数初始化次数
    uint16   nResetModuleTimes; //复位模块次数
}AMR_QUERY_READING_STATE_T; 

//查询抄表模块信息
typedef struct
{
    uint8   aLocalCommTypeModel[6];  //本地通信模式字
    uint16   nSlaveMonitorMaxOvertime; //从节点监控最大超时
    uint16   nBroadcastMaxOvertime; //广播最大超时时间
    uint16   nMaxAllowedFrameLen; //最大支持报文长度
    uint16   nMaxUpgradeSingleFrameLen; //升级最大单个包长度
    uint8   nUpgradeWaitingPeriod;  //升级操作等待时间
    uint16   nBroadcastDelayPeriod;  //广播成功延时时间
    uint8   aModuleID[6];  //模块编号
    uint16   nMaxSupportSlaveNum; //支持的最大从节点数量
    uint16   nCurrentSlaveNum; //当前从节点数量
    uint8    InfoBuff[9];        //原始报文(兼容性)
    uint8   ManufacturerCode[2]; //厂商代码
    uint8   ChipCode[2];         //芯片代码
    uint8   EditionDate[3];      //版本日期日
    uint16  nModuleVer;         //版本号
}AMR_QUERY_MODULE_INFO_T; 

//模组信息中的通道信息
typedef struct
{
    uint8 ChannelType;//接口类型    enum{CDC-ACM(1), CDC-ECM(2)},
    uint8 ChannelFunc;//功能配置    enum{管理通道(0), 远程通信(1)，PLC/RF通信(2)，RS-485通信(3)，遥信脉冲(4)，CAN通信(5)，M-Bus通信(6)，控制(7)，模拟量采集(8)，RS232通信(9)}
    uint8 DriveName[32];
}CHANNEL_INFO_T;  //模组信息中的通道信息

//模组信息
typedef struct
{
    pthread_rwlock_t   ModuleLock;  //读写模组信息所用读写锁
    uint8 IsValid;//是否有效 0无效 1有效
    uint8 DeviceType[32];//设备型号
    uint8 DeviceID[32];//设备id
    uint8 SoftVersion[32];//软件版本
    uint8 SoftDate[10];//软件发布日期
    uint8 HardVersion[32];//硬件版本
    uint8 HardDate[10];//硬件发布日期
    uint8 ManufacturerCode[8];//厂商编码
    uint8 ChannelNum;//通道个数
    CHANNEL_INFO_T ChannelInfo[MAX_CHANNEL_NUM];//通道配置
    BOOL bCCO;//是否本地通信模块
}MODULE_INFO_T;//模组信息

//本地模块HAL接口参数
typedef struct
{
    char  deviceName[30];  /*设备名 */
    char  ctrlName[30]; /* 控制通道名称 */
    uint8 isvalid;
    uint8 moduleIndex;  /*模块编号 从1开始 */
    UART_DEVICE_T* dev;
    LOCAL_CU_DEVICE_T *ctrlDev; /* 控制通道句柄 */
}DEV_HAL_INFO;


/*******************************************************************************
* 枚举和结构体定义
*******************************************************************************/



//队列
typedef struct
{
    int32  QueueLen;                            //队列长度
    uint8   QueueBuf[MAX_QUEUE_BUFF_LEN];       //队列存储缓冲区
}Queue_T;

//锁共享缓冲区结构
typedef struct
{
    pthread_mutex_t QueueLock;                  //接收缓冲区互斥锁
    int16  QueueHead;                           //队列头
    int16  QueueTail;                           //队列尾
    Queue_T Queue[MAX_QUEUE_CNT];               //通讯数据队列
}LockQueue_T;

//线程参数结构
typedef struct
{
    uint8        nArrIndex;                     //数组下标
    LockQueue_T *RecvBuf;                       //接收缓冲区结构指针
    LockQueue_T *SendBuf;                       //发送缓冲区结构指针
}ThreadArg_T;

//自动抄表线程参数结构
typedef struct
{
    uint8        nArrIndex;                     //数组下标
    ThreadArg_T *UpIpcArg;                      //上行IPC通信进程锁结构指针
    ThreadArg_T *DownCommArg;                   //下行通信进程锁结构指针
}AmrThreadArg_T;

typedef struct
{
    uint8   DeviceName[30];                     //设备名，如"/dev/ttyS0"
    int32  BaudRate;                            //波特率
    int8   DataBits;                            //数据位, 4-8
    int8   ParityFlag;                          //校验 'n'  'e'  'o'  's' = None,Odd,Even,Space
    int8   StopBits;                            //停止位 0,1,2 = 1, 1.5, 2
}COMM_PARA_T;

typedef  struct
{
    uint8   AFN;                                //应用功能码
    uint16  Fn;                                 //信息类标识
    uint8   ModuleID;                           //通信模块标识(0:集中器 / 1:载波表)
    uint8   DesAddr[6];                         //目的地址
    uint8   RInfo[6];                           //信息域
    uint16  DataUnitLen;                        //应用数据长度
    uint8   DataUnitBuff[MAX_QUEUE_BUFF_LEN];   //应用数据缓冲区
} AppData_T; 

typedef struct
{
    uint8   Second;                             //BIN or BCD
    uint8   Minute;                             //BIN or BCD
    uint8   Hour;                               //BIN or BCD
    uint8   Day;                                //BIN or BCD
    uint8   Month;                              //BIN or BCD
    uint8   Year;                               //BIN or BCD
}DateTime_T;                                    //日期时间结构

typedef struct
{
    uint8   Second              :1;             //过秒标志
    uint8   Minute              :1;             //过分标志
    uint8   Hour                :1;             //过时标志
    uint8   Day                 :1;             //过日标志
    uint8   Month               :1;             //过月标志
    uint8   ReserveBit6         :1;             //保留
    uint8   ReserveBit7         :1;             //保留
    uint8   ReserveBit8         :1;             //保留
}PastTimeFlagBit_T;

typedef union
{
    PastTimeFlagBit_T   BitSect;
    uint8               Word;
}PastTimeFlag_T;       

typedef struct
{
    uint8  COMM_TYPE        :6; //通信方式
    uint8  PRM              :1; //启动标志位
    uint8  DIR              :1; //控制码
}CTRLCODE_BITS_T;//遥信位

typedef struct
{
    uint8   CommPattern        :4;  //通信方式(1:窄带,2:宽带,3:小无线)
    uint8   RouterMangerPattern    :1;  //路由管理方式(0:无路由管理,1:有路由管理)
    uint8   SlaverInfoMode               :1;    //从节点信息模式(0:不需下发,1:需要下发)
    uint8   AmrCycleMode                 :2;    //周期抄表模式(01:主动,10:被动,11:都支持,00:保留)
} LOCAL_COMM_PATTERN_WORD_1;

typedef union
{
    LOCAL_COMM_PATTERN_WORD_1    BitSect;
    uint8                        Word;
} LOCAL_COMM_PATTERN_WORD_B1;

typedef struct
{
    uint8   TransDelayParamSupport      :3; //传输延时参数支持
    uint8   FailureNodeSwitchLaunchPattern  :2; //失败节点切换发起方式
    uint8   BroadcastAffirmPattern                     :1; //广播命令确认方式(0,1)
    uint8   BroadcastChannelExecPattern        :2; //广播命令信道执行方式(0:执行广播命令无需逐个播发,1:需要)
} LOCAL_COMM_PATTERN_WORD_2;

typedef union
{
    LOCAL_COMM_PATTERN_WORD_2    BitSect;
    uint8            Word;
} LOCAL_COMM_PATTERN_WORD_B2;

typedef struct
{
    uint8   ChannelNum              :5;             //信道数量 <corrigendum-2013-09-18>
    uint8   LowVoltageGridPowerDownInfo :3;         //低压电网掉电信息 <corrigendum-2013-09-18>
} LOCAL_COMM_PATTERN_WORD_3;

typedef union
{
    LOCAL_COMM_PATTERN_WORD_3    BitSect;
    uint8            Word;
} LOCAL_COMM_PATTERN_WORD_B3;

typedef struct
{
    uint8   SpeedNum                        :4;             //速率数量n-l <corrigendum-2013-09-18>
    uint8   Reserved                        :4;             //保留字 h <corrigendum-2013-09-18>
} LOCAL_COMM_PATTERN_WORD_4;

typedef union
{
    LOCAL_COMM_PATTERN_WORD_4    BitSect;
    uint8            Word;
} LOCAL_COMM_PATTERN_WORD_B4;

typedef struct
{
    uint8   Reserved            :8;             //保留字
} LOCAL_COMM_PATTERN_WORD_5;

typedef union
{
    LOCAL_COMM_PATTERN_WORD_5    BitSect;
    uint8            Word;
} LOCAL_COMM_PATTERN_WORD_B5;

typedef struct
{
    uint8   Reserved            :8;             //保留字
} LOCAL_COMM_PATTERN_WORD_6;

typedef union
{
    LOCAL_COMM_PATTERN_WORD_6    BitSect;
    uint8            Word;
} LOCAL_COMM_PATTERN_WORD_B6;


typedef struct
{
    LOCAL_COMM_PATTERN_WORD_B1 b1;
    LOCAL_COMM_PATTERN_WORD_B2 b2;
    LOCAL_COMM_PATTERN_WORD_B3 b3;
    LOCAL_COMM_PATTERN_WORD_B4 b4;
    LOCAL_COMM_PATTERN_WORD_B5 b5;
    LOCAL_COMM_PATTERN_WORD_B6 b6;
} LOCAL_COMM_PATTERN_WORD;

typedef struct
{
    uint8   ManufacturerCode[2]; //厂商代码
    uint8   ChipCode[2];         //芯片代码
    uint8   EditionDate[3];      //版本日期日
    uint16  EditionInfo;         //版本号
    uint8    InfoBuff[9];        //原始报文(兼容性)
} MANUFACTURE_INFO;

typedef struct
{
    uint16   CommSpeed                      :15;            //通信速率
    uint16   SpeedUintIdentifying           :1;         //速率单位标识
} COMM_SPEED_UNIT_INFO_T;

typedef union
{
    COMM_SPEED_UNIT_INFO_T       BitSect;
    uint16               Word;
} COMM_SPEED_UNIT_INFO;


typedef struct
{
    LOCAL_COMM_PATTERN_WORD LocalCommPatternWord;   //本地通信模式字
    uint8  SlaverMonitorMaxOverTime;                //从节点监控最大超时时间(单位:s)<corrigendum-2013-09-18>
    uint16 BroadcastMaxOverTime;                    //广播命令最大超时时间(单位:s)
    uint16 MaxFrameLen;                             //最大支持的报文长度
    uint16 FileTransferMaxSolePackageLen;           //文件传输支持的最大单个数据包长度
    uint8  UpdateOperatorWaittingTime;              //升级操作等待时间
    uint8  MasterAddr[6];                           //主节点地址
    uint16 MaxSlaverNum;                            //支持的最大从节点数量
    uint16 CurrentSlaverNum;                        //当前从节点数量
    uint8  ModuleProtocolReleaseDate[3];            //通信模块使用的协议发布日期(BCD)
    uint8  ModuleProtocolLastRecordDate[3];         //通信模块使用的协议最后备案日期(BCD)
    MANUFACTURE_INFO  ManufacturerInfo;             //通信模块厂商代码及版本信息
    COMM_SPEED_UNIT_INFO CommSpeedUnitInfo[256];    //速率单位标识和通信速率
} LOCAL_COMM_MODULE_RUNNING_PATTERN_INFO;

typedef enum
{
    MS_IDLE = 0,                                //空闲
    MS_DELAY_DEAL,                              //1 延时处理
    MS_GET_MANU_ID,                             //2 获取厂商标识
    MS_POWER_ON,                                //3 上电处理
    MS_FILE_CMP,                                //4 电表档案比对
    MS_FILE_MODIFY,                             //5 电表档案修改处理
    MS_DEL_SLAVE_DEAL,                          //6 删除从节点处理
    MS_CUS_TASK_DEAL,                           //7 自定义单发任务处理
    MS_SWITCH_CHANNEL,                          //8 切换信道处理
    MS_ROUTER_RESTART,                          //9 重启
    MS_ROUTER_PAUSE,                            //10 暂停
    MS_ROUTER_RESUME,                           //11 恢复
    MS_UPDATE_DEAL,                             //12 载波模块本地文件升级处理
    MS_WORK_MODE,                               //13 设置路由工作模式
    MS_NET_STUDY_DEAL,                          //14 微功率无线组网处理
    MS_ADDR_SEARCH,                             //15 检测表号搜索
    MS_RELAY_TRANS,                             //16 检测透明转发
    MS_ROUTER_DEAL,                             //17 路由操作
    MS_HOUR_AMR,                                //18 小时抄表
    MS_ROUTER_STUDY,                            //19 路由学习
    MS_NORMAL_AMR,                              //20 日常抄表
    MS_SLAVE_DELAY,                             //21 获取从节点监控延时
    MS_SLAVE_MONITOR,                           //22 从节点监控
    MS_BROADCAST_DELAY,                         //23 获取广播延时
    MS_BROADCAST_DEAL,                          //24 广播
    MS_HPLC_REPORT,                             //25 HPLC查询信息上报
    MS_HPLC_FR_SET,                             //26 HPLC频点设置
    MS_HPLC_FR_QUERY,                           //27 HPLC频点查询
    MS_HPLC_AREA,                               //28 HPLC台区识别
    MS_HPLC_OFFLINE,                            //29 HPLC离网感知
    MS_HPLC_REFUSE,                             //30 HPLC拒绝节点上报
    MS_AMR_STATE_QUERY,                         //31 查询抄表状态
    MS_AMR_MODULE_QUERY,                        //32 查询抄表模块信息
}MAIN_CYC_STATE_T;                              //主循环状态字

typedef enum
{
    RCS_NULL = 0,                               //无效
    RCS_RESTART,                                //重启
    RCS_PAUSE,                                  //暂停
    RCS_RESUME,                                 //恢复
    RCS_RESUME_EXEC,                            //恢复执行
}ROUTER_CTRL_STATE_T;                           //路由表控制状态

typedef enum
{
    TASK_INIT = 0,                      //任务初始化
    ROUTERCTRL_DEAL,                    //路由控制
    ROUTERCTRL_DELAY,                   //延时执行
    FUN_DEAL,                           //功能函数执行
    ROUTERRECOVER_DELAY,                //路由恢复延时
    ROUTERRECOVER_DEAL,                 //路由恢复执行
}TASKDEAL_STAT;

typedef struct
{
    uint8 isValidFlag;  //当前路由操作是否有效(是否存在尚未完成的路由操作)
    ROUTER_CTRL_STATE_T eRouterCtrl;  //路由操作类型
    INFO_DATA_T stInfoMsgData;  //原始报文内容
}ROUTER_OPER_DETAIL_T;//路由操作具体内容

typedef struct
{
    INFO_DATA_HEAD_T      InfoDataHead;  // 消息原始HEAD
    
    uint8                 TaskStat;              //任务有效性
    MAIN_CYC_STATE_T      TaskName;              //任务名
    char                  strTaskName[50];       //任务名(字符串)
    int8                  Priority;              //任务优先级
    ROUTER_CTRL_STATE_T   RoutrtCtrl;            //路由控制操作
    uint8                 DelaySec;              //延时
    TASKDEAL_STAT         PlcTaskDeal_Stat;      //载波任务执行状态
    uint8                 InfoMsgBuff[MAX_MSG_PAYLOAD_LEN];           //任务关联的消息内容
    uint32                TaskArrIndex;            //任务作用的端口数组下标(从0开始)
}TASK_FUN_INPUT;//载波任务具体信息，仅用于任务功能函数的入参


typedef struct
{
    INFO_DATA_HEAD_T      InfoDataHead;  // 消息原始HEAD

    uint8                 TaskStat;              //任务有效性
    MAIN_CYC_STATE_T      TaskName;              //任务名
    char                  strTaskName[50];       //任务名(字符串)
    int8                  Priority;              //任务优先级
    ROUTER_CTRL_STATE_T   RoutrtCtrl;            //路由控制操作
    uint8                 DelaySec;              //延时
    TASKDEAL_STAT         PlcTaskDeal_Stat;      //载波任务执行状态
    uint8                 InfoMsgBuff[MAX_MSG_PAYLOAD_LEN];           //任务关联的消息内容
    uint32                TaskArrIndex;            //任务作用的端口数组下标(从0开始)
    int                   (*pPlcTaskFun)(TASK_FUN_INPUT);  //任务功能函数
}PLCTASK_DETAIL;//载波任务具体信息

typedef struct node
{
    PLCTASK_DETAIL        stPlcTaskDetail;       //载波任务具体信息
    struct node           *next;                 //指向下一个任务的指针
}PLCTASK_NODE;//载波任务队列
typedef struct node *PLCTASK_QUEUE; /* 定义LinkList */

typedef struct
{
    uint8                   TaskStat;              //任务有效性
    MAIN_CYC_STATE_T        TaskName;              //任务名
    char*                   strTaskName;           //任务名(字符串)
    uint8                   Priority;              //任务优先级
    ROUTER_CTRL_STATE_T     RoutrtCtrl;            //任务路由操作
    uint8                   DelaySec;              //任务延时时间
    int                     (*pPlcTaskFun)(TASK_FUN_INPUT);  //任务功能函数
}PLCTASK_FUNINFO;//载波任务功能信息


typedef enum
{
    ACT_COMM_IDLE = 0,                          //空闲
    ACT_COMM_SEND,                              //发送
    ACT_COMM_RECV,                              //接收
    ACT_COMM_RETRY,                             //重试
    ACT_COMM_DELAY,                             //延时
}ACTIVE_COMM_STATE_T;                           //主动通讯状态字


typedef enum
{
    MFCS_IDLE = 0,                              //空闲
    MFCS_PAUSE,                                 //暂停
    MFCS_PAUSE_DELAY,                           //暂停延时
    MFCS_SET_TIME,                              //设置时间
    MFCS_READ_AMOUNT_FILE,                      //读取从节点数量
    MFCS_READ_INFO_FILE,                        //读取从节点信息
    MFCS_RESULT_DEAL,                           //结果处理
}METER_FILE_CMP_STATE_T;                        //电表档案比对状态字

typedef enum
{
    ACT_MODULE_IDLE = 0,                          //空闲
    ACT_WAIT_MODULEINFO,                          //等待模块上报信息主动上报
    ACT_READ_MODULEINFO,                          //获取模块上报信息
    ACT_MODULE_SEND_GET_ADDRESS,                  //获取主节点地址
    ACT_MODULE_SEND_SET_ADDRESS,                  //设置主节点地址
    ACT_MODULE_END,                               //模块上报信息结束

}GET_MODULE_STATE_T;                           //主动通讯状态字

typedef struct
{
    uint8   HourAmr;                            //整点抄表
    uint8   RelayTrans;                         //中继
    uint8   AddrSearch;                         //表号搜索
}BREAK_STATE_T;   

typedef enum
{
    MFMS_IDLE = 0,                              //空闲
    MFMS_INIT,                                  //初始化
    MFMS_PAUSE,                                 //暂停
    MFMS_DELET_METERFILE_FLAG,    //忽略档案修改//start Common Problem
    MFMS_FILE_CLEAR,                            //档案清除
    MFMS_FILE_ADD,                              //档案添加
    MFMS_FILE_DELETE,                           //档案删除
    MFMS_RESTART,                               //重启
}METER_FILE_MODIFY_STATE_T;                     //电表档案修改状态字

typedef enum
{
    AS_IDLE = 0,                                //空闲
    AS_PAUSE,                                   //暂停
    AS_PAUSE_DELAY,                             //暂停延时
    AS_ACT_SEARCH_MTRID,                        //激活自动搜表
    AS_ACT_REG,                                 //激活注册
    AS_WAIT_REPORT,                             //等待上报
    AS_GET_RUN_STATE,                           //读取路由运行状态
    AS_SEARCH_END,                              //搜索结束
}ADDR_SEARCH_STATE_T;                           //表号搜索状态字

typedef enum
{
    INNER_RTS_IDLE = 0,                               //空闲
    INNER_RTS_BROASTDELAY,                            //广播对时获取通道的延时时间
    INNER_RTS_SEND,                                   //发送
    INNER_RTS_RECV,                                   //接收
    INNER_RTS_RETRY,                                  //重试
    INNER_RTS_END,                                    //结束
}INNER_RELAY_TASK_STATE_T;                            //内部处理穿透的逻辑

typedef enum
{
    RSS_IDLE = 0,                               //空闲
    RSS_SET_STUDY_MODE,                         //设置路由学习模式
    RSS_GET_RUN_STATE,                          //读取运行状态
    RSS_DELAY,                                  //延时状态
    RSS_SET_AMR_MODE,                           //设置路由抄表模式
    RSS_STUDY_END,                              //学习结束
}ROUTER_STUDY_STATE_T;                          //路由学习处理状态字

typedef enum
{
    RTS_IDLE = 0,                               //空闲
    RTS_PAUSE,                                  //暂停
    RTS_PAUSE_DELAY,                            //暂停延时
    RTS_SET_TIME,                               //设置时间
    RTS_READ_MODULE_STATE,                      //查询模块状态
    RTS_READ_DELAY,                             //查询延时
    RTS_SEND,                                   //发送
    RTS_RECV,                                   //接收
    RTS_RETRY,                                  //重试
    RTS_RESUME,                                 //恢复
    RTS_END,                                    //结束
}RELAY_TRANS_STATE_T;

typedef enum
{
    MUD_IDLE = 0,                           //空闲
    MUD_DELAY,                              //延时
    MUD_READ_FILE_DELAY,
    MUD_OPEN_FILE,                          //打开升级文件
    MUD_READ_FILE,                          //读取升级文件
    MUD_SEND,                               //发送
    MUD_RECV,                               //接收
    MUD_RETRY,                              //重试
    MUD_END_AGAIN,                          //整体重试
    MUD_END,                                //结束处理
    MUD_GET_RUN_STATE,                      //获取路由运行状态
}MODULE_UPDATE_DEAL_T;                      //载波模块升级状态字

typedef enum
{
    NSD_IDLE = 0,                                //空闲
    NSD_ACT_START_NET_STUDY,                     //启动组网(AFN=11H-F102, AFN=11H-F101)
    NSD_WAIT_STATE,                              //等待状态
    NSD_QUERY_NET_INFO,                          //查询组网信息(AFN=10-F4)
    NSD_NET_STUDY_END,                           //组网结束
    NSD_QUERY_SLAVE_INFO,                        //查询从节点信息(AFN=10H-F101)
    NSD_QUERY_SLAVE_INFO_RECV,                   //接收从节点信息状态
    NSD_END,                                     //结束
}NET_STUDY_DEAL_T;                               //微功率无线组网状态字

typedef enum
{
    DSD_IDLE = 0,                                //空闲
    DSD_SEND_STATE,                              //发送删除指令状态
    DSD_END,                                     //结束
}DEL_SLAVE_DEAL_T;                               //删除从节点信息

typedef enum
{
    SCD_IDLE = 0,                                //空闲
    SCD_SEND_STATE,                              //发送切换信道状态
    SCD_DELAY_STATE,                             //延时状态
    SCD_END,                                     //结束
}SWITCH_CHANNEL_DEAL_T;                          //切换信道

typedef enum
{
    HPLC_QUERY_IDLE = 0,                               //空闲
    HPLC_QUERY_SEND,                                   //发送
    HPLC_QUERY_RECV,                                   //接收
    HPLC_QUERY_RETRY,                                  //重试
    HPLC_QUERY_END,                                    //结束
}HPLC_QUERY_STATE_T;                            //HPLC信息查询的流程

typedef enum
{
    CTD_IDLE = 0,                                //空闲
    CTD_SEND_STATE,                              //发送状态
    CTD_END,                                     //结束
}CUS_TASK_DEAL_T;                                //自定义任务


typedef struct
{
    GET_MODULE_STATE_T          GetManuID;      //获取厂商模块信息
    BREAK_STATE_T               BreakState;     //中断状态
    METER_FILE_MODIFY_STATE_T   MeterFileModify;//电表档案修改状态字
    METER_FILE_CMP_STATE_T      MeterFileCmp;   //电表档案比对状态字
    ADDR_SEARCH_STATE_T         AddrSearch;     //表号搜索
    INNER_RELAY_TASK_STATE_T    RelayTrans;      // 中继任务转发
    INNER_RELAY_TASK_STATE_T    SlaveMonitor;      // 从节点监控
    INNER_RELAY_TASK_STATE_T    SlaveMonitorTime;  // 获取从节点监控延时
    ROUTER_STUDY_STATE_T        RouterStudy;    //路由学习
    RELAY_TRANS_STATE_T         HourAmr;        //整点抄表
    RELAY_TRANS_STATE_T         NormalAmr;      //日常抄表
    MODULE_UPDATE_DEAL_T        ModuleUpdateDeal; //本地升级处理
    NET_STUDY_DEAL_T            NetStudyDeal;   //微功率无线组网处理
    DEL_SLAVE_DEAL_T            DelSlaveDeal;   //删除从节点处理
    SWITCH_CHANNEL_DEAL_T       SwitchChannel;  //切换信道处理
    HPLC_QUERY_STATE_T          HplcQuery;      //HPLC信息查询
    CUS_TASK_DEAL_T             CustomTask;     //自定义任务
}TOTAL_STATE_T;  

typedef struct
{
    uint8   AmrBusyFlag         :1;             //抄表忙/闲标志
    uint8   AmrFinishFlag       :1;             //抄表任务时段完成标志
    uint8   ReserveBit3         :1;             //保留
    uint8   ReserveBit4         :1;             //保留
    uint8   ReserveBit5         :1;             //保留
    uint8   ReserveBit6         :1;             //保留
    uint8   ReserveBit7         :1;             //保留
    uint8   ReserveBit8         :1;             //保留
}AMR_WORK_STATE_BIT_T;

typedef union
{
    AMR_WORK_STATE_BIT_T    BitSect;
    uint8                   Word;
}AMR_WORK_STATE_T;


typedef struct
{
    uint16        SlaveCnt;                     //从节点数量
    uint16        MaxAllowSlaveCnt;             //最大允许从节点数量
    uint16        MtrCnt;                       //电表总数
    uint16        CurMtrIndex;                  //当前电表索引
    uint8         AcqUnitFailureFlag;           //采集器失败标志
    uint16        AmrSucceedCnt;                //抄表成功块数
    uint8         OftenPriority;                //频繁任务执行最低优先级
    AMR_WORK_STATE_T    AmrWorkState;           //抄表工作状态
    uint8   MachineReboot;                      //重启集中器标志
    uint8         SucRateState;                 //日冻结成功率统计状态 0:全部完成 1: >=95% 2: <95%
} TmnAmrInfo_T;                                 //终端抄表信息

typedef struct
{
    uint16  Pn;                                 //测量点号
    uint8   SpecialFlag;                        //特殊标志，有2个功能:
                                                    //1:下发到路由板标志(0:未设置 / 1:已设置)
                                                    //2:抄表成功标志(0:失败 / 1:成功)
    uint8   AmrTurnFlag;                        //抄表轮次置位标记
    uint8   AdaptFlag;                          //自适应标志(0:单抄未结束 / 1:单抄已结束)
    uint8   CurAdaptIndex;                      //自适应当前帧数
    uint8   AcqFlag;
    uint16  LastTaskId;                         //最近一次抄读数据所属的方案号
    BOOL    IsNeedGetSafeMode;                  //是否需要读取面向对象安全模式开关
    uint8   BroadFailCnt;                       //抄读失败计数

    uint32  nPortID;   //端口号
    //uint16  nMeterFilesID; //档案序号
    uint8   aMeterFilesAddr[6];  //档案地址
    uint8   aMeterFilesMac[6];  //档案MAC
    uint8   nProtocolType;  //协议类型
    //uint8   nCommPara;  //通信参数
} MtrAmrInfo_T;                                 //测量点抄表信息


typedef struct
{
    TmnAmrInfo_T TmnAmrInfo;                    //终端抄表信息
    MtrAmrInfo_T MtrAmrInfo[MAX_AMR_COLLECT_CNT]; //测量点抄表信息
} AmInfo_T;                                     //抄表信息


typedef enum
{
    FILES_MODEFY_NULL = 0,                           //档案无需修改
    FILES_MODEFY_CLEAR,                              //档案全清的方式
    FILES_MODEFY_UPDATE,                                //档案添加/删除的方式
}METER_FILES_MODEFY_TYPE_E;                                  //电表档案更改类型

typedef enum
{
    PRTL_NULL = 0,
    PRTL_97,                                    //DLT/645-1997规约类型
    PRTL_07,                                    //DLT/645-2007规约类型
    PRTL_OOP,                                   //面向对象规约电表类型
    RRTL_188,                                   //面向对象规约188协议
}PRTL_MODE_T;                                   //规约类型

typedef enum
{
    METER_NULL = 0,
    METER_PLC,                                  //PLC电表
    METER_485,                                  //485电表
}MTR_TYPE_T;                                    //电表类型


typedef struct
{
    uint8    DelayFlag;                         //组帧时的通信延时标志
    uint16   DelayCnt;                          //通信延时时长
    MTR_TYPE_T   MtrType;                       //电表类型(1:plc/2:485)
    uint8   PrtlMode;                           //规约类型
    uint8   AcqUnitAddr[6];                     //采集器地址
    uint8   MtrAddr[6];                         //电表地址
    uint8   CtrlCode;                           //控制码
    uint8   DataLen;                            //数据域长度
    uint8   DataBuff[2048];                      //数据域内容
}MeterPrtlFmt_T;                                //电表规约格式


typedef struct
{
    uint8   MeterAddr[6];                       //电表地址
    uint8   PrtlMode;                           //协议类型(1:07/30:07)
    uint8   RealPhase;                          //电表相位(低半字节/(0:未知/1~3对应ABC相))/信号品质(高半字节)
    uint8   AcqUnitId[6];                       //采集器地址
    uint8   ChgFlag;                            //变更标志
    uint8   MasterAddr[6];                      //主节点地址
    uint8   AddrSearchTime[7];                  //抄表时间
}ACT_REPORT_INFO_T;     //主动上报信息

/*******************************************************************************
* 模块类型枚举定义
*******************************************************************************/
typedef enum
{
    MMID_NULL = 0,
    MMID_NOT_NULL,
} ModuleManuID_T;                               //模块厂商类型

#define AUAM_TWO_ADDR_PRE       0               //双地址,电表地址放在数据域前
#define AUAM_TWO_ADDR_POST      1               //双地址,电表地址放在数据域后
#define AUAM_ONE_ADDR           2               //单地址,测量点地址替换376.2的目的地址


typedef struct
{
    uint8       ValidFlag;                 //1:有效; 0:无效
    uint8       FrameSeq;                  //F1-01帧序号
    uint16      Pn;                        //该帧对应的测量点号
    int         MsIndex;                   //该帧对应的方案MS索引
    uint8       PrtlMode;                  //对应的表规约
    MTR_TYPE_T  Mettype;                   //节点类型
    uint8       MeterAddr[6];              //电表地址
    uint8       AcqAddr[6];                //采集器地址
    time_t      CommBeginTime;             //该帧通讯开始时间
}FRAME_SEQ_INFO_T;                    //帧序号信息

typedef struct
{
    uint8 CurSeqCnt;    //最多5帧
    FRAME_SEQ_INFO_T FrameSeqInfo[MAX_NORMAL_AMR_INFO_CNT];
}GLOBAL_SEQ_INFO_T;


typedef struct
{
    uint16  PointNum;  //点抄测量号
    uint8   SuccessLabel;  //点抄成功标志
}GUI_RELAY_PN_T;      //保存点抄的测量点号


typedef struct
{
    uint8   ValidFlag;  //第1个字节为点抄启动标志0x55有效,0xBB为点抄取消态,其余为0xAA
    uint16  RelayNum;   //第2、3个字节为要点抄的数目
    GUI_RELAY_PN_T GuiRelayPn[100];  //第4、5个字节起为要点抄测量号,第6为点抄成功标志，后面重复第4、5、6个字节格式
}GUI_RELAY_PN_LIST_T;  //保存点抄的测量点号


typedef enum
{
    TYPE_NULL = 0,                           //空闲
    TYPE_SETTERMALADDRESS,                           //设置终端地址
    TYPE_INFOADDMETERFILES,                           //添加表档案
    TYPE_INFODELETEMETERFILES,                           //删除表档案
    TYPE_INFOCLEARMETERFILES,                           //清除表档案
    TYPE_INFOCURRENTFILESNUM,                           //当前档案数量
    TYPE_INFOQUERYFILESDETAIL,                           //查询档案
    TYPE_INFOROUTERDEAL_PAUSE,                           //暂停抄表
    TYPE_INFOROUTERDEAL_RECO,                           //恢复抄表
    TYPE_INFOROUTERDEAL_REST,                           //重启抄表
    TYPE_INFOACTIVEMETER,                           //主动抄表
    TYPE_INFOSEARCHMETER,                           //启动搜表
    TYPE_INFODATATRANSMIT,                           //数据透传
    TYPE_INFOSLAVEMONITOR,                           //从节点监控
    TYPE_INFOGETBROADCASTDELAY,                           //获取广播延时
    TYPE_INFOBROADCASTDEAL,                           //广播
}INFO_TASK_TPYE_T;                      //消息任务类型

typedef enum
{
    METER_PROTOCOL_NULL = 0,                      //无法解析
    METER_PROTOCOL_645,                           //645报文格式
    METER_PROTOCOL_69845,                         //698.45报文格式
}METER_PROTOCOL_TPYE_T;                      //报文格式

typedef struct
{
    uint8 HidRead[2040];
    uint8 pinjie;
    uint8 HidWrite[2040];
}COMM_JOIN_MESSAGE_T;      //串口拼接报文信息

typedef enum
{
    HPLC_NONE = 0,
    HPLC_TOPOLOGYINFO,
    HPLC_CHIPINFO,
    HPLC_NETSINFO,
    HPLC_PHASEINFO,
    HPLC_SCALE,
    HPLC_MASTERID,
    HPLC_SLAVEID,
    //HPLC_BASEINFO,
    HPLC_MODULEINFO,
    //HPLC_NETNODEINFO,
    HPLC_NODEVERSION,
    HPLC_LAST,
}HPLC_TPYE_E;                               //删除从节点信息

typedef struct
{
    uint8   topo:1;  //网络拓扑信息
    uint8   chip:1;  //宽带载波芯片信息
    uint8   nets:1;  //多网络信息
    uint8   phase:1;  //节点相线信息
    uint8   scale:1;  //网络规模
    uint8   master:1;  //本地主节点模块ID信息
    uint8   slave:1;  //从节点ID信息
    //uint8   base:1;  //网络基本信息
    uint8   module:1;  //模块版本信息
    //uint8   node:1;  //网络节点信息
    uint8   nodeversion:1;  //网络节点版本
    uint8   res:7;  //保留
} HPLC_TYPE_BIT_T;  //注意与上述枚举对应

typedef union
{
    HPLC_TYPE_BIT_T BitType;
    uint16 Value;
} HPLC_TYPE_QUERY_UN;


typedef struct
{
    uint8 nFrValue;  //当前HPLC频点
    uint8 nAreaIdentifyState;  //当前HPLC台区识别开启状态
    uint8 nOfflineFeelState;  //当前HPLC离网感知开启状态
    uint8 nRefuseReportState;  //当前HPLC拒绝节点上报开启状态
}GUI_SHOW_MESSAGE_T;      //GUI显示内容结构体

typedef struct
{
   uint16  TotalPackageNum;
   uint16  PackageLen;
   uint16  CurrentPackageNo;
}UpdateInfoState;     //载波模块本地升级功能记录结构体

typedef struct
{
   uint8   isEnableUpdate; //模块是否支持升级功能 00:不支持  01:支持
   uint16  packageLen;  //包长度 每个模块可能要求不一样
   uint8   crcMode;         //是否需要校验 00:不需要校验  01:CRC16校验  其它:待定
   uint8   isNeedDeleteDownFile;//是否需要清除下装文件  00:不用清除 01:需要清除
}UpdateSpecialProcess;   //载波模块本地升级,模块间特殊功能处理

#define MAX_CCO_STA_NUM    500             //CCO和全部STA最大数量

typedef struct
{
   uint8 aSoftVer[2];//软件版本号
   uint8 nVerDay;//版本日期 日
   uint8 nVerMonth;//版本日期 月
   uint8 nVerYear;//版本日期 年
   char aFacCode[2];//模块厂商代码
   char aChipCode[2];//芯片代码
}SINGLE_CCO_STA_INFO_T;   //查询宽带载波模块节点信息数据单元格式


typedef struct
{
   uint16 nCcoStaNum;//节点总数量
   uint8 aAddr[MAX_CCO_STA_NUM][6];//节点地址
   SINGLE_CCO_STA_INFO_T aInfo[MAX_CCO_STA_NUM];//节点信息
   uint8 aType[MAX_CCO_STA_NUM];//节点类型
}CCO_STA_INFO_T;   //查询宽带载波模块节点信息数据单元格式


#ifdef __cplusplus
}
#endif

#endif // __CCO_REDEF_H__
