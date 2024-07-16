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

ALL_CONTAINER_AND_APP_T allcontainerinfo;
uint8 Normal_oper_time = 0;

int strrpc(char *str,char *outstr,char *oldstr,char *newstr){
	char bstr[strlen(str)];
	memset(bstr,0,sizeof(bstr));
	int i=0;
	for(i = 0;i < strlen(str);i++){
	if(!strncmp(str+i,oldstr,strlen(oldstr))){
		strcat(bstr,newstr);
		i += strlen(oldstr) - 1;
	}else{
		strncat(bstr,str + i,1);
		}
	}
 
	strcpy(outstr,bstr);
	return 0;
}

/**********************************************************************
* @name      : get_len_offset
* @brief     ：获取串类型数据元素长度
* @param[in] ：*indata      数据Buffer
* @param[out]：*bytes       数据Buffer长度
* @return    ：长度
* @Create    : 
* @Date      ：2019-10-30
* @Update    :
**********************************************************************/

uint16 get_len_offset(uint8 *indata,uint8 *bytes)
{
    uint16 EleNum;
    uint8 nbytes = 1;
    
    EleNum = indata[0];
    if(EleNum==0x81)
    {
        EleNum = indata[1];
        nbytes = 2;
    }
    else if(EleNum==0x82)
    {
        EleNum = indata[1];
        EleNum <<= 8;
        EleNum += indata[2];
        nbytes = 3;
    }
    else if(EleNum==0x80 || EleNum>0x82)
    {
        return 0;
    }
    
    if(bytes != NULL)
        *bytes = nbytes;
        
    return EleNum;
}

OOP_DATETIME_S_T timeformat_s(char* time)
{

    OOP_DATETIME_S_T tmp;
    memset(&tmp,0x00,sizeof(OOP_DATETIME_S_T));
    int year,month,day,hour,minute,second;
    
    sscanf(time,"%d-%d-%d %d:%d:%d.",&year,&month,&day,&hour,&minute,&second);
    tmp.year =year;
    tmp.month =month;
    tmp.day =day;
    tmp.hour =hour;
    tmp.minute =minute;
    tmp.second =second;
    return tmp;
}

/**********************************************************************
* @name      : strim
* @brief     : 去除首尾空格
* @param[in] ：*str      数据Buffer
* @param[out]：
* @return    ：新的字符串
* @Create    : 
* @Date      ：2019-10-30
* @Update    :
**********************************************************************/
char * strim(char *str)
{
    char *strend,*sp,*ep;
    int len;
    sp = str;
    strend = str + strlen(str) - 1;
    ep = strend;
 
    while(sp<=strend && isspace(*sp))// *sp == ' '也可以
        sp++;
    while(ep>=sp && isspace(*ep))
        ep--;
    len = (ep < sp) ? 0:(ep-sp)+1;//(ep < sp)判断是否整行都是空格
    sp[len] = '\0';
    return sp;

}

/**********************************************************************
* @name      : percentdatachange
* @brief     : 百分比的字符串转为数字
* @param[in] ：buf      数据Buffer
* @param[out]：
* @return    ：新的字符串
* @Create    : 
* @Date      ：2019-10-30
* @Update    :
**********************************************************************/

uint16 percentdatachange(char* buf)
{
    char* t;
    char data[10]={0};
    t = strstr(buf,"%");
    memcpy(data,buf,t-buf);
    return atof(data)*100;
}

uint8 get_appnum(char* buf)
{
    char data[10]={0};
    const char* str = "Total app number";
    char* p = strstr(buf,str);
    if(p==NULL)
        return 0;
    strcpy(data,p+strlen(str));
    return atoi(strim(data));
}


void timeformat(char* time,OOP_DATETIME_S_T *ooptime)
{

    OOP_DATETIME_S_T tmp;
    memset(&tmp,0x00,sizeof(OOP_DATETIME_S_T));
    int year,month,day,hour,minute,second;
    time_t tmptime;
    sscanf(time,"%d-%d-%dT%d:%d:%d.",&year,&month,&day,&hour,&minute,&second);
    tmp.year =year;
    tmp.month =month;
    tmp.day =day;
    tmp.hour =hour;
    tmp.minute =minute;
    tmp.second =second;
    DT_DateTimeShort2Time(&tmp,&tmptime);
    //tmptime+=8*3600; //转为东八区
    DT_Time2DateTimeShort(tmptime,ooptime);
    //printf("time %d %d %d %d %d %d \n",ooptime->year,ooptime->month,ooptime->day,ooptime->hour,ooptime->minute,ooptime->second);
}

