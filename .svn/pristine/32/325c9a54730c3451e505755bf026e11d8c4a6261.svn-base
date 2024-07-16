/*
*********************************************************************
* Copyright(C) 2020 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   回路监控初始化
* @date：    2020-6-30
* @history： 
*********************************************************************
*/

#include "baseCommon.h"
#include <arpa/inet.h>
#include "framecomm.h"
#ifndef PRODUCT_SCU
#include "hal.h"
#include "uart.h"
#endif
#include "iniparser.h"
#include "task_msg.h"
#include "mapmsg.h"
#include "mapmsg_split.h"
#include "appmsg_split.h"
#include "db_api.h"
#include "uart_api.h"

#include "lc_gui.h"
#include "lc_comm.h"
#include "lc_mqtt.h"
#include "lc_monitor.h"
#include "lc_mod.h"
#include "Encrypt.h"

int g_lcm_slot = 0; //回路巡检固定在3号槽，调试版本直接写死
LC_MONITOR_COL_E g_lc_colstatus = LCM_SEND;
BOOL g_mod_status_chg = 0;
void *g_pUart = NULL; /*串口句柄*/
OOP_LOOPSTAT_T g_legendStatus = {0};
OOP_LOOPSTAT_T g_lastStatus = {0};//两次连续相同状态才记录状态变化
uint8 g_recvRetry = 0; //收不到报文的重试次数
LC_WORK_STATUS_T g_workstatus[MOD_SAMPLE_PERIOD];
extern uint32 g_secNum;
OOP_CT_VERTIONS_T g_ctVer = {0}; //给显示用
LC_EVT_FIFO_T g_ctEvt = {0}; //给显示用
uint8 g_verNeedRetry = FALSE; //是否需要重新读版本号

extern void *memcpy_r(void *dest, const void *src, int count);

LC_MODPARSE_T g_modParse[] = 
{
    {MAPMSG_DT_LCM_WORKSTATUS,  lc_ctworkstate_get},
    {MAPMSG_DT_LCM_VERSIONINFO, lc_versioninfo_get},
};

uint32 g_modParse_Max = sizeof(g_modParse)/sizeof(LC_MODPARSE_T);

uint8 send_cmd_7001[20]={0x68,0x01,0x00,0x00,0x00,0x00,0x00,0x68,0x03,0x08,0x34,0x33,0x33,0x3A,0x33,0x33,0x33,0x33,0x7C,0x16}; //7001
uint8 send_cmd_7002[20]={0x68,0x01,0x00,0x00,0x00,0x00,0x00,0x68,0x03,0x08,0x35,0x33,0x33,0x3A,0x33,0x33,0x33,0x33,0x7D,0x16}; //7002
uint8 send_cmd_7003[20]={0x68,0x01,0x00,0x00,0x00,0x00,0x00,0x68,0x03,0x08,0x36,0x33,0x33,0x3A,0x33,0x33,0x33,0x33,0x7E,0x16}; //7003

char *g_uart_parity_desc[] = {"none", "odd", "even"};

#if DESC("事件相关操作",1)

void lc_cur_abn_event_fifo(OOP_ABNORMALCTLOG_T *pEvt)
{
    uint32 slipID = g_ctEvt.nCount;

    if(g_ctEvt.nCount < 10)
    {
        g_ctEvt.nCount++;
    }
    else //超过队列深度，则第一个数据出队，新的添加到尾部
    {
        memmove(&g_ctEvt.fifo[0], &g_ctEvt.fifo[1], 9*sizeof(g_ctEvt.fifo[0]));
        slipID = 9;
    }

    memcpy(&g_ctEvt.fifo[slipID], pEvt, sizeof(g_ctEvt.fifo[slipID]));
}

/**********************************************************************
* @name      : lc_abn_event_make
* @brief     ：构造事件结构体
* @param[in] ：tmEvt  事件发生时，事件结束时间为0，事件恢复时，结束时间是恢复时的时间
* @param[in] ：source  事件发生源 1-发生 0-恢复
* @param[in] ：flag   异常标志

* @param[out]：oopEvent 构造的事件结构体      
* @return    ：
* @Create    : 贺宁
* @Date      ：2020-06-28
* @Update    :
**********************************************************************/
void lc_cur_abn_event_make(time_t tmEvt, uint8 source,uint8 flag, uint32 evtIndex, OOP_ABNORMALCTLOG_T *oopEvent)
{
    OOP_DATETIME_S_T evtTime = {0};
//    OOP_DATETIME_S_T startTm = {0};
        
    //事件开始时间
    DT_Time2DateTimeShort(time(NULL), &evtTime);
//    DT_Time2DateTimeShort(tmEvt, &startTm);

    oopEvent->nIndex = evtIndex+1; //事件序号，单调递增

//    if(source == 1) //事件发生
    {
        oopEvent->tmStart = evtTime; //事件开始时间
        MEMZERO(&oopEvent->tmEnd, sizeof(OOP_DATETIME_S_T));      //事件结束时间固定为0 
    }
//    else
//    {
//        oopEvent->tmStart = startTm; //事件开始时间
//        oopEvent->tmEnd = evtTime;
//    }
    
    oopEvent->source.choice = DT_ENUM; //发生源
    oopEvent->source.nValue = source;
    oopEvent->abnmlflg.nValue = flag; //异常标志 

    return;
}

/**********************************************************************
* @name      : lc_abn_event_check
* @brief     ：电流回路异常事件检查
* @param[in] ：
* @param[out]：
* @return    ：
* @Create    : 贺宁
* @Date      ：2020-7-10
* @Update    :
**********************************************************************/
int lc_cur_abn_event_read(DB_CLIENT client_id, RECORD_OAD_T recordOAD, OOP_OCTETVAR64_T mac, uint16* nEventNum, uint32* nIndex)
{
    READ_RECORD_T startReadRecord;
    STEP_READ_RECORD_T stepReadRecord;
    RECORD_UNIT_T stepRecordOut;
    OOP_EVENT_T *pTemp = NULL;
    uint32 handle = 0;
    uint32 recNum = 0;
    int  ret = 0;
    time_t timep;

    memset(&startReadRecord,0,sizeof(READ_RECORD_T));
    memset(&stepReadRecord,0,sizeof(STEP_READ_RECORD_T));
    memset(&stepRecordOut,0,sizeof(RECORD_UNIT_T));

    time(&timep); //获取从1970至今过了多少秒，存入time_t类型的timep
    timep += COMPENSATION_TIME;
    startReadRecord.recordOAD = recordOAD;
    startReadRecord.cType = 0x02;
    startReadRecord.cStart = 0;
    startReadRecord.cEnd = timep;
    startReadRecord.sortType = 0x01;
   
    ret = db_read_record_start(client_id, &startReadRecord, &handle, &recNum);
    if (ret != ERR_OK)
    {
        LCMONIT_FMT_DEBUG("db_read_record_start, OAD=0x%08x error ret %d\n", recordOAD.road.oadMain.value, ret);
        return ret;

    }

    *nEventNum = (uint16)recNum;
    
    if(recNum == 0)
    {
        db_read_record_end(client_id, handle);
        
        return ERR_OK;
    }

    stepReadRecord.MAC = mac;
    
    ret = db_read_record_step(client_id, handle, &stepReadRecord, &stepRecordOut);   
    db_read_record_end(client_id, handle);
    
    if (ret != ERR_OK)
    {
        LCMONIT_FMT_DEBUG("db_read_record_step,infoNum=%hu, OAD=0x%08x error ret %d\n", recordOAD.infoNum, recordOAD.road.oadMain.value, ret);
        return ret;

    }
    else
    {
        pTemp = (OOP_EVENT_T *)stepRecordOut.record[0].outDataBuf;
        *nIndex = pTemp->nIndex;
    }

    return ERR_OK;
}


/**********************************************************************
* @name      : lc_abn_event_write
* @brief     ：写电流互感器异常事件
* @param[in] ：source   事件发生源 1-发生 0-恢复
* @param[in] ：type      异常类型
* @param[in] ：phasemap 响应的相位A,B,C  

* @param[out]：oopEvent 构造的事件结构体      
* @return    ：
* @Create    : 贺宁
* @Date      ：2020-06-28
* @Update    :
**********************************************************************/
int lc_cur_abn_event_write(DB_CLIENT client_id, uint8 source,uint8 type,uint8 phasemap)//电流互感器异常事件
{
    int ret = 0;
    uint32 storeNo = 0;
    uint8 abnflg=0; //异常标志 0x27串接 0x87短路 0xC7开路
    WRITE_RECORD_T recInData;
    OOP_ABNORMALCTLOG_T oopEvent;     //本次事件信息
    OOP_OCTETVAR16_T  tmnaddr = {0};        //终端通信地址        
    NOMAL_OAD_T  normal = {0}; 
    uint32 len = 0;
    GET_RECORD_NUM_T getRecordNum;
    uint16 recNum = 0;
    uint32 evtIndex = 0;   

    MEMZERO(&recInData, sizeof(WRITE_RECORD_T));
    MEMZERO(&getRecordNum, sizeof(GET_RECORD_NUM_T));
    MEMZERO(&oopEvent, sizeof(OOP_ABNORMALCTLOG_T));

    time_t timep;
    time(&timep); //获取当前时间

    //获取终端地址
    normal.oad.value = 0x40010200;
    ret = db_read_nomal(client_id, &normal, sizeof(OOP_OCTETVAR16_T), (uint8*)&tmnaddr, &len);
    if(ret != ERR_OK)
    {
        LCMONIT_FMT_DEBUG("db_read_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
            ret, normal.logicId, normal.infoNum, normal.oad.value);
        return ret;
    }
    
    //recInData.recordOAD.logicId = 2; //逻辑设备号
    recInData.recordOAD.infoNum = 0; //信息点号
    recInData.recordOAD.classtag = CLASS_DATA_EVENT; //数据类标识：事件数据类
    recInData.recordOAD.road.oadCols.nNum = 1;
    recInData.recordOAD.road.oadCols.oad[0].value = 0x31200200; //事件OAD为次DI，主DI为0
/*
    // 校验码，通信地址的前6个字节,
    recInData.MAC.nNum =6;
    memcpy(recInData.MAC.value, tmnaddr.value, 6);
*/
    lc_cur_abn_event_read(client_id, recInData.recordOAD, recInData.MAC, &recNum, &evtIndex);

    recInData.colStartTm = timep;//开始时间
    recInData.colEndTm = timep; //结束时间
    recInData.colStoreTm = evtIndex+1; //存储时间为事件序号
    recInData.inDatalen = sizeof(OOP_ABNORMALCTLOG_T);
    
    abnflg = (type<<5) | phasemap;
    
    lc_cur_abn_event_make(0, source, abnflg, evtIndex, &oopEvent);
    LCMONIT_FMT_TRACE("source=%d, abnflg=0x%02x, evtIndex=%d, evtNum=%d\n", source, abnflg, evtIndex, recNum);

    //写入对列给显示用
    lc_cur_abn_event_fifo(&oopEvent);

    //数据缓冲为完整的事件记录结构体
    memcpy(recInData.inDataBuf, &oopEvent, sizeof(OOP_ABNORMALCTLOG_T));
    
    //写入数据中心
    ret = db_write_record(client_id, &recInData, &storeNo);
    if (ret != 0)
    {
        LCMONIT_FMT_DEBUG("db_write_record ctevent failed, ret(%d).\n", ret);
        return ret;
    }

    //存储当前记录数
    normal.oad.value = 0x31200400;
    normal.classtag = CLASS_DATA_NORMAL;
    recNum++;
    
    ret = db_write_nomal(client_id, &normal, (uint8*)&recNum, sizeof(uint16));
    if(ret != ERR_OK)
    {
        PRTL_FMT_DEBUG("db_write_recNum failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
            ret, normal.logicId, normal.infoNum, normal.oad.value);
        return ret;
    }

    return ret;
}

/**********************************************************************
* @name      : lc_abn_event_check
* @brief     ：电流回路异常事件检查
* @param[in] ：
* @param[out]：
* @return    ：
* @Create    : 贺宁
* @Date      ：2020-7-10
* @Update    :
**********************************************************************/
uint16 lc_cur_abn_event_check(DB_CLIENT clientid, OOP_LOOPSTAT_T *loopStat)
{
    int i = 0;
    uint8 phasemap = 0;
    NOMAL_OAD_T  normal = {0}; 
    uint32 len = 0;
    BOOLEAN valid = FALSE;
    int ret = 0;
    char phasename[3] = {'A','B','C'};

    //读取事件使能标记
    normal.oad.value = 0x31200900;
    ret = db_read_nomal(clientid, &normal, sizeof(BOOLEAN), (uint8*)&valid, &len);
    if((ret != ERR_OK) || (len == 0) || (valid == FALSE))
    {
        return ret;
    }

    for(i = 0; i < 3; i++)
    {
        //先确定出有效状态，三相整体判断,2次采集的ABC三相状态相同，则状态有效
        if(loopStat->loopstat[i] != g_lastStatus.loopstat[i])
        {
            LCMONIT_FMT_DEBUG("check phase %c status invalid, lastest(%d) != current(%d).\n", phasename[i], g_lastStatus.loopstat[i], loopStat->loopstat[i]);
            valid = FALSE;
            g_lastStatus.loopstat[i] = loopStat->loopstat[i];
        }  
    }

    //状态有效才会继续后面的判断
    if(valid == FALSE)
    {
        return 0;
    }

    for(i = 0; i < 3; i++)
    {
        LCMONIT_FMT_DEBUG("phase %c: legendStatus %d, currentStatus %d.\n", phasename[i], g_legendStatus.loopstat[i], loopStat->loopstat[i]);
    }
    
    //检查三相是否发生短路事件
    for(i = 0; i < 3; i++)
    {
        if((loopStat->loopstat[i] == LC_OOP_STATUS_SC) && (g_legendStatus.loopstat[i] == LC_OOP_STATUS_NR))
        {
            phasemap |= 1<<i;
        }
    }

    //有短路事件发生
    if(phasemap != 0)
    {
        lc_cur_abn_event_write(clientid, 1, LC_EVT_STATUS_SC, phasemap);
        LCMONIT_FMT_DEBUG("check SC event happen, phasemap(%d).\n", phasemap);
    }

    //其它相还有事件
    if(phasemap != 7)
    {
        phasemap = 0;
        //检查三相是否发生开路事件
        for(i = 0; i < 3; i++)
        {
            if((loopStat->loopstat[i] == LC_OOP_STATUS_OC) && (g_legendStatus.loopstat[i] == LC_OOP_STATUS_NR))
            {
                phasemap |= 1<<i;
            }
        }

        //有开路事件发生
        if(phasemap != 0)
        {
            lc_cur_abn_event_write(clientid, 1, LC_EVT_STATUS_OC, phasemap);
            LCMONIT_FMT_DEBUG("check OC event happen, phasemap(%d).\n", phasemap);
        }
    }

    //其它相还有事件
    if(phasemap != 7)
    {
        phasemap = 0;
        //检查三相是否发生串接半导体事件
        for(i = 0; i < 3; i++)
        {
            if((loopStat->loopstat[i] == LC_OOP_STATUS_SE) && (g_legendStatus.loopstat[i] == LC_OOP_STATUS_NR))
            {
                phasemap |= 1<<i;
            }
        }

        //有串接半导体事件发生
        if(phasemap != 0)
        {
            lc_cur_abn_event_write(clientid, 1, LC_EVT_STATUS_SE, phasemap);
            LCMONIT_FMT_DEBUG("check SCR event happen, phasemap(%d).\n", phasemap);
        }   
    }

    //短路异常恢复
    if(phasemap != 7)
    {
        phasemap = 0;
        //检查三相是否恢复短路异常事件
        for(i = 0; i < 3; i++)
        {
            if((loopStat->loopstat[i] == LC_OOP_STATUS_NR) && (g_legendStatus.loopstat[i] == LC_OOP_STATUS_SC))
            {
                phasemap |= 1<<i;
            }
        }

        //短路异常恢复
        if(phasemap != 0)
        {
            lc_cur_abn_event_write(clientid, 0, LC_EVT_STATUS_SC, phasemap);
            LCMONIT_FMT_DEBUG("check SC event recover, phasemap(%d).\n", phasemap);
        }  
    } 

    //开路异常恢复
    if(phasemap != 7)
    {
        phasemap = 0;
        //检查三相是否发生串接半导体事件
        for(i = 0; i < 3; i++)
        {
            if((loopStat->loopstat[i] == LC_OOP_STATUS_NR) && (g_legendStatus.loopstat[i] == LC_OOP_STATUS_OC))
            {
                phasemap |= 1<<i;
            }
        }

        //开路异常恢复
        if(phasemap != 0)
        {
            lc_cur_abn_event_write(clientid, 0, LC_EVT_STATUS_OC, phasemap);
            LCMONIT_FMT_DEBUG("check OC event recover, phasemap(%d).\n", phasemap);
        }   
    }

    //串接半导体异常恢复
    if(phasemap != 7)
    {
        phasemap = 0;
        //检查三相是否发生串接半导体事件
        for(i = 0; i < 3; i++)
        {
            if((loopStat->loopstat[i] == LC_OOP_STATUS_NR) && (g_legendStatus.loopstat[i] == LC_OOP_STATUS_SE))
            {
                phasemap |= 1<<i;
            }
        }

        //串接半导体异常恢复
        if(phasemap != 0)
        {
            lc_cur_abn_event_write(clientid, 0, LC_EVT_STATUS_SE, phasemap);
            LCMONIT_FMT_DEBUG("check SCR event recover, phasemap(%d).\n", phasemap);
        }  
    }
 
    //暂不支持从一个异常状态切换到到另一个异常状态的场景，此场景是否需要生成上个异常的恢复事件尚无定论

    //更新三相有效状态
    memcpy(&g_legendStatus, loopStat, sizeof(OOP_LOOPSTAT_T));

    return 0;
}

