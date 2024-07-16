/*-***************************************************************
* Copyright (c) 2006,南京新联电子仪器有限公司
*            All rights reserved.
* 程序名称：rs232.c
* 版本号：V1.0
* 功能：232动态库
* 开发人:尤斌
* 开发时间：2006年12月22日
* 修改人员、时间及修改内容:
* 其他： 
******************************************************************-*/
#include "../include/rs232.h"

//#define MAX_FRAME_OVERTIME  3       //帧超时时间 定义一个默认的超时时间 3秒
//#define MAX_CHAR_OVERTIME  500       //字符超时时间 10ms

struct timespec gLastSendTime[AMR_THREAD_NUM];   //485一发一收 记录上次发送时间
uint16 gLastSendSeq[AMR_THREAD_NUM];    //记录上次发送的消息序号
uint8  gOverTime[AMR_THREAD_NUM];       //记录下超时时间
uint16  gByteOverTime[AMR_THREAD_NUM];       //字节超时时间
extern uint8 patch_policy_flg[AMR_THREAD_NUM];


#define UART_APP_TOPIC_NAME              "M-uartManager"

//#ifdef PRODUCT_ECU
#if 0
#include "uart_api.h"
UART_DEVICE_T *gUartDev[MAX_485_NUM-2] = {NULL}; //485的hal设备
CAN_DEVICE_T *gCanDev[2]={NULL};        //can口
uint32 gSpeed[MAX_485_NUM]={0};
/*******************************************************************************
* 函数名称: taskmng_uart_devname_get
* 函数功能: 根据端口号获取串口设备名称
* 输入参数: 
* 输出参数: 无
* 返 回 值: -1 表示没有设备 0正常
*******************************************************************************/
int taskmng_uart_devname_get(uint8 pipe,char* devname)
{
    uint8 i,j;
    int moduleIndex = -1;
    if(pipe < PLC_NUM || pipe >= PLC_NUM+MAX_485_NUM)
        return -1;
    //前两路是本体 
    if(pipe == PLC_NUM)
    {
        strcpy(devname,UART_RS4851_DEVICE_ID_STR);
        return 0;
    }else if(pipe == PLC_NUM+1)
    {
        strcpy(devname,UART_RS4852_DEVICE_ID_STR);
        return 0;
    }else if(pipe >= PLC_NUM+2 && pipe < PLC_NUM+6)
    {
        //需要检查模组信息
        pthread_mutex_lock(&gModuleInfoLock);
        for(i=0;i<gModuleInfo.nNum;i++)
        {
            for(j=0;j<gModuleInfo.item[i].nChNum;j++)
            {
                if(gModuleInfo.item[i].chInfo[j].nFunc == 3)
                {
                    moduleIndex = gModuleInfo.item[i].nSlot;
                    break;
                }
            }
            if(moduleIndex!=-1)
                break;
        }
        pthread_mutex_unlock(&gModuleInfoLock);
        if(moduleIndex ==-1)
        {
            return -1;
        }else
        {
            sprintf(devname,"uart_acm%d_%d",moduleIndex,pipe-PLC_NUM-1);
            return 0;
        }
    }else
    {
        //can口
        sprintf(devname,"can%d",pipe-PLC_NUM-6);
        return 00;
    }
}

