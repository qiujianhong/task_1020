/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief： 功能描述
* @date： 2019-10-25
*********************************************************************
*/


#include "CcoFiles.h"

#include "CcoCommon.h"
#include "CcoDebug.h"
#include "CcoLib.h"
#include "CcoPrtl.h"
#include "CcoTTL.h"

/*******************************************************************************
* 函数名称:	
* 函数功能:	定时保存档案私有文件
* 输入参数:	
* 输出参数:	
* 返 回 值: 
*******************************************************************************/
void RegularWriteFilesToPData(uint8	  nArrIndex, uint16 nMinute)
{
	static int AccumulateWaitTime[MAX_DOWN_THREAD_NUM] = {0};             //累计等待时间
	if (gPastTimeFlag[nArrIndex].BitSect.Minute == 1)
	{
		AccumulateWaitTime[nArrIndex]++;
	}
	if(AccumulateWaitTime[nArrIndex] > nMinute)
	{
		WriteFilesToPData(nArrIndex);
		AccumulateWaitTime[nArrIndex] = 0;
	}
}



/*******************************************************************************
* 函数名称:	
* 函数功能:	档案发生变化时，修改私有文件
* 输入参数:	
* 输出参数:	
* 返 回 值: 
*******************************************************************************/
void WriteFilesToPData(uint8      nArrIndex)
{
	//将档案全局变量（或增、删、改的相关档案）写入档案私有文件
	//待完善
	int Ret = 0;

	uint8 nTtlIndex = 0;  //不同的端口号对应不同的数组下标
	nTtlIndex = nArrIndex;

	
	Ret = clear_pdata_xram(gLocalFilesName[nTtlIndex]);
	
	CCOUP_FMT_DEBUG("WriteFilesToPData() Ret = clear_pdata_xram() Ret:%d... \n", Ret);

	CCOUP_FMT_DEBUG("***************写私有类数据文件(write_pdata_xram)*************begin********** \n");
	PrintFilesList(gAmrMeterFilesListTemp[nTtlIndex], 0xaa);//打印档案列表详细信息
	CCOUP_FMT_DEBUG("filename:%s...offset:%d, len:%d... \n", gLocalFilesName[nTtlIndex], 0, sizeof(gAmrMeterFilesListTemp[nTtlIndex]));
	
	Ret = write_pdata_xram(gLocalFilesName[nTtlIndex], (char *)&gAmrMeterFilesListTemp[nTtlIndex], 0, sizeof(gAmrMeterFilesListTemp[nTtlIndex]));
	
	CCOUP_FMT_DEBUG("WriteFilesToPData() Ret = write_pdata_xram() Ret:%d... \n", Ret);
	CCOUP_FMT_DEBUG("***************写私有类数据文件(write_pdata_xram)*************end********** \n");
}

/*******************************************************************************
* 函数名称:	ReadFilesFromPData
* 函数功能:	将档案私有文件内容读取到档案列表全局变量
* 输入参数:	
* 输出参数:	
* 返 回 值: 
*******************************************************************************/
void ReadFilesFromPData(uint8 nArrIndex)
{
	//将档案私有文件内容，整体读取到档案列表全局变量
	//待完善
	int Ret = 0;
	
	uint8 nTtlIndex = 0;  //不同的端口号对应不同的数组下标
	nTtlIndex = nArrIndex;

	CCODOWN_FMT_DEBUG(nArrIndex, "***************读私有类数据文件(read_pdata_xram)***********begin************\n");
	PrintFilesList(gAmrMeterFilesListTemp[nTtlIndex], nTtlIndex);//打印档案列表详细信息
	CCODOWN_FMT_DEBUG(nArrIndex, "filename:%s...offset:%d, len:%d...\n", gLocalFilesName[nTtlIndex], 0, sizeof(gAmrMeterFilesListTemp[nTtlIndex]));
	
	memset((uint8 *)&gAmrMeterFilesListTemp[nTtlIndex], 0x00, sizeof(gAmrMeterFilesListTemp[nTtlIndex]));
	Ret = read_pdata_xram(gLocalFilesName[nTtlIndex], (char *)&gAmrMeterFilesListTemp[nTtlIndex], 0, sizeof(gAmrMeterFilesListTemp[nTtlIndex]));
	CCODOWN_FMT_DEBUG(nArrIndex, "ReadFilesFromPData() Ret = read_pdata_xram() Ret:%d...\n", Ret);
	
	PrintFilesList(gAmrMeterFilesListTemp[nTtlIndex], nTtlIndex);//打印档案列表详细信息
	CCODOWN_FMT_DEBUG(nArrIndex, "filename:%s...offset:%d, len:%d...\n", gLocalFilesName[nTtlIndex], 0, sizeof(gAmrMeterFilesListTemp[nTtlIndex]));
	CCODOWN_FMT_DEBUG(nArrIndex, "***************读私有类数据文件(read_pdata_xram)***********end************\n");
}