#endif

#if DESC("串口操作",1)

//算下两个时间的差值 微秒
int time_check(struct timespec *time1,struct timespec *time2)
{
    int tmp =0;
    if(time2->tv_sec == time1->tv_sec)
    {
        tmp = (time2->tv_nsec -time1->tv_nsec)/1000;
        return tmp;
    }
    if(time2->tv_sec > time1->tv_sec)
    {
        tmp = (time2->tv_sec-time1->tv_sec)*1000+time2->tv_nsec/1000-time1->tv_nsec/1000;
        return tmp;
    }
    return tmp;
}

#ifdef PRODUCT_SCU

/**********************************************************************
* @name      : uart_lock_set
* @brief     ：设置串口锁
* @param[in] ：fd      串口句柄
* @return    ：=0,正确，!=0,错误
* @Create    : 贺宁
* @Date      ：2021-02-01
* @Update    :
**********************************************************************/
int uart_lock_set(int fd, int type)
{
    struct flock lock;
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;
    lock.l_type = type;
    
    if (fcntl(fd, F_SETLK, &lock) == 0)
    {   
        return 1;
    }
    else
    {
        return -1;
    }
}

/**********************************************************************
* @name      : uart_speed_set
* @brief     ：设置串口波特率
* @param[in] ：fd      串口句柄
* @return    ：=0,正确，!=0,错误
* @Create    : 贺宁
* @Date      ：2021-02-01
* @Update    :
**********************************************************************/
int uart_speed_set(int fd, int speed)
{
    int i; 
    int status; 
    int speed_arr[] = {B115200, B38400, B19200, B9600, B4800, B2400, B1200, B600, B300};
    int name_arr[] = {115200, 38400,  19200,  9600,  4800,  2400,  1200, 600, 300};

    struct termios Opt;     /*Opt中存储波特率数据结构*/
    tcgetattr(fd, &Opt);    /*获取termios结构*/
    
    for ( i = 0;  i < sizeof(speed_arr) / sizeof(int);  i++) 
    { 
        if (speed == name_arr[i])   /*查找支持的波特率*/
        { 
            cfsetispeed(&Opt, speed_arr[i]); /*设置输入波特?*/
            cfsetospeed(&Opt, speed_arr[i]); /*设置输出波特?*/
            status = tcsetattr(fd, TCSANOW, &Opt);  /*设置串口新的termios结构*/
            if  (status != 0) 
            {                                 
                return -1;     
            }    
            
            tcflush(fd,TCIOFLUSH); /*清除串口的输入输出队列中的数?*/
            return 1;                     
        }  
    }
    return -1;
}

/**********************************************************************
* @name      : uart_param_set
* @brief     ：设置串口参数
* @param[in] ：fd      串口句柄
* @return    ：=0,正确，!=0,错误
* @Create    : 贺宁
* @Date      ：2021-02-01
* @Update    :
**********************************************************************/
int uart_param_set(const int fd, LC_UART_PARAM_T *param)
{ 
    struct termios options; 
    if  ( tcgetattr( fd,&options)  !=  0) /*读取串口termios结构*/
    { 
        return(FALSE);  
    }
    
    /*设置为不加工模式, 不回?*/
    options.c_oflag = 0;
    options.c_lflag = 0;
    options.c_iflag = IGNBRK;
    options.c_cflag |= CLOCAL | CREAD;
    
    options.c_cflag &= ~CSIZE;  /*清除原字符大*/
    switch (param->dataBits) /*设置数据位数*/
    {   
        case 7:             
            options.c_cflag |= CS7;  /*设置7位数据位?*/
            break;
        case 8:     
            options.c_cflag |= CS8;  /*设置8位数据位?*/
            break;   
        default:    
            return (FALSE);  
    }
    
    switch (param->parity) 
    {   
        case UART_PARITY_NONE: 
            options.c_cflag &= ~PARENB;   /* 输入奇偶校验无效 */
            break;
              
        case UART_PARITY_ODD:                                                               
            options.c_cflag |= (PARODD | PARENB);   /*设置为奇效验*/  
            break;
              
        case UART_PARITY_EVEN:  
            options.c_cflag &= ~PARODD;
            options.c_cflag |= PARENB;  /* 使奇偶校验有?*/ 
            break;
            
        case UART_PARITY_SPACE: /*space校验*/
            options.c_cflag &= ~CSTOPB;
            break;
            
        default:   
            return (FALSE);  
    } 
     
    /* 设置停止?*/ 
    switch (param->stopBits)
    {   
        case 1: /*1位停止位*/
            options.c_cflag &= ~CSTOPB;  
            break;  
        case 2: /*2位停止位*/
            options.c_cflag |= CSTOPB;  
           break;
        default:  
             return (FALSE); 
    }
        
    tcflush(fd,TCIFLUSH);                   /* 清输入队?*/
    options.c_cc[VTIME] = 100;      /* 设置超时10 seconds*/   
    options.c_cc[VMIN] = 0;             /* 更新设置，使马上生效 */
    
    options.c_iflag &= ~(IXON|IXOFF|IXANY); /*关软件流?*/
    
    if (param->flowCtrl == 1)
        options.c_cflag |= CRTSCTS; /*开硬件流控*/
    else
        options.c_cflag &= ~CRTSCTS;/*关硬件流?*/

    options.c_iflag &= ~IGNCR;  /*不忽略收到的CR字符*/
    options.c_iflag &= ~ICRNL;  

    if (tcsetattr(fd,TCSANOW,&options) != 0)  /* 对串口进行参数设??*/
    {  
        return (FALSE);  
    }
    return (TRUE);
}

/**********************************************************************
* @name      : uart_param_get
* @brief     ：读取串口参数
* @param[in] ：fd      串口句柄
* @return    ：=0,正确，!=0,错误
* @Create    : 贺宁
* @Date      ：2021-02-01
* @Update    :
**********************************************************************/
int uart_param_get(const int fd, LC_UART_PARAM_T *param)
{ 
    struct termios options; 
    if  ( tcgetattr( fd,&options)  !=  0) /*读取串口termios结构*/
    { 
        return(FALSE);  
    }

    return TRUE;
}

/**********************************************************************
* @name      : uart_open
* @brief     ：打开串口,防重复打开
* @param[in] ：uart      串口句柄
* @return    ：=0,正确，!=0,错误
* @Create    : 贺宁
* @Date      ：2021-02-01
* @Update    :
**********************************************************************/
int uart_open(LC_UART_VDEV_T *pUart)
{
    if(pUart == NULL)
    {
        return ERR_PNULL;
    }

    if(pUart->fd < 0)
    {  
        pUart->fd = open(pUart->desc, O_RDWR|O_NOCTTY|O_NONBLOCK ); /*打开串口*/
        if(pUart->fd >= 0)
        {
            if (uart_lock_set(pUart->fd, F_WRLCK) == -1)    /*锁串?*/
            {
                LCMONIT_FMT_DEBUG("uart_lock_set %s failed.\n", pUart->desc);  
                close(pUart->fd);
                pUart->fd = -1;
                return ERR_NORMAL;
            }
        }
    }
        
    if (pUart->fd < 0)     /*若打开串口失败，返??1，并通知应用?*/
    {           
        LCMONIT_FMT_DEBUG("open %s failed, %s \n", pUart->desc, strerror(errno));  
        return -1;  
    }

    if (uart_speed_set(pUart->fd, pUart->param.baudRate) == -1)  /*设置串口波特?*/
    {
        LCMONIT_FMT_DEBUG("uart_speed_set %s failed.\n", pUart->desc);  
        close(pUart->fd);
        pUart->fd = -1;
        return ERR_INVAL;
    }           

    if (uart_param_set(pUart->fd, &pUart->param) == FALSE) /*设置串口参数 数据??校验 停止?*/ 
    {
        LCMONIT_FMT_DEBUG("uart_param_set %s failed.\n", pUart->desc);  
        close(pUart->fd);
        pUart->fd = -1;
        return ERR_INVAL;
    }

    LCMONIT_FMT_DEBUG("open %s success.\n", pUart->desc); 
    
    return ERR_OK;
}

/**********************************************************************
* @name      : uart_open
* @brief     ：打开串口
* @param[in] ：uart      串口句柄
* @return    ：=0,正确，!=0,错误
* @Create    : 贺宁
* @Date      ：2021-02-01
* @Update    :
**********************************************************************/
int uart_close(LC_UART_VDEV_T *pUart)
{
    int ret = 0;
    
    if(NULL == pUart)
    {
        return ERR_PNULL;
    }
    
    if(pUart->fd >= 0)
    {
        uart_lock_set(pUart->fd, F_UNLCK);
        ret = close(pUart->fd);
        if(ret == ERR_OK)
        {
            pUart->fd = -1;
            
            LCMONIT_FMT_DEBUG("close %s success.\n", pUart->desc);  
        }
        else
        {
            LCMONIT_FMT_DEBUG("close %s failed, %s \n", pUart->desc, strerror(errno));  
        }
    }

    return ret;
}

/**********************************************************************
* @name      : uart_write
* @brief     ：向串口写数据
* @param[in] ：uart    串口句柄
* @param[in] ：buf     数据缓冲区
* @param[in] ：len     缓冲区长度
* @return    ：=0,正确，!=0,错误
* @Create    : 贺宁
* @Date      ：2021-02-01
* @Update    :
**********************************************************************/
int uart_write(LC_UART_VDEV_T *pUart, uint8* buf, int len)
{
//    int write_len= 0;
    int send_len = 0;  

    if(pUart== NULL)
    {
        return ERR_PNULL;
    }
    
    if (pUart->fd < 0)  
    {  
        LCMONIT_FMT_DEBUG("fd %d is invalid\n", pUart->fd);
        return ERR_INVALID;  
    }

//    do 
//    {
//        if((write_len = write(pUart->fd, buf, len)) <= 0)
//        {
//            if(write_len < 0 && errno == EINTR)
//            {
//                write_len = 0;
//            }
//            else
//            {
//                return ERR_NORMAL;
//            }
//        }
//
//        send_len += write_len;
//
//    }while(send_len < len);

    send_len = write(pUart->fd, buf, len);    
    if(send_len == len)
    {
        clock_gettime(CLOCK_MONOTONIC, &pUart->sendTm);
        pUart->sendCnt++;
        return ERR_OK;
    }
    else
    {
        return ERR_NORMAL;
    }
      
    return ERR_OK;
}

/**********************************************************************
* @name      : uart_read
* @brief     ：从串口读数据
* @param[in] ：uart    串口句柄
* @param[out]：buf     数据缓冲区
* @param[in] ：len     缓冲区长度
* @return    ：读到的数据长度
* @Create    : 贺宁
* @Date      ：2021-02-01
* @Update    :
**********************************************************************/
int uart_read(LC_UART_VDEV_T *pUart, uint8* buf, int len)
{
    int fd = pUart->fd;
    int recv_len = 0;                                       
    int data_len = 0;                                       
    int count = 0;
    int delay = 0;
    fd_set set;
    struct timeval timeout;
    int n = 0;
    uint32 baud = pUart->param.baudRate;
    int firstCnt = pUart->frametimeout*1000;//微秒计算
    int packetCnt = pUart->bytetimeout*1000;//微秒计算
    
    if(NULL == pUart)
    {
        return ERR_PNULL;
    }

    if(fd < 0)
    {
        return ERR_INVAL;
    }
        
    /*串口接收数据超时时间按20个字节间隔时间来计算，300bps 发送一个字节需要36667us
      一个字节包括起始位、停止位、校验位、8位数据 共11位，则每串耗时11/baud秒*/
    if((baud%300 != 0 ) || baud == 0)
    {   
        delay = 11000000/9600;
    }
    else
    {   
        delay = 11000000/baud;
    }

    firstCnt /= delay;
    packetCnt /= delay;

    while(1)
    {
        //等待串口可读
        FD_ZERO(&set);
        FD_SET(fd, &set);
        timeout.tv_sec = 0;
        timeout.tv_usec = (baud < 19200)?30000:1000;
        
        do 
        {
            n = select(fd+1, &set, NULL, NULL, &timeout); //超时返回0
        }while(n < 0);
  
        if(n > 0)  
        {
            recv_len = read(fd, buf+data_len, len-data_len);
        }
        else
        {
            recv_len = 0;
        }

        // 收到字节后延时一个字节的时间再去接收
        if(recv_len > 0)   
        {
            data_len += recv_len;
            count = 0;
            usleep(delay);
        }
        else
        {
            // 读取超时
            if((data_len == 0) && (count > firstCnt)) 
            {
                clock_gettime(CLOCK_MONOTONIC, &pUart->recvTm);
                pUart->recvCnt++;
                usleep(delay);
                return data_len;
            }
            
            //后续字节读取超时或长度已经超过缓冲区
            if((data_len != 0) && ((count > packetCnt) || (data_len > len))) 
            {
                clock_gettime(CLOCK_MONOTONIC, &pUart->recvTm);
                pUart->recvCnt++;
                usleep(delay);
                return (data_len < 0) ? 0 : data_len;                    
            }
        }
  
        count++;
    }

    return data_len;
}

