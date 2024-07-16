/*
*********************************************************************
* Copyright(C) 2020 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   模组管理器
* @date：    2020-03
* @history： 
*********************************************************************
*/

#include "pageInterface.h"
#include "mdlmgr.h"
#include "mdlcommon.h"
#include "Mqtt.h"
#include <pthread.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <hal.h>
#include <sys/syscall.h>
#include "gui_c_prtl.h"
#include "message.h"
#include "appmsg_split.h"
#include "dictionary.h"
#include "iniparser.h"
#include "framecomm.h"
#include "trace_back.h"

#define  MDLMGR_INI_FILE            "mapManager.ini"
#define  MDLSTARTLOG                "/data/app/mapManager/log/mdlstart.log"
extern PLUGIN_INFO* g_plugin[];
extern int          g_nPluginCnt;
BOOL                gBuzzerEn = FALSE;
int16               g_flag_thread_msg[mdlnum];  // 线程守护标志及通道的信息
int                 mdl_update_thread_cnt = 0;  //模组升级子线程监测计数器

MDL_UPDATE_DEAL_T   MdlUpdateDeal = MDL_UPDATE_IDLE;
int32               g_UpdateUsbNo = -1;         //记录正在升级的槽位
int                 g_MqttThreadFlag = 0;       //用于检测mqtt线程是否正常

uint8               Event_Flag[mdlnum] ;        //用于检测是否产生事件
PLUG_STATUS_T       plug_status[mdlnum];        //模组插拔状态
MDL_CHANNEL_CFG_T   mdlChannelCfg[mdlnum];      //模组通道信息，从配置文件中读取
SLOT_INFO_T         slotInfo[mdlnum];           //模组槽位信息
MODE_ALL_INFO_T     moduleInfo_all[mdlnum];     //模组通用信息
REMOTE_PULSE_T      YX_pulse_info;              //遥信脉冲模块信息体
RELAY_OUTMODE_E     relayMode;                  //继电器输出模式
uint16              pulseWidth;                 //继电器输出脉冲宽度
UPDATE_INFO_ALL_T   updateAllInfo;              //升级数据结构体
uint8               globleSeq = 0;              //全局序列号
static uint8        update_seq = 0;             //升级文件序列号
char                usbPath[256] = {0};         //usb路径
char                LinkDir[100] = {0};         //软连接路径   
pthread_mutex_t     closeLock;
time_t              clockSet[mdlnum] = {0};     //模组时钟设置标志，用于查询
uint8               LoginFlag = 0;
MOD_OFF_POWER_T     modPowerCtl[mdlnum];        //模组断电控制信息
static uint8        modEventFlag[mdlnum] = {0}; //事件已上报标志
MDL_COMM_PARA       g_tDevPara[mdlnum];
POWER_DEVICE_T*     dev_power = NULL;

char    pathname[mdlnum][512] = {{0}, {0}, {0}, {0}, {0}};
char    mdlLog[100] = {0};                       //日志相关变量

int modInfoGetCnt[mdlnum] = {0};                 //模组信息查询次数
int modCnt[mdlnum] = {0};

int32 gCtrlDev[mdlnum] = {-1, -1, -1, -1, -1};   //管理通道设备状态

//调节日志路径
char *gDownRecordPath[mdlnum + 1] = {"mdlmgr1", "mdlmgr2", 
                                     "mdlmgr3", "mdlmgr4", 
                                     "mdlmgr5", "mdlmgr"}; 

int16 GetStartAddrOfFrame(uint8 *czBuf, int16 len);

/***********************************************************************
 * @brief   模组管理器虚拟口初始化
 * @param[in]  usbNo - USB号
 *
 * @retval  0   成功
 *         其他 失败
 **********************************************************************/
int32 module_ctrl_comm_init(int32 usbNo)
{
    int32 ret = -1;

    sprintf((char*)g_tDevPara[usbNo].deviceName, "uart_acm%d_0", (usbNo + 1));

    // 打开虚拟串口
    ret = mdl_comm_init(&g_tDevPara[usbNo]);

    return ret;
}

/***********************************************************************
 * @brief   模组管理器虚拟串口关闭
 * @param[in]  usbNo - USB号
 *
 * @retval  0   成功
 *         其他 失败
 **********************************************************************/
void module_ctrl_comm_uninit(int32 usbNo)
{    
    //关闭虚拟串口
    pthread_mutex_lock(&closeLock);
    mdl_comm_uninit(&g_tDevPara[usbNo]);
    pthread_mutex_unlock(&closeLock);
    return;
}


/***********************************************************************
 * @brief      发送函数
 * @param[in]  usbNo   -  USB号
 *             pBuffer -  发送缓存
 *             nBufLen -  发送长度
 * @retval     0   成功
 *             其他 失败
 **********************************************************************/
int32 SendData(int32 usbNo, uint8* pBuffer, int32 nBufLen)
{
    return mdl_comm_send(&g_tDevPara[usbNo], pBuffer, nBufLen);     /*发送数据*/
}

/***********************************************************************
 * @brief      接收函数
 * @param[in]  usbNo   -  USB号
 * @param[out] pBuffer -  接收缓存
 *             nBufLen -  接收长度
 * @retval     0   成功
 *             其他 失败
 **********************************************************************/
int32 RecvData(int32 usbNo, uint8* pBuffer, int32 nBufLen)
{
    int32 recv_len;
    recv_len = mdl_comm_recv(&g_tDevPara[usbNo], pBuffer, nBufLen, 150, 8, 0xFF);
    return recv_len;
}

/**
*********************************************************************
* @brief:        将日志名称上加通道号
* @param[in]：   filepath 日志名称
* @param[in]：   pipe     通道号
* @Date：        2020-06-7
* @return：      char *
*********************************************************************
*/
char *get_logname_by_pipe(char* filepath,int8 pipe)
{
    if (pipe < 0 || pipe >= mdlnum)
    {
        return filepath;
    }
    else
    {
        char    *pfile = NULL;
        memset(pathname[pipe],0x0,512);
        pfile = strstr(filepath,".log");

        memcpy(pathname[pipe],filepath,(pfile-filepath));
        sprintf(pathname[pipe],"%s%02d",pathname[pipe],pipe);
        strcat(pathname[pipe],pfile);
        return pathname[pipe];
    }
}

/**
*********************************************************************
* @brief:        全局和锁的初始化
* @param[in]：   
* @param[in]：   
* @Date：        2020-06-7
* @return：      0 成功
*********************************************************************
*/
int global_var_init(void)
{
    uint8 i = 0;

    memset(&LockRecvBuf, 0x00, sizeof(LockQueue_T));
    memset(&LockSendBuf[0], 0x00, sizeof(LockQueue_T)*mdlnum);
    memset(&InfoDataQueue, 0x00, sizeof(InfoDataQueue_T));
    memset(&mdlChannelCfg[0], 0x00, sizeof(MDL_CHANNEL_CFG_T)*mdlnum);
    memset(&g_tDevPara[0], 0x00, sizeof(MDL_COMM_PARA));
    memset(&modPowerCtl, 0x00, sizeof(modPowerCtl));

    // 消息INDEX锁初始化
    if (pthread_mutex_init(&ActiveInfoIndexLk, NULL) != 0)
    {
        MDLMGR_FMT_DEBUG(mdlnum, "ActiveInfoIndexLk lock error!\n");
        return -1;
    }

    // mqtt接收缓冲区锁初始化
    if (pthread_mutex_init(&LockRecvBuf.QueueLock, NULL) != 0)
    {
        MDLMGR_FMT_DEBUG(mdlnum, "LockRecvBuf.QueueLock lock error!\n");
        return -2;
    }

    // mqtt发送缓冲区锁初始化
    for (i = 0; i < mdlnum; i++)
    {
        if (pthread_mutex_init(&LockSendBuf[i].QueueLock, NULL) != 0)
        {
            MDLMGR_FMT_DEBUG(mdlnum, "LockSendBuf[i].QueueLock lock error!\n");
            return -3;
        }
    }

    // 已处理mqtt消息帧信息锁初始化
    if (pthread_mutex_init(&InfoDataQueue.FrameLock, NULL) != 0)
    {
        MDLMGR_FMT_DEBUG(mdlnum, "InfoDataQueue.FrameLock lock error!\n");
        return -4;
    }

    if (pthread_mutex_init(&closeLock, NULL) != 0)
    {
        MDLMGR_FMT_DEBUG(mdlnum, "closeLock lock error!\n");
        return -5;
    }

    if (pthread_mutex_init(&g_MsgSendlock, NULL) != 0)
    {
        MDLMGR_FMT_DEBUG(mdlnum, "g_MsgSendlock lock error!\n");
        return -6;
    }

    return 0;
}

/**
*********************************************************************
* @brief:        同步更新模组通道配置其他信息
* @param[in]：   通道号
* @param[in]：   
* @return：
*********************************************************************
*/
int32 mdl_channel_scan(void)
{
    uint32 i = 0;
    FILE*  fp = NULL;
    char   cmd[128] = {0};
    int32  lastUsbHubNo = -1;
    int32  usbHubNo = -1;
    char   buf[512] = {0};
    char   *str = NULL;

    sprintf(cmd, "find /sys/devices/ -name ttyACM*");

    //如果没有扫描到，就尝试多次
    for (i = 0; i < 10; i++)
    {
        fp = popen(cmd, "r"); 
        if (NULL == fp)   
        {   
            writePlclog(MDLSTARTLOG, "mdl_channel_scan:popen error\n");  
            usbHubNo = -1;
            return usbHubNo;   
        }

        while (fgets(buf, sizeof(buf), fp) != NULL)
        {
            writePlclog(MDLSTARTLOG, "scan channal buf:%s\n", buf);
            if ((str = strstr(buf, "/usb")) != NULL)
            {
                usbHubNo = str[4] - 0x30;
                if ((usbHubNo > 9) || (usbHubNo < 0))
                {
                    continue;
                }
                else
                {
                    if (lastUsbHubNo != usbHubNo)
                    {
                        writePlclog(MDLSTARTLOG, "mdl_channel_scan:lastUsbHubNo[%d] != usbHubNo[%d]\n", lastUsbHubNo, usbHubNo);
                    }
                    lastUsbHubNo = usbHubNo;
                }
            }
        }

        fclose(fp);

        if ((usbHubNo >= 0) && (usbHubNo <= 9))
        {
            break;
        }
        else
        {
            usbHubNo = -1;
            lastUsbHubNo = -1;
        }
    }

    writePlclog(MDLSTARTLOG, "mdl_channel_scan:find usbHubNo[%d]\n", usbHubNo);

    return usbHubNo;
}

/**
*********************************************************************
* @brief:        同步更新模组通道配置其他信息
* @param[in]：   通道号
* @param[in]：   
* @return：
*********************************************************************
*/
void mdl_channel_info_update(int32 i)
{
    if (strlen(mdlChannelCfg[i].channelName) == 0)
    {
        writePlclog(MDLSTARTLOG, "mdlChannelCfg[%d].channelName null\n", i);
        return;
    }

    sprintf(mdlChannelCfg[i].channelPath, "/sys/bus/usb/devices/");
    sprintf(mdlChannelCfg[i].channelInfo, "%s", &mdlChannelCfg[i].channelName[21]); 

    writePlclog(MDLSTARTLOG, "mdlChannelCfg[%d].channelName:%s\n", i, mdlChannelCfg[i].channelName);
    writePlclog(MDLSTARTLOG, "mdlChannelCfg[%d].channelPath:%s\n", i, mdlChannelCfg[i].channelPath);
    writePlclog(MDLSTARTLOG, "mdlChannelCfg[%d].channelInfo:%s\n", i, mdlChannelCfg[i].channelInfo);

    return;
}

