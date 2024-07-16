/*
*********************************************************************
* Copyright(C) 2020 �Ͼ��������ӹɷ����޹�˾
* All rights reserved.
* @brief��   APPʾ��
* @date��    2020-4-16
* @history��
*********************************************************************
*/
#include "xlChannel.h"

#include "baseCommon.h"
#include "db_api.h"
#include "comm_log.h"

#include "mqtt_base.h"
#include "mqtt_proc.h"

#include "test_thread.h"
#include "client_id.h"
#include "iniparser.h"
#include "task_msg.h"

#include "framecomm.h"


pthread_t a_thread[MAX_THREAD];
#define  APP_INI_FILE            "dbfuctest.ini"
pthread_t a_threads[4];
pthread_t a_threadsSy[1];
pthread_t b_thread; 
Para  para;
extern bool FirstRun[2];
extern bool dayFrozen;

/**********************************************************************
* @name      : init_config
* @brief     ����ʼ�������ļ�
* @param[in] ��
* @param[out]��
* @return    ��0-�ɹ�/������
* @Create    : ����
* @Date      ��2020-2-27
* @Update    : 
**********************************************************************/
int app_init_config(uint32 *ip, uint16 *port)
{
    dictionary *ini = NULL;

    uint32 ipaddr = 0;
    char  *stringvalue = NULL;
    int    intvalue = 0;

    char   curpath[256] = {0};
    char   filename[256] = {0};
    int i;

    //��ȡ��ǰĿ¼����·������ȥ��������
    int cnt = readlink("/proc/self/exe", curpath, 256);
    if(cnt < 0 || cnt >= 256)
    {
        printf("get err path!");
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
    sprintf(filename, "%s%s", curpath, "dbfunctest.ini");
    printf("filename is %s!\n", filename);

    ini = iniparser_load(filename);
    if(NULL != ini)
    {
        /* ip��ȡ */
        /* ip��ȡ */
        stringvalue = iniparser_getstring(ini, "MQTT_CONF:IP", "172.17.0.1");
        ipaddr = inet_addr(stringvalue);
        ipaddr = ntohl(ipaddr);
        *ip = ipaddr;

        /* �˿ڻ�ȡ */
        intvalue = iniparser_getint(ini, "MQTT_CONF:PORT", 1883);
        *port = (uint16)intvalue;
        
        /* ��ȡipc·�� */
        stringvalue = iniparser_getstring(ini, "FILEPATH_CONF:IPCPATH", (char *)"/mnt/ipc_socket");
        task_msg_prepare(stringvalue);

        /* ��ȡlog·�� */
        stringvalue = iniparser_getstring(ini, "FILEPATH_CONF:LOGPATH", (char *)"/mnt/log");
        
        iniparser_freedict(ini);
    }
    else
    {
        printf("Get no ini file dbfunctest.ini!\n");

        *ip = 0xAC110001;
        *port = 1883;
        task_msg_prepare((char *)"/mnt/ipc_socket");
    }

    /* ��־ģ���ʼ�� */
    diag_init(filename, TRUE);

    return 0;
}


int main(int argc, char** argv)
{
    /* mqtt��ip�Ͷ˿ڿ����� ���������ļ� doto */
    uint32 mqttip = 0xAC110001;
    uint16 mqttport = 1883;
    int ret;
    int i;
   /* void *thread_result;
	void *thread_results;*/
	char  appName[128] = {0};

	//APP�ظ����м��
    if(!singleton_check((char *)APP_NAME))
    {
        GUI_FMT_DEBUG("gui already running!\n");
        exit(-1);
        return 0;
    }

	app_init_config(&mqttip, &mqttport);

	FirstRun[0] = true;
	FirstRun[1] = true;
	dayFrozen = false;

    /* ����ͨ���߳����� */
    if(0 != msg_task_start(mqttip, mqttport))
    {
        printf("msg task start failed\n");
        return -1;
    }
    //�����Զ�ɾ��
	for (i = 0; i < 2; i++)
	{
		g_dbClients[i] = db_client_create(DB_AGENT_ID, DB_CLIENT_START+i);
		if (g_dbClients[i] <= 0)
		{
			printf("g_dbClient[%d]����ʧ��, ����! \n", i);
			return -1;
		}
	}
    
    if(argc >= 2)
    {
        sprintf(appName, "%s0", APP_NAME);
        if (0 != db_mqtt_init(&g_dbMqttMsgCtx[0], appName))
    	{
    		printf(" Main recv init MQTT error!\n");
    		return -1;
    	}
        
        if(0 == memcmp(argv[1], "T", 1))
        {
            ret = pthread_create(&b_thread, NULL, thread_OadTest, NULL);
        	if (ret != 0)
        	{
        		perror("Thread creation failed!");
        		exit(EXIT_FAILURE);
        	}
        }
		else if(0 == memcmp(argv[1], "debug", strlen("debug")))
        {
            ret = pthread_create(&b_thread, NULL, thread_OadTest1, NULL);
        	if (ret != 0)
        	{
        		perror("Thread creation failed!");
        		exit(EXIT_FAILURE);
        	}
        }

    }
    else
    {
        //�����Զ�ɾ��
    	ret = pthread_create(&a_threads[0], NULL, thread_functions, NULL);
    	if (ret != 0)
    	{
    		perror("thread_functions creation failed!");
    		exit(EXIT_FAILURE);
    	}
    	usleep(100);

    	//�����Ծ�ɾ��
    	ret = pthread_create(&a_threadsSy[0], NULL, thread_functionsSY, NULL);
    	if (ret != 0)
    	{
    		perror("Thread creation failed!");
    		exit(EXIT_FAILURE);
    	}
    	usleep(100);

    	ret = pthread_create(&b_thread, NULL, thread_StabilityTest, NULL);
    	if (ret != 0)
    	{
    		perror("Thread creation failed!");
    		exit(EXIT_FAILURE);
    	}
    	usleep(100);

        int num[MAX_THREAD];
        for (i = 0; i < MAX_THREAD; i++)
        {
            sprintf(appName, "%s%d", APP_NAME, i);
        	if (0 != db_mqtt_init(&g_dbMqttMsgCtx[i], appName))
        	{
        		printf(" Main recv init MQTT error!\n");
        		return -1;
        	}
            num[i] = i;
            ret = pthread_create(&a_thread[i], NULL, thread_function, (void *)&num[i]);
            if (ret != 0)
            {
                perror("Thread creation failed!");
                exit(EXIT_FAILURE);
            }
    		usleep(100);
        }
    }
    	
    printf("Waiting for thread to finish...\n");


    while (1)
    {
        sleep(10);
    }

    exit(EXIT_FAILURE);

    return 0;
}

