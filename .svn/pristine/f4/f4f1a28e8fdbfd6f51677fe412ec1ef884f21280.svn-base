/*
*********************************************************************
* Copyright(C) 2020 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   回路监测主线程功能
* @date：    2020-6-30
* @history： 
*********************************************************************
*/
#include "iot_public.h"
#include "sm3sum.h"
#include <errno.h>
int mqtt_topic_write(uint8* SendBuf,uint32 SendLen,char* topic)
{
    return mqtt_client_send_msg(i_MqttClient,topic,0,SendBuf, SendLen);
}

uint16 percentdatachange(char* buf)
{
    char* t;
    char data[100]={0};
    t = strstr(buf,"%");
    if(NULL == t)
        return 0;
    if(t-buf > 10)
        return 0;
    memcpy(data,buf,t-buf);
    return atof(data)*100;
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
    while(sp<=strend && *sp == ',')
        sp++;
    while(sp<=strend && *sp == '"')
        sp++;
    while(ep>=sp && isspace(*ep))
        ep--;
    while(ep>=sp && *ep == ',')
        ep--;
    while(ep>=sp && *ep == '"')
        ep--;
    len = (ep < sp) ? 0:(ep-sp)+1;//(ep < sp)判断是否整行都是空格
    sp[len] = '\0';
    return sp;

}

void ssplit(char *src,const char *delim,char dest[][500],int *num)
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
    tmptime+=8*3600; //转为东八区
    DT_Time2DateTimeShort(tmptime,ooptime);
    //printf("time %d %d %d %d %d %d \n",ooptime->year,ooptime->month,ooptime->day,ooptime->hour,ooptime->minute,ooptime->second);
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


