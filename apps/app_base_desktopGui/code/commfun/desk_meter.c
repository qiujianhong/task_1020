/*
��ʾ�������ӶԵ����Ĵ����̣߳����𵵰��ļ��أ�����ʵʱ��ѯ������ʱ��Ῠ
*/

#include "commfun.h"
#include "db_api.h"
#include "message.h"
#include "framecomm.h"
#include "task_id.h"
#include "desk_meter.h"


DESK_METER_LIST_T gDeskMeter;   //�������е�����Ϣ
pthread_mutex_t gDeskMeterlock;

DBUPDATE_QUEUE_T gUpdateQueue;  //��������õĶ���
/*******************************************************************************
* ��������: meter_thread_param_init
* ��������: ȫ�ֱ�����ʼ��
* ���������
* ���������
* �ڲ�����
* �� �� ֵ: 
*******************************************************************************/
void meter_thread_param_init()
{
    memset(&gDeskMeter,0x00,sizeof(gDeskMeter));
    
    if(pthread_mutex_init(&gDeskMeterlock, NULL) != 0)
    {
    
        GUI_FMT_DEBUG("init gDeskMeter.lock error!\n");
        return;
    }
    gDeskMeter.meterStatus = DESK_METER_INIT;
    memset(&gUpdateQueue,0x00,sizeof(gUpdateQueue));

}

/*******************************************************************************
* ��������: meter_valid_check
* ��������: ��鵵���Ƿ�Ϸ�
* ���������meterinfo ����
* ���������
* �ڲ�����
* �� �� ֵ: TRUE �Ϸ� FALSE �Ƿ�����
*****************************************************************************/
BOOL meter_valid_check(OOP_METER_T *meterinfo)
{
    uint8 tmp[16];
    do
    {
        //�ж���Э�������Ƿ�����
        if(meterinfo->basic.protocol != METER_PRTL_97 && meterinfo->basic.protocol != METER_PRTL_07 &&
            meterinfo->basic.protocol != METER_PRTL_OOP)
        {
            GUI_FMT_DEBUG("pn %d ��Լ���� %d ����  !\n",meterinfo->nIndex,meterinfo->basic.protocol);
            break;
        }
        //�жϵ�ַ�Ƿ�Ϸ�
        memset(tmp,0x00,sizeof(tmp));
        if(memcmp(meterinfo->basic.tsa.add,tmp,meterinfo->basic.tsa.len+1) == 0)
        {
            GUI_FMT_DEBUG("pn %d ��ַȫ0\n",meterinfo->nIndex);
            break;
        }
        memset(tmp,0xff,sizeof(tmp));
        if(memcmp(meterinfo->basic.tsa.add,tmp,meterinfo->basic.tsa.len+1) == 0)
        {
            GUI_FMT_DEBUG("pn %d ��ַȫff\n",meterinfo->nIndex);
            break;
        }
        return TRUE;
    }
    while(0);
    
    return FALSE;
    
}