/*******************************************************************************
* 函数名称: taskmng_uart_init
* 函数功能: 串口初始化
* 输入参数: 
* 输出参数: 无
* 返 回 值: 
*******************************************************************************/
int taskmng_uart_init(uint8 pipe,uint8 speed,PARITY_TYPE_E parity)
{
    int ret;
    char devname[100]={0};
    uint32 baud; //将波特率的enum转换为真实值
    UART_BAUD_TO_STRING(baud,speed);
    gSpeed[pipe-PLC_NUM] = baud;
    ret = taskmng_uart_devname_get(pipe,devname);
    if(ret <0)
    {
        TASK_FMT_DEBUG( pipe, REC_LOG, "获取设备名称失败\n");
        return -1;
    }
    if(pipe < PLC_NUM+MAX_485_NUM -2)
    {
        //485
        UART_DEVICE_T* dev;
        if(NULL == gUartDev[pipe-PLC_NUM])
        {
            gUartDev[pipe-PLC_NUM] = (UART_DEVICE_T*)hal_device_get(devname);
        }
        if(NULL == gUartDev[pipe-PLC_NUM])
        {
            TASK_FMT_DEBUG( pipe, REC_LOG, "485 hal_device_get: %s 失败  \n",devname);
            return -1;
        }
        dev = gUartDev[pipe-PLC_NUM];
        ret = dev->uart_param_set(dev,baud,8,1,parity);
        if(ret == 0)
        {
            TASK_FMT_DEBUG( pipe, REC_LOG, "串口初始化成功 波特率 %d  \n",baud);
        }else
        {
            TASK_FMT_DEBUG( pipe, REC_LOG, "串口初始化失败 波特率 %d  \n",baud);
        }
        return ret;
    }else
    {
        //can
        CAN_DEVICE_T* candev ;
        if(NULL == gCanDev[pipe+2 - PLC_NUM - MAX_485_NUM])
        {
            gCanDev[pipe+2 - PLC_NUM - MAX_485_NUM] = (CAN_DEVICE_T*)hal_device_get(devname);
        }
        if(NULL == gCanDev[pipe+2 - PLC_NUM - MAX_485_NUM])
        {
            TASK_FMT_DEBUG( pipe, REC_LOG, "can口 hal_device_get: %s 失败  \n",devname);
            return -1;
        }
        candev = gCanDev[pipe+2 - PLC_NUM - MAX_485_NUM];
        ret = candev->can_param_set(candev,baud);
        if(ret == 0)
        {
            TASK_FMT_DEBUG( pipe, REC_LOG, "can口初始化成功 波特率 %d  \n",baud);
        }else
        {
            TASK_FMT_DEBUG( pipe, REC_LOG, "can口初始化失败 波特率 %d  \n",baud);
        }
        return ret;
    }

}

void taskmng_uart_release(uint8 pipe)
{
    if(pipe < PLC_NUM+MAX_485_NUM -2)
    {
        if(NULL == gUartDev[pipe-PLC_NUM])
            return;
        hal_device_release((HW_DEVICE *)gUartDev[pipe-PLC_NUM]);
        gUartDev[pipe-PLC_NUM] = NULL;
    }else
    {
        if(NULL == gCanDev[pipe+2 - PLC_NUM - MAX_485_NUM])
            return;
        hal_device_release((HW_DEVICE *)gCanDev[pipe+2 - PLC_NUM - MAX_485_NUM]);
        gCanDev[pipe+2 - PLC_NUM - MAX_485_NUM] = NULL;
    }


}

int taskmng_uart_recv(uint8 pipe, uint8* pBuffer, int nBufLen)
{
    int rsptime = 0;
    int rcvtime = 10;
    int speed;
    int fd = 0;
    int recv_len = 0;                                       
    int data_len = 0;                                       
    int count = 0;
    int delay = 0;
    fd_set set;
    struct timeval timeout;
    int n = 0;
    uint8 overtime = MAX_FRAME_OVERTIME;     //
    UART_DEVICE_T* dev=NULL;
    CAN_DEVICE_T* candev =NULL;
    if(pipe < PLC_NUM+MAX_485_NUM -2)
    {
        dev = gUartDev[pipe-PLC_NUM];
        if(dev == NULL)
        {
            return 0;
        }
        fd = dev->uart_get_handle(dev);
    }else
    {
        candev = gCanDev[pipe+2 - PLC_NUM - MAX_485_NUM];
        if(candev == NULL)
        {
            return 0;
        }
        fd = candev->can_get_handle(candev);
    }

    if (0 >= fd)  
    {  
        return 0;  
    }
    
    /*串口接收数据超时时间按20个字节间隔时间来计算，300bps 发送一个字节需要36667us
    一个字节包括起始位、停止位、校验位、8位数据 共11位*/
    
    speed = gSpeed[pipe-PLC_NUM];
    //先计算出每位需要的时间 再乘以11就等于一个字节需要的时间 1000000/speed*11 单位us
    delay = 11000000/speed;
    //
    if(overtime>5)
        overtime = 5;
    rsptime = (overtime*1000)/30;
    while (1)
    {
        FD_ZERO(&set);
        FD_SET(fd, &set);
        timeout.tv_sec = 0;
        timeout.tv_usec = 30000;
        
        do {
            n = select(fd+1, &set, NULL, NULL, &timeout);
        }while(n<0);

        if (n > 0)  
        {
            if(pipe < PLC_NUM+MAX_485_NUM -2)
            {
                recv_len = dev->uart_data_recv(dev,pBuffer+data_len, nBufLen-data_len);    /*调用read()，进行串口数据接收*/
            }else
            {
                recv_len = candev->can_data_recv(candev,pBuffer+data_len, nBufLen-data_len);    /*调用read()，进行串口数据接收*/
            }
        }
        else
        {
            recv_len = 0;
        }

        // 收到字节后延时一个字节的时间再去接收
        if (recv_len > 0)   
        {
            data_len += recv_len;
            count = 0;
            usleep(delay);
        }
        else
        {
            /*-接收数据超时-*/
            if ((data_len == 0) && (count > rsptime)) 
            {
                usleep(delay);
                return data_len;
            }
            
            if ((data_len != 0) && ((count > rcvtime) || (data_len > nBufLen))) 
            {
                usleep(delay);
                return data_len;                    
            }
        }

        count++;
    }    
    return data_len;     

}



