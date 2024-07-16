/*
显示界面增加对档案的处理线程，负责档案的加载，以免实时查询档案的时候会卡
*/

#include "commfun.h"
#include "db_api.h"
#include "message.h"
#include "framecomm.h"
#include "task_id.h"
#include "desk_meter.h"


DESK_METER_LIST_T gDeskMeter;   //加载所有档案信息
pthread_mutex_t gDeskMeterlock;

DBUPDATE_QUEUE_T gUpdateQueue;  //档案变更用的队列
/*******************************************************************************
* 函数名称: meter_thread_param_init
* 函数功能: 全局变量初始化
* 输入参数：
* 输出参数：
* 内部处理：
* 返 回 值: 
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
* 函数名称: meter_valid_check
* 函数功能: 检查档案是否合法
* 输入参数：meterinfo 档案
* 输出参数：
* 内部处理：
* 返 回 值: TRUE 合法 FALSE 非法档案
*****************************************************************************/
BOOL meter_valid_check(OOP_METER_T *meterinfo)
{
    uint8 tmp[16];
    do
    {
        //判断下协议类型是否正常
        if(meterinfo->basic.protocol != METER_PRTL_97 && meterinfo->basic.protocol != METER_PRTL_07 &&
            meterinfo->basic.protocol != METER_PRTL_OOP)
        {
            GUI_FMT_DEBUG("pn %d 规约类型 %d 不对  !\n",meterinfo->nIndex,meterinfo->basic.protocol);
            break;
        }
        //判断地址是否合法
        memset(tmp,0x00,sizeof(tmp));
        if(memcmp(meterinfo->basic.tsa.add,tmp,meterinfo->basic.tsa.len+1) == 0)
        {
            GUI_FMT_DEBUG("pn %d 地址全0\n",meterinfo->nIndex);
            break;
        }
        memset(tmp,0xff,sizeof(tmp));
        if(memcmp(meterinfo->basic.tsa.add,tmp,meterinfo->basic.tsa.len+1) == 0)
        {
            GUI_FMT_DEBUG("pn %d 地址全ff\n",meterinfo->nIndex);
            break;
        }
        return TRUE;
    }
    while(0);
    
    return FALSE;
    
}


/*******************************************************************************
* 函数名称: meter_info_init
* 函数功能: 档案初始化加载
* 输入参数：
* 输出参数：
* 内部处理：
* 返 回 值: 0表示初始化成功 -1表示初始化失败
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
    GUI_FMT_DEBUG("开始档案初始化!\n");
    
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
        GUI_FMT_DEBUG("开始读档案记录失败 ret  %d recNum %d\n",ret,recNum);
    }else
    {
        for(i=0;i<recNum;i++)
        {
            ret = db_read_record_step(clientid, handle, &stepReadRecord, &stepRecordOut);
            if(ret!=0)
            {
                GUI_FMT_DEBUG("单步读档案记录失败 ret  %d i %d \n",ret,i);
                continue;
            }
            if(stepRecordOut.recordnum!=1 || stepRecordOut.record[0].road.oadCols.oad[0].value!=0x60000200
                ||stepRecordOut.record[0].outDatalen!=sizeof(OOP_METER_T))
            {
                GUI_FMT_DEBUG("单步读取的档案记录有问题 \n");
                continue;
            }
            memcpy_s(&meterInfo,sizeof(meterInfo),stepRecordOut.record[0].outDataBuf,sizeof(OOP_METER_T));
            if(meter_valid_check(&meterInfo)==FALSE)
            {
                GUI_FMT_DEBUG("PN %d 档案非法 \n",meterInfo.nIndex);
                continue;
            }
            GUI_FMT_DEBUG("电表pn %d port %08x prtl %d \n",meterInfo.nIndex,meterInfo.basic.port.value,meterInfo.basic.protocol);
            GUI_FMT_DEBUG("地址 oldvxd %d vxd %d 0X%02x%02x%02x%02x%02x%02x \n",meterInfo.basic.tsa.oldvxd,meterInfo.basic.tsa.vxd,
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
    GUI_FMT_DEBUG( "初始化后档案个数为 %d \n",num);
    meterList->meterNum = num;
    PTHREAD_UNLOCK_SAFE(&gDeskMeterlock);
    //ret不等于0表示 可能是读数据中心超时了   刚安装启动的时候界面比数据中心起的快
    if(ret !=0 )
        return -1;
    else 
        return 0;
}

/*******************************************************************************
* 函数名称: taskmng_db_update_queue_read
* 函数功能: 读队列
* 输入参数：pLockQueue          锁缓冲区结构指针
*           buf                 读数据缓冲区，将队列中的数据取出放入该缓冲区
* 输出参数：plockqueue          队列中的数据
* 内部处理: 判队列中是否有数据，有则从队列中取出数据放入buf中，每执行一次，取一个数据
* 返 回 值: len                 读到的数据长度
*******************************************************************************/
int meter_update_queue_read(DB_UPDATE_T *buf)
{
    int len = 0;

    //锁缓冲区
    PTHREAD_LOCK_SAFE(&gUpdateQueue.QueueLock);  
  
    //若队列中有数据，则取到buf中，解开锁
    if ((gUpdateQueue.QueueHead != gUpdateQueue.QueueTail)
        || ((gUpdateQueue.QueueHead == gUpdateQueue.QueueTail)
            && (gUpdateQueue.Queue[gUpdateQueue.QueueHead].isvalid != 0)))
    {   
        //pdata_buf =&(pLockQueue->Queue[pLockQueue->QueueHead].dbupdate);  
        len = sizeof(DB_UPDATE_T);
        
        //取队列中的数据
        memcpy(buf, &(gUpdateQueue.Queue[gUpdateQueue.QueueHead].dbupdate), len); 

        //清数据
        memset(&(gUpdateQueue.Queue[gUpdateQueue.QueueHead].dbupdate), 0, len);   
        gUpdateQueue.Queue[gUpdateQueue.QueueHead].isvalid = 0;

        //队列头向后移动一位
        gUpdateQueue.QueueHead= (gUpdateQueue.QueueHead + 1) % (MAX_DBUPDATE_QUEUE_CNT); 
    }
        
    PTHREAD_UNLOCK_SAFE(&gUpdateQueue.QueueLock);
    
    return len;
}

