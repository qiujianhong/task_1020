/*
*********************************************************************
* Copyright(C) 2019 �Ͼ��������ӹɷ����޹�˾
* All rights reserved. 
* @brief�� ��������
* @date�� 2020-01-03
*********************************************************************
*/

#include "pw_common.h"
#include "pw_lib.h"
#include "pw_protocol.h"
#include "pw_debug.h"

//������־·��
char *g_pRecordPathMain = {"/data/app/advPdAmr/PwRecordPathMain.log"};//���̵߳�����־
char *g_pRecordPathUpRecv = {"/data/app/advPdAmr/PwRecordPathUpRecv.log"};//���н��յ�����־
char *g_pRecordPathUpSend = {"/data/app/advPdAmr/PwRecordPathUpSend.log"};//���з��͵�����־
char *g_pRecordPathDownDB = {"/data/app/advPdAmr/PwRecordPathDownDB.log"};//�����������Ľ�����־

//��������˽���ļ���
char *g_pFixedAreaName = {"/data/app/advPdAmr/PwFixedAreaName"};  //��ֵ������˽���ļ�
char *g_pParamValueName = {"/data/app/advPdAmr/PwParamValueName"};  //��ֵ����˽���ļ�

/*******************************************************************************
* ȫ�ֱ���
*******************************************************************************/
DB_CLIENT g_sDataCenterClient;  //���������Ľ����ľ��(��)
DB_CLIENT g_sEventDbClient;  //���������Ľ����ľ��(�¼�ʹ��)
METER_FILE_T g_sMeterFileAll;   //�ն˵�����Ϣ
BOOL g_bTestEnable = FALSE;  //ң��ң�ŵ����������ݿ���


/*******************************************************************************
* ��ʱ��ر�������
*******************************************************************************/
PAST_TIME_FLAG_T             g_unPastTimeFlag;                           //��ʱ���־
DATE_TIME_T                  g_stCurBinTime;                             //��ǰBINʱ��
DATE_TIME_T                  g_stCurBcdTime;                             //��ǰBCDʱ��
DATE_TIME_T                  g_stPreBinTime;                             //�ϴ�BINʱ��

/*******************************************************************************
* ��ʱ��ر�������
*******************************************************************************/
PAST_TIME_FLAG_T             g_unDownPastTimeFlag;                           //��ʱ���־
DATE_TIME_T                  g_stDownCurBinTime;                             //��ǰBINʱ��
DATE_TIME_T                  g_stDownCurBcdTime;                             //��ǰBCDʱ��
DATE_TIME_T                  g_stDownPreBinTime;                             //�ϴ�BINʱ��

/*******************************************************************************
* ��ʱ��ر�������
*******************************************************************************/
PAST_TIME_FLAG_T             g_eMtPastTimeFlag;                            //��ʱ���־
DATE_TIME_T                  g_stMtCurBinTime;                             //��ǰBINʱ��
DATE_TIME_T                  g_stMtCurBcdTime;                             //��ǰBCDʱ��
DATE_TIME_T                  g_stMtPreBinTime;                             //�ϴ�BINʱ��

/*******************************************************************************
* ��������: param_data_init
* ��������: ��ֵ������ʼ��
* �������: ��
* �������: ��
* �� �� ֵ: ��
*******************************************************************************/
void param_data_init()
{
	int i = 0;
	int j = 0;
	int ret = 0;

	//ȫ�ֱ�����ʼ��
	memset(g_aYxDataSend, 0, sizeof(g_aYxDataSend));
	memset(g_aYcDataSend, 0, sizeof(g_aYcDataSend));
	memset(g_aEnergyDataSend, 0, sizeof(g_aEnergyDataSend));
	memset(g_aYkDataSend, 0, sizeof(g_aYkDataSend));
	memset(g_aFaultDataSend, 0, sizeof(g_aFaultDataSend));

	//��ȡ��ֵ������
	ret = read_pdata_xram(g_pFixedAreaName, (char *)&g_aFixedValueArea, 0, sizeof(g_aFixedValueArea));
	if(ret < 0)
    {
		FRAME104_FMT_DEBUG("��ȡ���ض�ֵ������ʧ��  read_pdata_xram(g_pFixedAreaName) error! ret=%d \n", ret);
	}

	//��ȡ��ֵ����
	ret = read_pdata_xram(g_pParamValueName, (char *)&g_aParaSetValue, 0, sizeof(g_aParaSetValue));
    if(ret < 0)
    {
    	FRAME104_FMT_DEBUG("��ȡ���ض�ֵ����ʧ��  read_pdata_xram(g_pParamValueName) error! ret=%d \n", ret);
		FRAME104_FMT_DEBUG("����Ĭ�϶�ֵ����  g_aParaValueInit \n");
		for(i=0; i<MAX_FIXED_AREA_NUM; i++)
		{
			for(j=0; j<g_nConfigParaNum; j++)
			{
				memcpy(&g_aParaSetValue[i][j], &g_aParaValueInit[j], sizeof(PARA_SET_VALUE_T));
			}
		}
	}

	//��ֵ������ʼ�����ش�ӡ��Ϣ
	for(i=0; i<MAX_FIXED_AREA_NUM; i++)
	{
		FRAME104_FMT_DEBUG("param_data_init() ��ֵ�����=%d, ��ֵ������ϸ��Ϣ: \n", i);
		for(j=0; j<g_nConfigParaNum; j++)
		{
			FRAME104_FMT_DEBUG("j=%d, Addr=%04x, TagType=%d, Valid=%d, DefaultValue=%f, MinValue=%f, MaxValue=%f, nDataLen=%d, unValue=%02x %02x %02x %02x %02x %02x %02x %02x %02x %02x  \n", 
								j, g_aParaSetValue[i][j].nObjAddr, g_aParaSetValue[i][j].nTagType, g_aParaSetValue[i][j].bValid, g_aParaSetValue[i][j].fDefaultValue, g_aParaSetValue[i][j].fMinValue, g_aParaSetValue[i][j].fMaxValue, g_aParaSetValue[i][j].nDataLen, 
								g_aParaSetValue[i][j].unValue.arrayV[0], g_aParaSetValue[i][j].unValue.arrayV[1], g_aParaSetValue[i][j].unValue.arrayV[2], g_aParaSetValue[i][j].unValue.arrayV[3], g_aParaSetValue[i][j].unValue.arrayV[4], 
								g_aParaSetValue[i][j].unValue.arrayV[5], g_aParaSetValue[i][j].unValue.arrayV[6], g_aParaSetValue[i][j].unValue.arrayV[7], g_aParaSetValue[i][j].unValue.arrayV[8], g_aParaSetValue[i][j].unValue.arrayV[9]);
		}
	}
}

/*******************************************************************************
* ��������: task_run_step_init
* ��������: 104����ִ������ȫ�ֱ�����ʼ��
* �������: ��
* �������: ��
* �� �� ֵ: ��
*******************************************************************************/
void task_run_step_init()
{
    //104ͨ������
    memset(&g_sTaskRunStep, 0x00, sizeof(g_sTaskRunStep));
	g_sTaskRunStep.nTaskStep = STEP_CONT_DEFAULT;
    g_sTaskRunStep.nStartNum = 0;
    g_sTaskRunStep.bFirstRun = 1;
}

/*******************************************************************************
* ��������: meter_file_by_pn_read
* ��������: ���ݲ�����Ŵ��������Ķ�ȡ������Ϣ
* �������: ��
* �������: ��
* �� �� ֵ: ��
*******************************************************************************/
int meter_file_by_pn_read(uint16 infonum, OOP_METER_T *gMeterFile)
{
	int ret = 0;
    uint32 len = 0;
    NOMAL_OAD_T NormalOad = {0};
    NormalOad.logicId = 0;
    NormalOad.oad.value = 0x60000200;
    NormalOad.infoNum = infonum;

    ret = db_read_nomal(g_sDataCenterClient, &NormalOad, sizeof(OOP_METER_T), (uint8*)gMeterFile, &len);
    if((ret !=0) || (len != sizeof(OOP_METER_T)))
    {
		return -1 ;
	}
	
    return 1;
}

/*******************************************************************************
* ��������: meter_file_init
* ��������: ��ʼ�������
* �������: ��
* �������: ��
* �� �� ֵ: ��
*******************************************************************************/
uint16 meter_file_init(METER_FILE_T *pMeterFileAll)
{
	uint16 MeterCount = 0;
    uint16 tmpId = 0;
    uint8 MeterNum = 0;     //��������
    int ret = 0;
    
    OOP_METER_T gMeterFile;     //ÿ�����ĵ�����Ϣ

    memset(pMeterFileAll, 0x00, sizeof(METER_FILE_T));
	
    FRAME104_FMT_DEBUG("��ʼ������ʼ��! g_sDataCenterClient=%08x \n", g_sDataCenterClient);
    //pthread_rwlock_wrlock(&pMeterFileAll->rwLock);

	//����tmpId��ȡ�����
    while (tmpId <= METER_MAX_NUM)
    {
        tmpId++;
        memset(&gMeterFile, 0x00, sizeof(OOP_METER_T));
        ret = meter_file_by_pn_read(tmpId, &gMeterFile);
        if(ret < 0)
        {
			continue;
		}

		
		FRAME104_FMT_TRACE("sizeof(OOP_METER_T)=%d, ������=%d, ret=%d. \n", sizeof(OOP_METER_T), tmpId, ret);
		FRAME104_FMT_TRACE("������=%d, �˿ں�=%08x, Э������=%d, ���߷�ʽ=%d. \n", tmpId, gMeterFile.basic.port.value, gMeterFile.basic.protocol, gMeterFile.basic.pwrType);
		FRAME104_FMT_TRACE("����ַ=0X%02x%02x%02x%02x%02x%02x. \n", gMeterFile.basic.tsa.add[5], gMeterFile.basic.tsa.add[4], gMeterFile.basic.tsa.add[3], gMeterFile.basic.tsa.add[2], gMeterFile.basic.tsa.add[1], gMeterFile.basic.tsa.add[0]);

		MeterNum = pMeterFileAll->MeterCount;
        pMeterFileAll->MeterFile[MeterNum] = gMeterFile;
        pMeterFileAll->MeterCount++;
    }
	
    MeterCount = pMeterFileAll->MeterCount;
    FRAME104_FMT_DEBUG("������ʼ����� ��������Ϊ %d \n",pMeterFileAll->MeterCount);
    //pthread_rwlock_unlock(&pMeterFileAll->rwLock);
    return MeterCount;
}

/*******************************************************************************
* ��������: config_xml_load
* ��������: ����xml�����Ϣ
* �������: ��
* �������: ��
* �� �� ֵ: ��
*******************************************************************************/
int fixed_para_set()
{
	int i = 0;

	for(i=0; i<g_nConfigParaNum; i++)
	{
		switch(g_aParaValueInit[i].nDealType)
		{
			case CALC_PARA_TMN_TYPE://����ն��ͺ�
			{
				g_aParaValueInit[i].nTagType = TAG_STRING;
				char datatemp[] = "TMN-2020";
				snprintf((void *)&g_aParaValueInit[i].unValue.arrayV[0], MAX_FIXED_VALUE_LEN, datatemp);
				g_aParaValueInit[i].nDataLen = sizeof(datatemp);
			}
				break;
			case CALC_PARA_TMN_OPSYS://�ն˲���ϵͳ
			{
				g_aParaValueInit[i].nTagType = TAG_STRING;
				char datatemp[] = "Linux3.10.108";
				snprintf((void *)&g_aParaValueInit[i].unValue.arrayV[0], MAX_FIXED_VALUE_LEN, datatemp);
				g_aParaValueInit[i].nDataLen = sizeof(datatemp);
			}
				break;
			case CALC_PARA_TMN_MAKER://�ն�������
			{
				g_aParaValueInit[i].nTagType = TAG_STRING;
				char datatemp[] = "XLDZ";
				snprintf((void *)&g_aParaValueInit[i].unValue.arrayV[0], MAX_FIXED_VALUE_LEN, datatemp);
				g_aParaValueInit[i].nDataLen = sizeof(datatemp);
			}
				break;
			case CALC_PARA_TMN_HARD://�ն�Ӳ���汾
			{
				g_aParaValueInit[i].nTagType = TAG_STRING;
				char datatemp[] = "HV00.01";
				snprintf((void *)&g_aParaValueInit[i].unValue.arrayV[0], MAX_FIXED_VALUE_LEN, datatemp);
				g_aParaValueInit[i].nDataLen = sizeof(datatemp);
			}
				break;
			case CALC_PARA_TMN_SOFT://�ն�����汾
			{
				g_aParaValueInit[i].nTagType = TAG_STRING;
				char datatemp[] = "HV00.01";
				snprintf((void *)&g_aParaValueInit[i].unValue.arrayV[0], MAX_FIXED_VALUE_LEN, datatemp);
				g_aParaValueInit[i].nDataLen = sizeof(datatemp);
			}
				break;
			case CALC_PARA_TMN_CHECK://�ն�����汾У����
			{
				g_aParaValueInit[i].nTagType = TAG_STRING;
				g_aParaValueInit[i].unValue.arrayV[0] = 0x11;
				g_aParaValueInit[i].unValue.arrayV[1] = 0x22;
				g_aParaValueInit[i].nDataLen = 2;
			}
				break;
			case CALC_PARA_TMN_PROTOCOL://�ն�ͨ�Ź�Լ����
			{
				g_aParaValueInit[i].nTagType = TAG_STRING;
				char datatemp[] = "2009���׼104";
				snprintf((void *)&g_aParaValueInit[i].unValue.arrayV[0], MAX_FIXED_VALUE_LEN, datatemp);
				g_aParaValueInit[i].nDataLen = sizeof(datatemp);
			}
				break;
			case CALC_PARA_TMN_MODEL://�ն˳����ͺ�
			{
				g_aParaValueInit[i].nTagType = TAG_STRING;
				char datatemp[] = "XLDZ-2020-1";
				snprintf((void *)&g_aParaValueInit[i].unValue.arrayV[0], MAX_FIXED_VALUE_LEN, datatemp);
				g_aParaValueInit[i].nDataLen = sizeof(datatemp);
			}
				break;
			case CALC_PARA_TMN_ID://�ն�ID��
			{
				g_aParaValueInit[i].nTagType = TAG_STRING;
				char datatemp[] = "XLDZ-2020-06-10-0001";
				snprintf((void *)&g_aParaValueInit[i].unValue.arrayV[0], MAX_FIXED_VALUE_LEN, datatemp);
				g_aParaValueInit[i].nDataLen = sizeof(datatemp);
			}
				break;
			case CALC_PARA_TMN_MAC://�ն�����MAC��ַ
			{
				g_aParaValueInit[i].nTagType = TAG_STRING;
				char datatemp[] = "MAC-123-456-7890";
				snprintf((void *)&g_aParaValueInit[i].unValue.arrayV[0], MAX_FIXED_VALUE_LEN, datatemp);
				g_aParaValueInit[i].nDataLen = sizeof(datatemp);
			}
				break;
			default:
				break;
		}
	}
	
	return 1;
}