#endif


/*******************************************************************************
* 函数名称: taskmng_msg_send_to_uartManager
* 函数功能: 组消息
* 输入参数: 
* 输出参数: 无
* 返 回 值: 数据帧长度
*******************************************************************************/
uint16 taskmng_msg_send_to_uartManager(uint8 pipe,uint8 speed,PARITY_TYPE_E parity,uint16 datalen,uint8 *databuf,uint16* MsgIndex)
{
    uint8 payload[2048]={0};
    uint16 index = 0;
    uint8 lenoffset;

    OOP_OAD_U uartport =  taskmng_pipe_oad_get(pipe);
    //uint32 uartport = pipe -1;
    uint16 overtime;
    uint16 maxrecv = 1024;
    memcpy_r(&payload[index],&uartport.value,4);
    index+=4;
    payload[index++] = 1; //启动帧
    payload[index++] = 0;   //优先级
    overtime = gOverTime[pipe]*1000;
    memcpy_r(&payload[index],&overtime,2); //帧超时时间
    index+=2;
    overtime = gByteOverTime[pipe];
    memcpy_r(&payload[index],&overtime,2);  //字符超时时间
    index+=2;
    overtime = gOverTime[pipe]*1000;
    memcpy_r(&payload[index],&overtime,2); //最大超时时间
    index+=2;
    //通信参数 optional
    payload[index++] = 1;
    payload[index++] = speed;
    //payload[index++] = 11; //00001011 8数据位 偶校验 1停止位 无流控
    payload[index++] = 3 + (parity<<2);
    //预计最大接收字节数
    memcpy_r(&payload[index],&maxrecv,2);
    index+=2;
    lenoffset = set_len_offset(datalen, &payload[index]);
    index+=lenoffset;
    memcpy(&payload[index],databuf,datalen);
    index+=datalen;

    MESSAGE_INFO_T *MsgSend = (MESSAGE_INFO_T*)malloc(sizeof(MESSAGE_INFO_T)+index);
    memset(MsgSend,0x00,sizeof(MESSAGE_INFO_T)+index);

    MsgSend->rpm = 1;
    MsgSend->priority = 1;
    MsgSend->index = GetMsgSeq();
    *MsgIndex = MsgSend->index;
    MsgSend->label = pipe;
    MsgSend->sourlen = strlen(DownName);
    memcpy(MsgSend->souraddr,DownName,MsgSend->sourlen);
    MsgSend->destlen = strlen(UART_APP_TOPIC_NAME);
    memcpy(MsgSend->destaddr,UART_APP_TOPIC_NAME,MsgSend->destlen );

    MsgSend->IID = 0x0009;
    MsgSend->IOP = 0x0013;
    MsgSend->msglen = index;
    memcpy(MsgSend->playload,payload,index);
    TASK_FMT_TRACE( pipe, COMM_LOG, "Send to uartManagerMsg Index %d \n",MsgSend->index);

    TASK_BUF_TRACE( pipe, COMM_LOG, payload, index);
    taskmng_plc_queue_write ( pAmrSendDown[pipe], (uint8*)MsgSend, sizeof(MESSAGE_INFO_T)+index );
    free(MsgSend);
    return sizeof(MESSAGE_INFO_T)+index;
    
}


