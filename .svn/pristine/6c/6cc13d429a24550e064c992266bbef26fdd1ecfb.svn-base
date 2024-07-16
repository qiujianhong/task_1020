/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief： 功能描述
* @date： 2019-10-25
*********************************************************************
*/
#include "pageInterface.h"

#include "CcoMain.h"

#include "CcoCommon.h"
#include "CcoDebug.h"
#include "CcoFiles.h"
#include "CcoLib.h"
#include "CcoPrtl.h"
#include "CcoTTL.h"
#include "CcoCommdrv.h"
#include <sys/prctl.h>
#include "area.h"
#include "Mqtt.h"

#include "gui_c_prtl.h"
#include "message.h"
#include "appmsg_split.h"
#include "mqtt_client.h"
#include "trace_back.h"


extern PLUGIN_INFO* g_plugin[];
extern int g_nPluginCnt;

// mqtt 客户端句柄
extern MQTT_CLIENT_T *g_MqttClient;

/*******************************************************************************
* 全局变量声明
*******************************************************************************/
#define  CCO_ROUTER_INI_FILE            "ccoRouter.ini"

pthread_mutex_t ActiveInfoIndexLock;

uint16	CommDelayCnt = 0;			            //通信延时计数器
uint8 gUpIpcThreadFlag;                         //上行IPC线程标志
uint8 gDownTtlThreadFlag[MAX_DOWN_THREAD_NUM] = {0};  //下行串口通信线程标志
uint8 gAmrThreadFlag[MAX_DOWN_THREAD_NUM] = {0};                       //自动抄表线程标志
uint8 gRebootAmrThread[MAX_DOWN_THREAD_NUM] = {0};                     //重启抄表线程标志

COMM_JOIN_MESSAGE_T gCommJoinMessage[MAX_DOWN_THREAD_NUM];  //串口拼接报文信息

//mqtt ip地址与端口号
char mqttIP[100] = {0};
int mqttport = 0;

//是否关注的app均已注册
//uint8 gMapManagerAppLoginFlag = 0;
uint8 gDBCenterAppLoginFlag = 1;//与数据中心没有交互

//本app是否已经注册成功
uint8 bAppSelfLoginState = 0;

//appname: app英文名称(用于查询appid)
char *AppIdName = "ccoRouter";

//appid: 对应app的ID信息
char  AppId[APP_ID_LEN] = {0};

/**********************************************************************
* @name      : ccoRouter_gui_reg
* @brief     ：本地APP显示注册
* @param[in] ：
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 梅安宁
* @Date      ：2020-5-12
* @Update    : 
**********************************************************************/
int ccoRouter_gui_reg()
{
    const char*		appName = "ccoRouter";
	const char*		iconFile ="/usr/local/extapps/ccoRouter/bin/cfg/ccoRouter.bmp";
	APP_REGIST_INFO_T appRegInfo;		//向桌面gui发送的注册信息
    int           ret = 0;
    
    memset(&appRegInfo,0,sizeof(APP_REGIST_INFO_T));
    //APP名称
	memcpy(appRegInfo.appname,appName,strlen(appName));
    memcpy(appRegInfo.shortname, ADV_SHOW_NAME, strlen(ADV_SHOW_NAME));
    //app显示图标
	memcpy(appRegInfo.iconFileName,iconFile,strlen(iconFile));
    ret = appGUI_send_req_handle(MESSAGE_IID_DESKGUI, DESKGUI_IOP_APP_REG, &appRegInfo, sizeof(APP_REGIST_INFO_T));

    if(0 != ret)
    {
        CCOGUI_FMT_DEBUG("Send appgui reg failed %d\n", ret);
    }
    return ret;
}

/**********************************************************************
* @name      : wireless_gui_entry
* @brief     ：本地APP显示
* @param[in] ：
* @param[out]：
* @return    ：
* @Create    : 梅安宁
* @Date      ：2020-5-12
* @Update    : 
**********************************************************************/
void* ccoRouter_gui_entry()
{

    pthread_t mdl_gui_id;
    
    /* 变量初始化 todo */
    initlize_desk("ccoRouter",mqtt_topic_send_msg);

    CCOGUI_FMT_DEBUG("ccoRouter_gui_entry() Start to reg Appgui... \n");
    /* APP注册gui */
    while(getLoginguiID() == 0)
    {
        ccoRouter_gui_reg();
        sleep(2);
    }

    /* 其他业务 */
    AppPluginReg(g_plugin, g_nPluginCnt);
    if (pthread_create(&mdl_gui_id,NULL,app_gui_entry,NULL) != 0)				/*mdl_gui 线程*/
    {
        CCOGUI_FMT_DEBUG("creating the app_gui_entry failed!\n");
    }

	CCOGUI_FMT_DEBUG("ccoRouter_gui_entry() end... \n");    
    return 0;
}

/**********************************************************************
* @name      : ccoRouter_recv_appGUI_back_handle
* @brief     ：本地APP接收gui消息处理
* @param[in] ：
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 梅安宁
* @Date      ：2020-5-12
* @Update    : 
**********************************************************************/
int ccoRouter_recv_appGUI_back_handle(MSG_INFO_T *ptMsgInfo)
{
    MESSAGE_INFO_T *ptmp = NULL;
    uint32    len = 0;
    int       ret = 0;
    len = sizeof(MESSAGE_INFO_T) + ptMsgInfo->MsgLen;
    ptmp = (MESSAGE_INFO_T*)malloc(len);
    if (NULL == ptmp)
    {
        CCOGUI_FMT_DEBUG("ccoRouter_recv_appGUI_back_handle malloc failed %d\n", len);
        return -1;
    }
    ptmp->rpm = ptMsgInfo->MsgRPM;
    ptmp->priority = ptMsgInfo->MsgPRIORITY;
    ptmp->index = ptMsgInfo->MsgIndex;
    ptmp->label = ptMsgInfo->MsgLabel;
    ptmp->IID = ptMsgInfo->IID;
    ptmp->IOP = ptMsgInfo->IOP;
    ptmp->sourlen = ptMsgInfo->SourLen;
    ptmp->destlen = ptMsgInfo->DestLen;
    ptmp->msglen = ptMsgInfo->MsgLen;

    memcpy(ptmp->souraddr, ptMsgInfo->SourAddr, ptmp->sourlen);
    memcpy(ptmp->destaddr, ptMsgInfo->DestAddr, ptmp->destlen);
    memcpy(ptmp->playload, ptMsgInfo->MsgData, ptmp->msglen);

    ret = appGUI_recv_back_handle(ptmp);
    free(ptmp);
    return ret;
}

/*******************************************************************************
* 函数名称: UpMqttThread
* 函数功能: 上行线程(MQTT消息中心)
* 输入参数: void *arg            指向参数区的指针
* 输出参数: 无
* 返 回 值: 无
*******************************************************************************/
#if MSG_MQTT == 1
void *UpMqttThread(void *arg)
{
    CCOUP_FMT_DEBUG("void *UpMqttThread(void *arg) \n");

    //线程名
    char ThreadName[30] = {0};
    sprintf(ThreadName, "CcoUpMqtt-%d", 0);
    prctl(PR_SET_NAME,ThreadName);

    static time_t LastLoginTime;
    int8    SocketState = IPC_SOCK_INIT;        //环回socket运行状态
    uint8   RecvBuf[MAX_QUEUE_BUFF_LEN] = {0}; //socket接收缓冲区
    uint8   SendBuf[MAX_QUEUE_BUFF_LEN] = {0}; //socekt发送缓冲区
    uint8   SendInfoBuf[MAX_QUEUE_BUFF_LEN] = {0}; //socekt发送缓冲区    
    int32   RecvLen = 0;                        //接收数据长度
    int32   SendLen = 0;                        //发送数据长度
    int32   SendInfoLen = 0;                        //发送数据长度
    ThreadArg_T *pThreadArg;                    //线程参数
    LockQueue_T *pLockRecv;                     //载波锁接收缓冲区指针
    
    int     ret;

    MSG_INFO_T *ptMsgInfo = NULL;
    MSG_INFO_T *pMsgInfoSend = NULL;
    
    char topic[500] = {0};  //主题

    memset(RecvBuf, 0x00, sizeof(RecvBuf));
    memset(SendBuf, 0x00, sizeof(SendBuf));
    memset(SendInfoBuf, 0x00, sizeof(SendInfoBuf));

    pThreadArg = (ThreadArg_T *)arg;            //初始化锁结构指针
    pLockRecv = pThreadArg->RecvBuf;

    INFO_DATA_T stInfoData;  //消息
    memset(&stInfoData, 0x00, sizeof(stInfoData));

    //uint16 nMapManagerSendIndex = 0;//发送消息index
    //time_t MapManagerLastCheckTime = abs(time(NULL) - 5);//查询时间

    //uint16 nDBCenterSendIndex = 0;//发送消息index
    //time_t DBCenterLastCheckTime = abs(time(NULL) - 5);//查询时间

    //BOOL tempFlag = TRUE;

    while(1)
    {
        gUpIpcThreadFlag = 1;                  //用于检测线程运行是否正常
        GetCurBcdTime(&gCurBcdTimeUp);         //刷新当前BCD码时间

        switch (SocketState)
        {
            case IPC_SOCK_INIT:
            {
                //printx(" void *UpMqttThread(void *arg)...case IPC_SOCK_INIT: \n");                        
#if 0
                if(0 == gMapManagerAppLoginFlag)//查看模组管理器是否注册成功
                {
                    if( abs(time(NULL) - MapManagerLastCheckTime) > 2 )
                    {
                        CCOUP_FMT_DEBUG("pthread_mqtt() ACIPC_INIT...查询模组管理器注册状态 \n");
                        MapManagerLastCheckTime = time(NULL);
                        check_app_login_state(mapManagerName, &nMapManagerSendIndex);
                    }
                    SocketState = IPC_SOCK_RECV;
                    
                }
                else if(0 == gDBCenterAppLoginFlag)//查看数据中心是否注册成功
                {
                    if( abs(time(NULL) - DBCenterLastCheckTime) > 2 )
                    {
                        CCOUP_FMT_DEBUG("pthread_mqtt() ACIPC_INIT...查询数据中心注册状态 \n");
                        DBCenterLastCheckTime = time(NULL);
                        check_app_login_state(dbName, &nDBCenterSendIndex);
                    }
                    SocketState = IPC_SOCK_RECV;
                    
                }
#endif
                //else
                //{
                //转到注册
                SocketState = IPC_SOCK_LOGIN; 
                //}
                
                break;
            }
            case IPC_SOCK_LOGIN:
            {
                //printx(" void *UpMqttThread(void *arg)...case IPC_SOCK_LOGIN: \n");
                login_to_smiOS();
                LastLoginTime = time(NULL);
                SocketState = IPC_SOCK_LOGIN_WAIT;
                
                break;
            }
            case IPC_SOCK_LOGIN_WAIT:
            {
                //printx(" void *UpMqttThread(void *arg)...case MSG_LOGIN_WAIT: \n");
                if( abs(time(NULL) - LastLoginTime) > 5)
                    SocketState = IPC_SOCK_LOGIN;
                else
                    SocketState = IPC_SOCK_RECV;
                
                break;
            }
            case IPC_SOCK_RECV:
            {
                //printx(" void *UpMqttThread(void *arg)...case IPC_SOCK_RECV: \n");
                //环回进行数据接收         
                memset(RecvBuf,0x00,sizeof(RecvBuf));
                RecvLen = mqtt_read(RecvBuf);
                
                if(RecvLen > 0)
                {
                    CCOUP_FMT_DEBUG("Recv Up IPC data... RecvLen=%d \n", RecvLen);
                    CCOUP_BUF_DEBUG(RecvBuf, RecvLen, "接收mqtt报文:");
                    
                    //将消息解析成MSG_INFO_T 
                    ret = msg_mqtt_split(RecvBuf,RecvLen, 0, 0, &ptMsgInfo);
                    if(ret < 0)
                    {
                        CCOUP_FMT_DEBUG("UpMqttThread() msg_mqtt_split() 解析失败 \n");
                        break;
                    }
                    
                    //打印完整消息内容
                    CCOUP_FMT_DEBUG("UpMqttThread()...IPC_SOCK_RECV...通过Mqtt收到的消息为: \n");
                    PrintInfoDetail(ptMsgInfo);

                    if(ptMsgInfo->IID == 0x1155)
                    {
                        switch(ptMsgInfo->IOP)
                        {
                        case 0x9090:  //查询厂商版本信息
                        {
                             Factoryversion_Response(ptMsgInfo);
                        }
                            break;
                        default:
                            break;
                        }
                    }
                    if(ptMsgInfo->IID == 0x0000)
                    {
                        switch(ptMsgInfo->IOP)
                        {
                            case 0x0010:  //心跳
                            {
                                heart_beat(ptMsgInfo->MsgIndex);
                            }
                                break;
                            case 0x0013://版本信息
                            {
                                Appversion_Response(ptMsgInfo);
                            }
                                break;
                            case 0x0016://appid信息
                            {
                                appid_response(ptMsgInfo);
                            }
                                break;
                            default:
                                break;
                        }
                    }
                    else if(ptMsgInfo->IID == 0x0001)
                    {
                        switch (ptMsgInfo->IOP)
                        {
                            case 0x0010://注册响应
                            {
                                bAppSelfLoginState = 1;
                            }
                                break;
                            case 0x0013://查询指定App信息
                            {
#if 0
                                if(0 == gMapManagerAppLoginFlag)//查看模组管理器是否注册成功
                                {
                                    gMapManagerAppLoginFlag = unpack_login_info(ptMsgInfo, mapManagerName, nMapManagerSendIndex);
                                }

                                if(0 == gDBCenterAppLoginFlag)//查看数据中心是否注册成功
                                {
                                    gDBCenterAppLoginFlag = unpack_login_info(ptMsgInfo, dbName, nDBCenterSendIndex);
                                }
#endif
                            }
                                break;
                            default:
                                break;
                        }
                    }
#if 0
                    else if(ptMsgInfo->IID == 0x0004)//模组管理器
                    {
                        switch (ptMsgInfo->IOP)
                        {
                            case 0x0001://模块插拔事件
                            {
                                ModuleEventFun(ptMsgInfo->MsgData, ptMsgInfo->MsgLen);
                            }
                                break;
                            case 0x0010://查询模组信息响应
                            {
                                PlcWriteQueue(pAmrRecvUp, (uint8*)ptMsgInfo, sizeof(MSG_INFO_T)+ptMsgInfo->MsgLen);
                            }
                                break;
                            default:
                                break;
                        }
                    }
#endif
                    else if(ptMsgInfo->IID == MESSAGE_IID_DESKGUI)
                    {
                        ccoRouter_recv_appGUI_back_handle(ptMsgInfo);
                    }
                    else
                    {
                        //添加至MSG队列
                        if(ptMsgInfo->MsgLen > MAX_MSG_PAYLOAD_LEN)
                        {
                            CCOUP_FMT_DEBUG("错误! if(ptMsgInfo->MsgLen > MAX_MSG_PAYLOAD_LEN) \n");
                        }
                        else
                        {
                            memset(&stInfoData, 0x00, sizeof(stInfoData));
                            memcpy(&stInfoData, ptMsgInfo, sizeof(MSG_INFO_T)+ptMsgInfo->MsgLen);
                            AddInfoDataIntoList(stInfoData);
                        }
                    }

                    free(ptMsgInfo);
                    ptMsgInfo = NULL;

                    //根据消息产生任务
                    CreateTaskByInfo();

                    usleep(2000);                         //出让CPU资源
                }
                else
                {
                    usleep(10000);                         //出让CPU资源
                }               

                /* if((0 == gMapManagerAppLoginFlag) || (0 == gDBCenterAppLoginFlag) )
                {
                    SocketState = IPC_SOCK_INIT;
                }*/
                //else
                //{
                //判断是否登录成功
                if(bAppSelfLoginState == 1)
                {
                    SocketState = IPC_SOCK_SEND;
                }                   
                else
                {
                    SocketState = IPC_SOCK_LOGIN_WAIT;
                }
                //}
                                            
                break;
            }           

            case IPC_SOCK_SEND:
            {
                //printx(" void *UpMqttThread(void *arg)...case IPC_SOCK_SEND: \n");
                
                //查共享的发送缓冲区        
                SendLen = PlcReadQueue(pLockRecv, SendBuf);
                if (SendLen > 0)
                {                   
                    //获取MSG
                    memset(&stInfoData, 0x00, sizeof(stInfoData));
                    memcpy(&stInfoData, SendBuf, SendLen);
                    
                    //分配内存
                    pMsgInfoSend = (MSG_INFO_T *)malloc(SendLen);
                    memset(pMsgInfoSend, 0x00, SendLen);

                    //获取HEAD
                    GetHeadInfo(stInfoData, pMsgInfoSend);

                    //消息主题
                    memset(topic, 0x00, sizeof(topic));
                    sprintf(topic,"%s/%s", pMsgInfoSend->SourAddr, pMsgInfoSend->DestAddr);

                    //打印完整消息内容
                    CCOUP_FMT_DEBUG("UpMqttThread()...IPC_SOCK_SEND...通过MQTT发送的消息为: (topic:%s) \n", topic);
                    PrintInfoDetail(pMsgInfoSend);

                    //组消息
                    SendInfoLen = msg_mqtt_package(pMsgInfoSend, MAX_QUEUE_BUFF_LEN, SendInfoBuf);

                    //发送
                    mqtt_topic_write(SendInfoBuf, SendInfoLen, topic);
                    CCOUP_FMT_DEBUG("Send Up IPC data... SendInfoLen=%d \n", SendInfoLen);
                    CCOUP_BUF_DEBUG(SendInfoBuf, SendInfoLen, "发送mqtt报文:");
                    memset(SendInfoBuf, 0, sizeof(SendInfoBuf));
                    
                    memset(SendBuf, 0, sizeof(SendBuf));
                    free(pMsgInfoSend);
                    pMsgInfoSend = NULL;
                }

                //转入进程间数据接收处理
                SocketState = IPC_SOCK_RECV;
                            
                break;
            }
                
            default:
            {
                SocketState = IPC_SOCK_INIT;
                break;
            }
           
        }

        //usleep(20000);                         //出让CPU资源
    }
    pthread_exit(NULL);

}
#endif