/*******************************************************************************
* ��������: config_xml_load
* ��������: ����xml�����Ϣ
* �������: ��
* �������: ��
* �� �� ֵ: ��
*******************************************************************************/
int config_xml_load()
{
	
	g_nConfigYxNum = 0;  //ң�ŵ��ʵ�����ø���
	g_nConfigYcNum = 0;  //ң����ʵ�����ø���
	g_nConfigYkNum = 0;  //ң�ص��ʵ�����ø���
	g_nConfigEnNum = 0;  //�������ʵ�����ø���
	g_nConfigFaultNum = 0;  //�����¼����ʵ�����ø���
	g_nConfigParaNum = 0;  //�������ʵ�����ø���

	//���ñ��ʼ��
	memset(&g_aYxDataConfig[0], 0x00, sizeof(g_aYxDataConfig));
	memset(&g_aYcDataConfig[0], 0x00, sizeof(g_aYcDataConfig));
	memset(&g_aEnergyDataConfig[0], 0x00, sizeof(g_aEnergyDataConfig));
	memset(&g_aYkDataConfig[0], 0x00, sizeof(g_aYkDataConfig));
	memset(&g_aParaValueInit[0], 0x00, sizeof(g_aParaValueInit));
	memset(&g_aParaSetValue[0], 0x00, sizeof(g_aParaSetValue));
	memset(&g_aFaultDataConfig[0], 0x00, sizeof(g_aFaultDataConfig));

	//���ص��
	advPd_readyxfromXML(&g_nConfigYxNum);
	advPd_readycfromXML(&g_nConfigYcNum);
	advPd_readenfromXML(&g_nConfigEnNum);
	advPd_readykfromXML(&g_nConfigYkNum);
	advPd_readfaultfromXML(&g_nConfigFaultNum);
	advPd_readParafromXML(&g_nConfigParaNum);

	//�����б���в�����ֵ
	fixed_para_set();

	if(g_nConfigYxNum > MAX_YX_NUM)
	{
		FRAME104_FMT_DEBUG("if(g_nConfigYxNum > MAX_YX_NUM) g_nConfigYxNum=%d, MAX_YX_NUM=%d \n", g_nConfigYxNum, MAX_YX_NUM);
		g_nConfigYxNum = MAX_YX_NUM;
	}	

	if(g_nConfigYcNum > MAX_YC_NUM)
	{
		FRAME104_FMT_DEBUG("if(g_nConfigYcNum > MAX_YC_NUM) g_nConfigYcNum=%d, MAX_YC_NUM=%d \n", g_nConfigYcNum, MAX_YC_NUM);
		g_nConfigYcNum = MAX_YC_NUM;
	}	

	if(g_nConfigEnNum > MAX_ENERGY_NUM)
	{
		FRAME104_FMT_DEBUG("if(g_nConfigEnNum > MAX_ENERGY_NUM) g_nConfigEnNum=%d, MAX_ENERGY_NUM=%d \n", g_nConfigEnNum, MAX_ENERGY_NUM);
		g_nConfigEnNum = MAX_ENERGY_NUM;
	}	

	if(g_nConfigYkNum > MAX_YK_NUM)
	{
		FRAME104_FMT_DEBUG("if(g_nConfigYkNum > MAX_YK_NUM) g_nConfigYkNum=%d, MAX_YK_NUM=%d \n", g_nConfigYkNum, MAX_YK_NUM);
		g_nConfigYkNum = MAX_YK_NUM;
	}	

	if(g_nConfigFaultNum > FAULT_MAX_NUM)
	{
		FRAME104_FMT_DEBUG("if(g_nConfigFaultNum > FAULT_MAX_NUM) g_nConfigFaultNum=%d, FAULT_MAX_NUM=%d \n", g_nConfigFaultNum, FAULT_MAX_NUM);
		g_nConfigFaultNum = FAULT_MAX_NUM;
	}	

	if(g_nConfigParaNum > PARA_MAX_NUM)
	{
		FRAME104_FMT_DEBUG("if(g_nConfigParaNum > PARA_MAX_NUM) g_nConfigParaNum=%d, PARA_MAX_NUM=%d \n", g_nConfigParaNum, PARA_MAX_NUM);
		g_nConfigParaNum = PARA_MAX_NUM;
	}
	
	FRAME104_FMT_DEBUG("******���XML����: ң�Ÿ���:%d, ң�����:%d, ��������:%d, ң�ظ���:%d, �¼�����:%d, ��������:%d \n", 
		g_nConfigYxNum, g_nConfigYcNum, g_nConfigEnNum, g_nConfigYkNum, g_nConfigFaultNum, g_nConfigParaNum);

	return 1;
}

/*******************************************************************************
* ��������: global_init
* ��������: ���̳�ʼ���ܺ���
* �������: ��
* �������: ��
* �� �� ֵ: ��
*******************************************************************************/
int global_init()
{
    param_data_init();  //��ֵ������ʼ��
	past_time_init();  //ʱ������ʼ��
	task_run_step_init();  //104����ִ������ȫ�ֱ�����ʼ��
	
	return 1;
}

/*******************************************************************************
* ��������: meter_file_change
* ��������: ��������Ĵ�����
* �������: choice 1 ��ʾ������� �������߸ı�; choice 0 ��ʾ����ɾ�� gMeterFile��nindexΪɾ�������
* �������: ��
* �� �� ֵ: ��
*******************************************************************************/
void meter_file_change(METER_FILE_T *pMeterFileAll, uint8 choice, OOP_METER_T gMeterFile)
{
    uint16 i;
    int index = -1;
    
    //pthread_rwlock_wrlock(&pMeterFileAll->rwLock);

    DB104_FMT_TRACE("meter_file_change() ���뵵���޸����� choice=%d \n",choice);
	DB104_FMT_TRACE("������=%d, �˿ں�=%08x, Э������=%d, ���߷�ʽ=%d. \n", gMeterFile.nIndex, gMeterFile.basic.port.value, gMeterFile.basic.protocol, gMeterFile.basic.pwrType);
	DB104_FMT_TRACE("����ַ=0X%02x%02x%02x%02x%02x%02x. \n", gMeterFile.basic.tsa.add[5], gMeterFile.basic.tsa.add[4], gMeterFile.basic.tsa.add[3], gMeterFile.basic.tsa.add[2], gMeterFile.basic.tsa.add[1], gMeterFile.basic.tsa.add[0]);
	
    if(choice == 0)
    {
        for(i=0; i<pMeterFileAll->MeterCount; i++)
        {
            if(pMeterFileAll->MeterFile[i].nIndex == gMeterFile.nIndex)
            {
                index = i;
                memset(&pMeterFileAll->MeterFile[i], 0x00, sizeof(OOP_METER_T));
				DB104_FMT_TRACE("meter_file_change() ���뵵���޸����� ɾ�������ɹ�! \n");
                break;
            }
        }
        if(index != -1)
        {
            for(i=index; i<pMeterFileAll->MeterCount-1; i++)
            {
                pMeterFileAll->MeterFile[i] = pMeterFileAll->MeterFile[i+1];
            }
            memset(&(pMeterFileAll->MeterFile[pMeterFileAll->MeterCount-1]),0x00,sizeof(OOP_METER_T));
            pMeterFileAll->MeterCount--;
        }
    }
    else
    {
        for(i=0; i<pMeterFileAll->MeterCount; i++)
        {
            //�����ǰ�������ҵ���ͬ�� ��ֱ���滻
            if(pMeterFileAll->MeterFile[i].nIndex == gMeterFile.nIndex)
            {
                index = i;
                memcpy(&pMeterFileAll->MeterFile[i],&gMeterFile,sizeof(OOP_METER_T));
				DB104_FMT_TRACE("meter_file_change() ���뵵���޸����� ���µ����ɹ�! \n");
                break;
            }
        }
        //���û���ҵ� ������
        if(index == -1)
        {
            pMeterFileAll->MeterFile[pMeterFileAll->MeterCount] = gMeterFile;
            pMeterFileAll->MeterCount++;
			DB104_FMT_TRACE("meter_file_change() ���뵵���޸����� ���ӵ����ɹ�! \n");
        }
    }
    DB104_FMT_TRACE("meter_file_change() �޸�֮�󵵰�����=%d \n", pMeterFileAll->MeterCount);
    //pthread_rwlock_unlock(&pMeterFileAll->rwLock);

}


/*******************************************************************************
* ��������: files_update_event_deal
* ��������: �������ĵ��������¼�������
* �������: ��
* �������: ��
* �� �� ֵ: ��
*******************************************************************************/
int files_update_event_deal(DB_UPDATE_MSG_T *pUpdateMsg, METER_FILE_T *pMeterFileAll)
{
	int ret = 0;
	OOP_METER_T stMeterFile;
	
	//0xffff��ʾ���е���ɾ��
    if(pUpdateMsg->infoNum == 0xffff)
    {
        DB104_FMT_TRACE("files_update_event_deal() �������! \n");
        pMeterFileAll->MeterCount = 0;
        memset(pMeterFileAll->MeterFile, 0x00, sizeof(OOP_METER_T)*METER_MAX_NUM);
    }
	else
    {
        memset(&stMeterFile, 0x00, sizeof(OOP_METER_T));
        ret = meter_file_by_pn_read(pUpdateMsg->infoNum, &stMeterFile);
        DB104_FMT_TRACE("files_update_event_deal() ��ȡ����: infoNum=%d, ret=%d !\n", pUpdateMsg->infoNum, ret);
        if(ret > 0)
        {
            //��ȡ��������Ϣ ��ʾ���������������
            meter_file_change(pMeterFileAll, 1, stMeterFile);
        }
        else
        {
            //δ��ȡ��������Ϣ����ʾ����ɾ��
            stMeterFile.nIndex = pUpdateMsg->infoNum;
            meter_file_change(pMeterFileAll, 0, stMeterFile);
        }
    }
	return 1;
}

/*******************************************************************************
* ��������: past_time_init
* ��������: ʱ��������ʼ��
* �������: ��
* �������: ��
* �� �� ֵ: ��
*******************************************************************************/
void past_time_init()
{
    cur_bin_time_get(&g_stCurBinTime);
    memcpy(&g_stPreBinTime, &g_stCurBinTime, sizeof(DATE_TIME_T));
}

/*******************************************************************************
* ��������: past_time_init_down
* ��������: ʱ��������ʼ��
* �������: ��
* �������: ��
* �� �� ֵ: ��
*******************************************************************************/
void past_time_init_down()
{
    cur_bin_time_get(&g_stDownCurBinTime);
    memcpy(&g_stDownPreBinTime, &g_stDownCurBinTime, sizeof(DATE_TIME_T));
}

/*******************************************************************************
* ��������: TimeManage
* ��������: ʱ�������
* �������: ��
* �������: ��
* �� �� ֵ: ��
*******************************************************************************/
void time_manage()
{
    g_unPastTimeFlag.Word = 0;                   //���ʱ���־

    cur_bin_time_get(&g_stCurBinTime);              //ˢ�µ�ǰHEX��ʱ��
    cur_bcd_time_get(&g_stCurBcdTime);              //ˢ�µ�ǰBCD��ʱ��


    //����
    if (g_stCurBinTime.Month != g_stPreBinTime.Month)
    {
        //ͬʱ�������¡����ա���ʱ�����֡������־
        g_unPastTimeFlag.BitSect.Month = TRUE;
        g_unPastTimeFlag.BitSect.Day = TRUE;
        g_unPastTimeFlag.BitSect.Hour = TRUE;
        g_unPastTimeFlag.BitSect.Minute = TRUE;
        g_unPastTimeFlag.BitSect.Second = TRUE;
    }
    //����
    if (g_stCurBinTime.Day != g_stPreBinTime.Day)
    {
        //ͬʱ�������ա���ʱ�����ֱ�־
        g_unPastTimeFlag.BitSect.Day = TRUE;
        g_unPastTimeFlag.BitSect.Hour = TRUE;
        g_unPastTimeFlag.BitSect.Minute = TRUE;
        g_unPastTimeFlag.BitSect.Second = TRUE;
    }
    //��ʱ
    else if (g_stCurBinTime.Hour != g_stPreBinTime.Hour)
    {
        //ͬʱ������ʱ�����ֱ�־
        g_unPastTimeFlag.BitSect.Hour = TRUE;
        g_unPastTimeFlag.BitSect.Minute = TRUE;
        g_unPastTimeFlag.BitSect.Second = TRUE;
    }
    //����
    else if (g_stCurBinTime.Minute != g_stPreBinTime.Minute)
    {
        //�������ֱ�־
        g_unPastTimeFlag.BitSect.Minute = TRUE;
        g_unPastTimeFlag.BitSect.Second = TRUE;
    }

    //����
    else if (g_stCurBinTime.Second != g_stPreBinTime.Second)
    {
        g_unPastTimeFlag.BitSect.Second = TRUE;
    }
    else
    {
        usleep(10000);
    }

    memcpy(&g_stPreBinTime, &g_stCurBinTime, sizeof(DATE_TIME_T));

}

