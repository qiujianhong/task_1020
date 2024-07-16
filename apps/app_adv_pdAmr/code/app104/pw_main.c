/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved.
* @brief： 功能描述
* @date： 2020-01-03
*********************************************************************
*/

#include "pw_redef.h"
#include "pw_main.h"
#include "pw_common.h"
#include "pw_protocol.h"
#include "pw_debug.h"
#include "pw_lib.h"
#include "framecomm.h"
#include <sys/prctl.h>

#ifdef AREA_ZHEJIANG
#include "../area/zhejiang/pw_protocol_zhj.h"
#include "../area/zhejiang/pw_common_zhj.h"
#endif

/*******************************************************************************
* 全局变量声明
*******************************************************************************/
THREAD_ARG_T g_sUpRecvDealThreadArg;         //上行主站控制任务线程参数
THREAD_ARG_T g_sUpSendDealThreadArg;         //上行主站上报任务线程参数
THREAD_ARG_T g_sDownDeviceThreadArg;         //下行设备通信线程参数
THREAD_ARG_T g_sEventCheckThreadArg;         //交采模块事件检测线程参数
THREAD_ARG_T g_sMonitStatThreadArg;          //监测统计线程参数

uint8 g_nUpRecvDealThreadFlag = 0;               //主站控制任务处理线程标志
uint8 g_nUpSendDealThreadFlag = 0;               //上报主站任务处理线程标志
uint8 g_nDownDeviceThreadFlag = 0;               //下行设备线程标志
uint8 g_nEventCheckThreadFlag = 0;               //交采模块事件检测线程标志
uint8 g_nMonitStatThreadFlag = 0;                //监测统计线程标志

uint8 g_nUpRecvType = 0;  //0:以明文方式接收104报文(用于测试); 1:以密文方式接收104报文

/*******************************************************************************
* 数据结构设计
*******************************************************************************/

//线程函数运行流程
typedef enum
{
    THREAD_RUN_INIT = 0,
    THREAD_RUN_UPDATE,//更新
    THREAD_RUN_RECV,
    THREAD_RUN_CONFIRM,//确认帧
    THREAD_RUN_SEND,
    THREAD_RUN_CLOSE,
}THREAD_RUN_STEP_E;


/*******************************************************************************
* 函数名称: event_check_thread
* 函数功能: 事件检测线程
* 输入参数：
* 输出参数：
* 返 回 值:
*******************************************************************************/
void *event_check_thread(void *arg)
{
    FRAME104_FMT_DEBUG("****** event_check_thread() start... ****** \n");

    while(1)
    {
        g_nEventCheckThreadFlag = 1;  //用于检测线程运行是否正常
        
        if(!g_bGlobalInitFlag)
        {
            sleep(1);
            continue;
        }



        sleep(60);
    }
}

#ifdef AREA_ZHEJIANG

