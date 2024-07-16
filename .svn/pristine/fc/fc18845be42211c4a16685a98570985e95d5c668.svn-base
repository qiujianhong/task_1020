/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief： 功能描述
* @date： 2019-10-25
*********************************************************************
*/


#include "CcoPrtl.h"

#include "CcoCommon.h"
#include "CcoDebug.h"
#include "CcoFiles.h"
#include "CcoLib.h"
#include "CcoTTL.h"

/*******************************************************************************
* 变量申明
*******************************************************************************/
uint8   gPrtlLinkLen = 2;                       //规约链路长度，默认2字节
uint8   gChannelID = 0;                         //信道标识
uint8   g_InerSeq[MAX_DOWN_THREAD_NUM] = {0};   //内部帧序列号
uint8	g_MetRetLen = 0x28;                     //预计返回的数据长度

/*******************************************************************************
* 函数名称:	CheckFrameFormat
* 函数功能:	检查帧格式
* 输入参数:	FrameLen            帧长度
*           FrameBuff           帧数据
*           LenMode             长度模式
* 输出参数:	FrameEnd            帧结束偏移
* 返 回 值:	>0                  有效帧
*           <0                  无效帧
*******************************************************************************/
int8 CheckFrameFormat(uint16 FrameLen, uint8 *FrameBuff, uint16 *FrameEnd, uint8 LenMode)
{
	uint16  i = 0;
    uint16  DataIndex = 0;                      //指针偏移
    uint16  FrameDataLen=0;                     //帧数据总长度
    uint8   CheckSum = 0;                       //校验和

    //查找帧起始符68H
    for (i = 0; i < FrameLen; i++)
    {
        if (FrameBuff[i] == 0x68)
        {
            break;
        }
        else
        {
            DataIndex++;
        }
    }
	
    //无效帧
    if (DataIndex >= FrameLen)
    {
        *FrameEnd = FrameLen;
        return -1;
    }
    if (LenMode == 1)
    {
        FrameDataLen = (uint8)FrameBuff[DataIndex+1];
    }
    else
    {
        //计算帧数据长度
        memcpy(&FrameDataLen, &FrameBuff[DataIndex+1], 2);
    }
	
    //容错处理
    if ((DataIndex + FrameDataLen) > FrameLen)
    {
        *FrameEnd = FrameLen;
        return -2;
    }
	
    //检测帧尾
    if (FrameBuff[DataIndex+FrameDataLen-1] != 0x16)
    {
        *FrameEnd = DataIndex+1;
        return -3;
    }
	
    //检验和的范围为控制域+用户数据区
    for (i = (LenMode+1); i < (DataIndex+FrameDataLen-2); i++)
    {
        CheckSum += FrameBuff[DataIndex+i];
    }
    if (FrameBuff[DataIndex+FrameDataLen-2] != CheckSum)
    {
        *FrameEnd = DataIndex+1;
        return -4;
    }
	
    *FrameEnd = DataIndex+FrameDataLen;
    gPrtlLinkLen = LenMode;

    if (gPrtlLinkLen == 1)
    {
        //printx("支持698.42规约格式\n");
		//writelog(ROUTER_CCO_RECD_LOG, "支持698.42规约格式");
    }
	
    return 1;
}


