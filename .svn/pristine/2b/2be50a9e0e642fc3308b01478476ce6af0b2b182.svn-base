#ifdef AREA_FUJIAN
#include "CtsAll.h"
#include "fujian_task.h"
#include "fujian_common.h"
extern char *smiosName;

uint8 IsSetGatherMode[PLC_NUM]={0};          //是否给本地设置过通信协议模式
uint8 gIsGetCCOInfo[PLC_NUM] = {0};         //是否获取过模块信息
RS485_RS232_PARA Rs485ComNo[MAX_485_NUM];

extern OOP_GATHER_E gGatherMode;

extern uint8 g_gatherflag;

extern char gUartDevName[MAX_485_NUM][64];



BOOL fujian_pipe_is_485(uint8 pipe)
{
    if(pipe>=PLC_NUM && pipe< PLC_NUM+MAX_485_NUM )
        return TRUE;
    return FALSE;
}

void Init_Get_Grop_Meter(FUJIAN_grop_meter *gropMeter)
{
    taskmng_grop_meter_init(gropMeter);
}

/*******************************************************************************
* 函数名称: fujian_gather_get
* 函数功能:从数据中心加载6e000200外部设备数据采集模式
* 输入参数:
* 输出参数: 
* 返 回 值: 
*******************************************************************************/
OOP_GATHER_E fujian_gather_mode_get(DB_CLIENT clientid)
{
    int   ret;
    uint32 Len;
    uint8 mode;
    OOP_GATHER_E result = OOP_GATHER_FJ;
    NOMAL_OAD_T NormalOad;
    NormalOad.logicId = 0;
    NormalOad.oad.value = 0x6E000200;
    NormalOad.infoNum = 0;

    ret = db_read_nomal(clientid,&NormalOad,1,&mode,&Len);
    if(ret!=0 || Len != 1)
    {
        TASK_FMT_TRACE(-1,UP_LOG, "加载0x6E000200失败 ret %d  \n",ret);
    }else
    {
        result = mode;
    }
    TASK_FMT_TRACE(-1,UP_LOG, "当前外部设备数据采集模式 %d (0 国网协议 1福建增补) \n",result);
    return result;
}

/*******************************************************************************
* 函数名称: taskmng_all_state_init
* 函数功能: 状态字初始化
* 输入参数: 无
* 输出参数: 无
* 返 回 值: 无
*******************************************************************************/
void fujian_state_init ( uint8 CommPipe )
{
    AmrThreadArg_T* pAmrThreadArg;

    pAmrThreadArg = &gAmrThreadArg[CommPipe];

    memset(&pAmrThreadArg->taskRunList,0x00,sizeof(FUJIAN_TASK_RUN_LIST_T));
    pAmrThreadArg->taskHead = NULL;
    //上电事件靠puamr通知
    //pAmrThreadArg->taskPowerOnFlag = 1; 
}

/*******************************************************************************
* 函数名称: fujian_data_init_check
* 函数功能: 检查是否收到数据初始化命令
* 输入参数:
* 输出参数: 
* 返 回 值: 
*******************************************************************************/
void fujian_data_init_check(uint8 pipe)
{
    if(DataInitFlag[pipe]==0)
        return;
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];

    uint16 i;

    //删除任务记录文件
    char pfile[200];
    int ret;

    for(i = 0;i<pAmrThreadArg->taskRunList.taskNum;i++)
    {
        memset(pfile,0x00,sizeof(pfile));
        sprintf(pfile,"%s/%s%d",PrivateDir,TASK_RECORD_FILE,pAmrThreadArg->taskRunList.taskList[i].taskid);
        if(access(pfile,R_OK)==0)
        {
            ret=remove(pfile);
            TASK_FMT_DEBUG(pipe,REC_LOG, "删除任务记录文件%s ret is %d\n",pfile,ret);
        }
        pAmrThreadArg->taskRunList.taskList[i].specificTaskIndex = 0;
        pAmrThreadArg->taskRunList.taskList[i].meterIndex = 0;
        pAmrThreadArg->taskRunList.taskList[i].result = SCHEME_UNDONE;
        pAmrThreadArg->taskRunList.taskList[i].storageCnt = 1;
        taskmng_trans_task_collection_state_update(pipe,&pAmrThreadArg->taskRunList.taskList[i],TRANS_TASK_CLEAR,NULL);

    }

    pAmrThreadArg->TaskModifyFlag = TASK_CFG_UNINIT;
    DataInitFlag[pipe] = 0;
    
}

/*******************************************************************************
* 函数名称: fujian_time_manage
* 函数功能: 时间管理函数
* 输入参数: 端口号
* 输出参数: 无
* 返 回 值: 无
*******************************************************************************/
void fujian_time_manage ( uint8 CommPipe )
{
    gPastTimeFlag[CommPipe].Word = 0;          //清除时间标志

    taskmng_cur_bin_time_get ( &gCurBinTime[CommPipe] );  //刷新当前HEX码时间
    taskmng_cur_bcd_time_get ( &gCurBcdTime[CommPipe] );  //刷新当前BCD码时间

    //过月
    if ( gCurBinTime[CommPipe].Month != gPreBinTime[CommPipe].Month )
    {
        //同时产生过月、过日、过时、过分、过秒标志
        gPastTimeFlag[CommPipe].BitSect.Month   = TRUE;
        gPastTimeFlag[CommPipe].BitSect.Day     = TRUE;
        gPastTimeFlag[CommPipe].BitSect.Hour    = TRUE;
        gPastTimeFlag[CommPipe].BitSect.Minute  = TRUE;
        gPastTimeFlag[CommPipe].BitSect.Second  = TRUE;
    }
    //过日
    if ( gCurBinTime[CommPipe].Day != gPreBinTime[CommPipe].Day )
    {

        gPastTimeFlag[CommPipe].BitSect.Day     = TRUE;
        gPastTimeFlag[CommPipe].BitSect.Hour    = TRUE;
        gPastTimeFlag[CommPipe].BitSect.Minute  = TRUE;
        gPastTimeFlag[CommPipe].BitSect.Second  = TRUE;
    }
    //过时
    else if ( gCurBinTime[CommPipe].Hour != gPreBinTime[CommPipe].Hour )
    {
        //同时产生过时、过分标志
        gPastTimeFlag[CommPipe].BitSect.Hour    = TRUE;
        gPastTimeFlag[CommPipe].BitSect.Minute  = TRUE;
        gPastTimeFlag[CommPipe].BitSect.Second  = TRUE;
    }
    //过分
    else if ( gCurBinTime[CommPipe].Minute != gPreBinTime[CommPipe].Minute )
    {
        //产生过分标志
        gPastTimeFlag[CommPipe].BitSect.Minute = TRUE;
        gPastTimeFlag[CommPipe].BitSect.Second = TRUE;

    }

    //过秒
    else if ( gCurBinTime[CommPipe].Second != gPreBinTime[CommPipe].Second )
    {
        gPastTimeFlag[CommPipe].BitSect.Second = TRUE;
    }
    else
    {
        usleep ( 10000 );
    }

    //过月处理
    if ( gPastTimeFlag[CommPipe].BitSect.Month == TRUE )
    {
        
    }

    //过日处理
    if ( gPastTimeFlag[CommPipe].BitSect.Day == TRUE )
    {
 
    }

    //过时处理
    if ( gPastTimeFlag[CommPipe].BitSect.Hour == TRUE )
    {

    }

    //过分处理
    if ( gPastTimeFlag[CommPipe].BitSect.Minute == TRUE )
    {
    }

    if ( gPastTimeFlag[CommPipe].BitSect.Second == TRUE )
    {
    }

    memcpy ( &gPreBinTime[CommPipe], &gCurBinTime[CommPipe], sizeof ( DateTime_T ) );

}


/*******************************************************************************
* 函数名称: taskmng_task_record_file_del_check 
* 函数功能: 检查是否有需要删除的任务记录文件
* 输入参数: pipe 端口号

* 输出参数:  
* 返 回 值:
*******************************************************************************/
int fujian_task_record_file_del_check(uint8 pipe)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];

    int Recvlen = 0;
    uint16 i;
    DB_UPDATE_T dbupdate;
    memset(&dbupdate,0x00,sizeof(DB_UPDATE_T));
    char  pfile[100];
    int ret;
    Recvlen = taskmng_db_update_queue_read(&pTaskRecordFileDel[pipe], &dbupdate);
    if(Recvlen>0)
    {

        TASK_FMT_DEBUG(pipe, REC_LOG, "收到任务 %d 的变更消息 删除任务记录文件\n",dbupdate.infonum);
        
        memset(pfile,0x00,sizeof(pfile));
        sprintf(pfile,"%s/%s%d",PrivateDir,TASK_RECORD_FILE,dbupdate.infonum);
        if(access(pfile,R_OK)==0)
        {
            ret=remove(pfile);
            TASK_FMT_DEBUG(pipe,REC_LOG, "删除任务记录文件%s ret is %d\n",pfile,ret);
        }
        //删除任务记录文件时清空采集状态集
        for(i=0;i<pAmrThreadArg->taskRunList.taskNum;i++)
        {
            if(pAmrThreadArg->taskRunList.taskList[i].taskid == dbupdate.infonum )
            {
                taskmng_trans_task_collection_state_update(pipe,&pAmrThreadArg->taskRunList.taskList[i],TRANS_TASK_CLEAR,NULL);
                break;
            }
        }
        return 1;
    }
    return 0;
}


/*******************************************************************************
* 函数名称: fujian_time_calc 
* 函数功能: 计算任务开始结束时间 datetimeOut = (datetime1除以ti的整数部分)*ti+(datetime2除以ti的小数部分)
            datetime2允许是空 计算出来的是存储时标           
            最新时间规整值从2000年1月1日开始算
* 输入参数: datetime1  ti datetime2
 
* 输出参数:  datetimeOut
* 返 回 值:0 成功 -1 失败
*******************************************************************************/

int fujian_time_calc(OOP_DATETIME_S_T * datetime1, OOP_TI_T * ti, OOP_DATETIME_S_T * datetime2, 
    OOP_DATETIME_S_T * datetimeOut)
{
    uint32          period;

    if(datetime1 == NULL || ti == NULL || datetimeOut == NULL)
        return - 1;

    period = ti->value;


    //time_t只到2038年 所以计算的时候如果超过全按照2037年算
    OOP_DATETIME_S_T tmp1, tmp2, tmpout,tmpbegin;
    time_t          tmp1_t, tmp2_t, out_t,tmpbegin_t;
    memset(&tmpbegin,0x00,sizeof(OOP_DATETIME_S_T));
    tmpbegin.year = 2000;
    tmpbegin.month = 1;
    tmpbegin.day = 1;
    DT_DateTimeShort2Time(&tmpbegin, &tmpbegin_t);
    
    memcpy(&tmp1, datetime1, sizeof(OOP_DATETIME_S_T));
    if(datetime2 != NULL)
    {
        memcpy(&tmp2, datetime2, sizeof(OOP_DATETIME_S_T));
    }
    memset(&tmpout, 0x00, sizeof(OOP_DATETIME_S_T));

    if(tmp1.year >= 2038)
        tmp1.year = 2037;

    if(tmp1.year < 2000)
        tmp1.year = 2000;

    if(tmp2.year >= 2038)
        tmp2.year = 2037;

    if(tmp2.year < 2000)
        tmp2.year = 2000;

    
    DT_DateTimeShort2Time(&tmp1, &tmp1_t);
    tmp1_t -= tmpbegin_t;
    if(datetime2 != NULL)
    {
        DT_DateTimeShort2Time(&tmp2, &tmp2_t);
        tmp2_t -= tmpbegin_t;
    }
    else
    {
        tmp2_t = 0;
    }
    

    switch(ti->unit)
    {
        case TI_SEC:
            {
                if(period == 0)
                {
                    period = 60;
                }
                out_t = (tmp1_t / period) *period + tmp2_t % period;
                out_t+=tmpbegin_t;
                DT_Time2DateTimeShort(out_t, &tmpout);
            }
            break;

        case TI_MIN:
            {
                if(period == 0)
                {
                    period = 1;
                }
                period = period * 60;
                out_t = (tmp1_t / period) *period + tmp2_t % period;
                out_t+=tmpbegin_t;
                DT_Time2DateTimeShort(out_t, &tmpout);
            }
            break;

        case TI_HOUR:
            {
                if(period == 0)
                {
                    period = 1;
                }
                period = period * 3600;
                out_t = (tmp1_t / period) *period + tmp2_t % period;
                out_t+=tmpbegin_t;
                DT_Time2DateTimeShort(out_t, &tmpout);
            }
            break;

        case TI_DAY:
            {
                if(period == 0)
                {
                    period = 1;
                }
                period = period * 86400;
                out_t = (tmp1_t / period) *period + tmp2_t % period;
                out_t += tmpbegin_t;
                DT_Time2DateTimeShort(out_t, &tmpout);
            }
            break;

        //月和年默认是一个月和一年
        //datetime2的天可能是31号 不能直接给到当前的月 当前月可能没有31天
        case TI_MON:
            {
                tmpout.year = datetime1->year;
                tmpout.month = datetime1->month;
                if(NULL != datetime2)
                {
                    tmpout.day = datetime2->day;
                    tmpout.hour = datetime2->hour;
                    tmpout.minute = datetime2->minute;
                    tmpout.second = datetime2->second;
                    tmpout.week = datetime2->week;
                    while(DT_DateTimeShortIsValid(&tmpout)==FALSE)
                    {
                        tmpout.day--;
                    }
                }else
                {
                    tmpout.day = 1;
                }

            }
            break;

        case TI_YEAR:
            {
                tmpout.year = datetime1->year;
                if(NULL != datetime2)
                {
                    tmpout.month = datetime2->month;
                    tmpout.day = datetime2->day;
                    tmpout.hour = datetime2->hour;
                    tmpout.minute = datetime2->minute;
                    tmpout.second = datetime2->second;
                    tmpout.week = datetime2->week;
                }
                
            }
            break;

        default:
            return - 1;

            break;
    }

    memcpy(datetimeOut, &tmpout, sizeof(OOP_DATETIME_S_T));
    return 0;
}



/*******************************************************************************
* 函数名称: fujian_datetime_s_log_write 
* 函数功能: 把OOP_DATETIME_S_T格式数据写日志
* 输入参数: 
 
* 输出参数:  
* 返 回 值:
*******************************************************************************/
void fujian_datetime_s_log_write(uint8 pipe,char* path,OOP_DATETIME_S_T *datetime)
{
    TASK_FMT_TRACE( pipe, path,"%d/%02d/%02d %02d:%02d:%02d\n",datetime->year,datetime->month,datetime->day,datetime->hour,
        datetime->minute,datetime->second);
}

/*******************************************************************************
* 函数名称: fujian_time_compare 
* 函数功能: 对比time1和datetime表示的时间
* 输入参数: 
 
* 输出参数:  
* 返 回 值:-1 小于 0等于 1大于
*******************************************************************************/
int fujian_time_compare(time_t time1,OOP_DATETIME_S_T *datetime)
{
    time_t tmp = 0;
    if(DT_DateTimeShort2Time(datetime,&tmp)==FALSE)
    {
        //时间无效
        return 1;
    }else
    {
        if(tmp!=0)
        {
            if(time1<tmp)
                return -1;
            else if(time1==tmp)
                return 0;
            else 
                return 1;
        }else
        {
            //等于0时表示可能年大于2038 或者小于1970
            if(datetime->year<1970)
                return 1;
            else
                return -1;
        }
    }
}

/*******************************************************************************
* 函数名称: fujian_datetime_s_compare 
* 函数功能: 对比datetime1和datetime2表示的时间
* 输入参数: 
 
* 输出参数:  
* 返 回 值:-1 小于 0等于 1大于
*******************************************************************************/
int fujian_datetime_s_compare(OOP_DATETIME_S_T *datetime1,OOP_DATETIME_S_T *datetime2)
{
    if(datetime1->year<datetime2->year)
    {
        return -1;
    }else if(datetime1->year>datetime2->year)
    {
        return 1;
    }else
    {
        OOP_DATETIME_S_T time1,time2;
        time_t tmp1,tmp2;
        memcpy(&time1,datetime1,sizeof(OOP_DATETIME_S_T));
        memcpy(&time2,datetime2,sizeof(OOP_DATETIME_S_T));
        //既然年都相同 就把年全变成2000年比较 规避转time_t的问题
        time1.year = 2000;
        time2.year = 2000;
        DT_DateTimeShort2Time(&time1,&tmp1);
        DT_DateTimeShort2Time(&time2,&tmp2);
        if(tmp1<tmp2)
            return -1;
        else if(tmp1 == tmp2)
            return 0;
        else 
            return 1;
    }
}


/*******************************************************************************
* 函数名称: taskmng_local_addr_set 
* 函数功能: //给本地通信APP设置终端通信地址
* 输入参数: pipe 端口号
* 输出参数: 
* 返 回 值: 
*******************************************************************************/
void fujian_local_addr_set(uint8 pipe)
{

    uint8 payload[20]={0};
    uint8 len = 0;
    OOP_OAD_U pipeoad = taskmng_pipe_oad_get( pipe);
    memcpy_r(&payload[len],(uint8*)&pipeoad.value,4);
    len+=4;
    memcpy_r(&payload[len],LocalAddr,6);
    len+=6;
    uint16 recvlen;
    uint8 recvbuf[100]={0};
    taskmng_msg_fun_deal(pipe,0x0003,0x0088,len,payload,&recvlen,recvbuf);
    if(recvlen==1&&recvbuf[0]==1)
    {
        TASK_FMT_DEBUG(pipe,COMM_LOG,"设置模块地址成功 \n");
        IsSetAddr[pipe] = 1;
    }

}

/*******************************************************************************
* 函数名称: taskmng_local_addr_set 
* 函数功能: //给本地通信APP设置通信协议模式
* 输入参数: pipe 端口号
* 输出参数: 
* 返 回 值: 
*******************************************************************************/
void fujian_gather_mode_set(uint8 pipe)
{

    uint8 payload[20]={0};
    uint8 len = 0;
    OOP_OAD_U pipeoad = taskmng_pipe_oad_get( pipe);
    memcpy_r(&payload[len],(uint8*)&pipeoad.value,4);
    len+=4;
    payload[len++] = gGatherMode+1; //698和3762规定的不一样

    uint16 recvlen;
    uint8 recvbuf[100]={0};
    taskmng_msg_fun_deal(pipe,0x0003,0x0096,len,payload,&recvlen,recvbuf);
    if(recvlen==1&&recvbuf[0]==1)
    {
        TASK_FMT_DEBUG(pipe,COMM_LOG,"设置通信协议模式 \n");
        IsSetGatherMode[pipe] = 1;
    }

}


/*******************************************************************************
* 函数名称: taskmng_local_addr_set 
* 函数功能: //从本地通信APP获取通信协议模式
* 输入参数: pipe 端口号
* 输出参数: 
* 返 回 值: 
*******************************************************************************/
int fujian_gather_mode_get_from_ccoRouter(uint8 pipe)
{
    int result = -1;
    uint8 payload[20]={0};
    uint8 len = 0;
    OOP_OAD_U pipeoad = taskmng_pipe_oad_get( pipe);
    memcpy_r(&payload[len],(uint8*)&pipeoad.value,4);
    len+=4;
    uint16 recvlen;
    uint8 recvbuf[100]={0};
    taskmng_msg_fun_deal(pipe,0x0003,0x0087,len,payload,&recvlen,recvbuf);
    if(recvlen==1)
    {
        result = recvbuf[0];
        TASK_FMT_DEBUG(pipe,COMM_LOG,"从ccoRouter获取到通信协议模式 %d 1-国网；2-福建\n",result);
    }
    return result;

}
/**********************************************************************
* @name      : fujian_rs485_release
* @brief     ：串口释放
* @param[in] :comdcb  本地通讯参数
* @param[out]：
* @return    ：0-成功/失败
* @Create    : 
* @Date      ：2020-3-10
* @Update    :
**********************************************************************/
//void fujian_rs485_release(uint8 pipe)
//{
//    AmrThreadArg_T * pAmrThreadArg;
//    pAmrThreadArg = &gAmrThreadArg[pipe];
// 
//    if(pAmrThreadArg->rs485Dev == NULL)
//        return;
//    hal_device_release((HW_DEVICE *)pAmrThreadArg->rs485Dev);
//    pAmrThreadArg->rs485Dev = NULL;
//
//}


/**********************************************************************
* @name      : fujian_rs485_init
* @brief     ：串口初始化
* @param[in] :comdcb  本地通讯参数
* @param[out]：
* @return    ：0-成功/失败
* @Create    : 
* @Date      ：2020-3-10
* @Update    :
**********************************************************************/
int fujian_rs485_init(uint8 pipe,OOP_COMDCB_T *comdcb)
{
    AmrThreadArg_T * pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
//    static OOP_COMDCB_T lastParam[MAX_485_NUM] ; //记录下上一次用的通信参数 如果一样则不需要再
//    
//    //UART_DEVICE_T* dev;
//    int speed;
//    int ret=0;
//
//    //TASK_FMT_DEBUG( -1, COMM_LOG, "第一次获取设备初始化 pCommPara->dev11 is %p\n",pCommPara->dev);
//    if(pAmrThreadArg->rs485Dev == NULL)
//    {
//        pAmrThreadArg->rs485Dev = (UART_DEVICE_T*)hal_device_get(gUartDevName[pipe-PLC_NUM]);
//        if(NULL == pAmrThreadArg->rs485Dev)
//        {
//            TASK_FMT_DEBUG( -1, COMM_LOG, "hal_device_get %s 失败\n",gUartDevName[pipe-PLC_NUM]);
//            return -1;
//        }
//    }
    //如果需要重设的参数和以前的一样 则不再重设
//    if(memcmp(comdcb,&lastParam[pipe-PLC_NUM],sizeof(OOP_COMDCB_T)) == 0)
//    {
//        return 0;
//    }
    pAmrThreadArg->rs485baud = comdcb->baud;
//    dev = pAmrThreadArg->rs485Dev;
//    speed = uart_baud_get(comdcb->baud);
//    //有的测试用例配置的数据位和停止位是错的 先写死
//    //ret=dev->uart_param_set(dev, speed, comdcb->databits, comdcb->stopbits, comdcb->parity);
//    ret=dev->uart_param_set(dev, speed, 8, 1, PARITY_EVEN);
//
//    memcpy(&lastParam[pipe-PLC_NUM],comdcb,sizeof(OOP_COMDCB_T));
    //TASK_FMT_DEBUG( pipe, COMM_LOG, "初始化 speed %d ret %d \n",speed,ret);

    return 0;
}