/*******************************************************************************
* 函数名称: UpIpcThread
* 函数功能: 上行线程(UDP消息中心)
* 输入参数: void *arg            指向参数区的指针
* 输出参数: 无
* 返 回 值: 无
*******************************************************************************/
#if MSG_MQTT == 0
void *UpIpcThread(void *arg)
{
    printx(" void *UpIpcThread(void *arg) \n");
	writelog(ROUTER_CCO_RECD_LOG, " void *UpIpcThread(void *arg)");
	
	int8    SocketState = IPC_SOCK_INIT;        //环回socket运行状态
    uint8   RecvBuf[MAX_QUEUE_BUFF_LEN] = {0}; //socket接收缓冲区
    uint8   SendBuf[MAX_QUEUE_BUFF_LEN] = {0}; //socekt发送缓冲区
    int32   SendLen = 0;                        //发送数据长度
    ThreadArg_T *pThreadArg;                    //线程参数
    LockQueue_T *pLockRecv;                     //载波锁接收缓冲区指针
    
    int     ret;
	int     result;
	int     createflag = 0;

	PRAR_UDP_T  tUdp = {0};
	MSG_INFO_T *ptMsgInfo = NULL;
	MSG_INFO_T *pMsgInfoSend = NULL;
	HMSG        phMsg = NULL;

	memset(RecvBuf, 0x00, sizeof(RecvBuf));
	memset(SendBuf, 0x00, sizeof(SendBuf));

    pThreadArg = (ThreadArg_T *)arg;            //初始化锁结构指针
    pLockRecv = pThreadArg->RecvBuf;

	INFO_DATA_T stInfoData;  //消息
	memset(&stInfoData, 0x00, sizeof(stInfoData));

	//BOOL tempFlag = FALSE;

    while(1)
    {
        gUpIpcThreadFlag = 1;                  //用于检测线程运行是否正常

		switch (SocketState)
        {
	        case IPC_SOCK_INIT:
	        {
				//printx(" void *UpIpcThread(void *arg)...case IPC_SOCK_INIT: \n");
	            //打开进程间通讯
				tUdp.appName = gCcoRouterAppName;
			    inet_aton(gCcoRouterAppIP, (struct in_addr*)&tUdp.srcIP);
			    tUdp.srcIP = ntohl(tUdp.srcIP);
			    tUdp.srcPort = gCcoRouterAppPort;
			    tUdp.bBlock = 0;         //非阻塞

				phMsg = msg_handle_create(tUdp);
			    if(NULL == phMsg)
			    {		        
	                printx("open UpIpcThread error!\n");
					writelog(ROUTER_CCO_RECD_LOG, "Open UpIpcThread error!");
	                sleep(1);  //打开失败，延时1S重新尝试打开进程间通讯
			    }
				else
	            {
	                SocketState = IPC_SOCK_RECV;    //打开成功转至socket接收处理
	            }
	            break;
			}			

	        case IPC_SOCK_RECV:
	        {
				//printx(" void *UpIpcThread(void *arg)...case IPC_SOCK_RECV: \n");
	            //环回进行数据接收         
				ret = msg_udp_read(phMsg, &ptMsgInfo);
				
				if(0 == ret && NULL != ptMsgInfo)
				{
					//打印完整消息内容
					printx("\n UpIpcThread()...IPC_SOCK_RECV...通过UDP收到的消息为: \n");
					PrintInfoDetail(ptMsgInfo);
					writelog(ROUTER_CCO_RECD_LOG, "\n UpIpcThread()...IPC_SOCK_RECV...通过UDP收到的消息为:");
					PrintLogInfoDetail(ptMsgInfo);
				
					//添加至MSG队列
					if(ptMsgInfo->MsgLen > MAX_MSG_PAYLOAD_LEN)
					{
						printx(" 错误! if(ptMsgInfo->MsgLen > MAX_MSG_PAYLOAD_LEN) \n");
						writelog(ROUTER_CCO_RECD_LOG, " 错误! if(ptMsgInfo->MsgLen > MAX_MSG_PAYLOAD_LEN) \n");
					}
					else
					{
						memset(&stInfoData, 0x00, sizeof(stInfoData));
						memcpy(&stInfoData, ptMsgInfo, sizeof(MSG_INFO_T)+ptMsgInfo->MsgLen);
						AddInfoDataIntoList(stInfoData);
					}								

					free(ptMsgInfo);
					ptMsgInfo = NULL;
					
					SocketState = IPC_SOCK_SEND;				
				}
				else
				{
					SocketState = IPC_SOCK_SEND;
				}

				/***临时测试报文...begin***/
				
				/***临时测试报文...end***/

				//根据消息产生任务
				CreateTaskByInfo();
				            
	            break;
			}			

	        case IPC_SOCK_SEND:
	        {
				//printx(" void *UpIpcThread(void *arg)...case IPC_SOCK_SEND: \n");
				
	            //查共享的发送缓冲区        
	            SendLen = PlcReadQueue(pLockRecv, SendBuf);
	            if (SendLen > 0)
	            {
	            	printx("\n Send Up IPC data:%d\n", SendLen);
	                PrintBuf(SendBuf, SendLen);
					
					//获取MSG
					memset(&stInfoData, 0x00, sizeof(stInfoData));
					memcpy(&stInfoData, SendBuf, SendLen);
					
					//分配内存
					pMsgInfoSend = (MSG_INFO_T *)malloc(SendLen);

					//获取HEAD, 填充UDP目标ip地址和端口
					result = GetHeadInfo(stInfoData, pMsgInfoSend);
					result = 1;  //测试阶段, 置1
					if(result >= 0)
					{
						//打印完整消息内容
						printx(" UpIpcThread()...IPC_SOCK_SEND...通过UDP发送的消息为: \n");
						PrintInfoDetail(pMsgInfoSend);
						writelog(ROUTER_CCO_RECD_LOG, "\n UpIpcThread()...IPC_SOCK_SEND...通过UDP发送的消息为:");
						PrintLogInfoDetail(pMsgInfoSend);

						//发送
		                msg_udp_write(phMsg, pMsgInfoSend);
					}
					else
					{
						printx(" UpIpcThread()...IPC_SOCK_SEND GetHeadInfo()失败...\n");
					}
					
	                memset(SendBuf, 0, sizeof(SendBuf));
					free(pMsgInfoSend);
					pMsgInfoSend = NULL;
	            }

	            //转入进程间数据接收处理
	            SocketState = IPC_SOCK_RECV;
							
	            break;
			}
				
	        case IPC_SOCK_CLOSE:
	        {
				//printx(" void *UpIpcThread(void *arg)...case IPC_SOCK_CLOSE: \n");
	            //关闭进程间通讯
	            msg_handle_close(phMsg);

	            //重新初始化进程间通讯
	            SocketState = IPC_SOCK_INIT;
	            break;
			}			

	        default:
	        {
				 SocketState = IPC_SOCK_INIT;
	            break;
			}
           
        }

        usleep(100000);                         //出让CPU资源
    }
    pthread_exit(NULL);

}
#endif