/*******************************************************************************
* 函数名称:	UnPackFrameData
* 函数功能:	解帧
* 输入参数:	FrameLen            帧长度
*           FrameBuff           帧数据
* 输出参数:	AppData             应用层数据
* 返 回 值: CHECK_RESULT_SUCCEED
*           CHECK_RESULT_FAILUER
*           CHECK_RESULT_CONFIRM
*           CHECK_RESULT_DENY
*******************************************************************************/
int8 UnPackFrameData(uint16 FrameLen, uint8 *FrameBuff, AppData_T *AppData)
{
    //CCODOWN_FMT_DEBUG(0, "UnPackFrameData()...start...FrameLen=%d \n", FrameLen);
    
    uint16  FrameDataLen=0;                     //帧数据总长度
    uint16  DataIndex = 0;                      //指针偏移
    int     i = 0;
    uint8   CheckSum = 0;  //校验和

    //CCODOWN_FMT_DEBUG(0, "UnPackFrameData()... 111 \n");

    if (gPrtlLinkLen == 1)
    {
        //计算帧数据长度, 偏移到信息域R
        FrameDataLen= (uint8)FrameBuff[1];
        DataIndex = 3;
    }
    else
    {
        //计算帧数据长度, 偏移到信息域R
        memcpy(&FrameDataLen, &FrameBuff[1], 2);
        DataIndex = 4;
    }

    //CCODOWN_FMT_DEBUG(0, "UnPackFrameData()... 222 \n");

    //判断报文长度是否合法
    if(FrameDataLen > FrameLen)
    {
        //报文长度大于buff长度, 判断该帧报文不完整
        CCODOWN_FMT_DEBUG(0, "UnPackFrameData()...if(FrameDataLen > FrameLen) 该帧报文不完整! \n");
        return CHECK_RESULT_FAILUER;
    }
    
    //CCODOWN_FMT_DEBUG(0, "UnPackFrameData()... 333 \n");

    //解析信息域, 偏移到地址域
    memcpy(AppData->RInfo, &FrameBuff[DataIndex], 6);
    DataIndex += 6;

    //CCODOWN_FMT_DEBUG(0, "UnPackFrameData()... 444 \n");

    //判断通讯模块标识，偏移到应用数据域
    if (AppData->RInfo[0]& 0x04)  //存在地址域
    {
        //偏移掉A1和A3，从节点上行无A2
        DataIndex += 12;
    }
    
    //CCODOWN_FMT_DEBUG(0, "UnPackFrameData()... 555 \n");

    AppData->AFN = FrameBuff[DataIndex];
    for (i = 0; i < 8; i++)
    {
        if (FrameBuff[DataIndex+1] & (0x01 << i))
        {
            AppData->Fn = FrameBuff[DataIndex+2] * 8 + (i + 1);
            break;
        }
    }
    DataIndex += 3;
    
    //CCODOWN_FMT_DEBUG(0, "UnPackFrameData()... 666 \n");

    if(FrameDataLen < (DataIndex + 2))
    {
        CCODOWN_FMT_DEBUG(0, "UnPackFrameData()...if(FrameDataLen < (DataIndex + 2)) 错误! FrameDataLen:%d, DataIndex:%d \n", FrameDataLen, DataIndex);
        return CHECK_RESULT_FAILUER;
    }
    AppData->DataUnitLen = FrameDataLen - (DataIndex + 2);

    //CCODOWN_FMT_DEBUG(0, "UnPackFrameData()... 777 \n");
    
    if(AppData->DataUnitLen > sizeof(AppData->DataUnitBuff))
    {
        CCODOWN_FMT_DEBUG(0, "UnPackFrameData()...if(AppData->DataUnitLen > sizeof(AppData->DataUnitBuff)) 错误! AppData->DataUnitLen:%d \n", AppData->DataUnitLen);
        return CHECK_RESULT_FAILUER;
    }
    memcpy(AppData->DataUnitBuff, &FrameBuff[DataIndex], AppData->DataUnitLen);
    
    //CCODOWN_FMT_DEBUG(0, "UnPackFrameData()... 888 \n");

    //校验和检查
    if (gPrtlLinkLen == 1)
    {
        for(i=2; i<FrameDataLen-2; i++)
        {
            CheckSum += FrameBuff[i];
        }
    }
    else
    {
        for(i=3; i<FrameDataLen-2; i++)
        {
            CheckSum += FrameBuff[i];
        }
    }
    if(CheckSum != FrameBuff[FrameDataLen-2])
    {
        CCODOWN_FMT_DEBUG(0, "UnPackFrameData()...if(CheckSum != FrameBuff[FrameDataLen-2]) 校验和错误! \n");
        return CHECK_RESULT_FAILUER;
    }
    
    //CCODOWN_FMT_DEBUG(0, "UnPackFrameData()... 999 \n");

    //帧尾检查
    if(FrameBuff[FrameDataLen-1] != 0x16)
    {
        CCODOWN_FMT_DEBUG(0, "UnPackFrameData()...if(FrameBuff[FrameDataLen-1] != 0x16) 帧尾错误! \n");
        return CHECK_RESULT_FAILUER;
    }
    
    //CCODOWN_FMT_DEBUG(0, "UnPackFrameData()... aaa \n");

    if (AppData->AFN == 0x00)
    {
        if (AppData->Fn == 1)                   //确认
        {
            return CHECK_RESULT_CONFIRM;
        }
        else if (AppData->Fn == 2)              //否认
        {
            return CHECK_RESULT_DENY;
        }
        else
        {
            return CHECK_RESULT_FAILUER;
        }
    }
    else
    {
        //缓存信道标识
        gChannelID = AppData->RInfo[1] & 0x0F;

        return CHECK_RESULT_SUCCEED;
    }


}