BOOL DT_DateTimeShort2Time(OOP_DATETIME_S_T *dateTime,time_t *linuxTim)
{
    struct tm ptm;
    
    if(dateTime == NULL)
    {
        return FALSE;
    }
    
    if(FALSE == DT_DateTimeShortIsValid(dateTime))
    {
        return FALSE;
    }

    ptm.tm_year = dateTime->year-1900;
    ptm.tm_mon = dateTime->month-1;
    ptm.tm_mday = dateTime->day;
    ptm.tm_hour = dateTime->hour;
    ptm.tm_min = dateTime->minute;
    ptm.tm_sec = dateTime->second;

    //*linuxTim = mktime(&ptm);
    *linuxTim = FastDateToSecond(&ptm);
    return TRUE;
}


//IP地址转换
void IPchange(char* buf,uint8* out)
{
    char* p = buf;
    char* t;
    t = strsep(&p,".");
    while(t!=NULL)
    {
        *out++ = atoi(t);
        t = strsep(&p,".");
    }
}
/*
void retu_kbval(char *src)
{
    while(src)
    {
        if(src)
        src++;
    }
}*/

void ssplit(char *src,const char *delim,char dest[][255],int *num)
{
    char* p = src;
    char* tmp;
    int i =0;
    tmp = strsep(&p,delim);
    while(tmp!=NULL)
    {
        strcpy(dest[i],tmp);
        tmp = strsep(&p,delim);
        i++;
    }
    *num = i;
}

int getshellresult(const char* cmd,char result[][500],int size)
{
    //UPDATE_FMT_DEBUG(cmd,strlen(cmd),"\n");
    char temp[1024];
    FILE* fp = NULL;
    int row = 0;
    fp = popen(cmd, "r");
    if(fp == NULL)
    {
        return 0;
    }
  /*  int fd = fileno(fp);
    int flags = fcntl(fd,F_GETFL,0);
    flags |=O_NONBLOCK;
    fcntl(fd, F_SETFL, flags); 
    fp = fdopen(fd, "r");*/
    while(fgets(temp, sizeof(temp), fp) != NULL)
    {
        strcpy(result[row], strim(temp));
        row++;
        if(row>=size)
            break;
    }

    if(fp != NULL)
    {
        pclose(fp);
    }

    return row;

}//2020-06-09T01:01:57.208792693Z 格式的时间


/**********************************************************************
* @name      : get_app_info
* @brief     : 获取App的信息
* @param[in] : containername 容器名字 
* @param[out]：appinfo       app信息
* @return    : 
* @Create    : 
* @Date      : 2019-10-30
* @Update    :
**********************************************************************/