int fujian_rs485_send(uint8 pipe,uint8 overtime,uint8 *sendbuf,uint16 sendlen)
{
    AmrThreadArg_T * pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];

//    UART_DEVICE_T* dev = pAmrThreadArg->rs485Dev;
//    if(pAmrThreadArg->rs485Dev == NULL)
//    {
//        TASK_FMT_TRACE( pipe, REC_LOG,"rs485Dev 无效 发送失败 \n");
//        return -1;
//    }
//    dev->uart_data_send(dev, sendbuf, sendlen);
    rs485_RS232_send(pipe,overtime,MAX_CHAR_OVERTIME,pAmrThreadArg->rs485baud,PARITY_EVEN,sendbuf, sendlen);
    return 0;
}

/**********************************************************************
* @name      : fujian_rs485_recv
* @brief     ：串口接收
* @param[in] :comdcb  本地通讯参数
                overtime 超时时间 单位秒

* @param[out]：
* @return    ：
* @Create    : 
* @Date      ：
* @Update    :
**********************************************************************/


int fujian_rs485_recv(uint8 pipe,uint8 overtime,OOP_COMDCB_T *comdcb, uint8* pBuffer, int nBufLen)
{
    return rs485_RS232_recv(pipe,pBuffer,nBufLen);

#if 0

    AmrThreadArg_T * pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
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
    UART_DEVICE_T* dev = pAmrThreadArg->rs485Dev;

    if(dev == NULL)
    {
        return 0;
    }

    fd =  dev->uart_get_handle(dev);

    if (0 >= fd)  
    {  
        return 0;  
    }
    /*串口接收数据超时时间按20个字节间隔时间来计算，300bps 发送一个字节需要36667us
    一个字节包括起始位、停止位、校验位、8位数据 共11位*/
    
    speed = uart_baud_get(comdcb->baud);
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
            recv_len = dev->uart_data_recv(dev,pBuffer+data_len, nBufLen-data_len);    /*调用read()，进行串口数据接收*/
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
#endif
}




/*******************************************************************************
* 函数名称: fujian_trans_task_buf_send
* 函数功能: 数据发送 根据通道号区分如何发送数据 载波扔队列 485直接发送
* 输入参数: pipi 通道号
            iop 如果是载波的 需要iop
            sendbuf 发送的报文 载波的payload
            sendlen 报文长度
* 输出参数: 无
* 返 回 值: >0                  成功
*           <0                  失败
*******************************************************************************/
int fujian_trans_task_buf_send(uint8 pipe,uint16 iop,uint8* sendbuf,uint16 sendlen,uint16 *msgIndex)
{
//    AmrThreadArg_T * pAmrThreadArg;
//    pAmrThreadArg = &gAmrThreadArg[pipe];

    if(pipe<PLC_NUM)
    {
        //载波
        MESSAGE_INFO_T    *MsgSend = NULL;
        MsgSend = (MESSAGE_INFO_T*)malloc(sizeof(MESSAGE_INFO_T)+sendlen);
        memset(MsgSend,0x00,sizeof(MESSAGE_INFO_T)+sendlen);
        MsgSend->rpm = 1;
        MsgSend->index = GetMsgSeq();
        *msgIndex = MsgSend->index;
        MsgSend->label = pipe;
        MsgSend->priority = 1;
        MsgSend->IID = 0x0003;
        MsgSend->IOP = iop;
        MsgSend->msglen = sendlen;
        MsgSend->sourlen = strlen(DownName);
        memcpy(MsgSend->souraddr,DownName,MsgSend->sourlen);
        MsgSend->destlen = strlen(CCOName);
        memcpy(MsgSend->destaddr,CCOName,MsgSend->destlen );
        memcpy(MsgSend->playload,sendbuf,sendlen);
        TASK_FMT_TRACE( pipe, REC_LOG,"Sendframe msgindex %d \n",MsgSend->index);
        taskmng_plc_queue_write(pAmrSendDown[pipe], (uint8*)MsgSend, sizeof(MESSAGE_INFO_T)+sendlen);
        free(MsgSend);
    }else if(pipe>=PLC_NUM && pipe < PLC_NUM+MAX_485_NUM)
    {
        //485
//        UART_DEVICE_T* dev = pAmrThreadArg->rs485Dev;
//        if(pAmrThreadArg->rs485Dev == NULL)
//        {
//            TASK_FMT_TRACE( pipe, REC_LOG,"rs485Dev 无效 发送失败 \n");
//            return -1;
//        }
//        dev->uart_data_send(dev, sendbuf, sendlen);
        fujian_rs485_send(pipe,3,sendbuf, sendlen);
    }else
    {
        MESSAGE_INFO_T    *MsgSend = NULL;
        MsgSend = (MESSAGE_INFO_T*)malloc(sizeof(MESSAGE_INFO_T)+sendlen);
        memset(MsgSend,0x00,sizeof(MESSAGE_INFO_T)+sendlen);
        MsgSend->rpm = 1;
        MsgSend->index = GetMsgSeq();
        *msgIndex = MsgSend->index;
        MsgSend->label = pipe;
        MsgSend->priority = 1;
        MsgSend->IID = 0x0000;
        MsgSend->IOP = iop;
        MsgSend->msglen = sendlen;
        MsgSend->sourlen = strlen(DownName);
        memcpy(MsgSend->souraddr,DownName,MsgSend->sourlen);
        //如果端口是交采的 则发给puamr
        if(pipe == PLC_NUM + MAX_485_NUM + COMM_MODULE_NUM)
        {
            MsgSend->destlen = strlen(PUAMR_APP_NAME);
            memcpy(MsgSend->destaddr, PUAMR_APP_NAME, MsgSend->destlen);
        }
        else
        {
            MsgSend->destlen = strlen(WIRELESSName);
            memcpy(MsgSend->destaddr, WIRELESSName, MsgSend->destlen);
        }
        
        memcpy(MsgSend->playload,sendbuf,sendlen);
        TASK_FMT_TRACE( pipe, REC_LOG,"Sendframe msgindex %d \n",MsgSend->index);
        taskmng_plc_queue_write(pAmrSendDown[pipe], (uint8*)MsgSend, sizeof(MESSAGE_INFO_T)+sendlen);
        free(MsgSend);
    }
    return 0;
}

/*******************************************************************************
* 函数名称: fujian_send_payload_pack_0100
* 函数功能: 数据转发（国网）
* 输入参数: pipi 通道号
            sendbuf 发送的报文
            sendlen 报文长度
* 输出参数: payload
* 返 回 值: >0                  成功
*           <0                  失败
*******************************************************************************/
uint16 fujian_send_payload_pack_0100(uint8 pipe,uint8* sendbuf,uint16 sendlen,uint8 *payload)
{
    uint8  buf[4000]={0};
    uint16 index = 0;
    uint8 lenoffset;
    OOP_OAD_U pipeoad = taskmng_pipe_oad_get(pipe); ;
    memcpy_r(&buf[index],&pipeoad.value,4);
    index+=4;
    buf[index++] = 0;   //协议类型写0
    lenoffset = set_len_offset(sendlen,&buf[index]);
    index+=lenoffset;
    memcpy(&buf[index],sendbuf,sendlen);
    index+=sendlen;

    memcpy(payload,buf,index);
    return index;
}
/*******************************************************************************
* 函数名称: fujian_recv_payload_unpack_0100
* 函数功能: 数据转发（国网）payload解析
* 输入参数: 
            payload 发送的报文 
            sendlen 报文长度
* 输出参数: buf
* 返 回 值: 数据长度
*******************************************************************************/
uint16 fujian_recv_payload_unpack_0012(uint8* payload,uint16 payloadLen,uint8 *buf)
{
    uint16 len;
    uint16 index = 0;   
    uint8 lenoffset;
    len = get_len_offset(&payload[index], &lenoffset);
    index+=lenoffset;
    memcpy(buf,&payload[index],len);
    return len;
}


/*******************************************************************************
* 函数名称: fujian_send_payload_pack_0012
* 函数功能: 通用透明传输
* 输入参数: pipi 通道号
            sendbuf 发送的报文
            sendlen 报文长度
* 输出参数: payload
* 返 回 值: >0                  成功
*           <0                  失败
*******************************************************************************/
uint16 fujian_send_payload_pack_0012(uint8 pipe,uint8* sendbuf,uint16 sendlen,uint8 *payload)
{
    uint8  buf[4000]={0};
    uint16 index = 0;
    uint8 lenoffset;
    lenoffset = set_len_offset(sendlen,&buf[index]);
    index+=lenoffset;
    memcpy(&buf[index],sendbuf,sendlen);
    index+=sendlen;
    memcpy(payload,buf,index);
    return index;
}

/*******************************************************************************
* 函数名称: fujian_send_payload_pack_0100
* 函数功能: 数据转发（国网）
* 输入参数: pipe 通道号
            tsa 地址
            sendbuf 发送的报文 
            sendlen 报文长度
* 输出参数: payload
* 返 回 值: >0                  成功
*           <0                  失败
*******************************************************************************/
uint16 fujian_send_payload_pack_0101(uint8 pipe,OOP_TSA_T *tsa,uint8* sendbuf,uint16 sendlen,uint8 *payload)
{
    uint8  buf[4000]={0};
    uint16 index = 0;
    uint8 lenoffset;
    uint16 overtime = 0;
    OOP_OAD_U pipeoad = taskmng_pipe_oad_get(pipe); 
    memcpy_r(&buf[index],&pipeoad.value,4);
    index+=4;
    buf[index++] = 3;   //通信对象类型 默认电能表类
    memcpy(&buf[index],tsa->add,6);
    index+=6;
    buf[index++] = 0;   //通讯控制字
    overtime = 90;      //帧超时时间 90秒
    memcpy_r(&buf[index],&overtime,2);
    index+=2;
    overtime = 50;      //字符超时时间 0.5秒 单位10ms
    memcpy_r(&buf[index],&overtime,2);
    index+=2;
    lenoffset = set_len_offset(sendlen,&buf[index]);
    index+=lenoffset;
    memcpy(&buf[index],sendbuf,sendlen);
    index+=sendlen;

    memcpy(payload,buf,index);
    return index;
}

/*******************************************************************************
* 函数名称: fujian_recv_payload_unpack_0100
* 函数功能: 数据转发（国网）payload解析
* 输入参数: 
            payload 发送的报文 
            sendlen 报文长度
* 输出参数: buf
* 返 回 值: 数据长度
*******************************************************************************/
uint16 fujian_recv_payload_unpack_0100(uint8* payload,uint16 payloadLen,uint8 *buf)
{
    uint16 len;
    uint16 index = 0;   
    uint8 lenoffset;
    index++;    //协议类型
    len = get_len_offset(&payload[index], &lenoffset);
    index+=lenoffset;
    memcpy(buf,&payload[index],len);
    return len;
}

/*******************************************************************************
* 函数名称: fujian_recv_payload_unpack_0100
* 函数功能: 数据转发（国网）payload解析
* 输入参数: 
            payload 发送的报文 
            sendlen 报文长度
* 输出参数: buf
* 返 回 值: 数据长度
*******************************************************************************/
uint16 fujian_recv_payload_unpack_0101(uint8* payload,uint16 payloadLen,uint8 *buf)
{
    uint16 len;
    uint16 index = 0;   
    uint8 lenoffset;

    len = get_len_offset(&payload[index], &lenoffset);
    index+=lenoffset;
    memcpy(buf,&payload[index],len);
    return len;
}

/*******************************************************************************
* 函数名称: fujian_tsa_add
* 函数功能: 查找meterList里面有没有和addr相同的地址 如果没有则插入
* 输入参数: addr 新增加的地址
            num 表里地址个数
            meterList 地址表
* 输出参数: buf
* 返 回 值: 
*******************************************************************************/
int fujian_tsa_add(OOP_TSA_T * addr, uint16 * num, OOP_TSA_T * meterList)
{
    uint16          meternum = *num;
    uint16          i;
    uint8           flag = 0;
    if(NULL == addr)
        return -1;
    for(i = 0; i < meternum; i++)
    {
        if(memcmp(&meterList[i], addr, sizeof(OOP_TSA_T))==0)
        {
            flag = 1;
            break;
        }
    }

    if(flag == 0)
    {
        memcpy(&meterList[meternum], addr, sizeof(OOP_TSA_T));
        meternum++;
        *num = meternum;
    }

    return 0;
}

/*******************************************************************************
* 函数名称: fujian_module_info_get 
* 函数功能: 查询本地模块参数 获取预告抄读对象数量 0082 对应AFN53F1
* 输入参数: pipe 端口号
* 输出参数: 
* 返 回 值: 预告抄读对象 0表示失败
*******************************************************************************/
uint8 fujian_module_info_get(uint8 pipe)
{
    uint8 objectNum=0;
    uint8 payload[20]={0};
    uint8 len = 0;
    int ret;
    OOP_OAD_U pipeoad = taskmng_pipe_oad_get( pipe);
    memcpy_r(&payload[len],(uint8*)&pipeoad.value,4);
    len+=4;

    uint16 recvlen;
    uint8 recvbuf[100]={0};
    ret =taskmng_msg_fun_deal(pipe,0x0003,0x0082,len,payload,&recvlen,recvbuf);
    if(ret>0)
    {
        if(recvlen!=3)
            return 0;
        objectNum = recvbuf[2];
        return objectNum;
    }
    return objectNum;

}

/*******************************************************************************
* 函数名称: fujian_amr_mode_get 
* 函数功能: 查询抄读方式 0108
* 输入参数: pipe 端口号
* 输出参数: 
* 返 回 值: 预告抄读对象 0表示失败
*******************************************************************************/
uint8 fujian_amr_mode_get(uint8 pipe)
{
    uint8 mode=0;
    uint8 payload[20]={0};
    uint8 len = 0;

    int ret;
    OOP_OAD_U pipeoad = taskmng_pipe_oad_get( pipe);
    memcpy_r(&payload[len],(uint8*)&pipeoad.value,4);
    len+=4;

    uint16 recvlen;
    uint8 recvbuf[100]={0};
    ret =taskmng_msg_fun_deal(pipe,0x0003,0x0108,len,payload,&recvlen,recvbuf);
    if(ret>0)
    {
        if(recvlen!=1)
            return 0;
        mode = recvbuf[0];
        return mode;
    }
    return mode;

}

/*******************************************************************************
* 函数名称: fujian_current_task_end 
* 函数功能: 结束当前任务 0093
* 输入参数: pipe 端口号
* 输出参数: 
* 返 回 值: 
*******************************************************************************/
uint8 fujian_current_task_end(uint8 pipe)
{
    uint8 payload[20]={0};
    uint8 len = 0;

    int ret;
    OOP_OAD_U pipeoad = taskmng_pipe_oad_get( pipe);
    memcpy_r(&payload[len],(uint8*)&pipeoad.value,4);
    len+=4;

    uint16 recvlen;
    uint8 recvbuf[100]={0};
    TASK_FMT_TRACE( pipe, REC_LOG, "发送命令载波结束当前任务 \n");
    ret =taskmng_msg_fun_deal(pipe,0x0003,0x0093,len,payload,&recvlen,recvbuf);
    if(ret>0)
    {
        if(recvlen!=1)
            return 0;
        return recvbuf[0];
    }
    return 0;

}

/*******************************************************************************
* 函数名称: fujian_trans_task_buf_ack
* 函数功能: 数据发送 上报的回码
* 输入参数: pipi 通道号
            iop 如果是载波的 需要iop
            sendbuf 发送的报文 载波的payload
            sendlen 报文长度
* 输出参数: 无
* 返 回 值: >0                  成功
*           <0                  失败
*******************************************************************************/
int fujian_trans_task_buf_ack(uint8 pipe,uint16 iop,uint8* sendbuf,uint16 sendlen,uint16 msgIndex,uint16 msgLabel)
{

    MESSAGE_INFO_T    *MsgSend = NULL;
    MsgSend = (MESSAGE_INFO_T*)malloc(sizeof(MESSAGE_INFO_T)+sendlen);
    memset(MsgSend,0x00,sizeof(MESSAGE_INFO_T)+sendlen);
    MsgSend->rpm = 0;
    MsgSend->index = msgIndex;
    MsgSend->label = msgLabel;
    MsgSend->priority = 1;
    MsgSend->IID = 0x0003;
    MsgSend->IOP = iop;
    MsgSend->msglen = sendlen;
    MsgSend->sourlen = strlen(DownName);
    memcpy(MsgSend->souraddr,DownName,MsgSend->sourlen);
    MsgSend->destlen = strlen(CCOName);
    memcpy(MsgSend->destaddr,CCOName,MsgSend->destlen );
    memcpy(MsgSend->playload,sendbuf,sendlen);
    TASK_FMT_TRACE( pipe, REC_LOG,"Sendframe msgindex %d \n",MsgSend->index);
    taskmng_plc_queue_write(pAmrSendDown[pipe], (uint8*)MsgSend, sizeof(MESSAGE_INFO_T)+sendlen);
    free(MsgSend);

    return 0;
}

/*******************************************************************************
* 函数名称: fujian_meter_search_start 
* 函数功能: 启动从节点注册 0093
* 输入参数: pipe 端口号
* 输出参数: 
* 返 回 值: 
*******************************************************************************/
uint8 fujian_meter_search_start(uint8 pipe,uint16 allowTime)
{
    uint8 payload[20]={0};
    uint8 len = 0;

    int ret;
    OOP_OAD_U pipeoad = taskmng_pipe_oad_get( pipe);
    memcpy_r(&payload[len],&pipeoad.value,4);
    len+=4;
    memcpy_r(&payload[len],&allowTime,2);
    len+=2;
    
    uint16 recvlen;
    uint8 recvbuf[100]={0};
    TASK_FMT_TRACE( pipe, REC_LOG, "启动搜表 允许时间 %d 分钟\n",allowTime);
    ret =taskmng_msg_fun_deal(pipe,0x0003,0x0092,len,payload,&recvlen,recvbuf);
    if(ret>0)
    {
        if(recvlen!=1)
            return 0;
        return recvbuf[0];
    }
    return 0;

}

/*******************************************************************************
* 函数名称: fujian_metersearch_result_check 
* 函数功能://检查搜表结果是否重复 如果重复则不写入数据中心 福建只能对比地址
* 输入参数:pipe 端口号
        searchresult 搜表结果
* 输出参数: infonum 写入数据中心用的信息点号
* 返 回 值: TRUE 没有重复的 FALSE有重复的
*******************************************************************************/
BOOL fujian_metersearch_result_check(uint8 pipe,OOP_SEARCHMET_RES_T searchresult,uint16 *infonum)
{

    uint16 i;
    int index = -1;
    uint8 tmpaddr[6];
    pthread_mutex_lock(&MeterSearchResult.lock);
    TASK_FMT_DEBUG( pipe, REC_LOG, "CheckMeterSearchResult infonum %d count %d \n",MeterSearchResult.infonum,MeterSearchResult.count);
    for(i=0;i<MeterSearchResult.count;i++)
    {
        memcpy_r(tmpaddr,searchresult.commAdd.add,6);
        if( memcmp(tmpaddr,MeterSearchResult.result[i].Addr,6)==0)
        {
            pthread_mutex_unlock(&MeterSearchResult.lock);
            return FALSE;
        }
    }

    if(index==-1)
    {
        MeterSearchResult.infonum++;

        MeterSearchResult.result[MeterSearchResult.count].infonum = MeterSearchResult.infonum;
        memcpy_r(MeterSearchResult.result[MeterSearchResult.count].Addr,searchresult.commAdd.add,6);
        MeterSearchResult.count++;

        *infonum = MeterSearchResult.infonum;
    }
    TASK_FMT_DEBUG( pipe, REC_LOG, "fujian_metersearch_result_check 添加记录 infonum  %d count %d \n",MeterSearchResult.infonum,MeterSearchResult.count);

    pthread_mutex_unlock(&MeterSearchResult.lock);
    return TRUE;
}