/*******************************************************************************
* 函数名称: GetCtrlCode
* 函数功能: 获取控制码
* 输入参数: AppData             应用层数据
* 输出参数: DesData             控制域数据
* 返 回 值: 无
*******************************************************************************/
void GetCtrlCode(uint8 *DesData, AppData_T AppData, uint8 nArrIndex)
{
    CTRLCODE_BITS_T CtrlCode;
    memset(&CtrlCode,0x0,sizeof(CtrlCode));

    uint8 nTtlIndex = 0;  //不同的端口号对应不同的数组下标
    nTtlIndex = nArrIndex;

    switch(gLocalCommModuleInfo[nTtlIndex].LocalCommPatternWord.b1.BitSect.CommPattern)
    {
    case 1://窄带
        CtrlCode.COMM_TYPE = 1;
        break;

    case 2://宽带
        CtrlCode.COMM_TYPE = 3;
        break;

    case 3://小无线
        CtrlCode.COMM_TYPE = 10;
        break;

    default:
        CtrlCode.COMM_TYPE = 1;
        break;
    }

    //传输方向位
    CtrlCode.DIR = 0;

    switch (AppData.AFN)
    {
    case 0x00:                                  //确认/否认
    case 0x14:                                  //路由数据抄读
        CtrlCode.PRM = 0;
        break;

    case 0x01:                                  //初始化
        CtrlCode.PRM = 1;
        break;

    default:
        CtrlCode.PRM = 1;
        break;
    }

    memcpy(DesData,&CtrlCode,1);

}

/*******************************************************************************
* 函数名称: GetInfoZoneData
* 函数功能: 获取信息域数据
* 输入参数: AppData             应用层数据
* 输出参数: DesData             信息域数据
* 返 回 值: 无
*******************************************************************************/
void GetInfoZoneData(uint8 *DesData, AppData_T AppData, uint8 nArrIndex)
{
    memset(DesData, 0x00, 6);

    uint8 nTtlIndex = 0;  //不同的端口号对应不同的数组下标
    nTtlIndex = nArrIndex;

    //路由标识
    if(gLocalCommModuleInfo[nTtlIndex].LocalCommPatternWord.b1.BitSect.RouterMangerPattern == 0)//带有路由管理方式
        DesData[0] |= 0x01;

    switch (AppData.AFN)
    {
        case 0x00://确认/否认
            //从模块上报帧中获取帧序列号
            DesData[5] = AppData.RInfo[5];
            DesData[1] = AppData.RInfo[1] & 0x0F;//信道标识
            break;

        case 0x01://初始化
        case 0x03://查询数据
        case 0x04://链路接口检测
        case 0x05://控制命令
        case 0x10://路由查询
        case 0x11://路由设置
        case 0x12://路由控制
        case 0x15://文件传输
            DesData[5] = g_InerSeq[nTtlIndex]++;//报文序列号
            break;


        case 0x02://数据转发
        case 0x13://路由数据转发
            DesData[0] |= 0x04;
            DesData[5] = g_InerSeq[nTtlIndex]++;//报文序列号
            DesData[2] = g_MetRetLen+16;
            g_MetRetLen = 60;
            break;

        case 0x14: //请求数据抄读
            if (AppData.ModuleID == 1)
            {
                DesData[0] |= 0x04;
            }
            DesData[1] = gChannelID;
            DesData[2] = g_MetRetLen+16;
            g_MetRetLen = 60;
            DesData[5] = AppData.RInfo[5];
        break;

        case 0xF1:
            if (AppData.ModuleID == 1)
            {
                DesData[0] |= 0x04;
            }
            DesData[5] = g_InerSeq[nTtlIndex]++;//报文序列号
            break;

     default:
        break;
    }

    //printf("帧序列号  : %d \n",DesData[5]);
}

/*******************************************************************************
* 函数名称:	ReadTmnAddr
* 函数功能:	读取终端的地址
* 输入参数:	无
* 输出参数:	RtuAddr             传出的6字节终端地址缓冲区
* 返 回 值:	1                   成功
*           0                   失败
*******************************************************************************/
uint8 ReadTmnAddr(uint8 *RtuAddr, uint8 nArrIndex)
{
    uint8  TmpAddr[30];

    memset(TmpAddr, 0x00, sizeof(TmpAddr));
    memcpy(RtuAddr, gTerminalAddr[nArrIndex], 6);
    return 1;
}

/*******************************************************************************
* 函数名称:	GetAddrZoneA1
* 函数功能:	获取地址域源地址A1
* 输入参数:	无
* 输出参数:	SrcAddr             源地址
* 返 回 值: 无
*******************************************************************************/
void GetAddrZoneA1(uint8 *SrcAddr, uint8 nArrIndex)
{
    ReadTmnAddr(SrcAddr, nArrIndex);
}


