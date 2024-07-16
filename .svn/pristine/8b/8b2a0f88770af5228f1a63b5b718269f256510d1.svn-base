/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief： 运行日志和调试信息打印
* @date： 2019-10-25
*********************************************************************
*/

#include "CcoDebug.h"

#include "CcoCommon.h"
#include "CcoFiles.h"
#include "CcoLib.h"
#include "CcoPrtl.h"
#include "CcoTTL.h"

/*******************************************************************************
* 调节日志路径
*******************************************************************************/
//main函数和上行线程日志文件
char       *gUpRecordPath = "cco_up";   //main函数和上行线程日志文件

//调节日志路径
char       *gDownRecordPath[MAX_ALLOWED_DOWN_THREAD_NUM] = {"cco_down", "cco_down2"};  //调节日志路径

//显示GUI线程日志文件
char       *gGuiRecordPath = "cco_gui";   //显示GUI线程日志文件

//打印buff
void PrintBuf(uint8 *buf, int len)
{
    int i;

    for (i=0; i < len; i++)
    {
        printx("%02x ", buf[i]);
    }
    
    if (len > 0)
    {
        printx("\n");
    }
}

//打印任务详细信息
void PrintTaskDetail(PLCTASK_DETAIL PlcTaskDetail, uint8 nArrIndex)
{
	CCODOWN_FMT_DEBUG(nArrIndex, "任务名:%d, 任务名:%s, 任务端口:%d, 任务有效性:%d, 任务优先级:%d, 任务路由操作:%d \n", (int)PlcTaskDetail.TaskName,
		PlcTaskDetail.strTaskName, (int)PlcTaskDetail.TaskArrIndex, (int)PlcTaskDetail.TaskStat, (int)PlcTaskDetail.Priority, (int)PlcTaskDetail.RoutrtCtrl);
}

//打印任务队列详细信息
void PrintTaskList(PLCTASK_QUEUE lstPlcTaskQueue, uint8 nArrIndex)
{
	CCODOWN_FMT_DEBUG(nArrIndex, "刷新任务队列PLCTASK_QUEUE...begin \n");
    PLCTASK_QUEUE p=lstPlcTaskQueue->next;
    while(p)
    {
        PrintTaskDetail(p->stPlcTaskDetail, nArrIndex);
        p=p->next;
    }
	CCODOWN_FMT_DEBUG(nArrIndex, "刷新任务队列PLCTASK_QUEUE...end \n");
}


//打印档案详细信息
void PrintFilesDetail(AMR_METER_FILES_INFO_T AmrMeterFilesInfo, uint8 nArrIndex)
{
    uint32  nPortID;   //端口号
    //uint16  nMeterFilesID; //档案序号
    uint8   aMeterFilesAddr[6];  //档案地址
    uint8   aMeterFilesMac[6];  //档案MAC
    uint8   nProtocolType;  //协议类型
    //uint8   nCommPara;  //通信参数

    nPortID = AmrMeterFilesInfo.nPortID;
    //nMeterFilesID = AmrMeterFilesInfo.stAmrMeterFiles.nMeterFilesID;
    memcpy(aMeterFilesAddr, AmrMeterFilesInfo.stAmrMeterFiles.aMeterFilesAddr, sizeof(aMeterFilesAddr));
    memcpy(aMeterFilesMac, AmrMeterFilesInfo.stAmrMeterFiles.aMeterFilesMac, sizeof(aMeterFilesMac));
    nProtocolType = AmrMeterFilesInfo.stAmrMeterFiles.nProtocolType;
    //nCommPara = AmrMeterFilesInfo.stAmrMeterFiles.nCommPara;

    if(nArrIndex == 0xaa)//对上
    {
        CCOUP_FMT_DEBUG("档案信息: 端口号:%08x, 档案地址:%02x %02x %02x %02x %02x %02x, 档案MAC:%02x %02x %02x %02x %02x %02x, 协议类型:%d. \n", 
            nPortID, aMeterFilesAddr[0], aMeterFilesAddr[1],aMeterFilesAddr[2],aMeterFilesAddr[3],
            aMeterFilesAddr[4],aMeterFilesAddr[5], aMeterFilesMac[0],aMeterFilesMac[1],aMeterFilesMac[2],aMeterFilesMac[3],
            aMeterFilesMac[4],aMeterFilesMac[5],nProtocolType);
    }
    else if(nArrIndex <= MAX_ALLOWED_DOWN_THREAD_NUM)
    {
        CCODOWN_FMT_DEBUG(nArrIndex, "档案信息: 端口号:%08x, 档案地址:%02x %02x %02x %02x %02x %02x, 档案MAC:%02x %02x %02x %02x %02x %02x, 协议类型:%d. \n", 
            nPortID, aMeterFilesAddr[0], aMeterFilesAddr[1],aMeterFilesAddr[2],aMeterFilesAddr[3],
            aMeterFilesAddr[4],aMeterFilesAddr[5], aMeterFilesMac[0],aMeterFilesMac[1],aMeterFilesMac[2],aMeterFilesMac[3],
            aMeterFilesMac[4],aMeterFilesMac[5],nProtocolType);
    }
    
}