/*******************************************************************************
* 函数名称: up_recv_deal_thread(浙江模式)
* 函数功能: 主站控制任务处理线程
* 输入参数：
* 输出参数：
* 返 回 值:
*******************************************************************************/
void *up_recv_deal_thread_zhj(void *arg)
{
    FRAME104_FMT_DEBUG("****** up_recv_deal_thread_zhj()  start... ****** \n");

    //线程名
	char ThreadName[30] = {0};
    sprintf(ThreadName, "104upzj");
    prctl(PR_SET_NAME,ThreadName);

    THREAD_RUN_STEP_E emRunStep = THREAD_RUN_INIT;  //运行状态
    uint8   RecvBuf[MAX_MSG_LEN] = {0};  //接收缓冲区
    uint8   SendBuf[MAX_MSG_LEN] = {0};  //发送缓冲区
    int32   RecvLen = 0, SendLen = 0;  //接收、发送数据长度
    uint8   nPipeIndex = 0;  //收发通道号
    uint8  nAppType = APP_TYPE_NOMMAL;  //主要用于加密类型
    uint8  nNetWorkFlag = PRO_DATA_TRANSMIT;  //链路连接状态标志
    int    ret = -1;

    uint8   SendBufSecure[DA_ESAM_MAX_LEN] = {0};  //加密之后的发送缓冲区
    int32   SendLenSecure = 0;          //加密之后的发送数据长度

    TOTAL_104_FRAME_T stTotal104Frame;
    memset(&stTotal104Frame, 0, sizeof(TOTAL_104_FRAME_T));
    int nTotal104FrameLen = 0;
    uint16 nTotal104FramePipe = 0;
    uint8  nSendFlag = 0;//判断是否存在连续发送情况

    while(1)
    {
        g_nUpRecvDealThreadFlag = 1;  //用于检测线程运行是否正常
        
        if(!g_bGlobalInitFlag)
        {
            sleep(1);
            continue;
        }

        time_manage();  //时间管理刷新
        nSendFlag = 0;//判断是否存在连续发送情况
        
        switch (emRunStep)
        {
            case THREAD_RUN_INIT:
            {
                nAppType = APP_TYPE_NOMMAL;
                emRunStep = THREAD_RUN_RECV;  //打开成功转至接收处理
                usleep(10000);
            }
            case THREAD_RUN_RECV:
            {
                //读共享的接收缓冲区
                memset(RecvBuf, 0x00, sizeof(RecvBuf));
                RecvLen = advPd_get_commmsg(&nPipeIndex, &nNetWorkFlag, RecvBuf);
                g_nNetWorkFlag = PRO_DATA_TRANSMIT;
                
                if (nNetWorkFlag == PROT_OPEN_CHANNLE)
                    g_bInitEndFlag = 0; /* 刚打开通道，主站还没有发送启动传输报文 */
                if (RecvLen > 0)
                {
                    //记录最新的收发通道号
                    g_nGlobalPipe = nPipeIndex;

                    //记录报文
                    FRAME104_FMT_TRACE("控制任务 接收 up_recv_deal_thread_zhj() THREAD_RUN_RECV RecvLen:%d, nPipeIndex:%d \n", RecvLen, nPipeIndex);
                    FRAME104_BUF_TRACE(RecvBuf, RecvLen, "报文: ");

                    //报文解析
                    SendLen = security_unpackframe(g_nGlobalPipe, RecvBuf, RecvLen, SendBuf);  //调用解密接口
                }

                //清接收缓冲区
                memset(RecvBuf, 0, sizeof(RecvBuf));
                RecvLen = 0;

                //转入数据发送处理
                emRunStep = THREAD_RUN_CONFIRM;
                usleep(5000);
            }
            case THREAD_RUN_CONFIRM:
            {
                //写共享的发送缓冲区(主站控制命令的应答报文)
                if(SendLen > 0)
                {
                    //记录报文
                    FRAME104_FMT_TRACE("控制任务 发送 up_recv_deal_thread_zhj() THREAD_RUN_SEND SendLen:%d, nPipeIndex:%d \n", SendLen, nPipeIndex);
                    FRAME104_BUF_TRACE(SendBuf, SendLen, "报文: ");

                    //写共享的发送缓冲区
                    advPd_send_104msg(nPipeIndex, PRO_DATA_TRANSMIT, nAppType, SendBuf, SendLen);
                    PRTL_BUF_DEBUG(SendBuf, SendLen, "SECURITY 发送报文,pipe:%u,", nPipeIndex);
                }

                //清发送缓冲区
                memset(SendBuf, 0, sizeof(SendBuf));
                SendLen = 0;

                //转入数据接收处理
                emRunStep = THREAD_RUN_SEND;
                usleep(5000);
            }
            case THREAD_RUN_SEND:
            {
                // 发送初始化结束命令
                if (g_bInitEndFlag)
                {
                    SendLen = i_frame_data_init_end_zhj(SendBuf);
                    
                    advPd_send_104msg(g_nGlobalPipe, PRO_DATA_TRANSMIT, nAppType, SendBuf, SendLen);  //调用发送接口
                    PRTL_BUF_DEBUG(SendBuf, SendLen, "SECURITY 发送报文,pipe:%u,", g_nGlobalPipe);
                    g_bInitEndFlag = 0;
                }
                
                //调用故障信息事件生成和检测接口
                if(1)//if(g_bDataSendFlag == 2)
                {
                    if(g_sTaskRunStep.nTaskStep == STEP_CONT_DEFAULT)//首先判断当前任务是否空闲
                    {
                        event_happen_proc_zhj(g_sEventDbClient);  //事件生成接口
                        ret = event_check_faults_zhj();  //判断是否有事件生成
                        if(ret > 0)
                        {
                            FRAME104_FMT_TRACE("up_recv_deal_thread_zhj() 检测到故障事件生成, 需要上报! ret=%d\n", ret);
                            g_bFaultChanged = 1;
                        }
                    }
                }
                usleep(5000);
                
                //上报安全证书时不容许进行其它发送
                if ((SendLen = security_report(&nPipeIndex, SendBuf)) >= 0)
                {
                    if (SendLen > 0) {
                        advPd_send_104msg(nPipeIndex, PRO_DATA_TRANSMIT, 0, SendBuf, SendLen);  //调用发送接口
                        PRTL_BUF_DEBUG(SendBuf, SendLen, "SECURITY 发送报文,pipe:%u,", nPipeIndex);
                    }

                    break;
                }
                usleep(5000);

                //SOE缓存区报文
                if(g_nNetWorkFlag == PRO_DATA_TRANSMIT)
                {
                    nTotal104FrameLen = pw_lock_queue_read(&g_stSoeBuff, (uint8 *)&stTotal104Frame, &nTotal104FramePipe);
                    if(nTotal104FrameLen > 0)
                    {
                        SendLen = i_frame_data_update_zhj(stTotal104Frame, SendBuf);

                        //写共享的发送缓冲区
                        if(SendLen > 0)
                        {
                            //记录报文
                            FRAME104_FMT_TRACE("SOE/COS/FAULT缓存报文 上报 up_recv_deal_thread_zhj() SendLen:%d, nPipeIndex:%d\n", SendLen, g_nGlobalPipe);
                            FRAME104_BUF_TRACE(SendBuf, SendLen, "报文: ");

                            //写共享的发送缓冲区
                            SendLenSecure = security_packappframe(g_nGlobalPipe, nAppType, SendBuf, SendLen, SendBufSecure);  //调用加密接口
                            if(SendLenSecure > 0)
                            {
                                advPd_send_104msg(g_nGlobalPipe, PRO_DATA_TRANSMIT, nAppType, SendBufSecure, SendLenSecure);  //调用发送接口
                                PRTL_BUF_DEBUG(SendBufSecure, SendLenSecure, "SECURITY 发送报文,pipe:%u,", g_nGlobalPipe);
                            }
                        }

                        nSendFlag = 1;
                    }
                }
                usleep(5000);

                //定时主动上报任务
                if(g_nNetWorkFlag == PRO_DATA_TRANSMIT)
                {
                    timing_active_send_zhj();
                    SendLen = timing_active_run_zhj(&nPipeIndex, SendBuf, &nAppType);

                    //写共享的发送缓冲区
                    if(SendLen > 0)
                    {
                        //记录报文
                        FRAME104_FMT_TRACE("数据冻结与上报 上报 up_recv_deal_thread_zhj() SendLen:%d, nPipeIndex:%d\n", SendLen, nPipeIndex);
                        FRAME104_BUF_TRACE(SendBuf, SendLen, "报文: ");

                        //写共享的发送缓冲区
                        SendLenSecure = security_packappframe(nPipeIndex, nAppType, SendBuf, SendLen, SendBufSecure);  //调用加密接口
                        if(SendLenSecure > 0)
                        {
                            advPd_send_104msg(nPipeIndex, PRO_DATA_TRANSMIT, nAppType, SendBufSecure, SendLenSecure);  //调用发送接口
                            PRTL_BUF_DEBUG(SendBufSecure, SendLenSecure, "SECURITY 发送报文,pipe:%u,", nPipeIndex);
                        }

                        nSendFlag = 1;
                    }
                }
                usleep(5000);
                
                //创建任务与任务处理
                if(g_nNetWorkFlag == PRO_DATA_TRANSMIT)
                {
                    report_task_create_zhj();
                    SendLen = report_task_run_zhj(&nPipeIndex, SendBuf, &nAppType);

                    //写共享的发送缓冲区
                    if(SendLen > 0)
                    {
                        //记录报文
                        FRAME104_FMT_TRACE("任务处理与上报 上报 up_recv_deal_thread_zhj() SendLen:%d, nPipeIndex:%d\n", SendLen, nPipeIndex);
                        FRAME104_BUF_TRACE(SendBuf, SendLen, "报文: ");

                        //写共享的发送缓冲区
                        SendLenSecure = security_packappframe(nPipeIndex, nAppType, SendBuf, SendLen, SendBufSecure);  //调用加密接口
                        if(SendLenSecure > 0)
                        {
                            advPd_send_104msg(nPipeIndex, PRO_DATA_TRANSMIT, nAppType, SendBufSecure, SendLenSecure);  //调用发送接口
                            PRTL_BUF_DEBUG(SendBufSecure, SendLenSecure, "SECURITY 发送报文,pipe:%u,", nPipeIndex);
                        }

                        nSendFlag = 1;
                    }
                }

                //清发送缓冲区
                memset(SendBuf, 0, sizeof(SendBuf));
                SendLen = 0;
                memset(SendBufSecure, 0, sizeof(SendBufSecure));
                SendLenSecure = 0;

                //转入进程间数据接收处理
                emRunStep = THREAD_RUN_RECV;
                //break;
            }
            case THREAD_RUN_CLOSE:
            {
                //重新初始化
                emRunStep = THREAD_RUN_INIT;
                break;
            }
            default:
            {
                emRunStep = THREAD_RUN_INIT;
                break;
            }
        }

        if(1 == nSendFlag)
        {
            usleep(50000);                         //出让CPU资源
        }
        else
        {
            sleep(1);
        }
        
    }

    pthread_exit(NULL);
}
#else