void get_app_info(char* containername,ALL_APP_INFO_T* appinfo)
{
    int count;
    uint8 i;
    char buff[100][500];
    memset(buff,0,sizeof(buff));
    char cmd[50]={0};
    if(NULL == containername || 0 == strlen(containername))
        return ;
    sprintf(cmd,"appm -I -c %s",containername);
    count = getshellresult(cmd,buff,100);
    if(count<=1)
        return;
    appinfo->appcount = get_appnum(buff[0]);
    for(i=0;i<appinfo->appcount;i++)
    {
        strcpy((char*)appinfo->appinfo[i].containername,containername);
        appinfo->appinfo[i].servicecount = 1;
    }
    i = 1;
    int appindex = -1;
    uint32 memlimit = 300 ;
    while(i<count)
    {
        if(strlen(buff[i])==0)
        {
            i++;
            continue;
        }
        char key[50]={0};
        char value[50]={0};
        char tmp[20] = {0};
        char *p = strstr(buff[i],":");
        memcpy(key,buff[i],p-buff[i]);
        memcpy(value,p+1,strlen(p)-1);
        char* keytrim = strim(key);
        char* valuetrim = strim(value);
#ifdef PRODUCT_CCU
        if(strcmp(keytrim,"APP index")==0)
#else
        if(strcmp(keytrim,"App index")==0)
#endif 
        {
            appindex++;
            appinfo->appinfo[appindex].appindex = atoi(valuetrim);
         //   UPDATE_FMT_DEBUG("index %d \n\n",appinfo->appinfo[appindex].appindex);
        }
#ifdef PRODUCT_CCU
        else if(strcmp(keytrim,"APP name")==0)
#else
        else if(strcmp(keytrim,"App name")==0)
#endif
        {
            strcpy((char*)appinfo->appinfo[appindex].appname,valuetrim);
            //UPDATE_FMT_DEBUG("name %s\n\n",appinfo->appinfo[appindex].appname);
        }
#ifdef PRODUCT_CCU
        else if(strcmp(keytrim,"APP version")==0)
#else
        else if(strcmp(keytrim,"App version")==0)
#endif
        {
            strcpy((char*)appinfo->appinfo[appindex].appversion,valuetrim);
        //    UPDATE_FMT_DEBUG("version %s\n\n",appinfo->appinfo[appindex].appversion);
        }
#ifdef PRODUCT_CCU
        else if(strcmp(keytrim,"APP hash")==0)
#else
        else if(strcmp(keytrim,"App hash")==0)
#endif
        {
            strcpy((char*)appinfo->appinfo[appindex].apphash,valuetrim);
        //    UPDATE_FMT_DEBUG("hash %s\n\n",appinfo->appinfo[appindex].apphash);
        }else if(strcmp(keytrim,"Service index")==0)
        {
            appinfo->appinfo[appindex].serviceinfo[0].serviceindex = atoi(valuetrim);
         //   UPDATE_FMT_DEBUG("Serviceindex %d\n\n",appinfo->appinfo[appindex].serviceinfo[0].serviceindex);
        }else if(strcmp(keytrim,"Service name")==0)
        {
            strcpy((char*)appinfo->appinfo[appindex].serviceinfo[0].servicename,valuetrim);
         //   UPDATE_FMT_DEBUG("Servicename  %s\n\n",appinfo->appinfo[appindex].serviceinfo[0].servicename);
        }else if(strcmp(keytrim,"Service enable")==0)
        {
#ifdef PRODUCT_CCU
            if(strcmp(valuetrim,"yes")==0)
#else
            if(strcmp(valuetrim,"YES")==0)
#endif
                appinfo->appinfo[appindex].serviceinfo[0].serviceenable = 1;
            else
                appinfo->appinfo[appindex].serviceinfo[0].serviceenable = 0;
         //   UPDATE_FMT_DEBUG("Service enable %d\n\n",appinfo->appinfo[appindex].serviceinfo[0].serviceenable);
        }else if(strcmp(keytrim,"Service status")==0)
        {
            if(strcmp(valuetrim,"running")==0)
                appinfo->appinfo[appindex].serviceinfo[0].servicestate = 0;
            else
                appinfo->appinfo[appindex].serviceinfo[0].servicestate = 1;
         //   UPDATE_FMT_DEBUG("Service status %d\n\n",appinfo->appinfo[appindex].serviceinfo[0].servicestate);
        }else if(strcmp(keytrim,"CPUs")==0)
        {
            appinfo->appinfo[appindex].cpucore = atoi(valuetrim);
        }else if(strcmp(keytrim,"CPU threshold")==0)
        {
            appinfo->appinfo[appindex].serviceinfo[0].cpulimit = percentdatachange(valuetrim);
            appinfo->appinfo[appindex].cpuLimit = appinfo->appinfo[appindex].serviceinfo[0].cpulimit;
           // UPDATE_FMT_DEBUG("CPU threshold %d\n\n",appinfo->appinfo[appindex].cpuLimit);
        }else if(strcmp(keytrim,"CPU usage")==0)
        {
            appinfo->appinfo[appindex].serviceinfo[0].cpurate = percentdatachange(valuetrim);
          // UPDATE_FMT_DEBUG("CPU usage %d\n\n",appinfo->appinfo[appindex].serviceinfo[0].cpurate);
        }
        
#ifdef PRODUCT_CCU
        else if(strcmp(keytrim,"MEM Limit")==0)
#else
        else if(strcmp(keytrim,"Mem Limit")==0)
#endif
        {
            /*将值转为kb*/
            //retu_kbval(valuetrim); 
            char *p = strstr(valuetrim,"kB");
            if(p==NULL)
            {
                //说明配置的是M
                p = strstr(valuetrim,"m");
                if(p!=NULL)
                {   
                    memcpy(tmp,valuetrim,p-valuetrim);
                    memlimit = atoi(tmp)*1024;
                }
            }
            else
            {
                memcpy(tmp,valuetrim,p-valuetrim);
                memlimit = atoi(tmp);
            }
            appinfo->appinfo[appindex].memLval = atoi(valuetrim);
            if(memlimit == 0)
            {
                UPDATE_FMT_DEBUG("错误的参数 memlimit %d \n",memlimit);
                memlimit = 300;
            }

        }
#ifdef PRODUCT_CCU
        else if(strcmp(keytrim,"MEM threshold")==0)
#else
        else if(strcmp(keytrim,"Mem threshold")==0)
#endif
        {
           appinfo->appinfo[appindex].memLimit = percentdatachange(valuetrim);
        }
#ifdef PRODUCT_CCU
        else if(strcmp(keytrim,"MEM usage")==0)
        {
            appinfo->appinfo[appindex].serviceinfo[0].memrate = percentdatachange(valuetrim);
        }
#else
        else if(strcmp(keytrim,"Mem usage")==0)
        {
            uint32 memrate = 0;
            char *p = strstr(valuetrim,"kB");
            if(p==NULL)
            {
                //说明配置的是M
                p = strstr(valuetrim,"m");
                if(p!=NULL)
                {   
                    memcpy(tmp,valuetrim,p-valuetrim);
                    memrate = atoi(tmp)*1024;
                }
            }
            else
            {
                memcpy(tmp,valuetrim,p-valuetrim);
                memrate = atoi(tmp);
            }

            appinfo->appinfo[appindex].serviceinfo[0].memrate = memrate*100*100/memlimit;
       //     UPDATE_FMT_DEBUG("memrate %d  memlimit %d  占比%d\n\n",memrate,memlimit,appinfo->appinfo[appindex].serviceinfo[0].memrate);
        }
#endif
       else if(strcmp(keytrim,"Start time")==0)
        {
            if(strcmp(keytrim,"unknown")!=0)
            {
                
                appinfo->appinfo[appindex].serviceinfo[0].starttime =timeformat_s(valuetrim);
            }
        }
        i++;
    }
}