//打印档案全局列表信息
void PrintFilesList(AMR_METER_FILES_LIST_T AmrMeterFilesList, uint8 nArrIndex)
{
	if(nArrIndex == 0xaa)//对上
	{
		CCOUP_FMT_DEBUG("刷新档案列表 AMR_METER_FILES_LIST_T...begin \n");
		int i;
		for(i=0;i<AmrMeterFilesList.nMeterFilesNum;i++)
		        PrintFilesDetail(AmrMeterFilesList.stMeterFilesData[i], nArrIndex);
		CCOUP_FMT_DEBUG("刷新档案列表 AMR_METER_FILES_LIST_T...end \n");
	}
	else if(nArrIndex <= MAX_ALLOWED_DOWN_THREAD_NUM)//对下
	{
		CCODOWN_FMT_DEBUG(nArrIndex, "刷新档案列表 AMR_METER_FILES_LIST_T...begin \n");
		int i;
		for(i=0;i<AmrMeterFilesList.nMeterFilesNum;i++)
		        PrintFilesDetail(AmrMeterFilesList.stMeterFilesData[i], nArrIndex);
		CCODOWN_FMT_DEBUG(nArrIndex, "刷新档案列表 AMR_METER_FILES_LIST_T...end \n");
	}
}


//打印主动抄表详细信息
void PrintAmrActiveDetail(AMR_MODE_ACTIVE_T AmrModeActive)
{
    uint32   nPortID;  //端口号
    //uint16   nMeterFilesID; //档案序号
    CCO_COM_DCB_T comDcb;  //通信控制字
    uint8    nMeterAddr[6]; //表计地址
    uint16   nDataBufLen;  //报文长度
    uint8    aDataBuf[MAX_MSG_PAYLOAD_LEN]; //报文内容

    nPortID = AmrModeActive.nPortID;
    //nMeterFilesID = AmrModeActive.nMeterFilesID;
    comDcb = AmrModeActive.comDcb;
    memcpy(nMeterAddr, AmrModeActive.nMeterAddr, sizeof(nMeterAddr));
    nDataBufLen = AmrModeActive.nDataBufLen;
    memcpy(aDataBuf, AmrModeActive.aDataBuf, nDataBufLen);
    CCOUP_FMT_DEBUG("主动抄表信息: 端口号:%08x, 波特率:%u, 控制位:%u, 表计地址:%02x %02x %02x %02x %02x %02x, 报文长度:%d, 抄表报文内容: \n", 
        nPortID, comDcb.baud, comDcb.ctrl, nMeterAddr[0], nMeterAddr[1],nMeterAddr[2],nMeterAddr[3],
        nMeterAddr[4],nMeterAddr[5], nDataBufLen);
    CCOUP_BUF_DEBUG(aDataBuf, nDataBufLen, "报文:");
}

