#ifdef AREA_FUJIAN
#include "CtsAll.h"
#include "fujian_task.h"
#include "fujian_common.h"
#include "task_msg.h"

extern uint8   gTaskThreadFlag;
extern uint8   gAmrThreadFlag[AMR_THREAD_NUM];
extern uint8 g485ShareFlag;

/*******************************************************************************
* 函数名称: thread_task_fujian
* 函数功能: 任务管理线程
* 输入参数：arg                 指向参数区的指针
* 输出参数：无
* 内部处理： 
* 返 回 值: 无
*******************************************************************************/
void* thread_task_fujian ( void* arg )
{

    prctl(PR_SET_NAME,"task_fujian");
    int RecvLen;
    uint8 RecvBuf[MAX_MSG_LEN];
    uint32 sender;
    taskmng_cur_bin_time_get(&gCurBinTimeTask);
    memcpy(&gPreBinTimeTask, &gCurBinTimeTask, sizeof(DateTime_T));

    gDBClientTask = db_client_create(DB_AGENT_CTS,DB_CLIENT_TASK);
    gTaskSock = task_msg_init(IPC_TASK);
    task_msg_settime(gTaskSock,0,50000);

    memset(&dbupdateTask,0x00,sizeof(DB_UPDATE_T));
    taskmng_trans_task_cfg_init();
    while(1)
    {
        gTaskThreadFlag = 1;
        RecvLen = task_recv_msg(gTaskSock,RecvBuf,MAX_MSG_LEN,&sender);
        if(RecvLen>0)
        {
            TASK_MSG_t *msg = (TASK_MSG_t *)RecvBuf;
            TASK_FMT_DEBUG(-1, TASK_LOG, "收到IPC消息 \n");
            TASK_BUF_DEBUG(-1, TASK_LOG,msg->msg, msg->msglen );
            switch(msg->msg_id)
            {
                case MSG_COLLECTION_UPDATE:
                {
                    fujian_collection_update_deal(msg->msg, msg->msglen);
                }
                break;
            }
        }

        taskmng_trans_task_db_update_check(&dbupdateTask);
        
        taskmng_trans_task_ergodic();
        taskmng_time_manage_task();
        usleep(20000);
    }
    db_client_close(gDBClientTask);

}


/*******************************************************************************
* 函数名称: thread_amr_fujian
* 函数功能: 对应各端口的抄表线程
* 输入参数：arg                 指向参数区的指针
* 输出参数：无
* 内部处理： 
* 返 回 值: 无
*******************************************************************************/

void* thread_amr_fujian ( void* arg )
{
    uint8 pipe;
    int RecvLen;
    uint8 RecvBuf[MAX_MSG_LEN];
    uint32 sender;
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = (AmrThreadArg_T*)arg;
    pipe = pAmrThreadArg->CommPipe;
    char ThreadName[30];
    sprintf(ThreadName,"amr_fujian-%d",pipe);
    prctl(PR_SET_NAME,ThreadName);
    taskmng_amr_thread_queue_init ( arg );     //线程队列初始化
    taskmng_past_time_deal_init ( pipe );  //时间过处理初始化
    taskmng_all_state_init ( pipe );      //状态字初始化
    taskmng_all_flag_init ( pipe ); 
    fujian_state_init (pipe);
    taskmng_pipe_collection_state_write(pipe,TASK_NULL,0,0);
    while(1)
    {
        gAmrThreadFlag[pipe] = 1;
        RecvLen = task_recv_msg(pAmrThreadArg->MsgSock,RecvBuf,MAX_MSG_LEN,&sender);
        if(g485ShareFlag==1 && pipe == PLC_NUM+MAX_485_NUM-3)
        {
            //fujian_rs485_release(pipe);
            sleep(3);
            continue;
        }
        if(RecvLen>0)
        {
            TASK_MSG_t *msg = (TASK_MSG_t *)RecvBuf;
            TASK_FMT_DEBUG(pipe, RELAY_LOG, "收到IPC消息 \n");
            TASK_BUF_DEBUG(pipe, RELAY_LOG,msg->msg, msg->msglen );
            switch(msg->msg_id)
            {
                case MSG_PRIORITY_UPDATE:
                {
                    fujian_task_priority_update_deal(pipe,msg->msg, msg->msglen);
                }
                break;
                case MSG_PRIORITY_CHANGE:
                {
                    fujian_task_priority_change(pipe,msg->msg, msg->msglen);

                }
                break;
            }
        }
        fujian_data_init_check(pipe);
        fujian_time_manage(pipe);
        taskmng_trans_task_run_ergodic(pipe);
        taskmng_fujian_task_creat(pipe);
        taskmng_fujian_task_run(pipe);
    }
    pthread_exit( NULL );
}



#endif