/*******************************************************************************
* 函数名称: DownTtyacmThread
* 函数功能: 与下行串口进程进行通信 鼎信模块
* 输入参数: void *arg            指向参数区的指针
* 输出参数: 无
* 返 回 值: 无
*******************************************************************************/
#if DOWN_DEV_TYPE == 1
void *DownTtyacmThread(void *arg)
{

	uint8   nTtlIndex = 0;  //不同的端口号对应不同的数组下标
	int     fd_sock = -1;                       //socket连接标识符
    int8   SocketState = IPC_SOCK_INIT;        //线程运行状态
    uint8   RecvBuf[MAX_QUEUE_BUFF_LEN];        //接收缓冲区
    uint8   SendBuf[MAX_QUEUE_BUFF_LEN];        //发送缓冲区
    int32  RecvLen = 0, SendLen = 0;           //接收、发送数据长度
    RS232_COMM_PARA CommPara;
    uint16  FrameLen=0;
    int     ret = 0;
    uint8   DevName[30] = {0};
    ThreadArg_T *pThreadArg;
    LockQueue_T *pLockRecv;
    LockQueue_T *pLockSend;
	uint8 *ptr=NULL;									//应答数据帧起始指针
	
    pThreadArg = (ThreadArg_T *)arg;
	nTtlIndex = pThreadArg->nArrIndex;
	if((nTtlIndex >= MAX_DOWN_THREAD_NUM)
		|| (nTtlIndex >= MAX_ALLOWED_DOWN_THREAD_NUM))
	{
		printx(" DownTtyacmThread() 线程数组下标nTtlIndex=%d异常! \n", nTtlIndex);
		return NULL;
	}

	printx(" 端口[%d] void *DownTtyacmThread(void *arg) \n", nTtlIndex);
	writelog(gRecordPath[nTtlIndex], " 端口[%d] void *DownTtyacmThread(void *arg)", nTtlIndex);

	//printx("&pThreadArg->RecvBuf.QueueLock:%p 端口:%d \n", &pThreadArg->RecvBuf->QueueLock, nTtlIndex);
	//printx("&pThreadArg->SendBuf.QueueLock:%p 端口:%d \n", &pThreadArg->SendBuf->QueueLock, nTtlIndex);
	
    pLockRecv = pThreadArg->RecvBuf;
    pLockSend = pThreadArg->SendBuf;

	//printx("RecvBuf.QueueLock:%p 端口:%d \n", &pLockRecv->QueueLock, nTtlIndex);
	//printx("SendBuf.QueueLock:%p 端口:%d \n", &pLockSend->QueueLock, nTtlIndex);

	memset(&gCommJoinMessage[nTtlIndex], 0x00, sizeof(COMM_JOIN_MESSAGE_T));

    while(1)
    {
    	//sleep(1);
		
        gDownTtlThreadFlag[nTtlIndex] = 1;                 //用于检测线程运行是否正常
        //printx(" gDownTtlThreadFlag[%d] = 1 \n", nTtlIndex);

        switch (SocketState)
        {
        case IPC_SOCK_INIT:                    //初始化串口
        	//printx(" 串口[%d]初始化void *DownTtyacmThread(void *arg)...case IPC_SOCK_INIT: \n", nTtlIndex);
            memcpy(CommPara.sDeviceName, gDingxinDevName[nTtlIndex], 30);
            CommPara.dwBaudRate = 9600;           //波特率
            CommPara.byByteSize = 8;              //数据位, 7-8
            CommPara.byParity = 'e';          //校验 None,Odd,Even,Space
            CommPara.byStopBits = 1;              //停止位 1,2

            fd_sock = commdrv_Init_232(&CommPara);   /*打开串口*/
            fd_plc[nTtlIndex] = fd_sock;

            if (fd_sock < 0)                    //若打开不成功，延时1s后，重新打开
            {               
                printx("\n 串口[%d]打开失败DownTtyacmThread Error\n\n", nTtlIndex);
				writelog(gRecordPath[nTtlIndex], " 串口[%d]打开失败DownTtyacmThread Error", nTtlIndex);
                sleep(1);
            }
            else
            {
                SocketState = IPC_SOCK_RECV;
				printx("\n 串口[%d]打开成功DownTtyacmThread Success\n\n", nTtlIndex);
				writelog(gRecordPath[nTtlIndex], " 串口[%d]打开成功DownTtyacmThread Success", nTtlIndex);
            }
            break;

        case IPC_SOCK_RECV:
			//printx(" 串口[%d]接收void *DownTtyacmThread(void *arg)...case IPC_SOCK_RECV: \n", nTtlIndex);
            memset(RecvBuf, 0x00, sizeof(RecvBuf));
            RecvLen = commdrv_Recv_232(fd_sock, RecvBuf, sizeof(RecvBuf), 5, 5, 0xFF);

			if (RecvLen > 6)
            {
            	printx("\n 串口[%d]接收 (单条报文) RecvLen = PlcRecvTTL();...if(RecvLen > 0) \n", nTtlIndex);
            	printx(" local ttl: recv data len: %d\n", RecvLen);
                PrintBuf(RecvBuf, RecvLen);
				writelog(gRecordPath[nTtlIndex], "\n 串口[%d]接收 (单条报文) RecvLen:%d 报文内容为:", nTtlIndex, RecvLen);
				PrintLogBuffDetail(RecvBuf, RecvLen, nTtlIndex);

				while ((CheckFrameFormat(RecvLen, RecvBuf, &FrameLen, 2) > 0) 
					|| (CheckFrameFormat(RecvLen, RecvBuf, &FrameLen, 1) > 0))
				{
                	if(InstantReportTaskDeal(RecvBuf, FrameLen, nTtlIndex) < 0) //若判断为上报，则不再写入缓冲区
                    {
                        printx("\n 串口[%d]接收 (完整报文) if(InstantReportTaskDeal() < 0) ... 长度FrameLen:%d 报文RecvBuf: \n", nTtlIndex, FrameLen);
			            PrintBuf(RecvBuf, FrameLen);
						printx("\n");
						writelog(gRecordPath[nTtlIndex], "\n 串口[%d]接收 (完整报文) if(InstantReportTaskDeal() < 0) ... 长度FrameLen:%d 报文RecvBuf: \n", nTtlIndex, FrameLen);
						PrintLogBuffDetail(RecvBuf, FrameLen, nTtlIndex);
						
						ret = PlcWriteQueue(pAmrRecvDown[nTtlIndex], RecvBuf, FrameLen);
						//printx("ret = PlcWriteQueue(pLockSend, RecvBuf, FrameLen);...ret:%d\n", ret);
                        if (ret == 0)
                        {
                        	printx(" ret = PlcWriteQueue(pLockSend, RecvBuf, FrameLen);...if (ret == 0)\n");
							writelog(gRecordPath[nTtlIndex], " ret = PlcWriteQueue(pLockSend, RecvBuf, FrameLen);...if (ret == 0)");
							
                            printx(" DownTtyacmThread is full!\n");
							writelog(gRecordPath[nTtlIndex], " DownTtyacmThread is full!");

                            //清空接收缓冲区
                            ClearCommRecvBuff(nTtlIndex);
                        }
                    }           
 
                    RecvLen -= FrameLen;
                    memset(RecvBuf, 0x00, FrameLen);
                    memmove(RecvBuf, &RecvBuf[FrameLen], RecvLen);
									
                }
            }

            SocketState = IPC_SOCK_SEND;
            break;

        case IPC_SOCK_SEND:
			//printx(" 串口[%d]发送void *DownTtyacmThread(void *arg)...case IPC_SOCK_SEND: \n", nTtlIndex);
            SendLen = PlcReadQueue(pAmrSendDown[nTtlIndex], SendBuf);
            if (SendLen > 0)
            {
            	printx("\n 串口[%d]发送SendLen = PlcReadQueue(pLockRecv, SendBuf);...if (SendLen > 0) \n", nTtlIndex);
                printx(" local ttl: send data len: %d\n", SendLen);
                PrintBuf(SendBuf, SendLen);
				writelog(gRecordPath[nTtlIndex], "\n 串口[%d]发送报文 SendLen:%d 报文内容为:", nTtlIndex, SendLen);
				PrintLogBuffDetail(SendBuf, SendLen, nTtlIndex);
				
                commdrv_Send_232(fd_sock, SendBuf, SendLen);

                //写通信日志

                memset(SendBuf, 0, sizeof(SendBuf));
            }
            SocketState = IPC_SOCK_RECV;
            break;

        case IPC_SOCK_CLOSE:
			//printx(" 串口[%d]关闭void *DownTtyacmThread(void *arg)...case IPC_SOCK_CLOSE: \n", nTtlIndex);
            PlcCloseTTL(fd_sock);
            SocketState = IPC_SOCK_INIT;
            break;

        default:
            PlcCloseTTL(fd_sock);
            SocketState = IPC_SOCK_INIT;
            break;
        }
       
        usleep(100000);                         //出让CPU资源

    }

    pthread_exit(NULL);

}
#endif

/*******************************************************************************
* 函数名称: DownTtlThread
* 函数功能: 与下行串口进程进行通信 智芯模块
* 输入参数: void *arg            指向参数区的指针
* 输出参数: 无
* 返 回 值: 无
*******************************************************************************/
#if DOWN_DEV_TYPE == 0
void *DownTtlThread(void *arg)
{

	uint8   nTtlIndex = 0;  //不同的端口号对应不同的数组下标
	int     fd_sock = -1;                       //socket连接标识符
    int8   SocketState = IPC_SOCK_INIT;        //线程运行状态
    uint8   RecvBuf[MAX_QUEUE_BUFF_LEN];        //接收缓冲区
    uint8   SendBuf[MAX_QUEUE_BUFF_LEN];        //发送缓冲区
    int32  RecvLen = 0, SendLen = 0;           //接收、发送数据长度
    COMM_PARA_T CommPara;
    uint16  FrameLen=0;
    int     ret = 0;
    uint8   DevName[30] = {0};
    ThreadArg_T *pThreadArg;
    LockQueue_T *pLockRecv;
    LockQueue_T *pLockSend;
	
    pThreadArg = (ThreadArg_T *)arg;
	nTtlIndex = pThreadArg->nArrIndex;
	if((nTtlIndex >= MAX_DOWN_THREAD_NUM)
		|| (nTtlIndex >= MAX_ALLOWED_DOWN_THREAD_NUM))
	{
		printx(" DownTtlThread() 线程数组下标nTtlIndex=%d异常! \n", nTtlIndex);
		return NULL;
	}

	printx(" 端口[%d] void *DownTtlThread(void *arg) \n", nTtlIndex);
	writelog(gRecordPath[nTtlIndex], " 端口[%d] void *DownTtlThread(void *arg)", nTtlIndex);

	//printx("&pThreadArg->RecvBuf.QueueLock:%p 端口:%d \n", &pThreadArg->RecvBuf->QueueLock, nTtlIndex);
	//printx("&pThreadArg->SendBuf.QueueLock:%p 端口:%d \n", &pThreadArg->SendBuf->QueueLock, nTtlIndex);
	
    pLockRecv = pThreadArg->RecvBuf;
    pLockSend = pThreadArg->SendBuf;

	//printx("RecvBuf.QueueLock:%p 端口:%d \n", &pLockRecv->QueueLock, nTtlIndex);
	//printx("SendBuf.QueueLock:%p 端口:%d \n", &pLockSend->QueueLock, nTtlIndex);

	memset(&gCommJoinMessage[nTtlIndex], 0x00, sizeof(COMM_JOIN_MESSAGE_T));

    while(1)
    {
    	//sleep(1);
		
        gDownTtlThreadFlag[nTtlIndex] = 1;                 //用于检测线程运行是否正常
        //printx(" gDownTtlThreadFlag[%d] = 1 \n", nTtlIndex);

        switch (SocketState)
        {
        case IPC_SOCK_INIT:                    //初始化串口
        	//printx(" 串口[%d]初始化void *DownTtlThread(void *arg)...case IPC_SOCK_INIT: \n", nTtlIndex);
            memcpy(CommPara.DeviceName, gDevName[nTtlIndex], 30);
            CommPara.BaudRate = 9600;           //波特率
            CommPara.DataBits = 8;              //数据位, 7-8
            CommPara.ParityFlag = 'e';          //校验 None,Odd,Even,Space
            CommPara.StopBits = 1;              //停止位 1,2

            fd_sock = PlcInitTTL(&CommPara);    //打开串口
            fd_plc[nTtlIndex] = fd_sock;

            if (fd_sock < 0)                    //若打开不成功，延时1s后，重新打开
            {               
                printx("\n 串口[%d]打开失败DownTtlThread Error\n\n", nTtlIndex);
				writelog(gRecordPath[nTtlIndex], " 串口[%d]打开失败DownTtlThread Error", nTtlIndex);
                sleep(1);
            }
            else
            {
                SocketState = IPC_SOCK_RECV;
				printx("\n 串口[%d]打开成功DownTtlThread Success\n\n", nTtlIndex);
				writelog(gRecordPath[nTtlIndex], " 串口[%d]打开成功DownTtlThread Success", nTtlIndex);
            }
            break;

        case IPC_SOCK_RECV:
			//printx(" 串口[%d]接收void *DownTtlThread(void *arg)...case IPC_SOCK_RECV: \n", nTtlIndex);
            memset(RecvBuf, 0x00, sizeof(RecvBuf));
            RecvLen = PlcRecvTTL(fd_sock, CommPara.BaudRate, RecvBuf, sizeof(RecvBuf));
            if(RecvLen <= 0)
            {
            	gCommJoinMessage[nTtlIndex].pinjie = 0;
                memset(gCommJoinMessage[nTtlIndex].HidRead,0x00,sizeof(gCommJoinMessage[nTtlIndex].HidRead));
                SocketState = IPC_SOCK_SEND;
                //break;
            }

            if (RecvLen > 0)
            {
            	printx("\n 串口[%d]接收 (单条报文) RecvLen = PlcRecvTTL();...if(RecvLen > 0) \n", nTtlIndex);
            	printx(" local ttl: recv data len: %d\n", RecvLen);
                PrintBuf(RecvBuf, RecvLen);
				writelog(gRecordPath[nTtlIndex], "\n 串口[%d]接收 (单条报文) RecvLen:%d 报文内容为:", nTtlIndex, RecvLen);
				PrintLogBuffDetail(RecvBuf, RecvLen, nTtlIndex);
				
                if(gCommJoinMessage[nTtlIndex].pinjie >= 15)//容错
                {
                    gCommJoinMessage[nTtlIndex].pinjie = 0;
                    memset(gCommJoinMessage[nTtlIndex].HidRead,0x00,sizeof(gCommJoinMessage[nTtlIndex].HidRead));
                }
                memcpy(&gCommJoinMessage[nTtlIndex].HidRead[gCommJoinMessage[nTtlIndex].pinjie*64],RecvBuf,RecvLen);//拼接数据
                gCommJoinMessage[nTtlIndex].pinjie += 1;
                //commfun_printstring(HidRead,pinjie*64);
				
                while ((CheckFrameFormat(gCommJoinMessage[nTtlIndex].pinjie*64, gCommJoinMessage[nTtlIndex].HidRead, &FrameLen, 2) > 0) 
					|| (CheckFrameFormat(gCommJoinMessage[nTtlIndex].pinjie*64, gCommJoinMessage[nTtlIndex].HidRead, &FrameLen, 1) > 0))
				{
                	//printx("while ((CheckFrameFormat(pinjie*64, HidRead, &FrameLen, 2) > 0) ||...begin FrameLen:%d\n", FrameLen);
                    memcpy(RecvBuf,&gCommJoinMessage[nTtlIndex].HidRead[0],FrameLen);
                    gCommJoinMessage[nTtlIndex].pinjie = 0;
                    memset(gCommJoinMessage[nTtlIndex].HidRead,0x00,sizeof(gCommJoinMessage[nTtlIndex].HidRead));
                    if(InstantReportTaskDeal(RecvBuf, FrameLen, nTtlIndex) < 0) //若判断为上报，则不再写入缓冲区
                    {
                        printx("\n 串口[%d]接收 (完整报文) if(InstantReportTaskDeal() < 0) ... 长度FrameLen:%d 报文RecvBuf: \n", nTtlIndex, FrameLen);
			            PrintBuf(RecvBuf, FrameLen);
						printx("\n");
						writelog(gRecordPath[nTtlIndex], "\n 串口[%d]接收 (完整报文) if(InstantReportTaskDeal() < 0) ... 长度FrameLen:%d 报文RecvBuf: \n", nTtlIndex, FrameLen);
						PrintLogBuffDetail(RecvBuf, FrameLen, nTtlIndex);
						
						ret = PlcWriteQueue(pAmrRecvDown[nTtlIndex], RecvBuf, FrameLen);
						//printx("ret = PlcWriteQueue(pLockSend, RecvBuf, FrameLen);...ret:%d\n", ret);
                        if (ret == 0)
                        {
                        	printx(" ret = PlcWriteQueue(pLockSend, RecvBuf, FrameLen);...if (ret == 0)\n");
							writelog(gRecordPath[nTtlIndex], " ret = PlcWriteQueue(pLockSend, RecvBuf, FrameLen);...if (ret == 0)");
							
                            printx(" DownTtlThread is full!\n");
							writelog(gRecordPath[nTtlIndex], " DownTtlThread is full!");

                            //清空接收缓冲区
                            ClearCommRecvBuff(nTtlIndex);
                        }
                    }           
 
                    RecvLen = (FrameLen/64+1)*64;
                    RecvLen -= FrameLen;
                    memset(RecvBuf, 0x00, FrameLen);
                    memmove(RecvBuf, &RecvBuf[FrameLen], RecvLen);
									
                }
            }

            SocketState = IPC_SOCK_SEND;
            break;

        case IPC_SOCK_SEND:
			//printx(" 串口[%d]发送void *DownTtlThread(void *arg)...case IPC_SOCK_SEND: \n", nTtlIndex);
            SendLen = PlcReadQueue(pAmrSendDown[nTtlIndex], SendBuf);
            if (SendLen > 0)
            {
            	printx("\n 串口[%d]发送SendLen = PlcReadQueue(pLockRecv, SendBuf);...if (SendLen > 0) \n", nTtlIndex);
                printx(" local ttl: send data len: %d\n", SendLen);
                PrintBuf(SendBuf, SendLen);
				writelog(gRecordPath[nTtlIndex], "\n 串口[%d]发送报文 SendLen:%d 报文内容为:", nTtlIndex, SendLen);
				PrintLogBuffDetail(SendBuf, SendLen, nTtlIndex);
				
                PlcSendTTL(fd_sock, SendBuf, SendLen);

                //写通信日志

                memset(SendBuf, 0, sizeof(SendBuf));
            }
            SocketState = IPC_SOCK_RECV;
            break;

        case IPC_SOCK_CLOSE:
			//printx(" 串口[%d]关闭void *DownTtlThread(void *arg)...case IPC_SOCK_CLOSE: \n", nTtlIndex);
            PlcCloseTTL(fd_sock);
            SocketState = IPC_SOCK_INIT;
            break;

        default:
            PlcCloseTTL(fd_sock);
            SocketState = IPC_SOCK_INIT;
            break;
        }
       
        usleep(100000);                         //出让CPU资源

    }

    pthread_exit(NULL);

}
#endif

