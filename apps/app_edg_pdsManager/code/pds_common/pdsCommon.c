
#include "pdsAll.h"
#include "pdsCommon.h"
#include "pdsMqtt.h"
#include "storage.h"
#include "area.h"

Meter_File_T MeterFileAll;
Line_Loss_Info_T LineMeter;
Line_Loss_ALL_Task_Info_T LineTask;

Calc_Thread_Arg  gCalcArg;

DB_CLIENT lineloss;
OOP_DATETIME_S_T             gDaytaskEndTime;   //记录下召的任务结束时间 如果变了就是下一轮
OOP_DATETIME_S_T             gMonthtaskEndTime;   //记录下召的任务结束时间 如果变了就是下一轮
OOP_DATETIME_S_T             gMintaskEndTime;   //记录下召的任务结束时间 如果变了就是下一轮


PastTimeFlag_T             gPastTimeFlag;                           //过时间标志
DateTimeHex_t              gCurBinTime;                             //当前BIN时间
DateTimeHex_t              gPreBinTime;                             //上次BIN时间

LockQueue_T gTransQueue;            //存放收到的taskManager透传消息回码报文
Line_Loss_Period_T  gLineLossPeriod;
All_Meter_Data_T AllMeterData;
All_Vol_Cur_t  AllVolCur;
Line_Impedance_Limit_T gImpeLimit;       //阻抗限值
uint32 gImpeEventIndex;
extern uint8 LocalAddr[6];
extern int gMqttSock ;
extern int gCalcSock ;
extern char PrivateDir[100];
uint32 g_sendchecktime = 0; //发送查询时间报文的时间 

/* 任务的缓存 */
OOP_TASK_LIST_T oopTaskList;

//普通采集方案的缓存
OOP_PLAN_NORMAL_LIST_T oopPlanNormalList;

typedef struct _LINE_LOSS_T
{
    uint32 lastenerger[800];
    uint32 curenerger[800];
    int32 addenergry[800];

}LINE_LOSS_T;

LINE_LOSS_T linelossvalue;
 extern uint8 gdelay;


 /*******************************************************************************
* 函数名称: taskmng_tsa_get
* 函数功能: 根据序号获取电表地址
* 输入参数: Pn             要检测的测量点

* 输出参数: 
  
* 返 回 值:  电表地址
*******************************************************************************/
OOP_TSA_T taskmng_tsa_get(uint16 Pn,uint8 logicid)
{
    OOP_TSA_T tsa;
    memset(&tsa,0x00,sizeof(OOP_TSA_T));
    uint16 i;
    pthread_rwlock_rdlock(&MeterFileAll.rwLock);
    for(i=0;i<MeterFileAll.MeterCount;i++)
    {
        if(Pn == MeterFileAll.MeterFile[i].nIndex )
        {
            tsa=MeterFileAll.MeterFile[i].basic.tsa;
            pthread_rwlock_unlock(&MeterFileAll.rwLock);
            return tsa;
        }
    }
    pthread_rwlock_unlock(&MeterFileAll.rwLock);
    return tsa;
}

void GetLineLossMeter()
{
	uint16 i=0;
	uint8 j=0;
	uint16 linevalue=0;
	
    LineMeter.MeterCount=0;

	for(i=0;i<MeterFileAll.MeterCount;i++)
	{
		for(j=0;j<MeterFileAll.MeterFile[i].annex.nNum;j++)
		{
			if(MeterFileAll.MeterFile[i].annex.annexObj[j].oad.value ==0x60500200)
			{
			
				memcpy_r((uint8*)&linevalue,MeterFileAll.MeterFile[i].annex.annexObj[j].value,2);
				if(linevalue&0x0010)//不参与线损计算
				{
					break;
				}
				else if(linevalue&0x0001)//把总表放在第一个位置；这种写法不好，后面优化
				{
					 LineMeter.MeterFile[0].Pn = MeterFileAll.MeterFile[i].nIndex;
					 LineMeter.MeterFile[0].lineword = linevalue;
					 PDSDB_FMT_DEBUG("参与线损的总表测量点号 infonum %d ",LineMeter.MeterFile[0].Pn);
				}
				else
				{
					LineMeter.MeterFile[LineMeter.MeterCount].Pn = MeterFileAll.MeterFile[i].nIndex;
					LineMeter.MeterFile[LineMeter.MeterCount].lineword = linevalue;
					PDSDB_FMT_DEBUG("参与线损的户表表测量点号 infonum %d ",LineMeter.MeterFile[LineMeter.MeterCount].Pn);
				}
				LineMeter.MeterCount++;
				break;
			}
		}
	}
}
uint16 MeterFileInit(DB_CLIENT clientid)
{
    memset(&MeterFileAll,0x00,sizeof(Meter_File_T));
    pthread_rwlock_init( &MeterFileAll.rwLock, NULL ) ;
	memset(&LineMeter,0x00,sizeof(LineMeter));
    
    uint16  MeterCount=0;
    uint16   MeterNum=0;     //档案个数
    int   ret;
    uint16 i;
    OOP_METER_T gMeterFile;     //每个电表的档案信息
    READ_RECORD_T      inReadRecord;
    uint32             handle = 0;
    uint32             recNum = 0;
    STEP_READ_RECORD_T stepReadRecord;
    RECORD_UNIT_T      stepRecordOut;
    memset(&inReadRecord, 0, sizeof(READ_RECORD_T));
    memset(&stepReadRecord, 0, sizeof(STEP_READ_RECORD_T));
    memset(&stepRecordOut, 0, sizeof(RECORD_UNIT_T));
    PDS_FMT_DEBUG("开始档案初始化!  \n");
    pthread_rwlock_wrlock(&MeterFileAll.rwLock);

    inReadRecord.recordOAD.logicId = 0;
    inReadRecord.recordOAD.infoNum = 0xffff;
    inReadRecord.cType = COL_NO_INFO;
    inReadRecord.cStart = 1;
    inReadRecord.cEnd = MTR_MAX_COUNT;
    inReadRecord.sortType = UP_SORT;
    inReadRecord.recordOAD.road.oadMain.value = 0;
    inReadRecord.recordOAD.road.oadCols.nNum = 0x01;
    inReadRecord.recordOAD.road.oadCols.oad[0].value = 0x60000200;

    stepReadRecord.MAC.nNum = 0;
    ret = db_read_record_start(clientid, &inReadRecord, &handle, &recNum);
    if(ret!=0 || recNum==0)
    {
        PDS_FMT_DEBUG("开始读档案记录失败 ret  %d recNum %d \n",ret,recNum);
    }
    else
    {
        for(i=0;i<recNum;i++)
        {
            ret = db_read_record_step(clientid, handle, &stepReadRecord, &stepRecordOut);
            if(ret!=0)
            {
                PDS_FMT_DEBUG("单步读档案记录失败 ret  %d i %d \n",ret,i);
                continue;
            }
            if(stepRecordOut.recordnum!=1 || stepRecordOut.record[0].road.oadCols.oad[0].value!=0x60000200
                ||stepRecordOut.record[0].outDatalen!=sizeof(OOP_METER_T))
            {
                PDS_FMT_DEBUG( "单步读取的档案记录有问题 \n");
                continue;
            }
            memcpy(&gMeterFile,stepRecordOut.record[0].outDataBuf,sizeof(OOP_METER_T));

            PDS_FMT_DEBUG("sizeof(OOP_METER_T) %d 电表序号 %d ret = %d !\n",sizeof(OOP_METER_T),gMeterFile.nIndex,ret);
            PDS_FMT_DEBUG("port =%08x prtl = %d pwrType =%d \n",gMeterFile.basic.port.value,gMeterFile.basic.protocol,gMeterFile.basic.pwrType);
            PDS_FMT_DEBUG("地址 0X%02x%02x%02x%02x%02x%02x \n",gMeterFile.basic.tsa.add[0],gMeterFile.basic.tsa.add[1],gMeterFile.basic.tsa.add[2],gMeterFile.basic.tsa.add[3],gMeterFile.basic.tsa.add[4],gMeterFile.basic.tsa.add[5]);

            MeterNum=MeterFileAll.MeterCount;
            MeterFileAll.MeterFile[MeterNum]=gMeterFile;
            MeterFileAll.MeterCount++;
       }
    }
    db_read_record_end(clientid, handle);

    MeterCount = MeterFileAll.MeterCount;
    PDS_FMT_DEBUG("加载完成后档案个数为 %d \n",MeterFileAll.MeterCount);
    pthread_rwlock_unlock(&MeterFileAll.rwLock);
	GetLineLossMeter();
    return MeterCount;

}


/*******************************************************************************
* 函数名称: MeterFileChange 
* 函数功能:档案变更的处理
* 输入参数: pMeterFileAll 电表档案
          choice 1 表示档案变更 新增或者改变
          choice 0 表示档案删除
          gMeterFile中nindex为删除的序号
* 输出参数: 
* 返 回 值: 
*******************************************************************************/
void MeterFileChange(Meter_File_T *pMeterFileAll,uint8 choice,OOP_METER_T gMeterFile)
{
    uint16 i;
    int index =-1;
    
    pthread_rwlock_wrlock(&pMeterFileAll->rwLock);

    PDSDB_FMT_DEBUG("MeterFileChange choice %d \n",choice);
    if(choice==0)
    {
        for(i=0;i<pMeterFileAll->MeterCount;i++)
        {
            if(pMeterFileAll->MeterFile[i].nIndex == gMeterFile.nIndex )
            {
                index = i;
                memset(&pMeterFileAll->MeterFile[i],0x00,sizeof(OOP_METER_T));
                
                break;
            }
        }
        if(index!=-1)
        {
            for(i=index;i<pMeterFileAll->MeterCount-1;i++)
            {
                pMeterFileAll->MeterFile[i] = pMeterFileAll->MeterFile[i+1];
                
            }
            memset(&(pMeterFileAll->MeterFile[pMeterFileAll->MeterCount-1]),0x00,sizeof(OOP_METER_T));
            pMeterFileAll->MeterCount--;
        }
    }
    else
    {
        for(i=0;i<pMeterFileAll->MeterCount;i++)
        {
            //如果当前档案中找到相同的 则直接替换
            if(pMeterFileAll->MeterFile[i].nIndex == gMeterFile.nIndex )
            {
                index = i;
                memcpy(&pMeterFileAll->MeterFile[i],&gMeterFile,sizeof(OOP_METER_T));
                break;
            }
        }
        //如果没有找到 则新增
        if(index==-1)
        {
            pMeterFileAll->MeterFile[pMeterFileAll->MeterCount] = gMeterFile;
            pMeterFileAll->MeterCount++;
        }
    }
    PDSDB_FMT_DEBUG( "档案个数  %d \n",pMeterFileAll->MeterCount);
    pthread_rwlock_unlock(&pMeterFileAll->rwLock);

}


/**
*********************************************************************
* @name：      appdb_task_list_record_init
* @brief：     任务参数变更初始化(记录数据方式)
* @param[in] ：clientid  数据库句柄
* @param[out]：
* @return    ：
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
void appdb_task_list_record_init(int clientid)
{
    int                ret = ERR_NORMAL;
    uint32             i = 0;
    uint32             oopTaskNum = 0;
    OOP_TASK_T         oopTask = {0};
    READ_RECORD_T      inReadRecord;
    uint32             handle = 0;
    uint32             recNum = 0;
    STEP_READ_RECORD_T stepReadRecord;
    RECORD_UNIT_T      stepRecordOut;

    memset(&oopTaskList, 0, sizeof(OOP_TASK_LIST_T));
    memset(&inReadRecord, 0, sizeof(READ_RECORD_T));
    memset(&stepReadRecord, 0, sizeof(STEP_READ_RECORD_T));
    memset(&stepRecordOut, 0, sizeof(RECORD_UNIT_T));

    PDSDB_FMT_DEBUG("Initializing tasks. please wait......\n");
    inReadRecord.recordOAD.logicId = 0;
    inReadRecord.recordOAD.infoNum = 0xffff;
    inReadRecord.cType = COL_NO_INFO;
    inReadRecord.cStart = 1;
    inReadRecord.cEnd = 256;
    inReadRecord.sortType = UP_SORT;
    inReadRecord.recordOAD.optional = 0;
    inReadRecord.recordOAD.road.oadMain.value = 0;
    inReadRecord.recordOAD.road.oadCols.nNum = 0x01;
    inReadRecord.recordOAD.road.oadCols.oad[0].value = 0x60120200;

    //不需要附加校验
    stepReadRecord.MAC.nNum = 0;

    ret = db_read_record_start(clientid, &inReadRecord, &handle, &recNum);
    if ((ret != ERR_OK) || (recNum == 0))
    {
        PDSDB_FMT_DEBUG("appdb_task_list_record_init db_read_record_start err! ret = %d, infoNum = %d recNum = %d\n", 
            ret, inReadRecord.recordOAD.infoNum, recNum);
    }
    else
    {
        PDSDB_FMT_DEBUG("appdb_task_list_record_init db_read_record_start ok ret = %d, infoNum = %d recNum = %d\n", 
            ret, inReadRecord.recordOAD.infoNum, recNum);
        for (i = 0; i < recNum; i++)
        {
            ret = db_read_record_step(clientid, handle, &stepReadRecord, &stepRecordOut);
            if (ret != ERR_OK)
            {
                PDSDB_FMT_DEBUG("appdb_task_list_record_init db_read_record_step err = %d!\n", ret);
                continue;
            }

            //检查数据有效性
            if ((stepRecordOut.recordnum != 1) 
            || (stepRecordOut.record[0].road.oadCols.oad[0].value != 0x60120200)
            || (stepRecordOut.record[0].outDatalen != sizeof(OOP_TASK_T)))
            {
                PDSDB_FMT_DEBUG("appdb_task_list_record_init data err !\n");
                PDSDB_FMT_DEBUG("appdb_task_list_record_init stepRecordOut.recordnum = %d\n", stepRecordOut.recordnum);
                PDSDB_FMT_DEBUG("appdb_task_list_record_init stepRecordOut.record[0].road.oadCols.oad[0] = 0x%08x\n", 
                    stepRecordOut.record[0].road.oadCols.oad[0].value);
                PDSDB_FMT_DEBUG("appdb_task_list_record_init stepRecordOut.record[0].outDatalen = %d\n", 
                    stepRecordOut.record[0].outDatalen);
                PDSDB_FMT_DEBUG("appdb_task_list_record_init sizeof(OOP_TASK_T) = %d\n", 
                    sizeof(OOP_TASK_T));
                continue;
            }

            memcpy(&oopTask, stepRecordOut.record[0].outDataBuf, stepRecordOut.record[0].outDatalen);
            PDSDB_FMT_DEBUG("stepRecordOut.infoNum = %d\n", stepRecordOut.infoNum);
            PDSDB_FMT_DEBUG("oopTask.id = %d\n", oopTask.id);
            PDSDB_FMT_DEBUG("oopTask.planType = %d\n", oopTask.planType);
            PDSDB_FMT_DEBUG("oopTask.planID = %d\n", oopTask.planID);

            oopTaskNum++;

            //序号从1开始
            oopTaskList.isChanged[stepRecordOut.infoNum] = TRUE;
            oopTaskList.isVaild[stepRecordOut.infoNum] = TRUE;
            oopTaskList.oopTask[stepRecordOut.infoNum] = oopTask;
            
        }
    }
    db_read_record_end(clientid, handle);
    
    PDSDB_FMT_DEBUG("Initialize tasks end.\n");

    /* 记录任务数量 */
    oopTaskList.oopTaskNum = oopTaskNum;

    PDSDB_FMT_DEBUG("read task num: %d\n", oopTaskList.oopTaskNum);

    return;
}



/**
*********************************************************************
* @name：      appdb_plan_normal_list_record_init
* @brief：     普通采集方案参数变更初始化(记录数据方式)
* @param[in] ：clientid  数据库句柄
* @param[out]：
* @return    ：
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
void appdb_plan_normal_list_record_init(int clientid)
{
    int                ret = ERR_NORMAL;
    uint32             i = 0;
    uint32             oopPlanNormalNum = 0;
    OOP_PLAN_NORMAL_T  oopPlanNormal = {0};
    READ_RECORD_T      inReadRecord;
    uint32             handle = 0;
    uint32             recNum = 0;
    STEP_READ_RECORD_T stepReadRecord;
    RECORD_UNIT_T      stepRecordOut;

    memset(&oopPlanNormalList, 0, sizeof(OOP_PLAN_NORMAL_LIST_T));
    memset(&inReadRecord, 0, sizeof(READ_RECORD_T));
    memset(&stepReadRecord, 0, sizeof(STEP_READ_RECORD_T));
    memset(&stepRecordOut, 0, sizeof(RECORD_UNIT_T));

    PDSDB_FMT_DEBUG("Initializing plan normals. please wait......\n");
    inReadRecord.recordOAD.logicId = 0;
    inReadRecord.recordOAD.infoNum = 0xffff;
    inReadRecord.cType = COL_NO_INFO;
    inReadRecord.cStart = 1;
    inReadRecord.cEnd = 256;
    inReadRecord.sortType = UP_SORT;
    inReadRecord.recordOAD.optional = 0;
    inReadRecord.recordOAD.road.oadMain.value = 0;
    inReadRecord.recordOAD.road.oadCols.nNum = 0x01;
    inReadRecord.recordOAD.road.oadCols.oad[0].value = 0x60140200;

    //不需要附加校验
    stepReadRecord.MAC.nNum = 0;

    ret = db_read_record_start(clientid, &inReadRecord, &handle, &recNum);
    if ((ret != ERR_OK) || (recNum == 0))
    {
        PDSDB_FMT_DEBUG("appdb_plan_normal_list_record_init db_read_record_start err! ret = %d, infoNum = %d recNum = %d\n", 
            ret, inReadRecord.recordOAD.infoNum, recNum);
    }
    else
    {
        PDSDB_FMT_DEBUG("appdb_plan_normal_list_record_init db_read_record_start ok ret = %d, infoNum = %d recNum = %d\n", 
            ret, inReadRecord.recordOAD.infoNum, recNum);
        for (i = 0; i < recNum; i++)
        {
            ret = db_read_record_step(clientid, handle, &stepReadRecord, &stepRecordOut);

            if (ret != ERR_OK)
            {
                PDSDB_FMT_DEBUG("appdb_plan_normal_list_record_init db_read_record_step err = %d!\n", ret);
                continue;
            }

            //检查数据有效性
            if ((stepRecordOut.recordnum != 1) 
            || (stepRecordOut.record[0].road.oadCols.oad[0].value != 0x60140200)
            || (stepRecordOut.record[0].outDatalen != sizeof(OOP_PLAN_NORMAL_T)))
            {
                PDSDB_FMT_DEBUG("appdb_plan_normal_list_record_init data err !\n");
                PDSDB_FMT_DEBUG("appdb_plan_normal_list_record_init stepRecordOut.recordnum = %d\n", stepRecordOut.recordnum);
                PDSDB_FMT_DEBUG("appdb_plan_normal_list_record_init stepRecordOut.record[0].road.oadCols.oad[0] = 0x%08x\n", 
                    stepRecordOut.record[0].road.oadCols.oad[0].value);
                PDSDB_FMT_DEBUG("appdb_plan_normal_list_record_init stepRecordOut.record[0].outDatalen = %d\n", 
                    stepRecordOut.record[0].outDatalen);
                PDSDB_FMT_DEBUG("appdb_plan_normal_list_record_init sizeof(OOP_PLAN_NORMAL_T) = %d\n", 
                    sizeof(OOP_PLAN_NORMAL_T));
                continue;
            }

            memcpy(&oopPlanNormal, stepRecordOut.record[0].outDataBuf, stepRecordOut.record[0].outDatalen);
            PDSDB_FMT_DEBUG("stepRecordOut.infoNum = %d\n", stepRecordOut.infoNum);
            PDSDB_FMT_DEBUG("oopPlanNormal.planID = %d\n", oopPlanNormal.planID);
            PDSDB_FMT_DEBUG("oopPlanNormal.memDepth = %d\n", oopPlanNormal.memDepth);
            PDSDB_FMT_DEBUG("oopPlanNormal.recdRow.choice = %d\n", oopPlanNormal.recdRow.choice);
            PDSDB_FMT_DEBUG("oopPlanNormal.tmFlag = %d\n", oopPlanNormal.tmFlag);

            oopPlanNormalNum++;

            //序号从1开始
            oopPlanNormalList.isVaild[stepRecordOut.infoNum] = TRUE;
            oopPlanNormalList.oopPlanNormal[stepRecordOut.infoNum] = oopPlanNormal;
            
        }
    }
    db_read_record_end(clientid, handle);
    
    PDSDB_FMT_DEBUG("Initialize plan normals end.\n");

    /* 记录数量 */
    oopPlanNormalList.oopPlanNormalNum = oopPlanNormalNum;
    oopPlanNormalList.isChanged = TRUE;

    PDSDB_FMT_DEBUG("read plan normals num: %d\n", oopPlanNormalList.oopPlanNormalNum);

    return;
}


/**********************************************************************
* @name      : DBGetMeterFile
* @brief     ：收到数据变更事件后读取档案信息
* @param[in] ：int clientid    socket句柄
               uint8 logicId   逻辑地址
               uint16 infoNum  信息点号
               uint32 value    oad值
* @param[out]：
* @return    ：0-成功/错误码
**********************************************************************/
int DBGetMeterFile(int clientid, uint8 logicId, uint16 infoNum, uint32 value)
{
    int                ret = 0;
    uint32             readlen = 0;
    NOMAL_OAD_T        tOAD = {0};
    OOP_METER_T        oopMeter = {0};

    if(0x60000200 != value)
    {
        return -1;
    }
    tOAD.logicId   = logicId;
    tOAD.infoNum   = infoNum;
    tOAD.oad.value = value;
    PDSDB_FMT_DEBUG("收到档案的数据变更事件 Logicid %d infonum %d \n",logicId,infoNum);
    if(infoNum==0xffff)
    {
        pthread_rwlock_wrlock(&MeterFileAll.rwLock);
        MeterFileAll.MeterCount = 0;
        memset(MeterFileAll.MeterFile,0x00,sizeof(OOP_METER_T)*MTR_MAX_COUNT);
        pthread_rwlock_unlock(&MeterFileAll.rwLock);
        return 0;
    }
    ret = db_read_nomal(clientid, &tOAD, sizeof(OOP_METER_T), (uint8 *)&oopMeter, &readlen);
    if(0 != ret || readlen != sizeof(OOP_METER_T))
    {
       oopMeter.nIndex = infoNum;
       MeterFileChange(&MeterFileAll,0,oopMeter);
       return 0;
    }
    MeterFileChange(&MeterFileAll,1,oopMeter);
	GetLineLossMeter();
    return 0;
}

void task_list_lock_init(void)
{
    /* 初始化锁 */
     memset(&oopTaskList, 0, sizeof(OOP_TASK_LIST_T));
    pthread_mutex_init(&oopTaskList.mutex, NULL);
}

void plan_normal_list_lock_init(void)
{
    /* 初始化锁 */
    memset(&oopPlanNormalList, 0, sizeof(OOP_PLAN_NORMAL_LIST_T));
    pthread_mutex_init(&oopPlanNormalList.mutex, NULL);
}


/**********************************************************************
* @name      : app_set_task_list
* @brief     ：将更新的任务数据写入缓存
* @param[in] ：taskID  - 任务序号          
               oopTask - 任务数据
* @return    ：错误码
**********************************************************************/
int app_set_task_list(uint8 taskID, OOP_TASK_T *oopTask)
{
    if(NULL == oopTask)
    {
        return ERR_PNULL;
    }

    /* 使用互斥量进行加锁 */
    pthread_mutex_lock(&oopTaskList.mutex);

    oopTaskList.oopTask[taskID] = *oopTask;
    oopTaskList.isVaild[taskID] = TRUE;
    oopTaskList.isChanged[taskID] = TRUE;
    if(oopTaskList.isVaild[taskID]==FALSE)
    {
        oopTaskList.oopTaskNum ++;
    }

    /* 使用互斥量进行解锁 */
    pthread_mutex_unlock(&oopTaskList.mutex);

    return ERR_OK;
}

/**********************************************************************
* @name      : app_set_task_invalid
* @brief     ：设置任务数据无效
* @param[in] : taskID - 任务序号
* @param[out]：
* @return    ：错误码
**********************************************************************/
int app_set_task_invalid(uint8 taskID)
{

    /* 使用互斥量进行加锁 */
    pthread_mutex_lock(&oopTaskList.mutex);

    memset(&oopTaskList.oopTask[taskID], 0, sizeof(OOP_TASK_T));
    oopTaskList.isVaild[taskID] = FALSE;
    oopTaskList.isChanged[taskID] = TRUE;
    oopTaskList.oopTaskNum --;

    /* 使用互斥量进行解锁 */
    pthread_mutex_unlock(&oopTaskList.mutex);

    return ERR_OK;
}