/*******************************************************************************
* 函数名称: up_recv_deal_thread(通用模式)
* 函数功能: 主站控制任务处理线程
* 输入参数：
* 输出参数：
* 返 回 值:
*******************************************************************************/
void *up_recv_deal_thread_base(void *arg)
{
    FRAME104_FMT_DEBUG("****** up_recv_deal_thread_base()  start... ****** \n");

    //线程名
	char ThreadName[30] = {0};
    sprintf(ThreadName, "104upbase");
    prctl(PR_SET_NAME,ThreadName);
    
    THREAD_RUN_STEP_E emRunStep = THREAD_RUN_INIT;  //运行状态
    uint8   RecvBuf[MAX_MSG_LEN] = {0};  //接收缓冲区
    uint8   SendBuf[MAX_MSG_LEN] = {0};  //发送缓冲区
    int32   RecvLen = 0, SendLen = 0;  //接收、发送数据长度
    uint8   nPipeIndex = 0;  //收发通道号
    uint8  nAppType = APP_TYPE_NOMMAL;  //主要用于加密类型
    uint8  nNetWorkFlag = PRO_DATA_TRANSMIT;  //链路连接状态标志

    uint8   SendBufSecure[DA_ESAM_MAX_LEN] = {0};  //加密之后的发送缓冲区
    int32   SendLenSecure = 0;          //加密之后的发送数据长度
    int     ret = 0;

    while(1)
    {
        g_nUpRecvDealThreadFlag = 1;  //用于检测线程运行是否正常
        
        if(!g_bGlobalInitFlag)
        {
            sleep(1);
            continue;
        }
        
        switch (emRunStep)
        {
            case THREAD_RUN_INIT:
            {
                nAppType = APP_TYPE_NOMMAL;
                emRunStep = THREAD_RUN_RECV;  //打开成功转至接收处理
                break;
            }
            case THREAD_RUN_RECV:
            {
                //读共享的接收缓冲区
                memset(RecvBuf, 0x00, sizeof(RecvBuf));
                RecvLen = advPd_get_commmsg(&nPipeIndex, &nNetWorkFlag, RecvBuf);
                g_nNetWorkFlag = PRO_DATA_TRANSMIT;
                
                if (RecvLen > 0)
                {
                    //记录最新的收发通道号
                    g_nGlobalPipe = nPipeIndex;

                    //记录报文
                    FRAME104_FMT_TRACE("控制任务 接收 up_recv_deal_thread THREAD_RUN_RECV RecvLen:%d, nPipeIndex:%d \n", RecvLen, nPipeIndex);
                    FRAME104_BUF_TRACE(RecvBuf, RecvLen, "报文: ");

                    //报文解析
                    SendLen = security_unpackframe(g_nGlobalPipe, RecvBuf, RecvLen, SendBuf);  //调用解密接口
                }

                //清接收缓冲区
                memset(RecvBuf, 0, sizeof(RecvBuf));
                RecvLen = 0;

                //转入数据发送处理
                emRunStep = THREAD_RUN_CONFIRM;
                break;
            }
            case THREAD_RUN_CONFIRM:
            {
                //写共享的发送缓冲区(主站控制命令的应答报文)
                if(SendLen > 0)
                {
                    //记录报文
                    FRAME104_FMT_TRACE("控制任务 发送 up_recv_deal_thread THREAD_RUN_SEND SendLen:%d, nPipeIndex:%d \n", SendLen, nPipeIndex);
                    FRAME104_BUF_TRACE(SendBuf, SendLen, "报文: ");

                    //写共享的发送缓冲区
                    advPd_send_104msg(nPipeIndex, PRO_DATA_TRANSMIT, nAppType, SendBuf, SendLen);
                    PRTL_BUF_DEBUG(SendBuf, SendLen, "SECURITY 发送报文,pipe:%u,", nPipeIndex);
                }

                //清发送缓冲区
                memset(SendBuf, 0, sizeof(SendBuf));
                SendLen = 0;

                //转入数据接收处理
                emRunStep = THREAD_RUN_SEND;
                break;
            }
            case THREAD_RUN_SEND:
            {
                //调用故障信息事件生成和检测接口
                if(g_bDataSendFlag == 2)
                {
                    if(g_sTaskRunStep.nTaskStep == STEP_CONT_DEFAULT)//首先判断当前任务是否空闲
                    {
                        event_happen_proc();  //事件生成接口
                        ret = event_check_faults();  //判断是否有事件生成
                        if(ret > 0)
                        {
                            FRAME104_FMT_TRACE("up_send_deal_thread() 检测到故障事件生成, 需要上报! \n");
                            g_bFaultChanged = 1;
                        }
                    }
                }
                
                //上报安全证书时不容许进行其它发送
                if ((SendLen = security_report(&nPipeIndex, SendBuf)) >= 0)
                {
                    if (SendLen > 0) {
                        advPd_send_104msg(nPipeIndex, PRO_DATA_TRANSMIT, 0, SendBuf, SendLen);  //调用发送接口
                        PRTL_BUF_DEBUG(SendBuf, SendLen, "SECURITY 发送报文,pipe:%u,", nPipeIndex);
                    }

                    break;
                }

                //定时主动上报任务
                if(g_nNetWorkFlag == PRO_DATA_TRANSMIT)
                {
                    timing_active_send();
                }

                //创建任务与任务处理
                if(g_nNetWorkFlag == PRO_DATA_TRANSMIT)
                {
                    report_task_create();
                    SendLen = report_task_run(&nPipeIndex, SendBuf, &nAppType);

                    //写共享的发送缓冲区
                    if(SendLen > 0)
                    {
                        //记录报文
                        FRAME104_FMT_TRACE("任务处理与上报 上报 up_send_deal_thread() SendLen:%d, nPipeIndex:%d\n", SendLen, nPipeIndex);
                        FRAME104_BUF_TRACE(SendBuf, SendLen, "报文: ");

                        //写共享的发送缓冲区
                        SendLenSecure = security_packappframe(nPipeIndex, nAppType, SendBuf, SendLen, SendBufSecure);  //调用加密接口
                        if(SendLenSecure > 0)
                        {
                            advPd_send_104msg(nPipeIndex, PRO_DATA_TRANSMIT, nAppType, SendBufSecure, SendLenSecure);  //调用发送接口
                            PRTL_BUF_DEBUG(SendBufSecure, SendLenSecure, "SECURITY 发送报文,pipe:%u,", nPipeIndex);
                        }
                    }
                }

                //清发送缓冲区
                memset(SendBuf, 0, sizeof(SendBuf));
                SendLen = 0;
                memset(SendBufSecure, 0, sizeof(SendBufSecure));
                SendLenSecure = 0;

                //转入进程间数据接收处理
                emRunStep = THREAD_RUN_RECV;
                break;
            }
            case THREAD_RUN_CLOSE:
            {
                //重新初始化
                emRunStep = THREAD_RUN_INIT;
                break;
            }
            default:
            {
                emRunStep = THREAD_RUN_INIT;
                break;
            }
        }
        usleep(50000);                         //出让CPU资源
    }

    pthread_exit(NULL);
}
#endif