uint8 SearchMetetPackMeterPrtl(uint8 *SearchMeterSend, uint8 *MeterAddr, uint8 PrtlId,uint8 pipe)
{
    uint32  CheckSum = 0;                       //校验和
    uint8   CurIndex = 0;                           //索引
    int     i;
    uint16 oopcs = 0;

    if(PrtlId == PRTL_OOP)
    {
        SearchMeterSend[CurIndex++] =0x68;
        /* 长度域 */
        SearchMeterSend[CurIndex++] = 23;
        SearchMeterSend[CurIndex++] = 0;
        /* 控制域 */
        SearchMeterSend[CurIndex++] = 0x43;
        SearchMeterSend[CurIndex++] = (OOP_ADDRLEN-1) | 0x40;
        memcpy(&SearchMeterSend[CurIndex], MeterAddr, 6);
        CurIndex+=6;
        SearchMeterSend[CurIndex++] = 0;
        /* 首部校验 */
        oopcs = CRC16_Get((const uint8 *)&SearchMeterSend[1+OOP_0XFE_CNT],11);
        SearchMeterSend[CurIndex++] = (uint8)(oopcs & 0x00FF);
        SearchMeterSend[CurIndex++] = (uint8)(oopcs >> 8);
        /* 用户数据 */
        SearchMeterSend[CurIndex++] = 0x05;
        SearchMeterSend[CurIndex++] = 0x01;
        SearchMeterSend[CurIndex++] = 0x01;
        SearchMeterSend[CurIndex++] = 0x40;
        SearchMeterSend[CurIndex++] = 0x01;
        SearchMeterSend[CurIndex++] = 0x02;
        SearchMeterSend[CurIndex++] = 0x00;
        SearchMeterSend[CurIndex++] = 0x00;
        /* 校验 */
        oopcs = CRC16_Get((const uint8 *)&SearchMeterSend[1+OOP_0XFE_CNT],21);
        SearchMeterSend[CurIndex++] = (uint8)(oopcs & 0x00FF);
        SearchMeterSend[CurIndex++] = (uint8)(oopcs >> 8);
        /*帧尾*/
        SearchMeterSend[CurIndex++] = 0x16;
    }
    else
    {
        SearchMeterSend[0] = 0x68;
        memcpy(&SearchMeterSend[1], MeterAddr, 6);
        SearchMeterSend[7] = 0x68;

        if(PrtlId == PRTL_97)
        {
            SearchMeterSend[8]  = 0x01;//控制码
            SearchMeterSend[9]  = 0x02;//数据长度
            SearchMeterSend[10] = (0x1f + 0x33) & 0xff;//待发送的数据
            SearchMeterSend[11] = (0x90 + 0x33) & 0xff;//(当前)正向有功电能数据块
        }
        else//07
        {
            SearchMeterSend[8]  = 0x11;//主站请求读数据
            SearchMeterSend[9]  = 0x04;
            SearchMeterSend[10] = (0x00 + 0x33) & 0xff;
            SearchMeterSend[11] = (0xff + 0x33) & 0xff;
            SearchMeterSend[12] = (0x01 + 0x33) & 0xff;
            SearchMeterSend[13] = (0x00 + 0x33) & 0xff;//(当前)正向有功电能数据块
        }

        CurIndex = SearchMeterSend[9] + 10;//数据长度加上前面10位长度等于校验位的位置

        for(i = 0; i < CurIndex; i++)
        {
            CheckSum += SearchMeterSend[i];
        }
        SearchMeterSend[CurIndex] = (uint8)CheckSum;
        SearchMeterSend[CurIndex + 1] = 0x16;

        CurIndex += 2;
    }

    return CurIndex;
}
int8 checkCS(uint8 *pBuf, uint16 offset, uint8 len, uint8 CS)
{
    uint8   i;
    uint8   sum = 0;

    for(i = 0; i < len; i++)
    {
        sum += pBuf[(offset + i)];
    }

    if(sum == CS)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

uint16 SearchFrameCheck(uint8     pipe, uint8 *pRecv, uint16 revlen,uint8 *pAppRecv)
{
    
   
    uint16  Len = 0;             //规约帧数据域的规约帧长度
   
    uint16  offset = 0;             //临时读指针偏移
    uint8   ret;
    uint8   CS = 0;

    uint8   frameCnt = 0;
    uint16  validLen = 0;


    if(revlen <= 4)
    {
        return 0;
    }
    while(offset < revlen)
    {
        /*1、帧头检验：0x68-*/
        if(0x68 != pRecv[offset])
        {
            offset++;
            continue;
        }
        /*2、预判断长度*/
        if((revlen - offset) < MIN_PRTL_LEN)
        {
            offset++;
            continue;
        }
        /*3、第2  个68   判断*/
        if(0x68 != pRecv[offset + 7])
        {
            offset++;
            continue;
        }

        /*4、取帧数据域长度*/

        Len = pRecv[(offset + 9)] + MIN_PRTL_LEN;
        
        /*5、再次判断长度*/
        if(Len > (revlen - offset))
        {
            offset++;
            continue;
        }

        /*6、检查校验和CS*/
        CS = pRecv[(offset + Len - 2)];

        ret = checkCS(pRecv, offset, Len - 2, CS);

        if(ret == FALSE)
        {
            offset++;
            continue;
        }

        /*7、检查帧尾16*/
        if(0x16 != pRecv[(offset + (Len - 1))])
        {
            offset++;
            continue;
        }
        memcpy(pAppRecv, &pRecv[offset], Len);
        validLen = Len;
        offset+=Len;
        frameCnt++;
    }
    TASK_FMT_DEBUG(pipe, REC_LOG,"Len %d framecnt %d revlen %d validlen %d \n",Len,frameCnt,revlen,validLen);

    if(Len > 0)
    {
        /* 搜表结果中，搜到两帧及以上的报文，或者含有乱码，则搜第二轮 */
        if((frameCnt > 1) || ((revlen -validLen) > 4))
        {
            return R_FRAME_ERROR;
        }
        else
        {
            return Len ;
        }
    }
    else
    {
        return R_FRAME_ERROR;
    }
}


/*******************************************************************************
* 函数名称: CommFrameCheck3761_CIRCULAR
* 函数功能: 从DMA循环接收缓存区中找到3761报文，并拷贝到另一缓存区
* 输入参数: cch_id 通道号
* 输出参数: RecvPtr 输出缓存区结构体指针
* 返 回 值: 报文长度
*******************************************************************************/
uint16 SearchFrameCheck_OOP(uint8 pipe, uint8 *pRecv,uint16 revlen, uint8 *pAppRecv)
{

    uint16  Len = 0;            /* 规约帧数据域的规约帧长度 */
    uint16  offset = 0;         /* 临时读指针偏移 */
    
    uint16 Crc16=0, CrcL, CrcH;
    uint8 HeadAddrLen;
    
    //如果能识别出完整的两个698报文 则也认为是冲突 如果识别出一个完整的698后还有多余的报文 也认为是冲突
    uint8   frameCnt = 0;
    uint16  validLen = 0;

    //现场台体测试时会出现回复三条一样的报文黏在一起的 需要特殊处理下
    uint8  addrList[10][6]={0};
    uint8 i,flag = 0;
    /* 如果数据长度小于最小数据帧长度则视为一帧数据还没有写入 */
    if (revlen <= 4)
    {
        return 0;
    }
    while(offset <= revlen)
    {
        /*1、帧头检验：0x68 */
        if(0x68 != pRecv[offset])
        {
            offset++;
            continue;
        }

        /* 2、取帧数据域长度-长度为2个字节，有可能在缓存的首尾各一个字节*/
        Len = ((uint16)pRecv[(offset+1)])|((uint16)pRecv[(offset+2)]<<8);
        Len += 2;

        /* 3、再次判断长度 */
        if(Len > (revlen - offset))
        {

            offset++;
            continue;

        }

        /*4、检查帧尾16 */
        if(0x16 != pRecv[offset + (Len-1)] )
        {
            offset++;
            continue;
        }


        HeadAddrLen = (pRecv[offset+4] & 0x0F)+1;
        CrcL = (uint16)pRecv[offset+6+HeadAddrLen];
        CrcH = (uint16)pRecv[offset+7+HeadAddrLen];
        Crc16 = (CrcH << 8)|(CrcL & 0x00FF);
        if(Crc16 != CRC16_Get((const uint8*)&pRecv[offset+1],5+HeadAddrLen))
        {
            offset++;
            continue;
        }

        CrcL = pRecv[offset+Len-3];
        CrcH = pRecv[offset+Len-2];
        Crc16 = ((CrcH) << 8)|((CrcL)& 0x00FF);
        if(Crc16 != CRC16_Get((const uint8*)&pRecv[offset+1], Len-4))
        {
            offset++;
            continue;
        }
        memcpy(pAppRecv,&pRecv[offset],Len);
        memcpy(addrList[frameCnt],&pAppRecv[5],6);
        frameCnt++;
        validLen += Len;
        offset+=Len;
        if(frameCnt > 10)
            break;
    }
    TASK_FMT_DEBUG(pipe, REC_LOG,"Len %d framecnt %d revlen %d validlen %d \n",Len,frameCnt,revlen,validLen);
    if(frameCnt > 0)
    {
        for(i=1;i<frameCnt;i++)
        {
            if(memcmp(addrList[0],addrList[i],6)!=0)
            {
                flag = 1;
                break;
            }
        }
        if(flag == 0)
        {
            TASK_FMT_DEBUG(pipe, REC_LOG,"收到 %d 条回复相同的地址\n",frameCnt);
            frameCnt = 1;

        }
    }
    if(Len > 0)
    {
        /* 搜表结果中，搜到两帧及以上的报文，或者含有乱码，则搜第二轮 */
        if((frameCnt > 1) || ((revlen -validLen) > 4))
        {
            return R_FRAME_ERROR;
        }
        else
        {
            return Len;
        }
    }
    else
    {
        return R_FRAME_ERROR;
    }
}

/*******************************************************************
* 函数名称: IsNeedSend
* 函数功能:
* 输入参数: 无
* 输出参数: 无
* 返 回  值  :
*******************************************************************/
uint8 IsNeedSend(uint8 *pRecvData)
{
    uint8 i =0;
    uint8 nLen =0;

    nLen = pRecvData[9];
    for(i = 0; i < nLen-3; i++)
    {
        if(pRecvData[i+14] == 0x68)
        {
            return TRUE;
        }
    }
    return FALSE;
}
//检查搜表应答帧的地址是否和发出的缩位寻址的地址相同
uint8 CheckRspMeterAddr(uint8 *pRecvData,uint8 *SndMtrAdd)
{
    if(memcmp(&SndMtrAdd[0], &pRecvData[0], 1) == 0)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/*******************************************************************************
* 函数名称: taskmng_metersearch_task_deal_485
* 函数功能: 搜表任务的执行逻辑 485
* 输入参数: 无
* 输出参数: 无
* 返 回 值: >0                  成功
*           <0                  失败
*******************************************************************************/

int taskmng_metersearch_task_deal_485(uint8 pipe, FUJIAN_TASK_RUN_T * runTask)
{
    uint8 i;

    uint8 FrameLen = 0;

    uint8 tmpSendData[50] = {0};
    
    uint8 RecvFlag=0;
    //uint8 RecvFinishFlag = FALSE;               //接收完成标志
    uint8 cmpAddr[6], ret;
    uint8 revbuf[1024];
    uint16 revlen=0;
    uint16 reallen=0;
    uint8 AppRecv[1024];
    time_t timenow;
    NOMAL_OAD_T NormalOad;
    
    AmrThreadArg_T  *pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    OOP_SEARCHMET_RES_T  SearchResult;
    memset(&SearchResult,0x00,sizeof(OOP_SEARCHMET_RES_T));
    if(pAmrThreadArg->TotalState.NormalAmr == RTS_IDLE)
    {
        taskmng_pipe_collection_state_write(pipe,TASK_METERSEARCH,runTask->taskid,runTask->priority);
        pAmrThreadArg->SearchMeterState = SS_INIT_SEARCH;
        pAmrThreadArg->TotalState.NormalAmr = RTS_SEND;
    }
    OOP_COMDCB_T comdcb;
    comdcb.parity = 2;
    comdcb.databits = 8;
    comdcb.stopbits = 1;
    if(pAmrThreadArg->SearchMeter.OopBaudRate==9600)
        comdcb.baud = 6;
    else
        comdcb.baud = 3;
    //进入搜表状态
    switch(pAmrThreadArg->SearchMeterState)
    {

    case SS_INIT_SEARCH:
        memset(&pAmrThreadArg->Rs485Smet, 0, sizeof(SerchStruct));
        pAmrThreadArg->Rs485Smet.SOrder = Order_One;
        pAmrThreadArg->SearchMeterState = SS_READY_SEARCH;
        //pAmrThreadArg->searchRetryCnt= 0;
        //pAmrThreadArg->SecSendFlag = 0;
        //pAmrThreadArg->SerchNumRec = 0;
        pAmrThreadArg->CurPrtlMode = PRTL_OOP;
        pAmrThreadArg->SearchMeter.OopBaudRate =9600;
        //测试用
        //pAmrThreadArg->Rs485Smet.S_One.SerchNum = 90;
        break;
    //进行缩位寻址搜表准备
    case SS_READY_SEARCH:
        //如果表档案已经填满，不进行搜表

        memset(pAmrThreadArg->SearchMeterFile.MeterAddr,0xaa,6);
        pAmrThreadArg->SearchMeterFile.PrtlId = pAmrThreadArg->CurPrtlMode;
        //第一轮进行07低一字节表搜索，第二轮是07低二字节，第三轮97低一字节，第四轮97低二字节
        // 1、刷新搜表轮次
        if(pAmrThreadArg->Rs485Smet.SOrder==Order_One)
        {
            pAmrThreadArg->SearchMeterFile.MeterAddr[0]=BintoBCD(pAmrThreadArg->Rs485Smet.S_One.SerchNum);
        }
        else
        {
            pAmrThreadArg->SearchMeterFile.MeterAddr[0] = pAmrThreadArg->Rs485Smet.S_One.MetClash[pAmrThreadArg->Rs485Smet.S_Two.DoClashNum];
            pAmrThreadArg->SearchMeterFile.MeterAddr[1] = BintoBCD(pAmrThreadArg->Rs485Smet.S_Two.SerchNum);
        }
        // 4、组搜表帧
        FrameLen = SearchMetetPackMeterPrtl(&tmpSendData[0], pAmrThreadArg->SearchMeterFile.MeterAddr, pAmrThreadArg->SearchMeterFile.PrtlId,pipe);
//
//        pAmrThreadArg->SearchMeterState = SS_SEND_SEARCH;
//        break;
//
//    case SS_SEND_SEARCH:

        if(fujian_rs485_init( pipe,&comdcb) <0 )
        {
            TASK_FMT_DEBUG(pipe, REC_LOG,"串口初始化失败 \n");
            return -1;
        }

        ret=fujian_rs485_send(pipe,2,tmpSendData, FrameLen);
        if(ret<0)
        {
             return -1;
        }
        TASK_FMT_DEBUG(pipe, REC_LOG,"发送搜表报文 \n");
        TASK_BUF_DEBUG(pipe, REC_LOG,tmpSendData, FrameLen);
        pAmrThreadArg->SearchMeterState = SS_RECV_SEARCH;

        break;

    //接收搜表返回数据帧
    case SS_RECV_SEARCH:

        //revlen = rs485_RS232_recv(&Rs485ComNo[pipe-PLC_NUM],revbuf, sizeof(revbuf), 100, 10, 0xFF);
        revlen = fujian_rs485_recv(pipe,2, &comdcb, revbuf, sizeof(revbuf));
        if(revlen>0)
        {
            TASK_FMT_DEBUG(pipe, REC_LOG,"收到搜表回码 SOrder %d prtl %d\n",pAmrThreadArg->Rs485Smet.SOrder,
                    pAmrThreadArg->SearchMeterFile.PrtlId);
            TASK_BUF_DEBUG(pipe, REC_LOG,revbuf, revlen);
        }
        if (pAmrThreadArg->Rs485Smet.SOrder == Order_One)
        {
            if(pAmrThreadArg->SearchMeterFile.PrtlId == PRTL_OOP)
            {
                reallen = SearchFrameCheck_OOP(pipe, revbuf, revlen,AppRecv);
            }
            else
            {
                reallen = SearchFrameCheck(pipe, revbuf, revlen,AppRecv);
            }
        }
        else
        {
            if(pAmrThreadArg->SearchMeterFile.PrtlId == PRTL_OOP)
            {
                reallen = SearchFrameCheck_OOP(pipe, revbuf, revlen,AppRecv);
            }
            else
            {
                reallen = SearchFrameCheck(pipe, revbuf, revlen,AppRecv);
            }
        }

        //无回码
        if(reallen == 0)
        {
            RecvFlag = 0;
        }
        else if(reallen == R_FRAME_ERROR)
        {
            RecvFlag = 0xEE;
        }
        else
        {
            //正常回码0x16,无回码0x00
            RecvFlag = 0x16;
        }
        TASK_FMT_DEBUG(pipe, REC_LOG,"RecvFlag %02x \n",RecvFlag);

        //正确回码
        //将地址解析出来
        //检验表地址
        if(0x16==RecvFlag)
        {
//            if(pAmrThreadArg->SearchMeterFile.PrtlId != PRTL_OOP)
//            {
//                if(IsNeedSend(&AppRecv[0]))
//                {
//                    pAmrThreadArg->SecSendFlag= 1;
//                    if(pAmrThreadArg->Rs485Smet.SOrder==Order_One)
//                    {
//                        if(pAmrThreadArg->Rs485Smet.S_One.SerchNum == 99)
//                        {
//                            pAmrThreadArg->SerchNumRec= 0;
//                        }
//                        else
//                        {
//                            pAmrThreadArg->SerchNumRec= pAmrThreadArg->Rs485Smet.S_One.SerchNum+1;
//                        }
//                    }
//                    else
//                    {
//                        if(pAmrThreadArg->Rs485Smet.S_Two.SerchNum == 99)
//                        {
//                            pAmrThreadArg->SerchNumRec = 0;
//                        }
//                        else
//                        {
//                            pAmrThreadArg->SerchNumRec = pAmrThreadArg->Rs485Smet.S_Two.SerchNum+1;
//                        }
//                    }
//                }
//            }

            if(pAmrThreadArg->SearchMeterFile.PrtlId == PRTL_OOP)
            {
                memcpy(cmpAddr, &(AppRecv[OOP_ADDRINDEX]), 6);
            }
            else
            {
                memcpy(cmpAddr, &(AppRecv[ACQ_ADDR_INDEX]), 6);
            }
            TASK_FMT_DEBUG(pipe, REC_LOG,"搜到的地址 %02x%02x%02x%02x%02x%02x \n",cmpAddr[5],cmpAddr[4],cmpAddr[3],
                        cmpAddr[2],cmpAddr[1],cmpAddr[0]);
            timenow = time(NULL);
            SearchResult.serchTime = Time_tToDataTimeS(timenow);
            SearchResult.proto = pAmrThreadArg->SearchMeterFile.PrtlId;
            memcpy_r(SearchResult.commAdd.add,cmpAddr,6);
            SearchResult.commAdd.len = 5;
            TASK_FMT_DEBUG(pipe, REC_LOG,"发送的地址 %02x%02x%02x%02x%02x%02x \n",pAmrThreadArg->SearchMeterFile.MeterAddr[5],pAmrThreadArg->SearchMeterFile.MeterAddr[4],pAmrThreadArg->SearchMeterFile.MeterAddr[3],
                        pAmrThreadArg->SearchMeterFile.MeterAddr[2],pAmrThreadArg->SearchMeterFile.MeterAddr[1],pAmrThreadArg->SearchMeterFile.MeterAddr[0]);
            if(CheckRspMeterAddr(cmpAddr, pAmrThreadArg->SearchMeterFile.MeterAddr) == TRUE)
            {
                if(fujian_metersearch_result_check(pipe,SearchResult,&NormalOad.infoNum)==TRUE)
                {
                    //RecvFinishFlag=TRUE;      
                    NormalOad.logicId = 0;
                    NormalOad.oad.value = 0x60020200;
                    NormalOad.classtag = 3;
                    ret = db_write_nomal(pAmrThreadArg->DBClientHandle,&NormalOad,(uint8*)&SearchResult,sizeof(OOP_SEARCHMET_RES_T));
                    if(ret ==0)
                    {
                        TASK_FMT_DEBUG( pipe, REC_LOG, "搜表结果写入成功 index %d \n",NormalOad.infoNum);
                        TASK_FMT_DEBUG( pipe, REC_LOG, "搜表地址 0x%02x%02x%02x%02x%02x%02x \n",cmpAddr[5],cmpAddr[4],cmpAddr[3],cmpAddr[2],cmpAddr[1],cmpAddr[0]);
                    }
                }
            }
            if(pAmrThreadArg->CurPrtlMode==PRTL_97)
            {
                usleep(200000);
            }
            else
            {
                usleep(120000);
            }

            pAmrThreadArg->SearchMeterState = SS_READY_SEARCH;
            //pAmrThreadArg->Rs485Smet.S_One.SerchNum++;
            //pAmrThreadArg->searchRetryCnt= 0;
            if (pAmrThreadArg->Rs485Smet.SOrder == Order_One)
            {
                pAmrThreadArg->Rs485Smet.S_One.SerchNum++;
            }else
            {
                pAmrThreadArg->Rs485Smet.S_Two.SerchNum++;
            }
        }
        else
        {
            //冲突处理
            if(RecvFlag==0xEE)
            {
                //收到报文，但是是乱码，应该是缩位寻址
                //电表地址有重复的，需要进行地址的保存
                //进行第2轮缩位寻址
               
                if ((pAmrThreadArg->Rs485Smet.SOrder == Order_One) && (pAmrThreadArg->Rs485Smet.S_One.ClashMetNum < MAX_SEARCH_METER_CNT))
                {
                    for(i = 0; i < pAmrThreadArg->Rs485Smet.S_One.ClashMetNum; i++)
                    {
                        if(pAmrThreadArg->Rs485Smet.S_One.MetClash[i] == BintoBCD(pAmrThreadArg->Rs485Smet.S_One.SerchNum))
                        {
                            break;
                        }
                    }

                    if(i >= pAmrThreadArg->Rs485Smet.S_One.ClashMetNum)
                    {
                        pAmrThreadArg->Rs485Smet.S_One.MetClash[pAmrThreadArg->Rs485Smet.S_One.ClashMetNum++] = BintoBCD(pAmrThreadArg->Rs485Smet.S_One.SerchNum);
                        TASK_FMT_DEBUG( pipe, REC_LOG, "有冲突的搜表次数 %d 冲突搜表字节为 %02x\n",pAmrThreadArg->Rs485Smet.S_One.ClashMetNum,
                                pAmrThreadArg->Rs485Smet.S_One.MetClash[pAmrThreadArg->Rs485Smet.S_One.ClashMetNum-1]);
                    }
                }
            }
            pAmrThreadArg->SearchMeterState = SS_READY_SEARCH;
            if (pAmrThreadArg->Rs485Smet.SOrder == Order_One)
            {
                pAmrThreadArg->Rs485Smet.S_One.SerchNum++;
            }else
            {
                pAmrThreadArg->Rs485Smet.S_Two.SerchNum++;
            }
            
//            if (RecvFinishFlag == TRUE)
//            {
//                //切换到重试状态
//                pAmrThreadArg->SearchMeterState = SS_READY_SEARCH;
//
//                if (pAmrThreadArg->Rs485Smet.SOrder == Order_One)
//                {
//
//                    if((pAmrThreadArg->searchRetryCnt>= 2) && (pAmrThreadArg->Rs485Smet.S_One.ClashMetNum < MAX_SEARCH_METER_CNT))
//                    {
//                        for(i = 0; i < pAmrThreadArg->Rs485Smet.S_One.ClashMetNum; i++)
//                        {
//                            if(pAmrThreadArg->Rs485Smet.S_One.MetClash[i] == BintoBCD(pAmrThreadArg->Rs485Smet.S_One.SerchNum))
//                            {
//                                break;
//                            }
//                        }
//
//                        if(i >= pAmrThreadArg->Rs485Smet.S_One.ClashMetNum)
//                        {
//                            pAmrThreadArg->Rs485Smet.S_One.MetClash[pAmrThreadArg->Rs485Smet.S_One.ClashMetNum++] = BintoBCD(pAmrThreadArg->Rs485Smet.S_One.SerchNum);
//                        }
//                    }
//                    if((pAmrThreadArg->SecSendFlag == 1) && (pAmrThreadArg->SerchNumRec == pAmrThreadArg->Rs485Smet.S_One.SerchNum))
//                    {
//                        pAmrThreadArg->SecSendFlag = 0;
//                        pAmrThreadArg->SerchNumRec = 0;
//                    }
//                    else
//                    {
//                        pAmrThreadArg->Rs485Smet.S_One.SerchNum++;
//                        pAmrThreadArg->searchRetryCnt= 0;
//                    }
//                    
//                }
//                else
//                {
//
//                    if((pAmrThreadArg->SecSendFlag == 1) && (pAmrThreadArg->SerchNumRec == pAmrThreadArg->Rs485Smet.S_Two.SerchNum))
//                    {
//                        pAmrThreadArg->SecSendFlag = 0;
//                        pAmrThreadArg->SerchNumRec = 0;
//                    }
//                    else
//                    {
//                        pAmrThreadArg->Rs485Smet.S_Two.SerchNum++;
//                        pAmrThreadArg->searchRetryCnt= 0;
//                    }
//                    
//                }
//            }
        }

        if (pAmrThreadArg->Rs485Smet.SOrder == Order_One)
        {
            //第一轮搜索结束，进入第二轮搜索
            if (pAmrThreadArg->Rs485Smet.S_One.SerchNum > NUM_99)
            {
                //如果第一轮捞表没有搜索到任何表，则没有必要在进入第二轮
                if (pAmrThreadArg->Rs485Smet.S_One.ClashMetNum == 0)
                {

                    if((pAmrThreadArg->CurPrtlMode== PRTL_OOP) && (pAmrThreadArg->SearchMeter.OopBaudRate == 9600))
//                    {
//                        //切换到面向对象2400波特率搜索
//                        TASK_FMT_DEBUG( pipe, REC_LOG, "第一轮切换到698规约 2400波特率 \n");
//                        pAmrThreadArg->CurPrtlMode = PRTL_OOP;
//                        pAmrThreadArg->SearchMeter.OopBaudRate = 2400;
//                        pAmrThreadArg->Rs485Smet.S_One.SerchNum = 0;
//                        pAmrThreadArg->SearchMeterState = SS_READY_SEARCH;
//                    }
//                    else if((pAmrThreadArg->CurPrtlMode == PRTL_OOP) && (pAmrThreadArg->SearchMeter.OopBaudRate == 2400))
                    {
                        TASK_FMT_DEBUG( pipe, REC_LOG, "第一轮切换到07规约 2400波特率 \n");
                        pAmrThreadArg->CurPrtlMode = PRTL_07;
                        pAmrThreadArg->SearchMeter.OopBaudRate = 2400;
                        pAmrThreadArg->Rs485Smet.S_One.SerchNum = 0;
                        pAmrThreadArg->SearchMeterState = SS_READY_SEARCH;
                    }
                    else
                    {
                       pAmrThreadArg->SearchMeterState = SS_OVER;
                    }
                }
                else
                {
                    //为进入第二轮搜索做准备
                    TASK_FMT_DEBUG( pipe, REC_LOG, "有冲突 进行第二轮搜表 \n");
                    pAmrThreadArg->Rs485Smet.SOrder = Order_Two;
                }
            }
        }
        else
        {
            if (pAmrThreadArg->Rs485Smet.S_Two.SerchNum > NUM_99)
            {
                pAmrThreadArg->Rs485Smet.S_Two.SerchNum = 0;
                pAmrThreadArg->Rs485Smet.S_Two.DoClashNum++;
                TASK_FMT_DEBUG( pipe, REC_LOG, "处理完第 %d 个冲突 总共 %d\n",pAmrThreadArg->Rs485Smet.S_Two.DoClashNum,    
                        pAmrThreadArg->Rs485Smet.S_One.ClashMetNum);
            }

            if (pAmrThreadArg->Rs485Smet.S_Two.DoClashNum >= pAmrThreadArg->Rs485Smet.S_One.ClashMetNum)
            {

                if((pAmrThreadArg->CurPrtlMode == PRTL_OOP) && (pAmrThreadArg->SearchMeter.OopBaudRate == 9600))
//                {
//                    //切换到面向对象2400波特率搜索
//                    TASK_FMT_DEBUG( pipe, REC_LOG, "第二轮切换到698规约 2400波特率 \n");
//                    pAmrThreadArg->CurPrtlMode = PRTL_OOP;
//                    pAmrThreadArg->SearchMeter.OopBaudRate = 2400;
//                    pAmrThreadArg->Rs485Smet.S_Two.SerchNum = 0;
//                    pAmrThreadArg->SearchMeterState = SS_READY_SEARCH;
//                }
//                else if((pAmrThreadArg->CurPrtlMode == PRTL_OOP) && (pAmrThreadArg->SearchMeter.OopBaudRate == 2400))
                {
                    TASK_FMT_DEBUG( pipe, REC_LOG, "第二轮切换到07规约 2400波特率 \n");
                    pAmrThreadArg->CurPrtlMode = PRTL_07;
                    pAmrThreadArg->SearchMeter.OopBaudRate = 2400;
                    pAmrThreadArg->Rs485Smet.S_Two.SerchNum = 0;
                    pAmrThreadArg->SearchMeterState = SS_READY_SEARCH;
                }
//                else if(pAmrThreadArg->CurPrtlMode == PRTL_07)
//                {
//                    //切换到97搜索
//                    pAmrThreadArg->CurPrtlMode= PRTL_97;
//                    pAmrThreadArg->SearchMeter.OopBaudRate = 1200;
//                    pAmrThreadArg->SearchMeterState = SS_INIT_SEARCH;//如果是97则切换为搜索准备
//                }
                else
                {
                    pAmrThreadArg->SearchMeterState = SS_OVER;
                }
            }
        }
        break;

    case SS_OVER://搜表结束

        memset(&pAmrThreadArg->SearchMeter.MeterCheckIndex, 0x00, sizeof(SEARCH_METER_T));
        //置回搜表状态机初始状态
        pAmrThreadArg->SearchMeterState = SS_READY_CHECK;
        runTask->result = SCHEME_FINISHED;
        TASK_FMT_DEBUG(pipe, REC_LOG,"485搜表结束 \n");

        return -1;

    default:
        break;
    }

    return 0;
}

/*******************************************************************************
* 函数名称: comm_log_read 
* 函数功能:给通信日志上报方案用 每次从文件中读1k数据
* 输入参数:file 文件名字 commlog_22033119.tar.gz 文件由taskManager生成 puamr从文件读数据上报
        
* 输出参数: outbuf 读出的数据
            buflen 读出的数据长度
* 返 回 值: 1 文件读完了
            0 正常读到数据
            -1 失败
*******************************************************************************/

int comm_log_read(char *file,char *outbuf,uint16 *buflen)
{
    FILE *fp;
    static int index = 0;
    static char lastfile[100]={0};
    char bufread[2000]={0};
    int readlen;
    if(NULL == file || NULL == outbuf || NULL == buflen)
        return -1;
    if(strcmp(lastfile,file)!=0)
     {
        index = 0;
        strcpy(lastfile,file);
    }
    char filepath[200]={0};
    
    sprintf(filepath,"/data/app/record/%s",file);
    fp = fopen(filepath,"rb");
    if(NULL == fp)
    {
        return -1;
    }
    fseek(fp,index*1024,SEEK_SET);
    readlen = fread(bufread,1,1024,fp);
    index++;
    memcpy(outbuf,bufread,readlen);
    *buflen = readlen;

    if(feof(fp) !=0)
    {
        fclose(fp);
        index = 0;
        return 1;
    }
    fclose(fp);
    return 0;
}

/*******************************************************************************
* 函数名称: taskmng_smart_proxy_msg_unpack_trans_command_request 
* 函数功能: 代理智能透明转发命令请求 
* 输入参数: pipe 端口号
        buf 消息内容
        len  长度
* 输出参数:  
* 返 回 值:  TRUE 解析成功
    FALSE 解析失败
*******************************************************************************/
BOOL taskmng_smart_proxy_msg_unpack_trans_command_request(uint8 pipe,uint8* buf,uint32 len)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    uint16 offset = 0;
    uint8 lenoffset;
    uint8 Addr[6];


    memcpy_r(&pAmrThreadArg->RelayTrans.OadPort,buf+offset,4);
    offset+=4;       //端口oad

    //优先级
    pAmrThreadArg->RelayTrans.priority = buf[offset++];;
    //端口波特率
    pAmrThreadArg->RelayTrans.comdcb.baud = buf[offset++];
    pAmrThreadArg->RelayTrans.comdcb.parity = buf[offset++];
    pAmrThreadArg->RelayTrans.comdcb.databits = buf[offset++];
    pAmrThreadArg->RelayTrans.comdcb.stopbits = buf[offset++];
    pAmrThreadArg->RelayTrans.comdcb.flowctrl = buf[offset++];
    memcpy_r((uint8*)&pAmrThreadArg->RelayTrans.OverTime,&buf[offset],2);
    offset+=2;
    memcpy_r((uint8*)&pAmrThreadArg->RelayTrans.ByteOverTime,&buf[offset],2);
    offset+=2;
    //通信地址
    offset++;
    pAmrThreadArg->RelayTrans.tsa.af = buf[offset++];
    memcpy(pAmrThreadArg->RelayTrans.tsa.add,&buf[offset],pAmrThreadArg->RelayTrans.tsa.len+1);
    memcpy_r(Addr,pAmrThreadArg->RelayTrans.tsa.add,pAmrThreadArg->RelayTrans.tsa.len+1);
    offset += (pAmrThreadArg->RelayTrans.tsa.len+1);
    //通信对象
    pAmrThreadArg->RelayTrans.objiecttype = buf[offset++];
    //下行报文格式字
    pAmrThreadArg->RelayTrans.reqframeformat = buf[offset++];
    //上行报文格式字
    pAmrThreadArg->RelayTrans.resframeformat = buf[offset++];
    //透明转发内容
    pAmrThreadArg->RelayTrans.RelayLen = XADR_GetEleNum(&buf[offset],&lenoffset);
    offset+=lenoffset;
    memcpy(pAmrThreadArg->RelayTrans.RelayData,&buf[offset],pAmrThreadArg->RelayTrans.RelayLen);
    memcpy(pAmrThreadArg->RelayTrans.METER_LIST[0].setaction,pAmrThreadArg->RelayTrans.RelayData,pAmrThreadArg->RelayTrans.RelayLen);
    pAmrThreadArg->RelayTrans.METER_LIST[0].setlen = pAmrThreadArg->RelayTrans.RelayLen;
    
    TASK_FMT_DEBUG( pipe, RELAY_LOG, "智能代理透明转发电表地址 0x%02x%02x%02x%02x%02x%02x\n",Addr[5],Addr[4],Addr[3],Addr[2],Addr[1],Addr[0]);
    memcpy(pAmrThreadArg->RelayTrans.MtrAddr,Addr,6);
    
    return TRUE;
    
}

void fujian_relay_trans_485_comdcb_get(uint8 pipe,OOP_COMDCB_T *outcomdcb)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    uint8 MsgIndex;
    OOP_TSA_T tsa;

    OOP_COMDCB_T comdcb;
    memset(&comdcb,0x00,sizeof(OOP_COMDCB_T));
    comdcb.parity = 2;
    comdcb.databits = 8;
    comdcb.stopbits = 1;
    
    switch(pAmrThreadArg->RelayTrans.IOP)
    {
        case 0x0010:
        {
            comdcb.baud=pAmrThreadArg->RelayTrans.comdcb.baud;
        }
        break;
        case 0x0011:
        {
            switch(pAmrThreadArg->RelayTrans.proxychoice)
            {
                case 0x01:
                {
                    MsgIndex=pAmrThreadArg->RelayTrans.AmrIndex;
                    comdcb.baud=taskmng_baud_get_from_tsa(pAmrThreadArg->RelayTrans.AmrNormal[MsgIndex].Mtraddr,LOGIC_ID_YC);
                }
                break;
                case 0x02:
                {
                    memcpy_r(tsa.add,pAmrThreadArg->RelayTrans.MtrAddr,6);
                    comdcb.baud=taskmng_baud_get_from_tsa(tsa,LOGIC_ID_YC);  
                }
                break;
                case 0x03:
                {
                    MsgIndex=pAmrThreadArg->RelayTrans.AmrIndex;
                    comdcb.baud=taskmng_baud_get_from_tsa(pAmrThreadArg->RelayTrans.METER_LIST[MsgIndex].Mtraddr,LOGIC_ID_YC);  
      

                }
                break;
                case 0x07:
                case 50:
                {
                    comdcb=pAmrThreadArg->RelayTrans.comdcb;
                }
                break;
                default:
                break;
            }
        }
        break;

    }

    memcpy(outcomdcb,&comdcb,sizeof(OOP_COMDCB_T));
    
}

