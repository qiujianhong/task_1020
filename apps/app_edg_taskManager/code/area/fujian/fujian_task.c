
#ifdef AREA_FUJIAN
#include "CtsAll.h"
#include "fujian_task.h"
#include "fujian_common.h"

#define ERROR_SUCCESS_CODE_ADD  100   //����һ��������ӵ�ֵ ��������֤���
extern uint8  gPowerOnFlag[AMR_THREAD_NUM];
extern FUJIAN_grop_meter oop_grop_meter;
extern OOP_GATHER_E gGatherMode;
FUJIAN_MULTI_645BUF_T gMulti645Buf[MAX_485_NUM]; //ר�Ÿ�485����֡645��
FUJIAN_MULTI_645_RECV_T gMulti645Recv[MAX_485_NUM];//ר�Ÿ�485����֡645�� �յ��ı���

char* gRecordPath = "/data/app/record";
FUJIAN_TASK_CFG_LIST_T gTransTaskCfg; //ȫ��͸�����񷽰���
FUJIAN_SPECIFIC_TASK_LIST_T gSpecificTaskList; //�������񼯺�
FUJIAN_TASK_COLLECTION_STATE_T gTaskCollectionState; //͸�����񷽰��ɼ�״̬��

const PLCTASK_FUNINFO gInnerTaskList[] =
{
    {MS_RELAY_TRANS,    5,      RCS_NULL,      0,            fujian_relay_trans_deal},                  //���͸��ת��

    {MS_NORMAL_AMR,     11,     RCS_NULL,   1,           taskmng_trans_task_deal},               //698��Լ���񳭱�

    {0xFF,              0xFF,   RCS_NULL,       60,           NULL},
};


/*******************************************************************************
* ��������: taskmng_ms_fujian_get_by_mettype
* ��������: ����һ���û����ͻ�ȡ��Ӧ�ĵ��MS
* �������:           inMS          //���������Ļ�ȡ�ĵ����Ϣ
            pMsInfo                 //���������Ϣ
* �������: ��
* �� �� ֵ: 0      //�ظ�
            1      //���ظ�
*******************************************************************************/
uint16 taskmng_ms_fujian_get_by_mettype(OOP_MSC_T *inMS, FUJIAN_ADDR_LIST_T *pMsInfo)
{
    uint16 i,j;
    uint8 MtrType;
    uint16 MsNum = 0;
    for(i=0;i<MeterFileAll.MeterCount;i++)
    {   
        MtrType = MeterFileAll.MeterFile[i].basic.userType;
        for(j=0;j<inMS->mst.size;j++)
        {
            if(MtrType == inMS->mst.type[j])
            {   
                pMsInfo->addr[MsNum] = MeterFileAll.MeterFile[i].basic.tsa;
                MsNum++;
                break;
            }
        }
    }
    pMsInfo->num = MsNum;
    return MsNum;
}

/*******************************************************************************
* ��������: ����һ��������Ż�ȡ��Ӧ�ĵ��MS
* �������:           inMS          //���������Ļ�ȡ�ĵ����Ϣ
            pMsInfo                 //���������Ϣ
* �������: ��
* �� �� ֵ: 0      //�ظ�
            1      //���ظ�
*******************************************************************************/
uint16 taskmng_ms_fujian_get_by_metsn(OOP_MSC_T *inMS, FUJIAN_ADDR_LIST_T *pMsInfo)
{
    uint16 i,j;
    uint16 Sn;
    uint16 MsNum = 0;
    for(i=0;i<MeterFileAll.MeterCount;i++)
    {   
        Sn = MeterFileAll.MeterFile[i].nIndex;
        for(j=0;j<inMS->msi.size;j++)
        {
            if(Sn == inMS->msi.idx[j])
            {   
                pMsInfo->addr[MsNum] = MeterFileAll.MeterFile[i].basic.tsa;
                MsNum++;
                break;
            }
        }
        pMsInfo->num = MsNum;
    }
    return MsNum;
}

/*******************************************************************************
* ��������: ����һ���û����������ȡϵ���û�����
* �������:           inMS          //���������Ļ�ȡ�ĵ����Ϣ
            pMsInfo                 //���������Ϣ
* �������: ��
* �� �� ֵ: 0      //�ظ�
            1      //���ظ�
*******************************************************************************/
uint16 taskmng_ms_fujian_get_by_mettype_region(OOP_MSC_T *inMS, FUJIAN_ADDR_LIST_T *pMsInfo)
{
    uint16 i,j;
    REGION_TYPE    RegionType;
    uint8 start=0;
    uint8 end=0;
    uint16 MsNum = 0;
    uint8 MtrType;
    for(j=0;j<inMS->msrt.size;j++)
    {
        RegionType = inMS->msrt.region[j].nType;
        start = inMS->msrt.region[j].start;
        end = inMS->msrt.region[j].end;
        switch(RegionType)
        {
        case FRONTCLOSE_BACKOPEN://ǰ�պ�
        {
            if(end == 0)
                return 0;
            end -= 1;
            break;
        }

        case FRONTOPEN_BACKCLOSE://ǰ�����
        {
            start += 1;
            break;
        }

        case FRONTCLOSE_BACKCLOSE://ǰ�պ��
        {
            break;
        }

        case FRONTOPEN_BACKOPEN://ǰ����
        {
            start += 1;
            if(end == 0)
                return 0;
            end -= 1;
            break;
        }
        default:
            break;
        }
        if(start>end)
            return 0;
        for(i=0;i<MeterFileAll.MeterCount;i++)
        {
            MtrType = MeterFileAll.MeterFile[i].basic.userType;
            if(MtrType>=start && MtrType<=end)
            {
                pMsInfo->addr[MsNum] = MeterFileAll.MeterFile[i].basic.tsa;
                MsNum++;
            }
        }
    }
    pMsInfo->num = MsNum;
    return MsNum;
}

/*******************************************************************************
* ��������: ����һ���û���ַ�����ȡϵ���û�����
* �������:           inMS          //���������Ļ�ȡ�ĵ����Ϣ
            pMsInfo                 //���������Ϣ
* �������: ��
* �� �� ֵ: 0      //�ظ�
            1      //���ظ�
*******************************************************************************/
uint16 taskmng_ms_fujian_get_by_metaddr_region(OOP_MSC_T *inMS, FUJIAN_ADDR_LIST_T *pMsInfo)
{
    uint16 i,j;
    REGION_TYPE    RegionType;
    OOP_TSA_T start;
    OOP_TSA_T end;
    int64         tmpaddr = 0;
    int64         startaddr64 = 0;
    int64         endaddr64 = 0;
    uint16 MsNum = 0;

    for(j=0;j<inMS->msra.size;j++)
    {
        RegionType = inMS->msra.region[j].nType;
        start = inMS->msra.region[j].start;
        end = inMS->msra.region[j].end;
        switch(RegionType)
        {
        case FRONTCLOSE_BACKOPEN://ǰ�պ�
        {
            TsaCalcfun(&end,-1);//�Ե�ַ���м�1����
            break;
        }

        case FRONTOPEN_BACKCLOSE://ǰ�����
        {
            TsaCalcfun(&start,1);//�Ե�ַ���м�1����
            break;
        }

        case FRONTCLOSE_BACKCLOSE://ǰ�պ��
        {
            break;
        }

        case FRONTOPEN_BACKOPEN://ǰ����
        {
            TsaCalcfun(&start,1);//�Ե�ַ���м�1����
            TsaCalcfun(&end,-1);//�Ե�ַ���м�1����
        }

        default:
            break;
        }
        memcpy_r((uint8*)&startaddr64, start.add, start.len+1);
        memcpy_r((uint8*)&endaddr64, end.add, end.len+1);
        for(i=0;i<MeterFileAll.MeterCount;i++)
        {
            memcpy_r((uint8*)&tmpaddr, MeterFileAll.MeterFile[i].basic.tsa.add, MeterFileAll.MeterFile[i].basic.tsa.len+1);
            if(tmpaddr>=startaddr64 && tmpaddr<=endaddr64)
            {
                pMsInfo->addr[MsNum] = MeterFileAll.MeterFile[i].basic.tsa;
                MsNum++;
            }
        }
    }
    pMsInfo->num = MsNum;
    return MsNum;
}

/*******************************************************************************
* ��������: ����һ���û���������ȡϵ���û�����
* �������:           inMS          //���������Ļ�ȡ�ĵ����Ϣ
            pMsInfo                 //���������Ϣ
* �������: ��
* �� �� ֵ: 0      //�ظ�
            1      //���ظ�
*******************************************************************************/
uint16 taskmng_ms_fujian_get_by_metsn_region(OOP_MSC_T *inMS, FUJIAN_ADDR_LIST_T *pMsInfo)
{
    uint16 i,j;
    REGION_TYPE    RegionType;
    uint16 start=0;
    uint16 end=0;
    uint16 MsNum = 0;
    uint16 Sn;
    for(j=0;j<inMS->msri.size;j++)
    {
        RegionType = inMS->msri.region[j].nType;
        start = inMS->msri.region[j].start;
        end = inMS->msri.region[j].end;
        switch(RegionType)
        {
        case FRONTCLOSE_BACKOPEN://ǰ�պ�
        {
            if(end == 0)
                return 0;
            end -= 1;
            break;
        }

        case FRONTOPEN_BACKCLOSE://ǰ�����
        {
            start += 1;
            break;
        }

        case FRONTCLOSE_BACKCLOSE://ǰ�պ��
        {
            break;
        }

        case FRONTOPEN_BACKOPEN://ǰ����
        {
            start += 1;
            if(end == 0)
                return 0;
            end -= 1;
            break;
        }
        default:
            break;
        }

        if(start > end)
            return 0;
        for(i=0;i<MeterFileAll.MeterCount;i++)
        {
            Sn = MeterFileAll.MeterFile[i].nIndex;
            if(Sn>=start && Sn<=end)
            {
                pMsInfo->addr[MsNum] = MeterFileAll.MeterFile[i].basic.tsa;
                MsNum++;
            }
        }
    }
    pMsInfo->num = MsNum;
    return MsNum;
}

/*******************************************************************************
* ��������: ȥ���ظ���ַ
* �������:   tsa          //�Ѵ洢�ĵ�ַ
              addr               //��ǰ����ַ
* �������: ��
* �� �� ֵ: true      //�ظ�
            false      //���ظ�
*******************************************************************************/

BOOL taskmng_addr_is_repet(FUJIAN_ADDR_LIST_T* tsa,uint8* addr,uint16 count)
{
    if(taskmng_addr_isvalid_check(addr,6)<0)
        return TRUE;
    uint16 i ;
    if(count == 0)
        return FALSE;
    for(i=0;i<count;i++)
    {
        if(memcmp(tsa->addr[i].add,addr,6)==0)
            return TRUE;
    }
    return FALSE;
}
/*******************************************************************************
* ��������: ����һ���û���ַ��ȡϵ���û�����
* �������:           inMS          //���������Ļ�ȡ�ĵ����Ϣ
            pMsInfo                 //���������Ϣ
* �������: ��
* �� �� ֵ: 0      //�ظ�
            1      //���ظ�
*******************************************************************************/

/*ͬʱ����Ⱥ���ַ�ͱ�׼��ַ*/
uint16 taskmng_FJ_ms_list_get_by_metaddr(OOP_MSC_T *inMS, FUJIAN_ADDR_LIST_T *pMsInfo)
{
    int i,j,k=0;
    FUJIAN_ADDR_LIST_T      tmpmsa;   //�洢���з���Ҫ��ĵ�� 
    uint16 count = 0;
    memset(&tmpmsa,0x00,sizeof(FUJIAN_ADDR_LIST_T));
    uint8 flag = 0;
    for(i=0;i<inMS->msa.size;i++)
    {
        if(taskmng_grop_addr_isvalid(inMS->msa.add[i].add)>0)                  //����Ƿ���Ⱥ���ַ
        {
            pthread_rwlock_rdlock(&oop_grop_meter.rwLock);
            for(j=0;j<oop_grop_meter.nNum;j++)
            {

                if(memcmp(inMS->msa.add[i].add,oop_grop_meter.grop_meter[j].groupAddr.add,6)==0)
                {
                    for(k = 0;k<oop_grop_meter.grop_meter[j].num;k++)
                    { 
                        memcpy(tmpmsa.addr[tmpmsa.num].add,oop_grop_meter.grop_meter[j].addr[k].add,6);
                        tmpmsa.addr[tmpmsa.num].len = 5;
                        tmpmsa.num ++;               
                        if(tmpmsa.num>=MAX_SPECIFIC_ADDR_NUM)
                        {
                            TASK_FMT_DEBUG(-1, TASK_LOG, "���������ȡ�ĵ�����ǰ����%d ����������� %d\n",tmpmsa.num,MAX_SPECIFIC_ADDR_NUM);
                            flag = 1;
                            break;
                        }
                    }
                }
                if(flag == 1)
                    break;
            }
            pthread_rwlock_unlock(&oop_grop_meter.rwLock);
        }
        else
        {
            memcpy(&tmpmsa.addr[tmpmsa.num],&inMS->msa.add[i],sizeof(OOP_TSA_T));
            tmpmsa.num ++;
        }
        if(tmpmsa.num>=MAX_SPECIFIC_ADDR_NUM)
        {
            TASK_FMT_DEBUG(-1, TASK_LOG, "���������ȡ�ĵ�����ǰ����%d ����������� %d\n",tmpmsa.num,MAX_SPECIFIC_ADDR_NUM);
            break;
        }
    }
    if(tmpmsa.num!=0)
    {
        memset(pMsInfo,0x00,sizeof(FUJIAN_ADDR_LIST_T));
        for(i=0;i<tmpmsa.num;i++)
        {
              /*ȥ���ظ�����Ч����*/
           if( taskmng_addr_is_repet(pMsInfo,tmpmsa.addr[i].add,count))
           {
               continue;
           }
           memcpy(&pMsInfo->addr[count],&tmpmsa.addr[i],sizeof(OOP_TSA_T));
           count ++;
        }
        pMsInfo->num = count;
    }
    /*չʾ��ַ*/
    TASK_FMT_DEBUG(-1, TASK_LOG, "��ȡ���ĵ�������%d\n",count);
 /*   for(i=0;i<pMsInfo->num;i++)
    {
       TASK_FMT_DEBUG(-1, TASK_LOG, "��ǰΪ��%d������\n",i+1);
       TASK_BUF_DEBUG(-1, TASK_LOG,pMsInfo->addr[i].add,6);
    }*/
    return count;
}
uint16 taskmng_FJ_ms_list_get(OOP_MSC_T *inMS, FUJIAN_ADDR_LIST_T *pMsInfo)
{
    uint16          MsNum = 0;
    uint16 i;
    pthread_rwlock_rdlock(&MeterFileAll.rwLock);

    TASK_FMT_TRACE(-1, TASK_LOG, "taskmng_ms_list_get choice %d \n", inMS->choice);

    switch(inMS->choice)
    {
        //һ���û����ͣ�������ʱ��֧��
        case 1:                                 //ȫ���û���ַ
            {
                for(i=0;i<MeterFileAll.MeterCount;i++)
                {           
                    pMsInfo->addr[MsNum] = MeterFileAll.MeterFile[i].basic.tsa;
                    MsNum++;
                }
                pMsInfo->num = MsNum;
            }

            break;
        case 2:     //һ���û�����
            MsNum = taskmng_ms_fujian_get_by_mettype(inMS,pMsInfo);
            break;
        case 3:     //һ���û���ַ
            MsNum = taskmng_FJ_ms_list_get_by_metaddr(inMS,pMsInfo);
            break;
        case 4:     //һ���������
            MsNum = taskmng_ms_fujian_get_by_metsn(inMS,pMsInfo);
            break;
        case 5:     //һ���û���������
            MsNum =  taskmng_ms_fujian_get_by_mettype_region(inMS,pMsInfo);
            break;
        case 6:     //һ���û���ַ����
            MsNum = taskmng_ms_fujian_get_by_metaddr_region(inMS,pMsInfo);
            break;
        case 7:     //һ�������������
            MsNum = taskmng_ms_fujian_get_by_metsn_region(inMS,pMsInfo);
            break;
        default:
            pthread_rwlock_unlock(&MeterFileAll.rwLock);
            return 0;

    }
    pthread_rwlock_unlock(&MeterFileAll.rwLock);
    return MsNum;
}

/*******************************************************************************
* ��������: taskmng_meter_file_init_with_logicid
* ��������: Ⱥ���ַ������ʼ�� 
* �������: 
            pMeterFileAll   //�����
           logicid �߼��豸��
* �������: ��
* �� �� ֵ:   ��
*******************************************************************************/
uint16 taskmng_grop_meter_init(FUJIAN_grop_meter *pgropMeter)
{
    uint16  MeterCount=0;
    uint16   MeterNum=0;     //��������
    int   ret;
    uint16 i;
    OOP_GROUPMETER_NEW_T gMeterFile;     //ÿ�����ĵ�����Ϣ
    READ_RECORD_T      inReadRecord;
    uint32             handle = 0;
    uint32             recNum = 0;
    STEP_READ_RECORD_T stepReadRecord;
    RECORD_UNIT_T      stepRecordOut;
    memset(&inReadRecord, 0, sizeof(READ_RECORD_T));
    memset(&stepReadRecord, 0, sizeof(STEP_READ_RECORD_T));
    memset(&stepRecordOut, 0, sizeof(RECORD_UNIT_T));
    TASK_FMT_DEBUG(-1,TASK_LOG, "��ʼȺ���ַ��ʼ��! \n");
    pthread_rwlock_wrlock(&pgropMeter->rwLock);

    inReadRecord.recordOAD.infoNum = 0xffff;
    inReadRecord.recordOAD.logicId = 0;
    inReadRecord.cType = COL_NO_INFO;
    inReadRecord.cStart = 0;
    inReadRecord.cEnd = 0xffff;
    inReadRecord.sortType = UP_SORT;
    inReadRecord.recordOAD.optional = 0;
    inReadRecord.recordOAD.road.oadMain.value = 0;
    inReadRecord.recordOAD.road.oadCols.nNum = 0x01;
    inReadRecord.recordOAD.road.oadCols.oad[0].value = 0x6e050200;

    
    stepReadRecord.MAC.nNum = 0;
    ret = db_read_record_start(gDBClientMain, &inReadRecord, &handle, &recNum);
    if(ret!=0 || recNum==0)
    {
        TASK_FMT_DEBUG(-1,TASK_LOG, "��ʼ����¼ʧ�� ret  %d recNum %d \n",ret,recNum);
    }
    else
    {
        for(i=0;i<recNum;i++)
        {
            ret = db_read_record_step(gDBClientMain, handle, &stepReadRecord, &stepRecordOut);
            if(ret!=0)
            {
                TASK_FMT_DEBUG(-1,TASK_LOG, "��������¼ʧ�� ret  %d i %d \n",ret,i);
                continue;
            }
            if(stepRecordOut.recordnum!=1 || stepRecordOut.record[0].road.oadCols.oad[0].value!=0x6E050200
                ||stepRecordOut.record[0].outDatalen!=sizeof(OOP_GROUPMETER_NEW_T))
            {
                TASK_FMT_DEBUG(-1,TASK_LOG, "������ȡ�ļ�¼������ \n");
                continue;
            }
            memcpy(&gMeterFile,stepRecordOut.record[0].outDataBuf,sizeof(OOP_GROUPMETER_NEW_T));
            if(taskmng_grop_addr_isvalid(gMeterFile.groupAddr.add)<0)
            {
                TASK_FMT_DEBUG(-1,TASK_LOG, "Ⱥ���ַ�Ƿ� \n");
                TASK_BUF_DEBUG(-1,TASK_LOG, gMeterFile.groupAddr.add,6);
                continue;
            }
            TASK_FMT_DEBUG(-1,TASK_LOG, "Ⱥ���ַ 0X%02x%02x%02x%02x%02x%02x \n",gMeterFile.groupAddr.add[0],gMeterFile.groupAddr.add[1],gMeterFile.groupAddr.add[2],gMeterFile.groupAddr.add[3],gMeterFile.groupAddr.add[4],gMeterFile.groupAddr.add[5]);
            if(taskmng_grop_addr_repect_check(pgropMeter,&gMeterFile))
            {
                TASK_FMT_DEBUG(-1,TASK_LOG, "Ⱥ���ַ�ظ� ���� \n");
                continue;
            }
            
            MeterNum=pgropMeter->nNum;
            pgropMeter->grop_meter[MeterNum]=gMeterFile;
            pgropMeter->nNum++;
            if(pgropMeter->nNum >= GROUPMETER_NUM)
            break;               //��ʱ������
       }
    }
    db_read_record_end(gDBClientMain, handle);

    MeterCount = pgropMeter->nNum;
    TASK_FMT_DEBUG(-1,TASK_LOG, "������ɺ�Ⱥ���ַ����Ϊ %d \n",MeterCount);
    pgropMeter->FileState = TASK_CFG_INITED;
    pthread_rwlock_unlock(&pgropMeter->rwLock);
    return MeterCount;    
}

/*******************************************************************************
* ��������: taskmng_addr_isvalid_check
* ��������: ����Ƿ���Ч��Ⱥ���ַ
* �������: MtrAddr             Ҫ���Ĳ������ַ
*           
* �������: ��
* �� �� ֵ:  >0                  ��Ч
*           <0                  ��Ч
*******************************************************************************/
int taskmng_grop_addr_isvalid(uint8 *MtrAddr)
{
    uint8   TestAddr[6];

    //�ж�Ⱥ���ַ�Ƿ���4���ֽ�0x88
    memset(TestAddr, 0x88, 4);
    if (memcmp(MtrAddr, TestAddr, 4) == 0)
    {
        return 1;
    }

    return -1;
}

/*******************************************************************************
* ��������: taskmng_meter_file_addr_check
* ��������: �ж��µ�Ⱥ���ַ�Ƿ�����е��ظ�
* �������: 
            pMeterFileAll   //�����
           meterFile  �����ĵ���
* �������: ��
* �� �� ֵ:   TRUE ���ظ��� FALSE û���ظ���
*******************************************************************************/
BOOL taskmng_grop_addr_repect_check(FUJIAN_grop_meter *pMeterFileAll,OOP_GROUPMETER_NEW_T *meterFile)
{
    uint16 i;
    BOOL ret = FALSE;
    for(i=0;i<pMeterFileAll->nNum;i++)
    {
        if(memcmp(meterFile->groupAddr.add,pMeterFileAll->grop_meter[i].groupAddr.add,6) == 0)
        {
            ret = TRUE;
            break;
        }
    }
    return ret;
}

/*******************************************************************************
* ��������: taskmng_645_meter_data_unpack
* ��������: ����Լ��֡
* �������: FrameLen            ֡����
*           FrameBuff           ֡����
* �������: 
*           DataLen             �����򳤶�
*           DataBuff            ����������
* �� �� ֵ: CHECK_RESULT_SUCCEED
*           CHECK_RESULT_FAILUER
*           CHECK_RESULT_CONFIRM
*           CHECK_RESULT_DENY
*******************************************************************************/
/*int8 taskmng_645_meter_data_unpack(uint16 FrameLen, uint8 *FrameBuff,uint16 *DataLen, uint8 *DataBuff)
{
    uint16  DataIndex = 0xff;                   //ָ��ƫ��
    uint8   CS = 0;
    uint16   i;//start Common Problem

    //����ƫ�ƣ���λ֡ͷ
    for (i = 0; i < FrameLen; i++)
    {
        if ((FrameBuff[i] == 0x68) && (FrameBuff[i+7] == 0x68))
        {
            DataIndex = i;
            break;
        }
    }

    //δ��λ��֡ͷ����Ϊʧ��
    if (DataIndex == 0xff)
    {
        return CHECK_RESULT_FAILUER;
    }

    //ˢ�����ݳ���
    *DataLen = FrameBuff[DataIndex+9];

    //�ж�֡β
    if (FrameBuff[(DataIndex + *DataLen + 11)] != 0x16)
    {
        return CHECK_RESULT_FAILUER;
    }

    //�ж�У���
    for (i=0; i < (*DataLen + 10); i++)
    {
        CS += FrameBuff[DataIndex+i];
    }
    if (CS != FrameBuff[(DataIndex + *DataLen + 10)])
    {
        return CHECK_RESULT_FAILUER;
    }

    //ˢ�����������ݣ�ͬʱ����3�봦��
    for (i = 0; i < *DataLen; i++)
    {
        DataBuff[i] = FrameBuff[DataIndex+10+i] - 0x33;
    }

    //�ж��Ƿ���ȷ��֡
    if (*DataLen == 0)
    {
        return CHECK_RESULT_CONFIRM;
    }
    return CHECK_RESULT_SUCCEED;
}
*/

/*******************************************************************************
* ��������: judge_buf_inter
* ��������: �Ƚϵ�ǰ�����Ƿ��ڷ�Χ��
* �������: 
            sectionless   ��Χ����ֵ
            sectionMax    ��Χ����ֵ
            buf          ƥ�䵽�����ֽں�ӿ�ʼ�ֽڵı���
            codelen      ���Ľ�ȡ����
* �������: ��
* �� �� ֵ:   TRUE ���ظ��� FALSE û���ظ���
*******************************************************************************/

BOOL judge_buf_inter(uint8 pipe,OOP_OCTETVAR256_T *sectionless,OOP_OCTETVAR256_T *sectionMax,uint8 *buf,uint32 codelen)
{
    int ret1,ret2;
    BOOL backtype = FALSE;
    ret1 = memcmp(buf,sectionless->value,codelen);
    ret2 = memcmp(buf,sectionMax->value,codelen);
    if(ret1 >=0 && ret2 <=0)
    {
        backtype = TRUE;
    }
    else
    {
        TASK_FMT_DEBUG(pipe,REC_LOG, "������֤ʧ��\n");
        TASK_FMT_DEBUG(pipe,REC_LOG, "��ǰ����\n");
        TASK_BUF_DEBUG(pipe,REC_LOG, buf,codelen);
        TASK_FMT_DEBUG(pipe,REC_LOG, "��֤��ֵ\n");
        TASK_BUF_DEBUG(pipe,REC_LOG, sectionless->value,codelen);
        TASK_FMT_DEBUG(pipe,REC_LOG, "��֤��ֵ\n");
        TASK_BUF_DEBUG(pipe,REC_LOG, sectionMax->value,codelen);
    }
    return backtype;
}
/*******************************************************************************
* ��������: normal_data_type
* ��������: ͨ������(�����������򼴸��ֽ���ǰ)
* �������: 
            datacomp   ���ݱȶԲ���
            buf        ���α���
            lastbuf    �ϴα���
            thisindex  ���α���ƫ��
            lastindex  ��һ�α���ƫ��
            add        ��ǰ����ַ
* �������: ��
* �� �� ֵ:   TRUE ��֤�ɹ� FALSE ��֤ʧ��
*******************************************************************************/

BOOL normal_data_type(uint8 pipe,OOP_DATACOMP_PARAM_T *datacomp,uint8* buf,uint8*lastbuf,uint16 thisindex,uint16 lastindex,OOP_TSA_T *add)
{

    BOOL ret = FALSE;
    OOP_OCTETVAR256_T sectionMax,sectionless;    
    uint8 i,reportnum=0,successnum=0;
    OOP_DATACOMP_SECTION_T tmpverific;
    //uint8 tmpbuf[datacomp->len] = {0};
    for(i=0;i<datacomp->num;i++)
    {
        tmpverific = datacomp->param[i];
        if(tmpverific.section1.nNum != tmpverific.section2.nNum)
            continue;
        if(tmpverific.section1.nNum != datacomp->len)
            continue;
        if(datacomp->datatype==0 || datacomp->datatype== 20 )
        {
            if(memcmp(tmpverific.section1.value,tmpverific.section2.value,datacomp->len)>= 0)  
            {
                sectionMax=tmpverific.section1;
                sectionless = tmpverific.section2;
            }
            else
            {
                sectionMax=tmpverific.section2;
                sectionless = tmpverific.section1;
            }
        }
        else
        {
            OOP_OCTETVAR256_T section1,section2;
            section1.nNum = tmpverific.section1.nNum;
            section2.nNum = tmpverific.section2.nNum;
            memcpy_r(section1.value,tmpverific.section1.value,tmpverific.section1.nNum);
            memcpy_r(section2.value,tmpverific.section2.value,tmpverific.section2.nNum);
            
            if(memcmp(section1.value,section2.value,datacomp->len)>= 0)  
            {
                sectionMax=section1;
                sectionless = section2;
            }
            else                
            {
                sectionMax=section2;
                sectionless = section1;
            }
        }
        /*�Ƚϵ�ǰ�����Ƿ��ڷ�Χ��*/
        ret = judge_buf_inter(pipe,&sectionless,&sectionMax,&buf[thisindex],datacomp->len);
        TASK_FMT_DEBUG(pipe,REC_LOG, "����index %d ������Ӧ������֤����ֵ ret %d\n",i,ret);
        //ֻ�л�ȡ�����ϴεı��Ĳ��ж�
        if(ret && lastindex >0)
        {
            successnum++;
            if(FALSE == judge_buf_inter(pipe,&sectionless,&sectionMax,&lastbuf[lastindex],datacomp->len))
            {
                if(tmpverific.taskid!=0)
                {
                    reportnum++;
                    taskmng_task_trigger(pipe,tmpverific.taskid,add);
                }
            }
        }
    
    }

    TASK_FMT_DEBUG(pipe,REC_LOG, "��֤������%d,��֤�ɹ���%d,���������ϱ���%d\n",datacomp->num,successnum,reportnum);
    return ret;
}

/*******************************************************************************
* ��������: normal_data_type
* ��������: ͨ��bitλ�жϽ�ȡ���ȹ̶�Ϊ1����Ӧλ������1���ý������Ϊ������֤ͨ��
* �������: 
            datacomp   ���ݱȶԲ���
            buf        ���α���
            lastbuf    �ϴα���
            thisindex  ���α���ƫ��
            lastindex  ��һ�α���ƫ��
            add        ��ǰ����ַ
* �������: ��
* �� �� ֵ:   TRUE ��֤�ɹ� FALSE ��֤ʧ��
*******************************************************************************/

BOOL noramal_bit_type(uint8 pipe,OOP_DATACOMP_PARAM_T *datacomp,uint8* buf,uint8*lastbuf,uint16 thisindex,uint16 lastindex,OOP_TSA_T *add)
{
    uint8 i,reportnum=0,successnum=0;
    BOOL ret = FALSE;
    OOP_DATACOMP_SECTION_T tmpverific;
    if(datacomp->len !=1)
    {
        TASK_FMT_DEBUG(pipe,REC_LOG, "normal_bit_type ��ȡ���Ȳ���1  \n");
        return ret;    
    }
    //num+=1;             /*�̶�Ϊ1*/
    for(i=0;i<datacomp->num;i++)
    {
        tmpverific = datacomp->param[i];
        if(datacomp->len != tmpverific.section1.nNum && datacomp->len != tmpverific.section1.nNum)
        {
            continue;
        }
        if((buf[thisindex]^tmpverific.section1.value[0])==1)
        {
            ret = TRUE;
            successnum++;
            if(lastindex>0 && (lastbuf[lastindex]^tmpverific.section1.value[0])==0)
            {                
                if(tmpverific.taskid!=0)
                {
                    reportnum++;
                    taskmng_task_trigger(pipe,tmpverific.taskid,add);
                }
            }
        }
    }

    TASK_FMT_DEBUG(pipe,REC_LOG, "��֤����%d��֤������%d,��֤�ɹ���%d,���������ϱ���%d\n",datacomp->datatype,datacomp->num,successnum,reportnum);

    return ret;
}



/*******************************************************************************
* ��������: abnormal_data_type_645
* ��������: DL/T645�������޷���������
* �������: 
            verific   ���ݱȶԲ���
            buf        ���α���
            buflen     ���α��ĳ���
            lastbuf    �ϴα���
            lastbuflen ��һ�α��ĳ���
            pcurrruntask  ��ǰ����
            add        ��ǰ����ַ
* �������: ��
* �� �� ֵ:   TRUE ��֤�ɹ� FALSE ��֤ʧ��
*******************************************************************************/

BOOL abnormal_data_type_645(uint8 pipe,OOP_RECV_MESS_PROC_T *verific,uint8* buf,uint16 buflen,uint8*lastbuf,uint16 lastbuflen,FUJIAN_SPECIFIC_TASK_T *pcurrruntask,OOP_TSA_T *add)
{
    /*���������ģ�������-0x33*/
    //uint8 i;
    uint8 apdu[1000];
    uint8 lastapdu[1000];
    uint16 apdulen,lastapdulen;
    uint16 lastindex,thisindex;
    OOP_DATACOMP_PARAM_T *datacomp = &verific->dataCompare;
    //uint16 tmplen = datacomp->feature.nNum;
    if(datacomp->len == 0)
        return FALSE;
    if(datacomp->datatype==11)
    {
       if(datacomp->len != 1)
        return FALSE;
    }
    apdulen = respons_645_ctrl(pipe,pcurrruntask->resMessType,buf,buflen,apdu);
    lastapdulen = respons_645_ctrl(pipe,pcurrruntask->resMessType,lastbuf,lastbuflen,lastapdu);
//    TASK_FMT_DEBUG(pipe,REC_LOG, "������Ӧ���ļ�0x33֮���ֵ\n");
//    TASK_BUF_DEBUG(pipe,REC_LOG, apdu,apdulen);
//    TASK_FMT_DEBUG(pipe,REC_LOG, "�ϴ���Ӧ���ļ�0x33֮���ֵ\n");
//    TASK_BUF_DEBUG(pipe,REC_LOG, lastapdu,lastapdulen);
    if(apdulen ==0 )
    {
        return FALSE;
    }
/*�ж�645���ģ��ȼ�0x33��Ȼ����ƥ��*/
    lastindex = taskmng_data_intercept(lastapdu,lastapdulen,verific);
    thisindex = taskmng_data_intercept(apdu,apdulen,verific);
    if(thisindex == 0)
    {
        TASK_FMT_DEBUG(pipe,REC_LOG, "δ�ҵ������ֽ�\n");
        TASK_FMT_DEBUG(pipe,REC_LOG, "���α��������� index %d �ϴα���������index%d\n",thisindex,lastindex);
        return FALSE;
    }
    if(datacomp->datatype==10)
    {
        return  normal_data_type(pipe,datacomp,apdu,lastapdu,thisindex,lastindex,add);
    }
    else 
    {
        return noramal_bit_type(pipe,datacomp,lastapdu,lastapdu,thisindex,lastindex,add);
    }
    return FALSE;
}


uint16 respons_645_ctrl(uint8 pipe,uint8 resMessType,uint8 *buf,uint16 buflen,uint8 *outbuf)
{
    uint16 index,apdulen = 0,i;
    if(buflen ==0 )
        return 0;
    if(resMessType == 0x10)  /*����֡*/
        index = 1+6+1+1;
    else
        index = 1;     
    apdulen = buf[index];
    memcpy(outbuf,&buf[index+1],apdulen);
    for(i=0;i<apdulen;i++)
    {
        outbuf[i] -=0x33;
    }
    return apdulen;
}

uint16 respons_698_ctrl(uint8 pipe,LinkLayerCtrl_t ctrl,uint8 *buf,uint16 buflen,uint8 resMessType,uint8 *outbuf)
{
    FRAME_CONVERT_T convert;
    uint8 apdu[1000];
    uint16 apdulen;
    uint16 i=0 ;
    if(buflen == 0)
        return 0;
    TASK_FMT_DEBUG(pipe,REC_LOG, "����У������ %d\n",resMessType);
    if(resMessType==0x50 || resMessType==0x53)
    {
        memset(&convert,0x00,sizeof(FRAME_CONVERT_T));
        convert.srcBuf = buf;
        convert.srcLen = buflen;
        convert.dstBuf = apdu;
        Convert_Response_Frame(pipe,&convert,0x51);
        apdulen = convert.dstLen;
        ctrl = convert.ctrl;
    }
    else
    {
        memcpy(apdu,buf,buflen);
        apdulen = buflen;
    }
    TASK_FMT_DEBUG(pipe,REC_LOG, "������%d �����־ %d\n",ctrl.ctrl,ctrl.ctrl_discode);
    if(ctrl.ctrl_discode==1)
    {
        for(i=0;i<apdulen;i++)
        {
            apdu[i]-=0x33;
        }
    }
    memcpy(outbuf,apdu,apdulen);
    return apdulen;
}

/*******************************************************************************
* ��������: abnormal_data_type_698
* ��������: DL/T698�������޷���������
* �������: 
            verific   ���ݱȶԲ���
            buf        ���α���
            buflen     ���α��ĳ���
            lastbuf    �ϴα���
            lastbuflen ��һ�α��ĳ���
            pcurrruntask  ��ǰ����
            add        ��ǰ����ַ
* �������: ��
* �� �� ֵ:   TRUE ��֤�ɹ� FALSE ��֤ʧ��
*******************************************************************************/

BOOL abnormal_data_type_698(uint8 pipe,OOP_RECV_MESS_PROC_T *verific,uint8* buf,uint16 buflen,uint8*lastbuf,uint16 lastbuflen,FUJIAN_SPECIFIC_TASK_T *pcurrruntask,OOP_TSA_T *add,LinkLayerCtrl_t ctrl)
{
    uint8 apdu[1000];
    uint8 lastapdu[1000];
    uint16 apdulen;
    uint16 lastapdulen;
    uint16 lastindex,thisindex;
    OOP_DATACOMP_PARAM_T *datacomp = &verific->dataCompare;
    //uint16 tmplen = datacomp->feature.nNum;
    if(datacomp->len == 0)
        return FALSE;
    if(datacomp->datatype==21)
    {
       if(datacomp->len != 1)
           return FALSE;
    }
    /*���698֡��ʽ�����洢�������apdu*/
  
    apdulen = respons_698_ctrl(pipe,ctrl,buf,buflen,pcurrruntask->resMessType,apdu);
    lastapdulen = respons_698_ctrl(pipe,ctrl,lastbuf,lastbuflen,pcurrruntask->resMessType,lastapdu);
//    TASK_FMT_DEBUG(pipe,REC_LOG, "apdu���� \n");
//    TASK_BUF_DEBUG(pipe,REC_LOG, apdu,apdulen);
//    TASK_FMT_DEBUG(pipe,REC_LOG, "lastapdu���� \n");
//    TASK_BUF_DEBUG(pipe,REC_LOG, lastapdu,lastapdulen);
    if(apdulen ==0)
    {
        return FALSE;
    }
    lastindex = taskmng_data_intercept(lastapdu,lastapdulen,verific);
    thisindex = taskmng_data_intercept(apdu,apdulen,verific);
    
    if(thisindex == 0)
    {
        TASK_FMT_DEBUG(pipe,REC_LOG, "δƥ�䵽�����ֽ�\n");
        TASK_BUF_DEBUG(pipe,REC_LOG, verific->dataCompare.feature.value,verific->dataCompare.feature.nNum);
        TASK_FMT_DEBUG(pipe,REC_LOG, "���α���\n");
        TASK_BUF_DEBUG(pipe,REC_LOG, apdu,apdulen);
        return FALSE;
    }
    if(datacomp->datatype == 20)
    {
        return  normal_data_type(pipe,&verific->dataCompare,apdu,lastapdu,thisindex,lastindex,add);
    }
    else
    {
         return  noramal_bit_type(pipe,&verific->dataCompare,apdu,lastapdu,thisindex,lastindex,add);
    }
    return FALSE;
}

/*******************************************************************************
* ��������: get_last_buf
* ��������: ��ȡ��һ������ 
* �������: 
           pAddTaskRunInfo   //����ָ��
           nodeid               ���������
           add                  //���ַ
* �������: lastbuf          //������һ���ɼ��ɹ��ı���
* �� �� ֵ:   0     ��ȡ��һ������,���ı���Ǵ����
              1     ��ȡ��һ�����ĳɹ������Ĵ��������ǳɹ�
              2     û����һ������(��һ�βɼ��ɹ���ֱ���ϱ�)
              3     û��ȡ��������������
*******************************************************************************/
uint8 get_last_buf(uint8 pipe,uint8* lastbuf,uint16 *lastbuflen,FUJIAN_TASK_RUN_T * pAddTaskRunInfo,uint16 nodeid,OOP_TSA_T *add)
{
    int   ret;
    uint8 tmpbuf[1000]={0};
    READ_RECORD_T      inReadRecord;
    uint32             handle = 0;
    uint32             recNum = 0;
    STEP_READ_RECORD_T stepReadRecord;
    RECORD_UNIT_T      stepRecordOut;
    OOP_TRANS_TASK_RESULT_T tmpcode;
    //OOP_DATETIME_S_T tmpstorageTime = pAddTaskRunInfo->storageTime;
//    time_t tmpTime;
//    OOP_DATETIME_S_T dateTime;
//    DT_DateTimeShort2Time(&pAddTaskRunInfo->storageTime,&tmpTime);
//    
//    //��ȡ��һ�����ڵı���ַ��ı���
//    tmpTime=tmpTime-(time_t)DT_TI2Seconds(pAddTaskRunInfo->transTask->cycle);
//    if(tmpTime<0)
//    {
//        TASK_FMT_DEBUG(pipe,REC_LOG,"tmpTime error\n");
//    }
//    DT_Time2DateTimeShort(tmpTime,&dateTime);
//    TASK_FMT_DEBUG(pipe,REC_LOG,"ȡ��һ�α��ĵĴ洢ʱ��\n");
//    TASK_FMT_DEBUG(pipe,REC_LOG,"%d\n",tmpTime);
//    fujian_datetime_s_log_write(1, REC_LOG, &dateTime);
//    tmpTime+= (8*60*60);
    memset(&inReadRecord, 0, sizeof(READ_RECORD_T));
    memset(&stepRecordOut, 0, sizeof(RECORD_UNIT_T));
    memset(&stepReadRecord,0,sizeof(STEP_READ_RECORD_T));
    memset(&tmpcode, 0, sizeof(OOP_TRANS_TASK_RESULT_T));

    inReadRecord.recordOAD.infoNum = pAddTaskRunInfo->taskid;
    inReadRecord.recordOAD.classtag = 5;
    inReadRecord.recordOAD.road.oadMain.value = 0x6E150200;
    inReadRecord.recordOAD.road.oadCols.nNum = 1;
    inReadRecord.recordOAD.road.oadCols.oad[0].value = nodeid;
    
    inReadRecord.cType = COL_TM_STORE;
    inReadRecord.cStart = 0;
    inReadRecord.cEnd = time(NULL);
#if IS_NEED_ADD_8HOUR
    inReadRecord.cEnd+=(8*60*60);

#endif
    inReadRecord.sortType = 1;

    inReadRecord.bTable = 1;
    inReadRecord.table.nNum =  6;
    memcpy(inReadRecord.table.value,add->add,6);
    stepReadRecord.MAC.nNum = 6;
    memcpy(stepReadRecord.MAC.value,add->add,6);

    ret = db_read_record_start(gAmrThreadArg[pipe].DBClientHandle, &inReadRecord, &handle, &recNum);
    if(ret!=0 )
    {
        TASK_FMT_DEBUG(-1,TASK_LOG, "��ʼ����һ�δ洢����ʧ�� ret  %d recNum %d \n",ret,recNum);
        db_read_record_end(gAmrThreadArg[pipe].DBClientHandle, handle);
        return 3;
    }
    else
    {
        if(recNum==0)
        {
            TASK_FMT_DEBUG(-1,TASK_LOG, "����һ�δ洢���ĸ���recNum %d \n",ret,recNum);
            db_read_record_end(gAmrThreadArg[pipe].DBClientHandle, handle);
            return 2;
        }
        
        ret = db_read_record_step(gAmrThreadArg[pipe].DBClientHandle, handle, &stepReadRecord, &stepRecordOut);
        if(ret!=0)
        {
            TASK_FMT_DEBUG(-1,TASK_LOG, "��������һ�δ洢����ʧ�� ret  %d \n",ret);
            db_read_record_end(gAmrThreadArg[pipe].DBClientHandle, handle);
            return 3;
        }
        
   /*     if(stepRecordOut.recordnum!=1 || stepRecordOut.record[0].road.oadCols.oad[0].value!=nodeid
            ||stepRecordOut.record[0].outDatalen!=sizeof(OOP_TRANS_TASK_RESULT_T))
        {
            TASK_FMT_DEBUG(-1,TASK_LOG, "��������һ�δ洢���������� \n");
            db_read_record_end(gDBClientMain, handle);
            return 3;
        }*/
        memcpy(&tmpcode,stepRecordOut.record[0].outDataBuf,sizeof(OOP_TRANS_TASK_RESULT_T));

        memcpy(tmpbuf,tmpcode.transMsg.resultData.value,tmpcode.transMsg.resultData.nNum);

    }
    db_read_record_end(gAmrThreadArg[pipe].DBClientHandle, handle);
    *lastbuflen = tmpcode.transMsg.resultData.nNum;
    memcpy(lastbuf,tmpbuf,*lastbuflen);
    
    return 1;
}

/*******************************************************************************
* ��������: taskmng_data_intercept
* ��������: ���ձ��ĶԱȲ�����buf�л�ȡ��ȡ�ֶ� 
* �������: buf   Ҫ�Աȵı���
           buflen ���ĳ���
           messCompare ���ĶԱȲ���
* �������: 
* �� �� ֵ:  0 ûƥ�䵽
            ��������buf�е�����
*******************************************************************************/
uint16 taskmng_data_intercept(uint8 *buf,uint16 buflen,OOP_RECV_MESS_PROC_T * verific)
{
    uint16 i;
    if(buflen==0)
        return 0;
    //ֻ�Աȵ������ֽڳ���
    switch (verific->type)
        {
            case 1:
            case 2:
            {
                if(buflen<=verific->messCompare.feature.nNum)
                break;
                for(i=0;i<buflen-verific->messCompare.feature.nNum+1;i++)
                {
                    if(memcmp(buf+i,verific->messCompare.feature.value,verific->messCompare.feature.nNum) == 0)
                    {
                        return i+verific->messCompare.feature.nNum+verific->messCompare.start-1;
                    }   
                }
            }break;
            case 3:
            {
                if(buflen<=verific->dataCompare.feature.nNum)
                break;
                for(i=0;i<buflen-verific->dataCompare.feature.nNum+1;i++)
                {
                    if(memcmp(buf+i,verific->dataCompare.feature.value,verific->dataCompare.feature.nNum) == 0)
                    {
                        return i+verific->dataCompare.feature.nNum+verific->dataCompare.start-1;
                    }   
                }
            }break;
        }
    return 0;
}

/*******************************************************************************
* ��������: verific_type1
* ��������: ���ıȶԲ��� 
* �������: 
           verific ���ر�����֤��ʽ
           buf   Ҫ�Աȵı���
           buflen ���ĳ���
           pAddTaskRunInfo ���񷽰�����
           nodeid          ��ǰ���������
           add             tsa ����ַ

* �������: 
* �� �� ֵ:  
            true : ��֤�ɹ�
            FALSE�� ��֤ʧ��
*******************************************************************************/

uint8 verific_type1(uint8 pipe,OOP_RECV_MESS_PROC_T *verific,uint8* buf,uint16 buflen,FUJIAN_TASK_RUN_T * pAddTaskRunInfo,uint16 nodeid, OOP_TSA_T *add)
{
    uint8 ret = 0;
    uint8 lastbuf[1000] = {0};
    uint16 lastbuflen;
    uint16 lastindex,thisindex;
    /*��ȡ��һ�βɼ��ɹ��ı��ģ���ʼ��Ϊ0��˵������Ϊ��һ�βɼ���ֱ���ϱ� return 1*/
    ret = get_last_buf(pipe,lastbuf,&lastbuflen,pAddTaskRunInfo,nodeid,add);
    if(ret == 3)
    {
        TASK_FMT_DEBUG(pipe,REC_LOG, "����һ������ʧ��\n");
        return ret;
    }
    else if(ret == 2)
    {
        //��Ϊ����Ϊ��һ�� Ĭ����ȷ
        return ret;
    }
    TASK_FMT_DEBUG(pipe,REC_LOG, "�ñ���һ�γ�������\n");
    TASK_BUF_DEBUG(pipe,REC_LOG, lastbuf,lastbuflen);
    //TASK_FMT_DEBUG(pipe,REC_LOG, "�ñ��γ�������\n");
    //TASK_BUF_DEBUG(pipe,REC_LOG, buf,buflen);
    lastindex = taskmng_data_intercept(lastbuf,lastbuflen,verific);
    thisindex = taskmng_data_intercept(buf,buflen,verific);
    //û�ҵ������ֽڿ�������һ�ֳ�ʧ���� ��һ����Ϊ�״γ� ���ϱ�
    if(lastindex==0 || thisindex == 0)
    {
        TASK_FMT_DEBUG(pipe,REC_LOG, "δ�ҵ������ֽ�\n");
        return 2;
    }

    if(memcmp(&buf[thisindex],&lastbuf[lastindex],verific->messCompare.len) == 0)
    {
        return 1;
    }
    else
    {
        TASK_FMT_DEBUG(pipe,REC_LOG, "������֤��ͨ��,��ǰ���Ľ�ȡ���ֽ�\n");
        TASK_BUF_DEBUG(pipe,REC_LOG, &buf[thisindex],verific->messCompare.len);
        TASK_FMT_DEBUG(pipe,REC_LOG, "��һ�α��Ľ�ȡ���ֽ�\n");
        TASK_BUF_DEBUG(pipe,REC_LOG, &lastbuf[lastindex],verific->messCompare.len);
        return 0;
    }

}
/*******************************************************************************
* ��������: verific_type3
* ��������: �����жϲ��� 
* �������: 
           verific ���ر�����֤��ʽ
           buf   Ҫ�Աȵı���
           buflen ���ĳ���
           pAddTaskRunInfo ���񷽰�����
           currruntask     ��ǰ��������
           add             tsa ����ַ
           ctrl            ������
* �������: 
* �� �� ֵ:  
            true : ��֤�ɹ�
            FALSE�� ��֤ʧ��
*******************************************************************************/

BOOL verific_type3(uint8 pipe,OOP_RECV_MESS_PROC_T *verific,uint8* buf,uint16 buflen,FUJIAN_TASK_RUN_T *pAddTaskRunInfo,FUJIAN_SPECIFIC_TASK_T * currruntask, OOP_TSA_T *add,LinkLayerCtrl_t ctrl)
{
    uint8 ret;
    uint8 lastbuf[1000]={0};
    uint16 lastbuflen = 0;
    //uint32 tmplen;
    uint16 lastindex,thisindex;
    OOP_DATACOMP_PARAM_T *tmpdataparam = &verific->dataCompare;
    //tmplen = tmpdataparam->feature.nNum;
    if(buflen==0)
    {
        return FALSE;
    }
    ret = get_last_buf(pipe,lastbuf,&lastbuflen,pAddTaskRunInfo,currruntask->nodeid,add);

    if(ret == 3)
    {
        TASK_FMT_DEBUG(-1,TASK_LOG, "��һ�α��Ļ�ȡʧ��\n");
//        return FALSE;
    }
    else if(ret == 2)
    {
        //��Ϊ����Ϊ��һ�� Ĭ����ȷ
        TASK_FMT_DEBUG(pipe,REC_LOG, "��ǰΪ��һ������\n");
//        if(verific->messCompare.taskid !=0)   //�����ϱ������Ų��ϱ�
//        {
//            taskmng_task_trigger(pipe,verific->messCompare.taskid,add);
//        }
//        return TRUE;
    }
    TASK_FMT_DEBUG(pipe,REC_LOG, "��һ�α��Ļ�ȡ�ɹ�\n");
    TASK_BUF_DEBUG(pipe,REC_LOG, lastbuf,lastbuflen);
    if(tmpdataparam->datatype<10)              /*����Ҫ�Ա��Ľ��д���*/
    {
        lastindex = taskmng_data_intercept(lastbuf,lastbuflen,verific);
        thisindex = taskmng_data_intercept(buf,buflen,verific);
        
        if(thisindex == 0)
        {
            TASK_FMT_DEBUG(pipe,REC_LOG, "δ�ҵ������ֽ�\n");
            return 3;
        }

    }
    
    switch (tmpdataparam->datatype)
        {
            case 0:
            {   
                normal_data_type(pipe,tmpdataparam,buf,lastbuf,thisindex,lastindex,add);
            }   
                break;
            case 1:
            {
                normal_data_type(pipe,tmpdataparam,buf,lastbuf,thisindex,lastindex,add);
            }
                break;
            case 2:
            {
                noramal_bit_type(pipe,tmpdataparam,buf,lastbuf,thisindex,lastindex,add);
            }
                break;
            case 10:
            case 11:
            {
                abnormal_data_type_645(pipe,verific,buf,buflen,lastbuf,lastbuflen,currruntask,add);
            } break;            
            case 20:
            case 21:
            {
                abnormal_data_type_698(pipe,verific,buf,buflen,lastbuf,lastbuflen,currruntask,add,ctrl);
            }break;
                
            default:
                return FALSE;
                break;
        }
    return TRUE;
}

/*******************************************************************************
* ��������: ���ر�����֤
* �������:           inMS          //���������Ļ�ȡ�ĵ����Ϣ
            pMsInfo                 //���������Ϣ
* �������: ��
* �� �� ֵ: 0      //��֤��ͨ��
            1      //��֤ͨ��
            2      //�״λ�ñ���
*******************************************************************************/

uint8 packet_verific_Ispass(uint8 pipe,OOP_RECV_MESS_PROC_T *verific,FRAME_CONVERT_T *convert,FUJIAN_TASK_RUN_T *pAddTaskRunInfo,FUJIAN_SPECIFIC_TASK_T *currruntask, OOP_TSA_T *add)
{
    uint8 *buf = convert->dstBuf;
    uint16 buflen = convert->dstLen;
    LinkLayerCtrl_t ctrl = convert->ctrl;
    if(buflen==0)
        return FALSE;
    if(verific->type<3 || verific->type == 4)
        TASK_FMT_DEBUG(pipe,REC_LOG, "������֤��ʽ%d \n",verific->type);
    else
        TASK_FMT_DEBUG(pipe,REC_LOG, "������֤��ʽ%d,������֤��ʽ %d \n",verific->type,verific->dataCompare.datatype);    
    uint8 ret;
    switch (verific->type)
        {
            case 0:
            {
                return 1;
            }
                break;
            case 1:
            {
                ret = verific_type1(pipe,verific,buf,buflen,pAddTaskRunInfo,currruntask->nodeid,add);
                if(ret == 0)
                {
                     if(verific->messCompare.taskid !=0)   //�����ϱ������Ų��ϱ�
                     {
                        taskmng_task_trigger(pipe,verific->messCompare.taskid,add);
                     }
                
                     return 1;
                }
                else if(ret == 2 )
                {
                    return ret;                          //�״β���������,������֤���Ϊ��ȷ
                }
                    
            }
                break;
            case 2:
            {
                ret = verific_type1(pipe,verific,buf,buflen,pAddTaskRunInfo,currruntask->nodeid,add);
                if(ret==1)
                {
                     if(verific->messCompare.taskid !=0)   //�����ϱ������Ų��ϱ�
                     {
                        taskmng_task_trigger(pipe,verific->messCompare.taskid,add);
                     }
                
                     return 1;
                }
                else if(ret == 2)
                {
                    return ret;                               // �״β���������
                }
            }
                break;
            case 3:                           /*����������������֤ͨ�����ϱ�*/
            {
                verific_type3(pipe,verific,buf,buflen,pAddTaskRunInfo,currruntask,add,ctrl);
                return 1;
            }
                break;
            case 4:
                return 0;
                break;
            default :
                return 0;
                break;
        }
    return 0;
}

/*******************************************************************************
* ��������: taskmng_trans_task_cfg_init
* ��������: ����ṹ���ʼ��
* �������: 
            
* �������: ��
* �� �� ֵ:   ��
*******************************************************************************/
void taskmng_trans_task_cfg_init()
{
    memset(&gTransTaskCfg, 0x00, sizeof(FUJIAN_TASK_CFG_LIST_T));
    pthread_mutex_init(&gTransTaskCfg.Lock, NULL);
    gTransTaskCfg.TaskCfgState = TASK_CFG_UNINIT;

    memset(&gSpecificTaskList, 0X00, sizeof(FUJIAN_SPECIFIC_TASK_LIST_T));

    memset(&gTaskCollectionState, 0x00, sizeof(FUJIAN_TASK_COLLECTION_STATE_T));
    pthread_mutex_init(&gTaskCollectionState.lock, NULL);
}

/*******************************************************************************
* ��������: taskmng_trans_task_init
* ��������: ���������ļ���ȫ��͸�����񷽰�
* �������: 
            
* �������: ��
* �� �� ֵ:   ��
*******************************************************************************/
void taskmng_trans_task_init()
{
    int             ret;
    uint32          handle = 0;
    uint32          recNum = 0;
    uint16          i;
    uint16          taskNum = 0;
    READ_RECORD_T   inReadRecord;
    STEP_READ_RECORD_T stepReadRecord;
    RECORD_UNIT_T   stepRecordOut;
    OOP_TRANS_TASK_NODE_T specificTask;

    memset(&inReadRecord, 0x00, sizeof(READ_RECORD_T));
    memset(&stepReadRecord, 0, sizeof(STEP_READ_RECORD_T));
    memset(&stepRecordOut, 0, sizeof(RECORD_UNIT_T));
    inReadRecord.recordOAD.logicId = 0;
    inReadRecord.recordOAD.infoNum = 0xffff;
    inReadRecord.cType = COL_NO_INFO;
    inReadRecord.cStart = 1;
    inReadRecord.cEnd = 65535;
    inReadRecord.sortType = UP_SORT;
    inReadRecord.recordOAD.road.oadMain.value = 0;
    inReadRecord.recordOAD.road.oadCols.nNum = 0x01;
    inReadRecord.recordOAD.road.oadCols.oad[0].value = 0x6E110200;
    stepReadRecord.MAC.nNum = 0;

    ret = db_read_record_start(gDBClientTask, &inReadRecord, &handle, &recNum);

    if(ret != 0 || recNum == 0)
    {
        TASK_FMT_DEBUG(-1, TASK_LOG, "��ʼ������ʧ�� ret %d recNum %d \n", ret, recNum);
    }
    else
    {
        for(i = 0; i < recNum; i++)
        {
            ret = db_read_record_step(gDBClientTask, handle, &stepReadRecord, &stepRecordOut);

            if(ret != 0)
            {
                TASK_FMT_DEBUG(-1, TASK_LOG, "����������ʧ�� ret %d i %d \n", ret, i);
                continue;
            }

            if(stepRecordOut.recordnum != 1 || stepRecordOut.record[0].road.oadCols.oad[0].value != 0x6E110200 ||
                 stepRecordOut.record[0].outDatalen != sizeof(OOP_TRANS_TASK_T))
            {
                TASK_FMT_DEBUG(-1, TASK_LOG, "������ȡ�������¼������ \n");
                continue;
            }

            memcpy(&gTransTaskCfg.TaskCfg[taskNum], stepRecordOut.record[0].outDataBuf, 
                sizeof(OOP_TRANS_TASK_T));
            TASK_FMT_DEBUG(-1, TASK_LOG, "��ȡ��ȫ��͸�����񷽰� %d �˿� %08x ������� %d ���ȼ� %d\n",
                 
                gTransTaskCfg.TaskCfg[taskNum].taskid, 
                gTransTaskCfg.TaskCfg[taskNum].oad.value, gTransTaskCfg.TaskCfg[taskNum].taskTypeData.taskType, 
                gTransTaskCfg.TaskCfg[taskNum].priority);
            taskNum++;

            if(taskNum >= MAX_TRANS_TASK_NUM)
                break;
        }
    }

    gTransTaskCfg.TaskNum = taskNum;
    TASK_FMT_DEBUG(-1, TASK_LOG, "ȫ��͸�����񷽰����� %d \n", taskNum);
    db_read_record_end(gDBClientTask, handle);

    //�������о�������F102  
    /*  ��Ϣ��� ͸�����񷽰���
        ��oadΪ������������
        ��oad 0x6E130200
        �������ʶ�� 2���ɳ�ʼ������)
        ��ȡ����չf012��ȡ�����ַ��0
    */
    taskNum = 0;
    memset(&inReadRecord, 0x00, sizeof(READ_RECORD_T));
    memset(&stepReadRecord, 0, sizeof(STEP_READ_RECORD_T));
    memset(&stepRecordOut, 0, sizeof(RECORD_UNIT_T));
    memset(&specificTask, 0x00, sizeof(OOP_TRANS_TASK_NODE_T));
    inReadRecord.recordOAD.logicId = 0;
    inReadRecord.recordOAD.infoNum = 0xffff;
    inReadRecord.cType = COL_NO_INFO;
    inReadRecord.cStart = 1;
    inReadRecord.cEnd = 65535;
    inReadRecord.sortType = UP_SORT;
    inReadRecord.recordOAD.road.oadMain.value = 0x6E130200;
    //������ ֱ����0����oad
    inReadRecord.recordOAD.road.oadCols.nNum = 0;
    //inReadRecord.recordOAD.road.oadCols.oad[0].value = 0;
    inReadRecord.recordOAD.classtag = CLASS_DATA_INIT;
    inReadRecord.bTable = 1;
    inReadRecord.table.nNum = 0;
    stepReadRecord.MAC.nNum = 0;

    ret = db_read_record_start(gDBClientTask, &inReadRecord, &handle, &recNum);

    if(ret != 0 || recNum == 0)
    {
        TASK_FMT_DEBUG(-1, TASK_LOG, "��ʼ����������ʧ�� ret %d recNum %d \n", ret, recNum);
    }
    else
    {
        for(i = 0; i < recNum; i++)
        {
            ret = db_read_record_step(gDBClientTask, handle, &stepReadRecord, &stepRecordOut);

            if(ret != 0)
            {
                TASK_FMT_DEBUG(-1, TASK_LOG, "��������������ʧ�� ret %d i %d \n", ret, i);
                continue;
            }

            if(stepRecordOut.recordnum != 1 || stepRecordOut.record[0].road.oadMain.value != 0x6E130200 ||
                 stepRecordOut.record[0].outDatalen != sizeof(OOP_TRANS_TASK_NODE_T))
            {
                TASK_FMT_DEBUG(-1, TASK_LOG, "������ȡ�ľ��������¼������ \n");
                continue;
            }

            memcpy(&specificTask, stepRecordOut.record[0].outDataBuf, sizeof(OOP_TRANS_TASK_NODE_T));
            TASK_FMT_DEBUG(-1, TASK_LOG, "��ȡ���������� ���񷽰��� %d ����������� %d \n", 
                specificTask.transTaskid, specificTask.nodeid);
            gSpecificTaskList.taskList[taskNum].transTaskid = specificTask.transTaskid;
            gSpecificTaskList.taskList[taskNum].nodeid = specificTask.nodeid;

            //��ȡ�������ͨ�ŵ�ַ���� ���255��
            gSpecificTaskList.taskList[taskNum].addrAll.num = taskmng_FJ_ms_list_get(&specificTask.ms,&gSpecificTaskList.taskList[taskNum].addrAll);
            gSpecificTaskList.taskList[taskNum].uartParam = specificTask.uartParam;
            gSpecificTaskList.taskList[taskNum].reqMessType = specificTask.reqMessType;
            gSpecificTaskList.taskList[taskNum].reqMess = specificTask.reqMess;
            gSpecificTaskList.taskList[taskNum].resMessType = specificTask.resMessType;
            gSpecificTaskList.taskList[taskNum].resMaxLen = specificTask.resMaxLen;
            gSpecificTaskList.taskList[taskNum].resMinLen = specificTask.resMinLen;
            gSpecificTaskList.taskList[taskNum].resMessProc = specificTask.resMessProc;
            gSpecificTaskList.taskList[taskNum].next = NULL; //���Ե��̳߳�ʼ����ʱ�����ֵ
            taskNum++;

            if(taskNum >= MAX_SPECIFIC_TASK_NUM)
                break;
        }
    }

    gSpecificTaskList.taskNum = taskNum;
    TASK_FMT_DEBUG(-1, TASK_LOG, "����������� %d \n", taskNum);
    db_read_record_end(gDBClientTask, handle);
}


/*******************************************************************************
* ��������: taskmng_trans_task_db_update_check
* ��������: ����Ƿ��յ�������صı���¼�
* �������: 

* �������: ��
* �� �� ֵ:   ��
*******************************************************************************/
void taskmng_trans_task_db_update_check(DB_UPDATE_T * dbupdate)
{
    int             Recvlen = 0;

    Recvlen = taskmng_db_update_queue_read(&pTaskDBUpdate, dbupdate);

    if(Recvlen > 0)
    {
        TASK_FMT_DEBUG(-1, TASK_LOG, "�յ�mqtt���͵ĸ�����Ϣ logicid %d  infonum %d oad %08x !\n", 
            dbupdate->logicid, dbupdate->infonum, dbupdate->oad);
        gTransTaskCfg.TaskCfgState = TASK_CFG_MODIFY;
    }
}


/*******************************************************************************
* ��������: taskmng_trans_task_collection_state_load
* ��������://���������ļ���͸�����񷽰��ɼ�״̬��
* �������:

* �������: 
* �� �� ֵ:
*******************************************************************************/
void taskmng_trans_task_collection_state_load()
{
    uint16          i;
    uint16          TaskId;
    int             ret;
    OOP_TRANS_TASK_RLUESTATE_T data;

    pthread_mutex_lock(&gTaskCollectionState.lock);

    gTaskCollectionState.taskNum = gTransTaskCfg.TaskNum;
    memset(gTaskCollectionState.taskState,0x00,sizeof(gTaskCollectionState.taskState));
    TASK_FMT_DEBUG(-1, TASK_LOG, "����͸�����񷽰��ɼ�״̬�� ��������� %d \n", 
        gTaskCollectionState.taskNum);
    
    RECORD_UNIT_T RecordOut;
    memset(&RecordOut,0x00,sizeof(RECORD_UNIT_T));
    READ_RECORD_T ReadRecord;
    memset(&ReadRecord,0x00,sizeof(READ_RECORD_T));
    
    ReadRecord.recordOAD.classtag = 5;
    ReadRecord.recordOAD.road.oadMain.value = 0;
    ReadRecord.recordOAD.road.oadCols.nNum = 1;
    ReadRecord.recordOAD.road.oadCols.oad[0].value = 0x6E200200;

    for(i = 0; i < gTransTaskCfg.TaskNum; i++)
    {
        TaskId = gTransTaskCfg.TaskCfg[i].taskid;
        //�����һ����¼
        ReadRecord.recordOAD.infoNum = TaskId;
        ReadRecord.cType = COL_TM_STORE;
        ReadRecord.cStart = 0;
        ReadRecord.cEnd = time(NULL);
        ReadRecord.sortType = DOWN_SORT;
        
        ret = db_read_record(gDBClientTask,&ReadRecord,DATA_LEN_MAX,&RecordOut);
        if(ret == 0 && RecordOut.record[0].outDatalen == sizeof(OOP_TRANS_TASK_RLUESTATE_T))
        {
            memcpy(&data,RecordOut.record[0].outDataBuf,sizeof(OOP_TRANS_TASK_RLUESTATE_T));
            TASK_FMT_TRACE(-1, TASK_LOG, "��ȡ���ɼ�״̬�� taskid %d send %d recv %d \n",TaskId,data.sendNum,data.receNum);
            memcpy(&gTaskCollectionState.taskState[i], &data, sizeof(OOP_TRANS_TASK_RLUESTATE_T));

        }else
        {
            TASK_FMT_DEBUG(-1, TASK_LOG, "��ȡ���� %d �ɼ�״̬ʧ�� ret %d \n", TaskId, ret);
            gTaskCollectionState.taskState[i].transTaskid = TaskId;
        }

    }

    pthread_mutex_unlock(&gTaskCollectionState.lock);
}


/*******************************************************************************
* ��������: taskmng_trans_task_changeflag_set
* ��������: �յ��������� �������źͶ˿� ����Ӧ�̵߳Ķ�Ӧ������λ �������̵߳�
    ��ʱ��
* �������:taskid ���񷽰���
            port ����˿�
            flag Ҫ��λ�ı�� 2��ʾ�޸� 3��ʾɾ��
* �������: 
* �� �� ֵ:
*******************************************************************************/
void taskmng_trans_task_changeflag_set(uint16 taskid, OOP_OAD_U port, uint8 flag)
{
    uint8           pipe = taskmng_mtr_pipe_get(&port);

    
    if(pipe > AMR_THREAD_NUM)
        return;

    FUJIAN_TASK_RUN_LIST_T * taskRunList = NULL;
    uint16          i;

    taskRunList = &gAmrThreadArg[pipe].taskRunList;

    for(i = 0; i < taskRunList->taskNum; i++)
    {
        if(taskRunList->taskList[i].taskid == taskid)
        {
            taskRunList->taskList[i].changeFlag = flag;
            TASK_FMT_DEBUG(-1, TASK_LOG, "�˿�%08x �ҵ����� %d ��λ %d\n", port.value, taskid, flag);
            return;
        }
    }

    TASK_FMT_DEBUG(-1, TASK_LOG, "�˿�%08x δ�ҵ����� %d ��λ %d\n", port.value, taskid, flag);
}

/*******************************************************************************
* ��������: taskmng_trans_task_record_file_delete
* ��������: �������źͶ˿� ֪ͨ�����߳�ɾ�������¼�ļ�
    ��ʱ��
* �������:taskid ���񷽰���
            port ����˿�
            flag Ҫ��λ�ı�� 1��ʾ�޸� 2��ʾɾ��
* �������: 
* �� �� ֵ:
*******************************************************************************/
void taskmng_trans_task_record_file_delete(uint16 taskid, OOP_OAD_U port)
{
    uint8           pipe = taskmng_mtr_pipe_get(&port);

    //Ŀǰֻ֧��һ·�ز���4·485
    if(pipe > AMR_THREAD_NUM)
        return;

    int ret;
    DB_UPDATE_T dbupdate;
    memset(&dbupdate,0x00,sizeof(DB_UPDATE_T));
    dbupdate.infonum = taskid;
    ret = taskmng_db_update_queue_write( &pTaskRecordFileDel[pipe], &dbupdate);

    TASK_FMT_DEBUG(-1, TASK_LOG, "�˿�%08x ֪ͨ�����߳�ɾ������ %d ��¼�ļ� ret %d\n", port.value, taskid, ret);
}

/*******************************************************************************
* ��������: taskmng_trans_task_info_read
* ��������: //����͸�����񷽰��Ż�ȡ������Ϣ
* �������: Taskid �����
* �������: TaskInfo ��ȡ����������
          
* �� �� ֵ: -1 ��ȡʧ��
        0  ��ȡ�ɹ�
*******************************************************************************/
int taskmng_trans_task_info_read(uint16 Taskid, OOP_TRANS_TASK_T * TaskInfo)
{
    int             ret;
    uint32          len = 0;
    NOMAL_OAD_T     NormalOad;

    memset(&NormalOad, 0x00, sizeof(NOMAL_OAD_T));
    NormalOad.oad.value = 0x6E110200;
    NormalOad.infoNum = Taskid;

    ret = db_read_nomal(gDBClientTask, &NormalOad, sizeof(OOP_TRANS_TASK_T), (uint8 *) TaskInfo, &len);

    if(ret != 0 || len != sizeof(OOP_TRANS_TASK_T))
        return - 1;

    return 0;
}

/*******************************************************************************
* ��������: taskmng_trans_specific_task_info_read
* ��������: //���ݾ�������Ż�ȡ������Ϣ
* �������: Taskid ���������
* �������: TaskInfo ��ȡ����������
          
* �� �� ֵ: -1 ��ȡʧ��
        0  ��ȡ�ɹ�
*******************************************************************************/
int taskmng_trans_specific_task_info_read(uint16 Taskid,OOP_OAD_U oad, OOP_TRANS_TASK_NODE_T * TaskInfo)
{
    int             ret;
    uint32          handle = 0;
    uint32          recNum = 0;
    READ_RECORD_T   inReadRecord;
    STEP_READ_RECORD_T stepReadRecord;
    RECORD_UNIT_T   stepRecordOut;
    memset(&inReadRecord, 0x00, sizeof(READ_RECORD_T));
    memset(&stepReadRecord, 0, sizeof(STEP_READ_RECORD_T));
    memset(&stepRecordOut, 0, sizeof(RECORD_UNIT_T));
    inReadRecord.recordOAD.logicId = 0;
    inReadRecord.recordOAD.infoNum = Taskid;
    inReadRecord.cType = COL_NO_INFO;
    inReadRecord.cStart = Taskid;
    inReadRecord.cEnd = Taskid+1;
    inReadRecord.sortType = UP_SORT;
    inReadRecord.recordOAD.road.oadMain.value = 0x6E130200;
    inReadRecord.recordOAD.road.oadCols.nNum = 0x01;
    inReadRecord.recordOAD.road.oadCols.oad[0].value = oad.value;
    inReadRecord.recordOAD.classtag = CLASS_DATA_INIT;
    inReadRecord.bTable = 1;
    inReadRecord.table.nNum = 0;
    stepReadRecord.MAC.nNum = 0;

    ret = db_read_record_start(gDBClientTask, &inReadRecord, &handle, &recNum);

    if(ret != 0 || recNum != 1)
    {
        TASK_FMT_DEBUG(-1, TASK_LOG, "��ʼ����������ʧ�� ret %d recNum %d \n", ret, recNum);
    }
    else
    {
        ret = db_read_record_step(gDBClientTask, handle, &stepReadRecord, &stepRecordOut);

        if(ret != 0)
        {
            TASK_FMT_DEBUG(-1, TASK_LOG, "��������������ʧ�� ret %d \n", ret);
            return -1;
        }

        if(stepRecordOut.recordnum != 1 || stepRecordOut.record[0].road.oadMain.value != 0x6E130200 ||
             stepRecordOut.record[0].outDatalen != sizeof(OOP_TRANS_TASK_NODE_T))
        {
            TASK_FMT_DEBUG(-1, TASK_LOG, "������ȡ�ľ��������¼������ \n");
            return -1;
        }

        memcpy(TaskInfo, stepRecordOut.record[0].outDataBuf, sizeof(OOP_TRANS_TASK_NODE_T));
        TASK_FMT_DEBUG(-1, TASK_LOG, "��ȡ���������� ���񷽰��� %d ����������� %d \n", 
            TaskInfo->transTaskid, TaskInfo->nodeid);
    }

   
    db_read_record_end(gDBClientTask, handle);
    return 0;
}

/*******************************************************************************
* ��������: taskmng_trans_task_change_fun 
* ��������:͸�����񷽰��������
* �������: 
          choice 0 ɾ�� 1 ���
          TaskInfo �����������Ϣ
* �������: 
* �� �� ֵ: TRUE �������ı� FALSE ��������ı�
*******************************************************************************/
BOOL taskmng_trans_task_change_fun(uint8 choice, OOP_TRANS_TASK_T * taskInfo)
{
    uint16          i;
    int             index = -1;

    //����ɾ����
    if(choice == 0)
    {
        for(i = 0; i < gTransTaskCfg.TaskNum; i++)
        {
            if(gTransTaskCfg.TaskCfg[i].taskid == taskInfo->taskid)
            {
                index = i;
                //�����޸��˾Ͱ������¼�ļ�ɾ��

                taskmng_trans_task_record_file_delete(gTransTaskCfg.TaskCfg[i].taskid, gTransTaskCfg.TaskCfg[i].oad);
                //�ȸ������߳��ñ�� �������������
                taskmng_trans_task_changeflag_set(gTransTaskCfg.TaskCfg[i].taskid, gTransTaskCfg.TaskCfg[i].oad, 3);
                memset(&gTransTaskCfg.TaskCfg[i], 0x00, sizeof(OOP_TRANS_TASK_T));
                break;
            }
        }

        //�������ǰŲ
        if(index != -1)
        {
            for(i = index; i < gTransTaskCfg.TaskNum - 1; i++)
            {
                gTransTaskCfg.TaskCfg[i] = gTransTaskCfg.TaskCfg[i + 1];
            }

            memset(&gTransTaskCfg.TaskCfg[gTransTaskCfg.TaskNum - 1], 0x00, sizeof(OOP_TRANS_TASK_T));
            gTransTaskCfg.TaskNum--;
            return TRUE;
        }
        

        return FALSE;
    }
    else
    {
        //�ж������������޸�
        for(i = 0; i < gTransTaskCfg.TaskNum; i++)
        {
            if(gTransTaskCfg.TaskCfg[i].taskid == taskInfo->taskid)
            {
                index = i;
                //�ж����޸ĵ��ǲ��Ǿͱ������ȼ�Ϊ254 
                if(taskInfo->priority == 254)
                {
                    gTransTaskCfg.TaskCfg[i].priority = 254;
                    if(memcmp(&gTransTaskCfg.TaskCfg[index],taskInfo,sizeof(OOP_TRANS_TASK_T))==0)
                    {
                        TASK_FMT_DEBUG(-1, TASK_LOG, "���񷽰� %d ��ͣ \n", taskInfo->taskid);
                        return FALSE;
                    }
                }
                //�ж����ǲ���ֻ�޸������ȼ�
                if(gTransTaskCfg.TaskCfg[i].priority != taskInfo->priority)
                {               
                    gTransTaskCfg.TaskCfg[i].priority = taskInfo->priority;
                    if(memcmp(&gTransTaskCfg.TaskCfg[index],taskInfo,sizeof(OOP_TRANS_TASK_T))==0)
                    {
                        TASK_FMT_DEBUG(-1, TASK_LOG, "���񷽰� %d ���ȼ����Ϊ %d \n", taskInfo->taskid,taskInfo->priority);
                        fujian_task_priority_change_notify(taskInfo);
                        return FALSE;
                    }

                }

                taskmng_trans_task_record_file_delete(gTransTaskCfg.TaskCfg[i].taskid, gTransTaskCfg.TaskCfg[i].oad);
                //�����޸� �������߳��ñ�� 
                taskmng_trans_task_changeflag_set(gTransTaskCfg.TaskCfg[i].taskid, gTransTaskCfg.TaskCfg[i].oad, 2);
                break;
            }
        }

        //���������񷽰�
        if(index == -1)
        {
            index = gTransTaskCfg.TaskNum;
            gTransTaskCfg.TaskNum++;
        }
        memcpy(&gTransTaskCfg.TaskCfg[index],taskInfo,sizeof(OOP_TRANS_TASK_T));
        return TRUE;
    }
}

/*******************************************************************************
* ��������: taskmng_trans_specific_task_change_fun 
* ��������:�������񷽰��������
* �������: 
          choice 0 ɾ�� 1 ���
          TaskInfo �����������Ϣ
* �������: 
* �� �� ֵ: TRUE �������ı� FALSE ��������ı�
*******************************************************************************/
BOOL taskmng_trans_specific_task_change_fun(uint8 choice, OOP_TRANS_TASK_NODE_T * taskInfo)
{
    uint16          i;
    int             index = -1;
    uint16 taskid;

    //��������ɾ���� �������������ҵ���� �ٸ������񷽰���֪ͨɾ�������¼�ļ�
    if(choice == 0)
    {
        for(i=0;i<gSpecificTaskList.taskNum;i++)
        {
            if(gSpecificTaskList.taskList[i].nodeid == taskInfo->nodeid && gSpecificTaskList.taskList[i].transTaskid == taskInfo->transTaskid)
            {
                index = i;
                taskid = gSpecificTaskList.taskList[i].transTaskid; //�ҵ����񷽰��ſ���û�ж�Ӧ�����񷽰�
                break;
            }
        }
        if(index!=-1)
        {
            for(i = index;i<gSpecificTaskList.taskNum-1;i++)
            {
                gSpecificTaskList.taskList[i] = gSpecificTaskList.taskList[i+1];
            }
            memset(&gSpecificTaskList.taskList[gSpecificTaskList.taskNum-1],0x00,sizeof(FUJIAN_SPECIFIC_TASK_T));
            gSpecificTaskList.taskNum--;
            
            for(i=0;i<gTransTaskCfg.TaskNum; i++)
            {
                if(gTransTaskCfg.TaskCfg[i].taskid == taskid)
                {
                    //�������Ӧ��ɾ���ľ�������
                    taskmng_trans_task_record_file_delete(gTransTaskCfg.TaskCfg[i].taskid, gTransTaskCfg.TaskCfg[i].oad);
                    taskmng_trans_task_changeflag_set(gTransTaskCfg.TaskCfg[i].taskid, gTransTaskCfg.TaskCfg[i].oad, 2);
                    return TRUE;
                }
            }
        }
        return FALSE;
    }
    else
    {
        //�ж������������޸�
        for(i=0;i<gSpecificTaskList.taskNum;i++)
        {
            if(gSpecificTaskList.taskList[i].nodeid == taskInfo->nodeid && gSpecificTaskList.taskList[i].transTaskid == taskInfo->transTaskid)
            {
                index = i;
                break;
            }
        }
        if(index == -1)
        {
            index = gSpecificTaskList.taskNum;
            gSpecificTaskList.taskNum++;
        }
        
        gSpecificTaskList.taskList[index].transTaskid = taskInfo->transTaskid;
        gSpecificTaskList.taskList[index].nodeid = taskInfo->nodeid;
        
        //��ȡ�������ͨ�ŵ�ַ���� ���255��
        gSpecificTaskList.taskList[index].addrAll.num = taskmng_FJ_ms_list_get(&taskInfo->ms,&gSpecificTaskList.taskList[index].addrAll);;
        gSpecificTaskList.taskList[index].uartParam =taskInfo->uartParam;
        gSpecificTaskList.taskList[index].reqMessType = taskInfo->reqMessType;
        gSpecificTaskList.taskList[index].reqMess = taskInfo->reqMess;
        gSpecificTaskList.taskList[index].resMessType = taskInfo->resMessType;
        gSpecificTaskList.taskList[index].resMaxLen = taskInfo->resMaxLen;
        gSpecificTaskList.taskList[index].resMinLen = taskInfo->resMinLen;
        gSpecificTaskList.taskList[index].resMessProc = taskInfo->resMessProc;
        gSpecificTaskList.taskList[index].next = NULL; //���Ե��̳߳�ʼ����ʱ�����ֵ

        //����û�ж�Ӧ�����񷽰�
        for(i=0;i<gTransTaskCfg.TaskNum; i++)
        {
            if(gTransTaskCfg.TaskCfg[i].taskid == taskInfo->transTaskid)
            {
                //�������Ӧ��ɾ���ľ�������
                taskmng_trans_task_record_file_delete(gTransTaskCfg.TaskCfg[i].taskid, gTransTaskCfg.TaskCfg[i].oad);
                taskmng_trans_task_changeflag_set(gTransTaskCfg.TaskCfg[i].taskid, gTransTaskCfg.TaskCfg[i].oad, 2);
                return TRUE;
            }
        }

        return FALSE;
    }
}



/*******************************************************************************
* ��������: taskmng_trans_task_modify_fun 
* ��������:�����߳��յ�ȫ��͸����������Ϣ����д���
* �������: dbupdate �������Ϣ��ź����ݱ�ʶ
          
* �������: 
* �� �� ֵ: TRUE �������ı� FALSE ��������ı�
*******************************************************************************/
BOOL taskmng_trans_task_modify_fun(DB_UPDATE_T dbupdate)
{
    int             ret;
    int  priority;
    uint8 pipe;
    uint16 i;
    TASK_FMT_DEBUG(-1, TASK_LOG, "͸�������� ��oad %08x ��oad %08x infonum %d \n",dbupdate.MasterOad.value, dbupdate.oad.value, dbupdate.infonum);
    if(dbupdate.MasterOad.value == 0x6E130200)
    {
        //��Ϣ���0xffffʱɾ�����о������� ��oadΪ0xffffʱɾ��һ��͸�����񷽰��µ����о�������
        if(dbupdate.infonum == 0xFFFF)
        {
            TASK_FMT_DEBUG(-1, TASK_LOG, "���о�������ɾ�� \n");
            for(i = 0; i < gTransTaskCfg.TaskNum; i++)
            {
                //֪ͨɾ�������¼�ļ�
                taskmng_trans_task_record_file_delete(gTransTaskCfg.TaskCfg[i].taskid, gTransTaskCfg.TaskCfg[i].oad);
                taskmng_trans_task_changeflag_set(gTransTaskCfg.TaskCfg[i].taskid, gTransTaskCfg.TaskCfg[i].oad, 3);
            }
            memset(&gSpecificTaskList, 0X00, sizeof(FUJIAN_SPECIFIC_TASK_LIST_T));
            return TRUE;
        }
        OOP_TRANS_TASK_NODE_T specificTask;
        memset(&specificTask, 0x00, sizeof(OOP_TRANS_TASK_NODE_T));
        if(dbupdate.oad.value == 0xffffffff)
        {
            TASK_FMT_DEBUG(-1, TASK_LOG, "ȫ�����񷽰� %d �������о�������ɾ�� \n",dbupdate.infonum);
            int          j;
            //���ҳ����о�������� ��ɾ��
            for(i=0;i<gSpecificTaskList.taskNum;i++)
            {
                if(gSpecificTaskList.taskList[i].transTaskid == dbupdate.infonum)
                {
                    for(j=i;j<gSpecificTaskList.taskNum-1;j++)
                    {
                        gSpecificTaskList.taskList[j] = gSpecificTaskList.taskList[j+1];
                    }
                    memset(&gSpecificTaskList.taskList[gSpecificTaskList.taskNum-1],0x00,sizeof(FUJIAN_SPECIFIC_TASK_T));
                    gSpecificTaskList.taskNum--;
                    i--;
                }

            }
            for(i = 0; i < gTransTaskCfg.TaskNum; i++)
            {
                if(gTransTaskCfg.TaskCfg[i].taskid == dbupdate.infonum)
                {
                    //֪ͨɾ�������¼�ļ�
                    taskmng_trans_task_record_file_delete(gTransTaskCfg.TaskCfg[i].taskid, gTransTaskCfg.TaskCfg[i].oad);
                    taskmng_trans_task_changeflag_set(gTransTaskCfg.TaskCfg[i].taskid, gTransTaskCfg.TaskCfg[i].oad, 3);
                    return TRUE;
                }
            }
            return FALSE;
        }

        ret = taskmng_trans_specific_task_info_read(dbupdate.infonum,dbupdate.oad,&specificTask);
        
        if(ret != 0)
        {
            //����ɾ��
            specificTask.transTaskid = dbupdate.infonum;
            specificTask.nodeid = dbupdate.oad.value;
            return taskmng_trans_specific_task_change_fun(0, &specificTask);
        }
        else
        {
            return taskmng_trans_specific_task_change_fun(1, &specificTask);
        }

    }

    switch(dbupdate.oad.value)
    {
        case 0x6E110200: //ȫ��͸�����񷽰�����
            {
                //�������
                if(dbupdate.infonum == 0xffff)
                {
                    TASK_FMT_DEBUG(-1, TASK_LOG, "����ȫ��͸�����񷽰�ɾ�� \n");
                    for(i=0;i<gTransTaskCfg.TaskNum;i++)
                    {
                        taskmng_trans_task_record_file_delete(gTransTaskCfg.TaskCfg[i].taskid, gTransTaskCfg.TaskCfg[i].oad);
                    }
                    //���߳��յ�����¼����ѯһ���������� �����0��ֱ���Լ����
                    gTransTaskCfg.TaskNum = 0;
                    memset(&gTransTaskCfg.TaskCfg, 0x00, sizeof(gTransTaskCfg.TaskCfg));
                    return TRUE;
                }

                OOP_TRANS_TASK_T TaskInfo;

                memset(&TaskInfo, 0x00, sizeof(OOP_TRANS_TASK_T));

                ret = taskmng_trans_task_info_read(dbupdate.infonum, &TaskInfo);
                TASK_FMT_DEBUG(-1, TASK_LOG, "��ʼ��ȡ���� %d  ret %d \n", dbupdate.infonum, ret);

                if(ret != 0)
                {
                    TaskInfo.taskid = dbupdate.infonum;
                    return taskmng_trans_task_change_fun(0, &TaskInfo);
                }
                else
                {
                    return taskmng_trans_task_change_fun(1, &TaskInfo);
                }

            }
            break;
        case 0x6E11FFFF: //�������ȼ����� ͨ����Ϣ֪ͨ��Ӧ�����߳� ����Ҫ���³�ʼ������
        {
            if(dbupdate.infonum != 0xFFFF)
            {
                priority = fujian_task_priority_update_get(gDBClientTask, dbupdate.infonum);
                TASK_FMT_DEBUG(-1, TASK_LOG, "͸�����񷽰� %d ���ȼ�����Ϊ %d \n", dbupdate.infonum, priority);
                if(priority == 254 || priority == 255)
                {
                    for(i=0;i<gTransTaskCfg.TaskNum;i++)
                    {
                        if(dbupdate.infonum == gTransTaskCfg.TaskCfg[i].taskid)
                        {
                            pipe = taskmng_mtr_pipe_get(&gTransTaskCfg.TaskCfg[i].oad);
                            if(pipe != 0xff)
                            {
                                OOP_TRANS_UPDATE_T data;
                                data.transTaskid = dbupdate.infonum;
                                data.priority = priority;
                                task_send_msg(gTaskSock,MSG_PRIORITY_UPDATE,(uint8*)&data,sizeof(OOP_TRANS_UPDATE_T),IPC_AMR+pipe);
                                TASK_FMT_DEBUG(-1, TASK_LOG, "֪ͨ�˿� %d �������ȼ����\n",pipe);
                            }
                            break;
                        }
                    }
                }
                return FALSE;
            }
        }
            break;
        default:
            break;
    }

    return FALSE;
}


/*******************************************************************************
* ��������: taskmng_trans_task_ergodic
* ��������: �ɼ������������
* �������: 

* �������: ��
* �� �� ֵ:   ��
*******************************************************************************/
void taskmng_trans_task_ergodic()
{
    time_t          RealTime_t;
    uint8           i;
    BOOL            ischange;                       //���������Ƿ���Ҫ���
    static BOOL     ischangelast = FALSE;

    if( ReloadTaskFlag ==1)
    {
        TASK_FMT_DEBUG(-1,TASK_LOG, "������������������ʼ�� \n");
        gTransTaskCfg.TaskCfgState = TASK_CFG_UNINIT;
        ReloadTaskFlag = 0;
    }

    //���ݷ����������״ִ̬����ز���
    switch(gTransTaskCfg.TaskCfgState)
    {
        case TASK_CFG_UNINIT:
            {
                TASK_FMT_DEBUG(-1, TASK_LOG, "��ʼ�����ʼ�� \n");
                pthread_mutex_lock(&gTransTaskCfg.Lock);
                taskmng_trans_task_init();
                pthread_mutex_unlock(&gTransTaskCfg.Lock);
                taskmng_trans_task_collection_state_load();

                for(i = 0; i < AMR_THREAD_NUM; i++)
                {
                    gAmrThreadArg[i].TaskNeedInit = 1;
                    gAmrThreadArg[i].taskRunList.isInited = 0;
                }

                //TGThreadArg.TaskNeedInit = 1;
                gTransTaskCfg.TaskCfgState = TASK_CFG_INITED;
                return;
            }

        case TASK_CFG_MODIFY:
            {
                //�յ����񷽰��������Ϣ���д��� ��Ӧ�ı�������ڸ��߳������λ ������֪ͨ�����̸߳�����Щ�����������Ϣ
                TASK_FMT_DEBUG(-1, TASK_LOG, "Task�߳̿�ʼ���д���ȫ��͸�������� \n");
                pthread_mutex_lock(&gTransTaskCfg.Lock);

                ischange = taskmng_trans_task_modify_fun(dbupdateTask);
                memset(&dbupdateTask, 0x00, sizeof(DB_UPDATE_T));
                pthread_mutex_unlock(&gTransTaskCfg.Lock);


                //�������ı���Ҫ֪ͨ�����߳����³�ʼ�� ���û�ı�����Ҫ
                if(ischangelast == FALSE)
                {
                    ischangelast = ischange;
                }

                if((ischange == FALSE) && (ischangelast == FALSE))
                {
                    gTransTaskCfg.TaskCfgState = TASK_CFG_INITED;
                    return;
                }

                TASK_FMT_DEBUG(-1, TASK_LOG, "������ȷ�ϣ� \n");
                gTransTaskCfg.ProCfgModifyDelaySec = 5; //ԭ��30�룬�����ȸ����̵�5��;
                RealTime_t = time(NULL);
                gTransTaskCfg.FunLastRunTime = RealTime_t;
                gTransTaskCfg.TaskCfgState = TASK_CFG_MODIFY_DELAY;
                return;
            }

        case TASK_CFG_MODIFY_DELAY:
            {
                RealTime_t = time(NULL);

                if(RealTime_t != gTransTaskCfg.FunLastRunTime)
                {
                    if(gTransTaskCfg.ProCfgModifyDelaySec % 10 == 0)
                    {
                        TASK_FMT_DEBUG(-1, TASK_LOG, "���������޸���ʱ : %d\n",gTransTaskCfg.ProCfgModifyDelaySec);
                    }

                    gTransTaskCfg.ProCfgModifyDelaySec--;
                    gTransTaskCfg.FunLastRunTime = RealTime_t;
                }

                if(gTransTaskCfg.ProCfgModifyDelaySec <= 0)
                {
                    TASK_FMT_DEBUG(-1, TASK_LOG, "���������޸���ʱ�Ѿ���� : %d\n",gTransTaskCfg.ProCfgModifyDelaySec);
                    taskmng_trans_task_collection_state_load();

                    for(i = 0; i < AMR_THREAD_NUM; i++)
                    {
                        gAmrThreadArg[i].TaskNeedInit = 1;
                    }

                    //TGThreadArg.TaskNeedInit = 1;
                    gTransTaskCfg.TaskCfgState = TASK_CFG_INITED;
                    ischangelast = FALSE;
                }
                return;
            }

        default:
            break;
    }

}

/*******************************************************************************
* ��������: taskmng_scheme_task_link_add
* ��������: �����������ȼ���������
* �������: pAddTaskRunInfo ��Ҫ���������
            headTaskRunInfo ��ǰ���ȼ���ߵ�����ͷ
* �������: 
          
* �� �� ֵ: ���ȼ���ߵ�����ͷ
*******************************************************************************/
FUJIAN_TASK_RUN_T *taskmng_trans_task_link_add(FUJIAN_TASK_RUN_T *pAddTaskRunInfo,FUJIAN_TASK_RUN_T *headTaskRunInfo)
{
    FUJIAN_TASK_RUN_T *PTmpHeadTaskLink = NULL;//��ѯ����ʱʹ��
    FUJIAN_TASK_RUN_T *ScollLink = NULL;//��ѯ����ʱʹ��

    if(pAddTaskRunInfo == NULL)
        return headTaskRunInfo;

    if(headTaskRunInfo == NULL)
    {
        headTaskRunInfo = pAddTaskRunInfo;
        pAddTaskRunInfo->nextTask = NULL;
        return headTaskRunInfo; //��ӳɹ�
    }

    if(pAddTaskRunInfo->priority < headTaskRunInfo->priority)
    {
        pAddTaskRunInfo->nextTask = headTaskRunInfo;
        return pAddTaskRunInfo;
    }

    PTmpHeadTaskLink = headTaskRunInfo;
    while(PTmpHeadTaskLink != NULL)
    {
        //�������Ľڵ��������ȼ�С�ڽڵ���������ȼ�
        if(pAddTaskRunInfo->priority >= PTmpHeadTaskLink->priority)
        {
            //��������Ƕ�ռ�Ե� ��ŵ���ͬ���ȼ�����ǰ��
            if(pAddTaskRunInfo->transTask->bmonopoly == 1 && pAddTaskRunInfo->priority == PTmpHeadTaskLink->priority)
            {
                break;
            }
            //��ͬ���ȼ��ĶԱ��·����� С����ǰ��
            if(pAddTaskRunInfo->priority == PTmpHeadTaskLink->priority && pAddTaskRunInfo->taskid < PTmpHeadTaskLink->taskid)
            {
                break;
            }
            //����ǿ������� ��ŵ���ͬ���ȼ�������� ֻ�е�ǰ�п����������������Ϳ����������ȼ�һ��ʱ����Ҫ�ж�
            if(PTmpHeadTaskLink->transTask->taskTypeData.taskType == 3 && pAddTaskRunInfo->priority == PTmpHeadTaskLink->priority)
            {
                break;
            }
            ScollLink = PTmpHeadTaskLink;
            PTmpHeadTaskLink = PTmpHeadTaskLink->nextTask;
        }
        else
            break;
    }
    pAddTaskRunInfo->nextTask = PTmpHeadTaskLink;
    if(NULL == ScollLink)
    {
        return pAddTaskRunInfo;
    }else
    {
        ScollLink->nextTask = pAddTaskRunInfo;
    }
    return  headTaskRunInfo;
}



/*******************************************************************************
* ��������: taskmng_trans_task_sort
* ��������: //��ÿ���߳��е�����ʱ������а������ȼ����������γ�����
* �������: 
            
* �������: ��
* �� �� ֵ:  
*******************************************************************************/
void taskmng_trans_task_sort(uint8 Pipe)
{
    uint16 i;
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[Pipe];
    pAmrThreadArg->taskHead = NULL;
    for(i = 0;i<pAmrThreadArg->taskRunList.taskNum;i++)
    {
        pAmrThreadArg->taskHead = taskmng_trans_task_link_add(&pAmrThreadArg->taskRunList.taskList[i],pAmrThreadArg->taskHead);
    }
    
    FUJIAN_TASK_RUN_T *PTmpHeadTaskLink = NULL;//��ѯ����ʱʹ��
    PTmpHeadTaskLink = pAmrThreadArg->taskHead;
    TASK_FMT_TRACE(Pipe,REC_LOG,"�����������   : \n");

    while(PTmpHeadTaskLink != NULL)
    {
        TASK_FMT_TRACE(Pipe,REC_LOG," TaskId %d \n",PTmpHeadTaskLink->taskid);
        TASK_FMT_TRACE(Pipe,REC_LOG," Priority  %d  \n",PTmpHeadTaskLink->priority);
        PTmpHeadTaskLink = PTmpHeadTaskLink->nextTask;
    }
}

/*******************************************************************************
* ��������: taskmng_trans_task_meter_info_get
* ��������: ����һ�����񷽰��µ����о������� �ҳ����е�����
* �������: 
* �������: ��
* �� �� ֵ:   
*******************************************************************************/
uint16 taskmng_trans_task_meter_info_get(uint8 pipe,FUJIAN_TASK_RUN_T *runTask)
{
    uint16 meterNum = 0;
    uint16 i;
    OOP_TSA_T meterList[MAX_METER_NUM];
    memset(meterList,0x00,sizeof(meterList));
    FUJIAN_SPECIFIC_TASK_T *specificTask = runTask->specificTask;
    memset(runTask->meterInfo,0x00,sizeof(runTask->meterInfo));

    while(specificTask!=NULL)
    {
        TASK_FMT_DEBUG(pipe, REC_LOG, "͸�����񷽰� %d ��������� %d ��Ҫ����������� %d \n", 
                runTask->taskid,specificTask->nodeid,specificTask->addrAll.num);
        for(i=0;i<specificTask->addrAll.num;i++)
        {
            TASK_FMT_DEBUG(pipe, REC_LOG, " %d ����ַ %02x%02x%02x%02x%02x%02x \n",i,specificTask->addrAll.addr[i].add[0], 
                specificTask->addrAll.addr[i].add[1],specificTask->addrAll.addr[i].add[2],specificTask->addrAll.addr[i].add[3],
                specificTask->addrAll.addr[i].add[4],specificTask->addrAll.addr[i].add[5]);
            fujian_tsa_add(&specificTask->addrAll.addr[i],&meterNum,meterList);
        }
        specificTask = specificTask->next;
    }
    runTask->meterNum = meterNum;
    TASK_FMT_DEBUG(pipe, REC_LOG, "���˺��������Ҫ�����������%d \n", meterNum);
    for(i=0;i<meterNum;i++)
    {
        memcpy(&runTask->meterInfo[i].meterAddr,&meterList[i],sizeof(OOP_TSA_T));
    }
    return meterNum;
}


/*******************************************************************************
* ��������: taskmng_trans_task_run_change
* ��������: ÿ�������߳���������Ĵ���
* �������: 
* �������: ��
* �� �� ֵ:   
*******************************************************************************/
BOOL taskmng_trans_task_run_change(uint8 Pipe)
{
    AmrThreadArg_T * pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[Pipe];
    uint16 i,j;
    int priority;
    uint8 flag = 0;
    uint8           taskPipe;
    uint16          taskNum = 0;
    FUJIAN_TASK_RUN_LIST_T * taskRunList = NULL;
    taskRunList = &pAmrThreadArg->taskRunList;
    //FUJIAN_SPECIFIC_TASK_T *tmpSpecificTask= NULL;
    pthread_mutex_lock(&gTransTaskCfg.Lock);
    if(gTransTaskCfg.TaskNum == 0)
    {
        TASK_FMT_DEBUG(Pipe, REC_LOG, "����ȫ�� \n");
        memset(&pAmrThreadArg->taskRunList, 0x00, sizeof(FUJIAN_TASK_RUN_LIST_T));
        pAmrThreadArg->taskHead = NULL;
        pthread_mutex_unlock(&gTransTaskCfg.Lock);
        return TRUE;
    }
    
    FUJIAN_TASK_RUN_LIST_T currentlist;
    memcpy(&currentlist,&pAmrThreadArg->taskRunList,sizeof(FUJIAN_TASK_RUN_LIST_T));
    
    for(i=0;i<gTransTaskCfg.TaskNum;i++)
    {
        flag = 0;
        taskPipe = taskmng_mtr_pipe_get(&gTransTaskCfg.TaskCfg[i].oad);
        if(taskPipe != Pipe)
            continue;
        for(j=0;j<currentlist.taskNum;j++)
        {
            if(currentlist.taskList[j].taskid == gTransTaskCfg.TaskCfg[i].taskid)
            {
                flag = 1;
                break;
            }
        }
        if(flag == 1 && currentlist.taskList[j].changeFlag == 1)
        {
            memcpy(&taskRunList->taskList[taskNum],&currentlist.taskList[j],sizeof(FUJIAN_TASK_RUN_T));
        }else
        {

            memset(&taskRunList->taskList[taskNum],0x00,sizeof(FUJIAN_TASK_RUN_T));
            taskRunList->taskList[taskNum].changeFlag = 1;//��ʼ����ֵ
            taskRunList->taskList[taskNum].taskid = gTransTaskCfg.TaskCfg[i].taskid;
            taskRunList->taskList[taskNum].priority = gTransTaskCfg.TaskCfg[i].priority;
            taskRunList->taskList[taskNum].transTask = &gTransTaskCfg.TaskCfg[i];
        }
        taskRunList->taskList[taskNum].nextTask = NULL;
        taskRunList->taskList[taskNum].specificTask = NULL;
        taskRunList->taskList[taskNum].specificTaskNum = 0;
        //���������Ƿ��޸ģ������¼��ؾ�������
        if(gTransTaskCfg.TaskCfg[i].taskTypeData.taskType == 0 || gTransTaskCfg.TaskCfg[i].taskTypeData.taskType == 1 ||
             gTransTaskCfg.TaskCfg[i].taskTypeData.taskType == 5)
        {
            //���ҵ������Ӧ�ľ������� ֻ�ж�ʱ �㲥 �����������о�������
            for(j = 0; j < gSpecificTaskList.taskNum; j++)
            {
                if(gSpecificTaskList.taskList[j].transTaskid == gTransTaskCfg.TaskCfg[i].taskid )
                {
                    //��ʼ����ʱ���ȰѾ��������next��һ�� Ҫ��Ȼ����������׳�����
                    gSpecificTaskList.taskList[j].next = NULL;
                    
                    taskRunList->taskList[taskNum].specificTask = fujian_specific_task_add(&gSpecificTaskList.taskList[j],taskRunList->taskList[taskNum].specificTask);
//                    if(taskRunList->taskList[taskNum].specificTask == NULL)
//                    {
//                        taskRunList->taskList[taskNum].specificTask = &gSpecificTaskList.taskList[j];
//                    }
//                    else
//                    {
//                        tmpSpecificTask = taskRunList->taskList[taskNum].specificTask;
//    
//                        while(tmpSpecificTask->next != NULL)
//                        {
//                            tmpSpecificTask = tmpSpecificTask->next;
//                        }
//    
//                        tmpSpecificTask->next = &gSpecificTaskList.taskList[j];
//                    }
                    taskRunList->taskList[taskNum].specificTaskNum++;
                }
            }
        }
        taskmng_trans_task_meter_info_get(Pipe,&taskRunList->taskList[taskNum]);
        TASK_FMT_DEBUG(Pipe, REC_LOG, "���񷽰� %d ��Ҫ���������� %d ������ %d\n",taskRunList->taskList[taskNum].taskid,
                taskRunList->taskList[taskNum].meterNum,taskRunList->taskList[taskNum].result);
        //��ȡ6E11FFFF��ȡ������µ����ȼ� �����254 ��������ͣ
        priority = fujian_task_priority_update_get(pAmrThreadArg->DBClientHandle,taskRunList->taskList[taskNum].taskid);
        TASK_FMT_DEBUG(Pipe, REC_LOG, "��ȡ��������������ȼ� %d \n", priority);
        if(priority == 254 || taskRunList->taskList[taskNum].priority == 254 )
        {
            taskRunList->taskList[taskNum].pauseFlag = 1;
        }
        taskNum++;
    }
    taskRunList->taskNum = taskNum;
    TASK_FMT_DEBUG(Pipe, REC_LOG, "������������� ������� %d \n", taskNum);
    pthread_mutex_unlock(&gTransTaskCfg.Lock);
    pAmrThreadArg->taskHead = NULL;
    taskmng_trans_task_sort(Pipe);
    return TRUE;
}


/*******************************************************************************
* ��������: taskmng_trans_task_run_init
* ��������: ÿ�������̳߳�ʼ������������Ϣ
* �������: 
* �������: ��
* �� �� ֵ:   
*******************************************************************************/
BOOL taskmng_trans_task_run_init(uint8 Pipe)
{

    uint16          i, j;
    uint8           taskPipe;
    uint16          taskNum = 0;
    AmrThreadArg_T * pAmrThreadArg;

    pAmrThreadArg = &gAmrThreadArg[Pipe];
    FUJIAN_TASK_RUN_LIST_T * taskRunList = NULL;
    //FUJIAN_SPECIFIC_TASK_T *tmpSpecificTask= NULL;
    int priority;
    if(gTransTaskCfg.TaskCfgState != TASK_CFG_INITED)
        return FALSE;
    
    if(pAmrThreadArg->taskRunList.isInited ==1)
    {
        //�����յ��ı��
        return taskmng_trans_task_run_change(Pipe);
    }

    memset(&pAmrThreadArg->taskRunList, 0x00, sizeof(FUJIAN_TASK_RUN_LIST_T));
    pAmrThreadArg->taskHead = NULL;
    taskRunList = &pAmrThreadArg->taskRunList;

    TASK_FMT_DEBUG(Pipe, REC_LOG, "��ʼ�����ʼ�� �� \n");
    pthread_mutex_lock(&gTransTaskCfg.Lock);

    for(i = 0; i < gTransTaskCfg.TaskNum; i++)
    {
        taskPipe = taskmng_mtr_pipe_get(&gTransTaskCfg.TaskCfg[i].oad);
    
        if(taskPipe != Pipe)
            continue;
        taskRunList->taskList[taskNum].changeFlag = 1;//��ʼ����ֵ
        taskRunList->taskList[taskNum].taskid = gTransTaskCfg.TaskCfg[i].taskid;
        taskRunList->taskList[taskNum].priority = gTransTaskCfg.TaskCfg[i].priority;
        taskRunList->taskList[taskNum].transTask = &gTransTaskCfg.TaskCfg[i];
        if(gTransTaskCfg.TaskCfg[i].taskTypeData.taskType == 0 || gTransTaskCfg.TaskCfg[i].taskTypeData.taskType == 1 ||
             gTransTaskCfg.TaskCfg[i].taskTypeData.taskType == 5)
        {
            //���ҵ������Ӧ�ľ������� ֻ�ж�ʱ �㲥 �����������о�������
            //��������Ҫ������Ŵ�С���� ��С����
            for(j = 0; j < gSpecificTaskList.taskNum; j++)
            {
                if(gSpecificTaskList.taskList[j].transTaskid == gTransTaskCfg.TaskCfg[i].taskid )
                {
                    //��ʼ����ʱ���ȰѾ��������next��һ�� Ҫ��Ȼ����������׳�����
                    gSpecificTaskList.taskList[j].next = NULL;

                    taskRunList->taskList[taskNum].specificTask = fujian_specific_task_add(&gSpecificTaskList.taskList[j],taskRunList->taskList[taskNum].specificTask);
//                    if(taskRunList->taskList[taskNum].specificTask == NULL)
//                    {
//                        taskRunList->taskList[taskNum].specificTask = &gSpecificTaskList.taskList[j];
//                    }
//                    else
//                    {
//                        tmpSpecificTask = taskRunList->taskList[taskNum].specificTask;
//    
//                        while(tmpSpecificTask->next != NULL)
//                        {
//                            tmpSpecificTask = tmpSpecificTask->next;
//                        }
//    
//                        tmpSpecificTask->next = &gSpecificTaskList.taskList[j];
//                    }
                    taskRunList->taskList[taskNum].specificTaskNum++;
                }
            }
        }
        taskmng_trans_task_meter_info_get(Pipe,&taskRunList->taskList[taskNum]);
        TASK_FMT_DEBUG(Pipe, REC_LOG, "���񷽰� %d ��Ҫ���������� %d \n",taskRunList->taskList[taskNum].taskid,
                taskRunList->taskList[taskNum].meterNum);
        //��ȡ6E11FFFF��ȡ������µ����ȼ� �����254 ��������ͣ
        priority = fujian_task_priority_update_get(pAmrThreadArg->DBClientHandle,taskRunList->taskList[taskNum].taskid);
        TASK_FMT_DEBUG(Pipe, REC_LOG, "��ȡ��������������ȼ� %d \n", priority);
        if(priority == 254 || taskRunList->taskList[taskNum].priority == 254 )
        {
            taskRunList->taskList[taskNum].pauseFlag = 1;
        }
        taskNum++;
    }
    
    
    taskRunList->taskNum = taskNum;
    TASK_FMT_DEBUG(Pipe, REC_LOG, "�����ʼ����� ������� %d \n", taskNum);
    pthread_mutex_unlock(&gTransTaskCfg.Lock);

    taskmng_trans_task_sort(Pipe);
    return TRUE;
}
/*******************************************************************************
* ��������: taskmng_trans_task_record_file_create 
* ��������://���������¼�ļ�
* �������: pipe �˿ں�
            TaskId �����
* �������: 
* �� �� ֵ: 
*******************************************************************************/
void taskmng_trans_task_record_file_create(uint8 pipe,uint16 TaskId)
{

    char      pfile[100]={0};
    memset(pfile,0x00,sizeof(pfile));
    sprintf(pfile,"%s/%s%d",PrivateDir,TASK_RECORD_FILE,TaskId);
    FUJIAN_TASK_RECORD_FILE_T taskRecord;
    memset(&taskRecord,0X00,sizeof(FUJIAN_TASK_RECORD_FILE_T));
    taskRecord.taskid = TaskId;

    TASK_FMT_DEBUG(pipe, REC_LOG,"���������¼�ļ�%s \n",pfile);
    write_pdata_xram(pfile,(char*)&taskRecord,0,sizeof(FUJIAN_TASK_RECORD_FILE_T));
}




/*******************************************************************************
* ��������: taskmng_trans_task_record_file_check 
* ��������://���ÿ�������˽���ļ��Ƿ���� ��û���򴴽� ˽���ļ��洢ÿ������ĳ���״̬
* �������: pipe �˿ں�

* �������: 
* �� �� ֵ: 
*******************************************************************************/
void taskmng_trans_task_record_file_check(uint8 pipe)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    char      pfile[100]={0};
    uint16 i;
    uint16 taskid;

    for(i = 0;i<pAmrThreadArg->taskRunList.taskNum;i++)
    {
        taskid = pAmrThreadArg->taskRunList.taskList[i].taskid;
        memset(pfile,0x00,sizeof(pfile));
        sprintf(pfile,"%s/%s%d",PrivateDir,TASK_RECORD_FILE,taskid);
        if(access(pfile,R_OK) == 0)
        {
            TASK_FMT_DEBUG(pipe, REC_LOG, "�����¼�ļ�%s ����\n",pfile);
            continue;
        }
        taskmng_trans_task_record_file_create(pipe,taskid);
    }

}


/*******************************************************************************
* ��������: taskmng_trans_task_run_reset
* ��������: ������һ�� ���³�ʼ��
* �������: runTaskInfo ��������ʱ����

* �������: ��
* �� �� ֵ:  
*******************************************************************************/
void taskmng_trans_task_run_reset(FUJIAN_TASK_RUN_T *runTaskInfo)
{
    FUJIAN_SPECIFIC_TASK_T *tmp = runTaskInfo->specificTask;
    uint16 i;
    runTaskInfo->specificTaskIndex = 0;
    runTaskInfo->meterIndex = 0;
    runTaskInfo->noticeState = NOTICE_INIT;
    runTaskInfo->failFlag = 0;
    runTaskInfo->noticeFailCnt = 0;
    //runTaskInfo->thisTurnRetryCnt = 0;
    runTaskInfo->retryCycle = 0;
    if(runTaskInfo->storageCnt ==0)
        runTaskInfo->storageCnt = 1;
    //�������ûִ�� ��Ӧ�ðѴ洢��������
    if(runTaskInfo->result != SCHEME_UNDONE)
    {
        runTaskInfo->storageCnt++;
        if(runTaskInfo->storageCnt > runTaskInfo->transTask->storeNum)
            runTaskInfo->storageCnt=1;
    }
    while(tmp)
    {
        for(i=0;i<tmp->addrAll.num;i++)
        {
            tmp->addrAll.result[i] = 0;
            tmp->addrAll.retryCnt[i] = 0;
            tmp->addrAll.isReport[i] = 0;
        }
        tmp = tmp->next;
    }
    for(i=0;i<runTaskInfo->meterNum;i++)
    {
        runTaskInfo->meterInfo[i].meterResult = 0;
        runTaskInfo->meterInfo[i].specificTaskIndex = 0;
        runTaskInfo->meterInfo[i].failCnt = 0;
        runTaskInfo->meterInfo[i].multiBufIndex = 0;
    }
    runTaskInfo->meterSucNum = 0;
}

/*******************************************************************************
* ��������: taskmng_trans_task_run_retry
* ��������: ������һ��ʧ���������ڵĳ�ʼ��
* �������: runTaskInfo ��������ʱ����

* �������: ��
* �� �� ֵ:  
*******************************************************************************/
void taskmng_trans_task_run_retry(uint8 pipe,FUJIAN_TASK_RUN_T *runTaskInfo)
{
    //����Ƕ�֡�������� ʧ������ʱ��ͷ��ʼ
    uint8 retryCycle;
    if(runTaskInfo->transTask->bRelation == 1)
    {
        retryCycle = runTaskInfo->retryCycle;
        taskmng_trans_task_run_reset(runTaskInfo);
        //��֡������������ʱ��ղɼ�״̬
        taskmng_trans_task_collection_state_update(pipe,runTaskInfo,TRANS_TASK_RELATION,NULL);
        runTaskInfo->retryCycle = retryCycle+1;
    }else
    {
        //�����ÿ����ʧ�ܴ���
        uint16 i;
        FUJIAN_SPECIFIC_TASK_T *tmp = runTaskInfo->specificTask;
        while(tmp)
        {
            for(i=0;i<tmp->addrAll.num;i++)
            {
                tmp->addrAll.retryCnt[i] = 0;
                //���Ե�ʱ���ܰ��ϱ���ʶ���
                //tmp->addrAll.isReport[i] = 0;
            }
            tmp = tmp->next;
        }
        runTaskInfo->retryCycle++;
        for(i=0;i<runTaskInfo->meterNum;i++)
        {
            runTaskInfo->meterInfo[i].failCnt = 0;
            runTaskInfo->meterInfo[i].multiBufIndex = 0;
            if(runTaskInfo->meterInfo[i].meterResult==3)
                runTaskInfo->meterInfo[i].meterResult = 0;
        }
        //����ʱ��ͷ��ʼ��һ��
        runTaskInfo->specificTaskIndex = 0;
        runTaskInfo->meterIndex = 0;
        runTaskInfo->meterSucNum = 0;
        runTaskInfo->failFlag = 0;
    }
    
}


/*******************************************************************************
* ��������: TaskRun_UpdateTimes
* ��������: ����ÿ���߳�������ִ���ִ�
* �������: Pipe  �˿ں�
          index  ��������
          TaskInfo ��������ʱ����
* �������: ��
* �� �� ֵ:  
*******************************************************************************/

BOOL task_trans_task_run_cycle_update(uint8 Pipe, FUJIAN_TASK_RUN_T * runTaskInfo)
{

    time_t          RealTime_t;                     //��ǰϵͳʱ���ת��������
    time_t          ThisTurnStorageTime_t;
    time_t          ThisTurnBeginTime_t;
    time_t          ThisTurnEndTime_t;
    time_t          NextTurnBeginTime_t;
    time_t          tmp;
    OOP_TI_T        runTi;                          //ִ������
    OOP_TI_T        retryTi;                        //ʧ����������
    OOP_TI_T        tmpTi;
    OOP_TRANS_TASK_T * taskInfo;
    time_t          thisTurnRetryTime_t;
    OOP_DATETIME_S_T timeNow;

    taskInfo = runTaskInfo->transTask;

    //�������
    if(gPastTimeFlag[Pipe].BitSect.Second != TRUE)
        return FALSE;

    memcpy(&runTi, &taskInfo->cycle, sizeof(OOP_TI_T));
    memcpy(&retryTi, &taskInfo->retryCycle, sizeof(OOP_TI_T));

    RealTime_t = time(NULL);
    DT_Time2DateTimeShort(RealTime_t, &timeNow);

    //���жϵ�ǰʱ���Ƿ������õ�����ʼʱ��ͽ���ʱ����
    if(fujian_time_compare(RealTime_t, &taskInfo->startTime) < 0 || fujian_time_compare(RealTime_t, &taskInfo->endTime) > 0)
    {
        runTaskInfo->periodStat = AMR_AFT_PERIOD;
        return FALSE;
    }
    
    //����Ϊ0������Ҫ������ʱ��������
    //�п�����Ĵ�����������ûд0
    if(runTi.value == 0 || runTaskInfo->transTask->taskTypeData.taskType == 5)
    {
        //�������ʱ��Ϊ0 ���ʾ�������޷�ִ�� �������ж�ʱ���Ƿ���Ч
        if(runTaskInfo->triggerTime.year == 0)
        {
            runTaskInfo->periodStat = AMR_PRE_PERIOD;
            return FALSE;
        }
        else
        {
            //�д���ʱ�� ��Ҫ��������ʼʱ��ͽ���ʱ��
            //�ж��¿�ʼʱ���Ƿ�ʹ���ʱ����� �Է���δ��� �����µ�һ��Ϊ׼
            if(runTaskInfo->thisTurnBeginTime.year != 0 && runTaskInfo->thisTurnEndTime.year != 0)
            {
                if(memcmp(&runTaskInfo->thisTurnBeginTime, &runTaskInfo->triggerTime, sizeof(OOP_DATETIME_S_T)) == 0)
                {
                
                    DT_DateTimeShort2Time(&runTaskInfo->thisTurnBeginTime, &ThisTurnBeginTime_t);
                    DT_DateTimeShort2Time(&runTaskInfo->thisTurnEndTime, &ThisTurnEndTime_t);
                    
                    if(RealTime_t >= ThisTurnBeginTime_t && RealTime_t < ThisTurnEndTime_t)
                    {
                        //�����ǰʱ�仹�ڱ��� ������һ��ʧ�����Կ�ʼʱ���Ƿ���Ҫ����
                        DT_DateTimeShort2Time(&runTaskInfo->thisTurnRetryTime, &thisTurnRetryTime_t);
                        runTaskInfo->periodStat = AMR_IN_PERIOD;
                        if(RealTime_t < thisTurnRetryTime_t)
                        {
                            return FALSE;
                        }
                        else
                        {
                            //�������ǰ״̬�ǳɹ� ����Ҫ�ټ���
                            if(runTaskInfo->result == SCHEME_FINISHED)
                            {
                                return FALSE;
                            }
                            //������һ��ʧ�����Կ�ʼʱ��
                            fujian_time_calc(&timeNow, &retryTi, &runTaskInfo->triggerTime,
                                 &runTaskInfo->thisTurnRetryTime);
                            DT_DateTimeShort2Time(&runTaskInfo->thisTurnRetryTime,&tmp);
                            DT_TimeAddInv(&tmp,retryTi);
                            DT_Time2DateTimeShort(tmp,&runTaskInfo->thisTurnRetryTime);
                            TASK_FMT_DEBUG(Pipe, REC_LOG, "��������%d ������µ�ʧ������ʱ��\n",runTaskInfo->taskid);
                            fujian_datetime_s_log_write(Pipe, REC_LOG, &runTaskInfo->thisTurnRetryTime);
                            

                            //�������ʧ���� �����������һ��
                            if(runTaskInfo->result == SCHEME_FAIL)
                            {
                                runTaskInfo->result = SCHEME_UNDONE;
                                runTaskInfo->isNewTurn = 1;
                                taskmng_trans_task_run_retry(Pipe,runTaskInfo);
                            }

                            runTaskInfo->periodStat = AMR_IN_PERIOD;
                            return TRUE;
                        }
                    }
                    else
                    {
                        //��������˱��ֽ���ʱ�� ��ô���������ִ�� �Ⱥ����ٱ����� ������ʱ���ÿ�
                        runTaskInfo->periodStat = AMR_AFT_PERIOD;
                        memset(&runTaskInfo->triggerTime, 0x00, sizeof(OOP_DATETIME_S_T));
                        return FALSE;
                    }
                }
                else
                {
                    //�����µĴ���ʱ������������ʱ��
                    memcpy(&runTaskInfo->thisTurnBeginTime,&runTaskInfo->triggerTime,sizeof(OOP_DATETIME_S_T));
                    memcpy(&runTaskInfo->storageTime,&runTaskInfo->triggerTime,sizeof(OOP_DATETIME_S_T));
                    //����ʱ��Ϊ����ʱ�̼�������ʱ��
                    DT_DateTimeShort2Time(&runTaskInfo->triggerTime,&ThisTurnEndTime_t);
                    tmpTi.value = taskInfo->taskTypeData.addi_param.condition.time;
                    tmpTi.unit = TI_MIN;
                    DT_TimeAddInv(&ThisTurnEndTime_t,tmpTi);

                    DT_Time2DateTimeShort(ThisTurnEndTime_t,&runTaskInfo->thisTurnEndTime);
                    if(retryTi.value == 0)
                    {
                        runTaskInfo->thisTurnRetryTime = runTaskInfo->thisTurnEndTime;

                    }else
                    {
                        fujian_time_calc(&timeNow, &retryTi, &runTaskInfo->triggerTime,&runTaskInfo->thisTurnRetryTime);
                        DT_DateTimeShort2Time(&runTaskInfo->thisTurnRetryTime,&tmp);
                        DT_TimeAddInv(&tmp,retryTi);
                        DT_Time2DateTimeShort(tmp,&runTaskInfo->thisTurnRetryTime);

                    }

                    TASK_FMT_DEBUG(Pipe, REC_LOG, "�������� %d ���µ�����ʼʱ��\n",runTaskInfo->taskid);
                    fujian_datetime_s_log_write(Pipe, REC_LOG, &runTaskInfo->thisTurnBeginTime);
                    TASK_FMT_DEBUG(Pipe, REC_LOG, "�������� %d ���µ��������ʱ��\n",runTaskInfo->taskid);
                    fujian_datetime_s_log_write(Pipe, REC_LOG, &runTaskInfo->thisTurnEndTime);
                    TASK_FMT_DEBUG(Pipe, REC_LOG, "�������� %d ���µ�ʧ����������\n",runTaskInfo->taskid);
                    fujian_datetime_s_log_write(Pipe, REC_LOG, &runTaskInfo->thisTurnRetryTime);
                    TASK_FMT_DEBUG(Pipe, REC_LOG, "�������� %d �洢ʱ��\n",runTaskInfo->taskid);
                    fujian_datetime_s_log_write(Pipe, REC_LOG, &runTaskInfo->storageTime);
                    //�����µĴ���ʱ��������ִ��
                    runTaskInfo->result = SCHEME_UNDONE;
                    taskmng_trans_task_run_reset(runTaskInfo);
                    runTaskInfo->isNewTurn = 1;
                    runTaskInfo->periodStat = AMR_IN_PERIOD;
                    taskmng_trans_task_collection_state_update(Pipe,runTaskInfo,TRANS_TASK_INIT,NULL);
                    return TRUE;
                }
            }
            else
            {
                //�����µ���������
                memcpy(&runTaskInfo->thisTurnBeginTime,&runTaskInfo->triggerTime,sizeof(OOP_DATETIME_S_T));
                memcpy(&runTaskInfo->storageTime,&runTaskInfo->triggerTime,sizeof(OOP_DATETIME_S_T));
                //����ʱ��Ϊ����ʱ�̼�������ʱ��
                DT_DateTimeShort2Time(&runTaskInfo->triggerTime,&ThisTurnEndTime_t);
                tmpTi.value = taskInfo->taskTypeData.addi_param.condition.time;
                tmpTi.unit = TI_MIN;
                DT_TimeAddInv(&ThisTurnEndTime_t,tmpTi);
                TASK_FMT_DEBUG(Pipe, REC_LOG, "�������� %d ������Чʱ�� %d ���� ThisTurnEndTime_t %ld\n",runTaskInfo->taskid,
                        taskInfo->taskTypeData.addi_param.condition.time,ThisTurnEndTime_t);
                DT_Time2DateTimeShort(ThisTurnEndTime_t,&runTaskInfo->thisTurnEndTime);
                if(retryTi.value == 0)
                    {
                    runTaskInfo->thisTurnRetryTime = runTaskInfo->thisTurnEndTime;

                }else
                {
                    fujian_time_calc(&timeNow, &retryTi, &runTaskInfo->triggerTime,&runTaskInfo->thisTurnRetryTime);
                    DT_DateTimeShort2Time(&runTaskInfo->thisTurnRetryTime,&tmp);
                    DT_TimeAddInv(&tmp,retryTi);
                    DT_Time2DateTimeShort(tmp,&runTaskInfo->thisTurnRetryTime);

                }

                TASK_FMT_DEBUG(Pipe, REC_LOG, "�������� %d ����ʼʱ��\n",runTaskInfo->taskid);
                fujian_datetime_s_log_write(Pipe, REC_LOG, &runTaskInfo->thisTurnBeginTime);
                TASK_FMT_DEBUG(Pipe, REC_LOG, "�������� %d �������ʱ��\n",runTaskInfo->taskid);
                fujian_datetime_s_log_write(Pipe, REC_LOG, &runTaskInfo->thisTurnEndTime);
                TASK_FMT_DEBUG(Pipe, REC_LOG, "�������� %d ʧ������ʱ��\n",runTaskInfo->taskid);
                fujian_datetime_s_log_write(Pipe, REC_LOG, &runTaskInfo->thisTurnRetryTime);
                TASK_FMT_DEBUG(Pipe, REC_LOG, "�������� %d �洢ʱ��\n",runTaskInfo->taskid);
                fujian_datetime_s_log_write(Pipe, REC_LOG, &runTaskInfo->storageTime);
                taskmng_trans_task_run_reset(runTaskInfo);
                runTaskInfo->result = SCHEME_UNDONE;
                runTaskInfo->isNewTurn = 1;
                runTaskInfo->periodStat = AMR_IN_PERIOD;
                taskmng_trans_task_collection_state_update(Pipe,runTaskInfo,TRANS_TASK_INIT,NULL);
                return TRUE;
            }
        }
    }
    else
    {
        //�������� �����ִ�
        if(runTaskInfo->thisTurnBeginTime.year != 0 && runTaskInfo->thisTurnEndTime.year != 0)
        {
            //�����ʼ����ʱ�� �����Ƿ���Ҫ����
            DT_DateTimeShort2Time(&runTaskInfo->thisTurnBeginTime, &ThisTurnBeginTime_t);
            DT_DateTimeShort2Time(&runTaskInfo->thisTurnEndTime, &ThisTurnEndTime_t);
            DT_DateTimeShort2Time(&runTaskInfo->nextTurnBeginTime, &NextTurnBeginTime_t);
            DT_DateTimeShort2Time(&runTaskInfo->storageTime, &ThisTurnStorageTime_t);
            if(RealTime_t >= ThisTurnBeginTime_t && RealTime_t < ThisTurnEndTime_t)
            {
                runTaskInfo->periodStat = AMR_IN_PERIOD;
                //�����ǰʱ�仹�ڱ��� ������һ��ʧ�����Կ�ʼʱ���Ƿ���Ҫ����
                DT_DateTimeShort2Time(&runTaskInfo->thisTurnRetryTime, &thisTurnRetryTime_t);
                if(RealTime_t < thisTurnRetryTime_t)
                {
                    return FALSE;
                }else
                {
                    //�������ǰ״̬�ǳɹ� ����Ҫ�ټ���
                    if(runTaskInfo->result == SCHEME_FINISHED)
                    {
                        return FALSE;
                    }
                    //������һ��ʧ�����Կ�ʼʱ��
                    fujian_time_calc(&timeNow, &retryTi, &runTaskInfo->thisTurnBeginTime,&runTaskInfo->thisTurnRetryTime);
                    DT_DateTimeShort2Time(&runTaskInfo->thisTurnRetryTime,&tmp);
                    DT_TimeAddInv(&tmp,retryTi);
                    DT_Time2DateTimeShort(tmp,&runTaskInfo->thisTurnRetryTime);
                    TASK_FMT_DEBUG(Pipe, REC_LOG, "�������� %d ������µ�ʧ������ʱ�� result %d\n",runTaskInfo->taskid,runTaskInfo->result);
                    fujian_datetime_s_log_write(Pipe, REC_LOG, &runTaskInfo->thisTurnRetryTime);
                    //�������ʧ���� �����������һ��
                    if(runTaskInfo->result == SCHEME_FAIL)
                    {
                        runTaskInfo->result = SCHEME_UNDONE;
                        runTaskInfo->isNewTurn = 1;
                        taskmng_trans_task_run_retry(Pipe,runTaskInfo);
                    }
                    runTaskInfo->periodStat = AMR_IN_PERIOD;
                    return TRUE;
                }
            }
            else if(RealTime_t < ThisTurnBeginTime_t && RealTime_t > ThisTurnStorageTime_t)
            {
                //�Է���ǰ��ʱ��ʱ�� ���񲻻�����ִ��
                //��û������ִ�е�ʱ��
                runTaskInfo->periodStat = AMR_PRE_PERIOD;
                return FALSE;
            }
            else if(RealTime_t >= ThisTurnEndTime_t && RealTime_t < NextTurnBeginTime_t )
            {
                //�����˱��ֽ���ʱ�� ��û����һ�� 
                taskmng_trans_task_isreport_check(Pipe,runTaskInfo);
                runTaskInfo->periodStat = AMR_AFT_PERIOD;
                return FALSE;
            }else
            {
                TASK_FMT_DEBUG(Pipe, REC_LOG, "�������� %d ׼��������һ�� ��ǰ�洢���� %d\n",runTaskInfo->taskid,runTaskInfo->storageCnt);
                taskmng_trans_task_isreport_check(Pipe,runTaskInfo);
                //������һ�ֿ�ʼʱ�� ��ʼ������һ��
                //�����µ���������
                fujian_time_calc(&timeNow, &runTi, &taskInfo->startTime,&runTaskInfo->thisTurnBeginTime);
                fujian_time_calc(&timeNow, &runTi, NULL,&runTaskInfo->storageTime);
                fujian_time_calc(&timeNow, &runTi, &taskInfo->endTime,&runTaskInfo->thisTurnEndTime);
                //���ո������㷨 �������ʼʱ��ͽ���ʱ�䶼���Ա�ִ���������� ��������ı��ֿ�ʼ����ʱ������
                //��ʱ�ֶ������ֽ���ʱ������һ������ �����������ʼʱ��Ƚ���ʱ������� ����������ʱ���ǰ��
                //if(memcmp(&runTaskInfo->thisTurnBeginTime,&runTaskInfo->thisTurnEndTime,sizeof(OOP_DATETIME_S_T))==0)
                if(fujian_datetime_s_compare(&runTaskInfo->thisTurnBeginTime,&runTaskInfo->thisTurnEndTime)>=0)
                {
                    DT_DateTimeShort2Time(&runTaskInfo->thisTurnEndTime,&tmp);
                    DT_TimeAddInv(&tmp,runTi);
                    DT_Time2DateTimeShort(tmp,&runTaskInfo->thisTurnEndTime);
                }
                if(retryTi.value == 0|| fujian_ti_compare(&runTi,&retryTi)<0)
                {
                    runTaskInfo->thisTurnRetryTime = runTaskInfo->thisTurnEndTime;

                }else
                {
                    fujian_time_calc(&timeNow, &retryTi, &taskInfo->startTime,&runTaskInfo->thisTurnRetryTime);
                    //ʧ������ʱ��Ҫ�ټ�һ��ʧ���������� Ҫ��Ȼ��������ȵ�ǰʱ��С
                    DT_DateTimeShort2Time(&runTaskInfo->thisTurnRetryTime,&tmp);
                    DT_TimeAddInv(&tmp,retryTi);
                    DT_Time2DateTimeShort(tmp,&runTaskInfo->thisTurnRetryTime);

                }

                    
                DT_DateTimeShort2Time(&runTaskInfo->thisTurnBeginTime,&NextTurnBeginTime_t);
                DT_TimeAddInv(&NextTurnBeginTime_t,runTi);
                DT_Time2DateTimeShort(NextTurnBeginTime_t,&runTaskInfo->nextTurnBeginTime);
                
                TASK_FMT_DEBUG(Pipe, REC_LOG, "�������� %d ��������ʼʱ��\n",runTaskInfo->taskid);
                fujian_datetime_s_log_write(Pipe, REC_LOG, &runTaskInfo->thisTurnBeginTime);
                TASK_FMT_DEBUG(Pipe, REC_LOG, "�������� %d �����������ʱ��\n",runTaskInfo->taskid);
                fujian_datetime_s_log_write(Pipe, REC_LOG, &runTaskInfo->thisTurnEndTime);
                TASK_FMT_DEBUG(Pipe, REC_LOG, "�������� %d ����ʧ������ʱ��\n",runTaskInfo->taskid);
                fujian_datetime_s_log_write(Pipe, REC_LOG, &runTaskInfo->thisTurnRetryTime);
                TASK_FMT_DEBUG(Pipe, REC_LOG, "�������� %d ��������ʼʱ��\n",runTaskInfo->taskid);
                fujian_datetime_s_log_write(Pipe, REC_LOG, &runTaskInfo->nextTurnBeginTime);
                TASK_FMT_DEBUG(Pipe, REC_LOG, "�������� %d �洢ʱ��\n",runTaskInfo->taskid);
                fujian_datetime_s_log_write(Pipe, REC_LOG, &runTaskInfo->storageTime);
                taskmng_trans_task_run_reset(runTaskInfo);
                TASK_FMT_DEBUG(Pipe, REC_LOG, "�������� %d �洢���� %d\n",runTaskInfo->taskid,runTaskInfo->storageCnt);
                runTaskInfo->result = SCHEME_UNDONE;
                runTaskInfo->isNewTurn = 1;
                runTaskInfo->periodStat = AMR_IN_PERIOD;
                //������ǰ��ʱ����������������ִ������
                if(fujian_time_compare(RealTime_t,&runTaskInfo->thisTurnBeginTime)<0 ||fujian_time_compare(RealTime_t,&runTaskInfo->thisTurnEndTime)>0 )
                {
                    TASK_FMT_DEBUG(Pipe, REC_LOG, "��ǰ��ʱ��������ִ��������\n");
                    runTaskInfo->periodStat = AMR_PRE_PERIOD;
                    return FALSE;
                }
                taskmng_trans_task_collection_state_update(Pipe,runTaskInfo,TRANS_TASK_INIT,NULL);
                return TRUE;
                
            }
            

        }else
        {
            //����ʼ����ʱ��Ϊ�� ��һ�μ���
            fujian_time_calc(&timeNow, &runTi, &taskInfo->startTime,&runTaskInfo->thisTurnBeginTime);
            fujian_time_calc(&timeNow, &runTi, NULL,&runTaskInfo->storageTime);
            fujian_time_calc(&timeNow, &runTi, &taskInfo->endTime,&runTaskInfo->thisTurnEndTime);
            if(fujian_datetime_s_compare(&runTaskInfo->thisTurnBeginTime,&runTaskInfo->thisTurnEndTime)>=0)
            {
                DT_DateTimeShort2Time(&runTaskInfo->thisTurnEndTime,&tmp);
                DT_TimeAddInv(&tmp,runTi);
                DT_Time2DateTimeShort(tmp,&runTaskInfo->thisTurnEndTime);
            }
            //���ʧ������������0 �Ͳ����� �����������ʱ��
            //ʧ���������ڱ�ִ�����ڴ� Ҳ������
            if(retryTi.value == 0 || fujian_ti_compare(&runTi,&retryTi)<0)
            {
                runTaskInfo->thisTurnRetryTime = runTaskInfo->thisTurnEndTime;

            }else
            {
                fujian_time_calc(&timeNow, &retryTi, &taskInfo->startTime,&runTaskInfo->thisTurnRetryTime);
                DT_DateTimeShort2Time(&runTaskInfo->thisTurnRetryTime,&tmp);
                DT_TimeAddInv(&tmp,retryTi);
                DT_Time2DateTimeShort(tmp,&runTaskInfo->thisTurnRetryTime);
            }
            DT_DateTimeShort2Time(&runTaskInfo->thisTurnBeginTime,&NextTurnBeginTime_t);
            DT_TimeAddInv(&NextTurnBeginTime_t,runTi);
            DT_Time2DateTimeShort(NextTurnBeginTime_t,&runTaskInfo->nextTurnBeginTime);
            
            TASK_FMT_DEBUG(Pipe, REC_LOG, "�������� %d ��������ʼʱ��\n",runTaskInfo->taskid);
            fujian_datetime_s_log_write(Pipe, REC_LOG, &runTaskInfo->thisTurnBeginTime);
            TASK_FMT_DEBUG(Pipe, REC_LOG, "�������� %d �����������ʱ��\n",runTaskInfo->taskid);
            fujian_datetime_s_log_write(Pipe, REC_LOG, &runTaskInfo->thisTurnEndTime);
            TASK_FMT_DEBUG(Pipe, REC_LOG, "�������� %d ����ʧ����������\n",runTaskInfo->taskid);
            fujian_datetime_s_log_write(Pipe, REC_LOG, &runTaskInfo->thisTurnRetryTime);
            TASK_FMT_DEBUG(Pipe, REC_LOG, "�������� %d ��������ʼʱ��\n",runTaskInfo->taskid);
            fujian_datetime_s_log_write(Pipe, REC_LOG, &runTaskInfo->nextTurnBeginTime);
            TASK_FMT_DEBUG(Pipe, REC_LOG, "�������� %d �洢ʱ��\n",runTaskInfo->taskid);
            fujian_datetime_s_log_write(Pipe, REC_LOG, &runTaskInfo->storageTime);
            taskmng_trans_task_run_reset(runTaskInfo);
            runTaskInfo->result = SCHEME_UNDONE;
            runTaskInfo->isNewTurn = 1;
            runTaskInfo->periodStat = AMR_IN_PERIOD;

            //��һ�����ʱ����Ҫ���ж��µ�ǰʱ��Ϳ�ʼʱ�� �п��ܻ�û������ִ�е�ʱ��
            if(fujian_time_compare(RealTime_t,&runTaskInfo->thisTurnBeginTime)<0 ||fujian_time_compare(RealTime_t,&runTaskInfo->thisTurnEndTime)>0 )
            {
                TASK_FMT_DEBUG(Pipe, REC_LOG, "������μ����������ִ��������\n");
                runTaskInfo->periodStat = AMR_PRE_PERIOD;
                return FALSE;
            }
            //����ִ�������ڵĲ����²ɼ�״̬
            taskmng_trans_task_collection_state_update(Pipe,runTaskInfo,TRANS_TASK_INIT,NULL);

            return TRUE;

        }
    }


}



/*******************************************************************************
* ��������: taskmng_task_run_ergodic
* ��������: ÿ���߳��ڸ�����������ʱ����
* �������: 
* �������:
* �� �� ֵ:
*******************************************************************************/
void taskmng_trans_task_run_ergodic(uint8 Pipe)
{
    uint16 i;
    AmrThreadArg_T* pAmrThreadArg;

    pAmrThreadArg = &gAmrThreadArg[Pipe];
    if(pAmrThreadArg->TaskNeedInit == 1)
    {
        pAmrThreadArg->TaskModifyFlag = TASK_CFG_UNINIT;
        pAmrThreadArg->TaskNeedInit =0;
    }
    switch(pAmrThreadArg->TaskModifyFlag)
    {

    case TASK_CFG_UNINIT:
    {
        while(1)
        {
            if(fujian_task_record_file_del_check(Pipe)==0)
                break;
        }
        if(taskmng_trans_task_run_init(Pipe)==TRUE)
        {
            taskmng_trans_task_record_file_check(Pipe);
            pAmrThreadArg->TaskModifyFlag = TASK_CFG_INITED;
            pAmrThreadArg->taskRunList.isInited = 1;
            //�����ʼ����֮���ж����Ƿ����ϵ紥������
//            if(pAmrThreadArg->taskPowerOnFlag ==1)
//            {
//                taskmng_trigger_task_poweron_check(Pipe);
//                pAmrThreadArg->taskPowerOnFlag = 0 ;
//            }

        }
        break;
    }
    default:
        break;
    }
    if(pAmrThreadArg->taskPowerOnFlag ==1)
    {
        taskmng_trigger_task_poweron_check(Pipe);
        pAmrThreadArg->taskPowerOnFlag = 0 ;
    }

    for(i=0;i<pAmrThreadArg->taskRunList.taskNum;i++)
    {
        task_trans_task_run_cycle_update(Pipe,&pAmrThreadArg->taskRunList.taskList[i]);
    }

}

/*******************************************************************************
* ��������:         taskmng_fujian_task_queue_add
* ��������:         �����������
* �������:         PLCTASK_QUEUE *PlcTaskQueue//�������
                    PlcTask                    //��ǰ����
* �������:         int
* �� �� ֵ:         > 0  ��ӳɹ�
                    < 0  ���ʧ��

*******************************************************************************/
int taskmng_fujian_task_queue_add ( uint8 CommPipe,MAIN_CYC_STATE_T TaskName )
{
    int             i = 0;
    PLCTASK_QUEUE*  PlcAddTask = NULL;
    PLCTASK_QUEUE*  PlcTaskQueue = NULL;

    PlcTaskQueue = gAmrThreadArg[CommPipe].PlcTaskQueue;

    for ( i = 0; i < PLCTASK_MAXNUM; i++ )
    {
        if ( PlcTaskQueue[i].TaskStat == 0 )
        {
            PlcAddTask = &PlcTaskQueue[i];
            break;
        }
    }

    //TASK_FMT_TRACE ( CommPipe,COMM_LOG, "���ҵ���Ӧ��������� %d \n",i );
    //���������
    if ( PlcAddTask == NULL )
    {
        return -1;
    }

    i = 0;
    while ( gInnerTaskList[i].TaskName != 0xFF )
    {
        if ( gInnerTaskList[i].TaskName == TaskName )
        {
            PlcAddTask->TaskStat    = 1;
            PlcAddTask->TaskName    = TaskName;
            PlcAddTask->Priority    = gInnerTaskList[i].Priority;
            PlcAddTask->DelaySec    = gInnerTaskList[i].DelaySec;
            PlcAddTask->RoutrtCtrl  = gInnerTaskList[i].RoutrtCtrl;
            PlcAddTask->pPlcTaskFun = gInnerTaskList[i].pPlcTaskFun;      //����Ĵ�����
            PlcAddTask->PlcTaskDeal_Stat = TASK_INIT;                      //����ֵΪ��ʼ״̬
            break;
        }
        i++;
    }

    //������еĲ���
    InsertPlcTaskQueue ( CommPipe,PlcAddTask );
    //TASK_FMT_TRACE ( CommPipe,COMM_LOG,"�������� %d\n",TaskName);

    PLCTASK_QUEUE*   pTmpLink = NULL;

    pTmpLink = gAmrThreadArg[CommPipe].PlcTask_RunLink;
    TASK_FMT_TRACE ( CommPipe,COMM_LOG,"��������� ˢ���������   : \n" );
    while ( pTmpLink != NULL )
    {
        TASK_FMT_TRACE ( CommPipe,COMM_LOG,"TaskName  %d Priority %d  \n",pTmpLink->TaskName,pTmpLink->Priority);
        pTmpLink = pTmpLink->next;
    }

    return 1;

}



/*******************************************************************************
* ��������: taskmng_fujian_task_creat
* ��������: ������в����ж�
* �������: 
* �������: ��
* �� �� ֵ: ��
*******************************************************************************/
void taskmng_fujian_task_creat(uint8 Pipe)
{
    AmrThreadArg_T* pAmrThreadArg;

    pAmrThreadArg = &gAmrThreadArg[Pipe];
    int ret;
    //�ϵ�����
    if(gPowerOnFlag[Pipe]==TRUE)
    {
        gPowerOnFlag[Pipe]=FALSE;
        //�ϵ���������ȶ�����
        TASK_FMT_DEBUG( Pipe, COMM_LOG, "�ϵ������������ ��\n");
        taskmng_fujian_task_queue_add(Pipe,MS_NORMAL_AMR); 

        Init_ReadMtr(Pipe);
        
        //taskmng_trigger_task_poweron_check(Pipe);
    }
    
    //�ز����е�����
    if(Pipe == 0)
    {
        if(gPastTimeFlag[Pipe].BitSect.Second == TRUE)
        {
            if(pAmrThreadArg->IsReportHPLCInfo == 0 && time(NULL) -pAmrThreadArg->HPLCInfoSendTime > 60)
            {
                //taskmng_HPLC_info_get(Pipe);
    
            }
        }
    
        if(IsSetAddr[Pipe] == 0 && taskmng_addr_isvalid_check(LocalAddr,6) > 0)
        {
            TASK_FMT_DEBUG(Pipe, COMM_LOG, "����ģ���ַ ��\n");
            fujian_local_addr_set(Pipe);
        }

        if(IsSetGatherMode[Pipe] == 0 )
        {
            if(fujian_gather_mode_get_from_ccoRouter(Pipe) == gGatherMode+1)
            {
                IsSetGatherMode[Pipe] = 1; 
            }else
            {
                TASK_FMT_DEBUG(Pipe, COMM_LOG, "����ͨ��Э��ģʽ ��\n");
                fujian_gather_mode_set(Pipe);
            }
        }
        if(gIsGetCCOInfo[Pipe] == 0)
        {
            fujian_cco_info_get(Pipe);
        }
    }
    
    //��͸����Ĳ���
    if(pAmrThreadArg->RelayTrans.isValid ==0)
    {
        while(1)
        {
            ret = taskmng_relay_exist_check(Pipe);
            if(ret !=-1)
            {
                break;
            }
        }
       if(ret == 1)
       {
           TASK_FMT_DEBUG( Pipe, RELAY_LOG, "������͸���� ��\n");
           taskmng_fujian_task_queue_add(Pipe,MS_RELAY_TRANS);
       }
    }

}

/*******************************************************************************
* ��������: taskmng_plc_task_run
* ��������: ����ִ�д���
* �������: 
* �������: ��
* �� �� ֵ: ��
*******************************************************************************/
void taskmng_fujian_task_run ( uint8 CommPipe )
{

    int              Ret;
    PLCTASK_QUEUE*   pTmpLink = NULL;
    PLCTASK_QUEUE*   PlcTask_RunLink = NULL;
    static uint8     gDelaySec[AMR_THREAD_NUM];

    PlcTask_RunLink = gAmrThreadArg[CommPipe].PlcTask_RunLink;

    //��ǰ�����Ƿ���Ч,�����Ч�����Ѿ�ִ�����,ִ����һ���������
    if ( PlcTask_RunLink == NULL || PlcTask_RunLink->TaskStat == 0 )
    {
        //TASK_FMT_TRACE(CommPipe,COMM_LOG, "��ǰû��������Ҫִ�� \n" );
        return;//�������Ϊ��ִ�з��ز���Ҫִ��
    }

    //��ǰ�����ִ�д���

    switch ( PlcTask_RunLink->PlcTaskDeal_Stat )
    {
        case TASK_INIT:                         //����ĳ�ʼ������
            TASK_FMT_TRACE(CommPipe,COMM_LOG, "---->    ������ : %d ִ�� ��ʼ   ------   \n",PlcTask_RunLink->TaskName );
            if ( PlcTask_RunLink->RoutrtCtrl != RCS_NULL )
            {
                TASK_FMT_TRACE(CommPipe,COMM_LOG, " ·�ɿ��� : %d  \n",PlcTask_RunLink->RoutrtCtrl );
                PlcTask_RunLink->PlcTaskDeal_Stat = ROUTERCTRL_DEAL;
            }
            else
            {
                PlcTask_RunLink->PlcTaskDeal_Stat = FUN_DEAL;
            }
            break;

        case ROUTERCTRL_DEAL:                   //·�ɵĲ���
            Ret = taskmng_router_ctrl_set ( CommPipe,PlcTask_RunLink->RoutrtCtrl );
            if ( Ret > 0 )
            {
                if ( PlcTask_RunLink->DelaySec <= 5 )
                {
                    gDelaySec[CommPipe] = PlcTask_RunLink->DelaySec;
                }
                else
                {
                    gDelaySec[CommPipe] = GetEnvValue ( "PLC_DELAY",PlcTask_RunLink->DelaySec );
                }
                PlcTask_RunLink->PlcTaskDeal_Stat = ROUTERCTRL_DELAY;
            }
            break;

        case ROUTERCTRL_DELAY:                  //��ʱ����
            Ret = taskmng_delay_deal ( CommPipe,&gDelaySec[CommPipe] );
            if ( Ret > 0 )
            {
                PlcTask_RunLink->PlcTaskDeal_Stat = FUN_DEAL;
            }
            break;

        case FUN_DEAL:                          //��ǰ������

            Ret = PlcTask_RunLink->pPlcTaskFun ( CommPipe );
            if ( Ret > 0 )
            {
                TASK_FMT_TRACE(CommPipe,COMM_LOG, "---->  ���� : %d ִ�� ����  ------    \n",PlcTask_RunLink->TaskName );

                pTmpLink = PlcTask_RunLink->next;
                PlcTask_RunLink->TaskStat = 0;
                PlcTask_RunLink = pTmpLink;
                
            }
            else
                return;  //�����������ӵ��¼��ϱ����������return������ᱻɾ��
            
            break;
        default:
            break;
 
    }
    gAmrThreadArg[CommPipe].PlcTask_RunLink = PlcTask_RunLink;
}

/*******************************************************************************
* ��������: taskmng_task_runlist_check
* ��������: ����Ƿ�����Ҫ���еĳ�������
* �������: �˿�
* �������: TaskInfo  �����е����� ��û����Ϊ��
* �� �� ֵ: >0                  ������
*           <0               ��������ȫ�����
*******************************************************************************/
int taskmng_trans_task_run_check(uint8 Pipe, FUJIAN_TASK_RUN_T **TaskInfo)
{
    AmrThreadArg_T * pAmrThreadArg;

    pAmrThreadArg = &gAmrThreadArg[Pipe];
    FUJIAN_TASK_RUN_T * tmpTask = pAmrThreadArg->taskHead;

    while(1)
    {
        if(tmpTask != NULL)
        {
            //��������Ƕ�ռ�� ����������
            if(tmpTask->periodStat == AMR_IN_PERIOD && tmpTask->transTask->bmonopoly == 1)
            {
                *TaskInfo = tmpTask;
                return 1;
            }
            else
            {   
                //û�����������ķ���Ҳ����ִ�� ���ܸ���ִ��ʱ��
                if(tmpTask->periodStat != AMR_IN_PERIOD || tmpTask->result == SCHEME_FINISHED ||
                     tmpTask->result == SCHEME_FAIL || tmpTask->pauseFlag == 1 || 
                     ((tmpTask->transTask->taskTypeData.taskType == 0 ||tmpTask->transTask->taskTypeData.taskType == 1 ||
                     tmpTask->transTask->taskTypeData.taskType == 5)&& tmpTask->specificTask==NULL))
                {
                    tmpTask = tmpTask->nextTask;
                }
                else
                {
                    *TaskInfo = tmpTask;
                    return 1;
                }
            }
        }
        else
            return - 1;
    }
}

/*******************************************************************************
* ��������: taskmng_task_record_file_load 
* ��������://����ʼ����ʱ��������¼�ļ� ��ǰ�ִε������Ƿ񳭹� ��������򽫲��ֽ������
* �������: pipe �˿ں�
            TaskInfo ������Ϣ
            RunTask ��������ʱ��Ϣ
* �������: 
* �� �� ֵ: 
*******************************************************************************/
void taskmng_trans_task_record_file_load(uint8 pipe,FUJIAN_TASK_RUN_T* RunTask)
{

    uint16 i,j;

    FUJIAN_TASK_RECORD_FILE_T TaskRecord;
    memset(&TaskRecord,0x00,sizeof(FUJIAN_TASK_RECORD_FILE_T));
    uint16 TaskId = RunTask->taskid;
    uint8 flag = 0;
    char      pfile[100]={0};
    memset(pfile,0x00,sizeof(pfile));
    sprintf(pfile,"%s/%s%d",PrivateDir,TASK_RECORD_FILE,TaskId);

    read_pdata_xram(pfile, (char*)&TaskRecord,0, sizeof(FUJIAN_TASK_RECORD_FILE_T));

    //��������¼�ļ��м�¼����Ϣ������ǰ�ִ���ͬ����ؼ�¼�еĳ�����
    if(memcmp(&TaskRecord.thisTurnBeginTime,&RunTask->thisTurnBeginTime,sizeof(OOP_DATETIME_S_T))==0)
    {
        //�ݲ�ȡ������ ���ݱ�ɹ�������ж�
        //RunTask->specificTaskIndex = TaskRecord.specificTaskIndex;
        RunTask->storageCnt = TaskRecord.storageCnt;
        //RunTask->meterIndex = TaskRecord.meterIndex;
        RunTask->result  = TaskRecord.result;
        //�ȸ��ɹ��������0
        RunTask->meterSucNum = 0;
        //������ǳɹ�״̬ һ���ٳ�һ��
        if(RunTask->result != SCHEME_FINISHED)
            RunTask->result = SCHEME_UNDONE;
        if(RunTask->meterNum!=TaskRecord.meterNum)
        {
            TASK_FMT_DEBUG(pipe, REC_LOG,"�����¼�ļ��ڵ������͵�ǰ����һ��RunTask->meterNum %d TaskRecord.meterNum %d \n",
                RunTask->meterNum,TaskRecord.meterNum);
        }
        for(i=0;i<RunTask->meterNum;i++)
        {
            flag = 0;
            for(j=0;j<TaskRecord.meterNum;j++)
            {
                if(memcmp(&RunTask->meterInfo[i].meterAddr,&TaskRecord.meterInfo[j].meterAddr,sizeof(OOP_TSA_T))==0)
                {
                    flag = 1;
                    RunTask->meterInfo[i].meterResult = TaskRecord.meterInfo[j].meterResult;
                    //û���ɹ��ı���ֵ
                    if(RunTask->meterInfo[i].meterResult == 2)
                    {
                        RunTask->meterSucNum++;
                    }
                    else
                    {
                        RunTask->meterInfo[i].meterResult = 0;
                    }
                    
                    break;
                }
            }
            if(flag == 0)
            {
                TASK_FMT_DEBUG(pipe, REC_LOG,"�б�δ���ɹ� \n");
                RunTask->result = SCHEME_UNDONE;
            }
        }
        
    }
    else  //�������¼�ļ�����Ϣ��ʼ��Ϊ��ǰ�ִ�
    {
        memset(&TaskRecord,0X00,sizeof(FUJIAN_TASK_RECORD_FILE_T));
        memcpy(&TaskRecord.thisTurnBeginTime,&RunTask->thisTurnBeginTime,sizeof(OOP_DATETIME_S_T));
        TaskRecord.taskid = RunTask->taskid;
        TaskRecord.meterNum = RunTask->meterNum;
        memcpy(TaskRecord.meterInfo,RunTask->meterInfo,sizeof(FUJIAN_METER_INFO_T)*MAX_METER_NUM);
        TASK_FMT_DEBUG(pipe, REC_LOG,"�����¼�ļ���������ǰ�ִ� \n");
        write_pdata_xram(pfile,(char*)&TaskRecord,0,sizeof(FUJIAN_TASK_RECORD_FILE_T));
    }
    TASK_FMT_DEBUG( pipe, REC_LOG, "LOAD �����¼�ļ��� specificTaskIndex %d meterIndex %d meterSucNum %d �洢��� %d\n",
        RunTask->specificTaskIndex,RunTask->meterIndex,RunTask->meterSucNum,RunTask->storageCnt);
    if(RunTask->result == SCHEME_FINISHED)
    {
        TASK_FMT_DEBUG(pipe, REC_LOG,"���� %d ��ǰ�ִ��Ѿ�ִ�гɹ� \n",RunTask->taskid);
    }
}

/*******************************************************************************
* ��������: taskmng_trans_task_record_file_write 
* ��������:д�����¼�ļ�
* �������: pipe �˿ں�
            TaskInfo ������Ϣ
            RunTask ��������ʱ��Ϣ
* �������: 
* �� �� ֵ: 
*******************************************************************************/
void taskmng_trans_task_record_file_write(uint8 pipe,FUJIAN_TASK_RUN_T* RunTask)
{
    FUJIAN_TASK_RECORD_FILE_T TaskRecord;
    memset(&TaskRecord,0x00,sizeof(FUJIAN_TASK_RECORD_FILE_T));
    uint16 TaskId = RunTask->taskid;

    char      pfile[100]={0};
    memset(pfile,0x00,sizeof(pfile));
    sprintf(pfile,"%s/%s%d",PrivateDir,TASK_RECORD_FILE,TaskId);

    read_pdata_xram(pfile, (char*)&TaskRecord,0, sizeof(FUJIAN_TASK_RECORD_FILE_T));
    
    TaskRecord.specificTaskIndex = RunTask->specificTaskIndex;
    TaskRecord.storageCnt = RunTask->storageCnt;
    TaskRecord.meterIndex = RunTask->meterIndex;
    TaskRecord.result = RunTask->result;
    TaskRecord.meterNum = RunTask->meterNum;
    memcpy(TaskRecord.meterInfo,RunTask->meterInfo,sizeof(FUJIAN_METER_INFO_T)*MAX_METER_NUM);
    write_pdata_xram(pfile,(char*)&TaskRecord,0,sizeof(FUJIAN_TASK_RECORD_FILE_T));
}


/*******************************************************************************
* ��������: taskmng_specific_task_get_from_index
* ��������: ���ݾ�������������ȡ��ǰ��Ҫִ�еľ�������ָ��
* �������: ��
* �������: ��
* �� �� ֵ: >0                  �ɹ�
*           <0                  ʧ��
*******************************************************************************/
int taskmng_specific_task_get_from_index(FUJIAN_TASK_RUN_T * runTask,uint16 index,FUJIAN_SPECIFIC_TASK_T **specificTask)
{
    uint16 i;
    FUJIAN_SPECIFIC_TASK_T *tmp=runTask->specificTask;
    for(i =0;i<index;i++)
    {
        if(tmp == NULL)
            return -1;
        tmp = tmp->next;
    }
    if(NULL == tmp)
        return -1;
    *specificTask = tmp;
    return 0;
}

/*******************************************************************************
* ��������: taskmng_trigger_flag_get
* ��������: ��ȡ�������
* �������: pipi ͨ����
            runTask ���񷽰�
            
* �������: ��
* �� �� ֵ: 0   
*******************************************************************************/
uint8 taskmng_trigger_flag_get(FUJIAN_TASK_RUN_T * runTask,OOP_TSA_T *tsa)
{
    uint16 i;
    uint8 flag = 0;
    for(i=0;i<runTask->meterNum;i++)
    {
        if(memcmp(tsa,&runTask->meterInfo[i].meterAddr,sizeof(OOP_TSA_T))==0)
        {
            flag = runTask->meterInfo[i].triggerFlag;
            break;
        }

    }
    return flag;
}

/*******************************************************************************
* ��������: taskmng_fully_next_specific_get
* ��������: �ж�������Ƿ��к�������
* �������: pipi ͨ����
            runTask ���񷽰�

* �������: ��
* �� �� ֵ: 
            0          �к�������
*           <0         û�к�������
*******************************************************************************/
int taskmng_fully_next_specific_get(uint8 pipe,FUJIAN_TASK_RUN_T * runTask,FUJIAN_SPECIFIC_TASK_T **outSpecificTask,uint16 meterIndex)
{
    uint8 flag = 0;
    uint16 i;
    int ret;
    FUJIAN_SPECIFIC_TASK_T *specificTask = NULL;
    TASK_FMT_DEBUG(pipe, REC_LOG,"specificTaskIndex %d \n",runTask->specificTaskIndex);
    ret = taskmng_specific_task_get_from_index(runTask,runTask->specificTaskIndex,&specificTask);
    if(ret<0)
    {
        return -1;
    }
    while(specificTask)
    {
        for(i =0;i<specificTask->addrAll.num;i++)
        {
            if(memcmp(&runTask->meterInfo[meterIndex].meterAddr ,&specificTask->addrAll.addr[i],sizeof(OOP_TSA_T)) == 0)
            {
                flag = 1;
                break;
            }
        }
        if(flag == 1)
        {
            *outSpecificTask = specificTask;
            return 0;
        }
        specificTask = specificTask->next;
        runTask->specificTaskIndex++;
    }
    return -1;
}


/*******************************************************************************
* ��������: taskmng_fully_trans_task_next_meter_get
* ��������: ȫ͸��ת�� ��ȡ��һ����Ҫ����ľ�������ͱ� ȫ͸�������Ϊ���ձ����� ��һ�������еľ�����������
            ��һ����
* �������: pipi ͨ����
            runTask ���񷽰�
            
* �������: ��
* �� �� ֵ: 0   �ҵ���һ��Ҫ���ı�
*           1                ȫ��������
           
*******************************************************************************/

int taskmng_fully_trans_task_next_meter_get(uint8 pipe,FUJIAN_TASK_RUN_T * runTask,OOP_TSA_T *outTsa,FUJIAN_SPECIFIC_TASK_T **SpecificTask)
{
    uint16 meterIndex = runTask->meterIndex;
    OOP_TSA_T tsa;
    int ret;
    //int index = -1;
    FUJIAN_SPECIFIC_TASK_T *currentSpecificTask = NULL;
    FUJIAN_SPECIFIC_TASK_T *tmp; 
    uint16 i;
    //���ݵ�ǰ������Ϣ��ȡ���͵ı���
    ret = taskmng_specific_task_get_from_index(runTask,runTask->specificTaskIndex, &currentSpecificTask);
    if(ret<0)
    {
        TASK_FMT_DEBUG(pipe, REC_LOG,"��ȡ��%d����������ʧ�� \n",runTask->specificTaskIndex);
        runTask->result = SCHEME_FAIL;
        return 1;
    }
    while(1)
    {
        if(meterIndex >= runTask->meterNum)
        {
            TASK_FMT_DEBUG(pipe, REC_LOG,"���񷽰� %d ִ����� \n",runTask->taskid);
            if(runTask->failFlag == 1)
            {
                runTask->result = SCHEME_FAIL;
                TASK_FMT_DEBUG(pipe, REC_LOG,"����״̬ʧ�� �ȴ����� \n");
            }else
            {
                runTask->result = SCHEME_FINISHED;
                TASK_FMT_DEBUG(pipe, REC_LOG,"����ȫ��ִ�гɹ� \n");
            }
            taskmng_trans_task_record_file_write(pipe,runTask);
            return 1;
        }
        if(runTask->meterInfo[meterIndex].meterResult ==2 )
        {
            meterIndex++;
            runTask->meterIndex = meterIndex;
            runTask->specificTaskIndex = 0;
            continue;
        }
        memcpy(&tsa,&runTask->meterInfo[meterIndex].meterAddr,sizeof(OOP_TSA_T));
        //�ж�����Щ��������
        if(runTask->transTask->taskTypeData.taskType == 5 && runTask->transTask->taskTypeData.addi_param.condition.object ==1)
        {
            if(taskmng_trigger_flag_get(runTask,&tsa) == 0)
            {
                meterIndex++;
                runTask->meterIndex = meterIndex;
                runTask->specificTaskIndex = 0;
                continue;
            }
        }

        if(runTask->meterInfo[meterIndex].failCnt >runTask->transTask->turnRetryNum  )
        {
            TASK_FMT_DEBUG(pipe, REC_LOG,"����ʧ�ܴ������� %d �б� ���ַ %02x%02x%02x%02x%02x%02x\n",runTask->meterInfo[meterIndex].failCnt,
                tsa.add[0],tsa.add[1],tsa.add[2],tsa.add[3],tsa.add[4],tsa.add[5]);
            runTask->failFlag = 1;
            //дһ��ʧ�ܵĽ�� �ɹ������½��
            //�ñ�ʧ�� ��¼���� ����ľ�������Ҳ������
            runTask->meterInfo[meterIndex].meterResult = 3;
            runTask->meterInfo[meterIndex].multiBufIndex = 0;
            runTask->meterInfo[meterIndex].execTime = time(NULL);
            //���ֻʧ����һ�� ��ֱ���ϱ���� ���ֱ�Ӷ��洢����������һ��ʧ�ܵĽ�� ����ִ��ʱ��᲻��

            ret = taskmng_trans_task_error_report(pipe, &tsa,runTask->meterInfo[meterIndex].execTime, runTask, currentSpecificTask);
            if(ret < 0)
            {
                taskmng_trans_task_result_save(pipe, 1, &tsa, runTask->meterInfo[meterIndex].execTime, runTask,
                     currentSpecificTask, OOP_FJ_ERR_RESQ_TIME_OUT, NULL, 0);
            }
            
            
           //�Ѻ���ľ�������������Ľ��дδִ��
            tmp = currentSpecificTask->next;
            while(tmp)
            {
                for(i=0;i<tmp->addrAll.num;i++)
                {
                    if(memcmp(&runTask->meterInfo[meterIndex].meterAddr,&tmp->addrAll.addr[i],sizeof(OOP_TSA_T)) == 0)
                    {
                        taskmng_trans_task_result_save(pipe,1,&runTask->meterInfo[meterIndex].meterAddr,runTask->meterInfo[meterIndex].execTime, runTask,tmp,OOP_FJ_ERR_TASK_NO_EXECUTE, NULL,0);
                        break;
                    }
                }
                tmp = tmp->next;
            }
            meterIndex++;
            runTask->meterIndex = meterIndex;
            runTask->specificTaskIndex = 0;
            continue;
        }

        //�ҵ���ǰ��Ҫ���ľ�������
        ret = taskmng_fully_next_specific_get(pipe,runTask,&currentSpecificTask,meterIndex);
        if(ret < 0 )
        {
            TASK_FMT_DEBUG( pipe, REC_LOG, "��� %02x%02x%02x%02x%02x%02x ������� \n",tsa.add[0],tsa.add[1],
                tsa.add[2],tsa.add[3],tsa.add[4],tsa.add[5]);

            runTask->meterInfo[meterIndex].meterResult = 2;
            meterIndex++;
            runTask->meterIndex = meterIndex;
            runTask->specificTaskIndex = 0;
            continue;
        }else
        {
            //�ҵ��������еı�
            memcpy(outTsa,&tsa,sizeof(OOP_TSA_T));
            *SpecificTask = currentSpecificTask;
            
            taskmng_trans_task_record_file_write(pipe,runTask);
            return 0;
        }

    }
}

/*******************************************************************************
* ��������: taskmng_fully_trans_task_send
* ��������: ȫ͸��ת�� ����
* �������: pipi ͨ����
            runTask ���񷽰�
            type 1��ʾ376.2����Э��͸��ת��ģʽ 2��ʾ376.2��������Э��V1.2͸��ת��ģʽ
* �������: ��
* �� �� ֵ: >0                  �ɹ�
*           <0                  ʧ��
*******************************************************************************/

int taskmng_fully_trans_task_send(uint8 pipe,FUJIAN_TASK_RUN_T * runTask,uint8 type)
{
    uint8 sendbuf[2000]={0};
    //uint8 recvbuf[2000]={0};
    uint16 sendlen;
    //uint16 recvlen = 0;
    uint8 dstbuf[2000]={0};
    int ret;
    //uint8 errorCode= 0;
    //uint8 result =0;
    OOP_TSA_T tsa;
    FUJIAN_SPECIFIC_TASK_T *currentSpecificTask = NULL;
    time_t timenow = time(NULL);
    uint16 meterindex ;
    uint8 i;
    uint16 index=0;
    FRAME_CONVERT_T convert;
    memset(&convert,0x00,sizeof(FRAME_CONVERT_T));
    memset(&tsa,0x00,sizeof(OOP_TSA_T));


    ret = taskmng_fully_trans_task_next_meter_get(pipe,runTask,&tsa,&currentSpecificTask);
    if(ret == 1)
    {
        return 1;
    }
    TASK_FMT_DEBUG(pipe, REC_LOG," �����ַ %02x%02x%02x%02x%02x%02x ��������� %d\n",
        tsa.add[0],tsa.add[1],tsa.add[2],tsa.add[3],tsa.add[4],tsa.add[5],currentSpecificTask->nodeid);

    //�ն˱���˿ڵ����� ����Ǹ����ɵ� �߼���ַ��1 ȫ�����͸�puamr����
    if(pipe == PLC_NUM + MAX_485_NUM + COMM_MODULE_NUM)
    {
        //��ַ���ն˵�ַ �Է������������ַ����
        memcpy_r(tsa.add, LocalAddr, tsa.len + 1);
        if(runTask->transTask->commType == OOP_COMM_TYPE_AC)
        {
            tsa.oldvxd = 1;
        }
    }
    if(pipe >= PLC_NUM && pipe < PLC_NUM+MAX_485_NUM)
    {
        if(fujian_rs485_init(pipe,&currentSpecificTask->uartParam)<0)
        {
            TASK_FMT_DEBUG(pipe, REC_LOG,"���ڳ�ʼ��ʧ�� \n");
            return -1;
        }
        //��֡645��Ҫһ��һ���ķ���
        meterindex = runTask->meterIndex;
        if(((currentSpecificTask->reqMessType>>4) & 0x0f) == 1  )
        {
            if(runTask->meterInfo[meterindex].multiBufIndex ==0)
            {
                convert.srcBuf = currentSpecificTask->reqMess.value;
                convert.srcLen = currentSpecificTask->reqMess.nNum;
                convert.dstBuf = dstbuf;
                Convert_Request_Frame(pipe,&convert,currentSpecificTask->reqMessType,&tsa);
                
                TASK_FMT_DEBUG(pipe, REC_LOG,"������ת��ǰ���� \n");
                TASK_BUF_DEBUG(pipe,REC_LOG,currentSpecificTask->reqMess.value,currentSpecificTask->reqMess.nNum);
                TASK_FMT_DEBUG(pipe, REC_LOG,"�����ĸ�ʽ�� 0x%02x ����ַ %02x%02x%02x%02x%02x%02x ת������ 645���ĸ���\n",currentSpecificTask->reqMessType,
                    tsa.add[0],tsa.add[1],tsa.add[2],tsa.add[3],tsa.add[4],tsa.add[5],convert.num645);
                TASK_BUF_DEBUG(pipe,REC_LOG,convert.dstBuf,convert.dstLen);
                memset(&gMulti645Buf[pipe-PLC_NUM],0x00,sizeof(FUJIAN_MULTI_645BUF_T));
                gMulti645Buf[pipe-PLC_NUM].num = convert.num645;
                for(i=0;i<convert.num645;i++)
                {
                    gMulti645Buf[pipe-PLC_NUM].databuf[i].buflen = convert.datalen645[i];
                    memcpy(gMulti645Buf[pipe-PLC_NUM].databuf[i].buf,dstbuf+index,convert.datalen645[i]);
                    index += convert.datalen645[i];
                }
            }
        }
        else
        {
            convert.srcBuf = currentSpecificTask->reqMess.value;
            convert.srcLen = currentSpecificTask->reqMess.nNum;
            convert.dstBuf = dstbuf;
            Convert_Request_Frame(pipe, &convert, currentSpecificTask->reqMessType, &tsa);
            
            TASK_FMT_DEBUG(pipe, REC_LOG, "������ת��ǰ���� \n");
            TASK_BUF_DEBUG(pipe, REC_LOG, currentSpecificTask->reqMess.value, currentSpecificTask->reqMess.nNum);
            TASK_FMT_DEBUG(pipe, REC_LOG, "�����ĸ�ʽ�� 0x%02x ����ַ %02x%02x%02x%02x%02x%02x ת������\n",
                 currentSpecificTask->reqMessType, 
                tsa.add[0], tsa.add[1], tsa.add[2], tsa.add[3], tsa.add[4], tsa.add[5]);
            TASK_BUF_DEBUG(pipe, REC_LOG, convert.dstBuf, convert.dstLen);

        }
    }else
    {
    
        convert.srcBuf = currentSpecificTask->reqMess.value;
        convert.srcLen = currentSpecificTask->reqMess.nNum;
        convert.dstBuf = dstbuf;
        Convert_Request_Frame(pipe, &convert, currentSpecificTask->reqMessType, &tsa);
    
        TASK_FMT_DEBUG(pipe, REC_LOG, "������ת��ǰ���� \n");
        TASK_BUF_DEBUG(pipe, REC_LOG, currentSpecificTask->reqMess.value, currentSpecificTask->reqMess.nNum);
        TASK_FMT_DEBUG(pipe, REC_LOG, "�����ĸ�ʽ�� 0x%02x ����ַ %02x%02x%02x%02x%02x%02x ת������\n",
             currentSpecificTask->reqMessType, 
            tsa.add[0], tsa.add[1], tsa.add[2], tsa.add[3], tsa.add[4], tsa.add[5]);
        TASK_BUF_DEBUG(pipe, REC_LOG, convert.dstBuf, convert.dstLen);
    }
    //�ز���Ҫ��һ����Ϣ
    if(pipe<PLC_NUM)
    {

        if(type == 1)
        {
            sendlen = fujian_send_payload_pack_0100(pipe, convert.dstBuf, convert.dstLen, sendbuf);
            ret = fujian_trans_task_buf_send(pipe, 0x0100, sendbuf, sendlen, &runTask->sendMsgIndex);
        }else
            {
            sendlen = fujian_send_payload_pack_0101(pipe, &tsa,convert.dstBuf, convert.dstLen, sendbuf);
            ret = fujian_trans_task_buf_send(pipe, 0x0101, sendbuf, sendlen, &runTask->sendMsgIndex);
        }
        
    }else if(pipe>=PLC_NUM && pipe<PLC_NUM+MAX_485_NUM)
     {
        if(((currentSpecificTask->reqMessType>>4) & 0x0f) == 1  )
        {
            
            uint8 bufindex = runTask->meterInfo[meterindex].multiBufIndex;
            ret = fujian_trans_task_buf_send(pipe,0,gMulti645Buf[pipe-PLC_NUM].databuf[bufindex].buf ,gMulti645Buf[pipe-PLC_NUM].databuf[bufindex].buflen,&runTask->sendMsgIndex);
            TASK_FMT_DEBUG(pipe, REC_LOG, "��֡645���� ���� index %d �ܹ� %d \n",bufindex,gMulti645Buf[pipe-PLC_NUM].num);
            TASK_BUF_DEBUG(pipe, REC_LOG, gMulti645Buf[pipe-PLC_NUM].databuf[bufindex].buf ,gMulti645Buf[pipe-PLC_NUM].databuf[bufindex].buflen);
        }else
        {
            ret = fujian_trans_task_buf_send(pipe,0,convert.dstBuf,convert.dstLen,&runTask->sendMsgIndex);
        }
    }
    else if(pipe ==PLC_NUM+MAX_485_NUM )
    {       
        sendlen = fujian_send_payload_pack_0012(pipe, convert.dstBuf, convert.dstLen, sendbuf);
        ret = fujian_trans_task_buf_send(pipe,0x0012,sendbuf,sendlen,&runTask->sendMsgIndex);
    }
    else if(pipe ==PLC_NUM+MAX_485_NUM+COMM_MODULE_NUM)
    {
        
        //���� ȫ�����͸�puamr����
        //�о�ͨ�Ŷ������������·����ն˱����
        //if(runTask->transTask->commType == 0)
        //{
            //ͨ��ͨ��͸����Ϣ���͸�puamr
            sendlen = fujian_send_payload_pack_0012(pipe, convert.dstBuf, convert.dstLen, sendbuf);
            ret = fujian_trans_task_buf_send(pipe,0x0012,sendbuf,sendlen,&runTask->sendMsgIndex);
       // }
//        else
//        {
//        
//            ret = taskManager_acMeter_revsnd(convert.dstBuf, convert.dstLen, recvbuf, &recvlen);
//        
//            if(ret != ERR_OK)
//            {
//                TASK_FMT_DEBUG(pipe, REC_LOG, "taskManager ��ȡ��������ʧ�� \n");
//                runTask->result = SCHEME_FAIL;
//                taskmng_trans_task_record_file_write(pipe, runTask);
//                return 1;
//            }
//            else
//            {
//                TASK_FMT_DEBUG(pipe, REC_LOG, "taskManager ��ȡ�������ݳɹ� \n");
//        
//                //�����յ��ı��� ������Ӧ���ĸ�ʽ�ֽ���
//                convert.srcBuf = recvbuf;
//                convert.srcLen = recvlen;
//                convert.dstBuf = dstbuf;
//                Convert_Response_Frame(pipe, &convert, currentSpecificTask->resMessType);
//        
//                TASK_FMT_DEBUG(pipe, REC_LOG, "���ձ���ת��ǰ���� \n");
//                TASK_BUF_DEBUG(pipe, REC_LOG, recvbuf, recvlen);
//                TASK_FMT_DEBUG(pipe, REC_LOG, "���ձ��ĸ�ʽ�� %d ת������\n", currentSpecificTask->reqMessType);
//                TASK_BUF_DEBUG(pipe, REC_LOG, convert.dstBuf, convert.dstLen);
//        
//                /*��ȡ���ص�ַΪ���ɵ�ַ*/
//                tsa.len = 5;
//                memcpy(tsa.add, LocalAddr, tsa.len + 1);
//        
//                //������֤
//                result = packet_verific_Ispass(pipe, &currentSpecificTask->resMessProc, &convert, runTask,
//                     currentSpecificTask, & tsa);
//                TASK_FMT_DEBUG(pipe, REC_LOG, "������֤��� %d \n", result);
//                errorCode = result + ERROR_SUCCESS_CODE_ADD;
//        
//                taskmng_trans_task_result_save(pipe, 1, &tsa, timenow, runTask, currentSpecificTask, errorCode,
//                     convert.dstBuf, convert.dstLen);
//        
//                taskmng_trans_task_collection_state_update(pipe, runTask, TRANS_TASK_SUCC, (void *) &timenow);
//        
//                //���о�������ͼ����� û�о��������
//                if(currentSpecificTask->next != NULL)
//                {
//                    runTask->specificTaskIndex++;
//                    taskmng_trans_task_record_file_write(pipe, runTask);
//                    return - 1;
//                }
//                else
//                {
//                    TASK_FMT_DEBUG(pipe, REC_LOG, "��������ִ�����\n");
//                    runTask->result = SCHEME_FINISHED;
//                    taskmng_trans_task_record_file_write(pipe, runTask);
//        
//                    return 1;
//        
//                }
//        
//            }
//        }
        
    }
    if(ret <0 )
    {
        TASK_FMT_DEBUG(pipe, REC_LOG,"���ķ���ʧ�� \n");
        return -1;
    }
    runTask->lastSendTime = timenow;
    taskmng_trans_task_collection_state_update(pipe,runTask,TRANS_TASK_EXEC,(void*)&timenow);
    return 0;
}

/*******************************************************************************
* ��������: taskmng_notice_object_get
* ��������: ��ȡԤ�泭������
* �������: pipi ͨ����
            runTask ���񷽰�

* �������: ��
* �� �� ֵ: 0 ��Ԥ�泭�������� 
            -1 ȫ��Ԥ������
*******************************************************************************/
int taskmng_notice_object_get(uint8 pipe,FUJIAN_TASK_RUN_T * runTask)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    uint16 num = 0;
    OOP_TSA_T tsaList[255];
    uint16 i,j;
    int ret;
    memset(tsaList,0x00,sizeof(tsaList));
    FUJIAN_SPECIFIC_TASK_T * tmp = NULL;

    if(runTask->noticeFailCnt > runTask->transTask->turnRetryNum)
    {
        TASK_FMT_DEBUG(pipe, REC_LOG,"Ԥ�泭��ʧ�ܴ������� ���ֲ��ٳ��� \n");
        runTask->failFlag = 1;
        return -1;
    }

    for(i=0;i<runTask->meterNum;i++)
    {
        if(runTask->meterInfo[i].meterResult ==0 )
        {
            //����Ǵ������� ���ж���
            if(runTask->transTask->taskTypeData.taskType == 5 && runTask->transTask->taskTypeData.addi_param.condition.object ==1)
            {
                if(runTask->meterInfo[i].triggerFlag == 0)
                {
                    runTask->meterInfo[i].meterResult = 2;
                    continue;
                }
            }
            if(runTask->meterInfo[i].failCnt > runTask->transTask->turnRetryNum)
            {   
                OOP_TSA_T tsa;
                memcpy(&tsa,&runTask->meterInfo[i].meterAddr,sizeof(OOP_TSA_T));
                TASK_FMT_DEBUG(pipe, REC_LOG,"����ʧ�ܴ������� %d ���ٽ���Ԥ�� specificTaskIndex %d ����ַ %02x%02x%02x%02x%02x%02x\n",runTask->meterInfo[i].failCnt,
                    runTask->meterInfo[i].specificTaskIndex,tsa.add[0],tsa.add[1],tsa.add[2],tsa.add[3],tsa.add[4],tsa.add[5]);
                runTask->meterInfo[i].meterResult = 3;
                runTask->failFlag = 1;
                //�����������д������δִ��

                taskmng_specific_task_get_from_index(runTask, runTask->meterInfo[i].specificTaskIndex,&tmp);
                ret = taskmng_trans_task_error_report(pipe,&runTask->meterInfo[i].meterAddr,runTask->meterInfo[i].execTime,runTask,tmp);
                if(ret<0)
                {
                    taskmng_trans_task_result_save(pipe,1,&runTask->meterInfo[i].meterAddr,runTask->meterInfo[i].execTime, runTask,tmp,OOP_FJ_ERR_RESQ_TIME_OUT, NULL,0);
                }
                tmp = tmp->next;
                while(tmp)
                {
                    for(j=0;j<tmp->addrAll.num;j++)
                    {
                        if(memcmp(&runTask->meterInfo[i].meterAddr,&tmp->addrAll.addr[j],sizeof(OOP_TSA_T)) == 0)
                        {
                            TASK_FMT_DEBUG(pipe, REC_LOG,"��������� %d δִ�� \n",tmp->nodeid);
                            taskmng_trans_task_result_save(pipe,1,&runTask->meterInfo[i].meterAddr,runTask->meterInfo[i].execTime, runTask,tmp,OOP_FJ_ERR_TASK_NO_EXECUTE, NULL,0);
                            break;
                        }
                    }
                    tmp = tmp->next;
                }
                continue;
            }
            runTask->meterInfo[i].meterResult = 1;
            //�ҵ�������һ��Ҫ���͵ľ�����������
            if(runTask->meterInfo[i].specificTaskIndex == 0)
            {
                tmp = runTask->specificTask;
                uint8 flag = 0;
                while(tmp)
                {
                    for(j=0;j<tmp->addrAll.num;j++)
                    {
                        if(memcmp(&runTask->meterInfo[i].meterAddr,&tmp->addrAll.addr[j],sizeof(OOP_TSA_T)) == 0)
                        {
                            flag = 1;
                            break;
                        }
                    }
                    if(flag == 1)
                        break;
                    tmp = tmp->next;
                    runTask->meterInfo[i].specificTaskIndex++;
                }
            }

            
            memcpy(&tsaList[num],&runTask->meterInfo[i].meterAddr,sizeof(OOP_TSA_T));
            num++;
            if(num >= pAmrThreadArg->maxNoticeNum)
                break;
        }
    }
    if(num == 0)
    {
        TASK_FMT_DEBUG(pipe, REC_LOG,"���ж���ȫ��Ԥ����� \n");
        return -1;
    }

    TASK_FMT_DEBUG(pipe, REC_LOG,"����Ԥ�������� %d \n",num);
    //��Ԥ�泭�������� 0095
    uint8 payload[3000]={0};
    uint16 recvlen;
    uint8 recvbuf[100]={0};
    uint16 index = 0;
    uint16 seq;

    uint8 lenoffset;
    OOP_OAD_U pipeoad = taskmng_pipe_oad_get( pipe);
    memcpy_r(&payload[index],&pipeoad.value,4);
    index+=4;
    memcpy_r(&payload[index],&num,2);
    index+=2;
    payload[index++] = 1;   //Ĭ��ȫ������ͨ����ʱ���� �Ⱦ��峭����ʱ���ٿ�Ҫ��Ҫ������ʱ
    lenoffset = set_len_offset(num,&payload[index]);
    index+=lenoffset;
    for(i=0;i<num;i++)
    {
        seq = i+1;
        memcpy_r(&payload[index],&seq,2);
        index+=2;
        payload[index++] = 3;       //���ܱ�
        memcpy(&payload[index],tsaList[i].add,6);
        index+=6;
        TASK_FMT_DEBUG(pipe, REC_LOG,"i %d Ԥ��ĵ���ַ 0x%02x%02x%02x%02x%02x%02x\n",i,
            tsaList[i].add[0],tsaList[i].add[1],tsaList[i].add[2],tsaList[i].add[3],tsaList[i].add[4],tsaList[i].add[5]);
    }

    ret = taskmng_msg_fun_deal(pipe,0x0003,0x0095,index,payload,&recvlen,recvbuf);
    if(ret <0 || recvbuf[0]==0)
    {
        TASK_FMT_DEBUG(pipe, REC_LOG,"Ԥ�泭������ʧ��\n");
    }
    return 0;
    
}

/*******************************************************************************
* ��������: fujian_notice_task_start 
* ��������: ����Ԥ������ִ�� 0094
* �������: pipe �˿ں�
* �������: 
* �� �� ֵ: 
*******************************************************************************/
uint8 fujian_notice_task_start(uint8 pipe)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    uint8 payload[20]={0};
    uint8 len = 0;

    int ret;
    OOP_OAD_U pipeoad = taskmng_pipe_oad_get( pipe);
    memcpy_r(&payload[len],(uint8*)&pipeoad.value,4);
    len+=4;
    payload[len++] = pAmrThreadArg->noticeMode;

    uint16 recvlen;
    uint8 recvbuf[100]={0};
    ret =taskmng_msg_fun_deal(pipe,0x0003,0x0094,len,payload,&recvlen,recvbuf);
    if(ret>0)
    {
        if(recvlen!=1)
            return 0;
        return recvbuf[0];
    }
    return 0;

}

/*******************************************************************************
* ��������: taskmng_notice_task_amr_info_ask
* ��������: Ԥ�泭�� �յ��������������Ĵ���
* �������: pipi ͨ����
            runTask ���񷽰�
            payload ��Ϣ����
            payloadLen ��Ϣ����
* �������: ��
* �� �� ֵ: 
            0          ��������
*           <0         ʧ��
*******************************************************************************/
int taskmng_notice_task_amr_info_ask(uint8 pipe,FUJIAN_TASK_RUN_T * runTask,MESSAGE_INFO_T * msgInfo)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    int ret;
    uint16 index = 0;
    uint8 addr[6];      //��ַ ���
    uint16 delay;
    int meterIndex = -1;
    uint16 i;
    FUJIAN_SPECIFIC_TASK_T *specificTask = NULL;
    FRAME_CONVERT_T convert;
    uint8 dstbuf[2000]={0};
    memset(&convert,0x00,sizeof(FRAME_CONVERT_T));
    
    index+=4;
    uint8 commType = msgInfo->playload[index++];
    memcpy(addr,&msgInfo->playload[index],6);
    index+=6;
    memcpy_r(&delay,&msgInfo->playload[index],2);
    index+=2;
    TASK_FMT_TRACE( pipe, REC_LOG, "��� %02x%02x%02x%02x%02x%02x ���󳭶����� �ŵ���ʱ %d \n",addr[0],addr[1],addr[2],addr[3],addr[4],
        addr[5],delay);
    //�ұ�
    for(i=0;i<runTask->meterNum;i++)
    {
        if(memcmp(addr,runTask->meterInfo[i].meterAddr.add,6) == 0)
        {
            meterIndex = i;
            break;
        }
    }
    if(meterIndex == -1)
    {
        TASK_FMT_TRACE( pipe, REC_LOG,"δ���������ҵ��ñ�\n");
        return -1;
    }
    ret = taskmng_specific_task_get_from_index(runTask,runTask->meterInfo[meterIndex].specificTaskIndex,&specificTask);
    if(ret<0)
    {
        TASK_FMT_TRACE( pipe, REC_LOG,"��ȡ��������ʧ�� specificTaskIndex %d \n",runTask->meterInfo[meterIndex].specificTaskIndex);
        return -1;
    }
    convert.srcBuf = specificTask->reqMess.value;
    convert.srcLen = specificTask->reqMess.nNum;
    convert.pipedelay = delay;
    convert.dstBuf = dstbuf;
    Convert_Request_Frame(pipe,&convert,specificTask->reqMessType,&runTask->meterInfo[meterIndex].meterAddr);
    
    TASK_FMT_DEBUG(pipe, REC_LOG,"������ת��ǰ���� ��������� %d\n",specificTask->nodeid);
    TASK_BUF_DEBUG(pipe,REC_LOG,specificTask->reqMess.value,specificTask->reqMess.nNum);
    TASK_FMT_DEBUG(pipe, REC_LOG,"�����ĸ�ʽ�� 0x%02x ת������\n",specificTask->reqMessType);
    TASK_BUF_DEBUG(pipe,REC_LOG,convert.dstBuf,convert.dstLen);

    //�鷢�ͱ���
    uint8 sendbuf[2000]={0};
    uint8 lenoffset;
    index = 0;
    memcpy(&sendbuf[index],msgInfo->playload,4);
    index+=4;
    memcpy_r(&sendbuf[index],&specificTask->transTaskid,2);
    index+=2;
    memcpy_r(&sendbuf[index],&specificTask->nodeid,2);
    index+=2;
    sendbuf[index++] = commType;
    memcpy(&sendbuf[index],addr,6);
    index+=6;
    sendbuf[index++] = convert.prtl;
    sendbuf[index++] = pAmrThreadArg->noticeMode;
    lenoffset = set_len_offset(convert.dstLen, &sendbuf[index]);
    index+=lenoffset;
    memcpy(&sendbuf[index],convert.dstBuf,convert.dstLen);
    index+=convert.dstLen;
    TASK_FMT_DEBUG(pipe, REC_LOG,"���ͳ������� \n");
    fujian_trans_task_buf_ack(pipe,0x0104,sendbuf,index,msgInfo->index,msgInfo->label);
    return 0;
}

/*******************************************************************************
* ��������: taskmng_notice_next_specific_get
* ��������: �ж�������Ƿ��к�������
* �������: pipi ͨ����
            runTask ���񷽰�

* �������: ��
* �� �� ֵ: 
            0          �к�������
*           <0         û�к�������
*******************************************************************************/
int taskmng_notice_next_specific_get(uint8 pipe,FUJIAN_TASK_RUN_T * runTask,FUJIAN_SPECIFIC_TASK_T *specificTask,uint16 meterIndex)
{
    uint8 flag = 0;
    uint16 i;
    TASK_FMT_DEBUG(pipe, REC_LOG,"meterIndex %d specificTaskIndex %d \n",meterIndex,runTask->meterInfo[meterIndex].specificTaskIndex);
    FUJIAN_SPECIFIC_TASK_T *tmp = specificTask->next;
    runTask->meterInfo[meterIndex].specificTaskIndex++;
    while(tmp)
    {
        for(i =0;i<tmp->addrAll.num;i++)
        {
            if(memcmp(&runTask->meterInfo[meterIndex].meterAddr ,&tmp->addrAll.addr[i],sizeof(OOP_TSA_T)) == 0)
            {
                flag = 1;
                break;
            }
        }
        if(flag == 1)
            break;
        tmp = tmp->next;
        runTask->meterInfo[meterIndex].specificTaskIndex++;
    }
    if(flag == 1)
        return 0;
    else
        return -1;
}

/*******************************************************************************
* ��������: taskmng_notice_task_data_recv
* ��������: Ԥ�泭�� �յ��ϱ���ͨ�Ŷ������ݺ�Ĵ���
* �������: pipi ͨ����
            runTask ���񷽰�
            payload ��Ϣ����
            payloadLen ��Ϣ����
* �������: ��
* �� �� ֵ: 
            0          ��������
*           <0         ʧ��
*******************************************************************************/
int taskmng_notice_task_data_recv(uint8 pipe,FUJIAN_TASK_RUN_T * runTask,MESSAGE_INFO_T* msgInfo)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    uint16 index = 0;
    uint16 transTaskId,specificTaskId,datalen;
    uint8 commType,lenoffset;
    uint8 addr[6]={0};
    uint8 databuf[2000]={0};
    int meterIndex = -1;
    uint16 i;
    int ret;
    uint8 result = 0;
    uint8 errorCode = 0;
    FUJIAN_SPECIFIC_TASK_T *specificTask = NULL;
    FRAME_CONVERT_T convert;
    uint8 dstbuf[2000]={0};
    
    memset(&convert,0x00,sizeof(FRAME_CONVERT_T));
    index+=4;
    memcpy_r(&transTaskId,&msgInfo->playload[index],2);
    index+=2;
    memcpy_r(&specificTaskId,&msgInfo->playload[index],2);
    index+=2;
    commType = msgInfo->playload[index++];
    memcpy(addr,&msgInfo->playload[index],6);
    index+=6;
    datalen = get_len_offset(&msgInfo->playload[index], &lenoffset);
    index+=lenoffset;
    memcpy(databuf,&msgInfo->playload[index],datalen);
    TASK_FMT_TRACE( pipe, REC_LOG, "��� %02x%02x%02x%02x%02x%02x �ϱ����� ���񷽰���%d ��������� %d\n",addr[0],addr[1],addr[2],addr[3],addr[4],
        addr[5],transTaskId,specificTaskId);
    TASK_BUF_TRACE( pipe, REC_LOG,databuf,datalen);

    //�ұ�
    for(i=0;i<runTask->meterNum;i++)
    {
        if(memcmp(addr,runTask->meterInfo[i].meterAddr.add,6) == 0)
        {
            meterIndex = i;
            break;
        }
    }
    if(meterIndex == -1)
    {
        TASK_FMT_TRACE( pipe, REC_LOG,"δ���������ҵ��ñ�\n");
        return 0;
    }
    ret = taskmng_specific_task_get_from_index(runTask,runTask->meterInfo[meterIndex].specificTaskIndex,&specificTask);
    if(ret<0)
        return 0;
    //������ϱ�������ź͵�ǰ���Ĳ�һ���� ���ʱ�򷵻�0 ������ȴ� Ҫ��Ȼ�ͻ�����Ԥ�� Ȼ���������쳣����
    if(specificTask->transTaskid!=transTaskId || specificTask->nodeid!=specificTaskId)
    {
        TASK_FMT_TRACE( pipe, REC_LOG,"�շ�������Ų�ͬ\n");
        return 0;
    }
    uint8 state = 2;
    time_t timenow = time(NULL);
    if(datalen < specificTask->resMinLen )
    {
        //�ظ��ı���С�������Ч������Ϊ����ʧ�� �ٴ�Ԥ��
        //���Ե�ʱ�����������Ч���ȸ�Ϊ�ܳ� ���ʱ�������ر���У�����
        state = 1;
        if(datalen == 1)
        {
            errorCode = OOP_FJ_ERR_RESQ_TIME_OUT;
        }else
        {
            errorCode = OOP_FJ_ERR_MESS_CHECK;
        }
        
        convert.dstBuf = dstbuf;
        memcpy(convert.dstBuf,databuf,datalen);
        convert.dstLen = datalen;
        runTask->meterInfo[meterIndex].meterResult = 0; //����Ԥ��
        runTask->meterInfo[meterIndex].failCnt++;
        taskmng_trans_task_collection_state_update(pipe,runTask,TRANS_TASK_EXEC,(void*)&timenow);
        fujian_col_state_update(pipe,&runTask->meterInfo[meterIndex].meterAddr,1);
        taskmng_trans_task_record_file_write(pipe,runTask);
        TASK_FMT_DEBUG(pipe, REC_LOG,"���볤��С�������Ч���� %d ʧ�ܴ��� %d\n",specificTask->resMinLen,runTask->meterInfo[meterIndex].failCnt);
    }else
    {
        //�����յ��ı��� ������Ӧ���ĸ�ʽ�ֽ���
        convert.srcBuf = databuf;
        convert.srcLen = datalen;
        convert.dstBuf = dstbuf;
        Convert_Response_Frame(pipe,&convert,specificTask->resMessType);
        

        TASK_FMT_DEBUG(pipe, REC_LOG,"���ձ��ĸ�ʽ�� 0x%02x ת������\n",specificTask->resMessType);
        TASK_BUF_DEBUG(pipe,REC_LOG,convert.dstBuf,convert.dstLen);

        //������֤
        result = packet_verific_Ispass(pipe,&specificTask->resMessProc ,&convert, runTask,specificTask,&runTask->meterInfo[meterIndex].meterAddr);
        TASK_FMT_DEBUG(pipe, REC_LOG,"������֤��� %d \n",result);
        errorCode = result+ERROR_SUCCESS_CODE_ADD;

        fujian_col_state_update(pipe,&runTask->meterInfo[meterIndex].meterAddr,0);
        taskmng_trans_task_collection_state_update(pipe,runTask,TRANS_TASK_SUCC,(void*)&timenow);
        ret = taskmng_notice_next_specific_get(pipe,runTask,specificTask,meterIndex);
        if(ret <0)
        {
            TASK_FMT_DEBUG(pipe, REC_LOG,"�ñ�ȫ������ û�к�������\n");
            state = 1;
            runTask->meterInfo[meterIndex].meterResult = 2;
            taskmng_trans_task_record_file_write(pipe,runTask);
        }else
        {
            TASK_FMT_DEBUG(pipe, REC_LOG,"�ñ��к������� specificTaskIndex %d \n",runTask->meterInfo[meterIndex].specificTaskIndex);
        }
    }
    runTask->meterInfo[meterIndex].execTime = timenow;

    taskmng_trans_task_result_save(pipe,0,&runTask->meterInfo[meterIndex].meterAddr,runTask->meterInfo[meterIndex].execTime,runTask,specificTask,errorCode,convert.dstBuf,convert.dstLen);
    
    //����� �жϻ���û���¸�����������Ҫ�� ���û�� ���øñ�ɹ�
    

    uint8 sendbuf[2000]={0};
    index = 0;
    memcpy(&sendbuf[index],msgInfo->playload,4);
    index+=4;
    memcpy_r(&sendbuf[index],&specificTask->transTaskid,2);
    index+=2;
    memcpy_r(&sendbuf[index],&specificTask->nodeid,2);
    index+=2;
    sendbuf[index++] = commType;
    memcpy(&sendbuf[index],addr,6);
    index+=6;
    sendbuf[index++] = pAmrThreadArg->noticeMode;
    sendbuf[index++] = state;
    sendbuf[index++] = 0;
    TASK_FMT_DEBUG(pipe, REC_LOG,"�ϱ�����ȷ�ϻظ� \n");
    fujian_trans_task_buf_ack(pipe,0x0105,sendbuf,index,msgInfo->index,msgInfo->label);
    //����µ�ǰԤ��ı��Ƿ񶼳�����
    uint8 flag = 0;
    for(i = 0;i<runTask->meterNum;i++)
    {
        if(runTask->meterInfo[i].meterResult == 1)
        {
            flag = 1;
            break;
        }
    }
    if(flag == 0)
    {
        TASK_FMT_DEBUG(pipe, REC_LOG,"��ǰԤ��Ķ���ȫ������\n");
        return -1;
    }
    return 0;
}

/*******************************************************************************
* ��������: taskmng_notice_trans_task_recv
* ��������: Ԥ�泭�� ���մ���
* �������: pipi ͨ����
            runTask ���񷽰�

* �������: ��
* �� �� ֵ: >0                  �����ڽ���̬
            0          �����л�������
*           <0               ���ճ�ʱ
*******************************************************************************/
int taskmng_notice_trans_task_recv(uint8 pipe,FUJIAN_TASK_RUN_T * runTask)
{
    uint8 recvbuf[2000]={0};
    uint16 recvlen;
    MESSAGE_INFO_T      *MsgInfo = NULL;
    int ret;
    uint16 i;
    switch(runTask->noticeState)
    {
        //�ȴ��ϱ������������� ���� �ϱ�Ԥ���ͨ�Ŷ�������
        case NOTICE_WAIT:
            {
                recvlen = taskmng_plc_queue_read(pAmrRecvDown_Cb[pipe], recvbuf);
                if(recvlen>0)
                {
                    MsgInfo = (MESSAGE_INFO_T*)recvbuf;
                    TASK_FMT_TRACE( pipe, REC_LOG, "Recv msgindex %d IOP %04X payload \n",MsgInfo->index,MsgInfo->IOP);
                    TASK_BUF_TRACE( pipe, REC_LOG,MsgInfo->playload, MsgInfo->msglen);
                    runTask->lastNoticeRecvTime = time(NULL);
                    //������������
                    if(MsgInfo->IOP == 0x0104)
                    {
                        taskmng_notice_task_amr_info_ask(pipe,runTask,MsgInfo);

                    }else if(MsgInfo->IOP == 0x0105)
                    {
                        ret = taskmng_notice_task_data_recv(pipe,runTask,MsgInfo);
                        if(ret <0 )
                        {
                            TASK_FMT_DEBUG(pipe, REC_LOG,"�ж��Ƿ���δԤ��Ķ��� \n");
                            runTask->noticeState = NOTICE_OBJECT;
                            return 0;
                        }
                    }
                }else
                {
                    //�ж��Ƿ�ʱ �������Ԥ���ʲô��û�յ� ������Ԥ�� 
                    if(runTask->lastNoticeRecvTime==0)
                    {
                        if(time(NULL) - runTask->lastSendTime > 300)
                        {
                            TASK_FMT_DEBUG(pipe, REC_LOG,"����Ԥ���ʱδ�յ������������� \n");
                            runTask->noticeFailCnt++;
                            runTask->noticeState = NOTICE_OBJECT;
                            for(i=0;i<runTask->meterNum;i++)
                            {
                                if(runTask->meterInfo[i].meterResult == 1)
                                {
                                    runTask->meterInfo[i].meterResult = 0;
                                    runTask->meterInfo[i].execTime = time(NULL);
                                    fujian_col_state_update(pipe,&runTask->meterInfo[i].meterAddr,1);
                                    FUJIAN_SPECIFIC_TASK_T * tmp = NULL;
                                    taskmng_specific_task_get_from_index(runTask, runTask->meterInfo[i].specificTaskIndex,&tmp);
                                    taskmng_trans_task_result_save(pipe,0,&runTask->meterInfo[i].meterAddr,runTask->meterInfo[i].execTime, runTask,tmp,OOP_FJ_ERR_MODULE_EXCEPTION, NULL,0);
                                }
                            }
                            taskmng_trans_task_record_file_write(pipe,runTask);
                            return 0;
                        }
                    }else
                    {
                        if(time(NULL) - runTask->lastNoticeRecvTime > 120)
                        {
                            TASK_FMT_DEBUG(pipe, REC_LOG,"��ʱδ�յ��κ��ϱ����� \n");
                            runTask->noticeFailCnt++;
                            //ʧ�ܵ���Ԥ��һ��
                            for(i=0;i<runTask->meterNum;i++)
                            {
                                if(runTask->meterInfo[i].meterResult == 1)
                                {
                                    fujian_col_state_update(pipe,&runTask->meterInfo[i].meterAddr,1);
                                    runTask->meterInfo[i].meterResult = 0;
                                    runTask->meterInfo[i].execTime = time(NULL);
                                    FUJIAN_SPECIFIC_TASK_T * tmp = NULL;
                                    if(taskmng_specific_task_get_from_index(runTask, runTask->meterInfo[i].specificTaskIndex,&tmp)<0)
                                    {
                                        TASK_FMT_DEBUG(pipe, REC_LOG,"��ȡ��������ʧ�� i %d specificTaskIndex %d \n",i,runTask->meterInfo[i].specificTaskIndex);
                                    }else
                                    {
                                        taskmng_trans_task_result_save(pipe,0,&runTask->meterInfo[i].meterAddr,runTask->meterInfo[i].execTime, runTask,tmp,OOP_FJ_ERR_MODULE_EXCEPTION, NULL,0);
                                    }
                                }

                            }
                            runTask->noticeState = NOTICE_OBJECT;
                            taskmng_trans_task_record_file_write(pipe,runTask);
                            return 0;
                        }
                    }
                }
            }
            break;
    
        default:
            return 0;
            break;
    }
    
    return 1;
    
}


/*******************************************************************************
* ��������: taskmng_notice_trans_task_send
* ��������: Ԥ�泭�� ����
* �������: pipi ͨ����
            runTask ���񷽰�

* �������: ��
* �� �� ֵ: 1 ����ִ�����
            0 �����ݷ���
*           <0                  ���л��շ�״̬
*******************************************************************************/
int taskmng_notice_trans_task_send(uint8 pipe,FUJIAN_TASK_RUN_T * runTask)
{
    int ret;
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    uint8 objectNum;

    switch(runTask->noticeState)
    {
        //��ѯ����ģ����� ��ȡһ��Ԥ�����Ķ������� ��ѯģ�鴮�ڲ��� �ж��¾ɳ���ģʽ
        case NOTICE_INIT:
            {
                objectNum = fujian_module_info_get(pipe);
                if(objectNum>0)
                {
                    pAmrThreadArg->maxNoticeNum = objectNum;
                    pAmrThreadArg->noticeMode = fujian_amr_mode_get(pipe);
                    TASK_FMT_DEBUG(pipe, REC_LOG,"��������Ԥ��������� %d ����ģʽ %d \n",objectNum,pAmrThreadArg->noticeMode );
                    //������ǰ����
                    //fujian_current_task_end(pipe);
                    runTask->noticeState = NOTICE_OBJECT;
                    return -1;
                }else
                    return -1;
            }
            break;
    
        case NOTICE_OBJECT:
            {
                //Ԥ�泭������
                //Ԥ��ǰ�����µ�ǰ����
                fujian_current_task_end(pipe);
                ret = taskmng_notice_object_get(pipe,runTask);
                if(ret<0)
                {
                    
                    if(runTask->failFlag == 1)
                    {
                        runTask->result = SCHEME_FAIL;
                        TASK_FMT_DEBUG(pipe, REC_LOG,"����ִ����� ��ʧ�� ������\n");
                    }
                    else
                    {
                        runTask->result = SCHEME_FINISHED;
                        TASK_FMT_DEBUG(pipe, REC_LOG,"����ִ����� ȫ���ɹ�\n");
                    }
                    runTask->noticeState  = NOTICE_INIT;
                    taskmng_trans_task_record_file_write(pipe,runTask);
                    return 1;
                }else
                {
                    //����Ԥ������ִ��
                    TASK_FMT_DEBUG(pipe, REC_LOG,"����Ԥ������ִ��\n");
                    fujian_notice_task_start(pipe);
                    time_t timenow = time(NULL);
                    runTask->lastSendTime = timenow;
                    runTask->noticeState = NOTICE_WAIT;
                    runTask->lastNoticeRecvTime = 0;
                    taskmng_trans_task_collection_state_update(pipe,runTask,TRANS_TASK_EXEC,(void*)&timenow);
                    return 0;
                }
            }
            break;

        default:
            runTask->noticeState = NOTICE_INIT;
            break;
    }
    return 0;
}

/*******************************************************************************
* ��������: taskmng_parallel_next_specific_get
* ��������: �ж�������Ƿ��к�������
* �������: pipi ͨ����
            runTask ���񷽰�

* �������: ��
* �� �� ֵ: 
            0          �к�������
*           <0         û�к�������
*******************************************************************************/
int taskmng_parallel_next_specific_get(uint8 pipe,FUJIAN_TASK_RUN_T * runTask,FUJIAN_SPECIFIC_TASK_T **outSpecificTask,uint16 meterIndex)
{
    uint8 flag = 0;
    uint16 i;
    int ret;
    FUJIAN_SPECIFIC_TASK_T *specificTask = NULL;
    TASK_FMT_DEBUG(pipe, REC_LOG,"specificTaskIndex %d \n",runTask->meterInfo[meterIndex].specificTaskIndex);
    ret = taskmng_specific_task_get_from_index(runTask,runTask->meterInfo[meterIndex].specificTaskIndex,&specificTask);
    if(ret<0)
    {
        return -1;
    }
    while(specificTask)
    {
        for(i =0;i<specificTask->addrAll.num;i++)
        {
            if(memcmp(&runTask->meterInfo[meterIndex].meterAddr ,&specificTask->addrAll.addr[i],sizeof(OOP_TSA_T)) == 0)
            {
                flag = 1;
                break;
            }
        }
        if(flag == 1)
        {
            *outSpecificTask = specificTask;
            return 0;
        }
        specificTask = specificTask->next;
        runTask->meterInfo[meterIndex].specificTaskIndex++;
    }
    return -1;
}


/*******************************************************************************
* ��������: taskmng_parallel_next_meter_get
* ��������:  ������������ ��ȡ��һ�����͵ı�;��������
* �������: pipi ͨ����
            runTask ���񷽰�

* �������: ��
* �� �� ֵ: 1 ȫ������
            0 �����ݷ���
*           <0    û���ݷ��� �����ȴ�����
*******************************************************************************/

int taskmng_parallel_next_meter_get(uint8 pipe,FUJIAN_TASK_RUN_T * runTask,FUJIAN_SPECIFIC_TASK_T **specificTask,uint16 *index)
{
    int meterIndex = -1;
    int ret;
    uint16 i;
    while(meterIndex < runTask->meterNum-1)
    {
        meterIndex++;
        if(runTask->meterSucNum>=runTask->meterNum)
        {
            TASK_FMT_DEBUG(pipe, REC_LOG,"�����������б�ȫ���������\n");
            if(runTask->failFlag == 1)
            {
                TASK_FMT_DEBUG(pipe, REC_LOG,"��ʧ�ܱ� ������\n");
                runTask->result = SCHEME_FAIL;
            }else
            {
                runTask->result = SCHEME_FINISHED;
            }
            taskmng_trans_task_record_file_write(pipe,runTask);
            return 1;
        }
        //TASK_FMT_DEBUG(pipe, REC_LOG,"runTask->meterInfo[%d].meterResult %d\n",meterIndex,runTask->meterInfo[meterIndex].meterResult);
        if(runTask->meterInfo[meterIndex].meterResult!=0)
        {
            continue;
        }
        if(runTask->meterInfo[meterIndex].failCnt > runTask->transTask->turnRetryNum)
        {
            TASK_FMT_DEBUG(pipe, REC_LOG,"��ʧ�ܴ������� %d \n",runTask->meterInfo[meterIndex].failCnt);
            FUJIAN_SPECIFIC_TASK_T * tmp = NULL;
            taskmng_specific_task_get_from_index(runTask, runTask->meterInfo[meterIndex].specificTaskIndex,&tmp);
            runTask->meterInfo[meterIndex].execTime = time(NULL);
            taskmng_trans_task_result_save(pipe,1,&runTask->meterInfo[meterIndex].meterAddr,runTask->meterInfo[meterIndex].execTime, runTask,tmp,OOP_FJ_ERR_RESQ_TIME_OUT, NULL,0);
            runTask->meterInfo[meterIndex].meterResult = 3;
            runTask->meterSucNum++;
            runTask->failFlag = 1;
            taskmng_trans_task_record_file_write(pipe,runTask);
            //�Ѻ���ľ�������������Ľ��дδִ��
            tmp = tmp->next;
            while(tmp)
            {
                for(i=0;i<tmp->addrAll.num;i++)
                {
                    if(memcmp(&runTask->meterInfo[meterIndex].meterAddr,&tmp->addrAll.addr[i],sizeof(OOP_TSA_T)) == 0)
                    {
                        taskmng_trans_task_result_save(pipe,1,&runTask->meterInfo[meterIndex].meterAddr,runTask->meterInfo[meterIndex].execTime, runTask,tmp,OOP_FJ_ERR_TASK_NO_EXECUTE, NULL,0);
                        break;
                    }
                }
                tmp = tmp->next;
            }
            continue;
        }
        if(runTask->transTask->taskTypeData.taskType == 5 && runTask->transTask->taskTypeData.addi_param.condition.object ==1)
        {
            if(runTask->meterInfo[meterIndex].triggerFlag ==0)
            {
                runTask->meterInfo[meterIndex].meterResult = 2;
                runTask->meterSucNum++;
                continue;
            }
        }
        //�ҵ�������Ҿ���������û������
        ret = taskmng_parallel_next_specific_get(pipe,runTask,specificTask,meterIndex);
        if(ret <0 )
        {
            TASK_FMT_DEBUG(pipe, REC_LOG,"�ñ����о�������ȫ���������\n");
            runTask->meterInfo[meterIndex].meterResult = 2;
            runTask->meterSucNum++;
            taskmng_trans_task_record_file_write(pipe,runTask);
            continue;
        }
        //�ҵ�Ҫ���ı�;�������
        runTask->meterInfo[meterIndex].meterResult = 1;
        *index = meterIndex;
        TASK_FMT_DEBUG(pipe, REC_LOG,"�ҵ���Ҫ����ĵ��meterIndex %d specificTaskIndex %d ��������� %d failCnt %d\n",meterIndex,
                    runTask->meterInfo[meterIndex].specificTaskIndex,(*specificTask)->nodeid,runTask->meterInfo[meterIndex].failCnt);
        return 0;
    }
    //û�ҵ�Ҫ���͵����� �Ȼ���
    return -1;
}

/*******************************************************************************
* ��������: taskmng_parallel_trans_task_send
* ��������:  ������������ ���� ��¼��ÿ�������ÿ������������ĳɹ�״̬
* �������: pipi ͨ����
            runTask ���񷽰�

* �������: ��
* �� �� ֵ: 1 ����ִ�����
            0 �����ݷ���
*           <0                  ���л��շ�״̬
*******************************************************************************/
int taskmng_parallel_trans_task_send(uint8 pipe,FUJIAN_TASK_RUN_T * runTask)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    uint8 i;
    int ret;
    FUJIAN_SPECIFIC_TASK_T *specificTask = NULL;
    uint16 meterIndex;
    FRAME_CONVERT_T convert;
    uint8 dstbuf[2000];
    OOP_TSA_T tsa;
    uint16 index;
    OOP_OAD_U pipeOAD = taskmng_pipe_oad_get(pipe); 
    uint16 msgIndex;
    //7������
    for(i=0;i<PARALLEL_QUEUE_NUM;i++)
    {
        if(pAmrThreadArg->AmrSendFrameInfo.Queue[i].ValidFlag==0)
        {
            ret = taskmng_parallel_next_meter_get(pipe,runTask,&specificTask,&meterIndex);
            if(ret>0)
            {
                return 1;
            }else if(ret <0)
            {
                return 0;
            }else
            {
                memset(&convert,0x00,sizeof(FRAME_CONVERT_T));
                memset(&dstbuf,0x00,sizeof(dstbuf));
                memcpy(&tsa,&runTask->meterInfo[meterIndex].meterAddr,sizeof(OOP_TSA_T));
                convert.srcBuf = specificTask->reqMess.value;
                convert.srcLen = specificTask->reqMess.nNum;
                convert.dstBuf = dstbuf;
                Convert_Request_Frame(pipe,&convert,specificTask->reqMessType,&runTask->meterInfo[meterIndex].meterAddr);
                TASK_FMT_DEBUG(pipe, REC_LOG,"������ת��ǰ���� \n");
                TASK_BUF_DEBUG(pipe,REC_LOG,specificTask->reqMess.value,specificTask->reqMess.nNum);
                TASK_FMT_DEBUG(pipe, REC_LOG,"�����ĸ�ʽ�� 0x%02x ����ַ %02x%02x%02x%02x%02x%02x ת������\n",specificTask->reqMessType,
                    tsa.add[0],tsa.add[1],tsa.add[2],tsa.add[3],tsa.add[4],tsa.add[5]);
                TASK_BUF_DEBUG(pipe,REC_LOG,convert.dstBuf,convert.dstLen);

                //�鷢�ͱ���
                uint8 sendbuf[2000]={0};
                uint8 lenoffset;
                index = 0;
                memcpy_r(&sendbuf[index],&pipeOAD.value,4);
                index+=4;
                index+=2;

                memcpy(&sendbuf[index],tsa.add,6);
                index+=6;
                lenoffset = set_len_offset(convert.dstLen, &sendbuf[index]);
                index+=lenoffset;
                memcpy(&sendbuf[index],convert.dstBuf,convert.dstLen);
                index+=convert.dstLen;
                fujian_trans_task_buf_send(pipe,0x003E,sendbuf,index,&msgIndex);
                TASK_FMT_DEBUG(pipe, REC_LOG,"�������������� ��Ϣ���� msgIndex %d \n",msgIndex);
                
                pAmrThreadArg->AmrSendFrameInfo.QueueCnt++;
                pAmrThreadArg->AmrSendFrameInfo.Queue[i].ValidFlag=1;
                pAmrThreadArg->AmrSendFrameInfo.Queue[i].MsIndex = meterIndex;
                pAmrThreadArg->AmrSendFrameInfo.Queue[i].CommBeginTime = time(NULL);
                memcpy(pAmrThreadArg->AmrSendFrameInfo.Queue[i].MeterAddr,tsa.add,6);
            }
        }


    }
    return 0;
}

/*******************************************************************************
* ��������: taskmng_seq_is_valid
* ��������: �ж�����֡�Ƿ���Ч֡
* �������: InSeq     ���жϵĵ���
* �������: OutSeq    �����Ӧ��֡����
* �� �� ֵ: 1         ��Ч
*           0         ��Ч
*******************************************************************************/
uint8 taskmng_seq_is_valid_fujian(uint8 Pipe,uint8* OutSeq, uint8 *addr)
{
    int i = 0;
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[Pipe];
    for(i = 0; i < PARALLEL_QUEUE_NUM; i++)
    {
        //����֡�����������Ч֮֡��

        if(( memcmp(pAmrThreadArg->AmrSendFrameInfo.Queue[i].MeterAddr,addr,6)==0)
                && (pAmrThreadArg->AmrSendFrameInfo.Queue[i].ValidFlag == 1))
        {
            *OutSeq = i;
            return 1;
        }
    }

    return 0;
}


/*******************************************************************************
* ��������: taskmng_parallel_task_data_recv
* ��������: ���������յ����ݺ�Ĵ���
* �������: pipi ͨ����
            runTask ���񷽰�
            payload ��Ϣ����
            payloadLen ��Ϣ����
* �������: ��
* �� �� ֵ: 
            0          ���ݴ���ɹ� �������̬
*           <0         ʧ��
*******************************************************************************/
int taskmng_parallel_task_data_recv(uint8 pipe,FUJIAN_TASK_RUN_T * runTask,uint8* payload,uint16 payloadLen)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    uint16 index = 0;
    uint16 datalen;
    uint8 lenoffset;
    uint8 addr[6]={0};
    uint8 databuf[2000]={0};
    int meterIndex = -1;

    int ret;
    uint8 result = 0;
    uint8 errorCode = 0;
    FUJIAN_SPECIFIC_TASK_T *specificTask = NULL;
    FRAME_CONVERT_T convert;
    uint8 dstbuf[2000]={0};
    uint8 seq;
    memset(&convert,0x00,sizeof(FRAME_CONVERT_T));
    
    index+=4;
    memcpy(addr,&payload[index],6);
    index+=6;
    index++;    //Э������
    datalen = get_len_offset(&payload[index], &lenoffset);
    index+=lenoffset;
    memcpy(databuf,&payload[index],datalen);
    TASK_FMT_TRACE( pipe, REC_LOG, "��� %02x%02x%02x%02x%02x%02x �������� \n",addr[0],addr[1],addr[2],addr[3],addr[4],
        addr[5]);
    TASK_BUF_TRACE( pipe, REC_LOG,databuf,datalen);

    if(taskmng_seq_is_valid_fujian(pipe,&seq,addr)==0)
    {
        TASK_FMT_DEBUG(pipe, REC_LOG,"δ��ȡ�����Ͷ������� \n");
        return -1;
    }
    meterIndex = pAmrThreadArg->AmrSendFrameInfo.Queue[seq].MsIndex;
    //��ȡ�����ľ�������
    ret = taskmng_specific_task_get_from_index(runTask,runTask->meterInfo[meterIndex].specificTaskIndex,&specificTask);
    if(ret<0)
        return -1;
    time_t timenow = time(NULL);
    if(datalen < specificTask->resMinLen )
    {
        //�ظ��ı���С�������Ч������Ϊ����ʧ�� �ٴ�Ԥ��
        if(datalen == 1)
        {
            errorCode = OOP_FJ_ERR_RESQ_TIME_OUT;
        }else
        {
            errorCode = OOP_FJ_ERR_MESS_CHECK;
        }
        
        convert.dstBuf = dstbuf;
        memcpy(convert.dstBuf,databuf,datalen);
        convert.dstLen = datalen;
        taskmng_trans_task_collection_state_update(pipe,runTask,TRANS_TASK_EXEC,(void*)&timenow);

        taskmng_parallel_task_meter_fail( pipe, seq,runTask);
        fujian_col_state_update(pipe,&runTask->meterInfo[meterIndex].meterAddr,1);
        taskmng_trans_task_record_file_write(pipe,runTask);
        TASK_FMT_DEBUG(pipe, REC_LOG,"���볤��С�������Ч����%d ʧ�ܴ��� %d\n",specificTask->resMinLen,runTask->meterInfo[meterIndex].failCnt);
    }else
    {

        //�����յ��ı��� ������Ӧ���ĸ�ʽ�ֽ���
        convert.srcBuf = databuf;
        convert.srcLen = datalen;
        convert.dstBuf = dstbuf;
        Convert_Response_Frame(pipe,&convert,specificTask->resMessType);
        

        TASK_FMT_DEBUG(pipe, REC_LOG,"���ձ��ĸ�ʽ�� 0x%02x ת������\n",specificTask->reqMessType);
        TASK_BUF_DEBUG(pipe,REC_LOG,convert.dstBuf,convert.dstLen);

        //������֤
        result = packet_verific_Ispass(pipe,&specificTask->resMessProc ,&convert, runTask,specificTask,&runTask->meterInfo[meterIndex].meterAddr);
        TASK_FMT_DEBUG(pipe, REC_LOG,"������֤��� %d \n",result);
        errorCode = result+ERROR_SUCCESS_CODE_ADD;

        fujian_col_state_update(pipe,&runTask->meterInfo[meterIndex].meterAddr,0);
        taskmng_trans_task_collection_state_update(pipe,runTask,TRANS_TASK_SUCC,(void*)&timenow);
        runTask->meterInfo[meterIndex].specificTaskIndex++;
        runTask->meterInfo[meterIndex].meterResult = 0;
        pAmrThreadArg->AmrSendFrameInfo.Queue[seq].ValidFlag=0;
        if(pAmrThreadArg->AmrSendFrameInfo.QueueCnt>0)
            pAmrThreadArg->AmrSendFrameInfo.QueueCnt--;
    }
    //�洢���� �洢����Ϊ0ʱ���洢

    taskmng_trans_task_result_save(pipe,0,&runTask->meterInfo[meterIndex].meterAddr,timenow,runTask,specificTask,errorCode,convert.dstBuf,convert.dstLen);
    


    return 0;
}

/*******************************************************************************
* ��������: taskmng_pn_state_refresh
* ��������: ˢ��Pn��ʧ�������б�
* �������: SeqIndex  ֡���
            MeterPrtlData �ϱ���645֡��Ϣ
* �������: ��
* �� �� ֵ: ��
*******************************************************************************/
void taskmng_parallel_task_meter_fail(uint8 Pipe,uint8 SeqIndex,FUJIAN_TASK_RUN_T * runTask)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[Pipe];

    uint16 MtrIndex = pAmrThreadArg->AmrSendFrameInfo.Queue[SeqIndex].MsIndex;

     pAmrThreadArg->AmrSendFrameInfo.Queue[SeqIndex].ValidFlag = 0;
    if( pAmrThreadArg->AmrSendFrameInfo.QueueCnt> 0)
        pAmrThreadArg->AmrSendFrameInfo.QueueCnt--;

    runTask->meterInfo[MtrIndex].failCnt++;
    runTask->meterInfo[MtrIndex].meterResult = 0;

    return;
}


/*******************************************************************************
* ��������: taskmng_parallel_trans_task_recv
* ��������: �������� ���մ���
* �������: pipi ͨ����
            runTask ���񷽰�

* �������: ��
* �� �� ֵ: >0                  �����ڽ���̬
            0          �����л�������
*           <0               ���ճ�ʱ
*******************************************************************************/

int taskmng_parallel_trans_task_recv(uint8 pipe,FUJIAN_TASK_RUN_T * runTask)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];

    uint8 recvbuf[2000]={0};
    uint16 recvlen;
    MESSAGE_INFO_T      *MsgInfo = NULL;
    int ret;
    uint16 i;

    recvlen = taskmng_plc_queue_read(pAmrRecvDown_Cb[pipe], recvbuf);
    if(recvlen>0)
    {
        MsgInfo = (MESSAGE_INFO_T*)recvbuf;
        TASK_FMT_TRACE( pipe, REC_LOG, "Recv msgindex %d IOP %04X payload \n",MsgInfo->index,MsgInfo->IOP);
        TASK_BUF_TRACE( pipe, REC_LOG,MsgInfo->playload, MsgInfo->msglen);

        //�յ��ϱ�����
        if(MsgInfo->IOP == 0x0031)
        {
            ret = taskmng_parallel_task_data_recv(pipe,runTask,MsgInfo->playload, MsgInfo->msglen);
            if(ret ==0 )
                return 0;
            else 
                return 1;
        }
    }
    else
    {
        uint8 flag = 0;
        for(i=0;i<PARALLEL_QUEUE_NUM;i++)
        {
            if(pAmrThreadArg->AmrSendFrameInfo.Queue[i].ValidFlag==1)
            {
                flag =1;
                if((time(NULL)-pAmrThreadArg->AmrSendFrameInfo.Queue[i].CommBeginTime)>90)
                {
                    TASK_FMT_DEBUG( pipe, REC_LOG, "Queue index %d ��ʱ meterindex %d \n",i,pAmrThreadArg->AmrSendFrameInfo.Queue[i].MsIndex);
                    taskmng_parallel_task_meter_fail( pipe, i,runTask);
                    fujian_col_state_update(pipe,&runTask->meterInfo[pAmrThreadArg->AmrSendFrameInfo.Queue[i].MsIndex].meterAddr,0);
                    pAmrThreadArg->TotalState.NormalAmr = RTS_SEND;
                }
            }
        }
        if(flag==0)
        {
            TASK_FMT_TRACE( pipe, REC_LOG, "���Ͷ�����Ч �л������� \n");
            pAmrThreadArg->TotalState.NormalAmr = RTS_SEND;
        }

    }
    return 1;
}

/*******************************************************************************
* ��������: taskmng_fully_trans_task_recv
* ��������: ȫ͸��ת�� ����
* �������: pipi ͨ����
            runTask ���񷽰�
            type 1��ʾ376.2����Э��͸��ת��ģʽ 2��ʾ376.2��������Э��V1.2͸��ת��ģʽ
* �������: ��
* �� �� ֵ: >0                  �����ڽ���̬
            0          �����л�������
*           <0               ���ճ�ʱ
*******************************************************************************/

int taskmng_fully_trans_task_recv(uint8 pipe,FUJIAN_TASK_RUN_T * runTask,uint8 type)
{
    uint8 recvbuf[2000]={0};
    uint16 recvlen;
    uint16 overtime = 90;
    MESSAGE_INFO_T      *MsgInfo = NULL;
    uint8 errorCode= 0;
    uint8 databuf[5000]={0};
    uint16 datalen = 0;
    uint8 dstbuf[5000]={0};
    int ret;
    uint8 result;
    OOP_TSA_T tsa;
    FUJIAN_SPECIFIC_TASK_T *currentSpecificTask = NULL;
    FRAME_CONVERT_T convert;
    memset(&convert,0x00,sizeof(FRAME_CONVERT_T));
    memset(&tsa,0x00,sizeof(OOP_TSA_T));
        
    uint16 meterindex = runTask->meterIndex;
    //���ݵ�ǰ������Ϣ��ȡ���͵ı���
    ret = taskmng_specific_task_get_from_index(runTask,runTask->specificTaskIndex, &currentSpecificTask);
    if(ret<0)
    {
        TASK_FMT_DEBUG(pipe, REC_LOG,"��ȡ��%d����������ʧ�� \n",runTask->specificTaskIndex);
        runTask->result = SCHEME_FAIL;
        return -1;
    }
    //memcpy(&tsa,&currentSpecificTask->addrAll.addr[meterindex],sizeof(OOP_TSA_T));
    memcpy(&tsa,&runTask->meterInfo[meterindex].meterAddr,sizeof(OOP_TSA_T));
    if(pipe<PLC_NUM)
    {
        recvlen = taskmng_plc_queue_read(pAmrRecvDown[pipe], recvbuf);
        if(recvlen>0)
        {
            MsgInfo = (MESSAGE_INFO_T*)recvbuf;
            TASK_FMT_TRACE( pipe, REC_LOG, "Recv msgindex %d IOP %04X payload \n",MsgInfo->index,MsgInfo->IOP);
            TASK_BUF_TRACE( pipe, REC_LOG,MsgInfo->playload, MsgInfo->msglen);
            if(MsgInfo->msglen != runTask->sendMsgIndex)
            {
                TASK_FMT_TRACE( pipe, REC_LOG, "��Ϣ��Ų��� ���͵��� %d \n",runTask->sendMsgIndex);
                return 1;
            }
            if(type == 1)
            {
                datalen = fujian_recv_payload_unpack_0100(MsgInfo->playload,MsgInfo->msglen,databuf);
            }else
                {
                datalen = fujian_recv_payload_unpack_0101(MsgInfo->playload,MsgInfo->msglen,databuf);
            }
            if(datalen==0)
            {
                TASK_FMT_TRACE( pipe, REC_LOG, "�յ�����Ϊ�� \n");
                fujian_col_state_update(pipe,&tsa,1);
                //runTask->thisTurnRetryCnt++;
                runTask->meterInfo[meterindex].failCnt++;
                //currentSpecificTask->addrAll.retryCnt[meterindex]++;
                return 0;
            }
            
        }else
        {
            if(time(NULL) - runTask->lastSendTime > overtime)
            {
                TASK_FMT_DEBUG(pipe, REC_LOG,"���� %d ��������%d ��ʱ\n",runTask->taskid,currentSpecificTask->nodeid);
                fujian_col_state_update(pipe,&tsa,1);
                //currentSpecificTask->addrAll.retryCnt[meterindex]++;
                runTask->meterInfo[meterindex].failCnt++;
                return -1;
            }
            return 1;
        }
    }else if(pipe>=PLC_NUM && pipe<PLC_NUM+MAX_485_NUM)
    {
        datalen = fujian_rs485_recv(pipe,3,&currentSpecificTask->uartParam,databuf,sizeof(databuf));
        if(datalen == 0)
        {
            TASK_FMT_TRACE( pipe, REC_LOG, "485��ʱ \n");
            fujian_col_state_update(pipe,&tsa,1);
            //runTask->thisTurnRetryCnt++;
            //currentSpecificTask->addrAll.retryCnt[meterindex]++;
            runTask->meterInfo[meterindex].failCnt++;
            return 0;
        }
    }else                                                               // ͨ��ģ��
    {
        recvlen = taskmng_plc_queue_read(pAmrRecvDown[pipe], recvbuf);
        if(recvlen>0)
        {
            MsgInfo = (MESSAGE_INFO_T*)recvbuf;
            TASK_FMT_DEBUG( pipe, REC_LOG, "Recv msgindex %d IOP %04X payload \n",MsgInfo->index,MsgInfo->IOP);
            TASK_BUF_DEBUG( pipe, REC_LOG,MsgInfo->playload, MsgInfo->msglen);
            if(MsgInfo->index != runTask->sendMsgIndex)
            {
                TASK_FMT_TRACE( pipe, REC_LOG, "��Ϣ��Ų��� ���͵��� %d \n",runTask->sendMsgIndex);
                return 1;
            }
            datalen=fujian_recv_payload_unpack_0012(MsgInfo->playload,MsgInfo->msglen,databuf);
            if(datalen==0)
            {
                TASK_FMT_DEBUG( pipe, REC_LOG, "�յ�����Ϊ�� \n");
                //currentSpecificTask->addrAll.retryCnt[meterindex]++;
                runTask->meterInfo[meterindex].failCnt++;
                return 0;
            }
            
        }else
        {
            if(time(NULL) - runTask->lastSendTime > 10)
            {
                TASK_FMT_DEBUG(pipe, REC_LOG,"���� %d ��������%d ��ʱ\n",runTask->taskid,currentSpecificTask->nodeid);
                //currentSpecificTask->addrAll.retryCnt[meterindex]++;
                runTask->meterInfo[meterindex].failCnt++;
                return -1;
            }
            return 1;
        }
    }
    //645��֡��Ҫ���⴦��
    if(pipe>=PLC_NUM && pipe<PLC_NUM+MAX_485_NUM && ((currentSpecificTask->reqMessType>>4) & 0x0f) == 1)
    {
        if(runTask->meterInfo[meterindex].multiBufIndex == 0)
        {
            memset(&gMulti645Recv[pipe-PLC_NUM],0x00,sizeof(FUJIAN_MULTI_645_RECV_T));
        }
        TASK_FMT_DEBUG(pipe, REC_LOG,"��֡645�յ����� multiBufIndex %d \n",runTask->meterInfo[meterindex].multiBufIndex);
        TASK_BUF_DEBUG(pipe,REC_LOG,databuf,datalen);
        
        memcpy(gMulti645Recv[pipe-PLC_NUM].recvbuf+gMulti645Recv[pipe-PLC_NUM].len,databuf,datalen);
        gMulti645Recv[pipe-PLC_NUM].len += datalen;

        if(runTask->meterInfo[meterindex].multiBufIndex >= gMulti645Buf[pipe-PLC_NUM].num-1)
        {
            //һ��������������ж�֡645ȫ�������� ��������ȥ
            memcpy(databuf,gMulti645Recv[pipe-PLC_NUM].recvbuf,gMulti645Recv[pipe-PLC_NUM].len );
            datalen = gMulti645Recv[pipe-PLC_NUM].len ;
        }else
        {
            runTask->meterInfo[meterindex].multiBufIndex++;
            return 0;
        }
    }
    
    time_t timenow = time(NULL);
    if(datalen < currentSpecificTask->resMinLen  )  
    {
        TASK_FMT_DEBUG(pipe, REC_LOG,"���볤��С�������Ч����%d\n",currentSpecificTask->resMinLen);
        //errorCode = OOP_FJ_ERR_MODULE_EXCEPTION;
        if(datalen == 1)
        {
            errorCode = OOP_FJ_ERR_RESQ_TIME_OUT;
        }else
        {
            errorCode = OOP_FJ_ERR_MESS_CHECK;
        }
        convert.dstBuf = dstbuf;
        memcpy(convert.dstBuf,databuf,datalen);
        convert.dstLen = datalen;
        //currentSpecificTask->addrAll.retryCnt[meterindex]++;
        runTask->meterInfo[meterindex].failCnt++;
        fujian_col_state_update(pipe,&tsa,1);
        taskmng_trans_task_collection_state_update(pipe,runTask,TRANS_TASK_EXEC,(void*)&timenow);
    } 
    else
    {

        TASK_FMT_DEBUG(pipe, REC_LOG,"���ձ���ת��ǰ���� \n");
        TASK_BUF_DEBUG(pipe,REC_LOG,databuf,datalen);
        //�����յ��ı��� ������Ӧ���ĸ�ʽ�ֽ���
        convert.srcBuf = databuf;
        convert.srcLen = datalen;
        convert.dstBuf = dstbuf;
        Convert_Response_Frame(pipe,&convert,currentSpecificTask->resMessType);
        
        TASK_FMT_DEBUG(pipe, REC_LOG,"���ձ��ĸ�ʽ�� 0x%02x ת������\n",currentSpecificTask->resMessType);
        TASK_BUF_DEBUG(pipe,REC_LOG,convert.dstBuf,convert.dstLen);


        //������֤
        result = packet_verific_Ispass(pipe,&currentSpecificTask->resMessProc ,&convert, runTask,currentSpecificTask,&tsa);

        TASK_FMT_DEBUG(pipe, REC_LOG,"������֤��� %d \n",result);
        errorCode = result+ERROR_SUCCESS_CODE_ADD;
        
        fujian_col_state_update(pipe,&tsa,0);
        //currentSpecificTask->addrAll.result[meterindex] = 1;
        if(currentSpecificTask->next != NULL)
        {
            runTask->specificTaskIndex++;
            runTask->meterInfo[meterindex].multiBufIndex = 0;
        }else
        {
            TASK_FMT_DEBUG( pipe, REC_LOG, "��� %02x%02x%02x%02x%02x%02x ������� \n",tsa.add[0],tsa.add[1],
                tsa.add[2],tsa.add[3],tsa.add[4],tsa.add[5]);
            runTask->meterInfo[meterindex].meterResult = 2;
            runTask->meterInfo[meterindex].multiBufIndex = 0;
            runTask->meterIndex++;
            runTask->specificTaskIndex = 0;
        }
        taskmng_trans_task_collection_state_update(pipe,runTask,TRANS_TASK_SUCC,(void*)&timenow);
        
        taskmng_trans_task_record_file_write(pipe,runTask);
    }

    taskmng_trans_task_result_save(pipe,0,&tsa,timenow,runTask,currentSpecificTask,errorCode,convert.dstBuf,convert.dstLen);

    
    return 0;

}


/*******************************************************************************
* ��������: taskmng_trans_task_send
* ��������: ��ʱ�������ݷ���
* �������: pipi ͨ����
            runTask ���񷽰�
* �������: ��
* �� �� ֵ: 1 ����ִ�����
            0 �����ݷ���
*           <0                  ʧ��
*******************************************************************************/
int taskmng_scheduled_task_send(uint8 pipe,FUJIAN_TASK_RUN_T * runTask)
{
    
    OOP_TRANS_TASK_T * transTask = runTask->transTask;
    //485ֻ֧�� ȫ͸��ת����͸���ŵ��ѱ�
    if(pipe>=PLC_NUM && pipe<PLC_NUM+MAX_485_NUM)
    {
        if(transTask->packType!=0 && transTask->packType!=9)
        {
            TASK_FMT_DEBUG(pipe, REC_LOG,"��ǰ�˿ڲ�֧�ֵ�Э�������ʽ %d \n",transTask->packType);
            runTask->result = SCHEME_FAIL;
            return -1;
        }
    }
    if(pipe==PLC_NUM+MAX_485_NUM )                   //�����߲���app�����Ķ˿� 3
    {
        if(transTask->packType!=0)
        {
            TASK_FMT_DEBUG(pipe, REC_LOG,"��ǰ�˿�%d ��֧�ֵ�Э�������ʽ %d \n",pipe,transTask->packType);
            runTask->result = SCHEME_FAIL;
            return -1;
        }
    }
    if(pipe==PLC_NUM+MAX_485_NUM+COMM_MODULE_NUM)                //���ɶ˿�
    {
        if(transTask->packType!=0)
        {
            TASK_FMT_DEBUG(pipe, REC_LOG,"��ǰ�˿ڽ��� ��֧�ֵ�Э�������ʽ %d \n",transTask->packType);
            runTask->result = SCHEME_FAIL;
            return -1;
        }
    }
    
    
    switch(transTask->packType)
    {
        case 0: //ȫ͸��ת��
        case 1: //376.2����Э��͸��ת��ģʽ AFN=02H_F1H
            return taskmng_fully_trans_task_send(pipe, runTask, 1);
    
        case 2: //376.2��������Э��V1.2͸��ת��ģʽ��AFN=52H_F1H��
            return taskmng_fully_trans_task_send(pipe, runTask, 2);
    
        case 3: //376.2��������Э��V1.2����Ԥ�泭��(�㲥)��ʽ
            return taskmng_notice_trans_task_send(pipe, runTask);

        case 4: //376.2 ������������ʽ
            return taskmng_parallel_trans_task_send(pipe, runTask);
        default:
            break;
    }
    
    
    return - 1;
    
}

/*******************************************************************************
* ��������: taskmng_scheduled_task_recv
* ��������: ��ʱ�������ݽ��ܴ���
* �������: pipi ͨ����
            runTask ���񷽰�
* �������: ��
* �� �� ֵ:  >0                  �����ڽ���̬
            0          �����л�������
*           <0               ���ճ�ʱ
*******************************************************************************/
int taskmng_scheduled_task_recv(uint8 pipe,FUJIAN_TASK_RUN_T * runTask)
{

    OOP_TRANS_TASK_T * transTask = runTask->transTask;

    switch(transTask->packType)
    {
        case 0: //ȫ͸��ת��
        case 1: //376.2����Э��͸��ת��ģʽ AFN=02H_F1H
            return taskmng_fully_trans_task_recv(pipe, runTask, 1);
    
        case 2: //376.2��������Э��V1.2͸��ת��ģʽ��AFN=52H_F1H��
            return taskmng_fully_trans_task_recv(pipe, runTask, 2);
    
        case 3: //376.2��������Э��V1.2����Ԥ�泭��(�㲥)��ʽ
            return taskmng_notice_trans_task_recv(pipe, runTask);

        case 4://376.2 ������������ʽ
            return taskmng_parallel_trans_task_recv(pipe,runTask);
        default:
            break;
    }
    
    return 0;
}


/*******************************************************************************
* ��������: taskmng_scheduled_task_deal
* ��������: ��ʱ�����ִ���߼�
* �������: pipi ͨ����
            runTask ���񷽰�
* �������: ��
* �� �� ֵ: >0                  �ɹ�
*           <0                  ʧ��
*******************************************************************************/
int taskmng_scheduled_task_deal(uint8 pipe,FUJIAN_TASK_RUN_T * runTask)
{
    
    AmrThreadArg_T * pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    int ret;
    FUJIAN_SPECIFIC_TASK_T *currentSpecificTask = NULL;
    uint16 i;
    switch(pAmrThreadArg->TotalState.NormalAmr)
    {
        case RTS_IDLE:
            {
                taskmng_pipe_collection_state_write(pipe,runTask->transTask->taskTypeData.taskType,runTask->taskid,runTask->priority);
                taskmng_trans_task_record_file_load(pipe,runTask);
                taskmng_comm_recv_buff_clear(pipe);
                if(pipe<PLC_NUM)
                {
                    fujian_current_task_end(pipe);
                    memset(&pAmrThreadArg->AmrSendFrameInfo,0x00,sizeof(SendFrameQueue_T));
                    if(runTask->transTask->taskTypeData.taskType == 3)
                    {
                        runTask->noticeState = NOTICE_INIT;
                        //Ԥ����������ִ��ʱ��ÿ�����״̬����һ�� �Է����񱻴������ִ�к�������
                        for(i=0;i<runTask->meterNum;i++)
                        {
                            if(runTask->meterInfo[i].meterResult == 1)
                            {
                                runTask->meterInfo[i].meterResult = 0;
                            }
                        }
                    }
                }
                if(pipe>=PLC_NUM && pipe<PLC_NUM+MAX_485_NUM)
                {
                    pAmrThreadArg->TotalState.NormalAmr = RTS_485_SET;
                }
                else
                {
                    pAmrThreadArg->TotalState.NormalAmr = RTS_SEND;
                }
            }
            break;
        case RTS_485_SET:
            {
                ret = taskmng_specific_task_get_from_index(runTask,runTask->specificTaskIndex,&currentSpecificTask);
                if(ret<0)
                {
                    TASK_FMT_DEBUG(pipe, REC_LOG,"��ȡ��%d����������ʧ�� \n",runTask->specificTaskIndex);
                    runTask->result = SCHEME_FAIL;
                    pAmrThreadArg->TotalState.NormalAmr = RTS_IDLE;
                    return -1;
                }
                if(fujian_rs485_init(pipe,&currentSpecificTask->uartParam)<0)
                {
                    TASK_FMT_DEBUG(pipe, REC_LOG,"���ڳ�ʼ��ʧ�� \n");
                    return -1;
                }
                pAmrThreadArg->TotalState.NormalAmr = RTS_SEND;
            }
            break;
        case RTS_SEND:
            {
                ret = taskmng_scheduled_task_send(pipe,runTask);
                if(ret == 1)
                {
                    TASK_FMT_DEBUG(pipe, REC_LOG,"ȫ��͸�����񷽰� %d ִ����� \n",runTask->taskid);
                    fujian_task_fail_addr_write(pipe,runTask);
                    pAmrThreadArg->TotalState.NormalAmr = RTS_IDLE;
                    return -1;
                }else if(ret == 0)
                {
                    pAmrThreadArg->TotalState.NormalAmr = RTS_RECV;
                }else if(ret == -2)
                {
                    pAmrThreadArg->TotalState.NormalAmr = RTS_485_SET;
                }else
                {
                    //����-1ʱ���л��շ�̬
                    return -1;
                }
            }
            break;
    
        case RTS_RECV:
            {
                ret = taskmng_scheduled_task_recv(pipe,runTask);
                if(ret<=0)
                {
                    pAmrThreadArg->TotalState.NormalAmr = RTS_SEND;

                }
            }
            break;
    
        default:
            pAmrThreadArg->TotalState.NormalAmr = RTS_IDLE;
            break;
    }
    
    return - 1;
    
}

/*******************************************************************************
* ��������: get_channel_delay_revise
* ��������: ���������ĸ�ʽ���ж��Ƿ���Ҫ�����ŵ�ʱ��
* �������: 
                resMess        ��������ı��ĸ�ʽ��
* �������: ��
* �� �� ֵ: 1                    ����
*           0                  ������
*******************************************************************************/

BOOL get_channel_delay_revise(uint8 reqMessType,OOP_OCTETVAR2K_T* resMess)
{
    uint8 framelen,tmpresType;
    uint16 delayrevise = 0,apdulen = 0;
    tmpresType = reqMessType;
    switch((tmpresType>>4)&0x0f)
    {
        case 0:
        break;
        case 1:
        {
            if(((tmpresType)&(0x0F))==1 || ((tmpresType)&(0x0F))==2)
            {
                framelen = resMess->value[0];
                if(resMess->nNum != framelen + 3)
                {
                    return FALSE;
                }
                else
                {
                    memcpy_r(&delayrevise,&resMess->value[1+framelen],2);
                }
            }
            else if(((tmpresType)&(0x0F))==4 || ((tmpresType)&(0x0F))==5)
            {
                framelen = resMess->value[1]; 
                if(resMess->nNum != framelen + 4)
                {
                    return FALSE;
                }
                else 
                {
                    memcpy_r(&delayrevise,&resMess->value[2+framelen],2);
                }
            }
        }break;
            /*����2 ��֧��3761 ���� 3 4û������ֵ*/
        case 5:
        {
            if(((tmpresType)&(0x0F))==2)
            {
                memcpy_r(&apdulen,&resMess->value[2],2);
                if(resMess->nNum != apdulen + 6)
                {
                    return FALSE;
                } 
                else
                {
                    memcpy_r(&delayrevise,&resMess->value[4+apdulen],2);
                }
            }
            else if(((tmpresType)&(0x0F))==3)
            {
                    OOP_TSA_T tsa;
                    tsa.af = resMess->value[1];
                  
                    memcpy_r(&apdulen,&resMess->value[tsa.len+3],2);
                    if(resMess->nNum != apdulen + 7 +  tsa.len)
                    {
                        return FALSE;
                    }
                    else
                    {
                        memcpy_r(&delayrevise,&resMess->value[5+apdulen+tsa.len],2);
                    }
            }
        }break;
        default:
        break;
    }
    
    return delayrevise & 0x4000;
}
void task_revise_mess_send(uint8 pipe,FUJIAN_TASK_RUN_T *runTask)
{
    FRAME_CONVERT_T *frame = NULL;
    uint8 index = 0;
    uint32 tmplen = frame->dstLen;
    OOP_OAD_U pipe_oad;
    uint16 SendLen=0;
    uint8 SendBuffer[MAX_MSG_LEN]={0};
    memset(SendBuffer,0x00,MAX_MSG_LEN);
    pipe_oad = taskmng_pipe_oad_get(pipe);
    memcpy(SendBuffer,&pipe_oad.value,SendLen);
    index+=4;
    if(tmplen<0x80)
    {
        SendBuffer[index++] = tmplen;
    }
    else
    {
        SendBuffer[index++] = 0x82;
        SendBuffer[index++] = (uint8)(tmplen>>8);
        SendBuffer[index++] = (uint8)tmplen;
    }
    memcpy(&SendBuffer[index],frame->dstBuf,tmplen);
    index+=tmplen;
    SendLen = index;
    taskmng_msg_data_pack(pipe,0x0003,0x0106,SendLen,SendBuffer,&runTask->sendMsgIndex);
}

void task_broad_finish_reply(uint8 pipe,FUJIAN_TASK_RUN_T *runTask)
{
    uint8 index = 0;
    OOP_OAD_U pipe_oad;
    uint16 SendLen=0;
    uint8 SendBuffer[MAX_MSG_LEN]={0};
    memset(SendBuffer,0x00,MAX_MSG_LEN);
    pipe_oad = taskmng_pipe_oad_get(pipe);
    memcpy(SendBuffer,&pipe_oad.value,SendLen);
    index+=4;
    SendLen = index;
    taskmng_msg_data_pack(pipe,0x0003,0x0107,SendLen,SendBuffer,&runTask->sendMsgIndex);;
}

/*��Ҫ�����ı����·�*/
void Broad_Timing_Send_abnomal(uint8 pipe,FUJIAN_TASK_RUN_T * runTask,FUJIAN_SPECIFIC_TASK_T  *currentSpecificTask)
{
    AmrThreadArg_T * pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];

    switch(pAmrThreadArg->BroadTask.state)
    {
        case BROAD_TASK_REVISE_MESS:
        {
            task_revise_mess_send(pipe,runTask);
            pAmrThreadArg->TotalState.SyncTimeState = INNER_RTS_RECV;
        }
        break;
        case BROAD_TASK_REPORT_FINISH:
        {
            task_broad_finish_reply(pipe,runTask);
//            /*�洢һ���ɹ���͸����¼*/
//            if(runTask->storageCnt>0)
//            {
//                taskmng_trans_task_result_save(pipe,runTask,currentSpecificTask,OOP_FJ_ERR_SUCCESS, NULL,0);
//            }
            runTask->specificTaskIndex++;   /*��ǰ����ָ��++*/
        }
        break;
        default:
        break;
    }

}


uint8 Broad_Timing_Send_nomal(uint8 pipe,FUJIAN_TASK_RUN_T * runTask,FUJIAN_SPECIFIC_TASK_T *currentSpecificTask)
{
    uint8 ret;
    FRAME_CONVERT_T frame;
    uint8 dstbuf[1000] = {0};
    uint8 index = 0;
    OOP_OAD_U pipe_oad;
    uint16 SendLen=0,RecvLen = 0;
    uint8 SendBuffer[MAX_MSG_LEN]={0};
    uint8 RecvBuffer[MAX_MSG_LEN]={0};
    OOP_TSA_T tsa;
    memset(SendBuffer,0x00,MAX_MSG_LEN);
    pipe_oad = taskmng_pipe_oad_get(pipe);
    memcpy_r(SendBuffer,&pipe_oad.value,4);
    index+=4;
    SendBuffer[index++] = runTask->transTask->commType;
    SendBuffer[index++] = runTask->transTask->taskTypeData.addi_param.broadcast/60;        //����    
    memset(&frame,0x00,sizeof(FRAME_CONVERT_T));
    ret = fujian_broadcast_tsa_get(currentSpecificTask->reqMessType,&tsa);
    if(ret<0)
    {
        TASK_FMT_TRACE( pipe, REC_LOG,"�㲥����֧�ֵ������ʽ�� %d ",currentSpecificTask->reqMessType);
        return -1;
    }
    frame.srcLen = currentSpecificTask->reqMess.nNum;
    frame.srcBuf = currentSpecificTask->reqMess.value;
    frame.pipedelay = 0;
    frame.dstBuf = dstbuf;
    Convert_Request_Frame(pipe,&frame,currentSpecificTask->reqMessType,&tsa);
    
    TASK_FMT_DEBUG(pipe, REC_LOG,"������ת��ǰ���� \n");
    TASK_BUF_DEBUG(pipe,REC_LOG,currentSpecificTask->reqMess.value,currentSpecificTask->reqMess.nNum);
    TASK_FMT_DEBUG(pipe, REC_LOG,"�����ĸ�ʽ�� 0x%02x ת������\n",currentSpecificTask->reqMessType);
    TASK_BUF_DEBUG(pipe,REC_LOG,frame.dstBuf,frame.dstLen);
    if(pipe>=PLC_NUM)
    {
        //���ڷ���
        TASK_FMT_DEBUG(pipe, REC_LOG,"485�㲥���ķ��� \n");
        fujian_rs485_send(pipe,3,frame.dstBuf,frame.dstLen);
        return 0;
    }
    if (frame.dstLen < 0x80)
    {
        SendBuffer[index++] = frame.dstLen;
    }
    else
    {
        SendBuffer[index++] = 0x82;
        SendBuffer[index++] = (uint8)(frame.dstLen >> 8);
        SendBuffer[index++] = (uint8)frame.dstLen;
    }
    memcpy(&SendBuffer[index],frame.dstBuf,frame.dstLen);
    index+=frame.dstLen;
    SendLen = index;
    ret = taskmng_msg_fun_deal(pipe,0x0003,0x0091, SendLen, SendBuffer, &RecvLen, RecvBuffer);
    if(ret > 0)
    {
         if(RecvLen==1&&RecvBuffer[0]==1)
         {
            return 1;
         }
    }
    return -1;
 
}

void Broad_Timing_Send(uint8 pipe,FRAME_CONVERT_T *frame,uint16 MsgIndex,uint16 IOP)
{
    uint8 payload[200]={0};
    uint16 index = 0;
    uint8 tmplen = frame->dstLen;
    OOP_OAD_U pipeoad = taskmng_pipe_oad_get( pipe);
    memcpy_r(&payload[index],(uint8*)&pipeoad.value,4);
    index+=4;
    if(tmplen<0x80)
    {
        payload[index++] = tmplen;
    }
    else
    {
        payload[index++] = 0x82;
        payload[index++] = (uint8)(tmplen>>8);
        payload[index++] = (uint8)tmplen;
    }
    memcpy(&payload[index],frame->dstBuf,tmplen);
    index+=tmplen;
    taskmng_msg_data_pack(pipe,0x0003,IOP,index,payload,&MsgIndex);
}

void Broad_Timing_Send_NoDelay(uint8 pipe,FRAME_CONVERT_T *frame,uint16 MsgIndex,uint16 IOP,uint8 commtype,uint16 runtime )
{
    uint8 payload[200]={0};
    uint16 index = 0;
    uint8 tmplen = frame->dstLen;
    OOP_OAD_U pipeoad = taskmng_pipe_oad_get( pipe);
    memcpy_r(&payload[index],(uint8*)&pipeoad.value,4);
    index+=4;
    payload[index++] = commtype;
    payload[index++] = runtime%60;
    if(tmplen<0x80)
    {
        payload[index++] = tmplen; 
    }
    else
    {
        payload[index++] = 0x82;
        payload[index++] = (uint8)(tmplen>>8);
        payload[index++] = (uint8)tmplen;
    }
    memcpy(&payload[index],frame->dstBuf,tmplen);
    index+=tmplen;
    taskmng_msg_data_pack(pipe,0x0003,IOP,index,payload,&MsgIndex);
}

uint8 get_broads_delay(uint8 pipe,uint8 commtype,uint8 runtime)
{
    uint16 IID = 0x0003;
    uint16 IOP; 
    uint8 index = 0;
    IOP = 0x0090;
    OOP_OAD_U pipe_oad;
    uint16 SendLen=0,RecvLen = 0;
    uint8 SendBuffer[MAX_MSG_LEN]={0};
    uint8 RecvBuffer[MAX_MSG_LEN]={0};
    memset(SendBuffer,0x00,MAX_MSG_LEN);
    pipe_oad = taskmng_pipe_oad_get(pipe);
    memcpy_r(SendBuffer,&pipe_oad.value,SendLen);
    index+=4;
    SendBuffer[index++] = commtype;
    SendBuffer[index++] = runtime/60;        //����
    SendLen = index;
    return taskmng_msg_fun_deal(pipe,IID,IOP, SendLen, SendBuffer, &RecvLen, RecvBuffer);
}

/*******************************************************************************
* ��������: taskmng_broadcast_delay_report_deal
* ��������: �յ��ϱ��㲥��ʱ���ĺ�Ĵ���
* �������: pipi ͨ����
            runTask ���񷽰�
            payload ��Ϣ����
            payloadLen ��Ϣ����
* �������: ��
* �� �� ֵ: 
            0          ��������
*           <0         ʧ��
*******************************************************************************/
int taskmng_broadcast_delay_report_deal(uint8 pipe,FUJIAN_TASK_RUN_T * runTask,MESSAGE_INFO_T* msgInfo)
{

    uint8 lenoffset;
    int ret;
    uint16 index;
    FUJIAN_SPECIFIC_TASK_T *specificTask = NULL;
    FRAME_CONVERT_T convert;
    uint8 dstbuf[2000]={0};
    uint16 delay = 0;
    OOP_TSA_T tsa;
    memcpy_r(&delay,&msgInfo->playload[4],2);
    TASK_FMT_TRACE( pipe, REC_LOG,"�ϱ��Ĺ㲥��ʱΪ%d (��λ10ms)\n",delay);

    //�����

    ret = taskmng_specific_task_get_from_index(runTask,runTask->specificTaskIndex,&specificTask);
    if(ret<0)
    {
        TASK_FMT_TRACE( pipe, REC_LOG,"δ�ҵ��������� specificTaskIndex %d ",runTask->specificTaskIndex);
        return -1;
    }
    ret = fujian_broadcast_tsa_get(specificTask->reqMessType,&tsa);
    if(ret<0)
    {
        TASK_FMT_TRACE( pipe, REC_LOG,"�㲥����֧�ֵ������ʽ�� %d ",specificTask->reqMessType);
            return -1;

    }
    convert.srcBuf = specificTask->reqMess.value;
    convert.srcLen = specificTask->reqMess.nNum;
    convert.pipedelay = delay;
    convert.dstBuf = dstbuf;
    Convert_Request_Frame(pipe,&convert,specificTask->reqMessType,&tsa);
    
    TASK_FMT_DEBUG(pipe, REC_LOG,"������ת��ǰ���� \n");
    TASK_BUF_DEBUG(pipe,REC_LOG,specificTask->reqMess.value,specificTask->reqMess.nNum);
    TASK_FMT_DEBUG(pipe, REC_LOG,"�����ĸ�ʽ�� 0x%02x ת������\n",specificTask->reqMessType);
    TASK_BUF_DEBUG(pipe,REC_LOG,convert.dstBuf,convert.dstLen);
    
    //����� �жϻ���û���¸�����������Ҫ�� ���û�� ���øñ�ɹ�
    uint8 sendbuf[2000]={0};
    index = 0;
    memcpy(&sendbuf[index],msgInfo->playload,4);
    index+=4;
    lenoffset = set_len_offset(convert.dstLen, &sendbuf[index]);
    index+=lenoffset;
    memcpy(&sendbuf[index],convert.dstBuf,convert.dstLen);
    index+=convert.dstLen;

    TASK_FMT_DEBUG(pipe, REC_LOG,"�ϱ��㲥��ʱ�ظ� \n");
    fujian_trans_task_buf_ack(pipe,0x0106,sendbuf,index,msgInfo->index,msgInfo->label);
    return 0;
}


/*******************************************************************************
* ��������: taskmng_broadcast_task_deal
* ��������: �㲥�����ִ���߼�
* �������: ��
* �������: ��
* �� �� ֵ: >0                  �ɹ�
*           <0                  ʧ��
*******************************************************************************/
int taskmng_broadcast_task_deal(uint8 pipe,FUJIAN_TASK_RUN_T * runTask)
{
    if(pipe>PLC_NUM+MAX_485_NUM)
    {
        TASK_FMT_DEBUG(pipe, RELAY_LOG, "�㲥����ֻ֧���ز���485�˿�\n");
        runTask->result = SCHEME_FINISHED;
        return -1;
    }
    /*pipe�����Ȳ���,���ز�*/
    AmrThreadArg_T * pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    int ret;
    uint16 RecvLen = 0;
    uint8 RecvBuffer[MAX_MSG_LEN]={0};
    OOP_OAD_U pipe_oad;
    memset(&pipe_oad,0x00,sizeof(OOP_OAD_U));
    MESSAGE_INFO_T* MsgInfo =NULL; 
    FUJIAN_SPECIFIC_TASK_T * currentSpecificTask = NULL;

    if(pAmrThreadArg->TotalState.NormalAmr == RTS_IDLE)
    {
        TASK_FMT_DEBUG(pipe, RELAY_LOG, "�㲥���� %d ��ʼִ��\n",runTask->taskid);
        pAmrThreadArg->TotalState.SyncTimeState = INNER_RTS_PIPE_INIT;
        pAmrThreadArg->TotalState.NormalAmr = RTS_SEND;
        memset(&pAmrThreadArg->BroadTask,0x00,sizeof(BROAD_TASK_T));
    }
    if(pAmrThreadArg->BroadTask.FailCnt > runTask->transTask->turnRetryNum)
    {
        TASK_FMT_DEBUG(pipe, RELAY_LOG, "�㲥���� %d ʧ�ܴ������� ���ֲ���ִ��\n",runTask->taskid);
        runTask->result = SCHEME_FAIL;
        return -1;
    }
    switch(pAmrThreadArg->TotalState.SyncTimeState)
    {
        case INNER_RTS_PIPE_INIT:
        {
            taskmng_pipe_collection_state_write(pipe,TASK_BROADCAST,runTask->taskid,runTask->priority);
            if(pipe < PLC_NUM)
            {
                if(!fujian_current_task_end(pipe))
                {
                    break;
                }
                pAmrThreadArg->TotalState.SyncTimeState = INNER_RTS_BROASTDELAY;
            }else
            {
                pAmrThreadArg->TotalState.SyncTimeState = INNER_RTS_485_INIT;
            }
            
            
        }break;
        case INNER_RTS_485_INIT:
        {
            ret = taskmng_specific_task_get_from_index(runTask,runTask->specificTaskIndex,&currentSpecificTask);
            if(ret<0)
            {
                TASK_FMT_DEBUG(pipe, REC_LOG,"�㲥����ִ����� \n");
                pAmrThreadArg->TotalState.SyncTimeState = INNER_RTS_PIPE_INIT;
                runTask->result = SCHEME_FINISHED;
                pAmrThreadArg->TotalState.NormalAmr = RTS_IDLE;
                time_t timenow = time(NULL);
                taskmng_trans_task_collection_state_update(pipe,runTask,TRANS_TASK_SUCC,&timenow);
                return -1;
            }
            if(fujian_rs485_init(pipe,&currentSpecificTask->uartParam)<0)
            {
                TASK_FMT_DEBUG(pipe, REC_LOG,"���ڳ�ʼ��ʧ�� \n");
                return -1;
            }
            pAmrThreadArg->TotalState.SyncTimeState = INNER_RTS_SEND;
        }
            break;
        case INNER_RTS_BROASTDELAY:
        {
            /*��ȡ��ǰ�ľ�������ָ��*/
            if(taskmng_specific_task_get_from_index(runTask,runTask->specificTaskIndex, &currentSpecificTask) == 0)
            {
                if(get_channel_delay_revise(currentSpecificTask->reqMessType,&currentSpecificTask->reqMess))
                {
                    TASK_FMT_DEBUG(pipe, REC_LOG,"��ȡ�㲥��ʱ �������� %d \n",currentSpecificTask->nodeid);
                    pAmrThreadArg->BroadTask.GET_DELAY = TRUE;
                    ret = get_broads_delay(pipe,runTask->transTask->commType,runTask->transTask->taskTypeData.addi_param.broadcast);
                    if(ret <0)
                    {
                        runTask->result = SCHEME_FAIL; 
                        TASK_FMT_DEBUG(pipe, REC_LOG,"��ȡ�㲥��ʱʧ�� \n");
                        break;
                    }
                    else if(ret >0)
                    {
                        pAmrThreadArg->BroadTask.sendTime = time(NULL);
                        pAmrThreadArg->TotalState.SyncTimeState = INNER_RTS_DELAY_WAIT;
                    }
                }
                else
                {
                    pAmrThreadArg->TotalState.SyncTimeState = INNER_RTS_SEND;
                    pAmrThreadArg->BroadTask.GET_DELAY = FALSE;
                }
            }
            else
            {
                 TASK_FMT_DEBUG(pipe, REC_LOG,"�㲥����ִ����� \n");
                 pAmrThreadArg->TotalState.SyncTimeState = INNER_RTS_PIPE_INIT;
                 runTask->result = SCHEME_FINISHED;
                 pAmrThreadArg->TotalState.NormalAmr = RTS_IDLE;
                 time_t timenow = time(NULL);
                 taskmng_trans_task_collection_state_update(pipe,runTask,TRANS_TASK_SUCC,&timenow);
                 return -1;
            }

            /*�㲥����ִ��ʱ������Ϊ�������㲥���ÿ֡���ʱ��*/
        }break;
        case INNER_RTS_DELAY_WAIT:
        {
            RecvLen = taskmng_plc_queue_read ( pAmrRecvDown[pipe], RecvBuffer );
            if(RecvLen>0)
            {
                MsgInfo = (MESSAGE_INFO_T*)RecvBuffer;
                TASK_FMT_DEBUG( pipe, REC_LOG, "�㲥�յ���Ϣ IID %04X IOP %04X payload \n",MsgInfo->IID,MsgInfo->IOP);
                TASK_BUF_DEBUG( pipe, REC_LOG,MsgInfo->playload,MsgInfo->msglen);
                if(MsgInfo->IOP == 0x0106)
                {
                    ret = taskmng_broadcast_delay_report_deal(pipe,runTask,MsgInfo);
                    if(ret<0)
                    {
                        pAmrThreadArg->BroadTask.FailCnt++;
                        pAmrThreadArg->TotalState.SyncTimeState = INNER_RTS_BROASTDELAY;
                    }else
                    {
                        pAmrThreadArg->BroadTask.sendTime = time(NULL);
                        pAmrThreadArg->TotalState.SyncTimeState = INNER_RTS_WAIT_NEXT;
                    }
                }
            }else
            {
                if(time(NULL) - pAmrThreadArg->BroadTask.sendTime > 90)
                {
                    TASK_FMT_DEBUG(pipe, REC_LOG,"��ʱδ�յ��ϱ��Ĺ㲥��ʱ \n");
                    pAmrThreadArg->BroadTask.FailCnt++;
                    pAmrThreadArg->TotalState.SyncTimeState = INNER_RTS_BROASTDELAY;
                }
            }
        }
            break;
        case INNER_RTS_WAIT_NEXT:
        {
            if(time(NULL) - pAmrThreadArg->BroadTask.sendTime > runTask->transTask->taskTypeData.addi_param.broadcast)
            {
                TASK_FMT_DEBUG(pipe, REC_LOG,"��ǰ��������ִ����� �㲥ʱ�� %d\n",runTask->transTask->taskTypeData.addi_param.broadcast);
                runTask->specificTaskIndex++;
                if(pipe<PLC_NUM)
                    pAmrThreadArg->TotalState.SyncTimeState = INNER_RTS_BROASTDELAY;
                else
                    pAmrThreadArg->TotalState.SyncTimeState = INNER_RTS_485_INIT;
            }
        }
            break;
        
        case INNER_RTS_SEND:
        {
            if(taskmng_specific_task_get_from_index(runTask,runTask->specificTaskIndex, &currentSpecificTask) < 0)
            {
                TASK_FMT_DEBUG(pipe, REC_LOG,"��ȡ��������ʧ�� specificTaskIndex %d \n",runTask->specificTaskIndex);
                pAmrThreadArg->TotalState.SyncTimeState = INNER_RTS_PIPE_INIT;
                runTask->result = SCHEME_FAIL;
                pAmrThreadArg->TotalState.NormalAmr = RTS_IDLE;
                return -1;
            }
            TASK_FMT_DEBUG(pipe, REC_LOG,"���͹㲥 ����ͨ����ʱ ��������� %d \n",currentSpecificTask->nodeid);
            Broad_Timing_Send_nomal(pipe,runTask,currentSpecificTask);
            /*���������ж���û����һ����������û��ֱ�����*/
            if(taskmng_specific_task_get_from_index(runTask,runTask->specificTaskIndex+1, &currentSpecificTask) < 0)
            {
                TASK_FMT_DEBUG(pipe, REC_LOG,"û���ҵ���һ���������񣬹㲥����ִ����ϣ����²ɼ�״̬�� \n");
                pAmrThreadArg->TotalState.SyncTimeState = INNER_RTS_PIPE_INIT;
                runTask->result = SCHEME_FINISHED;
                pAmrThreadArg->TotalState.NormalAmr = RTS_IDLE;
                time_t timenow = time(NULL);
                taskmng_trans_task_collection_state_update(pipe,runTask,TRANS_TASK_SUCC,&timenow);
                return -1;
            }
            pAmrThreadArg->BroadTask.sendTime = time(NULL);
            pAmrThreadArg->TotalState.SyncTimeState = INNER_RTS_WAIT_NEXT;
        }break;
//        case INNER_RTS_RECV:
//        {
//            RecvLen = taskmng_plc_queue_read ( pAmrRecvDown[pipe], RecvBuffer );
//            if(RecvLen>0)
//            {
//                MsgInfo = (MSG_INFO_T*)RecvBuffer;
//                TASK_FMT_DEBUG( pipe, RELAY_LOG, "�㲥�յ���Ϣ IID %04X IOP %04X MsgLen %d payload \n",MsgInfo->IID,MsgInfo->IOP,MsgInfo->MsgLen);
//                if(MsgInfo->MsgLen ==0)
//                {
//                    TASK_FMT_DEBUG(pipe, RELAY_LOG, "MsgInfo->MsgLen is %d\n",MsgInfo->MsgLen);
//                    break;
//                }
//                else if(MsgInfo->IOP == 0x0106)
//                {
//                    memcpy_r(&pAmrThreadArg->BroadTask.BroadDelay,&MsgInfo->MsgData[4],2);
//                    pAmrThreadArg->TotalState.SyncTimeState = INNER_RTS_SEND;
//                    pAmrThreadArg->BroadTask.state = BROAD_TASK_REVISE_MESS;
//                    pAmrThreadArg->BroadTask.sendTime = time(NULL);
//                }
//                else if(MsgInfo->IOP == 0x0107)
//                {
//                    pAmrThreadArg->TotalState.SyncTimeState = INNER_RTS_SEND;
//                    pAmrThreadArg->BroadTask.state = BROAD_TASK_REPORT_FINISH;
//                }
//                else
//                    pAmrThreadArg->TotalState.SyncTimeState = INNER_RTS_RETRY;
//            }
//            else
//            {
//                pAmrThreadArg->TotalState.SyncTimeState = INNER_RTS_RETRY;
//            }
//        }break;
//        case INNER_RTS_RETRY:
//        {
//            time_t tmpnum = runTask->transTask->taskTypeData.addi_param.broadcast;
//            if(time(NULL) - pAmrThreadArg->BroadTask.retrytime > tmpnum)
//            {
//                /*��һ����������*/
//                runTask->specificTaskIndex++;
//                pAmrThreadArg->TotalState.SyncTimeState = INNER_RTS_BROASTDELAY;
//                pAmrThreadArg->BroadTask.state = BROAD_TASK_GET_DELAY;
////                if(runTask->storageCnt >0)
////                {
////                    taskmng_trans_task_result_save(pipe, runTask, currentSpecificTask,OOP_FJ_ERR_RESQ_TIME_OUT, NULL,0);
////                }
//            }
//
//        }break;
        default:
            pAmrThreadArg->TotalState.NormalAmr = RTS_IDLE;
        break;
    }
    return -1;

}

/*******************************************************************************
* ��������: taskmng_metersearch_task_deal_485
* ��������: �ѱ������ִ���߼� 485
* �������: ��
* �������: ��
* �� �� ֵ: >0                  �ɹ�
*           <0                  ʧ��
*******************************************************************************/

//int taskmng_metersearch_task_deal_485(uint8 pipe, FUJIAN_TASK_RUN_T * runTask)
//{
//    return SearchRs485Meter(pipe);
//}



/*******************************************************************************
* ��������: taskmng_metersearch_save
* ��������: �洢�ѱ�����
* �������: pipi ͨ����
            runTask ���񷽰�
            payload ��Ϣ����
            payloadLen ��Ϣ����
* �������: ��
* �� �� ֵ: 

*******************************************************************************/
int taskmng_metersearch_save(uint8 pipe,FUJIAN_TASK_RUN_T * runTask,MESSAGE_INFO_T *MsgInfo)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    uint16 index = 0;
    uint16 num,i;
    uint8 lenoffset;
    uint8 addr[6];
    int ret;
    OOP_SEARCHMET_RES_T  searchResult;
    NOMAL_OAD_T NormalOad;
    NormalOad.logicId = 0;
    NormalOad.oad.value = 0x60020200;
    NormalOad.classtag = 3;
    memset(&searchResult,0x00,sizeof(OOP_SEARCHMET_RES_T));
    
    index+=4;
    num = get_len_offset(&MsgInfo->playload[index], &lenoffset);
    index+=lenoffset;
    TASK_FMT_TRACE( pipe, REC_LOG, "�ϱ��ӽڵ���Ϣ ���� %d\n",num);
    DT_Time2DateTimeShort(time(NULL),&searchResult.serchTime);
    
    for(i=0;i<num;i++)
    {
        memcpy(addr,&MsgInfo->playload[index],6);
        index+=6;
        if(taskmng_addr_isvalid_check(addr,6)<0)
        {
            TASK_FMT_DEBUG( pipe, REC_LOG, "�ѱ��ϱ���ַ�Ƿ� 0x%02x%02x%02x%02x%02x%02x \n",addr[0],addr[1],addr[2],addr[3],addr[4],addr[5]);
            continue;
        }
        searchResult.commAdd.len = 5;
        memcpy(searchResult.commAdd.add,addr,6);
        if(fujian_metersearch_result_check(pipe,searchResult,&NormalOad.infoNum)==FALSE)
        {
            TASK_FMT_DEBUG( pipe, REC_LOG, "�ѱ��ַ�ظ�ֱ�ӹ��� 0x%02x%02x%02x%02x%02x%02x \n",addr[0],addr[1],addr[2],addr[3],addr[4],addr[5]);
            continue;
        }
        ret = db_write_nomal(pAmrThreadArg->DBClientHandle,&NormalOad,(uint8*)&searchResult,sizeof(OOP_SEARCHMET_RES_T));
        if(ret==0)
        {
            TASK_FMT_DEBUG( pipe, REC_LOG, "�ѱ��ַд��ɹ�infonum %d 0x%02x%02x%02x%02x%02x%02x \n",NormalOad.infoNum,addr[0],addr[1],addr[2],addr[3],addr[4],addr[5]);
        }
    }
    //�ٻظ�ȷ��
    uint8 sendbuf[10]={0};
    index = 0;
    sendbuf[index++] = 1;
    fujian_trans_task_buf_ack(pipe,0x0102,sendbuf,index,MsgInfo->index,MsgInfo->label);
    return  0;
}

/*******************************************************************************
* ��������: taskmng_metersearch_recv
* ��������: �ѱ� ���մ��� ���ֽ���ʱ��֮ǰ����
* �������: pipi ͨ����
            runTask ���񷽰�

* �������: ��
* �� �� ֵ: >0                  �����ڽ���̬
            0          �����л�������
*           <0               ���ճ�ʱ
*******************************************************************************/

int taskmng_metersearch_recv(uint8 pipe,FUJIAN_TASK_RUN_T * runTask)
{

    uint8 recvbuf[2000]={0};
    uint16 recvlen;
    MESSAGE_INFO_T      *MsgInfo = NULL;


    recvlen = taskmng_plc_queue_read(pAmrRecvDown_Cb[pipe], recvbuf);
    if(recvlen>0)
    {
        MsgInfo = (MESSAGE_INFO_T*)recvbuf;
        TASK_FMT_TRACE( pipe, REC_LOG, "�ѱ� recv msgindex %d IOP %04X payload \n",MsgInfo->index,MsgInfo->IOP);
        TASK_BUF_TRACE( pipe, REC_LOG,MsgInfo->playload, MsgInfo->msglen);

        //�յ��ϱ�����
        if(MsgInfo->IOP == 0x0102)
        {
            taskmng_metersearch_save(pipe,runTask,MsgInfo);
        }
    }
    else
    {
        //�ж��Ƿ��������ʱ��
        time_t endTime;
        DT_DateTimeShort2Time(&runTask->thisTurnEndTime,&endTime);
        if(endTime - time(NULL) < 5 )
        {
            return 0;
        }
    }
    return 1;
}


/*******************************************************************************
* ��������: taskmng_metersearch_task_deal
* ��������: �ѱ������ִ���߼�
* �������: ��
* �������: ��
* �� �� ֵ: >0                  �ɹ�
*           <0                  ʧ��
*******************************************************************************/

int taskmng_metersearch_task_deal(uint8 pipe, FUJIAN_TASK_RUN_T * runTask)
{
    if(pipe>=PLC_NUM && pipe<PLC_NUM+MAX_485_NUM)
    {
        return taskmng_metersearch_task_deal_485(pipe,runTask);
    }

    AmrThreadArg_T * pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    uint16 allowTime;
    int ret;
    time_t endtime;
    switch(pAmrThreadArg->TotalState.NormalAmr)
    {
        case RTS_IDLE:
            {
                TASK_FMT_TRACE( pipe, REC_LOG, "��ʼִ���ѱ�����\n");
                taskmng_pipe_collection_state_write(pipe,TASK_METERSEARCH,runTask->taskid,runTask->priority);
                //�Ƚ�����ǰ����
                fujian_current_task_end(pipe);
                pAmrThreadArg->TotalState.NormalAmr = RTS_SEND;
            }
            break;

        case RTS_SEND:
            {
                //�����ѱ�
                DT_DateTimeShort2Time(&runTask->thisTurnEndTime,&endtime);
                allowTime = (endtime-time(NULL))/60;
                fujian_meter_search_start(pipe,allowTime);
                pAmrThreadArg->TotalState.NormalAmr = RTS_RECV;
                TASK_FMT_TRACE( pipe, REC_LOG, "�ȴ��ϱ��ѱ���\n");
            }
            break;

        case RTS_RECV:
            {
                ret = taskmng_metersearch_recv(pipe,runTask);
                if(ret<=0)
                {
                    TASK_FMT_TRACE( pipe, REC_LOG, "�ѱ�������� \n");
                    fujian_current_task_end(pipe);
                    runTask->result = SCHEME_FINISHED;
                    pAmrThreadArg->TotalState.NormalAmr = RTS_IDLE;
                }
            }
            break;

        default:
            pAmrThreadArg->TotalState.NormalAmr = RTS_IDLE;
            break;
    }

    return - 1;

}

/*******************************************************************************
* ��������: taskmng_idle_task_deal
* ��������: ���������ִ���߼�
* �������: ��
* �������: ��
* �� �� ֵ: >0                  �ɹ�
*           <0                  ʧ��
*******************************************************************************/
int taskmng_idle_task_deal(uint8 pipe,FUJIAN_TASK_RUN_T * runTask)
{
    time_t timenow = time(NULL);
    taskmng_pipe_collection_state_write(pipe,TASK_IDLE,runTask->taskid,runTask->priority);
    //��ʱ�������ִ��ʱ��
    taskmng_trans_task_collection_state_update(pipe,runTask,TRANS_TASK_EXEC,(void*)&timenow);
    sleep(1);
    return -1;

}

/*******************************************************************************
* ��������: taskmng_commlog_task_deal
* ��������: ͨ����־�����ִ���߼� ��������ʱ�� ��/mnt/data/app/recordĿ¼�������е���־�ļ����
* �������: ��
* �������: ��
* �� �� ֵ: >0                  �ɹ�
*           <0                  ʧ��
*******************************************************************************/
int taskmng_commlog_task_deal(uint8 pipe,FUJIAN_TASK_RUN_T * runTask)
{
    taskmng_trans_task_record_file_load(pipe,runTask);    
    if(runTask->result == SCHEME_FINISHED)
    {
        TASK_FMT_DEBUG(pipe, REC_LOG, "ͨ����־�����������\n");
        return -1;
    }
    //��ȡ���ֿ�ʼʱ�� ��Ϊɸѡ��־�ļ��Ŀ�ʼʱ��
    taskmng_pipe_collection_state_write(pipe,runTask->transTask->taskTypeData.taskType,runTask->taskid,runTask->priority);
    time_t tmp;
    char saveDir[1000]={0};
    char dirName[100]={0};
    DIR           *dirp;
    struct dirent *entp;
    char cmd[1000]={0};
    char *str1,*str2;
    char  datestr[10]={0};
    int year,month,day,hour;
    OOP_DATETIME_S_T fileDate;
    time_t fileTime;
    memset(&fileDate,0x00,sizeof(OOP_DATETIME_S_T));
    DT_DateTimeShort2Time(&runTask->storageTime, &tmp);
    //�ô洢ʱ���һ����������Ϊɸѡ�Ŀ�ʼʱ��
    DT_TimeSubInv(&tmp,runTask->transTask->cycle);
    sprintf(dirName,"commlog_%02d%02d%02d%02d",runTask->storageTime.year -2000,runTask->storageTime.month,
            runTask->storageTime.day,runTask->storageTime.hour);
    sprintf(saveDir,"%s/%s",gRecordPath,dirName);
    if(!path_exist(saveDir))
    {
        path_dir_make(saveDir);
    }
    if ((dirp = opendir(gRecordPath)) == NULL)
    {
        TASK_FMT_DEBUG(pipe, REC_LOG, "��Ŀ¼%s ʧ��\n",gRecordPath);
        return -1;
    }
    while ((entp = readdir(dirp)) != NULL)
    {
        if((strcmp(entp->d_name, ".") == 0) || (strcmp(entp->d_name, "..") == 0))
        {
            continue;
        }
        str1 = strstr(entp->d_name,".log.");
        str2 = strstr(entp->d_name,".record");
        //����־�ļ������� ctsrec00.log.22033118.record
        if(str1!=NULL && str2!=NULL && (str2-str1 == 13))
        {
            memset(datestr,0x00,sizeof(datestr));
            memcpy(datestr,str1+5,8);
            sscanf(datestr,"%02d%02d%02d%02d",&year,&month,&day,&hour);
            fileDate.year = year+2000;
            fileDate.month = month;
            fileDate.day = day;
            fileDate.hour = hour;
            if(DT_DateTimeShort2Time(&fileDate,&fileTime))
            {
                if(fileTime>=tmp && fileTime< time(NULL))
                {
                    memset(cmd,0x00,sizeof(cmd));
                    sprintf(cmd,"mv -f %s/%s %s",gRecordPath,entp->d_name,saveDir);
                    TASK_FMT_DEBUG(pipe, REC_LOG, "�����ļ� ����Ϊ %s \n",cmd);
                    system(cmd);
                }
            }
        }
    }
    //�ٰ��ļ��д�һ��ѹ����
    memset(cmd,0x00,sizeof(cmd));
    sprintf(cmd,"cd %s ;tar -czvf %s.tar.gz %s",gRecordPath,dirName,dirName);
    TASK_FMT_DEBUG(pipe, REC_LOG, "��������ļ� ����Ϊ %s \n",cmd);
    system(cmd);
    //������ɾ��
    memset(cmd,0x00,sizeof(cmd));
    sprintf(cmd,"rm -rf  %s",saveDir);
    TASK_FMT_DEBUG(pipe, REC_LOG, "ɾ������Ϊ %s \n",cmd);
    system(cmd);
    char logfile[200]={0};
    sprintf(logfile,"%s/%s.tar.gz",gRecordPath,dirName);
    fujian_task_log_event_report( pipe,logfile,runTask->taskid,&runTask->storageTime);
    runTask->result = SCHEME_FINISHED;
    taskmng_trans_task_record_file_write( pipe, runTask);
    return -1;

}

/*******************************************************************************
* ��������: taskmng_trigger_task_deal
* ��������: �������������ִ���߼�
* �������: ��
* �������: ��
* �� �� ֵ: >0                  �ɹ�
*           <0                  ʧ��
*******************************************************************************/
int taskmng_trigger_task_deal(uint8 pipe,FUJIAN_TASK_RUN_T * runTask)
{
    //���񴥷���Ͷ�ʱ����һ���߼�
    return taskmng_scheduled_task_deal(pipe,runTask);
   // return -1;

}

/*******************************************************************************
* ��������: taskmng_task_trigger
* ��������: ���������������� �ҵ��������������ø�����ʱ�伴��
* �������: ���������񷽰���
* �������: ��
* �� �� ֵ: >0                  �ɹ�
*           <0                  ʧ��
*******************************************************************************/

int taskmng_task_trigger(uint8 pipe,uint16 taskId,OOP_TSA_T *tsa)
{
    uint16 i,j;
    uint8 taskpipe = 0xff;
    //�������񷽰����ҵ�����
    pthread_mutex_lock(&gTransTaskCfg.Lock);
    for(i=0;i<gTransTaskCfg.TaskNum;i++)
    {
        if(taskId == gTransTaskCfg.TaskCfg[i].taskid && gTransTaskCfg.TaskCfg[i].taskTypeData.taskType == 5)
        {
            taskpipe = taskmng_mtr_pipe_get(&gTransTaskCfg.TaskCfg[i].oad);
            break;
        }
    }
    pthread_mutex_unlock(&gTransTaskCfg.Lock);
    if(taskpipe == 0xff)
    {
        TASK_FMT_DEBUG(pipe, REC_LOG, "δ�ҵ����������񷽰��� %d\n",taskId);
        return 0;
    }
    AmrThreadArg_T * pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[taskpipe];
    FUJIAN_TASK_RUN_T * runTask = NULL;
    for(i=0;i<pAmrThreadArg->taskRunList.taskNum;i++)
    {
        if(pAmrThreadArg->taskRunList.taskList[i].taskid == taskId)
        {
            runTask = &pAmrThreadArg->taskRunList.taskList[i];
            DT_Time2DateTimeShort(time(NULL),&runTask->triggerTime);
            TASK_FMT_DEBUG(taskpipe, REC_LOG, "���񷽰��� %d ������\n",taskId);
            //����Ǵ�����ִ�еı�������ͬ��ַ����ľ������� ��������ַ�ø����
            if(runTask->transTask->taskTypeData.addi_param.condition.object ==1 && tsa!=NULL)
            {
                for(j=0;j<runTask->meterNum;j++)
                {
                    if(memcmp(tsa,&runTask->meterInfo[j].meterAddr,sizeof(OOP_TSA_T)) == 0)
                    {
                        runTask->meterInfo[j].triggerFlag = 1;
                        TASK_FMT_DEBUG(taskpipe, REC_LOG, "��ַΪ %02x%02x%02x%02x%02x%02x\n",tsa->add[0],tsa->add[1],tsa->add[2],
                            tsa->add[3],tsa->add[4],tsa->add[5]);
                        break;
                    }
                }

            }

            break;
        }
    }
    return 0;
}

/*******************************************************************************
* ��������: taskmng_trans_task_deal
* ��������: ȫ��͸������ִ�� ��������һֱ���� ������ 
* �������: ��
* �������: ��
* �� �� ֵ: >0                  �ɹ�
*           <0                  ʧ��
*******************************************************************************/
int taskmng_trans_task_deal(uint8 pipe)
{
    AmrThreadArg_T * pAmrThreadArg;

    pAmrThreadArg = &gAmrThreadArg[pipe];
    int             ret;
    static uint8 flag[AMR_THREAD_NUM] = {0};
    FUJIAN_TASK_RUN_T * runTask = NULL;
    //����û��ʼ����ʱ��ִ��
    if(pAmrThreadArg->TaskModifyFlag != TASK_CFG_INITED)
        return -1;
    //�ҵ���ǰ����ִ�е�����
    ret = taskmng_trans_task_run_check(pipe, &runTask);

    if(ret < 0)
    {
        pAmrThreadArg->TotalState.NormalAmr = RTS_IDLE;
        taskmng_pipe_collection_state_write(pipe,TASK_NULL,0,0);
        return - 1;
    }

    if(runTask->changeFlag != 1)
    {
        TASK_FMT_DEBUG(pipe, REC_LOG, "���� %d ���޸� ������\n", runTask->taskid);
        return - 1;
    }

    if(runTask->taskid != pAmrThreadArg->currentTaskId)
    {
        flag[pipe]=0;
        pAmrThreadArg->TotalState.NormalAmr = RTS_IDLE;
        pAmrThreadArg->currentTaskId = runTask->taskid;
        TASK_FMT_DEBUG(pipe, REC_LOG, "�����л� ��ǰ�������� %d \n", runTask->taskid);
    }
    if(runTask->isNewTurn == 1)
    {
        flag[pipe]=0;
        runTask->isNewTurn = 0;
        taskmng_trans_task_collection_state_update(pipe,runTask,TRANS_TASK_INIT,NULL);
        runTask->result = SCHEME_DOING;
        pAmrThreadArg->TotalState.NormalAmr = RTS_IDLE;
        TASK_FMT_DEBUG(pipe, REC_LOG, "��ǰ�������� %d ����һ�� retryCycle %d �����ռ�� %d ��֡���� %d\n", runTask->taskid,runTask->retryCycle,
            runTask->transTask->bmonopoly,runTask->transTask->bRelation);
        if(runTask->retryCycle == 0)
        {
            TASK_FMT_DEBUG(pipe, REC_LOG, "д����ִ�п�ʼʱ��\n");
            taskmng_trans_task_collection_state_update(pipe,runTask,TRANS_TASK_START,NULL);
        }
        if(runTask->retryCycle > runTask->transTask->turnNum)
        {
            TASK_FMT_DEBUG(pipe, REC_LOG, "���� %d ʧ���ִ� %d ���� %d ����������\n", runTask->taskid,runTask->retryCycle,runTask->transTask->turnNum);
            runTask->result = SCHEME_FINISHED;
            return -1;
        }
    }
    //����Ƕ�ռ�����������Ѿ�ִ����� �Ͳ�Ҫ��������
    //�����ռ��������ִ�� ��ʼ����ʱ�����������ÿ� ���ܵ��µ��������ڵ�ʱ������
    if(runTask->transTask->bmonopoly == 1 && (runTask->result ==SCHEME_FINISHED || runTask->result == SCHEME_FAIL))
    {
        if(flag[pipe]==0)
        {
            TASK_FMT_DEBUG(pipe, REC_LOG, "���� %d ��ռ���� ������ִ�����\n", runTask->taskid);
            flag[pipe]=1;
        }
        return -1;
    }

    switch(runTask->transTask->taskTypeData.taskType)
    {
        case 0: //��ʱ����
            {
                return taskmng_scheduled_task_deal(pipe,runTask);
            }
            break;

        case 1: //�㲥����
            {
                return taskmng_broadcast_task_deal(pipe,runTask);
            }
            break;

        case 2: //�ѱ�
            {
                return taskmng_metersearch_task_deal(pipe,runTask);

            }
            break;

        case 3: //����
            {
                return taskmng_idle_task_deal(pipe,runTask);
            }
            break;

        case 4: //ͨ����־
            {
               return taskmng_commlog_task_deal(pipe,runTask);
            }
            break;

        case 5: //��������
            {
                return taskmng_trigger_task_deal(pipe,runTask);
            }
            break;

        default:
            TASK_FMT_DEBUG(pipe, REC_LOG, "����%d ��֧�ֵ�������� %d \n", runTask->taskid,
                 runTask->transTask->taskTypeData.taskType);
            break;
    }

    return - 1;
}

/*******************************************************************************
* ��������: taskmng_trans_task_result_save
* ��������: ȫ��͸������������洢
* �������:
* �������: ��
* �� �� ֵ: 0                  �ɹ�
*           <0                  ʧ��
*******************************************************************************/

int taskmng_trans_task_result_save(uint8 pipe,uint8 isreport,OOP_TSA_T *tsa,time_t exectime,FUJIAN_TASK_RUN_T * runTask,FUJIAN_SPECIFIC_TASK_T *currentSpecificTask,uint8 errorCode,uint8 *databuf,uint16 datalen)
{
    uint8 result = 0;
    OOP_TRANS_TASK_RESULT_T transResult;
    memset(&transResult,0x00,sizeof(OOP_TRANS_TASK_RESULT_T));
    time_t timenow = time(NULL);
    int ret;
    uint32 storeNo=0;
    transResult.transTaskid = runTask->taskid;
    transResult.nodeid = currentSpecificTask->nodeid;
    memcpy(&transResult.groupAdd,tsa,sizeof(OOP_TSA_T));
    transResult.commType = runTask->transTask->commType;
    transResult.storeTime = runTask->storageTime;
    //�ն˳���ʱ��ֱ��ȡ��ǰʱ��
    DT_Time2DateTimeShort(exectime,&transResult.readTime);
    transResult.storeNO = runTask->storageCnt;
    
    //�˴������ִ�Ӧֵ������� ����δִ�е���0 ������1��ʼ
    if(errorCode == OOP_FJ_ERR_TASK_NO_EXECUTE)
        transResult.retryTurn = 0;
    else
        transResult.retryTurn = runTask->retryCycle + 1;
    if(errorCode >= ERROR_SUCCESS_CODE_ADD)
    {
        result = errorCode-ERROR_SUCCESS_CODE_ADD;
        transResult.result = 1;
        transResult.transMsg.errCode = 0;
    }else
    {
        transResult.result = (errorCode==0?1:0);
        transResult.transMsg.errCode = errorCode;
    }
    
    transResult.resultMessType = currentSpecificTask->resMessType;


    if(datalen != 0)
    {
        if(datalen > currentSpecificTask->resMaxLen)
        {
            TASK_FMT_TRACE(pipe, REC_LOG, "���ĳ�������� %d \n",currentSpecificTask->resMaxLen);
            datalen = currentSpecificTask->resMaxLen;
            transResult.transMsg.errCode = OOP_FJ_ERR_MESS_CHECK;
        }
        transResult.transMsg.resultData.nNum = datalen;
        memcpy(transResult.transMsg.resultData.value, databuf, datalen);
    }

    //�洢���Ϊ0ʱ���ô洢 ���ǻ���Ҫ�ϱ�
    if(runTask->transTask->storeNum != 0)
    {
        WRITE_RECORD_T  inData;
    
        memset(&inData, 0x00, sizeof(WRITE_RECORD_T));
        inData.recordOAD.infoNum = runTask->taskid;
        inData.recordOAD.classtag = 5;
        inData.recordOAD.road.oadMain.value = 0x6E150200;
        inData.recordOAD.road.oadCols.nNum = 1;
        inData.recordOAD.road.oadCols.oad[0].value = currentSpecificTask->nodeid;
        inData.MAC.nNum = 6;
        memcpy(inData.MAC.value, transResult.groupAdd.add, 6);
        DT_DateTimeShort2Time(&runTask->thisTurnBeginTime, (time_t *) &inData.colStartTm);
        DT_DateTimeShort2Time(&runTask->storageTime, (time_t *) &inData.colStoreTm);
        inData.colEndTm = timenow;
        inData.inDatalen = sizeof(OOP_TRANS_TASK_RESULT_T);
        memcpy(inData.inDataBuf, &transResult, sizeof(OOP_TRANS_TASK_RESULT_T));
    
    #if IS_NEED_ADD_8HOUR
    
        //����ʱ�궼Ҫ����8Сʱ
        inData.colStartTm += (8 * 3600);
        inData.colStoreTm += (8 * 3600);
        inData.colEndTm += (8 * 3600);
    #endif
    
        //�Ȳ�ѯ������������û����ʷ���� ����������
        RECORD_UNIT_T   RecordOut;
    
        memset(&RecordOut, 0x00, sizeof(RECORD_UNIT_T));
        READ_RECORD_T   ReadRecord;
    
        memset(&ReadRecord, 0x00, sizeof(READ_RECORD_T));
        memcpy(&ReadRecord.recordOAD, &inData.recordOAD, sizeof(RECORD_OAD_T));
        ReadRecord.cType = COL_TM_STORE;
        ReadRecord.cStart = inData.colStoreTm;
        ReadRecord.cEnd = inData.colStoreTm;
        ReadRecord.sortType = 0;
        memcpy(&ReadRecord.MAC, &inData.MAC, sizeof(OOP_OCTETVAR64_T));
        ReadRecord.bTable = 1;
        memcpy(&ReadRecord.table, &inData.MAC, sizeof(OOP_OCTETVAR64_T));
        ret = db_read_record(gAmrThreadArg[pipe].DBClientHandle, &ReadRecord, DATA_LEN_MAX, &RecordOut);
    
        if(ret == 0)
        {
            TASK_FMT_TRACE(pipe, REC_LOG, "��Ҫ��������\n");
            UPDATE_RECORD_T update;
    
            memset(&update, 0x00, sizeof(UPDATE_RECORD_T));
            memcpy(&update.recordOAD, &inData.recordOAD, sizeof(RECORD_OAD_T));
            update.filter.choice = 1;
            update.filter.storeTm = inData.colStoreTm;
            memcpy(&update.MAC, &inData.MAC, sizeof(OOP_OCTETVAR64_T));
            update.inDatalen = sizeof(OOP_TRANS_TASK_RESULT_T);
            memcpy(&update.inDataBuf, &transResult, sizeof(OOP_TRANS_TASK_RESULT_T));
            ret = db_update_record(gAmrThreadArg[pipe].DBClientHandle, &update);
    
            if(ret != 0)
            {
                TASK_FMT_TRACE(pipe, REC_LOG, "͸�����������ʧ�� ret is %d\n", ret);
            }
        }
        else
        {
    
            ret = db_write_record(gAmrThreadArg[pipe].DBClientHandle, &inData, &storeNo);
    
            if(ret != 0)
            {
                TASK_FMT_TRACE(pipe, REC_LOG, "͸��������洢ʧ�� ret is %d\n", ret);
            }
    
        }
    }
    
    
    //�ϱ�͸���������¼� ��ʱû�����Ĳ��ϱ�
    //���ֻ�ϱ��ɹ��� ������������ϱ����һ��
    uint16 i;
    TASK_FMT_TRACE(pipe, REC_LOG, "�ϱ���� �洢��� %d\n",transResult.storeNO);
    if(runTask->transTask->storeNum ==0)
        transResult.storeNO = 1;
    if( isreport == 1 || transResult.result == 1)
    {
        fujian_task_data_event_report(pipe, &transResult,result);
        for(i=0;i<currentSpecificTask->addrAll.num;i++)
        {
            if(memcmp(tsa,&currentSpecificTask->addrAll.addr[i],sizeof(OOP_TSA_T)) == 0)
            {
                currentSpecificTask->addrAll.isReport[i] = 1;
                break;
            }
        }
    }
    
    return ret;
}

/*******************************************************************************
* ��������: taskmng_trigger_task_poweron_check
* ��������: ����ն��ϵ�Ĵ�������
* �������: ��
* �������: ��
* �� �� ֵ: 
*******************************************************************************/
void taskmng_trigger_task_poweron_check(uint8 pipe)
{
    uint16 i;
    AmrThreadArg_T * pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    FUJIAN_TASK_RUN_T *runTask;
    for(i  = 0;i<pAmrThreadArg->taskRunList.taskNum;i++)
    {
        if(pAmrThreadArg->taskRunList.taskList[i].transTask->taskTypeData.taskType == 5 && 
                pAmrThreadArg->taskRunList.taskList[i].transTask->taskTypeData.addi_param.condition.type == 0)
        {
            runTask = &pAmrThreadArg->taskRunList.taskList[i];
            DT_Time2DateTimeShort(time(NULL),&runTask->triggerTime);
            TASK_FMT_DEBUG(pipe, REC_LOG, "���񷽰��� %d �ն��ϵ类����\n",runTask->taskid);
        }

    }
}
/*******************************************************************************
* ��������: taskmng_trigger_task_yxchange_check 
* ��������:�յ�ң�ű�λ�����Ƿ񴥷�����
* �������:buf ��Ϣ����
        len ��Ϣ����

* �������: 
* �� �� ֵ: 
*******************************************************************************/
void taskmng_trigger_task_yxchange_check(uint8* buf,uint16 len)
{
    uint16 index = 0;
    uint16 i,j;
    uint8  yxchange=0;
    uint8  taskYxState;
    uint8 taskpipe=0xff;
    AmrThreadArg_T * pAmrThreadArg;
    FUJIAN_TASK_RUN_T * runTask = NULL;
    index++;    //�豸��
    index+=2;   //ң��״̬ һ���ֽڳ��ȼ�һ���ֽ�״̬ ������
    index++;
    yxchange = buf[index++];    //��ȡ��ң�ű�λ״̬ ����Ĳ�����
        
    //���������Ҷ�Ӧ����
    pthread_mutex_lock(&gTransTaskCfg.Lock);
    for(i=0;i<gTransTaskCfg.TaskNum;i++)
    {
        if(gTransTaskCfg.TaskCfg[i].taskTypeData.taskType == 5 && gTransTaskCfg.TaskCfg[i].taskTypeData.addi_param.condition.type == 1)
        {
            taskYxState = gTransTaskCfg.TaskCfg[i].taskTypeData.addi_param.condition.data.yx.nValue;
            membitrev(&taskYxState,8);
            //ֻҪ�������ң�ű�λ��Чλ��ң�ű�λ��Ϣĳλȫ��1 �򴥷�����
            if((yxchange & taskYxState) >0)
            {
                taskpipe = taskmng_mtr_pipe_get(&gTransTaskCfg.TaskCfg[i].oad);
                if(taskpipe == 0xff)
                {
                    TASK_FMT_DEBUG(-1, TASK_LOG, "ң�ű�λ���������񷽰��� %d û��ͨ��\n",gTransTaskCfg.TaskCfg[i].taskid);
                    continue;
                }
                TASK_FMT_DEBUG(-1, TASK_LOG, "ң�ű�λ���������񷽰��� %d ͨ���� %d \n",gTransTaskCfg.TaskCfg[i].taskid,taskpipe);
                pAmrThreadArg = &gAmrThreadArg[taskpipe];
                for(j=0;j<pAmrThreadArg->taskRunList.taskNum;j++)
                {
                    if(pAmrThreadArg->taskRunList.taskList[j].taskid == gTransTaskCfg.TaskCfg[i].taskid)
                    {
                        runTask = &pAmrThreadArg->taskRunList.taskList[j];
                        DT_Time2DateTimeShort(time(NULL),&runTask->triggerTime);
                        TASK_FMT_DEBUG(taskpipe, REC_LOG, "���񷽰��� %d ��ң�ű�λ����\n",runTask->taskid);
                        break;
                    }

                }
            }
        }
    }
    pthread_mutex_unlock(&gTransTaskCfg.Lock);
}

/*******************************************************************************
* ��������: fujian_meter_event_report_check 
* ��������://�յ�����¼��ϱ����ж��Ƿ��ܴ���
* �������:buf ��Ϣ����
        len ��Ϣ����

* �������: 
* �� �� ֵ: 
*******************************************************************************/

BOOL fujian_meter_event_report_check(uint8 *buf,uint16 len,OOP_TRIGGER_T *trigger)
{
    if(trigger->type!=4)
        return FALSE;
    uint16            start;              /**< ��ȡ��ʼ       */
    uint16            end;                /**< ��ȡ����       */
    OOP_OCTETVAR256_T features;           /**< �����ֽ�       */
    start = trigger->data.features.start;
    end = trigger->data.features.end;
    memcpy(&features,&trigger->data.features.features,sizeof(OOP_OCTETVAR256_T));
    //�������ֽ�ƥ��
    if(end>len || end<=start)
    {
        TASK_FMT_DEBUG( 0, RELAY_LOG, "����¼��ϱ�������ƥ��������� start %d end %d len %d \n",start,end,len);
        return FALSE;
    }
    //start���ܴ�1��ʼ
    if(memcmp(features.value,buf+start-1,features.nNum) == 0)
    {
        TASK_FMT_DEBUG( 0, RELAY_LOG, "����¼��ϱ�������ƥ������ɹ� �����ֽ�����\n");
        TASK_BUF_DEBUG( 0, RELAY_LOG,features.value,features.nNum);
        return TRUE;
    }
    return FALSE;
}

/*******************************************************************************
* ��������: fujian_event_report_mqtt 
* ��������://mqtt�߳����յ��ϱ��¼���Ĵ���
* �������:buf ��Ϣ����
        len ��Ϣ����

* �������: 
* �� �� ֵ: 
*******************************************************************************/
void fujian_event_report_mqtt(uint8* buf,uint16 len)
{
    AmrThreadArg_T * pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[0];
    uint16 i,j;
    uint16 index=0;
    uint8 DataBuf[2000]={0};
    uint16 DataLen;

    uint8 lenoffset;
    uint8 addr[6];
    FUJIAN_TASK_RUN_T * runTask = NULL;
    index+=4;       //�˿�
    index++;        //ͨ������
    memcpy(addr,&buf[index],6);
    index+=6;
    //������֡����������376.2�¼���ʶ��1 �� ���ܱ���豸���¼������ϱ�
    DataLen = XADR_GetEleNum(&buf[index],&lenoffset);
    index+=lenoffset;
    TASK_FMT_DEBUG( 0, RELAY_LOG, "�յ�����ϱ����¼� ���� %d \n",DataLen);
    if(DataLen!=0)
    {
        memcpy(DataBuf,&buf[index],DataLen);
        index+=DataLen;
        TASK_BUF_DEBUG( 0, RELAY_LOG,DataBuf,DataLen);
    }
    

    for(i=0;i<pAmrThreadArg->taskRunList.taskNum;i++)
    {
        runTask  = &pAmrThreadArg->taskRunList.taskList[i];
        if(runTask->transTask->taskTypeData.taskType == 5)
        {
            if((DataLen == 0 && runTask->transTask->taskTypeData.addi_param.condition.type == 2) ||
                (DataLen > 0 && fujian_meter_event_report_check(DataBuf,DataLen,&runTask->transTask->taskTypeData.addi_param.condition)==TRUE))
            {
                DT_Time2DateTimeShort(time(NULL),&runTask->triggerTime);
                TASK_FMT_DEBUG(0, RELAY_LOG, "���񷽰��� %d ���¼��ϱ�����\n",runTask->taskid);
                //���ж��´�����ִ�еľ����������
                if(runTask->transTask->taskTypeData.addi_param.condition.object ==1)
                {
                    for(j=0;j<runTask->meterNum;j++)
                    {
                        if(memcmp(addr,runTask->meterInfo[j].meterAddr.add,6) == 0)
                        {
                            runTask->meterInfo[j].triggerFlag = 1;
                            TASK_FMT_DEBUG(0, RELAY_LOG, "������ַΪ %02x%02x%02x%02x%02x%02x\n",addr[0],addr[1],addr[2],
                                addr[3],addr[4],addr[5]);
                            break;
                        }
                    }

                }
            }

        }
    }
                
}
/*******************************************************************************
* ��������: taskmng_trans_task_meter_num_get
* ��������://��ȡ����������
* �������:

* �������: 
* �� �� ֵ:
*******************************************************************************/

uint16 taskmng_trans_task_meter_num_get(FUJIAN_TASK_RUN_T * runTask)
{
    uint16 num = 0;
    FUJIAN_SPECIFIC_TASK_T *tmp = runTask->specificTask;
    while(tmp)
    {
        num+=tmp->addrAll.num;
        tmp = tmp->next;
    }
    return num;
}

/*******************************************************************************
* ��������: taskmng_trans_task_collection_state_update
* ��������://͸�����񷽰��ɼ�״̬�� ����
* �������:

* �������: 
* �� �� ֵ:
*******************************************************************************/
void taskmng_trans_task_collection_state_update(uint8 pipe,FUJIAN_TASK_RUN_T * runTask,FUJIAN_TASK_STATE_CMD_E cmd,void* inbuf)
{
    uint16          i;
    int             ret;
    time_t exectime = 0;
    OOP_TRANS_TASK_RLUESTATE_T *data = NULL;

    pthread_mutex_lock(&gTaskCollectionState.lock);
    for(i = 0;i<gTaskCollectionState.taskNum;i++)
    {
        if(runTask->taskid == gTaskCollectionState.taskState[i].transTaskid)
        {
            data = &gTaskCollectionState.taskState[i];
            break;
        }
    }
    if(data  == NULL)
    {
        TASK_FMT_DEBUG(pipe, REC_LOG, "δ�ҵ����񷽰��Ųɼ�״̬ %d\n",runTask->taskid);
        pthread_mutex_unlock(&gTaskCollectionState.lock);
        return ;
    }
    //���û�г�ʼ���� �����������Ӧ
    if(cmd!=TRANS_TASK_INIT)
    {
        if(data->storeTime.year == 0)
        {
            TASK_FMT_DEBUG(pipe, REC_LOG, "���񷽰� %d �ɼ�״̬δ��ʼ��\n",runTask->taskid);
            pthread_mutex_unlock(&gTaskCollectionState.lock);
            return;
        }
    }
    
    switch(cmd)
    {
        case TRANS_TASK_CLEAR: //�������
        {
            //ֻ�屾�ִε� �Է���ǰ��ʱ�������ݳ�ʼ��
            if(memcmp(&data->storeTime,&runTask->storageTime,sizeof(OOP_DATETIME_S_T))!=0)
            {
                TASK_FMT_TRACE(pipe, REC_LOG, "��ǰ��ʱ�󲻸��²ɼ�״̬��\n");
                pthread_mutex_unlock(&gTaskCollectionState.lock);
                return;
            }
            memset(data,0x00,sizeof(OOP_TRANS_TASK_RLUESTATE_T));
            data->transTaskid = runTask->taskid;
            data->storeTime = runTask->storageTime;
            //�洢����Ϊ0ʱ�ϱ���Ҫ��1
            if(runTask->transTask->storeNum == 0)
                data->storenum = 1;
            else
                data->storenum = runTask->storageCnt;
            //data->firststartTime = runTask->thisTurnBeginTime;
            data->paramSum = taskmng_trans_task_meter_num_get(runTask);
        }
        break;
        case TRANS_TASK_INIT: //��ʼ��
            {
                //���·������¾�������ʱ������ܸ���Ϊ0�����
                TASK_FMT_DEBUG(pipe, REC_LOG, "��ʼ�� paramSum %d \n",data->paramSum);
                fujian_datetime_s_log_write(pipe, REC_LOG, &data->firststartTime);
                fujian_datetime_s_log_write(pipe, REC_LOG, &data->storeTime);
                if(memcmp(&data->storeTime,&runTask->storageTime,sizeof(OOP_DATETIME_S_T))==0
                        && data->paramSum == taskmng_trans_task_meter_num_get(runTask))
                    break;
                memset(data,0x00,sizeof(OOP_TRANS_TASK_RLUESTATE_T));
                data->transTaskid = runTask->taskid;
                data->storeTime = runTask->storageTime;
                if(runTask->transTask->storeNum == 0)
                    data->storenum = 1;
                else
                    data->storenum = runTask->storageCnt;
                //data->firststartTime = runTask->thisTurnBeginTime;
                data->paramSum = taskmng_trans_task_meter_num_get(runTask);
                //������ִ�п�ʼʱ������ִ��ʱ�丳��2099��12��31��23:59:59
                data->firststartTime.year = 2099;
                data->firststartTime.month = 12;
                data->firststartTime.day = 31;
                data->firststartTime.hour = 23;
                data->firststartTime.minute = 59;
                data->firststartTime.second = 59;
                data->laststartTime = data->firststartTime;
            }
            break;
        case TRANS_TASK_START: //���¿�ʼʱ��
        {
            //����������� ����¿�ʼʱ�� �޸�������
            if(data->firststartTime.year != 0 && data->firststartTime.year !=2099)
                    break;
            time_t timenow = time(NULL);
            DT_Time2DateTimeShort(timenow,&data->firststartTime);
            DT_Time2DateTimeShort(timenow,&data->laststartTime);
        }
            break;
        case TRANS_TASK_EXEC: //�������ִ��ʱ��
        {
            if(inbuf != NULL)
            {
                memcpy(&exectime , (time_t*)inbuf,sizeof(time_t));
                DT_Time2DateTimeShort(exectime,&data->laststartTime);
            }
        }
            break;
        case TRANS_TASK_SUCC: //�ɹ���ʱ��Ҳ���������ִ��ʱ��
            {
                if(inbuf != NULL)
                {
                    memcpy(&exectime , (time_t*)inbuf,sizeof(time_t));
                    DT_Time2DateTimeShort(exectime,&data->laststartTime);
                }
                if(data->success>=data->paramSum)
                    break;
                data->success++;
                data->machingSum++;

            }
            break;
        case TRANS_TASK_RELATION:
        {
                data->success = 0;
                data->machingSum = 0;
        }
        break;
        default:
            break;
    }
    TASK_FMT_DEBUG(pipe, REC_LOG, "���񷽰� %d all %d �洢���� %d succ %d ���ִ��ʱ�� %d:%d:%d cmd %d\n",runTask->taskid,data->paramSum,
            data->storenum,data->success,data->laststartTime.hour,data->laststartTime.minute,data->laststartTime.second,cmd);
    //�����������ͣ״̬ ��ֻ���� ��д��������
    if(runTask->pauseFlag == 1)
    {
        TASK_FMT_TRACE(pipe, REC_LOG, "���񷽰���ͣ �����²ɼ�״̬��\n");
        pthread_mutex_unlock(&gTaskCollectionState.lock);
        return;

    }

    //����״̬��д��¼������ д֮ǰ����Ҫ����������
    uint32 storeNo;
    WRITE_RECORD_T inData;
    memset(&inData,0x00,sizeof(WRITE_RECORD_T));
    inData.recordOAD.infoNum = runTask->taskid;
    inData.recordOAD.classtag = 5;
    inData.recordOAD.road.oadMain.value = 0;
    inData.recordOAD.road.oadCols.nNum = 1;
    inData.recordOAD.road.oadCols.oad[0].value = 0x6E200200;
    DT_DateTimeShort2Time(&runTask->storageTime,(time_t*)&inData.colStoreTm);
    inData.inDatalen = sizeof(OOP_TRANS_TASK_RLUESTATE_T);
    memcpy(inData.inDataBuf,data,sizeof(OOP_TRANS_TASK_RLUESTATE_T));

    //�Ȳ�ѯ������������û����ʷ���� ����������
    RECORD_UNIT_T RecordOut;
    memset(&RecordOut,0x00,sizeof(RECORD_UNIT_T));
    READ_RECORD_T ReadRecord;
    memset(&ReadRecord,0x00,sizeof(READ_RECORD_T));
    memcpy(&ReadRecord.recordOAD,&inData.recordOAD,sizeof(RECORD_OAD_T));
    ReadRecord.cType = COL_TM_STORE;
    ReadRecord.cStart = inData.colStoreTm;
    ReadRecord.cEnd = inData.colStoreTm;
    ReadRecord.sortType = 0;

    ret = db_read_record(gAmrThreadArg[pipe].DBClientHandle,&ReadRecord,DATA_LEN_MAX,&RecordOut);
    if(ret == 0)
    {
        TASK_FMT_TRACE(pipe, REC_LOG, "�ɼ�״̬����Ҫ��������\n");
        UPDATE_RECORD_T update;
        memset(&update,0x00,sizeof(UPDATE_RECORD_T));
        memcpy(&update.recordOAD,&inData.recordOAD,sizeof(RECORD_OAD_T));
        update.filter.choice = 1;
        update.filter.storeTm = inData.colStoreTm;

        update.inDatalen = sizeof(OOP_TRANS_TASK_RLUESTATE_T);
        memcpy(&update.inDataBuf,data,sizeof(OOP_TRANS_TASK_RLUESTATE_T));
        ret = db_update_record(gAmrThreadArg[pipe].DBClientHandle,&update);
        if(ret != 0)
        {
            TASK_FMT_TRACE(pipe, REC_LOG, "�ɼ�״̬������ʧ�� ret is %d\n", ret);
        }
    }
    else
    {
        ret = db_write_record(gAmrThreadArg[pipe].DBClientHandle, &inData, &storeNo);
        if(ret != 0)
        {
            TASK_FMT_TRACE(pipe, REC_LOG, "�ɼ�״̬���洢ʧ�� ret is %d\n", ret);
        }
        
    }

    pthread_mutex_unlock(&gTaskCollectionState.lock);
}

/*******************************************************************************
* ��������: taskmng_pipe_collection_state_write
* ��������: дͨ���ɼ�״̬
* �������:

* �������: 
* �� �� ֵ:
*******************************************************************************/

void taskmng_pipe_collection_state_write(uint8 pipe,FUJIAN_TASK_TYPE_E type,uint16 taskid,uint8 priority)
{
    AmrThreadArg_T * pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    int ret;
    OOP_CHAN_RLUESTATE_T state;
    memset(&state,0x00,sizeof(OOP_CHAN_RLUESTATE_T));
    state.oad = taskmng_pipe_oad_get(pipe);
    state.taskType = type;
    state.transTaskid = taskid;
    state.priority = priority;
    if(memcmp(&state,&pAmrThreadArg->collectionState,sizeof(OOP_CHAN_RLUESTATE_T)) == 0)
    {
        return;
    }
    memcpy(&pAmrThreadArg->collectionState,&state,sizeof(OOP_CHAN_RLUESTATE_T));
    NOMAL_OAD_T     NormalOad;
    memset(&NormalOad,0x00,sizeof(NOMAL_OAD_T));
    NormalOad.oad.value = 0x6E220200;
    NormalOad.infoNum = pipe;
    NormalOad.classtag = 3;
    ret = db_write_nomal(gAmrThreadArg[pipe].DBClientHandle,&NormalOad,(uint8*)&state,sizeof(OOP_CHAN_RLUESTATE_T));
    if(ret!=0)
    {
        TASK_FMT_DEBUG(pipe, REC_LOG, "ͨ���ɼ�״̬дʧ�� ret %d \n",ret);
    }
    TASK_FMT_DEBUG(pipe, REC_LOG, "ͨ���ɼ�״̬д�ɹ� type %d \n",type);
}

/*******************************************************************************
* ��������: taskmng_grop_meter_file_change_fun 
* ��������:Ⱥ�鵵������Ĵ���
* �������: pMeterFileAll �����
          choice 1 ��ʾ������� �������߸ı�
          choice 0 ��ʾ����ɾ��
          gMeterFile��nindexΪɾ�������
* �������: 
* �� �� ֵ: 
*******************************************************************************/
void taskmng_grop_meter_file_change_fun(FUJIAN_grop_meter *pMeterFileAll,uint8 choice,OOP_GROUPMETER_NEW_T gMeterFile)
{
    uint16 i,j;
    int index =-1;
    
    pthread_rwlock_wrlock(&pMeterFileAll->rwLock);

    TASK_FMT_DEBUG(-1,TASK_LOG, "���뵵���޸����� choice %d \n",choice);
    if(choice==0)
    {
        for(i=0;i<pMeterFileAll->nNum;i++)
        {
            if(memcmp(pMeterFileAll->grop_meter[i].groupAddr.add,gMeterFile.groupAddr.add,6)==0)                
            {
                index = i;
                memset(&pMeterFileAll->grop_meter[i],0x00,sizeof(OOP_GROUPMETER_NEW_T));               
                break; 
            }
        }
        if(index!=-1)
        {
            for(i=index;i<pMeterFileAll->nNum-1;i++)
            {
                pMeterFileAll->grop_meter[i] = pMeterFileAll->grop_meter[i+1];
            }
            memset(&(pMeterFileAll->grop_meter[pMeterFileAll->nNum-1]),0x00,sizeof(OOP_METER_T));
            pMeterFileAll->nNum--;
        }
    }
    else
    {
        for(i=0;i<pMeterFileAll->nNum;i++)
        {
            //�����ǰ�������ҵ���ͬ�� ��ֱ���滻
            if(memcmp(pMeterFileAll->grop_meter[i].groupAddr.add,gMeterFile.groupAddr.add,6)==0)
            {
                index = i;
                memcpy(&pMeterFileAll->grop_meter[i],&gMeterFile,sizeof(OOP_GROUPMETER_NEW_T));
                break; 
            }
        }
        //���û���ҵ� ������
        if(index==-1)
        {
            pMeterFileAll->grop_meter[pMeterFileAll->nNum] = gMeterFile;
            pMeterFileAll->nNum++;
        }
    }
    TASK_FMT_DEBUG(-1,TASK_LOG, "Ⱥ�鵵������  %d \n",pMeterFileAll->nNum);
    for(i=0;i<pMeterFileAll->nNum;i++)
    {
            TASK_FMT_DEBUG(-1,TASK_LOG, "Ⱥ�鵵����ַ\n");
            TASK_BUF_DEBUG(-1,TASK_LOG,pMeterFileAll->grop_meter[i].groupAddr.add,6);
        for(j=0;j<pMeterFileAll->grop_meter[i].num;j++)
        {
            TASK_FMT_DEBUG(-1,TASK_LOG, "����%d ��ַ\n",j+1);
            TASK_BUF_DEBUG(-1,TASK_LOG,&pMeterFileAll->grop_meter[i].addr[j],6);
        }
        
    }
    pthread_rwlock_unlock(&pMeterFileAll->rwLock);

}


/*******************************************************************************
* ��������: taskmng_grop_meter_file_update_check 
* ��������: ����Ƿ��յ�Ⱥ�鵵�����µ���Ϣ 
* �������: 
* �������: �����ĸ�����Ϣ
* �� �� ֵ: 
*******************************************************************************/
void taskmng_grop_meter_file_update_check(DB_UPDATE_T *dbupdate)
{
    int Recvlen = 0;

    Recvlen = taskmng_db_update_queue_read(&pGropMeterFileUpdate, dbupdate);
    if(Recvlen>0)
    {

        TASK_FMT_DEBUG(-1,TASK_LOG, "�յ�mqtt���͵ĸ���Ⱥ�鵵����Ϣ infonum %d oad %08x !\n",dbupdate->infonum,dbupdate->oad);
        oop_grop_meter.FileState = TASK_CFG_MODIFY;
    }
}
/*******************************************************************************
* ��������: taskmng_grop_meter_file_read_from_pn
* ��������: ������Ϣ��Ŵ��������Ķ�ȡȺ�鵵��
* �������: 
            infonum     ��Ϣ���
           
* �������: gMeterFile  Ⱥ�鵵����Ϣ
* �� �� ֵ:  0 �ɹ�

*******************************************************************************/

uint8 taskmng_grop_meter_file_read_from_pn(uint16 infonum,OOP_GROUPMETER_NEW_T *gMeterFile)
{
    int   ret;
    uint32 len=0;
    NOMAL_OAD_T NormalOad;
    NormalOad.logicId = 0;
    NormalOad.oad.value = 0x6E050200;
    NormalOad.infoNum = infonum;
    
#if DBCENTER_MQTT
    ret = db_read_nomal(gDBClientMain,&NormalOad,sizeof(OOP_GROUPMETER_NEW_T),(uint8*)gMeterFile,&len);
#else
    ret = nomal_data_read(MainHandle, NormalOad, sizeof(OOP_GROUPMETER_NEW_T),(uint8*)gMeterFile, &len);
#endif
    if(ret !=0 ||len!=sizeof(OOP_GROUPMETER_NEW_T))
    {
        TASK_FMT_DEBUG(-1,TASK_LOG, "��ȡȺ���ַʧ�� ret %d len %d  nomal len %d \n",ret,len,sizeof(OOP_GROUPMETER_NEW_T));
         return -1 ;
    }
       
    
    return 0;
}

/*******************************************************************************
* ��������: taskmng_grop_meter_file_delete
* ��������: ɾ��ȫ�������
* �������: 
            pMeterFileAll   //�����
           
* �������: ��
* �� �� ֵ:   ������ʣ��ĵ�����
*******************************************************************************/
uint16 taskmng_grop_meter_file_delete(FUJIAN_grop_meter *pMeterFileAll)
{
    uint16 leftcount = 0;
    pthread_rwlock_wrlock(&pMeterFileAll->rwLock);

    pMeterFileAll->nNum = leftcount;
    memset(pMeterFileAll->grop_meter,0x00,sizeof(OOP_GROUPMETER_NEW_T)*255);
    pthread_rwlock_unlock(&pMeterFileAll->rwLock);
    return leftcount;
}

/*******************************************************************************
* ��������: taskmng_grop_meter_file_ergodic
* ��������: ���Ⱥ�鵵��
* �������: 
            oop_grop_meter   //Ⱥ�鵵��
           
* �������: ��
* �� �� ֵ:  
*******************************************************************************/

void taskmng_grop_meter_file_ergodic(FUJIAN_grop_meter *oop_grop_meter)
{
    time_t RealTime_t;
    int ret;
    OOP_GROUPMETER_NEW_T gMeterFile;
    uint16 leftcount=0;
    //OOP_OAD_U oad = {0xF1010200};
    switch(oop_grop_meter->FileState)
    {
    case TASK_CFG_MODIFY:
        //�յ����������Ϣ����д���
        
        //0xff��ʾ���е���ɾ��
        if(dbupdateMeter.infonum == 0xffff)
        {
            TASK_FMT_DEBUG(-1,TASK_LOG, "infonum %d Ⱥ�鵵����� !\n",dbupdateMeter.infonum);
            
            leftcount = taskmng_grop_meter_file_delete(oop_grop_meter);
            TASK_FMT_DEBUG(-1,TASK_LOG, "ʣ��Ⱥ�鵵������ %d \n",leftcount);
        }else
        {
            memset(&gMeterFile,0x00,sizeof(OOP_GROUPMETER_NEW_T));
            ret = taskmng_grop_meter_file_read_from_pn(dbupdateMeter.infonum,&gMeterFile);
            TASK_FMT_DEBUG(-1,TASK_LOG, "��ȡ����%d logicid %d ret %d !\n",dbupdateMeter.infonum,dbupdateMeter.logicid,ret);
            if(ret==0)
            {
                //���ɾ��ʱͬʱɾ�����������а�ȫģʽ
                //taskmng_normal_data_delete(dbupdateMeter.infonum,oad);
                //��ȡ��������Ϣ ��ʾ���������������
                TASK_FMT_DEBUG(-1,TASK_LOG, "Ⱥ�鵵����ַ \n");
                TASK_BUF_DEBUG(-1,TASK_LOG, gMeterFile.groupAddr.add,6);
                if(taskmng_grop_addr_isvalid(gMeterFile.groupAddr.add)>0)
                {
                    taskmng_grop_meter_file_change_fun(oop_grop_meter,1,gMeterFile);
                }
                 
                else
                {
                    TASK_FMT_DEBUG(-1,TASK_LOG, "Ⱥ�鵵����ַ���Ϸ� \n");
                }
            }
            else
            {
                //δ��ȡ��������Ϣ����ʾ����ɾ��
               memset(gMeterFile.groupAddr.add,0x88,4);
               gMeterFile.groupAddr.add[4] = dbupdateMeter.infonum/100;
               gMeterFile.groupAddr.add[5] = dbupdateMeter.infonum%100;
               gMeterFile.groupAddr.add[4] = commfun_HexToBcd(gMeterFile.groupAddr.add[4]);
               gMeterFile.groupAddr.add[5] = commfun_HexToBcd(gMeterFile.groupAddr.add[5]);
               taskmng_grop_meter_file_change_fun(oop_grop_meter,0,gMeterFile);
            }
        }
        //����30���ӳ� Ԥ������޸ĵ���
        oop_grop_meter->ModifyDelaySec = 5; //ԭ��30�룬�����ȸ����̵�5��
        RealTime_t = time(NULL);
        oop_grop_meter->FunLastRunTime = RealTime_t;
        oop_grop_meter->FileState = TASK_CFG_MODIFY_DELAY;
        break;
    case TASK_CFG_MODIFY_DELAY:

        RealTime_t = time(NULL);
        if(RealTime_t!=oop_grop_meter->FunLastRunTime)
        {   
            oop_grop_meter->ModifyDelaySec--;
            oop_grop_meter->FunLastRunTime=RealTime_t;
            TASK_FMT_DEBUG(-1,TASK_LOG, "��ʱ%d \n",oop_grop_meter->ModifyDelaySec);
        }
        //��ʱ���� ֪ͨ�������̵߳������
        if(oop_grop_meter->ModifyDelaySec<=0)
        {   
            oop_grop_meter->FileState = TASK_CFG_INITED;
            ReloadTaskFlag = 1;
        }
        break;
    default:
        break;   
    }
}

/*******************************************************************************
* ��������: fujian_collection_update_deal 
* ��������:����ɼ�״̬�е��ѷ��ͺ���ȷ������
* �������:

* �������: 
* �� �� ֵ: 
*******************************************************************************/
void fujian_collection_update_deal(uint8* buf,uint16 len)
{
    uint16          i;
    int             ret;
    OOP_TRANS_TASK_RLUESTATE_T *data = NULL;
    uint16 index = 0;
    uint16 taskid;
    uint8 cmd ;
    uint16 num;
    OOP_DATETIME_S_T storgeTime;
    memset(&storgeTime,0x00,sizeof(OOP_DATETIME_S_T));
    memcpy_r(&taskid,&buf[index],2);
    index+=2;
    memcpy_r(&storgeTime.year,&buf[index],2);
    index+=2;
    storgeTime.month = buf[index++];
    storgeTime.day = buf[index++];
    storgeTime.hour = buf[index++];
    storgeTime.minute = buf[index++];
    storgeTime.second = buf[index++];
    cmd = buf[index++];
    memcpy_r(&num,&buf[index],2);
    index+=2;
    
    pthread_mutex_lock(&gTaskCollectionState.lock);
    for(i = 0;i<gTaskCollectionState.taskNum;i++)
    {
        if(taskid == gTaskCollectionState.taskState[i].transTaskid)
        {
            data = &gTaskCollectionState.taskState[i];
            break;
        }
    }
    if(data  == NULL)
    {
        TASK_FMT_DEBUG(-1, TASK_LOG, "δ�ҵ����񷽰��Ųɼ�״̬ %d\n",taskid);
        pthread_mutex_unlock(&gTaskCollectionState.lock);
        return ;
    }

    uint32 tmptime;
    RECORD_UNIT_T RecordOut;
    memset(&RecordOut,0x00,sizeof(RECORD_UNIT_T));
    READ_RECORD_T ReadRecord;
    memset(&ReadRecord,0x00,sizeof(READ_RECORD_T));
    ReadRecord.recordOAD.infoNum = taskid;
    ReadRecord.recordOAD.classtag = 5;
    ReadRecord.recordOAD.road.oadMain.value = 0;
    ReadRecord.recordOAD.road.oadCols.nNum = 1;
    ReadRecord.recordOAD.road.oadCols.oad[0].value = 0x6E200200;
    UPDATE_RECORD_T update;
    memset(&update,0x00,sizeof(UPDATE_RECORD_T));

    if(storgeTime.year == data->storeTime.year && storgeTime.month == data->storeTime.month &&
        storgeTime.day == data->storeTime.day && storgeTime.hour == data->storeTime.hour && 
        storgeTime.minute == data->storeTime.minute && storgeTime.second == data->storeTime.second)
    {
        if(cmd == 0)
        {
            if(data->sendNum+num > data->paramSum)
            {
                data->sendNum = data->paramSum;
            }else
            {
                data->sendNum+=num;
            }
        }else
        {
            if(data->receNum+num > data->paramSum)
            {
                data->receNum = data->paramSum;
            }else
            {
                data->receNum+=num;
            }
        }

        DT_DateTimeShort2Time(&data->storeTime,(time_t*)&tmptime);

        ReadRecord.cType = COL_TM_STORE;
        ReadRecord.cStart = tmptime;
        ReadRecord.cEnd = tmptime;
        ReadRecord.sortType = 0;
        
        ret = db_read_record(gDBClientTask,&ReadRecord,DATA_LEN_MAX,&RecordOut);
        if(ret == 0)
        {
            TASK_FMT_DEBUG(-1, TASK_LOG, "�ɼ�״̬���������� taskid %d send %d recv %d \n",taskid,data->sendNum,data->receNum);

            memcpy(&update.recordOAD,&ReadRecord.recordOAD,sizeof(RECORD_OAD_T));
            update.filter.choice = 1;
            update.filter.storeTm = tmptime;
        
            update.inDatalen = sizeof(OOP_TRANS_TASK_RLUESTATE_T);
            memcpy(&update.inDataBuf,data,sizeof(OOP_TRANS_TASK_RLUESTATE_T));
            ret = db_update_record(gDBClientTask,&update);
            if(ret != 0)
            {
                TASK_FMT_DEBUG(-1, TASK_LOG, "�ɼ�״̬������ʧ�� ret is %d\n", ret);
            }
        } 

    }
    else
    {
        //������ǵ�ǰ�ִε� ��Ҫ���������Ķ���������
        DT_DateTimeShort2Time(&storgeTime,(time_t*)&tmptime);
        ReadRecord.cType = COL_TM_STORE;
        ReadRecord.cStart = tmptime;
        ReadRecord.cEnd = tmptime;
        ReadRecord.sortType = 0;
        
        ret = db_read_record(gDBClientTask,&ReadRecord,DATA_LEN_MAX,&RecordOut);
        if(ret == 0 && RecordOut.record[0].outDatalen == sizeof(OOP_TRANS_TASK_RLUESTATE_T))
        {
            OOP_TRANS_TASK_RLUESTATE_T readdata;
            memset(&readdata,0x00,sizeof(OOP_TRANS_TASK_RLUESTATE_T));
            memcpy(&readdata,RecordOut.record[0].outDataBuf,sizeof(OOP_TRANS_TASK_RLUESTATE_T));
            TASK_FMT_DEBUG(-1, TASK_LOG, "�������Ķ������洢ʱ�� %d��%d�� %d:%d:%d taskid %d send %d recv %d paramSum %d\n",
                readdata.storeTime.month,readdata.storeTime.day,readdata.storeTime.hour,readdata.storeTime.minute,
                    readdata.storeTime.second ,taskid,readdata.sendNum,readdata.receNum,readdata.paramSum);
            if(cmd == 0)
            {
                if(readdata.sendNum+num > readdata.paramSum)
                {
                    readdata.sendNum = readdata.paramSum;
                }else
                {
                    readdata.sendNum+=num;
                }
            }else
            {
                if(readdata.receNum+num > readdata.paramSum)
                {
                    readdata.receNum = readdata.paramSum;
                }else
                {
                    readdata.receNum+=num;
                }
            }

            memcpy(&update.recordOAD,&ReadRecord.recordOAD,sizeof(RECORD_OAD_T));
            update.filter.choice = 1;
            update.filter.storeTm = tmptime;
        
            update.inDatalen = sizeof(OOP_TRANS_TASK_RLUESTATE_T);
            memcpy(&update.inDataBuf,&readdata,sizeof(OOP_TRANS_TASK_RLUESTATE_T));
            
            TASK_FMT_DEBUG(-1, TASK_LOG, "�ɼ�״̬������ǰ���ִ����� �洢ʱ�� %d��%d�� %d:%d:%d taskid %d send %d recv %d \n",
                readdata.storeTime.month,readdata.storeTime.day,readdata.storeTime.hour,readdata.storeTime.minute,
                    readdata.storeTime.second ,taskid,readdata.sendNum,readdata.receNum);
            ret = db_update_record(gDBClientTask,&update);
            if(ret != 0)
            {
                TASK_FMT_DEBUG(-1, TASK_LOG, "�ɼ�״̬������ʧ�� ret is %d\n", ret);
            }
        }

    }
    pthread_mutex_unlock(&gTaskCollectionState.lock);
}

/*******************************************************************************
* ��������: taskmng_trans_task_error_report
* ��������: ����ʧ�ܵı��ϱ���� ���������Ķ����һ����¼�ϱ�
* �������: ��
* �������: ��
* �� �� ֵ: 0                  �ɹ�
*           <0                  ʧ��
*******************************************************************************/

int taskmng_trans_task_error_report(uint8 pipe,OOP_TSA_T *tsa,time_t exectime,FUJIAN_TASK_RUN_T * runTask,FUJIAN_SPECIFIC_TASK_T *currentSpecificTask)
{
    uint16 i;
    OOP_TRANS_TASK_RESULT_T transResult;
    memset(&transResult,0x00,sizeof(OOP_TRANS_TASK_RESULT_T));
    int ret;
    time_t storetime,readtime;
    DT_DateTimeShort2Time(&runTask->storageTime,(time_t*)&storetime);

    RECORD_UNIT_T RecordOut;
    memset(&RecordOut,0x00,sizeof(RECORD_UNIT_T));
    READ_RECORD_T ReadRecord;
    memset(&ReadRecord,0x00,sizeof(READ_RECORD_T));
    ReadRecord.recordOAD.infoNum = runTask->taskid;
    ReadRecord.recordOAD.classtag = 5;
    ReadRecord.recordOAD.road.oadMain.value = 0x6E150200;
    ReadRecord.recordOAD.road.oadCols.nNum = 1;
    ReadRecord.recordOAD.road.oadCols.oad[0].value = currentSpecificTask->nodeid;
    ReadRecord.MAC.nNum = 6;
    memcpy(ReadRecord.MAC.value,tsa->add,6);
    ReadRecord.cType = COL_TM_STORE;
    ReadRecord.cStart = storetime;
    ReadRecord.cEnd = storetime;
    ReadRecord.sortType = 0;

    ReadRecord.bTable = 1;     
    memcpy(&ReadRecord.table,&ReadRecord.MAC,sizeof(OOP_OCTETVAR64_T));
    ret = db_read_record(gAmrThreadArg[pipe].DBClientHandle,&ReadRecord,DATA_LEN_MAX,&RecordOut);
    if(ret == 0 && RecordOut.record[0].outDatalen == sizeof(OOP_TRANS_TASK_RESULT_T))
    {
        TASK_FMT_TRACE(pipe, REC_LOG, "��ȡ�����һ����¼�ϱ�\n");
        memcpy(&transResult,RecordOut.record[0].outDataBuf,sizeof(OOP_TRANS_TASK_RESULT_T));
        //�Ա��»�ȡ���ļ�¼�Ĵ洢ʱ��͵�ǰһ��һ��
        if(memcmp(&transResult.storeTime, &runTask->storageTime, sizeof(OOP_DATETIME_S_T)) == 0)
        {
            //������������ĵĶ��ĳ���ʱ�仹���ϵ� �������
            DT_DateTimeShort2Time(&transResult.readTime ,&readtime);
            if(readtime < exectime)
            {
                DT_Time2DateTimeShort(exectime,&transResult.readTime);
            }
            transResult.retryTurn = runTask->retryCycle + 1;
            fujian_task_data_event_report(pipe, &transResult, 0);
            for(i=0;i<currentSpecificTask->addrAll.num;i++)
            {
                if(memcmp(tsa,&currentSpecificTask->addrAll.addr[i],sizeof(OOP_TSA_T)) == 0)
                {
                    currentSpecificTask->addrAll.isReport[i] = 1;
                    break;
                }
            }
            return 0;
        }
    }

    return -1;


}

/*******************************************************************************
* ��������: taskmng_trans_task_isreport_check
* ��������: ����һ�ֽ���ʱ�������û��û�ϱ�������
* �������: ��
* �������: ��
* �� �� ֵ: 0                  �ɹ�
*           <0                  ʧ��
*******************************************************************************/

void taskmng_trans_task_isreport_check(uint8 pipe,FUJIAN_TASK_RUN_T * runTask)
{
    uint16 i,j;
    uint8 flag = 0;
    int ret;
    FUJIAN_SPECIFIC_TASK_T *specificTask =NULL;
    //������ͣʱ���ϱ����
    if(runTask->pauseFlag == 1 || runTask->result == SCHEME_FINISHED)
        return;
    for(j=0;j<runTask->meterNum;j++)
    {
        if(runTask->meterInfo[j].meterResult == 2)
            continue;
        specificTask = runTask->specificTask;
        flag = 0;
        ret =-1;
        while(specificTask)
        {
            for(i=0;i<specificTask->addrAll.num;i++)
            {
                if(specificTask->addrAll.isReport[i] == 0 && 
                    memcmp(&specificTask->addrAll.addr[i],&runTask->meterInfo[j].meterAddr,sizeof(OOP_TSA_T))==0)
                {
                    TASK_FMT_TRACE(pipe, REC_LOG, "������ %d ��������� %d ��� %02x%02x%02x%02x%02x%02x ��δ�ϱ���� flag %d\n",runTask->taskid,
                            specificTask->nodeid,specificTask->addrAll.addr[i].add[0],specificTask->addrAll.addr[i].add[1],specificTask->addrAll.addr[i].add[2],
                            specificTask->addrAll.addr[i].add[3],specificTask->addrAll.addr[i].add[4],specificTask->addrAll.addr[i].add[5],flag);
                    if(flag == 0)
                    {
                        ret = taskmng_trans_task_error_report(pipe,&specificTask->addrAll.addr[i],runTask->meterInfo[j].execTime,runTask,specificTask);
                        if(ret < 0)
                        {
                            TASK_FMT_TRACE(pipe, REC_LOG,"δ��ȡ���������\n");
                        }
                        specificTask->addrAll.isReport[i] = 1;
                        flag = 1;
                    }else
                    {
                        if(ret == 0)
                        {
                            taskmng_trans_task_result_save(pipe,1,&runTask->meterInfo[j].meterAddr,runTask->meterInfo[j].execTime, runTask,specificTask,OOP_FJ_ERR_TASK_NO_EXECUTE, NULL,0);
                        }
                        specificTask->addrAll.isReport[i] = 1;
                    }
                }
            }
            specificTask = specificTask->next;
        }

    }

}

#endif