#else
/**********************************************************************
* @name      : uart_open
* @brief     ：打开串口,防重复打开
* @param[in] ：uart      串口句柄
* @return    ：=0,正确，!=0,错误
* @Create    : 贺宁
* @Date      ：2021-02-01
* @Update    :
**********************************************************************/
int uart_open(LC_UART_VDEV_T *pUart)
{
    int ret = ERR_OK;
    UART_DEVICE_T* dev = NULL;
    
    if((pUart == NULL) || (pUart->desc == NULL))
    {
        return ERR_PNULL;
    }

    dev = (UART_DEVICE_T*)hal_device_get(pUart->halname);
    if(NULL == dev)
    {
        return ERR_PNULL;    
    }

    ret = dev->uart_get_real_dev_name(dev, (uint8*)pUart->desc);
    if(ret <= 0)
    {
        LCMONIT_FMT_DEBUG("uart_get_real_dev_name failed. dev(%s), ret(%d)\n", pUart->halname, ret);
        return ERR_OPTION;    
    }

    // 打开设备
    ret = dev->uart_param_set(dev, pUart->param.baudRate, pUart->param.dataBits, pUart->param.stopBits, pUart->param.parity);
    if(ret != 0)
    {
        LCMONIT_FMT_DEBUG("uart_param_set failed. dev(%s), ret(%d)\n", pUart->desc, ret);
        hal_device_release((HW_DEVICE *)dev);
        return ERR_INVAL;
    }

    pUart->fd = (int)dev;

    LCMONIT_FMT_DEBUG("open %s success: baudrate[%d]-databit[%d]-parity[%s]-stopbit[%d].\n", 
        pUart->halname, pUart->param.baudRate, pUart->param.dataBits, g_uart_parity_desc[pUart->param.parity], pUart->param.stopBits);  
    
    return ERR_OK;
}

/**********************************************************************
* @name      : uart_open
* @brief     ：打开串口
* @param[in] ：uart      串口句柄
* @return    ：=0,正确，!=0,错误
* @Create    : 贺宁
* @Date      ：2021-02-01
* @Update    :
**********************************************************************/
int uart_close(LC_UART_VDEV_T *pUart)
{
    UART_DEVICE_T* dev = (UART_DEVICE_T* )pUart->fd;

    if(NULL == pUart)
    {
        return ERR_PNULL;
    }
    
    if(dev != NULL)
    {
        hal_device_release((HW_DEVICE *)dev);

        dev = NULL;
    }

    pUart->fd = (int)dev;
    
    return ERR_OK;
}

/**********************************************************************
* @name      : uart_write
* @brief     ：向串口写数据
* @param[in] ：uart    串口句柄
* @param[in] ：buf     数据缓冲区
* @param[in] ：len     缓冲区长度
* @return    ：=0,正确，!=0,错误
* @Create    : 贺宁
* @Date      ：2021-02-01
* @Update    :
**********************************************************************/
int uart_write(LC_UART_VDEV_T *pUart, uint8* buf, int len)
{
    UART_DEVICE_T* dev = (UART_DEVICE_T* )pUart->fd;
    int ret = 0;  

    if(pUart== NULL)
    {
        return ERR_PNULL;
    }
    
    if(dev == NULL)  
    {  
        LCMONIT_FMT_DEBUG("dev is invalid\n");
        return ERR_INVALID;  
    }

    ret = dev->uart_data_send(dev, buf, len);    
    if(ret == 0)
    {
        clock_gettime(CLOCK_MONOTONIC, &pUart->sendTm);
        pUart->sendCnt++;
        return ERR_OK;
    }
    else
    {        
        LCMONIT_FMT_DEBUG("%s send failed.\n", pUart->halname);
        return ERR_NORMAL;
    }
      
    return ERR_OK;
}

/**********************************************************************
* @name      : uart_read
* @brief     ：从串口读数据
* @param[in] ：uart    串口句柄
* @param[out]：buf     数据缓冲区
* @param[in] ：len     缓冲区长度
* @return    ：读到的数据长度
* @Create    : 贺宁
* @Date      ：2021-02-01
* @Update    :
**********************************************************************/
int uart_read(LC_UART_VDEV_T *pUart, uint8* buf, int len)
{
    UART_DEVICE_T* dev = (UART_DEVICE_T* )pUart->fd;
    int fd = -1;
    int recv_len = 0;                                       
    int data_len = 0;                                       
    int delay = 0;
    fd_set set;
    struct timeval timeout;
    int n = 0;
    uint32 baud = pUart->param.baudRate;
    int frameus = pUart->frametimeout*1000;//微秒计算
    int octetus = pUart->bytetimeout*1000;//微秒计算
    struct timespec timenow;
    struct timespec lastrecv;

    if(dev == NULL)
    {
        return ERR_INVAL;
    }

    fd = dev->uart_get_handle(dev);
    if(fd < 0)
    {
        return ERR_INVAL;
    }

    //等待串口可读
    FD_ZERO(&set);
    FD_SET(fd, &set);
    timeout.tv_sec = frameus/1000000;
    timeout.tv_usec = frameus%1000000;
    
    do 
    {
        n = select(fd+1, &set, NULL, NULL, &timeout); //超时返回0
    }while(n < 0);

    if(n == 0)
    {
        LCMONIT_FMT_DEBUG("%s frame timeout: %d ms\n", pUart->desc, pUart->frametimeout);
        return 0;
    }

    /*串口接收数据超时时间按20个字节间隔时间来计算，300bps 发送一个字节需要36667us
    一个字节包括起始位、停止位、校验位、8位数据 共11位，则每串耗时11/baud秒*/
    if((baud%300 != 0 ) || baud == 0)
    {   
        delay = 11000000/9600;
    }
    else
    {   
        delay = 11000000/baud;
    }

    //防止字节超时过小
    if(octetus < delay)
    {
        octetus += delay;
    }
    
    while(1)
    {
        recv_len = dev->uart_data_recv(dev, buf+data_len, len-data_len);
        
        // 收到字节后延时一个字节的时间再去接收
        if(recv_len > 0)   
        {        
            clock_gettime(CLOCK_MONOTONIC, &lastrecv);
            data_len += recv_len;

            //长度已经超过缓冲区
            if(data_len > len)
            {
                clock_gettime(CLOCK_MONOTONIC, &pUart->recvTm);
                pUart->recvCnt++;
                break;                    
            }
        }
        else
        { 
            clock_gettime(CLOCK_MONOTONIC, &timenow);
            //字节读取超时
            if(time_check(&lastrecv,&timenow) > octetus)
            {
                clock_gettime(CLOCK_MONOTONIC, &pUart->recvTm);
                pUart->recvCnt++;
                break;                    
            }
        }
    }

    data_len = (data_len < 0) ? 0 : data_len;
    return data_len;
}

#endif

/**********************************************************************
* @name      : uart_transmit
* @brief     ：串口数据传输
* @param[in] ：void *pDev                   串口句柄
               int nBufLen                  最大接收长度
               int rsptime                  数据返回时间(20ms)
* @param[out]：
* @return    ：接收到数据的长度
* @Create    : 贺宁
* @Date      ：2020-3-13
* @Update    :
**********************************************************************/
int uart_transmit(LC_UART_VDEV_T *pUart, uint8* pSend, int sendLen, uint8* pRecv, int maxRecv)
{
    int    ret = 0;

    ret = uart_write(pUart, pSend, sendLen);
    if(ret != 0)
    {
        return 0;
    }
    
    return uart_read(pUart, pRecv, maxRecv);
}

/**********************************************************************
* @name      : uart_reopen
* @brief     ：重新打开串口
* @param[in] ：uart      串口句柄
* @return    ：=0,正确，!=0,错误
* @Create    : 贺宁
* @Date      ：2021-02-01
* @Update    :
**********************************************************************/
int uart_reopen(LC_UART_VDEV_T *pUart)
{
    int ret = 0;
    
    ret = uart_close(pUart);
    if(ret == 0)
    {
       sleep_no_interrupt(2);
       ret = uart_open(pUart); 
    }

    return ret;
}

#endif

#if DESC("模组协议解析",1)

BOOLEAN lc_zero_check(void *buf, uint16 len)
{
    uint8 *tmp = (uint8*)buf;
    
    while(len--)
    {
        if(*tmp++ != '0')
        {
            return FALSE;
        }
    }

    return TRUE;
}


/**********************************************************************
* @name      : lc_make_frame
* @brief     ：模组接口协议组帧
* @param[in] ：
* @param[out]：
* @return    ：
* @Create    : 贺宁
* @Date      ：2020-7-10
* @Update    :
**********************************************************************/
LC_OOP_STATUS_E lc_status_mod2oop(LC_MOD_STATUS_E mod)
{
    switch(mod)
    {
        case LC_MOD_STATUS_NR: return LC_OOP_STATUS_NR; break;
        case LC_MOD_STATUS_SC: return LC_OOP_STATUS_SC; break;
        case LC_MOD_STATUS_OC: return LC_OOP_STATUS_OC; break;
        case LC_MOD_STATUS_SE: return LC_OOP_STATUS_SE; break;
        default:break; 
    }

    return 0;
}


/**********************************************************************
* @name      : lc_make_frame
* @brief     ：模组接口协议组帧
* @param[in] ：
* @param[out]：
* @return    ：
* @Create    : 贺宁
* @Date      ：2020-7-10
* @Update    :
**********************************************************************/
int lc_analogs_parse(uint8 *srcBuf, uint32 *pOffset, uint8* dstBuf)
{
    uint32 sOffset = *pOffset;
    uint8          i = 0;
    uint32 dstData[3] = {0};
    
    if(srcBuf[sOffset++] != DT_ARRAY) //非array
    {
        LCMONIT_BUF_DEBUG(srcBuf, sOffset, "check array type failed.\n");
        return ERR_TYPE;
    }

    if(srcBuf[sOffset++] != 3) //array成员数
    {
        LCMONIT_BUF_DEBUG(srcBuf, sOffset, "check array num failed.\n");
        return ERR_TYPE;
    }

    for(i = 0; i < 3; i++)
    {
        if(srcBuf[sOffset++] != DT_DOUBLE_LONG_UNSIGNED) //非double-long-unsigned
        {
            LCMONIT_BUF_DEBUG(srcBuf, sOffset, "check double-long-unsigned type failed.\n");
            return ERR_TYPE;
        }

        memcpy_r(&dstData[i], &srcBuf[sOffset], sizeof(uint32));
        sOffset += sizeof(uint32);
    }

    memcpy(dstBuf, dstData, sizeof(dstData));
    *pOffset = sOffset;

    return 0;
}

/**********************************************************************
* @name      : lc_make_frame
* @brief     ：模组接口协议组帧
* @param[in] ：
* @param[out]：
* @return    ：
* @Create    : 贺宁
* @Date      ：2020-7-10
* @Update    :
**********************************************************************/
uint16 lc_modmsg_make(uint16 dt, uint8 *sendBuf, uint16 buflen)
{
    uint8          msgbuf[128] = {0};
    MAPMSG_INFO_T *pMsgInfo = (MAPMSG_INFO_T *)msgbuf;
    uint8         *Temp = pMsgInfo->apdu;
    uint16         offset = 0;
    static uint8 index = 0;

    pMsgInfo->cmt.dir = MAPMSG_DIR_TERMINAL;
    pMsgInfo->cmt.prm = MAPMSG_PRM_TERMINAL;
    pMsgInfo->cmt.cmd = MAPMSG_CTR_MSG;
    pMsgInfo->cmt.fid = index++;
    
    Temp[offset++] = MAPMSG_APDU_GetRequest;
    mapmsg_memcpy_r(Temp + offset, &dt, sizeof(uint16));
    offset += sizeof(uint16);

    pMsgInfo->len = offset;

    return mapmsg_package(pMsgInfo, sendBuf, buflen);
}