/*******************************************************************************
* 函数名称: up_recv_deal_thread
* 函数功能: 主站控制任务处理线程
* 输入参数：
* 输出参数：
* 返 回 值:
*******************************************************************************/
void *up_recv_deal_thread(void *arg)
{

#ifdef AREA_ZHEJIANG
    return up_recv_deal_thread_zhj(arg);
#else
    return up_recv_deal_thread_base(arg);
#endif

}
#ifdef AREA_ZHEJIANG

/*******************************************************************************
* 函数名称: down_device_thread(浙江模式)
* 函数功能: 下行设备线程
* 输入参数：
* 输出参数：
* 返 回 值:
*******************************************************************************/
void *down_device_thread_zhj(void *arg)
{
    DB104_FMT_DEBUG("****** down_device_thread_zhj()  start... ****** \n");

    //线程名
	char ThreadName[30] = {0};
    sprintf(ThreadName, "104downzj");
    prctl(PR_SET_NAME,ThreadName);
    
    THREAD_RUN_STEP_E emRunStep = THREAD_RUN_INIT;  //运行状态

    //变量采集计时器
    SECOND_COUNTER_T stSecondCounter = {0};

    //变量采集状态
    READ_STATE_T stReadState = {0};
    
    while(1)
    {
        g_nDownDeviceThreadFlag = 1;  // 用于检测线程运行是否正常
        
        if(!g_bGlobalInitFlag)
        {
            sleep(1);
            continue;
        }

        time_manage_down();

        // 过秒处理
        if(g_unDownPastTimeFlag.BitSect.Second)
        {
            stSecondCounter.nCommonYxSecCounter ++;  //普通遥信 秒计时器
            stSecondCounter.nCommonYcSecCounter ++;  //普通遥测 秒计时器
            stSecondCounter.nRecordYcSecCounter ++;  //记录遥测 秒计时器
            stSecondCounter.nDemandYcSecCounter ++;  //需量 秒计时器
            stSecondCounter.nHarmonicYcSecCounter ++;  //谐波 秒计时器
        }

        //检测任务类型
        stReadState.nHardYxFlag = 0;//硬遥信 改为事件变更
        
        if(stSecondCounter.nCommonYxSecCounter >= 3)
        {
            stReadState.nCommonYxFlag = 1;//普通遥信
            stSecondCounter.nCommonYxSecCounter = 0;
        }
        
        if(stSecondCounter.nCommonYcSecCounter >= 1) /* 原来是3，跑统计台体不过 */
        {
            stReadState.nCommonYcFlag = 1;//普通遥测
            stSecondCounter.nCommonYcSecCounter = 0;
        }

        if(stSecondCounter.nRecordYcSecCounter >= 3)
        {
            stReadState.nRecordYcFlag = 1;//记录遥测
            stSecondCounter.nRecordYcSecCounter = 0;
        }
        
        if(stSecondCounter.nDemandYcSecCounter >= 5*60)
        {
            stReadState.nDemandYcFlag = 1;//需量
            stSecondCounter.nDemandYcSecCounter = 0;
        }
        
        if(stSecondCounter.nHarmonicYcSecCounter >= 2)  /* 原来是5，跑统计台体不过 */
        {
            stReadState.nHarmonicYcFlag = 1;//谐波
            stSecondCounter.nHarmonicYcSecCounter = 0;
        }

#if TEST_VER_FLAG == 1
        //测试版本, 直接按照普通遥测采集周期
        if(1 == stReadState.nCommonYcFlag)
        {
            stReadState.nPreDayYcFlag = 1;//上日统计数据 是否采集
            stReadState.nPreMonYcFlag = 1;//上月统计数据 是否采集
        }
#else
        time_t tCurrentTime; //系统当前时间
        // 过日处理
        if(g_unDownPastTimeFlag.BitSect.Day)
        {
            stReadState.nPreDayYcFlag = 1;//上日统计数据 是否采集
            time(&tCurrentTime);  //获取time_t类型的当前时间
            nPreDayTime_t = tCurrentTime - (g_stDownCurBinTime.Hour*60*60 + g_stDownCurBinTime.Minute*60 + g_stDownCurBinTime.Second + 10);  //前一日23:59:50转换的秒数
        }

        // 过月处理
        if(g_unDownPastTimeFlag.BitSect.Month)
        {
            stReadState.nPreMonYcFlag = 1;//上月统计数据 是否采集
            time(&tCurrentTime);  //获取time_t类型的当前时间
            nPreMonTime_t = tCurrentTime - ((g_stDownCurBinTime.Day - 1)*24*60*60 + g_stDownCurBinTime.Hour*60*60 + g_stDownCurBinTime.Minute*60 + g_stDownCurBinTime.Second + 10);  //上一月最后一天23:59:50转换的秒数
        }
#endif

        switch (emRunStep)
        {
            case THREAD_RUN_INIT:
            {
                //DB104_FMT_TRACE("down_device_thread() THREAD_RUN_INIT... \n");
                
                //打开成功转至更新处理
                emRunStep = THREAD_RUN_UPDATE;
                
                //break;
            }
            case THREAD_RUN_UPDATE:
            {
                //DB104_FMT_TRACE("down_device_thread() THREAD_RUN_UPDATE... \n");

                //打开成功转至接收处理
                emRunStep = THREAD_RUN_RECV;
                
                //break;
            }
            case THREAD_RUN_RECV:
            {
                //DB104_FMT_TRACE("down_device_thread() THREAD_RUN_RECV... \n");
                
                //从数据中心读取遥信/遥测/电量数据
                data_db_center_read_zhj(stReadState);

                //转入数据发送处理
                emRunStep = THREAD_RUN_SEND;
                
                //break;
            }
            case THREAD_RUN_SEND:
            {
                //DB104_FMT_TRACE("down_device_thread() THREAD_RUN_SEND... \n");

                //转入进程间数据接收处理
                emRunStep = THREAD_RUN_UPDATE;
                
                break;
            }
            case THREAD_RUN_CLOSE:
            {
                //重新初始化
                emRunStep = THREAD_RUN_INIT;
                break;
            }
            default:
            {
                emRunStep = THREAD_RUN_INIT;
                break;
            }
        }

        memset(&stReadState, 0x00, sizeof(READ_STATE_T));
        usleep(200000);                         //出让CPU资源
    }

    pthread_exit(NULL);
}

