/*
*********************************************************************
* Copyright(C) 2020 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   回路监控mqtt
* @date：    2020-6-30
* @history： 
*********************************************************************
*/
#include "baseCommon.h"
#include "framecomm.h"
#include "task_id.h"
#include "task_msg.h"

#include "db_api.h"
#include "appmsg_split.h"

#include "mapmsg.h"
#include "mapmsg_split.h"
#include "mqtt_client.h"
#include "iot_mqtt.h"
#include "iot_init.h"
#include "iot_inspect.h"
#include "iot_update.h"

pthread_mutex_t g_IotIndexlock;

uint8 inspect_thread_type = 0;
uint16 g_IotMsgindex = 0;
int iot_data_inspect_proc(void *pArg,void *pPublic, void *pPrivate);
int iot_data_inspect_init(void *pPublic, void **ppPrivate);
int getshellresult(const char* cmd,char result[][500],int size);

uint8 get_appnum(char* buf);
char * strim(char *str);
uint16 percentdatachange(char* buf);
OOP_DATETIME_S_T timeformat_s(char* time);
extern ALL_CONTAINER_AND_APP_T allcontainerinfo; 
extern int32 taskmng_plc_queue_read(LockQueue_T *pLockQueue, uint8 *buf);
extern int32 taskmng_plc_queue_write(LockQueue_T *pLockQueue, uint8 *buf, int32 buf_len);
extern uint8 set_len_offset(uint16 eleLen, uint8 *indata);
extern void app_action_notice(SEQ_STR *docker,SEQ_STR *app,uint8 type,uint8 errcode);


uint16 iot_get_next_index()
{
    uint16 curindex = 0;
    
    PTHREAD_LOCK_SAFE((void *)&g_IotIndexlock);
    curindex = g_IotMsgindex;
    g_IotMsgindex++;
    PTHREAD_UNLOCK_SAFE((void *)&g_IotIndexlock);

    return curindex;
}
/**********************************************************************
* @name      : set_len_offset
* @brief     ：设置元素长度
* @param[in] ：eleLen      输入数据长度
* @param[out]：indata      数据Buffer
* @return    ：数据Buffer长度
* @Create    : 
* @Date      ：2019-10-30
* @Update    :
**********************************************************************/
uint8 set_len_offset(uint16 eleLen, uint8 *indata)
{
    if(eleLen < 0x80)
    {
        indata[0] = eleLen;
        return 1;
    }
    else if(eleLen <= 0xff)
    {
        indata[0] = 0x81;
        indata[1] = eleLen;
        return 2;
    }
    else if(eleLen <= 0xffff)
    {
        indata[0] = 0x82;
        indata[1] = (uint8)((eleLen >> 8) & 0xff);
        indata[2] = (uint8)(eleLen & 0xff);
        return 3;
    }
        
    return 0;
}


/*******************************************************************************
* 函数名称: taskmng_plc_queue_write
* 函数功能: 写队列
* 输入参数：plockqueue          锁缓冲区结构指针
            buf                 需要写入队列的数据
            buf_len             数据长度
* 输出参数：无
* 内部处理: 若队列不满，将buf中的数据放入队列中；若队列满，丢弃该数据
* 返 回 值: >0                  成功
            =0                  失败
*******************************************************************************/
int32 taskmng_plc_queue_write(LockQueue_T *pLockQueue, uint8 *buf, int32 buf_len)
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

/*******************************************************************************
* 函数名称: taskmng_plc_queue_read
* 函数功能: 读队列
* 输入参数：pLockQueue          锁缓冲区结构指针
*           buf                 读数据缓冲区，将队列中的数据取出放入该缓冲区
* 输出参数：plockqueue          队列中的数据
* 内部处理: 判队列中是否有数据，有则从队列中取出数据放入buf中，每执行一次，取一个数据
* 返 回 值: len                 读到的数据长度
*******************************************************************************/
int32 taskmng_plc_queue_read(LockQueue_T *pLockQueue, uint8 *buf)
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


/**********************************************************************
* @name      : get_appinfo
* @brief     : 获取本周期的限值
* @param[in] : apps      数据更新线程获得的数据   
* @param[out]: pApp     
* @return    ：数据Buffer长度
* @Create    : 
* @Date      ：2019-10-30
* @Update    :
**********************************************************************/

int get_appinfo(iot_docker_info *pApp,ALL_APP_INFO_T *apps)
{
    uint8 i=0;
    iot_docker_info * ptmpApp = pApp;
    ptmpApp->appNum = apps->appcount;

    for(i=0;i<ptmpApp->appNum;i++)
    {
        strcpy((char *)ptmpApp->apprun[i].Appname,(char *)apps->appinfo[i].appname);
        ptmpApp->apprun[i].mem_limit = apps->appinfo[i].memLimit;
        ptmpApp->apprun[i].cpu_limit = apps->appinfo[i].cpuLimit;
        ptmpApp->apprun[i].mem_usage = apps->appinfo[i].serviceinfo[0].memrate;
        ptmpApp->apprun[i].cpu_usage = apps->appinfo[i].serviceinfo[0].cpurate;
        time_t starttime;
        memset(&starttime,0,sizeof(starttime));
        DT_DateTimeShort2Time(&apps->appinfo[i].serviceinfo[0].starttime,&starttime);
        if(starttime==0)
        {
            ptmpApp->apprun[i].lasttime = 0;
            ptmpApp->apprun[i].thistime = 0;
        }
        else
        {
            if( ptmpApp->apprun[i].lasttime==0) //首次上电
            {
                ptmpApp->apprun[i].lasttime = starttime;
                ptmpApp->apprun[i].thistime = starttime;
            }
            else
            {
                ptmpApp->apprun[i].lasttime = ptmpApp->apprun[i].thistime;
                ptmpApp->apprun[i].thistime = starttime;
            }
         //   ptmpApp->apprun[i].runtime = time(NULL)-starttime;
        }
        ptmpApp->apprun[i].status = apps->appinfo[i].serviceinfo[0].servicestate;
        ptmpApp->apprun[i].enable = apps->appinfo[i].serviceinfo[0].serviceenable;
    //    MQTTMSG_FMT_DEBUG("Appname %s cpulimit :%d cpurate:%d  memlimit %d memrate%d\n",ptmpApp->apprun[i].Appname,ptmpApp->apprun[i].cpu_limit,ptmpApp->apprun[i].cpu_usage,ptmpApp->apprun[i].mem_limit,ptmpApp->apprun[i].mem_usage);
    }
    return 1;
}