/**********************************************************************
* @name      : get_container_info
* @brief     : 获取容器的信息
* @param[in] : 
* @param[out]: allcontainer   容器信息
* @return    : 
* @Create    : 
* @Date      : 2019-10-30
* @Update    :
**********************************************************************/

void get_container_info(ALL_CONTAINER_INFO_T *allcontainer)
{
    int count=0;
    uint8 i,realnum=0,j=0;
    char buff[20][500];
    memset(buff,0,sizeof(buff));
    char cmd[500] = "docker ps --format '{{.ID}}#{{.Names}}#{{.Mounts}}#{{.Ports}}#{{.Image}}'";
    count = getshellresult(cmd,buff,20);
    if(count == 0)
    {
        UPDATE_FMT_DEBUG("ERROR 没有检测到容器 \n");
        return ;
    }
    allcontainer->containercount = count;
    for(i=0;i<count;i++)
    {
        char str[50][255];
        char tmpstr[20][255];
        int num=0;
        memset(str,0x00,sizeof(str));
        memset(tmpstr,0x00,sizeof(tmpstr));
        ssplit(buff[i], "#",str, &num);
        if(num == 0)
            continue;
        //UPDATE_FMT_DEBUG("FilePath %s\n",str[1]);
        strcpy((char *)allcontainer->container[realnum].ContainerID,str[0]);
        strcpy((char*)allcontainer->container[realnum].ContainerName,str[1]);
//        ssplit(str[1], ",",tmpstr, &num);
//        if(num == 0)
//            continue;
        //allcontainer->container[realnum].FilePathnum = num;
//        uint16 offset = 0;
        char rpcbuf[255] = {0};
        strrpc(str[2],rpcbuf,",",";");
        
        memcpy(allcontainer->container[realnum].FilePath,rpcbuf,strlen(rpcbuf));
        strcpy((char*)allcontainer->container[realnum].Interface,"/dev/");
        strcpy((char*)allcontainer->container[realnum].DownloadFilePath,"/usr/local/extapps/");
        strcpy((char*)allcontainer->container[realnum].PortConfig,str[3]);
        strcpy((char*)allcontainer->container[realnum].ImageName,str[4]);
        char cmd[1024]={0};
        char result[1][500];
        memset(result,0x00,sizeof(result));
        sprintf(cmd,"docker inspect --format='{{.State.Status}}#{{.HostConfig.Memory}}#{{.NetworkSettings.IPAddress}}#{{.Created}}#{{.State.StartedAt}}' %s | grep '#'",allcontainer->container[i].ContainerName );
        if(getshellresult(cmd,result,1)!=1)
        {
            UPDATE_FMT_DEBUG("ERROR 命令执行没有返回或返回不正确\n %s \n",cmd);
            continue;
        }   
        memset(str,0x00,sizeof(str));
        ssplit(result[0], "#",str, &num);
        strcpy((char*)allcontainer->container[realnum].ContainerState,str[0]);
        IPchange(str[2], allcontainer->container[realnum].ContainerIP);
        timeformat(str[3],&allcontainer->container[realnum].CreateTime);
        timeformat(str[4],&allcontainer->container[realnum].StartUpTime);
        time_t starttime;
        memset(&starttime,0,sizeof(starttime));
        DT_DateTimeShort2Time(&allcontainer->container[realnum].StartUpTime,&starttime);
        allcontainer->container[realnum].RunTime =time(NULL)-starttime;
        realnum++;
    }
    realnum = 0;
    for(i=0;i<count;i++)
    {
         char cmd[255]={0};
         char str[4][255];
         char key[100]={0};
         char value[100]={0};
         char *p=NULL;
         memset(buff,0x00,sizeof(buff));
         memset(str,0,sizeof(str));

		 int ret = 0;
#ifdef PRODUCT_CCU
		
	    sprintf(cmd,"container monitor -d %s | grep limit",allcontainer->container[i].ContainerName);
		int num =0;
		do
		{
			ret = getshellresult(cmd,buff,DOCKERMAXLEN);

			if(ret ==3)
	         {
//	           UPDATE_FMT_DEBUG("ERROR 命令执行没有返回或返回不正确\n ret %d\n %s \n", ret ,cmd);
	           break;
	         }
			sleep(1);
			num ++;
		}
		while (num  <=3);

//		ret = getshellresult(cmd,buff,DOCKERMAXLEN);

		if(ret !=3)
         {
           UPDATE_FMT_DEBUG("ERROR 命令执行没有返回或返回不正确\n ret %d\n %s \n", ret ,cmd);
           continue;
         }
         p=buff[0];
         p=strsep(&p,"%");
         strsep(&p,":");
         allcontainer->container[realnum].cpuLimit = atoi(p)*100;
         p=buff[1];
         p=strsep(&p,"%");
         strsep(&p,":");
         allcontainer->container[realnum].memLimit = atoi(p)*100;
         p=buff[2];
         p=strsep(&p,"%");
         strsep(&p,":");
         allcontainer->container[realnum].stroLimit = atoi(p)*100;

		 memset(buff,0x00,sizeof(buff));
         memset(str,0,sizeof(str));
         sprintf(cmd,"container status %s ",allcontainer->container[i].ContainerName);
		 ret = getshellresult(cmd,buff,DOCKERMAXLEN);
         if(ret !=15)
         {
           UPDATE_FMT_DEBUG("ERROR 命令执行没有返回或返回不正确\n ret %d\n %s \n", ret ,cmd);
           continue;
         }
#else
        int num=0;
		sprintf(cmd,"container monitor -d %s | grep Limit",allcontainer->container[i].ContainerName);
		if(getshellresult(cmd,buff,DOCKERMAXLEN)!=1)
         {
           UPDATE_FMT_DEBUG("ERROR 命令执行没有返回或返回不正确\n %s \n",cmd);
           continue;
         }
         //containerName[c_cco], cpuLimit[90], memLimit[90], diskLimit[90]
         ssplit(buff[0], ",",str, &num);
         if(num == 0)
            continue;
         p=str[1];
         p=strsep(&p,"]");
         strsep(&p,"[");
         allcontainer->container[realnum].cpuLimit = atoi(p)*100;
         p=str[2];
         p=strsep(&p,"]");
         strsep(&p,"[");
         allcontainer->container[realnum].memLimit = atoi(p)*100;
         p=str[3];
         p=strsep(&p,"]");
         strsep(&p,"[");
         allcontainer->container[realnum].stroLimit = atoi(p)*100;

		 memset(buff,0x00,sizeof(buff));
         memset(str,0,sizeof(str));
         sprintf(cmd,"container status %s ",allcontainer->container[i].ContainerName);
		 ret = getshellresult(cmd,buff,DOCKERMAXLEN);
         if(ret!=10)
         {
           UPDATE_FMT_DEBUG("ERROR 命令执行没有返回或返回不正确 ret %d\n %s \n", ret, cmd);
           continue;
         }
#endif
         
         for(j=0;j<ret;j++)
         {
            char *q = NULL;
            char tmp[50] = {0};
            p = strstr(buff[j],":");
			if(p ==NULL)
            {
                continue;
            }   
            memset(key,0,sizeof(key));
            memset(value,0,sizeof(value));
            memcpy(key,buff[j],p-buff[j]);
            memcpy(value,p+1,strlen(p)-1);
            char* keytrim = strim(key);
            char* valuetrim = strim(value);
            if(strcmp(keytrim,"container cpu")==0)
            {
                q = strstr(valuetrim,"cores");
                if(q!=NULL)
                {
                    memcpy(&tmp,valuetrim,q-valuetrim);
                    char * tmpcore = strim(tmp);
                    allcontainer->container[realnum].CpuCores = atoi(tmpcore);
                }                
                else
                {
                    allcontainer->container[realnum].CpuCores = 4;
                }
            }else if(strcmp(keytrim,"container max mem")==0)
            {   
                
                 q = strstr(valuetrim,"M");
                if(q!=NULL)
                {   
                    memcpy(allcontainer->container[realnum].MemLimit,valuetrim,q-valuetrim);
                }
            }else if(strcmp(keytrim,"container disk-size")==0)
            {
                 q = strstr(valuetrim,"M");
                if(q!=NULL)
                {   
                    memcpy(allcontainer->container[realnum].StorageLimit,valuetrim,q-valuetrim);
                }
            }else if(strcmp(keytrim,"container cpu-usage")==0)
            {
                allcontainer->container[realnum].CpuRate = percentdatachange(valuetrim);
            }else if(strcmp(keytrim,"container mem-usage")==0)
            {
                allcontainer->container[realnum].MenRate = percentdatachange(valuetrim);
            }else if(strcmp(keytrim,"container disk-usage")==0)
            {
                allcontainer->container[realnum].StorageRate = percentdatachange(valuetrim);
            }
         }
 
        realnum++;
    } 
    allcontainer->containercount = realnum;
}


