/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief： 功能描述
* @date： 2020-01-03
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

//#define GUO_BIAO  /* 电压、电流不平衡度国标算法 */

LOCK_QUEUE_T g_stSoeBuff;                        //遥信变位共享内存

time_t nPreDayTime_t;  //前一日23:59:59转换的秒数
time_t nPreMonTime_t;  //上一月最后一天23:59:59转换的秒数
time_t nTmnStartTime_t;  //终端启动时间

/*******************************************************************************
* 函数名称: his_frozen_file_read_zhj
* 函数功能: 私有文件 读取历史冻结数据
* 输入参数: 无
* 输出参数: 无
* 返 回 值: 无
*******************************************************************************/
int his_frozen_file_read_zhj(uint16 groupIndex, uint16 seqIndex, uint16 taskIndex, char *pData, uint32 nMaxLen)
{
    //私有文件路径
    char filename[100] = {0};//私有文件路径
    memset(filename, 0, sizeof(filename));
    sprintf(filename, "%s_%d_%d_%d", g_pFrozenValuePathZhj, groupIndex, seqIndex+1, taskIndex+1);

    //读出既有冻结数据
    int ret = 0;
    ret = read_pdata_xram(filename, pData, 0, nMaxLen);
    return ret;
}

/*******************************************************************************
* 函数名称: his_frozen_file_write_zhj
* 函数功能: 私有文件 写入历史冻结数据
* 输入参数: 无
* 输出参数: 无
* 返 回 值: 无
*******************************************************************************/
int his_frozen_file_write_zhj(uint16 groupIndex, uint16 seqIndex, uint16 taskIndex, char *pData, uint32 nMaxLen)
{
    //私有文件路径
    char filename[100] = {0};//私有文件路径
    memset(filename, 0, sizeof(filename));
    sprintf(filename, "%s_%d_%d_%d", g_pFrozenValuePathZhj, groupIndex, seqIndex+1, taskIndex+1);

    //读出既有冻结数据
    int ret = 0;
    ret = write_pdata_xram(filename, pData, 0, nMaxLen);
    return ret;
}

/*******************************************************************************
* 函数名称: fixed_area_iden_read_zhj
* 函数功能: 私有文件 读取定值参数标识
* 输入参数: 无
* 输出参数: 无
* 返 回 值: 无
*******************************************************************************/
int fixed_area_iden_read_zhj(uint16 nArea, char *pData, uint32 nMaxLen)
{
    //私有文件路径
    char filename[100] = {0};//私有文件路径
    memset(filename, 0, sizeof(filename));
    sprintf(filename, "%s_%d", g_pFixedAreaPathZhj, nArea);

    //读出既有冻结数据
    int ret = 0;
    ret = read_pdata_xram(filename, pData, 0, nMaxLen);
    return ret;
}

/*******************************************************************************
* 函数名称: fixed_area_iden_write_zhj
* 函数功能: 私有文件 读取定值参数标识
* 输入参数: 无
* 输出参数: 无
* 返 回 值: 无
*******************************************************************************/
int fixed_area_iden_write_zhj(uint16 nArea, char *pData, uint32 nMaxLen)
{
    //私有文件路径
    char filename[100] = {0};//私有文件路径
    memset(filename, 0, sizeof(filename));
    sprintf(filename, "%s_%d", g_pFixedAreaPathZhj, nArea);

    //读出既有冻结数据
    int ret = 0;
    ret = write_pdata_xram(filename, pData, 0, nMaxLen);
    return ret;
}

/*******************************************************************************
* 函数名称: fixed_para_value_read_zhj
* 函数功能: 私有文件 读取定值参数
* 输入参数: 无
* 输出参数: 无
* 返 回 值: 无
*******************************************************************************/
int fixed_para_value_read_zhj(uint16 nArea, char *pData, uint32 nMaxLen)
{
    //私有文件路径
    char filename[100] = {0};//私有文件路径
    memset(filename, 0, sizeof(filename));
    sprintf(filename, "%s_%d", g_pParamValuePathZhj, nArea);

    //读出既有冻结数据
    int ret = 0;
    ret = read_pdata_xram(filename, pData, 0, nMaxLen);
    return ret;
}

/*******************************************************************************
* 函数名称: fixed_para_value_write_zhj
* 函数功能: 私有文件 写入定值参数
* 输入参数: 无
* 输出参数: 无
* 返 回 值: 无
*******************************************************************************/
int fixed_para_value_write_zhj(uint16 nArea, char *pData, uint32 nMaxLen)
{
    //私有文件路径
    char filename[100] = {0};//私有文件路径
    memset(filename, 0, sizeof(filename));
    sprintf(filename, "%s_%d", g_pParamValuePathZhj, nArea);

    //读出既有冻结数据
    int ret = 0;
    ret = write_pdata_xram(filename, pData, 0, nMaxLen);
    return ret;
}