/*******************************************************************************
* ��������: TimeManage
* ��������: ʱ�������
* �������: ��
* �������: ��
* �� �� ֵ: ��
*******************************************************************************/
void time_manage_down()
{
    g_unDownPastTimeFlag.Word = 0;                   //���ʱ���־

    cur_bin_time_get(&g_stDownCurBinTime);              //ˢ�µ�ǰHEX��ʱ��
    cur_bcd_time_get(&g_stDownCurBcdTime);              //ˢ�µ�ǰBCD��ʱ��


    //����
    if (g_stDownCurBinTime.Month != g_stDownPreBinTime.Month)
    {
        //ͬʱ�������¡����ա���ʱ�����֡������־
        g_unDownPastTimeFlag.BitSect.Month = TRUE;
        g_unDownPastTimeFlag.BitSect.Day = TRUE;
        g_unDownPastTimeFlag.BitSect.Hour = TRUE;
        g_unDownPastTimeFlag.BitSect.Minute = TRUE;
        g_unDownPastTimeFlag.BitSect.Second = TRUE;
    }
    //����
    if (g_stDownCurBinTime.Day != g_stDownPreBinTime.Day)
    {
        //ͬʱ�������ա���ʱ�����ֱ�־
        g_unDownPastTimeFlag.BitSect.Day = TRUE;
        g_unDownPastTimeFlag.BitSect.Hour = TRUE;
        g_unDownPastTimeFlag.BitSect.Minute = TRUE;
        g_unDownPastTimeFlag.BitSect.Second = TRUE;
    }
    //��ʱ
    else if (g_stDownCurBinTime.Hour != g_stDownPreBinTime.Hour)
    {
        //ͬʱ������ʱ�����ֱ�־
        g_unDownPastTimeFlag.BitSect.Hour = TRUE;
        g_unDownPastTimeFlag.BitSect.Minute = TRUE;
        g_unDownPastTimeFlag.BitSect.Second = TRUE;
    }
    //����
    else if (g_stDownCurBinTime.Minute != g_stDownPreBinTime.Minute)
    {
        //�������ֱ�־
        g_unDownPastTimeFlag.BitSect.Minute = TRUE;
        g_unDownPastTimeFlag.BitSect.Second = TRUE;
    }

    //����
    else if (g_stDownCurBinTime.Second != g_stDownPreBinTime.Second)
    {
        g_unDownPastTimeFlag.BitSect.Second = TRUE;
    }
    else
    {
        usleep(10000);
    }

    memcpy(&g_stDownPreBinTime, &g_stDownCurBinTime, sizeof(DATE_TIME_T));

}

/*******************************************************************************
* ��������: TimeManage
* ��������: ʱ�������
* �������: ��
* �������: ��
* �� �� ֵ: ��
*******************************************************************************/
void time_manage_mt()
{
    g_eMtPastTimeFlag.Word = 0;                   //���ʱ���־

    cur_bin_time_get(&g_stMtCurBinTime);              //ˢ�µ�ǰHEX��ʱ��
    cur_bcd_time_get(&g_stMtCurBcdTime);              //ˢ�µ�ǰBCD��ʱ��


    //����
    if (g_stMtCurBinTime.Month != g_stMtPreBinTime.Month)
    {
        //ͬʱ�������¡����ա���ʱ�����֡������־
        g_eMtPastTimeFlag.BitSect.Month = TRUE;
        g_eMtPastTimeFlag.BitSect.Day = TRUE;
        g_eMtPastTimeFlag.BitSect.Hour = TRUE;
        g_eMtPastTimeFlag.BitSect.Minute = TRUE;
        g_eMtPastTimeFlag.BitSect.Second = TRUE;
    }
    //����
    if (g_stMtCurBinTime.Day != g_stMtPreBinTime.Day)
    {
        //ͬʱ�������ա���ʱ�����ֱ�־
        g_eMtPastTimeFlag.BitSect.Day = TRUE;
        g_eMtPastTimeFlag.BitSect.Hour = TRUE;
        g_eMtPastTimeFlag.BitSect.Minute = TRUE;
        g_eMtPastTimeFlag.BitSect.Second = TRUE;
    }
    //��ʱ
    else if (g_stMtCurBinTime.Hour != g_stMtPreBinTime.Hour)
    {
        //ͬʱ������ʱ�����ֱ�־
        g_eMtPastTimeFlag.BitSect.Hour = TRUE;
        g_eMtPastTimeFlag.BitSect.Minute = TRUE;
        g_eMtPastTimeFlag.BitSect.Second = TRUE;
    }
    //����
    else if (g_stMtCurBinTime.Minute != g_stMtPreBinTime.Minute)
    {
        //�������ֱ�־
        g_eMtPastTimeFlag.BitSect.Minute = TRUE;
        g_eMtPastTimeFlag.BitSect.Second = TRUE;
    }

    //����
    else if (g_stMtCurBinTime.Second != g_stMtPreBinTime.Second)
    {
        g_eMtPastTimeFlag.BitSect.Second = TRUE;
    }
    else
    {
        usleep(10000);
    }

    memcpy(&g_stMtPreBinTime, &g_stMtCurBinTime, sizeof(DATE_TIME_T));

}

/*******************************************************************************
* ��������: data_block_split
* ��������: ���ݿ���
* �������: ��
* �������: ��
* �� �� ֵ: ��
*******************************************************************************/
int data_block_split(uint32 nOAD, uint8 nItemID, uint8 *buff, float32 *fOut)
{
	uint16 nNum = 0;

	switch(nOAD)
	{
		case 0x00100200:			//(��ǰ)�����й������ܼ��ƽ��ʾֵ
        case 0x00200200:			//(��ǰ)�����й������ܼ��ƽ��ʾֵ
        case 0x00300200:			//(��ǰ)�����޹������ܼ��ƽ��ʾֵ
        case 0x00400200:			//(��ǰ)�����޹������ܼ��ƽ��ʾֵ
        case 0x00500200:			//(��ǰ)��һ�����޹��ܼ��ƽ�ȵ���ʾֵ
        case 0x00600200:			//(��ǰ)�ڶ������޹��ܼ��ƽ�ȵ���ʾֵ
        case 0x00700200:			//(��ǰ)���������޹��ܼ��ƽ�ȵ���ʾֵ
        case 0x00800200:			//(��ǰ)���������޹��ܼ��ƽ�ȵ���ʾֵ
        {
			OOP_ENERGY_T Energy = {0};
			memcpy(&Energy, buff, sizeof(OOP_ENERGY_T));
			nNum = Energy.nNum;
			if(nItemID >= nNum) 
			{
				return -1;
			}
			*fOut = (float32)Energy.nValue[nItemID];
		}
            break;
		case 0x20000200:			//(��ǰ)�����ѹ
		case 0x260B0200:			//����������
		{
			OOP_WORD3V_T Voltage = {0};
			memcpy(&Voltage, buff, sizeof(OOP_WORD3V_T));
			nNum = Voltage.nNum;
			if(nItemID >= nNum) 
			{
				return -1;
			}
			*fOut = (float32)Voltage.nValue[nItemID];
		}
			break;
		case 0x20010200:			//(��ǰ)�������
		case 0x260A0200:			//�������ֲ�����
		{
			OOP_INT3V_T Current = {0};
			memcpy(&Current, buff, sizeof(OOP_INT3V_T));
			nNum = Current.nNum;
			if(nItemID >= nNum) 
			{
				return -1;
			}
			*fOut = (float32)Current.nValue[nItemID];
		}
			break;
		case 0x20010400:			//(��ǰ)�������
		{
			OOP_WORD3V_T Current = {0};
			memcpy(&Current, buff, sizeof(OOP_WORD3V_T));
			nNum = 1;
			if(nItemID >= nNum) 
			{
				return -1;
			}
			*fOut = (float32)Current.nValue[nItemID];
		}
			break;
		case 0x20040200:			//(��ǰ)���й�����������
		{
			OOP_INT4V_T ValueP = {0};
			memcpy(&ValueP, buff, sizeof(OOP_INT4V_T));
			nNum = ValueP.nNum;
			if(nItemID >= nNum) 
			{
				return -1;
			}
			*fOut = (float32)ValueP.nValue[nItemID];
		}
			break;
		case 0x20050200:			//(��ǰ)���޹�����������
		{
			OOP_INT4V_T ValueQ = {0};
			memcpy(&ValueQ, buff, sizeof(OOP_INT4V_T));
			nNum = ValueQ.nNum;
			if(nItemID >= nNum) 
			{
				return -1;
			}
			*fOut = (float32)ValueQ.nValue[nItemID];
		}
			break;
		case 0x20060200:			//(��ǰ)�����ڹ���������
		{
			OOP_INT4V_T ValueS = {0};
			memcpy(&ValueS, buff, sizeof(OOP_INT4V_T));
			nNum = ValueS.nNum;
			if(nItemID >= nNum) 
			{
				return -1;
			}
			*fOut = (float32)ValueS.nValue[nItemID];
		}
			break;
		case 0x200F0200:			//(��ǰ)����Ƶ��
		{
			uint32 Frequency;
			memcpy(&Frequency, buff, sizeof(uint32));
			nNum = 1;
			if(nItemID >= nNum) 
			{
				return -1;
			}
			*fOut = (float32)Frequency;
		}
			break;
		case 0x200A0200:			//(��ǰ)��������
		{
			OOP_LONG4V_T Pf = {0};
			memcpy(&Pf, buff, sizeof(OOP_LONG4V_T));
			nNum = Pf.nNum;
			if(nItemID >= nNum) 
			{
				return -1;
			}
			*fOut = (float32)Pf.nValue[nItemID];
		}
			break;
		case 0x2A000200:			//������Ͷ�д���
		{
			OOP_DWORD3V_T Pf = {0};
			memcpy(&Pf, buff, sizeof(OOP_DWORD3V_T));
			nNum = Pf.nNum;
			if(nItemID >= nNum) 
			{
				return -1;
			}
			*fOut = (float32)Pf.nValue[nItemID];
		}
			break;
		
		default:
			return -1;
            break;
	}

	return 1;
}

/*******************************************************************************
* ��������: para_query_float
* ��������: ��ֵ������ѯ
* �������: ��
* �������: ��
* �� �� ֵ: ��
*******************************************************************************/
int para_query_int64(uint16 nParaAddr, int64 *nOut)
{
	int i = 0;
	int64   nParaValue = 0;
	BOOL    bSuccess   = FALSE;

	for(i=0; i<g_nConfigParaNum; i++)
	{
		if(g_aParaSetValue[g_nFixedValueArea][i].nObjAddr == nParaAddr)
		{
			switch(g_aParaSetValue[g_nFixedValueArea][i].nTagType)
			{
				case TAG_NONE:
				{
					nParaValue = (int64)g_aParaSetValue[g_nFixedValueArea][i].fDefaultValue;
				}
					break;
				case TAG_BOOL:
				{
					nParaValue = (int64)g_aParaSetValue[g_nFixedValueArea][i].unValue.boolV;
				}
					break;
				case TAG_INT8:
				{
					nParaValue = (int64)g_aParaSetValue[g_nFixedValueArea][i].unValue.int8V;
				}
					break;
				case TAG_UINT8:
				{
					nParaValue = (int64)g_aParaSetValue[g_nFixedValueArea][i].unValue.uint8V;
				}
					break;
				case TAG_INT16:
				{
					nParaValue = (int64)g_aParaSetValue[g_nFixedValueArea][i].unValue.int16V;
				}
					break;
				case TAG_UINT16:
				{
					nParaValue = (int64)g_aParaSetValue[g_nFixedValueArea][i].unValue.uint16V;
				}
					break;
				case TAG_INT32:
				{
					nParaValue = (int64)g_aParaSetValue[g_nFixedValueArea][i].unValue.int32V;
				}
					break;
				case TAG_UINT32:
				{
					nParaValue = (int64)g_aParaSetValue[g_nFixedValueArea][i].unValue.uint32V;
				}
					break;
				case TAG_INT64:
				{
					nParaValue = (int64)g_aParaSetValue[g_nFixedValueArea][i].unValue.int64V;
				}
					break;
				case TAG_UINT64:
				{
					nParaValue = (int64)g_aParaSetValue[g_nFixedValueArea][i].unValue.uint64V;
				}
					break;
				case TAG_FLOAT32:
				{
					nParaValue = (int64)g_aParaSetValue[g_nFixedValueArea][i].unValue.float32V;
				}
					break;
				case TAG_FLOAT64:
				{
					nParaValue = (int64)g_aParaSetValue[g_nFixedValueArea][i].unValue.float64V;
				}
					break;
				default:
					nParaValue = (int64)g_aParaSetValue[g_nFixedValueArea][i].fDefaultValue;
		            break;
			}
			
			bSuccess = TRUE;
			break;
		}
	}

	if(bSuccess)
	{
		*nOut = nParaValue;
		return 1;
	}
	else
	{
		return -1;
	}
}