/**********************************************************************
* @name      : appdb_get_task_list
* @brief     ：任务列表到缓存
* @param[in] ：int clientid    socket句柄
               uint8 logicId   逻辑地址
               uint16 infoNum  信息点号
               uint32 value    oad值
* @param[out]：
* @return    ：0-成功/错误码
**********************************************************************/
int appdb_get_task_list(int clientid, uint8 logicId, uint16 infoNum, uint32 value)
{
    int                ret = 0;
    uint32             readlen = 0;
    uint32             i = 0;
    NOMAL_OAD_T        tOAD = {0};
    OOP_TASK_T         oopTask = {0};

    if(0x60120200 != value)
    {
        return -1;
    }

    tOAD.logicId   = logicId;
    tOAD.infoNum   = infoNum;
    tOAD.oad.value = value;

    PDSDB_FMT_DEBUG("appdb_get_task_list logicId(%d) infoNum(%d) value(0x%x) running.....\n", logicId, infoNum, value);

    if (infoNum == 0xffff)
    {
        oopTaskList.oopTaskNum = 0;
        for (i = 1; i <= 255; i++)
        {
            //判断档案是否有效
            if (!oopTaskList.isVaild[i])
            {
                continue;
            }

            if (app_set_task_invalid(i) < ERR_OK)
            {
                PDSDB_FMT_DEBUG("app_set_task_invalid err\n");
            }
        }      
        return 0;
    }

    ret = db_read_nomal(clientid, &tOAD, sizeof(OOP_TASK_T), (uint8 *)&oopTask, &readlen);
    if(0 != ret || readlen != sizeof(OOP_TASK_T))
    {
        app_set_task_invalid(infoNum);
        PRTL_FMT_DEBUG("db_read_nomal failed and app_set_task_invalid. ret(%d), infoNum(%d), readlen(%d), explen(%d)\n", ret, infoNum, readlen, sizeof(OOP_TASK_T));
        return ERR_NORMAL;
    }

    PDSDB_FMT_DEBUG("oopTask.taskID = %d\n", oopTask.id);
    PDSDB_FMT_DEBUG("oopTask.acqFreq.value = %d\n", oopTask.acqFreq.value);
    PDSDB_FMT_DEBUG("oopTask.acqFreq.unit = %d\n", oopTask.acqFreq.unit);
    PDSDB_FMT_DEBUG("oopTask.planType = %d\n", oopTask.planType);
    PDSDB_FMT_DEBUG("oopTask.planID = %d\n", oopTask.planID);
    PDSDB_FMT_DEBUG("oopTask.startTime = %d\n", oopTask.startTime);
    PDSDB_FMT_DEBUG("oopTask.endTime = %d\n", oopTask.endTime);
    PDSDB_FMT_DEBUG("oopTask.acqDelay = %d\n", oopTask.acqDelay);
    PDSDB_FMT_DEBUG("oopTask.state = %d\n", oopTask.state);
    
    ret = app_set_task_list(infoNum, &oopTask);
    if (ret != ERR_OK)
    {
        app_set_task_invalid(infoNum);
        PDSDB_FMT_DEBUG("app_set_task_list failed. ret(%d), infoNum(%d)\n", ret, infoNum);
        return ERR_NORMAL;
    }
    
    return 0;
}



/**********************************************************************
* @name      : app_set_plan_normal_invalid
* @brief     ：设置普通采集方案数据无效
* @param[in] : planID - 上报方案序号
* @param[out]：
* @return    ：错误码
* @Create    : 梅安宁
* @Date      ：2020-2-27
* @Update    :
**********************************************************************/
int app_set_plan_normal_invalid(uint8 planID)
{

    /* 使用互斥量进行加锁 */
    pthread_mutex_lock(&oopPlanNormalList.mutex);

    memset(&oopPlanNormalList.oopPlanNormal[planID], 0, sizeof(OOP_PLAN_NORMAL_T));
    oopPlanNormalList.isVaild[planID] = FALSE;
    oopPlanNormalList.isChanged = TRUE;
    oopPlanNormalList.oopPlanNormalNum --;

    /* 使用互斥量进行解锁 */
    pthread_mutex_unlock(&oopPlanNormalList.mutex);

    return ERR_OK;
}


/**********************************************************************
* @name      : app_set_plan_normal_list
* @brief     ：将更新的普通采集方案数据写入缓存
* @param[in] ：planID          - 方案序号          
* @param[out]: oopPlanNormal  - 方案数据
* @return    ：错误码
* @Create    : 梅安宁
* @Date      ：2020-2-27
* @Update    :
**********************************************************************/
int app_set_plan_normal_list(uint8 planID, OOP_PLAN_NORMAL_T *oopPlanNormal)
{
    if(NULL == oopPlanNormal)
    {
        return ERR_PNULL;
    }

    /* 使用互斥量进行加锁 */
    pthread_mutex_lock(&oopPlanNormalList.mutex);

    oopPlanNormalList.oopPlanNormal[planID] = *oopPlanNormal;
    oopPlanNormalList.isVaild[planID] = TRUE;
    oopPlanNormalList.isChanged = TRUE;
    if(oopPlanNormalList.isVaild[planID]==FALSE)
    {
        oopPlanNormalList.oopPlanNormalNum ++;
    }

    /* 使用互斥量进行解锁 */
    pthread_mutex_unlock(&oopPlanNormalList.mutex);

    return ERR_OK;
}


/**********************************************************************
* @name      : appdb_get_plan_normal_list
* @brief     ：普通采集方案列表到缓存
* @param[in] ：int clientid    socket句柄
               uint8 logicId   逻辑地址
               uint16 infoNum  信息点号
               uint32 value    oad值
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 梅安宁
* @Date      ：2020-2-27
* @Update    :
**********************************************************************/
int appdb_get_plan_normal_list(int clientid, uint8 logicId, uint16 infoNum, uint32 value)
{
    int                i = 0;
    int                ret = 0;
    uint32             readlen = 0;
    NOMAL_OAD_T        tOAD = {0};
    OOP_PLAN_NORMAL_T  oopPlanNormal = {0};

    if(0x60140200 != value)
    {
        return -1;
    }

    tOAD.logicId   = logicId;
    tOAD.infoNum   = infoNum;
    tOAD.oad.value = value;

    PDSDB_FMT_DEBUG("appdb_get_meter_list logicId(%d) infoNum(%d) value(0x%x) running.....\n", logicId, infoNum, value);

    if (infoNum == 0xffff)
    {
        oopPlanNormalList.oopPlanNormalNum = 0;
        for (i = 1; i <= 255; i++)
        {
            //判断是否有效
            if (!oopPlanNormalList.isVaild[i])
            {
                continue;
            }

            if (app_set_plan_normal_invalid(i) < ERR_OK)
            {
                PDSDB_FMT_DEBUG("app_set_plan_normal_invalid err\n");
            }
        }      
        return 0;
    }

    ret = db_read_nomal(clientid, &tOAD, sizeof(OOP_PLAN_NORMAL_T), (uint8 *)&oopPlanNormal, &readlen);
    if(0 != ret || readlen != sizeof(OOP_PLAN_NORMAL_T))
    {
        app_set_plan_normal_invalid(infoNum);
        PDSDB_FMT_DEBUG("db_read_nomal failed. ret(%d), infoNum(%d), readlen(%d), explen(%d)\n", ret, infoNum, readlen, sizeof(OOP_PLAN_NORMAL_T));
        return ERR_NORMAL;
    }
    
    ret = app_set_plan_normal_list(infoNum, &oopPlanNormal);
    if (ret != ERR_OK)
    {
        app_set_plan_normal_invalid(infoNum);
        PDSDB_FMT_DEBUG("app_set_plan_normal_list failed. ret(%d), infoNum(%d)\n", ret, infoNum);
        return ERR_NORMAL;
    }
    
    return 0;
}

/**********************************************************************
* @name      : topology_start
* @brief     ：处理启动拓扑的消息
* @param[in] ：MESSAGE_INFO_T *pMsg 消息结构体
* @param[out]：
* @return    ：

**********************************************************************/
void topology_start(MESSAGE_INFO_T *pMsg)
{
    uint16 topotime;
    int ret;
    if(pMsg->msglen!=2)
    {
        MQTTMSG_FMT_TRACE("启动拓扑消息 msglen=%d 错误  \n",pMsg->msglen);
        return;
    }
    memcpy_r(&topotime,pMsg->playload,pMsg->msglen);
    MQTTMSG_FMT_TRACE("启动拓扑消息 拓扑时长 %d 秒  \n",topotime);

    ret = task_send_msg(gMqttSock,MSG_PDS_TOPOSTART,(uint8*)&topotime,sizeof(uint16),PDS_CALC_IPC);
    if(ret!=0)
    {
        MQTTMSG_FMT_TRACE("task_send_msg to calc error ret %d \n",ret);
    }
    
}

/**********************************************************************
* @name      : topology_start
* @brief     ：处理启动拓扑的消息
* @param[in] ：MESSAGE_INFO_T *pMsg 消息结构体
* @param[out]：
* @return    ：

**********************************************************************/
void checkTopology(uint8 *buf,uint16 len)
{
    if(len!= sizeof(uint16 ))
    {
        PDS_FMT_DEBUG("checkTopology len=%d error \n",len);
        return;
    }
    uint16 topotime;
    memcpy(&topotime,buf,len);
    if(gCalcArg.topologyArg.isInTopo ==1)
    {
        PDS_FMT_DEBUG("当前正在拓扑识别过程中，不响应拓扑启动命令 \n");
        return;
    }
    gCalcArg.topologyArg.isInTopo = 1;
}

void calcThreadInit()
{
    memset(&gCalcArg,0x00,sizeof(Calc_Thread_Arg));
    gCalcSock = task_msg_init(PDS_CALC_IPC);
    if(gCalcSock<0)
    {
        PDS_FMT_DEBUG("创建PDS_CALC_IPC失败 ！！ \n");
        return ;
    }
    task_msg_settime(gCalcSock,0,100000);
    DT_Time2DateTimeHex(time(NULL),&gCurBinTime);
    memcpy(&gPreBinTime,&gCurBinTime,sizeof(DateTimeHex_t));
    gCalcArg.clientid = db_client_create(DB_PDS_AGENT, DB_PDS_CLIENT_COMMON);
    if(gCalcArg.clientid <0)
    {
        PDS_FMT_DEBUG("DB_PDS_CLIENT_COMMON db_client_create失败 ！！ \n");
        return ;
    }
    LoadTopoInfoFile(&gCalcArg.topologyArg);
    gImpeEventIndex = GetEventIndex(gCalcArg.clientid,0x31F20200);
    gImpeEventIndex++;
    memset(&gDaytaskEndTime,0x00,sizeof(gDaytaskEndTime));
    memset(&gMonthtaskEndTime,0x00,sizeof(gMonthtaskEndTime));
    memset(&gMintaskEndTime,0x00,sizeof(gMintaskEndTime));
}


void LinelossThreadInit()
{
    lineloss = db_client_create(DB_PDS_AGENT, DB_PDS_CLIENT_LINELOSS);
    if(lineloss <0)
    {
        PDS_FMT_DEBUG("DB_PDS_CLIENT_LINELOSS失败 ！！ \n");
        return ;
    }
}


/*******************************************************************************
* 函数名称: TimeManage
* 函数功能: 时间管理函数
* 输入参数: 
* 输出参数: 
* 返 回 值: 
*******************************************************************************/
void TimeManage()
{
    gPastTimeFlag.Word = 0;
    DT_Time2DateTimeHex(time(NULL),&gCurBinTime);

    //过月
    if (gCurBinTime.month != gPreBinTime.month)
    {
        //同时产生过月、过日、过时、过分、过秒标志
        gPastTimeFlag.BitSect.Month = TRUE;
        gPastTimeFlag.BitSect.Day = TRUE;
        gPastTimeFlag.BitSect.Hour = TRUE;
        gPastTimeFlag.BitSect.Minute = TRUE;
        gPastTimeFlag.BitSect.Second = TRUE;
    }
    //过日
    if (gCurBinTime.day != gPreBinTime.day)
    {
        //同时产生过日、过时、过分标志
        gPastTimeFlag.BitSect.Day = TRUE;
        gPastTimeFlag.BitSect.Hour = TRUE;
        gPastTimeFlag.BitSect.Minute = TRUE;
        gPastTimeFlag.BitSect.Second = TRUE;
    }
    //过时
    else if (gCurBinTime.hour != gPreBinTime.hour)
    {
        //同时产生过时、过分标志
        gPastTimeFlag.BitSect.Hour = TRUE;
        gPastTimeFlag.BitSect.Minute = TRUE;
        gPastTimeFlag.BitSect.Second = TRUE;
    }
    //过分
    else if (gCurBinTime.min != gPreBinTime.min)
    {
        //产生过分标志
        gPastTimeFlag.BitSect.Minute = TRUE;
        gPastTimeFlag.BitSect.Second = TRUE;
//        if((access(DELAY_FILE_PATH,F_OK)==0))
//        {    
//            if(gdelay == 0)
//            {
//                gdelay = 1;
//                PDS_FMT_DEBUG("检测到开始减压\n");
//            }
//        }
//        else
//        {
//            if(gdelay == 1)
//            {
//                gdelay = 0;
//            }
//        }
        if((access(DELAY_FILE_PATH,F_OK)==0))
        {    
            if(gdelay == 0)
            {
                gdelay = 1;
                gdelaytime = DELAYTIME;
                PDS_FMT_DEBUG("检测到开始减压\n");
            }
        }else if((access(DELAY_FILE_PATH2,F_OK)==0))
        {
            if(gdelay != 2)
            {
                gdelay = 2;
                gdelaytime = DELAYTIME_MORE;
                PDS_FMT_DEBUG("进一步减压\n");

            }
        }
        else
        {
            if(gdelay != 0)
            {
                gdelay = 0;
                gdelaytime = 0;
            }
        }
    }

    //过秒
    else if (gCurBinTime.sec != gPreBinTime.sec)
    {
        gPastTimeFlag.BitSect.Second = TRUE;
    }
    else
    {
        usleep(10000);
    }

    memcpy(&gPreBinTime, &gCurBinTime, sizeof(DateTimeHex_t));
}

/*******************************************************************************
* 函数名称: PackMeterPrtl
* 函数功能: 645电表规约组帧
* 输入参数: MeterData           电表规约数据
* 输出参数: PrtlData            组帧后数据
* 返 回 值:  帧长度
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
* 函数名称: getAllBreak
* 函数功能:从档案中获取所有断路器
* 输入参数: 
* 输出参数: 
* 返 回 值: 断路器档案个数
*******************************************************************************/
uint16 getAllBreaks()
{
    uint16 i;
    Topology_Arg_T *gTopologyArg = &gCalcArg.topologyArg;
    uint16 count = 0;
    uint8 usertype;
    gTopologyArg->breakCount = 0;
    memset(gTopologyArg->breakInfo,0x00,sizeof(Break_Info_T)*MAX_BREAK_COUNT);
    pthread_rwlock_rdlock(&MeterFileAll.rwLock);
    for(i=0;i<MeterFileAll.MeterCount;i++)
    {
        usertype = MeterFileAll.MeterFile[i].basic.userType;
        if(usertype == USERTYPE_LOW_BREAKER||usertype==USERTYPE_METERBOX_BREAKER)
        {
            gTopologyArg->breakInfo[count].Pn = MeterFileAll.MeterFile[i].nIndex;
            gTopologyArg->breakInfo[count].usertype = usertype;
            memcpy_r(gTopologyArg->breakInfo[count].Addr,MeterFileAll.MeterFile[i].basic.tsa.add,6);
            gTopologyArg->breakInfo[count].port = MeterFileAll.MeterFile[i].basic.port.value;
            count++;
        }
    }
    pthread_rwlock_unlock(&MeterFileAll.rwLock);
    gTopologyArg->breakCount = count;
    return count;
}

void mainInit()
{
    memset(&gTransQueue,0x00,sizeof(LockQueue_T));
    pthread_mutex_init(&gTransQueue.QueueLock , NULL);
    memset(&gLineLossPeriod,0x00,sizeof(Line_Loss_Period_T));
    pthread_mutex_init(&gLineLossPeriod.lock , NULL);
    memset(&gImpeLimit,0x00,sizeof(Line_Impedance_Limit_T));
    pthread_mutex_init(&gImpeLimit.lock , NULL);
}

/*******************************************************************************
* 函数名称: UnpackProtocol_645
* 函数功能:按照645规约解析报文
* 输入参数: buf 报文
        len 报文长度
* 输出参数: data 解析出来的数据
* 返 回 值: 0 解析成功 -1 解析错误
*******************************************************************************/
int UnpackProtocol_645(uint8 *buf,uint16 len,MeterPrtlFmt_T *data)
{
    uint16 index = 0xff;
    uint8 cs = 0;
    uint16 i;
    MeterPrtlFmt_T tmp;
    for(i=0;i<len;i++)
    {
        if(buf[i]==0x68 &&buf[i+7]==0x68)
        {
            index = i;
            break;
        }
    }
    if(index == 0xff)
    {
        return -1;
    }
    //地址域
    memcpy(tmp.MtrAddr,&buf[index+1],6);
    //控制码
    tmp.CtrlCode = buf[index+8];
    //数据域长度
    tmp.DataLen = buf[index+9];

    if(buf[index+tmp.DataLen+11]!=0x16)
    {
        return -2;
    }
    
    for(i=0;i<tmp.DataLen+10;i++)
    {
        cs = (cs + buf[index+i])%256;
    }
    
    if(cs!=buf[index+tmp.DataLen+10])
    {
        return -3;
    }
    for(i=0;i<tmp.DataLen;i++)
    {
        tmp.DataBuff[i] = buf[index+10+i]-0x33;
    }
    memcpy(data,&tmp,sizeof(MeterPrtlFmt_T));
    return 0;
}

/*******************************************************************************
* 函数名称: DelayDeal
* 函数功能: 拓扑识别延时处理
* 输入参数: delaysec 延时参数
* 输出参数: 无
* 返 回 值:0 延时结束 -1 继续延时
*******************************************************************************/
int DelayDeal(uint16 *delaysec)
{
    
    if(*delaysec ==0)
    {
        return 0;
    }
    if(gPastTimeFlag.BitSect.Second)
    {
        (*delaysec)--;
    }
    return -1;
}

/*******************************************************************************
* 函数名称: GetNextBreakIndex
* 函数功能: 根据用户类型找到下一个断路器索引
* 输入参数: usertype 用户类型 分支短路器或者表箱断路器
* 输出参数: 
* 返 回 值:断路器索引
*******************************************************************************/
uint16 GetNextBreakIndex(uint8 usertype)
{
    Topology_Arg_T *gTopologyArg = &gCalcArg.topologyArg;
    uint16 breakindex = gTopologyArg->breakIndex;
    while(breakindex<gTopologyArg->breakCount)
    {
        if(gTopologyArg->breakInfo[breakindex].usertype == usertype)
        {
            gTopologyArg->breakIndex = breakindex;
            return breakindex;
        }
        breakindex++;
    }
    gTopologyArg->breakIndex = breakindex;
    return breakindex;
}

/*******************************************************************************
* 函数名称: GetNextBreakIndexReadMeter
* 函数功能: 获取下一个需要读表档案信息的断路器索引 表箱断路器和一级分支
* 输入参数: 
* 输出参数: 
* 返 回 值:断路器索引
*******************************************************************************/
uint16 GetNextBreakIndexReadMeter()
{
    Topology_Arg_T *gTopologyArg = &gCalcArg.topologyArg;
    uint16 breakindex = gTopologyArg->breakIndex;
    while(breakindex<gTopologyArg->breakCount)
    {
        if(gTopologyArg->breakInfo[breakindex].usertype == USERTYPE_METERBOX_BREAKER||
            gTopologyArg->breakInfo[breakindex].branchtype ==1)
        {
            gTopologyArg->breakIndex = breakindex;
            return breakindex;
        }
        breakindex++;
    }
    gTopologyArg->breakIndex = breakindex;
    return breakindex;
}

/*******************************************************************************
* 函数名称: GetNextMeterPLC
* 函数功能: 获取下一个载波档案
* 输入参数: 
* 输出参数: Addr 载波档案地址
* 返 回 值: 档案序号 如果没有则返回0
*******************************************************************************/
uint16 GetNextMeterPLC(uint8 *Addr)
{
    static uint16 i = 0;
    uint16 Pn = 0;
    if(i>=MeterFileAll.MeterCount)
    {
        i = 0;
        return Pn;
    }
    pthread_rwlock_rdlock(&MeterFileAll.rwLock);
    while(i<MeterFileAll.MeterCount)
    {
        if(MeterFileAll.MeterFile[i].basic.port.value==0xF2090201||MeterFileAll.MeterFile[i].basic.port.value==0xF2090202)
        {
            Pn = MeterFileAll.MeterFile[i].nIndex;
            memcpy_r(Addr,MeterFileAll.MeterFile[i].basic.tsa.add,6);
            i++;
            break;
        }
        i++;
    }

    pthread_rwlock_unlock(&MeterFileAll.rwLock);
    return Pn;
}

/*******************************************************************************
* 函数名称: GetNextMeterPLC
* 函数功能: 获取下一个载波电表档案
* 输入参数: 
* 输出参数: Addr 载波档案地址
* 返 回 值: 档案序号 如果没有则返回0
*******************************************************************************/
uint16 GetNextNonBreakPLC(uint8 *Addr)
{
    static uint16 i = 0;
    uint16 Pn = 0;

    pthread_rwlock_rdlock(&MeterFileAll.rwLock);
    while(i<MeterFileAll.MeterCount)
    {
        if((MeterFileAll.MeterFile[i].basic.port.value==0xF2090201||MeterFileAll.MeterFile[i].basic.port.value==0xF2090202)
            &&(MeterFileAll.MeterFile[i].basic.userType != USERTYPE_METERBOX_BREAKER)
            &&(MeterFileAll.MeterFile[i].basic.userType != USERTYPE_LOW_BREAKER)
            &&(MeterFileAll.MeterFile[i].basic.userType != USERTYPE_FRAME_BREAKER))
        {
            Pn = MeterFileAll.MeterFile[i].nIndex;
            memcpy_r(Addr,MeterFileAll.MeterFile[i].basic.tsa.add,6);
            i++;
            break;
        }
        i++;
    }
  
    pthread_rwlock_unlock(&MeterFileAll.rwLock);

    //防止最后一次未找到档案，下次调用时，直接不进循环，和最后一次找到表档案，无限循环
    if((i>=MeterFileAll.MeterCount)&&(Pn == 0))
    {
        i = 0;
    }
    
    return Pn;
}

/*******************************************************************************
* 函数名称: WriteNormalData 
* 函数功能:写普通数据到数据中心
* 输入参数:pipe 端口号
        DataBuf 数据内容
        DataLen 数据长度
        Oad
* 输出参数: 
* 返 回 值:
*******************************************************************************/
int WriteNormalData(DB_CLIENT clientid,uint8* DataBuf,uint16 DataLen,OOP_OAD_U Oad,uint16 infonum)
{

    int ret;
    NOMAL_OAD_T NomalOad;
    NomalOad.logicId = 0;
    NomalOad.oad =Oad;
    NomalOad.classtag = 3;
    NomalOad.infoNum = infonum;

    ret = db_write_nomal(clientid,&NomalOad,DataBuf,DataLen);
    if(ret !=0)
    {
        PDS_FMT_DEBUG("普通数据写入数据库失败 ret %d oad %08x\n",ret,Oad.value);
    }
    return ret;
}

/*******************************************************************************
* 函数名称: bubble_Sort
* 函数功能: 冒泡排序
* 输入参数: 
* 输出参数: 
* 返 回 值:
*******************************************************************************/
void bubble_Sort(void *arr, int n, SWAPFUN swap)
{
    int i,j;
    
    for(i=0;i<n-1;i++)
    {
        for(j=0;j<n-1-i;j++)
        {
            swap(arr, j);
        }
    }

    return;
}

/*******************************************************************************
* 函数名称: SaveTopoResult
* 函数功能: 存储拓扑结果，按档案号存，包含电表
* 输入参数: 
* 输出参数: 
* 返 回 值:
*******************************************************************************/

/*******************************************************************************
* 函数名称: ReadLocalAddr 
* 函数功能: //获取终端通信地址
* 输入参数: 
* 输出参数: addr 终端地址
* 返 回 值:  0成功 -1 失败
*******************************************************************************/
int ReadLocalAddr(DB_CLIENT clientid,uint8 *addr)
{
    int   ret;
    uint32 len=0;
    NOMAL_OAD_T NormalOad;
    NormalOad.logicId = 0;
    NormalOad.oad.value = 0x40010200;
    NormalOad.infoNum = 0;
    OOP_OCTETVAR16_T LocalAddr;
    memset(&LocalAddr, 0, sizeof(OOP_OCTETVAR16_T));
    ret = db_read_nomal(clientid,&NormalOad,sizeof(OOP_OCTETVAR16_T),(uint8*)&LocalAddr,&len);
    if(ret !=0 ||len!=sizeof(OOP_OCTETVAR16_T))
        return -1 ;
    memcpy_r(addr,LocalAddr.value,6);
    PDS_FMT_DEBUG("获取终端地址 0x%02x%02x%02x%02x%02x%02x \n",addr[5],addr[4],addr[3],addr[2],addr[1],addr[0]);
    return 0;
}
/**********************************************************************
* @name      : ReloadLocalAddr
* @brief     ：获取本地地址
* @param[in] ：int clientid    socket句柄
               uint8 logicId   逻辑地址
               uint16 infoNum  信息点号
               uint32 value    oad值
* @param[out]：
* @return    ：0-成功/错误码

**********************************************************************/
int ReloadLocalAddr(int clientid, uint8 logicId, uint16 infoNum, uint32 value)
{
    if(0x40010200 != value)
    {
        return -1;
    }
    ReadLocalAddr(clientid,LocalAddr);
    return 0;
}


