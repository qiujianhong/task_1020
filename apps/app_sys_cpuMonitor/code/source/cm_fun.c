#include "framecomm.h"
#include "cm_fun.h"
#define CM_APPNAME  "cpuMonitor"
#define CM_APP_INI  "cpuMonitor.ini"

/**********************************************************************
* @name      : cm_version_print
* @brief     ：版本打印
* @param[in] ：uint32 buflen     缓存长度
* @param[out]：char *buf         输出日志
* @return    ：实际长度
* @Create    :
* @Date      ：
* @Update    :
**********************************************************************/
int cm_version_print(char *buf, uint32 buflen)
{
    return snprintf(buf, buflen, "Version: %s, Time: %s", APP_VERSION, APP_PUBLISH_TIME);
}


/**********************************************************************
* @name      : cm_app_init
* @brief     ：初始化
* @param[in] ：
* @param[out]：void **ppPublicData
* @return    ：0-成功/错误码
* @Create    :
* @Date      ：
* @Update    :
**********************************************************************/
int cm_app_init(void **ppPublicData)
{
    int ret = 0;
    if(!singleton_check(CM_APPNAME))
    {
        printf("%s is started!\n", CM_APPNAME);
        return -1;
    }

    char   curpath[256] = {0};
    char   filename[256] = {0};
    int    i;

    int cnt = readlink("/proc/self/exe", curpath, 256);
    if(cnt < 0 || cnt >= 256)
    {
        printf("cpuChange get err path!\n");
    }
    //获取当前目录绝对路径,即去掉程序名

    for (i = cnt; i >= 0; --i)
    {
        if (curpath[i] == '/')
        {
            curpath[i+1] = '\0';
            break;
        }
    }

    sprintf(filename, "%s%s", curpath,CM_APP_INI );
    /* 日志模块初始化 */
    diag_init(filename, TRUE);


    return ret;
}


/*******************************************************************************
* 函数名称: cmd_exec
* 函数功能: 执行shell命令 返回
* 输入参数: DataLen  数据长度
*           SrcData  数据缓冲区
* 输出参数: 无
* 返 回 值: 累加和
*******************************************************************************/

char* cm_cmd_exec(const char* cmd, char* buff, int size)
{
    char temp[256];
    FILE* fp = NULL;
    int offset = 0;
    int len;

    fp = popen(cmd, "r");
    if(fp == NULL)
    {
        return NULL;
    }

    while(fgets(temp, sizeof(temp), fp) != NULL)
    {
        len = strlen(temp);
        if(offset + len < size)
        {
            strcpy(buff+offset, temp);
            offset += len;
        }
        else
        {
            buff[offset] = 0;
            break;
        }
    }

    if(fp != NULL)
    {
        pclose(fp);
    }

    return buff;
}

/**********************************************************************
* @name      : cm_thread_init
* @brief     ：
* @param[in] ：void *pPublic    公有数据
* @param[out]：void **ppPrivate 私有数据
* @return    ：0-成功/错误码
* @Create    :
* @Date      ：
* @Update    :
**********************************************************************/
int cm_thread_init(void *pPublic, void **ppPrivate)
{
    CM_PRIVATE_T *PrivateData = NULL;

    PrivateData = (CM_PRIVATE_T *)malloc(sizeof(CM_PRIVATE_T));
    if(NULL == PrivateData)
    {
        INIT_FMT_DEBUG("CM_PRIVATE_T 初始化失败 \n");
        return -1;
    }
    memset(PrivateData,0x00,sizeof(CM_PRIVATE_T));
    PrivateData->sleeptime = 1000000;
    *ppPrivate  = PrivateData;
    INIT_FMT_DEBUG(" cpuMonitor 初始化成功 \n");
    char cmd[1000]={0};
    sprintf(cmd,"rm -rf %s",DELAY_FILE_PATH);
    system(cmd);
    sprintf(cmd,"rm -rf %s",DELAY_FILE_PATH2);
    system(cmd);
    return 0;
}


