/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief： 功能描述
* @date： 2020-01-03
*********************************************************************
*/

#include "pw_common.h"
#include "pw_lib.h"
#include "pw_protocol.h"
#include "pw_debug.h"

//调节日志路径
char *g_pRecordPathMain = {"/data/app/advPdAmr/PwRecordPathMain.log"};//主线程调节日志
char *g_pRecordPathUpRecv = {"/data/app/advPdAmr/PwRecordPathUpRecv.log"};//上行接收调节日志
char *g_pRecordPathUpSend = {"/data/app/advPdAmr/PwRecordPathUpSend.log"};//上行发送调节日志
char *g_pRecordPathDownDB = {"/data/app/advPdAmr/PwRecordPathDownDB.log"};//下行数据中心交互日志

//变量保存私有文件名
char *g_pFixedAreaName = {"/data/app/advPdAmr/PwFixedAreaName"};  //定值区特性私有文件
char *g_pParamValueName = {"/data/app/advPdAmr/PwParamValueName"};  //定值参数私有文件

/*******************************************************************************
* 全局变量
*******************************************************************************/
DB_CLIENT g_sDataCenterClient;  //和数据中心交互的句柄(新)
DB_CLIENT g_sEventDbClient;  //和数据中心交互的句柄(事件使用)
METER_FILE_T g_sMeterFileAll;   //终端档案信息
BOOL g_bTestEnable = FALSE;  //遥测遥信电量测试数据开关


/*******************************************************************************
* 计时相关变量声明
*******************************************************************************/
PAST_TIME_FLAG_T             g_unPastTimeFlag;                           //过时间标志
DATE_TIME_T                  g_stCurBinTime;                             //当前BIN时间
DATE_TIME_T                  g_stCurBcdTime;                             //当前BCD时间
DATE_TIME_T                  g_stPreBinTime;                             //上次BIN时间

/*******************************************************************************
* 计时相关变量声明
*******************************************************************************/
PAST_TIME_FLAG_T             g_unDownPastTimeFlag;                           //过时间标志
DATE_TIME_T                  g_stDownCurBinTime;                             //当前BIN时间
DATE_TIME_T                  g_stDownCurBcdTime;                             //当前BCD时间
DATE_TIME_T                  g_stDownPreBinTime;                             //上次BIN时间

/*******************************************************************************
* 计时相关变量声明
*******************************************************************************/
PAST_TIME_FLAG_T             g_eMtPastTimeFlag;                            //过时间标志
DATE_TIME_T                  g_stMtCurBinTime;                             //当前BIN时间
DATE_TIME_T                  g_stMtCurBcdTime;                             //当前BCD时间
DATE_TIME_T                  g_stMtPreBinTime;                             //上次BIN时间