/*******************************************************************************
* 函数名称: GetLineLossPeriod 
* 函数功能: //获取线损统计周期 60510400
* 输入参数: 
* 输出参数: addr 终端地址
* 返 回 值:  0成功 -1 失败
*******************************************************************************/
int GetLineLossPeriod(DB_CLIENT clientid)
{
    int   ret;
    uint32 len=0;
    NOMAL_OAD_T NormalOad;
    NormalOad.logicId = 0;
    NormalOad.oad.value = 0x60510400;
    NormalOad.infoNum = 0;
    //uint32 period = 0;
    OOP_TI_T ti;
    memset(&ti, 0, sizeof(OOP_TI_T));
    ret = db_read_nomal(clientid,&NormalOad,sizeof(OOP_TI_T),(uint8*)&ti,&len);
    if(ret !=0 ||len!=sizeof(OOP_TI_T))
    {
        PDS_FMT_DEBUG("获取线损计算周期失败 默认1小时  \n");
        ti.unit = 2;
        ti.value = 1;
    }else
    {
        if(ti.value==0)
        {
            ti.unit = 2;
            ti.value = 1;
        }else
        {
            switch(ti.unit)
            {
                case 0: //配的秒改为15分钟
                {
                    ti.unit = 1;
                    ti.value = 15;
                }
                    break;
                case 1:
                {
                    if(ti.value<15)
                        ti.value = 15;
                }
                    break;
                default:
                {
                }
                    break;
            }
        }

    }

    PDS_FMT_DEBUG("获取线损计算周期 value %d unit %d  \n",ti.value,ti.unit);
    pthread_mutex_lock(&gLineLossPeriod.lock);
    gLineLossPeriod.ti = ti;
    pthread_mutex_unlock(&gLineLossPeriod.lock);
    return 0;
}

/**********************************************************************
* @name      : ReloadLocalAddr
* @brief     ：获取本地地址
* @param[in] ：int clientid    socket句柄
               uint8 logicId   逻辑地址
               uint16 infoNum  信息点号
               uint32 value    oad值
* @param[out]：
* @return    ：0-成功/错误码

**********************************************************************/
int ReloadLineLossPeriod(int clientid, uint8 logicId, uint16 infoNum, uint32 value)
{
    if(0x60510400 != value)
    {
        return -1;
    }
    GetLineLossPeriod(clientid);
    return 0;
}


/*******************************************************************************
* 函数名称: CheckLineLossCalc
* 函数功能:过分检查是否到了线损计算时间
* 输入参数:
* 输出参数: 
* 返 回 值:  
*******************************************************************************/
BOOL CheckLineLossCalc()
{
    Line_Loss_Calc_T *gLineLossArg = &gCalcArg.lineLossArg;
    DateTimeHex_t  timetmp;
    uint16 period;
    OOP_TI_T ti ;
    OOP_TI_T timesub;
    OOP_TI_T timecompensate;
    if(gLineLossArg->isNeedCalc == 1)
    {
        return FALSE;
    }
    timecompensate.unit = 1;
    timecompensate.value = 120;
    pthread_mutex_lock(&gLineLossPeriod.lock);
    ti = gLineLossPeriod.ti;
    pthread_mutex_unlock(&gLineLossPeriod.lock);
    //默认有日线损数据 零点35分开始计算
    if(gCurBinTime.hour == 0 &&gCurBinTime.min == LINE_LOSS_CALC_DAY_DELAY)
    {
        //取当天0点和上一天0点
        gLineLossArg->delay = 0;
        gLineLossArg->mainOad.value =0x50040200;
        memcpy(&timetmp,&gCurBinTime,sizeof(DateTimeHex_t));
        timetmp.min = 0;
        timetmp.sec = 0;
        timesub.unit = 3;
        timesub.value =1;       
        DT_DateTimeHex2Time(&timetmp,&gLineLossArg->lastStoreTime[0]);        
        DT_TimeSubInv(&gLineLossArg->lastStoreTime[0],timesub);
        gLineLossArg->lastStoreTime[1] = gLineLossArg->lastStoreTime[0];
        DT_TimeSubInv(&gLineLossArg->lastStoreTime[1],timesub);
        return TRUE;
    }
    //默认有日冻结 所以如果配的统计周期是日就不用往下处理
    if(ti.unit==3)
    {
        return FALSE;
    }
    //判断是否到线损计算周期 一般最少15分钟 所以和零点10分执行的日线损不会冲突
    uint16 curmin = gCurBinTime.hour*60+gCurBinTime.min;
    //分钟
    if(ti.unit == 1)
    {
        period = ti.value;
        if(period>=60)
        {
            period = 15;
        }
        
        if(curmin%period == 0)
        {
            //取当天0点和上一天0点
            gLineLossArg->delay = LINE_LOSS_CALC_MIN_DELAY*60;
            gLineLossArg->mainOad.value =0x50020200;
            timetmp = gCurBinTime;
            timetmp.sec = 0;

            DT_DateTimeHex2Time(&timetmp,&gLineLossArg->lastStoreTime[0]);
            //由于不确定之后的任务执行时间，所以强制抄前2个小时的。
            DT_TimeSubInv(&gLineLossArg->lastStoreTime[0],timecompensate);
            DT_TimeSubInv(&gLineLossArg->lastStoreTime[0],ti);
            gLineLossArg->lastStoreTime[1] = gLineLossArg->lastStoreTime[0];
            DT_TimeSubInv(&gLineLossArg->lastStoreTime[1],ti);
            return TRUE;
        }

    }
    //小时
    if(ti.unit == 2)
    {
        period = ti.value*60;
        if(curmin%period == 0)
        {
            gLineLossArg->delay =LINE_LOSS_CALC_HOUR_DELAY*60;
            gLineLossArg->mainOad.value =0x50020200;
            timetmp = gCurBinTime;
            timetmp.sec = 0;
            DT_DateTimeHex2Time(&timetmp,&gLineLossArg->lastStoreTime[0]);
            DT_TimeSubInv(&gLineLossArg->lastStoreTime[0],ti);
            gLineLossArg->lastStoreTime[1] = gLineLossArg->lastStoreTime[0];
            DT_TimeSubInv(&gLineLossArg->lastStoreTime[1],ti);
            return TRUE;
        }
    }
    //月 默认一个月
    if(ti.unit == 4)
    {
        //每个月一号的凌晨一点统计 因为过分判断 所以加上分等于0
        if(gCurBinTime.day==1&&gCurBinTime.hour ==1&&gCurBinTime.min == 0)
        {
            gLineLossArg->delay = 0;
            gLineLossArg->mainOad.value =0x50060200;
            timetmp = gCurBinTime;
            timetmp.hour = 0;
            timetmp.min = 0;
            timetmp.sec = 0;
            DT_DateTimeHex2Time(&timetmp,&gLineLossArg->lastStoreTime[0]);
            if(timetmp.month==1)
            {
                timetmp.year_l--;
                timetmp.month = 12;
            }else
            {
                timetmp.month--;
            }
            DT_DateTimeHex2Time(&timetmp,&gLineLossArg->lastStoreTime[1]);
            return TRUE;
        }

    }
    return FALSE;
}
//获取计算的线损的任务：日冻结任务考虑到现场可能有多个，分钟和月冻结也是一样；
void GetTaskRunInfo()
{
	uint16 i;
	memset((uint8*)&LineTask,0x00,sizeof(LineTask));

	for(i=0;i<256;i++)
	{
		if((oopTaskList.oopTask[i].planType ==NORMALPLAN+1)&&(oopTaskList.isVaild[i]))
		{
			if(oopPlanNormalList.oopPlanNormal[oopTaskList.oopTask[i].planID].recdCol.csd[0].choice ==1)
			{
				if(oopPlanNormalList.oopPlanNormal[oopTaskList.oopTask[i].planID].recdCol.csd[0].road.oadMain.value == 0x50040200)
				{
					LineTask.LineDay.taskid[LineTask.LineDay.tasknum].taskid=oopTaskList.oopTask[i].planID;
					LineTask.LineDay.taskid[LineTask.LineDay.tasknum].runti = oopTaskList.oopTask[i].acqFreq;
					if(oopTaskList.oopTask[i].acqDelay.unit ==1)
					{
						LineTask.LineDay.taskid[LineTask.LineDay.tasknum].delaytime = oopTaskList.oopTask[i].acqDelay.value*60;
					}
					else if(oopTaskList.oopTask[i].acqDelay.unit ==0)
					{
						LineTask.LineDay.taskid[LineTask.LineDay.tasknum].delaytime = oopTaskList.oopTask[i].acqDelay.value;
					}
					else if(oopTaskList.oopTask[i].acqDelay.unit ==2)
					{
						LineTask.LineDay.taskid[LineTask.LineDay.tasknum].delaytime = oopTaskList.oopTask[i].acqDelay.value*3600;
					}
					LineTask.LineDay.tasknum++;
				}
				else if((oopPlanNormalList.oopPlanNormal[oopTaskList.oopTask[i].planID].recdCol.csd[0].road.oadMain.value == 0x50020200))
				{
					
					LineTask.LineMinute.taskid[LineTask.LineMinute.tasknum].taskid=oopTaskList.oopTask[i].planID;
					LineTask.LineMinute.taskid[LineTask.LineMinute.tasknum].runti = oopTaskList.oopTask[i].acqFreq;
					if(oopTaskList.oopTask[i].acqDelay.unit ==1)
					{
						LineTask.LineMinute.taskid[LineTask.LineMinute.tasknum].delaytime = oopTaskList.oopTask[i].acqDelay.value*60;
					}
					else if(oopTaskList.oopTask[i].acqDelay.unit ==0)
					{
						LineTask.LineMinute.taskid[LineTask.LineMinute.tasknum].delaytime = oopTaskList.oopTask[i].acqDelay.value;
					}
					else if(oopTaskList.oopTask[i].acqDelay.unit ==2)
					{
						LineTask.LineMinute.taskid[LineTask.LineMinute.tasknum].delaytime = oopTaskList.oopTask[i].acqDelay.value*3600;
					}
					LineTask.LineMinute.tasknum++;

				}
				else if((oopPlanNormalList.oopPlanNormal[oopTaskList.oopTask[i].planID].recdCol.csd[0].road.oadMain.value == 0x50060200))
				{	
					LineTask.LineMonth.taskid[LineTask.LineMonth.tasknum].taskid=oopTaskList.oopTask[i].planID;
					LineTask.LineMonth.taskid[LineTask.LineMonth.tasknum].runti = oopTaskList.oopTask[i].acqFreq;
					if(oopTaskList.oopTask[i].acqDelay.unit ==1)
					{
						LineTask.LineMonth.taskid[LineTask.LineMonth.tasknum].delaytime = oopTaskList.oopTask[i].acqDelay.value*60;
					}
					else if(oopTaskList.oopTask[i].acqDelay.unit ==0)
					{
						LineTask.LineMonth.taskid[LineTask.LineMonth.tasknum].delaytime = oopTaskList.oopTask[i].acqDelay.value;
					}
					else if(oopTaskList.oopTask[i].acqDelay.unit ==2)
					{
						LineTask.LineMonth.taskid[LineTask.LineMonth.tasknum].delaytime = oopTaskList.oopTask[i].acqDelay.value*3600;
					}
					LineTask.LineMonth.tasknum++;
				}
			}
		}

	}
}
//判断要参雨线损计算的任务：日冻结、曲线、月冻结都是否执行完毕，执行完毕后才去获取电表数据
BOOL JudgeTaskRunState(DB_CLIENT clien)
{
	uint8 i=0;

	NOMAL_OAD_T normaload;
	OOP_ACQ_MONITOR_T taskinfo;
	int ret;
	uint32 len;

    memset(&taskinfo,0x00,sizeof(OOP_ACQ_MONITOR_T));
    memset(&normaload,0x00,sizeof(NOMAL_OAD_T));
    normaload.oad.value = 0x60340200;
	
	if(gLineLossPeriod.ti.unit ==3)
	{
		for(i=0;i<LineTask.LineDay.tasknum;i++)
		{
			normaload.infoNum = LineTask.LineDay.taskid[i].taskid;
			ret = db_read_nomal(clien, &normaload, sizeof(OOP_ACQ_MONITOR_T), (uint8*)&taskinfo, &len);
	        PDS_FMT_DEBUG("rpt_normal_task_finish_check taskid %d ret %d taskstate %d \n",LineTask.LineDay.taskid[i].taskid,ret,taskinfo.taskState);
	        if(ret!=0||len!=sizeof(OOP_ACQ_MONITOR_T))
	        {
	            return FALSE;
	        }
	        if(taskinfo.taskState!=2)
	            return FALSE;

            if(memcmp(&taskinfo.endTime,&gDaytaskEndTime,sizeof(OOP_DATETIME_S_T))==0)
                return FALSE;
            memcpy(&gDaytaskEndTime,&taskinfo.endTime,sizeof(OOP_DATETIME_S_T));
            PDS_FMT_DEBUG("开始获取日冻结数据 \n");
            return TRUE;
//			DT_DateTimeShort2Time(&taskinfo.endTime,&linuxTim);
//			if((time(NULL)-linuxTim) <=60)
//			{
//				return TRUE;
//			}else
//                return FALS
		}
	}
	else if(gLineLossPeriod.ti.unit ==4)
	{
		for(i=0;i<LineTask.LineMonth.tasknum;i++)
		{
			normaload.infoNum = LineTask.LineMonth.taskid[i].taskid;
			ret = db_read_nomal(clien, &normaload, sizeof(OOP_ACQ_MONITOR_T), (uint8*)&taskinfo, &len);
	        PDS_FMT_DEBUG("rpt_normal_task_finish_check taskid %d ret %d taskstate %d \n",LineTask.LineDay.taskid[i].taskid,ret,taskinfo.taskState);
	        if(ret!=0||len!=sizeof(OOP_ACQ_MONITOR_T))
	        {
	            return FALSE;
	        }
	        if(taskinfo.taskState!=2)
	            return FALSE;
            if(memcmp(&taskinfo.endTime,&gMonthtaskEndTime,sizeof(OOP_DATETIME_S_T))==0)
                return FALSE;
            memcpy(&gMonthtaskEndTime,&taskinfo.endTime,sizeof(OOP_DATETIME_S_T));
            PDS_FMT_DEBUG("开始获取月冻结数据 \n");
            return TRUE;
		}
	}if(gLineLossPeriod.ti.unit ==1)
	{
		for(i=0;i<LineTask.LineMinute.tasknum;i++)
		{
			normaload.infoNum = LineTask.LineMinute.taskid[i].taskid;
			ret = db_read_nomal(clien, &normaload, sizeof(OOP_ACQ_MONITOR_T), (uint8*)&taskinfo, &len);
	        PDS_FMT_DEBUG("rpt_normal_task_finish_check taskid %d ret %d taskstate %d \n",LineTask.LineMinute.taskid[i],ret,taskinfo.taskState);
	        if(ret!=0||len!=sizeof(OOP_ACQ_MONITOR_T))
	        {
	            return FALSE;
	        }
	        if(taskinfo.taskState!=2)
	            return FALSE;
            if(memcmp(&taskinfo.endTime,&gMintaskEndTime,sizeof(OOP_DATETIME_S_T))==0)
                return FALSE;
            memcpy(&gMintaskEndTime,&taskinfo.endTime,sizeof(OOP_DATETIME_S_T));
            PDS_FMT_DEBUG("开始获取分钟冻结数据 \n");
            return TRUE;
		}
	}
	return FALSE;
}


int GetMeterEnergyDataNew(uint16 pn, OOP_OAD_U mainoad, uint32 storetime, OOP_ENERGY_T* data)
{
    int ret;
    uint8 i,j;
    RECORD_UNIT_T RecordOut;
    READ_RECORD_T startReadRecord;
    OOP_ENERGY_T energys;
    OOP_DATETIME_S_T timetmp;
    OOP_TSA_T tsa;
    DT_Time2DateTimeShort(storetime, &timetmp);
    
    MEMZERO(&energys, sizeof(OOP_ENERGY_T));
    memset((uint8*)&startReadRecord,0x00,sizeof(READ_RECORD_T));
    memset((uint8*)&RecordOut,0x00,sizeof(RECORD_UNIT_T));
    startReadRecord.recordOAD.road.oadMain.value = mainoad.value;
    startReadRecord.recordOAD.road.oadCols.nNum = 2;
    startReadRecord.recordOAD.road.oadCols.oad[0].value =0x00100200;
    startReadRecord.recordOAD.road.oadCols.oad[1].value =0x00200200;

    startReadRecord.recordOAD.logicId = 0;
    startReadRecord.recordOAD.infoNum = pn;
    startReadRecord.recordOAD.classtag =5;
    startReadRecord.MAC.nNum=6;
    tsa=taskmng_tsa_get(pn,0);
    memcpy(startReadRecord.MAC.value,tsa.add,6);
    startReadRecord.cType = COL_TM_STORE;
    startReadRecord.cStart = storetime+READ_DATA_TIME_ADD;
    startReadRecord.cEnd = storetime+READ_DATA_TIME_ADD;
    
    ret = db_read_record(gCalcArg.clientid ,&startReadRecord, DATA_LEN_MAX,&RecordOut);
    if((ret!=0) || (RecordOut.recordnum == 0))
    {
        PDS_FMT_DEBUG("pn %d  获取电量数据失败 ret %d, recordnum %d, oadMain 0x%08x, store %04d-%02d-%02d %02d:%02d:%02d\n",pn, ret,RecordOut.recordnum,
            mainoad, timetmp.year, timetmp.month, timetmp.day,timetmp.hour, timetmp.minute, timetmp.second);

        return ret;
    }
    PDS_FMT_DEBUG("pn %d  获取电量数据成功 oadMain 0x%08x, store %04d-%02d-%02d %02d:%02d:%02d\n",pn,
         mainoad, timetmp.year, timetmp.month, timetmp.day,timetmp.hour, timetmp.minute, timetmp.second);

    data->nNum = startReadRecord.recordOAD.road.oadCols.nNum;

    for(i = 0; i < startReadRecord.recordOAD.road.oadCols.nNum; i++)
    {
        
        for(j = 0; j < RecordOut.recordnum; j++)
        {
            if(RecordOut.record[j].road.oadCols.oad[0].value != startReadRecord.recordOAD.road.oadCols.oad[i].value)
            {
                continue;
            }

            //总及费率电能量只取总电能量
            memcpy(&energys, RecordOut.record[j].outDataBuf, sizeof(OOP_ENERGY_T));
            memcpy(&data[i],&energys,sizeof(OOP_ENERGY_T));
            break;
        }

        if(j == RecordOut.recordnum)
        {
            PDS_FMT_DEBUG("pn %d all&phase energys get failed. oad(0x%08x)\n", pn, startReadRecord.recordOAD.road.oadCols.oad[i].value);
        }

    }

    return 0;
}




