/*
*********************************************************************
* Copyright(C) 2020 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   回路监测主线程功能
* @date：    2020-6-30
* @history： 
*********************************************************************
*/
#include "iot_init.h"
#include "iot_public.h"
#include "iot_msg.h"
#include "sm3sum.h"
#include <stdlib.h>

char PrivateDir[50]="/data/app/iotManager/data/";
char PackLogDir[50]="/log/sys/";
char SysLogDir[50]="/log/SysLog/"; 
uint16 packnum = 0;


void print_exec_info(char *cmd,char p[][500],int  num)
{
    if (num <= 0)
        return;
    MQTTMSG_FMT_DEBUG("命令执行 :\n %s \n",cmd); 
    MQTTMSG_FMT_DEBUG("返回值 : \n"); 
    uint8 i=0;
    for(i=0;i<num;i++)
    {
        MQTTMSG_FMT_DEBUG("%s\n",p[i]);
    }
}
#if 0
bool inspct_cmd_returnvalue(char p[][500],uint8 num,uint8 type)
{
    if(num == 0)
        return TRUE;
    uint8 i;
    bool ret = TRUE;
    switch(type)
    {
        case 0: //卸载容器
        {
          for(i=0;i<num;i++)
          {
                if(strstr(p[i],CON_UNINDTATALL_VALUE) != NULL)
                {
                    ret = TRUE;
                    break;
                }
          }
        }break;
        case 1: //安装容器
        {

        }break;
        default:
            break;
    }
    return ret;
}
#endif

/**********************************************************************
* @name      : packge_report_msg
* @brief     ：组回复报文
* @param[in] ：MSG_INFO_T *MsgRecv 消息报文 buff 回复的报文 msglen 回复报文长度
* @param[out]：
* @return    ：
* @Create    : 
* @Date      ：2020-5-22
* @Update    :
**********************************************************************/

void packge_report_msg(MSG_INFO_T *MsgRecv,uint8 *buff,uint16 msglen)
{

    MSG_INFO_T *MsgSend = (MSG_INFO_T*)malloc(sizeof(MSG_INFO_T)+msglen);
    memset(MsgSend,0x00,sizeof(MSG_INFO_T)+msglen);
    MsgSend->MsgRPM = 0;
    MsgSend->MsgPRIORITY = MsgRecv->MsgPRIORITY;
    MsgSend->MsgIndex = MsgRecv->MsgIndex;
    MsgSend->MsgLabel = MsgRecv->MsgLabel;
    uint32 MSGLen = 0;
    uint8 *MSGBuf= NULL;
    MsgSend->SourLen = strlen(WUGUAN_APP_NAME_M);
    memcpy(MsgSend->SourAddr,WUGUAN_APP_NAME_M,MsgSend->SourLen);

    MsgSend->DestLen = MsgRecv->SourLen;
    memcpy(MsgSend->DestAddr,MsgRecv->SourAddr,MsgSend->DestLen);
    MsgSend->IID = MsgRecv->IID;
    MsgSend->IOP = MsgRecv->IOP;
    MsgSend->MsgLen = msglen;
    memcpy(MsgSend->MsgData,buff,msglen);
 //   taskmng_plc_queue_write (&LockRecvMsg, (uint8*)MsgSend, sizeof(MSG_INFO_T)+MsgSend->MsgLen );
    //直接发送确认帧
    char topic[1000]={0};
    sprintf(topic,"%s/%s",MsgSend->SourAddr,MsgSend->DestAddr);
    //组消息
    COMM_FMT_DEBUG("topic %s IID %04x IOP %04x\n",topic,MsgSend->IID,MsgSend->IOP);
    MSGLen = iot_appmsg_package(MsgSend,&MSGBuf);    
    COMM_BUF_DEBUG(MSGBuf, MSGLen,"");
    mqtt_topic_write(MSGBuf, MSGLen,topic); 
}

/**********************************************************************
* @name      : get_alldocker_with_app
* @brief     ：消息App操作命令执行
* @param[in] ：MSG_INFO_T *MsgRecv 消息,int MSGLen 长度
* @param[out]：
* @return    ：
* @Create    : 
* @Date      ：2020-5-22
* @Update    :
**********************************************************************/

int get_alldocker_with_app(MSG_INFO_T *MsgRecv,ALL_APP_INFO_T *app_t)
{
    uint8 count=0,i=0,j=0;
    SEQ_STR app;
    uint8 index=0;
    char cmd[1024]={0};
    uint8 *data = MsgRecv->MsgData;
    char buff[20][500];
    char dockerbuff[20][500];
    memset(buff,0,sizeof(buff));
    memset(dockerbuff,0,sizeof(dockerbuff));
    memset(&app,0,sizeof(SEQ_STR));
    count = getshellresult("docker ps -a --format '{{.Names}}'",dockerbuff,20);
    if(count==0)
    {
        return -1;
    }
    index++;
    app.len=data[index++];
    memcpy(app.buff,&data[index],app.len);
    index+=app.len;
/*    if(index!=MsgRecv->MsgLen)
    {
        return -1; 
    }*/
    for(i=0;i<count;i++)
    {
        memset(cmd,0,sizeof(cmd));
        sprintf(cmd,"appm -I -c %s -n %s",dockerbuff[i],app.buff);
        if(getshellresult(cmd,buff,20)>1)
        {
           app_t->appcount++; 
           strcpy((char *)app_t->appinfo[j].containername,dockerbuff[i]);
           strcpy((char *)app_t->appinfo[j].appname,(char *)app.buff);
           j++;
        }
    }
    return index;
}

/**********************************************************************
* @name      : get_allapp_in_docker
* @brief     : 获取容器内所有app的信息
* @param[in] ：MSG_INFO_T *MsgRecv 消息,int MSGLen 长度
* @param[out]：
* @return    ：
* @Create    : 
* @Date      ：2020-5-22
* @Update    :
**********************************************************************/

uint8 get_allapp_in_docker(ALL_APP_INFO_T *app_t,char *dockername)
{
    char cmd[1024]={0};
    uint8 count=0,i=0;
    char buff[20][500];
    memset(buff,0,sizeof(buff));
    if(dockername==NULL || app_t == NULL)
    {
        return 1;
    }
    sprintf(cmd,"appm -I -c %s | grep 'APP name' | awk '{print $4}'",dockername);
    count = getshellresult(cmd,buff,100);
    print_exec_info(cmd,buff,count);
    if(count== 0)
    {
        return 2;
    }
    for(i=0;i<count;i++)
    {
        strcpy((char *)app_t->appinfo[app_t->appcount].appname,buff[i]);
        strcpy((char *)app_t->appinfo[app_t->appcount].containername,dockername);
		app_t->appcount ++;
    }
    return 0;
}

/**********************************************************************
* @name      : app_action_notice
* @brief     ：App告警通知
* @param[in] ：docker 容器 app app ,type操作类型{安装（0），升级（1），卸载（2），启动（4），停止（4），配置参数（5），使能,6），去使能（7），下发许可（8）}，
* @param[out]：
* @return    ：
* @Create    : 
* @Date      ：2020-5-22
* @Update    :
**********************************************************************/

void app_action_notice(SEQ_STR *docker,SEQ_STR *app,uint8 type,uint8 errcode)
{
    uint16 index = 0;
    uint8 buff[500]={0};
    OOP_DATETIME_S_T StartTime;
    memset(&StartTime,0x00,sizeof(OOP_DATETIME_S_T));
    buff[index++]=docker->len;
    memcpy(&buff[index],docker->buff,docker->len);
    index+=docker->len;
    buff[index++]=app->len;
    memcpy(&buff[index],app->buff,app->len);
    index+=app->len;
    DT_Time2DateTimeShort(time(NULL),&StartTime);
    memcpy_r(&buff[index],&StartTime.year,sizeof(uint16));
    index+=sizeof(uint16);
    buff[index++]=StartTime.month;
    buff[index++]=StartTime.day;
    buff[index++]=StartTime.hour;
    buff[index++]=StartTime.minute;
    buff[index++]=StartTime.second;
    buff[index++] = type;
    buff[index++] = !errcode;
    buff[index++] = 0x00;

    
    MSG_INFO_T* msg  = malloc(sizeof(MSG_INFO_T)+index);
    
    if(msg==NULL)
        return;
    memset(msg,0,sizeof(MSG_INFO_T)+index);
    msg->MsgLen = index;
    msg->MsgRPM = 1;
    msg->MsgPRIORITY = 1;
    msg->MsgIndex = iot_get_next_index();
    msg->MsgLabel = 0;
    uint8 *MSGBuf=NULL;
    uint16 MSGLen=0;
    char topic[1000]={0};
    msg->SourLen = strlen(WUGUAN_APP_NAME_M);
    memcpy(msg->SourAddr, WUGUAN_APP_NAME_M, msg->SourLen);

    msg->DestLen = strlen("Broadcast");
    memcpy(msg->DestAddr, "Broadcast", msg->DestLen);
    msg->IID = MESSAGE_IID_IOT;
    msg->IOP = IOT_IOP_APP_ACTION;
    
    memcpy(msg->MsgData,buff,msg->MsgLen);
    sprintf(topic,"%s/%s/%04X/%04X",msg->SourAddr,msg->DestAddr,msg->IID,msg->IOP);
    MSGLen = iot_appmsg_package(msg,&MSGBuf);    
    MQTTMSG_BUF_DEBUG(MSGBuf, MSGLen,"");
    mqtt_topic_write(MSGBuf, MSGLen,topic); 
   // taskmng_plc_queue_write (&LockWarringMsg, (uint8*)msg, sizeof(MSG_INFO_T)+msg->MsgLen);
    free(msg);
}

/**********************************************************************
* @name      : App_comand_exec
* @brief     ：执行App的一系列操作
* @param[in] ：docker 容器 app app ,type操作类型enum{启动（0），停止（1），卸载（2），使能（3），去使能（4）}
* @param[out]：
* @return    ：
* @Create    : 
* @Date      ：2020-5-22
* @Update    :
**********************************************************************/

BOOL App_comand_exec(SEQ_STR *docker,SEQ_STR *app,uint8 type)
{

    if(docker->buff==NULL || app->buff==NULL || type >4 )
    {
        return FALSE;
    }
    char cmd[1024]={0};
    char buff[20][500]={{0}};
    uint8 errcode=TRUE;
    
    switch (type)
    {
        case  0: // 启动
        {
            sprintf(cmd,"appm -s -c %s -n %s | grep started",docker->buff,app->buff);
        }break;
        case 1: //停止
        {
            sprintf(cmd,"appm -S -c %s -n %s | grep stopped",docker->buff,app->buff);
        }break;
        case 2: //卸载
        {
            sprintf(cmd,"appm -u -c %s -n %s | grep Uninstalled",docker->buff,app->buff);
        }break;
        case 3:  //使能
        {
            sprintf(cmd,"appm -e -c %s -n %s | grep enabled",docker->buff,app->buff);
        }break;
        case 4: //去使能
        {
            sprintf(cmd,"appm -d -c %s -n %s | grep disenabled",docker->buff,app->buff);
        }break;
        default:
            break;
    }
	//判断如果时启动需要看这个app有没有软件注册码 ，存在注册码且为36个0的时候，不启动回复错误 台体测试应用管理
    if(type == 0)
    {
     //   app_license_T liscen;
     //   memset(&liscen,0,sizeof(liscen));
    }
    uint8 ret = 0;
    ret = getshellresult(cmd,buff,20);
    MQTTMSG_FMT_DEBUG("执行APP操作 CMD %s ret %d \n",cmd,ret);
    print_exec_info(cmd, buff,ret);
    if(ret ==0)
    {
        
        errcode = FALSE;
        MQTTMSG_FMT_DEBUG("recv cmd %s\n",buff[0]);
    }
    //再验证一下
    //取消复检，认为只要返回没有问题，就确认无问题。
    
    return errcode;
}