/*******************************************************************************
* 函数名称: DownTtlHalThread
* 函数功能: 与下行串口进程进行通信 HAL接口
* 输入参数: void *arg            指向参数区的指针
* 输出参数: 无
* 返 回 值: 无
*******************************************************************************/
#if DOWN_DEV_TYPE == 2
void *DownTtlHalThread(void *arg)
{
    uint8   nTtlIndex = 0;  //不同的端口号对应不同的数组下标
    int8   SocketState = IPC_SOCK_INIT;        //线程运行状态
    uint8   RecvBuf[MAX_QUEUE_BUFF_LEN] = {0};        //接收缓冲区(完整报文)
    uint8   SendBuf[MAX_QUEUE_BUFF_LEN] = {0};        //发送缓冲区
    int32  RecvLen = 0;           //接收数据长度(完整报文)
    int32  SendLen = 0;           //发送数据长度
    uint16  FrameLen=0;           //合法的一帧3762报文
    int     ret = 0;
    ThreadArg_T *pThreadArg;
    int checkRet1 = 0;
    int checkRet2 = 0;
    uint8   SingleRecvBuf[MAX_QUEUE_BUFF_LEN] = {0};        //接收缓冲区(单次报文)
    int32  SingleRecvLen = 0;           //接收数据长度(单次报文)

    //测试使用cco模块信息begin
    /*
    uint8 aTestModuleBuf[] = {0x68, 0x38, 0x00, 0xC3, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x03, 0x02, 0x01, 0xF2, 0x37, 0x01, 0x01, 0x00, 0x00, 0x3C, 0x0A, 0x00, 0x00, 0x04, 0x00, 0x04, 0x3C, 0x00, 0x00, 0x00, 0x00, 0x01, 0x23, 0xF8, 0x07, 0x64, 0x00, 0x18, 0x09, 0x13, 0x18, 0x09, 0x13, 0x58, 0x54, 0x33, 0x30, 0x24, 0x09, 0x19, 0x05, 0x00, 0x00, 0x80, 0x48, 0x16 };
    uint16 nTestModuleBufLen = sizeof(aTestModuleBuf);
    int count = 0;
    int nCopyIndex = 0;
    int nCopyLen = 0;
    */
    //测试使用cco模块信息end
    
    pThreadArg = (ThreadArg_T *)arg;
    nTtlIndex = pThreadArg->nArrIndex;
    if((nTtlIndex >= MAX_DOWN_THREAD_NUM)
        || (nTtlIndex >= MAX_ALLOWED_DOWN_THREAD_NUM))
    {
        CCODOWN_FMT_DEBUG(0, "DownTtlThread() 线程数组下标nTtlIndex=%d异常! \n", nTtlIndex);
        return NULL;
    }

    CCODOWN_FMT_DEBUG(nTtlIndex, "端口[%d] void *DownTtlThread(void *arg) \n", nTtlIndex);

    //线程名
    char ThreadName[30] = {0};
    sprintf(ThreadName, "CcoDownHal-%d", nTtlIndex);
    prctl(PR_SET_NAME,ThreadName);

    while(1)
    {
        gDownTtlThreadFlag[nTtlIndex] = 1;                 //用于检测线程运行是否正常
        GetCurBcdTime(&gCurBcdTimeDown[nTtlIndex]);         //刷新当前BCD码时间

        switch (SocketState)
        {
        case IPC_SOCK_INIT:                    //初始化串口
            if(gDevHalInfo[nTtlIndex].isvalid == 0)
            {
                SocketState = IPC_SOCK_INIT;
                CCODOWN_FMT_DEBUG(nTtlIndex, "DownTtlHalThread() 串口[%d] if(gDevHalInfo[nTtlIndex].isvalid == 0) \n", nTtlIndex);
                sleep(5);  //模块信息无效, 线程空转
                break;
            }

            if((NULL == gDevHalInfo[nTtlIndex].dev) || (1 == gReloadDevFlag[nTtlIndex]))
            {
                //获取设备
                gDevHalInfo[nTtlIndex].dev = (UART_DEVICE_T*)hal_device_get(gDevHalInfo[nTtlIndex].deviceName);
                CCODOWN_FMT_DEBUG(nTtlIndex, "串口[%d] deviceName:%s, dev:%p, gReloadDevFlag=%d \n", 
                    nTtlIndex, gDevHalInfo[nTtlIndex].deviceName, gDevHalInfo[nTtlIndex].dev, gReloadDevFlag[nTtlIndex]);

                // 打开设备
                ret = gDevHalInfo[nTtlIndex].dev->uart_param_set(gDevHalInfo[nTtlIndex].dev, 9600, 8, 1, PARITY_EVEN);  //TRUE:1, FALSE:0
                CCODOWN_FMT_DEBUG(nTtlIndex, "串口[%d] dev->uart_param_set() ret=%d \n", nTtlIndex, ret);
                if(ret == 0)
                {
                    SocketState = IPC_SOCK_RECV;
                    CCODOWN_FMT_DEBUG(nTtlIndex, "串口[%d]打开成功DownTtlThread Success \n", nTtlIndex);
                    gReloadDevFlag[nTtlIndex] = 0;
                }
                else
                {
                    CCODOWN_FMT_DEBUG(nTtlIndex, "串口[%d]打开失败DownTtlThread Error \n", nTtlIndex);
                    sleep(2);
                    break;
                }
                sleep(1);
            }
            else
            {
                SocketState = IPC_SOCK_RECV;
            }
            break;

        case IPC_SOCK_RECV:
            if((gDevHalInfo[nTtlIndex].isvalid == 0) || (1 == gReloadDevFlag[nTtlIndex]))
            {
                SocketState = IPC_SOCK_INIT;
                break;
            }

            //初始化
            SingleRecvLen = 0;
            memset(SingleRecvBuf, 0x00, sizeof(SingleRecvBuf));

            //接收数据
            if(gDevHalInfo[nTtlIndex].dev == NULL)
            {
                SocketState = IPC_SOCK_INIT;
                break;
            }
            SingleRecvLen = gDevHalInfo[nTtlIndex].dev->uart_data_recv(gDevHalInfo[nTtlIndex].dev, SingleRecvBuf, sizeof(SingleRecvBuf));

            //临时测试报文拼帧begin
            /*
            if(count == 0)
            {
                nCopyLen = 8;
                SingleRecvLen = nCopyLen;
                memcpy(SingleRecvBuf, &aTestModuleBuf[nCopyIndex], nCopyLen);
                nCopyIndex += nCopyLen;
                count ++;
            }
            else if(count == 1)
            {
                nCopyLen = 10;
                SingleRecvLen = nCopyLen;
                memcpy(SingleRecvBuf, &aTestModuleBuf[nCopyIndex], nCopyLen);
                nCopyIndex += nCopyLen;
                count ++;
            }
            else if(count == 2)
            {
                int backCpLen = 0;
                nCopyLen = nTestModuleBufLen - nCopyIndex%nTestModuleBufLen;
                SingleRecvLen = nCopyLen;
                memcpy(SingleRecvBuf, &aTestModuleBuf[nCopyIndex%nTestModuleBufLen], nCopyLen);
                nCopyIndex += nCopyLen;
                backCpLen = nCopyLen;

                nCopyLen = 20;
                SingleRecvLen += nCopyLen;
                memcpy(&SingleRecvBuf[backCpLen], &aTestModuleBuf[nCopyIndex%nTestModuleBufLen], nCopyLen);
                nCopyIndex += nCopyLen;
                count ++;
            }
            else if(count == 3)
            {
                nCopyLen = nTestModuleBufLen - nCopyIndex%nTestModuleBufLen;
                SingleRecvLen = nCopyLen;
                memcpy(SingleRecvBuf, &aTestModuleBuf[nCopyIndex%nTestModuleBufLen], nCopyLen);
                nCopyIndex += nCopyLen;
                count ++;
            }
            CCODOWN_FMT_DEBUG(nTtlIndex, "nTestModuleBufLen=%d, count=%d, nCopyLen=%d, SingleRecvLen=%d, nCopyIndex=%d \n", nTestModuleBufLen, count, nCopyLen, SingleRecvLen, nCopyIndex);
            */
            //临时测试报文拼帧end

            //如果没有收到数据, 则将缓存区报文清零
            if(SingleRecvLen <= 0)
            {
                RecvLen = 0;
                memset(RecvBuf, 0x00, sizeof(RecvBuf));
            }
            else
            {
                CCODOWN_FMT_DEBUG(nTtlIndex, "串口[%d]接收 (单条报文) dev->uart_data_recv()...SingleRecvLen=%d \n", nTtlIndex, SingleRecvLen);
                CCODOWN_BUF_DEBUG(nTtlIndex, SingleRecvBuf, SingleRecvLen, "#接收 (单条报文)#: ");

                //缓存区报文已满, 无法继续拼接
                if(RecvLen >= sizeof(RecvBuf))
                {
                    CCODOWN_FMT_DEBUG(nTtlIndex, "缓存区已满! sizeof(RecvBuf)=%d, RecvLen=%d \n", sizeof(RecvBuf), RecvLen);
                    SocketState = IPC_SOCK_SEND;
                    break;
                }

                //对单条报文进行拼接处理
                if(SingleRecvLen > (sizeof(RecvBuf) - RecvLen))
                {
                    SingleRecvLen = sizeof(RecvBuf) - RecvLen;
                }
                memcpy(&RecvBuf[RecvLen], SingleRecvBuf, SingleRecvLen);
                RecvLen += SingleRecvLen;
                //CCODOWN_BUF_DEBUG(nTtlIndex, RecvBuf, RecvLen, "拼接之后(RecvLen=%d)缓存报文: ", RecvLen);

                //对拼接之后的总报文进行解析
                checkRet1 = CheckFrameFormat(RecvLen, RecvBuf, &FrameLen, 1);
                checkRet2 = CheckFrameFormat(RecvLen, RecvBuf, &FrameLen, 2);
                CCODOWN_FMT_DEBUG(nTtlIndex, "checkRet1=%d, checkRet2=%d \n", checkRet1, checkRet2);
                while ((checkRet1 > 0)  || (checkRet2 > 0))
                {
#ifdef AREA_FUJIAN
                    if (CcoPickFrame(amr, RecvBuf, FrameLen, nTtlIndex) != 0)
                    {
#endif
                        ret = InstantReportTaskDeal(RecvBuf, FrameLen, nTtlIndex);
                        CCODOWN_FMT_DEBUG(nTtlIndex, "InstantReportTaskDeal() ret=%d \n", ret);
                        if(ret < 0) //若判断为上报，则不再写入缓冲区
                        {
                            CCODOWN_FMT_DEBUG(nTtlIndex, "串口[%d]接收 (完整报文) if(InstantReportTaskDeal() < 0) ... 长度FrameLen:%d, 报文RecvBuf: \n", nTtlIndex, FrameLen);
                            CCODOWN_BUF_DEBUG(nTtlIndex, RecvBuf, FrameLen, "##接收 (完整报文)##: ");

                            ret = PlcWriteQueue(pAmrRecvDown[nTtlIndex], RecvBuf, FrameLen);
                            if (ret == 0)
                            {
                                CCODOWN_FMT_DEBUG(nTtlIndex, "ret = PlcWriteQueue(pLockSend, RecvBuf, FrameLen);...if (ret == 0) \n");
                                CCODOWN_FMT_DEBUG(nTtlIndex, "DownTtlThread is full! \n");

                                //清空接收缓冲区
                                ClearCommRecvBuff(nTtlIndex);
                            }
                        }           
#ifdef AREA_FUJIAN
                    }
#endif
                    RecvLen -= FrameLen;
                    memset(RecvBuf, 0x00, FrameLen);
                    memmove(RecvBuf, &RecvBuf[FrameLen], RecvLen);

                    checkRet1 = CheckFrameFormat(RecvLen, RecvBuf, &FrameLen, 1);
                    checkRet2 = CheckFrameFormat(RecvLen, RecvBuf, &FrameLen, 2);
                    CCODOWN_FMT_DEBUG(nTtlIndex, "while ((checkRet1 > 0) || (checkRet2 > 0)) checkRet1=%d, checkRet2=%d \n", checkRet1, checkRet2);
                                    
                }
            }

            SocketState = IPC_SOCK_SEND;
            break;

        case IPC_SOCK_SEND:
            if((gDevHalInfo[nTtlIndex].isvalid == 0) || (1 == gReloadDevFlag[nTtlIndex]))
            {
                SocketState = IPC_SOCK_INIT;
                break;
            }
            
            SendLen = PlcReadQueue(pAmrSendDown[nTtlIndex], SendBuf);
            if (SendLen > 0)
            {
                CCODOWN_FMT_DEBUG(nTtlIndex, "串口[%d]发送SendLen = PlcReadQueue(pLockRecv, SendBuf);...SendLen=%d \n", nTtlIndex, SendLen);
                CCODOWN_BUF_DEBUG(nTtlIndex, SendBuf, SendLen, "@@发送报文@@:");

                if(gDevHalInfo[nTtlIndex].dev == NULL)
                {
                    SocketState = IPC_SOCK_INIT;
                    break;
                }
                ret = gDevHalInfo[nTtlIndex].dev->uart_data_send(gDevHalInfo[nTtlIndex].dev, SendBuf, SendLen);
                CCODOWN_FMT_DEBUG(nTtlIndex, "发送结果: nTtlIndex=%d, deviceName=%s, ret=%d \n",nTtlIndex, gDevHalInfo[nTtlIndex].deviceName, ret);

                //写通信日志
                memset(SendBuf, 0, sizeof(SendBuf));
            }
            SocketState = IPC_SOCK_INIT;
            break;

        case IPC_SOCK_CLOSE:
            //printx(" 串口[%d]关闭void *DownTtlThread(void *arg)...case IPC_SOCK_CLOSE: \n", nTtlIndex);
            ret = hal_device_release((HW_DEVICE *)gDevHalInfo[nTtlIndex].dev);
            CCODOWN_FMT_DEBUG(nTtlIndex, "hal_device_release() ret=%d \n", ret);
            SocketState = IPC_SOCK_INIT;
            break;

        default:
            ret = hal_device_release((HW_DEVICE *)gDevHalInfo[nTtlIndex].dev);
            CCODOWN_FMT_DEBUG(nTtlIndex, "hal_device_release() ret=%d \n", ret);
            SocketState = IPC_SOCK_INIT;
            break;
        }
       
        usleep(100000);                         //出让CPU资源

    }

    pthread_exit(NULL);

}
#endif