/**********************************************************************
* @name      : get_all_infolimit
* @brief     : 获取本周期容器的限值
* @param[in] : thisdata      数据更新线程获得的数据   
* @param[out]: ptData         运行结构体 
* @return    ：数据Buffer长度
* @Create    : 
* @Date      ：2019-10-30
* @Update    :
**********************************************************************/

int get_all_infolimit(IOT_INSPECT_T *ptData,ALL_CONTAINER_AND_APP_T *thisdata)
{
    /*容器阈值在全局变量里有，app的阈值自己组命令取*/
    uint8 dockernum=0,i=0;
    int ret = 0;
    ptData->dockernum = thisdata->count;
    dockernum = ptData->dockernum;
    for(i=0;i<dockernum;i++)
    {
       iot_docker_info *tmplimit = &ptData->docker_runinfo[i];
       memcpy(&tmplimit->runlimit.cpu_limit,&thisdata->containers[i].container.cpuLimit,sizeof(uint16));
       memcpy(&tmplimit->runlimit.mem_limit,&thisdata->containers[i].container.memLimit,sizeof(uint16));
       memcpy(&tmplimit->runlimit.disk_limit,&thisdata->containers[i].container.stroLimit,sizeof(uint16));
       strcpy((char *)tmplimit->name,(char *)thisdata->containers[i].container.ContainerName);
       tmplimit->cpu_usage = thisdata->containers[i].container.CpuRate;
       tmplimit->mem_usage = thisdata->containers[i].container.MenRate;
       tmplimit->disk_usage =  thisdata->containers[i].container.StorageRate;
       if(tmplimit->Lastruntime==0) //首次
       {
        tmplimit->Lastruntime =thisdata->containers[i].container.RunTime;
        tmplimit->runtime = thisdata->containers[i].container.RunTime;
       }
       else
       {
        tmplimit->Lastruntime=tmplimit->runtime;
        tmplimit->runtime = thisdata->containers[i].container.RunTime;
       }
       
       if(strcmp((char *)thisdata->containers[i].container.ContainerState,"running")==0)
       {
        tmplimit->status = 0;
       }
       else
       {
        tmplimit->status = 1;
       }
       ret = get_appinfo(tmplimit,&thisdata->containers[i].apps);
       if(ret < 0)
       {
            
       }
    }
    /*获取终端阈值*/
	/*
    uint8 j=0;
    for(i=0;i<dockernum;i++)
    {
        iot_docker_info *appinfo =&ptData->docker_runinfo[i];
        IOT_FMT_DEBUG("获取容器限制成功\n");
        IOT_FMT_DEBUG("name %s cpulimit %d memlimit %d disklimit %d status %d\n",appinfo->name,appinfo->runlimit.cpu_limit,appinfo->runlimit.mem_limit,appinfo->runlimit.disk_limit,appinfo->status);
        IOT_FMT_DEBUG("name %s cpuuse %d memuset %d diskuse %d\n",appinfo->name,appinfo->cpu_usage,appinfo->mem_usage,appinfo->disk_usage);
        for(j=0;j<appinfo->appNum;j++)
        {
            IOT_FMT_DEBUG("appname %s memlimit %d cpulimit %d status %d enable %d\n",appinfo->apprun[j].Appname,appinfo->apprun[j].mem_limit,appinfo->apprun[j].cpu_limit,appinfo->apprun[j].status,appinfo->apprun[j].enable);
        }

    }  */
    return 1;
}

/**********************************************************************
* @name      : iot_app_alarm_notifi
* @brief     : App告警组报文
* @param[in] : uint8 *dockername,uint8 *appname,uint8 alarmtype,uint8 notiftype  相关参数  
* @param[out]: ptData         运行结构体 
* @return    ：数据Buffer长度
* @Create    : 
* @Date      ：2019-10-30
* @Update    :
**********************************************************************/

void iot_app_alarm_notifi(uint8 *appname,uint8 *dockername,uint8 alarmtype,uint8 notiftype)
{
    if(dockername==NULL || appname==NULL )
        return;
    IOT_FMT_DEBUG("产生APP告警事件\n");
    IOT_FMT_DEBUG("容器 ：%s 应用名称: %s 告警类别：%d 通知类别：%d\n",dockername,appname,alarmtype,notiftype);
    uint16 offset=0;


    uint8 buff[2048]={0};
    OOP_DATETIME_S_T StartTime;
    memset(&StartTime,0x00,sizeof(OOP_DATETIME_S_T));
    buff[offset++] = strlen((char *)dockername);
    memcpy(&buff[offset],dockername,strlen((char *)dockername));
    offset+=strlen((char *)dockername);
    buff[offset++] = strlen((char *)appname);
    memcpy(&buff[offset],appname,strlen((char *)appname));
    offset+=strlen((char *)appname);
    DT_Time2DateTimeShort(time(NULL),&StartTime);
    memcpy_r(&buff[offset],&StartTime.year,sizeof(uint16));
    offset+=sizeof(uint16);
    buff[offset++]=StartTime.month;
    buff[offset++]=StartTime.day;
    buff[offset++]=StartTime.hour;
    buff[offset++]=StartTime.minute;
    buff[offset++]=StartTime.second;
    buff[offset++]=alarmtype;
    buff[offset++]=notiftype;
    buff[offset++]=00;  //  告警描述暂时为0;
    MSG_INFO_T* msg  = malloc(sizeof(MSG_INFO_T)+offset);    
    if(msg==NULL)
        return;
    memset(msg,0,sizeof(MSG_INFO_T)+offset);
    msg->MsgRPM = 1;
    msg->MsgPRIORITY = 1;
    msg->MsgIndex = iot_get_next_index();
    msg->MsgLabel = 0;
    
    msg->SourLen = strlen(IOT_APP_NAME_M);
    memcpy(msg->SourAddr, IOT_APP_NAME_M, msg->SourLen);

    msg->DestLen = strlen("Broadcast");
    strcpy(msg->DestAddr, "Broadcast");
    msg->IID = MESSAGE_IID_IOT;
    msg->IOP = IOT_IOP_APP_WARRING;
    msg->MsgLen = offset;
    memcpy(msg->MsgData,buff,offset);
    taskmng_plc_queue_write(&LockWarringMsg,(uint8*)msg, msg->MsgLen+sizeof(MSG_INFO_T));
    free(msg);
}


