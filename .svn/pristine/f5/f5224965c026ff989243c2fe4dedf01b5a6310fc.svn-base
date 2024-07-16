
/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief： 功能描述
* @date： 2019-10-25
*********************************************************************
*/

#include "ac_module.h"
#include "ac_lib.h"
#include "ac_mqtt.h"


//5个模组的信息
DEV_HAL_INFO  gDevHalInfo[MAX_ALLOWED_DOWN_NUM];  //485模块HAL信息
BOOL gQueryAllModuleFlag = TRUE;  //程序刚启动时, 读取所有模块信息
MODULE_INFO_T gModuleInfo[MAX_ALLOWED_DOWN_NUM];  //8路485通道的信息(实际模组顺序及信息)
uint8         gReloadModuleFlag[6] = {0};  //首位：0表示无变化, 1表示变化, 0xaa表示初始化, 后面5个0/1表示模组变化状态

//模块插拔重新获取设备
uint8 gReloadDevFlag[MAX_ALLOWED_DOWN_NUM] = {0};       //模块插拔重新获取设备
uint8 gAc485Port = 0;  //485槽位号

/**********************************************************
* 函数名称：
		XADR_GetEleNum
* 功能描述：
		获取串类型数据元素长度
* 输入描述：
		indata:数据Buffer
* 返回值：
		bytes:编码的字节个数据
**********************************************************/
uint16 XADR_GetEleNum(const uint8 *indata,uint8 *bytes)
{
    uint16 EleNum;
    uint8 nbytes = 1;
    EleNum = indata[0];
    if(EleNum==0x81)
    {
        EleNum = indata[1];
        nbytes = 2;
    }
    else if(EleNum==0x82)
    {
        EleNum = indata[1];
        EleNum <<= 8;
        EleNum += indata[2];
        nbytes = 3;
    }
    else if(EleNum==0x80 || EleNum>0x82)
        return 0;
    if(bytes!=NULL)
        *bytes = nbytes;
    return EleNum;
}

/*******************************************************************************
* 函数名称: ModuleInfoDeal
* 函数功能: 模组信息解析
* 输入参数: 
* 输出参数: 无
* 返 回 值: 无
*******************************************************************************/
void ModuleInfoDeal(uint8 ModuleIndex, uint8 *RecvBuf, uint32 RecvLen)
{
	ACMQTT_FMT_TRACE("ModuleInfoDeal() ModuleIndex=%d \n", ModuleIndex);
	
	int i = 0;
	uint16 index = 0;
	uint16 lentmp = 0;
	uint8 lenoffset = 0;
	
	pthread_rwlock_wrlock(&gModuleInfo[ModuleIndex-1].ModuleLock);

    if(RecvBuf[index++] != 0)
    {
        gModuleInfo[ModuleIndex-1].IsValid = 0;
        pthread_rwlock_unlock(&gModuleInfo[ModuleIndex-1].ModuleLock);
        return;
    }
	
    lentmp = XADR_GetEleNum(&RecvBuf[index], &lenoffset);
    index += lenoffset;
    memcpy(gModuleInfo[ModuleIndex-1].DeviceType, &RecvBuf[index], lentmp);
    index += lentmp;
    
    lentmp = XADR_GetEleNum(&RecvBuf[index], &lenoffset);
    index += lenoffset;
    memcpy(gModuleInfo[ModuleIndex-1].DeviceID, &RecvBuf[index],lentmp);
    index += lentmp;
    
    memcpy_r((uint8*)&(gModuleInfo[ModuleIndex-1].SoftVersion), &RecvBuf[index], 4);
    index += 4;
    lentmp = XADR_GetEleNum(&RecvBuf[index], &lenoffset);
    index += lenoffset;
    memcpy(gModuleInfo[ModuleIndex-1].SoftDate, &RecvBuf[index], lentmp );
    index += lentmp ;
    
    memcpy_r((uint8*)&(gModuleInfo[ModuleIndex-1].HardVersion), &RecvBuf[index], 4);
    index += 4;

    lentmp = XADR_GetEleNum(&RecvBuf[index], &lenoffset);
    index += lenoffset;
    memcpy(gModuleInfo[ModuleIndex-1].HardDate, &RecvBuf[index], lentmp);
    index += lentmp;

    lentmp = XADR_GetEleNum(&RecvBuf[index], &lenoffset);
    index += lenoffset;
    memcpy(gModuleInfo[ModuleIndex-1].ManufacturerCode, &RecvBuf[index], lentmp);
    index += lentmp;
    gModuleInfo[ModuleIndex-1].ChannelNum = RecvBuf[index++];
    ACMQTT_FMT_TRACE("DeviceType %s DeviceID %s \n", gModuleInfo[ModuleIndex-1].DeviceType,gModuleInfo[ModuleIndex-1].DeviceID);
    ACMQTT_FMT_TRACE("SoftVersion %08x SoftDate %s \n", gModuleInfo[ModuleIndex-1].SoftVersion,gModuleInfo[ModuleIndex-1].SoftDate);
    ACMQTT_FMT_TRACE("HardVersion %08x HardDate %s \n", gModuleInfo[ModuleIndex-1].HardVersion,gModuleInfo[ModuleIndex-1].HardDate);
    ACMQTT_FMT_TRACE("ManufacturerCode %s ChannelNum %d\n", gModuleInfo[ModuleIndex-1].ManufacturerCode, gModuleInfo[ModuleIndex-1].ChannelNum);
    for(i = 0; i < gModuleInfo[ModuleIndex-1].ChannelNum; i++)
    {
        gModuleInfo[ModuleIndex-1].ChannelInfo[i].ChannelType = RecvBuf[index++];
        gModuleInfo[ModuleIndex-1].ChannelInfo[i].ChannelFunc = RecvBuf[index++];
        ACMQTT_FMT_TRACE("i %d type %d func %d \n", i, gModuleInfo[ModuleIndex-1].ChannelInfo[i].ChannelType, gModuleInfo[ModuleIndex-1].ChannelInfo[i].ChannelFunc);
    }
    gModuleInfo[ModuleIndex-1].IsValid = 1;
	gModuleInfo[ModuleIndex-1].ModuleID = ModuleIndex;

    pthread_rwlock_unlock(&gModuleInfo[ModuleIndex-1].ModuleLock);
}