/*******************************************************************************
* 函数名称:	ReadAmrInfo
* 函数功能:	读抄表信息
* 输入参数:	无
* 输出参数:	无
* 返 回 值:	无
*******************************************************************************/
void ReadAmrInfo(uint8    nArrIndex)
{
    //printx(" void ReadAmrInfo(uint32 nPortID)...begin \n");
    
    int     nMeterFilesNum;
    int     i = 0;
    uint8   nTtlIndex = 0;  //不同的端口号对应不同的数组下标
    nTtlIndex = nArrIndex;
    
    //抄表汇总信息初始化
    memset(&gAmrInfo[nTtlIndex], 0x00, sizeof(AmInfo_T));
    
    nMeterFilesNum = gAmrMeterFilesList[nTtlIndex].nMeterFilesNum;  //档案数量

    //从测量点参数中读取信息，与结构体比对，如果结构体中无，则增之。以本地档案私有文件内容为准。
    for (i = 0; i < nMeterFilesNum; i++)
    {
        //电表统计数+1
        gAmrInfo[nTtlIndex].TmnAmrInfo.MtrCnt++;
        
        //保存测量点号
        gAmrInfo[nTtlIndex].MtrAmrInfo[i].Pn = i+1;
        
        gAmrInfo[nTtlIndex].MtrAmrInfo[i].nPortID = gAmrMeterFilesList[nTtlIndex].stMeterFilesData[i].nPortID;
        //gAmrInfo[nTtlIndex].MtrAmrInfo[i].nMeterFilesID = gAmrMeterFilesList[nTtlIndex].stMeterFilesData[i].stAmrMeterFiles.nMeterFilesID;
        memcpy(gAmrInfo[nTtlIndex].MtrAmrInfo[i].aMeterFilesAddr, gAmrMeterFilesList[nTtlIndex].stMeterFilesData[i].stAmrMeterFiles.aMeterFilesAddr, 6);
        memcpy(gAmrInfo[nTtlIndex].MtrAmrInfo[i].aMeterFilesMac, gAmrMeterFilesList[nTtlIndex].stMeterFilesData[i].stAmrMeterFiles.aMeterFilesMac, 6);
        gAmrInfo[nTtlIndex].MtrAmrInfo[i].nProtocolType = gAmrMeterFilesList[nTtlIndex].stMeterFilesData[i].stAmrMeterFiles.nProtocolType;
        //gAmrInfo[nTtlIndex].MtrAmrInfo[i].nCommPara = gAmrMeterFilesList[nTtlIndex].stMeterFilesData[i].stAmrMeterFiles.nCommPara;

        if (gAmrInfo[nTtlIndex].TmnAmrInfo.MtrCnt > MAX_AMR_COLLECT_CNT)
        {
            break;
        }
    }
    
}

/*******************************************************************************
* 函数名称:	IsValidMtrAddr
* 函数功能:	检测是否有效的测量点地址
* 输入参数:	MtrAddr             要检测的测量点地址
*           AcqUnitFlag         采集器标志
* 输出参数:	无
* 返 回 值:	>0                  有效
*           <0                  无效
*******************************************************************************/
int IsValidMtrAddr(uint8 *MtrAddr, uint8 AcqUnitFlag)
{
    uint8   TestAddr[6];

    //判断采集器地址是否是6个字节0xff
    memset(TestAddr, 0xff, 6);
    if (memcmp(MtrAddr, TestAddr, 6) == 0)
    {
        return -1;
    }

    memset(TestAddr, 0xEE, 6);
    if (memcmp(MtrAddr, TestAddr, 6) == 0)
    {
        return -1;
    }

	//common bug(2014-6-24-14-07-42)
    //判断采集器地址或电表地址是否是6个字节0x00
    memset(TestAddr, 0x00, 6);
    if (memcmp(MtrAddr, TestAddr, 6) == 0)
    {
        return -1;
    }

    return 1;
}