/*******************************************************************************
* ��������: para_query_float
* ��������: ��ֵ������ѯ
* �������: ��
* �������: ��
* �� �� ֵ: ��
*******************************************************************************/
int para_query_float32(uint16 nParaAddr, float32 *fOut)
{
	int i = 0;
	float32 fParaValue = 0.0;
	BOOL    bSuccess   = FALSE;

	for(i=0; i<g_nConfigParaNum; i++)
	{
		//DB104_FMT_TRACE("para_query_float32() nParaAddr=%04x, g_aParaSetValue[%d][%d].nObjAddr=%04x \n", nParaAddr, g_nFixedValueArea, i, g_aParaSetValue[g_nFixedValueArea][i].nObjAddr);
		if(g_aParaSetValue[g_nFixedValueArea][i].nObjAddr == nParaAddr)
		{
			switch(g_aParaSetValue[g_nFixedValueArea][i].nTagType)
			{
				case TAG_NONE:
				{
					fParaValue = (float32)g_aParaSetValue[g_nFixedValueArea][i].fDefaultValue;
				}
					break;
				case TAG_BOOL:
				{
					fParaValue = (float32)g_aParaSetValue[g_nFixedValueArea][i].unValue.boolV;
				}
					break;
				case TAG_INT8:
				{
					fParaValue = (float32)g_aParaSetValue[g_nFixedValueArea][i].unValue.int8V;
				}
					break;
				case TAG_UINT8:
				{
					fParaValue = (float32)g_aParaSetValue[g_nFixedValueArea][i].unValue.uint8V;
				}
					break;
				case TAG_INT16:
				{
					fParaValue = (float32)g_aParaSetValue[g_nFixedValueArea][i].unValue.int16V;
				}
					break;
				case TAG_UINT16:
				{
					fParaValue = (float32)g_aParaSetValue[g_nFixedValueArea][i].unValue.uint16V;
				}
					break;
				case TAG_INT32:
				{
					fParaValue = (float32)g_aParaSetValue[g_nFixedValueArea][i].unValue.int32V;
				}
					break;
				case TAG_UINT32:
				{
					fParaValue = (float32)g_aParaSetValue[g_nFixedValueArea][i].unValue.uint32V;
				}
					break;
				case TAG_INT64:
				{
					fParaValue = (float32)g_aParaSetValue[g_nFixedValueArea][i].unValue.int64V;
				}
					break;
				case TAG_UINT64:
				{
					fParaValue = (float32)g_aParaSetValue[g_nFixedValueArea][i].unValue.uint64V;
				}
					break;
				case TAG_FLOAT32:
				{
					fParaValue = (float32)g_aParaSetValue[g_nFixedValueArea][i].unValue.float32V;
				}
					break;
				case TAG_FLOAT64:
				{
					fParaValue = (float32)g_aParaSetValue[g_nFixedValueArea][i].unValue.float64V;
				}
					break;
				default:
					fParaValue = (float32)g_aParaSetValue[g_nFixedValueArea][i].fDefaultValue;
		            break;
			}

			DB104_FMT_TRACE("para_query_float32() bSuccess=TRUE g_aParaSetValue[%d][%d].nTagType=%d, fParaValue=%f, arrayV: \n", 
				g_nFixedValueArea, i, g_aParaSetValue[g_nFixedValueArea][i].nTagType, fParaValue);
			DB104_BUF_TRACE(&g_aParaSetValue[g_nFixedValueArea][i].unValue.arrayV[0], MAX_FIXED_VALUE_LEN, "����: ");
			bSuccess = TRUE;
			break;
		}
	}

	if(bSuccess)
	{
		*fOut = fParaValue;
		return 1;
	}
	else
	{
		return -1;
	}
}

/*******************************************************************************
* ��������: yc_query_float32
* ��������: ң��ʵʱֵ��ѯ
* �������: ��
* �������: ��
* �� �� ֵ: ��
*******************************************************************************/
int yc_query_float32(uint16 nParaAddr, float32 *fOut)
{
	int i = 0;
	float32 fParaValue = 0.0;
	BOOL    bSuccess   = FALSE;

	for(i=0; i<g_nConfigYcNum; i++)
	{
		if(g_aYcDataConfig[i].nObjAddr == nParaAddr)
		{
			fParaValue = g_aYcDataSend[i].fNewValue;
			bSuccess = TRUE;
			
			DB104_FMT_TRACE("yc_query_float32() bSuccess=TRUE i=%d, g_aYcDataSend[i].fNewValue=%f, fParaValue=%f \n", 
				i, g_aYcDataSend[i].fNewValue, fParaValue);
			
			break;
		}
	}

	if(bSuccess)
	{
		*fOut = fParaValue;
		return 1;
	}
	else
	{
		return -1;
	}
}


/*******************************************************************************
* ��������: system_time_get
* ��������: ȡϵͳʱ��
* �������: ��
* �������: ��
* �� �� ֵ: ��
*******************************************************************************/
void system_time_get(uint8 *aTime)
{
	struct timeval c_time;
	SYSTEM_BCD_TIME_T  sSysBcdTime;  //ϵͳʱ��
	uint16  dtemp = 0;

	gettimeofday(&c_time, NULL);
	sys_bcd_time_get(&sSysBcdTime);
	dtemp = ((uint16)(bcd_to_hex(sSysBcdTime.ss)))*1000 + c_time.tv_usec/1000;

	aTime[0] =  dtemp&0xff;
	aTime[1] =  dtemp>>8;
	aTime[2] =  (bcd_to_hex(sSysBcdTime.nn))&0x3f;//����λ����
	aTime[3] =  (bcd_to_hex(sSysBcdTime.hh))&0x1f;
	aTime[4] =  (bcd_to_hex(sSysBcdTime.dd))&0x1f;
	if(sSysBcdTime.ww == 0)
		aTime[4] |= (7<<5);
	else
		aTime[4] |= (sSysBcdTime.ww<<5);			
	aTime[5] =  (bcd_to_hex(sSysBcdTime.mm))&0x0f;
	aTime[6] =  (bcd_to_hex(sSysBcdTime.yy))&0x7f;
}

/*******************************************************************************
* ��������: ratio_convert
* ��������: ��ʵֵ-OADֵ��OADֵ-��ʵֵ���任ϵ��
* �������: ��
* �������: ��
* �� �� ֵ: ��
*******************************************************************************/
const Map645ToOop* oad_info_find(uint32 oad)
{
	const Map645ToOop *pMap645ToOop = NULL;

	pMap645ToOop = g_aMap645ToOopRT;//ʵʱ����
	
	if(pMap645ToOop==NULL)
	{
		return NULL;
	}  

	while(pMap645ToOop->Oad.value != 0xFFFFFFFF)
	{
		if(pMap645ToOop->Oad.value == oad) 
		{
			return pMap645ToOop;
		}
		pMap645ToOop++;
	}
	
	return NULL;
}

/*******************************************************************************
* ��������: meter_addr_query
* ��������: ���ݲ�����Ų�ѯ�������ַ
* �������: ��
* �������: ��
* �� �� ֵ: ��
*******************************************************************************/
int meter_addr_query(uint16 Pn, uint8 *pAddr, uint8 nAddrLen)
{
	int i = 0;
	int ret = -1;
	int len = nAddrLen;
	if(len > 6)
	{
		len = 6;
	}
    //pthread_rwlock_wrlock(&g_sMeterFileAll.rwLock);
    for(i=0; i<g_sMeterFileAll.MeterCount; i++)
    {
        if(Pn == g_sMeterFileAll.MeterFile[i].nIndex)
        {
            memcpy(pAddr, g_sMeterFileAll.MeterFile[i].basic.tsa.add, len);
            ret = 1;
			break;
        }
    }
    //pthread_rwlock_unlock(&g_sMeterFileAll.rwLock);

	return ret;
}

/*******************************************************************************
* ��������: yx_db_center_read
* ��������: ���������Ķ�ȡң��ʵʱ����
* �������: uint16 index �������
* �������: ��
* �� �� ֵ: ��
*******************************************************************************/
void yx_db_center_read(uint16 index)
{
	if(index >= g_nConfigYxNum)
	{
		return;
	}

	OOP_SWITCHIN_STATE_T SwitchState;  //ң��״̬�ṹ��
	memset(&SwitchState, 0x00, sizeof(OOP_SWITCHIN_STATE_T));
	
	uint8	aRecvBuff[500] = {0};
	uint32	nRevcBuffLen = 0;
	int 	ret  = 0;
	NOMAL_OAD_T NomalOad;

	NomalOad.logicId = 0;  //�߼���ַ
	NomalOad.infoNum = 0;  //��Ϣ��

	//ң�����ݶ�ȡ
	NomalOad.oad.value = g_aYxDataConfig[index].nDbCenterOAD;  //��ͨOAD
	NomalOad.classtag = CLASS_DATA_NORMAL;

	ret = db_read_nomal(g_sDataCenterClient, &NomalOad, sizeof(aRecvBuff), (uint8*)aRecvBuff, &nRevcBuffLen);
	DB104_FMT_TRACE("<--��%d��ң��--> yx_db_center_read() ret = nomal_data_read ret:%d \n", index, ret);
	if(ret == 0)
	{
		DB104_FMT_TRACE("yx_db_center_read() ��ȡң�����ݳɹ�  NomalOad.oad=%08x \n", NomalOad.oad.value);
		DB104_BUF_TRACE(aRecvBuff, nRevcBuffLen, "����: ");

		//��ȡ�����������ݿ�(uint8)����Ҫ���з���(��λ)����
		memcpy(&SwitchState, aRecvBuff, nRevcBuffLen);
		
		g_aYxDataSend[index].nNewValue = SwitchState.state[g_aYxDataConfig[index].nItemIndex].st;
		if(g_aYxDataSend[index].nNewValue != g_aYxDataSend[index].nHisValue)
		{
			g_aYxDataSend[index].bChangeFlag = 1;
		}
		
		//��ʱ��ֵ, ���Ա仯����
		if(g_bTestEnable)
		{
			g_aYxDataSend[index].nNewValue = rand()%2;//��2ȡ��õ�����0����1
			g_aYxDataSend[index].bChangeFlag = 1;
			DB104_FMT_TRACE("ң������� g_aYxDataSend[%d].nNewValue=%d \n", index, g_aYxDataSend[index].nNewValue);
		}
		
		if(g_aYxDataSend[index].bChangeFlag)
		{
			g_bYxChanged = TRUE;
			g_aYxDataSend[index].bSendFlag = FALSE;
		}

		DB104_FMT_TRACE("g_aYxDataSend[%d]: nNewValue=%d, nHisValue=%d, bChangeFlag=%d, bSendFlag=%d, ȫ��g_bYxChanged=%d \n", 
			index, g_aYxDataSend[index].nNewValue, g_aYxDataSend[index].nHisValue, g_aYxDataSend[index].bChangeFlag, 
			g_aYxDataSend[index].bSendFlag, g_bYxChanged);
	}
	else
	{
		DB104_FMT_TRACE("yx_db_center_read() ��ȡ����  NomalOad.oad=%08x \n", NomalOad.oad.value);
		DB104_FMT_TRACE("yx_db_center_read() ң�����ݶ�ȡʧ��! \n");
	}
	
	system_time_get(&g_aYxDataSend[index].btime[0]);

}

/*******************************************************************************
* ��������: yx_real_signal_read
* ��������: ͨ��mqtt��Ϣ��ȡӲң���ź�
* �������: uint16 index �������
* �������: ��
* �� �� ֵ: ��
*******************************************************************************/
void yx_real_signal_read(uint16 index)
{
	if(index >= g_nConfigYxNum)
	{
		return;
	}

	RST_RMT_T stRmt;
	uint8 status = 0;

	memset(&stRmt, 0x00, sizeof(RST_RMT_T));
	status = advPd_get_rmt(&stRmt);

	//���ݸ�������
	g_aYxDataSend[index].nNewValue = (uint8)( (stRmt.ST >> g_aYxDataConfig[index].nItemIndex)&0x01 );
	if(g_aYxDataSend[index].nNewValue != g_aYxDataSend[index].nHisValue)
	{
		g_aYxDataSend[index].bChangeFlag = 1;
	}
	
	//��ʱ��ֵ, ���Ա仯����
	if(g_bTestEnable)
	{
		g_aYxDataSend[index].nNewValue = rand()%2;//��2ȡ��õ�����0����1
		g_aYxDataSend[index].bChangeFlag = 1;
		DB104_FMT_TRACE("ң������� g_aYxDataSend[%d].nNewValue=%d \n", index, g_aYxDataSend[index].nNewValue);
	}
	
	if(g_aYxDataSend[index].bChangeFlag)
	{
		g_bYxChanged = TRUE;
		g_aYxDataSend[index].bSendFlag = FALSE;
	}

	DB104_FMT_TRACE("Ӳң��g_aYxDataSend[%d]: nNewValue=%d, nHisValue=%d, bChangeFlag=%d, bSendFlag=%d, ȫ��g_bYxChanged=%d, status=0x%02x \n", 
		index, g_aYxDataSend[index].nNewValue, g_aYxDataSend[index].nHisValue, g_aYxDataSend[index].bChangeFlag, 
		g_aYxDataSend[index].bSendFlag, g_bYxChanged, status);

	system_time_get(&g_aYxDataSend[index].btime[0]);
}