/*******************************************************************************
* 函数名称: taskmng_db_update_queue_write
* 函数功能: 写队列
* 输入参数：plockqueue          锁缓冲区结构指针
            buf                 需要写入队列的数据
            buf_len             数据长度
* 输出参数：无
* 内部处理: 若队列不满，将buf中的数据放入队列中；若队列满，丢弃该数据
* 返 回 值: >0                  成功
            =0                  失败
*******************************************************************************/
int meter_update_queue_write(DB_UPDATE_T *buf)
{
    int flags = 0;
        
    //锁缓冲区
    PTHREAD_LOCK_SAFE(&gUpdateQueue.QueueLock);  

    //判队列中是否还有空的位置
    if (gUpdateQueue.Queue[gUpdateQueue.QueueTail].isvalid == 0)
    {
        gUpdateQueue.Queue[gUpdateQueue.QueueTail].isvalid = 1;    
        memcpy(&(gUpdateQueue.Queue[gUpdateQueue.QueueTail].dbupdate), buf, sizeof(DB_UPDATE_T));
        flags = 1;
        gUpdateQueue.QueueTail = (gUpdateQueue.QueueTail + 1) % (MAX_DBUPDATE_QUEUE_CNT);
    }   
    
    //解缓冲区
    PTHREAD_UNLOCK_SAFE(&gUpdateQueue.QueueLock);

    return flags;
}
/*******************************************************************************
* 函数名称: taskmng_meter_file_read_from_pn
* 函数功能: 根据信息点号从数据中心读取电表档案
* 输入参数: 
            infonum
           
* 输出参数: gMeterFile
* 返 回 值:  0 成功

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
* 函数名称: meter_info_sort
* 函数功能: 档案变更后，按照pn从小到大存储
* 输入参数：
* 输出参数：
* 内部处理：
* 返 回 值: 
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
* 函数名称: meter_info_init
* 函数功能: 档案初始化加载
* 输入参数：
* 输出参数：
* 内部处理：
* 返 回 值: 
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
        GUI_FMT_DEBUG("档案清空\n");
        gDeskMeter.meterNum = 0;
        memset(gDeskMeter.meterInfo,0x00,sizeof(gDeskMeter.meterInfo));
    }else
    {
        memset(&meterInfo,0x00,sizeof(meterInfo));
        ret = meter_read_from_pn(clientid,pn,&meterInfo);
        if(ret ==0 )
        {
            //能读到表示新增或者修改
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
                GUI_FMT_DEBUG("新增电表pn %d port %08x prtl %d \n",meterInfo.nIndex,meterInfo.basic.port.value,meterInfo.basic.protocol);
                GUI_FMT_DEBUG("地址 oldvxd %d vxd %d 0X%02x%02x%02x%02x%02x%02x \n",meterInfo.basic.tsa.oldvxd,meterInfo.basic.tsa.vxd,
                    meterInfo.basic.tsa.add[0],meterInfo.basic.tsa.add[1],meterInfo.basic.tsa.add[2],meterInfo.basic.tsa.add[3],
                    meterInfo.basic.tsa.add[4],meterInfo.basic.tsa.add[5]);
                //新增的表
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
                GUI_FMT_DEBUG("更新电表pn %d port %08x prtl %d \n",meterInfo.nIndex,meterInfo.basic.port.value,meterInfo.basic.protocol);
                GUI_FMT_DEBUG("地址 oldvxd %d vxd %d 0X%02x%02x%02x%02x%02x%02x \n",meterInfo.basic.tsa.oldvxd,meterInfo.basic.tsa.vxd,
                    meterInfo.basic.tsa.add[0],meterInfo.basic.tsa.add[1],meterInfo.basic.tsa.add[2],meterInfo.basic.tsa.add[3],
                    meterInfo.basic.tsa.add[4],meterInfo.basic.tsa.add[5]);
                //更新的表
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
            //没读到表示删除
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
                GUI_FMT_DEBUG("删除电表pn %d  \n",pn);
                for(i = index ;i<gDeskMeter.meterNum-1;i++)
                {
                    gDeskMeter.meterInfo[i] = gDeskMeter.meterInfo[i+1];
                }
                memset(&gDeskMeter.meterInfo[gDeskMeter.meterNum-1],0x00,sizeof(DESK_METER_T));
                gDeskMeter.meterNum--;
            }
            
        }

    }

    //按照从小到大存一下
    meter_info_sort();
    PTHREAD_UNLOCK_SAFE(&gDeskMeterlock);
}


/*******************************************************************************
* 函数名称: meter_thread
* 函数功能: 档案管理线程
* 输入参数：
* 输出参数：
* 内部处理：
* 返 回 值: 
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
            GUI_FMT_DEBUG("收到档案的变更信息 pn %d \n",updateinfo.infonum);
            gDeskMeter.meterStatus = DESK_METER_CHANGE;
        }
        switch(gDeskMeter.meterStatus)
        {
            case DESK_METER_INIT:
            {
                ret = meter_info_init(meterArg.clientid,&gDeskMeter);
                if(ret != 0)
                {   
                    GUI_FMT_DEBUG("档案初始化超时 5秒后重试\n");
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
* 函数名称: DBUpdateFun 
* 函数功能: //处理数据中心上报的数据更新事件
* 输入参数: ptMsg 消息内容
* 输出参数: 
* 返 回 值: 
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
    //数据项标识 optional
    if(payload[index++]==0)
        return ;
    //关心的档案方案任务变更事件 没有主di 只有一个di

    if(payload[index++]!=0)
        return ;

    if(payload[index++]!=1)
        return ;
    memcpy_r((uint8*)&oad,&payload[index],4);


    dbupdate.infonum = infonum;
    dbupdate.oad = oad;

    switch(oad.value)
    {
    case 0x60000200:   //档案更新
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
* 函数名称: meter_info_read_from_pn 
* 函数功能: 根据信息点号查询档案信息
* 输入参数: pn 档案号
* 输出参数: meterInfo 如果查询到档案的话
* 返 回 值: 0 查询到档案 -1 未查询到档案
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
* 函数名称: hplc_info_read_from_meter
* 函数功能: 根据档案信息查询HPLC网络规模
* 返 回 值: 网络规模
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
* 函数名称: meter_info_read_all 
* 函数功能: 根据信息点号查询档案信息
* 输入参数: pn 档案号
* 输出参数: meterInfo 如果查询到档案的话
* 返 回 值: 0 查询到档案 -1 未查询到档案
*******************************************************************************/
int meter_info_read_all(DESK_METER_LIST_T *meterList)
{
    int ret = 0;
    PTHREAD_LOCK_SAFE(&gDeskMeterlock);
    memcpy(meterList,&gDeskMeter,sizeof(DESK_METER_LIST_T));
    PTHREAD_UNLOCK_SAFE(&gDeskMeterlock);
    return ret;
}

