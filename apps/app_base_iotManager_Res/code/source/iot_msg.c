/*
*********************************************************************
* Copyright(C) 2020 �Ͼ��������ӹɷ����޹�˾
* All rights reserved. 
* @brief��   ��·������̹߳���
* @date��    2020-6-30
* @history�� 
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
    MQTTMSG_FMT_DEBUG("����ִ�� :\n %s \n",cmd); 
    MQTTMSG_FMT_DEBUG("����ֵ : \n"); 
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
        case 0: //ж������
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
        case 1: //��װ����
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
* @brief     ����ظ�����
* @param[in] ��MSG_INFO_T *MsgRecv ��Ϣ���� buff �ظ��ı��� msglen �ظ����ĳ���
* @param[out]��
* @return    ��
* @Create    : 
* @Date      ��2020-5-22
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
    //ֱ�ӷ���ȷ��֡
    char topic[1000]={0};
    sprintf(topic,"%s/%s",MsgSend->SourAddr,MsgSend->DestAddr);
    //����Ϣ
    COMM_FMT_DEBUG("topic %s IID %04x IOP %04x\n",topic,MsgSend->IID,MsgSend->IOP);
    MSGLen = iot_appmsg_package(MsgSend,&MSGBuf);    
    COMM_BUF_DEBUG(MSGBuf, MSGLen,"");
    mqtt_topic_write(MSGBuf, MSGLen,topic); 
}

/**********************************************************************
* @name      : get_alldocker_with_app
* @brief     ����ϢApp��������ִ��
* @param[in] ��MSG_INFO_T *MsgRecv ��Ϣ,int MSGLen ����
* @param[out]��
* @return    ��
* @Create    : 
* @Date      ��2020-5-22
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
* @brief     : ��ȡ����������app����Ϣ
* @param[in] ��MSG_INFO_T *MsgRecv ��Ϣ,int MSGLen ����
* @param[out]��
* @return    ��
* @Create    : 
* @Date      ��2020-5-22
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
* @brief     ��App�澯֪ͨ
* @param[in] ��docker ���� app app ,type��������{��װ��0����������1����ж�أ�2����������4����ֹͣ��4�������ò�����5����ʹ��,6����ȥʹ�ܣ�7�����·���ɣ�8��}��
* @param[out]��
* @return    ��
* @Create    : 
* @Date      ��2020-5-22
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
* @brief     ��ִ��App��һϵ�в���
* @param[in] ��docker ���� app app ,type��������enum{������0����ֹͣ��1����ж�أ�2����ʹ�ܣ�3����ȥʹ�ܣ�4��}
* @param[out]��
* @return    ��
* @Create    : 
* @Date      ��2020-5-22
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
        case  0: // ����
        {
            sprintf(cmd,"appm -s -c %s -n %s | grep started",docker->buff,app->buff);
        }break;
        case 1: //ֹͣ
        {
            sprintf(cmd,"appm -S -c %s -n %s | grep stopped",docker->buff,app->buff);
        }break;
        case 2: //ж��
        {
            sprintf(cmd,"appm -u -c %s -n %s | grep Uninstalled",docker->buff,app->buff);
        }break;
        case 3:  //ʹ��
        {
            sprintf(cmd,"appm -e -c %s -n %s | grep enabled",docker->buff,app->buff);
        }break;
        case 4: //ȥʹ��
        {
            sprintf(cmd,"appm -d -c %s -n %s | grep disenabled",docker->buff,app->buff);
        }break;
        default:
            break;
    }
	//�ж����ʱ������Ҫ�����app��û�����ע���� ������ע������Ϊ36��0��ʱ�򣬲������ظ����� ̨�����Ӧ�ù���
    if(type == 0)
    {
     //   app_license_T liscen;
     //   memset(&liscen,0,sizeof(liscen));
    }
    uint8 ret = 0;
    ret = getshellresult(cmd,buff,20);
    MQTTMSG_FMT_DEBUG("ִ��APP���� CMD %s ret %d \n",cmd,ret);
    print_exec_info(cmd, buff,ret);
    if(ret ==0)
    {
        
        errcode = FALSE;
        MQTTMSG_FMT_DEBUG("recv cmd %s\n",buff[0]);
    }
    //����֤һ��
    //ȡ�����죬��ΪֻҪ����û�����⣬��ȷ�������⡣
    
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
    index+=tmp;           //�˿���Դ���ò���
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
            //ȫ������
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
* @brief     : ������־���Ͳ�������
* @param[in] ��MSG_INFO_T *MsgRecv ��Ϣ,int MSGLen ����
* @param[out]��
* @return    ��
* @Create    : 
* @Date      ��2020-5-22
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
* @brief     : ������־·����������
* @param[in] : exec �������� 0 ��ȡ��Ϣ 1. ��� 2. ɾ�� filePath �������ļ���·�� 
* @param[out]��recvBuf �ظ����ģ�msglen ���ĳ���
* @return    ��0 �ɹ� -1 ʧ��
* @Create    : 
* @Date      ��2020-5-22
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
* @brief     : ������־·�����log�ṹ�庯��
* @param[in] : path ��־·��
* @param[out]: log �����log�ṹ��
* @return    : �ַ���ʱ���׼:  1970-01-01 00:00:00
* @Create    : 
* @Date      ��2020-5-22
* @Update    :
**********************************************************************/