/*******************************************************************************
* ��������: yx_calculate_sta_set
* ��������: �¼�ģ�����Ӧң����λ
* �������: ��
* �������: ��
* �� �� ֵ: ��
*******************************************************************************/
void yx_calculate_sta_set(uint32 subtype, uint8 subindex, uint8 value)
{
	DB104_FMT_TRACE("yx_calculate_sta_set() subtype=0x%08x, subindex=%d, value=%d \n", subtype, subindex, value);
	
	int i = 0;
	int ret = -1;
	int index = 0;
	
	//ң�����ݶ�ȡ
	for(i=0; i<g_nConfigYxNum; i++)
	{
		if(g_aYxDataConfig[i].nDbCenterOAD == 0xFFFFFFFF)
		{
			if((g_aYxDataConfig[i].nDealType == subtype) && (g_aYxDataConfig[i].nItemIndex == subindex))
			{
				index = i;
				ret = 1;
				break;
			}
		}
	}

	if(ret < 0)
	{
		return;
	}

	//���ݸ�������
	g_aYxDataSend[index].nNewValue = value>0? 1: 0;
	if(g_aYxDataSend[index].nNewValue != g_aYxDataSend[index].nHisValue)
	{
		g_aYxDataSend[index].bChangeFlag = 1;
	}
	
	//��ʱ��ֵ, ���Ա仯����
	if(g_bTestEnable)
	{
		g_aYxDataSend[index].nNewValue = rand()%2;//��2ȡ��õ�����0����1
		g_aYxDataSend[index].bChangeFlag = 1;
		DB104_FMT_TRACE("ң������� g_aYxDataSend[%d].nNewValue=%d \n", index, g_aYxDataSend[index].nNewValue);
	}
	
	if(g_aYxDataSend[index].bChangeFlag)
	{
		g_bYxChanged = TRUE;
		g_aYxDataSend[index].bSendFlag = FALSE;
	}

	DB104_FMT_TRACE("g_aYxDataSend[%d]: nNewValue=%d, nHisValue=%d, bChangeFlag=%d, bSendFlag=%d, ȫ��g_bYxChanged=%d \n", 
		index, g_aYxDataSend[index].nNewValue, g_aYxDataSend[index].nHisValue, g_aYxDataSend[index].bChangeFlag, 
		g_aYxDataSend[index].bSendFlag, g_bYxChanged);

	system_time_get(&g_aYxDataSend[index].btime[0]);
}


/*******************************************************************************
* ��������: yx_calculate_statistics
* ��������: ��Ҫͳ�ƻ�����ң���ź�
* �������: uint16 index �������
* �������: ��
* �� �� ֵ: ��
*******************************************************************************/
void yx_calculate_statistics(uint16 index)
{
	if(index >= g_nConfigYxNum)
	{
		return;
	}
}

/*******************************************************************************
* ��������: yc_db_nomal_read
* ��������: ���������Ķ�ȡң��ʵʱ����(��ͨ������)
* �������: uint16 index �������
* �������: ��
* �� �� ֵ: ��
*******************************************************************************/
void yc_db_nomal_read(uint16 index)
{
	if(index >= g_nConfigYcNum)
	{
		return;
	}
	
	uint8   aRecvBuff[500] = {0};
	uint32  nRevcBuffLen = 0;
	int     ret  = 0;
	NOMAL_OAD_T NomalOad = {0};  //��ͨ��OAD

	float32 fValueDB;  //�������������ĵ�ֵ
	float32 fDeadZone;  //����ֵ
	float32 fDiffValue;  //��ֵ
	float32 fRatio = 1;  //����
	const Map645ToOop *pMap645ToOop = NULL;

	//���ݶ�ȡ
	NomalOad.logicId = g_aYcDataConfig[index].nLogicId;  //�߼���ַ
	NomalOad.infoNum = g_aYcDataConfig[index].nInfoNum;  //��Ϣ���
	NomalOad.oad.value = g_aYcDataConfig[index].nDbCenterOAD;  //��ͨOAD
	NomalOad.classtag = CLASS_DATA_NORMAL;

	ret = db_read_nomal(g_sDataCenterClient, &NomalOad, sizeof(aRecvBuff), (uint8*)aRecvBuff, &nRevcBuffLen);
	DB104_FMT_TRACE("<--��%d��ң��--> yc_db_nomal_read() ret = db_read_nomal() ret:%d \n", index, ret);
	
	if(ret == 0)
	{
		DB104_FMT_TRACE("yc_db_nomal_read() ��ȡ���ݳɹ�  g_aYcDataConfig[%d].nDbCenterOAD=%08x \n", 
			index, g_aYcDataConfig[index].nDbCenterOAD);
		DB104_BUF_TRACE(aRecvBuff, nRevcBuffLen, "����: ");

		//��ȡ�����������ݿ飬��Ҫ���з��ദ��
		ret = data_block_split(g_aYcDataConfig[index].nDbCenterOAD, g_aYcDataConfig[index].nItemIndex, aRecvBuff, &fValueDB);
		if(ret > 0)
		{
			//��ȡ����
			pMap645ToOop = oad_info_find(g_aYcDataConfig[index].nDbCenterOAD);
			if(pMap645ToOop == NULL)
			{
				DB104_FMT_TRACE("yc_db_nomal_read() if(pMap645ToOop == NULL) \n");
			}
			else
			{
				fRatio = ratio_convert(pMap645ToOop->DesScaler, E_OOP_TO_REAL);
				DB104_FMT_TRACE("��ȡ���� fRatio=%f \n", fRatio);
			}
			g_aYcDataSend[index].fNewValue = fValueDB*fRatio;
			DB104_FMT_TRACE("���ʴ���֮��g_aYcDataSend[index].fNewValue=%f, ���ʴ���֮ǰfValueDB=%f \n", g_aYcDataSend[index].fNewValue, fValueDB);
			
			//��ȡ�仯����ֵ
			if(para_query_float32(g_aYcDataConfig[index].nDeadZoneAddr, &fDeadZone) < 0)
			{
				DB104_FMT_TRACE("��ȡ�仯����ֵpara_query_float32()ʧ�ܣ�ֱ�Ӹ�Ĭ��ֵ�� \n");
				fDeadZone = g_aYcDataConfig[index].fDeadZoneValue;
			}
			DB104_FMT_TRACE("�仯����ֵfDeadZone=%f \n", fDeadZone);

			//��ʱ��ֵ, ���Ա仯����
			if(g_bTestEnable)
			{
				g_aYcDataSend[index].fHisValue = rand()%100;//100���������
				DB104_FMT_TRACE("ң������� g_aYcDataSend[%d].fHisValue=%f \n", index, g_aYcDataSend[index].fHisValue);
			}

			//�ж�ʵʱֵ�Ƿ�Խ������
			fDiffValue = fabs(g_aYcDataSend[index].fNewValue - g_aYcDataSend[index].fHisValue);
			if(fDiffValue > fDeadZone)
			{
				DB104_FMT_TRACE("�仯ֵfDiffValue=%f, ��������ֵ, fNewValue=%f, fHisValue=%f ��Ҫ�����ϱ�! \n", 
					fDiffValue, g_aYcDataSend[index].fNewValue, g_aYcDataSend[index].fHisValue);
				g_bYcChanged = TRUE;
				g_aYcDataSend[index].bChangeFlag = TRUE;
				g_aYcDataSend[index].bSendFlag = FALSE;
			}
			else
			{
				DB104_FMT_TRACE("�仯ֵfDiffValue=%f, С������ֵ, fNewValue=%f, fHisValue=%f \n", 
					fDiffValue, g_aYcDataSend[index].fNewValue, g_aYcDataSend[index].fHisValue);
			}
			
		}
		else
		{
			DB104_FMT_TRACE("yc_db_nomal_read() ��������ʧ��  g_aYcDataConfig[%d].nDbCenterOAD=%08x \n", 
				index, g_aYcDataConfig[index].nDbCenterOAD);
		}
	}
	else
	{
		DB104_FMT_TRACE("yc_db_nomal_read() ��ȡ����  g_aYcDataConfig[%d].nDbCenterOAD=%08x \n", 
			index, g_aYcDataConfig[index].nDbCenterOAD);
		DB104_FMT_TRACE("yc_db_nomal_read() ���ݶ�ȡʧ��! \n");
	}
	
	system_time_get(&g_aYcDataSend[index].btime[0]);
}

/*******************************************************************************
* ��������: yc_db_record_read
* ��������: ���������Ķ�ȡң��ʵʱ����(��¼������)
* �������: uint16 index �������
* �������: ��
* �� �� ֵ: ��
*******************************************************************************/
void yc_db_record_read(uint16 index)
{
	if(index >= g_nConfigYcNum)
	{
		return;
	}

	uint8	aRecvBuff[500] = {0};
	uint32	nRevcBuffLen = 0;
	int 	ret  = 0;
	READ_RECORD_T ReadRecord;  //��¼��OAD
	RECORD_UNIT_T RecordOut;  //��¼�Ͳ�ѯ���
	memset(&ReadRecord, 0, sizeof(READ_RECORD_T));
	memset(&RecordOut, 0, sizeof(RECORD_UNIT_T));
	
	time_t nRealTime_t;  //��ǰϵͳʱ���ת��������

	float32 fValueDB;  //�������������ĵ�ֵ
	float32 fDeadZone;	//����ֵ
	float32 fDiffValue;  //��ֵ
	float32 fRatio = 1;  //����
	const Map645ToOop *pMap645ToOop = NULL;

	//���ݶ�ȡ
	nRealTime_t = time(NULL);
	
	ReadRecord.recordOAD.logicId = g_aYcDataConfig[index].nLogicId;  //�߼���ַ
	ReadRecord.recordOAD.infoNum = g_aYcDataConfig[index].nInfoNum;  //��Ϣ���
	ReadRecord.recordOAD.road.oadMain.value = g_aYcDataConfig[index].nDbCenterOAD;  //��OAD
	ReadRecord.recordOAD.road.oadCols.nNum = g_aYcDataConfig[index].nOadColsNum;  //��OAD����
	memcpy(&ReadRecord.recordOAD.road.oadCols.oad[0], &g_aYcDataConfig[index].aOadCols[0],
		g_aYcDataConfig[index].nOadColsNum*sizeof(uint32));  //������OAD
	ReadRecord.recordOAD.classtag = CLASS_DATA_COLLEC;

	ReadRecord.cType = COL_TM_STORE;  //�ɼ��洢ʱ��
	//ReadRecord.cStart = nRealTime_t-24*3600;  //ʱ����ʼ�㣬��ǰʱ����ǰһ��
	ReadRecord.cStart = 0;  //ʱ����ʼ�㣬��ǰʱ����ǰһ��
	ReadRecord.cEnd = nRealTime_t;  //ʱ�������
	ReadRecord.sortType = DOWN_SORT;  //����ʽ ����
	ReadRecord.MAC.nNum = 6;  //����У�� ����
	meter_addr_query(g_aYcDataConfig[index].nInfoNum, ReadRecord.MAC.value, ReadRecord.MAC.nNum);  //����У�� ����ַ
	ReadRecord.ti.unit = TI_HOUR;  //ʱ���� ��λ
	ReadRecord.ti.value = 15;  //ʱ���� ֵ

	DB104_FMT_TRACE("<--��%d��ң��--> yc_db_record_read() �����±�index=%d, �߼���ַlogicId=%d, ��Ϣ���infoNum=%d, ��OAD=%08x, ��OAD����=%d, ��OAD=%08x, У��MAC.nNum=%d, MAC.value=%02x%02x%02x%02x%02x%02x \n", 
		index, index, ReadRecord.recordOAD.logicId, ReadRecord.recordOAD.infoNum, ReadRecord.recordOAD.road.oadMain.value, g_aEnergyDataConfig[index].nOadColsNum, 
		ReadRecord.recordOAD.road.oadCols.oad[0].value, ReadRecord.MAC.nNum, ReadRecord.MAC.value[5], ReadRecord.MAC.value[4], ReadRecord.MAC.value[3], ReadRecord.MAC.value[2], ReadRecord.MAC.value[1], ReadRecord.MAC.value[0]);
	
	ret = db_read_record(g_sDataCenterClient, &ReadRecord, sizeof(aRecvBuff), &RecordOut);
	DB104_FMT_TRACE("yc_db_center_read() ret = db_read_record() ret:%d \n", ret);
	if(ret == 0)
	{
		nRevcBuffLen = RecordOut.record[0].outDatalen;
		memcpy(&aRecvBuff[0], &RecordOut.record[0].outDataBuf[0], nRevcBuffLen);

		DB104_FMT_TRACE("yc_db_record_read() ��ȡ���ݳɹ�  g_aYcDataConfig[%d].nDbCenterOAD=%08x \n", 
			index, g_aYcDataConfig[index].nDbCenterOAD);
		DB104_BUF_TRACE(aRecvBuff, nRevcBuffLen, "����: ");

		//��ȡ�����������ݿ飬��Ҫ���з��ദ��
		ret = data_block_split(g_aYcDataConfig[index].aOadCols[0], g_aYcDataConfig[index].nItemIndex, aRecvBuff, &fValueDB);
		if(ret > 0)
		{
			//��ȡ����
			pMap645ToOop = oad_info_find(g_aYcDataConfig[index].aOadCols[0]);
			if(pMap645ToOop == NULL)
			{
				DB104_FMT_TRACE("yc_db_record_read() if(pMap645ToOop == NULL) \n");
			}
			else
			{
				fRatio = ratio_convert(pMap645ToOop->DesScaler, E_OOP_TO_REAL);
				DB104_FMT_TRACE("��ȡ���� fRatio=%f \n", fRatio);
			}
			g_aYcDataSend[index].fNewValue = fValueDB*fRatio;
			DB104_FMT_TRACE("���ʴ���֮��g_aYcDataSend[index].fNewValue=%f, ���ʴ���֮ǰfValueDB=%f \n", g_aYcDataSend[index].fNewValue, fValueDB);
			
			//��ȡ�仯����ֵ
			if(para_query_float32(g_aYcDataConfig[index].nDeadZoneAddr, &fDeadZone) < 0)
			{
				DB104_FMT_TRACE("��ȡ�仯����ֵpara_query_float32()ʧ�ܣ�ֱ�Ӹ�Ĭ��ֵ�� \n");
				fDeadZone = g_aYcDataConfig[index].fDeadZoneValue;
			}
			DB104_FMT_TRACE("�仯����ֵfDeadZone=%f \n", fDeadZone);

			//��ʱ��ֵ, ���Ա仯����
			if(g_bTestEnable)
			{
				g_aYcDataSend[index].fHisValue = rand()%100;//100���������
				DB104_FMT_TRACE("ң������� g_aYcDataSend[%d].fHisValue=%f \n", index, g_aYcDataSend[index].fHisValue);
			}

			//�ж�ʵʱֵ�Ƿ�Խ������
			fDiffValue = fabs(g_aYcDataSend[index].fNewValue - g_aYcDataSend[index].fHisValue);
			if(fDiffValue > fDeadZone)
			{
				DB104_FMT_TRACE("�仯ֵfDiffValue=%f, ��������ֵ, fNewValue=%f, fHisValue=%f ��Ҫ�����ϱ�! \n", 
					fDiffValue, g_aYcDataSend[index].fNewValue, g_aYcDataSend[index].fHisValue);
				g_bYcChanged = TRUE;
				g_aYcDataSend[index].bChangeFlag = TRUE;
				g_aYcDataSend[index].bSendFlag = FALSE;
			}
			else
			{
				DB104_FMT_TRACE("�仯ֵfDiffValue=%f, С������ֵ, fNewValue=%f, fHisValue=%f \n", 
					fDiffValue, g_aYcDataSend[index].fNewValue, g_aYcDataSend[index].fHisValue);
			}
			
		}
		else
		{
			DB104_FMT_TRACE("yc_db_record_read() ��������ʧ��  g_aYcDataConfig[%d].nDbCenterOAD=%08x \n", 
				index, g_aYcDataConfig[index].nDbCenterOAD);
		}
	}
	else
	{
		DB104_FMT_TRACE("yc_db_record_read() ��ȡ����  g_aYcDataConfig[%d].nDbCenterOAD=%08x \n", 
			index, g_aYcDataConfig[index].nDbCenterOAD);
		DB104_FMT_TRACE("yc_db_record_read() ���ݶ�ȡʧ��! \n");
	}
	
	system_time_get(&g_aYcDataSend[index].btime[0]);
}