/**********************************************************************
* @name      : lc_workstatus_store
* @brief     ：工况信息存储
* @param[in] ：
* @param[out]：
* @return    ：
* @Create    : 贺宁
* @Date      ：2020-7-10
* @Update    :
* @detail    :
* 回路巡检模块应答的数据单位和倍率                  698对应的换算和单位          转换成698倍率
* 电流回路状态             HH          1               
* 组合互感器环境温度 XXX.X                2   ℃      -1     ℃     1
* 相频率最大值             XXX.XXX     3   kHz     0     Hz    1
* 相频率最小值             XXX.XXX     3   kHz     0     Hz    1
* 工频电流有效值            XXX.XXX     3   A      -3     A     1
* 第一组回路阻抗频率 XXX.X                2   kHz     0     Hz    100
* 第一组回路阻抗模值 XXXXXX.XX            4   mV     -2     Ω     1
* 第一组回路阻抗角度 XXX.X                2   °      -1     °     1
* 第二组回路阻抗频率 XXX.X                2   kHz     0     Hz    100
* 第二组回路阻抗模值 XXXXXX.XX            4   mV     -2     Ω     1
* 第二组回路阻抗角度 XXX.X                2   °      -1     °     1
* 第三组回路阻抗频率 XXX.X                2   kHz     0     Hz    100
* 第三组回路阻抗模值 XXXXXX.XX            4   mV     -2     Ω     1
* 第三组回路阻抗角度 XXX.X                2   °      -1     °     1
**********************************************************************/
int lc_workstatus_store(DB_CLIENT clientid, LC_WORK_STATUS_T *workStatus)
{
    int ret = 0;
    NOMAL_OAD_T nomalOAD = {0};
    OOP_WORKINFO_T workinfo;
    MEMZERO(&workinfo, sizeof(OOP_WORKINFO_T));
    OOP_LOOPSTAT_T loopStat = {0};
    int i = 0;
    char phasename[3] = {'A','B','C'};

    for(i = 0; i < 3; i++)
    {
        workinfo.item[i].curloopstat = lc_status_mod2oop(workStatus->loopStatus[i]);
        workinfo.item[i].temp = workStatus->envTemp[i];
        workinfo.item[i].maxfreq = workStatus->freqMax[i];
        workinfo.item[i].minfreq = workStatus->freqMin[i];
        workinfo.item[i].curvalid = workStatus->curValue[i];

        workinfo.item[i].impval.nNum = 3;    
        workinfo.item[i].impval.impeval[0] = workStatus->impeValue1[i];
        workinfo.item[i].impval.impeval[1] = workStatus->impeValue2[i];
        workinfo.item[i].impval.impeval[2] = workStatus->impeValue3[i];

        workinfo.item[i].impang.nNum = 3;
        workinfo.item[i].impang.impeang[0] = workStatus->impeAnge1[i];  
        workinfo.item[i].impang.impeang[1] = workStatus->impeAnge2[i];        
        workinfo.item[i].impang.impeang[2] = workStatus->impeAnge3[i];   

        workinfo.item[i].impfrq.nNum = 3;
        workinfo.item[i].impfrq.impefreq[0] = workStatus->impeFreq1[i]*100;
        workinfo.item[i].impfrq.impefreq[1] = workStatus->impeFreq2[i]*100;
        workinfo.item[i].impfrq.impefreq[2] = workStatus->impeFreq3[i]*100;

        loopStat.loopstat[i] = workinfo.item[i].curloopstat;    

        LCMONIT_FMT_DEBUG("\r\n store phase %c:\
                           \r\n loopState: %d,\
                           \r\n temperature: %d\
                           \r\n freqMax: %d\
                           \r\n freqMin: %d\
                           \r\n current: %d\
                           \r\n Freq_Am_Ang_1: [%d %d %d]\
                           \r\n Freq_Am_Ang_2: [%d %d %d]\
                           \r\n Freq_Am_Ang_3: [%d %d %d]\n",
                            phasename[i],
                            workStatus->loopStatus[i],workStatus->envTemp[i],
                            workStatus->freqMax[i],workStatus->freqMin[i],workStatus->curValue[i],
                            workStatus->impeFreq1[i], workStatus->impeValue1[i],workStatus->impeAnge1[i],
                            workStatus->impeFreq2[i],workStatus->impeValue2[i],workStatus->impeAnge2[i],
                            workStatus->impeFreq3[i],workStatus->impeValue3[i],workStatus->impeAnge3[i]);
     
    }

    //工况信息
    workinfo.nNum = 3;

    nomalOAD.classtag = CLASS_DATA_NORMAL;
    nomalOAD.oad.value = 0x40400300;
    ret = db_write_nomal(clientid, &nomalOAD, (uint8*)&workinfo, sizeof(workinfo));
    if(ret != 0)
    {
        LCMONIT_FMT_DEBUG("lc_workstatus_store failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x), inLen(%d)\n",
        ret, nomalOAD.logicId, nomalOAD.infoNum, nomalOAD.oad.value, sizeof(uint8));
    
        return ret;
    }

    //回路状态
    loopStat.nNum = 3;
    
    nomalOAD.oad.value = 0x20500200;
    ret = db_write_nomal(clientid, &nomalOAD, (uint8*)&loopStat, sizeof(loopStat));
    if(ret != 0)
    {
        LCMONIT_FMT_DEBUG("lc_loopStatus_store failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x), inLen(%d)\n",
        ret, nomalOAD.logicId, nomalOAD.infoNum, nomalOAD.oad.value, sizeof(uint8));
    
        return ret;
    }

    //回路异常事件生成
    lc_cur_abn_event_check(clientid, &loopStat);
        
    return ret;
}

int lc_versioninfo_store(DB_CLIENT clientid, OOP_CT_VERTIONS_T *version)
{
    int ret = ERR_OK;
    NOMAL_OAD_T nomalOAD = {0};
    OOP_CTINFO_T ctinfo = {0};
    int i = 0;
    char *type[4] = {"CORE","CT-A","CT-B", "CT-C"};
    ctinfo.nNum = version->nNum;

    for(i = 0; i < ctinfo.nNum; i++)
    {
        memcpy(&ctinfo.item[i].id, &version->ver[i].szId, sizeof(OOP_OCTETVAR64_T));
        ctinfo.item[i].szSoftVer = version->ver[i].szSoftVer;
        ctinfo.item[i].szSoftDate = version->ver[i].szSoftDate;
        ctinfo.item[i].szHardVer = version->ver[i].szHardVer;
        ctinfo.item[i].szHardDate = version->ver[i].szHardDate;
        memcpy(&ctinfo.item[i].szFactory,  &version->ver[i].szFactory, sizeof(OOP_VISIBLESTR4_T));

        MDLMSG_FMT_DEBUG("\r\n 类型        : %s\
                           \r\n 型号        : %s\
                           \r\n ID          : %02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x\
                           \r\n 厂商代码    : %s\
                           \r\n 软件版本号  : %s\
                           \r\n 软件发布日期: %s\
                           \r\n 硬件版本号  : %s\
                           \r\n 硬件发布日期: %s\
                           \r\n 厂商扩展信息: %s\n", 
                           type[i], version->ver[i].szType.value,version->ver[i].szId.value[0],version->ver[i].szId.value[1],
                           version->ver[i].szId.value[2],version->ver[i].szId.value[3],version->ver[i].szId.value[4],
                           version->ver[i].szId.value[5],version->ver[i].szId.value[6],version->ver[i].szId.value[7],
                           version->ver[i].szId.value[8],version->ver[i].szId.value[9],version->ver[i].szId.value[10],
                           version->ver[i].szId.value[11],version->ver[i].szId.value[12],version->ver[i].szId.value[13],
                           version->ver[i].szId.value[14],version->ver[i].szId.value[15],version->ver[i].szId.value[16],
                           version->ver[i].szId.value[17],version->ver[i].szId.value[18],version->ver[i].szId.value[19],
                           version->ver[i].szId.value[20],version->ver[i].szId.value[21],version->ver[i].szId.value[22], version->ver[i].szId.value[23], 
                           version->ver[i].szFactory.value,version->ver[i].szSoftVer.value,version->ver[i].szSoftDate.value,
                           version->ver[i].szHardVer.value,version->ver[i].szHardDate.value,version->ver[i].szExtend.value);

        if(lc_zero_check(ctinfo.item[i].szSoftDate.value, strlen(ctinfo.item[i].szSoftDate.value)) == TRUE)
        {
            MDLMSG_FMT_DEBUG("full zero data, need retry.\n");
            g_verNeedRetry = TRUE;
        }
    }

    //版本信息
    nomalOAD.classtag = CLASS_DATA_NORMAL;
    nomalOAD.oad.value = 0x40400500;
    ret = db_write_nomal(clientid, &nomalOAD, (uint8*)&ctinfo, sizeof(OOP_CTINFO_T));
    if(ret != 0)
    {
        LCMONIT_FMT_DEBUG("lc_workstatus_store failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x), inLen(%d)\n",
        ret, nomalOAD.logicId, nomalOAD.infoNum, nomalOAD.oad.value, sizeof(uint8));
    
        return ret;
    }
 
    return ret;
}

int lc_versioninfo_get(DB_CLIENT clientid, uint8 *apdu, uint32 apduLen, void *stdata)
{
    int ret = 0;
    uint32 sOffset = 0;
    uint8  i = 0;
    uint8 bytes = 0;      //数组数量长度
    uint32 nNum  = 0;
    OOP_CT_VERTIONS_T ctVersion;
    uint32 len = 0;

    MEMZERO(&ctVersion, sizeof(OOP_CT_VERTIONS_T));
    
    LCMONIT_BUF_TRACE(apdu, apduLen, "lc version ciphertext apdu.\n");    
    
    //只解密APDU中的DATA,解密后再解析
    String_Decrypt(apdu, apduLen);
    
    LCMONIT_BUF_DEBUG(apdu, apduLen, "lc version plaintext apdu.\n");
    
    if(apdu[sOffset++] != DT_ARRAY) //非array
    {
        LCMONIT_BUF_DEBUG(apdu, sOffset, "check array type failed.\n");
        return ERR_TYPE;
    }

    ctVersion.nNum = apdu[sOffset++];

    if(ctVersion.nNum != 4) //array成员数
    {
        LCMONIT_BUF_DEBUG(apdu, sOffset, "check array num failed.\n");
        return ERR_TYPE;
    }

    for(i = 0; i < ctVersion.nNum; i++)
    {
        if(apdu[sOffset++] != DT_STRUCTURE) //非struct
        {
            LCMONIT_BUF_DEBUG(apdu, sOffset, "check struct type failed.\n");
            return ERR_TYPE;
        }
        
        if(apdu[sOffset++] != 8) //成员数
        {
            LCMONIT_BUF_DEBUG(apdu, sOffset, "check struct num failed.\n");
            return ERR_TYPE;
        }

        //型号
        if(apdu[sOffset++] != DT_VISIBLE_STRING) //visible-string
        {
            LCMONIT_BUF_DEBUG(apdu, sOffset, "check visible-string type failed.\n");
            return ERR_TYPE;
        }

        ctVersion.ver[i].szType.nNum = appmsg_get_len_offset(&apdu[sOffset], &bytes); //计算visible-string的长度
        sOffset += bytes;

        memcpy(&ctVersion.ver[i].szType.value, &apdu[sOffset], ctVersion.ver[i].szType.nNum);
        sOffset += ctVersion.ver[i].szType.nNum;

        //ID
        if(apdu[sOffset++] != DT_VISIBLE_STRING) //visible-string
        {
            LCMONIT_BUF_DEBUG(apdu, sOffset, "check visible-string type failed.\n");
            return ERR_TYPE;
        }

        len = appmsg_get_len_offset(&apdu[sOffset], &bytes); //计算visible-string的长度
        sOffset += bytes;

        //此处698上行需要的是octet-string要做转换
        ctVersion.ver[i].szId.nNum = strt2octet((char*)&apdu[sOffset], len, ctVersion.ver[i].szId.value, 64);
        if ((ctVersion.ver[i].szId.nNum < 0) || (ctVersion.ver[i].szId.nNum > 64))
        {
            LCMONIT_BUF_DEBUG(apdu, sOffset, "check visible-string type failed. ctVersion.ver[i].szId.nNum[%d]\n", ctVersion.ver[i].szId.nNum);
            return ERR_TYPE;
        }
        sOffset += len;
        
        //厂商代码
        if(apdu[sOffset++] != DT_VISIBLE_STRING) //visible-string
        {
            LCMONIT_BUF_DEBUG(apdu, sOffset, "check visible-string type failed.\n");
            return ERR_TYPE;
        }

        nNum = appmsg_get_len_offset(&apdu[sOffset], &bytes); //计算visible-string的长度
        sOffset += bytes;

        memcpy(&ctVersion.ver[i].szFactory.value, &apdu[sOffset], nNum);
        sOffset += nNum;

        //软件版本号
        if(apdu[sOffset++] != DT_VISIBLE_STRING) //visible-string
        {
            LCMONIT_BUF_DEBUG(apdu, sOffset, "check visible-string type failed.\n");
            return ERR_TYPE;
        }

        nNum = appmsg_get_len_offset(&apdu[sOffset], &bytes); //计算visible-string的长度
        sOffset += bytes;

        memcpy(&ctVersion.ver[i].szSoftVer.value, &apdu[sOffset], nNum);
        sOffset += nNum;

        //软件版本日期
        if(apdu[sOffset++] != DT_VISIBLE_STRING) //visible-string
        {
            LCMONIT_BUF_DEBUG(apdu, sOffset, "check visible-string type failed.\n");
            return ERR_TYPE;
        }

        nNum = appmsg_get_len_offset(&apdu[sOffset], &bytes); //计算visible-string的长度
        sOffset += bytes;

        memcpy(&ctVersion.ver[i].szSoftDate.value, &apdu[sOffset], nNum);
        sOffset += nNum;

        //硬件版本号
        if(apdu[sOffset++] != DT_VISIBLE_STRING) //visible-string
        {
            LCMONIT_BUF_DEBUG(apdu, sOffset, "check visible-string type failed.\n");
            return ERR_TYPE;
        }

        nNum = appmsg_get_len_offset(&apdu[sOffset], &bytes); //计算visible-string的长度
        sOffset += bytes;

        memcpy(&ctVersion.ver[i].szHardVer.value, &apdu[sOffset], nNum);
        sOffset += nNum;

        //硬件版本日期
        if(apdu[sOffset++] != DT_VISIBLE_STRING) //visible-string
        {
            LCMONIT_BUF_DEBUG(apdu, sOffset, "check visible-string type failed.\n");
            return ERR_TYPE;
        }

        nNum = appmsg_get_len_offset(&apdu[sOffset], &bytes); //计算visible-string的长度
        sOffset += bytes;

        memcpy(&ctVersion.ver[i].szHardDate.value, &apdu[sOffset], nNum);
        sOffset += nNum;

        //厂家扩展信息
        if(apdu[sOffset++] != DT_VISIBLE_STRING) //visible-string
        {
            LCMONIT_BUF_DEBUG(apdu, sOffset, "check visible-string type failed.\n");
            return ERR_TYPE;
        }

        nNum = appmsg_get_len_offset(&apdu[sOffset], &bytes); //计算visible-string的长度
        sOffset += bytes;

        memcpy(&ctVersion.ver[i].szExtend.value, &apdu[sOffset], nNum);
        sOffset += nNum;
    }
    
    //版本信息存储
    ret = lc_versioninfo_store(clientid, &ctVersion);
    g_ctVer = ctVersion;

    return ret;
}

/**********************************************************************
* @name      : lc_modmsg_parse
* @brief     ：模组接口协议解析
* @param[in] ：
* @param[out]：
* @return    ：
* @Create    : 贺宁
* @Date      ：2020-7-10
* @Update    :
**********************************************************************/
int lc_modmsg_parse(DB_CLIENT clientid, uint8 *recvBuf, uint16 buflen)
{
    int            ret = 0;
    uint16         offset = 0;
    MAPMSG_INFO_T *pMsgInfo = NULL;
    uint8         *temp = NULL;
    uint16         dt = 0;
    uint8          i = 0;
    int            tempret = 0;
    uint32 sOffset = 0;
    
    //找到帧起始位
    while(offset < buflen)
    {
        if(FRAMESTART == recvBuf[offset])
        {
            tempret = mapmsg_split(recvBuf + offset, buflen - offset, &pMsgInfo);
            if(tempret != 0)
            {
                LCMONIT_BUF_DEBUG(recvBuf + offset, buflen - offset, "mapmsg split failed, tempret(%d)\n", tempret);
            }
            break;
        }
        offset++;
    }

    if(pMsgInfo == NULL)
    {
        LCMONIT_BUF_DEBUG(temp, sOffset, "check null pointer.\n");
        return ERR_PNULL;
    }

    //功能单一，不考虑多帧
    if(MAPMSG_CTR_MSG == pMsgInfo->cmt.cmd && pMsgInfo->len > 4)
    {
        temp = pMsgInfo->apdu;

        if((MAPMSG_APDU_GetResponse == temp[0])&&(1 == temp[3]))  //data [1]
        {
            sOffset += 2;

            memcpy_r(&dt, &temp[1], sizeof(uint16));
            sOffset += sizeof(uint16);

            for(i = 0; i < g_modParse_Max; i++)
            {
                if((g_modParse[i].di == dt)&&(g_modParse[i].apduGet != NULL))
                {
                    ret = g_modParse[i].apduGet(clientid, &temp[sOffset], pMsgInfo->len-sOffset, NULL);
                    break;
                }
            }

            if(i == g_modParse_Max)
            {
                ret = ERR_OPTION;
            }
        }
    }
    
    FREE_S(pMsgInfo);
    
    return ret;
}

