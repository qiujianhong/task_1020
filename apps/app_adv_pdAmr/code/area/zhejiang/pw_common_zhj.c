/*
*********************************************************************
* Copyright(C) 2019 �Ͼ��������ӹɷ����޹�˾
* All rights reserved. 
* @brief�� ��������
* @date�� 2020-01-03
*********************************************************************
*/

#include "pw_redef.h"
#include "pw_common.h"
#include "pw_lib.h"
#include "pw_protocol.h"
#include "pw_debug.h"

#include "pw_protocol_zhj.h"
#include "pw_common_zhj.h"
#include "app104_xml_zhj.h"

//#define GUO_BIAO  /* ��ѹ��������ƽ��ȹ����㷨 */

LOCK_QUEUE_T g_stSoeBuff;                        //ң�ű�λ�����ڴ�

time_t nPreDayTime_t;  //ǰһ��23:59:59ת��������
time_t nPreMonTime_t;  //��һ�����һ��23:59:59ת��������
time_t nTmnStartTime_t;  //�ն�����ʱ��

/*******************************************************************************
* ��������: his_frozen_file_read_zhj
* ��������: ˽���ļ� ��ȡ��ʷ��������
* �������: ��
* �������: ��
* �� �� ֵ: ��
*******************************************************************************/
int his_frozen_file_read_zhj(uint16 groupIndex, uint16 seqIndex, uint16 taskIndex, char *pData, uint32 nMaxLen)
{
    //˽���ļ�·��
    char filename[100] = {0};//˽���ļ�·��
    memset(filename, 0, sizeof(filename));
    sprintf(filename, "%s_%d_%d_%d", g_pFrozenValuePathZhj, groupIndex, seqIndex+1, taskIndex+1);

    //�������ж�������
    int ret = 0;
    ret = read_pdata_xram(filename, pData, 0, nMaxLen);
    return ret;
}

/*******************************************************************************
* ��������: his_frozen_file_write_zhj
* ��������: ˽���ļ� д����ʷ��������
* �������: ��
* �������: ��
* �� �� ֵ: ��
*******************************************************************************/
int his_frozen_file_write_zhj(uint16 groupIndex, uint16 seqIndex, uint16 taskIndex, char *pData, uint32 nMaxLen)
{
    //˽���ļ�·��
    char filename[100] = {0};//˽���ļ�·��
    memset(filename, 0, sizeof(filename));
    sprintf(filename, "%s_%d_%d_%d", g_pFrozenValuePathZhj, groupIndex, seqIndex+1, taskIndex+1);

    //�������ж�������
    int ret = 0;
    ret = write_pdata_xram(filename, pData, 0, nMaxLen);
    return ret;
}

/*******************************************************************************
* ��������: fixed_area_iden_read_zhj
* ��������: ˽���ļ� ��ȡ��ֵ������ʶ
* �������: ��
* �������: ��
* �� �� ֵ: ��
*******************************************************************************/
int fixed_area_iden_read_zhj(uint16 nArea, char *pData, uint32 nMaxLen)
{
    //˽���ļ�·��
    char filename[100] = {0};//˽���ļ�·��
    memset(filename, 0, sizeof(filename));
    sprintf(filename, "%s_%d", g_pFixedAreaPathZhj, nArea);

    //�������ж�������
    int ret = 0;
    ret = read_pdata_xram(filename, pData, 0, nMaxLen);
    return ret;
}

/*******************************************************************************
* ��������: fixed_area_iden_write_zhj
* ��������: ˽���ļ� ��ȡ��ֵ������ʶ
* �������: ��
* �������: ��
* �� �� ֵ: ��
*******************************************************************************/
int fixed_area_iden_write_zhj(uint16 nArea, char *pData, uint32 nMaxLen)
{
    //˽���ļ�·��
    char filename[100] = {0};//˽���ļ�·��
    memset(filename, 0, sizeof(filename));
    sprintf(filename, "%s_%d", g_pFixedAreaPathZhj, nArea);

    //�������ж�������
    int ret = 0;
    ret = write_pdata_xram(filename, pData, 0, nMaxLen);
    return ret;
}

/*******************************************************************************
* ��������: fixed_para_value_read_zhj
* ��������: ˽���ļ� ��ȡ��ֵ����
* �������: ��
* �������: ��
* �� �� ֵ: ��
*******************************************************************************/
int fixed_para_value_read_zhj(uint16 nArea, char *pData, uint32 nMaxLen)
{
    //˽���ļ�·��
    char filename[100] = {0};//˽���ļ�·��
    memset(filename, 0, sizeof(filename));
    sprintf(filename, "%s_%d", g_pParamValuePathZhj, nArea);

    //�������ж�������
    int ret = 0;
    ret = read_pdata_xram(filename, pData, 0, nMaxLen);
    return ret;
}

/*******************************************************************************
* ��������: fixed_para_value_write_zhj
* ��������: ˽���ļ� д�붨ֵ����
* �������: ��
* �������: ��
* �� �� ֵ: ��
*******************************************************************************/
int fixed_para_value_write_zhj(uint16 nArea, char *pData, uint32 nMaxLen)
{
    //˽���ļ�·��
    char filename[100] = {0};//˽���ļ�·��
    memset(filename, 0, sizeof(filename));
    sprintf(filename, "%s_%d", g_pParamValuePathZhj, nArea);

    //�������ж�������
    int ret = 0;
    ret = write_pdata_xram(filename, pData, 0, nMaxLen);
    return ret;
}

/*******************************************************************************
* ��������: config_xml_load
* ��������: ����xml�����Ϣ
* �������: ��
* �������: ��
* �� �� ֵ: ��
*******************************************************************************/
int config_xml_load_zhj()
{
    int i = 1;//�豸���� ��1��ʼ
    int z = 0;//��� ��0��ʼ
    int j = 1;//����ƫ�Ƶ�ַ ��1��ʼ

    for(i=0; i<MAX_EQUIP_TYPE_NUM; i++)
    {
        //�ж��Ƿ�֧�ֵ��豸����
        switch(i+1)
        {
            case E_EQUIP_POWERTERMINAL:
            case E_EQUIP_TRANSFORMER:
            case E_EQUIP_CAPACITOR:
            case E_EQUIP_RESIDUALCURRENT:
            {
                for(z=0; z<g_aGroupCountZhj[i]; z++)
                {
                    for(j=0; j<g_aEquipCountZhj[i][z]; j++)
                    {
                        //����ң�ŵ��
                        pw_readyxfromXML_zhj(i+1, z, j+1, &g_nConfigYxNumZhj[i][z][j]);
                        if(g_nConfigYxNumZhj[i][z][j] > MAX_YX_NUM)
                    	{
                    		FRAME104_FMT_DEBUG("if(g_nConfigYcNumZhj > MAX_YX_NUM) g_nConfigYcNumZhj[%d][%d][%d]=%d, MAX_YX_NUM=%d \n", i, z, j, g_nConfigYxNumZhj[i][z][j], MAX_YX_NUM);
                    		g_nConfigYxNumZhj[i][z][j] = MAX_YX_NUM;
                    	}

                        //����ң����
                        pw_readycfromXML_zhj(i+1, z, j+1, &g_nConfigYcNumZhj[i][z][j]);
                        if(g_nConfigYcNumZhj[i][z][j] > MAX_YC_NUM)
                    	{
                    		FRAME104_FMT_DEBUG("if(g_nConfigYcNumZhj > MAX_YC_NUM) g_nConfigYcNumZhj[%d][%d][%d]=%d, MAX_YC_NUM=%d \n", i, z, j, g_nConfigYcNumZhj[i][z][j], MAX_YC_NUM);
                    		g_nConfigYcNumZhj[i][z][j] = MAX_YC_NUM;
                    	}

                        //���ض�ֵ�������
                        pw_readParafromXML_zhj(i+1, z, j+1, &g_nConfigParaNumZhj[i][z][j]);
                        if(g_nConfigParaNumZhj[i][z][j] > PARA_MAX_NUM)
                    	{
                    		FRAME104_FMT_DEBUG("if(g_nConfigParaNumZhj > PARA_MAX_NUM) g_nConfigParaNumZhj[%d][%d][%d]=%d, PARA_MAX_NUM=%d \n", i, z, j, g_nConfigParaNumZhj[i][z][j], PARA_MAX_NUM);
                    		g_nConfigParaNumZhj[i][z][j] = PARA_MAX_NUM;
                    	}

                        //���ع�����Ϣ���
                        pw_readfaultfromXML_zhj(i+1, z, j+1, &g_nConfigFaultNumZhj[i][z][j]);
                        if(g_nConfigFaultNumZhj[i][z][j] > FAULT_MAX_NUM)
                    	{
                    		FRAME104_FMT_DEBUG("if(g_nConfigFaultNumZhj > FAULT_MAX_NUM) g_nConfigParaNumZhj[%d][%d][%d]=%d, FAULT_MAX_NUM=%d \n", i, z, j, g_nConfigParaNumZhj[i][z][j], FAULT_MAX_NUM);
                    		g_nConfigFaultNumZhj[i][z][j] = FAULT_MAX_NUM;
                    	}

                        FRAME104_FMT_DEBUG("******���XML����: �豸����:%d, �豸���:%d, �豸���:%d, ң�Ÿ���:%d, ң�����:%d, ��������:%d, �����¼�����:%d \n", 
                                i+1, z, j+1, g_nConfigYxNumZhj[i][z][j], g_nConfigYcNumZhj[i][z][j], g_nConfigParaNumZhj[i][z][j], g_nConfigFaultNumZhj[i][z][j]);
                    }
                }
                
            }
                break;
            
            default:
                break;
        }
        
    }
    
	return 1;
}

/*******************************************************************************
* ��������: task_run_step_init
* ��������: 104����ִ������ȫ�ֱ�����ʼ��
* �������: ��
* �������: ��
* �� �� ֵ: ��
*******************************************************************************/
void task_run_step_init_zhj()
{
    //104ͨ������
    memset(&g_sTaskRunStep, 0x00, sizeof(g_sTaskRunStep));
	g_sTaskRunStep.nTaskStep = STEP_CONT_DEFAULT;
    g_sTaskRunStep.nStartNum = 0;
    g_sTaskRunStep.bFirstRun = 1;

    //104����ִ������ȫ�ֱ���(���ڶ������ϱ�ר��)
    memset(&g_sTimingTaskRunStep, 0x00, sizeof(g_sTimingTaskRunStep));
	g_sTimingTaskRunStep.nTaskStep = STEP_TIMING_TASK_SEND;
    g_sTimingTaskRunStep.nDataCallStep = TIMING_STEP_SEND_YX;
    g_sTimingTaskRunStep.nStartNum = 0;
    g_sTimingTaskRunStep.bFirstRun = 1;
}

/*******************************************************************************
* ��������: param_data_init_zhj
* ��������: ��ֵ������ʼ��
* �������: ��
* �������: ��
* �� �� ֵ: ��
*******************************************************************************/
void param_data_init_zhj()
{
    int i = 0;//�豸����
	int j = 0;//�豸���
    int k = 0;//�豸���
    int m = 0;//��ֵ����
    uint32 n = 0;//��ֵ����
	int ret = 0;
    uint32 ArrIdx = 0;//һά��������(��ʼ)

    //��ȡ��ֵ������
    for(m=0; m<MAX_AREA_NUM_ZHJ; m++)
    {
        ret = fixed_area_iden_read_zhj(m, (char *)&g_aFixedValueAreaZhj[m][0], sizeof(g_aFixedValueAreaZhj[m]));
        FRAME104_FMT_DEBUG("��ȡ���ض�ֵ������ ��ֵ����=%d, MaxLen=%u, ���ret=%d \n", m, sizeof(g_aFixedValueAreaZhj[m]), ret);
    	if(ret < 0)
        {
    		FRAME104_FMT_DEBUG("��ȡ���ض�ֵ������ ʧ��! \n");
    	}
    }
    
    //��ӡ��ֵ������
    for(m=0; m<MAX_AREA_NUM_ZHJ; m++)
    {
        for(i=0; i<MAX_EQUIP_TYPE_NUM; i++)
        {
            for(j=0; j<g_aGroupCountZhj[i]; j++)
            {
                for(k=0; k<g_aEquipCountZhj[i][j];k++)
                {
                    ArrIdx = g_aFixedAreaIndexZhj[m][i][j][k];
                    
                    FRAME104_FMT_DEBUG("��ӡ���ض�ֵ������(ѭ������) �豸����=%d, ���=%d, ���=%d, ��ֵ����=%d \n", i+1, j, k+1, m);
                    FRAME104_FMT_DEBUG("��ӡ���ض�ֵ������(ʵ�ʲ���) �豸����=%d, ���=%d, ���=%d, һά����=%u \n", 
                        g_aFixedValueAreaZhj[m][ArrIdx].nEqType, g_aFixedValueAreaZhj[m][ArrIdx].nEqGroup, g_aFixedValueAreaZhj[m][ArrIdx].nEquipNo, 
                        ArrIdx);
                    FRAME104_FMT_DEBUG("��ֵ����=%d, ������ʶ=0x%02x(����״̬λ=%d, CRδ��/ȡ��Ԥ��=%d, S/E�̻�/Ԥ��=%d), ˽��S/E=%d \n", 
                        g_aFixedValueAreaZhj[m][ArrIdx].nFixedAreaID, g_aFixedValueAreaZhj[m][ArrIdx].unParaIdentify.nParaIden, 
                        g_aFixedValueAreaZhj[m][ArrIdx].unParaIdentify.unParaIden.cont, g_aFixedValueAreaZhj[m][ArrIdx].unParaIdentify.unParaIden.cr, 
                        g_aFixedValueAreaZhj[m][ArrIdx].unParaIdentify.unParaIden.se, g_aFixedValueAreaZhj[m][ArrIdx].nSetState);
                }
            }
        }
    }

    //��ȡ��ֵ����
    for(m=0; m<MAX_AREA_NUM_ZHJ; m++)
    {
        ret = fixed_para_value_read_zhj(m, (char *)&g_aParaSetValueZhj[m][0], sizeof(g_aParaSetValueZhj[m]));
        FRAME104_FMT_DEBUG("��ȡ���ض�ֵ���� ��ֵ����=%d, MaxLen=%u, ���ret=%d \n", m, sizeof(g_aParaSetValueZhj[m]), ret);
    	if(ret < 0)
        {
    		FRAME104_FMT_DEBUG("��ȡ���ض�ֵ����...ʧ��! \n");
            FRAME104_FMT_DEBUG("����Ĭ�϶�ֵ����... \n");
            
            memcpy(&g_aParaSetValueZhj[m][0], &g_aParaValueInitZhj[0], sizeof(g_aParaSetValueZhj[m]));
    	}
        else
        {
            FRAME104_FMT_DEBUG("��ȡ���ض�ֵ����...�ɹ�! \n");
            //�ж��Ƿ���Ҫ��������ز���ͬ����ȫ�ֱ���(��Ҫ���ڲ���ά��)
            for(n=0; n<g_nTotalParaNumZhj; n++)
            {
                if( (1 == g_aParaValueInitZhj[n].nUpdateFlag) 
                    && (m == g_aParaValueInitZhj[n].nUpdateArea) )
                {
                    FRAME104_FMT_DEBUG("����! �豸����=%d, ���=%d, ���=%d, ��ֵ����=%d, һά�����±�=%d \n", 
                        g_aParaSetValueZhj[m][n].nEqType, g_aParaSetValueZhj[m][n].nEqGroup, g_aParaSetValueZhj[m][n].nEquipNo, m, n);
                    memcpy(&g_aParaSetValueZhj[m][n], &g_aParaValueInitZhj[n], sizeof(PARA_SET_VALUE_T));
                }
            }
        }
    }

	//��ֵ������ʼ����ӡ��Ϣ
	for(i=0; i<MAX_EQUIP_TYPE_NUM; i++)
    {
        for(j=0; j<g_aGroupCountZhj[i]; j++)
        {
            for(k=0; k<g_aEquipCountZhj[i][j];k++)
            {
                for(m=0; m<g_aFixedAreaNumZhj[i][j][k]; m++)
                {
                    ArrIdx = g_aParaValueIndexZhj[i][j][k];
                    FRAME104_FMT_DEBUG("��ӡ��ֵ����(ѭ������) �豸����=%d, ���=%d, ���=%d, ��ֵ����=%d \n", i+1, j, k+1, m);
                	
                	for(n=0; n<g_nConfigParaNumZhj[i][j][k]; n++)
                    {
                        FRAME104_FMT_DEBUG("��ӡ��ֵ����(ʵ�ʲ���) �豸����=%d, ���=%d, ���=%d, ��ʼ����=%u, ʵ������=%u \n", 
                            g_aParaSetValueZhj[m][ArrIdx+n].nEqType, g_aParaSetValueZhj[m][ArrIdx+n].nEqGroup, g_aParaSetValueZhj[m][ArrIdx+n].nEquipNo, 
                            ArrIdx, ArrIdx+n);
                        
                        FRAME104_FMT_DEBUG("n=%d, Addr=%08x, TagType=%d, Valid=%d, DefaultValue=%f, MinValue=%f, MaxValue=%f, nDealType=%08x, nDataLen=%d, nArrLen=%d \n", 
            					n, g_aParaSetValueZhj[m][ArrIdx+n].nObjAddrU32, 
            					g_aParaSetValueZhj[m][ArrIdx+n].nTagType, g_aParaSetValueZhj[m][ArrIdx+n].bValid, 
            					g_aParaSetValueZhj[m][ArrIdx+n].fDefaultValue, 
            					g_aParaSetValueZhj[m][ArrIdx+n].fMinValue, g_aParaSetValueZhj[m][ArrIdx+n].fMaxValue, 
            					g_aParaSetValueZhj[m][ArrIdx+n].nDealType, g_aParaSetValueZhj[m][ArrIdx+n].nDataLen, 
            					g_aParaSetValueZhj[m][ArrIdx+n].nArrLen);
                        if(g_aParaSetValueZhj[m][ArrIdx+n].nDataLen > 0)
                        {
                            FRAME104_BUF_DEBUG(g_aParaSetValueZhj[m][ArrIdx+n].unValue.arrayV, g_aParaSetValueZhj[m][ArrIdx+n].nDataLen, "unValue: ");
                        }
                        if(g_aParaSetValueZhj[m][ArrIdx+n].nArrLen > 0)
                        {
                            FRAME104_BUF_DEBUG(g_aParaSetValueZhj[m][ArrIdx+n].aArrValue, g_aParaSetValueZhj[m][ArrIdx+n].nArrLen, "aArrValue: ");
                        }
                    }

                    FRAME104_FMT_DEBUG("\n");
                }
                
            }
        }
    }
	
}