//打印启动搜表详细信息
void PrintSearchMeterDetail(AMR_METER_START_SEARCH_T       AmrMeterStartSearch)
{
	uint32	 nPortID;  //端口号
	int32	 nStartTime;  //开始时间
	int32	 nContinuedTime;  //持续时间
	uint8	nSlaveNodeReSendCount;	//从节点重发次数
	uint8	nRandWaitTimeSlice;  //随机等待时间片个数

	nPortID = AmrMeterStartSearch.nPortID;
	nStartTime = AmrMeterStartSearch.nStartTime;
	nContinuedTime = AmrMeterStartSearch.nContinuedTime;
	nSlaveNodeReSendCount = AmrMeterStartSearch.nSlaveNodeReSendCount;
	nRandWaitTimeSlice = AmrMeterStartSearch.nRandWaitTimeSlice;
	
	CCOUP_FMT_DEBUG(" 启动搜表信息: 端口号:%08x, 开始时间:%d, 持续时间:%d, 从节点重发次数:%d, 随机等待时间片个数:%d \n", 
		nPortID, nStartTime, nContinuedTime, nSlaveNodeReSendCount, nRandWaitTimeSlice);
}

//打印完整消息（head+msg)
void PrintInfoDetail(MSG_INFO_T *stMsgInfoSend)
{
	uint32 IP;        //目的端ip
    uint16 port;
	
	uint8  MsgRPM;         /* 启动标志位, 1-启动, 0-非启动 */
    uint8  MsgPRIORITY;    /* 优先级 */
    uint16 MsgIndex;       /* 消息序号 */
    uint16 MsgLabel;       /* 消息标签 */

    uint8  SourLen;
    uint8  DestLen;
    char   SourAddr[128] = {0};
    char   DestAddr[128] = {0};

    uint16 IID;
    uint16 IOP;

    uint32 MsgLen; 
    uint8  MsgData[65535] = {0};

	IP = stMsgInfoSend->IP;
	port = stMsgInfoSend->port;

	MsgRPM = stMsgInfoSend->MsgRPM;
	MsgPRIORITY = stMsgInfoSend->MsgPRIORITY;
	MsgIndex = stMsgInfoSend->MsgIndex;
	MsgLabel = stMsgInfoSend->MsgLabel;
	SourLen = stMsgInfoSend->SourLen;
	DestLen = stMsgInfoSend->DestLen;
	memcpy(SourAddr, stMsgInfoSend->SourAddr, SourLen);
	memcpy(DestAddr, stMsgInfoSend->DestAddr, DestLen);
	IID = stMsgInfoSend->IID;
	IOP = stMsgInfoSend->IOP;
	MsgLen = stMsgInfoSend->MsgLen;
	memcpy(MsgData, stMsgInfoSend->MsgData, MsgLen);

	CCOUP_FMT_DEBUG("消息的完整内容为: IP:%x, port:%d, MsgRPM:%d, MsgPRIORITY:%d, MsgIndex:%d, MsgLabel:%d, SourLen:%d, DestLen:%d, IID:%04x, IOP:%04x, MsgLen:%d. \n", 
		IP, port, MsgRPM, MsgPRIORITY,MsgIndex, MsgLabel, SourLen, DestLen, IID, IOP, MsgLen);
	CCOUP_FMT_DEBUG("SourAddr: %s\n", SourAddr);
	CCOUP_BUF_DEBUG((uint8*)SourAddr, SourLen, "SourAddr:");
	CCOUP_FMT_DEBUG("DestAddr: %s\n", DestAddr);
	CCOUP_BUF_DEBUG((uint8*)DestAddr, DestLen, "DestAddr:");
	CCOUP_FMT_DEBUG("MsgData: \n");
	CCOUP_BUF_DEBUG((uint8*)MsgData, MsgLen, "报文:");
	CCOUP_FMT_DEBUG("消息的完整内容打印完毕. \n");
}

//报文buff转换为string打印，没有空格
void ComposeBuf(char *buf, int len, char *comm_log)
{
  	int i;
  	char *p;
  
  	p = comm_log;
  
  	if (len < 1500)
  	{
    	for (i=0; i<len; i++)
      	{	
      		sprintf(p+2*i, "%02x", buf[i]);   
    	}
  	}
  	else
  	{
    	for (i=0; i<1500; i++)
      	{	
      		sprintf(p+2*i, "%02x", buf[i]);
    	}
  	}
}