/****************************************************************************
*函数名称：fujian_relay_trans_485_init
*功能：执行代理和透传前初始化抄表通道;
*输入参数：
*输出参数：
*返回：
****************************************************************************/

int fujian_relay_trans_485_init(uint8 pipe)
{
    int ret;
    OOP_COMDCB_T comdcb;
    memset(&comdcb,0x00,sizeof(OOP_COMDCB_T));
    fujian_relay_trans_485_comdcb_get(pipe,&comdcb);

    ret = fujian_rs485_init(pipe,&comdcb);
    if(ret<0)
    {
        TASK_FMT_TRACE( pipe, RELAY_LOG, "485初始化失败\n");
        return -1;
    }
    return 0;

}

/*******************************************************************************
* 函数名称: fujian_relay_trans_send_485
* 函数功能: 执行透传和代理的抄表发送
* 输入参数: 
* 输出参数:
* 返 回 值:
*           
*******************************************************************************/

int fujian_relay_trans_send_485(uint8 pipe)
{
    AmrThreadArg_T * pAmrThreadArg;

    pAmrThreadArg = &gAmrThreadArg[pipe];

    uint8           prtl = 0;

    int             ret;

    switch(pAmrThreadArg->RelayTrans.IOP)
    {
        case 0x0010: //请求代理读取若干个服务器的若干个对象属性
            {

                uint16          DataLen = 0;

                //通过从节点监控下发
                DataLen = pAmrThreadArg->RelayTrans.RelayLen;

                TASK_FMT_TRACE(pipe, RELAY_LOG, "透传组帧 DataLen is %d\n", DataLen);

                TASK_BUF_TRACE(pipe, RELAY_LOG, pAmrThreadArg->RelayTrans.RelayData, DataLen);

                if(DataLen > 0)
                {

                    ret = fujian_rs485_send(pipe, 3,pAmrThreadArg->RelayTrans.RelayData,
                         pAmrThreadArg->RelayTrans.RelayLen);

                    if(ret < 0)
                    {
                        return - 1;
                    }
                }

            }
            break;

        case 0x0011:
            {
                switch(pAmrThreadArg->RelayTrans.proxychoice)
                {
                    case 0x01: //请求代理读取若干个服务器的若干个对象属性
                        {
                            DI_INFO_T       DiInfo;
                            uint8           DataBuff[1024] =
                            {
                                0
                            };
                            uint16          DataLen;

                            if(GetNext485RelayDI(pipe, &DiInfo, &prtl) > 0)
                            {
                                //通过从节点监控下发
                                DataLen = MakeRelay485Data_0010(pipe, DiInfo, prtl, DataBuff);

                                TASK_FMT_TRACE(pipe, RELAY_LOG, "0011_01透传组帧 DataLen is %d\n", DataLen);

                                TASK_BUF_TRACE(pipe, RELAY_LOG, DataBuff, DataLen);

                                if(DataLen > 0)
                                {

                                    ret = fujian_rs485_send(pipe,3, DataBuff, DataLen);

                                    if(ret < 0)
                                    {
                                        return - 1;
                                    }
                                }
                            }
                            else
                            {
                                return 0;
                            }
                        }
                        break;

                    case 0x02:
                        {

                            TASK_FMT_TRACE(pipe, RELAY_LOG, "pAmrThreadArg->RelayTrans.RelayLen is %d\n",
                                 pAmrThreadArg->RelayTrans.RelayLen);

                            if(pAmrThreadArg->RelayTrans.RelayLen > 0)
                            {

                                ret = fujian_rs485_send(pipe,3, pAmrThreadArg->RelayTrans.RelayData,
                                     pAmrThreadArg->RelayTrans.RelayLen);
                                TASK_BUF_TRACE(pipe, RELAY_LOG, pAmrThreadArg->RelayTrans.RelayData,
                                     pAmrThreadArg->RelayTrans.RelayLen);

                                if(ret < 0)
                                {
                                    return - 1;
                                }
                            }
                        }
                        break;

                    case 0x03:
                    case 0x04:
                    case 0x05:
                    case 0x06:
                        {

                            TASK_FMT_TRACE(pipe, RELAY_LOG, "pAmrThreadArg->RelayTrans.RelayLen is %d,pAmrThreadArg->RelayTrans.proxychoice is %d\n",
                                 pAmrThreadArg->RelayTrans.RelayLen, pAmrThreadArg->RelayTrans.proxychoice);
                            ret = GetNext485SetRelayData(pipe);

                            if(ret < 0)
                            {
                                return - 1;
                            }
                        }
                        break;

                    case 0x07:
                        {

                            uint16          DataLen = 0;
                            OOP_OAD_U       oad;
                            uint8           tmppipe;

                            //通过从节点监控下发
                            oad.value = pAmrThreadArg->RelayTrans.OadPort;
                            DataLen = pAmrThreadArg->RelayTrans.METER_LIST[0].setlen;
                            tmppipe = taskmng_mtr_pipe_get(&oad);

                            if(tmppipe != pipe)
                            {
                                return - 1;
                            }

                            TASK_FMT_TRACE(pipe, RELAY_LOG, "透传组帧 DataLen is %d\n", DataLen);

                            TASK_BUF_TRACE(pipe, RELAY_LOG, pAmrThreadArg->RelayTrans.METER_LIST[0].setaction, DataLen);

                            if(DataLen > 0)
                            {

                                ret = fujian_rs485_send(pipe, 3,pAmrThreadArg->RelayTrans.METER_LIST[0].setaction,DataLen);

                                if(ret < 0)
                                {
                                    return - 1;
                                }
                            }
                        }
                        break;

                    case 50:
                        {
                            FRAME_CONVERT_T convert;
                            uint8 dstbuf[2000]={0};

                            memset(&convert,0x00,sizeof(FRAME_CONVERT_T));
                            convert.srcBuf = pAmrThreadArg->RelayTrans.METER_LIST[0].setaction;
                            convert.srcLen = pAmrThreadArg->RelayTrans.METER_LIST[0].setlen;
                            convert.dstBuf = dstbuf;
                            Convert_Request_Frame(pipe,&convert,pAmrThreadArg->RelayTrans.reqframeformat,&pAmrThreadArg->RelayTrans.tsa);
                            TASK_FMT_DEBUG(pipe, REC_LOG,"代理请求报文转换前内容 \n");
                            TASK_BUF_DEBUG(pipe,REC_LOG,pAmrThreadArg->RelayTrans.METER_LIST[0].setaction,pAmrThreadArg->RelayTrans.METER_LIST[0].setlen);
                            TASK_FMT_DEBUG(pipe, REC_LOG,"请求报文格式字 %d 电表地址 %02x%02x%02x%02x%02x%02x 转换后报文\n",pAmrThreadArg->RelayTrans.reqframeformat,
                                pAmrThreadArg->RelayTrans.tsa.add[0],pAmrThreadArg->RelayTrans.tsa.add[1],pAmrThreadArg->RelayTrans.tsa.add[2],
                                pAmrThreadArg->RelayTrans.tsa.add[3],pAmrThreadArg->RelayTrans.tsa.add[4],pAmrThreadArg->RelayTrans.tsa.add[5]);
                            TASK_BUF_DEBUG(pipe,REC_LOG,convert.dstBuf,convert.dstLen);

                            if(convert.dstBuf > 0)
                            {

                                ret = fujian_rs485_send(pipe,3,convert.dstBuf,convert.dstLen);

                                if(ret < 0)
                                {
                                    return - 1;
                                }
                            }
                        }
                        break;

                    default:
                        break;
                }
            }
            break;

        default:
            return - 1;
    }

    return 1;

}

/*******************************************************************************
* 函数名称: fujian_relay_trans_send_485
* 函数功能: 执行透传和代理的抄表发送
* 输入参数: 
* 输出参数:
* 返 回 值: 0 正常 
            -1 超时
*           
*******************************************************************************/