#else

/*******************************************************************************
* 函数名称: down_device_thread(通用模式)
* 函数功能: 下行设备线程
* 输入参数：
* 输出参数：
* 返 回 值:
*******************************************************************************/
void *down_device_thread_base(void *arg)
{
    DB104_FMT_DEBUG("****** down_device_thread_base()  start... ****** \n");

    //线程名
	char ThreadName[30] = {0};
    sprintf(ThreadName, "104downbase");
    prctl(PR_SET_NAME,ThreadName);
    
    int ret = 0;
    THREAD_RUN_STEP_E emRunStep = THREAD_RUN_INIT;  //运行状态
    BOOL bFirstRun = TRUE;
    //uint8 RecvBuf[MAX_MSG_LEN] = {0};  //接收缓冲区
    //int32 RecvLen = 0;  //接收数据长度
    //uint32  sender = 0;
    
    while(1)
    {
        g_nDownDeviceThreadFlag = 1;  // 用于检测线程运行是否正常
        
        if(!g_bGlobalInitFlag)
        {
            sleep(1);
            continue;
        }

        time_manage();  //时间管理刷新

        switch (emRunStep)
        {
            case THREAD_RUN_INIT:
            {
                DB104_FMT_TRACE("down_device_thread() THREAD_RUN_INIT... \n");
                
                //打开成功转至更新处理
                emRunStep = THREAD_RUN_UPDATE;
                
                //break;
            }
            case THREAD_RUN_UPDATE:
            {
                DB104_FMT_TRACE("down_device_thread() THREAD_RUN_UPDATE... \n");
                
                //获取电表档案更新事件
                /*
                RecvLen = task_recv_msg(g_FilesUpdate, RecvBuf, MAX_MSG_LEN, &sender);
                if(RecvLen > 0)
                {
                    DB104_FMT_TRACE("down_device_thread() 收到档案更新事件, RecvLen=%d ... \n", RecvLen);
                    TASK_MSG_t *pMsg = (TASK_MSG_t *)RecvBuf;
                    switch(pMsg->msg_id)
                    {
                        case MSG_MINITOR_DBUPDATE:
                        {
                            DB_UPDATE_MSG_T *pUpdateMsg = (DB_UPDATE_MSG_T *)pMsg->msg;
                            DB104_FMT_TRACE("收到mqtt发送的档案更新消息 infonum=%d, oad=%08x \n", pUpdateMsg->infoNum, pUpdateMsg->slavedata[0]);
                            if(pUpdateMsg->slavedata[0] == 0x60000200)
                            {
                                files_update_event_deal(pUpdateMsg, &g_sMeterFileAll);
                            }
                        }
                            break;
                        default:
                            break;
                    }
                }
                */

                //打开成功转至接收处理
                emRunStep = THREAD_RUN_RECV;
                
                //break;
            }
            case THREAD_RUN_RECV:
            {
                DB104_FMT_TRACE("down_device_thread() THREAD_RUN_RECV... \n");
                sleep(20);  //测试使用
                
                //从数据中心读取遥信/遥测/电量数据
                data_db_center_read();

                //转入数据发送处理
                emRunStep = THREAD_RUN_SEND;
                
                //break;
            }
            case THREAD_RUN_SEND:
            {
                DB104_FMT_TRACE("down_device_thread() THREAD_RUN_SEND... \n");
                sleep(20);  //测试使用

                //第一次启动需要将定值参数写入数据中心
                if(bFirstRun)
                {
                    para_db_center_save();
                    bFirstRun = FALSE;
                }
                
                //判断定值参数是否变化
                if (g_bFixedValueChanged)
                {
                    g_bFixedValueChanged = FALSE;
                    ret = write_pdata_xram(g_pParamValueName, (char *)&g_aParaSetValue[0][0], 0, sizeof(g_aParaSetValue));
                    if(ret < 0)
                    {
                        DB104_FMT_TRACE("保存本地定值参数失败  write_pdata_xram(g_pParamValueName) error! ret=%d \n", ret);
                    }
                    para_db_center_save();
                }

                //转入进程间数据接收处理
                emRunStep = THREAD_RUN_UPDATE;
                
                break;
            }
            case THREAD_RUN_CLOSE:
            {
                //重新初始化
                emRunStep = THREAD_RUN_INIT;
                break;
            }
            default:
            {
                emRunStep = THREAD_RUN_INIT;
                break;
            }
        }
        
        usleep(100000);  //出让CPU资源
    }

    pthread_exit(NULL);
}