/*******************************************************************************
* 函数名称:	CheckRepeatMtrAddr
* 函数功能:	检测电表地址是否已经重复
* 输入参数:	无
* 输出参数:	无
* 返 回 值:	> 0 重复
            < 0 不重复
*******************************************************************************/
int CheckRepeatMtrAddr(char *MtrAddr, uint8 nArrIndex)
{
    uint16 	Pn;
    int 	i;
    uint8	CmpMtrAddr[6];

	uint8   nTtlIndex = 0;  //不同的端口号对应不同的数组下标
	nTtlIndex = nArrIndex;

	for(i = 0;i < gAmrInfo[nTtlIndex].TmnAmrInfo.MtrCnt;i++)
	{
	    Pn = gAmrInfo[nTtlIndex].MtrAmrInfo[i].Pn;
        ReadMtrAddr(CmpMtrAddr, Pn, nTtlIndex);

		if(memcmp(CmpMtrAddr,MtrAddr,6) == 0)
			return 1;

	}

	return -1;
}

/*******************************************************************************
* 函数名称:	GetPnByMeterAddr
* 函数功能:	通过电表地址获取测量点号
* 输入参数:	uint8 *MtrAddr 电表地址
* 输出参数:	无
* 返 回 值:	测量点号
*******************************************************************************/
uint16 GetPnByMeterAddr(uint8 *MtrAddr, uint8 nArrIndex)
{
    int i = 0;
    uint16 Pn = 0;
    uint8   TempMtrAddr[17] = {0};

	uint8   nTtlIndex = 0;  //不同的端口号对应不同的数组下标
	nTtlIndex = nArrIndex;

    for (i = 0; i < gAmrInfo[nTtlIndex].TmnAmrInfo.MtrCnt; i++)
    {
        Pn = gAmrInfo[nTtlIndex].MtrAmrInfo[i].Pn;
        ReadMtrAddr(TempMtrAddr, Pn, nTtlIndex);

        if (memcmp(MtrAddr, TempMtrAddr, 6) == 0)
        {
            return Pn;
        }
    }

    return 0;
}

/*******************************************************************************
* 函数名称:	ReadMtrType
* 函数功能:	从测量点参数中获取测量点类型
* 输入参数:	Pn                  侧脸点号
* 输出参数:	无
* 返 回 值:
*******************************************************************************/
MTR_TYPE_T ReadMtrType(uint16 Pn, uint8 nArrIndex)
{
    return METER_PLC;
}

/*******************************************************************************
* 函数名称:	ReadAcqUnitAddr
* 函数功能:	从测量点参数中获取采集器地址
* 输入参数:	Pn                  侧脸点号
* 输出参数:	DesData             地址
* 返 回 值:	< 0                 错误
*		    > 0                 长度
*******************************************************************************/
int ReadAcqUnitAddr(uint8 *DesData, uint16 Pn, uint8 nArrIndex)
{
    uint32  i = 0;
	uint16  nPnTemp;
	
	uint8   nTtlIndex = 0;  //不同的端口号对应不同的数组下标
	nTtlIndex = nArrIndex;

	for(i = 0;i < gAmrInfo[nTtlIndex].TmnAmrInfo.MtrCnt;i++)
	{
	    nPnTemp = gAmrInfo[nTtlIndex].MtrAmrInfo[i].Pn;
        if(Pn == nPnTemp)
        {
			memcpy(DesData, gAmrInfo[nTtlIndex].MtrAmrInfo[i].aMeterFilesAddr, 6);
			return 1;
		}

	}
	return 0;
}