/*******************************************************************************
* 函数名称: DownCcuThread
* 函数功能: 与下行串口进程进行通信
* 输入参数: void *arg            指向参数区的指针
* 输出参数: 无
* 返 回 值: 无
*******************************************************************************/
#if DOWN_DEV_TYPE == 3
typedef struct tagCCO_STATS_T
{
    uint64  sendNum;
    uint64  recvNum;
}CCO_STATS_T;
CCO_STATS_T g_CcoStats = {0};
void *DownCcuThread(void *arg)
{
    uint8   nTtlIndex = 0;  //不同的端口号对应不同的数组下标
    int8   SocketState = IPC_SOCK_INIT;        //线程运行状态
    uint8   RecvBuf[MAX_QUEUE_BUFF_LEN] = {0};        //接收缓冲区(完整报文)
    uint8   SendBuf[MAX_QUEUE_BUFF_LEN] = {0};        //发送缓冲区
    int32  RecvLen = 0;           //接收数据长度(完整报文)
    int32  SendLen = 0;           //发送数据长度
    uint16  FrameLen=0;           //合法的一帧3762报文
    int     ret = 0;
    ThreadArg_T *pThreadArg;
    int checkRet1 = 0;
    int checkRet2 = 0;
    uint8   SingleRecvBuf[MAX_QUEUE_BUFF_LEN] = {0};        //接收缓冲区(单次报文)
    int32  SingleRecvLen = 0;           //接收数据长度(单次报文)
    uint32  flag = 1;   /* 用于检测模块拔插动作，1为拔出状态 */
    //int     fd_sock = -1;                       //socket连接标识符
    //COMM_PARA_T CommPara;
    //uint8   DevName[30] = {0};
    uint32  cnt = 0;
#ifdef AREA_FUJIAN
    CCO_AMR_T  *amr = &g_CcoAmr;
    amr->flag.initFlag = UINT_MAX;
#endif
    //测试使用cco模块信息begin
    /*
    uint8 aTestModuleBuf[] = {0x68, 0x38, 0x00, 0xC3, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x03, 0x02, 0x01, 0xF2, 0x37, 0x01, 0x01, 0x00, 0x00, 0x3C, 0x0A, 0x00, 0x00, 0x04, 0x00, 0x04, 0x3C, 0x00, 0x00, 0x00, 0x00, 0x01, 0x23, 0xF8, 0x07, 0x64, 0x00, 0x18, 0x09, 0x13, 0x18, 0x09, 0x13, 0x58, 0x54, 0x33, 0x30, 0x24, 0x09, 0x19, 0x05, 0x00, 0x00, 0x80, 0x48, 0x16 };
    uint16 nTestModuleBufLen = sizeof(aTestModuleBuf);
    int count = 0;
    int nCopyIndex = 0;
    int nCopyLen = 0;
    */
    //测试使用cco模块信息end
    
    pThreadArg = (ThreadArg_T *)arg;
    nTtlIndex = pThreadArg->nArrIndex;
    if((nTtlIndex >= MAX_DOWN_THREAD_NUM)
        || (nTtlIndex >= MAX_ALLOWED_DOWN_THREAD_NUM))
    {
        CCODOWN_FMT_DEBUG(0, "DownCcuThread() 线程数组下标nTtlIndex=%d异常! \n", nTtlIndex);
        return NULL;
    }

    CCODOWN_FMT_DEBUG(nTtlIndex, "端口[%d] void *DownCcuThread(void *arg) \n", nTtlIndex);

    //线程名
    char ThreadName[30] = {0};
    sprintf(ThreadName, "CcoDownCcu-%d", nTtlIndex);
    prctl(PR_SET_NAME,ThreadName);
    sprintf(gDevHalInfo[nTtlIndex].deviceName, "%s", UART_LOCAL_CU_DEVICE_ID_STR);
    sprintf(gDevHalInfo[nTtlIndex].ctrlName, "%s", HW_DEVICE_ID_LOCAL_CU);
    gDevHalInfo[nTtlIndex].ctrlDev = NULL;

    while(1)
    {
        gDownTtlThreadFlag[nTtlIndex] = 1;                 //用于检测线程运行是否正常
        GetCurBcdTime(&gCurBcdTimeDown[nTtlIndex]);         //刷新当前BCD码时间

        if (gDevHalInfo[nTtlIndex].ctrlDev == NULL)
        {
            gDevHalInfo[nTtlIndex].ctrlDev = (LOCAL_CU_DEVICE_T *)hal_device_get(gDevHalInfo[nTtlIndex].ctrlName);

            if (gDevHalInfo[nTtlIndex].ctrlDev == NULL)
            {
                CCODOWN_FMT_DEBUG(nTtlIndex, "串口[%d] hal_device_get(%s) fail!\n", nTtlIndex, gDevHalInfo[nTtlIndex].ctrlName);
                sleep(2);
                continue;
            }
        }

        if (cnt % 500 == 0)
        {
            if (ModuleCheck(nTtlIndex) == 1)
            {
                gDevHalInfo[nTtlIndex].isvalid = 1;
                //CCODOWN_FMT_DEBUG(nTtlIndex, "DownCcuThread 有载波模块\n");

                if (flag == 1)
                {
                    /* 模块插入 */
                    /*while (1)
                    {
                        if (PlcResetRouter(nTtlIndex) > 0)
                        {
                            CCODOWN_FMT_DEBUG(nTtlIndex, "DownCcuThread 复位载波模块成功\n");
                            break;
                        }
                        else
                        {
                            CCODOWN_FMT_DEBUG(nTtlIndex, "DownCcuThread 复位载波模块失败\n");
                        }
                    }*/

                    gMeterFileCompareFlag[nTtlIndex] = 1;//更新cco模块档案
                    gReloadDevFlag[nTtlIndex] = 1;//更新串口操作句柄
                    gModuleManuID[nTtlIndex] = MMID_NULL;//更新模块信息
                    CheckModuleSoftReset(1, nTtlIndex);//主动查询模块信息标志置1
                    ClearCommQueueBuff(&gAmrRecvDownReportModule[nTtlIndex]);//清空主动上报模块信息
                    flag = 0;
                }
            }
            else
            {
                gDevHalInfo[nTtlIndex].isvalid = 0;
                flag = 1;
                CCODOWN_FMT_DEBUG(nTtlIndex, "DownCcuThread 没有载波模块\n");
                sleep(5);  //模块信息无效, 线程空转
                cnt = 0;
                continue;
            }
        }

        switch (SocketState)
        {
            case IPC_SOCK_INIT:                    //初始化串口
                if(gDevHalInfo[nTtlIndex].isvalid == 0)
                {
                    SocketState = IPC_SOCK_INIT;
                    CCODOWN_FMT_DEBUG(nTtlIndex, "DownCcuThread() 串口[%d] if(gDevHalInfo[nTtlIndex].isvalid == 0) \n", nTtlIndex);
                    
                    break;
                }

                if ((NULL == gDevHalInfo[nTtlIndex].dev) || (1 == gReloadDevFlag[nTtlIndex]))
                {

                    //获取设备
                    gDevHalInfo[nTtlIndex].dev = (UART_DEVICE_T*)hal_device_get(gDevHalInfo[nTtlIndex].deviceName);
                    CCODOWN_FMT_DEBUG(nTtlIndex, "串口[%d] deviceName:%s, dev:%p, gReloadDevFlag=%d \n", 
                        nTtlIndex, gDevHalInfo[nTtlIndex].deviceName, gDevHalInfo[nTtlIndex].dev, gReloadDevFlag[nTtlIndex]);

                    if (gDevHalInfo[nTtlIndex].dev == NULL)
                    {
                        CCODOWN_FMT_DEBUG(nTtlIndex, "串口[%d] hal_device_get(%s) fail!\n", nTtlIndex, gDevHalInfo[nTtlIndex].deviceName);
                        sleep(2);
                        break;
                    }

                    // 打开设备
                    ret = gDevHalInfo[nTtlIndex].dev->uart_param_set(gDevHalInfo[nTtlIndex].dev, 9600, 8, 1, PARITY_EVEN);  //TRUE:1, FALSE:0
                    CCODOWN_FMT_DEBUG(nTtlIndex, "串口[%d] dev->uart_param_set() ret=%d \n", nTtlIndex, ret);
                    if(ret == 0)
                    {
                        SocketState = IPC_SOCK_RECV;
                        CCODOWN_FMT_DEBUG(nTtlIndex, "串口[%d]打开成功%u bps DownCcuThread Success \n", nTtlIndex, 9600);
                        gReloadDevFlag[nTtlIndex] = 0;
#ifdef AREA_FUJIAN
                        amr->flag.initFlag = UINT_MAX;
#endif
                    }
                    else
                    {
                        CCODOWN_FMT_DEBUG(nTtlIndex, "串口[%d]打开失败DownCcuThread Error \n", nTtlIndex);
                        sleep(2);
                        break;
                    }
                    sleep(1);

#if 0
                    //初始化串口
                    //sprintf((char *)DevName, "%s", PLC_CHANNEL_TTYS);
                    //memcpy(CommPara.DeviceName, DevName, 30);
                    CommPara.BaudRate = 9600;           //波特率
                    //CommPara.DataBits = 8;              //数据位, 7-8
                    //CommPara.ParityFlag = 'e';          //校验 None,Odd,Even,Space
                    //CommPara.StopBits = 1;              //停止位 1,2

                    fd_sock = open(PLC_CHANNEL_TTYS, O_RDONLY);    //打开io口
                    fd_plc[nTtlIndex] = fd_sock;

                    if (fd_sock <= 0)                    //若打开不成功，延时1s后，重新打开
                    {
                        CCODOWN_FMT_DEBUG(nTtlIndex, "串口[%d]打开失败fd_sock=%d DownCcuThread Error \n", nTtlIndex, fd_sock);
                        sleep(1);
                    }
                    else
                    {
                        SocketState = IPC_SOCK_RECV;
                        CCODOWN_FMT_DEBUG(nTtlIndex, "串口[%d]打开成功fd_sock=%d DownCcuThread Success \n", nTtlIndex, fd_sock);
                        gReloadDevFlag[nTtlIndex] = 0;
#ifdef AREA_FUJIAN
                        amr->flag.initFlag = UINT_MAX;
#endif
                    }
#endif
                }
                else
                {
                    SocketState = IPC_SOCK_RECV;
                }
                break;

            case IPC_SOCK_RECV:
                if((gDevHalInfo[nTtlIndex].isvalid == 0) || (1 == gReloadDevFlag[nTtlIndex]))
                {
                    SocketState = IPC_SOCK_INIT;
                    break;
                }

                //初始化
                SingleRecvLen = 0;
                memset(SingleRecvBuf, 0x00, sizeof(SingleRecvBuf));

                //接收数据
                if(gDevHalInfo[nTtlIndex].dev == NULL)
                {
                    SocketState = IPC_SOCK_INIT;
                    break;
                }
                SingleRecvLen = gDevHalInfo[nTtlIndex].dev->uart_data_recv(gDevHalInfo[nTtlIndex].dev, SingleRecvBuf, sizeof(SingleRecvBuf));
                //SingleRecvLen = PlcRecvTTL(fd_sock, CommPara.BaudRate, SingleRecvBuf, sizeof(SingleRecvBuf));
                //临时测试报文拼帧begin
                /*
                if(count == 0)
                {
                    nCopyLen = 8;
                    SingleRecvLen = nCopyLen;
                    memcpy(SingleRecvBuf, &aTestModuleBuf[nCopyIndex], nCopyLen);
                    nCopyIndex += nCopyLen;
                    count ++;
                }
                else if(count == 1)
                {
                    nCopyLen = 10;
                    SingleRecvLen = nCopyLen;
                    memcpy(SingleRecvBuf, &aTestModuleBuf[nCopyIndex], nCopyLen);
                    nCopyIndex += nCopyLen;
                    count ++;
                }
                else if(count == 2)
                {
                    int backCpLen = 0;
                    nCopyLen = nTestModuleBufLen - nCopyIndex%nTestModuleBufLen;
                    SingleRecvLen = nCopyLen;
                    memcpy(SingleRecvBuf, &aTestModuleBuf[nCopyIndex%nTestModuleBufLen], nCopyLen);
                    nCopyIndex += nCopyLen;
                    backCpLen = nCopyLen;

                    nCopyLen = 20;
                    SingleRecvLen += nCopyLen;
                    memcpy(&SingleRecvBuf[backCpLen], &aTestModuleBuf[nCopyIndex%nTestModuleBufLen], nCopyLen);
                    nCopyIndex += nCopyLen;
                    count ++;
                }
                else if(count == 3)
                {
                    nCopyLen = nTestModuleBufLen - nCopyIndex%nTestModuleBufLen;
                    SingleRecvLen = nCopyLen;
                    memcpy(SingleRecvBuf, &aTestModuleBuf[nCopyIndex%nTestModuleBufLen], nCopyLen);
                    nCopyIndex += nCopyLen;
                    count ++;
                }
                CCODOWN_FMT_DEBUG(nTtlIndex, "nTestModuleBufLen=%d, count=%d, nCopyLen=%d, SingleRecvLen=%d, nCopyIndex=%d \n", nTestModuleBufLen, count, nCopyLen, SingleRecvLen, nCopyIndex);
                */
                //临时测试报文拼帧end

                //如果没有收到数据, 则将缓存区报文清零
                if(SingleRecvLen <= 0)
                {
                    RecvLen = 0;
                    memset(RecvBuf, 0x00, sizeof(RecvBuf));
                }
                else
                {
                    CCODOWN_FMT_DEBUG(nTtlIndex, "串口[%d]接收 (单条报文) dev->uart_data_recv()...SingleRecvLen=%d \n", nTtlIndex, SingleRecvLen);
                    CCODOWN_BUF_DEBUG(nTtlIndex, SingleRecvBuf, SingleRecvLen, "#接收 (单条报文)#: ");

                    //缓存区报文已满, 无法继续拼接
                    if(RecvLen >= sizeof(RecvBuf))
                    {
                        CCODOWN_FMT_DEBUG(nTtlIndex, "缓存区已满! sizeof(RecvBuf)=%d, RecvLen=%d \n", sizeof(RecvBuf), RecvLen);
                        SocketState = IPC_SOCK_SEND;
                        break;
                    }

                    //对单条报文进行拼接处理
                    if(SingleRecvLen > (sizeof(RecvBuf) - RecvLen))
                    {
                        SingleRecvLen = sizeof(RecvBuf) - RecvLen;
                    }
                    memcpy(&RecvBuf[RecvLen], SingleRecvBuf, SingleRecvLen);
                    RecvLen += SingleRecvLen;
                    //CCODOWN_BUF_DEBUG(nTtlIndex, RecvBuf, RecvLen, "拼接之后(RecvLen=%d)缓存报文: ", RecvLen);

                    //对拼接之后的总报文进行解析
                    checkRet1 = CheckFrameFormat(RecvLen, RecvBuf, &FrameLen, 1);
                    checkRet2 = CheckFrameFormat(RecvLen, RecvBuf, &FrameLen, 2);
                    CCODOWN_FMT_DEBUG(nTtlIndex, "checkRet1=%d, checkRet2=%d \n", checkRet1, checkRet2);
                    while ((checkRet1 > 0)  || (checkRet2 > 0))
                    {
                        g_CcoStats.recvNum++;
#ifdef AREA_FUJIAN
                        if (CcoPickFrame(amr, RecvBuf, FrameLen, nTtlIndex) != 0)
                        {
#endif
                            ret = InstantReportTaskDeal(RecvBuf, FrameLen, nTtlIndex);
                            CCODOWN_FMT_DEBUG(nTtlIndex, "InstantReportTaskDeal() ret=%d \n", ret);
                            if(ret < 0) //若判断为上报，则不再写入缓冲区
                            {
                                CCODOWN_FMT_DEBUG(nTtlIndex, "串口[%d]接收 (完整报文) if(InstantReportTaskDeal() < 0) ... 长度FrameLen:%d, 报文RecvBuf: \n", nTtlIndex, FrameLen);
                                CCODOWN_BUF_DEBUG(nTtlIndex, RecvBuf, FrameLen, "##接收 (完整报文)##: ");

                                ret = PlcWriteQueue(pAmrRecvDown[nTtlIndex], RecvBuf, FrameLen);
                                if (ret == 0)
                                {
                                    CCODOWN_FMT_DEBUG(nTtlIndex, "ret = PlcWriteQueue(pLockSend, RecvBuf, FrameLen);...if (ret == 0) \n");
                                    CCODOWN_FMT_DEBUG(nTtlIndex, "DownCcuThread is full! \n");

                                    //清空接收缓冲区
                                    ClearCommRecvBuff(nTtlIndex);
                                }
                            }           
#ifdef AREA_FUJIAN
                        }
#endif
                        RecvLen -= FrameLen;
                        memset(RecvBuf, 0x00, FrameLen);
                        memmove(RecvBuf, &RecvBuf[FrameLen], RecvLen);

                        checkRet1 = CheckFrameFormat(RecvLen, RecvBuf, &FrameLen, 1);
                        checkRet2 = CheckFrameFormat(RecvLen, RecvBuf, &FrameLen, 2);
                        CCODOWN_FMT_DEBUG(nTtlIndex, "while ((checkRet1 > 0) || (checkRet2 > 0))checkRet1=%d,checkRet2=%d,sendNum=%llu,recvNum=%llu\n", 
                                          checkRet1, checkRet2, g_CcoStats.sendNum, g_CcoStats.recvNum);
                                        
                    }
                }

                SocketState = IPC_SOCK_SEND;
                break;

            case IPC_SOCK_SEND:
                if((gDevHalInfo[nTtlIndex].isvalid == 0) || (1 == gReloadDevFlag[nTtlIndex]))
                {
                    SocketState = IPC_SOCK_INIT;
                    break;
                }
                
                SendLen = PlcReadQueue(pAmrSendDown[nTtlIndex], SendBuf);
                if (SendLen > 0)
                {
                    CCODOWN_FMT_DEBUG(nTtlIndex, "串口[%d]发送SendLen = PlcReadQueue(pLockRecv, SendBuf);...SendLen=%d \n", nTtlIndex, SendLen);
                    CCODOWN_BUF_DEBUG(nTtlIndex, SendBuf, SendLen, "@@发送报文@@:");

                    if(gDevHalInfo[nTtlIndex].dev == NULL)
                    {
                        SocketState = IPC_SOCK_INIT;
                        break;
                    }
                    ret = gDevHalInfo[nTtlIndex].dev->uart_data_send(gDevHalInfo[nTtlIndex].dev, SendBuf, SendLen);
                    g_CcoStats.sendNum++;
                    CCODOWN_FMT_DEBUG(nTtlIndex, "发送结果: nTtlIndex=%d,deviceName=%s,ret=%d,sendNum=%llu,recvNum=%llu\n",
                                      nTtlIndex, gDevHalInfo[nTtlIndex].deviceName, ret, g_CcoStats.sendNum, g_CcoStats.recvNum);
                    //ret = PlcSendTTL(fd_sock, SendBuf, SendLen);
                    //CCODOWN_FMT_DEBUG(nTtlIndex, "发送结果: nTtlIndex=%d, ret=%d \n", nTtlIndex, ret);

                    //写通信日志
                    memset(SendBuf, 0, sizeof(SendBuf));
                }
                SocketState = IPC_SOCK_RECV;
                break;

            case IPC_SOCK_CLOSE:
                //printx(" 串口[%d]关闭void *DownTtlThread(void *arg)...case IPC_SOCK_CLOSE: \n", nTtlIndex);
                ret = hal_device_release((HW_DEVICE *)gDevHalInfo[nTtlIndex].dev);
                CCODOWN_FMT_DEBUG(nTtlIndex, "hal_device_release() ret=%d \n", ret);
                hal_device_release((HW_DEVICE *)gDevHalInfo[nTtlIndex].ctrlDev);
                //PlcCloseTTL(fd_sock);
                //CCODOWN_FMT_DEBUG(nTtlIndex, "PlcCloseTTL()\n");
                SocketState = IPC_SOCK_INIT;
                break;

            default:
                ret = hal_device_release((HW_DEVICE *)gDevHalInfo[nTtlIndex].dev);
                CCODOWN_FMT_DEBUG(nTtlIndex, "hal_device_release() ret=%d \n", ret);
                hal_device_release((HW_DEVICE *)gDevHalInfo[nTtlIndex].ctrlDev);
                //PlcCloseTTL(fd_sock);
                //CCODOWN_FMT_DEBUG(nTtlIndex, "PlcCloseTTL()\n");
                SocketState = IPC_SOCK_INIT;
                break;
        }
       
        usleep(20000);                         //出让CPU资源
        cnt++;
    }

    pthread_exit(NULL);

}