/**
*********************************************************************
* @brief:        配置文件初始化
* @param[in]：   
* @param[in]：   
* @Date：        2020-06-7
* @return：      0 成功
*********************************************************************
*/
int init_config(uint32 *ip, uint16 *port)
{
    dictionary *ini = NULL;
    uint32 ipaddr = 0;
    char  *stringvalue = NULL;
    int    intvalue = NULL;
    char   tmpChannelStr[64] = {0};
    char   tmpDefNameStr[64] = {0};
    char   curpath[256] = {0};
    char   filename[256] = {0};
    int32  usbHubNo = -1;
    int    i = 0;

    //获取当前目录绝对路径，即去掉程序名
    int cnt = readlink("/proc/self/exe", curpath, 256);
    if(cnt < 0 || cnt >= 256)
    {
        printf("mdlmgr get err path!\n");
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
    sprintf(filename, "%s%s", curpath, MDLMGR_INI_FILE);
    printf("配置文件filename: %s!\n", filename);

    ini = iniparser_load(filename);
    if(NULL != ini)
    {
        /* 配置文件内容获取 */
        stringvalue = iniparser_getstring(ini, "MQTT_CONF:IP", "127.0.0.1");
        ipaddr = inet_addr(stringvalue);
        *ip = ntohl(ipaddr);

        /* 端口获取 */
        intvalue = iniparser_getint(ini, "MQTT_CONF:PORT", 1883);
        *port = (uint16)intvalue;

        stringvalue = iniparser_getstring(ini, "FILEPATH_CONF:LOGPATH", "/data/app/mapManager/log");
        char   exceptpath[256] = {0};
        sprintf(exceptpath, "%s/Exception.log", stringvalue);
        change_sig_file(exceptpath);

        /* 模组配置信息 */
        for (i = 0; i < mdlnum; i++)
        {
            sprintf(tmpChannelStr, "USB_CONF:CHANNEL%d", i + 1);
            stringvalue = iniparser_getstring(ini, tmpChannelStr, "NULL");
            if (strlen(stringvalue) > 255)
            {
                MDLMGR_FMT_DEBUG(mdlnum, "the channelName of mdlChannelCfg[%d] is too long\n", i);
            }
            else if (strstr(stringvalue, "NULL") != NULL)
            {
                usbHubNo = mdl_channel_scan();
                if (usbHubNo != -1)
                {
                    sprintf(tmpDefNameStr, "/sys/bus/usb/devices/%d-1.%d", usbHubNo, i + 1);
                }
                else
                {
                    sprintf(tmpDefNameStr, "/sys/bus/usb/devices/2-1.%d", i + 1);
                }
                sprintf(mdlChannelCfg[i].channelName, "%s", tmpDefNameStr);
                mdl_channel_info_update(i);
            }
            else
            {
                sprintf(mdlChannelCfg[i].channelName, "%s", stringvalue);
                mdl_channel_info_update(i);
            }
        }

        /* 是否打开蜂鸣器 */
        gBuzzerEn = iniparser_getint(ini, "BUZZER_CONF:ENABLE", 0);
        INIT_FMT_DEBUG("读蜂鸣器使能标志[%d]\n", gBuzzerEn);

        iniparser_freedict(ini);
    }
    else
    {
        printf("Get no ini file: %s!\n", MDLMGR_INI_FILE);
    }

    /* 日志模块初始化 */
    diag_init(filename, TRUE);

    return 0;
}

/***********************************************************************
 * @brief   是否此设备的USB端口信息
 * @param[in] usbNo 槽位序号
 *
 * @retval    BOOL
 **********************************************************************/
BOOL NgtMsgIsClear(int32 usbNo)
{
    BOOL ret = FALSE;

    if (slotInfo[usbNo].modeTypeLen == 0 || slotInfo[usbNo].modeTypeLen == 0xff)
    {
        ret = FALSE;
    }
    else
    {
        ret = TRUE;
    }
    
    return ret;
}

/*******************************************************************************
* 函数名称:         MdlCommFunDeal
* 函数功能:         串口收发函数
* 输入参数:         fd - 串口设备符
                   SendAppData - 发送内容
                   sendfn - 发送fn
* 输出参数:         NULL
* 返 回 值:          > 0 接收成功
                    < 0 接收失败

*******************************************************************************/
int MdlCommFunDeal(int32 usbNo,TRANS_INFO_T SendAppData,  TRANS_INFO_T *pRecvAppData)
{
    uint8                       RetryCnt = 0;                       //接收尝试次数
    uint8                       SendBuf[1024];
    int                         RecvLen;                            //接收数据长度
    uint8                       RecvBuf[1024];                      //接收数据缓冲区
    static time_t               CommBeginTime;                      //通讯开始时间
    MDL_COMM_STATE_T            CommStat = MDL_COMM_IDLE;
    int                         RecvResult = 1;                     //接收结果(1:成功 / -1:失败)
    uint16                      len;
    MDL_FRAME_INFO_T            recvModeData;
    int                         Ret = -1;
    uint8                       send_fail_cont = 0;

    while(1)
    {
        switch(CommStat)
        {
        case MDL_COMM_IDLE:
            //ClearCommRecvBuff();                    //清接收数据
            CommStat = MDL_COMM_SEND;               //切换到发送
            break;

        case MDL_COMM_SEND:
            len = MakeFrame(usbNo, &SendAppData, &SendBuf[0]);
            Ret = SendData(usbNo, &SendBuf[0], len);
            MDLUP_BUF_DEBUG(SendBuf, len, "升级发送帧ret[%d]:", Ret);
            if (Ret == 0)
            {
                CommBeginTime = time(NULL);
                CommStat = MDL_COMM_RECV;           //切换到接收
            }
            else
            {
                send_fail_cont ++;
            }

            if (send_fail_cont > 10)
            {
                CommStat = MDL_COMM_IDLE;
                return -1;
            }
            
            break;

        case MDL_COMM_RECV:
            RecvLen = RecvData(usbNo, &RecvBuf[0], sizeof(RecvBuf));
            MDLUP_BUF_DEBUG(RecvBuf, RecvLen, "升级接收帧:");  
            if(RecvLen > 0)
            {   
                memset(&recvModeData,0x00,sizeof(MDL_FRAME_INFO_T));
                recvModeData.dataLen = RecvLen;
                memcpy(recvModeData.pDataBuf,&RecvBuf[0],RecvLen);
                Ret = AnalysisFrameOfModule(usbNo, &recvModeData,(uint8*)pRecvAppData->setData,1);
                if ( Ret >= 0)
                {
                    return Ret;//接收成功
                }              
                else
                {
                    MDLUP_FMT_DEBUG("AnalysisFrameOfModule err: Ret = %d\n",Ret);
                    RecvResult = -1;//接收失败
                }
            }
            else
                RecvResult = -1;//接收失败

            if(RecvResult == -1)
            {
                if(time(NULL) - CommBeginTime > MAX_MDL_COMM_DELAY_TIME)
                {
                    RetryCnt++;
                    CommStat = MDL_COMM_RETRY;
                }
            }

            break;

        case MDL_COMM_RETRY:
            if(RetryCnt > 3)
            {
                CommStat = MDL_COMM_IDLE;
                return -2;
            }

            CommStat = MDL_COMM_SEND;
            break;

        default:
            break;

        }

        usleep(200000);
    }

    return 0;
}

/*******************************************************************************
* 函数名称: MakeFrameUpdateModule
* 函数功能: 模组模块升级组帧，并且发送到缓冲区
* 输入参数: fileBuf - 文件内容
           Len - 内容长度
           Uis - 文件升级信息
* 输出参数: sendbuf - 发送报文
* 返 回 值: >0                  发送长度
*           <0                  失败
*******************************************************************************/
uint16 MakeFrameUpdateModule(int32 usbNo, uint8 *sendbuf, uint8 *fileBuf, uint16 Len, MdlUpdateInfoState Uis)
{
    int32 send_len;
    //static uint8 fid = 0;
    //uint8 buf[1024] = {0};

    TRANS_INFO_T srcframeInfo;
    UPDATE_INFO_T updateInfo;

    srcframeInfo.DTA = 0x02;
    srcframeInfo.DTB = 0x03;
    srcframeInfo.seq = update_seq++;
    srcframeInfo.clkSetFlag = 1;

    updateInfo.fileID = 0x00; //模块文件
    //buf[1] = (uint8)(Uis.CurrentPackageNo & 0x00ff);  //段序号，是不是要+1？
    //buf[2] = (uint8)((Uis.CurrentPackageNo >> 8) & 0x00ff);
    memcpy(&updateInfo.packNo ,&Uis.CurrentPackageNo,4);
    memcpy(&updateInfo.packCnt,&Uis.TotalPackageNum,4);
    updateInfo.packDataLen = Len;
    //memcpy(&buf[11], fileBuf, Len);
    memcpy(updateInfo.packData,fileBuf,Len);
    srcframeInfo.dataLen = sizeof(UPDATE_INFO_T);
    memcpy(srcframeInfo.setData,&updateInfo,srcframeInfo.dataLen);

    // fid++;
    // if (fid > 0x3F) //帧序号占6个位
    // {
    //  fid = 0;
    // }

    // 数据区地址
    send_len = MakeFrame(usbNo, &srcframeInfo,sendbuf);

    return send_len;
}

/*******************************************************************************
* 函数名称: GetMdlUpdateInfo
* 函数功能: 对升级文件进行预读，取得UpdateInfoState结构体信息
* 输入参数: Path - 文件路径
           PackageLen - 每包的长度
* 输出参数: uifo - 升级信息
* 返 回 值: >0                  成功
*          <0                  失败
*******************************************************************************/
int GetMdlUpdateInfo(const char *Path, MdlUpdateInfoState *uifo, uint16 PackageLen)
{
    int fd = -1;
    int size = 0;
    uint8 buffer[1024];
    uint16 package_len = PackageLen;
    uint32 package_no = 0;

    memset(buffer,0x00,sizeof(buffer));
    fd=open(Path,O_RDONLY);
    if(fd <= 0)
    {
        MDLUP_FMT_DEBUG( "GetUpdateInfo open file error!\n");
        return -1;
    }


    while(size >= 0)
    {
        size = read(fd,buffer,package_len);

        if(0 == size)
        {
            break;
        }

        package_no++;
        memset(buffer,0x00,sizeof(buffer));

    }

    uifo->TotalPackageNum = package_no;
    uifo->CurrentPackageNo = 0;
    uifo->PackageLen = package_len;

    package_no=0;
    memset(buffer,0x00,sizeof(buffer));
    close(fd);

    return 1;
}

/*******************************************************************************
* 函数名称: mdlIsIncludeName
* 函数功能: 查找str1种是不是包含str2
* 输入参数: 无
* 输出参数: 无
* 返 回 值: 
*******************************************************************************/
BOOL mdlIsIncludeName(char *str1, char *str2, uint16 len)
{
    uint16 i = 0;
    BOOL ret = FALSE;
    for (i = 0; i < (len - strlen(str2)); i++)
    {
        if (memcmp(&str1[i], &str2[0], strlen(str2)) == 0)
        {
            ret = TRUE;
            break;
        }
    }
    if (i >= len - strlen(str2))
    {
        ret = FALSE;
    }

    return ret;
}

/**
*********************************************************************
* @brief：     判断是否存在升级文件
* @param[in]： 
* @param[out]：usbNo          - 槽位序号
               UpdateFileName - 获取升级文件名称
* @return：    TRUE           - 有
               FALSE          - 没有
*********************************************************************
*/
BOOL IsExistUpdateMDL(char *UpdateFileName, int32 *usbNo)
{
    static uint8 delay = 0;
    char TmpPath[256] = {0};
    char fileName[256] = {0};
    char fileNameBack[256] = {0};
    int i;

    if (UpdateFileName == NULL)
    {
        MDLUP_FMT_DEBUG("IsExistUpdateMDL err: UpdateFileName[%p]\n", UpdateFileName);
        return FALSE;
    }
    
    for(i = 0; i < mdlnum; i++)
    {
        memset(fileName, 0x0, sizeof(fileName));
        sprintf(fileName, "%s_%d", MDL_UPDATE_FILE, i);

        if(access(fileName, R_OK) == 0)    //先判断主节点升级包
        {
            delay++;
            if(delay < 5)
            {
                MDLUP_FMT_DEBUG( "发现槽位%d上的升级文件，延时5秒 delay = %d\n", i, delay);
                return FALSE;
            }
            else
            {
                delay = 0;
            }
            
            memset(fileNameBack, 0x0, sizeof(fileNameBack));
            sprintf(fileNameBack, "%s_%d", MDL_UPDATE_FILE_BACK, i);

            //如果存在 MDL_CTL_UPDATE_FILE_BACK 文件先将其删除
            if (access(fileNameBack, R_OK) == 0)
            {
                sprintf(TmpPath, "rm -rf %s", fileNameBack);
                system(TmpPath);
                MDLUP_FMT_DEBUG("删除备份%s\n", TmpPath);
            }

            if (rename(fileName, fileNameBack) < 0)
            {
                MDLUP_FMT_DEBUG("rename err\n");
                return FALSE;
            }

            *usbNo = i;
            memcpy(UpdateFileName, fileNameBack, strlen(fileNameBack) + 1);
            MDLUP_FMT_DEBUG("存在升级文件[%s], 槽位[%d], 设备类型[%s], 准备升级\n", UpdateFileName, i, slotInfo[i].modeType);
            return TRUE;
        }
    }

    return FALSE;
}

/*******************************************************************************
* 函数名称: IsNeedGetVerByNo()
* 函数功能: 根据设备判断是否升级
* 输入参数: 
* 输出参数: 槽位序号
* 返 回 值: 0  无升级文件
*          >0 升级文件类型
*******************************************************************************/
BOOL IsNeedGetVerByNo(int32 *usbNo)
{
    BOOL RET = FALSE;
    uint8 i = 0;

    for(i = 0; i < mdlnum; i++)
    {
        if ((updateAllInfo.mdl_up_info[i].softVersion == 0xffffffff) && 
            (updateAllInfo.mdl_up_info[i].softVer[0] == 0x00) && 
            (updateAllInfo.mdl_up_info[i].softVer[1] == 0x00))
        {
            MDLUP_FMT_DEBUG("存在需要获取软件版本的模组，槽位[%d]\n", i + 1);
            break;
        }
    }

    if (i < mdlnum)
    {
        *usbNo = i;
        RET = TRUE;
    }
    else
    {
        RET = FALSE;
    }

    return RET;
}

/*******************************************************************************
* 函数名称: mdlCheckFileStatus
* 函数功能: 检测文件传输状态
* 输入参数: 文件状态报文
* 输出参数: 无
* 返 回 值: 0  升级失败
*          1   升级成功
*******************************************************************************/
BOOL mdlCheckFileStatus(uint8 *fileStatus)
{
    uint32 recvPackageSum = 0;
    uint16 lenByte = 0;
    uint8  lenBit = 0;
    uint8 i = 0;
    //uint8 j = 0;
    BOOL ret;
    if (fileStatus != NULL)
    {
        //recvPackageSum = (fileStatus[0] & 0x00ff) + ((fileStatus[1] >> 8) & 0x00ff);
        memcpy_r(&recvPackageSum, fileStatus, 4);
        if (recvPackageSum > 0)
        {
            lenByte = recvPackageSum/8;
            lenBit = recvPackageSum%8;
            for (i = 0; i < lenByte; i++)
            {
                if (fileStatus[4 + i] != 0xff)
                {
                    ret = FALSE;
                    MDLUP_FMT_TRACE("检测文件传输状态错误 error1, status = %x\n", fileStatus[4 + i]);
                    break;
                }
            }
            if (i == lenByte)
            {
                if ((fileStatus[4 + lenByte] >> (8-lenBit)) != (uint8)(0xff >> (8 - lenBit)))
                {
                    MDLUP_FMT_TRACE("检测文件传输状态错误 error2, status = %x\n", fileStatus[4 + lenByte]);
                    ret = FALSE;
                }
                else
                {
                    MDLUP_FMT_TRACE("检测文件传输状态成功\n");
                    ret = TRUE;
                }
            }
        }
        else
        {
            MDLUP_FMT_TRACE("检测文件传输状态错误 error3, recvPackageSum = %d\n", recvPackageSum);
            ret = FALSE;
        }
    }
    else
    {
        MDLUP_FMT_TRACE("检测文件传输状态错误 error4, fileStatus == NULL\n");
        ret = FALSE;
    }

    return ret;
}

#if MDL_RECOGNITION_EN == 1
/*******************************************************************************
* 函数名称: is_negotiate_ok
* 函数功能: 是否协商成功 
* 输入参数: usbNo 槽位序号
* 输出参数:  
* 返 回 值:
*******************************************************************************/
int32 is_negotiate_ok(int32 usbNo)
{
    int32 ret = RET_ERR;
    SLOT_INFO_T *mdl;

    mdl = &slotInfo[usbNo];

    if ((mdl->modeTypeLen != 0) && (mdl->modeTypeLen != 0xff))
    {
        ret = RET_OK;
    }

    if (ret < RET_OK)
    {
        MDLUP_FMT_DEBUG( "is_negotiate_err\n");
    }

    return ret;
}
#endif

/*******************************************************************************
* 函数名称: mdl_update_thread
* 函数功能: 模组升级线程
* 输入参数: 无
* 输出参数: 无
* 返 回 值: 
*******************************************************************************/
void* mdl_update_thread(void *arg)
{
    BOOL isUpfileExist = FALSE;
    BOOL isGetVer = FALSE;
    int32 ret = -1;
    MdlUpdateInfoState Uis;          //更新文件结构体
    char  UpdateFileName[100] = {0}; //升级文件名
    int32  Fd = -1;                  //文件标识符
    int32  Size = 0;                 //每次从文件中读出的大小
    uint8  Buffer[1024];             //从文件读入的缓冲区
    int32  CommRetryCnt = 0;         //通讯重试次数
    time_t CommBeginTime;            //通讯开始时间
    char   str[100] = {0};
    int32  RecvResult = 1; 
    int32  TotalRetryCnt = 0;        //整体重试次数统计
    int32 recvLen = 0;
    int32 sendLen = 0;
    uint8 recvBuf[1024];
    uint8 sendBuf[1024];

    TRANS_INFO_T SendAppData;                       //模组帧中间结构体
    TRANS_INFO_T RecvAppData;

    MDL_FRAME_INFO_T recvModuleData;
    
    //FILE_TRANS_STATE_T    fileTransState;             //文件传输状态
    
    uint8   softVer[MAX_CHAR_LEN] = {0};
    
    uint8 recv_fail_cont = 0;
    uint8 send_fail_cont = 0;
    uint8 init_fail_cont = 0;

    uint8 cntGetVer = 0;
    MDL_UPDATE_STEP_E updateStep = UPDATE_IDLE;

    MODULE_INF_T msg;

    char TmpPath[256] = {0};

    uint32 delayms = 20000;

    g_UpdateUsbNo = -1;

    INIT_FMT_DEBUG("模组升级线程启动\n");
    
    while (1)
    {
        mdl_update_thread_cnt = 0;
        
        isUpfileExist = IsExistUpdateMDL(UpdateFileName, &g_UpdateUsbNo);
        if (!isUpfileExist)
        {
            MdlUpdateDeal = MDL_UPDATE_IDLE;
            usleep(1000000);

            if (IsNeedGetVerByNo(&g_UpdateUsbNo))
            {
                if (NgtMsgIsClear(g_UpdateUsbNo))               
                {
                    MDLUP_FMT_DEBUG("产生模组获取版本任务\n");
                    MdlUpdateDeal = MDL_UPDATE_INIT;
                    isGetVer = TRUE;
                }
            }
            if ((!isGetVer) || (MDL_UPDATE_IDLE == MdlUpdateDeal))
            {
                isGetVer = FALSE;
                memset(UpdateFileName, 0, sizeof(UpdateFileName));
                g_UpdateUsbNo = -1;
                continue;
            }
        }
        else
        {
            if (!NgtMsgIsClear(g_UpdateUsbNo))
            {
                usleep(1000000);
                memset(UpdateFileName, 0, sizeof(UpdateFileName));
                g_UpdateUsbNo = -1;
                continue;
            }
            MDLUP_FMT_DEBUG("产生模组升级任务\n");
            sprintf(str, "正在对槽位%d进行升级", g_UpdateUsbNo + 1);
            refresh_status_bar(0, str);
            MdlUpdateDeal = MDL_UPDATE_INIT;
        }

        while (1)
        {
            mdl_update_thread_cnt = 0;
            delayms = 20000;
            
            switch (MdlUpdateDeal)
            {
                case MDL_UPDATE_IDLE:
                    break;

                case MDL_UPDATE_INIT:
                    if(g_UpdateUsbNo == -1)
                    {
                        MDLUP_FMT_DEBUG("升级槽位号错误\n");
                        MdlUpdateDeal = MDL_UPDATE_IDLE;                   
                        break;
                    }
                    cntGetVer = 0;
                    if (is_negotiate_ok(g_UpdateUsbNo) < 0)
                    {
                        MDLUP_FMT_DEBUG("槽位[%d]没有协商成功\n", g_UpdateUsbNo);
                        MdlUpdateDeal = MDL_UPDATE_IDLE;                   
                        break;
                    }
                    
                    if (gCtrlDev[g_UpdateUsbNo] < 0)
                    {
                        gCtrlDev[g_UpdateUsbNo] = module_ctrl_comm_init(g_UpdateUsbNo);   /*打开串口*/
                    }

                    MDLUP_FMT_DEBUG("槽位[%d]模组升级 设备名称[%s] 设备状态[%d]\n", g_UpdateUsbNo + 1, (char *)g_tDevPara[g_UpdateUsbNo].deviceName, 
                        gCtrlDev[g_UpdateUsbNo]);

                    if (gCtrlDev[g_UpdateUsbNo] < 0)                         
                    {
                        init_fail_cont++;
                        MdlUpdateDeal = MDL_UPDATE_INIT;  
                        usleep(100000); 
                        if (init_fail_cont > 3) //初始化3次失败
                        {
                            init_fail_cont = 0;
                            MdlUpdateDeal = MDL_UPDATE_AGAIN;
                            MDLUP_FMT_DEBUG("初始化失败超过3次 重新升级\n");
                        }
                        break;
                    }

                    if ((!isUpfileExist) && (isGetVer))
                    {
                        MdlUpdateDeal = MDL_UPDATE_GET_VER; 
                        updateStep = UPDATE_IDLE;
                        break;
                    }

                    updateStep = UPDATE_START;
                    MdlUpdateDeal = MDL_UPDATE_READ_FILE_DELAY;
                    sleep(1);
                    break;

                case MDL_UPDATE_READ_FILE_DELAY: //升级文件预处理
                    ret = GetMdlUpdateInfo(UpdateFileName, &Uis, UPDATE_PACKAGE_LEN);
                    if(ret < 0)
                    {
                        MdlUpdateDeal = MDL_UPDATE_AGAIN;
                        MDLUP_FMT_DEBUG( "预处理错误，错误码[%d]\n", ret);
                        break;
                    }

                    MDLUP_FMT_DEBUG( "预处理成功 总包数[%d] 每包大小[%d]\n", Uis.TotalPackageNum, UPDATE_PACKAGE_LEN);
                    MdlUpdateDeal = MDL_UPDATE_OPEN_FILE;
                    break;

                case MDL_UPDATE_OPEN_FILE:
                    Fd = open(UpdateFileName, O_RDONLY);
                    if(Fd <= 0)
                    {
                        MDLUP_FMT_DEBUG("升级文件打开失败 错误码[%d]\n", Fd);
                        MdlUpdateDeal = MDL_UPDATE_AGAIN;
                        break;
                    }
                    MdlUpdateDeal = MDL_UPDATE_READ_FILE;
                    break;

                case MDL_UPDATE_READ_FILE:
                    memset(Buffer, 0x00, sizeof(Buffer));
                    Size = read(Fd, Buffer, UPDATE_PACKAGE_LEN);
                    if(Size < 0)
                    {
                        MDLUP_FMT_DEBUG("读升级文件失败, 返回值[%d]\n",Size);
                        MdlUpdateDeal = MDL_UPDATE_AGAIN;
                    }
                    else
                    {
                        MDLUP_FMT_DEBUG( "升级报文序号:%d, 总升级报文:%d 每升级报文长度:%d\n", Uis.CurrentPackageNo, Uis.TotalPackageNum, Size);
                        MdlUpdateDeal = MDL_UPDATE_SEND;
                    }

                    //在新读出一帧时，将重试次数置0
                    CommRetryCnt = 0;
                    break;

                case MDL_UPDATE_SEND:
                    sendLen = MakeFrameUpdateModule(g_UpdateUsbNo, sendBuf, Buffer, Size, Uis);
                    //刷新发送时间
                    CommBeginTime = time(NULL);
                    
                    ret = SendData(g_UpdateUsbNo, sendBuf, sendLen);
                    if (ret < 0)
                    {
                        MDLUP_FMT_DEBUG("发送报文失败 错误码[%d]\n", ret);
                        send_fail_cont++;
                        MdlUpdateDeal = MDL_UPDATE_SEND; //继续发送
                        if (send_fail_cont > 3)
                        {
                            send_fail_cont = 0; //发送失败超过3次
                            MdlUpdateDeal = MDL_UPDATE_AGAIN;
                            MDLUP_FMT_DEBUG( "发送失败超过3次 重试\n");
                        }
                    }

                    MDLUP_BUF_DEBUG(sendBuf, sendLen, "升级发送帧:");

                    //数据发送完成，等待接收
                    MdlUpdateDeal = MDL_UPDATE_RECV;
                    break;

                case MDL_UPDATE_RECV:
                    RecvResult = 1;
                    recvLen = RecvData(g_UpdateUsbNo, recvBuf, sizeof(recvBuf));
                    MDLUP_BUF_DEBUG(recvBuf, recvLen, "升级接受帧:");
                    if (recvLen < 0)
                    {
                        recv_fail_cont++;
                        MdlUpdateDeal = MDL_UPDATE_RECV; //继续接收
                        if (recv_fail_cont > 3)
                        {
                            recv_fail_cont = 0;//接收失败超过3此
                            MdlUpdateDeal = MDL_UPDATE_AGAIN;
                        }
                    }
                    else if (recvLen > 0)
                    {
                        memset(&recvModuleData,0x00,sizeof(MDL_FRAME_INFO_T));
                        memcpy(&recvModuleData.pDataBuf[0],recvBuf,recvLen);
                        recvModuleData.dataLen = recvLen;
                        ret = AnalysisFrameOfModule(g_UpdateUsbNo, &recvModuleData,NULL,1);
                        if (ret !=  ACTSUCCESS)
                        {
                            RecvResult = -1;
                            MDLUP_FMT_DEBUG( "升级报文解释错误 错误码[%d]\n", ret);
                        }

                        if(RecvResult != -1)
                        {
                            Uis.CurrentPackageNo++;
                            MdlUpdateDeal = MDL_UPDATE_READ_FILE;

                            //收到确认后，线程不延时
                            delayms = 0;

                            if(Uis.CurrentPackageNo >= Uis.TotalPackageNum)
                            {
                                MDLUP_FMT_DEBUG( "文件传输结束 当前报文序号[%d] 总报文数[%d] 准备查询升级状态\n",
                                    Uis.CurrentPackageNo,Uis.TotalPackageNum);
                                MdlUpdateDeal = MDL_UPDATE_GET_RUN_STATE;
                            }
                            break;
                        }
                    }
                    else
                    {
                        if ((time(NULL) - CommBeginTime) > MAX_UPDATE_DELAY_TIME)
                        {
                            RecvResult = -1;
                        }
                        else
                        {
                            MdlUpdateDeal = MDL_UPDATE_RECV; //继续接收
                        }

                    }

                    if (RecvResult == -1)
                    {
                        CommRetryCnt++; //重试次数+1

                        MdlUpdateDeal = MDL_UPDATE_RETRY; //切换到重试状态
                    }

                    break;

                case MDL_UPDATE_RETRY:
                    if (CommRetryCnt < 5)
                    {
                        MdlUpdateDeal = MDL_UPDATE_SEND;
                    }
                    else
                    {
                        CommRetryCnt = 0;
                        MdlUpdateDeal = MDL_UPDATE_AGAIN;
                    }
                    break;

                case MDL_UPDATE_AGAIN:
                    close(Fd);
                    Fd = -1;
                    module_ctrl_comm_uninit(g_UpdateUsbNo);
                    gCtrlDev[g_UpdateUsbNo] = -1;
                    Size = 0;
                    CommRetryCnt = 0;
                    recv_fail_cont = 0;
                    send_fail_cont = 0;
                    init_fail_cont = 0;
                    updateStep = UPDATE_IDLE;
                    cntGetVer = 0;
                    recvLen = 0;
                    sendLen = 0;
                    memset(Buffer,0x00,sizeof(Buffer));
                    memset(sendBuf,0x00,sizeof(Buffer));
                    memset(recvBuf,0x00,sizeof(Buffer));

                    memset((uint8 *)&Uis,0x00,sizeof(Uis));
                    TotalRetryCnt++;
                    if(TotalRetryCnt <= 3)
                    {
                        MDLUP_FMT_DEBUG( "重试，重新初始化\n");
                        MdlUpdateDeal = MDL_UPDATE_INIT;
                    }
                    else
                    {
                        TotalRetryCnt = 0;
                        MDLUP_FMT_DEBUG( "重启次数达到上限，升级结束\n");
                        MdlUpdateDeal = MDL_UPDATE_END;
                    }
                    break;

                case MDL_UPDATE_GET_RUN_STATE:
                    memset(&SendAppData,0x00,sizeof(TRANS_INFO_T));
                    SendAppData.DTA = 0x00;
                    SendAppData.DTB = 0x04;             //查询文件传输状态
                    SendAppData.seq = globleSeq++;
                    
                    ret = MdlCommFunDeal(g_UpdateUsbNo, SendAppData, &RecvAppData);
                    if (ret == 1)
                    {
                        if (mdlCheckFileStatus(RecvAppData.setData))                        
                        {
                            MdlUpdateDeal = MDL_UPDATE_REBOOT_DEV;
                            MDLUP_FMT_DEBUG( "文件传输状态正常，准备重启\n");
                        }
                        else
                        {
                            MdlUpdateDeal = MDL_UPDATE_AGAIN;
                        }   
                    }
                    else
                    {
                        MDLUP_FMT_DEBUG("查询文件传输状态失败\n");
                        MdlUpdateDeal = MDL_UPDATE_REBOOT_DEV;
                    }
                    break;

                case MDL_UPDATE_REBOOT_DEV:
                    memset(&SendAppData,0x00,sizeof(TRANS_INFO_T));
                    SendAppData.DTA = 0x02;
                    SendAppData.DTB = 0x00;             //模组重启
                    SendAppData.seq = globleSeq++;

                    ret = MdlCommFunDeal(g_UpdateUsbNo, SendAppData, &RecvAppData);
                    if (ret == ACTSUCCESS)
                    {
                        if (RecvAppData.setData[0] == MOD_CTR_OK)
                        {
                            MDLUP_FMT_DEBUG( "模组升级后重启成功，准备获取版本号\n");
                            MdlUpdateDeal = MDL_UPDATE_GET_VER;
                            updateStep = UPDATE_END;
                        }
                        else
                        {
                            MDLUP_FMT_DEBUG( "模组升级后重启成功，result = %d\n", RecvAppData.setData[0]);
                        }

                    }
                    else
                    {
                        MDLUP_FMT_DEBUG( "模组升级后重启失败，ret = %d\n", ret);
                        MdlUpdateDeal = MDL_UPDATE_GET_VER;
                        updateStep = UPDATE_END;
                    }
                    break;

                case MDL_UPDATE_GET_VER:
                    if (updateStep == UPDATE_END)
                    {
                        if (gCtrlDev[g_UpdateUsbNo] == 0)
                        {
                            module_ctrl_comm_uninit(g_UpdateUsbNo);
                            gCtrlDev[g_UpdateUsbNo] = -1;

                            MDLUP_FMT_DEBUG( "关闭%s 设备状态[%d]\n", (char *)g_tDevPara[g_UpdateUsbNo].deviceName, gCtrlDev[g_UpdateUsbNo]);
                        }
                        cntGetVer++;
                        if (cntGetVer < 15)
                        {
                            MDLUP_FMT_DEBUG( "准备获取模组信息, 等待 %d\n", cntGetVer);
                            sleep(1);
                            break;
                        }
                        else
                        {
                            if (gCtrlDev[g_UpdateUsbNo] < 0)
                            {
                                gCtrlDev[g_UpdateUsbNo] = module_ctrl_comm_init(g_UpdateUsbNo);   /*打开串口*/
                                MDLUP_FMT_DEBUG("重新打开%s 设备状态[%d]\n", (char *)g_tDevPara[g_UpdateUsbNo].deviceName, gCtrlDev[g_UpdateUsbNo]);
                            }
                            cntGetVer = 0;
                        }
                    }

                    MDLUP_FMT_DEBUG( "发送获取模组信息报文\n");
                    memset(&SendAppData,0x00,sizeof(TRANS_INFO_T));
                    SendAppData.DTA = 0x00;
                    SendAppData.DTB = 0x00;             //查询模块信息
                    SendAppData.seq = globleSeq++;

                    ret = MdlCommFunDeal(g_UpdateUsbNo ,SendAppData, &RecvAppData);
                    if ((ret == 1) /*&& (RecvAppData.dataLen >=  sizeof(MODULE_INF_T))*/)
                    {   
                        memcpy(&msg,RecvAppData.setData,sizeof(MODULE_INF_T));
                        MODULE_INF_T *pModuleInfoNew = NULL;
                        memcpy(&updateAllInfo.mdl_up_info[g_UpdateUsbNo], RecvAppData.setData,sizeof(MODULE_INF_T));
                        pModuleInfoNew = &updateAllInfo.mdl_up_info[g_UpdateUsbNo]; 

                        //模组协议兼容
                        if (pModuleInfoNew->type == MDL_PRTL_VER1)
                        {
                            MDLUP_FMT_DEBUG( "设备型号：%s 设备ID：%s softVersion:%s hardwVersion:%s 厂商编码：%s\n",
                                pModuleInfoNew->DevType,pModuleInfoNew->DevID,pModuleInfoNew->softVer,
                                pModuleInfoNew->hardVer,pModuleInfoNew->VendorCode);

                            strcpy((char *)softVer, msg.softVer);
                    
                            if (updateStep == UPDATE_START)
                            {
                                MDLUP_FMT_DEBUG( "%s开始升级, 此时版本1[%s]\n", UpdateFileName, softVer);
                                MdlUpdateDeal = MDL_UPDATE_READ_FILE_DELAY;

                            }
                            else if(updateStep == UPDATE_END)
                            {
                                MDLUP_FMT_DEBUG( "%s结束升级, 此时版本2[%s]\n", UpdateFileName, softVer);
                                MDLUP_FMT_TRACE("模组升级成功\n");
                                MdlUpdateDeal = MDL_UPDATE_END;
                            }
                            else if((!isUpfileExist) && (isGetVer))
                            {
                                MDLUP_FMT_DEBUG( "版本[%s]\n", softVer);
                                module_ctrl_comm_uninit(g_UpdateUsbNo);
                                gCtrlDev[g_UpdateUsbNo] = -1;
                                MdlUpdateDeal = MDL_UPDATE_IDLE;
                            }
                            else
                            {
                                MDLUP_FMT_DEBUG("整体重试 步骤码[%d]\n", updateStep);
                                MdlUpdateDeal = MDL_UPDATE_AGAIN;
                            }  
                        }
                        else
                        {
                            MDLUP_FMT_DEBUG( "设备型号：%s 设备ID：%s softVersion:%08x hardwVersion:%08x 厂商编码：%s\n",
                                pModuleInfoNew->DevType,pModuleInfoNew->DevID,pModuleInfoNew->softVersion,
                                pModuleInfoNew->hardwVersion,pModuleInfoNew->VendorCode);

                            memcpy(softVer, &msg.softVersion, 4);
                    
                            if (updateStep == UPDATE_START)
                            {
                                MDLUP_FMT_DEBUG( "%s开始升级, 此时版本msg1 = %X %X %X %X\n", UpdateFileName, softVer[3], softVer[2], softVer[1], softVer[0]);
                                MdlUpdateDeal = MDL_UPDATE_READ_FILE_DELAY;

                            }
                            else if(updateStep == UPDATE_END)
                            {
                                MDLUP_FMT_DEBUG( "%s结束升级, 此时版本msg2 = %X %X %X %X\n", UpdateFileName, softVer[3], softVer[2], softVer[1], softVer[0]);
                                MDLUP_FMT_TRACE("模组升级成功\n");
                                MdlUpdateDeal = MDL_UPDATE_END;
                            }
                            else if((!isUpfileExist) && (isGetVer))
                            {
                                MDLUP_FMT_DEBUG( "msg = %X %X %X %X\n", softVer[3], softVer[2], softVer[1], softVer[0]);
                                module_ctrl_comm_uninit(g_UpdateUsbNo);
                                gCtrlDev[g_UpdateUsbNo] = -1;
                                MdlUpdateDeal = MDL_UPDATE_IDLE;
                            }
                            else
                            {
                                MDLUP_FMT_DEBUG("updateStep = %d\n", updateStep);
                                MdlUpdateDeal = MDL_UPDATE_AGAIN;
                            }  
                        }
                    }
                    else
                    {
                        MDLUP_FMT_DEBUG("MDL_UPDATE_GET_VER error ret = %d\n", ret);
                        MdlUpdateDeal = MDL_UPDATE_AGAIN;
                    }
                    break;

                case MDL_UPDATE_END:
                    close(Fd);
                    Fd = -1;
                    module_ctrl_comm_uninit(g_UpdateUsbNo);
                    gCtrlDev[g_UpdateUsbNo] = -1;

                    sprintf(TmpPath,"rm -rf %s", &UpdateFileName[0]);
                    system(TmpPath);
                    MDLUP_FMT_DEBUG("删除升级文件%s\n", TmpPath);

                    MdlUpdateDeal = MDL_UPDATE_IDLE;
                    break;

                default:
                    MdlUpdateDeal = MDL_UPDATE_IDLE;
                    break;
            }

            if (MdlUpdateDeal == MDL_UPDATE_IDLE)//跳出小循环，继续查找升级文件
            {
                sprintf(str, "模组升级结束");
                refresh_status_bar(60, str);
                MDLUP_FMT_DEBUG("跳出循环，继续查找是否有升级文件\n");
                break;
            }

            usleep(delayms); 
        }
    }
    return (void *)NULL;
}

/***********************************************************************
 * @brief   USB插拔识别
 * @param[in]  usbNo     : USB号
 *
 * @retval  > 0 初始化返回句柄值
  *         < 0 初始化失败 
 **********************************************************************/
USB_STATE_E CheckUsb(int32 usbNo)
{
    USB_STATE_E usbStatus = UNKNOW_STATE;
    uint8 name[256] = {0x00};

    sprintf((char*)name, "%s", mdlChannelCfg[usbNo].channelName);
    
    // 找设备目录
    if (access((char*)name, F_OK) >= 0)     
    {
        usbStatus = ON_STATE;
    }
    
    return usbStatus;
}

/***********************************************************************
 * @brief   判断是否存在控制口
 * @param[in]  usbNo     : USB号
 *             portName  ：控制口设备名称
 *
 * @retval  0 存在
 *          -1 错误         
 **********************************************************************/
int32 CheckIsExitCtrlPort(int32 usbNo, char *usbDevName)
{
    FILE* fp;
    char cmd[512] = {0};
    char buf[512] = {0};
    char *tmp;
    int32 ret = -1;

    sprintf(cmd, "ls %s:1.0/tty/", mdlChannelCfg[usbNo].channelName); 
    MDLMGR_FMT_TRACE(usbNo,"usbNo[%d], cmd:%s\n", usbNo, cmd); 

    fp = popen(cmd, "r"); 
    if (NULL == fp)   
    {   
        perror("popen error");   
        ret = -1;
        return ret;   
    }

    while (fgets(buf, sizeof(buf), fp) != NULL)
    {
        tmp = buf;
        MDLMGR_FMT_TRACE(usbNo,"usbNo[%d], output:%s", usbNo, buf);
        if(strncmp(tmp, "ttyACM", strlen("ttyACM")) == 0)
        {
            MDLMGR_FMT_TRACE(usbNo, "from cmd:%s find %s", cmd, tmp);
            strncpy(&usbDevName[0],tmp, strlen(tmp)-1);
            MDLMGR_FMT_TRACE(usbNo,"find USB%d ctl port usbDevName = %s\n", usbNo, usbDevName);
            ret = 0;
            break;
        }
    }

    pclose(fp);

    return ret;
}

/***********************************************************************
 * @brief   映射数据端口，此端口需在协商成功后，执行，防止驱动识别慢
 * @param[in]  usbNo     : USB号
 *
 * @retval  0 成功
 *          -1 错误         
 **********************************************************************/
int32 LnUsbDataComm(int32 usbNo)
{
    FILE* fp = NULL;
    FILE* fp1 = NULL;
    FILE* fp2 = NULL;
    FILE* fp3 = NULL;
    char cmd[512] = {0};
    char cmd2[512] = {0};
    char filepath[20] = {0};
    char buf[512] = {0};
    char *tmp;
    int32 ret = -1;
    char ttyName[20] = {0};
    char tmpStr[256] = {0};
    char tmpStr1[256] = {0};
    char *str;
    uint8 j = 0;
    uint8 i = 0;
    BOOL failFlag = FALSE;
    char name[256] = {0x00};
    memset(name, 0x00, sizeof(name));
    SLOT_INFO_T *mdl;
    LINKRESPONSE_T *msg;
    mdl = &slotInfo[usbNo];
    msg = &moduleInfo_all[usbNo].linkInfo;
    uint8 virIndex = 0;
    
    if (mdl->modeTypeLen == 0xff || mdl->usbNo == 0xff || mdl->modeTypeLen == 0)
    {
        MDLMGR_FMT_TRACE(usbNo, "LnUsbDataComm fail, ReadRamXData(MDL_RECOGNITION_USB) error\n");
        return ret;
    }

    if ( msg->modMode[0] == 0xff)
    {
        MDLMGR_FMT_TRACE(usbNo, "LnUsbDataComm fail, ReadRamXData(MDL_NGT_INFO_USB) error\n");
        return ret;
    }

    sprintf(cmd, "ls %s:1.* -d",mdlChannelCfg[usbNo].channelName);
    MDLMGR_FMT_TRACE(usbNo, "usbNo[%d], cmd:%s\n", usbNo, cmd); 

    //多次尝试，防止ACM顺序错误
    for (j = 0; j < 10; j++)
    {
        i = 0;
        failFlag = FALSE;
        memset(&buf[0], 0, sizeof(buf));
        memset(&name[0], 0x00, sizeof(name));
        memset(&ttyName[0], 0x00, sizeof(ttyName));
        
        fp = popen(cmd, "r"); 
        if (NULL == fp)   
        {   
            perror("popen error");   
            ret = -1;
            return ret;   
        }
        while (fgets(buf, sizeof(buf), fp) != NULL)
        {
            tmp = buf;
            MDLMGR_FMT_TRACE(usbNo, "usbNo = %d, result = %s\n", usbNo, buf);

            sprintf(&tmpStr[0], "%s:1.", mdlChannelCfg[usbNo].channelInfo);
            if ((str = strstr(tmp, tmpStr)) == NULL)
            {
                failFlag = TRUE;
                break;
            }
            else
            {
                MDLMGR_FMT_TRACE(usbNo, "str = %s no = %d\n", str, str[8] - 0x30);
            }

            if((str[8] - 0x30)%2 == 0)
            {
                i = (str[8] - 0x30)/2 ;
            }
            else
            {
                continue;
            }

            if(strncpy(tmpStr1,str,9) == NULL)
            {
                failFlag = TRUE;
                break;
            }
            MDLMGR_FMT_TRACE(usbNo, "tmpStr1 = %s\n",tmpStr1);
            
            sprintf(cmd2,"ls /sys/bus/usb/devices/%s/ | egrep tty\\|net",tmpStr1);
            MDLMGR_FMT_TRACE(usbNo, "%s\n",cmd2);
            memset(buf,0x0,sizeof(buf));
            if((fp1 = popen(cmd2,"r")) == NULL || fgets(buf, sizeof(buf), fp1) == NULL )
            {
                MDLMGR_FMT_TRACE(usbNo, "未找到 tty或者net\n");
                pclose(fp1);
                continue;
            }
            if(buf[0] == 0x00)
            {
                continue;
            }
            strncpy(filepath,buf,3);
            MDLMGR_FMT_TRACE(usbNo, "filepath = %s,buf = %s\n",filepath,buf);
            
            sprintf(cmd2,"ls /sys/bus/usb/devices/%s/%s/ | egrep ttyACM*\\|usb*",tmpStr1,filepath);
            MDLMGR_FMT_TRACE(usbNo, "%s\n",cmd2);
            memset(buf,0x0,sizeof(buf));
            if((fp2 = popen(cmd2,"r")) == NULL || fgets(buf,sizeof(buf),fp2) == NULL)
            {
                MDLMGR_FMT_TRACE(usbNo, "未找到 ttyACM*或者usb*\n");
                pclose(fp2);
                continue;
            }
            if(buf[0] == 0x00)
            {
                continue;
            }
            memset(ttyName,0x0,sizeof(ttyName));
            //MDLMGR_FMT_TRACE(usbNo, "LnUsbDataComm:---------------%s ----\n",buf);
            if(strstr(buf,"ttyACM") != NULL)
            {
                strncpy(ttyName,buf,strlen(buf)-1);
            }
            else if(strstr(buf,"usb") != NULL)
            {
                strncpy(ttyName,buf,strlen(buf)-1);
            }
            else
            {
                continue;
            }
            MDLMGR_FMT_TRACE(usbNo, "ttyName = %s,buf = %s\n",ttyName,buf);
            MDLMGR_FMT_TRACE(usbNo, "i = %d\n", i);
            
            sprintf(name, "%s/usb_exm%d_%d", LinkDir,(usbNo + 1), i);
            
            // 已经建立映射直接退出
            if (access(name, F_OK) >= 0)        
            {
                MDLMGR_FMT_TRACE(usbNo, "存在软链接的设备名 name = %s\n", name);
            }
            else if (strstr(ttyName,"usb") != NULL)
            {
                MDLMGR_FMT_TRACE(usbNo, "ECM口无需映射，ttyName = %s\n",ttyName);
            }
            else
            {
                memset(name, 0x00, sizeof(name));
                sprintf(name, "ln -s /dev/%s %s/usb_exm%d_%d",ttyName, LinkDir,(usbNo + 1), i);
                if ((fp3 = popen(name, "r")) == NULL)
                {
                    pclose(fp1);
                    pclose(fp2);
                    MDLMGR_FMT_DEBUG(usbNo, "ln -s /dev/%s %s/usb_exm%d_%d err\n",ttyName, LinkDir,(usbNo + 1), i);
                    continue;
                }
                pclose(fp3);
                MDLMGR_FMT_TRACE(usbNo, "创建软链接的设备名 name = %s\n", name);
            }
            ret = 0;
            pclose(fp1);
            pclose(fp2);
            virIndex = (virIndex > i) ? virIndex : i ;

            //填入虚拟通道驱动名称
            memset(name, 0x00, sizeof(name));
            sprintf(name, "/dev/%s",ttyName);
            strcpy(msg->virturMode[i].driveName, name);
            MDLMGR_FMT_DEBUG(usbNo, "记录槽位[%d]的管理通道驱动名称:%s\n", usbNo + 1, msg->virturMode[i].driveName);
        }

        if (virIndex+1 < msg->virPipeCnt)
        {
            //不判断协商通道和实际通道是否一致
            #if 0
            failFlag = TRUE;            
            ret = -1;
            #endif
            
            MDLMGR_FMT_DEBUG(usbNo, "数据端口数量不对，消息通道数 = %d, 实际通道数 = %d\n", msg->virPipeCnt,virIndex+1);
        }
        
        pclose(fp);

        //如果没有顺序错误，则跳出循环
        if (!failFlag)
        {
            break;
        }
        else
        {
            sleep(1);
        }
    }

    return ret;

}

/***********************************************************************
 * @brief   USB链接对应的设备名称
 * @param[in]  usbNo     : USB号
 *
 * @retval  无
  *         
 **********************************************************************/
int32 LnUsbComm(int32 usbNo)
{
    char usbDevName[50] = {0};
    FILE* fp = NULL;
    uint8 j = 0;
    DIR*  dirp = NULL;

    LINKRESPONSE_T *msg;
    msg = &moduleInfo_all[usbNo].linkInfo;
    
    char name[256] = {0x00};
    char buf[256]  = {0x00};

    //防止驱动识别反应慢，多尝试几次
    uint8 i = 0;
    for (i = 0; i < 10; i++)
    {
        if (CheckIsExitCtrlPort(usbNo, &usbDevName[0]) < 0)
        {
            MDLMGR_FMT_TRACE(usbNo,"查找控制口失败\n");
            sleep(1);
            continue;
        }
        else
        {
            break;
        }  
    }
    if (i == 10)
    {
        return -1;
    }

    sprintf(name, "%s/usb_exm%d_0", LinkDir,(usbNo + 1));
    // 已经建立映射直接退出
    if (access(name, F_OK) >= 0)        
    {
        MDLMGR_FMT_TRACE(usbNo,"映射设备%s 已经存在\n", name);
        MDLMGR_FMT_DEBUG(mdlnum, "映射设备%s 已经存在\n", name);
    }
    else
    {
        memset(name, 0x00, sizeof(name));
        sprintf(name, "ln -s /dev/%s %s/usb_exm%d_0", usbDevName, LinkDir,(usbNo + 1));
        char tmpCmd[100] = {0};
        sprintf(tmpCmd,"mkdir %s",LinkDir);
        if((dirp = opendir(LinkDir)) == NULL && system(tmpCmd) < 0)     //路径不存在需创建
        {
            MDLMGR_FMT_DEBUG(mdlnum, "路径创建失败\n");
            if (dirp != NULL)
            {
                closedir(dirp);
                dirp = NULL;
            }
            return -1;
        }
        if (dirp != NULL)
        {
            closedir(dirp);
            dirp = NULL;
        }
        
        while ((fp = popen(name, "r")) == NULL)
        {
            j++;
            if (j > 3)
            {
                MDLMGR_FMT_DEBUG(mdlnum, "ln -s /dev/%s %s/usb_exm%d_0 err\n", usbDevName, LinkDir,(usbNo + 1));
                return -1;
            }
        }
        pclose(fp);

        //填入虚拟通道驱动名称
        memset(buf, 0x00, sizeof(buf));
        sprintf(buf, "/dev/%s", usbDevName);
        strcpy(msg->virturMode[0].driveName, buf);
        MDLMGR_FMT_DEBUG(usbNo, "记录槽位[%d]的管理通道驱动名称:%s\n", usbNo + 1, msg->virturMode[0].driveName);
                    
        MDLMGR_FMT_TRACE(usbNo,"成功创建软链接 %s\n", strstr(name,"usb"));
        MDLMGR_FMT_DEBUG(mdlnum, "ln -s /dev/%s %s/usb_exm%d_0 ok\n", usbDevName, LinkDir,(usbNo + 1));
        return 0;
    }
    return 0;
}


/***********************************************************************
 * @brief   根据USB的插拔确定模组的状态
 * @param[in]  usbNo     : 槽位号0-4
 *
 * @retval  MODULE_STATE_E
 **********************************************************************/
MODULE_STATE_E FixMdlStatus(int32 usbNo, USB_STATE_E *usbStatusLst, uint8 *cntFailInit, uint8 *cntSuccInit)
{
    int32          ret = -1;
    USB_STATE_E    curUsbStatus = UNKNOW_STATE;
    MODULE_STATE_E em_status = IDLE_STATE;
    char           barStr[100] = {0};

    // usb状态有变化时设置对应的处理状态
    curUsbStatus = CheckUsb(usbNo);
    
    if (curUsbStatus != *usbStatusLst)
    {
        plug_status[usbNo].plugStatus = curUsbStatus;
        plug_status[usbNo].changeStatus = 1;
        if(plug_status[usbNo].plugStatus != ON_STATE)
        {
            PlugEventInform(usbNo);             //上报模组拔出事件
            MDLMGR_FMT_DEBUG(usbNo, "上报模组[%d]拔出事件!\n", usbNo + 1);

            //声光提示
            sprintf(barStr, "上报模组[%d]拔出事件", usbNo + 1);
            refresh_status_bar(60, barStr);
            mdl_open_buzzer(100);
        }
        
        //slotInfo[usbNo].usbNo = -1;           //清除槽位信息

        //ClearNgtMsg(usbNo);

//      if(curUsbStatus == ON_STATE)
//      {
//          //查询模组信息
//          TRANS_INFO_T transinfo;
//          memset(&transinfo,0x00,sizeof(TRANS_INFO_T));
//          transinfo.seq = globleSeq++;
//          PlcWriteQueue(&LockSendBuf[usbNo], &transinfo, sizeof(TRANS_INFO_T));
//      }
        MDLMGR_FMT_DEBUG(usbNo,"usbNo[%d]. curUsbStatus = %d. *usbStatusLst = %d \n", usbNo, curUsbStatus, *usbStatusLst);
        // 根据上一次的状态来确定本次操作
        switch (*usbStatusLst)
        {
            case UNKNOW_STATE:
            case OFF_STATE:
                // 做软链接处理
                ret = LnUsbComm(usbNo);
                if (ret < 0)
                {
                    em_status = OTHER_STATE;
                }
                else
                {
                    em_status = INIT_STATE;
                }  
                break;

            case ON_STATE:
                // 上次为插入此次为拔出需要关闭
                MDLMGR_FMT_DEBUG(usbNo,"usbNo[%d]. 上次为插入此次为拔出, 需要关闭\n", usbNo);

                // 拔出时清除初始化标记
                *cntFailInit = 0;
                *cntSuccInit = 0;
                em_status = CLOSE_STATE;
                break;

            default:
                em_status = INIT_STATE;
                break;                    
        }
        *usbStatusLst = curUsbStatus;
    }

    return em_status;
}

/***********************************************************************
 * @brief   获取Fn
 * @param[in]  
 *
 * @retval  >= 0 
  *         < 0 
 **********************************************************************/
int16 GetFn(uint8 dt1, uint8 dt2)
{
    uint8 i = 0;
    uint8 fn = 0;

    for (i = 0; i < 8; i++)
    {
        if (((dt1>>i)&0x01) == 0x01)
        {
            fn = dt2 * 8 + i + 1;
            break;
        }
    }

    if (i == 8)
    {
        fn = -1;
    }

    return fn;
}

/***********************************************************************
 * @brief   清除获取链路信息
 * @param[in]  
 *
 * @retval  >= 0 
  *         < 0 
 **********************************************************************/
void ClearNgtMsg(int32 usbNo)
{
    if (usbNo > mdlnum-1 || usbNo < 0)
    {
        return;
    }
    
    LINKRESPONSE_T *msg;
    msg = &moduleInfo_all[usbNo].linkInfo;
    memset(msg, 0xff, sizeof(LINKRESPONSE_T));

    moduleInfo_all[usbNo].modInfoFlag = 0x0;
    MODULE_INF_T *mdl;
    mdl = &moduleInfo_all[usbNo].moduleInfo;
    memset(mdl, 0xff, sizeof(MODULE_INF_T));
    
    SLOT_INFO_T *slot;
    slot = &slotInfo[usbNo];
    memset(slot,0xff,sizeof(SLOT_INFO_T));

    MDLMGR_FMT_TRACE(usbNo, "clear the mdlmsg on slot[%d]\n", usbNo);

    return;
}


/***********************************************************************
 * @brief   模组管理器组帧函数
 * @param[in]  
 *
 * @retval  > 0 
  *         < 0 
 **********************************************************************/
int32 Mdl_MakeFrame(MDL_FRAME_INFO_T *srcframeInfo, uint8 *desDataBuf)
{
    uint16 offset = 0;
    uint16 len = 0;
    uint16 crc = 0;
    desDataBuf[H_SITE_M] = 0x7e;

    SFP_CMD_L_U cmd_l;

    cmd_l.bit.dir = 0;
    cmd_l.bit.prm = 1;
    cmd_l.bit.seq = srcframeInfo->seq;

    desDataBuf[C_SITE_M] = cmd_l.all;
    
    offset = D_SITE_M;

    if (len > 0)
    {
        desDataBuf[L_LOW_M] = (uint8)len;
        desDataBuf[L_HIGH_M] = (uint8)(len >> 8);
    }

    // 效验
    crc = CRC16_Get(&desDataBuf[1], offset - 1); 
    memcpy((uint8 *)&desDataBuf[offset], (uint8 *)&crc, 2);
    offset += 2;
    
    desDataBuf[offset] = 0x7e;
    offset += 1;

    return offset;
}


/***********************************************************************
 * @brief   模块帧格式有效性判断
 * @param[in]  
 *
 * @retval  > 0 
  *         < 0 
 **********************************************************************/
BOOL CheckFrame(uint8 *czBuf, int16 len)
{
    uint16 lenData = 0;             // 数据长度
    uint16 crc = 0;                 // 数据校验
    uint16 checkcrc = 0;            // 数据校验

    memcpy((char *)&lenData, (char *)&czBuf[L_LOW_M], 2);

    // 长度不可能小于固定长度
    if  (lenData > len - 8)
    {
        return FALSE;
    }
    
    // 计算效验和
    crc = CRC16_Get(&czBuf[1], 2+2+lenData);

    memcpy((uint8 *)&checkcrc, &czBuf[5+lenData], 2);

    // 比对效验和
    if (checkcrc != crc)
    {   
        return FALSE;
    }

    if(czBuf[7+lenData] != 0x16)
    {
        return FALSE;
    }
    
    return TRUE;
}

/***********************************************************************
 * @brief   获取有效帧的起始地址
 * @param[in]  
 *
 * @retval  > 0 
  *         < 0 
 **********************************************************************/
int16 GetStartAddrOfFrame(uint8 *czBuf, int16 len)
{
    int16 dwOffset = 0;

    while (dwOffset < len - 7)
    {
        // 找到7e头
        if (czBuf[dwOffset] != 0x68)
        {
            dwOffset++;
            continue;
        } 

        // 验证码子的有效性
        if (CheckFrame(((uint8 *)&czBuf[dwOffset]), (len - dwOffset)))
        {
            return dwOffset;
        }
        else
        {
            dwOffset++;
            continue;
        }
    }

    return -1;
}


/***********************************************************************
 * @brief   查询链路协商帧处理
 * @param[in]  
 *
 * @retval  > 0 
  *         < 0 
 **********************************************************************/
int32 GetNegotiateDeal(int32 usbNo,uint8 *czBuf, int16 len, BOOL isInclude)
{
    LINKRESPONSE_T  responseData;
    uint16 dwOffset = 0;
    int i;
    LINKRESPONSE_T *pLinkData = &moduleInfo_all[usbNo].linkInfo;
    
    memset(&responseData,0x0,sizeof(LINKRESPONSE_T));
    
    if(czBuf[dwOffset++] != LONG_UNSIGNED)  //协议版本类型不对
    {   
        return -1;
    }
    memcpy(&responseData.prtlVersion,&czBuf[dwOffset],2);
    MDLMGR_FMT_TRACE(usbNo,"协议版本：%04x\n",responseData.prtlVersion);
    dwOffset += 2;
    if(czBuf[dwOffset++] != VISIBLE_STRING)     //模块型号类型不对
    {
        return -1;
    }
    responseData.modModeLen = czBuf[dwOffset];
    //printf("模块类型长度：%d\n",responseData.modModeLen);
    dwOffset++;
    memcpy(responseData.modMode,&czBuf[dwOffset],responseData.modModeLen);
    MDLMGR_FMT_TRACE(usbNo,"模块类型：%s\n",responseData.modMode);
    dwOffset += responseData.modModeLen;
    if(czBuf[dwOffset++] != VISIBLE_STRING)     //模块ID类型不对
    {
        return -1;
    }
    responseData.modIDLen = czBuf[dwOffset];
    //printf("模块ID长度：%d\n",responseData.modIDLen);
    dwOffset++;
    memcpy(responseData.modID,&czBuf[dwOffset],responseData.modIDLen);
    MDLMGR_FMT_TRACE(usbNo,"模块ID：%s\n",responseData.modID);
    dwOffset += responseData.modIDLen;
    if(czBuf[dwOffset++] != LONG_UNSIGNED)      //最大发送缓冲区长度
    {
        return -1;
    }
    //memcpy(&responseData.sendBufLen,&czBuf[dwOffset],2);
    responseData.sendBufLen = (czBuf[dwOffset] << 8) +czBuf[dwOffset+1];
    MDLMGR_FMT_TRACE(usbNo,"发送缓冲区长度：%d\n",responseData.sendBufLen);
    dwOffset += 2;
    if(czBuf[dwOffset++] != LONG_UNSIGNED)      //最大接收缓冲区长度
    {

        return -1;
    }
    responseData.recvBufLen = (czBuf[dwOffset] << 8) +czBuf[dwOffset+1];
    MDLMGR_FMT_TRACE(usbNo,"接收缓冲区长度：%d\n",responseData.recvBufLen);
    dwOffset += 2;
    if(czBuf[dwOffset++] != UNSIGNED)           //并发窗口数
    {
        return -1;
    }
    responseData.conCurrWDs = czBuf[dwOffset];
    MDLMGR_FMT_TRACE(usbNo,"并发窗口数：%d\n",responseData.conCurrWDs);
    dwOffset++;

    //兼容老版本协议：
    if(isInclude == 1)
    {
        dwOffset += 2;
    }
    
    if(czBuf[dwOffset++] != ARRAY)          //数组类型
    {
        return -1;
    }
    responseData.virPipeCnt = czBuf[dwOffset];
    MDLMGR_FMT_TRACE(usbNo,"虚拟通道数：%d\n",responseData.virPipeCnt);
    dwOffset++;
        
    for (i = 0; i< responseData.virPipeCnt; i++)
    {
            if(czBuf[dwOffset++] != STRUCTURE)          //structure
            {
                return -1;
            }
            if(czBuf[dwOffset++] != 2)              //structure中数据项
            {
                return -1;
            }
            if(czBuf[dwOffset++] != ENUM)           //虚拟通道类型
            {
                return -1;
            }
            responseData.virturMode[i].interf = (INTERFACE_TYPE_T)czBuf[dwOffset++];
            MDLMGR_FMT_TRACE(usbNo,"接口%d类型：%d\n",i,(responseData.virturMode[i].interf));
            if(czBuf[dwOffset++] != UNSIGNED)       //功能配置
            {
                return -1;
            }
            responseData.virturMode[i].funcConfig = czBuf[dwOffset++];
            MDLMGR_FMT_TRACE(usbNo,"功能配置：%d\n",responseData.virturMode[i].funcConfig);
    }

    

    if(dwOffset != len)
    {
        MDLMGR_FMT_TRACE(usbNo, "[%d]NegotiateDeal解析数据帧长度%d与输入长度%d不符\n",usbNo+1,dwOffset,len);
        return -1;
    }

#ifdef  TEMP_DINGXIN_MDL
    else if(mdlIsIncludeName(responseData.modMode, "TTT", responseData.modModeLen))     
    {
        char *DX =DINGXING_TYPE_NAME;
        responseData.modModeLen =strlen(DX);
        memset(responseData.modMode,0x0,sizeof(responseData.modMode));
        memcpy(responseData.modMode,DX,strlen(DX));
    }
#else
    //鼎信(本地通信)模块实际增加
    
#endif

    //mdl.usbNo = usbNo;

    //保存链路信息
    memcpy(pLinkData,&responseData,sizeof(LINKRESPONSE_T));
    //刷新槽位信息
    slotInfo[usbNo].usbNo = usbNo;
    slotInfo[usbNo].modeTypeLen = responseData.modModeLen;
    memset(slotInfo[usbNo].modeType,0x0,sizeof(slotInfo[usbNo].modeType));
    memcpy(slotInfo[usbNo].modeType,responseData.modMode,responseData.modModeLen);
    memset(&moduleInfo_all[usbNo].linkInfo,0x0,sizeof(LINKRESPONSE_T));
    memcpy(&moduleInfo_all[usbNo].linkInfo,&responseData,sizeof(LINKRESPONSE_T));
    MDLMGR_FMT_TRACE(usbNo,"卡槽%d信息：模组型号 %s \n",usbNo+1, responseData.modMode);
    return 0;
}


/***********************************************************************
 * @brief   链路协商帧处理
 * @param[in]  
 *
 * @retval  > 0 
  *         < 0 
 **********************************************************************/
int32 NegotiateDeal(int32 usbNo, uint8 *czBuf, int16 len)
{
    LINKRESPONSE_T  responseData;
    uint16 dwOffset = 0;
    int i;
    LINKRESPONSE_T *pLinkData = &moduleInfo_all[usbNo].linkInfo;
    
    memset(&responseData, 0x00, sizeof(LINKRESPONSE_T));

    memcpy(&responseData.prtlVersion, &czBuf[dwOffset],2);
    MDLMGR_FMT_TRACE(usbNo, "协议版本：%04x\n", responseData.prtlVersion);
    dwOffset += 2;
    responseData.modModeLen = czBuf[dwOffset];
    dwOffset++;
    memcpy(responseData.modMode, &czBuf[dwOffset], responseData.modModeLen);
    MDLMGR_FMT_TRACE(usbNo, "模块类型：%s\n", responseData.modMode);
    dwOffset += responseData.modModeLen;

    responseData.modIDLen = czBuf[dwOffset];
    dwOffset++;
    memcpy(responseData.modID, &czBuf[dwOffset], responseData.modIDLen);
    MDLMGR_FMT_TRACE(usbNo,"模块ID：%s\n", responseData.modID);
    dwOffset += responseData.modIDLen;

    responseData.sendBufLen = (czBuf[dwOffset] << 8) +czBuf[dwOffset+1];
    MDLMGR_FMT_TRACE(usbNo,"发送缓冲区长度：%d\n", responseData.sendBufLen);
    dwOffset += 2;

    responseData.recvBufLen = (czBuf[dwOffset] << 8) +czBuf[dwOffset+1];
    MDLMGR_FMT_TRACE(usbNo,"接收缓冲区长度：%d\n", responseData.recvBufLen);
    dwOffset += 2;

    responseData.conCurrWDs = czBuf[dwOffset];
    MDLMGR_FMT_TRACE(usbNo,"并发窗口数：%d\n", responseData.conCurrWDs);
    dwOffset++;
    responseData.virPipeCnt = czBuf[dwOffset];
    MDLMGR_FMT_TRACE(usbNo,"虚拟通道数：%d\n", responseData.virPipeCnt);
    dwOffset++;
    if(responseData.conCurrWDs < 5 && responseData.virPipeCnt < 10)
    {
        for (i = 0; i < responseData.virPipeCnt; i++)
        {

            responseData.virturMode[i].interf = (INTERFACE_TYPE_T)czBuf[dwOffset];
            MDLMGR_FMT_TRACE(usbNo,"接口%d类型：%d\n", i, responseData.virturMode[i].interf);
            dwOffset++;

            responseData.virturMode[i].funcConfig = czBuf[dwOffset];
            MDLMGR_FMT_TRACE(usbNo, "功能配置：%d\n", responseData.virturMode[i].funcConfig);
            dwOffset++;
        }

    }
    else
    {
        MDLMGR_FMT_DEBUG(usbNo, "[%d]并发串口数:%d or 虚拟通道数:%d 异常!\n", usbNo + 1, responseData.conCurrWDs, responseData.virPipeCnt);
    }

    if(dwOffset != len)
    {
        MDLMGR_FMT_DEBUG(usbNo, "usbNo[%d]NegotiateDeal无类型解析数据帧长度%d与输入长度[%d]不符，重新解析\n", usbNo+1, dwOffset, len);
        memset(&responseData, 0x0, sizeof(LINKRESPONSE_T));
        dwOffset = 0;

        if(czBuf[dwOffset++] != LONG_UNSIGNED)  //协议版本类型不对
        {   
            return -1;
        }
        memcpy(&responseData.prtlVersion, &czBuf[dwOffset], 2);
        MDLMGR_FMT_TRACE(usbNo,"协议版本：%04x\n", responseData.prtlVersion);
        dwOffset += 2;
        if(czBuf[dwOffset++] != VISIBLE_STRING)     //模块型号类型不对
        {
            return -1;
        }
        responseData.modModeLen = czBuf[dwOffset];
        //printf("模块类型长度：%d\n",responseData.modModeLen);
        dwOffset++;
        memcpy(responseData.modMode, &czBuf[dwOffset], responseData.modModeLen);
        MDLMGR_FMT_TRACE(usbNo,"模块类型：%s\n", responseData.modMode);
        dwOffset += responseData.modModeLen;
        if(czBuf[dwOffset++] != VISIBLE_STRING)     //模块ID类型不对
        {
            return -1;
        }
        responseData.modIDLen = czBuf[dwOffset];
        //printf("模块ID长度：%d\n",responseData.modIDLen);
        dwOffset++;
        memcpy(responseData.modID,&czBuf[dwOffset],responseData.modIDLen);
        MDLMGR_FMT_TRACE(usbNo,"模块ID：%s\n", responseData.modID);
        dwOffset += responseData.modIDLen;
        if(czBuf[dwOffset++] != LONG_UNSIGNED)      //最大发送缓冲区长度
        {
            return -1;
        }
        //memcpy(&responseData.sendBufLen,&czBuf[dwOffset],2);
        responseData.sendBufLen = (czBuf[dwOffset] << 8) +czBuf[dwOffset+1];
        MDLMGR_FMT_TRACE(usbNo,"发送缓冲区长度：%d\n", responseData.sendBufLen);
        dwOffset += 2;
        if(czBuf[dwOffset++] != LONG_UNSIGNED)      //最大接收缓冲区长度
        {

            return -1;
        }
        responseData.recvBufLen = (czBuf[dwOffset] << 8) +czBuf[dwOffset+1];
        MDLMGR_FMT_TRACE(usbNo,"接收缓冲区长度：%d\n", responseData.recvBufLen);
        dwOffset += 2;
        if(czBuf[dwOffset++] != UNSIGNED)           //并发窗口数
        {
            return -1;
        }
        responseData.conCurrWDs = czBuf[dwOffset];
        MDLMGR_FMT_TRACE(usbNo,"并发窗口数：%d\n",responseData.conCurrWDs);
        dwOffset++;
        responseData.virPipeCnt = czBuf[dwOffset];
        MDLMGR_FMT_TRACE(usbNo,"虚拟通道数：%d\n",responseData.virPipeCnt);
        dwOffset++;
        
        for (i = 0; i< responseData.virPipeCnt; i++)
        {
            if(czBuf[dwOffset++] != ENUM)           //虚拟通道类型
            {
                return -1;
            }
            responseData.virturMode[i].interf = (INTERFACE_TYPE_T)czBuf[dwOffset];
            MDLMGR_FMT_TRACE(usbNo,"接口%d类型：%d\n", i, responseData.virturMode[i].interf);
            dwOffset++;
            if(czBuf[dwOffset++] != UNSIGNED)       //功能配置
            {
                return -1;
            }
            responseData.virturMode[i].funcConfig = czBuf[dwOffset];
            MDLMGR_FMT_TRACE(usbNo,"功能配置：%d\n", responseData.virturMode[i].funcConfig);
            dwOffset++;
        }

    }

    if(dwOffset != len)
    {
        MDLMGR_FMT_DEBUG(usbNo, "[%d]NegotiateDeal解析数据帧长度%d与输入长度%d不符\n",usbNo+1,dwOffset,len);
        return -1;
    }

#ifdef  TEMP_DINGXIN_MDL
    else if(mdlIsIncludeName(responseData.modMode, "TTT", responseData.modModeLen))     
    {
        char *DX = DINGXING_TYPE_NAME;
        responseData.modModeLen =strlen(DX);
        memset(responseData.modMode, 0x0, sizeof(responseData.modMode));
        memcpy(responseData.modMode, DX, strlen(DX));
    }
#else
    //鼎信(本地通信)模块实际增加
    
#endif
    

    //保存链路信息
    memcpy(pLinkData,&responseData,sizeof(LINKRESPONSE_T));
    //刷新槽位信息
    slotInfo[usbNo].usbNo = usbNo;
    slotInfo[usbNo].modeTypeLen = responseData.modModeLen;
    memset(slotInfo[usbNo].modeType, 0x0, sizeof(slotInfo[usbNo].modeType));
    memcpy(slotInfo[usbNo].modeType, responseData.modMode, responseData.modModeLen);
    memset(&moduleInfo_all[usbNo].linkInfo, 0x0, sizeof(LINKRESPONSE_T));
    memcpy(&moduleInfo_all[usbNo].linkInfo, &responseData, sizeof(LINKRESPONSE_T));
    MDLMGR_FMT_TRACE(usbNo,"卡槽%d信息：模组型号 %s\n",usbNo+1, responseData.modMode);
    return 0;
}

/***********************************************************************
 * @brief   根据模组信息组mqtt消息结构体
 * @param[in]  
 *
 * @retval  > 0 成功
            < 0 失败
 **********************************************************************/
int32 makeInfoData(uint8 usbNo,MODULE_INF_T *pModuleInfo, INFO_DATA_T *pRecvInfo,      uint8 seq, INFO_DATA_T *pSendInfo)
{
    uint8  payLoad[MAX_MSG_PAYLOAD_LEN] = {0};
    int    ploffset = 0;
    uint8  datetemp[5] = {0};
    char   dateAscii[20] = {0};
    uint16 asclen = 0;
    uint8  choiceCode = 0;             //默认已识别
    int i = 0;
    LINKRESPONSE_T* pLinkInfo = &moduleInfo_all[usbNo].linkInfo;

    if(pModuleInfo == NULL || pSendInfo == NULL)
    {
        return -1;
    }
    
    if(moduleInfo_all[usbNo].modInfoFlag == 0x55)
    {
        choiceCode = 0;                 
    }
    else if(moduleInfo_all[usbNo].modInfoFlag == 0xAA)
    {
        MDLMGR_FMT_TRACE(mdlnum, "槽位 %d 无模组信息!\n",usbNo);
        choiceCode = 1;                 //模组未识别
    }
    else
    {
        MDLMGR_FMT_TRACE(mdlnum, "槽位 %d 无模组信息!\n",usbNo);
        choiceCode = 2;                 //模组未接入
    }

    payLoad[ploffset++] = choiceCode;
    if(choiceCode == 0)
    {
        //组模块信息回码payload： INFO_DATA_T
        payLoad[ploffset++] = pLinkInfo->modModeLen;

        memcpy(&payLoad[ploffset], pLinkInfo->modMode, pLinkInfo->modModeLen);      //模块类型从链路信息取
        ploffset += pLinkInfo->modModeLen;

        payLoad[ploffset++] = pModuleInfo->DevIDLen;

        memcpy(&payLoad[ploffset], pModuleInfo->DevID, pModuleInfo->DevIDLen);
        ploffset += pModuleInfo->DevIDLen;

        if (pModuleInfo->type == MDL_PRTL_VER1)
        {
            memcpy(&payLoad[ploffset], pModuleInfo->softVer, 4);
            ploffset += 4;

            MDLMGR_FMT_TRACE(mdlnum, "槽位 %d-软件版本: %s\n", usbNo, pModuleInfo->softVer);
            MDLMGR_FMT_TRACE(mdlnum, "槽位 %d-软件发布日期:%s \n", usbNo, pModuleInfo->softDate);

            asclen = strlen(pModuleInfo->softDate) + 1;
            payLoad[ploffset++] = asclen;
            memcpy(&payLoad[ploffset], pModuleInfo->softDate, asclen);
            ploffset += asclen;

            //硬件版本，新协议
            memcpy(&payLoad[ploffset], pModuleInfo->hardVer, 4);
            ploffset += 4;

            MDLMGR_FMT_TRACE(mdlnum, "槽位 %d-硬件版本: %s\n", usbNo, pModuleInfo->softVer);
            MDLMGR_FMT_TRACE(mdlnum, "槽位 %d-硬件发布日期:%s \n", usbNo, pModuleInfo->softDate);

            asclen = strlen(pModuleInfo->hardDate) + 1;
            payLoad[ploffset++] = asclen;
            memcpy(&payLoad[ploffset], pModuleInfo->hardDate, asclen);
            ploffset += asclen;
        }
        else
        {
            memcpy_r(&payLoad[ploffset], (uint8*)&pModuleInfo->softVersion, 4);
            ploffset += 4;

            MDLMGR_FMT_TRACE(mdlnum, "槽位 %d-软件版本 %d\n", usbNo, pModuleInfo->softVersion);
            MDLMGR_FMT_TRACE(mdlnum, "槽位 %d-软件发布日期: %04d-%02d-%02d-%02d\n", usbNo, pModuleInfo->date_soft.year,
            pModuleInfo->date_soft.month,pModuleInfo->date_soft.dayofmonth,pModuleInfo->date_soft.dayofweek);
            memcpy(datetemp, &pModuleInfo->date_soft, 5);
            datetemp[0] = pModuleInfo->date_soft.year/100;
            datetemp[1] = pModuleInfo->date_soft.year%100;
            BINtoASCII(datetemp, 5, dateAscii, &asclen);

            payLoad[ploffset++] = asclen;

            memcpy(&payLoad[ploffset],dateAscii,asclen);
            ploffset += asclen;
            
            memcpy_r(&payLoad[ploffset], (uint8*)&pModuleInfo->hardwVersion,4);
            ploffset += 4;

            MDLMGR_FMT_TRACE(mdlnum, "槽位 %d-硬件版本 %d\n", usbNo, pModuleInfo->hardwVersion);
            MDLMGR_FMT_TRACE(mdlnum, "槽位 %d-硬件发布日期: %04d-%02d-%02d-%02d\n", usbNo, pModuleInfo->date_hard.year,
                pModuleInfo->date_hard.month, pModuleInfo->date_hard.dayofmonth, pModuleInfo->date_hard.dayofweek);
            memcpy(datetemp,&pModuleInfo->date_hard, 5);
            datetemp[0] = pModuleInfo->date_hard.year/100;
            datetemp[1] = pModuleInfo->date_hard.year%100;
            BINtoASCII(datetemp, 5, dateAscii, &asclen);

            payLoad[ploffset++] = asclen;

            memcpy(&payLoad[ploffset], dateAscii, asclen);
            ploffset += asclen;
        }
        
        payLoad[ploffset++] = pModuleInfo->VendorCodeLen;

        memcpy(&payLoad[ploffset],pModuleInfo->VendorCode,pModuleInfo->VendorCodeLen);
        ploffset += pModuleInfo->VendorCodeLen;

        payLoad[ploffset++] = pLinkInfo->virPipeCnt;
        for (i =0; i < pLinkInfo->virPipeCnt; i++ )
        {
            payLoad[ploffset++] = pLinkInfo->virturMode[i].interf;
            payLoad[ploffset++] = pLinkInfo->virturMode[i].funcConfig;
        }
    }
    
    memset(pSendInfo, 0x00, sizeof(INFO_DATA_T));
    if(pRecvInfo == NULL)
    {
        MDLReadInfo(&InfoDataQueue, seq, pSendInfo);
    }
    else
    {
        pSendInfo->IP = pRecvInfo->IP;
        pSendInfo->port = pRecvInfo->port;
        pSendInfo->MsgRPM = 0;
        pSendInfo->MsgPRIORITY = pRecvInfo->MsgPRIORITY;        
        pSendInfo->MsgIndex = pRecvInfo->MsgIndex;
        pSendInfo->MsgLabel = pRecvInfo->MsgLabel;
        pSendInfo->SourLen = pRecvInfo->SourLen;
        memcpy(pSendInfo->SourAddr,pRecvInfo->SourAddr, pRecvInfo->SourLen);
        pSendInfo->DestLen = pRecvInfo->DestLen;
        memcpy(pSendInfo->DestAddr, pRecvInfo->DestAddr, pRecvInfo->DestLen);
        //msg
        pSendInfo->IID_ID = pRecvInfo->IID_ID;
        pSendInfo->IOP_ID = pRecvInfo->IOP_ID;
    }
    pSendInfo->nPayloadLength = ploffset;
    memcpy(pSendInfo->aPayloadData, payLoad, ploffset);
    
    return 1;
}

/***********************************************************************
 * @brief   根据模组信息组mqtt消息结构体(新协议)
 * @param[in]  
 *
 * @retval  > 0 成功
            < 0 失败
 **********************************************************************/
int32 makeInfoData_new(uint8 usbNo,MODULE_INF_T *pModuleInfo, INFO_DATA_T *pRecvInfo,      uint8 seq, INFO_DATA_T *pSendInfo)
{
    uint8 payLoad[MAX_MSG_PAYLOAD_LEN] = {0};
    int ploffset = 0;
    uint8 datetemp[5] = {0};
    char dateAscii[20] = {0};
    uint16 asclen = 0;
    uint8   choiceCode = 0;             //默认已识别
    int i;

    char ver[256] = {0};
    
    LINKRESPONSE_T* pLinkInfo = &moduleInfo_all[usbNo].linkInfo;

    if(pModuleInfo == NULL || pSendInfo == NULL)
    {
        return -1;
    }
    
    if(moduleInfo_all[usbNo].modInfoFlag == 0x55)
    {
        choiceCode = 0;                 
    }
    else if(moduleInfo_all[usbNo].modInfoFlag == 0xAA)
    {
        MDLMGR_FMT_TRACE(mdlnum, "槽位 %d 无模组信息!\n",usbNo);
        choiceCode = 1;                 //模组未识别
    }
    else
    {
        MDLMGR_FMT_TRACE(mdlnum, "槽位 %d 无模组信息!\n",usbNo);
        choiceCode = 2;                 //模组未接入
    }

    payLoad[ploffset++] = choiceCode;
    if(choiceCode == 0)
    {
        //组模块信息回码payload： INFO_DATA_T
        payLoad[ploffset++] = pLinkInfo->modModeLen;

        memcpy(&payLoad[ploffset], pLinkInfo->modMode, pLinkInfo->modModeLen);      //模块类型从链路信息取
        ploffset += pLinkInfo->modModeLen;

        payLoad[ploffset++] = pModuleInfo->DevIDLen;

        memcpy(&payLoad[ploffset], pModuleInfo->DevID, pModuleInfo->DevIDLen);
        ploffset += pModuleInfo->DevIDLen;

        //软件版本，新协议
        if (pModuleInfo->type == MDL_PRTL_VER1)
        {
            sprintf(ver, "%s", pModuleInfo->softVer);
            payLoad[ploffset++] = strlen(ver) + 1;
            memcpy(&payLoad[ploffset], ver, strlen(ver) + 1);
            ploffset += strlen(ver) + 1;

            MDLMGR_FMT_TRACE(mdlnum, "usb %d-软件版本: %s\n", usbNo, pModuleInfo->softVer);
            MDLMGR_FMT_TRACE(mdlnum, "usb %d-软件发布日期:%s \n", usbNo, pModuleInfo->softDate);

            asclen = strlen(pModuleInfo->softDate) + 1;
            payLoad[ploffset++] = asclen;
            memcpy(&payLoad[ploffset], pModuleInfo->softDate, asclen);
            ploffset += asclen;

            //硬件版本，新协议
            memset(ver, 0, sizeof(ver));
            sprintf(ver, "%s", pModuleInfo->hardVer);
            payLoad[ploffset++] = strlen(ver) + 1;
            memcpy(&payLoad[ploffset], ver, strlen(ver) + 1);
            ploffset += strlen(ver) + 1;

            MDLMGR_FMT_TRACE(mdlnum, "usb %d-硬件版本: %s\n", usbNo, pModuleInfo->softVer);
            MDLMGR_FMT_TRACE(mdlnum, "usb %d-硬件发布日期:%s \n", usbNo, pModuleInfo->softDate);

            asclen = strlen(pModuleInfo->hardDate) + 1;
            payLoad[ploffset++] = asclen;
            memcpy(&payLoad[ploffset], pModuleInfo->hardDate, asclen);
            ploffset += asclen;
        }
        else
        {
            sprintf(ver, "%d", pModuleInfo->softVersion);
            payLoad[ploffset++] = strlen(ver) + 1;
            memcpy(&payLoad[ploffset], ver, strlen(ver) + 1);
            ploffset += strlen(ver) + 1;

            MDLMGR_FMT_TRACE(mdlnum, "usb %d-pModuleInfo->date_soft: %04d-%02d-%02d-%02d\n", usbNo, pModuleInfo->date_soft.year,
                pModuleInfo->date_soft.month,pModuleInfo->date_soft.dayofmonth,pModuleInfo->date_soft.dayofweek);
            memcpy(datetemp, &pModuleInfo->date_soft, 5);
            datetemp[0] = pModuleInfo->date_soft.year/100;
            datetemp[1] = pModuleInfo->date_soft.year%100;
            BINtoASCII(datetemp, 5, dateAscii, &asclen);
            MDLMGR_FMT_TRACE(mdlnum, "usb %d-makeInfoData：软件发布日期 %s \n", usbNo, dateAscii);

            payLoad[ploffset++] = asclen;

            memcpy(&payLoad[ploffset],dateAscii,asclen);
            ploffset += asclen;

            //硬件版本，新协议
            memset(ver, 0, sizeof(ver));
            sprintf(ver, "%d", pModuleInfo->hardwVersion);
            payLoad[ploffset++] = strlen(ver) + 1;
            memcpy(&payLoad[ploffset], ver, strlen(ver) + 1);
            ploffset += strlen(ver) + 1;

            MDLMGR_FMT_TRACE(mdlnum, "usb %d-pModuleInfo->date_hard: %04d-%02d-%02d-%02d\n", usbNo, pModuleInfo->date_hard.year,
                pModuleInfo->date_hard.month, pModuleInfo->date_hard.dayofmonth, pModuleInfo->date_hard.dayofweek);
            memcpy(datetemp,&pModuleInfo->date_hard, 5);
            datetemp[0] = pModuleInfo->date_hard.year/100;
            datetemp[1] = pModuleInfo->date_hard.year%100;
            BINtoASCII(datetemp, 5, dateAscii, &asclen);
            MDLMGR_FMT_TRACE(mdlnum, "usb %d-makeInfoData：硬件发布日期 %s \n", usbNo, dateAscii);

            payLoad[ploffset++] = asclen;

            memcpy(&payLoad[ploffset], dateAscii, asclen);
            ploffset += asclen;
        }
        
        payLoad[ploffset++] = pModuleInfo->VendorCodeLen;

        memcpy(&payLoad[ploffset],pModuleInfo->VendorCode,pModuleInfo->VendorCodeLen);
        ploffset += pModuleInfo->VendorCodeLen;

        payLoad[ploffset++] = pLinkInfo->virPipeCnt;
        for (i =0; i < pLinkInfo->virPipeCnt; i++ )
        {
            payLoad[ploffset++] = pLinkInfo->virturMode[i].interf;
            payLoad[ploffset++] = pLinkInfo->virturMode[i].funcConfig;
            payLoad[ploffset++] = strlen(pLinkInfo->virturMode[i].driveName) + 1;
            memcpy(&payLoad[ploffset], pLinkInfo->virturMode[i].driveName, strlen(pLinkInfo->virturMode[i].driveName) + 1);
            ploffset += strlen(pLinkInfo->virturMode[i].driveName) + 1;
        }
    }
    
    memset(pSendInfo, 0x00, sizeof(INFO_DATA_T));
    if(pRecvInfo == NULL)
    {
        MDLReadInfo(&InfoDataQueue, seq, pSendInfo);
    }
    else
    {
        pSendInfo->IP = pRecvInfo->IP;
        pSendInfo->port = pRecvInfo->port;
        pSendInfo->MsgRPM = 0;
        pSendInfo->MsgPRIORITY = pRecvInfo->MsgPRIORITY;        
        pSendInfo->MsgIndex = pRecvInfo->MsgIndex;
        pSendInfo->MsgLabel = pRecvInfo->MsgLabel;
        pSendInfo->SourLen = pRecvInfo->SourLen;
        memcpy(pSendInfo->SourAddr,pRecvInfo->SourAddr, pRecvInfo->SourLen);
        pSendInfo->DestLen = pRecvInfo->DestLen;
        memcpy(pSendInfo->DestAddr, pRecvInfo->DestAddr, pRecvInfo->DestLen);
        //msg
        pSendInfo->IID_ID = pRecvInfo->IID_ID;
        pSendInfo->IOP_ID = pRecvInfo->IOP_ID;
    }
    pSendInfo->nPayloadLength = ploffset;
    memcpy(pSendInfo->aPayloadData, payLoad, ploffset);
    
    return 1;
}

/*******************************************************************************
* 函数名称: ModeInfoFrameDeal
* 函数功能: 查询模组信息
* 输入参数: usbNo   槽位
            czBufIn 输入报文
            len     输入长度
            seq     序号
* 输出参数: pOutBuf 模组信息
* 返 回 值: 错误码
*******************************************************************************/
int32   ModeInfoFrameDeal_0(int32 usbNo, uint8 *czBufIn, int16 len,uint8 seq, uint8 *pOutBuf)
{
    uint16 dwOffset = 0;
    MODULE_INF_T infoFrameData = {0};
    
    if(czBufIn[dwOffset] == 0 && czBufIn[dwOffset+1] == ENUM)   //错误信息回码
    {
        MDLMGR_FMT_DEBUG(usbNo,"信息帧回码出错，DAR = %d \n",czBufIn[dwOffset+2]);
        return -1;
    }
    else if(czBufIn[dwOffset++] == 0x01)        //Data
    {
        memset(&infoFrameData,0x00,sizeof(MODULE_INF_T));
        if(czBufIn[dwOffset] != 0x02)           //Data非struncture，类型错误
        {
            return -1;
        }
        dwOffset += 2;
        if(czBufIn[dwOffset++] != VISIBLE_STRING)       //设备型号数据类型错误        
        {
            return -1;
        }
        infoFrameData.DevTypeLen = czBufIn[dwOffset++];
        memcpy(infoFrameData.DevType,&czBufIn[dwOffset],infoFrameData.DevTypeLen);
        MDLMGR_FMT_TRACE(usbNo,"usbNo %d 模块型号：%s\n",usbNo,infoFrameData.DevType);
        dwOffset += infoFrameData.DevTypeLen;
        if(czBufIn[dwOffset++] != VISIBLE_STRING)       //设备ID数据类型错误
        {
            return -1;
        }
        infoFrameData.DevIDLen = czBufIn[dwOffset++];
        memcpy(infoFrameData.DevID,&czBufIn[dwOffset],infoFrameData.DevIDLen);
        MDLMGR_FMT_TRACE(usbNo,"模块ID：%s\n",infoFrameData.DevID);
        dwOffset += infoFrameData.DevIDLen;
        if(czBufIn[dwOffset++] != DOUBLE_LONG_UNSIGNED)     //软件版本数据类型不对
        {
            return -1;
        }
        //memcpy(&infoFrameData.softVersion,&czBufIn[dwOffset],4);
        infoFrameData.softVersion = (czBufIn[dwOffset]<<24)+(czBufIn[dwOffset+1]<<16)+(czBufIn[dwOffset+2]<<8)+(czBufIn[dwOffset+3]);
        MDLMGR_FMT_TRACE(usbNo,"软件版本：%08X\n",infoFrameData.softVersion);
        dwOffset += 4;
        if(czBufIn[dwOffset++] != TDATE)                    //时间数据类型
        {
            return -1;
        }
        //memcpy(&infoFrameData.date_soft.year,&czBufIn[dwOffset],2);
        infoFrameData.date_soft.year = (czBufIn[dwOffset] << 8) + czBufIn[dwOffset + 1];
        infoFrameData.date_soft.month = czBufIn[dwOffset+2];
        infoFrameData.date_soft.dayofmonth = czBufIn[dwOffset+3];
        infoFrameData.date_soft.dayofweek = czBufIn[dwOffset+4];
        MDLMGR_FMT_TRACE(usbNo,"发布时间：%04d-%02d-%02d(周%d)\n",infoFrameData.date_soft.year,infoFrameData.date_soft.month,infoFrameData.date_soft.dayofmonth,infoFrameData.date_soft.dayofweek);
        dwOffset += 5;
        if(czBufIn[dwOffset++] != DOUBLE_LONG_UNSIGNED)     //硬件版本数据类型不对
        {
            return -1;
        }
        //memcpy(&infoFrameData.hardwVersion,&czBufIn[dwOffset],4);
        infoFrameData.hardwVersion = (czBufIn[dwOffset]<<24)+(czBufIn[dwOffset+1]<<16)+(czBufIn[dwOffset+2]<<8)+(czBufIn[dwOffset+3]);
        MDLMGR_FMT_TRACE(usbNo,"硬件版本：%08X\n",infoFrameData.hardwVersion);
        dwOffset += 4;
        if(czBufIn[dwOffset++] != TDATE)                    //时间数据类型
        {
            return -1;
        }
        //memcpy(&infoFrameData.date_hard.year,&czBufIn[dwOffset],2);
        infoFrameData.date_hard.year = (czBufIn[dwOffset] << 8) + czBufIn[dwOffset + 1];
        infoFrameData.date_hard.month = czBufIn[dwOffset+2];
        infoFrameData.date_hard.dayofmonth = czBufIn[dwOffset+3];
        infoFrameData.date_hard.dayofweek = czBufIn[dwOffset+4];
        MDLMGR_FMT_TRACE(usbNo,"发布时间：%04d-%02d-%02d(周%d)\n",infoFrameData.date_hard.year,infoFrameData.date_hard.month,infoFrameData.date_hard.dayofmonth,infoFrameData.date_hard.dayofweek);
        dwOffset += 5;
        if(czBufIn[dwOffset++] != VISIBLE_STRING)           //厂商编码数据类型
        {
            return -1;
        }
        infoFrameData.VendorCodeLen = czBufIn[dwOffset++];  
        memcpy(&infoFrameData.VendorCode,&czBufIn[dwOffset],infoFrameData.VendorCodeLen);
        MDLMGR_FMT_TRACE(usbNo,"厂商编码：%s \n",infoFrameData.VendorCode);
        dwOffset += infoFrameData.VendorCodeLen;
        if(dwOffset != len)
        {
            MDLMGR_FMT_DEBUG(usbNo,"InfoFrameDeal_New数据长度出错,传入长度len %d,解析长度 %d \n",len,dwOffset);
            //目前有模组长度问题，或者新老协议兼容问题，暂时不判断太严格
            //return -1;
        }

        //厂商编码长度，最多4个字节（规范）
        if (infoFrameData.VendorCodeLen > 4)
        {
            infoFrameData.VendorCodeLen = 4;
        }

        memcpy(&moduleInfo_all[usbNo].moduleInfo, &infoFrameData, sizeof(MODULE_INF_T));
        moduleInfo_all[usbNo].modInfoFlag = 0x55;

        if(pOutBuf != NULL)
        {
            memcpy(pOutBuf, &infoFrameData, sizeof(MODULE_INF_T));
        }
    }

    //查询成功，标志清0
    modInfoGetCnt[usbNo] = 0;

    return 1;
}

/*******************************************************************************
* 函数名称: ModeInfoFrameDeal
* 函数功能: 查询模组信息
* 输入参数: usbNo   槽位
            czBufIn 输入报文
            len     输入长度
            seq     序号
* 输出参数: pOutBuf 模组信息
* 返 回 值: 错误码
*******************************************************************************/
int32   ModeInfoFrameDeal_1(int32 usbNo, uint8 *czBufIn, int16 len,uint8 seq, uint8 *pOutBuf)
{
    uint16 wlen = 0;
    uint16 dwOffset = 0;
    MODULE_INF_T infoFrameData = {0};
    
    if(czBufIn[dwOffset] == 0 && czBufIn[dwOffset+1] == ENUM)   //错误信息回码
    {
        MDLMGR_FMT_DEBUG(usbNo,"信息帧回码出错，DAR = %d \n",czBufIn[dwOffset+2]);
        return -1;
    }
    else if(czBufIn[dwOffset++] == 0x01)        //Data
    {
        memset(&infoFrameData, 0x00, sizeof(MODULE_INF_T));
        if(czBufIn[dwOffset] != 0x02)           //Data非struncture，类型错误
        {
            return -1;
        }
        dwOffset += 2;
        
        if(czBufIn[dwOffset++] != VISIBLE_STRING)       //设备型号数据类型错误        
        {
            return -1;
        }
        infoFrameData.DevTypeLen = czBufIn[dwOffset++];
        memcpy(infoFrameData.DevType, &czBufIn[dwOffset], infoFrameData.DevTypeLen);
        MDLMGR_FMT_TRACE(usbNo, "usbNo %d 模块型号：%s\n", usbNo, infoFrameData.DevType);
        dwOffset += infoFrameData.DevTypeLen;
        
        if(czBufIn[dwOffset++] != VISIBLE_STRING)       //设备ID数据类型错误
        {
            return -1;
        }
        infoFrameData.DevIDLen = czBufIn[dwOffset++];
        memcpy(infoFrameData.DevID, &czBufIn[dwOffset], infoFrameData.DevIDLen);
        MDLMGR_FMT_TRACE(usbNo, "模块ID：%s\n", infoFrameData.DevID);
        dwOffset += infoFrameData.DevIDLen;

        if(czBufIn[dwOffset++] != VISIBLE_STRING)           //厂商编码数据类型
        {
            return -1;
        }
        infoFrameData.VendorCodeLen = czBufIn[dwOffset++];  
        memcpy(&infoFrameData.VendorCode, &czBufIn[dwOffset], infoFrameData.VendorCodeLen);
        MDLMGR_FMT_TRACE(usbNo, "厂商编码：%s \n", infoFrameData.VendorCode);
        dwOffset += infoFrameData.VendorCodeLen;

        //厂商编码长度，最多4个字节（规范）
        if (infoFrameData.VendorCodeLen > 4)
        {
            infoFrameData.VendorCodeLen = 4;
        }
        
        if(czBufIn[dwOffset++] != VISIBLE_STRING)     //软件版本数据类型不对
        {
            return -1;
        }
        wlen = czBufIn[dwOffset++];
        memcpy(infoFrameData.softVer, &czBufIn[dwOffset], MIN(wlen, (MAX_CHAR_LEN - 1)));
        MDLMGR_FMT_TRACE(usbNo,"软件版本：%s\n", infoFrameData.softVer);
        dwOffset += wlen;
        
        if(czBufIn[dwOffset++] != VISIBLE_STRING)       //时间数据类型
        {
            return -1;
        }
        wlen = czBufIn[dwOffset++];
        memcpy(infoFrameData.softDate, &czBufIn[dwOffset], MIN(wlen, (MAX_CHAR_LEN - 1)));
        MDLMGR_FMT_TRACE(usbNo,"发布时间：%s\n", infoFrameData.softDate);
        dwOffset += wlen;
        
        if(czBufIn[dwOffset++] != VISIBLE_STRING)         //硬件版本数据类型不对
        {
            return -1;
        }
        wlen = czBufIn[dwOffset++];
        memcpy(infoFrameData.hardVer, &czBufIn[dwOffset], MIN(wlen, (MAX_CHAR_LEN - 1)));
        MDLMGR_FMT_TRACE(usbNo,"硬件版本：%s\n", infoFrameData.hardVer);
        dwOffset += wlen;
        
        if(czBufIn[dwOffset++] != VISIBLE_STRING)       //时间数据类型
        {
            return -1;
        }
        wlen = czBufIn[dwOffset++];
        memcpy(infoFrameData.hardDate, &czBufIn[dwOffset], MIN(wlen, (MAX_CHAR_LEN - 1)));
        MDLMGR_FMT_TRACE(usbNo,"发布时间：%s\n", infoFrameData.hardDate);
        dwOffset += wlen;
        
        if(czBufIn[dwOffset++] != VISIBLE_STRING)           //厂商扩展
        {
            return -1;
        }
        wlen = czBufIn[dwOffset++];
        memcpy(infoFrameData.VendorExt, &czBufIn[dwOffset], MIN(wlen, (MAX_CHAR_LEN - 1)));
        MDLMGR_FMT_TRACE(usbNo,"厂商编码：%s\n", infoFrameData.VendorExt);
        dwOffset += wlen;
        if(dwOffset != len)
        {
            MDLMGR_FMT_DEBUG(usbNo, "InfoFrameDeal_New数据长度出错,传入长度len %d,解析长度 %d \n", len, dwOffset);
            //目前有模组长度问题，或者新老协议兼容问题，暂时不判断太严格
            //return -1;
        }

        //厂商编码长度，最多4个字节（规范）
        if (infoFrameData.VendorCodeLen > 4)
        {
            infoFrameData.VendorCodeLen = 4;
        }

        //标注新协议
        infoFrameData.type = MDL_PRTL_VER1;
        
        memcpy(&moduleInfo_all[usbNo].moduleInfo, &infoFrameData, sizeof(MODULE_INF_T));
        moduleInfo_all[usbNo].modInfoFlag = 0x55;
        
        if(pOutBuf != NULL)
        {
            memcpy(pOutBuf, &infoFrameData, sizeof(MODULE_INF_T));
        }
    }

    //查询成功，标志清0
    modInfoGetCnt[usbNo] = 0;

    return 1;
}

/*******************************************************************************
* 函数名称: ModeInfoFrameDeal
* 函数功能: 查询模组信息
* 输入参数: usbNo   槽位
            czBufIn 输入报文
            len     输入长度
            seq     序号
* 输出参数: pOutBuf 模组信息
* 返 回 值: 错误码
*******************************************************************************/
int32 ModeInfoFrameDeal(int32 usbNo, uint8 *czBufIn, int16 len,uint8 seq, uint8 *pOutBuf)
{
    uint16 dwOffset = 0;
    MODULE_INF_T infoFrameData;
    
    if(czBufIn[dwOffset] == 0 && czBufIn[dwOffset+1] == ENUM)   //错误信息回码
    {
        MDLMGR_FMT_DEBUG(usbNo,"信息帧回码出错，DAR = %d \n",czBufIn[dwOffset+2]);
        return -1;
    }
    else if(czBufIn[dwOffset++] == 0x01)        //Data
    {
        memset(&infoFrameData,0x00,sizeof(MODULE_INF_T));
        if(czBufIn[dwOffset++] != 0x02)           //Data非struncture，类型错误
        {
            return -1;
        }

        if (czBufIn[dwOffset] == 0x07)
        {
            return ModeInfoFrameDeal_0(usbNo, czBufIn, len, seq, pOutBuf);
        }
        else if (czBufIn[dwOffset] == 0x08)
        {
            return ModeInfoFrameDeal_1(usbNo, czBufIn, len, seq, pOutBuf);
        }
        else
        {
            MDLMGR_FMT_DEBUG(usbNo,"信息帧回码出错，信息内容数量 = %d \n",czBufIn[dwOffset]);
        }
    }

    return -2;
}

int32 LinkInfoFrameDeal(int32 usbNo, uint8 *czBufIn, int16 len,uint8 seq)
{
    uint16 dwOffset = 0;
    int32 ret = 0;
    BOOL  isIncludeVipeCnt = 0;

    if(czBufIn[dwOffset] == 0 && czBufIn[dwOffset+1] == ENUM)   //错误信息回码
    {
        MDLMGR_FMT_DEBUG(usbNo, "链路帧回码出错，出错码DAR = %d \n",czBufIn[dwOffset+2]);
        ret = -1;
    }
    else if(czBufIn[dwOffset++] == 0x01)        //Data
    {
        if(czBufIn[dwOffset++] != 0x02)         //Data非struncture，类型错误
        {
            return -1;
        }
        if(czBufIn[dwOffset++] == 8)            //兼容老版本协议
        {
            isIncludeVipeCnt = 1;
        }
        
        ret = GetNegotiateDeal(usbNo,&czBufIn[dwOffset], len-dwOffset,isIncludeVipeCnt);
    }
    return ret;
}

int32 FileTransfState(int32 usbNo, uint8 *czBufIn, int16 len,uint8 seq,uint8 *pOutBuf)
{
    uint16 dwOffset = 0;
    uint8  tempDataBuf[1024];
    uint32  allPackCnt = 0;
    uint16  bytesCnt = 0;
    
    //FILE_TRANS_STATE_T *pfileState = &moduleInfo_all[usbNo].fileState;

    if(czBufIn[dwOffset] == 0 && czBufIn[dwOffset+1] == ENUM)   //错误信息回码
    {
        MDLMGR_FMT_DEBUG(usbNo, "文件传输状态回码出错，出错码DAR = %d \n",czBufIn[dwOffset+2]);
        return -1;
    }
    else if(czBufIn[dwOffset++] == 0x01)        //Data
    {   
        if(czBufIn[dwOffset] != STRUCTURE)          //Data非struncture，类型错误
        {
            return -1;
        }
        dwOffset += 2;
        if(czBufIn[dwOffset++] != DOUBLE_LONG_UNSIGNED)     
        {
            return -1;
        }

        memcpy_r(&allPackCnt,&czBufIn[dwOffset],4);
        dwOffset += 4;
        if(czBufIn[dwOffset++] != BIT_STRING)
        {
            return -1;
        }
        if(czBufIn[dwOffset] & 0x80 )
        {
            if(czBufIn[dwOffset] == 0x81)
            {
                //长度为单字节
                bytesCnt = czBufIn[dwOffset+1];
                dwOffset += 2;
            }
            else if(czBufIn[dwOffset] == 0x82)
            {

                bytesCnt = czBufIn[dwOffset+1]*256 + czBufIn[dwOffset+2];
                dwOffset += 3;
            }
        }
        else
        {
            bytesCnt = czBufIn[dwOffset];
            dwOffset++;
        }
        memcpy_r(tempDataBuf,&allPackCnt,sizeof(uint32));

        memcpy(&tempDataBuf[4],&czBufIn[dwOffset],bytesCnt);    //位串

        bytesCnt = bytesCnt/8 + ((bytesCnt%8) ? 1:0);       //位串字节数
        dwOffset += bytesCnt;
        if(dwOffset != len)
        {
            MDLMGR_FMT_DEBUG(usbNo, "传入的长度len %d 与解析长度dwoffset %d不符\n",len,dwOffset);
            return -1;
        }
        
        //memcpy(pfileState,&fileStateTemp,sizeof(FILE_TRANS_STATE_T));
        if(pOutBuf != NULL)
        {
            memcpy(pOutBuf,&tempDataBuf,bytesCnt+4);
        }
        MDLMGR_FMT_TRACE(usbNo,"bytesCnt = %d\n",bytesCnt);

    }
    return 1;
}

int32 PowerStateDeal(int32 usbNo, uint8 *czBufIn, int16 len,uint8 seq)
{
    uint16 dwOffset = 0;
    uint8   *pPower = (uint8*)&moduleInfo_all[usbNo].powerSta;
    
    if(czBufIn[dwOffset] == 0 && czBufIn[dwOffset+1] == ENUM)   //错误信息回码
    {
        MDLMGR_FMT_DEBUG(usbNo, "电源状态回码出错，出错码DAR = %d \n",czBufIn[dwOffset+2]);
        return -1;
    }
    else if(czBufIn[dwOffset++] == 0x01)        //Data
    {
        if(czBufIn[dwOffset++] == ENUM)         //非枚举类型
        {
            return -1;
        }

        if(czBufIn[dwOffset] != powerOn && czBufIn[dwOffset] != powerOff)
        {
            MDLMGR_FMT_DEBUG(usbNo, "错误的电源状态值：%d \n",czBufIn[dwOffset]);
            return -1;
        }
        
        *pPower = czBufIn[dwOffset];
        dwOffset++;

        if(dwOffset != len)
        {
            return -1;
        }
    }
    return 1;

}

int32 ProtocolVerDeal(int32 usbNo, uint8 *czBufIn, int16 len,uint8 seq)
{
    uint16 dwOffset = 0;
    uint16  *pProtocol = &moduleInfo_all[usbNo].protocolVer;
    
    if(czBufIn[dwOffset] == 0 && czBufIn[dwOffset+1] == ENUM)   //错误信息回码
    {
        MDLMGR_FMT_DEBUG(usbNo, "协议版本回码出错，出错码DAR = %d \n",czBufIn[dwOffset+2]);
        return -1;
    }
    else if(czBufIn[dwOffset++] == 0x01)        //Data
    {
        if(czBufIn[dwOffset++] != LONG_UNSIGNED)
        {
            return -1;
        }
        
        memcpy(pProtocol,&czBufIn[dwOffset],2);
        dwOffset += 2;

        if(dwOffset != len)
        {
            return -1;
        }
    }
    return 1;

}

int32 GetClockDeal(int32 usbNo, uint8 *czBufIn, int16 len,uint8 seq)
{
    uint16 dwOffset = 0;
    Date_Time_t *pClock = &moduleInfo_all[usbNo].clock;
    
    if(czBufIn[dwOffset] == 0 && czBufIn[dwOffset+1] == ENUM)   //错误信息回码
    {
        MDLMGR_FMT_DEBUG(usbNo, "查询时钟回码出错，出错码DAR = %d \n",czBufIn[dwOffset+2]);
        return -1;
    }
    else if(czBufIn[dwOffset++] == 0x01)        //Data
    {
        if(czBufIn[dwOffset++] != DATE_TIME)
        {
            return -1;
        }

        memcpy(pClock,&czBufIn[dwOffset],sizeof(Date_Time_t));
        dwOffset += sizeof(Date_Time_t);

        if(dwOffset != len)
        {
            return -1;
        }
    }
    return 1;
}

int32   GetYXState(uint8 DTA,uint8 DTB,int32 usbNo, uint8 *czBufIn, int16 len,uint8 seq)
{
    uint16 dwOffset = 0;
    uint16 DT = DTA;
    //REMOTE_PULSE_T *pYXState = &YX_pulse_info;
    DT = (DT<<8) | DTB;
    int i;

    if(czBufIn[dwOffset] == 0 && czBufIn[dwOffset+1] == ENUM)   //错误信息回码
    {
        MDLMGR_FMT_DEBUG(usbNo, "查询遥信DTA %d,DTB %d,回码出错，出错码DAR = %d \n",DTA,DTB,czBufIn[dwOffset+2]);
        return -1;
    }
    dwOffset++;
    
    switch (DT)
    {
        case 0x4000:        //遥信状态
            {
            REMOTE_STATE_T *pRemote_state = &YX_pulse_info.remote_state;
            if(czBufIn[dwOffset] != STRUCTURE)
            {
                return -1;
            }
            dwOffset += 2;
            if(czBufIn[dwOffset] != BIT_STRING)
            {
                return -1;
            }
            dwOffset += 2;
            if(czBufIn[dwOffset++] != 0x04)         //遥信路数
            {
                return -1;
            }
            pRemote_state->YX_state = czBufIn[dwOffset++];      //遥信状态
            if(czBufIn[dwOffset] != BIT_STRING)
            {
                return -1;
            }
            dwOffset += 2;
            if(czBufIn[dwOffset++] != 0x04)         //遥信路数
            {
                return -1;
            }
            pRemote_state->YX_change = czBufIn[dwOffset++];     //变位状态
            if(czBufIn[dwOffset++] != ARRAY)
            {
                return -1;
            }
            if(czBufIn[dwOffset++] != 0x04)         //数组长度
            {
                return -1;
            }

            for(i = 0; i < 4; i++)
            {
                if(czBufIn[dwOffset++] != DATE_TIME)
                {
                    return -1;
                }
                memcpy(&pRemote_state->date_time[i],&czBufIn[dwOffset],sizeof(Date_Time_t));
                dwOffset += sizeof(Date_Time_t);
            }
            break;
            }
        case 0x4001:        //上一次脉冲计数和时间
        case 0x4002:        //脉冲计数和时间
            {
            PULSE_CNT_T * pPulseCnt;
            if(DT == 0x4001)
            {
                pPulseCnt = &YX_pulse_info.pulse_cnt_last;
            }
            else if(DT == 0x4002)
            {
                pPulseCnt = &YX_pulse_info.pulse_cnt;
            }
             
            if(czBufIn[dwOffset++] != ARRAY)
            {
                return -1;
            }
            if(czBufIn[dwOffset++] != 0x04)     //对应4路遥信
            {
                return -1;
            }
            for(i = 0; i < 4; i++)
            {
                if(czBufIn[dwOffset] == NULL_DATA)
                {
                    dwOffset++;                 
                }
                else if(czBufIn[dwOffset] == STRUCTURE)
                {
                    dwOffset += 2;      //跳过结构体数据项数目
                    if(czBufIn[dwOffset++] != DOUBLE_LONG_UNSIGNED)
                    {
                        return -1;
                    }
                    else
                    {
                        memcpy(&pPulseCnt->pulseCnt[i],&czBufIn[dwOffset],4);
                        dwOffset += 4;
                    }

                    if(czBufIn[dwOffset++] != DOUBLE_LONG_UNSIGNED)
                    {
                        return -1;
                    }
                    else
                    {
                        memcpy(&pPulseCnt->time[i],&czBufIn[dwOffset],4);
                        dwOffset += 4;
                    }
                }
            }
            break;
            }
        case 0x4100:        //遥信脉冲功能配置
            if(czBufIn[dwOffset] != BIT_STRING)
            {
                return -1;
            }
            dwOffset += 3;      //跳过0x81 0x04
            YX_pulse_info.YX_FuncSet = czBufIn[dwOffset++];
            
            break;

        case 0x4101:        //遥信防抖时间
            if(czBufIn[dwOffset++] != LONG_UNSIGNED)
            {
                return -1;
            }
            memcpy(&YX_pulse_info.YX_AntiShakeTime,&czBufIn[dwOffset],2);
            dwOffset += 2;
            break;

        default:

            break;
    }
    return 1;
}

int GetRelayInfo(uint8 DTA,uint8 DTB,int32 usbNo, uint8 *czBufIn, int16 len,uint8 seq)
{
    uint16 dwOffset = 0;
    uint16 DT = DTA;
    //REMOTE_PULSE_T *pYXState = &YX_pulse_info;
    DT = (DT<<8) | DTB;
    

    if(czBufIn[dwOffset] == 0 && czBufIn[dwOffset+1] == ENUM)   //错误信息回码
    {
        MDLMGR_FMT_DEBUG(usbNo, "查询继电器回码出错，出错码DAR = %d \n",czBufIn[dwOffset+2]);
        return -1;
    }
    dwOffset++;

    if(DT == 0x7100)
    {
        if(czBufIn[dwOffset++] != ENUM)
        {
            return -1;
        }
        else
        {
            relayMode = (RELAY_OUTMODE_E)czBufIn[dwOffset++];
        }
    }
    else if(DT == 0x7101)
    {
        if(czBufIn[dwOffset++] != LONG_UNSIGNED)
        {
            return -1;
        }
        else
        {
            memcpy(&pulseWidth,&czBufIn[dwOffset],2);
            dwOffset += 2;
        }
    }
    return 1;
}

/***********************************************************************
 * @brief   信息数据帧处理
 * @param[in]  
 *
 * @retval  > 0 需要回码处理
  *         = 0 需要确认帧
            < 0 不需要回码处理
 **********************************************************************/
int32 InfoFrameDeal_New(int32 usbNo, uint8 *czBufIn, int16 len,uint8 seq,uint8 *pOutBuf)
{
    uint16 dwOffset = 0;
    
    uint8 DTA,DTB;
    //MODULE_INF_T infoFrameData;
    //INFO_DATA_T infodataSend,infodataRecv;
    //printf("%02x,%02x,%02x\n",czBufIn[0],czBufIn[1],czBufIn[2]);
    if(czBufIn[dwOffset++] != 0x82)     //非信息查询帧回码
    {
        return -1;
    }
    DTA = czBufIn[dwOffset++];
    DTB = czBufIn[dwOffset++];
    if(DTA == 0x00)
    {
        switch (DTB)
        {
            case 0x00:              //查询模块信息回码
                return ModeInfoFrameDeal(usbNo, &czBufIn[dwOffset], len-dwOffset,seq,pOutBuf);
                //break;

            case 0x01:              //查询链路信息回码
                return LinkInfoFrameDeal(usbNo, &czBufIn[dwOffset], len-dwOffset,seq);
                //break;

            case 0x02:              //协议版本回码
            {
                return ProtocolVerDeal(usbNo, &czBufIn[dwOffset], len-dwOffset,seq);
            }
                //break;

            case 0x03:              //电源状态回码
                return PowerStateDeal(usbNo, &czBufIn[dwOffset], len-dwOffset,seq);
                //break;

            case 0x04:              //文件传输状态回码
                return FileTransfState(usbNo, &czBufIn[dwOffset], len-dwOffset,seq,pOutBuf);
                //break;

            default:
                MDLMGR_FMT_TRACE(usbNo, "InfoFrameDeal_New错误的DTB %d\n",DTB);
                break;
        }
    }
    else if(DTA == 0x01 && DTB == 0x00)     //查询时钟的回码
    {
        return GetClockDeal(usbNo, &czBufIn[dwOffset], len-dwOffset,seq);
    }

    if(DTA == 0x40 || DTA == 0x41)      //遥信相关
    {
        return GetYXState(DTA,DTB,usbNo, &czBufIn[dwOffset], len-dwOffset,seq);
    }

    if(DTA == 0x71)         //继电器
    {
        return GetRelayInfo(DTA,DTB,usbNo, &czBufIn[dwOffset], len-dwOffset,seq);
    }

    return -1;
    
}

int32 SetFrameDeal(int32 usbNo, uint8 *czBufIn, int16 len,uint8 seq,BOOL isupdate)
{
    uint8   result;
    uint8   DTA,DTB;
    uint16  DT;
    uint8   dwOffset = 0;
    //MODULE_INF_T infoFrameData;
    INFO_DATA_T infodataSend ;
    
    int ret = -1;

    memset(&infodataSend,0x0,sizeof(INFO_DATA_T));
    
    if(czBufIn[dwOffset++] != 0x83)
    {
        return -1;
    }
    DTA = czBufIn[dwOffset++];
    DTB = czBufIn[dwOffset++];
    result = czBufIn[dwOffset++];
    DT = DTA;
    DT = (DT<<8)|DTB;
    if(dwOffset != len)
    {
        MDLMGR_FMT_DEBUG(usbNo, "SetFrameDeal解析数据长度%d 与传入len %d不符\n",dwOffset,len);
        return -1;
    }
    switch (DT)
    {
        case 0x0100:        //时钟
            return (result == 0)? 1: 0;
            //break;
        case 0x0200:        //硬件初始化
        case 0x0201:
        case 0x0202:
        case 0x4100:
        case 0x4101:
        case 0x7100:
        case 0x7101:
            //根据收到的消息组回复消息结构体
            if(DT != 0x0200 || (!isupdate))
            {
                ret = MDLReadInfo(&InfoDataQueue, seq, &infodataSend);
                if(ret < 0)
                {
                    MDLMGR_FMT_DEBUG(usbNo, "SetFrameDeal:DT = %04x,未读到缓存mqtt message head,mqtt回复失败!\n",DT);
                }
                else
                {
                    infodataSend.MsgRPM = 0;                            //非启动
                    infodataSend.nPayloadLength = 1;
                    infodataSend.aPayloadData[0] = ((result == 0)?1:0);
                    //写入上报队列
                    PlcWriteQueue(&LockRecvBuf, (uint8*)&infodataSend, sizeof(INFO_DATA_T));
                }
            }           
            MDLMGR_FMT_TRACE(usbNo, "SetFrameDeal设置回码 DT %04x,执行结果 %d\n",DT,result);
            return result;
            break;

        case 0x0203:        //文件传输回码
            return result;
            //break;

        default:
            break;
    }
    
    return -1;
}


int32 MakeFrameOfNGTGet(uint8* sendBuf)
{
    MDL_FRAME_INFO_T srcframeInfo;
    uint8 dataBuf[100] = {0};
    memset(&srcframeInfo,0x0,sizeof(MDL_FRAME_INFO_T));
    
    srcframeInfo.cntl.bit.ctlcode = INF_F;  //信息帧
    srcframeInfo.cntl.bit.prm = 1;          //发起请求
    srcframeInfo.cntl.bit.dir = 0;          //终端发起
    srcframeInfo.seq =globleSeq++;
    srcframeInfo.frameFunc = GETREQUEST;   //链路连接请求
    srcframeInfo.dataLen = 3;

    dataBuf[0] = srcframeInfo.frameFunc;
    dataBuf[1] = 0x00;
    dataBuf[2] = 0x01;
    memcpy(srcframeInfo.pDataBuf,dataBuf,srcframeInfo.dataLen);
    return MakeModeFrame(&srcframeInfo, sendBuf);

}


/***********************************************************************
 * @brief   组织协商帧
 * @param[in]  
 *
 * @retval  > 0 
 *          < 0 
 **********************************************************************/
int32 MakeFrameOfNGT(uint8 *sendBuf)
{
    MDL_FRAME_INFO_T srcframeInfo;
    LINKREQUEST_T    linkrequest;
    memset(&srcframeInfo,0x0,sizeof(MDL_FRAME_INFO_T));
    memset(&linkrequest,0x0,sizeof(LINKREQUEST_T));
    
    srcframeInfo.cntl.bit.ctlcode = INF_F;  //信息帧
    srcframeInfo.cntl.bit.prm = 1;          //发起请求
    srcframeInfo.cntl.bit.dir = 0;          //终端发起
    srcframeInfo.seq =globleSeq++;
    srcframeInfo.frameFunc = LINKREQUEST;   //链路连接请求
    
    linkrequest.prtlVersion = 0;
    strcpy(linkrequest.tmnMode, "NCXXXX-XXXX");
    linkrequest.tmnModeLen = strlen(linkrequest.tmnMode);
    strcpy(linkrequest.tmnID, "0000000001");
    linkrequest.tmnIDLen = strlen(linkrequest.tmnID);
    linkrequest.sendBufLen = 512;
    linkrequest.recvBufLen = 512;
    linkrequest.conCurrWDs = 1;
    
    //组链路协商apdu
    uint16 dataIndex = 0;
    srcframeInfo.pDataBuf[dataIndex++] = srcframeInfo.frameFunc;

    memcpy_r(&srcframeInfo.pDataBuf[dataIndex], &linkrequest.prtlVersion, 2);
    dataIndex += 2;

    srcframeInfo.pDataBuf[dataIndex++] = linkrequest.tmnModeLen;
    memcpy(&srcframeInfo.pDataBuf[dataIndex], linkrequest.tmnMode, linkrequest.tmnModeLen);
    dataIndex += linkrequest.tmnModeLen;

    srcframeInfo.pDataBuf[dataIndex++] = linkrequest.tmnIDLen;
    memcpy(&srcframeInfo.pDataBuf[dataIndex], linkrequest.tmnID, linkrequest.tmnIDLen);
    dataIndex += linkrequest.tmnIDLen;

    memcpy_r(&srcframeInfo.pDataBuf[dataIndex], &linkrequest.sendBufLen, 2);
    dataIndex += 2;

    memcpy_r(&srcframeInfo.pDataBuf[dataIndex], &linkrequest.recvBufLen, 2);
    dataIndex += 2;

    srcframeInfo.pDataBuf[dataIndex++] = linkrequest.conCurrWDs;

    srcframeInfo.dataLen = dataIndex;

    return MakeModeFrame(&srcframeInfo, sendBuf);

}

int32 GetmoduleClock(int32 usbNo)
{
    if(!clockSet[usbNo])
    {
        return 0;
    }
    if(time(NULL) - clockSet[usbNo] < 60*59)    //第59分查模组时钟，测试模块软时钟精度
    {
        return 0;
    }
    MDLMGR_FMT_DEBUG(mdlnum, "模块%d产生模组对时任务5秒后,查询该模组时钟!\n",usbNo);
    TRANS_INFO_T SendAppData;
    memset(&SendAppData,0x00,sizeof(TRANS_INFO_T));
    SendAppData.DTA = 0x01;
    SendAppData.DTB = 0x00;
    SendAppData.seq = globleSeq++;
    clockSet[usbNo] = 0;
    PlcWriteQueue(&LockSendBuf[usbNo], (uint8*)&SendAppData, sizeof(TRANS_INFO_T));
    return 1;
}

//模组请求帧的响应
int32 AskLocalInfo(int32 usbNo, uint8 *czBufIn, int16 len,uint8 seq)
{
    uint8   DTA,DTB;
    uint16  DT;
    uint8   dwOffset = 0;
    INFO_DATA_T infodataSend ;
    TRANS_INFO_T SendAppData;

    memset(&infodataSend,0x0,sizeof(INFO_DATA_T));
    if(czBufIn[dwOffset++] != 0x02)
    {
        return -1;
    }
    DTA = czBufIn[dwOffset++];
    DTB = czBufIn[dwOffset++];
    
    DT = DTA;
    DT = (DT<<8)|DTB;
    if(dwOffset != len)
    {
        MDLMGR_FMT_DEBUG(usbNo, "SetFrameDeal解析数据长度%d 与传入len %d不符\n",dwOffset,len);
        return -1;
    }
    switch (DT)
    {
        case 0x0100:        //模块请求时钟
            memset(&SendAppData,0x00,sizeof(TRANS_INFO_T));
            SendAppData.DTA = DTA;
            SendAppData.DTB = DTB;              
            SendAppData.seq = seq;
            SendAppData.clkSetFlag = 1;     //设置时钟标志
            clockSet[usbNo] = time(NULL);
            PlcWriteQueue(&LockSendBuf[usbNo], (uint8*)&SendAppData, sizeof(TRANS_INFO_T));
            MDLMGR_FMT_DEBUG(usbNo, "收到模块%d对时请求，产生对时任务\n", usbNo);
            return 1;
            break;

        default:
            break;
    }
    
    return -1;

}


/***********************************************************************
 * @brief    模组报文解析
 * @param[in]  usbNo           USB号   
 *             desFrameInfo    报文信息
 *             pOutBuf         返回内容
 *             isupdate        特殊标记
 * @retval  > = 0       成功
 *          < 0         失败
 **********************************************************************/
int32 AnalysisFrameOfModule(int32 usbNo, MDL_FRAME_INFO_T *desFrameInfo, uint8 *pOutBuf,BOOL isupdate)
{
    int16  dwOffset = 0;
    uint16 dataLen = 0;
    uint8  cmd = 0;
    uint8  frameFunc = 0;
    int32  nRet = -1;

    dwOffset = GetStartAddrOfFrame(desFrameInfo->pDataBuf, desFrameInfo->dataLen);
    
    if (dwOffset < 0)
    {
        MDLMGR_FMT_DEBUG(usbNo, "报文格式或者校验不正确\n");
        return -1;
    }

    memcpy((uint8 *)&dataLen, &desFrameInfo->pDataBuf[dwOffset+1], 2);
    cmd = desFrameInfo->pDataBuf[dwOffset+3 ];
    desFrameInfo->seq = desFrameInfo->pDataBuf[dwOffset+4];
    frameFunc = desFrameInfo->pDataBuf[dwOffset+ 5 ];
    
    MDLMGR_FMT_TRACE(usbNo, "报文格式和校验正确 cmd = %2x\n", cmd);
    switch(cmd&0x0f)
    {
        // 确认帧
        case 0x00:
            nRet =0;
            break;
    
        // 否认帧
        case 0x01:
            
            break;

        // 信息帧
        case 0x02:
            if(frameFunc == LINKRESPONSE)
            {   
                nRet = NegotiateDeal(usbNo, &desFrameInfo->pDataBuf[dwOffset+D_SITE_M + 1], dataLen-1);
                MDLMGR_FMT_TRACE(usbNo, "链路协商结果nRet = %d\n",nRet);
            }
            else if(frameFunc == GETRESPONSE)
            {   
                nRet = InfoFrameDeal_New(usbNo, &desFrameInfo->pDataBuf[dwOffset+D_SITE_M], dataLen, desFrameInfo->seq, pOutBuf);
                MDLMGR_FMT_TRACE(usbNo, "信息查询cmd = %02x ，结果nRet = %d\n",cmd,nRet);
            }
            else if(frameFunc == SETRESPONSE)
            {
                nRet = SetFrameDeal(usbNo, &desFrameInfo->pDataBuf[dwOffset+D_SITE_M], dataLen, desFrameInfo->seq, isupdate);
            }
            else if(frameFunc == GETREQUEST)    //模组请求
            {
                nRet = AskLocalInfo(usbNo, &desFrameInfo->pDataBuf[dwOffset+D_SITE_M], dataLen, desFrameInfo->seq);
            }
            break;
        // 保留            
        default:

            break;
    }

    // 判断是不是有其他报文
    if ((dataLen + 8) < (desFrameInfo->dataLen - dwOffset))
    {
        MDL_FRAME_INFO_T mdlFrameInfo;
        memset(&mdlFrameInfo, 0, sizeof(MDL_FRAME_INFO_T));
        mdlFrameInfo.dataLen = desFrameInfo->dataLen - dwOffset - dataLen - 8;
        memcpy(mdlFrameInfo.pDataBuf, &desFrameInfo->pDataBuf[dwOffset + dataLen + 8], mdlFrameInfo.dataLen);

        MDLMGR_FMT_DEBUG(usbNo, "可能发生粘包现象，寻找下一个报文\n");

        // 先判断是否是有效报文
        dwOffset = GetStartAddrOfFrame(mdlFrameInfo.pDataBuf, mdlFrameInfo.dataLen);
        if (dwOffset < 0)
        {
            MDLMGR_FMT_DEBUG(usbNo, "报文格式或者校验不正确\n");
        }
        else
        {
            return AnalysisFrameOfModule(usbNo, &mdlFrameInfo, pOutBuf, isupdate);
        }
    }   
    
    return nRet;
}

/***********************************************************************
 * @brief   删除对应控制口的软连接
 * @param[in]  usbNo     : 对应的USB口
 *
 * @retval  无
 **********************************************************************/
void DelLn(int32 usbNo, USB_STATE_E *usbStatusLst)
{
    char cmd[50] = {0};
    uint8 i = 0;

    LINKRESPONSE_T *msg;
    msg = &moduleInfo_all[usbNo].linkInfo;
    moduleInfo_all[usbNo].modInfoFlag = 0x0;
    if(msg->modMode[0] == 0xff)
    {
        MDLMGR_FMT_DEBUG(usbNo, "MDL_NGT_INFO_USB[%d]没有相应的信息\n", usbNo);
    }
    modEventFlag[usbNo] = 0;
    //容错，防止漏删,狂删
    if (msg->virPipeCnt == 0 || msg->virPipeCnt == 0xff)
    {
        msg->virPipeCnt = 8;
    }

    for (i = 0; i < msg->virPipeCnt; i++)
    {
        memset(&cmd[0], 0, 50);
        sprintf(cmd, "rm -rf %s/usb_exm%d_%d", LinkDir,(usbNo + 1), i);
        system(cmd);
        MDLMGR_FMT_DEBUG(usbNo, "DelLn %s\n", cmd);
    }

    *usbStatusLst = UNKNOW_STATE;

    return;
}

/***********************************************************************
 * @brief   模组管理器识别线程
 * @param[in]  usbNo : 槽位号0-4
 *
 * @retval  无
 **********************************************************************/
void RecognizeModuleDeal(int32 usbNo)
{
    int32 bRet = 0;
    TRANS_INFO_T    transInfo;
    uint8 sendDataBuf[200];
    uint8 dataBufOut[2048];
    int32 sendDataLen = 0;
    char  barStr[100] = {0};
    MDL_FRAME_INFO_T desFrameInfo;

    MODULE_STATE_E em_status = IDLE_STATE;
    MODULE_STATE_E em_status_temp = IDLE_STATE;
    USB_STATE_E usbStatus = UNKNOW_STATE;
    
    uint8 cntFailInit = 0;
    uint8 cntSuccInit = 0;
    uint8 negotialCnt = 0;          //协商失败次数
    int i;

    slotInfo[usbNo].usbNo = -1;     //初始化槽位信息
    ClearNgtMsg(usbNo);

    INIT_FMT_DEBUG("模组管理器识别线程启动 usbNo[%d]\n", usbNo);
    
    while (1)
    {
        g_flag_thread_msg[usbNo] = 1;    // 线程守护标志及通道的信息

        // 根据USB有没有插拔来确定处理步骤
        em_status_temp = FixMdlStatus(usbNo, &usbStatus, &cntFailInit, &cntSuccInit);

        // 未插入并状态空闲,直接空转
        if (usbStatus != ON_STATE && em_status_temp == IDLE_STATE)
        {
            sleep(1);
            continue;
        }
        if (em_status_temp != IDLE_STATE)
        {
            em_status = em_status_temp;
        }

        //模组升级，暂停管理线程
        if ((MdlUpdateDeal != MDL_UPDATE_IDLE) && g_UpdateUsbNo == usbNo)
        {
            em_status = WAIT_STATE;
        }
        
        // 根据USB具体的处理状态执行散转操作
        switch (em_status)
        {
        case INIT_STATE:        // 初始化模块虚拟管理串口
            {
                negotialCnt = 0;
                memset(&LockSendBuf[usbNo],0x0,sizeof(LockQueue_T));
                gCtrlDev[usbNo] = module_ctrl_comm_init(usbNo);
                if (gCtrlDev[usbNo] == 0)
                {
                    cntFailInit = 0;
                    cntSuccInit ++;
                    MDLMGR_FMT_TRACE(usbNo, "模组管理通道打开成功. retMdlComm[%d]. usbNo[%d]. cntSuccInit[%d]\n", 
                        gCtrlDev[usbNo], usbNo, cntSuccInit);
                    em_status = NGTT_SEND_STATE;
                }
                else
                {
                    cntFailInit ++;
                    MDLMGR_FMT_DEBUG(usbNo, "模组管理通道打开失败. retMdlComm[%d]. usbNo[%d]. cntFailInit[%d]\n", 
                        gCtrlDev[usbNo], usbNo, cntFailInit);
                    sleep(1);
                }

                //做个容错，多次初始化失败或者初始化次数过多，删除之前的软连接，usbStatus状态也初始化
                if ((cntFailInit > 3) || (cntSuccInit > 6))
                {
                    DelLn(usbNo, &usbStatus);

                    //多次初始化失败，重启模组
                    MDLMGR_FMT_DEBUG(usbNo, "cntFailInit[%d]. cntSuccInit[%d]. 多次初始化失败，重启模组[%d]!\n", 
                        cntFailInit, cntSuccInit, usbNo + 1);
                    if (gCtrlDev[usbNo] == 0)
                    {
                        module_ctrl_comm_uninit(usbNo);
                        gCtrlDev[usbNo] = -1;
                    }

                    cntFailInit = 0;
                    cntSuccInit = 0;

                    if (ModulePowerOff(usbNo + 1, 0) < 0)
                    {
                        MDLMGR_FMT_DEBUG(usbNo, "模组[%d]重启失败!\n", usbNo + 1);
                    }
                    modPowerCtl[usbNo].offpowerflag = 1;
                    modPowerCtl[usbNo].offpowertime = 10;
                    em_status = IDLE_STATE;
                    em_status_temp = IDLE_STATE;
                }
                break;
            }
        case NGTT_SEND_STATE:        // 进行链路协商
            {
                modEventFlag[usbNo] = 0;
                if(negotialCnt <= 2)
                {
                    sendDataLen = MakeFrameOfNGT(sendDataBuf);
                }
                else if (negotialCnt <= 10)
                {   
                    //组链路查询帧
                    sendDataLen = MakeFrameOfNGTGet(sendDataBuf);
                }
                else
                {
                    em_status = CLOSE_STATE;
                    MDLMGR_FMT_DEBUG(usbNo, "模组协商失败 negotialCnt[%d]\n", negotialCnt);
                    break;
                }
                
                bRet = SendData(usbNo, sendDataBuf, sendDataLen);
                if (sendDataLen <= 0 || bRet < 0)
                {
                    MDLMGR_BUF_DEBUG(usbNo, sendDataBuf, sendDataLen, "数据发送失败, sendDataLen[%d], bRet[%d], sendDataBuf:\n", sendDataLen, bRet);
                    em_status = CLOSE_STATE;
                    break;
                }

                MDLMGR_BUF_TRACE(usbNo, sendDataBuf, sendDataLen, "模组链路协商报文发送:");
                
                em_status = NGTT_RECV_STATE;
                break;
            }
        case NGTT_RECV_STATE:        // 协商接收
            {
                memset((uint8 *)&desFrameInfo, 0x00, sizeof(MDL_FRAME_INFO_T));
                desFrameInfo.dataLen = RecvData(usbNo, desFrameInfo.pDataBuf, sizeof(desFrameInfo.pDataBuf));

                MDLMGR_BUF_TRACE(usbNo, desFrameInfo.pDataBuf, desFrameInfo.dataLen, "模组链路协商报文接收:");

                bRet = AnalysisFrameOfModule(usbNo, &desFrameInfo, dataBufOut,0);
                if (bRet >= 0)
                {
                    // 链路识别结束, 映射数据端口
                    if (LnUsbDataComm(usbNo) < 0)
                    {
                        em_status = NGTT_SEND_STATE;
                    }
                    else
                    {   
                        negotialCnt = 0;
                        moduleInfo_all[usbNo].modInfoFlag = 0xAA;

                        //查询模组信息
                        TRANS_INFO_T transinfo;
                        memset(&transinfo, 0x00, sizeof(TRANS_INFO_T));
                        transinfo.seq = globleSeq++;
                        PlcWriteQueue(&LockSendBuf[usbNo], (uint8*)&transinfo, sizeof(TRANS_INFO_T));
                        MDLMGR_FMT_DEBUG(usbNo, "查询模组[%d]信息!\n", usbNo + 1);
                        em_status = SEND_STATE;
                    }               
                }
                else
                {
                    negotialCnt++;
                    em_status = NGTT_SEND_STATE;
                }
                break;
            }
        case SEND_STATE:        // 正常数据发送
            {
                if(moduleInfo_all[usbNo].modInfoFlag != 0x55)
                {
                    modCnt[usbNo]++;
                    if(modCnt[usbNo] > 10)
                    {
                        modCnt[usbNo] = 0;
                        
                        //查询模组信息
                        TRANS_INFO_T transinfo;
                        memset(&transinfo,0x00,sizeof(TRANS_INFO_T));

                        //现场出现模组查询模组信息不回现象，这里做个容错，达到一定次数后重启
                        modInfoGetCnt[usbNo]++;
                        if (modInfoGetCnt[usbNo] > 5)
                        {
                            modInfoGetCnt[usbNo] = 0;
                            if (ModulePowerOff(usbNo + 1, 0) < 0)
                            {
                                MDLMGR_FMT_DEBUG(usbNo, "模组[%d]重启失败\n", usbNo + 1);
                            }
                        }

                        transinfo.seq = globleSeq++;
                        PlcWriteQueue(&LockSendBuf[usbNo], (uint8*)&transinfo, sizeof(TRANS_INFO_T));
                        MDLMGR_FMT_DEBUG(usbNo, "查询模组[%d]信息\n", usbNo + 1);
                    }
                }
                else if((modEventFlag[usbNo] != 1) && (LoginFlag == 1))
                {
                    //上报模组插入事件
                    modEventFlag[usbNo] = 1;
                    PlugEventInform(usbNo); 
                    MDLMGR_FMT_DEBUG(usbNo, "上报模组[%d]插入事件\n", usbNo + 1);

                    //声光提示
                    sprintf(barStr, "上报模组[%d]插入事件", usbNo + 1);
                    refresh_status_bar(60, barStr);
                    mdl_open_buzzer(100);
                }

                //判断是否产生模组时钟查询
                GetmoduleClock(usbNo);      
                memset((char *)&transInfo, 0x00, sizeof(TRANS_INFO_T));
                memset((char *)sendDataBuf,0x00,sizeof(sendDataBuf));

                // 获取消息缓冲区
                bRet = PlcReadQueue(&LockSendBuf[usbNo], (uint8*)&transInfo);
                if(bRet > 0)
                {
                    while(i < 3)
                    {   
                        if(gCtrlDev[usbNo] < 0)
                        {
                            gCtrlDev[usbNo] = module_ctrl_comm_init(usbNo);
                            if (gCtrlDev[usbNo] == 0)
                            {
                                cntSuccInit ++;
                                break;
                            }
                            i++;
                            sleep(1);
                        }
                        else
                        {
                            break;
                        }
                    }
                    if(gCtrlDev[usbNo] < 0)
                    {
                        MDLMGR_FMT_DEBUG(usbNo, "发送数据时串口打开%d次失败，需要关闭串口\n",i);
                        em_status = CLOSE_STATE;
                        break;
                    }
                    i = 0;
                    sendDataLen = MakeFrame(usbNo, &transInfo ,sendDataBuf);
                    bRet = SendData(usbNo, sendDataBuf, sendDataLen);
                    if (sendDataLen <= 0 || bRet < 0)
                    {
                        MDLMGR_BUF_DEBUG(usbNo, sendDataBuf, sendDataLen, "数据发送失败, sendDataLen[%d], bRet[%d], sendDataBuf:\n", sendDataLen, bRet);
                        em_status = CLOSE_STATE;
                        break;
                    }

                    MDLMGR_BUF_TRACE(usbNo, sendDataBuf, sendDataLen, "模组数据发送报文:");

                    em_status = RECV_STATE;
                }
                else
                {
                    em_status = RECV_STATE;
                }
                
                break;
            }
        case RECV_STATE:        // 数据接收
            {
                memset((uint8 *)&desFrameInfo, 0x00, sizeof(MDL_FRAME_INFO_T));
                desFrameInfo.dataLen = RecvData(usbNo, desFrameInfo.pDataBuf, sizeof(desFrameInfo.pDataBuf));
                if (desFrameInfo.dataLen <= 0)
                {
                    em_status = SEND_STATE;
                    break;
                }

                MDLMGR_BUF_TRACE(usbNo, desFrameInfo.pDataBuf, desFrameInfo.dataLen, "模组数据接收报文:");

                bRet = AnalysisFrameOfModule(usbNo, &desFrameInfo, dataBufOut,0);
                if (bRet >= 0)
                {
                    em_status = SEND_STATE;
                }
                else
                {
                    MDLMGR_FMT_DEBUG(usbNo, "接收到的模组[%d]信息回码处理失败\n", usbNo + 1);
                }
                break;
            }
        case WAIT_STATE:        // 待机关串口，暂未使用
            {
                em_status = INIT_STATE;
                break;
            }
        case CLOSE_STATE:        // 关闭管理通道并释放资源
            {
                if (gCtrlDev[usbNo] == 0)
                {
                    MDLMGR_FMT_TRACE(usbNo, "关闭管理器通道让出资源 retMdlComm = %d usbNo = %d\n", gCtrlDev[usbNo], usbNo);
                    module_ctrl_comm_uninit(usbNo);
                    gCtrlDev[usbNo] = -1;
                }

                //模组升级时候，关闭这里的控制口
                if (MdlUpdateDeal != MDL_UPDATE_IDLE)
                {
                    break;
                }

                //删除软连接，释放信息
                ClearNgtMsg(usbNo);
                DelLn(usbNo, &usbStatus);
                
                em_status = INIT_STATE;
                break;
            }
        default:
            break;
        }

        usleep(50000);
    }
    return;
}

/***********************************************************************
 * @brief   模组管理器管理模组收发处理线程
 * @param[in]  argc     : USB端口号
 *
 * @retval  无
 **********************************************************************/
static void* OnMdlUsbThread(void* arg)
{
    int32 usbNo = 0;

    usbNo = *(int32 *)(arg);
    RecognizeModuleDeal(usbNo);

    return NULL;
}

void checkPowerCnt()
{
    int i;
    uint8 result = 1;
    
    for (i = 0; i < mdlnum; i++)
    {
        if(modPowerCtl[i].offpowerflag == 1)
        {
            if(modPowerCtl[i].offpowertime > 0)
            {
                modPowerCtl[i].offpowertime--;
            }
            else
            {
                modPowerCtl[i].offpowerflag = 0;

                if(ModulePowerOff(i+1,1) < 0 )
                {
                    MDLMGR_FMT_DEBUG(mdlnum, "模组 %d 上电失败! \n",i+1);
                    result = 0;             //失败
                }

                MDLMGR_FMT_DEBUG(mdlnum,"模组槽位%d断电重启:%s\n", i+1 ,result==0 ? "失败":"成功");
            }
        }
    }
}

//模组管理器mqtt通信线程
#if MSG_MQTT == 1
void * mqtt_thread(void* arg)
{
    INIT_FMT_DEBUG("模组管理器mqtt通信线程 启动\n");

    static time_t   LastLoginTime = 0;
    static time_t   LastLoginGUI = 0;
	struct timespec curtime = {0};
    int8    mqttState = MQTT_INIT;
    uint8   RecvBuf[MAX_BUF_LEN] = {0};
    uint8   SendBuf[MAX_BUF_LEN] = {0};
    uint8   SendInfoBuf[MAX_BUF_LEN] = {0};
    int32   RecvLen = 0;
    int32   SendLen = 0;
    int32   SendInfoLen = 0;
    ThreadArg_T     *pThreadArg;
    LockQueue_T     *pLockRecv;
    const char*     appName = "mapManager";
    const char*     iconFile = "/usr/local/extapps/mapManager/bin/cfg/mapManager.bmp";
    APP_REGIST_INFO_T appRegInfo;       //向桌面gui发送的注册信息

    int             ret;
    int             result;
    static  time_t  lastTime;
    MSG_INFO_T      *ptMsgInfo = NULL;
    MSG_INFO_T      *pMsgInfoSend = NULL;
    MESSAGE_INFO_T  *pMsgInfo = NULL;
    pThreadArg =    (ThreadArg_T *)arg;
    pLockRecv =     pThreadArg->RecvBuf;
    INFO_DATA_T     stInfoData;
    TRANS_INFO_T    SendAppData;
    int             i;
    memset(RecvBuf,0x00,sizeof(RecvBuf));
    memset(SendBuf,0x00,sizeof(SendBuf));
    memset(SendInfoBuf,0x00,sizeof(SendInfoBuf));
    memset(&stInfoData,0x00,sizeof(stInfoData));
    
    dev_power = (POWER_DEVICE_T*)hal_device_get(HW_DEVICE_ID_POWER);
    
    if(NULL == dev_power)
    {
        MDLMGR_FMT_DEBUG(mdlnum, "dev_power is null\n");
    }
    
    while(1)
    {
    	clock_gettime(CLOCK_MONOTONIC, &curtime);
        g_MqttThreadFlag = 0;
        
        if(lastTime != curtime.tv_sec)      
        {
            lastTime = curtime.tv_sec;
            checkPowerCnt();            //过秒检查处理槽位断电情况
        }

        if(LoginFlag == 1 && getLoginguiID() == 0 && (curtime.tv_sec - LastLoginGUI > 3))           //向桌面gui发注册
        {
            memset(&appRegInfo,0,sizeof(APP_REGIST_INFO_T));
            memcpy(appRegInfo.appname,appName,strlen(appName));
            memcpy(appRegInfo.shortname, ADV_SHOW_NAME, strlen(ADV_SHOW_NAME));
            memcpy(appRegInfo.iconFileName,iconFile,strlen(iconFile));
            appGUI_send_req_handle(MESSAGE_IID_DESKGUI, DESKGUI_IOP_APP_REG, &appRegInfo, sizeof(APP_REGIST_INFO_T));
            LastLoginGUI = curtime.tv_sec;
        }
        
        switch(mqttState)
        {
            case MQTT_INIT:
            {
                //将mqtt初始化移到主函数中，否则会出现mqtt的时序问题
                mqttState = MQTT_LOGIN;
                break;
            }
            case MQTT_LOGIN:
            {
                login_to_smiOS();
                LastLoginTime = time(NULL);
                mqttState = MQTT_LOGIN_WAIT;

                break;
            }
            case MQTT_LOGIN_WAIT:
            {   
                if(time(NULL) - LastLoginTime > 5)
                    mqttState = MQTT_LOGIN;
                else
                    mqttState = MQTT_RECV;
                break;
            }
            case MQTT_RECV:
            {
                memset(RecvBuf,0x00,sizeof(RecvBuf));
                RecvLen = mqtt_recv_msg(RecvBuf,MAX_QUEUE_BUFF_LEN);
                if(RecvLen > 0)
                {   
                    ret = msg_mqtt_split(RecvBuf,RecvLen, 0 ,0, &ptMsgInfo);
                    if(ret < 0)
                    {
                        MDLMSG_BUF_DEBUG(RecvBuf, RecvLen, "收到的消息解析失败");
                        break;
                    }
                    char topic[256] = {0};
                    sprintf(topic, "%s/%s", ptMsgInfo->SourAddr, ptMsgInfo->DestAddr);
                    MDLMSG_BUF_TRACE(RecvBuf, RecvLen, "mqtt_RECV[%s]", topic);
                    
                    PrintInfoDetail(ptMsgInfo);

                    if(ptMsgInfo->IID == 0x0000)
                    {
                        switch(ptMsgInfo->IOP)
                        {
                            case 0x0001:
                            case 0x0002:
                            case 0x0003:
                            {
                                MDLMSG_FMT_DEBUG("收到系统停电事件!\n");
                                break;
                            }
                            case 0x0005:            //系统校时事件
                            {
                                MDLMSG_FMT_DEBUG("收到系统校时事件，全部模组产生对时任务!\n");   
                                for(i = 0; i< mdlnum; i++)
                                {
                                    memset(&SendAppData,0x00,sizeof(TRANS_INFO_T));
                                    SendAppData.DTA = 0x01;
                                    SendAppData.DTB = 0x00;             
                                    SendAppData.seq = globleSeq++;
                                    SendAppData.clkSetFlag = 1;     //设置时钟标志
                                    clockSet[i] = time(NULL);
                                    PlcWriteQueue(&LockSendBuf[i], (uint8*)&SendAppData, sizeof(TRANS_INFO_T));
                                }
                                break;
                            }
                            case 0x0010:         //心跳
                            {
                                heart_beat(ptMsgInfo->MsgIndex);
                                break;
                            }
                            case 0x0013:         //版本信息
                            {
                                appversion_response(ptMsgInfo);
                                break;
                            }
                            case 0x0016://appid信息
                            {
                                appid_response(ptMsgInfo);
                                break;
                            }
                            default:
                                break;
                        }
                    }
                    else if(ptMsgInfo->IID == 0x0001)
                    {
                        switch (ptMsgInfo->IOP)
                        {
                            case 0x0010:         //注册响应
                            {
                                LoginFlag = 1;
                                MDLMSG_FMT_DEBUG("登录smiOS成功\n");    
                                break;
                            }
                            default:
                                break;
                        }
                    }
                    else if(ptMsgInfo->IID == 0x0007)
                    {
                        appmsg_split(RecvBuf, (uint16)RecvLen , &pMsgInfo);
                        appGUI_recv_back_handle(pMsgInfo);
                        free(pMsgInfo);
                        pMsgInfo = NULL;
                    }
                    else
                    {
                        if(ptMsgInfo->MsgLen > MAX_MSG_PAYLOAD_LEN)
                        {
                            MDLMSG_FMT_DEBUG("错误！ptMsgInfo->MsgLen > MAX_MSG_PAYLOAD_LEN\n");
                        }
                        else
                        {
                            memset(&stInfoData,0x00,sizeof(stInfoData));
                            memcpy(&stInfoData,ptMsgInfo,sizeof(MSG_INFO_T)+ptMsgInfo->MsgLen);
                            AddInfoDataIntoList(stInfoData);
                        }
                    }

                    free(ptMsgInfo);
                    ptMsgInfo = NULL;

                }

                mqttState = MQTT_SEND;

                CreateTaskByInfo();
                break;
            }
            case MQTT_SEND:
            {
                SendLen = PlcReadQueue(pLockRecv, SendBuf);
                if (SendLen > 0)
                {
                    //获取MSG
                    memset(&stInfoData, 0x00, sizeof(stInfoData));
                    memcpy(&stInfoData, SendBuf, SendLen);

                    //分配内存
                    pMsgInfoSend = (MSG_INFO_T *)malloc(SendLen);

                    //获取HEAD，填充udp目标ip地址和端口
                    memset(pMsgInfoSend, 0x0, sizeof(MSG_INFO_T));
                    result = GetHeadInfo(stInfoData, pMsgInfoSend);         
                    if (result >= 0)
                    {
                        //组消息
                        SendInfoLen = msg_mqtt_package(pMsgInfoSend, MAX_QUEUE_BUFF_LEN, SendInfoBuf);
                        
                        //发送
                        char topic[256] = {0};
                        if ((pMsgInfoSend->IID == 0x04) && (pMsgInfoSend->IOP == 0x01))
                        {
                            sprintf(topic, "%s/%s/%04X/%04X", pMsgInfoSend->SourAddr, pMsgInfoSend->DestAddr, pMsgInfoSend->IID, pMsgInfoSend->IOP);
                        }
                        else
                        {
                            sprintf(topic, "%s/%s", pMsgInfoSend->SourAddr, pMsgInfoSend->DestAddr);
                        }
                        mqtt_topic_write(SendInfoBuf, SendInfoLen, topic);
                        
                        MDLMSG_BUF_DEBUG(SendInfoBuf, SendInfoLen, "mqtt_SEND[%s]", topic);
                        memset(SendInfoBuf, 0x0, sizeof(SendInfoBuf));

                        //打印完整消息内容
                        PrintInfoDetail(pMsgInfoSend);
                    }
                    else
                    {
                        MDLMSG_FMT_DEBUG("获取报文头失败 错误码[%d]\n", result);
                    }

                    memset(SendBuf, 0x0, sizeof(SendBuf));
                    free(pMsgInfoSend);
                    pMsgInfoSend = NULL; 
                }

                if(LoginFlag == 1)
                {
                    mqttState = MQTT_RECV;
                }
                else
                {
                    mqttState = MQTT_LOGIN_WAIT;
                }
                break;
            }
            default:
            {
                mqttState = MQTT_INIT;
                break;
            }
                
        }

        usleep(10000);
    }
    hal_device_release((HW_DEVICE *)dev_power);
    pthread_exit(NULL);

}
#endif
/***********************************************************************
 * @brief   线程维护函数
 * @param[in]  : INT32 cnt, INT32 time, 
 *
 * @retval  无
 **********************************************************************/
int32 sys_thread_defend(void *fun, void *arg, MDL_THREAD_DEFEND_T *dc_info, MDL_THREAD_INFO_T *pf_info)
{
    uint32 err;
    printf(" pf_info->flag_thread_msg = %d dc_info->cnt_thread_msg = %d\n", pf_info->flag_thread_msg, dc_info->cnt_thread_msg);
    if (pf_info->flag_thread_msg == 1)
    {
        pf_info->flag_thread_msg = 0;
        dc_info->cnt_thread_msg = 0;
    }
    else 
    {
        if (dc_info->cnt_thread_msg >= 3)
        {
            pthread_cancel(dc_info->ntid);//有时返回的不是0
            pthread_join(dc_info->ntid, NULL);
            err = pthread_create(&dc_info->ntid, NULL, (void* (*)(void*))fun, arg);
            if (err != 0)
            {
                exit(1);
            }
            else
            {
                dc_info->cnt_thread_msg = 0;
            }
        }
        else
        {         
            dc_info->cnt_thread_msg++;
        }
    }
    return 1;
}

void init_cfg()
{
    char *tempDir = "/tmp/dev";
    strncpy(LinkDir,tempDir,sizeof(LinkDir));
    //....
}

/** 
*********************************************************************
* @brief     ：初始化配置文件
* @param[in] ：
* @param[out]：
* @return    ：0-成功/错误码
*********************************************************************
*/
int cfgfile_get(uint32 *ip, uint16 *port)
{   
    char buf[1024];
    char tempbuf[1024];
    char usbFile[1024];

    // 读取配置文件
    init_config(ip, port);

    init_cfg();                             //预配置
    FILE * fp = fopen(CONFIG_FILE, "r");
    if(!fp)
    {
        printf("%s not exists,exit!\n",CONFIG_FILE);
        //exit(1);
        return 0;
    }

    while(!feof(fp))
    {
        memset(buf,0x0,sizeof(buf));
        
        memset(usbFile,0x0,sizeof(usbFile));

        if(fgets(buf,sizeof(buf),fp))
        {
            if(NULL == strstr(buf,"="))
                continue;
            if(strstr(buf,"#") == buf)
                continue;

            char *pfile = NULL;
            if(strstr(buf,"CHANNEL") == buf && (pfile = strstr(buf,"/")) != NULL)
            {
                memcpy(usbFile,pfile,strlen(pfile));
                //printf("usbFile = %s",usbFile);
                if((pfile = strstr(usbFile,"2-1.")) == NULL)
                    continue;
                memset(usbPath,0x0,sizeof(usbPath));
                memcpy(usbPath,usbFile,pfile-usbFile);
                strcat(usbPath,"2-1.");
                printf("usbPath = %s\n",usbPath);
            }
            
            if(strstr(buf,"IP") == buf)
            {
                memset(tempbuf,0x0,sizeof(tempbuf));
                sscanf(buf,"%*[^1-9]%s",tempbuf);
                printf("IP = %s\n",tempbuf);
            }

            if(strstr(buf,"PORT") == buf)
            {
                memset(tempbuf,0x0,sizeof(tempbuf));
                sscanf(buf,"%*[^1-9]%s",tempbuf);
                printf("PORT = %s\n",tempbuf);
            }
            if(strstr(buf,"LINKDIR") == buf)
            {
                memset(tempbuf,0x0,sizeof(tempbuf));
                sscanf(buf,"%*[^/]%s",tempbuf);
                strncpy(LinkDir,tempbuf,sizeof(LinkDir));
                printf("LINKDIR = %s\n",LinkDir);
            }
        }
    }

    fclose(fp);

    return 1;
}

/**********************************************************************
* @name      : app_main_task
* @brief     ：线程维护
* @param[in] ：void *arg
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2020-5-15
* @Update    : 
**********************************************************************/
void* mdl_defend_task(void *arg)
{
    uint8 i;
    int16 cnt_thread_msg[mdlnum] = {0};    // 线程守护次数及ID的信息

    INIT_FMT_DEBUG("线程维护启动\n");
    
    while(1)
    {
        // USB口线程守护
        for (i = 0; i < mdlnum; i++)
        {
            if(g_flag_thread_msg[i] == 1)
            {
                g_flag_thread_msg[i] = 0;
                cnt_thread_msg[i] = 0;
            }
            else
            {
                if (cnt_thread_msg[i] >= 20)
                {
                    MDLMGR_FMT_DEBUG(mdlnum, "mdlmgr OnMdlUsbThread[%d] running failed!\n", i);
                    cnt_thread_msg[i] = 0;
                    exit(0);
                }
            }            
        }

        if (mdl_update_thread_cnt ++ > 20)
        {
            MDLMGR_FMT_DEBUG(mdlnum, "mdlmgr mdl_update_thread running failed!\n");
            mdl_update_thread_cnt = 0;
            exit(0);
        }

        if (g_MqttThreadFlag ++ > 20)
        {
            MDLMGR_FMT_DEBUG(mdlnum, "mdlmgr mqtt_thread running failed!\n");
            g_MqttThreadFlag = 0;
            exit(0);
        }

        sleep(2);
    }

    return 0;
}

int32 main(int argc, char** argv)
{   
    ThreadArg_T Mqtt_Thread_Arg;
    uint32      mqttip = 0;
    uint16      mqttport = 0;
    pthread_t   ntid_mdl_usb[mdlnum];
    pthread_t   mqtt_thread_id;
    pthread_t   mdl_gui_id;
    pthread_t   mdl_defend_id;
    pthread_t   mdl_update_thread_id;               //模组升级子线程标识符
    int32       usbNo[mdlnum];
    int32       i = 0;
    uint32      err = 0;
    char        cmdBuf[128] = {0};
    Mqtt_Thread_Arg.RecvBuf = &LockRecvBuf;
    Mqtt_Thread_Arg.SendBuf = LockSendBuf;

    if (argc >= 2)
    {
        if(0 == memcmp(argv[1], "version", strlen("version")))
        {
            printf("Version: %s\nTime: %s\n", APP_VERSION, APP_PUBLISH_TIME);
            return 0;
        }
    }

    if (argc >= 2)
    {
        if(0 == memcmp(argv[1], "debug", strlen("debug")))
        {
            printf("30s waiting...\n");
            sleep(30);
        }
    }

    if (global_var_init() < 0)
    {
        printf("global_var_init err\n");
        exit(1);
    }

    sprintf(cmdBuf,"sudo mkdir -p %s",MDL_LOG_DIR);
    if (opendir(MDL_LOG_DIR) == NULL && system(cmdBuf) < 0)
    {
        printf("日志路径 %s 不存在!\n",MDL_LOG_DIR);
        exit(1);
    }

    writePlclog(MDLSTARTLOG, "mapManager start\n");

    // 防止多次执行
    if (!singleton_check("mapManager")) 
    {
        sleep(1);
        if (!singleton_check("mapManager"))
        {
            writePlclog(MDLSTARTLOG, "already running!\n");
            exit(1);
        }
    }

    /* 信号处理 */
    init_sig_action("/data/app/mapManager/log/Exception.log");

    // hal初始化
    if (0 != hal_init())
    {
        writePlclog(MDLSTARTLOG, "hal_init failed!\n");
        exit(1);
    }

    // 读配置文件
    cfgfile_get(&mqttip, &mqttport);

    // hal获取appid
    appid_query(regAppName, AppId, APP_ID_LEN);

    INIT_FMT_DEBUG("Version: %s Time: %s, area: %s, Build: %s %s\n", APP_VERSION, APP_PUBLISH_TIME, AREANAME_PLACE, 
        __DATE__, __TIME__);

    // mqtt初始化
    if (mqtt_init(mqttip, mqttport) != 0)
    {
        INIT_FMT_DEBUG("mqtt初始化错误\n");
        exit(1);
    }
    INIT_FMT_DEBUG("mqtt初始化成功\n");

    // 创建USB口数据交互处理线程
    for (i = 0; i < mdlnum; i++)
    {
        g_flag_thread_msg[i] = 0;
        usbNo[i] = i;
        err = pthread_create(&ntid_mdl_usb[i], NULL, OnMdlUsbThread, &usbNo[i]);
        if (err != 0)
        {
            exit(1);
        }
    }
    
    err = pthread_create(&mdl_update_thread_id, NULL, mdl_update_thread, NULL);
    if (err != 0)   
    {
        INIT_FMT_DEBUG("creating the mdl_update_thread failed!\n");
        exit(1);
    }

    err = pthread_create(&mqtt_thread_id, NULL, mqtt_thread, &Mqtt_Thread_Arg);
    if(err != 0)
    {
        INIT_FMT_DEBUG("creating the mqtt_thread failed!\n");
        exit(1);
    }
    
    if (g_MdlMqttClient != NULL)
    {
        initlize_desk("mapManager", mdlmgr_send_msg);
    }
    else
    {
        exit(1);
    }
    
    usleep(20000);

    /* 显示注册 */
    AppPluginReg(g_plugin, g_nPluginCnt);
    err = pthread_create(&mdl_gui_id,NULL,app_gui_entry,NULL);
    if (err != 0)
    {
        INIT_FMT_DEBUG("creating the app_gui_entry failed!\n");
        exit(0);
        return -1;
    }

    /* 启动守护线程 */
    err = pthread_create(&mdl_defend_id, NULL, mdl_defend_task, NULL);
    if (err != 0)
    {
        INIT_FMT_DEBUG("Thread mdl_defend_task create error,err: %d\n", err);
        exit(0);
        return -1;
    }

    /* 启动task */
    while(1)
    {
        sleep(2);
    }
    
    pthread_join(mdl_update_thread_id, NULL);
    pthread_join(mqtt_thread_id, NULL);
    pthread_join(mdl_gui_id, NULL);
    pthread_join(mdl_defend_id, NULL);
    for (i = 0; i < mdlnum; i++)
    {
        pthread_join(ntid_mdl_usb[i], NULL);
    }
    return 0;
}