//获取电表数据，用例里面数据的存储时表不一致，所以考虑用成功时表来筛选数据
void GetMeterData()
{
	OOP_OAD_U mainoad;
	uint32 curtime;
	OOP_DATETIME_S_T timetmp;
	uint32           cStart;      //时间起始点
	//uint32           cEnd;
    uint32 storetime[2];
    int ret;
	uint8 i;
	//RECORD_UNIT_T RecordOut;
	//READ_RECORD_T startReadRecord;
	//OOP_TSA_T tsa;
	OOP_ENERGY_T energys[2];
	
	curtime =time(NULL);
	DT_Time2DateTimeShort(curtime, &timetmp);
		
	if(gLineLossPeriod.ti.unit ==3)
	{
		timetmp.hour =0x00;
		timetmp.minute =0x00;
		timetmp.second =0x00;
		
		mainoad.value =0x50040200;

		DT_DateTimeShort2Time(&timetmp,(time_t*)&cStart);
        storetime[1] =storetime[0]  = cStart;
        //storetime 0上一个存储时标 1上上个存储时标
        DT_TimeSubInv((time_t*)&storetime[1],gLineLossPeriod.ti);

		PDS_FMT_DEBUG("storetime[0] is %d, storetime[1] is %d\n",storetime[0] ,storetime[1]);
	
	}
	else if(gLineLossPeriod.ti.unit ==1)
	{
		mainoad.value =0x50020200;
        timetmp.second = 0;
        timetmp.minute = 15 * (gCurBinTime.min/15);

        DT_DateTimeShort2Time(&timetmp,(time_t*)&cStart);
        //DT_TimeSubInv((time_t*)&cStart,gLineLossPeriod.ti);
        storetime[1] =storetime[0]  = cStart;
        //storetime 0上一个存储时标 1上上个存储时标
        DT_TimeSubInv((time_t*)&storetime[1],gLineLossPeriod.ti);

		PDS_FMT_DEBUG("storetime[0] is %d, storetime[1] is %d\n",storetime[0] ,storetime[1]);

		
	}
	else if(gLineLossPeriod.ti.unit ==4)
	{
		timetmp.day =0x01;
		timetmp.hour =0x00;
		timetmp.minute =0x00;
		timetmp.second =0x00;
		
		mainoad.value =0x50060200;

		DT_DateTimeShort2Time(&timetmp,(time_t*)&cStart);
        storetime[1] =storetime[0]  = cStart;
        //storetime 0上一个存储时标 1上上个存储时标
        DT_TimeSubInv((time_t*)&storetime[1],gLineLossPeriod.ti);

		PDS_FMT_DEBUG("storetime[0] is %d, storetime[1] is %d\n",storetime[0] ,storetime[1]);

	}
    for(i=0;i<LineMeter.MeterCount;i++)
    {
        memset(energys,0x00,sizeof(energys));
        ret =  GetMeterEnergyDataNew(LineMeter.MeterFile[i].Pn,mainoad,storetime[0],energys);
        if(ret ==0 )
        {   
            LineMeter.MeterFile[i].curdatavalid = 1;
            LineMeter.MeterFile[i].curpositiveenergy = energys[0].nValue[0];
            LineMeter.MeterFile[i].curnegetiveenergy = energys[1].nValue[0];
        }else
        {
            LineMeter.MeterFile[i].curdatavalid = 0;
            LineMeter.MeterFile[i].curpositiveenergy = 0;
            LineMeter.MeterFile[i].curnegetiveenergy = 0;
        }
        memset(energys,0x00,sizeof(energys));
        ret = GetMeterEnergyDataNew(LineMeter.MeterFile[i].Pn,mainoad,storetime[1],energys);
        if(ret == 0)
        {
            LineMeter.MeterFile[i].lastdatavalid = 1;
            LineMeter.MeterFile[i].lastpositiveenergy = energys[0].nValue[0];
            LineMeter.MeterFile[i].lastnegetiveenergy = energys[1].nValue[0];
        }else
        {
            LineMeter.MeterFile[i].lastdatavalid = 0;
            LineMeter.MeterFile[i].lastpositiveenergy = 0;
            LineMeter.MeterFile[i].lastnegetiveenergy = 0;
        }
        
        PDS_FMT_DEBUG("i %d pn %d 当前valid %d 正向电量 %d 反向 %d  上次valid %d 正 %d 反 %d\n",i,LineMeter.MeterFile[i].Pn,LineMeter.MeterFile[i].curdatavalid,LineMeter.MeterFile[i].curpositiveenergy,LineMeter.MeterFile[i].curnegetiveenergy,
            LineMeter.MeterFile[i].lastdatavalid,LineMeter.MeterFile[i].lastpositiveenergy ,LineMeter.MeterFile[i].lastnegetiveenergy);

    }

    return;
    
//	LineMeter.savetime++;
//	for(k=0;k<LineMeter.MeterCount;k++)
//	{
//		memset((uint8*)&startReadRecord,0x00,sizeof(READ_RECORD_T));
//		memset((uint8*)&RecordOut,0x00,sizeof(RECORD_UNIT_T));
//		startReadRecord.recordOAD.road.oadMain.value = mainoad.value;
//		startReadRecord.recordOAD.road.oadCols.nNum = 2;
//		startReadRecord.recordOAD.road.oadCols.oad[0].value =0x00100200;
//		startReadRecord.recordOAD.road.oadCols.oad[1].value =0x00200200;
//
//		startReadRecord.recordOAD.logicId = 0;
//		startReadRecord.recordOAD.infoNum = LineMeter.MeterFile[i].Pn;
//		startReadRecord.recordOAD.classtag =5;
//		startReadRecord.MAC.nNum=6;
//		tsa=taskmng_tsa_get(LineMeter.MeterFile[i].Pn,0);
//		memcpy(startReadRecord.MAC.value,tsa.add,6);
//		startReadRecord.cType = COL_TM_END;
//		startReadRecord.cStart = cStart+READ_DATA_TIME_ADD;
//		startReadRecord.cEnd = cEnd+READ_DATA_TIME_ADD;
//		
//		ret = db_read_record(gCalcArg.clientid ,&startReadRecord, DATA_LEN_MAX,&RecordOut);
//		if((ret!=0) || (RecordOut.recordnum == 0))
//		{
//			PDS_FMT_DEBUG("pn %d 获取电量数据失败 ret %d, recordnum %d, oadMain 0x%08x, store %04d-%02d-%02d %02d:%02d:%02d\n",LineMeter.MeterFile[i].Pn, ret,RecordOut.recordnum,
//				mainoad, timetmp.year, timetmp.month, timetmp.day,timetmp.hour, timetmp.minute, timetmp.second);
//	
//			return ;
//		}
//	
//		
//		for(i = 0; i < startReadRecord.recordOAD.road.oadCols.nNum; i++)
//		{
//			
//			for(j = 0; j < RecordOut.recordnum; j++)
//			{
//				if(RecordOut.record[j].road.oadCols.oad[0].value != startReadRecord.recordOAD.road.oadCols.oad[i].value)
//				{
//					continue;
//				}
//	
//				//总及费率电能量只取总电能量
//
//				if(startReadRecord.recordOAD.road.oadCols.oad[i].value ==0x00100200)
//				{
//
//					if((LineMeter.savetime %2)==1)
//					{
//
//						memcpy(&energys, RecordOut.record[j].outDataBuf, sizeof(OOP_ENERGY_T));
//						LineMeter.MeterFile[i].lastpositiveenergy = energys.nValue[0];
//						PDS_FMT_DEBUG("奇数次正向电量LineMeter.MeterFile[i].lastpositiveenergy is %d, i is %d\n",LineMeter.MeterFile[i].lastpositiveenergy,i);
//					}
//					else
//					{
//						memcpy(&energys, RecordOut.record[j].outDataBuf, sizeof(OOP_ENERGY_T));
//						LineMeter.MeterFile[i].curpositiveenergy = energys.nValue[0];
//						
//						PDS_FMT_DEBUG("偶数次正向电量LineMeter.MeterFile[i].curpositiveenergy is %d, i is %d\n",LineMeter.MeterFile[i].curpositiveenergy,i);
//
//					}
//
//				}
//				else if(startReadRecord.recordOAD.road.oadCols.oad[i].value ==0x00200200)
//				{
//					if((LineMeter.savetime %2)==1)
//					{
//
//						memcpy(&energys, RecordOut.record[j].outDataBuf, sizeof(OOP_ENERGY_T));
//						LineMeter.MeterFile[i].lastnegetiveenergy = energys.nValue[0];
//						
//						PDS_FMT_DEBUG("奇数次反向电量LineMeter.MeterFile[i].lastnegetiveenergy is %d, i is %d\n",LineMeter.MeterFile[i].lastnegetiveenergy,i);
//					}
//					else
//					{
//						memcpy(&energys, RecordOut.record[j].outDataBuf, sizeof(OOP_ENERGY_T));
//						LineMeter.MeterFile[i].curnegetiveenergy = energys.nValue[0];
//						
//						PDS_FMT_DEBUG("偶数次反向电量LineMeter.MeterFile[i].curnegetiveenergy is %d, i is %d\n",LineMeter.MeterFile[i].curnegetiveenergy,i);
//
//					}
//				}
//			}
//	
//
//	
//		}
//	}
}
//计算线损数据：是直接获取上两次数据还是,还是每次都获取一次存起来
void CalLinessLoss()
{
	uint16 i=0;
	uint32  curtotalpositivevalue=0;
	uint32  curtotalnegetivevalue=0;
	uint32  lasttotalpositivevalue=0;
	uint32  lasttotalnegetivevalue=0;
	int32  diffenergy=0;
	int32  masterdiffenergy=0;
	OOP_LNLS_STATISVAL_T lnslstatsva;
	WRITE_RECORD_T inRecord;
		OOP_OAD_U mainoad;
	uint32 curtime;
	OOP_DATETIME_S_T timetmp;
	uint32           cStart;      //时间起始点
	uint32 storeNo = 0;
	int ret  = 0;

	memset(&lnslstatsva,0x00,sizeof(OOP_LNLS_STATISVAL_T));
    memset(&inRecord,0x00,sizeof(READ_RECORD_T));

//	if(LineMeter.savetime<2)
//	{
//		PDS_FMT_DEBUG("冻结数据不够两次LineMeter.savetime %d\n", LineMeter.savetime);
//		return;
//	}
    if(LineMeter.MeterFile[0].curdatavalid !=1 || LineMeter.MeterFile[0].lastdatavalid !=1)
        return;
    for(i=1;i<LineMeter.MeterCount;i++)
    {
        PDS_FMT_DEBUG("%d pn %d 当前正 %d 反 %d 上一次 正 %d 反 %d \n",i,LineMeter.MeterFile[i].Pn,LineMeter.MeterFile[i].curpositiveenergy,
            LineMeter.MeterFile[i].curnegetiveenergy,LineMeter.MeterFile[i].lastpositiveenergy,LineMeter.MeterFile[i].lastnegetiveenergy);
        curtotalpositivevalue += LineMeter.MeterFile[i].curpositiveenergy;
        curtotalnegetivevalue += LineMeter.MeterFile[i].curnegetiveenergy;
        lasttotalpositivevalue += LineMeter.MeterFile[i].lastpositiveenergy;
        lasttotalnegetivevalue += LineMeter.MeterFile[i].lastnegetiveenergy;

    }
	PDS_FMT_DEBUG("户表所有表的电量当前 正 %d 反 %d 上一次 正 %d 反 %d \n",curtotalpositivevalue,curtotalnegetivevalue,lasttotalpositivevalue,
        lasttotalnegetivevalue);


//
//
//	for(i=1;i<LineMeter.MeterCount;i++)
//	{
//		if(LineMeter.savetime>=2)
//		{
//			if((LineMeter.savetime%2)==0)
//			{
//				curtotalpositivevalue=curtotalpositivevalue+LineMeter.MeterFile[i].curpositiveenergy;
//				PDS_FMT_DEBUG("本次正向 curtotalpositivevalue %d,LineMeter.MeterFile[i].curpositiveenergy is %d ,信息点 is%d\n",curtotalpositivevalue, LineMeter.MeterFile[i].curpositiveenergy,LineMeter.MeterFile[i].Pn);
//				curtotalnegetivevalue =curtotalnegetivevalue+LineMeter.MeterFile[i].curnegetiveenergy;
//				PDS_FMT_DEBUG("上次反向 curtotalnegetivevalue %d,LineMeter.MeterFile[i].curnegetiveenergy is %d ,信息点 is%d\n",curtotalpositivevalue, LineMeter.MeterFile[i].curnegetiveenergy,LineMeter.MeterFile[i].Pn);
//			
//				lasttotalpositivevalue =lasttotalpositivevalue+LineMeter.MeterFile[i].lastpositiveenergy;
//				
//				PDS_FMT_DEBUG("上次正向 lasttotalpositivevalue %d,LineMeter.MeterFile[i].curpositiveenergy is %d ,信息点 is%d\n",lasttotalpositivevalue, LineMeter.MeterFile[i].lastpositiveenergy,LineMeter.MeterFile[i].Pn);
//				lasttotalnegetivevalue = lasttotalnegetivevalue+LineMeter.MeterFile[i].lastnegetiveenergy;
//				PDS_FMT_DEBUG("上次反向 lasttotalpositivevalue %d,LineMeter.MeterFile[i].lastnegetiveenergy is %d ,信息点 is%d\n",lasttotalpositivevalue, LineMeter.MeterFile[i].lastnegetiveenergy,LineMeter.MeterFile[i].Pn);
//			
//
//			}
//			else
//			{
//				curtotalpositivevalue=curtotalpositivevalue+LineMeter.MeterFile[i].lastpositiveenergy;
//				PDS_FMT_DEBUG("本次正向 curtotalpositivevalue %d,LineMeter.MeterFile[i].curpositiveenergy is %d ,信息点 is%d\n",curtotalpositivevalue, LineMeter.MeterFile[i].curpositiveenergy,LineMeter.MeterFile[i].Pn);
//				
//				curtotalnegetivevalue =curtotalnegetivevalue+LineMeter.MeterFile[i].lastpositiveenergy;
//				PDS_FMT_DEBUG("上次反向 curtotalnegetivevalue %d,LineMeter.MeterFile[i].curnegetiveenergy is %d ,信息点 is%d\n",curtotalpositivevalue, LineMeter.MeterFile[i].curnegetiveenergy,LineMeter.MeterFile[i].Pn);
//			
//				lasttotalpositivevalue =lasttotalpositivevalue+LineMeter.MeterFile[i].curpositiveenergy;
//				PDS_FMT_DEBUG("上次正向 lasttotalpositivevalue %d,LineMeter.MeterFile[i].curpositiveenergy is %d ,信息点 is%d\n",lasttotalpositivevalue, LineMeter.MeterFile[i].lastpositiveenergy,LineMeter.MeterFile[i].Pn);
//			
//				lasttotalnegetivevalue = lasttotalnegetivevalue+LineMeter.MeterFile[i].curpositiveenergy;
//				PDS_FMT_DEBUG("上次反向 lasttotalpositivevalue %d,LineMeter.MeterFile[i].lastnegetiveenergy is %d ,信息点 is%d\n",lasttotalpositivevalue, LineMeter.MeterFile[i].lastnegetiveenergy,LineMeter.MeterFile[i].Pn);
//
//			}
//		}
//
//	}
	
	diffenergy =abs(curtotalpositivevalue-curtotalnegetivevalue)-abs(lasttotalpositivevalue-lasttotalnegetivevalue);
	
	PDS_FMT_DEBUG("户表所有表的电量差值 diffenergy %d\n",diffenergy);
//	if(LineMeter.savetime>=2)
//	{
//		if((LineMeter.savetime%2)==0)
//		{
//			masterdiffenergy = abs(LineMeter.MeterFile[0].curpositiveenergy-LineMeter.MeterFile[0].curnegetiveenergy)-abs(LineMeter.MeterFile[0].lastpositiveenergy-LineMeter.MeterFile[0].lastnegetiveenergy);
//			PDS_FMT_DEBUG("总表的电量差值11 diffenergy %d\n",masterdiffenergy);
//		}
//		else
//		{
//			masterdiffenergy = abs(LineMeter.MeterFile[0].lastpositiveenergy-LineMeter.MeterFile[0].lastnegetiveenergy)-abs(LineMeter.MeterFile[0].curpositiveenergy-LineMeter.MeterFile[0].curnegetiveenergy);
//			PDS_FMT_DEBUG("总表电量差值22 diffenergy %d\n",masterdiffenergy);
//
//		}
//	}
    
    masterdiffenergy = abs(LineMeter.MeterFile[0].curpositiveenergy-LineMeter.MeterFile[0].curnegetiveenergy)-abs(LineMeter.MeterFile[0].lastpositiveenergy-LineMeter.MeterFile[0].lastnegetiveenergy);
	PDS_FMT_DEBUG("总表的电量差值 diffenergy %d\n",masterdiffenergy);
    
	curtime =time(NULL);
	DT_Time2DateTimeShort(curtime, &timetmp);
	if(gLineLossPeriod.ti.unit ==3)
	{
		timetmp.hour =0x00;
		timetmp.minute =0x00;
		timetmp.second =0x00;
		
		mainoad.value =0x50040200;
		DT_DateTimeShort2Time(&timetmp,(time_t*)&cStart);

		
		//PDS_FMT_DEBUG("线损存储时间 cStart %d,mainoad.value is %x\n",cStart,mainoad.value);
	}
	else if(gLineLossPeriod.ti.unit ==1)
	{	
		mainoad.value =0x50020200;	
		timetmp.second = 0;
        timetmp.minute = 15 * (gCurBinTime.min/15);

        DT_DateTimeShort2Time(&timetmp,(time_t*)&cStart);
        //DT_TimeSubInv((time_t*)&cStart,gLineLossPeriod.ti);
		//PDS_FMT_DEBUG("线损存储时间 cStart %d,mainoad.value is %x\n",cStart,mainoad.value);
	}
	else if(gLineLossPeriod.ti.unit ==4)
	{
		timetmp.day =0x01;
		timetmp.hour =0x00;
		timetmp.minute =0x00;
		timetmp.second =0x00;
		
		mainoad.value =0x50060200;
		DT_DateTimeShort2Time(&timetmp,(time_t*)&cStart);
		
		//PDS_FMT_DEBUG("线损存储时间 cStart %d,mainoad.value is %x\n",cStart,mainoad.value);

	}

    DT_Time2DateTimeShort(cStart, &timetmp);

    PDS_FMT_DEBUG("线损存储时间 %d/%d/%d %d:%d:%d ,mainoad is %x\n",timetmp.year,timetmp.month,timetmp.day,timetmp.hour,
        timetmp.minute,timetmp.second,mainoad.value);
	lnslstatsva.nNum =0x01;
	lnslstatsva.rsv=0x01;
	lnslstatsva.nVal[0].eneSup = masterdiffenergy;
	lnslstatsva.nVal[0].eneSal = diffenergy;
    //线损率单位% 换算-2 所以要乘以10000
    if(masterdiffenergy!=0)
	    lnslstatsva.nVal[0].nRatio = ((masterdiffenergy-diffenergy)*10000)/masterdiffenergy;
    if(masterdiffenergy > diffenergy)
        lnslstatsva.nVal[0].abnorFlag =2;
    else if(masterdiffenergy < diffenergy)
        lnslstatsva.nVal[0].abnorFlag =1;
    else
	    lnslstatsva.nVal[0].abnorFlag =0;
	PDS_FMT_DEBUG("lnslstatsva.nVal[0].nRatio is %d\n",lnslstatsva.nVal[0].nRatio);
	inRecord.recordOAD.classtag = CLASS_DATA_COLLEC;
    inRecord.recordOAD.infoNum = LineMeter.MeterFile[0].Pn;
    inRecord.recordOAD.road.oadMain.value = mainoad.value;
    inRecord.recordOAD.road.oadCols.nNum = 1;
    inRecord.recordOAD.road.oadCols.oad[0].value = 0x60510200;
    inRecord.MAC.nNum = 0;
    //memcpy_r(inRecord.MAC.value, addr, 6);
    inRecord.colStartTm = time(NULL)+READ_DATA_TIME_ADD;
    inRecord.colEndTm = time(NULL)+READ_DATA_TIME_ADD;
    inRecord.colStoreTm = cStart+READ_DATA_TIME_ADD; 
    inRecord.inDatalen = sizeof(OOP_LNLS_STATISVAL_T);
    memcpy(inRecord.inDataBuf, &lnslstatsva, inRecord.inDatalen);
    
    ret = db_write_record(gCalcArg.clientid, &inRecord, &storeNo);
    if(ret!=0)
    {
        PDS_FMT_DEBUG("line loss data write failed, Pn %d ret %d\n", 0, ret);
    }      
}
/*******************************************************************************
* 函数名称: CheckLineImpedanceCalc
* 函数功能:过分检查是否到了线路阻抗计算时间
* 输入参数:
* 输出参数: 
* 返 回 值:  
*******************************************************************************/
BOOL CheckLineImpedanceCalc()
{
    Line_Impedance_Calc_T *gLineImpedanceArg = &gCalcArg.lineImpedanceArg;
    DateTimeHex_t  timetmp;
    if(gLineImpedanceArg->isNeedCalc==1)
    {
        return FALSE;
    }
    if((gCurBinTime.min)%15 == 0)
    {
        gLineImpedanceArg->delay = 300;
        gLineImpedanceArg->isNeedCalc = 1;
        memcpy(&timetmp,&gCurBinTime,sizeof(DateTimeHex_t));
        timetmp.sec = 0;
        DT_DateTimeHex2Time(&timetmp,&gLineImpedanceArg->storeTime[0]);
        gLineImpedanceArg->storeTime[0]=gLineImpedanceArg->storeTime[0]-15*60;
        gLineImpedanceArg->storeTime[1]=gLineImpedanceArg->storeTime[0]-15*60;
        return TRUE;
    }
    return FALSE;
}
/*******************************************************************************
* 函数名称: CalcTaskCheck
* 函数功能:calc线程检查是否有指定任务
* 输入参数:
* 输出参数: 
* 返 回 值:  
*******************************************************************************/
void CalcTaskCheckNew()
{
	BOOL state=FALSE;
	if(gPastTimeFlag.BitSect.Minute == TRUE)
	{
		GetTaskRunInfo();
		
		state=JudgeTaskRunState(gCalcArg.clientid);

		if(state==TRUE)
		{
			GetMeterData();

			CalLinessLoss();
		}
	}
}

/*******************************************************************************
* 函数名称: CalcTaskCheck
* 函数功能:calc线程检查是否有指定任务
* 输入参数:
* 输出参数: 
* 返 回 值:  
*******************************************************************************/
void CalcTaskCheck()
{
    Line_Loss_Calc_T *gLineLossArg = &gCalcArg.lineLossArg;
    #ifdef AREA_HUNAN
    Line_Impedance_Calc_T *gLineImpedanceArg = &gCalcArg.lineImpedanceArg;
    #endif
    OOP_DATETIME_S_T timetmp;
    if(gPastTimeFlag.BitSect.Minute == TRUE)
    {   
        if(CheckLineLossCalc())
        {
            DT_Time2DateTimeShort(gLineLossArg->lastStoreTime[0], &timetmp);
            PDS_FMT_DEBUG("Line loss calc start %ds later, oad 0x%08x, store at: %04d-%02d-%02d %02d:%02d:%02d\n", gLineLossArg->delay, gLineLossArg->mainOad,timetmp.year, timetmp.month, timetmp.day,
                timetmp.hour, timetmp.minute, timetmp.second);
            gLineLossArg->isNeedCalc = 1;
        }
        #ifdef AREA_HUNAN
        //默认15分钟计算一次阻抗值 延时5分钟
        if(CheckLineImpedanceCalc())
        {
            DT_Time2DateTimeShort(gLineImpedanceArg->storeTime[0], &timetmp);
            PDS_FMT_DEBUG("Line impedance calc start %ds later, store at: %04d-%02d-%02d %02d:%02d:%02d\n", gLineImpedanceArg->delay, timetmp.year, timetmp.month, timetmp.day,
                timetmp.hour, timetmp.minute, timetmp.second);
        }
        #endif
    }
    if(gLineLossArg->isNeedCalc ==1 && DelayDeal(&gLineLossArg->delay)==0)
    {
        LineLossCalc();
    }
    #ifdef AREA_HUNAN
    if(gLineImpedanceArg->isNeedCalc ==1 && DelayDeal(&gLineImpedanceArg->delay)==0)
    {
        LineImpedanceCalc();
    }
    #endif
}
//
//
///*******************************************************************************
//* 函数名称: CalcTaskCheck
//* 函数功能:calc线程检查是否有指定任务
//* 输入参数:
//* 输出参数: 
//* 返 回 值:  
//*******************************************************************************/
//void CalcLineloss()
//{
//    READ_RECORD_T      inReadRecord;
//    uint32             handle = 0;
//    uint32             recNum = 0;
//    STEP_READ_RECORD_T stepReadRecord;
//    RECORD_UNIT_T      stepRecordOut;
//
//    time_t              timep       = 0;
//    uint32 bufLen;
//    uint32 totaladd=0;
//    int ret=0;
//    uint16 i=0;
//    int rate;
//    uint6 j=0;
//    memset(&inReadRecord, 0, sizeof(READ_RECORD_T));
//    memset(&stepReadRecord, 0, sizeof(STEP_READ_RECORD_T));
//    memset(&stepRecordOut, 0, sizeof(RECORD_UNIT_T));
//
//    inReadRecord.recordOAD.logicId = 0;
//
//    inReadRecord.cType = COL_TM_STORE;
//    inReadRecord.cStart = time(NULL)-172800;
//    inReadRecord.cEnd = time(NULL);
//    inReadRecord.sortType = UP_SORT;
//    inReadRecord.recordOAD.optional = 0;
//    inReadRecord.recordOAD.road.oadMain.value = 0x50040200;
//    inReadRecord.recordOAD.road.oadCols.nNum = 4;
//    inReadRecord.recordOAD.road.oadCols.oad[0].value =0x00100200;
//    inReadRecord.recordOAD.road.oadCols.oad[1].value =0x00110200;
//    inReadRecord.recordOAD.road.oadCols.oad[2].value =0x00120200;
//    inReadRecord.recordOAD.road.oadCols.oad[3].value =0x00130200;
//    
//    for(i=0;i<MeterFileAll.MeterCount;i++)
//    {
//        inReadRecord.recordOAD.infoNum = MeterFileAll.MeterFile[i].nIndex.;
//
//        ret = db_read_record_start(lineloss, &inReadRecord, &handle, &recNum);
//        if((ret != 0)||(recNum !=2))
//        {
//            db_read_record_end(lineloss, handle);
//            return;
//        }
//        stepReadRecord.MAC.nNum = 6;
//        memcpy(stepReadRecord.MAC.value,MeterFileAll.MeterFile[i].basic.tsa.add,6);
//        for(j=0;j<recNum;j++)
//        {
//            ret = db_read_record_step(lineloss, handle, &stepReadRecord, &stepRecordOut);
//            if (ret != ERR_OK)
//            {
//                db_read_record_end(lineloss, handle);
//                return;
//            }
//            if(j==0)
//            {
//                memcpy((uint8*)&linelossvalue.lastenerger[i],stepRecordOut.record[0].outDataBuf, stepRecordOut.record[0].outDatalen);
//            }
//            else
//            {
//                memcpy((uint8*)&linelossvalue.curenerger[i],stepRecordOut.record[0].outDataBuf, stepRecordOut.record[0].outDatalen);
//            }
//
//        }
//        linelossvalue.addenergry[i] = linelossvalue.curenerger[i]-linelossvalue.lastenerger[i];
//    }
//    for(i=1;i<MeterFileAll.MeterCount;i++)
//    {
//        totaladd=linelossvalue.addenergry[i]+totaladd;
//    }
//    rate = (linelossvalue.addenergry[0]-totaladd)/linelossvalue.addenergry[0];
//}


void pds_ac_ptct_get(uint32 *pt, uint32 *ct)
{
    NOMAL_OAD_T nomalOAD = {0};
    int ret = 0;
    uint32 len = 0;
    nomalOAD.logicId = 0;
    nomalOAD.oad.value = 0x401c0200;
    ret =  db_read_nomal(gCalcArg.clientid , &nomalOAD, sizeof(uint32), (uint8*)ct, &len);
    if(ret != 0)
    {
        *ct = 1;
        PDS_FMT_DEBUG("ret %d, acdata use default ct=1\n",ret);
    }
    nomalOAD.oad.value = 0x401d0200;
    ret =  db_read_nomal(gCalcArg.clientid , &nomalOAD, sizeof(uint32), (uint8*)pt, &len);
    if(ret != 0)
    {
        *pt = 1;
        PDS_FMT_DEBUG("ret %d, acdata use default pt=1\n",ret);
    }
    return;
}

/*******************************************************************************
* 函数名称: transReadQueue
* 函数功能: 读队列
* 输入参数：pLockQueue          锁缓冲区结构指针
*           buf                 读数据缓冲区，将队列中的数据取出放入该缓冲区
* 输出参数：plockqueue          队列中的数据
* 内部处理: 判队列中是否有数据，有则从队列中取出数据放入buf中，每执行一次，取一个数据
* 返 回 值: len                 读到的数据长度
*******************************************************************************/
int32 TransReadQueue(LockQueue_T *pLockQueue, uint8 *buf)
{
    int32 len = 0;
    uint8 *pdata_buf;

    //锁缓冲区
    pthread_mutex_lock(&pLockQueue->QueueLock);  
  
    //若队列中有数据，则取到buf中，解开锁
    if ((pLockQueue->QueueHead != pLockQueue->QueueTail)
        || ((pLockQueue->QueueHead == pLockQueue->QueueTail)
            && (pLockQueue->Queue[pLockQueue->QueueHead].QueueLen != 0)))
    {   
        pdata_buf = pLockQueue->Queue[pLockQueue->QueueHead].QueueBuf;  
        len = pLockQueue->Queue[pLockQueue->QueueHead].QueueLen;
        
        //取队列中的数据
        memcpy(buf, pdata_buf, len); 

        //清数据
        memset(pdata_buf, 0, len);   
        pLockQueue->Queue[pLockQueue->QueueHead].QueueLen = 0;

        //队列头向后移动一位
        pLockQueue->QueueHead= (pLockQueue->QueueHead + 1) % (MAX_QUEUE_CNT); 
    }
        
    pthread_mutex_unlock(&pLockQueue->QueueLock);
    
    return len;
}

/*******************************************************************************
* 函数名称: transWriteQueue
* 函数功能: 写队列
* 输入参数：plockqueue          锁缓冲区结构指针
            buf                 需要写入队列的数据
            buf_len             数据长度
* 输出参数：无
* 内部处理: 若队列不满，将buf中的数据放入队列中；若队列满，丢弃该数据
* 返 回 值: >0                  成功
            =0                  失败
*******************************************************************************/
int32 TransWriteQueue(LockQueue_T *pLockQueue, uint8 *buf, int32 buf_len)
{
    int32 flags = 0;
        
    //锁缓冲区
    pthread_mutex_lock(&pLockQueue->QueueLock);  
    //判队列中是否还有空的位置
    if (pLockQueue->Queue[pLockQueue->QueueTail].QueueLen == 0)
    {
        pLockQueue->Queue[pLockQueue->QueueTail].QueueLen = buf_len;    
        memcpy(pLockQueue->Queue[pLockQueue->QueueTail].QueueBuf, buf, buf_len);
        flags = 1;
        pLockQueue->QueueTail = (pLockQueue->QueueTail + 1) % (MAX_QUEUE_CNT);
    }   
    
    //解缓冲区
    pthread_mutex_unlock(&pLockQueue->QueueLock); 
    
    return flags;
}

#if DESC("拓扑识别代码",1)