/**********************************************************************
* @name      : get_dev_runinfo
* @brief     : 获取设备的信息
* @param[in] :  
* @param[out]: allcontainerinfo.dev_run    设备信息
* @return    : 
* @Create    : 
* @Date      : 2019-10-30
* @Update    :
**********************************************************************/

void get_dev_runinfo()
{
    char buff[5][500]={{0}};
    char str[5][255] = {{0}};
    uint8 ret=0;
    int num=0;
    uint16 cpu1temp,cpu0temp;
    ret = getshellresult("top -n1 | grep 'CPU:' | awk '{print 10000-$8*100}'",buff,5);
    if(ret!=1)
    {
        return;
    }
    allcontainerinfo.dev_run.cpu_usage = atoi(buff[0]);
    memset(buff,0,sizeof(buff));
    ret = getshellresult("free -m | grep 'Mem' | awk '{print $3/$2*100*100}'",buff,5);
    if(ret!=1)
    {
        return;
    }
    /*再把.去掉*/
    ssplit(buff[0], ".",str, &num);
    if(num!=0)
    {
        allcontainerinfo.dev_run.mem_usage = atoi(str[0]);
    }    
    memset(buff,0,sizeof(buff));
    ret = getshellresult("df -h / | grep rootfs | awk '{print $5*100}'",buff,5);
    if(ret!=1)
    {
        return;
    }
    allcontainerinfo.dev_run.disk_usage = atoi(buff[0]);
    memset(buff,0,sizeof(buff));
    ret = getshellresult("cat /sys/class/thermal/thermal_zone0/temp",buff,5);
    if(ret!=1)
    {
        return;
    }
    cpu0temp = atoi(buff[0])/100;
    memset(buff,0,sizeof(buff));
    ret = getshellresult("cat /sys/class/thermal/thermal_zone1/temp",buff,5);
    if(ret!=1)
    {
        return;
    }
    cpu1temp = atoi(buff[0])/100;
    allcontainerinfo.dev_run.cputemp =  cpu0temp>cpu1temp?cpu0temp:cpu1temp;
}