#endif

/*******************************************************************************
* 函数名称: down_device_thread
* 函数功能: 下行设备线程
* 输入参数：
* 输出参数：
* 返 回 值:
*******************************************************************************/
void *down_device_thread(void *arg)
{

#ifdef AREA_ZHEJIANG
    return down_device_thread_zhj(arg);
#else
    return down_device_thread_base(arg);
#endif

}

/*******************************************************************************
* 函数名称: monit_stat_thread
* 函数功能: 监测统计线程
* 输入参数：
* 输出参数：
* 返 回 值:
*******************************************************************************/
void *monit_stat_thread(void *arg)
{
    STAT_FMT_DEBUG("****** monit_stat_thread start ****** \n");

    DB_CLIENT mtClient = db_client_create(DB_PD_AGENT, DB_PD_CLIENT_STAT);
    if(mtClient <= 0)
    {
        STAT_FMT_DEBUG("mt db client create failed.\n");
        return 0;
    }

    while(1)
    {
        g_nMonitStatThreadFlag = 1;  //用于检测线程运行是否正常
        
        if(!g_bGlobalInitFlag)
        {
            sleep(1);
            continue;
        }

        //产生过分标志
        time_manage_mt();

        //过分处理
        if(g_eMtPastTimeFlag.BitSect.Minute)
        {
            mt_calc_entry(mtClient);
        }
    }
}
#ifdef AREA_ZHEJIANG