/*******************************************************************************
* ��������: meter_info_init
* ��������: ������ʼ������
* ���������
* ���������
* �ڲ�����
* �� �� ֵ: 0��ʾ��ʼ���ɹ� -1��ʾ��ʼ��ʧ��
******************************************************************************/
int meter_info_init(DB_CLIENT clientid,DESK_METER_LIST_T *meterList)
{
    uint16 i,num=0;
    int ret;
    OOP_METER_T meterInfo;
    READ_RECORD_T      inReadRecord;
    uint32             handle = 0;
    uint32             recNum = 0;
    STEP_READ_RECORD_T stepReadRecord;
    RECORD_UNIT_T      stepRecordOut;
    memset(&inReadRecord, 0, sizeof(READ_RECORD_T));
    memset(&stepReadRecord, 0, sizeof(STEP_READ_RECORD_T));
    memset(&stepRecordOut, 0, sizeof(RECORD_UNIT_T));
    GUI_FMT_DEBUG("��ʼ������ʼ��!\n");
    
    PTHREAD_LOCK_SAFE(&gDeskMeterlock);
    memset(meterList,0x00,sizeof(DESK_METER_LIST_T));
    
    inReadRecord.recordOAD.logicId = 0;
    inReadRecord.recordOAD.infoNum = 0xffff;
    inReadRecord.cType = COL_NO_INFO;
    inReadRecord.cStart = 1;
    inReadRecord.cEnd = 5000;
    inReadRecord.sortType = UP_SORT;
    inReadRecord.recordOAD.road.oadMain.value = 0;
    inReadRecord.recordOAD.road.oadCols.nNum = 0x01;
    inReadRecord.recordOAD.road.oadCols.oad[0].value = 0x60000200;

    stepReadRecord.MAC.nNum = 0;

    ret = db_read_record_start(clientid, &inReadRecord, &handle, &recNum);
    if(ret!=0 || recNum ==0)
    {
        GUI_FMT_DEBUG("��ʼ��������¼ʧ�� ret  %d recNum %d\n",ret,recNum);
    }else
    {
        for(i=0;i<recNum;i++)
        {
            ret = db_read_record_step(clientid, handle, &stepReadRecord, &stepRecordOut);
            if(ret!=0)
            {
                GUI_FMT_DEBUG("������������¼ʧ�� ret  %d i %d \n",ret,i);
                continue;
            }
            if(stepRecordOut.recordnum!=1 || stepRecordOut.record[0].road.oadCols.oad[0].value!=0x60000200
                ||stepRecordOut.record[0].outDatalen!=sizeof(OOP_METER_T))
            {
                GUI_FMT_DEBUG("������ȡ�ĵ�����¼������ \n");
                continue;
            }
            memcpy_s(&meterInfo,sizeof(meterInfo),stepRecordOut.record[0].outDataBuf,sizeof(OOP_METER_T));
            if(meter_valid_check(&meterInfo)==FALSE)
            {
                GUI_FMT_DEBUG("PN %d �����Ƿ� \n",meterInfo.nIndex);
                continue;
            }
            GUI_FMT_DEBUG("���pn %d port %08x prtl %d \n",meterInfo.nIndex,meterInfo.basic.port.value,meterInfo.basic.protocol);
            GUI_FMT_DEBUG("��ַ oldvxd %d vxd %d 0X%02x%02x%02x%02x%02x%02x \n",meterInfo.basic.tsa.oldvxd,meterInfo.basic.tsa.vxd,
                meterInfo.basic.tsa.add[0],meterInfo.basic.tsa.add[1],meterInfo.basic.tsa.add[2],meterInfo.basic.tsa.add[3],
                meterInfo.basic.tsa.add[4],meterInfo.basic.tsa.add[5]);
            meterList->meterInfo[num].pn = meterInfo.nIndex;
            meterList->meterInfo[num].tsa = meterInfo.basic.tsa;
            meterList->meterInfo[num].prtl = meterInfo.basic.protocol;
            meterList->meterInfo[num].port = meterInfo.basic.port;
            meterList->meterInfo[num].PT = meterInfo.extend.PT;
            meterList->meterInfo[num].CT = meterInfo.extend.CT;
            num++;
        }

    }
    db_read_record_end(clientid, handle);
    GUI_FMT_DEBUG( "��ʼ���󵵰�����Ϊ %d \n",num);
    meterList->meterNum = num;
    PTHREAD_UNLOCK_SAFE(&gDeskMeterlock);
    //ret������0��ʾ �����Ƕ��������ĳ�ʱ��   �հ�װ������ʱ����������������Ŀ�
    if(ret !=0 )
        return -1;
    else 
        return 0;
}