/**********************************************************************
* @name      : rs485_RS232_Init
* @brief     ：初始化串口接口
* @param[in] ：RS485_RS232_PARA *pCommPara  本地通讯参数
* @param[out]：
* @return    ：0-成功/失败
* @Create    : 高启翔
* @Date      ：2020-3-10
* @Update    :
**********************************************************************/
//int rs485_Dlms_Init(RS485_RS232_PARA *pCommPara,uint16 speed)
//{
//    UART_DEVICE_T* dev;
//    int fd=-1;
//    int ret=0;
//    //printf("初始化%s,speed is %d\n",pCommPara->deviceName,speed);
//    //TASK_FMT_DEBUG( -1, COMM_LOG, "第一次获取设备初始化 pCommPara->dev11 is %p\n",pCommPara->dev);
//    if(pCommPara->dev == NULL)
//    {
//        pCommPara->dev = (UART_DEVICE_T*)hal_device_get(pCommPara->deviceName);
//        TASK_FMT_DEBUG( -1, COMM_LOG, "第一次获取设备初始化 pCommPara->dev is %p\n",pCommPara->dev);
//    }
//    if(NULL == pCommPara->dev)
//    {
//        return -1;    
//    }
//    dev = pCommPara->dev;
//    // 打开设备
//    if(speed==0)
//    {
//        speed=2400;
//    }
//    pCommPara->Buad= speed;
//    ret=dev->uart_param_set(dev, speed, 8, 1, PARITY_NO);
//    pCommPara->pDev = dev; 
//    fd =  dev->uart_get_handle(dev);
//    TASK_FMT_DEBUG( -1, COMM_LOG, "初始化 dev is %p,speed is %d\n",dev,speed);
//    TASK_FMT_DEBUG( -1, COMM_LOG, "初始化 fd is %d,ret is %d\n",fd,ret);
//    return ret;
//}

uint16 Getbyteovertime(uint8 speed)
{
    uint32 baud=0;
    uint32 delay=0;

    if(speed >10)
    {
        return MAX_CHAR_OVERTIME;
    }
    switch(speed)
    {
        case 0:
            baud = 300;
            break;

        case 1:
            baud = 600;
            break;

        case 2:
            baud = 1200;
            break;

        case 3:
            baud = 2400;
            break;

        case 4:
            baud = 4800;
            break;

        case 5:
            baud = 7200;
            break;

        case 6:
            baud = 9600;
            break;

        case 7:
            baud = 19200;
            break;

        case 8:
            baud = 38400;
            break;

        case 9:
            baud = 57600;
            break;

        case 10:
            baud = 115200;
            break;

        default:
            break;
    }
    /*串口接收数据超时时间按20个字节间隔时间来计算，300bps 发送一个字节需要36667us
    一个字节包括起始位、停止位、校验位、8位数据 共11位*/
    if ((baud%300 != 0 ) || baud == 0)             // 若波特率不对，默认9600
    {   
        delay = 11000000/9600;
    }
    else
    {   
        delay = 11000000/baud;
    }

    delay=delay/100;
    //115200的时候直接10毫秒
    if(baud == 115200)
        return 10;
    if(delay==0)
    {
        return 100;
    }
    //台体测试发现2400波特率的时候长报文会收不全
    if(speed < 6 )
        return 500;
    //串口限制 一次收32字节 所以字节延时放到40个字节的时间
#if defined(PRODUCT_ZCU_3) || defined(PRODUCT_ZCU_1) || defined(PRODUCT_ECU)
    return delay*4;
#else
    return delay+100;
#endif
}

/**********************************************************************
* @name      : rs485_RS232_send
* @brief     ：串口数据发送接口
* @param[in] ：RS485_RS232_PARA *pCommPara  本地通讯参数
               uint8* pBuffer               发送数据缓冲区
               int nBufLen                  发送长度
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 高启翔
* @Date      ：2020-3-10
* @Update    :
**********************************************************************/
int rs485_RS232_send(uint8 pipe,uint8 overtime,uint16 byteovertime, uint8 speed,PARITY_TYPE_E parity, uint8* pBuffer, int nBufLen)
{

//#ifdef PRODUCT_ECU
  byteovertime = Getbyteovertime(speed);
#if 0
    int ret;
    if(pipe < PLC_NUM+MAX_485_NUM -2)
    {
        UART_DEVICE_T* dev = NULL;
        if(NULL == gUartDev[pipe-PLC_NUM])
        {
            ret = taskmng_uart_init(pipe, speed,parity);
            if(ret<0)
            {
                TASK_FMT_DEBUG( pipe, REC_LOG, "串口初始化失败 无法发送\n");
                return -1;
            }
        }
        dev = gUartDev[pipe-PLC_NUM];
        ret = dev->uart_data_send(dev,pBuffer,nBufLen);
        if(ret!=0)
        {
            TASK_FMT_DEBUG( pipe, REC_LOG, "串口发送失败\n");
        }
        return ret;
    }else
    {
        CAN_DEVICE_T* candev =NULL;
        if(NULL == gCanDev[pipe+2 - PLC_NUM - MAX_485_NUM])
        {
            ret = taskmng_uart_init(pipe, speed,parity);
            if(ret<0)
            {
                TASK_FMT_DEBUG( pipe, REC_LOG, "can口初始化失败 无法发送\n");
                return -1;
            }
        }
        candev = gCanDev[pipe+2 - PLC_NUM - MAX_485_NUM];
        ret = candev->can_data_send(candev,pBuffer,nBufLen);
        if(ret!=0)
        {
            TASK_FMT_DEBUG( pipe, REC_LOG, "can口发送失败\n");
        }
        return ret;
    }

#else
    if(patch_policy_flg[pipe] == 1)
    {
        gOverTime[pipe] = 3;
    }
    else
    {
        //帧超时时间
        if(overtime==0)
            gOverTime[pipe] = MAX_FRAME_OVERTIME;
        else 
            gOverTime[pipe] = overtime;
    }
    
    //字符超时时间
    if(byteovertime==0)
        gByteOverTime[pipe] = MAX_CHAR_OVERTIME;
    else 
        gByteOverTime[pipe] = byteovertime;
    taskmng_down_recv_buff_clear(pipe);
    taskmng_msg_send_to_uartManager(pipe,speed,parity,nBufLen,pBuffer,&gLastSendSeq[pipe]);

    clock_gettime(CLOCK_MONOTONIC, &gLastSendTime[pipe]);
#endif
    return 0;

}