/*******************************************************************************
* 函数名称: GetAddrZoneData
* 函数功能: 获取地址域数据
* 输入参数: AppData             应用层数据
* 输出参数: DesData             地址域数据
* 返 回 值: >0                  有地址域
*           <0                  无地址域
*******************************************************************************/
int GetAddrZoneData(uint8 *DesData, AppData_T AppData, uint8 nArrIndex)
{
    switch (AppData.AFN)
    {
    case 0x02:                                  //数据转发
    case 0x13:                                  //路由数据转发
        GetAddrZoneA1(DesData, nArrIndex);
        memcpy(&DesData[6], AppData.DesAddr, 6);
        return 1;

    case 0x14:                                  //路由请求抄读
        if(AppData.Fn == 0x03)
        {
            printx("14 f3没有地址域\n");
            return -1;
        }
        if (AppData.ModuleID == 1)
        {
            GetAddrZoneA1(DesData, nArrIndex);
            memcpy(&DesData[6], AppData.DesAddr, 6);
            return 1;
        }
        break;

    case 0xF1:
        if (AppData.ModuleID == 1)
        {
            GetAddrZoneA1(DesData, nArrIndex);
            memcpy(&DesData[6], AppData.DesAddr, 6);
            return 1;
        }
        break;

    default:
        return -1;
    }

    return -1;
}

/*******************************************************************************
* 函数名称:	GetDtInfo
* 函数功能:	读取信息类DT
* 输入参数:	Fn                      数据单元标识
* 输出参数:	DT                      信息类DT
* 返 回 值: 数据帧长度
*******************************************************************************/
void GetDtInfo(uint8 *DT, uint16 Fn)
{
    DT[0] = 1 << ((Fn-1) % 8);
    DT[1] = (Fn-1) / 8;
}


/*******************************************************************************
* 函数名称: PackFrameData
* 函数功能: 组帧
* 输入参数: AppData             应用层数据
* 输出参数: 无
* 返 回 值: 数据帧长度
*******************************************************************************/
uint16 PackFrameData(AppData_T AppData, uint8 nArrIndex)
{
    uint8   nTtlIndex = 0;  //不同的端口号对应不同的数组下标
    uint8   FrameBuff[MAX_QUEUE_BUFF_LEN];      //帧数据缓冲区
    uint16  DataIndex = 0;
    int     i;
    uint8   CheckSum = 0;                       //校验和
    //printx("组帧\n");
    //PrintBuf(&AppData,30);
    nTtlIndex = nArrIndex;
    
    //帧头
    FrameBuff[0] = 0x68;

    if (gPrtlLinkLen == 1)
    {
        //跳过帧长度
        DataIndex = 2;
    }
    else
    {
        //跳过帧长度
        DataIndex = 3;
    }

    //控制码
    GetCtrlCode(&FrameBuff[DataIndex], AppData, nTtlIndex);
    DataIndex++;

    //信息域
    GetInfoZoneData(&FrameBuff[DataIndex], AppData, nTtlIndex);
    DataIndex += 6;

    //地址域
    if (GetAddrZoneData(&FrameBuff[DataIndex], AppData, nTtlIndex) > 0)
    {
        DataIndex += 12;  //没有中继地址？如何判断？
    }

    //应用数据域
    FrameBuff[DataIndex] = AppData.AFN;
    GetDtInfo(&FrameBuff[DataIndex+1], AppData.Fn);
    DataIndex += 3;

    if (AppData.DataUnitLen > 0)
    {
        memcpy(&FrameBuff[DataIndex], AppData.DataUnitBuff, AppData.DataUnitLen);
        DataIndex += AppData.DataUnitLen;
    }

    //检验和的范围为控制域+用户数据区
    for (i = (gPrtlLinkLen+1); i < DataIndex; i++)
    {
        CheckSum += FrameBuff[i];
    }
    FrameBuff[DataIndex] = CheckSum;
    DataIndex++;

    //帧尾
    FrameBuff[DataIndex] = 0x16;
    DataIndex++;

    if (gPrtlLinkLen == 1)
    {
        //回填长度L
        FrameBuff[1] = (uint8)DataIndex;
    }
    else
    {
        //回填长度L
        memcpy(&FrameBuff[1],&DataIndex,2);
    }

    //将数据发送到共享缓冲区
    PlcWriteQueue(pAmrSendDown[nTtlIndex], FrameBuff, DataIndex);

    return DataIndex;
}