uint16 get_containerinfo_to_out(uint8 *payload,uint16 index,uint16 j,ALL_CONTAINER_INFO_T *allcontainerinfo)
{
    uint16 tmp=0;
    tmp = strlen((char *)allcontainerinfo->container[j].ContainerID);
    payload[index++] = tmp;
    memcpy(&payload[index],allcontainerinfo->container[j].ContainerID,tmp);
    index+=tmp;
    payload[index++] = strlen((char *)allcontainerinfo->container[j].ContainerName);
    memcpy(&payload[index],(char *)allcontainerinfo->container[j].ContainerName,strlen((char *)allcontainerinfo->container[j].ContainerName));
    index+=strlen((char *)allcontainerinfo->container[j].ContainerName);
    payload[index++] = allcontainerinfo->container[j].CpuCores;
    memcpy_r(&payload[index],&allcontainerinfo->container[j].cpuLimit,sizeof(uint16));
    index+=sizeof(uint16);
    uint16 tmpcode = 0;
    tmpcode = atoi((char *)allcontainerinfo->container[j].MemLimit);
    memcpy_r(&payload[index],&tmpcode,sizeof(uint16));
    index+=sizeof(uint16);
    memcpy_r(&payload[index],&allcontainerinfo->container[j].memLimit,sizeof(uint16));
    index+=sizeof(uint16);
    tmpcode = atoi((char *)allcontainerinfo->container[j].StorageLimit);
    memcpy_r(&payload[index],&tmpcode,sizeof(uint16));
    index+=sizeof(uint16);
    memcpy_r(&payload[index],&allcontainerinfo->container[j].stroLimit,sizeof(uint16));
    index+=sizeof(uint16);
    uint8 filePathnum = strlen((char *)allcontainerinfo->container[j].FilePath);
    uint8 tmpbuf[3]={0};
    uint8 tmpindex=0;
    tmpindex = set_len_offset(filePathnum, tmpbuf);
    memcpy(&payload[index],tmpbuf,tmpindex);
    index+=tmpindex;
    memcpy(&payload[index],allcontainerinfo->container[j].FilePath,filePathnum);
    index+=filePathnum;
    tmp=0;
    tmp = strlen((char *)allcontainerinfo->container[j].DownloadFilePath);
    payload[index++] = tmp;
    memcpy(&payload[index],allcontainerinfo->container[j].DownloadFilePath,tmp);
    index+=tmp;
    uint8 Interfacenum = strlen((char *)allcontainerinfo->container[j].Interface);
    payload[index++] = Interfacenum;
    memcpy(&payload[index],allcontainerinfo->container[j].Interface,Interfacenum);
    index+=Interfacenum;
    tmp  = strlen((char *)allcontainerinfo->container[j].PortConfig);
    payload[index++] = tmp;
    memcpy(&payload[index],allcontainerinfo->container[j].PortConfig,tmp);
    index+=tmp;           //端口资源配置参数
    tmp=0;
    tmp = strlen((char *)allcontainerinfo->container[j].ContainerState);
    payload[index++] = tmp; 
    memcpy(&payload[index],allcontainerinfo->container[j].ContainerState,tmp);
    index+=tmp;
    memcpy_r(&payload[index],&allcontainerinfo->container[j].CpuRate,sizeof(uint16));
    index+=sizeof(uint16);
    memcpy_r(&payload[index],&allcontainerinfo->container[j].MenRate,sizeof(uint16));
    index+=sizeof(uint16);
    memcpy_r(&payload[index],&allcontainerinfo->container[j].StorageRate,sizeof(uint16));
    index+=sizeof(uint16);
    payload[index++]=4;
    payload[index++]=allcontainerinfo->container[j].ContainerIP[0];
    payload[index++]=allcontainerinfo->container[j].ContainerIP[1];
    payload[index++]=allcontainerinfo->container[j].ContainerIP[2];
    payload[index++]=allcontainerinfo->container[j].ContainerIP[3];
    tmp=0;
    tmp = strlen((char *)allcontainerinfo->container[j].ImageName);
    payload[index++] = tmp; 
    memcpy(&payload[index],allcontainerinfo->container[j].ImageName,tmp);
    index+=tmp;
    memcpy_r(&payload[index],&allcontainerinfo->container[j].CreateTime.year,sizeof(uint16));
    index+=sizeof(uint16);
    payload[index++]=allcontainerinfo->container[j].CreateTime.month;
    payload[index++]=allcontainerinfo->container[j].CreateTime.day;
    payload[index++]=allcontainerinfo->container[j].CreateTime.hour;
    payload[index++]=allcontainerinfo->container[j].CreateTime.minute;
    payload[index++]=allcontainerinfo->container[j].CreateTime.second;
    memcpy_r(&payload[index],&allcontainerinfo->container[j].StartUpTime.year,sizeof(uint16));
    index+=sizeof(uint16);
    payload[index++]=allcontainerinfo->container[j].StartUpTime.month;
    payload[index++]=allcontainerinfo->container[j].StartUpTime.day;
    payload[index++]=allcontainerinfo->container[j].StartUpTime.hour;
    payload[index++]=allcontainerinfo->container[j].StartUpTime.minute;
    payload[index++]=allcontainerinfo->container[j].StartUpTime.second; 
    memcpy_r(&payload[index],&allcontainerinfo->container[j].RunTime,sizeof(uint32));
    index+=sizeof(uint32);
    return index;
}


uint16 packge_0013_report(SEQ_STR *con,uint8 *payload,ALL_CONTAINER_INFO_T *allcontainerinfo)
{
    uint16 index = 0,j=0;
    if(allcontainerinfo->containercount==0)
    {
        payload[index++]=0;
        return index;
    }
    if(con->len == 0)
    {
        payload[index++] = allcontainerinfo->containercount;
    }
    for(j=0;j<allcontainerinfo->containercount;j++)
    {
        if(con->len == 0)
        {
            //全部容器
               index = get_containerinfo_to_out(payload,index,j,allcontainerinfo);
        }
        else
        {
            if(strcmp((char *)con->buff,(char *)allcontainerinfo->container[j].ContainerName)==0)
            {
                payload[index++]=1;
                index =  get_containerinfo_to_out(payload,index,j,allcontainerinfo);
                break;
            } 
        }
      
    }

    return index;
}

/**********************************************************************
* @name      : pack_accord_logPath
* @brief     : 根据日志类型操作函数
* @param[in] ：MSG_INFO_T *MsgRecv 消息,int MSGLen 长度
* @param[out]：
* @return    ：
* @Create    : 
* @Date      ：2020-5-22
* @Update    :
**********************************************************************/
void operate_accord_logType(uint8 type,char *source)
{
    sprintf(source,"/log/sys/messages");
}

int judge_path_fold_dir(char *path)
{
    if(path==NULL)
        return -1;
    struct stat buf;
    stat(path,&buf);
    if(S_IFDIR& buf.st_mode)
    {
        return 1;
    }else if(S_IFREG & buf.st_mode)
    {
        return 0;
    }else if(strstr(path,".gz")!=NULL)
    {
        return 0;
    }
   return -1;
}

/**********************************************************************
* @name      : pack_accord_logPath
* @brief     : 根据日志路径操作函数
* @param[in] : exec 操作类型 0 读取信息 1. 打包 2. 删除 filePath 操作的文件的路径 
* @param[out]：recvBuf 回复报文，msglen 报文长度
* @return    ：0 成功 -1 失败
* @Create    : 
* @Date      ：2020-5-22
* @Update    :
**********************************************************************/
int operate_accord_logPath(char *filePath,char *packfile)
{
    char cmd[1024]={0};
    char buff[10][500]={{0}};
    int ret = 0;
    
    sprintf(cmd,"log -t -lnp %s -tlnp %s ",filePath,packfile);
    ret = getshellresult(cmd,buff,10);
    print_exec_info(cmd,buff,ret);
    
#ifdef PRODUCT_ECU
    memset(cmd, 0, 1024);
    sprintf(cmd, "gzip -c %s > %s", filePath, packfile);
    system(cmd);
#endif

    return 0;
}

/**********************************************************************
* @name      : get_logFile_info
* @brief     : 根据日志路径填充log结构体函数
* @param[in] : path 日志路径
* @param[out]: log 填充后的log结构体
* @return    : 字符串时间标准:  1970-01-01 00:00:00
* @Create    : 
* @Date      ：2020-5-22
* @Update    :
**********************************************************************/

int get_logFile_info(log_info_T *log,char *path,uint8 type)
{
    //修改为用log命令获取
    char cmd[1024] = {0};
    char result[15][500];
    int ret,i=0;
    if(type != 0xFF)
    {
        sprintf(cmd,"log -i -lt %d",type);
    }
    else 
    {
        sprintf(cmd,"log -i -lnp %s",path);
    }
    ret =  getshellresult(cmd,result,15) ;
    print_exec_info(cmd,result, ret);
    
    if(ret > 2)
    {
        for(i=0;i < ret;i++)
        {
            if(strlen(result[i]) == 0)
            {
                i++;
                continue;
            }
     //       MQTTMSG_FMT_DEBUG("i %d ret %d \n",i ,ret);
            char key[50]={0};
            char value[50]={0};
            char *p = strstr(result[i],":");
            if(p==NULL)
            {
                continue;
            }
            memcpy(key,result[i],p-result[i]);
            memcpy(value,p+1,strlen(p)-1);
            char* keytrim = strim(key);
            char* valuetrim = strim(value);
            if(strcmp(keytrim,"log_cnt")==0)
            {
                log->lognum = atoi(valuetrim);
            }
            if(strcmp(keytrim,"file")==0 && strlen(valuetrim)>0)
            {
                strcpy(log->log[0].logPath,valuetrim);
            }
            if(strcmp(keytrim,"size")==0 && strlen(valuetrim)>0)
            {
                //log->log[0].logSize = atoi(valuetrim);
                continue;
            }
            if(strcmp(keytrim,"md5")==0 && strlen(valuetrim)>0)
            {
//                strcpy(log->log[0].logMd5,valuetrim);
//                log->log[0].md5len = strlen(log->log[0].logMd5);  //24规范采用SM3编码，本段函数后面手动计算
                continue;
            }
            if(strcmp(keytrim,"create_time")==0 && strlen(valuetrim)>0)
            {
                sscanf(valuetrim,"%hu-%u-%u %u:%u:%u",&log->log[0].logCreat.year,(int *)&log->log[0].logCreat.month,(int *)&log->log[0].logCreat.day,(int *)&log->log[0].logCreat.hour,(int *)&log->log[0].logCreat.minute,(int *)&log->log[0].logCreat.second);
            }
            if(strcmp(keytrim,"type")==0)
            {
                if(type != 0xFF)
                {
                    log->log[0].logType = type;
                }
                else
                {
                    log->log[0].logType = atoi(valuetrim);
                }
            }
           
            if(strcmp(keytrim,"container_name")==0&& strlen(valuetrim)>0)
            {
                strcpy(log->log[0].conName,valuetrim);
            }
            if(strcmp(keytrim,"app_name")==0&& strlen(valuetrim)>0)
            {
                strcpy(log->log[0].appName,valuetrim);
            }
            
        }
        char buff[10][500];
        memset(buff,0,sizeof(buff));
        memset(cmd,0,sizeof(cmd));
        sprintf(cmd,"wc -c %s | awk '{print$1}'",log->log[0].logPath);
        if(getshellresult(cmd,buff,1)!=1)
            return 1;
        
        log->log[0].logSize = atoi(buff[0]);
        MQTTMSG_FMT_DEBUG("日志大小 ：%d\n", log->log[0].logSize);
        
        ret = GetFileSM3(log->log[0].logPath, 32, (uint8 *)log->log[0].logMd5);
        if(ret != 0)
        {
            MQTTMSG_FMT_DEBUG("获取[%s]SM3编码失败, %s\n", ret == -2 ? "文件不存在" : "其他原因");
        }
        else
        {
            log->log[0].md5len = 32;
            MQTTMSG_BUF_DEBUG(log->log[0].logMd5, log->log[0].md5len, "获取[%s]SM3编码成功", log->log[0].logPath);
        }
    }
    else
    {
        
    }
    return 1;
}

uint16 pack_report_fileinfo(log_info_T *log,uint8 *recv)
{
    uint16 index = 0,i=0;
    recv[index++] = log->lognum;
    for(i=0;i<log->lognum;i++)
    {
        recv[index++] = strlen(log->log[i].logPath);
        memcpy(&recv[index],log->log[i].logPath,strlen(log->log[i].logPath));
        index+=strlen(log->log[i].logPath);
        memcpy_r(&recv[index],&log->log[i].logSize,sizeof(uint32));
        index+=sizeof(uint32);
        if(log->log[i].md5len==0)
        {
            recv[index++] = 0;
        }
        else
        {
            if(log->log[i].md5len == 32)
            {
                recv[index++] = 1;
                recv[index++] = log->log[i].md5len;
                memcpy(&recv[index],(uint8 *)log->log[i].logMd5, log->log[i].md5len);
                index += 32;
            }
            else
            {
                //MD5转换
                uint8 md5[100]={0};
                uint8 md5len =0;
                
                md5len = strt2octet(log->log[i].logMd5,log->log[i].md5len,md5,64);
                if(md5len > 0)
                {
                    MQTTMSG_BUF_DEBUG(md5,md5len,"转换后的md5值 ：");
                }
                recv[index++] = 1;
                recv[index++] = md5len;
                memcpy(&recv[index],md5,md5len);
                index+=md5len;
            }
        }
        memcpy_r(&recv[index],&log->log[i].logCreat.year,sizeof(uint16));
        index+=sizeof(uint16);
        recv[index++] = log->log[i].logCreat.month;
        recv[index++] = log->log[i].logCreat.day;
        recv[index++] = log->log[i].logCreat.hour;
        recv[index++] = log->log[i].logCreat.minute;
        recv[index++] = log->log[i].logCreat.second;
        recv[index++] = log->log[i].logType;
        if(strlen(log->log[i].conName)!=0)
        {
            recv[index++] = 1;
            recv[index++] = strlen(log->log[i].conName);
            memcpy(&recv[index],log->log[i].conName,strlen(log->log[i].conName));
            index+=strlen(log->log[i].conName);
        }
        else
        {
            recv[index++] = 0;
        }
        if(strlen(log->log[i].appName)!=0)
        {
            recv[index++] = 1;
            recv[index++] = strlen(log->log[i].appName);
            memcpy(&recv[index],log->log[i].appName,strlen(log->log[i].appName));
            index+=strlen(log->log[i].appName);
        }
        else
        {
            recv[index++] = 0;
        }
    }
    return index;
}

BOOL judge_path_exec(char *logFile,uint8 type)
{
    char cmd[1024] = {0};
    char buff[5][500];
    uint8 ret =0;
    memset(buff,0,sizeof(buff));
    if(type !=0xFF)
    {
        sprintf(cmd,"log -d -lt %d",type);
    }
    else
    {
        sprintf(cmd,"log -d -lnp %s",logFile);
    }
    ret = getshellresult(cmd,buff,5);
    print_exec_info(cmd,buff,ret);
    if(strcmp(buff[0],LOG_DELETE_VALUE)==0)
    {
        MQTTMSG_FMT_DEBUG("日志删除成功 \n");
    }

    return TRUE;
}