void topology_swap(void *a, int id)
{
    Topology_Arg_T *gTopologyArg = &gCalcArg.topologyArg;
    Break_Son_T tmpNode;  
    uint16 breakID1 = ((Break_Son_T*)((Break_Son_T*)a+id))->breakID;
    uint16 breakID2 = ((Break_Son_T*)((Break_Son_T*)a+id+1))->breakID;
    Break_Info_T *pBreak1 = &gTopologyArg->breakInfo[breakID1];
    Break_Info_T *pBreak2 = &gTopologyArg->breakInfo[breakID2];

    //升序
    if(pBreak1->SonNum > pBreak2->SonNum)
    {
        memcpy(&tmpNode, (Break_Son_T*)a+id, sizeof(Break_Son_T));
        memcpy((Break_Son_T*)a+id, (Break_Son_T*)a+id+1, sizeof(Break_Son_T));
        memcpy((Break_Son_T*)a+id+1, &tmpNode, sizeof(Break_Son_T));
    }
    
    return;
}

int pds_topodata_clear()
{
    CLEAR_DATA_T clearData; 
    MEMZERO(&clearData, sizeof(clearData));

    /*清空所有拓扑信息*/
    clearData.recordOAD.logicId = 0x00;
    clearData.recordOAD.optional = 0;
    clearData.recordOAD.road.oadMain.value = 0x00;
    clearData.recordOAD.road.oadCols.nNum = 1;
    clearData.recordOAD.road.oadCols.oad[0].value = 0x60F10200;
    clearData.bClassTag = FALSE;
	clearData.recordOAD.classtag = CLASS_DATA_NORMAL;
    clearData.recordOAD.infoNum = 0xffff;
    return db_clear_oad_data(gCalcArg.clientid, &clearData);
}

/**********************************************************************
* @name      : SaveTopoInfoFile
* @brief     ：将拓扑识别结果存储到私有文件内
* @param[in]  topoArg 拓扑识别结果

* @param[out]：
* @return    ：

**********************************************************************/
void SaveTopoInfoFile(Topology_Arg_T *topoArg)
{
    int ret = ERR_OK;
    char file[200]={0};
    sprintf(file,"%s/TopoInfoFile",PrivateDir);
    ret = write_pdata_xram(file,(char*)topoArg,0,sizeof(Topology_Arg_T));
    if(ret < 0)
    {
        PDS_FMT_DEBUG("SaveTopoInfoFile 失败 path=%s, ret=%d \n", file, ret);
    }
}

/**********************************************************************
* @name      : LoadTopoInfoFile
* @brief     ：上电加载拓扑识别结果
* @param[in]  
* @param[out]：topoArg 拓扑识别结果

* @return    ：

**********************************************************************/
void LoadTopoInfoFile(Topology_Arg_T *topoArg)
{
    Topology_Arg_T topoinfo;
    char file[200]={0};
    sprintf(file,"%s/TopoInfoFile",PrivateDir);
    if(access(file,R_OK)==0)
    {
        memset(&topoinfo,0x00,sizeof(Topology_Arg_T));
        read_pdata_xram(file, (char*)&topoinfo,0, sizeof(Topology_Arg_T));
        memcpy(topoArg,&topoinfo,sizeof(Topology_Arg_T));
        PDS_FMT_DEBUG("LoadTopoInfoFile 成功 \n");
    }
}

/*******************************************************************************
* 函数名称: GetPhase
* 函数功能: 获取的拓扑信息中发生相位（ BIT0:A BIT1:B BIT2:C,置位代表发生） 转换为698相位信息
* 输入参数: in 获取的拓扑信息发生相位

* 输出参数: 
* 返 回 值:相位信息 未知（0），A（1），B（2），C（3），三相（FF） 
*******************************************************************************/
uint8 GetPhase(uint8 in)
{
    uint8 phase = 0;
    if(in==1)
    {
        phase = 1;
    }else if(in == 2)
    {
        phase = 2;
    }else if(in == 4)
    {
        phase = 3;
    }else if(in == 7)
    {
        phase = 0xFF;
    }
    return phase;
}

/*******************************************************************************
* 函数名称: GetBreakSon
* 函数功能: 解析获取的拓扑信息中的记录，找到断路器父亲节点
* 输入参数: recordtime 记录发生时间
         phase 相位
* 输出参数: 
* 返 回 值:断路器索引
*******************************************************************************/
void GetBreakSon(Topology_Arg_T *gTopologyArg, uint32 recordtime,uint8 phase)
{
    uint16 breakindex = gTopologyArg->breakIndex;
    uint32 starttime;
    uint8 i;
    uint8 parentCnt = 0;
    uint8 suncount = 0;
    int interval = 0;

    //遍历所有断路器，找到发送时间与之匹配的断路器，即收到的为该断路器发出的信号，当前收到信息的断路器为亲代
    for(i=0;i<gTopologyArg->breakCount;i++)
    {
        //获取除自己外的拓扑信息发送时间，并与收到的拓扑信息时间相比
        if(i == breakindex)
        {
            continue;
        }
        
        starttime = gTopologyArg->breakInfo[i].starttime;
        interval = recordtime-starttime;
        
        PDS_FMT_DEBUG("%012llx recordtime:%d - starttime:%d = %d, Limit(0,%d) \n", *(uint64*)gTopologyArg->breakInfo[i].Addr, recordtime, starttime, interval, TOPO_INTERVAL);

        //时间差小于阈值，且不是自己，则放入亲子列表
        if(abs(interval) > 0 && abs(interval) < TOPO_INTERVAL)
        {
            //亲代角度存储，方便拓扑网络计算
            parentCnt = gTopologyArg->breakInfo[i].ParentNum;
            gTopologyArg->breakInfo[i].ParentInfo[parentCnt].breakID = breakindex;
            gTopologyArg->breakInfo[i].ParentInfo[parentCnt].phase = phase;
            memcpy(gTopologyArg->breakInfo[i].ParentInfo[parentCnt].addr,gTopologyArg->breakInfo[breakindex].Addr,6);
            gTopologyArg->breakInfo[i].ParentNum++;
//            PDS_BUF_DEBUG(gTopologyArg->breakInfo[i].ParentInfo[parentCnt].addr,6,"识别到亲代 来自相位 %d   \n",phase);
//            PDS_BUF_DEBUG(gTopologyArg->breakInfo[i].Addr,6,"子代地址\n");

            //子代角度存储，方便阻抗和线损计算，这地方有问题，这个相位未必就是这个子代的相位
            suncount = gTopologyArg->breakInfo[breakindex].SonNum;
            gTopologyArg->breakInfo[breakindex].breakSonInfo[suncount].phase = phase;
            memcpy(gTopologyArg->breakInfo[breakindex].breakSonInfo[suncount].addr,gTopologyArg->breakInfo[i].Addr,6);
            gTopologyArg->breakInfo[breakindex].SonNum++;

//            PDS_BUF_DEBUG(gTopologyArg->breakInfo[i].Addr,6,"识别到子代 相位 %d   \n",phase);  
//            PDS_BUF_DEBUG(gTopologyArg->breakInfo[breakindex].Addr,6,"亲代地址\n");

            PDS_FMT_DEBUG("识别到后代:%012llx-->%012llx, phase %d \n",*(uint64*)gTopologyArg->breakInfo[breakindex].Addr, *(uint64*)gTopologyArg->breakInfo[i].Addr, phase);

            break;
        }
    }

}

/*******************************************************************************
* 函数名称: UnpackTopoInfo
* 函数功能: 解析获取的拓扑信息
* 输入参数: buf 获取的拓扑信息报文数据域
         len 数据域长度
* 输出参数: 
* 返 回 值:断路器索引
*******************************************************************************/
uint8 UnpackTopoInfo(uint8 *buf,uint16 len)
{
    uint16 index = 4;
    uint8 recordnum = 0;
    uint8 i;
    uint32 DI =0;
    DateTimeBcd_t timebcd;
    time_t recordtime;
    uint8 phase;
    Topology_Arg_T *gTopologyArg = &gCalcArg.topologyArg;
    uint16 breakindex = gTopologyArg->breakIndex;
    
    if(len<5)
    {
        PDS_FMT_DEBUG(" UnpackTopoInfo error len %d \n",len);
        return 0;
    }
    memcpy(&DI,buf,4);
    if(DI!=0X08C0C001)
    {
        PDS_FMT_DEBUG(" UnpackTopoInfo error di 0X%08x !=0X08C0C001 \n",DI);
        return 0;
    }
    
    recordnum = buf[index++];

    //len = 4+1+(6+1+3+2)*recordnum
    if(len!=5+12*recordnum)
    {
        PDS_FMT_DEBUG(" UnpackTopoInfo error len %d recordnum %d \n",len,recordnum);
        return 0;
    }

    PDS_FMT_DEBUG("%012llx 本次拓扑记录数 %d \n", *(uint64*)gTopologyArg->breakInfo[breakindex].Addr, recordnum);

    gTopologyArg->breakInfo[breakindex].topoInfoNum += recordnum;
    
    for(i=0;i<recordnum ;i++)
    {
        PDS_FMT_DEBUG("%012llx 第%d条拓扑记录\n", *(uint64*)gTopologyArg->breakInfo[breakindex].Addr, i);
        timebcd.sec = buf[index++];
        timebcd.min = buf[index++];
        timebcd.hour = buf[index++];
        timebcd.day = buf[index++];
        timebcd.month = buf[index++];
        timebcd.year_h = 0x20;
        timebcd.year_l = buf[index++];

        DT_DateTimeBCD2Time(&timebcd,&recordtime);
        phase = GetPhase(buf[index++]);
        index+=5;
        GetBreakSon(gTopologyArg, recordtime+gTopologyArg->breakInfo[breakindex].timeoffset, phase);
    }

    return recordnum;
}

/*******************************************************************************
* 函数名称: UnpackTimeInfo
* 函数功能: 解析获取的时间报文
* 输入参数: buf 获取的时间信息数据域
         len 数据域长度
* 输出参数: 
* 返 回 值:断路器索引
*******************************************************************************/
void UnpackTimeInfo(uint8 *buf,uint16 len)
{
    Topology_Arg_T *gTopologyArg = &gCalcArg.topologyArg;
    uint16 breakindex = gTopologyArg->breakIndex;
    uint32 DI =0;
    uint16 index = 4;
    DateTimeBcd_t breaktmBCD;
    time_t breaktime = 0;
    uint32 commtime = (time(NULL) - g_sendchecktime)/2 ;
    int timeoffset = 0;
    
    DT_Time2DateTimeBCD(g_sendchecktime+commtime,&breaktmBCD); 
    
    if(len<7)
    {
        PDS_FMT_DEBUG(" UnpackTimeInfo error len %d \n",len);
        return;
    }
    
    memcpy(&DI,buf,4);
    if(DI!=0X04000102)
    {
        PDS_FMT_DEBUG(" UnpackTimeInfo error di 0X%08x !=0X04000102 \n",DI);
        return;
    }

    breaktmBCD.sec = buf[index++];  
    breaktmBCD.min = buf[index++]; 
    breaktmBCD.hour = buf[index++];
    
    DT_DateTimeBCD2Time(&breaktmBCD,&breaktime); 

    timeoffset = g_sendchecktime + commtime - breaktime;

    if(abs(timeoffset) > TOPO_INTERVAL)
    {
        gTopologyArg->breakInfo[breakindex].timeoffset = 0;
    }
    else
    {
        gTopologyArg->breakInfo[breakindex].timeoffset = timeoffset;
    }
    
    PDS_FMT_DEBUG("理论补偿时间 %ds 实际补偿时间 %ds\n",timeoffset, gTopologyArg->breakInfo[breakindex].timeoffset);  
}


/*******************************************************************************
* 函数名称: UnpackMeterInfo
* 函数功能: 解析获取的表档案信息报文
* 输入参数: buf 获取的表档案信息报文数据域
         len 数据域长度
* 输出参数: 
* 返 回 值:断路器索引
*******************************************************************************/
void UnpackMeterInfo(uint8 *buf,uint16 len)
{
    Topology_Arg_T *gTopologyArg = &gCalcArg.topologyArg;
    uint16 breadindex = gTopologyArg->breakIndex;
    uint32 DI =0;
    uint16 index = 4;
    uint8 meternum;
    uint8 i;
    if(len<5)
    {
        PDS_FMT_DEBUG(" UnpackMeterInfo error len %d \n",len);
        return;
    }
    memcpy(&DI,buf,4);
    if(DI!=0X04A00301)
    {
        PDS_FMT_DEBUG(" UnpackMeterInfo error di 0X%08x !=0X04A00301 \n",DI);
        return;
    }
    meternum = buf[index++];
    PDS_FMT_DEBUG("断路器 pn %d %012llx 表档案个数 %d\n",gTopologyArg->breakInfo[breadindex].Pn,*(uint64*)gTopologyArg->breakInfo[breadindex].Addr,meternum);
    if(len!=5+7*meternum)
    {
        PDS_FMT_DEBUG(" UnpackMeterInfo error len %d meternum %d \n",len,meternum);
        return;
    }
    for(i=0;i<meternum;i++)
    {
        memcpy(gTopologyArg->breakInfo[breadindex].breakSonInfo[i].addr,&buf[index],6);
        PDS_FMT_DEBUG("表档案 %012llx\n", *(uint64*)gTopologyArg->breakInfo[breadindex].breakSonInfo[i].addr);
        index+=7;
    }

    gTopologyArg->breakInfo[breadindex].isMeterSon = TRUE;
    gTopologyArg->breakInfo[breadindex].SonNum = meternum;
}