/**********************************************************************
* @name      : iot_appruninfo_judge
* @brief     : App告警判断函数
* @param[in] : ptData iot线程的结构体  pData iot结构体 每里面是检查后更新的用来判断容器和app是否是重启还是新安装
* @param[out]: ptData         运行结构体 
* @return    ：
* @Create    : 
* @Date      ：2019-10-30
* @Update    :
**********************************************************************/

int iot_appruninfo_judge(iot_docker_info *ptData,iot_inspec_conList *pData,IOT_APPDATA_T * pbdata)
{
    iot_app_info *LimitData = NULL;
    iot_inspec_conList *inspec=NULL;
    uint8 i=0,appnum=0;
    appnum = ptData->appNum;
    if(appnum == 0)
        return -1;
    inspec = pData;
    for(i=0;i<appnum;i++)
    {
        LimitData = &ptData->apprun[i];                         //直接返回不会判断他的其他异常，因为现在的数据时不准确的
        if(LimitData->Appname == NULL || strlen((char*)LimitData->Appname)==0)
        {
            continue;
        }
        if(LimitData->enable == 0)    //如果app出现异常状态 
        
        {
            //todo容器没有执行  异常
            IOT_FMT_DEBUG("容器 %s APP %s enble %d\n",ptData->name,LimitData->Appname,LimitData->enable);
            iot_app_alarm_notifi(LimitData->Appname,ptData->name,0,3);
            continue;
        }
        if(LimitData->cputype==1)  //已经越线
        {
            if(LimitData->cpu_usage < LimitData->cpu_limit)
            {
                //上报恢复事件
                IOT_FMT_DEBUG("容器 %s APP %s CPU越限恢复事件 cpurate %d  CPUlimit %d\n",ptData->name,LimitData->Appname,LimitData->cpu_usage,LimitData->cpu_limit);
                iot_app_alarm_notifi(LimitData->Appname,ptData->name,2,0);
                LimitData->cputype = 0;
            }
        }
        else
        {
            if(LimitData->cpu_usage >= LimitData->cpu_limit)
            {
                //上报越限事件
                LimitData->cputype = 1;
                IOT_FMT_DEBUG("容器 %s APP %s CPU越限发生事件 cpurate %d  CPUlimit %d\n",ptData->name,LimitData->Appname,LimitData->cpu_usage,LimitData->cpu_limit);
                iot_app_alarm_notifi(LimitData->Appname,ptData->name,1,0);
            }
        }
        if(LimitData->memtype==1)  //已经越线
        {
            if(LimitData->mem_usage < LimitData->mem_limit)
            {
                //上报恢复事件
                IOT_FMT_DEBUG("容器 %s APP %s MEM越限恢复事件 memrarte %d  MEMlimit %d\n",ptData->name,LimitData->Appname,LimitData->mem_usage,LimitData->mem_limit);
                iot_app_alarm_notifi(LimitData->Appname,ptData->name,2,1);
                LimitData->memtype = 0;
                
            }
        }
        else
        {
            if(LimitData->mem_usage >= LimitData->mem_limit) //过台体修改
            {
                //上报越限事件
                IOT_FMT_DEBUG("容器 %s APP %s MEM越限发生事件 memrate %d  MEMlimit %d\n",ptData->name,LimitData->Appname,LimitData->mem_usage,LimitData->mem_limit);
                iot_app_alarm_notifi(LimitData->Appname,ptData->name,1,1);
                LimitData->memtype = 1;               
            }
        }

        
        //if((time(NULL) - LimitData->runtime)  <= 60 && LimitData->runtime !=0)
        //todo运行时间突然变化可能是重启
        //IOT_FMT_DEBUG("thistime %d lastTime %d \n",LimitData->thistime,LimitData->lasttime);
        if(LimitData->thistime != LimitData->lasttime  && LimitData->lasttime !=0 && LimitData->thistime !=0)        //可能存在重启周期
        {
            //检查在不在更新的容器和app列表
            if(inspec->conNum!=0)
            {
                if(pbdata->app_u.type_u == 1)           //检测到存在升级消息
                {
                    if(pbdata->app_u.appname.len != 0 && pbdata->app_u.conname.len != 0)
                    {
                        IOT_FMT_DEBUG("App %s 存在升级行为 延时10s后上报升级消息\n",LimitData->Appname);
                        sleep(10);
                        app_action_notice(&pbdata->app_u.conname,&pbdata->app_u.appname,1,pbdata->app_u.result); //升级消息通知
                        memset(&pbdata->app_u,0,sizeof(pbdata->app_u));
                        pbdata->app_u.type_u = 0;
                        continue;
                    }
                }

                uint8 i =0,j=0;
                for(i=0;i<inspec->conNum;i++)
                {
                    if(strcmp((char *)ptData->name,(char *)inspec->conList[i].conName)==0)
                    {
                        for(j=0;j<inspec->conList[i].appnum;j++)
                        {
                            if(strcmp((char *)LimitData->Appname,(char *)inspec->conList[i].app_t[j].conName)==0)
                            {
                                IOT_FMT_DEBUG("App %s 存在重启行为 上报重启事件 \n",LimitData->Appname);
                                iot_app_alarm_notifi(LimitData->Appname,ptData->name,0,2);
                                continue;
                            }
                        }
                        break;
                    }
                }
            }
        }
            
        
    }
    return 1;
}