void get_coninfo_all(ALL_CONTAINER_INFO_T* allcontainer)
{
    int count=0;
    uint8 i,realnum=0,j=0;
    char buff[20][500];
    memset(buff,0,sizeof(buff));
    char cmd[500] = "docker ps --format '{{.ID}}#{{.Names}}#{{.Mounts}}#{{.Ports}}#{{.Image}}'";
    count = getshellresult(cmd,buff,20);
    if(count == 0)
    {
        MQTTMSG_FMT_DEBUG("ERROR 没有检测到容器 \n");
        return ;
    }
    allcontainer->containercount = count;
    for(i=0;i<count;i++)
    {
        char str[50][500];
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
            MQTTMSG_FMT_DEBUG("ERROR 命令执行没有返回或返回不正确\n %s \n",cmd);
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
//#ifdef PRODUCT_CCU
        
        sprintf(cmd,"container monitor -d %s | grep limit",allcontainer->container[i].ContainerName);
        int num =0;
        do
        {

            ret = getshellresult(cmd,buff,DOCKERMAXLEN);

            if(ret ==3)
             {
//             UPDATE_FMT_DEBUG("ERROR 命令执行没有返回或返回不正确\n ret %d\n %s \n", ret ,cmd);
               break;
             }
            sleep(1);
            num ++;
        }
        while (num  <=3);

//      ret = getshellresult(cmd,buff,DOCKERMAXLEN);

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
           MQTTMSG_FMT_DEBUG("ERROR 命令执行没有返回或返回不正确\n ret %d\n %s \n", ret ,cmd);
           continue;
         }
         
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
* @name      : get_app_info
* @brief     : 获取App的信息
* @param[in] : containername 容器名字 
* @param[out]：appinfo       app信息
* @return    : 
* @Create    : 
* @Date      : 2019-10-30
* @Update    :
**********************************************************************/

void get_app_info(char* containername,char* appname,ALL_APP_INFO_T* appinfo,uint8 appindex)
{
    int count;
    uint8 i;
    char buff[100][500];
    memset(buff,0,sizeof(buff));
    char cmd[50]={0};
    if(NULL == containername || 0 == strlen(containername)||NULL == appname || 0 == strlen(appname))
        return ;
    sprintf(cmd,"appm -I -c %s -n %s",containername,appname);
    count = getshellresult(cmd,buff,100);
    if(count<=1)
        return;
    i = 1;
    uint32 memlimit = 300*1024 ;
    appinfo->appinfo[appindex].servicecount = 0;
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
        if(strcmp(keytrim,"APP index")==0)
        {
            appinfo->appinfo[appindex].appindex = atoi(valuetrim);
         //   UPDATE_FMT_DEBUG("index %d \n\n",appinfo->appinfo[appindex].appindex);
        }
        else if(strcmp(keytrim,"APP name")==0)
        {
            strcpy((char*)appinfo->appinfo[appindex].appname,valuetrim);
            //UPDATE_FMT_DEBUG("name %s\n\n",appinfo->appinfo[appindex].appname);
        }
        else if(strcmp(keytrim,"APP version")==0)
        {
            strcpy((char*)appinfo->appinfo[appindex].appversion,valuetrim);
        //    UPDATE_FMT_DEBUG("version %s\n\n",appinfo->appinfo[appindex].appversion);
        }
        else if(strcmp(keytrim,"APP hash")==0)
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
            if(strcmp(valuetrim,"yes")==0)
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
        
        else if(strcmp(keytrim,"MEM Limit")==0)
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
                memlimit = 300*1024;
            }
        }
        else if(strcmp(keytrim,"MEM threshold")==0)
        {
           appinfo->appinfo[appindex].memLimit = percentdatachange(valuetrim);
        }

        else if(strcmp(keytrim,"MEM usage")==0)
        {
            appinfo->appinfo[appindex].serviceinfo[0].memrate = percentdatachange(valuetrim);
        }

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

BOOL judge_linsence_legal(SEQ_STR *app)
{
    uint8 i=0,ret=0;
    uint32 count=0;
    BOOL result = TRUE;
    app_license_T liscen;
    memset(&liscen,0,sizeof(liscen));
    DEVINFO_DEVICE_T* dev = (DEVINFO_DEVICE_T*) hal_device_get(HW_DEVICE_ID_DEVINFO);
    if(NULL == dev)
    {
        MQTTMSG_FMT_DEBUG("devinfo hal_device_get error \n");
    }
    else
    {
        count = dev->devinfo_applicense_count(dev);
        if(count == 0)
        {
            MQTTMSG_FMT_DEBUG("devinfo devinfo_applicense_count error count %d\n", count);
        }
        else
        {
            MQTTMSG_FMT_DEBUG("devinfo devinfo_applicense_count  %d\n", count);
        }

		for(i=0; i<count; i++)
		{
			ret = dev->devinfo_read_applicense_index(dev, i, liscen.licen[liscen.licennum].appnum, liscen.licen[liscen.licennum].appname, liscen.licen[liscen.licennum].applicen, 100);
	        if(ret == 0)
	        {
	            liscen.licennum ++;
	        }
	        else
	        {
	            MQTTMSG_FMT_DEBUG(" devinfo_read_applicense_index error index%d ret%d !\n", i, ret);
	        }
		}
        hal_device_release((HW_DEVICE *)dev);
    }

    for(i=0;i<liscen.licennum;i++)
    {
        if(memcmp(app->buff,liscen.licen[i].appname,app->len)==0)
        {
            uint8 tmplinlen = 0,j=0,tempvalue,index;
            uint8 tmplincen[100]={0};
            if(strlen(liscen.licen[i].applicen)%2!=0)
            {
                tmplinlen = strlen(liscen.licen[i].applicen) -1;
            }
            else
            {
                tmplinlen = strlen(liscen.licen[i].applicen);
            }
			liscen.licen[i].licenlen = tmplinlen/2;

			for(j=0; j<tmplinlen;j+=2)
            {
            	tempvalue = (liscen.licen[i].applicen[j] <= '9') ? (liscen.licen[i].applicen[j] - '0') : (liscen.licen[i].applicen[j] - 'A' + 10);
				tmplincen[index] = tempvalue << 4;
				tempvalue = (liscen.licen[i].applicen[j+1] <= '9') ? (liscen.licen[i].applicen[j+1] - '0') : (liscen.licen[i].applicen[j+1] - 'A' + 10);
                tmplincen[index] += tempvalue;
            	index++;
            }
            MQTTMSG_BUF_DEBUG(tmplincen,index,"lincense转换后的值");
            char tmpcode[30]= {0};
            if(memcmp(tmplincen,tmpcode,24)==0)
            {
                MQTTMSG_FMT_DEBUG("匹配到24位0 消息回复错误\n");
                result = FALSE;
                break;
            }
        }
    }
    return result;
}

/**********************************************************************
* @name      : docker_exec_001F
* @brief     ：消息App操作命令执行
* @param[in] ：MSG_INFO_T *MsgRecv 消息,int MSGLen 长度
* @param[out]：
* @return    ：
* @Create    : 
* @Date      ：2020-5-22
* @Update    :
**********************************************************************/

void docker_exec_001F(WGPubattr *pData,MSG_INFO_T *MsgRecv,int MSGLen)
{
    uint8 *data   = MsgRecv->MsgData;
    uint16 msglen = MsgRecv->MsgLen;
    SYS_CONF sysconf;
    memset(&sysconf,0,sizeof(sysconf));
  //  char filePath[255] = {0};
    uint8 recvBUFF[20] = {0};
//    char buff[10][500];
//    uint8 index=0,ret=0;
    uint8 index=0;
    BOOL result = TRUE;
do{
    memcpy_r(&sysconf.memLimit,&data[index],sizeof(uint16));
    index+= sizeof(uint16);
    memcpy_r(&sysconf.diskLimit,&data[index],sizeof(uint16));
    index+= sizeof(uint16);
    memcpy_r(&sysconf.cpulimit,&data[index],sizeof(uint16));
    index+= sizeof(uint16);
    memcpy_r(&sysconf.cputemp_up,&data[index],sizeof(uint16));
    index+= sizeof(uint16);
    memcpy_r(&sysconf.cputemp_down,&data[index],sizeof(uint16));
    index+= sizeof(uint16);
    memcpy_r(&sysconf.sys_monitortime,&data[index],sizeof(uint16));
    index+= sizeof(uint16);
    if(index != msglen)
    {
        result = FALSE;
        break;
    }
}while(0);
    pData->systheadinfo.sysconfig.devCpuLimit = sysconf.cpulimit;
    pData->systheadinfo.sysconfig.devMemLimit = sysconf.memLimit;
    pData->systheadinfo.sysconfig.devDiskLimit = sysconf.diskLimit;
    pData->systheadinfo.sysconfig.devCpuTempup = sysconf.cputemp_up;
    pData->systheadinfo.sysconfig.devCpuTempdown = sysconf.cputemp_down;
    pData->systheadinfo.sysconfig.monitorTime = sysconf.sys_monitortime*60;
    pData->systheadinfo.sysconfig.updateTime = sysconf.sys_monitortime*60;
    UPDATE_FMT_DEBUG("系统阈值参数变更\n");
    INIT_FMT_DEBUG("系统参数配置 devCpuLimit %d ，devMemLimit %d devDiskLimit %d devCpuTempup %d devCpuTempdown %d monitorTime %d \n",pData->systheadinfo.sysconfig.devCpuLimit,pData->systheadinfo.sysconfig.devMemLimit,pData->systheadinfo.sysconfig.devDiskLimit
    ,pData->systheadinfo.sysconfig.devCpuTempup, pData->systheadinfo.sysconfig.devCpuTempdown,pData->systheadinfo.sysconfig.monitorTime);
    //用monitorctl 设置参数
#if 0
    char cmd[500] = {0};
    sprintf(cmd,"monitorctl -T cpu %d;monitorctl -T memory %d;monitorctl -T storage %d;monitorctl -T temperature %d;",sysconf.cpulimit/100,sysconf.memLimit/100,sysconf.diskLimit/100,sysconf.cputemp_up/10);
    ret = getshellresult(cmd,buff,10);
    print_exec_info(cmd,buff,ret);
    if(strcmp(buff[0],SYS_SETCPU_VALUE)==0)
    {
        MQTTMSG_FMT_DEBUG("系统设置CPU阈值成功 \n");
    }
    if(strcmp(buff[1],SYS_SETMEM_VALUE)==0)
    {
        MQTTMSG_FMT_DEBUG("系统设置内存阈值成功 \n");
    }
    if(strcmp(buff[2],SYS_SETSTO_VALUE)==0)
    {
        MQTTMSG_FMT_DEBUG("系统设置存储阈值成功 \n");
    }
    if(strcmp(buff[3],SYS_SETTEMP_VALUE)==0)
    {
        MQTTMSG_FMT_DEBUG("系统设置温度上限阈值成功 \n");
    }
#endif
    recvBUFF[0] = result;
    packge_report_msg(MsgRecv,recvBUFF,1);
}


/**********************************************************************
* @name      : docker_exec_001E
* @brief     ：消息App操作命令执行
* @param[in] ：MSG_INFO_T *MsgRecv 消息,int MSGLen 长度
* @param[out]：
* @return    ：
* @Create    : 
* @Date      ：2020-5-22
* @Update    :
**********************************************************************/

void docker_exec_001E(MSG_INFO_T *MsgRecv,int MSGLen)
{
    uint8 *data   = MsgRecv->MsgData;
    uint16 msglen = MsgRecv->MsgLen;
    char logFile[255] = {0};
    uint8 filelen = 0,index=1;
    BOOL result = TRUE;

    uint8 recvBUFF[50]={0};
    do{
        if(data[0]==0)       //日志文件路径 [0]  visible-string   
        {
            filelen = data[index++];
            memcpy(logFile,&data[index],filelen);
            index+=filelen;
            result  = judge_path_exec(logFile,0xFF);
            break;
        }
        else if(data[0]==1) //日志类型     [1]  LOGTYPE
        {
            uint8 logtype=0;
            logtype = data[index++];
           // operate_accord_logType(logtype,logFile);
            result  = judge_path_exec(logFile,logtype);
            break;

        }else
        {
            result = FALSE;
            break;
        }
        if(index!=msglen)
        {
            break;
        }
    }while(0);
    recvBUFF[0] = result;
    packge_report_msg(MsgRecv,recvBUFF,1);
}


/**********************************************************************
* @name      : docker_exec_001D
* @brief     : 001D 日志打包
* @param[in] ：MSG_INFO_T *MsgRecv 消息,int MSGLen 长度
* @param[out]：
* @return    ：
* @Create    : 
* @Date      ：2020-5-22
* @Update    :
**********************************************************************/

void docker_exec_001D(MSG_INFO_T *MsgRecv,int MSGLen)
{
    uint8 *data   = MsgRecv->MsgData;
    uint16 msglen = MsgRecv->MsgLen;
    int ret=0;
    uint8 recvBuff[1024] = {0};      
   // uint16 recvLen=0;
    uint16 index=1,tmplen=0,option=0;
    char tmppackPath[255] = {0};
    char SourcePath[255]={0};
    char packPath[255] = {0};
    uint8 retype = 1;
   do{
        if(data[0]==0)            //日志文件路径 [0]  visible-string，
        {
            tmplen = data[index++];
            memcpy(SourcePath,&data[index],tmplen);
            index+=tmplen;
            option  = data[index++];
            if(option == 0)
            {
                sprintf(packPath,"%s%s%d",PackLogDir,"pack",packnum++);
            }else if(option == 1)
            {
                tmplen = data[index++];
                memcpy(tmppackPath,&data[index],tmplen);
                index+=tmplen;
            }else
            {
                retype = 0;
                MQTTMSG_FMT_DEBUG("recv message error option %d\n",option);
                break;
            }

            /*判断是文件还是文件夹*/
        }
        else if(data[0]==1)     //日志类型     [1]  LOGTYPE
        {
            uint8 type = data[index++];
            operate_accord_logType(type,SourcePath);
            option   = data[index++];
            if(option == 0)  //默认文件夹下
            {
                sprintf(packPath,"%s%s%d",PackLogDir,"pack",packnum++);
            }else if(option == 1)
            {
                tmplen = data[index++];
                memcpy(tmppackPath,&data[index],tmplen);
                index+=tmplen;
            }else
            {
                retype = 0;
                MQTTMSG_FMT_DEBUG("recv message error option %d\n",option);
                break;
            }
        }
        else
        {
            retype = 0;
            MQTTMSG_FMT_DEBUG("recv message error option %d\n",option);
            break; 
        }
#if 0       
         if(option == 1)
         {
            /*判断是目录还是文件*/
            int filetype = judge_path_fold_dir(tmppackPath);
            if(filetype == -1 )
            {
                retype =0;
                MQTTMSG_FMT_DEBUG("path error %s\n",tmppackPath);
                break;
            }else if(filetype == 0)
            {
                //文件
                sprintf(packPath,"%s",tmppackPath);
            }else
            {
                //目录
                sprintf(packPath,"%s%s%d",tmppackPath,"pack",packnum++);
            }
        }
#endif
         if(index!=msglen)
         {
              retype = 0;
              MQTTMSG_FMT_DEBUG("recv message len != index \n");
              break;
         }
        
   }while(0);
    index=0;
    if(retype == 0)
    {
        recvBuff[index++] = 0;
    }else
    {
        ret = operate_accord_logPath(SourcePath,tmppackPath);
        if(ret == -1)
        {
            recvBuff[index++]=0;
        }
        else
        {  
            index  = set_len_offset(strlen(tmppackPath), recvBuff);
            memcpy(&recvBuff[index],tmppackPath,strlen(tmppackPath));
            index+=strlen(tmppackPath);              
        }
    }

    packge_report_msg(MsgRecv,recvBuff,index);    
}


/**********************************************************************
* @name      : docker_exec_001C
* @brief     : 消息App操作命令执行
* @param[in] : MSG_INFO_T *MsgRecv 消息,int MSGLen 长度
* @param[out]:
* @return    :
* @Create    : 
* @Date      : 2020-5-22
* @Update    :
**********************************************************************/