/**********************************************************
* 函 数 名 : cm_cpu_temperature_get
* 函数功能 : 获取当前CPU温度
* 输入参数 : 无
* 输出参数 : 无
* 返 回 值 : 成功 : CPU温度 换算-1
*            失败 : 0
* 修改历史 :  2024/1/2
************************************************************/
int cm_cpu_temperature_get()
{
    char tempstr[40] = {0};
    int  tempvalue = 0;

    FILE* pipe = fopen("/sys/class/thermal/thermal_zone0/temp", "r");
    if (!pipe)
    {
        pipe = fopen("/sys/class/thermal/thermal_zone1/temp", "r");
    }
    if (!pipe)
    {
        return 0;
    }

    memset(tempstr, 0, 40);
    if(fgets(tempstr, 40, pipe) == NULL)
    {
        fclose(pipe);
        return 0;
    }

    tempvalue = atoi(tempstr);
    //801终端里面的值直接就是温度 换算-1得再乘以10
    //tempvalue = tempvalue / 100;
    tempvalue = tempvalue*10;
    fclose(pipe);
    return tempvalue;
}

/**********************************************************
* 函 数 名 : cm_process_check
* 函数功能 : 检查指定进程是否存在
* 输入参数 : 进程名称
* 输出参数 : 无
* 返 回 值 : 成功 : 0
*            失败 : -1
* 修改历史 :  2024/1/10
************************************************************/
int cm_process_check(const char *process)
{
    char cmd[1024] = {0};
    char recvbuf[2048]={0};
    snprintf(cmd,sizeof(cmd),"ps -ef|grep %s |grep -v grep",process);
    cm_cmd_exec(cmd,recvbuf,sizeof(recvbuf));
    if(strlen(recvbuf) != 0)
    {
        return 0;
    }
    return -1;
}

/**********************************************************
* 函 数 名 : cm_performance_check
* 函数功能 : 通过命令来检测是否在进行性能测试
* 输入参数 : 无
* 输出参数 : 无
* 返 回 值 : 成功 : 0
*            失败 : -1
* 修改历史 :  2024/1/10
************************************************************/
int cm_performance_check()
{
    char cmd[1024] = {0};
    char recvbuf[2048]={0};
    /* 不能带上stream 可能会误判 */
    snprintf(cmd,sizeof(cmd),"ps -ef|grep -E 'dhrystone|whetstone|wprime|pi_css5|bonnie++' |grep -v grep");
    cm_cmd_exec(cmd,recvbuf,sizeof(recvbuf));
    if(strlen(recvbuf) != 0)
    {
        return 0;
    }
    return -1;
}

