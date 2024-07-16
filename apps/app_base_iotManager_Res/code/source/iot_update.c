/*
*********************************************************************
* Copyright(C) 2020 �Ͼ��������ӹɷ����޹�˾
* All rights reserved. 
* @brief��   ���APPϵͳ�����߳�
* @date��    2020-6-30
* @history�� 
*********************************************************************
*/
#include"iot_init.h"
#include"iot_public.h"

/**********************************************************************
* @name      : iot_dev_alarm_notifi
* @brief     : �豸�澯��Ϣ
* @param[in] : uint8 alarmtype,uint8 notiftype) ��ز���
* @param[out]:  
* @return    ��
* @Create    : 
* @Date      ��2019-10-30
* @Update    :
**********************************************************************/

void iot_dev_alarm_notifi(WGPubattr *sys,uint8 alarmtype,uint8 notiftype)
{
    UPDATE_FMT_DEBUG("����ϵͳ�澯�¼�\n");
    UPDATE_FMT_DEBUG("�澯���%d ֪ͨ���%d\n",alarmtype,notiftype);
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
    buff[index++]=00;  //  �澯������ʱΪ0;
    MSG_INFO_T* msg  = malloc(sizeof(MSG_INFO_T)+index);
    if(msg==NULL)
        return;
    memset(msg,0,sizeof(MSG_INFO_T)+index);
    //char broad[50] = "Broadcast";
    msg->MsgRPM = 1;
    msg->MsgPRIORITY = 1;
    msg->MsgIndex = iot_get_next_index();
    msg->MsgLabel = 0;

    msg->SourLen = strlen(WUGUAN_APP_NAME_M);
    memcpy(msg->SourAddr, WUGUAN_APP_NAME_M, msg->SourLen);

    msg->DestLen = strlen("Broadcast");
    strcpy(msg->DestAddr, "Broadcast");
    //memcpy(msg->DestAddr, "Broadcast", msg->DestLen);
    msg->IID = MESSAGE_IID_IOT;
    msg->IOP = IOT_IOP_SYS_WARRING;
    msg->MsgLen = index;
    memcpy(msg->MsgData,buff,index);
    taskmng_plc_queue_write(&sys->upque,(uint8*)msg, msg->MsgLen+sizeof(MSG_INFO_T));
    free(msg);
}


/**********************************************************************
* @name      : judg_dev_threshold
* @brief     : �豸�澯�жϺ���
* @param[in] : ptData �߳̽ṹ�� pbData �豸��ֵ����
* @param[out]: 
* @return    ��
* @Create    : 
* @Date      ��2022-05-28
* @Update    :
**********************************************************************/

void judg_dev_threshold(WGPubattr *sys)
{
    if(sys==NULL)
        return;
    
    SYSCONFIG *sysconfig = &sys->systheadinfo.sysconfig;
    SYSINFO *sysinfo = &sys->systheadinfo.sysinfo;
    SYSEVENTFLAG *sysflag = &sys->systheadinfo.sysflag;
    if(sysflag->cpuflag== 1 && sysinfo->cpu_usage < sysconfig->devCpuLimit)
    {
        //cpu �ָ�
       // IOT_FMT_DEBUG("dev CPU usage less than Limit cpu-usage %d cpu-Limit %d\n",dev->dev_run.cpu_usage,pbData->app_t.devCpuLimit);
        iot_dev_alarm_notifi(sys,2,0);
        sysflag->cpuflag=0;
    }
    else if(sysflag->cpuflag== 0 && sysinfo->cpu_usage >= sysconfig->devCpuLimit)
    {
        //cpu Խ��
        //IOT_FMT_DEBUG("dev CPU usage more than Limit cpu-usage %d cpu-Limit %d\n",dev->dev_run.cpu_usage,pbData->app_t.devCpuLimit);
        iot_dev_alarm_notifi(sys,1,0);
        sysflag->cpuflag=1;
    }
    if(sysflag->memflag== 1 && sysinfo->mem_usage < sysconfig->devMemLimit)
    {
        //mem �ָ�
      //  IOT_FMT_DEBUG("dev MEM usage less than Limit mem-usage %d mem-Limit %d\n",dev->dev_run.mem_usage,pbData->app_t.devMemLimit);
        iot_dev_alarm_notifi(sys,2,1);
        sysflag->memflag= 0;
    }
    else if(sysflag->memflag== 0 && sysinfo->mem_usage >= sysconfig->devMemLimit)
    {
        //mem Խ��
      //  IOT_FMT_DEBUG("dev MEM usage more than Limit mem-usage %d mem-Limit %d\n",dev->dev_run.mem_usage,pbData->app_t.devMemLimit);
        iot_dev_alarm_notifi(sys,1,1);
       sysflag->memflag= 1;
    }
    if(sysflag->diskflag== 1 && sysinfo->disk_usage < sysconfig->devDiskLimit)
    {
        //disk �ָ�
     //   IOT_FMT_DEBUG("dev DISK usage less than Limit disk-usage %d disk-Limit %d\n",dev->dev_run.disk_usage,pbData->app_t.devDiskLimit);
        iot_dev_alarm_notifi(sys,2,2);
        sysflag->diskflag= 0;
    }
    else if(sysflag->diskflag== 0 && sysinfo->disk_usage >= sysconfig->devDiskLimit)
    {
        //disk Խ��
       // IOT_FMT_DEBUG("dev DISK usage more than Limit disk-usage %d eisk-Limit %d\n",dev->dev_run.disk_usage,pbData->app_t.devDiskLimit);
        iot_dev_alarm_notifi(sys,1,2);
        sysflag->diskflag = 1;

    }

    if(sysflag->tempflag== 1 && sysinfo->cputemp < sysconfig->devCpuTempup)
    {
        iot_dev_alarm_notifi(sys,2,3);
        sysflag->tempflag= 0 ;
    }
    else if(sysflag->tempflag== 0 && sysinfo->cputemp >= sysconfig->devCpuTempup)
    {
        //mem Խ��
        iot_dev_alarm_notifi(sys,1,3);
        sysflag->tempflag= 1 ;
    }

}