void docker_exec_001C(MSG_INFO_T *MsgRecv,int MSGLen)
{
    uint8 *data   = MsgRecv->MsgData;
    char logFile[255] = {0};
    uint8 filelen = 0,index=1;
    int ret = 0;
    log_info_T loginfo;
    memset(&loginfo,0,sizeof(loginfo));
    uint8 recvBUFF[1024] = {0};
    uint32 recvlen = 0; 
    do{
        if(data[0] == 0)       //日志名称
        {
            filelen = data[index++];
            memcpy(logFile,&data[index],filelen);         //带路径的文件名字 
            index+=filelen;
            MQTTMSG_FMT_DEBUG("用日志路径来召测日志信息 日志路径 %s \n",logFile);
            ret = get_logFile_info(&loginfo,logFile,0xFF);
            if(ret < 0)
            {
                loginfo.lognum = 0;
                break;
            }
            else
            {
                loginfo.lognum = 1;
                break;
            }
        }else if(data[0] == 1) //日志类型
        {
            uint8 type = data[index++];
            MQTTMSG_FMT_DEBUG("用日志类型来召测日志信息 日志类型 %d \n", type);
            if(type == 0)
                type = 1;           //目前不支持全部类型文件读取
            get_logFile_info(&loginfo,logFile,type);
        }       
        else 
        {
            MQTTMSG_FMT_DEBUG("日志类型有误 日志类型 %d \n", data[0]);
            break;            
        }
    }while(0);
    recvlen = pack_report_fileinfo(&loginfo,recvBUFF);
    packge_report_msg(MsgRecv,recvBUFF,recvlen);
}


/**********************************************************************
* @name      : docker_exec_001B
* @brief     ：消息App操作命令执行
* @param[in] ：MSG_INFO_T *MsgRecv 消息,int MSGLen 长度
* @param[out]：
* @return    ：
* @Create    : 
* @Date      ：2020-5-22
* @Update    :
**********************************************************************/

void docker_exec_001B(MSG_INFO_T *MsgRecv,int MSGLen)
{
    char buff[1][500];
    char sysversion[100]={0};
    char kerversion[100]={0};
    char tmpData[500]={0};
    //char filesysver[100]={0};
    char hardversion[100]={0};
    uint16 index = 0;
    memset(buff,0,sizeof(buff));
    if(getshellresult("uname -r",buff,1)==1)
    {
        strcpy(sysversion,buff[0]);
    }   
    memset(buff,0,sizeof(buff));
    if(getshellresult("uname -n",buff,1)==1)
    {
        strcpy(kerversion,buff[0]);    
    }
    strcpy(hardversion,"DM01");  //暂时写死
    tmpData[index++]=strlen(sysversion);
    strcpy(&tmpData[index],sysversion);
    index+=strlen(sysversion);
    tmpData[index++]=strlen(kerversion);
    strcpy(&tmpData[index],kerversion);
    index+=strlen(kerversion);
    tmpData[index++] = 0;//文件系统版本为NULL
    tmpData[index++]=strlen(hardversion);
    strcpy(&tmpData[index],hardversion);
    index+=strlen(hardversion);
    packge_report_msg(MsgRecv,(uint8 *)tmpData,index);  //应答
}


/**********************************************************************
* @name      : docker_exec_0019
* @brief     ：消息App操作命令执行
* @param[in] ：MSG_INFO_T *MsgRecv 消息,int MSGLen 长度
* @param[out]：
* @return    ：
* @Create    : 
* @Date      ：2020-5-22
* @Update    :
**********************************************************************/

void docker_exec_001A(MSG_INFO_T *MsgRecv,int MSGLen)
{
    uint8 *data   = MsgRecv->MsgData;
    uint16 msglen = MsgRecv->MsgLen;
    uint8 tmplen=0,sigtype=0,ret=0;
    uint16 index=0, checklen = 0;
    BOOL result = TRUE;
    
    char cmd[2048] = {0};
    uint8 filemd5[255] = {0};
    uint8 signmd5[255] = {0};
    uint8 filesm3[255] = {0};
    uint8 recvBuff[50] = {0};
    char buff[10][500] = {{0}};
    char strsm3[255]   = {0};
    char filePath[1024]={0};
    sys_update_info sys_info;
    memset(&sys_info,0,sizeof(sys_update_info));
    tmplen = data[index++];
    memcpy(sys_info.updatever,&data[index],tmplen);
    index+=tmplen;
    sys_info.updatetype = data[index++];
    do{
        if(sys_info.updatetype > 3)
        {
            //返回错误
            result = FALSE;
            break;
        }
        tmplen = data[index++];
        memcpy(sys_info.file_info_t.filename,&data[index],tmplen);
        index+=tmplen;
        tmplen = data[index++];
        memcpy(sys_info.file_info_t.filetype,&data[index],tmplen);
        index+=tmplen;
        tmplen = data[index++];
        memcpy(sys_info.file_info_t.filepath,&data[index],tmplen);
        index+=tmplen;
        memcpy_r(&sys_info.file_info_t.filesize,&data[index],sizeof(uint32));
        index+=sizeof(uint32);
        tmplen = data[index++];
        checklen = tmplen;   //记录一下校验长度
        if(tmplen > 0)
        {
            memcpy(sys_info.file_info_t.filemd5, &data[index], tmplen);
        }
        index += tmplen;
        
        sigtype = data[index++];
        if(sigtype!=0)
        {
            tmplen = data[index++];
            memcpy(sys_info.file_info_t.signinfo_t.signname,&data[index],tmplen);
            index+=tmplen;
            tmplen = data[index++];
            memcpy(sys_info.file_info_t.signinfo_t.signpath,&data[index],tmplen);
            index+=tmplen;
            memcpy_r(&sys_info.file_info_t.signinfo_t.signsize,&data[index],sizeof(uint16));
            index+=sizeof(uint32);
            tmplen = data[index++];
            memcpy(sys_info.file_info_t.signinfo_t.signmd5,&data[index],tmplen);
            index+=tmplen;
        }
        if(index!=msglen)
        {
            MQTTMSG_FMT_DEBUG("报文验证错误 index %d msglen %d \n",index,msglen);
            result = FALSE;
            break;
        }
        //验证升级文件是否存在
       memset(filePath,0,sizeof(filePath));
       sprintf(filePath,"%s/%s",sys_info.file_info_t.filepath,sys_info.file_info_t.filename);
       
       if(checklen == 32) //32位表示为sm3校验
       {
           if(0 == sys_info.file_info_t.filemd5[0])
           {
                ret = sizeAndSM3_judge(filePath,sys_info.file_info_t.filesize, NULL);
           }
           else
           {
                ret = sizeAndSM3_judge(filePath,sys_info.file_info_t.filesize, (char *)sys_info.file_info_t.filemd5);
           }
       }
       else // MD5校验
       {
           if(0 == sys_info.file_info_t.filemd5[0])
           {
                ret = sizeAndmd5_judge(filePath,sys_info.file_info_t.filesize, NULL);
           }
           else
           {
                ret = sizeAndmd5_judge(filePath,sys_info.file_info_t.filesize, (char *)sys_info.file_info_t.filemd5);
           }
       }
        
        if(ret != 0)
        {
            MQTTMSG_FMT_DEBUG("升级文件大小或者[%s]值校验有问题\n", tmplen == 32 ? "SM3" : "MD5");
            result = FALSE;
            break;
        }
        if(sigtype != 0)
        {
            strt2octet((char *)sys_info.file_info_t.signinfo_t.signmd5,strlen((char *)sys_info.file_info_t.signinfo_t.signmd5),filemd5,64);
            if(sigtype==1)
            {
                memset(filePath, 0, sizeof(filePath));
                sprintf(filePath,"%s/%s",sys_info.file_info_t.signinfo_t.signpath,sys_info.file_info_t.signinfo_t.signname);
                if(checklen == 32) //32位的是SM3校验
                {
                    ret = sizeAndSM3_judge(filePath,sys_info.file_info_t.signinfo_t.signsize,(char *)sys_info.file_info_t.signinfo_t.signmd5);
                }
                else
                {
                    ret = sizeAndmd5_judge(filePath,sys_info.file_info_t.signinfo_t.signsize,(char *)sys_info.file_info_t.signinfo_t.signmd5);
                }
                
                if(ret != 0)
                {
                    result = FALSE;
                    break;
                }
                strt2octet((char *)sys_info.file_info_t.signinfo_t.signmd5,strlen((char *)sys_info.file_info_t.signinfo_t.signmd5),signmd5,64);
            }
        }
    }while(0);
    //转换md5值
    recvBuff[0] = result;
    packge_report_msg(MsgRecv,recvBuff,1);
    if(FALSE == result)
    {
        return;
    }
    //先获取一下md5值
    memset(cmd,0,sizeof(cmd));
    memset(buff,0,sizeof(buff));
    ret=0;
    
    checklen = 16;  //由于upgrade工具暂未支持sm3编码，这里手动变更以下升级时用的命令，后续upgrade工具升级后，直接删除改行即可
    
    if(checklen == 32)  //SM3校验值32位
    {
        char tmpfilepath[128] = {0};
        snprintf(tmpfilepath, 128, "%s%s", sys_info.file_info_t.filepath,sys_info.file_info_t.filename);
        ret = GetFileSM3(tmpfilepath, 255, filesm3);
        if(ret != 0)
        {
            MQTTMSG_FMT_DEBUG("获取%s SM3校验失败\n", tmpfilepath);
        }
    }
    else
    {
        sprintf(cmd,"md5sum %s%s | awk '{print $1}'",sys_info.file_info_t.filepath,sys_info.file_info_t.filename);
        ret = getshellresult(cmd,buff,1);
        //打印返回值
        MQTTMSG_BUF_DEBUG(buff, ret,"执行命令 ：%s \n",cmd);
    }

    
    //拼接命令
    if(sys_info.updatetype==0)
    {
        sprintf(cmd,"update -p ");
    }else if(sys_info.updatetype==1)
    {
        sprintf(cmd,"update -fs ");
    }else if(sys_info.updatetype==2)
    {
        sprintf(cmd,"update -k ");
    }else if(sys_info.updatetype==3)
    {
        sprintf(cmd,"update -a ");
    }
    tmplen = strlen(cmd);

    if(checklen == 32)  //SM3校验值32位
    {
        memset(strsm3, 0, sizeof(strsm3));
        octet2str(filesm3, 32, strsm3, 255);
        sprintf(&cmd[tmplen]," -fn %s -ft 00 -fp %s -sm3 %s",sys_info.file_info_t.filename,sys_info.file_info_t.filepath, strsm3);
        if(sigtype == 1)
        {
            memset(strsm3, 0, sizeof(strsm3));
            octet2str(sys_info.file_info_t.signinfo_t.signmd5, 32, strsm3, 255);
            tmplen = strlen(cmd);
            sprintf(&cmd[tmplen]," -si -fp %s -sm3 %s",sys_info.file_info_t.signinfo_t.signpath, strsm3);
        }
    }
    else
    {
        sprintf(&cmd[tmplen]," -fn %s -ft 00 -fp %s -ch %s",sys_info.file_info_t.filename,sys_info.file_info_t.filepath, buff[0]);
        if(sigtype == 1)
        {
            memset(strsm3, 0, sizeof(strsm3));
            octet2str(sys_info.file_info_t.signinfo_t.signmd5, 16, strsm3, 255);
            tmplen = strlen(cmd);
            sprintf(&cmd[tmplen]," -si -fp %s -ch %s",sys_info.file_info_t.signinfo_t.signpath,strsm3);
        }
    }

     //执行命令不需要返回值
     //先回复正确并创建文件
    MQTTMSG_FMT_DEBUG("检测到系统升级  命令 CMD: %s\n",cmd);
    ret = getshellresult(cmd, buff, 20);
    if(ret > 0 )
    {
       // print_exec_faild(ret,buff);
    }

  //  memset(filePath,0,sizeof(filePath));
 //   sprintf(filePath,"%s%s",PrivateDir,"SYSUPDATE");
  //  write_pdata_xram(filePath,(char *)&sys_info,0,sizeof(sys_update_info));
    system(cmd);

    //临时操作 升级修改版本号
    if(strstr(sys_info.updatever, "1.1") != NULL)
	{
		system("cp /backup/os-version_1.1.yaml /etc/os-version.yaml");
	}
	else
	{
		system("cp /backup/os-version_1.2.yaml /etc/os-version.yaml");
	}

	system("cp /etc/os-version.yaml  /tmp/vsys");
    
}


/**********************************************************************
* @name      : docker_exec_0019
* @brief     ：消息App操作命令执行
* @param[in] ：MSG_INFO_T *MsgRecv 消息,int MSGLen 长度
* @param[out]：
* @return    ：
* @Create    : 
* @Date      ：2020-5-22
* @Update    :
**********************************************************************/

