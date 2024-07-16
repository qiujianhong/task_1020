/*
*********************************************************************
* Copyright(C) 2019 �Ͼ��������ӹɷ����޹�˾
* All rights reserved. 
* @brief�� ��������
* @date�� 2019-10-25
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

// mqtt �ͻ��˾��
extern MQTT_CLIENT_T *g_MqttClient;

/*******************************************************************************
* ȫ�ֱ�������
*******************************************************************************/
#define  CCO_ROUTER_INI_FILE            "ccoRouter.ini"

pthread_mutex_t ActiveInfoIndexLock;

uint16	CommDelayCnt = 0;			            //ͨ����ʱ������
uint8 gUpIpcThreadFlag;                         //����IPC�̱߳�־
uint8 gDownTtlThreadFlag[MAX_DOWN_THREAD_NUM] = {0};  //���д���ͨ���̱߳�־
uint8 gAmrThreadFlag[MAX_DOWN_THREAD_NUM] = {0};                       //�Զ������̱߳�־
uint8 gRebootAmrThread[MAX_DOWN_THREAD_NUM] = {0};                     //���������̱߳�־

COMM_JOIN_MESSAGE_T gCommJoinMessage[MAX_DOWN_THREAD_NUM];  //����ƴ�ӱ�����Ϣ

//mqtt ip��ַ��˿ں�
char mqttIP[100] = {0};
int mqttport = 0;

//�Ƿ��ע��app����ע��
//uint8 gMapManagerAppLoginFlag = 0;
uint8 gDBCenterAppLoginFlag = 1;//����������û�н���

//��app�Ƿ��Ѿ�ע��ɹ�
uint8 bAppSelfLoginState = 0;

//appname: appӢ������(���ڲ�ѯappid)
char *AppIdName = "ccoRouter";

//appid: ��Ӧapp��ID��Ϣ
char  AppId[APP_ID_LEN] = {0};