/*******************************************************************************
* 函数名称: topologySend
* 函数功能:根据当前所处状态 进行报文发送
* 输入参数: 
* 输出参数: 
* 返 回 值: 0 发送成功切换状态到接收 1 切换拓扑过程状态 2 拓扑结束
*******************************************************************************/
int topologySend()
{
    Topology_Arg_T *gTopologyArg = &gCalcArg.topologyArg;
    MeterPrtlFmt_T data;
    int ret;
    memset(&data,0x00,sizeof(MeterPrtlFmt_T));
    uint16 Sendlen;
    uint8 Sendbuf[MAX_MSG_LEN]={0};
    uint8 breakindex = gTopologyArg->breakIndex;

    //连续多个表失败超过3次也要过滤掉
    while((breakindex < gTopologyArg->breakCount)&&(gTopologyArg->breakInfo[breakindex].failcnt>=3))
    {
        PDS_FMT_DEBUG("断路器pn %d 失败次数超过3次 不再抄读 \n",gTopologyArg->breakInfo[breakindex].Pn);
        //gTopologyArg->breakInfo[breakindex].failcnt = 0;//调试时为了覆盖后面步骤，会清错误次数

        //重试后发送拓扑要先延时，保证拓扑发送的时间间隔
        if(gTopologyArg->topoState == TOPO_START)
        {
            gTopologyArg->sendRecvState = PDS_DELAY;
            gTopologyArg->delay = TOPO_DELAY_TIME;
        }
        
        if(gTopologyArg->topoState ==TOPO_TIMING )
        {
            PDS_FMT_DEBUG("广播校时未收到回码 任务结束  \n");
            return 2;
        }
        breakindex = ++gTopologyArg->breakIndex;
    }
    switch(gTopologyArg->topoState)
    {
    //广播校时
    case TOPO_TIMING:
    {
        PDS_FMT_DEBUG("拓扑识别状态机：广播校时 \n");
        rtc_t prtc;
        DT_Time2RTC((uint32)time(NULL),&prtc);
        data.CtrlCode = 0x08;
        memset(data.MtrAddr,0x99,6);
        data.DataLen = 6;
        data.DataBuff[0] = prtc.ss;
        data.DataBuff[1] = prtc.nn;
        data.DataBuff[2] = prtc.hh;
        data.DataBuff[3] = prtc.dd;
        data.DataBuff[4] = prtc.mm;
        data.DataBuff[5] = prtc.yy;
    }
        break;
    //清空所有拓扑信息0x08C0C000
    case TOPO_CLEAR:
    {
        PDS_FMT_DEBUG("拓扑识别状态机：清空拓扑 \n");
        
        if(breakindex >= gTopologyArg->breakCount)
        {
            PDS_FMT_DEBUG("所有断路器拓扑信息清空完毕 开始时间补偿 \n");
            gTopologyArg->topoState = TOPO_TIME_OFFSET;
            gTopologyArg->breakIndex =0;
            return 1;
        }  
        
        memcpy(data.MtrAddr,gTopologyArg->breakInfo[breakindex].Addr,6);
        data.CtrlCode = 0x04;
        uint8 databuf[]={0x00,0xC0,0xC0,0x08};
        memcpy(data.DataBuff,databuf,4);
        data.DataLen = 4;
        PDS_FMT_DEBUG("清空断路器 pn %d 拓扑信息 \n",gTopologyArg->breakInfo[breakindex].Pn);
    }
        break;
    //时间补偿
    case TOPO_TIME_OFFSET:
    {
        PDS_FMT_DEBUG("拓扑识别状态机：时间补偿 \n");
        
        if(breakindex >= gTopologyArg->breakCount)
        {
            PDS_FMT_DEBUG("所有断路器启动时间补偿完毕 开始启动拓扑\n");
            gTopologyArg->topoState = TOPO_START;
            gTopologyArg->breakIndex =0;
            return 1;
        }   
        
        memcpy(data.MtrAddr,gTopologyArg->breakInfo[breakindex].Addr,6);
        data.CtrlCode = 0x11;
        uint8 databuf[]={0x02,0x01,0x00,0x04};
        memcpy(data.DataBuff,databuf,4);
        data.DataLen = 4;       
        g_sendchecktime = time(NULL);
        PDS_FMT_DEBUG("断路器 pn %d 时间补偿 \n",gTopologyArg->breakInfo[breakindex].Pn);
    }
        break;        
    //触发拓扑
    case TOPO_START:
    {
        PDS_FMT_DEBUG("拓扑识别状态机：触发拓扑 \n");
        
        if(breakindex >= gTopologyArg->breakCount)
        {
            PDS_FMT_DEBUG("所有断路器触发拓扑完毕 开始查询拓扑信息 \n");
            gTopologyArg->topoState = TOPO_START_METER;
            gTopologyArg->breakIndex =0;
            return 1;
        }
        
        memcpy(data.MtrAddr,gTopologyArg->breakInfo[breakindex].Addr,6);
        data.CtrlCode = 0x04;
        uint8 databuf[]={0x04,0x00,0x01,0x01,0x3c,0x02,0xaa,0xe9};
        memcpy(data.DataBuff,databuf,8);
        data.DataLen = 8;
        gTopologyArg->breakInfo[breakindex].starttime = time(NULL);
        PDS_FMT_DEBUG("断路器 pn %d 启动拓扑 \n",gTopologyArg->breakInfo[breakindex].Pn);
    }
        break;

    //启动搜表
    case TOPO_START_METER:
    {
        PDS_FMT_DEBUG("拓扑识别状态机：启动搜表 \n");
        breakindex = GetNextBreakIndexReadMeter();
        if(breakindex>=gTopologyArg->breakCount)
        {
            PDS_FMT_DEBUG("所有表箱断路器启动搜表成功 \n");
            gTopologyArg->topoState = TOPO_READ;
            gTopologyArg->breakIndex =0;
            return 1;
        }
        memcpy(data.MtrAddr,gTopologyArg->breakInfo[breakindex].Addr,6);
        data.CtrlCode = 0x13;
        uint8 databuf[]={0x0A,0x0B,0x0A,0x0B,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
        memcpy(data.DataBuff,databuf,12);
        data.DataLen = 12;
        PDS_FMT_DEBUG("断路器 pn %d 启动搜表 \n",gTopologyArg->breakInfo[breakindex].Pn);
    }
        break;    
 
    //读取拓扑记录
    case TOPO_READ:
    {
        PDS_FMT_DEBUG("拓扑识别状态机：读取拓扑 \n");
        
        if(breakindex >= gTopologyArg->breakCount)
        {
            PDS_FMT_DEBUG("所有读拓扑信息完毕 开始读档案信息 \n");
            gTopologyArg->topoState = TOPO_GETMETER;
            gTopologyArg->breakIndex =0;
            return 1;
        }

        memcpy(data.MtrAddr,gTopologyArg->breakInfo[breakindex].Addr,6);
        data.CtrlCode = 0x04;
        uint8 databuf[]={0x01,0xC0,0xC0,0x08,gTopologyArg->breakInfo[breakindex].topoInfoNum+1, TOPO_INFOLIMIT};
        memcpy(data.DataBuff,databuf,6);
        data.DataLen = 6;
        PDS_FMT_DEBUG("断路器 pn %d 从第%d条开始查询%d拓扑条信息\n",gTopologyArg->breakInfo[breakindex].Pn, gTopologyArg->breakInfo[breakindex].topoInfoNum+1, TOPO_INFOLIMIT);
    }
        break;
    
    //读取档案信息
    case TOPO_GETMETER:
    {
        PDS_FMT_DEBUG("拓扑识别状态机：读取档案 \n");
        breakindex = GetNextBreakIndexReadMeter();
        if(breakindex>=gTopologyArg->breakCount)
        {
            PDS_FMT_DEBUG("所有断路器读取表档案成功 \n");
            gTopologyArg->topoState = TOPO_FINISH;
            gTopologyArg->breakIndex =0;
            return 1;
        }
        memcpy(data.MtrAddr,gTopologyArg->breakInfo[breakindex].Addr,6);
        data.CtrlCode = 0x11;
        uint8 databuf[]={0x01,0x03,0xA0,0x04};
        memcpy(data.DataBuff,databuf,4);
        data.DataLen = 4;
        PDS_FMT_DEBUG("断路器 pn %d 查询表档案信息 \n",gTopologyArg->breakInfo[breakindex].Pn);
    }
        break;
    //结束 汇总拓扑结构 将信息写入数据中心
    case TOPO_FINISH:
    {
        PDS_FMT_DEBUG("拓扑识别状态机：结束 \n");
        return 2;
    }
        break;
    default:
        PDS_FMT_DEBUG("拓扑识别状态机：出错了 \n");
        break;
    }
    Sendlen = PackMeterPrtl(Sendbuf, data);
    ret = mqtt_sendtrans(Sendbuf,Sendlen,gTopologyArg->breakInfo[breakindex].port,gTopologyArg->breakInfo[breakindex].baud,gTopologyArg->overtime,&gTopologyArg->msgIndex);
    PDS_BUF_DEBUG(Sendbuf,Sendlen,"拓扑识别消息发送 msgindex %d ret %d \n",gTopologyArg->msgIndex,ret);
    return 0;
}

/*******************************************************************************
* 函数名称: topologyRecv
* 函数功能:处理拓扑识别收到的报文
* 输入参数: 
* 输出参数: 
* 返 回 值: 0 解析成功切换状态到发送 -1 解析有问题
*******************************************************************************/
int topologyRecv(uint8 *Recvbuf,uint16 Recvlen)
{
    Topology_Arg_T *gTopologyArg = &gCalcArg.topologyArg;
    uint16 breakindex = gTopologyArg->breakIndex;
    int ret;
    MeterPrtlFmt_T data;
    memset(&data,0x00,sizeof(MeterPrtlFmt_T));
    uint16 index = gTopologyArg->breakIndex;
    uint8 lenoffset;
    uint16 framelen = get_len_offset(Recvbuf,&lenoffset);
    if(framelen==0)
    {
        PDS_FMT_DEBUG("回码报文为空 \n");
        if(gTopologyArg->topoState==TOPO_TIMING)
        {
            PDS_FMT_DEBUG("收到广播校时回码  \n");
            gTopologyArg->delay = TOPO_DELAY_TIME;
            gTopologyArg->topoState=TOPO_CLEAR;
            return 0;
        }
        return -1;
    }
    ret = UnpackProtocol_645(&Recvbuf[lenoffset],framelen,&data);
    if(ret!=0)
    {
        PDS_FMT_DEBUG("645报文解析失败 ret %d \n",ret);
        return -1;
    }

    switch(gTopologyArg->topoState)
    {

        case TOPO_CLEAR:
        {
            if((data.CtrlCode == 0x84)&& !memcmp(data.MtrAddr, gTopologyArg->breakInfo[index].Addr,6))
            {
                PDS_FMT_DEBUG("断路器 pn %d 清拓扑记录成功 \n",gTopologyArg->breakInfo[index].Pn);
                gTopologyArg->breakIndex++;
            }else
            {
                PDS_FMT_DEBUG("断路器 pn %d 清拓扑记录异常 \n",gTopologyArg->breakInfo[index].Pn);
                return -1;
            }
        }
            break;
        case TOPO_TIME_OFFSET:
        {      
            if((data.CtrlCode == 0x91)&& !memcmp(data.MtrAddr, gTopologyArg->breakInfo[index].Addr,6))
            {
                PDS_FMT_DEBUG("断路器 pn %d 获取时间成功 \n",gTopologyArg->breakInfo[index].Pn);
                UnpackTimeInfo(data.DataBuff,data.DataLen);
                gTopologyArg->breakIndex++;
            }else
            {
                PDS_FMT_DEBUG("断路器 pn %d 获取时间异常 \n",gTopologyArg->breakInfo[index].Pn);
                return -1;
            }
        }
            break;        
        case TOPO_START:
        {
            if((data.CtrlCode == 0x84)&& !memcmp(data.MtrAddr, gTopologyArg->breakInfo[index].Addr,6))
            {
                PDS_FMT_DEBUG("断路器 pn %d 启动拓扑成功 \n",gTopologyArg->breakInfo[index].Pn);
                
                gTopologyArg->delay = TOPO_DELAY_TIME;
                gTopologyArg->breakIndex++;
            }else
            {
                PDS_FMT_DEBUG("断路器 pn %d 启动拓扑异常 \n",gTopologyArg->breakInfo[index].Pn);
                return -1;
            }
        }
            break;
        case TOPO_START_METER:
        {
            if((data.CtrlCode == 0x93)&& !memcmp(data.MtrAddr, gTopologyArg->breakInfo[index].Addr,6))
            {
                PDS_FMT_DEBUG("断路器 pn %d 启动搜表成功 \n",gTopologyArg->breakInfo[index].Pn);
                gTopologyArg->breakIndex++;
                
            }else
            {
                PDS_FMT_DEBUG("断路器 pn %d 启动搜表异常 \n",gTopologyArg->breakInfo[index].Pn);
                return -1;
            }
        }
            break;        
        case TOPO_READ:
        {
            if((data.CtrlCode==0x84)&& !memcmp(data.MtrAddr, gTopologyArg->breakInfo[index].Addr,6))
            {
                PDS_BUF_DEBUG(data.DataBuff,data.DataLen,"收到断路器 pn %d 拓扑信息 \n",gTopologyArg->breakInfo[index].Pn);

                if(UnpackTopoInfo(data.DataBuff,data.DataLen) < TOPO_INFOLIMIT)
                {
                    PDS_FMT_DEBUG("%012llx 拓扑记录总数 %d \n", *(uint64*)gTopologyArg->breakInfo[breakindex].Addr, gTopologyArg->breakInfo[breakindex].topoInfoNum);
                    gTopologyArg->breakIndex++;
                }
                
                //进入读表档案前，统一延时5分钟
                if(gTopologyArg->breakIndex >= gTopologyArg->breakCount)
                {
                    gTopologyArg->delay = TOPO_DELAY_TIME_METER;
                }
            }else
            {
                PDS_FMT_DEBUG("断路器 pn %d 查询拓扑信息异常 \n",gTopologyArg->breakInfo[index].Pn);
                return -1;
            }
        }
            break;
        case TOPO_GETMETER:
        {
            if((data.CtrlCode==0x91)&& !memcmp(data.MtrAddr, gTopologyArg->breakInfo[index].Addr,6))
            {
                PDS_BUF_DEBUG(data.DataBuff,data.DataLen," 收到断路器 pn %d 表档案信息 \n",gTopologyArg->breakInfo[index].Pn);
                UnpackMeterInfo(data.DataBuff,data.DataLen);
                gTopologyArg->breakIndex++;
            }else
            {
                PDS_FMT_DEBUG(" 断路器 pn %d 查询表档案信息异常 \n",gTopologyArg->breakInfo[index].Pn);
                return -1;
            }
            
        }
            break;
        case TOPO_FINISH:
        {

        }
            break;
        default:
            break;
    }
    return 0;
}

/*******************************************************************************
* 函数名称: topoISleaf
* 函数功能: 叶子判断
* 输入参数: BreakInfo 断路器信息
* 输出参数: 
* 返 回 值:0 写入成功 其他失败
*******************************************************************************/
BOOLEAN isTopolLeaf(Break_Info_T *BreakInfo)
{
    //没有儿子，或者儿子全是电表的节点为叶子节点
    return ((BreakInfo->SonNum == 0) || (BreakInfo->isMeterSon == TRUE));
}

/*******************************************************************************
* 函数名称: topoInfoUpdate
* 函数功能: 刷新拓扑关系软表
* 输入参数: Addr 档案地址
        Pn信息点号
* 输出参数: 
* 返 回 值:0 写入成功 其他失败
*******************************************************************************/
void topoInfoUpdate()
{
    uint16 i,j;
    Topology_Arg_T *gTopologyArg = &gCalcArg.topologyArg;
    Break_Info_T *BreakInfo=NULL;
    Break_Info_T *sonBreakInfo=NULL;
    Break_Info_T *parentBreakInfo=NULL;    

    for(i = 0;i<gTopologyArg->breakCount;i++)
    {
    
        BreakInfo = &gTopologyArg->breakInfo[i];
        
        //首先清零除表箱断路器外，所有断路器的子节点列表
        if(BreakInfo->isMeterSon  != TRUE)
        {
            BreakInfo->SonNum = 0;
        }

    }

    for(i = 0;i<gTopologyArg->breakCount;i++)
    {
    
        BreakInfo = &gTopologyArg->breakInfo[i];
        
        if(BreakInfo->nodeType  == NODE_LEAF)
        {
            //叶子的拓扑关系
             parentBreakInfo = &gTopologyArg->breakInfo[BreakInfo->ParentInfo[0].breakID];
             parentBreakInfo->breakSonInfo[parentBreakInfo->SonNum].breakID = i;
             memcpy(parentBreakInfo->breakSonInfo[parentBreakInfo->SonNum].addr, BreakInfo->Addr, 6);
             parentBreakInfo->SonNum++;
             
             //除去叶子和根，躯干断路器间拓扑关系
             for(j = 0; j < BreakInfo->ParentNum-1; j++)
             {
                 sonBreakInfo = &gTopologyArg->breakInfo[BreakInfo->ParentInfo[j].breakID];
                 parentBreakInfo = &gTopologyArg->breakInfo[BreakInfo->ParentInfo[j+1].breakID];

                 //为儿子找父亲
                 memcpy(&sonBreakInfo->ParentInfo[0], &BreakInfo->ParentInfo[j+1], sizeof(Break_Son_T));

                 //为父亲添儿子
                 memcpy(&parentBreakInfo->breakSonInfo[parentBreakInfo->SonNum++], &BreakInfo->ParentInfo[j], sizeof(Break_Son_T));
             }

             //根的拓扑关系
             sonBreakInfo = &gTopologyArg->breakInfo[BreakInfo->ParentInfo[j].breakID];
             //根的父亲为能源控制器
             memcpy(sonBreakInfo->ParentInfo[0].addr, LocalAddr, 6);
        }

    }

    for(i = 0; i<gTopologyArg->breakCount; i++)
    {
        BreakInfo = &gTopologyArg->breakInfo[i];
        
        for(j = 0; j < BreakInfo->SonNum; j++)
        {
            PDS_FMT_DEBUG("拓扑路径 %d %012llx-->%012llx-->%012llx\n", *(uint64*)BreakInfo->ParentInfo[0].addr&0xffffffffffff, *(uint64*)BreakInfo->Addr&0xffffffffffff, *(uint64*)BreakInfo->breakSonInfo[j].addr&0xffffffffffff);
        }         
    }
    
    return;
}
int GetEverPnPhase(uint16 pn)
{
    OOP_NODEPHASEINFO_T Pn_Phase;
    uint16 tmp;
    Pn_Phase = gettmpphase(pn);
//    if(sizeof(Pn_Phase)==0)       //读节点信息失败，直接返回
//      return 0;
    tmp = Pn_Phase.info&0x07;
    PDS_FMT_DEBUG("表节点 Pn %d 相位tmp %d\n",pn,tmp);
    if(tmp==1)
        return 1;
    if(tmp == 2)
        return 2;
    if(tmp == 4)
        return 3;
    if(tmp == 3 || tmp == 5 || tmp == 6 || tmp == 7)
        return 0xff;
    else
        return 0;
}
/*******************************************************************************
* 函数名称: WriteTopoInfo
* 函数功能: 感觉档案地址找对应的父节点信息
* 输入参数: Addr 档案地址
        Pn信息点号
* 输出参数: 
* 返 回 值:0 写入成功 其他失败
*******************************************************************************/
void SaveTopoResult()
{
    int ret;
    Topology_Arg_T *gTopologyArg = &gCalcArg.topologyArg;
    Break_Info_T *BreakInfo=NULL;
    Break_Info_T *sonBreakInfo=NULL;
    Break_Info_T *parentBreakInfo=NULL;
    OOP_NodeTopologyInfo_T TopoInfo;
    uint16 i,j;
    OOP_OAD_U oad ={0x60F10200};
    uint16 Pn;
    uint8 Addr[6]={0};

    for(i = 0;i<gTopologyArg->breakCount;i++)
    {
        BreakInfo = &gTopologyArg->breakInfo[i];
        PDS_FMT_DEBUG("断路器 Pn %d %012llx, 亲代个数：%d, 子代个数：%d, 是否有电表：%d\n", BreakInfo->Pn, *(uint64*)BreakInfo->Addr, BreakInfo->ParentNum, BreakInfo->SonNum, BreakInfo->isMeterSon);

        //叶子与其亲代列表组成一条从根到叶子的完整路径
        if(isTopolLeaf(BreakInfo))
        {
            //遍历亲代列表，并根据亲代的子代数升序排序，从叶到根
            bubble_Sort(BreakInfo->ParentInfo, BreakInfo->ParentNum, topology_swap);
            
            //如果该叶子是表箱断路器,儿子全是电表
            if(BreakInfo->isMeterSon == TRUE)
            {

                for(;;) //为每个电表找到父亲
                {
                    Pn = GetNextNonBreakPLC(Addr);
                    if(Pn == 0)
                    {
                        break;
                    }

                    //每个表都有节点信息，档案序号作为信息点号来存储
                    for(j = 0; j < BreakInfo->SonNum; j++)
                    {
                        if(memcmp(Addr,BreakInfo->breakSonInfo[j].addr,6)==0)
                        {
                            memset(&TopoInfo,0x00,sizeof(OOP_NodeTopologyInfo_T));
                            TopoInfo.NodeDevAddr.len = 5;
                            memcpy_r(TopoInfo.NodeDevAddr.add, Addr,6);
                            TopoInfo.NodePhaseInfo = GetEverPnPhase(Pn);;
                            TopoInfo.ParentNodeDevAddr.len = 5;
                            memcpy_r(TopoInfo.ParentNodeDevAddr.add,BreakInfo->Addr,6);
                            TopoInfo.ParentNodeLineAddr.len = 5;
                            memcpy_r(TopoInfo.ParentNodeLineAddr.add,BreakInfo->Addr,6);
                            ret = WriteNormalData(gCalcArg.clientid,(uint8*)&TopoInfo,sizeof(OOP_NodeTopologyInfo_T),oad,Pn);
                            PDS_FMT_DEBUG("表节点 Pn %d %012llx<--%012llx, phase %d ret %d  \n",Pn, *(uint64*)Addr, *(uint64*)BreakInfo->Addr, TopoInfo.NodePhaseInfo, ret);
                        }
                    }

                }
            }
            
            if(BreakInfo->ParentNum == 0)
            {
                //非报文交互失败导致的，仍然认为是根，想要与既不发送也不接收信号的故障断路器区分，除非能源控制器能收拓扑信号
                if(BreakInfo->failcnt == 0) 
                {
                    BreakInfo->nodeType = NODE_ROOT;
                    memset(&TopoInfo,0x00,sizeof(OOP_NodeTopologyInfo_T));
                    TopoInfo.NodeDevAddr.len = 5;
                    memcpy_r(TopoInfo.NodeDevAddr.add,BreakInfo->Addr ,6);
                    TopoInfo.NodePhaseInfo = 0xff;
                    TopoInfo.ParentNodeDevAddr.len = 5;
                    memcpy_r(TopoInfo.ParentNodeDevAddr.add,LocalAddr,6);
                    TopoInfo.ParentNodeLineAddr.len = 5;
                    memcpy_r(TopoInfo.ParentNodeLineAddr.add,LocalAddr,6);
                    ret = WriteNormalData(gCalcArg.clientid,(uint8*)&TopoInfo,sizeof(OOP_NodeTopologyInfo_T),oad,BreakInfo->Pn);
                    PDS_FMT_DEBUG("孤节点 Pn %d %012llx<--%012llx, phase %d ret %d  \n",BreakInfo->Pn, *(uint64*)BreakInfo->Addr, *(uint64*)LocalAddr, TopoInfo.NodePhaseInfo, ret);
                }
                else
                {
                    PDS_FMT_DEBUG("坏节点 Pn %d %012llx\n", BreakInfo->Pn, *(uint64*)BreakInfo->Addr);
                }
            }
            else
            {
                parentBreakInfo = &gTopologyArg->breakInfo[BreakInfo->ParentInfo[0].breakID];
                BreakInfo->nodeType = NODE_LEAF;
                memset(&TopoInfo,0x00,sizeof(OOP_NodeTopologyInfo_T));
                TopoInfo.NodeDevAddr.len = 5;
                memcpy_r(TopoInfo.NodeDevAddr.add,BreakInfo->Addr,6);
                TopoInfo.NodePhaseInfo = 0xff; //相位来自父亲 //修改为恒定0xff
                TopoInfo.ParentNodeDevAddr.len = 5;
                memcpy_r(TopoInfo.ParentNodeDevAddr.add,parentBreakInfo->Addr,6);
                TopoInfo.ParentNodeLineAddr.len = 5;
                memcpy_r(TopoInfo.ParentNodeLineAddr.add,parentBreakInfo->Addr,6);
                ret = WriteNormalData(gCalcArg.clientid,(uint8*)&TopoInfo,sizeof(OOP_NodeTopologyInfo_T),oad,BreakInfo->Pn);
                PDS_FMT_DEBUG("叶节点 Pn %d %012llx<--%012llx, phase %d ret %d  \n",BreakInfo->Pn, *(uint64*)BreakInfo->Addr, *(uint64*)parentBreakInfo->Addr, TopoInfo.NodePhaseInfo, ret);

                //除去叶子和根，躯干断路器间拓扑关系
                for(j = 0; j < BreakInfo->ParentNum-1; j++)
                {
                    sonBreakInfo = &gTopologyArg->breakInfo[BreakInfo->ParentInfo[j].breakID];
                    parentBreakInfo = &gTopologyArg->breakInfo[BreakInfo->ParentInfo[j+1].breakID];

                    sonBreakInfo->nodeType = NODE_BODY;

                    memset(&TopoInfo,0x00,sizeof(OOP_NodeTopologyInfo_T));
                    TopoInfo.NodeDevAddr.len = 5;
                    memcpy_r(TopoInfo.NodeDevAddr.add,sonBreakInfo->Addr,6);
                    TopoInfo.NodePhaseInfo = 0xff; //相位来自父亲
                    TopoInfo.ParentNodeDevAddr.len = 5;
                    memcpy_r(TopoInfo.ParentNodeDevAddr.add,parentBreakInfo->Addr,6);
                    TopoInfo.ParentNodeLineAddr.len = 5;
                    memcpy_r(TopoInfo.ParentNodeLineAddr.add,parentBreakInfo->Addr,6);
                    ret = WriteNormalData(gCalcArg.clientid,(uint8*)&TopoInfo,sizeof(OOP_NodeTopologyInfo_T),oad,sonBreakInfo->Pn);
                    PDS_FMT_DEBUG("干节点 Pn %d %012llx<--%012llx, phase %d ret %d  \n",sonBreakInfo->Pn, *(uint64*)sonBreakInfo->Addr, *(uint64*)parentBreakInfo->Addr, TopoInfo.NodePhaseInfo, ret);

                }

                //根为最后一个，根的父亲为能源控制器    
                sonBreakInfo = &gTopologyArg->breakInfo[BreakInfo->ParentInfo[j].breakID];
                sonBreakInfo->nodeType = NODE_ROOT;
                
                memset(&TopoInfo,0x00,sizeof(OOP_NodeTopologyInfo_T));
                TopoInfo.NodeDevAddr.len = 5;
                memcpy_r(TopoInfo.NodeDevAddr.add,sonBreakInfo->Addr ,6);
                TopoInfo.NodePhaseInfo = 0xff;
                TopoInfo.ParentNodeDevAddr.len = 5;
                memcpy_r(TopoInfo.ParentNodeDevAddr.add,LocalAddr,6);
                TopoInfo.ParentNodeLineAddr.len = 5;
                memcpy_r(TopoInfo.ParentNodeLineAddr.add,LocalAddr,6);
                ret = WriteNormalData(gCalcArg.clientid,(uint8*)&TopoInfo,sizeof(OOP_NodeTopologyInfo_T),oad,sonBreakInfo->Pn);
                PDS_FMT_DEBUG("根节点 Pn %d %012llx<--%012llx, phase %d ret %d  \n",sonBreakInfo->Pn, *(uint64*)sonBreakInfo->Addr, *(uint64*)LocalAddr, TopoInfo.NodePhaseInfo, ret);

            }
        }
    }

    memset(&TopoInfo,0x00,sizeof(OOP_NodeTopologyInfo_T));
    TopoInfo.NodeDevAddr.len = 5;
    memcpy_r(TopoInfo.NodeDevAddr.add, LocalAddr ,6);
    TopoInfo.NodePhaseInfo = 0xff;
    TopoInfo.ParentNodeDevAddr.len = 5;
    memcpy_r(TopoInfo.ParentNodeDevAddr.add,LocalAddr,6);
    TopoInfo.ParentNodeLineAddr.len = 5;
    memcpy_r(TopoInfo.ParentNodeLineAddr.add,LocalAddr,6);
    ret = WriteNormalData(gCalcArg.clientid,(uint8*)&TopoInfo,sizeof(OOP_NodeTopologyInfo_T),oad, 0);
    PDS_FMT_DEBUG("辅节点 Pn 0 %012llx-->%012llx, phase %d ret %d  \n", *(uint64*)LocalAddr, *(uint64*)LocalAddr, TopoInfo.NodePhaseInfo, ret);

    //数据中心存储完成后刷新一遍全局变量，便于后面的计算
    topoInfoUpdate();
    
    return;
}

/*******************************************************************************
* 函数名称: topologyManager
* 函数功能: 处理拓扑识别功能 包括启动拓扑，读取拓扑记录，计算拓扑关系等
* 输入参数: 
* 输出参数: 
* 返 回 值: 
*******************************************************************************/
void topologyManager()
{
    Topology_Arg_T *gTopologyArg = &gCalcArg.topologyArg;
    int ret;
    if(gTopologyArg->isInTopo == 0)
        return ;
    uint16 Recvlen = 0;
    uint8 Recvbuf[MAX_MSG_LEN]={0};
    MESSAGE_INFO_T *MsgRecv = NULL;
    static struct timespec Sendtime;
    struct timespec Curtime;
    const uint16 overtime = 60; //默认超时时间60秒
    uint16 breakindex=0;
    //执行拓扑识别功能
    switch(gTopologyArg->sendRecvState)
    {
        case PDS_INIT:
        {
            PDS_FMT_DEBUG("拓扑收发状态机：初始\n");
            if(getAllBreaks()==0)
            {
                PDS_FMT_DEBUG("当前档案中未识别到断路器，无法进行拓扑识别功能！ \n");
                gTopologyArg->isInTopo =0;
                gTopologyArg->isFinish =0;
                return;
            }

            if(pds_topodata_clear() == 0)
            {
                PDS_FMT_DEBUG("删除全部拓扑信息成功！ \n");
            }
            
            gTopologyArg->breakIndex = 0;
            gTopologyArg->overtime = overtime;
            gTopologyArg->topoState = TOPO_TIMING;
            gTopologyArg->sendRecvState =PDS_SEND; 
        }
            break;
        case PDS_SEND:
        {
            PDS_FMT_DEBUG("拓扑收发状态机：发送\n");
            ret = topologySend();
            if(ret == 0)
            {
                clock_gettime(CLOCK_MONOTONIC, &Sendtime);
                gTopologyArg->sendRecvState = PDS_RECV;
            }else if(ret ==1)
            {
                break;
            }else
            {
                PDS_FMT_DEBUG("拓扑识别结束！ \n");
                SaveTopoResult();
                gTopologyArg->isInTopo =0;
                gTopologyArg->sendRecvState = PDS_INIT;
                gTopologyArg->isFinish= 1;
                SaveTopoInfoFile(gTopologyArg);
                return;
            }
        }
            break;
        case PDS_DELAY:
        {
            if( DelayDeal(&gTopologyArg->delay)==0)
            {
                PDS_FMT_DEBUG("延时结束 \n");
                gTopologyArg->sendRecvState =PDS_SEND; 
            }
        }
            break;
        case PDS_RECV:
        {
            Recvlen = TransReadQueue(&gTransQueue, Recvbuf);
            if(Recvlen>0)
            {
                MsgRecv = (MESSAGE_INFO_T*)Recvbuf;
                if(MsgRecv->index!=gTopologyArg->msgIndex)
                {
                    PDS_FMT_DEBUG("收到的消息序号 %d 不等于发出的消息序号 %d  \n",MsgRecv->index,gTopologyArg->msgIndex);
                    break;
                }
                PDS_BUF_TRACE(MsgRecv->playload, MsgRecv->msglen,"收到回码消息 \n");
                ret = topologyRecv(MsgRecv->playload, MsgRecv->msglen);
                if(ret == 0)
                {
                    gTopologyArg->sendRecvState = PDS_DELAY;
                }else
                {
                    gTopologyArg->sendRecvState = PDS_RETRY;
                }
            }else
            {
                clock_gettime(CLOCK_MONOTONIC, &Curtime);
                if(Curtime.tv_sec - Sendtime.tv_sec>overtime)
                {
                    PDS_FMT_DEBUG("发送超时 \n");
                    gTopologyArg->sendRecvState = PDS_RETRY;
                }
            }
        }
            break;
        case PDS_RETRY:
        {
            PDS_FMT_DEBUG("拓扑收发状态机：重试\n");
            breakindex = gTopologyArg->breakIndex;
            gTopologyArg->breakInfo[breakindex].failcnt++;
            PDS_FMT_DEBUG("断路器 pn %d 失败次数 %d \n",gTopologyArg->breakInfo[breakindex].Pn,gTopologyArg->breakInfo[breakindex].failcnt);
            gTopologyArg->sendRecvState =PDS_SEND;
        }
            break;
        default:
            PDS_FMT_DEBUG("拓扑收发状态机：出错了 \n");
            break;
    }
}

#endif

#if DESC("线损相关代码",1)
/**********************************************************************
* @name      : GetMeterEnergyData
* @brief     ：根据信息点号和存储时间 获取正向有功总电能
* @param[in] ：pn 信息点号
            addr 地址
            storetime 存储时间
* @param[out]：data 电量数据
* @return    ：0-成功/错误码

**********************************************************************/
#ifdef AREA_HUNAN
int GetMeterEnergyData(Meter_Data_T *meterData, OOP_OAD_U mainoad, uint32 storetime, PDS_ENERGY4V_T* data)
{
    int ret;
    uint8 i,j;
    RECORD_UNIT_T RecordOut;
    READ_RECORD_T startReadRecord;
    OOP_ENERGY_T energys;
    OOP_DATETIME_S_T timetmp;

    DT_Time2DateTimeShort(storetime, &timetmp);
    
    MEMZERO(&energys, sizeof(OOP_ENERGY_T));
    memset((uint8*)&startReadRecord,0x00,sizeof(READ_RECORD_T));
    memset((uint8*)&RecordOut,0x00,sizeof(RECORD_UNIT_T));
    startReadRecord.recordOAD.road.oadMain.value = mainoad.value;
    startReadRecord.recordOAD.road.oadCols.nNum = 4;
    startReadRecord.recordOAD.road.oadCols.oad[0].value =0x00100200;
    startReadRecord.recordOAD.road.oadCols.oad[1].value =0x00110200;
    startReadRecord.recordOAD.road.oadCols.oad[2].value =0x00120200;
    startReadRecord.recordOAD.road.oadCols.oad[3].value =0x00130200;
    startReadRecord.recordOAD.logicId = 0;
    startReadRecord.recordOAD.infoNum = meterData->Pn;
    startReadRecord.recordOAD.classtag =5;
    startReadRecord.MAC.nNum=6;
    memcpy_r(startReadRecord.MAC.value,meterData->Addr,6);
    startReadRecord.cType = COL_TM_STORE;
    startReadRecord.cStart = storetime+READ_DATA_TIME_ADD;
    startReadRecord.cEnd = storetime+READ_DATA_TIME_ADD;
    
    ret = db_read_record(gCalcArg.clientid ,&startReadRecord, DATA_LEN_MAX,&RecordOut);
    if((ret!=0) || (RecordOut.recordnum == 0))
    {
        PDS_FMT_DEBUG("pn %d %012llx 获取电量数据失败 ret %d, recordnum %d, oadMain 0x%08x, store %04d-%02d-%02d %02d:%02d:%02d\n",meterData->Pn,*(uint64*)meterData->Addr&0xffffffffffff, ret,RecordOut.recordnum,
            mainoad, timetmp.year, timetmp.month, timetmp.day,timetmp.hour, timetmp.minute, timetmp.second);

        return ret;
    }

    data->nNum = startReadRecord.recordOAD.road.oadCols.nNum;

    for(i = 0; i < startReadRecord.recordOAD.road.oadCols.nNum; i++)
    {
        
        for(j = 0; j < RecordOut.recordnum; j++)
        {
            if(RecordOut.record[j].road.oadCols.oad[0].value != startReadRecord.recordOAD.road.oadCols.oad[i].value)
            {
                continue;
            }

            //总及费率电能量只取总电能量
            memcpy(&energys, RecordOut.record[j].outDataBuf, sizeof(OOP_ENERGY_T));
            data->nValue[i] = energys.nValue[0]*meterData->Pt*meterData->Ct;
            set_bit_value((uint8*)&data->rsv, 32, i);

            break;
        }

        if(j == RecordOut.recordnum)
        {
            PDS_FMT_DEBUG("pn %d all&phase energys get failed. oad(0x%08x)\n", meterData->Pn, startReadRecord.recordOAD.road.oadCols.oad[i].value);
        }

    }

    return 0;
}
#else
int GetMeterEnergyData(Meter_Data_T *meterData, OOP_OAD_U mainoad, uint32 storetime, PDS_ENERGY4V_T* data)
{
    int ret;
    uint8 i,j;
    RECORD_UNIT_T RecordOut;
    READ_RECORD_T startReadRecord;
    OOP_ENERGY_T energys;
    OOP_DATETIME_S_T timetmp;

    DT_Time2DateTimeShort(storetime, &timetmp);
    
    MEMZERO(&energys, sizeof(OOP_ENERGY_T));
    memset((uint8*)&startReadRecord,0x00,sizeof(READ_RECORD_T));
    memset((uint8*)&RecordOut,0x00,sizeof(RECORD_UNIT_T));
    startReadRecord.recordOAD.road.oadMain.value = 0;
    startReadRecord.recordOAD.road.oadCols.nNum = 4;
    startReadRecord.recordOAD.road.oadCols.oad[0].value =0x00100200;
    startReadRecord.recordOAD.road.oadCols.oad[1].value =0x00110200;
    startReadRecord.recordOAD.road.oadCols.oad[2].value =0x00120200;
    startReadRecord.recordOAD.road.oadCols.oad[3].value =0x00130200;
    startReadRecord.recordOAD.logicId = 0;
    startReadRecord.recordOAD.infoNum = meterData->Pn;
    startReadRecord.recordOAD.classtag =5;
    startReadRecord.MAC.nNum=6;
    memcpy_r(startReadRecord.MAC.value,meterData->Addr,6);
    startReadRecord.cType = COL_TM_STORE;
    startReadRecord.cStart = storetime+READ_DATA_TIME_ADD;
    startReadRecord.cEnd = storetime+READ_DATA_TIME_ADD+60;
    
    ret = db_read_record(gCalcArg.clientid ,&startReadRecord, DATA_LEN_MAX,&RecordOut);
    if((ret!=0) || (RecordOut.recordnum == 0))
    {
        PDS_FMT_DEBUG("pn %d %012llx 获取电量数据失败 ret %d, recordnum %d, oadMain 0x%08x, store %04d-%02d-%02d %02d:%02d:%02d\n",meterData->Pn,*(uint64*)meterData->Addr&0xffffffffffff, ret,RecordOut.recordnum,
            mainoad, timetmp.year, timetmp.month, timetmp.day,timetmp.hour, timetmp.minute, timetmp.second);

        return ret;
    }

    data->nNum = startReadRecord.recordOAD.road.oadCols.nNum;

    for(i = 0; i < startReadRecord.recordOAD.road.oadCols.nNum; i++)
    {
        
        for(j = 0; j < RecordOut.recordnum; j++)
        {
            if(RecordOut.record[j].road.oadCols.oad[0].value != startReadRecord.recordOAD.road.oadCols.oad[i].value)
            {
                continue;
            }

            //总及费率电能量只取总电能量
            memcpy(&energys, RecordOut.record[j].outDataBuf, sizeof(OOP_ENERGY_T));
            data->nValue[i] = energys.nValue[0]*meterData->Pt*meterData->Ct;
            set_bit_value((uint8*)&data->rsv, 32, i);
            
            break;
        }

        if(j == RecordOut.recordnum)
        {
            PDS_FMT_DEBUG("pn %d all&phase energys get failed. oad(0x%08x)\n", meterData->Pn, startReadRecord.recordOAD.road.oadCols.oad[i].value);
        }

    }
    if(((meterData->phaseinfo)&0x08)==0x00)
    {
        switch((meterData->phaseinfo)&0x07)
        {
            case 1:
                {
                    data->nValue[1] = data->nValue[0];
                    set_bit_value((uint8*)&data->rsv, 32, 1);
                    
                PDS_FMT_DEBUG("A 相数据 data->nValue[0] is %d\n",data->nValue[0]);
            break;
            case 2:
                {
                    data->nValue[2] = data->nValue[0];
                    set_bit_value((uint8*)&data->rsv, 32, 2);
                    
                PDS_FMT_DEBUG("B 相数据 data->nValue[0] is %d\n",data->nValue[0]);
                }
            break;
            case 4:
                {
                    data->nValue[3] = data->nValue[0];
                    set_bit_value((uint8*)&data->rsv, 32, 3);
                    
                PDS_FMT_DEBUG("C 相数据 data->nValue[0] is %d\n",data->nValue[0]);
                }
            break;
            default:
                break;

        }
        }
    }
   else if(((meterData->phaseinfo)&0x07) == 0)
    {
        data->nValue[1] = data->nValue[0];
        set_bit_value((uint8*)&data->rsv, 32, 1);
        PDS_FMT_DEBUG("AA 相数据 data->nValue[0] is %d\n",data->nValue[0]);
    }
    return 0;
}



#endif

/**********************************************************************
* @name      : GetAllEnergyData
* @brief     ：加载所有档案的数据
* @param[in] ：

* @param[out]：data 电量数据
* @return    ：0-成功/错误码

**********************************************************************/
int GetAllEnergyData(All_Meter_Data_T *AllMeterData)
{
    uint16 i;
    DB_CLIENT client;
    NOMAL_OAD_T NormalOad;
    int ret=0;
    OOP_NODEPHASEINFO_T nodeinfo;
    uint32 len=0;
    NormalOad.logicId = 0;
    NormalOad.oad.value = 0xF2090A00;
    NormalOad.infoNum = 0;
    client = db_client_create(DB_PDS_AGENT, DB_PDS_CLIENT);
    if(client <= 0)
    {
        PDSDB_FMT_DEBUG("DB_PDS_CLIENT db_client_create ERROR \n");
        return -1;
    }
    Line_Loss_Calc_T *gLineLossArg = &gCalcArg.lineLossArg;
    Meter_Data_T *data = NULL;
    pthread_rwlock_rdlock(&MeterFileAll.rwLock);
    for(i=0;i<MeterFileAll.MeterCount;i++)
    {
        AllMeterData->data[i].Pn = MeterFileAll.MeterFile[i].nIndex;
        memcpy_r(AllMeterData->data[i].Addr,MeterFileAll.MeterFile[i].basic.tsa.add,6);
        AllMeterData->data[i].Pt = MeterFileAll.MeterFile[i].extend.PT;
        AllMeterData->data[i].Ct = MeterFileAll.MeterFile[i].extend.CT;       
    }
    AllMeterData->count = MeterFileAll.MeterCount;
    pthread_rwlock_unlock(&MeterFileAll.rwLock);

	OOP_DATETIME_S_T time_tmp;
    for(i=0;i<AllMeterData->count;i++)
    {
        NormalOad.infoNum = AllMeterData->data[i].Pn;
        ret = db_read_nomal(client,&NormalOad,sizeof(nodeinfo),(uint8*)&nodeinfo,&len);
        if(ret ==0)
        {
         AllMeterData->data[i].phaseinfo = (nodeinfo.info);
             
         PDS_FMT_DEBUG("该表的相位是 AllMeterData->data[i].phaseinfo %d\n,AllMeterData->data[i].Pn is %d\n",AllMeterData->data[i].phaseinfo,AllMeterData->data[i].Pn);
        }
    
        data = &AllMeterData->data[i];
        
        GetMeterEnergyData(data,gLineLossArg->mainOad,gLineLossArg->lastStoreTime[0],&data->EleData[0]);
        DT_Time2DateTimeShort(gLineLossArg->lastStoreTime[0], &time_tmp);
        PDS_FMT_DEBUG("pn %d MainOad 0x%08x pt%d ct%d 上次存储时间%04d-%02d-%02d %02d:%02d:%02d 上次正向有功总值 %d\n",data->Pn,gLineLossArg->mainOad,data->Pt,data->Ct,time_tmp.year,time_tmp.month,time_tmp.day,time_tmp.hour,time_tmp.minute,time_tmp.second,data->EleData[0].nValue[0]);
        GetMeterEnergyData(data,gLineLossArg->mainOad,gLineLossArg->lastStoreTime[1],&data->EleData[1]);
        DT_Time2DateTimeShort(gLineLossArg->lastStoreTime[1], &time_tmp);
        PDS_FMT_DEBUG("pn %d MainOad 0x%08x pt%d ct%d 上上次存储时间%04d-%02d-%02d %02d:%02d:%02d 上上次正向有功总值 %d\n",data->Pn,gLineLossArg->mainOad,data->Pt,data->Ct,time_tmp.year,time_tmp.month,time_tmp.day,time_tmp.hour,time_tmp.minute,time_tmp.second,data->EleData[1].nValue[0]);
    }
    return 0;
}

void CalcEnergyIncrement(PDS_ENERGY4V_T *data1,PDS_ENERGY4V_T *data2,PDS_ENERGY4V_T *outdata)
{
    uint8 i;
    outdata->nNum = 4;
    
    for(i=0;i<4;i++)
    {
        if((0==get_bit_value((uint8*)&data1->rsv,32,i))||(0==get_bit_value((uint8*)&data2->rsv,32,i)))
        {            
            continue;
        }

        set_bit_value((uint8*)&outdata->rsv, 32, i);
        outdata->nValue[i] = data1->nValue[i]-data2->nValue[i];
    }

}

/**********************************************************************
* @name      : EnergyDataAdd
* @brief     ：子节点电量相加
* @param[in] ：

* @param[out]：
* @return    ：0-成功/错误码

**********************************************************************/
void EnergyDataAdd(PDS_ENERGY4V_T *dataresult,PDS_ENERGY4V_T *data)
{
    dataresult->nNum = 4;
    uint8 i;
    
    for(i=0;i<4;i++)
    {
        if(0!=get_bit_value((uint8*)&data->rsv,32,i))
        {
            dataresult->nValue[i]+=data->nValue[i];
            set_bit_value((uint8*)&dataresult->rsv,32,i);
        }
    }
}

 int pds_ac_energy_get(Line_Loss_Calc_T *glineLossArg, PDS_ENERGY4V_T *data)
{
    int ret = 0;
    Meter_Data_T meterData;

    memset(&meterData, 0, sizeof(Meter_Data_T));

    memcpy(meterData.Addr, LocalAddr, sizeof(meterData.Addr));

    pds_ac_ptct_get(&meterData.Pt, &meterData.Ct);

    ret = GetMeterEnergyData(&meterData, glineLossArg->mainOad, glineLossArg->lastStoreTime[0], &data[0]);
    ret |= GetMeterEnergyData(&meterData, glineLossArg->mainOad, glineLossArg->lastStoreTime[1], &data[1]);

    return ret;
}

 PDS_ENERGY4V_T * GetEnergyData(uint16 pn)
{
    uint16 i;

    if(pn==0)
    {
        return NULL;
    }
        
    for(i=0;i<AllMeterData.count;i++)
    {
        if(pn == AllMeterData.data[i].Pn)
        {
            return AllMeterData.data[i].EleData;
        }
    }
    
    return NULL;
}

/**********************************************************************
* @name      : CalcLineLossData
* @brief     根据父节点和子节点的电量计算线损数据 线损率=（（父节点电量增量-父节点所有子节点电量增量之和）/父节点电量增量）*100%
* @param[in] ：

* @param[out]：
* @return    ：0-成功/错误码

**********************************************************************/
void CalcLineLossData(Line_Loss_Calc_T *lineLossArg, uint16 pn, uint8 *addr, PDS_ENERGY4V_T *parentdata, PDS_ENERGY4V_T *sondata)
{
    uint8 i;
    WRITE_RECORD_T inRecord;
    uint32 storeNo = 0;
    int ret  = 0;
    OOP_OAD_U oad = {0x60510200};
    OOP_LNLS_STATISVAL_T lineLossData;
    PDS_ENERGY4V_T parentIncre;
    PDS_ENERGY4V_T sonIncre;

    memset(&lineLossData,0x00,sizeof(OOP_LNLS_STATISVAL_T));
    memset(&inRecord,0x00,sizeof(READ_RECORD_T));
    memset(&parentIncre,0x00,sizeof(PDS_ENERGY4V_T));
    memset(&sonIncre,0x00,sizeof(PDS_ENERGY4V_T));

    PDS_FMT_DEBUG("line loss calc Pn %d\n", pn);

    //计算父节点电量增量
    CalcEnergyIncrement(parentdata, parentdata+1,&parentIncre);

	PDS_FMT_DEBUG("父节点正向有功总值增量%d\n",parentIncre.nValue[0]);

    //计算子节点电量增量
    CalcEnergyIncrement(sondata, sondata+1,&sonIncre);
	PDS_FMT_DEBUG("子节点正向有功总值增量%d\n",sonIncre.nValue[0]);
 
    lineLossData.nNum = 4;
    
    for(i=0; i<4; i++)
    {
        PDS_FMT_DEBUG("item: %d valid: p %d s %d incre: p %d s %d\n", 
            i, parentIncre.rsv, sonIncre.rsv, parentIncre.nValue[i], sonIncre.nValue[i]);
        
        if((get_bit_value((uint8*)&parentIncre.rsv,32,i)!= 0) 
            && (get_bit_value((uint8*)&sonIncre.rsv,32,i)!= 0) && (parentIncre.nValue[i]!=0))
        {
            lineLossData.nVal[i].eneSup = parentIncre.nValue[i];
            lineLossData.nVal[i].eneSal = sonIncre.nValue[i];
            //lineLossData.nVal[i].nRatio = (parentIncre.nValue[i])<=(sonIncre.nValue[i])?0:(((parentIncre.nValue[i]-sonIncre.nValue[i])*100)/parentIncre.nValue[i]);
            lineLossData.nVal[i].nRatio = ((parentIncre.nValue[i]-sonIncre.nValue[i])*100)/parentIncre.nValue[i];
            lineLossData.nVal[i].abnorFlag = 0;

            set_bit_value((uint8*)&lineLossData.rsv,32,i);
            
            PDS_FMT_DEBUG("item: %d valid: %d nRatio: %d\n", i, lineLossData.rsv, lineLossData.nVal[i].nRatio);
        }
        else
        {
            lineLossData.nVal[i].eneSup = 0;
            lineLossData.nVal[i].eneSal = 0;
            lineLossData.nVal[i].nRatio = 0;
            lineLossData.nVal[i].abnorFlag = 0; 
            set_bit_value((uint8*)&lineLossData.rsv,32,i);
        }
    }

    inRecord.recordOAD.classtag = CLASS_DATA_COLLEC;
    inRecord.recordOAD.infoNum = pn;
    inRecord.recordOAD.road.oadMain.value = lineLossArg->mainOad.value;
    inRecord.recordOAD.road.oadCols.nNum = 1;
    inRecord.recordOAD.road.oadCols.oad[0].value = oad.value;
    inRecord.MAC.nNum = 6;
    memcpy_r(inRecord.MAC.value, addr, 6);
    inRecord.colStartTm = lineLossArg->lastStoreTime[0]+READ_DATA_TIME_ADD;
    inRecord.colEndTm = lineLossArg->lastStoreTime[0]+READ_DATA_TIME_ADD;
    inRecord.colStoreTm = lineLossArg->lastStoreTime[0]+READ_DATA_TIME_ADD; 
    inRecord.inDatalen = sizeof(OOP_LNLS_STATISVAL_T);
    memcpy(inRecord.inDataBuf, &lineLossData, inRecord.inDatalen);
    
    ret = db_write_record(gCalcArg.clientid, &inRecord, &storeNo);
    if(ret!=0)
    {
        PDS_FMT_DEBUG("line loss data write failed, Pn %d ret %d\n", pn, ret);
    }      
}
/**********************************************************************
* @name      : LineLossCalc
* @brief     ：线损计算函数
* @param[in] ：

* @param[out]：
* @return    ：0-成功/错误码

**********************************************************************/
int LineLossCalc()
{
    Line_Loss_Calc_T *glineLossArg = &gCalcArg.lineLossArg;
    #ifdef AREA_HUNAN
    Topology_Arg_T *gTopologyArg = &gCalcArg.topologyArg;
    Break_Info_T *Breakinfo = NULL;
    #endif

    #ifdef AREA_HUNAN
    PDS_ENERGY4V_T sondata[2]; 
    uint16 i,j;
    #endif
    #ifdef AREA_JIANGXI
    PDS_ENERGY4V_T sondata[2]; 
    //PDS_ENERGY4V_T allleafdata[2]={{0}};     //所有子节点电量之和
    uint16 i;
    #endif
    uint16 pn;
    PDS_ENERGY4V_T *data = NULL;
    PDS_ENERGY4V_T parentdata[2];       //父节点电量
    //PDS_ENERGY4V_T sondata[2];  
    //#ifdef AREA_HUNAN//所有子节点电量之和
    PDS_ENERGY4V_T allleafdata[2]={{0}};     //所有子节点电量之和
    //#endif
    uint8 Addr[6] = {0};

    memset(&AllMeterData,0x00,sizeof(All_Meter_Data_T));    
    #ifdef AREA_HUNAN
    if(gTopologyArg->isFinish == 0)
    {
        PDS_FMT_DEBUG("当前没有识别到拓扑结构 无法进行线损计算 \n");
        gCalcArg.lineLossArg.isNeedCalc =0;
        return -1;
    }
    #endif
    GetAllEnergyData(&AllMeterData);
    #ifdef AREA_HUNAN
    //依次计算每个断路器的线损数据
    for(i=0; i<gTopologyArg->breakCount; i++)
    {
        Breakinfo = &gTopologyArg->breakInfo[i];
        data = GetEnergyData(Breakinfo->Pn);
        if(NULL==data || data[0].nNum != 4 || data[1].nNum != 4)
        {
            PDS_FMT_DEBUG("parent pn %d frozen data abnormal, lstnum %d lstlstnum %d\n",Breakinfo->Pn, data[0].nNum, data[1].nNum);
            continue;
        }
		PDS_FMT_DEBUG("断路器 pn  %d 正向有功增量 %d\n",Breakinfo->Pn,((data->nValue[0])-(data->nValue[1])));
        memcpy(&parentdata,data,sizeof(parentdata));

        memset(&sondata,0x00,sizeof(PDS_ENERGY4V_T)*2);
        
        for(j=0;j<Breakinfo->SonNum;j++)
        {
            
            pn = GetPnFromAddr(Breakinfo->breakSonInfo[j].addr);
            if(pn==0)
            {
                PDS_BUF_DEBUG(Breakinfo->breakSonInfo[j].addr,6,"未找到档案序号 \n");
                continue;
            }
            data = GetEnergyData(pn) ;
            if(NULL==data || data[0].nNum != 4 || data[1].nNum != 4)
            {
                PDS_FMT_DEBUG("son pn %d frozen data abnormal, lstnum %d lstlstnum %d\n",pn, data[0].nNum, data[1].nNum);
                continue;
            }
            
            EnergyDataAdd(&sondata[0],data);
            EnergyDataAdd(&sondata[1],data+1);
        }

        CalcLineLossData(glineLossArg, Breakinfo->Pn, Breakinfo->Addr, parentdata, sondata);        

        //如果当前断路器为表箱断路器 则其所有儿子为叶子
        if(Breakinfo->usertype == USERTYPE_METERBOX_BREAKER)
        {
            EnergyDataAdd(&allleafdata[0],&sondata[0]);
            EnergyDataAdd(&allleafdata[1],&sondata[1]);
        }

        //若当前断路器为叶子，则将自身数据加到叶子数据中
        if(Breakinfo->SonNum == 0)
        {
            EnergyDataAdd(&allleafdata[0],&parentdata[0]);
            EnergyDataAdd(&allleafdata[1],&parentdata[1]);
            PDS_FMT_DEBUG("该节点 Pn %d 无儿子将父节点值赋给自己 上次累计正向有功累计和  %d 上上次累计正向有功和 %d\n",Breakinfo->Pn,allleafdata[0].nValue[0],allleafdata[1].nValue[0]);
        }
    }
    #endif
    #ifdef AREA_JIANGXI
    memset(&sondata,0x00,sizeof(PDS_ENERGY4V_T)*2);
    for(i=1; i<MeterFileAll.MeterCount; i++)
    {
        pn = MeterFileAll.MeterFile[i].nIndex;
        if(pn==0)
        {
            PDS_FMT_DEBUG("未找到档案序号 \n");
            continue;
        }
        data = GetEnergyData(pn) ;
        if(NULL==data || data[0].nNum != 4 || data[1].nNum != 4)
        {
            PDS_FMT_DEBUG("son pn %d frozen data abnormal, lstnum %d lstlstnum %d\n",pn, data[0].nNum, data[1].nNum);
            continue;
        }
        PDS_FMT_DEBUG("未找到档案序号 pn is %d\n",pn);
        EnergyDataAdd(&sondata[0],data);
        PDS_FMT_DEBUG("电量1 data->rsv is %d,%d, %d, %d,%d\n",data->rsv,data->nValue[0],data->nValue[1],data->nValue[2],data->nValue[3]);
        EnergyDataAdd(&sondata[1],data+1);
        
    PDS_FMT_DEBUG("电量2 data->rsv is %d,%d, %d, %d,%d\n",(data+1)->rsv,(data+1)->nValue[0],(data+1)->nValue[1],(data+1)->nValue[2],(data+1)->nValue[3]);
    }
    #endif
    //计算台区总线损
    pn = GetMasterBreak(Addr);
    if(pn==0)
    {
        PDS_FMT_DEBUG("未找到智能总开关,获取交采电量数据\n");
        memcpy(Addr, LocalAddr, 6);
        pds_ac_energy_get(glineLossArg, parentdata);
        if(parentdata[0].nNum != 4 || parentdata[1].nNum != 4)
        {
            PDS_FMT_DEBUG("ac frozen data abnormal, lstnum %d lstlstnum %d\n", parentdata[0].nNum, parentdata[1].nNum);
            glineLossArg->isNeedCalc =0;
            return 0;
        }
    }
    else
    {
        PDS_FMT_DEBUG("找到智能总开关pn %d\n",pn);
        data = GetEnergyData(pn) ;
        if(NULL==data || data[0].nNum != 4 || data[1].nNum != 4)
        {
            PDS_FMT_DEBUG("未获取到智能总开关冻结数据, lstnum %d lstlstnum %d\n", pn, data[0].nNum, data[1].nNum);
            glineLossArg->isNeedCalc =0;
            return 0;
        }

        memcpy(&parentdata,data,sizeof(parentdata));
    }

    //计算线损数据
    CalcLineLossData(glineLossArg, pn, Addr, parentdata, allleafdata);
        
    glineLossArg->isNeedCalc =0;
    
    return 0;
}

#endif

#if DESC("阻抗相关代码",1)

/**********************************************************************
* @name      : GetMeterEnergyData
* @brief     ：根据信息点号和存储时间 获取电压和电流分钟冻结数据
* @param[in] ：pn 信息点号
            addr 地址
            storetime 存储时间
* @param[out] vol 电压
            cur 电流
* @return    ：0-成功/错误码

**********************************************************************/
int GetMeterVolAndCur(uint16 pn,uint8 *addr,uint32 storetime,OOP_WORD3V_T * vol,OOP_INT3V_T *cur)
{
    int ret;
    uint8 i;
    RECORD_UNIT_T RecordOut;
    READ_RECORD_T startReadRecord;
    OOP_DATETIME_S_T timetmp;

    DT_Time2DateTimeShort(storetime, &timetmp);

    memset((uint8*)&startReadRecord,0x00,sizeof(READ_RECORD_T));
    memset((uint8*)&RecordOut,0x00,sizeof(RECORD_UNIT_T));
    startReadRecord.recordOAD.road.oadMain.value = 0x50020200;
    startReadRecord.recordOAD.road.oadCols.nNum = 2;
    startReadRecord.recordOAD.road.oadCols.oad[0].value =0x20000200;
    startReadRecord.recordOAD.road.oadCols.oad[1].value =0x20010200;
    startReadRecord.recordOAD.logicId = 0;
    startReadRecord.recordOAD.infoNum = pn;
    startReadRecord.recordOAD.classtag =5;
    startReadRecord.MAC.nNum=6;
    memcpy_r(startReadRecord.MAC.value,addr,6);
    startReadRecord.cType = COL_TM_STORE;
    startReadRecord.cStart = storetime+READ_DATA_TIME_ADD;
    startReadRecord.cEnd = storetime+READ_DATA_TIME_ADD;
   
    ret = db_read_record(gCalcArg.clientid ,&startReadRecord, DATA_LEN_MAX,&RecordOut);
    if((ret!=0) || (RecordOut.recordnum == 0))
    {
        PDS_FMT_DEBUG("pn %d %012llx 电压电流数据失败 ret %d, recordnum %d, oadMain 0x%08x, store %04d-%02d-%02d %02d:%02d:%02d\n", pn, *(uint64*)addr&0xffffffffffff, ret,RecordOut.recordnum,
            startReadRecord.recordOAD.road.oadMain, timetmp.year, timetmp.month, timetmp.day,timetmp.hour, timetmp.minute, timetmp.second);

        return ret;
    }
    
    if(RecordOut.recordnum>2)
    {
        PDS_FMT_DEBUG("pn %d %012llx 电压电流记录条数有问题 recordnum %d > 2 \n",pn, *(uint64*)addr&0xffffffffffff, RecordOut.recordnum);
        return ret;
    }
    for(i=0;i<RecordOut.recordnum;i++)
    {
        if(RecordOut.record[i].road.oadCols.oad[0].value==0x20000200 && RecordOut.record[i].outDatalen==sizeof(OOP_WORD3V_T))
        {   
            memcpy(vol,RecordOut.record[i].outDataBuf,sizeof(OOP_WORD3V_T));
        }else if(RecordOut.record[i].road.oadCols.oad[0].value==0x20010200 && RecordOut.record[i].outDatalen==sizeof(OOP_INT3V_T))
        {
            memcpy(cur,RecordOut.record[i].outDataBuf,sizeof(OOP_INT3V_T));
        }
    }
   
    return 0;
}

/**********************************************************************
* @name      : GetMeterEnergyData
* @brief     ：根据信息点号和存储时间 获取阻抗值
* @param[in] ：pn 信息点号
            addr 地址
            storetime 存储时间
* @param[out]：data 电量数据
* @return    ：0-成功/错误码

**********************************************************************/
int GetMeterImpedanceData(uint16 pn,uint8 *addr,uint32 storetime,OOP_DWORD3V_T * data)
{
    int ret;
    RECORD_UNIT_T RecordOut;
    READ_RECORD_T startReadRecord;

    memset((uint8*)&startReadRecord,0x00,sizeof(READ_RECORD_T));
    memset((uint8*)&RecordOut,0x00,sizeof(RECORD_UNIT_T));
    startReadRecord.recordOAD.road.oadMain.value = 0x50020200;
    startReadRecord.recordOAD.road.oadCols.nNum = 1;
    startReadRecord.recordOAD.road.oadCols.oad[0].value =0x20A10200;
    startReadRecord.recordOAD.logicId = 0;
    startReadRecord.recordOAD.infoNum = pn;
    startReadRecord.recordOAD.classtag =5;
    startReadRecord.MAC.nNum=6;
    memcpy_r(startReadRecord.MAC.value,addr,6);
    startReadRecord.cType = COL_TM_STORE;
    startReadRecord.cStart = storetime+READ_DATA_TIME_ADD;
    startReadRecord.cEnd = storetime+READ_DATA_TIME_ADD;
    ret = db_read_record(gCalcArg.clientid ,&startReadRecord, DATA_LEN_MAX,&RecordOut);
    if(ret!=0 ||RecordOut.record[0].outDatalen!=sizeof(OOP_DWORD3V_T))
    {
        PDS_FMT_DEBUG("获取pn %d 阻抗数据失败 ret %d  \n",pn,ret);
        return ret;
    }
    memcpy(data,RecordOut.record[0].outDataBuf,sizeof(OOP_DWORD3V_T));
    return 0;
}
/**********************************************************************
* @name      : GetAllVolAndCur
* @brief     ：加载所有档案的冻结电压电流数据
* @param[in] ：

* @param[out]AllMeterData 数据
* @return    ：0-成功/错误码

**********************************************************************/
void GetAllVolAndCur(All_Vol_Cur_t *AllMeterData)
{
    uint16 i;
    Line_Impedance_Calc_T *gLineImpedanceArg = &gCalcArg.lineImpedanceArg;
    Meter_Vol_Cur_T *data = NULL;
    pthread_rwlock_rdlock(&MeterFileAll.rwLock);
    for(i=0;i<MeterFileAll.MeterCount;i++)
    {
        AllMeterData->data[i].Pn = MeterFileAll.MeterFile[i].nIndex;
        memcpy_r(AllMeterData->data[i].Addr,MeterFileAll.MeterFile[i].basic.tsa.add,6);
        AllMeterData->data[i].Pt = MeterFileAll.MeterFile[i].extend.PT;
        AllMeterData->data[i].Ct = MeterFileAll.MeterFile[i].extend.CT;
    }
    AllMeterData->count = MeterFileAll.MeterCount;
    pthread_rwlock_unlock(&MeterFileAll.rwLock);
    for(i=0;i<AllMeterData->count;i++)
    {
        data = &AllMeterData->data[i];
        GetMeterVolAndCur(data->Pn,data->Addr,gLineImpedanceArg->storeTime[0],&data->vol,&data->cur);
        GetMeterVolAndCur(data->Pn,data->Addr,gLineImpedanceArg->storeTime[1],&data->lastVol,&data->lastCur);
        GetMeterImpedanceData(data->Pn,data->Addr,gLineImpedanceArg->storeTime[1],&data->lastImpedance);
    }
}

int pds_ac_volandcur_get(Line_Impedance_Calc_T *gLineImpedanceArg, Meter_Vol_Cur_T *ACdata)
{
    int ret = 0;

    ret = GetMeterVolAndCur(0, LocalAddr, gLineImpedanceArg->storeTime[0], &ACdata->vol, &ACdata->cur);
    ret |= GetMeterVolAndCur(0, LocalAddr, gLineImpedanceArg->storeTime[1], &ACdata->lastVol, &ACdata->lastCur);

    return ret;
}

 Meter_Vol_Cur_T * GetVolAndCurIndex(uint16 pn)
{
    uint16 i;

    if(pn==0)
    {
        return NULL;
    }

    for(i=0;i<AllVolCur.count;i++)
    {
        if(pn ==AllVolCur.data[i].Pn)
        {
            return &AllVolCur.data[i];
        }
    }
    return NULL;

}
 OOP_NODEPHASEINFO_T gettmpphase(uint16 Pn)
 {
//    uint8 outdata=0;
    uint32 len;
    uint8 ret;
    OOP_NODEPHASEINFO_T PnPhase;
    NOMAL_OAD_T NormalOad ;
    NormalOad.logicId = 0;
    NormalOad.infoNum = Pn;
    NormalOad.oad.value = 0xF2090A00;
    memset(&PnPhase,0x0,sizeof(OOP_NODEPHASEINFO_T));
    ret=db_read_nomal(gCalcArg.clientid,&NormalOad,sizeof(OOP_NODEPHASEINFO_T),(uint8*)&PnPhase,&len);
    if(ret<0)
    {
      PDS_FMT_DEBUG("Pn %d 读相位信息出错,按照默认解析 \n", Pn);
      return PnPhase;
    }
    return PnPhase;
 }
 
 /*******************************************************************************
 * 函数名称: taskmng_usertype_get_from_no
 * 函数功能: 根据序号获取电表用户类型
 * 输入参数: Pn             要检测的测量点
 
 * 输出参数: 
   
 * 返 回 值:  电表地址
 *******************************************************************************/
 uint8 taskmng_usertype_get_from_no(uint16 Pn,uint8 logicid)
 {
     uint8 usertype = 0;
     uint16 i;
     pthread_rwlock_rdlock(&MeterFileAll.rwLock);
     for(i=0; i<MeterFileAll.MeterCount; i++)
     {
         if(Pn == MeterFileAll.MeterFile[i].nIndex)
         {
             usertype = MeterFileAll.MeterFile[i].basic.userType;
             break;
         }
     }
     pthread_rwlock_unlock(&MeterFileAll.rwLock);
     return usertype;
 }
 
 /**********************************************************************
 * @name      : LineImpedanceData
 * @brief     ：感觉父子节点数据计算线路阻抗 （父电压-子电压）/父电流
 * @param[in] ：
 
 * @param[out]：
 * @return    ：
 
 **********************************************************************/
 void LineImpedanceData(uint16 pn,uint8 *addr,Meter_Vol_Cur_T *parentdata,Meter_Vol_Cur_T *sondata,uint32 limit)
 {
     uint8 i;
     uint8 j;
     uint8 eventflag=0;    //1发生 2恢复
     OOP_DWORD3V_T lineImpeData;
     memset(&lineImpeData,0x00,sizeof(OOP_DWORD3V_T));
     OOP_IMPE_OVER_T  event;
     WRITE_RECORD_T  indata;
     uint32 storeNo;
     time_t timenow;
     int ret;
     uint8 usertype;
     OOP_NODEPHASEINFO_T Pn_Phase;
     lineImpeData.nNum = 3;
     uint8 inValidNum = 0;
     //临时添加，计算节点的相位
     //读f2090a00
     usertype=taskmng_usertype_get_from_no(pn,0);
     
//   if(sizeof(Pn_Phase.info)==0)       //读节点信息失败，直接返回
//      return;
     Pn_Phase = gettmpphase(pn);  //读节点相位信息
     for(i=0;i<3;i++)    //忽略单相表和三相表都检查相位防止三相表线不全接
     {
        if(usertype==USERTYPE_LOW_BREAKER||usertype==USERTYPE_METERBOX_BREAKER)     //断路器不能按照分相计算
            {
                j=i;
            }
         else
            {
                if((Pn_Phase.info&(0x01<<i))==0)
                    continue;
                if(usertype==LOT_METER_TPIND)      //三相表
                    j=i;
                else
                    j=0;                //单相表只有A相值，所以固定为0
            }

         if(parentdata->vol.nValue[i]==0||sondata->cur.nValue[j]==0||sondata->vol.nValue[j]==0||
           parentdata->vol.nValue[i]==0xFFFF||sondata->cur.nValue[j]==0XFFFFFFFF||sondata->vol.nValue[j]==0XFFFF)
         {
             inValidNum++;
             PDS_FMT_DEBUG("Pn %d LineImpedanceData %d 分项无数据 无法计算 父代电压%d 子代电流%d 子代电压%d \n", pn, i,parentdata->vol.nValue[i],sondata->cur.nValue[j],sondata->vol.nValue[j]);
             continue;
         }

         //阻抗单位是欧姆，换算是-3,电压单位是伏，换算是-1，电流单位是安，换算是-3,计算时,电压除以电流的结果再乘以1000，所以直接拿采集数据来算就要乘以100000
         lineImpeData.nValue[i] = abs(parentdata->vol.nValue[i]*parentdata->Pt - sondata->vol.nValue[j]*sondata->Pt)*100000/(sondata->cur.nValue[j]*sondata->Ct);
         PDS_FMT_DEBUG("Pn %d LineImpedanceData %d 分项阻抗计算结果为 %d limit %d \n", pn, i,lineImpeData.nValue[i],limit);
         set_bit_value((uint8*)&lineImpeData.rsv, 32, i);
         
         //判断是否产生线路阻抗越限事件
         if(sondata->lastImpedance.nValue[j]<=limit && lineImpeData.nValue[i] >limit)
         {
             //线路阻抗越限事件发生
             PDS_FMT_DEBUG("线路阻抗越限事件发生\n");
             eventflag = 1;
         }
         if(sondata->lastImpedance.nValue[j]>limit && lineImpeData.nValue[i]<=limit)
         {
             //线路阻抗越限事件恢复
             PDS_FMT_DEBUG("线路阻抗越限事件恢复\n");
             eventflag = 2;
         }
         if(eventflag!=0)
         {
             timenow = time(NULL);
             memset(&event,0x00,sizeof(OOP_IMPE_OVER_T));
             event.nIndex =gImpeEventIndex++;
             event.source.choice = DT_TSA;
             event.source.add.len = 5;
             memcpy_r(event.source.add.add,addr,6);
//             event.impeBef = sondata->lastImpedance.nValue[i];
//             event.impeAft = lineImpeData.nValue[i];
             memcpy(&event.impeBef,&sondata->lastImpedance,sizeof(event.impeBef));
             memcpy(&event.impeAft, &lineImpeData, sizeof(event.impeAft));             
             event.curPhase = i+1; //相位 1 A 2 B 3 C
             //OAD的“属性特征”取值如下：
              //   1：事件发生前；
              //   2：事件发生后；
              //   3：事件结束前；
              //   4：事件结束后。
            event.data.oadNum = 4;
            event.data.cols.nNum = 4;
            event.data.cols.oad[0].value = 0x20002200; //事件发生前电压
            event.data.cols.oad[1].value = 0x20004200; //事件发生后电压
            event.data.cols.oad[2].value = 0x20012200; //事件发生前电流
            event.data.cols.oad[3].value = 0x20014200; //事件发生后电流
            if(eventflag == 1)
            {
                DT_Time2DateTimeShort(timenow,&event.tmStart);
//                event.data.oadNum = 4;
//                event.data.cols.nNum = 4;
//                event.data.cols.oad[0].value = 0x20002200+i+1; //事件发生前电压
//                event.data.cols.oad[1].value = 0x20004200+i+1; //事件发生后电压
//                event.data.cols.oad[2].value = 0x20012200+i+1; //事件发生前电流
//                event.data.cols.oad[3].value = 0x20014200+i+1; //事件发生后电流                 
                 
            }else
            {
                DT_Time2DateTimeShort(timenow,&event.tmStart);
                DT_Time2DateTimeShort(timenow,&event.tmEnd);
//                 event.data.oadNum = 4;
//                 event.data.cols.nNum = 4;
//                 event.data.cols.oad[0].value = 0x20006200+i+1; //事件结束前电压
//                 event.data.cols.oad[1].value = 0x20008200+i+1; //事件结束后电压
//                 event.data.cols.oad[2].value = 0x20016200+i+1; //事件结束前电流
//                 event.data.cols.oad[3].value = 0x20018200+i+1; //事件结束后电流
            }
//             event.data.idLen[0] = sizeof(uint16);
//             event.data.idLen[1] = sizeof(uint16);
//             event.data.idLen[2] = sizeof(int32);
//             event.data.idLen[3] = sizeof(int32);
//             memcpy(event.data.buf+event.data.len,&sondata->lastVol.nValue[i],sizeof(uint16));
             event.data.idLen[0] = sizeof(OOP_WORD3V_T);
             event.data.idLen[1] = sizeof(OOP_WORD3V_T);
             event.data.idLen[2] = sizeof(OOP_INT3V_T);
             event.data.idLen[3] = sizeof(OOP_INT3V_T);
             memcpy(event.data.buf+event.data.len,&sondata->lastVol,sizeof(OOP_WORD3V_T));
             event.data.len+=event.data.idLen[0];
             memcpy(event.data.buf+event.data.len,&sondata->vol,sizeof(OOP_WORD3V_T));
             event.data.len+=event.data.idLen[1];
             memcpy(event.data.buf+event.data.len,&sondata->lastCur,sizeof(OOP_INT3V_T));
             event.data.len+=event.data.idLen[2];
             memcpy(event.data.buf+event.data.len,&sondata->cur,sizeof(OOP_INT3V_T));
             event.data.len+=event.data.idLen[3];
             //写事件记录
             memset(&indata,0x00,sizeof(WRITE_RECORD_T));
             indata.recordOAD.road.oadCols.nNum = 1;
             indata.recordOAD.road.oadCols.oad[0].value = 0x31F20200;
             indata.recordOAD.classtag = CLASS_DATA_EVENT;
             indata.MAC.nNum = 6;
             memcpy_r(indata.MAC.value,LocalAddr,6);
             indata.colStartTm = timenow+READ_DATA_TIME_ADD;
             indata.colEndTm = timenow +READ_DATA_TIME_ADD;
             indata.colStoreTm = event.nIndex;   //使用事件序号作为存储时间
             indata.inDatalen = sizeof(OOP_IMPE_OVER_T);
             memcpy(indata.inDataBuf,&event,sizeof(OOP_IMPE_OVER_T));
             ret = db_write_record(gCalcArg.clientid, &indata, &storeNo);
             if(ret!=0)
             {
                 PDS_FMT_DEBUG("线路阻抗越限事件写入失败  \n");
             }
         }
     }
     
     if(inValidNum < 3)
     {
         //写阻抗数据到数据中心
         memset(&indata,0x00,sizeof(WRITE_RECORD_T));
         indata.recordOAD.road.oadMain.value = 0x50020200;
         indata.recordOAD.road.oadCols.nNum = 1;
         indata.recordOAD.road.oadCols.oad[0].value = 0x20A10200;;
         indata.recordOAD.classtag = CLASS_DATA_COLLEC;
         indata.recordOAD.infoNum = pn;
         indata.MAC.nNum = 6;
         memcpy_r(indata.MAC.value,addr,6);
         indata.colStartTm = gCalcArg.lineImpedanceArg.storeTime[0]+READ_DATA_TIME_ADD;
         indata.colEndTm = gCalcArg.lineImpedanceArg.storeTime[0]+READ_DATA_TIME_ADD;
         indata.colStoreTm = gCalcArg.lineImpedanceArg.storeTime[0]+READ_DATA_TIME_ADD; 
         indata.inDatalen = sizeof(OOP_DWORD3V_T);
         memcpy(indata.inDataBuf,&lineImpeData,sizeof(OOP_DWORD3V_T));
         ret = db_write_record(gCalcArg.clientid, &indata, &storeNo);
         if(ret!=0)
         {
                 PDS_FMT_DEBUG("线路阻抗数据写入失败 Pn %d \n", pn);
         }
     }
 }
 
 /**********************************************************************
 * @name      : LineImpedanceCalc
 * @brief     ：线路阻抗计算
 * @param[in] ：
 
 * @param[out]：
 * @return    ：
 
 **********************************************************************/
 void LineImpedanceCalc()
 {
     Line_Impedance_Calc_T *gLineImpedanceArg = &gCalcArg.lineImpedanceArg;
     Topology_Arg_T *gTopologyArg = &gCalcArg.topologyArg;
     Break_Info_T *Breakinfo = NULL;
     uint32 limit=0;
     Meter_Vol_Cur_T *parentdata=NULL;
     Meter_Vol_Cur_T *sondata=NULL;
     Meter_Vol_Cur_T ACdata;
     memset(&ACdata,0X00,sizeof(Meter_Vol_Cur_T));
     uint16 i,j;
     uint16 pn;
     uint8 Addr[6]={0};
     if(gTopologyArg->isFinish == 0)
     {
         PDS_FMT_DEBUG("当前没有识别到拓扑结构 无法进行线路阻抗计算 \n");
         gLineImpedanceArg->isNeedCalc = 0;
         return ;
     }
     pthread_mutex_lock(&gImpeLimit.lock);
     limit = gImpeLimit.limit ;
     pthread_mutex_unlock(&gImpeLimit.lock);
     if(limit==0)
     {
         PDS_FMT_DEBUG("当前没有设置阻抗限值 无法进行线路阻抗计算 \n");
         gLineImpedanceArg->isNeedCalc = 0;
         return ;
     }
 
     memset(&AllVolCur,0x00,sizeof(All_Vol_Cur_t));
     GetAllVolAndCur(&AllVolCur);
     if(GetMeterVolAndCur(0,LocalAddr,gLineImpedanceArg->storeTime[0],&ACdata.vol,&ACdata.cur)!=0)
     {
         PDS_FMT_DEBUG("获取终端交采冻结电压电流数据失败 \n");
         pn = GetMasterBreak(Addr);
         parentdata = GetVolAndCurIndex(pn);
         if(NULL == parentdata)
         {
             PDS_FMT_DEBUG("Pn %d 未获取到总表数据 \n",pn);
         }else
         {
            memcpy(&ACdata,parentdata,sizeof(Meter_Vol_Cur_T));
         }
     }
     else
     {
         pds_ac_ptct_get(&ACdata.Pt, &ACdata.Ct);
     }
 
     for(i=0;i<gTopologyArg->breakCount;i++)
     {
         Breakinfo = &gTopologyArg->breakInfo[i];
         parentdata = GetVolAndCurIndex(Breakinfo->Pn); //父亲的数据
         if(NULL==parentdata)
         {
             PDS_FMT_DEBUG("Pn %d 未获取到数据索引 \n",Breakinfo->Pn);
             continue;
         }
         for(j=0;j<Breakinfo->SonNum;j++) //儿子们的数据
         {
             pn = GetPnFromAddr(Breakinfo->breakSonInfo[j].addr);
             if(pn==0)
             {
                 PDS_BUF_DEBUG(Breakinfo->breakSonInfo[j].addr,6,"未找到档案序号 \n");
                 continue;
             }
             sondata = GetVolAndCurIndex(pn);
             if(NULL==sondata)
             {
                 PDS_FMT_DEBUG("子节点 Pn %d 未获取到数据索引 \n",Breakinfo->Pn);
                 continue;
             }
             LineImpedanceData(pn,Breakinfo->breakSonInfo[j].addr,parentdata,sondata,limit);
         }
         //如果是一级分支 则和能源控制器计算阻抗
         if(Breakinfo->nodeType == NODE_ROOT)
         {
             LineImpedanceData(Breakinfo->Pn,Breakinfo->Addr,&ACdata,parentdata,limit);
         }
     }
     gLineImpedanceArg->isNeedCalc = 0;
 }
#endif

 /*******************************************************************************
* 函数名称: GetPnFromAddr
* 函数功能: 根据电表地址获取PN
* 输入参数: Addr   电表地址

* 输出参数: 
  
* 返 回 值: 电表测量点号
*******************************************************************************/
uint16 GetPnFromAddr(uint8* Addr)
{
    uint16 i;
    uint16 Pn = 0;
    uint8 tmpadd[6];
    memcpy_r(tmpadd,Addr,6);
    pthread_rwlock_rdlock(&MeterFileAll.rwLock);
    for(i=0;i<MeterFileAll.MeterCount;i++)
    {
        if(memcmp(tmpadd,MeterFileAll.MeterFile[i].basic.tsa.add,6)==0 )
        {
            Pn = MeterFileAll.MeterFile[i].nIndex;
            break;
        }
    }
    pthread_rwlock_unlock(&MeterFileAll.rwLock);
    return Pn;
}

 /*******************************************************************************
* 函数名称: GetMasterBreak
* 函数功能: 根据用户类型获取总表序号
* 输入参数: 

* 输出参数: 
  
* 返 回 值: 电表测量点号
*******************************************************************************/
uint16 GetMasterBreak(uint8 *Addr)
{
    uint16 i;
    uint16 Pn = 0;

    for(i=0;i<MeterFileAll.MeterCount;i++)
    {
        if(MeterFileAll.MeterFile[i].basic.userType==mastermeter )
        {
            Pn = MeterFileAll.MeterFile[i].nIndex;
            memcpy_r(Addr,MeterFileAll.MeterFile[i].basic.tsa.add,6);
            break;
        }
    }
    pthread_rwlock_unlock(&MeterFileAll.rwLock);
    return Pn;
}

/*******************************************************************************
* 函数名称: GetImpedanceLimit 
* 函数功能: //获取阻抗限值 31F20600
* 输入参数: 
* 输出参数: 
* 返 回 值:  0成功 -1 失败
*******************************************************************************/
int GetImpedanceLimit(DB_CLIENT clientid)
{
    int   ret;
    uint32 len=0;
    NOMAL_OAD_T NormalOad;
    NormalOad.logicId = 0;
    NormalOad.oad.value = 0x31F20600;
    NormalOad.infoNum = 0;

    OOP_EVTCFG_31F2_T limit;
    memset(&limit, 0, sizeof(OOP_EVTCFG_31F2_T));
    ret = db_read_nomal(clientid,&NormalOad,sizeof(OOP_EVTCFG_31F2_T),(uint8*)&limit,&len);
    if(ret !=0 ||len!=sizeof(OOP_EVTCFG_31F2_T))
    {
        PDS_FMT_DEBUG("获取阻抗超限值失败 设置默认阻抗越限值1.5欧姆\n");
        gImpeLimit.limit = 1500;
        return ret;
    }
    PDS_FMT_DEBUG("获取阻抗超限值 %d  \n",limit.impeLimit);
    pthread_mutex_lock(&gImpeLimit.lock);
    gImpeLimit.limit = limit.impeLimit;
    pthread_mutex_unlock(&gImpeLimit.lock);
    return 0;
}

/**********************************************************************
* @name      : ReloadLocalAddr
* @brief     ：获取本地地址
* @param[in] ：int clientid    socket句柄
               uint8 logicId   逻辑地址
               uint16 infoNum  信息点号
               uint32 value    oad值
* @param[out]：
* @return    ：0-成功/错误码

**********************************************************************/
int ReloadImpedenceLimit(int clientid, uint8 logicId, uint16 infoNum, uint32 value)
{
    if(0x31F20600 != value)
    {
        return -1;
    }
    GetImpedanceLimit(clientid);
    return 0;
}


/*******************************************************************************
* 函数名称: GetEventIndex 
* 函数功能: 从数据中心获取事件当前最大序号
* 输入参数: 
* 输出参数: 
* 返 回 值:  0成功 -1 失败
*******************************************************************************/
uint32 GetEventIndex(DB_CLIENT clientid,uint32 oad)
{
    int ret;

    OOP_EVENT_T *pTemp = NULL;
    RECORD_UNIT_T RecordOut;
    READ_RECORD_T startReadRecord;

    memset((uint8*)&startReadRecord,0x00,sizeof(READ_RECORD_T));
    memset((uint8*)&RecordOut,0x00,sizeof(RECORD_UNIT_T));
    startReadRecord.recordOAD.road.oadMain.value = 0;
    startReadRecord.recordOAD.road.oadCols.nNum = 1;
    startReadRecord.recordOAD.road.oadCols.oad[0].value =oad;
    startReadRecord.recordOAD.logicId = 0;
    startReadRecord.recordOAD.infoNum = 0;
    startReadRecord.recordOAD.classtag =5;
    startReadRecord.MAC.nNum=6;
    memcpy_r(startReadRecord.MAC.value,LocalAddr,6);
    startReadRecord.cType = COL_TM_STORE;
    startReadRecord.cStart = 0;
    startReadRecord.cEnd = 0xffffffff;
    startReadRecord.sortType = DOWN_SORT;
    
    ret = db_read_record(gCalcArg.clientid ,&startReadRecord, DATA_LEN_MAX,&RecordOut);
    if(ret!=0 )
    {
        PDS_FMT_DEBUG("获取事件oad 0x%08x 数据失败 \n",oad);
        return 0;
    }
    pTemp = (OOP_EVENT_T*)RecordOut.record[0].outDataBuf;
    return pTemp->nIndex;
}