/*******************************************************************************
* ��������: taskmng_db_update_queue_read
* ��������: ������
* ���������pLockQueue          ���������ṹָ��
*           buf                 �����ݻ��������������е�����ȡ������û�����
* ���������plockqueue          �����е�����
* �ڲ�����: �ж������Ƿ������ݣ�����Ӷ�����ȡ�����ݷ���buf�У�ÿִ��һ�Σ�ȡһ������
* �� �� ֵ: len                 ���������ݳ���
*******************************************************************************/
int meter_update_queue_read(DB_UPDATE_T *buf)
{
    int len = 0;

    //��������
    PTHREAD_LOCK_SAFE(&gUpdateQueue.QueueLock);  
  
    //�������������ݣ���ȡ��buf�У��⿪��
    if ((gUpdateQueue.QueueHead != gUpdateQueue.QueueTail)
        || ((gUpdateQueue.QueueHead == gUpdateQueue.QueueTail)
            && (gUpdateQueue.Queue[gUpdateQueue.QueueHead].isvalid != 0)))
    {   
        //pdata_buf =&(pLockQueue->Queue[pLockQueue->QueueHead].dbupdate);  
        len = sizeof(DB_UPDATE_T);
        
        //ȡ�����е�����
        memcpy(buf, &(gUpdateQueue.Queue[gUpdateQueue.QueueHead].dbupdate), len); 

        //������
        memset(&(gUpdateQueue.Queue[gUpdateQueue.QueueHead].dbupdate), 0, len);   
        gUpdateQueue.Queue[gUpdateQueue.QueueHead].isvalid = 0;

        //����ͷ����ƶ�һλ
        gUpdateQueue.QueueHead= (gUpdateQueue.QueueHead + 1) % (MAX_DBUPDATE_QUEUE_CNT); 
    }
        
    PTHREAD_UNLOCK_SAFE(&gUpdateQueue.QueueLock);
    
    return len;
}

/*******************************************************************************
* ��������: taskmng_db_update_queue_write
* ��������: д����
* ���������plockqueue          ���������ṹָ��
            buf                 ��Ҫд����е�����
            buf_len             ���ݳ���
* �����������
* �ڲ�����: �����в�������buf�е����ݷ�������У���������������������
* �� �� ֵ: >0                  �ɹ�
            =0                  ʧ��
*******************************************************************************/
int meter_update_queue_write(DB_UPDATE_T *buf)
{
    int flags = 0;
        
    //��������
    PTHREAD_LOCK_SAFE(&gUpdateQueue.QueueLock);  

    //�ж������Ƿ��пյ�λ��
    if (gUpdateQueue.Queue[gUpdateQueue.QueueTail].isvalid == 0)
    {
        gUpdateQueue.Queue[gUpdateQueue.QueueTail].isvalid = 1;    
        memcpy(&(gUpdateQueue.Queue[gUpdateQueue.QueueTail].dbupdate), buf, sizeof(DB_UPDATE_T));
        flags = 1;
        gUpdateQueue.QueueTail = (gUpdateQueue.QueueTail + 1) % (MAX_DBUPDATE_QUEUE_CNT);
    }   
    
    //�⻺����
    PTHREAD_UNLOCK_SAFE(&gUpdateQueue.QueueLock);

    return flags;
}
/*******************************************************************************
* ��������: taskmng_meter_file_read_from_pn
* ��������: ������Ϣ��Ŵ��������Ķ�ȡ�����
* �������: 
            infonum
           
* �������: gMeterFile
* �� �� ֵ:  0 �ɹ�

*******************************************************************************/
int meter_read_from_pn(DB_CLIENT clientid,uint16 infonum,OOP_METER_T *gMeterFile)
{
    int   ret;
    uint32 len=0;
    NOMAL_OAD_T NormalOad;
    NormalOad.logicId = 0;
    NormalOad.oad.value = 0x60000200;
    NormalOad.infoNum = infonum;

    ret = db_read_nomal(clientid,&NormalOad,sizeof(OOP_METER_T),(uint8*)gMeterFile,&len);

    if(ret !=0 ||len!=sizeof(OOP_METER_T))
        return -1 ;
    
    return 0;
}