/*********************************************************************
 功能描述：主初始化函数(浙江模式)
 输入参数：
 输出参数：
 函数返回值：>0 : 初始化成功
             <0 : 初始化失败
********************************************************************/
int pw_main_init_zhj()
{
    FRAME104_FMT_DEBUG("****** pw_main_init_zhj()  start... ****** \n");

    int z = 0;

    //线程参数初始化
    memset(&g_sUpRecvDealThreadArg, 0, sizeof(THREAD_ARG_T));
    memset(&g_sUpSendDealThreadArg, 0, sizeof(THREAD_ARG_T));
    memset(&g_sDownDeviceThreadArg, 0, sizeof(THREAD_ARG_T));

    //终端数量
    for(z=0; z<g_aGroupCountZhj[E_EQUIP_POWERTERMINAL-1]; z++)
    {
        FRAME104_FMT_DEBUG("终端数量 g_aEquipCountZhj[E_EQUIP_POWERTERMINAL-1][%d] = %d \n", z, g_aEquipCountZhj[E_EQUIP_POWERTERMINAL-1][z]);
    }

    //配电变压器数量
    for(z=0; z<g_aGroupCountZhj[E_EQUIP_TRANSFORMER-1]; z++)
    {
        FRAME104_FMT_DEBUG("配电变压器数量 g_aEquipCountZhj[E_EQUIP_TRANSFORMER-1][%d] = %d \n", z, g_aEquipCountZhj[E_EQUIP_TRANSFORMER-1][z]);
    }
    
    //智能电容器数量
    for(z=0; z<g_aGroupCountZhj[E_EQUIP_CAPACITOR-1]; z++)
    {
        FRAME104_FMT_DEBUG("智能电容器数量 g_aEquipCountZhj[E_EQUIP_CAPACITOR-1][%d] = %d \n", z, g_aEquipCountZhj[E_EQUIP_CAPACITOR-1][z]);
    }
    
    //剩余电流保护器数量
    for(z=0; z<g_aGroupCountZhj[E_EQUIP_RESIDUALCURRENT-1]; z++)
    {
        FRAME104_FMT_DEBUG("剩余电流保护器数量 g_aEquipCountZhj[E_EQUIP_RESIDUALCURRENT-1][%d] = %d \n", z, g_aEquipCountZhj[E_EQUIP_RESIDUALCURRENT-1][z]);
    }

    //全局参数初始化
    if(global_init_zhj() < 0)
    {
        FRAME104_FMT_DEBUG("init global_init_zhj() error! \n" );
        return -1;
    }

    //加载XML点表
    config_xml_load_zhj();

    //定值参数初始化(私有文件)
    param_data_init_zhj();

    //和数据中心交互的句柄(读取数值)
    g_sDataCenterClient  = db_client_create(DB_PD_AGENT, DB_PD_CLIENT_104FRAME);
    if(g_sDataCenterClient <= 0)
    {
        FRAME104_FMT_DEBUG("gDataCenterClient 创建失败, 返回! \n");
        return -1;
    }

    //和数据中心交互的句柄(事件使用)
    g_sEventDbClient  = db_client_create(DB_PD_AGENT, DB_PD_CLIENT_EVENT);
    if(g_sEventDbClient <= 0)
    {
        FRAME104_FMT_DEBUG("g_sEventDbClient 创建失败, 返回! \n");
        return -1;
    }

    //根据点表内容生成采集方案列关联OAD
    scheme_coload_save_zhj();

    //根据点表内容生成对上上报任务(周期实时上报、周期冻结与上报)
    timing_task_create_zhj();

    //根据点表内容生成档案存入数据中心
    equip_files_save_zhj();

    //根据点表内容生成对下设备采集方案
    equip_scheme_save_zhj();

    //根据点表内容生成对下设备采集任务
    equip_task_save_zhj();

    //共享队列初始化
    memset(&g_stSoeBuff, 0, sizeof(g_stSoeBuff));
    if (pthread_mutex_init(&g_stSoeBuff.QueueLock, NULL) != 0)
    {
        FRAME104_FMT_DEBUG("init g_stSoeBuff.QueueLock error! \n");
    }

    //事件相关初始化
    event_init_zhj(g_sEventDbClient);
    
    g_bGlobalInitFlag = TRUE;
    FRAME104_FMT_TRACE("pw_main_init_zhj() 初始化结束 g_bGlobalInitFlag=%d \n", g_bGlobalInitFlag);
    return 1;
}