int get_logFile_info(log_info_T *log,char *path,uint8 type)
{
    //�޸�Ϊ��log�����ȡ
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
//                log->log[0].md5len = strlen(log->log[0].logMd5);  //24�淶����SM3���룬���κ��������ֶ�����
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
        MQTTMSG_FMT_DEBUG("��־��С ��%d\n", log->log[0].logSize);
        
        ret = GetFileSM3(log->log[0].logPath, 32, (uint8 *)log->log[0].logMd5);
        if(ret != 0)
        {
            MQTTMSG_FMT_DEBUG("��ȡ[%s]SM3����ʧ��, %s\n", ret == -2 ? "�ļ�������" : "����ԭ��");
        }
        else
        {
            log->log[0].md5len = 32;
            MQTTMSG_BUF_DEBUG(log->log[0].logMd5, log->log[0].md5len, "��ȡ[%s]SM3����ɹ�", log->log[0].logPath);
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
                //MD5ת��
                uint8 md5[100]={0};
                uint8 md5len =0;
                
                md5len = strt2octet(log->log[i].logMd5,log->log[i].md5len,md5,64);
                if(md5len > 0)
                {
                    MQTTMSG_BUF_DEBUG(md5,md5len,"ת�����md5ֵ ��");
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
        MQTTMSG_FMT_DEBUG("��־ɾ���ɹ� \n");
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
        MQTTMSG_FMT_DEBUG("ERROR û�м�⵽���� \n");
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
            MQTTMSG_FMT_DEBUG("ERROR ����ִ��û�з��ػ򷵻ز���ȷ\n %s \n",cmd);
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
//             UPDATE_FMT_DEBUG("ERROR ����ִ��û�з��ػ򷵻ز���ȷ\n ret %d\n %s \n", ret ,cmd);
               break;
             }
            sleep(1);
            num ++;
        }
        while (num  <=3);

//      ret = getshellresult(cmd,buff,DOCKERMAXLEN);

        if(ret !=3)
         {
           UPDATE_FMT_DEBUG("ERROR ����ִ��û�з��ػ򷵻ز���ȷ\n ret %d\n %s \n", ret ,cmd);
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
           MQTTMSG_FMT_DEBUG("ERROR ����ִ��û�з��ػ򷵻ز���ȷ\n ret %d\n %s \n", ret ,cmd);
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
* @brief     : ��ȡApp����Ϣ
* @param[in] : containername �������� 
* @param[out]��appinfo       app��Ϣ
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
            /*��ֵתΪkb*/
            //retu_kbval(valuetrim); 
            char *p = strstr(valuetrim,"kB");
            if(p==NULL)
            {
                //˵�����õ���M
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
                UPDATE_FMT_DEBUG("����Ĳ��� memlimit %d \n",memlimit);
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
            MQTTMSG_BUF_DEBUG(tmplincen,index,"lincenseת�����ֵ");
            char tmpcode[30]= {0};
            if(memcmp(tmplincen,tmpcode,24)==0)
            {
                MQTTMSG_FMT_DEBUG("ƥ�䵽24λ0 ��Ϣ�ظ�����\n");
                result = FALSE;
                break;
            }
        }
    }
    return result;
}