/*******************************************************************************
* ��������: meter_info_sort
* ��������: ��������󣬰���pn��С����洢
* ���������
* ���������
* �ڲ�����
* �� �� ֵ: 
******************************************************************************/
void meter_info_sort()
{
    DESK_METER_LIST_T meterList_sort;
    int pn_index_cur = 0;
    int pn_index = 0;
    int num = 0;

    memset(&meterList_sort,0x00,sizeof(DESK_METER_LIST_T));
    for(num = 0; num < MAX_METER_NUM; num++)
    {
        for(pn_index = 0; pn_index < gDeskMeter.meterNum; pn_index++)
        {
            if(num == gDeskMeter.meterInfo[pn_index].pn)
            {
                memcpy(&meterList_sort.meterInfo[pn_index_cur],&gDeskMeter.meterInfo[pn_index],sizeof(DESK_METER_T));
                pn_index_cur++;
                break;
            }
        } 
    }

    memset(gDeskMeter.meterInfo,0x00,sizeof(DESK_METER_T)*MAX_METER_NUM);
    memcpy(gDeskMeter.meterInfo,meterList_sort.meterInfo,sizeof(DESK_METER_T)*MAX_METER_NUM);
}


/*******************************************************************************
* ��������: meter_info_init
* ��������: ������ʼ������
* ���������
* ���������
* �ڲ�����
* �� �� ֵ: 
******************************************************************************/
void meter_info_change(DB_CLIENT clientid,DESK_METER_LIST_T *meterList,uint16 pn)
{
    int ret;
    OOP_METER_T meterInfo;
    uint16 i;
    int index = -1;
    PTHREAD_LOCK_SAFE(&gDeskMeterlock);
    if(pn == 0xffff)
    {
        GUI_FMT_DEBUG("�������\n");
        gDeskMeter.meterNum = 0;
        memset(gDeskMeter.meterInfo,0x00,sizeof(gDeskMeter.meterInfo));
    }else
    {
        memset(&meterInfo,0x00,sizeof(meterInfo));
        ret = meter_read_from_pn(clientid,pn,&meterInfo);
        if(ret ==0 )
        {
            //�ܶ�����ʾ���������޸�
            for(i=0;i<meterList->meterNum;i++)
            {
                if(meterList->meterInfo[i].pn == pn)
                {
                    index = i;
                    break;
                }
            }
            
            if(index == -1)
            {
                GUI_FMT_DEBUG("�������pn %d port %08x prtl %d \n",meterInfo.nIndex,meterInfo.basic.port.value,meterInfo.basic.protocol);
                GUI_FMT_DEBUG("��ַ oldvxd %d vxd %d 0X%02x%02x%02x%02x%02x%02x \n",meterInfo.basic.tsa.oldvxd,meterInfo.basic.tsa.vxd,
                    meterInfo.basic.tsa.add[0],meterInfo.basic.tsa.add[1],meterInfo.basic.tsa.add[2],meterInfo.basic.tsa.add[3],
                    meterInfo.basic.tsa.add[4],meterInfo.basic.tsa.add[5]);
                //�����ı�
                if(meter_valid_check(&meterInfo) == TRUE)
                {
                    gDeskMeter.meterInfo[gDeskMeter.meterNum].pn = meterInfo.nIndex;
                    gDeskMeter.meterInfo[gDeskMeter.meterNum].tsa = meterInfo.basic.tsa;
                    gDeskMeter.meterInfo[gDeskMeter.meterNum].prtl = meterInfo.basic.protocol;
                    gDeskMeter.meterInfo[gDeskMeter.meterNum].port = meterInfo.basic.port;
                    gDeskMeter.meterInfo[gDeskMeter.meterNum].PT = meterInfo.extend.PT;
                    gDeskMeter.meterInfo[gDeskMeter.meterNum].CT = meterInfo.extend.CT;
                    gDeskMeter.meterNum++;
                }

            }else
            {
                GUI_FMT_DEBUG("���µ��pn %d port %08x prtl %d \n",meterInfo.nIndex,meterInfo.basic.port.value,meterInfo.basic.protocol);
                GUI_FMT_DEBUG("��ַ oldvxd %d vxd %d 0X%02x%02x%02x%02x%02x%02x \n",meterInfo.basic.tsa.oldvxd,meterInfo.basic.tsa.vxd,
                    meterInfo.basic.tsa.add[0],meterInfo.basic.tsa.add[1],meterInfo.basic.tsa.add[2],meterInfo.basic.tsa.add[3],
                    meterInfo.basic.tsa.add[4],meterInfo.basic.tsa.add[5]);
                //���µı�
                if(meter_valid_check(&meterInfo) == TRUE)
                {
                    gDeskMeter.meterInfo[index].pn = meterInfo.nIndex;
                    gDeskMeter.meterInfo[index].tsa = meterInfo.basic.tsa;
                    gDeskMeter.meterInfo[index].prtl = meterInfo.basic.protocol;
                    gDeskMeter.meterInfo[index].port = meterInfo.basic.port;
                    gDeskMeter.meterInfo[index].PT = meterInfo.extend.PT;
                    gDeskMeter.meterInfo[index].CT = meterInfo.extend.CT;
                }
            }
        }else
        {
            //û������ʾɾ��
            for(i=0;i<meterList->meterNum;i++)
            {
                if(meterList->meterInfo[i].pn == pn)
                {
                    index = i;
                    break;
                }
            }
            if(index != -1)
            {
                GUI_FMT_DEBUG("ɾ�����pn %d  \n",pn);
                for(i = index ;i<gDeskMeter.meterNum-1;i++)
                {
                    gDeskMeter.meterInfo[i] = gDeskMeter.meterInfo[i+1];
                }
                memset(&gDeskMeter.meterInfo[gDeskMeter.meterNum-1],0x00,sizeof(DESK_METER_T));
                gDeskMeter.meterNum--;
            }
            
        }

    }

    //���մ�С�����һ��
    meter_info_sort();
    PTHREAD_UNLOCK_SAFE(&gDeskMeterlock);
}


