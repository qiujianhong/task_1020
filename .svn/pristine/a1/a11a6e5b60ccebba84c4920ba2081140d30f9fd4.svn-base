#include "msgthread.h"
#include "dataType.h"
#include "oopType.h"
#include "db_api.h"
#include "task_msg.h"
#include "task_id.h"
#include "msg_id.h"
#include "appmsg_split.h"
#include "msg_base.h"
#include "task_proc.h"
#include "out_msg.h"
#include "msg_proc.h"

#include "label_id.h"

#include"extern.h"

DB_AGENT   g_dbAgent;


MsgThread::MsgThread(QObject *parent) :
    QThread(parent)
{
    /* 对外通信线程启动初始化 */
//    gui_outmsg_init(MSG_TYPE_CHOOSE, gMqttIp, gMqttPort);
}

MsgThread::~MsgThread()
{
    gui_outmsg_uninit();
}

void MsgThread::run()
{
    uint8       recvBuf[APP_RECV_BUF_LEN] = {0};
    int         recvlen = 0;
    int         ret = 0;
    MESSAGE_INFO_T *pMsgInfo = NULL;
    uint8       appstatus = GUI_STATUS_INIT;  //正式代码改为 GUI_STATUS_INIT
//    int         dbagent = gui_get_dbagent();
    struct timespec curtime = {0};
    struct timespec oldtime = {0};

    /* 先发送一次APP注册消息 */
    ret = gui_login_to_smios();
    if(0 != ret)
    {
        GUI_FMT_TRACE("gui_login_to_smios failed %d\n", ret);
    }

    /* 循环收消息 */
    clock_gettime(CLOCK_MONOTONIC, &oldtime);
    for(;;)
    {
        recvlen = gui_recv_msg(recvBuf, APP_RECV_BUF_LEN);
        if(recvlen > 0)
        {
            ret = appmsg_split(recvBuf, (uint16)recvlen , &pMsgInfo);
            if(NULL != pMsgInfo)
            {
                GUI_BUF_TRACE(recvBuf, recvlen, "Recv msg from %s:", pMsgInfo->souraddr);
            }
            else
            {
                GUI_BUF_TRACE(recvBuf, recvlen, "Recv msg:");
            }
            if(0 == ret && NULL != pMsgInfo)
            {
                /* 注册应答 */
                if(MESSAGE_IID_SYSTEM == pMsgInfo->IID && SYSTEM_IOP_APP_REGISTER == pMsgInfo->IOP)
                {
                    if(TRUE == *pMsgInfo->playload)
                    {
                        appstatus = GUI_STATUS_WORK;
//                        appstatus = GUI_STATUS_SEEK;
                        GUI_FMT_DEBUG("=======gui login to OS=======\n");
//                        ret = gui_seek_app();  //查询注册的APP
//                        if(0 != ret)
                        {
//                            GUI_FMT_TRACE("gui_seek_app failed %d\n", ret);
                        }
                    }
                    goto goon;
                }
                
                /* 心跳管理 */
                if(MESSAGE_IID_COMMON == pMsgInfo->IID && COMMON_IOP_HERTBEAT == pMsgInfo->IOP)
                {
                    ret = gui_heart_bear(pMsgInfo);
                    if(0 != ret)
                    {
                        GUI_FMT_TRACE("gui send heart failed %d\n", ret);
                    }
                    goto goon;
                }

                /* 查询版本号 */
                if(MESSAGE_IID_COMMON == pMsgInfo->IID && COMMON_IOP_VERSION == pMsgInfo->IOP)
                {
                    ret = gui_get_version(pMsgInfo);
                    if(0 != ret)
                    {
                        GUI_FMT_DEBUG("advance get version failed %d\n", ret);
                    }
                    goto goon;
                }

                /* 查询APP  ID */
                if(MESSAGE_IID_COMMON == pMsgInfo->IID && COMMON_IOP_GETAPPID == pMsgInfo->IOP)
                {
                    ret = gui_get_appid(pMsgInfo);
                    if(0 != ret)
                    {
                        GUI_FMT_DEBUG(" gui_get_appid failed %d\n", ret);
                    }
                    goto goon;
                }
                
				/* 桌面消息处理 */
				if(MESSAGE_IID_DESKGUI == pMsgInfo->IID)
				{
					ret = desktop_msg_handle(pMsgInfo);
					if(0 != ret)
					{	
						GUI_FMT_TRACE("desktop msg handle failed %d\n",ret);
					}
					goto goon;
				}
				

                /* 处于初始化状态后续操作停止 */
                if(GUI_STATUS_INIT == appstatus)
                {
                    goto goon;
                }

				/* APP查询返回, 全部关注的接口都返回, 则结束查询 */
//                if(MESSAGE_IID_SYSTEM == pMsgInfo->IID && SYSTEM_IOP_GET_REGISTERAPP == pMsgInfo->IOP)
//                {
//                    if(TRUE == gui_recv_regapp(pMsgInfo))
//                    {
//                        appstatus = GUI_STATUS_WORK;
//                    }
//                    goto goon;
//                }
                
                /* 数据中心更新 无 */

                /* 数据中心 */
                ret = db_agent_msg_entry(g_dbAgent, pMsgInfo);
                if(DB_AGENT_UNKONW_MSG != ret)
                {
                    goto goon;
                }

                /* 高级APP + 采集任务调度 + 无线拨号 + 模组 */
                ret = task_recv_outmsg(pMsgInfo);
                if(TASK_UNKNOW_MSG != ret)
                {
                    goto goon;
                }

goon:
                free(pMsgInfo);
                pMsgInfo = NULL;
            }
        }

        /* 查询其他APP信息 */
        if(GUI_STATUS_WORK != appstatus)
        {
            clock_gettime(CLOCK_MONOTONIC, &curtime);
            if(curtime.tv_sec - oldtime.tv_sec > 5)  //5s发送一次消息
            {
                if(GUI_STATUS_INIT == appstatus)
                {
                    ret = gui_login_to_smios();
                    if(0 != ret)
                    {
                        GUI_FMT_TRACE("gui_login_to_smios failed %d\n", ret);
                    }
                }
//                else if(GUI_STATUS_SEEK == appstatus)
//                {
//                    ret = gui_seek_app();
//                    if(0 != ret)
//                    {
//                        GUI_FMT_TRACE("gui_seek_app failed %d\n", ret);
//                    }
//                }
				oldtime = curtime;
            }
        }
    }
}