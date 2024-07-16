
/*
*********************************************************************
* Copyright(C) 2019 �Ͼ��������ӹɷ����޹�˾
* All rights reserved. 
* @brief�� ��������
* @date�� 2019-10-25
*********************************************************************
*/

#include "ac_module.h"
#include "ac_lib.h"
#include "ac_mqtt.h"


//5��ģ�����Ϣ
DEV_HAL_INFO  gDevHalInfo[MAX_ALLOWED_DOWN_NUM];  //485ģ��HAL��Ϣ
BOOL gQueryAllModuleFlag = TRUE;  //���������ʱ, ��ȡ����ģ����Ϣ
MODULE_INFO_T gModuleInfo[MAX_ALLOWED_DOWN_NUM];  //8·485ͨ������Ϣ(ʵ��ģ��˳����Ϣ)
uint8         gReloadModuleFlag[6] = {0};  //��λ��0��ʾ�ޱ仯, 1��ʾ�仯, 0xaa��ʾ��ʼ��, ����5��0/1��ʾģ��仯״̬

//ģ�������»�ȡ�豸
uint8 gReloadDevFlag[MAX_ALLOWED_DOWN_NUM] = {0};       //ģ�������»�ȡ�豸
uint8 gAc485Port = 0;  //485��λ��

/**********************************************************
* �������ƣ�
		XADR_GetEleNum
* ����������
		��ȡ����������Ԫ�س���
* ����������
		indata:����Buffer
* ����ֵ��
		bytes:������ֽڸ�����
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
* ��������: ModuleInfoDeal
* ��������: ģ����Ϣ����
* �������: 
* �������: ��
* �� �� ֵ: ��
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
* ��������: GetModuleInfo
* ��������: ��ȡģ����Ŷ�Ӧ��ģ����Ϣ
* �������: 
* �������: ��
* �� �� ֵ: ��
*******************************************************************************/
int GetModuleInfo(uint8 ModuleIndex)
{
    ACMQTT_FMT_TRACE("GetModuleInfo ModuleIndex[%d]\n", ModuleIndex);
    if (ModuleIndex < 1 || ModuleIndex > MAX_MODULE_NUM)
    {
		return -1;
	}

	int8   SocketState = ACT_COMM_IDLE;        //����״̬
	time_t CommBeginTime;  //ͨѶ��ʼʱ��
	uint8  RecvBuf[MAX_QUEUE_BUFF_LEN] = {0};  //socket���ջ�����    
    int32  RecvLen = 0;                        //�������ݳ���
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
				//���Ͳ�ѯ��Ϣ
				module_from_mapManager(aPayload, nPayloadLen, &nIID, &nIOP, &nIndex);

				//���¼�ʱ��
				CommBeginTime= time(NULL);

	            //ת����̼����ݽ��մ���
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
					
					//��ӡ������Ϣ����
					//RSPUP_FMT_DEBUG("GetModuleInfo()...ACT_COMM_RECV...ͨ��Mqtt�յ�����ϢΪ: \n");
					//PrintInfoDetail(ptMsgInfo);

					//��Ϣ����
					if((ptMsgInfo->IID == nIID) && (ptMsgInfo->IOP == nIOP) && (ptMsgInfo->MsgIndex == nIndex))
					{
						ModuleInfoDeal(ModuleIndex, ptMsgInfo->MsgData, ptMsgInfo->MsgLen);
						return 1;
					}
					else if(ptMsgInfo->IID == 0x0000 && ptMsgInfo->IOP == 0x0014)
					{
						ACMQTT_FMT_TRACE("GetModuleInfo() �յ�IID == 0x0000, IOP == 0x0014���������Ϣ! \n");
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
                        ACMQTT_FMT_TRACE("��ȡģ����Ϣ��ʱ. ����RetryCnt = %d\n", RetryCnt);
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
* ��������: GetAllModuleInfo
* ��������: ��ѯ����ģ����Ϣ
* �������: 
* �������: ��
* �� �� ֵ: ��
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
        ACMQTT_FMT_TRACE("��ʼ��ȡģ��%d��Ϣ��\n", i+1);
		
        if (GetModuleInfo(i+1) > 0)
        {
            ACMQTT_FMT_TRACE("��ȡģ��%d��Ϣ�ɹ���\n", i+1);
			result = 1; //��⵽ģ��, �����ǲ���ң������ģ������ж�
        }
		else
        {
            ACMQTT_FMT_TRACE("��ȡģ��%d��Ϣʧ�ܣ�\n", i+1);
        }
    }

	return result;
}