/**********************************************************************
* @name      : ccoRouter_gui_reg
* @brief     ������APP��ʾע��
* @param[in] ��
* @param[out]��
* @return    ��0-�ɹ�/������
* @Create    : ÷����
* @Date      ��2020-5-12
* @Update    : 
**********************************************************************/
int ccoRouter_gui_reg()
{
    const char*		appName = "ccoRouter";
	const char*		iconFile ="/usr/local/extapps/ccoRouter/bin/cfg/ccoRouter.bmp";
	APP_REGIST_INFO_T appRegInfo;		//������gui���͵�ע����Ϣ
    int           ret = 0;
    
    memset(&appRegInfo,0,sizeof(APP_REGIST_INFO_T));
    //APP����
	memcpy(appRegInfo.appname,appName,strlen(appName));
    memcpy(appRegInfo.shortname, ADV_SHOW_NAME, strlen(ADV_SHOW_NAME));
    //app��ʾͼ��
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
* @brief     ������APP��ʾ
* @param[in] ��
* @param[out]��
* @return    ��
* @Create    : ÷����
* @Date      ��2020-5-12
* @Update    : 
**********************************************************************/
void* ccoRouter_gui_entry()
{

    pthread_t mdl_gui_id;
    
    /* ������ʼ�� todo */
    initlize_desk("ccoRouter",mqtt_topic_send_msg);

    CCOGUI_FMT_DEBUG("ccoRouter_gui_entry() Start to reg Appgui... \n");
    /* APPע��gui */
    while(getLoginguiID() == 0)
    {
        ccoRouter_gui_reg();
        sleep(2);
    }

    /* ����ҵ�� */
    AppPluginReg(g_plugin, g_nPluginCnt);
    if (pthread_create(&mdl_gui_id,NULL,app_gui_entry,NULL) != 0)				/*mdl_gui �߳�*/
    {
        CCOGUI_FMT_DEBUG("creating the app_gui_entry failed!\n");
    }

	CCOGUI_FMT_DEBUG("ccoRouter_gui_entry() end... \n");    
    return 0;
}

/**********************************************************************
* @name      : ccoRouter_recv_appGUI_back_handle
* @brief     ������APP����gui��Ϣ����
* @param[in] ��
* @param[out]��
* @return    ��0-�ɹ�/������
* @Create    : ÷����
* @Date      ��2020-5-12
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
* ��������: UpMqttThread
* ��������: �����߳�(MQTT��Ϣ����)
* �������: void *arg            ָ���������ָ��
* �������: ��
* �� �� ֵ: ��
*******************************************************************************/
#if MSG_MQTT == 1
void *UpMqttThread(void *arg)
{
    CCOUP_FMT_DEBUG("void *UpMqttThread(void *arg) \n");

    //�߳���
    char ThreadName[30] = {0};
    sprintf(ThreadName, "CcoUpMqtt-%d", 0);
    prctl(PR_SET_NAME,ThreadName);

    static time_t LastLoginTime;
    int8    SocketState = IPC_SOCK_INIT;        //����socket����״̬
    uint8   RecvBuf[MAX_QUEUE_BUFF_LEN] = {0}; //socket���ջ�����
    uint8   SendBuf[MAX_QUEUE_BUFF_LEN] = {0}; //socekt���ͻ�����
    uint8   SendInfoBuf[MAX_QUEUE_BUFF_LEN] = {0}; //socekt���ͻ�����    
    int32   RecvLen = 0;                        //�������ݳ���
    int32   SendLen = 0;                        //�������ݳ���
    int32   SendInfoLen = 0;                        //�������ݳ���
    ThreadArg_T *pThreadArg;                    //�̲߳���
    LockQueue_T *pLockRecv;                     //�ز������ջ�����ָ��
    
    int     ret;

    MSG_INFO_T *ptMsgInfo = NULL;
    MSG_INFO_T *pMsgInfoSend = NULL;
    
    char topic[500] = {0};  //����

    memset(RecvBuf, 0x00, sizeof(RecvBuf));
    memset(SendBuf, 0x00, sizeof(SendBuf));
    memset(SendInfoBuf, 0x00, sizeof(SendInfoBuf));

    pThreadArg = (ThreadArg_T *)arg;            //��ʼ�����ṹָ��
    pLockRecv = pThreadArg->RecvBuf;

    INFO_DATA_T stInfoData;  //��Ϣ
    memset(&stInfoData, 0x00, sizeof(stInfoData));

    //uint16 nMapManagerSendIndex = 0;//������Ϣindex
    //time_t MapManagerLastCheckTime = abs(time(NULL) - 5);//��ѯʱ��

    //uint16 nDBCenterSendIndex = 0;//������Ϣindex
    //time_t DBCenterLastCheckTime = abs(time(NULL) - 5);//��ѯʱ��

    //BOOL tempFlag = TRUE;

    while(1)
    {
        gUpIpcThreadFlag = 1;                  //���ڼ���߳������Ƿ�����
        GetCurBcdTime(&gCurBcdTimeUp);         //ˢ�µ�ǰBCD��ʱ��

        switch (SocketState)
        {
            case IPC_SOCK_INIT:
            {
                //printx(" void *UpMqttThread(void *arg)...case IPC_SOCK_INIT: \n");                        
#if 0
                if(0 == gMapManagerAppLoginFlag)//�鿴ģ��������Ƿ�ע��ɹ�
                {
                    if( abs(time(NULL) - MapManagerLastCheckTime) > 2 )
                    {
                        CCOUP_FMT_DEBUG("pthread_mqtt() ACIPC_INIT...��ѯģ�������ע��״̬ \n");
                        MapManagerLastCheckTime = time(NULL);
                        check_app_login_state(mapManagerName, &nMapManagerSendIndex);
                    }
                    SocketState = IPC_SOCK_RECV;
                    
                }
                else if(0 == gDBCenterAppLoginFlag)//�鿴���������Ƿ�ע��ɹ�
                {
                    if( abs(time(NULL) - DBCenterLastCheckTime) > 2 )
                    {
                        CCOUP_FMT_DEBUG("pthread_mqtt() ACIPC_INIT...��ѯ��������ע��״̬ \n");
                        DBCenterLastCheckTime = time(NULL);
                        check_app_login_state(dbName, &nDBCenterSendIndex);
                    }
                    SocketState = IPC_SOCK_RECV;
                    
                }
#endif
                //else
                //{
                //ת��ע��
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
                //���ؽ������ݽ���         
                memset(RecvBuf,0x00,sizeof(RecvBuf));
                RecvLen = mqtt_read(RecvBuf);
                
                if(RecvLen > 0)
                {
                    CCOUP_FMT_DEBUG("Recv Up IPC data... RecvLen=%d \n", RecvLen);
                    CCOUP_BUF_DEBUG(RecvBuf, RecvLen, "����mqtt����:");
                    
                    //����Ϣ������MSG_INFO_T 
                    ret = msg_mqtt_split(RecvBuf,RecvLen, 0, 0, &ptMsgInfo);
                    if(ret < 0)
                    {
                        CCOUP_FMT_DEBUG("UpMqttThread() msg_mqtt_split() ����ʧ�� \n");
                        break;
                    }
                    
                    //��ӡ������Ϣ����
                    CCOUP_FMT_DEBUG("UpMqttThread()...IPC_SOCK_RECV...ͨ��Mqtt�յ�����ϢΪ: \n");
                    PrintInfoDetail(ptMsgInfo);

                    if(ptMsgInfo->IID == 0x1155)
                    {
                        switch(ptMsgInfo->IOP)
                        {
                        case 0x9090:  //��ѯ���̰汾��Ϣ
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
                            case 0x0010:  //����
                            {
                                heart_beat(ptMsgInfo->MsgIndex);
                            }
                                break;
                            case 0x0013://�汾��Ϣ
                            {
                                Appversion_Response(ptMsgInfo);
                            }
                                break;
                            case 0x0016://appid��Ϣ
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
                            case 0x0010://ע����Ӧ
                            {
                                bAppSelfLoginState = 1;
                            }
                                break;
                            case 0x0013://��ѯָ��App��Ϣ
                            {
#if 0
                                if(0 == gMapManagerAppLoginFlag)//�鿴ģ��������Ƿ�ע��ɹ�
                                {
                                    gMapManagerAppLoginFlag = unpack_login_info(ptMsgInfo, mapManagerName, nMapManagerSendIndex);
                                }

                                if(0 == gDBCenterAppLoginFlag)//�鿴���������Ƿ�ע��ɹ�
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
                    else if(ptMsgInfo->IID == 0x0004)//ģ�������
                    {
                        switch (ptMsgInfo->IOP)
                        {
                            case 0x0001://ģ�����¼�
                            {
                                ModuleEventFun(ptMsgInfo->MsgData, ptMsgInfo->MsgLen);
                            }
                                break;
                            case 0x0010://��ѯģ����Ϣ��Ӧ
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
                        //�����MSG����
                        if(ptMsgInfo->MsgLen > MAX_MSG_PAYLOAD_LEN)
                        {
                            CCOUP_FMT_DEBUG("����! if(ptMsgInfo->MsgLen > MAX_MSG_PAYLOAD_LEN) \n");
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

                    //������Ϣ��������
                    CreateTaskByInfo();

                    usleep(2000);                         //����CPU��Դ
                }
                else
                {
                    usleep(10000);                         //����CPU��Դ
                }               

                /* if((0 == gMapManagerAppLoginFlag) || (0 == gDBCenterAppLoginFlag) )
                {
                    SocketState = IPC_SOCK_INIT;
                }*/
                //else
                //{
                //�ж��Ƿ��¼�ɹ�
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
                
                //�鹲��ķ��ͻ�����        
                SendLen = PlcReadQueue(pLockRecv, SendBuf);
                if (SendLen > 0)
                {                   
                    //��ȡMSG
                    memset(&stInfoData, 0x00, sizeof(stInfoData));
                    memcpy(&stInfoData, SendBuf, SendLen);
                    
                    //�����ڴ�
                    pMsgInfoSend = (MSG_INFO_T *)malloc(SendLen);
                    memset(pMsgInfoSend, 0x00, SendLen);

                    //��ȡHEAD
                    GetHeadInfo(stInfoData, pMsgInfoSend);

                    //��Ϣ����
                    memset(topic, 0x00, sizeof(topic));
                    sprintf(topic,"%s/%s", pMsgInfoSend->SourAddr, pMsgInfoSend->DestAddr);

                    //��ӡ������Ϣ����
                    CCOUP_FMT_DEBUG("UpMqttThread()...IPC_SOCK_SEND...ͨ��MQTT���͵���ϢΪ: (topic:%s) \n", topic);
                    PrintInfoDetail(pMsgInfoSend);

                    //����Ϣ
                    SendInfoLen = msg_mqtt_package(pMsgInfoSend, MAX_QUEUE_BUFF_LEN, SendInfoBuf);

                    //����
                    mqtt_topic_write(SendInfoBuf, SendInfoLen, topic);
                    CCOUP_FMT_DEBUG("Send Up IPC data... SendInfoLen=%d \n", SendInfoLen);
                    CCOUP_BUF_DEBUG(SendInfoBuf, SendInfoLen, "����mqtt����:");
                    memset(SendInfoBuf, 0, sizeof(SendInfoBuf));
                    
                    memset(SendBuf, 0, sizeof(SendBuf));
                    free(pMsgInfoSend);
                    pMsgInfoSend = NULL;
                }

                //ת����̼����ݽ��մ���
                SocketState = IPC_SOCK_RECV;
                            
                break;
            }
                
            default:
            {
                SocketState = IPC_SOCK_INIT;
                break;
            }
           
        }

        //usleep(20000);                         //����CPU��Դ
    }
    pthread_exit(NULL);

}
#endif

/*******************************************************************************
* ��������: UpIpcThread
* ��������: �����߳�(UDP��Ϣ����)
* �������: void *arg            ָ���������ָ��
* �������: ��
* �� �� ֵ: ��
*******************************************************************************/
#if MSG_MQTT == 0
void *UpIpcThread(void *arg)
{
    printx(" void *UpIpcThread(void *arg) \n");
	writelog(ROUTER_CCO_RECD_LOG, " void *UpIpcThread(void *arg)");
	
	int8    SocketState = IPC_SOCK_INIT;        //����socket����״̬
    uint8   RecvBuf[MAX_QUEUE_BUFF_LEN] = {0}; //socket���ջ�����
    uint8   SendBuf[MAX_QUEUE_BUFF_LEN] = {0}; //socekt���ͻ�����
    int32   SendLen = 0;                        //�������ݳ���
    ThreadArg_T *pThreadArg;                    //�̲߳���
    LockQueue_T *pLockRecv;                     //�ز������ջ�����ָ��
    
    int     ret;
	int     result;
	int     createflag = 0;

	PRAR_UDP_T  tUdp = {0};
	MSG_INFO_T *ptMsgInfo = NULL;
	MSG_INFO_T *pMsgInfoSend = NULL;
	HMSG        phMsg = NULL;

	memset(RecvBuf, 0x00, sizeof(RecvBuf));
	memset(SendBuf, 0x00, sizeof(SendBuf));

    pThreadArg = (ThreadArg_T *)arg;            //��ʼ�����ṹָ��
    pLockRecv = pThreadArg->RecvBuf;

	INFO_DATA_T stInfoData;  //��Ϣ
	memset(&stInfoData, 0x00, sizeof(stInfoData));

	//BOOL tempFlag = FALSE;

    while(1)
    {
        gUpIpcThreadFlag = 1;                  //���ڼ���߳������Ƿ�����

		switch (SocketState)
        {
	        case IPC_SOCK_INIT:
	        {
				//printx(" void *UpIpcThread(void *arg)...case IPC_SOCK_INIT: \n");
	            //�򿪽��̼�ͨѶ
				tUdp.appName = gCcoRouterAppName;
			    inet_aton(gCcoRouterAppIP, (struct in_addr*)&tUdp.srcIP);
			    tUdp.srcIP = ntohl(tUdp.srcIP);
			    tUdp.srcPort = gCcoRouterAppPort;
			    tUdp.bBlock = 0;         //������

				phMsg = msg_handle_create(tUdp);
			    if(NULL == phMsg)
			    {		        
	                printx("open UpIpcThread error!\n");
					writelog(ROUTER_CCO_RECD_LOG, "Open UpIpcThread error!");
	                sleep(1);  //��ʧ�ܣ���ʱ1S���³��Դ򿪽��̼�ͨѶ
			    }
				else
	            {
	                SocketState = IPC_SOCK_RECV;    //�򿪳ɹ�ת��socket���մ���
	            }
	            break;
			}			

	        case IPC_SOCK_RECV:
	        {
				//printx(" void *UpIpcThread(void *arg)...case IPC_SOCK_RECV: \n");
	            //���ؽ������ݽ���         
				ret = msg_udp_read(phMsg, &ptMsgInfo);
				
				if(0 == ret && NULL != ptMsgInfo)
				{
					//��ӡ������Ϣ����
					printx("\n UpIpcThread()...IPC_SOCK_RECV...ͨ��UDP�յ�����ϢΪ: \n");
					PrintInfoDetail(ptMsgInfo);
					writelog(ROUTER_CCO_RECD_LOG, "\n UpIpcThread()...IPC_SOCK_RECV...ͨ��UDP�յ�����ϢΪ:");
					PrintLogInfoDetail(ptMsgInfo);
				
					//�����MSG����
					if(ptMsgInfo->MsgLen > MAX_MSG_PAYLOAD_LEN)
					{
						printx(" ����! if(ptMsgInfo->MsgLen > MAX_MSG_PAYLOAD_LEN) \n");
						writelog(ROUTER_CCO_RECD_LOG, " ����! if(ptMsgInfo->MsgLen > MAX_MSG_PAYLOAD_LEN) \n");
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

				/***��ʱ���Ա���...begin***/
				
				/***��ʱ���Ա���...end***/

				//������Ϣ��������
				CreateTaskByInfo();
				            
	            break;
			}			

	        case IPC_SOCK_SEND:
	        {
				//printx(" void *UpIpcThread(void *arg)...case IPC_SOCK_SEND: \n");
				
	            //�鹲��ķ��ͻ�����        
	            SendLen = PlcReadQueue(pLockRecv, SendBuf);
	            if (SendLen > 0)
	            {
	            	printx("\n Send Up IPC data:%d\n", SendLen);
	                PrintBuf(SendBuf, SendLen);
					
					//��ȡMSG
					memset(&stInfoData, 0x00, sizeof(stInfoData));
					memcpy(&stInfoData, SendBuf, SendLen);
					
					//�����ڴ�
					pMsgInfoSend = (MSG_INFO_T *)malloc(SendLen);

					//��ȡHEAD, ���UDPĿ��ip��ַ�Ͷ˿�
					result = GetHeadInfo(stInfoData, pMsgInfoSend);
					result = 1;  //���Խ׶�, ��1
					if(result >= 0)
					{
						//��ӡ������Ϣ����
						printx(" UpIpcThread()...IPC_SOCK_SEND...ͨ��UDP���͵���ϢΪ: \n");
						PrintInfoDetail(pMsgInfoSend);
						writelog(ROUTER_CCO_RECD_LOG, "\n UpIpcThread()...IPC_SOCK_SEND...ͨ��UDP���͵���ϢΪ:");
						PrintLogInfoDetail(pMsgInfoSend);

						//����
		                msg_udp_write(phMsg, pMsgInfoSend);
					}
					else
					{
						printx(" UpIpcThread()...IPC_SOCK_SEND GetHeadInfo()ʧ��...\n");
					}
					
	                memset(SendBuf, 0, sizeof(SendBuf));
					free(pMsgInfoSend);
					pMsgInfoSend = NULL;
	            }

	            //ת����̼����ݽ��մ���
	            SocketState = IPC_SOCK_RECV;
							
	            break;
			}
				
	        case IPC_SOCK_CLOSE:
	        {
				//printx(" void *UpIpcThread(void *arg)...case IPC_SOCK_CLOSE: \n");
	            //�رս��̼�ͨѶ
	            msg_handle_close(phMsg);

	            //���³�ʼ�����̼�ͨѶ
	            SocketState = IPC_SOCK_INIT;
	            break;
			}			

	        default:
	        {
				 SocketState = IPC_SOCK_INIT;
	            break;
			}
           
        }

        usleep(100000);                         //����CPU��Դ
    }
    pthread_exit(NULL);

}
#endif

/*******************************************************************************
* ��������: DownTtyacmThread
* ��������: �����д��ڽ��̽���ͨ�� ����ģ��
* �������: void *arg            ָ���������ָ��
* �������: ��
* �� �� ֵ: ��
*******************************************************************************/
#if DOWN_DEV_TYPE == 1
void *DownTtyacmThread(void *arg)
{

	uint8   nTtlIndex = 0;  //��ͬ�Ķ˿ںŶ�Ӧ��ͬ�������±�
	int     fd_sock = -1;                       //socket���ӱ�ʶ��
    int8   SocketState = IPC_SOCK_INIT;        //�߳�����״̬
    uint8   RecvBuf[MAX_QUEUE_BUFF_LEN];        //���ջ�����
    uint8   SendBuf[MAX_QUEUE_BUFF_LEN];        //���ͻ�����
    int32  RecvLen = 0, SendLen = 0;           //���ա��������ݳ���
    RS232_COMM_PARA CommPara;
    uint16  FrameLen=0;
    int     ret = 0;
    uint8   DevName[30] = {0};
    ThreadArg_T *pThreadArg;
    LockQueue_T *pLockRecv;
    LockQueue_T *pLockSend;
	uint8 *ptr=NULL;									//Ӧ������֡��ʼָ��
	
    pThreadArg = (ThreadArg_T *)arg;
	nTtlIndex = pThreadArg->nArrIndex;
	if((nTtlIndex >= MAX_DOWN_THREAD_NUM)
		|| (nTtlIndex >= MAX_ALLOWED_DOWN_THREAD_NUM))
	{
		printx(" DownTtyacmThread() �߳������±�nTtlIndex=%d�쳣! \n", nTtlIndex);
		return NULL;
	}

	printx(" �˿�[%d] void *DownTtyacmThread(void *arg) \n", nTtlIndex);
	writelog(gRecordPath[nTtlIndex], " �˿�[%d] void *DownTtyacmThread(void *arg)", nTtlIndex);

	//printx("&pThreadArg->RecvBuf.QueueLock:%p �˿�:%d \n", &pThreadArg->RecvBuf->QueueLock, nTtlIndex);
	//printx("&pThreadArg->SendBuf.QueueLock:%p �˿�:%d \n", &pThreadArg->SendBuf->QueueLock, nTtlIndex);
	
    pLockRecv = pThreadArg->RecvBuf;
    pLockSend = pThreadArg->SendBuf;

	//printx("RecvBuf.QueueLock:%p �˿�:%d \n", &pLockRecv->QueueLock, nTtlIndex);
	//printx("SendBuf.QueueLock:%p �˿�:%d \n", &pLockSend->QueueLock, nTtlIndex);

	memset(&gCommJoinMessage[nTtlIndex], 0x00, sizeof(COMM_JOIN_MESSAGE_T));

    while(1)
    {
    	//sleep(1);
		
        gDownTtlThreadFlag[nTtlIndex] = 1;                 //���ڼ���߳������Ƿ�����
        //printx(" gDownTtlThreadFlag[%d] = 1 \n", nTtlIndex);

        switch (SocketState)
        {
        case IPC_SOCK_INIT:                    //��ʼ������
        	//printx(" ����[%d]��ʼ��void *DownTtyacmThread(void *arg)...case IPC_SOCK_INIT: \n", nTtlIndex);
            memcpy(CommPara.sDeviceName, gDingxinDevName[nTtlIndex], 30);
            CommPara.dwBaudRate = 9600;           //������
            CommPara.byByteSize = 8;              //����λ, 7-8
            CommPara.byParity = 'e';          //У�� None,Odd,Even,Space
            CommPara.byStopBits = 1;              //ֹͣλ 1,2

            fd_sock = commdrv_Init_232(&CommPara);   /*�򿪴���*/
            fd_plc[nTtlIndex] = fd_sock;

            if (fd_sock < 0)                    //���򿪲��ɹ�����ʱ1s�����´�
            {               
                printx("\n ����[%d]��ʧ��DownTtyacmThread Error\n\n", nTtlIndex);
				writelog(gRecordPath[nTtlIndex], " ����[%d]��ʧ��DownTtyacmThread Error", nTtlIndex);
                sleep(1);
            }
            else
            {
                SocketState = IPC_SOCK_RECV;
				printx("\n ����[%d]�򿪳ɹ�DownTtyacmThread Success\n\n", nTtlIndex);
				writelog(gRecordPath[nTtlIndex], " ����[%d]�򿪳ɹ�DownTtyacmThread Success", nTtlIndex);
            }
            break;

        case IPC_SOCK_RECV:
			//printx(" ����[%d]����void *DownTtyacmThread(void *arg)...case IPC_SOCK_RECV: \n", nTtlIndex);
            memset(RecvBuf, 0x00, sizeof(RecvBuf));
            RecvLen = commdrv_Recv_232(fd_sock, RecvBuf, sizeof(RecvBuf), 5, 5, 0xFF);

			if (RecvLen > 6)
            {
            	printx("\n ����[%d]���� (��������) RecvLen = PlcRecvTTL();...if(RecvLen > 0) \n", nTtlIndex);
            	printx(" local ttl: recv data len: %d\n", RecvLen);
                PrintBuf(RecvBuf, RecvLen);
				writelog(gRecordPath[nTtlIndex], "\n ����[%d]���� (��������) RecvLen:%d ��������Ϊ:", nTtlIndex, RecvLen);
				PrintLogBuffDetail(RecvBuf, RecvLen, nTtlIndex);

				while ((CheckFrameFormat(RecvLen, RecvBuf, &FrameLen, 2) > 0) 
					|| (CheckFrameFormat(RecvLen, RecvBuf, &FrameLen, 1) > 0))
				{
                	if(InstantReportTaskDeal(RecvBuf, FrameLen, nTtlIndex) < 0) //���ж�Ϊ�ϱ�������д�뻺����
                    {
                        printx("\n ����[%d]���� (��������) if(InstantReportTaskDeal() < 0) ... ����FrameLen:%d ����RecvBuf: \n", nTtlIndex, FrameLen);
			            PrintBuf(RecvBuf, FrameLen);
						printx("\n");
						writelog(gRecordPath[nTtlIndex], "\n ����[%d]���� (��������) if(InstantReportTaskDeal() < 0) ... ����FrameLen:%d ����RecvBuf: \n", nTtlIndex, FrameLen);
						PrintLogBuffDetail(RecvBuf, FrameLen, nTtlIndex);
						
						ret = PlcWriteQueue(pAmrRecvDown[nTtlIndex], RecvBuf, FrameLen);
						//printx("ret = PlcWriteQueue(pLockSend, RecvBuf, FrameLen);...ret:%d\n", ret);
                        if (ret == 0)
                        {
                        	printx(" ret = PlcWriteQueue(pLockSend, RecvBuf, FrameLen);...if (ret == 0)\n");
							writelog(gRecordPath[nTtlIndex], " ret = PlcWriteQueue(pLockSend, RecvBuf, FrameLen);...if (ret == 0)");
							
                            printx(" DownTtyacmThread is full!\n");
							writelog(gRecordPath[nTtlIndex], " DownTtyacmThread is full!");

                            //��ս��ջ�����
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
			//printx(" ����[%d]����void *DownTtyacmThread(void *arg)...case IPC_SOCK_SEND: \n", nTtlIndex);
            SendLen = PlcReadQueue(pAmrSendDown[nTtlIndex], SendBuf);
            if (SendLen > 0)
            {
            	printx("\n ����[%d]����SendLen = PlcReadQueue(pLockRecv, SendBuf);...if (SendLen > 0) \n", nTtlIndex);
                printx(" local ttl: send data len: %d\n", SendLen);
                PrintBuf(SendBuf, SendLen);
				writelog(gRecordPath[nTtlIndex], "\n ����[%d]���ͱ��� SendLen:%d ��������Ϊ:", nTtlIndex, SendLen);
				PrintLogBuffDetail(SendBuf, SendLen, nTtlIndex);
				
                commdrv_Send_232(fd_sock, SendBuf, SendLen);

                //дͨ����־

                memset(SendBuf, 0, sizeof(SendBuf));
            }
            SocketState = IPC_SOCK_RECV;
            break;

        case IPC_SOCK_CLOSE:
			//printx(" ����[%d]�ر�void *DownTtyacmThread(void *arg)...case IPC_SOCK_CLOSE: \n", nTtlIndex);
            PlcCloseTTL(fd_sock);
            SocketState = IPC_SOCK_INIT;
            break;

        default:
            PlcCloseTTL(fd_sock);
            SocketState = IPC_SOCK_INIT;
            break;
        }
       
        usleep(100000);                         //����CPU��Դ

    }

    pthread_exit(NULL);

}
#endif

/*******************************************************************************
* ��������: DownTtlThread
* ��������: �����д��ڽ��̽���ͨ�� ��оģ��
* �������: void *arg            ָ���������ָ��
* �������: ��
* �� �� ֵ: ��
*******************************************************************************/
#if DOWN_DEV_TYPE == 0
void *DownTtlThread(void *arg)
{

	uint8   nTtlIndex = 0;  //��ͬ�Ķ˿ںŶ�Ӧ��ͬ�������±�
	int     fd_sock = -1;                       //socket���ӱ�ʶ��
    int8   SocketState = IPC_SOCK_INIT;        //�߳�����״̬
    uint8   RecvBuf[MAX_QUEUE_BUFF_LEN];        //���ջ�����
    uint8   SendBuf[MAX_QUEUE_BUFF_LEN];        //���ͻ�����
    int32  RecvLen = 0, SendLen = 0;           //���ա��������ݳ���
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
		printx(" DownTtlThread() �߳������±�nTtlIndex=%d�쳣! \n", nTtlIndex);
		return NULL;
	}

	printx(" �˿�[%d] void *DownTtlThread(void *arg) \n", nTtlIndex);
	writelog(gRecordPath[nTtlIndex], " �˿�[%d] void *DownTtlThread(void *arg)", nTtlIndex);

	//printx("&pThreadArg->RecvBuf.QueueLock:%p �˿�:%d \n", &pThreadArg->RecvBuf->QueueLock, nTtlIndex);
	//printx("&pThreadArg->SendBuf.QueueLock:%p �˿�:%d \n", &pThreadArg->SendBuf->QueueLock, nTtlIndex);
	
    pLockRecv = pThreadArg->RecvBuf;
    pLockSend = pThreadArg->SendBuf;

	//printx("RecvBuf.QueueLock:%p �˿�:%d \n", &pLockRecv->QueueLock, nTtlIndex);
	//printx("SendBuf.QueueLock:%p �˿�:%d \n", &pLockSend->QueueLock, nTtlIndex);

	memset(&gCommJoinMessage[nTtlIndex], 0x00, sizeof(COMM_JOIN_MESSAGE_T));

    while(1)
    {
    	//sleep(1);
		
        gDownTtlThreadFlag[nTtlIndex] = 1;                 //���ڼ���߳������Ƿ�����
        //printx(" gDownTtlThreadFlag[%d] = 1 \n", nTtlIndex);

        switch (SocketState)
        {
        case IPC_SOCK_INIT:                    //��ʼ������
        	//printx(" ����[%d]��ʼ��void *DownTtlThread(void *arg)...case IPC_SOCK_INIT: \n", nTtlIndex);
            memcpy(CommPara.DeviceName, gDevName[nTtlIndex], 30);
            CommPara.BaudRate = 9600;           //������
            CommPara.DataBits = 8;              //����λ, 7-8
            CommPara.ParityFlag = 'e';          //У�� None,Odd,Even,Space
            CommPara.StopBits = 1;              //ֹͣλ 1,2

            fd_sock = PlcInitTTL(&CommPara);    //�򿪴���
            fd_plc[nTtlIndex] = fd_sock;

            if (fd_sock < 0)                    //���򿪲��ɹ�����ʱ1s�����´�
            {               
                printx("\n ����[%d]��ʧ��DownTtlThread Error\n\n", nTtlIndex);
				writelog(gRecordPath[nTtlIndex], " ����[%d]��ʧ��DownTtlThread Error", nTtlIndex);
                sleep(1);
            }
            else
            {
                SocketState = IPC_SOCK_RECV;
				printx("\n ����[%d]�򿪳ɹ�DownTtlThread Success\n\n", nTtlIndex);
				writelog(gRecordPath[nTtlIndex], " ����[%d]�򿪳ɹ�DownTtlThread Success", nTtlIndex);
            }
            break;

        case IPC_SOCK_RECV:
			//printx(" ����[%d]����void *DownTtlThread(void *arg)...case IPC_SOCK_RECV: \n", nTtlIndex);
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
            	printx("\n ����[%d]���� (��������) RecvLen = PlcRecvTTL();...if(RecvLen > 0) \n", nTtlIndex);
            	printx(" local ttl: recv data len: %d\n", RecvLen);
                PrintBuf(RecvBuf, RecvLen);
				writelog(gRecordPath[nTtlIndex], "\n ����[%d]���� (��������) RecvLen:%d ��������Ϊ:", nTtlIndex, RecvLen);
				PrintLogBuffDetail(RecvBuf, RecvLen, nTtlIndex);
				
                if(gCommJoinMessage[nTtlIndex].pinjie >= 15)//�ݴ�
                {
                    gCommJoinMessage[nTtlIndex].pinjie = 0;
                    memset(gCommJoinMessage[nTtlIndex].HidRead,0x00,sizeof(gCommJoinMessage[nTtlIndex].HidRead));
                }
                memcpy(&gCommJoinMessage[nTtlIndex].HidRead[gCommJoinMessage[nTtlIndex].pinjie*64],RecvBuf,RecvLen);//ƴ������
                gCommJoinMessage[nTtlIndex].pinjie += 1;
                //commfun_printstring(HidRead,pinjie*64);
				
                while ((CheckFrameFormat(gCommJoinMessage[nTtlIndex].pinjie*64, gCommJoinMessage[nTtlIndex].HidRead, &FrameLen, 2) > 0) 
					|| (CheckFrameFormat(gCommJoinMessage[nTtlIndex].pinjie*64, gCommJoinMessage[nTtlIndex].HidRead, &FrameLen, 1) > 0))
				{
                	//printx("while ((CheckFrameFormat(pinjie*64, HidRead, &FrameLen, 2) > 0) ||...begin FrameLen:%d\n", FrameLen);
                    memcpy(RecvBuf,&gCommJoinMessage[nTtlIndex].HidRead[0],FrameLen);
                    gCommJoinMessage[nTtlIndex].pinjie = 0;
                    memset(gCommJoinMessage[nTtlIndex].HidRead,0x00,sizeof(gCommJoinMessage[nTtlIndex].HidRead));
                    if(InstantReportTaskDeal(RecvBuf, FrameLen, nTtlIndex) < 0) //���ж�Ϊ�ϱ�������д�뻺����
                    {
                        printx("\n ����[%d]���� (��������) if(InstantReportTaskDeal() < 0) ... ����FrameLen:%d ����RecvBuf: \n", nTtlIndex, FrameLen);
			            PrintBuf(RecvBuf, FrameLen);
						printx("\n");
						writelog(gRecordPath[nTtlIndex], "\n ����[%d]���� (��������) if(InstantReportTaskDeal() < 0) ... ����FrameLen:%d ����RecvBuf: \n", nTtlIndex, FrameLen);
						PrintLogBuffDetail(RecvBuf, FrameLen, nTtlIndex);
						
						ret = PlcWriteQueue(pAmrRecvDown[nTtlIndex], RecvBuf, FrameLen);
						//printx("ret = PlcWriteQueue(pLockSend, RecvBuf, FrameLen);...ret:%d\n", ret);
                        if (ret == 0)
                        {
                        	printx(" ret = PlcWriteQueue(pLockSend, RecvBuf, FrameLen);...if (ret == 0)\n");
							writelog(gRecordPath[nTtlIndex], " ret = PlcWriteQueue(pLockSend, RecvBuf, FrameLen);...if (ret == 0)");
							
                            printx(" DownTtlThread is full!\n");
							writelog(gRecordPath[nTtlIndex], " DownTtlThread is full!");

                            //��ս��ջ�����
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
			//printx(" ����[%d]����void *DownTtlThread(void *arg)...case IPC_SOCK_SEND: \n", nTtlIndex);
            SendLen = PlcReadQueue(pAmrSendDown[nTtlIndex], SendBuf);
            if (SendLen > 0)
            {
            	printx("\n ����[%d]����SendLen = PlcReadQueue(pLockRecv, SendBuf);...if (SendLen > 0) \n", nTtlIndex);
                printx(" local ttl: send data len: %d\n", SendLen);
                PrintBuf(SendBuf, SendLen);
				writelog(gRecordPath[nTtlIndex], "\n ����[%d]���ͱ��� SendLen:%d ��������Ϊ:", nTtlIndex, SendLen);
				PrintLogBuffDetail(SendBuf, SendLen, nTtlIndex);
				
                PlcSendTTL(fd_sock, SendBuf, SendLen);

                //дͨ����־

                memset(SendBuf, 0, sizeof(SendBuf));
            }
            SocketState = IPC_SOCK_RECV;
            break;

        case IPC_SOCK_CLOSE:
			//printx(" ����[%d]�ر�void *DownTtlThread(void *arg)...case IPC_SOCK_CLOSE: \n", nTtlIndex);
            PlcCloseTTL(fd_sock);
            SocketState = IPC_SOCK_INIT;
            break;

        default:
            PlcCloseTTL(fd_sock);
            SocketState = IPC_SOCK_INIT;
            break;
        }
       
        usleep(100000);                         //����CPU��Դ

    }

    pthread_exit(NULL);

}
#endif

/*******************************************************************************
* ��������: DownTtlHalThread
* ��������: �����д��ڽ��̽���ͨ�� HAL�ӿ�
* �������: void *arg            ָ���������ָ��
* �������: ��
* �� �� ֵ: ��
*******************************************************************************/
#if DOWN_DEV_TYPE == 2
void *DownTtlHalThread(void *arg)
{
    uint8   nTtlIndex = 0;  //��ͬ�Ķ˿ںŶ�Ӧ��ͬ�������±�
    int8   SocketState = IPC_SOCK_INIT;        //�߳�����״̬
    uint8   RecvBuf[MAX_QUEUE_BUFF_LEN] = {0};        //���ջ�����(��������)
    uint8   SendBuf[MAX_QUEUE_BUFF_LEN] = {0};        //���ͻ�����
    int32  RecvLen = 0;           //�������ݳ���(��������)
    int32  SendLen = 0;           //�������ݳ���
    uint16  FrameLen=0;           //�Ϸ���һ֡3762����
    int     ret = 0;
    ThreadArg_T *pThreadArg;
    int checkRet1 = 0;
    int checkRet2 = 0;
    uint8   SingleRecvBuf[MAX_QUEUE_BUFF_LEN] = {0};        //���ջ�����(���α���)
    int32  SingleRecvLen = 0;           //�������ݳ���(���α���)

    //����ʹ��ccoģ����Ϣbegin
    /*
    uint8 aTestModuleBuf[] = {0x68, 0x38, 0x00, 0xC3, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x03, 0x02, 0x01, 0xF2, 0x37, 0x01, 0x01, 0x00, 0x00, 0x3C, 0x0A, 0x00, 0x00, 0x04, 0x00, 0x04, 0x3C, 0x00, 0x00, 0x00, 0x00, 0x01, 0x23, 0xF8, 0x07, 0x64, 0x00, 0x18, 0x09, 0x13, 0x18, 0x09, 0x13, 0x58, 0x54, 0x33, 0x30, 0x24, 0x09, 0x19, 0x05, 0x00, 0x00, 0x80, 0x48, 0x16 };
    uint16 nTestModuleBufLen = sizeof(aTestModuleBuf);
    int count = 0;
    int nCopyIndex = 0;
    int nCopyLen = 0;
    */
    //����ʹ��ccoģ����Ϣend
    
    pThreadArg = (ThreadArg_T *)arg;
    nTtlIndex = pThreadArg->nArrIndex;
    if((nTtlIndex >= MAX_DOWN_THREAD_NUM)
        || (nTtlIndex >= MAX_ALLOWED_DOWN_THREAD_NUM))
    {
        CCODOWN_FMT_DEBUG(0, "DownTtlThread() �߳������±�nTtlIndex=%d�쳣! \n", nTtlIndex);
        return NULL;
    }

    CCODOWN_FMT_DEBUG(nTtlIndex, "�˿�[%d] void *DownTtlThread(void *arg) \n", nTtlIndex);

    //�߳���
    char ThreadName[30] = {0};
    sprintf(ThreadName, "CcoDownHal-%d", nTtlIndex);
    prctl(PR_SET_NAME,ThreadName);

    while(1)
    {
        gDownTtlThreadFlag[nTtlIndex] = 1;                 //���ڼ���߳������Ƿ�����
        GetCurBcdTime(&gCurBcdTimeDown[nTtlIndex]);         //ˢ�µ�ǰBCD��ʱ��

        switch (SocketState)
        {
        case IPC_SOCK_INIT:                    //��ʼ������
            if(gDevHalInfo[nTtlIndex].isvalid == 0)
            {
                SocketState = IPC_SOCK_INIT;
                CCODOWN_FMT_DEBUG(nTtlIndex, "DownTtlHalThread() ����[%d] if(gDevHalInfo[nTtlIndex].isvalid == 0) \n", nTtlIndex);
                sleep(5);  //ģ����Ϣ��Ч, �߳̿�ת
                break;
            }

            if((NULL == gDevHalInfo[nTtlIndex].dev) || (1 == gReloadDevFlag[nTtlIndex]))
            {
                //��ȡ�豸
                gDevHalInfo[nTtlIndex].dev = (UART_DEVICE_T*)hal_device_get(gDevHalInfo[nTtlIndex].deviceName);
                CCODOWN_FMT_DEBUG(nTtlIndex, "����[%d] deviceName:%s, dev:%p, gReloadDevFlag=%d \n", 
                    nTtlIndex, gDevHalInfo[nTtlIndex].deviceName, gDevHalInfo[nTtlIndex].dev, gReloadDevFlag[nTtlIndex]);

                // ���豸
                ret = gDevHalInfo[nTtlIndex].dev->uart_param_set(gDevHalInfo[nTtlIndex].dev, 9600, 8, 1, PARITY_EVEN);  //TRUE:1, FALSE:0
                CCODOWN_FMT_DEBUG(nTtlIndex, "����[%d] dev->uart_param_set() ret=%d \n", nTtlIndex, ret);
                if(ret == 0)
                {
                    SocketState = IPC_SOCK_RECV;
                    CCODOWN_FMT_DEBUG(nTtlIndex, "����[%d]�򿪳ɹ�DownTtlThread Success \n", nTtlIndex);
                    gReloadDevFlag[nTtlIndex] = 0;
                }
                else
                {
                    CCODOWN_FMT_DEBUG(nTtlIndex, "����[%d]��ʧ��DownTtlThread Error \n", nTtlIndex);
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

            //��ʼ��
            SingleRecvLen = 0;
            memset(SingleRecvBuf, 0x00, sizeof(SingleRecvBuf));

            //��������
            if(gDevHalInfo[nTtlIndex].dev == NULL)
            {
                SocketState = IPC_SOCK_INIT;
                break;
            }
            SingleRecvLen = gDevHalInfo[nTtlIndex].dev->uart_data_recv(gDevHalInfo[nTtlIndex].dev, SingleRecvBuf, sizeof(SingleRecvBuf));

            //��ʱ���Ա���ƴ֡begin
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
            //��ʱ���Ա���ƴ֡end

            //���û���յ�����, �򽫻�������������
            if(SingleRecvLen <= 0)
            {
                RecvLen = 0;
                memset(RecvBuf, 0x00, sizeof(RecvBuf));
            }
            else
            {
                CCODOWN_FMT_DEBUG(nTtlIndex, "����[%d]���� (��������) dev->uart_data_recv()...SingleRecvLen=%d \n", nTtlIndex, SingleRecvLen);
                CCODOWN_BUF_DEBUG(nTtlIndex, SingleRecvBuf, SingleRecvLen, "#���� (��������)#: ");

                //��������������, �޷�����ƴ��
                if(RecvLen >= sizeof(RecvBuf))
                {
                    CCODOWN_FMT_DEBUG(nTtlIndex, "����������! sizeof(RecvBuf)=%d, RecvLen=%d \n", sizeof(RecvBuf), RecvLen);
                    SocketState = IPC_SOCK_SEND;
                    break;
                }

                //�Ե������Ľ���ƴ�Ӵ���
                if(SingleRecvLen > (sizeof(RecvBuf) - RecvLen))
                {
                    SingleRecvLen = sizeof(RecvBuf) - RecvLen;
                }
                memcpy(&RecvBuf[RecvLen], SingleRecvBuf, SingleRecvLen);
                RecvLen += SingleRecvLen;
                //CCODOWN_BUF_DEBUG(nTtlIndex, RecvBuf, RecvLen, "ƴ��֮��(RecvLen=%d)���汨��: ", RecvLen);

                //��ƴ��֮����ܱ��Ľ��н���
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
                        if(ret < 0) //���ж�Ϊ�ϱ�������д�뻺����
                        {
                            CCODOWN_FMT_DEBUG(nTtlIndex, "����[%d]���� (��������) if(InstantReportTaskDeal() < 0) ... ����FrameLen:%d, ����RecvBuf: \n", nTtlIndex, FrameLen);
                            CCODOWN_BUF_DEBUG(nTtlIndex, RecvBuf, FrameLen, "##���� (��������)##: ");

                            ret = PlcWriteQueue(pAmrRecvDown[nTtlIndex], RecvBuf, FrameLen);
                            if (ret == 0)
                            {
                                CCODOWN_FMT_DEBUG(nTtlIndex, "ret = PlcWriteQueue(pLockSend, RecvBuf, FrameLen);...if (ret == 0) \n");
                                CCODOWN_FMT_DEBUG(nTtlIndex, "DownTtlThread is full! \n");

                                //��ս��ջ�����
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
                CCODOWN_FMT_DEBUG(nTtlIndex, "����[%d]����SendLen = PlcReadQueue(pLockRecv, SendBuf);...SendLen=%d \n", nTtlIndex, SendLen);
                CCODOWN_BUF_DEBUG(nTtlIndex, SendBuf, SendLen, "@@���ͱ���@@:");

                if(gDevHalInfo[nTtlIndex].dev == NULL)
                {
                    SocketState = IPC_SOCK_INIT;
                    break;
                }
                ret = gDevHalInfo[nTtlIndex].dev->uart_data_send(gDevHalInfo[nTtlIndex].dev, SendBuf, SendLen);
                CCODOWN_FMT_DEBUG(nTtlIndex, "���ͽ��: nTtlIndex=%d, deviceName=%s, ret=%d \n",nTtlIndex, gDevHalInfo[nTtlIndex].deviceName, ret);

                //дͨ����־
                memset(SendBuf, 0, sizeof(SendBuf));
            }
            SocketState = IPC_SOCK_INIT;
            break;

        case IPC_SOCK_CLOSE:
            //printx(" ����[%d]�ر�void *DownTtlThread(void *arg)...case IPC_SOCK_CLOSE: \n", nTtlIndex);
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
       
        usleep(100000);                         //����CPU��Դ

    }

    pthread_exit(NULL);

}
#endif

/*******************************************************************************
* ��������: DownCcuThread
* ��������: �����д��ڽ��̽���ͨ��
* �������: void *arg            ָ���������ָ��
* �������: ��
* �� �� ֵ: ��
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
    uint8   nTtlIndex = 0;  //��ͬ�Ķ˿ںŶ�Ӧ��ͬ�������±�
    int8   SocketState = IPC_SOCK_INIT;        //�߳�����״̬
    uint8   RecvBuf[MAX_QUEUE_BUFF_LEN] = {0};        //���ջ�����(��������)
    uint8   SendBuf[MAX_QUEUE_BUFF_LEN] = {0};        //���ͻ�����
    int32  RecvLen = 0;           //�������ݳ���(��������)
    int32  SendLen = 0;           //�������ݳ���
    uint16  FrameLen=0;           //�Ϸ���һ֡3762����
    int     ret = 0;
    ThreadArg_T *pThreadArg;
    int checkRet1 = 0;
    int checkRet2 = 0;
    uint8   SingleRecvBuf[MAX_QUEUE_BUFF_LEN] = {0};        //���ջ�����(���α���)
    int32  SingleRecvLen = 0;           //�������ݳ���(���α���)
    uint32  flag = 1;   /* ���ڼ��ģ��β嶯����1Ϊ�γ�״̬ */
    //int     fd_sock = -1;                       //socket���ӱ�ʶ��
    //COMM_PARA_T CommPara;
    //uint8   DevName[30] = {0};
    uint32  cnt = 0;
#ifdef AREA_FUJIAN
    CCO_AMR_T  *amr = &g_CcoAmr;
    amr->flag.initFlag = UINT_MAX;
#endif
    //����ʹ��ccoģ����Ϣbegin
    /*
    uint8 aTestModuleBuf[] = {0x68, 0x38, 0x00, 0xC3, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x03, 0x02, 0x01, 0xF2, 0x37, 0x01, 0x01, 0x00, 0x00, 0x3C, 0x0A, 0x00, 0x00, 0x04, 0x00, 0x04, 0x3C, 0x00, 0x00, 0x00, 0x00, 0x01, 0x23, 0xF8, 0x07, 0x64, 0x00, 0x18, 0x09, 0x13, 0x18, 0x09, 0x13, 0x58, 0x54, 0x33, 0x30, 0x24, 0x09, 0x19, 0x05, 0x00, 0x00, 0x80, 0x48, 0x16 };
    uint16 nTestModuleBufLen = sizeof(aTestModuleBuf);
    int count = 0;
    int nCopyIndex = 0;
    int nCopyLen = 0;
    */
    //����ʹ��ccoģ����Ϣend
    
    pThreadArg = (ThreadArg_T *)arg;
    nTtlIndex = pThreadArg->nArrIndex;
    if((nTtlIndex >= MAX_DOWN_THREAD_NUM)
        || (nTtlIndex >= MAX_ALLOWED_DOWN_THREAD_NUM))
    {
        CCODOWN_FMT_DEBUG(0, "DownCcuThread() �߳������±�nTtlIndex=%d�쳣! \n", nTtlIndex);
        return NULL;
    }

    CCODOWN_FMT_DEBUG(nTtlIndex, "�˿�[%d] void *DownCcuThread(void *arg) \n", nTtlIndex);

    //�߳���
    char ThreadName[30] = {0};
    sprintf(ThreadName, "CcoDownCcu-%d", nTtlIndex);
    prctl(PR_SET_NAME,ThreadName);
    sprintf(gDevHalInfo[nTtlIndex].deviceName, "%s", UART_LOCAL_CU_DEVICE_ID_STR);
    sprintf(gDevHalInfo[nTtlIndex].ctrlName, "%s", HW_DEVICE_ID_LOCAL_CU);
    gDevHalInfo[nTtlIndex].ctrlDev = NULL;

    while(1)
    {
        gDownTtlThreadFlag[nTtlIndex] = 1;                 //���ڼ���߳������Ƿ�����
        GetCurBcdTime(&gCurBcdTimeDown[nTtlIndex]);         //ˢ�µ�ǰBCD��ʱ��

        if (gDevHalInfo[nTtlIndex].ctrlDev == NULL)
        {
            gDevHalInfo[nTtlIndex].ctrlDev = (LOCAL_CU_DEVICE_T *)hal_device_get(gDevHalInfo[nTtlIndex].ctrlName);

            if (gDevHalInfo[nTtlIndex].ctrlDev == NULL)
            {
                CCODOWN_FMT_DEBUG(nTtlIndex, "����[%d] hal_device_get(%s) fail!\n", nTtlIndex, gDevHalInfo[nTtlIndex].ctrlName);
                sleep(2);
                continue;
            }
        }

        if (cnt % 500 == 0)
        {
            if (ModuleCheck(nTtlIndex) == 1)
            {
                gDevHalInfo[nTtlIndex].isvalid = 1;
                //CCODOWN_FMT_DEBUG(nTtlIndex, "DownCcuThread ���ز�ģ��\n");

                if (flag == 1)
                {
                    /* ģ����� */
                    /*while (1)
                    {
                        if (PlcResetRouter(nTtlIndex) > 0)
                        {
                            CCODOWN_FMT_DEBUG(nTtlIndex, "DownCcuThread ��λ�ز�ģ��ɹ�\n");
                            break;
                        }
                        else
                        {
                            CCODOWN_FMT_DEBUG(nTtlIndex, "DownCcuThread ��λ�ز�ģ��ʧ��\n");
                        }
                    }*/

                    gMeterFileCompareFlag[nTtlIndex] = 1;//����ccoģ�鵵��
                    gReloadDevFlag[nTtlIndex] = 1;//���´��ڲ������
                    gModuleManuID[nTtlIndex] = MMID_NULL;//����ģ����Ϣ
                    CheckModuleSoftReset(1, nTtlIndex);//������ѯģ����Ϣ��־��1
                    ClearCommQueueBuff(&gAmrRecvDownReportModule[nTtlIndex]);//��������ϱ�ģ����Ϣ
                    flag = 0;
                }
            }
            else
            {
                gDevHalInfo[nTtlIndex].isvalid = 0;
                flag = 1;
                CCODOWN_FMT_DEBUG(nTtlIndex, "DownCcuThread û���ز�ģ��\n");
                sleep(5);  //ģ����Ϣ��Ч, �߳̿�ת
                cnt = 0;
                continue;
            }
        }

        switch (SocketState)
        {
            case IPC_SOCK_INIT:                    //��ʼ������
                if(gDevHalInfo[nTtlIndex].isvalid == 0)
                {
                    SocketState = IPC_SOCK_INIT;
                    CCODOWN_FMT_DEBUG(nTtlIndex, "DownCcuThread() ����[%d] if(gDevHalInfo[nTtlIndex].isvalid == 0) \n", nTtlIndex);
                    
                    break;
                }

                if ((NULL == gDevHalInfo[nTtlIndex].dev) || (1 == gReloadDevFlag[nTtlIndex]))
                {

                    //��ȡ�豸
                    gDevHalInfo[nTtlIndex].dev = (UART_DEVICE_T*)hal_device_get(gDevHalInfo[nTtlIndex].deviceName);
                    CCODOWN_FMT_DEBUG(nTtlIndex, "����[%d] deviceName:%s, dev:%p, gReloadDevFlag=%d \n", 
                        nTtlIndex, gDevHalInfo[nTtlIndex].deviceName, gDevHalInfo[nTtlIndex].dev, gReloadDevFlag[nTtlIndex]);

                    if (gDevHalInfo[nTtlIndex].dev == NULL)
                    {
                        CCODOWN_FMT_DEBUG(nTtlIndex, "����[%d] hal_device_get(%s) fail!\n", nTtlIndex, gDevHalInfo[nTtlIndex].deviceName);
                        sleep(2);
                        break;
                    }

                    // ���豸
                    ret = gDevHalInfo[nTtlIndex].dev->uart_param_set(gDevHalInfo[nTtlIndex].dev, 9600, 8, 1, PARITY_EVEN);  //TRUE:1, FALSE:0
                    CCODOWN_FMT_DEBUG(nTtlIndex, "����[%d] dev->uart_param_set() ret=%d \n", nTtlIndex, ret);
                    if(ret == 0)
                    {
                        SocketState = IPC_SOCK_RECV;
                        CCODOWN_FMT_DEBUG(nTtlIndex, "����[%d]�򿪳ɹ�%u bps DownCcuThread Success \n", nTtlIndex, 9600);
                        gReloadDevFlag[nTtlIndex] = 0;
#ifdef AREA_FUJIAN
                        amr->flag.initFlag = UINT_MAX;
#endif
                    }
                    else
                    {
                        CCODOWN_FMT_DEBUG(nTtlIndex, "����[%d]��ʧ��DownCcuThread Error \n", nTtlIndex);
                        sleep(2);
                        break;
                    }
                    sleep(1);

#if 0
                    //��ʼ������
                    //sprintf((char *)DevName, "%s", PLC_CHANNEL_TTYS);
                    //memcpy(CommPara.DeviceName, DevName, 30);
                    CommPara.BaudRate = 9600;           //������
                    //CommPara.DataBits = 8;              //����λ, 7-8
                    //CommPara.ParityFlag = 'e';          //У�� None,Odd,Even,Space
                    //CommPara.StopBits = 1;              //ֹͣλ 1,2

                    fd_sock = open(PLC_CHANNEL_TTYS, O_RDONLY);    //��io��
                    fd_plc[nTtlIndex] = fd_sock;

                    if (fd_sock <= 0)                    //���򿪲��ɹ�����ʱ1s�����´�
                    {
                        CCODOWN_FMT_DEBUG(nTtlIndex, "����[%d]��ʧ��fd_sock=%d DownCcuThread Error \n", nTtlIndex, fd_sock);
                        sleep(1);
                    }
                    else
                    {
                        SocketState = IPC_SOCK_RECV;
                        CCODOWN_FMT_DEBUG(nTtlIndex, "����[%d]�򿪳ɹ�fd_sock=%d DownCcuThread Success \n", nTtlIndex, fd_sock);
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

                //��ʼ��
                SingleRecvLen = 0;
                memset(SingleRecvBuf, 0x00, sizeof(SingleRecvBuf));

                //��������
                if(gDevHalInfo[nTtlIndex].dev == NULL)
                {
                    SocketState = IPC_SOCK_INIT;
                    break;
                }
                SingleRecvLen = gDevHalInfo[nTtlIndex].dev->uart_data_recv(gDevHalInfo[nTtlIndex].dev, SingleRecvBuf, sizeof(SingleRecvBuf));
                //SingleRecvLen = PlcRecvTTL(fd_sock, CommPara.BaudRate, SingleRecvBuf, sizeof(SingleRecvBuf));
                //��ʱ���Ա���ƴ֡begin
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
                //��ʱ���Ա���ƴ֡end

                //���û���յ�����, �򽫻�������������
                if(SingleRecvLen <= 0)
                {
                    RecvLen = 0;
                    memset(RecvBuf, 0x00, sizeof(RecvBuf));
                }
                else
                {
                    CCODOWN_FMT_DEBUG(nTtlIndex, "����[%d]���� (��������) dev->uart_data_recv()...SingleRecvLen=%d \n", nTtlIndex, SingleRecvLen);
                    CCODOWN_BUF_DEBUG(nTtlIndex, SingleRecvBuf, SingleRecvLen, "#���� (��������)#: ");

                    //��������������, �޷�����ƴ��
                    if(RecvLen >= sizeof(RecvBuf))
                    {
                        CCODOWN_FMT_DEBUG(nTtlIndex, "����������! sizeof(RecvBuf)=%d, RecvLen=%d \n", sizeof(RecvBuf), RecvLen);
                        SocketState = IPC_SOCK_SEND;
                        break;
                    }

                    //�Ե������Ľ���ƴ�Ӵ���
                    if(SingleRecvLen > (sizeof(RecvBuf) - RecvLen))
                    {
                        SingleRecvLen = sizeof(RecvBuf) - RecvLen;
                    }
                    memcpy(&RecvBuf[RecvLen], SingleRecvBuf, SingleRecvLen);
                    RecvLen += SingleRecvLen;
                    //CCODOWN_BUF_DEBUG(nTtlIndex, RecvBuf, RecvLen, "ƴ��֮��(RecvLen=%d)���汨��: ", RecvLen);

                    //��ƴ��֮����ܱ��Ľ��н���
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
                            if(ret < 0) //���ж�Ϊ�ϱ�������д�뻺����
                            {
                                CCODOWN_FMT_DEBUG(nTtlIndex, "����[%d]���� (��������) if(InstantReportTaskDeal() < 0) ... ����FrameLen:%d, ����RecvBuf: \n", nTtlIndex, FrameLen);
                                CCODOWN_BUF_DEBUG(nTtlIndex, RecvBuf, FrameLen, "##���� (��������)##: ");

                                ret = PlcWriteQueue(pAmrRecvDown[nTtlIndex], RecvBuf, FrameLen);
                                if (ret == 0)
                                {
                                    CCODOWN_FMT_DEBUG(nTtlIndex, "ret = PlcWriteQueue(pLockSend, RecvBuf, FrameLen);...if (ret == 0) \n");
                                    CCODOWN_FMT_DEBUG(nTtlIndex, "DownCcuThread is full! \n");

                                    //��ս��ջ�����
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
                    CCODOWN_FMT_DEBUG(nTtlIndex, "����[%d]����SendLen = PlcReadQueue(pLockRecv, SendBuf);...SendLen=%d \n", nTtlIndex, SendLen);
                    CCODOWN_BUF_DEBUG(nTtlIndex, SendBuf, SendLen, "@@���ͱ���@@:");

                    if(gDevHalInfo[nTtlIndex].dev == NULL)
                    {
                        SocketState = IPC_SOCK_INIT;
                        break;
                    }
                    ret = gDevHalInfo[nTtlIndex].dev->uart_data_send(gDevHalInfo[nTtlIndex].dev, SendBuf, SendLen);
                    g_CcoStats.sendNum++;
                    CCODOWN_FMT_DEBUG(nTtlIndex, "���ͽ��: nTtlIndex=%d,deviceName=%s,ret=%d,sendNum=%llu,recvNum=%llu\n",
                                      nTtlIndex, gDevHalInfo[nTtlIndex].deviceName, ret, g_CcoStats.sendNum, g_CcoStats.recvNum);
                    //ret = PlcSendTTL(fd_sock, SendBuf, SendLen);
                    //CCODOWN_FMT_DEBUG(nTtlIndex, "���ͽ��: nTtlIndex=%d, ret=%d \n", nTtlIndex, ret);

                    //дͨ����־
                    memset(SendBuf, 0, sizeof(SendBuf));
                }
                SocketState = IPC_SOCK_RECV;
                break;

            case IPC_SOCK_CLOSE:
                //printx(" ����[%d]�ر�void *DownTtlThread(void *arg)...case IPC_SOCK_CLOSE: \n", nTtlIndex);
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
       
        usleep(20000);                         //����CPU��Դ
        cnt++;
    }

    pthread_exit(NULL);

}

#endif

/*******************************************************************************
* ��������: AmrThreadQueueInit
* ��������: ������г�ʼ��
* �������: void *arg            ָ���������ָ��
* �������: ��
* �� �� ֵ: ��
*******************************************************************************/
void AmrThreadQueueInit(void *arg)
{
    AmrThreadArg_T  *AmrThreadArg;              //�����̲߳���
    AmrThreadArg = (AmrThreadArg_T *)arg;

    uint8           nTtlIndex = 0;  //��ͬ�Ķ˿ںŶ�Ӧ��ͬ�������±�
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
* ��������: AmrThread
* ��������: �����߼������߳�
* �������: void *arg            ָ���������ָ��
* �������: ��
* �� �� ֵ: ��
*******************************************************************************/
void *AmrThread(void *arg)
{
    AmrThreadArg_T  *AmrThreadArg;              //�����̲߳���
    AmrThreadArg = (AmrThreadArg_T *)arg;
#ifdef AREA_FUJIAN
    CCO_AMR_T  *amr = &g_CcoAmr;
#endif
    uint8           nTtlIndex = 0;  //��ͬ�Ķ˿ںŶ�Ӧ��ͬ�������±�
    nTtlIndex = AmrThreadArg->nArrIndex;
    if((nTtlIndex >= MAX_DOWN_THREAD_NUM)
        || (nTtlIndex >= MAX_ALLOWED_DOWN_THREAD_NUM))
    {
        CCODOWN_FMT_DEBUG(0, "AmrThread() �߳������±�nTtlIndex=%d�쳣! \n", nTtlIndex);
        return NULL;
    }

    CCODOWN_FMT_DEBUG(nTtlIndex, "void *AmrThread(void *arg) \n");

    //�߳���
    char ThreadName[30] = {0};
    sprintf(ThreadName, "CcoAmr-%d", nTtlIndex);
    prctl(PR_SET_NAME,ThreadName);

    AmrThreadQueueInit(arg);                    //�̶߳��г�ʼ��
    PastTimeDealInit(nTtlIndex);                        //ʱ��������ʼ��
    AmrInfoInit(nTtlIndex);                             //������Ϣ��ʼ��
    AllStateInit(nTtlIndex);                            //״̬�ֳ�ʼ��
    AllFlagInit(nTtlIndex);                             //��־��ʼ��

    CCODOWN_FMT_DEBUG(nTtlIndex, "AmrThread() ��ʼ��ȫ������! \n");
    
    while(1)
    {
        gAmrThreadFlag[nTtlIndex] = 1;  //���ڼ���߳������Ƿ�����

        if((gDevHalInfo[nTtlIndex].isvalid == 0) || (1 == gReloadDevFlag[nTtlIndex]))
        {
            CCODOWN_FMT_DEBUG(nTtlIndex, "AmrThread() ����[%d] isvalid=%d, gReloadDevFlag=%d \n", nTtlIndex, gDevHalInfo[nTtlIndex].isvalid, gReloadDevFlag[nTtlIndex]);
            sleep(5);  //ģ����Ϣ��Ч, �߳̿�ת
            continue;
        }

        //�����ǰû������ִ�е���ͬ����������Ϣ�Ǳ��Ѿ��յ������������Ҫ����Ϣ�Ǳߵ���ʱ�����ȿ�����ʵ��ʹ�õ�ȫ�ֱ���
        if(0 == gMeterFileModifyState[nTtlIndex])
        {
            if((1==gMeterFileAddDelFlagTemp[nTtlIndex]) || (1==gMeterFileClearFlagTemp[nTtlIndex]))
            {
                CCODOWN_FMT_DEBUG(nTtlIndex, "��Ϣ���µĵ�����־״̬ gMeterFileAddDelFlagTemp=%d, gMeterFileClearFlagTemp=%d \n", gMeterFileAddDelFlagTemp[nTtlIndex], gMeterFileClearFlagTemp[nTtlIndex]);
                gMeterFileAddDelFlag[nTtlIndex] |= gMeterFileAddDelFlagTemp[nTtlIndex];
                gMeterFileClearFlag[nTtlIndex] |= gMeterFileClearFlagTemp[nTtlIndex];
                gMeterFileAddDelFlagTemp[nTtlIndex] = 0;
                gMeterFileClearFlagTemp[nTtlIndex] = 0;
                memcpy(&gAmrMeterFilesList[nTtlIndex], &gAmrMeterFilesListTemp[nTtlIndex], sizeof(gAmrMeterFilesList[nTtlIndex]));
                gMeterFileCompareFlag[nTtlIndex] = 1;
                CCODOWN_FMT_DEBUG(nTtlIndex, "����ͬ���ο���״̬ gMeterFileAddDelFlag=%d, gMeterFileClearFlag=%d, gMeterFileCompareFlag=%d \n", 
                    gMeterFileAddDelFlagTemp[nTtlIndex], gMeterFileClearFlagTemp[nTtlIndex], gMeterFileCompareFlag[nTtlIndex]);
                
            }
        }

        TimeManage(nTtlIndex);                   //ʱ�������
#ifdef AREA_FUJIAN
        /* ������Լ������ */
        CcoFuJianAmr(amr, nTtlIndex);
#endif
        PlcTaskCreat(nTtlIndex);                 //����Ĳ����ж�
        PlcTaskRunRouterDeal(nTtlIndex);         //�����ִ�д���(·�ɲ���)
        PlcTaskRun(nTtlIndex);                   //�����ִ�д���
        PlcTaskRunSearchMeter(nTtlIndex);        //�����ִ�д���(�ѱ�)

        usleep(50000);
    }

    pthread_exit(NULL);

}

/*******************************************************************************
* ��������: init_config
* ��������: �����ļ���ʼ��
* �������: ��
* �������: ��
* �� �� ֵ: 0 �ɹ�; ��0������
*******************************************************************************/
int init_config()
{
    dictionary *ini = NULL;
	char* tmp;

    char   curpath[256] = {0};
    char   filename[256] = {0};
    int i;

    //��ȡ��ǰĿ¼����·������ȥ��������
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
    printf("�����ļ�filename: %s!\n", filename);
    ini = iniparser_load(filename);
	
    if(NULL != ini)
    {
        /* �����ļ����ݻ�ȡ */
        tmp = iniparser_getstring(ini, "mqtt:MQTTIP", "172.17.0.1");
        printf("��ȡmqttip = %s \n", tmp);
        strcpy(mqttIP, tmp);
        mqttport = iniparser_getint(ini, "mqtt:MQTTPORT", 1883);
        printf("��ȡmqttport = %d \n", mqttport);

        iniparser_freedict(ini);
    }
    else
    {
        printf("Get no ini file: %s!\n", CCO_ROUTER_INI_FILE);
    }

    /* ��־ģ���ʼ�� */
    diag_init(filename, TRUE);

    return 0;
}

/*******************************************************************************
* ��������: main
* ��������: ������ ������ά���߳�
* �������: ��
* �������: ��
* �� �� ֵ: >=0:��ȷ�� -1:����
*******************************************************************************/
int main(int argc, char *argv[])
{
    CCOUP_FMT_DEBUG("ccoRouter main() start ... \n");
    BOOL bOpenGui = 0;  //�Ƿ����ʾGUI(�������֮���ٴ�)

    /* ������ֻ������һ������ */
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

    //�߳���
    /*
    char ThreadName[30] = {0};
    sprintf(ThreadName, "CcoMain-%d", 0);
    prctl(PR_SET_NAME,ThreadName);
    */
    /* �źŴ��� */
    init_sig_action("/data/app/ccoRouter/log/Exception.log");

    /* ��ȡ�����ļ� */
    init_config();

    /* ȫ�ֱ�����ʼ�� */
    GlobalInit();
    
    uint8       i = 0;
    
    pthread_t   UpIpcThreadID;                  //����IPCͨ���̱߳�ʶ
    pthread_t   DownTtlThreadID[MAX_DOWN_THREAD_NUM];                //���д���ͨ���̱߳�ʶ
    pthread_t   AmrThreadID[MAX_DOWN_THREAD_NUM];                    //�Զ������̱߳�ʶ

    LockQueue_T LockRecvUp;                     //���н��ջ������ṹ
    LockQueue_T LockSendUp;                     //���з��ͻ������ṹ

    LockQueue_T LockRecvDown[MAX_DOWN_THREAD_NUM];                   //���н��ջ������ṹ
    LockQueue_T LockSendDown[MAX_DOWN_THREAD_NUM];                   //���з��ͻ������ṹ

    ThreadArg_T UpIpcThreadArg;                 //����IPC�̲߳���
    ThreadArg_T DownTtlThreadArg[MAX_DOWN_THREAD_NUM];              //���д���ͨ���̲߳���
    AmrThreadArg_T AmrThreadArg[MAX_DOWN_THREAD_NUM];               //�����̲߳���

    uint8 UpIpcThreadCnt = 0;
    uint8 DownTtlThreadCnt[MAX_DOWN_THREAD_NUM] = {0};
    uint8 AmrThreadCnt[MAX_DOWN_THREAD_NUM] = {0};

    int32 err;
    //int ret = 0;

    CCOUP_FMT_DEBUG("ccoRouter main()  \n");
    CCOUP_FMT_DEBUG("ccoRouter main()  -COMPILE TIME : %s %s\n", __TIME__, __DATE__);
    CCOUP_FMT_DEBUG("ccoRouter main()  -GCC VERSION  : %s\n", __VERSION__);

    /*mqtt��ʼ�� */
    if(mqtt_init() < 0)
    {
        CCOUP_FMT_DEBUG("ccoRouter main() mqtt_init() ��ʼ��ʧ��! \n");
        exit(1);
    }       

    /* hal��ʼ�� */
    while(0 != hal_init())
    {
        CCOUP_FMT_DEBUG("ccoRouter main() hal_init() ��ʼ��ʧ��! \n");
        sleep(1);
    }

    /* hal��ȡappid */
    appid_query(AppIdName, AppId, APP_ID_LEN);

    //����������ϢINDEX����ʼ��
    if ( pthread_mutex_init ( &ActiveInfoIndexLock, NULL ) != 0 )
    {
        CCOUP_FMT_DEBUG("ccoRouter main() init pthread ActiveInfoIndexLock lock error! \n");
        exit (1);
    }
    
    //����IPCͨ���̳߳�ʼ��
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

    //����ͨ���̲߳�����ʼ��
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


    //�Զ������̳߳�ʼ��
    for ( i = 0; i < MAX_DOWN_THREAD_NUM; i++ )
    {
        AmrThreadArg[i].nArrIndex = i;
        AmrThreadArg[i].DownCommArg = &DownTtlThreadArg[i];
        AmrThreadArg[i].UpIpcArg = &UpIpcThreadArg;
    }

#if MSG_MQTT == 1
    //��������IPC�߳�
    CCOUP_FMT_DEBUG("ccoRouter main() ��ǰ����ͨ�ŷ�ʽΪMQTT. \n");
    err = pthread_create(&UpIpcThreadID, NULL, UpMqttThread, &UpIpcThreadArg);
    if(err != 0)
    {
        CCOUP_FMT_DEBUG("ccoRouter main() 0pthread_create UpIpcThread error! \n");
        exit(1);
    }
#else
    //��������IPC�߳�
    CCOUP_FMT_DEBUG("ccoRouter main() ��ǰ����ͨ�ŷ�ʽΪUDP. \n");
    err = pthread_create(&UpIpcThreadID, NULL, UpIpcThread, &UpIpcThreadArg);
    if(err != 0)
    {
        CCOUP_FMT_DEBUG("ccoRouter main() 0pthread_create UpIpcThread error! \n");
        exit(1);
    }
#endif


#if DOWN_DEV_TYPE == 1
    //��������ͨ���߳�
    CCOUP_FMT_DEBUG("ccoRouter main() ��ǰ����ͨ��ģ��Ϊ����. \n");
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
    //��������ͨ���߳�
    CCOUP_FMT_DEBUG("ccoRouter main() ��ǰ����ͨ��ģ��Ϊ��о. \n");
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
    //��������ͨ���߳�
    CCOUP_FMT_DEBUG("ccoRouter main() ��ǰ����ͨ��ģ��ΪHAL�ӿ�. \n");
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
    //��������ͨ���߳�
    CCOUP_FMT_DEBUG("ccoRouter main() ��ǰ����ͨ��ģ��ΪHAL�ӿ�. \n");
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

    //���������߳�
    for ( i = 0; i < MAX_DOWN_THREAD_NUM; i++ )
    {
        err = pthread_create(&AmrThreadID[i], NULL, AmrThread, &AmrThreadArg[i]);
        if(err != 0)
        {
            CCOUP_FMT_DEBUG("ccoRouter main() 0pthread_create AmrThread[%d] error! \n", i);
            exit(1);
        }
    } 

    time_t CommBeginTime= time(NULL);  //ͨѶ��ʼʱ��

    //gui��ʾ
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
            //��ѯģ����Ϣ
            if(gQueryAllModuleFlag == TRUE)//�����������ʼ��, ��ѯ����ģ����Ϣ
            {
                sleep(5);
                
                CCOUP_FMT_DEBUG("ccoRouter main() if(gQueryAllModuleFlag == TRUE)...\n");
                //��ѯ����ģ����Ϣ
                if(GetAllModuleInfo() > 0)
                {
                    UpdateModuleInfo();
                    gQueryAllModuleFlag = FALSE;
                }
            }
            else//�ж��Ƿ����ģ�����¼�
            {
                //�ж��Ƿ��β�ѯģ����Ϣ
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
                    //��ѯ����ģ����Ϣ
                    ret = ModuleEventDeal();
                    if( (ret>0) && (ret<6) )
                    {
                        UpdateModuleInfo();
                        gReloadModuleFlag[0] = 0;

                        //�жϲ����Ƿ񱾵�ģ�� (������Ҫ�ڸ���֮���ж��Ƿ񱾵�ģ��)
                        if( gModuleInfo[ret-1].bCCO == TRUE )
                        {
                            //���±���ģ�鵵����Ϣ
                            UpdateModuleFiles();
                        }
                    }
                }           
            }
        }   
#endif
        //���������з�ʽ
        //printx("111111 \n");

        for ( i = 0; i < MAX_DOWN_THREAD_NUM; i++ )
        {
            if(gRebootAmrThread[i] == 1)//��������߳�֪ͨ��Ҫ��������
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
        

        //120����һ���߳��������
        //printx("222222 \n");
        if( abs(time(NULL)-CommBeginTime) > 120)
        {
            //���¼�ʱ��
            CommBeginTime= time(NULL);

            //�������IPCͨ���߳�
            if (gUpIpcThreadFlag == 1)
            {
                gUpIpcThreadFlag = 0;               //���߳����в�������־������
                UpIpcThreadCnt = 0;
            }
            else
            {
                if (UpIpcThreadCnt >= 3)            //��3�μ�ⶼ����������رմ��̣߳�Ȼ�����´򿪴��߳�
                {
                    CCOUP_FMT_DEBUG("ccoRouter main() UpIpcThread timeout, so reset it \n");

                    err = pthread_cancel(UpIpcThreadID);
                    pthread_join(UpIpcThreadID, NULL);
                #if MSG_MQTT == 1
                    //��������IPC�߳�
                    err = pthread_create(&UpIpcThreadID, NULL, UpMqttThread, &UpIpcThreadArg);
                #else
                    //��������IPC�߳�
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

            //�������ͨ���߳�
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
            

            //��Ⳮ���߳�
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