/*******************************************************************************
* ��������: meter_thread
* ��������: ���������߳�
* ���������
* ���������
* �ڲ�����
* �� �� ֵ: 
*******************************************************************************/
void* meter_thread(void *arg)
{
    METER_THREAD_ARG_T meterArg;
    int len = 0;
    int ret;
    DB_UPDATE_T updateinfo;
    memset(&updateinfo,0x00,sizeof(DB_UPDATE_T));
    memset(&meterArg,0x00,sizeof(meterArg));
    meterArg.clientid = db_client_create(DB_GUI_AGENT, DB_CLIENT_GUI_METER);
    if(meterArg.clientid <= 0)
    {
        GUI_FMT_DEBUG("db_client_create DB_CLIENT_GUI_METER failed!\n");
        exit(1) ;
    }

    while(1)
    {
        len = meter_update_queue_read(&updateinfo);
        if(len>0)
        {
            GUI_FMT_DEBUG("�յ������ı����Ϣ pn %d \n",updateinfo.infonum);
            gDeskMeter.meterStatus = DESK_METER_CHANGE;
        }
        switch(gDeskMeter.meterStatus)
        {
            case DESK_METER_INIT:
            {
                ret = meter_info_init(meterArg.clientid,&gDeskMeter);
                if(ret != 0)
                {   
                    GUI_FMT_DEBUG("������ʼ����ʱ 5�������\n");
                    sleep(5);
                    break;
                }
                gDeskMeter.meterStatus = DESK_METER_OK;
            }
                break;
            case DESK_METER_CHANGE:
            {
                meter_info_change(meterArg.clientid,&gDeskMeter,updateinfo.infonum);
                gDeskMeter.meterStatus = DESK_METER_OK;
            }
                break;
            case DESK_METER_OK:
            {
            
            }
                break;
            default:
                gDeskMeter.meterStatus = DESK_METER_INIT;
                break;
        }
        usleep(200000);
    }
}

