/*
*********************************************************************
* Copyright(C) 2020 南京新联电子股份有限公司
* All rights reserved. 
* @brief：台区状态管理APP主函数
* @date： 2020-11-16
*********************************************************************
*/
#include "pdsAll.h"
#include "iniparser.h"
#include "pdsCommon.h"
#include "pdsMqtt.h"
#include "pdsDB_update.h"
#include "area.h"
#include "hal.h"

char mqttIP[100]={0};
int mqttport=0;

uint8 LocalAddr[6]={0};
int gCalcSock = 0;
char PrivateDir[100]={0};
uint8 isDBCenterUp=0;       //数据中心是否启动成功
uint8 gdelay=0;
/*******************************************************************************
* 函数名称: LoadIniFile
* 函数功能: 加载配置文件
* 输入参数: 
* 输出参数: 无
* 返 回 值: 
*******************************************************************************/
void LoadIniFile()
{
    char filename[256] = {0};
    dictionary *ini = NULL;
    char* tmp;
    char curpath[256]={0};
   
    int i;
     //获取当前目录绝对路径，即去掉程序名
    int cnt = readlink("/proc/self/exe", curpath, 256);
    if(cnt < 0 || cnt >= 256)
    {
        printf(" get err path!\n");
        
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
    sprintf(filename, "%s%s", curpath, PDS_INI_NAME);
    ini = iniparser_load(filename);
    printf("读取配置文件%s \n",filename);
    if(ini!=NULL)
    {
        tmp = iniparser_getstring(ini,"mqtt:MQTTIP","172.17.0.1:1883");
        printf("读取mqttip%s \n",tmp);
        strcpy(mqttIP,tmp);
        mqttport = iniparser_getint(ini,"mqtt:MQTTPORT",1883);
        printf("读取mqttport %d \n",mqttport);

        tmp = iniparser_getstring(ini,"FILEPATH_CONF:IPCPATH","/usr/local/extapps/pdsManager/socket");
        task_msg_prepare(tmp);

        tmp = iniparser_getstring(ini,"FILEPATH_CONF:PRIVATE_DIR","/usr/local/extapps/pdsManager/private");
        strcpy(PrivateDir,tmp);
        iniparser_freedict(ini);
    }
    diag_init(filename, TRUE);

    printf("读取配置文件%s 结束 \n",filename);

}

//加载一些参数 例如档案等
void LoadData()
{
    DB_CLIENT client;
    client = db_client_create(DB_PDS_AGENT, DB_PDS_CLIENT);
    if(client <= 0)
    {
        PDSDB_FMT_DEBUG("DB_PDS_CLIENT db_client_create ERROR \n");
        return ;
    }
    //读取档案
    MeterFileInit(client);
    appdb_task_list_record_init(client);
    appdb_plan_normal_list_record_init(client);
    ReadLocalAddr(client,LocalAddr);
    GetLineLossPeriod(client);
#ifdef AREA_HUNAN
    GetImpedanceLimit(client);
#endif
#ifdef AREA_JIANGXI
    GetLineLossPeriod(client);
#endif

}

/*******************************************************************************
* 函数名称: calc_thread
* 函数功能: 计算线程，负责拓扑结构的计算，线损和阻抗的计算
* 输入参数: 
* 输出参数: 无
* 返 回 值: 
*******************************************************************************/
void* calc_thread(void* arg)
{
    #ifdef AREA_HUNAN
    int Recvlen;
    uint8 RecvBuf[TASK_MSG_MAX_LEN] = {0};
    uint32 sender ;
    #endif
    calcThreadInit();
    for(;;)
    {
        #ifdef AREA_HUNAN
        Recvlen = task_recv_msg(gCalcSock, RecvBuf, TASK_MSG_MAX_LEN, &sender);
        if(Recvlen>0)
        {
            TASK_MSG_t *msg = (TASK_MSG_t *)RecvBuf;
            switch(msg->msg_id)
            {
                case MSG_PDS_TOPOSTART:
                {
                    PDS_FMT_DEBUG("收到启动拓扑消息 \n");
                    checkTopology(msg->msg,msg->msglen);
                }
                    break;
                default:
                    break;
            }
            memset(RecvBuf,0x00,sizeof(RecvBuf));
        }
        #endif
        TimeManage();
        #ifdef AREA_HUNAN
        topologyManager();
        #endif
		#ifdef PRODUCT_CCU
		CalcTaskCheckNew();
		#else
        CalcTaskCheck();
		#endif
        if(gdelay != 0)
        {
            usleep(gdelaytime);
        }
		
    }
}
//
///*******************************************************************************
//* 函数名称: calc_thread
//* 函数功能: 计算线程，负责拓扑结构的计算，线损和阻抗的计算
//* 输入参数: 
//* 输出参数: 无
//* 返 回 值: 
//*******************************************************************************/
//void* calc_lineloss_thread(void* arg)
//{
//    int Recvlen;
//    uint8 RecvBuf[TASK_MSG_MAX_LEN] = {0};
//    uint32 sender ;
//
//    LinelossThreadInit();
//    for(;;)
//    {
//
//        CalcLineloss();
//    }
//}


int main(int argc,char* argv[])
{
    int ret;
    pthread_t updateid = 0;
    pthread_t calcid=0;
#ifdef AREA_JIANGXI
    pthread_t topoId=0;

#endif

    /* 检查程序只启动了一个副本 */
    if(!singleton_check(PDS_APP_NAME))
    {
        return -1;
    }
        /* 加载hal */
    if(0 != hal_init())
    {
        printf("hal_init failed!\n");
        return -2;
    }
    
    LoadIniFile();
    PDS_FMT_DEBUG("台区状态管理APP启动！！！\n");
    mainInit();
    if(mqtt_init(mqttIP,mqttport)<0)
    {
        PDS_FMT_DEBUG("mqtt初始化失败 ！\n");
        return -1;
    }
    //检查数据中心是否启动
    while(1)
    {
        PDS_FMT_DEBUG("检查数据中心是否启动 \n");
        if(isDBCenterUp==0)
        {
            mqtt_query_app_info(DBCENTER_NAME);
        }else
        {
            break;
        }
        sleep(3);
    }

    LoadData();
    ret = pthread_create(&updateid,NULL,db_update_entry,NULL);
    if(ret!=0)
    {
        PDS_FMT_DEBUG("创建db_update线程失败 ！！ \n");
        return -1;
    }
//#ifdef AREA_HUNAN
//    ret = pthread_create(&calcid,NULL,calc_thread,NULL);
//    if(ret!=0)
//    {
//        PDS_FMT_DEBUG("创建calc_thread线程失败 ！！ \n");
//        return -1;
//    }
//#endif

    ret = pthread_create(&calcid,NULL,calc_thread,NULL);
    if(ret!=0)
    {
        PDS_FMT_DEBUG("创建calc_thread线程失败 ！！ \n");
        return -1;
    }

#ifdef AREA_JIANGXI
        ret = pthread_create(&topoId, NULL, TopoThread, NULL);

        if (ret != 0)
        {
            PDS_FMT_DEBUG("创建TopoThread线程失败 ！！ \n");
            return -1;
        }
//        ret = pthread_create(&callinecid,NULL,calc_lineloss_thread,NULL);
//        if(ret!=0)
//        {
//            PDS_FMT_DEBUG("创建calc_thread线程失败 ！！ \n");
//            return -1;
//        }
#endif

    while(1)
    {
        sleep(2);
    }
    return 0;
}