int lc_ctworkstate_get(DB_CLIENT clientid, uint8 *apdu, uint32 apduLen, void *stdata)
{
    uint32 sOffset = 0;
    uint8  i = 0;
    uint8 bytes = 0;      //数组数量长度
    uint32 nNum  = 0;
    int ret = ERR_OK;
    LC_WORK_STATUS_T workstatus;
    char phasename[3] = {'A','B','C'};

    MEMZERO(&workstatus, sizeof(LC_WORK_STATUS_T));

    LCMONIT_BUF_TRACE(apdu, apduLen, "lc ciphertext apdu.\n");    
    
    //只解密APDU中的DATA,解密后再解析
    String_Decrypt(apdu, apduLen);
    
    LCMONIT_BUF_DEBUG(apdu, apduLen, "lc plaintext apdu.\n");
    
    if(apdu[sOffset++] != DT_STRUCTURE) //非struct
    {
        LCMONIT_BUF_DEBUG(apdu, sOffset, "check struct type failed.\n");
        return ERR_TYPE;
    }
    
    if(apdu[sOffset++] != 32) //成员数
    {
        LCMONIT_BUF_DEBUG(apdu, sOffset, "check struct num failed.\n");
        return ERR_TYPE;
    }
    
    //电流回路状态
    if(apdu[sOffset++] != DT_ARRAY) //非array
    {
        LCMONIT_BUF_DEBUG(apdu, sOffset, "check array type failed.\n");
        return ERR_TYPE;
    }
    
    if(apdu[sOffset++] != 3) //array成员数
    {
        LCMONIT_BUF_DEBUG(apdu, sOffset, "check array num failed.\n");
        return ERR_TYPE;
    }
    
    for(i = 0; i < 3; i++)
    {
        if(apdu[sOffset++] != DT_OCTET_STRING) //octet-string
        {
            LCMONIT_BUF_DEBUG(apdu, sOffset, "check octet-string type failed.\n");
            return ERR_TYPE;
        }
    
        nNum = appmsg_get_len_offset(&apdu[sOffset], &bytes); //计算octet-string的长度
        sOffset += bytes;
    
        memcpy(&workstatus.loopStatus[i], &apdu[sOffset], nNum);
        sOffset += nNum;
    }
    
    //标识量：备用
    if(apdu[sOffset++] != DT_ARRAY) //非array
    {
        LCMONIT_BUF_DEBUG(apdu, sOffset, "check array type failed.\n");
        return ERR_TYPE;
    }
    
    if(apdu[sOffset++] != 0) //array成员数
    {
        LCMONIT_BUF_DEBUG(apdu, sOffset, "check array num failed.\n");
        return ERR_TYPE;
    }
    
    //环境温度
    ret |= lc_analogs_parse(apdu, &sOffset, (uint8*)workstatus.envTemp);
    
    //频率最大值
    ret |= lc_analogs_parse(apdu, &sOffset, (uint8*)workstatus.freqMax);
    
    //频率最小值
    ret |= lc_analogs_parse(apdu, &sOffset, (uint8*)workstatus.freqMin);
    
    //工频电流有效值
    ret |= lc_analogs_parse(apdu, &sOffset, (uint8*)workstatus.curValue);
    
    //第一组阻抗频率
    ret |= lc_analogs_parse(apdu, &sOffset, (uint8*)workstatus.impeFreq1);
    
    //第一组阻抗模值
    ret |= lc_analogs_parse(apdu, &sOffset, (uint8*)workstatus.impeValue1);  
    
    //第一组阻抗角度
    ret |= lc_analogs_parse(apdu, &sOffset, (uint8*)workstatus.impeAnge1);                    
    
    //第二组阻抗频率
    ret |= lc_analogs_parse(apdu, &sOffset, (uint8*)workstatus.impeFreq2);
    
    //第二组阻抗模值
    ret |= lc_analogs_parse(apdu, &sOffset, (uint8*)workstatus.impeValue2);  
    
    //第二组阻抗角度
    ret |= lc_analogs_parse(apdu, &sOffset, (uint8*)workstatus.impeAnge2);       
    
    //第三组阻抗频率
    ret |= lc_analogs_parse(apdu, &sOffset, (uint8*)workstatus.impeFreq3);
    
    //第三组阻抗模值
    ret |= lc_analogs_parse(apdu, &sOffset, (uint8*)workstatus.impeValue3);  
    
    //第三组阻抗角度
    ret |= lc_analogs_parse(apdu, &sOffset, (uint8*)workstatus.impeAnge3);
    if(ret != ERR_OK)
    {
        return ret;
    }

    memcpy(&g_workstatus[g_secNum-1], &workstatus, sizeof(LC_WORK_STATUS_T));
    
    for(i = 0; i < 3; i++)
    {
        LCMONIT_FMT_DEBUG("secNum %d, phase %c, loopState %d\n",g_secNum, phasename[i], workstatus.loopStatus[i]);
    }

    return ret;
}

/**
*********************************************************************
* @brief：     lc_status_cpy
* @param[in]： NA
* @param[out]：NA
* @return：     void
*********************************************************************
*/
void lc_status_cpy(LC_WORK_STATUS_T *dstData, LC_WORK_STATUS_T *srcData,  uint8 phase)
{
    dstData->loopStatus[phase] = srcData->loopStatus[phase];
    
    dstData->envTemp[phase] = srcData->envTemp[phase];
    dstData->freqMax[phase] = srcData->freqMax[phase];
    dstData->freqMin[phase] = srcData->freqMin[phase];
    dstData->curValue[phase] = srcData->curValue[phase];  

    dstData->impeValue1[phase] = srcData->impeValue1[phase];
    dstData->impeValue2[phase] = srcData->impeValue2[phase];
    dstData->impeValue3[phase] = srcData->impeValue3[phase];

    dstData->impeAnge1[phase] = srcData->impeAnge1[phase];  
    dstData->impeAnge2[phase] = srcData->impeAnge2[phase];        
    dstData->impeAnge3[phase] = srcData->impeAnge3[phase];   

    dstData->impeFreq1[phase] = srcData->impeFreq1[phase];
    dstData->impeFreq2[phase] = srcData->impeFreq2[phase];
    dstData->impeFreq3[phase] = srcData->impeFreq3[phase];    
}

#endif

#if DESC("模组消息收发",1)

/**
*********************************************************************
* @brief：     lc_version_get
* @param[in]： NA
* @param[out]：NA
* @return：     void
*********************************************************************
*/
int lc_version_get(void *pDev, DB_CLIENT clientid, int socketfd)
{
    int ret = 0;
    uint8 sendbuf[128] = {0};
    uint16 sendlen = 0;   //发送报文长度
    uint8 sRecvbuf[1024] = {0};//单次接收
    uint16 sRecvLen = 0; //单次接收长度
    static uint16 retryCnt = 0;

    if(pDev == NULL)
    {
        LCMONIT_FMT_DEBUG("pDev is NULL\n");
        return ERR_PNULL;
    }
    
    sendlen = lc_modmsg_make(MAPMSG_DT_LCM_VERSIONINFO, sendbuf, sizeof(sendbuf));

    LCMONIT_BUF_DEBUG(sendbuf, sendlen, "lc_uart_send: version.\n");

    sRecvLen = uart_transmit(pDev, sendbuf, sendlen, sRecvbuf, sizeof(sRecvbuf));
    if(sRecvLen != 0)
    {
        retryCnt = 0;
        ret = lc_modmsg_parse(clientid, sRecvbuf, sRecvLen);
        if(ret != 0)
        {
            LCMONIT_BUF_DEBUG(sRecvbuf, sRecvLen, "lc_modmsg_parse failed,ret(%d).\n", ret);
            return ERR_NORMAL;
        }
    }
    else
    {
        if(++retryCnt >= LCM_SEND_RETRY)
        {
            retryCnt = 0;
            return ERR_INVALID;
        }
    }
    
    return ret;
}

/**
*********************************************************************
* @brief：     lc_workstate_get
* @param[in]： NA
* @param[out]：NA
* @return：     void
*********************************************************************
*/
int lc_workstate_get(void *pDev, DB_CLIENT clientid, int socketfd)
{
    int ret = 0;
    uint8 sendbuf[128] = {0};
    uint16 sendlen = 0;   //发送报文长度
    uint8 sRecvbuf[1024] = {0};//单次接收
    uint16 sRecvLen = 0; //单次接收长度
    static uint16 retryCnt = 0;

    if(pDev == NULL)
    {
        LCMONIT_FMT_DEBUG("pDev is NULL\n");
        return ERR_PNULL;
    }
    
    sendlen = lc_modmsg_make(MAPMSG_DT_LCM_WORKSTATUS, sendbuf, sizeof(sendbuf));

    LCMONIT_BUF_DEBUG(sendbuf, sendlen, "lc_uart_send: workstate.\n");

    sRecvLen = uart_transmit(pDev, sendbuf, sendlen, sRecvbuf, sizeof(sRecvbuf));
    if(sRecvLen != 0)
    {
        retryCnt = 0;
        ret = lc_modmsg_parse(clientid, sRecvbuf, sRecvLen);
        if(ret != 0)
        {
            LCMONIT_BUF_DEBUG(sRecvbuf, sRecvLen, "lc_modmsg_parse failed,ret(%d).\n", ret);
            return ERR_NORMAL;
        }
    }
    else
    {
        if(++retryCnt >= LCM_SEND_RETRY)
        {
            retryCnt = 0;
            return ERR_INVALID;
        }
    }

    return ret;
}

/**
*********************************************************************
* @brief：     lc_monitor_read
* @param[in]： NA
* @param[out]：NA
* @return：     void
*********************************************************************
*/
void lc_monitor_read(void *pDev, uint16 dt, DB_CLIENT clientid, int socketfd)
{
    int ret = 0;
    uint8 sendbuf[128] = {0};
    uint16 sendlen = 0;   //发送报文长度
    uint8 sRecvbuf[1024] = {0};//单次接收
    uint16 sRecvLen = 0; //单次接收长度

    if(pDev == NULL)
    {
        LCMONIT_FMT_DEBUG("pDev is NULL\n");
        return;
    }
    
    sendlen = lc_modmsg_make(dt, sendbuf, sizeof(sendbuf));

    LCMONIT_BUF_DEBUG(sendbuf, sendlen, "lc_uart_send.\n");

    sRecvLen = uart_transmit(pDev, sendbuf, sendlen, sRecvbuf, sizeof(sRecvbuf));
    if(sRecvLen != 0)
    {
        ret = lc_modmsg_parse(clientid, sRecvbuf, sRecvLen);
        if(ret != 0)
        {
            LCMONIT_BUF_DEBUG(sRecvbuf, sRecvLen, "lc_modmsg_parse failed,ret(%d).\n", ret);
        }
    }
}

void lc_workstate_analyze(DB_CLIENT clientid, int socketfd)
{
    uint32 i = 0, j = 0;
    LC_STATUS_STAT_T statusStat[LC_MOD_STATUS_ED];
    LC_WORK_STATUS_T workstatus;   
    char phasename[3] = {'A','B','C'};

    MEMZERO(&statusStat, sizeof(statusStat));
    MEMZERO(&workstatus, sizeof(LC_WORK_STATUS_T));

    //统计每种状态出现的次数
    for(i = 0; i < g_secNum; i++)
    {
        for(j = 0; j <3; j++)
        {
            switch(g_workstatus[i].loopStatus[j])
            {
                case LC_MOD_STATUS_NR:
                {
                    statusStat[LC_MOD_STATUS_NR].count[j]++;
                    lc_status_cpy(&statusStat[LC_MOD_STATUS_NR].status, &g_workstatus[i], j);
                }break;

                case LC_MOD_STATUS_SC:
                {
                    statusStat[LC_MOD_STATUS_SC].count[j]++;
                    lc_status_cpy(&statusStat[LC_MOD_STATUS_SC].status, &g_workstatus[i], j);
                }break;

                case LC_MOD_STATUS_OC:
                {
                    statusStat[LC_MOD_STATUS_OC].count[j]++;
                    lc_status_cpy(&statusStat[LC_MOD_STATUS_OC].status, &g_workstatus[i], j);
                }break;

                case LC_MOD_STATUS_SE:
                {
                    statusStat[LC_MOD_STATUS_SE].count[j]++;
                    lc_status_cpy(&statusStat[LC_MOD_STATUS_SE].status, &g_workstatus[i], j);
                }break;

                default:
                {
                    if(i == g_secNum -1) //最后一个无效，意味着本次数据无效
                    {
                        LCMONIT_FMT_DEBUG("本周期数据均无效.\n");
                        return;
                    }
                }
           }

            if((i+1)%MOD_SAMPLE_INTERV == 0)
            {
                LCMONIT_FMT_DEBUG("sec %d, phase %c, status %d, count %d\n", i+1, phasename[j], g_workstatus[i].loopStatus[j], statusStat[g_workstatus[i].loopStatus[j]].count[j]);
            }
        }

    }


    for(j = 0; j < 3; j++)
    {
        for(i = 0; i < LC_MOD_STATUS_ED; i++)
        {
            if(statusStat[i].count[j] > MOD_SAMPLE_LIMITS) //该相该状态超过阈值
            {
                lc_status_cpy(&workstatus, &statusStat[i].status, j);
                break;
            }
        }
        
        if(i == LC_MOD_STATUS_ED) //没有一个超阈值，则取最后一个状态
        {            
            lc_status_cpy(&workstatus, &g_workstatus[g_secNum -1], j);
        }
    }

    //工况信息存储
    lc_workstatus_store(clientid, &workstatus);

    return;
}

/**
*********************************************************************
* @brief：     lc_monitor_collect
* @param[in]： NA
* @param[out]：NA
* @return：     void
*********************************************************************
*/
void lc_monitor_collect(void *pPublic, void *pPrivate)
{
    LC_APPDATA_T *pAppData = (LC_APPDATA_T *)pPublic;
    LC_MONITOR_DATA_T *ptMsgData = (LC_MONITOR_DATA_T *)pPrivate;
    void *pDev = ptMsgData->pUart;
    DB_CLIENT clientid = ptMsgData->clientid;
    int socketfd = ptMsgData->tasksocket;
    int ret = 0;
    static uint8 lc_status = LCM_SEND;
    LC_UART_VDEV_T *pUart = (LC_UART_VDEV_T*)pDev;

    switch(lc_status)
    {
        case LCM_OPEN:
        {
            uart_close(pDev);
            if(0 == access(pUart->desc, F_OK))
            {
                LCMONIT_FMT_DEBUG("%s is exist\n", pUart->desc);
                lc_status = LCM_INIT;
            }
            sleep_no_interrupt(2);
        }break;

        case LCM_INIT:
        {
            ret = uart_open(pDev);
            if(ret == ERR_OK)
            {
                lc_status = LCM_SEND;
            }
        }break;

        case LCM_SEND:
        {
            if(g_secNum % MOD_SAMPLE_INTERV == 0)
            {
                if(pAppData->appType == LC_APP_OLD)
                {
                    ret = lc_workstate_get(pDev, clientid, socketfd);
                }
                else
                {
                    ret = lcm_workstate_get(pDev, clientid, socketfd);
                }
                
                if(ret != ERR_OK)
                {
                    if(ret == ERR_INVALID)
                    {
                        lc_status = LCM_OPEN;
                    }
                }
            }
        }break;

        default:break;
    }

    return;
}