/*******************************************************************************
* 函数名称: param_data_init
* 函数功能: 定值参数初始化
* 输入参数: 无
* 输出参数: 无
* 返 回 值: 无
*******************************************************************************/
void param_data_init()
{
	int i = 0;
	int j = 0;
	int ret = 0;

	//全局变量初始化
	memset(g_aYxDataSend, 0, sizeof(g_aYxDataSend));
	memset(g_aYcDataSend, 0, sizeof(g_aYcDataSend));
	memset(g_aEnergyDataSend, 0, sizeof(g_aEnergyDataSend));
	memset(g_aYkDataSend, 0, sizeof(g_aYkDataSend));
	memset(g_aFaultDataSend, 0, sizeof(g_aFaultDataSend));

	//读取定值区特性
	ret = read_pdata_xram(g_pFixedAreaName, (char *)&g_aFixedValueArea, 0, sizeof(g_aFixedValueArea));
	if(ret < 0)
    {
		FRAME104_FMT_DEBUG("读取本地定值区特性失败  read_pdata_xram(g_pFixedAreaName) error! ret=%d \n", ret);
	}

	//读取定值参数
	ret = read_pdata_xram(g_pParamValueName, (char *)&g_aParaSetValue, 0, sizeof(g_aParaSetValue));
    if(ret < 0)
    {
    	FRAME104_FMT_DEBUG("读取本地定值参数失败  read_pdata_xram(g_pParamValueName) error! ret=%d \n", ret);
		FRAME104_FMT_DEBUG("加载默认定值参数  g_aParaValueInit \n");
		for(i=0; i<MAX_FIXED_AREA_NUM; i++)
		{
			for(j=0; j<g_nConfigParaNum; j++)
			{
				memcpy(&g_aParaSetValue[i][j], &g_aParaValueInit[j], sizeof(PARA_SET_VALUE_T));
			}
		}
	}

	//定值参数初始化加载打印信息
	for(i=0; i<MAX_FIXED_AREA_NUM; i++)
	{
		FRAME104_FMT_DEBUG("param_data_init() 定值区编号=%d, 定值参数详细信息: \n", i);
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
* 函数名称: task_run_step_init
* 函数功能: 104任务执行流程全局变量初始化
* 输入参数: 无
* 输出参数: 无
* 返 回 值: 无
*******************************************************************************/
void task_run_step_init()
{
    //104通用流程
    memset(&g_sTaskRunStep, 0x00, sizeof(g_sTaskRunStep));
	g_sTaskRunStep.nTaskStep = STEP_CONT_DEFAULT;
    g_sTaskRunStep.nStartNum = 0;
    g_sTaskRunStep.bFirstRun = 1;
}

/*******************************************************************************
* 函数名称: meter_file_by_pn_read
* 函数功能: 根据测量点号从数据中心读取档案信息
* 输入参数: 无
* 输出参数: 无
* 返 回 值: 无
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
* 函数名称: meter_file_init
* 函数功能: 初始化电表档案
* 输入参数: 无
* 输出参数: 无
* 返 回 值: 无
*******************************************************************************/
uint16 meter_file_init(METER_FILE_T *pMeterFileAll)
{
	uint16 MeterCount = 0;
    uint16 tmpId = 0;
    uint8 MeterNum = 0;     //档案个数
    int ret = 0;
    
    OOP_METER_T gMeterFile;     //每个电表的档案信息

    memset(pMeterFileAll, 0x00, sizeof(METER_FILE_T));
	
    FRAME104_FMT_DEBUG("开始档案初始化! g_sDataCenterClient=%08x \n", g_sDataCenterClient);
    //pthread_rwlock_wrlock(&pMeterFileAll->rwLock);

	//根据tmpId读取电表档案
    while (tmpId <= METER_MAX_NUM)
    {
        tmpId++;
        memset(&gMeterFile, 0x00, sizeof(OOP_METER_T));
        ret = meter_file_by_pn_read(tmpId, &gMeterFile);
        if(ret < 0)
        {
			continue;
		}

		
		FRAME104_FMT_TRACE("sizeof(OOP_METER_T)=%d, 电表序号=%d, ret=%d. \n", sizeof(OOP_METER_T), tmpId, ret);
		FRAME104_FMT_TRACE("电表序号=%d, 端口号=%08x, 协议类型=%d, 接线方式=%d. \n", tmpId, gMeterFile.basic.port.value, gMeterFile.basic.protocol, gMeterFile.basic.pwrType);
		FRAME104_FMT_TRACE("电表地址=0X%02x%02x%02x%02x%02x%02x. \n", gMeterFile.basic.tsa.add[5], gMeterFile.basic.tsa.add[4], gMeterFile.basic.tsa.add[3], gMeterFile.basic.tsa.add[2], gMeterFile.basic.tsa.add[1], gMeterFile.basic.tsa.add[0]);

		MeterNum = pMeterFileAll->MeterCount;
        pMeterFileAll->MeterFile[MeterNum] = gMeterFile;
        pMeterFileAll->MeterCount++;
    }
	
    MeterCount = pMeterFileAll->MeterCount;
    FRAME104_FMT_DEBUG("档案初始化完成 档案个数为 %d \n",pMeterFileAll->MeterCount);
    //pthread_rwlock_unlock(&pMeterFileAll->rwLock);
    return MeterCount;
}

/*******************************************************************************
* 函数名称: config_xml_load
* 函数功能: 加载xml点表信息
* 输入参数: 无
* 输出参数: 无
* 返 回 值: 无
*******************************************************************************/
int fixed_para_set()
{
	int i = 0;

	for(i=0; i<g_nConfigParaNum; i++)
	{
		switch(g_aParaValueInit[i].nDealType)
		{
			case CALC_PARA_TMN_TYPE://配变终端型号
			{
				g_aParaValueInit[i].nTagType = TAG_STRING;
				char datatemp[] = "TMN-2020";
				snprintf((void *)&g_aParaValueInit[i].unValue.arrayV[0], MAX_FIXED_VALUE_LEN, datatemp);
				g_aParaValueInit[i].nDataLen = sizeof(datatemp);
			}
				break;
			case CALC_PARA_TMN_OPSYS://终端操作系统
			{
				g_aParaValueInit[i].nTagType = TAG_STRING;
				char datatemp[] = "Linux3.10.108";
				snprintf((void *)&g_aParaValueInit[i].unValue.arrayV[0], MAX_FIXED_VALUE_LEN, datatemp);
				g_aParaValueInit[i].nDataLen = sizeof(datatemp);
			}
				break;
			case CALC_PARA_TMN_MAKER://终端制造商
			{
				g_aParaValueInit[i].nTagType = TAG_STRING;
				char datatemp[] = "XLDZ";
				snprintf((void *)&g_aParaValueInit[i].unValue.arrayV[0], MAX_FIXED_VALUE_LEN, datatemp);
				g_aParaValueInit[i].nDataLen = sizeof(datatemp);
			}
				break;
			case CALC_PARA_TMN_HARD://终端硬件版本
			{
				g_aParaValueInit[i].nTagType = TAG_STRING;
				char datatemp[] = "HV00.01";
				snprintf((void *)&g_aParaValueInit[i].unValue.arrayV[0], MAX_FIXED_VALUE_LEN, datatemp);
				g_aParaValueInit[i].nDataLen = sizeof(datatemp);
			}
				break;
			case CALC_PARA_TMN_SOFT://终端软件版本
			{
				g_aParaValueInit[i].nTagType = TAG_STRING;
				char datatemp[] = "HV00.01";
				snprintf((void *)&g_aParaValueInit[i].unValue.arrayV[0], MAX_FIXED_VALUE_LEN, datatemp);
				g_aParaValueInit[i].nDataLen = sizeof(datatemp);
			}
				break;
			case CALC_PARA_TMN_CHECK://终端软件版本校验码
			{
				g_aParaValueInit[i].nTagType = TAG_STRING;
				g_aParaValueInit[i].unValue.arrayV[0] = 0x11;
				g_aParaValueInit[i].unValue.arrayV[1] = 0x22;
				g_aParaValueInit[i].nDataLen = 2;
			}
				break;
			case CALC_PARA_TMN_PROTOCOL://终端通信规约类型
			{
				g_aParaValueInit[i].nTagType = TAG_STRING;
				char datatemp[] = "2009版标准104";
				snprintf((void *)&g_aParaValueInit[i].unValue.arrayV[0], MAX_FIXED_VALUE_LEN, datatemp);
				g_aParaValueInit[i].nDataLen = sizeof(datatemp);
			}
				break;
			case CALC_PARA_TMN_MODEL://终端出厂型号
			{
				g_aParaValueInit[i].nTagType = TAG_STRING;
				char datatemp[] = "XLDZ-2020-1";
				snprintf((void *)&g_aParaValueInit[i].unValue.arrayV[0], MAX_FIXED_VALUE_LEN, datatemp);
				g_aParaValueInit[i].nDataLen = sizeof(datatemp);
			}
				break;
			case CALC_PARA_TMN_ID://终端ID号
			{
				g_aParaValueInit[i].nTagType = TAG_STRING;
				char datatemp[] = "XLDZ-2020-06-10-0001";
				snprintf((void *)&g_aParaValueInit[i].unValue.arrayV[0], MAX_FIXED_VALUE_LEN, datatemp);
				g_aParaValueInit[i].nDataLen = sizeof(datatemp);
			}
				break;
			case CALC_PARA_TMN_MAC://终端网卡MAC地址
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
* 函数名称: config_xml_load
* 函数功能: 加载xml点表信息
* 输入参数: 无
* 输出参数: 无
* 返 回 值: 无
*******************************************************************************/
int config_xml_load()
{
	
	g_nConfigYxNum = 0;  //遥信点表实际配置个数
	g_nConfigYcNum = 0;  //遥测点表实际配置个数
	g_nConfigYkNum = 0;  //遥控点表实际配置个数
	g_nConfigEnNum = 0;  //电量点表实际配置个数
	g_nConfigFaultNum = 0;  //故障事件点表实际配置个数
	g_nConfigParaNum = 0;  //参数点表实际配置个数

	//配置表初始化
	memset(&g_aYxDataConfig[0], 0x00, sizeof(g_aYxDataConfig));
	memset(&g_aYcDataConfig[0], 0x00, sizeof(g_aYcDataConfig));
	memset(&g_aEnergyDataConfig[0], 0x00, sizeof(g_aEnergyDataConfig));
	memset(&g_aYkDataConfig[0], 0x00, sizeof(g_aYkDataConfig));
	memset(&g_aParaValueInit[0], 0x00, sizeof(g_aParaValueInit));
	memset(&g_aParaSetValue[0], 0x00, sizeof(g_aParaSetValue));
	memset(&g_aFaultDataConfig[0], 0x00, sizeof(g_aFaultDataConfig));

	//加载点表
	advPd_readyxfromXML(&g_nConfigYxNum);
	advPd_readycfromXML(&g_nConfigYcNum);
	advPd_readenfromXML(&g_nConfigEnNum);
	advPd_readykfromXML(&g_nConfigYkNum);
	advPd_readfaultfromXML(&g_nConfigFaultNum);
	advPd_readParafromXML(&g_nConfigParaNum);

	//参数列表固有参数赋值
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
	
	FRAME104_FMT_DEBUG("******点表XML加载: 遥信个数:%d, 遥测个数:%d, 电量个数:%d, 遥控个数:%d, 事件个数:%d, 参数个数:%d \n", 
		g_nConfigYxNum, g_nConfigYcNum, g_nConfigEnNum, g_nConfigYkNum, g_nConfigFaultNum, g_nConfigParaNum);

	return 1;
}

/*******************************************************************************
* 函数名称: global_init
* 函数功能: 进程初始化总函数
* 输入参数: 无
* 输出参数: 无
* 返 回 值: 无
*******************************************************************************/
int global_init()
{
    param_data_init();  //定值参数初始化
	past_time_init();  //时间管理初始化
	task_run_step_init();  //104任务执行流程全局变量初始化
	
	return 1;
}

/*******************************************************************************
* 函数名称: meter_file_change
* 函数功能: 档案变更的处理函数
* 输入参数: choice 1 表示档案变更 新增或者改变; choice 0 表示档案删除 gMeterFile中nindex为删除的序号
* 输出参数: 无
* 返 回 值: 无
*******************************************************************************/
void meter_file_change(METER_FILE_T *pMeterFileAll, uint8 choice, OOP_METER_T gMeterFile)
{
    uint16 i;
    int index = -1;
    
    //pthread_rwlock_wrlock(&pMeterFileAll->rwLock);

    DB104_FMT_TRACE("meter_file_change() 进入档案修改流程 choice=%d \n",choice);
	DB104_FMT_TRACE("电表序号=%d, 端口号=%08x, 协议类型=%d, 接线方式=%d. \n", gMeterFile.nIndex, gMeterFile.basic.port.value, gMeterFile.basic.protocol, gMeterFile.basic.pwrType);
	DB104_FMT_TRACE("电表地址=0X%02x%02x%02x%02x%02x%02x. \n", gMeterFile.basic.tsa.add[5], gMeterFile.basic.tsa.add[4], gMeterFile.basic.tsa.add[3], gMeterFile.basic.tsa.add[2], gMeterFile.basic.tsa.add[1], gMeterFile.basic.tsa.add[0]);
	
    if(choice == 0)
    {
        for(i=0; i<pMeterFileAll->MeterCount; i++)
        {
            if(pMeterFileAll->MeterFile[i].nIndex == gMeterFile.nIndex)
            {
                index = i;
                memset(&pMeterFileAll->MeterFile[i], 0x00, sizeof(OOP_METER_T));
				DB104_FMT_TRACE("meter_file_change() 进入档案修改流程 删除档案成功! \n");
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
            //如果当前档案中找到相同的 则直接替换
            if(pMeterFileAll->MeterFile[i].nIndex == gMeterFile.nIndex)
            {
                index = i;
                memcpy(&pMeterFileAll->MeterFile[i],&gMeterFile,sizeof(OOP_METER_T));
				DB104_FMT_TRACE("meter_file_change() 进入档案修改流程 更新档案成功! \n");
                break;
            }
        }
        //如果没有找到 则新增
        if(index == -1)
        {
            pMeterFileAll->MeterFile[pMeterFileAll->MeterCount] = gMeterFile;
            pMeterFileAll->MeterCount++;
			DB104_FMT_TRACE("meter_file_change() 进入档案修改流程 增加档案成功! \n");
        }
    }
    DB104_FMT_TRACE("meter_file_change() 修改之后档案个数=%d \n", pMeterFileAll->MeterCount);
    //pthread_rwlock_unlock(&pMeterFileAll->rwLock);

}


/*******************************************************************************
* 函数名称: files_update_event_deal
* 函数功能: 数据中心档案更新事件处理函数
* 输入参数: 无
* 输出参数: 无
* 返 回 值: 无
*******************************************************************************/
int files_update_event_deal(DB_UPDATE_MSG_T *pUpdateMsg, METER_FILE_T *pMeterFileAll)
{
	int ret = 0;
	OOP_METER_T stMeterFile;
	
	//0xffff表示所有档案删除
    if(pUpdateMsg->infoNum == 0xffff)
    {
        DB104_FMT_TRACE("files_update_event_deal() 档案清空! \n");
        pMeterFileAll->MeterCount = 0;
        memset(pMeterFileAll->MeterFile, 0x00, sizeof(OOP_METER_T)*METER_MAX_NUM);
    }
	else
    {
        memset(&stMeterFile, 0x00, sizeof(OOP_METER_T));
        ret = meter_file_by_pn_read(pUpdateMsg->infoNum, &stMeterFile);
        DB104_FMT_TRACE("files_update_event_deal() 读取档案: infoNum=%d, ret=%d !\n", pUpdateMsg->infoNum, ret);
        if(ret > 0)
        {
            //读取到档案信息 表示档案变更或者新增
            meter_file_change(pMeterFileAll, 1, stMeterFile);
        }
        else
        {
            //未读取到档案信息，表示档案删除
            stMeterFile.nIndex = pUpdateMsg->infoNum;
            meter_file_change(pMeterFileAll, 0, stMeterFile);
        }
    }
	return 1;
}

/*******************************************************************************
* 函数名称: past_time_init
* 函数功能: 时间过处理初始化
* 输入参数: 无
* 输出参数: 无
* 返 回 值: 无
*******************************************************************************/
void past_time_init()
{
    cur_bin_time_get(&g_stCurBinTime);
    memcpy(&g_stPreBinTime, &g_stCurBinTime, sizeof(DATE_TIME_T));
}

/*******************************************************************************
* 函数名称: past_time_init_down
* 函数功能: 时间过处理初始化
* 输入参数: 无
* 输出参数: 无
* 返 回 值: 无
*******************************************************************************/
void past_time_init_down()
{
    cur_bin_time_get(&g_stDownCurBinTime);
    memcpy(&g_stDownPreBinTime, &g_stDownCurBinTime, sizeof(DATE_TIME_T));
}

/*******************************************************************************
* 函数名称: TimeManage
* 函数功能: 时间管理函数
* 输入参数: 无
* 输出参数: 无
* 返 回 值: 无
*******************************************************************************/
void time_manage()
{
    g_unPastTimeFlag.Word = 0;                   //清除时间标志

    cur_bin_time_get(&g_stCurBinTime);              //刷新当前HEX码时间
    cur_bcd_time_get(&g_stCurBcdTime);              //刷新当前BCD码时间


    //过月
    if (g_stCurBinTime.Month != g_stPreBinTime.Month)
    {
        //同时产生过月、过日、过时、过分、过秒标志
        g_unPastTimeFlag.BitSect.Month = TRUE;
        g_unPastTimeFlag.BitSect.Day = TRUE;
        g_unPastTimeFlag.BitSect.Hour = TRUE;
        g_unPastTimeFlag.BitSect.Minute = TRUE;
        g_unPastTimeFlag.BitSect.Second = TRUE;
    }
    //过日
    if (g_stCurBinTime.Day != g_stPreBinTime.Day)
    {
        //同时产生过日、过时、过分标志
        g_unPastTimeFlag.BitSect.Day = TRUE;
        g_unPastTimeFlag.BitSect.Hour = TRUE;
        g_unPastTimeFlag.BitSect.Minute = TRUE;
        g_unPastTimeFlag.BitSect.Second = TRUE;
    }
    //过时
    else if (g_stCurBinTime.Hour != g_stPreBinTime.Hour)
    {
        //同时产生过时、过分标志
        g_unPastTimeFlag.BitSect.Hour = TRUE;
        g_unPastTimeFlag.BitSect.Minute = TRUE;
        g_unPastTimeFlag.BitSect.Second = TRUE;
    }
    //过分
    else if (g_stCurBinTime.Minute != g_stPreBinTime.Minute)
    {
        //产生过分标志
        g_unPastTimeFlag.BitSect.Minute = TRUE;
        g_unPastTimeFlag.BitSect.Second = TRUE;
    }

    //过秒
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
* 函数名称: TimeManage
* 函数功能: 时间管理函数
* 输入参数: 无
* 输出参数: 无
* 返 回 值: 无
*******************************************************************************/
void time_manage_down()
{
    g_unDownPastTimeFlag.Word = 0;                   //清除时间标志

    cur_bin_time_get(&g_stDownCurBinTime);              //刷新当前HEX码时间
    cur_bcd_time_get(&g_stDownCurBcdTime);              //刷新当前BCD码时间


    //过月
    if (g_stDownCurBinTime.Month != g_stDownPreBinTime.Month)
    {
        //同时产生过月、过日、过时、过分、过秒标志
        g_unDownPastTimeFlag.BitSect.Month = TRUE;
        g_unDownPastTimeFlag.BitSect.Day = TRUE;
        g_unDownPastTimeFlag.BitSect.Hour = TRUE;
        g_unDownPastTimeFlag.BitSect.Minute = TRUE;
        g_unDownPastTimeFlag.BitSect.Second = TRUE;
    }
    //过日
    if (g_stDownCurBinTime.Day != g_stDownPreBinTime.Day)
    {
        //同时产生过日、过时、过分标志
        g_unDownPastTimeFlag.BitSect.Day = TRUE;
        g_unDownPastTimeFlag.BitSect.Hour = TRUE;
        g_unDownPastTimeFlag.BitSect.Minute = TRUE;
        g_unDownPastTimeFlag.BitSect.Second = TRUE;
    }
    //过时
    else if (g_stDownCurBinTime.Hour != g_stDownPreBinTime.Hour)
    {
        //同时产生过时、过分标志
        g_unDownPastTimeFlag.BitSect.Hour = TRUE;
        g_unDownPastTimeFlag.BitSect.Minute = TRUE;
        g_unDownPastTimeFlag.BitSect.Second = TRUE;
    }
    //过分
    else if (g_stDownCurBinTime.Minute != g_stDownPreBinTime.Minute)
    {
        //产生过分标志
        g_unDownPastTimeFlag.BitSect.Minute = TRUE;
        g_unDownPastTimeFlag.BitSect.Second = TRUE;
    }

    //过秒
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
* 函数名称: TimeManage
* 函数功能: 时间管理函数
* 输入参数: 无
* 输出参数: 无
* 返 回 值: 无
*******************************************************************************/
void time_manage_mt()
{
    g_eMtPastTimeFlag.Word = 0;                   //清除时间标志

    cur_bin_time_get(&g_stMtCurBinTime);              //刷新当前HEX码时间
    cur_bcd_time_get(&g_stMtCurBcdTime);              //刷新当前BCD码时间


    //过月
    if (g_stMtCurBinTime.Month != g_stMtPreBinTime.Month)
    {
        //同时产生过月、过日、过时、过分、过秒标志
        g_eMtPastTimeFlag.BitSect.Month = TRUE;
        g_eMtPastTimeFlag.BitSect.Day = TRUE;
        g_eMtPastTimeFlag.BitSect.Hour = TRUE;
        g_eMtPastTimeFlag.BitSect.Minute = TRUE;
        g_eMtPastTimeFlag.BitSect.Second = TRUE;
    }
    //过日
    if (g_stMtCurBinTime.Day != g_stMtPreBinTime.Day)
    {
        //同时产生过日、过时、过分标志
        g_eMtPastTimeFlag.BitSect.Day = TRUE;
        g_eMtPastTimeFlag.BitSect.Hour = TRUE;
        g_eMtPastTimeFlag.BitSect.Minute = TRUE;
        g_eMtPastTimeFlag.BitSect.Second = TRUE;
    }
    //过时
    else if (g_stMtCurBinTime.Hour != g_stMtPreBinTime.Hour)
    {
        //同时产生过时、过分标志
        g_eMtPastTimeFlag.BitSect.Hour = TRUE;
        g_eMtPastTimeFlag.BitSect.Minute = TRUE;
        g_eMtPastTimeFlag.BitSect.Second = TRUE;
    }
    //过分
    else if (g_stMtCurBinTime.Minute != g_stMtPreBinTime.Minute)
    {
        //产生过分标志
        g_eMtPastTimeFlag.BitSect.Minute = TRUE;
        g_eMtPastTimeFlag.BitSect.Second = TRUE;
    }

    //过秒
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
* 函数名称: data_block_split
* 函数功能: 数据块拆分
* 输入参数: 无
* 输出参数: 无
* 返 回 值: 无
*******************************************************************************/
int data_block_split(uint32 nOAD, uint8 nItemID, uint8 *buff, float32 *fOut)
{
	uint16 nNum = 0;

	switch(nOAD)
	{
		case 0x00100200:			//(当前)正向有功电能总尖峰平谷示值
        case 0x00200200:			//(当前)反向有功电能总尖峰平谷示值
        case 0x00300200:			//(当前)正向无功电能总尖峰平谷示值
        case 0x00400200:			//(当前)反向无功电能总尖峰平谷示值
        case 0x00500200:			//(当前)第一象限无功总尖峰平谷电能示值
        case 0x00600200:			//(当前)第二象限无功总尖峰平谷电能示值
        case 0x00700200:			//(当前)第三象限无功总尖峰平谷电能示值
        case 0x00800200:			//(当前)第四象限无功总尖峰平谷电能示值
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
		case 0x20010400:			//(当前)零序电流
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
			*fOut = (float32)Frequency;
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
		
		default:
			return -1;
            break;
	}

	return 1;
}

/*******************************************************************************
* 函数名称: para_query_float
* 函数功能: 定值参数查询
* 输入参数: 无
* 输出参数: 无
* 返 回 值: 无
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
* 函数名称: para_query_float
* 函数功能: 定值参数查询
* 输入参数: 无
* 输出参数: 无
* 返 回 值: 无
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
			DB104_BUF_TRACE(&g_aParaSetValue[g_nFixedValueArea][i].unValue.arrayV[0], MAX_FIXED_VALUE_LEN, "报文: ");
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
* 函数名称: yc_query_float32
* 函数功能: 遥测实时值查询
* 输入参数: 无
* 输出参数: 无
* 返 回 值: 无
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
* 函数名称: system_time_get
* 函数功能: 取系统时钟
* 输入参数: 无
* 输出参数: 无
* 返 回 值: 无
*******************************************************************************/
void system_time_get(uint8 *aTime)
{
	struct timeval c_time;
	SYSTEM_BCD_TIME_T  sSysBcdTime;  //系统时间
	uint16  dtemp = 0;

	gettimeofday(&c_time, NULL);
	sys_bcd_time_get(&sSysBcdTime);
	dtemp = ((uint16)(bcd_to_hex(sSysBcdTime.ss)))*1000 + c_time.tv_usec/1000;

	aTime[0] =  dtemp&0xff;
	aTime[1] =  dtemp>>8;
	aTime[2] =  (bcd_to_hex(sSysBcdTime.nn))&0x3f;//高两位保留
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
* 函数名称: ratio_convert
* 函数功能: 真实值-OAD值，OAD值-真实值，变换系数
* 输入参数: 无
* 输出参数: 无
* 返 回 值: 无
*******************************************************************************/
const Map645ToOop* oad_info_find(uint32 oad)
{
	const Map645ToOop *pMap645ToOop = NULL;

	pMap645ToOop = g_aMap645ToOopRT;//实时数据
	
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
* 函数名称: meter_addr_query
* 函数功能: 根据测量点号查询测量点地址
* 输入参数: 无
* 输出参数: 无
* 返 回 值: 无
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
* 函数名称: yx_db_center_read
* 函数功能: 从数据中心读取遥信实时数据
* 输入参数: uint16 index 点号索引
* 输出参数: 无
* 返 回 值: 无
*******************************************************************************/
void yx_db_center_read(uint16 index)
{
	if(index >= g_nConfigYxNum)
	{
		return;
	}

	OOP_SWITCHIN_STATE_T SwitchState;  //遥信状态结构体
	memset(&SwitchState, 0x00, sizeof(OOP_SWITCHIN_STATE_T));
	
	uint8	aRecvBuff[500] = {0};
	uint32	nRevcBuffLen = 0;
	int 	ret  = 0;
	NOMAL_OAD_T NomalOad;

	NomalOad.logicId = 0;  //逻辑地址
	NomalOad.infoNum = 0;  //信息点

	//遥信数据读取
	NomalOad.oad.value = g_aYxDataConfig[index].nDbCenterOAD;  //普通OAD
	NomalOad.classtag = CLASS_DATA_NORMAL;

	ret = db_read_nomal(g_sDataCenterClient, &NomalOad, sizeof(aRecvBuff), (uint8*)aRecvBuff, &nRevcBuffLen);
	DB104_FMT_TRACE("<--第%d个遥信--> yx_db_center_read() ret = nomal_data_read ret:%d \n", index, ret);
	if(ret == 0)
	{
		DB104_FMT_TRACE("yx_db_center_read() 读取遥信数据成功  NomalOad.oad=%08x \n", NomalOad.oad.value);
		DB104_BUF_TRACE(aRecvBuff, nRevcBuffLen, "报文: ");

		//读取上来的是数据块(uint8)，需要进行分相(按位)处理
		memcpy(&SwitchState, aRecvBuff, nRevcBuffLen);
		
		g_aYxDataSend[index].nNewValue = SwitchState.state[g_aYxDataConfig[index].nItemIndex].st;
		if(g_aYxDataSend[index].nNewValue != g_aYxDataSend[index].nHisValue)
		{
			g_aYxDataSend[index].bChangeFlag = 1;
		}
		
		//临时赋值, 测试变化上送
		if(g_bTestEnable)
		{
			g_aYxDataSend[index].nNewValue = rand()%2;//对2取余得到就是0或者1
			g_aYxDataSend[index].bChangeFlag = 1;
			DB104_FMT_TRACE("遥信随机数 g_aYxDataSend[%d].nNewValue=%d \n", index, g_aYxDataSend[index].nNewValue);
		}
		
		if(g_aYxDataSend[index].bChangeFlag)
		{
			g_bYxChanged = TRUE;
			g_aYxDataSend[index].bSendFlag = FALSE;
		}

		DB104_FMT_TRACE("g_aYxDataSend[%d]: nNewValue=%d, nHisValue=%d, bChangeFlag=%d, bSendFlag=%d, 全局g_bYxChanged=%d \n", 
			index, g_aYxDataSend[index].nNewValue, g_aYxDataSend[index].nHisValue, g_aYxDataSend[index].bChangeFlag, 
			g_aYxDataSend[index].bSendFlag, g_bYxChanged);
	}
	else
	{
		DB104_FMT_TRACE("yx_db_center_read() 读取数据  NomalOad.oad=%08x \n", NomalOad.oad.value);
		DB104_FMT_TRACE("yx_db_center_read() 遥信数据读取失败! \n");
	}
	
	system_time_get(&g_aYxDataSend[index].btime[0]);

}

/*******************************************************************************
* 函数名称: yx_real_signal_read
* 函数功能: 通过mqtt消息获取硬遥信信号
* 输入参数: uint16 index 点号索引
* 输出参数: 无
* 返 回 值: 无
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

	//数据更新与检测
	g_aYxDataSend[index].nNewValue = (uint8)( (stRmt.ST >> g_aYxDataConfig[index].nItemIndex)&0x01 );
	if(g_aYxDataSend[index].nNewValue != g_aYxDataSend[index].nHisValue)
	{
		g_aYxDataSend[index].bChangeFlag = 1;
	}
	
	//临时赋值, 测试变化上送
	if(g_bTestEnable)
	{
		g_aYxDataSend[index].nNewValue = rand()%2;//对2取余得到就是0或者1
		g_aYxDataSend[index].bChangeFlag = 1;
		DB104_FMT_TRACE("遥信随机数 g_aYxDataSend[%d].nNewValue=%d \n", index, g_aYxDataSend[index].nNewValue);
	}
	
	if(g_aYxDataSend[index].bChangeFlag)
	{
		g_bYxChanged = TRUE;
		g_aYxDataSend[index].bSendFlag = FALSE;
	}

	DB104_FMT_TRACE("硬遥信g_aYxDataSend[%d]: nNewValue=%d, nHisValue=%d, bChangeFlag=%d, bSendFlag=%d, 全局g_bYxChanged=%d, status=0x%02x \n", 
		index, g_aYxDataSend[index].nNewValue, g_aYxDataSend[index].nHisValue, g_aYxDataSend[index].bChangeFlag, 
		g_aYxDataSend[index].bSendFlag, g_bYxChanged, status);

	system_time_get(&g_aYxDataSend[index].btime[0]);
}

/*******************************************************************************
* 函数名称: yx_calculate_sta_set
* 函数功能: 事件模块对相应遥信置位
* 输入参数: 无
* 输出参数: 无
* 返 回 值: 无
*******************************************************************************/
void yx_calculate_sta_set(uint32 subtype, uint8 subindex, uint8 value)
{
	DB104_FMT_TRACE("yx_calculate_sta_set() subtype=0x%08x, subindex=%d, value=%d \n", subtype, subindex, value);
	
	int i = 0;
	int ret = -1;
	int index = 0;
	
	//遥信数据读取
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

	//数据更新与检测
	g_aYxDataSend[index].nNewValue = value>0? 1: 0;
	if(g_aYxDataSend[index].nNewValue != g_aYxDataSend[index].nHisValue)
	{
		g_aYxDataSend[index].bChangeFlag = 1;
	}
	
	//临时赋值, 测试变化上送
	if(g_bTestEnable)
	{
		g_aYxDataSend[index].nNewValue = rand()%2;//对2取余得到就是0或者1
		g_aYxDataSend[index].bChangeFlag = 1;
		DB104_FMT_TRACE("遥信随机数 g_aYxDataSend[%d].nNewValue=%d \n", index, g_aYxDataSend[index].nNewValue);
	}
	
	if(g_aYxDataSend[index].bChangeFlag)
	{
		g_bYxChanged = TRUE;
		g_aYxDataSend[index].bSendFlag = FALSE;
	}

	DB104_FMT_TRACE("g_aYxDataSend[%d]: nNewValue=%d, nHisValue=%d, bChangeFlag=%d, bSendFlag=%d, 全局g_bYxChanged=%d \n", 
		index, g_aYxDataSend[index].nNewValue, g_aYxDataSend[index].nHisValue, g_aYxDataSend[index].bChangeFlag, 
		g_aYxDataSend[index].bSendFlag, g_bYxChanged);

	system_time_get(&g_aYxDataSend[index].btime[0]);
}


/*******************************************************************************
* 函数名称: yx_calculate_statistics
* 函数功能: 需要统计或计算的遥信信号
* 输入参数: uint16 index 点号索引
* 输出参数: 无
* 返 回 值: 无
*******************************************************************************/
void yx_calculate_statistics(uint16 index)
{
	if(index >= g_nConfigYxNum)
	{
		return;
	}
}

/*******************************************************************************
* 函数名称: yc_db_nomal_read
* 函数功能: 从数据中心读取遥测实时数据(普通型数据)
* 输入参数: uint16 index 点号索引
* 输出参数: 无
* 返 回 值: 无
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
	NOMAL_OAD_T NomalOad = {0};  //普通型OAD

	float32 fValueDB;  //存在于数据中心的值
	float32 fDeadZone;  //死区值
	float32 fDiffValue;  //差值
	float32 fRatio = 1;  //倍率
	const Map645ToOop *pMap645ToOop = NULL;

	//数据读取
	NomalOad.logicId = g_aYcDataConfig[index].nLogicId;  //逻辑地址
	NomalOad.infoNum = g_aYcDataConfig[index].nInfoNum;  //信息点号
	NomalOad.oad.value = g_aYcDataConfig[index].nDbCenterOAD;  //普通OAD
	NomalOad.classtag = CLASS_DATA_NORMAL;

	ret = db_read_nomal(g_sDataCenterClient, &NomalOad, sizeof(aRecvBuff), (uint8*)aRecvBuff, &nRevcBuffLen);
	DB104_FMT_TRACE("<--第%d个遥测--> yc_db_nomal_read() ret = db_read_nomal() ret:%d \n", index, ret);
	
	if(ret == 0)
	{
		DB104_FMT_TRACE("yc_db_nomal_read() 读取数据成功  g_aYcDataConfig[%d].nDbCenterOAD=%08x \n", 
			index, g_aYcDataConfig[index].nDbCenterOAD);
		DB104_BUF_TRACE(aRecvBuff, nRevcBuffLen, "报文: ");

		//读取上来的是数据块，需要进行分相处理
		ret = data_block_split(g_aYcDataConfig[index].nDbCenterOAD, g_aYcDataConfig[index].nItemIndex, aRecvBuff, &fValueDB);
		if(ret > 0)
		{
			//读取倍率
			pMap645ToOop = oad_info_find(g_aYcDataConfig[index].nDbCenterOAD);
			if(pMap645ToOop == NULL)
			{
				DB104_FMT_TRACE("yc_db_nomal_read() if(pMap645ToOop == NULL) \n");
			}
			else
			{
				fRatio = ratio_convert(pMap645ToOop->DesScaler, E_OOP_TO_REAL);
				DB104_FMT_TRACE("读取倍率 fRatio=%f \n", fRatio);
			}
			g_aYcDataSend[index].fNewValue = fValueDB*fRatio;
			DB104_FMT_TRACE("倍率处理之后g_aYcDataSend[index].fNewValue=%f, 倍率处理之前fValueDB=%f \n", g_aYcDataSend[index].fNewValue, fValueDB);
			
			//读取变化死区值
			if(para_query_float32(g_aYcDataConfig[index].nDeadZoneAddr, &fDeadZone) < 0)
			{
				DB104_FMT_TRACE("读取变化死区值para_query_float32()失败，直接赋默认值。 \n");
				fDeadZone = g_aYcDataConfig[index].fDeadZoneValue;
			}
			DB104_FMT_TRACE("变化死区值fDeadZone=%f \n", fDeadZone);

			//临时赋值, 测试变化上送
			if(g_bTestEnable)
			{
				g_aYcDataSend[index].fHisValue = rand()%100;//100以内随机数
				DB104_FMT_TRACE("遥测随机数 g_aYcDataSend[%d].fHisValue=%f \n", index, g_aYcDataSend[index].fHisValue);
			}

			//判断实时值是否越过死区
			fDiffValue = fabs(g_aYcDataSend[index].fNewValue - g_aYcDataSend[index].fHisValue);
			if(fDiffValue > fDeadZone)
			{
				DB104_FMT_TRACE("变化值fDiffValue=%f, 大于死区值, fNewValue=%f, fHisValue=%f 需要更新上报! \n", 
					fDiffValue, g_aYcDataSend[index].fNewValue, g_aYcDataSend[index].fHisValue);
				g_bYcChanged = TRUE;
				g_aYcDataSend[index].bChangeFlag = TRUE;
				g_aYcDataSend[index].bSendFlag = FALSE;
			}
			else
			{
				DB104_FMT_TRACE("变化值fDiffValue=%f, 小于死区值, fNewValue=%f, fHisValue=%f \n", 
					fDiffValue, g_aYcDataSend[index].fNewValue, g_aYcDataSend[index].fHisValue);
			}
			
		}
		else
		{
			DB104_FMT_TRACE("yc_db_nomal_read() 解析数据失败  g_aYcDataConfig[%d].nDbCenterOAD=%08x \n", 
				index, g_aYcDataConfig[index].nDbCenterOAD);
		}
	}
	else
	{
		DB104_FMT_TRACE("yc_db_nomal_read() 读取数据  g_aYcDataConfig[%d].nDbCenterOAD=%08x \n", 
			index, g_aYcDataConfig[index].nDbCenterOAD);
		DB104_FMT_TRACE("yc_db_nomal_read() 数据读取失败! \n");
	}
	
	system_time_get(&g_aYcDataSend[index].btime[0]);
}

/*******************************************************************************
* 函数名称: yc_db_record_read
* 函数功能: 从数据中心读取遥测实时数据(记录型数据)
* 输入参数: uint16 index 点号索引
* 输出参数: 无
* 返 回 值: 无
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

	//数据读取
	nRealTime_t = time(NULL);
	
	ReadRecord.recordOAD.logicId = g_aYcDataConfig[index].nLogicId;  //逻辑地址
	ReadRecord.recordOAD.infoNum = g_aYcDataConfig[index].nInfoNum;  //信息点号
	ReadRecord.recordOAD.road.oadMain.value = g_aYcDataConfig[index].nDbCenterOAD;  //主OAD
	ReadRecord.recordOAD.road.oadCols.nNum = g_aYcDataConfig[index].nOadColsNum;  //次OAD数量
	memcpy(&ReadRecord.recordOAD.road.oadCols.oad[0], &g_aYcDataConfig[index].aOadCols[0],
		g_aYcDataConfig[index].nOadColsNum*sizeof(uint32));  //关联次OAD
	ReadRecord.recordOAD.classtag = CLASS_DATA_COLLEC;

	ReadRecord.cType = COL_TM_STORE;  //采集存储时间
	//ReadRecord.cStart = nRealTime_t-24*3600;  //时间起始点，当前时间提前一天
	ReadRecord.cStart = 0;  //时间起始点，当前时间提前一天
	ReadRecord.cEnd = nRealTime_t;  //时间结束点
	ReadRecord.sortType = DOWN_SORT;  //排序方式 降序
	ReadRecord.MAC.nNum = 6;  //附加校验 长度
	meter_addr_query(g_aYcDataConfig[index].nInfoNum, ReadRecord.MAC.value, ReadRecord.MAC.nNum);  //附加校验 电表地址
	ReadRecord.ti.unit = TI_HOUR;  //时间间隔 单位
	ReadRecord.ti.value = 15;  //时间间隔 值

	DB104_FMT_TRACE("<--第%d个遥测--> yc_db_record_read() 数组下标index=%d, 逻辑地址logicId=%d, 信息点号infoNum=%d, 主OAD=%08x, 次OAD数量=%d, 次OAD=%08x, 校验MAC.nNum=%d, MAC.value=%02x%02x%02x%02x%02x%02x \n", 
		index, index, ReadRecord.recordOAD.logicId, ReadRecord.recordOAD.infoNum, ReadRecord.recordOAD.road.oadMain.value, g_aEnergyDataConfig[index].nOadColsNum, 
		ReadRecord.recordOAD.road.oadCols.oad[0].value, ReadRecord.MAC.nNum, ReadRecord.MAC.value[5], ReadRecord.MAC.value[4], ReadRecord.MAC.value[3], ReadRecord.MAC.value[2], ReadRecord.MAC.value[1], ReadRecord.MAC.value[0]);
	
	ret = db_read_record(g_sDataCenterClient, &ReadRecord, sizeof(aRecvBuff), &RecordOut);
	DB104_FMT_TRACE("yc_db_center_read() ret = db_read_record() ret:%d \n", ret);
	if(ret == 0)
	{
		nRevcBuffLen = RecordOut.record[0].outDatalen;
		memcpy(&aRecvBuff[0], &RecordOut.record[0].outDataBuf[0], nRevcBuffLen);

		DB104_FMT_TRACE("yc_db_record_read() 读取数据成功  g_aYcDataConfig[%d].nDbCenterOAD=%08x \n", 
			index, g_aYcDataConfig[index].nDbCenterOAD);
		DB104_BUF_TRACE(aRecvBuff, nRevcBuffLen, "报文: ");

		//读取上来的是数据块，需要进行分相处理
		ret = data_block_split(g_aYcDataConfig[index].aOadCols[0], g_aYcDataConfig[index].nItemIndex, aRecvBuff, &fValueDB);
		if(ret > 0)
		{
			//读取倍率
			pMap645ToOop = oad_info_find(g_aYcDataConfig[index].aOadCols[0]);
			if(pMap645ToOop == NULL)
			{
				DB104_FMT_TRACE("yc_db_record_read() if(pMap645ToOop == NULL) \n");
			}
			else
			{
				fRatio = ratio_convert(pMap645ToOop->DesScaler, E_OOP_TO_REAL);
				DB104_FMT_TRACE("读取倍率 fRatio=%f \n", fRatio);
			}
			g_aYcDataSend[index].fNewValue = fValueDB*fRatio;
			DB104_FMT_TRACE("倍率处理之后g_aYcDataSend[index].fNewValue=%f, 倍率处理之前fValueDB=%f \n", g_aYcDataSend[index].fNewValue, fValueDB);
			
			//读取变化死区值
			if(para_query_float32(g_aYcDataConfig[index].nDeadZoneAddr, &fDeadZone) < 0)
			{
				DB104_FMT_TRACE("读取变化死区值para_query_float32()失败，直接赋默认值。 \n");
				fDeadZone = g_aYcDataConfig[index].fDeadZoneValue;
			}
			DB104_FMT_TRACE("变化死区值fDeadZone=%f \n", fDeadZone);

			//临时赋值, 测试变化上送
			if(g_bTestEnable)
			{
				g_aYcDataSend[index].fHisValue = rand()%100;//100以内随机数
				DB104_FMT_TRACE("遥测随机数 g_aYcDataSend[%d].fHisValue=%f \n", index, g_aYcDataSend[index].fHisValue);
			}

			//判断实时值是否越过死区
			fDiffValue = fabs(g_aYcDataSend[index].fNewValue - g_aYcDataSend[index].fHisValue);
			if(fDiffValue > fDeadZone)
			{
				DB104_FMT_TRACE("变化值fDiffValue=%f, 大于死区值, fNewValue=%f, fHisValue=%f 需要更新上报! \n", 
					fDiffValue, g_aYcDataSend[index].fNewValue, g_aYcDataSend[index].fHisValue);
				g_bYcChanged = TRUE;
				g_aYcDataSend[index].bChangeFlag = TRUE;
				g_aYcDataSend[index].bSendFlag = FALSE;
			}
			else
			{
				DB104_FMT_TRACE("变化值fDiffValue=%f, 小于死区值, fNewValue=%f, fHisValue=%f \n", 
					fDiffValue, g_aYcDataSend[index].fNewValue, g_aYcDataSend[index].fHisValue);
			}
			
		}
		else
		{
			DB104_FMT_TRACE("yc_db_record_read() 解析数据失败  g_aYcDataConfig[%d].nDbCenterOAD=%08x \n", 
				index, g_aYcDataConfig[index].nDbCenterOAD);
		}
	}
	else
	{
		DB104_FMT_TRACE("yc_db_record_read() 读取数据  g_aYcDataConfig[%d].nDbCenterOAD=%08x \n", 
			index, g_aYcDataConfig[index].nDbCenterOAD);
		DB104_FMT_TRACE("yc_db_record_read() 数据读取失败! \n");
	}
	
	system_time_get(&g_aYcDataSend[index].btime[0]);
}

/*******************************************************************************
* 函数名称: yc_db_record_read
* 函数功能: 需要统计或计算的遥测信号
* 输入参数: uint16 index 点号索引
* 输出参数: 无
* 返 回 值: 无
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
	
	float32 resultValue = 0;//结果值
	float32 fDeadZone = 0;	//死区值
	float32 fDiffValue = 0;  //差值
	float32 fRatio = 1;  //倍率

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
				DB104_FMT_TRACE("yc_calculate_statistics 错误! ycRet=%f, paraRet=%f \n", ycRet, paraRet);
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
				DB104_FMT_TRACE("yc_calculate_statistics 错误! ycRet=%f \n", ycRet);
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
				DB104_FMT_TRACE("yc_calculate_statistics 错误! ycRet=%f, paraRet=%f \n", ycRet, paraRet);
				return;
			}

			if( (paraValue<0.00001) && (paraRet>(-0.00001)) )
			{
				DB104_FMT_TRACE("yc_calculate_statistics 错误! paraValue=%f \n", paraValue);
				return;
			}

			resultValue = ycValue/paraValue;
		}
			break;
		default:
			return;
			break;
	}
	
	//读取倍率
	g_aYcDataSend[index].fNewValue = resultValue*fRatio;
	DB104_FMT_TRACE("倍率处理之后g_aYcDataSend[index].fNewValue=%f, 倍率处理之前resultValue=%f, 倍率fRatio=%f \n", g_aYcDataSend[index].fNewValue, resultValue, fRatio);
	
	//读取变化死区值
	if(para_query_float32(g_aYcDataConfig[index].nDeadZoneAddr, &fDeadZone) < 0)
	{
		DB104_FMT_TRACE("读取变化死区值para_query_float32()失败，直接赋默认值。 \n");
		fDeadZone = g_aYcDataConfig[index].fDeadZoneValue;
	}
	DB104_FMT_TRACE("变化死区值fDeadZone=%f \n", fDeadZone);

	//临时赋值, 测试变化上送
	if(g_bTestEnable)
	{
		g_aYcDataSend[index].fHisValue = rand()%100;//100以内随机数
		DB104_FMT_TRACE("遥测随机数 g_aYcDataSend[%d].fHisValue=%f \n", index, g_aYcDataSend[index].fHisValue);
	}

	//判断实时值是否越过死区
	fDiffValue = fabs(g_aYcDataSend[index].fNewValue - g_aYcDataSend[index].fHisValue);
	if(fDiffValue > fDeadZone)
	{
		DB104_FMT_TRACE("变化值fDiffValue=%f, 大于死区值, fNewValue=%f, fHisValue=%f 需要更新上报! \n", 
			fDiffValue, g_aYcDataSend[index].fNewValue, g_aYcDataSend[index].fHisValue);
		g_bYcChanged = TRUE;
		g_aYcDataSend[index].bChangeFlag = TRUE;
		g_aYcDataSend[index].bSendFlag = FALSE;
	}
	else
	{
		DB104_FMT_TRACE("变化值fDiffValue=%f, 小于死区值, fNewValue=%f, fHisValue=%f \n", 
			fDiffValue, g_aYcDataSend[index].fNewValue, g_aYcDataSend[index].fHisValue);
	}
	
	system_time_get(&g_aYcDataSend[index].btime[0]);
}


/*******************************************************************************
* 函数名称: en_db_nomal_read
* 函数功能: 从数据中心读取电量实时数据(普通型数据)
* 输入参数: uint16 index 点号索引
* 输出参数: 无
* 返 回 值: 无
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
	NOMAL_OAD_T NomalOad = {0};  //普通型OAD

	float32 fValueDB;  //存在于数据中心的值
	float32 fDeadZone;  //死区值
	float32 fDiffValue;  //差值
	float32 fRatio = 1;  //倍率
	const Map645ToOop *pMap645ToOop = NULL;

	//数据读取
	NomalOad.logicId = g_aEnergyDataConfig[index].nLogicId;  //逻辑地址
	NomalOad.infoNum = g_aEnergyDataConfig[index].nInfoNum;  //信息点号
	NomalOad.oad.value = g_aEnergyDataConfig[index].nDbCenterOAD;  //普通OAD
	NomalOad.classtag = CLASS_DATA_NORMAL;

	ret = db_read_nomal(g_sDataCenterClient, &NomalOad, sizeof(aRecvBuff), (uint8*)aRecvBuff, &nRevcBuffLen);
	DB104_FMT_TRACE("<--第%d个电量--> en_db_nomal_read() ret = db_read_nomal() ret:%d \n", index, ret);
	
	if(ret == 0)
	{
		DB104_FMT_TRACE("en_db_nomal_read() 读取数据成功  g_aEnergyDataConfig[%d].nDbCenterOAD=%08x \n", 
			index, g_aEnergyDataConfig[index].nDbCenterOAD);
		DB104_BUF_TRACE(aRecvBuff, nRevcBuffLen, "报文: ");

		//读取上来的是数据块，需要进行分相处理
		ret = data_block_split(g_aEnergyDataConfig[index].nDbCenterOAD, g_aEnergyDataConfig[index].nItemIndex, aRecvBuff, &fValueDB);
		if(ret > 0)
		{
			//读取倍率
			pMap645ToOop = oad_info_find(g_aEnergyDataConfig[index].nDbCenterOAD);
			if(pMap645ToOop == NULL)
			{
				DB104_FMT_TRACE("en_db_nomal_read() if(pMap645ToOop == NULL) \n");
			}
			else
			{
				fRatio = ratio_convert(pMap645ToOop->DesScaler, E_OOP_TO_REAL);
				DB104_FMT_TRACE("读取倍率 fRatio=%f \n", fRatio);
			}
			g_aEnergyDataSend[index].fNewValue = fValueDB*fRatio;
			DB104_FMT_TRACE("倍率处理之后g_aEnergyDataSend[index].fNewValue=%f, 倍率处理之前fValueDB=%f \n", g_aEnergyDataSend[index].fNewValue, fValueDB);
			
			//读取变化死区值
			if(para_query_float32(g_aEnergyDataConfig[index].nDeadZoneAddr, &fDeadZone) < 0)
			{
				DB104_FMT_TRACE("读取变化死区值para_query_float32()失败，直接赋默认值。 \n");
				fDeadZone = g_aEnergyDataConfig[index].fDeadZoneValue;
			}
			DB104_FMT_TRACE("变化死区值fDeadZone=%f \n", fDeadZone);

			//判断实时值是否越过死区
			fDiffValue = fabs(g_aEnergyDataSend[index].fNewValue - g_aEnergyDataSend[index].fHisValue);
			if(fDiffValue > fDeadZone)
			{
				DB104_FMT_TRACE("变化值fDiffValue=%f, 大于死区值, fNewValue=%f, fHisValue=%f 需要更新上报! \n", 
					fDiffValue, g_aEnergyDataSend[index].fNewValue, g_aEnergyDataSend[index].fHisValue);
				g_bEnergyChanged = TRUE;
				g_aEnergyDataSend[index].bChangeFlag = TRUE;
				g_aEnergyDataSend[index].bSendFlag = FALSE;
			}
			else
			{
				DB104_FMT_TRACE("变化值fDiffValue=%f, 小于死区值, fNewValue=%f, fHisValue=%f \n", 
					fDiffValue, g_aEnergyDataSend[index].fNewValue, g_aEnergyDataSend[index].fHisValue);
			}
			
		}
		else
		{
			DB104_FMT_TRACE("en_db_nomal_read() 解析数据失败  g_aEnergyDataConfig[%d].nDbCenterOAD=%08x \n", 
				index, g_aEnergyDataConfig[index].nDbCenterOAD);
		}
	}
	else
	{
		DB104_FMT_TRACE("en_db_nomal_read() 读取数据  g_aEnergyDataConfig[%d].nDbCenterOAD=%08x \n", 
			index, g_aEnergyDataConfig[index].nDbCenterOAD);
		DB104_FMT_TRACE("en_db_nomal_read() 数据读取失败! \n");

		//临时赋值, 测试变化上送
		if(g_bTestEnable)
		{
			g_aEnergyDataSend[index].fNewValue = rand()%100;//100以内随机数
			DB104_FMT_TRACE("电量随机数 g_aEnergyDataSend[%d].fNewValue=%f \n", index, g_aEnergyDataSend[index].fNewValue);
		}
	}

	system_time_get(&g_aEnergyDataSend[index].btime[0]);
}

/*******************************************************************************
* 函数名称: yc_db_record_read
* 函数功能: 从数据中心读取遥测实时数据(记录型数据)
* 输入参数: uint16 index 点号索引
* 输出参数: 无
* 返 回 值: 无
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

	//数据读取
	nRealTime_t = time(NULL);
	
	ReadRecord.recordOAD.logicId = g_aEnergyDataConfig[index].nLogicId;  //逻辑地址
	ReadRecord.recordOAD.infoNum = g_aEnergyDataConfig[index].nInfoNum;  //信息点号
	ReadRecord.recordOAD.road.oadMain.value = g_aEnergyDataConfig[index].nDbCenterOAD;  //主OAD
	ReadRecord.recordOAD.road.oadCols.nNum = g_aEnergyDataConfig[index].nOadColsNum;  //次OAD数量
	memcpy(&ReadRecord.recordOAD.road.oadCols.oad[0], &g_aEnergyDataConfig[index].aOadCols[0],
		g_aEnergyDataConfig[index].nOadColsNum*sizeof(uint32));  //关联次OAD
	ReadRecord.recordOAD.classtag = CLASS_DATA_COLLEC;

	ReadRecord.cType = COL_TM_STORE;  //采集存储时间
	//ReadRecord.cStart = nRealTime_t-24*3600;  //时间起始点，当前时间提前一天
	ReadRecord.cStart = 0;  //时间起始点，当前时间提前一天
	ReadRecord.cEnd = nRealTime_t;  //时间结束点
	ReadRecord.sortType = DOWN_SORT;  //排序方式 降序
	ReadRecord.MAC.nNum = 6;  //附加校验 长度
	meter_addr_query(g_aEnergyDataConfig[index].nInfoNum, ReadRecord.MAC.value, ReadRecord.MAC.nNum);  //附加校验 电表地址
	ReadRecord.ti.unit = 6;  //时间间隔 单位
	ReadRecord.ti.value = 6;  //时间间隔 值

	DB104_FMT_TRACE("<--第%d个电量--> en_db_record_read() 数组下标index=%d, 逻辑地址logicId=%d, 信息点号infoNum=%d, 主OAD=%08x, 次OAD数量=%d, 次OAD=%08x, 校验MAC.nNum=%d, MAC.value=%02x%02x%02x%02x%02x%02x \n", 
		index, index, ReadRecord.recordOAD.logicId, ReadRecord.recordOAD.infoNum, ReadRecord.recordOAD.road.oadMain.value, g_aEnergyDataConfig[index].nOadColsNum, 
		ReadRecord.recordOAD.road.oadCols.oad[0].value, ReadRecord.MAC.nNum, ReadRecord.MAC.value[5], ReadRecord.MAC.value[4], ReadRecord.MAC.value[3], ReadRecord.MAC.value[2], ReadRecord.MAC.value[1], ReadRecord.MAC.value[0]);
	
	ret = db_read_record(g_sDataCenterClient, &ReadRecord, sizeof(aRecvBuff), &RecordOut);
	DB104_FMT_TRACE("en_db_record_read() ret = db_read_record() ret:%d \n", ret);
	if(ret == 0)
	{
		nRevcBuffLen = RecordOut.record[0].outDatalen;
		memcpy(&aRecvBuff[0], &RecordOut.record[0].outDataBuf[0], nRevcBuffLen);

		DB104_FMT_TRACE("en_db_record_read() 读取数据成功  g_aEnergyDataConfig[%d].nDbCenterOAD=%08x \n", 
			index, g_aEnergyDataConfig[index].nDbCenterOAD);
		DB104_BUF_TRACE(aRecvBuff, nRevcBuffLen, "报文: ");

		//读取上来的是数据块，需要进行分相处理
		ret = data_block_split(g_aEnergyDataConfig[index].aOadCols[0], g_aEnergyDataConfig[index].nItemIndex, aRecvBuff, &fValueDB);
		if(ret > 0)
		{
			//读取倍率
			pMap645ToOop = oad_info_find(g_aEnergyDataConfig[index].aOadCols[0]);
			if(pMap645ToOop == NULL)
			{
				DB104_FMT_TRACE("en_db_record_read() if(pMap645ToOop == NULL) \n");
			}
			else
			{
				fRatio = ratio_convert(pMap645ToOop->DesScaler, E_OOP_TO_REAL);
				DB104_FMT_TRACE("读取倍率 fRatio=%f \n", fRatio);
			}
			g_aEnergyDataSend[index].fNewValue = fValueDB*fRatio;
			DB104_FMT_TRACE("倍率处理之后g_aEnergyDataSend[index].fNewValue=%f, 倍率处理之前fValueDB=%f \n", g_aEnergyDataSend[index].fNewValue, fValueDB);
			
			//读取变化死区值
			if(para_query_float32(g_aEnergyDataConfig[index].nDeadZoneAddr, &fDeadZone) < 0)
			{
				DB104_FMT_TRACE("读取变化死区值para_query_float32()失败，直接赋默认值。 \n");
				fDeadZone = g_aEnergyDataConfig[index].fDeadZoneValue;
			}
			DB104_FMT_TRACE("变化死区值fDeadZone=%f \n", fDeadZone);

			//判断实时值是否越过死区
			fDiffValue = fabs(g_aEnergyDataSend[index].fNewValue - g_aEnergyDataSend[index].fHisValue);
			if(fDiffValue > fDeadZone)
			{
				DB104_FMT_TRACE("变化值fDiffValue=%f, 大于死区值, fNewValue=%f, fHisValue=%f 需要更新上报! \n", 
					fDiffValue, g_aEnergyDataSend[index].fNewValue, g_aEnergyDataSend[index].fHisValue);
				g_bEnergyChanged = TRUE;
				g_aEnergyDataSend[index].bChangeFlag = TRUE;
				g_aEnergyDataSend[index].bSendFlag = FALSE;
			}
			else
			{
				DB104_FMT_TRACE("变化值fDiffValue=%f, 小于死区值, fNewValue=%f, fHisValue=%f \n", 
					fDiffValue, g_aEnergyDataSend[index].fNewValue, g_aEnergyDataSend[index].fHisValue);
			}
			
		}
		else
		{
			DB104_FMT_TRACE("en_db_record_read() 解析数据失败  g_aEnergyDataConfig[%d].nDbCenterOAD=%08x \n", 
				index, g_aEnergyDataConfig[index].nDbCenterOAD);
		}
	}
	else
	{
		DB104_FMT_TRACE("en_db_record_read() 读取数据  g_aEnergyDataConfig[%d].nDbCenterOAD=%08x \n", 
			index, g_aEnergyDataConfig[index].nDbCenterOAD);
		DB104_FMT_TRACE("en_db_record_read() 数据读取失败! \n");

		//临时赋值, 测试变化上送
		if(g_bTestEnable)
		{
			g_aEnergyDataSend[index].fNewValue = rand()%100;//100以内随机数
			DB104_FMT_TRACE("电量随机数 g_aEnergyDataSend[%d].fNewValue=%f \n", index, g_aEnergyDataSend[index].fNewValue);
		}
	}

	system_time_get(&g_aEnergyDataSend[index].btime[0]);
}

/*******************************************************************************
* 函数名称: en_calculate_statistics
* 函数功能: 需要统计或计算的电量信号
* 输入参数: uint16 index 点号索引
* 输出参数: 无
* 返 回 值: 无
*******************************************************************************/
void en_calculate_statistics(uint16 index)
{
	if(index >= g_nConfigEnNum)
	{
		return;
	}

	DB104_FMT_TRACE("en_calculate_statistics() index=%d, 过分标志=%d, 当前分钟=%d, 分钟%15=%d, 过日标志=%d \n", 
		index, g_unPastTimeFlag.BitSect.Minute, g_stCurBinTime.Minute, g_stCurBinTime.Minute%15, g_unPastTimeFlag.BitSect.Day);
	
	switch(g_aEnergyDataConfig[index].nDealType)
	{
		case CALC_EN_15MIN_FREEZE:
		{
			if( g_unPastTimeFlag.BitSect.Minute && (0 == g_stCurBinTime.Minute%15) )
			{
				DB104_FMT_TRACE("en_calculate_statistics() 15分钟冻结数据! index=%d \n", index);
				en_db_nomal_read(index);
			}
		}
			break;
		case CALC_EN_DAY_FREEZE:
		{
			if(g_unPastTimeFlag.BitSect.Day)
			{
				DB104_FMT_TRACE("en_calculate_statistics() 日冻结数据! index=%d \n", index);
				en_db_nomal_read(index);
			}
		}
			break;
		default:
			break;
	}
}

/*******************************************************************************
* 函数名称: data_db_center_read
* 函数功能: 从数据中心读取实时数据
* 输入参数: 无
* 输出参数: 无
* 返 回 值: 无
*******************************************************************************/
void data_db_center_read()
{
	int i = 0;
	
	//遥信数据读取
	DB104_FMT_TRACE("\n************data_db_center_read() 遥信数据读取 start...************ \n\n");
	for(i=0; i<g_nConfigYxNum; i++)
	{
		DB104_FMT_TRACE("i=%d, g_nConfigYxNum=%d, g_aYxDataConfig[i].nDbCenterOAD=0x%08x, g_aYxDataConfig[i].nDealType=0x%08x \n", 
			i, g_nConfigYxNum, g_aYxDataConfig[i].nDbCenterOAD, g_aYxDataConfig[i].nDealType);

		if(g_aYxDataConfig[i].nDbCenterOAD == 0xFFFFFFFF)
		{
			if(g_aYxDataConfig[i].nDealType == CALC_YX_REAL_SIGNAL)
			{
				yx_real_signal_read(i);//硬遥信
			}
			else
			{
				yx_calculate_statistics(i);//统计数据, 非实际采集
			}
		}
		else
		{
			yx_db_center_read(i);//从数据中心读取数据
		}
	}

	//遥测数据读取
	DB104_FMT_TRACE("\n************data_db_center_read() 遥测数据读取 start...************ \n\n");
	for(i=0; i<g_nConfigYcNum; i++)
	{
		if(g_aYcDataConfig[i].nDbCenterOAD == 0xFFFFFFFF)
		{
			yc_calculate_statistics(i);//统计数据, 非实际采集
		}
		else
		{
			if(g_aYcDataConfig[i].nLogicId == 1)  //交采数据
			{
				yc_db_nomal_read(i);
			}
			else if(g_aYcDataConfig[i].nLogicId == 0)  //电表数据
			{
				yc_db_record_read(i);
			}
		}
	}

	//电能量数据读取
	DB104_FMT_TRACE("\n************data_db_center_read() 电能量数据读取 start...************ \n\n");
	for(i=0; i<g_nConfigEnNum; i++)
	{
		DB104_FMT_TRACE("i=%d, g_nConfigEnNum=%d, g_aEnergyDataConfig[i].nLogicId=%d, g_aEnergyDataConfig[i].nDealType=0x%08x \n", 
			i, g_nConfigEnNum, g_aEnergyDataConfig[i].nLogicId, g_aEnergyDataConfig[i].nDealType);
		if(g_aEnergyDataConfig[i].nDealType != 0)
		{
			en_calculate_statistics(i);//统计数据, 非实际采集
		}
		else
		{
			if(g_aEnergyDataConfig[i].nLogicId == 1)  //交采数据
			{
				en_db_nomal_read(i);
			}
			else if(g_aEnergyDataConfig[i].nLogicId == 0)  //电表数据
			{
				en_db_record_read(i);
			}
		}
	}
}

/*******************************************************************************
* 函数名称: para_db_center_save
* 函数功能: 将定值参数保存至数据中心
* 输入参数: 无
* 输出参数: 无
* 返 回 值: 无
*******************************************************************************/
int para_data_search(uint8 *pDesbuffer, uint16 *nDesLen, const Map104ToOop *pOadInfo)
{
	int i = 0;
	float32 fRatio = 1;
	float32 fValue = 0;

	switch(pOadInfo->Oad.value)
	{
		case (0x49010200)://死区参数0x49010200
		{
			OOP_DEAD_ZONE_T sDeadZone;
			memset(&sDeadZone, 0, sizeof(OOP_DEAD_ZONE_T));
			for(i=0; i<MAX_OOP_ADDR_NUM; i++)
			{
				//读取参数值
				if(para_query_float32(pOadInfo->Addr[i], &fValue) < 0)
				{
					DB104_FMT_TRACE("para_data_search() 未读到104地址对应参数值 pOadInfo->Oad=%08x, pOadInfo->Addr[%d]=%04x \n", pOadInfo->Oad.value, i, pOadInfo->Addr[i]);
					continue;  //未读到104地址对应参数值
				}
				
				fRatio = ratio_convert(pOadInfo->InfoSca[i], E_REAL_TO_OOP);  //读取倍率
				DB104_FMT_TRACE("para_data_search() 读取倍率 pOadInfo->Oad=%08x, pOadInfo->InfoSca[%d]=%d, fRatio=%f \n", 
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
			
			DB104_FMT_TRACE("para_data_search() 组装报文 pOadInfo->Oad=%08x, *nDesLen=%d, pDesbuffer: \n", pOadInfo->Oad.value, *nDesLen);
			DB104_BUF_TRACE(pDesbuffer, *nDesLen, "报文: ");
		}
			break;
		case (0x49020200)://PT参数0x49020200
		{
			OOP_PT_T sPT;
			memset(&sPT, 0, sizeof(OOP_PT_T));
			for(i=0; i<MAX_OOP_ADDR_NUM; i++)
			{
				//读取参数值
				if(para_query_float32(pOadInfo->Addr[i], &fValue) < 0)
				{
					continue;  //未读到104地址对应参数值
				}
				fRatio = ratio_convert(pOadInfo->InfoSca[i], E_REAL_TO_OOP);  //读取倍率
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
		case (0x49030200)://CT参数0x49030200
		{
			OOP_CT_T sCT;
			memset(&sCT, 0, sizeof(OOP_CT_T));
			for(i=0; i<MAX_OOP_ADDR_NUM; i++)
			{
				//读取参数值
				if(para_query_float32(pOadInfo->Addr[i], &fValue) < 0)
				{
					continue;  //未读到104地址对应参数值
				}
				fRatio = ratio_convert(pOadInfo->InfoSca[i], E_REAL_TO_OOP);  //读取倍率
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
		case (0x49040200)://低电压报警0x49040200
		{
			OOP_LOW_U_T sLowU;
			memset(&sLowU, 0, sizeof(OOP_LOW_U_T));
			for(i=0; i<MAX_OOP_ADDR_NUM; i++)
			{
				//读取参数值
				if(para_query_float32(pOadInfo->Addr[i], &fValue) < 0)
				{
					continue;  //未读到104地址对应参数值
				}
				fRatio = ratio_convert(pOadInfo->InfoSca[i], E_REAL_TO_OOP);  //读取倍率
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
		case (0x49050200)://过电压报警0x49050200
		{
			OOP_OVER_U_T sOverU;
			memset(&sOverU, 0, sizeof(OOP_OVER_U_T));
			for(i=0; i<MAX_OOP_ADDR_NUM; i++)
			{
				//读取参数值
				if(para_query_float32(pOadInfo->Addr[i], &fValue) < 0)
				{
					continue;  //未读到104地址对应参数值
				}
				fRatio = ratio_convert(pOadInfo->InfoSca[i], E_REAL_TO_OOP);  //读取倍率
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
		case (0x49060200)://重载报警0x49060200
		{
			OOP_HEAVY_LOAD_T sHeavyLoad;
			memset(&sHeavyLoad, 0, sizeof(OOP_HEAVY_LOAD_T));
			for(i=0; i<MAX_OOP_ADDR_NUM; i++)
			{
				//读取参数值
				if(para_query_float32(pOadInfo->Addr[i], &fValue) < 0)
				{
					continue;  //未读到104地址对应参数值
				}
				fRatio = ratio_convert(pOadInfo->InfoSca[i], E_REAL_TO_OOP);  //读取倍率
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
		case (0x49070200)://过载报警0x49070200
		{
			OOP_OVER_LOAD_T sOverLoad;
			memset(&sOverLoad, 0, sizeof(OOP_OVER_LOAD_T));
			for(i=0; i<MAX_OOP_ADDR_NUM; i++)
			{
				//读取参数值
				if(para_query_float32(pOadInfo->Addr[i], &fValue) < 0)
				{
					continue;  //未读到104地址对应参数值
				}
				fRatio = ratio_convert(pOadInfo->InfoSca[i], E_REAL_TO_OOP);  //读取倍率
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
		case (0x49080200)://开入量采集防抖动时间0x49080200
		{
			OOP_YX_GET_TIME_T sYxGetTime;
			memset(&sYxGetTime, 0, sizeof(OOP_YX_GET_TIME_T));
			for(i=0; i<MAX_OOP_ADDR_NUM; i++)
			{
				//读取参数值
				if(para_query_float32(pOadInfo->Addr[i], &fValue) < 0)
				{
					continue;  //未读到104地址对应参数值
				}
				fRatio = ratio_convert(pOadInfo->InfoSca[i], E_REAL_TO_OOP);  //读取倍率
				if(i == 0)
				{
					sYxGetTime.nTime = (uint32)(fValue*fRatio);
				}
			}
			*nDesLen = sizeof(OOP_YX_GET_TIME_T);
			memcpy(pDesbuffer, &sYxGetTime, sizeof(OOP_YX_GET_TIME_T));
		}
			break;
		case (0x49090200)://分闸输出脉冲保持时间0x49090200
		{
			OOP_PULSE_TIME_T sPulseTime;
			memset(&sPulseTime, 0, sizeof(OOP_PULSE_TIME_T));
			for(i=0; i<MAX_OOP_ADDR_NUM; i++)
			{
				//读取参数值
				if(para_query_float32(pOadInfo->Addr[i], &fValue) < 0)
				{
					continue;  //未读到104地址对应参数值
				}
				fRatio = ratio_convert(pOadInfo->InfoSca[i], E_REAL_TO_OOP);  //读取倍率
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
		case (0x490A0200)://蓄电池自动活化周期0x490A0200
		{
			OOP_AUTO_ACT_CYCLE_T sAutoActCycle;
			memset(&sAutoActCycle, 0, sizeof(OOP_AUTO_ACT_CYCLE_T));
			for(i=0; i<MAX_OOP_ADDR_NUM; i++)
			{
				//读取参数值
				if(para_query_float32(pOadInfo->Addr[i], &fValue) < 0)
				{
					continue;  //未读到104地址对应参数值
				}
				fRatio = ratio_convert(pOadInfo->InfoSca[i], E_REAL_TO_OOP);  //读取倍率
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
* 函数名称: para_db_center_save
* 函数功能: 将定值参数保存至数据中心
* 输入参数: 无
* 输出参数: 无
* 返 回 值: 无
*******************************************************************************/
void para_db_center_save()
{
	uint8   aSendBuff[500] = {0};
	uint16  nSendBuffLen = 0;
	uint8   aRecvBuff[500] = {0};
	uint32  nRevcBuffLen = 0;
	int     ret  = 0;
	const Map104ToOop *pMap104ToOop = NULL;
	pMap104ToOop = g_aMap104ToOopRT;//实时数据
	
	if(pMap104ToOop==NULL)
    {
		return;
	} 

	NOMAL_OAD_T NomalOad = {0};
	NomalOad.logicId = 1;  //逻辑地址
	NomalOad.infoNum = 0;  //信息点

	DB104_FMT_TRACE("\n************para_db_center_save() 定值参数写入数据中心 start...************ \n\n");

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
			DB104_FMT_TRACE("para_db_center_save() 获取存储内容 pMap104ToOop->Oad=%08x, nSendBuffLen=%d, aSendBuff: \n", pMap104ToOop->Oad.value, nSendBuffLen);
			DB104_BUF_TRACE(aSendBuff, nSendBuffLen, "报文: ");
			
			//普通型数据
			NomalOad.oad = pMap104ToOop->Oad;  //普通OAD
			NomalOad.classtag = CLASS_DATA_NORMAL;

			//写入数据
			ret = db_write_nomal(g_sDataCenterClient, &NomalOad, (uint8*)aSendBuff, nSendBuffLen);
			DB104_FMT_TRACE("para_db_center_save() ret = nomal_data_write ret:%d \n", ret);
			if(ret == 0)
			{
				
				DB104_FMT_TRACE("para_db_center_save() 写入数据  NomalOad.oad=%08x \n", NomalOad.oad.value);
				DB104_BUF_TRACE(aSendBuff, nSendBuffLen, "报文: ");
				DB104_FMT_TRACE("para_db_center_save() 定值参数数据写入成功! \n");
			}

			//读取数据
			ret = db_read_nomal(g_sDataCenterClient, &NomalOad, sizeof(aRecvBuff), (uint8*)aRecvBuff, &nRevcBuffLen);
			DB104_FMT_TRACE("para_db_center_save() ret = nomal_data_read ret:%d \n", ret);
			if(ret == 0)
			{
				DB104_FMT_TRACE("para_db_center_save() 读取数据  NomalOad.oad=%08x \n", NomalOad.oad.value);
				DB104_BUF_TRACE(aRecvBuff, nRevcBuffLen, "报文: ");
				DB104_FMT_TRACE("para_db_center_save() 定值参数数据读取成功! \n");
			}
		}

		pMap104ToOop++;
	}
}