/**********************************************************************
* @name      : docker_exec_001F
* @brief     ����ϢApp��������ִ��
* @param[in] ��MSG_INFO_T *MsgRecv ��Ϣ,int MSGLen ����
* @param[out]��
* @return    ��
* @Create    : 
* @Date      ��2020-5-22
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
    UPDATE_FMT_DEBUG("ϵͳ��ֵ�������\n");
    INIT_FMT_DEBUG("ϵͳ�������� devCpuLimit %d ��devMemLimit %d devDiskLimit %d devCpuTempup %d devCpuTempdown %d monitorTime %d \n",pData->systheadinfo.sysconfig.devCpuLimit,pData->systheadinfo.sysconfig.devMemLimit,pData->systheadinfo.sysconfig.devDiskLimit
    ,pData->systheadinfo.sysconfig.devCpuTempup, pData->systheadinfo.sysconfig.devCpuTempdown,pData->systheadinfo.sysconfig.monitorTime);
    //��monitorctl ���ò���
#if 0
    char cmd[500] = {0};
    sprintf(cmd,"monitorctl -T cpu %d;monitorctl -T memory %d;monitorctl -T storage %d;monitorctl -T temperature %d;",sysconf.cpulimit/100,sysconf.memLimit/100,sysconf.diskLimit/100,sysconf.cputemp_up/10);
    ret = getshellresult(cmd,buff,10);
    print_exec_info(cmd,buff,ret);
    if(strcmp(buff[0],SYS_SETCPU_VALUE)==0)
    {
        MQTTMSG_FMT_DEBUG("ϵͳ����CPU��ֵ�ɹ� \n");
    }
    if(strcmp(buff[1],SYS_SETMEM_VALUE)==0)
    {
        MQTTMSG_FMT_DEBUG("ϵͳ�����ڴ���ֵ�ɹ� \n");
    }
    if(strcmp(buff[2],SYS_SETSTO_VALUE)==0)
    {
        MQTTMSG_FMT_DEBUG("ϵͳ���ô洢��ֵ�ɹ� \n");
    }
    if(strcmp(buff[3],SYS_SETTEMP_VALUE)==0)
    {
        MQTTMSG_FMT_DEBUG("ϵͳ�����¶�������ֵ�ɹ� \n");
    }
#endif
    recvBUFF[0] = result;
    packge_report_msg(MsgRecv,recvBUFF,1);
}


/**********************************************************************
* @name      : docker_exec_001E
* @brief     ����ϢApp��������ִ��
* @param[in] ��MSG_INFO_T *MsgRecv ��Ϣ,int MSGLen ����
* @param[out]��
* @return    ��
* @Create    : 
* @Date      ��2020-5-22
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
        if(data[0]==0)       //��־�ļ�·�� [0]  visible-string   
        {
            filelen = data[index++];
            memcpy(logFile,&data[index],filelen);
            index+=filelen;
            result  = judge_path_exec(logFile,0xFF);
            break;
        }
        else if(data[0]==1) //��־����     [1]  LOGTYPE
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
* @brief     : 001D ��־���
* @param[in] ��MSG_INFO_T *MsgRecv ��Ϣ,int MSGLen ����
* @param[out]��
* @return    ��
* @Create    : 
* @Date      ��2020-5-22
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
        if(data[0]==0)            //��־�ļ�·�� [0]  visible-string��
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

            /*�ж����ļ������ļ���*/
        }
        else if(data[0]==1)     //��־����     [1]  LOGTYPE
        {
            uint8 type = data[index++];
            operate_accord_logType(type,SourcePath);
            option   = data[index++];
            if(option == 0)  //Ĭ���ļ�����
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
            /*�ж���Ŀ¼�����ļ�*/
            int filetype = judge_path_fold_dir(tmppackPath);
            if(filetype == -1 )
            {
                retype =0;
                MQTTMSG_FMT_DEBUG("path error %s\n",tmppackPath);
                break;
            }else if(filetype == 0)
            {
                //�ļ�
                sprintf(packPath,"%s",tmppackPath);
            }else
            {
                //Ŀ¼
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
* @brief     : ��ϢApp��������ִ��
* @param[in] : MSG_INFO_T *MsgRecv ��Ϣ,int MSGLen ����
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
        if(data[0] == 0)       //��־����
        {
            filelen = data[index++];
            memcpy(logFile,&data[index],filelen);         //��·�����ļ����� 
            index+=filelen;
            MQTTMSG_FMT_DEBUG("����־·�����ٲ���־��Ϣ ��־·�� %s \n",logFile);
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
        }else if(data[0] == 1) //��־����
        {
            uint8 type = data[index++];
            MQTTMSG_FMT_DEBUG("����־�������ٲ���־��Ϣ ��־���� %d \n", type);
            if(type == 0)
                type = 1;           //Ŀǰ��֧��ȫ�������ļ���ȡ
            get_logFile_info(&loginfo,logFile,type);
        }       
        else 
        {
            MQTTMSG_FMT_DEBUG("��־�������� ��־���� %d \n", data[0]);
            break;            
        }
    }while(0);
    recvlen = pack_report_fileinfo(&loginfo,recvBUFF);
    packge_report_msg(MsgRecv,recvBUFF,recvlen);
}