/*******************************************************************************
* 函数名称: config_xml_load
* 函数功能: 加载xml点表信息
* 输入参数: 无
* 输出参数: 无
* 返 回 值: 无
*******************************************************************************/
int config_xml_load_zhj()
{
    int i = 1;//设备类型 从1开始
    int z = 0;//组别 从0开始
    int j = 1;//组内偏移地址 从1开始

    for(i=0; i<MAX_EQUIP_TYPE_NUM; i++)
    {
        //判断是否支持的设备类型
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
                        //加载遥信点表
                        pw_readyxfromXML_zhj(i+1, z, j+1, &g_nConfigYxNumZhj[i][z][j]);
                        if(g_nConfigYxNumZhj[i][z][j] > MAX_YX_NUM)
                    	{
                    		FRAME104_FMT_DEBUG("if(g_nConfigYcNumZhj > MAX_YX_NUM) g_nConfigYcNumZhj[%d][%d][%d]=%d, MAX_YX_NUM=%d \n", i, z, j, g_nConfigYxNumZhj[i][z][j], MAX_YX_NUM);
                    		g_nConfigYxNumZhj[i][z][j] = MAX_YX_NUM;
                    	}

                        //加载遥测点表
                        pw_readycfromXML_zhj(i+1, z, j+1, &g_nConfigYcNumZhj[i][z][j]);
                        if(g_nConfigYcNumZhj[i][z][j] > MAX_YC_NUM)
                    	{
                    		FRAME104_FMT_DEBUG("if(g_nConfigYcNumZhj > MAX_YC_NUM) g_nConfigYcNumZhj[%d][%d][%d]=%d, MAX_YC_NUM=%d \n", i, z, j, g_nConfigYcNumZhj[i][z][j], MAX_YC_NUM);
                    		g_nConfigYcNumZhj[i][z][j] = MAX_YC_NUM;
                    	}

                        //加载定值参数点表
                        pw_readParafromXML_zhj(i+1, z, j+1, &g_nConfigParaNumZhj[i][z][j]);
                        if(g_nConfigParaNumZhj[i][z][j] > PARA_MAX_NUM)
                    	{
                    		FRAME104_FMT_DEBUG("if(g_nConfigParaNumZhj > PARA_MAX_NUM) g_nConfigParaNumZhj[%d][%d][%d]=%d, PARA_MAX_NUM=%d \n", i, z, j, g_nConfigParaNumZhj[i][z][j], PARA_MAX_NUM);
                    		g_nConfigParaNumZhj[i][z][j] = PARA_MAX_NUM;
                    	}

                        //加载故障信息点表
                        pw_readfaultfromXML_zhj(i+1, z, j+1, &g_nConfigFaultNumZhj[i][z][j]);
                        if(g_nConfigFaultNumZhj[i][z][j] > FAULT_MAX_NUM)
                    	{
                    		FRAME104_FMT_DEBUG("if(g_nConfigFaultNumZhj > FAULT_MAX_NUM) g_nConfigParaNumZhj[%d][%d][%d]=%d, FAULT_MAX_NUM=%d \n", i, z, j, g_nConfigParaNumZhj[i][z][j], FAULT_MAX_NUM);
                    		g_nConfigFaultNumZhj[i][z][j] = FAULT_MAX_NUM;
                    	}

                        FRAME104_FMT_DEBUG("******点表XML加载: 设备类型:%d, 设备组别:%d, 设备序号:%d, 遥信个数:%d, 遥测个数:%d, 参数个数:%d, 故障事件个数:%d \n", 
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
* 函数名称: task_run_step_init
* 函数功能: 104任务执行流程全局变量初始化
* 输入参数: 无
* 输出参数: 无
* 返 回 值: 无
*******************************************************************************/
void task_run_step_init_zhj()
{
    //104通用流程
    memset(&g_sTaskRunStep, 0x00, sizeof(g_sTaskRunStep));
	g_sTaskRunStep.nTaskStep = STEP_CONT_DEFAULT;
    g_sTaskRunStep.nStartNum = 0;
    g_sTaskRunStep.bFirstRun = 1;

    //104任务执行流程全局变量(周期冻结与上报专用)
    memset(&g_sTimingTaskRunStep, 0x00, sizeof(g_sTimingTaskRunStep));
	g_sTimingTaskRunStep.nTaskStep = STEP_TIMING_TASK_SEND;
    g_sTimingTaskRunStep.nDataCallStep = TIMING_STEP_SEND_YX;
    g_sTimingTaskRunStep.nStartNum = 0;
    g_sTimingTaskRunStep.bFirstRun = 1;
}

/*******************************************************************************
* 函数名称: param_data_init_zhj
* 函数功能: 定值参数初始化
* 输入参数: 无
* 输出参数: 无
* 返 回 值: 无
*******************************************************************************/
void param_data_init_zhj()
{
    int i = 0;//设备类型
	int j = 0;//设备组别
    int k = 0;//设备序号
    int m = 0;//定值区号
    uint32 n = 0;//定值索引
	int ret = 0;
    uint32 ArrIdx = 0;//一维数组索引(起始)

    //读取定值区特性
    for(m=0; m<MAX_AREA_NUM_ZHJ; m++)
    {
        ret = fixed_area_iden_read_zhj(m, (char *)&g_aFixedValueAreaZhj[m][0], sizeof(g_aFixedValueAreaZhj[m]));
        FRAME104_FMT_DEBUG("读取本地定值区特性 定值区号=%d, MaxLen=%u, 结果ret=%d \n", m, sizeof(g_aFixedValueAreaZhj[m]), ret);
    	if(ret < 0)
        {
    		FRAME104_FMT_DEBUG("读取本地定值区特性 失败! \n");
    	}
    }
    
    //打印定值区特性
    for(m=0; m<MAX_AREA_NUM_ZHJ; m++)
    {
        for(i=0; i<MAX_EQUIP_TYPE_NUM; i++)
        {
            for(j=0; j<g_aGroupCountZhj[i]; j++)
            {
                for(k=0; k<g_aEquipCountZhj[i][j];k++)
                {
                    ArrIdx = g_aFixedAreaIndexZhj[m][i][j][k];
                    
                    FRAME104_FMT_DEBUG("打印本地定值区特性(循环变量) 设备类型=%d, 组别=%d, 序号=%d, 定值区号=%d \n", i+1, j, k+1, m);
                    FRAME104_FMT_DEBUG("打印本地定值区特性(实际参数) 设备类型=%d, 组别=%d, 序号=%d, 一维索引=%u \n", 
                        g_aFixedValueAreaZhj[m][ArrIdx].nEqType, g_aFixedValueAreaZhj[m][ArrIdx].nEqGroup, g_aFixedValueAreaZhj[m][ArrIdx].nEquipNo, 
                        ArrIdx);
                    FRAME104_FMT_DEBUG("定值区号=%d, 特征标识=0x%02x(后续状态位=%d, CR未用/取消预置=%d, S/E固化/预置=%d), 私有S/E=%d \n", 
                        g_aFixedValueAreaZhj[m][ArrIdx].nFixedAreaID, g_aFixedValueAreaZhj[m][ArrIdx].unParaIdentify.nParaIden, 
                        g_aFixedValueAreaZhj[m][ArrIdx].unParaIdentify.unParaIden.cont, g_aFixedValueAreaZhj[m][ArrIdx].unParaIdentify.unParaIden.cr, 
                        g_aFixedValueAreaZhj[m][ArrIdx].unParaIdentify.unParaIden.se, g_aFixedValueAreaZhj[m][ArrIdx].nSetState);
                }
            }
        }
    }

    //读取定值参数
    for(m=0; m<MAX_AREA_NUM_ZHJ; m++)
    {
        ret = fixed_para_value_read_zhj(m, (char *)&g_aParaSetValueZhj[m][0], sizeof(g_aParaSetValueZhj[m]));
        FRAME104_FMT_DEBUG("读取本地定值参数 定值区号=%d, MaxLen=%u, 结果ret=%d \n", m, sizeof(g_aParaSetValueZhj[m]), ret);
    	if(ret < 0)
        {
    		FRAME104_FMT_DEBUG("读取本地定值参数...失败! \n");
            FRAME104_FMT_DEBUG("加载默认定值参数... \n");
            
            memcpy(&g_aParaSetValueZhj[m][0], &g_aParaValueInitZhj[0], sizeof(g_aParaSetValueZhj[m]));
    	}
        else
        {
            FRAME104_FMT_DEBUG("读取本地定值参数...成功! \n");
            //判断是否需要将点表的相关参数同步至全局变量(主要用于测试维护)
            for(n=0; n<g_nTotalParaNumZhj; n++)
            {
                if( (1 == g_aParaValueInitZhj[n].nUpdateFlag) 
                    && (m == g_aParaValueInitZhj[n].nUpdateArea) )
                {
                    FRAME104_FMT_DEBUG("覆盖! 设备类型=%d, 组别=%d, 序号=%d, 定值区号=%d, 一维数组下标=%d \n", 
                        g_aParaSetValueZhj[m][n].nEqType, g_aParaSetValueZhj[m][n].nEqGroup, g_aParaSetValueZhj[m][n].nEquipNo, m, n);
                    memcpy(&g_aParaSetValueZhj[m][n], &g_aParaValueInitZhj[n], sizeof(PARA_SET_VALUE_T));
                }
            }
        }
    }

	//定值参数初始化打印信息
	for(i=0; i<MAX_EQUIP_TYPE_NUM; i++)
    {
        for(j=0; j<g_aGroupCountZhj[i]; j++)
        {
            for(k=0; k<g_aEquipCountZhj[i][j];k++)
            {
                for(m=0; m<g_aFixedAreaNumZhj[i][j][k]; m++)
                {
                    ArrIdx = g_aParaValueIndexZhj[i][j][k];
                    FRAME104_FMT_DEBUG("打印定值参数(循环变量) 设备类型=%d, 组别=%d, 序号=%d, 定值区号=%d \n", i+1, j, k+1, m);
                	
                	for(n=0; n<g_nConfigParaNumZhj[i][j][k]; n++)
                    {
                        FRAME104_FMT_DEBUG("打印定值参数(实际参数) 设备类型=%d, 组别=%d, 序号=%d, 起始索引=%u, 实际索引=%u \n", 
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
* 函数名称: global_init
* 函数功能: 进程初始化总函数
* 输入参数: 无
* 输出参数: 无
* 返 回 值: 无
*******************************************************************************/
int global_init_zhj()
{
    int i = 0;//设备类型
	int j = 0;//设备组别
    int k = 0;//设备序号
    int m = 0;//定值区号
    
    //配置表初始化
    memset(g_aYxDataConfigZhj, 0x00, sizeof(g_aYxDataConfigZhj));
    memset(g_aYcDataConfigZhj, 0x00, sizeof(g_aYcDataConfigZhj));
    memset(g_aParaValueInitZhj, 0x00, sizeof(g_aParaValueInitZhj));
    memset(g_aFaultDataConfigZhj, 0x00, sizeof(g_aFaultDataConfigZhj));

    //上报表初始化
    memset(g_aYxDataSendZhj, 0x00, sizeof(g_aYxDataSendZhj));
    memset(g_aYcDataSendZhj, 0x00, sizeof(g_aYcDataSendZhj));

    //协议中使用的变量初始化
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

    //定值相关参数初始化
    memset(g_aParaSetValueZhj, 0x00, sizeof(g_aParaSetValueZhj));
    memset(g_nFixedAreaIdZhj, 0x00, sizeof(g_nFixedAreaIdZhj));
    memset(g_bFixedChangeFlagZhj, 0x00, sizeof(g_bFixedChangeFlagZhj));

    memset(g_aFixedAreaNumZhj, 0x00, sizeof(g_aFixedAreaNumZhj));
    memset(g_aFixedValueAreaZhj, 0x00, sizeof(g_aFixedValueAreaZhj));
    memset(g_aFixedValueStateZhj, 0x00, sizeof(g_aFixedValueStateZhj));
    memset(g_nParaFixedFlagZhj, 0x00, sizeof(g_nParaFixedFlagZhj));

    //定值区数量及默认定值区初始化
    for(i=0; i<MAX_EQUIP_TYPE_NUM; i++)
    {
        for(j=0; j<g_aGroupCountZhj[i]; j++)
        {
            for(k=0; k<g_aEquipCountZhj[i][j];k++)
            {
                g_nFixedAreaIdZhj[i][j][k] = 0;//默认采用0号定值区 从0开始
                g_aFixedAreaNumZhj[i][j][k] = MAX_FIXED_AREA_NUM;
            }
        }
    }

    //定值区特征标识初始化
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
                    
                    g_aFixedValueAreaZhj[m][IdenIndex].nFixedAreaID = m;//定值区号 从0开始
                    g_aFixedValueAreaZhj[m][IdenIndex].nSetState = 0xaa;
                    
                    g_aFixedAreaIndexZhj[m][i][j][k] = IdenIndex;
                    IdenIndex += 1;
                }
            }
        }
    }

    time(&nTmnStartTime_t);  //获取time_t类型的当前时间
    past_time_init();  //时间管理初始化
    past_time_init_down();  //时间管理初始化
	task_run_step_init_zhj();  //104任务执行流程全局变量初始化
	//scheme_coload_init_zhj();  //普通采集方案记录列选择初始化

    //计算上日/上月末时间
    nPreDayTime_t = nTmnStartTime_t - (g_stDownCurBinTime.Hour*60*60 + g_stDownCurBinTime.Minute*60 + g_stDownCurBinTime.Second + 10);  //前一日23:59:50转换的秒数
    nPreMonTime_t = nTmnStartTime_t - ((g_stDownCurBinTime.Day - 1)*24*60*60 + g_stDownCurBinTime.Hour*60*60 + g_stDownCurBinTime.Minute*60 + g_stDownCurBinTime.Second + 10);  //上一月最后一天23:59:50转换的秒数
	
	return 1;
}

/*******************************************************************************
* 函数名称: para_query_int64_zhj
* 函数功能: 整型定值参数查询
* 输入参数: typeIndex         设备类型数组下标, 从0开始
            groupIndex    设备组别数组下标, 从0开始
            seqIndex      组内偏移地址数组下标, 从0开始
            nParaAddr     参数点表中四字节地址
* 输出参数: nOut              整型定值参数
* 返 回 值: 1：成功；-1：失败
*******************************************************************************/
int para_query_int64_zhj(uint16 typeIndex, uint16 groupIndex, uint16 seqIndex, uint32 nParaAddr, int64 *nOut)
{
	int i = 0;
	int64   nParaValue = 0;
	BOOL    bSuccess   = FALSE;

    int m = typeIndex;//设备类型数组下标
    int x = groupIndex;//设备组别数组下标
    int n=  seqIndex;//设备序号下标
    uint16 nAreaId = g_nFixedAreaIdZhj[m][x][n];//定值区号

    //判断设备类型是否合法
    if(typeIndex >= MAX_EQUIP_TYPE_NUM)
    {
        return -1;
    }

    //判断设备组别是否合法
    if(groupIndex >= MAX_EQUIP_GROUP_NUM)
    {
        return -1;
    }

    //判断设备序号是否合法
    if(seqIndex >= MAX_EQUIP_COUNT_NUM)
    {
        return -1;
    }

    uint32 ArrIdx = 0;//一维数组索引(起始)
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
			DB104_BUF_LOGCC(&g_aParaSetValueZhj[nAreaId][ArrIdx+i].unValue.arrayV[0], MAX_FIXED_VALUE_LEN, "报文: ");
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
* 函数名称: para_query_float32_zhj
* 函数功能: 浮点型定值参数查询
* 输入参数: typeIndex         设备类型数组下标, 从0开始
            groupIndex    设备组别数组下标, 从0开始
            seqIndex      组内偏移地址数组下标, 从0开始
            nParaAddr     参数点表中四字节地址
* 输出参数: fOut              浮点型定值参数
* 返 回 值: 1：成功；-1：失败
*******************************************************************************/
int para_query_float32_zhj(uint16 typeIndex, uint16 groupIndex, uint16 seqIndex, uint32 nParaAddr, float32 *fOut)
{
	int i = 0;
	float32 fParaValue = 0.0;
	BOOL    bSuccess   = FALSE;

    int m = typeIndex;//设备类型数组下标
    int x = groupIndex;//设备组别数组下标
    int n=  seqIndex;//设备序号下标
    uint16 nAreaId = g_nFixedAreaIdZhj[m][x][n];//定值区号

    //判断设备类型是否合法
    if(typeIndex >= MAX_EQUIP_TYPE_NUM)
    {
        return -1;
    }

    //判断设备组别是否合法
    if(groupIndex >= MAX_EQUIP_GROUP_NUM)
    {
        return -1;
    }

    //判断设备序号是否合法
    if(seqIndex >= MAX_EQUIP_COUNT_NUM)
    {
        return -1;
    }

    uint32 ArrIdx = 0;//一维数组索引(起始)
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
			DB104_BUF_LOGCC(&g_aParaSetValueZhj[nAreaId][ArrIdx+i].unValue.arrayV[0], MAX_FIXED_VALUE_LEN, "报文: ");
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
* 函数名称: para_query_array_zhj
* 函数功能: 字符串型定值参数查询
* 输入参数: typeIndex         设备类型数组下标, 从0开始
            groupIndex    设备组别数组下标, 从0开始
            seqIndex      组内偏移地址数组下标, 从0开始
            nParaAddr     参数点表中四字节地址
* 输出参数: pOut              字符串定值参数
* 返 回 值: 大于0：字符串长度；-1：失败
*******************************************************************************/
int para_query_array_zhj(uint16 typeIndex, uint16 groupIndex, uint16 seqIndex, uint32 nParaAddr, uint8 *pOut)
{
	int i = 0;
    int nArrLen = -1;

    int m = typeIndex;//设备类型数组下标
    int x = groupIndex;//设备组别数组下标
    int n=  seqIndex;//设备序号下标
    uint16 nAreaId = g_nFixedAreaIdZhj[m][x][n];//定值区号

    //判断设备类型是否合法
    if(typeIndex >= MAX_EQUIP_TYPE_NUM)
    {
        return -1;
    }

    //判断设备组别是否合法
    if(groupIndex >= MAX_EQUIP_GROUP_NUM)
    {
        return -1;
    }

    //判断设备序号是否合法
    if(seqIndex >= MAX_EQUIP_COUNT_NUM)
    {
        return -1;
    }

    uint32 ArrIdx = 0;//一维数组索引(起始)
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
                DB104_BUF_LOGCC(&g_aParaSetValueZhj[nAreaId][ArrIdx+i].aArrValue, nArrLen, "报文: ");
            }
			break;
		}
	}

	return nArrLen;
}

/*******************************************************************************
* 函数名称: yx_soe_buff_combine_zhj
* 函数功能: 组装遥信变位SOE报文
* 输入参数: 无
* 输出参数: 无
* 返 回 值: 无
*******************************************************************************/
int yx_soe_buff_combine_zhj(uint8 eqTpIndex, uint8 eqGroupIndex, uint16 eqNoIndex, uint16 addrIndex, TOTAL_104_FRAME_T *pTotal104Frame)
{
    uint8 addr[4] = {0};//信息体地址
    int index = 0;//偏移位
    uint32 ArrIdx = 0;//一维数组索引(起始)
    
    TOTAL_104_FRAME_T stTotal104Frame;
    memset(&stTotal104Frame, 0, sizeof(TOTAL_104_FRAME_T));

    //HEAD
    stTotal104Frame.nHead = 0x68;

    //控制欲(暂时不填)
    
    //数据类型
    stTotal104Frame.nDataType = M_SP_TB_1;

    //VSQ
    stTotal104Frame.nVSQ = 0x01;

    //COT
    stTotal104Frame.nTransCauseL = CAUSE_SPONT;
    stTotal104Frame.nTransCauseH = 0x00;

    //公共地址
    stTotal104Frame.nPubAddrL = eqNoIndex + 1;
    stTotal104Frame.nPubAddrH = (uint8)((uint8)(eqTpIndex+1) << 3) + eqGroupIndex;

    //信息体地址
    ArrIdx = g_aYxDataIndexZhj[eqTpIndex][eqGroupIndex][eqNoIndex];
	memcpy(addr, &g_aYxDataConfigZhj[ArrIdx+addrIndex].nObjAddrU32, 4);
	stTotal104Frame.aObjData[index++] = addr[0];
	stTotal104Frame.aObjData[index++] = addr[1];
	stTotal104Frame.aObjData[index++] = addr[2];
	//单点遥信数值
	stTotal104Frame.aObjData[index++] = g_aYxDataSendZhj[ArrIdx+addrIndex].nNewValue;
	//带时标
	memcpy(&stTotal104Frame.aObjData[index], &g_aYxDataSendZhj[ArrIdx+addrIndex].btime[0], 7);
	index += 7;

    //更新报文长度(不包括头部和长度自身)
    stTotal104Frame.nLength = CTRL_UNIT_LABEL_LEN + index;

    memcpy(pTotal104Frame, &stTotal104Frame, sizeof(TOTAL_104_FRAME_T));
    return 1;
}

/*******************************************************************************
* 函数名称: yx_cos_buff_combine_zhj
* 函数功能: 组装遥信变位cos报文
* 输入参数: 无
* 输出参数: 无
* 返 回 值: 无
*******************************************************************************/
int yx_cos_buff_combine_zhj(uint8 eqTpIndex, uint8 eqGroupIndex, uint16 eqNoIndex, uint16 addrIndex, TOTAL_104_FRAME_T *pTotal104Frame)
{
    uint8 addr[4] = {0};//信息体地址
    int index = 0;//偏移位
    uint32 ArrIdx = 0;//一维数组索引(起始)
    
    TOTAL_104_FRAME_T stTotal104Frame;
    memset(&stTotal104Frame, 0, sizeof(TOTAL_104_FRAME_T));

    //HEAD
    stTotal104Frame.nHead = 0x68;

    //控制欲(暂时不填)
    
    //数据类型
    stTotal104Frame.nDataType = M_SP_NA_1;

    //VSQ
    stTotal104Frame.nVSQ = 0x01;

    //COT
    stTotal104Frame.nTransCauseL = CAUSE_SPONT;
    stTotal104Frame.nTransCauseH = 0x00;

    //公共地址
    stTotal104Frame.nPubAddrL = eqNoIndex + 1;
    stTotal104Frame.nPubAddrH = (uint8)((uint8)(eqTpIndex+1) << 3) + eqGroupIndex;

    //信息体地址
	ArrIdx = g_aYxDataIndexZhj[eqTpIndex][eqGroupIndex][eqNoIndex];
	memcpy(addr, &g_aYxDataConfigZhj[ArrIdx+addrIndex].nObjAddrU32, 4);
	stTotal104Frame.aObjData[index++] = addr[0];
	stTotal104Frame.aObjData[index++] = addr[1];
	stTotal104Frame.aObjData[index++] = addr[2];
	//单点遥信数值
	stTotal104Frame.aObjData[index++] = g_aYxDataSendZhj[ArrIdx+addrIndex].nNewValue;

    //更新报文长度(不包括头部和长度自身)
    stTotal104Frame.nLength = CTRL_UNIT_LABEL_LEN + index;

    memcpy(pTotal104Frame, &stTotal104Frame, sizeof(TOTAL_104_FRAME_T));
    return 1;
}


/*******************************************************************************
* 函数名称: yx_real_signal_update_zhj
* 函数功能: 通过mqtt消息获取硬遥信信号
* 输入参数: 无
* 输出参数: 无
* 返 回 值: 无
*******************************************************************************/
void yx_real_signal_update_zhj(RST_RMT_T stRmt, uint8 status)
{
    int m = 0;//设备类型
    int x = 0;//设备组别
    int n = 0;//设备序号
	int i = 0;
    uint16 index = 0;
    uint32 ArrIdx = 0;//一维数组索引(起始)
    
    TOTAL_104_FRAME_T stTotal104Frame;
    static TOTAL_104_FRAME_T tmpTotal104Frame;
    static int count = 0;
    
    memset(&stTotal104Frame, 0, sizeof(TOTAL_104_FRAME_T));
    
    for(m=0; m<MAX_EQUIP_TYPE_NUM; m++)//类型
    {
        if(0 == g_aGroupCountZhj[m])
        {
            continue;//该类型的组数量为0
        }
        
        for(x=0; x<g_aGroupCountZhj[m]; x++)//组号
        {
            if(0 == g_aEquipCountZhj[m][x])
            {
                continue;//该类型的设备数量为0
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
                            
            			    //数据更新与检测
            			    system_time_get(&g_aYxDataSendZhj[ArrIdx+index].btime[0]);//变位时间记录
                        	g_aYxDataSendZhj[ArrIdx+index].nNewValue = (uint8)( (stRmt.ST >> g_aYxDataConfigZhj[ArrIdx+index].nItemIndex)&0x01 );
                            if(g_aYxDataSendZhj[ArrIdx+index].nNewValue)//硬遥信 1表示开 0表示闭
                            {
                                g_aYxDataSendZhj[ArrIdx+index].nNewValue = 0;
                            }
                            else
                            {
                                g_aYxDataSendZhj[ArrIdx+index].nNewValue = 1;
                            }
                            DB104_FMT_TRACE("硬遥信 m=%d, x=%d, n=%d, g_aYxDataSend[%d]: nNewValue=%d, nHisValue=%d, stRmt.ST=0x%02x, status=0x%02x \n", 
                            		m, x, n, index, g_aYxDataSendZhj[ArrIdx+index].nNewValue, g_aYxDataSendZhj[ArrIdx+index].nHisValue, stRmt.ST, status);
                            
                        	if(g_aYxDataSendZhj[ArrIdx+index].nNewValue != g_aYxDataSendZhj[ArrIdx+index].nHisValue)
                        	{
                        	    DB104_FMT_TRACE("遥信变位, 产生SOE与COS! \n");

                                count++;
                                if (count == 1)
                                {
                                    memset(&stTotal104Frame, 0, sizeof(TOTAL_104_FRAME_T));
                                    yx_soe_buff_combine_zhj(m, x, n, index, &stTotal104Frame);
                                    memcpy(&tmpTotal104Frame, &stTotal104Frame, sizeof(TOTAL_104_FRAME_T));
                                    DB104_BUF_TRACE((uint8 *)&stTotal104Frame, stTotal104Frame.nLength+2, "临时SOE报文:");
                                }

                                memset(&stTotal104Frame, 0, sizeof(TOTAL_104_FRAME_T));
                                yx_cos_buff_combine_zhj(m, x, n, index, &stTotal104Frame);
                                pw_lock_queue_write(&g_stSoeBuff, (uint8 *)&stTotal104Frame, stTotal104Frame.nLength+2, 0);
                                DB104_BUF_TRACE((uint8 *)&stTotal104Frame, stTotal104Frame.nLength+2, "COS报文:");
                                
                                if (count >= 2) /* SOE报文携带发生和恢复的信息 */
                                {
                                    memset(&stTotal104Frame, 0, sizeof(TOTAL_104_FRAME_T));
                                    yx_soe_buff_combine_zhj(m, x, n, index, &stTotal104Frame);
                                    stTotal104Frame.nVSQ = 0x02;
                                    memcpy(&stTotal104Frame.aObjData[11], stTotal104Frame.aObjData, 11);
                                    memcpy(stTotal104Frame.aObjData, tmpTotal104Frame.aObjData, 11);
                                    stTotal104Frame.nLength += 11; 
                                    pw_lock_queue_write(&g_stSoeBuff, (uint8 *)&stTotal104Frame, stTotal104Frame.nLength+2, 0);
                                    DB104_BUF_TRACE((uint8 *)&stTotal104Frame, stTotal104Frame.nLength+2, "SOE报文:");
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
* 函数名称: yx_event_signal_update_zhj
* 函数功能: 根据事件更新相关的遥信状态
* 输入参数: typeIndex         设备类型数组下标, 从0开始
            groupIndex    设备组别数组下标, 从0开始
            seqIndex      组内偏移地址数组下标, 从0开始
            nAddr         遥信点表中四字节地址
            nNewState     事件状态 1发生 0恢复
* 输出参数: 无
* 返 回 值: 1：成功；-1：失败
*******************************************************************************/
int yx_event_signal_update_zhj(uint16 typeIndex, uint16 groupIndex, uint16 seqIndex, uint32 nAddr, uint8 nNewState)
{
	int m = typeIndex;//设备类型-1
    int x = groupIndex;//设备组别 从0开始
    int n=  seqIndex;//设备序号-1
    int index = -1;//位于配置点表中的序号
    uint32 ArrIdx = 0;//一维数组索引(起始)
    
    TOTAL_104_FRAME_T stTotal104Frame;
    memset(&stTotal104Frame, 0, sizeof(TOTAL_104_FRAME_T));
    
    index = query_yx_index_config_zhj(typeIndex, groupIndex, seqIndex, nAddr);
    if(index < 0)
    {
        DB104_FMT_TRACE("事件遥信 typeIndex=%d, groupIndex=%d, seqIndex=%d, nAddr=%08x, nNewState=%d 错误!\n", 
		    m, x, n, nAddr, nNewState);
        return -1;
    }

    ArrIdx = g_aYxDataIndexZhj[m][x][n];
    DB104_FMT_TRACE("事件遥信 typeIndex=%d, groupIndex=%d, seqIndex=%d, nAddr=%08x, index=%d, nNewState=%d \n", 
		    typeIndex, groupIndex, seqIndex, nAddr, index, nNewState);

    //数据更新与检测
    system_time_get(&g_aYxDataSendZhj[ArrIdx+index].btime[0]);
	g_aYxDataSendZhj[ArrIdx+index].nNewValue = nNewState>0? 1: 0;//1发生 0恢复
	DB104_FMT_TRACE("事件遥信 m=%d, x=%d, n=%d, g_aYxDataSend[%d]: nNewValue=%d, nHisValue=%d \n", 
		m, x, n, index, g_aYxDataSendZhj[ArrIdx+index].nNewValue, g_aYxDataSendZhj[ArrIdx+index].nHisValue);

    if((g_aYxDataSendZhj[ArrIdx+index].nNewValue != g_aYxDataSendZhj[ArrIdx+index].nHisValue) || 
        (nAddr == 0x00100005 && nNewState == 0)) // 针对终端上电特殊处理
	{
		DB104_FMT_TRACE("遥信变位, 产生SOE与COS! \n");

        memset(&stTotal104Frame, 0, sizeof(TOTAL_104_FRAME_T));
        yx_soe_buff_combine_zhj(m, x, n, index, &stTotal104Frame);
        pw_lock_queue_write(&g_stSoeBuff, (uint8 *)&stTotal104Frame, stTotal104Frame.nLength+2, 0);
        DB104_BUF_TRACE((uint8 *)&stTotal104Frame, stTotal104Frame.nLength+2, "临时SOE报文:");

        memset(&stTotal104Frame, 0, sizeof(TOTAL_104_FRAME_T));
        yx_cos_buff_combine_zhj(m, x, n, index, &stTotal104Frame);
        pw_lock_queue_write(&g_stSoeBuff, (uint8 *)&stTotal104Frame, stTotal104Frame.nLength+2, 0);
        DB104_BUF_TRACE((uint8 *)&stTotal104Frame, stTotal104Frame.nLength+2, "临时COS报文:");

        g_aYxDataSendZhj[ArrIdx+index].nHisValue = g_aYxDataSendZhj[ArrIdx+index].nNewValue;
	}
	
	return 1;
}

/*******************************************************************************
* 函数名称: yx_db_center_read_zhj
* 函数功能: 从数据中心读取遥信信息
* 输入参数: 无
* 输出参数: 无
* 返 回 值: 无
*******************************************************************************/
void yx_db_center_read_zhj(uint8 typeIndex, uint16 groupIndex, uint16 seqIndex, uint16 index)
{

}

/*******************************************************************************
* 函数名称: yx_real_signal_read_zhj
* 函数功能: 通过mqtt消息获取硬遥信信号
* 输入参数: 无
* 输出参数: 无
* 返 回 值: 无
*******************************************************************************/
void yx_real_signal_read_zhj(uint8 typeIndex, uint16 groupIndex, uint16 seqIndex, uint16 index)
{
    int m = typeIndex;//设备类型-1
    int x = groupIndex;//设备组别 从0开始
    int n=  seqIndex;//设备序号-1
    uint32 ArrIdx = 0;//一维数组索引(起始)
    
	if(index >= g_nConfigYxNumZhj[m][x][n])
	{
		return;
	}

	RST_RMT_T stRmt;
	uint8 status = 0;

	memset(&stRmt, 0x00, sizeof(RST_RMT_T));
	status = advPd_get_rmt(&stRmt);

	//数据更新与检测
	ArrIdx = g_aYxDataIndexZhj[m][x][n];
	g_aYxDataSendZhj[ArrIdx+index].nNewValue = (uint8)( (stRmt.ST >> g_aYxDataConfigZhj[ArrIdx+index].nItemIndex)&0x01 );
	if(g_aYxDataSendZhj[ArrIdx+index].nNewValue != g_aYxDataSendZhj[ArrIdx+index].nHisValue)
	{
		g_aYxDataSendZhj[ArrIdx+index].bChangeFlag = 1;
	}
	
	//临时赋值, 测试变化上送
	if(g_bTestEnable)
	{
		g_aYxDataSendZhj[ArrIdx+index].nNewValue = rand()%2;//对2取余得到就是0或者1
		g_aYxDataSendZhj[ArrIdx+index].bChangeFlag = 1;
		DB104_FMT_TRACE("遥信随机数 g_aYxDataSend[%d].nNewValue=%d \n", index, g_aYxDataSendZhj[ArrIdx+index].nNewValue);
	}
	
	if(g_aYxDataSendZhj[ArrIdx+index].bChangeFlag)
	{
		g_bYxChanged = TRUE;
		g_aYxDataSendZhj[ArrIdx+index].bSendFlag = FALSE;
        g_aChangedEquipStateYX[m][x][n] = 1;
	}

    DB104_FMT_TRACE("硬遥信 m=%d, x=%d, n=%d, g_aYxDataSend[%d]: nNewValue=%d, nHisValue=%d, bChangeFlag=%d, bSendFlag=%d, 全局g_bYxChanged=%d, status=0x%02x \n", 
    		m, x, n, index, g_aYxDataSendZhj[ArrIdx+index].nNewValue, g_aYxDataSendZhj[ArrIdx+index].nHisValue, g_aYxDataSendZhj[ArrIdx+index].bChangeFlag, 
    		g_aYxDataSendZhj[ArrIdx+index].bSendFlag, g_bYxChanged, status);

	system_time_get(&g_aYxDataSendZhj[ArrIdx+index].btime[0]);
}

/*******************************************************************************
* 函数名称: yx_calculate_statistics
* 函数功能: 需要统计或计算的遥信信号
* 输入参数: 无
* 输出参数: 无
* 返 回 值: 无
*******************************************************************************/
void yx_calculate_statistics_zhj(uint8 typeIndex, uint16 groupIndex, uint16 seqIndex, uint16 index)
{
	int m = typeIndex;//设备类型-1
    int x = groupIndex;//设备组别 从0开始
    int n=  seqIndex;//设备序号-1
    uint8 nNewState = 0;
    uint32 ArrIdx = 0;//一维数组索引(起始)
    
	if(index >= g_nConfigYxNumZhj[m][x][n])
	{
		return;
	}

    //数据更新与检测
    ArrIdx = g_aYxDataIndexZhj[m][x][n];
    switch(g_aYxDataConfigZhj[ArrIdx+index].nDealType)
    {
        case 0x0001000B://视在功率越限
        {
            nNewState = 0;
        }
            break;
        
        default://其他类型遥信暂不作处理
            return;
            break;
    }   

	//数据更新与检测
	g_aYxDataSendZhj[ArrIdx+index].nNewValue = nNewState;
	if(g_aYxDataSendZhj[ArrIdx+index].nNewValue != g_aYxDataSendZhj[ArrIdx+index].nHisValue)
	{
		g_aYxDataSendZhj[ArrIdx+index].bChangeFlag = 1;
	}
	
	//临时赋值, 测试变化上送
	if(g_bTestEnable)
	{
		g_aYxDataSendZhj[ArrIdx+index].nNewValue = rand()%2;//对2取余得到就是0或者1
		g_aYxDataSendZhj[ArrIdx+index].bChangeFlag = 1;
		DB104_FMT_TRACE("遥信随机数 g_aYxDataSend[%d].nNewValue=%d \n", index, g_aYxDataSendZhj[ArrIdx+index].nNewValue);
	}
	
	if(g_aYxDataSendZhj[ArrIdx+index].bChangeFlag)
	{
		g_bYxChanged = TRUE;
		g_aYxDataSendZhj[ArrIdx+index].bSendFlag = FALSE;
        g_aChangedEquipStateYX[m][x][n] = 1;
	}

	DB104_FMT_TRACE("计算遥信 m=%d, x=%d, n=%d, g_aYxDataSend[%d]: nNewValue=%d, nHisValue=%d, bChangeFlag=%d, bSendFlag=%d, 全局g_bYxChanged=%d, nNewState=0x%02x \n", 
		m, x, n, index, g_aYxDataSendZhj[ArrIdx+index].nNewValue, g_aYxDataSendZhj[ArrIdx+index].nHisValue, g_aYxDataSendZhj[ArrIdx+index].bChangeFlag, 
		g_aYxDataSendZhj[ArrIdx+index].bSendFlag, g_bYxChanged, nNewState);

	system_time_get(&g_aYxDataSendZhj[ArrIdx+index].btime[0]);
}

/*******************************************************************************
* 函数名称: yx_event_occur_set_zhj
* 函数功能: 根据事件判断生成的遥信变位
* 输入参数: 无
* 输出参数: 无
* 返 回 值: 无
*******************************************************************************/
int yx_event_occur_set_zhj(uint16 typeIndex, uint16 groupIndex, uint16 seqIndex,uint32 nAddr, uint8 nNewState)
{
	int m = typeIndex;//设备类型-1
    int x = groupIndex;//设备组别 从0开始
    int n=  seqIndex;//设备序号-1
    int index = -1;//位于配置点表中的序号
	uint32 ArrIdx = 0;//一维数组索引(起始)
    
    index = query_yx_index_config_zhj(typeIndex, groupIndex, seqIndex, nAddr);
    if(index < 0)
    {
        DB104_FMT_TRACE("事件遥信 m=%d, x=%d, n=%d, nAddr=%08x, nNewState=%d \n", 
		m, x, n, nAddr, nNewState);
        return -1;
    }

	ArrIdx = g_aYxDataIndexZhj[m][x][n];
    //数据更新与检测
	g_aYxDataSendZhj[ArrIdx+index].nNewValue = nNewState>0? 1: 0;
	if(g_aYxDataSendZhj[ArrIdx+index].nNewValue != g_aYxDataSendZhj[ArrIdx+index].nHisValue)
	{
		g_aYxDataSendZhj[ArrIdx+index].bChangeFlag = 1;
	}
	
	//临时赋值, 测试变化上送
	if(g_bTestEnable)
	{
		g_aYxDataSendZhj[ArrIdx+index].nNewValue = rand()%2;//对2取余得到就是0或者1
		g_aYxDataSendZhj[ArrIdx+index].bChangeFlag = 1;
		DB104_FMT_TRACE("遥信随机数 g_aYxDataSend[%d].nNewValue=%d \n", index, g_aYxDataSendZhj[ArrIdx+index].nNewValue);
	}
	
	if(g_aYxDataSendZhj[ArrIdx+index].bChangeFlag)
	{
		g_bYxChanged = TRUE;
		g_aYxDataSendZhj[ArrIdx+index].bSendFlag = FALSE;
        g_aChangedEquipStateYX[m][x][n] = 1;
	}

	DB104_FMT_TRACE("事件遥信 m=%d, x=%d, n=%d, g_aYxDataSend[%d]: nNewValue=%d, nHisValue=%d, bChangeFlag=%d, bSendFlag=%d, 全局g_bYxChanged=%d, nNewState=0x%02x \n", 
		m, x, n, index, g_aYxDataSendZhj[ArrIdx+index].nNewValue, g_aYxDataSendZhj[ArrIdx+index].nHisValue, g_aYxDataSendZhj[ArrIdx+index].bChangeFlag, 
		g_aYxDataSendZhj[ArrIdx+index].bSendFlag, g_bYxChanged, nNewState);

	system_time_get(&g_aYxDataSendZhj[ArrIdx+index].btime[0]);
    return 1;
}

/*******************************************************************************
* 函数名称: data_block_split_zhj
* 函数功能: 数据块拆分
* 输入参数: 无
* 输出参数: 无
* 返 回 值: 无
*******************************************************************************/
int data_block_split_zhj(uint32 nOAD, uint8 nItemID, uint8 *buff, float32 *fOut, uint8 *timeFlag, OOP_DATETIME_S_T *pTime)
{
	uint16 nNum = 0;

	switch(nOAD)
	{
		case 0x00100200:			//(当前) 正向有功电能总尖峰平谷示值
        case 0x00200200:			//(当前) 反向有功电能总尖峰平谷示值
        case 0x00300200:			//(当前) 正向无功电能总尖峰平谷示值
        case 0x00400200:			//(当前) 反向无功电能总尖峰平谷示值
        case 0x00500200:			//(当前) 第一象限无功总尖峰平谷电能示值
        case 0x00600200:			//(当前) 第二象限无功总尖峰平谷电能示值
        case 0x00700200:			//(当前) 第三象限无功总尖峰平谷电能示值
        case 0x00800200:			//(当前) 第四象限无功总尖峰平谷电能示值
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
		case 0x20000200:			//(当前)三相电压
		case 0x260B0200:			//电容器容量
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
		case 0x20010200:			//(当前)三相电流
		case 0x260A0200:			//电容器分补电流
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
        case 0x20000400:			   //(当前)零序电压
        {
			uint16 Voltage;
			memset((uint8*)&Voltage,0x00,sizeof(uint16));
            memcpy((uint8*)&Voltage,buff,sizeof(uint16));
			*fOut = (float32)Voltage;
		}
            break;
        case 0x20010600:				//(当前)零序电流
        {
			int32 Current;
			memset((uint8*)&Current,0x00,sizeof(int32));
            memcpy((uint8*)&Current,buff,sizeof(int32));
			*fOut = (float32)Current;
		}
            break;
		case 0x20040200:			//(当前)总有功功率数据项
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
		case 0x20050200:			//(当前)总无功功率数据项
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
		case 0x20060200:			//(当前)总视在功率数据项
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
		case 0x200F0200:			//(当前)电网频率
		{
			uint32 Frequency;
			memcpy(&Frequency, buff, sizeof(uint32));
			nNum = 1;
			if(nItemID >= nNum) 
			{
				return -1;
			}
			*fOut = (float32)Frequency+0.9;//临时处理, 频率精确到0.001
		}
			break;
		case 0x200A0200:			//(当前)功率因数
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
		case 0x2A000200:			//电容器投切次数
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
        
        case 0x10100200:                //(当前)正向有功总尖峰平谷最大需量
		case 0x10200200:                //(当前)反向有功总尖峰平谷最大需量
		case 0x10300200:                //(当前)正向无功总尖峰平谷最大需量
		case 0x10400200:                //(当前)反向无功总尖峰平谷最大需量
		case 0x10500200:                //(当前)1象限无功总尖峰平谷最大需量
		case 0x10600200:                //(当前)2象限无功总尖峰平谷最大需量
		case 0x10700200:                //(当前)3象限无功总尖峰平谷最大需量
		case 0x10800200:                //(当前)4象限无功总尖峰平谷最大需量
        {
            OOP_DEMAND_T Demand = {0};
            memcpy(&Demand, buff, sizeof(OOP_DEMAND_T));
			nNum = Demand.nNum;
            if(nItemID >= nNum) 
			{
				return -1;
			}
			*fOut = (float32)Demand.demand[nItemID].nValue;
            *timeFlag = 1;//带时标
            memcpy(pTime, &Demand.demand[nItemID].time, sizeof(OOP_DATETIME_S_T));
		}
            break;
        case 0x201F0206:                //漏保设备 运行时间
        {
            uint32 RunTime = {0};
            memcpy(&RunTime, buff, sizeof(uint32));
			*fOut = (float32)RunTime;
		}
            break;
        case 0x201F0207:                //漏保设备 额定剩余电流动作值
        case 0x20010500:                //漏保设备 当前剩余电流
        {
            int16 Value = {0};
            memcpy(&Value, buff, sizeof(int16));
			*fOut = (float32)Value;
		}
            break;
        case 0x20020200:			   //(当前)电压相角
        case 0x20030200:			   //(当前)电压电流相角
        case 0xAAAA0001:			   //(当前)电流相角
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
        case 0x20260200:						//电压不平衡率
        case 0x20270200:						//电流不平衡率
		{
			uint16 ValueU16 = {0};
			memcpy(&ValueU16, buff, sizeof(uint16));
			*fOut = (float32)ValueU16;
		}
			break;
        case 0x200D0200:			   //(当前)A相 电压谐波含有率（总及2…n次）
        case 0x200D0300:			   //(当前)B相 电压谐波含有率（总及2…n次）
        case 0x200D0400:			   //(当前)C相 电压谐波含有率（总及2…n次）
        case 0x200E0200:			   //(当前)A相 电流谐波含有率（总及2…n次）
        case 0x200E0300:			   //(当前)B相 电流谐波含有率（总及2…n次）
        case 0x200E0400:			   //(当前)C相 电流谐波含有率（总及2…n次）
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
        case 0x20330200:			   //(当前)A相 电压谐波含量（2…n次）
        case 0x20330300:			   //(当前)B相 电压谐波含量（2…n次）
        case 0x20330400:			   //(当前)C相 电压谐波含量（2…n次）
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
        case 0x20340200:			   //(当前)A相 电流谐波含量（2…n次）
        case 0x20340300:			   //(当前)B相 电流谐波含量（2…n次）
        case 0x20340400:			   //(当前)C相 电流谐波含量（2…n次）
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
* 函数名称: sp_data_block_split_zhj
* 函数功能: 数据块拆分 自定义数据块
* 输入参数: 无
* 输出参数: 无
* 返 回 值: 无
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
* 函数名称: fratio_get_by_sptype_zhj
* 函数功能: 数据块拆分 自定义数据块
* 输入参数: 无
* 输出参数: 无
* 返 回 值: 无
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
* 函数名称: sp_data_block_split_zhj
* 函数功能: 数据块拆分 自定义数据块
* 输入参数: 无
* 输出参数: 无
* 返 回 值: 无
*******************************************************************************/
int sp_data_block_split_zhj(uint32 nMainOAD, uint32 nOAD, uint32 nSpType, uint8 nItemID, uint8 *buff, float32 *fOut, uint8 *timeFlag, OOP_DATETIME_S_T *pTime)
{
	uint16 nNum = 0;

    if( (0x21220200 == nMainOAD) || (0x21230200 == nMainOAD) )//极值统计：按照面向对象极值统计思路定义统计参数
    {
        switch(nOAD)
    	{
    		case 0x20000201://A相电压
            case 0x20000202://B相电压
            case 0x20000203://C相电压
            
            case 0x20010201://A相电流
            case 0x20010202://B相电流
            case 0x20010203://C相电流
            case 0x20010600://零序电流

            case 0x200F0200://电网频率
            case 0x20260200://电压不平衡率
            case 0x20270200://电流不平衡率
            
            case 0x200D0201://A相电压总畸变率
            case 0x200D0301://B相电压总畸变率
            case 0x200D0401://C相电压总畸变率
            {
    			if((YC_SP_PRE_DAY_MAX == nSpType)||(YC_SP_PRE_MON_MAX == nSpType))//上日、上月最大值/峰值
                {
                    OOP_EXTREMERES_T Extremeres;
                    memset(&Extremeres, 0, sizeof(OOP_EXTREMERES_T));
        			memcpy(&Extremeres, buff, sizeof(OOP_EXTREMERES_T));
                    fvalue_get_by_choice_zhj(Extremeres.dbMaxValue, fOut);
                    *timeFlag = 1;//带时标
                    memcpy(pTime, &Extremeres.tmMax, sizeof(OOP_DATETIME_S_T));
                }
                else if((YC_SP_PRE_DAY_MIN == nSpType)||(YC_SP_PRE_MON_MIN == nSpType))//上日、上月最小值/谷值
                {
                    OOP_EXTREMERES_T Extremeres;
                    memset(&Extremeres, 0, sizeof(OOP_EXTREMERES_T));
        			memcpy(&Extremeres, buff, sizeof(OOP_EXTREMERES_T));
                    fvalue_get_by_choice_zhj(Extremeres.dbMinValue, fOut);
                    *timeFlag = 1;//带时标
                    memcpy(pTime, &Extremeres.tmMin, sizeof(OOP_DATETIME_S_T));
                }
    		}
                break;
    		
    		default:
    			return -1;
                break;
    	}
    }
    else if( (0x21920200 == nMainOAD) || (0x21930200 == nMainOAD) )//合格率统计
    {
        switch(nOAD)
    	{
    		case 0x20000201://A相电压
            case 0x20000202://B相电压
            case 0x20000203://C相电压
            case 0x20260200://电压不平衡率
            case 0x20270200://电流不平衡率
            
            case 0x200D0201://A相电压总畸变率
            case 0x200D0301://B相电压总畸变率
            case 0x200D0401://C相电压总畸变率
            {
    			MT_QR_RES_T mtQrRes;
    			memset(&mtQrRes, 0, sizeof(MT_QR_RES_T));
                memcpy(&mtQrRes, buff, sizeof(MT_QR_RES_T));
                if((YC_SP_PRE_DAY_MT_TIME == nSpType)||(YC_SP_PRE_MON_MT_TIME == nSpType))//上日、上月监测时间
                {
                    *fOut = mtQrRes.mtTime;
                }
                else if((YC_SP_PRE_DAY_QR_TIME == nSpType)||(YC_SP_PRE_MON_QR_TIME == nSpType))//上日、上月合格时间
                {
                    *fOut = mtQrRes.qrTime;
                }
                else if((YC_SP_PRE_DAY_UP_TIME == nSpType)||(YC_SP_PRE_MON_UP_TIME == nSpType))//上日、上月超上限时间
                {
                    *fOut = mtQrRes.upTime;
                }
                else if((YC_SP_PRE_DAY_DW_TIME == nSpType)||(YC_SP_PRE_MON_DW_TIME == nSpType))//上日、上月超下限时间
                {
                    *fOut = mtQrRes.dwTime;
                }
                else if((YC_SP_PRE_DAY_QT_RATE == nSpType)||(YC_SP_PRE_MON_QT_RATE == nSpType))//上日、上月合格率
                {
                    *fOut = mtQrRes.qrRate;
                }
                else if((YC_SP_PRE_DAY_UP_RATE == nSpType)||(YC_SP_PRE_MON_UP_RATE == nSpType))//上日、上月超上限率
                {
                    *fOut = mtQrRes.upRate;
                }
                else if((YC_SP_PRE_DAY_DW_RATE == nSpType)||(YC_SP_PRE_MON_DW_RATE == nSpType))//上日、上月超下限率
                {
                    *fOut = mtQrRes.dwRate;
                }
                else if((YC_SP_PRE_DAY_MT_TOTAL == nSpType)||(YC_SP_PRE_MON_MT_TOTAL == nSpType))//上日、上月累加和
                {
                    *fOut = mtQrRes.mtTotal;
                }
                else if((YC_SP_PRE_DAY_MT_AVG == nSpType)||(YC_SP_PRE_MON_MT_AVG == nSpType))//上日、上月平均值
                {
                    *fOut = mtQrRes.mtAvg;
                }
    		}
                break;

            case 0x200F0200://电网频率
            {
    			MT_QR_RES_T mtQrRes;
    			memset(&mtQrRes, 0, sizeof(MT_QR_RES_T));
                memcpy(&mtQrRes, buff, sizeof(MT_QR_RES_T));
                if((YC_SP_PRE_DAY_MT_TIME == nSpType)||(YC_SP_PRE_MON_MT_TIME == nSpType))//上日、上月监测时间
                {
                    *fOut = mtQrRes.mtTime;
                }
                else if((YC_SP_PRE_DAY_QR_TIME == nSpType)||(YC_SP_PRE_MON_QR_TIME == nSpType))//上日、上月合格时间
                {
                    *fOut = mtQrRes.qrTime;
                }
                else if((YC_SP_PRE_DAY_UP_TIME == nSpType)||(YC_SP_PRE_MON_UP_TIME == nSpType))//上日、上月超上限时间
                {
                    *fOut = mtQrRes.upTime + mtQrRes.dwTime;//点表为 频率超限时间=超上限时间+超下限时间
                }
                else if((YC_SP_PRE_DAY_DW_TIME == nSpType)||(YC_SP_PRE_MON_DW_TIME == nSpType))//上日、上月超下限时间
                {
                    *fOut = mtQrRes.dwTime;
                }
                else if((YC_SP_PRE_DAY_QT_RATE == nSpType)||(YC_SP_PRE_MON_QT_RATE == nSpType))//上日、上月合格率
                {
                    *fOut = mtQrRes.qrRate;
                }
                else if((YC_SP_PRE_DAY_UP_RATE == nSpType)||(YC_SP_PRE_MON_UP_RATE == nSpType))//上日、上月超上限率
                {
                    *fOut = mtQrRes.upRate + mtQrRes.dwRate;//点表为 频率超限率=超上限率+超下限率
                }
                else if((YC_SP_PRE_DAY_DW_RATE == nSpType)||(YC_SP_PRE_MON_DW_RATE == nSpType))//上日、上月超下限率
                {
                    *fOut = mtQrRes.dwRate;
                }
                else if((YC_SP_PRE_DAY_MT_TOTAL == nSpType)||(YC_SP_PRE_MON_MT_TOTAL == nSpType))//上日、上月累加和
                {
                    *fOut = mtQrRes.mtTotal;
                }
                else if((YC_SP_PRE_DAY_MT_AVG == nSpType)||(YC_SP_PRE_MON_MT_AVG == nSpType))//上日、上月平均值
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
    		case 0x200A0201://瞬时总功率因数三段统计
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
    		case 0x10100200://正向有功最大需量
    		case 0x10200200://反向有功最大需量
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
                *timeFlag = 1;//带时标
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
* 函数名称: string_data_block_split
* 函数功能: 数据块拆分 字符串
* 输入参数: 无
* 输出参数: 无
* 返 回 值: 无
*******************************************************************************/
int string_data_block_split_zhj(uint32 nOAD, uint8 nItemID, uint8 *buff, uint8 *pOut, uint8 *timeFlag, OOP_DATETIME_S_T *pTime)
{
	uint16 nNum = 0;

	switch(nOAD)
	{
		case 0x202F0200:                //智能电容器 运行状态字
		case 0x201F0201:                //剩余电流保护器 当前剩余电流最大相
		case 0x201F0202:                //剩余电流保护器 故障相别
		case 0x201F0203:                //剩余电流保护器 运行状态字
		case 0x201F0204:                //剩余电流保护器 控制字
		case 0x201F0205:                //剩余电流保护器 额定剩余电流动作值参数组
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
* 函数名称: yc_db_nomal_read
* 函数功能: 从数据中心读取遥测实时数据(普通型数据)
* 输入参数: typeIndex         设备类型数组下标, 从0开始
            groupIndex    设备组别数组下标, 从0开始
            seqIndex      组内偏移地址数组下标, 从0开始
            index         点号索引
* 输出参数: 无
* 返 回 值: 无
*******************************************************************************/
void yc_db_nomal_read_zhj(uint8 typeIndex, uint16 groupIndex, uint16 seqIndex, uint16 index)
{
    int m = typeIndex;//设备类型-1
    int x = groupIndex;//设备组别 从0开始
    int n=  seqIndex;//设备序号-1
    uint32 ArrIdx = 0;//一维数组索引(起始)
    
	if(index >= g_nConfigYcNumZhj[m][x][n])
	{
		return;
	}
	
	uint8   aRecvBuff[500] = {0};
	uint32  nRevcBuffLen = 0;
	int     ret  = 0;
	NOMAL_OAD_T NomalOad = {0};  //普通型OAD

	float32 fValueDB;  //存在于数据中心的值
	float32 fDeadZone;  //死区值
	float32 fDiffValue;  //差值
	float32 fRatio = 1;  //倍率
	const Map645ToOop *pMap645ToOop = NULL;

    uint8 nTimeFlag = 0;//是否带时标
    OOP_DATETIME_S_T stTime = {0};//数据自身时标

    ArrIdx = g_aYcDataIndexZhj[m][x][n];

	//数据读取
	NomalOad.logicId = g_aYcDataConfigZhj[ArrIdx+index].nLogicId;  //逻辑地址
	NomalOad.infoNum = g_aYcDataConfigZhj[ArrIdx+index].nInfoNum;  //信息点号
	NomalOad.oad.value = g_aYcDataConfigZhj[ArrIdx+index].nDbCenterOAD;  //普通OAD
	NomalOad.classtag = CLASS_DATA_NORMAL;

	ret = db_read_nomal(g_sDataCenterClient, &NomalOad, sizeof(aRecvBuff), (uint8*)aRecvBuff, &nRevcBuffLen);
	DB104_FMT_TRACE("m=%d, x=%d, n=%d, <--第%d个遥测--> addr=%08x, yc_db_nomal_read() ret = db_read_nomal() ret:%d \n", 
        m, x, n, index, g_aYcDataConfigZhj[ArrIdx+index].nObjAddrU32, ret);
	
	if(ret == 0)
	{
		DB104_FMT_TRACE("yc_db_nomal_read() 读取数据成功  g_aYcDataConfig[%d].nDbCenterOAD=%08x \n", 
			index, g_aYcDataConfigZhj[ArrIdx+index].nDbCenterOAD);
		DB104_BUF_TRACE(aRecvBuff, nRevcBuffLen, "报文: ");

		//读取上来的是数据块，需要进行分相处理
		ret = data_block_split_zhj(g_aYcDataConfigZhj[ArrIdx+index].nDbCenterOAD, g_aYcDataConfigZhj[ArrIdx+index].nItemIndex, aRecvBuff, &fValueDB, &nTimeFlag, &stTime);
		if(ret > 0)
		{
		    g_aYcDataSendZhj[ArrIdx+index].bNewValidFlag = TRUE;
            
			//读取倍率
			pMap645ToOop = oad_info_find(g_aYcDataConfigZhj[ArrIdx+index].nDbCenterOAD);
			if(pMap645ToOop == NULL)
			{
				DB104_FMT_TRACE("yc_db_nomal_read() if(pMap645ToOop == NULL) \n");
			}
			else
			{
				fRatio = ratio_convert(pMap645ToOop->DesScaler, E_OOP_TO_REAL);
				DB104_FMT_TRACE("读取倍率 fRatio=%f \n", fRatio);
			}
			g_aYcDataSendZhj[ArrIdx+index].fNewValue = fValueDB*fRatio;
			DB104_FMT_TRACE("倍率处理之后g_aYcDataSend[index].fNewValue=%f, 倍率处理之前fValueDB=%f \n", g_aYcDataSendZhj[ArrIdx+index].fNewValue, fValueDB);
			
			//读取变化死区值
			fDeadZone = g_aYcDataConfigZhj[ArrIdx+index].fDeadZoneValue;
			DB104_FMT_TRACE("变化死区值fDeadZone=%f \n", fDeadZone);

			//临时赋值, 测试变化上送
			if(g_bTestEnable)
			{
				g_aYcDataSendZhj[ArrIdx+index].fHisValue = rand()%100;//100以内随机数
				DB104_FMT_TRACE("遥测随机数 g_aYcDataSend[%d].fHisValue=%f \n", index, g_aYcDataSendZhj[ArrIdx+index].fHisValue);
			}

			//判断实时值是否越过死区
			fDiffValue = fabs(g_aYcDataSendZhj[ArrIdx+index].fNewValue - g_aYcDataSendZhj[ArrIdx+index].fHisValue);
			if(fDiffValue > fDeadZone)
			{
				DB104_FMT_TRACE("变化值fDiffValue=%f, 大于死区值, fNewValue=%f, fHisValue=%f 需要更新上报! \n", 
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
				DB104_FMT_TRACE("变化值fDiffValue=%f, 小于死区值, fNewValue=%f, fHisValue=%f \n", 
					fDiffValue, g_aYcDataSendZhj[ArrIdx+index].fNewValue, g_aYcDataSendZhj[ArrIdx+index].fHisValue);
			}
			
		}
		else
		{
		    g_aYcDataSendZhj[ArrIdx+index].bNewValidFlag = FALSE;
			DB104_FMT_TRACE("yc_db_nomal_read() 解析数据失败  g_aYcDataConfig[%d].nDbCenterOAD=%08x \n", 
				index, g_aYcDataConfigZhj[ArrIdx+index].nDbCenterOAD);
		}
	}
	else
	{
	    g_aYcDataSendZhj[ArrIdx+index].bNewValidFlag = FALSE;
		DB104_FMT_TRACE("yc_db_nomal_read() 读取数据  g_aYcDataConfig[%d].nDbCenterOAD=%08x \n", 
			index, g_aYcDataConfigZhj[ArrIdx+index].nDbCenterOAD);
		DB104_FMT_TRACE("yc_db_nomal_read() 数据读取失败! \n");
	}

    if(nTimeFlag > 0)
    {
        DB104_FMT_TRACE("yc_db_nomal_read() 数据带时标nTimeFlag=%d \n", nTimeFlag);
        ooptm_to_cp56time2a(stTime, &g_aYcDataSendZhj[ArrIdx+index].btime[0]);
    }
	else
    {
        system_time_get(&g_aYcDataSendZhj[ArrIdx+index].btime[0]);
    }
}

/*******************************************************************************
* 函数名称: yc_db_record_read
* 函数功能: 从数据中心读取遥测实时数据(记录型数据)
* 输入参数: typeIndex         设备类型数组下标, 从0开始
            groupIndex    设备组别数组下标, 从0开始
            seqIndex      组内偏移地址数组下标, 从0开始
            index         点号索引
* 输出参数: 无
* 返 回 值: 无
*******************************************************************************/
void yc_db_record_read_zhj(uint8 typeIndex, uint16 groupIndex, uint16 seqIndex, uint16 index)
{
    int m = typeIndex;//设备类型-1
    int x = groupIndex;//设备组别 从0开始
    int n=  seqIndex;//设备序号-1
    int i = 0;//定值下标
    uint16 areaID = g_nFixedAreaIdZhj[m][x][n];//定值区号
    uint32 macAddr = 0;//参数配置中设备地址的点号
	uint32 ArrIdx = 0;//一维数组索引(起始)
	uint32 ArrParaIdx = 0;//一维数组索引(参数起始)
    
	if(index >= g_nConfigYcNumZhj[m][x][n])
	{
		return;
	}

	uint8	aRecvBuff[500] = {0};
	uint32	nRevcBuffLen = 0;
	int 	ret  = 0;
	READ_RECORD_T ReadRecord;  //记录型OAD
	RECORD_UNIT_T RecordOut;  //记录型查询结果
	memset(&ReadRecord, 0, sizeof(READ_RECORD_T));
	memset(&RecordOut, 0, sizeof(RECORD_UNIT_T));
	
	time_t nRealTime_t;  //当前系统时间的转换的秒数

	float32 fValueDB;  //存在于数据中心的值
	float32 fDeadZone;	//死区值
	float32 fDiffValue;  //差值
	float32 fRatio = 1;  //倍率
	const Map645ToOop *pMap645ToOop = NULL;

    uint8 nTimeFlag = 0;//是否带时标
    OOP_DATETIME_S_T stTime = {0};//数据自身时标
	
	ArrIdx = g_aYcDataIndexZhj[m][x][n];

	//数据读取
	nRealTime_t = time(NULL);
	
	ReadRecord.recordOAD.logicId = g_aYcDataConfigZhj[ArrIdx+index].nLogicId;  //逻辑地址
	ReadRecord.recordOAD.infoNum = g_aYcDataConfigZhj[ArrIdx+index].nInfoNum;  //信息点号
	ReadRecord.recordOAD.road.oadMain.value = g_aYcDataConfigZhj[ArrIdx+index].nDbCenterOAD;  //主OAD
	ReadRecord.recordOAD.road.oadCols.nNum = g_aYcDataConfigZhj[ArrIdx+index].nOadColsNum;  //次OAD数量
	memcpy(&ReadRecord.recordOAD.road.oadCols.oad[0], &g_aYcDataConfigZhj[ArrIdx+index].aOadCols[0],
		g_aYcDataConfigZhj[ArrIdx+index].nOadColsNum*sizeof(uint32));  //关联次OAD
	ReadRecord.recordOAD.classtag = CLASS_DATA_COLLEC;

	ReadRecord.cType = COL_TM_STORE;  //采集存储时间
	//ReadRecord.cStart = nRealTime_t + 8*60*60-24*3600;  //时间起始点，当前时间提前一天
	//ReadRecord.cStart = 0;  //时间起始点
	ReadRecord.cStart = nRealTime_t + 8*60*60 - 10*60;  //时间起始点, 当前时间往前10分钟
	ReadRecord.cEnd = nRealTime_t + 8*60*60;  //时间结束点
	ReadRecord.sortType = DOWN_SORT;  //排序方式 降序
	ReadRecord.MAC.nNum = 6;  //附加校验 长度
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
	
	ReadRecord.ti.unit = TI_HOUR;  //时间间隔 单位
	ReadRecord.ti.value = 15;  //时间间隔 值

	DB104_FMT_TRACE("m=%d, x=%d, n=%d <--第%d个遥测--> addr=%08x, deal=%08x, yc_db_record_read() 数组下标index=%d, 逻辑地址logicId=%d, 信息点号infoNum=%d, 主OAD=%08x, 次OAD数量=%d, 次OAD=%08x, 校验MAC.nNum=%d, MAC.value=%02x%02x%02x%02x%02x%02x \n", 
		m, x, n, index, g_aYcDataConfigZhj[ArrIdx+index].nObjAddrU32, g_aYcDataConfigZhj[ArrIdx+index].nDealType, index, ReadRecord.recordOAD.logicId, ReadRecord.recordOAD.infoNum, ReadRecord.recordOAD.road.oadMain.value, ReadRecord.recordOAD.road.oadCols.nNum, 
		ReadRecord.recordOAD.road.oadCols.oad[0].value, ReadRecord.MAC.nNum, ReadRecord.MAC.value[5], ReadRecord.MAC.value[4], ReadRecord.MAC.value[3], ReadRecord.MAC.value[2], ReadRecord.MAC.value[1], ReadRecord.MAC.value[0]);
	
	ret = db_read_record(g_sDataCenterClient, &ReadRecord, sizeof(aRecvBuff), &RecordOut);
	DB104_FMT_TRACE("yc_db_center_read() ret = db_read_record() ret:%d \n", ret);
	if(ret == 0)
	{
		nRevcBuffLen = RecordOut.record[0].outDatalen;
		memcpy(&aRecvBuff[0], &RecordOut.record[0].outDataBuf[0], nRevcBuffLen);

		DB104_FMT_TRACE("yc_db_record_read() 读取数据成功  g_aYcDataConfig[%d].nDbCenterOAD=%08x \n", 
			index, g_aYcDataConfigZhj[ArrIdx+index].nDbCenterOAD);
		DB104_BUF_TRACE(aRecvBuff, nRevcBuffLen, "报文: ");

		//读取上来的是数据块，需要进行分相处理
		ret = data_block_split_zhj(g_aYcDataConfigZhj[ArrIdx+index].aOadCols[0], g_aYcDataConfigZhj[ArrIdx+index].nItemIndex, aRecvBuff, &fValueDB, &nTimeFlag, &stTime);
		if(ret > 0)
		{
		    g_aYcDataSendZhj[ArrIdx+index].bNewValidFlag = TRUE;
            
			//读取倍率
			pMap645ToOop = oad_info_find(g_aYcDataConfigZhj[ArrIdx+index].aOadCols[0]);
			if(pMap645ToOop == NULL)
			{
				DB104_FMT_TRACE("yc_db_record_read() if(pMap645ToOop == NULL) \n");
			}
			else
			{
				fRatio = ratio_convert(pMap645ToOop->DesScaler, E_OOP_TO_REAL);
				DB104_FMT_TRACE("读取倍率 fRatio=%f \n", fRatio);
			}
			g_aYcDataSendZhj[ArrIdx+index].fNewValue = fValueDB*fRatio;
			DB104_FMT_TRACE("倍率处理之后g_aYcDataSend[index].fNewValue=%f, 倍率处理之前fValueDB=%f \n", g_aYcDataSendZhj[ArrIdx+index].fNewValue, fValueDB);
			
			//读取变化死区值
			fDeadZone = g_aYcDataConfigZhj[ArrIdx+index].fDeadZoneValue;
			DB104_FMT_TRACE("变化死区值fDeadZone=%f \n", fDeadZone);

			//临时赋值, 测试变化上送
			if(g_bTestEnable)
			{
				g_aYcDataSendZhj[ArrIdx+index].fHisValue = rand()%100;//100以内随机数
				DB104_FMT_TRACE("遥测随机数 g_aYcDataSend[%d].fHisValue=%f \n", index, g_aYcDataSendZhj[ArrIdx+index].fHisValue);
			}

			//判断实时值是否越过死区
			fDiffValue = fabs(g_aYcDataSendZhj[ArrIdx+index].fNewValue - g_aYcDataSendZhj[ArrIdx+index].fHisValue);
			if(fDiffValue > fDeadZone)
			{
				DB104_FMT_TRACE("变化值fDiffValue=%f, 大于死区值, fNewValue=%f, fHisValue=%f 需要更新上报! \n", 
					fDiffValue, g_aYcDataSendZhj[ArrIdx+index].fNewValue, g_aYcDataSendZhj[ArrIdx+index].fHisValue);
				g_bYcChanged = TRUE;
				g_aYcDataSendZhj[ArrIdx+index].bChangeFlag = TRUE;
				g_aYcDataSendZhj[ArrIdx+index].bSendFlag = FALSE;
                g_aChangedEquipStateYC_R[m][x][n] = 1;
			}
			else
			{
				DB104_FMT_TRACE("变化值fDiffValue=%f, 小于死区值, fNewValue=%f, fHisValue=%f \n", 
					fDiffValue, g_aYcDataSendZhj[ArrIdx+index].fNewValue, g_aYcDataSendZhj[ArrIdx+index].fHisValue);
			}
			
		}
		else
		{
		    g_aYcDataSendZhj[ArrIdx+index].bNewValidFlag = FALSE;
			DB104_FMT_TRACE("yc_db_record_read() 解析数据失败  g_aYcDataConfig[%d].nDbCenterOAD=%08x \n", 
				index, g_aYcDataConfigZhj[ArrIdx+index].nDbCenterOAD);
		}
	}
	else
	{
	    g_aYcDataSendZhj[ArrIdx+index].bNewValidFlag = FALSE;
		DB104_FMT_TRACE("yc_db_record_read() 读取数据  g_aYcDataConfig[%d].nDbCenterOAD=%08x \n", 
			index, g_aYcDataConfigZhj[ArrIdx+index].nDbCenterOAD);
		DB104_FMT_TRACE("yc_db_record_read() 数据读取失败! \n");

        //临时特殊处理
        /*
        if( ((E_EQUIP_CAPACITOR-1)==m && 0x00200001==g_aYcDataConfigZhj[ArrIdx+index].nObjAddrU32) 
            || ((E_EQUIP_CAPACITOR-1)==m && 0x00200002==g_aYcDataConfigZhj[ArrIdx+index].nObjAddrU32))
        {
            g_aYcDataSendZhj[ArrIdx+index].bNewValidFlag = TRUE;
            DB104_FMT_TRACE("特殊处理 m=%d, addr=%08x \n", m, g_aYcDataConfigZhj[ArrIdx+index].nObjAddrU32);
        }
        */
	}
	
	if(nTimeFlag > 0)
    {
        DB104_FMT_TRACE("yc_db_record_read() 数据带时标nTimeFlag=%d \n", nTimeFlag);
        ooptm_to_cp56time2a(stTime, &g_aYcDataSendZhj[ArrIdx+index].btime[0]);
    }
	else
    {
        system_time_get(&g_aYcDataSendZhj[ArrIdx+index].btime[0]);
    }
}

/*******************************************************************************
* 函数名称: yc_db_sp_record_read_zhj
* 函数功能: 从数据中心读取遥测实时数据(特殊 记录型数据)
* 输入参数: typeIndex         设备类型数组下标, 从0开始
            groupIndex    设备组别数组下标, 从0开始
            seqIndex      组内偏移地址数组下标, 从0开始
            index         点号索引
* 输出参数: 无
* 返 回 值: 无
*******************************************************************************/
void yc_db_sp_record_read_zhj(uint8 typeIndex, uint16 groupIndex, uint16 seqIndex, uint16 index)
{
    int m = typeIndex;//设备类型-1
    int x = groupIndex;//设备组别 从0开始
    int n=  seqIndex;//设备序号-1
    int i = 0;//定值下标
    uint16 areaID = g_nFixedAreaIdZhj[m][x][n];//定值区号
    uint32 macAddr = 0;//参数配置中设备地址的点号
	uint32 ArrIdx = 0;//一维数组索引(起始)
	uint32 ArrParaIdx = 0;//一维数组索引(参数起始)
    
	if(index >= g_nConfigYcNumZhj[m][x][n])
	{
		return;
	}

	uint8	aRecvBuff[500] = {0};
	uint32	nRevcBuffLen = 0;
	int 	ret  = 0;
	READ_RECORD_T ReadRecord;  //记录型OAD
	RECORD_UNIT_T RecordOut;  //记录型查询结果
	memset(&ReadRecord, 0, sizeof(READ_RECORD_T));
	memset(&RecordOut, 0, sizeof(RECORD_UNIT_T));
	
	time_t nRealTime_t;  //当前系统时间的转换的秒数

	float32 fValueDB;  //存在于数据中心的值
	float32 fDeadZone;	//死区值
	float32 fDiffValue;  //差值
	float32 fRatio = 1;  //倍率
	const Map645ToOop *pMap645ToOop = NULL;

    uint8 nTimeFlag = 0;//是否带时标
    OOP_DATETIME_S_T stTime = {0};//数据自身时标
	
	ArrIdx = g_aYcDataIndexZhj[m][x][n];

	//数据读取
	nRealTime_t = time(NULL);
	
	ReadRecord.recordOAD.logicId = g_aYcDataConfigZhj[ArrIdx+index].nLogicId;  //逻辑地址
	ReadRecord.recordOAD.infoNum = g_aYcDataConfigZhj[ArrIdx+index].nInfoNum;  //信息点号
	ReadRecord.recordOAD.road.oadMain.value = g_aYcDataConfigZhj[ArrIdx+index].nDbCenterOAD;  //主OAD
	ReadRecord.recordOAD.road.oadCols.nNum = g_aYcDataConfigZhj[ArrIdx+index].nOadColsNum;  //次OAD数量
	memcpy(&ReadRecord.recordOAD.road.oadCols.oad[0], &g_aYcDataConfigZhj[ArrIdx+index].aOadCols[0],
		g_aYcDataConfigZhj[ArrIdx+index].nOadColsNum*sizeof(uint32));  //关联次OAD
	ReadRecord.recordOAD.classtag = CLASS_DATA_COLLEC;

	ReadRecord.cType = COL_TM_STORE;  //采集存储时间
	//ReadRecord.cStart = nRealTime_t-24*3600;  //时间起始点，当前时间提前一天
	ReadRecord.cStart = 0;  //时间起始点，当前时间提前一天

#if TEST_VER_FLAG == 1
    //测试版本直接采用当前时间, 读取实时统计值
	ReadRecord.cEnd = nRealTime_t + 8*60*60;  //时间结束点
#else
    if( (YC_SP_PRE_DAY_START < g_aYcDataConfigZhj[ArrIdx+index].nDealType) 
                                    && (YC_SP_PRE_DAY_END > g_aYcDataConfigZhj[ArrIdx+index].nDealType) )
    {
        ReadRecord.cEnd = nPreDayTime_t + 8*60*60;  //时间结束点
    }
    else if( (YC_SP_PRE_MON_START < g_aYcDataConfigZhj[ArrIdx+index].nDealType) 
                && (YC_SP_PRE_MON_END > g_aYcDataConfigZhj[ArrIdx+index].nDealType) )
    {
        ReadRecord.cEnd = nPreMonTime_t + 8*60*60;  //时间结束点
    }
    else
    {
        ReadRecord.cEnd = nRealTime_t + 8*60*60;  //时间结束点
    }
#endif

	ReadRecord.sortType = DOWN_SORT;  //排序方式 降序
	ReadRecord.MAC.nNum = 6;  //附加校验 长度
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

    //高级统计功能不考虑mac校验!!!!!!
    memset(ReadRecord.MAC.value, 0, ReadRecord.MAC.nNum);
	
	ReadRecord.ti.unit = TI_HOUR;  //时间间隔 单位
	ReadRecord.ti.value = 15;  //时间间隔 值

	DB104_FMT_TRACE("m=%d, x=%d, n=%d <--第%d个遥测--> addr=%08x, deal=%08x, yc_db_record_read() 数组下标index=%d, 逻辑地址logicId=%d, 信息点号infoNum=%d, 主OAD=%08x, 次OAD数量=%d, 次OAD=%08x, 校验MAC.nNum=%d, MAC.value=%02x%02x%02x%02x%02x%02x \n", 
		m, x, n, index, g_aYcDataConfigZhj[ArrIdx+index].nObjAddrU32, g_aYcDataConfigZhj[ArrIdx+index].nDealType, index, ReadRecord.recordOAD.logicId, ReadRecord.recordOAD.infoNum, ReadRecord.recordOAD.road.oadMain.value, ReadRecord.recordOAD.road.oadCols.nNum, 
		ReadRecord.recordOAD.road.oadCols.oad[0].value, ReadRecord.MAC.nNum, ReadRecord.MAC.value[5], ReadRecord.MAC.value[4], ReadRecord.MAC.value[3], ReadRecord.MAC.value[2], ReadRecord.MAC.value[1], ReadRecord.MAC.value[0]);
	
	ret = db_read_record(g_sDataCenterClient, &ReadRecord, sizeof(aRecvBuff), &RecordOut);
	DB104_FMT_TRACE("yc_db_center_read() ret = db_read_record()  ret:%d \n", ret);
	if(ret == 0)
	{
		nRevcBuffLen = RecordOut.record[0].outDatalen;
		memcpy(&aRecvBuff[0], &RecordOut.record[0].outDataBuf[0], nRevcBuffLen);

		DB104_FMT_TRACE("yc_db_record_read() 读取数据成功 g_aYcDataConfig[%d].nDbCenterOAD=%08x \n", 
			index, g_aYcDataConfigZhj[ArrIdx+index].nDbCenterOAD);
		DB104_BUF_TRACE(aRecvBuff, nRevcBuffLen, "报文: ");

		//读取上来的是数据块，需要进行分相处理
		ret = sp_data_block_split_zhj(g_aYcDataConfigZhj[ArrIdx+index].nDbCenterOAD, g_aYcDataConfigZhj[ArrIdx+index].aOadCols[0], g_aYcDataConfigZhj[ArrIdx+index].nDealType, 
		    g_aYcDataConfigZhj[ArrIdx+index].nItemIndex, aRecvBuff, &fValueDB, &nTimeFlag, &stTime);
		if(ret > 0)
		{
		    g_aYcDataSendZhj[ArrIdx+index].bNewValidFlag = TRUE;
            
			//读取倍率
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
    				DB104_FMT_TRACE("ratio_convert() 读取倍率 fRatio=%f \n", fRatio);
    			}
            }
			
			g_aYcDataSendZhj[ArrIdx+index].fNewValue = fValueDB*fRatio;
			DB104_FMT_TRACE("倍率处理之后g_aYcDataSend[index].fNewValue=%f, 倍率处理之前fValueDB=%f, 倍率fRatio=%f \n", g_aYcDataSendZhj[ArrIdx+index].fNewValue, fValueDB, fRatio);
			
			//读取变化死区值
			fDeadZone = g_aYcDataConfigZhj[ArrIdx+index].fDeadZoneValue;
			DB104_FMT_TRACE("变化死区值fDeadZone=%f \n", fDeadZone);

			//临时赋值, 测试变化上送
			if(g_bTestEnable)
			{
				g_aYcDataSendZhj[ArrIdx+index].fHisValue = rand()%100;//100以内随机数
				DB104_FMT_TRACE("遥测随机数 g_aYcDataSend[%d].fHisValue=%f \n", index, g_aYcDataSendZhj[ArrIdx+index].fHisValue);
			}

			//判断实时值是否越过死区
			fDiffValue = fabs(g_aYcDataSendZhj[ArrIdx+index].fNewValue - g_aYcDataSendZhj[ArrIdx+index].fHisValue);
			if(fDiffValue > fDeadZone)
			{
				DB104_FMT_TRACE("变化值fDiffValue=%f, 大于死区值, fNewValue=%f, fHisValue=%f 需要更新上报! \n", 
					fDiffValue, g_aYcDataSendZhj[ArrIdx+index].fNewValue, g_aYcDataSendZhj[ArrIdx+index].fHisValue);
				g_bYcChanged = TRUE;
				g_aYcDataSendZhj[ArrIdx+index].bChangeFlag = TRUE;
				g_aYcDataSendZhj[ArrIdx+index].bSendFlag = FALSE;
                g_aChangedEquipStateYC_R[m][x][n] = 1;
			}
			else
			{
				DB104_FMT_TRACE("变化值fDiffValue=%f, 小于死区值, fNewValue=%f, fHisValue=%f \n", 
					fDiffValue, g_aYcDataSendZhj[ArrIdx+index].fNewValue, g_aYcDataSendZhj[ArrIdx+index].fHisValue);
			}
			
		}
		else
		{
		    g_aYcDataSendZhj[ArrIdx+index].bNewValidFlag = FALSE;
			DB104_FMT_TRACE("yc_db_record_read() 解析数据失败  g_aYcDataConfig[%d].nDbCenterOAD=%08x \n", 
				index, g_aYcDataConfigZhj[ArrIdx+index].nDbCenterOAD);
		}
	}
	else
	{
	    g_aYcDataSendZhj[ArrIdx+index].bNewValidFlag = FALSE;
		DB104_FMT_TRACE("yc_db_record_read() 读取数据  g_aYcDataConfig[%d].nDbCenterOAD=%08x \n", 
			index, g_aYcDataConfigZhj[ArrIdx+index].nDbCenterOAD);
		DB104_FMT_TRACE("yc_db_record_read() 数据读取失败! \n");
	}
	
	if(nTimeFlag > 0)
    {
        DB104_FMT_TRACE("yc_db_record_read() 数据带时标nTimeFlag=%d \n", nTimeFlag);
        ooptm_to_cp56time2a(stTime, &g_aYcDataSendZhj[ArrIdx+index].btime[0]);
    }
	else
    {
        system_time_get(&g_aYcDataSendZhj[ArrIdx+index].btime[0]);
    }
}


/*******************************************************************************
* 函数名称: yc_db_record_str_read_zhj
* 函数功能: 从数据中心读取遥测实时数据(记录型数据)
* 输入参数: typeIndex         设备类型数组下标, 从0开始
            groupIndex    设备组别数组下标, 从0开始
            seqIndex      组内偏移地址数组下标, 从0开始
            index         点号索引
* 输出参数: 无
* 返 回 值: 无
*******************************************************************************/
void yc_db_record_str_read_zhj(uint8 typeIndex, uint16 groupIndex, uint16 seqIndex, uint16 index)
{
    int m = typeIndex;//设备类型-1
    int x = groupIndex;//设备组别 从0开始
    int n=  seqIndex;//设备序号-1
    int i = 0;//定值下标
    uint16 areaID = g_nFixedAreaIdZhj[m][x][n];//定值区号
    uint32 macAddr = 0;//参数配置中设备地址的点号
    uint8  octArr[64] = {0};//字符数组
	uint32 ArrIdx = 0;//一维数组索引(起始)
	uint32 ArrParaIdx = 0;//一维数组索引(参数起始)
    
	if(index >= g_nConfigYcNumZhj[m][x][n])
	{
		return;
	}

	uint8	aRecvBuff[500] = {0};
	uint32	nRevcBuffLen = 0;
	int 	ret  = 0;
	READ_RECORD_T ReadRecord;  //记录型OAD
	RECORD_UNIT_T RecordOut;  //记录型查询结果
	memset(&ReadRecord, 0, sizeof(READ_RECORD_T));
	memset(&RecordOut, 0, sizeof(RECORD_UNIT_T));

    uint8 nTimeFlag = 0;//是否带时标
    OOP_DATETIME_S_T stTime = {0};//数据自身时标
	
	ArrIdx = g_aYcDataIndexZhj[m][x][n];
	
	time_t nRealTime_t;  //当前系统时间的转换的秒数

	//数据读取
	nRealTime_t = time(NULL);
	
	ReadRecord.recordOAD.logicId = g_aYcDataConfigZhj[ArrIdx+index].nLogicId;  //逻辑地址
	ReadRecord.recordOAD.infoNum = g_aYcDataConfigZhj[ArrIdx+index].nInfoNum;  //信息点号
	ReadRecord.recordOAD.road.oadMain.value = g_aYcDataConfigZhj[ArrIdx+index].nDbCenterOAD;  //主OAD
	ReadRecord.recordOAD.road.oadCols.nNum = g_aYcDataConfigZhj[ArrIdx+index].nOadColsNum;  //次OAD数量
	memcpy(&ReadRecord.recordOAD.road.oadCols.oad[0], &g_aYcDataConfigZhj[ArrIdx+index].aOadCols[0],
		g_aYcDataConfigZhj[ArrIdx+index].nOadColsNum*sizeof(uint32));  //关联次OAD
	ReadRecord.recordOAD.classtag = CLASS_DATA_COLLEC;

	ReadRecord.cType = COL_TM_STORE;  //采集存储时间
	//ReadRecord.cStart = nRealTime_t-24*3600;  //时间起始点，当前时间提前一天
	ReadRecord.cStart = 0;  //时间起始点，当前时间提前一天
	ReadRecord.cEnd = nRealTime_t + 8*60*60;  //时间结束点
	ReadRecord.sortType = DOWN_SORT;  //排序方式 降序
	ReadRecord.MAC.nNum = 6;  //附加校验 长度
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
	
	ReadRecord.ti.unit = TI_HOUR;  //时间间隔 单位
	ReadRecord.ti.value = 15;  //时间间隔 值

	DB104_FMT_TRACE("m=%d, x=%d, n=%d <--第%d个遥测--> addr=%08x, yc_db_record_read() 数组下标index=%d, 逻辑地址logicId=%d, 信息点号infoNum=%d, 主OAD=%08x, 次OAD数量=%d, 次OAD=%08x, 校验MAC.nNum=%d, MAC.value=%02x%02x%02x%02x%02x%02x \n", 
		m, x, n, index, g_aYcDataConfigZhj[ArrIdx+index].nObjAddrU32, index, ReadRecord.recordOAD.logicId, ReadRecord.recordOAD.infoNum, ReadRecord.recordOAD.road.oadMain.value, ReadRecord.recordOAD.road.oadCols.nNum, 
		ReadRecord.recordOAD.road.oadCols.oad[0].value, ReadRecord.MAC.nNum, ReadRecord.MAC.value[5], ReadRecord.MAC.value[4], ReadRecord.MAC.value[3], ReadRecord.MAC.value[2], ReadRecord.MAC.value[1], ReadRecord.MAC.value[0]);
	
	ret = db_read_record(g_sDataCenterClient, &ReadRecord, sizeof(aRecvBuff), &RecordOut);
	DB104_FMT_TRACE("yc_db_record_str_read_zhj() ret = db_read_record() ret:%d \n", ret);
	if(ret == 0)
	{
		nRevcBuffLen = RecordOut.record[0].outDatalen;
		memcpy(&aRecvBuff[0], &RecordOut.record[0].outDataBuf[0], nRevcBuffLen);

		DB104_FMT_TRACE("yc_db_record_str_read_zhj() 读取数据成功  g_aYcDataConfig[%d].nDbCenterOAD=%08x \n", 
			index, g_aYcDataConfigZhj[ArrIdx+index].nDbCenterOAD);
		DB104_BUF_TRACE(aRecvBuff, nRevcBuffLen, "报文: ");

		//读取上来的是数据块，需要进行分相处理
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
            DB104_BUF_TRACE(g_aYcDataSendZhj[ArrIdx+index].aNewArrValue, ret, "octString内容: ");
		}
        else
		{
		    g_aYcDataSendZhj[ArrIdx+index].bNewValidFlag = FALSE;
			DB104_FMT_TRACE("yc_db_record_str_read_zhj() 解析数据失败  g_aYcDataConfig[%d].nDbCenterOAD=%08x, ret=%d \n", 
				index, g_aYcDataConfigZhj[ArrIdx+index].nDbCenterOAD, ret);
		}
	}
	else
	{
	    g_aYcDataSendZhj[ArrIdx+index].bNewValidFlag = FALSE;
		DB104_FMT_TRACE("yc_db_record_str_read_zhj() 读取数据  g_aYcDataConfig[%d].nDbCenterOAD=%08x \n", 
			index, g_aYcDataConfigZhj[ArrIdx+index].nDbCenterOAD);
		DB104_FMT_TRACE("yc_db_record_str_read_zhj() 数据读取失败! \n");
	}
	
	if(nTimeFlag > 0)
    {
        DB104_FMT_TRACE("yc_db_record_str_read_zhj() 数据带时标nTimeFlag=%d \n", nTimeFlag);
        ooptm_to_cp56time2a(stTime, &g_aYcDataSendZhj[ArrIdx+index].btime[0]);
    }
	else
    {
        system_time_get(&g_aYcDataSendZhj[ArrIdx+index].btime[0]);
    }
}


/*******************************************************************************
* 函数名称: yc_db_record_read
* 函数功能: 需要统计或计算的遥测信号
* 输入参数: typeIndex         设备类型数组下标, 从0开始
            groupIndex    设备组别数组下标, 从0开始
            seqIndex      组内偏移地址数组下标, 从0开始
            index         点号索引
* 输出参数: 无
* 返 回 值: 无
*******************************************************************************/
void yc_calculate_statistics_zhj(uint8 typeIndex, uint16 groupIndex, uint16 seqIndex, uint16 index)
{
    int m = typeIndex;//设备类型-1
    int x = groupIndex;//设备组别 从0开始
    int n=  seqIndex;//设备序号-1
    uint16 nAreaId = g_nFixedAreaIdZhj[m][x][n];
    
	uint32 ArrIdx = 0;//遥测一维数组索引(起始)
	uint32 ParaArrIdx = 0;//参数一维数组索引(起始)
    
    if(index >= g_nConfigYcNumZhj[m][x][n])
    {
        return;
    }
    
    int     ret  = -1;
    float32 fTemp1 = 0;
    float32 fTemp2 = 0;
    float32 fTemp3 = 0;

    float32 resultValue = 0;//结果值
    float32 fDeadZone;  //死区值
    float32 fDiffValue;  //差值
    float32 fRatio = 1;  //倍率

    uint8 nTimeFlag = 0;//是否带时标
    OOP_DATETIME_S_T stTime = {0};//数据自身时标
    time_t tCurrentTime; //系统当前时间
	ArrIdx = g_aYcDataIndexZhj[m][x][n];
    ParaArrIdx = g_aParaValueIndexZhj[m][x][n];

    switch(g_aYcDataConfigZhj[ArrIdx+index].nDealType)
	{
		case YC_SP_RT_TMN_WORK_TIME:
		{
			time(&tCurrentTime);  //获取time_t类型的当前时间
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
                if(0 == nItemIndex)//电压正序分量
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
                else if(1 == nItemIndex)//电压负序分量
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
                else if(2 == nItemIndex)//电压零序分量
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
                if(0 == nItemIndex)//电流正序分量
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
                else if(1 == nItemIndex)//电流负序分量
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
                else if(2 == nItemIndex)//电流零序分量
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
            int arrIndexVoltS = query_para_index_config_zhj(m, n, n, nAreaId, 0x00008008);//额定电压
            if(0 == nItemIndex)//A相 电压偏差
            {
                if(arrIndexVoltA>=0 && arrIndexVoltS>=0)
                {
                    resultValue = g_aYcDataSendZhj[ArrIdx+arrIndexVoltA].fNewValue - g_aParaSetValueZhj[nAreaId][ParaArrIdx+arrIndexVoltS].unValue.float32V;
                    ret = 1;
                }
            }
            else if(1 == nItemIndex)//B相 电压偏差
            {
                if(arrIndexVoltB>=0 && arrIndexVoltS>=0)
                {
                    resultValue = g_aYcDataSendZhj[ArrIdx+arrIndexVoltB].fNewValue - g_aParaSetValueZhj[nAreaId][ParaArrIdx+arrIndexVoltS].unValue.float32V;
                    ret = 1;
                }
            }
            else if(2 == nItemIndex)//C相 电压偏差
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
            int arrIndexFrT = query_yc_index_config_zhj(m, n, n, 0x0000001E);//总频率
            int arrIndexFrA = query_yc_index_config_zhj(m, n, n, 0x0000001F);
            int arrIndexFrB = query_yc_index_config_zhj(m, n, n, 0x00000020);
            int arrIndexFrC = query_yc_index_config_zhj(m, n, n, 0x00000021);
            if(0 == nItemIndex)//总
            {
                if(arrIndexFrT>=0)
                {
                    resultValue = g_aYcDataSendZhj[ArrIdx+arrIndexFrT].fNewValue - 50;//额定频率默认50
                    ret = 1;
                }
            }
            else if(1 == nItemIndex)//A频率
            {
                if(arrIndexFrA>=0)
                {
                    resultValue = g_aYcDataSendZhj[ArrIdx+arrIndexFrA].fNewValue - 50;//额定频率默认50
                    ret = 1;
                }
            }
            else if(2 == nItemIndex)//B频率
            {
                if(arrIndexFrB>=0)
                {
                    resultValue = g_aYcDataSendZhj[ArrIdx+arrIndexFrB].fNewValue - 50;//额定频率默认50
                    ret = 1;
                }
            }
            else if(3 == nItemIndex)//C频率
            {
                if(arrIndexFrC>=0)
                {
                    resultValue = g_aYcDataSendZhj[ArrIdx+arrIndexFrC].fNewValue - 50;//额定频率默认50
                    ret = 1;
                }
            }
		}
			break;

        case YC_SP_RT_VOLT_IMBALA:
		{
/* 电压不平衡度国标的计算方法是：负序/正序 */
#ifdef GUO_BIAO
            int arrIndexVoltP = query_yc_index_config_zhj(m, n, n, 0x00000022);//电压正序分量
            int arrIndexVoltN = query_yc_index_config_zhj(m, n, n, 0x00000023);//电压负序分量
            float32 fBaseValue = g_aYcDataSendZhj[ArrIdx+arrIndexVoltP].fNewValue;
            if(arrIndexVoltP>=0 && arrIndexVoltN>=0 && fBaseValue!=0)
            {
                resultValue = g_aYcDataSendZhj[ArrIdx+arrIndexVoltN].fNewValue/fBaseValue*100;//负序/正序 单位%
            }
            
/* 电压不平衡度估算的计算方法是：电压最大值 - 电压最小值 / 电压平均值 */
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
/* 电流不平衡度国标的计算方法是：负序/正序 */
#ifdef GUO_BIAO
            int arrIndexCurrP = query_yc_index_config_zhj(m, n, n, 0x00000025);//电流正序分量
            int arrIndexCurrN = query_yc_index_config_zhj(m, n, n, 0x00000026);//电流负序分量
            float32 fBaseValue = g_aYcDataSendZhj[ArrIdx+arrIndexCurrP].fNewValue;
            if(arrIndexCurrP>=0 && arrIndexCurrN>=0 && fBaseValue!=0)
            {
                resultValue = g_aYcDataSendZhj[ArrIdx+arrIndexCurrN].fNewValue/fBaseValue*100;//负序/正序 单位%
            }
/* 电流不平衡度估算的计算方法是：电流最大值 - 电流最小值 / 电流平均值 */            
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
		    int arrIndexS = query_yc_index_config_zhj(m, n, n, 0x00000013);//视在功率
            int arrIndexCap = query_para_index_config_zhj(m, n, n, nAreaId, 0x0000800B);//容量
            int arrIndexCT = query_para_index_config_zhj(m, n, n, nAreaId, 0x00008006);//CT变比
            int arrIndexPT = query_para_index_config_zhj(m, n, n, nAreaId, 0x00008007);//PT变
            if(arrIndexS>=0 && arrIndexCap>=0 && arrIndexCT>=0 && arrIndexPT>=0)
            {
                float32 fCapValue = g_aParaSetValueZhj[nAreaId][ParaArrIdx+arrIndexCap].unValue.float32V;
                float32 fCtValue = g_aParaSetValueZhj[nAreaId][ParaArrIdx+arrIndexCT].unValue.uint16V;
                float32 fPtValue = g_aParaSetValueZhj[nAreaId][ParaArrIdx+arrIndexPT].unValue.uint16V;
                float32 fBaseValue = fCapValue*1000;
                if(fBaseValue!=0)
                {
                    resultValue = g_aYcDataSendZhj[ArrIdx+arrIndexS].fNewValue*fCtValue*fPtValue/fBaseValue*100;//视在功率/容量 单位%
                    ret = 1;
                }
                
            }
		}
			break;
        case YC_SP_RT_U_HMONIC_ODD:
		{
		    int nItemIndex = g_aYcDataConfigZhj[ArrIdx+index].nItemIndex;
            if(0 == nItemIndex)//A相 电压 奇次谐波
            {
                int arrIndexStart = query_yc_index_config_zhj(m, n, n, 0x00000201);//3次
                int arrIndexEnd = query_yc_index_config_zhj(m, n, n, 0x00000211);//最大19次
                while(arrIndexStart <= arrIndexEnd)
                {
                    resultValue += g_aYcDataSendZhj[ArrIdx+arrIndexStart].fNewValue;
                    arrIndexStart += 2;
                    ret = 1;
                }
            }
            else if(1 == nItemIndex)//B相 电压 奇次谐波
            {
                int arrIndexStart = query_yc_index_config_zhj(m, n, n, 0x00000251);//3次
                int arrIndexEnd = query_yc_index_config_zhj(m, n, n, 0x00000261);//最大19次
                while(arrIndexStart <= arrIndexEnd)
                {
                    resultValue += g_aYcDataSendZhj[ArrIdx+arrIndexStart].fNewValue;
                    arrIndexStart += 2;
                    ret = 1;
                }
            }
            else if(2 == nItemIndex)//C相 电压 奇次谐波
            {
                int arrIndexStart = query_yc_index_config_zhj(m, n, n, 0x000002A1);//3次
                int arrIndexEnd = query_yc_index_config_zhj(m, n, n, 0x000002B1);//最大19次
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
            if(0 == nItemIndex)//A相 电压 偶次谐波
            {
                int arrIndexStart = query_yc_index_config_zhj(m, n, n, 0x00000200);//2次
                int arrIndexEnd = query_yc_index_config_zhj(m, n, n, 0x00000210);//最大18次
                while(arrIndexStart <= arrIndexEnd)
                {
                    resultValue += g_aYcDataSendZhj[ArrIdx+arrIndexStart].fNewValue;
                    arrIndexStart += 2;
                    ret = 1;
                }
            }
            else if(1 == nItemIndex)//B相 电压 偶次谐波
            {
                int arrIndexStart = query_yc_index_config_zhj(m, n, n, 0x00000250);//2次
                int arrIndexEnd = query_yc_index_config_zhj(m, n, n, 0x00000260);//最大18次
                while(arrIndexStart <= arrIndexEnd)
                {
                    resultValue += g_aYcDataSendZhj[ArrIdx+arrIndexStart].fNewValue;
                    arrIndexStart += 2;
                    ret = 1;
                }
            }
            else if(2 == nItemIndex)//C相 电压 偶次谐波
            {
                int arrIndexStart = query_yc_index_config_zhj(m, n, n, 0x000002A0);//2次
                int arrIndexEnd = query_yc_index_config_zhj(m, n, n, 0x000002B0);//最大18次
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

    DB104_FMT_TRACE("m=%d, x=%d, n=%d, <--第%d个遥测--> addr=%08x, dealtype=%08x, yc_calculate_statistics_zhj() ret:%d \n", 
        m, x, n, index, g_aYcDataConfigZhj[ArrIdx+index].nObjAddrU32, g_aYcDataConfigZhj[ArrIdx+index].nDealType, ret);

    //判断结果是否有效
    if(ret > 0)
    {
        g_aYcDataSendZhj[ArrIdx+index].bNewValidFlag = TRUE;
        
        g_aYcDataSendZhj[ArrIdx+index].fNewValue = resultValue*fRatio;
        DB104_FMT_TRACE("倍率处理之后g_aYcDataSend[index].fNewValue=%f, 倍率处理之前fValueDB=%f \n", g_aYcDataSendZhj[ArrIdx+index].fNewValue, resultValue);
        
        //读取变化死区值
        fDeadZone = g_aYcDataConfigZhj[ArrIdx+index].fDeadZoneValue;
        DB104_FMT_TRACE("变化死区值fDeadZone=%f \n", fDeadZone);

        //临时赋值, 测试变化上送
        if(g_bTestEnable)
        {
            g_aYcDataSendZhj[ArrIdx+index].fHisValue = rand()%100;//100以内随机数
            DB104_FMT_TRACE("遥测随机数 g_aYcDataSend[%d].fHisValue=%f \n", index, g_aYcDataSendZhj[ArrIdx+index].fHisValue);
        }

        //判断实时值是否越过死区
        fDiffValue = fabs(g_aYcDataSendZhj[ArrIdx+index].fNewValue - g_aYcDataSendZhj[ArrIdx+index].fHisValue);
        if(fDiffValue > fDeadZone)
        {
            DB104_FMT_TRACE("变化值fDiffValue=%f, 大于死区值, fNewValue=%f, fHisValue=%f 需要更新上报! \n", 
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
            DB104_FMT_TRACE("变化值fDiffValue=%f, 小于死区值, fNewValue=%f, fHisValue=%f \n", 
                fDiffValue, g_aYcDataSendZhj[ArrIdx+index].fNewValue, g_aYcDataSendZhj[ArrIdx+index].fHisValue);
        }
        
    }
    else
    {
        g_aYcDataSendZhj[ArrIdx+index].bNewValidFlag = FALSE;
        DB104_FMT_TRACE("yc_db_nomal_read() 解析数据失败  g_aYcDataConfig[%d].nDbCenterOAD=%08x \n", 
            index, g_aYcDataConfigZhj[ArrIdx+index].nDbCenterOAD);
    }

    if(nTimeFlag > 0)
    {
        DB104_FMT_TRACE("yc_calculate_statistics_zhj() 数据带时标nTimeFlag=%d \n", nTimeFlag);
        ooptm_to_cp56time2a(stTime, &g_aYcDataSendZhj[ArrIdx+index].btime[0]);
    }
    else
    {
        system_time_get(&g_aYcDataSendZhj[ArrIdx+index].btime[0]);
    }
}

/*******************************************************************************
* 函数名称: fault_buff_combine_zhj
* 函数功能: 故障事件报文组帧
* 输入参数: 无
* 输出参数: 无
* 返 回 值: 无
*******************************************************************************/
int fault_buff_combine_zhj(uint8 eqTpIndex, uint8 eqGroupIndex, uint16 eqNoIndex, OOP_PROTECTOR_31BB_T event, TOTAL_104_FRAME_T *pTotal104Frame)
{
    //uint8 addr[4] = {0};//信息体地址
    int index = 0;//偏移位
    uint16 nTempU16 = 0;
    uint32 nTempU32 = 0;
    float32 fTempF32 = 0;
    
    TOTAL_104_FRAME_T stTotal104Frame;
    memset(&stTotal104Frame, 0, sizeof(TOTAL_104_FRAME_T));

    //HEAD
    stTotal104Frame.nHead = 0x68;

    //控制欲(暂时不填)
    
    //数据类型
    stTotal104Frame.nDataType = M_FT_NA_ZHJ_1;

    //VSQ
    stTotal104Frame.nVSQ = 0x00;

    //COT
    stTotal104Frame.nTransCauseL = CAUSE_SPONT;
    stTotal104Frame.nTransCauseH = 0x00;

    //公共地址
    stTotal104Frame.nPubAddrL = eqNoIndex + 1;
    stTotal104Frame.nPubAddrH = (uint8)((uint8)(eqTpIndex+1) << 3) + eqGroupIndex;

    //事件编码
    nTempU16 = 0x0121;
    memcpy(&stTotal104Frame.aObjData[index], &nTempU16, 2);
    index += 2;

    //事件发生时刻
    ooptm_to_cp56time2a(event.tmStart, &stTotal104Frame.aObjData[index]);
    index += 7;

    //遥信个数
    stTotal104Frame.aObjData[index] = 0;
    index += 1;

    //遥测个数
    stTotal104Frame.aObjData[index] = 7;
    index += 1;
    
    nTempU32 = 0x00000011;//1 当前剩余电流
    memcpy(&stTotal104Frame.aObjData[index], &nTempU32, 3);//地址
    index += 3;
    fTempF32 = (float32)event.residualcurrent;
    memcpy(&stTotal104Frame.aObjData[index], &fTempF32, 4);//值
    index += 4;
    stTotal104Frame.aObjData[index] = 0x00;//品质
    index += 1;
    
    nTempU32 = 0x00000001;//2 A相电压
    memcpy(&stTotal104Frame.aObjData[index], &nTempU32, 3);//地址
    index += 3;
    fTempF32 = (float32)event.voltage[0];
    memcpy(&stTotal104Frame.aObjData[index], &fTempF32, 4);//值
    index += 4;
    stTotal104Frame.aObjData[index] = 0x00;//品质
    index += 1;

    nTempU32 = 0x00000002;//3 B相电压
    memcpy(&stTotal104Frame.aObjData[index], &nTempU32, 3);//地址
    index += 3;
    fTempF32 = (float32)event.voltage[1];
    memcpy(&stTotal104Frame.aObjData[index], &fTempF32, 4);//值
    index += 4;
    stTotal104Frame.aObjData[index] = 0x00;//品质
    index += 1;

    nTempU32 = 0x00000003;//4 C相电压
    memcpy(&stTotal104Frame.aObjData[index], &nTempU32, 3);//地址
    index += 3;
    fTempF32 = (float32)event.voltage[2];
    memcpy(&stTotal104Frame.aObjData[index], &fTempF32, 4);//值
    index += 4;
    stTotal104Frame.aObjData[index] = 0x00;//品质
    index += 1;

    nTempU32 = 0x00000004;//5 A相电流
    memcpy(&stTotal104Frame.aObjData[index], &nTempU32, 3);//地址
    index += 3;
    fTempF32 = (float32)event.current[0];
    memcpy(&stTotal104Frame.aObjData[index], &fTempF32, 4);//值
    index += 4;
    stTotal104Frame.aObjData[index] = 0x00;//品质
    index += 1;

    nTempU32 = 0x00000005;//6 B相电流
    memcpy(&stTotal104Frame.aObjData[index], &nTempU32, 3);//地址
    index += 3;
    fTempF32 = (float32)event.current[1];
    memcpy(&stTotal104Frame.aObjData[index], &fTempF32, 4);//值
    index += 4;
    stTotal104Frame.aObjData[index] = 0x00;//品质
    index += 1;

    nTempU32 = 0x00000006;//7 C相电流
    memcpy(&stTotal104Frame.aObjData[index], &nTempU32, 3);//地址
    index += 3;
    fTempF32 = (float32)event.current[2];
    memcpy(&stTotal104Frame.aObjData[index], &fTempF32, 4);//值
    index += 4;
    stTotal104Frame.aObjData[index] = 0x00;//品质
    index += 1;

    //遥测个数(带时标)
    stTotal104Frame.aObjData[index] = 0;
    index += 1;

    //电能量个数
    stTotal104Frame.aObjData[index] = 0;
    index += 1;

    //比特串个数
    stTotal104Frame.aObjData[index] = 2;
    index += 1;
    nTempU32 = 0x00020002;//1 故障相别
    memcpy(&stTotal104Frame.aObjData[index], &nTempU32, 3);//地址
    index += 3;
    stTotal104Frame.aObjData[index] = 1;//长度
    index += 1;
    stTotal104Frame.aObjData[index] = event.faultphase;//值
    index += 1;
	stTotal104Frame.aObjData[index] = 0x00;//品质
    index += 1;
    
    nTempU32 = 0x00020003;//2 运行状态字
    memcpy(&stTotal104Frame.aObjData[index], &nTempU32, 3);//地址
    index += 3;
    stTotal104Frame.aObjData[index] = 1;//长度
    index += 1;
    stTotal104Frame.aObjData[index] = 0x00;//值
    index += 1;
	stTotal104Frame.aObjData[index] = 0x00;//品质
    index += 1;

	//参数个数
    stTotal104Frame.aObjData[index] = 0;
	index += 1;
	
    //更新报文长度(不包括头部和长度自身)
    stTotal104Frame.nLength = CTRL_UNIT_LABEL_LEN + index;

    memcpy(pTotal104Frame, &stTotal104Frame, sizeof(TOTAL_104_FRAME_T));
    return (stTotal104Frame.nLength+2);
}

/*******************************************************************************
* 函数名称: yc_db_event_record_read_zhj
* 函数功能: 从数据中心读取设备事件数据(记录型数据)
* 输入参数: typeIndex         设备类型数组下标, 从0开始
            groupIndex    设备组别数组下标, 从0开始
            seqIndex      组内偏移地址数组下标, 从0开始
            index         点号索引
* 输出参数: 无
* 返 回 值: 无
*******************************************************************************/
void yc_db_event_record_read_zhj(uint8 typeIndex, uint16 groupIndex, uint16 seqIndex, uint16 index)
{
    int m = typeIndex;//设备类型-1
    int x = groupIndex;//设备组别 从0开始
    int n=  seqIndex;//设备序号-1
    int i = 0;//定值下标
    uint16 areaID = g_nFixedAreaIdZhj[m][x][n];//定值区号
    uint32 macAddr = 0;//参数配置中设备地址的点号
    static OOP_DATETIME_S_T tmStart = {0};  //事件发生时间

    /*
	if(index >= g_nConfigYcNumZhj[m][x][n])
	{
		return;
	}
	*/

	uint8	aRecvBuff[500] = {0};
	uint32	nRevcBuffLen = 0;
	int 	ret  = 0;
	READ_RECORD_T ReadRecord;  //记录型OAD
	RECORD_UNIT_T RecordOut;  //记录型查询结果
	memset(&ReadRecord, 0, sizeof(READ_RECORD_T));
	memset(&RecordOut, 0, sizeof(RECORD_UNIT_T));

    uint32 ArrIdx = 0;//一维数组索引(起始)
    ArrIdx = g_aParaValueIndexZhj[m][x][n];
	
	time_t nRealTime_t;  //当前系统时间的转换的秒数

    TOTAL_104_FRAME_T stTotal104Frame;
    memset(&stTotal104Frame, 0, sizeof(TOTAL_104_FRAME_T));
    int nTotal104FrameLen = 0;

    //事件配置参数
    YC_DATA_CONFIG stEventConfig;
    memset(&stEventConfig, 0, sizeof(YC_DATA_CONFIG));
    stEventConfig.nLogicId = 7;
    stEventConfig.nInfoNum = (uint16)((uint16)(m+1) << 11) + (uint16)((uint16)(x) << 8) + (n+1);
    stEventConfig.nDbCenterOAD = 0x31BB0200;
    stEventConfig.nOadColsNum = 1;
    stEventConfig.aOadCols[0] = 0x201F0800;

	//数据读取
	nRealTime_t = time(NULL);
	
	ReadRecord.recordOAD.logicId = stEventConfig.nLogicId;  //逻辑地址
	ReadRecord.recordOAD.infoNum = stEventConfig.nInfoNum;  //信息点号
	ReadRecord.recordOAD.road.oadMain.value = stEventConfig.nDbCenterOAD;  //主OAD
	ReadRecord.recordOAD.road.oadCols.nNum = stEventConfig.nOadColsNum;  //次OAD数量
	memcpy(&ReadRecord.recordOAD.road.oadCols.oad[0], &stEventConfig.aOadCols[0],
		stEventConfig.nOadColsNum*sizeof(uint32));  //关联次OAD
	ReadRecord.recordOAD.classtag = CLASS_DATA_COLLEC;

	ReadRecord.cType = COL_TM_STORE;  //采集存储时间
	//ReadRecord.cStart = nRealTime_t-24*3600;  //时间起始点，当前时间提前一天
	ReadRecord.cStart = 0;  //时间起始点，当前时间提前一天
	ReadRecord.cEnd = nRealTime_t + 8*60*60;  //时间结束点
	ReadRecord.sortType = DOWN_SORT;  //排序方式 降序
	ReadRecord.MAC.nNum = 6;  //附加校验 长度
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
	
	ReadRecord.ti.unit = TI_HOUR;  //时间间隔 单位
	ReadRecord.ti.value = 15;  //时间间隔 值

	DB104_FMT_TRACE("m=%d, n=%d <--第%d个遥测--> yc_db_event_record_read_zhj() 数组下标index=%d, 逻辑地址logicId=%d, 信息点号infoNum=%d, 主OAD=%08x, 次OAD数量=%d, 次OAD=%08x, 校验MAC.nNum=%d, MAC.value=%02x%02x%02x%02x%02x%02x \n", 
		m, n, index, index, ReadRecord.recordOAD.logicId, ReadRecord.recordOAD.infoNum, ReadRecord.recordOAD.road.oadMain.value, ReadRecord.recordOAD.road.oadCols.nNum, 
		ReadRecord.recordOAD.road.oadCols.oad[0].value, ReadRecord.MAC.nNum, ReadRecord.MAC.value[5], ReadRecord.MAC.value[4], ReadRecord.MAC.value[3], ReadRecord.MAC.value[2], ReadRecord.MAC.value[1], ReadRecord.MAC.value[0]);
	
	ret = db_read_record(g_sDataCenterClient, &ReadRecord, sizeof(aRecvBuff), &RecordOut);
	DB104_FMT_TRACE("yc_db_center_read() ret = db_read_record() ret:%d \n", ret);
	if(ret == 0)
	{
		nRevcBuffLen = RecordOut.record[0].outDatalen;
		memcpy(&aRecvBuff[0], &RecordOut.record[0].outDataBuf[0], nRevcBuffLen);

		DB104_FMT_TRACE("yc_db_event_record_read_zhj() 读取数据成功  nDbCenterOAD=%08x \n", 
			stEventConfig.nDbCenterOAD);
		DB104_BUF_TRACE(aRecvBuff, nRevcBuffLen, "报文: ");

		OOP_PROTECTOR_31BB_T event = {0};
		memcpy(&event, aRecvBuff, sizeof(OOP_PROTECTOR_31BB_T));

        if(0 == memcmp(&tmStart, &event.tmStart, sizeof(OOP_DATETIME_S_T)))
        {
            //两次事件的发生时间相同, 认为是同一个事件
        }
        else
        {
            DB104_FMT_TRACE("上次事件发生时间 %d-%d-%d %d:%d:%d \n", tmStart.year, tmStart.month,       tmStart.day, tmStart.hour, tmStart.minute, tmStart.second);
            DB104_FMT_TRACE("本次时间发生时间 %d-%d-%d %d:%d:%d \n", event.tmStart.year, event.tmStart.month, event.tmStart.day, event.tmStart.hour, event.tmStart.minute, event.tmStart.second);
            
            memcpy(&tmStart, &event.tmStart, sizeof(OOP_DATETIME_S_T));
            nTotal104FrameLen = fault_buff_combine_zhj(m, x, n, event, &stTotal104Frame);
            if(nTotal104FrameLen > 0)
            {
                pw_lock_queue_write(&g_stSoeBuff, (uint8 *)&stTotal104Frame, nTotal104FrameLen, 0);
                DB104_BUF_TRACE((uint8 *)&stTotal104Frame, nTotal104FrameLen, "临时FAULT报文:");
            }
        }
	}
	else
	{
	    DB104_FMT_TRACE("yc_db_event_record_read_zhj() 读取数据  nDbCenterOAD=%08x \n", 
			stEventConfig.nDbCenterOAD);
		DB104_FMT_TRACE("yc_db_event_record_read_zhj() 数据读取失败! \n");
	}
}


/*******************************************************************************
* 函数名称: data_db_center_read_zhj
* 函数功能: 从数据中心读取实时数据
* 输入参数: READ_STATE_T ReadState  读取数据计时器
* 输出参数: 无
* 返 回 值: 无
*******************************************************************************/
void data_db_center_read_zhj(READ_STATE_T ReadState)
{
    int m = 0;//设备类型
    int x = 0;//设备组别
    int n = 0;//设备序号
	int i = 0;
    uint32 ArrIdx = 0;//一维数组索引(起始)
	
	//遥信数据读取
	//DB104_FMT_TRACE("\n************data_db_center_read() 遥信数据读取 start...************ \n\n");
    for(m=0; m<MAX_EQUIP_TYPE_NUM; m++)//类型
    {
        if(0 == g_aGroupCountZhj[m])
        {
            continue;//该类型的组数量为0
        }
        
        for(x=0; x<g_aGroupCountZhj[m]; x++)//组号
        {
            if(0 == g_aEquipCountZhj[m][x])
            {
                continue;//该类型的设备数量为0
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
                                //由主动查询改为事件触发处理
                                //yx_real_signal_read_zhj(m, x, n, i);//硬遥信
                            }
            			}
            			else
            			{
            			    if(1 == ReadState.nCommonYxFlag)
                            {
                                //yx_calculate_statistics_zhj(m, x, n, i);//统计数据, 非实际采集
                            }
            			}
            		}
            		else
            		{
            		    if(1 == ReadState.nCommonYxFlag)
                        {
                            //yx_db_center_read_zhj(m, x, n, i);//从数据中心读取数据
                        }
            		}
            	}
            }
        }
    }

	//遥测数据读取
	//DB104_FMT_TRACE("\n************data_db_center_read() 遥测数据读取 start...************ \n\n");
    for(m=0; m<MAX_EQUIP_TYPE_NUM; m++)
    {
        if(0 == g_aGroupCountZhj[m])
        {
            continue;//该类型的组数量为0
        }
        
        for(x=0; x<g_aGroupCountZhj[m]; x++)//组号
        {
            if(0 == g_aEquipCountZhj[m][x])
            {
                continue;//该类型的设备数量为0
            }

            for(n=0; n<g_aEquipCountZhj[m][x]; n++)
            {
                ArrIdx = g_aYcDataIndexZhj[m][x][n];
                for(i=0; i<g_nConfigYcNumZhj[m][x][n]; i++)
            	{
            	    if(M_VBO_TB_1 == g_aYcDataConfigZhj[ArrIdx+i].nSignalType)//比特串数据
                    {
                        //比特串数据
                        if(g_aYcDataConfigZhj[ArrIdx+i].nDbCenterOAD != 0xFFFFFFFF)
                        {
                            if(g_aYcDataConfigZhj[ArrIdx+i].nLogicId == 7)//7 配网设备
                			{
                			    if(g_aYcDataConfigZhj[ArrIdx+i].nOadColsNum > 0)//次OAD数量要大于0
                                {
                                    if(1 == ReadState.nRecordYcFlag)
                                    {
                                        yc_db_record_str_read_zhj(m, x, n, i);
                                    }
                                }
                    	    }
                        }
                    }
                    else//浮点型数据
                    {
                        if(g_aYcDataConfigZhj[ArrIdx+i].nDbCenterOAD == 0xFFFFFFFF)
                		{
                		    if(1 == ReadState.nCommonYcFlag)
                            {
                                //尚不知如何处理的数据
                            }
                		}
                        else if( (YC_SP_RT_START < g_aYcDataConfigZhj[ArrIdx+i].nDealType) 
                                    && (YC_SP_RT_END > g_aYcDataConfigZhj[ArrIdx+i].nDealType) )
                        {
                            if(1 == ReadState.nCommonYcFlag)
                            {
                                yc_calculate_statistics_zhj(m, x, n, i);//实时统计数据, 非实际采集
                            }
                        } 
                        else if( (YC_SP_PRE_DAY_START < g_aYcDataConfigZhj[ArrIdx+i].nDealType) 
                                    && (YC_SP_PRE_DAY_END > g_aYcDataConfigZhj[ArrIdx+i].nDealType) )
                        {
                            if(1 == ReadState.nPreDayYcFlag)
                            {
                                //yc_db_sp_record_read_zhj(m, x, n, i);//当日统计数据
                            }
                        }
                        else if( (YC_SP_PRE_MON_START < g_aYcDataConfigZhj[ArrIdx+i].nDealType) 
                                    && (YC_SP_PRE_MON_END > g_aYcDataConfigZhj[ArrIdx+i].nDealType) )
                        {
                            if(1 == ReadState.nPreMonYcFlag)
                            {
                                //yc_db_sp_record_read_zhj(m, x, n, i);//当月统计数据
                            }
                        }
                        else if ( ((YC_SP_ON_DAY_START < g_aYcDataConfigZhj[ArrIdx+i].nDealType) 
                                    && (YC_SP_ON_DAY_END > g_aYcDataConfigZhj[ArrIdx+i].nDealType)) || 
                                   ((YC_SP_ON_MON_START < g_aYcDataConfigZhj[ArrIdx+i].nDealType) 
                                    && (YC_SP_ON_MON_END > g_aYcDataConfigZhj[ArrIdx+i].nDealType)) )
                        {
                            /* 上日上月的统计数据，电表里配置了，为了避免数据更新延迟，这里为了满足测试，暂不处理 */
                        }
                        else if (YC_SP_INVALID == g_aYcDataConfigZhj[ArrIdx+i].nDealType)
                        {
                            /* 忽略数据 */
                        }
                        else
                        {
                            if(g_aYcDataConfigZhj[ArrIdx+i].nLogicId == 1)//交采数据
                            {
                                if( (YC_SP_HARMONIC_START < g_aYcDataConfigZhj[ArrIdx+i].nDealType) 
                                    && (YC_SP_HARMONIC_END > g_aYcDataConfigZhj[ArrIdx+i].nDealType) )
                                {
                                    if(1 == ReadState.nHarmonicYcFlag)
                                    {
                                        yc_db_nomal_read_zhj(m, x, n, i);//交采谐波
                                    }
                                }
                                else if( (YC_SP_DEMAND_START < g_aYcDataConfigZhj[ArrIdx+i].nDealType) 
                                            && (YC_SP_DEMAND_END > g_aYcDataConfigZhj[ArrIdx+i].nDealType) )
                                {
                                    if(1 == ReadState.nDemandYcFlag)
                                    {
                                        yc_db_nomal_read_zhj(m, x, n, i);//交采需量
                                    }
                                }
                                else
                                {
                                    if(1 == ReadState.nCommonYcFlag)
                                    {
                                        yc_db_nomal_read_zhj(m, x, n, i);//交采实时值
                                    }
                                }
                			}
                			else if(g_aYcDataConfigZhj[ArrIdx+i].nLogicId == 0 || 
                                g_aYcDataConfigZhj[ArrIdx+i].nLogicId == 7)//0 电表数据;7 配网设备
                			{
                			    if(g_aYcDataConfigZhj[ArrIdx+i].nOadColsNum > 0)//次OAD数量要大于0
                                {
                                    if(1 == ReadState.nRecordYcFlag)
                                    {
                                        yc_db_record_read_zhj(m, x, n, i);//记录型数据
                                    }
                                }
                    	    }
                		}
                    }
            	}

                usleep(10000);//每个设备读取完后休眠一下
            }
        }
    }

    //特殊数据读取
    if(1 == ReadState.nRecordYcFlag)
    {
        yc_db_event_record_read_zhj(8, 0, 0, 0);//漏保事件读取
    }
    
}