/*******************************************************************************
* ��������: yc_db_record_read
* ��������: ��Ҫͳ�ƻ�����ң���ź�
* �������: uint16 index �������
* �������: ��
* �� �� ֵ: ��
*******************************************************************************/
void yc_calculate_statistics(uint16 index)
{
	if(index >= g_nConfigYcNum)
	{
		return;
	}

	int ycRet = 0;
	int paraRet = 0;
	uint16 ycAddr = 0;
	uint16 paraAddr = 0;
	float32 ycValue = 0;
	float32 paraValue = 0;
	
	float32 resultValue = 0;//���ֵ
	float32 fDeadZone = 0;	//����ֵ
	float32 fDiffValue = 0;  //��ֵ
	float32 fRatio = 1;  //����

	switch(g_aYcDataConfig[index].nDealType)
	{
		case CALC_YC_A_VOLT_DVALUE:
		case CALC_YC_B_VOLT_DVALUE:
		case CALC_YC_C_VOLT_DVALUE:
		{
			if(g_aYcDataConfig[index].aOadCols[0] == BASE_CLASS_YC)
			{
				memcpy(&ycAddr, &g_aYcDataConfig[index].aOadCols[1], sizeof(uint16));
				ycRet = yc_query_float32(ycAddr, &ycValue);
			}
			if(g_aYcDataConfig[index].aOadCols[2] == BASE_CLASS_PARA)
			{
				memcpy(&paraAddr, &g_aYcDataConfig[index].aOadCols[3], sizeof(uint16));
				paraRet = para_query_float32(paraAddr, &paraValue);
			}

			if((ycRet<0) || (paraRet<0))
			{
				DB104_FMT_TRACE("yc_calculate_statistics ����! ycRet=%f, paraRet=%f \n", ycRet, paraRet);
				return;
			}

			resultValue = fabs(ycValue - paraValue);
		}
			break;
		case CALC_YC_FR_DVALUE:
		{
			if(g_aYcDataConfig[index].aOadCols[0] == BASE_CLASS_YC)
			{
				memcpy(&ycAddr, &g_aYcDataConfig[index].aOadCols[1], sizeof(uint16));
				ycRet = yc_query_float32(ycAddr, &ycValue);
			}

			if(ycRet<0)
			{
				DB104_FMT_TRACE("yc_calculate_statistics ����! ycRet=%f \n", ycRet);
				return;
			}

			resultValue = fabs(ycValue - 50);
		}
			break;
		case CALC_YC_PD_LOAD_RATE:
		{
			if(g_aYcDataConfig[index].aOadCols[0] == BASE_CLASS_YC)
			{
				memcpy(&ycAddr, &g_aYcDataConfig[index].aOadCols[1], sizeof(uint16));
				ycRet = yc_query_float32(ycAddr, &ycValue);
			}
			if(g_aYcDataConfig[index].aOadCols[2] == BASE_CLASS_PARA)
			{
				memcpy(&paraAddr, &g_aYcDataConfig[index].aOadCols[3], sizeof(uint16));
				paraRet = para_query_float32(paraAddr, &paraValue);
			}

			if((ycRet<0) || (paraRet<0))
			{
				DB104_FMT_TRACE("yc_calculate_statistics ����! ycRet=%f, paraRet=%f \n", ycRet, paraRet);
				return;
			}

			if( (paraValue<0.00001) && (paraRet>(-0.00001)) )
			{
				DB104_FMT_TRACE("yc_calculate_statistics ����! paraValue=%f \n", paraValue);
				return;
			}

			resultValue = ycValue/paraValue;
		}
			break;
		default:
			return;
			break;
	}
	
	//��ȡ����
	g_aYcDataSend[index].fNewValue = resultValue*fRatio;
	DB104_FMT_TRACE("���ʴ���֮��g_aYcDataSend[index].fNewValue=%f, ���ʴ���֮ǰresultValue=%f, ����fRatio=%f \n", g_aYcDataSend[index].fNewValue, resultValue, fRatio);
	
	//��ȡ�仯����ֵ
	if(para_query_float32(g_aYcDataConfig[index].nDeadZoneAddr, &fDeadZone) < 0)
	{
		DB104_FMT_TRACE("��ȡ�仯����ֵpara_query_float32()ʧ�ܣ�ֱ�Ӹ�Ĭ��ֵ�� \n");
		fDeadZone = g_aYcDataConfig[index].fDeadZoneValue;
	}
	DB104_FMT_TRACE("�仯����ֵfDeadZone=%f \n", fDeadZone);

	//��ʱ��ֵ, ���Ա仯����
	if(g_bTestEnable)
	{
		g_aYcDataSend[index].fHisValue = rand()%100;//100���������
		DB104_FMT_TRACE("ң������� g_aYcDataSend[%d].fHisValue=%f \n", index, g_aYcDataSend[index].fHisValue);
	}

	//�ж�ʵʱֵ�Ƿ�Խ������
	fDiffValue = fabs(g_aYcDataSend[index].fNewValue - g_aYcDataSend[index].fHisValue);
	if(fDiffValue > fDeadZone)
	{
		DB104_FMT_TRACE("�仯ֵfDiffValue=%f, ��������ֵ, fNewValue=%f, fHisValue=%f ��Ҫ�����ϱ�! \n", 
			fDiffValue, g_aYcDataSend[index].fNewValue, g_aYcDataSend[index].fHisValue);
		g_bYcChanged = TRUE;
		g_aYcDataSend[index].bChangeFlag = TRUE;
		g_aYcDataSend[index].bSendFlag = FALSE;
	}
	else
	{
		DB104_FMT_TRACE("�仯ֵfDiffValue=%f, С������ֵ, fNewValue=%f, fHisValue=%f \n", 
			fDiffValue, g_aYcDataSend[index].fNewValue, g_aYcDataSend[index].fHisValue);
	}
	
	system_time_get(&g_aYcDataSend[index].btime[0]);
}


/*******************************************************************************
* ��������: en_db_nomal_read
* ��������: ���������Ķ�ȡ����ʵʱ����(��ͨ������)
* �������: uint16 index �������
* �������: ��
* �� �� ֵ: ��
*******************************************************************************/
void en_db_nomal_read(uint16 index)
{
	if(index >= g_nConfigEnNum)
	{
		return;
	}
	
	uint8   aRecvBuff[500] = {0};
	uint32  nRevcBuffLen = 0;
	int     ret  = 0;
	NOMAL_OAD_T NomalOad = {0};  //��ͨ��OAD

	float32 fValueDB;  //�������������ĵ�ֵ
	float32 fDeadZone;  //����ֵ
	float32 fDiffValue;  //��ֵ
	float32 fRatio = 1;  //����
	const Map645ToOop *pMap645ToOop = NULL;

	//���ݶ�ȡ
	NomalOad.logicId = g_aEnergyDataConfig[index].nLogicId;  //�߼���ַ
	NomalOad.infoNum = g_aEnergyDataConfig[index].nInfoNum;  //��Ϣ���
	NomalOad.oad.value = g_aEnergyDataConfig[index].nDbCenterOAD;  //��ͨOAD
	NomalOad.classtag = CLASS_DATA_NORMAL;

	ret = db_read_nomal(g_sDataCenterClient, &NomalOad, sizeof(aRecvBuff), (uint8*)aRecvBuff, &nRevcBuffLen);
	DB104_FMT_TRACE("<--��%d������--> en_db_nomal_read() ret = db_read_nomal() ret:%d \n", index, ret);
	
	if(ret == 0)
	{
		DB104_FMT_TRACE("en_db_nomal_read() ��ȡ���ݳɹ�  g_aEnergyDataConfig[%d].nDbCenterOAD=%08x \n", 
			index, g_aEnergyDataConfig[index].nDbCenterOAD);
		DB104_BUF_TRACE(aRecvBuff, nRevcBuffLen, "����: ");

		//��ȡ�����������ݿ飬��Ҫ���з��ദ��
		ret = data_block_split(g_aEnergyDataConfig[index].nDbCenterOAD, g_aEnergyDataConfig[index].nItemIndex, aRecvBuff, &fValueDB);
		if(ret > 0)
		{
			//��ȡ����
			pMap645ToOop = oad_info_find(g_aEnergyDataConfig[index].nDbCenterOAD);
			if(pMap645ToOop == NULL)
			{
				DB104_FMT_TRACE("en_db_nomal_read() if(pMap645ToOop == NULL) \n");
			}
			else
			{
				fRatio = ratio_convert(pMap645ToOop->DesScaler, E_OOP_TO_REAL);
				DB104_FMT_TRACE("��ȡ���� fRatio=%f \n", fRatio);
			}
			g_aEnergyDataSend[index].fNewValue = fValueDB*fRatio;
			DB104_FMT_TRACE("���ʴ���֮��g_aEnergyDataSend[index].fNewValue=%f, ���ʴ���֮ǰfValueDB=%f \n", g_aEnergyDataSend[index].fNewValue, fValueDB);
			
			//��ȡ�仯����ֵ
			if(para_query_float32(g_aEnergyDataConfig[index].nDeadZoneAddr, &fDeadZone) < 0)
			{
				DB104_FMT_TRACE("��ȡ�仯����ֵpara_query_float32()ʧ�ܣ�ֱ�Ӹ�Ĭ��ֵ�� \n");
				fDeadZone = g_aEnergyDataConfig[index].fDeadZoneValue;
			}
			DB104_FMT_TRACE("�仯����ֵfDeadZone=%f \n", fDeadZone);

			//�ж�ʵʱֵ�Ƿ�Խ������
			fDiffValue = fabs(g_aEnergyDataSend[index].fNewValue - g_aEnergyDataSend[index].fHisValue);
			if(fDiffValue > fDeadZone)
			{
				DB104_FMT_TRACE("�仯ֵfDiffValue=%f, ��������ֵ, fNewValue=%f, fHisValue=%f ��Ҫ�����ϱ�! \n", 
					fDiffValue, g_aEnergyDataSend[index].fNewValue, g_aEnergyDataSend[index].fHisValue);
				g_bEnergyChanged = TRUE;
				g_aEnergyDataSend[index].bChangeFlag = TRUE;
				g_aEnergyDataSend[index].bSendFlag = FALSE;
			}
			else
			{
				DB104_FMT_TRACE("�仯ֵfDiffValue=%f, С������ֵ, fNewValue=%f, fHisValue=%f \n", 
					fDiffValue, g_aEnergyDataSend[index].fNewValue, g_aEnergyDataSend[index].fHisValue);
			}
			
		}
		else
		{
			DB104_FMT_TRACE("en_db_nomal_read() ��������ʧ��  g_aEnergyDataConfig[%d].nDbCenterOAD=%08x \n", 
				index, g_aEnergyDataConfig[index].nDbCenterOAD);
		}
	}
	else
	{
		DB104_FMT_TRACE("en_db_nomal_read() ��ȡ����  g_aEnergyDataConfig[%d].nDbCenterOAD=%08x \n", 
			index, g_aEnergyDataConfig[index].nDbCenterOAD);
		DB104_FMT_TRACE("en_db_nomal_read() ���ݶ�ȡʧ��! \n");

		//��ʱ��ֵ, ���Ա仯����
		if(g_bTestEnable)
		{
			g_aEnergyDataSend[index].fNewValue = rand()%100;//100���������
			DB104_FMT_TRACE("��������� g_aEnergyDataSend[%d].fNewValue=%f \n", index, g_aEnergyDataSend[index].fNewValue);
		}
	}

	system_time_get(&g_aEnergyDataSend[index].btime[0]);
}