/*******************************************************************************
* 函数名称: GetModuleInfo
* 函数功能: 获取模组序号对应的模组信息
* 输入参数: 
* 输出参数: 无
* 返 回 值: 无
*******************************************************************************/
int GetModuleInfo(uint8 ModuleIndex)
{
    ACMQTT_FMT_TRACE("GetModuleInfo ModuleIndex[%d]\n", ModuleIndex);
    if (ModuleIndex < 1 || ModuleIndex > MAX_MODULE_NUM)
    {
		return -1;
	}

	int8   SocketState = ACT_COMM_IDLE;        //运行状态
	time_t CommBeginTime;  //通讯开始时间
	uint8  RecvBuf[MAX_QUEUE_BUFF_LEN] = {0};  //socket接收缓冲区    
    int32  RecvLen = 0;                        //接收数据长度
    int8   RecvResult = 0;
    uint8  RetryCnt = 0;
    MSG_INFO_T *ptMsgInfo = NULL;
	//int ret = 0;

	uint8  aPayload[200] = {0};
	uint16 nPayloadLen = 0;

	uint16 nIID = 0;
	uint16 nIOP = 0;
	uint16 nIndex = 0;

    nPayloadLen += 3;
    aPayload[nPayloadLen++] = ModuleIndex;

    while (1)
	{
		switch (SocketState)
        {
	        case ACT_COMM_IDLE:
	        {
				SocketState = ACT_COMM_SEND;
	            break;
			}
			case ACT_COMM_SEND:
	        {
				//发送查询信息
				module_from_mapManager(aPayload, nPayloadLen, &nIID, &nIOP, &nIndex);

				//更新计时器
				CommBeginTime= time(NULL);

	            //转入进程间数据接收处理
	            SocketState = ACT_COMM_RECV;

	            break;
			}
			case ACT_COMM_RECV:
	        {
				memset(RecvBuf,0x00,sizeof(RecvBuf));
		    	RecvLen = PlcReadQueue(pModuleRecvUp, RecvBuf);
				
                if (RecvLen > 0)
				{
					ptMsgInfo = (MSG_INFO_T *)RecvBuf;
					
					//打印完整消息内容
					//RSPUP_FMT_DEBUG("GetModuleInfo()...ACT_COMM_RECV...通过Mqtt收到的消息为: \n");
					//PrintInfoDetail(ptMsgInfo);

					//消息过滤
					if((ptMsgInfo->IID == nIID) && (ptMsgInfo->IOP == nIOP) && (ptMsgInfo->MsgIndex == nIndex))
					{
						ModuleInfoDeal(ModuleIndex, ptMsgInfo->MsgData, ptMsgInfo->MsgLen);
						return 1;
					}
					else if(ptMsgInfo->IID == 0x0000 && ptMsgInfo->IOP == 0x0014)
					{
						ACMQTT_FMT_TRACE("GetModuleInfo() 收到IID == 0x0000, IOP == 0x0014回码错误信息! \n");
						return -1;
					}
					else
					{
						RecvResult = -1;
					}
				}
				else
				{
					RecvResult = -1;
				}
				
				if(RecvResult == -1)
	            {
	                if( abs(time(NULL)-CommBeginTime) > MAX_MODULE_DELAY_TIME)
	                {
	                    RetryCnt++;
	                    SocketState = ACT_COMM_RETRY;
                        ACMQTT_FMT_TRACE("获取模组信息超时. 重试RetryCnt = %d\n", RetryCnt);
	                }
	            }
				            
	            break;
			}
			case ACT_COMM_RETRY:
			{
				if(RetryCnt>3)
	            {
	                SocketState = ACT_COMM_IDLE;
	                return -1;
	            }
	            SocketState = ACT_COMM_SEND;
				
				break;
			}
	        default:
	        {
				SocketState = ACT_COMM_IDLE;
	            break;
			}
        }
		
		usleep(20000);
	}

	return 1;
}