int fujian_relay_trans_recv_485(uint8 pipe)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    int RecvLen=0;
    uint8 RecvBuf[MAX_MSG_LEN]={0};
    uint8           len, lenbuf[3];
    uint8 meterindex=0;
    MeterPrtlFmt_T  MeterPrtlData;
    char           apdu[2]; 
    uint16 index=0;
    int tmprecvlen=0;
    uint16 reallen;
    int ret;
    OOP_COMDCB_T comdcb;
    memset(&comdcb,0x00,sizeof(OOP_COMDCB_T));
    fujian_relay_trans_485_comdcb_get(pipe,&comdcb);

    
    TASK_FMT_TRACE(pipe, RELAY_LOG, "开始接收透传数据\n");
    //RecvLen = rs485_RS232_recv(&Rs485ComNo[pipe - PLC_NUM], RecvBuf, sizeof(RecvBuf), rsptime, 30, 0xFF);
    RecvLen = fujian_rs485_recv( pipe,3, &comdcb, RecvBuf, sizeof(RecvBuf));
    TASK_FMT_TRACE(pipe, RELAY_LOG, "RecvLen is %d\n", RecvLen);
    meterindex = pAmrThreadArg->RelayTrans.AmrIndex;
    TASK_FMT_TRACE(pipe, RELAY_LOG, "pAmrThreadArg->RelayTrans.METER_LIST[meterindex].VliadMeter is %d\n",
         pAmrThreadArg->RelayTrans.METER_LIST[meterindex].VliadMeter);
    TASK_BUF_TRACE(pipe, RELAY_LOG, RecvBuf, RecvLen);
    
    if((RecvLen > 0) || ((pAmrThreadArg->RelayTrans.METER_LIST[meterindex].VliadMeter == 0) &&
         (pAmrThreadArg->RelayTrans.proxychoice != 0x02) && (pAmrThreadArg->RelayTrans.proxychoice != 0x07)))
    {
        switch(pAmrThreadArg->RelayTrans.IOP)
        {
            case 0x0010:
                {
                    len = set_len_offset(RecvLen, lenbuf);
                    memcpy(pAmrThreadArg->RelayTrans.RelayData, lenbuf, len);
                    pAmrThreadArg->RelayTrans.RelayLen = RecvLen + len;
                    memcpy(pAmrThreadArg->RelayTrans.RelayData + len, RecvBuf, RecvLen);
                    TASK_FMT_TRACE(pipe, RELAY_LOG, "穿透收到数据 任务结束 \n");
                    taskmng_relay_up_msg_write(pipe, 0);
                    pAmrThreadArg->RelayTrans.RelayResult = 1;
                    pAmrThreadArg->TotalState.RelayTrans = INNER_RTS_END;
    
                }
                break;
    
            case 0x0011:
                {
                    switch(pAmrThreadArg->RelayTrans.proxychoice)
                    {
                        case 0x01:
                            {
                                uint8           prtl;
    
                                taskmng_prtl_get_from_tsa(pAmrThreadArg->RelayTrans.AmrNormal[pAmrThreadArg->RelayTrans.AmrIndex].Mtraddr,
                                     LOGIC_ID_YC, &prtl);
                                ret = UnpackRelay485Data_0010(pipe, RecvBuf, RecvLen, &MeterPrtlData, apdu);
                                TASK_FMT_TRACE(pipe, RELAY_LOG, "0x0010收到回码 解帧 ret %d \n", ret);
    
                                if(ret < 0)
                                {
                                    pAmrThreadArg->TotalState.RelayTrans = INNER_RTS_RETRY; //台体测试,现有模块有可能只回复一个字节FF,这里面才尝试一次
                                    break;
                                }
    
                                if(prtl == PRTL_OOP)
                                {
                                    if(apdu[0] == 0x85 && apdu[1] == 0x01)
                                    {
    
                                        Oop485Relay_Save(pipe, MeterPrtlData.DataBuff, MeterPrtlData.DataLen);
                                    }
                                }
                                else
                                {
                                    if(ret == CHECK_RESULT_SUCCEED)
                                    {
                                        if(taskmng_645_relay_isvalid(pipe, prtl, MeterPrtlData.DataBuff, MeterPrtlData.CtrlCode, &MeterPrtlData.DataLen) >
                                             0)
                                        {
                                            TASK_FMT_TRACE(pipe, RELAY_LOG, "645验证成功 DataLen %d\n",
                                                 MeterPrtlData.DataLen);
    
                                            TASK_BUF_TRACE(pipe, RELAY_LOG, MeterPrtlData.DataBuff,
                                                 MeterPrtlData.DataLen);
    
                                            if(prtl == PRTL_97)
                                            {
                                                taskmng_645_relay_data_save(pipe, prtl, &MeterPrtlData.DataBuff[2],
                                                     MeterPrtlData.DataLen - 2);
                                            }
                                            else
                                            {
                                                taskmng_645_relay_data_save(pipe, prtl, &MeterPrtlData.DataBuff[4],
                                                     MeterPrtlData.DataLen - 4);
                                            }
                                        }
    
                                        pAmrThreadArg->RelayTrans.MapIndex++;
                                    }
                                    else
                                    {
                                        pAmrThreadArg->RelayTrans.MapIndex++;
                                    }
                                }
    
                                pAmrThreadArg->TotalState.RelayTrans = INNER_RTS_SEND;
                            }
                            break;
    
                        case 0x02:
                            {
                                AppLayerList_t * pAppLayer;
    
                                ret = LinkDataReceive(pipe, RecvBuf, RecvLen, NULL);
                                TASK_FMT_TRACE(pipe, RELAY_LOG, "LinkDataReceive 698解帧返回值 %d \n", ret);
    
                                if((ret >= eOopComm_Fail)) //解帧失败直接返回
                                {
                                    pAmrThreadArg->TotalState.RelayTrans = INNER_RTS_END;
                                    break;
                                }
    
                                //获取电表地址
                                pAppLayer = FindAppLayer(pipe, NULL);
    
                                if(pAppLayer == NULL)
                                {
                                    pAmrThreadArg->TotalState.RelayTrans = INNER_RTS_END;
                                    break;
                                }
    
                                index=pAmrThreadArg->RelayTrans.RelayLen;
    
                                pAmrThreadArg->RelayTrans.RelayData[index++] = 0x07; //tsa长度
                                pAmrThreadArg->RelayTrans.RelayData[index++] = 5; //
                                memcpy_r(&pAmrThreadArg->RelayTrans.RelayData[index], pAmrThreadArg->RelayTrans.MtrAddr,6);
                                index += 6;
    
                                memcpy(&pAmrThreadArg->RelayTrans.RelayData[index], &pAppLayer->recvApduList->apdu[3],
                                     pAppLayer->recvApduList->apduLen - 5);
                                index += (pAppLayer->recvApduList->apduLen - 5);
                                pAmrThreadArg->RelayTrans.RelayData[index++] = 0x00; //跟随上报信息域
                                pAmrThreadArg->RelayTrans.RelayData[index++] = 0x00; //时间标签
   
                                pAmrThreadArg->RelayTrans.RelayLen = index;

                                pAmrThreadArg->TotalState.RelayTrans = INNER_RTS_END;
                            }
                            break;
    
                        case 0x03:
                            {
                                AppLayerList_t * pAppLayer;
                                uint8           i;
    
                                ret = LinkDataReceive(pipe, RecvBuf, RecvLen, NULL);
                                TASK_FMT_TRACE(pipe, RELAY_LOG, "LinkDataReceive 698解帧返回值 %d \n", ret);
    
                                if((ret >= eOopComm_Fail) &&
                                     (pAmrThreadArg->RelayTrans.METER_LIST[meterindex].VliadMeter != 0)) //解帧失败直接返回
                                {
                                    pAmrThreadArg->TotalState.RelayTrans = INNER_RTS_END;
                                    break;
                                }
    
                                //获取电表地址
                                pAppLayer = FindAppLayer(pipe, NULL);
    
                                if(pAppLayer == NULL)
                                {
                                    pAmrThreadArg->TotalState.RelayTrans = INNER_RTS_END;
                                    break;
                                }
                                index=pAmrThreadArg->RelayTrans.RelayLen;
                                TASK_FMT_TRACE(pipe, RELAY_LOG, "pAmrThreadArg->RelayTrans.AmrIndex is %d,pAmrThreadArg->RelayTrans.TotalNum is %d\n",
                                     pAmrThreadArg->RelayTrans.AmrIndex, pAmrThreadArg->RelayTrans.TotalNum);
    
                                if(pAmrThreadArg->RelayTrans.AmrIndex < pAmrThreadArg->RelayTrans.TotalNum)
                                {
                                    meterindex = pAmrThreadArg->RelayTrans.AmrIndex;
                                    TASK_FMT_TRACE(pipe, RELAY_LOG, "pAmrThreadArg->RelayTrans.METER_LIST[meterindex].VliadMeter is %d\n",
                                         pAmrThreadArg->RelayTrans.METER_LIST[meterindex].VliadMeter);
    
                                    if(pAmrThreadArg->RelayTrans.METER_LIST[meterindex].VliadMeter == 0)
                                    {
                                        pAmrThreadArg->RelayTrans.RelayData[index++] = 0x07; //tsa长度
                                        pAmrThreadArg->RelayTrans.RelayData[index++] = 5; //
                                        memcpy(&pAmrThreadArg->RelayTrans.RelayData[index],
                                             pAmrThreadArg->RelayTrans.METER_LIST[meterindex].Mtraddr.add, 6);
                                        index += 6;
                                        pAmrThreadArg->RelayTrans.RelayData[index++] =
                                             pAmrThreadArg->RelayTrans.METER_LIST[meterindex].OadNum;
    
                                        for(i = 0; i < pAmrThreadArg->RelayTrans.METER_LIST[meterindex].OadNum; i++)
                                        {
                                            memcpy_r(&pAmrThreadArg->RelayTrans.RelayData[index],
                                                 &pAmrThreadArg->RelayTrans.METER_LIST[meterindex].SubOad[i],
                                                 sizeof(OOP_OAD_U));
                                            index += sizeof(OOP_OAD_U);
                                            pAmrThreadArg->RelayTrans.RelayData[index++] = 0xFF;
                                        }
                                        pAmrThreadArg->RelayTrans.AmrIndex++;
    
                                        if(pAmrThreadArg->RelayTrans.AmrIndex >= pAmrThreadArg->RelayTrans.TotalNum)
                                        {
                                        }
                                        else
                                        {
                                            pAmrThreadArg->TotalState.RelayTrans = INNER_RTS_SEND;
                                            break;
                                        }
                                    }
                                    else
                                    {
                                        pAmrThreadArg->RelayTrans.RelayData[index++] = 0x07; //tsa长度
                                        pAmrThreadArg->RelayTrans.RelayData[index++] = 5; //
                                        memcpy(&pAmrThreadArg->RelayTrans.RelayData[index],
                                             pAmrThreadArg->RelayTrans.METER_LIST[meterindex].Mtraddr.add, 6);
                                        index += 6;
    
                                        memcpy(&pAmrThreadArg->RelayTrans.RelayData[index],
                                             &pAppLayer->recvApduList->apdu[3], pAppLayer->recvApduList->apduLen - 5);
                                        index += (pAppLayer->recvApduList->apduLen - 5);
                                        pAmrThreadArg->RelayTrans.AmrIndex++;
    
                                        if(pAmrThreadArg->RelayTrans.AmrIndex >= pAmrThreadArg->RelayTrans.TotalNum)
                                        {
                                        }
                                        else
                                        {
                                            pAmrThreadArg->TotalState.RelayTrans = INNER_RTS_SEND;
                                            break;
                                        }
    
                                    }
                                }
    
                                pAmrThreadArg->RelayTrans.RelayLen = index;
                                TASK_FMT_TRACE(pipe, RELAY_LOG, "pAppLayer->recvApduList->apduLen is %d \n",
                                     pAppLayer->recvApduList->apduLen);

                                pAmrThreadArg->TotalState.RelayTrans = INNER_RTS_END;
    
                            }
                            break;
    
                        case 0x04:
                            {
                                AppLayerList_t * pAppLayer;
                                uint8           mn;
    
                                ret = LinkDataReceive(pipe, RecvBuf, RecvLen, NULL);
                                TASK_FMT_TRACE(pipe, RELAY_LOG, "LinkDataReceive 698解帧返回值 %d \n", ret);
    
                                if((ret >= eOopComm_Fail) &&
                                     (pAmrThreadArg->RelayTrans.METER_LIST[meterindex].VliadMeter != 0)) //解帧失败直接返回
                                {
                                    pAmrThreadArg->TotalState.RelayTrans = INNER_RTS_END;
                                    break;
                                }
    
                                //获取电表地址
                                pAppLayer = FindAppLayer(pipe, NULL);
    
                                if(pAppLayer == NULL)
                                {
                                    pAmrThreadArg->TotalState.RelayTrans = INNER_RTS_END;
                                    break;
                                }
                                index=pAmrThreadArg->RelayTrans.RelayLen;

    
                                TASK_FMT_TRACE(pipe, RELAY_LOG, "pAmrThreadArg->RelayTrans.AmrIndex is %d,pAmrThreadArg->RelayTrans.TotalNum is %d\n",
                                     pAmrThreadArg->RelayTrans.AmrIndex, pAmrThreadArg->RelayTrans.TotalNum);
    
                                if(pAmrThreadArg->RelayTrans.AmrIndex < pAmrThreadArg->RelayTrans.TotalNum)
                                {
                                    meterindex = pAmrThreadArg->RelayTrans.AmrIndex;
                                    TASK_FMT_TRACE(pipe, RELAY_LOG, "pAmrThreadArg->RelayTrans.METER_LIST[meterindex].VliadMeter is %d\n",
                                         pAmrThreadArg->RelayTrans.METER_LIST[meterindex].VliadMeter);
    
                                    if(pAmrThreadArg->RelayTrans.METER_LIST[meterindex].VliadMeter == 0)
                                    {
                                        pAmrThreadArg->RelayTrans.RelayData[index++] = 0x07; //tsa长度
                                        pAmrThreadArg->RelayTrans.RelayData[index++] = 5; //
                                        memcpy(&pAmrThreadArg->RelayTrans.RelayData[index],
                                             pAmrThreadArg->RelayTrans.METER_LIST[meterindex].Mtraddr.add, 6);
                                        index += 6;
                                        pAmrThreadArg->RelayTrans.RelayData[index++] =
                                             pAmrThreadArg->RelayTrans.METER_LIST[meterindex].OadNum;
    
                                        for(mn = 0; mn < pAmrThreadArg->RelayTrans.METER_LIST[meterindex].OadNum; mn++)
                                        {
    
                                            memcpy_r(pAmrThreadArg->RelayTrans.RelayData + index,
                                                 (uint8 *) &pAmrThreadArg->RelayTrans.METER_LIST[meterindex].SetOad[mn].value,
                                                 4);
                                            index = index + 4;
                                            pAmrThreadArg->RelayTrans.RelayData[index++] = 0x21;
    
                                            memcpy_r(pAmrThreadArg->RelayTrans.RelayData + index,
                                                 (uint8 *) &pAmrThreadArg->RelayTrans.METER_LIST[meterindex].GetOad[mn].value,
                                                 4);
                                            index = index + 4;
                                            pAmrThreadArg->RelayTrans.RelayData[index++] = 0x00;
                                            pAmrThreadArg->RelayTrans.RelayData[index++] = 0x21;
    
                                        }
    
                                        pAmrThreadArg->RelayTrans.AmrIndex++;
    
                                        if(pAmrThreadArg->RelayTrans.AmrIndex >= pAmrThreadArg->RelayTrans.TotalNum)
                                        {
                                        }
                                        else
                                        {
                                            pAmrThreadArg->TotalState.RelayTrans = INNER_RTS_SEND;
                                            break;
                                        }
                                    }
                                    else
                                    {
                                        pAmrThreadArg->RelayTrans.RelayData[index++] = 0x07; //tsa长度
                                        pAmrThreadArg->RelayTrans.RelayData[index++] = 5; //
                                        memcpy(&pAmrThreadArg->RelayTrans.RelayData[index],
                                             pAmrThreadArg->RelayTrans.METER_LIST[meterindex].Mtraddr.add, 6);
                                        index += 6;
    
                                        memcpy(&pAmrThreadArg->RelayTrans.RelayData[index],
                                             &pAppLayer->recvApduList->apdu[3], pAppLayer->recvApduList->apduLen - 5);
                                        index += (pAppLayer->recvApduList->apduLen - 5);
                                        pAmrThreadArg->RelayTrans.AmrIndex++;
    
                                        if(pAmrThreadArg->RelayTrans.AmrIndex >= pAmrThreadArg->RelayTrans.TotalNum)
                                        {
                                        }
                                        else
                                        {
                                            pAmrThreadArg->TotalState.RelayTrans = INNER_RTS_SEND;
                                            break;
                                        }
    
                                    }
                                }
    
                                pAmrThreadArg->RelayTrans.RelayLen = index;
                                TASK_FMT_TRACE(pipe, RELAY_LOG, "pAppLayer->recvApduList->apduLen is %d \n",
                                     pAppLayer->recvApduList->apduLen);

                                pAmrThreadArg->TotalState.RelayTrans = INNER_RTS_END;
    
                            }
                            break;
    
                        case 0x05:
                            {
                                AppLayerList_t * pAppLayer;
                                uint8           i;
    
                                ret = LinkDataReceive(pipe, RecvBuf, RecvLen, NULL);
                                TASK_FMT_TRACE(pipe, RELAY_LOG, "LinkDataReceive 698解帧返回值 %d \n", ret);
    
                                if((ret >= eOopComm_Fail) &&
                                     (pAmrThreadArg->RelayTrans.METER_LIST[meterindex].VliadMeter != 0)) //解帧失败直接返回
                                {
                                    pAmrThreadArg->TotalState.RelayTrans = INNER_RTS_END;
                                    break;
                                }
    
                                //获取电表地址
                                pAppLayer = FindAppLayer(pipe, NULL);
    
                                if(pAppLayer == NULL)
                                {
                                    pAmrThreadArg->TotalState.RelayTrans = INNER_RTS_END;
                                    break;
                                }
                                index=pAmrThreadArg->RelayTrans.RelayLen;
    
                                TASK_FMT_TRACE(pipe, RELAY_LOG, "pAmrThreadArg->RelayTrans.AmrIndex is %d,pAmrThreadArg->RelayTrans.TotalNum is %d\n",
                                     pAmrThreadArg->RelayTrans.AmrIndex, pAmrThreadArg->RelayTrans.TotalNum);
    
                                if(pAmrThreadArg->RelayTrans.AmrIndex < pAmrThreadArg->RelayTrans.TotalNum)
                                {
                                    meterindex = pAmrThreadArg->RelayTrans.AmrIndex;
                                    TASK_FMT_TRACE(pipe, RELAY_LOG, "pAmrThreadArg->RelayTrans.METER_LIST[meterindex].VliadMeter is %d\n",
                                         pAmrThreadArg->RelayTrans.METER_LIST[meterindex].VliadMeter);
    
                                    if(pAmrThreadArg->RelayTrans.METER_LIST[meterindex].VliadMeter == 0)
                                    {
                                        pAmrThreadArg->RelayTrans.RelayData[index++] = 0x07; //tsa长度
                                        pAmrThreadArg->RelayTrans.RelayData[index++] = 5; //
                                        memcpy(&pAmrThreadArg->RelayTrans.RelayData[index],
                                             pAmrThreadArg->RelayTrans.METER_LIST[meterindex].Mtraddr.add, 6);
                                        index += 6;
                                        pAmrThreadArg->RelayTrans.RelayData[index++] =
                                             pAmrThreadArg->RelayTrans.METER_LIST[meterindex].OadNum;
    
                                        for(i = 0; i < pAmrThreadArg->RelayTrans.METER_LIST[meterindex].OadNum; i++)
                                        {
                                            memcpy_r(pAmrThreadArg->RelayTrans.RelayData + index,
                                                 (uint8 *) &pAmrThreadArg->RelayTrans.METER_LIST[meterindex].SetOad[i].value,
                                                 4);
                                            index = index + 4;
                                            pAmrThreadArg->RelayTrans.RelayData[index++] = 0xFF;
                                            pAmrThreadArg->RelayTrans.RelayData[index++] = 0x00;
                                        }
   
                                        pAmrThreadArg->RelayTrans.AmrIndex++;
    
                                        if(pAmrThreadArg->RelayTrans.AmrIndex >= pAmrThreadArg->RelayTrans.TotalNum)
                                        {
                                        }
                                        else
                                        {
                                            pAmrThreadArg->TotalState.RelayTrans = INNER_RTS_SEND;
                                            break;
                                        }
                                    }
                                    else
                                    {
                                        pAmrThreadArg->RelayTrans.RelayData[index++] = 0x07; //tsa长度
                                        pAmrThreadArg->RelayTrans.RelayData[index++] = 5; //
                                        memcpy(&pAmrThreadArg->RelayTrans.RelayData[index],
                                             pAmrThreadArg->RelayTrans.METER_LIST[meterindex].Mtraddr.add, 6);
                                        index += 6;
    
                                        memcpy(&pAmrThreadArg->RelayTrans.RelayData[index],
                                             &pAppLayer->recvApduList->apdu[3], pAppLayer->recvApduList->apduLen - 5);
                                        index += (pAppLayer->recvApduList->apduLen - 5);
                                        pAmrThreadArg->RelayTrans.AmrIndex++;
    
                                        if(pAmrThreadArg->RelayTrans.AmrIndex >= pAmrThreadArg->RelayTrans.TotalNum)
                                        {
                                        }
                                        else
                                        {
                                            pAmrThreadArg->TotalState.RelayTrans = INNER_RTS_SEND;
                                            break;
                                        }
    
                                    }
                                }
    

                                pAmrThreadArg->RelayTrans.RelayLen = index;
                                TASK_FMT_TRACE(pipe, RELAY_LOG, "pAppLayer->recvApduList->apduLen is %d \n",
                                     pAppLayer->recvApduList->apduLen);

                                pAmrThreadArg->TotalState.RelayTrans = INNER_RTS_END;
    
                            }
                            break;
    
                        case 0x06:
                            {
                                AppLayerList_t * pAppLayer;
                                uint8           mn;
    
                                TASK_FMT_TRACE(pipe, RELAY_LOG, "pAmrThreadArg->RelayTrans.proxychoice is %d \n",
                                     pAmrThreadArg->RelayTrans.proxychoice);
                                TASK_BUF_TRACE(pipe, RELAY_LOG, RecvBuf, RecvLen);
                                ret = LinkDataReceive(pipe, RecvBuf, RecvLen, NULL);
                                TASK_FMT_TRACE(pipe, RELAY_LOG, "LinkDataReceive 698解帧返回值 %d \n", ret);
    
                                if((ret >= eOopComm_Fail) &&
                                     (pAmrThreadArg->RelayTrans.METER_LIST[meterindex].VliadMeter != 0)) //解帧失败直接返回
                                {
                                    pAmrThreadArg->TotalState.RelayTrans = INNER_RTS_END;
                                    break;
                                }
    
                                //获取电表地址
                                pAppLayer = FindAppLayer(pipe, NULL);
    
                                if(pAppLayer == NULL)
                                {
                                    pAmrThreadArg->TotalState.RelayTrans = INNER_RTS_END;
                                    break;
                                }
                                index=pAmrThreadArg->RelayTrans.RelayLen;

                                TASK_FMT_TRACE(pipe, RELAY_LOG, "pAmrThreadArg->RelayTrans.AmrIndex is %d,pAmrThreadArg->RelayTrans.TotalNum is %d\n",
                                     pAmrThreadArg->RelayTrans.AmrIndex, pAmrThreadArg->RelayTrans.TotalNum);
    
                                if(pAmrThreadArg->RelayTrans.AmrIndex < pAmrThreadArg->RelayTrans.TotalNum)
                                {
                                    meterindex = pAmrThreadArg->RelayTrans.AmrIndex;
                                    TASK_FMT_TRACE(pipe, RELAY_LOG, "pAmrThreadArg->RelayTrans.METER_LIST[meterindex].VliadMeter is %d\n",
                                         pAmrThreadArg->RelayTrans.METER_LIST[meterindex].VliadMeter);
    
                                    if(pAmrThreadArg->RelayTrans.METER_LIST[meterindex].VliadMeter == 0)
                                    {
                                        pAmrThreadArg->RelayTrans.RelayData[index++] = 0x07; //tsa长度
                                        pAmrThreadArg->RelayTrans.RelayData[index++] = 5; //
                                        memcpy(&pAmrThreadArg->RelayTrans.RelayData[index],
                                             pAmrThreadArg->RelayTrans.METER_LIST[meterindex].Mtraddr.add, 6);
                                        index += 6;
                                        pAmrThreadArg->RelayTrans.RelayData[index++] =
                                             pAmrThreadArg->RelayTrans.METER_LIST[meterindex].OadNum;
    
                                        for(mn = 0; mn < pAmrThreadArg->RelayTrans.METER_LIST[meterindex].OadNum; mn++)
                                        {
    
                                            memcpy_r(pAmrThreadArg->RelayTrans.RelayData + index,
                                                 (uint8 *) &pAmrThreadArg->RelayTrans.METER_LIST[meterindex].SetOad[mn].value,
                                                 4);
                                            index = index + 4;
                                            pAmrThreadArg->RelayTrans.RelayData[index++] = 0x21;
                                            pAmrThreadArg->RelayTrans.RelayData[index++] = 0x00;
                                            memcpy_r(pAmrThreadArg->RelayTrans.RelayData + index,
                                                 (uint8 *) &pAmrThreadArg->RelayTrans.METER_LIST[meterindex].GetOad[mn].value,
                                                 4);
                                            index = index + 4;
                                            pAmrThreadArg->RelayTrans.RelayData[index++] = 0x00;
                                            pAmrThreadArg->RelayTrans.RelayData[index++] = 0x21;
    
                                        }
    
                                        pAmrThreadArg->RelayTrans.AmrIndex++;
    
                                        if(pAmrThreadArg->RelayTrans.AmrIndex >= pAmrThreadArg->RelayTrans.TotalNum)
                                        {
                                        }
                                        else
                                        {
                                            pAmrThreadArg->TotalState.RelayTrans = INNER_RTS_SEND;
                                            break;
                                        }
                                    }
                                    else
                                    {
                                        pAmrThreadArg->RelayTrans.RelayData[index++] = 0x07; //tsa长度
                                        pAmrThreadArg->RelayTrans.RelayData[index++] = 5; //
                                        memcpy(&pAmrThreadArg->RelayTrans.RelayData[index],
                                             pAmrThreadArg->RelayTrans.METER_LIST[meterindex].Mtraddr.add, 6);
                                        index += 6;
    
                                        memcpy(&pAmrThreadArg->RelayTrans.RelayData[index],
                                             &pAppLayer->recvApduList->apdu[3], pAppLayer->recvApduList->apduLen - 5);
                                        index += (pAppLayer->recvApduList->apduLen - 5);
                                        pAmrThreadArg->RelayTrans.AmrIndex++;
    
                                        if(pAmrThreadArg->RelayTrans.AmrIndex >= pAmrThreadArg->RelayTrans.TotalNum)
                                        {
                                        }
                                        else
                                        {
                                            pAmrThreadArg->TotalState.RelayTrans = INNER_RTS_SEND;
                                            break;
                                        }
    
                                    }
                                }
                                pAmrThreadArg->RelayTrans.RelayLen = index;
                                TASK_FMT_TRACE(pipe, RELAY_LOG, "pAppLayer->recvApduList->apduLen is %d \n",
                                     pAppLayer->recvApduList->apduLen);

                                pAmrThreadArg->TotalState.RelayTrans = INNER_RTS_END;
    
                            }
                            break;
    
                        case 0x07:
                            {
                                uint16          Offset = 0;
                                uint8 *         ptr = NULL;
    
                                TASK_FMT_TRACE(pipe, RELAY_LOG, "pAmrThreadArg->RelayTrans.frametype is %d \n",
                                     pAmrThreadArg->RelayTrans.frametype);
    
                                if(pAmrThreadArg->RelayTrans.frametype == PRTL_OOP)
                                {
                                    tmprecvlen = GetFrame(RecvBuf, RecvLen, &Offset);
                                }
                                else
                                {
                                    ptr = (uint8 *) commfun_645_frame_get((char *) RecvBuf, 0x68, RecvLen, &reallen); //找到645报文的68头
    
                                    if(ptr == NULL)
                                    {
                                        RecvLen = 0;
                                        pAmrThreadArg->TotalState.RelayTrans = INNER_RTS_END;
                                        break;
                                    }
    
                                    tmprecvlen = reallen; //纯645报文长度
    
                                    if(tmprecvlen <= 0)
                                    {
                                        RecvLen = 0;
                                        pAmrThreadArg->TotalState.RelayTrans = INNER_RTS_END;
                                        break;
                                    }
    
                                    memmove(RecvBuf, ptr, RecvLen);
                                }
                                
                                index=pAmrThreadArg->RelayTrans.RelayLen;

                                //结果有数据
                                pAmrThreadArg->RelayTrans.RelayData[index++] = 0x01;
    
                                //数据octer_string
                                len = set_len_offset(tmprecvlen, lenbuf);
                                memcpy(pAmrThreadArg->RelayTrans.RelayData + index, lenbuf, len);
                                index = index + len;
                                memcpy(pAmrThreadArg->RelayTrans.RelayData + index, RecvBuf + Offset, tmprecvlen);
                                index = index + tmprecvlen;
  
                                pAmrThreadArg->RelayTrans.RelayLen = index;
                                TASK_FMT_TRACE(pipe, RELAY_LOG, "pAmrThreadArg->RelayTrans.RelayLen is %d \n",
                                     pAmrThreadArg->RelayTrans.RelayLen);
                                pAmrThreadArg->TotalState.RelayTrans = INNER_RTS_END;
    
                            }
                            break;
    
                        case 50:
                            {
                                FRAME_CONVERT_T convert;
                                uint8 dstbuf[2000]={0};
                                uint8 lenoffset;
                                memset(&convert,0x00,sizeof(FRAME_CONVERT_T));
                                
                                index = pAmrThreadArg->RelayTrans.RelayLen;
                                convert.srcBuf = RecvBuf;
                                convert.srcLen = RecvLen;
                                convert.dstBuf = dstbuf;
                                Convert_Response_Frame(pipe,&convert,pAmrThreadArg->RelayTrans.resframeformat);
                                
                                TASK_FMT_DEBUG(pipe, RELAY_LOG,"接收报文转换前内容 \n");
                                TASK_BUF_DEBUG(pipe,RELAY_LOG,RecvBuf,RecvLen);
                                TASK_FMT_DEBUG(pipe, RELAY_LOG,"接收报文格式字 %d 转换后报文\n",pAmrThreadArg->RelayTrans.resframeformat);
                                TASK_BUF_DEBUG(pipe,RELAY_LOG,convert.dstBuf,convert.dstLen);
                                
                                pAmrThreadArg->RelayTrans.RelayData[index++] = 0x01;
                                lenoffset = set_len_offset(convert.dstLen, &pAmrThreadArg->RelayTrans.RelayData[index]);
                                index+=lenoffset;
                                memcpy(&pAmrThreadArg->RelayTrans.RelayData[index],convert.dstBuf,convert.dstLen);
                                index+=convert.dstLen;
                                pAmrThreadArg->RelayTrans.RelayLen = index;
                                
                                pAmrThreadArg->TotalState.RelayTrans = INNER_RTS_END;
                            }
                            break;
    
                        default:
                            break;
                    }
                }
                break;
    
            default:
                break;
        }
        return 0;
    }
    return -1;
}