/**********************************************************************
* @name      : iot_data_upDate_init
* @brief     : 数据更新线程初始化
* @param[in] : pPublic 公共结构体  
* @param[out]: ppPrivate线程结构体
* @return    : 
* @Create    : 
* @Date      : 2019-10-30
* @Update    :
**********************************************************************/

int iot_data_upDate_init(void *pPublic, void **ppPrivate)
{
    IOT_UPDATE_RUN *ptData = NULL;
    IOT_APPDATA_T* pData = NULL; 
    pData = pPublic;
    ptData = malloc(sizeof(IOT_UPDATE_RUN));
    if(NULL == ptData)
    {
        return -1;
    }
    memset(ptData, 0, sizeof(IOT_UPDATE_RUN));
    ptData->update_time = time(NULL);
    pthread_mutex_init(&allcontainerinfo.lock, NULL);
    memset(&allcontainerinfo,0x00,sizeof(ALL_CONTAINER_AND_APP_T));
    
    UPDATE_FMT_DEBUG("data update time %d sec \n",pData->app_t.updateTime);
    ptData->type = 1;
    *ppPrivate = ptData;
    return 0;
}


/**********************************************************************
* @name      : iot_data_upDate_proc
* @brief     : 数据更新线程主函数
* @param[in] : pPublic 公共结构体  ppPrivate线程结构体
* @param[out]: 
* @return    : 
* @Create    : 
* @Date      : 2019-10-30
* @Update    :
**********************************************************************/