/*******************************************************************************
* ��������: UpdateModuleInfo
* ��������: ����ģ����Ϣ
* �������: 
* �������: ��
* �� �� ֵ: ��
*******************************************************************************/
int UpdateModuleInfo()
{
	ACMQTT_FMT_TRACE("UpdateModuleInfo() ���ڳ�ʼ��/��ε��µ�ģ����Ϣ���� begin... \n");
	uint8 i = 0;
	uint8 j = 0;
	uint8 count = 0;
	int result = -1;
    int pipe = 0;
    int ret = 0;

    uint8 newport = 0;

    /* �ж��Ƿ�485ģ����ֱ�λ */
    for (i = 0; i < MAX_MODULE_NUM; i++)
	{
		pthread_rwlock_rdlock(&gModuleInfo[i].ModuleLock);

		if(gModuleInfo[i].IsValid==1)
		{
            /* �ж��Ƿ�485ģ�� */
            if(0 == memcmp(gModuleInfo[i].DeviceType, "GBR", 3))
            {
                newport = i + 1;
            }
		}
		pthread_rwlock_unlock(&gModuleInfo[i].ModuleLock);
	}

    if(newport == gAc485Port)
    {
        ACMQTT_FMT_TRACE("485ģ��λ��û�и���\n");
        return 0;
    }

    //��ʼ��֮ǰ��Ҫ���ͷ�
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

					if(j != 2)//Ŀǰֻά����1��485ģ��ĵ�2·ͨ��
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

	ACMQTT_FMT_TRACE("UpdateModuleInfo() ���ڳ�ʼ��/��ε��µ�ģ����Ϣ���� end... \n");
	return result;
}


/*******************************************************************************
* ��������: ModuleEventFun
* ��������: ����ģ�����¼�
* �������:
* �������:
* �ڲ�����:
* �� �� ֵ: 
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

    //ģ�����
    memcpy_r((uint8*)&ModuleIndex, &buf[index], 4);
    index += 4;
    status = buf[index++];//���״̬ 0�� 1��
    
    ACMQTT_FMT_TRACE("�յ�ģ�����¼� ģ�����=%d, status=%d, 0�� 1��\n", ModuleIndex, status);
	
    if(ModuleIndex<1 || ModuleIndex>5)
    {
		return;
	}

    if(status == 0)
    {
        //֪ͨ���̶߳�ȡ�������Ϣ
        gReloadModuleFlag[0] = 1;
		gReloadModuleFlag[ModuleIndex] = 1;
    }
    else if(status == 1)
    {
        /* �ж����Ƿ���485ģ��γ� */
        if(ModuleIndex != gAc485Port)
        {
            return;
        }
        
        //ֱ��ɾ������Ŷ�Ӧ��Ϣ
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
                    ACMQTT_FMT_TRACE("ModuleEventFun()�ͷ� hal_device_release() i=%d, gDevHalInfo[i].moduleIndex=%d, ret=%d \n", i, gDevHalInfo[i].moduleIndex, ret);
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
* ��������: ModuleEventDeal
* ��������: ����ģ�����¼�
* �������:
* �������:
* �ڲ�����:
* �� �� ֵ: 
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

	if(ret == 1)//�¼��������в���ģ���������ѯ��
	{
		gReloadModuleFlag[0] = 0;
	}

	return result;
}