/**********************************************************************
* @name      : iot_update_conList
* @brief     : conList更新
* @param[in] : pData iot结构体 每里面是检查后更新的用来判断容器和app是否是重启还是新安装
* @param[out]: ptData         运行结构体 
* @return    ：
* @Create    : 
* @Date      ：2019-10-30
* @Update    :
**********************************************************************/

void iot_update_conList(IOT_INSPECT_T * ptData)
{
    if(ptData==NULL)
        return;
    uint8 i,j;
    ptData->inspect_conlist.conNum = ptData->dockernum;
    for(i=0;i<ptData->dockernum;i++)
    {
        ptData->inspect_conlist.conList[i].conlen = strlen((char *)ptData->docker_runinfo[i].name);
        strcpy((char*)ptData->inspect_conlist.conList[i].conName,(char*)ptData->docker_runinfo[i].name);
        ptData->inspect_conlist.conList[i].appnum = ptData->docker_runinfo[i].appNum;
        for(j=0;j<ptData->docker_runinfo[i].appNum;j++)
        {
            ptData->inspect_conlist.conList[i].app_t[j].conlen = strlen((char *)ptData->docker_runinfo[i].apprun[j].Appname);
            strcpy((char *)ptData->inspect_conlist.conList[i].app_t[j].conName,(char *)ptData->docker_runinfo[i].apprun[j].Appname);
        }
    }
#if 0    
    for(i=0;i<ptData->inspect_conlist.conNum ;i++)
    {
        for(j=0;j<ptData->docker_runinfo[i].appNum;j++)
        {
            IOT_FMT_DEBUG("app name : %s\n",ptData->inspect_conlist.conList[i].app_t[j].conName);
        }
     //   IOT_FMT_DEBUG("\n");
    }
#endif
}

/**********************************************************************
* @name      : iot_con_alarm_notifi
* @brief     : 容器告警信息
* @param[in] : uint8 *dockername,uint8 alarmtype,uint8 notiftype 相关参数
* @param[out]: ptData         运行结构体 
* @return    ：
* @Create    : 
* @Date      ：2019-10-30
* @Update    :
**********************************************************************/
void iot_con_alarm_notifi(uint8 *dockername,uint8 alarmtype,uint8 notiftype)
{
    if(dockername==NULL)
        return;
    IOT_FMT_DEBUG("产生容器告警事件\n");
    IOT_FMT_DEBUG("容器 ：%s 告警类别：%d 通知类别：%d\n",(char*)dockername,alarmtype,notiftype);
    uint16 index=0;

    uint8 buff[2048]={0};
    OOP_DATETIME_S_T StartTime;
    memset(&StartTime,0x00,sizeof(OOP_DATETIME_S_T));
    buff[index++] = strlen((char *)dockername);
    memcpy(&buff[index],dockername,strlen((char *)dockername));
    index+=strlen((char *)dockername);
    DT_Time2DateTimeShort(time(NULL),&StartTime);
    memcpy_r(&buff[index],&StartTime.year,sizeof(uint16));
    index+=sizeof(uint16);
    buff[index++]=StartTime.month;
    buff[index++]=StartTime.day;
    buff[index++]=StartTime.hour;
    buff[index++]=StartTime.minute;
    buff[index++]=StartTime.second;
    buff[index++]=alarmtype;
    buff[index++]=notiftype;
    buff[index++]=00;  //  告警描述暂时为0;
    
    MSG_INFO_T* msg  = malloc(sizeof(MSG_INFO_T)+index);  
    if(msg==NULL)
        return;
    memset(msg,0,sizeof(MSG_INFO_T)+index);
    msg->MsgRPM = 1;
    msg->MsgPRIORITY = 1;
    msg->MsgIndex = iot_get_next_index();
    msg->MsgLabel = 0;

    msg->SourLen = strlen(IOT_APP_NAME_M);
    memcpy(msg->SourAddr, IOT_APP_NAME_M, msg->SourLen);

    msg->DestLen = strlen("Broadcast");
    strcpy(msg->DestAddr, "Broadcast");
    msg->IID = MESSAGE_IID_IOT;
    msg->IOP = IOT_IOP_DOCKER_WARRING;
    msg->MsgLen = index;
    memcpy(msg->MsgData,buff,index);

    taskmng_plc_queue_write(&LockWarringMsg,(uint8*)msg, msg->MsgLen+sizeof(MSG_INFO_T));
    free(msg);
}


/**********************************************************************
* @name      : iot_dev_alarm_notifi
* @brief     : 设备告警信息
* @param[in] : uint8 alarmtype,uint8 notiftype) 相关参数
* @param[out]:  
* @return    ：
* @Create    : 
* @Date      ：2019-10-30
* @Update    :
**********************************************************************/