/*******************************************************************************
* 函数名称:	PackMeterPrtl
* 函数功能:	电表规约组帧
* 输入参数:	MeterData           电表规约数据
* 输出参数:	PrtlData            组帧后数据
* 返 回 值:	帧长度
*******************************************************************************/
int PackMeterPrtl(uint8 *PrtlData, MeterPrtlFmt_T MeterData)
{
    uint8   CheckSum = 0;                       //校验和
    int     i;
    uint8   CurIndex;                           //索引

    PrtlData[0] = 0x68;
    memcpy(&PrtlData[1], MeterData.MtrAddr, 6);
    PrtlData[7] = 0x68;
    PrtlData[8] = MeterData.CtrlCode;
    PrtlData[9] = MeterData.DataLen;

    for (i=0; i < MeterData.DataLen; i++)
    {
        PrtlData[10+i] = MeterData.DataBuff[i] + 0x33;
    }
    CurIndex = MeterData.DataLen + 10;

    for (i = 0; i < CurIndex; i++)
    {
        CheckSum += PrtlData[i];
    }
    PrtlData[CurIndex] = CheckSum;
    PrtlData[CurIndex+1] = 0x16;

    CurIndex += 2;

    return CurIndex;
}


/*******************************************************************************
* 函数名称:	GetLocalCommModuleRunningModeInfo
* 函数功能:	通讯:设置是否允许上报
* 输入参数:	isAllowFlag          是否允许上报标志
* 输出参数:	无
* 返 回 值: 无
*******************************************************************************/
int GetLocalCommModuleRunningModeInfo(const uint8 *Buff, LOCAL_COMM_MODULE_RUNNING_PATTERN_INFO *lcmrpi,uint16 Len)
{
    int pos = 0;

    //<2013-8-28-9-45-10 t:修改AFN=03-F10状态字解析错误问题>
    lcmrpi->LocalCommPatternWord.b1.Word = Buff[pos];
    pos++;
    lcmrpi->LocalCommPatternWord.b2.Word = Buff[pos];
    pos++;
    lcmrpi->LocalCommPatternWord.b3.Word = Buff[pos];
    pos++;
    lcmrpi->LocalCommPatternWord.b4.Word = Buff[pos];
    pos++;
    lcmrpi->LocalCommPatternWord.b5.Word = Buff[pos];
    pos++;
    lcmrpi->LocalCommPatternWord.b6.Word = Buff[pos];
    pos++;
    //</2013-8-28-9-45-10>


	memcpy(&(lcmrpi->SlaverMonitorMaxOverTime), &Buff[pos], 1);//<corrigendum-2013-09-18>
    pos += 1;//<corrigendum-2013-09-18>

    memcpy(&(lcmrpi->BroadcastMaxOverTime), &Buff[pos], 2);
    pos += 2;
    memcpy(&(lcmrpi->MaxFrameLen), &Buff[pos], 2);
    pos += 2;
    memcpy(&(lcmrpi->FileTransferMaxSolePackageLen), &Buff[pos], 2);
    pos += 2;
    memcpy(&(lcmrpi->UpdateOperatorWaittingTime), &Buff[pos], 1);
    pos += 1;
    memcpy(&(lcmrpi->MasterAddr[0]), &Buff[pos], 6);
    pos += 6;
    memcpy(&(lcmrpi->MaxSlaverNum), &Buff[pos], 2);
    pos += 2;
    memcpy(&(lcmrpi->CurrentSlaverNum), &Buff[pos], 2);
    pos += 2;
    memcpy(&(lcmrpi->ModuleProtocolReleaseDate[0]), &Buff[pos], 3);
    pos += 3;
    memcpy(&(lcmrpi->ModuleProtocolLastRecordDate[0]), &Buff[pos], 3);
    pos += 3;

    memcpy(&(lcmrpi->ManufacturerInfo.InfoBuff[0]), &Buff[pos], 9);
    memcpy(&(lcmrpi->ManufacturerInfo.ManufacturerCode[0]), &Buff[pos], 2);
    pos += 2;
    memcpy(&(lcmrpi->ManufacturerInfo.ChipCode[0]), &Buff[pos], 2);
    pos += 2;
    memcpy(&(lcmrpi->ManufacturerInfo.EditionDate[0]), &Buff[pos], 3);
    pos += 3;
    memcpy(&(lcmrpi->ManufacturerInfo.EditionInfo), &Buff[pos], 2);
    pos += 2;


	int SpeedNum = lcmrpi->LocalCommPatternWord.b4.BitSect.SpeedNum;//<corrigendum-2013-09-18>
    int i = 0;
    for(i = 0; i < SpeedNum; i++)
    {
        memcpy(&(lcmrpi->CommSpeedUnitInfo[i]), &Buff[pos], 2);
    }

    return 1;
}