int iot_data_upDate_proc(void *pArg,void *pPublic, void *pPrivate)
{
    IOT_UPDATE_RUN *ptData = pPrivate;
    IOT_APPDATA_T * pbData = pPublic;
    if(time(NULL)<= ptData->update_time )
    {
        ptData->update_time = time(NULL);
        return 100000;
    }
        
    if(time(NULL)-ptData->update_time <pbData->app_t.updateTime)
        return 100000;
    UPDATE_FMT_DEBUG("start update \n");
    ptData->update_time = time(NULL);
    pbData->init_type=0;
    Normal_oper_time++;
    if(Normal_oper_time ==30)
    {
        Normal_oper_time =0;
        UPDATE_FMT_DEBUG("Update thread Normal runing 30 period \n");
    }
    //更新数据
    uint8 i =0,dockernum = 0,j=0;
    
    
    ALL_CONTAINER_INFO_T AllContainer;
    memset(&AllContainer,0x00,sizeof(ALL_CONTAINER_INFO_T));
    get_container_info(&AllContainer);
    dockernum = AllContainer.containercount;
    if(dockernum==0 || dockernum>DOCKERMAXLEN)
    {
       UPDATE_FMT_DEBUG("container num  error %d \n",dockernum); 
       ptData->update_time = time(NULL);
       return 1000;
    }
    pthread_mutex_lock(&allcontainerinfo.lock);
    uint8 tmpnum = allcontainerinfo.count;
    UPDATE_FMT_DEBUG("dockernum %d  tmpnum %d \n",dockernum,tmpnum);
    if(allcontainerinfo.count == 0)  //首次
    {
        memset(&allcontainerinfo,0,sizeof(allcontainerinfo));
        allcontainerinfo.count = AllContainer.containercount;
        for(i=0;i<AllContainer.containercount;i++)
        {
            memcpy(&allcontainerinfo.containers[i].container,&AllContainer.container[i],sizeof(EVERY_CONTAINER_T));
            
            get_app_info((char*)AllContainer.container[i].ContainerName,&allcontainerinfo.containers[i].apps);
        }
    }
    else if(dockernum < tmpnum)  //容器有删除
    {
         uint8 j =0,lable;
        for(i=0;i<tmpnum;i++)
        {
          //  char*  lastcon =(char *)allcontainerinfo.containers[i].container;
         //   lastcon.container
            for(j=0;j<dockernum;j++)
            {
                lable = 0;
             //   char *nowcon = (char *)AllContainer.container[j];
                if(strcmp((char *)allcontainerinfo.containers[i].container.ContainerName,(char *)AllContainer.container[j].ContainerName)==0)
                {
                    lable =1 ;
                    break;
                }
            }
            if(lable == 0)
            {
                //去除全局变量中序号为i的值
                uint8 k = 0;
               
                for(k=i;k<tmpnum-1;k++)
                {
                     memset(&allcontainerinfo.containers[k],0,sizeof(CONTAINER_AND_APP_T));
                     memcpy(&allcontainerinfo.containers[k],&allcontainerinfo.containers[k+1],sizeof(CONTAINER_AND_APP_T));
                }
                allcontainerinfo.count--;
                memset(&allcontainerinfo.containers[tmpnum-1],0,sizeof(CONTAINER_AND_APP_T));
            }
        }
    }
    else{
        uint8 j =0,lable;
        for(i=0;i<dockernum;i++)
        {
            for(j=0;j<allcontainerinfo.count;j++)
            {
                lable = 0;
                char*conname =(char *)allcontainerinfo.containers[j].container.ContainerName;
                if(strcmp((char*)AllContainer.container[i].ContainerName,conname)==0)
                {
                    memcpy(&allcontainerinfo.containers[j].container,&AllContainer.container[i],sizeof(EVERY_CONTAINER_T));
                    get_app_info(conname,&allcontainerinfo.containers[j].apps);
                    lable= 1;
                    break;
                }
            }
            if(lable!=1)
            {
                uint8 num = allcontainerinfo.count;
                memcpy(&allcontainerinfo.containers[num].container,&AllContainer.container[i],sizeof(EVERY_CONTAINER_T));
                get_app_info((char *)allcontainerinfo.containers[j].container.ContainerName,&allcontainerinfo.containers[num].apps);
                allcontainerinfo.count++;
            }
            UPDATE_FMT_DEBUG("ContainerName--%s cpu %d mem %d disk %d \n",AllContainer.container[i].ContainerName,AllContainer.container[i].cpuLimit,AllContainer.container[i].memLimit,AllContainer.container[i].stroLimit);
        }
        
    }     
    get_dev_runinfo();
    if((dockernum != tmpnum && dockernum != 0) || ptData->type==1 )
    {
        UPDATE_FMT_DEBUG("value update container count %d \n",allcontainerinfo.count);
        uint8 k = 0;
        for(k=0;k<allcontainerinfo.count;k++)
        {
            UPDATE_FMT_DEBUG("container name %s\n",allcontainerinfo.containers[k].container.ContainerName);
            for(j=0;j<allcontainerinfo.containers[k].apps.appcount;j++)
            {
                UPDATE_FMT_DEBUG("App name %s\n",allcontainerinfo.containers[k].apps.appinfo[j].appname);
            }
        }
    }
    pthread_mutex_unlock(&allcontainerinfo.lock);
    /*系统检测*/
 //   ptData->update_time = time(NULL);
    if(ptData->type==1)
    {        
        ptData->type = 0;
    }
    pbData->init_type=1;
    return 100000;
}