/*******************************************************************************
* ��������: yc_db_record_read
* ��������: ���������Ķ�ȡң��ʵʱ����(��¼������)
* �������: uint16 index �������
* �������: ��
* �� �� ֵ: ��
*******************************************************************************/
void en_db_record_read(uint16 index)
{
	if(index >= g_nConfigEnNum)
	{
		return;
	}

	uint8	aRecvBuff[500] = {0};
	uint32	nRevcBuffLen = 0;
	int 	ret  = 0;
	READ_RECORD_T ReadRecord;  //��¼��OAD
	RECORD_UNIT_T RecordOut;  //��¼�Ͳ�ѯ���
	memset(&ReadRecord, 0, sizeof(READ_RECORD_T));
	memset(&RecordOut, 0, sizeof(RECORD_UNIT_T));
	
	time_t nRealTime_t;  //��ǰϵͳʱ���ת��������

	float32 fValueDB;  //�������������ĵ�ֵ
	float32 fDeadZone;	//����ֵ
	float32 fDiffValue;  //��ֵ
	float32 fRatio = 1;  //����
	const Map645ToOop *pMap645ToOop = NULL;

	//���ݶ�ȡ
	nRealTime_t = time(NULL);
	
	ReadRecord.recordOAD.logicId = g_aEnergyDataConfig[index].nLogicId;  //�߼���ַ
	ReadRecord.recordOAD.infoNum = g_aEnergyDataConfig[index].nInfoNum;  //��Ϣ���
	ReadRecord.recordOAD.road.oadMain.value = g_aEnergyDataConfig[index].nDbCenterOAD;  //��OAD
	ReadRecord.recordOAD.road.oadCols.nNum = g_aEnergyDataConfig[index].nOadColsNum;  //��OAD����
	memcpy(&ReadRecord.recordOAD.road.oadCols.oad[0], &g_aEnergyDataConfig[index].aOadCols[0],
		g_aEnergyDataConfig[index].nOadColsNum*sizeof(uint32));  //������OAD
	ReadRecord.recordOAD.classtag = CLASS_DATA_COLLEC;

	ReadRecord.cType = COL_TM_STORE;  //�ɼ��洢ʱ��
	//ReadRecord.cStart = nRealTime_t-24*3600;  //ʱ����ʼ�㣬��ǰʱ����ǰһ��
	ReadRecord.cStart = 0;  //ʱ����ʼ�㣬��ǰʱ����ǰһ��
	ReadRecord.cEnd = nRealTime_t;  //ʱ�������
	ReadRecord.sortType = DOWN_SORT;  //����ʽ ����
	ReadRecord.MAC.nNum = 6;  //����У�� ����
	meter_addr_query(g_aEnergyDataConfig[index].nInfoNum, ReadRecord.MAC.value, ReadRecord.MAC.nNum);  //����У�� ����ַ
	ReadRecord.ti.unit = 6;  //ʱ���� ��λ
	ReadRecord.ti.value = 6;  //ʱ���� ֵ

	DB104_FMT_TRACE("<--��%d������--> en_db_record_read() �����±�index=%d, �߼���ַlogicId=%d, ��Ϣ���infoNum=%d, ��OAD=%08x, ��OAD����=%d, ��OAD=%08x, У��MAC.nNum=%d, MAC.value=%02x%02x%02x%02x%02x%02x \n", 
		index, index, ReadRecord.recordOAD.logicId, ReadRecord.recordOAD.infoNum, ReadRecord.recordOAD.road.oadMain.value, g_aEnergyDataConfig[index].nOadColsNum, 
		ReadRecord.recordOAD.road.oadCols.oad[0].value, ReadRecord.MAC.nNum, ReadRecord.MAC.value[5], ReadRecord.MAC.value[4], ReadRecord.MAC.value[3], ReadRecord.MAC.value[2], ReadRecord.MAC.value[1], ReadRecord.MAC.value[0]);
	
	ret = db_read_record(g_sDataCenterClient, &ReadRecord, sizeof(aRecvBuff), &RecordOut);
	DB104_FMT_TRACE("en_db_record_read() ret = db_read_record() ret:%d \n", ret);
	if(ret == 0)
	{
		nRevcBuffLen = RecordOut.record[0].outDatalen;
		memcpy(&aRecvBuff[0], &RecordOut.record[0].outDataBuf[0], nRevcBuffLen);

		DB104_FMT_TRACE("en_db_record_read() ��ȡ���ݳɹ�  g_aEnergyDataConfig[%d].nDbCenterOAD=%08x \n", 
			index, g_aEnergyDataConfig[index].nDbCenterOAD);
		DB104_BUF_TRACE(aRecvBuff, nRevcBuffLen, "����: ");

		//��ȡ�����������ݿ飬��Ҫ���з��ദ��
		ret = data_block_split(g_aEnergyDataConfig[index].aOadCols[0], g_aEnergyDataConfig[index].nItemIndex, aRecvBuff, &fValueDB);
		if(ret > 0)
		{
			//��ȡ����
			pMap645ToOop = oad_info_find(g_aEnergyDataConfig[index].aOadCols[0]);
			if(pMap645ToOop == NULL)
			{
				DB104_FMT_TRACE("en_db_record_read() if(pMap645ToOop == NULL) \n");
			}
			else
			{
				fRatio = ratio_convert(pMap645ToOop->DesScaler, E_OOP_TO_REAL);
				DB104_FMT_TRACE("��ȡ���� fRatio=%f \n", fRatio);
			}
			g_aEnergyDataSend[index].fNewValue = fValueDB*fRatio;
			DB104_FMT_TRACE("���ʴ���֮��g_aEnergyDataSend[index].fNewValue=%f, ���ʴ���֮ǰfValueDB=%f \n", g_aEnergyDataSend[index].fNewValue, fValueDB);
			
			//��ȡ�仯����ֵ
			if(para_query_float32(g_aEnergyDataConfig[index].nDeadZoneAddr, &fDeadZone) < 0)
			{
				DB104_FMT_TRACE("��ȡ�仯����ֵpara_query_float32()ʧ�ܣ�ֱ�Ӹ�Ĭ��ֵ�� \n");
				fDeadZone = g_aEnergyDataConfig[index].fDeadZoneValue;
			}
			DB104_FMT_TRACE("�仯����ֵfDeadZone=%f \n", fDeadZone);

			//�ж�ʵʱֵ�Ƿ�Խ������
			fDiffValue = fabs(g_aEnergyDataSend[index].fNewValue - g_aEnergyDataSend[index].fHisValue);
			if(fDiffValue > fDeadZone)
			{
				DB104_FMT_TRACE("�仯ֵfDiffValue=%f, ��������ֵ, fNewValue=%f, fHisValue=%f ��Ҫ�����ϱ�! \n", 
					fDiffValue, g_aEnergyDataSend[index].fNewValue, g_aEnergyDataSend[index].fHisValue);
				g_bEnergyChanged = TRUE;
				g_aEnergyDataSend[index].bChangeFlag = TRUE;
				g_aEnergyDataSend[index].bSendFlag = FALSE;
			}
			else
			{
				DB104_FMT_TRACE("�仯ֵfDiffValue=%f, С������ֵ, fNewValue=%f, fHisValue=%f \n", 
					fDiffValue, g_aEnergyDataSend[index].fNewValue, g_aEnergyDataSend[index].fHisValue);
			}
			
		}
		else
		{
			DB104_FMT_TRACE("en_db_record_read() ��������ʧ��  g_aEnergyDataConfig[%d].nDbCenterOAD=%08x \n", 
				index, g_aEnergyDataConfig[index].nDbCenterOAD);
		}
	}
	else
	{
		DB104_FMT_TRACE("en_db_record_read() ��ȡ����  g_aEnergyDataConfig[%d].nDbCenterOAD=%08x \n", 
			index, g_aEnergyDataConfig[index].nDbCenterOAD);
		DB104_FMT_TRACE("en_db_record_read() ���ݶ�ȡʧ��! \n");

		//��ʱ��ֵ, ���Ա仯����
		if(g_bTestEnable)
		{
			g_aEnergyDataSend[index].fNewValue = rand()%100;//100���������
			DB104_FMT_TRACE("��������� g_aEnergyDataSend[%d].fNewValue=%f \n", index, g_aEnergyDataSend[index].fNewValue);
		}
	}

	system_time_get(&g_aEnergyDataSend[index].btime[0]);
}

/*******************************************************************************
* ��������: en_calculate_statistics
* ��������: ��Ҫͳ�ƻ����ĵ����ź�
* �������: uint16 index �������
* �������: ��
* �� �� ֵ: ��
*******************************************************************************/
void en_calculate_statistics(uint16 index)
{
	if(index >= g_nConfigEnNum)
	{
		return;
	}

	DB104_FMT_TRACE("en_calculate_statistics() index=%d, ���ֱ�־=%d, ��ǰ����=%d, ����%15=%d, ���ձ�־=%d \n", 
		index, g_unPastTimeFlag.BitSect.Minute, g_stCurBinTime.Minute, g_stCurBinTime.Minute%15, g_unPastTimeFlag.BitSect.Day);
	
	switch(g_aEnergyDataConfig[index].nDealType)
	{
		case CALC_EN_15MIN_FREEZE:
		{
			if( g_unPastTimeFlag.BitSect.Minute && (0 == g_stCurBinTime.Minute%15) )
			{
				DB104_FMT_TRACE("en_calculate_statistics() 15���Ӷ�������! index=%d \n", index);
				en_db_nomal_read(index);
			}
		}
			break;
		case CALC_EN_DAY_FREEZE:
		{
			if(g_unPastTimeFlag.BitSect.Day)
			{
				DB104_FMT_TRACE("en_calculate_statistics() �ն�������! index=%d \n", index);
				en_db_nomal_read(index);
			}
		}
			break;
		default:
			break;
	}
}

/*******************************************************************************
* ��������: data_db_center_read
* ��������: ���������Ķ�ȡʵʱ����
* �������: ��
* �������: ��
* �� �� ֵ: ��
*******************************************************************************/
void data_db_center_read()
{
	int i = 0;
	
	//ң�����ݶ�ȡ
	DB104_FMT_TRACE("\n************data_db_center_read() ң�����ݶ�ȡ start...************ \n\n");
	for(i=0; i<g_nConfigYxNum; i++)
	{
		DB104_FMT_TRACE("i=%d, g_nConfigYxNum=%d, g_aYxDataConfig[i].nDbCenterOAD=0x%08x, g_aYxDataConfig[i].nDealType=0x%08x \n", 
			i, g_nConfigYxNum, g_aYxDataConfig[i].nDbCenterOAD, g_aYxDataConfig[i].nDealType);

		if(g_aYxDataConfig[i].nDbCenterOAD == 0xFFFFFFFF)
		{
			if(g_aYxDataConfig[i].nDealType == CALC_YX_REAL_SIGNAL)
			{
				yx_real_signal_read(i);//Ӳң��
			}
			else
			{
				yx_calculate_statistics(i);//ͳ������, ��ʵ�ʲɼ�
			}
		}
		else
		{
			yx_db_center_read(i);//���������Ķ�ȡ����
		}
	}

	//ң�����ݶ�ȡ
	DB104_FMT_TRACE("\n************data_db_center_read() ң�����ݶ�ȡ start...************ \n\n");
	for(i=0; i<g_nConfigYcNum; i++)
	{
		if(g_aYcDataConfig[i].nDbCenterOAD == 0xFFFFFFFF)
		{
			yc_calculate_statistics(i);//ͳ������, ��ʵ�ʲɼ�
		}
		else
		{
			if(g_aYcDataConfig[i].nLogicId == 1)  //��������
			{
				yc_db_nomal_read(i);
			}
			else if(g_aYcDataConfig[i].nLogicId == 0)  //�������
			{
				yc_db_record_read(i);
			}
		}
	}

	//���������ݶ�ȡ
	DB104_FMT_TRACE("\n************data_db_center_read() ���������ݶ�ȡ start...************ \n\n");
	for(i=0; i<g_nConfigEnNum; i++)
	{
		DB104_FMT_TRACE("i=%d, g_nConfigEnNum=%d, g_aEnergyDataConfig[i].nLogicId=%d, g_aEnergyDataConfig[i].nDealType=0x%08x \n", 
			i, g_nConfigEnNum, g_aEnergyDataConfig[i].nLogicId, g_aEnergyDataConfig[i].nDealType);
		if(g_aEnergyDataConfig[i].nDealType != 0)
		{
			en_calculate_statistics(i);//ͳ������, ��ʵ�ʲɼ�
		}
		else
		{
			if(g_aEnergyDataConfig[i].nLogicId == 1)  //��������
			{
				en_db_nomal_read(i);
			}
			else if(g_aEnergyDataConfig[i].nLogicId == 0)  //�������
			{
				en_db_record_read(i);
			}
		}
	}
}