void docker_exec_0019(MSG_INFO_T *MsgRecv,int MSGLen)
{
    uint8 *data = MsgRecv->MsgData;
    uint16 msglen = MsgRecv->MsgLen;
    uint16 index=0,i=0,ret=0,type = 0;
    uint8 recvbuff[2048]={0};
    SEQ_STR app;
    app_license_T liscen;
    memset(&liscen,0,sizeof(liscen));
    memset(&app,0,sizeof(SEQ_STR));
	uint32 count = 0;
	int j = 0;
	uint8 tempvalue = 0;
	
    DEVINFO_DEVICE_T* dev = (DEVINFO_DEVICE_T*) hal_device_get(HW_DEVICE_ID_DEVINFO);
    if(NULL == dev)
    {
        MQTTMSG_FMT_DEBUG("devinfo hal_device_get error \n");
    }
    else
    {
        count = dev->devinfo_applicense_count(dev);
        if(count == 0)
        {
            MQTTMSG_FMT_DEBUG("devinfo devinfo_applicense_count error count %d\n", count);
        }
        else
        {
            MQTTMSG_FMT_DEBUG("devinfo devinfo_applicense_count  %d\n", count);
        }

		for(i=0; i<count; i++)
		{
			ret = dev->devinfo_read_applicense_index(dev, i, liscen.licen[liscen.licennum].appnum, liscen.licen[liscen.licennum].appname, liscen.licen[liscen.licennum].applicen, 100);
	        if(ret == 0)
	        {
	            liscen.licennum ++;
	        }
	        else
	        {
	            MQTTMSG_FMT_DEBUG(" devinfo_read_applicense_index error index%d ret%d !\n", i, ret);
	        }
		}
        hal_device_release((HW_DEVICE *)dev);

		
        /*需要哪些app的LICENSE*/
        if(data[index]==0)
        {
            //全部
            type = 0;
            index++;
        }
        else
        {
            app.len = data[index++];
            memcpy(app.buff,&data[index],app.len);
            index+=app.len;
            type = 1;
        }
        if(index!=msglen)
        {
            //消息报文有错误
            MQTTMSG_FMT_DEBUG("消息报文有错误 \n");
        }
        index=0;
        if(type==0)
        {   
            recvbuff[index++]=liscen.licennum;
            for(i=0;i<liscen.licennum;i++)
            {
            	liscen.licen[i].num = strlen(liscen.licen[i].appnum);
                recvbuff[index++] = liscen.licen[i].num;
                memcpy(&recvbuff[index],liscen.licen[i].appnum,liscen.licen[i].num);
                index+=liscen.licen[i].num;

				liscen.licen[i].namelen = strlen(liscen.licen[i].appname);
                recvbuff[index++] = liscen.licen[i].namelen;
                memcpy(&recvbuff[index],liscen.licen[i].appname,liscen.licen[i].namelen);
                index+=liscen.licen[i].namelen;

                uint8 tmplinlen = 0;
                if(strlen(liscen.licen[i].applicen)%2!=0)
                {
                    tmplinlen = strlen(liscen.licen[i].applicen) -1;
                }
                else
                {
                    tmplinlen = strlen(liscen.licen[i].applicen);
                }
				liscen.licen[i].licenlen = tmplinlen/2;
				recvbuff[index++] = liscen.licen[i].licenlen;

				for(j=0; j<tmplinlen;j+=2)
	            {
	            	tempvalue = (liscen.licen[i].applicen[j] <= '9') ? (liscen.licen[i].applicen[j] - '0') : (liscen.licen[i].applicen[j] - 'A' + 10);
					recvbuff[index] = tempvalue << 4;
					tempvalue = (liscen.licen[i].applicen[j+1] <= '9') ? (liscen.licen[i].applicen[j+1] - '0') : (liscen.licen[i].applicen[j+1] - 'A' + 10);
	                recvbuff[index] += tempvalue;
                	index++;
	            }
            }
        }
        else if(type==1)
        {
            
            for(i=0;i<liscen.licennum;i++)
            {
                if(memcmp(app.buff,liscen.licen[i].appname,app.len)==0)
                {
                	recvbuff[index++]=1;
                    recvbuff[index++] = liscen.licen[i].num;
                    memcpy(&recvbuff[index],liscen.licen[i].appnum,liscen.licen[i].num);
                    index+=liscen.licen[i].num;
                    recvbuff[index++] = liscen.licen[i].namelen;
                    memcpy(&recvbuff[index],liscen.licen[i].appname,liscen.licen[i].namelen);
                    index+=liscen.licen[i].namelen;
					
                    uint8 tmplinlen = 0;
                    if(strlen(liscen.licen[i].applicen)/2!=0)
                    {
                        tmplinlen = strlen(liscen.licen[i].applicen) -1;
                    }
                    else
                    {
                        tmplinlen = strlen(liscen.licen[i].applicen);
                    }
                    liscen.licen[i].licenlen = tmplinlen/2;
                    recvbuff[index++] = liscen.licen[i].licenlen;
                    
                    for(j=0; j<tmplinlen;j+=2)

		            {
		            	tempvalue = (liscen.licen[i].applicen[j] <= '9') ? (liscen.licen[i].applicen[j] - '0') : (liscen.licen[i].applicen[j] - 'A' + 10);
						recvbuff[index] = tempvalue << 4;
						tempvalue = (liscen.licen[i].applicen[j+1] <= '9') ? (liscen.licen[i].applicen[j+1] - '0') : (liscen.licen[i].applicen[j+1] - 'A' + 10);
		                recvbuff[index] += tempvalue;
	                	index++;
		            }
                    break;
                }
            }

			if(i == liscen.licennum)	//没有找到
			{
				recvbuff[index++]=1;
			}
        }
        packge_report_msg(MsgRecv,recvbuff,index);
        return;
   
    }   
    
}


/**********************************************************************
* @name      : package_recv_0017
* @brief     ：消息App操作命令执行
* @param[in] ：MSG_INFO_T *MsgRecv 消息,int MSGLen 长度
* @param[out]：
* @return    ：
* @Create    : 
* @Date      ：2020-5-22
* @Update    :
**********************************************************************/

uint16 package_recv_0017(ALL_APP_INFO_T *app,uint8 *data)
{
    uint16 index=0,i=0,j=0,servernum=0;
    data[index++]=app->appcount;
    for(i=0;i<app->appcount;i++)
    {
       data[index++]= app->appinfo[i].appindex;
       data[index++]= strlen((char *)app->appinfo[i].appname);
       strcpy((char *)&data[index],(char *)app->appinfo[i].appname);
       index+=strlen((char *)app->appinfo[i].appname);
       data[index++]= strlen((char *)app->appinfo[i].containername);
       strcpy((char *)&data[index],(char *)app->appinfo[i].containername);
       index+=strlen((char *)app->appinfo[i].containername);
       data[index++]= strlen((char *)app->appinfo[i].appversion);
       strcpy((char *)&data[index],(char *)app->appinfo[i].appversion);
       index+=strlen((char *)app->appinfo[i].appversion);
       data[index++]= strlen((char *)app->appinfo[i].apphash);
       strcpy((char *)&data[index],(char *)app->appinfo[i].apphash);
       index+=strlen((char *)app->appinfo[i].apphash);
       data[index++] = app->appinfo[i].servicecount;
       servernum = app->appinfo[i].servicecount;
       for(j=0;j<servernum;j++)
       {
           data[index++] = app->appinfo[i].serviceinfo[j].serviceindex;
           data[index++]= strlen((char *)app->appinfo[i].serviceinfo[j].servicename);
           strcpy((char *)&data[index],(char *)app->appinfo[i].serviceinfo[j].servicename);
           index+=strlen((char *)app->appinfo[i].serviceinfo[j].servicename);
           data[index++] = app->appinfo[i].serviceinfo[j].serviceenable;
           data[index++] = app->appinfo[i].serviceinfo[j].servicestate;
           memcpy_r(&data[index],&app->appinfo[i].serviceinfo[j].cpurate,sizeof(uint16));
           index+=sizeof(uint16);
           memcpy_r(&data[index],&app->appinfo[i].serviceinfo[j].memrate,sizeof(uint16));
           index+=sizeof(uint16);
           memcpy_r(&data[index],&app->appinfo[i].serviceinfo[j].starttime.year,sizeof(uint16));
           index+=sizeof(uint16);
           data[index++]=app->appinfo[i].serviceinfo[j].starttime.month;
           data[index++]=app->appinfo[i].serviceinfo[j].starttime.day;
           data[index++]=app->appinfo[i].serviceinfo[j].starttime.hour;
           data[index++]=app->appinfo[i].serviceinfo[j].starttime.minute;
           data[index++]=app->appinfo[i].serviceinfo[j].starttime.second;
       }
       data[index++]=app->appinfo[i].cpucore;
       memcpy_r(&data[index],&app->appinfo[i].cpuLimit,sizeof(uint16));
       index+=sizeof(uint16);
       memcpy_r(&data[index],&app->appinfo[i].memLval,sizeof(uint16));
       index+=sizeof(uint16);
       memcpy_r(&data[index],&app->appinfo[i].memLimit,sizeof(uint16));
       index+=sizeof(uint16);
    }
    return index;
}


/**********************************************************************
* @name      : docker_exec_0018
* @brief     ：消息App操作命令执行
* @param[in] ：MSG_INFO_T *MsgRecv 消息,int MSGLen 长度
* @param[out]：
* @return    ：
* @Create    : 
* @Date      ：2020-5-22
* @Update    :
**********************************************************************/

void docker_exec_0018(MSG_INFO_T *MsgRecv,int MSGLen)
{
    MQTTMSG_FMT_DEBUG("收到软件注册码下发指令 \n");
    uint8 *data = MsgRecv->MsgData;
    uint16 msglen = MsgRecv->MsgLen;
    uint16 index=0,count=0,i=0;
    uint8 recvbuff[10]={0};
    app_license_T liscen;
    app_license_T tmpliscen;
    
//    uint8 result= 1;
    uint8 ret=0;
    memset(&liscen,0,sizeof(liscen));
    memset(&tmpliscen,0,sizeof(tmpliscen));
    if(data[index]==0)
    {
        recvbuff[0]=0;
        packge_report_msg(MsgRecv,recvbuff,1);
        return;
    }
    count = data[index++];
    liscen.licennum = count;
    for(i=0;i<count;i++)
    {
        liscen.licen[i].num = data[index++];
        memcpy(liscen.licen[i].appnum,&data[index],liscen.licen[i].num);
        index+=liscen.licen[i].num;
        liscen.licen[i].namelen = data[index++];
        memcpy(liscen.licen[i].appname,&data[index],liscen.licen[i].namelen);
        index+=liscen.licen[i].namelen;
        liscen.licen[i].licenlen = data[index++];
        memcpy(liscen.licen[i].applicen,&data[index],liscen.licen[i].licenlen);
        index+=liscen.licen[i].licenlen;
        
    }
    if(index!=msglen)
    {
        
        recvbuff[0]=0;
        packge_report_msg(MsgRecv,recvbuff,1);
        return;
    }
    memcpy(&tmpliscen,&liscen,sizeof(liscen));
    DEVINFO_DEVICE_T* dev = (DEVINFO_DEVICE_T*) hal_device_get(HW_DEVICE_ID_DEVINFO);
    if(NULL == dev)
    {
        MQTTMSG_FMT_DEBUG("devinfo hal_device_get error\n");
    }
    else
    {
        char lictemp[100] = {0};
        int  len = 0;
        uint8 ii = 0;
        uint8 tempvalue = 0;
        for(i=0;i<tmpliscen.licennum;i++)
        {
            memset(lictemp, 0, 100);
            len = 0;
            for(ii = 0; ii < tmpliscen.licen[i].licenlen; ii++)
            {
                tempvalue = ((tmpliscen.licen[i].applicen[ii] >> 4) & 0x0F);
                lictemp[len] = (tempvalue <= 9) ? (tempvalue + '0') : (tempvalue + 'A' - 10);
                tempvalue = (tmpliscen.licen[i].applicen[ii] & 0x0F);
                lictemp[len + 1] = (tempvalue <= 9) ? (tempvalue + '0') : (tempvalue + 'A' - 10);
                len += 2;
            }
            ret = dev->devinfo_write_applicense(dev,tmpliscen.licen[i].appnum,tmpliscen.licen[i].appname, lictemp, len);
            if(ERR_OK != ret)
            {
                MQTTMSG_FMT_DEBUG("devinfo devinfo_write_applicense error, ret %d\n", ret);
            }
        }

        hal_device_release((HW_DEVICE *)dev);
    }
    recvbuff[0]=1;
    packge_report_msg(MsgRecv,recvbuff,1);
   
}


/**********************************************************************
* @name      : docker_exec_0017
* @brief     ：消息App操作命令执行
* @param[in] ：MSG_INFO_T *MsgRecv 消息,int MSGLen 长度
* @param[out]：
* @return    ：
* @Create    : 
* @Date      ：2020-5-22
* @Update    :
**********************************************************************/

void docker_exec_0017(MSG_INFO_T *MsgRecv,int MSGLen)
{
    uint8 *data = MsgRecv->MsgData;
    uint16 msglen =0;
    uint8 index=0,ret=0;
    uint8 recvbuff[2048]={0};
    ALL_APP_INFO_T app_info;
    SEQ_STR app;
    SEQ_STR docker;
    memset(&app,0,sizeof(SEQ_STR));
    memset(&docker,0,sizeof(SEQ_STR));
    memset(&app_info,0,sizeof(ALL_APP_INFO_T));
	//先把需要获取多少个app信息
    if(data[index]==0 && data[index+1]==0)
    {
	    //没有容器也没有app 全部的APP
	    index+=2;
	    char cmd[100] = "docker ps --format '{{.Names}}'";
		char buff[10][500]={{0}};
		uint8 count= 0;
		count = getshellresult(cmd,buff,10);
		MQTTMSG_FMT_DEBUG("获取当前终端内所有的容器信息，容器个数为%d  \n",count);
		if(count != 0)
		{
			for(uint8 i=0;i<count;i++)
			{
				ret = get_allapp_in_docker(&app_info,buff[i]);
				if(ret != 0)
				{
                    continue;
                }
                MQTTMSG_FMT_DEBUG("index %d %s %s   \n",i,app_info.appinfo[i].containername,app_info.appinfo[i].appname);
			}
            MQTTMSG_FMT_DEBUG("获取当前终端内所有的容器信息，容器个数为%d  \n",app_info.appcount);
		}    
    }
    else if(data[index]==0 && data[index+1]==1)
    {
        /*检查所有容器内这个名字的app*/
        index = 2;
        app.len = data[index++];
        memcpy(app.buff,&data[index],app.len);
        index+=app.len;
		MQTTMSG_FMT_DEBUG("获取当前终端容器内所有相同app信息 %s \n",docker.buff);
    }
	else{
        docker.len = data[index++];
        memcpy(docker.buff,&data[index],docker.len);
        index+=docker.len;
        if(data[index]==0)
        {
            ret = get_allapp_in_docker(&app_info,(char *)docker.buff);
            if(ret == 1)
            {
                recvbuff[0]=0;
                packge_report_msg(MsgRecv,recvbuff,1); 
            }
        }
        else
        {
            app.len = data[index++];
            memcpy(app.buff,&data[index],app.len);
            index+=app.len;
            app_info.appcount = 1;
            memcpy(app_info.appinfo[0].containername,docker.buff,docker.len);
            memcpy(app_info.appinfo[0].appname,app.buff,app.len);
        }
    }
    /*获取相应的消息然后组报文*/
   if(index!=MsgRecv->MsgLen)
    {
        MQTTMSG_FMT_DEBUG("报文解析有误 index %d msglen %d\n",index,MsgRecv->MsgLen);
    }
   	for(uint8 i=0;i<app_info.appcount;i++)
   	{
		get_app_info((char *)app_info.appinfo[i].containername,(char *)app_info.appinfo[i].appname,&app_info,i);
	}
    msglen = package_recv_0017(&app_info,recvbuff);
    packge_report_msg(MsgRecv,recvbuff,msglen);
}


