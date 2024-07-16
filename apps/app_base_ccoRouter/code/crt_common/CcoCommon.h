/*
*********************************************************************
* Copyright(C) 2019 �Ͼ��������ӹɷ����޹�˾
* All rights reserved. 
* @brief�� ��������
* @date�� 2019-10-25
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

extern uint8 gNullArr[100];//������, ���ڱȶ�

/*******************************************************************************
* ���ض���ͨ�� UDPͨ�Ų���
*******************************************************************************/
extern char       *gCcoRouterAppName;
extern uint16      gCcoRouterAppPort;
extern char       *gCcoRouterAppIP;

/*******************************************************************************
* ���ȹ���APP UDPͨ�Ų���
*******************************************************************************/

//���ȹ���APP CTS_ROUTER_APP_NAME
extern char       *gCtsRouterAppName[MAX_ALLOWED_CTS_THREAD_NUM];  //���ȹ���APP CTS_ROUTER_APP_NAME

//���ȹ���APP CTS_ROUTER_APP_PORT
extern uint16      gCtsRouterAppPort[MAX_ALLOWED_CTS_THREAD_NUM];  //���ȹ���APP CTS_ROUTER_APP_PORT

//���ȹ���APP CTS_ROUTER_APP_IP
extern char       *gCtsRouterAppIP[MAX_ALLOWED_CTS_THREAD_NUM];  //���ȹ���APP CTS_ROUTER_APP_IP

/*******************************************************************************
* ���������ز�ģ�����
*******************************************************************************/

//�˿ں�
extern uint32      gPortID[MAX_ALLOWED_DOWN_THREAD_NUM];  //�����˿ں�
extern BOOL        bSetPortIDFlag[MAX_ALLOWED_DOWN_THREAD_NUM];  //�Ƿ��յ��˿ں����õı�־

//ͨ��ģ���豸����
extern char       *gDevName[MAX_ALLOWED_DOWN_THREAD_NUM];  //����ͨ���豸(��о)
extern char       *gDingxinDevName[MAX_ALLOWED_DOWN_THREAD_NUM]; //����ͨ���豸(����)
extern DEV_HAL_INFO gDevHalInfo[MAX_ALLOWED_DOWN_THREAD_NUM];

/*******************************************************************************
* ˽���ļ�·��
*******************************************************************************/

//����˽���ļ���
extern char       *gLocalFilesName[MAX_ALLOWED_DOWN_THREAD_NUM];  //��������˽���ļ���

//ͨ��ģ���ַ˽���ļ���
extern char       *gModuleManuName[MAX_ALLOWED_DOWN_THREAD_NUM];  //����˽���ļ�

//�˿ں���Ϣ˽���ļ���
extern char       *gPortIDName[MAX_ALLOWED_DOWN_THREAD_NUM];  //����˽���ļ�

//��ʾ�����Ϣ˽���ļ���
extern char       *gGuiInfoName[MAX_ALLOWED_DOWN_THREAD_NUM];  //����˽���ļ�

/*******************************************************************************
* �ն˵�ַ
*******************************************************************************/
extern uint8       gTerminalAddr[MAX_ALLOWED_DOWN_THREAD_NUM][6];  //�ն˵�ַ
extern BOOL        gUpdateTerminalAddrFlag[MAX_ALLOWED_DOWN_THREAD_NUM];  //�����ն˵�ַ��־

/*******************************************************************************
* ��������
*******************************************************************************/
extern PastTimeFlag_T             gPastTimeFlag[MAX_DOWN_THREAD_NUM];                           //��ʱ���־
extern DateTime_T                 gCurBinTime[MAX_DOWN_THREAD_NUM];                             //��ǰBINʱ��
extern DateTime_T                 gCurBcdTime[MAX_DOWN_THREAD_NUM];                             //��ǰBCDʱ��
extern DateTime_T                 gPreBinTime[MAX_DOWN_THREAD_NUM];                             //�ϴ�BINʱ��
extern DateTime_T                 gCurBcdTimeUp;  //��ǰBCDʱ��(��¼������Ϣ)
extern DateTime_T                 gCurBcdTimeDown[MAX_DOWN_THREAD_NUM];  //��ǰBCDʱ��(��¼������Ϣ)

//�ϵ��־
extern int                        gPowerOnFlag[MAX_DOWN_THREAD_NUM];                            //�ϵ��־

//�ϵ��ȡģ����Ϣ
extern ModuleManuID_T             gModuleManuID[MAX_DOWN_THREAD_NUM];               //ģ�鳧�̱�ʶ
extern LOCAL_COMM_MODULE_RUNNING_PATTERN_INFO  gLocalCommModuleInfo[MAX_DOWN_THREAD_NUM];         //ģ����Ϣ��ȫ�ֱ���

//�����б�ȫ�ֱ���
extern AMR_METER_FILES_LIST_T gAmrMeterFilesListTemp[MAX_DOWN_THREAD_NUM];  //�����б���ʱ����(��Ϣ������ʱ����)
extern AMR_METER_FILES_LIST_T gAmrMeterFilesList[MAX_DOWN_THREAD_NUM];  //�����б�ȫ�ֱ���
extern AMR_METER_FILES_LIST_T gToAddFilesList[MAX_DOWN_THREAD_NUM];  //��Ҫ��ӵĵ����б�ȫ�ֱ���
extern AMR_METER_FILES_LIST_T gToDeleteFilesList[MAX_DOWN_THREAD_NUM];  //��Ҫɾ���ĵ����б�ȫ�ֱ���

extern uint8                     gMeterFileAddDelFlagTemp[MAX_DOWN_THREAD_NUM];       //��������ɾ����־(��Ϣ������ʱ����)
extern uint8                     gMeterFileClearFlagTemp[MAX_DOWN_THREAD_NUM];        //���������־(��Ϣ������ʱ����)
extern uint8                     gMeterFileAddDelFlag[MAX_DOWN_THREAD_NUM];       //��������ɾ����־
extern uint8                     gMeterFileClearFlag[MAX_DOWN_THREAD_NUM];        //���������־
extern uint8                     gMeterFileCompareFlag[MAX_DOWN_THREAD_NUM];      //�����ȶԱ�־
extern uint8                     gMeterFileModifyFlag[MAX_DOWN_THREAD_NUM];       //�����޸ı�־
extern uint8                     gMeterFileModifyState[MAX_DOWN_THREAD_NUM];      //�����޸�״̬��0�������꣬1��������
extern METER_FILES_MODEFY_TYPE_E gMeterFilesModifyType[MAX_DOWN_THREAD_NUM];      //�����޸ķ�ʽ

extern AMR_ACTIVE_INFO_LIST_T gActiveAmrInfoList[MAX_DOWN_THREAD_NUM];  //��ų�����Ϣ�Ļ�����

//�ز��������ʹ�õ�ȫ�ֱ���
extern PLCTASK_QUEUE              PlcTaskQueue[MAX_DOWN_THREAD_NUM];            //�ز��������
extern PLCTASK_DETAIL             gPlcTaskDetail[MAX_DOWN_THREAD_NUM];                 //��ǰ�ز���������ϸ��
extern PLCTASK_DETAIL             gPlcTaskDetailSearchMeter[MAX_DOWN_THREAD_NUM];  //��ǰ�ز���������ϸ��(�ѱ�)

//�����������ڲ�ʹ�õ�ȫ�ֱ���
extern TOTAL_STATE_T              gTotalState[MAX_DOWN_THREAD_NUM];                             //��״̬��

//���������Ϣ
extern AmInfo_T    gAmrInfo[MAX_DOWN_THREAD_NUM];			                //���������Ϣ


//�յ�����Ϣ������
extern INFO_DATA_LIST_T gInfoDataList;                      //�յ�����Ϣ�������б�

//�����ϱ���ϢINDEX
extern uint16  ActiveInfoIndex;                                      //�����ϱ���ϢINDEX

//���񼯺�
extern const PLCTASK_FUNINFO PlcTask_FunList[];

//plc��λ���ź�ģ��������ż���ļ�������
extern int fd_plc[MAX_DOWN_THREAD_NUM];//plc��λ���ź�ģ��������ż���ļ�������

//����ճ��������
extern GLOBAL_SEQ_INFO_T gGlobalSeqInfo[MAX_DOWN_THREAD_NUM];

//·�ɿ��Ʋ���
extern ROUTER_CTRL_STATE_T   gRoutrtCtrl[MAX_DOWN_THREAD_NUM];           //·�ɿ��Ʋ���

//·�ɿ��Ʋ���
extern ROUTER_OPER_DETAIL_T  gRouterOper[MAX_DOWN_THREAD_NUM];           //·�ɿ��Ʋ���

//�����ѱ��־
extern WHETHER_START_SEARCH_METER_T gWhetherStartSearchMeter[MAX_DOWN_THREAD_NUM];       //�����ѱ��־

//��ͣ�ѱ��־
extern BOOL gPauseSearchMeterFlag[MAX_DOWN_THREAD_NUM];       //��ͣ�ѱ��־

//�ѱ������(��0��ʼ)
extern uint16 gSearchMeterFilesIndex[MAX_DOWN_THREAD_NUM];

//��͸�������
extern AMR_DATA_TRANSMIT_LIST_T gDataTransmitQueue[MAX_DOWN_THREAD_NUM];

//�ӽڵ����������
extern AMR_SLAVE_MONITOR_LIST_T gSlaveMonitorQueue[MAX_DOWN_THREAD_NUM];

//�Ƿ��ȡ�㲥��ʱ
extern WHETHER_GET_BROADCAST_DELAY_T gWhetherGetBroadcastDelay[MAX_DOWN_THREAD_NUM];

//�Ƿ�㲥
extern WHETHER_DO_BROADCAST_T gWhetherDoBroadcast[MAX_DOWN_THREAD_NUM];