/*******************************************************************************
* 函数名称:	ReadMtrAddr
* 函数功能:	从测量点参数中获取测量点地址
* 输入参数:	Pn                  侧脸点号
* 输出参数:	DesData             地址
* 返 回 值:	< 0                 错误
*		    > 0                 长度
*******************************************************************************/
int ReadMtrAddr(uint8 *DesData, uint16 Pn, uint8 nArrIndex)
{
	uint32  i = 0;
	uint16  nPnTemp;
	
	uint8   nTtlIndex = 0;  //不同的端口号对应不同的数组下标
	nTtlIndex = nArrIndex;

	for(i = 0;i < gAmrInfo[nTtlIndex].TmnAmrInfo.MtrCnt;i++)
	{
	    nPnTemp = gAmrInfo[nTtlIndex].MtrAmrInfo[i].Pn;
        if(Pn == nPnTemp)
        {
			memcpy(DesData, gAmrInfo[nTtlIndex].MtrAmrInfo[i].aMeterFilesAddr, 6);
			return 1;
		}

	}
	return 0;
}
#if 0
/*******************************************************************************
* 函数名称:	GetFilesIdByMeterAddr
* 函数功能:	通过电表地址获取档案序号
* 输入参数:	uint8 *MtrAddr 电表地址
* 输出参数:	无
* 返 回 值:	测量点号
*******************************************************************************/
uint16 GetFilesIdByMeterAddr(uint8 *MtrAddr, uint8 nArrIndex)
{
    int i = 0;

	uint8   nTtlIndex = 0;  //不同的端口号对应不同的数组下标
	nTtlIndex = nArrIndex;

    for (i = 0; i < gAmrInfo[nTtlIndex].TmnAmrInfo.MtrCnt; i++)
    {
        if (memcmp(MtrAddr, gAmrInfo[nTtlIndex].MtrAmrInfo[i].aMeterFilesAddr, 6) == 0)
        {
            return gAmrInfo[nTtlIndex].MtrAmrInfo[i].nMeterFilesID;
        }
    }

    return 0;
}
#endif
/*******************************************************************************
* 函数名称:	GetProtocolByMeterAddr
* 函数功能:	通过电表地址获取电表规约
* 输入参数:	uint8 *MtrAddr 电表地址
* 输出参数:	无
* 返 回 值:	测量点号
*******************************************************************************/
uint16 GetProtocolByMeterAddr(uint8 *MtrAddr, uint8 nArrIndex)
{
    int i = 0;

	uint8   nTtlIndex = 0;  //不同的端口号对应不同的数组下标
	nTtlIndex = nArrIndex;

    for (i = 0; i < gAmrInfo[nTtlIndex].TmnAmrInfo.MtrCnt; i++)
    {
        if (memcmp(MtrAddr, gAmrInfo[nTtlIndex].MtrAmrInfo[i].aMeterFilesAddr, 6) == 0)
        {
            return gAmrInfo[nTtlIndex].MtrAmrInfo[i].nProtocolType;
        }
    }

    return 0;
}



/*******************************************************************************
* 函数名称:	ReadMtrPrtlMode
* 函数功能:	从测量点参数中获取规约类型
* 输入参数:	Pn                  侧脸点号
* 输出参数:	DesData             地址
* 返 回 值:	< 0                 错误
*		    > 0                 长度
*******************************************************************************/
int ReadMtrPrtlMode(uint8 *DesData, uint16 Pn, uint8 nArrIndex)
{	 
	uint32  i = 0;
	uint16  nPnTemp;
	
	uint8   nTtlIndex = 0;  //不同的端口号对应不同的数组下标
	nTtlIndex = nArrIndex;

	for(i = 0;i < gAmrInfo[nTtlIndex].TmnAmrInfo.MtrCnt;i++)
	{
	    nPnTemp = gAmrInfo[nTtlIndex].MtrAmrInfo[i].Pn;
        if(Pn == nPnTemp)
        {
			*DesData = gAmrInfo[nTtlIndex].MtrAmrInfo[i].nProtocolType;
			return 1;
		}

	}
	return 0;
	
}