void iot_dev_alarm_notifi(uint8 alarmtype,uint8 notiftype)
{
    IOT_FMT_DEBUG("产生系统告警事件\n");
    IOT_FMT_DEBUG("告警类别：%d 通知类别：%d\n",alarmtype,notiftype);
    uint16 index=0;

    uint8 buff[2048]={0};
    OOP_DATETIME_S_T StartTime;
    memset(&StartTime,0x00,sizeof(OOP_DATETIME_S_T));
    DT_Time2DateTimeShort(time(NULL),&StartTime);
    memcpy_r(&buff[index],&StartTime.year,sizeof(uint16));
    index+=sizeof(uint16);
    buff[index++]=StartTime.month;
    buff[index++]=StartTime.day;
    buff[index++]=StartTime.hour;
    buff[index++]=StartTime.minute;
    buff[index++]=StartTime.second;
    buff[index++]=alarmtype;
    buff[index++]=notiftype;
    buff[index++]=00;  //  告警描述暂时为0;
    MSG_INFO_T* msg  = malloc(sizeof(MSG_INFO_T)+index);
    if(msg==NULL)
        return;
    memset(msg,0,sizeof(MSG_INFO_T)+index);
    //char broad[50] = "Broadcast";
    msg->MsgRPM = 1;
    msg->MsgPRIORITY = 1;
    msg->MsgIndex = iot_get_next_index();
    msg->MsgLabel = 0;

    msg->SourLen = strlen(IOT_APP_NAME_M);
    memcpy(msg->SourAddr, IOT_APP_NAME_M, msg->SourLen);

    msg->DestLen = strlen("Broadcast");
    strcpy(msg->DestAddr, "Broadcast");
    //memcpy(msg->DestAddr, "Broadcast", msg->DestLen);
    msg->IID = MESSAGE_IID_IOT;
    msg->IOP = IOT_IOP_SYS_WARRING;
    msg->MsgLen = index;
    memcpy(msg->MsgData,buff,index);
    taskmng_plc_queue_write(&LockWarringMsg,(uint8*)msg, msg->MsgLen+sizeof(MSG_INFO_T));
    free(msg);
}

int exec_restart_container(uint8 *conname)
{
    return 2;
    if(conname == NULL)
        return -1;
    char cmd[100] = {0};
    char buff[5][500]= {{0}};
    sprintf(cmd,"container stop %s | grep success",conname);
    if(getshellresult(cmd,buff,5)!=1)
    {
        if(getshellresult(cmd,buff,5)!=1)
        {
            return -1;
        }
    }
    memset(cmd,0,sizeof(cmd));
    memset(buff,0,sizeof(buff));
    sprintf(cmd,"container start %s | grep success",conname);
    if(getshellresult(cmd,buff,5)!=1)
    {
        if(getshellresult(cmd,buff,5)!=1)
        {
            return -1;
        }
    }
    return 0;
}

/**********************************************************************
* @name      : judg_value_threshold
* @brief     : 容器告警判断
* @param[in] : ptData 线程运行结构体
* @param[out]:  
* @return    ：
* @Create    : 
* @Date      ：2019-10-30
* @Update    :
**********************************************************************/