/*******************************************************************************
* 函数名称: 中继转发内部流程
* 函数功能:
* 输入参数: 无
* 输出参数: 无
* 返 回 值: >0                  成功
*           <0                  失败
*******************************************************************************/
int fujian_relay_trans_deal ( uint8 pipe )
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    static time_t  TaskBeginTime[PLC_NUM+MAX_485_NUM];
    static time_t  BeginTime[PLC_NUM+MAX_485_NUM];
    static uint16  OverTime[PLC_NUM+MAX_485_NUM]; //超时时间
    int ret;
    int RecvLen=0;
    uint8 RecvBuf[MAX_MSG_LEN]={0};
    uint8 meterindex=0;
    MESSAGE_INFO_T* MsgInfo=NULL;
    uint8 PayloadBuf[MAX_MSG_LEN]={0};

    MeterPrtlFmt_T  MeterPrtlData;
    char            apdu[2]; 
    OOP_OAD_U oad;
    uint8 tmppipe;
    uint8 MtrIndex;
    uint16 index;
    if((pAmrThreadArg->RelayTrans.proxychoice==0x07 || pAmrThreadArg->RelayTrans.proxychoice==50)&&(pAmrThreadArg->RelayTrans.IOP == 0x0011))
    {
        oad.value = pAmrThreadArg->RelayTrans.OadPort;
        tmppipe = taskmng_mtr_pipe_get(&oad);
        if(tmppipe !=pipe)
        {
            TASK_FMT_TRACE( pipe, RELAY_LOG, "端口不匹配\n");
            pAmrThreadArg->RelayTrans.isValid = 0;
            return 1;
        }
    }
    //检查智能转发任务和当前任务的优先级
    if((pAmrThreadArg->RelayTrans.proxychoice==50)||(pAmrThreadArg->RelayTrans.proxychoice==7))
    {
        FUJIAN_TASK_RUN_T * runTask = NULL;
        //找到当前可以执行的任务
        ret = taskmng_trans_task_run_check(pipe, &runTask);
        if(ret > 0)
        {
            if(pAmrThreadArg->RelayTrans.priority >= runTask->priority)
            {
                TASK_FMT_TRACE( pipe, RELAY_LOG, "当前执行任务优先级 %d 代理透传优先级 %d 不允许执行\n",
                        runTask->priority,pAmrThreadArg->RelayTrans.priority );
                taskmng_relay_up_msg_write_0011(pipe,1,0,NULL);
                pAmrThreadArg->RelayTrans.isValid = 0;
                memset(&pAmrThreadArg->RelayTrans,0x00,sizeof(RELAY_TRANS_T));
                return 1;
            }
            
        }

    }
    
    switch(pAmrThreadArg->TotalState.RelayTrans)
    {
    case INNER_RTS_IDLE:
    {
        TASK_FMT_DEBUG( pipe, RELAY_LOG, "穿透任务初始化\n");
        taskmng_pipe_collection_state_write(pipe,TASK_MASTER_TRANS,0,pAmrThreadArg->RelayTrans.priority);
        //485端口需要先初始化串口
        if(fujian_pipe_is_485(pipe))
        {
            ret = fujian_relay_trans_485_init(pipe);
            if(ret<0)
            {
                break;
            }
        }

        OverTime[pipe] = 60;
        TaskBeginTime[pipe] = time(NULL);
        if(pAmrThreadArg->RelayTrans.IOP==0x0011)
        {
            index = 0;
            switch(pAmrThreadArg->RelayTrans.proxychoice)
            {
                case 0x02:
                {
                    pAmrThreadArg->RelayTrans.RelayData[index++]=0x89;
                    pAmrThreadArg->RelayTrans.RelayData[index++]=2;
                    pAmrThreadArg->RelayTrans.RelayData[index++]=pAmrThreadArg->RelayTrans.piid;
                }
                    break;
                case 0x03:
                case 0x04:
                case 0x05:
                case 0x06:
                {
                    pAmrThreadArg->RelayTrans.RelayData[index++] = 0x89;
                    pAmrThreadArg->RelayTrans.RelayData[index++] = pAmrThreadArg->RelayTrans.proxychoice;
                    pAmrThreadArg->RelayTrans.RelayData[index++] = pAmrThreadArg->RelayTrans.piid;
                    pAmrThreadArg->RelayTrans.RelayData[index++] = pAmrThreadArg->RelayTrans.TotalNum;
                }
                    break;
                case 0x07:
                {
                    pAmrThreadArg->RelayTrans.RelayData[index++] = 0x89;
                    pAmrThreadArg->RelayTrans.RelayData[index++] = pAmrThreadArg->RelayTrans.proxychoice;
                    pAmrThreadArg->RelayTrans.RelayData[index++] = pAmrThreadArg->RelayTrans.piid;
                    memcpy_r((uint8*)&pAmrThreadArg->RelayTrans.RelayData[index],(uint8*)&pAmrThreadArg->RelayTrans.OadPort, 4);
                    index +=4;

                    
                }
                    break;
               case 50:
                {
                    pAmrThreadArg->RelayTrans.RelayData[index++] = 0x89;
                    pAmrThreadArg->RelayTrans.RelayData[index++] = pAmrThreadArg->RelayTrans.proxychoice;
                    pAmrThreadArg->RelayTrans.RelayData[index++] = pAmrThreadArg->RelayTrans.piid;
                    memcpy_r((uint8*)&pAmrThreadArg->RelayTrans.RelayData[index],(uint8*)&pAmrThreadArg->RelayTrans.OadPort, 4);
                    index +=4;
                    pAmrThreadArg->RelayTrans.RelayData[index++] = pAmrThreadArg->RelayTrans.tsa.len+2;
                    pAmrThreadArg->RelayTrans.RelayData[index++] = pAmrThreadArg->RelayTrans.tsa.len;
                    memcpy((uint8*)&pAmrThreadArg->RelayTrans.RelayData[index],pAmrThreadArg->RelayTrans.tsa.add,pAmrThreadArg->RelayTrans.tsa.len+1);
                    index+=pAmrThreadArg->RelayTrans.tsa.len+1;
                    pAmrThreadArg->RelayTrans.RelayData[index++] = pAmrThreadArg->RelayTrans.objiecttype;
                    pAmrThreadArg->RelayTrans.RelayData[index++] = pAmrThreadArg->RelayTrans.resframeformat;
                }
                    break;
            }
            pAmrThreadArg->RelayTrans.RelayLen = index;
        }
        pAmrThreadArg->TotalState.RelayTrans = INNER_RTS_SEND;//进入组帧处理
    }
        break;
    case INNER_RTS_SEND:
    {
        if(fujian_pipe_is_485(pipe))
        {
            ret = fujian_relay_trans_send_485( pipe);
        }else
        {
            ret = taskmng_meter_relay_data_send(pipe);
        }
        if(ret>0)
        {
            TASK_FMT_DEBUG( pipe, RELAY_LOG,"Sendindex %d \n",pAmrThreadArg->RelayTrans.Sendindex );
            BeginTime[pipe] = time(NULL);
            pAmrThreadArg->TotalState.RelayTrans = INNER_RTS_RECV;
            if(pAmrThreadArg->RelayTrans.IOP==0x0011)
            {
                MtrIndex = pAmrThreadArg->RelayTrans.AmrIndex;
                switch(pAmrThreadArg->RelayTrans.proxychoice)
                {
                    case 0x01:
                    {
                        OverTime[pipe] = taskmng_proxy_overtime_get(pAmrThreadArg->RelayTrans.AmrNormal[MtrIndex].OverTime);
                    }
                        break;
                    case 0x03:
                    case 0x04:
                    case 0x05:
                    case 0x06:
                    {
                        OverTime[pipe] = taskmng_proxy_overtime_get(pAmrThreadArg->RelayTrans.METER_LIST[meterindex].overtime);
                    }
                        break;
                    case 0x02:
                    case 0x07:
                    case 50:
                    {
                        OverTime[pipe] = taskmng_proxy_overtime_get(pAmrThreadArg->RelayTrans.OverTime);
                    }
                        break;
                        
                }
            }
        }else if(ret<0)
        {
            pAmrThreadArg->TotalState.RelayTrans = INNER_RTS_INVALID;
        }else
        {
            pAmrThreadArg->TotalState.RelayTrans = INNER_RTS_END;
        }
        
    }
        break;
    case INNER_RTS_RECV:
    {
        
        
        if(fujian_pipe_is_485(pipe))
        {
            ret = fujian_relay_trans_recv_485(pipe);
            if(ret<0)
            {
                if((time(NULL) -BeginTime[pipe] > OverTime[pipe]) ||
                                 (time(NULL) -TaskBeginTime[pipe] > pAmrThreadArg->RelayTrans.OverTime))
                {
                    TASK_FMT_DEBUG(pipe, RELAY_LOG, "穿透超时 未收到回复 \n");
                    pAmrThreadArg->TotalState.RelayTrans = INNER_RTS_RETRY;
                }
            }
        }
        else
        {
            RecvLen = taskmng_plc_queue_read(pAmrRecvDown[pipe], RecvBuf);
        
            if(RecvLen > 0)
            {
                MsgInfo = (MESSAGE_INFO_T *) RecvBuf;
                memcpy(PayloadBuf, MsgInfo->playload, MsgInfo->msglen);
                TASK_FMT_DEBUG(pipe, RELAY_LOG, "穿透收到消息 IID %04X IOP %04X MsgIndex %d payload \n", MsgInfo->IID,
                     MsgInfo->IOP, MsgInfo->index);
        
                if(MsgInfo->IOP != pAmrThreadArg->RelayTrans.DownIOP)
                    break;
        
                if(MsgInfo->index != pAmrThreadArg->RelayTrans.Sendindex)
                {
                    TASK_FMT_DEBUG(pipe, RELAY_LOG, "RecvIndex error %d \n", MsgInfo->index);
                    break;
                }
        
                meterindex = pAmrThreadArg->RelayTrans.AmrIndex;
                TASK_BUF_DEBUG(pipe, RELAY_LOG, MsgInfo->playload, MsgInfo->msglen);
                TASK_FMT_DEBUG(pipe, RELAY_LOG, "IOP %04x isBroad %d stat %d \n", pAmrThreadArg->RelayTrans.IOP,
                     pAmrThreadArg->RelayTrans.isBroad, pAmrThreadArg->RelayTrans.BroadStat);
        
                switch(pAmrThreadArg->RelayTrans.IOP)
                {
                    case 0x0010:
                        {
                            //广播校时接收处理
                            if(pAmrThreadArg->RelayTrans.isBroad == 1)
                            {
                                taskmng_transparent_broad_recv(pipe, MsgInfo->playload, MsgInfo->msglen);
                                break;
                            }
#ifdef AREA_FUJIAN
                            //福建协议下透传用0100
                            if(gGatherMode == OOP_GATHER_FJ)
                            {
                                pAmrThreadArg->RelayTrans.RelayLen = MsgInfo->msglen-1;
                                memcpy(pAmrThreadArg->RelayTrans.RelayData,MsgInfo->playload+1,MsgInfo->msglen-1);
                            }else
                            {
#endif
                            pAmrThreadArg->RelayTrans.RelayLen = MsgInfo->msglen;
                            memcpy(pAmrThreadArg->RelayTrans.RelayData,MsgInfo->playload,MsgInfo->msglen);
#ifdef AREA_FUJIAN
                            }
#endif

                            TASK_FMT_DEBUG(pipe, RELAY_LOG, "穿透收到数据 任务结束 \n");
                            taskmng_relay_up_msg_write(pipe, 0);
                            pAmrThreadArg->RelayTrans.RelayResult = 1;
                            pAmrThreadArg->TotalState.RelayTrans = INNER_RTS_END;
                        }
                        break;
        
                    case 0x0011:
                        {
                            switch(pAmrThreadArg->RelayTrans.proxychoice)
                            {
                                case 0x01:
                                    {
                                        uint8           lenoffset = 0;
                                        uint16          index = 0;
                                        uint16          DataLen = 0;
                                        uint8           DataBuff[MAX_MSG_LEN] =
                                        {
                                            0
                                        };
                                        uint8           prtl;
        
                                        DataLen = XADR_GetEleNum(&PayloadBuf[index], &lenoffset);
                                        index += lenoffset;
                                        memcpy(DataBuff, &PayloadBuf[index], DataLen);
                                        taskmng_prtl_get_from_tsa(pAmrThreadArg->RelayTrans.AmrNormal[pAmrThreadArg->RelayTrans.AmrIndex].Mtraddr,
                                             LOGIC_ID_YC, &prtl);
                                        ret = taskmng_relay_data_01_unpack(pipe, DataBuff, DataLen, &MeterPrtlData, apdu);
                                        TASK_FMT_DEBUG(pipe, RELAY_LOG, "0x0011 01收到回码 解帧 ret %d \n", ret);
        
                                        if(ret < 0)
                                        {
                                            pAmrThreadArg->TotalState.RelayTrans = INNER_RTS_RETRY;
                                            break;
                                        }
        
                                        if(prtl == PRTL_OOP)
                                        {
                                            if(apdu[0] == 0x85 && apdu[1] == 0x02)
                                            {
                                                TASK_FMT_DEBUG(pipe, RELAY_LOG, "oop表解析后 datalen %d \n",
                                                     MeterPrtlData.DataLen);
        
                                                TASK_BUF_TRACE(pipe, RELAY_LOG, MeterPrtlData.DataBuff,
                                                     MeterPrtlData.DataLen);
                                                taskmng_oop_relay_data_normallist_save(pipe, MeterPrtlData.DataBuff,
                                                     MeterPrtlData.DataLen);
                                            }
                                        }
                                        else
                                        {
                                            if(ret == CHECK_RESULT_SUCCEED)
                                            {
                                                if(taskmng_645_relay_isvalid(pipe, prtl, MeterPrtlData.DataBuff, MeterPrtlData.CtrlCode, &MeterPrtlData.DataLen) >
                                                     0)
                                                {
                                                    TASK_FMT_DEBUG(pipe, RELAY_LOG, "645验证成功 DataLen %d\n",
                                                         MeterPrtlData.DataLen);
        
                                                    TASK_BUF_DEBUG(pipe, RELAY_LOG, MeterPrtlData.DataBuff,
                                                         MeterPrtlData.DataLen);
        
                                                    if(prtl == PRTL_97)
                                                    {
                                                        taskmng_645_relay_data_save(pipe, prtl, &MeterPrtlData.DataBuff[2],
                                                             MeterPrtlData.DataLen - 2);
                                                    }
                                                    else
                                                    {
                                                        taskmng_645_relay_data_save(pipe, prtl, &MeterPrtlData.DataBuff[4],
                                                             MeterPrtlData.DataLen - 4);
                                                    }
                                                }
        
                                                pAmrThreadArg->RelayTrans.MapIndex++;
                                            }
                                            else
                                                pAmrThreadArg->RelayTrans.MapIndex++;
                                        }
        
                                        pAmrThreadArg->TotalState.RelayTrans = INNER_RTS_SEND;
        
        
                                    }
                                    break;
        
                                case 0x02:
                                case 0x03:
                                case 0x04:
                                case 0x05:
                                case 0x06:
                                    {
                                        uint8           tmpbuffer[MAX_MSG_LEN] =
                                        {
                                            0
                                        };
                                        AppLayerList_t * pAppLayer;
        
                                        memcpy(tmpbuffer, MsgInfo->playload, MsgInfo->msglen);
                                        uint8           lenoffset;
                                        uint16          len = XADR_GetEleNum(tmpbuffer, &lenoffset);
        
                                        ret = LinkDataReceive(pipe, &tmpbuffer[lenoffset], len, NULL);
                                        TASK_FMT_TRACE(pipe, RELAY_LOG, "LinkDataReceive 698解帧返回值 %d \n", ret);
        
                                        if(ret >= eOopComm_Fail) //解帧失败直接返回
                                        {
                                            pAmrThreadArg->TotalState.RelayTrans = INNER_RTS_RETRY;
                                            break;
                                        }
        
                                        //获取电表地址
                                        pAppLayer = FindAppLayer(pipe, NULL);
        
                                        if(pAppLayer == NULL)
                                        {
                                            pAmrThreadArg->TotalState.RelayTrans = INNER_RTS_RETRY;
                                            break;
                                        }
        
                                        index = pAmrThreadArg->RelayTrans.RelayLen;
        
                                        TASK_FMT_TRACE(pipe, RELAY_LOG, "pAmrThreadArg->RelayTrans.AmrIndex is %d,pAmrThreadArg->RelayTrans.TotalNum is %d\n",
                                             pAmrThreadArg->RelayTrans.AmrIndex, pAmrThreadArg->RelayTrans.TotalNum);
        
                                        pAmrThreadArg->RelayTrans.RelayData[index++] = 0x07; //tsa长度
                                        pAmrThreadArg->RelayTrans.RelayData[index++] = 5; //
        
                                        if(pAmrThreadArg->RelayTrans.proxychoice == 0x02)
                                            memcpy_r(&pAmrThreadArg->RelayTrans.RelayData[index],
                                                 pAmrThreadArg->RelayTrans.MtrAddr, 6);
                                        else
                                            memcpy(&pAmrThreadArg->RelayTrans.RelayData[index],
                                                 pAmrThreadArg->RelayTrans.METER_LIST[meterindex].Mtraddr.add, 6);
        
                                        index += 6;
        
                                        memcpy(&pAmrThreadArg->RelayTrans.RelayData[index], &pAppLayer->recvApduList->apdu[3],
                                             pAppLayer->recvApduList->apduLen - 5);
                                        index += (pAppLayer->recvApduList->apduLen - 5);
        
                                        pAmrThreadArg->RelayTrans.RelayLen = index;
        
                                        if(pAmrThreadArg->RelayTrans.proxychoice == 0x02)
                                        {
                                            pAmrThreadArg->TotalState.RelayTrans = INNER_RTS_END;
                                            break;
                                        }
        
                                        pAmrThreadArg->RelayTrans.AmrIndex++;
        
                                        if(pAmrThreadArg->RelayTrans.AmrIndex < pAmrThreadArg->RelayTrans.TotalNum)
                                        {
                                            pAmrThreadArg->TotalState.RelayTrans = INNER_RTS_SEND;
                                        }
                                        else
                                        {
                                            pAmrThreadArg->TotalState.RelayTrans = INNER_RTS_END;
                                        }
                                    }
                                    break;
        
                                case 0x07:
                                    {
                                        //广播校时接收处理
                                        if(pAmrThreadArg->RelayTrans.isBroad == 1)
                                        {
                                            taskmng_transparent_broad_recv(pipe, MsgInfo->playload, MsgInfo->msglen);
                                            break;
                                        }
        
                                        index = pAmrThreadArg->RelayTrans.RelayLen;
        
                                        //结果有数据
                                        pAmrThreadArg->RelayTrans.RelayData[index++] = 0x01;
#ifdef AREA_FUJIAN
                                        //福建协议下透传用0100
                                        if(gGatherMode == OOP_GATHER_FJ)
                                        {
                                            memcpy(pAmrThreadArg->RelayTrans.RelayData+index,MsgInfo->playload+1,MsgInfo->msglen-1);
                                            index += (MsgInfo->msglen-1);
                
                                        }else
                                        {
#endif
                                        memcpy(pAmrThreadArg->RelayTrans.RelayData+index,MsgInfo->playload,MsgInfo->msglen);
                                        index +=MsgInfo->msglen;
#ifdef AREA_FUJIAN
                                        }
#endif

                                        pAmrThreadArg->RelayTrans.RelayLen = index;
        
                                        pAmrThreadArg->TotalState.RelayTrans = INNER_RTS_END;
        
                                    }
                                    break;
        
                                case 50:
                                    { 
                                        FRAME_CONVERT_T convert;
                                        uint8 databuf[2000]={0};
                                        uint16 datalen = 0;
                                        uint8 dstbuf[2000]={0};
                                        uint8 lenoffset;
                                        memset(&convert,0x00,sizeof(FRAME_CONVERT_T));

                                        index = pAmrThreadArg->RelayTrans.RelayLen;
                                        datalen = fujian_recv_payload_unpack_0101(MsgInfo->playload,MsgInfo->msglen,databuf);
                                        convert.srcBuf = databuf;
                                        convert.srcLen = datalen;
                                        convert.dstBuf = dstbuf;
                                        Convert_Response_Frame(pipe,&convert,pAmrThreadArg->RelayTrans.resframeformat);
                                        
                                        TASK_FMT_DEBUG(pipe, RELAY_LOG,"接收报文转换前内容 \n");
                                        TASK_BUF_DEBUG(pipe,RELAY_LOG,databuf,datalen);
                                        TASK_FMT_DEBUG(pipe, RELAY_LOG,"接收报文格式字 %02x 转换后报文\n",pAmrThreadArg->RelayTrans.resframeformat);
                                        TASK_BUF_DEBUG(pipe,RELAY_LOG,convert.dstBuf,convert.dstLen);
                                        
                                        pAmrThreadArg->RelayTrans.RelayData[index++] = 0x01;
                                        lenoffset = set_len_offset(convert.dstLen, &pAmrThreadArg->RelayTrans.RelayData[index]);
                                        index+=lenoffset;
                                        memcpy(&pAmrThreadArg->RelayTrans.RelayData[index],convert.dstBuf,convert.dstLen);
                                        index+=convert.dstLen;
                                        pAmrThreadArg->RelayTrans.RelayLen = index;
        
                                        pAmrThreadArg->TotalState.RelayTrans = INNER_RTS_END;
        
                                    }
                                    break;
        
                                default:
                                    break;
                            }
                        }
                        break;
        
                    default:
                        break;
                }
        
            }
            else
            {
                if((time(NULL) -BeginTime[pipe] > OverTime[pipe]) ||
                     (time(NULL) -TaskBeginTime[pipe] > pAmrThreadArg->RelayTrans.OverTime))
                {
                    TASK_FMT_DEBUG(pipe, RELAY_LOG, "穿透超时 未收到回复 \n");
        
                    pAmrThreadArg->TotalState.RelayTrans = INNER_RTS_RETRY;
                }
            }
        
            }
    }
        break;
    case INNER_RTS_RETRY:
    {
        switch(pAmrThreadArg->RelayTrans.IOP)
        {

        case 0x0010:
        {
            TASK_FMT_DEBUG( pipe, RELAY_LOG, "穿透超时 超时时间 %d 当前时间 %ld TaskBeginTime %ld \n",pAmrThreadArg->RelayTrans.OverTime,time(NULL),TaskBeginTime[pipe]);
            if(time(NULL)-TaskBeginTime[pipe]>pAmrThreadArg->RelayTrans.OverTime)
            {
                TASK_FMT_DEBUG( pipe, RELAY_LOG, "穿透超时 任务结束 \n");
                taskmng_relay_up_msg_write( pipe, -1);
                pAmrThreadArg->TotalState.RelayTrans = INNER_RTS_END;
                break;
            }
            pAmrThreadArg->TotalState.RelayTrans = INNER_RTS_SEND;
        }
            break;
        case 0x0011:
        {
            switch(pAmrThreadArg->RelayTrans.proxychoice)
            {
                case 0x01:
                {
                    MtrIndex = pAmrThreadArg->RelayTrans.AmrIndex;
                    TASK_FMT_DEBUG( pipe, RELAY_LOG, "MtrIndex %d 发送时间 %ld 超时时间 %d \n",MtrIndex,pAmrThreadArg->RelayTrans.AmrNormal[MtrIndex].SendTime,pAmrThreadArg->RelayTrans.AmrNormal[MtrIndex].OverTime);
                    TASK_FMT_DEBUG( pipe, RELAY_LOG, "timenow %ld TaskBeginTime %ld OverTime %d \n",time(NULL),TaskBeginTime[pipe],pAmrThreadArg->RelayTrans.OverTime);
        
                    if(time(NULL)- pAmrThreadArg->RelayTrans.AmrNormal[MtrIndex].SendTime>pAmrThreadArg->RelayTrans.AmrNormal[MtrIndex].OverTime)
                    {
                        pAmrThreadArg->RelayTrans.AmrIndex++;
                        pAmrThreadArg->RelayTrans.OadIndex=0;
                        pAmrThreadArg->RelayTrans.MapIndex=0;
                        pAmrThreadArg->TotalState.RelayTrans = INNER_RTS_SEND;
                        TASK_FMT_DEBUG( pipe, RELAY_LOG, "该表穿透超时 AmrIndex %d  \n",pAmrThreadArg->RelayTrans.AmrIndex);
                    }
                    if(time(NULL)-TaskBeginTime[pipe]>pAmrThreadArg->RelayTrans.OverTime)
                    {
                        TASK_FMT_DEBUG( pipe, RELAY_LOG, "穿透超时 任务结束 \n");
                        pAmrThreadArg->TotalState.RelayTrans = INNER_RTS_END;
                        break;
                    }
                    pAmrThreadArg->TotalState.RelayTrans = INNER_RTS_SEND;
                }
                    break;
                case 0x03:
                case 0x04:
                case 0x05:
                case 0x06:
                {
                    MtrIndex = pAmrThreadArg->RelayTrans.AmrIndex;
                    if(time(NULL)- pAmrThreadArg->RelayTrans.METER_LIST[MtrIndex].firstSendtime>pAmrThreadArg->RelayTrans.METER_LIST[MtrIndex].overtime)
                    {
                        pAmrThreadArg->TotalState.RelayTrans = INNER_RTS_INVALID;
                        TASK_FMT_DEBUG( pipe, RELAY_LOG, "该表穿透超时 AmrIndex %d  \n",pAmrThreadArg->RelayTrans.AmrIndex);
                        break;
                    }
                    if(time(NULL)-TaskBeginTime[pipe]>pAmrThreadArg->RelayTrans.OverTime)
                    {
                        TASK_FMT_DEBUG( pipe, RELAY_LOG, "穿透超时 任务结束 \n");
                        pAmrThreadArg->TotalState.RelayTrans = INNER_RTS_END;
                        break;
                    }
                    pAmrThreadArg->TotalState.RelayTrans = INNER_RTS_SEND;
                }
                    break;
                case 0x02:
                case 0x07:
                case 50:
                {
                    TASK_FMT_DEBUG( pipe, RELAY_LOG, "穿透超时 超时时间 %d 当前时间 %ld TaskBeginTime %ld \n",pAmrThreadArg->RelayTrans.OverTime,time(NULL),TaskBeginTime[pipe]);
                    if(time(NULL)-TaskBeginTime[pipe]>pAmrThreadArg->RelayTrans.OverTime)
                    {
                        TASK_FMT_DEBUG( pipe, RELAY_LOG, "穿透超时 组无效报文 \n");
                        pAmrThreadArg->TotalState.RelayTrans = INNER_RTS_INVALID;
                        break;
                    }
                    pAmrThreadArg->TotalState.RelayTrans = INNER_RTS_SEND;
                }
                    break;
                default:
                    break;
            }
        }
            break;
        default:
            break;
        }

    }
        break;
    case INNER_RTS_INVALID: //代理时某块表无效或者超时
    {
        if(pAmrThreadArg->RelayTrans.IOP==0x0011)
        {
            switch(pAmrThreadArg->RelayTrans.proxychoice)
            {
            case 0x02:
            {
                index = pAmrThreadArg->RelayTrans.RelayLen;
                pAmrThreadArg->RelayTrans.RelayData[index++]=0x07;//tsa长度
                pAmrThreadArg->RelayTrans.RelayData[index++]=5;//
                memcpy_r(&pAmrThreadArg->RelayTrans.RelayData[index],pAmrThreadArg->RelayTrans.MtrAddr,6);
                index+=6;
                //A-ResultRecord
                memcpy_r(&pAmrThreadArg->RelayTrans.RelayData[index],&pAmrThreadArg->RelayTrans.Road.MainOad,sizeof(OOP_OAD_U));
                index+=sizeof(OOP_OAD_U);
                pAmrThreadArg->RelayTrans.RelayData[index++]=pAmrThreadArg->RelayTrans.Road.SubOadNum;
                uint8 i;
                for(i=0;i<pAmrThreadArg->RelayTrans.Road.SubOadNum;i++)
                {
                    pAmrThreadArg->RelayTrans.RelayData[index++]=0x00;
                    memcpy_r(&pAmrThreadArg->RelayTrans.RelayData[index],&pAmrThreadArg->RelayTrans.Road.OadListInfo[i],sizeof(OOP_OAD_U));
                    index+=sizeof(OOP_OAD_U);
                }
                pAmrThreadArg->RelayTrans.RelayData[index++]=0x00;
                pAmrThreadArg->RelayTrans.RelayData[index++]=0x21;
            }
                break;
            case 0x03:
            case 0x04:
            case 0x05:
            case 0x06:
            {
                taskmng_proxy_error_data_pack(pipe);
                pAmrThreadArg->RelayTrans.AmrIndex++;
                if(pAmrThreadArg->RelayTrans.AmrIndex<pAmrThreadArg->RelayTrans.TotalNum)
                {
                    pAmrThreadArg->TotalState.RelayTrans = INNER_RTS_SEND;
                }else
                {
                    pAmrThreadArg->TotalState.RelayTrans = INNER_RTS_END;
                }
            }
                break;
            case 0x07:
            {
                index = pAmrThreadArg->RelayTrans.RelayLen;
                pAmrThreadArg->RelayTrans.RelayData[index++] = 0x00;
                pAmrThreadArg->RelayTrans.RelayData[index++] = 0x21;
                pAmrThreadArg->RelayTrans.RelayLen = index;
               
                pAmrThreadArg->TotalState.RelayTrans = INNER_RTS_END;
            }
                break;
            case 50:
            {
                index = pAmrThreadArg->RelayTrans.RelayLen;

                pAmrThreadArg->RelayTrans.RelayData[index++] = 0x00;
                pAmrThreadArg->RelayTrans.RelayData[index++] = 0x0F;
                pAmrThreadArg->RelayTrans.RelayLen = index;
               
                pAmrThreadArg->TotalState.RelayTrans = INNER_RTS_END;
            }
                break;
            default:
                break;
            }
        }
    }
        break;
    case INNER_RTS_END:
    {
        switch(pAmrThreadArg->RelayTrans.IOP)
        {
        
        case 0x0011:
        {
            
            switch(pAmrThreadArg->RelayTrans.proxychoice)
            {
                case 0x01:
                {
                    //上报数据
                    taskmng_proxy_get_response_list_up_msg_write(pipe);
                }
                    break;
                case 0x02:
                case 0x03:
                case 0x04:
                case 0x05:
                case 0x06:
                case 0x07:
                case 50:
                {
                    if(pAmrThreadArg->RelayTrans.proxychoice !=0x07&&pAmrThreadArg->RelayTrans.proxychoice !=0x02)
                    {
                        //判断下是否由于超时未抄完所有表结束
                        while(pAmrThreadArg->RelayTrans.AmrIndex<pAmrThreadArg->RelayTrans.TotalNum)
                        {
                            TASK_FMT_DEBUG( pipe, RELAY_LOG, "穿透超时组未抄完表的无效报文 amrindex %d \n",pAmrThreadArg->RelayTrans.AmrIndex);
                            taskmng_proxy_error_data_pack(pipe);
                            pAmrThreadArg->RelayTrans.AmrIndex++;
                        }
                    }
                    index=pAmrThreadArg->RelayTrans.RelayLen;
                    uint8  tmpbuffer[MAX_MSG_LEN]={0};
                    uint8  Byteoffset=0;

                    pAmrThreadArg->RelayTrans.RelayData[index++]=0x00;//跟随上报信息域
                    pAmrThreadArg->RelayTrans.RelayData[index++]=0x00;//时间标签
                    if(index<0x80)
                    {
                         tmpbuffer[Byteoffset++]=index;
                    }else
                    {
                         tmpbuffer[Byteoffset++] = 0x82;
                         tmpbuffer[Byteoffset++] = (uint8)(index>>8) ;
                         tmpbuffer[Byteoffset++] = (uint8)index ;
                    }
                    memcpy(tmpbuffer+Byteoffset,pAmrThreadArg->RelayTrans.RelayData,index);
                    index=index+Byteoffset;
                    memcpy(pAmrThreadArg->RelayTrans.RelayData,tmpbuffer,index);
                    pAmrThreadArg->RelayTrans.RelayLen = index;

                    taskmng_relay_up_msg_write( pipe, 0);
                    pAmrThreadArg->RelayTrans.RelayResult =1;

                }
                break;
            default:
                break;
            }
        }
            break;
           
        default:
            break;
        }
        pAmrThreadArg->RelayTrans.isValid = 0;
        memset(&pAmrThreadArg->RelayTrans,0x00,sizeof(RELAY_TRANS_T));
        pAmrThreadArg->TotalState.RelayTrans = INNER_RTS_IDLE;
        return 1;
    }
        break;
    default:
        break;
    }
    return -1;
}