/*******************************************************************************
* ��������: global_init
* ��������: ���̳�ʼ���ܺ���
* �������: ��
* �������: ��
* �� �� ֵ: ��
*******************************************************************************/
int global_init_zhj()
{
    int i = 0;//�豸����
	int j = 0;//�豸���
    int k = 0;//�豸���
    int m = 0;//��ֵ����
    
    //���ñ��ʼ��
    memset(g_aYxDataConfigZhj, 0x00, sizeof(g_aYxDataConfigZhj));
    memset(g_aYcDataConfigZhj, 0x00, sizeof(g_aYcDataConfigZhj));
    memset(g_aParaValueInitZhj, 0x00, sizeof(g_aParaValueInitZhj));
    memset(g_aFaultDataConfigZhj, 0x00, sizeof(g_aFaultDataConfigZhj));

    //�ϱ����ʼ��
    memset(g_aYxDataSendZhj, 0x00, sizeof(g_aYxDataSendZhj));
    memset(g_aYcDataSendZhj, 0x00, sizeof(g_aYcDataSendZhj));

    //Э����ʹ�õı�����ʼ��
    memset(g_nConfigYxNumZhj, 0x00, sizeof(g_nConfigYxNumZhj));
    memset(g_nConfigYcNumZhj, 0x00, sizeof(g_nConfigYcNumZhj));
    memset(g_nConfigParaNumZhj, 0x00, sizeof(g_nConfigParaNumZhj));
    memset(g_nConfigFaultNumZhj, 0x00, sizeof(g_nConfigFaultNumZhj));
    
    memset(g_aReportTaskConfig, 0x00, sizeof(g_aReportTaskConfig));
    memset(g_aReportTaskPubAddrNum, 0x00, sizeof(g_aReportTaskPubAddrNum));
    memset(g_aReportTaskPubAddr, 0x00, sizeof(g_aReportTaskPubAddr));
    memset(g_aTaskCallPubAddrState, 0x00, sizeof(g_aTaskCallPubAddrState));
    memset(g_aReportTaskData, 0x00, sizeof(g_aReportTaskData));
    
    memset(g_aAllEquipFiles, 0x00, sizeof(g_aAllEquipFiles));
    memset(g_aSchemeColOADNum, 0x00, sizeof(g_aSchemeColOADNum));
    memset(g_aSchemeColOAD, 0x00, sizeof(g_aSchemeColOAD));
    memset(g_aAllCallEquipState, 0x00, sizeof(g_aAllCallEquipState));
    memset(g_aAllCallEquipStateYC_R, 0x00, sizeof(g_aAllCallEquipStateYC_R));
    memset(g_aAllCallEquipStateYC_R_T, 0x00, sizeof(g_aAllCallEquipStateYC_R_T));
    memset(g_aAllCallEquipStateYX, 0x00, sizeof(g_aAllCallEquipStateYX));
    memset(g_aAllCallEquipStateYC_STR, 0x00, sizeof(g_aAllCallEquipStateYC_STR));

    memset(g_aEnergyCallEquipState, 0x00, sizeof(g_aEnergyCallEquipState));

    memset(g_aPointCallEquipState, 0x00, sizeof(g_aPointCallEquipState));
    memset(g_aPointCallEquipStateYX, 0x00, sizeof(g_aPointCallEquipStateYX));
    memset(g_aPointCallEquipStateYC_R, 0x00, sizeof(g_aPointCallEquipStateYC_R));
    memset(g_aPointCallEquipStateYC_R_T, 0x00, sizeof(g_aPointCallEquipStateYC_R_T));
    memset(g_aPointCallEquipStateYC_R_EN, 0x00, sizeof(g_aPointCallEquipStateYC_R_EN));
    memset(g_aPointCallEquipStateYC_STR, 0x00, sizeof(g_aPointCallEquipStateYC_STR));

    memset(g_aTaskCallEquipState, 0x00, sizeof(g_aTaskCallEquipState));
    memset(g_aTaskCallEquipStateYX, 0x00, sizeof(g_aTaskCallEquipStateYX));
    memset(g_aTaskCallEquipStateYC_R, 0x00, sizeof(g_aTaskCallEquipStateYC_R));
    memset(g_aTaskCallEquipStateYC_STR, 0x00, sizeof(g_aTaskCallEquipStateYC_STR));
    
    memset(g_aReportTaskState, 0x00, sizeof(g_aReportTaskState));
    memset(g_aReportTaskStateYC_R, 0x00, sizeof(g_aReportTaskStateYC_R));
    memset(g_aReportTaskStateYX, 0x00, sizeof(g_aReportTaskStateYX));
    memset(g_aReportTaskStateYC_STR, 0x00, sizeof(g_aReportTaskStateYC_STR));
    memset(g_aChangedEquipStateYC_R, 0x00, sizeof(g_aChangedEquipStateYC_R));
    memset(g_aChangedEquipStateYC_R_T, 0x00, sizeof(g_aChangedEquipStateYC_R_T));
    memset(g_aChangedEquipStateYC_R_EN, 0x00, sizeof(g_aChangedEquipStateYC_R_EN));
    memset(g_aChangedEquipStateYX, 0x00, sizeof(g_aChangedEquipStateYX));
    memset(g_aChangedEquipStateYC_STR, 0x00, sizeof(g_aChangedEquipStateYC_STR));

    memset(g_aAllParaEquipState, 0x00, sizeof(g_aAllParaEquipState));
    memset(g_aSomeParaCallEquipState, 0x00, sizeof(g_aSomeParaCallEquipState));

    //��ֵ��ز�����ʼ��
    memset(g_aParaSetValueZhj, 0x00, sizeof(g_aParaSetValueZhj));
    memset(g_nFixedAreaIdZhj, 0x00, sizeof(g_nFixedAreaIdZhj));
    memset(g_bFixedChangeFlagZhj, 0x00, sizeof(g_bFixedChangeFlagZhj));

    memset(g_aFixedAreaNumZhj, 0x00, sizeof(g_aFixedAreaNumZhj));
    memset(g_aFixedValueAreaZhj, 0x00, sizeof(g_aFixedValueAreaZhj));
    memset(g_aFixedValueStateZhj, 0x00, sizeof(g_aFixedValueStateZhj));
    memset(g_nParaFixedFlagZhj, 0x00, sizeof(g_nParaFixedFlagZhj));

    //��ֵ��������Ĭ�϶�ֵ����ʼ��
    for(i=0; i<MAX_EQUIP_TYPE_NUM; i++)
    {
        for(j=0; j<g_aGroupCountZhj[i]; j++)
        {
            for(k=0; k<g_aEquipCountZhj[i][j];k++)
            {
                g_nFixedAreaIdZhj[i][j][k] = 0;//Ĭ�ϲ���0�Ŷ�ֵ�� ��0��ʼ
                g_aFixedAreaNumZhj[i][j][k] = MAX_FIXED_AREA_NUM;
            }
        }
    }

    //��ֵ��������ʶ��ʼ��
    uint32 IdenIndex = 0;
    for(m=0; m<MAX_FIXED_AREA_NUM; m++)
    {
        IdenIndex = 0;
        for(i=0; i<MAX_EQUIP_TYPE_NUM; i++)
        {
            for(j=0; j<g_aGroupCountZhj[i]; j++)
            {
                for(k=0; k<g_aEquipCountZhj[i][j];k++)
                {
                    g_aFixedValueAreaZhj[m][IdenIndex].nEqType = i+1;
                    g_aFixedValueAreaZhj[m][IdenIndex].nEqGroup = j;
                    g_aFixedValueAreaZhj[m][IdenIndex].nEquipNo = k+1;
                    
                    g_aFixedValueAreaZhj[m][IdenIndex].nFixedAreaID = m;//��ֵ���� ��0��ʼ
                    g_aFixedValueAreaZhj[m][IdenIndex].nSetState = 0xaa;
                    
                    g_aFixedAreaIndexZhj[m][i][j][k] = IdenIndex;
                    IdenIndex += 1;
                }
            }
        }
    }

    time(&nTmnStartTime_t);  //��ȡtime_t���͵ĵ�ǰʱ��
    past_time_init();  //ʱ������ʼ��
    past_time_init_down();  //ʱ������ʼ��
	task_run_step_init_zhj();  //104����ִ������ȫ�ֱ�����ʼ��
	//scheme_coload_init_zhj();  //��ͨ�ɼ�������¼��ѡ���ʼ��

    //��������/����ĩʱ��
    nPreDayTime_t = nTmnStartTime_t - (g_stDownCurBinTime.Hour*60*60 + g_stDownCurBinTime.Minute*60 + g_stDownCurBinTime.Second + 10);  //ǰһ��23:59:50ת��������
    nPreMonTime_t = nTmnStartTime_t - ((g_stDownCurBinTime.Day - 1)*24*60*60 + g_stDownCurBinTime.Hour*60*60 + g_stDownCurBinTime.Minute*60 + g_stDownCurBinTime.Second + 10);  //��һ�����һ��23:59:50ת��������
	
	return 1;
}