/*******************************************************************************
* 函数名称:	IsExistAcqUnitAddrInAmrInfo
* 函数功能:	检测结构体中是否存在指定的采集器地址
* 输入参数:	AcqUnitAddr         要检测的测量点地址
* 输出参数:	MtrIndex            测量点索引（从0开始）
* 返 回 值:	电表类型值
*******************************************************************************/
MTR_TYPE_T IsExistAcqUnitAddrInAmrInfo(uint8 *AcqUnitAddr, uint16 *MtrIndex, uint8 nArrIndex)
{
    uint8   TempAcqAddr[6];
    uint16  Pn;                                 //测量点号
    int i;
	uint8   nTtlIndex = 0;  //不同的端口号对应不同的数组下标
	nTtlIndex = nArrIndex;

    for (i = 0; i < gAmrInfo[nTtlIndex].TmnAmrInfo.MtrCnt; i++)
    {
        Pn = gAmrInfo[nTtlIndex].MtrAmrInfo[i].Pn;
        ReadAcqUnitAddr(TempAcqAddr, Pn, nTtlIndex);

        //采集器地址有效
        if (IsValidMtrAddr(TempAcqAddr, 1) > 0)
        {
            //比较采集器地址
            if (memcmp(AcqUnitAddr, TempAcqAddr, 6) == 0)
            {
                *MtrIndex = i;
                return METER_PLC;
            }
        }
    }

    //不存在
    *MtrIndex = i;
    return METER_NULL;
}

/*******************************************************************************
* 函数名称:	IsExistCommAddrInAmrInfo
* 函数功能:	检测结构体中是否存在指定的通讯地址(采集器地址或测量点地址)
* 输入参数:	MtrAddr             要检测的测量点地址
* 输出参数:	MtrIndex            测量点索引（从0开始）
* 返 回 值:	电表类型值
*******************************************************************************/
MTR_TYPE_T IsExistCommAddrInAmrInfo(uint8 *CommAddr, uint16 *MtrIndex, uint8 nArrIndex)
{
    uint8   TempMtrAddr[6];
    uint16  Pn;                                 //测量点号
    int i;
	uint8   nTtlIndex = 0;  //不同的端口号对应不同的数组下标
	nTtlIndex = nArrIndex;

    for (i = 0; i < gAmrInfo[nTtlIndex].TmnAmrInfo.MtrCnt; i++)
    {
        Pn = gAmrInfo[nTtlIndex].MtrAmrInfo[i].Pn;
		ReadMtrAddr(TempMtrAddr, Pn, nTtlIndex);

        //测量点地址有效
        if (IsValidMtrAddr(TempMtrAddr, 1) > 0)
        {
            //比较测量点地址
            if (memcmp(CommAddr, TempMtrAddr, 6) == 0)
	        {
	            *MtrIndex = i;
	            return METER_PLC;
	        }
        }

    }

    //不存在
    *MtrIndex = i;
    return METER_NULL;
}


/*******************************************************************************
* 函数名称:	IsExistMtrAddrInAmrInfo
* 函数功能:	检测结构体中是否存在指定的测量点地址
* 输入参数:	MtrAddr             要检测的测量点地址
* 输出参数:	MtrIndex            测量点索引（从0开始）
* 返 回 值:	电表类型值
*******************************************************************************/
int IsExistMtrAddrInAmrInfo(uint8 *MtrAddr, uint16 *MtrIndex, uint8 nArrIndex)
{
    //uint8   TempMtrAddr[6];
    //uint16  Pn;                                 //测量点号
    int i;
    uint8 TempAddr9[6]={0x99,0x99,0x99,0x99,0x99,0x99};
    uint8 TempAddrA[6]={0xAA,0xAA,0xAA,0xAA,0xAA,0xAA};

	uint8   nTtlIndex = 0;  //不同的端口号对应不同的数组下标
	nTtlIndex = nArrIndex;

    for (i = 0; i < gAmrInfo[nTtlIndex].TmnAmrInfo.MtrCnt; i++)
    {
        if (memcmp(MtrAddr, gAmrInfo[nTtlIndex].MtrAmrInfo[i].aMeterFilesAddr, 6) == 0)
        {
            *MtrIndex = i;
            return gAmrInfo[nTtlIndex].MtrAmrInfo[i].nProtocolType;
        }
    }

	if((memcmp(TempAddr9,MtrAddr,6) == 0)||(memcmp(TempAddrA,MtrAddr,6) == 0))//广播校时过滤
    {
	      return -1;
    }

    //不存在
    *MtrIndex = i;
    return -1;
}



