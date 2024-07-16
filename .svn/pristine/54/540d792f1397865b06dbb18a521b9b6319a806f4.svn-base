/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief： 功能描述
* @date： 2019-10-25
*********************************************************************
*/


#ifndef __CCO_COMMON_H__
#define __CCO_COMMON_H__

#include "CcoRedef.h"



#ifdef __cplusplus
extern "C" {
#endif

#if defined(_MSC_VER)
#pragma warning(disable:4201)
#endif

extern uint8 gNullArr[100];//空数组, 用于比对

/*******************************************************************************
* 本地对上通信 UDP通信参数
*******************************************************************************/
extern char       *gCcoRouterAppName;
extern uint16      gCcoRouterAppPort;
extern char       *gCcoRouterAppIP;

/*******************************************************************************
* 调度管理APP UDP通信参数
*******************************************************************************/

//调度管理APP CTS_ROUTER_APP_NAME
extern char       *gCtsRouterAppName[MAX_ALLOWED_CTS_THREAD_NUM];  //调度管理APP CTS_ROUTER_APP_NAME

//调度管理APP CTS_ROUTER_APP_PORT
extern uint16      gCtsRouterAppPort[MAX_ALLOWED_CTS_THREAD_NUM];  //调度管理APP CTS_ROUTER_APP_PORT

//调度管理APP CTS_ROUTER_APP_IP
extern char       *gCtsRouterAppIP[MAX_ALLOWED_CTS_THREAD_NUM];  //调度管理APP CTS_ROUTER_APP_IP

/*******************************************************************************
* 本地两个载波模块参数
*******************************************************************************/

//端口号
extern uint32      gPortID[MAX_ALLOWED_DOWN_THREAD_NUM];  //两个端口号
extern BOOL        bSetPortIDFlag[MAX_ALLOWED_DOWN_THREAD_NUM];  //是否收到端口号设置的标志

//通信模块设备名称
extern char       *gDevName[MAX_ALLOWED_DOWN_THREAD_NUM];  //两个通信设备(智芯)
extern char       *gDingxinDevName[MAX_ALLOWED_DOWN_THREAD_NUM]; //两个通信设备(鼎信)
extern DEV_HAL_INFO gDevHalInfo[MAX_ALLOWED_DOWN_THREAD_NUM];

/*******************************************************************************
* 私有文件路径
*******************************************************************************/

//档案私有文件名
extern char       *gLocalFilesName[MAX_ALLOWED_DOWN_THREAD_NUM];  //两个档案私有文件名

//通信模块地址私有文件名
extern char       *gModuleManuName[MAX_ALLOWED_DOWN_THREAD_NUM];  //两个私有文件

//端口号信息私有文件名
extern char       *gPortIDName[MAX_ALLOWED_DOWN_THREAD_NUM];  //两个私有文件

//显示相关信息私有文件名
extern char       *gGuiInfoName[MAX_ALLOWED_DOWN_THREAD_NUM];  //两个私有文件

/*******************************************************************************
* 终端地址
*******************************************************************************/
extern uint8       gTerminalAddr[MAX_ALLOWED_DOWN_THREAD_NUM][6];  //终端地址
extern BOOL        gUpdateTerminalAddrFlag[MAX_ALLOWED_DOWN_THREAD_NUM];  //更新终端地址标志

/*******************************************************************************
* 变量声明
*******************************************************************************/
extern PastTimeFlag_T             gPastTimeFlag[MAX_DOWN_THREAD_NUM];                           //过时间标志
extern DateTime_T                 gCurBinTime[MAX_DOWN_THREAD_NUM];                             //当前BIN时间
extern DateTime_T                 gCurBcdTime[MAX_DOWN_THREAD_NUM];                             //当前BCD时间
extern DateTime_T                 gPreBinTime[MAX_DOWN_THREAD_NUM];                             //上次BIN时间
extern DateTime_T                 gCurBcdTimeUp;  //当前BCD时间(记录上行消息)
extern DateTime_T                 gCurBcdTimeDown[MAX_DOWN_THREAD_NUM];  //当前BCD时间(记录下行消息)

//上电标志
extern int                        gPowerOnFlag[MAX_DOWN_THREAD_NUM];                            //上电标志

//上电获取模块信息
extern ModuleManuID_T             gModuleManuID[MAX_DOWN_THREAD_NUM];               //模块厂商标识
extern LOCAL_COMM_MODULE_RUNNING_PATTERN_INFO  gLocalCommModuleInfo[MAX_DOWN_THREAD_NUM];         //模块信息的全局变量

//档案列表全局变量
extern AMR_METER_FILES_LIST_T gAmrMeterFilesListTemp[MAX_DOWN_THREAD_NUM];  //档案列表临时变量(消息缓存临时变量)
extern AMR_METER_FILES_LIST_T gAmrMeterFilesList[MAX_DOWN_THREAD_NUM];  //档案列表全局变量
extern AMR_METER_FILES_LIST_T gToAddFilesList[MAX_DOWN_THREAD_NUM];  //需要添加的档案列表全局变量
extern AMR_METER_FILES_LIST_T gToDeleteFilesList[MAX_DOWN_THREAD_NUM];  //需要删除的档案列表全局变量

extern uint8                     gMeterFileAddDelFlagTemp[MAX_DOWN_THREAD_NUM];       //档案增加删除标志(消息缓存临时变量)
extern uint8                     gMeterFileClearFlagTemp[MAX_DOWN_THREAD_NUM];        //档案清除标志(消息缓存临时变量)
extern uint8                     gMeterFileAddDelFlag[MAX_DOWN_THREAD_NUM];       //档案增加删除标志
extern uint8                     gMeterFileClearFlag[MAX_DOWN_THREAD_NUM];        //档案清除标志
extern uint8                     gMeterFileCompareFlag[MAX_DOWN_THREAD_NUM];      //档案比对标志
extern uint8                     gMeterFileModifyFlag[MAX_DOWN_THREAD_NUM];       //档案修改标志
extern uint8                     gMeterFileModifyState[MAX_DOWN_THREAD_NUM];      //档案修改状态，0：更新完，1：更新中
extern METER_FILES_MODEFY_TYPE_E gMeterFilesModifyType[MAX_DOWN_THREAD_NUM];      //档案修改方式

extern AMR_ACTIVE_INFO_LIST_T gActiveAmrInfoList[MAX_DOWN_THREAD_NUM];  //存放抄表消息的缓冲区

//载波任务调度使用的全局变量
extern PLCTASK_QUEUE              PlcTaskQueue[MAX_DOWN_THREAD_NUM];            //载波任务队列
extern PLCTASK_DETAIL             gPlcTaskDetail[MAX_DOWN_THREAD_NUM];                 //当前载波具体任务细节
extern PLCTASK_DETAIL             gPlcTaskDetailSearchMeter[MAX_DOWN_THREAD_NUM];  //当前载波具体任务细节(搜表)

//各个子任务内部使用的全局变量
extern TOTAL_STATE_T              gTotalState[MAX_DOWN_THREAD_NUM];                             //总状态字

//抄表汇总信息
extern AmInfo_T    gAmrInfo[MAX_DOWN_THREAD_NUM];			                //抄表汇总信息


//收到的消息缓存区
extern INFO_DATA_LIST_T gInfoDataList;                      //收到的消息缓存区列表

//主动上报消息INDEX
extern uint16  ActiveInfoIndex;                                      //主动上报消息INDEX

//任务集合
extern const PLCTASK_FUNINFO PlcTask_FunList[];

//plc复位引脚和模块插入引脚检测文件描述符
extern int fd_plc[MAX_DOWN_THREAD_NUM];//plc复位引脚和模块插入引脚检测文件描述符

//宽带日常抄表队列
extern GLOBAL_SEQ_INFO_T gGlobalSeqInfo[MAX_DOWN_THREAD_NUM];

//路由控制操作
extern ROUTER_CTRL_STATE_T   gRoutrtCtrl[MAX_DOWN_THREAD_NUM];           //路由控制操作

//路由控制操作
extern ROUTER_OPER_DETAIL_T  gRouterOper[MAX_DOWN_THREAD_NUM];           //路由控制操作

//启动搜表标志
extern WHETHER_START_SEARCH_METER_T gWhetherStartSearchMeter[MAX_DOWN_THREAD_NUM];       //启动搜表标志

//暂停搜表标志
extern BOOL gPauseSearchMeterFlag[MAX_DOWN_THREAD_NUM];       //暂停搜表标志

//搜表档案序号(从0开始)
extern uint16 gSearchMeterFilesIndex[MAX_DOWN_THREAD_NUM];

//穿透任务队列
extern AMR_DATA_TRANSMIT_LIST_T gDataTransmitQueue[MAX_DOWN_THREAD_NUM];

//从节点监控任务队列
extern AMR_SLAVE_MONITOR_LIST_T gSlaveMonitorQueue[MAX_DOWN_THREAD_NUM];

//是否获取广播延时
extern WHETHER_GET_BROADCAST_DELAY_T gWhetherGetBroadcastDelay[MAX_DOWN_THREAD_NUM];

//是否广播
extern WHETHER_DO_BROADCAST_T gWhetherDoBroadcast[MAX_DOWN_THREAD_NUM];

//查询HPLC信息 信息类型
HPLC_TYPE_QUERY_UN gQueryTypeHPLC[MAX_DOWN_THREAD_NUM];  //每个bit对应不同的HPLC查询类型

//5个模组的信息
extern BOOL gQueryAllModuleFlag;  //程序刚启动时, 读取所有模块信息
extern MODULE_INFO_T gModuleInfo[5];  //5个模组的信息
extern uint8 gReloadModuleFlag[6];  //是否需要重新从模组管理器加载模组信息

//备份最近一次是否CCO模块标记
extern uint8 gCcoFlag[5];  //备份最近一次是否CCO模块标记

//HPLC频点设置标志
extern uint8 gFrSetFlag[MAX_DOWN_THREAD_NUM];  //HPLC频点设置标志
//HPLC频点设置值
extern uint8 gFrSetValue[MAX_DOWN_THREAD_NUM];  //HPLC频点设置值

//HPLC频点查询标志
extern uint8 gFrQueryFlag[MAX_DOWN_THREAD_NUM];  //HPLC频点查询标志
extern INFO_DATA_HEAD_T gFrQueryInfoDataHead[MAX_DOWN_THREAD_NUM];  //HPLC频点查询消息原始HEAD
extern uint8 gInitFrQueryFlag[MAX_DOWN_THREAD_NUM];  //HPLC频点查询标志(上电初始化查询)

//查询抄表状态结构体
extern AMR_QUERY_READING_STATE_T gAmrStateQuery[MAX_DOWN_THREAD_NUM];  //查询抄表状态结构体

//查询抄表模块信息结构体
extern AMR_QUERY_MODULE_INFO_T gAmrModuleQuery[MAX_DOWN_THREAD_NUM];  //查询抄表模块信息结构体

//HPLC台区识别标志
extern uint8 gAreaIdentifyFlag[MAX_DOWN_THREAD_NUM];  //HPLC台区识别标志
//HPLC台区识别值
extern uint8 gAreaIdentifyValue[MAX_DOWN_THREAD_NUM];  //HPLC台区识别值

//HPLC离网感知标志
extern uint8 gOfflineFeelFlag[MAX_DOWN_THREAD_NUM];  //HPLC离网感知标志
//HPLC离网感知值
extern uint8 gOfflineFeelValue[MAX_DOWN_THREAD_NUM];  //HPLC离网感知值

//HPLC拒绝节点上报标志
extern uint8 gRefuseNodeReportFlag[MAX_DOWN_THREAD_NUM];  //HPLC拒绝节点上报标志
//HPLC拒绝节点上报开关
extern uint8 gRefuseNodeReportValue[MAX_DOWN_THREAD_NUM];  //HPLC拒绝节点上报开关

//模块插拔重新获取设备
extern uint8 gReloadDevFlag[MAX_DOWN_THREAD_NUM];       //模块插拔重新获取设备

//gui展示内容
extern GUI_SHOW_MESSAGE_T gGuiShowMessage[MAX_DOWN_THREAD_NUM];  //gui展示内容

//升级文件标识全局标记
extern uint8 gPlcUpdateFileMark[MAX_DOWN_THREAD_NUM];  //升级文件标识全局标记. 0x03:主节点升级文件; 0x07:主节点+全部子节点;0x08:全部子节点

//CCO各类升级文件路径
extern char *gRouterUpdateFile[MAX_ALLOWED_DOWN_THREAD_NUM];  //主节点升级包
extern char *gRouterUpdateFileBack[MAX_ALLOWED_DOWN_THREAD_NUM];  //主节点升级包
extern char *gAllRouterUpdateFile[MAX_ALLOWED_DOWN_THREAD_NUM];  //主节点+全部子节点升级
extern char *gAllRouterUpdateFileBack[MAX_ALLOWED_DOWN_THREAD_NUM];  //主节点+全部子节点升级
extern char *gSubRouterUpdateFile[MAX_ALLOWED_DOWN_THREAD_NUM];  //全部子节点升级
extern char *gSubRouterUpdateFileBack[MAX_ALLOWED_DOWN_THREAD_NUM];  //全部子节点升级

//查询宽带载波模块节点信息数据单元格式
extern CCO_STA_INFO_T gAllCcoStaInfo[MAX_DOWN_THREAD_NUM];

/*******************************************************************************
* 函数申明
*******************************************************************************/
extern uint16 GetInfoIndex();
extern int ModuleUpdateDeal(TASK_FUN_INPUT stTaskFunInput);
extern uint8 ModuleID_Recv(AppData_T  *AppData,ModuleManuID_T *ModuleManuID);
extern int UnPackModuleInfo(AppData_T RecvAppData, uint8 nArrIndex);
extern uint8 GetMasterAddr_Recv(AppData_T  *AppData, uint8 nArrIndex);
extern void RegPlcModule(uint8 nArrIndex);
extern int GetModuleManuID(TASK_FUN_INPUT stTaskFunInput);
extern int CustomTaskDeal(TASK_FUN_INPUT stTaskFunInput);
extern uint8 CheckMeterFilesResult(char *MeterInfo,uint8 MeterNum, uint8 nArrIndex);
extern uint8 CheckAllMeterFileCmpResult(uint8 nArrIndex);
extern int MeterFileCmpDeal(TASK_FUN_INPUT stTaskFunInput);
extern int8 Comm_AddSlaveInfo_New(AMR_METER_FILES_LIST_T *PModuleInfo, uint8 AddNum, uint8 nArrIndex);
extern int8 Comm_DeleteRouterInfo_New(AMR_METER_FILES_LIST_T *PModuleInfo, uint8 DeleteNum, uint8 nArrIndex);
extern int MeterFileMofifyDeal(TASK_FUN_INPUT stTaskFunInput);
extern int ReportInfoDeal(uint8 *ReportInfo, uint8 *RealPhase, uint8 *AddrSearchReportType, AppData_T *AppData, uint8 nArrIndex);
extern int ActReportNodeDeal(uint8 *ReportInfo, uint8 *RealPhase, uint8 *AddrSearchReportType, uint8 nArrIndex);
extern void SaveReportInfo_F1(uint8 ReportCnt, uint8 *ReportInfo, uint8 RealPhase, uint8 nArrIndex);
extern void SaveReportInfo_F4_Collector(uint8 ReportCnt, uint8 *ReportInfo, uint8 RealPhase, uint8 nArrIndex);
extern void SaveReportInfo_F4_Meter(uint8 ReportCnt, uint8 *ReportInfo, uint8 RealPhase, uint8 nArrIndex);
extern void SaveReportInfo_F4(uint8 ReportCnt, uint8 *ReportInfo, uint8 RealPhase, uint8 nArrIndex);
extern int AddrSearchDeal(TASK_FUN_INPUT stTaskFunInput);
extern int DelSlaveInfoDeal(TASK_FUN_INPUT stTaskFunInput);
extern int CmdDataRelay(MeterPrtlFmt_T MeterData, uint8 *buf, uint32 buf_len, uint8 nArrIndex);
extern int RelayCmdMeterPrtlData(MeterPrtlFmt_T MeterPrtlData, uint8 *buf, uint32 buf_len, uint8 nArrIndex);
extern int RelayTransDeal(TASK_FUN_INPUT stTaskFunInput);
extern int SlaveMonitorDeal(TASK_FUN_INPUT stTaskFunInput);
extern int GetSlaveMonitorDelay(TASK_FUN_INPUT stTaskFunInput);
extern int GetBroadcastDelay(TASK_FUN_INPUT stTaskFunInput);
extern void CmdOopBroadcast(MeterPrtlFmt_T MeterData, uint8 nArrIndex);
extern int BroadcastDeal(TASK_FUN_INPUT stTaskFunInput);
extern int WirelessNetStudyDeal(TASK_FUN_INPUT stTaskFunInput);
extern int SwitchChannelDeal(TASK_FUN_INPUT stTaskFunInput);
extern int RouterStudyDeal(TASK_FUN_INPUT stTaskFunInput);
extern void ClearCommRecvBuff(uint8 nArrIndex);
extern void ClearCommQueueBuff(LockQueue_T *pQueueBuff);
extern int RecvFrameCheck(AppData_T AppData,AppData_T RecvAppData, uint8 nArrIndex );
extern int CommFunDeal(AppData_T SendAppData,AppData_T *RecvAppData, uint8 nArrIndex );
extern int CommFunDealByType(AppData_T SendAppData,AppData_T *RecvAppData, uint8 nArrIndex, int32 nFunType);
extern int AppRouterDealFun(TASK_FUN_INPUT stTaskFunInput);
extern int OftenAmrDeal(TASK_FUN_INPUT stTaskFunInput);
extern uint16 PackFrameDataX(AppData_T AppData, uint8* Buff, uint8 nArrIndex);
extern uint16 CmdMeterPrtlMakeFrame(AppData_T AppData, uint8 *Buff, uint8 nArrIndex);
uint16 CcoPackAFN03F4(AppData_T AppData, uint8 *Buff, uint8 nArrIndex);
extern int ParalAmrMakeFrame(uint8 nArrIndex);
extern uint8 IsValidSeq(uint8* OutSeq, uint8 InSeq, uint8 nArrIndex);
extern uint8 IsActiveAmrFinished(uint8 nArrIndex);
extern void RefreshPnState(uint8 SeqIndex, uint8 nArrIndex);
extern int BroadBand_ParallelAmr(TASK_FUN_INPUT stTaskFunInput);
extern int NormalDataReading(TASK_FUN_INPUT stTaskFunInput);
extern int HplcInfoReport_NetNode(TASK_FUN_INPUT stTaskFunInput);
extern int HplcInfoReport_NetNode(TASK_FUN_INPUT stTaskFunInput);
extern int HplcInfoReport_Slave(TASK_FUN_INPUT stTaskFunInput);
extern int HplcInfoReport_Master(TASK_FUN_INPUT stTaskFunInput);
extern int HplcInfoReport_Scale(TASK_FUN_INPUT stTaskFunInput);
extern int HplcInfoReport_Phase(TASK_FUN_INPUT stTaskFunInput);
extern int HplcInfoReport_Nets(TASK_FUN_INPUT stTaskFunInput);
extern int HplcInfoReport_Chip(TASK_FUN_INPUT stTaskFunInput);
extern int HplcInfoReport_Topo(TASK_FUN_INPUT stTaskFunInput);
extern int HplcInfoReport(TASK_FUN_INPUT stTaskFunInput);
extern int HplcFrSet(TASK_FUN_INPUT stTaskFunInput);
extern int HplcFrQuery(TASK_FUN_INPUT stTaskFunInput);
extern int HplcAreaIdentify(TASK_FUN_INPUT stTaskFunInput);
extern int HplcOfflineFeel(TASK_FUN_INPUT stTaskFunInput);
extern int HplcRefuseNodeReport(TASK_FUN_INPUT stTaskFunInput);
extern int8 CheckModuleSoftReset(int8 CheckRmt, uint8 nArrIndex);
extern void Comm_ConfirmAckTakeFrameSeq(AppData_T ModuleRequestAppData, uint8 nArrIndex);
extern void PackInfoMsgStruct(uint16 IID_ID, uint16 IOP_ID, uint16 nBuffLen, uint8 *aBuff, INFO_DATA_T *InfoMsgData);
extern void InfoSimpleAck(INFO_DATA_T InfoMsgData, BOOL bResult);
extern void InfoEmptyAck(INFO_DATA_T InfoMsgData);
extern void InfoSetPortID(INFO_DATA_T InfoMsgData);
extern void InfoAddMeterFiles(INFO_DATA_T InfoMsgData);
extern void InfoDeleteMeterFiles(INFO_DATA_T InfoMsgData);
extern void InfoClearMeterFiles(INFO_DATA_T InfoMsgData);
extern void InfoCurrentFilesNum(INFO_DATA_T InfoMsgData);
extern void InfoQueryFilesDetail(INFO_DATA_T InfoMsgData);
extern void InfoRouterDeal(INFO_DATA_T InfoMsgData);
extern void InfoActiveMeter(INFO_DATA_T InfoMsgData);
extern void InfoSearchMeter(INFO_DATA_T InfoMsgData);
extern void InfoPauseSearchMeter(INFO_DATA_T InfoMsgData);
extern void InfoDataTransmit(INFO_DATA_T InfoMsgData);
extern void InfoGetSlaveMonitorDelayTime(INFO_DATA_T InfoMsgData);
extern void InfoSlaveMonitor(INFO_DATA_T InfoMsgData);
extern void InfoSetTmnAddr(INFO_DATA_T InfoMsgData);
extern void InfoGetBroadcastDelayTime(INFO_DATA_T InfoMsgData);
extern void InfoBroadcastDeal(INFO_DATA_T InfoMsgData);
extern void InfoQueryHPLC(INFO_DATA_T InfoMsgData);
extern void InfoQueryHPLC(INFO_DATA_T InfoMsgData);
extern void InfoFrSet(INFO_DATA_T InfoMsgData);
extern void InfoFrQuery(INFO_DATA_T InfoMsgData);
extern void InfoAreaIdentify(INFO_DATA_T InfoMsgData);
extern void InfoOfflineFeel(INFO_DATA_T InfoMsgData);
extern void InfoAmrStateQuery(INFO_DATA_T InfoMsgData);
extern void InfoAmrModuleQuery(INFO_DATA_T InfoMsgData);
extern int8 Remove_RepeatPlcTask(PLCTASK_DETAIL      stPlcTaskDetail, uint8  nArrIndex);
extern int PlcTask_AddQueue(PLCTASK_DETAIL      stPlcTaskDetail, uint8 nArrIndex);
extern int8 ClearAmrList(uint8 nArrIndex);
extern int8 AddAmrIntoList(AMR_MODE_ACTIVE_T AmrModeActive, uint8 nArrIndex);
extern int8 GetAmrFromList(AMR_MODE_ACTIVE_T *AmrModeActive, uint8 nArrIndex);
extern int8 QueryAmrFromList(AMR_MODE_ACTIVE_T *AmrModeActive, uint8 nArrIndex);
extern int8 DeleteAmrFromList(uint8 nArrIndex);
extern uint16 GetAmrNumFromList(uint8 nArrIndex);
extern int8 AddDataTransmitList(uint8 nArrIndex, AMR_DATA_TRANSMIT_T AmrDataTransmit, INFO_DATA_HEAD_T InfoDataHead);
extern int8 GetDataTransmitFromList(uint8 nArrIndex, AMR_DATA_TRANSMIT_T *AmrDataTransmit, INFO_DATA_HEAD_T *InfoDataHead);
extern int8 AddSlaveMonitorTimeList(uint8 nArrIndex, AMR_SLAVE_NODE_DELAY_TIME_T AmrSlaveNodeMonitorTime, INFO_DATA_HEAD_T InfoDataHead);
extern int8 GetSlaveMonitorTimeFromList(uint8 nArrIndex, AMR_SLAVE_NODE_DELAY_TIME_T *AmrSlaveNodeMonitor, INFO_DATA_HEAD_T *InfoDataHead);
extern int8 AddSlaveMonitorList(uint8 nArrIndex, AMR_SLAVE_NODE_MONITOR_T AmrSlaveNodeMonitor, INFO_DATA_HEAD_T InfoDataHead);
extern int8 GetSlaveMonitorFromList(uint8 nArrIndex, AMR_SLAVE_NODE_MONITOR_T *AmrSlaveNodeMonitor, INFO_DATA_HEAD_T *InfoDataHead);
extern int8 AddInfoDataIntoList(INFO_DATA_T RecvInfoData);
extern int8 GetInfoDataFromList(INFO_DATA_T *SendInfoData);
extern int8 CreateTaskByInfo(void);
extern int InstantReportTaskDeal(uint8 *buf, int32 buf_len, uint8 nArrIndex);
extern void AmrInfoInit(uint8 nArrIndex);
extern void PastTimeDealInit(uint8 nArrIndex);
extern void AllStateInit(uint8 nArrIndex);
extern void AllFlagInit(uint8 nArrIndex);
int ModuleCheck(uint8 index);
int PlcResetRouter(uint8 index);
extern void TimeManage(uint8 nArrIndex);
extern void PlcTaskCreat(uint8    nArrIndex);
extern int CombineTaskFunInput(const PLCTASK_DETAIL stPlcTaskDetail, TASK_FUN_INPUT *stTaskFunInput);
extern void PlcTaskRunRouterDeal(uint8 nArrIndex);
extern void PlcTaskRun(uint8    nArrIndex);
extern void PlcTaskRunSearchMeter(uint8 nArrIndex);
extern int GetHeadInfo(INFO_DATA_T InfoMsgData, MSG_INFO_T *ptMsgInfo);
extern void ModuleInfoDeal(uint8 ModuleIndex, uint8 *RecvBuf, uint32 RecvLen);
extern int GetModuleInfo(uint8 ModuleIndex);
extern int GetAllModuleInfo();
extern int UpdateModuleFiles();
extern int UpdateModuleInfo();
extern int GlobalInit();
extern void ModuleEventFun(uint8* buf, uint16 len);
extern int ModuleEventDeal();

#ifdef __cplusplus
}
#endif

#endif // __CCO_COMMON_H__
