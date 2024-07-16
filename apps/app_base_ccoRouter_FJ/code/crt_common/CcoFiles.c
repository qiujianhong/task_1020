/*
*********************************************************************
* Copyright(C) 2019 �Ͼ��������ӹɷ����޹�˾
* All rights reserved. 
* @brief�� ��������
* @date�� 2019-10-25
*********************************************************************
*/


#include "CcoFiles.h"

#include "CcoCommon.h"
#include "CcoDebug.h"
#include "CcoLib.h"
#include "CcoPrtl.h"
#include "CcoTTL.h"

/*******************************************************************************
* ��������:	
* ��������:	��ʱ���浵��˽���ļ�
* �������:	
* �������:	
* �� �� ֵ: 
*******************************************************************************/
void RegularWriteFilesToPData(uint8	  nArrIndex, uint16 nMinute)
{
	static int AccumulateWaitTime[MAX_DOWN_THREAD_NUM] = {0};             //�ۼƵȴ�ʱ��
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
* ��������:	
* ��������:	���������仯ʱ���޸�˽���ļ�
* �������:	
* �������:	
* �� �� ֵ: 
*******************************************************************************/
void WriteFilesToPData(uint8      nArrIndex)
{
	//������ȫ�ֱ�����������ɾ���ĵ���ص�����д�뵵��˽���ļ�
	//������
	int Ret = 0;

	uint8 nTtlIndex = 0;  //��ͬ�Ķ˿ںŶ�Ӧ��ͬ�������±�
	nTtlIndex = nArrIndex;

	
	Ret = clear_pdata_xram(gLocalFilesName[nTtlIndex]);
	
	CCOUP_FMT_DEBUG("WriteFilesToPData() Ret = clear_pdata_xram() Ret:%d... \n", Ret);

	CCOUP_FMT_DEBUG("***************д˽���������ļ�(write_pdata_xram)*************begin********** \n");
	PrintFilesList(gAmrMeterFilesListTemp[nTtlIndex], 0xaa);//��ӡ�����б���ϸ��Ϣ
	CCOUP_FMT_DEBUG("filename:%s...offset:%d, len:%d... \n", gLocalFilesName[nTtlIndex], 0, sizeof(gAmrMeterFilesListTemp[nTtlIndex]));
	
	Ret = write_pdata_xram(gLocalFilesName[nTtlIndex], (char *)&gAmrMeterFilesListTemp[nTtlIndex], 0, sizeof(gAmrMeterFilesListTemp[nTtlIndex]));
	
	CCOUP_FMT_DEBUG("WriteFilesToPData() Ret = write_pdata_xram() Ret:%d... \n", Ret);
	CCOUP_FMT_DEBUG("***************д˽���������ļ�(write_pdata_xram)*************end********** \n");
}

/*******************************************************************************
* ��������:	ReadFilesFromPData
* ��������:	������˽���ļ����ݶ�ȡ�������б�ȫ�ֱ���
* �������:	
* �������:	
* �� �� ֵ: 
*******************************************************************************/
void ReadFilesFromPData(uint8 nArrIndex)
{
	//������˽���ļ����ݣ������ȡ�������б�ȫ�ֱ���
	//������
	int Ret = 0;
	
	uint8 nTtlIndex = 0;  //��ͬ�Ķ˿ںŶ�Ӧ��ͬ�������±�
	nTtlIndex = nArrIndex;

	CCODOWN_FMT_DEBUG(nArrIndex, "***************��˽���������ļ�(read_pdata_xram)***********begin************\n");
	PrintFilesList(gAmrMeterFilesListTemp[nTtlIndex], nTtlIndex);//��ӡ�����б���ϸ��Ϣ
	CCODOWN_FMT_DEBUG(nArrIndex, "filename:%s...offset:%d, len:%d...\n", gLocalFilesName[nTtlIndex], 0, sizeof(gAmrMeterFilesListTemp[nTtlIndex]));
	
	memset((uint8 *)&gAmrMeterFilesListTemp[nTtlIndex], 0x00, sizeof(gAmrMeterFilesListTemp[nTtlIndex]));
	Ret = read_pdata_xram(gLocalFilesName[nTtlIndex], (char *)&gAmrMeterFilesListTemp[nTtlIndex], 0, sizeof(gAmrMeterFilesListTemp[nTtlIndex]));
	CCODOWN_FMT_DEBUG(nArrIndex, "ReadFilesFromPData() Ret = read_pdata_xram() Ret:%d...\n", Ret);
	
	PrintFilesList(gAmrMeterFilesListTemp[nTtlIndex], nTtlIndex);//��ӡ�����б���ϸ��Ϣ
	CCODOWN_FMT_DEBUG(nArrIndex, "filename:%s...offset:%d, len:%d...\n", gLocalFilesName[nTtlIndex], 0, sizeof(gAmrMeterFilesListTemp[nTtlIndex]));
	CCODOWN_FMT_DEBUG(nArrIndex, "***************��˽���������ļ�(read_pdata_xram)***********end************\n");
}


/*******************************************************************************
* ��������:	ReadAmrInfo
* ��������:	��������Ϣ
* �������:	��
* �������:	��
* �� �� ֵ:	��
*******************************************************************************/
void ReadAmrInfo(uint8    nArrIndex)
{
    //printx(" void ReadAmrInfo(uint32 nPortID)...begin \n");
    
    int     nMeterFilesNum;
    int     i = 0;
    uint8   nTtlIndex = 0;  //��ͬ�Ķ˿ںŶ�Ӧ��ͬ�������±�
    nTtlIndex = nArrIndex;
    
    //���������Ϣ��ʼ��
    memset(&gAmrInfo[nTtlIndex], 0x00, sizeof(AmInfo_T));
    
    nMeterFilesNum = gAmrMeterFilesList[nTtlIndex].nMeterFilesNum;  //��������

    //�Ӳ���������ж�ȡ��Ϣ����ṹ��ȶԣ�����ṹ�����ޣ�����֮���Ա��ص���˽���ļ�����Ϊ׼��
    for (i = 0; i < nMeterFilesNum; i++)
    {
        //���ͳ����+1
        gAmrInfo[nTtlIndex].TmnAmrInfo.MtrCnt++;
        
        //����������
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
* ��������:	IsValidMtrAddr
* ��������:	����Ƿ���Ч�Ĳ������ַ
* �������:	MtrAddr             Ҫ���Ĳ������ַ
*           AcqUnitFlag         �ɼ�����־
* �������:	��
* �� �� ֵ:	>0                  ��Ч
*           <0                  ��Ч
*******************************************************************************/
int IsValidMtrAddr(uint8 *MtrAddr, uint8 AcqUnitFlag)
{
    uint8   TestAddr[6];

    //�жϲɼ�����ַ�Ƿ���6���ֽ�0xff
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
    //�жϲɼ�����ַ�����ַ�Ƿ���6���ֽ�0x00
    memset(TestAddr, 0x00, 6);
    if (memcmp(MtrAddr, TestAddr, 6) == 0)
    {
        return -1;
    }

    return 1;
}


/*******************************************************************************
* ��������:	CheckRepeatMtrAddr
* ��������:	������ַ�Ƿ��Ѿ��ظ�
* �������:	��
* �������:	��
* �� �� ֵ:	> 0 �ظ�
            < 0 ���ظ�
*******************************************************************************/
int CheckRepeatMtrAddr(char *MtrAddr, uint8 nArrIndex)
{
    uint16 	Pn;
    int 	i;
    uint8	CmpMtrAddr[6];

	uint8   nTtlIndex = 0;  //��ͬ�Ķ˿ںŶ�Ӧ��ͬ�������±�
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
* ��������:	GetPnByMeterAddr
* ��������:	ͨ������ַ��ȡ�������
* �������:	uint8 *MtrAddr ����ַ
* �������:	��
* �� �� ֵ:	�������
*******************************************************************************/
uint16 GetPnByMeterAddr(uint8 *MtrAddr, uint8 nArrIndex)
{
    int i = 0;
    uint16 Pn = 0;
    uint8   TempMtrAddr[17] = {0};

	uint8   nTtlIndex = 0;  //��ͬ�Ķ˿ںŶ�Ӧ��ͬ�������±�
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
* ��������:	ReadMtrType
* ��������:	�Ӳ���������л�ȡ����������
* �������:	Pn                  �������
* �������:	��
* �� �� ֵ:
*******************************************************************************/
MTR_TYPE_T ReadMtrType(uint16 Pn, uint8 nArrIndex)
{
    return METER_PLC;
}

/*******************************************************************************
* ��������:	ReadAcqUnitAddr
* ��������:	�Ӳ���������л�ȡ�ɼ�����ַ
* �������:	Pn                  �������
* �������:	DesData             ��ַ
* �� �� ֵ:	< 0                 ����
*		    > 0                 ����
*******************************************************************************/
int ReadAcqUnitAddr(uint8 *DesData, uint16 Pn, uint8 nArrIndex)
{
    uint32  i = 0;
	uint16  nPnTemp;
	
	uint8   nTtlIndex = 0;  //��ͬ�Ķ˿ںŶ�Ӧ��ͬ�������±�
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
* ��������:	ReadMtrAddr
* ��������:	�Ӳ���������л�ȡ�������ַ
* �������:	Pn                  �������
* �������:	DesData             ��ַ
* �� �� ֵ:	< 0                 ����
*		    > 0                 ����
*******************************************************************************/
int ReadMtrAddr(uint8 *DesData, uint16 Pn, uint8 nArrIndex)
{
	uint32  i = 0;
	uint16  nPnTemp;
	
	uint8   nTtlIndex = 0;  //��ͬ�Ķ˿ںŶ�Ӧ��ͬ�������±�
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
* ��������:	GetFilesIdByMeterAddr
* ��������:	ͨ������ַ��ȡ�������
* �������:	uint8 *MtrAddr ����ַ
* �������:	��
* �� �� ֵ:	�������
*******************************************************************************/
uint16 GetFilesIdByMeterAddr(uint8 *MtrAddr, uint8 nArrIndex)
{
    int i = 0;

	uint8   nTtlIndex = 0;  //��ͬ�Ķ˿ںŶ�Ӧ��ͬ�������±�
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
* ��������:	GetProtocolByMeterAddr
* ��������:	ͨ������ַ��ȡ����Լ
* �������:	uint8 *MtrAddr ����ַ
* �������:	��
* �� �� ֵ:	�������
*******************************************************************************/
uint16 GetProtocolByMeterAddr(uint8 *MtrAddr, uint8 nArrIndex)
{
    int i = 0;

	uint8   nTtlIndex = 0;  //��ͬ�Ķ˿ںŶ�Ӧ��ͬ�������±�
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
* ��������:	ReadMtrPrtlMode
* ��������:	�Ӳ���������л�ȡ��Լ����
* �������:	Pn                  �������
* �������:	DesData             ��ַ
* �� �� ֵ:	< 0                 ����
*		    > 0                 ����
*******************************************************************************/
int ReadMtrPrtlMode(uint8 *DesData, uint16 Pn, uint8 nArrIndex)
{	 
	uint32  i = 0;
	uint16  nPnTemp;
	
	uint8   nTtlIndex = 0;  //��ͬ�Ķ˿ںŶ�Ӧ��ͬ�������±�
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
* ��������:	IsExistAcqUnitAddrInAmrInfo
* ��������:	���ṹ�����Ƿ����ָ���Ĳɼ�����ַ
* �������:	AcqUnitAddr         Ҫ���Ĳ������ַ
* �������:	MtrIndex            ��������������0��ʼ��
* �� �� ֵ:	�������ֵ
*******************************************************************************/
MTR_TYPE_T IsExistAcqUnitAddrInAmrInfo(uint8 *AcqUnitAddr, uint16 *MtrIndex, uint8 nArrIndex)
{
    uint8   TempAcqAddr[6];
    uint16  Pn;                                 //�������
    int i;
	uint8   nTtlIndex = 0;  //��ͬ�Ķ˿ںŶ�Ӧ��ͬ�������±�
	nTtlIndex = nArrIndex;

    for (i = 0; i < gAmrInfo[nTtlIndex].TmnAmrInfo.MtrCnt; i++)
    {
        Pn = gAmrInfo[nTtlIndex].MtrAmrInfo[i].Pn;
        ReadAcqUnitAddr(TempAcqAddr, Pn, nTtlIndex);

        //�ɼ�����ַ��Ч
        if (IsValidMtrAddr(TempAcqAddr, 1) > 0)
        {
            //�Ƚϲɼ�����ַ
            if (memcmp(AcqUnitAddr, TempAcqAddr, 6) == 0)
            {
                *MtrIndex = i;
                return METER_PLC;
            }
        }
    }

    //������
    *MtrIndex = i;
    return METER_NULL;
}

/*******************************************************************************
* ��������:	IsExistCommAddrInAmrInfo
* ��������:	���ṹ�����Ƿ����ָ����ͨѶ��ַ(�ɼ�����ַ��������ַ)
* �������:	MtrAddr             Ҫ���Ĳ������ַ
* �������:	MtrIndex            ��������������0��ʼ��
* �� �� ֵ:	�������ֵ
*******************************************************************************/
MTR_TYPE_T IsExistCommAddrInAmrInfo(uint8 *CommAddr, uint16 *MtrIndex, uint8 nArrIndex)
{
    uint8   TempMtrAddr[6];
    uint16  Pn;                                 //�������
    int i;
	uint8   nTtlIndex = 0;  //��ͬ�Ķ˿ںŶ�Ӧ��ͬ�������±�
	nTtlIndex = nArrIndex;

    for (i = 0; i < gAmrInfo[nTtlIndex].TmnAmrInfo.MtrCnt; i++)
    {
        Pn = gAmrInfo[nTtlIndex].MtrAmrInfo[i].Pn;
		ReadMtrAddr(TempMtrAddr, Pn, nTtlIndex);

        //�������ַ��Ч
        if (IsValidMtrAddr(TempMtrAddr, 1) > 0)
        {
            //�Ƚϲ������ַ
            if (memcmp(CommAddr, TempMtrAddr, 6) == 0)
	        {
	            *MtrIndex = i;
	            return METER_PLC;
	        }
        }

    }

    //������
    *MtrIndex = i;
    return METER_NULL;
}


/*******************************************************************************
* ��������:	IsExistMtrAddrInAmrInfo
* ��������:	���ṹ�����Ƿ����ָ���Ĳ������ַ
* �������:	MtrAddr             Ҫ���Ĳ������ַ
* �������:	MtrIndex            ��������������0��ʼ��
* �� �� ֵ:	�������ֵ
*******************************************************************************/
int IsExistMtrAddrInAmrInfo(uint8 *MtrAddr, uint16 *MtrIndex, uint8 nArrIndex)
{
    //uint8   TempMtrAddr[6];
    //uint16  Pn;                                 //�������
    int i;
    uint8 TempAddr9[6]={0x99,0x99,0x99,0x99,0x99,0x99};
    uint8 TempAddrA[6]={0xAA,0xAA,0xAA,0xAA,0xAA,0xAA};

	uint8   nTtlIndex = 0;  //��ͬ�Ķ˿ںŶ�Ӧ��ͬ�������±�
	nTtlIndex = nArrIndex;

    for (i = 0; i < gAmrInfo[nTtlIndex].TmnAmrInfo.MtrCnt; i++)
    {
        if (memcmp(MtrAddr, gAmrInfo[nTtlIndex].MtrAmrInfo[i].aMeterFilesAddr, 6) == 0)
        {
            *MtrIndex = i;
            return gAmrInfo[nTtlIndex].MtrAmrInfo[i].nProtocolType;
        }
    }

	if((memcmp(TempAddr9,MtrAddr,6) == 0)||(memcmp(TempAddrA,MtrAddr,6) == 0))//�㲥Уʱ����
    {
	      return -1;
    }

    //������
    *MtrIndex = i;
    return -1;
}