/*******************************************************************************
* ��������: para_query_int64_zhj
* ��������: ���Ͷ�ֵ������ѯ
* �������: typeIndex         �豸���������±�, ��0��ʼ
            groupIndex    �豸��������±�, ��0��ʼ
            seqIndex      ����ƫ�Ƶ�ַ�����±�, ��0��ʼ
            nParaAddr     ������������ֽڵ�ַ
* �������: nOut              ���Ͷ�ֵ����
* �� �� ֵ: 1���ɹ���-1��ʧ��
*******************************************************************************/
int para_query_int64_zhj(uint16 typeIndex, uint16 groupIndex, uint16 seqIndex, uint32 nParaAddr, int64 *nOut)
{
	int i = 0;
	int64   nParaValue = 0;
	BOOL    bSuccess   = FALSE;

    int m = typeIndex;//�豸���������±�
    int x = groupIndex;//�豸��������±�
    int n=  seqIndex;//�豸����±�
    uint16 nAreaId = g_nFixedAreaIdZhj[m][x][n];//��ֵ����

    //�ж��豸�����Ƿ�Ϸ�
    if(typeIndex >= MAX_EQUIP_TYPE_NUM)
    {
        return -1;
    }

    //�ж��豸����Ƿ�Ϸ�
    if(groupIndex >= MAX_EQUIP_GROUP_NUM)
    {
        return -1;
    }

    //�ж��豸����Ƿ�Ϸ�
    if(seqIndex >= MAX_EQUIP_COUNT_NUM)
    {
        return -1;
    }

    uint32 ArrIdx = 0;//һά��������(��ʼ)
    ArrIdx = g_aParaValueIndexZhj[m][x][n];

	for(i=0; i<g_nConfigParaNumZhj[m][x][n]; i++)
	{
		if(g_aParaSetValueZhj[nAreaId][ArrIdx+i].nObjAddrU32 == nParaAddr)
		{
			switch(g_aParaSetValueZhj[nAreaId][ArrIdx+i].nTagType)
			{
				case TAG_NONE:
				{
					nParaValue = (int64)g_aParaSetValueZhj[nAreaId][ArrIdx+i].fDefaultValue;
				}
					break;
				case TAG_BOOL:
				{
					nParaValue = (int64)g_aParaSetValueZhj[nAreaId][ArrIdx+i].unValue.boolV;
				}
					break;
				case TAG_INT8:
				{
					nParaValue = (int64)g_aParaSetValueZhj[nAreaId][ArrIdx+i].unValue.int8V;
				}
					break;
				case TAG_UINT8:
				{
					nParaValue = (int64)g_aParaSetValueZhj[nAreaId][ArrIdx+i].unValue.uint8V;
				}
					break;
				case TAG_INT16:
				{
					nParaValue = (int64)g_aParaSetValueZhj[nAreaId][ArrIdx+i].unValue.int16V;
				}
					break;
				case TAG_UINT16:
				{
					nParaValue = (int64)g_aParaSetValueZhj[nAreaId][ArrIdx+i].unValue.uint16V;
				}
					break;
				case TAG_INT32:
				{
					nParaValue = (int64)g_aParaSetValueZhj[nAreaId][ArrIdx+i].unValue.int32V;
				}
					break;
				case TAG_UINT32:
				{
					nParaValue = (int64)g_aParaSetValueZhj[nAreaId][ArrIdx+i].unValue.uint32V;
				}
					break;
				case TAG_INT64:
				{
					nParaValue = (int64)g_aParaSetValueZhj[nAreaId][ArrIdx+i].unValue.int64V;
				}
					break;
				case TAG_UINT64:
				{
					nParaValue = (int64)g_aParaSetValueZhj[nAreaId][ArrIdx+i].unValue.uint64V;
				}
					break;
				case TAG_FLOAT32:
				{
					nParaValue = (int64)g_aParaSetValueZhj[nAreaId][ArrIdx+i].unValue.float32V;
				}
					break;
				case TAG_FLOAT64:
				{
					nParaValue = (int64)g_aParaSetValueZhj[nAreaId][ArrIdx+i].unValue.float64V;
				}
					break;
				default:
					nParaValue = (int64)g_aParaSetValueZhj[nAreaId][ArrIdx+i].fDefaultValue;
		            break;
			}

            DB104_FMT_LOGCC("para_query_int64_zhj() bSuccess=TRUE g_aParaSetValue[%d][%d][%d][%d][%d].nTagType=%d, nParaValue=%d, arrayV: \n", 
				m, x, n, nAreaId, i, g_aParaSetValueZhj[nAreaId][ArrIdx+i].nTagType, nParaValue);
			DB104_BUF_LOGCC(&g_aParaSetValueZhj[nAreaId][ArrIdx+i].unValue.arrayV[0], MAX_FIXED_VALUE_LEN, "����: ");
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
* ��������: para_query_float32_zhj
* ��������: �����Ͷ�ֵ������ѯ
* �������: typeIndex         �豸���������±�, ��0��ʼ
            groupIndex    �豸��������±�, ��0��ʼ
            seqIndex      ����ƫ�Ƶ�ַ�����±�, ��0��ʼ
            nParaAddr     ������������ֽڵ�ַ
* �������: fOut              �����Ͷ�ֵ����
* �� �� ֵ: 1���ɹ���-1��ʧ��
*******************************************************************************/
int para_query_float32_zhj(uint16 typeIndex, uint16 groupIndex, uint16 seqIndex, uint32 nParaAddr, float32 *fOut)
{
	int i = 0;
	float32 fParaValue = 0.0;
	BOOL    bSuccess   = FALSE;

    int m = typeIndex;//�豸���������±�
    int x = groupIndex;//�豸��������±�
    int n=  seqIndex;//�豸����±�
    uint16 nAreaId = g_nFixedAreaIdZhj[m][x][n];//��ֵ����

    //�ж��豸�����Ƿ�Ϸ�
    if(typeIndex >= MAX_EQUIP_TYPE_NUM)
    {
        return -1;
    }

    //�ж��豸����Ƿ�Ϸ�
    if(groupIndex >= MAX_EQUIP_GROUP_NUM)
    {
        return -1;
    }

    //�ж��豸����Ƿ�Ϸ�
    if(seqIndex >= MAX_EQUIP_COUNT_NUM)
    {
        return -1;
    }

    uint32 ArrIdx = 0;//һά��������(��ʼ)
    ArrIdx = g_aParaValueIndexZhj[m][x][n];

	for(i=0; i<g_nConfigParaNumZhj[m][x][n]; i++)
	{
		if(g_aParaSetValueZhj[nAreaId][ArrIdx+i].nObjAddrU32 == nParaAddr)
		{
			switch(g_aParaSetValueZhj[nAreaId][ArrIdx+i].nTagType)
			{
				case TAG_NONE:
				{
					fParaValue = (float32)g_aParaSetValueZhj[nAreaId][ArrIdx+i].fDefaultValue;
				}
					break;
				case TAG_BOOL:
				{
					fParaValue = (float32)g_aParaSetValueZhj[nAreaId][ArrIdx+i].unValue.boolV;
				}
					break;
				case TAG_INT8:
				{
					fParaValue = (float32)g_aParaSetValueZhj[nAreaId][ArrIdx+i].unValue.int8V;
				}
					break;
				case TAG_UINT8:
				{
					fParaValue = (float32)g_aParaSetValueZhj[nAreaId][ArrIdx+i].unValue.uint8V;
				}
					break;
				case TAG_INT16:
				{
					fParaValue = (float32)g_aParaSetValueZhj[nAreaId][ArrIdx+i].unValue.int16V;
				}
					break;
				case TAG_UINT16:
				{
					fParaValue = (float32)g_aParaSetValueZhj[nAreaId][ArrIdx+i].unValue.uint16V;
				}
					break;
				case TAG_INT32:
				{
					fParaValue = (float32)g_aParaSetValueZhj[nAreaId][ArrIdx+i].unValue.int32V;
				}
					break;
				case TAG_UINT32:
				{
					fParaValue = (float32)g_aParaSetValueZhj[nAreaId][ArrIdx+i].unValue.uint32V;
				}
					break;
				case TAG_INT64:
				{
					fParaValue = (float32)g_aParaSetValueZhj[nAreaId][ArrIdx+i].unValue.int64V;
				}
					break;
				case TAG_UINT64:
				{
					fParaValue = (float32)g_aParaSetValueZhj[nAreaId][ArrIdx+i].unValue.uint64V;
				}
					break;
				case TAG_FLOAT32:
				{
					fParaValue = (float32)g_aParaSetValueZhj[nAreaId][ArrIdx+i].unValue.float32V;
				}
					break;
				case TAG_FLOAT64:
				{
					fParaValue = (float32)g_aParaSetValueZhj[nAreaId][ArrIdx+i].unValue.float64V;
				}
					break;
				default:
					fParaValue = (float32)g_aParaSetValueZhj[nAreaId][ArrIdx+i].fDefaultValue;
		            break;
			}

			DB104_FMT_LOGCC("para_query_float32_zhj() bSuccess=TRUE g_aParaSetValue[%d][%d][%d][%d][%d].nTagType=%d, fParaValue=%f, arrayV: \n", 
				m, x, n, nAreaId, i, g_aParaSetValueZhj[nAreaId][ArrIdx+i].nTagType, fParaValue);
			DB104_BUF_LOGCC(&g_aParaSetValueZhj[nAreaId][ArrIdx+i].unValue.arrayV[0], MAX_FIXED_VALUE_LEN, "����: ");
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
* ��������: para_query_array_zhj
* ��������: �ַ����Ͷ�ֵ������ѯ
* �������: typeIndex         �豸���������±�, ��0��ʼ
            groupIndex    �豸��������±�, ��0��ʼ
            seqIndex      ����ƫ�Ƶ�ַ�����±�, ��0��ʼ
            nParaAddr     ������������ֽڵ�ַ
* �������: pOut              �ַ�����ֵ����
* �� �� ֵ: ����0���ַ������ȣ�-1��ʧ��
*******************************************************************************/
int para_query_array_zhj(uint16 typeIndex, uint16 groupIndex, uint16 seqIndex, uint32 nParaAddr, uint8 *pOut)
{
	int i = 0;
    int nArrLen = -1;

    int m = typeIndex;//�豸���������±�
    int x = groupIndex;//�豸��������±�
    int n=  seqIndex;//�豸����±�
    uint16 nAreaId = g_nFixedAreaIdZhj[m][x][n];//��ֵ����

    //�ж��豸�����Ƿ�Ϸ�
    if(typeIndex >= MAX_EQUIP_TYPE_NUM)
    {
        return -1;
    }

    //�ж��豸����Ƿ�Ϸ�
    if(groupIndex >= MAX_EQUIP_GROUP_NUM)
    {
        return -1;
    }

    //�ж��豸����Ƿ�Ϸ�
    if(seqIndex >= MAX_EQUIP_COUNT_NUM)
    {
        return -1;
    }

    uint32 ArrIdx = 0;//һά��������(��ʼ)
    ArrIdx = g_aParaValueIndexZhj[m][x][n];

	for(i=0; i<g_nConfigParaNumZhj[m][x][n]; i++)
	{
		if(g_aParaSetValueZhj[nAreaId][ArrIdx+i].nObjAddrU32 == nParaAddr)
		{
			switch(g_aParaSetValueZhj[nAreaId][ArrIdx+i].nTagType)
			{
				case TAG_STRING:
                case TAG_OCTSTRING_ZHJ:
				{
					nArrLen = g_aParaSetValueZhj[nAreaId][ArrIdx+i].nArrLen;
                    if(nArrLen > 0)
                    {
                        memcpy(pOut, g_aParaSetValueZhj[nAreaId][ArrIdx+i].aArrValue, nArrLen);
                    }
				}
					break;
				default:
					break;
			}

			DB104_FMT_LOGCC("para_query_array_zhj() bSuccess=TRUE g_aParaSetValue[%d][%d][%d][%d][%d].nTagType=%d, nArrLen=%d, arrayV: \n", 
				m, x, n, nAreaId, i, g_aParaSetValueZhj[nAreaId][ArrIdx+i].nTagType, nArrLen);
            if(nArrLen > 0)
            {
                DB104_BUF_LOGCC(&g_aParaSetValueZhj[nAreaId][ArrIdx+i].aArrValue, nArrLen, "����: ");
            }
			break;
		}
	}

	return nArrLen;
}

/*******************************************************************************
* ��������: yx_soe_buff_combine_zhj
* ��������: ��װң�ű�λSOE����
* �������: ��
* �������: ��
* �� �� ֵ: ��
*******************************************************************************/
int yx_soe_buff_combine_zhj(uint8 eqTpIndex, uint8 eqGroupIndex, uint16 eqNoIndex, uint16 addrIndex, TOTAL_104_FRAME_T *pTotal104Frame)
{
    uint8 addr[4] = {0};//��Ϣ���ַ
    int index = 0;//ƫ��λ
    uint32 ArrIdx = 0;//һά��������(��ʼ)
    
    TOTAL_104_FRAME_T stTotal104Frame;
    memset(&stTotal104Frame, 0, sizeof(TOTAL_104_FRAME_T));

    //HEAD
    stTotal104Frame.nHead = 0x68;

    //������(��ʱ����)
    
    //��������
    stTotal104Frame.nDataType = M_SP_TB_1;

    //VSQ
    stTotal104Frame.nVSQ = 0x01;

    //COT
    stTotal104Frame.nTransCauseL = CAUSE_SPONT;
    stTotal104Frame.nTransCauseH = 0x00;

    //������ַ
    stTotal104Frame.nPubAddrL = eqNoIndex + 1;
    stTotal104Frame.nPubAddrH = (uint8)((uint8)(eqTpIndex+1) << 3) + eqGroupIndex;

    //��Ϣ���ַ
    ArrIdx = g_aYxDataIndexZhj[eqTpIndex][eqGroupIndex][eqNoIndex];
	memcpy(addr, &g_aYxDataConfigZhj[ArrIdx+addrIndex].nObjAddrU32, 4);
	stTotal104Frame.aObjData[index++] = addr[0];
	stTotal104Frame.aObjData[index++] = addr[1];
	stTotal104Frame.aObjData[index++] = addr[2];
	//����ң����ֵ
	stTotal104Frame.aObjData[index++] = g_aYxDataSendZhj[ArrIdx+addrIndex].nNewValue;
	//��ʱ��
	memcpy(&stTotal104Frame.aObjData[index], &g_aYxDataSendZhj[ArrIdx+addrIndex].btime[0], 7);
	index += 7;

    //���±��ĳ���(������ͷ���ͳ�������)
    stTotal104Frame.nLength = CTRL_UNIT_LABEL_LEN + index;

    memcpy(pTotal104Frame, &stTotal104Frame, sizeof(TOTAL_104_FRAME_T));
    return 1;
}

/*******************************************************************************
* ��������: yx_cos_buff_combine_zhj
* ��������: ��װң�ű�λcos����
* �������: ��
* �������: ��
* �� �� ֵ: ��
*******************************************************************************/
int yx_cos_buff_combine_zhj(uint8 eqTpIndex, uint8 eqGroupIndex, uint16 eqNoIndex, uint16 addrIndex, TOTAL_104_FRAME_T *pTotal104Frame)
{
    uint8 addr[4] = {0};//��Ϣ���ַ
    int index = 0;//ƫ��λ
    uint32 ArrIdx = 0;//һά��������(��ʼ)
    
    TOTAL_104_FRAME_T stTotal104Frame;
    memset(&stTotal104Frame, 0, sizeof(TOTAL_104_FRAME_T));

    //HEAD
    stTotal104Frame.nHead = 0x68;

    //������(��ʱ����)
    
    //��������
    stTotal104Frame.nDataType = M_SP_NA_1;

    //VSQ
    stTotal104Frame.nVSQ = 0x01;

    //COT
    stTotal104Frame.nTransCauseL = CAUSE_SPONT;
    stTotal104Frame.nTransCauseH = 0x00;

    //������ַ
    stTotal104Frame.nPubAddrL = eqNoIndex + 1;
    stTotal104Frame.nPubAddrH = (uint8)((uint8)(eqTpIndex+1) << 3) + eqGroupIndex;

    //��Ϣ���ַ
	ArrIdx = g_aYxDataIndexZhj[eqTpIndex][eqGroupIndex][eqNoIndex];
	memcpy(addr, &g_aYxDataConfigZhj[ArrIdx+addrIndex].nObjAddrU32, 4);
	stTotal104Frame.aObjData[index++] = addr[0];
	stTotal104Frame.aObjData[index++] = addr[1];
	stTotal104Frame.aObjData[index++] = addr[2];
	//����ң����ֵ
	stTotal104Frame.aObjData[index++] = g_aYxDataSendZhj[ArrIdx+addrIndex].nNewValue;

    //���±��ĳ���(������ͷ���ͳ�������)
    stTotal104Frame.nLength = CTRL_UNIT_LABEL_LEN + index;

    memcpy(pTotal104Frame, &stTotal104Frame, sizeof(TOTAL_104_FRAME_T));
    return 1;
}


/*******************************************************************************
* ��������: yx_real_signal_update_zhj
* ��������: ͨ��mqtt��Ϣ��ȡӲң���ź�
* �������: ��
* �������: ��
* �� �� ֵ: ��
*******************************************************************************/
void yx_real_signal_update_zhj(RST_RMT_T stRmt, uint8 status)
{
    int m = 0;//�豸����
    int x = 0;//�豸���
    int n = 0;//�豸���
	int i = 0;
    uint16 index = 0;
    uint32 ArrIdx = 0;//һά��������(��ʼ)
    
    TOTAL_104_FRAME_T stTotal104Frame;
    static TOTAL_104_FRAME_T tmpTotal104Frame;
    static int count = 0;
    
    memset(&stTotal104Frame, 0, sizeof(TOTAL_104_FRAME_T));
    
    for(m=0; m<MAX_EQUIP_TYPE_NUM; m++)//����
    {
        if(0 == g_aGroupCountZhj[m])
        {
            continue;//�����͵�������Ϊ0
        }
        
        for(x=0; x<g_aGroupCountZhj[m]; x++)//���
        {
            if(0 == g_aEquipCountZhj[m][x])
            {
                continue;//�����͵��豸����Ϊ0
            }

            for(n=0; n<g_aEquipCountZhj[m][x]; n++)
            {
                ArrIdx = g_aYxDataIndexZhj[m][x][n];
                for(i=0; i<g_nConfigYxNumZhj[m][x][n]; i++)
            	{
            		if(g_aYxDataConfigZhj[ArrIdx+i].nDbCenterOAD == 0xFFFFFFFF)
            		{
            			if(g_aYxDataConfigZhj[ArrIdx+i].nDealType == CALC_YX_REAL_SIGNAL)
            			{
            			    index = i;
                            
            			    //���ݸ�������
            			    system_time_get(&g_aYxDataSendZhj[ArrIdx+index].btime[0]);//��λʱ���¼
                        	g_aYxDataSendZhj[ArrIdx+index].nNewValue = (uint8)( (stRmt.ST >> g_aYxDataConfigZhj[ArrIdx+index].nItemIndex)&0x01 );
                            if(g_aYxDataSendZhj[ArrIdx+index].nNewValue)//Ӳң�� 1��ʾ�� 0��ʾ��
                            {
                                g_aYxDataSendZhj[ArrIdx+index].nNewValue = 0;
                            }
                            else
                            {
                                g_aYxDataSendZhj[ArrIdx+index].nNewValue = 1;
                            }
                            DB104_FMT_TRACE("Ӳң�� m=%d, x=%d, n=%d, g_aYxDataSend[%d]: nNewValue=%d, nHisValue=%d, stRmt.ST=0x%02x, status=0x%02x \n", 
                            		m, x, n, index, g_aYxDataSendZhj[ArrIdx+index].nNewValue, g_aYxDataSendZhj[ArrIdx+index].nHisValue, stRmt.ST, status);
                            
                        	if(g_aYxDataSendZhj[ArrIdx+index].nNewValue != g_aYxDataSendZhj[ArrIdx+index].nHisValue)
                        	{
                        	    DB104_FMT_TRACE("ң�ű�λ, ����SOE��COS! \n");

                                count++;
                                if (count == 1)
                                {
                                    memset(&stTotal104Frame, 0, sizeof(TOTAL_104_FRAME_T));
                                    yx_soe_buff_combine_zhj(m, x, n, index, &stTotal104Frame);
                                    memcpy(&tmpTotal104Frame, &stTotal104Frame, sizeof(TOTAL_104_FRAME_T));
                                    DB104_BUF_TRACE((uint8 *)&stTotal104Frame, stTotal104Frame.nLength+2, "��ʱSOE����:");
                                }

                                memset(&stTotal104Frame, 0, sizeof(TOTAL_104_FRAME_T));
                                yx_cos_buff_combine_zhj(m, x, n, index, &stTotal104Frame);
                                pw_lock_queue_write(&g_stSoeBuff, (uint8 *)&stTotal104Frame, stTotal104Frame.nLength+2, 0);
                                DB104_BUF_TRACE((uint8 *)&stTotal104Frame, stTotal104Frame.nLength+2, "COS����:");
                                
                                if (count >= 2) /* SOE����Я�������ͻָ�����Ϣ */
                                {
                                    memset(&stTotal104Frame, 0, sizeof(TOTAL_104_FRAME_T));
                                    yx_soe_buff_combine_zhj(m, x, n, index, &stTotal104Frame);
                                    stTotal104Frame.nVSQ = 0x02;
                                    memcpy(&stTotal104Frame.aObjData[11], stTotal104Frame.aObjData, 11);
                                    memcpy(stTotal104Frame.aObjData, tmpTotal104Frame.aObjData, 11);
                                    stTotal104Frame.nLength += 11; 
                                    pw_lock_queue_write(&g_stSoeBuff, (uint8 *)&stTotal104Frame, stTotal104Frame.nLength+2, 0);
                                    DB104_BUF_TRACE((uint8 *)&stTotal104Frame, stTotal104Frame.nLength+2, "SOE����:");
                                    count = 0;
                                    memset(&tmpTotal104Frame, 0, sizeof(TOTAL_104_FRAME_T));
                                }

                                g_aYxDataSendZhj[ArrIdx+index].nHisValue = g_aYxDataSendZhj[ArrIdx+index].nNewValue;
                        	}
            			}
            		}
            	}
            }
        }
    }

}

/*******************************************************************************
* ��������: yx_event_signal_update_zhj
* ��������: �����¼�������ص�ң��״̬
* �������: typeIndex         �豸���������±�, ��0��ʼ
            groupIndex    �豸��������±�, ��0��ʼ
            seqIndex      ����ƫ�Ƶ�ַ�����±�, ��0��ʼ
            nAddr         ң�ŵ�������ֽڵ�ַ
            nNewState     �¼�״̬ 1���� 0�ָ�
* �������: ��
* �� �� ֵ: 1���ɹ���-1��ʧ��
*******************************************************************************/
int yx_event_signal_update_zhj(uint16 typeIndex, uint16 groupIndex, uint16 seqIndex, uint32 nAddr, uint8 nNewState)
{
	int m = typeIndex;//�豸����-1
    int x = groupIndex;//�豸��� ��0��ʼ
    int n=  seqIndex;//�豸���-1
    int index = -1;//λ�����õ���е����
    uint32 ArrIdx = 0;//һά��������(��ʼ)
    
    TOTAL_104_FRAME_T stTotal104Frame;
    memset(&stTotal104Frame, 0, sizeof(TOTAL_104_FRAME_T));
    
    index = query_yx_index_config_zhj(typeIndex, groupIndex, seqIndex, nAddr);
    if(index < 0)
    {
        DB104_FMT_TRACE("�¼�ң�� typeIndex=%d, groupIndex=%d, seqIndex=%d, nAddr=%08x, nNewState=%d ����!\n", 
		    m, x, n, nAddr, nNewState);
        return -1;
    }

    ArrIdx = g_aYxDataIndexZhj[m][x][n];
    DB104_FMT_TRACE("�¼�ң�� typeIndex=%d, groupIndex=%d, seqIndex=%d, nAddr=%08x, index=%d, nNewState=%d \n", 
		    typeIndex, groupIndex, seqIndex, nAddr, index, nNewState);

    //���ݸ�������
    system_time_get(&g_aYxDataSendZhj[ArrIdx+index].btime[0]);
	g_aYxDataSendZhj[ArrIdx+index].nNewValue = nNewState>0? 1: 0;//1���� 0�ָ�
	DB104_FMT_TRACE("�¼�ң�� m=%d, x=%d, n=%d, g_aYxDataSend[%d]: nNewValue=%d, nHisValue=%d \n", 
		m, x, n, index, g_aYxDataSendZhj[ArrIdx+index].nNewValue, g_aYxDataSendZhj[ArrIdx+index].nHisValue);

    if((g_aYxDataSendZhj[ArrIdx+index].nNewValue != g_aYxDataSendZhj[ArrIdx+index].nHisValue) || 
        (nAddr == 0x00100005 && nNewState == 0)) // ����ն��ϵ����⴦��
	{
		DB104_FMT_TRACE("ң�ű�λ, ����SOE��COS! \n");

        memset(&stTotal104Frame, 0, sizeof(TOTAL_104_FRAME_T));
        yx_soe_buff_combine_zhj(m, x, n, index, &stTotal104Frame);
        pw_lock_queue_write(&g_stSoeBuff, (uint8 *)&stTotal104Frame, stTotal104Frame.nLength+2, 0);
        DB104_BUF_TRACE((uint8 *)&stTotal104Frame, stTotal104Frame.nLength+2, "��ʱSOE����:");

        memset(&stTotal104Frame, 0, sizeof(TOTAL_104_FRAME_T));
        yx_cos_buff_combine_zhj(m, x, n, index, &stTotal104Frame);
        pw_lock_queue_write(&g_stSoeBuff, (uint8 *)&stTotal104Frame, stTotal104Frame.nLength+2, 0);
        DB104_BUF_TRACE((uint8 *)&stTotal104Frame, stTotal104Frame.nLength+2, "��ʱCOS����:");

        g_aYxDataSendZhj[ArrIdx+index].nHisValue = g_aYxDataSendZhj[ArrIdx+index].nNewValue;
	}
	
	return 1;
}

/*******************************************************************************
* ��������: yx_db_center_read_zhj
* ��������: ���������Ķ�ȡң����Ϣ
* �������: ��
* �������: ��
* �� �� ֵ: ��
*******************************************************************************/
void yx_db_center_read_zhj(uint8 typeIndex, uint16 groupIndex, uint16 seqIndex, uint16 index)
{

}

/*******************************************************************************
* ��������: yx_real_signal_read_zhj
* ��������: ͨ��mqtt��Ϣ��ȡӲң���ź�
* �������: ��
* �������: ��
* �� �� ֵ: ��
*******************************************************************************/
void yx_real_signal_read_zhj(uint8 typeIndex, uint16 groupIndex, uint16 seqIndex, uint16 index)
{
    int m = typeIndex;//�豸����-1
    int x = groupIndex;//�豸��� ��0��ʼ
    int n=  seqIndex;//�豸���-1
    uint32 ArrIdx = 0;//һά��������(��ʼ)
    
	if(index >= g_nConfigYxNumZhj[m][x][n])
	{
		return;
	}

	RST_RMT_T stRmt;
	uint8 status = 0;

	memset(&stRmt, 0x00, sizeof(RST_RMT_T));
	status = advPd_get_rmt(&stRmt);

	//���ݸ�������
	ArrIdx = g_aYxDataIndexZhj[m][x][n];
	g_aYxDataSendZhj[ArrIdx+index].nNewValue = (uint8)( (stRmt.ST >> g_aYxDataConfigZhj[ArrIdx+index].nItemIndex)&0x01 );
	if(g_aYxDataSendZhj[ArrIdx+index].nNewValue != g_aYxDataSendZhj[ArrIdx+index].nHisValue)
	{
		g_aYxDataSendZhj[ArrIdx+index].bChangeFlag = 1;
	}
	
	//��ʱ��ֵ, ���Ա仯����
	if(g_bTestEnable)
	{
		g_aYxDataSendZhj[ArrIdx+index].nNewValue = rand()%2;//��2ȡ��õ�����0����1
		g_aYxDataSendZhj[ArrIdx+index].bChangeFlag = 1;
		DB104_FMT_TRACE("ң������� g_aYxDataSend[%d].nNewValue=%d \n", index, g_aYxDataSendZhj[ArrIdx+index].nNewValue);
	}
	
	if(g_aYxDataSendZhj[ArrIdx+index].bChangeFlag)
	{
		g_bYxChanged = TRUE;
		g_aYxDataSendZhj[ArrIdx+index].bSendFlag = FALSE;
        g_aChangedEquipStateYX[m][x][n] = 1;
	}

    DB104_FMT_TRACE("Ӳң�� m=%d, x=%d, n=%d, g_aYxDataSend[%d]: nNewValue=%d, nHisValue=%d, bChangeFlag=%d, bSendFlag=%d, ȫ��g_bYxChanged=%d, status=0x%02x \n", 
    		m, x, n, index, g_aYxDataSendZhj[ArrIdx+index].nNewValue, g_aYxDataSendZhj[ArrIdx+index].nHisValue, g_aYxDataSendZhj[ArrIdx+index].bChangeFlag, 
    		g_aYxDataSendZhj[ArrIdx+index].bSendFlag, g_bYxChanged, status);

	system_time_get(&g_aYxDataSendZhj[ArrIdx+index].btime[0]);
}

/*******************************************************************************
* ��������: yx_calculate_statistics
* ��������: ��Ҫͳ�ƻ�����ң���ź�
* �������: ��
* �������: ��
* �� �� ֵ: ��
*******************************************************************************/
void yx_calculate_statistics_zhj(uint8 typeIndex, uint16 groupIndex, uint16 seqIndex, uint16 index)
{
	int m = typeIndex;//�豸����-1
    int x = groupIndex;//�豸��� ��0��ʼ
    int n=  seqIndex;//�豸���-1
    uint8 nNewState = 0;
    uint32 ArrIdx = 0;//һά��������(��ʼ)
    
	if(index >= g_nConfigYxNumZhj[m][x][n])
	{
		return;
	}

    //���ݸ�������
    ArrIdx = g_aYxDataIndexZhj[m][x][n];
    switch(g_aYxDataConfigZhj[ArrIdx+index].nDealType)
    {
        case 0x0001000B://���ڹ���Խ��
        {
            nNewState = 0;
        }
            break;
        
        default://��������ң���ݲ�������
            return;
            break;
    }   

	//���ݸ�������
	g_aYxDataSendZhj[ArrIdx+index].nNewValue = nNewState;
	if(g_aYxDataSendZhj[ArrIdx+index].nNewValue != g_aYxDataSendZhj[ArrIdx+index].nHisValue)
	{
		g_aYxDataSendZhj[ArrIdx+index].bChangeFlag = 1;
	}
	
	//��ʱ��ֵ, ���Ա仯����
	if(g_bTestEnable)
	{
		g_aYxDataSendZhj[ArrIdx+index].nNewValue = rand()%2;//��2ȡ��õ�����0����1
		g_aYxDataSendZhj[ArrIdx+index].bChangeFlag = 1;
		DB104_FMT_TRACE("ң������� g_aYxDataSend[%d].nNewValue=%d \n", index, g_aYxDataSendZhj[ArrIdx+index].nNewValue);
	}
	
	if(g_aYxDataSendZhj[ArrIdx+index].bChangeFlag)
	{
		g_bYxChanged = TRUE;
		g_aYxDataSendZhj[ArrIdx+index].bSendFlag = FALSE;
        g_aChangedEquipStateYX[m][x][n] = 1;
	}

	DB104_FMT_TRACE("����ң�� m=%d, x=%d, n=%d, g_aYxDataSend[%d]: nNewValue=%d, nHisValue=%d, bChangeFlag=%d, bSendFlag=%d, ȫ��g_bYxChanged=%d, nNewState=0x%02x \n", 
		m, x, n, index, g_aYxDataSendZhj[ArrIdx+index].nNewValue, g_aYxDataSendZhj[ArrIdx+index].nHisValue, g_aYxDataSendZhj[ArrIdx+index].bChangeFlag, 
		g_aYxDataSendZhj[ArrIdx+index].bSendFlag, g_bYxChanged, nNewState);

	system_time_get(&g_aYxDataSendZhj[ArrIdx+index].btime[0]);
}

/*******************************************************************************
* ��������: yx_event_occur_set_zhj
* ��������: �����¼��ж����ɵ�ң�ű�λ
* �������: ��
* �������: ��
* �� �� ֵ: ��
*******************************************************************************/
int yx_event_occur_set_zhj(uint16 typeIndex, uint16 groupIndex, uint16 seqIndex,uint32 nAddr, uint8 nNewState)
{
	int m = typeIndex;//�豸����-1
    int x = groupIndex;//�豸��� ��0��ʼ
    int n=  seqIndex;//�豸���-1
    int index = -1;//λ�����õ���е����
	uint32 ArrIdx = 0;//һά��������(��ʼ)
    
    index = query_yx_index_config_zhj(typeIndex, groupIndex, seqIndex, nAddr);
    if(index < 0)
    {
        DB104_FMT_TRACE("�¼�ң�� m=%d, x=%d, n=%d, nAddr=%08x, nNewState=%d \n", 
		m, x, n, nAddr, nNewState);
        return -1;
    }

	ArrIdx = g_aYxDataIndexZhj[m][x][n];
    //���ݸ�������
	g_aYxDataSendZhj[ArrIdx+index].nNewValue = nNewState>0? 1: 0;
	if(g_aYxDataSendZhj[ArrIdx+index].nNewValue != g_aYxDataSendZhj[ArrIdx+index].nHisValue)
	{
		g_aYxDataSendZhj[ArrIdx+index].bChangeFlag = 1;
	}
	
	//��ʱ��ֵ, ���Ա仯����
	if(g_bTestEnable)
	{
		g_aYxDataSendZhj[ArrIdx+index].nNewValue = rand()%2;//��2ȡ��õ�����0����1
		g_aYxDataSendZhj[ArrIdx+index].bChangeFlag = 1;
		DB104_FMT_TRACE("ң������� g_aYxDataSend[%d].nNewValue=%d \n", index, g_aYxDataSendZhj[ArrIdx+index].nNewValue);
	}
	
	if(g_aYxDataSendZhj[ArrIdx+index].bChangeFlag)
	{
		g_bYxChanged = TRUE;
		g_aYxDataSendZhj[ArrIdx+index].bSendFlag = FALSE;
        g_aChangedEquipStateYX[m][x][n] = 1;
	}

	DB104_FMT_TRACE("�¼�ң�� m=%d, x=%d, n=%d, g_aYxDataSend[%d]: nNewValue=%d, nHisValue=%d, bChangeFlag=%d, bSendFlag=%d, ȫ��g_bYxChanged=%d, nNewState=0x%02x \n", 
		m, x, n, index, g_aYxDataSendZhj[ArrIdx+index].nNewValue, g_aYxDataSendZhj[ArrIdx+index].nHisValue, g_aYxDataSendZhj[ArrIdx+index].bChangeFlag, 
		g_aYxDataSendZhj[ArrIdx+index].bSendFlag, g_bYxChanged, nNewState);

	system_time_get(&g_aYxDataSendZhj[ArrIdx+index].btime[0]);
    return 1;
}

/*******************************************************************************
* ��������: data_block_split_zhj
* ��������: ���ݿ���
* �������: ��
* �������: ��
* �� �� ֵ: ��
*******************************************************************************/
int data_block_split_zhj(uint32 nOAD, uint8 nItemID, uint8 *buff, float32 *fOut, uint8 *timeFlag, OOP_DATETIME_S_T *pTime)
{
	uint16 nNum = 0;

	switch(nOAD)
	{
		case 0x00100200:			//(��ǰ) �����й������ܼ��ƽ��ʾֵ
        case 0x00200200:			//(��ǰ) �����й������ܼ��ƽ��ʾֵ
        case 0x00300200:			//(��ǰ) �����޹������ܼ��ƽ��ʾֵ
        case 0x00400200:			//(��ǰ) �����޹������ܼ��ƽ��ʾֵ
        case 0x00500200:			//(��ǰ) ��һ�����޹��ܼ��ƽ�ȵ���ʾֵ
        case 0x00600200:			//(��ǰ) �ڶ������޹��ܼ��ƽ�ȵ���ʾֵ
        case 0x00700200:			//(��ǰ) ���������޹��ܼ��ƽ�ȵ���ʾֵ
        case 0x00800200:			//(��ǰ) ���������޹��ܼ��ƽ�ȵ���ʾֵ
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
        case 0x20000400:			   //(��ǰ)�����ѹ
        {
			uint16 Voltage;
			memset((uint8*)&Voltage,0x00,sizeof(uint16));
            memcpy((uint8*)&Voltage,buff,sizeof(uint16));
			*fOut = (float32)Voltage;
		}
            break;
        case 0x20010600:				//(��ǰ)�������
        {
			int32 Current;
			memset((uint8*)&Current,0x00,sizeof(int32));
            memcpy((uint8*)&Current,buff,sizeof(int32));
			*fOut = (float32)Current;
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
			*fOut = (float32)Frequency+0.9;//��ʱ����, Ƶ�ʾ�ȷ��0.001
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
        
        case 0x10100200:                //(��ǰ)�����й��ܼ��ƽ���������
		case 0x10200200:                //(��ǰ)�����й��ܼ��ƽ���������
		case 0x10300200:                //(��ǰ)�����޹��ܼ��ƽ���������
		case 0x10400200:                //(��ǰ)�����޹��ܼ��ƽ���������
		case 0x10500200:                //(��ǰ)1�����޹��ܼ��ƽ���������
		case 0x10600200:                //(��ǰ)2�����޹��ܼ��ƽ���������
		case 0x10700200:                //(��ǰ)3�����޹��ܼ��ƽ���������
		case 0x10800200:                //(��ǰ)4�����޹��ܼ��ƽ���������
        {
            OOP_DEMAND_T Demand = {0};
            memcpy(&Demand, buff, sizeof(OOP_DEMAND_T));
			nNum = Demand.nNum;
            if(nItemID >= nNum) 
			{
				return -1;
			}
			*fOut = (float32)Demand.demand[nItemID].nValue;
            *timeFlag = 1;//��ʱ��
            memcpy(pTime, &Demand.demand[nItemID].time, sizeof(OOP_DATETIME_S_T));
		}
            break;
        case 0x201F0206:                //©���豸 ����ʱ��
        {
            uint32 RunTime = {0};
            memcpy(&RunTime, buff, sizeof(uint32));
			*fOut = (float32)RunTime;
		}
            break;
        case 0x201F0207:                //©���豸 �ʣ���������ֵ
        case 0x20010500:                //©���豸 ��ǰʣ�����
        {
            int16 Value = {0};
            memcpy(&Value, buff, sizeof(int16));
			*fOut = (float32)Value;
		}
            break;
        case 0x20020200:			   //(��ǰ)��ѹ���
        case 0x20030200:			   //(��ǰ)��ѹ�������
        case 0xAAAA0001:			   //(��ǰ)�������
		{
			OOP_WORD3V_T Angle = {0};
			memcpy(&Angle, buff, sizeof(OOP_WORD3V_T));
			nNum = Angle.nNum;
			if(nItemID >= nNum) 
			{
				return -1;
			}
			*fOut = (float32)Angle.nValue[nItemID];
		}
			break;
        case 0x20260200:						//��ѹ��ƽ����
        case 0x20270200:						//������ƽ����
		{
			uint16 ValueU16 = {0};
			memcpy(&ValueU16, buff, sizeof(uint16));
			*fOut = (float32)ValueU16;
		}
			break;
        case 0x200D0200:			   //(��ǰ)A�� ��ѹг�������ʣ��ܼ�2��n�Σ�
        case 0x200D0300:			   //(��ǰ)B�� ��ѹг�������ʣ��ܼ�2��n�Σ�
        case 0x200D0400:			   //(��ǰ)C�� ��ѹг�������ʣ��ܼ�2��n�Σ�
        case 0x200E0200:			   //(��ǰ)A�� ����г�������ʣ��ܼ�2��n�Σ�
        case 0x200E0300:			   //(��ǰ)B�� ����г�������ʣ��ܼ�2��n�Σ�
        case 0x200E0400:			   //(��ǰ)C�� ����г�������ʣ��ܼ�2��n�Σ�
		{
			OOP_HARMONIC_T Harmonic;
			memset((uint8*)&Harmonic,0x00,sizeof(OOP_HARMONIC_T));
			memcpy(&Harmonic, buff, sizeof(OOP_HARMONIC_T));
			nNum = Harmonic.nNum;
			if(nItemID >= nNum) 
			{
				return -1;
			}
			*fOut = (float32)Harmonic.nValue[nItemID];
		}
			break;
        case 0x20330200:			   //(��ǰ)A�� ��ѹг��������2��n�Σ�
        case 0x20330300:			   //(��ǰ)B�� ��ѹг��������2��n�Σ�
        case 0x20330400:			   //(��ǰ)C�� ��ѹг��������2��n�Σ�
		{
			OOP_HARMONIC_U_VALUE_T Harmonic;
			memset((uint8*)&Harmonic,0x00,sizeof(OOP_HARMONIC_U_VALUE_T));
			memcpy(&Harmonic, buff, sizeof(OOP_HARMONIC_U_VALUE_T));
			nNum = Harmonic.nNum;
			if(nItemID >= nNum) 
			{
				return -1;
			}
			*fOut = (float32)Harmonic.nValue[nItemID];
		}
			break;
        case 0x20340200:			   //(��ǰ)A�� ����г��������2��n�Σ�
        case 0x20340300:			   //(��ǰ)B�� ����г��������2��n�Σ�
        case 0x20340400:			   //(��ǰ)C�� ����г��������2��n�Σ�
		{
			OOP_HARMONIC_I_VALUE_T Harmonic;
			memset((uint8*)&Harmonic,0x00,sizeof(OOP_HARMONIC_I_VALUE_T));
			memcpy(&Harmonic, buff, sizeof(OOP_HARMONIC_I_VALUE_T));
			nNum = Harmonic.nNum;
			if(nItemID >= nNum) 
			{
				return -1;
			}
			*fOut = (float32)Harmonic.nValue[nItemID];
		}
			break;
		
		default:
			return -1;
            break;
	}

	return 1;
}

/*******************************************************************************
* ��������: sp_data_block_split_zhj
* ��������: ���ݿ��� �Զ������ݿ�
* �������: ��
* �������: ��
* �� �� ֵ: ��
*******************************************************************************/
int fvalue_get_by_choice_zhj(OOP_DIGIT_T srcData, float32 *fOut)
{
    switch(srcData.choice)
	{
		case DT_UNSIGNED:
		{
			*fOut = (float32)srcData.bVal;
		}
			break;
		case DT_INTEGER:
		{
			*fOut = (float32)srcData.cVal;
		}
			break;
        case DT_LONG:
		{
			*fOut = (float32)srcData.lVal;
		}
			break;
        case DT_LONG_UNSIGNED:
		{
			*fOut = (float32)srcData.wVal;
		}
			break;
        case DT_DOUBLE_LONG:
		{
			*fOut = (float32)srcData.nVal;
		}
			break;
        case DT_DOUBLE_LONG_UNSIGNED:
		{
			*fOut = (float32)srcData.dwVal;
		}
			break;
        case DT_LONG64:
		{
			*fOut = (float32)srcData.llVal;
		}
			break;
        case DT_LONG64_UNSIGNED:
		{
			*fOut = (float32)srcData.ullVal;
		}
			break;
		default:
            return -1;
			break;
    }

    return 1;
}

/*******************************************************************************
* ��������: fratio_get_by_sptype_zhj
* ��������: ���ݿ��� �Զ������ݿ�
* �������: ��
* �������: ��
* �� �� ֵ: ��
*******************************************************************************/
int fratio_get_by_sptype_zhj(uint32 spType, float32 *fOut)
{
    int ret = -1;
    
    switch(spType)
	{
		case YC_SP_PRE_DAY_MT_TIME:
        case YC_SP_PRE_DAY_QR_TIME:
        case YC_SP_PRE_DAY_UP_TIME:
        case YC_SP_PRE_DAY_DW_TIME:
        case YC_SP_PRE_DAY_PF_CAL:
            
        case YC_SP_PRE_MON_MT_TIME:
        case YC_SP_PRE_MON_QR_TIME:
        case YC_SP_PRE_MON_UP_TIME:
        case YC_SP_PRE_MON_DW_TIME:
        case YC_SP_PRE_MON_PF_CAL:
		{
			*fOut = 1;
            ret = 1;
		}
			break;
        case YC_SP_PRE_DAY_QT_RATE:
        case YC_SP_PRE_DAY_UP_RATE:
        case YC_SP_PRE_DAY_DW_RATE:

        case YC_SP_PRE_MON_QT_RATE:
        case YC_SP_PRE_MON_UP_RATE:
        case YC_SP_PRE_MON_DW_RATE:
		{
			*fOut = 0.01;
            ret = 1;
		}
			break;
		
		default:
            return -1;
			break;
    }

    return ret;
}


/*******************************************************************************
* ��������: sp_data_block_split_zhj
* ��������: ���ݿ��� �Զ������ݿ�
* �������: ��
* �������: ��
* �� �� ֵ: ��
*******************************************************************************/
int sp_data_block_split_zhj(uint32 nMainOAD, uint32 nOAD, uint32 nSpType, uint8 nItemID, uint8 *buff, float32 *fOut, uint8 *timeFlag, OOP_DATETIME_S_T *pTime)
{
	uint16 nNum = 0;

    if( (0x21220200 == nMainOAD) || (0x21230200 == nMainOAD) )//��ֵͳ�ƣ������������ֵͳ��˼·����ͳ�Ʋ���
    {
        switch(nOAD)
    	{
    		case 0x20000201://A���ѹ
            case 0x20000202://B���ѹ
            case 0x20000203://C���ѹ
            
            case 0x20010201://A�����
            case 0x20010202://B�����
            case 0x20010203://C�����
            case 0x20010600://�������

            case 0x200F0200://����Ƶ��
            case 0x20260200://��ѹ��ƽ����
            case 0x20270200://������ƽ����
            
            case 0x200D0201://A���ѹ�ܻ�����
            case 0x200D0301://B���ѹ�ܻ�����
            case 0x200D0401://C���ѹ�ܻ�����
            {
    			if((YC_SP_PRE_DAY_MAX == nSpType)||(YC_SP_PRE_MON_MAX == nSpType))//���ա��������ֵ/��ֵ
                {
                    OOP_EXTREMERES_T Extremeres;
                    memset(&Extremeres, 0, sizeof(OOP_EXTREMERES_T));
        			memcpy(&Extremeres, buff, sizeof(OOP_EXTREMERES_T));
                    fvalue_get_by_choice_zhj(Extremeres.dbMaxValue, fOut);
                    *timeFlag = 1;//��ʱ��
                    memcpy(pTime, &Extremeres.tmMax, sizeof(OOP_DATETIME_S_T));
                }
                else if((YC_SP_PRE_DAY_MIN == nSpType)||(YC_SP_PRE_MON_MIN == nSpType))//���ա�������Сֵ/��ֵ
                {
                    OOP_EXTREMERES_T Extremeres;
                    memset(&Extremeres, 0, sizeof(OOP_EXTREMERES_T));
        			memcpy(&Extremeres, buff, sizeof(OOP_EXTREMERES_T));
                    fvalue_get_by_choice_zhj(Extremeres.dbMinValue, fOut);
                    *timeFlag = 1;//��ʱ��
                    memcpy(pTime, &Extremeres.tmMin, sizeof(OOP_DATETIME_S_T));
                }
    		}
                break;
    		
    		default:
    			return -1;
                break;
    	}
    }
    else if( (0x21920200 == nMainOAD) || (0x21930200 == nMainOAD) )//�ϸ���ͳ��
    {
        switch(nOAD)
    	{
    		case 0x20000201://A���ѹ
            case 0x20000202://B���ѹ
            case 0x20000203://C���ѹ
            case 0x20260200://��ѹ��ƽ����
            case 0x20270200://������ƽ����
            
            case 0x200D0201://A���ѹ�ܻ�����
            case 0x200D0301://B���ѹ�ܻ�����
            case 0x200D0401://C���ѹ�ܻ�����
            {
    			MT_QR_RES_T mtQrRes;
    			memset(&mtQrRes, 0, sizeof(MT_QR_RES_T));
                memcpy(&mtQrRes, buff, sizeof(MT_QR_RES_T));
                if((YC_SP_PRE_DAY_MT_TIME == nSpType)||(YC_SP_PRE_MON_MT_TIME == nSpType))//���ա����¼��ʱ��
                {
                    *fOut = mtQrRes.mtTime;
                }
                else if((YC_SP_PRE_DAY_QR_TIME == nSpType)||(YC_SP_PRE_MON_QR_TIME == nSpType))//���ա����ºϸ�ʱ��
                {
                    *fOut = mtQrRes.qrTime;
                }
                else if((YC_SP_PRE_DAY_UP_TIME == nSpType)||(YC_SP_PRE_MON_UP_TIME == nSpType))//���ա����³�����ʱ��
                {
                    *fOut = mtQrRes.upTime;
                }
                else if((YC_SP_PRE_DAY_DW_TIME == nSpType)||(YC_SP_PRE_MON_DW_TIME == nSpType))//���ա����³�����ʱ��
                {
                    *fOut = mtQrRes.dwTime;
                }
                else if((YC_SP_PRE_DAY_QT_RATE == nSpType)||(YC_SP_PRE_MON_QT_RATE == nSpType))//���ա����ºϸ���
                {
                    *fOut = mtQrRes.qrRate;
                }
                else if((YC_SP_PRE_DAY_UP_RATE == nSpType)||(YC_SP_PRE_MON_UP_RATE == nSpType))//���ա����³�������
                {
                    *fOut = mtQrRes.upRate;
                }
                else if((YC_SP_PRE_DAY_DW_RATE == nSpType)||(YC_SP_PRE_MON_DW_RATE == nSpType))//���ա����³�������
                {
                    *fOut = mtQrRes.dwRate;
                }
                else if((YC_SP_PRE_DAY_MT_TOTAL == nSpType)||(YC_SP_PRE_MON_MT_TOTAL == nSpType))//���ա������ۼӺ�
                {
                    *fOut = mtQrRes.mtTotal;
                }
                else if((YC_SP_PRE_DAY_MT_AVG == nSpType)||(YC_SP_PRE_MON_MT_AVG == nSpType))//���ա�����ƽ��ֵ
                {
                    *fOut = mtQrRes.mtAvg;
                }
    		}
                break;

            case 0x200F0200://����Ƶ��
            {
    			MT_QR_RES_T mtQrRes;
    			memset(&mtQrRes, 0, sizeof(MT_QR_RES_T));
                memcpy(&mtQrRes, buff, sizeof(MT_QR_RES_T));
                if((YC_SP_PRE_DAY_MT_TIME == nSpType)||(YC_SP_PRE_MON_MT_TIME == nSpType))//���ա����¼��ʱ��
                {
                    *fOut = mtQrRes.mtTime;
                }
                else if((YC_SP_PRE_DAY_QR_TIME == nSpType)||(YC_SP_PRE_MON_QR_TIME == nSpType))//���ա����ºϸ�ʱ��
                {
                    *fOut = mtQrRes.qrTime;
                }
                else if((YC_SP_PRE_DAY_UP_TIME == nSpType)||(YC_SP_PRE_MON_UP_TIME == nSpType))//���ա����³�����ʱ��
                {
                    *fOut = mtQrRes.upTime + mtQrRes.dwTime;//���Ϊ Ƶ�ʳ���ʱ��=������ʱ��+������ʱ��
                }
                else if((YC_SP_PRE_DAY_DW_TIME == nSpType)||(YC_SP_PRE_MON_DW_TIME == nSpType))//���ա����³�����ʱ��
                {
                    *fOut = mtQrRes.dwTime;
                }
                else if((YC_SP_PRE_DAY_QT_RATE == nSpType)||(YC_SP_PRE_MON_QT_RATE == nSpType))//���ա����ºϸ���
                {
                    *fOut = mtQrRes.qrRate;
                }
                else if((YC_SP_PRE_DAY_UP_RATE == nSpType)||(YC_SP_PRE_MON_UP_RATE == nSpType))//���ա����³�������
                {
                    *fOut = mtQrRes.upRate + mtQrRes.dwRate;//���Ϊ Ƶ�ʳ�����=��������+��������
                }
                else if((YC_SP_PRE_DAY_DW_RATE == nSpType)||(YC_SP_PRE_MON_DW_RATE == nSpType))//���ա����³�������
                {
                    *fOut = mtQrRes.dwRate;
                }
                else if((YC_SP_PRE_DAY_MT_TOTAL == nSpType)||(YC_SP_PRE_MON_MT_TOTAL == nSpType))//���ա������ۼӺ�
                {
                    *fOut = mtQrRes.mtTotal;
                }
                else if((YC_SP_PRE_DAY_MT_AVG == nSpType)||(YC_SP_PRE_MON_MT_AVG == nSpType))//���ա�����ƽ��ֵ
                {
                    *fOut = mtQrRes.mtAvg;
                }
    		}
                break;

    		default:
    			return -1;
                break;
    	}
    }
	else if( (0x21020200 == nMainOAD) || (0x21030200 == nMainOAD) )
    {
        switch(nOAD)
    	{
    		case 0x200A0201://˲ʱ�ܹ�����������ͳ��
            {
    			OOP_REGION_T Region;
    			memset(&Region, 0, sizeof(OOP_REGION_T));
                memcpy(&Region, buff, sizeof(OOP_REGION_T));
    			nNum = Region.nNum;
    			if(nItemID >= nNum) 
    			{
    				return -1;
    			}
    			*fOut = (float32)Region.value[nItemID].dwTimes;
    		}
                break;

    		default:
    			return -1;
                break;
    	}
    }
    else if( (0x50060200 == nMainOAD) )
    {
        switch(nOAD)
    	{
    		case 0x10100200://�����й��������
    		case 0x10200200://�����й��������
            {
    			OOP_DEMAND_T Demand;
			    memset((uint8*)&Demand,0x00,sizeof(OOP_DEMAND_T));
                memcpy(&Demand, buff, sizeof(OOP_DEMAND_T));
    			nNum = Demand.nNum;
    			if(nItemID >= nNum) 
    			{
    				return -1;
    			}
    			*fOut = (float32)Demand.demand[nItemID].nValue;
                *timeFlag = 1;//��ʱ��
                 memcpy(pTime, &Demand.demand[nItemID].time, sizeof(OOP_DATETIME_S_T));
    		}
                break;

    		default:
    			return -1;
                break;
    	}
    }
    else
    {
        return -1;
    }

	return 1;
}


/*******************************************************************************
* ��������: string_data_block_split
* ��������: ���ݿ��� �ַ���
* �������: ��
* �������: ��
* �� �� ֵ: ��
*******************************************************************************/
int string_data_block_split_zhj(uint32 nOAD, uint8 nItemID, uint8 *buff, uint8 *pOut, uint8 *timeFlag, OOP_DATETIME_S_T *pTime)
{
	uint16 nNum = 0;

	switch(nOAD)
	{
		case 0x202F0200:                //���ܵ����� ����״̬��
		case 0x201F0201:                //ʣ����������� ��ǰʣ����������
		case 0x201F0202:                //ʣ����������� �������
		case 0x201F0203:                //ʣ����������� ����״̬��
		case 0x201F0204:                //ʣ����������� ������
		case 0x201F0205:                //ʣ����������� �ʣ���������ֵ������
        {
			OOP_OCTETVAR64_T octString = {0};
			memcpy(&octString, buff, sizeof(OOP_OCTETVAR64_T));
			nNum = octString.nNum;
            memcpy(pOut, octString.value, nNum);
		}
            break;
		
		default:
            return -1;
			break;
	}

	return nNum;
}


/*******************************************************************************
* ��������: yc_db_nomal_read
* ��������: ���������Ķ�ȡң��ʵʱ����(��ͨ������)
* �������: typeIndex         �豸���������±�, ��0��ʼ
            groupIndex    �豸��������±�, ��0��ʼ
            seqIndex      ����ƫ�Ƶ�ַ�����±�, ��0��ʼ
            index         �������
* �������: ��
* �� �� ֵ: ��
*******************************************************************************/
void yc_db_nomal_read_zhj(uint8 typeIndex, uint16 groupIndex, uint16 seqIndex, uint16 index)
{
    int m = typeIndex;//�豸����-1
    int x = groupIndex;//�豸��� ��0��ʼ
    int n=  seqIndex;//�豸���-1
    uint32 ArrIdx = 0;//һά��������(��ʼ)
    
	if(index >= g_nConfigYcNumZhj[m][x][n])
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

    uint8 nTimeFlag = 0;//�Ƿ��ʱ��
    OOP_DATETIME_S_T stTime = {0};//��������ʱ��

    ArrIdx = g_aYcDataIndexZhj[m][x][n];

	//���ݶ�ȡ
	NomalOad.logicId = g_aYcDataConfigZhj[ArrIdx+index].nLogicId;  //�߼���ַ
	NomalOad.infoNum = g_aYcDataConfigZhj[ArrIdx+index].nInfoNum;  //��Ϣ���
	NomalOad.oad.value = g_aYcDataConfigZhj[ArrIdx+index].nDbCenterOAD;  //��ͨOAD
	NomalOad.classtag = CLASS_DATA_NORMAL;

	ret = db_read_nomal(g_sDataCenterClient, &NomalOad, sizeof(aRecvBuff), (uint8*)aRecvBuff, &nRevcBuffLen);
	DB104_FMT_TRACE("m=%d, x=%d, n=%d, <--��%d��ң��--> addr=%08x, yc_db_nomal_read() ret = db_read_nomal() ret:%d \n", 
        m, x, n, index, g_aYcDataConfigZhj[ArrIdx+index].nObjAddrU32, ret);
	
	if(ret == 0)
	{
		DB104_FMT_TRACE("yc_db_nomal_read() ��ȡ���ݳɹ�  g_aYcDataConfig[%d].nDbCenterOAD=%08x \n", 
			index, g_aYcDataConfigZhj[ArrIdx+index].nDbCenterOAD);
		DB104_BUF_TRACE(aRecvBuff, nRevcBuffLen, "����: ");

		//��ȡ�����������ݿ飬��Ҫ���з��ദ��
		ret = data_block_split_zhj(g_aYcDataConfigZhj[ArrIdx+index].nDbCenterOAD, g_aYcDataConfigZhj[ArrIdx+index].nItemIndex, aRecvBuff, &fValueDB, &nTimeFlag, &stTime);
		if(ret > 0)
		{
		    g_aYcDataSendZhj[ArrIdx+index].bNewValidFlag = TRUE;
            
			//��ȡ����
			pMap645ToOop = oad_info_find(g_aYcDataConfigZhj[ArrIdx+index].nDbCenterOAD);
			if(pMap645ToOop == NULL)
			{
				DB104_FMT_TRACE("yc_db_nomal_read() if(pMap645ToOop == NULL) \n");
			}
			else
			{
				fRatio = ratio_convert(pMap645ToOop->DesScaler, E_OOP_TO_REAL);
				DB104_FMT_TRACE("��ȡ���� fRatio=%f \n", fRatio);
			}
			g_aYcDataSendZhj[ArrIdx+index].fNewValue = fValueDB*fRatio;
			DB104_FMT_TRACE("���ʴ���֮��g_aYcDataSend[index].fNewValue=%f, ���ʴ���֮ǰfValueDB=%f \n", g_aYcDataSendZhj[ArrIdx+index].fNewValue, fValueDB);
			
			//��ȡ�仯����ֵ
			fDeadZone = g_aYcDataConfigZhj[ArrIdx+index].fDeadZoneValue;
			DB104_FMT_TRACE("�仯����ֵfDeadZone=%f \n", fDeadZone);

			//��ʱ��ֵ, ���Ա仯����
			if(g_bTestEnable)
			{
				g_aYcDataSendZhj[ArrIdx+index].fHisValue = rand()%100;//100���������
				DB104_FMT_TRACE("ң������� g_aYcDataSend[%d].fHisValue=%f \n", index, g_aYcDataSendZhj[ArrIdx+index].fHisValue);
			}

			//�ж�ʵʱֵ�Ƿ�Խ������
			fDiffValue = fabs(g_aYcDataSendZhj[ArrIdx+index].fNewValue - g_aYcDataSendZhj[ArrIdx+index].fHisValue);
			if(fDiffValue > fDeadZone)
			{
				DB104_FMT_TRACE("�仯ֵfDiffValue=%f, ��������ֵ, fNewValue=%f, fHisValue=%f ��Ҫ�����ϱ�! \n", 
					fDiffValue, g_aYcDataSendZhj[ArrIdx+index].fNewValue, g_aYcDataSendZhj[ArrIdx+index].fHisValue);
				g_bYcChanged = TRUE;
				g_aYcDataSendZhj[ArrIdx+index].bChangeFlag = TRUE;
				g_aYcDataSendZhj[ArrIdx+index].bSendFlag = FALSE;
                if(M_ME_NC_1 == g_aYcDataConfigZhj[ArrIdx+index].nSignalType)
                {
                    g_aChangedEquipStateYC_R[m][x][n] = 1;
                }
                else if(M_ME_TF_1 == g_aYcDataConfigZhj[ArrIdx+index].nSignalType)
                {
                    g_aChangedEquipStateYC_R_T[m][x][n] = 1;
                }
                else if(M_IT_NB_1 == g_aYcDataConfigZhj[ArrIdx+index].nSignalType)
                {
                    g_aChangedEquipStateYC_R_EN[m][x][n] = 1;
                }
			}
			else
			{
				DB104_FMT_TRACE("�仯ֵfDiffValue=%f, С������ֵ, fNewValue=%f, fHisValue=%f \n", 
					fDiffValue, g_aYcDataSendZhj[ArrIdx+index].fNewValue, g_aYcDataSendZhj[ArrIdx+index].fHisValue);
			}
			
		}
		else
		{
		    g_aYcDataSendZhj[ArrIdx+index].bNewValidFlag = FALSE;
			DB104_FMT_TRACE("yc_db_nomal_read() ��������ʧ��  g_aYcDataConfig[%d].nDbCenterOAD=%08x \n", 
				index, g_aYcDataConfigZhj[ArrIdx+index].nDbCenterOAD);
		}
	}
	else
	{
	    g_aYcDataSendZhj[ArrIdx+index].bNewValidFlag = FALSE;
		DB104_FMT_TRACE("yc_db_nomal_read() ��ȡ����  g_aYcDataConfig[%d].nDbCenterOAD=%08x \n", 
			index, g_aYcDataConfigZhj[ArrIdx+index].nDbCenterOAD);
		DB104_FMT_TRACE("yc_db_nomal_read() ���ݶ�ȡʧ��! \n");
	}

    if(nTimeFlag > 0)
    {
        DB104_FMT_TRACE("yc_db_nomal_read() ���ݴ�ʱ��nTimeFlag=%d \n", nTimeFlag);
        ooptm_to_cp56time2a(stTime, &g_aYcDataSendZhj[ArrIdx+index].btime[0]);
    }
	else
    {
        system_time_get(&g_aYcDataSendZhj[ArrIdx+index].btime[0]);
    }
}

/*******************************************************************************
* ��������: yc_db_record_read
* ��������: ���������Ķ�ȡң��ʵʱ����(��¼������)
* �������: typeIndex         �豸���������±�, ��0��ʼ
            groupIndex    �豸��������±�, ��0��ʼ
            seqIndex      ����ƫ�Ƶ�ַ�����±�, ��0��ʼ
            index         �������
* �������: ��
* �� �� ֵ: ��
*******************************************************************************/
void yc_db_record_read_zhj(uint8 typeIndex, uint16 groupIndex, uint16 seqIndex, uint16 index)
{
    int m = typeIndex;//�豸����-1
    int x = groupIndex;//�豸��� ��0��ʼ
    int n=  seqIndex;//�豸���-1
    int i = 0;//��ֵ�±�
    uint16 areaID = g_nFixedAreaIdZhj[m][x][n];//��ֵ����
    uint32 macAddr = 0;//�����������豸��ַ�ĵ��
	uint32 ArrIdx = 0;//һά��������(��ʼ)
	uint32 ArrParaIdx = 0;//һά��������(������ʼ)
    
	if(index >= g_nConfigYcNumZhj[m][x][n])
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

    uint8 nTimeFlag = 0;//�Ƿ��ʱ��
    OOP_DATETIME_S_T stTime = {0};//��������ʱ��
	
	ArrIdx = g_aYcDataIndexZhj[m][x][n];

	//���ݶ�ȡ
	nRealTime_t = time(NULL);
	
	ReadRecord.recordOAD.logicId = g_aYcDataConfigZhj[ArrIdx+index].nLogicId;  //�߼���ַ
	ReadRecord.recordOAD.infoNum = g_aYcDataConfigZhj[ArrIdx+index].nInfoNum;  //��Ϣ���
	ReadRecord.recordOAD.road.oadMain.value = g_aYcDataConfigZhj[ArrIdx+index].nDbCenterOAD;  //��OAD
	ReadRecord.recordOAD.road.oadCols.nNum = g_aYcDataConfigZhj[ArrIdx+index].nOadColsNum;  //��OAD����
	memcpy(&ReadRecord.recordOAD.road.oadCols.oad[0], &g_aYcDataConfigZhj[ArrIdx+index].aOadCols[0],
		g_aYcDataConfigZhj[ArrIdx+index].nOadColsNum*sizeof(uint32));  //������OAD
	ReadRecord.recordOAD.classtag = CLASS_DATA_COLLEC;

	ReadRecord.cType = COL_TM_STORE;  //�ɼ��洢ʱ��
	//ReadRecord.cStart = nRealTime_t + 8*60*60-24*3600;  //ʱ����ʼ�㣬��ǰʱ����ǰһ��
	//ReadRecord.cStart = 0;  //ʱ����ʼ��
	ReadRecord.cStart = nRealTime_t + 8*60*60 - 10*60;  //ʱ����ʼ��, ��ǰʱ����ǰ10����
	ReadRecord.cEnd = nRealTime_t + 8*60*60;  //ʱ�������
	ReadRecord.sortType = DOWN_SORT;  //����ʽ ����
	ReadRecord.MAC.nNum = 6;  //����У�� ����
	if(E_EQUIP_CAPACITOR == (typeIndex + 1))
    {
        macAddr = 0x00008003;
    }
    else if(E_EQUIP_RESIDUALCURRENT == (typeIndex + 1))
    {
        macAddr = 0x00008003;
    }

    ArrParaIdx = g_aParaValueIndexZhj[m][x][n];
    for(i=0; i<g_nConfigParaNumZhj[m][x][n]; i++)
    {
        if(g_aParaSetValueZhj[areaID][ArrParaIdx+i].nObjAddrU32 == macAddr)
        {
            memcpy(ReadRecord.MAC.value, g_aParaSetValueZhj[areaID][ArrParaIdx+i].aArrValue, ReadRecord.MAC.nNum);
            break;
        }
    }
	
	ReadRecord.ti.unit = TI_HOUR;  //ʱ���� ��λ
	ReadRecord.ti.value = 15;  //ʱ���� ֵ

	DB104_FMT_TRACE("m=%d, x=%d, n=%d <--��%d��ң��--> addr=%08x, deal=%08x, yc_db_record_read() �����±�index=%d, �߼���ַlogicId=%d, ��Ϣ���infoNum=%d, ��OAD=%08x, ��OAD����=%d, ��OAD=%08x, У��MAC.nNum=%d, MAC.value=%02x%02x%02x%02x%02x%02x \n", 
		m, x, n, index, g_aYcDataConfigZhj[ArrIdx+index].nObjAddrU32, g_aYcDataConfigZhj[ArrIdx+index].nDealType, index, ReadRecord.recordOAD.logicId, ReadRecord.recordOAD.infoNum, ReadRecord.recordOAD.road.oadMain.value, ReadRecord.recordOAD.road.oadCols.nNum, 
		ReadRecord.recordOAD.road.oadCols.oad[0].value, ReadRecord.MAC.nNum, ReadRecord.MAC.value[5], ReadRecord.MAC.value[4], ReadRecord.MAC.value[3], ReadRecord.MAC.value[2], ReadRecord.MAC.value[1], ReadRecord.MAC.value[0]);
	
	ret = db_read_record(g_sDataCenterClient, &ReadRecord, sizeof(aRecvBuff), &RecordOut);
	DB104_FMT_TRACE("yc_db_center_read() ret = db_read_record() ret:%d \n", ret);
	if(ret == 0)
	{
		nRevcBuffLen = RecordOut.record[0].outDatalen;
		memcpy(&aRecvBuff[0], &RecordOut.record[0].outDataBuf[0], nRevcBuffLen);

		DB104_FMT_TRACE("yc_db_record_read() ��ȡ���ݳɹ�  g_aYcDataConfig[%d].nDbCenterOAD=%08x \n", 
			index, g_aYcDataConfigZhj[ArrIdx+index].nDbCenterOAD);
		DB104_BUF_TRACE(aRecvBuff, nRevcBuffLen, "����: ");

		//��ȡ�����������ݿ飬��Ҫ���з��ദ��
		ret = data_block_split_zhj(g_aYcDataConfigZhj[ArrIdx+index].aOadCols[0], g_aYcDataConfigZhj[ArrIdx+index].nItemIndex, aRecvBuff, &fValueDB, &nTimeFlag, &stTime);
		if(ret > 0)
		{
		    g_aYcDataSendZhj[ArrIdx+index].bNewValidFlag = TRUE;
            
			//��ȡ����
			pMap645ToOop = oad_info_find(g_aYcDataConfigZhj[ArrIdx+index].aOadCols[0]);
			if(pMap645ToOop == NULL)
			{
				DB104_FMT_TRACE("yc_db_record_read() if(pMap645ToOop == NULL) \n");
			}
			else
			{
				fRatio = ratio_convert(pMap645ToOop->DesScaler, E_OOP_TO_REAL);
				DB104_FMT_TRACE("��ȡ���� fRatio=%f \n", fRatio);
			}
			g_aYcDataSendZhj[ArrIdx+index].fNewValue = fValueDB*fRatio;
			DB104_FMT_TRACE("���ʴ���֮��g_aYcDataSend[index].fNewValue=%f, ���ʴ���֮ǰfValueDB=%f \n", g_aYcDataSendZhj[ArrIdx+index].fNewValue, fValueDB);
			
			//��ȡ�仯����ֵ
			fDeadZone = g_aYcDataConfigZhj[ArrIdx+index].fDeadZoneValue;
			DB104_FMT_TRACE("�仯����ֵfDeadZone=%f \n", fDeadZone);

			//��ʱ��ֵ, ���Ա仯����
			if(g_bTestEnable)
			{
				g_aYcDataSendZhj[ArrIdx+index].fHisValue = rand()%100;//100���������
				DB104_FMT_TRACE("ң������� g_aYcDataSend[%d].fHisValue=%f \n", index, g_aYcDataSendZhj[ArrIdx+index].fHisValue);
			}

			//�ж�ʵʱֵ�Ƿ�Խ������
			fDiffValue = fabs(g_aYcDataSendZhj[ArrIdx+index].fNewValue - g_aYcDataSendZhj[ArrIdx+index].fHisValue);
			if(fDiffValue > fDeadZone)
			{
				DB104_FMT_TRACE("�仯ֵfDiffValue=%f, ��������ֵ, fNewValue=%f, fHisValue=%f ��Ҫ�����ϱ�! \n", 
					fDiffValue, g_aYcDataSendZhj[ArrIdx+index].fNewValue, g_aYcDataSendZhj[ArrIdx+index].fHisValue);
				g_bYcChanged = TRUE;
				g_aYcDataSendZhj[ArrIdx+index].bChangeFlag = TRUE;
				g_aYcDataSendZhj[ArrIdx+index].bSendFlag = FALSE;
                g_aChangedEquipStateYC_R[m][x][n] = 1;
			}
			else
			{
				DB104_FMT_TRACE("�仯ֵfDiffValue=%f, С������ֵ, fNewValue=%f, fHisValue=%f \n", 
					fDiffValue, g_aYcDataSendZhj[ArrIdx+index].fNewValue, g_aYcDataSendZhj[ArrIdx+index].fHisValue);
			}
			
		}
		else
		{
		    g_aYcDataSendZhj[ArrIdx+index].bNewValidFlag = FALSE;
			DB104_FMT_TRACE("yc_db_record_read() ��������ʧ��  g_aYcDataConfig[%d].nDbCenterOAD=%08x \n", 
				index, g_aYcDataConfigZhj[ArrIdx+index].nDbCenterOAD);
		}
	}
	else
	{
	    g_aYcDataSendZhj[ArrIdx+index].bNewValidFlag = FALSE;
		DB104_FMT_TRACE("yc_db_record_read() ��ȡ����  g_aYcDataConfig[%d].nDbCenterOAD=%08x \n", 
			index, g_aYcDataConfigZhj[ArrIdx+index].nDbCenterOAD);
		DB104_FMT_TRACE("yc_db_record_read() ���ݶ�ȡʧ��! \n");

        //��ʱ���⴦��
        /*
        if( ((E_EQUIP_CAPACITOR-1)==m && 0x00200001==g_aYcDataConfigZhj[ArrIdx+index].nObjAddrU32) 
            || ((E_EQUIP_CAPACITOR-1)==m && 0x00200002==g_aYcDataConfigZhj[ArrIdx+index].nObjAddrU32))
        {
            g_aYcDataSendZhj[ArrIdx+index].bNewValidFlag = TRUE;
            DB104_FMT_TRACE("���⴦�� m=%d, addr=%08x \n", m, g_aYcDataConfigZhj[ArrIdx+index].nObjAddrU32);
        }
        */
	}
	
	if(nTimeFlag > 0)
    {
        DB104_FMT_TRACE("yc_db_record_read() ���ݴ�ʱ��nTimeFlag=%d \n", nTimeFlag);
        ooptm_to_cp56time2a(stTime, &g_aYcDataSendZhj[ArrIdx+index].btime[0]);
    }
	else
    {
        system_time_get(&g_aYcDataSendZhj[ArrIdx+index].btime[0]);
    }
}

/*******************************************************************************
* ��������: yc_db_sp_record_read_zhj
* ��������: ���������Ķ�ȡң��ʵʱ����(���� ��¼������)
* �������: typeIndex         �豸���������±�, ��0��ʼ
            groupIndex    �豸��������±�, ��0��ʼ
            seqIndex      ����ƫ�Ƶ�ַ�����±�, ��0��ʼ
            index         �������
* �������: ��
* �� �� ֵ: ��
*******************************************************************************/
void yc_db_sp_record_read_zhj(uint8 typeIndex, uint16 groupIndex, uint16 seqIndex, uint16 index)
{
    int m = typeIndex;//�豸����-1
    int x = groupIndex;//�豸��� ��0��ʼ
    int n=  seqIndex;//�豸���-1
    int i = 0;//��ֵ�±�
    uint16 areaID = g_nFixedAreaIdZhj[m][x][n];//��ֵ����
    uint32 macAddr = 0;//�����������豸��ַ�ĵ��
	uint32 ArrIdx = 0;//һά��������(��ʼ)
	uint32 ArrParaIdx = 0;//һά��������(������ʼ)
    
	if(index >= g_nConfigYcNumZhj[m][x][n])
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

    uint8 nTimeFlag = 0;//�Ƿ��ʱ��
    OOP_DATETIME_S_T stTime = {0};//��������ʱ��
	
	ArrIdx = g_aYcDataIndexZhj[m][x][n];

	//���ݶ�ȡ
	nRealTime_t = time(NULL);
	
	ReadRecord.recordOAD.logicId = g_aYcDataConfigZhj[ArrIdx+index].nLogicId;  //�߼���ַ
	ReadRecord.recordOAD.infoNum = g_aYcDataConfigZhj[ArrIdx+index].nInfoNum;  //��Ϣ���
	ReadRecord.recordOAD.road.oadMain.value = g_aYcDataConfigZhj[ArrIdx+index].nDbCenterOAD;  //��OAD
	ReadRecord.recordOAD.road.oadCols.nNum = g_aYcDataConfigZhj[ArrIdx+index].nOadColsNum;  //��OAD����
	memcpy(&ReadRecord.recordOAD.road.oadCols.oad[0], &g_aYcDataConfigZhj[ArrIdx+index].aOadCols[0],
		g_aYcDataConfigZhj[ArrIdx+index].nOadColsNum*sizeof(uint32));  //������OAD
	ReadRecord.recordOAD.classtag = CLASS_DATA_COLLEC;

	ReadRecord.cType = COL_TM_STORE;  //�ɼ��洢ʱ��
	//ReadRecord.cStart = nRealTime_t-24*3600;  //ʱ����ʼ�㣬��ǰʱ����ǰһ��
	ReadRecord.cStart = 0;  //ʱ����ʼ�㣬��ǰʱ����ǰһ��

#if TEST_VER_FLAG == 1
    //���԰汾ֱ�Ӳ��õ�ǰʱ��, ��ȡʵʱͳ��ֵ
	ReadRecord.cEnd = nRealTime_t + 8*60*60;  //ʱ�������
#else
    if( (YC_SP_PRE_DAY_START < g_aYcDataConfigZhj[ArrIdx+index].nDealType) 
                                    && (YC_SP_PRE_DAY_END > g_aYcDataConfigZhj[ArrIdx+index].nDealType) )
    {
        ReadRecord.cEnd = nPreDayTime_t + 8*60*60;  //ʱ�������
    }
    else if( (YC_SP_PRE_MON_START < g_aYcDataConfigZhj[ArrIdx+index].nDealType) 
                && (YC_SP_PRE_MON_END > g_aYcDataConfigZhj[ArrIdx+index].nDealType) )
    {
        ReadRecord.cEnd = nPreMonTime_t + 8*60*60;  //ʱ�������
    }
    else
    {
        ReadRecord.cEnd = nRealTime_t + 8*60*60;  //ʱ�������
    }
#endif

	ReadRecord.sortType = DOWN_SORT;  //����ʽ ����
	ReadRecord.MAC.nNum = 6;  //����У�� ����
	if(E_EQUIP_TRANSFORMER == (typeIndex + 1))
    {
        macAddr = 0x00008003;
    }
    else if(E_EQUIP_CAPACITOR == (typeIndex + 1))
    {
        macAddr = 0x00008003;
    }
    else if(E_EQUIP_RESIDUALCURRENT == (typeIndex + 1))
    {
        macAddr = 0x00008003;
    }

    ArrParaIdx = g_aParaValueIndexZhj[m][x][n];
    for(i=0; i<g_nConfigParaNumZhj[m][x][n]; i++)
    {
        if(g_aParaSetValueZhj[areaID][ArrParaIdx+i].nObjAddrU32 == macAddr)
        {
            memcpy(ReadRecord.MAC.value, g_aParaSetValueZhj[areaID][ArrParaIdx+i].aArrValue, ReadRecord.MAC.nNum);
            break;
        }
    }

    //�߼�ͳ�ƹ��ܲ�����macУ��!!!!!!
    memset(ReadRecord.MAC.value, 0, ReadRecord.MAC.nNum);
	
	ReadRecord.ti.unit = TI_HOUR;  //ʱ���� ��λ
	ReadRecord.ti.value = 15;  //ʱ���� ֵ

	DB104_FMT_TRACE("m=%d, x=%d, n=%d <--��%d��ң��--> addr=%08x, deal=%08x, yc_db_record_read() �����±�index=%d, �߼���ַlogicId=%d, ��Ϣ���infoNum=%d, ��OAD=%08x, ��OAD����=%d, ��OAD=%08x, У��MAC.nNum=%d, MAC.value=%02x%02x%02x%02x%02x%02x \n", 
		m, x, n, index, g_aYcDataConfigZhj[ArrIdx+index].nObjAddrU32, g_aYcDataConfigZhj[ArrIdx+index].nDealType, index, ReadRecord.recordOAD.logicId, ReadRecord.recordOAD.infoNum, ReadRecord.recordOAD.road.oadMain.value, ReadRecord.recordOAD.road.oadCols.nNum, 
		ReadRecord.recordOAD.road.oadCols.oad[0].value, ReadRecord.MAC.nNum, ReadRecord.MAC.value[5], ReadRecord.MAC.value[4], ReadRecord.MAC.value[3], ReadRecord.MAC.value[2], ReadRecord.MAC.value[1], ReadRecord.MAC.value[0]);
	
	ret = db_read_record(g_sDataCenterClient, &ReadRecord, sizeof(aRecvBuff), &RecordOut);
	DB104_FMT_TRACE("yc_db_center_read() ret = db_read_record()  ret:%d \n", ret);
	if(ret == 0)
	{
		nRevcBuffLen = RecordOut.record[0].outDatalen;
		memcpy(&aRecvBuff[0], &RecordOut.record[0].outDataBuf[0], nRevcBuffLen);

		DB104_FMT_TRACE("yc_db_record_read() ��ȡ���ݳɹ� g_aYcDataConfig[%d].nDbCenterOAD=%08x \n", 
			index, g_aYcDataConfigZhj[ArrIdx+index].nDbCenterOAD);
		DB104_BUF_TRACE(aRecvBuff, nRevcBuffLen, "����: ");

		//��ȡ�����������ݿ飬��Ҫ���з��ദ��
		ret = sp_data_block_split_zhj(g_aYcDataConfigZhj[ArrIdx+index].nDbCenterOAD, g_aYcDataConfigZhj[ArrIdx+index].aOadCols[0], g_aYcDataConfigZhj[ArrIdx+index].nDealType, 
		    g_aYcDataConfigZhj[ArrIdx+index].nItemIndex, aRecvBuff, &fValueDB, &nTimeFlag, &stTime);
		if(ret > 0)
		{
		    g_aYcDataSendZhj[ArrIdx+index].bNewValidFlag = TRUE;
            
			//��ȡ����
			ret = fratio_get_by_sptype_zhj(g_aYcDataConfigZhj[ArrIdx+index].nDealType, &fRatio);
            if(ret < 0)
            {
                pMap645ToOop = oad_info_find(g_aYcDataConfigZhj[ArrIdx+index].aOadCols[0]);
    			if(pMap645ToOop == NULL)
    			{
    				DB104_FMT_TRACE("oad_info_find() if(pMap645ToOop == NULL) \n");
    			}
    			else
    			{
    				fRatio = ratio_convert(pMap645ToOop->DesScaler, E_OOP_TO_REAL);
    				DB104_FMT_TRACE("ratio_convert() ��ȡ���� fRatio=%f \n", fRatio);
    			}
            }
			
			g_aYcDataSendZhj[ArrIdx+index].fNewValue = fValueDB*fRatio;
			DB104_FMT_TRACE("���ʴ���֮��g_aYcDataSend[index].fNewValue=%f, ���ʴ���֮ǰfValueDB=%f, ����fRatio=%f \n", g_aYcDataSendZhj[ArrIdx+index].fNewValue, fValueDB, fRatio);
			
			//��ȡ�仯����ֵ
			fDeadZone = g_aYcDataConfigZhj[ArrIdx+index].fDeadZoneValue;
			DB104_FMT_TRACE("�仯����ֵfDeadZone=%f \n", fDeadZone);

			//��ʱ��ֵ, ���Ա仯����
			if(g_bTestEnable)
			{
				g_aYcDataSendZhj[ArrIdx+index].fHisValue = rand()%100;//100���������
				DB104_FMT_TRACE("ң������� g_aYcDataSend[%d].fHisValue=%f \n", index, g_aYcDataSendZhj[ArrIdx+index].fHisValue);
			}

			//�ж�ʵʱֵ�Ƿ�Խ������
			fDiffValue = fabs(g_aYcDataSendZhj[ArrIdx+index].fNewValue - g_aYcDataSendZhj[ArrIdx+index].fHisValue);
			if(fDiffValue > fDeadZone)
			{
				DB104_FMT_TRACE("�仯ֵfDiffValue=%f, ��������ֵ, fNewValue=%f, fHisValue=%f ��Ҫ�����ϱ�! \n", 
					fDiffValue, g_aYcDataSendZhj[ArrIdx+index].fNewValue, g_aYcDataSendZhj[ArrIdx+index].fHisValue);
				g_bYcChanged = TRUE;
				g_aYcDataSendZhj[ArrIdx+index].bChangeFlag = TRUE;
				g_aYcDataSendZhj[ArrIdx+index].bSendFlag = FALSE;
                g_aChangedEquipStateYC_R[m][x][n] = 1;
			}
			else
			{
				DB104_FMT_TRACE("�仯ֵfDiffValue=%f, С������ֵ, fNewValue=%f, fHisValue=%f \n", 
					fDiffValue, g_aYcDataSendZhj[ArrIdx+index].fNewValue, g_aYcDataSendZhj[ArrIdx+index].fHisValue);
			}
			
		}
		else
		{
		    g_aYcDataSendZhj[ArrIdx+index].bNewValidFlag = FALSE;
			DB104_FMT_TRACE("yc_db_record_read() ��������ʧ��  g_aYcDataConfig[%d].nDbCenterOAD=%08x \n", 
				index, g_aYcDataConfigZhj[ArrIdx+index].nDbCenterOAD);
		}
	}
	else
	{
	    g_aYcDataSendZhj[ArrIdx+index].bNewValidFlag = FALSE;
		DB104_FMT_TRACE("yc_db_record_read() ��ȡ����  g_aYcDataConfig[%d].nDbCenterOAD=%08x \n", 
			index, g_aYcDataConfigZhj[ArrIdx+index].nDbCenterOAD);
		DB104_FMT_TRACE("yc_db_record_read() ���ݶ�ȡʧ��! \n");
	}
	
	if(nTimeFlag > 0)
    {
        DB104_FMT_TRACE("yc_db_record_read() ���ݴ�ʱ��nTimeFlag=%d \n", nTimeFlag);
        ooptm_to_cp56time2a(stTime, &g_aYcDataSendZhj[ArrIdx+index].btime[0]);
    }
	else
    {
        system_time_get(&g_aYcDataSendZhj[ArrIdx+index].btime[0]);
    }
}


/*******************************************************************************
* ��������: yc_db_record_str_read_zhj
* ��������: ���������Ķ�ȡң��ʵʱ����(��¼������)
* �������: typeIndex         �豸���������±�, ��0��ʼ
            groupIndex    �豸��������±�, ��0��ʼ
            seqIndex      ����ƫ�Ƶ�ַ�����±�, ��0��ʼ
            index         �������
* �������: ��
* �� �� ֵ: ��
*******************************************************************************/
void yc_db_record_str_read_zhj(uint8 typeIndex, uint16 groupIndex, uint16 seqIndex, uint16 index)
{
    int m = typeIndex;//�豸����-1
    int x = groupIndex;//�豸��� ��0��ʼ
    int n=  seqIndex;//�豸���-1
    int i = 0;//��ֵ�±�
    uint16 areaID = g_nFixedAreaIdZhj[m][x][n];//��ֵ����
    uint32 macAddr = 0;//�����������豸��ַ�ĵ��
    uint8  octArr[64] = {0};//�ַ�����
	uint32 ArrIdx = 0;//һά��������(��ʼ)
	uint32 ArrParaIdx = 0;//һά��������(������ʼ)
    
	if(index >= g_nConfigYcNumZhj[m][x][n])
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

    uint8 nTimeFlag = 0;//�Ƿ��ʱ��
    OOP_DATETIME_S_T stTime = {0};//��������ʱ��
	
	ArrIdx = g_aYcDataIndexZhj[m][x][n];
	
	time_t nRealTime_t;  //��ǰϵͳʱ���ת��������

	//���ݶ�ȡ
	nRealTime_t = time(NULL);
	
	ReadRecord.recordOAD.logicId = g_aYcDataConfigZhj[ArrIdx+index].nLogicId;  //�߼���ַ
	ReadRecord.recordOAD.infoNum = g_aYcDataConfigZhj[ArrIdx+index].nInfoNum;  //��Ϣ���
	ReadRecord.recordOAD.road.oadMain.value = g_aYcDataConfigZhj[ArrIdx+index].nDbCenterOAD;  //��OAD
	ReadRecord.recordOAD.road.oadCols.nNum = g_aYcDataConfigZhj[ArrIdx+index].nOadColsNum;  //��OAD����
	memcpy(&ReadRecord.recordOAD.road.oadCols.oad[0], &g_aYcDataConfigZhj[ArrIdx+index].aOadCols[0],
		g_aYcDataConfigZhj[ArrIdx+index].nOadColsNum*sizeof(uint32));  //������OAD
	ReadRecord.recordOAD.classtag = CLASS_DATA_COLLEC;

	ReadRecord.cType = COL_TM_STORE;  //�ɼ��洢ʱ��
	//ReadRecord.cStart = nRealTime_t-24*3600;  //ʱ����ʼ�㣬��ǰʱ����ǰһ��
	ReadRecord.cStart = 0;  //ʱ����ʼ�㣬��ǰʱ����ǰһ��
	ReadRecord.cEnd = nRealTime_t + 8*60*60;  //ʱ�������
	ReadRecord.sortType = DOWN_SORT;  //����ʽ ����
	ReadRecord.MAC.nNum = 6;  //����У�� ����
	if(E_EQUIP_CAPACITOR == (typeIndex + 1))
    {
        macAddr = 0x00008003;
    }
    else if(E_EQUIP_RESIDUALCURRENT == (typeIndex + 1))
    {
        macAddr = 0x00008003;
    }

    ArrParaIdx = g_aParaValueIndexZhj[m][x][n];
    for(i=0; i<g_nConfigParaNumZhj[m][x][n]; i++)
    {
        if(g_aParaSetValueZhj[areaID][ArrParaIdx+i].nObjAddrU32 == macAddr)
        {
            memcpy(ReadRecord.MAC.value, g_aParaSetValueZhj[areaID][ArrParaIdx+i].aArrValue, ReadRecord.MAC.nNum);
            break;
        }
    }
	
	ReadRecord.ti.unit = TI_HOUR;  //ʱ���� ��λ
	ReadRecord.ti.value = 15;  //ʱ���� ֵ

	DB104_FMT_TRACE("m=%d, x=%d, n=%d <--��%d��ң��--> addr=%08x, yc_db_record_read() �����±�index=%d, �߼���ַlogicId=%d, ��Ϣ���infoNum=%d, ��OAD=%08x, ��OAD����=%d, ��OAD=%08x, У��MAC.nNum=%d, MAC.value=%02x%02x%02x%02x%02x%02x \n", 
		m, x, n, index, g_aYcDataConfigZhj[ArrIdx+index].nObjAddrU32, index, ReadRecord.recordOAD.logicId, ReadRecord.recordOAD.infoNum, ReadRecord.recordOAD.road.oadMain.value, ReadRecord.recordOAD.road.oadCols.nNum, 
		ReadRecord.recordOAD.road.oadCols.oad[0].value, ReadRecord.MAC.nNum, ReadRecord.MAC.value[5], ReadRecord.MAC.value[4], ReadRecord.MAC.value[3], ReadRecord.MAC.value[2], ReadRecord.MAC.value[1], ReadRecord.MAC.value[0]);
	
	ret = db_read_record(g_sDataCenterClient, &ReadRecord, sizeof(aRecvBuff), &RecordOut);
	DB104_FMT_TRACE("yc_db_record_str_read_zhj() ret = db_read_record() ret:%d \n", ret);
	if(ret == 0)
	{
		nRevcBuffLen = RecordOut.record[0].outDatalen;
		memcpy(&aRecvBuff[0], &RecordOut.record[0].outDataBuf[0], nRevcBuffLen);

		DB104_FMT_TRACE("yc_db_record_str_read_zhj() ��ȡ���ݳɹ�  g_aYcDataConfig[%d].nDbCenterOAD=%08x \n", 
			index, g_aYcDataConfigZhj[ArrIdx+index].nDbCenterOAD);
		DB104_BUF_TRACE(aRecvBuff, nRevcBuffLen, "����: ");

		//��ȡ�����������ݿ飬��Ҫ���з��ദ��
		ret = string_data_block_split_zhj(g_aYcDataConfigZhj[ArrIdx+index].aOadCols[0], g_aYcDataConfigZhj[ArrIdx+index].nItemIndex, aRecvBuff, octArr, &nTimeFlag, &stTime);
		if(ret > 0)
		{
		    g_aYcDataSendZhj[ArrIdx+index].bNewValidFlag = TRUE;
            
		    if(ret > MAX_YC_STRING_LEN)
            {
                ret = MAX_YC_STRING_LEN;
            }
		    g_aYcDataSendZhj[ArrIdx+index].nNewArrValueNum = ret;
            memcpy(g_aYcDataSendZhj[ArrIdx+index].aNewArrValue, octArr, ret);
            DB104_BUF_TRACE(g_aYcDataSendZhj[ArrIdx+index].aNewArrValue, ret, "octString����: ");
		}
        else
		{
		    g_aYcDataSendZhj[ArrIdx+index].bNewValidFlag = FALSE;
			DB104_FMT_TRACE("yc_db_record_str_read_zhj() ��������ʧ��  g_aYcDataConfig[%d].nDbCenterOAD=%08x, ret=%d \n", 
				index, g_aYcDataConfigZhj[ArrIdx+index].nDbCenterOAD, ret);
		}
	}
	else
	{
	    g_aYcDataSendZhj[ArrIdx+index].bNewValidFlag = FALSE;
		DB104_FMT_TRACE("yc_db_record_str_read_zhj() ��ȡ����  g_aYcDataConfig[%d].nDbCenterOAD=%08x \n", 
			index, g_aYcDataConfigZhj[ArrIdx+index].nDbCenterOAD);
		DB104_FMT_TRACE("yc_db_record_str_read_zhj() ���ݶ�ȡʧ��! \n");
	}
	
	if(nTimeFlag > 0)
    {
        DB104_FMT_TRACE("yc_db_record_str_read_zhj() ���ݴ�ʱ��nTimeFlag=%d \n", nTimeFlag);
        ooptm_to_cp56time2a(stTime, &g_aYcDataSendZhj[ArrIdx+index].btime[0]);
    }
	else
    {
        system_time_get(&g_aYcDataSendZhj[ArrIdx+index].btime[0]);
    }
}


/*******************************************************************************
* ��������: yc_db_record_read
* ��������: ��Ҫͳ�ƻ�����ң���ź�
* �������: typeIndex         �豸���������±�, ��0��ʼ
            groupIndex    �豸��������±�, ��0��ʼ
            seqIndex      ����ƫ�Ƶ�ַ�����±�, ��0��ʼ
            index         �������
* �������: ��
* �� �� ֵ: ��
*******************************************************************************/
void yc_calculate_statistics_zhj(uint8 typeIndex, uint16 groupIndex, uint16 seqIndex, uint16 index)
{
    int m = typeIndex;//�豸����-1
    int x = groupIndex;//�豸��� ��0��ʼ
    int n=  seqIndex;//�豸���-1
    uint16 nAreaId = g_nFixedAreaIdZhj[m][x][n];
    
	uint32 ArrIdx = 0;//ң��һά��������(��ʼ)
	uint32 ParaArrIdx = 0;//����һά��������(��ʼ)
    
    if(index >= g_nConfigYcNumZhj[m][x][n])
    {
        return;
    }
    
    int     ret  = -1;
    float32 fTemp1 = 0;
    float32 fTemp2 = 0;
    float32 fTemp3 = 0;

    float32 resultValue = 0;//���ֵ
    float32 fDeadZone;  //����ֵ
    float32 fDiffValue;  //��ֵ
    float32 fRatio = 1;  //����

    uint8 nTimeFlag = 0;//�Ƿ��ʱ��
    OOP_DATETIME_S_T stTime = {0};//��������ʱ��
    time_t tCurrentTime; //ϵͳ��ǰʱ��
	ArrIdx = g_aYcDataIndexZhj[m][x][n];
    ParaArrIdx = g_aParaValueIndexZhj[m][x][n];

    switch(g_aYcDataConfigZhj[ArrIdx+index].nDealType)
	{
		case YC_SP_RT_TMN_WORK_TIME:
		{
			time(&tCurrentTime);  //��ȡtime_t���͵ĵ�ǰʱ��
			resultValue = (uint32)((tCurrentTime - nTmnStartTime_t)/60);
            ret = 1;
		}
			break;
        case YC_SP_RT_VOLT_SPLIT:
		{
		    double pi=acos(-1.0);
		    int nItemIndex = g_aYcDataConfigZhj[ArrIdx+index].nItemIndex;
            int arrIndexVoltA = query_yc_index_config_zhj(m, n, n, 0x00000001);
            int arrIndexVoltB = query_yc_index_config_zhj(m, n, n, 0x00000002);
            int arrIndexVoltC = query_yc_index_config_zhj(m, n, n, 0x00000003);
            int arrIndexAnglA = query_yc_index_config_zhj(m, n, n, 0x00000017);
            int arrIndexAnglB = query_yc_index_config_zhj(m, n, n, 0x00000018);
            int arrIndexAnglC = query_yc_index_config_zhj(m, n, n, 0x00000019);
            if(arrIndexVoltA>=0 && arrIndexVoltB>=0 && arrIndexVoltC>=0 && 
                    arrIndexAnglA>=0 && arrIndexAnglB>=0 && arrIndexAnglC>=0)
            {
                if(0 == nItemIndex)//��ѹ�������
                {
                    fTemp1 = g_aYcDataSendZhj[ArrIdx+arrIndexVoltA].fNewValue*1 
                        + g_aYcDataSendZhj[ArrIdx+arrIndexVoltB].fNewValue*cos((g_aYcDataSendZhj[ArrIdx+arrIndexAnglB].fNewValue-g_aYcDataSendZhj[ArrIdx+arrIndexAnglA].fNewValue-120)/180.0*pi) 
                        + g_aYcDataSendZhj[ArrIdx+arrIndexVoltC].fNewValue*cos((g_aYcDataSendZhj[ArrIdx+arrIndexAnglC].fNewValue-g_aYcDataSendZhj[ArrIdx+arrIndexAnglA].fNewValue+120)/180.0*pi);
                    fTemp2 = g_aYcDataSendZhj[ArrIdx+arrIndexVoltA].fNewValue*0 
                            + g_aYcDataSendZhj[ArrIdx+arrIndexVoltB].fNewValue*sin((g_aYcDataSendZhj[ArrIdx+arrIndexAnglB].fNewValue-g_aYcDataSendZhj[ArrIdx+arrIndexAnglA].fNewValue-120)/180.0*pi) 
                            + g_aYcDataSendZhj[ArrIdx+arrIndexVoltC].fNewValue*sin((g_aYcDataSendZhj[ArrIdx+arrIndexAnglC].fNewValue-g_aYcDataSendZhj[ArrIdx+arrIndexAnglA].fNewValue+120)/180.0*pi);
                    fTemp3 = sqrt(pow(fTemp1, 2) + pow(fTemp2, 2));
                    resultValue = fTemp3;
                    ret = 1;
                }
                else if(1 == nItemIndex)//��ѹ�������
                {
                    fTemp1 = g_aYcDataSendZhj[ArrIdx+arrIndexVoltA].fNewValue*1 
                        + g_aYcDataSendZhj[ArrIdx+arrIndexVoltB].fNewValue*cos((g_aYcDataSendZhj[ArrIdx+arrIndexAnglB].fNewValue-g_aYcDataSendZhj[ArrIdx+arrIndexAnglA].fNewValue+120)/180.0*pi) 
                        + g_aYcDataSendZhj[ArrIdx+arrIndexVoltC].fNewValue*cos((g_aYcDataSendZhj[ArrIdx+arrIndexAnglC].fNewValue-g_aYcDataSendZhj[ArrIdx+arrIndexAnglA].fNewValue-120)/180.0*pi);
                    fTemp2 = g_aYcDataSendZhj[ArrIdx+arrIndexVoltA].fNewValue*0 
                            + g_aYcDataSendZhj[ArrIdx+arrIndexVoltB].fNewValue*sin((g_aYcDataSendZhj[ArrIdx+arrIndexAnglB].fNewValue-g_aYcDataSendZhj[ArrIdx+arrIndexAnglA].fNewValue+120)/180.0*pi) 
                            + g_aYcDataSendZhj[ArrIdx+arrIndexVoltC].fNewValue*sin((g_aYcDataSendZhj[ArrIdx+arrIndexAnglC].fNewValue-g_aYcDataSendZhj[ArrIdx+arrIndexAnglA].fNewValue-120)/180.0*pi);
                    fTemp3 = sqrt(pow(fTemp1, 2) + pow(fTemp2, 2));
                    resultValue = fTemp3;
                    ret = 1;
                }
                else if(2 == nItemIndex)//��ѹ�������
                {
                    fTemp1 = g_aYcDataSendZhj[ArrIdx+arrIndexVoltA].fNewValue*1 
                        + g_aYcDataSendZhj[ArrIdx+arrIndexVoltB].fNewValue*cos((g_aYcDataSendZhj[ArrIdx+arrIndexAnglB].fNewValue-g_aYcDataSendZhj[ArrIdx+arrIndexAnglA].fNewValue)/180.0*pi) 
                        + g_aYcDataSendZhj[ArrIdx+arrIndexVoltC].fNewValue*cos((g_aYcDataSendZhj[ArrIdx+arrIndexAnglC].fNewValue-g_aYcDataSendZhj[ArrIdx+arrIndexAnglA].fNewValue)/180.0*pi);
                    fTemp2 = g_aYcDataSendZhj[ArrIdx+arrIndexVoltA].fNewValue*0 
                            + g_aYcDataSendZhj[ArrIdx+arrIndexVoltB].fNewValue*sin((g_aYcDataSendZhj[ArrIdx+arrIndexAnglB].fNewValue-g_aYcDataSendZhj[ArrIdx+arrIndexAnglA].fNewValue)/180.0*pi) 
                            + g_aYcDataSendZhj[ArrIdx+arrIndexVoltC].fNewValue*sin((g_aYcDataSendZhj[ArrIdx+arrIndexAnglC].fNewValue-g_aYcDataSendZhj[ArrIdx+arrIndexAnglA].fNewValue)/180.0*pi);
                    fTemp3 = (sqrt(pow(fTemp1, 2) + pow(fTemp2, 2)))/3;
                    resultValue = fTemp3;
                    ret = 1;
                }
            }
		}
			break;

        case YC_SP_RT_CURR_SPLIT:
		{
		    double pi=acos(-1.0);
		    int nItemIndex = g_aYcDataConfigZhj[ArrIdx+index].nItemIndex;
            int arrIndexCurrA = query_yc_index_config_zhj(m, n, n, 0x00000004);
            int arrIndexCurrB = query_yc_index_config_zhj(m, n, n, 0x00000005);
            int arrIndexCurrC = query_yc_index_config_zhj(m, n, n, 0x00000006);
            int arrIndexAnglA = query_yc_index_config_zhj(m, n, n, 0x0000001A);
            int arrIndexAnglB = query_yc_index_config_zhj(m, n, n, 0x0000001B);
            int arrIndexAnglC = query_yc_index_config_zhj(m, n, n, 0x0000001C);
            if(arrIndexCurrA>=0 && arrIndexCurrA>=0 && arrIndexCurrA>=0 && 
                    arrIndexAnglA>=0 && arrIndexAnglB>=0 && arrIndexAnglC>=0)
            {
                if(0 == nItemIndex)//�����������
                {
                    fTemp1 = fabs(g_aYcDataSendZhj[ArrIdx+arrIndexCurrA].fNewValue)*1 
                        + fabs(g_aYcDataSendZhj[ArrIdx+arrIndexCurrB].fNewValue)*cos((g_aYcDataSendZhj[ArrIdx+arrIndexAnglB].fNewValue-g_aYcDataSendZhj[ArrIdx+arrIndexAnglA].fNewValue-120)/180.0*pi) 
                        + fabs(g_aYcDataSendZhj[ArrIdx+arrIndexCurrC].fNewValue)*cos((g_aYcDataSendZhj[ArrIdx+arrIndexAnglC].fNewValue-g_aYcDataSendZhj[ArrIdx+arrIndexAnglA].fNewValue+120)/180.0*pi);
                    fTemp2 = fabs(g_aYcDataSendZhj[ArrIdx+arrIndexCurrA].fNewValue)*0 
                            + fabs(g_aYcDataSendZhj[ArrIdx+arrIndexCurrB].fNewValue)*sin((g_aYcDataSendZhj[ArrIdx+arrIndexAnglB].fNewValue-g_aYcDataSendZhj[ArrIdx+arrIndexAnglA].fNewValue-120)/180.0*pi) 
                            + fabs(g_aYcDataSendZhj[ArrIdx+arrIndexCurrC].fNewValue)*sin((g_aYcDataSendZhj[ArrIdx+arrIndexAnglC].fNewValue-g_aYcDataSendZhj[ArrIdx+arrIndexAnglA].fNewValue+120)/180.0*pi);
                    fTemp3 = sqrt(pow(fTemp1, 2) + pow(fTemp2, 2));
                    resultValue = fTemp3;
                    ret = 1;
                }
                else if(1 == nItemIndex)//�����������
                {
                    fTemp1 = fabs(g_aYcDataSendZhj[ArrIdx+arrIndexCurrA].fNewValue)*1 
                        + fabs(g_aYcDataSendZhj[ArrIdx+arrIndexCurrB].fNewValue)*cos((g_aYcDataSendZhj[ArrIdx+arrIndexAnglB].fNewValue-g_aYcDataSendZhj[ArrIdx+arrIndexAnglA].fNewValue+120)/180.0*pi) 
                        + fabs(g_aYcDataSendZhj[ArrIdx+arrIndexCurrC].fNewValue)*cos((g_aYcDataSendZhj[ArrIdx+arrIndexAnglC].fNewValue-g_aYcDataSendZhj[ArrIdx+arrIndexAnglA].fNewValue-120)/180.0*pi);
                    fTemp2 = fabs(g_aYcDataSendZhj[ArrIdx+arrIndexCurrA].fNewValue)*0 
                            + fabs(g_aYcDataSendZhj[ArrIdx+arrIndexCurrB].fNewValue)*sin((g_aYcDataSendZhj[ArrIdx+arrIndexAnglB].fNewValue-g_aYcDataSendZhj[ArrIdx+arrIndexAnglA].fNewValue+120)/180.0*pi) 
                            + fabs(g_aYcDataSendZhj[ArrIdx+arrIndexCurrC].fNewValue)*sin((g_aYcDataSendZhj[ArrIdx+arrIndexAnglC].fNewValue-g_aYcDataSendZhj[ArrIdx+arrIndexAnglA].fNewValue-120)/180.0*pi);
                    fTemp3 = sqrt(pow(fTemp1, 2) + pow(fTemp2, 2));
                    resultValue = fTemp3;
                    ret = 1;
                }
                else if(2 == nItemIndex)//�����������
                {
                    fTemp1 = fabs(g_aYcDataSendZhj[ArrIdx+arrIndexCurrA].fNewValue)*1 
                        + fabs(g_aYcDataSendZhj[ArrIdx+arrIndexCurrB].fNewValue)*cos((g_aYcDataSendZhj[ArrIdx+arrIndexAnglB].fNewValue-g_aYcDataSendZhj[ArrIdx+arrIndexAnglA].fNewValue)/180.0*pi) 
                        + fabs(g_aYcDataSendZhj[ArrIdx+arrIndexCurrC].fNewValue)*cos((g_aYcDataSendZhj[ArrIdx+arrIndexAnglC].fNewValue-g_aYcDataSendZhj[ArrIdx+arrIndexAnglA].fNewValue)/180.0*pi);
                    fTemp2 = fabs(g_aYcDataSendZhj[ArrIdx+arrIndexCurrA].fNewValue)*0 
                            + fabs(g_aYcDataSendZhj[ArrIdx+arrIndexCurrB].fNewValue)*sin((g_aYcDataSendZhj[ArrIdx+arrIndexAnglB].fNewValue-g_aYcDataSendZhj[ArrIdx+arrIndexAnglA].fNewValue)/180.0*pi) 
                            + fabs(g_aYcDataSendZhj[ArrIdx+arrIndexCurrC].fNewValue)*sin((g_aYcDataSendZhj[ArrIdx+arrIndexAnglC].fNewValue-g_aYcDataSendZhj[ArrIdx+arrIndexAnglA].fNewValue)/180.0*pi);
                    fTemp3 = (sqrt(pow(fTemp1, 2) + pow(fTemp2, 2)))/3;
                    resultValue = fTemp3;
                    ret = 1;
                }
            }
		}
			break;

        case YC_SP_RT_VOLT_DIFF:
		{
		    
		    int nItemIndex = g_aYcDataConfigZhj[ArrIdx+index].nItemIndex;
            int arrIndexVoltA = query_yc_index_config_zhj(m, n, n, 0x00000001);
            int arrIndexVoltB = query_yc_index_config_zhj(m, n, n, 0x00000002);
            int arrIndexVoltC = query_yc_index_config_zhj(m, n, n, 0x00000003);
            int arrIndexVoltS = query_para_index_config_zhj(m, n, n, nAreaId, 0x00008008);//���ѹ
            if(0 == nItemIndex)//A�� ��ѹƫ��
            {
                if(arrIndexVoltA>=0 && arrIndexVoltS>=0)
                {
                    resultValue = g_aYcDataSendZhj[ArrIdx+arrIndexVoltA].fNewValue - g_aParaSetValueZhj[nAreaId][ParaArrIdx+arrIndexVoltS].unValue.float32V;
                    ret = 1;
                }
            }
            else if(1 == nItemIndex)//B�� ��ѹƫ��
            {
                if(arrIndexVoltB>=0 && arrIndexVoltS>=0)
                {
                    resultValue = g_aYcDataSendZhj[ArrIdx+arrIndexVoltB].fNewValue - g_aParaSetValueZhj[nAreaId][ParaArrIdx+arrIndexVoltS].unValue.float32V;
                    ret = 1;
                }
            }
            else if(2 == nItemIndex)//C�� ��ѹƫ��
            {
                if(arrIndexVoltC>=0 && arrIndexVoltS>=0)
                {
                    resultValue = g_aYcDataSendZhj[ArrIdx+arrIndexVoltC].fNewValue - g_aParaSetValueZhj[nAreaId][ParaArrIdx+arrIndexVoltS].unValue.float32V;
                    ret = 1;
                }
            }
		}
			break;

        case YC_SP_RT_FR_DIFF:
		{
		    int nItemIndex = g_aYcDataConfigZhj[ArrIdx+index].nItemIndex;
            int arrIndexFrT = query_yc_index_config_zhj(m, n, n, 0x0000001E);//��Ƶ��
            int arrIndexFrA = query_yc_index_config_zhj(m, n, n, 0x0000001F);
            int arrIndexFrB = query_yc_index_config_zhj(m, n, n, 0x00000020);
            int arrIndexFrC = query_yc_index_config_zhj(m, n, n, 0x00000021);
            if(0 == nItemIndex)//��
            {
                if(arrIndexFrT>=0)
                {
                    resultValue = g_aYcDataSendZhj[ArrIdx+arrIndexFrT].fNewValue - 50;//�Ƶ��Ĭ��50
                    ret = 1;
                }
            }
            else if(1 == nItemIndex)//AƵ��
            {
                if(arrIndexFrA>=0)
                {
                    resultValue = g_aYcDataSendZhj[ArrIdx+arrIndexFrA].fNewValue - 50;//�Ƶ��Ĭ��50
                    ret = 1;
                }
            }
            else if(2 == nItemIndex)//BƵ��
            {
                if(arrIndexFrB>=0)
                {
                    resultValue = g_aYcDataSendZhj[ArrIdx+arrIndexFrB].fNewValue - 50;//�Ƶ��Ĭ��50
                    ret = 1;
                }
            }
            else if(3 == nItemIndex)//CƵ��
            {
                if(arrIndexFrC>=0)
                {
                    resultValue = g_aYcDataSendZhj[ArrIdx+arrIndexFrC].fNewValue - 50;//�Ƶ��Ĭ��50
                    ret = 1;
                }
            }
		}
			break;

        case YC_SP_RT_VOLT_IMBALA:
		{
/* ��ѹ��ƽ��ȹ���ļ��㷽���ǣ�����/���� */
#ifdef GUO_BIAO
            int arrIndexVoltP = query_yc_index_config_zhj(m, n, n, 0x00000022);//��ѹ�������
            int arrIndexVoltN = query_yc_index_config_zhj(m, n, n, 0x00000023);//��ѹ�������
            float32 fBaseValue = g_aYcDataSendZhj[ArrIdx+arrIndexVoltP].fNewValue;
            if(arrIndexVoltP>=0 && arrIndexVoltN>=0 && fBaseValue!=0)
            {
                resultValue = g_aYcDataSendZhj[ArrIdx+arrIndexVoltN].fNewValue/fBaseValue*100;//����/���� ��λ%
            }
            
/* ��ѹ��ƽ��ȹ���ļ��㷽���ǣ���ѹ���ֵ - ��ѹ��Сֵ / ��ѹƽ��ֵ */
#else
            int arrIndexVoltA = query_yc_index_config_zhj(m, n, n, 0x00000001);
            int arrIndexVoltB = query_yc_index_config_zhj(m, n, n, 0x00000002);
            int arrIndexVoltC = query_yc_index_config_zhj(m, n, n, 0x00000003);
            float32 voltA = g_aYcDataSendZhj[ArrIdx + arrIndexVoltA].fNewValue;
            float32 voltB = g_aYcDataSendZhj[ArrIdx + arrIndexVoltB].fNewValue;
            float32 voltC = g_aYcDataSendZhj[ArrIdx + arrIndexVoltC].fNewValue;
            float32 max = (((voltA < voltB) ? voltB : voltA) < voltC ? voltC : ((voltA < voltB) ? voltB : voltA));
            float32 min = (((voltA < voltB) ? voltA : voltB) < voltC ? ((voltA < voltB) ? voltA : voltB) : voltC);
            float32 average = (voltA + voltB + voltC) / 3;
            if (average != 0)
                resultValue = (max - min) / average * 100;
#endif
		}
            ret = 1;
			break;

        case YC_SP_RT_CURR_IMBALA:
		{
/* ������ƽ��ȹ���ļ��㷽���ǣ�����/���� */
#ifdef GUO_BIAO
            int arrIndexCurrP = query_yc_index_config_zhj(m, n, n, 0x00000025);//�����������
            int arrIndexCurrN = query_yc_index_config_zhj(m, n, n, 0x00000026);//�����������
            float32 fBaseValue = g_aYcDataSendZhj[ArrIdx+arrIndexCurrP].fNewValue;
            if(arrIndexCurrP>=0 && arrIndexCurrN>=0 && fBaseValue!=0)
            {
                resultValue = g_aYcDataSendZhj[ArrIdx+arrIndexCurrN].fNewValue/fBaseValue*100;//����/���� ��λ%
            }
/* ������ƽ��ȹ���ļ��㷽���ǣ��������ֵ - ������Сֵ / ����ƽ��ֵ */            
#else
            int arrIndexCurrA = query_yc_index_config_zhj(m, n, n, 0x00000004);
            int arrIndexCurrB = query_yc_index_config_zhj(m, n, n, 0x00000005);
            int arrIndexCurrC = query_yc_index_config_zhj(m, n, n, 0x00000006);
            float32 currA = g_aYcDataSendZhj[ArrIdx+arrIndexCurrA].fNewValue;
            float32 currB = g_aYcDataSendZhj[ArrIdx+arrIndexCurrB].fNewValue;
            float32 currC = g_aYcDataSendZhj[ArrIdx+arrIndexCurrC].fNewValue;
            float32 max = (((currA < currB) ? currB : currA) < currC ? currC : ((currA < currB) ? currB : currA));
            float32 min = (((currA < currB) ? currA : currB) < currC ? ((currA < currB) ? currA : currB) : currC);
            float32 average = (currA + currB + currC) / 3;
            if (average != 0)
                resultValue = (max - min) / average * 100;
#endif
            ret = 1;
		}
			break;

        case YC_SP_RT_LOAD_RATE:
		{
		    int arrIndexS = query_yc_index_config_zhj(m, n, n, 0x00000013);//���ڹ���
            int arrIndexCap = query_para_index_config_zhj(m, n, n, nAreaId, 0x0000800B);//����
            int arrIndexCT = query_para_index_config_zhj(m, n, n, nAreaId, 0x00008006);//CT���
            int arrIndexPT = query_para_index_config_zhj(m, n, n, nAreaId, 0x00008007);//PT��
            if(arrIndexS>=0 && arrIndexCap>=0 && arrIndexCT>=0 && arrIndexPT>=0)
            {
                float32 fCapValue = g_aParaSetValueZhj[nAreaId][ParaArrIdx+arrIndexCap].unValue.float32V;
                float32 fCtValue = g_aParaSetValueZhj[nAreaId][ParaArrIdx+arrIndexCT].unValue.uint16V;
                float32 fPtValue = g_aParaSetValueZhj[nAreaId][ParaArrIdx+arrIndexPT].unValue.uint16V;
                float32 fBaseValue = fCapValue*1000;
                if(fBaseValue!=0)
                {
                    resultValue = g_aYcDataSendZhj[ArrIdx+arrIndexS].fNewValue*fCtValue*fPtValue/fBaseValue*100;//���ڹ���/���� ��λ%
                    ret = 1;
                }
                
            }
		}
			break;
        case YC_SP_RT_U_HMONIC_ODD:
		{
		    int nItemIndex = g_aYcDataConfigZhj[ArrIdx+index].nItemIndex;
            if(0 == nItemIndex)//A�� ��ѹ ���г��
            {
                int arrIndexStart = query_yc_index_config_zhj(m, n, n, 0x00000201);//3��
                int arrIndexEnd = query_yc_index_config_zhj(m, n, n, 0x00000211);//���19��
                while(arrIndexStart <= arrIndexEnd)
                {
                    resultValue += g_aYcDataSendZhj[ArrIdx+arrIndexStart].fNewValue;
                    arrIndexStart += 2;
                    ret = 1;
                }
            }
            else if(1 == nItemIndex)//B�� ��ѹ ���г��
            {
                int arrIndexStart = query_yc_index_config_zhj(m, n, n, 0x00000251);//3��
                int arrIndexEnd = query_yc_index_config_zhj(m, n, n, 0x00000261);//���19��
                while(arrIndexStart <= arrIndexEnd)
                {
                    resultValue += g_aYcDataSendZhj[ArrIdx+arrIndexStart].fNewValue;
                    arrIndexStart += 2;
                    ret = 1;
                }
            }
            else if(2 == nItemIndex)//C�� ��ѹ ���г��
            {
                int arrIndexStart = query_yc_index_config_zhj(m, n, n, 0x000002A1);//3��
                int arrIndexEnd = query_yc_index_config_zhj(m, n, n, 0x000002B1);//���19��
                while(arrIndexStart <= arrIndexEnd)
                {
                    resultValue += g_aYcDataSendZhj[ArrIdx+arrIndexStart].fNewValue;
                    arrIndexStart += 2;
                    ret = 1;
                }
            }
		}
			break;
        case YC_SP_RT_U_HMONIC_EVEN:
		{
		    int nItemIndex = g_aYcDataConfigZhj[ArrIdx+index].nItemIndex;
            if(0 == nItemIndex)//A�� ��ѹ ż��г��
            {
                int arrIndexStart = query_yc_index_config_zhj(m, n, n, 0x00000200);//2��
                int arrIndexEnd = query_yc_index_config_zhj(m, n, n, 0x00000210);//���18��
                while(arrIndexStart <= arrIndexEnd)
                {
                    resultValue += g_aYcDataSendZhj[ArrIdx+arrIndexStart].fNewValue;
                    arrIndexStart += 2;
                    ret = 1;
                }
            }
            else if(1 == nItemIndex)//B�� ��ѹ ż��г��
            {
                int arrIndexStart = query_yc_index_config_zhj(m, n, n, 0x00000250);//2��
                int arrIndexEnd = query_yc_index_config_zhj(m, n, n, 0x00000260);//���18��
                while(arrIndexStart <= arrIndexEnd)
                {
                    resultValue += g_aYcDataSendZhj[ArrIdx+arrIndexStart].fNewValue;
                    arrIndexStart += 2;
                    ret = 1;
                }
            }
            else if(2 == nItemIndex)//C�� ��ѹ ż��г��
            {
                int arrIndexStart = query_yc_index_config_zhj(m, n, n, 0x000002A0);//2��
                int arrIndexEnd = query_yc_index_config_zhj(m, n, n, 0x000002B0);//���18��
                while(arrIndexStart <= arrIndexEnd)
                {
                    resultValue += g_aYcDataSendZhj[ArrIdx+arrIndexStart].fNewValue;
                    arrIndexStart += 2;
                    ret = 1;
                }
            }
		}
			break;
		
		default:
			break;
	}

    DB104_FMT_TRACE("m=%d, x=%d, n=%d, <--��%d��ң��--> addr=%08x, dealtype=%08x, yc_calculate_statistics_zhj() ret:%d \n", 
        m, x, n, index, g_aYcDataConfigZhj[ArrIdx+index].nObjAddrU32, g_aYcDataConfigZhj[ArrIdx+index].nDealType, ret);

    //�жϽ���Ƿ���Ч
    if(ret > 0)
    {
        g_aYcDataSendZhj[ArrIdx+index].bNewValidFlag = TRUE;
        
        g_aYcDataSendZhj[ArrIdx+index].fNewValue = resultValue*fRatio;
        DB104_FMT_TRACE("���ʴ���֮��g_aYcDataSend[index].fNewValue=%f, ���ʴ���֮ǰfValueDB=%f \n", g_aYcDataSendZhj[ArrIdx+index].fNewValue, resultValue);
        
        //��ȡ�仯����ֵ
        fDeadZone = g_aYcDataConfigZhj[ArrIdx+index].fDeadZoneValue;
        DB104_FMT_TRACE("�仯����ֵfDeadZone=%f \n", fDeadZone);

        //��ʱ��ֵ, ���Ա仯����
        if(g_bTestEnable)
        {
            g_aYcDataSendZhj[ArrIdx+index].fHisValue = rand()%100;//100���������
            DB104_FMT_TRACE("ң������� g_aYcDataSend[%d].fHisValue=%f \n", index, g_aYcDataSendZhj[ArrIdx+index].fHisValue);
        }

        //�ж�ʵʱֵ�Ƿ�Խ������
        fDiffValue = fabs(g_aYcDataSendZhj[ArrIdx+index].fNewValue - g_aYcDataSendZhj[ArrIdx+index].fHisValue);
        if(fDiffValue > fDeadZone)
        {
            DB104_FMT_TRACE("�仯ֵfDiffValue=%f, ��������ֵ, fNewValue=%f, fHisValue=%f ��Ҫ�����ϱ�! \n", 
                fDiffValue, g_aYcDataSendZhj[ArrIdx+index].fNewValue, g_aYcDataSendZhj[ArrIdx+index].fHisValue);
            g_bYcChanged = TRUE;
            g_aYcDataSendZhj[ArrIdx+index].bChangeFlag = TRUE;
            g_aYcDataSendZhj[ArrIdx+index].bSendFlag = FALSE;
            if(M_ME_NC_1 == g_aYcDataConfigZhj[ArrIdx+index].nSignalType)
            {
                g_aChangedEquipStateYC_R[m][x][n] = 1;
            }
            else if(M_ME_TF_1 == g_aYcDataConfigZhj[ArrIdx+index].nSignalType)
            {
                g_aChangedEquipStateYC_R_T[m][x][n] = 1;
            }
            else if(M_IT_NB_1 == g_aYcDataConfigZhj[ArrIdx+index].nSignalType)
            {
                g_aChangedEquipStateYC_R_EN[m][x][n] = 1;
            }
        }
        else
        {
            DB104_FMT_TRACE("�仯ֵfDiffValue=%f, С������ֵ, fNewValue=%f, fHisValue=%f \n", 
                fDiffValue, g_aYcDataSendZhj[ArrIdx+index].fNewValue, g_aYcDataSendZhj[ArrIdx+index].fHisValue);
        }
        
    }
    else
    {
        g_aYcDataSendZhj[ArrIdx+index].bNewValidFlag = FALSE;
        DB104_FMT_TRACE("yc_db_nomal_read() ��������ʧ��  g_aYcDataConfig[%d].nDbCenterOAD=%08x \n", 
            index, g_aYcDataConfigZhj[ArrIdx+index].nDbCenterOAD);
    }

    if(nTimeFlag > 0)
    {
        DB104_FMT_TRACE("yc_calculate_statistics_zhj() ���ݴ�ʱ��nTimeFlag=%d \n", nTimeFlag);
        ooptm_to_cp56time2a(stTime, &g_aYcDataSendZhj[ArrIdx+index].btime[0]);
    }
    else
    {
        system_time_get(&g_aYcDataSendZhj[ArrIdx+index].btime[0]);
    }
}

/*******************************************************************************
* ��������: fault_buff_combine_zhj
* ��������: �����¼�������֡
* �������: ��
* �������: ��
* �� �� ֵ: ��
*******************************************************************************/
int fault_buff_combine_zhj(uint8 eqTpIndex, uint8 eqGroupIndex, uint16 eqNoIndex, OOP_PROTECTOR_31BB_T event, TOTAL_104_FRAME_T *pTotal104Frame)
{
    //uint8 addr[4] = {0};//��Ϣ���ַ
    int index = 0;//ƫ��λ
    uint16 nTempU16 = 0;
    uint32 nTempU32 = 0;
    float32 fTempF32 = 0;
    
    TOTAL_104_FRAME_T stTotal104Frame;
    memset(&stTotal104Frame, 0, sizeof(TOTAL_104_FRAME_T));

    //HEAD
    stTotal104Frame.nHead = 0x68;

    //������(��ʱ����)
    
    //��������
    stTotal104Frame.nDataType = M_FT_NA_ZHJ_1;

    //VSQ
    stTotal104Frame.nVSQ = 0x00;

    //COT
    stTotal104Frame.nTransCauseL = CAUSE_SPONT;
    stTotal104Frame.nTransCauseH = 0x00;

    //������ַ
    stTotal104Frame.nPubAddrL = eqNoIndex + 1;
    stTotal104Frame.nPubAddrH = (uint8)((uint8)(eqTpIndex+1) << 3) + eqGroupIndex;

    //�¼�����
    nTempU16 = 0x0121;
    memcpy(&stTotal104Frame.aObjData[index], &nTempU16, 2);
    index += 2;

    //�¼�����ʱ��
    ooptm_to_cp56time2a(event.tmStart, &stTotal104Frame.aObjData[index]);
    index += 7;

    //ң�Ÿ���
    stTotal104Frame.aObjData[index] = 0;
    index += 1;

    //ң�����
    stTotal104Frame.aObjData[index] = 7;
    index += 1;
    
    nTempU32 = 0x00000011;//1 ��ǰʣ�����
    memcpy(&stTotal104Frame.aObjData[index], &nTempU32, 3);//��ַ
    index += 3;
    fTempF32 = (float32)event.residualcurrent;
    memcpy(&stTotal104Frame.aObjData[index], &fTempF32, 4);//ֵ
    index += 4;
    stTotal104Frame.aObjData[index] = 0x00;//Ʒ��
    index += 1;
    
    nTempU32 = 0x00000001;//2 A���ѹ
    memcpy(&stTotal104Frame.aObjData[index], &nTempU32, 3);//��ַ
    index += 3;
    fTempF32 = (float32)event.voltage[0];
    memcpy(&stTotal104Frame.aObjData[index], &fTempF32, 4);//ֵ
    index += 4;
    stTotal104Frame.aObjData[index] = 0x00;//Ʒ��
    index += 1;

    nTempU32 = 0x00000002;//3 B���ѹ
    memcpy(&stTotal104Frame.aObjData[index], &nTempU32, 3);//��ַ
    index += 3;
    fTempF32 = (float32)event.voltage[1];
    memcpy(&stTotal104Frame.aObjData[index], &fTempF32, 4);//ֵ
    index += 4;
    stTotal104Frame.aObjData[index] = 0x00;//Ʒ��
    index += 1;

    nTempU32 = 0x00000003;//4 C���ѹ
    memcpy(&stTotal104Frame.aObjData[index], &nTempU32, 3);//��ַ
    index += 3;
    fTempF32 = (float32)event.voltage[2];
    memcpy(&stTotal104Frame.aObjData[index], &fTempF32, 4);//ֵ
    index += 4;
    stTotal104Frame.aObjData[index] = 0x00;//Ʒ��
    index += 1;

    nTempU32 = 0x00000004;//5 A�����
    memcpy(&stTotal104Frame.aObjData[index], &nTempU32, 3);//��ַ
    index += 3;
    fTempF32 = (float32)event.current[0];
    memcpy(&stTotal104Frame.aObjData[index], &fTempF32, 4);//ֵ
    index += 4;
    stTotal104Frame.aObjData[index] = 0x00;//Ʒ��
    index += 1;

    nTempU32 = 0x00000005;//6 B�����
    memcpy(&stTotal104Frame.aObjData[index], &nTempU32, 3);//��ַ
    index += 3;
    fTempF32 = (float32)event.current[1];
    memcpy(&stTotal104Frame.aObjData[index], &fTempF32, 4);//ֵ
    index += 4;
    stTotal104Frame.aObjData[index] = 0x00;//Ʒ��
    index += 1;

    nTempU32 = 0x00000006;//7 C�����
    memcpy(&stTotal104Frame.aObjData[index], &nTempU32, 3);//��ַ
    index += 3;
    fTempF32 = (float32)event.current[2];
    memcpy(&stTotal104Frame.aObjData[index], &fTempF32, 4);//ֵ
    index += 4;
    stTotal104Frame.aObjData[index] = 0x00;//Ʒ��
    index += 1;

    //ң�����(��ʱ��)
    stTotal104Frame.aObjData[index] = 0;
    index += 1;

    //����������
    stTotal104Frame.aObjData[index] = 0;
    index += 1;

    //���ش�����
    stTotal104Frame.aObjData[index] = 2;
    index += 1;
    nTempU32 = 0x00020002;//1 �������
    memcpy(&stTotal104Frame.aObjData[index], &nTempU32, 3);//��ַ
    index += 3;
    stTotal104Frame.aObjData[index] = 1;//����
    index += 1;
    stTotal104Frame.aObjData[index] = event.faultphase;//ֵ
    index += 1;
	stTotal104Frame.aObjData[index] = 0x00;//Ʒ��
    index += 1;
    
    nTempU32 = 0x00020003;//2 ����״̬��
    memcpy(&stTotal104Frame.aObjData[index], &nTempU32, 3);//��ַ
    index += 3;
    stTotal104Frame.aObjData[index] = 1;//����
    index += 1;
    stTotal104Frame.aObjData[index] = 0x00;//ֵ
    index += 1;
	stTotal104Frame.aObjData[index] = 0x00;//Ʒ��
    index += 1;

	//��������
    stTotal104Frame.aObjData[index] = 0;
	index += 1;
	
    //���±��ĳ���(������ͷ���ͳ�������)
    stTotal104Frame.nLength = CTRL_UNIT_LABEL_LEN + index;

    memcpy(pTotal104Frame, &stTotal104Frame, sizeof(TOTAL_104_FRAME_T));
    return (stTotal104Frame.nLength+2);
}

/*******************************************************************************
* ��������: yc_db_event_record_read_zhj
* ��������: ���������Ķ�ȡ�豸�¼�����(��¼������)
* �������: typeIndex         �豸���������±�, ��0��ʼ
            groupIndex    �豸��������±�, ��0��ʼ
            seqIndex      ����ƫ�Ƶ�ַ�����±�, ��0��ʼ
            index         �������
* �������: ��
* �� �� ֵ: ��
*******************************************************************************/
void yc_db_event_record_read_zhj(uint8 typeIndex, uint16 groupIndex, uint16 seqIndex, uint16 index)
{
    int m = typeIndex;//�豸����-1
    int x = groupIndex;//�豸��� ��0��ʼ
    int n=  seqIndex;//�豸���-1
    int i = 0;//��ֵ�±�
    uint16 areaID = g_nFixedAreaIdZhj[m][x][n];//��ֵ����
    uint32 macAddr = 0;//�����������豸��ַ�ĵ��
    static OOP_DATETIME_S_T tmStart = {0};  //�¼�����ʱ��

    /*
	if(index >= g_nConfigYcNumZhj[m][x][n])
	{
		return;
	}
	*/

	uint8	aRecvBuff[500] = {0};
	uint32	nRevcBuffLen = 0;
	int 	ret  = 0;
	READ_RECORD_T ReadRecord;  //��¼��OAD
	RECORD_UNIT_T RecordOut;  //��¼�Ͳ�ѯ���
	memset(&ReadRecord, 0, sizeof(READ_RECORD_T));
	memset(&RecordOut, 0, sizeof(RECORD_UNIT_T));

    uint32 ArrIdx = 0;//һά��������(��ʼ)
    ArrIdx = g_aParaValueIndexZhj[m][x][n];
	
	time_t nRealTime_t;  //��ǰϵͳʱ���ת��������

    TOTAL_104_FRAME_T stTotal104Frame;
    memset(&stTotal104Frame, 0, sizeof(TOTAL_104_FRAME_T));
    int nTotal104FrameLen = 0;

    //�¼����ò���
    YC_DATA_CONFIG stEventConfig;
    memset(&stEventConfig, 0, sizeof(YC_DATA_CONFIG));
    stEventConfig.nLogicId = 7;
    stEventConfig.nInfoNum = (uint16)((uint16)(m+1) << 11) + (uint16)((uint16)(x) << 8) + (n+1);
    stEventConfig.nDbCenterOAD = 0x31BB0200;
    stEventConfig.nOadColsNum = 1;
    stEventConfig.aOadCols[0] = 0x201F0800;

	//���ݶ�ȡ
	nRealTime_t = time(NULL);
	
	ReadRecord.recordOAD.logicId = stEventConfig.nLogicId;  //�߼���ַ
	ReadRecord.recordOAD.infoNum = stEventConfig.nInfoNum;  //��Ϣ���
	ReadRecord.recordOAD.road.oadMain.value = stEventConfig.nDbCenterOAD;  //��OAD
	ReadRecord.recordOAD.road.oadCols.nNum = stEventConfig.nOadColsNum;  //��OAD����
	memcpy(&ReadRecord.recordOAD.road.oadCols.oad[0], &stEventConfig.aOadCols[0],
		stEventConfig.nOadColsNum*sizeof(uint32));  //������OAD
	ReadRecord.recordOAD.classtag = CLASS_DATA_COLLEC;

	ReadRecord.cType = COL_TM_STORE;  //�ɼ��洢ʱ��
	//ReadRecord.cStart = nRealTime_t-24*3600;  //ʱ����ʼ�㣬��ǰʱ����ǰһ��
	ReadRecord.cStart = 0;  //ʱ����ʼ�㣬��ǰʱ����ǰһ��
	ReadRecord.cEnd = nRealTime_t + 8*60*60;  //ʱ�������
	ReadRecord.sortType = DOWN_SORT;  //����ʽ ����
	ReadRecord.MAC.nNum = 6;  //����У�� ����
	if(E_EQUIP_CAPACITOR == (typeIndex + 1))
    {
        macAddr = 0x00008003;
    }
    else if(E_EQUIP_RESIDUALCURRENT == (typeIndex + 1))
    {
        macAddr = 0x00008003;
    }

    for(i=0; i<g_nConfigParaNumZhj[m][x][n]; i++)
    {
        if(g_aParaSetValueZhj[areaID][ArrIdx+i].nObjAddrU32 == macAddr)
        {
            memcpy(ReadRecord.MAC.value, g_aParaSetValueZhj[areaID][ArrIdx+i].aArrValue, ReadRecord.MAC.nNum);
            break;
        }
    }
	
	ReadRecord.ti.unit = TI_HOUR;  //ʱ���� ��λ
	ReadRecord.ti.value = 15;  //ʱ���� ֵ

	DB104_FMT_TRACE("m=%d, n=%d <--��%d��ң��--> yc_db_event_record_read_zhj() �����±�index=%d, �߼���ַlogicId=%d, ��Ϣ���infoNum=%d, ��OAD=%08x, ��OAD����=%d, ��OAD=%08x, У��MAC.nNum=%d, MAC.value=%02x%02x%02x%02x%02x%02x \n", 
		m, n, index, index, ReadRecord.recordOAD.logicId, ReadRecord.recordOAD.infoNum, ReadRecord.recordOAD.road.oadMain.value, ReadRecord.recordOAD.road.oadCols.nNum, 
		ReadRecord.recordOAD.road.oadCols.oad[0].value, ReadRecord.MAC.nNum, ReadRecord.MAC.value[5], ReadRecord.MAC.value[4], ReadRecord.MAC.value[3], ReadRecord.MAC.value[2], ReadRecord.MAC.value[1], ReadRecord.MAC.value[0]);
	
	ret = db_read_record(g_sDataCenterClient, &ReadRecord, sizeof(aRecvBuff), &RecordOut);
	DB104_FMT_TRACE("yc_db_center_read() ret = db_read_record() ret:%d \n", ret);
	if(ret == 0)
	{
		nRevcBuffLen = RecordOut.record[0].outDatalen;
		memcpy(&aRecvBuff[0], &RecordOut.record[0].outDataBuf[0], nRevcBuffLen);

		DB104_FMT_TRACE("yc_db_event_record_read_zhj() ��ȡ���ݳɹ�  nDbCenterOAD=%08x \n", 
			stEventConfig.nDbCenterOAD);
		DB104_BUF_TRACE(aRecvBuff, nRevcBuffLen, "����: ");

		OOP_PROTECTOR_31BB_T event = {0};
		memcpy(&event, aRecvBuff, sizeof(OOP_PROTECTOR_31BB_T));

        if(0 == memcmp(&tmStart, &event.tmStart, sizeof(OOP_DATETIME_S_T)))
        {
            //�����¼��ķ���ʱ����ͬ, ��Ϊ��ͬһ���¼�
        }
        else
        {
            DB104_FMT_TRACE("�ϴ��¼�����ʱ�� %d-%d-%d %d:%d:%d \n", tmStart.year, tmStart.month,       tmStart.day, tmStart.hour, tmStart.minute, tmStart.second);
            DB104_FMT_TRACE("����ʱ�䷢��ʱ�� %d-%d-%d %d:%d:%d \n", event.tmStart.year, event.tmStart.month, event.tmStart.day, event.tmStart.hour, event.tmStart.minute, event.tmStart.second);
            
            memcpy(&tmStart, &event.tmStart, sizeof(OOP_DATETIME_S_T));
            nTotal104FrameLen = fault_buff_combine_zhj(m, x, n, event, &stTotal104Frame);
            if(nTotal104FrameLen > 0)
            {
                pw_lock_queue_write(&g_stSoeBuff, (uint8 *)&stTotal104Frame, nTotal104FrameLen, 0);
                DB104_BUF_TRACE((uint8 *)&stTotal104Frame, nTotal104FrameLen, "��ʱFAULT����:");
            }
        }
	}
	else
	{
	    DB104_FMT_TRACE("yc_db_event_record_read_zhj() ��ȡ����  nDbCenterOAD=%08x \n", 
			stEventConfig.nDbCenterOAD);
		DB104_FMT_TRACE("yc_db_event_record_read_zhj() ���ݶ�ȡʧ��! \n");
	}
}


/*******************************************************************************
* ��������: data_db_center_read_zhj
* ��������: ���������Ķ�ȡʵʱ����
* �������: READ_STATE_T ReadState  ��ȡ���ݼ�ʱ��
* �������: ��
* �� �� ֵ: ��
*******************************************************************************/
void data_db_center_read_zhj(READ_STATE_T ReadState)
{
    int m = 0;//�豸����
    int x = 0;//�豸���
    int n = 0;//�豸���
	int i = 0;
    uint32 ArrIdx = 0;//һά��������(��ʼ)
	
	//ң�����ݶ�ȡ
	//DB104_FMT_TRACE("\n************data_db_center_read() ң�����ݶ�ȡ start...************ \n\n");
    for(m=0; m<MAX_EQUIP_TYPE_NUM; m++)//����
    {
        if(0 == g_aGroupCountZhj[m])
        {
            continue;//�����͵�������Ϊ0
        }
        
        for(x=0; x<g_aGroupCountZhj[m]; x++)//���
        {
            if(0 == g_aEquipCountZhj[m][x])
            {
                continue;//�����͵��豸����Ϊ0
            }

            for(n=0; n<g_aEquipCountZhj[m][x]; n++)
            {
                ArrIdx = g_aYxDataIndexZhj[m][x][n];
                for(i=0; i<g_nConfigYxNumZhj[m][x][n]; i++)
            	{
            		if(g_aYxDataConfigZhj[ArrIdx+i].nDbCenterOAD == 0xFFFFFFFF)
            		{
            			if(g_aYxDataConfigZhj[ArrIdx+i].nDealType == CALC_YX_REAL_SIGNAL)
            			{
            			    if(1 == ReadState.nHardYxFlag)
                            {
                                //��������ѯ��Ϊ�¼���������
                                //yx_real_signal_read_zhj(m, x, n, i);//Ӳң��
                            }
            			}
            			else
            			{
            			    if(1 == ReadState.nCommonYxFlag)
                            {
                                //yx_calculate_statistics_zhj(m, x, n, i);//ͳ������, ��ʵ�ʲɼ�
                            }
            			}
            		}
            		else
            		{
            		    if(1 == ReadState.nCommonYxFlag)
                        {
                            //yx_db_center_read_zhj(m, x, n, i);//���������Ķ�ȡ����
                        }
            		}
            	}
            }
        }
    }

	//ң�����ݶ�ȡ
	//DB104_FMT_TRACE("\n************data_db_center_read() ң�����ݶ�ȡ start...************ \n\n");
    for(m=0; m<MAX_EQUIP_TYPE_NUM; m++)
    {
        if(0 == g_aGroupCountZhj[m])
        {
            continue;//�����͵�������Ϊ0
        }
        
        for(x=0; x<g_aGroupCountZhj[m]; x++)//���
        {
            if(0 == g_aEquipCountZhj[m][x])
            {
                continue;//�����͵��豸����Ϊ0
            }

            for(n=0; n<g_aEquipCountZhj[m][x]; n++)
            {
                ArrIdx = g_aYcDataIndexZhj[m][x][n];
                for(i=0; i<g_nConfigYcNumZhj[m][x][n]; i++)
            	{
            	    if(M_VBO_TB_1 == g_aYcDataConfigZhj[ArrIdx+i].nSignalType)//���ش�����
                    {
                        //���ش�����
                        if(g_aYcDataConfigZhj[ArrIdx+i].nDbCenterOAD != 0xFFFFFFFF)
                        {
                            if(g_aYcDataConfigZhj[ArrIdx+i].nLogicId == 7)//7 �����豸
                			{
                			    if(g_aYcDataConfigZhj[ArrIdx+i].nOadColsNum > 0)//��OAD����Ҫ����0
                                {
                                    if(1 == ReadState.nRecordYcFlag)
                                    {
                                        yc_db_record_str_read_zhj(m, x, n, i);
                                    }
                                }
                    	    }
                        }
                    }
                    else//����������
                    {
                        if(g_aYcDataConfigZhj[ArrIdx+i].nDbCenterOAD == 0xFFFFFFFF)
                		{
                		    if(1 == ReadState.nCommonYcFlag)
                            {
                                //�в�֪��δ��������
                            }
                		}
                        else if( (YC_SP_RT_START < g_aYcDataConfigZhj[ArrIdx+i].nDealType) 
                                    && (YC_SP_RT_END > g_aYcDataConfigZhj[ArrIdx+i].nDealType) )
                        {
                            if(1 == ReadState.nCommonYcFlag)
                            {
                                yc_calculate_statistics_zhj(m, x, n, i);//ʵʱͳ������, ��ʵ�ʲɼ�
                            }
                        } 
                        else if( (YC_SP_PRE_DAY_START < g_aYcDataConfigZhj[ArrIdx+i].nDealType) 
                                    && (YC_SP_PRE_DAY_END > g_aYcDataConfigZhj[ArrIdx+i].nDealType) )
                        {
                            if(1 == ReadState.nPreDayYcFlag)
                            {
                                //yc_db_sp_record_read_zhj(m, x, n, i);//����ͳ������
                            }
                        }
                        else if( (YC_SP_PRE_MON_START < g_aYcDataConfigZhj[ArrIdx+i].nDealType) 
                                    && (YC_SP_PRE_MON_END > g_aYcDataConfigZhj[ArrIdx+i].nDealType) )
                        {
                            if(1 == ReadState.nPreMonYcFlag)
                            {
                                //yc_db_sp_record_read_zhj(m, x, n, i);//����ͳ������
                            }
                        }
                        else if ( ((YC_SP_ON_DAY_START < g_aYcDataConfigZhj[ArrIdx+i].nDealType) 
                                    && (YC_SP_ON_DAY_END > g_aYcDataConfigZhj[ArrIdx+i].nDealType)) || 
                                   ((YC_SP_ON_MON_START < g_aYcDataConfigZhj[ArrIdx+i].nDealType) 
                                    && (YC_SP_ON_MON_END > g_aYcDataConfigZhj[ArrIdx+i].nDealType)) )
                        {
                            /* �������µ�ͳ�����ݣ�����������ˣ�Ϊ�˱������ݸ����ӳ٣�����Ϊ��������ԣ��ݲ����� */
                        }
                        else if (YC_SP_INVALID == g_aYcDataConfigZhj[ArrIdx+i].nDealType)
                        {
                            /* �������� */
                        }
                        else
                        {
                            if(g_aYcDataConfigZhj[ArrIdx+i].nLogicId == 1)//��������
                            {
                                if( (YC_SP_HARMONIC_START < g_aYcDataConfigZhj[ArrIdx+i].nDealType) 
                                    && (YC_SP_HARMONIC_END > g_aYcDataConfigZhj[ArrIdx+i].nDealType) )
                                {
                                    if(1 == ReadState.nHarmonicYcFlag)
                                    {
                                        yc_db_nomal_read_zhj(m, x, n, i);//����г��
                                    }
                                }
                                else if( (YC_SP_DEMAND_START < g_aYcDataConfigZhj[ArrIdx+i].nDealType) 
                                            && (YC_SP_DEMAND_END > g_aYcDataConfigZhj[ArrIdx+i].nDealType) )
                                {
                                    if(1 == ReadState.nDemandYcFlag)
                                    {
                                        yc_db_nomal_read_zhj(m, x, n, i);//��������
                                    }
                                }
                                else
                                {
                                    if(1 == ReadState.nCommonYcFlag)
                                    {
                                        yc_db_nomal_read_zhj(m, x, n, i);//����ʵʱֵ
                                    }
                                }
                			}
                			else if(g_aYcDataConfigZhj[ArrIdx+i].nLogicId == 0 || 
                                g_aYcDataConfigZhj[ArrIdx+i].nLogicId == 7)//0 �������;7 �����豸
                			{
                			    if(g_aYcDataConfigZhj[ArrIdx+i].nOadColsNum > 0)//��OAD����Ҫ����0
                                {
                                    if(1 == ReadState.nRecordYcFlag)
                                    {
                                        yc_db_record_read_zhj(m, x, n, i);//��¼������
                                    }
                                }
                    	    }
                		}
                    }
            	}

                usleep(10000);//ÿ���豸��ȡ�������һ��
            }
        }
    }

    //�������ݶ�ȡ
    if(1 == ReadState.nRecordYcFlag)
    {
        yc_db_event_record_read_zhj(8, 0, 0, 0);//©���¼���ȡ
    }
    
}


