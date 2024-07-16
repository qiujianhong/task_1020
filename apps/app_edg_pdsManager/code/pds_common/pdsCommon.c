
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
OOP_DATETIME_S_T             gDaytaskEndTime;   //��¼���ٵ��������ʱ�� ������˾�����һ��
OOP_DATETIME_S_T             gMonthtaskEndTime;   //��¼���ٵ��������ʱ�� ������˾�����һ��
OOP_DATETIME_S_T             gMintaskEndTime;   //��¼���ٵ��������ʱ�� ������˾�����һ��


PastTimeFlag_T             gPastTimeFlag;                           //��ʱ���־
DateTimeHex_t              gCurBinTime;                             //��ǰBINʱ��
DateTimeHex_t              gPreBinTime;                             //�ϴ�BINʱ��

LockQueue_T gTransQueue;            //����յ���taskManager͸����Ϣ���뱨��
Line_Loss_Period_T  gLineLossPeriod;
All_Meter_Data_T AllMeterData;
All_Vol_Cur_t  AllVolCur;
Line_Impedance_Limit_T gImpeLimit;       //�迹��ֵ
uint32 gImpeEventIndex;
extern uint8 LocalAddr[6];
extern int gMqttSock ;
extern int gCalcSock ;
extern char PrivateDir[100];
uint32 g_sendchecktime = 0; //���Ͳ�ѯʱ�䱨�ĵ�ʱ�� 

/* ����Ļ��� */
OOP_TASK_LIST_T oopTaskList;

//��ͨ�ɼ������Ļ���
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
* ��������: taskmng_tsa_get
* ��������: ������Ż�ȡ����ַ
* �������: Pn             Ҫ���Ĳ�����

* �������: 
  
* �� �� ֵ:  ����ַ
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
				if(linevalue&0x0010)//�������������
				{
					break;
				}
				else if(linevalue&0x0001)//���ܱ���ڵ�һ��λ�ã�����д�����ã������Ż�
				{
					 LineMeter.MeterFile[0].Pn = MeterFileAll.MeterFile[i].nIndex;
					 LineMeter.MeterFile[0].lineword = linevalue;
					 PDSDB_FMT_DEBUG("����������ܱ������� infonum %d ",LineMeter.MeterFile[0].Pn);
				}
				else
				{
					LineMeter.MeterFile[LineMeter.MeterCount].Pn = MeterFileAll.MeterFile[i].nIndex;
					LineMeter.MeterFile[LineMeter.MeterCount].lineword = linevalue;
					PDSDB_FMT_DEBUG("��������Ļ���������� infonum %d ",LineMeter.MeterFile[LineMeter.MeterCount].Pn);
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
    uint16   MeterNum=0;     //��������
    int   ret;
    uint16 i;
    OOP_METER_T gMeterFile;     //ÿ�����ĵ�����Ϣ
    READ_RECORD_T      inReadRecord;
    uint32             handle = 0;
    uint32             recNum = 0;
    STEP_READ_RECORD_T stepReadRecord;
    RECORD_UNIT_T      stepRecordOut;
    memset(&inReadRecord, 0, sizeof(READ_RECORD_T));
    memset(&stepReadRecord, 0, sizeof(STEP_READ_RECORD_T));
    memset(&stepRecordOut, 0, sizeof(RECORD_UNIT_T));
    PDS_FMT_DEBUG("��ʼ������ʼ��!  \n");
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
        PDS_FMT_DEBUG("��ʼ��������¼ʧ�� ret  %d recNum %d \n",ret,recNum);
    }
    else
    {
        for(i=0;i<recNum;i++)
        {
            ret = db_read_record_step(clientid, handle, &stepReadRecord, &stepRecordOut);
            if(ret!=0)
            {
                PDS_FMT_DEBUG("������������¼ʧ�� ret  %d i %d \n",ret,i);
                continue;
            }
            if(stepRecordOut.recordnum!=1 || stepRecordOut.record[0].road.oadCols.oad[0].value!=0x60000200
                ||stepRecordOut.record[0].outDatalen!=sizeof(OOP_METER_T))
            {
                PDS_FMT_DEBUG( "������ȡ�ĵ�����¼������ \n");
                continue;
            }
            memcpy(&gMeterFile,stepRecordOut.record[0].outDataBuf,sizeof(OOP_METER_T));

            PDS_FMT_DEBUG("sizeof(OOP_METER_T) %d ������ %d ret = %d !\n",sizeof(OOP_METER_T),gMeterFile.nIndex,ret);
            PDS_FMT_DEBUG("port =%08x prtl = %d pwrType =%d \n",gMeterFile.basic.port.value,gMeterFile.basic.protocol,gMeterFile.basic.pwrType);
            PDS_FMT_DEBUG("��ַ 0X%02x%02x%02x%02x%02x%02x \n",gMeterFile.basic.tsa.add[0],gMeterFile.basic.tsa.add[1],gMeterFile.basic.tsa.add[2],gMeterFile.basic.tsa.add[3],gMeterFile.basic.tsa.add[4],gMeterFile.basic.tsa.add[5]);

            MeterNum=MeterFileAll.MeterCount;
            MeterFileAll.MeterFile[MeterNum]=gMeterFile;
            MeterFileAll.MeterCount++;
       }
    }
    db_read_record_end(clientid, handle);

    MeterCount = MeterFileAll.MeterCount;
    PDS_FMT_DEBUG("������ɺ󵵰�����Ϊ %d \n",MeterFileAll.MeterCount);
    pthread_rwlock_unlock(&MeterFileAll.rwLock);
	GetLineLossMeter();
    return MeterCount;

}


/*******************************************************************************
* ��������: MeterFileChange 
* ��������:��������Ĵ���
* �������: pMeterFileAll �����
          choice 1 ��ʾ������� �������߸ı�
          choice 0 ��ʾ����ɾ��
          gMeterFile��nindexΪɾ�������
* �������: 
* �� �� ֵ: 
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
            //�����ǰ�������ҵ���ͬ�� ��ֱ���滻
            if(pMeterFileAll->MeterFile[i].nIndex == gMeterFile.nIndex )
            {
                index = i;
                memcpy(&pMeterFileAll->MeterFile[i],&gMeterFile,sizeof(OOP_METER_T));
                break;
            }
        }
        //���û���ҵ� ������
        if(index==-1)
        {
            pMeterFileAll->MeterFile[pMeterFileAll->MeterCount] = gMeterFile;
            pMeterFileAll->MeterCount++;
        }
    }
    PDSDB_FMT_DEBUG( "��������  %d \n",pMeterFileAll->MeterCount);
    pthread_rwlock_unlock(&pMeterFileAll->rwLock);

}


/**
*********************************************************************
* @name��      appdb_task_list_record_init
* @brief��     ������������ʼ��(��¼���ݷ�ʽ)
* @param[in] ��clientid  ���ݿ���
* @param[out]��
* @return    ��
* @author    : 
* @Date      ��2019-12-6
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

    //����Ҫ����У��
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

            //���������Ч��
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

            //��Ŵ�1��ʼ
            oopTaskList.isChanged[stepRecordOut.infoNum] = TRUE;
            oopTaskList.isVaild[stepRecordOut.infoNum] = TRUE;
            oopTaskList.oopTask[stepRecordOut.infoNum] = oopTask;
            
        }
    }
    db_read_record_end(clientid, handle);
    
    PDSDB_FMT_DEBUG("Initialize tasks end.\n");

    /* ��¼�������� */
    oopTaskList.oopTaskNum = oopTaskNum;

    PDSDB_FMT_DEBUG("read task num: %d\n", oopTaskList.oopTaskNum);

    return;
}



/**
*********************************************************************
* @name��      appdb_plan_normal_list_record_init
* @brief��     ��ͨ�ɼ��������������ʼ��(��¼���ݷ�ʽ)
* @param[in] ��clientid  ���ݿ���
* @param[out]��
* @return    ��
* @author    : 
* @Date      ��2019-12-6
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

    //����Ҫ����У��
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

            //���������Ч��
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

            //��Ŵ�1��ʼ
            oopPlanNormalList.isVaild[stepRecordOut.infoNum] = TRUE;
            oopPlanNormalList.oopPlanNormal[stepRecordOut.infoNum] = oopPlanNormal;
            
        }
    }
    db_read_record_end(clientid, handle);
    
    PDSDB_FMT_DEBUG("Initialize plan normals end.\n");

    /* ��¼���� */
    oopPlanNormalList.oopPlanNormalNum = oopPlanNormalNum;
    oopPlanNormalList.isChanged = TRUE;

    PDSDB_FMT_DEBUG("read plan normals num: %d\n", oopPlanNormalList.oopPlanNormalNum);

    return;
}


/**********************************************************************
* @name      : DBGetMeterFile
* @brief     ���յ����ݱ���¼����ȡ������Ϣ
* @param[in] ��int clientid    socket���
               uint8 logicId   �߼���ַ
               uint16 infoNum  ��Ϣ���
               uint32 value    oadֵ
* @param[out]��
* @return    ��0-�ɹ�/������
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
    PDSDB_FMT_DEBUG("�յ����������ݱ���¼� Logicid %d infonum %d \n",logicId,infoNum);
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
    /* ��ʼ���� */
     memset(&oopTaskList, 0, sizeof(OOP_TASK_LIST_T));
    pthread_mutex_init(&oopTaskList.mutex, NULL);
}

void plan_normal_list_lock_init(void)
{
    /* ��ʼ���� */
    memset(&oopPlanNormalList, 0, sizeof(OOP_PLAN_NORMAL_LIST_T));
    pthread_mutex_init(&oopPlanNormalList.mutex, NULL);
}


/**********************************************************************
* @name      : app_set_task_list
* @brief     �������µ���������д�뻺��
* @param[in] ��taskID  - �������          
               oopTask - ��������
* @return    ��������
**********************************************************************/
int app_set_task_list(uint8 taskID, OOP_TASK_T *oopTask)
{
    if(NULL == oopTask)
    {
        return ERR_PNULL;
    }

    /* ʹ�û��������м��� */
    pthread_mutex_lock(&oopTaskList.mutex);

    oopTaskList.oopTask[taskID] = *oopTask;
    oopTaskList.isVaild[taskID] = TRUE;
    oopTaskList.isChanged[taskID] = TRUE;
    if(oopTaskList.isVaild[taskID]==FALSE)
    {
        oopTaskList.oopTaskNum ++;
    }

    /* ʹ�û��������н��� */
    pthread_mutex_unlock(&oopTaskList.mutex);

    return ERR_OK;
}

/**********************************************************************
* @name      : app_set_task_invalid
* @brief     ����������������Ч
* @param[in] : taskID - �������
* @param[out]��
* @return    ��������
**********************************************************************/
int app_set_task_invalid(uint8 taskID)
{

    /* ʹ�û��������м��� */
    pthread_mutex_lock(&oopTaskList.mutex);

    memset(&oopTaskList.oopTask[taskID], 0, sizeof(OOP_TASK_T));
    oopTaskList.isVaild[taskID] = FALSE;
    oopTaskList.isChanged[taskID] = TRUE;
    oopTaskList.oopTaskNum --;

    /* ʹ�û��������н��� */
    pthread_mutex_unlock(&oopTaskList.mutex);

    return ERR_OK;
}


/**********************************************************************
* @name      : appdb_get_task_list
* @brief     �������б�����
* @param[in] ��int clientid    socket���
               uint8 logicId   �߼���ַ
               uint16 infoNum  ��Ϣ���
               uint32 value    oadֵ
* @param[out]��
* @return    ��0-�ɹ�/������
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
            //�жϵ����Ƿ���Ч
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
* @brief     ��������ͨ�ɼ�����������Ч
* @param[in] : planID - �ϱ��������
* @param[out]��
* @return    ��������
* @Create    : ÷����
* @Date      ��2020-2-27
* @Update    :
**********************************************************************/
int app_set_plan_normal_invalid(uint8 planID)
{

    /* ʹ�û��������м��� */
    pthread_mutex_lock(&oopPlanNormalList.mutex);

    memset(&oopPlanNormalList.oopPlanNormal[planID], 0, sizeof(OOP_PLAN_NORMAL_T));
    oopPlanNormalList.isVaild[planID] = FALSE;
    oopPlanNormalList.isChanged = TRUE;
    oopPlanNormalList.oopPlanNormalNum --;

    /* ʹ�û��������н��� */
    pthread_mutex_unlock(&oopPlanNormalList.mutex);

    return ERR_OK;
}