/*******************************************************************************
* 函数名称:	Comm_DataRelay
* 函数功能:	通讯:数据转发
* 输入参数:	MeterData           电表规约格式数据
*           RouterFlag          带路由标志
* 输出参数:	无
* 返 回 值: 无
*******************************************************************************/
void Comm_DataRelay(MeterPrtlFmt_T MeterData, uint8 RouterFlag, uint8 *buf, uint32 buf_len, uint8 nArrIndex)
{
    uint8   PrtlData[200];
    uint8   PrtlLen;
    uint8   Index=0;
    AppData_T AppData;

	PrtlLen = buf_len;
	if(PrtlLen > sizeof(PrtlData))
	{
		PrtlLen = sizeof(PrtlData);
	}
	memcpy(PrtlData, buf, PrtlLen);


	//以测量点地址刷新目的地址
	memcpy(AppData.DesAddr, MeterData.MtrAddr, 6);

    if (RouterFlag == 1)
    {
        AppData.AFN = 0x13;
        AppData.Fn = 1;
        AppData.DataUnitBuff[Index++] = MeterData.PrtlMode;
        if(MeterData.DelayFlag == 1)//当前通信延时有关
        {
            AppData.DataUnitBuff[Index++] = 1;
        }
        else
        {
            AppData.DataUnitBuff[Index++] = 0;
        }

        AppData.DataUnitBuff[Index++] = 0;
        AppData.DataUnitBuff[Index++] = PrtlLen;
        memcpy(&AppData.DataUnitBuff[Index], PrtlData, PrtlLen);
        AppData.DataUnitLen = PrtlLen+Index;
    }
    else if(RouterFlag == 2)
    {
        AppData.AFN = 0x05;
        AppData.Fn = 3;
        AppData.DataUnitLen = PrtlLen+2;
        AppData.DataUnitBuff[0] = MeterData.PrtlMode;
        AppData.DataUnitBuff[1] = PrtlLen;
        memcpy(&AppData.DataUnitBuff[2], PrtlData, PrtlLen);
    }
    else
    {
        AppData.AFN = 0x02;
        AppData.Fn = 1;
        AppData.DataUnitLen = PrtlLen+2;
        AppData.DataUnitBuff[0] = MeterData.PrtlMode;
        AppData.DataUnitBuff[1] = PrtlLen;
        memcpy(&AppData.DataUnitBuff[2], PrtlData, PrtlLen);
    }

    PackFrameData(AppData, nArrIndex);
}

/*******************************************************************************
* 函数名称:	CheckMeterProtocol
* 函数功能:	电表规约解帧
* 输入参数:	FrameLen            帧长度
*           FrameBuff           帧数据
* 输出参数:	MtrAddr             电表地址
*           DataLen             数据域长度
*           DataBuff            数据域内容
*           CtrlCode            控制码
* 返 回 值: METER_PROTOCOL_NULL
*           METER_PROTOCOL_645
*           METER_PROTOCOL_69845
*******************************************************************************/
int8 CheckMeterProtocol(uint16 FrameLen, uint8 *FrameBuff, MeterPrtlFmt_T *MeterPrtlData, uint16 *offset)
{
	int 			Ret = -1;
	
	Ret = UnPackMeterData(FrameLen, FrameBuff, MeterPrtlData, offset);
	if (Ret == CHECK_RESULT_FAILUER)
	{
		//如果查找645帧格式失败,那么按照698.45表协议进行解析
        Ret = GetFrame(FrameBuff, FrameLen, offset);
		if(Ret > 0)
		{
			printx(" CheckMeterProtocol()电表规约解帧 报文协议为698.45...\n");
			memcpy(MeterPrtlData->MtrAddr, &FrameBuff[*offset+5], 6);//地址偏移5，地址长度偏移4
			MeterPrtlData->CtrlCode = FrameBuff[*offset+3];//控制域偏移3
			MeterPrtlData->DataLen = Ret;//返回值即为抄表报文内部的实际有效数据长度
			memcpy(MeterPrtlData->DataBuff, &FrameBuff[*offset], MeterPrtlData->DataLen);//抄表报文内部的实际有效数据
			return METER_PROTOCOL_69845;
		}
		else
		{
			printx(" CheckMeterProtocol()电表规约解帧 报文格式解析错误...\n");
		}
        
	}
	else
	{
		printx(" CheckMeterProtocol()电表规约解帧 报文协议为645...\n");
		return METER_PROTOCOL_645;
	}

	return METER_PROTOCOL_NULL;
}


/*******************************************************************************
* 函数名称:	UnPackMeterData
* 函数功能:	电表规约解帧(645规约)
* 输入参数:	FrameLen            帧长度
*           FrameBuff           帧数据
* 输出参数:	MtrAddr             电表地址
*           DataLen             数据域长度
*           DataBuff            数据域内容
*           CtrlCode            控制码
* 返 回 值: CHECK_RESULT_SUCCEED
*           CHECK_RESULT_FAILUER
*           CHECK_RESULT_CONFIRM
*           CHECK_RESULT_DENY
*******************************************************************************/
int8 UnPackMeterData(uint16 FrameLen, uint8 *FrameBuff, MeterPrtlFmt_T *MeterPrtlData, uint16 *offset)