/*******************************************************************************
* 函数名称: fujian_task_data_event_repoort 
* 函数功能:抄表存储数据时上报个事件给puamr
* 输入参数:
   

* 输出参数: 
* 返 回 值: 
*******************************************************************************/
void fujian_task_data_event_report(uint8 pipe,OOP_TRANS_TASK_RESULT_T *transResult,uint8 result)
{
    uint8 payload[MAX_MSG_LEN]={0};
    uint16 index = 0;
    uint8 lenoffset;
    //choice 0 透明方案结果
    payload[index++] = 0;
    memcpy_r(&payload[index],&transResult->transTaskid,2);
    index+=2;
    memcpy_r(&payload[index],&transResult->nodeid,2);
    index+=2;
    memcpy(&payload[index],transResult->groupAdd.add,6);
    index+=6;
    payload[index++] = transResult->commType;
    memcpy_r(&payload[index],&transResult->storeTime.year,2);
    index+=2;
    payload[index++] = transResult->storeTime.month;
    payload[index++] = transResult->storeTime.day;
    payload[index++] = transResult->storeTime.hour;
    payload[index++] = transResult->storeTime.minute;
    payload[index++] = transResult->storeTime.second;
    memcpy_r(&payload[index],&transResult->readTime.year,2);
    index+=2;
    payload[index++] = transResult->readTime.month;
    payload[index++] = transResult->readTime.day;
    payload[index++] = transResult->readTime.hour;
    payload[index++] = transResult->readTime.minute;
    payload[index++] = transResult->readTime.second;
    memcpy_r(&payload[index],&transResult->storeNO,2);
    index+=2;
    payload[index++] = transResult->retryTurn;
    payload[index++] = transResult->result;
    payload[index++] = transResult->resultMessType;
    payload[index++] = transResult->transMsg.errCode;
    lenoffset = set_len_offset(transResult->transMsg.resultData.nNum ,&payload[index]);
    index+=lenoffset;
    if(transResult->transMsg.resultData.nNum>0)
    {
        memcpy(&payload[index],transResult->transMsg.resultData.value,transResult->transMsg.resultData.nNum );
        index += transResult->transMsg.resultData.nNum ;
    }
    //0验证不通过 1验证通过 2首次要上报
    payload[index++] = result;
    
    MESSAGE_INFO_T *MsgSend = NULL;
    TASK_FMT_DEBUG( pipe, REC_LOG, "透明任务方案结果上报\n");

    MsgSend = (MESSAGE_INFO_T*)malloc(sizeof(MESSAGE_INFO_T)+index);
    memset(MsgSend,0x00,sizeof(MESSAGE_INFO_T)+index);

    MsgSend->rpm = 1;
    MsgSend->index = 0;
    MsgSend->priority = 0;
    MsgSend->label = 0;
    MsgSend->IID = 0x1003;
    MsgSend->IOP = 0x0001;
    
    MsgSend->sourlen = strlen(APPNAME);
    memcpy(MsgSend->souraddr,APPNAME,MsgSend->sourlen);
    MsgSend->destlen = strlen(smiosName);
    memcpy(MsgSend->destaddr,smiosName,MsgSend->destlen );

    MsgSend->msglen = index;
    memcpy(MsgSend->playload,payload,index);
    
    taskmng_plc_queue_write(pAmrSendUp[pipe], (uint8*)MsgSend, sizeof(MESSAGE_INFO_T)+index);
    free(MsgSend);
}

/*******************************************************************************
* 函数名称: fujian_task_data_event_repoort 
* 函数功能:抄表存储数据时上报个事件给puamr
* 输入参数:
   

* 输出参数: 
* 返 回 值: 
*******************************************************************************/
void fujian_task_log_event_report(uint8 pipe,char *logfile,uint16 taskid,OOP_DATETIME_S_T *storetime)
{
    uint8 payload[MAX_MSG_LEN]={0};
    uint16 index = 0;
    uint8 lenoffset;
    //choice 0 透明方案结果
    payload[index++] = 1;
    memcpy_r(&payload[index],&taskid,2);
    index+=2;
    memcpy_r(&payload[index],&storetime->year,2);
    index+=2;
    payload[index++] = storetime->month;
    payload[index++] = storetime->day;
    payload[index++] = storetime->hour;
    payload[index++] = storetime->minute;
    payload[index++] = storetime->second;
    lenoffset = set_len_offset(strlen(logfile) ,&payload[index]);
    index+=lenoffset;

    memcpy(&payload[index],logfile,strlen(logfile) );
    index += strlen(logfile);

    MESSAGE_INFO_T *MsgSend = NULL;
    TASK_FMT_DEBUG( pipe, RELAY_LOG, "通信日志任务上报 文件名%s\n",logfile);

    MsgSend = (MESSAGE_INFO_T*)malloc(sizeof(MESSAGE_INFO_T)+index);
    memset(MsgSend,0x00,sizeof(MESSAGE_INFO_T)+index);

    MsgSend->rpm = 1;
    MsgSend->index = 0;
    MsgSend->priority = 0;
    MsgSend->label = 0;
    MsgSend->IID = 0x1003;
    MsgSend->IOP = 0x0001;
    
    MsgSend->sourlen = strlen(APPNAME);
    memcpy(MsgSend->souraddr,APPNAME,MsgSend->sourlen);
    MsgSend->destlen = strlen(smiosName);
    memcpy(MsgSend->destaddr,smiosName,MsgSend->destlen );

    MsgSend->msglen = index;
    memcpy(MsgSend->playload,payload,index);
    
    taskmng_plc_queue_write(pAmrSendUp[pipe], (uint8*)MsgSend, sizeof(MESSAGE_INFO_T)+index);
    free(MsgSend);
}

/*******************************************************************************
* 函数名称: fujian_broadcast_tsa_get 
* 函数功能:根据请求报文格式字获取广播地址 只支持645和698
* 输入参数:
            reqtype 请求报文格式字

* 输出参数: tsa 广播地址
* 返 回 值: -1 不支持的报文格式字 
            0 正常
*******************************************************************************/
int fujian_broadcast_tsa_get(uint8 reqtype,OOP_TSA_T *tsa)
{
    memset(tsa,0x00,sizeof(OOP_TSA_T));
    switch((reqtype>>4) & 0x0f)
    {
        case 0:
            return 0;
        case 1:
        {
            tsa->len = 5;
            memset(tsa->add,0x99,6);
        }
        break;
        case 5:
        {
            tsa->af = 0xC0;
            tsa->add[0] = 0xAA;
        }
        break;
        default:
            return -1;
    }
    return 0;
}