#else

int g_FilesUpdate = 0;  //数据中心档案变更事件处理

/*********************************************************************
 功能描述：主初始化函数(通用模式)
 输入参数：
 输出参数：
 函数返回值：>0 : 初始化成功
             <0 : 初始化失败
********************************************************************/
int pw_main_init_base()
{
    FRAME104_FMT_DEBUG("****** pw_main_init_base()  start... ****** \n");

    //线程参数初始化
    memset(&g_sUpRecvDealThreadArg, 0, sizeof(THREAD_ARG_T));
    memset(&g_sUpSendDealThreadArg, 0, sizeof(THREAD_ARG_T));
    memset(&g_sDownDeviceThreadArg, 0, sizeof(THREAD_ARG_T));

    //上报表初始化
    memset(&g_aYxDataSend[0], 0x00, sizeof(g_aYxDataSend));
    memset(&g_aYcDataSend[0], 0x00, sizeof(g_aYcDataSend));
    memset(&g_aEnergyDataSend[0], 0x00, sizeof(g_aEnergyDataSend));
    memset(&g_aYkDataSend[0], 0x00, sizeof(g_aYkDataSend));
    memset(&g_aFaultDataSend[0], 0x00, sizeof(g_aFaultDataSend));

    //加载XML点表
    if(1)
    {
        config_xml_load();
    }

    //全局参数初始化
    if(global_init() < 0)
    {
        FRAME104_FMT_DEBUG("init global_init() error! \n" );
        return -1;
    }

    //和数据中心交互的句柄(新)
    g_sDataCenterClient  = db_client_create(DB_PD_AGENT, DB_PD_CLIENT_104FRAME);
    if(g_sDataCenterClient <= 0)
    {
        FRAME104_FMT_DEBUG("gDataCenterClient创建失败, 返回! \n");
        return -1;
    }

    //数据中心档案更新线程交互fd
    g_FilesUpdate = task_msg_init(PD_UPDATE_FILES);
    if(g_FilesUpdate <= 0)
    {
        FRAME104_FMT_DEBUG("g_FilesUpdate档案更新fd创建失败, 返回! \n");
        return -1;
    }
    task_msg_settime(g_FilesUpdate, 0, 10000);//接收超时时间设置

    //初始化电表档案读写锁
    if ( pthread_rwlock_init ( &g_sMeterFileAll.rwLock, NULL ) != 0 )
    {
        FRAME104_FMT_DEBUG("init MeterFileAll.rwLock error! \n" );
        return -1;
    }

    //电表档案初始化
    uint16 nMeterCount = 0;
    nMeterCount = meter_file_init(&g_sMeterFileAll);
    FRAME104_FMT_DEBUG("数据中心读取的档案数量为:%d \n", nMeterCount);

    //事件相关初始化
    event_init();

    g_bGlobalInitFlag = TRUE;
    FRAME104_FMT_DEBUG("pw_main_init() 初始化结束 g_bGlobalInitFlag=%d \n", g_bGlobalInitFlag);
    return 1;
}
#endif

/*********************************************************************
 功能描述：主初始化函数
 输入参数：
 输出参数：
 函数返回值：>0 : 初始化成功
             <0 : 初始化失败
********************************************************************/
int pw_main_init()
{
#ifdef AREA_ZHEJIANG
    return pw_main_init_zhj();
#else
    return pw_main_init_base();
#endif
}