{
    uint16  DataIndex = 0xff;                   //指针偏移
    uint8   CS = 0;
    uint16   i;//start Common Problem

    //滑动偏移，定位帧头
    for (i = 0; i < FrameLen; i++)
    {
        if ((FrameBuff[i] == 0x68) && (FrameBuff[i+7] == 0x68))
        {
            DataIndex = i;
			*offset = DataIndex;
            break;
        }
    }

    //未定位到帧头，判为失败
    if (DataIndex == 0xff)
    {
        return CHECK_RESULT_FAILUER;
    }

    //刷新地址域
    memcpy(MeterPrtlData->MtrAddr, &FrameBuff[DataIndex+1], 6);

    //控制码
    MeterPrtlData->CtrlCode = FrameBuff[DataIndex+8];

    //刷新数据长度
    MeterPrtlData->DataLen = FrameBuff[DataIndex+9];

    //判断帧尾
    if (FrameBuff[(DataIndex + MeterPrtlData->DataLen + 11)] != 0x16)
    {
        return CHECK_RESULT_FAILUER;
    }

    //判断校验和
    for (i=0; i < (MeterPrtlData->DataLen + 10); i++)
    {
        CS += FrameBuff[DataIndex+i];
    }
    if (CS != FrameBuff[(DataIndex + MeterPrtlData->DataLen + 10)])
    {
        return CHECK_RESULT_FAILUER;
    }

    //刷新数据域内容，同时做余3码处理
    for (i = 0; i < MeterPrtlData->DataLen; i++)
    {
        MeterPrtlData->DataBuff[i] = FrameBuff[DataIndex+10+i] - 0x33;
    }

    //判断是否是否认帧
    if (MeterPrtlData->CtrlCode & 0x40)
    {
        return CHECK_RESULT_DENY;
    }

    //判断是否是确认帧
    if (MeterPrtlData->DataLen == 0)
    {
        return CHECK_RESULT_CONFIRM;
    }

    return CHECK_RESULT_SUCCEED;
}



const uint16 fcstab[256] = {
	0x0000, 0x1189, 0x2312, 0x329b, 0x4624, 0x57ad, 0x6536, 0x74bf,
	0x8c48, 0x9dc1, 0xaf5a, 0xbed3, 0xca6c, 0xdbe5, 0xe97e, 0xf8f7,
	0x1081, 0x0108, 0x3393, 0x221a, 0x56a5, 0x472c, 0x75b7, 0x643e,
	0x9cc9, 0x8d40, 0xbfdb, 0xae52, 0xdaed, 0xcb64, 0xf9ff, 0xe876,
	0x2102, 0x308b, 0x0210, 0x1399, 0x6726, 0x76af, 0x4434, 0x55bd,
	0xad4a, 0xbcc3, 0x8e58, 0x9fd1, 0xeb6e, 0xfae7, 0xc87c, 0xd9f5,
	0x3183, 0x200a, 0x1291, 0x0318, 0x77a7, 0x662e, 0x54b5, 0x453c,
	0xbdcb, 0xac42, 0x9ed9, 0x8f50, 0xfbef, 0xea66, 0xd8fd, 0xc974,
	0x4204, 0x538d, 0x6116, 0x709f, 0x0420, 0x15a9, 0x2732, 0x36bb,
	0xce4c, 0xdfc5, 0xed5e, 0xfcd7, 0x8868, 0x99e1, 0xab7a, 0xbaf3,
	0x5285, 0x430c, 0x7197, 0x601e, 0x14a1, 0x0528, 0x37b3, 0x263a,
	0xdecd, 0xcf44, 0xfddf, 0xec56, 0x98e9, 0x8960, 0xbbfb, 0xaa72,
	0x6306, 0x728f, 0x4014, 0x519d, 0x2522, 0x34ab, 0x0630, 0x17b9,
	0xef4e, 0xfec7, 0xcc5c, 0xddd5, 0xa96a, 0xb8e3, 0x8a78, 0x9bf1,
	0x7387, 0x620e, 0x5095, 0x411c, 0x35a3, 0x242a, 0x16b1, 0x0738,
	0xffcf, 0xee46, 0xdcdd, 0xcd54, 0xb9eb, 0xa862, 0x9af9, 0x8b70,
	0x8408, 0x9581, 0xa71a, 0xb693, 0xc22c, 0xd3a5, 0xe13e, 0xf0b7,
	0x0840, 0x19c9, 0x2b52, 0x3adb, 0x4e64, 0x5fed, 0x6d76, 0x7cff,
	0x9489, 0x8500, 0xb79b, 0xa612, 0xd2ad, 0xc324, 0xf1bf, 0xe036,
	0x18c1, 0x0948, 0x3bd3, 0x2a5a, 0x5ee5, 0x4f6c, 0x7df7, 0x6c7e,
	0xa50a, 0xb483, 0x8618, 0x9791, 0xe32e, 0xf2a7, 0xc03c, 0xd1b5,
	0x2942, 0x38cb, 0x0a50, 0x1bd9, 0x6f66, 0x7eef, 0x4c74, 0x5dfd,
	0xb58b, 0xa402, 0x9699, 0x8710, 0xf3af, 0xe226, 0xd0bd, 0xc134,
	0x39c3, 0x284a, 0x1ad1, 0x0b58, 0x7fe7, 0x6e6e, 0x5cf5, 0x4d7c,
	0xc60c, 0xd785, 0xe51e, 0xf497, 0x8028, 0x91a1, 0xa33a, 0xb2b3,
	0x4a44, 0x5bcd, 0x6956, 0x78df, 0x0c60, 0x1de9, 0x2f72, 0x3efb,
	0xd68d, 0xc704, 0xf59f, 0xe416, 0x90a9, 0x8120, 0xb3bb, 0xa232,
	0x5ac5, 0x4b4c, 0x79d7, 0x685e, 0x1ce1, 0x0d68, 0x3ff3, 0x2e7a,
	0xe70e, 0xf687, 0xc41c, 0xd595, 0xa12a, 0xb0a3, 0x8238, 0x93b1,
	0x6b46, 0x7acf, 0x4854, 0x59dd, 0x2d62, 0x3ceb, 0x0e70, 0x1ff9,
	0xf78f, 0xe606, 0xd49d, 0xc514, 0xb1ab, 0xa022, 0x92b9, 0x8330,
	0x7bc7, 0x6a4e, 0x58d5, 0x495c, 0x3de3, 0x2c6a, 0x1ef1, 0x0f78
};
#define PPPINITFCS16 0xffff /* Initial FCS value */
#define PPPGOODFCS16 0xf0b8 /* Good final FCS value */