/**********************************************************************
* @name      : docker_exec_0016
* @brief     ：消息App操作命令执行
* @param[in] ：MSG_INFO_T *MsgRecv 消息,int MSGLen 长度
* @param[out]：
* @return    ：
* @Create    : 
* @Date      ：2020-5-22
* @Update    :
**********************************************************************/

void docker_exec_0016(MSG_INFO_T *MsgRecv,int MSGLen)
{
    iot_mqtt_0016 app_actinfo;
    char buff[10][500] = {0};
    char cmd[1024] = {0};
    //char errmess[500] = {0};
    BOOL result=TRUE;
    uint8 result_buff[100]={0};
    memset(&app_actinfo,0,sizeof(iot_mqtt_0016));
    uint8 offset=0;
    app_actinfo.docker.len = MsgRecv->MsgData[offset++]; //容器名称长度
    if(app_actinfo.docker.len>100)
        return ;
    memcpy(app_actinfo.docker.buff,&MsgRecv->MsgData[offset],app_actinfo.docker.len);
    offset+=app_actinfo.docker.len;
    app_actinfo.app.len = MsgRecv->MsgData[offset++];
    if(app_actinfo.app.len>100)
        return ;
    memcpy(app_actinfo.app.buff,&MsgRecv->MsgData[offset],app_actinfo.app.len);
    offset+=app_actinfo.app.len;
    app_actinfo.app_config_t.cpucore = MsgRecv->MsgData[offset++];
    memcpy_r(&app_actinfo.app_config_t.cpuLimit,&MsgRecv->MsgData[offset],sizeof(uint16));
    offset+=sizeof(uint16);
    memcpy_r(&app_actinfo.app_config_t.mem_limit,&MsgRecv->MsgData[offset],sizeof(uint16));
    offset+=sizeof(uint16);
    memcpy_r(&app_actinfo.app_config_t.memLimit,&MsgRecv->MsgData[offset],sizeof(uint16));
    offset+=sizeof(uint16);
    memset(cmd,0,1024);
    memset(buff,0,sizeof(buff));
    sprintf(cmd,"appm -t -cpu %d -mem %d -c %s -n %s | grep success ",app_actinfo.app_config_t.cpuLimit,app_actinfo.app_config_t.memLimit,app_actinfo.docker.buff,app_actinfo.app.buff);
	getshellresult(cmd,buff,10);
    MQTTMSG_FMT_DEBUG("App 参数设置成功 cmd %s\n",cmd);
    
    result_buff[0]=result;
    packge_report_msg(MsgRecv,result_buff,1);  //应答
}


/**********************************************************************
* @name      : docker_exec_0015
* @brief     ：消息App操作命令执行
* @param[in] ：MSG_INFO_T *MsgRecv 消息,int MSGLen 长度
* @param[out]：
* @return    ：
* @Create    : 
* @Date      ：2020-5-22
* @Update    :
**********************************************************************/

void docker_exec_0015(MSG_INFO_T *MsgRecv,int MSGLen)
{
    uint8 index = 0;
    BOOL errcode = TRUE;
    uint8 buff[100]={0};
    SEQ_STR docker;
    SEQ_STR app;
    memset(&docker,0,sizeof(SEQ_STR));
    memset(&app,0,sizeof(SEQ_STR));
    docker.len =  MsgRecv->MsgData[index++];
    memcpy(docker.buff,&MsgRecv->MsgData[index],docker.len);
    index+=docker.len;
    app.len = MsgRecv->MsgData[index++];
    memcpy(app.buff,&MsgRecv->MsgData[index],app.len);
    index+=app.len;
    uint8 type = MsgRecv->MsgData[index++];
	if(index!=MsgRecv->MsgLen)   //报文正确性判断
	{
		MQTTMSG_FMT_DEBUG("报文校验错误 index %d msglen %d \n",index,MsgRecv->MsgLen);
		errcode = FALSE;
	}
    if(type == 0)
    {
        //先判断下是否匹配24个0
        errcode = judge_linsence_legal(&app);
    }
	buff[0]=errcode;
    packge_report_msg(MsgRecv,buff,1); 
    App_comand_exec(&docker,&app,type);     
}

/**********************************************************************
* @name      : docker_exec_0014
* @brief     ：消息0014App安装命令执行
* @param[in] ：MSG_INFO_T *MsgRecv 消息,int MSGLen 长度
* @param[out]：
* @return    ：
* @Create    : 
* @Date      ：2020-5-22
* @Update    :
**********************************************************************/

void docker_exec_0014(MSG_INFO_T *MsgRecv,int MSGLen)
{
    uint16 index=0;
    uint8 option = 0,ret = 0,exectype = 0;//0为安装
    uint16 tmplen = 0, checklen = 0;
    BOOL result = TRUE;
    iot_mqtt_0014 app_install;
    char pathname[2048]={0};
    char buff[20][500];
    memset(buff,0,sizeof(buff));
    memset(&app_install,0,sizeof(iot_mqtt_0014));
    app_install.docker.len = MsgRecv->MsgData[index++];
    memcpy(app_install.docker.buff,&MsgRecv->MsgData[index],app_install.docker.len);
    index +=app_install.docker.len;
    
    app_install.app.len = MsgRecv->MsgData[index++];
    memcpy(app_install.app.buff,&MsgRecv->MsgData[index],app_install.app.len);
    index +=app_install.app.len;
    tmplen = MsgRecv->MsgData[index++];     //应用版本号
    memcpy(app_install.app_config_t.app_version,&MsgRecv->MsgData[index],tmplen);
    index +=tmplen;
    tmplen = MsgRecv->MsgData[index++];    //文件的名字
    memcpy(app_install.app_config_t.file_info_t.filename,&MsgRecv->MsgData[index],tmplen);
    index +=tmplen;
    tmplen = MsgRecv->MsgData[index++];    //文件的类型
    memcpy(app_install.app_config_t.file_info_t.filetype,&MsgRecv->MsgData[index],tmplen);
    index +=tmplen;
    tmplen = MsgRecv->MsgData[index++];    //文件路径
    memcpy(app_install.app_config_t.file_info_t.filepath,&MsgRecv->MsgData[index],tmplen);
    index +=tmplen;
    memcpy_r(&app_install.app_config_t.file_info_t.filesize,&MsgRecv->MsgData[index],sizeof(uint32));
    index +=sizeof(uint32);                //文件大小
    tmplen = MsgRecv->MsgData[index++];    //文件的 md5 值 
    checklen = tmplen;                     //记录校验值的长度
    memcpy(app_install.app_config_t.file_info_t.filemd5,&MsgRecv->MsgData[index],tmplen);
    index +=tmplen;
    option = MsgRecv->MsgData[index++];
    //文件的数字签证信息  OPTIONAL
    if(option == 1)
    {
        tmplen = MsgRecv->MsgData[index++];    //数字证书文件名字
        memcpy(app_install.app_config_t.file_info_t.signinfo_t.signname,&MsgRecv->MsgData[index],tmplen);
        index +=tmplen;
        tmplen = MsgRecv->MsgData[index++];    //数字证书文件路径
        memcpy(app_install.app_config_t.file_info_t.signinfo_t.signpath,&MsgRecv->MsgData[index],tmplen);
        index +=tmplen;
        memcpy_r(&app_install.app_config_t.file_info_t.signinfo_t.signsize,&MsgRecv->MsgData[index],sizeof(uint32));
        index +=sizeof(uint32); 
        tmplen = MsgRecv->MsgData[index++];    //数字证书文件的md5值
        memcpy(app_install.app_config_t.file_info_t.signinfo_t.signmd5,&MsgRecv->MsgData[index],tmplen);
        index +=tmplen;
    }
    app_install.app_config_t.app_config_t.cpucore = MsgRecv->MsgData[index++];
    memcpy_r(&app_install.app_config_t.app_config_t.cpuLimit,&MsgRecv->MsgData[index],sizeof(uint16));
    index +=sizeof(uint16);
    memcpy_r(&app_install.app_config_t.app_config_t.mem_limit,&MsgRecv->MsgData[index],sizeof(uint16));
    index +=sizeof(uint16);
    memcpy_r(&app_install.app_config_t.app_config_t.memLimit,&MsgRecv->MsgData[index],sizeof(uint16));
    index +=sizeof(uint16);
	if(index!=MsgRecv->MsgLen)   //报文正确性判断
	{
		MQTTMSG_FMT_DEBUG("报文校验错误 index %d msglen %d \n",index,MsgRecv->MsgLen);
		result = FALSE;
	}
	uint8 tmpbuff[10]={0};
	tmpbuff[0]=result;
	packge_report_msg(MsgRecv,tmpbuff,1);
   do{
        if(option == 1)
        {
            //校验一下数字签名文件是否存在
            sprintf(pathname,"%s%s",app_install.app_config_t.file_info_t.signinfo_t.signpath,app_install.app_config_t.file_info_t.signinfo_t.signname);
            if(access(pathname,R_OK)!=0)
            {
                result = FALSE;
                break;
            }
            else
            {
                if(checklen == 32)  //校验值长度为32 SM3校验
                {
                    ret = sizeAndSM3_judge(pathname,app_install.app_config_t.file_info_t.signinfo_t.signsize,(char *)app_install.app_config_t.file_info_t.signinfo_t.signmd5);
                }
                else
                {
                    ret = sizeAndmd5_judge(pathname,app_install.app_config_t.file_info_t.signinfo_t.signsize,(char *)app_install.app_config_t.file_info_t.signinfo_t.signmd5);
                }
                if(ret != 0)
                {
                    result = FALSE;
                    break;
                } 
            }       
            
        }

        /*判断升级文件存不存在*/
        memset(pathname,0,sizeof(pathname));
        sprintf(pathname,"%s/%s",app_install.app_config_t.file_info_t.filepath,app_install.app_config_t.file_info_t.filename);
        if(access(pathname,R_OK)!=0)
        {
            MQTTMSG_FMT_DEBUG("检查文件是否存在 CMD %s\n ",pathname);
            result = FALSE;
            break;
        }
        
        if(checklen == 32)  //校验值长度为32 SM3校验
        {
            ret = sizeAndSM3_judge(pathname,app_install.app_config_t.file_info_t.filesize,(char *)app_install.app_config_t.file_info_t.filemd5);
        }
        else
        {
            ret = sizeAndmd5_judge(pathname,app_install.app_config_t.file_info_t.filesize,(char *)app_install.app_config_t.file_info_t.filemd5);
        }
        if(ret != 0)
        {
             result = FALSE;
             break;
        }
        char filePath[255] = {0};
        memset(buff,0,sizeof(buff));
        //先判断当前终端是否存在该容器和App，如果存在视为升级，否则视为安装
        sprintf(filePath,"appm -I -c %s -n %s",app_install.docker.buff,app_install.app.buff); 
        ret = getshellresult(filePath,buff,20);
        print_exec_info(filePath, buff, ret);
        if(ret > 1)
        {
            MQTTMSG_FMT_DEBUG("存在同名app，视为升级\n");
            exectype = 1;
        }
        if(exectype == 1)
        {
            uint8 tmpbuff[10]={0};
            tmpbuff[0]=1;
            packge_report_msg(MsgRecv,tmpbuff,1);
            char cmd[100] = {0};
		   sprintf(cmd, "rm -rf /backup/app_back/*");
		   system(cmd);
		   system("sync");
        }
         //用指令安装app
        //appm -i -cpu 4 -mem 50  -t -cpu 70 -t -mem 80 -c c_master_yx -n puAmr -f puAmr.tar
        memset(filePath,0,sizeof(filePath));
        sprintf(filePath,"%s%s",app_install.app_config_t.file_info_t.filepath,app_install.app_config_t.file_info_t.filename); 
        memset(pathname,0,sizeof(pathname));
        memset(buff,0,sizeof(buff));

		if(exectype == 0)
        	sprintf(pathname,"appm -i -cpu %d -mem %d -c %s -n %s -f %s | grep Succeed",app_install.app_config_t.app_config_t.cpucore,app_install.app_config_t.app_config_t.mem_limit, app_install.docker.buff,app_install.app.buff,filePath);
        else
        	sprintf(pathname,"appm -U -c %s -n %s -f %s | grep Succeed", app_install.docker.buff,app_install.app.buff,filePath);
        MQTTMSG_FMT_DEBUG("执行安装/升级命令 CMD -- %s\n",pathname);
        uint8 tmp_num = 0;
        do{
            memset(buff,0,sizeof(buff));
            ret = getshellresult(pathname,buff,10);
            if (ret == 0)
            {
                MQTTMSG_FMT_DEBUG("失败重试 %d \n",tmp_num);
               // print_exec_faild(ret,buff);
                tmp_num++;
                sleep(1);
            }
            else
            {
                break;
            }
        }while(tmp_num<3);
        if(tmp_num==3)
        {
            result = FALSE;
            break;
        }
        if(exectype == 0)  //不设置参数，把参数存起来
        {
            memset(buff,0,sizeof(buff));
            memset(pathname,0,sizeof(pathname));
            sprintf(pathname,"appm -t -cpu %d -mem %d -c %s -n %s",app_install.app_config_t.app_config_t.cpuLimit,app_install.app_config_t.app_config_t.memLimit,app_install.docker.buff,app_install.app.buff);
            uint8 num_T=0;
            do{
                ret = getshellresult(pathname,buff,3);
                {
                    MQTTMSG_FMT_DEBUG("执行参数设置命令 CMD -- %s \n",pathname);
                    if(ret != 0)
                    {
                        MQTTMSG_FMT_DEBUG("参数设置失败 \n");
                        sleep(1);
                        num_T++;
                    }
                    else
                    {
                        break;
                    }
                }
            }while(num_T<3);
            if(num_T==3)
            {          
                MQTTMSG_FMT_DEBUG("参数设置失败 \n");
                result = FALSE;
                break;
            }
        }
    
    }while(0);
   
}