/*******************************************************************************
* 函数名称: GetAllModuleInfo
* 函数功能: 查询所有模组信息
* 输入参数: 
* 输出参数: 无
* 返 回 值: 无
*******************************************************************************/
int GetAllModuleInfo()
{
    uint8 i = 0;
	int result = -1;
    
    for (i = 0; i < MAX_MODULE_NUM; i++)
    {
        memset(&gModuleInfo[i], 0x00, sizeof(MODULE_INFO_T));
        if (pthread_rwlock_init(&gModuleInfo[i].ModuleLock, NULL ) != 0)
        {
            ACMQTT_FMT_TRACE ("init gModuleInfo lock %i error! \n", i);
            continue;
        }
        ACMQTT_FMT_TRACE("开始获取模组%d信息！\n", i+1);
		
        if (GetModuleInfo(i+1) > 0)
        {
            ACMQTT_FMT_TRACE("获取模组%d信息成功！\n", i+1);
			result = 1; //检测到模组, 至于是不是遥信脉冲模组后面判断
        }
		else
        {
            ACMQTT_FMT_TRACE("获取模组%d信息失败！\n", i+1);
        }
    }

	return result;
}


/*******************************************************************************
* 函数名称: UpdateModuleInfo
* 函数功能: 更新模组信息
* 输入参数: 
* 输出参数: 无
* 返 回 值: 无
*******************************************************************************/
int UpdateModuleInfo()
{
	ACMQTT_FMT_TRACE("UpdateModuleInfo() 由于初始化/插拔导致的模组信息更新 begin... \n");
	uint8 i = 0;
	uint8 j = 0;
	uint8 count = 0;
	int result = -1;
    int pipe = 0;
    int ret = 0;

    uint8 newport = 0;

    /* 判断是否485模块出现变位 */
    for (i = 0; i < MAX_MODULE_NUM; i++)
	{
		pthread_rwlock_rdlock(&gModuleInfo[i].ModuleLock);

		if(gModuleInfo[i].IsValid==1)
		{
            /* 判断是否485模块 */
            if(0 == memcmp(gModuleInfo[i].DeviceType, "GBR", 3))
            {
                newport = i + 1;
            }
		}
		pthread_rwlock_unlock(&gModuleInfo[i].ModuleLock);
	}

    if(newport == gAc485Port)
    {
        ACMQTT_FMT_TRACE("485模块位置没有更新\n");
        return 0;
    }

    //初始化之前需要先释放
    for(pipe=0; pipe<RUN_DOWN_NUM; pipe++)
    {
        if(NULL != gDevHalInfo[pipe].dev)
        {
            gDevHalInfo[i].isvalid = 0;
            ret = hal_device_release((HW_DEVICE *)gDevHalInfo[pipe].dev);
            gDevHalInfo[pipe].dev = NULL;
			AC485COM_FMT_DEBUG(pipe, "hal_device_release() ret=%d \n", ret);
        }
    }

	memset(gDevHalInfo,0x00,sizeof(gDevHalInfo));
	memset(gReloadDevFlag, 0x01, sizeof(gReloadDevFlag));

    gAc485Port = 0;
    for(i = 0; i < MAX_MODULE_NUM; i++)
	{
		pthread_rwlock_rdlock(&gModuleInfo[i].ModuleLock);

		if(gModuleInfo[i].IsValid==1)
		{
			for(j=0; j<gModuleInfo[i].ChannelNum; j++)
			{
				if(gModuleInfo[i].ChannelInfo[j].ChannelFunc==3)
                {
                    if(count >= MAX_ALLOWED_DOWN_NUM)
                    {
                        pthread_rwlock_unlock(&gModuleInfo[i].ModuleLock);
                        return result;
                    }

					if(j != 2)//目前只维护第1个485模块的第2路通道
					{
						continue;
					}

					sprintf(gDevHalInfo[count].deviceName, "%s%d_%d", AC485_MODULE_DEVNAME, i+1, j);
                    gDevHalInfo[count].isvalid = 1;
					gDevHalInfo[count].moduleIndex = i+1;
					result = 1;
                    count++;
					gAc485Port = i+1;
					pthread_rwlock_unlock(&gModuleInfo[i].ModuleLock);
                    return result;                    
                }
			}
		}
		
		pthread_rwlock_unlock(&gModuleInfo[i].ModuleLock);
	}

	ACMQTT_FMT_TRACE("UpdateModuleInfo() 由于初始化/插拔导致的模组信息更新 end... \n");
	return result;
}


