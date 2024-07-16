/*
*********************************************************************
* Copyright(C) 2020 南京新联电子股份有限公司
* All rights reserved.
* @brief：   高级APP对外通信模块
* @date：    2020-3-4
* @history：
*********************************************************************
*/
#include "xlChannel.h"
#include "baseCommon.h"
#include "db_api.h"
#include "appmsg_split.h"
#include "comm_log.h"

#include "mqtt_base.h"
#include "mqtt_proc.h"
#include "client_id.h"
#include "test_thread.h"
#include "framecomm.h"


DB_AGENT g_dbAgent = 0;


/**********************************************************************
* @name      : out_msg_entry
* @brief     ：启动对外通信消息任务
* @param[in] ：void *pArg
* @param[out]：
* @return    ：
* @Create    :
* @Date      ：2020-4-16
* @Update    :
**********************************************************************/
void *out_msg_entry(void *pArg)
{
	uint8       recvBuf[RECV_BUF_LEN] = { 0 };
	int         recvlen = 0;
	int         ret = 0;
	MESSAGE_INFO_T *pMsgInfo = NULL;


	/* 循环收消息 */
	for (;;)
	{
		recvlen = app_recv_msg(recvBuf, RECV_BUF_LEN);
		if (recvlen > 0)
		{
			ret = appmsg_split(recvBuf, (uint16)recvlen, &pMsgInfo);
			MSGAC_BUF_TRACE(recvBuf, recvlen, "Recv mqtt msg:\n");
            
			if (0 == ret && NULL != pMsgInfo)
			{
//				printf("IID = %x\n", pMsgInfo->IID);
				/* 性能测试 */
				if (0x2000 == pMsgInfo->IID)
				{
					app_test_msg(pMsgInfo);
					goto goon;
				}


				/* 统计自动删除 */
				if (0x3000 == pMsgInfo->IID)
				{
					app_deal_auto_del_msg(pMsgInfo);
					goto goon;
				}


				/* 采集自动删除 */
				if (0x4000 == pMsgInfo->IID)
				{
					app_deal_auto_del_msg2(pMsgInfo);
					goto goon;
				}

				/* 事件自动删除 */
				if (0x5000 == pMsgInfo->IID)
				{
					app_deal_auto_del_msg1(pMsgInfo);
					goto goon;
				}

				/* 冻结自动删除 */
				if (0x6000 == pMsgInfo->IID)
				{
					app_deal_auto_del_msg3(pMsgInfo);
					goto goon;
				}

				/* 自救删除 */
				if (0x7000 == pMsgInfo->IID)
				{
					app_deal_selfhelp_del_msg(pMsgInfo);
					goto goon;
				}

				if(0x8000 == pMsgInfo->IID)
				{
					app_deal_StabilityTest_msg(pMsgInfo);
					goto goon;
				}

				/* 数据中心代理 */
				ret = db_agent_msg_entry(g_dbAgent, pMsgInfo);
				if (DB_AGENT_UNKONW_MSG != ret)
				{
					goto goon;
				}

				/* 其他消息处理 自己添加代码todo */

			goon:
				free(pMsgInfo);
				pMsgInfo = NULL;
			}
		}

    }
}

/**********************************************************************
* @name      : msg_task_start
* @brief     ：启动消息任务
* @param[in] ：uint32 ip        mqtt连接的IP地址
			   uint16 port      mqtt连接的端口
* @param[out]：
* @return    ：
* @Create    :
* @Date      ：2019-12-24
* @Update    :
**********************************************************************/
int msg_task_start(uint32 ip, uint16 port)
{
	int ret = 0;
	pthread_t outmsgid = 0;

	/* 创建 db_agent 创建线程级别的代理 */
	g_dbAgent = db_agent_create(AGENT_TYPE_THREAD, DB_AGENT_ID, app_send_dbmsg, DB_CLIENT_MAX);
	{
		if (g_dbAgent < 0)
		{
			printf("db_agent_create error!\n");
			return -1;
		}
	}

	if (0 != app_mqtt_init(ip, port))
	{
		db_agent_close(g_dbAgent);

		printf("Main init MQTT error!\n");
		return -1;
	}

	/* out msg线程启动 */
	ret = pthread_create(&outmsgid, NULL, out_msg_entry, NULL);
	if (ret != 0)
	{
		app_outmsg_uninit();
		db_agent_close(g_dbAgent);
		printf("Msg out thread Create error! err:%d\n", ret);
		return -1;
	}

	printf("Main start MQTT msg task!\n");

	return 0;
}