int getshellresult(const char* cmd,char result[][500],int size)
{
    //UPDATE_FMT_DEBUG(cmd,strlen(cmd),"\n");
    char temp[1024];
    FILE* fp = NULL;
    int row = 0;
    fp = popen(cmd, "r");
    if(fp == NULL)
    {
        return -1;
    }
  /*  int fd = fileno(fp);
    int flags = fcntl(fd,F_GETFL,0);
    flags |=O_NONBLOCK;
    fcntl(fd, F_SETFL, flags); 
    fp = fdopen(fd, "r");*/
    MQTTMSG_FMT_DEBUG("执行命令 ：%s\n", cmd);
    
    while(fgets(temp, sizeof(temp), fp) != NULL)
    {
        MQTTMSG_FMT_DEBUG("返回结果 ：%s\n", temp);
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
* @name      : sizeAndmd5_judge
* @brief     ：判断文件的大小和md5值是否正确
* @param[in] filepath  文件绝对路径 size 文件大小 md5 md5值
* @param[out]：
* @return    ：
* @Create    : 
* @Date      ：2020-5-22
* @Update    :
**********************************************************************/
uint8 sizeAndmd5_judge(char *filepath,uint32 size,char* md5)
{
    if(size == 0 || md5 ==NULL)
        return 0;
    if(filepath == NULL)
        return 1;
    char cmd[500]={0};
    char buff[2][500];
    uint8 bit_str[100]= {0};
    int bitlen = 0;
    memset(buff,0,sizeof(buff));
    sprintf(cmd,"wc -c %s | awk '{print $1}'",filepath);
    if(getshellresult(cmd,buff,1)!=1)
    {
        MQTTMSG_FMT_DEBUG("命令执行失败 ：%s \n",cmd);
        return 1; 
    }
          
    if(atoi(buff[0])!=size)
    {
      MQTTMSG_FMT_DEBUG("[%s]大小不一致 wc %d size %d\n",filepath, atoi(buff[0]),size);
      return 1;  
    }
    memset(buff,0,sizeof(buff));
    memset(cmd,0,sizeof(cmd));
    sprintf(cmd,"md5sum %s | awk '{print $1}'",filepath);
    if(getshellresult(cmd,buff,1)!=1)
        return 1;
    MQTTMSG_BUF_DEBUG(buff[0],strlen(buff[0]),"md5sum value");
    MQTTMSG_BUF_DEBUG(md5,strlen(md5),"md5 value");
 //   bitlen = strt2octet(buff[0],strlen(buff[0]),bit_str,64);
 //   if((bitlen<0) || (bitlen >64))
 //   {
 //       MQTTMSG_FMT_DEBUG("转化失败 bufflen %d \n",bitlen);
  //      return 1;
 //   }
  //  MQTTMSG_BUF_DEBUG(bit_str,bitlen,"bit_str",);
  //  MQTTMSG_BUF_DEBUG(md5,bitlen,"md5 ");
    if(memcmp(bit_str,md5,bitlen)!=0)
        return 1;
    return 0;
}

/**********************************************************************
* @name      : sizeAndmd5_judge
* @brief     ：判断文件的大小和md5值是否正确
* @param[in] filepath  文件绝对路径 size 文件大小 md5 md5值
* @param[out]：
* @return    ：
* @Create    : 
* @Date      ：2020-5-22
* @Update    :
**********************************************************************/
uint8 sizeAndSM3_judge(char *filepath,uint32 size,char* sm3)
{
    if(size == 0 || sm3 ==NULL)
        return 0;
    if(filepath == NULL)
        return 1;
    char cmd[500]={0};
    char buff[2][500];
    uint8 calsm3[100]= {0};
    memset(buff,0,sizeof(buff));
    sprintf(cmd,"wc -c %s | awk '{print $1}'",filepath);
    if(getshellresult(cmd,buff,1)!=1)
    {
        MQTTMSG_FMT_DEBUG("命令执行失败 ：%s \n",cmd);
        return 1; 
    }
          
    if(atoi(buff[0])!=size)
    {
      MQTTMSG_FMT_DEBUG("[%s]大小不一致 wc %d size %d\n",filepath, atoi(buff[0]), size);
      return 1;  
    }
    memset(buff,0,sizeof(buff));
    int ret = GetFileSM3(filepath, 100, calsm3);
    if(ret != 0)
    {
        MQTTMSG_FMT_DEBUG("计算sm3失败，ret = %d\n",ret);
        return 1;
    }

    MQTTMSG_BUF_DEBUG(calsm3,32,"calsm3sum value");
    MQTTMSG_BUF_DEBUG(sm3,strlen(sm3),"sm3 value");

    if(memcmp(calsm3,sm3,32)!=0)
        return 1;
    return 0;
}


/**********************************************************************
* @name      : appmsg_axdr_package
* @brief     ：指定格式消息组装成
* @param[in] ：MESSAGE_INFO_T *pMsgInfo  结构消息
* @param[out]：uint8 **ppBuf             组装消息
* @return    ：消息长度
* @Create    : 
* @Date      ：2021-7-7
* @Update    :
**********************************************************************/
uint16 iot_appmsg_package(MSG_INFO_T *pMsgInfo, uint8 **ppBuf)
{
    uint16   bufLen = 0;
    uint8 lenBuf[3] = {0};
    uint8    offset = 0;
    uint8  *pMsgBuf = NULL;
    uint8    *pTemp = NULL;
    
    bufLen = 1 + 2 + 2 + pMsgInfo->SourLen + 1 + pMsgInfo->DestLen + 1 + 4;   //计算head和Tag长度
    /* 计算MSG's-Length 长度  */
    offset = appmsg_set_len_offset(pMsgInfo->MsgLen, lenBuf);
    if(0 == offset)
    {
   //     DPRINT("appmsg_udp_package offset wrong\n");
        return 0;
    }

    bufLen += offset + pMsgInfo->MsgLen;

    pMsgBuf = (uint8 *)malloc(bufLen);
    if(NULL == pMsgBuf)
    {
//        DPRINT("appmsg_udp_package malloc failed,size %p\n", pMsgBuf);

        return 0;
    }

    memset(pMsgBuf, 0, bufLen);

    pTemp = pMsgBuf;

    /* PRM PRIORITY */
    *pTemp = ((pMsgInfo->MsgPRIORITY << 1) & 0xfe) + (pMsgInfo->MsgRPM & 0x01);
    pTemp++;

    /* INDEX */
#if IS_LITTLE_ENDIAN
    *((uint16 *)pTemp) = pMsgInfo->MsgIndex;
#else
    *((uint16 *)pTemp) = htons(pMsgInfo->MsgIndex);
#endif
    pTemp += sizeof(uint16);

    /* LABEL */
#if IS_LITTLE_ENDIAN
    *((uint16 *)pTemp) = pMsgInfo->MsgLabel;
#else
    *((uint16 *)pTemp) = htons(pMsgInfo->MsgLabel);
#endif
    pTemp += sizeof(uint16);

    /* SOURCE */
    memcpy(pTemp, pMsgInfo->SourAddr, pMsgInfo->SourLen);
    pTemp += pMsgInfo->SourLen + 1;

    /* DESTINATION */
    memcpy(pTemp, pMsgInfo->DestAddr, pMsgInfo->DestLen);
    pTemp += pMsgInfo->DestLen + 1;

    /* MSG's-TAG */
#if IS_LITTLE_ENDIAN
    *((uint32 *)pTemp) =((((uint32)pMsgInfo->IID) << 16) & 0xffff0000) + (((uint32)pMsgInfo->IOP) & 0x0000ffff);
#else
    *((uint32 *)pTemp) = htonl(((((uint32)pMsgInfo->IID) << 16) & 0xffff0000) + (((uint32)pMsgInfo->IOP) & 0x0000ffff));
#endif

    pTemp += sizeof(uint32);

    /* MSG's-Length */
    memcpy(pTemp, lenBuf, offset);
    pTemp += offset;

    if(pMsgInfo->MsgLen)
    {
        memcpy(pTemp, pMsgInfo->MsgData, pMsgInfo->MsgLen);  //MSG's-Payload
    }

    *ppBuf = pMsgBuf;

    return bufLen;
}


/**********************************************************************
* @name      : appmsg_axdr_package
* @brief     ：指定格式消息组装成
* @param[in] ：MESSAGE_INFO_T *pMsgInfo  结构消息
* @param[out]：uint8 **ppBuf             组装消息
* @return    ：消息长度
* @Create    : 
* @Date      ：2021-7-7
* @Update    :
**********************************************************************/
uint16 wuguan_appmsg_package(MSG_INFO_T *pMsgInfo, uint8 **ppBuf)
{
    uint16   bufLen = 0;
    uint8 lenBuf[3] = {0};
    uint8    offset = 0;
    uint8  *pMsgBuf = NULL;
    uint8    *pTemp = NULL;
    
    bufLen = 1 + 2 + 2 + pMsgInfo->SourLen + 1 + pMsgInfo->DestLen + 1 + 4;   //计算head和Tag长度
    /* 计算MSG's-Length 长度  */
    offset = appmsg_set_len_offset(pMsgInfo->MsgLen, lenBuf);
    if(0 == offset)
    {
   //     DPRINT("appmsg_udp_package offset wrong\n");
        return 0;
    }

    bufLen += offset + pMsgInfo->MsgLen;

    pMsgBuf = (uint8 *)malloc(bufLen);
    if(NULL == pMsgBuf)
    {
//        DPRINT("appmsg_udp_package malloc failed,size %p\n", pMsgBuf);

        return 0;
    }

    memset(pMsgBuf, 0, bufLen);

    pTemp = pMsgBuf;

    /* PRM PRIORITY */
    *pTemp = ((pMsgInfo->MsgPRIORITY << 1) & 0xfe) + (pMsgInfo->MsgRPM & 0x01);
    pTemp++;

    /* INDEX */
#if IS_LITTLE_ENDIAN
    *((uint16 *)pTemp) = pMsgInfo->MsgIndex;
#else
    *((uint16 *)pTemp) = htons(pMsgInfo->MsgIndex);
#endif
    pTemp += sizeof(uint16);

    /* LABEL */
#if IS_LITTLE_ENDIAN
    *((uint16 *)pTemp) = pMsgInfo->MsgLabel;
#else
    *((uint16 *)pTemp) = htons(pMsgInfo->MsgLabel);
#endif
    pTemp += sizeof(uint16);

    /* SOURCE */
    memcpy(pTemp, pMsgInfo->SourAddr, pMsgInfo->SourLen);
    pTemp += pMsgInfo->SourLen + 1;

    /* DESTINATION */
    memcpy(pTemp, pMsgInfo->DestAddr, pMsgInfo->DestLen);
    pTemp += pMsgInfo->DestLen + 1;

    /* MSG's-TAG */
#if IS_LITTLE_ENDIAN
    *((uint32 *)pTemp) =((((uint32)pMsgInfo->IID) << 16) & 0xffff0000) + (((uint32)pMsgInfo->IOP) & 0x0000ffff);
#else
    *((uint32 *)pTemp) = htonl(((((uint32)pMsgInfo->IID) << 16) & 0xffff0000) + (((uint32)pMsgInfo->IOP) & 0x0000ffff));
#endif

    pTemp += sizeof(uint32);

    /* MSG's-Length */
    memcpy(pTemp, lenBuf, offset);
    pTemp += offset;

    if(pMsgInfo->MsgLen)
    {
        memcpy(pTemp, pMsgInfo->MsgData, pMsgInfo->MsgLen);  //MSG's-Payload
    }

    *ppBuf = pMsgBuf;

    return bufLen;
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


int32 taskmng_plc_queue_write(LockQueue_T *pLockQueue, uint8 *buf, int32 buf_len)
{
    int32 flags = 0;
        //加上防错 防止越界
    if(buf_len >MAX_QUEUE_SIZE )
        buf_len = MAX_QUEUE_SIZE;    
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


/**********************************************************************
* @name      : msg_mqtt_package
* @brief     ：指定格式消息组装成
* @param[in] ：MSG_INFO_T *pMsgInfo  结构消息
               bufLenMax             buf最大长度
* @param[out]：uint8 *buf            组装消息
* @return    ：消息长度
* @Create    : 
* @Date      ：2019-10-29
* @Update    :
**********************************************************************/
uint16 msg_mqtt_package(MSG_INFO_T *pMsgInfo, uint32 bufLenMax, uint8 *buf)
{
    uint16   bufLen = 0;
    uint8 lenBuf[3] = {0};
    uint8    offset = 0;
    uint8  *pMsgBuf = buf;
    uint8    *pTemp = NULL;

    if ((pMsgInfo == NULL) || (buf == NULL))
    {
     //   MPRINT("msg_mqtt_package param wrong\n");
        return 0;
    }
    
    bufLen = 1 + 2 + 2 + strlen(pMsgInfo->SourAddr) + 1 + strlen(pMsgInfo->DestAddr) + 1 + 4;   //计算head和Tag长度

    /* 计算MSG's-Length 长度  */
    offset = set_len_offset(pMsgInfo->MsgLen, lenBuf);
    if(0 == offset)
    {
    //    MPRINT("msg_mqtt_package offset wrong\n");
        return 0;
    }

    bufLen += offset + pMsgInfo->MsgLen;

    if (bufLen > bufLenMax)
    {
      ///  MPRINT("msg_mqtt_package bufLenMax wrong\n");
        return 0;
    }

    memset(pMsgBuf, 0, bufLen);

    pTemp = pMsgBuf;

    /* PRM PRIORITY */
    *pTemp = ((pMsgInfo->MsgPRIORITY << 1) & 0xfe) + (pMsgInfo->MsgRPM & 0x01);
    pTemp++;

    /* INDEX */
#if IS_LITTLE_ENDIAN
    *((uint16 *)pTemp) = pMsgInfo->MsgIndex;
#else
    *((uint16 *)pTemp) = htons(pMsgInfo->MsgIndex);
#endif
    pTemp += sizeof(uint16);

    /* LABEL */
#if IS_LITTLE_ENDIAN
    *((uint16 *)pTemp) = pMsgInfo->MsgLabel;
#else
    *((uint16 *)pTemp) = htons(pMsgInfo->MsgLabel);
#endif
    pTemp += sizeof(uint16);

    /* SOURCE */
    memcpy(pTemp, pMsgInfo->SourAddr, strlen(pMsgInfo->SourAddr));
    pTemp += strlen(pMsgInfo->SourAddr) + 1;

    /* DESTINATION */
    memcpy(pTemp, pMsgInfo->DestAddr, strlen(pMsgInfo->DestAddr));
    pTemp += strlen(pMsgInfo->DestAddr) + 1;

    /* MSG's-TAG */
#if IS_LITTLE_ENDIAN
    *((uint32 *)pTemp) =((((uint32)pMsgInfo->IID) << 16) & 0xffff0000) + (((uint32)pMsgInfo->IOP) & 0x0000ffff);
#else
    *((uint32 *)pTemp) = htonl(((((uint32)pMsgInfo->IID) << 16) & 0xffff0000) + (((uint32)pMsgInfo->IOP) & 0x0000ffff));
#endif
    pTemp += sizeof(uint32);

    /* MSG's-Length */
    memcpy(pTemp, lenBuf, offset);
    pTemp += offset;

    memcpy(pTemp, pMsgInfo->MsgData, pMsgInfo->MsgLen);  //MSG's-Payload

    return bufLen;
}


/**********************************************************************
* @name      : get_len_offset
* @brief     ：获取串类型数据元素长度
* @param[in] ：const uint8 *indata      数据Buffer
* @param[out]：uint8 *bytes             便宜量
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


/**********************************************************************
* @name      : wireless_get_appid
* @brief     ：获取APPID
* @param[in] ：uint8 len     缓存长度
* @param[out]：uint8 *appid  APPID
* @return    ：长度
* @Create    : 王津剑
* @Date      ：2021-3-22
* @Update    : 
**********************************************************************/
uint8 iot_get_appid(uint8 *appid, uint8 len)
{
    int ret = 0;
    DEVINFO_DEVICE_T* dev = (DEVINFO_DEVICE_T*) hal_device_get(HW_DEVICE_ID_DEVINFO);
    if(NULL == dev)
    {
        memset(appid, 0x00, len);
        INIT_FMT_DEBUG("devinfo hal_device_get error \n");
        return MIN(24, len);
    }
    else
    {
        ret = dev->devinfo_read_appid(dev, "wuguan", (char*)appid, len);
        if(ERR_OK != ret)
        {
            INIT_FMT_DEBUG("devinfo devinfo_read_appid error ret %d\n", ret);
            memset(appid, 0x00, len);
        }
        else
        {
            INIT_BUF_DEBUG(appid, MIN(24, len), "APPID\n");
        }
        hal_device_release((HW_DEVICE *)dev);
        return MIN(24, len);
    }
}