#endif

/*******************************************************************************
* 函数名称: AmrThreadQueueInit
* 函数功能: 抄表队列初始化
* 输入参数: void *arg            指向参数区的指针
* 输出参数: 无
* 返 回 值: 无
*******************************************************************************/
void AmrThreadQueueInit(void *arg)
{
    AmrThreadArg_T  *AmrThreadArg;              //抄表线程参数
    AmrThreadArg = (AmrThreadArg_T *)arg;

    uint8           nTtlIndex = 0;  //不同的端口号对应不同的数组下标
    nTtlIndex = AmrThreadArg->nArrIndex;

    pAmrRecvDown[nTtlIndex] = AmrThreadArg->DownCommArg->SendBuf;
    pAmrSendDown[nTtlIndex] = AmrThreadArg->DownCommArg->RecvBuf;
    pAmrRecvUp = AmrThreadArg->UpIpcArg->SendBuf;
    pAmrSendUp = AmrThreadArg->UpIpcArg->RecvBuf;

    if(pAmrRecvDown[nTtlIndex] != NULL)  CCODOWN_FMT_DEBUG(nTtlIndex, "if(pAmrRecvDown[%d] != NULL) \n", nTtlIndex);
    if(pAmrSendDown[nTtlIndex] != NULL)  CCODOWN_FMT_DEBUG(nTtlIndex, "if(pAmrSendDown[%d] != NULL) \n", nTtlIndex);

    CCODOWN_FMT_DEBUG(nTtlIndex, "pAmrRecvDown[%d]:%p \n", nTtlIndex, pAmrRecvDown[nTtlIndex]);
    CCODOWN_FMT_DEBUG(nTtlIndex, "pAmrSendDown[%d]:%p \n", nTtlIndex, pAmrSendDown[nTtlIndex]);

    if(pAmrRecvUp != NULL)  CCODOWN_FMT_DEBUG(nTtlIndex, "if(pAmrRecvUp != NULL) \n");
    if(pAmrSendUp != NULL)  CCODOWN_FMT_DEBUG(nTtlIndex, "if(pAmrSendUp != NULL) \n");

    CCODOWN_FMT_DEBUG(nTtlIndex, "pAmrRecvUp:%p \n", pAmrRecvUp);
    CCODOWN_FMT_DEBUG(nTtlIndex, "pAmrSendUp:%p \n", pAmrSendUp);
}