/*******************************************************************************
* 函数名称: fujian_col_state_write
* 函数功能://写6E230200采集状态集 福建
* 输入参数:
* 输出参数: 
* 返 回 值: 
*******************************************************************************/
void fujian_col_state_write(uint8 pipe,uint16 MtrIndex)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    uint16 Pn = pAmrThreadArg->pMtrAmrInfo[MtrIndex].Pn;
    
    OOP_OAD_U Oad = {0x6E230200};
    OOP_TRANS_ACQ_RLUESTATE_T state;
    memset(&state,0x00,sizeof(OOP_TRANS_ACQ_RLUESTATE_T));

    state.add = taskmng_tsa_get(Pn,0);
    state.commType = 3;
    state.oad = taskmng_pipe_oad_get(pipe);
    //如果时间为0  则不赋值 规约召出来是NULL
    if(pAmrThreadArg->pMtrAmrInfo[MtrIndex].LastRecvTime !=0)
    {
        DT_Time2DateTimeShort(pAmrThreadArg->pMtrAmrInfo[MtrIndex].LastRecvTime,&state.lastSuccTime);
    }
    if(pAmrThreadArg->pMtrAmrInfo[MtrIndex].LastFailTime !=0)
    {
        DT_Time2DateTimeShort(pAmrThreadArg->pMtrAmrInfo[MtrIndex].LastFailTime,&state.lastFailTime);
    }


    taskmng_normal_data_write_pipe(pipe,(uint8*)&state,sizeof(OOP_TRANS_ACQ_RLUESTATE_T),Oad,Pn,0);
}
/*******************************************************************************
* 函数名称: fujian_ms_index_get
* 函数功能://根据电表地址获取在当前线程档案中的序号 
* 输入参数:
* 输出参数: 
* 返 回 值: 电表索引 如果没有档案则返回-1
*******************************************************************************/

int fujian_ms_index_get(uint8 pipe,OOP_TSA_T *tsa)
{
    int index = -1;
    uint16 pn ;
    pn = taskmng_pn_get_from_tsa_pipe(pipe,tsa);
    if(pn == 0)
    {
        return -1;
    }
    index = taskmng_ms_index_get(pipe,pn,0);
    return index;
}

/*******************************************************************************
* 函数名称: fujian_col_state_update
* 函数功能:根据cmd更新采集状态 0更新成功时间 1更新失败时间 如果不再档案里就结束
* 输入参数:cmd 
* 输出参数: 
* 返 回 值: 
*******************************************************************************/
void fujian_col_state_update(uint8 pipe,OOP_TSA_T *tsa,uint8 cmd)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];

    int index;
    index = fujian_ms_index_get(pipe,tsa);
    TASK_FMT_TRACE( pipe, REC_LOG, "电表 %02x%02x%02x%02x%02x%02x 更新采集状态 cmd %d index %d\n",tsa->add[0],tsa->add[1],tsa->add[2],tsa->add[3],tsa->add[4],
        tsa->add[5],cmd,index);
    if(index<0)
        return ;
    if(cmd == 0)
        pAmrThreadArg->pMtrAmrInfo[index].LastRecvTime = time(NULL);
    else
        pAmrThreadArg->pMtrAmrInfo[index].LastFailTime = time(NULL);

    //更新完之后写数据中心
    fujian_col_state_write(pipe,index);
}

/*******************************************************************************
* 函数名称: fujian_task_fail_list_add
* 函数功能:将失败的表计添加到faillist中 如果已经添加过 则过滤
* 输入参数:
* 输出参数: 
* 返 回 值: 
*******************************************************************************/
void fujian_task_fail_list_add(OOP_TASK_FAIL_TSA_LIST_T *faillist,OOP_TSA_T *tsa)
{
    uint16 i;
    uint8 flag = 0;
    if(faillist->failNum >= MAX_FAIL_ADDR_NUM)
        return;
    for(i=0;i<faillist->failNum;i++)
    {
        if(memcmp(tsa->add,faillist->failAddr[i],6) == 0)
        {
            flag = 1;
            break;
        }
    }
    if(flag == 1)
        return;
    memcpy(faillist->failAddr[faillist->failNum],tsa->add,6);
    faillist->failNum++;
}

/*******************************************************************************
* 函数名称: fujian_task_fail_addr_write
* 函数功能:任务执行完毕后将失败的表计集合写入数据中心 OAD FF300A00 信息点号为任务方案号
* 输入参数:
* 输出参数: 
* 返 回 值: 
*******************************************************************************/
int fujian_task_fail_addr_write(uint8 pipe,FUJIAN_TASK_RUN_T * runTask)
{
    //if(runTask->result == SCHEME_FINISHED)
       // return 0;
    NOMAL_OAD_T NomalOad;
    uint16 i;
    int ret;
    OOP_TASK_FAIL_TSA_LIST_T faillist;
    memset(&faillist,0x00,sizeof(OOP_TASK_FAIL_TSA_LIST_T));
    faillist.taskid = runTask->taskid;
    //透明转发和预告抄表，并发抄表存失败表的地方不一样
//    if(runTask->transTask->packType >= 3 )
//    {
//        FUJIAN_SPECIFIC_TASK_T *specificTask = runTask->specificTask;
//        while(specificTask!=NULL)
//        {
//
//            for(i=0;i<specificTask->addrAll.num;i++)
//            {
//                if(specificTask->addrAll.result[i]!=1)
//                {
//                    fujian_task_fail_list_add(&faillist,&specificTask->addrAll.addr[i]);
//                }
//            }
//            specificTask = specificTask->next;
//        }
//    }else
    //{
    //抄成功时清空下失败列表
    if(runTask->result != SCHEME_FINISHED)
    {
        for(i=0;i<runTask->meterNum;i++)
        {
            if(runTask->meterInfo[i].meterResult!=2)
            {
                if(faillist.failNum>=MAX_FAIL_ADDR_NUM)
                    continue;
                memcpy(faillist.failAddr[faillist.failNum],runTask->meterInfo[i].meterAddr.add,6);
                faillist.failNum++;
            }
        }
    }
    //}
    TASK_FMT_DEBUG(pipe, REC_LOG, "透明任务方案 %d 总失败电表个数 %d \n",runTask->taskid,faillist.failNum);
    memset(&NomalOad,0x00,sizeof(NOMAL_OAD_T));
    NomalOad.oad.value = 0xFF300A00;
    NomalOad.classtag = 5;
    NomalOad.infoNum = runTask->taskid;
    ret = db_write_nomal(gAmrThreadArg[pipe].DBClientHandle,&NomalOad,(uint8*)&faillist,sizeof(OOP_TASK_FAIL_TSA_LIST_T));
    if(ret!=0)
    {
        TASK_FMT_DEBUG(pipe, REC_LOG, "任务方案 %d 失败表计集合写失败 ret %d\n",runTask->taskid,ret);
    }
    return ret;
}

/*******************************************************************************
* 函数名称: fujian_task_priority_update_get
* 函数功能:根据透明任务方案号从数据中心获取6E11FFFF参数 如果优先级是254 则任务暂停
* 输入参数:
* 输出参数: 
* 返 回 值: 获取的任务优先级 -1 失败 其他表示读到的优先级
*******************************************************************************/
int fujian_task_priority_update_get(DB_CLIENT client,uint16 taskid)
{
    int   ret;
    uint32 Len;

    NOMAL_OAD_T NormalOad;
    NormalOad.logicId = 0;
    NormalOad.oad.value = 0x6E11FFFF;
    NormalOad.infoNum = taskid;
    OOP_TRANS_UPDATE_T data;
    memset(&data,0x00,sizeof(OOP_TRANS_UPDATE_T));

    ret = db_read_nomal(client,&NormalOad,sizeof(OOP_TRANS_UPDATE_T),(uint8*)&data,&Len);
    if(ret!=0 || Len != sizeof(OOP_TRANS_UPDATE_T) || data.transTaskid!=taskid)
    {
        return -1;
    }else
    {
        return data.priority;
    }

}

/*******************************************************************************
* 函数名称: fujian_task_priority_update_deal
* 函数功能:抄表线程收到任务优先级更新后进行处理 254暂停 255重启
* 输入参数:
* 输出参数: 
* 返 回 值: 
*******************************************************************************/

void fujian_task_priority_update_deal(uint8 pipe,uint8* buf,uint16 len)
{
    OOP_TRANS_UPDATE_T data;
    AmrThreadArg_T * pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    uint16 i;
    FUJIAN_TASK_RUN_T *runTask = NULL;
    char pfile[100] ={0};
    int ret;
    if(len != sizeof(OOP_TRANS_UPDATE_T))
        return;
    memcpy(&data,buf,len);
    TASK_FMT_DEBUG(pipe, RELAY_LOG, "收到任务优先级变更 任务号 %d 优先级 %d\n",data.transTaskid,data.priority);

    for(i=0;i<pAmrThreadArg->taskRunList.taskNum;i++)
    {
        if(pAmrThreadArg->taskRunList.taskList[i].taskid == data.transTaskid)
        {
            runTask = &pAmrThreadArg->taskRunList.taskList[i];
            break;
        }
    }
    if(NULL == runTask)
        return;
    if(data.priority == 254)
    {
        runTask->pauseFlag = 1;
        TASK_FMT_DEBUG(pipe, RELAY_LOG, "任务方案号 %d 暂停执行\n",data.transTaskid);
    }
    else if(data.priority == 255)
    {
        taskmng_trans_task_run_reset(runTask);
        runTask->storageCnt = 1;
        runTask->isNewTurn = 1;
        runTask->result = SCHEME_UNDONE;
        runTask->pauseFlag = 0;
        memset(pfile,0x00,sizeof(pfile));
        sprintf(pfile,"%s/%s%d",PrivateDir,TASK_RECORD_FILE,data.transTaskid);
        if(access(pfile,R_OK)==0)
        {
            ret=remove(pfile);
            TASK_FMT_DEBUG(pipe,RELAY_LOG, "删除任务记录文件%s ret is %d\n",pfile,ret);
        }
        TASK_FMT_DEBUG(pipe, RELAY_LOG, "任务方案号 %d 重新执行\n",data.transTaskid);
        taskmng_trans_task_collection_state_update(pipe,runTask,TRANS_TASK_CLEAR,NULL);
    }
    

}
/*******************************************************************************
* 函数名称: fujian_meter_index_get
* 函数功能:根据电表地址在任务方案总的表结构体里找对应的索引
* 输入参数:
* 输出参数: 
* 返 回 值: -1 没找到  其他代表索引
*******************************************************************************/

int fujian_meter_index_get(OOP_TSA_T *tsa,FUJIAN_TASK_RUN_T * runTask)
{
    int index = -1;
    uint16 i;
    for(i=0;i<runTask->meterNum;i++)
    {
        if(memcmp(tsa,&runTask->meterInfo[i].meterAddr,sizeof(OOP_TSA_T)) == 0)
        {
            index = i;
            break;
        }
    }
    return index;
}


/*******************************************************************************
* 函数名称: fujian_cco_info_get 
* 函数功能: 获取cco节点信息 0084
* 输入参数: pipe 端口号
* 输出参数: 
* 返 回 值: 
*******************************************************************************/
void fujian_cco_info_get(uint8 pipe)
{
    int ret;
    uint8 payload[20]={0};
    uint8 len = 0;
    uint16 index = 0;
    ROUTEMODULE RouterModule;
    NOMAL_OAD_T NomalOad;
    memset(&RouterModule,0x00,sizeof(ROUTEMODULE));
    OOP_OAD_U pipeoad = taskmng_pipe_oad_get( pipe);
    memcpy_r(&payload[len],(uint8*)&pipeoad.value,4);
    len+=4;
    payload[len++] = 1;
    //地址填6个0
    len+=6;

    uint16 recvlen;
    uint8 recvbuf[100]={0};
    ret = taskmng_msg_fun_deal(pipe,0x0003,0x0084,len,payload,&recvlen,recvbuf);
    if(ret > 0)
    {
        TASK_FMT_DEBUG(pipe,COMM_LOG,"查询cco节点信息成功 \n");
        RouterModule.verInfo.factory.nNum = 2;
        memcpy(RouterModule.verInfo.factory.value,&recvbuf[index],2);
        index+=2;
        RouterModule.verInfo.ic.nNum = 2;
        memcpy(RouterModule.verInfo.ic.value,&recvbuf[index],2);
        index+=2;
        memcpy_r(&RouterModule.verInfo.dtVer.year,&recvbuf[index],2);
        index+=2;
        RouterModule.verInfo.dtVer.month = recvbuf[index++];
        RouterModule.verInfo.dtVer.mday = recvbuf[index++];
        RouterModule.verInfo.dtVer.wday = recvbuf[index++];
        RouterModule.verInfo.wVer = commfun_BcdToHex(recvbuf[index])*100+commfun_BcdToHex(recvbuf[index+1]);
        TASK_FMT_DEBUG(pipe,COMM_LOG,"厂商代码%s 芯片代码 %s 版本日期 %d年 %d月 %d日 软件版本 %d \n",RouterModule.verInfo.factory.value,
            RouterModule.verInfo.ic.value,RouterModule.verInfo.dtVer.year,RouterModule.verInfo.dtVer.month,
            RouterModule.verInfo.dtVer.mday,RouterModule.verInfo.wVer );
        memset(&NomalOad,0x00,sizeof(NOMAL_OAD_T));
        NomalOad.oad.value = 0xF2090200;
        NomalOad.classtag = 3;
        NomalOad.infoNum = 1; //和以前同步 填1
        ret = db_write_nomal(gAmrThreadArg[pipe].DBClientHandle,&NomalOad,(uint8*)&RouterModule,sizeof(ROUTEMODULE));
        if(ret!=0)
        {
            TASK_FMT_DEBUG(pipe, COMM_LOG, "f2090200写失败 ret %d\n",ret);
            return;
        }
        gIsGetCCOInfo[pipe] = 1;
    }

}

/*******************************************************************************
* 函数名称: fujian_485_share_flag_get
* 函数功能:从数据中心加载6e030300 RS485-2端口数据共享功能 停用 （0），启用（1）
* 输入参数:
* 输出参数: 
* 返 回 值: 
*******************************************************************************/
uint8 fujian_485_share_flag_get(DB_CLIENT clientid)
{
    int   ret;
    uint32 Len;
    uint8 flag = 0;
    uint8 result;
    NOMAL_OAD_T NormalOad;
    NormalOad.logicId = 0;
    NormalOad.oad.value = 0x6E030300;
    NormalOad.infoNum = 0;

    ret = db_read_nomal(clientid,&NormalOad,1,&flag,&Len);
    if(ret!=0 || Len != 1)
    {
        TASK_FMT_TRACE(-1,UP_LOG, "加载0x6E030300失败 ret %d  \n",ret);
        result = 0;
    }else
    {
        result = flag;
    }
    TASK_FMT_TRACE(-1,UP_LOG, "RS485-2端口数据共享功能  %d 停用 （0），启用（1）\n",result);
    return result;
}

/*******************************************************************************
* 函数名称: fujian_collection_send_recv_update 
* 函数功能://mqtt线程中收到0030后的处理
* 输入参数:

* 输出参数: 
* 返 回 值: 
*******************************************************************************/
void fujian_collection_send_recv_update(MESSAGE_INFO_T* MsgInfo )
{
    //发给任务线程处理
    int ret;
    uint8 payload[10]={0};
    uint16 index =0;
    ret = task_send_msg(gMqttSock,MSG_COLLECTION_UPDATE,MsgInfo->playload,MsgInfo->msglen,IPC_TASK);
    TASK_FMT_TRACE(-1,MQTT_LOG,"发送给任务管理线程 ret %d \n",ret);
    payload[index++] = 1;
    MESSAGE_INFO_T *MsgSend = (MESSAGE_INFO_T*)malloc(sizeof(MESSAGE_INFO_T)+index);
    memset(MsgSend,0x00,sizeof(MESSAGE_INFO_T)+index);
    MsgSend->rpm = 0;
    MsgSend->priority = MsgInfo->priority;
    MsgSend->index = MsgInfo->index;
    MsgSend->label = MsgInfo->label;
    MsgSend->sourlen = strlen(APPNAME);
    memset(MsgSend->souraddr,0x00,sizeof(MsgSend->souraddr));
    memcpy(MsgSend->souraddr,APPNAME,MsgSend->sourlen);
    MsgSend->destlen = MsgInfo->sourlen;
    memset(MsgSend->destaddr,0x00,sizeof(MsgSend->destaddr));
    memcpy(MsgSend->destaddr,MsgInfo->souraddr,MsgSend->destlen );
    MsgSend->IID = MsgInfo->IID;
    MsgSend->IOP = MsgInfo->IOP;
    MsgSend->msglen = index;
    memcpy(MsgSend->playload,payload,index);

    taskmng_plc_queue_write ( APPArg.SendBuf, (uint8*)MsgSend, sizeof(MESSAGE_INFO_T)+index );
    free(MsgSend);
}

/*******************************************************************************
* 函数名称: fujian_specific_task_add
* 函数功能: 将具体任务按照优先级插入链表
* 输入参数: pAddTaskRunInfo 需要插入的任务
            headTaskRunInfo 当前优先级最高的链表头
* 输出参数: 
          
* 返 回 值: 优先级最高的链表头
*******************************************************************************/
FUJIAN_SPECIFIC_TASK_T *fujian_specific_task_add(FUJIAN_SPECIFIC_TASK_T *pAddInfo,FUJIAN_SPECIFIC_TASK_T *headInfo)
{
    FUJIAN_SPECIFIC_TASK_T *PTmpHeadTaskLink = NULL;//轮询链表时使用
    FUJIAN_SPECIFIC_TASK_T *ScollLink = NULL;//轮询链表时使用

    if(pAddInfo == NULL)
        return headInfo;

    if(headInfo == NULL)
    {
        headInfo = pAddInfo;
        pAddInfo->next = NULL;
        return headInfo; //添加成功
    }

    if(pAddInfo->nodeid < headInfo->nodeid)
    {
        pAddInfo->next = headInfo;
        return pAddInfo;
    }

    PTmpHeadTaskLink = headInfo;
    while(PTmpHeadTaskLink != NULL)
    {
        //如果插入的节点任务优先级小于节点的任务优先级
        if(pAddInfo->nodeid >= PTmpHeadTaskLink->nodeid)
        {

            ScollLink = PTmpHeadTaskLink;
            PTmpHeadTaskLink = PTmpHeadTaskLink->next;
        }
        else
            break;
    }
    pAddInfo->next = PTmpHeadTaskLink;
    if(NULL == ScollLink)
    {
        return pAddInfo;
    }else
    {
        ScollLink->next = pAddInfo;
    }
    return  headInfo;
}

/*******************************************************************************
* 函数名称: fujian_power_on_notify 
* 函数功能://mqtt线程中收到0031后的处理 扩充的上电通知
* 输入参数:

* 输出参数: 
* 返 回 值: 
*******************************************************************************/
void fujian_power_on_notify(MESSAGE_INFO_T* MsgInfo )
{
    uint8 pipe;

    for(pipe=0;pipe<AMR_THREAD_NUM;pipe++)
    {
        
        gAmrThreadArg[pipe].taskPowerOnFlag = 1 ;

    }
    
}

uint32 fujian_ti_change(OOP_TI_T* ti)
{
    uint32 res = ti->value;
    if(ti->value ==0)
        res = 0;
    else
    {
        switch(ti->unit)
        {
            case TI_SEC:
            {
                res = ti->value;
            }
            break;
            case TI_MIN:
            {
                res = res * 60;
            }
            break;
            case TI_HOUR:
            {
                res = res*3600;
            }
            break;
            case TI_DAY:
            {
                res = res * 3600*24;
            }
            break;
            case TI_MON:
            {
                res = res*3600*24*30;
            }
            break;
            case TI_YEAR:
            {
                res = res*3600*24*365;
            }
            break;
            default:
                break;
        }
        
    }
    return res;
}

/*******************************************************************************
* 函数名称: fujian_ti_compare 
* 函数功能：比较两个ti值的大小
* 输入参数:

* 输出参数: 
* 返 回 值: 
*******************************************************************************/

int fujian_ti_compare(OOP_TI_T* ti1,OOP_TI_T* ti2)
{
    uint32 tmp1,tmp2;
    tmp1 = fujian_ti_change(ti1);
    tmp2 = fujian_ti_change(ti2);
    if(tmp1 < tmp2)
        return -1;
    else if(tmp1 == tmp2)
        return 0;
    else
        return 1;
}

/*******************************************************************************
* 函数名称: fujian_task_priority_change_notify 
* 函数功能：收到优先级变更任务 通知抄表线程
* 输入参数:

* 输出参数: 
* 返 回 值: 
*******************************************************************************/

int fujian_task_priority_change_notify(OOP_TRANS_TASK_T *taskInfo)
{
    OOP_TRANS_UPDATE_T msg;
    memset(&msg,0x00,sizeof(OOP_TRANS_UPDATE_T));
    int ret = 0;
    uint8 pipe = taskmng_mtr_pipe_get(&taskInfo->oad);
    if(pipe!=0xff)
    {
        msg.transTaskid = taskInfo->taskid;
        msg.priority = taskInfo->priority;
        ret = task_send_msg(gTaskSock,MSG_PRIORITY_CHANGE,(uint8*)&msg,sizeof(OOP_TRANS_UPDATE_T),IPC_AMR+pipe);
        TASK_FMT_DEBUG(-1, TASK_LOG, "通知端口 %d 任务优先级变更\n",pipe);
    }
    return ret;
}

/*******************************************************************************
* 函数名称: fujian_task_priority_update_deal
* 函数功能:抄表线程收到任务优先级更新后进行处理 254暂停 255重启
* 输入参数:
* 输出参数: 
* 返 回 值: 
*******************************************************************************/

void fujian_task_priority_change(uint8 pipe,uint8* buf,uint16 len)
{

    OOP_TRANS_UPDATE_T data;
    AmrThreadArg_T * pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    uint16 i;
    FUJIAN_TASK_RUN_T *runTask = NULL;

    if(len != sizeof(OOP_TRANS_UPDATE_T))
        return;
    memcpy(&data,buf,len);
    TASK_FMT_DEBUG(pipe, REC_LOG, "收到任务优先级变更 任务号 %d 优先级 %d 重新进行优先级排序\n",data.transTaskid,data.priority);

    for(i=0;i<pAmrThreadArg->taskRunList.taskNum;i++)
    {
        if(pAmrThreadArg->taskRunList.taskList[i].taskid == data.transTaskid)
        {
            runTask = &pAmrThreadArg->taskRunList.taskList[i];
            break;
        }
    }
    if(NULL == runTask)
        return;
    runTask->priority = data.priority;
    //如果任务前面是暂停 现在给启动
    if(runTask->pauseFlag == 1)
        runTask->pauseFlag = 0;
    taskmng_trans_task_sort(pipe);

}

#endif

