/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief： 功能描述
* @date： 2019-10-25
*********************************************************************
*/


#include "CcoCommon.h"

#include "CcoDebug.h"
#include "CcoFiles.h"
#include "CcoLib.h"
#include "CcoPrtl.h"
#include "CcoTTL.h"
#include "CcoMain.h"

#include "Mqtt.h"

uint8 gNullArr[100] = {0};//空数组, 用于比对

/*******************************************************************************
* 本地对上通信 UDP通信参数 *********************------<测试修改>------**********
*******************************************************************************/

//本地名称------<测试修改>------
char       *gCcoRouterAppName = "M-ccoRouter";

//本地端口号------<测试修改>------
uint16      gCcoRouterAppPort = 9012;

//本地IP地址------<测试修改>------
char       *gCcoRouterAppIP   = "192.168.9.188";


/*******************************************************************************
* 调度管理APP UDP通信参数 *********************------<测试修改>------**********
*******************************************************************************/

//调度管理APP CTS_ROUTER_APP_NAME------<测试修改>------
char       *gCtsRouterAppName[MAX_ALLOWED_CTS_THREAD_NUM] = {"M-taskManager", "M-taskManager"};  //调度管理APP CTS_ROUTER_APP_NAME

//调度管理APP CTS_ROUTER_APP_PORT------<测试修改>------
uint16      gCtsRouterAppPort[MAX_ALLOWED_CTS_THREAD_NUM] = {9020, 9021};  //调度管理APP CTS_ROUTER_APP_PORT

//调度管理APP CTS_ROUTER_APP_IP------<测试修改>------
char       *gCtsRouterAppIP[MAX_ALLOWED_CTS_THREAD_NUM] = {"192.168.9.188", "192.168.9.188"};  //调度管理APP CTS_ROUTER_APP_IP


/*******************************************************************************
* 本地两个载波模块参数               *********************------<测试修改>------**********
*******************************************************************************/

//端口号------<测试修改>------
//uint32		gPortID[MAX_ALLOWED_DOWN_THREAD_NUM] = {0xf2090201, 0xf2090202};  //两个端口号
uint32		gPortID[MAX_ALLOWED_DOWN_THREAD_NUM] = {0, 0};  //两个端口号
BOOL        bSetPortIDFlag[MAX_ALLOWED_DOWN_THREAD_NUM] = {0, 0};  //是否收到端口号设置的标志 0:删除, 1:增加


//通信模块设备名称------<测试修改>------
char       *gDevName[MAX_ALLOWED_DOWN_THREAD_NUM] = {"/dev/hidraw0", "/dev/hidraw1"};  //两个通信设备(智芯)
char       *gDingxinDevName[MAX_ALLOWED_DOWN_THREAD_NUM] = {"/tmp/dev/usb_exm2_1", "/tmp/dev/usb_exm1_0"};  //两个通信设备(鼎信)
DEV_HAL_INFO gDevHalInfo[MAX_ALLOWED_DOWN_THREAD_NUM];


/*******************************************************************************
* 私有文件路径
*******************************************************************************/

//档案私有文件名
char       *gLocalFilesName[MAX_ALLOWED_DOWN_THREAD_NUM] = {"/data/app/ccoRouter/LocalFilesName_1", "/data/app/ccoRouter/LocalFilesName_2"};  //两个私有文件

//通信模块地址私有文件名
char       *gModuleManuName[MAX_ALLOWED_DOWN_THREAD_NUM] = {"/data/app/ccoRouter/ModuleManuName_1", "/data/app/ccoRouter/ModuleManuName_2"};  //两个私有文件

//端口号信息私有文件名
char       *gPortIDName[MAX_ALLOWED_DOWN_THREAD_NUM] = {"/data/app/ccoRouter/PortIDName_1", "/data/app/ccoRouter/PortIDName_2"};  //两个私有文件

//显示相关信息私有文件名
char       *gGuiInfoName[MAX_ALLOWED_DOWN_THREAD_NUM] = {"/data/app/ccoRouter/GuiInfoName_1", "/data/app/ccoRouter/GuiInfoName_2"};  //两个私有文件

/*******************************************************************************
* 终端地址
*******************************************************************************/
uint8       gTerminalAddr[MAX_ALLOWED_DOWN_THREAD_NUM][6] = {{0}, {0}};  //终端地址
BOOL        gUpdateTerminalAddrFlag[MAX_ALLOWED_DOWN_THREAD_NUM] = {FALSE};  //更新终端地址标志

/*******************************************************************************
* 变量声明
*******************************************************************************/

PastTimeFlag_T             gPastTimeFlag[MAX_DOWN_THREAD_NUM];                           //过时间标志
DateTime_T                 gCurBinTime[MAX_DOWN_THREAD_NUM];                             //当前BIN时间
DateTime_T                 gCurBcdTime[MAX_DOWN_THREAD_NUM];                             //当前BCD时间
DateTime_T                 gPreBinTime[MAX_DOWN_THREAD_NUM];                             //上次BIN时间
DateTime_T                 gCurBcdTimeUp;  //当前BCD时间(记录上行消息)
DateTime_T                 gCurBcdTimeDown[MAX_DOWN_THREAD_NUM];  //当前BCD时间(记录下行消息)


//收到的消息缓存区
INFO_DATA_LIST_T gInfoDataList = {0};                      //收到的消息缓存区列表

//主动上报消息INDEX
uint16  ActiveInfoIndex = 0;                                      //主动上报消息INDEX
extern  pthread_mutex_t    ActiveInfoIndexLock;

//上电标志
int                        gPowerOnFlag[MAX_DOWN_THREAD_NUM];                            //上电标志

//上电获取模块信息
ModuleManuID_T             gModuleManuID[MAX_DOWN_THREAD_NUM];               //模块厂商标识
LOCAL_COMM_MODULE_RUNNING_PATTERN_INFO  gLocalCommModuleInfo[MAX_DOWN_THREAD_NUM];         //模块信息的全局变量

//档案列表全局变量
AMR_METER_FILES_LIST_T gAmrMeterFilesListTemp[MAX_DOWN_THREAD_NUM];  //档案列表临时变量(消息缓存临时变量)
AMR_METER_FILES_LIST_T gAmrMeterFilesList[MAX_DOWN_THREAD_NUM];  //档案列表全局变量
AMR_METER_FILES_LIST_T gToAddFilesList[MAX_DOWN_THREAD_NUM];  //需要添加的档案列表全局变量
AMR_METER_FILES_LIST_T gToDeleteFilesList[MAX_DOWN_THREAD_NUM];  //需要删除的档案列表全局变量

uint8                     gMeterFileAddDelFlagTemp[MAX_DOWN_THREAD_NUM] = {0};       //档案增加删除标志(消息缓存临时变量)
uint8                     gMeterFileClearFlagTemp[MAX_DOWN_THREAD_NUM] = {0};        //档案清除标志(消息缓存临时变量)

uint8                     gMeterFileAddDelFlag[MAX_DOWN_THREAD_NUM] = {0};       //档案增加删除标志
uint8                     gMeterFileClearFlag[MAX_DOWN_THREAD_NUM] = {0};        //档案清除标志
uint8                     gMeterFileCompareFlag[MAX_DOWN_THREAD_NUM] = {0};      //档案比对标志
uint8                     gMeterFileModifyFlag[MAX_DOWN_THREAD_NUM] = {0};       //档案修改标志
uint8                     gMeterFileModifyState[MAX_DOWN_THREAD_NUM] = {0};      //档案修改状态(包含cmp和modify整个过程)，0：更新完，1：更新中
METER_FILES_MODEFY_TYPE_E gMeterFilesModifyType[MAX_DOWN_THREAD_NUM] = {0};      //档案修改方式

//存放抄表消息的缓冲区
AMR_ACTIVE_INFO_LIST_T gActiveAmrInfoList[MAX_DOWN_THREAD_NUM];  //存放抄表消息的缓冲区

//载波任务调度使用的全局变量
PLCTASK_QUEUE              PlcTaskQueue[MAX_DOWN_THREAD_NUM];  //载波任务队列
PLCTASK_DETAIL             gPlcTaskDetail[MAX_DOWN_THREAD_NUM];  //当前载波具体任务细节
PLCTASK_DETAIL             gPlcTaskDetailSearchMeter[MAX_DOWN_THREAD_NUM];  //当前载波具体任务细节(搜表)

//各个子任务内部使用的全局变量
TOTAL_STATE_T              gTotalState[MAX_DOWN_THREAD_NUM];                             //总状态字

//抄表汇总信息
AmInfo_T    gAmrInfo[MAX_DOWN_THREAD_NUM];			                //抄表汇总信息

//plc复位引脚和模块插入引脚检测文件描述符
int fd_plc[MAX_DOWN_THREAD_NUM];//plc复位引脚和模块插入引脚检测文件描述符

//宽带日常抄表队列
GLOBAL_SEQ_INFO_T gGlobalSeqInfo[MAX_DOWN_THREAD_NUM];

//路由控制操作
ROUTER_CTRL_STATE_T   gRoutrtCtrl[MAX_DOWN_THREAD_NUM] = {RCS_NULL};           //路由控制操作

//路由控制操作
ROUTER_OPER_DETAIL_T  gRouterOper[MAX_DOWN_THREAD_NUM];           //路由控制操作

//启动搜表标志
WHETHER_START_SEARCH_METER_T gWhetherStartSearchMeter[MAX_DOWN_THREAD_NUM];       //启动搜表标志

//暂停搜表标志
BOOL gPauseSearchMeterFlag[MAX_DOWN_THREAD_NUM] = {FALSE};       //暂停搜表标志

//搜表档案序号(从0开始)
uint16 gSearchMeterFilesIndex[MAX_DOWN_THREAD_NUM] = {0};

//穿透任务队列
AMR_DATA_TRANSMIT_LIST_T gDataTransmitQueue[MAX_DOWN_THREAD_NUM];

//获取从节点监控延时任务队列
AMR_SLAVE_MONITOR_TIME_LIST_T gSlaveMonitorTimeQueue[MAX_DOWN_THREAD_NUM];

//从节点监控任务队列
AMR_SLAVE_MONITOR_LIST_T gSlaveMonitorQueue[MAX_DOWN_THREAD_NUM];

//是否获取广播延时
WHETHER_GET_BROADCAST_DELAY_T gWhetherGetBroadcastDelay[MAX_DOWN_THREAD_NUM];

//是否广播
WHETHER_DO_BROADCAST_T gWhetherDoBroadcast[MAX_DOWN_THREAD_NUM];

//查询HPLC信息 信息类型
HPLC_TYPE_QUERY_UN gQueryTypeHPLC[MAX_DOWN_THREAD_NUM];  //每个bit对应不同的HPLC查询类型

//5个模组的信息
BOOL gQueryAllModuleFlag = TRUE;  //程序刚启动时, 读取所有模块信息
MODULE_INFO_T gModuleInfo[5];  //5个模组的信息
uint8 gReloadModuleFlag[6] = {0};  //首位：0表示无变化, 1表示变化, 后面5个0/1表示模组变化状态

//备份最近一次是否CCO模块标记
uint8 gCcoFlag[5] = {0};  //备份最近一次是否CCO模块标记

//HPLC频点设置标志
uint8 gFrSetFlag[MAX_DOWN_THREAD_NUM] = {0};  //HPLC频点设置标志
//HPLC频点设置值
uint8 gFrSetValue[MAX_DOWN_THREAD_NUM] = {0};  //HPLC频点设置值

//HPLC频点查询标志
uint8 gFrQueryFlag[MAX_DOWN_THREAD_NUM] = {0};  //HPLC频点查询标志
INFO_DATA_HEAD_T gFrQueryInfoDataHead[MAX_DOWN_THREAD_NUM];  //HPLC频点查询消息原始HEAD
uint8 gInitFrQueryFlag[MAX_DOWN_THREAD_NUM] = {0};  //HPLC频点查询标志(上电初始化查询)

//查询抄表状态结构体
AMR_QUERY_READING_STATE_T gAmrStateQuery[MAX_DOWN_THREAD_NUM];  //查询抄表状态结构体

//查询抄表模块信息结构体
AMR_QUERY_MODULE_INFO_T gAmrModuleQuery[MAX_DOWN_THREAD_NUM];  //查询抄表模块信息结构体

//HPLC台区识别标志
uint8 gAreaIdentifyFlag[MAX_DOWN_THREAD_NUM] = {0};  //HPLC台区识别标志
//HPLC台区识别值
uint8 gAreaIdentifyValue[MAX_DOWN_THREAD_NUM] = {0};  //HPLC台区识别值

//HPLC离网感知标志
uint8 gOfflineFeelFlag[MAX_DOWN_THREAD_NUM] = {0};  //HPLC离网感知标志
//HPLC离网感知值
uint8 gOfflineFeelValue[MAX_DOWN_THREAD_NUM] = {0};  //HPLC离网感知值

//HPLC拒绝节点上报标志
uint8 gRefuseNodeReportFlag[MAX_DOWN_THREAD_NUM] = {0};  //HPLC拒绝节点上报标志
//HPLC拒绝节点上报开关
uint8 gRefuseNodeReportValue[MAX_DOWN_THREAD_NUM] = {0};  //HPLC拒绝节点上报开关

//模块插拔重新获取设备
uint8 gReloadDevFlag[MAX_DOWN_THREAD_NUM] = {0};       //模块插拔重新获取设备

//gui展示内容
GUI_SHOW_MESSAGE_T gGuiShowMessage[MAX_DOWN_THREAD_NUM];  //gui展示内容

//升级文件标识全局标记
uint8 gPlcUpdateFileMark[MAX_DOWN_THREAD_NUM] = {0};  //升级文件标识全局标记. 0x03:主节点升级文件; 0x07:主节点+全部子节点;0x08:全部子节点

//CCO各类升级文件路径
char *gRouterUpdateFile[MAX_ALLOWED_DOWN_THREAD_NUM] = {"/data/app/ccoRouter/router_update_file_1", "/data/app/ccoRouter/router_update_file_2"};  //主节点升级包 _1 第一个cco模块, _2 第二个cco模块
char *gRouterUpdateFileBack[MAX_ALLOWED_DOWN_THREAD_NUM] = {"/data/app/ccoRouter/router_update_file_back_1", "/data/app/ccoRouter/router_update_file_back_2"};  //主节点升级包
char *gAllRouterUpdateFile[MAX_ALLOWED_DOWN_THREAD_NUM] = {"/data/app/ccoRouter/all_router_update_file_1", "/data/app/ccoRouter/all_router_update_file_2"};  //主节点+全部子节点升级 _1 第一个cco模块, _2 第二个cco模块
char *gAllRouterUpdateFileBack[MAX_ALLOWED_DOWN_THREAD_NUM] = {"/data/app/ccoRouter/all_router_update_file_back_1", "/data/app/ccoRouter/all_router_update_file_back_2"};  //主节点+全部子节点升级
char *gSubRouterUpdateFile[MAX_ALLOWED_DOWN_THREAD_NUM] = {"/data/app/ccoRouter/sub_router_update_file_1", "/data/app/ccoRouter/sub_router_update_file_2"};  //全部子节点升级 _1 第一个cco模块, _2 第二个cco模块
char *gSubRouterUpdateFileBack[MAX_ALLOWED_DOWN_THREAD_NUM] = {"/data/app/ccoRouter/sub_router_update_file_back_1", "/data/app/ccoRouter/sub_router_update_file_back_2"};  //全部子节点升级

//查询宽带载波模块节点信息数据单元格式
CCO_STA_INFO_T gAllCcoStaInfo[MAX_DOWN_THREAD_NUM];

//文件升级接口相关
uint8  gIsUpdating = 0;     //当前是否在升级中
uint8  gIsNeedUpdating = 0;     //是否有升级任务
char gFilePath[200] ={0};       //文件路径
uint8 gFileFlag;        //文件标识
uint8 gUpdateState;  //升级状态 0未升级 1升级中 2 升级结束
uint32 gUpdateBeginTime;
uint32 gUpdateEndTime;
uint8 gUpdateResult;        //升级结果 0—升级成功；1—升级文件异常；2—升级传输失败；255—其他异常

const PLCTASK_FUNINFO PlcTask_FunList[] =
{
    {1, MS_UPDATE_DEAL,      "CCO本地文件升级",          0,      RCS_PAUSE,      10,       ModuleUpdateDeal},                //载波本地升级处理
    {1, MS_GET_MANU_ID,      "获取厂商标识",             1,      RCS_NULL,       0,        GetModuleManuID},                 //获取厂商标识
    {1, MS_ROUTER_DEAL,      "路由操作",               2,      RCS_NULL,       10,       AppRouterDealFun},                //路由操作
    {1, MS_FILE_CMP,         "档案比对",               3,      RCS_PAUSE,      15,       MeterFileCmpDeal},                //电表档案比对
    {1, MS_FILE_MODIFY,      "档案修改",               4,      RCS_PAUSE,      15,       MeterFileMofifyDeal},             //电表档案修改处理
    {1, MS_SLAVE_MONITOR,    "从节点监控",              5,      RCS_PAUSE,      3,        SlaveMonitorDeal},                //从节点监控
    {1, MS_RELAY_TRANS,      "透明转发",               6,      RCS_PAUSE,      3,        RelayTransDeal},                  //检测透明转发    
    {1, MS_ADDR_SEARCH,      "表号搜索",               7,      RCS_PAUSE,      60,       AddrSearchDeal},                  //检测表号搜索
    {1, MS_SLAVE_DELAY,      "获取从节点监控延时", 8,               RCS_PAUSE,      3,        GetSlaveMonitorDelay},            //获取从节点监控延时
    {1, MS_BROADCAST_DELAY,  "获取广播延时",             10,     RCS_PAUSE,      3,        GetBroadcastDelay},               //获取广播延时
    {1, MS_BROADCAST_DEAL,   "广播",                 11,     RCS_PAUSE,      3,        BroadcastDeal},                   //广播
    {1, MS_HPLC_REPORT,      "HPLC信息上报",           13,     RCS_NULL,        0,       HplcInfoReport},                  //HPLC信息上报
    {1, MS_HPLC_FR_SET,      "HPLC频点设置",           14,     RCS_NULL,        0,       HplcFrSet},                       //HPLC频点设置
    {1, MS_HPLC_FR_QUERY,    "HPLC频点查询",           15,     RCS_NULL,        0,       HplcFrQuery},                     //HPLC频点查询
    {1, MS_HPLC_AREA,        "HPLC台区识别",           16,     RCS_NULL,        0,       HplcAreaIdentify},                //HPLC台区识别
    {1, MS_HPLC_OFFLINE,     "HPLC离网感知",           17,     RCS_NULL,        0,       HplcOfflineFeel},                 //HPLC离网感知
    {1, MS_HPLC_REFUSE,      "HPLC拒绝节点上报",         18,     RCS_NULL,        0,       HplcRefuseNodeReport},            //HPLC拒绝节点上报
    {1, MS_NORMAL_AMR,       "主动抄表",               20,     RCS_RESTART,    60,       NormalDataReading},               //主动抄表
    {1, 0xFF,                "", 0xFF,   RCS_NULL,       60,       NULL},
};

extern uint32 g_CcoProduct;

/*******************************************************************************
* 函数名称: GetInfoIndex
* 函数功能: 获取当前主动上报消息的index
* 输入参数: 无
* 输出参数: 无
* 返 回 值: 主动上报消息的index
*******************************************************************************/
uint16 GetInfoIndex()
{
    uint16 index =0 ;
    pthread_mutex_lock(&ActiveInfoIndexLock);
    index = ActiveInfoIndex++;
    pthread_mutex_unlock(&ActiveInfoIndexLock);
    return index;
}
#if 0
/*******************************************************************************
* 函数名称: IsExistPlcUpDateFile
* 函数功能: 用于判断是否存在载波路由升级文件
* 输入参数: 模块线程索引(0:左起第一个CCO模块; 1:左起第二个CCO模块)
* 输出参数: 无
* 返 回 值: >0                  成功
*           <0               失败
*******************************************************************************/
int IsExistPlcUpDateFile(uint8 nArrIndex)
{
    char TmpPath[256] = {0};
    sprintf(TmpPath,"rm -rf %s", gRouterUpdateFileBack[nArrIndex]);

    if(access(gRouterUpdateFile[nArrIndex],R_OK) == 0)    //先判断主节点升级包
    {
        //如果存在 ROUTER_UPDATE_FILE_BACK 文件先将其删除
        if(access(gRouterUpdateFileBack[nArrIndex],R_OK) == 0)
        {
            system(TmpPath);
            printf("\n%s\n",TmpPath);
        }

        if(rename(gRouterUpdateFile[nArrIndex], gRouterUpdateFileBack[nArrIndex]) < 0)
        {
            return -1;
        }
        else
        {
            gPlcUpdateFileMark[nArrIndex] = 0x03;
            return 1;
        }

        return 1;
    }
    else if(access(gAllRouterUpdateFile[nArrIndex],R_OK) == 0)    //再判断主节点+子节点升级包
    {
        if(access(gAllRouterUpdateFileBack[nArrIndex],R_OK) == 0)
        {
            sprintf(TmpPath,"rm -rf %s", gAllRouterUpdateFileBack[nArrIndex]);
            system(TmpPath);
            printf("\n%s\n",TmpPath);
        }

        if(rename(gAllRouterUpdateFile[nArrIndex], gAllRouterUpdateFileBack[nArrIndex]) < 0)
        {
            return -1;
        }
        else
        {
            gPlcUpdateFileMark[nArrIndex] = 0x07;
            return 1;
        }

        return 1;
    }
    else if(access(gSubRouterUpdateFile[nArrIndex],R_OK) == 0)   //再判断全部子节点升级包
    {
        if(access(gSubRouterUpdateFileBack[nArrIndex],R_OK) == 0)
        {
            sprintf(TmpPath,"rm -rf %s", gSubRouterUpdateFileBack[nArrIndex]);
            system(TmpPath);
            printf("\n%s\n",TmpPath);
        }

        if(rename(gSubRouterUpdateFile[nArrIndex], gSubRouterUpdateFileBack[nArrIndex]) < 0)
        {
            return -1;
        }
        else
        {
            gPlcUpdateFileMark[nArrIndex] = 0x08;
            return 1;
        }

        return 1;
    }
    
    return -1;
}

/*******************************************************************************
* 函数名称: GetUpdateFileName
* 函数功能: 根据升级文件标识获取需升级文件名
* 输入参数: 模块线程索引(0:左起第一个CCO模块; 1:左起第二个CCO模块)
* 输出参数: 需升级文件名
* 返 回 值: 无
*******************************************************************************/
void GetUpdateFileName(char *TmpPath, uint8 nArrIndex)
{
    if(gPlcUpdateFileMark[nArrIndex] == 0x03)
        sprintf(TmpPath, "%s", gRouterUpdateFileBack[nArrIndex]);
    else if(gPlcUpdateFileMark[nArrIndex] == 0x07)
        sprintf(TmpPath, "%s", gAllRouterUpdateFileBack[nArrIndex]);
    else if(gPlcUpdateFileMark[nArrIndex] == 0x08)
        sprintf(TmpPath, "%s", gSubRouterUpdateFileBack[nArrIndex]);
    return;
}
#endif
/*******************************************************************************
* 函数名称:	UpdateFileSpecialDeal
* 函数功能:	升级文件特殊参数
* 输入参数:	模块线程索引(0:左起第一个CCO模块; 1:左起第二个CCO模块)
* 输出参数:	无
* 返 回 值:	UpdateSpecialProcess结构体数据
*******************************************************************************/
UpdateSpecialProcess UpdateFileSpecialDeal(uint8 nArrIndex)
{
    UpdateSpecialProcess tmpUpdateSpecialProcess;
    tmpUpdateSpecialProcess.isEnableUpdate 	     = 0x01;
	if(MMID_NULL == gModuleManuID[nArrIndex])
		tmpUpdateSpecialProcess.packageLen       = 128;
	else
    {
        tmpUpdateSpecialProcess.packageLen = gLocalCommModuleInfo[nArrIndex].FileTransferMaxSolePackageLen;
        //针对载波模块上报的升级报文单包长度是大于512字节的，默认为128
        if(gLocalCommModuleInfo[nArrIndex].FileTransferMaxSolePackageLen > 512)
            tmpUpdateSpecialProcess.packageLen 		 = 128;
    }

	tmpUpdateSpecialProcess.crcMode 		     = 0x00;
    tmpUpdateSpecialProcess.isNeedDeleteDownFile = 0x00;

    return tmpUpdateSpecialProcess;
}

/*******************************************************************************
* 函数名称: GetUpdateInfo
* 函数功能: 对升级文件进行预读，取得UpdateInfoState结构体信息
* 输入参数: const char *Path              文件路径
            uint16 PackageLen         包长度
            uint8 nArrIndex           模块线程索引(0:左起第一个CCO模块; 1:左起第二个CCO模块)
* 输出参数: UpdateInfoState *uifo     更新文件结构体
* 返 回 值: >0                  成功
*           <0               失败
*******************************************************************************/
int GetUpdateInfo(const char *Path, UpdateInfoState *uifo, uint16 PackageLen, uint8 nArrIndex)
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
        CCODOWN_FMT_DEBUG(nArrIndex, "GetUpdateInfo() open file error!\n");
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

    uifo->TotalPackageNum = package_no;//总的包个数
    uifo->CurrentPackageNo = 0;//当前包序号
    uifo->PackageLen = package_len;//单个包长度

    package_no=0;
    memset(buffer,0x00,sizeof(buffer));
    close(fd);

    return 1;
}

/*******************************************************************************
* 函数名称: PackUpdateFameAndSend
* 函数功能: 载波模块升级组帧，并且发送到缓冲区
* 输入参数: uint16 Len                报文长度限值
            UpdateInfoState Uis   升级过程记录
            uint8 nArrIndex       模块线程索引(0:左起第一个CCO模块; 1:左起第二个CCO模块)
* 输出参数: uint8 *Buffer             发送报文
* 返 回 值: >0                       报文长度
*           <0                    报文组帧失败
*******************************************************************************/
uint16 PackUpdateFameAndSend(uint8 *Buffer, uint16 Len, UpdateInfoState Uis, uint8 nArrIndex)
{
    AppData_T   AppData;
    uint8 FileIdentifying = gFileFlag;       //0x03:主节点;0x07:主节点+子节点;0x08:子节点.
    uint8 FileAttribute = 0x00;      //0x00:启始帧、中间帧, 0x01:结束帧
    uint8 FileCommand = 0x00;                       //报文方式下装
    uint16 TotalPackageNum = Uis.TotalPackageNum;//总段数，总包数
    uint32 PackageNo = Uis.CurrentPackageNo;     //段号，包号
    int PackageLen = Len;                       //段长度，包长度

    UpdateSpecialProcess specialDeal = UpdateFileSpecialDeal(nArrIndex);//不同模块特殊处理,应该根据配置去设置

    if(PackageNo == (TotalPackageNum-1))
    {
        FileAttribute = 0x01;//结束帧
    }

    if(specialDeal.isNeedDeleteDownFile == 0x01)//升级前清除下装文件
    {
        //第一帧将文件标识符设为0x00,路由将其自己得到的升级文件先清除
    }
    if(gFileFlag ==0)
        FileIdentifying = 3;
    else if(gFileFlag ==1)
        FileIdentifying = 7;
    else if(gFileFlag==2)
        FileIdentifying = 8;
    AppData.AFN = 0x15;//AFN 0x15
    AppData.Fn = 1; //FN  0x01

    AppData.DataUnitLen = 11+Len;
    AppData.DataUnitBuff[0] = FileIdentifying;//文件标识
    AppData.DataUnitBuff[1] = FileAttribute;//文件属性
    AppData.DataUnitBuff[2] = FileCommand;//文件指令
    memcpy(&(AppData.DataUnitBuff[3]),(uint8 *)&TotalPackageNum, sizeof(TotalPackageNum));//总段数
    memcpy(&(AppData.DataUnitBuff[5]),(uint8 *)&PackageNo, sizeof(PackageNo));//第i段标识

    if(specialDeal.crcMode == 0x01)//需要crc16校验
    {
        int crc = crc16(Buffer, PackageLen);    //计算CRC
        CCODOWN_FMT_DEBUG(nArrIndex, "当前包的crc ==0x%04x\n", crc);
        int tmpLen = PackageLen+2;//实际发送的长度要加2字节的CRC
        memcpy(&(AppData.DataUnitBuff[9]),(uint8 *)&tmpLen, sizeof(tmpLen));
        memcpy(&(AppData.DataUnitBuff[11]), Buffer, PackageLen);//拷贝数据体
        AppData.DataUnitBuff[11+PackageLen] =(uint8) (crc&0x00ff);//拷贝校验
        AppData.DataUnitBuff[11+PackageLen+1] = crc>>8;    //拷贝校验
        AppData.DataUnitLen += 2;
    }
    else
    {
        memcpy(&(AppData.DataUnitBuff[9]),(uint8 *)&PackageLen, sizeof(PackageLen));//第i段长度
        memcpy(&(AppData.DataUnitBuff[11]), Buffer, PackageLen);//数据
        CCODOWN_FMT_DEBUG(nArrIndex, "PackUpdateFameAndSend() 文件标识=%02x(00清除下装文件, 03主节点升级, 07主子节点升级, 08子节点升级), 文件属性=%02x(00起始帧, 01结束帧), 文件指令=%02x(00报文方式), 总段数=%d, 当前段标识=%u, 当前段数据长度=%d \n", 
            FileIdentifying, FileAttribute, FileCommand, TotalPackageNum, PackageNo, PackageLen);
    }

    return PackFrameData(AppData, nArrIndex);
}


/*******************************************************************************
* 函数名称: ModuleUpdateDeal
* 函数功能: 载波模块本地升级处理
* 输入参数: TASK_FUN_INPUT stTaskFunInput   载波任务具体信息
* 输出参数: 无
* 返 回 值: >0                  成功
*           <0               失败
*******************************************************************************/
int ModuleUpdateDeal(TASK_FUN_INPUT stTaskFunInput)
{
    static time_t       CommBeginTime[MAX_DOWN_THREAD_NUM] = {0};              //通讯开始时间
    static int          CommRetryCnt[MAX_DOWN_THREAD_NUM] = {0};           //通讯重试次数
    int                 RecvLen;                            //接收数据长度
    uint8               RecvBuf[MAX_QUEUE_BUFF_LEN] = {0};        //接收数据缓冲区
    AppData_T           AppData;                        //应用层数据
    int                 RecvResult = 1;                     //接收结果(1:成功 / -1:失败)
    static int          WaitTime[MAX_DOWN_THREAD_NUM] = {0};
    int                 ret = -1;
    uint32              Temp = 0;
    //static   int        PauseDelayCnt[MAX_DOWN_THREAD_NUM] = {0};     //暂停延时计数器
    static int          Fd[MAX_DOWN_THREAD_NUM];               //文件标识符
    static int          Size[MAX_DOWN_THREAD_NUM] = {0};              //每次从文件中读出的大小
    static uint8        Buffer[MAX_DOWN_THREAD_NUM][1024];          //从文件读入的缓冲区
    static uint16       PackageLen[MAX_DOWN_THREAD_NUM] = {0};     //每一段，段长(分包长度)
    //static int          PackageNo[MAX_DOWN_THREAD_NUM] = {0};         //段标号，分包号
    static char         TmpPath[MAX_DOWN_THREAD_NUM][256];          //shell命令缓冲区
    static int          TotalRetryCnt[MAX_DOWN_THREAD_NUM] = {0};     //整体重试次数统计
    static int          UpdateSuccessFlag[MAX_DOWN_THREAD_NUM] = {0}; //升级序列全部完成标记
    //static uint8        UpdataStat[MAX_DOWN_THREAD_NUM] = {0};        //告知GUI升级的状态
    static UpdateInfoState Uis[MAX_DOWN_THREAD_NUM];                //更新文件结构体

    AppData_T           RecvAppData = {0};          //接收缓存区

    uint8           nTtlIndex = 0;                      //不同的端口号对应不同的数组下标
	nTtlIndex = stTaskFunInput.TaskArrIndex;
    
    switch (gTotalState[nTtlIndex].ModuleUpdateDeal)
    {
    case MUD_IDLE:
        CCODOWN_FMT_DEBUG(nTtlIndex, "ModuleUpdateDeal()载波模块本地升级处理...MUD_IDLE \n");
        gUpdateBeginTime = time(NULL);
        gUpdateEndTime = 0;
        gUpdateState = 1;
        //变量初始化
        WaitTime[nTtlIndex] = 0;
        Fd[nTtlIndex] = -1;
        memset(Buffer[nTtlIndex], 0x00, sizeof(Buffer[nTtlIndex]));
        memset(TmpPath[nTtlIndex], 0x00, sizeof(TmpPath[nTtlIndex]));

        memset(&Uis[nTtlIndex], 0x00, sizeof(Uis[nTtlIndex]));

        ClearCommRecvBuff(nTtlIndex);
        
        UpdateSpecialProcess specialDeal = UpdateFileSpecialDeal(nTtlIndex);//不同模块特殊处理,应该根据配置去设置

        PackageLen[nTtlIndex] = specialDeal.packageLen;//每一包的长度
        gTotalState[nTtlIndex].ModuleUpdateDeal = MUD_READ_FILE_DELAY;
        CCODOWN_FMT_DEBUG(nTtlIndex, "即将进入进入载波模块升级处理!\n");

        //UpdataStat[nTtlIndex] = 0x55;//启动升级
        break;
    case MUD_READ_FILE_DELAY:
        ret = GetUpdateInfo(gFilePath, &Uis[nTtlIndex], PackageLen[nTtlIndex], nTtlIndex);
        if(ret < 0)
        {
            gTotalState[nTtlIndex].ModuleUpdateDeal = MUD_END_AGAIN;
            CCODOWN_FMT_DEBUG(nTtlIndex, "GetUpdateInfo()错误, 直接结束!\n");
            break;
        }

        gTotalState[nTtlIndex].ModuleUpdateDeal = MUD_OPEN_FILE;
        CCODOWN_FMT_DEBUG(nTtlIndex, "单包长度=%d, 总的包个数=%d\n", PackageLen[nTtlIndex], Uis[nTtlIndex].TotalPackageNum);
        break;

    case MUD_OPEN_FILE:
        Fd[nTtlIndex] = open(gFilePath,O_RDONLY);
        CCODOWN_FMT_DEBUG(nTtlIndex, "case MUD_OPEN_FILE: Fd=%d \n", Fd[nTtlIndex]);
        if(Fd[nTtlIndex] <= 0)
        {
            CCODOWN_FMT_DEBUG(nTtlIndex, "case MUD_OPEN_FILE: MUD_END_AGAIN \n");
            gTotalState[nTtlIndex].ModuleUpdateDeal = MUD_END_AGAIN;
            UpdateSuccessFlag[nTtlIndex] = 2;   //标识升级文件异常
            break;
        }

        CCODOWN_FMT_DEBUG(nTtlIndex, "case MUD_OPEN_FILE: MUD_READ_FILE \n");
        gTotalState[nTtlIndex].ModuleUpdateDeal = MUD_READ_FILE;
        break;

    case MUD_READ_FILE:
        memset(Buffer[nTtlIndex], 0x00, sizeof(Buffer[nTtlIndex]));
        Size[nTtlIndex] = read(Fd[nTtlIndex], Buffer[nTtlIndex], PackageLen[nTtlIndex]);
        if(Size[nTtlIndex] < 0)
        {
            CCODOWN_FMT_DEBUG(nTtlIndex, "case MUD_READ_FILE: Size=%d \n", Size[nTtlIndex]);
            gTotalState[nTtlIndex].ModuleUpdateDeal = MUD_END_AGAIN;
        }
        else
        {
            gTotalState[nTtlIndex].ModuleUpdateDeal = MUD_SEND;
            CCODOWN_FMT_DEBUG(nTtlIndex, "case MUD_READ_FILE: Size=%d, package_no:%d, package_len:%d \n", Size[nTtlIndex], Uis[nTtlIndex].CurrentPackageNo, PackageLen[nTtlIndex]);
        }

        //在新读出一帧时，将重试次数置0
        CommRetryCnt[nTtlIndex] = 0;
        break;

    case MUD_SEND:
        PackUpdateFameAndSend(Buffer[nTtlIndex], Size[nTtlIndex], Uis[nTtlIndex], nTtlIndex);
        //刷新发送时间
        CommBeginTime[nTtlIndex] = time(NULL);
        //数据发送完成，等待接收
        gTotalState[nTtlIndex].ModuleUpdateDeal = MUD_RECV;
        break;

    case MUD_RECV:                         //接收
        RecvLen = PlcReadQueue(pAmrRecvDown[nTtlIndex], RecvBuf);
        if (RecvLen > 0)
        {
            if (UnPackFrameData(RecvLen, RecvBuf, &AppData) != CHECK_RESULT_SUCCEED)
            {
                CCODOWN_FMT_DEBUG(nTtlIndex, "if (UnPackFrameData() != CHECK_RESULT_SUCCEED) \n");
                RecvResult = -1;
            }

            //非15-01的帧不处理，等待超时
            if((0x15 != AppData.AFN) || (0x01 != AppData.Fn))
            {
                CCODOWN_FMT_DEBUG(nTtlIndex, "if((0x15 != AppData.AFN) || (0x01 != AppData.Fn)) \n");
                break;
            }

            memcpy((uint8 *)&Temp,AppData.DataUnitBuff,AppData.DataUnitLen);
            if((Temp != Uis[nTtlIndex].CurrentPackageNo) || (0xffffffff == Temp))
            {
                CCODOWN_FMT_DEBUG(nTtlIndex, "if((Temp != Uis[nTtlIndex].CurrentPackageNo) || (0xffffffff == Temp)) \n");
                RecvResult = -1;
            }

            if(RecvResult != -1)
            {
                //UpdataStat[nTtlIndex] = 0xA5;
                Uis[nTtlIndex].CurrentPackageNo++;

                gTotalState[nTtlIndex].ModuleUpdateDeal = MUD_READ_FILE;
                if(Uis[nTtlIndex].CurrentPackageNo >= Uis[nTtlIndex].TotalPackageNum)
                {
                    CCODOWN_FMT_DEBUG(nTtlIndex, "if( Uis.CurrentPackageNo(=%d) >= Uis.TotalPackageNum(=%d) ) \n", 
                        Uis[nTtlIndex].CurrentPackageNo, Uis[nTtlIndex].TotalPackageNum);
                    if(gFileFlag == 0x03)  //主模块的升级直接结束
                    {
                        gTotalState[nTtlIndex].ModuleUpdateDeal = MUD_END;
                        UpdateSuccessFlag[nTtlIndex] = 1;
                        CCODOWN_FMT_DEBUG(nTtlIndex, "case MUD_RECV:主模块升级直接结束! \n");
                    }
                    else                            //主节点+子节点,子节点的升级需查询路由运行状态
                    {
                        gTotalState[nTtlIndex].ModuleUpdateDeal = MUD_GET_RUN_STATE;
                        CCODOWN_FMT_DEBUG(nTtlIndex, "case MUD_RECV:主子/子节点升级需要查询路由状态! \n");
                    }
                }

                break;
            }

        }
        else
        {
            if (abs(time(NULL) - CommBeginTime[nTtlIndex]) > MAX_COMM_DELAY_TIME)
            {
                CCODOWN_FMT_DEBUG(nTtlIndex, "case MUD_RECV: 超时! \n");
                RecvResult = -1;
            }
        }

        //接收失败
        if (RecvResult == -1)
        {
            //重试次数+1
            CommRetryCnt[nTtlIndex]++;

            //切换到重试状态
            gTotalState[nTtlIndex].ModuleUpdateDeal = MUD_RETRY;
        }
        break;

    case MUD_RETRY:                        //重试
        //if (CommRetryCnt <= MAX_COMM_RETRY_CNT)
        if (CommRetryCnt[nTtlIndex] <= 4)
        {
            CCODOWN_FMT_DEBUG(nTtlIndex, "case MUD_RETRY: 重试! CommRetryCnt[nTtlIndex]=%d \n", CommRetryCnt[nTtlIndex]);
            gTotalState[nTtlIndex].ModuleUpdateDeal = MUD_SEND;
        }
        else
        {
            CCODOWN_FMT_DEBUG(nTtlIndex, "case MUD_RETRY: 达到最大重试次数, 直接结束! CommRetryCnt[nTtlIndex]=%d \n", CommRetryCnt[nTtlIndex]);
            gTotalState[nTtlIndex].ModuleUpdateDeal = MUD_END_AGAIN;
        }

        break;

    case MUD_END_AGAIN:

        close(Fd[nTtlIndex]);
        Fd[nTtlIndex] = -1;

        //PauseDelayCnt[nTtlIndex] = 0;
        Size[nTtlIndex] = 0;
        memset(Buffer[nTtlIndex],0x00,sizeof(Buffer[nTtlIndex]));
        //PackageNo[nTtlIndex] = 0;
        memset((uint8 *)&Uis[nTtlIndex],0x00,sizeof(Uis[nTtlIndex]));
        memset(TmpPath[nTtlIndex],0x00,sizeof(TmpPath[nTtlIndex]));
        CommRetryCnt[nTtlIndex] = 0;

        TotalRetryCnt[nTtlIndex]++;
        if(TotalRetryCnt[nTtlIndex] <= 3)
        {
            CCODOWN_FMT_DEBUG(nTtlIndex, "case MUD_END_AGAIN: MUD_IDLE\n");
            gTotalState[nTtlIndex].ModuleUpdateDeal = MUD_IDLE;
        }
        else
        {
            TotalRetryCnt[nTtlIndex] = 0;
            CCODOWN_FMT_DEBUG(nTtlIndex, "case MUD_END_AGAIN: MUD_END\n");
            gTotalState[nTtlIndex].ModuleUpdateDeal = MUD_END;
        }

        break;
    case MUD_GET_RUN_STATE:
        //延时20s,查询路由运行状态
        if(IsCloseInnerDelayState((gPastTimeFlag[nTtlIndex].BitSect.Second == TRUE),&WaitTime[nTtlIndex],20) <= 0)
            break;

        //组查询路由状态帧
        AppData.AFN = 0x10;
        AppData.Fn = 4;
        AppData.DataUnitLen = 0;
        ret = CommFunDeal(AppData, &RecvAppData, nTtlIndex);
        if (ret > 0)
        {
            if (RecvAppData.DataUnitBuff[0] & 0x02)                     //上报完成
            {
                CCODOWN_FMT_DEBUG(nTtlIndex, "case MUD_GET_RUN_STATE: 升级还未结束，继续等待\n");
                WaitTime[nTtlIndex] = 0;
            }
            else
            {
                CCODOWN_FMT_DEBUG(nTtlIndex, "case MUD_GET_RUN_STATE: 切换至结束态，不再查询运行状态\n");
                WaitTime[nTtlIndex] = 100;                 //等待值置为最大100s，不再等延时
                UpdateSuccessFlag[nTtlIndex] = 1;          //升级成功
                gTotalState[nTtlIndex].ModuleUpdateDeal = MUD_END;
            }
        }
        else                //未收到主模块的回复，不判断为异常，有可能模块正在重启
        {
            CCODOWN_FMT_DEBUG(nTtlIndex, "case MUD_GET_RUN_STATE: 未收到主模块的回复\n");
            WaitTime[nTtlIndex] = 0;   //等待时间清0,下次再查询
        }
        break;
    case MUD_END:
        //升级完成后，100秒后重启载波进程
        if(IsCloseInnerDelayState((gPastTimeFlag[nTtlIndex].BitSect.Second == TRUE),&WaitTime[nTtlIndex],100) <= 0)
        {
            //UpdataStat[nTtlIndex] = 0x5A;
            return -1;
        }

        CCODOWN_FMT_DEBUG(nTtlIndex, "case MUD_END: 关闭载波模块升级文件\n");
        close(Fd[nTtlIndex]);
        Fd[nTtlIndex] = -1;

        //PauseDelayCnt[nTtlIndex] = 0;
        Size[nTtlIndex] = 0;
        memset(Buffer[nTtlIndex],0x00,sizeof(Buffer[nTtlIndex]));
        //PackageNo[nTtlIndex] = 0;
        memset((uint8 *)&Uis[nTtlIndex],0x00,sizeof(Uis[nTtlIndex]));
        memset(TmpPath[nTtlIndex],0x00,sizeof(TmpPath[nTtlIndex]));
        TotalRetryCnt[nTtlIndex] = 0;

        CCODOWN_FMT_DEBUG(nTtlIndex, "case MUD_END: 删除载波升级文件\n");
        sprintf(TmpPath[nTtlIndex],"rm -rf %s", gFilePath);
        system(TmpPath[nTtlIndex]);
        memset(TmpPath[nTtlIndex],0x00,sizeof(TmpPath[nTtlIndex]));

        if(1 == UpdateSuccessFlag[nTtlIndex])
        {
            CCODOWN_FMT_DEBUG(nTtlIndex, "case MUD_END: 升级文件序列发送完毕\n");
            gUpdateResult = 0;
        }
        else if(2 == UpdateSuccessFlag[nTtlIndex])
        {
            CCODOWN_FMT_DEBUG(nTtlIndex, "case MUD_END: 升级文件异常\n");
            gUpdateResult = 1;
        }else
        {
            CCODOWN_FMT_DEBUG(nTtlIndex, "case MUD_END: 升级文件序列未完成，请重试\n");
            gUpdateResult = 2;
        }
        gTotalState[nTtlIndex].ModuleUpdateDeal = MUD_IDLE;
        //UpdataStat[nTtlIndex] = 0xAA;

        gIsUpdating = 0;
        gUpdateEndTime =time(NULL);
        gUpdateState =  2;
        //CCODOWN_FMT_DEBUG(nTtlIndex, "case MUD_END: 倒计时100秒结束, 马上重启抄表线程!\n");
        //gRebootAmrThread[nTtlIndex] = 1;
        //sleep(3);
        break;

    default:
        gTotalState[nTtlIndex].ModuleUpdateDeal = MUD_IDLE;
        break;
    }

    return -1;
}

/*******************************************************************************
* 函数名称: GetUnitInfo
* 函数功能: 查询通信单元信息(升级后查询版本用)
* 输入参数: uint8 nArrIndex       模块线程索引(0:左起第一个CCO模块; 1:左起第二个CCO模块)
* 输出参数: 无
* 返 回 值: 无
*******************************************************************************/
void GetUnitInfo(uint8 nArrIndex)
{
    uint8           CurNum;                      //本次回复数量
    AppData_T       AppData;
    AppData_T       RecvAppData;
    int             i, j;
    int             ret = -1;
    uint16          TotalNum = 0;
    uint16          CalTotalNum = 0;//累计总量
    int             index = 0;//偏移位
    uint16          CurIndex = 0;//当前索引
    uint8           nTtlIndex = nArrIndex;//不同的端口号对应不同的数组下标

    memset(&gAllCcoStaInfo[nArrIndex], 0x00, sizeof(gAllCcoStaInfo[nArrIndex]));
    memset((uint8 *)&AppData, 0x0, sizeof(AppData_T));

    if(gLocalCommModuleInfo[nArrIndex].LocalCommPatternWord.b1.BitSect.CommPattern != 2)
    {
        return;
    }

    //主动上报信息
    INFO_DATA_T InfoMsgData;
	memset((uint8 *)&InfoMsgData, 0x00, sizeof(InfoMsgData));

    //发送查询报文
    AppData.AFN = 0x10;
    AppData.Fn = 104;
    AppData.DataUnitLen = 3;
    CCODOWN_FMT_DEBUG(nArrIndex, "GetUnitInfo() 查询路由通信单元信息\n");
    for(i=0; (i*15)<MAX_CCO_STA_NUM; i++)   //每帧读64条记录，重复直到读取了全部记录
    {
        *(uint16 *)AppData.DataUnitBuff = (i*15);       //起始序号从0开始
        AppData.DataUnitBuff[2] = 15;
        CCODOWN_FMT_DEBUG(nArrIndex, "查询: 起始序号=%d (0:CCO, 其他:STA), 节点数量=%d \n", i*15, AppData.DataUnitBuff[2]);
        ret = CommFunDeal(AppData, &RecvAppData, nArrIndex);
        if(ret == 2)
        {
            RecvAppData.DataUnitBuff[0] = 0xff;
            RecvAppData.DataUnitBuff[1] = 0xff;
            CCODOWN_FMT_DEBUG(nArrIndex, "GetUnitInfo() 路由信息查询失败\n");
            break;
        }
        TotalNum = *(uint16 *)RecvAppData.DataUnitBuff;//节点总数量
        CurNum = RecvAppData.DataUnitBuff[2];//本次应答数量
        index = 3;

        CCODOWN_FMT_DEBUG(nArrIndex, "GetUnitInfo() i=%d, 节点总数量=%d, 本次应答数量=%d \n", i, TotalNum, CurNum);

        //组装上报消息报文
        memset((uint8 *)&InfoMsgData, 0x00, sizeof(InfoMsgData));
        CurIndex = 0;//当前索引

        InfoMsgData.IP  = inet_addr(gCtsRouterAppIP[nTtlIndex]);
    	InfoMsgData.IP  = ntohl(InfoMsgData.IP);
    	InfoMsgData.port = gCtsRouterAppPort[nTtlIndex];
    	InfoMsgData.MsgPRIORITY = 0;
    	InfoMsgData.MsgRPM = 1;
    	InfoMsgData.MsgIndex = GetInfoIndex();
    	InfoMsgData.MsgLabel = nTtlIndex;  //mqtt存放端口序号(0或1)
    	InfoMsgData.SourLen = strlen(gCcoRouterAppName);
    	memset(InfoMsgData.SourAddr, 0x00, InfoMsgData.SourLen);
    	memcpy(InfoMsgData.SourAddr, gCcoRouterAppName, InfoMsgData.SourLen);
    	InfoMsgData.DestLen = strlen(gCtsRouterAppName[nTtlIndex]);
    	memset(InfoMsgData.DestAddr, 0x00, InfoMsgData.DestLen);
    	memcpy(InfoMsgData.DestAddr, gCtsRouterAppName[nTtlIndex], InfoMsgData.DestLen);
    				
    	InfoMsgData.IID_ID = 0x0003;
    	InfoMsgData.IOP_ID = 0xff01;

    	Uint32ToCharArr(gPortID[nTtlIndex], &InfoMsgData.aPayloadData[CurIndex]);  //端口号
    	CurIndex += 4;
        ChangeLenToBuff(CurNum, &CurIndex, &InfoMsgData.aPayloadData[CurIndex]);  //本次应答节点个数
        
        for(j=0; j<CurNum; j++)
        {
            if(CalTotalNum >= MAX_CCO_STA_NUM)
            {
                break;
            }
            memcpy_r(gAllCcoStaInfo[nArrIndex].aAddr[CalTotalNum], &RecvAppData.DataUnitBuff[index], 6);
            memcpy_r(&InfoMsgData.aPayloadData[CurIndex], &RecvAppData.DataUnitBuff[index], 6);
            index += 6;
            CurIndex += 6;
            memcpy_r(gAllCcoStaInfo[nArrIndex].aInfo[CalTotalNum].aSoftVer, &RecvAppData.DataUnitBuff[index], 2);
            memcpy_r(&InfoMsgData.aPayloadData[CurIndex], &RecvAppData.DataUnitBuff[index], 2);
            index += 2;
            CurIndex += 2;
            gAllCcoStaInfo[nArrIndex].aInfo[CalTotalNum].nVerDay = RecvAppData.DataUnitBuff[index];
            InfoMsgData.aPayloadData[CurIndex] = RecvAppData.DataUnitBuff[index];
            index += 1;
            CurIndex += 1;
            gAllCcoStaInfo[nArrIndex].aInfo[CalTotalNum].nVerMonth = RecvAppData.DataUnitBuff[index];
            InfoMsgData.aPayloadData[CurIndex] = RecvAppData.DataUnitBuff[index];
            index += 1;
            CurIndex += 1;
            gAllCcoStaInfo[nArrIndex].aInfo[CalTotalNum].nVerYear = RecvAppData.DataUnitBuff[index];
            InfoMsgData.aPayloadData[CurIndex] = RecvAppData.DataUnitBuff[index];
            index += 1;
            CurIndex += 1;
            memcpy_r((uint8*)&gAllCcoStaInfo[nArrIndex].aInfo[CalTotalNum].aFacCode[0], &RecvAppData.DataUnitBuff[index], 2);
            memcpy_r(&InfoMsgData.aPayloadData[CurIndex], &RecvAppData.DataUnitBuff[index], 2);
            index += 2;
            CurIndex += 2;
            memcpy_r((uint8*)&gAllCcoStaInfo[nArrIndex].aInfo[CalTotalNum].aChipCode[0], &RecvAppData.DataUnitBuff[index], 2);
            memcpy_r(&InfoMsgData.aPayloadData[CurIndex], &RecvAppData.DataUnitBuff[index], 2);
            index += 2;
            CurIndex += 2;

            if(0 == CalTotalNum)//序号0表示主节点
            {
                gAllCcoStaInfo[nArrIndex].aType[CalTotalNum] = 0;//0主节点 1其他
                InfoMsgData.aPayloadData[CurIndex++] = 0;
            }
            else
            {
                gAllCcoStaInfo[nArrIndex].aType[CalTotalNum] = 1;//0主节点 1其他
                InfoMsgData.aPayloadData[CurIndex++] = 1;
            }

            CCODOWN_FMT_DEBUG(nArrIndex, "第%d个节点信息: 地址 %02x %02x %02x %02x %02x %02x, 软件版本号 %02x %02x, 版本日期 %02x-%02x-%02x, 模块厂商代码 %02x %02x, 芯片代码 %02x %02x, 类型 %d\n", 
                j, gAllCcoStaInfo[nArrIndex].aAddr[CalTotalNum][0], gAllCcoStaInfo[nArrIndex].aAddr[CalTotalNum][1], gAllCcoStaInfo[nArrIndex].aAddr[CalTotalNum][2], 
                gAllCcoStaInfo[nArrIndex].aAddr[CalTotalNum][3], gAllCcoStaInfo[nArrIndex].aAddr[CalTotalNum][4], gAllCcoStaInfo[nArrIndex].aAddr[CalTotalNum][5], 
                gAllCcoStaInfo[nArrIndex].aInfo[CalTotalNum].aSoftVer[0], gAllCcoStaInfo[nArrIndex].aInfo[CalTotalNum].aSoftVer[1], gAllCcoStaInfo[nArrIndex].aInfo[CalTotalNum].nVerYear, 
                gAllCcoStaInfo[nArrIndex].aInfo[CalTotalNum].nVerMonth, gAllCcoStaInfo[nArrIndex].aInfo[CalTotalNum].nVerDay, gAllCcoStaInfo[nArrIndex].aInfo[CalTotalNum].aFacCode[0], 
                gAllCcoStaInfo[nArrIndex].aInfo[CalTotalNum].aFacCode[1], gAllCcoStaInfo[nArrIndex].aInfo[CalTotalNum].aChipCode[0], gAllCcoStaInfo[nArrIndex].aInfo[CalTotalNum].aChipCode[1], 
                gAllCcoStaInfo[nArrIndex].aType[CalTotalNum]);
            
            CalTotalNum ++;
            CCODOWN_FMT_DEBUG(nArrIndex, "当前累计总数量CalTotalNum=%d \n", CalTotalNum);
        }

        InfoMsgData.nPayloadLength = CurIndex;
	    PlcWriteQueue(pAmrSendUp, (uint8 *)&InfoMsgData, sizeof(MSG_INFO_T)+InfoMsgData.nPayloadLength);

        if(CalTotalNum >= MAX_CCO_STA_NUM)
        {
            break;
        }

        if(CurNum < 15)
        {
            break;   //若回复节点数小于查询节点数，则说明超过了最后一个节点序号，不需要再继续查询
        }
        gAmrThreadFlag[nArrIndex] = 1;//喂线程狗
    }

    gAllCcoStaInfo[nArrIndex].nCcoStaNum = CalTotalNum;//总数量
    CCODOWN_FMT_DEBUG(nArrIndex, "GetUnitInfo() 累计总数量nCcoStaNum=%d \n", gAllCcoStaInfo[nArrIndex].nCcoStaNum);

    return;
}

/*******************************************************************************
* 函数名称: ModuleID_Recv
* 函数功能: 解析获取的模块信息
* 输入参数: AppData_T *AppData    模块信息
* 输出参数: ModuleManuID_T *ModuleManuID    模块厂商类型
* 返 回 值: >0 成功; <0 失败
*******************************************************************************/
uint8 ModuleID_Recv(AppData_T  *AppData,ModuleManuID_T *ModuleManuID)
{
	*ModuleManuID = MMID_NOT_NULL;
	return 1;
}

/*******************************************************************************
* 函数名称: UnPackModuleInfo
* 函数功能: 解析获取的模块信息
* 输入参数: AppData_T RecvAppData    模块信息
            uint8 nArrIndex          模块线程索引(0:左起第一个CCO模块; 1:左起第二个CCO模块)
* 输出参数: 无
* 返 回 值: >0 成功; <0 失败
*******************************************************************************/
int UnPackModuleInfo(AppData_T RecvAppData, uint8 nArrIndex)
{
    AppData_T   TmpAppData;
    memset((uint8 *)&TmpAppData,0x00,sizeof(AppData_T));

	uint8 nTtlIndex = 0;  //不同的端口号对应不同的数组下标
	nTtlIndex = nArrIndex;

    GetLocalCommModuleRunningModeInfo(&RecvAppData.DataUnitBuff[0], &gLocalCommModuleInfo[nTtlIndex], sizeof(LOCAL_COMM_MODULE_RUNNING_PATTERN_INFO));

    memcpy(&(TmpAppData.DataUnitBuff[0]), &(gLocalCommModuleInfo[nTtlIndex].ManufacturerInfo.InfoBuff[0]), 9);

    return ModuleID_Recv(&TmpAppData, &gModuleManuID[nTtlIndex]);

}

/*******************************************************************************
* 函数名称: GetMasterAddr_Recv
* 函数功能: 获取主节点地址
* 输入参数: AppData_T  *AppData          模块信息
            uint8 nArrIndex          模块线程索引(0:左起第一个CCO模块; 1:左起第二个CCO模块)
* 输出参数: 无
* 返 回 值: 1 主节点地址与本地相同; 0 主节点地址与本地不同
*******************************************************************************/
uint8 GetMasterAddr_Recv(AppData_T  *AppData, uint8 nArrIndex)
{
    uint8    TmnAddr[6];//终端地址

    ReadTmnAddr(TmnAddr, nArrIndex);  //获取终端地址 

	CCODOWN_FMT_DEBUG(nArrIndex, "GetMasterAddr_Recv() 终端实际地址: \n");
	CCODOWN_BUF_DEBUG(nArrIndex, TmnAddr, 6, "地址");
	CCODOWN_FMT_DEBUG(nArrIndex, "GetMasterAddr_Recv() 03F4召唤地址: \n");
	CCODOWN_BUF_DEBUG(nArrIndex, AppData->DataUnitBuff, 6, "地址");

    if(0 ==  memcmp(TmnAddr,AppData->DataUnitBuff,6))
    {
        return 1;//相等
    }
    else
        return 0;
}

/*******************************************************************************
* 函数名称: RegPlcModule
* 函数功能: 注册载波模块
* 输入参数: uint8 nArrIndex          模块线程索引(0:左起第一个CCO模块; 1:左起第二个CCO模块)
* 输出参数: 无
* 返 回 值: 无
*******************************************************************************/
void RegPlcModule(uint8 nArrIndex)
{
    //将模块信息写入私有文件
	//待完善
}

/*******************************************************************************
* 函数名称: GetModuleManuID
* 函数功能: 获取模块厂商标识
* 输入参数: TASK_FUN_INPUT stTaskFunInput   载波任务具体信息
* 输出参数: 无
* 返 回 值: >0                  成功
*           <0               失败
*******************************************************************************/
int GetModuleManuID(TASK_FUN_INPUT stTaskFunInput)
{
    //printx("获取模块厂商标识GetModuleManuID(TASK_FUN_INPUT stTaskFunInput)...begin \n");
    int             RecvLen;                            //接收数据长度
    uint8           RecvBuf[MAX_QUEUE_BUFF_LEN];        //接收数据缓冲区
    AppData_T       AppData;                            //应用层数据
    AppData_T       RecvAppData;                        //应用层数据
    int             ret;
    uint8           TmnAddr[6];                         //终端地址
    uint8           ModuAdressId[8];
    static  time_t  CommBeginTime[MAX_DOWN_THREAD_NUM]; //通讯开始时间
    uint8           isNeedWait = 0;
    uint8           WaitSeconds = 0;
    static  uint8   RecvActFlag[MAX_DOWN_THREAD_NUM];   //收到模块主动上报信息标志
    
    uint8           nTtlIndex = 0;                      //不同的端口号对应不同的数组下标
    nTtlIndex = stTaskFunInput.TaskArrIndex;

    isNeedWait = stTaskFunInput.InfoMsgBuff[0]; //0:不需要等待上报; 1:需要等待上报模块信息

    //printx(" GetModuleManuID(TASK_FUN_INPUT stTaskFunInput)获取模块厂商标识...nPortID:%d, nTtlIndex:%d\n", nPortID, nTtlIndex);
    
    switch (gTotalState[nTtlIndex].GetManuID)
    {

    case ACT_MODULE_IDLE:                                      //模块初始化状态
        CCODOWN_FMT_DEBUG(nTtlIndex, "GetModuleManuID()模块初始化状态...ACT_MODULE_IDLE \n");
        ClearCommRecvBuff(nTtlIndex);                              //清接收数据
        CheckModuleSoftReset(1, nTtlIndex);                          //在此流程中路由复位属正常现象

        if (g_CcoProduct == CCO_PRODUCT_NO_MODULE)
        {
            PlcResetRouter(nTtlIndex);
        }

        gTotalState[nTtlIndex].GetManuID = ACT_WAIT_MODULEINFO;
        CommBeginTime[nTtlIndex] = time(NULL);
        RecvActFlag[nTtlIndex] = 0;
        if(0 == isNeedWait)
        {
            ClearCommQueueBuff(&gAmrRecvDownReportModule[nTtlIndex]);//清空主动上报模块信息
        }
        CCODOWN_FMT_DEBUG(nTtlIndex, "GetModuleManuID()等待路由主动上报模块厂商信息...60秒(isNeedWait=%d)...ACT_WAIT_MODULEINFO \n", isNeedWait);
        break;

    case ACT_WAIT_MODULEINFO:  //等待路由主动上报模块厂商信息
        //CCODOWN_FMT_DEBUG(nTtlIndex, "GetModuleManuID()等待路由主动上报模块厂商信息...ACT_WAIT_MODULEINFO \n");
        RecvLen = PlcReadQueue(&gAmrRecvDownReportModule[nTtlIndex], RecvBuf);
        if(RecvLen > 0)
        {
            CCODOWN_FMT_DEBUG(nTtlIndex, "收到主动上报数据 RecvLen = PlcReadQueue(gAmrRecvDownReportModule[nTtlIndex], RecvBuf);...RecvLen:%d \n", RecvLen);
            if(UnPackFrameData(RecvLen, RecvBuf, &AppData) == CHECK_RESULT_SUCCEED)
            {
                if ((AppData.AFN == 0x03) && (AppData.Fn == 10))
                {
                    ret = UnPackModuleInfo(AppData, nTtlIndex);
                    if(ret > 0)
                    {
                        RecvActFlag[nTtlIndex] = 1;
                        CCODOWN_FMT_DEBUG(nTtlIndex, "成功! GetModuleManuID()等待路由主动上报模块厂商信息, 收到主动上报AFN03H-F10数据, 报文解析正确! ret:%d \n", ret);
                        CheckModuleSoftReset(0, nTtlIndex);
                    }
                }
            }
        }
        else
        {
            if(1 == RecvActFlag[nTtlIndex])
            {
                gTotalState[nTtlIndex].GetManuID = ACT_MODULE_SEND_GET_ADDRESS;
                break;
            }
            
            if(1 == isNeedWait)
            {
                WaitSeconds = 60;
            }
            else
            {
                WaitSeconds = 0;
            }

            if ( abs(time(NULL) - CommBeginTime[nTtlIndex]) >= WaitSeconds)
            {
                CCODOWN_FMT_DEBUG(nTtlIndex, "未收到上报数据 if ((time(NULL) - CommBeginTime) >= %d 超时...\n", WaitSeconds);
                gTotalState[nTtlIndex].GetManuID = ACT_READ_MODULEINFO;  //重新主动获取
            }
            else
            {
                gTotalState[nTtlIndex].GetManuID = ACT_WAIT_MODULEINFO;  //继续等待主动上报
            }
        }
        break;

    case ACT_READ_MODULEINFO:                                //主动读取本地模块运行模式信息
        CCODOWN_FMT_DEBUG(nTtlIndex, "GetModuleManuID()主动读取本地模块运行模式信息...ACT_READ_MODULEINFO \n");
        AppData.AFN = 0x03;
        AppData.Fn = 10;
        AppData.DataUnitLen = 0;
        CheckModuleSoftReset(1, nTtlIndex);
        ret = CommFunDeal(AppData,&RecvAppData, nTtlIndex);
        if(ret > 0)
        {
            if(ret == 1)
            {
                ret = UnPackModuleInfo(RecvAppData, nTtlIndex);
                if(ret > 0)
                {
                    gTotalState[nTtlIndex].GetManuID = ACT_MODULE_SEND_GET_ADDRESS;
                    CheckModuleSoftReset(0, nTtlIndex);
                    CCODOWN_FMT_DEBUG(nTtlIndex, "成功! GetModuleManuID()主动读取本地模块运行模式信息, 收到AFN03H-F10应答数据, 报文解析正确! ret:%d \n", ret);
                }
            }
            else
            {
                gTotalState[nTtlIndex].GetManuID = ACT_MODULE_IDLE;//从头开始继续执行
            }
        }
        break;

    case ACT_MODULE_SEND_GET_ADDRESS:                        //读取主节点地址
        CCODOWN_FMT_DEBUG(nTtlIndex, "GetModuleManuID()读取主节点地址...ACT_MODULE_SEND_GET_ADDRESS \n");
        AppData.AFN = 0x03;
        AppData.Fn = 4;
        AppData.DataUnitLen = 0;
        ret = CommFunDeal(AppData,&RecvAppData, nTtlIndex);
        if(ret > 0)
        {
            ret = GetMasterAddr_Recv(&RecvAppData, nTtlIndex);
            if(ret == 1)
            {
                gTotalState[nTtlIndex].GetManuID = ACT_MODULE_END;
            }
            else
            {
                gTotalState[nTtlIndex].GetManuID = ACT_MODULE_SEND_SET_ADDRESS;
            }

        }
        break;

    case ACT_MODULE_SEND_SET_ADDRESS:                      //设置主节点地址
        CCODOWN_FMT_DEBUG(nTtlIndex, "GetModuleManuID()设置主节点地址...ACT_MODULE_SEND_SET_ADDRESS \n");

        ReadTmnAddr(TmnAddr, nTtlIndex);
        memset(ModuAdressId, '0', 8);
        memcpy(ModuAdressId,TmnAddr,6);
        AppData.AFN = 0x05;
        AppData.Fn = 1;
        AppData.DataUnitLen = 6;
        memcpy(AppData.DataUnitBuff, TmnAddr, 6);

        ret = CommFunDeal(AppData, &RecvAppData, nTtlIndex);
        if(ret > 0)
        {
            memcpy(&gLocalCommModuleInfo[nTtlIndex].MasterAddr[0], gTerminalAddr[nTtlIndex], 6);
            gTotalState[nTtlIndex].GetManuID = ACT_MODULE_END;
        }
        
        break;

    case ACT_MODULE_END:
        CCODOWN_FMT_DEBUG(nTtlIndex, "GetModuleManuID()结束...ACT_MODULE_END \n");

        RegPlcModule(nTtlIndex);    //注册载波模块
        
        CCODOWN_FMT_DEBUG(nTtlIndex, "端口[%d]周期抄表模式:%02x(0:保留,1:主动,2:被动,3:都支持)\n",nTtlIndex,gLocalCommModuleInfo[nTtlIndex].LocalCommPatternWord.b1.BitSect.AmrCycleMode);
        CCODOWN_FMT_DEBUG(nTtlIndex, "端口[%d]从节点信息模式:%02x\n",nTtlIndex,gLocalCommModuleInfo[nTtlIndex].LocalCommPatternWord.b1.BitSect.SlaverInfoMode);
        CCODOWN_FMT_DEBUG(nTtlIndex, "端口[%d]路由管理方式:%d\n",nTtlIndex,gLocalCommModuleInfo[nTtlIndex].LocalCommPatternWord.b1.BitSect.RouterMangerPattern);
        CCODOWN_FMT_DEBUG(nTtlIndex, "端口[%d]通信方式:%d(1:窄带,2:宽带,3:小无线)\n",nTtlIndex,gLocalCommModuleInfo[nTtlIndex].LocalCommPatternWord.b1.BitSect.CommPattern);
        CCODOWN_FMT_DEBUG(nTtlIndex, "端口[%d]广播命令信道执行方式:%d\n",nTtlIndex,gLocalCommModuleInfo[nTtlIndex].LocalCommPatternWord.b2.BitSect.BroadcastChannelExecPattern);
        CCODOWN_FMT_DEBUG(nTtlIndex, "端口[%d]广播命令确认方式:%d\n",nTtlIndex,gLocalCommModuleInfo[nTtlIndex].LocalCommPatternWord.b2.BitSect.BroadcastAffirmPattern);
        CCODOWN_FMT_DEBUG(nTtlIndex, "端口[%d]失败节点切换发起方式:%d\n",nTtlIndex,gLocalCommModuleInfo[nTtlIndex].LocalCommPatternWord.b2.BitSect.FailureNodeSwitchLaunchPattern);
        CCODOWN_FMT_DEBUG(nTtlIndex, "端口[%d]传输延时参数支持:%d\n",nTtlIndex,gLocalCommModuleInfo[nTtlIndex].LocalCommPatternWord.b2.BitSect.TransDelayParamSupport);
        CCODOWN_FMT_DEBUG(nTtlIndex, "端口[%d]速率数量:%d\n",nTtlIndex,gLocalCommModuleInfo[nTtlIndex].LocalCommPatternWord.b4.BitSect.SpeedNum);//<corrigendum-2013-09-18>
        CCODOWN_FMT_DEBUG(nTtlIndex, "端口[%d]信道数量:%d\n",nTtlIndex,gLocalCommModuleInfo[nTtlIndex].LocalCommPatternWord.b3.BitSect.ChannelNum);
        CCODOWN_FMT_DEBUG(nTtlIndex, "端口[%d]低压电网掉电信息:0X%02x\n",nTtlIndex,gLocalCommModuleInfo[nTtlIndex].LocalCommPatternWord.b3.BitSect.LowVoltageGridPowerDownInfo);//<corrigendum-2013-09-18>
        CCODOWN_FMT_DEBUG(nTtlIndex, "端口[%d]从节点监控最大超时时间:%d\n",nTtlIndex,gLocalCommModuleInfo[nTtlIndex].SlaverMonitorMaxOverTime);
        CCODOWN_FMT_DEBUG(nTtlIndex, "端口[%d]广播命令最大超时时间:%d\n",nTtlIndex,gLocalCommModuleInfo[nTtlIndex].BroadcastMaxOverTime);
        CCODOWN_FMT_DEBUG(nTtlIndex, "端口[%d]最大支持报文长度:%d\n",nTtlIndex,gLocalCommModuleInfo[nTtlIndex].MaxFrameLen);
        CCODOWN_FMT_DEBUG(nTtlIndex, "端口[%d]文件传输支持最大单个数据包长度:%d\n",nTtlIndex,gLocalCommModuleInfo[nTtlIndex].FileTransferMaxSolePackageLen);
        CCODOWN_FMT_DEBUG(nTtlIndex, "端口[%d]升级操作等待时间:%d\n",nTtlIndex,gLocalCommModuleInfo[nTtlIndex].UpdateOperatorWaittingTime);
        CCODOWN_FMT_DEBUG(nTtlIndex, "端口[%d]主节点地址:%02x %02x %02x %02x %02x %02x\n",nTtlIndex,gLocalCommModuleInfo[nTtlIndex].MasterAddr[0],
               gLocalCommModuleInfo[nTtlIndex].MasterAddr[1],gLocalCommModuleInfo[nTtlIndex].MasterAddr[2],
               gLocalCommModuleInfo[nTtlIndex].MasterAddr[3],gLocalCommModuleInfo[nTtlIndex].MasterAddr[4],
               gLocalCommModuleInfo[nTtlIndex].MasterAddr[5]);
        CCODOWN_FMT_DEBUG(nTtlIndex, "端口[%d]最大支持从节点数量:%d\n",nTtlIndex,gLocalCommModuleInfo[nTtlIndex].MaxSlaverNum);
        CCODOWN_FMT_DEBUG(nTtlIndex, "端口[%d]当前从节点数量:%d\n",nTtlIndex,gLocalCommModuleInfo[nTtlIndex].CurrentSlaverNum);
        CCODOWN_FMT_DEBUG(nTtlIndex, "端口[%d]通信模块使用的协议发布日期:%02x-%02x-%02x\n",nTtlIndex,gLocalCommModuleInfo[nTtlIndex].ModuleProtocolReleaseDate[0],gLocalCommModuleInfo[nTtlIndex].ModuleProtocolReleaseDate[1],gLocalCommModuleInfo[nTtlIndex].ModuleProtocolReleaseDate[2]);
        CCODOWN_FMT_DEBUG(nTtlIndex, "端口[%d]通信模块使用的协议最后备案日期:%02x-%02x-%02x\n",nTtlIndex,gLocalCommModuleInfo[nTtlIndex].ModuleProtocolLastRecordDate[0],gLocalCommModuleInfo[nTtlIndex].ModuleProtocolLastRecordDate[1],gLocalCommModuleInfo[nTtlIndex].ModuleProtocolLastRecordDate[2]);
        CCODOWN_FMT_DEBUG(nTtlIndex, "端口[%d]厂商代码:%02x %02x\n",nTtlIndex,gLocalCommModuleInfo[nTtlIndex].ManufacturerInfo.ManufacturerCode[0],gLocalCommModuleInfo[nTtlIndex].ManufacturerInfo.ManufacturerCode[1]);     
        CCODOWN_FMT_DEBUG(nTtlIndex, "端口[%d]芯片代码:%02x %02x\n",nTtlIndex,gLocalCommModuleInfo[nTtlIndex].ManufacturerInfo.ChipCode[0],gLocalCommModuleInfo[nTtlIndex].ManufacturerInfo.ChipCode[1]);
        CCODOWN_FMT_DEBUG(nTtlIndex, "端口[%d]版本日期日:%02x %02x %02x\n",nTtlIndex,gLocalCommModuleInfo[nTtlIndex].ManufacturerInfo.EditionDate[0],gLocalCommModuleInfo[nTtlIndex].ManufacturerInfo.EditionDate[1],gLocalCommModuleInfo[nTtlIndex].ManufacturerInfo.EditionDate[2]);
        CCODOWN_FMT_DEBUG(nTtlIndex, "端口[%d]版本号:%d(0x%04x)\n",nTtlIndex,gLocalCommModuleInfo[nTtlIndex].ManufacturerInfo.EditionInfo,gLocalCommModuleInfo[nTtlIndex].ManufacturerInfo.EditionInfo);

        //载波模块信息赋值到结构体, 供查询使用
        memcpy_r(&gAmrModuleQuery[nTtlIndex].aLocalCommTypeModel[0], (uint8 *)&gLocalCommModuleInfo[nTtlIndex].LocalCommPatternWord, 6);//本地通信模式字
        gAmrModuleQuery[nTtlIndex].nSlaveMonitorMaxOvertime = gLocalCommModuleInfo[nTtlIndex].SlaverMonitorMaxOverTime;//从节点监控最大超时时间(单位:s)
        gAmrModuleQuery[nTtlIndex].nBroadcastMaxOvertime = gLocalCommModuleInfo[nTtlIndex].BroadcastMaxOverTime;//广播命令最大超时时间(单位:s)
        gAmrModuleQuery[nTtlIndex].nMaxAllowedFrameLen = gLocalCommModuleInfo[nTtlIndex].MaxFrameLen;//最大支持的报文长度
        gAmrModuleQuery[nTtlIndex].nMaxUpgradeSingleFrameLen = gLocalCommModuleInfo[nTtlIndex].FileTransferMaxSolePackageLen;//文件传输支持的最大单个数据包长度
        gAmrModuleQuery[nTtlIndex].nUpgradeWaitingPeriod = gLocalCommModuleInfo[nTtlIndex].UpdateOperatorWaittingTime;//升级操作等待时间
        //跳过 广播成功延时时间
        memcpy_r(&gAmrModuleQuery[nTtlIndex].aModuleID[0], &gLocalCommModuleInfo[nTtlIndex].MasterAddr[0], 6);//模块编号
        gAmrModuleQuery[nTtlIndex].nMaxSupportSlaveNum = gLocalCommModuleInfo[nTtlIndex].MaxSlaverNum;//支持的最大从节点数量
        gAmrModuleQuery[nTtlIndex].nCurrentSlaveNum = gLocalCommModuleInfo[nTtlIndex].CurrentSlaverNum;//当前从节点数量

        //直接拷贝(小端)
        memcpy(&gAmrModuleQuery[nTtlIndex].InfoBuff[0], &gLocalCommModuleInfo[nTtlIndex].ManufacturerInfo.InfoBuff[0], 9);//通信模块厂商代码及版本信息(原始9字节)
        memcpy(&gAmrModuleQuery[nTtlIndex].ManufacturerCode[0], &gLocalCommModuleInfo[nTtlIndex].ManufacturerInfo.ManufacturerCode[0], 2);//厂商代码
        memcpy(&gAmrModuleQuery[nTtlIndex].ChipCode[0], &gLocalCommModuleInfo[nTtlIndex].ManufacturerInfo.ChipCode[0], 2);//芯片代码
        memcpy(&gAmrModuleQuery[nTtlIndex].EditionDate[0], &gLocalCommModuleInfo[nTtlIndex].ManufacturerInfo.EditionDate[0], 3);//版本日期日
        gAmrModuleQuery[nTtlIndex].nModuleVer = gLocalCommModuleInfo[nTtlIndex].ManufacturerInfo.EditionInfo;//模块版本

        gTotalState[nTtlIndex].GetManuID = ACT_MODULE_IDLE;
        return 1;
        
        break;
    }

    return -1;
}


/*******************************************************************************
* 函数名称: CustomTaskDeal
* 函数功能: 自定义任务流程
* 输入参数: TASK_FUN_INPUT stTaskFunInput   载波任务具体信息
* 输出参数: 无
* 返 回 值: >0                  成功
*           <0               失败
*******************************************************************************/
int CustomTaskDeal(TASK_FUN_INPUT stTaskFunInput)
{
	return 1;

}


/*******************************************************************************
* 函数名称: CheckMeterFilesResult
* 函数功能: 检查路由档案信息
* 输入参数: char *MeterInfo              档案信息
            uint8 MeterNum           档案数量
            uint8 nArrIndex          模块线程索引(0:左起第一个CCO模块; 1:左起第二个CCO模块)
* 输出参数: 无
* 返 回 值: >0 成功; <0 失败
*******************************************************************************/
uint8 CheckMeterFilesResult(char *MeterInfo,uint8 MeterNum, uint8 nArrIndex)
{
	uint16		i = 0;
	uint8		MtrAddr[6] = {0};
	uint16		MtrIndex = 0;
	uint8		Result = 0;//比较结果(0:相同 1:不同)
	int         ret = 0;
	AMR_METER_FILES_INFO_T MeterFilesInfo;
	uint8	    nProtocolType;	//协议类型
	

	uint8 nTtlIndex = 0;  //不同的端口号对应不同的数组下标
	nTtlIndex = nArrIndex;
	
	if(MeterNum > 0)
	{
		for(i = 0; i < MeterNum; i++)
		{
			//检测地址是否存在于档案列表
			memcpy(MtrAddr, &MeterInfo[8*i], 6);
			nProtocolType = (MeterInfo[8*i+7] >> 3) & 0x07;  //从收到的报文中解析协议
			
			ret = IsExistMtrAddrInAmrInfo(MtrAddr, &MtrIndex, nTtlIndex);  //地址是否存在
			//printx(" CheckMeterFilesResult() nProtocolType:%d, ret:%d \n", nProtocolType, ret);
			
			if((ret >= 0) && (ret == nProtocolType))
			{
				//置下载标志
				CCODOWN_FMT_DEBUG(nTtlIndex, "端口下标:%d, 档案地址:%02x %02x %02x %02x %02x %02x, 协议类型:%d, 存在于抄表结构体! \n", 
					nTtlIndex, MtrAddr[0], MtrAddr[1],MtrAddr[2],MtrAddr[3], MtrAddr[4],MtrAddr[5], nProtocolType);
				gAmrInfo[nTtlIndex].MtrAmrInfo[MtrIndex].SpecialFlag = 1;//一致的标志
			}
			else
			{
				CCODOWN_FMT_DEBUG(nTtlIndex, "端口下标:%d, 档案地址:%02x %02x %02x %02x %02x %02x, 协议类型:%d, 抄表结构体没有该档案, 需要删除! \n", 
					nTtlIndex, MtrAddr[0], MtrAddr[1],MtrAddr[2],MtrAddr[3], MtrAddr[4],MtrAddr[5], nProtocolType);
				//添加至删除队列
				MeterFilesInfo.nPortID = gPortID[nTtlIndex];				
				memcpy(MeterFilesInfo.stAmrMeterFiles.aMeterFilesAddr, MtrAddr, 6);
				MeterFilesInfo.stAmrMeterFiles.nProtocolType = nProtocolType;
				ListInsert_Files(&gToDeleteFilesList[nTtlIndex], gToDeleteFilesList[nTtlIndex].nMeterFilesNum+1, MeterFilesInfo);
				Result = 1;
			}
		}
	}
	return Result;//路由档案一致
}


/*******************************************************************************
* 函数名称: CheckAllMeterFileCmpResult
* 函数功能: 检查终端档案是否均已经比较完成
* 输入参数: uint8 nArrIndex          模块线程索引(0:左起第一个CCO模块; 1:左起第二个CCO模块)
* 输出参数: 无
* 返 回 值: >0 成功; <0 失败
*******************************************************************************/
uint8 CheckAllMeterFileCmpResult(uint8 nArrIndex)
{
    uint16         i;
    uint8          Result = 0;//比较结果(0:相同 1:不同)
	AMR_METER_FILES_INFO_T MeterFilesInfo;

	uint8 nTtlIndex = 0;  //不同的端口号对应不同的数组下标
	nTtlIndex = nArrIndex;

    //查看是否有未下载的测量点
    for (i = 0; i < gAmrInfo[nTtlIndex].TmnAmrInfo.MtrCnt; i++)
    {
        if (gAmrInfo[nTtlIndex].MtrAmrInfo[i].SpecialFlag == 0)
        {
        	Result = 1;
            //通信地址
            GetElem_Files(gAmrMeterFilesList[nTtlIndex], i+1, &MeterFilesInfo);
            CCODOWN_FMT_DEBUG(nTtlIndex, "端口号:%08x, 档案地址:%02x %02x %02x %02x %02x %02x, 通信协议:%d, 未搜到该表档案，需要添加! \n", 
					gPortID[nTtlIndex], MeterFilesInfo.stAmrMeterFiles.aMeterFilesAddr[0], MeterFilesInfo.stAmrMeterFiles.aMeterFilesAddr[1],
					MeterFilesInfo.stAmrMeterFiles.aMeterFilesAddr[2],MeterFilesInfo.stAmrMeterFiles.aMeterFilesAddr[3],
					MeterFilesInfo.stAmrMeterFiles.aMeterFilesAddr[4],MeterFilesInfo.stAmrMeterFiles.aMeterFilesAddr[5],
					MeterFilesInfo.stAmrMeterFiles.nProtocolType);
			gAmrInfo[nTtlIndex].MtrAmrInfo[i].SpecialFlag = 1;
			
            ListInsert_Files(&gToAddFilesList[nTtlIndex], gToAddFilesList[nTtlIndex].nMeterFilesNum+1, MeterFilesInfo);
        }
    }
    return Result;
}


/*******************************************************************************
* 函数名称: MeterFileCmpDeal
* 函数功能: 电表档案比对处理任务
* 输入参数: TASK_FUN_INPUT stTaskFunInput   载波任务具体信息
* 输出参数: 无
* 返 回 值: >0                  成功
*           <0               失败

路由档案的修改新机制:

----> 路由模块如果按照老的档案修改机制,可以再子模块中通过配置来处理

----> 每次档案的读取下发数量采取宏配置:MODULEDEAL_CNT

----> 档案修改分为两种情况:
      -->1)、模块定时判断:
                                   1) 读取路由档案信息
                                   2) 比较后如果有多余或者新增的路由档案,那么判定路由档案不一致
                                   3) 进行档案的修改处理
                                   4) 对路由进行参数初始化
                                   5) 添加终端档案到路由中
      -->2)、主站下发F10修改:
                                   1)读取路由档案信息
                                   2)比较后如果有多余的档案,添加到删除的链表中，如果有新增的档案，添加到新增档案的链表中
                                   3)进行档案的修改处理
                                   4)删除多余的路由档案
                                   5)添加新增的路由档案
                                   6)完成后进入档案的同步比较
                                   7) 读取路由档案信息
                                   8) 比较后如果有多余或者新增的路由档案,那么判定路由档案不一致
                                   9) 进行档案的修改处理
                                   10) 对路由进行参数初始化
                                   11) 添加终端档案到路由中

*******************************************************************************/
int MeterFileCmpDeal(TASK_FUN_INPUT stTaskFunInput)
{
	static	uint8	CmpResult[MAX_DOWN_THREAD_NUM] = {0};					//比较结果(0:相同 1:不同)
	static	uint16	CurIndex[MAX_DOWN_THREAD_NUM] = {0};					//2013版当前索引从1开始
	uint16			i;
	int 			ret;
	AppData_T		AppData;						//应用层数据
	AppData_T		RecvAppData;					//应用层接收数据
	char			MeterInfo[8*MODULEDEAL_CNT];    //电表信息，地址6字节，信息2字节

	uint8       nTtlIndex = 0;  //不同的端口号对应不同的数组下标
	nTtlIndex = stTaskFunInput.TaskArrIndex;
	
	//printx(" int MeterFileCmpDeal() 档案列表[%d]比对 ...begin... \n", nTtlIndex);
	
	switch (gTotalState[nTtlIndex].MeterFileCmp)
	{
	case MFCS_IDLE: 							//空闲		
		CCODOWN_FMT_DEBUG(nTtlIndex, "MeterFileCmpDeal() 档案列表[%d]比对 ...MFCS_IDLE... \n", nTtlIndex);
		ReadAmrInfo(nTtlIndex);  //加载档案信息至抄表信息

		//清下载标志
        for (i = 0; i < gAmrInfo[nTtlIndex].TmnAmrInfo.MtrCnt; i++)
        {
            gAmrInfo[nTtlIndex].MtrAmrInfo[i].SpecialFlag = 0;
        }
		if(gToAddFilesList[nTtlIndex].nMeterFilesNum > 0)//首次需要全部删除
		{
			ClearList_Files(&gToAddFilesList[nTtlIndex]);
		}
		if(gToDeleteFilesList[nTtlIndex].nMeterFilesNum > 0)//首次需要全部删除
		{
			ClearList_Files(&gToDeleteFilesList[nTtlIndex]);
		}
		CurIndex[nTtlIndex] = 1;//2013版当前索引从1开始

		gTotalState[nTtlIndex].MeterFileCmp = MFCS_READ_AMOUNT_FILE;
		
		break;

	case MFCS_READ_AMOUNT_FILE: 				//读取从节点数量
		CCODOWN_FMT_DEBUG(nTtlIndex, "int MeterFileCmpDeal() 档案列表[%d]比对 ...读取从节点数量MFCS_READ_AMOUNT_FILE... \n", nTtlIndex);
		AppData.AFN = 0x10;
		AppData.Fn = 1;
		AppData.DataUnitLen = 0;
		ret = CommFunDeal(AppData,&RecvAppData, nTtlIndex);
		if (ret > 0)
		{
			memcpy(&gAmrInfo[nTtlIndex].TmnAmrInfo.SlaveCnt,RecvAppData.DataUnitBuff,2);  //从节点总数量，2字节
			memcpy(&gAmrInfo[nTtlIndex].TmnAmrInfo.MaxAllowSlaveCnt,&RecvAppData.DataUnitBuff[2],2);  //路由支持最大从节点数量，2字节
			//从路由中读取的从节点数量大于系统支持的最大数量时-容错
			if(gAmrInfo[nTtlIndex].TmnAmrInfo.SlaveCnt > MAX_AMR_COLLECT_CNT)
			{
				gAmrInfo[nTtlIndex].TmnAmrInfo.SlaveCnt = gAmrInfo[nTtlIndex].TmnAmrInfo.MtrCnt;
			}

			CCODOWN_FMT_DEBUG(nTtlIndex, "MeterFileCmpDeal()端口[%d]从节点总数量:%d, 最大支持数量:%d \n"
				   , nTtlIndex, gAmrInfo[nTtlIndex].TmnAmrInfo.SlaveCnt, gAmrInfo[nTtlIndex].TmnAmrInfo.MaxAllowSlaveCnt);

			CmpResult[nTtlIndex] = 0;
			gTotalState[nTtlIndex].MeterFileCmp = MFCS_READ_INFO_FILE;
		}
		break;

	case MFCS_READ_INFO_FILE:					//读取从节点信息
		CCODOWN_FMT_DEBUG(nTtlIndex, "MeterFileCmpDeal() 档案列表[%d]比对  ...读取从节点信息MFCS_READ_INFO_FILE...CurIndex:%d SlaveCnt:%d \n", nTtlIndex, CurIndex[nTtlIndex], gAmrInfo[nTtlIndex].TmnAmrInfo.SlaveCnt);
		if(CurIndex[nTtlIndex] > gAmrInfo[nTtlIndex].TmnAmrInfo.SlaveCnt)
		{
			gTotalState[nTtlIndex].MeterFileCmp = MFCS_RESULT_DEAL;
		}
		else
		{
			AppData.AFN = 0x10;
			AppData.Fn = 2;
			AppData.DataUnitLen = 3;
			memcpy(AppData.DataUnitBuff,&CurIndex[nTtlIndex],2);  //从节点起始序号，2字节
			AppData.DataUnitBuff[2] = MODULEDEAL_CNT;  //从节点数量，1字节
			ret = CommFunDeal(AppData,&RecvAppData, nTtlIndex);
			CCODOWN_FMT_DEBUG(nTtlIndex, "读取从节点信息 ret = CommFunDeal(AppData,&RecvAppData, nPortID); ret=[%d] \n", ret);
			if(ret > 0)
			{
				memcpy(MeterInfo,&RecvAppData.DataUnitBuff[3],RecvAppData.DataUnitBuff[2]*8);  //recv从节点总数量2字节，本次应答从节点1字节，后面是6字节地址+2字节信息
				if(CheckMeterFilesResult(MeterInfo, RecvAppData.DataUnitBuff[2], nTtlIndex) > 0)
				{
					CmpResult[nTtlIndex] = 1;
				}

				CurIndex[nTtlIndex] += RecvAppData.DataUnitBuff[2];

				if(CmpResult[nTtlIndex] > 0 && gMeterFilesModifyType[nTtlIndex] == FILES_MODEFY_CLEAR)
                {
                    gTotalState[nTtlIndex].MeterFileCmp = MFCS_RESULT_DEAL;
                    break;
                }
			}
		}
		break;

	case MFCS_RESULT_DEAL:						//结果处理
		CCODOWN_FMT_DEBUG(nTtlIndex, "MeterFileCmpDeal() 档案列表[%d]比对 ...结果处理MFCS_RESULT_DEAL... \n", nTtlIndex);

		if(CheckAllMeterFileCmpResult(nTtlIndex) > 0)
		{
			CmpResult[nTtlIndex] = 1;
		}

		//比较结果不同，置档案修改标志
		if (CmpResult[nTtlIndex] == 1)//不同
		{
			if(gMeterFilesModifyType[nTtlIndex] == FILES_MODEFY_CLEAR)
			{
				//将cco待删除档案全部清空
				ClearList_Files(&gToDeleteFilesList[nTtlIndex]);

				//将当前档案全局变量全部作为待添加档案
				memcpy(&gToAddFilesList[nTtlIndex], &gAmrMeterFilesList[nTtlIndex], sizeof(gToAddFilesList[nTtlIndex]));
			}
			gMeterFileModifyFlag[nTtlIndex] = 1; //直接进行修改
			CCODOWN_FMT_DEBUG(nTtlIndex, "int MeterFileCmpDeal() 档案列表[%d]比对不一致, 需要同步... \n", nTtlIndex);
		}
		else//完全相同
		{
			gMeterFilesModifyType[nTtlIndex] = FILES_MODEFY_NULL;
			gMeterFileModifyState[nTtlIndex] = 0;
			CCODOWN_FMT_DEBUG(nTtlIndex, "MeterFileCmpDeal() 档案列表[%d]比对一致，不需要进行同步操作... \n", nTtlIndex);
		}

		CCODOWN_FMT_DEBUG(nTtlIndex, "比较后需要添加的从节点信息gToAddFilesList[%d] :  \n", nTtlIndex);
		for(i=0; i<gToAddFilesList[nTtlIndex].nMeterFilesNum; i++)
		{
			PrintFilesDetail(gToAddFilesList[nTtlIndex].stMeterFilesData[i], nTtlIndex);
		}
		CCODOWN_FMT_DEBUG(nTtlIndex, "比较后需要删除的从节点信息gToDeleteFilesList[%d] :  \n", nTtlIndex);
		for(i=0; i<gToDeleteFilesList[nTtlIndex].nMeterFilesNum; i++)
		{
			PrintFilesDetail(gToDeleteFilesList[nTtlIndex].stMeterFilesData[i], nTtlIndex);
		}				

		gTotalState[nTtlIndex].MeterFileCmp = MFCS_IDLE;
		return 1;
		break;

	default:
		gTotalState[nTtlIndex].MeterFileCmp = MFCS_IDLE;
		break;
	}

	return -1;
}


/*******************************************************************************
* 函数名称: Comm_AddSlaveInfo_New
* 函数功能: 增加从节点信息
* 输入参数: AMR_METER_FILES_LIST_T *PModuleInfo     新增从节点信息
            uint8 AddNum                            新增从节点数量
            uint8 nArrIndex          模块线程索引(0:左起第一个CCO模块; 1:左起第二个CCO模块)
* 输出参数: 无
* 返 回 值: >0 成功; <0 失败
*******************************************************************************/
int8 Comm_AddSlaveInfo_New(AMR_METER_FILES_LIST_T *PModuleInfo, uint8 AddNum, uint8 nArrIndex)
{
	CCODOWN_FMT_DEBUG(nArrIndex, "Comm_AddSlaveInfo_New()端口[%d]增加从节点信息...begin...\n", nArrIndex);

    int                 i;
    AppData_T           AppData;
    AppData_T           RecvAppData;
    uint8               AddCnt = 0;
	int                 nMeterFilesNum = 0;

    AppData.AFN = 0x11;
    AppData.Fn = 1;

    for(i = 0; i < AddNum; i++)
    {
		nMeterFilesNum = PModuleInfo->nMeterFilesNum;
        if(nMeterFilesNum > 0)
        {
        	AddCnt++;
            memcpy(&AppData.DataUnitBuff[1+7*i], PModuleInfo->stMeterFilesData[nMeterFilesNum-1].stAmrMeterFiles.aMeterFilesAddr, 6);
            switch (PModuleInfo->stMeterFilesData[nMeterFilesNum-1].stAmrMeterFiles.nProtocolType)
			{
				//0x00为透明传输，0x01为97表，0x02为07表，0x03为面向对象表
				case PRTL_97:
				{
					AppData.DataUnitBuff[1+7*i+6] = 0x01;
					break;
				}
				case PRTL_07:
				{
					AppData.DataUnitBuff[1+7*i+6] = 0x02;
					break;
				}
				case PRTL_OOP:
				{
					AppData.DataUnitBuff[1+7*i+6] = 0x03;
					break;
				}
				default:
					break;
			}
			
			CCODOWN_FMT_DEBUG(nArrIndex, "Comm_AddSlaveInfo_New()端口[%d]增加的从节点信息为: \n", nArrIndex);
			PrintFilesDetail(PModuleInfo->stMeterFilesData[nMeterFilesNum-1], nArrIndex);
			ListDeleteByIndex_Files(PModuleInfo, nMeterFilesNum);
			
        }
        else
            break;

    }

    AppData.DataUnitLen = 1 + 7*AddCnt;
    AppData.DataUnitBuff[0] = AddCnt;

    return CommFunDeal(AppData, &RecvAppData, nArrIndex);

}


/*******************************************************************************
* 函数名称: Comm_DeleteRouterInfo_New
* 函数功能: 删除从节点信息
* 输入参数: AMR_METER_FILES_LIST_T *PModuleInfo     删除从节点信息
            uint8 DeleteNum                         删除从节点数量
            uint8 nArrIndex          模块线程索引(0:左起第一个CCO模块; 1:左起第二个CCO模块)
* 输出参数: 无
* 返 回 值: >0 成功; <0 失败
*******************************************************************************/
int8 Comm_DeleteRouterInfo_New(AMR_METER_FILES_LIST_T *PModuleInfo, uint8 DeleteNum, uint8 nArrIndex)
{
	CCODOWN_FMT_DEBUG(nArrIndex, "Comm_DeleteRouterInfo_New()端口[%d]删除从节点信息...begin...\n", nArrIndex);
	
    AppData_T           AppData;
    AppData_T           RecvAppData;
    uint8               DeleteCnt = 0;
    int                 i;
	int                 nMeterFilesNum = 0;

    AppData.AFN = 0x11;
    AppData.Fn = 2;

	for(i = 0; i < DeleteNum; i++)
    {
		nMeterFilesNum = PModuleInfo->nMeterFilesNum;
        if(nMeterFilesNum > 0)
        {
        	DeleteCnt++;
            memcpy(&AppData.DataUnitBuff[1+6*i], PModuleInfo->stMeterFilesData[nMeterFilesNum-1].stAmrMeterFiles.aMeterFilesAddr, 6);

			CCODOWN_FMT_DEBUG(nArrIndex, "Comm_DeleteRouterInfo_New()端口[%d]删除的从节点信息为: \n", nArrIndex);
			PrintFilesDetail(PModuleInfo->stMeterFilesData[nMeterFilesNum-1], nArrIndex);
			ListDeleteByIndex_Files(PModuleInfo, nMeterFilesNum);
        }
        else
            break;

    }

    AppData.DataUnitLen = 1 + 6*DeleteCnt;
    AppData.DataUnitBuff[0] = DeleteCnt;

    return CommFunDeal(AppData, &RecvAppData, nArrIndex);
}


/*******************************************************************************
* 函数名称: MeterFileMofifyDeal
* 函数功能: 电表档案修改处理
* 输入参数: TASK_FUN_INPUT stTaskFunInput   载波任务具体信息
* 输出参数: 无
* 返 回 值: >0                  成功
*           <0               失败
*******************************************************************************/
int MeterFileMofifyDeal(TASK_FUN_INPUT stTaskFunInput)
{
	int 					ret;
    AppData_T				AppData;
    AppData_T				RecvAppData;

	uint8       nTtlIndex = 0;  //不同的端口号对应不同的数组下标
	nTtlIndex = stTaskFunInput.TaskArrIndex;

	//printx(" MeterFileMofifyDeal(TASK_FUN_INPUT stTaskFunInput)档案列表[%d]修改...begin...\n", nTtlIndex);

    switch(gTotalState[nTtlIndex].MeterFileModify)
    {
    case MFMS_IDLE: 							//空闲
    	CCODOWN_FMT_DEBUG(nTtlIndex, "MeterFileMofifyDeal()档案列表[%d]修改...空闲MFMS_IDLE...\n", nTtlIndex);

		ReadAmrInfo(nTtlIndex);  //加载档案信息至抄表信息
		
        if(gMeterFilesModifyType[nTtlIndex] == FILES_MODEFY_UPDATE)
        {
			CCODOWN_FMT_DEBUG(nTtlIndex, "==>  进入F10方式档案修改处理FILES_MODEFY_UPDATE \n");
			gTotalState[nTtlIndex].MeterFileModify = MFMS_FILE_DELETE;
		}
        else
        {
			CCODOWN_FMT_DEBUG(nTtlIndex, "==>  进入参数初始化方式档案修改处理FILES_MODEFY_CLEAR \n");
			gTotalState[nTtlIndex].MeterFileModify = MFMS_FILE_CLEAR;
		}
        break;

    case MFMS_FILE_CLEAR:						//档案清除
        CCODOWN_FMT_DEBUG(nTtlIndex, "MeterFileMofifyDeal()档案列表[%d]修改...档案清除MFMS_FILE_CLEAR...\n", nTtlIndex);
		AppData.AFN = 0x01;
        AppData.Fn = 2;
        AppData.DataUnitLen = 0;
        ret = CommFunDeal(AppData, &RecvAppData, nTtlIndex);
        if (ret > 0)							//完成，切换到下一状态
        {
            gTotalState[nTtlIndex].MeterFileModify = MFMS_FILE_ADD;
        }
        else
        {
            gTotalState[nTtlIndex].MeterFileModify  = MFMS_IDLE;
        }
        break;


    case MFMS_FILE_ADD: 						//档案增加
        CCODOWN_FMT_DEBUG(nTtlIndex, "MeterFileMofifyDeal()档案列表[%d]修改...档案增加MFMS_FILE_ADD...\n", nTtlIndex);
		if(gToAddFilesList[nTtlIndex].nMeterFilesNum > 0)
        {
            ret = Comm_AddSlaveInfo_New(&gToAddFilesList[nTtlIndex], MODULEDEAL_CNT, nTtlIndex);
			CCODOWN_FMT_DEBUG(nTtlIndex, "档案列表[%d]修改 Comm_AddSlaveInfo_New()...ret=%d...\n", nTtlIndex, ret);
            if(ret > 0)
            {
                
            }
        }
        else
        {
        	gTotalState[nTtlIndex].MeterFileModify = MFMS_RESTART;
            
        }
        break;

    case MFMS_FILE_DELETE:							 //档案删除
        CCODOWN_FMT_DEBUG(nTtlIndex, "MeterFileMofifyDeal()档案列表[%d]修改...档案删除MFMS_FILE_DELETE...\n", nTtlIndex);
		if(gToDeleteFilesList[nTtlIndex].nMeterFilesNum > 0)
        {
			ret = Comm_DeleteRouterInfo_New(&gToDeleteFilesList[nTtlIndex], MODULEDEAL_CNT, nTtlIndex);
			CCODOWN_FMT_DEBUG(nTtlIndex, "档案列表[%d]修改 Comm_DeleteRouterInfo_New()...ret=%d...\n", nTtlIndex, ret);
            if(ret > 0)
            {
                
            }
		}
        else
        {
        	gTotalState[nTtlIndex].MeterFileModify = MFMS_FILE_ADD;
            
        }
        break;

    case MFMS_RESTART:							//重启
    	
		CCODOWN_FMT_DEBUG(nTtlIndex, "MeterFileMofifyDeal()档案列表[%d]修改...重启MFMS_RESTART...\n", nTtlIndex);

		if((gToAddFilesList[nTtlIndex].nMeterFilesNum > 0) || (gToDeleteFilesList[nTtlIndex].nMeterFilesNum > 0))
        {
			CCODOWN_FMT_DEBUG(nTtlIndex, "MeterFileMofifyDeal()档案列表[%d]本次尚未全部同步, 需要下次继续...MFMS_RESTART...\n", nTtlIndex);
			gMeterFileModifyFlag[nTtlIndex] = 1;
		}
		else
		{				
			gMeterFileModifyState[nTtlIndex] = 0;
            gMeterFileCompareFlag[nTtlIndex] = 1;
		}
        
		gTotalState[nTtlIndex].MeterFileModify = MFMS_IDLE;
		return 1;

    default:
        gTotalState[nTtlIndex].MeterFileModify = MFMS_IDLE;
        break;
    }

    return -1;
}

/*******************************************************************************
* 函数名称: ActReportNodeDeal
* 函数功能: 主动上报节点处理
* 输入参数: 无
* 输出参数: uint8 *ReportInfo                  上报从节点信息
            uint8 *RealPhase               实测相位
            uint8 *AddrSearchReportType    上报从节点模式
            AppData_T *AppData             上报从节点报文
            uint8 nArrIndex                模块线程索引(0:左起第一个CCO模块; 1:左起第二个CCO模块)
* 返 回 值: >0                  上报从节点数量
           =-1               无上报
           =-2               注册结束
*******************************************************************************/
int ReportInfoDeal(uint8 *ReportInfo, uint8 *RealPhase, uint8 *AddrSearchReportType, AppData_T *AppData, uint8 nArrIndex)
{
    if((AppData->AFN == 0x06) && (AppData->Fn == 1))
    {
        //填充上报信息
        memcpy(ReportInfo, &AppData->DataUnitBuff[1], (AppData->DataUnitLen-1));

        *AddrSearchReportType  = 1;

        *RealPhase = (AppData->RInfo[2] & 0x0f); //相位
        *RealPhase += (AppData->RInfo[3] & 0xf0); //信号品质

        //确认帧应答
        Comm_ConfirmAckTakeFrameSeq(*AppData, nArrIndex);

        return AppData->DataUnitBuff[0];
    }

    if ((AppData->AFN == 0x06) && (AppData->Fn == 4))
    {
        //填充上报信息
        memcpy(ReportInfo, &AppData->DataUnitBuff[1], (AppData->DataUnitLen-1));

        *AddrSearchReportType  = 4;

        *RealPhase = (AppData->RInfo[2] & 0x0f); //相位
        *RealPhase += (AppData->RInfo[3] & 0xf0); //信号品质

        //确认帧应答
        Comm_ConfirmAckTakeFrameSeq(*AppData, nArrIndex);

        return AppData->DataUnitBuff[0];
    }

    return -1;
}


/*******************************************************************************
* 函数名称: ActReportNodeDeal
* 函数功能: 主动上报节点处理
* 输入参数: 无
* 输出参数: ReportInfo          上报从节点信息
*           RealPhase           实测相位
            AddrSearchReportType 上报从节点模式

* 返 回 值: >0                  上报从节点数量
*           =-1                 无上报
*           =-2                 注册结束
*******************************************************************************/
int ActReportNodeDeal(uint8 *ReportInfo, uint8 *RealPhase, uint8 *AddrSearchReportType, uint8 nArrIndex)
{
    int     RecvLen;                            //接收数据长度
    uint8   RecvBuf[MAX_QUEUE_BUFF_LEN];        //接收数据缓冲区
    AppData_T   AppData;                        //应用层数据
    
    uint8   nTtlIndex = 0;  //不同的端口号对应不同的数组下标
	nTtlIndex = nArrIndex;

    //读取数据
    RecvLen = PlcReadQueue(&gAmrRecvDownSearchMeter[nTtlIndex], RecvBuf);
	
    if (RecvLen <= 0)
    {
        return -1;
    }

    //数据解帧
    if (UnPackFrameData(RecvLen, RecvBuf, &AppData) != CHECK_RESULT_SUCCEED)
    {
        return -1;
    }

    if((AppData.AFN == 0x06) && (AppData.Fn == 1 || AppData.Fn == 4))
    {
        return ReportInfoDeal(ReportInfo, RealPhase, AddrSearchReportType, &AppData, nArrIndex);
    }

    if ((AppData.AFN == 0x06) && (AppData.Fn == 3))//上报路由工况变动信息 1:抄表任务结束  2:搜表任务结束
    {
        if (AppData.DataUnitBuff[0] == 0x02)
        {
            //确认帧应答
            Comm_ConfirmAckTakeFrameSeq(AppData, nTtlIndex);
            return -2;
        }
        //确认帧应答
        Comm_ConfirmAckTakeFrameSeq(AppData, nTtlIndex);
    }

    return -1;
}

/*******************************************************************************
* 函数名称: SaveReportInfo
* 函数功能: 保存上报信息
* 输入参数: ReportCnt           上报个数
*           ReportInfo          上报信息
*           RealPhase           实测相位
* 输出参数: 无
* 返 回 值: 无
*******************************************************************************/
void SaveReportInfo_F1(uint8 ReportCnt, uint8 *ReportInfo, uint8 RealPhase, uint8 nArrIndex)
{
    CCODOWN_FMT_DEBUG(nArrIndex, "SaveReportInfo_F1(ReportCnt[%d], *ReportInfo[%d], RealPhase[%d], nArrIndex[%d])...begin \n", ReportCnt, *ReportInfo, RealPhase, nArrIndex);
    ACT_REPORT_INFO_T ActInfo;
    uint16  MeterCnt = 0;                       //电表数目
    uint16  CurIndex = 0;                       //当前索引
    //uint16  SearchFilesIndex = 0;               //档案序号(搜索上报序号,非原始档案序号)
    int     i;
    int     pn = 0;

    AMR_METER_SEARCH_REPORT_T AmrMeterSearchReport;
    memset((uint8 *)&AmrMeterSearchReport, 0x00, sizeof(AmrMeterSearchReport));

    INFO_DATA_T InfoMsgData;
    memset((uint8 *)&InfoMsgData, 0x00, sizeof(InfoMsgData));

    uint8 nTtlIndex = 0;  //不同的端口号对应不同的数组下标
    nTtlIndex = nArrIndex;

    //填充上报信息
    for (i = 0; i < ReportCnt; i++)
    {
        //载波表地址
        memcpy(ActInfo.MeterAddr, &ReportInfo[CurIndex], 6);  //注意：ReportInfo不包含首字节“上报从节点的数量n”，直接从地址开始
        CurIndex += 6;

        ActInfo.PrtlMode = ReportInfo[CurIndex]; //规约类型
        CurIndex += 1;

        CurIndex += 2; //跳过从节点序号
        CCODOWN_FMT_DEBUG(nTtlIndex, "此设备为载波表 \n");
        
        //有效性检测
        if(1)
        {
            //载波表地址
            memcpy(AmrMeterSearchReport.stAmrMeterFiles[MeterCnt].aMeterFilesAddr, ActInfo.MeterAddr, 6);  //注意：ReportInfo不包含首字节“上报从节点的数量n”，直接从地址开始
            CurIndex += 6;

            //规约类型
            AmrMeterSearchReport.stAmrMeterFiles[MeterCnt].nProtocolType = ActInfo.PrtlMode; 
            CurIndex += 1;

            //跳过从节点序号
            CurIndex += 2; 

            MeterCnt++;
        }

        CCODOWN_FMT_DEBUG(nTtlIndex, "第 %d 个电表地址: %02x %02x %02x %02x %02x %02x, 协议:%02x \n", i, 
            ActInfo.MeterAddr[5], ActInfo.MeterAddr[4], ActInfo.MeterAddr[3], ActInfo.MeterAddr[2], ActInfo.MeterAddr[1], ActInfo.MeterAddr[0], ActInfo.PrtlMode);
    }

    CCODOWN_FMT_DEBUG(nTtlIndex, "SaveReportInfo_F1() 解析之后 MeterCnt=%d \n", MeterCnt);
    if(MeterCnt <= 0)
    {
        CCODOWN_FMT_DEBUG(nTtlIndex, "SaveReportInfo_F1() if(MeterCnt <= 0) 直接返回! \n", MeterCnt);
        return;
    }

    //组装消息报文
    InfoMsgData.IP  = inet_addr(gCtsRouterAppIP[nTtlIndex]);
    InfoMsgData.IP  = ntohl(InfoMsgData.IP);
    InfoMsgData.port = gCtsRouterAppPort[nTtlIndex];
    InfoMsgData.MsgPRIORITY = 0;
    InfoMsgData.MsgRPM = 1;
    InfoMsgData.MsgIndex = GetInfoIndex();
    InfoMsgData.MsgLabel = nTtlIndex;  //mqtt存放端口序号(0或1)
    InfoMsgData.SourLen = strlen(gCcoRouterAppName);
    memset(InfoMsgData.SourAddr, 0x00, InfoMsgData.SourLen);
    memcpy(InfoMsgData.SourAddr, gCcoRouterAppName, InfoMsgData.SourLen);
    InfoMsgData.DestLen = strlen(gCtsRouterAppName[nTtlIndex]);
    memset(InfoMsgData.DestAddr, 0x00, InfoMsgData.DestLen);
    memcpy(InfoMsgData.DestAddr, gCtsRouterAppName[nTtlIndex], InfoMsgData.DestLen);
                
    CurIndex = 0;
    InfoMsgData.IID_ID = 0x0003;
    InfoMsgData.IOP_ID = 0x0036;

    Uint32ToCharArr(gPortID[nTtlIndex], &InfoMsgData.aPayloadData[CurIndex]);  //端口号
    CurIndex += 4;
    InfoMsgData.aPayloadData[CurIndex] = 0x00;  //是否含有抄读信息标志, 0x00:无, 0x01:有
    CurIndex += 1;
    ChangeLenToBuff(MeterCnt, &CurIndex, &InfoMsgData.aPayloadData[CurIndex]);  //表计档案的个数
    for (i = 0; i < MeterCnt; i++)
    {
        pn = GetPnByMeterAddr(AmrMeterSearchReport.stAmrMeterFiles[i].aMeterFilesAddr, nTtlIndex);
        if(pn <= 0) //本地没有该电表档案，以收到的报文为准
        {
            //SearchFilesIndex = gSearchMeterFilesIndex[nTtlIndex] ++;
            //Uint16ToCharArr(SearchFilesIndex, &InfoMsgData.aPayloadData[CurIndex]);  //档案序号
            //CurIndex += 2;
            //CurIndex += 1;  //跳过地址长度
            memcpy_r(&InfoMsgData.aPayloadData[CurIndex], AmrMeterSearchReport.stAmrMeterFiles[i].aMeterFilesAddr, 6);  //档案地址
            CurIndex += 6;
            //CurIndex += 1;  //跳过mac长度
            memcpy_r(&InfoMsgData.aPayloadData[CurIndex], AmrMeterSearchReport.stAmrMeterFiles[i].aMeterFilesAddr, 6);  //档案mac
            CurIndex += 6; 
            InfoMsgData.aPayloadData[CurIndex] = AmrMeterSearchReport.stAmrMeterFiles[i].nProtocolType;  //协议类型(此处以档案为准, 还是以收到的报文为准?)
            CurIndex += 1;
            //InfoMsgData.aPayloadData[CurIndex] = 0;  //通信参数
            //CurIndex += 1; 
        }
        else
        {
            //SearchFilesIndex = gSearchMeterFilesIndex[nTtlIndex] ++;
            //Uint16ToCharArr(SearchFilesIndex, &InfoMsgData.aPayloadData[CurIndex]);  //档案序号
            //CurIndex += 2;
            //CurIndex += 1;  //跳过地址长度
            memcpy_r(&InfoMsgData.aPayloadData[CurIndex], gAmrInfo[nTtlIndex].MtrAmrInfo[pn-1].aMeterFilesAddr, 6);  //档案地址
            CurIndex += 6;
            //CurIndex += 1;  //跳过mac长度
            memcpy_r(&InfoMsgData.aPayloadData[CurIndex], gAmrInfo[nTtlIndex].MtrAmrInfo[pn-1].aMeterFilesMac, 6);  //档案mac
            CurIndex += 6; 
            InfoMsgData.aPayloadData[CurIndex] = gAmrInfo[nTtlIndex].MtrAmrInfo[pn-1].nProtocolType;  //协议类型(此处以档案为准, 还是以收到的报文为准?)
            CurIndex += 1;
            //InfoMsgData.aPayloadData[CurIndex] = gAmrInfo[nTtlIndex].MtrAmrInfo[pn-1].nCommPara;  //通信参数
            //CurIndex += 1;  
        }
        
    }
    InfoMsgData.nPayloadLength = CurIndex;
    PlcWriteQueue(pAmrSendUp, (uint8 *)&InfoMsgData, sizeof(MSG_INFO_T)+InfoMsgData.nPayloadLength);
    return;
}

/*******************************************************************************
* 函数名称: SaveReportInfo
* 函数功能: 保存上报信息
* 输入参数: ReportCnt           上报个数
*           ReportInfo          上报信息
*           RealPhase           实测相位
* 输出参数: 无
* 返 回 值: 无
*******************************************************************************/
void SaveReportInfo_F4_Collector(uint8 ReportCnt, uint8 *ReportInfo, uint8 RealPhase, uint8 nArrIndex)
{
    CCODOWN_FMT_DEBUG(nArrIndex, "SaveReportInfo_F4_Collector(ReportCnt[%d], *ReportInfo[%d], RealPhase[%d], nArrIndex[%d])...begin \n", ReportCnt, *ReportInfo, RealPhase, nArrIndex);
    
    uint16  MeterCnt = 0;                       //电表数目
    int     SlaveCnt = 0;                       //从节点数量
    uint16  CurIndex = 0;                       //当前索引
    //uint16  SearchFilesIndex = 0;               //档案序号(搜索上报序号,非原始档案序号)
    int     i,j;
    int     pn = 0;

    ACT_REPORT_INFO_T ActInfo;
    memset((uint8 *)&ActInfo, 0x00, sizeof(ACT_REPORT_INFO_T));
    AMR_METER_SEARCH_REPORT_T AmrMeterSearchReport;
    memset((uint8 *)&AmrMeterSearchReport, 0x00, sizeof(AmrMeterSearchReport));
    INFO_DATA_T InfoMsgData;
    memset((uint8 *)&InfoMsgData, 0x00, sizeof(InfoMsgData));

    uint8 nTtlIndex = 0;  //不同的端口号对应不同的数组下标
    nTtlIndex = nArrIndex;

    //填充上报信息
    for (i = 0; i < ReportCnt; i++)
    {
        //采集器地址
        memcpy(ActInfo.AcqUnitId, &ReportInfo[CurIndex], 6);   //注意：ReportInfo不包含首字节“上报从节点的数量n”，直接从地址开始
        CurIndex += 6;
        CCODOWN_FMT_DEBUG(nTtlIndex, "采集器地址: %02x %02x %02x %02x %02x %02x \n", 
                    ActInfo.AcqUnitId[5], ActInfo.AcqUnitId[4], ActInfo.AcqUnitId[3], ActInfo.AcqUnitId[2], ActInfo.AcqUnitId[1], ActInfo.AcqUnitId[0]);

        //跳过采集器规约、序号、设备类型及下接从节点总数量
        CurIndex += 5;

        //上报从节点数量
        SlaveCnt = ReportInfo[CurIndex];
        CurIndex += 1;
        CCODOWN_FMT_DEBUG(nTtlIndex, "本采集器下测量点数:%d \n", SlaveCnt);

        //非空采集器
        if (SlaveCnt > 0)
        {
            //循环检测上报的从节点
            for (j = 0; j < SlaveCnt; j++)
            {
                //从节点地址
                memcpy(ActInfo.MeterAddr, &ReportInfo[CurIndex], 6);
                CurIndex += 6;

                //规约类型
                if (ReportInfo[CurIndex] == 0x01)
                {
                    ActInfo.PrtlMode = 0x01;
                }
                else if(ReportInfo[CurIndex] == 0x02)
                {
                    ActInfo.PrtlMode = 0x02;
                }
                else if(ReportInfo[CurIndex] == 0x03)
                {
                    ActInfo.PrtlMode = 0x03;
                }
                else
                {
                    ActInfo.PrtlMode = 0x00;
                }

                CurIndex += 1;

                //有效性检测
                if(TRUE)
                {
                    //从节点地址
                    memcpy(AmrMeterSearchReport.stAmrMeterFiles[MeterCnt].aMeterFilesAddr, ActInfo.MeterAddr, 6);  //注意：ReportInfo不包含首字节“上报从节点的数量n”，直接从地址开始
                    
                    //规约类型
                    AmrMeterSearchReport.stAmrMeterFiles[MeterCnt].nProtocolType = ActInfo.PrtlMode;

                    MeterCnt ++;
                }          

                CCODOWN_FMT_DEBUG(nTtlIndex, "第 %d 个电表地址: %02x %02x %02x %02x %02x %02x, 协议:%02x \n", j, 
                    ActInfo.MeterAddr[5], ActInfo.MeterAddr[4], ActInfo.MeterAddr[3], ActInfo.MeterAddr[2], ActInfo.MeterAddr[1], ActInfo.MeterAddr[0], ActInfo.PrtlMode);
            }
        }
    }

    CCODOWN_FMT_DEBUG(nTtlIndex, "SaveReportInfo_F4_Collector() 解析之后 MeterCnt=%d \n", MeterCnt);
    if(MeterCnt <= 0)
    {
        CCODOWN_FMT_DEBUG(nTtlIndex, "SaveReportInfo_F4_Collector() if(MeterCnt <= 0) 直接返回! \n", MeterCnt);
        return;
    }

    //组装消息报文
    InfoMsgData.IP  = inet_addr(gCtsRouterAppIP[nTtlIndex]);
    InfoMsgData.IP  = ntohl(InfoMsgData.IP);
    InfoMsgData.port = gCtsRouterAppPort[nTtlIndex];
    InfoMsgData.MsgPRIORITY = 0;
    InfoMsgData.MsgRPM = 1;
    InfoMsgData.MsgIndex = GetInfoIndex();
    InfoMsgData.MsgLabel = nTtlIndex;  //mqtt存放端口序号(0或1)
    InfoMsgData.SourLen = strlen(gCcoRouterAppName);
    memset(InfoMsgData.SourAddr, 0x00, InfoMsgData.SourLen);
    memcpy(InfoMsgData.SourAddr, gCcoRouterAppName, InfoMsgData.SourLen);
    InfoMsgData.DestLen = strlen(gCtsRouterAppName[nTtlIndex]);
    memset(InfoMsgData.DestAddr, 0x00, InfoMsgData.DestLen);
    memcpy(InfoMsgData.DestAddr, gCtsRouterAppName[nTtlIndex], InfoMsgData.DestLen);
    
    CurIndex = 0;
    InfoMsgData.IID_ID = 0x0003;
    InfoMsgData.IOP_ID = 0x0036;

    Uint32ToCharArr(gPortID[nTtlIndex], &InfoMsgData.aPayloadData[CurIndex]);  //端口号
    CurIndex += 4;
    InfoMsgData.aPayloadData[CurIndex] = 0x00;  //是否含有抄读信息标志, 0x00:无, 0x01:有
    CurIndex += 1;
    if(0 == AREA_ZHEJIANG)
    {
        ChangeLenToBuff(MeterCnt, &CurIndex, &InfoMsgData.aPayloadData[CurIndex]);  //表计档案的个数
    }
    else
    {
        //浙江地区特殊需求: 搜表结果的第一个为采集器, 通信协议为特殊0xCC
        ChangeLenToBuff(1+MeterCnt, &CurIndex, &InfoMsgData.aPayloadData[CurIndex]);  //采集器个数+表计档案的个数

        //填充采集器地址
        //SearchFilesIndex = 0;
        //Uint16ToCharArr(SearchFilesIndex, &InfoMsgData.aPayloadData[CurIndex]);  //档案序号
        //CurIndex += 2;
        //CurIndex += 1;  //跳过地址长度
        memcpy_r(&InfoMsgData.aPayloadData[CurIndex], ActInfo.AcqUnitId, 6);  //档案地址
        CurIndex += 6;
        //CurIndex += 1;  //跳过mac长度
        memcpy_r(&InfoMsgData.aPayloadData[CurIndex], ActInfo.AcqUnitId, 6);  //档案mac
        CurIndex += 6; 
        InfoMsgData.aPayloadData[CurIndex] = 0xCC;  //协议类型(临时：采集器协议类型为0xCC)
        CurIndex += 1;
        //InfoMsgData.aPayloadData[CurIndex] = 0;  //通信参数
        //CurIndex += 1;  
    }

    //填充电表地址
    for (i = 0; i < MeterCnt; i++)
    {
        pn = GetPnByMeterAddr(AmrMeterSearchReport.stAmrMeterFiles[i].aMeterFilesAddr, nTtlIndex);
        if(pn <= 0) //本地没有该电表档案，以收到的报文为准
        {
            //SearchFilesIndex = gSearchMeterFilesIndex[nTtlIndex] ++;
            //Uint16ToCharArr(SearchFilesIndex, &InfoMsgData.aPayloadData[CurIndex]);  //档案序号
            //CurIndex += 2;
            //CurIndex += 1;  //跳过地址长度
            memcpy_r(&InfoMsgData.aPayloadData[CurIndex], AmrMeterSearchReport.stAmrMeterFiles[i].aMeterFilesAddr, 6);  //档案地址
            CurIndex += 6;
            //CurIndex += 1;  //跳过mac长度
            memcpy_r(&InfoMsgData.aPayloadData[CurIndex], AmrMeterSearchReport.stAmrMeterFiles[i].aMeterFilesAddr, 6);  //档案mac
            CurIndex += 6; 
            InfoMsgData.aPayloadData[CurIndex] = AmrMeterSearchReport.stAmrMeterFiles[i].nProtocolType;  //协议类型(此处以档案为准, 还是以收到的报文为准?)
            CurIndex += 1;
            //InfoMsgData.aPayloadData[CurIndex] = 0;  //通信参数
            //CurIndex += 1; 
        }
        else
        {
            //SearchFilesIndex = gSearchMeterFilesIndex[nTtlIndex] ++;
            //Uint16ToCharArr(SearchFilesIndex, &InfoMsgData.aPayloadData[CurIndex]);  //档案序号
            //CurIndex += 2;
            //CurIndex += 1;  //跳过地址长度
            memcpy_r(&InfoMsgData.aPayloadData[CurIndex], gAmrInfo[nTtlIndex].MtrAmrInfo[pn-1].aMeterFilesAddr, 6);  //档案地址
            CurIndex += 6;
            //CurIndex += 1;  //跳过mac长度
            memcpy_r(&InfoMsgData.aPayloadData[CurIndex], gAmrInfo[nTtlIndex].MtrAmrInfo[pn-1].aMeterFilesMac, 6);  //档案mac
            CurIndex += 6; 
            InfoMsgData.aPayloadData[CurIndex] = gAmrInfo[nTtlIndex].MtrAmrInfo[pn-1].nProtocolType;  //协议类型(此处以档案为准, 还是以收到的报文为准?)
            CurIndex += 1;
            //InfoMsgData.aPayloadData[CurIndex] = gAmrInfo[nTtlIndex].MtrAmrInfo[pn-1].nCommPara;  //通信参数
            //CurIndex += 1;  
        }
        
    }
    InfoMsgData.nPayloadLength = CurIndex;
    PlcWriteQueue(pAmrSendUp, (uint8 *)&InfoMsgData, sizeof(MSG_INFO_T)+InfoMsgData.nPayloadLength);
    return;
    
}

/*******************************************************************************
* 函数名称: SaveReportInfo
* 函数功能: 保存上报信息
* 输入参数: ReportCnt           上报个数
*           ReportInfo          上报信息
*           RealPhase           实测相位
* 输出参数: 无
* 返 回 值: 无
*******************************************************************************/
void SaveReportInfo_F4_Meter(uint8 ReportCnt, uint8 *ReportInfo, uint8 RealPhase, uint8 nArrIndex)
{
    CCODOWN_FMT_DEBUG(nArrIndex, "SaveReportInfo_F4_Meter(ReportCnt[%d], *ReportInfo[%d], RealPhase[%d], nArrIndex[%d])...begin \n", ReportCnt, *ReportInfo, RealPhase, nArrIndex);
    
    uint16  MeterCnt = 0;                       //电表数目
    int     SlaveCnt = 0;                       //从节点数量
    uint16  CurIndex = 0;                       //当前索引
    //uint16  SearchFilesIndex = 0;               //档案序号(搜索上报序号,非原始档案序号)
    int     i,j;
    int     pn = 0;

    ACT_REPORT_INFO_T ActInfo;
    memset((uint8 *)&ActInfo, 0x00, sizeof(ACT_REPORT_INFO_T));
    AMR_METER_SEARCH_REPORT_T AmrMeterSearchReport;
    memset((uint8 *)&AmrMeterSearchReport, 0x00, sizeof(AmrMeterSearchReport));
    INFO_DATA_T InfoMsgData;
    memset((uint8 *)&InfoMsgData, 0x00, sizeof(InfoMsgData));

    uint8 nTtlIndex = 0;  //不同的端口号对应不同的数组下标
    nTtlIndex = nArrIndex;

    //填充上报信息
    for (i = 0; i < ReportCnt; i++)
    {
        //载波表地址
        memcpy(ActInfo.MeterAddr, &ReportInfo[CurIndex], 6);  //注意：ReportInfo不包含首字节“上报从节点的数量n”，直接从地址开始
        CurIndex += 6;

        //载波表通信协议
        ActInfo.PrtlMode = ReportInfo[CurIndex];
        CurIndex += 1;
        
        //跳过序号、设备类型及下接从节点总数量
        CurIndex += 4;

        //上报从节点数量
        SlaveCnt = ReportInfo[CurIndex];
        CurIndex += 1;
        CCODOWN_FMT_DEBUG(nTtlIndex, "此设备为载波表 \n");
    
       //空载波表
        if (SlaveCnt == 0)
        {
            //载波表地址
            memcpy(AmrMeterSearchReport.stAmrMeterFiles[MeterCnt].aMeterFilesAddr, ActInfo.MeterAddr, 6);  //注意：ReportInfo不包含首字节“上报从节点的数量n”，直接从地址开始
            
            //载波表通信协议
            AmrMeterSearchReport.stAmrMeterFiles[MeterCnt].nProtocolType = ActInfo.PrtlMode;

            MeterCnt ++;
        }
        //非空载波表
        else
        {
            CCODOWN_FMT_DEBUG(nTtlIndex, "上报信息错误 \n");
            for (j = 0; j < SlaveCnt; j++)
            {
                CurIndex += 7;
            }
        }

        CCODOWN_FMT_DEBUG(nTtlIndex, "第 %d 个电表地址: %02x %02x %02x %02x %02x %02x, 协议:%02x \n", i, 
            ActInfo.MeterAddr[5], ActInfo.MeterAddr[4], ActInfo.MeterAddr[3], ActInfo.MeterAddr[2], ActInfo.MeterAddr[1], ActInfo.MeterAddr[0], ActInfo.PrtlMode);
    }

    CCODOWN_FMT_DEBUG(nTtlIndex, "SaveReportInfo_F4_Meter() 解析之后 MeterCnt=%d \n", MeterCnt);
    if(MeterCnt <= 0)
    {
        CCODOWN_FMT_DEBUG(nTtlIndex, "SaveReportInfo_F4_Meter() if(MeterCnt <= 0) 直接返回! \n", MeterCnt);
        return;
    }

    //组装消息报文
    InfoMsgData.IP  = inet_addr(gCtsRouterAppIP[nTtlIndex]);
    InfoMsgData.IP  = ntohl(InfoMsgData.IP);
    InfoMsgData.port = gCtsRouterAppPort[nTtlIndex];
    InfoMsgData.MsgPRIORITY = 0;
    InfoMsgData.MsgRPM = 1;
    InfoMsgData.MsgIndex = GetInfoIndex();
    InfoMsgData.MsgLabel = nTtlIndex;  //mqtt存放端口序号(0或1)
    InfoMsgData.SourLen = strlen(gCcoRouterAppName);
    memset(InfoMsgData.SourAddr, 0x00, InfoMsgData.SourLen);
    memcpy(InfoMsgData.SourAddr, gCcoRouterAppName, InfoMsgData.SourLen);
    InfoMsgData.DestLen = strlen(gCtsRouterAppName[nTtlIndex]);
    memset(InfoMsgData.DestAddr, 0x00, InfoMsgData.DestLen);
    memcpy(InfoMsgData.DestAddr, gCtsRouterAppName[nTtlIndex], InfoMsgData.DestLen);
    
    CurIndex = 0;
    InfoMsgData.IID_ID = 0x0003;
    InfoMsgData.IOP_ID = 0x0036;

    Uint32ToCharArr(gPortID[nTtlIndex], &InfoMsgData.aPayloadData[CurIndex]);  //端口号
    CurIndex += 4;
    InfoMsgData.aPayloadData[CurIndex] = 0x00;  //是否含有抄读信息标志, 0x00:无, 0x01:有
    CurIndex += 1;
    ChangeLenToBuff(MeterCnt, &CurIndex, &InfoMsgData.aPayloadData[CurIndex]);  //表计档案的个数
    for (i = 0; i < MeterCnt; i++)
    {
        pn = GetPnByMeterAddr(AmrMeterSearchReport.stAmrMeterFiles[i].aMeterFilesAddr, nTtlIndex);
        if(pn <= 0) //本地没有该电表档案，以收到的报文为准
        {
            //SearchFilesIndex = gSearchMeterFilesIndex[nTtlIndex] ++;
            //Uint16ToCharArr(SearchFilesIndex, &InfoMsgData.aPayloadData[CurIndex]);  //档案序号
            //CurIndex += 2;
            //CurIndex += 1;  //跳过地址长度
            memcpy_r(&InfoMsgData.aPayloadData[CurIndex], AmrMeterSearchReport.stAmrMeterFiles[i].aMeterFilesAddr, 6);  //档案地址
            CurIndex += 6;
            //CurIndex += 1;  //跳过mac长度
            memcpy_r(&InfoMsgData.aPayloadData[CurIndex], AmrMeterSearchReport.stAmrMeterFiles[i].aMeterFilesAddr, 6);  //档案mac
            CurIndex += 6; 
            InfoMsgData.aPayloadData[CurIndex] = AmrMeterSearchReport.stAmrMeterFiles[i].nProtocolType;  //协议类型(此处以档案为准, 还是以收到的报文为准?)
            CurIndex += 1;
            //InfoMsgData.aPayloadData[CurIndex] = 0;  //通信参数
            //CurIndex += 1; 
        }
        else
        {
            //SearchFilesIndex = gSearchMeterFilesIndex[nTtlIndex] ++;
            //Uint16ToCharArr(SearchFilesIndex, &InfoMsgData.aPayloadData[CurIndex]);  //档案序号
            //CurIndex += 2;
            //CurIndex += 1;  //跳过地址长度
            memcpy_r(&InfoMsgData.aPayloadData[CurIndex], gAmrInfo[nTtlIndex].MtrAmrInfo[pn-1].aMeterFilesAddr, 6);  //档案地址
            CurIndex += 6;
            //CurIndex += 1;  //跳过mac长度
            memcpy_r(&InfoMsgData.aPayloadData[CurIndex], gAmrInfo[nTtlIndex].MtrAmrInfo[pn-1].aMeterFilesMac, 6);  //档案mac
            CurIndex += 6; 
            InfoMsgData.aPayloadData[CurIndex] = gAmrInfo[nTtlIndex].MtrAmrInfo[pn-1].nProtocolType;  //协议类型(此处以档案为准, 还是以收到的报文为准?)
            CurIndex += 1;
            //InfoMsgData.aPayloadData[CurIndex] = gAmrInfo[nTtlIndex].MtrAmrInfo[pn-1].nCommPara;  //通信参数
            //CurIndex += 1;  
        }
        
    }
    InfoMsgData.nPayloadLength = CurIndex;
    PlcWriteQueue(pAmrSendUp, (uint8 *)&InfoMsgData, sizeof(MSG_INFO_T)+InfoMsgData.nPayloadLength);
    return;
    
}


/*******************************************************************************
* 函数名称: SaveReportInfo
* 函数功能: 保存上报信息
* 输入参数: ReportCnt           上报个数
*           ReportInfo          上报信息
*           RealPhase           实测相位
* 输出参数: 无
* 返 回 值: 无
*******************************************************************************/
void SaveReportInfo_F4(uint8 ReportCnt, uint8 *ReportInfo, uint8 RealPhase, uint8 nArrIndex)
{
	int 	CurIndex = 0;						//当前索引
	
	if(ReportInfo[CurIndex + 9] == DEV_COLLECTOR)  //采集器
	{
		SaveReportInfo_F4_Collector(ReportCnt, ReportInfo, RealPhase, nArrIndex);
	}
	else if(ReportInfo[CurIndex + 9] == DEV_METER)
	{
		SaveReportInfo_F4_Meter(ReportCnt, ReportInfo, RealPhase, nArrIndex);
	}
	
}


/*******************************************************************************
* 函数名称: AddrSearchDeal
* 函数功能: 表号搜索处理
* 输入参数: 无
* 输出参数: 无
* 返 回 值: >0                  成功
*           <0                  失败
*******************************************************************************/
int AddrSearchDeal(TASK_FUN_INPUT stTaskFunInput)
{
	static int      StudyDelayCnt[MAX_DOWN_THREAD_NUM] = {0};        //学习延时计数
	static uint16	StudyTime[MAX_DOWN_THREAD_NUM] = {0}; 			 //学习时间
	uint8			ReportInfo[500] = {0};			    //上报信息
	int 			ReportCnt;							//上报个数
	uint8			RealPhase;							//实测相位
	int 			ret;
	DateTime_T      stDateTime;
	static int      AccumulateWaitTime[MAX_DOWN_THREAD_NUM] = {0};             //累计等待时间
	//time_t CurTimeSec;

	AppData_T		SendAppData,RecvAppData;
	uint8			AddrSearchReportType = 0;
	
	uint8 nTtlIndex = 0;  //不同的端口号对应不同的数组下标
	nTtlIndex = stTaskFunInput.TaskArrIndex;
	
	//printx("AddrSearchDeal(TASK_FUN_INPUT stTaskFunInput)表号搜索处理,端口[%d]...begin \n", nTtlIndex);
	
	AMR_METER_START_SEARCH_T stAmrMeterStartSearch;
	memcpy(&stAmrMeterStartSearch, stTaskFunInput.InfoMsgBuff, sizeof(stAmrMeterStartSearch));

	switch (gTotalState[nTtlIndex].AddrSearch)
	{
	case AS_IDLE:								//空闲
		CCODOWN_FMT_DEBUG(nTtlIndex, "AddrSearchDeal(TASK_FUN_INPUT stTaskFunInput)表号搜索处理,端口[%d]...AS_IDLE \n", nTtlIndex);
		//读取注册持续时间
		StudyTime[nTtlIndex] = 5;  //测试用时，设为5min
		StudyTime[nTtlIndex] = stAmrMeterStartSearch.nContinuedTime;  //实际用时
		if(StudyTime[nTtlIndex] == 0)
		{
			StudyTime[nTtlIndex] = 180;
			CCODOWN_FMT_DEBUG(nTtlIndex, "容错 if(StudyTime[nTtlIndex] == 0) StudyTime[nTtlIndex] = 180; \n");
		}
		else if(StudyTime[nTtlIndex] > 1440)
		{
			StudyTime[nTtlIndex] = 180;
			CCODOWN_FMT_DEBUG(nTtlIndex, "容错 if(StudyTime[nTtlIndex] > 1440) StudyTime[nTtlIndex] = 180; \n");
		}
		gTotalState[nTtlIndex].AddrSearch = AS_ACT_REG;
		CCODOWN_FMT_DEBUG(nTtlIndex, "AddrSearchDeal()启动搜表 表号注册持续时间:%d \n", StudyTime[nTtlIndex]);
		ReadAmrInfo(nTtlIndex);  //加载档案信息至抄表信息
		gSearchMeterFilesIndex[nTtlIndex] = 0;  //搜表档案序号初始化为0
		//更新搜表状态
		gAmrStateQuery[nTtlIndex].nModuleSearchState = 1;
		break;

	case AS_ACT_REG:							//激活注册
		CCODOWN_FMT_DEBUG(nTtlIndex, "AddrSearchDeal(TASK_FUN_INPUT stTaskFunInput)表号搜索处理,端口[%d]...激活注册AS_ACT_REG \n", nTtlIndex);
		SendAppData.AFN = 0x11;
		SendAppData.Fn = 5;
		SendAppData.DataUnitLen = 10;

		/*
		CurTimeSec = BcdTimeToSec((uint8*)&gCurBcdTime);
		printx("CurTimeSec:%d \n", (int)CurTimeSec);
		memcpy(SendAppData.DataUnitBuff, (char*)&gCurBcdTime, 6);  //启动时间临时设为当前时间
		memcpy(&SendAppData.DataUnitBuff[6],&StudyTime,2);
		SendAppData.DataUnitBuff[8] = 1;
		SendAppData.DataUnitBuff[9] = 1;
		*/
				
		CCODOWN_FMT_DEBUG(nTtlIndex, "stAmrMeterStartSearch.nStartTime:%d \n", stAmrMeterStartSearch.nStartTime);
		stDateTime = SecToDateTime_T((time_t)stAmrMeterStartSearch.nStartTime);
		CCODOWN_BUF_DEBUG(nTtlIndex, (uint8*)&stDateTime, 6, "时间");
		stDateTime = DateTimeToBCDDateTime_T(stDateTime);
		CCODOWN_BUF_DEBUG(nTtlIndex, (uint8*)&stDateTime, 6, "时间");
		
		memcpy(SendAppData.DataUnitBuff, (char*)&stDateTime, 6);
		memcpy(&SendAppData.DataUnitBuff[6],&StudyTime[nTtlIndex],2);
		SendAppData.DataUnitBuff[8] = stAmrMeterStartSearch.nSlaveNodeReSendCount;
		SendAppData.DataUnitBuff[9] = stAmrMeterStartSearch.nRandWaitTimeSlice;	
		
		ret = CommFunDealByType(SendAppData, &RecvAppData, nTtlIndex, stTaskFunInput.TaskName);
		if (ret > 0)							//完成，切换到下一状态
		{
			gTotalState[nTtlIndex].AddrSearch = AS_WAIT_REPORT;
			StudyDelayCnt[nTtlIndex] = 0;
			AccumulateWaitTime[nTtlIndex] = 0;
		}
		break;

	case AS_WAIT_REPORT:						//等待上报
		//printx("AddrSearchDeal(TASK_FUN_INPUT stTaskFunInput)表号搜索处理,端口[%d]...AS_WAIT_REPORT \n", nTtlIndex);
		//读取上报信息处理
		ReportCnt = ActReportNodeDeal(ReportInfo, &RealPhase, &AddrSearchReportType, nTtlIndex);
		if (ReportCnt > 0)
		{
			CCODOWN_FMT_DEBUG(nTtlIndex, "AddrSearchDeal()表号搜索处理 保存上报信息, 测量点个数:%d \n", ReportCnt);
			if(AddrSearchReportType == 1)
			{
				SaveReportInfo_F1(ReportCnt, ReportInfo, RealPhase, nTtlIndex);
			}
			else if(AddrSearchReportType == 4)
			{
				SaveReportInfo_F4(ReportCnt, ReportInfo, RealPhase, nTtlIndex);
			}
			else
			{
				CCODOWN_FMT_DEBUG(nTtlIndex, "AddrSearchDeal()表号搜索处理 AddrSearchReportType既非1, 也非4... \n");
			}

		}
		else if (ReportCnt == -2)
		{
			CCODOWN_FMT_DEBUG(nTtlIndex, "AddrSearchDeal()表号搜索处理 表号注册上报时间结束! \n");
			gTotalState[nTtlIndex].AddrSearch = AS_SEARCH_END;
		}
		else
		{
			//printx("既非if (ReportCnt > 0), 也非ReportCnt == -2, 其他情况...\n", ReportCnt);
			if (gPastTimeFlag[nTtlIndex].BitSect.Minute == 1)
            {
                AccumulateWaitTime[nTtlIndex]++;
                StudyDelayCnt[nTtlIndex]++;

                CCODOWN_FMT_DEBUG(nTtlIndex, "AddrSearchDeal()表号搜索处理 学习时间累计:%d, 累计等待时间:%d. \n", StudyDelayCnt[nTtlIndex], AccumulateWaitTime[nTtlIndex]);
                if (StudyDelayCnt[nTtlIndex] > StudyTime[nTtlIndex] && (StudyTime[nTtlIndex] != 0))
                {
                    CCODOWN_FMT_DEBUG(nTtlIndex, "AddrSearchDeal()表号搜索处理 表号注册上报时间结束! \n");
                    gTotalState[nTtlIndex].AddrSearch = AS_SEARCH_END;
                }
                else
                {
                    //空闲时间超过10分钟,查询一次
                    if(AccumulateWaitTime[nTtlIndex] > 10)
                    {
                        gTotalState[nTtlIndex].AddrSearch = AS_GET_RUN_STATE;
                        CCODOWN_FMT_DEBUG(nTtlIndex, "AddrSearchDeal()表号搜索处理 累计等待时间:%d \n", AccumulateWaitTime[nTtlIndex]);

                        AccumulateWaitTime[nTtlIndex] = 0;
                    }

                }
            }
		}
		break;

	case AS_GET_RUN_STATE:						//读取路由运行状态
		CCODOWN_FMT_DEBUG(nTtlIndex, "AddrSearchDeal(TASK_FUN_INPUT stTaskFunInput)表号搜索处理,端口[%d]...读取路由运行状态AS_GET_RUN_STATE \n", nTtlIndex);
		SendAppData.AFN = 0x10;
		SendAppData.Fn = 4;
		SendAppData.DataUnitLen = 0;
		ret = CommFunDealByType(SendAppData, &RecvAppData, nTtlIndex, stTaskFunInput.TaskName);
		if (ret > 0)
		{
			if (RecvAppData.DataUnitBuff[0] & 0x02) 					//上报完成
			{
				CCODOWN_FMT_DEBUG(nTtlIndex, "AddrSearchDeal()表号搜索处理 继续等待注册! \n");
				gTotalState[nTtlIndex].AddrSearch = AS_WAIT_REPORT;
			}
			else
			{
				CCODOWN_FMT_DEBUG(nTtlIndex, "AddrSearchDeal()表号搜索处理 注册完成! \n");
				gTotalState[nTtlIndex].AddrSearch = AS_SEARCH_END;
			}
		}
		break;

	case AS_SEARCH_END: 						//搜索结束
		CCODOWN_FMT_DEBUG(nTtlIndex, "AddrSearchDeal(TASK_FUN_INPUT stTaskFunInput)表号搜索处理,端口[%d]...搜索结束AS_SEARCH_END \n", nTtlIndex);
		//清除中断状态
		gTotalState[nTtlIndex].BreakState.AddrSearch = 0;
		gTotalState[nTtlIndex].AddrSearch = AS_IDLE;
		//更新搜表状态
		gAmrStateQuery[nTtlIndex].nModuleSearchState = 0;
		return 1;

	default:
		gTotalState[nTtlIndex].AddrSearch = AS_IDLE;
		break;
	}

	return -1;
}

/*******************************************************************************
* 函数名称: DelSlaveInfoDeal
* 函数功能: 删除从节点信息处理流程
* 输入参数: 
* 输出参数: 
* 返 回 值: 
*******************************************************************************/
int DelSlaveInfoDeal(TASK_FUN_INPUT stTaskFunInput)
{
	return 1;

}

/*******************************************************************************
* 函数名称:	CmdDataRelay
* 函数功能:	确认帧应答命令
* 输入参数:	
* 输出参数:	
* 返 回 值: 
*******************************************************************************/
int CmdDataRelay(MeterPrtlFmt_T MeterData, uint8 *buf, uint32 buf_len, uint8 nArrIndex)
{
    uint8 tmpMeter[6] = {0};
    memset(tmpMeter,0x99,6);

    if(memcmp(MeterData.MtrAddr,tmpMeter,6) == 0)
    {
        Comm_DataRelay(MeterData, 2, buf, buf_len, nArrIndex);
    }
    else
    {
        Comm_DataRelay(MeterData, 1, buf, buf_len, nArrIndex);
    }
    return 1;
}



/*******************************************************************************
* 函数名称: RelayCmdMeterPrtlData
* 函数功能: 电表数据召测组帧处理
* 输入参数: 
* 输出参数:
* 返 回 值: 
*******************************************************************************/
int RelayCmdMeterPrtlData(MeterPrtlFmt_T MeterPrtlData, uint8 *buf, uint32 buf_len, uint8 nArrIndex)
{   		
    uint16 Pn;	
	uint8 nTtlIndex = 0;  //不同的端口号对应不同的数组下标
	nTtlIndex = nArrIndex;

	//电表基本信息
	Pn = GetPnByMeterAddr(MeterPrtlData.MtrAddr, nTtlIndex);
	MeterPrtlData.DelayFlag = 0;//延时标志
    ReadMtrPrtlMode(&MeterPrtlData.PrtlMode, Pn, nTtlIndex); //规约类型,1:97,2:07, 3:oop
    ReadAcqUnitAddr(MeterPrtlData.AcqUnitAddr, Pn, nTtlIndex); //采集器地址
    MeterPrtlData.MtrType = ReadMtrType(Pn, nTtlIndex);  //电表类型

	//组帧下发
    CmdDataRelay(MeterPrtlData, buf, buf_len, nTtlIndex);

    return 1;
}


/*******************************************************************************
* 函数名称: RelayTransDeal
* 函数功能: 透明转发
* 输入参数: 
* 输出参数: 
* 返 回 值: 
*******************************************************************************/
int RelayTransDeal(TASK_FUN_INPUT stTaskFunInput)
{
	int 			Ret = -1;
	static time_t	CommBeginTime[MAX_DOWN_THREAD_NUM];						//通讯开始时间
	int 			RecvLen;							//接收数据长度
	uint8			RecvBuf[MAX_QUEUE_BUFF_LEN];		//接收数据缓冲区
	AppData_T		SendAppData;							//应用层数据
	AppData_T		AppData;							//应用层数据
	int 			RecvResult = 1; 				    //接收结果(1:成功 / -1:失败)
	static uint32	OverTimeCnt[MAX_DOWN_THREAD_NUM] = {0};					//超时时间
	static	uint16	MaxRetrtCnt[MAX_DOWN_THREAD_NUM] = {0};					//最大重试次数
	static	int 	CommRetryCnt[MAX_DOWN_THREAD_NUM] = {0};					//通讯重试次数
	static MeterPrtlFmt_T	SendMeterPrtlData[MAX_DOWN_THREAD_NUM];		//电表规约格式数据（从消息中获取，解析下发）
	MeterPrtlFmt_T	RecvMeterPrtlData;		//电表规约格式数据（下发之后收到的回复）
	uint16          offset;
	static BOOL     bProtocol3762[MAX_DOWN_THREAD_NUM] = {0};
	static uint8    nFrameIndex3762[MAX_DOWN_THREAD_NUM] = {0};
	uint8           nZeroUint8 = 0;
	
	uint8           nTtlIndex = 0;  //不同的端口号对应不同的数组下标
	nTtlIndex = stTaskFunInput.TaskArrIndex;

	static AMR_DATA_TRANSMIT_T AmrDataTransmit[MAX_DOWN_THREAD_NUM];  //数据透传
	static INFO_DATA_T InfoMsgData[MAX_DOWN_THREAD_NUM];  //回复消息
	static INFO_DATA_HEAD_T InfoDataHead[MAX_DOWN_THREAD_NUM];  //消息原始HEAD


	switch (gTotalState[nTtlIndex].RelayTrans)
	{
	case INNER_RTS_IDLE://空闲执行,初始化
		CCODOWN_FMT_DEBUG(nTtlIndex, "RelayTransDeal()穿透任务 INNER_RTS_IDLE...端口[%d] \n", nTtlIndex);
		ClearCommRecvBuff(nTtlIndex);  //清通讯接收缓冲区
		ReadAmrInfo(nTtlIndex);  //加载档案信息至抄表信息
		CommBeginTime[nTtlIndex] = time(NULL);
		OverTimeCnt[nTtlIndex] = 60;
		MaxRetrtCnt[nTtlIndex] = 0;
		memset(&SendMeterPrtlData[nTtlIndex], 0, sizeof(MeterPrtlFmt_T));
		memset(&AmrDataTransmit[nTtlIndex], 0, sizeof(AMR_DATA_TRANSMIT_T));
		memset(&InfoMsgData[nTtlIndex], 0, sizeof(INFO_DATA_T));
		memset(&InfoDataHead[nTtlIndex], 0, sizeof(INFO_DATA_HEAD_T));

		//从队列中读取透传报文
		Ret = GetDataTransmitFromList(nTtlIndex, &AmrDataTransmit[nTtlIndex], &InfoDataHead[nTtlIndex]);
		if(Ret > 0)
		{
			memcpy(&InfoMsgData[nTtlIndex], &InfoDataHead[nTtlIndex], sizeof(INFO_DATA_HEAD_T));  //获取原始HEAD
			InfoMsgData[nTtlIndex].MsgRPM = 0;   //启动标志位, 回复，非启动
		}
		else
		{
			CCODOWN_FMT_DEBUG(nTtlIndex, "RelayTransDeal() 透明转发未查到报文, 直接结束! 端口:%d \n", nTtlIndex);
			gTotalState[nTtlIndex].RelayTrans = INNER_RTS_END;//任务完成
			break;
		}

		//模块上报的从节点超时时间≥8秒时，减2秒作为修正，抵消流程处理时间(按理应该从收到消息开始计时)。
		if(AmrDataTransmit[nTtlIndex].nFrameTimeover >= 8)
		{
			OverTimeCnt[nTtlIndex] = AmrDataTransmit[nTtlIndex].nFrameTimeover - 3;
		}
		else
		{
			OverTimeCnt[nTtlIndex] = AmrDataTransmit[nTtlIndex].nFrameTimeover;
		}
		
		//模块上报的从节点超时时间小于10s时，修正为60s。
        //OverTimeCnt[nTtlIndex] = (OverTimeCnt[nTtlIndex] < 10) ? 60 : OverTimeCnt[nTtlIndex];
        CCODOWN_FMT_DEBUG(nTtlIndex, "从节点最大超时时间 修正OverTimeCnt[%d]=%d, AmrDataTransmit[%d].nFrameTimeover=%d \n", 
        	nTtlIndex, OverTimeCnt[nTtlIndex], nTtlIndex, AmrDataTransmit[nTtlIndex].nFrameTimeover);		
		
		//判断透传报文是否376.2报文
		CCODOWN_FMT_DEBUG(nTtlIndex, "判断透传报文是否376.2报文 \n");
		Ret = UnPackFrameData(AmrDataTransmit[nTtlIndex].nTransmitDataLen, AmrDataTransmit[nTtlIndex].pTransmitDataBuf, &SendAppData);
		if(Ret != CHECK_RESULT_FAILUER)
		{
			//376.2报文直接放入下行发送队列, 将数据发送到共享缓冲区
			CCODOWN_FMT_DEBUG(nTtlIndex, "RelayTransDeal()透传规约解帧 报文协议为376.2...\n");
			bProtocol3762[nTtlIndex] = TRUE;
			nFrameIndex3762[nTtlIndex] = SendAppData.RInfo[5];			
			gTotalState[nTtlIndex].RelayTrans = INNER_RTS_SEND;  //直接跳到接收
		}
		else
		{
			//依次按照645报文和698报文解析
			CCODOWN_FMT_DEBUG(nTtlIndex, "依次按照645报文和698报文解析 \n");
			bProtocol3762[nTtlIndex] = FALSE;
			nFrameIndex3762[nTtlIndex] = g_InerSeq[nTtlIndex];
			Ret = CheckMeterProtocol(AmrDataTransmit[nTtlIndex].nTransmitDataLen, AmrDataTransmit[nTtlIndex].pTransmitDataBuf, &SendMeterPrtlData[nTtlIndex], &offset);
			if(Ret > 0)
			{
				if(Ret == METER_PROTOCOL_645)
				{
					CCODOWN_FMT_DEBUG(nTtlIndex, "CheckMeterProtocol()电表规约解帧 报文协议为645...\n");
				}
				else if(Ret == METER_PROTOCOL_69845)
				{
					CCODOWN_FMT_DEBUG(nTtlIndex, "CheckMeterProtocol()电表规约解帧 报文协议为698.45...\n");
				}
				gTotalState[nTtlIndex].RelayTrans = INNER_RTS_SEND;  //进入组帧处理
			}
			else
			{
				CCODOWN_FMT_DEBUG(nTtlIndex, "CheckMeterProtocol()电表规约解帧 报文格式解析错误...\n");
				gTotalState[nTtlIndex].RelayTrans = INNER_RTS_END;  //报文解帧失败, 直接结束
				
				//组装消息报文
				PackInfoMsgStruct(0x0003, 0x0033, 0, (uint8*)&nZeroUint8, &InfoMsgData[nTtlIndex]);
				PlcWriteQueue(pAmrSendUp, (uint8 *)&InfoMsgData[nTtlIndex], sizeof(MSG_INFO_T)+InfoMsgData[nTtlIndex].nPayloadLength);
			}
			
		}		
        
		break;

	case INNER_RTS_SEND: //发送
		CCODOWN_FMT_DEBUG(nTtlIndex, "RelayTransDeal()穿透任务 INNER_RTS_SEND...端口[%d] \n", nTtlIndex);
		if(bProtocol3762[nTtlIndex])  //原始透传报文是376.2报文
		{
			PlcWriteQueue(pAmrSendDown[nTtlIndex], AmrDataTransmit[nTtlIndex].pTransmitDataBuf, AmrDataTransmit[nTtlIndex].nTransmitDataLen);
			CommBeginTime[nTtlIndex] = time(NULL);
			gTotalState[nTtlIndex].RelayTrans = INNER_RTS_RECV;
		}
		else
		{
			Ret = RelayCmdMeterPrtlData(SendMeterPrtlData[nTtlIndex], AmrDataTransmit[nTtlIndex].pTransmitDataBuf, AmrDataTransmit[nTtlIndex].nTransmitDataLen, nTtlIndex);
			if(Ret > 0)
			{
				CommBeginTime[nTtlIndex] = time(NULL);
				gTotalState[nTtlIndex].RelayTrans = INNER_RTS_RECV;
			}
			else
			{
				gTotalState[nTtlIndex].RelayTrans = INNER_RTS_RECV;
			}
		}
		
		break;

	case INNER_RTS_RECV: //接收
		RecvResult = 1;  //接收结果(1:成功 / -1:失败)
		RecvLen = PlcReadQueue(pAmrRecvDown[nTtlIndex], RecvBuf);
		if (RecvLen > 0)
		{
			CCODOWN_FMT_DEBUG(nTtlIndex, "RelayTransDeal()穿透任务 INNER_RTS_RECV...if (RecvLen > 0)...端口[%d] \n", nTtlIndex);
			Ret = UnPackFrameData(RecvLen, RecvBuf, &AppData);

			switch(Ret)
			{
			case CHECK_RESULT_FAILUER:
			case CHECK_RESULT_CONFIRM:
			case CHECK_RESULT_DENY:
			case CHECK_RESULT_SUCCEED:
				CCODOWN_FMT_DEBUG(nTtlIndex, "RelayTransDeal()穿透任务 INNER_RTS_RECV...check结果=%d (FAILUER:-1, CONFIRM:1, DENY:2, SUCCEED:0) \n", Ret);
				if(bProtocol3762[nTtlIndex])  //原始透传报文是376.2报文
				{
					if(AppData.RInfo[5] == nFrameIndex3762[nTtlIndex])
					{
						gTotalState[nTtlIndex].RelayTrans = INNER_RTS_END;//任务完成

						//组装消息报文
						PackInfoMsgStruct(0x0003, 0x0033, RecvLen, RecvBuf, &InfoMsgData[nTtlIndex]);
						PlcWriteQueue(pAmrSendUp, (uint8 *)&InfoMsgData[nTtlIndex], sizeof(MSG_INFO_T)+InfoMsgData[nTtlIndex].nPayloadLength);
					}
					else
					{
						CCODOWN_FMT_DEBUG(nTtlIndex, "RelayTransDeal() INNER_RTS_RECV 376.2收发帧序号不匹配...\n");
					}
				}
				else  //原始透传报文非376.2报文
				{
					if(((AppData.AFN == 0x13) || (AppData.AFN == 0x02)) && (AppData.Fn == 1))
					{
						if(AppData.AFN == 0x13)
						{
							if (AppData.DataUnitBuff[3] <= 1)
							{
								RecvResult = -1;
								break;
							}
							Ret = CheckMeterProtocol(AppData.DataUnitBuff[3], &AppData.DataUnitBuff[4], &RecvMeterPrtlData, &offset);
						}
						else
						{
							if (AppData.DataUnitBuff[1] <= 1)
							{
								RecvResult = -1;
								break;
							}
							Ret = CheckMeterProtocol(AppData.DataUnitBuff[1], &AppData.DataUnitBuff[2], &RecvMeterPrtlData, &offset);
						}

						if(Ret > 0)
	                    {
	                    	if(Ret == METER_PROTOCOL_645)
							{
								CCODOWN_FMT_DEBUG(nTtlIndex, "CheckMeterProtocol()电表规约解帧 报文协议为645...\n");
							}
							else if(Ret == METER_PROTOCOL_69845)
							{
								CCODOWN_FMT_DEBUG(nTtlIndex, "CheckMeterProtocol()电表规约解帧 报文协议为698.45...\n");
							}
							
	                        Ret = memcmp(SendMeterPrtlData[nTtlIndex].MtrAddr, RecvMeterPrtlData.MtrAddr, 6);
	                        //if(Ret == 0)
	                        //{
	                            gTotalState[nTtlIndex].RelayTrans = INNER_RTS_END;//任务完成

								//组装消息报文
								if(AppData.AFN == 0x13)
								{
									PackInfoMsgStruct(0x0003, 0x0033, AppData.DataUnitBuff[3], (uint8*)&AppData.DataUnitBuff[4], &InfoMsgData[nTtlIndex]);
								}
								else
								{
									PackInfoMsgStruct(0x0003, 0x0033, AppData.DataUnitBuff[1], (uint8*)&AppData.DataUnitBuff[2], &InfoMsgData[nTtlIndex]);
								}
								PlcWriteQueue(pAmrSendUp, (uint8 *)&InfoMsgData[nTtlIndex], sizeof(MSG_INFO_T)+InfoMsgData[nTtlIndex].nPayloadLength);
	                        //}
	                        //else//解帧失败继续等待
	                       // {
	                       // 	CCODOWN_FMT_DEBUG(nTtlIndex, "RelayTransDeal() INNER_RTS_RECV 收发地址不匹配...\n");
	                       //     RecvResult = -1;
	                      //  }
	                    }
						else
						{
							CCODOWN_FMT_DEBUG(nTtlIndex, "CheckMeterProtocol()电表规约解帧 报文格式解析错误...\n");
							CCODOWN_FMT_DEBUG(nTtlIndex, "RelayTransDeal() INNER_RTS_RECV UnPackMeterData() 3762解帧失败...\n");
						}
						
					}
				}
				
				break;

			}
		}
		else//判断是否已经超时
		{
			if ( abs(time(NULL) - CommBeginTime[nTtlIndex]) > OverTimeCnt[nTtlIndex])
			{
				RecvResult = -1;
				CCODOWN_FMT_DEBUG(nTtlIndex, "超时...\n");
			}
		}


		if (RecvResult == -1)
		{
			CommRetryCnt[nTtlIndex]++; //重试次数+1
			gTotalState[nTtlIndex].RelayTrans = INNER_RTS_RETRY; //切换到重试状态	
		}
		break;

	case INNER_RTS_RETRY:
		CCODOWN_FMT_DEBUG(nTtlIndex, "RelayTransDeal()穿透任务 INNER_RTS_RETRY...端口[%d] \n", nTtlIndex);
		CCODOWN_FMT_DEBUG(nTtlIndex, "进入重新尝试阶段 当前重试次数:%d, 最大允许重试次数MaxRetrtCnt:%d \n", CommRetryCnt[nTtlIndex], MaxRetrtCnt[nTtlIndex]);
		if(CommRetryCnt[nTtlIndex] > MaxRetrtCnt[nTtlIndex])
		{
			gTotalState[nTtlIndex].RelayTrans = INNER_RTS_END; //达到最大重试次数, 任务完成

			//组装消息报文
			PackInfoMsgStruct(0x0003, 0x0033, 0, (uint8*)&nZeroUint8, &InfoMsgData[nTtlIndex]);			
			PlcWriteQueue(pAmrSendUp, (uint8 *)&InfoMsgData[nTtlIndex], sizeof(MSG_INFO_T)+InfoMsgData[nTtlIndex].nPayloadLength);
		}
		else
		{
			gTotalState[nTtlIndex].RelayTrans = INNER_RTS_SEND; //切换到发送的状态
		}

		break;

	case INNER_RTS_END:
		CCODOWN_FMT_DEBUG(nTtlIndex, "RelayTransDeal()穿透任务 INNER_RTS_END...端口[%d] \n", nTtlIndex);
		CommRetryCnt[nTtlIndex] = 0;
		bProtocol3762[nTtlIndex] = FALSE;
        gTotalState[nTtlIndex].RelayTrans = INNER_RTS_IDLE;
		return 1;
		break;

	default:
		break;
	}

	return -1;

}


/*******************************************************************************
* 函数名称: SlaveMonitorDeal
* 函数功能: 从节点监控流程
* 输入参数: 
* 输出参数: 
* 返 回 值: 
*******************************************************************************/
int SlaveMonitorDeal(TASK_FUN_INPUT stTaskFunInput)
{
	int 			Ret = -1;
	static time_t	CommBeginTime[MAX_DOWN_THREAD_NUM];						//通讯开始时间
	int 			RecvLen;							//接收数据长度
	uint8			RecvBuf[MAX_QUEUE_BUFF_LEN];		//接收数据缓冲区
	AppData_T		AppData;							//应用层数据
	int 			RecvResult = 1; 				    //接收结果(1:成功 / -1:失败)
	static uint32	OverTimeCnt[MAX_DOWN_THREAD_NUM] = {0};					//超时时间
	static	uint16	MaxRetrtCnt[MAX_DOWN_THREAD_NUM] = {0};					//最大重试次数
	static	int 	CommRetryCnt[MAX_DOWN_THREAD_NUM] = {0};					//通讯重试次数
	static MeterPrtlFmt_T	SendMeterPrtlData[MAX_DOWN_THREAD_NUM];		//电表规约格式数据（从消息中获取，解析下发）
	MeterPrtlFmt_T	RecvMeterPrtlData;		//电表规约格式数据（下发之后收到的回复）
	uint16          offset;
	uint16  IndexID = 0;  //偏移位
	
	uint8           nTtlIndex = 0;  //不同的端口号对应不同的数组下标
	nTtlIndex = stTaskFunInput.TaskArrIndex;

	static AMR_SLAVE_NODE_MONITOR_T  AmrSlaveNodeMonitor[MAX_DOWN_THREAD_NUM];  //从节点监控
	static INFO_DATA_T  InfoMsgData[MAX_DOWN_THREAD_NUM];  //回复消息
	static INFO_DATA_HEAD_T InfoDataHead[MAX_DOWN_THREAD_NUM];  //消息原始HEAD

	switch (gTotalState[nTtlIndex].SlaveMonitor)
	{
	case INNER_RTS_IDLE://空闲执行,初始化
		CCODOWN_FMT_DEBUG(nTtlIndex, "SlaveMonitorDeal()从节点监控 INNER_RTS_IDLE...端口[%d] \n", nTtlIndex);
		ClearCommRecvBuff(nTtlIndex);  //清通讯接收缓冲区
		ReadAmrInfo(nTtlIndex);  //加载档案信息至抄表信息
		CommBeginTime[nTtlIndex] = time(NULL);
		OverTimeCnt[nTtlIndex] = 60;
		MaxRetrtCnt[nTtlIndex] = 0;
		memset(&SendMeterPrtlData[nTtlIndex], 0, sizeof(MeterPrtlFmt_T));
		memset(&AmrSlaveNodeMonitor[nTtlIndex], 0, sizeof(AMR_SLAVE_NODE_MONITOR_T));
		memset(&InfoMsgData[nTtlIndex], 0, sizeof(INFO_DATA_T));
		memset(&InfoDataHead[nTtlIndex], 0, sizeof(INFO_DATA_HEAD_T));

		//从队列中读取从节点监控报文
		Ret = GetSlaveMonitorFromList(nTtlIndex, &AmrSlaveNodeMonitor[nTtlIndex], &InfoDataHead[nTtlIndex]);
		if(Ret > 0)
		{
			memcpy(&InfoMsgData[nTtlIndex], &InfoDataHead[nTtlIndex], sizeof(INFO_DATA_HEAD_T));  //获取原始HEAD
			InfoMsgData[nTtlIndex].MsgRPM = 0;   //启动标志位, 回复，非启动
		}
		else
		{
			CCODOWN_FMT_DEBUG(nTtlIndex, "SlaveMonitorDeal() 从节点监控未查到报文, 直接结束! 端口:%d \n", nTtlIndex);
			gTotalState[nTtlIndex].SlaveMonitor = INNER_RTS_END;//任务完成
			break;
		}
		
		//模块上报的从节点超时时间≥8秒时，减2秒作为修正，抵消流程处理时间(按理应该从收到消息开始计时)。
		if(AmrSlaveNodeMonitor[nTtlIndex].nFrameTimeover >= 8)
		{
			OverTimeCnt[nTtlIndex] = AmrSlaveNodeMonitor[nTtlIndex].nFrameTimeover - 3;
		}
		else
		{
			OverTimeCnt[nTtlIndex] = AmrSlaveNodeMonitor[nTtlIndex].nFrameTimeover;
		}
		
		//模块上报的从节点超时时间小于10s时，修正为60s。
        //OverTimeCnt[nTtlIndex] = (OverTimeCnt[nTtlIndex] < 10) ? 60 : OverTimeCnt[nTtlIndex];
        CCODOWN_FMT_DEBUG(nTtlIndex, "从节点最大超时时间 修正OverTimeCnt[%d]=%d, 原始AmrSlaveNodeMonitor[%d].nFrameTimeover=%d \n", 
        	nTtlIndex, OverTimeCnt[nTtlIndex], nTtlIndex, AmrSlaveNodeMonitor[nTtlIndex].nFrameTimeover);

		//不用解帧, 直接从消息中取信息
		CCODOWN_FMT_DEBUG(nTtlIndex, "不用解帧, 直接从消息中取信息 \n");
		memcpy(SendMeterPrtlData[nTtlIndex].MtrAddr, AmrSlaveNodeMonitor[nTtlIndex].aSlaveNodeAddr, 6);//地址
		SendMeterPrtlData[nTtlIndex].DataLen = AmrSlaveNodeMonitor[nTtlIndex].nDataBufLen;//数据长度
		memcpy(SendMeterPrtlData[nTtlIndex].DataBuff, AmrSlaveNodeMonitor[nTtlIndex].aDataBuf, SendMeterPrtlData[nTtlIndex].DataLen);//报文数据

		gTotalState[nTtlIndex].SlaveMonitor = INNER_RTS_SEND;  //进入组帧处理
        
		break;

	case INNER_RTS_SEND: //发送
		CCODOWN_FMT_DEBUG(nTtlIndex, "SlaveMonitorDeal()从节点监控 INNER_RTS_SEND...端口[%d] \n", nTtlIndex);
		Ret = RelayCmdMeterPrtlData(SendMeterPrtlData[nTtlIndex], AmrSlaveNodeMonitor[nTtlIndex].aDataBuf, AmrSlaveNodeMonitor[nTtlIndex].nDataBufLen, nTtlIndex);
		if(Ret > 0)
		{
			CommBeginTime[nTtlIndex] = time(NULL);
			gTotalState[nTtlIndex].SlaveMonitor = INNER_RTS_RECV;
		}
		else
		{
			gTotalState[nTtlIndex].SlaveMonitor = INNER_RTS_RECV;
		}
		break;

	case INNER_RTS_RECV: //接收
		RecvResult = 1;  //接收结果(1:成功 / -1:失败)
		RecvLen = PlcReadQueue(pAmrRecvDown[nTtlIndex], RecvBuf);
		//PrintBuf(RecvBuf, RecvLen);
		if (RecvLen > 0)
		{
			CCODOWN_FMT_DEBUG(nTtlIndex, "SlaveMonitorDeal()从节点监控 INNER_RTS_RECV...端口[%d]...if (RecvLen > 0) \n", nTtlIndex);
			Ret = UnPackFrameData(RecvLen, RecvBuf, &AppData);

			switch(Ret)
			{
			case CHECK_RESULT_FAILUER:
			case CHECK_RESULT_CONFIRM:
				CCODOWN_FMT_DEBUG(nTtlIndex, "RelayTransDeal()从节点监控 INNER_RTS_RECV...CHECK_RESULT_FAILUER/CONFIRM失败...端口[%d] \n", nTtlIndex);
				break;

			case CHECK_RESULT_DENY:
				gTotalState[nTtlIndex].SlaveMonitor = INNER_RTS_END;  //否认帧, 任务完成
				CCODOWN_FMT_DEBUG(nTtlIndex, "RelayTransDeal()从节点监控 INNER_RTS_RECV...CHECK_RESULT_DENY否认...端口[%d] \n", nTtlIndex);
				
				//组装消息报文
				InfoMsgData[nTtlIndex].IID_ID = 0x0003;
				InfoMsgData[nTtlIndex].IOP_ID = 0x0038;
				InfoMsgData[nTtlIndex].nPayloadLength = 0;
				memset(InfoMsgData[nTtlIndex].aPayloadData, 0x00, InfoMsgData[nTtlIndex].nPayloadLength);  //payload
				
				PlcWriteQueue(pAmrSendUp, (uint8 *)&InfoMsgData[nTtlIndex], sizeof(MSG_INFO_T)+InfoMsgData[nTtlIndex].nPayloadLength);
				break;

			case CHECK_RESULT_SUCCEED:
				CCODOWN_FMT_DEBUG(nTtlIndex, "RelayTransDeal()从节点监控 INNER_RTS_RECV...CHECK_RESULT_SUCCEED成功...端口[%d] \n", nTtlIndex);
				if(((AppData.AFN == 0x13) || (AppData.AFN == 0x02)) && (AppData.Fn == 1))
				{
					if(AppData.AFN == 0x13)  //AppData.DataUnitBuff:上行时长2字节，协议1字节，长度1字节
					{
						if (AppData.DataUnitBuff[3] <= 1)
						{
							RecvResult = -1;
							break;
						}
						Ret = CheckMeterProtocol(AppData.DataUnitBuff[3], &AppData.DataUnitBuff[4], &RecvMeterPrtlData, &offset);
					}
					else  //AppData.DataUnitBuff:协议1字节，长度1字节
					{
						if (AppData.DataUnitBuff[1] <= 1)
						{
							RecvResult = -1;
							break;
						}
						Ret = CheckMeterProtocol(AppData.DataUnitBuff[1], &AppData.DataUnitBuff[2], &RecvMeterPrtlData, &offset);
					}

					if(Ret > 0)
                    {
                    	if(Ret == METER_PROTOCOL_645)
						{
							CCODOWN_FMT_DEBUG(nTtlIndex, "CheckMeterProtocol()电表规约解帧 报文协议为645...\n");
						}
						else if(Ret == METER_PROTOCOL_69845)
						{
							CCODOWN_FMT_DEBUG(nTtlIndex, "CheckMeterProtocol()电表规约解帧 报文协议为698.45...\n");
						}
						
                        Ret = memcmp(SendMeterPrtlData[nTtlIndex].MtrAddr, RecvMeterPrtlData.MtrAddr, 6);
                        if(Ret == 0)
                        {
                            gTotalState[nTtlIndex].SlaveMonitor = INNER_RTS_END;//任务完成

							//组装消息报文
							IndexID = 0;

							InfoMsgData[nTtlIndex].IID_ID = 0x0003;
							InfoMsgData[nTtlIndex].IOP_ID = 0x0038;
							if(AppData.AFN == 0x13)
							{
								ChangeLenToBuff(AppData.DataUnitBuff[3], &IndexID, InfoMsgData[nTtlIndex].aPayloadData);  //octet-string变长																	
								memcpy(&InfoMsgData[nTtlIndex].aPayloadData[IndexID], &AppData.DataUnitBuff[4], AppData.DataUnitBuff[3]);  //payload
								IndexID += AppData.DataUnitBuff[3];
								InfoMsgData[nTtlIndex].nPayloadLength = IndexID;  
							}
							else
							{
								ChangeLenToBuff(AppData.DataUnitBuff[1], &IndexID, InfoMsgData[nTtlIndex].aPayloadData);  //octet-string变长																	
								memcpy(&InfoMsgData[nTtlIndex].aPayloadData[IndexID], &AppData.DataUnitBuff[2], AppData.DataUnitBuff[1]);  //payload
								IndexID += AppData.DataUnitBuff[1];
								InfoMsgData[nTtlIndex].nPayloadLength = IndexID;  						
							}														
							
							PlcWriteQueue(pAmrSendUp, (uint8 *)&InfoMsgData[nTtlIndex], sizeof(MSG_INFO_T)+InfoMsgData[nTtlIndex].nPayloadLength);
                        }
                        else//解帧失败继续等待
                        {
                        	CCODOWN_FMT_DEBUG(nTtlIndex, "RelayTransDeal() INNER_RTS_RECV 收发地址不匹配...\n");
                            RecvResult = -1;
                        }
                    }
					else
					{
						CCODOWN_FMT_DEBUG(nTtlIndex, "CheckMeterProtocol()电表规约解帧 报文格式解析错误...\n");
						CCODOWN_FMT_DEBUG(nTtlIndex, "RelayTransDeal() INNER_RTS_RECV UnPackMeterData() 3762解帧失败...\n");
					}
					
				}

				break;

			}
		}
		else//判断是否已经超时
		{
			//printx("if (RecvLen 小于等于 0)...\n");
			if ( abs(time(NULL) - CommBeginTime[nTtlIndex]) > OverTimeCnt[nTtlIndex])
			{
				RecvResult = -1;
				CCODOWN_FMT_DEBUG(nTtlIndex, "if ((time(NULL) - CommBeginTime) > OverTimeCnt) 超时...\n");
			}
		}


		if (RecvResult == -1)
		{
			CommRetryCnt[nTtlIndex]++; //重试次数+1
			gTotalState[nTtlIndex].SlaveMonitor = INNER_RTS_RETRY; //切换到重试状态	
		}
		break;

	case INNER_RTS_RETRY:
		CCODOWN_FMT_DEBUG(nTtlIndex, "SlaveMonitorDeal()从节点监控 INNER_RTS_RETRY...端口[%d] \n", nTtlIndex);
		CCODOWN_FMT_DEBUG(nTtlIndex, "进入重新尝试阶段 当前重试次数:%d, 最大允许重试次数MaxRetrtCnt:%d \n", CommRetryCnt[nTtlIndex], MaxRetrtCnt[nTtlIndex]);
		if(CommRetryCnt[nTtlIndex] > MaxRetrtCnt[nTtlIndex])
		{
			gTotalState[nTtlIndex].SlaveMonitor = INNER_RTS_END; //达到最大重试次数, 任务完成

			//组装消息报文
			InfoMsgData[nTtlIndex].IID_ID = 0x0003;
			InfoMsgData[nTtlIndex].IOP_ID = 0x0038;
			InfoMsgData[nTtlIndex].nPayloadLength = 0;
			memset(InfoMsgData[nTtlIndex].aPayloadData, 0x00, InfoMsgData[nTtlIndex].nPayloadLength);  //payload
			
			PlcWriteQueue(pAmrSendUp, (uint8 *)&InfoMsgData[nTtlIndex], sizeof(MSG_INFO_T)+InfoMsgData[nTtlIndex].nPayloadLength);
		}
		else
		{
			gTotalState[nTtlIndex].SlaveMonitor = INNER_RTS_SEND; //切换到发送的状态
		}

		break;

	case INNER_RTS_END:
		CCODOWN_FMT_DEBUG(nTtlIndex, "SlaveMonitorDeal()从节点监控 INNER_RTS_END...端口[%d] \n", nTtlIndex);
		CommRetryCnt[nTtlIndex] = 0;
        gTotalState[nTtlIndex].SlaveMonitor = INNER_RTS_IDLE;
		return 1;
		break;

	default:
		break;
	}

	return -1;

}

/*******************************************************************************
* 函数名称: GetSlaveMonitorDelay
* 函数功能: 获取从节点监控延时
* 输入参数: 无
* 输出参数: 无
* 返 回 值: 
*******************************************************************************/
int GetSlaveMonitorDelay(TASK_FUN_INPUT stTaskFunInput)
{
	static time_t	CommBeginTime[MAX_DOWN_THREAD_NUM];						//通讯开始时间
	int 			SendLen;							//发送数据长度
	int 			RecvLen;							//接收数据长度
	uint8			RecvBuf[MAX_QUEUE_BUFF_LEN];		//接收数据缓冲区
	int 			RecvResult = 1; 				    //接收结果(1:成功 / -1:失败)
	static uint32	OverTimeCnt[MAX_DOWN_THREAD_NUM] = {0};					//超时时间
	static uint16	MaxRetrtCnt[MAX_DOWN_THREAD_NUM] = {0};					//最大重试次数
	static int 	    CommRetryCnt[MAX_DOWN_THREAD_NUM] = {0};			    //通讯重试次数
	uint16          IndexID = 0;  //偏移位
	uint16          DelaySecNum;  //从节点监控延时
	static uint8    SendType[MAX_DOWN_THREAD_NUM] = {0};  //0x13:发送13F1, 0x14:回复14F3
	
	uint8           nTtlIndex = 0;  //不同的端口号对应不同的数组下标
	nTtlIndex = stTaskFunInput.TaskArrIndex;

	AppData_T       SendAppData,RecvAppData;
	memset(&SendAppData, 0, sizeof(AppData_T));
	memset(&RecvAppData, 0, sizeof(AppData_T));

	static AMR_SLAVE_NODE_DELAY_TIME_T  AmrSlaveNodeMonitorTime[MAX_DOWN_THREAD_NUM];  //获取从节点监控延时
	static INFO_DATA_T     InfoMsgData[MAX_DOWN_THREAD_NUM];  //回复消息
	static INFO_DATA_HEAD_T InfoDataHead[MAX_DOWN_THREAD_NUM];  //消息原始HEAD
	static uint8 BakBuff14F3[MAX_DOWN_THREAD_NUM][500];  //备份上行14F3报文数据域

	switch (gTotalState[nTtlIndex].SlaveMonitorTime)
	{
	case INNER_RTS_IDLE://空闲执行,初始化
		CCODOWN_FMT_DEBUG(nTtlIndex, "GetSlaveMonitorDelay()获取从节点监控延时 INNER_RTS_IDLE...端口[%d] \n", nTtlIndex);
		ClearCommRecvBuff(nTtlIndex);  //清通讯接收缓冲区
		ReadAmrInfo(nTtlIndex);  //加载档案信息至抄表信息
		CommBeginTime[nTtlIndex] = time(NULL);
		OverTimeCnt[nTtlIndex] = 60;
		MaxRetrtCnt[nTtlIndex] = 0;
		memset(&AmrSlaveNodeMonitorTime[nTtlIndex], 0, sizeof(AMR_SLAVE_NODE_DELAY_TIME_T));
		memset(&InfoMsgData[nTtlIndex], 0, sizeof(INFO_DATA_T));
		memset(&InfoDataHead[nTtlIndex], 0, sizeof(INFO_DATA_HEAD_T));
		memset(&BakBuff14F3[nTtlIndex][0], 0, sizeof(BakBuff14F3[nTtlIndex]));
		SendType[nTtlIndex] = 0x13;

		//从队列中读取从节点监控报文
		int Ret = GetSlaveMonitorTimeFromList(nTtlIndex, &AmrSlaveNodeMonitorTime[nTtlIndex], &InfoDataHead[nTtlIndex]);
		if(Ret > 0)
		{
			memcpy(&InfoMsgData[nTtlIndex], &InfoDataHead[nTtlIndex], sizeof(INFO_DATA_HEAD_T));	//获取原始HEAD
			InfoMsgData[nTtlIndex].MsgRPM = 0;   //启动标志位, 回复，非启动
			gTotalState[nTtlIndex].SlaveMonitorTime = INNER_RTS_SEND;  //进入组帧处理
		}
		else
		{
			CCODOWN_FMT_DEBUG(nTtlIndex, "GetSlaveMonitorDelay() 获取从节点监控延时 未查到报文, 直接结束! 端口:%d \n", nTtlIndex);
			gTotalState[nTtlIndex].SlaveMonitorTime = INNER_RTS_END;  //结束
			break;
		}			
        
		break;

	case INNER_RTS_SEND: //发送
		CCODOWN_FMT_DEBUG(nTtlIndex, "GetSlaveMonitorDelay()获取从节点监控延时 INNER_RTS_SEND...端口[%d], SendType=0x%02x \n", nTtlIndex, SendType[nTtlIndex]);

		if(SendType[nTtlIndex] == 0x13)
		{
			SendAppData.AFN = 0x13;
			SendAppData.Fn	= 1;
			memcpy(SendAppData.DesAddr, AmrSlaveNodeMonitorTime[nTtlIndex].aSlaveNodeAddr, 6);  //目的地址
			SendAppData.DataUnitBuff[0] = AmrSlaveNodeMonitorTime[nTtlIndex].nProtocolType;  //通信协议类型
			SendAppData.DataUnitBuff[1] = 0x01;  //通信延时相关性标志
			SendAppData.DataUnitBuff[2] = 0x00;  //从节点附属节点数量
			SendAppData.DataUnitBuff[3] = AmrSlaveNodeMonitorTime[nTtlIndex].nDataBufLen;	//数据帧长度
			memcpy(&SendAppData.DataUnitBuff[4], AmrSlaveNodeMonitorTime[nTtlIndex].aDataBuf, SendAppData.DataUnitBuff[3]);
			SendAppData.DataUnitLen = 4 + SendAppData.DataUnitBuff[3];
			SendLen = PackFrameData(SendAppData, nTtlIndex);
			CCODOWN_FMT_DEBUG(nTtlIndex, "GetSlaveMonitorDelay()获取从节点监控延时 13F1 发送长度SendLen=%d \n", SendLen);
		}
		else if(SendType[nTtlIndex] == 0x14)
		{
			SendAppData.AFN = 0x14;
			SendAppData.Fn	= 3;
			memcpy(SendAppData.DesAddr, AmrSlaveNodeMonitorTime[nTtlIndex].aSlaveNodeAddr, 6);  //目的地址
			SendAppData.DataUnitBuff[0] = AmrSlaveNodeMonitorTime[nTtlIndex].nDataBufLen;	//数据帧长度
			memcpy(&SendAppData.DataUnitBuff[1], &BakBuff14F3[nTtlIndex][0], SendAppData.DataUnitBuff[0]);
			SendAppData.DataUnitLen = 1 + SendAppData.DataUnitBuff[0];
			SendLen = PackFrameData(SendAppData, nTtlIndex);
			CCODOWN_FMT_DEBUG(nTtlIndex, "GetSlaveMonitorDelay()获取从节点监控延时 14F3 发送长度SendLen=%d \n", SendLen);
		}

		
		gTotalState[nTtlIndex].SlaveMonitorTime = INNER_RTS_RECV;
		break;

	case INNER_RTS_RECV: //接收
		RecvResult = 1;  //接收结果(1:成功 / -1:失败)
		RecvLen = PlcReadQueue(pAmrRecvDown[nTtlIndex], RecvBuf);
		if (RecvLen > 0)
		{
			CCODOWN_FMT_DEBUG(nTtlIndex, "GetSlaveMonitorDelay()获取从节点监控延时 INNER_RTS_RECV...端口[%d]...if (RecvLen > 0) \n", nTtlIndex);
			Ret = UnPackFrameData(RecvLen, RecvBuf, &RecvAppData);

			switch(Ret)
			{
			case CHECK_RESULT_FAILUER:
			case CHECK_RESULT_DENY:
			case CHECK_RESULT_CONFIRM:
				CCODOWN_FMT_DEBUG(nTtlIndex, "GetSlaveMonitorDelay()获取从节点监控延时 INNER_RTS_RECV...CHECK_RESULT_FAILUER/DENY/CONFIRM失败...端口[%d] \n", nTtlIndex);
				break;

			case CHECK_RESULT_SUCCEED:
				CCODOWN_FMT_DEBUG(nTtlIndex, "GetSlaveMonitorDelay()获取从节点监控延时 INNER_RTS_RECV...CHECK_RESULT_SUCCEED成功...端口[%d] \n", nTtlIndex);
				if(SendType[nTtlIndex] == 0x13)
				{
					CCODOWN_FMT_DEBUG(nTtlIndex, "GetSlaveMonitorDelay() INNER_RTS_RECV SendType=0x%02x \n", SendType[nTtlIndex]);
					if((RecvAppData.AFN == 0x14) && (RecvAppData.Fn == 3))
					{												
						memcpy(&DelaySecNum, &RecvAppData.DataUnitBuff[6], 2);  //从节点地址6字节，预计延迟时间2字节
						memcpy(&BakBuff14F3[nTtlIndex][0], &RecvAppData.DataUnitBuff[9], RecvAppData.DataUnitBuff[8]);  //长度1字节
						CCODOWN_FMT_DEBUG(nTtlIndex, "GetSlaveMonitorDelay() if((RecvAppData.AFN == 0x14) && (RecvAppData.Fn == 3)) 监控延时:%d \n", DelaySecNum);

						gTotalState[nTtlIndex].SlaveMonitorTime = INNER_RTS_SEND;//跳到发送，对14F3响应
						SendType[nTtlIndex] = 0x14;
					}
				}
				else if(SendType[nTtlIndex] == 0x14)
				{
					CCODOWN_FMT_DEBUG(nTtlIndex, "GetSlaveMonitorDelay() INNER_RTS_RECV SendType=0x%02x \n", SendType[nTtlIndex]);
					if((RecvAppData.AFN == 0x13) && (RecvAppData.Fn == 1))
					{
						gTotalState[nTtlIndex].SlaveMonitorTime = INNER_RTS_END;//任务完成
						
						memcpy(&DelaySecNum, &RecvAppData.DataUnitBuff[0], 2);  //当前报文本地通信上行时长2字节
						CCODOWN_FMT_DEBUG(nTtlIndex, "GetSlaveMonitorDelay() if((RecvAppData.AFN == 0x13) && (RecvAppData.Fn == 1)) 监控延时:%d \n", DelaySecNum);

						//组装消息报文
						InfoMsgData[nTtlIndex].IID_ID = 0x0003;
						InfoMsgData[nTtlIndex].IOP_ID = 0x0037;
						Uint16ToCharArr(DelaySecNum, InfoMsgData[nTtlIndex].aPayloadData);
						IndexID += 2;
						InfoMsgData[nTtlIndex].nPayloadLength = IndexID;

						PlcWriteQueue(pAmrSendUp, (uint8 *)&InfoMsgData[nTtlIndex], sizeof(MSG_INFO_T)+InfoMsgData[nTtlIndex].nPayloadLength);

						break;
					}
				}				
				
			}
		}
		
		//判断是否已经超时
		{
			if ( abs(time(NULL) - CommBeginTime[nTtlIndex]) > OverTimeCnt[nTtlIndex])
			{
				RecvResult = -1;
				CCODOWN_FMT_DEBUG(nTtlIndex, "if ((time(NULL) - CommBeginTime) > OverTimeCnt) 超时...\n");
			}
		}

		if (RecvResult == -1)
		{
			CommRetryCnt[nTtlIndex]++; //重试次数+1
			gTotalState[nTtlIndex].SlaveMonitorTime = INNER_RTS_RETRY; //切换到重试状态	
		}
		break;

	case INNER_RTS_RETRY:
		CCODOWN_FMT_DEBUG(nTtlIndex, "GetSlaveMonitorDelay()获取从节点监控延时 INNER_RTS_RETRY...端口[%d] \n", nTtlIndex);
		CCODOWN_FMT_DEBUG(nTtlIndex, "进入重新尝试阶段 当前重试次数:%d, 最大允许重试次数MaxRetrtCnt:%d \n", CommRetryCnt[nTtlIndex], MaxRetrtCnt[nTtlIndex]);
		if(CommRetryCnt[nTtlIndex] > MaxRetrtCnt[nTtlIndex])
		{
			CCODOWN_FMT_DEBUG(nTtlIndex, "GetSlaveMonitorDelay() 未查询到从节点监控延时! \n");
			gTotalState[nTtlIndex].SlaveMonitorTime = INNER_RTS_END; //达到最大重试次数, 任务完成

			//组装消息报文
			InfoMsgData[nTtlIndex].IID_ID = 0x0003;
			InfoMsgData[nTtlIndex].IOP_ID = 0x0037;
			InfoMsgData[nTtlIndex].nPayloadLength = 0;
			memset(InfoMsgData[nTtlIndex].aPayloadData, 0x00, InfoMsgData[nTtlIndex].nPayloadLength);  //payload
			
			PlcWriteQueue(pAmrSendUp, (uint8 *)&InfoMsgData[nTtlIndex], sizeof(MSG_INFO_T)+InfoMsgData[nTtlIndex].nPayloadLength);
		}
		else
		{
			gTotalState[nTtlIndex].SlaveMonitorTime = INNER_RTS_SEND; //切换到发送的状态
		}

		break;

	case INNER_RTS_END:
		CCODOWN_FMT_DEBUG(nTtlIndex, "GetSlaveMonitorDelay()获取从节点监控延时 INNER_RTS_END...端口[%d] \n", nTtlIndex);
		CommRetryCnt[nTtlIndex] = 0;
        gTotalState[nTtlIndex].SlaveMonitorTime = INNER_RTS_IDLE;
		return 1;
		break;

	default:
		break;
	}

	return -1;
}

/*******************************************************************************
* 函数名称: GetBroadcastDelay
* 函数功能: 获取广播延时
* 输入参数: 无
* 输出参数: 无
* 返 回 值: 
*******************************************************************************/
int GetBroadcastDelay(TASK_FUN_INPUT stTaskFunInput)
{
	uint16  DelaySecNum = 0;  //广播延时时间(秒)
	AppData_T       SendAppData,RecvAppData;
	uint16  IndexID = 0;  //偏移位
	
	uint8           nTtlIndex = 0;  //不同的端口号对应不同的数组下标
	nTtlIndex = stTaskFunInput.TaskArrIndex;

	AMR_GET_BROADCAST_DELAY_T stAmrGetBroadcastDelay;  //获取广播延时
	memcpy(&stAmrGetBroadcastDelay, stTaskFunInput.InfoMsgBuff, sizeof(AMR_GET_BROADCAST_DELAY_T));

	INFO_DATA_T     InfoMsgData;  //回复消息
	memset(&InfoMsgData, 0, sizeof(InfoMsgData));
	memcpy(&InfoMsgData, &stTaskFunInput.InfoDataHead, sizeof(INFO_DATA_HEAD_T));  //获取原始HEAD
	InfoMsgData.MsgRPM = 0;   //启动标志位, 回复，非启动

	ReadAmrInfo(nTtlIndex);  //加载档案信息至抄表信息

	SendAppData.AFN = 0x03;
    SendAppData.Fn  = 9;
	SendAppData.DataUnitBuff[0] = stAmrGetBroadcastDelay.nProtocolType;  //通信协议类型
	SendAppData.DataUnitBuff[1] = (uint8)stAmrGetBroadcastDelay.nDataBufLen;  //报文长度(1字节)
	memcpy(&SendAppData.DataUnitBuff[2], stAmrGetBroadcastDelay.aDataBuf, SendAppData.DataUnitBuff[1]);
	SendAppData.DataUnitLen = 2 + SendAppData.DataUnitBuff[1];
	
	if(CommFunDeal(SendAppData, &RecvAppData, nTtlIndex) > 0)
    {
        memcpy(&DelaySecNum,RecvAppData.DataUnitBuff,2);
        CCODOWN_FMT_DEBUG(nTtlIndex, "GetBroadcastDelay() 查询到广播延时:%d \n", DelaySecNum);

		//组装消息报文
		InfoMsgData.IID_ID = 0x0003;
		InfoMsgData.IOP_ID = 0x0039;
		Uint16ToCharArr(DelaySecNum, InfoMsgData.aPayloadData);
		IndexID += 2;
		InfoMsgData.nPayloadLength = IndexID;
		
		PlcWriteQueue(pAmrSendUp, (uint8 *)&InfoMsgData, sizeof(MSG_INFO_T)+InfoMsgData.nPayloadLength);
		return 1;
		
    }
	else
	{
		CCODOWN_FMT_DEBUG(nTtlIndex, "GetBroadcastDelay() 未查询到广播延时! \n");
		
		//组装消息报文
		InfoMsgData.IID_ID = 0x0003;
		InfoMsgData.IOP_ID = 0x0039;
		InfoMsgData.nPayloadLength = 0;
		
		PlcWriteQueue(pAmrSendUp, (uint8 *)&InfoMsgData, sizeof(MSG_INFO_T)+InfoMsgData.nPayloadLength);
		return 1;
	}

}

/*******************************************************************************
* 函数名称:	CmdOopBroadcast
* 函数功能:	面向对象表广播校时
* 输入参数:	MeterData           电表规约格式数据
* 输出参数:	无
* 返 回 值: 无
*******************************************************************************/
void CmdOopBroadcast(MeterPrtlFmt_T MeterData, uint8 nArrIndex)
{
    AppData_T AppData;
    uint16    tmpCrc;

    AppData.AFN = 0x05;
    AppData.Fn = 3;
    AppData.DataUnitLen = MeterData.DataLen+2;
    AppData.DataUnitBuff[0] = MeterData.PrtlMode;
    AppData.DataUnitBuff[1] = MeterData.DataLen;
    //重新计算校验
    tmpCrc = CRC16_Get((uint8*)&MeterData.DataBuff[1] ,MeterData.DataLen-4);
    MeterData.DataBuff[MeterData.DataLen-3] =  tmpCrc & 0x00FF;
    MeterData.DataBuff[MeterData.DataLen-2] =  (tmpCrc & 0xFF00)>>8;
    memcpy(&AppData.DataUnitBuff[2], MeterData.DataBuff, MeterData.DataLen);

    PackFrameData(AppData, nArrIndex);
}


/*******************************************************************************
* 函数名称: BroadcastDeal
* 函数功能: 广播
* 输入参数: 无
* 输出参数: 无
* 返 回 值: 
*******************************************************************************/
int BroadcastDeal(TASK_FUN_INPUT stTaskFunInput)
{
	uint8	nProtocolType;	//规约类型
	MeterPrtlFmt_T MeterPrtlData;
	int Ret = -1;
	uint16  offset;  //报文头部偏移位

	uint8           nTtlIndex = 0;  //不同的端口号对应不同的数组下标
	nTtlIndex = stTaskFunInput.TaskArrIndex;

	AMR_DO_BROADCAST_T stAmrDoBroadcast;  //广播
	memcpy(&stAmrDoBroadcast, stTaskFunInput.InfoMsgBuff, sizeof(AMR_DO_BROADCAST_T));

	nProtocolType = stAmrDoBroadcast.nProtocolType;  //什么地方使用这个协议?

	MeterPrtlData.MtrType = METER_PLC;
    MeterPrtlData.CtrlCode = 0x08;
    MeterPrtlData.DataLen = 6;
    MeterPrtlData.PrtlMode = 0x00;
    memset(MeterPrtlData.MtrAddr,0x99,6);

	ReadAmrInfo(nTtlIndex);  //加载档案信息至抄表信息

	//依次按照645报文和698报文解析
	Ret = CheckMeterProtocol(stAmrDoBroadcast.nDataBufLen, stAmrDoBroadcast.aDataBuf, &MeterPrtlData, &offset);
	if(Ret == METER_PROTOCOL_645)
	{
		CCODOWN_FMT_DEBUG(nTtlIndex, "CheckMeterProtocol()电表规约解帧 报文协议为645...\n");
		MeterPrtlData.PrtlMode = nProtocolType;
        CmdDataRelay(MeterPrtlData, stAmrDoBroadcast.aDataBuf, stAmrDoBroadcast.nDataBufLen, nTtlIndex);//电表的组帧
	}
	else if(Ret == METER_PROTOCOL_69845)
	{
		CCODOWN_FMT_DEBUG(nTtlIndex, "CheckMeterProtocol()电表规约解帧 报文协议为698.45...\n");
		//面向对象广播校时帧，按原始报文发送
        if(Ret > 0 && MeterPrtlData.MtrAddr[0] == 0xAA && stAmrDoBroadcast.aDataBuf[offset+4] == 0xC0)
        {
            MeterPrtlData.DataLen = stAmrDoBroadcast.nDataBufLen;
            memcpy(MeterPrtlData.DataBuff, stAmrDoBroadcast.aDataBuf, MeterPrtlData.DataLen);

			MeterPrtlData.PrtlMode = nProtocolType;
            CmdOopBroadcast(MeterPrtlData, nTtlIndex);//面向对象表的组帧
        }
	}
	else
	{
		//报文解帧失败, 直接结束
	}

	return 1;

}


/*******************************************************************************
* 函数名称: WirelessNetStudyDeal
* 函数功能: 微功率无线组网处理流程
* 输入参数: 无
* 输出参数: 无
* 返 回 值: 
*******************************************************************************/
int WirelessNetStudyDeal(TASK_FUN_INPUT stTaskFunInput)
{
	return 1;

}

/*******************************************************************************
* 函数名称: SwitchChannelDeal
* 函数功能: 切换信道处理流程
* 输入参数: 无
* 输出参数: 无
* 返 回 值: 
*******************************************************************************/
int SwitchChannelDeal(TASK_FUN_INPUT stTaskFunInput)
{
	return 1;
}

/*******************************************************************************
* 函数名称: RouterStudyDeal
* 函数功能: 路由学习处理
* 输入参数: 无
* 输出参数: 无
* 返 回 值: >0                  完成
*           <0                  未完成
*******************************************************************************/
int RouterStudyDeal(TASK_FUN_INPUT stTaskFunInput)
{
	return 1;
}

/*******************************************************************************
* 函数名称:	ClearCommRecvBuff
* 函数功能:	清除通讯接收数据
* 输入参数:	无
* 输出参数:	无
* 返 回 值:	无
*******************************************************************************/
void ClearCommRecvBuff(uint8 nArrIndex)
{
    int     RecvLen = 0xff;                     //接收数据长度
    uint8   RecvBuf[MAX_QUEUE_BUFF_LEN];        //接收数据缓冲区

	uint8   nTtlIndex = 0;  //不同的端口号对应不同的数组下标
	nTtlIndex = nArrIndex;

    while (RecvLen > 0)
    {
        RecvLen = PlcReadQueue(pAmrRecvDown[nTtlIndex], RecvBuf);
    }
}

/*******************************************************************************
* 函数名称:	ClearCommQueueBuff
* 函数功能:	清除通讯接收数据
* 输入参数:	无
* 输出参数:	无
* 返 回 值:	无
*******************************************************************************/
void ClearCommQueueBuff(LockQueue_T *pQueueBuff)
{
    int     RecvLen = 0xff;                     //接收数据长度
    uint8   RecvBuf[MAX_QUEUE_BUFF_LEN] = {0};        //接收数据缓冲区

    while (RecvLen > 0)
    {
        RecvLen = PlcReadQueue(pQueueBuff, RecvBuf);
    }
}


/*******************************************************************************
* 函数名称:			RecvFrameCheck
* 函数功能:			下行发送数据内容与接收到得数据进行判断
* 输入参数:			AppData_T AppData        //下行发送的数据内容
                AppData_T RecvAppData    //上行发送的数据内容
* 输出参数:			NULL
* 返 回 值:	        > 0 接收到正确回码
                    < 0 回码内容错误

*******************************************************************************/
int RecvFrameCheck(AppData_T AppData,AppData_T RecvAppData, uint8 nArrIndex )
{
    static int DenyContinuationTimes[MAX_DOWN_THREAD_NUM] = {0};

	PLCTASK_DETAIL  stPlcTaskDetail;       //载波任务具体信息
	memset((uint8 *)&stPlcTaskDetail, 0x00, sizeof(stPlcTaskDetail));

	uint8 nTtlIndex = 0;  //不同的端口号对应不同的数组下标
	nTtlIndex = nArrIndex;

    //一旦出现不是连续的否认帧，重置否认连续次数
    if(!(RecvAppData.AFN == 0x0 && RecvAppData.Fn == 2))
    {
        DenyContinuationTimes[nTtlIndex] = 0;
    }

    switch(AppData.AFN)//
    {
    case 0x0://确认否认
       return 1;

    //注意:0x01,0x04,0x05,0x11,0x12在1376.2中只存在确认否认帧
    //<prohibition_modify>...</prohibition_modify>两个标签之间的内容不可修改
    case 0x01://初始化
    case 0x04://链路接口检测
    case 0x05://控制命令
    case 0x11://路由设置
        if(RecvAppData.AFN == 0x0 && RecvAppData.Fn == 1)
        {
            CCODOWN_FMT_DEBUG(nTtlIndex, "收到确认帧!  \n");
            return 1;
        }
		
        if(RecvAppData.AFN == 0x0 && RecvAppData.Fn == 2)
        {
            CCODOWN_FMT_DEBUG(nTtlIndex, "收到否认帧!  \n");
            DenyContinuationTimes[nTtlIndex]++;

            if(DenyContinuationTimes[nTtlIndex] > 5)
            {
                DenyContinuationTimes[nTtlIndex] = 0;
                CCODOWN_FMT_DEBUG(nTtlIndex, "连续收到否认帧, 超过5次, 重启抄表线程! \n");
                gRebootAmrThread[nTtlIndex] = 1; //抄表线程重启
	            sleep(3);             //等待线程由主进程的重启
            }

            return 1;
        }
        break;

    case 0x12://路由控制
        if(RecvAppData.AFN == 0x0 && RecvAppData.Fn == 1)
        {
            CCODOWN_FMT_DEBUG(nTtlIndex, "路由控制命令(1:重启 / 2:暂停 / 3:恢复)确认:%d \n", AppData.Fn);
            return 1;
        }

        if(RecvAppData.AFN == 0x0 && RecvAppData.Fn == 2)
        {
            //立即对模块进行重启
            CCODOWN_FMT_DEBUG(nTtlIndex, "路由控制命令(1:重启 / 2:暂停 / 3:恢复)否认:%d \n", AppData.Fn);
            return 1;
        }
        return 1;
        break;

    case 0x02://数据转发
    case 0x03://查询数据
    case 0x10://路由查询
    case 0x13://路由数据转发
    case 0x15://文件传输
        //接收完成AFN=0x03,F10,清除主动查询标志(统一在GetModuleManuID函数中检查)
        /*
        if(RecvAppData.AFN == 0x03 && RecvAppData.Fn == 10)
            CheckModuleSoftReset(0, nTtlIndex);
        */

        if(RecvAppData.AFN == AppData.AFN
                && RecvAppData.Fn == AppData.Fn)
            return 1;
        break;

    case 0x06://主动上报
        return 1;

    case 0x14://路由数据抄读
        if(RecvAppData.AFN == AppData.AFN)
            return 1;
        break;

    default:
        return 1;
    }

    return -1;

}


/*******************************************************************************
* 函数名称:			CommFunDeal
* 函数功能:			串口收发函数
* 输入参数:			NULL
* 输出参数:			NULL
* 返 回 值:	        > 0 接收成功
                    < 0 接收失败

*******************************************************************************/
int CommFunDeal(AppData_T SendAppData,AppData_T *RecvAppData, uint8 nArrIndex )
{
	//printx("int CommFunDeal(AppData_T SendAppData,AppData_T *RecvAppData, uint32 nPortID)...begin \n");
	
    uint8 	             		RetryCnt = 0;                       //接收尝试次数
    int                  		RecvLen;                            //接收数据长度
    uint8               		RecvBuf[MAX_QUEUE_BUFF_LEN];        //接收数据缓冲区
    static time_t        		CommBeginTime[MAX_DOWN_THREAD_NUM];                      //通讯开始时间
    uint16                      SendLen = 0;
    ACTIVE_COMM_STATE_T         CommStat = ACT_COMM_IDLE;
    int                         RecvResult = 1;                     //接收结果(1:成功 / -1:失败)

	uint8   nTtlIndex = 0;  //不同的端口号对应不同的数组下标
	nTtlIndex = nArrIndex;

    while(1)
    {
        switch(CommStat)
        {
        case ACT_COMM_IDLE:
			//printx("串口收发函数[%d]CommFunDeal()...ACT_COMM_IDLE \n", nTtlIndex);
            ClearCommRecvBuff(nTtlIndex);                    //清接收数据
            CommBeginTime[nTtlIndex] = time(NULL);
            CommStat = ACT_COMM_SEND;               //切换到发送
            break;

        case ACT_COMM_SEND:
			//printx("串口收发函数[%d]CommFunDeal()...ACT_COMM_SEND \n", nTtlIndex);
            SendLen = PackFrameData(SendAppData, nTtlIndex);
            if(SendLen > 0)
            {
               //发送AFN=0x03,F10,置主动查询标志(统一在GetModuleManuID函数中检查)
               /*
               if(SendAppData.AFN == 0x03 && SendAppData.Fn == 10)
                   CheckModuleSoftReset(1, nTtlIndex);
               */

                CommBeginTime[nTtlIndex] = time(NULL);
                CommStat = ACT_COMM_RECV;           //切换到接收
            }
            break;

        case ACT_COMM_RECV:
			//printx("串口收发函数[%d]CommFunDeal()...ACT_COMM_RECV \n", nTtlIndex);
            RecvLen = PlcReadQueue(pAmrRecvDown[nTtlIndex], RecvBuf);
            if(RecvLen > 0)
            {
                UnPackFrameData(RecvLen, RecvBuf,RecvAppData);
                if(RecvFrameCheck(SendAppData,*RecvAppData, nTtlIndex) > 0)
                    return 1;//接收成功
                else
                    RecvResult = -1;//接收失败

            }
            else
                RecvResult = -1;//接收失败

            if(RecvResult == -1)
            {
                if( abs(time(NULL) - CommBeginTime[nTtlIndex]) > MAX_COMM_DELAY_TIME)
                {
                    RetryCnt++;
                    CommStat = ACT_COMM_RETRY;
                }
            }

            break;

        case ACT_COMM_RETRY:
			//printx("串口收发函数[%d]CommFunDeal()...ACT_COMM_RETRY \n", nTtlIndex);
            if(RetryCnt > 3)
            {
                CommStat = ACT_COMM_IDLE;
                return 2;
            }

            CommStat = ACT_COMM_SEND;
            break;

        default:
            break;

        }

        usleep(20000);
    }

    return 1;
}


/*******************************************************************************
* 函数名称:			CommFunDeal
* 函数功能:			串口收发函数
* 输入参数:			NULL
* 输出参数:			NULL
* 返 回 值:	        > 0 接收成功
                    < 0 接收失败

*******************************************************************************/
int CommFunDealByType(AppData_T SendAppData, AppData_T *RecvAppData, uint8 nArrIndex, int32 nFunType)
{
	//printx("int CommFunDeal(AppData_T SendAppData,AppData_T *RecvAppData, uint32 nPortID)...begin \n");
	
    uint8 	             		RetryCnt = 0;                       //接收尝试次数
    int                  		RecvLen;                            //接收数据长度
    uint8               		RecvBuf[MAX_QUEUE_BUFF_LEN];        //接收数据缓冲区
    static time_t        		CommBeginTime[MAX_DOWN_THREAD_NUM];                      //通讯开始时间
    uint16                      SendLen = 0;
    ACTIVE_COMM_STATE_T         CommStat = ACT_COMM_IDLE;
    int                         RecvResult = 1;                     //接收结果(1:成功 / -1:失败)

	uint8   nTtlIndex = 0;  //不同的端口号对应不同的数组下标
	nTtlIndex = nArrIndex;

    while(1)
    {
        switch(CommStat)
        {
        case ACT_COMM_IDLE:
			//printx("串口收发函数[%d]CommFunDeal()...ACT_COMM_IDLE \n", nTtlIndex);
			if(MS_ADDR_SEARCH == nFunType)//搜表
			{
				ClearCommQueueBuff(&gAmrRecvDownSearchMeter[nTtlIndex]);                    //清接收数据
			}
			else
			{
				ClearCommRecvBuff(nTtlIndex);                    //清接收数据
			}
            
            CommBeginTime[nTtlIndex] = time(NULL);
            CommStat = ACT_COMM_SEND;               //切换到发送
            break;

        case ACT_COMM_SEND:
			//printx("串口收发函数[%d]CommFunDeal()...ACT_COMM_SEND \n", nTtlIndex);
            SendLen = PackFrameData(SendAppData, nTtlIndex);
            if(SendLen > 0)
            {
				//发送AFN=0x03,F10,置主动查询标志(统一在GetModuleManuID函数中检查)
				/*
				if(SendAppData.AFN == 0x03 && SendAppData.Fn == 10)
				{
					CheckModuleSoftReset(1, nTtlIndex);
				}
				*/

                CommBeginTime[nTtlIndex] = time(NULL);
                CommStat = ACT_COMM_RECV;           //切换到接收
            }
            break;

        case ACT_COMM_RECV:
			//printx("串口收发函数[%d]CommFunDeal()...ACT_COMM_RECV \n", nTtlIndex);
			if(MS_ADDR_SEARCH == nFunType)//搜表
			{
				RecvLen = PlcReadQueue(&gAmrRecvDownSearchMeter[nTtlIndex], RecvBuf);
			}
			else
			{
				RecvLen = PlcReadQueue(pAmrRecvDown[nTtlIndex], RecvBuf);
			}
			
            if(RecvLen > 0)
            {
                UnPackFrameData(RecvLen, RecvBuf, RecvAppData);
                if(RecvFrameCheck(SendAppData,*RecvAppData, nTtlIndex) > 0)
                    return 1;//接收成功
                else
                    RecvResult = -1;//接收失败

            }
            else
                RecvResult = -1;//接收失败

            if(RecvResult == -1)
            {
                if( abs(time(NULL) - CommBeginTime[nTtlIndex]) > MAX_COMM_DELAY_TIME)
                {
                    RetryCnt++;
                    CommStat = ACT_COMM_RETRY;
                }
            }

            break;

        case ACT_COMM_RETRY:
			//printx("串口收发函数[%d]CommFunDeal()...ACT_COMM_RETRY \n", nTtlIndex);
            if(RetryCnt > 3)
            {
                CommStat = ACT_COMM_IDLE;
                return 2;
            }

            CommStat = ACT_COMM_SEND;
            break;

        default:
            break;

        }

        usleep(20000);
    }

    return 1;
}



/*******************************************************************************
* 函数名称: AppRouterDealFun
* 函数功能: 外部路由操作
* 输入参数: 无
* 输出参数: 无
* 内部处理:
* 返 回 值: >0                  成功
*           <0                  失败
*******************************************************************************/
int AppRouterDealFun(TASK_FUN_INPUT stTaskFunInput)
{
	AppData_T	SendAppData;						//发送
	AppData_T	RecvAppData;						//接收
	memset(&SendAppData, 0x00, sizeof(AppData_T));
	memset(&RecvAppData, 0x00, sizeof(AppData_T));

	uint8 nTtlIndex = 0;  //不同的端口号对应不同的数组下标
	nTtlIndex = stTaskFunInput.TaskArrIndex;

	SendAppData.AFN = 0x12;
	SendAppData.Fn = stTaskFunInput.RoutrtCtrl;
	SendAppData.DataUnitLen = 0;

	if(CommFunDeal(SendAppData, &RecvAppData, nTtlIndex))
	{
		return 1;
	}
	else
	{
		return 0;
	}
}



/*******************************************************************************
* 函数名称: OftenAmrDeal
* 函数功能: 处理频繁执行的任务(执行周期小于一天)
* 输入参数: 无
* 输出参数: 无
* 返 回 值: >0                  成功
*           <0                  失败
*******************************************************************************/
int OftenAmrDeal(TASK_FUN_INPUT stTaskFunInput)
{
	return 1;
}

/*******************************************************************************
* 函数名称: PackFrameDataX
* 函数功能: 组3762帧,组完后暂不发送到串口
* 输入参数: AppData
* 输出参数: Buff        3762帧
* 返 回 值: 长度
*******************************************************************************/
uint16 PackFrameDataX(AppData_T AppData, uint8* Buff, uint8 nArrIndex)
{
	//printx("PackFrameDataX()组3762帧 组完后暂不发送到串口 端口[%d]...begin \n", nArrIndex);
	
    uint8  	    FrameBuff[MAX_QUEUE_BUFF_LEN];      //帧数据缓冲区
    uint16      DataIndex = 0;
    int         i = 0;
    uint8       CheckSum = 0;                       //校验和

    FrameBuff[0] = 0x68;

    if (gPrtlLinkLen == 1)
    {
        //跳过帧长度
        DataIndex = 2;
    }
    else
    {
        //跳过帧长度
        DataIndex = 3;
    }

    //控制码
    GetCtrlCode(&FrameBuff[DataIndex], AppData, nArrIndex);
    DataIndex++;

    //信息域
    GetInfoZoneData(&FrameBuff[DataIndex], AppData, nArrIndex);

    DataIndex += 6;
    //地址域
    if (GetAddrZoneData(&FrameBuff[DataIndex], AppData, nArrIndex) > 0)
    {
        DataIndex += 12;
    }

    //应用数据域
    FrameBuff[DataIndex] = AppData.AFN;
    GetDtInfo(&FrameBuff[DataIndex+1], AppData.Fn);
    DataIndex += 3;

    if (AppData.DataUnitLen > 0)
    {
        memcpy(&FrameBuff[DataIndex], AppData.DataUnitBuff, AppData.DataUnitLen);
        DataIndex += AppData.DataUnitLen;
    }

    //检验和的范围为控制域+用户数据区
    for (i = (gPrtlLinkLen+1); i < DataIndex; i++)
    {
        CheckSum += FrameBuff[i];
    }
    FrameBuff[DataIndex] = CheckSum;
    DataIndex++;

    //帧尾
    FrameBuff[DataIndex] = 0x16;
    DataIndex++;

    if (gPrtlLinkLen == 1)
    {
        //回填长度L
        FrameBuff[1] = (uint8)DataIndex;
    }
    else
    {
        //回填长度L
        *(uint16*)&FrameBuff[1] = DataIndex;
    }

    memcpy((void *)Buff, (void *)FrameBuff, DataIndex);
	//printx("PackFrameDataX()组3762帧 端口[%d] 组完之后的报文帧为...end \n", nArrIndex);
	//PrintBuf((uint8 *)Buff, DataIndex);
    return DataIndex;
}


/*******************************************************************************
* 函数名称: CmdMeterPrtlMakeFrame
* 函数功能: 组F1-01下行帧
* 输入参数: 
* 输出参数: 
* 返 回 值: 
*******************************************************************************/
uint16 CmdMeterPrtlMakeFrame(AppData_T AppData, uint8 *Buff, uint8 nArrIndex)
{
	//printx("CmdMeterPrtlMakeFrame()组F1-01下行帧 端口号[%d]...begin \n", nPortID);

    AppData.AFN = 0xF1;
    AppData.Fn  = 1;
    AppData.ModuleID = 1;

    return PackFrameDataX(AppData, Buff, nArrIndex);
}

/*******************************************************************************
* 函数名称: CcoPackAFN03F4
* 函数功能: 组AFN03F4下行帧
* 输入参数: 
* 输出参数: 
* 返 回 值: 
*******************************************************************************/
uint16 CcoPackAFN03F4(AppData_T AppData, uint8 *Buff, uint8 nArrIndex)
{
    AppData.AFN = 0x03;
    AppData.Fn  = 4;
    AppData.DataUnitLen = 0;
    return PackFrameDataX(AppData, Buff, nArrIndex);
}

/*******************************************************************************
* 函数名称: ParalAmrMakeFrame
* 函数功能: 并行抄表组帧
* 输入参数: 无
* 输出参数: 无
* 返 回 值: 1                  全部完成
*           0               帧未抄读完成，切换到发送
*******************************************************************************/
int ParalAmrMakeFrame(uint8 nArrIndex)
{
	//printx("int ParalAmrMakeFrame(uint32 nPortID)端口号[%d]并行抄表组帧...begin \n", nPortID);
	
    int          i;
    uint16       FrameLen = 0;
    uint8        FrameBuff[MAX_QUEUE_BUFF_LEN];  //帧数据缓冲区
    AppData_T    AppData;
    //int          nActiveAmrInfoNum = 0;
	uint8	     nProtocolType;	//协议类型
	uint16       IndexID = 0;  //偏移位
	uint16       nDataBufLen = 0;  //消息中心的抄表原始报文长度
	AMR_MODE_ACTIVE_T AmrModeActive;  //抄表信息
	int          nCmpResult = 0;
	int          ret = 0;

	uint8   nTtlIndex = 0;  //不同的端口号对应不同的数组下标
	nTtlIndex = nArrIndex;
	
    for(i = 0; i < MAX_NORMAL_AMR_INFO_CNT; i++)
    {
    	//从抄表消息缓冲区查询报文
		ret = QueryAmrFromList(&AmrModeActive, nTtlIndex);
		if(ret < 0)
		{
			break;  //没有读到抄表报文
		}

		//查找到需要发送的空闲队列
        if(gGlobalSeqInfo[nTtlIndex].FrameSeqInfo[i].ValidFlag == 1)
        {
        	nCmpResult = memcmp(gGlobalSeqInfo[nTtlIndex].FrameSeqInfo[i].MeterAddr, AmrModeActive.nMeterAddr, 6);
			if(nCmpResult == 0)
			{
				//标志位有效的电表重新收到抄表报文，可能已经超时，重新抄表
				CCODOWN_FMT_DEBUG(nTtlIndex, "端口[%d] FrameSeqInfo[%d].ValidFlag==1, 抄表队列FrameSeqInfo[%d].MeterAddr:%02x %02x %02x %02x %02x %02x, 抄表报文电表地址:%02x %02x %02x %02x %02x %02x, 收到新的抄表报文，重新抄表. \n", 
					nTtlIndex, i, i, gGlobalSeqInfo[nTtlIndex].FrameSeqInfo[i].MeterAddr[0], gGlobalSeqInfo[nTtlIndex].FrameSeqInfo[i].MeterAddr[1], gGlobalSeqInfo[nTtlIndex].FrameSeqInfo[i].MeterAddr[2], 
					gGlobalSeqInfo[nTtlIndex].FrameSeqInfo[i].MeterAddr[3], gGlobalSeqInfo[nTtlIndex].FrameSeqInfo[i].MeterAddr[4], gGlobalSeqInfo[nTtlIndex].FrameSeqInfo[i].MeterAddr[5], 
					AmrModeActive.nMeterAddr[0], AmrModeActive.nMeterAddr[1],AmrModeActive.nMeterAddr[2],AmrModeActive.nMeterAddr[3],
					AmrModeActive.nMeterAddr[4],AmrModeActive.nMeterAddr[5]);
			}
			else
			{
				//标志位有效的电表没有重新收到抄表报文，继续等待，跳到seq的下一个位置
				/*
				CCODOWN_FMT_DEBUG(nTtlIndex, "端口[%d] FrameSeqInfo[%d].ValidFlag, 抄表队列FrameSeqInfo[%d].MeterAddr:%02x %02x %02x %02x %02x %02x, 抄表报文电表地址:%02x %02x %02x %02x %02x %02x, 没有收到新的抄表报文，跳到RTS_RECV，继续抄表. \n", 
					nTtlIndex, i, i, gGlobalSeqInfo[nTtlIndex].FrameSeqInfo[i].MeterAddr[0], gGlobalSeqInfo[nTtlIndex].FrameSeqInfo[i].MeterAddr[1], gGlobalSeqInfo[nTtlIndex].FrameSeqInfo[i].MeterAddr[2], 
					gGlobalSeqInfo[nTtlIndex].FrameSeqInfo[i].MeterAddr[3], gGlobalSeqInfo[nTtlIndex].FrameSeqInfo[i].MeterAddr[4], gGlobalSeqInfo[nTtlIndex].FrameSeqInfo[i].MeterAddr[5], 
					AmrModeActive.nMeterAddr[0], AmrModeActive.nMeterAddr[1],AmrModeActive.nMeterAddr[2],AmrModeActive.nMeterAddr[3],
					AmrModeActive.nMeterAddr[4],AmrModeActive.nMeterAddr[5]);
				*/
				
				continue;
			}
		}
		DeleteAmrFromList(nTtlIndex);//删除掉第一条信息
		
		//printx("ParalAmrMakeFrame()并行抄表组帧 循环i=[%d]时从gActiveAmrInfoList[%d]取出的抄表信息为: \n", i, nTtlIndex);
		//PrintAmrActiveDetail(AmrModeActive);
		//printx("ParalAmrMakeFrame()并行抄表组帧 循环i=[%d]时取出一条抄表信息之后还剩余[%d]条信息 \n", i, gActiveAmrInfoList[nTtlIndex].nActiveAmrInfoNum);

		memcpy(AppData.DesAddr, AmrModeActive.nMeterAddr, 6);
		nProtocolType = GetProtocolByMeterAddr(AppData.DesAddr, nTtlIndex);

		IndexID = 0;
		AppData.DataUnitBuff[IndexID] = nProtocolType;  //规约类型，1字节
		IndexID++;
		AppData.DataUnitBuff[IndexID] = 0x00;  //保留，1字节
		IndexID++;
		nDataBufLen = AmrModeActive.nDataBufLen;
  		Uint16ToCharArr_r(nDataBufLen, &AppData.DataUnitBuff[IndexID]);  //报文长度，2字节
		IndexID += 2;
		memcpy(&AppData.DataUnitBuff[IndexID], AmrModeActive.aDataBuf, nDataBufLen);
		IndexID += nDataBufLen;
		AppData.DataUnitLen = IndexID;

		//保存当前帧序号
		gGlobalSeqInfo[nTtlIndex].CurSeqCnt ++;
		gGlobalSeqInfo[nTtlIndex].FrameSeqInfo[i].CommBeginTime = time(NULL);
		gGlobalSeqInfo[nTtlIndex].FrameSeqInfo[i].ValidFlag = 1;
		gGlobalSeqInfo[nTtlIndex].FrameSeqInfo[i].FrameSeq = g_InerSeq[nTtlIndex];
		gGlobalSeqInfo[nTtlIndex].FrameSeqInfo[i].PrtlMode = nProtocolType;
		memcpy(gGlobalSeqInfo[nTtlIndex].FrameSeqInfo[i].MeterAddr, AppData.DesAddr, 6);
		
		FrameLen += CmdMeterPrtlMakeFrame(AppData, &FrameBuff[FrameLen], nTtlIndex);
		//printx("ParalAmrMakeFrame()并行抄表组帧 循环i=[%d]时FrameLen=[%d]...\n", i, FrameLen);
    	
	}
	
    if(FrameLen)
    {
    	PlcWriteQueue(pAmrSendDown[nTtlIndex], FrameBuff, FrameLen);
		
		CCODOWN_FMT_DEBUG(nTtlIndex, "ParalAmrMakeFrame()并行抄表组帧 FrameLen=[%d] 3762完整报文为: \n", FrameLen);
		CCODOWN_BUF_DEBUG(nTtlIndex, (uint8 *)FrameBuff, FrameLen, "报文");
		return 1;
    }

    return 0;
}

/*******************************************************************************
* 函数名称: IsValidSeq
* 函数功能: 判断上行帧是否有效帧
* 输入参数: InSeq     需判断的帧序号
* 输出参数: OutSeq    输出对应的帧索引
* 返 回 值: 1         有效
*           0      无效
*******************************************************************************/
uint8 IsValidSeq(uint8* OutSeq, uint8 InSeq, uint8 nArrIndex)
{
	//printx("uint8 IsValidSeq()判断上行帧是否有效帧 InSeq:%d...begin \n", InSeq);
    int i = 0;
    for(i = 0; i < MAX_NORMAL_AMR_INFO_CNT; i++)
    {
        CCODOWN_FMT_DEBUG(nArrIndex, "IsValidSeq() gGlobalSeqInfo.FrameSeqInfo[%d].FrameSeq=%02x, ValidFlag=%d, *OutSeq=%d \n", 
			i, gGlobalSeqInfo[nArrIndex].FrameSeqInfo[i].FrameSeq, gGlobalSeqInfo[nArrIndex].FrameSeqInfo[i].ValidFlag, *OutSeq);

		//上行帧序号在所发有效帧之内
        if((gGlobalSeqInfo[nArrIndex].FrameSeqInfo[i].FrameSeq == InSeq)
                && (gGlobalSeqInfo[nArrIndex].FrameSeqInfo[i].ValidFlag == 1))
        {      	
            *OutSeq = i;
			CCODOWN_FMT_DEBUG(nArrIndex, "IsValidSeq() 匹配! gGlobalSeqInfo.FrameSeqInfo[%d].FrameSeq=%02x, ValidFlag=%d, *OutSeq=%d \n", 
				i, gGlobalSeqInfo[nArrIndex].FrameSeqInfo[i].FrameSeq, gGlobalSeqInfo[nArrIndex].FrameSeqInfo[i].ValidFlag, *OutSeq);
			return 1;
        }
    }

    return 0;
}

/*******************************************************************************
* 函数名称: IsActiveAmrFinished
* 函数功能: 判断主动抄表列表中的所有任务是否完成
* 输入参数: 
* 输出参数: 
* 返 回 值: 1         有效
*           0      无效
*******************************************************************************/
uint8 IsActiveAmrFinished(uint8 nArrIndex)
{
    int i = 0;
	BOOL bFinished = TRUE;
	int         OverTimeCnt = 63;  //超时时间
	
    for(i = 0; i < MAX_NORMAL_AMR_INFO_CNT; i++)
    {
    	if(gGlobalSeqInfo[nArrIndex].FrameSeqInfo[i].ValidFlag == 1)
        {
        	bFinished = FALSE;
			if( abs(time(NULL) - gGlobalSeqInfo[nArrIndex].FrameSeqInfo[i].CommBeginTime) > OverTimeCnt)
            {
				CCODOWN_FMT_DEBUG(nArrIndex, "IsActiveAmrFinished() 超时OverTimeCnt=%d! if((time(NULL)-FrameSeqInfo[%d].CommBeginTime)>OverTimeCnt) \n", OverTimeCnt, i);
                RefreshPnState(i, nArrIndex);
				CCODOWN_FMT_DEBUG(nArrIndex, "IsActiveAmrFinished() RefreshPnState()刷新之后 gGlobalSeqInfo.FrameSeqInfo[%d].FrameSeq=%02x, ValidFlag=%d \n", 
					i, gGlobalSeqInfo[nArrIndex].FrameSeqInfo[i].FrameSeq, gGlobalSeqInfo[nArrIndex].FrameSeqInfo[i].ValidFlag);
            }
        }
    }

    return bFinished;
}

/*******************************************************************************
* 函数名称: RefreshPnState
* 函数功能: 刷新Pn的失败数据列表
* 输入参数: 
* 输出参数: 无
* 返 回 值: 无
*******************************************************************************/
void RefreshPnState(uint8 SeqIndex, uint8 nArrIndex)
{
    //uint16 Pn = gGlobalSeqInfo[nArrIndex].FrameSeqInfo[SeqIndex].Pn;

    gGlobalSeqInfo[nArrIndex].FrameSeqInfo[SeqIndex].ValidFlag = 0;
    if(gGlobalSeqInfo[nArrIndex].CurSeqCnt > 0)
        gGlobalSeqInfo[nArrIndex].CurSeqCnt--;

    return;
}


/*******************************************************************************
* 函数名称: BroadBand_ParallelAmr
* 函数功能: 宽带日常抄表
* 输入参数: 无
* 输出参数: 无
* 返 回 值: >0                  成功
*           <0                  失败
*******************************************************************************/
int BroadBand_ParallelAmr(TASK_FUN_INPUT stTaskFunInput)
{
    
    int             ret;
    uint8           SeqIndex = 0;
    int             RecvLen = 0;                              //接收数据长度
    uint8           RecvBuf[MAX_QUEUE_BUFF_LEN] = {0};        //接收数据缓冲区
    AppData_T       AppData = {0};                            //应用层数据
    INFO_DATA_T InfoMsgData;
    uint16          IndexID = 0;  //偏移位

    uint8       nTtlIndex = 0;  //不同的端口号对应不同的数组下标
    //uint16      nMeterFilesID = 0;  //档案序号
    uint16      nDataLenF101;  //F1-01上行原始报文长度

    memset(&InfoMsgData, 0x00, sizeof(InfoMsgData));
    
    nTtlIndex = stTaskFunInput.TaskArrIndex;

    //printx("int BroadBand_ParallelAmr(TASK_FUN_INPUT stTaskFunInput)宽带日常抄表 端口[%d]...begin \n", nTtlIndex);

    switch (gTotalState[nTtlIndex].NormalAmr)
    {
    case RTS_IDLE:                              //空闲
        //printx("int BroadBand_ParallelAmr(TASK_FUN_INPUT stTaskFunInput)宽带日常抄表 端口[%d]...RTS_IDLE \n", nTtlIndex);
        ReadAmrInfo(nTtlIndex);  //加载档案信息至抄表信息
        ClearCommRecvBuff(nTtlIndex);                   //清除接收缓冲区
        memset((void *)&gGlobalSeqInfo[nTtlIndex], 0x0, sizeof(GLOBAL_SEQ_INFO_T));         //清帧信息
        if(gActiveAmrInfoList[nTtlIndex].nActiveAmrInfoNum > 0)
        {
            gTotalState[nTtlIndex].NormalAmr = RTS_SEND;
        }
        else
        {
            //抄表任务队列没有任务，可能前一次抄表已经全部取出
            gTotalState[nTtlIndex].NormalAmr = RTS_END;
        }

        //更新抄表状态
        gAmrStateQuery[nTtlIndex].nModuleAmrState = 1;

        break;

    case RTS_SEND:                              //发送
        //printx("int BroadBand_ParallelAmr(TASK_FUN_INPUT stTaskFunInput)宽带日常抄表 端口[%d]...RTS_SEND \n", nTtlIndex);
        if(ParalAmrMakeFrame(nTtlIndex))
        {
            //全部抄读完成, 跳到RTS_RECV
            CCODOWN_FMT_DEBUG(nTtlIndex, "BroadBand_ParallelAmr()宽带日常抄表 端口[%d]...RTS_SEND \n", nTtlIndex);
        }
        else
        {
            //当前抄表任务队列为空
        }
        gTotalState[nTtlIndex].NormalAmr = RTS_RECV;
        break;

    case RTS_RECV:                              //接收
        RecvLen = PlcReadQueue(pAmrRecvDown[nTtlIndex], RecvBuf);
        if (RecvLen > 0)
        {
            CCODOWN_FMT_DEBUG(nTtlIndex, "BroadBand_ParallelAmr()宽带日常抄表 端口[%d]...RTS_RECV \n", nTtlIndex);
            
            ret = UnPackFrameData(RecvLen, RecvBuf, &AppData);
            
            CCODOWN_FMT_DEBUG(nTtlIndex, "BroadBand_ParallelAmr()接收帧检查 ret:%d, AppData.RInfo[5] = %02x \n", ret, AppData.RInfo[5]);

            //判断接收帧序号是否存在于发送队列的帧信息内
            if(IsValidSeq(&SeqIndex, AppData.RInfo[5], nTtlIndex) == 0)
            {
                CCODOWN_FMT_DEBUG(nTtlIndex, "BroadBand_ParallelAmr() SeqIndex:%d, AppData.RInfo[5]:%02x...上行帧非期望帧, 继续接收 \n", SeqIndex, AppData.RInfo[5]);
                break;
            }

            switch(ret)
            {
            case CHECK_RESULT_DENY:
                //主节点忙或该测量点正在抄读则继续等待
                if((AppData.DataUnitBuff[0] == 9) || (AppData.DataUnitBuff[0] == 111))
                {
                    break;
                }
                else
                {
                    RefreshPnState(SeqIndex, nTtlIndex);
                    gTotalState[nTtlIndex].NormalAmr = RTS_SEND; //切换到发送状态
                    break;
                }
            case CHECK_RESULT_FAILUER:
            case CHECK_RESULT_CONFIRM:
                RefreshPnState(SeqIndex, nTtlIndex);
                gTotalState[nTtlIndex].NormalAmr = RTS_SEND; //切换到发送状态
                break;

            case CHECK_RESULT_SUCCEED:
                if ((AppData.AFN == 0xF1) && (AppData.Fn == 1))
                {
                    CCODOWN_FMT_DEBUG(nTtlIndex, "BroadBand_ParallelAmr()端口[%d]...RTS_RECV...CHECK_RESULT_SUCCEED...if ((AppData.AFN == 0xF1) && (AppData.Fn == 1)) SeqIndex:%d \n", nTtlIndex, SeqIndex);
                    
                    //组装消息报文
                    InfoMsgData.IP  = inet_addr(gCtsRouterAppIP[nTtlIndex]);
                    InfoMsgData.IP  = ntohl(InfoMsgData.IP);
                    InfoMsgData.port = gCtsRouterAppPort[nTtlIndex];
                    InfoMsgData.MsgPRIORITY = 0;
                    InfoMsgData.MsgRPM = 1;
                    InfoMsgData.MsgIndex = GetInfoIndex();
                    InfoMsgData.MsgLabel = nTtlIndex;  //mqtt存放端口序号(0或1)
                    InfoMsgData.SourLen = strlen(gCcoRouterAppName);
                    memset(InfoMsgData.SourAddr, 0x00, InfoMsgData.SourLen);
                    memcpy(InfoMsgData.SourAddr, gCcoRouterAppName, InfoMsgData.SourLen);
                    InfoMsgData.DestLen = strlen(gCtsRouterAppName[nTtlIndex]);
                    memset(InfoMsgData.DestAddr, 0x00, InfoMsgData.DestLen);
                    memcpy(InfoMsgData.DestAddr, gCtsRouterAppName[nTtlIndex], InfoMsgData.DestLen);
    
                    InfoMsgData.IID_ID = 0x0003;
                    InfoMsgData.IOP_ID = 0x0031;
                    IndexID = 0;
                    Uint32ToCharArr(gPortID[nTtlIndex], &InfoMsgData.aPayloadData[IndexID]);  //端口号，4字节
                    IndexID += 4;
                    //nMeterFilesID = GetFilesIdByMeterAddr(gGlobalSeqInfo[nTtlIndex].FrameSeqInfo[SeqIndex].MeterAddr, nTtlIndex);
                    //Uint16ToCharArr(nMeterFilesID, &InfoMsgData.aPayloadData[IndexID]);  //档案序号，2字节
                    //IndexID += 2;
                    memcpy_r(&InfoMsgData.aPayloadData[IndexID], gGlobalSeqInfo[nTtlIndex].FrameSeqInfo[SeqIndex].MeterAddr, 6); //载波从节点地址，6字节
                    IndexID += 6;
                    InfoMsgData.aPayloadData[IndexID] = gGlobalSeqInfo[nTtlIndex].FrameSeqInfo[SeqIndex].PrtlMode;  //协议类型，1字节
                    IndexID += 1;
                    CharArrToUint16_r(&AppData.DataUnitBuff[1], &nDataLenF101);  //数据帧长度，F1-01是2字节（小端，前低后高）
                    ChangeLenToBuff(nDataLenF101, &IndexID, &InfoMsgData.aPayloadData[IndexID]);  //数据帧长度
                    memcpy(&InfoMsgData.aPayloadData[IndexID], &AppData.DataUnitBuff[3], nDataLenF101);  //数据帧
                    IndexID +=  nDataLenF101;
                    InfoMsgData.aPayloadData[IndexID] = 0x01;  //PLC_READING_INFO OPTIONAL 是否含有抄读信息标志, 0x00:无, 0x01:有
                    IndexID += 1;
                    InfoMsgData.aPayloadData[IndexID] = (AppData.RInfo[0]>>4) & 0x0f;  //PLC_READING_INFO 开始 中继路由级数 0-15
                    IndexID += 1;
                    InfoMsgData.aPayloadData[IndexID] = AppData.RInfo[1] & 0x0f;  //PLC_READING_INFO 信道标识
                    IndexID += 1;
                    InfoMsgData.aPayloadData[IndexID] = AppData.RInfo[2] & 0x0f;  //PLC_READING_INFO 载波抄读相位
                    IndexID += 1;
                    InfoMsgData.aPayloadData[IndexID] = (AppData.RInfo[3]>>4) & 0x0f;  //PLC_READING_INFO 载波信号品质(末级应答信号品质)
                    IndexID += 1;
                    InfoMsgData.aPayloadData[IndexID] = AppData.RInfo[0] & 0x01;  //PLC_READING_INFO 载波标志(路由标识?)
                    IndexID += 1;
                    InfoMsgData.aPayloadData[IndexID++] = 0x00;  //PLC_READING_INFO 结束 保留 3字节
                    InfoMsgData.aPayloadData[IndexID++] = 0x00;  //PLC_READING_INFO 结束 保留 3字节
                    InfoMsgData.aPayloadData[IndexID++] = 0x00;  //PLC_READING_INFO 结束 保留 3字节
                    
                    InfoMsgData.nPayloadLength = IndexID;
                    
                    PlcWriteQueue(pAmrSendUp, (uint8 *)&InfoMsgData, sizeof(MSG_INFO_T)+InfoMsgData.nPayloadLength);

                    gGlobalSeqInfo[nTtlIndex].FrameSeqInfo[SeqIndex].ValidFlag = 0;
                }
                else
                {
                    CCODOWN_FMT_DEBUG(nTtlIndex, "BroadBand_ParallelAmr()...AppData.AFN=%02x, AppData.Fn=%d...接收报文非AFN F1-F1期望帧! \n", AppData.AFN, AppData.Fn);
                }

                break;
            }           
        }
        else
        {
            //printx("BroadBand_ParallelAmr()宽带日常抄表 端口[%d]...RTS_RECV...RecvLen小于0 \n", nTtlIndex);
            //判断5条抄表任务是否均已完成
            if(IsActiveAmrFinished(nTtlIndex))
            {
                CCODOWN_FMT_DEBUG(nTtlIndex, "BroadBand_ParallelAmr()端口[%d]...RTS_RECV...IsActiveAmrFinished()5条抄表任务均已完成 \n", nTtlIndex);
                gTotalState[nTtlIndex].NormalAmr = RTS_END;
            }
            else
            {
                gTotalState[nTtlIndex].NormalAmr = RTS_SEND; //切换到发送状态
            }
        }
        
        return -1;

    case RTS_END:                               //结束
        {
            gTotalState[nTtlIndex].NormalAmr = RTS_IDLE;
            //更新抄表状态
            gAmrStateQuery[nTtlIndex].nModuleAmrState = 0;
            return 1;
        }       
        break;

    default:
        gTotalState[nTtlIndex].NormalAmr = RTS_IDLE;
        break;
    }

    return -1;
}



/*******************************************************************************
* 函数名称: NormalDataReading
* 函数功能: 日常数据抄收
* 输入参数: 无
* 输出参数: 无
* 返 回 值: >0                  成功
*           <0                  失败
*******************************************************************************/
int NormalDataReading(TASK_FUN_INPUT stTaskFunInput)
{
	//printx("int NormalDataReading(TASK_FUN_INPUT stTaskFunInput)日常数据抄收...\n");

	return BroadBand_ParallelAmr(stTaskFunInput);
	
}
#if 0
/*******************************************************************************
* 函数名称: HplcInfoReport_NetNode
* 函数功能: HPLC查询信息上报 网络节点信息
* 输入参数: 无
* 输出参数: 无
* 返 回 值: >0                  成功
*           <0                  失败
*******************************************************************************/
int HplcInfoReport_NetNode(TASK_FUN_INPUT stTaskFunInput)
{
	uint8           nTtlIndex = 0;  //不同的端口号对应不同的数组下标
	nTtlIndex = stTaskFunInput.TaskArrIndex;
	CCODOWN_FMT_DEBUG(nTtlIndex, "HplcInfoReport_NetNode()查询网络节点信息 HPLC_QUERY_END...端口[%d] \n", nTtlIndex);
	
	gQueryTypeHPLC[nTtlIndex].BitType.node = 0;
	return 1;

}
#endif
/*******************************************************************************
* 函数名称: HplcInfoReport_NodeVersion
* 函数功能: HPLC查询信息上报 网络节点版本信息
* 输入参数: 无
* 输出参数: 无
* 返 回 值: >0                  成功
*           <0                  失败
*******************************************************************************/
int HplcInfoReport_NodeVersion(TASK_FUN_INPUT stTaskFunInput)
{
    int i   = 0;
    int ret = 0;
    static  time_t  CommBeginTime[MAX_DOWN_THREAD_NUM];  //通讯开始时间
    static  uint32  OverTimeCnt[MAX_DOWN_THREAD_NUM] = {0};  //超时时间
    static  uint16  CurNodeIndex[MAX_DOWN_THREAD_NUM] = {0};  //2013版当前索引从1开始
    uint16  CurInfoIndex = 0;  //消息组帧偏移位
    uint16  TotalCount = 0;  //节点总数量
    uint8   ReplyCount = 0;  //本次应答的节点数量


    AppData_T   SendAppData;                        //发送
    AppData_T   RecvAppData;                        //接收
    memset(&SendAppData, 0x00, sizeof(AppData_T));
    memset(&RecvAppData, 0x00, sizeof(AppData_T));
    uint16 frameindex = 0;
    uint8           nTtlIndex = 0;  //不同的端口号对应不同的数组下标
    nTtlIndex = stTaskFunInput.TaskArrIndex;
    //MESSAGE_MQTT_T *msg = &gQueryTypeHPLC[nTtlIndex].msg;
    //uint8 payload[MAX_MSG_PAYLOAD_LEN]={0};
    INFO_DATA_T     InfoMsgData = {0};  //回复消息
    uint8 addr[6]={0};
    uint16 softversion = 0;
    OOP_DATE_T versiondate;
    memset(&versiondate,0x00,sizeof(OOP_DATE_T));
    uint8 factorycode[2]={0};
    uint8 icoode[2]={0};
    switch (gTotalState[nTtlIndex].HplcQuery)
    {
        case HPLC_QUERY_IDLE://空闲执行,初始化
        {
            CCODOWN_FMT_DEBUG(nTtlIndex, "HplcInfoReport_Chip()查询网络节点版本信息 HPLC_QUERY_IDLE...端口[%d] \n", nTtlIndex);
            ClearCommRecvBuff(nTtlIndex);  //清通讯接收缓冲区
            //ReadAmrInfo(nTtlIndex);  //加载档案信息至抄表信息
            CurNodeIndex[nTtlIndex] = 1;//0 表示 CCO， 1 表示第 1 个载波从节点， 2 以后
            CommBeginTime[nTtlIndex] = time(NULL);
            OverTimeCnt[nTtlIndex] = 30;
            //memset(&gNodeVersionInfo,0x00,sizeof(HPLC_NODEVERSION_INFO_T));
            gTotalState[nTtlIndex].HplcQuery = HPLC_QUERY_SEND;  //进入组帧处理
            break;
        }
        case HPLC_QUERY_SEND: //发送
        {
            CCODOWN_FMT_DEBUG(nTtlIndex, "HplcInfoReport_Chip()查询网络节点版本信息 HPLC_QUERY_SEND...端口[%d] \n", nTtlIndex);

            //发送报文组帧
            SendAppData.AFN = 0x10;
            SendAppData.Fn = 104;
            SendAppData.DataUnitLen = 3;  //节点起始序号2字节, 节点数量1字节
            memcpy(SendAppData.DataUnitBuff, &CurNodeIndex[nTtlIndex], 2);  //从节点起始序号，2字节
            SendAppData.DataUnitBuff[2] = 15;  //每次查询节点数量，1字节
            CCODOWN_FMT_DEBUG(nTtlIndex, "发送报文SendAppData 节点起始序号=%d, 节点数量=%d \n", CurNodeIndex[nTtlIndex], SendAppData.DataUnitBuff[2]);
            
            ret = CommFunDeal(SendAppData, &RecvAppData, nTtlIndex);
            if(ret > 0)
            {

                //接收报文解帧
                memcpy(&TotalCount, &RecvAppData.DataUnitBuff[frameindex], 2);  //节点总数量
                frameindex+=2;
                ReplyCount = RecvAppData.DataUnitBuff[frameindex++];  //本次应答的数量
                CCODOWN_FMT_DEBUG(nTtlIndex, "接收报文 节点总数量=%d,本次应答节点数量=%d \n", 
                    TotalCount, ReplyCount);

                if(ReplyCount > 0)
                {
                    CurInfoIndex = 0;
                    //msg->tAXDR.index = GetInfoIndex();
                    Uint32ToCharArr(gPortID[nTtlIndex], &InfoMsgData.aPayloadData[CurInfoIndex]);  //端口号
                    CurInfoIndex += 4;
                    InfoMsgData.aPayloadData[CurInfoIndex] = 0x01;  //是否含有起始序号信息, 0x00:无, 0x01:有
                    CurInfoIndex += 1;
                    memcpy_r(&InfoMsgData.aPayloadData[CurInfoIndex], (uint8 *)&CurNodeIndex[nTtlIndex] , 2);  //起始序号2字节
                    CurInfoIndex += 2;
                    InfoMsgData.aPayloadData[CurInfoIndex] = 0x09;  //查询HPLC信息choice标志
                    CurInfoIndex += 1;
                    ChangeLenToBuff(ReplyCount, &CurInfoIndex, &InfoMsgData.aPayloadData[CurInfoIndex]);  //信息个数seq of
                    CCODOWN_FMT_DEBUG(nTtlIndex, "信息个数seq of: %d(十进制) \n", ReplyCount);
                    
                    for(i=0; i<ReplyCount; i++)
                    {
                        memcpy(addr,&RecvAppData.DataUnitBuff[frameindex],6);
                        frameindex+=6; 
                        memcpy_r(&InfoMsgData.aPayloadData[CurInfoIndex],addr,6);
                        CurInfoIndex += 6;
                        softversion = commfun_BcdToHex(RecvAppData.DataUnitBuff[frameindex+1])*100+commfun_BcdToHex(RecvAppData.DataUnitBuff[frameindex]);
                        frameindex+=2;
                        versiondate.mday = commfun_BcdToHex(RecvAppData.DataUnitBuff[frameindex++]);
                        versiondate.month = commfun_BcdToHex(RecvAppData.DataUnitBuff[frameindex++]);
                        versiondate.year = commfun_BcdToHex(RecvAppData.DataUnitBuff[frameindex++])+2000;
                        memcpy_r(factorycode,&RecvAppData.DataUnitBuff[frameindex],2);
                        frameindex+=2;
                        memcpy_r(icoode,&RecvAppData.DataUnitBuff[frameindex],2);
                        frameindex+=2;

                        memcpy_r(&InfoMsgData.aPayloadData[CurInfoIndex],&softversion,2);
                        CurInfoIndex+=2;
                        memcpy_r(&InfoMsgData.aPayloadData[CurInfoIndex],&versiondate.year,2);
                        CurInfoIndex+=2;
                        InfoMsgData.aPayloadData[CurInfoIndex++] = versiondate.month;
                        InfoMsgData.aPayloadData[CurInfoIndex++] = versiondate.mday;
                        InfoMsgData.aPayloadData[CurInfoIndex++] = 0xff;
                        memcpy(&InfoMsgData.aPayloadData[CurInfoIndex], factorycode, 2);  //厂商代码
                        CurInfoIndex += 2;
                        memcpy(&InfoMsgData.aPayloadData[CurInfoIndex], icoode, 2);  //芯片代码
                        CurInfoIndex += 2;
                    }
                    //cco_axdr_reponse(&msg->tAXDR, payload, CurInfoIndex);
                    InfoMsgData.nPayloadLength = CurInfoIndex;
                    PlcWriteQueue(pAmrSendUp, (uint8 *)&InfoMsgData, sizeof(MSG_INFO_T)+InfoMsgData.nPayloadLength);

                }

                CurNodeIndex[nTtlIndex] += ReplyCount;  //当前序号累加
                if(CurNodeIndex[nTtlIndex] >= TotalCount)
                {
                    gTotalState[nTtlIndex].HplcQuery = HPLC_QUERY_END;//任务完成
                    break;
                }
            }

            gTotalState[nTtlIndex].HplcQuery = HPLC_QUERY_RECV;  //判断是否超时
            break;
        }
        
        case HPLC_QUERY_RECV:  //接收
        {
            CCODOWN_FMT_DEBUG(nTtlIndex, "HplcInfoReport_Chip()查询网络节点版本信息HPLC_QUERY_RECV...端口[%d] \n", nTtlIndex);
            if ( abs(time(NULL) - CommBeginTime[nTtlIndex]) > OverTimeCnt[nTtlIndex])
            {
                CCODOWN_FMT_DEBUG(nTtlIndex, "if ((time(NULL) - CommBeginTime) > OverTimeCnt) 超时...\n");
                gTotalState[nTtlIndex].HplcQuery = HPLC_QUERY_END;  //累计超时, 任务完成
            }
            else
            {
                gTotalState[nTtlIndex].HplcQuery = HPLC_QUERY_SEND;  //进入组帧处理
            }
            
            break;
        }

        case HPLC_QUERY_RETRY:  //重试
        {
            CCODOWN_FMT_DEBUG(nTtlIndex, "HplcInfoReport_Chip()查询网络节点版本信息 HPLC_QUERY_RETRY...端口[%d] \n", nTtlIndex);
            gTotalState[nTtlIndex].HplcQuery = HPLC_QUERY_END;  //达到最大重试次数, 任务完成
            break;
        }

        case HPLC_QUERY_END:  //结束
        {
            CCODOWN_FMT_DEBUG(nTtlIndex, "HplcInfoReport_Chip()查询网络节点版本信息 HPLC_QUERY_END...端口[%d] \n", nTtlIndex);
            gTotalState[nTtlIndex].HplcQuery = HPLC_QUERY_IDLE;
            gQueryTypeHPLC[nTtlIndex].BitType.nodeversion = 0;
            return 1;
            break;
        }

        default:
            break;
    }

    return -1;
}

#if 0
/*******************************************************************************
* 函数名称: HplcInfoReport_NetBase
* 函数功能: HPLC查询信息上报 网络基本信息
* 输入参数: 无
* 输出参数: 无
* 返 回 值: >0                  成功
*           <0                  失败
*******************************************************************************/
int HplcInfoReport_NetBase(TASK_FUN_INPUT stTaskFunInput)
{
	uint8           nTtlIndex = 0;  //不同的端口号对应不同的数组下标
	nTtlIndex = stTaskFunInput.TaskArrIndex;
	CCODOWN_FMT_DEBUG(nTtlIndex, "HplcInfoReport_NetBase()查询网络基本信息 HPLC_QUERY_END...端口[%d] \n", nTtlIndex);
	
	gQueryTypeHPLC[nTtlIndex].BitType.base = 0;
	return 1;

}
#endif
/*******************************************************************************
* 函数名称: HplcInfoReport_Module
* 函数功能: HPLC查询信息上报 模块版本信息
* 输入参数: 无
* 输出参数: 无
* 返 回 值: >0                  成功
*           <0                  失败
*******************************************************************************/
int HplcInfoReport_Module(TASK_FUN_INPUT stTaskFunInput)
{
    int ret = 0;
    static  time_t  CommBeginTime[MAX_DOWN_THREAD_NUM];  //通讯开始时间
    static  uint32  OverTimeCnt[MAX_DOWN_THREAD_NUM] = {0};  //超时时间
    uint16  CurInfoIndex = 0;  //消息组帧偏移位
    uint16  frameindex=0;
    uint8   num = 0;  

    AppData_T   SendAppData;                        //发送
    AppData_T   RecvAppData;                        //接收
    memset(&SendAppData, 0x00, sizeof(AppData_T));
    memset(&RecvAppData, 0x00, sizeof(AppData_T));
    
    uint8           nTtlIndex = 0;  //不同的端口号对应不同的数组下标
    nTtlIndex = stTaskFunInput.TaskArrIndex;
    uint16 softversion = 0;
    OOP_DATE_T versiondate;
    memset(&versiondate,0x00,sizeof(OOP_DATE_T));
    uint8 factorycode[2]={0};
    uint8 icoode[2]={0};
    //MESSAGE_MQTT_T *msg = &gQueryTypeHPLC[nTtlIndex].msg;
    //uint8 payload[MAX_MSG_PAYLOAD_LEN]={0};
    INFO_DATA_T     InfoMsgData = {0};  //回复消息

    switch (gTotalState[nTtlIndex].HplcQuery)
    {
        case HPLC_QUERY_IDLE://空闲执行,初始化
        {
            CCODOWN_FMT_DEBUG(nTtlIndex, "HplcInfoReport_Module()查询模块节点信息 HPLC_QUERY_IDLE...端口[%d] \n", nTtlIndex);
            ClearCommRecvBuff(nTtlIndex);  //清通讯接收缓冲区
            //ReadAmrInfo(nTtlIndex);  //加载档案信息至抄表信息
            CommBeginTime[nTtlIndex] = time(NULL);
            OverTimeCnt[nTtlIndex] = 30;

            gTotalState[nTtlIndex].HplcQuery = HPLC_QUERY_SEND;  //进入组帧处理
            break;
        }
        case HPLC_QUERY_SEND: //发送
        {
            CCODOWN_FMT_DEBUG(nTtlIndex, "HplcInfoReport_Module()查询模块节点信息 HPLC_QUERY_SEND...端口[%d] \n", nTtlIndex);

            SendAppData.AFN = 0x10;
            SendAppData.Fn = 104;
            SendAppData.DataUnitLen = 3;  
            SendAppData.DataUnitBuff[2] = 1;    //只需要查CCO的 节点起始序号0 查询节点数量1
            ret = CommFunDeal(SendAppData, &RecvAppData, nTtlIndex);
            if(ret > 0)
            {
                frameindex+=2;
                num = RecvAppData.DataUnitBuff[frameindex++];
                if(num==0)
                {
                    CCODOWN_FMT_DEBUG(nTtlIndex, "HplcInfoReport_Module()查询模块节点信息 回复节点个数为0\n");
                    gTotalState[nTtlIndex].HplcQuery = HPLC_QUERY_END;//任务完成
                    break;
                }
                frameindex+=6; //地址不需要
                softversion = commfun_BcdToHex(RecvAppData.DataUnitBuff[frameindex+1])*100+commfun_BcdToHex(RecvAppData.DataUnitBuff[frameindex]);
                frameindex+=2;
                versiondate.mday = commfun_BcdToHex(RecvAppData.DataUnitBuff[frameindex++]);
                versiondate.month = commfun_BcdToHex(RecvAppData.DataUnitBuff[frameindex++]);
                versiondate.year = commfun_BcdToHex(RecvAppData.DataUnitBuff[frameindex++])+2000;
                memcpy_r(factorycode,&RecvAppData.DataUnitBuff[frameindex],2);
                frameindex+=2;
                memcpy_r(icoode,&RecvAppData.DataUnitBuff[frameindex],2);
                frameindex+=2;

                CurInfoIndex = 0;

                //msg->tAXDR.index = GetInfoIndex();
                Uint32ToCharArr(gPortID[nTtlIndex], &InfoMsgData.aPayloadData[CurInfoIndex]);  //端口号
                CCODOWN_FMT_DEBUG(nTtlIndex, "端口号: %02x %02x %02x %02x \n", 
                        InfoMsgData.aPayloadData[CurInfoIndex], InfoMsgData.aPayloadData[CurInfoIndex+1], 
                        InfoMsgData.aPayloadData[CurInfoIndex+2], InfoMsgData.aPayloadData[CurInfoIndex+3]);
                
                CurInfoIndex += 4;
                InfoMsgData.aPayloadData[CurInfoIndex] = 0x00;  //是否含有起始序号信息, 0x00:无, 0x01:有
                CCODOWN_FMT_DEBUG(nTtlIndex, "是否含有起始序号信息: %02x (0x00:无, 0x01:有) \n", InfoMsgData.aPayloadData[CurInfoIndex]);
                
                CurInfoIndex += 1;
                InfoMsgData.aPayloadData[CurInfoIndex] = 0x08;  //查询HPLC信息choice标志
                CCODOWN_FMT_DEBUG(nTtlIndex, "查询HPLC信息choice标志: %02x \n", InfoMsgData.aPayloadData[CurInfoIndex]);
                
                CurInfoIndex += 1;

                memcpy(&InfoMsgData.aPayloadData[CurInfoIndex], factorycode, 2);  //厂商代码
                CCODOWN_FMT_DEBUG(nTtlIndex, "厂商代码: %02x %02x \n", 
                        InfoMsgData.aPayloadData[CurInfoIndex], InfoMsgData.aPayloadData[CurInfoIndex+1]);
                CurInfoIndex += 2;
                
                memcpy(&InfoMsgData.aPayloadData[CurInfoIndex], icoode, 2);  //芯片代码
                CCODOWN_FMT_DEBUG(nTtlIndex, "芯片代码: %02x %02x \n", 
                        InfoMsgData.aPayloadData[CurInfoIndex], InfoMsgData.aPayloadData[CurInfoIndex+1]);
                CurInfoIndex += 2;

                memcpy_r(&InfoMsgData.aPayloadData[CurInfoIndex],&versiondate.year,2);
                CurInfoIndex+=2;
                InfoMsgData.aPayloadData[CurInfoIndex++] = versiondate.month;
                InfoMsgData.aPayloadData[CurInfoIndex++] = versiondate.mday;
                InfoMsgData.aPayloadData[CurInfoIndex++] = 0xff;
                memcpy_r(&InfoMsgData.aPayloadData[CurInfoIndex],&softversion,2);
                CurInfoIndex+=2;
                //cco_axdr_reponse(&msg->tAXDR, payload, CurInfoIndex);
                InfoMsgData.nPayloadLength = CurInfoIndex;
                PlcWriteQueue(pAmrSendUp, (uint8 *)&InfoMsgData, sizeof(MSG_INFO_T)+InfoMsgData.nPayloadLength);

                gTotalState[nTtlIndex].HplcQuery = HPLC_QUERY_END;//任务完成
                break;
            }

            gTotalState[nTtlIndex].HplcQuery = HPLC_QUERY_RECV;
            break;
        }
        
        case HPLC_QUERY_RECV:  //接收
        {
            CCODOWN_FMT_DEBUG(nTtlIndex, "HplcInfoReport_Module()查询模块节点信息 HPLC_QUERY_RECV...端口[%d] \n", nTtlIndex);
            if ( abs(time(NULL) - CommBeginTime[nTtlIndex]) > OverTimeCnt[nTtlIndex])
            {
                CCODOWN_FMT_DEBUG(nTtlIndex, "if ((time(NULL) - CommBeginTime) > OverTimeCnt) 超时...\n");
                gTotalState[nTtlIndex].HplcQuery = HPLC_QUERY_END;  //累计超时, 任务完成
            }
            else
            {
                gTotalState[nTtlIndex].HplcQuery = HPLC_QUERY_SEND;  //进入组帧处理
            }
            
            break;
        }

        case HPLC_QUERY_RETRY:  //重试
        {
            CCODOWN_FMT_DEBUG(nTtlIndex, "HplcInfoReport_Module()查询模块节点信息 HPLC_QUERY_RETRY...端口[%d] \n", nTtlIndex);
            gTotalState[nTtlIndex].HplcQuery = HPLC_QUERY_END;  //达到最大重试次数, 任务完成
            break;
        }

        case HPLC_QUERY_END:  //结束
        {
            CCODOWN_FMT_DEBUG(nTtlIndex, "HplcInfoReport_Module()查询模块节点信息 HPLC_QUERY_END...端口[%d] \n", nTtlIndex);
            gTotalState[nTtlIndex].HplcQuery = HPLC_QUERY_IDLE;
            gQueryTypeHPLC[nTtlIndex].BitType.module = 0;
            return 1;
            break;
        }

        default:
            break;
    }

    return -1;
}

/*******************************************************************************
* 函数名称: HplcInfoReport_Slave
* 函数功能: HPLC查询信息上报 从节点ID信息
* 输入参数: 无
* 输出参数: 无
* 返 回 值: >0                  成功
*           <0                  失败
*******************************************************************************/
int HplcInfoReport_Slave(TASK_FUN_INPUT stTaskFunInput)
{
	int i   = 0;
	int ret = 0;
	static  time_t  CommBeginTime[MAX_DOWN_THREAD_NUM];  //通讯开始时间
	static  uint32  OverTimeCnt[MAX_DOWN_THREAD_NUM] = {0};  //超时时间
	static  uint16  CurNodeIndex[MAX_DOWN_THREAD_NUM] = {0};  //2013版当前索引从1开始
	uint16  CurInfoIndex = 0;  //消息组帧偏移位
	uint16  TotalCount = 0;  //节点总数量
	uint8   ReplyCount = 0;  //本次应答的节点数量
	//uint16  ReplyNodeIndex = 0;  //本次应答的起始序号
	uint16  NodeIndexTemp = 0;  //节点序号, 临时变量
	uint16  CurFrameIndex = 0;  //收到的报文解析偏移位
	uint8   LenTemp = 0;  //模块ID号长度, 临时变量

	AppData_T   SendAppData;                        //发送
    AppData_T   RecvAppData;                        //接收
    memset(&SendAppData, 0x00, sizeof(AppData_T));
	memset(&RecvAppData, 0x00, sizeof(AppData_T));
	
	uint8           nTtlIndex = 0;  //不同的端口号对应不同的数组下标
	nTtlIndex = stTaskFunInput.TaskArrIndex;

	INFO_DATA_T     InfoMsgData;  //回复消息
	memset(&InfoMsgData, 0, sizeof(InfoMsgData));

	switch (gTotalState[nTtlIndex].HplcQuery)
	{
		case HPLC_QUERY_IDLE://空闲执行,初始化
		{
			CCODOWN_FMT_DEBUG(nTtlIndex, "HplcInfoReport_Slave()查询从节点ID信息 HPLC_QUERY_IDLE...端口[%d] \n", nTtlIndex);
			ClearCommRecvBuff(nTtlIndex);  //清通讯接收缓冲区
			ReadAmrInfo(nTtlIndex);  //加载档案信息至抄表信息
			CurNodeIndex[nTtlIndex] = 1;//2013版当前索引从1开始
			CommBeginTime[nTtlIndex] = time(NULL);
			OverTimeCnt[nTtlIndex] = 30;
			
			gTotalState[nTtlIndex].HplcQuery = HPLC_QUERY_SEND;  //进入组帧处理
			break;
		}
		case HPLC_QUERY_SEND: //发送
		{
			CCODOWN_FMT_DEBUG(nTtlIndex, "HplcInfoReport_Slave()查询从节点ID信息 HPLC_QUERY_SEND...端口[%d] \n", nTtlIndex);

			//发送报文组帧
			SendAppData.AFN = 0x10;
		    SendAppData.Fn = 7;
		    SendAppData.DataUnitLen = 3;  //节点起始序号2字节, 节点数量1字节
			memcpy(SendAppData.DataUnitBuff, &CurNodeIndex[nTtlIndex], 2);  //从节点起始序号，2字节
			SendAppData.DataUnitBuff[2] = 10;  //每次查询节点数量，1字节
			CCODOWN_FMT_DEBUG(nTtlIndex, "发送报文SendAppData 节点起始序号=%d, 节点数量=%d \n", CurNodeIndex[nTtlIndex], SendAppData.DataUnitBuff[2]);
			
			ret = CommFunDeal(SendAppData, &RecvAppData, nTtlIndex);
			if(ret > 0)
			{
				//接收报文解帧
				memcpy(&TotalCount, &RecvAppData.DataUnitBuff[0], 2);  //节点总数量
				ReplyCount = RecvAppData.DataUnitBuff[2];  //本次应答的数量
				CCODOWN_FMT_DEBUG(nTtlIndex, "接收报文RecvAppData 节点总数量=%d, 本次应答节点数量=%d \n", 
					TotalCount, ReplyCount);

				if(ReplyCount > 0)
				{
					//组装消息报文
					InfoMsgData.IP  = inet_addr(gCtsRouterAppIP[nTtlIndex]);
					InfoMsgData.IP  = ntohl(InfoMsgData.IP);
					InfoMsgData.port = gCtsRouterAppPort[nTtlIndex];
					InfoMsgData.MsgPRIORITY = 0;
					InfoMsgData.MsgRPM = 1;
					InfoMsgData.MsgIndex = GetInfoIndex();
					InfoMsgData.MsgLabel = nTtlIndex;  //mqtt存放端口序号(0或1)
					InfoMsgData.SourLen = strlen(gCcoRouterAppName);
					memset(InfoMsgData.SourAddr, 0x00, InfoMsgData.SourLen);
					memcpy(InfoMsgData.SourAddr, gCcoRouterAppName, InfoMsgData.SourLen);
					InfoMsgData.DestLen = strlen(gCtsRouterAppName[nTtlIndex]);
					memset(InfoMsgData.DestAddr, 0x00, InfoMsgData.DestLen);
					memcpy(InfoMsgData.DestAddr, gCtsRouterAppName[nTtlIndex], InfoMsgData.DestLen);
					
					CurInfoIndex = 0;

					InfoMsgData.IID_ID = 0x0003;
					InfoMsgData.IOP_ID = 0x0051;
					
					Uint32ToCharArr(gPortID[nTtlIndex], &InfoMsgData.aPayloadData[CurInfoIndex]);  //端口号
					CCODOWN_FMT_DEBUG(nTtlIndex, "端口号: %02x %02x %02x %02x \n", 
						InfoMsgData.aPayloadData[CurInfoIndex], InfoMsgData.aPayloadData[CurInfoIndex+1], 
						InfoMsgData.aPayloadData[CurInfoIndex+2], InfoMsgData.aPayloadData[CurInfoIndex+3]);
					
					CurInfoIndex += 4;
					InfoMsgData.aPayloadData[CurInfoIndex] = 0x01;  //是否含有起始序号信息, 0x00:无, 0x01:有
					CCODOWN_FMT_DEBUG(nTtlIndex, "是否含有起始序号信息: %02x (0x00:无, 0x01:有) \n", InfoMsgData.aPayloadData[CurInfoIndex]);
					
					CurInfoIndex += 1;
					memcpy_r(&InfoMsgData.aPayloadData[CurInfoIndex], (uint8 *)&CurNodeIndex[nTtlIndex], 2);  //起始序号2字节
					CCODOWN_FMT_DEBUG(nTtlIndex, "起始序号: %02x %02x (十进制 %d)\n", 
						InfoMsgData.aPayloadData[CurInfoIndex], InfoMsgData.aPayloadData[CurInfoIndex+1], CurNodeIndex[nTtlIndex]);
					
					CurInfoIndex += 2;
					InfoMsgData.aPayloadData[CurInfoIndex] = 0x07;  //查询HPLC信息choice标志
					CCODOWN_FMT_DEBUG(nTtlIndex, "查询HPLC信息choice标志: %02x \n", InfoMsgData.aPayloadData[CurInfoIndex]);
					
					CurInfoIndex += 1;
					ChangeLenToBuff(ReplyCount, &CurInfoIndex, &InfoMsgData.aPayloadData[CurInfoIndex]);  //信息个数seq of
					CCODOWN_FMT_DEBUG(nTtlIndex, "信息个数seq of: %d(十进制) \n", ReplyCount);
					
					CurFrameIndex = 3;  //收到的回复报文, 从节点总数量2字节, 本次应答数量1字节
					for(i=0; i<ReplyCount; i++)
					{
  						NodeIndexTemp = i + CurNodeIndex[nTtlIndex];
  						Uint16ToCharArr(NodeIndexTemp, &InfoMsgData.aPayloadData[CurInfoIndex]);  //节点序号2字节
  						CCODOWN_FMT_DEBUG(nTtlIndex, "i=%d, 从节点序号: %02x %02x (十进制 %d) \n", i, 
							InfoMsgData.aPayloadData[CurInfoIndex], InfoMsgData.aPayloadData[CurInfoIndex+1], NodeIndexTemp);
						
						CurInfoIndex += 2;
						memcpy_r(&InfoMsgData.aPayloadData[CurInfoIndex], &RecvAppData.DataUnitBuff[CurFrameIndex], 6);  //节点地址
						CCODOWN_FMT_DEBUG(nTtlIndex, "从节点地址: %02x %02x %02x %02x %02x %02x \n", 
							InfoMsgData.aPayloadData[CurInfoIndex], InfoMsgData.aPayloadData[CurInfoIndex+1], 
							InfoMsgData.aPayloadData[CurInfoIndex+2], InfoMsgData.aPayloadData[CurInfoIndex+3], 
							InfoMsgData.aPayloadData[CurInfoIndex+4], InfoMsgData.aPayloadData[CurInfoIndex+5]);
						
						CurInfoIndex += 6;
						CurFrameIndex += 6;
						InfoMsgData.aPayloadData[CurInfoIndex] = RecvAppData.DataUnitBuff[CurFrameIndex];  //节点设备类型
						CCODOWN_FMT_DEBUG(nTtlIndex, "从节点设备类型: %02x (十进制 %d) \n", 
						InfoMsgData.aPayloadData[CurInfoIndex], InfoMsgData.aPayloadData[CurInfoIndex]);
						
						CurInfoIndex += 1;
						CurFrameIndex += 1;
						memcpy_r(&InfoMsgData.aPayloadData[CurInfoIndex], &RecvAppData.DataUnitBuff[CurFrameIndex], 2);  //模块厂商代码
						CCODOWN_FMT_DEBUG(nTtlIndex, "从节点模块厂商代码: %02x %02x \n", 
							InfoMsgData.aPayloadData[CurInfoIndex], InfoMsgData.aPayloadData[CurInfoIndex+1]);
						
						CurInfoIndex += 2;
						CurFrameIndex += 2;
						LenTemp = RecvAppData.DataUnitBuff[CurFrameIndex];
						InfoMsgData.aPayloadData[CurInfoIndex] = RecvAppData.DataUnitBuff[CurFrameIndex];  //模块ID长度
						CCODOWN_FMT_DEBUG(nTtlIndex, "从节点模块ID长度: %02x (十进制 %d) \n", 
							InfoMsgData.aPayloadData[CurInfoIndex], InfoMsgData.aPayloadData[CurInfoIndex]);
						
						CurInfoIndex += 1;
						CurFrameIndex += 1;
						InfoMsgData.aPayloadData[CurInfoIndex] = RecvAppData.DataUnitBuff[CurFrameIndex];  //模块ID格式
						CCODOWN_FMT_DEBUG(nTtlIndex, "从节点模块ID格式: %02x (十进制 %d) \n", 
							InfoMsgData.aPayloadData[CurInfoIndex], InfoMsgData.aPayloadData[CurInfoIndex]);
						
						CurInfoIndex += 1;
						CurFrameIndex += 1;
						ChangeLenToBuff(LenTemp, &CurInfoIndex, &InfoMsgData.aPayloadData[CurInfoIndex]);  //visible-string 长度
						CCODOWN_FMT_DEBUG(nTtlIndex, "从节点模块ID visible-string 长度: %d(十进制) \n", LenTemp);
						
						memcpy_r(&InfoMsgData.aPayloadData[CurInfoIndex], &RecvAppData.DataUnitBuff[CurFrameIndex], LenTemp);  //模块ID
						CCODOWN_BUF_DEBUG(nTtlIndex, &InfoMsgData.aPayloadData[CurInfoIndex], LenTemp, "从节点模块ID:");
						
						CurInfoIndex += LenTemp;
						CurFrameIndex += LenTemp;
					}
					
					InfoMsgData.nPayloadLength = CurInfoIndex;
					PlcWriteQueue(pAmrSendUp, (uint8 *)&InfoMsgData, sizeof(MSG_INFO_T)+InfoMsgData.nPayloadLength);
				}

				CurNodeIndex[nTtlIndex] += ReplyCount;  //当前序号累加
				if(CurNodeIndex[nTtlIndex] > TotalCount)
				{
					gTotalState[nTtlIndex].HplcQuery = HPLC_QUERY_END;//任务完成
					break;
				}
			}

			gTotalState[nTtlIndex].HplcQuery = HPLC_QUERY_RECV;  //判断是否超时
			break;
		}
		
		case HPLC_QUERY_RECV:  //接收
		{
			CCODOWN_FMT_DEBUG(nTtlIndex, "HplcInfoReport_Slave()查询从节点ID信息 HPLC_QUERY_RECV...端口[%d] \n", nTtlIndex);
			if ( abs(time(NULL) - CommBeginTime[nTtlIndex]) > OverTimeCnt[nTtlIndex])
			{
				CCODOWN_FMT_DEBUG(nTtlIndex, "if ((time(NULL) - CommBeginTime) > OverTimeCnt) 超时...\n");
				gTotalState[nTtlIndex].HplcQuery = HPLC_QUERY_END;  //累计超时, 任务完成
			}
			else
			{
				gTotalState[nTtlIndex].HplcQuery = HPLC_QUERY_SEND;  //进入组帧处理
			}
			
			break;
		}

		case HPLC_QUERY_RETRY:  //重试
		{
			CCODOWN_FMT_DEBUG(nTtlIndex, "HplcInfoReport_Slave()查询从节点ID信息 HPLC_QUERY_RETRY...端口[%d] \n", nTtlIndex);
			gTotalState[nTtlIndex].HplcQuery = HPLC_QUERY_END;  //达到最大重试次数, 任务完成
			break;
		}

		case HPLC_QUERY_END:  //结束
		{
			CCODOWN_FMT_DEBUG(nTtlIndex, "HplcInfoReport_Slave()查询从节点ID信息 HPLC_QUERY_END...端口[%d] \n", nTtlIndex);
			gTotalState[nTtlIndex].HplcQuery = HPLC_QUERY_IDLE;
			gQueryTypeHPLC[nTtlIndex].BitType.slave = 0;
			return 1;
			break;
		}

		default:
			break;
	}

	return -1;
}


/*******************************************************************************
* 函数名称: HplcInfoReport_Master
* 函数功能: HPLC查询信息上报 本地主节点模块ID信息
* 输入参数: 无
* 输出参数: 无
* 返 回 值: >0                  成功
*           <0                  失败
*******************************************************************************/
int HplcInfoReport_Master(TASK_FUN_INPUT stTaskFunInput)
{
	int ret = 0;
	static  time_t  CommBeginTime[MAX_DOWN_THREAD_NUM];  //通讯开始时间
	static  uint32  OverTimeCnt[MAX_DOWN_THREAD_NUM] = {0};  //超时时间
	uint16  CurInfoIndex = 0;  //消息组帧偏移位
	uint8   LenTemp = 0;  //模块ID号长度, 临时变量

	AppData_T   SendAppData;                        //发送
    AppData_T   RecvAppData;                        //接收
    memset(&SendAppData, 0x00, sizeof(AppData_T));
	memset(&RecvAppData, 0x00, sizeof(AppData_T));
	
	uint8           nTtlIndex = 0;  //不同的端口号对应不同的数组下标
	nTtlIndex = stTaskFunInput.TaskArrIndex;

	INFO_DATA_T     InfoMsgData;  //回复消息
	memset(&InfoMsgData, 0, sizeof(InfoMsgData));

	switch (gTotalState[nTtlIndex].HplcQuery)
	{
		case HPLC_QUERY_IDLE://空闲执行,初始化
		{
			CCODOWN_FMT_DEBUG(nTtlIndex, "HplcInfoReport_Master()查询本地主节点 HPLC_QUERY_IDLE...端口[%d] \n", nTtlIndex);
			ClearCommRecvBuff(nTtlIndex);  //清通讯接收缓冲区
			ReadAmrInfo(nTtlIndex);  //加载档案信息至抄表信息
			CommBeginTime[nTtlIndex] = time(NULL);
			OverTimeCnt[nTtlIndex] = 30;
			
			gTotalState[nTtlIndex].HplcQuery = HPLC_QUERY_SEND;  //进入组帧处理
			break;
		}
		case HPLC_QUERY_SEND: //发送
		{
			CCODOWN_FMT_DEBUG(nTtlIndex, "HplcInfoReport_Master()查询本地主节点 HPLC_QUERY_SEND...端口[%d] \n", nTtlIndex);

			SendAppData.AFN = 0x03;
		    SendAppData.Fn = 12;
		    SendAppData.DataUnitLen = 0;  //无数据单元
			
			ret = CommFunDeal(SendAppData, &RecvAppData, nTtlIndex);
			if(ret > 0)
			{
				//组装消息报文
				InfoMsgData.IP  = inet_addr(gCtsRouterAppIP[nTtlIndex]);
				InfoMsgData.IP  = ntohl(InfoMsgData.IP);
				InfoMsgData.port = gCtsRouterAppPort[nTtlIndex];
				InfoMsgData.MsgPRIORITY = 0;
				InfoMsgData.MsgRPM = 1;
				InfoMsgData.MsgIndex = GetInfoIndex();
				InfoMsgData.MsgLabel = nTtlIndex;  //mqtt存放端口序号(0或1)
				InfoMsgData.SourLen = strlen(gCcoRouterAppName);
				memset(InfoMsgData.SourAddr, 0x00, InfoMsgData.SourLen);
				memcpy(InfoMsgData.SourAddr, gCcoRouterAppName, InfoMsgData.SourLen);
				InfoMsgData.DestLen = strlen(gCtsRouterAppName[nTtlIndex]);
				memset(InfoMsgData.DestAddr, 0x00, InfoMsgData.DestLen);
				memcpy(InfoMsgData.DestAddr, gCtsRouterAppName[nTtlIndex], InfoMsgData.DestLen);
				
				CurInfoIndex = 0;

				InfoMsgData.IID_ID = 0x0003;
				InfoMsgData.IOP_ID = 0x0051;
				
				Uint32ToCharArr(gPortID[nTtlIndex], &InfoMsgData.aPayloadData[CurInfoIndex]);  //端口号
				CCODOWN_FMT_DEBUG(nTtlIndex, "端口号: %02x %02x %02x %02x \n", 
						InfoMsgData.aPayloadData[CurInfoIndex], InfoMsgData.aPayloadData[CurInfoIndex+1], 
						InfoMsgData.aPayloadData[CurInfoIndex+2], InfoMsgData.aPayloadData[CurInfoIndex+3]);
				
				CurInfoIndex += 4;
				InfoMsgData.aPayloadData[CurInfoIndex] = 0x00;  //是否含有起始序号信息, 0x00:无, 0x01:有
				CCODOWN_FMT_DEBUG(nTtlIndex, "是否含有起始序号信息: %02x (0x00:无, 0x01:有) \n", InfoMsgData.aPayloadData[CurInfoIndex]);
				
				CurInfoIndex += 1;
				InfoMsgData.aPayloadData[CurInfoIndex] = 0x06;  //查询HPLC信息choice标志
				CCODOWN_FMT_DEBUG(nTtlIndex, "查询HPLC信息choice标志: %02x \n", InfoMsgData.aPayloadData[CurInfoIndex]);
				
				CurInfoIndex += 1;
				//ChangeLenToBuff(1, &CurInfoIndex, &InfoMsgData.aPayloadData[CurInfoIndex]);  //信息个数seq of
				memcpy_r(&InfoMsgData.aPayloadData[CurInfoIndex], &RecvAppData.DataUnitBuff[0], 2);  //模块厂商代码
				CCODOWN_FMT_DEBUG(nTtlIndex, "模块厂商代码: %02x %02x \n", 
						InfoMsgData.aPayloadData[CurInfoIndex], InfoMsgData.aPayloadData[CurInfoIndex+1]);
				
				CurInfoIndex += 2;
				InfoMsgData.aPayloadData[CurInfoIndex] = RecvAppData.DataUnitBuff[2];  //模块ID长度
				LenTemp = RecvAppData.DataUnitBuff[2];
				CCODOWN_FMT_DEBUG(nTtlIndex, "模块ID长度: %02x (十进制 %d) \n", 
						InfoMsgData.aPayloadData[CurInfoIndex], InfoMsgData.aPayloadData[CurInfoIndex]);
				
				CurInfoIndex += 1;
				InfoMsgData.aPayloadData[CurInfoIndex] = RecvAppData.DataUnitBuff[3];  //模块ID格式
				CCODOWN_FMT_DEBUG(nTtlIndex, "模块ID格式: %02x (十进制 %d) \n", 
						InfoMsgData.aPayloadData[CurInfoIndex], InfoMsgData.aPayloadData[CurInfoIndex]);
				
				CurInfoIndex += 1;
				ChangeLenToBuff(LenTemp, &CurInfoIndex, &InfoMsgData.aPayloadData[CurInfoIndex]);  //visible-string 长度
				CCODOWN_FMT_DEBUG(nTtlIndex, "模块ID visible-string 长度: %d(十进制) \n", LenTemp);
				
				memcpy_r(&InfoMsgData.aPayloadData[CurInfoIndex], &RecvAppData.DataUnitBuff[4], RecvAppData.DataUnitBuff[2]);  //模块ID
				CCODOWN_BUF_DEBUG(nTtlIndex, &InfoMsgData.aPayloadData[CurInfoIndex], RecvAppData.DataUnitBuff[2], "模块ID:");
				
				CurInfoIndex += RecvAppData.DataUnitBuff[2];

				InfoMsgData.nPayloadLength = CurInfoIndex;
				PlcWriteQueue(pAmrSendUp, (uint8 *)&InfoMsgData, sizeof(MSG_INFO_T)+InfoMsgData.nPayloadLength);

				gTotalState[nTtlIndex].HplcQuery = HPLC_QUERY_END;//任务完成
				break;
			}

			gTotalState[nTtlIndex].HplcQuery = HPLC_QUERY_RECV;
			break;
		}
		
		case HPLC_QUERY_RECV:  //接收
		{
			CCODOWN_FMT_DEBUG(nTtlIndex, "HplcInfoReport_Master()查询本地主节点 HPLC_QUERY_RECV...端口[%d] \n", nTtlIndex);
			if ( abs(time(NULL) - CommBeginTime[nTtlIndex]) > OverTimeCnt[nTtlIndex])
			{
				CCODOWN_FMT_DEBUG(nTtlIndex, "if ((time(NULL) - CommBeginTime) > OverTimeCnt) 超时...\n");
				gTotalState[nTtlIndex].HplcQuery = HPLC_QUERY_END;  //累计超时, 任务完成
			}
			else
			{
				gTotalState[nTtlIndex].HplcQuery = HPLC_QUERY_SEND;  //进入组帧处理
			}
			
			break;
		}

		case HPLC_QUERY_RETRY:  //重试
		{
			CCODOWN_FMT_DEBUG(nTtlIndex, "HplcInfoReport_Master()查询本地主节点 HPLC_QUERY_RETRY...端口[%d] \n", nTtlIndex);
			gTotalState[nTtlIndex].HplcQuery = HPLC_QUERY_END;  //达到最大重试次数, 任务完成
			break;
		}

		case HPLC_QUERY_END:  //结束
		{
			CCODOWN_FMT_DEBUG(nTtlIndex, "HplcInfoReport_Master()查询本地主节点 HPLC_QUERY_END...端口[%d] \n", nTtlIndex);
			gTotalState[nTtlIndex].HplcQuery = HPLC_QUERY_IDLE;
			gQueryTypeHPLC[nTtlIndex].BitType.master = 0;
			return 1;
			break;
		}

		default:
			break;
	}

	return -1;
}


/*******************************************************************************
* 函数名称: HplcInfoReport_Scale
* 函数功能: HPLC查询信息上报 网络规模
* 输入参数: 无
* 输出参数: 无
* 返 回 值: >0                  成功
*           <0                  失败
*******************************************************************************/
int HplcInfoReport_Scale(TASK_FUN_INPUT stTaskFunInput)
{
	//int i   = 0;
	int ret = 0;
	static  time_t  CommBeginTime[MAX_DOWN_THREAD_NUM];  //通讯开始时间
	static  uint32  OverTimeCnt[MAX_DOWN_THREAD_NUM] = {0};  //超时时间
	uint16  CurInfoIndex = 0;  //消息组帧偏移位
	uint16  TotalCount = 0;  //节点总数量
	uint16   MaxAllowedCount = 0;  //路由支持最大从节点数量

	AppData_T   SendAppData;                        //发送
    AppData_T   RecvAppData;                        //接收
    memset(&SendAppData, 0x00, sizeof(AppData_T));
	memset(&RecvAppData, 0x00, sizeof(AppData_T));
	
	uint8           nTtlIndex = 0;  //不同的端口号对应不同的数组下标
	nTtlIndex = stTaskFunInput.TaskArrIndex;

	CCODOWN_FMT_DEBUG(nTtlIndex, "HplcInfoReport_Scale() &SendAppData=%p, &RecvAppData=%p, &nTtlIndex=%p \n", 
		&SendAppData, &RecvAppData, &nTtlIndex);
	CCODOWN_FMT_DEBUG(nTtlIndex, "HplcInfoReport_Scale() stTaskFunInput.TaskArrIndex:%d, nTtlIndex:%d \n", 
		stTaskFunInput.TaskArrIndex, nTtlIndex);

	INFO_DATA_T     InfoMsgData;  //回复消息
	memset(&InfoMsgData, 0, sizeof(InfoMsgData));

	switch (gTotalState[nTtlIndex].HplcQuery)
	{
		case HPLC_QUERY_IDLE://空闲执行,初始化
		{
			CCODOWN_FMT_DEBUG(nTtlIndex, "HplcInfoReport_Scale()查询网络规模 HPLC_QUERY_IDLE...端口[%d] \n", nTtlIndex);
			ClearCommRecvBuff(nTtlIndex);  //清通讯接收缓冲区
			ReadAmrInfo(nTtlIndex);  //加载档案信息至抄表信息
			CommBeginTime[nTtlIndex] = time(NULL);
			OverTimeCnt[nTtlIndex] = 30;
			
			gTotalState[nTtlIndex].HplcQuery = HPLC_QUERY_SEND;  //进入组帧处理
			break;
		}
		case HPLC_QUERY_SEND: //发送
		{
			CCODOWN_FMT_DEBUG(nTtlIndex, "HplcInfoReport_Scale()查询网络规模 HPLC_QUERY_SEND...端口[%d] \n", nTtlIndex);

			SendAppData.AFN = 0x10;
		    SendAppData.Fn = 1;
		    SendAppData.DataUnitLen = 0;  //无数据单元
			ret = CommFunDeal(SendAppData, &RecvAppData, nTtlIndex);
			if(ret > 0)
			{
				memcpy(&TotalCount, &RecvAppData.DataUnitBuff[0], 2);  //节点总数量
				memcpy(&MaxAllowedCount, &RecvAppData.DataUnitBuff[2], 2);  //路由支持最大从节点数量
				CCODOWN_FMT_DEBUG(nTtlIndex, "HplcInfoReport_Scale() 节点总数量=%d, 路由支持最大从节点数量=%d, 最大抄表边界个数=%d \n", TotalCount, MaxAllowedCount, MAX_AMR_COLLECT_CNT);
				if(TotalCount > MAX_AMR_COLLECT_CNT)
				{
					TotalCount = MAX_AMR_COLLECT_CNT;
				}

				CCODOWN_FMT_DEBUG(nTtlIndex, "HplcInfoReport_Scale() stTaskFunInput.TaskArrIndex:%d, nTtlIndex:%d \n", 
					stTaskFunInput.TaskArrIndex, nTtlIndex);
				//组装消息报文
				InfoMsgData.IP  = inet_addr(gCtsRouterAppIP[nTtlIndex]);
				InfoMsgData.IP  = ntohl(InfoMsgData.IP);
				InfoMsgData.port = gCtsRouterAppPort[nTtlIndex];
				InfoMsgData.MsgPRIORITY = 0;
				InfoMsgData.MsgRPM = 1;
				InfoMsgData.MsgIndex = GetInfoIndex();
				InfoMsgData.MsgLabel = nTtlIndex;  //mqtt存放端口序号(0或1)
				InfoMsgData.SourLen = strlen(gCcoRouterAppName);
				memset(InfoMsgData.SourAddr, 0x00, InfoMsgData.SourLen);
				memcpy(InfoMsgData.SourAddr, gCcoRouterAppName, InfoMsgData.SourLen);
				InfoMsgData.DestLen = strlen(gCtsRouterAppName[nTtlIndex]);
				memset(InfoMsgData.DestAddr, 0x00, InfoMsgData.DestLen);
				memcpy(InfoMsgData.DestAddr, gCtsRouterAppName[nTtlIndex], InfoMsgData.DestLen);
				
				CurInfoIndex = 0;

				InfoMsgData.IID_ID = 0x0003;
				InfoMsgData.IOP_ID = 0x0051;
				
				Uint32ToCharArr(gPortID[nTtlIndex], &InfoMsgData.aPayloadData[CurInfoIndex]);  //端口号
				CCODOWN_FMT_DEBUG(nTtlIndex, "端口号: %02x %02x %02x %02x \n", 
						InfoMsgData.aPayloadData[CurInfoIndex], InfoMsgData.aPayloadData[CurInfoIndex+1], 
						InfoMsgData.aPayloadData[CurInfoIndex+2], InfoMsgData.aPayloadData[CurInfoIndex+3]);
				
				CurInfoIndex += 4;
				InfoMsgData.aPayloadData[CurInfoIndex] = 0x00;  //是否含有起始序号信息, 0x00:无, 0x01:有
				CCODOWN_FMT_DEBUG(nTtlIndex, "是否含有起始序号信息: %02x (0x00:无, 0x01:有) \n", InfoMsgData.aPayloadData[CurInfoIndex]);
				
				CurInfoIndex += 1;
				InfoMsgData.aPayloadData[CurInfoIndex] = 0x05;  //查询HPLC信息choice标志
				CCODOWN_FMT_DEBUG(nTtlIndex, "查询HPLC信息choice标志: %02x \n", InfoMsgData.aPayloadData[CurInfoIndex]);
				
				CurInfoIndex += 1;
				Uint16ToCharArr(TotalCount, &InfoMsgData.aPayloadData[CurInfoIndex]);  //从节点总数量
				CCODOWN_FMT_DEBUG(nTtlIndex, "从节点总数量(网络规模): %02x %02x (十进制 %d)\n", 
						InfoMsgData.aPayloadData[CurInfoIndex], InfoMsgData.aPayloadData[CurInfoIndex+1], TotalCount);
				
				CurInfoIndex += 2;

				InfoMsgData.nPayloadLength = CurInfoIndex;
				PlcWriteQueue(pAmrSendUp, (uint8 *)&InfoMsgData, sizeof(MSG_INFO_T)+InfoMsgData.nPayloadLength);

				gTotalState[nTtlIndex].HplcQuery = HPLC_QUERY_END;//任务完成
				break;
			}

			gTotalState[nTtlIndex].HplcQuery = HPLC_QUERY_RECV;
			break;
		}
		
		case HPLC_QUERY_RECV:  //接收
		{
			CCODOWN_FMT_DEBUG(nTtlIndex, "HplcInfoReport_Scale()查询网络规模 HPLC_QUERY_RECV...端口[%d] \n", nTtlIndex);
			if ( abs(time(NULL) - CommBeginTime[nTtlIndex]) > OverTimeCnt[nTtlIndex])
			{
				CCODOWN_FMT_DEBUG(nTtlIndex, "if ((time(NULL) - CommBeginTime) > OverTimeCnt) 超时...\n");
				gTotalState[nTtlIndex].HplcQuery = HPLC_QUERY_END;  //累计超时, 任务完成
			}
			else
			{
				gTotalState[nTtlIndex].HplcQuery = HPLC_QUERY_SEND;  //进入组帧处理
			}
			
			break;
		}

		case HPLC_QUERY_RETRY:  //重试
		{
			CCODOWN_FMT_DEBUG(nTtlIndex, "HplcInfoReport_Scale()查询网络规模 HPLC_QUERY_RETRY...端口[%d] \n", nTtlIndex);
			gTotalState[nTtlIndex].HplcQuery = HPLC_QUERY_END;  //达到最大重试次数, 任务完成
			break;
		}

		case HPLC_QUERY_END:  //结束
		{
			CCODOWN_FMT_DEBUG(nTtlIndex, "HplcInfoReport_Scale()查询网络规模 HPLC_QUERY_END...端口[%d] \n", nTtlIndex);
			gTotalState[nTtlIndex].HplcQuery = HPLC_QUERY_IDLE;
			gQueryTypeHPLC[nTtlIndex].BitType.scale = 0;
			return 1;
			break;
		}

		default:
			break;
	}

	return -1;
}


/*******************************************************************************
* 函数名称: HplcInfoReport_Phase
* 函数功能: HPLC查询信息上报 节点相线信息
* 输入参数: 无
* 输出参数: 无
* 返 回 值: >0                  成功
*           <0                  失败
*******************************************************************************/
int HplcInfoReport_Phase(TASK_FUN_INPUT stTaskFunInput)
{
	int i   = 0;
	int ret = 0;
	static  time_t  CommBeginTime[MAX_DOWN_THREAD_NUM];  //通讯开始时间
	static  uint32  OverTimeCnt[MAX_DOWN_THREAD_NUM] = {0};  //超时时间
	static  uint16  CurNodeIndex[MAX_DOWN_THREAD_NUM] = {0};  //2013版当前索引从1开始
	uint16  CurInfoIndex = 0;  //消息组帧偏移位
	uint16  TotalCount = 0;  //节点总数量
	uint8   ReplyCount = 0;  //本次应答的节点数量
	uint16  ReplyNodeIndex = 0;  //本次应答的起始序号
	uint16  NodeIndexTemp = 0;  //节点序号, 临时变量

	AppData_T   SendAppData;                        //发送
    AppData_T   RecvAppData;                        //接收
    memset(&SendAppData, 0x00, sizeof(AppData_T));
	memset(&RecvAppData, 0x00, sizeof(AppData_T));
	
	uint8           nTtlIndex = 0;  //不同的端口号对应不同的数组下标
	nTtlIndex = stTaskFunInput.TaskArrIndex;

	INFO_DATA_T     InfoMsgData;  //回复消息
	memset(&InfoMsgData, 0, sizeof(InfoMsgData));

	switch (gTotalState[nTtlIndex].HplcQuery)
	{
		case HPLC_QUERY_IDLE://空闲执行,初始化
		{
			CCODOWN_FMT_DEBUG(nTtlIndex, "HplcInfoReport_Phase()查询节点相线信息 HPLC_QUERY_IDLE...端口[%d] \n", nTtlIndex);
			ClearCommRecvBuff(nTtlIndex);  //清通讯接收缓冲区
			ReadAmrInfo(nTtlIndex);  //加载档案信息至抄表信息
			CurNodeIndex[nTtlIndex] = 1;//2013版当前索引从1开始
			CommBeginTime[nTtlIndex] = time(NULL);
			OverTimeCnt[nTtlIndex] = 30;
			
			gTotalState[nTtlIndex].HplcQuery = HPLC_QUERY_SEND;  //进入组帧处理
			break;
		}
		case HPLC_QUERY_SEND: //发送
		{
			CCODOWN_FMT_DEBUG(nTtlIndex, "HplcInfoReport_Phase()查询节点相线信息 HPLC_QUERY_SEND...端口[%d] \n", nTtlIndex);

			//发送报文组帧
			SendAppData.AFN = 0x10;
		    SendAppData.Fn = 31;
		    SendAppData.DataUnitLen = 3;  //节点起始序号2字节, 节点数量1字节
			memcpy(SendAppData.DataUnitBuff, &CurNodeIndex[nTtlIndex], 2);  //从节点起始序号，2字节
			SendAppData.DataUnitBuff[2] = 10;  //每次查询节点数量，1字节
			CCODOWN_FMT_DEBUG(nTtlIndex, "发送报文SendAppData 节点起始序号=%d, 节点数量=%d \n", CurNodeIndex[nTtlIndex], SendAppData.DataUnitBuff[2]);
			
			ret = CommFunDeal(SendAppData, &RecvAppData, nTtlIndex);
			if(ret > 0)
			{
				//接收报文解帧
				memcpy(&TotalCount, &RecvAppData.DataUnitBuff[0], 2);  //节点总数量
				memcpy(&ReplyNodeIndex, &RecvAppData.DataUnitBuff[2], 2);  //节点起始序号
				ReplyCount = RecvAppData.DataUnitBuff[4];  //本次应答的数量
				CCODOWN_FMT_DEBUG(nTtlIndex, "接收报文RecvAppData 节点总数量=%d, 节点起始序号=%d, 本次应答节点数量=%d \n", 
					TotalCount, ReplyNodeIndex, ReplyCount);

				if(ReplyCount > 0)
				{
					//组装消息报文
					InfoMsgData.IP  = inet_addr(gCtsRouterAppIP[nTtlIndex]);
					InfoMsgData.IP  = ntohl(InfoMsgData.IP);
					InfoMsgData.port = gCtsRouterAppPort[nTtlIndex];
					InfoMsgData.MsgPRIORITY = 0;
					InfoMsgData.MsgRPM = 1;
					InfoMsgData.MsgIndex = GetInfoIndex();
					InfoMsgData.MsgLabel = nTtlIndex;  //mqtt存放端口序号(0或1)
					InfoMsgData.SourLen = strlen(gCcoRouterAppName);
					memset(InfoMsgData.SourAddr, 0x00, InfoMsgData.SourLen);
					memcpy(InfoMsgData.SourAddr, gCcoRouterAppName, InfoMsgData.SourLen);
					InfoMsgData.DestLen = strlen(gCtsRouterAppName[nTtlIndex]);
					memset(InfoMsgData.DestAddr, 0x00, InfoMsgData.DestLen);
					memcpy(InfoMsgData.DestAddr, gCtsRouterAppName[nTtlIndex], InfoMsgData.DestLen);
					
					CurInfoIndex = 0;

					InfoMsgData.IID_ID = 0x0003;
					InfoMsgData.IOP_ID = 0x0051;
					
					Uint32ToCharArr(gPortID[nTtlIndex], &InfoMsgData.aPayloadData[CurInfoIndex]);  //端口号
					CCODOWN_FMT_DEBUG(nTtlIndex, "端口号: %02x %02x %02x %02x \n", 
						InfoMsgData.aPayloadData[CurInfoIndex], InfoMsgData.aPayloadData[CurInfoIndex+1], 
						InfoMsgData.aPayloadData[CurInfoIndex+2], InfoMsgData.aPayloadData[CurInfoIndex+3]);
					
					CurInfoIndex += 4;
					InfoMsgData.aPayloadData[CurInfoIndex] = 0x01;  //是否含有起始序号信息, 0x00:无, 0x01:有
					CCODOWN_FMT_DEBUG(nTtlIndex, "是否含有起始序号信息: %02x (0x00:无, 0x01:有) \n", InfoMsgData.aPayloadData[CurInfoIndex]);
					
					CurInfoIndex += 1;
					memcpy_r(&InfoMsgData.aPayloadData[CurInfoIndex], (uint8 *)&ReplyNodeIndex, 2);  //起始序号2字节
					CCODOWN_FMT_DEBUG(nTtlIndex, "起始序号: %02x %02x (十进制 %d)\n", 
						InfoMsgData.aPayloadData[CurInfoIndex], InfoMsgData.aPayloadData[CurInfoIndex+1], ReplyNodeIndex);
					
					CurInfoIndex += 2;
					InfoMsgData.aPayloadData[CurInfoIndex] = 0x04;  //查询HPLC信息choice标志
					CCODOWN_FMT_DEBUG(nTtlIndex, "查询HPLC信息choice标志: %02x \n", InfoMsgData.aPayloadData[CurInfoIndex]);
					
					CurInfoIndex += 1;
					ChangeLenToBuff(ReplyCount, &CurInfoIndex, &InfoMsgData.aPayloadData[CurInfoIndex]);  //信息个数seq of
					CCODOWN_FMT_DEBUG(nTtlIndex, "信息个数seq of: %d(十进制) \n", ReplyCount);
					
					for(i=0; i<ReplyCount; i++)
					{
  						NodeIndexTemp = i + ReplyNodeIndex;
  						Uint16ToCharArr(NodeIndexTemp, &InfoMsgData.aPayloadData[CurInfoIndex]);  //节点序号2字节
  						CCODOWN_FMT_DEBUG(nTtlIndex, "i=%d, 节点序号: %02x %02x (十进制 %d) \n", i, 
							InfoMsgData.aPayloadData[CurInfoIndex], InfoMsgData.aPayloadData[CurInfoIndex+1], NodeIndexTemp);
						
						CurInfoIndex += 2;
						memcpy_r(&InfoMsgData.aPayloadData[CurInfoIndex], &RecvAppData.DataUnitBuff[5+i*8], 6);  //节点地址
						CCODOWN_FMT_DEBUG(nTtlIndex, "节点地址: %02x %02x %02x %02x %02x %02x \n", 
							InfoMsgData.aPayloadData[CurInfoIndex], InfoMsgData.aPayloadData[CurInfoIndex+1], 
							InfoMsgData.aPayloadData[CurInfoIndex+2], InfoMsgData.aPayloadData[CurInfoIndex+3], 
							InfoMsgData.aPayloadData[CurInfoIndex+4], InfoMsgData.aPayloadData[CurInfoIndex+5]);
						
						CurInfoIndex += 6;
						memcpy_r(&InfoMsgData.aPayloadData[CurInfoIndex], &RecvAppData.DataUnitBuff[5+i*8+6], 2);  //节点相位信息
						CCODOWN_FMT_DEBUG(nTtlIndex, "节点相位信息: %02x %02x \n", 
							InfoMsgData.aPayloadData[CurInfoIndex], InfoMsgData.aPayloadData[CurInfoIndex+1]);
						
						CurInfoIndex += 2;
					}
					InfoMsgData.nPayloadLength = CurInfoIndex;
					PlcWriteQueue(pAmrSendUp, (uint8 *)&InfoMsgData, sizeof(MSG_INFO_T)+InfoMsgData.nPayloadLength);
				}

				CurNodeIndex[nTtlIndex] += ReplyCount;  //当前序号累加
				if(CurNodeIndex[nTtlIndex] > TotalCount)
				{
					gTotalState[nTtlIndex].HplcQuery = HPLC_QUERY_END;//任务完成
					break;
				}
			}

			gTotalState[nTtlIndex].HplcQuery = HPLC_QUERY_RECV;  //判断是否超时
			break;
		}
		
		case HPLC_QUERY_RECV:  //接收
		{
			CCODOWN_FMT_DEBUG(nTtlIndex, "HplcInfoReport_Phase()查询节点相线信息 HPLC_QUERY_RECV...端口[%d] \n", nTtlIndex);
			if ( abs(time(NULL) - CommBeginTime[nTtlIndex]) > OverTimeCnt[nTtlIndex])
			{
				CCODOWN_FMT_DEBUG(nTtlIndex, "if ((time(NULL) - CommBeginTime) > OverTimeCnt) 超时...\n");
				gTotalState[nTtlIndex].HplcQuery = HPLC_QUERY_END;  //累计超时, 任务完成
			}
			else
			{
				gTotalState[nTtlIndex].HplcQuery = HPLC_QUERY_SEND;  //进入组帧处理
			}
			
			break;
		}

		case HPLC_QUERY_RETRY:  //重试
		{
			CCODOWN_FMT_DEBUG(nTtlIndex, "HplcInfoReport_Phase()查询节点相线信息 HPLC_QUERY_RETRY...端口[%d] \n", nTtlIndex);
			gTotalState[nTtlIndex].HplcQuery = HPLC_QUERY_END;  //达到最大重试次数, 任务完成
			break;
		}

		case HPLC_QUERY_END:  //结束
		{
			CCODOWN_FMT_DEBUG(nTtlIndex, "HplcInfoReport_Phase()查询节点相线信息 HPLC_QUERY_END...端口[%d] \n", nTtlIndex);
			gTotalState[nTtlIndex].HplcQuery = HPLC_QUERY_IDLE;
			gQueryTypeHPLC[nTtlIndex].BitType.phase = 0;
			return 1;
			break;
		}

		default:
			break;
	}

	return -1;
}


/*******************************************************************************
* 函数名称: HplcInfoReport_Nets
* 函数功能: HPLC查询信息上报 多网络信息
* 输入参数: 无
* 输出参数: 无
* 返 回 值: >0                  成功
*           <0                  失败
*******************************************************************************/
int HplcInfoReport_Nets(TASK_FUN_INPUT stTaskFunInput)
{
	int i   = 0;
	int ret = 0;
	static  time_t  CommBeginTime[MAX_DOWN_THREAD_NUM];  //通讯开始时间
	static  uint32  OverTimeCnt[MAX_DOWN_THREAD_NUM] = {0};  //超时时间
	uint16  CurInfoIndex = 0;  //消息组帧偏移位
	uint16  TotalCount = 0;  //节点总数量

	AppData_T   SendAppData;                        //发送
    AppData_T   RecvAppData;                        //接收
    memset(&SendAppData, 0x00, sizeof(AppData_T));
	memset(&RecvAppData, 0x00, sizeof(AppData_T));
	
	uint8           nTtlIndex = 0;  //不同的端口号对应不同的数组下标
	nTtlIndex = stTaskFunInput.TaskArrIndex;

	INFO_DATA_T     InfoMsgData;  //回复消息
	memset(&InfoMsgData, 0, sizeof(InfoMsgData));

	switch (gTotalState[nTtlIndex].HplcQuery)
	{
		case HPLC_QUERY_IDLE://空闲执行,初始化
		{
			CCODOWN_FMT_DEBUG(nTtlIndex, "HplcInfoReport_Nets()查询多网络信息 HPLC_QUERY_IDLE...端口[%d] \n", nTtlIndex);
			ClearCommRecvBuff(nTtlIndex);  //清通讯接收缓冲区
			ReadAmrInfo(nTtlIndex);  //加载档案信息至抄表信息
			CommBeginTime[nTtlIndex] = time(NULL);
			OverTimeCnt[nTtlIndex] = 30;
			
			gTotalState[nTtlIndex].HplcQuery = HPLC_QUERY_SEND;  //进入组帧处理
			break;
		}
		case HPLC_QUERY_SEND: //发送
		{
			CCODOWN_FMT_DEBUG(nTtlIndex, "HplcInfoReport_Nets()查询多网络信息 HPLC_QUERY_SEND...端口[%d] \n", nTtlIndex);

			//发送报文组帧
			SendAppData.AFN = 0x10;
		    SendAppData.Fn = 111;
		    SendAppData.DataUnitLen = 0;  //无数据单元
			
			ret = CommFunDeal(SendAppData, &RecvAppData, nTtlIndex);
			if(ret > 0)
			{
				//接收报文解帧
				TotalCount = RecvAppData.DataUnitBuff[0];  //多网络节点总数量
				CCODOWN_FMT_DEBUG(nTtlIndex, "接收报文RecvAppData 节点总数量=%d, 本节点网络标识号=%02x%02x%02x, 本节点主节点地址=%02x%02x%02x%02x%02x%02x \n", 
							TotalCount, RecvAppData.DataUnitBuff[3], RecvAppData.DataUnitBuff[2], RecvAppData.DataUnitBuff[1], 
							RecvAppData.DataUnitBuff[9], RecvAppData.DataUnitBuff[8], RecvAppData.DataUnitBuff[7], 
							RecvAppData.DataUnitBuff[6], RecvAppData.DataUnitBuff[5], RecvAppData.DataUnitBuff[4]);

				//组装消息报文
				InfoMsgData.IP  = inet_addr(gCtsRouterAppIP[nTtlIndex]);
				InfoMsgData.IP  = ntohl(InfoMsgData.IP);
				InfoMsgData.port = gCtsRouterAppPort[nTtlIndex];
				InfoMsgData.MsgPRIORITY = 0;
				InfoMsgData.MsgRPM = 1;
				InfoMsgData.MsgIndex = GetInfoIndex();
				InfoMsgData.MsgLabel = nTtlIndex;  //mqtt存放端口序号(0或1)
				InfoMsgData.SourLen = strlen(gCcoRouterAppName);
				memset(InfoMsgData.SourAddr, 0x00, InfoMsgData.SourLen);
				memcpy(InfoMsgData.SourAddr, gCcoRouterAppName, InfoMsgData.SourLen);
				InfoMsgData.DestLen = strlen(gCtsRouterAppName[nTtlIndex]);
				memset(InfoMsgData.DestAddr, 0x00, InfoMsgData.DestLen);
				memcpy(InfoMsgData.DestAddr, gCtsRouterAppName[nTtlIndex], InfoMsgData.DestLen);
				
				CurInfoIndex = 0;

				InfoMsgData.IID_ID = 0x0003;
				InfoMsgData.IOP_ID = 0x0051;
				
				Uint32ToCharArr(gPortID[nTtlIndex], &InfoMsgData.aPayloadData[CurInfoIndex]);  //端口号
				CCODOWN_FMT_DEBUG(nTtlIndex, "端口号: %02x %02x %02x %02x \n", 
						InfoMsgData.aPayloadData[CurInfoIndex], InfoMsgData.aPayloadData[CurInfoIndex+1], 
						InfoMsgData.aPayloadData[CurInfoIndex+2], InfoMsgData.aPayloadData[CurInfoIndex+3]);
				
				CurInfoIndex += 4;
				InfoMsgData.aPayloadData[CurInfoIndex] = 0x00;  //是否含有起始序号信息, 0x00:无, 0x01:有
				CCODOWN_FMT_DEBUG(nTtlIndex, "是否含有起始序号信息: %02x (0x00:无, 0x01:有) \n", InfoMsgData.aPayloadData[CurInfoIndex]);
				
				CurInfoIndex += 1;
				InfoMsgData.aPayloadData[CurInfoIndex] = 0x03;  //查询HPLC信息choice标志
				CCODOWN_FMT_DEBUG(nTtlIndex, "查询HPLC信息choice标志: %02x \n", InfoMsgData.aPayloadData[CurInfoIndex]);
				
				CurInfoIndex += 1;
				InfoMsgData.aPayloadData[CurInfoIndex] = RecvAppData.DataUnitBuff[0];  //邻居节点个数
				CCODOWN_FMT_DEBUG(nTtlIndex, "邻居节点个数: %02x (十进制 %d) \n", InfoMsgData.aPayloadData[CurInfoIndex], InfoMsgData.aPayloadData[CurInfoIndex]);
				
				CurInfoIndex += 1;
				memcpy_r(&InfoMsgData.aPayloadData[CurInfoIndex], &RecvAppData.DataUnitBuff[1], 3);  //本节点网络标识号
				CCODOWN_FMT_DEBUG(nTtlIndex, "本节点网络标识号: %02x %02x %02x \n", 
						InfoMsgData.aPayloadData[CurInfoIndex], InfoMsgData.aPayloadData[CurInfoIndex+1], 
						InfoMsgData.aPayloadData[CurInfoIndex+2]);
				
				CurInfoIndex += 3;
				memcpy_r(&InfoMsgData.aPayloadData[CurInfoIndex], &RecvAppData.DataUnitBuff[4], 6);  //本节点主节点地址
				CCODOWN_FMT_DEBUG(nTtlIndex, "本节点主节点地址: %02x %02x %02x %02x %02x %02x \n", 
							InfoMsgData.aPayloadData[CurInfoIndex], InfoMsgData.aPayloadData[CurInfoIndex+1], 
							InfoMsgData.aPayloadData[CurInfoIndex+2], InfoMsgData.aPayloadData[CurInfoIndex+3], 
							InfoMsgData.aPayloadData[CurInfoIndex+4], InfoMsgData.aPayloadData[CurInfoIndex+5]);
				
				CurInfoIndex += 6;
				ChangeLenToBuff(TotalCount, &CurInfoIndex, &InfoMsgData.aPayloadData[CurInfoIndex]);  //邻居节点网络标识号 seq of
				CCODOWN_FMT_DEBUG(nTtlIndex, "邻居节点网络标识号 seq of: %d(十进制) \n", TotalCount);
				
				for(i=0; i<TotalCount; i++)
				{
					memcpy_r(&InfoMsgData.aPayloadData[CurInfoIndex], &RecvAppData.DataUnitBuff[10+i*3], 3);  //邻居节点网络标识号
					CCODOWN_FMT_DEBUG(nTtlIndex, "i=%d, 邻居节点网络标识号: %02x %02x %02x \n", i, 
						InfoMsgData.aPayloadData[CurInfoIndex], InfoMsgData.aPayloadData[CurInfoIndex+1], 
						InfoMsgData.aPayloadData[CurInfoIndex+2]);
					
					CurInfoIndex += 3;
				}
				InfoMsgData.nPayloadLength = CurInfoIndex;
				PlcWriteQueue(pAmrSendUp, (uint8 *)&InfoMsgData, sizeof(MSG_INFO_T)+InfoMsgData.nPayloadLength);

				gTotalState[nTtlIndex].HplcQuery = HPLC_QUERY_END;//任务完成
				break;
			}

			gTotalState[nTtlIndex].HplcQuery = HPLC_QUERY_RECV;  //判断是否超时
			break;
		}
		
		case HPLC_QUERY_RECV:  //接收
		{
			CCODOWN_FMT_DEBUG(nTtlIndex, "HplcInfoReport_Nets()查询多网络信息 HPLC_QUERY_RECV...端口[%d] \n", nTtlIndex);
			if ( abs(time(NULL) - CommBeginTime[nTtlIndex]) > OverTimeCnt[nTtlIndex])
			{
				CCODOWN_FMT_DEBUG(nTtlIndex, "if ((time(NULL) - CommBeginTime) > OverTimeCnt) 超时...\n");
				gTotalState[nTtlIndex].HplcQuery = HPLC_QUERY_END;  //累计超时, 任务完成
			}
			else
			{
				gTotalState[nTtlIndex].HplcQuery = HPLC_QUERY_SEND;  //进入组帧处理
			}
			
			break;
		}

		case HPLC_QUERY_RETRY:  //重试
		{
			CCODOWN_FMT_DEBUG(nTtlIndex, "HplcInfoReport_Nets()查询多网络信息 HPLC_QUERY_RETRY...端口[%d] \n", nTtlIndex);
			gTotalState[nTtlIndex].HplcQuery = HPLC_QUERY_END;  //达到最大重试次数, 任务完成
			break;
		}

		case HPLC_QUERY_END:  //结束
		{
			CCODOWN_FMT_DEBUG(nTtlIndex, "HplcInfoReport_Nets()查询多网络信息 HPLC_QUERY_END...端口[%d] \n", nTtlIndex);
			gTotalState[nTtlIndex].HplcQuery = HPLC_QUERY_IDLE;
			gQueryTypeHPLC[nTtlIndex].BitType.nets = 0;
			return 1;
			break;
		}

		default:
			break;
	}

	return -1;
}


/*******************************************************************************
* 函数名称: HplcInfoReport_Chip
* 函数功能: HPLC查询信息上报 宽带载波芯片信息
* 输入参数: 无
* 输出参数: 无
* 返 回 值: >0                  成功
*           <0                  失败
*******************************************************************************/
int HplcInfoReport_Chip(TASK_FUN_INPUT stTaskFunInput)
{
	int i   = 0;
	int ret = 0;
	static  time_t  CommBeginTime[MAX_DOWN_THREAD_NUM];  //通讯开始时间
	static  uint32  OverTimeCnt[MAX_DOWN_THREAD_NUM] = {0};  //超时时间
	static  uint16  CurNodeIndex[MAX_DOWN_THREAD_NUM] = {0};  //2013版当前索引从1开始
	uint16  CurInfoIndex = 0;  //消息组帧偏移位
	uint16  TotalCount = 0;  //节点总数量
	uint8   ReplyCount = 0;  //本次应答的节点数量
	uint16  ReplyNodeIndex = 0;  //本次应答的起始序号
	uint16  NodeIndexTemp = 0;  //节点序号, 临时变量

	AppData_T   SendAppData;                        //发送
    AppData_T   RecvAppData;                        //接收
    memset(&SendAppData, 0x00, sizeof(AppData_T));
	memset(&RecvAppData, 0x00, sizeof(AppData_T));
	
	uint8           nTtlIndex = 0;  //不同的端口号对应不同的数组下标
	nTtlIndex = stTaskFunInput.TaskArrIndex;

	INFO_DATA_T     InfoMsgData;  //回复消息
	memset(&InfoMsgData, 0, sizeof(InfoMsgData));

	switch (gTotalState[nTtlIndex].HplcQuery)
	{
		case HPLC_QUERY_IDLE://空闲执行,初始化
		{
			CCODOWN_FMT_DEBUG(nTtlIndex, "HplcInfoReport_Chip()查询宽带载波芯片信息 HPLC_QUERY_IDLE...端口[%d] \n", nTtlIndex);
			ClearCommRecvBuff(nTtlIndex);  //清通讯接收缓冲区
			ReadAmrInfo(nTtlIndex);  //加载档案信息至抄表信息
			CurNodeIndex[nTtlIndex] = 1;//2013版当前索引从1开始
			CommBeginTime[nTtlIndex] = time(NULL);
			OverTimeCnt[nTtlIndex] = 30;
			
			gTotalState[nTtlIndex].HplcQuery = HPLC_QUERY_SEND;  //进入组帧处理
			break;
		}
		case HPLC_QUERY_SEND: //发送
		{
			CCODOWN_FMT_DEBUG(nTtlIndex, "HplcInfoReport_Chip()查询宽带载波芯片信息 HPLC_QUERY_SEND...端口[%d] \n", nTtlIndex);

			//发送报文组帧
			SendAppData.AFN = 0x10;
		    SendAppData.Fn = 112;
		    SendAppData.DataUnitLen = 3;  //节点起始序号2字节, 节点数量1字节
			memcpy(SendAppData.DataUnitBuff, &CurNodeIndex[nTtlIndex], 2);  //从节点起始序号，2字节
			SendAppData.DataUnitBuff[2] = 10;  //每次查询节点数量，1字节
			CCODOWN_FMT_DEBUG(nTtlIndex, "发送报文SendAppData 节点起始序号=%d, 节点数量=%d \n", CurNodeIndex[nTtlIndex], SendAppData.DataUnitBuff[2]);
			
			ret = CommFunDeal(SendAppData, &RecvAppData, nTtlIndex);
			if(ret > 0)
			{
				//接收报文解帧
				memcpy(&TotalCount, &RecvAppData.DataUnitBuff[0], 2);  //节点总数量
				memcpy(&ReplyNodeIndex, &RecvAppData.DataUnitBuff[2], 2);  //节点起始序号
				ReplyCount = RecvAppData.DataUnitBuff[4];  //本次应答的数量
				CCODOWN_FMT_DEBUG(nTtlIndex, "接收报文RecvAppData 节点总数量=%d, 节点起始序号=%d, 本次应答节点数量=%d \n", 
					TotalCount, ReplyNodeIndex, ReplyCount);

				if(ReplyCount > 0)
				{
					//组装消息报文
					InfoMsgData.IP  = inet_addr(gCtsRouterAppIP[nTtlIndex]);
					InfoMsgData.IP  = ntohl(InfoMsgData.IP);
					InfoMsgData.port = gCtsRouterAppPort[nTtlIndex];
					InfoMsgData.MsgPRIORITY = 0;
					InfoMsgData.MsgRPM = 1;
					InfoMsgData.MsgIndex = GetInfoIndex();
					InfoMsgData.MsgLabel = nTtlIndex;  //mqtt存放端口序号(0或1)
					InfoMsgData.SourLen = strlen(gCcoRouterAppName);
					memset(InfoMsgData.SourAddr, 0x00, InfoMsgData.SourLen);
					memcpy(InfoMsgData.SourAddr, gCcoRouterAppName, InfoMsgData.SourLen);
					InfoMsgData.DestLen = strlen(gCtsRouterAppName[nTtlIndex]);
					memset(InfoMsgData.DestAddr, 0x00, InfoMsgData.DestLen);
					memcpy(InfoMsgData.DestAddr, gCtsRouterAppName[nTtlIndex], InfoMsgData.DestLen);
					
					CurInfoIndex = 0;

					InfoMsgData.IID_ID = 0x0003;
					InfoMsgData.IOP_ID = 0x0051;
					
					Uint32ToCharArr(gPortID[nTtlIndex], &InfoMsgData.aPayloadData[CurInfoIndex]);  //端口号
					CCODOWN_FMT_DEBUG(nTtlIndex, "端口号: %02x %02x %02x %02x \n", 
						InfoMsgData.aPayloadData[CurInfoIndex], InfoMsgData.aPayloadData[CurInfoIndex+1], 
						InfoMsgData.aPayloadData[CurInfoIndex+2], InfoMsgData.aPayloadData[CurInfoIndex+3]);
					
					CurInfoIndex += 4;
					InfoMsgData.aPayloadData[CurInfoIndex] = 0x01;  //是否含有起始序号信息, 0x00:无, 0x01:有
					CCODOWN_FMT_DEBUG(nTtlIndex, "是否含有起始序号信息: %02x (0x00:无, 0x01:有) \n", InfoMsgData.aPayloadData[CurInfoIndex]);
					
					CurInfoIndex += 1;
					memcpy_r(&InfoMsgData.aPayloadData[CurInfoIndex], (uint8 *)&ReplyNodeIndex, 2);  //起始序号2字节
					CCODOWN_FMT_DEBUG(nTtlIndex, "起始序号: %02x %02x (十进制 %d)\n", 
						InfoMsgData.aPayloadData[CurInfoIndex], InfoMsgData.aPayloadData[CurInfoIndex+1], ReplyNodeIndex);
					
					CurInfoIndex += 2;
					InfoMsgData.aPayloadData[CurInfoIndex] = 0x02;  //查询HPLC信息choice标志
					CCODOWN_FMT_DEBUG(nTtlIndex, "查询HPLC信息choice标志: %02x \n", InfoMsgData.aPayloadData[CurInfoIndex]);
					
					CurInfoIndex += 1;
					ChangeLenToBuff(ReplyCount, &CurInfoIndex, &InfoMsgData.aPayloadData[CurInfoIndex]);  //信息个数seq of
					CCODOWN_FMT_DEBUG(nTtlIndex, "信息个数seq of: %d(十进制) \n", ReplyCount);
					
					for(i=0; i<ReplyCount; i++)
					{
  						NodeIndexTemp = i + ReplyNodeIndex;
  						Uint16ToCharArr(NodeIndexTemp, &InfoMsgData.aPayloadData[CurInfoIndex]);  //节点序号2字节
  						CCODOWN_FMT_DEBUG(nTtlIndex, "i=%d, 节点序号: %02x %02x (十进制 %d) \n", i, 
							InfoMsgData.aPayloadData[CurInfoIndex], InfoMsgData.aPayloadData[CurInfoIndex+1], NodeIndexTemp);
						
						CurInfoIndex += 2;
						memcpy_r(&InfoMsgData.aPayloadData[CurInfoIndex], &RecvAppData.DataUnitBuff[5+i*33], 6);  //节点地址
						CCODOWN_FMT_DEBUG(nTtlIndex, "节点地址: %02x %02x %02x %02x %02x %02x \n", 
							InfoMsgData.aPayloadData[CurInfoIndex], InfoMsgData.aPayloadData[CurInfoIndex+1], 
							InfoMsgData.aPayloadData[CurInfoIndex+2], InfoMsgData.aPayloadData[CurInfoIndex+3], 
							InfoMsgData.aPayloadData[CurInfoIndex+4], InfoMsgData.aPayloadData[CurInfoIndex+5]);
						
						CurInfoIndex += 6;
						InfoMsgData.aPayloadData[CurInfoIndex] = RecvAppData.DataUnitBuff[5+i*33+6];  //节点设备类型
						CCODOWN_FMT_DEBUG(nTtlIndex, "节点设备类型: %02x \n", 
							InfoMsgData.aPayloadData[CurInfoIndex]);
						
						CurInfoIndex += 1;
						memcpy_r(&InfoMsgData.aPayloadData[CurInfoIndex], &RecvAppData.DataUnitBuff[5+i*33+7], 24);  //节点芯片ID信息
						CCODOWN_BUF_DEBUG(nTtlIndex, &InfoMsgData.aPayloadData[CurInfoIndex], 24, "节点芯片ID信息:");
						
						CurInfoIndex += 24;
						memcpy_r(&InfoMsgData.aPayloadData[CurInfoIndex], &RecvAppData.DataUnitBuff[5+i*33+31], 2);  //节点芯片软件版本
						CCODOWN_FMT_DEBUG(nTtlIndex, "节点芯片软件版本: %02x %02x \n", 
							InfoMsgData.aPayloadData[CurInfoIndex], InfoMsgData.aPayloadData[CurInfoIndex+1]);
						
						CurInfoIndex += 2;
					}
					InfoMsgData.nPayloadLength = CurInfoIndex;
					PlcWriteQueue(pAmrSendUp, (uint8 *)&InfoMsgData, sizeof(MSG_INFO_T)+InfoMsgData.nPayloadLength);
				}

				CurNodeIndex[nTtlIndex] += ReplyCount;  //当前序号累加
				if(CurNodeIndex[nTtlIndex] > TotalCount)
				{
					gTotalState[nTtlIndex].HplcQuery = HPLC_QUERY_END;//任务完成
					break;
				}
			}

			gTotalState[nTtlIndex].HplcQuery = HPLC_QUERY_RECV;  //判断是否超时
			break;
		}
		
		case HPLC_QUERY_RECV:  //接收
		{
			CCODOWN_FMT_DEBUG(nTtlIndex, "HplcInfoReport_Chip()查询宽带载波芯片信息 HPLC_QUERY_RECV...端口[%d] \n", nTtlIndex);
			if ( abs(time(NULL) - CommBeginTime[nTtlIndex]) > OverTimeCnt[nTtlIndex])
			{
				CCODOWN_FMT_DEBUG(nTtlIndex, "if ((time(NULL) - CommBeginTime) > OverTimeCnt) 超时...\n");
				gTotalState[nTtlIndex].HplcQuery = HPLC_QUERY_END;  //累计超时, 任务完成
			}
			else
			{
				gTotalState[nTtlIndex].HplcQuery = HPLC_QUERY_SEND;  //进入组帧处理
			}
			
			break;
		}

		case HPLC_QUERY_RETRY:  //重试
		{
			CCODOWN_FMT_DEBUG(nTtlIndex, "HplcInfoReport_Chip()查询宽带载波芯片信息 HPLC_QUERY_RETRY...端口[%d] \n", nTtlIndex);
			gTotalState[nTtlIndex].HplcQuery = HPLC_QUERY_END;  //达到最大重试次数, 任务完成
			break;
		}

		case HPLC_QUERY_END:  //结束
		{
			CCODOWN_FMT_DEBUG(nTtlIndex, "HplcInfoReport_Chip()查询宽带载波芯片信息 HPLC_QUERY_END...端口[%d] \n", nTtlIndex);
			gTotalState[nTtlIndex].HplcQuery = HPLC_QUERY_IDLE;
			gQueryTypeHPLC[nTtlIndex].BitType.chip = 0;
			return 1;
			break;
		}

		default:
			break;
	}

	return -1;
}


/*******************************************************************************
* 函数名称: HplcInfoReport_Topo
* 函数功能: HPLC查询信息上报 网络拓扑信息
* 输入参数: 无
* 输出参数: 无
* 返 回 值: >0                  成功
*           <0                  失败
*******************************************************************************/
int HplcInfoReport_Topo(TASK_FUN_INPUT stTaskFunInput)
{
	int i   = 0;
	int ret = 0;
	static  time_t  CommBeginTime[MAX_DOWN_THREAD_NUM];  //通讯开始时间
	static  uint32  OverTimeCnt[MAX_DOWN_THREAD_NUM] = {0};  //超时时间
	static  uint16  CurNodeIndex[MAX_DOWN_THREAD_NUM] = {0};  //2013版当前索引从1开始
	uint16  CurInfoIndex = 0;  //消息组帧偏移位
	uint16  TotalCount = 0;  //节点总数量
	uint8   ReplyCount = 0;  //本次应答的节点数量
	uint16  ReplyNodeIndex = 0;  //本次应答的起始序号

	AppData_T   SendAppData;                        //发送
    AppData_T   RecvAppData;                        //接收
    memset(&SendAppData, 0x00, sizeof(AppData_T));
	memset(&RecvAppData, 0x00, sizeof(AppData_T));
	
	uint8           nTtlIndex = 0;  //不同的端口号对应不同的数组下标
	nTtlIndex = stTaskFunInput.TaskArrIndex;

	INFO_DATA_T     InfoMsgData;  //回复消息
	memset(&InfoMsgData, 0, sizeof(InfoMsgData));

	switch (gTotalState[nTtlIndex].HplcQuery)
	{
		case HPLC_QUERY_IDLE://空闲执行,初始化
		{
			CCODOWN_FMT_DEBUG(nTtlIndex, "HplcInfoReport_Topo()查询网络拓扑信息 HPLC_QUERY_IDLE...端口[%d] \n", nTtlIndex);
			ClearCommRecvBuff(nTtlIndex);  //清通讯接收缓冲区
			ReadAmrInfo(nTtlIndex);  //加载档案信息至抄表信息
			CurNodeIndex[nTtlIndex] = 1;//2013版当前索引从1开始
			CommBeginTime[nTtlIndex] = time(NULL);
			OverTimeCnt[nTtlIndex] = 30;
			
			gTotalState[nTtlIndex].HplcQuery = HPLC_QUERY_SEND;  //进入组帧处理
			break;
		}
		case HPLC_QUERY_SEND: //发送
		{
			CCODOWN_FMT_DEBUG(nTtlIndex, "HplcInfoReport_Topo()查询网络拓扑信息 HPLC_QUERY_SEND...端口[%d] \n", nTtlIndex);

			//发送报文组帧
			SendAppData.AFN = 0x10;
		    SendAppData.Fn = 21;
		    SendAppData.DataUnitLen = 3;  //节点起始序号2字节, 节点数量1字节
			memcpy(SendAppData.DataUnitBuff, &CurNodeIndex[nTtlIndex], 2);  //从节点起始序号，2字节
			SendAppData.DataUnitBuff[2] = 10;  //每次查询节点数量，1字节
			CCODOWN_FMT_DEBUG(nTtlIndex, "发送报文SendAppData 节点起始序号=%d, 节点数量=%d \n", CurNodeIndex[nTtlIndex], SendAppData.DataUnitBuff[2]);
			
			ret = CommFunDeal(SendAppData, &RecvAppData, nTtlIndex);
			if(ret > 0)
			{
				//接收报文解帧
				memcpy(&TotalCount, &RecvAppData.DataUnitBuff[0], 2);  //节点总数量
				memcpy(&ReplyNodeIndex, &RecvAppData.DataUnitBuff[2], 2);  //节点起始序号
				ReplyCount = RecvAppData.DataUnitBuff[4];  //本次应答的数量
				CCODOWN_FMT_DEBUG(nTtlIndex, "接收报文RecvAppData 节点总数量=%d, 节点起始序号=%d, 本次应答节点数量=%d \n", 
					TotalCount, ReplyNodeIndex, ReplyCount);

				if(ReplyCount > 0)
				{
					//组装消息报文
					InfoMsgData.IP  = inet_addr(gCtsRouterAppIP[nTtlIndex]);
					InfoMsgData.IP  = ntohl(InfoMsgData.IP);
					InfoMsgData.port = gCtsRouterAppPort[nTtlIndex];
					InfoMsgData.MsgPRIORITY = 0;
					InfoMsgData.MsgRPM = 1;
					InfoMsgData.MsgIndex = GetInfoIndex();
					InfoMsgData.MsgLabel = nTtlIndex;  //mqtt存放端口序号(0或1)
					InfoMsgData.SourLen = strlen(gCcoRouterAppName);
					memset(InfoMsgData.SourAddr, 0x00, InfoMsgData.SourLen);
					memcpy(InfoMsgData.SourAddr, gCcoRouterAppName, InfoMsgData.SourLen);
					InfoMsgData.DestLen = strlen(gCtsRouterAppName[nTtlIndex]);
					memset(InfoMsgData.DestAddr, 0x00, InfoMsgData.DestLen);
					memcpy(InfoMsgData.DestAddr, gCtsRouterAppName[nTtlIndex], InfoMsgData.DestLen);
					
					CurInfoIndex = 0;

					InfoMsgData.IID_ID = 0x0003;
					InfoMsgData.IOP_ID = 0x0051;
					
					Uint32ToCharArr(gPortID[nTtlIndex], &InfoMsgData.aPayloadData[CurInfoIndex]);  //端口号
					CCODOWN_FMT_DEBUG(nTtlIndex, "端口号: %02x %02x %02x %02x \n", 
						InfoMsgData.aPayloadData[CurInfoIndex], InfoMsgData.aPayloadData[CurInfoIndex+1], 
						InfoMsgData.aPayloadData[CurInfoIndex+2], InfoMsgData.aPayloadData[CurInfoIndex+3]);

					CurInfoIndex += 4;
					InfoMsgData.aPayloadData[CurInfoIndex] = 0x01;  //是否含有起始序号信息, 0x00:无, 0x01:有
					CCODOWN_FMT_DEBUG(nTtlIndex, "是否含有起始序号信息: %02x (0x00:无, 0x01:有) \n", InfoMsgData.aPayloadData[CurInfoIndex]);

					CurInfoIndex += 1;
					memcpy_r(&InfoMsgData.aPayloadData[CurInfoIndex], (uint8 *)&ReplyNodeIndex, 2);  //起始序号2字节
					CCODOWN_FMT_DEBUG(nTtlIndex, "起始序号: %02x %02x (十进制 %d) \n", 
						InfoMsgData.aPayloadData[CurInfoIndex], InfoMsgData.aPayloadData[CurInfoIndex+1], ReplyNodeIndex);

					CurInfoIndex += 2;
					InfoMsgData.aPayloadData[CurInfoIndex] = 0x01;  //查询HPLC信息choice标志
					CCODOWN_FMT_DEBUG(nTtlIndex, "查询HPLC信息choice标志: %02x \n", InfoMsgData.aPayloadData[CurInfoIndex]);

					CurInfoIndex += 1;
					ChangeLenToBuff(ReplyCount, &CurInfoIndex, &InfoMsgData.aPayloadData[CurInfoIndex]);  //信息个数seq of
					CCODOWN_FMT_DEBUG(nTtlIndex, "信息个数seq of: %d(十进制) \n", ReplyCount);
					
					for(i=0; i<ReplyCount; i++)
					{
						memcpy_r(&InfoMsgData.aPayloadData[CurInfoIndex], &RecvAppData.DataUnitBuff[5+i*11], 6);  //节点地址
						CCODOWN_FMT_DEBUG(nTtlIndex, "i=%d, 节点地址: %02x %02x %02x %02x %02x %02x \n", i, 
							InfoMsgData.aPayloadData[CurInfoIndex], InfoMsgData.aPayloadData[CurInfoIndex+1], 
							InfoMsgData.aPayloadData[CurInfoIndex+2], InfoMsgData.aPayloadData[CurInfoIndex+3], 
							InfoMsgData.aPayloadData[CurInfoIndex+4], InfoMsgData.aPayloadData[CurInfoIndex+5]);
						
						CurInfoIndex += 6;

                        //节点标识 2字节
						memcpy_r(&InfoMsgData.aPayloadData[CurInfoIndex], &RecvAppData.DataUnitBuff[5+i*11+6], 2);  //节点拓扑信息
						CCODOWN_FMT_DEBUG(nTtlIndex, "i=%d, 节点标识: %02x %02x \n", i, 
							InfoMsgData.aPayloadData[CurInfoIndex], InfoMsgData.aPayloadData[CurInfoIndex+1]);
						
						CurInfoIndex += 2;

                        //代理节点标识 2字节
                        memcpy_r(&InfoMsgData.aPayloadData[CurInfoIndex], &RecvAppData.DataUnitBuff[5+i*11+6+2], 2);  //节点拓扑信息
						CCODOWN_FMT_DEBUG(nTtlIndex, "i=%d, 代理节点标识: %02x %02x \n", i, 
							InfoMsgData.aPayloadData[CurInfoIndex], InfoMsgData.aPayloadData[CurInfoIndex+1]);
						
						CurInfoIndex += 2;

                        //节点信息
                        memcpy_r(&InfoMsgData.aPayloadData[CurInfoIndex], &RecvAppData.DataUnitBuff[5+i*11+6+4], 1);  //节点拓扑信息
						CCODOWN_FMT_DEBUG(nTtlIndex, "i=%d, 节点信息: %02x \n", i, 
							InfoMsgData.aPayloadData[CurInfoIndex]);
						
						CurInfoIndex += 1;
					}
					InfoMsgData.nPayloadLength = CurInfoIndex;
					PlcWriteQueue(pAmrSendUp, (uint8 *)&InfoMsgData, sizeof(MSG_INFO_T)+InfoMsgData.nPayloadLength);

					CurNodeIndex[nTtlIndex] += ReplyCount;  //当前序号累加
					if(CurNodeIndex[nTtlIndex] > TotalCount)
					{
						gTotalState[nTtlIndex].HplcQuery = HPLC_QUERY_END;//任务完成
						break;
					}
				}
			}
			
			gTotalState[nTtlIndex].HplcQuery = HPLC_QUERY_RECV;  //判断是否超时
			break;
		}
		
		case HPLC_QUERY_RECV:  //接收
		{
			CCODOWN_FMT_DEBUG(nTtlIndex, "HplcInfoReport_Topo()查询网络拓扑信息 HPLC_QUERY_RECV...端口[%d] \n", nTtlIndex);
			if ( abs(time(NULL) - CommBeginTime[nTtlIndex]) > OverTimeCnt[nTtlIndex])
			{
				CCODOWN_FMT_DEBUG(nTtlIndex, "if ((time(NULL) - CommBeginTime) > OverTimeCnt) 超时...\n");
				gTotalState[nTtlIndex].HplcQuery = HPLC_QUERY_END;  //累计超时, 任务完成
			}
			else
			{
				gTotalState[nTtlIndex].HplcQuery = HPLC_QUERY_SEND;  //进入组帧处理
			}
			
			break;
		}

		case HPLC_QUERY_RETRY:  //重试
		{
			CCODOWN_FMT_DEBUG(nTtlIndex, "HplcInfoReport_Topo()查询网络拓扑信息 HPLC_QUERY_RETRY...端口[%d] \n", nTtlIndex);
			gTotalState[nTtlIndex].HplcQuery = HPLC_QUERY_END;  //达到最大重试次数, 任务完成
			break;
		}

		case HPLC_QUERY_END:  //结束
		{
			CCODOWN_FMT_DEBUG(nTtlIndex, "HplcInfoReport_Topo()查询网络拓扑信息 HPLC_QUERY_END...端口[%d] \n", nTtlIndex);
			gTotalState[nTtlIndex].HplcQuery = HPLC_QUERY_IDLE;
			gQueryTypeHPLC[nTtlIndex].BitType.topo = 0;
			return 1;
			break;
		}

		default:
			break;
	}

	return -1;
}


/*******************************************************************************
* 函数名称: HplcInfoReport
* 函数功能: HPLC查询信息上报
* 输入参数: 无
* 输出参数: 无
* 返 回 值: >0                  成功
*           <0                  失败
*******************************************************************************/
int HplcInfoReport(TASK_FUN_INPUT stTaskFunInput)
{
    int result = 1;
    
    uint8 nTtlIndex = 0;  //不同的端口号对应不同的数组下标
    nTtlIndex = stTaskFunInput.TaskArrIndex;
    
    CCODOWN_FMT_DEBUG(nTtlIndex, "int HplcInfoReport(TASK_FUN_INPUT stTaskFunInput)HPLC查询信息上报...\n");

    if(gQueryTypeHPLC[nTtlIndex].BitType.topo == 1)
    {
        result = HplcInfoReport_Topo(stTaskFunInput);
    }
    else if(gQueryTypeHPLC[nTtlIndex].BitType.chip == 1)
    {
        result = HplcInfoReport_Chip(stTaskFunInput);
    }
    else if(gQueryTypeHPLC[nTtlIndex].BitType.nets == 1)
    {
        result = HplcInfoReport_Nets(stTaskFunInput);
    }
    else if(gQueryTypeHPLC[nTtlIndex].BitType.phase == 1)
    {
        result = HplcInfoReport_Phase(stTaskFunInput);
    }
    else if(gQueryTypeHPLC[nTtlIndex].BitType.scale == 1)
    {
        result = HplcInfoReport_Scale(stTaskFunInput);
    }
    else if(gQueryTypeHPLC[nTtlIndex].BitType.master == 1)
    {
        result = HplcInfoReport_Master(stTaskFunInput);
    }
    else if(gQueryTypeHPLC[nTtlIndex].BitType.slave == 1)
    {
        result = HplcInfoReport_Slave(stTaskFunInput);
    }
    else if(gQueryTypeHPLC[nTtlIndex].BitType.module == 1)
    {
        result = HplcInfoReport_Module(stTaskFunInput);
    }
    else if(gQueryTypeHPLC[nTtlIndex].BitType.nodeversion == 1)
    {
        result = HplcInfoReport_NodeVersion(stTaskFunInput);
    }
    else
    {
        result = 1;
    }

    return result;
    
}

/*******************************************************************************
* 函数名称: HplcFrSet
* 函数功能: HPLC频点设置
* 输入参数: 无
* 输出参数: 无
* 返 回 值: >0                  成功
*           <0                  失败
*******************************************************************************/
int HplcFrSet(TASK_FUN_INPUT stTaskFunInput)
{
	AppData_T   SendAppData;                        //发送
    AppData_T   RecvAppData;                        //接收
    memset(&SendAppData, 0x00, sizeof(AppData_T));
	memset(&RecvAppData, 0x00, sizeof(AppData_T));

	uint8 nTtlIndex = 0;  //不同的端口号对应不同的数组下标
	nTtlIndex = stTaskFunInput.TaskArrIndex;

	SendAppData.AFN = 0x05;
    SendAppData.Fn = 16;
    SendAppData.DataUnitLen = 1;
	SendAppData.DataUnitBuff[0] = gFrSetValue[nTtlIndex];

	if(CommFunDeal(SendAppData, &RecvAppData, nTtlIndex))
	{
		return 1;
	}
	else
	{
		return 0;
	}
	
}

/*******************************************************************************
* 函数名称: HplcFrQuery
* 函数功能: HPLC频点查询
* 输入参数: 无
* 输出参数: 无
* 返 回 值: >0                  成功
*           <0                  失败
*******************************************************************************/
int HplcFrQuery(TASK_FUN_INPUT stTaskFunInput)
{
	int ret = 0;
	static  time_t  CommBeginTime[MAX_DOWN_THREAD_NUM];  //通讯开始时间
	static  uint32  OverTimeCnt[MAX_DOWN_THREAD_NUM] = {0};  //超时时间
	uint16  CurInfoIndex = 0;  //消息组帧偏移位
	int     IsSendMqtt = 0;  //是否将查询结果上报mqtt

	AppData_T   SendAppData;                        //发送
    AppData_T   RecvAppData;                        //接收
    memset(&SendAppData, 0x00, sizeof(AppData_T));
	memset(&RecvAppData, 0x00, sizeof(AppData_T));
	
	uint8           nTtlIndex = 0;  //不同的端口号对应不同的数组下标
	nTtlIndex = stTaskFunInput.TaskArrIndex;

	INFO_DATA_T     InfoMsgData;  //回复消息
	memset(&InfoMsgData, 0, sizeof(InfoMsgData));
	memcpy(&InfoMsgData, &stTaskFunInput.InfoDataHead, sizeof(INFO_DATA_HEAD_T));  //获取原始HEAD
	InfoMsgData.MsgRPM = 0;   //启动标志位, 回复，非启动

	IsSendMqtt = stTaskFunInput.InfoMsgBuff[0];  //0:不需要上报; 1:需要上报mqtt

	switch (gTotalState[nTtlIndex].HplcQuery)
	{
		case HPLC_QUERY_IDLE://空闲执行,初始化
		{
			CCODOWN_FMT_DEBUG(nTtlIndex, "HplcFrQuery()查询HPLC频点 HPLC_QUERY_IDLE...端口[%d] \n", nTtlIndex);
			ClearCommRecvBuff(nTtlIndex);  //清通讯接收缓冲区
			ReadAmrInfo(nTtlIndex);  //加载档案信息至抄表信息
			CommBeginTime[nTtlIndex] = time(NULL);
			OverTimeCnt[nTtlIndex] = 20;
			
			gTotalState[nTtlIndex].HplcQuery = HPLC_QUERY_SEND;  //进入组帧处理
			break;
		}
		case HPLC_QUERY_SEND: //发送
		{
			CCODOWN_FMT_DEBUG(nTtlIndex, "HplcFrQuery()查询HPLC频点 HPLC_QUERY_SEND...端口[%d] \n", nTtlIndex);

			SendAppData.AFN = 0x03;
		    SendAppData.Fn = 16;
		    SendAppData.DataUnitLen = 0;  //无数据单元
			
			ret = CommFunDeal(SendAppData, &RecvAppData, nTtlIndex);
			if(ret > 0)
			{
				//频点赋值
				gGuiShowMessage[nTtlIndex].nFrValue = RecvAppData.DataUnitBuff[0];
				CCODOWN_FMT_DEBUG(nTtlIndex, "HplcFrQuery()查询HPLC频点 nFrValue=%d \n", RecvAppData.DataUnitBuff[0]);
				
				if(IsSendMqtt > 0)
				{
					//组装消息报文
					CurInfoIndex = 0;

					InfoMsgData.IID_ID = 0x0003;
					InfoMsgData.IOP_ID = 0x0051;
					
					InfoMsgData.aPayloadData[CurInfoIndex] = RecvAppData.DataUnitBuff[0];  //HPLC频段值
					CurInfoIndex += 1;
					InfoMsgData.nPayloadLength = CurInfoIndex;
					PlcWriteQueue(pAmrSendUp, (uint8 *)&InfoMsgData, sizeof(MSG_INFO_T)+InfoMsgData.nPayloadLength);
				}
				else
				{
					ret = write_pdata_xram(gGuiInfoName[nTtlIndex], (char *)&gGuiShowMessage[nTtlIndex], 0, sizeof(gGuiShowMessage[nTtlIndex]));
					CCOUP_FMT_DEBUG("HplcFrQuery()查询HPLC频点(非上报mqtt) 将显示相关信息写入私有文件, ret=%d \n", ret);
					CCODOWN_FMT_DEBUG(nTtlIndex, "HplcFrQuery()查询HPLC频点(非上报mqtt) 结束...端口[%d] \n", nTtlIndex);
				}
				
				gTotalState[nTtlIndex].HplcQuery = HPLC_QUERY_END;//任务完成
				break;
			}

			gTotalState[nTtlIndex].HplcQuery = HPLC_QUERY_RECV;
			break;
		}
		
		case HPLC_QUERY_RECV:  //接收
		{
			CCODOWN_FMT_DEBUG(nTtlIndex, "HplcFrQuery()查询HPLC频点 HPLC_QUERY_RECV...端口[%d] \n", nTtlIndex);
			if ( abs(time(NULL) - CommBeginTime[nTtlIndex]) > OverTimeCnt[nTtlIndex])
			{
				CCODOWN_FMT_DEBUG(nTtlIndex, "if ((time(NULL) - CommBeginTime) > OverTimeCnt) 超时...\n");
				gTotalState[nTtlIndex].HplcQuery = HPLC_QUERY_END;  //累计超时, 任务完成
			}
			else
			{
				gTotalState[nTtlIndex].HplcQuery = HPLC_QUERY_SEND;  //进入组帧处理
			}
			
			break;
		}

		case HPLC_QUERY_RETRY:  //重试
		{
			CCODOWN_FMT_DEBUG(nTtlIndex, "HplcFrQuery()查询HPLC频点 HPLC_QUERY_RETRY...端口[%d] \n", nTtlIndex);
			gTotalState[nTtlIndex].HplcQuery = HPLC_QUERY_END;  //达到最大重试次数, 任务完成
			break;
		}

		case HPLC_QUERY_END:  //结束
		{
			CCODOWN_FMT_DEBUG(nTtlIndex, "HplcFrQuery()查询HPLC频点 HPLC_QUERY_END...端口[%d] \n", nTtlIndex);
			gTotalState[nTtlIndex].HplcQuery = HPLC_QUERY_IDLE;
			return 1;
			break;
		}

		default:
			break;
	}

	return -1;
	
}


/*******************************************************************************
* 函数名称: HplcAreaIdentify
* 函数功能: HPLC台区识别
* 输入参数: 无
* 输出参数: 无
* 返 回 值: >0                  成功
*           <0                  失败
*******************************************************************************/
int HplcAreaIdentify(TASK_FUN_INPUT stTaskFunInput)
{
	AppData_T   SendAppData;                        //发送
    AppData_T   RecvAppData;                        //接收
    memset(&SendAppData, 0x00, sizeof(AppData_T));
	memset(&RecvAppData, 0x00, sizeof(AppData_T));

	uint8 nTtlIndex = 0;  //不同的端口号对应不同的数组下标
	nTtlIndex = stTaskFunInput.TaskArrIndex;

	SendAppData.AFN = 0x05;
    SendAppData.Fn = 6;
    SendAppData.DataUnitLen = 1;
	SendAppData.DataUnitBuff[0] = gAreaIdentifyValue[nTtlIndex];

	if(CommFunDeal(SendAppData, &RecvAppData, nTtlIndex))
	{
		return 1;
	}
	else
	{
		return 0;
	}
	
}

/*******************************************************************************
* 函数名称: HplcOfflineFeel
* 函数功能: HPLC离网感知
* 输入参数: 无
* 输出参数: 无
* 返 回 值: >0                  成功
*           <0                  失败
*******************************************************************************/
int HplcOfflineFeel(TASK_FUN_INPUT stTaskFunInput)
{
	return 1;
	
}

/*******************************************************************************
* 函数名称: HplcRefuseNodeReport
* 函数功能: HPLC拒绝节点上报
* 输入参数: 无
* 输出参数: 无
* 返 回 值: >0                  成功
*           <0                  失败
*******************************************************************************/
int HplcRefuseNodeReport(TASK_FUN_INPUT stTaskFunInput)
{
	AppData_T   SendAppData;                        //发送
    AppData_T   RecvAppData;                        //接收
    memset(&SendAppData, 0x00, sizeof(AppData_T));
	memset(&RecvAppData, 0x00, sizeof(AppData_T));

	uint8 nTtlIndex = 0;  //不同的端口号对应不同的数组下标
	nTtlIndex = stTaskFunInput.TaskArrIndex;

	SendAppData.AFN = 0x05;
    SendAppData.Fn = 200;
    SendAppData.DataUnitLen = 1;
	SendAppData.DataUnitBuff[0] = gRefuseNodeReportValue[nTtlIndex];

	if(CommFunDeal(SendAppData, &RecvAppData, nTtlIndex))
	{
		return 1;
	}
	else
	{
		return 0;
	}
	
}


/*******************************************************************************
* 函数名称:  CheckModuleSoftReset
* 函数功能:  检测模块是否复位
* 输入参数:
* 输出参数:
* 返 回 值:
*******************************************************************************/
int8 CheckModuleSoftReset(int8 CheckRmt, uint8 nArrIndex)
{
    static int8 ModuleRmt[MAX_DOWN_THREAD_NUM] = {0};//0x01为主动查询方式 0x0为复位后的主动上报    

    //printx(" CheckModuleSoftReset(int8 CheckRmt)模块厂商信息上报方式 : %d \n",ModuleRmt);
    //读取上报AFN=0x03 F10是主动查询还是主动上报方式
    if(CheckRmt == -1)
        return ModuleRmt[nArrIndex];

    ModuleRmt[nArrIndex] = CheckRmt;
    return ModuleRmt[nArrIndex];
}

/*******************************************************************************
* 函数名称:	Comm_ConfirmAckTakeFrameSeq
* 函数功能:	通讯:确认帧应答
* 输入参数:	无
* 输出参数:	无
* 返 回 值: 无
*******************************************************************************/
void Comm_ConfirmAckTakeFrameSeq(AppData_T ModuleRequestAppData, uint8 nArrIndex)
{
    AppData_T AppData;
    memset(&AppData,0x0,sizeof(AppData));
    AppData.RInfo[5] = ModuleRequestAppData.RInfo[5]; //获取帧序列
    AppData.RInfo[1] = ModuleRequestAppData.RInfo[1]; //信道标识
    //common bug(2014-6-24-14-09-30)
    AppData.AFN = 0x00;
    AppData.Fn = 1;
    AppData.DataUnitLen = 6;
    AppData.DataUnitBuff[0] = 0xff;
    AppData.DataUnitBuff[1] = 0xff;
    AppData.DataUnitBuff[2] = 0xff;
    AppData.DataUnitBuff[3] = 0xff;

    AppData.DataUnitBuff[4] = 0x00;
    AppData.DataUnitBuff[5] = 0x00;


    PackFrameData(AppData, nArrIndex);
}


/*******************************************************************************
* 函数名称:	PackInfoMsgStruct
* 函数功能:	组装消息MSG结构体
* 输入参数:	
* 输出参数:	
* 返 回 值: 
*******************************************************************************/
void PackInfoMsgStruct(uint16 IID_ID, uint16 IOP_ID, uint16 nBuffLen, uint8 *aBuff, INFO_DATA_T *InfoMsgData)
{
	uint16          IndexID = 0;  //偏移位
	
	InfoMsgData->IID_ID = IID_ID;
	InfoMsgData->IOP_ID = IOP_ID;

	ChangeLenToBuff(nBuffLen, &IndexID, InfoMsgData->aPayloadData);  //octet-string变长																	
	memcpy(&InfoMsgData->aPayloadData[IndexID], aBuff, nBuffLen);  //payload
	IndexID += nBuffLen;
	InfoMsgData->nPayloadLength = IndexID; 

}

/*******************************************************************************
* 函数名称:	InfoSimpleAck
* 函数功能:	用于消息的BOOL回复
* 输入参数:	
* 输出参数:	
* 返 回 值: 
*******************************************************************************/
void InfoSimpleAck(INFO_DATA_T InfoMsgData, BOOL bResult)
{
	memset(&InfoMsgData.aPayloadData[0], 0x00, InfoMsgData.nPayloadLength);
	InfoMsgData.MsgRPM = 0;
	InfoMsgData.nPayloadLength = 1;
	InfoMsgData.aPayloadData[0] = 0x00;
	if(bResult)
	{
		InfoMsgData.aPayloadData[0] = 0x01;
	}
	PlcWriteQueue(pAmrSendUp, (uint8 *)&InfoMsgData, sizeof(MSG_INFO_T)+InfoMsgData.nPayloadLength);
}

/*******************************************************************************
* 函数名称:	InfoEmptyAck
* 函数功能:	用于消息的空回复
* 输入参数:	
* 输出参数:	
* 返 回 值: 
*******************************************************************************/
void InfoEmptyAck(INFO_DATA_T InfoMsgData)
{
	memset(&InfoMsgData.aPayloadData[0], 0x00, InfoMsgData.nPayloadLength);
	InfoMsgData.MsgRPM = 0;
	InfoMsgData.nPayloadLength = 0;
	PlcWriteQueue(pAmrSendUp, (uint8 *)&InfoMsgData, sizeof(MSG_INFO_T)+InfoMsgData.nPayloadLength);
}


/*******************************************************************************
* 函数名称:	InfoSetPortID
* 函数功能:	抄表端口配置
* 输入参数:	
* 输出参数:	
* 返 回 值: 
*******************************************************************************/
void InfoSetPortID(INFO_DATA_T InfoMsgData)
{
	CCOUP_FMT_DEBUG("InfoSetPortID()抄表端口配置...begin \n");
	
	uint32      nPortID = 0;  //端口号
    uint16      IndexID = 0;  //偏移位
    uint8       nOpType = 0;  //0:删除, 1:增加
    int i = 0;
	int ret = -1;
	int Ret = -1;

	//解析消息MSG
	IndexID = 0;
	CharArrToUint32(&InfoMsgData.aPayloadData[IndexID], &nPortID);	//端口号
	IndexID += 4;
	nOpType = InfoMsgData.aPayloadData[IndexID];  //0:删除, 1:增加

	CCOUP_FMT_DEBUG("InfoSetPortID()抄表端口配置 端口号nPortID=%u, 操作nOpType=%d (0:删除, 1:增加) \n", nPortID, nOpType);

	for(i=0; i<MAX_ALLOWED_DOWN_THREAD_NUM; i++)
	{
		CCOUP_FMT_DEBUG("InfoSetPortID()抄表端口配置 更新之前: i=%d, gPortID[i]=%u (0x%08x), bSetPortIDFlag[i]=%d \n", i, gPortID[i], gPortID[i], bSetPortIDFlag[i]);
	}

	if((nPortID != PORT_F2090201) && (nPortID != PORT_F2090202))
	{
		CCOUP_FMT_DEBUG("InfoSetPortID() if((nPortID != PORT_F2090201) && (nPortID != PORT_F2090202)) 端口号错误! \n");

		//消息组帧发至消息中心
		InfoSimpleAck(InfoMsgData, FALSE);

		return;
	}

	if(nOpType == 0) //删除
	{
		for(i=0; i<MAX_ALLOWED_DOWN_THREAD_NUM; i++)
		{
			if( ((i==0) && (nPortID==PORT_F2090201))
				|| ((i==1) && (nPortID==PORT_F2090202)) )
			{
				ret = 1;
				gPortID[i] = 0;
				bSetPortIDFlag[i] = 0;
				CCOUP_FMT_DEBUG("InfoSetPortID()抄表端口配置 删除之后: i=%d, gPortID[i]=%u (0x%08x), bSetPortIDFlag[i]=%d \n", i, gPortID[i], gPortID[i], bSetPortIDFlag[i]);

				Ret = write_pdata_xram(gPortIDName[i], (char *)&gPortID[i], 0, sizeof(gPortID[i]));
				CCOUP_FMT_DEBUG("InfoSetPortID()抄表端口配置 将端口号写入私有文件, Ret=%d \n", Ret);
			}
		}
		if(ret < 0)
		{
			CCOUP_FMT_DEBUG("InfoSetPortID()抄表端口配置 删除端口操作失败 \n");
		}
	}
	else if(nOpType == 1) //增加
	{
		for(i=0; i<MAX_ALLOWED_DOWN_THREAD_NUM; i++)
		{
			if( ((i==0) && (nPortID==PORT_F2090201))
				|| ((i==1) && (nPortID==PORT_F2090202)) )
			{
				ret = 1;
				gPortID[i] = nPortID;
				bSetPortIDFlag[i] = 1;
				CCOUP_FMT_DEBUG("InfoSetPortID()抄表端口配置 增加之后: i=%d, gPortID[i]=%u (0x%08x), bSetPortIDFlag[i]=%d \n", i, gPortID[i], gPortID[i], bSetPortIDFlag[i]);

				Ret = write_pdata_xram(gPortIDName[i], (char *)&gPortID[i], 0, sizeof(gPortID[i]));
				CCOUP_FMT_DEBUG("InfoSetPortID()抄表端口配置 将端口号写入私有文件, Ret=%d \n", Ret);
			
				break;
			}
		}
		if(ret < 0)
		{
			CCOUP_FMT_DEBUG("InfoSetPortID()抄表端口配置 增加端口操作失败! \n");
		}
	}
	else //异常
	{
		CCOUP_FMT_DEBUG("InfoSetPortID()抄表端口配置 操作类型错误! \n");

		//消息组帧发至消息中心
		InfoSimpleAck(InfoMsgData, FALSE);

		return;
	}

	//消息组帧发至消息中心
	InfoSimpleAck(InfoMsgData, TRUE);
	
}

/*******************************************************************************
* 函数名称:	InfoAddMeterFiles
* 函数功能:	添加表档案
* 输入参数:	
* 输出参数:	
* 返 回 值: 
*******************************************************************************/
void InfoAddMeterFiles(INFO_DATA_T InfoMsgData)
{
    CCOUP_FMT_DEBUG("InfoAddMeterFiles()添加表档案...begin \n");
    
    uint8   nTtlIndex = 0;  //不同的端口号对应不同的数组下标
    int     i = 0;
    BOOL    bAddResult = FALSE;
    BOOL    bChanged[MAX_DOWN_THREAD_NUM] = {0};
    uint16  IndexID = 0;  //偏移位
    int     ret = 0;

    //解析消息MSG
    uint16   nAddFilesNum = 0;  //添加的电能表档案个数
    BuffToChangeLen(&InfoMsgData.aPayloadData[IndexID], &IndexID, &nAddFilesNum);
    
    if(nAddFilesNum <= 0)
    {
        //消息组帧发至消息中心
        InfoSimpleAck(InfoMsgData, FALSE);
        return;
    }
    AMR_METER_FILES_INFO_T aAddFilesData[nAddFilesNum];

    for(i=0; i<nAddFilesNum; i++)
    {
        CharArrToUint32(&InfoMsgData.aPayloadData[IndexID], &(aAddFilesData[i].nPortID));  //端口号
        IndexID += 4;
        //CharArrToUint16(&InfoMsgData.aPayloadData[IndexID], &(aAddFilesData[i].stAmrMeterFiles.nMeterFilesID));  //档案序号
        //IndexID += 2;
        //IndexID += 1;  //TLV，跳过档案地址（6字节<128）的L（1字节）
        memcpy_r(aAddFilesData[i].stAmrMeterFiles.aMeterFilesAddr, InfoMsgData.aPayloadData+IndexID, 6);  //档案地址
        IndexID += 6;
        //IndexID += 1;  //TLV，跳过档案Mac（6字节<128）的L（1字节）
        memcpy_r(aAddFilesData[i].stAmrMeterFiles.aMeterFilesMac, InfoMsgData.aPayloadData+IndexID, 6);  //档案mac
        IndexID += 6;
        aAddFilesData[i].stAmrMeterFiles.nProtocolType = InfoMsgData.aPayloadData[IndexID];  //协议类型
        IndexID += 1;
        //aAddFilesData[i].stAmrMeterFiles.nCommPara = InfoMsgData.aPayloadData[IndexID];  //通信参数
        //IndexID += 1;       
    }

    //写入内存
    for(i=0; i<nAddFilesNum; i++)
    {
        //添加档案
        CCOUP_FMT_DEBUG("InfoAddMeterFiles()gAmrMeterFilesListTemp[%d]数量:%d...begin \n", nTtlIndex, ListLength_Files(gAmrMeterFilesListTemp[nTtlIndex]));  //添加之前的数目
        
        //判断端口号是否有效
        ret = PortIDToArrIndex(aAddFilesData[i].nPortID, &nTtlIndex);
        if(ret < 0)
        {
            break;
        }
        
        //判断是否已经存在相同地址的档案
        ret = LocateElemByPara_Files(gAmrMeterFilesListTemp[nTtlIndex], aAddFilesData[i], CMP_FILES_ADDR);
        if(ret > 0)  //更新
        {
            bAddResult = UpdateByAddr_Files(gAmrMeterFilesListTemp[nTtlIndex], aAddFilesData[i]);
            //更新档案
            CCOUP_FMT_DEBUG("档案地址已存在, 直接更新. 更新的档案信息为: \n");
            PrintFilesDetail(aAddFilesData[i], 0xaa);
            CCOUP_FMT_DEBUG("InfoAddMeterFiles()更新之后 gAmrMeterFilesListTemp[%d]数量:%d...end \n", nTtlIndex, ListLength_Files(gAmrMeterFilesListTemp[nTtlIndex]));  //更新之后的数目
        }
        else  //添加
        {
            bAddResult = ListInsert_Files(&gAmrMeterFilesListTemp[nTtlIndex], gAmrMeterFilesListTemp[nTtlIndex].nMeterFilesNum+1, aAddFilesData[i]);
            //添加档案
            CCOUP_FMT_DEBUG("添加的档案信息为: \n");
            PrintFilesDetail(aAddFilesData[i], 0xaa);
            CCOUP_FMT_DEBUG("InfoAddMeterFiles()添加之后 gAmrMeterFilesListTemp[%d]数量:%d...end \n", nTtlIndex, ListLength_Files(gAmrMeterFilesListTemp[nTtlIndex]));  //添加之后的数目
        }
        
        if(bAddResult)
        {
            bChanged[nTtlIndex] = TRUE;         
        }
        
    }

    //刷新表档案
    for(nTtlIndex=0; nTtlIndex<MAX_DOWN_THREAD_NUM; nTtlIndex++)
    {
        if(bChanged[nTtlIndex])
        {
            gMeterFileAddDelFlagTemp[nTtlIndex] = 1;
        }       
    
        if(bChanged[nTtlIndex])
        {
            CCOUP_FMT_DEBUG("端口[%d] 全局档案列表 gAmrMeterFilesListTemp[%d]变化 刷新一下: \n", nTtlIndex, nTtlIndex);
            PrintFilesList(gAmrMeterFilesListTemp[nTtlIndex], 0xaa);

            //档案更新, 存入私有文件
            WriteFilesToPData(nTtlIndex);
        }       
    }

    //消息组帧发至消息中心
    InfoSimpleAck(InfoMsgData, TRUE);

}

/*******************************************************************************
* 函数名称:	InfoDeleteMeterFiles
* 函数功能:	删除表档案
* 输入参数:	
* 输出参数:	
* 返 回 值: 
*******************************************************************************/
#define MAX_DELETE_NUM 200
void InfoDeleteMeterFiles(INFO_DATA_T InfoMsgData)
{
    CCOUP_FMT_DEBUG("InfoDeleteMeterFiles()删除表档案...begin \n");
    
    uint8   nTtlIndex = 0;  //不同的端口号对应不同的数组下标
    int     i = 0;
    BOOL    bDeleteResult = FALSE;
    BOOL    bChanged[MAX_DOWN_THREAD_NUM] = {0};
    uint8  aDeleteFilesData[MAX_DELETE_NUM][6];  //待删除的档案地址
    uint16  IndexID = 0;  //偏移位
    int     ret = 0;
    uint32  nPortID = 0;  //端口号

    uint16  nFilesIndex = 0;//档案位置，从1开始

    //解析消息MSG
    CharArrToUint32(&InfoMsgData.aPayloadData[IndexID], &nPortID);  //端口号
    ret = PortIDToArrIndex(nPortID, &nTtlIndex);
    if(ret < 0)
    {
        //消息组帧发至消息中心
        InfoSimpleAck(InfoMsgData, FALSE);
        return;
    }
    IndexID += 4;
    
    uint16   nDeleteFilesNum = 0;  //删除的电能表档案个数
    BuffToChangeLen(&InfoMsgData.aPayloadData[IndexID], &IndexID, &nDeleteFilesNum);
    if(nDeleteFilesNum > MAX_DELETE_NUM)
    {
        nDeleteFilesNum = MAX_DELETE_NUM;
    }
    CCOUP_FMT_DEBUG("InfoDeleteMeterFiles()删除表档案...删除数量nDeleteFilesNum:%d \n", nDeleteFilesNum);
    
    for(i=0; i<nDeleteFilesNum; i++)
    {
        memcpy_r(aDeleteFilesData[i], &InfoMsgData.aPayloadData[IndexID], 6);
        IndexID += 2;
        CCOUP_FMT_DEBUG("InfoDeleteMeterFiles()删除表档案%d...删除的档案地址%02X %02X %02X %02X %02X %02X\n",
                        i, aDeleteFilesData[i][5], aDeleteFilesData[i][4], aDeleteFilesData[i][3], aDeleteFilesData[i][2],
                        aDeleteFilesData[i][1], aDeleteFilesData[i][0]);
    }

    //写入内存
    for(i=0; i<nDeleteFilesNum; i++)
    {
        nFilesIndex = GetIndexByAddr_Files(gAmrMeterFilesListTemp[nTtlIndex], aDeleteFilesData[i]);
        //printx(" i:%d, nTtlIndex:%d, nFilesIndex:%d \n", i, nTtlIndex, nFilesIndex);
        if(nFilesIndex > 0)
        {
            bDeleteResult = ListDeleteByIndex_Files(&gAmrMeterFilesListTemp[nTtlIndex], nFilesIndex);
            //printx(" i:%d, nTtlIndex:%d, nFilesIndex:%d bDeleteResult:%d \n", i, nTtlIndex, nFilesIndex, bDeleteResult);
            if(bDeleteResult)
            {
                bChanged[nTtlIndex] = TRUE;             

                CCOUP_FMT_DEBUG("InfoDeleteMeterFiles()删除表档案...删除的档案序号 aDeleteFilesData[%d]:%d 位于端口[%d] \n", i, aDeleteFilesData[i], nTtlIndex);
                CCOUP_FMT_DEBUG("InfoDeleteMeterFiles()删除表档案...删除之后 gAmrMeterFilesListTemp[%d]的数量nMeterFilesNum:%d \n", nTtlIndex, gAmrMeterFilesListTemp[nTtlIndex].nMeterFilesNum);
            }
        }
    }

    if(bChanged[nTtlIndex])
    {
        gMeterFileAddDelFlagTemp[nTtlIndex] = 1;
    }

    //刷新表档案
    if(bChanged[nTtlIndex])
    {
        CCOUP_FMT_DEBUG("端口[%d] 全局档案列表 gAmrMeterFilesListTemp[%d]变化 刷新一下: \n", nTtlIndex, nTtlIndex);
        PrintFilesList(gAmrMeterFilesListTemp[nTtlIndex], 0xaa);

        //档案更新, 存入私有文件
        WriteFilesToPData(nTtlIndex);
    }
    
    //消息组帧发至消息中心
    InfoSimpleAck(InfoMsgData, TRUE);
    
}

/*******************************************************************************
* 函数名称:	InfoClearMeterFiles
* 函数功能:	清除表档案
* 输入参数:	
* 输出参数:	
* 返 回 值: 
*******************************************************************************/
void InfoClearMeterFiles(INFO_DATA_T InfoMsgData)
{
	uint8   nTtlIndex = 0;  //不同的端口号对应不同的数组下标
	uint32  nPortID = 0;  //端口号
	uint16  IndexID = 0;  //偏移位
	int     ret = 0;
	BOOL    bChanged[MAX_DOWN_THREAD_NUM] = {0};

	//解析消息MSG
    CharArrToUint32(&InfoMsgData.aPayloadData[IndexID], &nPortID);  //端口号
    ret = PortIDToArrIndex(nPortID, &nTtlIndex);
	if(ret < 0)
	{
		//消息组帧发至消息中心
		InfoSimpleAck(InfoMsgData, FALSE);
		return;
	}

	CCOUP_FMT_DEBUG("InfoClearMeterFiles()清除表档案...端口:%d...begin \n", nTtlIndex);

	//写入内存	
	ret = ClearList_Files(&gAmrMeterFilesListTemp[nTtlIndex]);
	if(ret)
	{
		bChanged[nTtlIndex] = TRUE;
	}

	if(bChanged[nTtlIndex])
	{
		gMeterFileClearFlagTemp[nTtlIndex] = 1;
	}

	//刷新表档案
	if(bChanged[nTtlIndex])
	{
		CCOUP_FMT_DEBUG("端口[%d] 全局档案列表 gAmrMeterFilesListTemp[%d]变化 刷新一下: \n", nTtlIndex, nTtlIndex);
		PrintFilesList(gAmrMeterFilesListTemp[nTtlIndex], 0xaa);

		//档案更新, 存入私有文件
		WriteFilesToPData(nTtlIndex);
	}

	//消息组帧发至消息中心
	InfoSimpleAck(InfoMsgData, TRUE);
	
}

/*******************************************************************************
* 函数名称:	InfoCurrentFilesNum
* 函数功能:	当前档案数量（具体某个端口号）
* 输入参数:	
* 输出参数:	
* 返 回 值: 
*******************************************************************************/
void InfoCurrentFilesNum(INFO_DATA_T InfoMsgData)
{
	CCOUP_FMT_DEBUG("InfoCurrentFilesNum()当前档案数量...begin \n");

	uint8   nTtlIndex = 0;  //不同的端口号对应不同的数组下标
	uint32  nPortID = 0;  //端口号
	uint32  nFilesNum = 0;  //档案数量
	uint16  IndexID = 0;  //偏移位
	int     ret = 0;

	INFO_DATA_T SendInfoMsgData;
	memset((uint8*)&SendInfoMsgData, 0x00, sizeof(SendInfoMsgData));

	//解析消息MSG
    CharArrToUint32(&InfoMsgData.aPayloadData[IndexID], &nPortID);  //端口号
    ret = PortIDToArrIndex(nPortID, &nTtlIndex);
	if(ret < 0)
	{
		//消息组帧发至消息中心
		InfoEmptyAck(InfoMsgData);
		return;
	}

	//从内存中获取档案数量
	nFilesNum = ListLength_Files(gAmrMeterFilesListTemp[nTtlIndex]);
	
	//消息组帧发至消息中心
	IndexID = 0;
	memcpy(&SendInfoMsgData, &InfoMsgData, sizeof(MSG_INFO_T));
	SendInfoMsgData.MsgRPM = 0;
	
	Uint32ToCharArr(nFilesNum, &SendInfoMsgData.aPayloadData[IndexID]);
	IndexID = 4;
	SendInfoMsgData.nPayloadLength = IndexID;
	
    PlcWriteQueue(pAmrSendUp, (uint8 *)&SendInfoMsgData, sizeof(MSG_INFO_T)+SendInfoMsgData.nPayloadLength);
	
}

/*******************************************************************************
* 函数名称:	InfoQueryFilesDetail
* 函数功能:	查询档案信息
* 输入参数:	
* 输出参数:	
* 返 回 值: 
*******************************************************************************/
void InfoQueryFilesDetail(INFO_DATA_T InfoMsgData)
{
    CCOUP_FMT_DEBUG("InfoQueryFilesDetail()查询档案信息...begin \n");
    
    uint8   nTtlIndex = 0;  //不同的端口号对应不同的数组下标
    int     i = 0;
    uint32  nPortID = 0;  //端口号
    uint16  nBeginFilesID = 0;  //档案序号起始，从1开始
    uint16  nFilesNum = 0;  //查询档案数
    uint16  IndexID = 0;  //偏移位
    uint16  nFactQueryNum = 0;
    int     ret = 0;
    
    AMR_METER_FILES_INFO_T stFilesInfo;
    memset((uint8 *)&stFilesInfo, 0x00, sizeof(stFilesInfo));

    INFO_DATA_T SendInfoMsgData;
    memset((uint8 *)&SendInfoMsgData, 0x00, sizeof(SendInfoMsgData));

    //解析消息MSG
    CharArrToUint32(&InfoMsgData.aPayloadData[IndexID], &nPortID);  //端口号
    ret = PortIDToArrIndex(nPortID, &nTtlIndex);
    if(ret < 0)
    {
        //消息组帧发至消息中心
        InfoEmptyAck(InfoMsgData);
        return;
    }

    IndexID += 4;
    CharArrToUint16(&InfoMsgData.aPayloadData[IndexID], &nBeginFilesID);
    nBeginFilesID += 1; //档案序号起始，从1开始
    IndexID += 2;
    CharArrToUint16(&InfoMsgData.aPayloadData[IndexID], &nFilesNum);  //查询档案数
    IndexID += 2;

    //消息组帧发至消息中心
    IndexID = 0;
    memcpy(&SendInfoMsgData, &InfoMsgData, sizeof(MSG_INFO_T));
    SendInfoMsgData.MsgRPM = 0;

    //判断查询的起始序号和数量是否合理
    if((nBeginFilesID <= 0) || (nBeginFilesID > gAmrMeterFilesListTemp[nTtlIndex].nMeterFilesNum))
    {
        nFactQueryNum = 0;
    }   
    else if((nBeginFilesID+nFilesNum-1) <= gAmrMeterFilesListTemp[nTtlIndex].nMeterFilesNum)
    {
        nFactQueryNum = nFilesNum;
    }
    else
    {
        nFactQueryNum = gAmrMeterFilesListTemp[nTtlIndex].nMeterFilesNum - nBeginFilesID + 1 ;
    }

    //实际能够查询的档案数量
    ChangeLenToBuff(nFactQueryNum, &IndexID, &SendInfoMsgData.aPayloadData[IndexID]);  //档案的个数
    for(i=0; i<nFactQueryNum; i++)
    {
        if(GetElem_Files(gAmrMeterFilesListTemp[nTtlIndex], nBeginFilesID+i, &stFilesInfo))
        {
            //Uint16ToCharArr(stFilesInfo.stAmrMeterFiles.nMeterFilesID, &SendInfoMsgData.aPayloadData[IndexID]);  //档案序号
            //IndexID += 2;
            //IndexID += 1;  //跳过地址长度
            memcpy_r(&SendInfoMsgData.aPayloadData[IndexID], stFilesInfo.stAmrMeterFiles.aMeterFilesAddr, 6);  //档案地址
            IndexID += 6;
            //IndexID += 1;  //跳过mac长度
            memcpy_r(&SendInfoMsgData.aPayloadData[IndexID], stFilesInfo.stAmrMeterFiles.aMeterFilesMac, 6);  //档案mac
            IndexID += 6;
            SendInfoMsgData.aPayloadData[IndexID] = stFilesInfo.stAmrMeterFiles.nProtocolType;  //协议类型
            IndexID += 1;
            //SendInfoMsgData.aPayloadData[IndexID] = stFilesInfo.stAmrMeterFiles.nCommPara;  //通信参数
            //IndexID += 1;
        }
        
    }
    SendInfoMsgData.nPayloadLength = IndexID;
    
    PlcWriteQueue(pAmrSendUp, (uint8 *)&SendInfoMsgData, sizeof(MSG_INFO_T)+SendInfoMsgData.nPayloadLength);
    
}

/*******************************************************************************
* 函数名称:	InfoRouterDeal
* 函数功能:	路由操作
* 输入参数:	
* 输出参数:	
* 返 回 值: 
*******************************************************************************/
void InfoRouterDeal(INFO_DATA_T InfoMsgData)
{
	CCOUP_FMT_DEBUG("InfoRouterDeal()路由操作...begin \n");
	
	uint32		nPortID = 0;  //端口号
	uint16		IndexID = 0;  //偏移位
	int 		ret 	= 0;
	uint8 nTtlIndex = 0;  //不同的端口号对应不同的数组下标

	//解析消息MSG
	IndexID = 0;
	CharArrToUint32(&InfoMsgData.aPayloadData[IndexID], &nPortID);	//端口号
	ret = PortIDToArrIndex(nPortID, &nTtlIndex);
	if(ret < 0)
	{
		//消息组帧发至消息中心
		InfoSimpleAck(InfoMsgData, FALSE);
		return;
	}
	switch (InfoMsgData.IOP_ID)
	{
		case 0x0040://暂停抄表
		{
			gRoutrtCtrl[nTtlIndex] = RCS_PAUSE;
			break;
		}
		case 0x0041://恢复抄表
		{
			gRoutrtCtrl[nTtlIndex] = RCS_RESUME;
			break;
		}
		case 0x0042://重启抄表
		{
			gRoutrtCtrl[nTtlIndex] = RCS_RESTART;
			break;
		}
		default:
			//消息组帧发至消息中心
			InfoSimpleAck(InfoMsgData, FALSE);
			return;
			break;
	}

	//消息组帧发至消息中心
	InfoSimpleAck(InfoMsgData, TRUE);

}



/*******************************************************************************
* 函数名称:	InfoActiveMeter
* 函数功能:	主动抄表
* 输入参数:	
* 输出参数:	
* 返 回 值: 
*******************************************************************************/
void InfoActiveMeter(INFO_DATA_T InfoMsgData)
{   
    uint16  IndexID = 0;  //偏移位
    uint32  nPortID = 0;  //端口号
    uint8   nTtlIndex = 0;  //不同的端口号对应不同的数组下标
    int     ret = 0;
    AMR_MODE_ACTIVE_T AmrModeActive;
    memset(&AmrModeActive, 0x00, sizeof(AMR_MODE_ACTIVE_T));

    //解析消息MSG
    CharArrToUint32(&InfoMsgData.aPayloadData[IndexID], &nPortID);  //端口号
    ret = PortIDToArrIndex(nPortID, &nTtlIndex);
    if(ret < 0)
    {
        //消息组帧发至消息中心
        InfoSimpleAck(InfoMsgData, FALSE);
        return;
    }

    IndexID = 0;
    CharArrToUint32(&InfoMsgData.aPayloadData[IndexID], &AmrModeActive.nPortID);  //端口号
    IndexID += 4;
    //CharArrToUint16(&InfoMsgData.aPayloadData[IndexID], &AmrModeActive.nMeterFilesID);  //档案序号
    //IndexID += 2;
    AmrModeActive.comDcb.baud = InfoMsgData.aPayloadData[IndexID];  //波特率
    IndexID += 1;
    AmrModeActive.comDcb.ctrl = InfoMsgData.aPayloadData[IndexID];  //控制位
    IndexID += 1;
    //IndexID += 1;  //跳过地址长度
    memcpy_r(AmrModeActive.nMeterAddr, &InfoMsgData.aPayloadData[IndexID], 6);  //表计地址
    IndexID += 6;
    BuffToChangeLen(&InfoMsgData.aPayloadData[IndexID], &IndexID, &AmrModeActive.nDataBufLen);  //报文可变长度
    //长度容错
    if(AmrModeActive.nDataBufLen > MAX_MSG_PAYLOAD_LEN)
    {
        CCOUP_FMT_DEBUG("InfoActiveMeter()主动抄表 端口[%d] 抄表报文长度异常! AmrModeActive.nDataBufLen:%d, MAX_MSG_PAYLOAD_LEN=%d \n", 
            nTtlIndex, AmrModeActive.nDataBufLen, MAX_MSG_PAYLOAD_LEN);
        //消息组帧发至消息中心
        InfoSimpleAck(InfoMsgData, FALSE);
        return;
    }
    memcpy(AmrModeActive.aDataBuf, &InfoMsgData.aPayloadData[IndexID], AmrModeActive.nDataBufLen);  //抄表报文
    
    ret = AddAmrIntoList(AmrModeActive, nTtlIndex);
    if(ret > 0)
    {
        CCOUP_FMT_DEBUG("InfoActiveMeter()主动抄表 端口[%d] 抄表信息为: \n", nTtlIndex);
        PrintAmrActiveDetail(AmrModeActive);
    }

    //消息组帧发至消息中心
    InfoSimpleAck(InfoMsgData, TRUE);

    //更新抄表状态的最近一次接收时间
    gAmrStateQuery[nTtlIndex].nLastRecvTime = time(NULL);
    
}

/*******************************************************************************
* 函数名称:	InfoSearchMeter
* 函数功能:	启动搜表
* 输入参数:	
* 输出参数:	
* 返 回 值: 
*******************************************************************************/
void InfoSearchMeter(INFO_DATA_T InfoMsgData)
{
	CCOUP_FMT_DEBUG("InfoSearchMeter()启动搜表...begin \n");
	
	uint16                    IndexID = 0;  //偏移位
	uint32                    uint32_temp;
	int                       ret = 0;
	uint8 nTtlIndex = 0;  //不同的端口号对应不同的数组下标

	AMR_METER_START_SEARCH_T       AmrMeterStartSearch;            //启动搜表
	PLCTASK_DETAIL                 stPlcTaskDetail;                //载波任务具体信息
	memset(&AmrMeterStartSearch, 0, sizeof(AmrMeterStartSearch));
	memset(&stPlcTaskDetail, 0, sizeof(stPlcTaskDetail));

	//解析消息MSG
	CharArrToUint32(&InfoMsgData.aPayloadData[IndexID], &AmrMeterStartSearch.nPortID);
	ret = PortIDToArrIndex(AmrMeterStartSearch.nPortID, &nTtlIndex);
	if(ret < 0)
	{
		//消息组帧发至消息中心
		InfoSimpleAck(InfoMsgData, FALSE);
		return;
	}
	IndexID += 4;
	CharArrToUint32(&InfoMsgData.aPayloadData[IndexID], &uint32_temp);
	AmrMeterStartSearch.nStartTime = (int32)uint32_temp;
    if(AmrMeterStartSearch.nStartTime >= (8*60*60))
    {
        AmrMeterStartSearch.nStartTime -= 8*60*60;//采集调度时间戳的这种都用cst的0点, 同步修改
    }
    else
    {
        AmrMeterStartSearch.nStartTime = 0;
    }
	IndexID += 4;
	CharArrToUint32(&InfoMsgData.aPayloadData[IndexID], &uint32_temp);
	AmrMeterStartSearch.nContinuedTime = (int32)uint32_temp;
	IndexID += 4;
	AmrMeterStartSearch.nSlaveNodeReSendCount = InfoMsgData.aPayloadData[IndexID];
	IndexID += 1;
	AmrMeterStartSearch.nRandWaitTimeSlice = InfoMsgData.aPayloadData[IndexID];
	IndexID += 1;

	PrintSearchMeterDetail(AmrMeterStartSearch);

	//创建任务
	gWhetherStartSearchMeter[nTtlIndex].bStartSearch = TRUE;
	gWhetherStartSearchMeter[nTtlIndex].stAmrMeterStartSearch = AmrMeterStartSearch;

	//消息组帧发至消息中心
	InfoSimpleAck(InfoMsgData, TRUE);

}

/*******************************************************************************
* 函数名称:	InfoPauseSearchMeter
* 函数功能:	暂停搜表
* 输入参数:	
* 输出参数:	
* 返 回 值: 
*******************************************************************************/
void InfoPauseSearchMeter(INFO_DATA_T InfoMsgData)
{
	CCOUP_FMT_DEBUG("InfoPauseSearchMeter()暂停搜表...begin \n");
	
	int                       ret = 0;
	uint16                    IndexID = 0;  //偏移位
	uint32 nPortID = 0;  //端口号
	uint8 nTtlIndex = 0;  //不同的端口号对应不同的数组下标

	//解析消息MSG
	CharArrToUint32(&InfoMsgData.aPayloadData[IndexID], &nPortID);
	ret = PortIDToArrIndex(nPortID, &nTtlIndex);
	if(ret < 0)
	{
		//消息组帧发至消息中心
		InfoSimpleAck(InfoMsgData, FALSE);
		return;
	}
	IndexID += 4;

	//写入内存
	gPauseSearchMeterFlag[nTtlIndex] = TRUE;

	//消息组帧发至消息中心
	InfoSimpleAck(InfoMsgData, TRUE);
	
}


/*******************************************************************************
* 函数名称:	InfoDataTransmit
* 函数功能:	数据透传
* 输入参数:	
* 输出参数:	
* 返 回 值: 
*******************************************************************************/
void InfoDataTransmit(INFO_DATA_T InfoMsgData)
{
	CCOUP_FMT_DEBUG("InfoDataTransmit()数据透传...begin \n");
	
	uint16                    IndexID = 0;  //偏移位
	AMR_DATA_TRANSMIT_T       AmrDataTransmit;            //数据透传
	uint8                     nTtlIndex = 0;  //不同的端口号对应不同的数组下标
	int                       ret = 0;
	INFO_DATA_HEAD_T InfoDataHead;
	
	memset((uint8 *)&AmrDataTransmit, 0x00, sizeof(AmrDataTransmit));
	memcpy(&InfoDataHead, &InfoMsgData, sizeof(InfoDataHead));

	//解析消息MSG
	CharArrToUint32(&InfoMsgData.aPayloadData[IndexID], &AmrDataTransmit.nPortID);  //端口号
	ret = PortIDToArrIndex(AmrDataTransmit.nPortID, &nTtlIndex);
	if(ret < 0)
	{
		//消息组帧发至消息中心
		InfoEmptyAck(InfoMsgData);
		return;
	}
	IndexID += 4;
	AmrDataTransmit.nCommSymbol = InfoMsgData.aPayloadData[IndexID];  //通讯字
	IndexID += 2;
	CharArrToUint16(&InfoMsgData.aPayloadData[IndexID], &AmrDataTransmit.nFrameTimeover);  //帧超时时间
	IndexID += 2;
	CharArrToUint16(&InfoMsgData.aPayloadData[IndexID], &AmrDataTransmit.nCharTimeover);  //字符超时时间
	IndexID += 2;
	BuffToChangeLen(&InfoMsgData.aPayloadData[IndexID], &IndexID, &AmrDataTransmit.nTransmitDataLen);  //可变长
	memcpy(AmrDataTransmit.pTransmitDataBuf, &InfoMsgData.aPayloadData[IndexID], AmrDataTransmit.nTransmitDataLen);  //数据帧

	//创建任务
	AddDataTransmitList(nTtlIndex, AmrDataTransmit, InfoDataHead);
	
}

/*******************************************************************************
* 函数名称:	InfoGetSlaveMonitorDelayTime
* 函数功能:	获取从节点监控延时
* 输入参数:	
* 输出参数:	
* 返 回 值: 
*******************************************************************************/
void InfoGetSlaveMonitorDelayTime(INFO_DATA_T InfoMsgData)
{
	CCOUP_FMT_DEBUG("InfoGetSlaveMonitorDelayTime()获取从节点监控延时...begin \n");

	uint16                    IndexID = 0;  //偏移位
	AMR_SLAVE_NODE_DELAY_TIME_T       AmrSlaveNodeMonitorTime;  //从节点监控
	uint8                     nTtlIndex = 0;  //不同的端口号对应不同的数组下标
	int                       ret = 0;
	INFO_DATA_HEAD_T InfoDataHead;
	
	memset((uint8 *)&AmrSlaveNodeMonitorTime, 0x00, sizeof(AMR_SLAVE_NODE_DELAY_TIME_T));
	memcpy(&InfoDataHead, &InfoMsgData, sizeof(INFO_DATA_HEAD_T));

	//解析消息MSG
	CharArrToUint32(&InfoMsgData.aPayloadData[IndexID], &AmrSlaveNodeMonitorTime.nPortID);  //端口号
	ret = PortIDToArrIndex(AmrSlaveNodeMonitorTime.nPortID, &nTtlIndex);
	if(ret < 0)
	{
		//消息组帧发至消息中心
		InfoEmptyAck(InfoMsgData);
		return;
	}
	IndexID += 4;
	memcpy_r(AmrSlaveNodeMonitorTime.aSlaveNodeAddr, &InfoMsgData.aPayloadData[IndexID], 6);  //载波从节点地址
	IndexID += 6;
	AmrSlaveNodeMonitorTime.nProtocolType = InfoMsgData.aPayloadData[IndexID];  //规约类型
	IndexID += 1;
	BuffToChangeLen(&InfoMsgData.aPayloadData[IndexID], &IndexID, &AmrSlaveNodeMonitorTime.nDataBufLen);  //可变长
	memcpy(AmrSlaveNodeMonitorTime.aDataBuf, &InfoMsgData.aPayloadData[IndexID], AmrSlaveNodeMonitorTime.nDataBufLen);  //数据帧

	//创建任务
	AddSlaveMonitorTimeList(nTtlIndex, AmrSlaveNodeMonitorTime, InfoDataHead);
	
}


/*******************************************************************************
* 函数名称:	InfoSlaveMonitor
* 函数功能:	从节点监控
* 输入参数:	
* 输出参数:	
* 返 回 值: 
*******************************************************************************/
void InfoSlaveMonitor(INFO_DATA_T InfoMsgData)
{
    CCOUP_FMT_DEBUG("InfoSlaveMonitor()从节点监控...begin \n");
    
    uint16                    IndexID = 0;  //偏移位
    AMR_SLAVE_NODE_MONITOR_T       AmrSlaveNodeMonitor;  //从节点监控
    uint8                     nTtlIndex = 0;  //不同的端口号对应不同的数组下标
    int                       ret = 0;
    INFO_DATA_HEAD_T InfoDataHead;
    
    memset((uint8 *)&AmrSlaveNodeMonitor, 0x00, sizeof(AmrSlaveNodeMonitor));
    memcpy(&InfoDataHead, &InfoMsgData, sizeof(InfoDataHead));

    //解析消息MSG
    CharArrToUint32(&InfoMsgData.aPayloadData[IndexID], &AmrSlaveNodeMonitor.nPortID);  //端口号
    ret = PortIDToArrIndex(AmrSlaveNodeMonitor.nPortID, &nTtlIndex);
    if(ret < 0)
    {
        //消息组帧发至消息中心
        InfoEmptyAck(InfoMsgData);
        return;
    }
    IndexID += 4;
    AmrSlaveNodeMonitor.nProtocolType = InfoMsgData.aPayloadData[IndexID];  //规约类型
    IndexID += 1;
    memcpy_r(AmrSlaveNodeMonitor.aSlaveNodeAddr, &InfoMsgData.aPayloadData[IndexID], 6);  //载波从节点地址
    IndexID += 6;
    AmrSlaveNodeMonitor.comDcb.baud = InfoMsgData.aPayloadData[IndexID];  //波特率
    IndexID += 1;
    AmrSlaveNodeMonitor.comDcb.ctrl = InfoMsgData.aPayloadData[IndexID];  //控制位
    IndexID += 1;
    CharArrToUint16(&InfoMsgData.aPayloadData[IndexID], &AmrSlaveNodeMonitor.nFrameTimeover);  //帧超时时间
    IndexID += 2;
    CharArrToUint16(&InfoMsgData.aPayloadData[IndexID], &AmrSlaveNodeMonitor.nByteTimeover);  //字符超时时间
    IndexID += 2;
    BuffToChangeLen(&InfoMsgData.aPayloadData[IndexID], &IndexID, &AmrSlaveNodeMonitor.nDataBufLen);  //可变长
    memcpy(AmrSlaveNodeMonitor.aDataBuf, &InfoMsgData.aPayloadData[IndexID], AmrSlaveNodeMonitor.nDataBufLen);  //数据帧

    //创建任务
    AddSlaveMonitorList(nTtlIndex, AmrSlaveNodeMonitor, InfoDataHead);
    
}

/*******************************************************************************
* 函数名称:	InfoSetTmnAddr
* 函数功能:	模块地址设置
* 输入参数:	
* 输出参数:	
* 返 回 值: 
*******************************************************************************/
void InfoSetTmnAddr(INFO_DATA_T InfoMsgData)
{
	CCOUP_FMT_DEBUG("InfoSetTmnAddr()模块地址设置...begin \n");
	
	uint32      nPortID = 0;  //端口号
    uint16      IndexID = 0;  //偏移位
    int         ret     = 0;
	uint8       nTtlIndex = 0;  //不同的端口号对应不同的数组下标
	uint8       aAddrTemp[6] = {0};  //逆序备份消息里的终端地址

	//解析消息MSG
	IndexID = 0;
	CharArrToUint32(&InfoMsgData.aPayloadData[IndexID], &nPortID);	//端口号
	ret = PortIDToArrIndex(nPortID, &nTtlIndex);
	if(ret < 0)
	{
		//消息组帧发至消息中心
		InfoSimpleAck(InfoMsgData, FALSE);
		return;
	}
	IndexID += 4;

	//解析消息MSG
	memcpy_r(aAddrTemp, &InfoMsgData.aPayloadData[IndexID], 6);  //逆序备份消息里的终端地址
	ret = memcmp(&gTerminalAddr[nTtlIndex][0], aAddrTemp, 6);
	if(ret != 0)
	{
		gUpdateTerminalAddrFlag[nTtlIndex] = TRUE;
		memcpy(&gTerminalAddr[nTtlIndex][0], aAddrTemp, 6);  //模块地址
		CCOUP_FMT_DEBUG("InfoSetTmnAddr()模块地址设置...端口[%d]模块地址变化! \n", nTtlIndex);

        //将终端地址保存至私有文件
		ret = clear_pdata_xram(gModuleManuName[nTtlIndex]);
		CCODOWN_FMT_DEBUG(nTtlIndex, "gTerminalAddr() Ret = clear_pdata_xram() Ret:%d...\n", ret);
		
		ret = write_pdata_xram(gModuleManuName[nTtlIndex], (char *)&gTerminalAddr[nTtlIndex][0], 0, sizeof(gTerminalAddr[nTtlIndex]));
		CCODOWN_FMT_DEBUG(nTtlIndex, "gTerminalAddr() Ret = write_pdata_xram() Ret:%d...\n", ret);
	}
	IndexID += 6;

	//消息组帧发至消息中心
	InfoSimpleAck(InfoMsgData, TRUE);
	
}


/*******************************************************************************
* 函数名称:	InfoGetBroadcastDelayTime
* 函数功能:	获取广播延时
* 输入参数:	
* 输出参数:	
* 返 回 值: 
*******************************************************************************/
void InfoGetBroadcastDelayTime(INFO_DATA_T InfoMsgData)
{
	CCOUP_FMT_DEBUG("InfoGetBroadcastDelayTime()获取广播延时...begin \n");
	
	uint16                    IndexID = 0;  //偏移位
	//uint32                    uint32_temp;
	int                       ret = 0;
	uint8 nTtlIndex = 0;  //不同的端口号对应不同的数组下标

	AMR_GET_BROADCAST_DELAY_T       AmrGetBroadcastDelay;            //获取广播延时
	PLCTASK_DETAIL                  stPlcTaskDetail;                //载波任务具体信息
	memset(&AmrGetBroadcastDelay, 0, sizeof(AmrGetBroadcastDelay));
	memset(&stPlcTaskDetail, 0, sizeof(stPlcTaskDetail));

	//解析消息MSG
	CharArrToUint32(&InfoMsgData.aPayloadData[IndexID], &AmrGetBroadcastDelay.nPortID);  //端口号
	ret = PortIDToArrIndex(AmrGetBroadcastDelay.nPortID, &nTtlIndex);
	if(ret < 0)
	{
		//消息组帧发至消息中心
		InfoEmptyAck(InfoMsgData);
		return;
	}
	IndexID += 4;
	AmrGetBroadcastDelay.nProtocolType = InfoMsgData.aPayloadData[IndexID];  //规约
	IndexID += 1;
	BuffToChangeLen(&InfoMsgData.aPayloadData[IndexID], &IndexID, &AmrGetBroadcastDelay.nDataBufLen);  //可变长
	memcpy(AmrGetBroadcastDelay.aDataBuf, &InfoMsgData.aPayloadData[IndexID], AmrGetBroadcastDelay.nDataBufLen);  //数据帧

	//创建任务
	gWhetherGetBroadcastDelay[nTtlIndex].bGetBroadcastDelay = TRUE;
	memcpy(&gWhetherGetBroadcastDelay[nTtlIndex].InfoDataHead, &InfoMsgData, sizeof(INFO_DATA_HEAD_T));
	gWhetherGetBroadcastDelay[nTtlIndex].stAmrGetBroadcastDelay = AmrGetBroadcastDelay;

	
}

/*******************************************************************************
* 函数名称:	InfoBroadcastDeal
* 函数功能:	广播
* 输入参数:	
* 输出参数:	
* 返 回 值: 
*******************************************************************************/
void InfoBroadcastDeal(INFO_DATA_T InfoMsgData)
{
    CCOUP_FMT_DEBUG("InfoBroadcastDeal()广播...begin \n");
    
    uint16                    IndexID = 0;  //偏移位
    //uint32                    uint32_temp;
    int                       ret = 0;
    uint8 nTtlIndex = 0;  //不同的端口号对应不同的数组下标

    AMR_DO_BROADCAST_T       AmrDoBroadcast;            //启动广播
    PLCTASK_DETAIL                  stPlcTaskDetail;                //载波任务具体信息
    memset(&AmrDoBroadcast, 0, sizeof(AmrDoBroadcast));
    memset(&stPlcTaskDetail, 0, sizeof(stPlcTaskDetail));

    //解析消息MSG
    CharArrToUint32(&InfoMsgData.aPayloadData[IndexID], &AmrDoBroadcast.nPortID);  //端口号
    ret = PortIDToArrIndex(AmrDoBroadcast.nPortID, &nTtlIndex);
    if(ret < 0)
    {
        //消息组帧发至消息中心
        InfoSimpleAck(InfoMsgData, FALSE);
        return;
    }
    IndexID += 4;
    AmrDoBroadcast.nProtocolType = InfoMsgData.aPayloadData[IndexID];  //规约
    IndexID += 1;
    AmrDoBroadcast.comDcb.baud = InfoMsgData.aPayloadData[IndexID];  //波特率
    IndexID += 1;
    AmrDoBroadcast.comDcb.ctrl = InfoMsgData.aPayloadData[IndexID];  //控制位
    IndexID += 1;
    BuffToChangeLen(&InfoMsgData.aPayloadData[IndexID], &IndexID, &AmrDoBroadcast.nDataBufLen);  //可变长
    memcpy(AmrDoBroadcast.aDataBuf, &InfoMsgData.aPayloadData[IndexID], AmrDoBroadcast.nDataBufLen);  //数据帧

    //创建任务
    gWhetherDoBroadcast[nTtlIndex].bDoBroadcast = TRUE;
    gWhetherDoBroadcast[nTtlIndex].stAmrDoBroadcast = AmrDoBroadcast;

    //消息组帧发至消息中心
    InfoSimpleAck(InfoMsgData, TRUE);
    
}

/*******************************************************************************
* 函数名称:	InfoQueryHPLC
* 函数功能:	查询HPLC信息
* 输入参数:	
* 输出参数:	
* 返 回 值: 
*******************************************************************************/
void InfoQueryHPLC(INFO_DATA_T InfoMsgData)
{
	CCOUP_FMT_DEBUG("InfoQueryHPLC()查询HPLC信息...begin \n");
	
	uint16                    IndexID = 0;  //偏移位
	int                       ret = 0;
	uint8 nTtlIndex = 0;  //不同的端口号对应不同的数组下标

	AMR_HPLC_INFO_QUERY_T       AmrHplcInfoQuery;            //查询HPLC信息
	memset(&AmrHplcInfoQuery, 0, sizeof(AMR_HPLC_INFO_QUERY_T));

	//解析消息MSG
	CharArrToUint32(&InfoMsgData.aPayloadData[IndexID], &AmrHplcInfoQuery.nPortID);  //端口号
	ret = PortIDToArrIndex(AmrHplcInfoQuery.nPortID, &nTtlIndex);
	if(ret < 0)
	{
		//消息组帧发至消息中心
		InfoSimpleAck(InfoMsgData, FALSE);
		return;
	}
	IndexID += 4;
	AmrHplcInfoQuery.nInfoType = InfoMsgData.aPayloadData[IndexID];  //信息类型
	IndexID += 1;

	//判断hplc查询choice是否合法
	if((AmrHplcInfoQuery.nInfoType <= 0) ||	(AmrHplcInfoQuery.nInfoType >= HPLC_LAST))
	{
		CCOUP_FMT_DEBUG("AmrHplcInfoQuery.nInfoType error! AmrHplcInfoQuery.nInfoType=%d \n", AmrHplcInfoQuery.nInfoType);
		//消息组帧发至消息中心
		InfoSimpleAck(InfoMsgData, FALSE);
		return;
	}

	//创建任务
	switch(AmrHplcInfoQuery.nInfoType)
	{
		case HPLC_TOPOLOGYINFO:
		{
			gQueryTypeHPLC[nTtlIndex].BitType.topo = 1;
			break;
		}
		case HPLC_CHIPINFO:
		{
			gQueryTypeHPLC[nTtlIndex].BitType.chip = 1;
			break;
		}
		case HPLC_NETSINFO:
		{
			gQueryTypeHPLC[nTtlIndex].BitType.nets = 1;
			break;
		}
		case HPLC_PHASEINFO:
		{
			gQueryTypeHPLC[nTtlIndex].BitType.phase = 1;
			break;
		}
		case HPLC_SCALE:
		{
			gQueryTypeHPLC[nTtlIndex].BitType.scale = 1;
			break;
		}
		case HPLC_MASTERID:
		{
			gQueryTypeHPLC[nTtlIndex].BitType.master = 1;
			break;
		}
		case HPLC_SLAVEID:
		{
			gQueryTypeHPLC[nTtlIndex].BitType.slave = 1;
			break;
		}
		case HPLC_MODULEINFO:
		{
			gQueryTypeHPLC[nTtlIndex].BitType.module = 1;
			break;
		}
		case HPLC_NODEVERSION:
		{
			gQueryTypeHPLC[nTtlIndex].BitType.nodeversion = 1;
			break;
		}
		default:
		{
			CCOUP_FMT_DEBUG("AmrHplcInfoQuery.nInfoType error! AmrHplcInfoQuery.nInfoType=%d \n", AmrHplcInfoQuery.nInfoType);
			//消息组帧发至消息中心
			InfoSimpleAck(InfoMsgData, FALSE);
			return;
		}
	}

	//消息组帧发至消息中心
	InfoSimpleAck(InfoMsgData, TRUE);
	
}

/*******************************************************************************
* 函数名称:	InfoFrSet
* 函数功能:	HPLC频点设置
* 输入参数:	
* 输出参数:	
* 返 回 值: 
*******************************************************************************/
void InfoFrSet(INFO_DATA_T InfoMsgData)
{
	CCOUP_FMT_DEBUG("InfoFrSet()HPLC频点设置...begin \n");
	
	uint8   nTtlIndex = 0;  //不同的端口号对应不同的数组下标
	uint32  nPortID = 0;  //端口号
	uint16  IndexID = 0;  //偏移位
	int     ret = 0;

	//解析消息MSG
	CharArrToUint32(&InfoMsgData.aPayloadData[IndexID], &nPortID);  //端口号
    ret = PortIDToArrIndex(nPortID, &nTtlIndex);
	if(ret < 0)
	{
		//消息组帧发至消息中心
		InfoSimpleAck(InfoMsgData, FALSE);
		return;
	}

	IndexID += 4;
	gFrSetFlag[nTtlIndex] = 1;
	gFrSetValue[nTtlIndex] = InfoMsgData.aPayloadData[IndexID];

	//消息组帧发至消息中心
	InfoSimpleAck(InfoMsgData, TRUE);
}

/*******************************************************************************
* 函数名称:	InfoFrQuery
* 函数功能:	HPLC频点查询
* 输入参数:	
* 输出参数:	
* 返 回 值: 
*******************************************************************************/
void InfoFrQuery(INFO_DATA_T InfoMsgData)
{
	CCOUP_FMT_DEBUG("InfoFrQuery()HPLC频点查询...begin \n");
	
	uint8   nTtlIndex = 0;  //不同的端口号对应不同的数组下标
	uint32  nPortID = 0;  //端口号
	uint16  IndexID = 0;  //偏移位
	int     ret = 0;

	//解析消息MSG
	CharArrToUint32(&InfoMsgData.aPayloadData[IndexID], &nPortID);  //端口号
    ret = PortIDToArrIndex(nPortID, &nTtlIndex);
	if(ret < 0)
	{
		//消息组帧发至消息中心
		InfoEmptyAck(InfoMsgData);
		return;
	}

	gFrQueryFlag[nTtlIndex] = 1;  //设置查询标志
	memcpy(&gFrQueryInfoDataHead[nTtlIndex], &InfoMsgData, sizeof(INFO_DATA_HEAD_T));
}

/*******************************************************************************
* 函数名称:	InfoAreaIdentify
* 函数功能:	HPLC台区识别
* 输入参数:	
* 输出参数:	
* 返 回 值: 
*******************************************************************************/
void InfoAreaIdentify(INFO_DATA_T InfoMsgData)
{
	CCOUP_FMT_DEBUG("InfoAreaIdentify()HPLC台区识别...begin \n");
	
	uint8   nTtlIndex = 0;  //不同的端口号对应不同的数组下标
	uint32  nPortID = 0;  //端口号
	uint16  IndexID = 0;  //偏移位
	int     ret = 0;

	//解析消息MSG
	CharArrToUint32(&InfoMsgData.aPayloadData[IndexID], &nPortID);  //端口号
    ret = PortIDToArrIndex(nPortID, &nTtlIndex);
	if(ret < 0)
	{
		//消息组帧发至消息中心
		InfoSimpleAck(InfoMsgData, FALSE);
		return;
	}

	IndexID += 4;
	gAreaIdentifyFlag[nTtlIndex] = 1;
	gAreaIdentifyValue[nTtlIndex] = InfoMsgData.aPayloadData[IndexID];

	//显示结构体赋值
	gGuiShowMessage[nTtlIndex].nAreaIdentifyState = InfoMsgData.aPayloadData[IndexID];

	ret = write_pdata_xram(gGuiInfoName[nTtlIndex], (char *)&gGuiShowMessage[nTtlIndex], 0, sizeof(gGuiShowMessage[nTtlIndex]));
	CCOUP_FMT_DEBUG("InfoAreaIdentify()HPLC台区识别 将显示相关信息写入私有文件, ret=%d \n", ret);

	//消息组帧发至消息中心
	InfoSimpleAck(InfoMsgData, TRUE);
}

/*******************************************************************************
* 函数名称:	InfoOfflineFeel
* 函数功能:	HPLC离网感知
* 输入参数:	
* 输出参数:	
* 返 回 值: 
*******************************************************************************/
void InfoOfflineFeel(INFO_DATA_T InfoMsgData)
{
	CCOUP_FMT_DEBUG("InfoOfflineFeel()HPLC离网感知...begin \n");
	
	uint8   nTtlIndex = 0;  //不同的端口号对应不同的数组下标
	uint32  nPortID = 0;  //端口号
	uint16  IndexID = 0;  //偏移位
	int     ret = 0;

	//解析消息MSG
	CharArrToUint32(&InfoMsgData.aPayloadData[IndexID], &nPortID);  //端口号
    ret = PortIDToArrIndex(nPortID, &nTtlIndex);
	if(ret < 0)
	{
		//消息组帧发至消息中心
		InfoSimpleAck(InfoMsgData, FALSE);
		return;
	}

	IndexID += 4;
	gOfflineFeelFlag[nTtlIndex] = 1;
	gOfflineFeelValue[nTtlIndex] = InfoMsgData.aPayloadData[IndexID];

	//显示结构体赋值
	gGuiShowMessage[nTtlIndex].nAreaIdentifyState = InfoMsgData.aPayloadData[IndexID];

	ret = write_pdata_xram(gGuiInfoName[nTtlIndex], (char *)&gGuiShowMessage[nTtlIndex], 0, sizeof(gGuiShowMessage[nTtlIndex]));
	CCOUP_FMT_DEBUG("InfoAreaIdentify()HPLC离网感知 将显示相关信息写入私有文件, ret=%d \n", ret);

	//消息组帧发至消息中心
	InfoSimpleAck(InfoMsgData, TRUE);
}

/*******************************************************************************
* 函数名称:	InfoRefuseNodeReport
* 函数功能:	HPLC拒绝节点上报
* 输入参数:	
* 输出参数:	
* 返 回 值: 
*******************************************************************************/
void InfoRefuseNodeReport(INFO_DATA_T InfoMsgData)
{
	CCOUP_FMT_DEBUG("InfoRefuseNodeReport()HPLC拒绝节点上报...begin \n");
	
	uint8   nTtlIndex = 0;  //不同的端口号对应不同的数组下标
	uint32  nPortID = 0;  //端口号
	uint16  IndexID = 0;  //偏移位
	int     ret = 0;

	//解析消息MSG
	CharArrToUint32(&InfoMsgData.aPayloadData[IndexID], &nPortID);  //端口号
    ret = PortIDToArrIndex(nPortID, &nTtlIndex);
	if(ret < 0)
	{
		//消息组帧发至消息中心
		InfoSimpleAck(InfoMsgData, FALSE);
		return;
	}

	IndexID += 4;
	gRefuseNodeReportFlag[nTtlIndex] = 1;
	gRefuseNodeReportValue[nTtlIndex] = InfoMsgData.aPayloadData[IndexID];

	//显示结构体赋值
	gGuiShowMessage[nTtlIndex].nAreaIdentifyState = InfoMsgData.aPayloadData[IndexID];

	ret = write_pdata_xram(gGuiInfoName[nTtlIndex], (char *)&gGuiShowMessage[nTtlIndex], 0, sizeof(gGuiShowMessage[nTtlIndex]));
	CCOUP_FMT_DEBUG("InfoAreaIdentify()HPLC拒绝节点上报 将显示相关信息写入私有文件, ret=%d \n", ret);

	//消息组帧发至消息中心
	InfoSimpleAck(InfoMsgData, TRUE);
}


/*******************************************************************************
* 函数名称:	InfoAmrStateQuery
* 函数功能:	抄表状态查询
* 输入参数:	
* 输出参数:	
* 返 回 值: 
*******************************************************************************/
void InfoAmrStateQuery(INFO_DATA_T InfoMsgData)
{
	CCOUP_FMT_DEBUG("InfoAmrStateQuery()抄表状态查询...begin \n");

	uint8   nTtlIndex = 0;  //不同的端口号对应不同的数组下标
	uint32  nPortID = 0;  //端口号
	uint16  IndexID = 0;  //偏移位
	int     ret = 0;

	INFO_DATA_T SendInfoMsgData;
	memset((uint8*)&SendInfoMsgData, 0x00, sizeof(SendInfoMsgData));

	//解析消息MSG
    CharArrToUint32(&InfoMsgData.aPayloadData[IndexID], &nPortID);  //端口号
    ret = PortIDToArrIndex(nPortID, &nTtlIndex);
	if(ret < 0)
	{
		//消息组帧发至消息中心
		InfoEmptyAck(InfoMsgData);
		return;
	}

	//消息组帧发至消息中心
	IndexID = 0;
	memcpy(&SendInfoMsgData, &InfoMsgData, sizeof(MSG_INFO_T));
	SendInfoMsgData.MsgRPM = 0;
	
	memcpy_r(&SendInfoMsgData.aPayloadData[IndexID], (uint8 *)&gAmrStateQuery[nTtlIndex].nLastRecvTime, 4);//最后一次接收时间
	IndexID += 4;
	memcpy_r(&SendInfoMsgData.aPayloadData[IndexID], (uint8 *)&gAmrStateQuery[nTtlIndex].nModuleAmrState, 4);//模块抄表状态
	IndexID += 4;
	memcpy_r(&SendInfoMsgData.aPayloadData[IndexID], (uint8 *)&gAmrStateQuery[nTtlIndex].nModuleSearchState, 4);//模块搜表状态
	IndexID += 4;
	memcpy_r(&SendInfoMsgData.aPayloadData[IndexID], (uint8 *)&gAmrStateQuery[nTtlIndex].nAutoAmrState, 4);//自动抄表状态
	IndexID += 4;
	memcpy_r(&SendInfoMsgData.aPayloadData[IndexID], (uint8 *)&gAmrStateQuery[nTtlIndex].nClearRouterTimes, 2);//清除路由次数
	IndexID += 2;
	memcpy_r(&SendInfoMsgData.aPayloadData[IndexID], (uint8 *)&gAmrStateQuery[nTtlIndex].nDataInitTimes, 2);//数据初始化次数
	IndexID += 2;
	memcpy_r(&SendInfoMsgData.aPayloadData[IndexID], (uint8 *)&gAmrStateQuery[nTtlIndex].nParaInitTimes, 2);//参数初始化次数
	IndexID += 2;
	memcpy_r(&SendInfoMsgData.aPayloadData[IndexID], (uint8 *)&gAmrStateQuery[nTtlIndex].nResetModuleTimes, 2);//复位模块次数
	IndexID += 2;
	SendInfoMsgData.nPayloadLength = IndexID;
	
    PlcWriteQueue(pAmrSendUp, (uint8 *)&SendInfoMsgData, sizeof(MSG_INFO_T)+SendInfoMsgData.nPayloadLength);

	
}

/*******************************************************************************
* 函数名称:	InfoAmrModuleQuery
* 函数功能:	查询抄表模块信息
* 输入参数:	
* 输出参数:	
* 返 回 值: 
*******************************************************************************/
void InfoAmrModuleQuery(INFO_DATA_T InfoMsgData)
{
	CCOUP_FMT_DEBUG("InfoAmrModuleQuery()查询抄表模块信息...begin \n");

	uint8   nTtlIndex = 0;  //不同的端口号对应不同的数组下标
	uint32  nPortID = 0;  //端口号
	uint16  IndexID = 0;  //偏移位
	int     ret = 0;

	INFO_DATA_T SendInfoMsgData;
	memset((uint8*)&SendInfoMsgData, 0x00, sizeof(SendInfoMsgData));

	//解析消息MSG
    CharArrToUint32(&InfoMsgData.aPayloadData[IndexID], &nPortID);  //端口号
    ret = PortIDToArrIndex(nPortID, &nTtlIndex);
	if(ret < 0)
	{
		//消息组帧发至消息中心
		InfoEmptyAck(InfoMsgData);
		return;
	}

	//消息组帧发至消息中心
	IndexID = 0;
	memcpy(&SendInfoMsgData, &InfoMsgData, sizeof(MSG_INFO_T));
	SendInfoMsgData.MsgRPM = 0;
	
	memcpy(&SendInfoMsgData.aPayloadData[IndexID], &gAmrModuleQuery[nTtlIndex].aLocalCommTypeModel[0], 6);//本地通信模式字
	IndexID += 6;
	Uint16ToCharArr(gAmrModuleQuery[nTtlIndex].nSlaveMonitorMaxOvertime, &SendInfoMsgData.aPayloadData[IndexID]);//从节点监控最大超时时间(单位:s)
	IndexID += 2;
	Uint16ToCharArr(gAmrModuleQuery[nTtlIndex].nBroadcastMaxOvertime, &SendInfoMsgData.aPayloadData[IndexID]);//广播命令最大超时时间(单位:s)
	IndexID += 2;
	Uint16ToCharArr(gAmrModuleQuery[nTtlIndex].nMaxAllowedFrameLen, &SendInfoMsgData.aPayloadData[IndexID]);//最大支持的报文长度
	IndexID += 2;
	Uint16ToCharArr(gAmrModuleQuery[nTtlIndex].nMaxUpgradeSingleFrameLen, &SendInfoMsgData.aPayloadData[IndexID]);//文件传输支持的最大单个数据包长度
	IndexID += 2;
	SendInfoMsgData.aPayloadData[IndexID] = gAmrModuleQuery[nTtlIndex].nUpgradeWaitingPeriod;//升级操作等待时间
	IndexID += 1;
	
	IndexID += 2;//跳过 广播成功延时时间

	memcpy_r(&SendInfoMsgData.aPayloadData[IndexID], &gTerminalAddr[nTtlIndex][0], 6);//终端地址(模块编号)
	IndexID += 6;

	Uint16ToCharArr(gAmrModuleQuery[nTtlIndex].nMaxSupportSlaveNum, &SendInfoMsgData.aPayloadData[IndexID]);//支持的最大从节点数量
	IndexID += 2;

	Uint16ToCharArr(gAmrMeterFilesList[nTtlIndex].nMeterFilesNum, &SendInfoMsgData.aPayloadData[IndexID]);//当前从节点数量
	IndexID += 2;

    //组装通信模块厂商代码及版本信息(字符串)
    uint8 factLen = 16;
    char faccode[40] = {0};
    sprintf(faccode, "%C%C%C%C-%02X%02X%02X-%02X%02X", gAmrModuleQuery[nTtlIndex].InfoBuff[1], 
                                                   gAmrModuleQuery[nTtlIndex].InfoBuff[0], 
                                                   gAmrModuleQuery[nTtlIndex].InfoBuff[3], 
                                                   gAmrModuleQuery[nTtlIndex].InfoBuff[2], 
                                                   gAmrModuleQuery[nTtlIndex].InfoBuff[6], 
                                                   gAmrModuleQuery[nTtlIndex].InfoBuff[5], 
                                                   gAmrModuleQuery[nTtlIndex].InfoBuff[4], 
                                                   gAmrModuleQuery[nTtlIndex].InfoBuff[8], 
                                                   gAmrModuleQuery[nTtlIndex].InfoBuff[7]);

    CCOUP_FMT_DEBUG("InfoAmrModuleQuery() faccode=%s, strlen(faccode)=%d, factLen=%d \n", faccode, strlen(faccode), factLen);
    
    SendInfoMsgData.aPayloadData[IndexID] = factLen;//通信模块厂商代码及版本信息长度(字符个数)
	IndexID += 1;

    memcpy(&SendInfoMsgData.aPayloadData[IndexID], (uint8 *)&faccode, factLen);//通信模块厂商代码及版本信息
	IndexID += factLen;
	
	SendInfoMsgData.nPayloadLength = IndexID;
	
    PlcWriteQueue(pAmrSendUp, (uint8 *)&SendInfoMsgData, sizeof(MSG_INFO_T)+SendInfoMsgData.nPayloadLength);
}

/*******************************************************************************
* 函数名称: cco_master_node_get
* 函数功能: 查询主节点
* 输入参数: 
* 输出参数: 
* 返 回 值: 
*******************************************************************************/
void cco_master_node_get(INFO_DATA_T *InfoMsgData)
{
    INFO_DATA_T SendInfoMsgData = {0};

    memcpy(&SendInfoMsgData, InfoMsgData, sizeof(MSG_INFO_T));
    SendInfoMsgData.MsgRPM = 0;
    memcpy_r(SendInfoMsgData.aPayloadData, gTerminalAddr[0], 6);
    SendInfoMsgData.nPayloadLength = 6;
    PlcWriteQueue(pAmrSendUp, (uint8 *)&SendInfoMsgData, sizeof(MSG_INFO_T) + SendInfoMsgData.nPayloadLength);
}

/*******************************************************************************
* 函数名称: cco_file_update_start
* 函数功能: 启动升级
* 输入参数: 
* 输出参数: 
* 返 回 值: 
*******************************************************************************/
void cco_file_update_start(INFO_DATA_T *InfoMsgData)
{
    CCOUP_FMT_DEBUG("cco_file_update_start启动升级...begin \n");
    
    uint8   nTtlIndex = 0;  //不同的端口号对应不同的数组下标

    uint32  nPortID = 0;  //端口号

    uint16  IndexID = 0;  //偏移位
    uint16 pathlen;
    int     ret = 0;

    if(gIsUpdating == 1)
    {
        CCOUP_FMT_DEBUG("cco_file_update_start 当前正在升级中 \n");
        InfoSimpleAck(*InfoMsgData, FALSE);
        return;
    }

    //解析消息MSG
    CharArrToUint32(&InfoMsgData->aPayloadData[IndexID], &nPortID);  //端口号
    ret = PortIDToArrIndex(nPortID, &nTtlIndex);
    if(ret < 0)
    {
        //消息组帧发至消息中心
        InfoSimpleAck(*InfoMsgData, FALSE);
        return;
    }
    IndexID += 4;
    //文件路径
    BuffToChangeLen(&InfoMsgData->aPayloadData[IndexID], &IndexID, &pathlen);
    memcpy(gFilePath,&InfoMsgData->aPayloadData[IndexID],pathlen);
    IndexID+=pathlen;
    gFileFlag = InfoMsgData->aPayloadData[IndexID++];

    CCOUP_FMT_DEBUG("cco_file_update_start 文件路径 %s 文件标识 %d   \n",gFilePath,gFileFlag);
    gIsUpdating = 1;
    gIsNeedUpdating =1;
    InfoSimpleAck(*InfoMsgData, TRUE);
}

/*******************************************************************************
* 函数名称: cco_update_state_get
* 函数功能: 查询升级状态
* 输入参数: 
* 输出参数: 
* 返 回 值: 
*******************************************************************************/
void cco_update_state_get(INFO_DATA_T *InfoMsgData)
{
    CCOUP_FMT_DEBUG("cco_update_state_get()查询升级状态 \n");

    uint8   nTtlIndex = 0;  //不同的端口号对应不同的数组下标
    uint32  nPortID = 0;  //端口号
    uint16  IndexID = 0;  //偏移位
    int     ret = 0;
    //uint8 payload[100]={0};
    INFO_DATA_T SendInfoMsgData = {0};
    uint16 index=0;

    //解析消息MSG
    CharArrToUint32(&InfoMsgData->aPayloadData[IndexID], &nPortID);  //端口号
    ret = PortIDToArrIndex(nPortID, &nTtlIndex);
    if(ret < 0)
    {
        //消息组帧发至消息中心
        InfoSimpleAck(*InfoMsgData, FALSE);
        return;
    }
    SendInfoMsgData.aPayloadData[index++] = gUpdateState;
    memcpy_r(&SendInfoMsgData.aPayloadData[index],&gUpdateBeginTime,4);
    index+=4;
    memcpy_r(&SendInfoMsgData.aPayloadData[index],&gUpdateEndTime,4);
    index+=4;
    if(gUpdateState !=2)
    {
        SendInfoMsgData.aPayloadData[index++] = 0;
    }else
    {
        SendInfoMsgData.aPayloadData[index++] = 1;
        SendInfoMsgData.aPayloadData[index++] = gUpdateResult;
    }
    //消息组帧发至消息中心
    SendInfoMsgData.nPayloadLength = index;
    PlcWriteQueue(pAmrSendUp, (uint8 *)&SendInfoMsgData, sizeof(MSG_INFO_T) + SendInfoMsgData.nPayloadLength);
}


/*********************************
函数功能：    去除重复任务函数
函数原型：    Remove_RepeatPlcTask
函数参数：
函数返回值：
*********************************/
int8 Remove_RepeatPlcTask(PLCTASK_DETAIL      stPlcTaskDetail, uint8  nArrIndex)
{
	int IndexOfQueue = 0;

	//主动抄表、数据透传、从节点监控不判断重复任务
	/*
	if(stPlcTaskDetail.TaskName == MS_NORMAL_AMR
		|| stPlcTaskDetail.TaskName == MS_RELAY_TRANS
		|| stPlcTaskDetail.TaskName == MS_SLAVE_MONITOR
		|| stPlcTaskDetail.TaskName == MS_GET_MANU_ID)
	{
		return IndexOfQueue;
	}
	*/

	//检查任务队列是否存在重复任务
	IndexOfQueue = LocateElemSameNamePort_Task(PlcTaskQueue[nArrIndex], stPlcTaskDetail);
	return IndexOfQueue;
    
}

/*******************************************************************************
* 函数名称: PlcTask_AddQueue
* 函数功能: 任务链表添加
* 输入参数:     
* 输出参数:         
* 返 回 值:      
*******************************************************************************/
int PlcTask_AddQueue(PLCTASK_DETAIL      stPlcTaskDetail, uint8 nArrIndex)
{
	//printx(" PlcTask_AddQueue(PLCTASK_DETAIL stPlcTaskDetail)...begin \n");
	int i = 0;
	int IndexOfQueue;

	//补充任务信息
	while(PlcTask_FunList[i].TaskName != 0xFF)
	{
		if(PlcTask_FunList[i].TaskName == stPlcTaskDetail.TaskName)
		{
			stPlcTaskDetail.TaskStat = PlcTask_FunList[i].TaskStat;
			stPlcTaskDetail.Priority = PlcTask_FunList[i].Priority;
			stPlcTaskDetail.DelaySec = PlcTask_FunList[i].DelaySec;
			stPlcTaskDetail.pPlcTaskFun = PlcTask_FunList[i].pPlcTaskFun;
			memset(stPlcTaskDetail.strTaskName, 0, sizeof(stPlcTaskDetail.strTaskName));
			strncpy(stPlcTaskDetail.strTaskName, (char *)PlcTask_FunList[i].strTaskName, strlen((char *)PlcTask_FunList[i].strTaskName));
			break;
		}
		i++;
	}

	//检查任务队列是否存在重复任务
	IndexOfQueue = Remove_RepeatPlcTask(stPlcTaskDetail, nArrIndex);
	if(IndexOfQueue > 0)
	{
		return -1;
	}
	else
	{
		//添加任务
		//printx(" ******PlcTask_AddQueue()不存在重复的任务, 因此需要添加该任务****** \n");
		
		CCODOWN_FMT_DEBUG(nArrIndex, "PlcTask_AddQueue()添加之前任务数量:%d...begin \n", ListLength_Task(PlcTaskQueue[nArrIndex]));  //添加之前的数目
		CCODOWN_FMT_DEBUG(nArrIndex, "添加的任务信息: \n");
		PrintTaskDetail(stPlcTaskDetail, nArrIndex);
		
		int nNextPriorityID;  //比当前需要添加的任务的优先级低（数字越大，优先级越低）的任务ID（id从1开始）
		nNextPriorityID = LocateElemPriorityLater_Task(PlcTaskQueue[nArrIndex], stPlcTaskDetail);
		ListInsert_Task(&PlcTaskQueue[nArrIndex], nNextPriorityID, stPlcTaskDetail);
		
		CCODOWN_FMT_DEBUG(nArrIndex, "PlcTask_AddQueue()添加之后任务数量:%d...end \n", ListLength_Task(PlcTaskQueue[nArrIndex]));  //添加之后的数目
		PrintTaskList(PlcTaskQueue[nArrIndex], nArrIndex);
	}

	//判断是否需要执行更高优先级的任务
	if(stPlcTaskDetail.Priority < gPlcTaskDetail[nArrIndex].Priority)
	{
		switch (gPlcTaskDetail[nArrIndex].TaskName)
		{
			case MS_NORMAL_AMR:  //当前任务如果是抄表, 则立即中断, 执行更高优先级任务
			{
				ClearAmrList(nArrIndex);  //清空抄表队列
				gTotalState[nArrIndex].NormalAmr = RTS_END;
			}
				break;

			default:
				break;
		}
	}

	return 1;
}

/*******************************************************************************
* 函数名称:	AddAmrIntoList
* 函数功能:	添加抄表消息至队列
* 输入参数:	无
* 输出参数:	无
* 返 回 值: 无
*******************************************************************************/
int8 AddAmrIntoList(AMR_MODE_ACTIVE_T AmrModeActive, uint8 nArrIndex)
{
	int     result = 1;
	uint16  nAddIndex = 0;
	uint8   nTtlIndex = 0;  //不同的端口号对应不同的数组下标

	nTtlIndex = nArrIndex;
	
	//锁缓冲区
    pthread_mutex_lock(&gActiveAmrInfoList[nTtlIndex].ActiveAmrInfoLock);

	//添加报文
	if(gActiveAmrInfoList[nTtlIndex].nActiveAmrInfoNum < MAX_ACTIVE_AMR_INFO_NUM)
	{
		CCOUP_FMT_DEBUG("AddAmrIntoList() 添加之前...gActiveAmrInfoList[%d].nActiveAmrInfoNum = %d \n", nTtlIndex, gActiveAmrInfoList[nTtlIndex].nActiveAmrInfoNum);
		CCOUP_FMT_DEBUG("添加的抄表任务为: \n");
		PrintAmrActiveDetail(AmrModeActive);
		
		nAddIndex = (gActiveAmrInfoList[nTtlIndex].ActiveAmrInfoIndex + gActiveAmrInfoList[nTtlIndex].nActiveAmrInfoNum) % MAX_ACTIVE_AMR_INFO_NUM;//循环取添加地址（数组下标）				
		gActiveAmrInfoList[nTtlIndex].nActiveAmrInfoNum ++;
		gActiveAmrInfoList[nTtlIndex].aActiveAmrInfoList[nAddIndex] = AmrModeActive;

		CCOUP_FMT_DEBUG("AddAmrIntoList() 添加之后...gActiveAmrInfoList[%d].nActiveAmrInfoNum = %d \n", nTtlIndex, gActiveAmrInfoList[nTtlIndex].nActiveAmrInfoNum);
	}
	else
	{
		CCOUP_FMT_DEBUG("AddAmrIntoList() gActiveAmrInfoList[%d].nActiveAmrInfoNum = %d 大于等于 MAX_ACTIVE_AMR_INFO_NUM 请等待...\n", nTtlIndex, gActiveAmrInfoList[nTtlIndex].nActiveAmrInfoNum);
		result = -1;
	}

	//解锁
	pthread_mutex_unlock(&gActiveAmrInfoList[nTtlIndex].ActiveAmrInfoLock);

	return result;
}

/*******************************************************************************
* 函数名称:	GetAmrFromList
* 函数功能:	从队列取出抄表信息
* 输入参数:	无
* 输出参数:	无
* 返 回 值: 无
*******************************************************************************/
int8 GetAmrFromList(AMR_MODE_ACTIVE_T *AmrModeActive, uint8 nArrIndex)
{
	int     result = 1;
	//uint16  nAddIndex = 0;
	uint8   nTtlIndex = 0;  //不同的端口号对应不同的数组下标

	nTtlIndex = nArrIndex;
	
	//锁缓冲区
    pthread_mutex_lock(&gActiveAmrInfoList[nTtlIndex].ActiveAmrInfoLock);
	
	//判断消息队列是否为空
	if(gActiveAmrInfoList[nTtlIndex].nActiveAmrInfoNum > 0)
	{	
		CCOUP_FMT_DEBUG("GetAmrFromList() 取出之前...gActiveAmrInfoList[%d].nActiveAmrInfoNum = %d \n", nTtlIndex, gActiveAmrInfoList[nTtlIndex].nActiveAmrInfoNum);
		CCOUP_FMT_DEBUG("取出的抄表任务为: \n");
		PrintAmrActiveDetail(*AmrModeActive);
		
		//按照先进先出的顺序取出消息
		*AmrModeActive = gActiveAmrInfoList[nTtlIndex].aActiveAmrInfoList[gActiveAmrInfoList[nTtlIndex].ActiveAmrInfoIndex];
		gActiveAmrInfoList[nTtlIndex].nActiveAmrInfoNum --;
		gActiveAmrInfoList[nTtlIndex].ActiveAmrInfoIndex = (gActiveAmrInfoList[nTtlIndex].ActiveAmrInfoIndex+1) % MAX_ACTIVE_AMR_INFO_NUM;

		CCOUP_FMT_DEBUG("GetAmrFromList() 取出之后...gActiveAmrInfoList[%d].nActiveAmrInfoNum = %d \n", nTtlIndex, gActiveAmrInfoList[nTtlIndex].nActiveAmrInfoNum);
	}
	else
	{
		CCOUP_FMT_DEBUG("GetAmrFromList() gActiveAmrInfoList[%d].nActiveAmrInfoNum = %d 小于等于 0 请等待...\n", nTtlIndex, gActiveAmrInfoList[nTtlIndex].nActiveAmrInfoNum);
		result = -1;
	}

	//解锁
	pthread_mutex_unlock(&gActiveAmrInfoList[nTtlIndex].ActiveAmrInfoLock);

	return result;
}

/*******************************************************************************
* 函数名称:	QueryAmrFromList
* 函数功能:	从队列查询抄表信息（只查询，不删除）
* 输入参数:	无
* 输出参数:	无
* 返 回 值: 无
*******************************************************************************/
int8 QueryAmrFromList(AMR_MODE_ACTIVE_T *AmrModeActive, uint8 nArrIndex)
{
	int     result = 1;
	uint8   nTtlIndex = 0;  //不同的端口号对应不同的数组下标

	nTtlIndex = nArrIndex;
	
	//锁缓冲区
    pthread_mutex_lock(&gActiveAmrInfoList[nTtlIndex].ActiveAmrInfoLock);
	
	//判断消息队列是否为空
	if(gActiveAmrInfoList[nTtlIndex].nActiveAmrInfoNum > 0)
	{	
		//按照先进先出的顺序取出消息
		*AmrModeActive = gActiveAmrInfoList[nTtlIndex].aActiveAmrInfoList[gActiveAmrInfoList[nTtlIndex].ActiveAmrInfoIndex];

		//CCOUP_FMT_DEBUG("QueryAmrFromList() 查询到的抄表任务为: \n");
		//PrintAmrActiveDetail(*AmrModeActive);
	}
	else
	{
		result = -1;
	}

	//解锁
	pthread_mutex_unlock(&gActiveAmrInfoList[nTtlIndex].ActiveAmrInfoLock);

	return result;
}

/*******************************************************************************
* 函数名称:	DeleteAmrFromList
* 函数功能:	从队列删除第一条抄表信息
* 输入参数:	无
* 输出参数:	无
* 返 回 值: 无
*******************************************************************************/
int8 DeleteAmrFromList(uint8 nArrIndex)
{
	int     result = 1;
	uint8   nTtlIndex = 0;  //不同的端口号对应不同的数组下标

	nTtlIndex = nArrIndex;
	
	//锁缓冲区
    pthread_mutex_lock(&gActiveAmrInfoList[nTtlIndex].ActiveAmrInfoLock);
	
	//判断消息队列是否为空
	if(gActiveAmrInfoList[nTtlIndex].nActiveAmrInfoNum > 0)
	{	
		CCOUP_FMT_DEBUG("DeleteAmrFromList() 删除之前...gActiveAmrInfoList[%d].nActiveAmrInfoNum = %d \n", nTtlIndex, gActiveAmrInfoList[nTtlIndex].nActiveAmrInfoNum);		
		
		//按照先进先出的顺序，删除第一条信息
		gActiveAmrInfoList[nTtlIndex].nActiveAmrInfoNum --;
		gActiveAmrInfoList[nTtlIndex].ActiveAmrInfoIndex = (gActiveAmrInfoList[nTtlIndex].ActiveAmrInfoIndex+1) % MAX_ACTIVE_AMR_INFO_NUM;

		CCOUP_FMT_DEBUG("AddAmrIntoList() 删除之后...gActiveAmrInfoList[%d].nActiveAmrInfoNum = %d \n", nTtlIndex, gActiveAmrInfoList[nTtlIndex].nActiveAmrInfoNum);
	}
	else
	{
		CCOUP_FMT_DEBUG("DeleteAmrFromList() gActiveAmrInfoList[%d].nActiveAmrInfoNum = %d 小于等于 0 请等待...\n", nTtlIndex, gActiveAmrInfoList[nTtlIndex].nActiveAmrInfoNum);
		result = -1;
	}

	//解锁
	pthread_mutex_unlock(&gActiveAmrInfoList[nTtlIndex].ActiveAmrInfoLock);

	return result;
}

/*******************************************************************************
* 函数名称:	ClearAmrList
* 函数功能:	清空抄表队列
* 输入参数:	无
* 输出参数:	无
* 返 回 值: 无
*******************************************************************************/
int8 ClearAmrList(uint8 nArrIndex)
{
	int     result = 1;
	uint8   nTtlIndex = 0;  //不同的端口号对应不同的数组下标

	nTtlIndex = nArrIndex;
	
	//锁缓冲区
    pthread_mutex_lock(&gActiveAmrInfoList[nTtlIndex].ActiveAmrInfoLock);

	//清空队列
	memset(&gActiveAmrInfoList[nTtlIndex], 0x00, sizeof(AMR_ACTIVE_INFO_LIST_T));
	CCOUP_FMT_DEBUG("ClearAmrList() 清空抄表队列后: gActiveAmrInfoList[%d].nActiveAmrInfoNum=%d...\n", nTtlIndex, gActiveAmrInfoList[nTtlIndex].nActiveAmrInfoNum);

	//解锁
	pthread_mutex_unlock(&gActiveAmrInfoList[nTtlIndex].ActiveAmrInfoLock);

	return result;
}


/*******************************************************************************
* 函数名称:	GetAmrNumFromList
* 函数功能:	查询抄表信息数量
* 输入参数:	无
* 输出参数:	无
* 返 回 值: 无
*******************************************************************************/
uint16 GetAmrNumFromList(uint8 nArrIndex)
{
	int     nAmrNum = 0;
	uint8   nTtlIndex = 0;  //不同的端口号对应不同的数组下标

	nTtlIndex = nArrIndex;
	
	//锁缓冲区
    pthread_mutex_lock(&gActiveAmrInfoList[nTtlIndex].ActiveAmrInfoLock);
	
	//赋值
	nAmrNum = gActiveAmrInfoList[nTtlIndex].nActiveAmrInfoNum;

	//解锁
	pthread_mutex_unlock(&gActiveAmrInfoList[nTtlIndex].ActiveAmrInfoLock);

	return nAmrNum;

}

/*******************************************************************************
* 函数名称:	AddDataTransmitList
* 函数功能:	添加透传信息至队列
* 输入参数:	无
* 输出参数:	无
* 返 回 值: 无
*******************************************************************************/
int8 AddDataTransmitList(uint8 nArrIndex, AMR_DATA_TRANSMIT_T AmrDataTransmit, INFO_DATA_HEAD_T InfoDataHead)
{
	int     result = 1;
	uint16  nAddIndex = 0;
	uint8   nTtlIndex = 0;  //不同的端口号对应不同的数组下标

	nTtlIndex = nArrIndex;
	
	//锁缓冲区
    pthread_mutex_lock(&gDataTransmitQueue[nTtlIndex].DataTransmitLock);

	//添加报文
	if(gDataTransmitQueue[nTtlIndex].nDataTransmitNum < MAX_DATA_TRANSMIT_INFO_NUM)
	{
		CCOUP_FMT_DEBUG("AddDataTransmitList() 添加之前...gDataTransmitQueue[%d].nDataTransmitNum = %d \n", nTtlIndex, gDataTransmitQueue[nTtlIndex].nDataTransmitNum);
		
		nAddIndex = (gDataTransmitQueue[nTtlIndex].DataTransmitIndex + gDataTransmitQueue[nTtlIndex].nDataTransmitNum) % MAX_DATA_TRANSMIT_INFO_NUM;//循环取添加地址（数组下标）				
		gDataTransmitQueue[nTtlIndex].nDataTransmitNum ++;
		gDataTransmitQueue[nTtlIndex].aDataTransmitList[nAddIndex] = AmrDataTransmit;
		gDataTransmitQueue[nTtlIndex].aInfoDataHead[nAddIndex] = InfoDataHead;

		CCOUP_FMT_DEBUG("AddDataTransmitList() 添加之后...gDataTransmitQueue[%d].nDataTransmitNum = %d \n", nTtlIndex, gDataTransmitQueue[nTtlIndex].nDataTransmitNum);
	}
	else
	{
		result = -1;
	}

	//解锁
	pthread_mutex_unlock(&gDataTransmitQueue[nTtlIndex].DataTransmitLock);

	return result;
}

/*******************************************************************************
* 函数名称:	GetDataTransmitFromList
* 函数功能:	从队列取出透传信息
* 输入参数:	无
* 输出参数:	无
* 返 回 值: 无
*******************************************************************************/
int8 GetDataTransmitFromList(uint8 nArrIndex, AMR_DATA_TRANSMIT_T *AmrDataTransmit, INFO_DATA_HEAD_T *InfoDataHead)
{
	int     result = 1;
	uint8   nTtlIndex = 0;  //不同的端口号对应不同的数组下标

	nTtlIndex = nArrIndex;
	
	//锁缓冲区
    pthread_mutex_lock(&gDataTransmitQueue[nTtlIndex].DataTransmitLock);
	
	//判断消息队列是否为空
	if(gDataTransmitQueue[nTtlIndex].nDataTransmitNum > 0)
	{	
		CCOUP_FMT_DEBUG("GetDataTransmitFromList() 取出之前...gDataTransmitQueue[%d].nDataTransmitNum = %d \n", nTtlIndex, gDataTransmitQueue[nTtlIndex].nDataTransmitNum);
		
		//按照先进先出的顺序取出消息
		*AmrDataTransmit = gDataTransmitQueue[nTtlIndex].aDataTransmitList[gDataTransmitQueue[nTtlIndex].DataTransmitIndex];
		*InfoDataHead = gDataTransmitQueue[nTtlIndex].aInfoDataHead[gDataTransmitQueue[nTtlIndex].DataTransmitIndex];
		gDataTransmitQueue[nTtlIndex].nDataTransmitNum --;
		gDataTransmitQueue[nTtlIndex].DataTransmitIndex = (gDataTransmitQueue[nTtlIndex].DataTransmitIndex+1) % MAX_DATA_TRANSMIT_INFO_NUM;

		
		CCOUP_FMT_DEBUG("GetDataTransmitFromList() 取出之前...gDataTransmitQueue[%d].nDataTransmitNum = %d \n", nTtlIndex, gDataTransmitQueue[nTtlIndex].nDataTransmitNum);
	}
	else
	{
		result = -1;
	}

	//解锁
	pthread_mutex_unlock(&gDataTransmitQueue[nTtlIndex].DataTransmitLock);

	return result;
}


/*******************************************************************************
* 函数名称:	AddSlaveMonitorTimeList
* 函数功能:	添加从节点监控延时信息至队列
* 输入参数:	无
* 输出参数:	无
* 返 回 值: 无
*******************************************************************************/
int8 AddSlaveMonitorTimeList(uint8 nArrIndex, AMR_SLAVE_NODE_DELAY_TIME_T AmrSlaveNodeMonitorTime, INFO_DATA_HEAD_T InfoDataHead)
{
	int     result = 1;
	uint16  nAddIndex = 0;
	uint8   nTtlIndex = 0;  //不同的端口号对应不同的数组下标

	nTtlIndex = nArrIndex;
	
	//锁缓冲区
    pthread_mutex_lock(&gSlaveMonitorTimeQueue[nTtlIndex].SlaveMonitorTimeLock);

	//添加报文
	if(gSlaveMonitorTimeQueue[nTtlIndex].nSlaveMonitorTimeNum < MAX_SLAVE_MONITOR_INFO_NUM)
	{
		CCOUP_FMT_DEBUG("AddSlaveMonitorTimeList() 添加之前...gSlaveMonitorTimeQueue[%d].nSlaveMonitorTimeNum = %d \n", nTtlIndex, gSlaveMonitorTimeQueue[nTtlIndex].nSlaveMonitorTimeNum);
		
		nAddIndex = (gSlaveMonitorTimeQueue[nTtlIndex].SlaveMonitorTimeIndex + gSlaveMonitorTimeQueue[nTtlIndex].nSlaveMonitorTimeNum) % MAX_SLAVE_MONITOR_INFO_NUM;//循环取添加地址（数组下标）
		gSlaveMonitorTimeQueue[nTtlIndex].nSlaveMonitorTimeNum ++;
		gSlaveMonitorTimeQueue[nTtlIndex].aSlaveMonitorTimeList[nAddIndex] = AmrSlaveNodeMonitorTime;
		gSlaveMonitorTimeQueue[nTtlIndex].aInfoDataHead[nAddIndex] = InfoDataHead;

		CCOUP_FMT_DEBUG("AddSlaveMonitorTimeList() 添加之后...gSlaveMonitorTimeQueue[%d].nSlaveMonitorTimeNum = %d \n", nTtlIndex, gSlaveMonitorTimeQueue[nTtlIndex].nSlaveMonitorTimeNum);
	}
	else
	{
		result = -1;
	}

	//解锁
	pthread_mutex_unlock(&gSlaveMonitorTimeQueue[nTtlIndex].SlaveMonitorTimeLock);

	return result;
}

/*******************************************************************************
* 函数名称:	GetSlaveMonitorTimeFromList
* 函数功能:	从队列取出从节点监控延时信息
* 输入参数:	无
* 输出参数:	无
* 返 回 值: 无
*******************************************************************************/
int8 GetSlaveMonitorTimeFromList(uint8 nArrIndex, AMR_SLAVE_NODE_DELAY_TIME_T *AmrSlaveNodeMonitor, INFO_DATA_HEAD_T *InfoDataHead)
{
	int     result = 1;
	uint8   nTtlIndex = 0;  //不同的端口号对应不同的数组下标

	nTtlIndex = nArrIndex;
	
	//锁缓冲区
    pthread_mutex_lock(&gSlaveMonitorTimeQueue[nTtlIndex].SlaveMonitorTimeLock);
	
	//判断消息队列是否为空
	if(gSlaveMonitorTimeQueue[nTtlIndex].nSlaveMonitorTimeNum > 0)
	{	
		CCOUP_FMT_DEBUG("GetSlaveMonitorTimeFromList() 取出之前...gSlaveMonitorTimeQueue[%d].nSlaveMonitorTimeNum = %d \n", nTtlIndex, gSlaveMonitorTimeQueue[nTtlIndex].nSlaveMonitorTimeNum);
		
		//按照先进先出的顺序取出消息
		*AmrSlaveNodeMonitor = gSlaveMonitorTimeQueue[nTtlIndex].aSlaveMonitorTimeList[gSlaveMonitorTimeQueue[nTtlIndex].SlaveMonitorTimeIndex];
		*InfoDataHead = gSlaveMonitorTimeQueue[nTtlIndex].aInfoDataHead[gSlaveMonitorTimeQueue[nTtlIndex].SlaveMonitorTimeIndex];
		gSlaveMonitorTimeQueue[nTtlIndex].nSlaveMonitorTimeNum --;
		gSlaveMonitorTimeQueue[nTtlIndex].SlaveMonitorTimeIndex = (gSlaveMonitorTimeQueue[nTtlIndex].SlaveMonitorTimeIndex+1) % MAX_SLAVE_MONITOR_INFO_NUM;

		CCOUP_FMT_DEBUG("GetSlaveMonitorTimeFromList() 取出之后...gSlaveMonitorTimeQueue[%d].nSlaveMonitorTimeNum = %d \n", nTtlIndex, gSlaveMonitorTimeQueue[nTtlIndex].nSlaveMonitorTimeNum);
	}
	else
	{
		result = -1;
	}

	//解锁
	pthread_mutex_unlock(&gSlaveMonitorTimeQueue[nTtlIndex].SlaveMonitorTimeLock);

	return result;
}


/*******************************************************************************
* 函数名称:	AddSlaveMonitorList
* 函数功能:	添加从节点监控信息至队列
* 输入参数:	无
* 输出参数:	无
* 返 回 值: 无
*******************************************************************************/
int8 AddSlaveMonitorList(uint8 nArrIndex, AMR_SLAVE_NODE_MONITOR_T AmrSlaveNodeMonitor, INFO_DATA_HEAD_T InfoDataHead)
{
	int     result = 1;
	uint16  nAddIndex = 0;
	uint8   nTtlIndex = 0;  //不同的端口号对应不同的数组下标

	nTtlIndex = nArrIndex;
	
	//锁缓冲区
    pthread_mutex_lock(&gSlaveMonitorQueue[nTtlIndex].SlaveMonitorLock);

	//添加报文
	if(gSlaveMonitorQueue[nTtlIndex].nSlaveMonitorNum < MAX_SLAVE_MONITOR_INFO_NUM)
	{
		CCOUP_FMT_DEBUG("AddSlaveMonitorList() 添加之前...gSlaveMonitorQueue[%d].nSlaveMonitorNum = %d \n", nTtlIndex, gSlaveMonitorQueue[nTtlIndex].nSlaveMonitorNum);
		
		nAddIndex = (gSlaveMonitorQueue[nTtlIndex].SlaveMonitorIndex + gSlaveMonitorQueue[nTtlIndex].nSlaveMonitorNum) % MAX_SLAVE_MONITOR_INFO_NUM;//循环取添加地址（数组下标）
		gSlaveMonitorQueue[nTtlIndex].nSlaveMonitorNum ++;
		gSlaveMonitorQueue[nTtlIndex].aSlaveMonitorList[nAddIndex] = AmrSlaveNodeMonitor;
		gSlaveMonitorQueue[nTtlIndex].aInfoDataHead[nAddIndex] = InfoDataHead;

		CCOUP_FMT_DEBUG("AddSlaveMonitorList() 添加之后...gSlaveMonitorQueue[%d].nSlaveMonitorNum = %d \n", nTtlIndex, gSlaveMonitorQueue[nTtlIndex].nSlaveMonitorNum);
	}
	else
	{
		result = -1;
	}

	//解锁
	pthread_mutex_unlock(&gSlaveMonitorQueue[nTtlIndex].SlaveMonitorLock);

	return result;
}

/*******************************************************************************
* 函数名称:	GetSlaveMonitorFromList
* 函数功能:	从队列取出从节点监控信息
* 输入参数:	无
* 输出参数:	无
* 返 回 值: 无
*******************************************************************************/
int8 GetSlaveMonitorFromList(uint8 nArrIndex, AMR_SLAVE_NODE_MONITOR_T *AmrSlaveNodeMonitor, INFO_DATA_HEAD_T *InfoDataHead)
{
	int     result = 1;
	uint8   nTtlIndex = 0;  //不同的端口号对应不同的数组下标

	nTtlIndex = nArrIndex;
	
	//锁缓冲区
    pthread_mutex_lock(&gSlaveMonitorQueue[nTtlIndex].SlaveMonitorLock);
	
	//判断消息队列是否为空
	if(gSlaveMonitorQueue[nTtlIndex].nSlaveMonitorNum > 0)
	{	
		CCOUP_FMT_DEBUG("GetSlaveMonitorFromList() 取出之前...gSlaveMonitorQueue[%d].nSlaveMonitorNum = %d \n", nTtlIndex, gSlaveMonitorQueue[nTtlIndex].nSlaveMonitorNum);
		
		//按照先进先出的顺序取出消息
		*AmrSlaveNodeMonitor = gSlaveMonitorQueue[nTtlIndex].aSlaveMonitorList[gSlaveMonitorQueue[nTtlIndex].SlaveMonitorIndex];
		*InfoDataHead = gSlaveMonitorQueue[nTtlIndex].aInfoDataHead[gSlaveMonitorQueue[nTtlIndex].SlaveMonitorIndex];
		gSlaveMonitorQueue[nTtlIndex].nSlaveMonitorNum --;
		gSlaveMonitorQueue[nTtlIndex].SlaveMonitorIndex = (gSlaveMonitorQueue[nTtlIndex].SlaveMonitorIndex+1) % MAX_SLAVE_MONITOR_INFO_NUM;

		CCOUP_FMT_DEBUG("GetSlaveMonitorFromList() 取出之后...gSlaveMonitorQueue[%d].nSlaveMonitorNum = %d \n", nTtlIndex, gSlaveMonitorQueue[nTtlIndex].nSlaveMonitorNum);
	}
	else
	{
		result = -1;
	}

	//解锁
	pthread_mutex_unlock(&gSlaveMonitorQueue[nTtlIndex].SlaveMonitorLock);

	return result;
}



/*******************************************************************************
* 函数名称:	AddInfoDataIntoList
* 函数功能:	添加消息至队列
* 输入参数:	消息msg结构体
* 输出参数:	无
* 返 回 值: 
*******************************************************************************/
int8 AddInfoDataIntoList(INFO_DATA_T RecvInfoData)
{
	uint16 nAddIndex = 0;
	
	//将收到的新消息暂存缓冲区
	if(gInfoDataList.InfoDataNum < MAX_INFO_DATA_NUM)
	{
		nAddIndex = (gInfoDataList.InfoDataReadIndex+gInfoDataList.InfoDataNum) % MAX_INFO_DATA_NUM;//循环取添加地址（数组下标）
		CCOUP_FMT_DEBUG("AddMsgIntoList()添加之前...stInfoMsgDataList.InfoMsgDataNum=[%d]...begin \n", gInfoDataList.InfoDataNum);
		
		gInfoDataList.InfoDataNum ++;
		gInfoDataList.InfoDataList[nAddIndex] = RecvInfoData;

		CCOUP_FMT_DEBUG("AddMsgIntoList()添加之后...stInfoMsgDataList.InfoMsgDataNum=[%d]...end \n", gInfoDataList.InfoDataNum);
	}
	else
	{
		CCOUP_FMT_DEBUG("CreateTaskByInfo()...stInfoMsgDataList.InfoMsgDataNum 大于等于 MAX_MSG_INFO_NUM，请等待...\n");
		return -1;
	}
	

	return 1;
}

/*******************************************************************************
* 函数名称:	GetInfoDataFromList
* 函数功能:	从队列中取出消息
* 输入参数:	
* 输出参数:	消息msg结构体
* 返 回 值: 
*******************************************************************************/
int8 GetInfoDataFromList(INFO_DATA_T *SendInfoData)
{
	//判断消息队列是否为空
	if(gInfoDataList.InfoDataNum > 0)
	{
		CCOUP_FMT_DEBUG("CreateTaskByInfo()取出之前...收到消息的数量=[%d]...begin \n", gInfoDataList.InfoDataNum);
		
		//按照先进先出的顺序取出消息
		*SendInfoData = gInfoDataList.InfoDataList[gInfoDataList.InfoDataReadIndex];
		gInfoDataList.InfoDataNum --;
		gInfoDataList.InfoDataReadIndex = (gInfoDataList.InfoDataReadIndex+1) % MAX_INFO_DATA_NUM;

		CCOUP_FMT_DEBUG("CreateTaskByInfo()取出之后...收到消息的数量=[%d]...end \n", gInfoDataList.InfoDataNum);
	}
	else
	{
		return -1;
	}

	return 1;
}

/*******************************************************************************
* 函数名称:	CreateTaskByInfo
* 函数功能:	根据收到的消息产生任务
* 输入参数:	无
* 输出参数:	无
* 返 回 值: >0 成功
            =0
            <0 失败
*******************************************************************************/
int8 CreateTaskByInfo(void)
{
    int nGetRet = 0;
    
    INFO_DATA_T stInfoData;  //消息
    memset(&stInfoData, 0x00, sizeof(stInfoData));
    
    nGetRet = GetInfoDataFromList(&stInfoData);
    if(nGetRet < 0)
    {
        return -1;
    }
        
    //执行消息
    if(stInfoData.IID_ID != 0x0003)
    {
        return -1;
    }

    switch (stInfoData.IOP_ID)
    {
        case 0x0010://抄表端口配置
        {
            CCOUP_FMT_DEBUG("CreateTaskByInfo()抄表端口配置 \n");
            InfoSetPortID(stInfoData);
            break;
        }
        case 0x0020://添加表档案
        {
            CCOUP_FMT_DEBUG("CreateTaskByInfo()添加表档案 \n");
            InfoAddMeterFiles(stInfoData);
            break;
        }
        case 0x0021://删除表档案
        {
            CCOUP_FMT_DEBUG("CreateTaskByInfo()删除表档案 \n");
            InfoDeleteMeterFiles(stInfoData);
            break;
        }
        case 0x0022://清除表档案
        {
            CCOUP_FMT_DEBUG("CreateTaskByInfo()清除表档案 \n");
            InfoClearMeterFiles(stInfoData);
            break;
        }
        case 0x003D://当前档案数量
        {
            CCOUP_FMT_DEBUG("CreateTaskByInfo()当前档案数量 \n");
            InfoCurrentFilesNum(stInfoData);
            break;
        }
        case 0x0043://查询档案
        {
            CCOUP_FMT_DEBUG("CreateTaskByInfo()查询档案 \n");
            InfoQueryFilesDetail(stInfoData);
            break;
        }
        case 0x0040://暂停抄表
        case 0x0041://恢复抄表
        case 0x0042://重启抄表
        {
            CCOUP_FMT_DEBUG("CreateTaskByInfo()路由操作 \n");
            InfoRouterDeal(stInfoData);
            break;
        }
        case 0x003E://主动抄表
        {
            CCOUP_FMT_DEBUG("CreateTaskByInfo()主动抄表 \n");
            InfoActiveMeter(stInfoData);
            break;
        }
        case 0x0034://启动搜表
        {
            CCOUP_FMT_DEBUG("CreateTaskByInfo()启动搜表 \n");
            InfoSearchMeter(stInfoData);
            break;
        }
        case 0x0035://暂停搜表
        {
            CCOUP_FMT_DEBUG("CreateTaskByInfo()暂停搜表 \n");
            InfoPauseSearchMeter(stInfoData);
            break;
        }
        case 0x0033://数据透传
        {
            CCOUP_FMT_DEBUG("CreateTaskByInfo()数据透传 \n");
            InfoDataTransmit(stInfoData);
            break;
        }
        case 0x0037://获取从节点监控延时
        {
            CCOUP_FMT_DEBUG("CreateTaskByInfo()获取从节点监控延时 \n");
            InfoGetSlaveMonitorDelayTime(stInfoData);
            break;
        }
        case 0x0038://从节点监控
        {
            CCOUP_FMT_DEBUG("CreateTaskByInfo()从节点监控 \n");
            InfoSlaveMonitor(stInfoData);
            break;
        }
        case 0x003B://模块地址设置
        {
            CCOUP_FMT_DEBUG("CreateTaskByInfo()模块地址设置 \n");
            InfoSetTmnAddr(stInfoData);
            break;
        }
        case 0x0039://获取广播延时
        {
            CCOUP_FMT_DEBUG("CreateTaskByInfo()获取广播延时 \n");
            InfoGetBroadcastDelayTime(stInfoData);
            break;
        }
        case 0x003A://广播
        {
            CCOUP_FMT_DEBUG("CreateTaskByInfo()广播 \n");
            InfoBroadcastDeal(stInfoData);
            break;
        }
        case 0x0050://查询HPLC信息
        {
            CCOUP_FMT_DEBUG("CreateTaskByInfo()查询HPLC信息 \n");
            InfoQueryHPLC(stInfoData);
            break;
        }
        case 0x0052://HPLC频点设置
        {
            CCOUP_FMT_DEBUG("CreateTaskByInfo()HPLC频点设置 \n");
            InfoFrSet(stInfoData);
            break;
        }
        case 0x0053://HPLC频点查询
        {
            CCOUP_FMT_DEBUG("CreateTaskByInfo()HPLC频点查询 \n");
            InfoFrQuery(stInfoData);
            break;
        }
        case 0x0054://HPLC台区识别
        {
            CCOUP_FMT_DEBUG("CreateTaskByInfo()HPLC台区识别 \n");
            InfoAreaIdentify(stInfoData);
            break;
        }
        case 0x0055://HPLC离网感知
        {
            CCOUP_FMT_DEBUG("CreateTaskByInfo()HPLC离网感知 \n");
            InfoOfflineFeel(stInfoData);
            break;
        }
        case 0x0056://HPLC拒绝节点上报
        {
            CCOUP_FMT_DEBUG("CreateTaskByInfo()HPLC拒绝节点上报 \n");
            InfoRefuseNodeReport(stInfoData);
            break;
        }
        case 0x0060://查询抄表状态
        {
            CCOUP_FMT_DEBUG("CreateTaskByInfo()查询抄表状态 \n");
            InfoAmrStateQuery(stInfoData);
            break;
        }
        case 0x0061://查询抄表模块信息
        {
            CCOUP_FMT_DEBUG("CreateTaskByInfo()查询抄表模块信息 \n");
            InfoAmrModuleQuery(stInfoData);
            break;
        }
        case 0x0062://查询模块地址
        {
            CCOUP_FMT_DEBUG("CreateTaskByInfo()查询查询模块地址 \n");
            cco_master_node_get(&stInfoData);
            break;
        }            
        case 0x0063://启动升级
        {
            CCOUP_FMT_DEBUG("CreateTaskByInfo()启动升级 \n");
            cco_file_update_start(&stInfoData);
            break;
        }
        case 0x0064:
        {
            CCOUP_FMT_DEBUG("CreateTaskByInfo()查询升级状态 \n");
            cco_update_state_get(&stInfoData);
            break;
        }
        default:
        {
            CCOUP_FMT_DEBUG("CreateTaskByInfo() IOP类型错误! \n");
            return -1;
            break;
        } 
        
    }
    return 1;
}

/*******************************************************************************
* 函数名称: InstantReportTaskDeal
* 函数功能: 即时上报任务处理
* 输入参数:
            uint8 *buf,
            int32 buf_len
* 输出参数: 无
* 返 回 值: >0 已处理
            <0 无对应项处理
*******************************************************************************/
int InstantReportTaskDeal(uint8 *buf, int32 buf_len, uint8 nArrIndex)
{
    //CCODOWN_FMT_DEBUG(nArrIndex, "InstantReportTaskDeal()...start...buf_len=%d, nArrIndex=%d \n", buf_len, nArrIndex);
    AppData_T  AppData;
    AppData_T  SendAppData;
    uint16 IndexID = 0;  //偏移位
    INFO_DATA_T SendInfoMsgData;
    

    memset(&AppData,0x00,sizeof(AppData));
    memset(&SendAppData,0x00,sizeof(SendAppData));
    memset(&SendInfoMsgData, 0x00, sizeof(INFO_DATA_T));

    uint8 nTtlIndex = 0;  //不同的端口号对应不同的数组下标
    nTtlIndex = nArrIndex;
    
    if(UnPackFrameData(buf_len, buf, &AppData) != CHECK_RESULT_SUCCEED)
    {
        CCODOWN_FMT_DEBUG(nTtlIndex, "InstantReportTaskDeal()...if(UnPackFrameData() != CHECK_RESULT_SUCCEED) \n");
        return -1;
    }

    if((AppData.AFN == 0x14) && (AppData.Fn == 1))//路由主动抄表, 回复路由暂停
    {
        CCODOWN_FMT_DEBUG(nTtlIndex, "InstantReportTaskDeal()...路由主动抄表if((AppData.AFN == 0x14) && (AppData.Fn == 1))...\n");
        SendAppData.RInfo[5] =   AppData.RInfo[5];//帧序列
        //printf("AppData.RInfo[5] = %02x\n", AppData.RInfo[5]);
        SendAppData.AFN = 0x12;
        SendAppData.Fn = 2;
        SendAppData.DataUnitLen = 0;
        PackFrameData(SendAppData, nArrIndex);
        return 1;
    }

    if((AppData.AFN == 0x14) && (AppData.Fn == 2))
    {
        CCODOWN_FMT_DEBUG(nTtlIndex, "InstantReportTaskDeal()...if((AppData.AFN == 0x14) && (AppData.Fn == 2))...\n");
        SendAppData.RInfo[5] =   AppData.RInfo[5];//帧序列
        //printf("AppData.RInfo[5] = %02x\n", AppData.RInfo[5]);
        SendAppData.AFN = 0x14;
        SendAppData.Fn = 2;
        SendAppData.DataUnitLen = 6;
        SendAppData.DataUnitBuff[0] = gCurBcdTime[nArrIndex].Second;
        SendAppData.DataUnitBuff[1] = gCurBcdTime[nArrIndex].Minute;
        SendAppData.DataUnitBuff[2] = gCurBcdTime[nArrIndex].Hour;
        SendAppData.DataUnitBuff[3] = gCurBcdTime[nArrIndex].Day;
        SendAppData.DataUnitBuff[4] = gCurBcdTime[nArrIndex].Month;
        SendAppData.DataUnitBuff[5] = gCurBcdTime[nArrIndex].Year;
        PackFrameData(SendAppData, nArrIndex);
        return 2;
    }

    if((AppData.AFN == 0x03) && (AppData.Fn == 10))
    {
        CCODOWN_FMT_DEBUG(nTtlIndex, "InstantReportTaskDeal()...if((AppData.AFN == 0x03) && (AppData.Fn == 10))...\n");
        Comm_ConfirmAckTakeFrameSeq(AppData, nArrIndex);

        if(CheckModuleSoftReset(-1, nTtlIndex) == 0)
        {
            CCODOWN_FMT_DEBUG(nTtlIndex, "CheckModuleSoftReset()==0, 路由主动上报模块信息, 由于之前已经上报, 此次判断模块异常复位, 不处理... \n");
            ClearCommQueueBuff(&gAmrRecvDownReportModule[nTtlIndex]);
            CCODOWN_FMT_DEBUG(nTtlIndex, "模块异常复位执行 档案比对任务 端口:%d \n", nTtlIndex);
            gMeterFileCompareFlag[nTtlIndex] = 1;
            return 3;
        }
        else
        {
            CCODOWN_FMT_DEBUG(nTtlIndex, "CheckModuleSoftReset()==1, 当前正处于 主动获取路由/等待路由上报 状态, 需要处理... \n");

            if(0 == PlcWriteQueue(&gAmrRecvDownReportModule[nTtlIndex], buf, buf_len))
            {
                CCODOWN_FMT_DEBUG(nTtlIndex, "gAmrRecvDownReportModule队列写满! 清空! \n");
                ClearCommQueueBuff(&gAmrRecvDownReportModule[nTtlIndex]);
            }
        }
    }

    if((AppData.AFN == 0x06) && (AppData.Fn == 5))
    {
        CCODOWN_FMT_DEBUG(nTtlIndex, "InstantReportTaskDeal()...if((AppData.AFN == 0x06) && (AppData.Fn == 5))...\n");
        Comm_ConfirmAckTakeFrameSeq(AppData, nArrIndex);
        CCODOWN_FMT_DEBUG(nTtlIndex, "AFN=06H-F5，事件上报!\n");
        CCODOWN_FMT_DEBUG(nTtlIndex, "节点设备类型:%02x(00H:采集器, 01H:电能表，02：hplc)\n", AppData.DataUnitBuff[0]);
        CCODOWN_FMT_DEBUG(nTtlIndex, "通讯协议类型:%02x(00H:保留，01H:97表，02:07表,03:698表，04：从节点停复电)\n", AppData.DataUnitBuff[1]);
        CCODOWN_FMT_DEBUG(nTtlIndex, "长度:%d \n", AppData.DataUnitBuff[2]);
        CCODOWN_FMT_DEBUG(nTtlIndex, "内容: \n");
        CCODOWN_BUF_DEBUG(nTtlIndex, &(AppData.DataUnitBuff[3]), AppData.DataUnitBuff[2], "报文");

        //事件上报smiOS
        Uint32ToCharArr(gPortID[nArrIndex], &SendInfoMsgData.aPayloadData[IndexID]);  //端口号
        IndexID += 4;
        SendInfoMsgData.aPayloadData[IndexID] = AppData.DataUnitBuff[0];  //设备类型
        IndexID += 1;
        SendInfoMsgData.aPayloadData[IndexID] = AppData.DataUnitBuff[1];  //协议类型：0—透明传输；1—DL/T 645-1997；2-DL/T 645-2007；3—DL/T 698.45。 
        IndexID += 1;
        ChangeLenToBuff(AppData.DataUnitBuff[2], &IndexID, &SendInfoMsgData.aPayloadData[IndexID]);  //可变长
        memcpy(&SendInfoMsgData.aPayloadData[IndexID], &AppData.DataUnitBuff[3], AppData.DataUnitBuff[2]);  //数据帧
        IndexID += AppData.DataUnitBuff[2];
        SendInfoMsgData.aPayloadData[IndexID] = 0x00;  //PLC_READING_INFO OPTIONAL 是否含有抄读信息标志, 0x00:无, 0x01:有
        IndexID += 1;       
        SendInfoMsgData.nPayloadLength = IndexID;  //payload长度
        event_to_smiOS(SendInfoMsgData.aPayloadData, SendInfoMsgData.nPayloadLength);

        return 4;
    }

    if((AppData.AFN == 0x06) && (AppData.Fn == 6))
    {
        uint16 datalen = 0;
        CCODOWN_FMT_DEBUG(nTtlIndex, "InstantReportTaskDeal()...if((AppData.AFN == 0x06) && (AppData.Fn == 6))...\n");
        Comm_ConfirmAckTakeFrameSeq(AppData, nArrIndex);
        CCODOWN_FMT_DEBUG(nTtlIndex, "AFN=06H-F6，事件上报!\n");
        CCODOWN_FMT_DEBUG(nTtlIndex, "节点设备类型:%02x(00H:双计量能源, 01H:电能表, 02H:hplc, 03H:窄带载波, 04H:无线通信, 05H:双模通信单元, 06H～FFH:保留)\n", AppData.DataUnitBuff[0]);
        CCODOWN_FMT_DEBUG(nTtlIndex, "通讯协议类型:%02x(00H:保留, 01H:97表, 02:07表, 03:698表, 04H～FFH:保留)\n", AppData.DataUnitBuff[1]);
        memcpy(&datalen, &AppData.DataUnitBuff[2], 2);
        CCODOWN_FMT_DEBUG(nTtlIndex, "长度:%d \n", datalen);
        CCODOWN_FMT_DEBUG(nTtlIndex, "内容: \n");
        CCODOWN_BUF_DEBUG(nTtlIndex, &(AppData.DataUnitBuff[4]), datalen, "报文:");

        //事件上报smiOS
        Uint32ToCharArr(gPortID[nArrIndex], &SendInfoMsgData.aPayloadData[IndexID]);  //端口号
        IndexID += 4;
        SendInfoMsgData.aPayloadData[IndexID] = AppData.DataUnitBuff[0];  //设备类型
        IndexID += 1;
        SendInfoMsgData.aPayloadData[IndexID] = AppData.DataUnitBuff[1];  //协议类型：0—透明传输；1—DL/T 645-1997；2-DL/T 645-2007；3—DL/T 698.45。 
        IndexID += 1;
        ChangeLenToBuff(datalen, &IndexID, &SendInfoMsgData.aPayloadData[IndexID]);  //可变长
        memcpy(&SendInfoMsgData.aPayloadData[IndexID], &AppData.DataUnitBuff[4], datalen);  //数据帧
        IndexID += datalen;
        SendInfoMsgData.aPayloadData[IndexID] = 0x00;  //PLC_READING_INFO OPTIONAL 是否含有抄读信息标志, 0x00:无, 0x01:有
        IndexID += 1;       
        SendInfoMsgData.nPayloadLength = IndexID;  //payload长度
        event_to_smiOS(SendInfoMsgData.aPayloadData, SendInfoMsgData.nPayloadLength);

        return 5;
    }

    if((AppData.AFN == 0x06) && (AppData.Fn == 10))
    {
        uint16 datalen = AppData.DataUnitLen;
        uint8  equiptype = 0x00;
        uint8  prototpye = 0x0a;
        CCODOWN_FMT_DEBUG(nTtlIndex, "InstantReportTaskDeal()...if((AppData.AFN == 0x06) && (AppData.Fn == 10))...\n");
        Comm_ConfirmAckTakeFrameSeq(AppData, nArrIndex);
        CCODOWN_FMT_DEBUG(nTtlIndex, "AFN=06H-F10，事件上报!\n");
        CCODOWN_FMT_DEBUG(nTtlIndex, "节点设备类型:%02x(自定义)\n", equiptype);
        CCODOWN_FMT_DEBUG(nTtlIndex, "通讯协议类型:%02x(自定义)\n", prototpye);
        CCODOWN_FMT_DEBUG(nTtlIndex, "长度:%d \n", datalen);
        CCODOWN_FMT_DEBUG(nTtlIndex, "内容: \n");
        CCODOWN_BUF_DEBUG(nTtlIndex, &(AppData.DataUnitBuff[0]), datalen, "报文:");

        //事件上报smiOS
        Uint32ToCharArr(gPortID[nArrIndex], &SendInfoMsgData.aPayloadData[IndexID]);  //端口号
        IndexID += 4;
        SendInfoMsgData.aPayloadData[IndexID] = equiptype;  //设备类型
        IndexID += 1;
        SendInfoMsgData.aPayloadData[IndexID] = prototpye;  //协议类型：0—透明传输；1—DL/T 645-1997；2-DL/T 645-2007；3—DL/T 698.45。 
        IndexID += 1;
        ChangeLenToBuff(datalen, &IndexID, &SendInfoMsgData.aPayloadData[IndexID]);  //可变长
        memcpy(&SendInfoMsgData.aPayloadData[IndexID], &AppData.DataUnitBuff[0], datalen);  //数据帧
        IndexID += datalen;
        SendInfoMsgData.aPayloadData[IndexID] = 0x00;  //PLC_READING_INFO OPTIONAL 是否含有抄读信息标志, 0x00:无, 0x01:有
        IndexID += 1;       
        SendInfoMsgData.nPayloadLength = IndexID;  //payload长度
        event_to_smiOS(SendInfoMsgData.aPayloadData, SendInfoMsgData.nPayloadLength);

        return 6;
    }

    //搜表结果直接上报; 如果正在搜表, 则将串口报文拷贝一份至搜表队列
    if((AppData.AFN == 0x06) && (AppData.Fn == 1 || AppData.Fn == 4))
    {
        uint8           ReportInfo[500] = {0};              //上报信息
        int             ReportCnt;                          //上报个数
        uint8           RealPhase;                          //实测相位
        uint8           AddrSearchReportType = 0;
        ReportCnt = ReportInfoDeal(ReportInfo, &RealPhase, &AddrSearchReportType, &AppData, nTtlIndex);
        if (ReportCnt > 0)
        {
            CCODOWN_FMT_DEBUG(nTtlIndex, "InstantReportTaskDeal()主动上报搜表结果, 测量点个数:%d \n", ReportCnt);
            if(AddrSearchReportType == 1)
            {
                SaveReportInfo_F1(ReportCnt, ReportInfo, RealPhase, nTtlIndex);
            }
            else if(AddrSearchReportType == 4)
            {
                SaveReportInfo_F4(ReportCnt, ReportInfo, RealPhase, nTtlIndex);
            }
        }
        return 7;
    }
    else if(1 == gAmrStateQuery[nTtlIndex].nModuleSearchState)//当前正处于搜表状态
    {
        if(0 == PlcWriteQueue(&gAmrRecvDownSearchMeter[nTtlIndex], buf, buf_len))
        {
            ClearCommQueueBuff(&gAmrRecvDownSearchMeter[nTtlIndex]);
        }
    }
    
    return -2;
}


/*******************************************************************************
* 函数名称:	AmrInfoInit
* 函数功能:	抄表信息初始化
* 输入参数:	无
* 输出参数:	无
* 返 回 值:	无
*******************************************************************************/
void AmrInfoInit(uint8 nArrIndex)
{
	CCODOWN_FMT_DEBUG(nArrIndex, "AmrInfoInit() begin...\n");
	
	int Ret = -1;

	//将私有文件内容读出来，赋值到档案列表全局变量
	memset(&gAmrMeterFilesListTemp[nArrIndex], 0, sizeof(gAmrMeterFilesListTemp[nArrIndex]));
	ReadFilesFromPData(nArrIndex);

	//将全局电表档案拷贝至消息使用的临时变量
	memset(&gAmrMeterFilesList[nArrIndex], 0, sizeof(gAmrMeterFilesList[nArrIndex]));
	memcpy(&gAmrMeterFilesList[nArrIndex], &gAmrMeterFilesListTemp[nArrIndex], sizeof(gAmrMeterFilesList[nArrIndex]));
	
	//将档案列表全局信息赋值到抄表结构体
	ReadAmrInfo(nArrIndex); 

	//将私有文件内容读出来，赋值到端口号全部变量
	Ret = read_pdata_xram(gPortIDName[nArrIndex], (char *)&gPortID[nArrIndex], 0, sizeof(gPortID[nArrIndex]));
	CCODOWN_FMT_DEBUG(nArrIndex, "AmrInfoInit() 加载私有文件端口号 Ret=%d...\n", Ret);
	if(Ret == 0)
	{	
		bSetPortIDFlag[nArrIndex] = 1;
		CCODOWN_FMT_DEBUG(nArrIndex, "AmrInfoInit()端口号加载成功! i=%d, gPortID[i]=%d (0x%08x), bSetPortIDFlag[i]=%d \n", nArrIndex, gPortID[nArrIndex], gPortID[nArrIndex], bSetPortIDFlag[nArrIndex]);
	}

	//从本地私有文件读取终端地址
	Ret = read_pdata_xram(gModuleManuName[nArrIndex], (char *)&gTerminalAddr[nArrIndex][0], 0, sizeof(gTerminalAddr[nArrIndex]));
	if(Ret < 0)
	{
		CCODOWN_FMT_DEBUG(nArrIndex, "AmrInfoInit() read_pdata_xram(gModuleManuName[%d] error! \n", nArrIndex);
	}
	else
	{
		CCODOWN_FMT_DEBUG(nArrIndex, "AmrInfoInit() 本地私有文件获取 终端地址: \n");
		CCODOWN_BUF_DEBUG(nArrIndex, &gTerminalAddr[nArrIndex][0], 6, "地址:");
	}

	//将私有文件内容读出来，赋值到显示相关全局变量
	Ret = read_pdata_xram(gGuiInfoName[nArrIndex], (char *)&gGuiShowMessage[nArrIndex], 0, sizeof(gGuiShowMessage[nArrIndex]));
	CCODOWN_FMT_DEBUG(nArrIndex, "AmrInfoInit() 加载显示相关信息 Ret=%d...\n", Ret);
	if(Ret == 0)
	{	
		CCODOWN_FMT_DEBUG(nArrIndex, "AmrInfoInit()显示相关信息加载成功! nFrValue=%d, nAreaIdentifyState=%d, nOfflineFeelState=%d, nRefuseReportState=%d \n", 
			gGuiShowMessage[nArrIndex].nFrValue, gGuiShowMessage[nArrIndex].nAreaIdentifyState, 
			gGuiShowMessage[nArrIndex].nOfflineFeelState, gGuiShowMessage[nArrIndex].nRefuseReportState);
	}

	CCODOWN_FMT_DEBUG(nArrIndex, "AmrInfoInit() end...\n");
}


/*******************************************************************************
* 函数名称: PastTimeDealInit
* 函数功能: 时间过处理初始化
* 输入参数: 无
* 输出参数: 无
* 返 回 值: 无
*******************************************************************************/
void PastTimeDealInit(uint8 nArrIndex)
{
    GetCurBinTime(&gCurBinTime[nArrIndex]);
    memcpy(&gPreBinTime[nArrIndex], &gCurBinTime[nArrIndex], sizeof(DateTime_T));
}

/*******************************************************************************
* 函数名称: AllStateInit
* 函数功能: 状态字初始化
* 输入参数: 无
* 输出参数: 无
* 返 回 值: 无
*******************************************************************************/
void AllStateInit(uint8 nArrIndex)
{
	CCODOWN_FMT_DEBUG(nArrIndex, "AllStateInit() begin...\n");

    memset(&gTotalState[nArrIndex].ModuleUpdateDeal, 0x00, sizeof(MODULE_UPDATE_DEAL_T));
	memset(&gTotalState[nArrIndex].MeterFileModify, 0x00, sizeof(METER_FILE_MODIFY_STATE_T));
    memset(&gTotalState[nArrIndex].MeterFileCmp, 0x00, sizeof(METER_FILE_CMP_STATE_T));
    memset(&gTotalState[nArrIndex].RelayTrans, 0x00, sizeof(RELAY_TRANS_STATE_T));
	memset(&gTotalState[nArrIndex].SlaveMonitor, 0x00, sizeof(RELAY_TRANS_STATE_T));
	memset(&gTotalState[nArrIndex].SlaveMonitorTime, 0x00, sizeof(RELAY_TRANS_STATE_T));
    memset(&gTotalState[nArrIndex].HourAmr, 0x00, sizeof(RELAY_TRANS_STATE_T));
    memset(&gTotalState[nArrIndex].NormalAmr, 0x00, sizeof(RELAY_TRANS_STATE_T));
	memset(&gTotalState[nArrIndex].HplcQuery, 0x00, sizeof(HPLC_QUERY_STATE_T));
    

	//模块信息
	gModuleManuID[nArrIndex] = MMID_NULL;
	memset(&gLocalCommModuleInfo[nArrIndex], 0x00, sizeof(LOCAL_COMM_MODULE_RUNNING_PATTERN_INFO));
    
    //上电默认AFN=0x03,F10主动方式上报为查询
    CheckModuleSoftReset(1, nArrIndex);

	//创建任务链表
	InitList_Task(&PlcTaskQueue[nArrIndex]);  //载波任务队列
	memset(&gPlcTaskDetail[nArrIndex], 0x00, sizeof(gPlcTaskDetail[nArrIndex]));  //当前载波具体任务细节

	//抄表任务队列初始化
	memset(&gActiveAmrInfoList[nArrIndex], 0, sizeof(gActiveAmrInfoList[nArrIndex]));
	if (pthread_mutex_init(&gActiveAmrInfoList[nArrIndex].ActiveAmrInfoLock, NULL) != 0)
    {
        CCODOWN_FMT_DEBUG(nArrIndex, "init gActiveAmrInfoList[%d].ActiveAmrInfoLock error! \n", nArrIndex);
    }

	//穿透任务队列初始化
	memset(&gDataTransmitQueue[nArrIndex], 0, sizeof(gDataTransmitQueue[nArrIndex]));
	if (pthread_mutex_init(&gDataTransmitQueue[nArrIndex].DataTransmitLock, NULL) != 0)
    {
        CCODOWN_FMT_DEBUG(nArrIndex, "init gDataTransmitQueue[%d].DataTransmitLock error! \n", nArrIndex);
    }

    //从节点监控任务队列初始化
	memset(&gSlaveMonitorQueue[nArrIndex], 0, sizeof(gSlaveMonitorQueue[nArrIndex]));
	if (pthread_mutex_init(&gSlaveMonitorQueue[nArrIndex].SlaveMonitorLock, NULL) != 0)
    {
        CCODOWN_FMT_DEBUG(nArrIndex, "init gSlaveMonitorQueue[%d].SlaveMonitorLock error! \n", nArrIndex);
    }

	//当前载波具体任务细节(搜表)初始化
	memset(&gPlcTaskDetailSearchMeter[nArrIndex], 0, sizeof(gPlcTaskDetailSearchMeter[nArrIndex]));

	//搜表报文队列初始化
	memset(&gAmrRecvDownSearchMeter[nArrIndex], 0, sizeof(gAmrRecvDownSearchMeter[nArrIndex]));
	if (pthread_mutex_init(&gAmrRecvDownSearchMeter[nArrIndex].QueueLock, NULL) != 0)
    {
        CCODOWN_FMT_DEBUG(nArrIndex, "init gAmrRecvDownSearchMeter[%d].QueueLock error! \n", nArrIndex);
    }

	//路由操作结构体初始化
	memset(&gRouterOper[nArrIndex], 0, sizeof(gRouterOper[nArrIndex]));

	//主动上报模块信息队列初始化
	memset(&gAmrRecvDownReportModule[nArrIndex], 0, sizeof(gAmrRecvDownReportModule[nArrIndex]));
	if (pthread_mutex_init(&gAmrRecvDownReportModule[nArrIndex].QueueLock, NULL) != 0)
    {
        CCODOWN_FMT_DEBUG(nArrIndex, "init gAmrRecvDownReportModule[%d].QueueLock error! \n", nArrIndex);
    }

	CCODOWN_FMT_DEBUG(nArrIndex, "AllStateInit() end...\n");
}


/*******************************************************************************
* 函数名称: AllFlagInit
* 函数功能: 标志初始化
* 输入参数: 无
* 输出参数: 无
* 返 回 值: 无
*******************************************************************************/
void AllFlagInit(uint8 nArrIndex)
{
    gPowerOnFlag[nArrIndex] = TRUE;                        //置上电标志
}

/*******************************************************************************
* 函数名称:	ModuleCheck
* 函数功能:	载波模块是否插入检测
* 输入参数:	
* 输出参数:	
* 返 回 值:	
*******************************************************************************/
int ModuleCheck(uint8 index)
{
    int state = 0;

    if (gDevHalInfo[index].ctrlDev->local_cu_insert_state_get(gDevHalInfo[index].ctrlDev, &state) == 0)
    {
        return state;
    }

    return -1;
}

/*******************************************************************************
* 函数名称: PlcResetRouter
* 函数功能: 复位载波模块后进入死循环，等待超时后线程重启
* 输入参数: fd                  设备文件描述符
* 输出参数: 无
* 返 回 值:  >0                  成功
*           <0                  失败
*******************************************************************************/
int PlcResetRouter(uint8 index)
{
    if (gDevHalInfo[index].ctrlDev->local_cu_reset(gDevHalInfo[index].ctrlDev) == 0)
    {
        return 1;
    }

    return -1;
}

/*******************************************************************************
* 函数名称: TimeManage
* 函数功能: 时间管理函数
* 输入参数: 无
* 输出参数: 无
* 返 回 值: 无
*******************************************************************************/
void TimeManage(uint8 nArrIndex)
{
    gPastTimeFlag[nArrIndex].Word = 0;                           //清除时间标志

    GetCurBinTime(&gCurBinTime[nArrIndex]);                //刷新当前HEX码时间
    GetCurBcdTime(&gCurBcdTime[nArrIndex]);              //刷新当前BCD码时间


    //过月
    if (gCurBinTime[nArrIndex].Month != gPreBinTime[nArrIndex].Month)
    {
        //同时产生过月、过日、过时、过分、过秒标志
        gPastTimeFlag[nArrIndex].BitSect.Month = TRUE;
        gPastTimeFlag[nArrIndex].BitSect.Day = TRUE;
        gPastTimeFlag[nArrIndex].BitSect.Hour = TRUE;
        gPastTimeFlag[nArrIndex].BitSect.Minute = TRUE;
        gPastTimeFlag[nArrIndex].BitSect.Second = TRUE;
    }
    //过日
    if (gCurBinTime[nArrIndex].Day != gPreBinTime[nArrIndex].Day)
    {
        //同时产生过日、过时、过分标志
        gPastTimeFlag[nArrIndex].BitSect.Day = TRUE;
        gPastTimeFlag[nArrIndex].BitSect.Hour = TRUE;
        gPastTimeFlag[nArrIndex].BitSect.Minute = TRUE;
        gPastTimeFlag[nArrIndex].BitSect.Second = TRUE;
    }
    //过时
    else if (gCurBinTime[nArrIndex].Hour != gPreBinTime[nArrIndex].Hour)
    {
        //同时产生过时、过分标志
        gPastTimeFlag[nArrIndex].BitSect.Hour = TRUE;
        gPastTimeFlag[nArrIndex].BitSect.Minute = TRUE;
        gPastTimeFlag[nArrIndex].BitSect.Second = TRUE;
    }
    //过分
    else if (gCurBinTime[nArrIndex].Minute != gPreBinTime[nArrIndex].Minute)
    {
        //产生过分标志
        gPastTimeFlag[nArrIndex].BitSect.Minute = TRUE;
        gPastTimeFlag[nArrIndex].BitSect.Second = TRUE;
    }

    //过秒
    else if (gCurBinTime[nArrIndex].Second != gPreBinTime[nArrIndex].Second)
    {
        gPastTimeFlag[nArrIndex].BitSect.Second = TRUE;
    }
    else
    {
        usleep(10000);
    }

    memcpy(&gPreBinTime[nArrIndex], &gCurBinTime[nArrIndex], sizeof(DateTime_T));

}

/*******************************************************************************
* 函数名称: FillTaskRunInput
* 函数功能: 根据任务名填充入参
* 输入参数: 
* 输出参数: 无
* 返 回 值: 无
*******************************************************************************/
int FillTaskRunInput(PLCTASK_DETAIL *pPlcTaskDetail)
{
	int ret = -1;
	int i = 0;
	
	//补充任务信息
	while(PlcTask_FunList[i].TaskName != 0xFF)
	{
		if(PlcTask_FunList[i].TaskName == pPlcTaskDetail->TaskName)
		{
			pPlcTaskDetail->TaskStat = PlcTask_FunList[i].TaskStat;
			pPlcTaskDetail->Priority = PlcTask_FunList[i].Priority;
			pPlcTaskDetail->DelaySec = PlcTask_FunList[i].DelaySec;
			pPlcTaskDetail->pPlcTaskFun = PlcTask_FunList[i].pPlcTaskFun;
			memset(pPlcTaskDetail->strTaskName, 0, sizeof(pPlcTaskDetail->strTaskName));
			strncpy(pPlcTaskDetail->strTaskName, (char *)PlcTask_FunList[i].strTaskName, strlen((char *)PlcTask_FunList[i].strTaskName));
			ret = 1;
			break;
		}
		i++;
	}

	return ret;
}

/*******************************************************************************
* 函数名称: PlcTaskCreat
* 函数功能: 任务队列产生判断
* 输入参数: 
* 输出参数: 无
* 返 回 值: 无
*******************************************************************************/
void PlcTaskCreat(uint8    nArrIndex)
{
	if((gDevHalInfo[nArrIndex].isvalid == 0) || (1 == gReloadDevFlag[nArrIndex]))
	{
		//CCODOWN_FMT_DEBUG(nArrIndex, "PlcTaskCreat() gDevHalInfo[nArrIndex].isvalid=%d, gReloadDevFlag[nArrIndex]=%d \n", gDevHalInfo[nArrIndex].isvalid, gReloadDevFlag[nArrIndex]);
		//sleep(5);
		return;  //模块信息无效, 直接退出
	}
	
	int     ret = 0;
	uint8   aEmptyAddr[6] = {0};  //空地址
	uint8   nTtlIndex = 0;  //不同的端口号对应不同的数组下标
	nTtlIndex = nArrIndex;

	INFO_DATA_HEAD_T InfoDataHead;  //消息原始HEAD
	memset((uint8 *)&InfoDataHead, 0x00, sizeof(InfoDataHead));
	
	PLCTASK_DETAIL  stPlcTaskDetail;       //载波任务具体信息
	memset((uint8 *)&stPlcTaskDetail, 0x00, sizeof(stPlcTaskDetail));

	AMR_DATA_TRANSMIT_T       AmrDataTransmit;            //数据透传
	memset((uint8 *)&AmrDataTransmit, 0x00, sizeof(AmrDataTransmit));

	AMR_SLAVE_NODE_MONITOR_T  AmrSlaveNodeMonitor;            //从节点监控
	memset((uint8 *)&AmrSlaveNodeMonitor, 0x00, sizeof(AmrSlaveNodeMonitor));

	//判断调度管理app是否下发终端地址
	ret = memcmp(aEmptyAddr, &gTerminalAddr[nTtlIndex][0], 6);
	if(!ret)
	{
		CCODOWN_FMT_DEBUG(nTtlIndex, "PlcTaskCreat()获取终端地址失败 端口:%d \n", nTtlIndex);
		sleep(5);
		return;
	}

    //厂商模块获取成功后再执行其他任务
    if ((gModuleManuID[nTtlIndex]==MMID_NULL) && (gPlcTaskDetail[nTtlIndex].TaskName != MS_GET_MANU_ID))
	{
		CCODOWN_FMT_DEBUG(nTtlIndex, "PlcTaskCreat()上电/模块插拔获取厂商标识 端口:%d \n", nTtlIndex);

		//创建任务
		stPlcTaskDetail.TaskName = MS_GET_MANU_ID;
		stPlcTaskDetail.TaskArrIndex = nTtlIndex;
        stPlcTaskDetail.InfoMsgBuff[0] = 1; //0:不需要等待上报; 1:需要等待上报模块信息
		PlcTask_AddQueue(stPlcTaskDetail, nTtlIndex);		//上电获取厂商标识       
	}

	if (gModuleManuID[nTtlIndex]==MMID_NULL)
	{
		return;
	}

	//载波升级任务的判断
    if(MUD_IDLE == gTotalState[nTtlIndex].ModuleUpdateDeal)
    {
        if(gIsNeedUpdating == 1)
        {
            CCODOWN_FMT_DEBUG(nTtlIndex, "PlcTaskCreat()生成载波升级任务 端口:%d \n", nTtlIndex);
            gIsNeedUpdating = 0;
            stPlcTaskDetail.TaskName = MS_UPDATE_DEAL;
    	    stPlcTaskDetail.RoutrtCtrl = RCS_PAUSE;
    		stPlcTaskDetail.TaskArrIndex = nTtlIndex;
    		PlcTask_AddQueue(stPlcTaskDetail, nTtlIndex); 
        }
    }

    //本地模块地址更新任务
	if(gUpdateTerminalAddrFlag[nTtlIndex])
	{
		CCODOWN_FMT_DEBUG(nTtlIndex, "PlcTaskCreat()终端地址变化, 生成获取厂商标识 端口:%d \n", nTtlIndex);
		gUpdateTerminalAddrFlag[nTtlIndex] = FALSE;

		//创建任务 重新获取厂商标识
		stPlcTaskDetail.TaskName = MS_GET_MANU_ID;
		stPlcTaskDetail.TaskArrIndex = nTtlIndex;
        stPlcTaskDetail.InfoMsgBuff[0] = 0; //0:不需要等待上报; 1:需要等待上报模块信息
		PlcTask_AddQueue(stPlcTaskDetail, nTtlIndex);  //上电获取厂商标识 
	}
	
    //上电任务的判断
    if (gPowerOnFlag[nTtlIndex] == TRUE)
    {
    	gPowerOnFlag[nTtlIndex] = FALSE;
    	
		//创建 档案比对任务
		CCODOWN_FMT_DEBUG(nTtlIndex, "PlcTaskCreat()上电执行 档案比对任务 端口:%d \n", nTtlIndex);
		gMeterFileCompareFlag[nTtlIndex] = 1;

		//创建 频点查询任务
		CCODOWN_FMT_DEBUG(nTtlIndex, "PlcTaskCreat()上电执行 频点查询任务 端口:%d \n", nTtlIndex);
		gInitFrQueryFlag[nTtlIndex] = 1;

        //载波升级完后重启线程，首先读取通信单元信息(版本)
        GetUnitInfo(nTtlIndex);
    }

	/*
	//路由控制的判断(直接执行)
	if (gRoutrtCtrl[nTtlIndex] != RCS_NULL)
	{
		CCODOWN_FMT_DEBUG(nTtlIndex, "PlcTaskCreat()消息产生路由控制任务 端口:%d \n", nTtlIndex);
		
		//创建任务
        stPlcTaskDetail.TaskName = MS_ROUTER_DEAL;
	    stPlcTaskDetail.RoutrtCtrl = gRoutrtCtrl[nTtlIndex];
		stPlcTaskDetail.TaskArrIndex = nTtlIndex;
		PlcTask_AddQueue(stPlcTaskDetail, nTtlIndex); 	

		gRoutrtCtrl[nTtlIndex] = RCS_NULL;
	}
	*/

	//档案比对任务的判断
	if((1 == gMeterFileCompareFlag[nTtlIndex]) && (0 == gMeterFileModifyState[nTtlIndex]))
	{
		CCODOWN_FMT_DEBUG(nTtlIndex, "PlcTaskCreat()消息产生档案比对任务 端口:%d \n", nTtlIndex);
		gMeterFileCompareFlag[nTtlIndex] = 0;

		//创建任务
        stPlcTaskDetail.TaskName = MS_FILE_CMP;
	    stPlcTaskDetail.RoutrtCtrl = RCS_PAUSE;
		stPlcTaskDetail.TaskArrIndex = nTtlIndex;
		PlcTask_AddQueue(stPlcTaskDetail, nTtlIndex); 

		//判断是否需要参数初始化
		if(1 == gMeterFileClearFlag[nTtlIndex])
		{
			gMeterFilesModifyType[nTtlIndex] = FILES_MODEFY_CLEAR;
		}
		else
		{
			gMeterFilesModifyType[nTtlIndex] = FILES_MODEFY_UPDATE;
		}
		gMeterFileClearFlag[nTtlIndex] = 0;
		gMeterFileAddDelFlag[nTtlIndex] = 0;

		//档案同步状态
		gMeterFileModifyState[nTtlIndex] = 1;
	}

	//档案修改任务的判断
	if (1 == gMeterFileModifyFlag[nTtlIndex])
	{
		CCODOWN_FMT_DEBUG(nTtlIndex, "PlcTaskCreat()档案比对产生修改任务 端口:%d \n", nTtlIndex);
		gMeterFileModifyFlag[nTtlIndex] = 0;
		
		//创建任务
		stPlcTaskDetail.TaskName = MS_FILE_MODIFY;
	    stPlcTaskDetail.RoutrtCtrl = RCS_PAUSE;
		stPlcTaskDetail.TaskArrIndex = nTtlIndex;
		PlcTask_AddQueue(stPlcTaskDetail, nTtlIndex); 
	}

	//主动抄表任务的判断
	if (gActiveAmrInfoList[nTtlIndex].nActiveAmrInfoNum > 0)
	{
		//CCODOWN_FMT_DEBUG(nTtlIndex, "PlcTaskCreat()创建主动抄表任务 端口:%d \n", nTtlIndex);

		//创建任务
		stPlcTaskDetail.TaskName = MS_NORMAL_AMR;
	    stPlcTaskDetail.RoutrtCtrl = RCS_RESTART;
		stPlcTaskDetail.TaskArrIndex = nTtlIndex;
		PlcTask_AddQueue(stPlcTaskDetail, nTtlIndex);
	}

	//数据透传抄表
	if (gDataTransmitQueue[nTtlIndex].nDataTransmitNum > 0)
	{
		//CCODOWN_FMT_DEBUG(nTtlIndex, "PlcTaskCreat()消息产生数据透传任务 端口:%d \n", nTtlIndex);

		//创建任务
		stPlcTaskDetail.TaskName = MS_RELAY_TRANS;
		stPlcTaskDetail.RoutrtCtrl = RCS_PAUSE;
		stPlcTaskDetail.TaskArrIndex = nTtlIndex;
		PlcTask_AddQueue(stPlcTaskDetail, nTtlIndex);
		
	}
	
	//获取从节点监控延时
	if (gSlaveMonitorTimeQueue[nTtlIndex].nSlaveMonitorTimeNum > 0)
	{		
		//CCODOWN_FMT_DEBUG(nTtlIndex, "PlcTaskCreat()消息产生获取从节点监控延时 端口:%d \n", nTtlIndex);

		//创建任务
		stPlcTaskDetail.TaskName = MS_SLAVE_DELAY;
		stPlcTaskDetail.RoutrtCtrl = RCS_PAUSE;
		stPlcTaskDetail.TaskArrIndex = nTtlIndex;
		PlcTask_AddQueue(stPlcTaskDetail, nTtlIndex);
		
	}

	//从节点监控
	if (gSlaveMonitorQueue[nTtlIndex].nSlaveMonitorNum > 0)
	{		
		//CCODOWN_FMT_DEBUG(nTtlIndex, "PlcTaskCreat()消息产生从节点监控任务 端口:%d \n", nTtlIndex);

		//创建任务
		stPlcTaskDetail.TaskName = MS_SLAVE_MONITOR;
		stPlcTaskDetail.RoutrtCtrl = RCS_PAUSE;
		stPlcTaskDetail.TaskArrIndex = nTtlIndex;
		PlcTask_AddQueue(stPlcTaskDetail, nTtlIndex);
		
	}

	/*
	//启动搜表任务的判断
	if (gWhetherStartSearchMeter[nTtlIndex].bStartSearch)
	{
		CCODOWN_FMT_DEBUG(nTtlIndex, "PlcTaskCreat()消息产生搜表任务 端口:%d \n", nTtlIndex);
		
		//创建任务
        stPlcTaskDetail.TaskName = MS_ADDR_SEARCH;
	    stPlcTaskDetail.RoutrtCtrl = RCS_PAUSE;
		stPlcTaskDetail.TaskArrIndex = nTtlIndex;
		memcpy(stPlcTaskDetail.InfoMsgBuff, &gWhetherStartSearchMeter[nTtlIndex].stAmrMeterStartSearch, sizeof(AMR_METER_START_SEARCH_T));
		PlcTask_AddQueue(stPlcTaskDetail, nTtlIndex); 		

		gWhetherStartSearchMeter[nTtlIndex].bStartSearch = FALSE;
		memset(&gWhetherStartSearchMeter[nTtlIndex].stAmrMeterStartSearch, 0x00, sizeof(AMR_METER_START_SEARCH_T));
	}


	//暂停搜表任务的判断
	if (gPauseSearchMeterFlag[nTtlIndex])
	{
		gPauseSearchMeterFlag[nTtlIndex] = FALSE;
		if(gPlcTaskDetail[nTtlIndex].TaskName == MS_ADDR_SEARCH
			&& gPlcTaskDetail[nTtlIndex].TaskArrIndex == nTtlIndex)
		{
			CCODOWN_FMT_DEBUG(nTtlIndex, "PlcTaskCreat()消息产生暂停搜表任务 端口:%d \n", nTtlIndex);
		
			gTotalState[nTtlIndex].AddrSearch = AS_SEARCH_END;
		}
		
	}
	*/

	//获取广播延时任务的判断
	if (gWhetherGetBroadcastDelay[nTtlIndex].bGetBroadcastDelay)
	{
		CCODOWN_FMT_DEBUG(nTtlIndex, "PlcTaskCreat()消息产生获取广播延时任务 端口:%d \n", nTtlIndex);
		
		//创建任务
		stPlcTaskDetail.InfoDataHead = gWhetherGetBroadcastDelay[nTtlIndex].InfoDataHead;
        stPlcTaskDetail.TaskName = MS_BROADCAST_DELAY;
	    stPlcTaskDetail.RoutrtCtrl = RCS_PAUSE;
		stPlcTaskDetail.TaskArrIndex = nTtlIndex;
		memcpy(stPlcTaskDetail.InfoMsgBuff, &gWhetherGetBroadcastDelay[nTtlIndex].stAmrGetBroadcastDelay, sizeof(AMR_METER_START_SEARCH_T));
		PlcTask_AddQueue(stPlcTaskDetail, nTtlIndex); 
		
		gWhetherGetBroadcastDelay[nTtlIndex].bGetBroadcastDelay = FALSE;
		memset(&gWhetherGetBroadcastDelay[nTtlIndex].stAmrGetBroadcastDelay, 0x00, sizeof(AMR_METER_START_SEARCH_T));
	}

	//启动广播
	if (gWhetherDoBroadcast[nTtlIndex].bDoBroadcast)
	{
		CCODOWN_FMT_DEBUG(nTtlIndex, "PlcTaskCreat()消息产生启动广播任务 端口:%d \n", nTtlIndex);
		
		//创建任务
		stPlcTaskDetail.TaskName = MS_BROADCAST_DEAL;
	    stPlcTaskDetail.RoutrtCtrl = RCS_PAUSE;
		stPlcTaskDetail.TaskArrIndex = nTtlIndex;
		memcpy(stPlcTaskDetail.InfoMsgBuff, &gWhetherDoBroadcast[nTtlIndex].stAmrDoBroadcast, sizeof(AMR_DO_BROADCAST_T));
		PlcTask_AddQueue(stPlcTaskDetail, nTtlIndex); 
		
		gWhetherDoBroadcast[nTtlIndex].bDoBroadcast = FALSE;
		memset(&gWhetherDoBroadcast[nTtlIndex].stAmrDoBroadcast, 0x00, sizeof(AMR_METER_START_SEARCH_T));
	}

	//如果当前cco模块为窄带，则不向下查询hplc信息
	if(gLocalCommModuleInfo[nTtlIndex].LocalCommPatternWord.b1.BitSect.CommPattern == 2)//通信方式(1:窄带,2:宽带,3:小无线)
	{
		//HPLC频点查询(上电任务)
		if(gInitFrQueryFlag[nTtlIndex] > 0)
		{
			CCODOWN_FMT_DEBUG(nTtlIndex, "PlcTaskCreat()上电产生HPLC频点查询任务 端口:%d \n", nTtlIndex);
			
			//创建任务
			stPlcTaskDetail.TaskName = MS_HPLC_FR_QUERY;
		    stPlcTaskDetail.RoutrtCtrl = RCS_NULL;
			stPlcTaskDetail.TaskArrIndex = nTtlIndex;
			stPlcTaskDetail.InfoMsgBuff[0] = 0; //0:不需要上报; 1:需要上报mqtt
			PlcTask_AddQueue(stPlcTaskDetail, nTtlIndex); 

			gInitFrQueryFlag[nTtlIndex] = 0;
		}
		
		//HPLC信息上报
		if(gQueryTypeHPLC[nTtlIndex].Value > 0)
		{
			//创建任务
			stPlcTaskDetail.TaskName = MS_HPLC_REPORT;
		    stPlcTaskDetail.RoutrtCtrl = RCS_NULL;
			stPlcTaskDetail.TaskArrIndex = nTtlIndex;
			PlcTask_AddQueue(stPlcTaskDetail, nTtlIndex); 
		}

		//HPLC频点设置
		if(gFrSetFlag[nTtlIndex] > 0)
		{
			CCODOWN_FMT_DEBUG(nTtlIndex, "PlcTaskCreat()消息产生HPLC频点设置任务 端口:%d \n", nTtlIndex);
			
			//创建任务
			stPlcTaskDetail.TaskName = MS_HPLC_FR_SET;
		    stPlcTaskDetail.RoutrtCtrl = RCS_NULL;
			stPlcTaskDetail.TaskArrIndex = nTtlIndex;
			PlcTask_AddQueue(stPlcTaskDetail, nTtlIndex); 

			gFrSetFlag[nTtlIndex] = 0;
		}

		//HPLC频点查询
		if(gFrQueryFlag[nTtlIndex] > 0)
		{
			CCODOWN_FMT_DEBUG(nTtlIndex, "PlcTaskCreat()消息产生HPLC频点查询任务 端口:%d \n", nTtlIndex);
			
			//创建任务
			memcpy(&stPlcTaskDetail.InfoDataHead, &gFrQueryInfoDataHead[nTtlIndex], sizeof(INFO_DATA_HEAD_T));
			stPlcTaskDetail.TaskName = MS_HPLC_FR_QUERY;
		    stPlcTaskDetail.RoutrtCtrl = RCS_NULL;
			stPlcTaskDetail.TaskArrIndex = nTtlIndex;
			stPlcTaskDetail.InfoMsgBuff[0] = 1; //0:不需要上报; 1:需要上报mqtt
			PlcTask_AddQueue(stPlcTaskDetail, nTtlIndex); 

			gFrQueryFlag[nTtlIndex] = 0;
		}

		//HPLC台区识别
		if(gAreaIdentifyFlag[nTtlIndex] > 0)
		{
			CCODOWN_FMT_DEBUG(nTtlIndex, "PlcTaskCreat()消息产生HPLC台区识别任务 端口:%d \n", nTtlIndex);
			
			//创建任务
			stPlcTaskDetail.TaskName = MS_HPLC_AREA;
		    stPlcTaskDetail.RoutrtCtrl = RCS_NULL;
			stPlcTaskDetail.TaskArrIndex = nTtlIndex;
			PlcTask_AddQueue(stPlcTaskDetail, nTtlIndex); 

			gAreaIdentifyFlag[nTtlIndex] = 0;
		}

		//HPLC离网感知
		if(gOfflineFeelFlag[nTtlIndex] > 0)
		{
			CCODOWN_FMT_DEBUG(nTtlIndex, "PlcTaskCreat()消息产生HPLC离网感知任务 端口:%d \n", nTtlIndex);
			
			//创建任务
			stPlcTaskDetail.TaskName = MS_HPLC_OFFLINE;
		    stPlcTaskDetail.RoutrtCtrl = RCS_NULL;
			stPlcTaskDetail.TaskArrIndex = nTtlIndex;
			PlcTask_AddQueue(stPlcTaskDetail, nTtlIndex); 

			gOfflineFeelFlag[nTtlIndex] = 0;
		}

		//HPLC拒绝节点上报
		if(gRefuseNodeReportFlag[nTtlIndex] > 0)
		{
			CCODOWN_FMT_DEBUG(nTtlIndex, "PlcTaskCreat()消息产生HPLC拒绝节点上报 端口:%d \n", nTtlIndex);
			
			//创建任务
			stPlcTaskDetail.TaskName = MS_HPLC_REFUSE;
		    stPlcTaskDetail.RoutrtCtrl = RCS_NULL;
			stPlcTaskDetail.TaskArrIndex = nTtlIndex;
			PlcTask_AddQueue(stPlcTaskDetail, nTtlIndex); 

			gRefuseNodeReportFlag[nTtlIndex] = 0;
		}
	}

	//5分钟写一次档案私有文件
	//RegularWriteFilesToPData(nArrIndex, 5);

}

/*******************************************************************************
* 函数名称: CombineTaskFunInput
* 函数功能: 将任务的参数转化为任务函数的入参
* 输入参数: 无
* 输出参数: 无
* 返 回 值: >0                  成功
*           <0                  失败
*******************************************************************************/
int CombineTaskFunInput(const PLCTASK_DETAIL stPlcTaskDetail, TASK_FUN_INPUT *stTaskFunInput)
{
	stTaskFunInput->InfoDataHead = stPlcTaskDetail.InfoDataHead;
	
	stTaskFunInput->TaskStat = stPlcTaskDetail.TaskStat;
	stTaskFunInput->TaskName = stPlcTaskDetail.TaskName;
	memcpy(stTaskFunInput->strTaskName, stPlcTaskDetail.strTaskName, sizeof(stTaskFunInput->strTaskName));
	stTaskFunInput->Priority = stPlcTaskDetail.Priority;
	stTaskFunInput->RoutrtCtrl = stPlcTaskDetail.RoutrtCtrl;
	stTaskFunInput->DelaySec = stPlcTaskDetail.DelaySec;
	stTaskFunInput->PlcTaskDeal_Stat = stPlcTaskDetail.PlcTaskDeal_Stat;
	memcpy(stTaskFunInput->InfoMsgBuff, stPlcTaskDetail.InfoMsgBuff, sizeof(stTaskFunInput->InfoMsgBuff));
	stTaskFunInput->TaskArrIndex = stPlcTaskDetail.TaskArrIndex;
	return 1;
}

/*******************************************************************************
* 函数名称: PlcTaskRunRouterDeal
* 函数功能: 任务执行处理(路有操作)
* 输入参数: 
* 输出参数: 无
* 返 回 值: 无
*******************************************************************************/
void PlcTaskRunRouterDeal(uint8 nArrIndex)
{
	if((gDevHalInfo[nArrIndex].isvalid == 0) || (1 == gReloadDevFlag[nArrIndex]))
	{
		return;  //模块信息无效, 直接退出
	}

	if( gPlcTaskDetail[nArrIndex].PlcTaskDeal_Stat != TASK_INIT )
	{
		return;  //正在执行具体任务过程中, 直接退出
	}

	int Ret = 0;
	uint8	nTtlIndex = 0;	//不同的端口号对应不同的数组下标
	nTtlIndex = nArrIndex;

	PLCTASK_DETAIL	stPlcTaskDetail;	   //载波任务具体信息
	memset((uint8 *)&stPlcTaskDetail, 0x00, sizeof(stPlcTaskDetail));

	//路由控制的判断
	if (gRoutrtCtrl[nTtlIndex] != RCS_NULL)
	{
		CCODOWN_FMT_DEBUG(nTtlIndex, "PlcTaskRunRouterDeal()消息产生路由控制任务 端口:%d \n", nTtlIndex);
		
		//创建任务
		stPlcTaskDetail.TaskName = MS_ROUTER_DEAL;
		stPlcTaskDetail.RoutrtCtrl = gRoutrtCtrl[nTtlIndex];
		stPlcTaskDetail.TaskArrIndex = nTtlIndex;

		//标志清零
		gRoutrtCtrl[nTtlIndex] = RCS_NULL;

		//补充任务信息
		Ret = FillTaskRunInput(&stPlcTaskDetail);
		if(Ret < 0)
		{
			CCODOWN_FMT_DEBUG(nTtlIndex, "PlcTaskRunRouterDeal() input查找失败 Ret:%d \n", Ret);
			return;
		}

		//然后执行任务
		TASK_FUN_INPUT TASK_FUN_INPUT;
		memset(&TASK_FUN_INPUT, 0, sizeof(TASK_FUN_INPUT));
		CombineTaskFunInput(stPlcTaskDetail, &TASK_FUN_INPUT);
		
		Ret = stPlcTaskDetail.pPlcTaskFun(TASK_FUN_INPUT);
		CCODOWN_FMT_DEBUG(nTtlIndex, "stPlcTaskDetail.pPlcTaskFun(TASK_FUN_INPUT) Ret:%d \n", Ret);
	}
}


/*******************************************************************************
* 函数名称: PlcTaskRun
* 函数功能: 任务执行处理
* 输入参数: 
* 输出参数: 无
* 返 回 值: 无
*******************************************************************************/
void PlcTaskRun(uint8    nArrIndex)
{
	if((gDevHalInfo[nArrIndex].isvalid == 0) || (1 == gReloadDevFlag[nArrIndex]))
	{
		return;  //模块信息无效, 直接退出
	}
	
	int Ret = 0;
	uint8   nTtlIndex = 0;  //不同的端口号对应不同的数组下标
	nTtlIndex = nArrIndex;

	//当前任务的执行处理
    switch(gPlcTaskDetail[nTtlIndex].PlcTaskDeal_Stat)
    {
		case TASK_INIT:                         //任务的初始化操作
			{
				Ret = ListEmpty_Task(PlcTaskQueue[nTtlIndex]);
				if(Ret > 0)
				{
					//printx(" void PlcTaskRun(void) 当前任务队列为空...\n");
				}
				else
		        {
		        	CCODOWN_FMT_DEBUG(nTtlIndex, "  ######  端口[%d] PlcTaskRun(void)  ######  任务名=%d 执行 TASK_INIT  ######  \n", nTtlIndex, gPlcTaskDetail[nTtlIndex].TaskName);
					
					ListDelete_Task(&PlcTaskQueue[nTtlIndex], 1, &gPlcTaskDetail[nTtlIndex]);
					
					CCODOWN_FMT_DEBUG(nTtlIndex, "PlcTaskRun() 提取任务队列中的如下任务... \n");    //如果当前任务队列为空，则将任务队列的第一个任务作为当前任务
					PrintTaskDetail(gPlcTaskDetail[nTtlIndex], nTtlIndex);  //打印任务信息
					CCODOWN_FMT_DEBUG(nTtlIndex, "ListLength_Task(PlcTaskQueue) 任务队列剩余任务数量:%d \n", ListLength_Task(PlcTaskQueue[nTtlIndex])); 
					PrintTaskList(PlcTaskQueue[nTtlIndex], nTtlIndex);

					gPlcTaskDetail[nTtlIndex].PlcTaskDeal_Stat = FUN_DEAL;
				}
			}
			break;
		/*
		case ROUTERCTRL_DEAL:                   //路由的操作
			{
				CCODOWN_FMT_DEBUG(nTtlIndex, " ---->端口[%d] PlcTaskRun(void)    任务名 : %d 执行 ROUTERCTRL_DEAL   ------    \n", nTtlIndex, gPlcTaskDetail[nTtlIndex].TaskName);
				gPlcTaskDetail[nTtlIndex].PlcTaskDeal_Stat = ROUTERCTRL_DELAY;
			}
			break;
		
		case ROUTERCTRL_DELAY:                  //延时操作
			{
				CCODOWN_FMT_DEBUG(nTtlIndex, " ---->端口[%d] PlcTaskRun(void)    任务名 : %d 执行 ROUTERCTRL_DELAY   ------    \n", nTtlIndex, gPlcTaskDetail[nTtlIndex].TaskName);
				gPlcTaskDetail[nTtlIndex].PlcTaskDeal_Stat = FUN_DEAL;
			}
			break;
		*/
		case FUN_DEAL:                          //当前任务处理
			{
				TASK_FUN_INPUT TASK_FUN_INPUT;
				memset(&TASK_FUN_INPUT, 0, sizeof(TASK_FUN_INPUT));
				CombineTaskFunInput(gPlcTaskDetail[nTtlIndex], &TASK_FUN_INPUT);
				
				Ret = gPlcTaskDetail[nTtlIndex].pPlcTaskFun(TASK_FUN_INPUT);
				if(Ret > 0)
				{
					gPlcTaskDetail[nTtlIndex].PlcTaskDeal_Stat = TASK_INIT;
				}
			}
			break;
		/*
		case ROUTERRECOVER_DELAY:                //路由恢复延时操作
			{
				CCODOWN_FMT_DEBUG(nTtlIndex, " ---->端口[%d] PlcTaskRun(void)    任务名 : %d 执行 ROUTERRECOVER_DELAY   ------    \n", nTtlIndex, gPlcTaskDetail[nTtlIndex].TaskName);
				gPlcTaskDetail[nTtlIndex].PlcTaskDeal_Stat = ROUTERRECOVER_DEAL;
			}
			break;
		case ROUTERRECOVER_DEAL:                //路由恢复操作
			{
				CCODOWN_FMT_DEBUG(nTtlIndex, " ---->端口[%d] PlcTaskRun(void)    任务名 : %d 执行 ROUTERRECOVER_DEAL   ------    \n", nTtlIndex, gPlcTaskDetail[nTtlIndex].TaskName);
				gPlcTaskDetail[nTtlIndex].PlcTaskDeal_Stat = TASK_INIT;
			}
			break;
		*/
		
		default:
			gPlcTaskDetail[nTtlIndex].PlcTaskDeal_Stat = TASK_INIT;
			break;
	}

}

/*******************************************************************************
* 函数名称: PlcTaskRunSearchMeter
* 函数功能: 任务执行处理(搜表)
* 输入参数: 
* 输出参数: 无
* 返 回 值: 无
*******************************************************************************/
void PlcTaskRunSearchMeter(uint8 nArrIndex)
{
	if((gDevHalInfo[nArrIndex].isvalid == 0) || (1 == gReloadDevFlag[nArrIndex]))
	{
		return;  //模块信息无效, 直接退出
	}

	if(CheckModuleSoftReset(-1, nArrIndex) == 1)
	{
		return;  //未获取到cco模块信息, 直接退出
	}

	int Ret = 0;
	uint8   nTtlIndex = 0;  //不同的端口号对应不同的数组下标
	nTtlIndex = nArrIndex;

    //升级过程中停止搜表
    if(MUD_IDLE != gTotalState[nTtlIndex].ModuleUpdateDeal)
    {
        if(1 == gAmrStateQuery[nTtlIndex].nModuleSearchState)//当前正处于搜表状态
		{
			CCODOWN_FMT_DEBUG(nTtlIndex, "PlcTaskRunSearchMeter()模块升级产生暂停搜表任务 端口:%d \n", nTtlIndex);
			gTotalState[nTtlIndex].AddrSearch = AS_SEARCH_END;
            return;
		}
    }

	//当前任务的执行处理
    switch(gPlcTaskDetailSearchMeter[nTtlIndex].PlcTaskDeal_Stat)
    {
		case TASK_INIT:                         //任务的初始化操作
		{
			//启动搜表任务的判断
			if (gWhetherStartSearchMeter[nTtlIndex].bStartSearch)
			{
				CCODOWN_FMT_DEBUG(nTtlIndex, "PlcTaskRunSearchMeter()消息产生搜表任务 端口:%d \n", nTtlIndex);
				
				//创建任务
				memset(&gPlcTaskDetailSearchMeter[nTtlIndex], 0x00, sizeof(gPlcTaskDetailSearchMeter[nTtlIndex]));
			    gPlcTaskDetailSearchMeter[nTtlIndex].TaskName = MS_ADDR_SEARCH;
			    gPlcTaskDetailSearchMeter[nTtlIndex].RoutrtCtrl = RCS_PAUSE;
				gPlcTaskDetailSearchMeter[nTtlIndex].TaskArrIndex = nTtlIndex;
				memcpy(gPlcTaskDetailSearchMeter[nTtlIndex].InfoMsgBuff, &gWhetherStartSearchMeter[nTtlIndex].stAmrMeterStartSearch, sizeof(AMR_METER_START_SEARCH_T));	

				//补充任务信息
				Ret = FillTaskRunInput(&gPlcTaskDetailSearchMeter[nTtlIndex]);
				if(Ret < 0)
				{
					CCODOWN_FMT_DEBUG(nTtlIndex, "PlcTaskRunSearchMeter() input查找失败 Ret:%d \n", Ret);
					break;
				}

				//变量清零
				gWhetherStartSearchMeter[nTtlIndex].bStartSearch = FALSE;
				memset(&gWhetherStartSearchMeter[nTtlIndex].stAmrMeterStartSearch, 0x00, sizeof(AMR_METER_START_SEARCH_T));

				//转到任务处理
				gPlcTaskDetailSearchMeter[nTtlIndex].PlcTaskDeal_Stat = FUN_DEAL;
			}
		}
			break;
		case FUN_DEAL:                          //当前任务处理
		{
			//首先判断是否收到暂停搜表指令
			if (gPauseSearchMeterFlag[nTtlIndex])
			{
				gPauseSearchMeterFlag[nTtlIndex] = FALSE;
				if(1 == gAmrStateQuery[nTtlIndex].nModuleSearchState)//当前正处于搜表状态
				{
					CCODOWN_FMT_DEBUG(nTtlIndex, "PlcTaskRunSearchMeter()消息产生暂停搜表任务 端口:%d \n", nTtlIndex);
					gTotalState[nTtlIndex].AddrSearch = AS_SEARCH_END;
				}
			}

			//然后执行搜表任务
			TASK_FUN_INPUT TASK_FUN_INPUT;
			memset(&TASK_FUN_INPUT, 0, sizeof(TASK_FUN_INPUT));
			CombineTaskFunInput(gPlcTaskDetailSearchMeter[nTtlIndex], &TASK_FUN_INPUT);
			
			Ret = gPlcTaskDetailSearchMeter[nTtlIndex].pPlcTaskFun(TASK_FUN_INPUT);
			if(Ret > 0)
			{
				gPlcTaskDetailSearchMeter[nTtlIndex].PlcTaskDeal_Stat = TASK_INIT;
			}
		}
			break;
		default:
			gPlcTaskDetailSearchMeter[nTtlIndex].PlcTaskDeal_Stat = TASK_INIT;
			break;
	}
}


/*******************************************************************************
* 函数名称: GetHeadInfo
* 函数功能: 组装完整回复消息
* 输入参数: 
* 输出参数: 无
* 返 回 值: 无
*******************************************************************************/
int GetHeadInfo(INFO_DATA_T InfoMsgData, MSG_INFO_T *ptMsgInfo)
{	
	//head
	ptMsgInfo->IP = InfoMsgData.IP;
	ptMsgInfo->port = InfoMsgData.port;
	ptMsgInfo->MsgRPM = InfoMsgData.MsgRPM;
	ptMsgInfo->MsgPRIORITY = InfoMsgData.MsgPRIORITY;		
	ptMsgInfo->MsgIndex = InfoMsgData.MsgIndex;
	ptMsgInfo->MsgLabel = InfoMsgData.MsgLabel;
	if(InfoMsgData.MsgRPM == 0)  //消息回复
	{
		ptMsgInfo->SourLen = InfoMsgData.DestLen;
		memcpy(ptMsgInfo->SourAddr, InfoMsgData.DestAddr, ptMsgInfo->SourLen);
		ptMsgInfo->DestLen = InfoMsgData.SourLen;
		memcpy(ptMsgInfo->DestAddr, InfoMsgData.SourAddr, ptMsgInfo->DestLen);
	}
	else if(InfoMsgData.MsgRPM == 1)  //主动上报
	{
		ptMsgInfo->SourLen = InfoMsgData.SourLen;
		memcpy(ptMsgInfo->SourAddr, InfoMsgData.SourAddr, ptMsgInfo->SourLen);
		ptMsgInfo->DestLen = InfoMsgData.DestLen;
		memcpy(ptMsgInfo->DestAddr, InfoMsgData.DestAddr, ptMsgInfo->DestLen);
	}

	//msg
	ptMsgInfo->IID = InfoMsgData.IID_ID;
	ptMsgInfo->IOP = InfoMsgData.IOP_ID;
	ptMsgInfo->MsgLen = (uint32)InfoMsgData.nPayloadLength;
	memcpy(ptMsgInfo->MsgData, InfoMsgData.aPayloadData, ptMsgInfo->MsgLen);
	
	return 1;
}

/**********************************************************
* 函数名称：
		XADR_GetEleNum
* 功能描述：
		获取串类型数据元素长度
* 输入描述：
		indata:数据Buffer
* 返回值：
		bytes:编码的字节个数据
**********************************************************/
uint16 XADR_GetEleNum(const uint8 *indata,uint8 *bytes)
{
    uint16 EleNum;
    uint8 nbytes = 1;
    EleNum = indata[0];
    if(EleNum==0x81)
    {
        EleNum = indata[1];
        nbytes = 2;
    }
    else if(EleNum==0x82)
    {
        EleNum = indata[1];
        EleNum <<= 8;
        EleNum += indata[2];
        nbytes = 3;
    }
    else if(EleNum==0x80 || EleNum>0x82)
        return 0;
    if(bytes!=NULL)
        *bytes = nbytes;
    return EleNum;
}

/*******************************************************************************
* 函数名称: ModuleInfoDeal
* 函数功能: 模组信息解析
* 输入参数: 
* 输出参数: 无
* 返 回 值: 无
*******************************************************************************/
void ModuleInfoDeal(uint8 ModuleIndex, uint8 *RecvBuf, uint32 RecvLen)
{
	CCOUP_FMT_DEBUG("ModuleInfoDeal() ModuleIndex=%d \n", ModuleIndex);
	
	int i = 0;
	uint16 index = 0;
	uint16 lentmp = 0;
	uint8 lenoffset = 0;
	
	pthread_rwlock_wrlock(&gModuleInfo[ModuleIndex-1].ModuleLock);

	if(RecvBuf[index++] !=0)
    {
        gModuleInfo[ModuleIndex-1].IsValid = 0;
        pthread_rwlock_unlock(&gModuleInfo[ModuleIndex-1].ModuleLock);
        return;
    }
	
    lentmp = XADR_GetEleNum(&RecvBuf[index],&lenoffset);
    index+=lenoffset;
    memcpy(gModuleInfo[ModuleIndex-1].DeviceType,&RecvBuf[index],lentmp);
    index+=lentmp;
    
    lentmp = XADR_GetEleNum(&RecvBuf[index],&lenoffset);
    index+=lenoffset;
    memcpy(gModuleInfo[ModuleIndex-1].DeviceID,&RecvBuf[index],lentmp);
    index+=lentmp;
    
    lentmp = XADR_GetEleNum(&RecvBuf[index],&lenoffset);
    index+=lenoffset;
    memcpy(gModuleInfo[ModuleIndex-1].SoftVersion,&RecvBuf[index],lentmp);
    index+=lentmp;

    
    lentmp = XADR_GetEleNum(&RecvBuf[index],&lenoffset);
    index+=lenoffset;
    memcpy(gModuleInfo[ModuleIndex-1].SoftDate,&RecvBuf[index],lentmp );
    index+=lentmp ;
    
    lentmp = XADR_GetEleNum(&RecvBuf[index],&lenoffset);
    index+=lenoffset;
    memcpy(gModuleInfo[ModuleIndex-1].HardVersion,&RecvBuf[index],lentmp );
    index+=lentmp ;

    lentmp = XADR_GetEleNum(&RecvBuf[index],&lenoffset);
    index+=lenoffset;
    memcpy(gModuleInfo[ModuleIndex-1].HardDate,&RecvBuf[index],lentmp);
    index+=lentmp;

    lentmp = XADR_GetEleNum(&RecvBuf[index],&lenoffset);
    index+=lenoffset;
    memcpy(gModuleInfo[ModuleIndex-1].ManufacturerCode,&RecvBuf[index],lentmp);
    index+=lentmp;
    gModuleInfo[ModuleIndex-1].ChannelNum = RecvBuf[index++];
    CCOUP_FMT_DEBUG("DeviceType %s DeviceID %s \n",gModuleInfo[ModuleIndex-1].DeviceType,gModuleInfo[ModuleIndex-1].DeviceID);
    CCOUP_FMT_DEBUG("SoftVersion %s SoftDate %s \n",gModuleInfo[ModuleIndex-1].SoftVersion,gModuleInfo[ModuleIndex-1].SoftDate);
    CCOUP_FMT_DEBUG("HardVersion %s HardDate %s \n",gModuleInfo[ModuleIndex-1].HardVersion,gModuleInfo[ModuleIndex-1].HardDate);
    CCOUP_FMT_DEBUG("ManufacturerCode %s ChannelNum %d\n",gModuleInfo[ModuleIndex-1].ManufacturerCode,gModuleInfo[ModuleIndex-1].ChannelNum);
    for(i=0;i<gModuleInfo[ModuleIndex-1].ChannelNum;i++)
    {
        gModuleInfo[ModuleIndex-1].ChannelInfo[i].ChannelType = RecvBuf[index++];
        gModuleInfo[ModuleIndex-1].ChannelInfo[i].ChannelFunc = RecvBuf[index++];
        lentmp = XADR_GetEleNum(&RecvBuf[index],&lenoffset);
        index+=lenoffset;
        memcpy(gModuleInfo[ModuleIndex-1].ChannelInfo[i].DriveName,&RecvBuf[index],lentmp);
        index+=lentmp;
        CCOUP_FMT_DEBUG("i %d type %d func %d drivename %s\n",i,gModuleInfo[ModuleIndex-1].ChannelInfo[i].ChannelType,gModuleInfo[ModuleIndex-1].ChannelInfo[i].ChannelFunc,
            gModuleInfo[ModuleIndex-1].ChannelInfo[i].DriveName);
    }
    gModuleInfo[ModuleIndex-1].IsValid = 1;

    pthread_rwlock_unlock(&gModuleInfo[ModuleIndex-1].ModuleLock);
}


/*******************************************************************************
* 函数名称: GetModuleInfo
* 函数功能: 获取模组序号对应的模组信息
* 输入参数: 
* 输出参数: 无
* 返 回 值: 无
*******************************************************************************/
int GetModuleInfo(uint8 ModuleIndex)
{
	CCOUP_FMT_DEBUG("GetModuleInfo() 查询模组信息 ...ModuleIndex=%d \n", ModuleIndex);
    if(ModuleIndex<1 || ModuleIndex>MAX_MODULE_NUM)
    {
		return -1;
	}

	int8   SocketState = ACT_COMM_IDLE;        //运行状态
	time_t CommBeginTime;  //通讯开始时间
	uint8  RecvBuf[MAX_QUEUE_BUFF_LEN] = {0};  //socket接收缓冲区    
    int32  RecvLen = 0;                        //接收数据长度
    int8   RecvResult = 0;
    uint8  RetryCnt = 0;
    MSG_INFO_T *ptMsgInfo = NULL;
	//int ret = 0;

	uint8  aPayload[200] = {0};
	uint16 nPayloadLen = 0;

	uint16 nIID = 0;
	uint16 nIOP = 0;
	uint16 nIndex = 0;
	
    nPayloadLen += 3;
    aPayload[nPayloadLen++] = ModuleIndex;

	while(1)
	{
		switch (SocketState)
        {
	        case ACT_COMM_IDLE:
	        {
				SocketState = ACT_COMM_SEND;
	            break;
			}
			case ACT_COMM_SEND:
	        {
				//发送查询信息
				module_from_mapManager(aPayload, nPayloadLen, &nIID, &nIOP, &nIndex);

				//更新计时器
				CommBeginTime= time(NULL);

	            //转入进程间数据接收处理
	            SocketState = ACT_COMM_RECV;

	            break;
			}
			case ACT_COMM_RECV:
	        {
				memset(RecvBuf,0x00,sizeof(RecvBuf));
		    	RecvLen = PlcReadQueue(pAmrRecvUp, RecvBuf);
				
				if(RecvLen > 0)
				{
					ptMsgInfo = (MSG_INFO_T *)RecvBuf;
					
					//打印完整消息内容
					//CCOUP_FMT_DEBUG("GetModuleInfo()...ACT_COMM_RECV...通过Mqtt收到的消息为: \n");
					//PrintInfoDetail(ptMsgInfo);

					//消息过滤
					if((ptMsgInfo->IID == nIID) && (ptMsgInfo->IOP == nIOP) && (ptMsgInfo->MsgIndex == nIndex))
					{	CCOUP_FMT_DEBUG("GetModuleInfo() 查询模组信息 收到IID == %d, IOP == %d, index == %d ! \n", nIID, nIOP, nIndex);
						ModuleInfoDeal(ModuleIndex, ptMsgInfo->MsgData, ptMsgInfo->MsgLen);
						return 1;
					}
					else if(ptMsgInfo->IID == 0x0000 && ptMsgInfo->IOP == 0x0014)
					{
						CCOUP_FMT_DEBUG("GetModuleInfo() 查询模组信息 收到IID == 0x0000, IOP == 0x0014 回码错误信息! \n");
						return -1;
					}
					else
					{
						RecvResult = -1;
					}
				}
				else
				{
					RecvResult = -1;
				}
				
				if(RecvResult == -1)
	            {
	                if( abs(time(NULL)-CommBeginTime) > MAX_MODULE_DELAY_TIME)
	                {
	                    RetryCnt++;
	                    SocketState = ACT_COMM_RETRY;
	                    CCOUP_FMT_DEBUG("GetModuleInfo() 查询模组信息 超时重试RetryCnt=%d \n", RetryCnt);
	                }
	            }
				            
	            break;
			}
			case ACT_COMM_RETRY:
			{
				if(RetryCnt>3)
	            {
	                SocketState = ACT_COMM_IDLE;
	                return -1;
	            }
	            SocketState = ACT_COMM_SEND;
				
				break;
			}
	        default:
	        {
				SocketState = ACT_COMM_IDLE;
	            break;
			}
        }
		
		usleep(20000);
	}

	return 1;
}


/*******************************************************************************
* 函数名称: GetAllModuleInfo
* 函数功能: 查询所有模组信息
* 输入参数: 
* 输出参数: 无
* 返 回 值: 无
*******************************************************************************/
int GetAllModuleInfo()
{
    uint8 i = 0;
	uint8 result[MAX_MODULE_NUM] = {0};  //模组信息获取结果
    
    for(i=0; i<MAX_MODULE_NUM; i++)
    {
        memset(&gModuleInfo[i],0x00,sizeof(MODULE_INFO_T));
        if ( pthread_rwlock_init ( &gModuleInfo[i].ModuleLock, NULL ) != 0 )
        {
            CCOUP_FMT_DEBUG ("init gModuleInfo lock %i error! \n", i);
            continue;
        }
        CCOUP_FMT_DEBUG("开始获取模组%d信息！\n", i+1);
		
        if(GetModuleInfo(i+1) > 0)
        {
            CCOUP_FMT_DEBUG("获取模组%d信息成功！\n", i+1);
			result[i] = 1;
        }
		else
        {
            CCOUP_FMT_DEBUG("获取模组%d信息失败！\n", i+1);
			result[i] = 0;
        }
    }

	if((result[0]==0) && (result[1]==0))//没有本地模块
	{
		return -1;
	}

	return 1;
}

/*******************************************************************************
* 函数名称: UpdateModuleFiles
* 函数功能: 模组插拔导致 更新模组档案信息
* 输入参数: 
* 输出参数: 无
* 返 回 值: 无
*******************************************************************************/
int UpdateModuleFiles()
{
	CCOUP_FMT_DEBUG("UpdateModuleFiles() 由于插拔导致的模组档案更新... \n");
	uint8 i = 0;

	for(i=0; i<MAX_ALLOWED_DOWN_THREAD_NUM; i++)
	{
		if(gDevHalInfo[i].isvalid == 1)
		{
			gMeterFileCompareFlag[i] = 1;//更新cco模块档案
			gReloadDevFlag[i] = 1;//更新串口操作句柄
			gModuleManuID[i] = MMID_NULL;//更新模块信息
			CheckModuleSoftReset(1, i);//主动查询模块信息标志置1
			ClearCommQueueBuff(&gAmrRecvDownReportModule[i]);//清空主动上报模块信息
			
			CCOUP_FMT_DEBUG("UpdateModuleFiles() i=%d, gDevHalInfo[i].isvalid=%d, gMeterFileCompareFlag[i]=%d, gReloadDevFlag[i]=%d, gModuleManuID[i]=%d \n", 
				i, gDevHalInfo[i].isvalid, gMeterFileCompareFlag[i], gReloadDevFlag[i], gModuleManuID[i]);
		}
	}

	return 1;
}

/*******************************************************************************
* 函数名称: UpdateModuleInfo
* 函数功能: 更新模组信息
* 输入参数: 
* 输出参数: 无
* 返 回 值: 无
*******************************************************************************/
int UpdateModuleInfo()
{
	CCOUP_FMT_DEBUG("UpdateModuleInfo() 由于初始化/插拔导致的模组信息更新 begin... \n");
	uint8 i = 0;
	uint8 j = 0;
	uint8 count = 0;
    int pipe = 0;
    int ret = 0;
    uint8 gCcoFlagTemp[5] = {0};//是否CCO模块标记
    BOOL bCCOModuleChanged = FALSE;//CCO模块是否发生变化

    //记录最新的CCO模块槽位下标信息
    for(i=0; i<MAX_MODULE_NUM; i++)
	{
	    pthread_rwlock_rdlock(&gModuleInfo[i].ModuleLock);

		if(gModuleInfo[i].IsValid==1)
		{
		    /* 判断是否CCO模块 */
            if(0 == memcmp(gModuleInfo[i].DeviceType, "GBH", 3))
            {
                gCcoFlagTemp[i] = 1;
            }			
		}
		
		pthread_rwlock_unlock(&gModuleInfo[i].ModuleLock);
	}

    //判断CCO模块是否发生变位
    for(i=0; i<MAX_MODULE_NUM; i++)
	{
	    if(gCcoFlagTemp[i] != gCcoFlag[i])
        {
            bCCOModuleChanged = TRUE;
            CCOUP_FMT_DEBUG("CCO模块gModuleInfo[%d]发生变化, 需要更新! \n", i);
        }
	}

    if(FALSE == bCCOModuleChanged)
    {
        CCOUP_FMT_DEBUG("CCO模块位置没有发生变化, 不进行更新操作! \n");
        return -1;
    }

    //初始化之前需要先释放
    for(pipe=0; pipe<MAX_DOWN_THREAD_NUM; pipe++)
    {
        if(NULL != gDevHalInfo[pipe].dev)
        {
            gDevHalInfo[pipe].isvalid = 0;
            ret = hal_device_release((HW_DEVICE *)gDevHalInfo[pipe].dev);
            gDevHalInfo[pipe].dev = NULL;
            CCODOWN_FMT_DEBUG(pipe, "hal_device_release() ret=%d \n", ret);
        }
    }

	memset(gDevHalInfo, 0x00, sizeof(gDevHalInfo));
    memset(gCcoFlag, 0x00, sizeof(gCcoFlag));
    
	for(i=0; i<MAX_MODULE_NUM; i++)
	{
		pthread_rwlock_rdlock(&gModuleInfo[i].ModuleLock);

		if(gModuleInfo[i].IsValid==1)
		{
			for(j=0; j<gModuleInfo[i].ChannelNum; j++)
			{
				if(gModuleInfo[i].ChannelInfo[j].ChannelFunc==2)
                {
                    if(count >= MAX_ALLOWED_DOWN_THREAD_NUM)
                    {
                        pthread_rwlock_unlock(&gModuleInfo[i].ModuleLock);
                        break;
                    }
                    sprintf(gDevHalInfo[count].deviceName, "%s%d_%d", CCO_MODULE_DEVNAME, i+1, j);

                    gDevHalInfo[count].isvalid = 1;
					gDevHalInfo[count].moduleIndex = i+1;
                    count++;                    

					gModuleInfo[i].bCCO = TRUE;//含有本地cco功能
					gCcoFlag[i] = 1;//备份cco通道
                }
			}
		}
		
		pthread_rwlock_unlock(&gModuleInfo[i].ModuleLock);
	}

	CCOUP_FMT_DEBUG("UpdateModuleInfo() 由于初始化/插拔导致的模组信息更新 end... \n");
	return 1;
}

/*******************************************************************************
* 函数名称: GlobalInit
* 函数功能: 初始化函数
* 输入参数:
* 输出参数:
* 内部处理:
* 返 回 值: 
*******************************************************************************/
int GlobalInit()
{
	memset(gDevHalInfo, 0x00, sizeof(gDevHalInfo));
	memset(gModuleInfo, 0x00, sizeof(gModuleInfo));

	memset(gActiveAmrInfoList, 0x00, sizeof(gActiveAmrInfoList));
	memset(gGlobalSeqInfo, 0x00, sizeof(gGlobalSeqInfo));
	memset(gWhetherStartSearchMeter, 0x00, sizeof(gWhetherStartSearchMeter));
	memset(gDataTransmitQueue, 0x00, sizeof(gDataTransmitQueue));
	memset(gSlaveMonitorQueue, 0x00, sizeof(gSlaveMonitorQueue));
	memset(gWhetherGetBroadcastDelay, 0x00, sizeof(gWhetherGetBroadcastDelay));
	memset(gWhetherDoBroadcast, 0x00, sizeof(gWhetherDoBroadcast));
	memset(gQueryTypeHPLC, 0x00, sizeof(gQueryTypeHPLC));
	memset(gAmrStateQuery, 0x00, sizeof(gAmrStateQuery));
	memset(gFrQueryInfoDataHead, 0x00, sizeof(gFrQueryInfoDataHead));
	memset(gSlaveMonitorTimeQueue, 0x00, sizeof(gSlaveMonitorTimeQueue));
	memset(gAmrModuleQuery, 0x00, sizeof(gAmrModuleQuery));
	memset(gGuiShowMessage, 0x00, sizeof(gGuiShowMessage));
    memset(gAllCcoStaInfo, 0x00, sizeof(gAllCcoStaInfo));

	return 1;
}


/*******************************************************************************
* 函数名称: ModuleEventFun
* 函数功能: 处理模组插拔事件
* 输入参数:
* 输出参数:
* 内部处理:
* 返 回 值: 
*******************************************************************************/
void ModuleEventFun(uint8* buf, uint16 len)
{
    if(len < 5)
    {
		 return;
	}

    uint32 ModuleIndex = 0;
    uint16 index = 0;
    uint8 status = 0;
	BOOL bCCOModule = FALSE;

    //模组序号
    memcpy_r((uint8*)&ModuleIndex, &buf[index], 4);
    index += 4;
    status = buf[index++];//插拔状态 0插 1拔
    
    CCOUP_FMT_DEBUG("收到模块插拔事件 模组序号=%d, status=%d, 0插 1拔\n", ModuleIndex, status);
	
    if(ModuleIndex<1 || ModuleIndex>5)
    {
		return;
	}

    if(status == 0)
    {
        //通知主线程读取该序号消息
        gReloadModuleFlag[ModuleIndex] = 1;
    }
    else if(status == 1)
    {
        //判断是否有CCO模块拔出
        if(1 != gCcoFlag[ModuleIndex-1])
        {
            CCOUP_FMT_DEBUG("槽位下标ModuleIndex=%d非CCO模块, 不操作! \n", ModuleIndex);
            return;
        }
        
        //直接删除该序号对应信息
        pthread_rwlock_wrlock(&gModuleInfo[ModuleIndex-1].ModuleLock);
        gModuleInfo[ModuleIndex-1].IsValid = 0;
        pthread_rwlock_unlock(&gModuleInfo[ModuleIndex-1].ModuleLock); 

		//(拔出需要在更新之前判断是否本地模块)
		bCCOModule = gModuleInfo[ModuleIndex-1].bCCO;

		UpdateModuleInfo();

		//判断拔出是否本地模块  
		if( bCCOModule == TRUE )
		{
			//更新本地模块档案信息
			UpdateModuleFiles();
		}
    }
}


/*******************************************************************************
* 函数名称: ModuleEventDeal
* 函数功能: 处理模组插拔事件
* 输入参数:
* 输出参数:
* 内部处理:
* 返 回 值: 
*******************************************************************************/
int ModuleEventDeal()
{
	int ModuleIndex = 1;
	int num = sizeof(gReloadModuleFlag);
	int ret = -1;
	
	for(ModuleIndex=1; ModuleIndex<num; ModuleIndex++)
	{
		CCOUP_FMT_DEBUG("ModuleEventDeal()...gReloadModuleFlag[%d]=%d \n", ModuleIndex, gReloadModuleFlag[ModuleIndex]);
		if(gReloadModuleFlag[ModuleIndex] == 1)
		{
			if(GetModuleInfo(ModuleIndex) > 0)
			{
				gReloadModuleFlag[ModuleIndex] = 0;
				ret = ModuleIndex;
			}
			else
			{
				ret = -1;
			}

			break;
		}
	}

	return ret;
}


/*******************************************************************************
* 函数名称: gui_hplc_fr_query
* 函数功能: 显示GUI 查看当前HPLC频点
* 输入参数: uint8 ModuleID 本地模块序号(1或者2)
* 输出参数: uint8 *Fr 频点值
* 返 回 值: 大于0：正确；小于0：错误
*******************************************************************************/
int gui_hplc_fr_query(uint8 ModuleID, uint8 *Fr)
{
	CCOGUI_FMT_DEBUG("gui_hplc_fr_query() ModuleID=%d \n", ModuleID);

	if((ModuleID<1) || (ModuleID>2))
	{
		CCOGUI_FMT_DEBUG("gui_hplc_fr_query() ModuleID=%d 非法! \n", ModuleID);
		return -1;
	}

	*Fr = gGuiShowMessage[ModuleID-1].nFrValue;
	return 1;
}


/*******************************************************************************
* 函数名称: gui_amr_port_query
* 函数功能: 显示GUI 查看当前抄表端口信息
* 输入参数: uint8 ModuleID 本地模块序号(1或者2)
* 输出参数: uint32 *Port 端口号
* 返 回 值: 大于0：正确；小于0：错误
*******************************************************************************/
int gui_amr_port_query(uint8 ModuleID, uint32 *Port)
{
	CCOGUI_FMT_DEBUG("gui_amr_port_query() ModuleID=%d \n", ModuleID);

	if((ModuleID<1) || (ModuleID>2))
	{
		CCOGUI_FMT_DEBUG("gui_amr_port_query() ModuleID=%d 非法! \n", ModuleID);
		return -1;
	}

	*Port = gPortID[ModuleID-1];
	return 1;
}

/*******************************************************************************
* 函数名称: gui_module_addr_query
* 函数功能: 显示GUI 查看当前模块地址
* 输入参数: uint8 ModuleID 本地模块序号(1或者2)
			uint8 Len 地址缓冲区长度(6)
* 输出参数: uint8 *Addr 模块地址
* 返 回 值: 大于0：正确；小于0：错误
*******************************************************************************/
int gui_module_addr_query(uint8 ModuleID, uint8 Len, uint8 *Addr)
{
	CCOGUI_FMT_DEBUG("gui_module_addr_query() ModuleID=%d, Len=%d \n", ModuleID, Len);

	if((ModuleID<1) || (ModuleID>2))
	{
		CCOGUI_FMT_DEBUG("gui_module_addr_query() ModuleID=%d 非法! \n", ModuleID);
		return -1;
	}

	if(Len != 6)
	{
		CCOGUI_FMT_DEBUG("gui_module_addr_query() Len=%d 非法! \n", Len);
		return -1;
	}

	memcpy(Addr, &gTerminalAddr[ModuleID-1][0], Len);
	return 1;
}


/*******************************************************************************
* 函数名称: gui_module_info_query
* 函数功能: 显示GUI 查看模块参数及信息
* 输入参数: uint8 ModuleID 本地模块序号(1或者2)
* 输出参数: AMR_QUERY_MODULE_INFO_T *ModuleInfo 模块信息
* 返 回 值: 大于0：正确；小于0：错误
*******************************************************************************/
int gui_module_info_query(uint8 ModuleID, AMR_QUERY_MODULE_INFO_T *ModuleInfo)
{
	CCOGUI_FMT_DEBUG("gui_module_info_query() ModuleID=%d \n", ModuleID);

	if((ModuleID<1) || (ModuleID>2))
	{
		CCOGUI_FMT_DEBUG("gui_module_info_query() ModuleID=%d 非法! \n", ModuleID);
		return -1;
	}

	memcpy_r(&gAmrModuleQuery[ModuleID-1].aModuleID[0], &gTerminalAddr[ModuleID-1][0], 6);//终端地址
	gAmrModuleQuery[ModuleID-1].nCurrentSlaveNum = gAmrMeterFilesList[ModuleID-1].nMeterFilesNum;//当前从节点数量

	memcpy(ModuleInfo, &gAmrModuleQuery[ModuleID-1], sizeof(AMR_QUERY_MODULE_INFO_T));
	return 1;
}


/*******************************************************************************
* 函数名称: gui_amr_state_query
* 函数功能: 显示GUI 查询当前抄表状态
* 输入参数: uint8 ModuleID 本地模块序号(1或者2)
* 输出参数: AMR_QUERY_READING_STATE_T *AmrState 抄表状态
* 返 回 值: 大于0：正确；小于0：错误
*******************************************************************************/
int gui_amr_state_query(uint8 ModuleID, AMR_QUERY_READING_STATE_T *AmrState)
{
	CCOGUI_FMT_DEBUG("gui_amr_state_query() ModuleID=%d \n", ModuleID);

	if((ModuleID<1) || (ModuleID>2))
	{
		CCOGUI_FMT_DEBUG("gui_amr_state_query() ModuleID=%d 非法! \n", ModuleID);
		return -1;
	}

	memcpy(AmrState, &gAmrStateQuery[ModuleID-1], sizeof(AMR_QUERY_READING_STATE_T));
	return 1;
}


/*******************************************************************************
* 函数名称: gui_area_iden_query
* 函数功能: 显示GUI 查看当前HPLC台区识别开启状态
* 输入参数: uint8 ModuleID 本地模块序号(1或者2)
* 输出参数: 无
* 返 回 值: >=0:开关状态； -1:错误
*******************************************************************************/
int gui_area_iden_query(uint8 ModuleID)
{
	CCOGUI_FMT_DEBUG("gui_area_iden_query() ModuleID=%d \n", ModuleID);

	if((ModuleID<1) || (ModuleID>2))
	{
		CCOGUI_FMT_DEBUG("gui_area_iden_query() ModuleID=%d 非法! \n", ModuleID);
		return -1;
	}

	return gGuiShowMessage[ModuleID-1].nAreaIdentifyState;
}

/*******************************************************************************
* 函数名称: gui_offline_feel_query
* 函数功能: 显示GUI 查看当前HPLC离网感知开启状态
* 输入参数: uint8 ModuleID 本地模块序号(1或者2)
* 输出参数: 无
* 返 回 值: >=0:开关状态； -1:错误
*******************************************************************************/
int gui_offline_feel_query(uint8 ModuleID)
{
	CCOGUI_FMT_DEBUG("gui_offline_feel_query() ModuleID=%d \n", ModuleID);

	if((ModuleID<1) || (ModuleID>2))
	{
		CCOGUI_FMT_DEBUG("gui_offline_feel_query() ModuleID=%d 非法! \n", ModuleID);
		return -1;
	}

	return gGuiShowMessage[ModuleID-1].nOfflineFeelState;
}

/*******************************************************************************
* 函数名称: gui_refuse_report_query
* 函数功能: 显示GUI 查看当前HPLC拒绝节点上报开启状态
* 输入参数: uint8 ModuleID 本地模块序号(1或者2)
* 输出参数: 无
* 返 回 值: >=0:开关状态； -1:错误
*******************************************************************************/
int gui_refuse_report_query(uint8 ModuleID)
{
	CCOGUI_FMT_DEBUG("gui_refuse_report_query() ModuleID=%d \n", ModuleID);

	if((ModuleID<1) || (ModuleID>2))
	{
		CCOGUI_FMT_DEBUG("gui_refuse_report_query() ModuleID=%d 非法! \n", ModuleID);
		return -1;
	}

	return gGuiShowMessage[ModuleID-1].nRefuseReportState;
}

/*******************************************************************************
* 函数名称: gui_files_count_query
* 函数功能: 显示GUI 查看档案数量
* 输入参数: uint8 ModuleID 本地模块序号(1或者2)
* 输出参数: 无
* 返 回 值: >=0:档案数量； -1:错误
*******************************************************************************/
int gui_files_count_query(uint8 ModuleID)
{
	CCOGUI_FMT_DEBUG("gui_files_count_query() ModuleID=%d \n", ModuleID);

	if((ModuleID<1) || (ModuleID>2))
	{
		CCOGUI_FMT_DEBUG("gui_files_count_query() ModuleID=%d 非法! \n", ModuleID);
		return -1;
	}

	return gAmrMeterFilesList[ModuleID-1].nMeterFilesNum;
}

/*******************************************************************************
* 函数名称: gui_single_files_query
* 函数功能: 显示GUI 查看单个档案信息
* 输入参数: uint8 ModuleID 本地模块序号(1或者2)
			uint16 index 档案列表数组下标(从0开始)
* 输出参数: AMR_METER_FILES_INFO_T *FilesInfo 单个档案信息
* 返 回 值: 大于0：正确；小于0：错误
*******************************************************************************/
int gui_single_files_query(uint8 ModuleID, uint16 index, AMR_METER_FILES_INFO_T *FilesInfo)
{
	CCOGUI_FMT_DEBUG("gui_single_files_query() ModuleID=%d, index=%d \n", ModuleID, index);

	if((ModuleID<1) || (ModuleID>2))
	{
		CCOGUI_FMT_DEBUG("gui_single_files_query() ModuleID=%d 非法! \n", ModuleID);
		return -1;
	}

	if(index >= gAmrMeterFilesList[ModuleID-1].nMeterFilesNum)
	{
		CCOGUI_FMT_DEBUG("gui_single_files_query() index=%d 非法! \n", index);
		return -1;
	}

	memcpy(FilesInfo, &gAmrMeterFilesList[ModuleID-1].stMeterFilesData[index], sizeof(AMR_METER_FILES_INFO_T));

	return 1;
}

/*******************************************************************************
* 函数名称: gui_restart_app
* 函数功能: 显示GUI 对本应用软件重启
* 输入参数: uint8 ModuleID 本地模块序号(1或者2)
* 输出参数: 无
* 返 回 值: 大于0：正确；小于0：错误
*******************************************************************************/
int gui_restart_app(uint8 ModuleID)
{
	CCOGUI_FMT_DEBUG("gui_restart_app() ModuleID=%d \n", ModuleID);

	if((ModuleID<1) || (ModuleID>2))
	{
		CCOGUI_FMT_DEBUG("gui_restart_app() ModuleID=%d 非法! \n", ModuleID);
		return -1;
	}

	return 1;
}


/*******************************************************************************
* 函数名称: gui_hardware_init
* 函数功能: 显示GUI 硬件复位路由
* 输入参数: uint8 ModuleID 本地模块序号(1或者2)
* 输出参数: 无
* 返 回 值: 大于0：正确；小于0：错误
*******************************************************************************/
int gui_hardware_init(uint8 ModuleID)
{
	CCOGUI_FMT_DEBUG("gui_hardware_init() ModuleID=%d \n", ModuleID);

	if((ModuleID<1) || (ModuleID>2))
	{
		CCOGUI_FMT_DEBUG("gui_hardware_init() ModuleID=%d 非法! \n", ModuleID);
		return -1;
	}

	return 1;
}

/*******************************************************************************
* 函数名称: gui_param_init
* 函数功能: 显示GUI 初始化参数
* 输入参数: uint8 ModuleID 本地模块序号(1或者2)
* 输出参数: 无
* 返 回 值: 大于0：正确；小于0：错误
*******************************************************************************/
int gui_param_init(uint8 ModuleID)
{
	CCOGUI_FMT_DEBUG("gui_param_init() ModuleID=%d \n", ModuleID);

	if((ModuleID<1) || (ModuleID>2))
	{
		CCOGUI_FMT_DEBUG("gui_param_init() ModuleID=%d 非法! \n", ModuleID);
		return -1;
	}

	return 1;
}

/*******************************************************************************
* 函数名称: gui_software_info_query
* 函数功能: 显示GUI 查询软件版本和发布日期
* 输入参数: uint8 ModuleID 本地模块序号(1或者2)
* 输出参数: 无
* 返 回 值: 大于0：正确；小于0：错误
*******************************************************************************/
int gui_software_info_query(uint8 ModuleID, MODULE_INFO_T *pModuleInfo)
{
	CCOGUI_FMT_DEBUG("gui_software_info_query() ModuleID=%d \n", ModuleID);

	if((ModuleID<1) || (ModuleID>2))
	{
		CCOGUI_FMT_DEBUG("gui_software_info_query() ModuleID=%d 非法! \n", ModuleID);
		return -1;
	}

	memcpy(pModuleInfo, &gModuleInfo[ModuleID-1], sizeof(MODULE_INFO_T));

	return 1;
}