//报文buff转换为string打印，添加空格
void CharArrToString(char *buf, int len, int maxlen, char *comm_log)
{
  	int i;
  	char *p;
  
  	p = comm_log;
	
  	if(len <= 0)
  	{
		return;
	}
        
  	if (len < maxlen)
  	{
    	for (i=0; i<len; i++)
      	{	
      		sprintf(p+3*i, "%02x ", buf[i]);
    	}
  	}
  	else
  	{
    	for (i=0; i<maxlen; i++)
      	{	
      		sprintf(p+3*i, "%02x ", buf[i]);
    	}
  	}
}


//将完整消息内容存入日志（head+msg)
void PrintLogInfoDetail(MSG_INFO_T *stMsgInfoSend)
{
	uint32 IP;        //目的端ip
    uint16 port;
	
	uint8  MsgRPM;         /* 启动标志位, 1-启动, 0-非启动 */
    uint8  MsgPRIORITY;    /* 优先级 */
    uint16 MsgIndex;       /* 消息序号 */
    uint16 MsgLabel;       /* 消息标签 */

    uint8  SourLen;
    uint8  DestLen;
    char   SourAddr[128] = {0};
    char   DestAddr[128] = {0};

    uint16 IID;
    uint16 IOP;

    uint32 MsgLen; 
    uint8  MsgData[65535] = {0};

	int  nMaxLen = 2000;
	char aCommLog[65535] = {0};

	IP = stMsgInfoSend->IP;
	port = stMsgInfoSend->port;

	MsgRPM = stMsgInfoSend->MsgRPM;
	MsgPRIORITY = stMsgInfoSend->MsgPRIORITY;
	MsgIndex = stMsgInfoSend->MsgIndex;
	MsgLabel = stMsgInfoSend->MsgLabel;
	SourLen = stMsgInfoSend->SourLen;
	DestLen = stMsgInfoSend->DestLen;
	memcpy(SourAddr, stMsgInfoSend->SourAddr, SourLen);
	memcpy(DestAddr, stMsgInfoSend->DestAddr, DestLen);
	IID = stMsgInfoSend->IID;
	IOP = stMsgInfoSend->IOP;
	MsgLen = stMsgInfoSend->MsgLen;
	memcpy(MsgData, stMsgInfoSend->MsgData, MsgLen);
	
	writelog(gUpRecordPath, " 消息的完整内容为: IP:%x, port:%d, MsgRPM:%d, MsgPRIORITY:%d, MsgIndex:%d, MsgLabel:%d, SourLen:%d, DestLen:%d, IID:%04x, IOP:%04x, MsgLen:%d.", 
		IP, port, MsgRPM, MsgPRIORITY,MsgIndex, MsgLabel, SourLen, DestLen, IID, IOP, MsgLen);
	
	memset(aCommLog, 0, sizeof(aCommLog));
	CharArrToString(SourAddr, SourLen, nMaxLen, aCommLog);
	writelog(gUpRecordPath, " SourLen:%d, SourAddr:%s", SourLen, aCommLog);
	
	memset(aCommLog, 0, sizeof(aCommLog));
	CharArrToString(DestAddr, DestLen, nMaxLen, aCommLog);
	writelog(gUpRecordPath, " DestLen:%d, DestAddr:%s", DestLen, aCommLog);
	
	memset(aCommLog, 0, sizeof(aCommLog));
	CharArrToString((char*)MsgData, MsgLen, nMaxLen, aCommLog);
	writelog(gUpRecordPath, " MsgLen:%d, MsgData:%s", MsgLen, aCommLog);
	
	writelog(gUpRecordPath, " 消息的完整内容打印完毕. \n\n");
}


//将串口收发报文保存日志
void PrintLogBuffDetail(uint8 *buf, int len, uint8 nArrIndex)
{
	int  nMaxLen = 2000;
	char aCommLog[65535] = {0};

	memset(aCommLog, 0, sizeof(aCommLog));
	CharArrToString((char*)buf, len, nMaxLen, aCommLog);
	writelog(gDownRecordPath[nArrIndex], " len:%d, buf:%s \n\n", len, aCommLog);
}

//打印当前时间
void PrintCurrentTime(DateTime_T *dtbcd)
{
	printx("\n ******当前时间 YY-MM-DD  hh:nn:ss = %02x-%02x-%02x  %02x:%02x:%02x****** \n\n", 
		dtbcd->Year, dtbcd->Month, dtbcd->Day, 
		dtbcd->Hour, dtbcd->Minute, dtbcd->Second);
}