/**********************************************************************
* @name      : docker_exec_001B
* @brief     ����ϢApp��������ִ��
* @param[in] ��MSG_INFO_T *MsgRecv ��Ϣ,int MSGLen ����
* @param[out]��
* @return    ��
* @Create    : 
* @Date      ��2020-5-22
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
    strcpy(hardversion,"DM01");  //��ʱд��
    tmpData[index++]=strlen(sysversion);
    strcpy(&tmpData[index],sysversion);
    index+=strlen(sysversion);
    tmpData[index++]=strlen(kerversion);
    strcpy(&tmpData[index],kerversion);
    index+=strlen(kerversion);
    tmpData[index++] = 0;//�ļ�ϵͳ�汾ΪNULL
    tmpData[index++]=strlen(hardversion);
    strcpy(&tmpData[index],hardversion);
    index+=strlen(hardversion);
    packge_report_msg(MsgRecv,(uint8 *)tmpData,index);  //Ӧ��
}


/**********************************************************************
* @name      : docker_exec_0019
* @brief     ����ϢApp��������ִ��
* @param[in] ��MSG_INFO_T *MsgRecv ��Ϣ,int MSGLen ����
* @param[out]��
* @return    ��
* @Create    : 
* @Date      ��2020-5-22
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
            //���ش���
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
        checklen = tmplen;   //��¼һ��У�鳤��
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
            MQTTMSG_FMT_DEBUG("������֤���� index %d msglen %d \n",index,msglen);
            result = FALSE;
            break;
        }
        //��֤�����ļ��Ƿ����
       memset(filePath,0,sizeof(filePath));
       sprintf(filePath,"%s/%s",sys_info.file_info_t.filepath,sys_info.file_info_t.filename);
       
       if(checklen == 32) //32λ��ʾΪsm3У��
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
       else // MD5У��
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
            MQTTMSG_FMT_DEBUG("�����ļ���С����[%s]ֵУ��������\n", tmplen == 32 ? "SM3" : "MD5");
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
                if(checklen == 32) //32λ����SM3У��
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
    //ת��md5ֵ
    recvBuff[0] = result;
    packge_report_msg(MsgRecv,recvBuff,1);
    if(FALSE == result)
    {
        return;
    }
    //�Ȼ�ȡһ��md5ֵ
    memset(cmd,0,sizeof(cmd));
    memset(buff,0,sizeof(buff));
    ret=0;
    
    checklen = 16;  //����upgrade������δ֧��sm3���룬�����ֶ������������ʱ�õ��������upgrade����������ֱ��ɾ�����м���
    
    if(checklen == 32)  //SM3У��ֵ32λ
    {
        char tmpfilepath[128] = {0};
        snprintf(tmpfilepath, 128, "%s%s", sys_info.file_info_t.filepath,sys_info.file_info_t.filename);
        ret = GetFileSM3(tmpfilepath, 255, filesm3);
        if(ret != 0)
        {
            MQTTMSG_FMT_DEBUG("��ȡ%s SM3У��ʧ��\n", tmpfilepath);
        }
    }
    else
    {
        sprintf(cmd,"md5sum %s%s | awk '{print $1}'",sys_info.file_info_t.filepath,sys_info.file_info_t.filename);
        ret = getshellresult(cmd,buff,1);
        //��ӡ����ֵ
        MQTTMSG_BUF_DEBUG(buff, ret,"ִ������ ��%s \n",cmd);
    }

    
    //ƴ������
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

    if(checklen == 32)  //SM3У��ֵ32λ
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

     //ִ�������Ҫ����ֵ
     //�Ȼظ���ȷ�������ļ�
    MQTTMSG_FMT_DEBUG("��⵽ϵͳ����  ���� CMD: %s\n",cmd);
    ret = getshellresult(cmd, buff, 20);
    if(ret > 0 )
    {
       // print_exec_faild(ret,buff);
    }

  //  memset(filePath,0,sizeof(filePath));
 //   sprintf(filePath,"%s%s",PrivateDir,"SYSUPDATE");
  //  write_pdata_xram(filePath,(char *)&sys_info,0,sizeof(sys_update_info));
    system(cmd);

    //��ʱ���� �����޸İ汾��
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
* @brief     ����ϢApp��������ִ��
* @param[in] ��MSG_INFO_T *MsgRecv ��Ϣ,int MSGLen ����
* @param[out]��
* @return    ��
* @Create    : 
* @Date      ��2020-5-22
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

		
        /*��Ҫ��Щapp��LICENSE*/
        if(data[index]==0)
        {
            //ȫ��
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
            //��Ϣ�����д���
            MQTTMSG_FMT_DEBUG("��Ϣ�����д��� \n");
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

			if(i == liscen.licennum)	//û���ҵ�
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
* @brief     ����ϢApp��������ִ��
* @param[in] ��MSG_INFO_T *MsgRecv ��Ϣ,int MSGLen ����
* @param[out]��
* @return    ��
* @Create    : 
* @Date      ��2020-5-22
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
* @brief     ����ϢApp��������ִ��
* @param[in] ��MSG_INFO_T *MsgRecv ��Ϣ,int MSGLen ����
* @param[out]��
* @return    ��
* @Create    : 
* @Date      ��2020-5-22
* @Update    :
**********************************************************************/

void docker_exec_0018(MSG_INFO_T *MsgRecv,int MSGLen)
{
    MQTTMSG_FMT_DEBUG("�յ����ע�����·�ָ�� \n");
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
* @brief     ����ϢApp��������ִ��
* @param[in] ��MSG_INFO_T *MsgRecv ��Ϣ,int MSGLen ����
* @param[out]��
* @return    ��
* @Create    : 
* @Date      ��2020-5-22
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
	//�Ȱ���Ҫ��ȡ���ٸ�app��Ϣ
    if(data[index]==0 && data[index+1]==0)
    {
	    //û������Ҳû��app ȫ����APP
	    index+=2;
	    char cmd[100] = "docker ps --format '{{.Names}}'";
		char buff[10][500]={{0}};
		uint8 count= 0;
		count = getshellresult(cmd,buff,10);
		MQTTMSG_FMT_DEBUG("��ȡ��ǰ�ն������е�������Ϣ����������Ϊ%d  \n",count);
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
            MQTTMSG_FMT_DEBUG("��ȡ��ǰ�ն������е�������Ϣ����������Ϊ%d  \n",app_info.appcount);
		}    
    }
    else if(data[index]==0 && data[index+1]==1)
    {
        /*�������������������ֵ�app*/
        index = 2;
        app.len = data[index++];
        memcpy(app.buff,&data[index],app.len);
        index+=app.len;
		MQTTMSG_FMT_DEBUG("��ȡ��ǰ�ն�������������ͬapp��Ϣ %s \n",docker.buff);
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
    /*��ȡ��Ӧ����ϢȻ���鱨��*/
   if(index!=MsgRecv->MsgLen)
    {
        MQTTMSG_FMT_DEBUG("���Ľ������� index %d msglen %d\n",index,MsgRecv->MsgLen);
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
* @brief     ����ϢApp��������ִ��
* @param[in] ��MSG_INFO_T *MsgRecv ��Ϣ,int MSGLen ����
* @param[out]��
* @return    ��
* @Create    : 
* @Date      ��2020-5-22
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
    app_actinfo.docker.len = MsgRecv->MsgData[offset++]; //�������Ƴ���
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
    MQTTMSG_FMT_DEBUG("App �������óɹ� cmd %s\n",cmd);
    
    result_buff[0]=result;
    packge_report_msg(MsgRecv,result_buff,1);  //Ӧ��
}


/**********************************************************************
* @name      : docker_exec_0015
* @brief     ����ϢApp��������ִ��
* @param[in] ��MSG_INFO_T *MsgRecv ��Ϣ,int MSGLen ����
* @param[out]��
* @return    ��
* @Create    : 
* @Date      ��2020-5-22
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
	if(index!=MsgRecv->MsgLen)   //������ȷ���ж�
	{
		MQTTMSG_FMT_DEBUG("����У����� index %d msglen %d \n",index,MsgRecv->MsgLen);
		errcode = FALSE;
	}
    if(type == 0)
    {
        //���ж����Ƿ�ƥ��24��0
        errcode = judge_linsence_legal(&app);
    }
	buff[0]=errcode;
    packge_report_msg(MsgRecv,buff,1); 
    App_comand_exec(&docker,&app,type);     
}

/**********************************************************************
* @name      : docker_exec_0014
* @brief     ����Ϣ0014App��װ����ִ��
* @param[in] ��MSG_INFO_T *MsgRecv ��Ϣ,int MSGLen ����
* @param[out]��
* @return    ��
* @Create    : 
* @Date      ��2020-5-22
* @Update    :
**********************************************************************/

void docker_exec_0014(MSG_INFO_T *MsgRecv,int MSGLen)
{
    uint16 index=0;
    uint8 option = 0,ret = 0,exectype = 0;//0Ϊ��װ
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
    tmplen = MsgRecv->MsgData[index++];     //Ӧ�ð汾��
    memcpy(app_install.app_config_t.app_version,&MsgRecv->MsgData[index],tmplen);
    index +=tmplen;
    tmplen = MsgRecv->MsgData[index++];    //�ļ�������
    memcpy(app_install.app_config_t.file_info_t.filename,&MsgRecv->MsgData[index],tmplen);
    index +=tmplen;
    tmplen = MsgRecv->MsgData[index++];    //�ļ�������
    memcpy(app_install.app_config_t.file_info_t.filetype,&MsgRecv->MsgData[index],tmplen);
    index +=tmplen;
    tmplen = MsgRecv->MsgData[index++];    //�ļ�·��
    memcpy(app_install.app_config_t.file_info_t.filepath,&MsgRecv->MsgData[index],tmplen);
    index +=tmplen;
    memcpy_r(&app_install.app_config_t.file_info_t.filesize,&MsgRecv->MsgData[index],sizeof(uint32));
    index +=sizeof(uint32);                //�ļ���С
    tmplen = MsgRecv->MsgData[index++];    //�ļ��� md5 ֵ 
    checklen = tmplen;                     //��¼У��ֵ�ĳ���
    memcpy(app_install.app_config_t.file_info_t.filemd5,&MsgRecv->MsgData[index],tmplen);
    index +=tmplen;
    option = MsgRecv->MsgData[index++];
    //�ļ�������ǩ֤��Ϣ  OPTIONAL
    if(option == 1)
    {
        tmplen = MsgRecv->MsgData[index++];    //����֤���ļ�����
        memcpy(app_install.app_config_t.file_info_t.signinfo_t.signname,&MsgRecv->MsgData[index],tmplen);
        index +=tmplen;
        tmplen = MsgRecv->MsgData[index++];    //����֤���ļ�·��
        memcpy(app_install.app_config_t.file_info_t.signinfo_t.signpath,&MsgRecv->MsgData[index],tmplen);
        index +=tmplen;
        memcpy_r(&app_install.app_config_t.file_info_t.signinfo_t.signsize,&MsgRecv->MsgData[index],sizeof(uint32));
        index +=sizeof(uint32); 
        tmplen = MsgRecv->MsgData[index++];    //����֤���ļ���md5ֵ
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
	if(index!=MsgRecv->MsgLen)   //������ȷ���ж�
	{
		MQTTMSG_FMT_DEBUG("����У����� index %d msglen %d \n",index,MsgRecv->MsgLen);
		result = FALSE;
	}
	uint8 tmpbuff[10]={0};
	tmpbuff[0]=result;
	packge_report_msg(MsgRecv,tmpbuff,1);
   do{
        if(option == 1)
        {
            //У��һ������ǩ���ļ��Ƿ����
            sprintf(pathname,"%s%s",app_install.app_config_t.file_info_t.signinfo_t.signpath,app_install.app_config_t.file_info_t.signinfo_t.signname);
            if(access(pathname,R_OK)!=0)
            {
                result = FALSE;
                break;
            }
            else
            {
                if(checklen == 32)  //У��ֵ����Ϊ32 SM3У��
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

        /*�ж������ļ��治����*/
        memset(pathname,0,sizeof(pathname));
        sprintf(pathname,"%s/%s",app_install.app_config_t.file_info_t.filepath,app_install.app_config_t.file_info_t.filename);
        if(access(pathname,R_OK)!=0)
        {
            MQTTMSG_FMT_DEBUG("����ļ��Ƿ���� CMD %s\n ",pathname);
            result = FALSE;
            break;
        }
        
        if(checklen == 32)  //У��ֵ����Ϊ32 SM3У��
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
        //���жϵ�ǰ�ն��Ƿ���ڸ�������App�����������Ϊ������������Ϊ��װ
        sprintf(filePath,"appm -I -c %s -n %s",app_install.docker.buff,app_install.app.buff); 
        ret = getshellresult(filePath,buff,20);
        print_exec_info(filePath, buff, ret);
        if(ret > 1)
        {
            MQTTMSG_FMT_DEBUG("����ͬ��app����Ϊ����\n");
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
         //��ָ�װapp
        //appm -i -cpu 4 -mem 50  -t -cpu 70 -t -mem 80 -c c_master_yx -n puAmr -f puAmr.tar
        memset(filePath,0,sizeof(filePath));
        sprintf(filePath,"%s%s",app_install.app_config_t.file_info_t.filepath,app_install.app_config_t.file_info_t.filename); 
        memset(pathname,0,sizeof(pathname));
        memset(buff,0,sizeof(buff));

		if(exectype == 0)
        	sprintf(pathname,"appm -i -cpu %d -mem %d -c %s -n %s -f %s | grep Succeed",app_install.app_config_t.app_config_t.cpucore,app_install.app_config_t.app_config_t.mem_limit, app_install.docker.buff,app_install.app.buff,filePath);
        else
        	sprintf(pathname,"appm -U -c %s -n %s -f %s | grep Succeed", app_install.docker.buff,app_install.app.buff,filePath);
        MQTTMSG_FMT_DEBUG("ִ�а�װ/�������� CMD -- %s\n",pathname);
        uint8 tmp_num = 0;
        do{
            memset(buff,0,sizeof(buff));
            ret = getshellresult(pathname,buff,10);
            if (ret == 0)
            {
                MQTTMSG_FMT_DEBUG("ʧ������ %d \n",tmp_num);
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
        if(exectype == 0)  //�����ò������Ѳ���������
        {
            memset(buff,0,sizeof(buff));
            memset(pathname,0,sizeof(pathname));
            sprintf(pathname,"appm -t -cpu %d -mem %d -c %s -n %s",app_install.app_config_t.app_config_t.cpuLimit,app_install.app_config_t.app_config_t.memLimit,app_install.docker.buff,app_install.app.buff);
            uint8 num_T=0;
            do{
                ret = getshellresult(pathname,buff,3);
                {
                    MQTTMSG_FMT_DEBUG("ִ�в����������� CMD -- %s \n",pathname);
                    if(ret != 0)
                    {
                        MQTTMSG_FMT_DEBUG("��������ʧ�� \n");
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
                MQTTMSG_FMT_DEBUG("��������ʧ�� \n");
                result = FALSE;
                break;
            }
        }
    
    }while(0);
   
}


/**********************************************************************
* @name      : docker_exec_0013
* @brief     ��������ѯ��Ϣִ��
* @param[in] ��MSG_INFO_T *MsgRecv ��Ϣ���� int MSGLen ��С
* @param[out]��
* @return    ��
* @Create    : 
* @Date      ��2020-5-22
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
                MQTTMSG_FMT_DEBUG("��Լ������ index %d msglen %d \n",index,MsgRecv->MsgLen);
            }
            //��ȡȫ����������Ϣ
            MQTTMSG_FMT_DEBUG("��ȡȫ��������Ϣ \n");
        }
        else if(option == 1)
        {
            dockername.len = data[index++];
            memcpy(dockername.buff,&data[index],dockername.len);
            index += dockername.len;
            if(MsgRecv->MsgLen != index)
            {
                MQTTMSG_FMT_DEBUG("��Լ������ index %d msglen %d \n",index,MsgRecv->MsgLen);
            }
            //��ȡ�����������Ϣ
            MQTTMSG_FMT_DEBUG("��ȡ����[%s]��Ϣ \n",dockername.buff);
        }else
        {
            MQTTMSG_FMT_DEBUG("��Լ������ index %d msglen %d \n",index,MsgRecv->MsgLen);
        }
        get_coninfo_all(&allcontainerinfo);
        MQTTMSG_FMT_DEBUG("��ǰ�ն˹��� %d ���� \n",allcontainerinfo.containercount);
        index = packge_0013_report(&dockername,payload,&allcontainerinfo);
    }while(0);
             
    packge_report_msg(MsgRecv,payload,index); 
}


/*******************************************************************************
* ��������: container_exec_0012
* ��������: ִ��0012��������
* ���������docker_config_T     �������ýṹ��
*       
* ���������
* �ڲ�����: ���ж������Ƿ�ִ�У��ж�����ִ�вſ��Խ��ж�̬���ò���
* �� �� ֵ: uint8                0 ��ȷ ��������
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
    //Ŀǰֻ֧��һ��һ�������������

    sprintf(cmd,"container config %s -cpu %d -mem %d -disk %d ",tmpconfig->dockername,tmpconfig->iot_docker_config_T.cpu_core,tmpconfig->iot_docker_config_T.mem_limitval,\
       tmpconfig->iot_docker_config_T.disk_limitval);
    memset(buff,0,sizeof(buff));
    MQTTMSG_FMT_DEBUG("������������\n");
    ret = getshellresult(cmd,buff,10);
    print_exec_info(cmd,buff,ret);
    if(strcmp(buff[ret-1],CON_CONFIG_VALUE)==0)
    {
        MQTTMSG_FMT_DEBUG("�����������óɹ� \n");
    }
    else
    {
        MQTTMSG_FMT_DEBUG("������������ʧ�� \n");
    }

    memset(cmd,0x00,sizeof(cmd));
    memset(buff,0,sizeof(buff));
    MQTTMSG_FMT_DEBUG("������ֵ����\n");
    sprintf(cmd,"container monitor -t cpu %d %s",tmpconfig->iot_docker_config_T.cpu_limit/100,tmpconfig->dockername);
    system(cmd);  
    memset(cmd,0x00,sizeof(cmd));
    sprintf(cmd,"container monitor -t  memory %d %s ",tmpconfig->iot_docker_config_T.mem_limit/100,tmpconfig->dockername);
    system(cmd);     
    memset(cmd,0x00,sizeof(cmd));
    sprintf(cmd,"container monitor -t storage %d %s ",tmpconfig->iot_docker_config_T.disk_limit/100,tmpconfig->dockername);
    system(cmd); 
    MQTTMSG_FMT_DEBUG("������ֵ���óɹ�\n");

    /*���ص��������ļ��� 	array visible-string,
    Ӧ�ô洢�ļ�·��      visible-string,
    ӳ���Ӳ���ӿ�     	array visible-string,
    �˿���Դ���ò���      visible-string
    ��ʱ����
    */
    return 0;
}

/**********************************************************************
* @name      : docker_exec_0012
* @brief     ������������Ϣִ��
* @param[in] ��MSG_INFO_T *MsgRecv ��Ϣ���� int MSGLen ��С
* @param[out]��
* @return    ��
* @Create    : 
* @Date      ��2020-5-22
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
	if(index!=MsgRecv->MsgLen)   //������ȷ���ж�
	{
		MQTTMSG_FMT_DEBUG("����У����� index %d msglen %d \n",index,MsgRecv->MsgLen);
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
* @brief     ������������Ϣִ��
* @param[in] ��MSG_INFO_T *MsgRecv ��Ϣ���� int MSGLen ��С
* @param[out]��
* @return    ��
* @Create    : 
* @Date      ��2020-5-22
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
    //�ܳ���
    tmplen = payload[index++];
    memcpy(dockername,&payload[index],tmplen);
    index+=tmplen;
    type = payload[index++];
	if(index!=MsgRecv->MsgLen)   //������ȷ���ж�
	{
		MQTTMSG_FMT_DEBUG("����У����� index %d msglen %d \n",index,MsgRecv->MsgLen);
		result = FALSE;
	}
    if(tmplen == 0)  //û��������
    {
        MQTTMSG_FMT_DEBUG("��Ϣ����û��������\n");
        result = FALSE;
    }
	code[0]=result;
    packge_report_msg(MsgRecv,code,1);
    switch(type)
    {
        case 0:                                //��������
        {
            sprintf(cmd,"container start %s ",dockername);
            ret = getshellresult(cmd,buff,5);
            print_exec_info(cmd,buff, ret);
            if(strcmp(buff[0],CON_START_VALUE)==0)
            {
                MQTTMSG_FMT_DEBUG("���������ɹ�\n",dockername);
            }
            else
            {
                MQTTMSG_FMT_DEBUG("��������ʧ��\n",dockername);
                //result = FALSE;
            }
        }break;
        case 1:                             //ֹͣ����
        {
            sprintf(cmd,"container stop %s ",dockername);
            ret = getshellresult(cmd,buff,5);
            print_exec_info(cmd,buff, ret);
            if(strcmp(buff[0],CON_STOP_VALUE)==0)
            {
                MQTTMSG_FMT_DEBUG("ֹͣ�����ɹ�\n",dockername);
            }
            else
            {
                MQTTMSG_FMT_DEBUG("ֹͣ����ʧ��\n",dockername);
                //result = FALSE;
            }

        }break;
        case 2:                         //ж������ ��ֹͣ��ж�� ֹͣ�Ѿ�ֹͣ�������������
        {
            sprintf(cmd,"container uninstall %s ",dockername);
            ret = getshellresult(cmd,buff,5);
            print_exec_info(cmd,buff, ret);
            if(strcmp(buff[0],CON_UNINDTATALL_VALUE)==0)
            {
                MQTTMSG_FMT_DEBUG("ж�������ɹ�\n",dockername);
            }
            else
            {
                MQTTMSG_FMT_DEBUG("ж������ʧ��\n",dockername);
               // result = FALSE;
            }
        }break;
        default:
            break;
    } 

}


/**********************************************************************
* @name      : docker_exec_0010
* @brief     ��������װ��Ϣִ��
* @param[in] ��MSG_INFO_T *MsgRecv ��Ϣ���� int MSGLen ��С
* @param[out]��
* @return    ��
* @Create    : 
* @Date      ��2020-5-22
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
    if(AppSec==1) //��app
    {
        app.len = data[index++];
        memcpy(app.buff,&data[index],app.len);
        index+=app.len;
    }
    if(data[index++] == 1) //����������
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
    con_info.iot_docker_config_T.strogefilelen = data[index++]; //�洢�ļ���
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
        checklen = tmplen;  // ��¼У��ֵ����
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
        if(index!=msglen)   //������ȷ���ж�
        {
            MQTTMSG_FMT_DEBUG("����У����� index %d msglen %d \n",index,msglen);
            result = FALSE;
        }
		uint8 result_buf[100]={0};
		result_buf[0]=result;
		packge_report_msg(MsgRecv,result_buf,1);
        //���ж��ն����Ƿ�������������������ж��
        sprintf(cmd,"docker ps | grep -w %s",con_info.dockername);  //��׼ƥ��
        ret = getshellresult(cmd,buff,20);
        if(ret==1)
        {
            MQTTMSG_FMT_DEBUG("��鵽��ͬ��������ֹͣ��ж��\n");
            memset(cmd,0,sizeof(cmd));
            memset(buff,0,sizeof(buff));
            sprintf(cmd,"container stop %s;container uninstall %s",con_info.dockername,con_info.dockername);
            ret = getshellresult(cmd,buff,20);
            print_exec_info(cmd,buff,ret);
            if (ret > 0 )
            {
                if(strcmp(buff[0],CON_STOP_VALUE)==0)
                    MQTTMSG_FMT_DEBUG("ֹͣ�ɹ�\n");
                if(strcmp(buff[1],CON_UNINDTATALL_VALUE)==0)
                    MQTTMSG_FMT_DEBUG("ж�سɹ�\n");
            }
        }

        /*ȷ���ļ��Ƿ����*/
        /*�ж�app�ļ��Ƿ����*/

        if(AppInfoSec == 1 )    //app�ļ���Ч��md5ֵ�ж�
        {
            sprintf(pathname,"%s/%s",appinfo.file_info_t.filepath,appinfo.file_info_t.filename);
            if(access(pathname,R_OK)!=0)
            {
                MQTTMSG_FMT_DEBUG("·�������� %s\n",pathname);
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
                MQTTMSG_FMT_DEBUG("У��ֵ��һ��\n");
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
            if(AppSigSec==1)   //ǩ���ļ���Ч��md5ֵ�ж�
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
        /*������*/
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
                MQTTMSG_FMT_DEBUG("������װ�ɹ���APP��װ�ɹ�\n", ret,cmd);
           }
           else
           {
                MQTTMSG_FMT_DEBUG("��APP������װʧ��\n", ret,cmd);
                result = FALSE;
                break;
           }
        }
   
        if(AppInfoSec == 0 && ret>=1)
        {
           if(strcmp(buff[ret-1],CON_INDTALLBLANK_VALUE)==0)
           {
                MQTTMSG_FMT_DEBUG("��������װ�ɹ�\n", ret,cmd); 
           }
           else
           {
                MQTTMSG_FMT_DEBUG("��������װʧ��\n", ret,cmd); 
                result = FALSE;
                break;
           }
        }
        if(ret <= 0)
        {
           MQTTMSG_FMT_DEBUG("������װʧ�� %s %d \n",cmd,ret);  
        }
        /*����Ҫ��֤*/
        /*����APP��ֵ(option)*/
        if(AppSec==1 && AppInfoSec==1)
        {
            //����app��Ϣ��app����Ҫ���ļ�����ȥ��.tar
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
    /*ִ�а�װ����*/
    /*���ò�����������app��*/
    /*���ر��ĺͲ����澯*/
//�ڱ����жϺ�ֱ�Ӿͻظ����ġ�
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
        MQTTMSG_FMT_DEBUG("���յ����еı��� IID %04x IOP %04x \n",MsgRecv->IID,MsgRecv->IOP);
        if(MsgRecv->IID != 0x000B)
        {
            MQTTMSG_FMT_DEBUG("���б���IID�����⣬IID %04x \n",MsgRecv->IID);
        }
        switch (MsgRecv->IOP)
        {
            case 0x0010:
            {
              docker_exec_0010(MsgRecv,RecvLen); 
            }break;
            case 0x0011:            //��������
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
                    //�������
               docker_exec_0018(MsgRecv,RecvLen);  
            }break;
            case 0x0019:
            {
                //��ɲ�ѯ 
                docker_exec_0019(MsgRecv,RecvLen);
            }break;
            case 0x001A:
            {
                //Ŀǰû������ ϵͳ����
                docker_exec_001A(MsgRecv,RecvLen);
            }break;
            case 0x001B:
            {
               docker_exec_001B(MsgRecv,RecvLen); 
            }break;
            case 0x001C:
            {
                // ��־��Ϣ��ѯ  û��
                docker_exec_001C(MsgRecv,RecvLen); 
            }break;
            case 0x001D:
            {
                //��־���     ��־����û��
                docker_exec_001D(MsgRecv,RecvLen); 
            }break;
            case 0x001E:
            {
                //��־ɾ��     ��־����û��
                docker_exec_001E(MsgRecv,RecvLen); 
            }break;
            case 0x001F:
            {
                //ϵͳ���ò���     
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
    MQTTMSG_FMT_DEBUG("msg �߳����� \n");
    return 0;
}