//��ѯHPLC��Ϣ ��Ϣ����
HPLC_TYPE_QUERY_UN gQueryTypeHPLC[MAX_DOWN_THREAD_NUM];  //ÿ��bit��Ӧ��ͬ��HPLC��ѯ����

//5��ģ�����Ϣ
extern BOOL gQueryAllModuleFlag;  //���������ʱ, ��ȡ����ģ����Ϣ
extern MODULE_INFO_T gModuleInfo[5];  //5��ģ�����Ϣ
extern uint8 gReloadModuleFlag[6];  //�Ƿ���Ҫ���´�ģ�����������ģ����Ϣ

//�������һ���Ƿ�CCOģ����
extern uint8 gCcoFlag[5];  //�������һ���Ƿ�CCOģ����

//HPLCƵ�����ñ�־
extern uint8 gFrSetFlag[MAX_DOWN_THREAD_NUM];  //HPLCƵ�����ñ�־
//HPLCƵ������ֵ
extern uint8 gFrSetValue[MAX_DOWN_THREAD_NUM];  //HPLCƵ������ֵ

//HPLCƵ���ѯ��־
extern uint8 gFrQueryFlag[MAX_DOWN_THREAD_NUM];  //HPLCƵ���ѯ��־
extern INFO_DATA_HEAD_T gFrQueryInfoDataHead[MAX_DOWN_THREAD_NUM];  //HPLCƵ���ѯ��ϢԭʼHEAD
extern uint8 gInitFrQueryFlag[MAX_DOWN_THREAD_NUM];  //HPLCƵ���ѯ��־(�ϵ��ʼ����ѯ)

//��ѯ����״̬�ṹ��
extern AMR_QUERY_READING_STATE_T gAmrStateQuery[MAX_DOWN_THREAD_NUM];  //��ѯ����״̬�ṹ��

//��ѯ����ģ����Ϣ�ṹ��
extern AMR_QUERY_MODULE_INFO_T gAmrModuleQuery[MAX_DOWN_THREAD_NUM];  //��ѯ����ģ����Ϣ�ṹ��

//HPLC̨��ʶ���־
extern uint8 gAreaIdentifyFlag[MAX_DOWN_THREAD_NUM];  //HPLC̨��ʶ���־
//HPLC̨��ʶ��ֵ
extern uint8 gAreaIdentifyValue[MAX_DOWN_THREAD_NUM];  //HPLC̨��ʶ��ֵ

//HPLC������֪��־
extern uint8 gOfflineFeelFlag[MAX_DOWN_THREAD_NUM];  //HPLC������֪��־
//HPLC������ֵ֪
extern uint8 gOfflineFeelValue[MAX_DOWN_THREAD_NUM];  //HPLC������ֵ֪

//HPLC�ܾ��ڵ��ϱ���־
extern uint8 gRefuseNodeReportFlag[MAX_DOWN_THREAD_NUM];  //HPLC�ܾ��ڵ��ϱ���־
//HPLC�ܾ��ڵ��ϱ�����
extern uint8 gRefuseNodeReportValue[MAX_DOWN_THREAD_NUM];  //HPLC�ܾ��ڵ��ϱ�����

//ģ�������»�ȡ�豸
extern uint8 gReloadDevFlag[MAX_DOWN_THREAD_NUM];       //ģ�������»�ȡ�豸

//guiչʾ����
extern GUI_SHOW_MESSAGE_T gGuiShowMessage[MAX_DOWN_THREAD_NUM];  //guiչʾ����

//�����ļ���ʶȫ�ֱ��
extern uint8 gPlcUpdateFileMark[MAX_DOWN_THREAD_NUM];  //�����ļ���ʶȫ�ֱ��. 0x03:���ڵ������ļ�; 0x07:���ڵ�+ȫ���ӽڵ�;0x08:ȫ���ӽڵ�

//CCO���������ļ�·��
extern char *gRouterUpdateFile[MAX_ALLOWED_DOWN_THREAD_NUM];  //���ڵ�������
extern char *gRouterUpdateFileBack[MAX_ALLOWED_DOWN_THREAD_NUM];  //���ڵ�������
extern char *gAllRouterUpdateFile[MAX_ALLOWED_DOWN_THREAD_NUM];  //���ڵ�+ȫ���ӽڵ�����
extern char *gAllRouterUpdateFileBack[MAX_ALLOWED_DOWN_THREAD_NUM];  //���ڵ�+ȫ���ӽڵ�����
extern char *gSubRouterUpdateFile[MAX_ALLOWED_DOWN_THREAD_NUM];  //ȫ���ӽڵ�����
extern char *gSubRouterUpdateFileBack[MAX_ALLOWED_DOWN_THREAD_NUM];  //ȫ���ӽڵ�����

//��ѯ����ز�ģ��ڵ���Ϣ���ݵ�Ԫ��ʽ
extern CCO_STA_INFO_T gAllCcoStaInfo[MAX_DOWN_THREAD_NUM];

/*******************************************************************************
* ��������
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