uint16 CRC16_GetEx(uint16 CRC16_data,const uint8* buf, uint32 len)
{
	const unsigned char *cp=buf;
	while (len--)
		CRC16_data = (CRC16_data >> 8) ^ fcstab[(CRC16_data ^ *cp++) & 0xff];
	return (CRC16_data);
}
uint16 CRC16_Get(const uint8* buf, uint32 len)
{
	uint16 trialfcs;
	/* add on output */
	trialfcs = CRC16_GetEx( PPPINITFCS16, buf, len );
	trialfcs ^= 0xffff; /* complement */
	return trialfcs;
}

/****************************************************************************                                                              
*名称：GetFrame                                                                      
*函数功能:	电表规约解帧(698.45)                                                                      
*输入参数：                                                                  
*输出参数：返回报文头部0x68的实际偏移地址                                                                  
*处理：                                                                      
*返回：无                                                                    
****************************************************************************/
uint16 GetFrame(const uint8 *Buffer, uint16 DataLength, uint16 *OffSet)
{
	uint16	FrameLength=0;
	uint16	ffcs=0;
	uint16	cfcs=0;
	uint8	HeadLength=0;
	uint16  	offset = *OffSet;
	for (offset=0;offset<DataLength;offset++)
	{
		//framelen
		if (DataLength-offset<12)
		{
			*OffSet = offset;
			return 0;
		}
		//head
		if (0x68!=Buffer[offset])
		{
			continue;
		}
		//head fcs
		HeadLength=(Buffer[offset+4]&0x7)+4+3+2;
		if (DataLength-offset<HeadLength)
		{
			continue;
		}
		memcpy(&ffcs,Buffer+offset+HeadLength-2,2);
		cfcs=CRC16_Get(Buffer+offset+1,HeadLength-3);
		if (cfcs!=ffcs)
		{
			continue;
		}
		//framelen
		memcpy(&FrameLength,Buffer+offset+1,2);
		FrameLength+=2;
		//if (FrameLength>MAX_FRAME_LENGTH_LPDU)
		//{
		//	continue;
		//}
		if (DataLength-offset<FrameLength)
		{
			continue;
		}
		//tail 
		if (0x16!=Buffer[offset+FrameLength-1])
		{
			continue;
		}
		//fcs
		memcpy(&ffcs,Buffer+offset+FrameLength-3,2);
		cfcs=CRC16_Get(Buffer+offset+1,FrameLength-4);
		if (cfcs!=ffcs)
		{
			continue;
		}
		*OffSet = offset;
		return FrameLength;
	}
	*OffSet = offset;
	return 0;
}