/**********************************************************************
* @name      : docker_exec_0013
* @brief     ：容器查询消息执行
* @param[in] ：MSG_INFO_T *MsgRecv 消息报文 int MSGLen 大小
* @param[out]：
* @return    ：
* @Create    : 
* @Date      ：2020-5-22
* @Update    :
**********************************************************************/
void docker_exec_0013(MSG_INFO_T *MsgRecv,int MSGLen)
{
    uint8 payload[2048]={0};  
    uint16 index=0;
    char *data  = (char *)MsgRecv->MsgData;
    ALL_CONTAINER_INFO_T allcontainerinfo;
    memset(&allcontainerinfo,0,sizeof(allcontainerinfo));
    SEQ_STR dockername;
    memset(&dockername,0,sizeof(SEQ_STR));
    
    do{
        uint8 option = data[index];
        index ++;
        if(option == 0)
        {
            if(MsgRecv->MsgLen != index)
            {
                MQTTMSG_FMT_DEBUG("规约不符合 index %d msglen %d \n",index,MsgRecv->MsgLen);
            }
            //获取全部的容器信息
            MQTTMSG_FMT_DEBUG("获取全部容器信息 \n");
        }
        else if(option == 1)
        {
            dockername.len = data[index++];
            memcpy(dockername.buff,&data[index],dockername.len);
            index += dockername.len;
            if(MsgRecv->MsgLen != index)
            {
                MQTTMSG_FMT_DEBUG("规约不符合 index %d msglen %d \n",index,MsgRecv->MsgLen);
            }
            //获取这个容器的信息
            MQTTMSG_FMT_DEBUG("获取容器[%s]信息 \n",dockername.buff);
        }else
        {
            MQTTMSG_FMT_DEBUG("规约不符合 index %d msglen %d \n",index,MsgRecv->MsgLen);
        }
        get_coninfo_all(&allcontainerinfo);
        MQTTMSG_FMT_DEBUG("当前终端共有 %d 容器 \n",allcontainerinfo.containercount);
        index = packge_0013_report(&dockername,payload,&allcontainerinfo);
    }while(0);
             
    packge_report_msg(MsgRecv,payload,index); 
}


/*******************************************************************************
* 函数名称: container_exec_0012
* 函数功能: 执行0012容器配置
* 输入参数：docker_config_T     容器配置结构体
*       
* 输出参数：
* 内部处理: 先判断容器是否执行，判断正在执行才可以进行动态配置参数
* 返 回 值: uint8                0 正确 其他错误
*******************************************************************************/

uint8 container_exec_0012(docker_config_T *dock_config)
{
    char buff[20][500]={{0}};
    uint8 ret=0;
    docker_config_T * tmpconfig = dock_config;
    char cmd[500] = {0};
    memset(cmd,0x00,sizeof(cmd));
    if(tmpconfig->dockername ==NULL)
    {
        MQTTMSG_FMT_DEBUG("docker name NULL \n");
        return 1;
    }
    memset(cmd,0x00,sizeof(cmd));
    //目前只支持一个一个数据项单独设置

    sprintf(cmd,"container config %s -cpu %d -mem %d -disk %d ",tmpconfig->dockername,tmpconfig->iot_docker_config_T.cpu_core,tmpconfig->iot_docker_config_T.mem_limitval,\
       tmpconfig->iot_docker_config_T.disk_limitval);
    memset(buff,0,sizeof(buff));
    MQTTMSG_FMT_DEBUG("容器参数配置\n");
    ret = getshellresult(cmd,buff,10);
    print_exec_info(cmd,buff,ret);
    if(strcmp(buff[ret-1],CON_CONFIG_VALUE)==0)
    {
        MQTTMSG_FMT_DEBUG("容器参数配置成功 \n");
    }
    else
    {
        MQTTMSG_FMT_DEBUG("容器参数配置失败 \n");
    }

    memset(cmd,0x00,sizeof(cmd));
    memset(buff,0,sizeof(buff));
    MQTTMSG_FMT_DEBUG("容器阈值配置\n");
    sprintf(cmd,"container monitor -t cpu %d %s",tmpconfig->iot_docker_config_T.cpu_limit/100,tmpconfig->dockername);
    system(cmd);  
    memset(cmd,0x00,sizeof(cmd));
    sprintf(cmd,"container monitor -t  memory %d %s ",tmpconfig->iot_docker_config_T.mem_limit/100,tmpconfig->dockername);
    system(cmd);     
    memset(cmd,0x00,sizeof(cmd));
    sprintf(cmd,"container monitor -t storage %d %s ",tmpconfig->iot_docker_config_T.disk_limit/100,tmpconfig->dockername);
    system(cmd); 
    MQTTMSG_FMT_DEBUG("容器阈值配置成功\n");

    /*挂载的宿主机文件夹 	array visible-string,
    应用存储文件路径      visible-string,
    映射的硬件接口     	array visible-string,
    端口资源配置参数      visible-string
    暂时不做
    */
    return 0;
}

/**********************************************************************
* @name      : docker_exec_0012
* @brief     ：容器配置消息执行
* @param[in] ：MSG_INFO_T *MsgRecv 消息报文 int MSGLen 大小
* @param[out]：
* @return    ：
* @Create    : 
* @Date      ：2020-5-22
* @Update    :
**********************************************************************/

void docker_exec_0012(MSG_INFO_T *MsgRecv,int MSGLen)
{
    //uint8 payload[2048];
    uint16 index=0;
    uint8 filenum=0,i=0,ret=0;
    docker_config_T docker_config;
    BOOL result = TRUE;
    uint8 result_buf[500]={0};
    memset(&docker_config,0x00,sizeof(docker_config_T));
    //memcpy(payload,MsgRecv->MsgData,MsgRecv->MsgLen);
    docker_config.dockernamelen = MsgRecv->MsgData[index++];
    memcpy(docker_config.dockername,&MsgRecv->MsgData[index],docker_config.dockernamelen);
    index+=docker_config.dockernamelen;
    docker_config.iot_docker_config_T.cpu_core = MsgRecv->MsgData[index++];
    memcpy_r(&docker_config.iot_docker_config_T.cpu_limit,&MsgRecv->MsgData[index],sizeof(uint16));
    index+=sizeof(uint16);
    memcpy_r(&docker_config.iot_docker_config_T.mem_limitval,&MsgRecv->MsgData[index],sizeof(uint16));
    index+=sizeof(uint16);
    memcpy_r(&docker_config.iot_docker_config_T.mem_limit,&MsgRecv->MsgData[index],sizeof(uint16));
    index+=sizeof(uint16);
    memcpy_r(&docker_config.iot_docker_config_T.disk_limitval,&MsgRecv->MsgData[index],sizeof(uint16));
    index+=sizeof(uint16);
    memcpy_r(&docker_config.iot_docker_config_T.disk_limit,&MsgRecv->MsgData[index],sizeof(uint16));
    index+=sizeof(uint16);

    filenum = MsgRecv->MsgData[index++];
    docker_config.iot_docker_config_T.host_folder = filenum;
    for(i=0;i<filenum;i++)
    {
        docker_config.iot_docker_config_T.host_info[i].len = MsgRecv->MsgData[index++];
        memcpy(docker_config.iot_docker_config_T.host_info[i].buff,&MsgRecv->MsgData[index],docker_config.iot_docker_config_T.host_info[i].len);
        index+=docker_config.iot_docker_config_T.host_info[i].len;
    }
    docker_config.iot_docker_config_T.strogefilelen = MsgRecv->MsgData[index++];
    memcpy(docker_config.iot_docker_config_T.stroge_file,&MsgRecv->MsgData[index],docker_config.iot_docker_config_T.strogefilelen);
    index+=docker_config.iot_docker_config_T.strogefilelen;
    filenum = MsgRecv->MsgData[index++];
    docker_config.iot_docker_config_T.dev_infolen = filenum;
    for(i=0;i<filenum;i++)
    {
        docker_config.iot_docker_config_T.dev_info[i].len = MsgRecv->MsgData[index++];
        memcpy(docker_config.iot_docker_config_T.dev_info[i].buff,&MsgRecv->MsgData[index],docker_config.iot_docker_config_T.dev_info[i].len);
        index+=docker_config.iot_docker_config_T.dev_info[i].len;
    }
    docker_config.iot_docker_config_T.port_para = MsgRecv->MsgData[index++];
    memcpy(docker_config.iot_docker_config_T.port_val,&MsgRecv->MsgData[index],docker_config.iot_docker_config_T.port_para);
	if(index!=MsgRecv->MsgLen)   //报文正确性判断
	{
		MQTTMSG_FMT_DEBUG("报文校验错误 index %d msglen %d \n",index,MsgRecv->MsgLen);
		result = FALSE;
	}
	ret = container_exec_0012(&docker_config);
    if(ret != 0)
    {
        result = FALSE;
    }
	result_buf[0]=result;
    packge_report_msg(MsgRecv,result_buf,1);
}


/**********************************************************************
* @name      : docker_exec_0011
* @brief     ：容器操作消息执行
* @param[in] ：MSG_INFO_T *MsgRecv 消息报文 int MSGLen 大小
* @param[out]：
* @return    ：
* @Create    : 
* @Date      ：2020-5-22
* @Update    :
**********************************************************************/

void docker_exec_0011(MSG_INFO_T *MsgRecv,int MSGLen)
{
    uint8 payload[2048];
    uint16 index=0;
    int ret =0;
    char cmd[256] = {0};
    uint16 tmplen = 0;
    uint8 dockername[100]={0};
    uint8 type = 0 ;
    char buff[5][500]={{0}};
    BOOL result = TRUE;
    uint8 code[128] = {0};
    memcpy(payload,MsgRecv->MsgData,MsgRecv->MsgLen);
    //总长度
    tmplen = payload[index++];
    memcpy(dockername,&payload[index],tmplen);
    index+=tmplen;
    type = payload[index++];
	if(index!=MsgRecv->MsgLen)   //报文正确性判断
	{
		MQTTMSG_FMT_DEBUG("报文校验错误 index %d msglen %d \n",index,MsgRecv->MsgLen);
		result = FALSE;
	}
    if(tmplen == 0)  //没有容器名
    {
        MQTTMSG_FMT_DEBUG("消息错误，没有容器名\n");
        result = FALSE;
    }
	code[0]=result;
    packge_report_msg(MsgRecv,code,1);
    switch(type)
    {
        case 0:                                //启动容器
        {
            sprintf(cmd,"container start %s ",dockername);
            ret = getshellresult(cmd,buff,5);
            print_exec_info(cmd,buff, ret);
            if(strcmp(buff[0],CON_START_VALUE)==0)
            {
                MQTTMSG_FMT_DEBUG("启动容器成功\n",dockername);
            }
            else
            {
                MQTTMSG_FMT_DEBUG("启动容器失败\n",dockername);
                //result = FALSE;
            }
        }break;
        case 1:                             //停止容器
        {
            sprintf(cmd,"container stop %s ",dockername);
            ret = getshellresult(cmd,buff,5);
            print_exec_info(cmd,buff, ret);
            if(strcmp(buff[0],CON_STOP_VALUE)==0)
            {
                MQTTMSG_FMT_DEBUG("停止容器成功\n",dockername);
            }
            else
            {
                MQTTMSG_FMT_DEBUG("停止容器失败\n",dockername);
                //result = FALSE;
            }

        }break;
        case 2:                         //卸载容器 先停止后卸载 停止已经停止的容器是允许的
        {
            sprintf(cmd,"container uninstall %s ",dockername);
            ret = getshellresult(cmd,buff,5);
            print_exec_info(cmd,buff, ret);
            if(strcmp(buff[0],CON_UNINDTATALL_VALUE)==0)
            {
                MQTTMSG_FMT_DEBUG("卸载容器成功\n",dockername);
            }
            else
            {
                MQTTMSG_FMT_DEBUG("卸载容器失败\n",dockername);
               // result = FALSE;
            }
        }break;
        default:
            break;
    } 

}


/**********************************************************************
* @name      : docker_exec_0010
* @brief     ：容器安装消息执行
* @param[in] ：MSG_INFO_T *MsgRecv 消息报文 int MSGLen 大小
* @param[out]：
* @return    ：
* @Create    : 
* @Date      ：2020-5-22
* @Update    :
**********************************************************************/