/**********************************************************
* 函 数 名 : cm_cpu_fre_change
* 函数功能 : cpu频率设置
* 输入参数 : flag 0 设置到1.0
             flag 1 设置到1.2
* 输出参数 :
* 返 回 值 :
* 修改历史 :  2024/1/10
************************************************************/
void cm_cpu_freq_set(uint8 flag)
{
    //801不调cpu频率
    return;
    char cmd[1000]={0};
    char recvbuf[2048]={0};
    uint32 value =0;
    if(flag == 1 )
    {
        value = 1200000;
    }else
    {
        value = 1008000;
    }

    memset(cmd, 0, sizeof(cmd));
    memset(recvbuf, 0, sizeof(recvbuf));
    snprintf(cmd,sizeof(cmd),"echo %d > /sys/devices/system/cpu/cpu0/cpufreq/scaling_max_freq",value);
    cm_cmd_exec(cmd,recvbuf,sizeof(recvbuf));
    memset(cmd, 0, sizeof(cmd));
    memset(recvbuf, 0, sizeof(recvbuf));
    snprintf(cmd,sizeof(cmd),"echo %d > /sys/devices/system/cpu/cpu0/cpufreq/scaling_min_freq",value);
    cm_cmd_exec(cmd,recvbuf,sizeof(recvbuf));
    memset(cmd, 0, sizeof(cmd));
    memset(recvbuf, 0, sizeof(recvbuf));
    snprintf(cmd,sizeof(cmd),"echo %d > /sys/devices/system/cpu/cpu1/cpufreq/scaling_max_freq",value);
    cm_cmd_exec(cmd,recvbuf,sizeof(recvbuf));
    memset(cmd, 0, sizeof(cmd));
    memset(recvbuf, 0, sizeof(recvbuf));
    snprintf(cmd,sizeof(cmd),"echo %d > /sys/devices/system/cpu/cpu1/cpufreq/scaling_min_freq",value);
    cm_cmd_exec(cmd,recvbuf,sizeof(recvbuf));
    memset(cmd, 0, sizeof(cmd));
    memset(recvbuf, 0, sizeof(recvbuf));
    snprintf(cmd,sizeof(cmd),"echo %d > /sys/devices/system/cpu/cpu2/cpufreq/scaling_max_freq",value);
    cm_cmd_exec(cmd,recvbuf,sizeof(recvbuf));
    memset(cmd, 0, sizeof(cmd));
    memset(recvbuf, 0, sizeof(recvbuf));
    snprintf(cmd,sizeof(cmd),"echo %d > /sys/devices/system/cpu/cpu2/cpufreq/scaling_min_freq",value);
    cm_cmd_exec(cmd,recvbuf,sizeof(recvbuf));
    memset(cmd, 0, sizeof(cmd));
    memset(recvbuf, 0, sizeof(recvbuf));
    snprintf(cmd,sizeof(cmd),"echo %d > /sys/devices/system/cpu/cpu3/cpufreq/scaling_max_freq",value);
    cm_cmd_exec(cmd,recvbuf,sizeof(recvbuf));
    memset(cmd, 0, sizeof(cmd));
    memset(recvbuf, 0, sizeof(recvbuf));
    snprintf(cmd,sizeof(cmd),"echo %d > /sys/devices/system/cpu/cpu3/cpufreq/scaling_min_freq",value);
    cm_cmd_exec(cmd,recvbuf,sizeof(recvbuf));

}