/*******************************************************************************
* 函数名称: AmrThread
* 函数功能: 抄表逻辑处理线程
* 输入参数: void *arg            指向参数区的指针
* 输出参数: 无
* 返 回 值: 无
*******************************************************************************/
void *AmrThread(void *arg)
{
    AmrThreadArg_T  *AmrThreadArg;              //抄表线程参数
    AmrThreadArg = (AmrThreadArg_T *)arg;
#ifdef AREA_FUJIAN
    CCO_AMR_T  *amr = &g_CcoAmr;
#endif
    uint8           nTtlIndex = 0;  //不同的端口号对应不同的数组下标
    nTtlIndex = AmrThreadArg->nArrIndex;
    if((nTtlIndex >= MAX_DOWN_THREAD_NUM)
        || (nTtlIndex >= MAX_ALLOWED_DOWN_THREAD_NUM))
    {
        CCODOWN_FMT_DEBUG(0, "AmrThread() 线程数组下标nTtlIndex=%d异常! \n", nTtlIndex);
        return NULL;
    }

    CCODOWN_FMT_DEBUG(nTtlIndex, "void *AmrThread(void *arg) \n");

    //线程名
    char ThreadName[30] = {0};
    sprintf(ThreadName, "CcoAmr-%d", nTtlIndex);
    prctl(PR_SET_NAME,ThreadName);

    AmrThreadQueueInit(arg);                    //线程队列初始化
    PastTimeDealInit(nTtlIndex);                        //时间过处理初始化
    AmrInfoInit(nTtlIndex);                             //抄表信息初始化
    AllStateInit(nTtlIndex);                            //状态字初始化
    AllFlagInit(nTtlIndex);                             //标志初始化

    CCODOWN_FMT_DEBUG(nTtlIndex, "AmrThread() 初始化全部结束! \n");
    
    while(1)
    {
        gAmrThreadFlag[nTtlIndex] = 1;  //用于检测线程运行是否正常

        if((gDevHalInfo[nTtlIndex].isvalid == 0) || (1 == gReloadDevFlag[nTtlIndex]))
        {
            CCODOWN_FMT_DEBUG(nTtlIndex, "AmrThread() 串口[%d] isvalid=%d, gReloadDevFlag=%d \n", nTtlIndex, gDevHalInfo[nTtlIndex].isvalid, gReloadDevFlag[nTtlIndex]);
            sleep(5);  //模块信息无效, 线程空转
            continue;
        }

        //如果当前没有正在执行档案同步，并且消息那边已经收到档案变更，则要将消息那边的临时档案先拷贝至实际使用的全局变量
        if(0 == gMeterFileModifyState[nTtlIndex])
        {
            if((1==gMeterFileAddDelFlagTemp[nTtlIndex]) || (1==gMeterFileClearFlagTemp[nTtlIndex]))
            {
                CCODOWN_FMT_DEBUG(nTtlIndex, "消息更新的档案标志状态 gMeterFileAddDelFlagTemp=%d, gMeterFileClearFlagTemp=%d \n", gMeterFileAddDelFlagTemp[nTtlIndex], gMeterFileClearFlagTemp[nTtlIndex]);
                gMeterFileAddDelFlag[nTtlIndex] |= gMeterFileAddDelFlagTemp[nTtlIndex];
                gMeterFileClearFlag[nTtlIndex] |= gMeterFileClearFlagTemp[nTtlIndex];
                gMeterFileAddDelFlagTemp[nTtlIndex] = 0;
                gMeterFileClearFlagTemp[nTtlIndex] = 0;
                memcpy(&gAmrMeterFilesList[nTtlIndex], &gAmrMeterFilesListTemp[nTtlIndex], sizeof(gAmrMeterFilesList[nTtlIndex]));
                gMeterFileCompareFlag[nTtlIndex] = 1;
                CCODOWN_FMT_DEBUG(nTtlIndex, "档案同步参考的状态 gMeterFileAddDelFlag=%d, gMeterFileClearFlag=%d, gMeterFileCompareFlag=%d \n", 
                    gMeterFileAddDelFlagTemp[nTtlIndex], gMeterFileClearFlagTemp[nTtlIndex], gMeterFileCompareFlag[nTtlIndex]);
                
            }
        }

        TimeManage(nTtlIndex);                   //时间管理函数
#ifdef AREA_FUJIAN
        /* 福建规约处理函数 */
        CcoFuJianAmr(amr, nTtlIndex);
#endif
        PlcTaskCreat(nTtlIndex);                 //任务的产生判断
        PlcTaskRunRouterDeal(nTtlIndex);         //任务的执行处理(路由操作)
        PlcTaskRun(nTtlIndex);                   //任务的执行处理
        PlcTaskRunSearchMeter(nTtlIndex);        //任务的执行处理(搜表)

        usleep(50000);
    }

    pthread_exit(NULL);

}

/*******************************************************************************
* 函数名称: init_config
* 函数功能: 配置文件初始化
* 输入参数: 无
* 输出参数: 无
* 返 回 值: 0 成功; 非0：错误
*******************************************************************************/
int init_config()
{
    dictionary *ini = NULL;
	char* tmp;

    char   curpath[256] = {0};
    char   filename[256] = {0};
    int i;

    //获取当前目录绝对路径，即去掉程序名
    int cnt = readlink("/proc/self/exe", curpath, 256);
    if(cnt < 0 || cnt >= 256)
    {
        printf("ccoRouter get err path!\n");
    }
    else
    {
        for (i = cnt; i >=0; --i)
        {
            if (curpath[i] == '/')
            {
                curpath[i+1] = '\0';
                break;
            }
        }
    }
    sprintf(filename, "%s%s", curpath, CCO_ROUTER_INI_FILE);
    printf("配置文件filename: %s!\n", filename);
    ini = iniparser_load(filename);
	
    if(NULL != ini)
    {
        /* 配置文件内容获取 */
        tmp = iniparser_getstring(ini, "mqtt:MQTTIP", "172.17.0.1");
        printf("读取mqttip = %s \n", tmp);
        strcpy(mqttIP, tmp);
        mqttport = iniparser_getint(ini, "mqtt:MQTTPORT", 1883);
        printf("读取mqttport = %d \n", mqttport);

        iniparser_freedict(ini);
    }
    else
    {
        printf("Get no ini file: %s!\n", CCO_ROUTER_INI_FILE);
    }

    /* 日志模块初始化 */
    diag_init(filename, TRUE);

    return 0;
}