///**
//*********************************************************************
//* @brief：     lc_monitor_collect
//* @param[in]： NA
//* @param[out]：NA
//* @return：     void
//*********************************************************************
//*/
//void lc_monitor_collect(DB_CLIENT clientid, int socketfd)
//{
////    char dev[32] = {0};  /* 模块所用串口描述符 */
//    static uint8 retry = 0;
//    uint16 sendlen = 0;   //发送报文长度
//    uint16 revlen = 0;    //接收拼接后总长度
//    int singleLen = 0; //单次接收长度
//    int ret = 0;
//    uint8 sendbuf[128] = {0};
//    uint8 recvbuf[1024] = {0}; //拼接后缓存
//    uint8 sRecvbuf[512] = {0};//单次接收
//
//    switch(g_lc_colstatus)
//    {
//        case LCM_INIT: 
//        {
//            LCMONIT_FMT_DEBUG("LCM_INIT.\n");
//            
//            if(g_lcm_slot != 0)
//            {
//                //lc_uart_uninit(&g_pUart);
//                g_lc_colstatus = LCM_SEND;
//            }
//            else
//            {
//                //收到模块插拔事件
//                if(g_mod_status_chg)
//                {
//                    g_mod_status_chg = FALSE;
//                    g_lc_colstatus = LCM_GETM;
//                }
//            }
//        }break;
//
//        case LCM_SEND:
//        {
//            LCMONIT_FMT_DEBUG("LCM_SEND.\n");
////            if(g_lcm_slot == 0)
////            {
////                g_lc_colstatus = LCM_GETM;
////                break;
////            }
//            
////            //避免可能的重复注册
////            if(g_pUart == NULL)
////            {
////                sprintf(dev, "%s%d_1",MOD_DEV_DESC_PRFIX, g_lcm_slot);            
////                g_pUart = lc_uart_init(dev); /*打开串口*/
////                if(g_pUart == NULL)
////                {
////                    LCMONIT_FMT_DEBUG("lc_uart_init failed dev: %s.\n", dev);
////                    g_lc_colstatus = LCM_GETM;
////                    break;
////                }
////            }
//
//            sendlen = lc_modmsg_make(MAPMSG_DT_LCM_WORKSTATUS, sendbuf, sizeof(sendbuf));
//            //ret = lc_uart_send(g_pUart, sendbuf, sendlen);
//            //ret = uart_transmit(6, sendbuf, sendlen, sRecvbuf, sizeof(sRecvbuf), &revlen);
//            LCMONIT_BUF_DEBUG(sendbuf, sendlen, "lc_uart_send: workstatus. ret(%d)\n", ret);
//            if(ret != 0)
//            {
//                if(++retry > LCM_SEND_RETRY)
//                {
//                    g_lc_colstatus = LCM_INIT;
//                    retry = 0;
//                }
//            }
//            else
//            {
//                //g_lc_colstatus = LCM_RECV;
//                ret = lc_modmsg_parse(clientid, sRecvbuf, revlen);
//                if(ret != 0)
//                {
//                    LCMONIT_BUF_DEBUG(recvbuf, revlen, "lc_modmsg_parse failed,ret(%d).\n", ret);
//                }
//            }
//
//        }; //这里不break，发完后直接收，收发只一个状态
//
//        case LCM_RECV: 
//        {
//            usleep(100000);
//            
//            LCMONIT_FMT_DEBUG("LCM_RECV.\n");
//            
//            if(g_lcm_slot == 0)
//            {
//                g_lc_colstatus = LCM_GETM;
//                break;
//            }
//            
//            //singleLen = lc_uart_recv(g_pUart, sRecvbuf, sizeof(sRecvbuf));
//            if(singleLen > 0)
//            {
//                g_recvRetry = 0;
//                LCMONIT_BUF_DEBUG(sRecvbuf, singleLen, "lc_uart_recv frame.\n");
//            }
//            else
//            {
//                if(++g_recvRetry >= LCM_RECV_RETRY)
//                {
//                    g_recvRetry = 0;
//                    //lc_uart_uninit(&g_pUart);
//                    lc_mod_restart_send(g_lcm_slot, 10); //2分钟未收到回复，模块断电10s
//                }
//            }
//            
//            while(singleLen > 0)
//            {
//                //缓存区报文已满, 无法继续拼接
//                if(revlen >= sizeof(recvbuf))
//                {
//                    LCMONIT_FMT_DEBUG("recvlen(%d) >= sizeof(recvbuf)(%d) \n", revlen, sizeof(recvbuf));
//                    return;
//                }
//
//                //对单条报文进行拼接处理
//                if(singleLen > (sizeof(recvbuf) - revlen))
//                {
//                    singleLen = sizeof(recvbuf) - revlen;
//                    LCMONIT_FMT_DEBUG("singleLen(%d) > (sizeof(recvbuf)(%d) - revlen(%d))\n", singleLen, sizeof(recvbuf), revlen);
//                }
//                
//                memcpy(&recvbuf[revlen], sRecvbuf, singleLen);
//                revlen += singleLen;    
//
//                usleep(100000);
//                
//                //singleLen = lc_uart_recv(g_pUart, sRecvbuf, sizeof(sRecvbuf));
//                if(singleLen > 0)
//                {
//                    LCMONIT_BUF_DEBUG(sRecvbuf, singleLen, "lc_uart_recv continue frame.\n");
//                }             
//            }
//
//            if(revlen != 0)
//            {
//                ret = lc_modmsg_parse(clientid, recvbuf, revlen);
//                if(ret != 0)
//                {
//                    LCMONIT_BUF_DEBUG(recvbuf, revlen, "lc_modmsg_parse failed,ret(%d).\n", ret);
//                }
//            }
//            //为规避句柄同时释放可能导致的僵尸进程，收发过程中句柄打开后不释放，除非拔模块
//            //lc_uart_uninit(pUart);
//            //pUart = NULL;
//            //lc_version_get(g_pUart, clientid, socketfd);
//
//            g_lc_colstatus = LCM_SEND;   
//        }break;
//
//        case LCM_GETM:
//        {
//            LCMONIT_FMT_DEBUG("LCM_GETM.\n");
//            
//            if(g_lcm_slot != 0)
//            {
//                //lc_uart_uninit(&g_pUart);
//                g_lc_colstatus = LCM_SEND;
//                break;
//            }
//
//             //lc_uart_uninit(&g_pUart);
//             
//            //过秒执行，1s查一次，查10次
//            if(retry++ < LCM_SEND_RETRY )
//            {
//                lc_get_map_slot(socketfd, MOD_LC_TYPE_PRFIX);
//                g_lc_colstatus = LCM_SEND;
//                
//            }
//            else
//            {   
//                g_lc_colstatus = LCM_INIT;
//                retry = 0;
//            }
//        }break;
//
//        default: g_lc_colstatus = LCM_INIT; break;
//    }
//}

#endif

#if DESC("算法模块消息解析",1)

LC_MODPARSE_T g_ctalgParse[] = 
{
    {LCM_DI_WORKSTATUS,  lcm_ctworkstate_parse},
    {LCM_DI_CTVERSION,   lcm_ctversion_parse},
    {LCM_DI_COREVERSION, lcm_coreversion_parse},
    {LCM_DI_FILETRANSMIT,lcm_filetrans_parse},
};

uint32 g_ctalgParse_Max = sizeof(g_ctalgParse)/sizeof(LC_MODPARSE_T);

void hex_to_ascii(char *p, uint8 BcdNum)
{
    if((BcdNum & 0xF0) < 0xA0)
        p[0] = (BcdNum >> 4)+0x30;        //数字
    else
    {
        p[0] = (BcdNum >> 4)+0x37;        //字母ABCDEF
    }
    if((BcdNum & 0x0F) < 0x0A)
        p[1] = (BcdNum & 0x0F)+0x30;        //数字
    else
    {
        p[1] = (BcdNum & 0x0F)+0x37;        //字母ABCDEF
    }
}

/**********************************************************************
* @name      : data_byte_add
* @brief     ：逐字节加数据
* @param[out]：pBuf   被加数据缓冲
* @param[in] ：lengh 缓冲区大小
* @param[in] ：value 加数
* @return    ：无
* @Create    : 贺宁
* @Date      ：2021-05-20
* @Update    :
**********************************************************************/
void data_byte_add(uint8 *pBuf, uint8 lengh, uint8 value)
{
    uint8 i = 0;

    for(i = 0; i < lengh; i++)
    {
        pBuf[i] += value;
    } 
}

/**********************************************************************
* @name      : data_byte_sub
* @brief     ：逐字节减数据
* @param[out]：pBuf   被减数据缓冲
* @param[in] ：lengh 缓冲区大小
* @param[in] ：value 减数
* @return    ：无
* @Create    : 贺宁
* @Date      ：2021-05-20
* @Update    :
**********************************************************************/
void data_byte_sub(uint8 *pBuf, uint8 lengh, uint8 value)
{
    uint8 i = 0;

    for(i = 0; i < lengh; i++)
    {
        pBuf[i] -= value;
    }  
}

uint16 lcm_frame_encap(FRAME_INFO_T *pFrame, uint8 *frameBuf, uint16 maxLen)
{
    uint16  crc = 0;
    uint16 offset = 0;

    //帧起始标志
    frameBuf[offset++] = VF_BEGIN;

    //地址域
    memcpy(&frameBuf[offset], pFrame->addr, DL645_ADDR_LEN);
    offset += DL645_ADDR_LEN;
    
    frameBuf[offset++] = VF_BEGIN;

    //控制码
    frameBuf[offset++]  = pFrame->ctrl.value;

    //长度域
    frameBuf[offset++] = pFrame->lengh;

    //数据域
    memcpy(&frameBuf[offset], pFrame->pData, pFrame->lengh);
    offset += pFrame->lengh;

    crc = checksum_calc(&frameBuf[0], offset);
    frameBuf[offset++] = crc;

    //帧结束符
    frameBuf[offset++] = VF_END;

    return offset;
}


/**********************************************************************
* @name      : lc_make_frame
* @brief     ：模组接口协议组帧
* @param[in] ：
* @param[out]：
* @return    ：
* @Create    : 贺宁
* @Date      ：2020-7-10
* @Update    :
**********************************************************************/
uint16 lcm_modmsg_make(uint32 di, uint8 *sendBuf, uint16 buflen)
{
    uint16 offset = 0;
    uint8 tmpbuf[128] = {0};
    FRAME_INFO_T frameInfo = {0};

    //构造数据域
    //DI
    memcpy(&tmpbuf[offset], &di, sizeof(uint32));
    offset += sizeof(uint32);
    //操作码
    tmpbuf[offset++] = 0x00;
    tmpbuf[offset++] = 0x00;
    tmpbuf[offset++] = 0x00;
    tmpbuf[offset++] = 0x00;
    //DATA-无
    data_byte_add(&tmpbuf[0], offset, 0x33);
    //地址域
    frameInfo.addr[0] = 0x01;
    frameInfo.addr[1] = 0x00;
    frameInfo.addr[2] = 0x00;
    frameInfo.addr[3] = 0x00;
    frameInfo.addr[4] = 0x00;
    frameInfo.addr[5] = 0x00;
    //控制码    
    frameInfo.ctrl.value = 0x03;
    //长度域   
    frameInfo.lengh = offset;
    //数据域
    frameInfo.pData = tmpbuf;

    return lcm_frame_encap(&frameInfo, sendBuf, buflen);
}

/**********************************************************************
* @name      : lcm_upmsg_make
* @brief     ：升级消息组帧
* @param[in] ：
* @param[out]：
* @return    ：
* @Create    : 贺宁
* @Date      ：2022-4-16
* @Update    :
**********************************************************************/
uint16 lcm_upmsg_make(LC_FILE_TRANS_T *fileInfo, uint8 *sendBuf, uint16 buflen)
{
    uint16 offset = 0;
    uint8 tmpbuf[256] = {0};
    FRAME_INFO_T frameInfo = {0};
    uint16 encryptPos = 0;
    uint32 di = LCM_DI_FILETRANSMIT;
    uint8 wordPtr = 0; //4字节对齐位置 
    uint32 varAddr = 0; //变量地址

    //构造数据域
    
    //DI
    memcpy(&tmpbuf[offset], &di, sizeof(uint32));
    offset += sizeof(uint32);
    //操作码
    tmpbuf[offset++] = 0x00;
    tmpbuf[offset++] = 0x00;
    tmpbuf[offset++] = 0x00;
    tmpbuf[offset++] = 0x00;

    //DATA
    encryptPos = offset;

    //总帧数
    memcpy(&tmpbuf[offset], &fileInfo->nBlockNum, sizeof(uint16));
    offset += sizeof(uint16);

    //当前帧号
    memcpy(&tmpbuf[offset], &fileInfo->nBlockId, sizeof(uint16));
    offset += sizeof(uint16);

    //数据长度
    memcpy(&tmpbuf[offset], &fileInfo->nLengh, sizeof(uint16));
    offset += sizeof(uint16);
    
    //数据内容
    memcpy(&tmpbuf[offset], fileInfo->fileData, fileInfo->nLengh);
    offset += fileInfo->nLengh;

    //武高所算法库加密运算（加密起始位置应为4的倍数）
    varAddr = (uint32)&tmpbuf[encryptPos];
    wordPtr = (varAddr%4 == 0) ? 0: (4-varAddr%4);

    //先移动到4字节对齐位置，加密完之后，再移动回来
    memmove(&tmpbuf[encryptPos+wordPtr], &tmpbuf[encryptPos], offset-encryptPos);
    String_Encrypt(&tmpbuf[encryptPos+wordPtr], offset-encryptPos);
    memmove(&tmpbuf[encryptPos], &tmpbuf[encryptPos+wordPtr], offset-encryptPos);

//    //DATA使用算法库加密
//    String_Encrypt(&tmpbuf[encryptPos], offset-encryptPos);

    //数据域+0x33处理
    data_byte_add(&tmpbuf[0], offset, 0x33);
    
    //地址域
    frameInfo.addr[0] = 0x01;
    frameInfo.addr[1] = 0x00;
    frameInfo.addr[2] = 0x00;
    frameInfo.addr[3] = 0x00;
    frameInfo.addr[4] = 0x00;
    frameInfo.addr[5] = 0x00;
    //控制码    
    frameInfo.ctrl.value = 0x03;
    //长度域   
    frameInfo.lengh = offset;
    //数据域
    frameInfo.pData = tmpbuf;

    return lcm_frame_encap(&frameInfo, sendBuf, buflen);
}