int cm_thread_proc(void *pArg, void *pPublic, void *pPrivate)
{
    CM_PRIVATE_T *pprivate = (CM_PRIVATE_T *)pPrivate;
    char cmd[1000]={0};
    //char recvbuf[2048]={0};
    //struct timespec curtime = {0};
    //static struct timespec oldtime = {0};
	//char recvbuf2[2048]={0};
    struct timespec timenow;
    //uint32 i=0;
    int cputemperature;
    int ret;
    switch(pprivate->upflag)
    {
        /*初始态 检查有没有testService进程  */
        case CM_INIT:
        {
            ret = cm_process_check("testService");
            if(ret == 0)
            {
                INIT_FMT_DEBUG("检查到testService进程 开始加压\n");
                clock_gettime(CLOCK_MONOTONIC, &timenow);
                pprivate->beginTime = timenow.tv_sec;
                pprivate->lastTime = timenow.tv_sec;
                pprivate->upflag = CM_PRESS;
                pprivate->sleeptime = 1;
                pprivate->sleeptime = 1000000;
            }
        }
            break;
        /* 开始加压  并同时检测有没有性能测试的进程 没检测到就两分钟后进入温度检测状态*/
        case CM_PRESS:
        {
            clock_gettime(CLOCK_MONOTONIC, &timenow);
            if(timenow.tv_sec - pprivate->beginTime > 120)
            {
                pprivate->upflag = CM_TEMP;
                pprivate->sleeptime = 1000000;
                INIT_FMT_DEBUG("加压结束\n");
                break;
            }
            /* 过秒检测有没有性能检测的进程 whetstone wprime pi_css5 bonnie++ stream */
            /* ps -ef|grep -E "whetstone|wprime|pi_css5|bonnie++|stream" |grep -v grep */
            if(timenow.tv_sec != pprivate->lastTime)
            {
                ret = cm_performance_check();
                if(ret == 0)
                {
                    /* 检测到正在进行性能检测 进入状态CM_OVER */
                    pprivate->upflag = CM_OVER;
                    pprivate->sleeptime = 1000000;
                    INIT_FMT_DEBUG("性能检测模式\n");
                    memset(cmd,0x00,sizeof(cmd));
                    snprintf(cmd,sizeof(cmd),"touch %s",DELAY_FILE_PATH2);
                    system(cmd);
                    cm_cpu_freq_set(1);
                    pprivate->lastFreqTime = timenow.tv_sec;
                    break;
                }
                pprivate->lastTime = timenow.tv_sec;
            }
            //配上return 1 大概会占用cpu15%
            /* 改成2000 */
            /* 改一个不加压的版本 */
/*             while(i<2000)
            {
                i++;
            } */
        }
            break;
        /* 性能检测模式下 要监控性能检测的进程是否持续30秒没有                            判断到了就恢复频率 */
        case CM_OVER:
        {
            clock_gettime(CLOCK_MONOTONIC, &timenow);

            ret = cm_performance_check();
            if(ret < 0)
            {
                //INIT_FMT_DEBUG("没有性能检测\n");
                /* 如果没有性能检测进程30秒 则退出该模式 */
                if(timenow.tv_sec - pprivate->lastFreqTime > 30)
                {
                    pprivate->upflag = CM_END;
                    pprivate->sleeptime = 1000000;
                    INIT_FMT_DEBUG("退出性能检测模式\n");
                    memset(cmd,0x00,sizeof(cmd));
                    snprintf(cmd,sizeof(cmd),"rm -rf  %s",DELAY_FILE_PATH2);
                    system(cmd);
                    cm_cpu_freq_set(0);
                    break;
                }
            }else
            {
                pprivate->lastFreqTime = timenow.tv_sec;
            }

        }
            break;
        case CM_TEMP:
        {
            if(pprivate->fileFlag == 0)
            {
                cputemperature = cm_cpu_temperature_get();
                if(cputemperature >= 750)
                {
                    INIT_FMT_DEBUG("检测到CPU温度 %d\n",cputemperature);
                    //2标记为进一步延时
                    pprivate->fileFlag = 1;
                    memset(cmd,0x00,sizeof(cmd));
                    sprintf(cmd,"touch %s",DELAY_FILE_PATH2);
                    system(cmd);
                }
            }
            if(cm_process_check("testService") < 0 && cm_process_check("cpu_stress") < 0)
            {
                INIT_FMT_DEBUG("测试结束\n");
                pprivate->upflag = CM_INIT;
                memset(cmd,0x00,sizeof(cmd));
                sprintf(cmd,"rm -rf %s",DELAY_FILE_PATH2);
                system(cmd);
                pprivate->fileFlag = 0;
            }
        }
            break;
        case CM_END:
        {
            ret = cm_process_check("testService");
            if(ret < 0)
            {
                INIT_FMT_DEBUG("监控结束\n");
                pprivate->upflag = CM_INIT;
                break;
            }
            /* 如果没停testService继续进行测试 则还是超频 */
            ret = cm_performance_check();
            if(ret == 0)
            {
                /* 检测到正在进行性能检测 进入状态CM_OVER */
                pprivate->upflag = CM_OVER;
                pprivate->sleeptime = 1000000;
                INIT_FMT_DEBUG("性能检测模式\n");
                memset(cmd,0x00,sizeof(cmd));
                snprintf(cmd,sizeof(cmd),"touch %s",DELAY_FILE_PATH2);
                system(cmd);
                cm_cpu_freq_set(1);
                pprivate->lastFreqTime = timenow.tv_sec;
                break;
            }
        }
            break;
        default:
        {
            pprivate->upflag = CM_INIT;
        }
            break;

    }

#if 0
	if(pprivate->upflag == 0)
    {
        sprintf(cmd,"ps -ef|grep testService |grep -v grep");
        cm_cmd_exec(cmd,recvbuf,sizeof(recvbuf));
        if(strlen(recvbuf)!=0)
        {
            INIT_FMT_DEBUG("检查到testService进程 开始加压\n");
            clock_gettime(CLOCK_MONOTONIC, &timenow);
            pprivate->beginTime = timenow.tv_sec;
            pprivate->lastTime = timenow.tv_sec;
            /* 不再加压 直接到状态2 240103*/
            /* pprivate->upflag = 1; */
            pprivate->upflag = 1;
            pprivate->sleeptime = 1;
            //pprivate->sleeptime = 1000000;
            //memset(cmd,0x00,sizeof(cmd));
            //sprintf(cmd,"/usr/local/extapps/cpuMonitor/bin/onlineMonitor &");
            //system(cmd);
           // INIT_FMT_DEBUG("执行命令 %s\n",cmd);
        }
    }
    else if(pprivate->upflag == 1)
    {
        //处在加压环境下检查下是否有cpu_stress 进程或者加压时间超过10分钟
        //加两分钟压 然后结束
        clock_gettime(CLOCK_MONOTONIC, &timenow);
        if(timenow.tv_sec - pprivate->beginTime > 120)
        {
            pprivate->upflag = 2;
            pprivate->sleeptime = 1000000;
            INIT_FMT_DEBUG("加压结束\n");
        }
//        if(timenow.tv_sec != pprivate->lastTime)
//        {
//            sprintf(cmd,"ps -ef|grep cpu_stress |grep -v grep");
//            cm_cmd_exec(cmd,recvbuf,sizeof(recvbuf));
//            /* 改成5分钟 */
//            if(strlen(recvbuf)!=0 || timenow.tv_sec - pprivate->beginTime > 5*60)
//            {
//                INIT_FMT_DEBUG("停止加压\n");
//                pprivate->upflag = 2;
//                pprivate->sleeptime = 1000000;
//
////                memset(cmd,0x00,sizeof(cmd));
////                sprintf(cmd,"ps -ef|grep onlineMonitor |grep -v grep|awk '{print $1}' |xargs kill");
////                system(cmd);
////                INIT_FMT_DEBUG("执行命令 %s\n",cmd);
//            }
//            pprivate->lastTime = timenow.tv_sec;
//        }
        //配上return 1 大概会占用cpu15%
        while(i<3500)
        {
            i++;
        }

    }
    else if(pprivate->upflag == 2)
    {
        /* 上来先延时 然后等温度超过75的时候再加延时 */
        /* 检测到CPU温度超过75度 则置标记 */
//        if(pprivate->fileFlag == 0)
//        {
///*             cputemperature = cm_cpu_temperature_get();
//            if(cputemperature >= 750) */
//            {
//                /* INIT_FMT_DEBUG("检测到CPU温度 %d\n",cputemperature); */
//                INIT_FMT_DEBUG("降低延时 \n");
//                pprivate->fileFlag = 1;
//                memset(cmd,0x00,sizeof(cmd));
//                sprintf(cmd,"touch %s",DELAY_FILE_PATH);
//                system(cmd);
//            }
//        }
        if(pprivate->fileFlag == 0)
        {
            cputemperature = cm_cpu_temperature_get();
            if(cputemperature >= 750)
            {
                INIT_FMT_DEBUG("检测到CPU温度 %d\n",cputemperature);
                //2标记为进一步延时
                pprivate->fileFlag = 1;
                memset(cmd,0x00,sizeof(cmd));
                //sprintf(cmd,"rm -rf %s",DELAY_FILE_PATH);
                //system(cmd);
                sprintf(cmd,"touch %s",DELAY_FILE_PATH2);
                system(cmd);
            }
        }
        //测试过程中 检查测试是否完成
        sprintf(cmd,"ps -ef|grep testService |grep -v grep");
        cm_cmd_exec(cmd,recvbuf,sizeof(recvbuf));
        memset(cmd,0x00,sizeof(cmd));
        sprintf(cmd,"ps -ef|grep cpu_stress |grep -v grep");
        cm_cmd_exec(cmd,recvbuf2,sizeof(recvbuf));
        if(strlen(recvbuf)==0 && strlen(recvbuf2)==0)
        {
            INIT_FMT_DEBUG("测试结束\n");
            pprivate->upflag = 0;
            memset(cmd,0x00,sizeof(cmd));
            //sprintf(cmd,"rm -rf %s",DELAY_FILE_PATH);
            //system(cmd);
            sprintf(cmd,"rm -rf %s",DELAY_FILE_PATH2);
            system(cmd);
            pprivate->fileFlag = 0;
        }
    }

    if(pprivate->cpflag == 0)
    {
        memset(cmd, 0, sizeof(cmd));
        memset(recvbuf, 0, sizeof(recvbuf));
        sprintf(cmd,"ps |grep testService |grep -v grep");
        cm_cmd_exec(cmd,recvbuf,sizeof(recvbuf));
        if(strlen(recvbuf)!=0)
        {
            INIT_FMT_DEBUG("检查到testService进程 准备停止其它app\n");
            pprivate->cpflag = 1;
            clock_gettime(CLOCK_MONOTONIC, &oldtime);
        }
    }else if(pprivate->cpflag == 1)
    {
        //卸载容器
        //先把停止taskmanager的逻辑去掉，停一个app没什么意义
        //memset(cmd, 0, sizeof(cmd));
        //memset(recvbuf, 0, sizeof(recvbuf));
        //sprintf(cmd,"appm -u -c edgerContainer -n taskManager");
        //cm_cmd_exec(cmd,recvbuf,sizeof(recvbuf));

        //超频
        memset(cmd, 0, sizeof(cmd));
        memset(recvbuf, 0, sizeof(recvbuf));
        sprintf(cmd,"echo 1200000 > /sys/devices/system/cpu/cpu0/cpufreq/scaling_max_freq");
        cm_cmd_exec(cmd,recvbuf,sizeof(recvbuf));
        memset(cmd, 0, sizeof(cmd));
        memset(recvbuf, 0, sizeof(recvbuf));
        sprintf(cmd,"echo 1200000 > /sys/devices/system/cpu/cpu0/cpufreq/scaling_min_freq");
        cm_cmd_exec(cmd,recvbuf,sizeof(recvbuf));
        memset(cmd, 0, sizeof(cmd));
        memset(recvbuf, 0, sizeof(recvbuf));
        sprintf(cmd,"echo 1200000 > /sys/devices/system/cpu/cpu1/cpufreq/scaling_max_freq");
        cm_cmd_exec(cmd,recvbuf,sizeof(recvbuf));
        memset(cmd, 0, sizeof(cmd));
        memset(recvbuf, 0, sizeof(recvbuf));
        sprintf(cmd,"echo 1200000 > /sys/devices/system/cpu/cpu1/cpufreq/scaling_min_freq");
        cm_cmd_exec(cmd,recvbuf,sizeof(recvbuf));
        memset(cmd, 0, sizeof(cmd));
        memset(recvbuf, 0, sizeof(recvbuf));
        sprintf(cmd,"echo 1200000 > /sys/devices/system/cpu/cpu2/cpufreq/scaling_max_freq");
        cm_cmd_exec(cmd,recvbuf,sizeof(recvbuf));
        memset(cmd, 0, sizeof(cmd));
        memset(recvbuf, 0, sizeof(recvbuf));
        sprintf(cmd,"echo 1200000 > /sys/devices/system/cpu/cpu2/cpufreq/scaling_min_freq");
        cm_cmd_exec(cmd,recvbuf,sizeof(recvbuf));
        memset(cmd, 0, sizeof(cmd));
        memset(recvbuf, 0, sizeof(recvbuf));
        sprintf(cmd,"echo 1200000 > /sys/devices/system/cpu/cpu3/cpufreq/scaling_max_freq");
        cm_cmd_exec(cmd,recvbuf,sizeof(recvbuf));
        memset(cmd, 0, sizeof(cmd));
        memset(recvbuf, 0, sizeof(recvbuf));
        sprintf(cmd,"echo 1200000 > /sys/devices/system/cpu/cpu3/cpufreq/scaling_min_freq");
        cm_cmd_exec(cmd,recvbuf,sizeof(recvbuf));
        pprivate->cpflag = 2;
    }else if(pprivate->cpflag == 2)
    {
        clock_gettime(CLOCK_MONOTONIC, &curtime);
        //memset(cmd, 0, sizeof(cmd));
        //sprintf(cmd,"ps |grep testService |grep -v grep");
        //cm_cmd_exec(cmd,recvbuf2,sizeof(recvbuf2));
        //if(strlen(recvbuf2)==0)
        //{
        //    INIT_FMT_DEBUG("检查到testService进程终止 准备启动其它app\n");
        //    memset(cmd, 0, sizeof(cmd));
        //    sprintf(cmd,"cd /usr/local/extapps/taskManager/bin;./startup_app.sh");
        //    cm_cmd_exec(cmd,recvbuf2,sizeof(recvbuf2));

        //    pprivate->upflag = 3;
        //}

        if(curtime.tv_sec - oldtime.tv_sec > 60*10)
        {
            INIT_FMT_DEBUG("检查到testService进程超过10分钟curtime: %lu,  oldtime: %lu, 准备启动其它app\n", curtime.tv_sec, oldtime.tv_sec);
            //memset(cmd, 0, sizeof(cmd));
            //sprintf(cmd,"cd /usr/local/extapps/taskManager/bin;./startup_app.sh");
            //cm_cmd_exec(cmd,recvbuf2,sizeof(recvbuf2));

            //降频
            memset(cmd, 0, sizeof(cmd));
            memset(recvbuf, 0, sizeof(recvbuf));
            sprintf(cmd,"echo 1008000 > /sys/devices/system/cpu/cpu0/cpufreq/scaling_min_freq");
            cm_cmd_exec(cmd,recvbuf,sizeof(recvbuf));
            memset(cmd, 0, sizeof(cmd));
            memset(recvbuf, 0, sizeof(recvbuf));
            sprintf(cmd,"echo 1008000 > /sys/devices/system/cpu/cpu0/cpufreq/scaling_max_freq");
            cm_cmd_exec(cmd,recvbuf,sizeof(recvbuf));
            memset(cmd, 0, sizeof(cmd));
            memset(recvbuf, 0, sizeof(recvbuf));
            sprintf(cmd,"echo 1008000 > /sys/devices/system/cpu/cpu1/cpufreq/scaling_min_freq");
            cm_cmd_exec(cmd,recvbuf,sizeof(recvbuf));
            memset(cmd, 0, sizeof(cmd));
            memset(recvbuf, 0, sizeof(recvbuf));
            sprintf(cmd,"echo 1008000 > /sys/devices/system/cpu/cpu1/cpufreq/scaling_max_freq");
            cm_cmd_exec(cmd,recvbuf,sizeof(recvbuf));
            memset(cmd, 0, sizeof(cmd));
            memset(recvbuf, 0, sizeof(recvbuf));
            sprintf(cmd,"echo 1008000 > /sys/devices/system/cpu/cpu2/cpufreq/scaling_min_freq");
            cm_cmd_exec(cmd,recvbuf,sizeof(recvbuf));
            memset(cmd, 0, sizeof(cmd));
            memset(recvbuf, 0, sizeof(recvbuf));
            sprintf(cmd,"echo 1008000 > /sys/devices/system/cpu/cpu2/cpufreq/scaling_max_freq");
            cm_cmd_exec(cmd,recvbuf,sizeof(recvbuf));
            memset(cmd, 0, sizeof(cmd));
            memset(recvbuf, 0, sizeof(recvbuf));
            sprintf(cmd,"echo 1008000 > /sys/devices/system/cpu/cpu3/cpufreq/scaling_min_freq");
            cm_cmd_exec(cmd,recvbuf,sizeof(recvbuf));
            memset(cmd, 0, sizeof(cmd));
            memset(recvbuf, 0, sizeof(recvbuf));
            sprintf(cmd,"echo 1008000 > /sys/devices/system/cpu/cpu3/cpufreq/scaling_max_freq");
            cm_cmd_exec(cmd,recvbuf,sizeof(recvbuf));

            //2023.12.29:测试发现testService拉起来就一直在了，如果将状态置成0，就不断的超频降频循环了
            pprivate->cpflag = 3;
        }
    }
#endif
    return pprivate->sleeptime;
}


