/*-***************************************************************
* Copyright (c) 2006,南京新联电子仪器有限公司
*            All rights reserved.
* 程序名称：EventIO.h
* 版本号：1.0
* 功能：对事件检测过程中用到的数据结构，调度函数进行声明
* 原始开发人及时间：董宁 2006年12月25日
* 修改人员、时间及修改内容:无
* 其他： 
******************************************************************-*/
#ifndef _EVENTIO_H
#define _EVENTIO_H
typedef struct 
{
	char MetAddr[6];//电表地址
	uint8 SignalQuality;//通讯品质
	uint8 prtl;//规约类型
}newmeterfind_info;
typedef struct 
{
	uint8 metcomm_no;//电表通讯端口号
	uint8 meter_num;//新发现电表的数量
}totalmeter_info;

typedef struct 
{
	uint8			choice;		//类型标识	
	union
	{
		OOP_DAR_E	dar;		//错误信息
		struct
        {
        	uint8		nNum;			//关联对象属性个数		
        	uint8		databuf[1024];	//数据 
        }RecodeData;
	};
}ResponseData;


typedef struct 
{
	OOP_PIID_U      piid;       //执行序号和优先级
	OOP_TSA_T       tsa;		//地址
	OOP_OAD_U       oad;        //事件标识
	OOP_RCSD_T      eventRCSD;  //事件上报RCSD
	ResponseData    data;
}ProxyGetResponseRecord;

BOOLEAN GetTnmEventReport();
void InitLatestMeterReadDataList(int clientid);
void CheckERC_3109(int fd, uint8* wPW,uint32 PWlen);
void CheckERC_3111(int fd);
void CheckERC_3114(int fd, OOP_DATETIME_S_T startTm, OOP_DATETIME_S_T endTm);
void CheckERC_3118(int fd, uint8 flag,OOP_OAD_U oad);
void CheckERC_3140(int fd, uint32 msg_id, uint8 *pmsg, uint16 msglen);
void CheckERC_3117(int fd, OOP_SWITCHOUT_EVENT_T switchOut);
void CheckERC_3115(int fd, OOP_OAD_U SourceOAD, uint8 rCtrlType, int64 llPower[], int64 prePower[], uint8 isUpdate);
void CheckERC_3200(int fd, OI SourceOI, OI objCtrl, OOP_BITSTR8_T turnState, int64 llSetting, int64 power2Min, int64 power, uint8 isUpdate);
void CheckERC_3201(int fd, OI SourceOI, OI objCtrl, OOP_BITSTR8_T turnState, int64 llSetting, int64 energy);
void CheckERC_3202(int fd, OI SourceOI, OOP_CFGUNIT_810C_T cfgunit);
void CheckERC_3203(int fd, OI SourceOI, OI objCtrl, int64 llSetting);
void  evt_check_360X(int clientid, uint8 evtSource, OOP_DATETIME_S_T data, uint8 warnType, uint8 bType, char *pContainerName, char *pAppName);
int advance_iot_evt_msg(int cid, uint16 label, uint8* pMsg, uint16 msglen, uint32* nextStep);

int RecvReadMeterStatus(uint8 *pmsg, uint16 msglen, uint16 label);
int GetEventNomalParam(int clientid, uint8 logicId, uint16 infoNum, uint32 value);
int UpdateLatestMeterReadDataList(int clientid, uint8 logicId, uint16 infoNum, uint32 oadMain, uint8 oadColsNum, uint32* oadCols);
void InitLatestMeterReadDataLock(void);
int puamr_evt_init_event_deal(int clientid, uint8         initType, uint8 logicId, uint16 classTag);
void GetMntAddr(OOP_OCTETVAR16_T tLocalAddr);
void VersionCheck(OOP_VERSION_T tVersion);
void InitEvent(void);		//-事件监测初始化-

uint8 get_tmn_halt();


void evt_report_state_update(uint8 *pevt_data, OOP_REPORTS_T state);
int evt_db_update(int fd, OOP_OAD_U evt_oad, uint32 storagenum, uint8 *pevt_data, uint32 data_len, OOP_REPORTS_T state);

int16 EventProc(uint8 *pOutBuf, int16 nOutBufLen, uint8 *seq);//-事件主动上报接口-
void EventHappenProc(void);//-事件生成接口-
int Pro_RelayHalt(uint8 *recvBuf, uint16 recvLen); //停上电事件透传


#endif