int judg_value_threshold(IOT_INSPECT_T *ptData,IOT_APPDATA_T *pbdata)
{
  //  iot_docker_info *judge_pData = NULL;
    uint8 dockernum = ptData->dockernum;
    uint8 i=0;
    int ret = 0;
 //   uint8 j=0;
    /*
    for(i=0;i<dockernum;i++)
    {
        
        iot_docker_info *appinfo =&ptData->docker_runinfo[i];
        IOT_FMT_DEBUG("获取容器限制成功\n");
        IOT_FMT_DEBUG("name %s cpulimit %d memlimit %d disklimit %d status %d\n",appinfo->name,appinfo->runlimit.cpu_limit,appinfo->runlimit.mem_limit,appinfo->runlimit.disk_limit,appinfo->status);
        IOT_FMT_DEBUG("name %s cpuuse %d memuset %d diskuse %d\n",appinfo->name,appinfo->cpu_usage,appinfo->mem_usage,appinfo->disk_usage);
        for(j=0;j<appinfo->appNum;j++)
        {
            IOT_FMT_DEBUG("appname %s memlimit %d cpulimit %d status %d enable %d\n",appinfo->apprun[j].Appname,appinfo->apprun[j].mem_limit,appinfo->apprun[j].cpu_limit,appinfo->apprun[j].status,appinfo->apprun[j].enable);
        }

    } */
    for(i=0;i<dockernum;i++)
    {
       iot_docker_info * judge_pData = &ptData->docker_runinfo[i];
        /**/
        if(judge_pData->name==NULL || judge_pData->runtime ==0 || strlen((char*)judge_pData->name)==0)
            continue;
        if(judge_pData->cputype==1)  //已经越线
        {           
            if(judge_pData->cpu_usage < judge_pData->runlimit.cpu_limit && judge_pData->cpu_usage>0)
            {
                //上报恢复事件
                IOT_FMT_DEBUG("container %s cpu less than limit recover value %d ,limit %d\n",judge_pData->name,judge_pData->cpu_usage,judge_pData->runlimit.cpu_limit);
                judge_pData->cputype = 0;
                iot_con_alarm_notifi(judge_pData->name,2,0);   //cpu恢复
            }
        }
        else
        {
            if(judge_pData->cpu_usage >= judge_pData->runlimit.cpu_limit && judge_pData->cpu_usage!=0)
            {
                //上报越限事件
                IOT_FMT_DEBUG("container %s cpu more than limit value %d ,limit %d\n",judge_pData->name,judge_pData->cpu_usage,judge_pData->runlimit.cpu_limit);
                judge_pData->cputype=1;
                iot_con_alarm_notifi(judge_pData->name,1,0);
                /*执行重启*/
                ret = exec_restart_container(judge_pData->name);
                if(ret ==0)
                {
                    IOT_FMT_DEBUG("容器 %s cpu 越限 重启成功\n",judge_pData->name);
                }
                else if(ret == -1)
                {
                    IOT_FMT_DEBUG("容器 %s cpu 越限 重启失败\n",judge_pData->name);
                }else{
                    IOT_FMT_DEBUG("越限但不重启\n");
                }
            }
        }
        if(judge_pData->memtype==1)  //已经越线
        {
            if(judge_pData->mem_usage < judge_pData->runlimit.mem_limit && judge_pData->mem_usage>0)
            {
                //上报恢复事件
                IOT_FMT_DEBUG("container %s mem less than limit value %d ,limit %d\n",judge_pData->name,judge_pData->mem_usage,judge_pData->runlimit.mem_limit);
                judge_pData->memtype = 0;
                iot_con_alarm_notifi(judge_pData->name,0,1);
            }
        }
        else
        {
            if(judge_pData->mem_usage >= judge_pData->runlimit.mem_limit && judge_pData->mem_usage!=0)
            {
                //上报越限事件
                IOT_FMT_DEBUG("container %s mem more than limit value %d ,limit %d\n",judge_pData->name,judge_pData->mem_usage,judge_pData->runlimit.mem_limit);
                judge_pData->memtype = 1;
                iot_con_alarm_notifi(judge_pData->name,1,1);
                                /*执行重启*/
                ret = exec_restart_container(judge_pData->name);
                if(ret ==0)
                {
                    IOT_FMT_DEBUG("容器 %s mem 越限 重启成功\n",judge_pData->name);
                }
                else if(ret == -1)
                {
                    IOT_FMT_DEBUG("容器 %s mem 越限 重启失败\n",judge_pData->name);
                }else{
                    IOT_FMT_DEBUG("越限但不重启\n");
                }
            }
        }
        if(judge_pData->disktype==1)  //已经越线
        {
            IOT_FMT_DEBUG("disk 恢复\n");
            if(judge_pData->disk_usage < judge_pData->runlimit.disk_limit )
            {
                //上报恢复事件
                IOT_FMT_DEBUG("container %s disk less than limit value %d ,limit %d\n",judge_pData->name,judge_pData->disk_usage,judge_pData->runlimit.disk_limit);
                judge_pData->disktype = 0;
                iot_con_alarm_notifi(judge_pData->name,0,2);
            }
        }
        else
        {
            if(judge_pData->disk_usage >= judge_pData->runlimit.disk_limit && judge_pData->disk_usage!=0)
            {
                //上报越限事件       
                IOT_FMT_DEBUG("container %s disk more than limit value %d ,limit %d\n",judge_pData->name,judge_pData->disk_usage,judge_pData->runlimit.disk_limit);
                judge_pData->disktype = 1;
                iot_con_alarm_notifi(judge_pData->name,1,2);
                                /*执行重启*/
                ret = exec_restart_container(judge_pData->name);
                if(ret ==0)
                {
                    IOT_FMT_DEBUG("容器 %s disk 越限 重启成功\n",(char*)judge_pData->name);
                }
                else if(ret == -1)
                {
                    IOT_FMT_DEBUG("容器 %s disk 越限 重启失败\n",(char*)judge_pData->name);
                }else{
                    IOT_FMT_DEBUG("越限但不重启\n");
                }
            }
        }
  //      if(judge_pData->Lastruntime > judge_pData->runtime && judge_pData->runtime!=0)
  //      {
   //         IOT_FMT_DEBUG("container %s lasttime more than runtime last %d ,runtime %d\n",judge_pData->name,judge_pData->Lastruntime,judge_pData->runtime);
  //          iot_con_alarm_notifi(judge_pData->name,0,3);
  //      }
 /*       else
        {
            uint8 j;
            for(j=0;j<ptData->inspect_conlist.conNum;j++)
            {
                if(strcmp((char *)ptData->inspect_conlist.conList[j].conName,(char *)judge_pData->name)==0)
                {//在这个名单说名不是本周期新安装的
                    IOT_FMT_DEBUG("container %s reboot \n",judge_pData->name);
                    iot_con_alarm_notifi(judge_pData->name,0,3);
                }
            } 
        }*/
        /*
        if((time(NULL)-judge_pData->runtime)<montime)
        {
            //todo运行时间突然变化可能是重启
            uint8 j;
            for(j=0;j<ptData->inspect_conlist.conNum;j++)
            {
                if(strcmp((char *)ptData->inspect_conlist.conList[j].conName,(char *)judge_pData->name)==0)
                {//在这个名单说名不是本周期新安装的
                    IOT_FMT_DEBUG("container %s reboot \n",judge_pData->name);
                    iot_con_alarm_notifi(judge_pData->name,0,3);
                }
            }            
        }
        else
        {
            IOT_FMT_DEBUG("container %s reboot \n",judge_pData->name);
            iot_con_alarm_notifi(judge_pData->name,0,3);
        }*/
        if(judge_pData->status == 1)
        {
            //todo容器没有执行 或者APP数量为0
            IOT_FMT_DEBUG("container %s  unusual status \n",judge_pData->name,judge_pData->status);
            iot_con_alarm_notifi(judge_pData->name,0,4);
        }
        iot_appruninfo_judge(judge_pData,&ptData->inspect_conlist,pbdata);
    }
    //终端环境检测
    //更新 inspect_conList
    iot_update_conList(ptData);
    return 1;
    
}

/**********************************************************************
* @name      : iot_data_inspect_init
* @brief     : inspect线程初始化函数
* @param[in] : pPublic 公共结构 
* @param[out]: ppPrivate 线程结构体 
* @return    ：
* @Create    : 
* @Date      ：2022-05-28
* @Update    :
**********************************************************************/