/*******************************************************************************
* ��������: para_db_center_save
* ��������: ����ֵ������������������
* �������: ��
* �������: ��
* �� �� ֵ: ��
*******************************************************************************/
int para_data_search(uint8 *pDesbuffer, uint16 *nDesLen, const Map104ToOop *pOadInfo)
{
	int i = 0;
	float32 fRatio = 1;
	float32 fValue = 0;

	switch(pOadInfo->Oad.value)
	{
		case (0x49010200)://��������0x49010200
		{
			OOP_DEAD_ZONE_T sDeadZone;
			memset(&sDeadZone, 0, sizeof(OOP_DEAD_ZONE_T));
			for(i=0; i<MAX_OOP_ADDR_NUM; i++)
			{
				//��ȡ����ֵ
				if(para_query_float32(pOadInfo->Addr[i], &fValue) < 0)
				{
					DB104_FMT_TRACE("para_data_search() δ����104��ַ��Ӧ����ֵ pOadInfo->Oad=%08x, pOadInfo->Addr[%d]=%04x \n", pOadInfo->Oad.value, i, pOadInfo->Addr[i]);
					continue;  //δ����104��ַ��Ӧ����ֵ
				}
				
				fRatio = ratio_convert(pOadInfo->InfoSca[i], E_REAL_TO_OOP);  //��ȡ����
				DB104_FMT_TRACE("para_data_search() ��ȡ���� pOadInfo->Oad=%08x, pOadInfo->InfoSca[%d]=%d, fRatio=%f \n", 
					pOadInfo->Oad.value, i, pOadInfo->InfoSca[i], fRatio);
				
				if(i == 0)
				{
					sDeadZone.nI = (int32)(fValue*fRatio);
				}
				else if(i == 1)
				{
					sDeadZone.nAcU = (uint32)(fValue*fRatio);
				}
				else if(i == 2)
				{
					sDeadZone.nDcU = (uint32)(fValue*fRatio);
				}
				else if(i == 3)
				{
					sDeadZone.nPf = (int32)(fValue*fRatio);
				}
				else if(i == 4)
				{
					sDeadZone.nFr = (int16)(fValue*fRatio);
				}
			}
			*nDesLen = sizeof(OOP_DEAD_ZONE_T);
			memcpy(pDesbuffer, &sDeadZone, sizeof(OOP_DEAD_ZONE_T));
			
			DB104_FMT_TRACE("para_data_search() ��װ���� pOadInfo->Oad=%08x, *nDesLen=%d, pDesbuffer: \n", pOadInfo->Oad.value, *nDesLen);
			DB104_BUF_TRACE(pDesbuffer, *nDesLen, "����: ");
		}
			break;
		case (0x49020200)://PT����0x49020200
		{
			OOP_PT_T sPT;
			memset(&sPT, 0, sizeof(OOP_PT_T));
			for(i=0; i<MAX_OOP_ADDR_NUM; i++)
			{
				//��ȡ����ֵ
				if(para_query_float32(pOadInfo->Addr[i], &fValue) < 0)
				{
					continue;  //δ����104��ַ��Ӧ����ֵ
				}
				fRatio = ratio_convert(pOadInfo->InfoSca[i], E_REAL_TO_OOP);  //��ȡ����
				if(i == 0)
				{
					sPT.nFir = (uint16)(fValue*fRatio);
				}
				else if(i == 1)
				{
					sPT.nSec = (uint16)(fValue*fRatio);
				}
			}
			*nDesLen = sizeof(OOP_PT_T);
			memcpy(pDesbuffer, &sPT, sizeof(OOP_PT_T));
		}
			break;
		case (0x49030200)://CT����0x49030200
		{
			OOP_CT_T sCT;
			memset(&sCT, 0, sizeof(OOP_CT_T));
			for(i=0; i<MAX_OOP_ADDR_NUM; i++)
			{
				//��ȡ����ֵ
				if(para_query_float32(pOadInfo->Addr[i], &fValue) < 0)
				{
					continue;  //δ����104��ַ��Ӧ����ֵ
				}
				fRatio = ratio_convert(pOadInfo->InfoSca[i], E_REAL_TO_OOP);  //��ȡ����
				if(i == 0)
				{
					sCT.nPhFir = (int32)(fValue*fRatio);
				}
				else if(i == 1)
				{
					sCT.nPhSec = (int32)(fValue*fRatio);
				}
				else if(i == 2)
				{
					sCT.nZeroFir = (int32)(fValue*fRatio);
				}
				else if(i == 3)
				{
					sCT.nZeroSec = (int32)(fValue*fRatio);
				}
			}
			*nDesLen = sizeof(OOP_CT_T);
			memcpy(pDesbuffer, &sCT, sizeof(OOP_CT_T));
		}
			break;
		case (0x49040200)://�͵�ѹ����0x49040200
		{
			OOP_LOW_U_T sLowU;
			memset(&sLowU, 0, sizeof(OOP_LOW_U_T));
			for(i=0; i<MAX_OOP_ADDR_NUM; i++)
			{
				//��ȡ����ֵ
				if(para_query_float32(pOadInfo->Addr[i], &fValue) < 0)
				{
					continue;  //δ����104��ַ��Ӧ����ֵ
				}
				fRatio = ratio_convert(pOadInfo->InfoSca[i], E_REAL_TO_OOP);  //��ȡ����
				if(i == 0)
				{
					sLowU.nLimit = (uint32)(fValue*fRatio);
				}
				else if(i == 1)
				{
					sLowU.nDelay = (uint16)(fValue*fRatio);
				}
			}
			*nDesLen = sizeof(OOP_LOW_U_T);
			memcpy(pDesbuffer, &sLowU, sizeof(OOP_LOW_U_T));
		}
			break;
		case (0x49050200)://����ѹ����0x49050200
		{
			OOP_OVER_U_T sOverU;
			memset(&sOverU, 0, sizeof(OOP_OVER_U_T));
			for(i=0; i<MAX_OOP_ADDR_NUM; i++)
			{
				//��ȡ����ֵ
				if(para_query_float32(pOadInfo->Addr[i], &fValue) < 0)
				{
					continue;  //δ����104��ַ��Ӧ����ֵ
				}
				fRatio = ratio_convert(pOadInfo->InfoSca[i], E_REAL_TO_OOP);  //��ȡ����
				if(i == 0)
				{
					sOverU.nLimit = (uint32)(fValue*fRatio);
				}
				else if(i == 1)
				{
					sOverU.nDelay = (uint16)(fValue*fRatio);
				}
			}
			*nDesLen = sizeof(OOP_OVER_U_T);
			memcpy(pDesbuffer, &sOverU, sizeof(OOP_OVER_U_T));
		}
			break;
		case (0x49060200)://���ر���0x49060200
		{
			OOP_HEAVY_LOAD_T sHeavyLoad;
			memset(&sHeavyLoad, 0, sizeof(OOP_HEAVY_LOAD_T));
			for(i=0; i<MAX_OOP_ADDR_NUM; i++)
			{
				//��ȡ����ֵ
				if(para_query_float32(pOadInfo->Addr[i], &fValue) < 0)
				{
					continue;  //δ����104��ַ��Ӧ����ֵ
				}
				fRatio = ratio_convert(pOadInfo->InfoSca[i], E_REAL_TO_OOP);  //��ȡ����
				if(i == 0)
				{
					sHeavyLoad.nLimit = (int32)(fValue*fRatio);
				}
				else if(i == 1)
				{
					sHeavyLoad.nDelay = (uint16)(fValue*fRatio);
				}
			}
			*nDesLen = sizeof(OOP_HEAVY_LOAD_T);
			memcpy(pDesbuffer, &sHeavyLoad, sizeof(OOP_HEAVY_LOAD_T));
		}
			break;
		case (0x49070200)://���ر���0x49070200
		{
			OOP_OVER_LOAD_T sOverLoad;
			memset(&sOverLoad, 0, sizeof(OOP_OVER_LOAD_T));
			for(i=0; i<MAX_OOP_ADDR_NUM; i++)
			{
				//��ȡ����ֵ
				if(para_query_float32(pOadInfo->Addr[i], &fValue) < 0)
				{
					continue;  //δ����104��ַ��Ӧ����ֵ
				}
				fRatio = ratio_convert(pOadInfo->InfoSca[i], E_REAL_TO_OOP);  //��ȡ����
				if(i == 0)
				{
					sOverLoad.nLimit = (int32)(fValue*fRatio);
				}
				else if(i == 1)
				{
					sOverLoad.nDelay = (uint16)(fValue*fRatio);
				}
			}
			*nDesLen = sizeof(OOP_OVER_LOAD_T);
			memcpy(pDesbuffer, &sOverLoad, sizeof(OOP_OVER_LOAD_T));
		}
			break;
		case (0x49080200)://�������ɼ�������ʱ��0x49080200
		{
			OOP_YX_GET_TIME_T sYxGetTime;
			memset(&sYxGetTime, 0, sizeof(OOP_YX_GET_TIME_T));
			for(i=0; i<MAX_OOP_ADDR_NUM; i++)
			{
				//��ȡ����ֵ
				if(para_query_float32(pOadInfo->Addr[i], &fValue) < 0)
				{
					continue;  //δ����104��ַ��Ӧ����ֵ
				}
				fRatio = ratio_convert(pOadInfo->InfoSca[i], E_REAL_TO_OOP);  //��ȡ����
				if(i == 0)
				{
					sYxGetTime.nTime = (uint32)(fValue*fRatio);
				}
			}
			*nDesLen = sizeof(OOP_YX_GET_TIME_T);
			memcpy(pDesbuffer, &sYxGetTime, sizeof(OOP_YX_GET_TIME_T));
		}
			break;
		case (0x49090200)://��բ������屣��ʱ��0x49090200
		{
			OOP_PULSE_TIME_T sPulseTime;
			memset(&sPulseTime, 0, sizeof(OOP_PULSE_TIME_T));
			for(i=0; i<MAX_OOP_ADDR_NUM; i++)
			{
				//��ȡ����ֵ
				if(para_query_float32(pOadInfo->Addr[i], &fValue) < 0)
				{
					continue;  //δ����104��ַ��Ӧ����ֵ
				}
				fRatio = ratio_convert(pOadInfo->InfoSca[i], E_REAL_TO_OOP);  //��ȡ����
				if(i == 0)
				{
					sPulseTime.nOpenTime = (uint32)(fValue*fRatio);
				}
				else if(i == 1)
				{
					sPulseTime.nCloseTime = (uint32)(fValue*fRatio);
				}
			}
			*nDesLen = sizeof(OOP_PULSE_TIME_T);
			memcpy(pDesbuffer, &sPulseTime, sizeof(OOP_PULSE_TIME_T));
		}
			break;
		case (0x490A0200)://�����Զ������0x490A0200
		{
			OOP_AUTO_ACT_CYCLE_T sAutoActCycle;
			memset(&sAutoActCycle, 0, sizeof(OOP_AUTO_ACT_CYCLE_T));
			for(i=0; i<MAX_OOP_ADDR_NUM; i++)
			{
				//��ȡ����ֵ
				if(para_query_float32(pOadInfo->Addr[i], &fValue) < 0)
				{
					continue;  //δ����104��ַ��Ӧ����ֵ
				}
				fRatio = ratio_convert(pOadInfo->InfoSca[i], E_REAL_TO_OOP);  //��ȡ����
				if(i == 0)
				{
					sAutoActCycle.nCycle = (uint32)(fValue*fRatio);
				}
				else if(i == 1)
				{
					sAutoActCycle.nTime = (uint32)(fValue*fRatio);
				}
			}
			*nDesLen = sizeof(OOP_AUTO_ACT_CYCLE_T);
			memcpy(pDesbuffer, &sAutoActCycle, sizeof(OOP_AUTO_ACT_CYCLE_T));
		}
			break;
		default:
			return -1;
            break;
	}

	return 1;
}


/*******************************************************************************
* ��������: para_db_center_save
* ��������: ����ֵ������������������
* �������: ��
* �������: ��
* �� �� ֵ: ��
*******************************************************************************/
void para_db_center_save()
{
	uint8   aSendBuff[500] = {0};
	uint16  nSendBuffLen = 0;
	uint8   aRecvBuff[500] = {0};
	uint32  nRevcBuffLen = 0;
	int     ret  = 0;
	const Map104ToOop *pMap104ToOop = NULL;
	pMap104ToOop = g_aMap104ToOopRT;//ʵʱ����
	
	if(pMap104ToOop==NULL)
    {
		return;
	} 

	NOMAL_OAD_T NomalOad = {0};
	NomalOad.logicId = 1;  //�߼���ַ
	NomalOad.infoNum = 0;  //��Ϣ��

	DB104_FMT_TRACE("\n************para_db_center_save() ��ֵ����д���������� start...************ \n\n");

	while(pMap104ToOop->Oad.value != 0xFFFFFFFF)
	{
		if(pMap104ToOop->Oad.value == 0xFFFFFFFF)
		{
			break;
		}

		memset(aSendBuff, 0x00, sizeof(aSendBuff));
		nSendBuffLen = 0;
		memset(aRecvBuff, 0x00, sizeof(aRecvBuff));
		nRevcBuffLen = 0;
		
		ret = para_data_search(aSendBuff, &nSendBuffLen, pMap104ToOop);
		if(ret < 0)
		{
			DB104_FMT_TRACE("para_db_center_save() if(ret < 0) pMap104ToOop->Oad=%08x \n", pMap104ToOop->Oad.value);
		}
		else
		{
			DB104_FMT_TRACE("para_db_center_save() ��ȡ�洢���� pMap104ToOop->Oad=%08x, nSendBuffLen=%d, aSendBuff: \n", pMap104ToOop->Oad.value, nSendBuffLen);
			DB104_BUF_TRACE(aSendBuff, nSendBuffLen, "����: ");
			
			//��ͨ������
			NomalOad.oad = pMap104ToOop->Oad;  //��ͨOAD
			NomalOad.classtag = CLASS_DATA_NORMAL;

			//д������
			ret = db_write_nomal(g_sDataCenterClient, &NomalOad, (uint8*)aSendBuff, nSendBuffLen);
			DB104_FMT_TRACE("para_db_center_save() ret = nomal_data_write ret:%d \n", ret);
			if(ret == 0)
			{
				
				DB104_FMT_TRACE("para_db_center_save() д������  NomalOad.oad=%08x \n", NomalOad.oad.value);
				DB104_BUF_TRACE(aSendBuff, nSendBuffLen, "����: ");
				DB104_FMT_TRACE("para_db_center_save() ��ֵ��������д��ɹ�! \n");
			}

			//��ȡ����
			ret = db_read_nomal(g_sDataCenterClient, &NomalOad, sizeof(aRecvBuff), (uint8*)aRecvBuff, &nRevcBuffLen);
			DB104_FMT_TRACE("para_db_center_save() ret = nomal_data_read ret:%d \n", ret);
			if(ret == 0)
			{
				DB104_FMT_TRACE("para_db_center_save() ��ȡ����  NomalOad.oad=%08x \n", NomalOad.oad.value);
				DB104_BUF_TRACE(aRecvBuff, nRevcBuffLen, "����: ");
				DB104_FMT_TRACE("para_db_center_save() ��ֵ�������ݶ�ȡ�ɹ�! \n");
			}
		}

		pMap104ToOop++;
	}
}