/*******************************************************************************
* 函数名称: ModuleEventFun
* 函数功能: 处理模组插拔事件
* 输入参数:
* 输出参数:
* 内部处理:
* 返 回 值: 
*******************************************************************************/
void ModuleEventFun(uint8* buf, uint16 len)
{
    if(len != 5)
    {
		 return;
	}
       
    uint32 ModuleIndex = 0;
    uint16 index = 0;
    uint8 status = 0;
	//int i = 0;
	//int ret = 0;

    //模组序号
    memcpy_r((uint8*)&ModuleIndex, &buf[index], 4);
    index += 4;
    status = buf[index++];//插拔状态 0插 1拔
    
    ACMQTT_FMT_TRACE("收到模块插拔事件 模组序号=%d, status=%d, 0插 1拔\n", ModuleIndex, status);
	
    if(ModuleIndex<1 || ModuleIndex>5)
    {
		return;
	}

    if(status == 0)
    {
        //通知主线程读取该序号消息
        gReloadModuleFlag[0] = 1;
		gReloadModuleFlag[ModuleIndex] = 1;
    }
    else if(status == 1)
    {
        /* 判断是是否有485模块拔出 */
        if(ModuleIndex != gAc485Port)
        {
            return;
        }
        
        //直接删除该序号对应信息
        pthread_rwlock_wrlock(&gModuleInfo[ModuleIndex-1].ModuleLock);

        /*
		for(i=0; i<MAX_ALLOWED_DOWN_NUM; i++)
		{
			if(gDevHalInfo[i].moduleIndex == ModuleIndex)
			{
				if(NULL != gDevHalInfo[i].dev)
				{
				    gDevHalInfo[i].isvalid = 0; 
					ret = hal_device_release((HW_DEVICE *)gDevHalInfo[i].dev);
                    gDevHalInfo[i].dev = NULL;
                    ACMQTT_FMT_TRACE("ModuleEventFun()释放 hal_device_release() i=%d, gDevHalInfo[i].moduleIndex=%d, ret=%d \n", i, gDevHalInfo[i].moduleIndex, ret);
				}
				
			}
		}
		*/
        gModuleInfo[ModuleIndex-1].IsValid = 0;
        pthread_rwlock_unlock(&gModuleInfo[ModuleIndex-1].ModuleLock); 

		UpdateModuleInfo();
		gReloadModuleFlag[ModuleIndex] = 0;
    }
    
}


/*******************************************************************************
* 函数名称: ModuleEventDeal
* 函数功能: 处理模组插拔事件
* 输入参数:
* 输出参数:
* 内部处理:
* 返 回 值: 
*******************************************************************************/
int ModuleEventDeal()
{
	int ModuleIndex = 1;
	int num = sizeof(gReloadModuleFlag);
	int ret = 1;
	int result = -1;
	
	for(ModuleIndex=1; ModuleIndex<num; ModuleIndex++)
	{
		ACMQTT_FMT_TRACE("ModuleEventDeal()...gReloadModuleFlag[%d]=%d \n", ModuleIndex, gReloadModuleFlag[ModuleIndex]);
		if(gReloadModuleFlag[ModuleIndex] == 1)
		{
			if(GetModuleInfo(ModuleIndex) > 0)
			{
				gReloadModuleFlag[ModuleIndex] = 0;
				result = 1;
			}
			else
			{
				ret = -1;
			}
		}
	}

	if(ret == 1)//事件所报所有插入模块均被检测查询到
	{
		gReloadModuleFlag[0] = 0;
	}

	return result;
}