/*******************************************************************************
* ��������: DBUpdateFun 
* ��������: //�������������ϱ������ݸ����¼�
* �������: ptMsg ��Ϣ����
* �������: 
* �� �� ֵ: 
*******************************************************************************/
void mqtt_db_update_fun(MESSAGE_INFO_T *ptMsg)
{
    int ret;
    uint8 payload[2000] = {0};
    uint16 infonum;
    OOP_OAD_U oad;
    OOP_OAD_U MasterDI;
    uint16 index=0;
    uint16 classtag;
    DB_UPDATE_T dbupdate;
    memset(&dbupdate,0x00,sizeof(DB_UPDATE_T));
    memset(&MasterDI,0x00,sizeof(OOP_OAD_U));
    memcpy_s(payload,sizeof(payload),ptMsg->playload,ptMsg->msglen);

    index++;
    memcpy_r((uint8*)&infonum,&payload[index],2);
    index+=2;
    memcpy_r((uint8*)&classtag,&payload[index],2);
    index+=2;
    //�������ʶ optional
    if(payload[index++]==0)
        return ;
    //���ĵĵ��������������¼� û����di ֻ��һ��di

    if(payload[index++]!=0)
        return ;

    if(payload[index++]!=1)
        return ;
    memcpy_r((uint8*)&oad,&payload[index],4);


    dbupdate.infonum = infonum;
    dbupdate.oad = oad;

    switch(oad.value)
    {
    case 0x60000200:   //��������
    {
        ret = meter_update_queue_write(&dbupdate);
        GUI_FMT_DEBUG("meter_update_queue_write  ret %d  \n",ret);
    }
        break;
    
    default:
        break;
    }
   
}


/*******************************************************************************
* ��������: meter_info_read_from_pn 
* ��������: ������Ϣ��Ų�ѯ������Ϣ
* �������: pn ������
* �������: meterInfo �����ѯ�������Ļ�
* �� �� ֵ: 0 ��ѯ������ -1 δ��ѯ������
*******************************************************************************/
int meter_info_read_from_pn(uint16 pn,DESK_METER_T *meterInfo)
{
    uint16 i;
    int ret = -1;
    PTHREAD_LOCK_SAFE(&gDeskMeterlock);
    for(i=0;i<gDeskMeter.meterNum;i++)
    {
        if(pn == gDeskMeter.meterInfo[i].pn)
        {
            ret = 0;
            memcpy_s(meterInfo,sizeof(DESK_METER_T),&gDeskMeter.meterInfo[i],sizeof(DESK_METER_T));
            break;
        }
    }
    PTHREAD_UNLOCK_SAFE(&gDeskMeterlock);
    return ret;
}

/*******************************************************************************
* ��������: hplc_info_read_from_meter
* ��������: ���ݵ�����Ϣ��ѯHPLC�����ģ
* �� �� ֵ: �����ģ
*******************************************************************************/
uint16 hplc_info_read_from_meter()
{
    uint16 i;
    uint16 sum = 0;
    PTHREAD_LOCK_SAFE(&gDeskMeterlock);
    for(i=0;i<gDeskMeter.meterNum;i++)
    {
        if(0xF2090201 == gDeskMeter.meterInfo[i].port.value)
        {
            sum++;
        }
    }
    PTHREAD_UNLOCK_SAFE(&gDeskMeterlock);
    return sum;
}

/*******************************************************************************
* ��������: meter_info_read_all 
* ��������: ������Ϣ��Ų�ѯ������Ϣ
* �������: pn ������
* �������: meterInfo �����ѯ�������Ļ�
* �� �� ֵ: 0 ��ѯ������ -1 δ��ѯ������
*******************************************************************************/
int meter_info_read_all(DESK_METER_LIST_T *meterList)
{
    int ret = 0;
    PTHREAD_LOCK_SAFE(&gDeskMeterlock);
    memcpy(meterList,&gDeskMeter,sizeof(DESK_METER_LIST_T));
    PTHREAD_UNLOCK_SAFE(&gDeskMeterlock);
    return ret;
}