void docker_exec_0010(MSG_INFO_T *MsgRecv,int MSGLen)
{
    uint8 *data = MsgRecv->MsgData;
    uint16 msglen = MsgRecv->MsgLen;
    uint8 AppSec=0,AppInfoSec =0;
    uint8 AppSigSec=0,i=0;
    int ret = 0;
    uint16 index=0,tmplen=0, checklen = 0;
    BOOL result = TRUE;
    SEQ_STR app;
    container_image con_image;
    docker_config_T con_info;
    app_info appinfo;
    char cmd[1024]={0};
    char pathname[500]={0};
    char buff[20][500]={0};
    memset(&appinfo,0,sizeof(appinfo));
    memset(&con_info,0,sizeof(docker_config_T));
    memset(&con_image,0,sizeof(container_image));
    memset(&app,0,sizeof(SEQ_STR));
    con_info.dockernamelen  = data[index++];
    memcpy(con_info.dockername,&data[index],con_info.dockernamelen);
    index+=con_info.dockernamelen;
    AppSec = data[index++];
    if(AppSec==1) //有app
    {
        app.len = data[index++];
        memcpy(app.buff,&data[index],app.len);
        index+=app.len;
    }
    if(data[index++] == 1) //有容器镜像
    {
        con_image.ImageNamelen = data[index++];
        memcpy(con_image.ImageName,&data[index],con_image.ImageNamelen);
        index+=con_image.ImageNamelen;
        con_image.ImageSavelen = data[index++];
        memcpy(con_image.ImageSave,&data[index],con_image.ImageSavelen);
        index+=con_image.ImageSavelen;
    }
    con_info.iot_docker_config_T.cpu_core = data[index++];
    memcpy_r(&con_info.iot_docker_config_T.cpu_limit,&data[index],sizeof(uint16));
    index+=sizeof(uint16);
    memcpy_r(&con_info.iot_docker_config_T.mem_limitval,&data[index],sizeof(uint16));
    index+=sizeof(uint16);
    memcpy_r(&con_info.iot_docker_config_T.mem_limit,&data[index],sizeof(uint16));
    index+=sizeof(uint16);
    memcpy_r(&con_info.iot_docker_config_T.disk_limitval,&data[index],sizeof(uint16));
    index+=sizeof(uint16);
    memcpy_r(&con_info.iot_docker_config_T.disk_limit,&data[index],sizeof(uint16));
    index+=sizeof(uint16);
    con_info.iot_docker_config_T.host_folder = data[index++];
    for(i=0;i<con_info.iot_docker_config_T.host_folder;i++)
    {
        con_info.iot_docker_config_T.host_info[i].len = data[index++];
        memcpy(con_info.iot_docker_config_T.host_info[i].buff,&data[index],con_info.iot_docker_config_T.host_info[i].len);
        index+=con_info.iot_docker_config_T.host_info[i].len;
    }
    con_info.iot_docker_config_T.strogefilelen = data[index++]; //存储文件夹
    memcpy(con_info.iot_docker_config_T.stroge_file,&data[index],con_info.iot_docker_config_T.strogefilelen);
    index+=con_info.iot_docker_config_T.strogefilelen;
    con_info.iot_docker_config_T.dev_infolen = data[index++];
    for(i=0;i<con_info.iot_docker_config_T.dev_infolen;i++)
    {
        con_info.iot_docker_config_T.dev_info[i].len = data[index++];
        memcpy(con_info.iot_docker_config_T.dev_info[i].buff,&data[index],con_info.iot_docker_config_T.dev_info[i].len);
        index+=con_info.iot_docker_config_T.dev_info[i].len;
    }
    con_info.iot_docker_config_T.port_para = data[index++];
    memcpy(con_info.iot_docker_config_T.port_val,&data[index],con_info.iot_docker_config_T.port_para);
    index+=con_info.iot_docker_config_T.port_para;
    AppInfoSec = data[index++];
    if(AppInfoSec==1)
    {
        tmplen = data[index++];
        memcpy(appinfo.app_version,&data[index],tmplen);
        index+=tmplen;
        tmplen = data[index++];
        memcpy(appinfo.file_info_t.filename,&data[index],tmplen);
        index+=tmplen;
        tmplen = data[index++];
        memcpy(appinfo.file_info_t.filetype,&data[index],tmplen);
        index+=tmplen;
        tmplen = data[index++];
        memcpy(appinfo.file_info_t.filepath,&data[index],tmplen);
        index+=tmplen;
        memcpy_r(&appinfo.file_info_t.filesize,&data[index],sizeof(uint32));
        index+=sizeof(uint32);
        tmplen = data[index++];
        checklen = tmplen;  // 记录校验值长度
        memcpy(appinfo.file_info_t.filemd5,&data[index],tmplen);
        index+=tmplen;
        AppSigSec = data[index++];
        if(AppSigSec==1)
        {
            tmplen = data[index++];
            memcpy(appinfo.file_info_t.signinfo_t.signname,&data[index],tmplen);
            index+=tmplen;
            tmplen = data[index++];
            memcpy(appinfo.file_info_t.signinfo_t.signpath,&data[index],tmplen);
            index+=tmplen;
            tmplen = data[index++];
            memcpy_r(&appinfo.file_info_t.signinfo_t.signsize,&data[index],sizeof(uint32));
            index+=sizeof(uint32);
            memcpy(appinfo.file_info_t.signinfo_t.signmd5,&data[index],tmplen);
            index+=tmplen;
        }
        appinfo.app_config_t.cpucore = data[index++];
        memcpy_r(&appinfo.app_config_t.cpuLimit,&data[index],sizeof(uint16));
        index+=sizeof(uint16);
        memcpy_r(&appinfo.app_config_t.mem_limit,&data[index],sizeof(uint16));
        index+=sizeof(uint16);
        memcpy_r(&appinfo.app_config_t.memLimit,&data[index],sizeof(uint16));
        index+=sizeof(uint16);
    }    
    do{
        if(index!=msglen)   //报文正确性判断
        {
            MQTTMSG_FMT_DEBUG("报文校验错误 index %d msglen %d \n",index,msglen);
            result = FALSE;
        }
		uint8 result_buf[100]={0};
		result_buf[0]=result;
		packge_report_msg(MsgRecv,result_buf,1);
        //先判断终端内是否有这个容器，如果有先卸载
        sprintf(cmd,"docker ps | grep -w %s",con_info.dockername);  //精准匹配
        ret = getshellresult(cmd,buff,20);
        if(ret==1)
        {
            MQTTMSG_FMT_DEBUG("检查到有同名容器先停止后卸载\n");
            memset(cmd,0,sizeof(cmd));
            memset(buff,0,sizeof(buff));
            sprintf(cmd,"container stop %s;container uninstall %s",con_info.dockername,con_info.dockername);
            ret = getshellresult(cmd,buff,20);
            print_exec_info(cmd,buff,ret);
            if (ret > 0 )
            {
                if(strcmp(buff[0],CON_STOP_VALUE)==0)
                    MQTTMSG_FMT_DEBUG("停止成功\n");
                if(strcmp(buff[1],CON_UNINDTATALL_VALUE)==0)
                    MQTTMSG_FMT_DEBUG("卸载成功\n");
            }
        }

        /*确定文件是否存在*/
        /*判断app文件是否存在*/

        if(AppInfoSec == 1 )    //app文件有效和md5值判断
        {
            sprintf(pathname,"%s/%s",appinfo.file_info_t.filepath,appinfo.file_info_t.filename);
            if(access(pathname,R_OK)!=0)
            {
                MQTTMSG_FMT_DEBUG("路径不存在 %s\n",pathname);
                result = FALSE;
                break;
            }

            if(checklen == 32)
            {
                ret = sizeAndSM3_judge(pathname,appinfo.file_info_t.signinfo_t.signsize,(char *)appinfo.file_info_t.signinfo_t.signmd5);
            }
            else
            {
                ret = sizeAndmd5_judge(pathname,appinfo.file_info_t.signinfo_t.signsize,(char *)appinfo.file_info_t.signinfo_t.signmd5);
            }
            if(ret != 0)
            {
                MQTTMSG_FMT_DEBUG("校验值不一致\n");
                result = FALSE;
                break;
            }
            if(AppSec !=1)
            {
                
            }
            if(AppSec == 1)
            {

            }
            memset(pathname,0,sizeof(pathname));
            memset(cmd,0,sizeof(cmd));
            sprintf(pathname,"%s/%s",appinfo.file_info_t.filepath,appinfo.file_info_t.filename);
            sprintf(cmd,"container install %s %s -v /lib/hal_lib:/lib/hal_lib -v /data/app:/data/app -v /dev:/dev -v /tmp/dev:/tmp/dev -v /tmp/data:/tmp/data -v /data/devinfo:/data/devinfo -v /sys:/sys ",con_info.dockername,pathname);            
            if(AppSigSec==1)   //签名文件有效和md5值判断
            {
                memset(pathname,0,sizeof(pathname));
                sprintf(pathname,"%s/%s",appinfo.file_info_t.signinfo_t.signpath,appinfo.file_info_t.signinfo_t.signname);
                if(access(pathname,R_OK)!=0)
                {
                    result = FALSE;
                    break;
                }
                else
                {
                    if(checklen == 32)
                    {
                        ret = sizeAndSM3_judge(pathname,appinfo.file_info_t.signinfo_t.signsize,(char *)appinfo.file_info_t.signinfo_t.signmd5);
                    }
                    else
                    {
                        ret = sizeAndmd5_judge(pathname,appinfo.file_info_t.signinfo_t.signsize,(char *)appinfo.file_info_t.signinfo_t.signmd5);
                    }
                    if(ret != 0)
                    {
                        result = FALSE;
                        break;
                    } 
                }   
            }
        }
        else
        {          
            memset(pathname,0,sizeof(pathname));
            memset(cmd,0,sizeof(cmd));
            sprintf(cmd,"container install %s -v /data/app:/data/app -v /lib/hal_lib:/lib/hal_lib -v /data/devinfo:/data/devinfo -v /dev:/dev ",con_info.dockername);
        }
        /*组命令*/
        uint16 offset= strlen(cmd);
        for(i=0;i<con_info.iot_docker_config_T.host_folder;i++)
        {
            if(access((char *)con_info.iot_docker_config_T.host_info[i].buff,R_OK)==0)
            {
                sprintf(cmd+offset,"-v %s:%s ",con_info.iot_docker_config_T.host_info[i].buff,con_info.iot_docker_config_T.host_info[i].buff);
            }          
        }
        offset= strlen(cmd);
        if(access((char *)con_info.iot_docker_config_T.stroge_file,R_OK)==0)
        {
            sprintf(cmd+offset,"-v %s:%s ",con_info.iot_docker_config_T.stroge_file,con_info.iot_docker_config_T.stroge_file);
        }
        offset= strlen(cmd);
        for(i=0;i<con_info.iot_docker_config_T.dev_infolen;i++)
        {           
            if(access((char *)con_info.iot_docker_config_T.dev_info[i].buff,R_OK)==0)
            {
                offset= strlen(cmd);
                sprintf(cmd+offset,"-dev %s ",con_info.iot_docker_config_T.dev_info[i].buff);
            }          
        }
        offset= strlen(cmd);
        if(con_info.iot_docker_config_T.cpu_core<2 || con_info.iot_docker_config_T.cpu_core>4)
            con_info.iot_docker_config_T.cpu_core=4;
        if(strlen((char*)con_info.iot_docker_config_T.port_val)!=0&&strlen((char *)con_info.iot_docker_config_T.port_val)!=0)
        {
            sprintf(cmd+offset," -p %d:%d -dev-dir",atoi((char *)con_info.iot_docker_config_T.port_val),atoi((char *)con_info.iot_docker_config_T.port_val));
        }
        offset =strlen(cmd);     
        memset(buff,0,sizeof(buff));
        ret = getshellresult(cmd,buff,20);
        print_exec_info(cmd,buff,ret);
        if(AppInfoSec == 1 && ret>=1)
        {
           if(strcmp(buff[ret-1],CON_INDTALL_VALUE)==0)
           {
                MQTTMSG_FMT_DEBUG("容器安装成功，APP安装成功\n", ret,cmd);
           }
           else
           {
                MQTTMSG_FMT_DEBUG("带APP容器安装失败\n", ret,cmd);
                result = FALSE;
                break;
           }
        }
   
        if(AppInfoSec == 0 && ret>=1)
        {
           if(strcmp(buff[ret-1],CON_INDTALLBLANK_VALUE)==0)
           {
                MQTTMSG_FMT_DEBUG("空容器安装成功\n", ret,cmd); 
           }
           else
           {
                MQTTMSG_FMT_DEBUG("空容器安装失败\n", ret,cmd); 
                result = FALSE;
                break;
           }
        }
        if(ret <= 0)
        {
           MQTTMSG_FMT_DEBUG("容器安装失败 %s %d \n",cmd,ret);  
        }
        /*不需要验证*/
        /*设置APP阈值(option)*/
        if(AppSec==1 && AppInfoSec==1)
        {
            //存在app信息的app名字要用文件名字去掉.tar
            memset(cmd,0,sizeof(cmd));
            memset(buff,0,sizeof(buff));
            char name[100]={0};
            char*p=NULL;
            p=strstr(appinfo.file_info_t.filename,".tar");
        	if(p!=NULL)
            {
                memcpy(name,appinfo.file_info_t.filename,p-appinfo.file_info_t.filename);
                sprintf(cmd,"appm -t -cpu %d -mem %d -c %s -n %s ",appinfo.app_config_t.cpuLimit/100,appinfo.app_config_t.memLimit/100,con_info.dockername,name);
            }
            else
            {
                sprintf(cmd,"appm -t -cpu %d -mem %d -c %s -n %s ",appinfo.app_config_t.cpuLimit/100,appinfo.app_config_t.memLimit/100,con_info.dockername,app.buff);
            }
            ret = getshellresult(cmd,buff,20);
            print_exec_info(cmd,buff,ret);
        }
        memset(cmd,0,sizeof(cmd));
        memset(buff,0,sizeof(buff));
        ret = container_exec_0012(&con_info);
        if(ret != 0)
        {
            result = FALSE;
            break;
        }
    }while(0);
    /*执行安装命令*/
    /*配置参数（容器和app）*/
    /*返回报文和操作告警*/
//在报文判断后直接就回复报文。
}


int iot_msgproc(void *pArg, void *pPublic, void *pPrivate)
{
    int RecvLen;
    WGPubattr *public    =  (WGPubattr *)pPublic;
    MSG_INFO_T  *MsgRecv = NULL;
    uint8 recvBuff[2173] = {0};
    RecvLen = taskmng_plc_queue_read(&public->msgque, recvBuff);
    if(RecvLen > 0)
    {
        MsgRecv = (MSG_INFO_T *)recvBuff;
        MQTTMSG_FMT_DEBUG("接收到上行的报文 IID %04x IOP %04x \n",MsgRecv->IID,MsgRecv->IOP);
        if(MsgRecv->IID != 0x000B)
        {
            MQTTMSG_FMT_DEBUG("上行报文IID有问题，IID %04x \n",MsgRecv->IID);
        }
        switch (MsgRecv->IOP)
        {
            case 0x0010:
            {
              docker_exec_0010(MsgRecv,RecvLen); 
            }break;
            case 0x0011:            //容器操作
            {
              docker_exec_0011(MsgRecv,RecvLen);
            }break;
            case 0x0012:
            {
              docker_exec_0012(MsgRecv,RecvLen);
            }break;
            case 0x0013:
            {
              docker_exec_0013(MsgRecv,RecvLen);  
            }break;
            case 0x0014:
            {
              docker_exec_0014(MsgRecv,RecvLen);
            }break;
            case 0x0015:
            {
             docker_exec_0015(MsgRecv,RecvLen);   
            }break;
            case 0x0016:
            {
              docker_exec_0016(MsgRecv,RecvLen);   
            }break;
            case 0x0017:   
            {
              docker_exec_0017(MsgRecv,RecvLen);   
            }break;
            case 0x0018:
            {
                    //许可配置
               docker_exec_0018(MsgRecv,RecvLen);  
            }break;
            case 0x0019:
            {
                //许可查询 
                docker_exec_0019(MsgRecv,RecvLen);
            }break;
            case 0x001A:
            {
                //目前没有命令 系统升级
                docker_exec_001A(MsgRecv,RecvLen);
            }break;
            case 0x001B:
            {
               docker_exec_001B(MsgRecv,RecvLen); 
            }break;
            case 0x001C:
            {
                // 日志信息查询  没做
                docker_exec_001C(MsgRecv,RecvLen); 
            }break;
            case 0x001D:
            {
                //日志打包     日志类型没做
                docker_exec_001D(MsgRecv,RecvLen); 
            }break;
            case 0x001E:
            {
                //日志删除     日志类型没做
                docker_exec_001E(MsgRecv,RecvLen); 
            }break;
            case 0x001F:
            {
                //系统配置参数     
                docker_exec_001F(pPublic,MsgRecv,RecvLen); 
            }break;
            default :
                break;
        }

    }
    return 5000;
}


int iot_msginit(void *pPublic, void **ppPrivate)
{
    MQTTMSG_FMT_DEBUG("msg 线程启用 \n");
    return 0;
}