/**********************************************************************
* @name      : rs485_RS232_recv
* @brief     ：串口数据接收接口
* @param[in]   RS485_RS232_PARA *pCommPara  本地通讯参数
               int nBufLen                  最大接收长度
               int rsptime                  数据返回时间(20ms)
               int rcvtime                  数据字节之间间隔时间(20ms)
               uint8 endbyte                结束符，没有填0xff
* @param[out]：uint8* pBuffer               接收数据缓冲区
* @return    ：接收到数据的长度
* @Create    : 高启翔
* @Date      ：2020-3-10
* @Update    :
**********************************************************************/
int rs485_RS232_recv(uint8 pipe, uint8* pBuffer, int nBufLen)
{
//#ifdef PRODUCT_ECU
#if 0

    return taskmng_uart_recv( pipe, pBuffer,nBufLen);
#else
    int datalen;
     uint8 recvbuf[3000]={0};
     struct timespec timenow;
     MESSAGE_INFO_T *msg = NULL;
     uint16 index = 0;
     uint8 lenoffset;
     uint8 state;
     uint16          maxlen = MAX_QUEUE_SIZE - sizeof(MESSAGE_INFO_T);
     while(1)
     {
         datalen = taskmng_plc_queue_read(pAmrRecvDown[pipe], recvbuf);
         if(datalen>0)
         {
             msg = (MESSAGE_INFO_T*)recvbuf;
             TASK_FMT_DEBUG( pipe, COMM_LOG, "收到消息 index %d lastsend %d  \n",msg->index,gLastSendSeq[pipe]);
             //if( msg->IID == 0x0009 && msg->IOP ==0x0002)
             if(msg->index==gLastSendSeq[pipe] && msg->IID == 0x0009 && msg->IOP ==0x0002)
             {
                 //TASK_BUF_DEBUG( pipe, COMM_LOG,msg->playload,msg->msglen);
                 index+=4;//端口号
                 if(msg->playload[index++] ==1)
                 {
                     continue;
                 }
                 state = msg->playload[index++];
                 if(state!=0)
                 {
                     TASK_FMT_DEBUG( pipe, COMM_LOG, "recv state %d error\n",state);
                     return 0;
                 }
                 datalen = get_len_offset(&msg->playload[index],&lenoffset);
                 index+=lenoffset;
                //加上防错 用模拟表时经常能收到几千个字节的报文
                if(datalen > nBufLen || datalen > maxlen)
                {
                    if(nBufLen < maxlen)
                        datalen = nBufLen;
                    else
                        datalen = maxlen;
                }
                 memcpy(pBuffer, &msg->playload[index], datalen);
                 return datalen;

             }
    
         }
         clock_gettime(CLOCK_MONOTONIC, &timenow);
         //比串口根据的超时时间再多一秒
         if(timenow.tv_sec -gLastSendTime[pipe].tv_sec >= gOverTime[pipe]+3)
         {
             TASK_FMT_DEBUG( pipe, COMM_LOG, "接收超时 \n");
             return 0;
         }
         usleep(2000);
     }
#endif
}