int lcm_ctworkstate_parse(DB_CLIENT clientid, uint8 *apdu, uint32 apduLen, void *stdata)
{
    uint32 sOffset = 0;
    uint8  i = 0;
    int ret = ERR_OK;
    LC_WORK_STATUS_T workstatus;
    char phasename[3] = {'A','B','C'};

    MEMZERO(&workstatus, sizeof(LC_WORK_STATUS_T));

    LCMONIT_BUF_TRACE(apdu, apduLen, "lc ciphertext apdu.\n");    
    
    //只解密APDU中的DATA,解密后再解析
    String_Decrypt(apdu, apduLen);
    
    LCMONIT_BUF_DEBUG(apdu, apduLen, "lc plaintext apdu.\n");

    //此处不需要关心换算，跟模组协议的一致，在存储时再做
    for(i = 0; i < 3; i++)
    {
        workstatus.loopStatus[i] = apdu[sOffset++];                     //电流回路状态
        
        workstatus.envTemp[i] = BCD_change_to_long(&apdu[sOffset], 4);  //互感器环境温度
        sOffset += 2;

        workstatus.freqMax[i] = BCD_change_to_long(&apdu[sOffset], 6);  //频率最大值
        sOffset += 3;        

        workstatus.freqMin[i] = BCD_change_to_long(&apdu[sOffset], 6);  //频率最小值
        sOffset += 3;                

        workstatus.curValue[i] = BCD_change_to_long(&apdu[sOffset], 6); //工频电流有效值
        sOffset += 3;   

        workstatus.impeFreq1[i] = BCD_change_to_long(&apdu[sOffset], 4); //第一组相回路阻抗频率
        sOffset += 2;          

        workstatus.impeValue1[i] = BCD_change_to_long(&apdu[sOffset], 8); //第一组相回路阻抗模值
        sOffset += 4;           

        workstatus.impeAnge1[i] = BCD_change_to_long(&apdu[sOffset], 4);  //第一组相回路阻抗角度
        sOffset += 2;              

         workstatus.impeFreq2[i] = BCD_change_to_long(&apdu[sOffset], 4); //第二组相回路阻抗频率
        sOffset += 2;          

        workstatus.impeValue2[i] = BCD_change_to_long(&apdu[sOffset], 8); //第二组相回路阻抗模值
        sOffset += 4;           

        workstatus.impeAnge2[i] = BCD_change_to_long(&apdu[sOffset], 4);  //第二组相回路阻抗角度
        sOffset += 2;              

        workstatus.impeFreq3[i] = BCD_change_to_long(&apdu[sOffset], 4); //第三组相回路阻抗频率
        sOffset += 2;          

        workstatus.impeValue3[i] = BCD_change_to_long(&apdu[sOffset], 8); //第三组相回路阻抗模值
        sOffset += 4;           

        workstatus.impeAnge3[i] = BCD_change_to_long(&apdu[sOffset], 4);  //第三组相回路阻抗角度
        sOffset += 2;                      
    }
    
    memcpy(&g_workstatus[g_secNum-1], &workstatus, sizeof(LC_WORK_STATUS_T));
    
    for(i = 0; i < 3; i++)
    {
        LCMONIT_FMT_DEBUG("secNum %d, phase %c, loopState %d\n",g_secNum, phasename[i], workstatus.loopStatus[i]);
    }

    return ret;
}

int lcm_ctversion_parse(DB_CLIENT clientid, uint8 *apdu, uint32 apduLen, void *stdata)
{
    uint32 sOffset = 0;
    uint8  i = 0, j = 0;
    uint8 tmpDate[16] = {0};
    OOP_CT_VERTIONS_T ctVersion;

    memcpy(&ctVersion, stdata, sizeof(OOP_CT_VERTIONS_T));
    
    LCMONIT_BUF_TRACE(apdu, apduLen, "lc version ciphertext apdu.\n");    
    
    //只解密APDU中的DATA,解密后再解析
    String_Decrypt(apdu, apduLen);
    
    LCMONIT_BUF_DEBUG(apdu, apduLen, "lc version plaintext apdu.\n");
    
    for(i = 0; i < 3; i++)
    {
        ctVersion.ver[1+i].szType.nNum = 24; //互感器型号
        memcpy_r(&ctVersion.ver[1+i].szType.value, &apdu[sOffset], ctVersion.ver[1+i].szType.nNum);
        sOffset += ctVersion.ver[1+i].szType.nNum;

        //互感器ID, HEX
        ctVersion.ver[1+i].szId.nNum = 24; //互感器ID
        memcpy_r(&ctVersion.ver[1+i].szId.value, &apdu[sOffset], ctVersion.ver[1+i].szId.nNum);
        sOffset += ctVersion.ver[1+i].szId.nNum;

        //厂商代码
        memcpy_r(&ctVersion.ver[1+i].szFactory.value, &apdu[sOffset], 4);
        sOffset += 4;

        //软件版本号
        memcpy_r(&ctVersion.ver[1+i].szSoftVer.value, &apdu[sOffset], 4);
        sOffset += 4;

        //软件版本日期:BCB码
        memcpy_r(tmpDate, &apdu[sOffset], 3);
        for(j = 0; j < 3; j++)
        {
            hex_to_ascii(&ctVersion.ver[1+i].szSoftDate.value[j*2], tmpDate[j]);
        }
        sOffset += 3;      

        //硬件版本号
        memcpy_r(&ctVersion.ver[1+i].szHardVer.value, &apdu[sOffset], 4);
        sOffset += 4;

        //硬件版本日期:BCB码
        memcpy_r(tmpDate, &apdu[sOffset], 3);
        for(j = 0; j < 3; j++)
        {
            hex_to_ascii(&ctVersion.ver[1+i].szHardDate.value[j*2], tmpDate[j]);
        }        
        sOffset += 3;          

        //厂商扩展信息
        memcpy_r(&ctVersion.ver[1+i].szExtend.value, &apdu[sOffset], 8);
        sOffset += 8;              
    }

    memcpy(stdata, &ctVersion, sizeof(OOP_CT_VERTIONS_T));
    
    return ERR_OK;
}

int lcm_coreversion_parse(DB_CLIENT clientid, uint8 *apdu, uint32 apduLen, void *stdata)
{
    uint32 sOffset = 0;
    uint8  i = 0;
    uint8 tmpDate[16] = {0};    
    OOP_CT_VERTIONS_T ctVersion;

    memcpy(&ctVersion, stdata, sizeof(OOP_CT_VERTIONS_T));
    
    LCMONIT_BUF_TRACE(apdu, apduLen, "lc version ciphertext apdu.\n");    
    
    //只解密APDU中的DATA,解密后再解析
    String_Decrypt(apdu, apduLen);
    
    LCMONIT_BUF_DEBUG(apdu, apduLen, "lc version plaintext apdu.\n");
    
    ctVersion.ver[0].szType.nNum = 24; //算法板型号
    memcpy_r(&ctVersion.ver[0].szType.value, &apdu[sOffset], ctVersion.ver[0].szType.nNum);
    sOffset += ctVersion.ver[0].szType.nNum;

    //算法板ID,此处698上行需要的是octet-string
    ctVersion.ver[0].szId.nNum = 24; 
    memcpy_r(&ctVersion.ver[0].szId.value, &apdu[sOffset], ctVersion.ver[0].szId.nNum);
    sOffset += ctVersion.ver[0].szId.nNum;    

    //厂商代码
    memcpy_r(&ctVersion.ver[0].szFactory.value, &apdu[sOffset], 4);
    sOffset += 4;

    //软件版本号
    memcpy_r(&ctVersion.ver[0].szSoftVer.value, &apdu[sOffset], 4);
    sOffset += 4;

    //软件版本日期:BCD码
    memcpy_r(tmpDate, &apdu[sOffset], 3);
    for(i = 0; i < 3; i++)
    {
        hex_to_ascii(&ctVersion.ver[0].szSoftDate.value[i*2], tmpDate[i]);
    }    
    sOffset += 3;      

    //硬件版本号
    memcpy_r(&ctVersion.ver[0].szHardVer.value, &apdu[sOffset], 4);
    sOffset += 4;

    //硬件版本日期:BCD码
    memcpy_r(tmpDate, &apdu[sOffset], 3);
    for(i = 0; i < 3; i++)
    {
        hex_to_ascii(&ctVersion.ver[0].szHardDate.value[i*2], tmpDate[i]);
    }
    sOffset += 3;          

    //厂商扩展信息
    memcpy_r(&ctVersion.ver[0].szExtend.value, &apdu[sOffset], 8);
    sOffset += 8;              

    memcpy(stdata, &ctVersion, sizeof(OOP_CT_VERTIONS_T));
    
    return ERR_OK;
}

int lcm_filetrans_parse(DB_CLIENT clientid, uint8 *apdu, uint32 apduLen, void *stdata)
{
    uint32 sOffset = 0;
    uint16 blockID = 0;
    LC_FILE_TRANS_T *pFileInfo = (LC_FILE_TRANS_T *)stdata;

    UPDATE_BUF_TRACE(apdu, apduLen, "lc filetrans ciphertext apdu.\n");    
    
    //只解密APDU中的DATA,解密后再解析
    String_Decrypt(apdu, apduLen);
    
    UPDATE_BUF_DEBUG(apdu, apduLen, "lc filetrans plaintext apdu.\n");

    memcpy(&blockID, &apdu[sOffset], sizeof(uint16));
    sOffset += sizeof(uint16);

    if(pFileInfo->nBlockId != blockID)
    {
        return ERR_NORMAL;
    }
    
    return ERR_OK;
}


/**********************************************************************
* @name      : lcm_frame_decap
* @brief     ：接收消息解析成 MAPMSG_INFO_T
* @param[in] ：uint8 *pBuf              接受到的报文
               uint16 bufLen            报文长度
* @param[out]：MAPMSG_INFO_T **ppMsgOut 解析出来的消息
* @return    ：0-成功/错误码
* @Create    : 
* @Date      ：2020-1-3
* @Update    :
**********************************************************************/
int lcm_frame_decap(FRAME_INFO_T *pFrame, uint8 *pBuf, uint16 bufLen)
{
    uint8 readcs = 0;
    uint8 calccs = 0;

    if(bufLen < DL645_FRAME_MIN)
    {    
        LCMONIT_FMT_DEBUG("frame lengh check failed, lengh-%d < minlengh-%d.\n", bufLen, DL645_FRAME_MIN);
        return -1;
    }
        
    //地址域
    memcpy(pFrame->addr, &pBuf[DL645_ADDR], DL645_ADDR_LEN);
    
    //控制码
    pFrame->ctrl.value = pBuf[DL645_CTRL];

    //长度域
    pFrame->lengh = pBuf[DL645_LEGH];

    //数据域
    memcpy(pFrame->pData, &pBuf[DL645_DATA], pFrame->lengh);

    //校验码
    readcs = pBuf[DL645_DATA+pFrame->lengh];
    calccs = checksum_calc(&pBuf[0], DL645_DATA+pFrame->lengh);
    if(readcs != calccs)
    {
        pFrame->lengh = 0;
        LCMONIT_FMT_DEBUG("checksum_calc failed, readcs-%d != calccs-%d.\n", readcs, calccs);
        return -2;
    }

    //数据域按字节减33H处理
    data_byte_sub(pFrame->pData, pFrame->lengh, 0x33);

    return 0;
}

/**********************************************************************
* @name      : lcm_modmsg_parse
* @brief     ：算法板协议解析
* @param[in] ：
* @param[out]：
* @return    ：
* @Create    : 贺宁
* @Date      ：2020-7-10
* @Update    :
**********************************************************************/
int lcm_modmsg_parse(DB_CLIENT clientid, uint8 *recvBuf, uint16 buflen, void *stdata)
{
    int            ret = 0;
    uint16         offset = 0;
    FRAME_INFO_T   frameInfo = {0};
    uint32         di = 0;
    uint8          i = 0;
    int            tempret = 0;
    uint32 sOffset = 0;
    uint8 temp[DL645_DATA_MAX] = {0}; //接收报文的数据域

    frameInfo.pData = temp;
        
    //找到帧起始位
    while(offset < buflen)
    {
        if(VF_BEGIN == recvBuf[offset])
        {
            tempret = lcm_frame_decap(&frameInfo, recvBuf + offset, buflen - offset);
            if(tempret != 0)
            {
                LCMONIT_BUF_DEBUG(recvBuf + offset, buflen - offset, "lcm_frame_decap failed, tempret(%d)\n", tempret);
            }
            break;
        }
        offset++;
    }

    if(frameInfo.lengh == 0)
    {
        LCMONIT_BUF_DEBUG(temp, sOffset, "check null pointer.\n");
        return ERR_PNULL;
    }

    //功能单一，不考虑多帧
    memcpy(&di, &temp[sOffset], sizeof(uint32));
    sOffset += sizeof(uint32);

    for(i = 0; i < g_ctalgParse_Max; i++)
    {
        if((g_ctalgParse[i].di == di)&&(g_ctalgParse[i].apduGet != NULL))
        {
            ret = g_ctalgParse[i].apduGet(clientid, &temp[sOffset], frameInfo.lengh-sOffset, stdata);
            break;
        }
    }

    if(i == g_ctalgParse_Max)
    {
        ret = ERR_OPTION;
    }
    
    return ret;
}

#endif

#if DESC("算法模块收发",1)

/**
*********************************************************************
* @brief：     lc_version_get
* @param[in]： NA
* @param[out]：NA
* @return：     void
*********************************************************************
*/
int lcm_version_get(void *pDev, DB_CLIENT clientid, int socketfd)
{
    int ret = 0;
    uint8 sendbuf[128] = {0};
    uint16 sendlen = 0;   //发送报文长度
    uint8 sRecvbuf[1024] = {0};//单次接收
    uint16 sRecvLen = 0; //单次接收长度
    OOP_CT_VERTIONS_T ctVersion;
    LC_UART_VDEV_T *tmpUart = (LC_UART_VDEV_T *)pDev;
    uint32 useTime = 0;
    static uint16 retryCnt = 0;

    if(pDev == NULL)
    {
        LCMONIT_FMT_DEBUG("pDev is NULL\n");
        return ERR_PNULL;
    }

    MEMZERO(&ctVersion, sizeof(OOP_CT_VERTIONS_T));
    ctVersion.nNum = 4;
        
    sendlen = lcm_modmsg_make(LCM_DI_CTVERSION, sendbuf, sizeof(sendbuf));

    LCMONIT_BUF_DEBUG(sendbuf, sendlen, "lc_uart_send: ctversion.\n");

    sRecvLen = uart_transmit(pDev, sendbuf, sendlen, sRecvbuf, sizeof(sRecvbuf));
    if(sRecvLen != 0)
    {
        retryCnt = 0;
        ret = lcm_modmsg_parse(clientid, sRecvbuf, sRecvLen, &ctVersion);
        if(ret != 0)
        {
            LCMONIT_BUF_DEBUG(sRecvbuf, sRecvLen, "lc_modmsg_parse failed,ret(%d).\n", ret);
            return ERR_NORMAL;
        }
    }
    else
    {
        if(++retryCnt >= LCM_SEND_RETRY)
        {
            retryCnt = 0;
            return ERR_INVALID;
        }
    }

    memset(sendbuf, 0, sizeof(sendbuf));
    sendlen = lcm_modmsg_make(LCM_DI_COREVERSION, sendbuf, sizeof(sendbuf));

    LCMONIT_BUF_DEBUG(sendbuf, sendlen, "lc_uart_send: coreversion.\n");

    sRecvLen = uart_transmit(pDev, sendbuf, sendlen, sRecvbuf, sizeof(sRecvbuf));
    if(sRecvLen != 0)
    {
        retryCnt = 0;
        ret = lcm_modmsg_parse(clientid, sRecvbuf, sRecvLen, &ctVersion);
        if(ret != 0)
        {
            LCMONIT_BUF_DEBUG(sRecvbuf, sRecvLen, "lc_modmsg_parse failed,ret(%d).\n", ret);
            return ERR_NORMAL;
        }
    }
    else
    {
        if(++retryCnt >= LCM_SEND_RETRY)
        {
            retryCnt = 0;
            return ERR_INVALID;
        }
    }

    useTime = (tmpUart->recvTm.tv_sec - tmpUart->sendTm.tv_sec)*1000 + (tmpUart->recvTm.tv_nsec - tmpUart->sendTm.tv_nsec)/1000000;

    LCMONIT_FMT_DEBUG("comm use: %d ms, success rate: recv-%d/send-%d = %.2f%%.\n", 
        useTime, tmpUart->recvCnt, tmpUart->sendCnt, tmpUart->sendCnt==0 ? 0: tmpUart->recvCnt*100.0/tmpUart->sendCnt);

    //版本信息存储
    ret = lc_versioninfo_store(clientid, &ctVersion); 

    return ret;
}