int iot_data_inspect_init(void *pPublic, void **ppPrivate)
{
    IOT_INSPECT_T *ptData = NULL;
    IOT_APPDATA_T *pbData = NULL;
    pbData = pPublic;
    ptData = malloc(sizeof(IOT_INSPECT_T));
    if(NULL == ptData)
    {
        return -1;
    }
    memset(ptData, 0, sizeof(IOT_INSPECT_T));
    
    IOT_FMT_DEBUG("start inspect monitor period %d sec\n",pbData->app_t.monitorTime);
    /*获取每一个容器的错误信息*/
    /*1.获取现在有安装多少个容器,记录容器名字*/
    /*2.获取limit门限值*/
    /*初始化结构体*/
    pthread_mutex_init(&g_IotIndexlock,NULL);
#if 0    
    ret = get_all_infolimit(ptData);
    if(ret < 0)
    {
        //没有获取到容器
        return -1;
    } 
    、、ptData->InitFLag = 1;
#endif
    ptData->MonitorTime = time(NULL);
    ptData->InitFLag = 1;

    *ppPrivate = ptData;
    return 0;
}
#if 0
void get_tmp_storageinfo(IOT_INSPECT_T *thisptData,ALL_CONTAINER_AND_APP_T *tmpcode)
{
    uint8 dockernum=0,i,j,appnum=0,index=0;
    IOT_INSPECT_T *tmp_storage = thisptData;
    memset(tmp_storage,0,sizeof(IOT_INSPECT_T));
    tmp_storage->dockernum = tmpcode->count;
    dockernum = tmpcode->count;
    for(i=0;i<dockernum;i++)
    {
        strcpy((char *)tmp_storage->docker_runinfo[i].name,(char *)tmpcode->containers[i].container.ContainerName);
        tmp_storage->docker_runinfo[i].cpu_usage = tmpcode->containers[i].container.CpuRate;
        tmp_storage->docker_runinfo[i].mem_usage = tmpcode->containers[i].container.MenRate;
        tmp_storage->docker_runinfo[i].disk_usage = tmpcode->containers[i].container.StorageRate;
        tmp_storage->docker_runinfo[i].runtime   = tmpcode->containers[i].container.RunTime;
        if(strcmp((char *)tmpcode->containers[i].container.ContainerState,"running")==0)
        {
            tmp_storage->docker_runinfo[i].status = 0;
        }
        else
        {
            tmp_storage->docker_runinfo[i].status = 1;
        }
        appnum = tmpcode->containers[i].apps.appcount;
        for(j=0;j<appnum;j++)
        {
            if(strcmp((char *)tmpcode->containers[i].apps.appinfo[j].containername,(char *)tmp_storage->docker_runinfo[i].name)==0)
            {
                tmp_storage->docker_runinfo[i].appNum++;
                strcpy((char *)tmp_storage->docker_runinfo[i].apprun[index].Appname,(char *)tmpcode->containers[i].apps.appinfo[j].appname);
                //tmp_storage->docker_runinfo[i].apprun[index].cpu_usage = tmpcode->containers[i].apps.appinfo[j].serviceinfo[0].cpurate;
                //tmp_storage->docker_runinfo[i].apprun[index].mem_usage = tmpcode->containers[i].apps.appinfo[j].serviceinfo[0].memrate;
                OOP_DATETIME_S_T start_t;
                time_t tmpTime;
                memset(&tmpTime,0,sizeof(tmpTime));
                memset(&start_t,0,sizeof(OOP_DATETIME_S_T));
                start_t = tmpcode->containers[i].apps.appinfo[j].serviceinfo[0].starttime;
                DT_DateTimeShort2Time(&start_t,&tmpTime);
                tmp_storage->docker_runinfo[i].apprun[index].runtime = time(NULL) - tmpTime;
                tmp_storage->docker_runinfo[i].apprun[index].status = tmpcode->containers[i].apps.appinfo[j].serviceinfo[0].servicestate;
                tmp_storage->docker_runinfo[i].apprun[index].status = tmpcode->containers[i].apps.appinfo[j].serviceinfo[0].servicestate;
                tmp_storage->docker_runinfo[i].apprun[index].enable = tmpcode->containers[i].apps.appinfo[j].serviceinfo[0].serviceenable;
                index++;
            }
        }
        
    }
}
#endif

/**********************************************************************
* @name      : iot_devrun_get
* @brief     : 设备告警参数获取
* @param[in] : ptData 线程结构体 
* @param[out]: 
* @return    ：
* @Create    : 
* @Date      ：2022-05-28
* @Update    :
**********************************************************************/

void iot_devrun_get(IOT_INSPECT_T *ptData,dev_run_info *pbData)
{
    if(ptData==NULL || pbData == NULL)
        return;
    ptData->dev_info_t.dev_run.cpu_usage  =   pbData->cpu_usage;
    ptData->dev_info_t.dev_run.mem_usage  =   pbData->mem_usage;
    ptData->dev_info_t.dev_run.disk_usage =   pbData->disk_usage;
    ptData->dev_info_t.dev_run.cputemp = pbData->cputemp;
}


/**********************************************************************
* @name      : judg_dev_threshold
* @brief     : 设备告警判断函数
* @param[in] : ptData 线程结构体 pbData 设备阈值参数
* @param[out]: 
* @return    ：
* @Create    : 
* @Date      ：2022-05-28
* @Update    :
**********************************************************************/