/**********************************************************************
* @name      : app_set_plan_normal_list
* @brief     �������µ���ͨ�ɼ���������д�뻺��
* @param[in] ��planID          - �������          
* @param[out]: oopPlanNormal  - ��������
* @return    ��������
* @Create    : ÷����
* @Date      ��2020-2-27
* @Update    :
**********************************************************************/
int app_set_plan_normal_list(uint8 planID, OOP_PLAN_NORMAL_T *oopPlanNormal)
{
    if(NULL == oopPlanNormal)
    {
        return ERR_PNULL;
    }

    /* ʹ�û��������м��� */
    pthread_mutex_lock(&oopPlanNormalList.mutex);

    oopPlanNormalList.oopPlanNormal[planID] = *oopPlanNormal;
    oopPlanNormalList.isVaild[planID] = TRUE;
    oopPlanNormalList.isChanged = TRUE;
    if(oopPlanNormalList.isVaild[planID]==FALSE)
    {
        oopPlanNormalList.oopPlanNormalNum ++;
    }

    /* ʹ�û��������н��� */
    pthread_mutex_unlock(&oopPlanNormalList.mutex);

    return ERR_OK;
}


/**********************************************************************
* @name      : appdb_get_plan_normal_list
* @brief     ����ͨ�ɼ������б�����
* @param[in] ��int clientid    socket���
               uint8 logicId   �߼���ַ
               uint16 infoNum  ��Ϣ���
               uint32 value    oadֵ
* @param[out]��
* @return    ��0-�ɹ�/������
* @Create    : ÷����
* @Date      ��2020-2-27
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
            //�ж��Ƿ���Ч
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
* @brief     �������������˵���Ϣ
* @param[in] ��MESSAGE_INFO_T *pMsg ��Ϣ�ṹ��
* @param[out]��
* @return    ��

**********************************************************************/
void topology_start(MESSAGE_INFO_T *pMsg)
{
    uint16 topotime;
    int ret;
    if(pMsg->msglen!=2)
    {
        MQTTMSG_FMT_TRACE("����������Ϣ msglen=%d ����  \n",pMsg->msglen);
        return;
    }
    memcpy_r(&topotime,pMsg->playload,pMsg->msglen);
    MQTTMSG_FMT_TRACE("����������Ϣ ����ʱ�� %d ��  \n",topotime);

    ret = task_send_msg(gMqttSock,MSG_PDS_TOPOSTART,(uint8*)&topotime,sizeof(uint16),PDS_CALC_IPC);
    if(ret!=0)
    {
        MQTTMSG_FMT_TRACE("task_send_msg to calc error ret %d \n",ret);
    }
    
}