/**********************************************************************
* @name      : get_dev_runinfo
* @brief     : ��ȡ�豸����Ϣ
* @param[in] :  
* @param[out]: allcontainerinfo.dev_run    �豸��Ϣ
* @return    : 
* @Create    : 
* @Date      : 2019-10-30
* @Update    :
**********************************************************************/

void get_dev_runinfo(SYSINFO *sysinfo)
{
    char buff[5][500]={{0}};
    char str[5][500] = {{0}};
    uint8 ret=0;
    int num=0;
    uint16 cpu0temp = 0;
    ret = getshellresult("top -n1 | grep 'CPU:' | awk '{print 10000-$8*100}'",buff,5);
    if(ret==1)
    {
        sysinfo->cpu_usage = atoi(buff[0]);
    }
    memset(buff,0,sizeof(buff));
    ret = getshellresult("free -m | grep 'Mem' | awk '{print $3/$2*100*100}'",buff,5);
    if(ret==1)
    {
        /*�ٰ�.ȥ��*/
        ssplit(buff[0], ".",str, &num);
        if(num!=0)
        {
            sysinfo->mem_usage = atoi(str[0]);
        }   
    } 
    memset(buff,0,sizeof(buff));
    ret = getshellresult("df -h / | grep p9 | awk '{print $5*100}'",buff,5);
    if(ret==1)
    {
        sysinfo->disk_usage = atoi(buff[0]);
    }
    memset(buff,0,sizeof(buff));
    ret = getshellresult("cat /sys/class/thermal/thermal_zone0/temp",buff,5);
    if(ret==1)
    {
       cpu0temp = atoi(buff[0])*10;
    }
    sysinfo->cputemp = cpu0temp;
}


int iot_updateproc(void *pArg,void *pPublic, void *pPrivate)
{
    WGPubattr * public = pPublic;
    WGSYSINFO * sys = &public->systheadinfo;
    if(time(NULL)<= sys->runtime )
    {
        sys->runtime = time(NULL);
        return 100000;
    }
        
    if(time(NULL)-sys->runtime <sys->sysconfig.updateTime)
        return 100000;
//    UPDATE_FMT_DEBUG("start update \n");
    sys->runtime = time(NULL);
    //��������
    get_dev_runinfo(&sys->sysinfo);
    UPDATE_FMT_DEBUG("cpu_usage %d mem_usage %d disk_usage %d cputemp %d \n",sys->sysinfo.cpu_usage,sys->sysinfo.mem_usage,sys->sysinfo.disk_usage,sys->sysinfo.cputemp);
    //�ж��Ƿ�Խ��
    judg_dev_threshold(public);
    return 100000;
}

int iot_updateinit(void *pPublic, void **ppPrivate)
{
    WGPubattr *pPubattr = (WGPubattr*)pPublic;
    memset(&pPubattr->systheadinfo,0,sizeof(pPubattr->systheadinfo));
    pPubattr->systheadinfo.runtime = time(NULL);
    pPubattr->systheadinfo.sysconfig.devCpuLimit = 9000;
    pPubattr->systheadinfo.sysconfig.devMemLimit = 9000;
    pPubattr->systheadinfo.sysconfig.devDiskLimit = 9000;
    pPubattr->systheadinfo.sysconfig.devCpuTempup = 950;
    pPubattr->systheadinfo.sysconfig.devCpuTempdown = 100;
    pPubattr->systheadinfo.sysconfig.updateTime = 60;
    pPubattr->systheadinfo.sysconfig.monitorTime = 60;
    UPDATE_FMT_DEBUG("iotManager update thread runing \n");
    INIT_FMT_DEBUG("ϵͳĬ�ϲ��� devCpuLimit %d ��devMemLimit %d devDiskLimit %d devCpuTempup %d devCpuTempdown %d monitorTime %d \n",pPubattr->systheadinfo.sysconfig.devCpuLimit,pPubattr->systheadinfo.sysconfig.devMemLimit,pPubattr->systheadinfo.sysconfig.devDiskLimit
    ,pPubattr->systheadinfo.sysconfig.devCpuTempup, pPubattr->systheadinfo.sysconfig.devCpuTempdown,pPubattr->systheadinfo.sysconfig.monitorTime);
    //��monitorctl ���ò���
    return 0;
}