/*******************************************************************************
* 函数名称: main
* 函数功能: 主函数 创建与维护线程
* 输入参数: 无
* 输出参数: 无
* 返 回 值: >=0:正确； -1:错误
*******************************************************************************/
int main(int argc, char *argv[])
{
    CCOUP_FMT_DEBUG("ccoRouter main() start ... \n");
    BOOL bOpenGui = 0;  //是否打开显示GUI(测试完成之后再打开)

    /* 检查程序只启动了一个副本 */
    if(!singleton_check("ccoRouter"))
    {
        return -1;
    }

    if(argc >= 2)
    {
        if(0 == memcmp(argv[1], "debug", strlen("debug")))
        {
            CCOUP_FMT_DEBUG("ccoRouter main() 30s waiting...\n");
            sleep(30);
        }
    }

    //线程名
    /*
    char ThreadName[30] = {0};
    sprintf(ThreadName, "CcoMain-%d", 0);
    prctl(PR_SET_NAME,ThreadName);
    */
    /* 信号处理 */
    init_sig_action("/data/app/ccoRouter/log/Exception.log");

    /* 读取配置文件 */
    init_config();

    /* 全局变量初始化 */
    GlobalInit();
    
    uint8       i = 0;
    
    pthread_t   UpIpcThreadID;                  //上行IPC通信线程标识
    pthread_t   DownTtlThreadID[MAX_DOWN_THREAD_NUM];                //下行串口通信线程标识
    pthread_t   AmrThreadID[MAX_DOWN_THREAD_NUM];                    //自动抄表线程标识

    LockQueue_T LockRecvUp;                     //上行接收缓冲区结构
    LockQueue_T LockSendUp;                     //上行发送缓冲区结构

    LockQueue_T LockRecvDown[MAX_DOWN_THREAD_NUM];                   //下行接收缓冲区结构
    LockQueue_T LockSendDown[MAX_DOWN_THREAD_NUM];                   //下行发送缓冲区结构

    ThreadArg_T UpIpcThreadArg;                 //上行IPC线程参数
    ThreadArg_T DownTtlThreadArg[MAX_DOWN_THREAD_NUM];              //下行串口通信线程参数
    AmrThreadArg_T AmrThreadArg[MAX_DOWN_THREAD_NUM];               //抄表线程参数

    uint8 UpIpcThreadCnt = 0;
    uint8 DownTtlThreadCnt[MAX_DOWN_THREAD_NUM] = {0};
    uint8 AmrThreadCnt[MAX_DOWN_THREAD_NUM] = {0};

    int32 err;
    //int ret = 0;

    CCOUP_FMT_DEBUG("ccoRouter main()  \n");
    CCOUP_FMT_DEBUG("ccoRouter main()  -COMPILE TIME : %s %s\n", __TIME__, __DATE__);
    CCOUP_FMT_DEBUG("ccoRouter main()  -GCC VERSION  : %s\n", __VERSION__);

    /*mqtt初始化 */
    if(mqtt_init() < 0)
    {
        CCOUP_FMT_DEBUG("ccoRouter main() mqtt_init() 初始化失败! \n");
        exit(1);
    }       

    /* hal初始化 */
    while(0 != hal_init())
    {
        CCOUP_FMT_DEBUG("ccoRouter main() hal_init() 初始化失败! \n");
        sleep(1);
    }

    /* hal获取appid */
    appid_query(AppIdName, AppId, APP_ID_LEN);

    //主动上送消息INDEX锁初始化
    if ( pthread_mutex_init ( &ActiveInfoIndexLock, NULL ) != 0 )
    {
        CCOUP_FMT_DEBUG("ccoRouter main() init pthread ActiveInfoIndexLock lock error! \n");
        exit (1);
    }
    
    //上行IPC通信线程初始化
    memset(&LockRecvUp, 0, sizeof(LockRecvUp));
    memset(&LockSendUp, 0, sizeof(LockSendUp));

    if (pthread_mutex_init(&LockRecvUp.QueueLock, NULL) != 0)
    {
        CCOUP_FMT_DEBUG("ccoRouter main() init pthread LockRecvUp lock error! \n");
        exit(1);
    }
    if (pthread_mutex_init(&LockSendUp.QueueLock, NULL) != 0)
    {
        CCOUP_FMT_DEBUG("ccoRouter main() init pthread LockSendUp lock error! \n");
        exit(1);
    }

    UpIpcThreadArg.RecvBuf = &LockRecvUp;
    UpIpcThreadArg.SendBuf = &LockSendUp;

    //下行通信线程参数初始化
    for ( i = 0; i < MAX_DOWN_THREAD_NUM; i++ )
    {
        memset(&LockRecvDown[i], 0, sizeof(LockRecvDown[i]));
#ifdef AREA_FUJIAN
        memset(&g_CcoRecvDown[i], 0, sizeof(g_CcoRecvDown[i]));
#endif
        memset(&LockSendDown[i], 0, sizeof(LockSendDown[i]));

        if (pthread_mutex_init(&LockRecvDown[i].QueueLock, NULL) != 0)
        {
            CCOUP_FMT_DEBUG("ccoRouter main() init pthread LockRecvDown lock error! i=%d \n", i);
            exit(1);
        }
#ifdef AREA_FUJIAN
        if (pthread_mutex_init(&g_CcoRecvDown[i].QueueLock, NULL) != 0)
        {
            CCOUP_FMT_DEBUG("ccoRouter main() init pthread g_CcoRecvDown lock error! i=%d \n", i);
            exit(1);
        }
#endif
        if (pthread_mutex_init(&LockSendDown[i].QueueLock, NULL) != 0)
        {
            CCOUP_FMT_DEBUG("ccoRouter main() init pthread LockSendDown lock error! i=%d \n", i);
            exit(1);
        }


        DownTtlThreadArg[i].nArrIndex = i;
        DownTtlThreadArg[i].RecvBuf = &LockRecvDown[i];
        DownTtlThreadArg[i].SendBuf = &LockSendDown[i];
    }


    //自动抄表线程初始化
    for ( i = 0; i < MAX_DOWN_THREAD_NUM; i++ )
    {
        AmrThreadArg[i].nArrIndex = i;
        AmrThreadArg[i].DownCommArg = &DownTtlThreadArg[i];
        AmrThreadArg[i].UpIpcArg = &UpIpcThreadArg;
    }

#if MSG_MQTT == 1
    //创建上行IPC线程
    CCOUP_FMT_DEBUG("ccoRouter main() 当前上行通信方式为MQTT. \n");
    err = pthread_create(&UpIpcThreadID, NULL, UpMqttThread, &UpIpcThreadArg);
    if(err != 0)
    {
        CCOUP_FMT_DEBUG("ccoRouter main() 0pthread_create UpIpcThread error! \n");
        exit(1);
    }
#else
    //创建上行IPC线程
    CCOUP_FMT_DEBUG("ccoRouter main() 当前上行通信方式为UDP. \n");
    err = pthread_create(&UpIpcThreadID, NULL, UpIpcThread, &UpIpcThreadArg);
    if(err != 0)
    {
        CCOUP_FMT_DEBUG("ccoRouter main() 0pthread_create UpIpcThread error! \n");
        exit(1);
    }
#endif


#if DOWN_DEV_TYPE == 1
    //创建下行通信线程
    CCOUP_FMT_DEBUG("ccoRouter main() 当前本地通信模块为鼎信. \n");
    for ( i = 0; i < MAX_DOWN_THREAD_NUM; i++ )
    {
        err = pthread_create(&DownTtlThreadID[i], NULL, DownTtyacmThread, &DownTtlThreadArg[i]);
        if(err != 0)
        {
            CCOUP_FMT_DEBUG("ccoRouter main() 0pthread_create DownTtlThread[%d] error! \n", i);
            exit(1);
        }
    }
#elif DOWN_DEV_TYPE == 0
    //创建下行通信线程
    CCOUP_FMT_DEBUG("ccoRouter main() 当前本地通信模块为智芯. \n");
    for ( i = 0; i < MAX_DOWN_THREAD_NUM; i++ )
    {
        err = pthread_create(&DownTtlThreadID[i], NULL, DownTtlThread, &DownTtlThreadArg[i]);
        if(err != 0)
        {
            CCOUP_FMT_DEBUG("ccoRouter main() 0pthread_create DownTtlThread[%d] error! \n", i);
            exit(1);
        }
    }
#elif DOWN_DEV_TYPE == 2
    //创建下行通信线程
    CCOUP_FMT_DEBUG("ccoRouter main() 当前本地通信模块为HAL接口. \n");
    for ( i = 0; i < MAX_DOWN_THREAD_NUM; i++ )
    {
        err = pthread_create(&DownTtlThreadID[i], NULL, DownTtlHalThread, &DownTtlThreadArg[i]);
        if(err != 0)
        {
            CCOUP_FMT_DEBUG("ccoRouter main() 0pthread_create DownTtlHalThread[%d] error! \n", i);
            exit(1);
        }
    }
#elif DOWN_DEV_TYPE == 3
    //创建下行通信线程
    CCOUP_FMT_DEBUG("ccoRouter main() 当前本地通信模块为HAL接口. \n");
    for ( i = 0; i < MAX_DOWN_THREAD_NUM; i++ )
    {
        err = pthread_create(&DownTtlThreadID[i], NULL, DownCcuThread, &DownTtlThreadArg[i]);
        if(err != 0)
        {
            CCOUP_FMT_DEBUG("ccoRouter main() 0pthread_create DownCcuThread[%d] error! \n", i);
            exit(1);
        }
    }

#endif

    //创建抄表线程
    for ( i = 0; i < MAX_DOWN_THREAD_NUM; i++ )
    {
        err = pthread_create(&AmrThreadID[i], NULL, AmrThread, &AmrThreadArg[i]);
        if(err != 0)
        {
            CCOUP_FMT_DEBUG("ccoRouter main() 0pthread_create AmrThread[%d] error! \n", i);
            exit(1);
        }
    } 

    time_t CommBeginTime= time(NULL);  //通讯开始时间

    //gui显示
    bOpenGui = 1;
    if(bOpenGui)
    {
        ccoRouter_gui_entry();
    }   
    
    while(1)
    {
        sleep(1);
#if 0
        if( (1 == gMapManagerAppLoginFlag) && (1 == gDBCenterAppLoginFlag) && (1 == bAppSelfLoginState) )
        {
            //查询模组信息
            if(gQueryAllModuleFlag == TRUE)//程序刚启动初始化, 查询所有模块信息
            {
                sleep(5);
                
                CCOUP_FMT_DEBUG("ccoRouter main() if(gQueryAllModuleFlag == TRUE)...\n");
                //查询所有模组信息
                if(GetAllModuleInfo() > 0)
                {
                    UpdateModuleInfo();
                    gQueryAllModuleFlag = FALSE;
                }
            }
            else//判断是否存在模块插拔事件
            {
                //判断是否插拔查询模块信息
                if(gReloadModuleFlag[0] == 0)
                {
                    if(0 != memcmp(&gReloadModuleFlag[1], gNullArr, 5))
                    {
                        gReloadModuleFlag[0] = 1;
                    }
                }

                if(gReloadModuleFlag[0] == 1)
                {
                    CCOUP_FMT_DEBUG("ccoRouter main() else if(gReloadModuleFlag[0] == 1)...\n");
                    //查询单个模组信息
                    ret = ModuleEventDeal();
                    if( (ret>0) && (ret<6) )
                    {
                        UpdateModuleInfo();
                        gReloadModuleFlag[0] = 0;

                        //判断插入是否本地模块 (插入需要在更新之后判断是否本地模块)
                        if( gModuleInfo[ret-1].bCCO == TRUE )
                        {
                            //更新本地模块档案信息
                            UpdateModuleFiles();
                        }
                    }
                }           
            }
        }   
#endif
        //检查程序运行方式
        //printx("111111 \n");

        for ( i = 0; i < MAX_DOWN_THREAD_NUM; i++ )
        {
            if(gRebootAmrThread[i] == 1)//如果抄表线程通知需要立即重启
            {
                CCOUP_FMT_DEBUG("ccoRouter main() AmrThread[%d] timeout,  check module insert ! \n", i);
#ifdef AREA_FUJIAN
                g_CcoAmr.flag.initFlag = UINT_MAX;
#endif
                gRebootAmrThread[i] = 0;
                pthread_cancel(AmrThreadID[i]);
                pthread_join(AmrThreadID[i], NULL);
                err = pthread_create(&AmrThreadID[i], NULL, AmrThread, &AmrThreadArg[i]);
                if (err != 0)
                {
                    CCOUP_FMT_DEBUG("ccoRouter main() pthread_create AmrThread[%d] error \n", i);
                }
            }
        }
        

        //120秒检测一次线程运行情况
        //printx("222222 \n");
        if( abs(time(NULL)-CommBeginTime) > 120)
        {
            //更新计时器
            CommBeginTime= time(NULL);

            //检测上行IPC通信线程
            if (gUpIpcThreadFlag == 1)
            {
                gUpIpcThreadFlag = 0;               //清线程运行不正常标志及次数
                UpIpcThreadCnt = 0;
            }
            else
            {
                if (UpIpcThreadCnt >= 3)            //若3次检测都不正常，则关闭此线程，然后重新打开此线程
                {
                    CCOUP_FMT_DEBUG("ccoRouter main() UpIpcThread timeout, so reset it \n");

                    err = pthread_cancel(UpIpcThreadID);
                    pthread_join(UpIpcThreadID, NULL);
                #if MSG_MQTT == 1
                    //创建上行IPC线程
                    err = pthread_create(&UpIpcThreadID, NULL, UpMqttThread, &UpIpcThreadArg);
                #else
                    //创建上行IPC线程
                    err = pthread_create(&UpIpcThreadID, NULL, UpIpcThread, &UpIpcThreadArg);
                #endif   
                    if (err != 0)
                    {
                        CCOUP_FMT_DEBUG("ccoRouter main() pthread_create UpIpcThread error \n");
                    }
                    else
                    {
                        UpIpcThreadCnt = 0;
                    }
                }
                else
                {
                    UpIpcThreadCnt++;
                }
            }

            //检测下行通信线程
            //printx("333333 \n");
            for ( i = 0; i < MAX_DOWN_THREAD_NUM; i++ )
            {
                if (gDownTtlThreadFlag[i] == 1)
                {
                    gDownTtlThreadFlag[i] = 0;
                    DownTtlThreadCnt[i] = 0;
                }
                else
                {
                    if (DownTtlThreadCnt[i] >= 3)
                    {
                        CCOUP_FMT_DEBUG("ccoRouter main() DownTtlThread[%d] timeout, so reset it \n", i);

                        err = pthread_cancel(DownTtlThreadID[i]);
                        pthread_join(DownTtlThreadID[i], NULL);
                    #if DOWN_DEV_TYPE == 1
                        err = pthread_create(&DownTtlThreadID[i], NULL, DownTtyacmThread, &DownTtlThreadArg[i]);
                    #elif DOWN_DEV_TYPE == 0
                        err = pthread_create(&DownTtlThreadID[i], NULL, DownTtlThread, &DownTtlThreadArg[i]);
                    #elif DOWN_DEV_TYPE == 2
                        err = pthread_create(&DownTtlThreadID[i], NULL, DownTtlHalThread, &DownTtlThreadArg[i]);
                    #elif DOWN_DEV_TYPE == 3
                        err = pthread_create(&DownTtlThreadID[i], NULL, DownCcuThread, &DownTtlThreadArg[i]);
                    #endif
                        if (err != 0)
                        {
                            CCOUP_FMT_DEBUG("ccoRouter main() pthread_create DownTtlThread[%d] error \n", i);
                        }
                        else
                        {
                            DownTtlThreadCnt[i] = 0;
                        }
                    }
                    else
                    {
                        DownTtlThreadCnt[i]++;
                    }
                }
            } 
            

            //检测抄表线程
            //printx("444444 \n");
            for ( i = 0; i < MAX_DOWN_THREAD_NUM; i++ )
            {
                if (gAmrThreadFlag[i] == 1)
                {
                    gAmrThreadFlag[i] = 0;
                    AmrThreadCnt[i] = 0;
                }
                else
                {
                    if (AmrThreadCnt[i] >= 3)
                    {
                        CCOUP_FMT_DEBUG("ccoRouter main() AmrThread[%d] timeout, so reset it \n", i);

                        err = pthread_cancel(AmrThreadID[i]);
                        pthread_join(AmrThreadID[i], NULL);
                        err = pthread_create(&AmrThreadID[i], NULL, AmrThread, &AmrThreadArg[i]);
                        if (err != 0)
                        {
                            CCOUP_FMT_DEBUG("ccoRouter main() pthread_create AmrThread[%d] error \n", i);
                        }
                        else
                        {
                            AmrThreadCnt[i] = 0;
                        }
                    }
                    else
                    {
                        AmrThreadCnt[i]++;
                    }
                }
            }
        }

    }

    //printx("555555 \n");
    pthread_join(UpIpcThreadID, NULL);
    for ( i = 0; i < MAX_DOWN_THREAD_NUM; i++ )
    {
        pthread_join(DownTtlThreadID[i], NULL);
    } 
    for ( i = 0; i < MAX_DOWN_THREAD_NUM; i++ )
    {
        pthread_join(AmrThreadID[i], NULL);
    }
    

    for ( i = 0; i < MAX_DOWN_THREAD_NUM; i++ )
    {
        pthread_mutex_destroy(&LockRecvDown[i].QueueLock);
        pthread_mutex_destroy(&LockSendDown[i].QueueLock);
    }    
    pthread_mutex_destroy(&LockRecvUp.QueueLock);
    pthread_mutex_destroy(&LockSendUp.QueueLock);

    CCOUP_FMT_DEBUG("ccoRouter main() exit ccoRouter main() \n");

    exit(1);
}