/**********************************************************************
* @name      : topology_start
* @brief     �������������˵���Ϣ
* @param[in] ��MESSAGE_INFO_T *pMsg ��Ϣ�ṹ��
* @param[out]��
* @return    ��

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
        PDS_FMT_DEBUG("��ǰ��������ʶ������У�����Ӧ������������ \n");
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
        PDS_FMT_DEBUG("����PDS_CALC_IPCʧ�� ���� \n");
        return ;
    }
    task_msg_settime(gCalcSock,0,100000);
    DT_Time2DateTimeHex(time(NULL),&gCurBinTime);
    memcpy(&gPreBinTime,&gCurBinTime,sizeof(DateTimeHex_t));
    gCalcArg.clientid = db_client_create(DB_PDS_AGENT, DB_PDS_CLIENT_COMMON);
    if(gCalcArg.clientid <0)
    {
        PDS_FMT_DEBUG("DB_PDS_CLIENT_COMMON db_client_createʧ�� ���� \n");
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
        PDS_FMT_DEBUG("DB_PDS_CLIENT_LINELOSSʧ�� ���� \n");
        return ;
    }
}


/*******************************************************************************
* ��������: TimeManage
* ��������: ʱ�������
* �������: 
* �������: 
* �� �� ֵ: 
*******************************************************************************/
void TimeManage()
{
    gPastTimeFlag.Word = 0;
    DT_Time2DateTimeHex(time(NULL),&gCurBinTime);

    //����
    if (gCurBinTime.month != gPreBinTime.month)
    {
        //ͬʱ�������¡����ա���ʱ�����֡������־
        gPastTimeFlag.BitSect.Month = TRUE;
        gPastTimeFlag.BitSect.Day = TRUE;
        gPastTimeFlag.BitSect.Hour = TRUE;
        gPastTimeFlag.BitSect.Minute = TRUE;
        gPastTimeFlag.BitSect.Second = TRUE;
    }
    //����
    if (gCurBinTime.day != gPreBinTime.day)
    {
        //ͬʱ�������ա���ʱ�����ֱ�־
        gPastTimeFlag.BitSect.Day = TRUE;
        gPastTimeFlag.BitSect.Hour = TRUE;
        gPastTimeFlag.BitSect.Minute = TRUE;
        gPastTimeFlag.BitSect.Second = TRUE;
    }
    //��ʱ
    else if (gCurBinTime.hour != gPreBinTime.hour)
    {
        //ͬʱ������ʱ�����ֱ�־
        gPastTimeFlag.BitSect.Hour = TRUE;
        gPastTimeFlag.BitSect.Minute = TRUE;
        gPastTimeFlag.BitSect.Second = TRUE;
    }
    //����
    else if (gCurBinTime.min != gPreBinTime.min)
    {
        //�������ֱ�־
        gPastTimeFlag.BitSect.Minute = TRUE;
        gPastTimeFlag.BitSect.Second = TRUE;
//        if((access(DELAY_FILE_PATH,F_OK)==0))
//        {    
//            if(gdelay == 0)
//            {
//                gdelay = 1;
//                PDS_FMT_DEBUG("��⵽��ʼ��ѹ\n");
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
                PDS_FMT_DEBUG("��⵽��ʼ��ѹ\n");
            }
        }else if((access(DELAY_FILE_PATH2,F_OK)==0))
        {
            if(gdelay != 2)
            {
                gdelay = 2;
                gdelaytime = DELAYTIME_MORE;
                PDS_FMT_DEBUG("��һ����ѹ\n");

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

    //����
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
* ��������: PackMeterPrtl
* ��������: 645����Լ��֡
* �������: MeterData           ����Լ����
* �������: PrtlData            ��֡������
* �� �� ֵ:  ֡����
*******************************************************************************/
int PackMeterPrtl(uint8 *PrtlData, MeterPrtlFmt_T MeterData)
{
    uint8   CheckSum = 0;                       //У���
    int     i;
    uint8   CurIndex;                           //����

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
* ��������: getAllBreak
* ��������:�ӵ����л�ȡ���ж�·��
* �������: 
* �������: 
* �� �� ֵ: ��·����������
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
* ��������: UnpackProtocol_645
* ��������:����645��Լ��������
* �������: buf ����
        len ���ĳ���
* �������: data ��������������
* �� �� ֵ: 0 �����ɹ� -1 ��������
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
    //��ַ��
    memcpy(tmp.MtrAddr,&buf[index+1],6);
    //������
    tmp.CtrlCode = buf[index+8];
    //�����򳤶�
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
* ��������: DelayDeal
* ��������: ����ʶ����ʱ����
* �������: delaysec ��ʱ����
* �������: ��
* �� �� ֵ:0 ��ʱ���� -1 ������ʱ
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
* ��������: GetNextBreakIndex
* ��������: �����û������ҵ���һ����·������
* �������: usertype �û����� ��֧��·�����߱����·��
* �������: 
* �� �� ֵ:��·������
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
* ��������: GetNextBreakIndexReadMeter
* ��������: ��ȡ��һ����Ҫ��������Ϣ�Ķ�·������ �����·����һ����֧
* �������: 
* �������: 
* �� �� ֵ:��·������
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
* ��������: GetNextMeterPLC
* ��������: ��ȡ��һ���ز�����
* �������: 
* �������: Addr �ز�������ַ
* �� �� ֵ: ������� ���û���򷵻�0
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
* ��������: GetNextMeterPLC
* ��������: ��ȡ��һ���ز������
* �������: 
* �������: Addr �ز�������ַ
* �� �� ֵ: ������� ���û���򷵻�0
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

    //��ֹ���һ��δ�ҵ��������´ε���ʱ��ֱ�Ӳ���ѭ���������һ���ҵ�����������ѭ��
    if((i>=MeterFileAll.MeterCount)&&(Pn == 0))
    {
        i = 0;
    }
    
    return Pn;
}

/*******************************************************************************
* ��������: WriteNormalData 
* ��������:д��ͨ���ݵ���������
* �������:pipe �˿ں�
        DataBuf ��������
        DataLen ���ݳ���
        Oad
* �������: 
* �� �� ֵ:
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
        PDS_FMT_DEBUG("��ͨ����д�����ݿ�ʧ�� ret %d oad %08x\n",ret,Oad.value);
    }
    return ret;
}

/*******************************************************************************
* ��������: bubble_Sort
* ��������: ð������
* �������: 
* �������: 
* �� �� ֵ:
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
* ��������: SaveTopoResult
* ��������: �洢���˽�����������Ŵ棬�������
* �������: 
* �������: 
* �� �� ֵ:
*******************************************************************************/

/*******************************************************************************
* ��������: ReadLocalAddr 
* ��������: //��ȡ�ն�ͨ�ŵ�ַ
* �������: 
* �������: addr �ն˵�ַ
* �� �� ֵ:  0�ɹ� -1 ʧ��
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
    PDS_FMT_DEBUG("��ȡ�ն˵�ַ 0x%02x%02x%02x%02x%02x%02x \n",addr[5],addr[4],addr[3],addr[2],addr[1],addr[0]);
    return 0;
}
/**********************************************************************
* @name      : ReloadLocalAddr
* @brief     ����ȡ���ص�ַ
* @param[in] ��int clientid    socket���
               uint8 logicId   �߼���ַ
               uint16 infoNum  ��Ϣ���
               uint32 value    oadֵ
* @param[out]��
* @return    ��0-�ɹ�/������

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
* ��������: GetLineLossPeriod 
* ��������: //��ȡ����ͳ������ 60510400
* �������: 
* �������: addr �ն˵�ַ
* �� �� ֵ:  0�ɹ� -1 ʧ��
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
        PDS_FMT_DEBUG("��ȡ�����������ʧ�� Ĭ��1Сʱ  \n");
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
                case 0: //������Ϊ15����
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

    PDS_FMT_DEBUG("��ȡ����������� value %d unit %d  \n",ti.value,ti.unit);
    pthread_mutex_lock(&gLineLossPeriod.lock);
    gLineLossPeriod.ti = ti;
    pthread_mutex_unlock(&gLineLossPeriod.lock);
    return 0;
}

/**********************************************************************
* @name      : ReloadLocalAddr
* @brief     ����ȡ���ص�ַ
* @param[in] ��int clientid    socket���
               uint8 logicId   �߼���ַ
               uint16 infoNum  ��Ϣ���
               uint32 value    oadֵ
* @param[out]��
* @return    ��0-�ɹ�/������

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
* ��������: CheckLineLossCalc
* ��������:���ּ���Ƿ����������ʱ��
* �������:
* �������: 
* �� �� ֵ:  
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
    //Ĭ�������������� ���35�ֿ�ʼ����
    if(gCurBinTime.hour == 0 &&gCurBinTime.min == LINE_LOSS_CALC_DAY_DELAY)
    {
        //ȡ����0�����һ��0��
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
    //Ĭ�����ն��� ����������ͳ���������վͲ������´���
    if(ti.unit==3)
    {
        return FALSE;
    }
    //�ж��Ƿ������������ һ������15���� ���Ժ����10��ִ�е������𲻻��ͻ
    uint16 curmin = gCurBinTime.hour*60+gCurBinTime.min;
    //����
    if(ti.unit == 1)
    {
        period = ti.value;
        if(period>=60)
        {
            period = 15;
        }
        
        if(curmin%period == 0)
        {
            //ȡ����0�����һ��0��
            gLineLossArg->delay = LINE_LOSS_CALC_MIN_DELAY*60;
            gLineLossArg->mainOad.value =0x50020200;
            timetmp = gCurBinTime;
            timetmp.sec = 0;

            DT_DateTimeHex2Time(&timetmp,&gLineLossArg->lastStoreTime[0]);
            //���ڲ�ȷ��֮�������ִ��ʱ�䣬����ǿ�Ƴ�ǰ2��Сʱ�ġ�
            DT_TimeSubInv(&gLineLossArg->lastStoreTime[0],timecompensate);
            DT_TimeSubInv(&gLineLossArg->lastStoreTime[0],ti);
            gLineLossArg->lastStoreTime[1] = gLineLossArg->lastStoreTime[0];
            DT_TimeSubInv(&gLineLossArg->lastStoreTime[1],ti);
            return TRUE;
        }

    }
    //Сʱ
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
    //�� Ĭ��һ����
    if(ti.unit == 4)
    {
        //ÿ����һ�ŵ��賿һ��ͳ�� ��Ϊ�����ж� ���Լ��Ϸֵ���0
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
//��ȡ���������������ն��������ǵ��ֳ������ж�������Ӻ��¶���Ҳ��һ����
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
//�ж�Ҫ�����������������ն��ᡢ���ߡ��¶��ᶼ�Ƿ�ִ����ϣ�ִ����Ϻ��ȥ��ȡ�������
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
            PDS_FMT_DEBUG("��ʼ��ȡ�ն������� \n");
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
            PDS_FMT_DEBUG("��ʼ��ȡ�¶������� \n");
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
            PDS_FMT_DEBUG("��ʼ��ȡ���Ӷ������� \n");
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
        PDS_FMT_DEBUG("pn %d  ��ȡ��������ʧ�� ret %d, recordnum %d, oadMain 0x%08x, store %04d-%02d-%02d %02d:%02d:%02d\n",pn, ret,RecordOut.recordnum,
            mainoad, timetmp.year, timetmp.month, timetmp.day,timetmp.hour, timetmp.minute, timetmp.second);

        return ret;
    }
    PDS_FMT_DEBUG("pn %d  ��ȡ�������ݳɹ� oadMain 0x%08x, store %04d-%02d-%02d %02d:%02d:%02d\n",pn,
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

            //�ܼ����ʵ�����ֻȡ�ܵ�����
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




//��ȡ������ݣ������������ݵĴ洢ʱ��һ�£����Կ����óɹ�ʱ����ɸѡ����
void GetMeterData()
{
	OOP_OAD_U mainoad;
	uint32 curtime;
	OOP_DATETIME_S_T timetmp;
	uint32           cStart;      //ʱ����ʼ��
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
        //storetime 0��һ���洢ʱ�� 1���ϸ��洢ʱ��
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
        //storetime 0��һ���洢ʱ�� 1���ϸ��洢ʱ��
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
        //storetime 0��һ���洢ʱ�� 1���ϸ��洢ʱ��
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
        
        PDS_FMT_DEBUG("i %d pn %d ��ǰvalid %d ������� %d ���� %d  �ϴ�valid %d �� %d �� %d\n",i,LineMeter.MeterFile[i].Pn,LineMeter.MeterFile[i].curdatavalid,LineMeter.MeterFile[i].curpositiveenergy,LineMeter.MeterFile[i].curnegetiveenergy,
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
//			PDS_FMT_DEBUG("pn %d ��ȡ��������ʧ�� ret %d, recordnum %d, oadMain 0x%08x, store %04d-%02d-%02d %02d:%02d:%02d\n",LineMeter.MeterFile[i].Pn, ret,RecordOut.recordnum,
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
//				//�ܼ����ʵ�����ֻȡ�ܵ�����
//
//				if(startReadRecord.recordOAD.road.oadCols.oad[i].value ==0x00100200)
//				{
//
//					if((LineMeter.savetime %2)==1)
//					{
//
//						memcpy(&energys, RecordOut.record[j].outDataBuf, sizeof(OOP_ENERGY_T));
//						LineMeter.MeterFile[i].lastpositiveenergy = energys.nValue[0];
//						PDS_FMT_DEBUG("�������������LineMeter.MeterFile[i].lastpositiveenergy is %d, i is %d\n",LineMeter.MeterFile[i].lastpositiveenergy,i);
//					}
//					else
//					{
//						memcpy(&energys, RecordOut.record[j].outDataBuf, sizeof(OOP_ENERGY_T));
//						LineMeter.MeterFile[i].curpositiveenergy = energys.nValue[0];
//						
//						PDS_FMT_DEBUG("ż�����������LineMeter.MeterFile[i].curpositiveenergy is %d, i is %d\n",LineMeter.MeterFile[i].curpositiveenergy,i);
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
//						PDS_FMT_DEBUG("�����η������LineMeter.MeterFile[i].lastnegetiveenergy is %d, i is %d\n",LineMeter.MeterFile[i].lastnegetiveenergy,i);
//					}
//					else
//					{
//						memcpy(&energys, RecordOut.record[j].outDataBuf, sizeof(OOP_ENERGY_T));
//						LineMeter.MeterFile[i].curnegetiveenergy = energys.nValue[0];
//						
//						PDS_FMT_DEBUG("ż���η������LineMeter.MeterFile[i].curnegetiveenergy is %d, i is %d\n",LineMeter.MeterFile[i].curnegetiveenergy,i);
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
//�����������ݣ���ֱ�ӻ�ȡ���������ݻ���,����ÿ�ζ���ȡһ�δ�����
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
	uint32           cStart;      //ʱ����ʼ��
	uint32 storeNo = 0;
	int ret  = 0;

	memset(&lnslstatsva,0x00,sizeof(OOP_LNLS_STATISVAL_T));
    memset(&inRecord,0x00,sizeof(READ_RECORD_T));

//	if(LineMeter.savetime<2)
//	{
//		PDS_FMT_DEBUG("�������ݲ�������LineMeter.savetime %d\n", LineMeter.savetime);
//		return;
//	}
    if(LineMeter.MeterFile[0].curdatavalid !=1 || LineMeter.MeterFile[0].lastdatavalid !=1)
        return;
    for(i=1;i<LineMeter.MeterCount;i++)
    {
        PDS_FMT_DEBUG("%d pn %d ��ǰ�� %d �� %d ��һ�� �� %d �� %d \n",i,LineMeter.MeterFile[i].Pn,LineMeter.MeterFile[i].curpositiveenergy,
            LineMeter.MeterFile[i].curnegetiveenergy,LineMeter.MeterFile[i].lastpositiveenergy,LineMeter.MeterFile[i].lastnegetiveenergy);
        curtotalpositivevalue += LineMeter.MeterFile[i].curpositiveenergy;
        curtotalnegetivevalue += LineMeter.MeterFile[i].curnegetiveenergy;
        lasttotalpositivevalue += LineMeter.MeterFile[i].lastpositiveenergy;
        lasttotalnegetivevalue += LineMeter.MeterFile[i].lastnegetiveenergy;

    }
	PDS_FMT_DEBUG("�������б�ĵ�����ǰ �� %d �� %d ��һ�� �� %d �� %d \n",curtotalpositivevalue,curtotalnegetivevalue,lasttotalpositivevalue,
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
//				PDS_FMT_DEBUG("�������� curtotalpositivevalue %d,LineMeter.MeterFile[i].curpositiveenergy is %d ,��Ϣ�� is%d\n",curtotalpositivevalue, LineMeter.MeterFile[i].curpositiveenergy,LineMeter.MeterFile[i].Pn);
//				curtotalnegetivevalue =curtotalnegetivevalue+LineMeter.MeterFile[i].curnegetiveenergy;
//				PDS_FMT_DEBUG("�ϴη��� curtotalnegetivevalue %d,LineMeter.MeterFile[i].curnegetiveenergy is %d ,��Ϣ�� is%d\n",curtotalpositivevalue, LineMeter.MeterFile[i].curnegetiveenergy,LineMeter.MeterFile[i].Pn);
//			
//				lasttotalpositivevalue =lasttotalpositivevalue+LineMeter.MeterFile[i].lastpositiveenergy;
//				
//				PDS_FMT_DEBUG("�ϴ����� lasttotalpositivevalue %d,LineMeter.MeterFile[i].curpositiveenergy is %d ,��Ϣ�� is%d\n",lasttotalpositivevalue, LineMeter.MeterFile[i].lastpositiveenergy,LineMeter.MeterFile[i].Pn);
//				lasttotalnegetivevalue = lasttotalnegetivevalue+LineMeter.MeterFile[i].lastnegetiveenergy;
//				PDS_FMT_DEBUG("�ϴη��� lasttotalpositivevalue %d,LineMeter.MeterFile[i].lastnegetiveenergy is %d ,��Ϣ�� is%d\n",lasttotalpositivevalue, LineMeter.MeterFile[i].lastnegetiveenergy,LineMeter.MeterFile[i].Pn);
//			
//
//			}
//			else
//			{
//				curtotalpositivevalue=curtotalpositivevalue+LineMeter.MeterFile[i].lastpositiveenergy;
//				PDS_FMT_DEBUG("�������� curtotalpositivevalue %d,LineMeter.MeterFile[i].curpositiveenergy is %d ,��Ϣ�� is%d\n",curtotalpositivevalue, LineMeter.MeterFile[i].curpositiveenergy,LineMeter.MeterFile[i].Pn);
//				
//				curtotalnegetivevalue =curtotalnegetivevalue+LineMeter.MeterFile[i].lastpositiveenergy;
//				PDS_FMT_DEBUG("�ϴη��� curtotalnegetivevalue %d,LineMeter.MeterFile[i].curnegetiveenergy is %d ,��Ϣ�� is%d\n",curtotalpositivevalue, LineMeter.MeterFile[i].curnegetiveenergy,LineMeter.MeterFile[i].Pn);
//			
//				lasttotalpositivevalue =lasttotalpositivevalue+LineMeter.MeterFile[i].curpositiveenergy;
//				PDS_FMT_DEBUG("�ϴ����� lasttotalpositivevalue %d,LineMeter.MeterFile[i].curpositiveenergy is %d ,��Ϣ�� is%d\n",lasttotalpositivevalue, LineMeter.MeterFile[i].lastpositiveenergy,LineMeter.MeterFile[i].Pn);
//			
//				lasttotalnegetivevalue = lasttotalnegetivevalue+LineMeter.MeterFile[i].curpositiveenergy;
//				PDS_FMT_DEBUG("�ϴη��� lasttotalpositivevalue %d,LineMeter.MeterFile[i].lastnegetiveenergy is %d ,��Ϣ�� is%d\n",lasttotalpositivevalue, LineMeter.MeterFile[i].lastnegetiveenergy,LineMeter.MeterFile[i].Pn);
//
//			}
//		}
//
//	}
	
	diffenergy =abs(curtotalpositivevalue-curtotalnegetivevalue)-abs(lasttotalpositivevalue-lasttotalnegetivevalue);
	
	PDS_FMT_DEBUG("�������б�ĵ�����ֵ diffenergy %d\n",diffenergy);
//	if(LineMeter.savetime>=2)
//	{
//		if((LineMeter.savetime%2)==0)
//		{
//			masterdiffenergy = abs(LineMeter.MeterFile[0].curpositiveenergy-LineMeter.MeterFile[0].curnegetiveenergy)-abs(LineMeter.MeterFile[0].lastpositiveenergy-LineMeter.MeterFile[0].lastnegetiveenergy);
//			PDS_FMT_DEBUG("�ܱ�ĵ�����ֵ11 diffenergy %d\n",masterdiffenergy);
//		}
//		else
//		{
//			masterdiffenergy = abs(LineMeter.MeterFile[0].lastpositiveenergy-LineMeter.MeterFile[0].lastnegetiveenergy)-abs(LineMeter.MeterFile[0].curpositiveenergy-LineMeter.MeterFile[0].curnegetiveenergy);
//			PDS_FMT_DEBUG("�ܱ������ֵ22 diffenergy %d\n",masterdiffenergy);
//
//		}
//	}
    
    masterdiffenergy = abs(LineMeter.MeterFile[0].curpositiveenergy-LineMeter.MeterFile[0].curnegetiveenergy)-abs(LineMeter.MeterFile[0].lastpositiveenergy-LineMeter.MeterFile[0].lastnegetiveenergy);
	PDS_FMT_DEBUG("�ܱ�ĵ�����ֵ diffenergy %d\n",masterdiffenergy);
    
	curtime =time(NULL);
	DT_Time2DateTimeShort(curtime, &timetmp);
	if(gLineLossPeriod.ti.unit ==3)
	{
		timetmp.hour =0x00;
		timetmp.minute =0x00;
		timetmp.second =0x00;
		
		mainoad.value =0x50040200;
		DT_DateTimeShort2Time(&timetmp,(time_t*)&cStart);

		
		//PDS_FMT_DEBUG("����洢ʱ�� cStart %d,mainoad.value is %x\n",cStart,mainoad.value);
	}
	else if(gLineLossPeriod.ti.unit ==1)
	{	
		mainoad.value =0x50020200;	
		timetmp.second = 0;
        timetmp.minute = 15 * (gCurBinTime.min/15);

        DT_DateTimeShort2Time(&timetmp,(time_t*)&cStart);
        //DT_TimeSubInv((time_t*)&cStart,gLineLossPeriod.ti);
		//PDS_FMT_DEBUG("����洢ʱ�� cStart %d,mainoad.value is %x\n",cStart,mainoad.value);
	}
	else if(gLineLossPeriod.ti.unit ==4)
	{
		timetmp.day =0x01;
		timetmp.hour =0x00;
		timetmp.minute =0x00;
		timetmp.second =0x00;
		
		mainoad.value =0x50060200;
		DT_DateTimeShort2Time(&timetmp,(time_t*)&cStart);
		
		//PDS_FMT_DEBUG("����洢ʱ�� cStart %d,mainoad.value is %x\n",cStart,mainoad.value);

	}

    DT_Time2DateTimeShort(cStart, &timetmp);

    PDS_FMT_DEBUG("����洢ʱ�� %d/%d/%d %d:%d:%d ,mainoad is %x\n",timetmp.year,timetmp.month,timetmp.day,timetmp.hour,
        timetmp.minute,timetmp.second,mainoad.value);
	lnslstatsva.nNum =0x01;
	lnslstatsva.rsv=0x01;
	lnslstatsva.nVal[0].eneSup = masterdiffenergy;
	lnslstatsva.nVal[0].eneSal = diffenergy;
    //�����ʵ�λ% ����-2 ����Ҫ����10000
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
* ��������: CheckLineImpedanceCalc
* ��������:���ּ���Ƿ�����·�迹����ʱ��
* �������:
* �������: 
* �� �� ֵ:  
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
* ��������: CalcTaskCheck
* ��������:calc�̼߳���Ƿ���ָ������
* �������:
* �������: 
* �� �� ֵ:  
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
* ��������: CalcTaskCheck
* ��������:calc�̼߳���Ƿ���ָ������
* �������:
* �������: 
* �� �� ֵ:  
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
        //Ĭ��15���Ӽ���һ���迹ֵ ��ʱ5����
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
//* ��������: CalcTaskCheck
//* ��������:calc�̼߳���Ƿ���ָ������
//* �������:
//* �������: 
//* �� �� ֵ:  
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
* ��������: transReadQueue
* ��������: ������
* ���������pLockQueue          ���������ṹָ��
*           buf                 �����ݻ��������������е�����ȡ������û�����
* ���������plockqueue          �����е�����
* �ڲ�����: �ж������Ƿ������ݣ�����Ӷ�����ȡ�����ݷ���buf�У�ÿִ��һ�Σ�ȡһ������
* �� �� ֵ: len                 ���������ݳ���
*******************************************************************************/
int32 TransReadQueue(LockQueue_T *pLockQueue, uint8 *buf)
{
    int32 len = 0;
    uint8 *pdata_buf;

    //��������
    pthread_mutex_lock(&pLockQueue->QueueLock);  
  
    //�������������ݣ���ȡ��buf�У��⿪��
    if ((pLockQueue->QueueHead != pLockQueue->QueueTail)
        || ((pLockQueue->QueueHead == pLockQueue->QueueTail)
            && (pLockQueue->Queue[pLockQueue->QueueHead].QueueLen != 0)))
    {   
        pdata_buf = pLockQueue->Queue[pLockQueue->QueueHead].QueueBuf;  
        len = pLockQueue->Queue[pLockQueue->QueueHead].QueueLen;
        
        //ȡ�����е�����
        memcpy(buf, pdata_buf, len); 

        //������
        memset(pdata_buf, 0, len);   
        pLockQueue->Queue[pLockQueue->QueueHead].QueueLen = 0;

        //����ͷ����ƶ�һλ
        pLockQueue->QueueHead= (pLockQueue->QueueHead + 1) % (MAX_QUEUE_CNT); 
    }
        
    pthread_mutex_unlock(&pLockQueue->QueueLock);
    
    return len;
}

/*******************************************************************************
* ��������: transWriteQueue
* ��������: д����
* ���������plockqueue          ���������ṹָ��
            buf                 ��Ҫд����е�����
            buf_len             ���ݳ���
* �����������
* �ڲ�����: �����в�������buf�е����ݷ�������У���������������������
* �� �� ֵ: >0                  �ɹ�
            =0                  ʧ��
*******************************************************************************/
int32 TransWriteQueue(LockQueue_T *pLockQueue, uint8 *buf, int32 buf_len)
{
    int32 flags = 0;
        
    //��������
    pthread_mutex_lock(&pLockQueue->QueueLock);  
    //�ж������Ƿ��пյ�λ��
    if (pLockQueue->Queue[pLockQueue->QueueTail].QueueLen == 0)
    {
        pLockQueue->Queue[pLockQueue->QueueTail].QueueLen = buf_len;    
        memcpy(pLockQueue->Queue[pLockQueue->QueueTail].QueueBuf, buf, buf_len);
        flags = 1;
        pLockQueue->QueueTail = (pLockQueue->QueueTail + 1) % (MAX_QUEUE_CNT);
    }   
    
    //�⻺����
    pthread_mutex_unlock(&pLockQueue->QueueLock); 
    
    return flags;
}

#if DESC("����ʶ�����",1)

void topology_swap(void *a, int id)
{
    Topology_Arg_T *gTopologyArg = &gCalcArg.topologyArg;
    Break_Son_T tmpNode;  
    uint16 breakID1 = ((Break_Son_T*)((Break_Son_T*)a+id))->breakID;
    uint16 breakID2 = ((Break_Son_T*)((Break_Son_T*)a+id+1))->breakID;
    Break_Info_T *pBreak1 = &gTopologyArg->breakInfo[breakID1];
    Break_Info_T *pBreak2 = &gTopologyArg->breakInfo[breakID2];

    //����
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

    /*�������������Ϣ*/
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
* @brief     ��������ʶ�����洢��˽���ļ���
* @param[in]  topoArg ����ʶ����

* @param[out]��
* @return    ��

**********************************************************************/
void SaveTopoInfoFile(Topology_Arg_T *topoArg)
{
    int ret = ERR_OK;
    char file[200]={0};
    sprintf(file,"%s/TopoInfoFile",PrivateDir);
    ret = write_pdata_xram(file,(char*)topoArg,0,sizeof(Topology_Arg_T));
    if(ret < 0)
    {
        PDS_FMT_DEBUG("SaveTopoInfoFile ʧ�� path=%s, ret=%d \n", file, ret);
    }
}

/**********************************************************************
* @name      : LoadTopoInfoFile
* @brief     ���ϵ��������ʶ����
* @param[in]  
* @param[out]��topoArg ����ʶ����

* @return    ��

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
        PDS_FMT_DEBUG("LoadTopoInfoFile �ɹ� \n");
    }
}

/*******************************************************************************
* ��������: GetPhase
* ��������: ��ȡ��������Ϣ�з�����λ�� BIT0:A BIT1:B BIT2:C,��λ�������� ת��Ϊ698��λ��Ϣ
* �������: in ��ȡ��������Ϣ������λ

* �������: 
* �� �� ֵ:��λ��Ϣ δ֪��0����A��1����B��2����C��3�������ࣨFF�� 
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
* ��������: GetBreakSon
* ��������: ������ȡ��������Ϣ�еļ�¼���ҵ���·�����׽ڵ�
* �������: recordtime ��¼����ʱ��
         phase ��λ
* �������: 
* �� �� ֵ:��·������
*******************************************************************************/
void GetBreakSon(Topology_Arg_T *gTopologyArg, uint32 recordtime,uint8 phase)
{
    uint16 breakindex = gTopologyArg->breakIndex;
    uint32 starttime;
    uint8 i;
    uint8 parentCnt = 0;
    uint8 suncount = 0;
    int interval = 0;

    //�������ж�·�����ҵ�����ʱ����֮ƥ��Ķ�·�������յ���Ϊ�ö�·���������źţ���ǰ�յ���Ϣ�Ķ�·��Ϊ�״�
    for(i=0;i<gTopologyArg->breakCount;i++)
    {
        //��ȡ���Լ����������Ϣ����ʱ�䣬�����յ���������Ϣʱ�����
        if(i == breakindex)
        {
            continue;
        }
        
        starttime = gTopologyArg->breakInfo[i].starttime;
        interval = recordtime-starttime;
        
        PDS_FMT_DEBUG("%012llx recordtime:%d - starttime:%d = %d, Limit(0,%d) \n", *(uint64*)gTopologyArg->breakInfo[i].Addr, recordtime, starttime, interval, TOPO_INTERVAL);

        //ʱ���С����ֵ���Ҳ����Լ�������������б�
        if(abs(interval) > 0 && abs(interval) < TOPO_INTERVAL)
        {
            //�״��Ƕȴ洢�����������������
            parentCnt = gTopologyArg->breakInfo[i].ParentNum;
            gTopologyArg->breakInfo[i].ParentInfo[parentCnt].breakID = breakindex;
            gTopologyArg->breakInfo[i].ParentInfo[parentCnt].phase = phase;
            memcpy(gTopologyArg->breakInfo[i].ParentInfo[parentCnt].addr,gTopologyArg->breakInfo[breakindex].Addr,6);
            gTopologyArg->breakInfo[i].ParentNum++;
//            PDS_BUF_DEBUG(gTopologyArg->breakInfo[i].ParentInfo[parentCnt].addr,6,"ʶ���״� ������λ %d   \n",phase);
//            PDS_BUF_DEBUG(gTopologyArg->breakInfo[i].Addr,6,"�Ӵ���ַ\n");

            //�Ӵ��Ƕȴ洢�������迹��������㣬��ط������⣬�����λδ�ؾ�������Ӵ�����λ
            suncount = gTopologyArg->breakInfo[breakindex].SonNum;
            gTopologyArg->breakInfo[breakindex].breakSonInfo[suncount].phase = phase;
            memcpy(gTopologyArg->breakInfo[breakindex].breakSonInfo[suncount].addr,gTopologyArg->breakInfo[i].Addr,6);
            gTopologyArg->breakInfo[breakindex].SonNum++;

//            PDS_BUF_DEBUG(gTopologyArg->breakInfo[i].Addr,6,"ʶ���Ӵ� ��λ %d   \n",phase);  
//            PDS_BUF_DEBUG(gTopologyArg->breakInfo[breakindex].Addr,6,"�״���ַ\n");

            PDS_FMT_DEBUG("ʶ�𵽺��:%012llx-->%012llx, phase %d \n",*(uint64*)gTopologyArg->breakInfo[breakindex].Addr, *(uint64*)gTopologyArg->breakInfo[i].Addr, phase);

            break;
        }
    }

}

/*******************************************************************************
* ��������: UnpackTopoInfo
* ��������: ������ȡ��������Ϣ
* �������: buf ��ȡ��������Ϣ����������
         len �����򳤶�
* �������: 
* �� �� ֵ:��·������
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

    PDS_FMT_DEBUG("%012llx �������˼�¼�� %d \n", *(uint64*)gTopologyArg->breakInfo[breakindex].Addr, recordnum);

    gTopologyArg->breakInfo[breakindex].topoInfoNum += recordnum;
    
    for(i=0;i<recordnum ;i++)
    {
        PDS_FMT_DEBUG("%012llx ��%d�����˼�¼\n", *(uint64*)gTopologyArg->breakInfo[breakindex].Addr, i);
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
* ��������: UnpackTimeInfo
* ��������: ������ȡ��ʱ�䱨��
* �������: buf ��ȡ��ʱ����Ϣ������
         len �����򳤶�
* �������: 
* �� �� ֵ:��·������
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
    
    PDS_FMT_DEBUG("���۲���ʱ�� %ds ʵ�ʲ���ʱ�� %ds\n",timeoffset, gTopologyArg->breakInfo[breakindex].timeoffset);  
}


/*******************************************************************************
* ��������: UnpackMeterInfo
* ��������: ������ȡ�ı�����Ϣ����
* �������: buf ��ȡ�ı�����Ϣ����������
         len �����򳤶�
* �������: 
* �� �� ֵ:��·������
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
    PDS_FMT_DEBUG("��·�� pn %d %012llx �������� %d\n",gTopologyArg->breakInfo[breadindex].Pn,*(uint64*)gTopologyArg->breakInfo[breadindex].Addr,meternum);
    if(len!=5+7*meternum)
    {
        PDS_FMT_DEBUG(" UnpackMeterInfo error len %d meternum %d \n",len,meternum);
        return;
    }
    for(i=0;i<meternum;i++)
    {
        memcpy(gTopologyArg->breakInfo[breadindex].breakSonInfo[i].addr,&buf[index],6);
        PDS_FMT_DEBUG("���� %012llx\n", *(uint64*)gTopologyArg->breakInfo[breadindex].breakSonInfo[i].addr);
        index+=7;
    }

    gTopologyArg->breakInfo[breadindex].isMeterSon = TRUE;
    gTopologyArg->breakInfo[breadindex].SonNum = meternum;
}

/*******************************************************************************
* ��������: topologySend
* ��������:���ݵ�ǰ����״̬ ���б��ķ���
* �������: 
* �������: 
* �� �� ֵ: 0 ���ͳɹ��л�״̬������ 1 �л����˹���״̬ 2 ���˽���
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

    //���������ʧ�ܳ���3��ҲҪ���˵�
    while((breakindex < gTopologyArg->breakCount)&&(gTopologyArg->breakInfo[breakindex].failcnt>=3))
    {
        PDS_FMT_DEBUG("��·��pn %d ʧ�ܴ�������3�� ���ٳ��� \n",gTopologyArg->breakInfo[breakindex].Pn);
        //gTopologyArg->breakInfo[breakindex].failcnt = 0;//����ʱΪ�˸��Ǻ��沽�裬����������

        //���Ժ�������Ҫ����ʱ����֤���˷��͵�ʱ����
        if(gTopologyArg->topoState == TOPO_START)
        {
            gTopologyArg->sendRecvState = PDS_DELAY;
            gTopologyArg->delay = TOPO_DELAY_TIME;
        }
        
        if(gTopologyArg->topoState ==TOPO_TIMING )
        {
            PDS_FMT_DEBUG("�㲥Уʱδ�յ����� �������  \n");
            return 2;
        }
        breakindex = ++gTopologyArg->breakIndex;
    }
    switch(gTopologyArg->topoState)
    {
    //�㲥Уʱ
    case TOPO_TIMING:
    {
        PDS_FMT_DEBUG("����ʶ��״̬�����㲥Уʱ \n");
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
    //�������������Ϣ0x08C0C000
    case TOPO_CLEAR:
    {
        PDS_FMT_DEBUG("����ʶ��״̬����������� \n");
        
        if(breakindex >= gTopologyArg->breakCount)
        {
            PDS_FMT_DEBUG("���ж�·��������Ϣ������ ��ʼʱ�䲹�� \n");
            gTopologyArg->topoState = TOPO_TIME_OFFSET;
            gTopologyArg->breakIndex =0;
            return 1;
        }  
        
        memcpy(data.MtrAddr,gTopologyArg->breakInfo[breakindex].Addr,6);
        data.CtrlCode = 0x04;
        uint8 databuf[]={0x00,0xC0,0xC0,0x08};
        memcpy(data.DataBuff,databuf,4);
        data.DataLen = 4;
        PDS_FMT_DEBUG("��ն�·�� pn %d ������Ϣ \n",gTopologyArg->breakInfo[breakindex].Pn);
    }
        break;
    //ʱ�䲹��
    case TOPO_TIME_OFFSET:
    {
        PDS_FMT_DEBUG("����ʶ��״̬����ʱ�䲹�� \n");
        
        if(breakindex >= gTopologyArg->breakCount)
        {
            PDS_FMT_DEBUG("���ж�·������ʱ�䲹����� ��ʼ��������\n");
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
        PDS_FMT_DEBUG("��·�� pn %d ʱ�䲹�� \n",gTopologyArg->breakInfo[breakindex].Pn);
    }
        break;        
    //��������
    case TOPO_START:
    {
        PDS_FMT_DEBUG("����ʶ��״̬������������ \n");
        
        if(breakindex >= gTopologyArg->breakCount)
        {
            PDS_FMT_DEBUG("���ж�·������������� ��ʼ��ѯ������Ϣ \n");
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
        PDS_FMT_DEBUG("��·�� pn %d �������� \n",gTopologyArg->breakInfo[breakindex].Pn);
    }
        break;

    //�����ѱ�
    case TOPO_START_METER:
    {
        PDS_FMT_DEBUG("����ʶ��״̬���������ѱ� \n");
        breakindex = GetNextBreakIndexReadMeter();
        if(breakindex>=gTopologyArg->breakCount)
        {
            PDS_FMT_DEBUG("���б����·�������ѱ�ɹ� \n");
            gTopologyArg->topoState = TOPO_READ;
            gTopologyArg->breakIndex =0;
            return 1;
        }
        memcpy(data.MtrAddr,gTopologyArg->breakInfo[breakindex].Addr,6);
        data.CtrlCode = 0x13;
        uint8 databuf[]={0x0A,0x0B,0x0A,0x0B,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
        memcpy(data.DataBuff,databuf,12);
        data.DataLen = 12;
        PDS_FMT_DEBUG("��·�� pn %d �����ѱ� \n",gTopologyArg->breakInfo[breakindex].Pn);
    }
        break;    
 
    //��ȡ���˼�¼
    case TOPO_READ:
    {
        PDS_FMT_DEBUG("����ʶ��״̬������ȡ���� \n");
        
        if(breakindex >= gTopologyArg->breakCount)
        {
            PDS_FMT_DEBUG("���ж�������Ϣ��� ��ʼ��������Ϣ \n");
            gTopologyArg->topoState = TOPO_GETMETER;
            gTopologyArg->breakIndex =0;
            return 1;
        }

        memcpy(data.MtrAddr,gTopologyArg->breakInfo[breakindex].Addr,6);
        data.CtrlCode = 0x04;
        uint8 databuf[]={0x01,0xC0,0xC0,0x08,gTopologyArg->breakInfo[breakindex].topoInfoNum+1, TOPO_INFOLIMIT};
        memcpy(data.DataBuff,databuf,6);
        data.DataLen = 6;
        PDS_FMT_DEBUG("��·�� pn %d �ӵ�%d����ʼ��ѯ%d��������Ϣ\n",gTopologyArg->breakInfo[breakindex].Pn, gTopologyArg->breakInfo[breakindex].topoInfoNum+1, TOPO_INFOLIMIT);
    }
        break;
    
    //��ȡ������Ϣ
    case TOPO_GETMETER:
    {
        PDS_FMT_DEBUG("����ʶ��״̬������ȡ���� \n");
        breakindex = GetNextBreakIndexReadMeter();
        if(breakindex>=gTopologyArg->breakCount)
        {
            PDS_FMT_DEBUG("���ж�·����ȡ�����ɹ� \n");
            gTopologyArg->topoState = TOPO_FINISH;
            gTopologyArg->breakIndex =0;
            return 1;
        }
        memcpy(data.MtrAddr,gTopologyArg->breakInfo[breakindex].Addr,6);
        data.CtrlCode = 0x11;
        uint8 databuf[]={0x01,0x03,0xA0,0x04};
        memcpy(data.DataBuff,databuf,4);
        data.DataLen = 4;
        PDS_FMT_DEBUG("��·�� pn %d ��ѯ������Ϣ \n",gTopologyArg->breakInfo[breakindex].Pn);
    }
        break;
    //���� �������˽ṹ ����Ϣд����������
    case TOPO_FINISH:
    {
        PDS_FMT_DEBUG("����ʶ��״̬�������� \n");
        return 2;
    }
        break;
    default:
        PDS_FMT_DEBUG("����ʶ��״̬���������� \n");
        break;
    }
    Sendlen = PackMeterPrtl(Sendbuf, data);
    ret = mqtt_sendtrans(Sendbuf,Sendlen,gTopologyArg->breakInfo[breakindex].port,gTopologyArg->breakInfo[breakindex].baud,gTopologyArg->overtime,&gTopologyArg->msgIndex);
    PDS_BUF_DEBUG(Sendbuf,Sendlen,"����ʶ����Ϣ���� msgindex %d ret %d \n",gTopologyArg->msgIndex,ret);
    return 0;
}

/*******************************************************************************
* ��������: topologyRecv
* ��������:��������ʶ���յ��ı���
* �������: 
* �������: 
* �� �� ֵ: 0 �����ɹ��л�״̬������ -1 ����������
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
        PDS_FMT_DEBUG("���뱨��Ϊ�� \n");
        if(gTopologyArg->topoState==TOPO_TIMING)
        {
            PDS_FMT_DEBUG("�յ��㲥Уʱ����  \n");
            gTopologyArg->delay = TOPO_DELAY_TIME;
            gTopologyArg->topoState=TOPO_CLEAR;
            return 0;
        }
        return -1;
    }
    ret = UnpackProtocol_645(&Recvbuf[lenoffset],framelen,&data);
    if(ret!=0)
    {
        PDS_FMT_DEBUG("645���Ľ���ʧ�� ret %d \n",ret);
        return -1;
    }

    switch(gTopologyArg->topoState)
    {

        case TOPO_CLEAR:
        {
            if((data.CtrlCode == 0x84)&& !memcmp(data.MtrAddr, gTopologyArg->breakInfo[index].Addr,6))
            {
                PDS_FMT_DEBUG("��·�� pn %d �����˼�¼�ɹ� \n",gTopologyArg->breakInfo[index].Pn);
                gTopologyArg->breakIndex++;
            }else
            {
                PDS_FMT_DEBUG("��·�� pn %d �����˼�¼�쳣 \n",gTopologyArg->breakInfo[index].Pn);
                return -1;
            }
        }
            break;
        case TOPO_TIME_OFFSET:
        {      
            if((data.CtrlCode == 0x91)&& !memcmp(data.MtrAddr, gTopologyArg->breakInfo[index].Addr,6))
            {
                PDS_FMT_DEBUG("��·�� pn %d ��ȡʱ��ɹ� \n",gTopologyArg->breakInfo[index].Pn);
                UnpackTimeInfo(data.DataBuff,data.DataLen);
                gTopologyArg->breakIndex++;
            }else
            {
                PDS_FMT_DEBUG("��·�� pn %d ��ȡʱ���쳣 \n",gTopologyArg->breakInfo[index].Pn);
                return -1;
            }
        }
            break;        
        case TOPO_START:
        {
            if((data.CtrlCode == 0x84)&& !memcmp(data.MtrAddr, gTopologyArg->breakInfo[index].Addr,6))
            {
                PDS_FMT_DEBUG("��·�� pn %d �������˳ɹ� \n",gTopologyArg->breakInfo[index].Pn);
                
                gTopologyArg->delay = TOPO_DELAY_TIME;
                gTopologyArg->breakIndex++;
            }else
            {
                PDS_FMT_DEBUG("��·�� pn %d ���������쳣 \n",gTopologyArg->breakInfo[index].Pn);
                return -1;
            }
        }
            break;
        case TOPO_START_METER:
        {
            if((data.CtrlCode == 0x93)&& !memcmp(data.MtrAddr, gTopologyArg->breakInfo[index].Addr,6))
            {
                PDS_FMT_DEBUG("��·�� pn %d �����ѱ�ɹ� \n",gTopologyArg->breakInfo[index].Pn);
                gTopologyArg->breakIndex++;
                
            }else
            {
                PDS_FMT_DEBUG("��·�� pn %d �����ѱ��쳣 \n",gTopologyArg->breakInfo[index].Pn);
                return -1;
            }
        }
            break;        
        case TOPO_READ:
        {
            if((data.CtrlCode==0x84)&& !memcmp(data.MtrAddr, gTopologyArg->breakInfo[index].Addr,6))
            {
                PDS_BUF_DEBUG(data.DataBuff,data.DataLen,"�յ���·�� pn %d ������Ϣ \n",gTopologyArg->breakInfo[index].Pn);

                if(UnpackTopoInfo(data.DataBuff,data.DataLen) < TOPO_INFOLIMIT)
                {
                    PDS_FMT_DEBUG("%012llx ���˼�¼���� %d \n", *(uint64*)gTopologyArg->breakInfo[breakindex].Addr, gTopologyArg->breakInfo[breakindex].topoInfoNum);
                    gTopologyArg->breakIndex++;
                }
                
                //���������ǰ��ͳһ��ʱ5����
                if(gTopologyArg->breakIndex >= gTopologyArg->breakCount)
                {
                    gTopologyArg->delay = TOPO_DELAY_TIME_METER;
                }
            }else
            {
                PDS_FMT_DEBUG("��·�� pn %d ��ѯ������Ϣ�쳣 \n",gTopologyArg->breakInfo[index].Pn);
                return -1;
            }
        }
            break;
        case TOPO_GETMETER:
        {
            if((data.CtrlCode==0x91)&& !memcmp(data.MtrAddr, gTopologyArg->breakInfo[index].Addr,6))
            {
                PDS_BUF_DEBUG(data.DataBuff,data.DataLen," �յ���·�� pn %d ������Ϣ \n",gTopologyArg->breakInfo[index].Pn);
                UnpackMeterInfo(data.DataBuff,data.DataLen);
                gTopologyArg->breakIndex++;
            }else
            {
                PDS_FMT_DEBUG(" ��·�� pn %d ��ѯ������Ϣ�쳣 \n",gTopologyArg->breakInfo[index].Pn);
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
* ��������: topoISleaf
* ��������: Ҷ���ж�
* �������: BreakInfo ��·����Ϣ
* �������: 
* �� �� ֵ:0 д��ɹ� ����ʧ��
*******************************************************************************/
BOOLEAN isTopolLeaf(Break_Info_T *BreakInfo)
{
    //û�ж��ӣ����߶���ȫ�ǵ��Ľڵ�ΪҶ�ӽڵ�
    return ((BreakInfo->SonNum == 0) || (BreakInfo->isMeterSon == TRUE));
}

/*******************************************************************************
* ��������: topoInfoUpdate
* ��������: ˢ�����˹�ϵ���
* �������: Addr ������ַ
        Pn��Ϣ���
* �������: 
* �� �� ֵ:0 д��ɹ� ����ʧ��
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
        
        //��������������·���⣬���ж�·�����ӽڵ��б�
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
            //Ҷ�ӵ����˹�ϵ
             parentBreakInfo = &gTopologyArg->breakInfo[BreakInfo->ParentInfo[0].breakID];
             parentBreakInfo->breakSonInfo[parentBreakInfo->SonNum].breakID = i;
             memcpy(parentBreakInfo->breakSonInfo[parentBreakInfo->SonNum].addr, BreakInfo->Addr, 6);
             parentBreakInfo->SonNum++;
             
             //��ȥҶ�Ӻ͸������ɶ�·�������˹�ϵ
             for(j = 0; j < BreakInfo->ParentNum-1; j++)
             {
                 sonBreakInfo = &gTopologyArg->breakInfo[BreakInfo->ParentInfo[j].breakID];
                 parentBreakInfo = &gTopologyArg->breakInfo[BreakInfo->ParentInfo[j+1].breakID];

                 //Ϊ�����Ҹ���
                 memcpy(&sonBreakInfo->ParentInfo[0], &BreakInfo->ParentInfo[j+1], sizeof(Break_Son_T));

                 //Ϊ���������
                 memcpy(&parentBreakInfo->breakSonInfo[parentBreakInfo->SonNum++], &BreakInfo->ParentInfo[j], sizeof(Break_Son_T));
             }

             //�������˹�ϵ
             sonBreakInfo = &gTopologyArg->breakInfo[BreakInfo->ParentInfo[j].breakID];
             //���ĸ���Ϊ��Դ������
             memcpy(sonBreakInfo->ParentInfo[0].addr, LocalAddr, 6);
        }

    }

    for(i = 0; i<gTopologyArg->breakCount; i++)
    {
        BreakInfo = &gTopologyArg->breakInfo[i];
        
        for(j = 0; j < BreakInfo->SonNum; j++)
        {
            PDS_FMT_DEBUG("����·�� %d %012llx-->%012llx-->%012llx\n", *(uint64*)BreakInfo->ParentInfo[0].addr&0xffffffffffff, *(uint64*)BreakInfo->Addr&0xffffffffffff, *(uint64*)BreakInfo->breakSonInfo[j].addr&0xffffffffffff);
        }         
    }
    
    return;
}
int GetEverPnPhase(uint16 pn)
{
    OOP_NODEPHASEINFO_T Pn_Phase;
    uint16 tmp;
    Pn_Phase = gettmpphase(pn);
//    if(sizeof(Pn_Phase)==0)       //���ڵ���Ϣʧ�ܣ�ֱ�ӷ���
//      return 0;
    tmp = Pn_Phase.info&0x07;
    PDS_FMT_DEBUG("��ڵ� Pn %d ��λtmp %d\n",pn,tmp);
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
* ��������: WriteTopoInfo
* ��������: �о�������ַ�Ҷ�Ӧ�ĸ��ڵ���Ϣ
* �������: Addr ������ַ
        Pn��Ϣ���
* �������: 
* �� �� ֵ:0 д��ɹ� ����ʧ��
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
        PDS_FMT_DEBUG("��·�� Pn %d %012llx, �״�������%d, �Ӵ�������%d, �Ƿ��е��%d\n", BreakInfo->Pn, *(uint64*)BreakInfo->Addr, BreakInfo->ParentNum, BreakInfo->SonNum, BreakInfo->isMeterSon);

        //Ҷ�������״��б����һ���Ӹ���Ҷ�ӵ�����·��
        if(isTopolLeaf(BreakInfo))
        {
            //�����״��б��������״����Ӵ����������򣬴�Ҷ����
            bubble_Sort(BreakInfo->ParentInfo, BreakInfo->ParentNum, topology_swap);
            
            //�����Ҷ���Ǳ����·��,����ȫ�ǵ��
            if(BreakInfo->isMeterSon == TRUE)
            {

                for(;;) //Ϊÿ������ҵ�����
                {
                    Pn = GetNextNonBreakPLC(Addr);
                    if(Pn == 0)
                    {
                        break;
                    }

                    //ÿ�����нڵ���Ϣ�����������Ϊ��Ϣ������洢
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
                            PDS_FMT_DEBUG("��ڵ� Pn %d %012llx<--%012llx, phase %d ret %d  \n",Pn, *(uint64*)Addr, *(uint64*)BreakInfo->Addr, TopoInfo.NodePhaseInfo, ret);
                        }
                    }

                }
            }
            
            if(BreakInfo->ParentNum == 0)
            {
                //�Ǳ��Ľ���ʧ�ܵ��µģ���Ȼ��Ϊ�Ǹ�����Ҫ��Ȳ�����Ҳ�������źŵĹ��϶�·�����֣�������Դ���������������ź�
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
                    PDS_FMT_DEBUG("�½ڵ� Pn %d %012llx<--%012llx, phase %d ret %d  \n",BreakInfo->Pn, *(uint64*)BreakInfo->Addr, *(uint64*)LocalAddr, TopoInfo.NodePhaseInfo, ret);
                }
                else
                {
                    PDS_FMT_DEBUG("���ڵ� Pn %d %012llx\n", BreakInfo->Pn, *(uint64*)BreakInfo->Addr);
                }
            }
            else
            {
                parentBreakInfo = &gTopologyArg->breakInfo[BreakInfo->ParentInfo[0].breakID];
                BreakInfo->nodeType = NODE_LEAF;
                memset(&TopoInfo,0x00,sizeof(OOP_NodeTopologyInfo_T));
                TopoInfo.NodeDevAddr.len = 5;
                memcpy_r(TopoInfo.NodeDevAddr.add,BreakInfo->Addr,6);
                TopoInfo.NodePhaseInfo = 0xff; //��λ���Ը��� //�޸�Ϊ�㶨0xff
                TopoInfo.ParentNodeDevAddr.len = 5;
                memcpy_r(TopoInfo.ParentNodeDevAddr.add,parentBreakInfo->Addr,6);
                TopoInfo.ParentNodeLineAddr.len = 5;
                memcpy_r(TopoInfo.ParentNodeLineAddr.add,parentBreakInfo->Addr,6);
                ret = WriteNormalData(gCalcArg.clientid,(uint8*)&TopoInfo,sizeof(OOP_NodeTopologyInfo_T),oad,BreakInfo->Pn);
                PDS_FMT_DEBUG("Ҷ�ڵ� Pn %d %012llx<--%012llx, phase %d ret %d  \n",BreakInfo->Pn, *(uint64*)BreakInfo->Addr, *(uint64*)parentBreakInfo->Addr, TopoInfo.NodePhaseInfo, ret);

                //��ȥҶ�Ӻ͸������ɶ�·�������˹�ϵ
                for(j = 0; j < BreakInfo->ParentNum-1; j++)
                {
                    sonBreakInfo = &gTopologyArg->breakInfo[BreakInfo->ParentInfo[j].breakID];
                    parentBreakInfo = &gTopologyArg->breakInfo[BreakInfo->ParentInfo[j+1].breakID];

                    sonBreakInfo->nodeType = NODE_BODY;

                    memset(&TopoInfo,0x00,sizeof(OOP_NodeTopologyInfo_T));
                    TopoInfo.NodeDevAddr.len = 5;
                    memcpy_r(TopoInfo.NodeDevAddr.add,sonBreakInfo->Addr,6);
                    TopoInfo.NodePhaseInfo = 0xff; //��λ���Ը���
                    TopoInfo.ParentNodeDevAddr.len = 5;
                    memcpy_r(TopoInfo.ParentNodeDevAddr.add,parentBreakInfo->Addr,6);
                    TopoInfo.ParentNodeLineAddr.len = 5;
                    memcpy_r(TopoInfo.ParentNodeLineAddr.add,parentBreakInfo->Addr,6);
                    ret = WriteNormalData(gCalcArg.clientid,(uint8*)&TopoInfo,sizeof(OOP_NodeTopologyInfo_T),oad,sonBreakInfo->Pn);
                    PDS_FMT_DEBUG("�ɽڵ� Pn %d %012llx<--%012llx, phase %d ret %d  \n",sonBreakInfo->Pn, *(uint64*)sonBreakInfo->Addr, *(uint64*)parentBreakInfo->Addr, TopoInfo.NodePhaseInfo, ret);

                }

                //��Ϊ���һ�������ĸ���Ϊ��Դ������    
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
                PDS_FMT_DEBUG("���ڵ� Pn %d %012llx<--%012llx, phase %d ret %d  \n",sonBreakInfo->Pn, *(uint64*)sonBreakInfo->Addr, *(uint64*)LocalAddr, TopoInfo.NodePhaseInfo, ret);

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
    PDS_FMT_DEBUG("���ڵ� Pn 0 %012llx-->%012llx, phase %d ret %d  \n", *(uint64*)LocalAddr, *(uint64*)LocalAddr, TopoInfo.NodePhaseInfo, ret);

    //�������Ĵ洢��ɺ�ˢ��һ��ȫ�ֱ��������ں���ļ���
    topoInfoUpdate();
    
    return;
}

/*******************************************************************************
* ��������: topologyManager
* ��������: ��������ʶ���� �����������ˣ���ȡ���˼�¼���������˹�ϵ��
* �������: 
* �������: 
* �� �� ֵ: 
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
    const uint16 overtime = 60; //Ĭ�ϳ�ʱʱ��60��
    uint16 breakindex=0;
    //ִ������ʶ����
    switch(gTopologyArg->sendRecvState)
    {
        case PDS_INIT:
        {
            PDS_FMT_DEBUG("�����շ�״̬������ʼ\n");
            if(getAllBreaks()==0)
            {
                PDS_FMT_DEBUG("��ǰ������δʶ�𵽶�·�����޷���������ʶ���ܣ� \n");
                gTopologyArg->isInTopo =0;
                gTopologyArg->isFinish =0;
                return;
            }

            if(pds_topodata_clear() == 0)
            {
                PDS_FMT_DEBUG("ɾ��ȫ��������Ϣ�ɹ��� \n");
            }
            
            gTopologyArg->breakIndex = 0;
            gTopologyArg->overtime = overtime;
            gTopologyArg->topoState = TOPO_TIMING;
            gTopologyArg->sendRecvState =PDS_SEND; 
        }
            break;
        case PDS_SEND:
        {
            PDS_FMT_DEBUG("�����շ�״̬��������\n");
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
                PDS_FMT_DEBUG("����ʶ������� \n");
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
                PDS_FMT_DEBUG("��ʱ���� \n");
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
                    PDS_FMT_DEBUG("�յ�����Ϣ��� %d �����ڷ�������Ϣ��� %d  \n",MsgRecv->index,gTopologyArg->msgIndex);
                    break;
                }
                PDS_BUF_TRACE(MsgRecv->playload, MsgRecv->msglen,"�յ�������Ϣ \n");
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
                    PDS_FMT_DEBUG("���ͳ�ʱ \n");
                    gTopologyArg->sendRecvState = PDS_RETRY;
                }
            }
        }
            break;
        case PDS_RETRY:
        {
            PDS_FMT_DEBUG("�����շ�״̬��������\n");
            breakindex = gTopologyArg->breakIndex;
            gTopologyArg->breakInfo[breakindex].failcnt++;
            PDS_FMT_DEBUG("��·�� pn %d ʧ�ܴ��� %d \n",gTopologyArg->breakInfo[breakindex].Pn,gTopologyArg->breakInfo[breakindex].failcnt);
            gTopologyArg->sendRecvState =PDS_SEND;
        }
            break;
        default:
            PDS_FMT_DEBUG("�����շ�״̬���������� \n");
            break;
    }
}

#endif

#if DESC("������ش���",1)
/**********************************************************************
* @name      : GetMeterEnergyData
* @brief     ��������Ϣ��źʹ洢ʱ�� ��ȡ�����й��ܵ���
* @param[in] ��pn ��Ϣ���
            addr ��ַ
            storetime �洢ʱ��
* @param[out]��data ��������
* @return    ��0-�ɹ�/������

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
        PDS_FMT_DEBUG("pn %d %012llx ��ȡ��������ʧ�� ret %d, recordnum %d, oadMain 0x%08x, store %04d-%02d-%02d %02d:%02d:%02d\n",meterData->Pn,*(uint64*)meterData->Addr&0xffffffffffff, ret,RecordOut.recordnum,
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

            //�ܼ����ʵ�����ֻȡ�ܵ�����
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
        PDS_FMT_DEBUG("pn %d %012llx ��ȡ��������ʧ�� ret %d, recordnum %d, oadMain 0x%08x, store %04d-%02d-%02d %02d:%02d:%02d\n",meterData->Pn,*(uint64*)meterData->Addr&0xffffffffffff, ret,RecordOut.recordnum,
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

            //�ܼ����ʵ�����ֻȡ�ܵ�����
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
                    
                PDS_FMT_DEBUG("A ������ data->nValue[0] is %d\n",data->nValue[0]);
            break;
            case 2:
                {
                    data->nValue[2] = data->nValue[0];
                    set_bit_value((uint8*)&data->rsv, 32, 2);
                    
                PDS_FMT_DEBUG("B ������ data->nValue[0] is %d\n",data->nValue[0]);
                }
            break;
            case 4:
                {
                    data->nValue[3] = data->nValue[0];
                    set_bit_value((uint8*)&data->rsv, 32, 3);
                    
                PDS_FMT_DEBUG("C ������ data->nValue[0] is %d\n",data->nValue[0]);
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
        PDS_FMT_DEBUG("AA ������ data->nValue[0] is %d\n",data->nValue[0]);
    }
    return 0;
}



#endif

/**********************************************************************
* @name      : GetAllEnergyData
* @brief     ���������е���������
* @param[in] ��

* @param[out]��data ��������
* @return    ��0-�ɹ�/������

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
             
         PDS_FMT_DEBUG("�ñ����λ�� AllMeterData->data[i].phaseinfo %d\n,AllMeterData->data[i].Pn is %d\n",AllMeterData->data[i].phaseinfo,AllMeterData->data[i].Pn);
        }
    
        data = &AllMeterData->data[i];
        
        GetMeterEnergyData(data,gLineLossArg->mainOad,gLineLossArg->lastStoreTime[0],&data->EleData[0]);
        DT_Time2DateTimeShort(gLineLossArg->lastStoreTime[0], &time_tmp);
        PDS_FMT_DEBUG("pn %d MainOad 0x%08x pt%d ct%d �ϴδ洢ʱ��%04d-%02d-%02d %02d:%02d:%02d �ϴ������й���ֵ %d\n",data->Pn,gLineLossArg->mainOad,data->Pt,data->Ct,time_tmp.year,time_tmp.month,time_tmp.day,time_tmp.hour,time_tmp.minute,time_tmp.second,data->EleData[0].nValue[0]);
        GetMeterEnergyData(data,gLineLossArg->mainOad,gLineLossArg->lastStoreTime[1],&data->EleData[1]);
        DT_Time2DateTimeShort(gLineLossArg->lastStoreTime[1], &time_tmp);
        PDS_FMT_DEBUG("pn %d MainOad 0x%08x pt%d ct%d ���ϴδ洢ʱ��%04d-%02d-%02d %02d:%02d:%02d ���ϴ������й���ֵ %d\n",data->Pn,gLineLossArg->mainOad,data->Pt,data->Ct,time_tmp.year,time_tmp.month,time_tmp.day,time_tmp.hour,time_tmp.minute,time_tmp.second,data->EleData[1].nValue[0]);
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
* @brief     ���ӽڵ�������
* @param[in] ��

* @param[out]��
* @return    ��0-�ɹ�/������

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
* @brief     ���ݸ��ڵ���ӽڵ�ĵ��������������� ������=�������ڵ��������-���ڵ������ӽڵ��������֮�ͣ�/���ڵ����������*100%
* @param[in] ��

* @param[out]��
* @return    ��0-�ɹ�/������

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

    //���㸸�ڵ��������
    CalcEnergyIncrement(parentdata, parentdata+1,&parentIncre);

	PDS_FMT_DEBUG("���ڵ������й���ֵ����%d\n",parentIncre.nValue[0]);

    //�����ӽڵ��������
    CalcEnergyIncrement(sondata, sondata+1,&sonIncre);
	PDS_FMT_DEBUG("�ӽڵ������й���ֵ����%d\n",sonIncre.nValue[0]);
 
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
* @brief     ��������㺯��
* @param[in] ��

* @param[out]��
* @return    ��0-�ɹ�/������

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
    //PDS_ENERGY4V_T allleafdata[2]={{0}};     //�����ӽڵ����֮��
    uint16 i;
    #endif
    uint16 pn;
    PDS_ENERGY4V_T *data = NULL;
    PDS_ENERGY4V_T parentdata[2];       //���ڵ����
    //PDS_ENERGY4V_T sondata[2];  
    //#ifdef AREA_HUNAN//�����ӽڵ����֮��
    PDS_ENERGY4V_T allleafdata[2]={{0}};     //�����ӽڵ����֮��
    //#endif
    uint8 Addr[6] = {0};

    memset(&AllMeterData,0x00,sizeof(All_Meter_Data_T));    
    #ifdef AREA_HUNAN
    if(gTopologyArg->isFinish == 0)
    {
        PDS_FMT_DEBUG("��ǰû��ʶ�����˽ṹ �޷������������ \n");
        gCalcArg.lineLossArg.isNeedCalc =0;
        return -1;
    }
    #endif
    GetAllEnergyData(&AllMeterData);
    #ifdef AREA_HUNAN
    //���μ���ÿ����·������������
    for(i=0; i<gTopologyArg->breakCount; i++)
    {
        Breakinfo = &gTopologyArg->breakInfo[i];
        data = GetEnergyData(Breakinfo->Pn);
        if(NULL==data || data[0].nNum != 4 || data[1].nNum != 4)
        {
            PDS_FMT_DEBUG("parent pn %d frozen data abnormal, lstnum %d lstlstnum %d\n",Breakinfo->Pn, data[0].nNum, data[1].nNum);
            continue;
        }
		PDS_FMT_DEBUG("��·�� pn  %d �����й����� %d\n",Breakinfo->Pn,((data->nValue[0])-(data->nValue[1])));
        memcpy(&parentdata,data,sizeof(parentdata));

        memset(&sondata,0x00,sizeof(PDS_ENERGY4V_T)*2);
        
        for(j=0;j<Breakinfo->SonNum;j++)
        {
            
            pn = GetPnFromAddr(Breakinfo->breakSonInfo[j].addr);
            if(pn==0)
            {
                PDS_BUF_DEBUG(Breakinfo->breakSonInfo[j].addr,6,"δ�ҵ�������� \n");
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

        //�����ǰ��·��Ϊ�����·�� �������ж���ΪҶ��
        if(Breakinfo->usertype == USERTYPE_METERBOX_BREAKER)
        {
            EnergyDataAdd(&allleafdata[0],&sondata[0]);
            EnergyDataAdd(&allleafdata[1],&sondata[1]);
        }

        //����ǰ��·��ΪҶ�ӣ����������ݼӵ�Ҷ��������
        if(Breakinfo->SonNum == 0)
        {
            EnergyDataAdd(&allleafdata[0],&parentdata[0]);
            EnergyDataAdd(&allleafdata[1],&parentdata[1]);
            PDS_FMT_DEBUG("�ýڵ� Pn %d �޶��ӽ����ڵ�ֵ�����Լ� �ϴ��ۼ������й��ۼƺ�  %d ���ϴ��ۼ������й��� %d\n",Breakinfo->Pn,allleafdata[0].nValue[0],allleafdata[1].nValue[0]);
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
            PDS_FMT_DEBUG("δ�ҵ�������� \n");
            continue;
        }
        data = GetEnergyData(pn) ;
        if(NULL==data || data[0].nNum != 4 || data[1].nNum != 4)
        {
            PDS_FMT_DEBUG("son pn %d frozen data abnormal, lstnum %d lstlstnum %d\n",pn, data[0].nNum, data[1].nNum);
            continue;
        }
        PDS_FMT_DEBUG("δ�ҵ�������� pn is %d\n",pn);
        EnergyDataAdd(&sondata[0],data);
        PDS_FMT_DEBUG("����1 data->rsv is %d,%d, %d, %d,%d\n",data->rsv,data->nValue[0],data->nValue[1],data->nValue[2],data->nValue[3]);
        EnergyDataAdd(&sondata[1],data+1);
        
    PDS_FMT_DEBUG("����2 data->rsv is %d,%d, %d, %d,%d\n",(data+1)->rsv,(data+1)->nValue[0],(data+1)->nValue[1],(data+1)->nValue[2],(data+1)->nValue[3]);
    }
    #endif
    //����̨��������
    pn = GetMasterBreak(Addr);
    if(pn==0)
    {
        PDS_FMT_DEBUG("δ�ҵ������ܿ���,��ȡ���ɵ�������\n");
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
        PDS_FMT_DEBUG("�ҵ������ܿ���pn %d\n",pn);
        data = GetEnergyData(pn) ;
        if(NULL==data || data[0].nNum != 4 || data[1].nNum != 4)
        {
            PDS_FMT_DEBUG("δ��ȡ�������ܿ��ض�������, lstnum %d lstlstnum %d\n", pn, data[0].nNum, data[1].nNum);
            glineLossArg->isNeedCalc =0;
            return 0;
        }

        memcpy(&parentdata,data,sizeof(parentdata));
    }

    //������������
    CalcLineLossData(glineLossArg, pn, Addr, parentdata, allleafdata);
        
    glineLossArg->isNeedCalc =0;
    
    return 0;
}

#endif

#if DESC("�迹��ش���",1)

/**********************************************************************
* @name      : GetMeterEnergyData
* @brief     ��������Ϣ��źʹ洢ʱ�� ��ȡ��ѹ�͵������Ӷ�������
* @param[in] ��pn ��Ϣ���
            addr ��ַ
            storetime �洢ʱ��
* @param[out] vol ��ѹ
            cur ����
* @return    ��0-�ɹ�/������

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
        PDS_FMT_DEBUG("pn %d %012llx ��ѹ��������ʧ�� ret %d, recordnum %d, oadMain 0x%08x, store %04d-%02d-%02d %02d:%02d:%02d\n", pn, *(uint64*)addr&0xffffffffffff, ret,RecordOut.recordnum,
            startReadRecord.recordOAD.road.oadMain, timetmp.year, timetmp.month, timetmp.day,timetmp.hour, timetmp.minute, timetmp.second);

        return ret;
    }
    
    if(RecordOut.recordnum>2)
    {
        PDS_FMT_DEBUG("pn %d %012llx ��ѹ������¼���������� recordnum %d > 2 \n",pn, *(uint64*)addr&0xffffffffffff, RecordOut.recordnum);
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
* @brief     ��������Ϣ��źʹ洢ʱ�� ��ȡ�迹ֵ
* @param[in] ��pn ��Ϣ���
            addr ��ַ
            storetime �洢ʱ��
* @param[out]��data ��������
* @return    ��0-�ɹ�/������

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
        PDS_FMT_DEBUG("��ȡpn %d �迹����ʧ�� ret %d  \n",pn,ret);
        return ret;
    }
    memcpy(data,RecordOut.record[0].outDataBuf,sizeof(OOP_DWORD3V_T));
    return 0;
}
/**********************************************************************
* @name      : GetAllVolAndCur
* @brief     ���������е����Ķ����ѹ��������
* @param[in] ��

* @param[out]AllMeterData ����
* @return    ��0-�ɹ�/������

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
      PDS_FMT_DEBUG("Pn %d ����λ��Ϣ����,����Ĭ�Ͻ��� \n", Pn);
      return PnPhase;
    }
    return PnPhase;
 }
 
 /*******************************************************************************
 * ��������: taskmng_usertype_get_from_no
 * ��������: ������Ż�ȡ����û�����
 * �������: Pn             Ҫ���Ĳ�����
 
 * �������: 
   
 * �� �� ֵ:  ����ַ
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
 * @brief     ���о����ӽڵ����ݼ�����·�迹 ������ѹ-�ӵ�ѹ��/������
 * @param[in] ��
 
 * @param[out]��
 * @return    ��
 
 **********************************************************************/
 void LineImpedanceData(uint16 pn,uint8 *addr,Meter_Vol_Cur_T *parentdata,Meter_Vol_Cur_T *sondata,uint32 limit)
 {
     uint8 i;
     uint8 j;
     uint8 eventflag=0;    //1���� 2�ָ�
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
     //��ʱ��ӣ�����ڵ����λ
     //��f2090a00
     usertype=taskmng_usertype_get_from_no(pn,0);
     
//   if(sizeof(Pn_Phase.info)==0)       //���ڵ���Ϣʧ�ܣ�ֱ�ӷ���
//      return;
     Pn_Phase = gettmpphase(pn);  //���ڵ���λ��Ϣ
     for(i=0;i<3;i++)    //���Ե���������������λ��ֹ������߲�ȫ��
     {
        if(usertype==USERTYPE_LOW_BREAKER||usertype==USERTYPE_METERBOX_BREAKER)     //��·�����ܰ��շ������
            {
                j=i;
            }
         else
            {
                if((Pn_Phase.info&(0x01<<i))==0)
                    continue;
                if(usertype==LOT_METER_TPIND)      //�����
                    j=i;
                else
                    j=0;                //�����ֻ��A��ֵ�����Թ̶�Ϊ0
            }

         if(parentdata->vol.nValue[i]==0||sondata->cur.nValue[j]==0||sondata->vol.nValue[j]==0||
           parentdata->vol.nValue[i]==0xFFFF||sondata->cur.nValue[j]==0XFFFFFFFF||sondata->vol.nValue[j]==0XFFFF)
         {
             inValidNum++;
             PDS_FMT_DEBUG("Pn %d LineImpedanceData %d ���������� �޷����� ������ѹ%d �Ӵ�����%d �Ӵ���ѹ%d \n", pn, i,parentdata->vol.nValue[i],sondata->cur.nValue[j],sondata->vol.nValue[j]);
             continue;
         }

         //�迹��λ��ŷķ��������-3,��ѹ��λ�Ƿ���������-1��������λ�ǰ���������-3,����ʱ,��ѹ���Ե����Ľ���ٳ���1000������ֱ���òɼ����������Ҫ����100000
         lineImpeData.nValue[i] = abs(parentdata->vol.nValue[i]*parentdata->Pt - sondata->vol.nValue[j]*sondata->Pt)*100000/(sondata->cur.nValue[j]*sondata->Ct);
         PDS_FMT_DEBUG("Pn %d LineImpedanceData %d �����迹������Ϊ %d limit %d \n", pn, i,lineImpeData.nValue[i],limit);
         set_bit_value((uint8*)&lineImpeData.rsv, 32, i);
         
         //�ж��Ƿ������·�迹Խ���¼�
         if(sondata->lastImpedance.nValue[j]<=limit && lineImpeData.nValue[i] >limit)
         {
             //��·�迹Խ���¼�����
             PDS_FMT_DEBUG("��·�迹Խ���¼�����\n");
             eventflag = 1;
         }
         if(sondata->lastImpedance.nValue[j]>limit && lineImpeData.nValue[i]<=limit)
         {
             //��·�迹Խ���¼��ָ�
             PDS_FMT_DEBUG("��·�迹Խ���¼��ָ�\n");
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
             event.curPhase = i+1; //��λ 1 A 2 B 3 C
             //OAD�ġ�����������ȡֵ���£�
              //   1���¼�����ǰ��
              //   2���¼�������
              //   3���¼�����ǰ��
              //   4���¼�������
            event.data.oadNum = 4;
            event.data.cols.nNum = 4;
            event.data.cols.oad[0].value = 0x20002200; //�¼�����ǰ��ѹ
            event.data.cols.oad[1].value = 0x20004200; //�¼��������ѹ
            event.data.cols.oad[2].value = 0x20012200; //�¼�����ǰ����
            event.data.cols.oad[3].value = 0x20014200; //�¼����������
            if(eventflag == 1)
            {
                DT_Time2DateTimeShort(timenow,&event.tmStart);
//                event.data.oadNum = 4;
//                event.data.cols.nNum = 4;
//                event.data.cols.oad[0].value = 0x20002200+i+1; //�¼�����ǰ��ѹ
//                event.data.cols.oad[1].value = 0x20004200+i+1; //�¼��������ѹ
//                event.data.cols.oad[2].value = 0x20012200+i+1; //�¼�����ǰ����
//                event.data.cols.oad[3].value = 0x20014200+i+1; //�¼����������                 
                 
            }else
            {
                DT_Time2DateTimeShort(timenow,&event.tmStart);
                DT_Time2DateTimeShort(timenow,&event.tmEnd);
//                 event.data.oadNum = 4;
//                 event.data.cols.nNum = 4;
//                 event.data.cols.oad[0].value = 0x20006200+i+1; //�¼�����ǰ��ѹ
//                 event.data.cols.oad[1].value = 0x20008200+i+1; //�¼��������ѹ
//                 event.data.cols.oad[2].value = 0x20016200+i+1; //�¼�����ǰ����
//                 event.data.cols.oad[3].value = 0x20018200+i+1; //�¼����������
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
             //д�¼���¼
             memset(&indata,0x00,sizeof(WRITE_RECORD_T));
             indata.recordOAD.road.oadCols.nNum = 1;
             indata.recordOAD.road.oadCols.oad[0].value = 0x31F20200;
             indata.recordOAD.classtag = CLASS_DATA_EVENT;
             indata.MAC.nNum = 6;
             memcpy_r(indata.MAC.value,LocalAddr,6);
             indata.colStartTm = timenow+READ_DATA_TIME_ADD;
             indata.colEndTm = timenow +READ_DATA_TIME_ADD;
             indata.colStoreTm = event.nIndex;   //ʹ���¼������Ϊ�洢ʱ��
             indata.inDatalen = sizeof(OOP_IMPE_OVER_T);
             memcpy(indata.inDataBuf,&event,sizeof(OOP_IMPE_OVER_T));
             ret = db_write_record(gCalcArg.clientid, &indata, &storeNo);
             if(ret!=0)
             {
                 PDS_FMT_DEBUG("��·�迹Խ���¼�д��ʧ��  \n");
             }
         }
     }
     
     if(inValidNum < 3)
     {
         //д�迹���ݵ���������
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
                 PDS_FMT_DEBUG("��·�迹����д��ʧ�� Pn %d \n", pn);
         }
     }
 }
 
 /**********************************************************************
 * @name      : LineImpedanceCalc
 * @brief     ����·�迹����
 * @param[in] ��
 
 * @param[out]��
 * @return    ��
 
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
         PDS_FMT_DEBUG("��ǰû��ʶ�����˽ṹ �޷�������·�迹���� \n");
         gLineImpedanceArg->isNeedCalc = 0;
         return ;
     }
     pthread_mutex_lock(&gImpeLimit.lock);
     limit = gImpeLimit.limit ;
     pthread_mutex_unlock(&gImpeLimit.lock);
     if(limit==0)
     {
         PDS_FMT_DEBUG("��ǰû�������迹��ֵ �޷�������·�迹���� \n");
         gLineImpedanceArg->isNeedCalc = 0;
         return ;
     }
 
     memset(&AllVolCur,0x00,sizeof(All_Vol_Cur_t));
     GetAllVolAndCur(&AllVolCur);
     if(GetMeterVolAndCur(0,LocalAddr,gLineImpedanceArg->storeTime[0],&ACdata.vol,&ACdata.cur)!=0)
     {
         PDS_FMT_DEBUG("��ȡ�ն˽��ɶ����ѹ��������ʧ�� \n");
         pn = GetMasterBreak(Addr);
         parentdata = GetVolAndCurIndex(pn);
         if(NULL == parentdata)
         {
             PDS_FMT_DEBUG("Pn %d δ��ȡ���ܱ����� \n",pn);
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
         parentdata = GetVolAndCurIndex(Breakinfo->Pn); //���׵�����
         if(NULL==parentdata)
         {
             PDS_FMT_DEBUG("Pn %d δ��ȡ���������� \n",Breakinfo->Pn);
             continue;
         }
         for(j=0;j<Breakinfo->SonNum;j++) //�����ǵ�����
         {
             pn = GetPnFromAddr(Breakinfo->breakSonInfo[j].addr);
             if(pn==0)
             {
                 PDS_BUF_DEBUG(Breakinfo->breakSonInfo[j].addr,6,"δ�ҵ�������� \n");
                 continue;
             }
             sondata = GetVolAndCurIndex(pn);
             if(NULL==sondata)
             {
                 PDS_FMT_DEBUG("�ӽڵ� Pn %d δ��ȡ���������� \n",Breakinfo->Pn);
                 continue;
             }
             LineImpedanceData(pn,Breakinfo->breakSonInfo[j].addr,parentdata,sondata,limit);
         }
         //�����һ����֧ �����Դ�����������迹
         if(Breakinfo->nodeType == NODE_ROOT)
         {
             LineImpedanceData(Breakinfo->Pn,Breakinfo->Addr,&ACdata,parentdata,limit);
         }
     }
     gLineImpedanceArg->isNeedCalc = 0;
 }
#endif

 /*******************************************************************************
* ��������: GetPnFromAddr
* ��������: ���ݵ���ַ��ȡPN
* �������: Addr   ����ַ

* �������: 
  
* �� �� ֵ: ���������
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
* ��������: GetMasterBreak
* ��������: �����û����ͻ�ȡ�ܱ����
* �������: 

* �������: 
  
* �� �� ֵ: ���������
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
* ��������: GetImpedanceLimit 
* ��������: //��ȡ�迹��ֵ 31F20600
* �������: 
* �������: 
* �� �� ֵ:  0�ɹ� -1 ʧ��
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
        PDS_FMT_DEBUG("��ȡ�迹����ֵʧ�� ����Ĭ���迹Խ��ֵ1.5ŷķ\n");
        gImpeLimit.limit = 1500;
        return ret;
    }
    PDS_FMT_DEBUG("��ȡ�迹����ֵ %d  \n",limit.impeLimit);
    pthread_mutex_lock(&gImpeLimit.lock);
    gImpeLimit.limit = limit.impeLimit;
    pthread_mutex_unlock(&gImpeLimit.lock);
    return 0;
}

/**********************************************************************
* @name      : ReloadLocalAddr
* @brief     ����ȡ���ص�ַ
* @param[in] ��int clientid    socket���
               uint8 logicId   �߼���ַ
               uint16 infoNum  ��Ϣ���
               uint32 value    oadֵ
* @param[out]��
* @return    ��0-�ɹ�/������

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
* ��������: GetEventIndex 
* ��������: ���������Ļ�ȡ�¼���ǰ������
* �������: 
* �������: 
* �� �� ֵ:  0�ɹ� -1 ʧ��
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
        PDS_FMT_DEBUG("��ȡ�¼�oad 0x%08x ����ʧ�� \n",oad);
        return 0;
    }
    pTemp = (OOP_EVENT_T*)RecordOut.record[0].outDataBuf;
    return pTemp->nIndex;
}