/**
*********************************************************************
* @brief：     lc_workstate_get
* @param[in]： NA
* @param[out]：NA
* @return：     void
*********************************************************************
*/
int lcm_workstate_get(void *pDev, DB_CLIENT clientid, int socketfd)
{
    int ret = 0;
    uint8 sendbuf[128] = {0};
    uint16 sendlen = 0;   //发送报文长度
    uint8 sRecvbuf[1024] = {0};//单次接收
    uint16 sRecvLen = 0; //单次接收长度
    static uint16 retryCnt = 0;
    LC_UART_VDEV_T *tmpUart = (LC_UART_VDEV_T *)pDev;
    uint32 useTime = 0;

    if(pDev == NULL)
    {
        LCMONIT_FMT_DEBUG("pDev is NULL\n");
        return ERR_PNULL;
    }
    
    sendlen = lcm_modmsg_make(LCM_DI_WORKSTATUS, sendbuf, sizeof(sendbuf));

    LCMONIT_BUF_DEBUG(sendbuf, sendlen, "lc_uart_send: workstate.\n");

    sRecvLen = uart_transmit(pDev, sendbuf, sendlen, sRecvbuf, sizeof(sRecvbuf));
    if(sRecvLen != 0)
    {
        retryCnt = 0;
        ret = lcm_modmsg_parse(clientid, sRecvbuf, sRecvLen, NULL);
        if(ret != 0)
        {
            LCMONIT_BUF_DEBUG(sRecvbuf, sRecvLen, "lc_modmsg_parse failed,ret(%d).\n", ret);
            return ERR_NORMAL;
        }
    }
    else
    {
        if(++retryCnt >= LCM_SEND_RETRY)
        {
            retryCnt = 0;
            return ERR_INVALID;
        }
    }

    useTime = (tmpUart->recvTm.tv_sec - tmpUart->sendTm.tv_sec)*1000 + (tmpUart->recvTm.tv_nsec - tmpUart->sendTm.tv_nsec)/1000000;

    LCMONIT_FMT_DEBUG("comm use: %d ms, success rate: recv-%d/send-%d = %.2f%%.\n", 
        useTime, tmpUart->recvCnt, tmpUart->sendCnt, tmpUart->sendCnt==0 ? 0: tmpUart->recvCnt*100.0/tmpUart->sendCnt);

    return ret;
}

#endif

#if DESC("升级处理",PRODUCT_SCU)

uint8  g_fileData[LC_UPPACK_SIZE];
/**********************************************************************
* @name      : lc_upfile_scan
* @brief     ：升级文件扫描
* @param[in] ：void *pArg     线程句柄
               void *pPublic  公有数据
               void *pPrivate 私有数据
* @param[out]：
* @return    ：延时时间 us
* @Create    : 贺宁
* @Date      ：2021-09-07
* @Update    :
**********************************************************************/
int lc_upfile_scan(void *pPublic, void *pPrivate)
{
    LC_MONITOR_DATA_T *ptData = (LC_MONITOR_DATA_T *)pPrivate;
    LC_FILE_TRANS_T *pFileInfo = ptData->fileInfo;
    int size = 0;

    if(0 == access(LC_UPDATE_FILE, F_OK))
    {
        size = file_size_get(LC_UPDATE_FILE);
        UPDATE_FMT_DEBUG("检测到升级文件 %s，大小 %d B, 开始对回路巡检功能单元升级.\n", LC_UPDATE_FILE, size);
        
        memset(pFileInfo, 0, sizeof(LC_FILE_TRANS_T));
        pFileInfo->fileData = g_fileData;
    
        pFileInfo->nEndLengh = size % LC_UPPACK_SIZE;
        pFileInfo->nBlockNum = size/LC_UPPACK_SIZE;
        
        if(pFileInfo->nEndLengh)
        {
            pFileInfo->nBlockNum += 1;
        }
    
        ptData->fd = -1;
        ptData->fd = open(LC_UPDATE_FILE, O_RDONLY);
        if(ptData->fd < 0)
        {
            UPDATE_FMT_DEBUG("lc file open failed, %s.\n", strerror(errno));
            return ERR_NORMAL;
        }

        return ERR_OK;
    }

    return ERR_NORMAL;
}

/**********************************************************************
* @name      : lc_update_trans
* @brief     ：升级包发送
* @param[in] ：void *pPublic    公有数据
* @param[out]：void **ppPrivate 私有数据
* @return    ：0-成功/错误码
* @Create    : 贺宁
* @Date      ：2021-09-07
* @Update    :
**********************************************************************/
int lc_update_trans(void *pPublic, void *pPrivate)
{
    LC_MONITOR_DATA_T *ptData = (LC_MONITOR_DATA_T *)pPrivate;
    LC_FILE_TRANS_T *pFileInfo = ptData->fileInfo;
    LC_UART_VDEV_T *pUart = ptData->pUart;
    uint8 sendbuf[256] = {0};
    uint16 sendLen = 0;
    int recvLen = 0;
    uint8 recvBuf[256] = {0};
    int ret = 0;
     
    lseek(ptData->fd, pFileInfo->nBlockId*LC_UPPACK_SIZE, SEEK_SET);

    if(pFileInfo->nBlockId == (pFileInfo->nBlockNum-1))
    {
        pFileInfo->nLengh = pFileInfo->nEndLengh;
    }
    else
    {
        pFileInfo->nLengh = LC_UPPACK_SIZE;
    }

    read(ptData->fd, pFileInfo->fileData, pFileInfo->nLengh);

    sendLen = lcm_upmsg_make(pFileInfo, sendbuf, 256);
    UPDATE_BUF_DEBUG(sendbuf, sendLen, "总帧数[%d] 帧序号[%d] 发送:\n", pFileInfo->nBlockNum , pFileInfo->nBlockId);

    recvLen = uart_transmit(pUart, sendbuf, sendLen, recvBuf, sizeof(recvBuf));
    if(recvLen != 0)
    {
        ret = lcm_modmsg_parse(0, recvBuf, recvLen, pFileInfo);
        if(ret != 0)
        {
            UPDATE_BUF_DEBUG(recvBuf, recvLen, "lc_update_parse failed,ret(%d).\n", ret);
            return ret;
        }
    }

    UPDATE_FMT_DEBUG("帧序号[%d] 发送成功.\n",pFileInfo->nBlockId);

    pFileInfo->nBlockId++;

    return 0;
}

/**********************************************************************
* @name      : lc_transfer_scan
* @brief     ：透传扫描
* @param[in] ：void *pArg     线程句柄
               void *pPublic  公有数据
               void *pPrivate 私有数据
* @param[out]：
* @return    ：延时时间 us
* @Create    : 贺宁
* @Date      ：2021-09-07
* @Update    :
**********************************************************************/
int lc_data_transfer(void *pPublic, void *pPrivate)
{
    LC_MONITOR_DATA_T *ptData = (LC_MONITOR_DATA_T *)pPrivate;
    LC_UART_VDEV_T *pUart = ptData->pUart;
    uint8 sendbuf[288] = {0};
    uint16 sendLen = 0;
    UART_SEND_DATA_T tUratSend;
    uint8   dataBuf[512] = {0}; 
    uint32  dataLen = 0;

    memcpy(sendbuf, ptData->transBuf, ptData->transLen);
    sendLen = ptData->transLen;

    LCMONIT_BUF_DEBUG(sendbuf, sendLen, "透传发送:");

    tUratSend.datalen = uart_transmit(pUart, sendbuf, sendLen, tUratSend.data, UART_DATA_MAXLEN);
    if(tUratSend.datalen != 0)
    {
        LCMONIT_BUF_DEBUG(tUratSend.data, tUratSend.datalen, "透传接收:");
        
        tUratSend.port = ptData->transport;
        dataLen = uart_axdr_data_send_req_package(&tUratSend, dataBuf, 512);

        lc_send_uartmsg(0, UART_IOP_SEND_DATA, dataBuf, dataLen);
    }

    return 0;
}

#endif

#if DESC("回路巡检状态机",1)

/**
*********************************************************************
* @brief：     lc_monitor_collect
* @param[in]： NA
* @param[out]：NA
* @return：     void
*********************************************************************
*/
void lc_monitor_deal(void *pPublic, void *pPrivate)
{
//    LC_APPDATA_T *pAppData = (LC_APPDATA_T *)pPublic;
    LC_MONITOR_DATA_T *ptMsgData = (LC_MONITOR_DATA_T *)pPrivate;
    void *pDev = ptMsgData->pUart;
    DB_CLIENT clientid = ptMsgData->clientid;
    int socketfd = ptMsgData->tasksocket;
    int ret = 0;
    LC_UART_VDEV_T *pUart = (LC_UART_VDEV_T*)pDev;
    static uint8 retry = 0;
#ifdef PRODUCT_SCU    
    LC_FILE_TRANS_T *pFileInfo = ptMsgData->fileInfo;

    //文件存在则开始升级
    if(ERR_OK == lc_upfile_scan(pPublic, pPrivate))
    {
        ptMsgData->status = LCM_FUPG;
    }  

    //检测串口是否异常
    if(uart_param_get(pUart->fd, &pUart->param) == FALSE)
    {
        ptMsgData->status = LCM_OPEN;
    } 

    //检测是否监听从模式串口
    if((g_secNum % MOD_SAMPLE_INTERV == 0) && (ptMsgData->slavereg != UART_RST_SUCCESS))
    {
        lc_uart_mode_check(pPrivate);
    }      
#endif

#ifdef PRODUCT_ECU    

    //收到模块拔出事件
    if((g_lcm_slot == 0) && (ptMsgData->status != LCM_GETM))
    {
        ptMsgData->status = LCM_GETM;
        uart_close(pDev);
    }
#endif
                
    switch(ptMsgData->status)
    {
        case LCM_IDLE:
        {      
            LCMONIT_FMT_TRACE("LCM_IDLE.\n");
            //收到模块插入事件
            if(g_mod_status_chg)
            {
                g_mod_status_chg = FALSE;
                ptMsgData->status = LCM_GETM;
            }

        }break;

        case LCM_OPEN:
        {
            LCMONIT_FMT_TRACE("LCM_OPEN.\n");
            uart_close(pDev);

            if(0 == access(pUart->desc, F_OK))
            {
                LCMONIT_FMT_DEBUG("%s is exist\n", pUart->desc);

                sleep_no_interrupt(2);

                ret = uart_open(pDev);
                if(ret == ERR_OK)
                {
                    ptMsgData->status = LCM_INIT;
                }
            }
        }break;

        case LCM_INIT:
        {
            LCMONIT_FMT_TRACE("LCM_INIT.\n");
#ifdef PRODUCT_ECU   
            ret = lc_version_get(pDev, clientid, socketfd);
#else
            ret = lcm_version_get(pDev, clientid, socketfd);
#endif
            if(ret != ERR_OK)
            {
                if(ret == ERR_INVALID)
                {
                    ptMsgData->status = LCM_OPEN;
                }
                else
                {
                    sleep_no_interrupt(5);
                }
            }
            else
            {
                ptMsgData->status = LCM_SEND;
            }
        }break;

        case LCM_SEND:
        {
            LCMONIT_FMT_TRACE("LCM_SEND.\n");
            if(g_secNum % MOD_SAMPLE_INTERV == 0)
            {
#ifdef PRODUCT_ECU 
                ret = lc_workstate_get(pDev, clientid, socketfd);
#else
                ret = lcm_workstate_get(pDev, clientid, socketfd);
#endif
                if(ret != ERR_OK)
                {
                    if(ret == ERR_INVALID)
                    {
                        ptMsgData->status = LCM_OPEN;
                    }
                }
            }

            if(g_secNum % MOD_SAMPLE_PERIOD == 0)
            {
                //首次版本未读取成功，重试读取版本
                if(g_verNeedRetry == TRUE)
                {
                    g_verNeedRetry = FALSE;
#ifdef PRODUCT_ECU   
                    ret = lc_version_get(pDev, clientid, socketfd);
#else
                    ret = lcm_version_get(pDev, clientid, socketfd);
#endif
                }  
            }
        }break;
            
        case LCM_GETM:
        {
            LCMONIT_FMT_TRACE("LCM_GETM.\n");
            if(g_lcm_slot != 0)
            {
                sprintf(pUart->halname, "%s%d_1",MOD_DEV_DESC_PRFIX, g_lcm_slot); 
                ret = uart_open(pDev);
                if(ret == ERR_OK)
                {
                    ptMsgData->status = LCM_INIT;
                }
            }
            
            //过秒执行，1s查一次，查10次,查不到等待模组插入
            if(retry++ < 10 )
            {
                lc_get_map_slot(socketfd, MOD_LC_TYPE_PRFIX);                
            }
            else
            {   
                ptMsgData->status = LCM_IDLE;
                retry = 0;
            }
        }break;            
#ifdef PRODUCT_SCU        
        case LCM_FUPG:
        {
            LCMONIT_FMT_TRACE("LCM_FUPG.\n");
            if(pFileInfo->nBlockId >= pFileInfo->nBlockNum)
            {            
                remove(LC_UPDATE_FILE);
                UPDATE_FMT_DEBUG("回路巡检功能单元升级完成.\n");
                ptMsgData->status = LCM_INIT;
            }
            else
            {
                lc_update_trans(pPublic, pPrivate);
                ptMsgData->status = LCM_FUPG;
            }
        }break;

        case LCM_TRAN:
        {
            LCMONIT_FMT_TRACE("LCM_TRAN.\n");
            lc_data_transfer(pPublic, pPrivate);
            ptMsgData->status = LCM_SEND;
        }break;
#endif
        default:break;
    }
    
    return;
}

#endif