void judg_dev_threshold(IOT_INSPECT_T *ptData,IOT_APPDATA_T *pbData)
{
    if(ptData==NULL || pbData == NULL)
        return;
    iot_run_info *dev = &ptData->dev_info_t;
    
    if(dev->dev_type.cputype==1)
    {
        if(dev->dev_run.cpu_usage < pbData->app_t.devCpuLimit && dev->dev_run.cpu_usage>0)
        {
            //cpu 恢复
            IOT_FMT_DEBUG("dev CPU usage less than Limit cpu-usage %d cpu-Limit %d\n",dev->dev_run.cpu_usage,pbData->app_t.devCpuLimit);
            iot_dev_alarm_notifi(2,0);
            dev->dev_type.cputype=0;
        }
    }
    else
    {
        if(dev->dev_run.cpu_usage >= pbData->app_t.devCpuLimit)
        {
            //cpu 越限
            IOT_FMT_DEBUG("dev CPU usage more than Limit cpu-usage %d cpu-Limit %d\n",dev->dev_run.cpu_usage,pbData->app_t.devCpuLimit);
            iot_dev_alarm_notifi(1,0);
            dev->dev_type.cputype=1;
        }
    }
    if(dev->dev_type.memtype==1)
    {
        if(dev->dev_run.mem_usage < pbData->app_t.devMemLimit && dev->dev_run.mem_usage>0)
        {
            //mem 恢复
            IOT_FMT_DEBUG("dev MEM usage less than Limit mem-usage %d mem-Limit %d\n",dev->dev_run.mem_usage,pbData->app_t.devMemLimit);
            iot_dev_alarm_notifi(2,1);
            dev->dev_type.memtype=0;
        }
    }
    else
    {
        if(dev->dev_run.mem_usage >= pbData->app_t.devMemLimit)
        {
            //mem 越限
            IOT_FMT_DEBUG("dev MEM usage more than Limit mem-usage %d mem-Limit %d\n",dev->dev_run.mem_usage,pbData->app_t.devMemLimit);
            iot_dev_alarm_notifi(1,1);
            dev->dev_type.memtype=1;
        }
    }
    if(dev->dev_type.disktype==1)
    {
        if(dev->dev_run.disk_usage < pbData->app_t.devDiskLimit && dev->dev_run.disk_usage>0)
        {
            //mem 恢复
            IOT_FMT_DEBUG("dev DISK usage less than Limit disk-usage %d disk-Limit %d\n",dev->dev_run.disk_usage,pbData->app_t.devDiskLimit);
            iot_dev_alarm_notifi(2,2);
            dev->dev_type.disktype=0;
        }
    }
    else
    {
        if(dev->dev_run.disk_usage >= pbData->app_t.devDiskLimit)
        {
            //mem 越限
            IOT_FMT_DEBUG("dev DISK usage more than Limit disk-usage %d eisk-Limit %d\n",dev->dev_run.disk_usage,pbData->app_t.devDiskLimit);
            iot_dev_alarm_notifi(1,2);
            dev->dev_type.disktype=1;
        }
    }

    if(dev->dev_type.cpuTDtype==1)
    {
        if(dev->dev_run.cputemp > pbData->app_t.devCpuTempdown && dev->dev_run.cputemp >0)
        {
            //mem 恢复
            IOT_FMT_DEBUG("dev CPUTEMP usage more than cputempdown Limit cpu-temp %d cpu-tempdown %d\n",dev->dev_run.cputemp,pbData->app_t.devCpuTempdown);
            iot_dev_alarm_notifi(2,3);
            dev->dev_type.cpuTDtype=0;
        }
    }
    else
    {
        if(dev->dev_run.cputemp <= pbData->app_t.devCpuTempdown && dev->dev_run.cputemp>0)
        {
            //mem 越限
            IOT_FMT_DEBUG("dev CPUTEMP usage less than cputempdown Limit cpu-temp %d cpu-tempup %d\n",dev->dev_run.cputemp,pbData->app_t.devCpuTempdown);
            iot_dev_alarm_notifi(1,3);
            dev->dev_type.cpuTDtype=1;
        }
    }
    if(dev->dev_type.cpuTUtype==1)
    {
        if(dev->dev_run.cputemp < pbData->app_t.devCpuTempup && dev->dev_run.cputemp>0)
        {
            //mem 恢复
            IOT_FMT_DEBUG("dev CPUTEMP usage less than cputempup Limit cpu-temp %d cpu-tempdown %d\n",dev->dev_run.cputemp,pbData->app_t.devCpuTempup);
            iot_dev_alarm_notifi(2,3);
            dev->dev_type.cpuTUtype=0;
        }
    }
    else
    {
        if(dev->dev_run.cputemp >= pbData->app_t.devCpuTempup)
        {
            //mem 越限
            IOT_FMT_DEBUG("dev CPUTEMP usage more than cputempup Limit cpu-temp %d cpu-tempup %d\n",dev->dev_run.cputemp,pbData->app_t.devCpuTempup);
            iot_dev_alarm_notifi(1,3);
            dev->dev_type.cpuTUtype=1;
        }
    }

}

/**********************************************************************
* @name      : iot_data_inspect_proc
* @brief     : 设备告警主函数
* @param[in] : pPublic 公共结构体 pPrivate 线程结构体
* @param[out]: 
* @return    ：
* @Create    : 
* @Date      ：2022-05-28
* @Update    :
**********************************************************************/

int iot_data_inspect_proc(void *pArg,void *pPublic, void *pPrivate)
{
    IOT_INSPECT_T *ptData = pPrivate;
    IOT_APPDATA_T *pbData = pPublic;
    int ret = 0,i=0;
    if((time(NULL)-ptData->MonitorTime )< pbData->app_t.monitorTime || pbData->init_type != 1)
        return 100000;
    inspect_thread_type++;
    if(inspect_thread_type==30)
    {
        IOT_FMT_DEBUG("inspect thread Normal runing 30 period\n");
        inspect_thread_type = 0;
    }
    if(ptData->InitFLag == 1)
    {
       IOT_FMT_DEBUG("inspect thread is runing \n");
       ptData->InitFLag =0;
    }

    /*检测是否产生警告 todo*/
    IOT_INSPECT_T thisptData ; 
    ALL_CONTAINER_AND_APP_T tmp_allinfo;
    memset(&thisptData,0,sizeof(IOT_INSPECT_T));
    memset(&tmp_allinfo,0,sizeof(ALL_CONTAINER_AND_APP_T));
  //  pthread_mutex_lock(&allcontainerinfo.lock);
    tmp_allinfo.count = allcontainerinfo.count;
    for(i=0;i<tmp_allinfo.count;i++)
    {
        memcpy(&tmp_allinfo.dev_run,&allcontainerinfo.dev_run,sizeof(dev_run_info));
        memcpy(&tmp_allinfo.containers[i],&allcontainerinfo.containers[i],sizeof(CONTAINER_AND_APP_T));
    }    
  //  pthread_mutex_unlock(&allcontainerinfo.lock);
    //获取每一轮的数据放到临时结构体
    ret = get_all_infolimit(ptData,&tmp_allinfo);   //获取阈值
    if(ret < 0)
    {
        ptData->MonitorTime = time(NULL);
        return 100000;
    }
    iot_devrun_get(ptData,&tmp_allinfo.dev_run);
    //get_tmp_storageinfo(&thisptData,&tmp_allinfo);
    //和限制做判断是否高于限制
    judg_value_threshold(ptData,pbData);
    judg_dev_threshold(ptData,pbData);
    ptData->MonitorTime = time(NULL);
    return 100000;
}





