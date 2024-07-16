/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief：参数变更处理
* @date： 2019.12.03
*********************************************************************
*/

#include "oopVarUpdate.h"
#include "db_api.h"
#include "oopStorage.h"
#include "macro.h"
#include "hal.h"
#include "audio.h"
#include "iniparser.h"
#include "EventIO.h"
#include "appctrl_module.h"
#include "area.h"

/* 复位标记 */
uint32 resetFlag = 0x00;
uint32 datainitfalg=0;
uint32 writesoft = 0x00;

/* 系统管理器返回的复位标记 */
SIMOS_CTRL_E resetSmiOSFlag = SIMOS_NOACT;

/* 修改IP */
IPADDR_SET_T ipaddrSet;

/* 档案的缓存 */
OOP_METER_P_LIST_T oopMeterList;

/* 任务的缓存 */
OOP_TASK_LIST_T oopTaskList;

/* 上报方案的缓存 */
OOP_PLAN_REPORT_LIST_T oopPlanReportList;

//普通采集方案的缓存
OOP_PLAN_NORMAL_LIST_T oopPlanNormalList;

//事件采集方案的缓存
OOP_PLAN_EVENT_LIST_T oopPlanEventList;

//透明采集方案的缓存
OOP_PLAN_Transparent_LIST_T oopPlanTransparentList;

//搜表相关参数 0x60020800
OOP_SEARCHPARAM_G_T oopSearchParam;

//保电状态
OOP_GUARD_T oopGuard;

//遥控状态
OOP_REMOTE_CTRL_T oopRemoteCtrl;

//搜表结果据缓存
OOP_SEEK_METER_LIST_T seekMeterList;

//转加密结果据缓存
OOP_ENCRYPTION_RES_LIST_T encryptionList;

//跨台区搜表结果据缓存
OOP_SEEK_ACROSS_LIST_T seekAcrossList;

#ifdef AREA_ZHEJIANG
//拓扑关系识别信息
CBIR_RESULT_LIST_T cbirResultList;
#endif

//精准校时参数
OOP_SYNCCLOCK_TIMING_T syncClockParam;

//时间同步周期
OOP_SYNC_CYCLE_T syncCycleParam;

//建立计算剩余电量的互斥信号量
pthread_mutex_t calRemainEnLock;

//更新esam密钥下发状态
OOP_ESAM_STATE_T esamState;
extern int creat_smlog_event(int clientid, uint8 logicId, uint16 infoNum, uint32 value);
extern int creat_unmeterlog_event(int clientid, uint8 logicId, uint16 infoNum, uint32 value);
extern OOP_PERCEPTION_TERMINAL_LIST_T perceptionterminal;

//用于远程升级的版本信息
char update_version[64] = {0};

//45100400以太网网络配置
OOP_ETHIP_INFO_T  ethIPinfo;

//APP组件信息
OOP_REG_APP_INFO_LIST_T oopRegAppInfoList;

//载波字节版本信息
OOP_NODEVERSIONINFO_LIST_T nodeVersionList;

//子节点信息
OOP_CHILDNODE_LIST_T childNodeList;

//交采透抄口开启标志
uint32 acRelay485Flag = 0;

#ifdef AREA_FUJIAN
OOP_GROUPMETER_LIST_T groupMeterList;
OOPM_FTP_INFO_T       ftpInfo;
#endif

#if DESC("子节点变更", 1)
/**********************************************************************
* @name      : childnode_list_lock_init
* @brief     ：
* @param[out]：
* @return    ：
* @Date      ：2020-9-9
* @Update    :
**********************************************************************/
void childnode_list_lock_init(void)
{
    /* 初始化锁 */
    memset(&childNodeList, 0, sizeof(OOP_CHILDNODE_LIST_T));
    pthread_mutex_init(&childNodeList.mutex, NULL);
}

/**
*********************************************************************
* @name：      appdb_childnode_list_record_init
* @brief：     节点信息变更初始化(记录数据方式)
* @param[in] ：clientid  数据库句柄
* @param[out]：
* @return    ：
* @author    : 
* @Date      ：2020-9-9
*********************************************************************
*/
void appdb_childnode_list_record_init(int clientid)
{
    int                   ret = ERR_NORMAL;
    uint32                i = 0;
    uint32                childNodeNum = 0;
    OOP_CHILDNODE_T       childNode;
    READ_RECORD_T         inReadRecord;
    uint32                handle = 0;
    uint32                recNum = 0;
    STEP_READ_RECORD_T    stepReadRecord;
    RECORD_UNIT_T         stepRecordOut;

    memset(&childNodeList, 0, sizeof(OOP_CHILDNODE_LIST_T));
    memset(&inReadRecord, 0, sizeof(READ_RECORD_T));
    memset(&stepReadRecord, 0, sizeof(STEP_READ_RECORD_T));
    memset(&stepRecordOut, 0, sizeof(RECORD_UNIT_T));
    memset(&childNode, 0, sizeof(OOP_CHILDNODE_T));

    PRTL_FMT_DEBUG("Initializing childnode. please wait......\n");
    inReadRecord.recordOAD.logicId = 0;
    inReadRecord.recordOAD.infoNum = 0xffff;
    inReadRecord.cType = COL_NO_INFO;
    inReadRecord.cStart = 1;
    inReadRecord.cEnd = NO_VIR_MTR_MAX_NUM + 1;
    inReadRecord.sortType = UP_SORT;
    inReadRecord.recordOAD.optional = 0;
    inReadRecord.recordOAD.road.oadMain.value = 0;
    inReadRecord.recordOAD.road.oadCols.nNum = 0x01;
    inReadRecord.recordOAD.road.oadCols.oad[0].value = 0xF2090500;

    //不需要附加校验
    stepReadRecord.MAC.nNum = 0;

    ret = db_read_record_start(clientid, &inReadRecord, &handle, &recNum);
    if ((ret != ERR_OK) || (recNum == 0))
    {
        PRTL_FMT_DEBUG("appdb_childnode_list_record_init db_read_record_start err! ret = %d, infoNum = %d recNum = %d\n", 
            ret, inReadRecord.recordOAD.infoNum, recNum);
    }
    else
    {
        PRTL_FMT_DEBUG("appdb_childnode_list_record_init db_read_record_start ok ret = %d, infoNum = %d recNum = %d\n", 
            ret, inReadRecord.recordOAD.infoNum, recNum);
        for (i = 0; i < recNum; i++)
        {
            ret = db_read_record_step(clientid, handle, &stepReadRecord, &stepRecordOut);
            if (ret != ERR_OK)
            {
                PRTL_FMT_DEBUG("appdb_childnode_list_record_init db_read_record_step err = %d!\n", ret);
                continue;
            }

            //检查数据有效性
            if ((stepRecordOut.recordnum != 1) 
            || (stepRecordOut.record[0].road.oadCols.oad[0].value != 0xF2090500)
            || (stepRecordOut.record[0].outDatalen != sizeof(OOP_CHILDNODE_T)))
            {
                PRTL_FMT_DEBUG("appdb_childnode_list_record_init data err !\n");
                PRTL_FMT_DEBUG("appdb_childnode_list_record_init stepRecordOut.recordnum = %d\n", stepRecordOut.recordnum);
                PRTL_FMT_DEBUG("appdb_childnode_list_record_init stepRecordOut.record[0].road.oadCols.oad[0] = 0x%08x\n", 
                    stepRecordOut.record[0].road.oadCols.oad[0].value);
                PRTL_FMT_DEBUG("appdb_childnode_list_record_init stepRecordOut.record[0].outDatalen = %d\n", 
                    stepRecordOut.record[0].outDatalen);
                PRTL_FMT_DEBUG("appdb_childnode_list_record_init sizeof(OOP_CHILDNODE_T) = %d\n", 
                    sizeof(OOP_CHILDNODE_T));
                continue;
            }

            memcpy(&childNode, stepRecordOut.record[0].outDataBuf, stepRecordOut.record[0].outDatalen);
            PRTL_FMT_DEBUG("childNode.index = %d\n", childNode.index);
            PRTL_BUF_DEBUG(&childNode.add.add[0], childNode.add.len, "childNode.addr:");
            PRTL_BUF_DEBUG(&childNode.id.value[0], childNode.id.nNum, "childNode.id:");
//            PRTL_FMT_DEBUG("childNode.wResponse = %d\n", childNode.wResponse);
//            PRTL_FMT_DEBUG("childNode.dtLastOk.year = %d\n", childNode.dtLastOk.year);
//            PRTL_FMT_DEBUG("childNode.dtLastOk.month = %d\n", childNode.dtLastOk.month);
//            PRTL_FMT_DEBUG("childNode.dtLastOk.day = %d\n", childNode.dtLastOk.day);

            childNodeNum++;

            //序号从1开始
            childNodeList.isChanged[stepRecordOut.infoNum] = TRUE;
            childNodeList.isVaild[stepRecordOut.infoNum] = TRUE;
            childNodeList.childNode[stepRecordOut.infoNum] = childNode;
            
        }
    }
    db_read_record_end(clientid, handle);
    
    PRTL_FMT_DEBUG("Initialize childnode end.\n");

    /* 记录任务数量 */
    childNodeList.childNodeNum = childNodeNum;

    PRTL_FMT_DEBUG("read childnode num: %d\n", childNodeList.childNodeNum);

    return;
}

/**********************************************************************
* @name      : app_get_childnode_list
* @brief     ：从缓存中获取子节点信息
* @param[in] ：infoNum   - 序号          
* @param[out]: nodeversion - 数据
* @return    : 错误码
**********************************************************************/
int app_get_childnode_list(uint16 infoNum, OOP_CHILDNODE_T *childNode)
{
    int32 ret = ERR_NORMAL;

    if(NULL == childNode)
    {
        return ERR_PNULL;
    }

    /* 使用互斥量进行加锁 */
    pthread_mutex_lock(&childNodeList.mutex);

    *childNode = childNodeList.childNode[infoNum];
    childNodeList.isChanged[infoNum] = FALSE;

    /* 使用互斥量进行解锁 */
    pthread_mutex_unlock(&childNodeList.mutex);

    if (childNodeList.isVaild[infoNum])
    {
        ret = ERR_OK;
    }
    else
    {
        ret = ERR_INVALID;
    }

    return ret;
}

/**********************************************************************
* @name      : app_set_childnode_list
* @brief     ：将更新的节点信息数据写入缓存
* @param[in] ：infoNum   - 任务序号          
               childNode - 任务数据
* @return    ：错误码
**********************************************************************/
int app_set_childnode_list(uint16 infoNum, OOP_CHILDNODE_T *childNode)
{
    if(NULL == childNode)
    {
        return ERR_PNULL;
    }

    /* 使用互斥量进行加锁 */
    pthread_mutex_lock(&childNodeList.mutex);

    childNodeList.childNode[infoNum] = *childNode;
    childNodeList.isVaild[infoNum] = TRUE;
    childNodeList.isChanged[infoNum] = TRUE;
    childNodeList.childNodeNum ++;

    /* 使用互斥量进行解锁 */
    pthread_mutex_unlock(&childNodeList.mutex);

    return ERR_OK;
}

/**********************************************************************
* @name      : app_set_cnildnode_invalid
* @brief     ：设置数据无效
* @param[in] : infoNum - 序号
* @param[out]：
* @return    ：错误码
**********************************************************************/
int app_set_childnode_invalid(uint16 infoNum)
{

    /* 使用互斥量进行加锁 */
    pthread_mutex_lock(&childNodeList.mutex);

    memset(&childNodeList.childNode[infoNum], 0, sizeof(OOP_CHILDNODE_T));
    childNodeList.isVaild[infoNum] = FALSE;
    childNodeList.isChanged[infoNum] = TRUE;
    childNodeList.childNodeNum --;

    /* 使用互斥量进行解锁 */
    pthread_mutex_unlock(&childNodeList.mutex);

    return ERR_OK;
}

/**********************************************************************
* @name      : appdb_get_childnode_list
* @brief     ：搜表数据列表到缓存
* @param[in] ：int clientid    socket句柄
               uint8 logicId   逻辑地址
               uint16 infoNum  信息点号
               uint32 value    oad值
* @param[out]：
* @return    ：0-成功/错误码
**********************************************************************/
int appdb_get_childnode_list(int clientid, uint8 logicId, uint16 infoNum, uint32 value)
{
    int                   ret = 0;
    uint32                readlen = 0;
    uint32                i = 0;
    NOMAL_OAD_T           tOAD = {0};
    OOP_CHILDNODE_T childNode;

    if ((0xF2090500 != value) || (infoNum > NO_VIR_MTR_MAX_NUM))
    {
        return -1;
    }

    tOAD.logicId   = logicId;
    tOAD.infoNum   = infoNum;
    tOAD.oad.value = value;

    MEMZERO(&childNode, sizeof(childNode));

    PRTL_FMT_LOGCC("appdb_get_childnode_list logicId(%d) infoNum(%d) value(0x%x) running.....\n", logicId, infoNum, value);

    if (infoNum == 0xffff)
    {
        childNodeList.childNodeNum = 0;
        for (i = 1; i <= NO_VIR_MTR_MAX_NUM; i++)
        {
            //判断是否有效
            if (!childNodeList.isVaild[i])
            {
                continue;
            }

            if (app_set_childnode_invalid(i) < ERR_OK)
            {
                PRTL_FMT_LOGCC("app_set_childnode_invalid err\n");
            }
        }      
        return 0;
    }

    ret = db_read_nomal(clientid, &tOAD, sizeof(OOP_CHILDNODE_T), (uint8 *)&childNode, &readlen);
    if(0 != ret || readlen != sizeof(OOP_CHILDNODE_T))
    {
        app_set_childnode_invalid(infoNum);
        PRTL_FMT_DEBUG("db_read_nomal failed. ret(%d), infoNum(%d), readlen(%d), explen(%d)\n", ret, infoNum, readlen, sizeof(OOP_CHILDNODE_T));
        return ERR_NORMAL;
    }

    PRTL_FMT_DEBUG("childNode.index = %d\n", childNode.index);
    PRTL_BUF_DEBUG(&childNode.add.add[0], childNode.add.len, "childNode.addr:");
    PRTL_BUF_DEBUG(&childNode.id.value[0], childNode.id.nNum, "childNode.id:");
//    PRTL_FMT_DEBUG("childNode.wResponse = %d\n", childNode.wResponse);
//    PRTL_FMT_DEBUG("childNode.dtLastOk.year = %d\n", childNode.dtLastOk.year);
//    PRTL_FMT_DEBUG("childNode.dtLastOk.month = %d\n", childNode.dtLastOk.month);
//    PRTL_FMT_DEBUG("childNode.dtLastOk.day = %d\n", childNode.dtLastOk.day);
    
    ret = app_set_childnode_list(infoNum, &childNode);
    if (ret != ERR_OK)
    {
        app_set_childnode_invalid(infoNum);
        PRTL_FMT_LOGCC("app_set_childnode_list failed. ret(%d), infoNum(%d)\n", ret, infoNum);
        return ERR_NORMAL;
    }

    return 0;
}

/**********************************************************************
* @name      : app_get_childnode_count
* @brief     ：获取节点信息结果个数
* @param[in] ：
* @return    个数
**********************************************************************/
uint16 app_get_childnode_count(void)
{
    uint16 ret;

    /* 使用互斥量进行加锁 */
    pthread_mutex_lock(&childNodeList.mutex);
    ret = childNodeList.childNodeNum ;

    /* 使用互斥量进行解锁 */
    pthread_mutex_unlock(&childNodeList.mutex);

    return ret;
}

#endif

#if DESC("载波节点版本信息变更", 1)
/**********************************************************************
* @name      : nodeversion_list_lock_init
* @brief     ：
* @param[out]：
* @return    ：
* @Date      ：2020-9-9
* @Update    :
**********************************************************************/
void nodeversion_list_lock_init(void)
{
    /* 初始化锁 */
    memset(&nodeVersionList, 0, sizeof(OOP_NODEVERSIONINFO_LIST_T));
    pthread_mutex_init(&nodeVersionList.mutex, NULL);
}

/**
*********************************************************************
* @name：      appdb_nodeversion_list_record_init
* @brief：     节点信息变更初始化(记录数据方式)
* @param[in] ：clientid  数据库句柄
* @param[out]：
* @return    ：
* @author    : 
* @Date      ：2020-9-9
*********************************************************************
*/
void appdb_nodeversion_list_record_init(int clientid)
{
    int                   ret = ERR_NORMAL;
    uint32                i = 0;
    uint32                nodeVersionNum = 0;
    OOP_NODEVERSIONINFO_T nodeVersion;
    READ_RECORD_T         inReadRecord;
    uint32                handle = 0;
    uint32                recNum = 0;
    STEP_READ_RECORD_T    stepReadRecord;
    RECORD_UNIT_T         stepRecordOut;

    memset(&nodeVersionList, 0, sizeof(OOP_NODEVERSIONINFO_LIST_T));
    memset(&inReadRecord, 0, sizeof(READ_RECORD_T));
    memset(&stepReadRecord, 0, sizeof(STEP_READ_RECORD_T));
    memset(&stepRecordOut, 0, sizeof(RECORD_UNIT_T));
    memset(&nodeVersion, 0, sizeof(OOP_NODEVERSIONINFO_T));

    PRTL_FMT_DEBUG("Initializing nodeversion. please wait......\n");
    inReadRecord.recordOAD.logicId = 0;
    inReadRecord.recordOAD.infoNum = 0xffff;
    inReadRecord.cType = COL_NO_INFO;
    inReadRecord.cStart = 0;
    inReadRecord.cEnd = NO_VIR_MTR_MAX_NUM + 1;
    inReadRecord.sortType = UP_SORT;
    inReadRecord.recordOAD.optional = 0;
    inReadRecord.recordOAD.road.oadMain.value = 0;
    inReadRecord.recordOAD.road.oadCols.nNum = 0x01;
    inReadRecord.recordOAD.road.oadCols.oad[0].value = 0xF2090C00;

    //不需要附加校验
    stepReadRecord.MAC.nNum = 0;

    ret = db_read_record_start(clientid, &inReadRecord, &handle, &recNum);
    if ((ret != ERR_OK) || (recNum == 0))
    {
        PRTL_FMT_DEBUG("appdb_nodeversion_list_record_init db_read_record_start err! ret = %d, infoNum = %d recNum = %d\n", 
            ret, inReadRecord.recordOAD.infoNum, recNum);
    }
    else
    {
        PRTL_FMT_DEBUG("appdb_nodeversion_list_record_init db_read_record_start ok ret = %d, infoNum = %d recNum = %d\n", 
            ret, inReadRecord.recordOAD.infoNum, recNum);
        for (i = 0; i < recNum; i++)
        {
            ret = db_read_record_step(clientid, handle, &stepReadRecord, &stepRecordOut);
            if (ret != ERR_OK)
            {
                PRTL_FMT_DEBUG("appdb_nodeversion_list_record_init db_read_record_step err = %d!\n", ret);
                continue;
            }

            //检查数据有效性
            if ((stepRecordOut.recordnum != 1) 
            || (stepRecordOut.record[0].road.oadCols.oad[0].value != 0xF2090C00)
            || (stepRecordOut.record[0].outDatalen != sizeof(OOP_NODEVERSIONINFO_T)))
            {
                PRTL_FMT_DEBUG("appdb_nodeversion_list_record_init data err !\n");
                PRTL_FMT_DEBUG("appdb_nodeversion_list_record_init stepRecordOut.recordnum = %d\n", stepRecordOut.recordnum);
                PRTL_FMT_DEBUG("appdb_nodeversion_list_record_init stepRecordOut.record[0].road.oadCols.oad[0] = 0x%08x\n", 
                    stepRecordOut.record[0].road.oadCols.oad[0].value);
                PRTL_FMT_DEBUG("appdb_nodeversion_list_record_init stepRecordOut.record[0].outDatalen = %d\n", 
                    stepRecordOut.record[0].outDatalen);
                PRTL_FMT_DEBUG("appdb_nodeversion_list_record_init sizeof(OOP_NODEVERSIONINFO_T) = %d\n", 
                    sizeof(OOP_NODEVERSIONINFO_T));
                continue;
            }

            memcpy(&nodeVersion, stepRecordOut.record[0].outDataBuf, stepRecordOut.record[0].outDatalen);
            PRTL_BUF_DEBUG(&nodeVersion.addr.nValue[0], sizeof(nodeVersion.addr.nValue), "nodeVersion.addr:");
            PRTL_FMT_DEBUG("nodeVersion.type = %d\n", nodeVersion.type);
            PRTL_FMT_DEBUG("nodeVersion.wVer = %d\n", nodeVersion.wVer);
            PRTL_FMT_DEBUG("nodeVersion.dtVer.year = %d\n", nodeVersion.dtVer.year);
            PRTL_FMT_DEBUG("nodeVersion.dtVer.month = %d\n", nodeVersion.dtVer.month);
            PRTL_FMT_DEBUG("nodeVersion.dtVer.mday = %d\n", nodeVersion.dtVer.mday);
            PRTL_BUF_DEBUG(&nodeVersion.factory.value[0], nodeVersion.factory.nNum, "nodeVersion.factory:");
            PRTL_BUF_DEBUG(&nodeVersion.ic.value[0], nodeVersion.ic.nNum, "nodeVersion.addr:");

            nodeVersionNum++;

            //序号从1开始
            nodeVersionList.isChanged[stepRecordOut.infoNum] = TRUE;
            nodeVersionList.isVaild[stepRecordOut.infoNum] = TRUE;
            nodeVersionList.nodeVersion[stepRecordOut.infoNum] = nodeVersion;
            
        }
    }
    db_read_record_end(clientid, handle);
    
    PRTL_FMT_DEBUG("Initialize nodeversion end.\n");

    /* 记录任务数量 */
    nodeVersionList.nodeVersionNum = nodeVersionNum;

    PRTL_FMT_DEBUG("read nodeversion num: %d\n", nodeVersionList.nodeVersionNum);

    return;
}

/**********************************************************************
* @name      : app_get_nodeversion_list
* @brief     ：从缓存中获取载波节点信息
* @param[in] ：infoNum   - 序号          
* @param[out]: nodeversion - 数据
* @return    : 错误码
**********************************************************************/
int app_get_nodeversion_list(uint16 infoNum, OOP_NODEVERSIONINFO_T *nodeVersion)
{
    int32 ret = ERR_NORMAL;

    if(NULL == nodeVersion)
    {
        return ERR_PNULL;
    }

    /* 使用互斥量进行加锁 */
    pthread_mutex_lock(&nodeVersionList.mutex);

    *nodeVersion = nodeVersionList.nodeVersion[infoNum];
    nodeVersionList.isChanged[infoNum] = FALSE;

    /* 使用互斥量进行解锁 */
    pthread_mutex_unlock(&nodeVersionList.mutex);

    if (nodeVersionList.isVaild[infoNum])
    {
        ret = ERR_OK;
    }
    else
    {
        ret = ERR_INVALID;
    }

    return ret;
}

/**********************************************************************
* @name      : app_set_nodeversion_list
* @brief     ：将更新的节点信息数据写入缓存
* @param[in] ：infoNum   - 任务序号          
               nodeVersion - 任务数据
* @return    ：错误码
**********************************************************************/
int app_set_nodeversion_list(uint16 infoNum, OOP_NODEVERSIONINFO_T *nodeVersion)
{
    if(NULL == nodeVersion)
    {
        return ERR_PNULL;
    }

    /* 使用互斥量进行加锁 */
    pthread_mutex_lock(&nodeVersionList.mutex);

    nodeVersionList.nodeVersion[infoNum] = *nodeVersion;
    nodeVersionList.isVaild[infoNum] = TRUE;
    nodeVersionList.isChanged[infoNum] = TRUE;
    nodeVersionList.nodeVersionNum ++;

    /* 使用互斥量进行解锁 */
    pthread_mutex_unlock(&nodeVersionList.mutex);

    return ERR_OK;
}

/**********************************************************************
* @name      : app_set_nodeversion_invalid
* @brief     ：设置数据无效
* @param[in] : infoNum - 序号
* @param[out]：
* @return    ：错误码
**********************************************************************/
int app_set_nodeversion_invalid(uint16 infoNum)
{

    /* 使用互斥量进行加锁 */
    pthread_mutex_lock(&nodeVersionList.mutex);

    memset(&nodeVersionList.nodeVersion[infoNum], 0, sizeof(OOP_NODEVERSIONINFO_T));
    nodeVersionList.isVaild[infoNum] = FALSE;
    nodeVersionList.isChanged[infoNum] = TRUE;
    nodeVersionList.nodeVersionNum --;

    /* 使用互斥量进行解锁 */
    pthread_mutex_unlock(&nodeVersionList.mutex);

    return ERR_OK;
}

/**********************************************************************
* @name      : appdb_get_nodeversion_list
* @brief     ：搜表数据列表到缓存
* @param[in] ：int clientid    socket句柄
               uint8 logicId   逻辑地址
               uint16 infoNum  信息点号
               uint32 value    oad值
* @param[out]：
* @return    ：0-成功/错误码
**********************************************************************/
int appdb_get_nodeversion_list(int clientid, uint8 logicId, uint16 infoNum, uint32 value)
{
    int                   ret = 0;
    uint32                readlen = 0;
    uint32                i = 0;
    NOMAL_OAD_T           tOAD = {0};
    OOP_NODEVERSIONINFO_T nodeVersion;

    if (0xF2090C00 != value)
    {
        return -1;
    }

    tOAD.logicId   = logicId;
    tOAD.infoNum   = infoNum;
    tOAD.oad.value = value;

    MEMZERO(&nodeVersion, sizeof(nodeVersion));

    PRTL_FMT_LOGCC("appdb_get_nodeversion_list logicId(%d) infoNum(%d) value(0x%x) running.....\n", logicId, infoNum, value);

    if (infoNum == 0xffff)
    {
        nodeVersionList.nodeVersionNum = 0;
        for (i = 1; i <= NO_VIR_MTR_MAX_NUM; i++)
        {
            //判断是否有效
            if (!nodeVersionList.isVaild[i])
            {
                continue;
            }

            if (app_set_nodeversion_invalid(i) < ERR_OK)
            {
                PRTL_FMT_LOGCC("app_set_nodeversion_invalid err\n");
            }
        }      
        return 0;
    }

    ret = db_read_nomal(clientid, &tOAD, sizeof(OOP_NODEVERSIONINFO_T), (uint8 *)&nodeVersion, &readlen);
    if(0 != ret || readlen != sizeof(OOP_NODEVERSIONINFO_T))
    {
        app_set_nodeversion_invalid(infoNum);
        PRTL_FMT_DEBUG("db_read_nomal failed. ret(%d), infoNum(%d), readlen(%d), explen(%d)\n", ret, infoNum, readlen, sizeof(OOP_NODEVERSIONINFO_T));
        return ERR_NORMAL;
    }

    PRTL_BUF_DEBUG(&nodeVersion.addr.nValue[0], sizeof(nodeVersion.addr.nValue), "nodeVersion.addr:");
    PRTL_FMT_DEBUG("nodeVersion.type = %d\n", nodeVersion.type);
    PRTL_FMT_DEBUG("nodeVersion.wVer = %d\n", nodeVersion.wVer);
    PRTL_FMT_DEBUG("nodeVersion.dtVer.year = %d\n", nodeVersion.dtVer.year);
    PRTL_FMT_DEBUG("nodeVersion.dtVer.month = %d\n", nodeVersion.dtVer.month);
    PRTL_FMT_DEBUG("nodeVersion.dtVer.mday = %d\n", nodeVersion.dtVer.mday);
    PRTL_BUF_DEBUG(&nodeVersion.factory.value[0], nodeVersion.factory.nNum, "nodeVersion.factory:");
    PRTL_BUF_DEBUG(&nodeVersion.ic.value[0], nodeVersion.ic.nNum, "nodeVersion.addr:");
    
    ret = app_set_nodeversion_list(infoNum, &nodeVersion);
    if (ret != ERR_OK)
    {
        app_set_nodeversion_invalid(infoNum);
        PRTL_FMT_LOGCC("app_set_nodeversion_list failed. ret(%d), infoNum(%d)\n", ret, infoNum);
        return ERR_NORMAL;
    }

    return 0;
}

/**********************************************************************
* @name      : app_get_nodeversion_count
* @brief     ：获取节点版本信息结果个数
* @param[in] ：
* @return    个数
**********************************************************************/
uint16 app_get_nodeversion_count(void)
{
    uint16 ret;

    /* 使用互斥量进行加锁 */
    pthread_mutex_lock(&nodeVersionList.mutex);
    ret = nodeVersionList.nodeVersionNum ;

    /* 使用互斥量进行解锁 */
    pthread_mutex_unlock(&nodeVersionList.mutex);

    return ret;
}

#endif

#if DESC("根据配置文件读交采透抄是否开启", 1)
void load_safemod_init(void)
{
    dictionary         *ini = NULL;

    if (access("/data/app/desktopGui/safemode.ini", F_OK) != 0)
    {
        acRelay485Flag = 0;
        return;
    }

    ini = iniparser_load("/data/app/desktopGui/safemode.ini");
    PRTL_FMT_DEBUG("读取配置文件/data/app/desktopGui/safemode.ini\n");
    if (ini != NULL)
    {
        acRelay485Flag = iniparser_getint(ini, "SAFE_MODE:AC485FLAG", 0);
        PRTL_FMT_DEBUG("SAFE_MODE:AC485FLAG[%d]\n", acRelay485Flag);
        if ((acRelay485Flag < 0) || (acRelay485Flag > 1))
        {
            acRelay485Flag = 0;
            iniparser_freedict(ini);
            return;
        }

        iniparser_freedict(ini);
    }

    return;
}

/**********************************************************************
* @name      : ac_relay_flag_get
* @brief     ：获取交采透抄模式
* @param[in] ：
* @param[out]：
* @return    ：
**********************************************************************/
uint32 ac_relay_flag_get(void)
{
    return acRelay485Flag;
}
#endif

#if DESC("更新esam密钥下发状态", 1)
/**********************************************************************
* @name      : regapp_info_lock_init
* @brief     ：
* @param[out]：
* @return    ：
**********************************************************************/
void regapp_info_lock_init(void)
{
    /* 初始化锁 */
    memset(&oopRegAppInfoList, 0, sizeof(OOP_REG_APP_INFO_LIST_T));
    pthread_mutex_init(&oopRegAppInfoList.mutex, NULL);
}

/**********************************************************************
* @name      : regapp_callin_stat_get
* @brief     ：判断是否是主站在召测中
* @param[in] ：
* @param[out]：
* @return    ：
**********************************************************************/
uint8 regapp_callin_stat_get(void)
{
    uint8 ret = 0;
    /* 使用互斥量进行加锁 */
    pthread_mutex_lock(&oopRegAppInfoList.mutex);

    ret = oopRegAppInfoList.isCallIn;

    /* 使用互斥量进行解锁 */
    pthread_mutex_unlock(&oopRegAppInfoList.mutex);

    return ret;
}

/**********************************************************************
* @name      : regapp_callin_stat_set
* @brief     ：设置是否在召测APP信息
* @param[in] ：
* @param[out]：
* @return    ：
**********************************************************************/
void regapp_callin_stat_set(uint8 isCallin)
{
    /* 使用互斥量进行加锁 */
    pthread_mutex_lock(&oopRegAppInfoList.mutex);

    oopRegAppInfoList.isCallIn = isCallin;

    /* 使用互斥量进行解锁 */
    pthread_mutex_unlock(&oopRegAppInfoList.mutex);

    return;
}

#endif

#if DESC("远程升级的版本信息", 1)
/**********************************************************************
* @name      : app_update_version_set
* @brief     ：设置远程升级的版本
* @param[in] ：
* @param[out]：
* @return    ：
**********************************************************************/
void app_update_version_set(char *str)
{
    if ((str == NULL) || (strlen(str) > 4))
    {
        PRTL_FMT_DEBUG("param err str[%p] strlen(str):%d\n", str, strlen(str));
        return;
    }

    memcpy(update_version, str, sizeof(str) + 1);

    return;
}

/**********************************************************************
* @name      : app_update_version_get
* @brief     ：获取远程升级的版本
* @param[in] ：
* @param[out]：
* @return    ：错误码
**********************************************************************/
int app_update_version_get(char *ver)
{
    if (ver == NULL)
    {
        return -1;
    }

    if (strcmp(update_version, "0") == 0)
    {
        return -2;
    }

    memcpy(ver, update_version, strlen(update_version) + 1);
    return 0;
}

#endif

#if DESC("剩余电量相关接口" ,1)
/**********************************************************************
* @name      : remainn_en_lock_init
* @brief     ：
* @param[out]：
* @return    ：
**********************************************************************/
void remain_energy_lock_init(void)
{
    /* 初始化锁 */
    pthread_mutex_init(&calRemainEnLock, NULL);
}

/**********************************************************************
* @name      : remain_en_lock
* @brief     ：
* @param[out]：
* @return    ：
**********************************************************************/
void remain_energy_lock(void)
{
    pthread_mutex_lock(&calRemainEnLock);
}

/**********************************************************************
* @name      : remain_en_unlock
* @brief     ：
* @param[out]：
* @return    ：
**********************************************************************/
void remain_energy_unlock(void)
{
    pthread_mutex_unlock(&calRemainEnLock);
}

/**********************************************************************
* @name      : set_reset_flag
* @brief     ：写剩余电量
* @param[in] ：vir_no ---- 总加序号 从1开始
               energy ---- 剩余电量
* @return    ： 0  --  成功
               其他 -  错误码
**********************************************************************/
int remain_energy_write(int clientid, uint16 vir_no, int64 energy)
{
    NOMAL_OAD_T normal = {0};
    OOP_OAD_U oad;
    int32 ret = ERR_OK;

    if ((vir_no < 1) || (vir_no > 8))
    {
        return ERR_OPTION;
    }

    oad.value = 0x23010B00;
    oad.nObjID = 0x2300 + vir_no;
    //写入数据
    normal.oad.value = oad.value;
    normal.classtag = CLASS_DATA_INIT;
    
    ret = db_write_nomal(clientid, &normal, (uint8*)&energy, sizeof(energy));
    if(ret != ERR_OK)
    {
        PRTL_FMT_DEBUG("db_write_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
            ret, normal.logicId, normal.infoNum, normal.oad.value);
        return ERR_IO;
    }

    return ret;
}

/**********************************************************************
* @name      : remain_en_read
* @brief     ：读剩余电量
* @param[in] ：vir_no ---- 总加序号 从1开始
* @return    ： 0  --  成功
               其他 -  错误码
**********************************************************************/
int remain_energy_read(int clientid, uint16 vir_no, int64* energy)
{
    NOMAL_OAD_T normal = {0};
    OOP_OAD_U oad;
    int32 ret = ERR_OK;
    int64 energyTmp = 0;
    uint32 len = 0;

    if ((vir_no < 1) || (vir_no > 8) || (energy == NULL))
    {
        return ERR_OPTION;
    }

    oad.value = 0x23010B00;
    oad.nObjID = 0x2300 + vir_no;

    //读取数据
    normal.oad.value = oad.value;
    ret = db_read_nomal(clientid, &normal, sizeof(energyTmp), (uint8*)&energyTmp, &len);
    if((ret != ERR_OK) || (len != sizeof(energyTmp)))
    {
        PRTL_FMT_DEBUG("db_read_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x) len(%d)\n",
            ret, normal.logicId, normal.infoNum, normal.oad.value, len);
        return DATA_OTHER_REASON;
    }

    *energy = energyTmp;

    return ret;
}

#endif

#if DESC("复位标记" ,1)

extern void appmain_buzzer_status(int dbClient, uint8 status);
/**********************************************************************
* @name      : set_reset_flag
* @brief     ：设置复位标记
* @param[in] ：flag ---- 复位标记
* @return    ：NULL
**********************************************************************/
void set_buzzer_out(int dbClient, BOOLEAN bOpen)
{
    appmain_buzzer_status(dbClient, bOpen);
}

/**********************************************************************
* @name      : set_reset_flag
* @brief     ：设置复位标记
* @param[in] ：flag ---- 复位标记
* @return    ：NULL
**********************************************************************/
void set_reset_flag(int dbClient, uint32 flag)
{
    resetFlag = flag;

    if(TMN_REBOOTING == flag)
    {
        /* 终端重启 蜂鸣器响一下 */
        set_buzzer_out(dbClient, TRUE);
        usleep(500000);
        set_buzzer_out(dbClient, FALSE);
    }
    return;
}

/**********************************************************************
* @name      : set_reset_flag
* @brief     ：获取复位标记
* @param[in] ：NULL
* @return    ：flag ---- 复位标记
**********************************************************************/
uint32 get_reset_flag(void)
{
    return resetFlag;
}

#endif

#if DESC("系统管理器复位事件" , 1)

/**********************************************************************
* @name      : set_smiOS_reboot_flag
* @brief     ：设置复位标记
* @param[in] ：flag ---- 复位标记
* @return    ：NULL
**********************************************************************/
void set_smiOS_reboot_flag(SIMOS_CTRL_E flag)
{
    resetSmiOSFlag = flag;
    
    return;
}

/**********************************************************************
* @name      : set_reset_flag
* @brief     ：获取复位标记
* @param[in] ：NULL
* @return    ：flag ---- 复位标记
**********************************************************************/
SIMOS_CTRL_E get_smiOS_reboot_flag(void)
{
    return resetSmiOSFlag;
}

#endif

#if DESC("保电状态变更" ,1)
/**********************************************************************
* @name      : ipaddr_set_lock_init
* @brief     ：
* @param[out]：
* @return    ：
**********************************************************************/
void guard_set_lock_init(void)
{
    /* 初始化锁 */
    memset(&oopGuard, 0, sizeof(oopGuard));
    pthread_mutex_init(&oopGuard.mutex, NULL);
}

/**********************************************************************
* @name      : get_guard_state
* @brief     ：获取保电状态
* @param[in] ：NULL
* @param[out]：
* @return    ：0 - 初始化状态
               1 - 保电
               2 - 解除 
**********************************************************************/
uint8 get_guard_state(void)
{
    uint8 guardState = 0;

    /* 使用互斥量进行加锁 */
    pthread_mutex_lock(&oopGuard.mutex);

    guardState = oopGuard.guardState;

    /* 使用互斥量进行解锁 */
    pthread_mutex_unlock(&oopGuard.mutex);

    return guardState;
}

/**********************************************************************
* @name      : set_guard_state
* @brief     ：设置保电状态
* @param[in] ：guardState - 保电状态 0 - 无状态 1- 投入 2 - 解除
* @param[out]：NULL
* @return    ：0          - 成功
              !0          - 错误 
**********************************************************************/
int set_guard_state(uint8 guardState)
{
    if (guardState > 2)
    {
        return ERR_O_RANGE;
    }

    /* 使用互斥量进行加锁 */
    pthread_mutex_lock(&oopGuard.mutex);

    oopGuard.guardState = guardState;

    /* 使用互斥量进行解锁 */
    pthread_mutex_unlock(&oopGuard.mutex);

    return ERR_OK;
}

#endif

#if DESC("遥控状态变更" ,1)
/**********************************************************************
* @name      : ipaddr_set_lock_init
* @brief     ：
* @param[out]：
* @return    ：
**********************************************************************/
void remote_set_lock_init(void)
{
    /* 初始化锁 */
    memset(&oopRemoteCtrl, 0, sizeof(oopRemoteCtrl));
    pthread_mutex_init(&oopRemoteCtrl.mutex, NULL);
}

/**********************************************************************
* @name      : get_remote_state
* @brief     ：获取遥控状态
* @param[in] ：turn  - 轮次
* @param[out]：type  -  遥控种类 1-跳闸, 2-允许合闸, 3-直接合闸
* @return    ：state - 遥控状态 0-无效，1-有效
**********************************************************************/
uint8 get_remote_state(uint8 turn, uint8 *type)
{
    uint8 rmState = 0;

    if (type == NULL)
    {
        PRTL_FMT_DEBUG("get_remote_state err type[NULL]\n");
        return 0;
    }

    if ((turn > OOP_MAX_TRIP) || (turn < 1))
    {
        return 0;
    }

    /* 使用互斥量进行加锁 */
    pthread_mutex_lock(&oopRemoteCtrl.mutex);

    rmState = oopRemoteCtrl.state[turn - 1];
    *type = oopRemoteCtrl.type[turn - 1];

    /* 使用互斥量进行解锁 */
    pthread_mutex_unlock(&oopRemoteCtrl.mutex);

    return rmState;
}

/**********************************************************************
* @name      : set_remote_state_invalid
* @brief     ：清除遥控状态
* @param[in] ：turn   - 轮次
* @param[out]：NULL
* @return    ：
**********************************************************************/
void set_remote_state_invalid(uint8 turn)
{
    if ((turn > OOP_MAX_TRIP) || (turn < 1))
    {
        PRTL_FMT_DEBUG("set_remote_state_invalid err turn[%d]\n", turn);
        return;
    }

    /* 使用互斥量进行加锁 */
    pthread_mutex_lock(&oopRemoteCtrl.mutex);

    oopRemoteCtrl.state[turn - 1] = 0;

    /* 使用互斥量进行解锁 */
    pthread_mutex_unlock(&oopRemoteCtrl.mutex);

    return;
}

/**********************************************************************
* @name      : set_remote_state_valid
* @brief     ：设置遥控状态
* @param[in] ：turn   - 轮次
* @param[out]：NULL
* @return    ：
**********************************************************************/
void set_remote_state_valid(uint8 turn, uint8 type)
{
    if ((turn > OOP_MAX_TRIP) || (turn < 1))
    {
        PRTL_FMT_DEBUG("set_remote_state_valid err turn[%d]\n", turn);
        return;
    }

    /* 使用互斥量进行加锁 */
    pthread_mutex_lock(&oopRemoteCtrl.mutex);

    oopRemoteCtrl.state[turn - 1] = 1;
    oopRemoteCtrl.type[turn - 1] = type;

    /* 使用互斥量进行解锁 */
    pthread_mutex_unlock(&oopRemoteCtrl.mutex);

    return;
}

#endif

#if DESC("档案参数变更" ,1)

/**********************************************************************
* @name      : meter_list_lock_init
* @brief     ：
* @param[out]：
* @return    ：
* @Create    : 贺宁
* @Date      ：2020-1-10
* @Update    :
**********************************************************************/
void meter_list_lock_init(void)
{
    /* 初始化锁 */
    memset(&oopMeterList, 0, sizeof(OOP_METER_P_LIST_T));
    memset(oopMeterList.meterID, 0xff, sizeof(oopMeterList.meterID));
    pthread_mutex_init(&oopMeterList.mutex, NULL);
}

/**
*********************************************************************
* @name：      appdb_meter_list_init
* @brief：     档案参数变更初始化
* @param[in] ：clientid  数据库句柄
* @param[out]：
* @return    ：
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
void appdb_meter_list_init(int clientid)
{
    int             ret = ERR_NORMAL;
    uint32          i = 0;
    uint32          oopMeterNum = 0;
    NOMAL_OAD_T     tOAD = {0};
    OOP_METER_T     oopMeter = {0};
    uint32          readlen = 0;

    memset(&oopMeterList, 0, sizeof(OOP_METER_P_LIST_T));
    memset(oopMeterList.meterID, 0xff, sizeof(oopMeterList.meterID));

    tOAD.logicId   = 0;
    tOAD.oad.value = 0x60000200;

    printf("Initializing meters. please wait......\n");
    for (i = 1; i <= NO_VIR_MTR_MAX_NUM; i++)
    {
        tOAD.infoNum   = i;
        ret = db_read_nomal(clientid, &tOAD, sizeof(OOP_METER_T), (uint8 *)&oopMeter, &readlen);
        if(0 == ret && readlen == sizeof(OOP_METER_T))
        {
            oopMeterList.meterID[i] = oopMeterNum;

            //档案序号从1开始
            oopMeterList.oopMeter[oopMeterNum] = oopMeter;
            oopMeterNum++;
        }
    }
    printf("Initialize meters end.\n");

    /* 记录档案数量 */
    oopMeterList.oopMeterNum = oopMeterNum;

    PRTL_FMT_LOGCC("read meter num: %d\n", oopMeterList.oopMeterNum);

    return;
}

/**
*********************************************************************
* @name：      appdb_meter_list_init
* @brief：     档案参数变更初始化(记录数据方式)
* @param[in] ：clientid  数据库句柄
* @param[out]：
* @return    ：
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
void appdb_meter_list_record_init(int clientid)
{
    int                ret = ERR_NORMAL;
    uint32             i = 0;
    uint32             oopMeterNum = 0;
    OOP_METER_T        oopMeter = {0};
    READ_RECORD_T      inReadRecord;
    uint32             handle = 0;
    uint32             recNum = 0;
    STEP_READ_RECORD_T stepReadRecord;
    RECORD_UNIT_T      stepRecordOut;

    memset(&oopMeterList, 0, sizeof(OOP_METER_P_LIST_T));
    memset(oopMeterList.meterID, 0xff, sizeof(oopMeterList.meterID));    
    memset(&inReadRecord, 0, sizeof(READ_RECORD_T));
    memset(&stepReadRecord, 0, sizeof(STEP_READ_RECORD_T));
    memset(&stepRecordOut, 0, sizeof(RECORD_UNIT_T));

    PRTL_FMT_DEBUG("Initializing meters. please wait......\n");
    inReadRecord.recordOAD.logicId = 0;
    inReadRecord.recordOAD.infoNum = 0xffff;
    inReadRecord.cType = COL_NO_INFO;
    inReadRecord.cStart = 1;
    inReadRecord.cEnd = NO_VIR_MTR_MAX_NUM + 1;
    inReadRecord.sortType = UP_SORT;
    inReadRecord.recordOAD.optional = 0;
    inReadRecord.recordOAD.road.oadMain.value = 0;
    inReadRecord.recordOAD.road.oadCols.nNum = 0x01;
    inReadRecord.recordOAD.road.oadCols.oad[0].value = 0x60000200;

    //不需要附加校验
    stepReadRecord.MAC.nNum = 0;

    ret = db_read_record_start(clientid, &inReadRecord, &handle, &recNum);
    if ((ret != ERR_OK) || (recNum == 0))
    {
        PRTL_FMT_DEBUG("appdb_meter_list_record_init db_read_record_start err! ret = %d, infoNum = %d recNum = %d\n", 
            ret, inReadRecord.recordOAD.infoNum, recNum);
    }
    else
    {
        PRTL_FMT_DEBUG("appdb_meter_list_record_init db_read_record_start ok ret = %d, infoNum = %d recNum = %d\n", 
            ret, inReadRecord.recordOAD.infoNum, recNum);
        for (i = 0; i < recNum; i++)
        {
            ret = db_read_record_step(clientid, handle, &stepReadRecord, &stepRecordOut);
            if (ret != ERR_OK)
            {
                PRTL_FMT_DEBUG("appdb_meter_list_record_init db_read_record_step err = %d!\n", ret);
                continue;
            }

            //检查数据有效性
            if ((stepRecordOut.recordnum != 1) 
            || (stepRecordOut.record[0].road.oadCols.oad[0].value != 0x60000200)
            || (stepRecordOut.record[0].outDatalen != sizeof(OOP_METER_T)))
            {
                PRTL_FMT_DEBUG("appdb_meter_list_record_init data err !\n");
                PRTL_FMT_DEBUG("appdb_meter_list_record_init stepRecordOut.recordnum = %d\n", stepRecordOut.recordnum);
                PRTL_FMT_DEBUG("appdb_meter_list_record_init stepRecordOut.record[0].road.oadCols.oad[0] = 0x%08x\n", 
                    stepRecordOut.record[0].road.oadCols.oad[0].value);
                PRTL_FMT_DEBUG("appdb_meter_list_record_init stepRecordOut.record[0].outDatalen = %d\n", 
                    stepRecordOut.record[0].outDatalen);
                PRTL_FMT_DEBUG("appdb_meter_list_record_init sizeof(OOP_METER_T) = %d\n", 
                    sizeof(OOP_METER_T));
                continue;
            }

            memcpy(&oopMeter, stepRecordOut.record[0].outDataBuf, stepRecordOut.record[0].outDatalen);
            PRTL_FMT_DEBUG("stepRecordOut.infoNum = %d\n", stepRecordOut.infoNum);
            PRTL_FMT_DEBUG("oopMeter.nIndex = %d\n", oopMeter.nIndex);
            PRTL_FMT_DEBUG("oopMeter.basic.protocol = %d\n", oopMeter.basic.protocol);
            PRTL_BUF_LOGCC(oopMeter.basic.tsa.add, oopMeter.basic.tsa.len + 1, "oopMeter.addr:");

            oopMeterList.meterID[stepRecordOut.infoNum] = oopMeterNum;

            //档案序号从1开始
            oopMeterList.oopMeter[oopMeterNum] = oopMeter;
            oopMeterNum++;
            
        }
    }
    db_read_record_end(clientid, handle);
    
    PRTL_FMT_DEBUG("Initialize meters end.\n");

    /* 记录档案数量 */
    oopMeterList.oopMeterNum = oopMeterNum;

    PRTL_FMT_DEBUG("read meter num: %d\n", oopMeterList.oopMeterNum);

    return;
}

/**********************************************************************
* @name      : app_set_meter_list
* @brief     ：设置档案数据
* @param[in] ：oopMeter - 档案
               i        - 档案序号
* @param[out]：
* @return    ：错误码
**********************************************************************/
int app_set_meter_list(uint32 infoNum, OOP_METER_T *oopMeter)
{
    uint16 oopMeterNum = oopMeterList.oopMeterNum;
    uint16 meterID = 0;

    if ((infoNum < 1) || (infoNum > NO_VIR_MTR_MAX_NUM))
    {
        return ERR_OPTION;
    }
        
    if(NULL == oopMeter)
    {
        return ERR_PNULL;
    }

    meterID = oopMeterList.meterID[infoNum];

    /* 使用互斥量进行加锁 */
    pthread_mutex_lock(&oopMeterList.mutex);

    if(meterID == 0xffff)
    {
        oopMeterList.oopMeter[oopMeterNum] = *oopMeter;
        oopMeterList.meterID[infoNum] = oopMeterNum;

        oopMeterList.oopMeterNum++; 
    }
    else if(meterID < oopMeterNum)
    {
        oopMeterList.oopMeter[meterID] = *oopMeter;
    }
    
    /* 使用互斥量进行解锁 */
    pthread_mutex_unlock(&oopMeterList.mutex);

    return ERR_OK;
}

/**********************************************************************
* @name      : app_get_meter_list
* @brief     ：通过数组ID获取档案数据
* @param[in] ：meterID        - 档案内部编号
* @param[out]：oopMeter - 档案
* @return    : 错误码
* @Create    : 贺宁
* @Date      ：2019-12-16
* @Update    :
**********************************************************************/
int app_get_meter_list_by_id(uint32 meterID, OOP_METER_T *oopMeter)
{
    uint16 oopMeterNum = oopMeterList.oopMeterNum;

    if((NULL == oopMeter)||(meterID == 0xffff) || (meterID >= oopMeterNum))
    {
        return ERR_PNULL;
    }

    /* 使用互斥量进行加锁 */
    pthread_mutex_lock(&oopMeterList.mutex);

    *oopMeter = oopMeterList.oopMeter[meterID];

    /* 使用互斥量进行解锁 */
    pthread_mutex_unlock(&oopMeterList.mutex);

    return ERR_OK;    
}
int app_get_meter_list_by_tsa(OOP_TSA_T		tsa, OOP_METER_T *oopMeter)
{
    uint16 oopMeterNum = oopMeterList.oopMeterNum,i;

    /* 使用互斥量进行加锁 */
    pthread_mutex_lock(&oopMeterList.mutex);

    for(i=0;i<oopMeterNum;i++)
    {

        if(TRUE==oop_tsa_equal(&tsa,&oopMeterList.oopMeter[i].basic.tsa))
        {
            pthread_mutex_unlock(&oopMeterList.mutex);
            *oopMeter = oopMeterList.oopMeter[i];
            return ERR_OK;
        }

    }
    /* 使用互斥量进行解锁 */
    pthread_mutex_unlock(&oopMeterList.mutex);
    return ERR_NOTEXIST;
}
/**********************************************************************
* @name      : app_get_meter_pn_list_by_tsa
* @brief     ：通过数组ID获取档案数据
* @param[in] ：meterID        - 档案内部编号
* @param[out]：oopMeter - 档案
* @return    : 错误码
* @Create    : 贺宁
* @Date      ：2019-12-16
* @Update    :
**********************************************************************/
uint16 app_get_meter_pn_list_by_tsa(OOP_TSA_T		tsa)
{
    uint16 oopMeterNum = oopMeterList.oopMeterNum,i;

    /* 使用互斥量进行加锁 */
    pthread_mutex_lock(&oopMeterList.mutex);

    for(i=0;i<oopMeterNum;i++)
    {

        if(TRUE==oop_tsa_equal(&tsa,&oopMeterList.oopMeter[i].basic.tsa))
        {
            pthread_mutex_unlock(&oopMeterList.mutex);
            return oopMeterList.oopMeter[i].nIndex;
        }

    }
    /* 使用互斥量进行解锁 */
    pthread_mutex_unlock(&oopMeterList.mutex);

    return 0xFFFF;    
}


/**********************************************************************
* @name      : app_get_meter_pn_list_by_tsa
* @brief     ：通过数组ID获取档案数据
* @param[in] ：meterID        - 档案内部编号
* @param[out]：oopMeter - 档案
* @return    : 错误码
* @Create    : 贺宁
* @Date      ：2019-12-16
* @Update    :
**********************************************************************/
bool app_get_meter_tsa_list_by_pn(uint16		pn,OOP_TSA_T *tsa)
{
    uint16 oopMeterNum = oopMeterList.oopMeterNum,i;

    /* 使用互斥量进行加锁 */
    pthread_mutex_lock(&oopMeterList.mutex);

    for(i=0;i<oopMeterNum;i++)
    {

        if(pn==oopMeterList.oopMeter[i].nIndex)
        {
            *tsa = oopMeterList.oopMeter[i].basic.tsa;
            pthread_mutex_unlock(&oopMeterList.mutex);
            return TRUE;
        }

    }
    /* 使用互斥量进行解锁 */
    pthread_mutex_unlock(&oopMeterList.mutex);

    return FALSE;    
}

/**********************************************************************
* @name      : app_get_meter_port_list_by_tsa
* @brief     ：通过数组ID获取档案数据
* @param[in] ：meterID        - 档案内部编号
* @param[out]：oopMeter - 档案
* @return    : 错误码
* @Create    : 贺宁
* @Date      ：2019-12-16
* @Update    :
**********************************************************************/
uint32 app_get_meter_port_list_by_tsa(OOP_TSA_T		tsa)
{
    uint16 oopMeterNum = oopMeterList.oopMeterNum,i;

    /* 使用互斥量进行加锁 */
    pthread_mutex_lock(&oopMeterList.mutex);

    for(i=0;i<oopMeterNum;i++)
    {

        if(TRUE==oop_tsa_equal(&tsa,&oopMeterList.oopMeter[i].basic.tsa))
        {
            pthread_mutex_unlock(&oopMeterList.mutex);
            return oopMeterList.oopMeter[i].basic.port.value;
        }

    }
    /* 使用互斥量进行解锁 */
    pthread_mutex_unlock(&oopMeterList.mutex);

    return 0xFFFFFFFF;    
}

/**********************************************************************
* @name      : app_get_meter_port_list_by_tsa
* @brief     ：通过数组ID获取档案数据
* @param[in] ：meterID        - 档案内部编号
* @param[out]：oopMeter - 档案
* @return    : 错误码
* @Create    : 贺宁
* @Date      ：2019-12-16
* @Update    :
**********************************************************************/
bool  app_get_meter_basic_info_by_tsa(OOP_TSA_T		tsa,OOP_METER_BASIC_T* basic)
{
    uint16 oopMeterNum = oopMeterList.oopMeterNum,i;

    /* 使用互斥量进行加锁 */
    pthread_mutex_lock(&oopMeterList.mutex);

    for(i=0;i<oopMeterNum;i++)
    {

        if(TRUE==oop_tsa_equal(&tsa,&oopMeterList.oopMeter[i].basic.tsa))
        {
            *basic=oopMeterList.oopMeter[i].basic;
            pthread_mutex_unlock(&oopMeterList.mutex);
            return TRUE;
        }

    }
    /* 使用互斥量进行解锁 */
    pthread_mutex_unlock(&oopMeterList.mutex);
    return FALSE;
    
}

int  GetVirtualPipePn(uint16 pn,OOP_METER_T *oopMeter)
{
    uint16 i ,j;

    for(i=0;i<perceptionterminal.num;i++)
    {    
        for(j=0;j<perceptionterminal.terminalinfo[i].meterlist.num;j++)
        {
            if(pn==perceptionterminal.terminalinfo[i].meterlist.meterinfo[j].pn)
            {
               memcpy(oopMeter->basic.tsa.add,perceptionterminal.terminalinfo[i].meterlist.meterinfo[j].pipeaddr,6);
                return 0;
            }
        }
    }
    if(i>=perceptionterminal.num)
        return -1;
    return 0;
}


/**********************************************************************
* @name      : app_get_meter_list
* @brief     ：获取档案数据
* @param[in] ：i        - 档案序号
* @param[out]：oopMeter - 档案
* @return    : 错误码
* @Create    : 
* @Date      ：2019-12-16
* @Update    :
**********************************************************************/
int app_get_meter_list(uint32 infoNum, OOP_METER_T *oopMeter)
{
    uint16 meterID = 0;

    if ((infoNum < 1) || (infoNum > NO_VIR_MTR_MAX_NUM))
    {
        return ERR_OPTION;
    }

    meterID = oopMeterList.meterID[infoNum];

    return app_get_meter_list_by_id(meterID, oopMeter);
}

/**********************************************************************
* @name      : app_set_meter_list
* @brief     ：设置档案数据无效
* @param[in] ：oopMeter - 档案
               i        - 档案序号
* @param[out]：
* @return    ：错误码
**********************************************************************/
int app_set_meter_invalid(uint32 infoNum)
{
    int i = 0;
    uint16 oopMeterNum = oopMeterList.oopMeterNum;
    uint16 meterID = 0;
    
    if ((infoNum < 1) || (infoNum > NO_VIR_MTR_MAX_NUM))
    {
        return ERR_OPTION;
    }

    meterID = oopMeterList.meterID[infoNum];

    if(meterID == 0xffff)
    {
        return ERR_OK;
    }

    if(meterID >= oopMeterNum)
    {
        return ERR_OPTION;
    }
    
    /* 使用互斥量进行加锁 */
    pthread_mutex_lock(&oopMeterList.mutex);

    memmove(&oopMeterList.oopMeter[meterID], &oopMeterList.oopMeter[meterID+1], (oopMeterNum-meterID-1)*sizeof(OOP_METER_T));

    oopMeterList.meterID[infoNum] = 0xffff;
    
    oopMeterList.oopMeterNum--;

    //刷新meterID的映射关系
    for(i = 0; i < oopMeterList.oopMeterNum; i++)
    {
        oopMeterList.meterID[oopMeterList.oopMeter[i].nIndex] = i;
    }

    /* 使用互斥量进行解锁 */
    pthread_mutex_unlock(&oopMeterList.mutex);

    return ERR_OK;
}

/**********************************************************************
* @name      : app_set_meter_updating
* @brief     ：设置档案数据正在更新标志
* @param[in] ：i  - 档案序号
* @param[out]：
* @return    ：错误码
**********************************************************************/
int app_set_meter_update_start(uint32 infoNum)
{
    if ((infoNum < 1) || (infoNum > NO_VIR_MTR_MAX_NUM))
    {
        return ERR_OPTION;
    }
    
    /* 使用互斥量进行加锁 */
    pthread_mutex_lock(&oopMeterList.mutex);

    oopMeterList.isUpdate[infoNum] = 1;

    /* 使用互斥量进行解锁 */
    pthread_mutex_unlock(&oopMeterList.mutex);

    return ERR_OK;
}

/**********************************************************************
* @name      : app_set_meter_updating
* @brief     ：设置档案数据正在更新标志
* @param[in] ：i  - 档案序号
* @param[out]：
* @return    ：错误码
**********************************************************************/
int app_set_meter_update_ok(uint32 infoNum)
{
    if ((infoNum < 1) || (infoNum > NO_VIR_MTR_MAX_NUM))
    {
        return ERR_OPTION;
    }
    
    /* 使用互斥量进行加锁 */
    pthread_mutex_lock(&oopMeterList.mutex);

    oopMeterList.isUpdate[infoNum] = 0;

    /* 使用互斥量进行解锁 */
    pthread_mutex_unlock(&oopMeterList.mutex);

    return ERR_OK;
}

/**********************************************************************
* @name      : is_meter_invalid
* @brief     ：判断档案数据是否无效
* @param[in] ：infoNum - 信息点号
* @param[out]：
* @return    ：0 无效
**********************************************************************/
int is_meter_invalid(uint32 infoNum)
{
    uint16 oopMeterNum = oopMeterList.oopMeterNum;
    uint16 meterID = 0;
    
    if ((infoNum < 1) || (infoNum > NO_VIR_MTR_MAX_NUM))
    {
        PRTL_FMT_DEBUG("app_get_meter_invalid err. infoNum[%d]\n", infoNum);
        return ERR_OPTION;
    }

    meterID = oopMeterList.meterID[infoNum];

    if (meterID == 0xffff)
    {
        return ERR_OK;
    }

    if (meterID >= oopMeterNum)
    {
        PRTL_FMT_DEBUG("app_get_meter_invalid err. meterID[%d]. oopMeterNum[%d]\n", meterID, oopMeterNum);
        return ERR_OPTION;
    }

    return ERR_NORMAL;
}

/**********************************************************************
* @name      : is_meter_update_ok
* @brief     ：判断档案数据是否更新结束
* @param[in] ：infoNum - 信息点号
* @param[out]：
* @return    ：0 无效
**********************************************************************/
int is_meter_update_ok(uint32 infoNum)
{
    BOOL flag = 0;
    
    if ((infoNum < 1) || (infoNum > NO_VIR_MTR_MAX_NUM))
    {
        PRTL_FMT_DEBUG("app_get_meter_invalid err. infoNum[%d]\n", infoNum);
        return ERR_OPTION;
    }

    flag = oopMeterList.isUpdate[infoNum];

    if (flag != 0)
    {
        return ERR_NORMAL;
    }

    return ERR_OK;
}

/**********************************************************************
* @name      : appdb_get_meter_list
* @brief     ：获取档案列表到缓存
* @param[in] ：int clientid    socket句柄
               uint8 logicId   逻辑地址
               uint16 infoNum  信息点号
               uint32 value    oad值
* @param[out]：
* @return    ：0-成功/错误码
**********************************************************************/
int appdb_get_meter_list(int clientid, uint8 logicId, uint16 infoNum, uint32 value)
{
    int                ret = 0;
    uint32             readlen = 0;
    NOMAL_OAD_T        tOAD = {0};
    OOP_METER_T        oopMeter = {0};

    if(0x60000200 != value)
    {
        return -1;
    }

    tOAD.logicId   = logicId;
    tOAD.infoNum   = infoNum;
    tOAD.oad.value = value;

    PRTL_FMT_LOGCC("appdb_get_meter_list logicId(%d) infoNum(%d) value(0x%x) running.....\n", logicId, infoNum, value);

    if (infoNum == 0xffff)
    {
        memset(oopMeterList.meterID, 0xff, sizeof(oopMeterList.meterID));
        oopMeterList.oopMeterNum = 0;
        return 0;
    }

    ret = db_read_nomal(clientid, &tOAD, sizeof(OOP_METER_T), (uint8 *)&oopMeter, &readlen);
    if(0 != ret || readlen != sizeof(OOP_METER_T))
    {
        if (app_set_meter_invalid(infoNum) < ERR_OK)
        {
            PRTL_FMT_DEBUG("app_set_meter_invalid err\n");
        }
        PRTL_FMT_DEBUG("db_read_nomal err = %d\n", ret);
        return ERR_NORMAL;
    }
    
    ret = app_set_meter_list(infoNum, &oopMeter);
    if (ret != ERR_OK)
    {
        if (app_set_meter_invalid(infoNum) < ERR_OK)
        {
            PRTL_FMT_DEBUG("app_set_meter_invalid err\n");
        }
        PRTL_FMT_DEBUG("app_set_meter_list err[%d]\n", ret);
        return ERR_NORMAL;
    }

    app_set_meter_update_ok(infoNum);

    ret = app_module_meter_update();
    if (ret != ERR_OK)
    {
        PRTL_FMT_DEBUG("扩展控制模块档案更新错误 err[%d]\n", ret);
    }
    
    return 0;
}

#endif

#if DESC("任务参数变更" ,1)

/**********************************************************************
* @name      : task_list_lock_init
* @brief     ：
* @param[out]：
* @return    ：
* @Create    : 贺宁
* @Date      ：2020-1-10
* @Update    :
**********************************************************************/
void task_list_lock_init(void)
{
    /* 初始化锁 */
     memset(&oopTaskList, 0, sizeof(OOP_TASK_LIST_T));
    pthread_mutex_init(&oopTaskList.mutex, NULL);
}

/**
*********************************************************************
* @name：      appdb_task_list_init
* @brief：     档案参数变更初始化
* @param[in] ：clientid  数据库句柄
* @param[out]：
* @return    ：
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
void appdb_task_list_init(int clientid)
{
    int             ret = ERR_NORMAL;
    uint16          taskID = 0;
    uint32          oopTaskNum = 0;
    NOMAL_OAD_T     tOAD = {0};
    OOP_TASK_T      oopTask = {0};
    uint32          readlen = 0;

    memset(&oopTaskList, 0, sizeof(OOP_TASK_LIST_T));

    tOAD.logicId   = 0;
    tOAD.oad.value = 0x60120200;

    printf("Initializing tasks. please wait......\n");
    for (taskID = 1; taskID <= 255; taskID++)
    {
        tOAD.infoNum = taskID;
        ret = db_read_nomal(clientid, &tOAD, sizeof(OOP_TASK_T), (uint8 *)&oopTask, &readlen);
        if(0 != ret || readlen != sizeof(OOP_TASK_T))
        {
            oopTaskList.isVaild[taskID] = FALSE;
            continue;
        }
        else
        {
            oopTaskNum++;

            //任务序号从1开始
            oopTaskList.isChanged[taskID] = TRUE;
            oopTaskList.isVaild[taskID] = TRUE;
            oopTaskList.oopTask[taskID] = oopTask;
        }
    }
    
    printf("Initialize tasks end.\n");

    /* 记录任务数量 */
    oopTaskList.oopTaskNum = oopTaskNum;

    PRTL_FMT_LOGCC("read task num: %d\n", oopTaskList.oopTaskNum);

    return;
}

/**
*********************************************************************
* @name：      appdb_task_list_record_init
* @brief：     任务参数变更初始化(记录数据方式)
* @param[in] ：clientid  数据库句柄
* @param[out]：
* @return    ：
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
void appdb_task_list_record_init(int clientid)
{
    int                ret = ERR_NORMAL;
    uint32             i = 0;
    uint32             oopTaskNum = 0;
    OOP_TASK_T         oopTask = {0};
    READ_RECORD_T      inReadRecord;
    uint32             handle = 0;
    uint32             recNum = 0;
    STEP_READ_RECORD_T stepReadRecord;
    RECORD_UNIT_T      stepRecordOut;

    memset(&oopTaskList, 0, sizeof(OOP_TASK_LIST_T));
    memset(&inReadRecord, 0, sizeof(READ_RECORD_T));
    memset(&stepReadRecord, 0, sizeof(STEP_READ_RECORD_T));
    memset(&stepRecordOut, 0, sizeof(RECORD_UNIT_T));

    PRTL_FMT_DEBUG("Initializing tasks. please wait......\n");
    inReadRecord.recordOAD.logicId = 0;
    inReadRecord.recordOAD.infoNum = 0xffff;
    inReadRecord.cType = COL_NO_INFO;
    inReadRecord.cStart = 1;
    inReadRecord.cEnd = 256;
    inReadRecord.sortType = UP_SORT;
    inReadRecord.recordOAD.optional = 0;
    inReadRecord.recordOAD.road.oadMain.value = 0;
    inReadRecord.recordOAD.road.oadCols.nNum = 0x01;
    inReadRecord.recordOAD.road.oadCols.oad[0].value = 0x60120200;

    //不需要附加校验
    stepReadRecord.MAC.nNum = 0;

    ret = db_read_record_start(clientid, &inReadRecord, &handle, &recNum);
    if ((ret != ERR_OK) || (recNum == 0))
    {
        PRTL_FMT_DEBUG("appdb_task_list_record_init db_read_record_start err! ret = %d, infoNum = %d recNum = %d\n", 
            ret, inReadRecord.recordOAD.infoNum, recNum);
    }
    else
    {
        PRTL_FMT_DEBUG("appdb_task_list_record_init db_read_record_start ok ret = %d, infoNum = %d recNum = %d\n", 
            ret, inReadRecord.recordOAD.infoNum, recNum);
        for (i = 0; i < recNum; i++)
        {
            ret = db_read_record_step(clientid, handle, &stepReadRecord, &stepRecordOut);
            if (ret != ERR_OK)
            {
                PRTL_FMT_DEBUG("appdb_task_list_record_init db_read_record_step err = %d!\n", ret);
                continue;
            }

            //检查数据有效性
            if ((stepRecordOut.recordnum != 1) 
            || (stepRecordOut.record[0].road.oadCols.oad[0].value != 0x60120200)
            || (stepRecordOut.record[0].outDatalen != sizeof(OOP_TASK_T)))
            {
                PRTL_FMT_DEBUG("appdb_task_list_record_init data err !\n");
                PRTL_FMT_DEBUG("appdb_task_list_record_init stepRecordOut.recordnum = %d\n", stepRecordOut.recordnum);
                PRTL_FMT_DEBUG("appdb_task_list_record_init stepRecordOut.record[0].road.oadCols.oad[0] = 0x%08x\n", 
                    stepRecordOut.record[0].road.oadCols.oad[0].value);
                PRTL_FMT_DEBUG("appdb_task_list_record_init stepRecordOut.record[0].outDatalen = %d\n", 
                    stepRecordOut.record[0].outDatalen);
                PRTL_FMT_DEBUG("appdb_task_list_record_init sizeof(OOP_TASK_T) = %d\n", 
                    sizeof(OOP_TASK_T));
                continue;
            }

            memcpy(&oopTask, stepRecordOut.record[0].outDataBuf, stepRecordOut.record[0].outDatalen);
            PRTL_FMT_DEBUG("stepRecordOut.infoNum = %d\n", stepRecordOut.infoNum);
            PRTL_FMT_DEBUG("oopTask.id = %d\n", oopTask.id);
            PRTL_FMT_DEBUG("oopTask.planType = %d\n", oopTask.planType);
            PRTL_FMT_DEBUG("oopTask.planID = %d\n", oopTask.planID);

            oopTaskNum++;

            //序号从1开始
            oopTaskList.isChanged[stepRecordOut.infoNum] = TRUE;
            oopTaskList.isVaild[stepRecordOut.infoNum] = TRUE;
            oopTaskList.oopTask[stepRecordOut.infoNum] = oopTask;
            
        }
    }
    db_read_record_end(clientid, handle);
    
    PRTL_FMT_DEBUG("Initialize tasks end.\n");

    /* 记录任务数量 */
    oopTaskList.oopTaskNum = oopTaskNum;

    PRTL_FMT_DEBUG("read task num: %d\n", oopTaskList.oopTaskNum);

    return;
}

/**********************************************************************
* @name      : app_get_task_list
* @brief     ：从缓存中获取任务数据
* @param[in] ：taskID  - 任务序号          
* @param[out]: oopTask - 任务数据
* @return    : 错误码
**********************************************************************/
int app_get_task_list(uint8 taskID, OOP_TASK_T *oopTask)
{
    int32 ret = ERR_NORMAL;

    if(NULL == oopTask)
    {
        return ERR_PNULL;
    }

    /* 使用互斥量进行加锁 */
    pthread_mutex_lock(&oopTaskList.mutex);

    *oopTask = oopTaskList.oopTask[taskID];
    oopTaskList.isChanged[taskID] = FALSE;

    /* 使用互斥量进行解锁 */
    pthread_mutex_unlock(&oopTaskList.mutex);

    if (oopTaskList.isVaild[taskID])
    {
        ret = ERR_OK;
    }
    else
    {
        ret = ERR_INVALID;
    }

    return ret;
}

/**********************************************************************
* @name      : app_set_task_list
* @brief     ：将更新的任务数据写入缓存
* @param[in] ：taskID  - 任务序号          
               oopTask - 任务数据
* @return    ：错误码
**********************************************************************/
int app_set_task_list(uint8 taskID, OOP_TASK_T *oopTask)
{
    if(NULL == oopTask)
    {
        return ERR_PNULL;
    }

    /* 使用互斥量进行加锁 */
    pthread_mutex_lock(&oopTaskList.mutex);

    oopTaskList.oopTask[taskID] = *oopTask;
    oopTaskList.isVaild[taskID] = TRUE;
    oopTaskList.isChanged[taskID] = TRUE;
    if(oopTaskList.isVaild[taskID]==FALSE)
    {
        oopTaskList.oopTaskNum ++;
    }

    /* 使用互斥量进行解锁 */
    pthread_mutex_unlock(&oopTaskList.mutex);

    return ERR_OK;
}

/**********************************************************************
* @name      : app_set_task_invalid
* @brief     ：设置任务数据无效
* @param[in] : taskID - 任务序号
* @param[out]：
* @return    ：错误码
**********************************************************************/
int app_set_task_invalid(uint8 taskID)
{

    /* 使用互斥量进行加锁 */
    pthread_mutex_lock(&oopTaskList.mutex);

    memset(&oopTaskList.oopTask[taskID], 0, sizeof(OOP_TASK_T));
    oopTaskList.isVaild[taskID] = FALSE;
    oopTaskList.isChanged[taskID] = TRUE;
    oopTaskList.oopTaskNum --;

    /* 使用互斥量进行解锁 */
    pthread_mutex_unlock(&oopTaskList.mutex);

    return ERR_OK;
}

/**********************************************************************
* @name      : appdb_get_task_list
* @brief     ：任务列表到缓存
* @param[in] ：int clientid    socket句柄
               uint8 logicId   逻辑地址
               uint16 infoNum  信息点号
               uint32 value    oad值
* @param[out]：
* @return    ：0-成功/错误码
**********************************************************************/
int appdb_get_task_list(int clientid, uint8 logicId, uint16 infoNum, uint32 value)
{
    int                ret = 0;
    uint32             readlen = 0;
    uint32             i = 0;
    NOMAL_OAD_T        tOAD = {0};
    OOP_TASK_T         oopTask = {0};

    if(0x60120200 != value)
    {
        return -1;
    }

    tOAD.logicId   = logicId;
    tOAD.infoNum   = infoNum;
    tOAD.oad.value = value;

    PRTL_FMT_LOGCC("appdb_get_task_list logicId(%d) infoNum(%d) value(0x%x) running.....\n", logicId, infoNum, value);

    if (infoNum == 0xffff)
    {
        oopTaskList.oopTaskNum = 0;
        for (i = 1; i <= 255; i++)
        {
            //判断档案是否有效
            if (!oopTaskList.isVaild[i])
            {
                continue;
            }

            if (app_set_task_invalid(i) < ERR_OK)
            {
                PRTL_FMT_LOGCC("app_set_task_invalid err\n");
            }
        }      
        return 0;
    }

    ret = db_read_nomal(clientid, &tOAD, sizeof(OOP_TASK_T), (uint8 *)&oopTask, &readlen);
    if(0 != ret || readlen != sizeof(OOP_TASK_T))
    {
        app_set_task_invalid(infoNum);
        PRTL_FMT_DEBUG("db_read_nomal failed and app_set_task_invalid. ret(%d), infoNum(%d), readlen(%d), explen(%d)\n", ret, infoNum, readlen, sizeof(OOP_TASK_T));
        return ERR_NORMAL;
    }

    PRTL_FMT_LOGCC("oopTask.taskID = %d\n", oopTask.id);
    PRTL_FMT_LOGCC("oopTask.acqFreq.value = %d\n", oopTask.acqFreq.value);
    PRTL_FMT_LOGCC("oopTask.acqFreq.unit = %d\n", oopTask.acqFreq.unit);
    PRTL_FMT_LOGCC("oopTask.planType = %d\n", oopTask.planType);
    PRTL_FMT_LOGCC("oopTask.planID = %d\n", oopTask.planID);
    PRTL_FMT_LOGCC("oopTask.startTime = %d\n", oopTask.startTime);
    PRTL_FMT_LOGCC("oopTask.endTime = %d\n", oopTask.endTime);
    PRTL_FMT_LOGCC("oopTask.acqDelay = %d\n", oopTask.acqDelay);
    PRTL_FMT_LOGCC("oopTask.state = %d\n", oopTask.state);
    
    ret = app_set_task_list(infoNum, &oopTask);
    if (ret != ERR_OK)
    {
        app_set_task_invalid(infoNum);
        PRTL_FMT_DEBUG("app_set_task_list failed. ret(%d), infoNum(%d)\n", ret, infoNum);
        return ERR_NORMAL;
    }
    
    return 0;
}

#endif

#if DESC("上报方案参数变更" ,1)

/**********************************************************************
* @name      : plan_report_list_lock_init
* @brief     ：
* @param[out]：
* @return    ：
* @Create    : 贺宁
* @Date      ：2020-1-10
* @Update    :
**********************************************************************/
void plan_report_list_lock_init(void)
{
    /* 初始化锁 */
    memset(&oopPlanReportList, 0, sizeof(OOP_PLAN_REPORT_LIST_T));
    pthread_mutex_init(&oopPlanReportList.mutex, NULL);
}

/**
*********************************************************************
* @name：      appdb_plan_report_list_init
* @brief：     档案参数变更初始化
* @param[in] ：clientid  数据库句柄
* @param[out]：
* @return    ：
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
void appdb_plan_report_list_init(int clientid)
{
    int             ret = ERR_NORMAL;
    uint16          planID = 0;
    uint32          oopPlanReportNum = 0;
    NOMAL_OAD_T     tOAD = {0};
    OOP_PLAN_REPORT_T      oopPlanReport = {0};
    uint32          readlen = 0;

    memset(&oopPlanReportList, 0, sizeof(OOP_PLAN_REPORT_LIST_T));

    tOAD.logicId   = 0;
    tOAD.oad.value = 0x601C0200;

    printf("Initializing plan reports. please wait......\n");
    for (planID = 1; planID <= 255; planID++)
    {
        tOAD.infoNum = planID;
        ret = db_read_nomal(clientid, &tOAD, sizeof(OOP_PLAN_REPORT_T), (uint8 *)&oopPlanReport, &readlen);
        if(0 != ret || readlen != sizeof(OOP_PLAN_REPORT_T))
        {
            oopPlanReportList.isVaild[planID] = FALSE;
            continue;
        }
        else
        {
            oopPlanReportNum++;

            //上报方案序号从1开始 
            oopPlanReportList.isChanged[planID] = TRUE;
            oopPlanReportList.isVaild[planID] = TRUE;
            oopPlanReportList.oopPlanReport[planID] = oopPlanReport;
        }
    }
    
    printf("Initialize plan reports end.\n");

    /* 记录上报方案数量 */
    oopPlanReportList.oopPlanReportNum = oopPlanReportNum;

    PRTL_FMT_LOGCC("read plan_report num: %d\n", oopPlanReportList.oopPlanReportNum);

    return;
}

/**
*********************************************************************
* @name：      appdb_plan_report_list_record_init
* @brief：     上报方案参数变更初始化(记录数据方式)
* @param[in] ：clientid  数据库句柄
* @param[out]：
* @return    ：
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
void appdb_plan_report_list_record_init(int clientid)
{
    int                ret = ERR_NORMAL;
    uint32             i = 0;
    uint32             oopPlanReportNum = 0;
    OOP_PLAN_REPORT_T  oopPlanReport = {0};
    READ_RECORD_T      inReadRecord;
    uint32             handle = 0;
    uint32             recNum = 0;
    STEP_READ_RECORD_T stepReadRecord;
    RECORD_UNIT_T      stepRecordOut;

    memset(&oopPlanReportList, 0, sizeof(OOP_PLAN_REPORT_LIST_T));
    memset(&inReadRecord, 0, sizeof(READ_RECORD_T));
    memset(&stepReadRecord, 0, sizeof(STEP_READ_RECORD_T));
    memset(&stepRecordOut, 0, sizeof(RECORD_UNIT_T));

    PRTL_FMT_DEBUG("Initializing plan reports. please wait......\n");
    inReadRecord.recordOAD.logicId = 0;
    inReadRecord.recordOAD.infoNum = 0xffff;
    inReadRecord.cType = COL_NO_INFO;
    inReadRecord.cStart = 1;
    inReadRecord.cEnd = 256;
    inReadRecord.sortType = UP_SORT;
    inReadRecord.recordOAD.optional = 0;
    inReadRecord.recordOAD.road.oadMain.value = 0;
    inReadRecord.recordOAD.road.oadCols.nNum = 0x01;
    inReadRecord.recordOAD.road.oadCols.oad[0].value = 0x601C0200;

    //不需要附加校验
    stepReadRecord.MAC.nNum = 0;

    ret = db_read_record_start(clientid, &inReadRecord, &handle, &recNum);
    if ((ret != ERR_OK) || (recNum == 0))
    {
        PRTL_FMT_DEBUG("appdb_plan_report_list_record_init db_read_record_start err! ret = %d, infoNum = %d recNum = %d\n", 
            ret, inReadRecord.recordOAD.infoNum, recNum);
    }
    else
    {
        PRTL_FMT_DEBUG("appdb_plan_report_list_record_init db_read_record_start ok ret = %d, infoNum = %d recNum = %d\n", 
            ret, inReadRecord.recordOAD.infoNum, recNum);
        for (i = 0; i < recNum; i++)
        {
            ret = db_read_record_step(clientid, handle, &stepReadRecord, &stepRecordOut);
            if (ret != ERR_OK)
            {
                PRTL_FMT_DEBUG("appdb_plan_report_list_record_init db_read_record_step err = %d!\n", ret);
                continue;
            }

            //检查数据有效性
            if ((stepRecordOut.recordnum != 1) 
            || (stepRecordOut.record[0].road.oadCols.oad[0].value != 0x601C0200)
            || (stepRecordOut.record[0].outDatalen != sizeof(OOP_PLAN_REPORT_T)))
            {
                PRTL_FMT_DEBUG("appdb_plan_report_list_record_init data err !\n");
                PRTL_FMT_DEBUG("appdb_plan_report_list_record_init stepRecordOut.recordnum = %d\n", stepRecordOut.recordnum);
                PRTL_FMT_DEBUG("appdb_plan_report_list_record_init stepRecordOut.record[0].road.oadCols.oad[0] = 0x%08x\n", 
                    stepRecordOut.record[0].road.oadCols.oad[0].value);
                PRTL_FMT_DEBUG("appdb_plan_report_list_record_init stepRecordOut.record[0].outDatalen = %d\n", 
                    stepRecordOut.record[0].outDatalen);
                PRTL_FMT_DEBUG("appdb_plan_report_list_record_init sizeof(OOP_PLAN_REPORT_T) = %d\n", 
                    sizeof(OOP_PLAN_REPORT_T));
                continue;
            }

            memcpy(&oopPlanReport, stepRecordOut.record[0].outDataBuf, stepRecordOut.record[0].outDatalen);
            PRTL_FMT_DEBUG("stepRecordOut.infoNum = %d\n", stepRecordOut.infoNum);
            PRTL_FMT_DEBUG("oopPlanReport.planID = %d\n", oopPlanReport.planID);
            PRTL_FMT_DEBUG("oopPlanReport.retry = %d\n", oopPlanReport.retry);
            PRTL_FMT_DEBUG("oopPlanReport.timeout.value = %d\n", oopPlanReport.timeout.value);
            PRTL_FMT_DEBUG("oopPlanReport.timeout.unit = %d\n", oopPlanReport.timeout.unit);

            oopPlanReportNum++;

            //序号从1开始
            oopPlanReportList.isChanged[stepRecordOut.infoNum] = TRUE;
            oopPlanReportList.isVaild[stepRecordOut.infoNum] = TRUE;
            oopPlanReportList.oopPlanReport[stepRecordOut.infoNum] = oopPlanReport;
            
        }
    }
    db_read_record_end(clientid, handle);
    
    PRTL_FMT_DEBUG("Initialize plan reports end.\n");

    /* 记录任务数量 */
    oopPlanReportList.oopPlanReportNum = oopPlanReportNum;

    PRTL_FMT_DEBUG("read reports num: %d\n", oopPlanReportList.oopPlanReportNum);

    return;
}


/**********************************************************************
* @name      : app_get_plan_report_list
* @brief     ：从缓存中获取上报方案数据
* @param[in] ：plan_reportID  - 上报方案序号          
* @param[out]: oopPlanReport - 上报方案数据
* @return    : 错误码
**********************************************************************/
int app_get_plan_report_list(uint8 planID, OOP_PLAN_REPORT_T *oopPlanReport)
{
    int32 ret = ERR_NORMAL;

    if(NULL == oopPlanReport)
    {
        return ERR_PNULL;
    }

    /* 使用互斥量进行加锁 */
    pthread_mutex_lock(&oopPlanReportList.mutex);

    *oopPlanReport = oopPlanReportList.oopPlanReport[planID];
    oopPlanReportList.isChanged[planID] = FALSE;

    /* 使用互斥量进行解锁 */
    pthread_mutex_unlock(&oopPlanReportList.mutex);

    if (oopPlanReportList.isVaild[planID])
    {
        ret = ERR_OK;
    }
    else
    {
        ret = ERR_INVALID;
    }

    return ret;
}

/**********************************************************************
* @name      : app_set_plan_report_list
* @brief     ：将更新的上报方案数据写入缓存
* @param[in] ：plan_reportID  - 上报方案序号          
               oopPlanReport - 上报方案数据
* @return    ：错误码
**********************************************************************/
int app_set_plan_report_list(uint8 planID, OOP_PLAN_REPORT_T *oopPlanReport)
{
    if(NULL == oopPlanReport)
    {
        return ERR_PNULL;
    }

    /* 使用互斥量进行加锁 */
    pthread_mutex_lock(&oopPlanReportList.mutex);

    oopPlanReportList.oopPlanReport[planID] = *oopPlanReport;
    oopPlanReportList.isVaild[planID] = TRUE;
    oopPlanReportList.isChanged[planID] = TRUE;
    if(oopPlanReportList.isVaild[planID]==FALSE)
    {
        oopPlanNormalList.oopPlanNormalNum ++;
    }

    /* 使用互斥量进行解锁 */
    pthread_mutex_unlock(&oopPlanReportList.mutex);

    return ERR_OK;
}

/**********************************************************************
* @name      : app_set_plan_report_invalid
* @brief     ：设置上报方案数据无效
* @param[in] : planID - 上报方案序号
* @param[out]：
* @return    ：错误码
**********************************************************************/
int app_set_plan_report_invalid(uint8 planID)
{

    /* 使用互斥量进行加锁 */
    pthread_mutex_lock(&oopPlanReportList.mutex);

    memset(&oopPlanReportList.oopPlanReport[planID], 0, sizeof(OOP_PLAN_REPORT_T));
    oopPlanReportList.isVaild[planID] = FALSE;
    oopPlanReportList.isChanged[planID] = TRUE;
    oopPlanNormalList.oopPlanNormalNum --;

    /* 使用互斥量进行解锁 */
    pthread_mutex_unlock(&oopPlanReportList.mutex);

    return ERR_OK;
}

/**********************************************************************
* @name      : appdb_get_plan_report_list
* @brief     ：上报方案列表到缓存
* @param[in] ：int clientid    socket句柄
               uint8 logicId   逻辑地址
               uint16 infoNum  信息点号
               uint32 value    oad值
* @param[out]：
* @return    ：0-成功/错误码
**********************************************************************/
int appdb_get_plan_report_list(int clientid, uint8 logicId, uint16 infoNum, uint32 value)
{
    int                ret = 0;
    uint32             readlen = 0;
    NOMAL_OAD_T        tOAD = {0};
    OOP_PLAN_REPORT_T         oopPlanReport = {0};
    uint32 i = 0;
    uint32 j = 0;

    if(0x601C0200 != value)
    {
        return -1;
    }

    tOAD.logicId   = logicId;
    tOAD.infoNum   = infoNum;
    tOAD.oad.value = value;

    PRTL_FMT_LOGCC("appdb_get_plan_report_list logicId(%d) infoNum(%d) value(0x%x) running.....\n", logicId, infoNum, value);

    if (infoNum == 0xffff)
    {
        oopPlanReportList.oopPlanReportNum = 0;
        for (i = 1; i <= 255; i++)
        {
            //判断是否有效
            if (!oopPlanReportList.isVaild[i])
            {
                continue;
            }

            if (app_set_plan_report_invalid(i) < ERR_OK)
            {
                PRTL_FMT_LOGCC("app_set_plan_report_invalid err\n");
            }
        }      
        return 0;
    }

    ret = db_read_nomal(clientid, &tOAD, sizeof(OOP_PLAN_REPORT_T), (uint8 *)&oopPlanReport, &readlen);
    if(0 != ret || readlen != sizeof(OOP_PLAN_REPORT_T))
    {
        app_set_plan_report_invalid(infoNum);
        PRTL_FMT_DEBUG("db_read_nomal failed. ret(%d), oad(0x%08x), infoNum(%d), readlen(%d), explen(%d)\n", ret, tOAD.oad.value, infoNum, readlen, sizeof(OOP_PLAN_REPORT_T));
        return ERR_NORMAL;
    }

    PRTL_FMT_LOGCC("planID = %d\n", oopPlanReport.planID);
    PRTL_FMT_LOGCC("timeout.unit = %d\n", oopPlanReport.timeout.unit);
    PRTL_FMT_LOGCC("timeout.value = %d\n", oopPlanReport.timeout.value);
    PRTL_FMT_LOGCC("retry = %d\n", oopPlanReport.retry);
    PRTL_FMT_LOGCC("data.choice = %d\n", oopPlanReport.data.choice);

    if(oopPlanReport.data.choice == 0)
    {
        PRTL_FMT_LOGCC("data.oad = 0x%08x\n", oopPlanReport.data.oad.value);
    }
    else
    {
        //OAD
        PRTL_FMT_LOGCC("data.record.oad = %d\n", oopPlanReport.data.record.oad.value);

        //RCSD
        for(i = 0; i < oopPlanReport.data.record.tRCSD.nNum; i++)
        {
            PRTL_FMT_LOGCC("data.record.tRCSD.cols[%d].choice = %d\n", i , oopPlanReport.data.record.tRCSD.cols[i].choice);
            if(oopPlanReport.data.record.tRCSD.cols[i].choice == 0)
            {
                PRTL_FMT_LOGCC("data.record.tRCSD.cols[%d].oad = 0x%08x\n", i, oopPlanReport.data.record.tRCSD.cols[i].oad.value); 
            }
            else
            {
                PRTL_FMT_TRACE(".data.record.tRCSD.cols[%d].road.oadMain = 0x%08x\n", j, oopPlanReport.data.record.tRCSD.cols[i].road.oadMain.value);
                for(j = 0; j < oopPlanReport.data.record.tRCSD.cols[i].road.oadCols.nNum; j++)
                {
                    PRTL_FMT_TRACE(".data.record.tRCSD.cols[%d].road.oadCols.oad[%d] = 0x%08x\n", i, j, oopPlanReport.data.record.tRCSD.cols[i].road.oadCols.oad[j].value);
                }
            }
        }

        //RSD
        PRTL_FMT_LOGCC("data.record.tRSD.choice = %d\n", oopPlanReport.data.record.tRSD.choice);
    }
    
    ret = app_set_plan_report_list(infoNum, &oopPlanReport);
    if (ret != ERR_OK)
    {
        app_set_plan_report_invalid(infoNum);
        PRTL_FMT_DEBUG("app_set_plan_report_list failed. ret(%d), infoNum(%d)\n", ret, infoNum);
        return ERR_NORMAL;
    }
    
    return 0;
}

#endif

#if DESC("普通采集方案参数变更" ,1)

/**********************************************************************
* @name      : plan_normal_list_lock_init
* @brief     ：
* @param[out]：
* @return    ：
* @Create    : 梅安宁
* @Date      ：2020-2-27
* @Update    :
**********************************************************************/
void plan_normal_list_lock_init(void)
{
    /* 初始化锁 */
    memset(&oopPlanNormalList, 0, sizeof(OOP_PLAN_NORMAL_LIST_T));
    pthread_mutex_init(&oopPlanNormalList.mutex, NULL);
}

/**
*********************************************************************
* @name：      appdb_plan_normal_list_init
* @brief：     普通采集方案参数变更初始化
* @param[in] ：clientid  数据库句柄
* @param[out]：
* @return    ：
* @Create    : 梅安宁
* @Date      ：2020-2-27
* @Update    :
*********************************************************************
*/
void appdb_plan_normal_list_init(int clientid)
{
    int                 ret              = ERR_NORMAL;
    uint16              planID           = 0;
    uint32              oopPlanNormalNum = 0;
    NOMAL_OAD_T         tOAD             = {0};
    OOP_PLAN_NORMAL_T   oopPlanNormal    = {0};
    uint32              readlen          = 0;

    memset(&oopPlanNormalList, 0, sizeof(OOP_PLAN_NORMAL_LIST_T));

    tOAD.logicId   = 0;
    tOAD.oad.value = 0x60140200;

    printf("Initializing plan normals. please wait......\n");
    for (planID = 1; planID <= 255; planID++)
    {
        tOAD.infoNum = planID;
        ret = db_read_nomal(clientid, &tOAD, sizeof(OOP_PLAN_NORMAL_T), (uint8 *)&oopPlanNormal, &readlen);
        if(0 != ret || readlen != sizeof(OOP_PLAN_NORMAL_T))
        {
            oopPlanNormalList.isVaild[planID] = FALSE;
            continue;
        }
        else
        {
            oopPlanNormalNum++;

            //普通采集方案序号从1开始
            oopPlanNormalList.isVaild[planID] = TRUE;
            oopPlanNormalList.oopPlanNormal[planID] = oopPlanNormal;
        }
    }
    
    printf("Initialize plan normals end.\n");

    /* 记录数量 */
    oopPlanNormalList.oopPlanNormalNum = oopPlanNormalNum;
    oopPlanNormalList.isChanged = TRUE;

    PRTL_FMT_LOGCC("read plan_normal num: %d\n", oopPlanNormalList.oopPlanNormalNum);

    return;
}

/**
*********************************************************************
* @name：      appdb_plan_normal_list_record_init
* @brief：     普通采集方案参数变更初始化(记录数据方式)
* @param[in] ：clientid  数据库句柄
* @param[out]：
* @return    ：
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
void appdb_plan_normal_list_record_init(int clientid)
{
    int                ret = ERR_NORMAL;
    uint32             i = 0;
    uint32             oopPlanNormalNum = 0;
    OOP_PLAN_NORMAL_T  oopPlanNormal = {0};
    READ_RECORD_T      inReadRecord;
    uint32             handle = 0;
    uint32             recNum = 0;
    STEP_READ_RECORD_T stepReadRecord;
    RECORD_UNIT_T      stepRecordOut;

    memset(&oopPlanNormalList, 0, sizeof(OOP_PLAN_NORMAL_LIST_T));
    memset(&inReadRecord, 0, sizeof(READ_RECORD_T));
    memset(&stepReadRecord, 0, sizeof(STEP_READ_RECORD_T));
    memset(&stepRecordOut, 0, sizeof(RECORD_UNIT_T));

    PRTL_FMT_DEBUG("Initializing plan normals. please wait......\n");
    inReadRecord.recordOAD.logicId = 0;
    inReadRecord.recordOAD.infoNum = 0xffff;
    inReadRecord.cType = COL_NO_INFO;
    inReadRecord.cStart = 1;
    inReadRecord.cEnd = 256;
    inReadRecord.sortType = UP_SORT;
    inReadRecord.recordOAD.optional = 0;
    inReadRecord.recordOAD.road.oadMain.value = 0;
    inReadRecord.recordOAD.road.oadCols.nNum = 0x01;
    inReadRecord.recordOAD.road.oadCols.oad[0].value = 0x60140200;

    //不需要附加校验
    stepReadRecord.MAC.nNum = 0;

    ret = db_read_record_start(clientid, &inReadRecord, &handle, &recNum);
    if ((ret != ERR_OK) || (recNum == 0))
    {
        PRTL_FMT_DEBUG("appdb_plan_normal_list_record_init db_read_record_start err! ret = %d, infoNum = %d recNum = %d\n", 
            ret, inReadRecord.recordOAD.infoNum, recNum);
    }
    else
    {
        PRTL_FMT_DEBUG("appdb_plan_normal_list_record_init db_read_record_start ok ret = %d, infoNum = %d recNum = %d\n", 
            ret, inReadRecord.recordOAD.infoNum, recNum);
        for (i = 0; i < recNum; i++)
        {
            diag_ycmsg_open();
            ret = db_read_record_step(clientid, handle, &stepReadRecord, &stepRecordOut);
            diag_ycmsg_close();
            if (ret != ERR_OK)
            {
                PRTL_FMT_DEBUG("appdb_plan_normal_list_record_init db_read_record_step err = %d!\n", ret);
                continue;
            }

            //检查数据有效性
            if ((stepRecordOut.recordnum != 1) 
            || (stepRecordOut.record[0].road.oadCols.oad[0].value != 0x60140200)
            || (stepRecordOut.record[0].outDatalen != sizeof(OOP_PLAN_NORMAL_T)))
            {
                PRTL_FMT_DEBUG("appdb_plan_normal_list_record_init data err !\n");
                PRTL_FMT_DEBUG("appdb_plan_normal_list_record_init stepRecordOut.recordnum = %d\n", stepRecordOut.recordnum);
                PRTL_FMT_DEBUG("appdb_plan_normal_list_record_init stepRecordOut.record[0].road.oadCols.oad[0] = 0x%08x\n", 
                    stepRecordOut.record[0].road.oadCols.oad[0].value);
                PRTL_FMT_DEBUG("appdb_plan_normal_list_record_init stepRecordOut.record[0].outDatalen = %d\n", 
                    stepRecordOut.record[0].outDatalen);
                PRTL_FMT_DEBUG("appdb_plan_normal_list_record_init sizeof(OOP_PLAN_NORMAL_T) = %d\n", 
                    sizeof(OOP_PLAN_NORMAL_T));
                continue;
            }

            memcpy(&oopPlanNormal, stepRecordOut.record[0].outDataBuf, stepRecordOut.record[0].outDatalen);
            PRTL_FMT_DEBUG("stepRecordOut.infoNum = %d\n", stepRecordOut.infoNum);
            PRTL_FMT_DEBUG("oopPlanNormal.planID = %d\n", oopPlanNormal.planID);
            PRTL_FMT_DEBUG("oopPlanNormal.memDepth = %d\n", oopPlanNormal.memDepth);
            PRTL_FMT_DEBUG("oopPlanNormal.recdRow.choice = %d\n", oopPlanNormal.recdRow.choice);
            PRTL_FMT_DEBUG("oopPlanNormal.tmFlag = %d\n", oopPlanNormal.tmFlag);

            oopPlanNormalNum++;

            //序号从1开始
            oopPlanNormalList.isVaild[stepRecordOut.infoNum] = TRUE;
            oopPlanNormalList.oopPlanNormal[stepRecordOut.infoNum] = oopPlanNormal;
            
        }
    }
    db_read_record_end(clientid, handle);
    
    PRTL_FMT_DEBUG("Initialize plan normals end.\n");

    /* 记录数量 */
    oopPlanNormalList.oopPlanNormalNum = oopPlanNormalNum;
    oopPlanNormalList.isChanged = TRUE;

    PRTL_FMT_DEBUG("read plan normals num: %d\n", oopPlanNormalList.oopPlanNormalNum);

    return;
}


/**
*********************************************************************
* @name：     appdb_percetion_termimal_info_init
* @brief：     普通采集方案参数变更初始化(记录数据方式)
* @param[in] ：clientid  数据库句柄
* @param[out]：
* @return    ：
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
void appdb_percetion_termimal_info_init(int clientid)
{
    int   ret;
    uint32 Len;

    NOMAL_OAD_T NormalOad;
    NormalOad.logicId = 0;
    NormalOad.oad.value = 0xFF110E00;
    NormalOad.infoNum = 1;

    ret = db_read_nomal(clientid,&NormalOad,sizeof(perceptionterminal),(uint8*)&perceptionterminal,&Len);
    if(ret!=0 || Len != sizeof(perceptionterminal))
    {
        return;
    }
    return;
}


/**********************************************************************
* @name      : app_get_plan_normal_list
* @brief     ：从缓存中获取普通采集方案数据
* @param[in] ：plan_normalID  - 方案序号          
* @param[out]: oopPlanNormal  - 方案数据
* @return    : 错误码
* @Create    : 梅安宁
* @Date      ：2020-2-27
* @Update    :

**********************************************************************/
int app_get_plan_normal_list(uint8 planID, OOP_PLAN_NORMAL_T *oopPlanNormal)
{
    int32 ret = ERR_NORMAL;

    if(NULL == oopPlanNormal)
    {
        return ERR_PNULL;
    }

    /* 使用互斥量进行加锁 */
    pthread_mutex_lock(&oopPlanNormalList.mutex);

    *oopPlanNormal = oopPlanNormalList.oopPlanNormal[planID];
    oopPlanNormalList.isChanged = FALSE;

    /* 使用互斥量进行解锁 */
    pthread_mutex_unlock(&oopPlanNormalList.mutex);

    if (oopPlanNormalList.isVaild[planID])
    {
        ret = ERR_OK;
    }
    else
    {
        ret = ERR_INVALID;
    }

    return ret;
}

/**********************************************************************
* @name      : app_set_plan_normal_list
* @brief     ：将更新的普通采集方案数据写入缓存
* @param[in] ：planID          - 方案序号          
* @param[out]: oopPlanNormal  - 方案数据
* @return    ：错误码
* @Create    : 梅安宁
* @Date      ：2020-2-27
* @Update    :
**********************************************************************/
int app_set_plan_normal_list(uint8 planID, OOP_PLAN_NORMAL_T *oopPlanNormal)
{
    if(NULL == oopPlanNormal)
    {
        return ERR_PNULL;
    }

    /* 使用互斥量进行加锁 */
    pthread_mutex_lock(&oopPlanNormalList.mutex);

    oopPlanNormalList.oopPlanNormal[planID] = *oopPlanNormal;
    oopPlanNormalList.isVaild[planID] = TRUE;
    oopPlanNormalList.isChanged = TRUE;
    if(oopPlanNormalList.isVaild[planID]==FALSE)
    {
        oopPlanNormalList.oopPlanNormalNum ++;
    }

    /* 使用互斥量进行解锁 */
    pthread_mutex_unlock(&oopPlanNormalList.mutex);

    return ERR_OK;
}

/**********************************************************************
* @name      : app_set_plan_normal_invalid
* @brief     ：设置普通采集方案数据无效
* @param[in] : planID - 上报方案序号
* @param[out]：
* @return    ：错误码
* @Create    : 梅安宁
* @Date      ：2020-2-27
* @Update    :
**********************************************************************/
int app_set_plan_normal_invalid(uint8 planID)
{

    /* 使用互斥量进行加锁 */
    pthread_mutex_lock(&oopPlanNormalList.mutex);

    memset(&oopPlanNormalList.oopPlanNormal[planID], 0, sizeof(OOP_PLAN_NORMAL_T));
    oopPlanNormalList.isVaild[planID] = FALSE;
    oopPlanNormalList.isChanged = TRUE;
    oopPlanNormalList.oopPlanNormalNum --;

    /* 使用互斥量进行解锁 */
    pthread_mutex_unlock(&oopPlanNormalList.mutex);

    return ERR_OK;
}

/**********************************************************************
* @name      : appdb_get_plan_normal_list
* @brief     ：普通采集方案列表到缓存
* @param[in] ：int clientid    socket句柄
               uint8 logicId   逻辑地址
               uint16 infoNum  信息点号
               uint32 value    oad值
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 梅安宁
* @Date      ：2020-2-27
* @Update    :
**********************************************************************/
int appdb_get_plan_normal_list(int clientid, uint8 logicId, uint16 infoNum, uint32 value)
{
    int                i = 0;
    int                ret = 0;
    uint32             readlen = 0;
    NOMAL_OAD_T        tOAD = {0};
    OOP_PLAN_NORMAL_T  oopPlanNormal = {0};

    if(0x60140200 != value)
    {
        return -1;
    }

    tOAD.logicId   = logicId;
    tOAD.infoNum   = infoNum;
    tOAD.oad.value = value;

    PRTL_FMT_LOGCC("appdb_get_meter_list logicId(%d) infoNum(%d) value(0x%x) running.....\n", logicId, infoNum, value);

    if (infoNum == 0xffff)
    {
        oopPlanNormalList.oopPlanNormalNum = 0;
        for (i = 1; i <= 255; i++)
        {
            //判断是否有效
            if (!oopPlanNormalList.isVaild[i])
            {
                continue;
            }

            if (app_set_plan_normal_invalid(i) < ERR_OK)
            {
                PRTL_FMT_LOGCC("app_set_plan_normal_invalid err\n");
            }
        }      
        return 0;
    }

    ret = db_read_nomal(clientid, &tOAD, sizeof(OOP_PLAN_NORMAL_T), (uint8 *)&oopPlanNormal, &readlen);
    if(0 != ret || readlen != sizeof(OOP_PLAN_NORMAL_T))
    {
        app_set_plan_normal_invalid(infoNum);
        PRTL_FMT_DEBUG("db_read_nomal failed. ret(%d), infoNum(%d), readlen(%d), explen(%d)\n", ret, infoNum, readlen, sizeof(OOP_PLAN_NORMAL_T));
        return ERR_NORMAL;
    }
    
    ret = app_set_plan_normal_list(infoNum, &oopPlanNormal);
    if (ret != ERR_OK)
    {
        app_set_plan_normal_invalid(infoNum);
        PRTL_FMT_DEBUG("app_set_plan_normal_list failed. ret(%d), infoNum(%d)\n", ret, infoNum);
        return ERR_NORMAL;
    }
    
    return 0;
}

#endif

#if DESC("透明采集方案参数变更" ,1)

/**********************************************************************
* @name      : plan_normal_list_lock_init
* @brief     ：
* @param[out]：
* @return    ：
* @Create    : 高启翔
* @Date      ：2020-2-27
* @Update    :
**********************************************************************/
void plan_trans_list_lock_init(void)
{
    /* 初始化锁 */
    memset(&oopPlanTransparentList, 0, sizeof(OOP_PLAN_Transparent_LIST_T));
    pthread_mutex_init(&oopPlanTransparentList.mutex, NULL);
}


/**
*********************************************************************
* @name：      appdb_plan_normal_list_init
* @brief：     普通采集方案参数变更初始化
* @param[in] ：clientid  数据库句柄
* @param[out]：
* @return    ：
* @Create    : 梅安宁
* @Date      ：2020-2-27
* @Update    :
*********************************************************************
*/
void appdb_plan_tans_list_init(int clientid)
{
    int                 ret              = ERR_NORMAL;
    uint16              planID           = 0;
    uint32              oopPlanTranslNum = 0;
    NOMAL_OAD_T         tOAD             = {0};
    OOP_PLAN_TRANS_T   oopPlanTrans    = {0};
    uint32              readlen          = 0;

    memset(&oopPlanTransparentList, 0, sizeof(OOP_PLAN_Transparent_LIST_T));

    tOAD.logicId   = 0;
    tOAD.oad.value = 0x601A0200;

    printf("Initializing plan normals. please wait......\n");
    for (planID = 1; planID <= 255; planID++)
    {
        tOAD.infoNum = planID;
        ret = db_read_nomal(clientid, &tOAD, sizeof(OOP_PLAN_TRANS_T), (uint8 *)&oopPlanTrans, &readlen);
        if(0 != ret || readlen != sizeof(OOP_PLAN_TRANS_T))
        {
            oopPlanTransparentList.isVaild[planID] = FALSE;
            continue;
        }
        else
        {
            oopPlanTranslNum++;

            //普通采集方案序号从1开始
            oopPlanTransparentList.isVaild[planID] = TRUE;
            oopPlanTransparentList.oopPlanTransparent[planID] = oopPlanTrans;
        }
    }
    
    printf("Initialize plan normals end.\n");

    /* 记录数量 */
    oopPlanTransparentList.oopPlanTransparentNum = oopPlanTranslNum;
    oopPlanTransparentList.isChanged = TRUE;

    PRTL_FMT_LOGCC("read plan_normal num: %d\n", oopPlanTransparentList.oopPlanTransparentNum);

    return;
}

/**
*********************************************************************
* @name：      appdb_plan_normal_list_record_init
* @brief：     普通采集方案参数变更初始化(记录数据方式)
* @param[in] ：clientid  数据库句柄
* @param[out]：
* @return    ：
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
/**
*********************************************************************
* @name：      appdb_plan_event_list_record_init
* @brief：     透明采集方案参数变更初始化(记录数据方式)
* @param[in] ：clientid  数据库句柄
* @param[out]：
* @return    ：
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
void appdb_plan_transparent_list_record_init(int clientid)
{
    int                ret = ERR_NORMAL;
    uint32             i = 0;
//    uint32             j = 0;
    uint32             oopPlanTransparentNum = 0;
    OOP_PLAN_TRANS_T   PLAN_TRANS_T = {0};
    READ_RECORD_T      inReadRecord;
    uint32             handle = 0;
    uint32             recNum = 0;
    STEP_READ_RECORD_T stepReadRecord;
    RECORD_UNIT_T      stepRecordOut;

    memset(&oopPlanTransparentList, 0, sizeof(OOP_PLAN_Transparent_LIST_T));
    memset(&inReadRecord, 0, sizeof(READ_RECORD_T));
    memset(&stepReadRecord, 0, sizeof(STEP_READ_RECORD_T));
    memset(&stepRecordOut, 0, sizeof(RECORD_UNIT_T));

    PRTL_FMT_DEBUG("Initializing plan_transparent. please wait......\n");
    inReadRecord.recordOAD.logicId = 0;
    inReadRecord.recordOAD.infoNum = 0xffff;
    inReadRecord.cType = COL_NO_INFO;
    inReadRecord.cStart = 1;
    inReadRecord.cEnd = 256;
    inReadRecord.sortType = UP_SORT;
    inReadRecord.recordOAD.optional = 0;
    inReadRecord.recordOAD.road.oadMain.value = 0;
    inReadRecord.recordOAD.road.oadCols.nNum = 0x01;
    inReadRecord.recordOAD.road.oadCols.oad[0].value = 0x60180200;

    //不需要附加校验
    stepReadRecord.MAC.nNum = 0;

    ret = db_read_record_start(clientid, &inReadRecord, &handle, &recNum);
    if ((ret != ERR_OK) || (recNum == 0))
    {
        PRTL_FMT_DEBUG("appdb_plan_transparent_list_record_init db_read_record_start err! ret = %d, infoNum = %d recNum = %d\n", 
            ret, inReadRecord.recordOAD.infoNum, recNum);
    }
    else
    {
        PRTL_FMT_DEBUG("appdb_plan_transparent_list_record_init db_read_record_start ok ret = %d, infoNum = %d recNum = %d\n", 
            ret, inReadRecord.recordOAD.infoNum, recNum);
        for (i = 0; i < recNum; i++)
        {
            ret = db_read_record_step(clientid, handle, &stepReadRecord, &stepRecordOut);
            if (ret != ERR_OK)
            {
                PRTL_FMT_DEBUG("appdb_plan_transparent_list_record_init db_read_record_step err = %d!\n", ret);
                continue;
            }

            //检查数据有效性
            if ((stepRecordOut.recordnum != 1) 
            || (stepRecordOut.record[0].road.oadCols.oad[0].value != 0x60180200)
            || (stepRecordOut.record[0].outDatalen != sizeof(OOP_PLAN_TRANS_T)))
            {
                PRTL_FMT_DEBUG("appdb_plan_transparent_list_record_init data err !\n");
                PRTL_FMT_DEBUG("appdb_plan_transparent_list_record_init stepRecordOut.recordnum = %d\n", stepRecordOut.recordnum);
                PRTL_FMT_DEBUG("appdb_plan_transparent_list_record_init stepRecordOut.record[0].road.oadCols.oad[0] = 0x%08x\n", 
                    stepRecordOut.record[0].road.oadCols.oad[0].value);
                PRTL_FMT_DEBUG("appdb_plan_transparent_list_record_init stepRecordOut.record[0].outDatalen = %d\n", 
                    stepRecordOut.record[0].outDatalen);
                PRTL_FMT_DEBUG("appdb_plan_transparent_list_record_init sizeof(OOP_PLAN_TRANS_T) = %d\n", 
                    sizeof(OOP_PLAN_TRANS_T));
                continue;
            }

            memcpy(&PLAN_TRANS_T, stepRecordOut.record[0].outDataBuf, stepRecordOut.record[0].outDatalen);
            PRTL_FMT_DEBUG("stepRecordOut.infoNum = %d\n", stepRecordOut.infoNum);
            PRTL_FMT_DEBUG("oopPlanEvent.planID = %d\n", PLAN_TRANS_T.planID);
           
           
            

            oopPlanTransparentNum++;

            //序号从1开始
            oopPlanTransparentList.isVaild[stepRecordOut.infoNum] = TRUE;
            oopPlanTransparentList.oopPlanTransparent[stepRecordOut.infoNum] = PLAN_TRANS_T;
            
        }
    }
    db_read_record_end(clientid, handle);
    
    PRTL_FMT_DEBUG("Initialize plan_events end.\n");

    /* 记录数量 */
    oopPlanTransparentList.oopPlanTransparentNum = oopPlanTransparentNum;
    oopPlanTransparentList.isChanged = TRUE;

    PRTL_FMT_DEBUG("read plan_events num: %d\n", oopPlanTransparentList.oopPlanTransparentNum);

    return;
}

/**********************************************************************
* @name      : app_get_plan_event_list
* @brief     ：从缓存中获取事件采集方案数据
* @param[in] ：plan_normalID  - 方案序号          
* @param[out]: oopPlanEvent  - 方案数据
* @return    : 错误码
* @Create    : 梅安宁
* @Date      ：2020-2-27
* @Update    :

**********************************************************************/
int app_get_plan_transparent_list(uint8 planID, OOP_PLAN_TRANS_T *oopPlanTransparent)
{
    int32 ret = ERR_NORMAL;

    if(NULL == oopPlanTransparent)
    {
        return ERR_PNULL;
    }

    /* 使用互斥量进行加锁 */
    pthread_mutex_lock(&oopPlanTransparentList.mutex);

    *oopPlanTransparent = oopPlanTransparentList.oopPlanTransparent[planID];
    oopPlanTransparentList.isChanged = FALSE;

    /* 使用互斥量进行解锁 */
    pthread_mutex_unlock(&oopPlanTransparentList.mutex);

    if (oopPlanTransparentList.isVaild[planID])
    {
        ret = ERR_OK;
    }
    else
    {
        ret = ERR_INVALID;
    }

    return ret;
}


/**********************************************************************
* @name      : app_set_plan_normal_list
* @brief     ：将更新的普通采集方案数据写入缓存
* @param[in] ：planID          - 方案序号          
* @param[out]: oopPlanNormal  - 方案数据
* @return    ：错误码
* @Create    : 梅安宁
* @Date      ：2020-2-27
* @Update    :
**********************************************************************/
int app_set_plan_trans_list(uint8 planID, OOP_PLAN_TRANS_T *oopPlanTrans)
{
    if(NULL == oopPlanTrans)
    {
        return ERR_PNULL;
    }

    /* 使用互斥量进行加锁 */
    pthread_mutex_lock(&oopPlanTransparentList.mutex);

    oopPlanTransparentList.oopPlanTransparent[planID] = *oopPlanTrans;
    oopPlanTransparentList.isVaild[planID] = TRUE;
    oopPlanTransparentList.isChanged = TRUE;
    if(oopPlanTransparentList.isVaild[planID] == FALSE)
    {
        oopPlanTransparentList.oopPlanTransparentNum ++;
    }

    /* 使用互斥量进行解锁 */
    pthread_mutex_unlock(&oopPlanTransparentList.mutex);

    return ERR_OK;
}

/**********************************************************************
* @name      : app_set_plan_normal_invalid
* @brief     ：设置普通采集方案数据无效
* @param[in] : planID - 上报方案序号
* @param[out]：
* @return    ：错误码
* @Create    : 梅安宁
* @Date      ：2020-2-27
* @Update    :
**********************************************************************/
int app_set_plan_trans_invalid(uint8 planID)
{

    /* 使用互斥量进行加锁 */
    pthread_mutex_lock(&oopPlanTransparentList.mutex);

    memset(&oopPlanTransparentList.oopPlanTransparent[planID], 0, sizeof(OOP_PLAN_TRANS_T));
    oopPlanTransparentList.isVaild[planID] = FALSE;
    oopPlanTransparentList.isChanged = TRUE;
    oopPlanTransparentList.oopPlanTransparentNum --;

    /* 使用互斥量进行解锁 */
    pthread_mutex_unlock(&oopPlanTransparentList.mutex);

    return ERR_OK;
}

/**********************************************************************
* @name      : appdb_get_plan_normal_list
* @brief     ：普通采集方案列表到缓存
* @param[in] ：int clientid    socket句柄
               uint8 logicId   逻辑地址
               uint16 infoNum  信息点号
               uint32 value    oad值
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 梅安宁
* @Date      ：2020-2-27
* @Update    :
**********************************************************************/
int appdb_get_plan_trans_list(int clientid, uint8 logicId, uint16 infoNum, uint32 value)
{
    int                i = 0;
    int                ret = 0;
    uint32             readlen = 0;
    NOMAL_OAD_T        tOAD = {0};
    OOP_PLAN_TRANS_T  oopPlanTrans = {0};

    if(0x60180200 != value)
    {
        return -1;
    }

    tOAD.logicId   = logicId;
    tOAD.infoNum   = infoNum;
    tOAD.oad.value = value;

    PRTL_FMT_LOGCC("appdb_get_plan_trans_list logicId(%d) infoNum(%d) value(0x%x) running.....\n", logicId, infoNum, value);

    if (infoNum == 0xffff)
    {
        oopPlanTransparentList.oopPlanTransparentNum = 0;
        for (i = 1; i <= 255; i++)
        {
            //判断是否有效
            if (!oopPlanTransparentList.isVaild[i])
            {
                continue;
            }

            if (app_set_plan_trans_invalid(i) < ERR_OK)
            {
                PRTL_FMT_LOGCC("app_set_plan_normal_invalid err\n");
            }
        }      
        return 0;
    }

    ret = db_read_nomal(clientid, &tOAD, sizeof(OOP_PLAN_TRANS_T), (uint8 *)&oopPlanTrans, &readlen);
    if(0 != ret || readlen != sizeof(OOP_PLAN_TRANS_T))
    {
        app_set_plan_trans_invalid(infoNum);
        PRTL_FMT_DEBUG("db_read_nomal failed. ret(%d), infoNum(%d), readlen(%d), explen(%d)\n", ret, infoNum, readlen, sizeof(OOP_PLAN_TRANS_T));
        return ERR_NORMAL;
    }
    
    ret = app_set_plan_trans_list(infoNum, &oopPlanTrans);
    if (ret != ERR_OK)
    {
        app_set_plan_trans_invalid(infoNum);
        PRTL_FMT_LOGCC("appdb_get_plan_trans_list failed. ret(%d), infoNum(%d)\n", ret, infoNum);
        return ERR_NORMAL;
    }
    PRTL_FMT_LOGCC("oopPlanTransparentNum is %d\n", oopPlanTransparentList.oopPlanTransparentNum);
    return 0;
}


#endif

#if DESC("事件采集方案参数变更" ,1)

/**********************************************************************
* @name      : plan_event_list_lock_init
* @brief     ：
* @param[out]：
* @return    ：
* @Create    : 梅安宁
* @Date      ：2020-2-27
* @Update    :
**********************************************************************/
void plan_event_list_lock_init(void)
{
    /* 初始化锁 */
    memset(&oopPlanEventList, 0, sizeof(OOP_PLAN_EVENT_LIST_T));
    pthread_mutex_init(&oopPlanEventList.mutex, NULL);
}
/**
*********************************************************************
* @name：      appdb_plan_event_list_init
* @brief：     事件采集方案参数变更初始化
* @param[in] ：clientid  数据库句柄
* @param[out]：
* @return    ：
* @Create    : 梅安宁
* @Date      ：2020-2-27
* @Update    :
*********************************************************************
*/
void appdb_plan_event_list_init(int clientid)
{
    int                 ret              = ERR_NORMAL;
    uint16              planID           = 0;
    uint32              oopPlanEventNum  = 0;
    NOMAL_OAD_T         tOAD             = {0};
    OOP_PLAN_EVENT_T    oopPlanEvent     = {0};
    uint32              readlen          = 0;

    memset(&oopPlanEventList, 0, sizeof(OOP_PLAN_EVENT_LIST_T));

    tOAD.logicId   = 0;
    tOAD.oad.value = 0x60160200;

    printf("Initializing plan events. please wait......\n");
    for (planID = 1; planID <= 255; planID++)
    {
        tOAD.infoNum = planID;
        ret = db_read_nomal(clientid, &tOAD, sizeof(OOP_PLAN_EVENT_T), (uint8 *)&oopPlanEvent, &readlen);
        if(0 != ret || readlen != sizeof(OOP_PLAN_EVENT_T))
        {
            oopPlanEventList.isVaild[planID] = FALSE;
            continue;
        }
        else
        {
            oopPlanEventNum++;

            //普通采集方案序号从1开始
            oopPlanEventList.isVaild[planID] = TRUE;
            oopPlanEventList.oopPlanEvent[planID] = oopPlanEvent;
        }
    }
    
    printf("Initialize plan events end.\n");

    /* 记录上报方案数量 */
    oopPlanEventList.oopPlanEventNum = oopPlanEventNum;
    oopPlanEventList.isChanged = TRUE;

    PRTL_FMT_LOGCC("read plan_events num: %d\n", oopPlanEventList.oopPlanEventNum);

    return;
}

/**
*********************************************************************
* @name：      appdb_plan_event_list_record_init
* @brief：     事件采集方案参数变更初始化(记录数据方式)
* @param[in] ：clientid  数据库句柄
* @param[out]：
* @return    ：
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
void appdb_plan_event_list_record_init(int clientid)
{
    int                ret = ERR_NORMAL;
    uint32             i = 0;
    uint32             j = 0;
    uint32             oopPlanEventNum = 0;
    OOP_PLAN_EVENT_T   oopPlanEvent = {0};
    READ_RECORD_T      inReadRecord;
    uint32             handle = 0;
    uint32             recNum = 0;
    STEP_READ_RECORD_T stepReadRecord;
    RECORD_UNIT_T      stepRecordOut;

    memset(&oopPlanEventList, 0, sizeof(OOP_PLAN_EVENT_LIST_T));
    memset(&inReadRecord, 0, sizeof(READ_RECORD_T));
    memset(&stepReadRecord, 0, sizeof(STEP_READ_RECORD_T));
    memset(&stepRecordOut, 0, sizeof(RECORD_UNIT_T));

    PRTL_FMT_DEBUG("Initializing plan_events. please wait......\n");
    inReadRecord.recordOAD.logicId = 0;
    inReadRecord.recordOAD.infoNum = 0xffff;
    inReadRecord.cType = COL_NO_INFO;
    inReadRecord.cStart = 1;
    inReadRecord.cEnd = 256;
    inReadRecord.sortType = UP_SORT;
    inReadRecord.recordOAD.optional = 0;
    inReadRecord.recordOAD.road.oadMain.value = 0;
    inReadRecord.recordOAD.road.oadCols.nNum = 0x01;
    inReadRecord.recordOAD.road.oadCols.oad[0].value = 0x60160200;

    //不需要附加校验
    stepReadRecord.MAC.nNum = 0;

    ret = db_read_record_start(clientid, &inReadRecord, &handle, &recNum);
    if ((ret != ERR_OK) || (recNum == 0))
    {
        PRTL_FMT_DEBUG("appdb_plan_event_list_record_init db_read_record_start err! ret = %d, infoNum = %d recNum = %d\n", 
            ret, inReadRecord.recordOAD.infoNum, recNum);
    }
    else
    {
        PRTL_FMT_DEBUG("appdb_plan_event_list_record_init db_read_record_start ok ret = %d, infoNum = %d recNum = %d\n", 
            ret, inReadRecord.recordOAD.infoNum, recNum);
        for (i = 0; i < recNum; i++)
        {
            ret = db_read_record_step(clientid, handle, &stepReadRecord, &stepRecordOut);
            if (ret != ERR_OK)
            {
                PRTL_FMT_DEBUG("appdb_plan_event_list_record_init db_read_record_step err = %d!\n", ret);
                continue;
            }

            //检查数据有效性
            if ((stepRecordOut.recordnum != 1) 
            || (stepRecordOut.record[0].road.oadCols.oad[0].value != 0x60160200)
            || (stepRecordOut.record[0].outDatalen != sizeof(OOP_PLAN_EVENT_T)))
            {
                PRTL_FMT_DEBUG("appdb_plan_event_list_record_init data err !\n");
                PRTL_FMT_DEBUG("appdb_plan_event_list_record_init stepRecordOut.recordnum = %d\n", stepRecordOut.recordnum);
                PRTL_FMT_DEBUG("appdb_plan_event_list_record_init stepRecordOut.record[0].road.oadCols.oad[0] = 0x%08x\n", 
                    stepRecordOut.record[0].road.oadCols.oad[0].value);
                PRTL_FMT_DEBUG("appdb_plan_event_list_record_init stepRecordOut.record[0].outDatalen = %d\n", 
                    stepRecordOut.record[0].outDatalen);
                PRTL_FMT_DEBUG("appdb_plan_event_list_record_init sizeof(OOP_PLAN_EVENT_T) = %d\n", 
                    sizeof(OOP_PLAN_EVENT_T));
                continue;
            }

            memcpy(&oopPlanEvent, stepRecordOut.record[0].outDataBuf, stepRecordOut.record[0].outDatalen);
            PRTL_FMT_DEBUG("stepRecordOut.infoNum = %d\n", stepRecordOut.infoNum);
            PRTL_FMT_DEBUG("oopPlanEvent.planID = %d\n", oopPlanEvent.planID);
            PRTL_FMT_DEBUG("oopPlanEvent.events.nNum = %d\n", oopPlanEvent.events.nNum);
            for (j = 0; j < oopPlanEvent.events.nNum; j++)
            {
                PRTL_FMT_DEBUG("oopPlanEvent.events.cols[%d].oadMain.value = 0x%08x\n", 
                    j, oopPlanEvent.events.cols[j].oadMain.value);
            }
            PRTL_FMT_DEBUG("oopPlanEvent.bReport = %d\n", oopPlanEvent.bReport);
            PRTL_FMT_DEBUG("oopPlanEvent.nDepth = %d\n", oopPlanEvent.nDepth);
            PRTL_FMT_DEBUG("oopPlanEvent.nType = %d\n", oopPlanEvent.nType);

            oopPlanEventNum++;

            //序号从1开始
            oopPlanEventList.isVaild[stepRecordOut.infoNum] = TRUE;
            oopPlanEventList.oopPlanEvent[stepRecordOut.infoNum] = oopPlanEvent;
            
        }
    }
    db_read_record_end(clientid, handle);
    
    PRTL_FMT_DEBUG("Initialize plan_events end.\n");

    /* 记录数量 */
    oopPlanEventList.oopPlanEventNum = oopPlanEventNum;
    oopPlanEventList.isChanged = TRUE;

    PRTL_FMT_DEBUG("read plan_events num: %d\n", oopPlanEventList.oopPlanEventNum);

    return;
}


/**********************************************************************
* @name      : app_get_plan_event_list
* @brief     ：从缓存中获取事件采集方案数据
* @param[in] ：plan_normalID  - 方案序号          
* @param[out]: oopPlanEvent  - 方案数据
* @return    : 错误码
* @Create    : 梅安宁
* @Date      ：2020-2-27
* @Update    :

**********************************************************************/
int app_get_plan_event_list(uint8 planID, OOP_PLAN_EVENT_T *oopPlanEvent)
{
    int32 ret = ERR_NORMAL;

    if(NULL == oopPlanEvent)
    {
        return ERR_PNULL;
    }

    /* 使用互斥量进行加锁 */
    pthread_mutex_lock(&oopPlanEventList.mutex);

    *oopPlanEvent = oopPlanEventList.oopPlanEvent[planID];
    oopPlanEventList.isChanged = FALSE;

    /* 使用互斥量进行解锁 */
    pthread_mutex_unlock(&oopPlanEventList.mutex);

    if (oopPlanEventList.isVaild[planID])
    {
        ret = ERR_OK;
    }
    else
    {
        ret = ERR_INVALID;
    }

    return ret;
}


/**********************************************************************
* @name      : app_set_plan_event_list
* @brief     ：将更新的事件采集方案数据写入缓存
* @param[in] ：planID          - 方案序号          
* @param[out]: oopPlanEvent  - 方案数据
* @return    ：错误码
* @Create    : 梅安宁
* @Date      ：2020-2-27
* @Update    :
**********************************************************************/
int app_set_plan_event_list(uint8 planID, OOP_PLAN_EVENT_T *oopPlanEvent)
{
    if(NULL == oopPlanEvent)
    {
        return ERR_PNULL;
    }

    /* 使用互斥量进行加锁 */
    pthread_mutex_lock(&oopPlanEventList.mutex);

    oopPlanEventList.oopPlanEvent[planID] = *oopPlanEvent;
    oopPlanEventList.isVaild[planID] = TRUE;
    oopPlanEventList.isChanged = TRUE;
    oopPlanEventList.oopPlanEventNum ++;

    /* 使用互斥量进行解锁 */
    pthread_mutex_unlock(&oopPlanEventList.mutex);

    return ERR_OK;
}

/**********************************************************************
* @name      : app_set_plan_event_invalid
* @brief     ：设置事件采集方案数据无效
* @param[in] : planID - 上报方案序号
* @param[out]：
* @return    ：错误码
* @Create    : 梅安宁
* @Date      ：2020-2-27
* @Update    :
**********************************************************************/
int app_set_plan_event_invalid(uint8 planID)
{

    /* 使用互斥量进行加锁 */
    pthread_mutex_lock(&oopPlanEventList.mutex);

    memset(&oopPlanEventList.oopPlanEvent[planID], 0, sizeof(OOP_PLAN_EVENT_T));
    oopPlanEventList.isVaild[planID] = FALSE;
    oopPlanEventList.isChanged = TRUE;
    oopPlanEventList.oopPlanEventNum --;

    /* 使用互斥量进行解锁 */
    pthread_mutex_unlock(&oopPlanEventList.mutex);

    return ERR_OK;
}

/**********************************************************************
* @name      : appdb_get_plan_event_list
* @brief     ：事件采集方案列表到缓存
* @param[in] ：int clientid    socket句柄
               uint8 logicId   逻辑地址
               uint16 infoNum  信息点号
               uint32 value    oad值
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 梅安宁
* @Date      ：2020-2-27
* @Update    :
**********************************************************************/
int appdb_get_plan_event_list(int clientid, uint8 logicId, uint16 infoNum, uint32 value)
{
    int                ret = 0;
    uint32             readlen = 0;
    uint32             i = 0;
    NOMAL_OAD_T        tOAD = {0};
    OOP_PLAN_EVENT_T   oopPlanEvent = {0};

    if(0x60160200 != value)
    {
        return -1;
    }

    tOAD.logicId   = logicId;
    tOAD.infoNum   = infoNum;
    tOAD.oad.value = value;

    PRTL_FMT_LOGCC("appdb_get_meter_list logicId(%d) infoNum(%d) value(0x%x) running.....\n", logicId, infoNum, value);

    if (infoNum == 0xffff)
    {
        oopPlanEventList.oopPlanEventNum = 0;
        for (i = 1; i <= 255; i++)
        {
            //判断是否有效
            if (!oopPlanEventList.isVaild[i])
            {
                continue;
            }

            if (app_set_plan_event_invalid(i) < ERR_OK)
            {
                PRTL_FMT_LOGCC("app_set_plan_event_invalid err\n");
            }
        }      
        return 0;
    }

    ret = db_read_nomal(clientid, &tOAD, sizeof(OOP_PLAN_EVENT_T), (uint8 *)&oopPlanEvent, &readlen);
    if(0 != ret || readlen != sizeof(OOP_PLAN_EVENT_T))
    {
        app_set_plan_event_invalid(infoNum);
        PRTL_FMT_DEBUG("db_read_nomal failed. ret(%d), infoNum(%d), readlen(%d), explen(%d)\n", ret, infoNum, readlen, sizeof(OOP_PLAN_EVENT_T));
        return ERR_NORMAL;
    }
    
    ret = app_set_plan_event_list(infoNum, &oopPlanEvent);
    if (ret != ERR_OK)
    {
        app_set_plan_event_invalid(infoNum);
        PRTL_FMT_DEBUG("app_set_plan_event_list failed. ret(%d), infoNum(%d)\n", ret, infoNum);
        return ERR_NORMAL;
    }
    
    return 0;
}

#endif

#if DESC("转加密结果" ,1)
/**********************************************************************
* @name      : seek_encryption_list_lock_init
* @brief     ：
* @param[out]：
* @return    ：
* @Date      ：2020-1-10
* @Update    :
**********************************************************************/
void seek_encryption_list_lock_init(void)
{
    /* 初始化锁 */
    memset(&encryptionList, 0, sizeof(OOP_ENCRYPTION_RES_LIST_T));
    pthread_mutex_init(&encryptionList.mutex, NULL);
}

/**
*********************************************************************
* @name：      appdb_encryption_res_list_record_init
* @brief：     变更初始化(记录数据方式)
* @param[in] ：clientid  数据库句柄
* @param[out]：
* @return    ：
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
void appdb_encryption_res_list_record_init(int clientid)
{
    int                 ret = ERR_NORMAL;
    uint32              i = 0;
    uint32              encryptionNum = 0;
    OOP_ENCRYPTRES_T    encryption;
    READ_RECORD_T       inReadRecord;
    uint32              handle = 0;
    uint32              recNum = 0;
    STEP_READ_RECORD_T  stepReadRecord;
    RECORD_UNIT_T       stepRecordOut;

    memset(&encryptionList, 0, sizeof(OOP_ENCRYPTION_RES_LIST_T));
    memset(&inReadRecord, 0, sizeof(READ_RECORD_T));
    memset(&stepReadRecord, 0, sizeof(STEP_READ_RECORD_T));
    memset(&stepRecordOut, 0, sizeof(RECORD_UNIT_T));
    memset(&encryption, 0, sizeof(OOP_ENCRYPTRES_T));

    PRTL_FMT_DEBUG("Initializing encryption. please wait......\n");
    inReadRecord.recordOAD.logicId = 0;
    inReadRecord.recordOAD.infoNum = 0xffff;
    inReadRecord.cType = COL_NO_INFO;
    inReadRecord.cStart = 1;
    inReadRecord.cEnd = 256;
    inReadRecord.sortType = UP_SORT;
    inReadRecord.recordOAD.optional = 0;
    inReadRecord.recordOAD.road.oadMain.value = 0;
    inReadRecord.recordOAD.road.oadCols.nNum = 0x01;
    inReadRecord.recordOAD.road.oadCols.oad[0].value = 0x60200200;

    //不需要附加校验
    stepReadRecord.MAC.nNum = 0;

    ret = db_read_record_start(clientid, &inReadRecord, &handle, &recNum);
    if ((ret != ERR_OK) || (recNum == 0))
    {
        PRTL_FMT_DEBUG("appdb_encryption_res_list_record_init db_read_record_start err! ret = %d, infoNum = %d recNum = %d\n", 
            ret, inReadRecord.recordOAD.infoNum, recNum);
    }
    else
    {
        PRTL_FMT_DEBUG("appdb_encryption_res_list_record_init db_read_record_start ok ret = %d, infoNum = %d recNum = %d\n", 
            ret, inReadRecord.recordOAD.infoNum, recNum);
        for (i = 0; i < recNum; i++)
        {
            ret = db_read_record_step(clientid, handle, &stepReadRecord, &stepRecordOut);
            if (ret != ERR_OK)
            {
                PRTL_FMT_DEBUG("appdb_encryption_res_list_record_init db_read_record_step err = %d!\n", ret);
                continue;
            }

            //检查数据有效性
            if ((stepRecordOut.recordnum != 1) 
            || (stepRecordOut.record[0].road.oadCols.oad[0].value != 0x60200200)
            || (stepRecordOut.record[0].outDatalen != sizeof(OOP_ENCRYPTRES_T)))
            {
                PRTL_FMT_DEBUG("appdb_encryption_res_list_record_init data err !\n");
                PRTL_FMT_DEBUG("appdb_encryption_res_list_record_init stepRecordOut.recordnum = %d\n", stepRecordOut.recordnum);
                PRTL_FMT_DEBUG("appdb_encryption_res_list_record_init stepRecordOut.record[0].road.oadCols.oad[0] = 0x%08x\n", 
                    stepRecordOut.record[0].road.oadCols.oad[0].value);
                PRTL_FMT_DEBUG("appdb_encryption_res_list_record_init stepRecordOut.record[0].outDatalen = %d\n", 
                    stepRecordOut.record[0].outDatalen);
                PRTL_FMT_DEBUG("appdb_encryption_res_list_record_init sizeof(OOP_ENCRYPTRES_T) = %d\n", 
                    sizeof(OOP_ENCRYPTRES_T));
                continue;
            }

            memcpy(&encryption, stepRecordOut.record[0].outDataBuf, stepRecordOut.record[0].outDatalen);
            PRTL_FMT_DEBUG("stepRecordOut.infoNum = %d\n", stepRecordOut.infoNum);
            PRTL_FMT_DEBUG("encryption.planID = %d\n", encryption.planID);
            PRTL_FMT_DEBUG("encryption.planType = %d\n", encryption.planType);
            PRTL_FMT_DEBUG("encryption.nMetTotal = %d\n", encryption.nMetTotal);

            encryptionNum++;

            //序号从1开始
            encryptionList.isChanged[stepRecordOut.infoNum] = TRUE;
            encryptionList.isVaild[stepRecordOut.infoNum] = TRUE;
            encryptionList.encryption[stepRecordOut.infoNum] = encryption;
            encryptionList.isReport[stepRecordOut.infoNum] = TRUE; 
        }
    }
    db_read_record_end(clientid, handle);
    
    PRTL_FMT_DEBUG("Initialize encryption end.\n");

    /* 记录任务数量 */
    encryptionList.encryptionNum = encryptionNum;

    PRTL_FMT_DEBUG("read encryption num: %d\n", encryptionList.encryptionNum);

    return;
}

/**********************************************************************
* @name      : app_get_encryption_res_list
* @brief     ：从缓存中获取转加密结果数据
* @param[in] ：infoNum   - 序号          
* @param[out]: encryption - 数据
* @return    : 错误码
**********************************************************************/
int app_get_encryption_res_list(uint16 infoNum, OOP_ENCRYPTRES_T *encryption)
{
    int32 ret = ERR_NORMAL;

    if(NULL == encryption)
    {
        return ERR_PNULL;
    }

    /* 使用互斥量进行加锁 */
    pthread_mutex_lock(&encryptionList.mutex);

    *encryption = encryptionList.encryption[infoNum];
    encryptionList.isChanged[infoNum] = FALSE;

    /* 使用互斥量进行解锁 */
    pthread_mutex_unlock(&encryptionList.mutex);

    if (encryptionList.isVaild[infoNum])
    {
        ret = ERR_OK;
    }
    else
    {
        ret = ERR_INVALID;
    }

    return ret;
}

/**********************************************************************
* @name      : app_check_encryption_res_isreport
* @brief     ：检查转加密结果是否已上报过未知电表事件
* @param[in] ：infoNum   - 任务序号          
               
* @return    ：
**********************************************************************/
BOOLEAN app_check_encryption_res_isreport(uint16 infoNum)
{
    BOOLEAN ret = FALSE;
    pthread_mutex_lock(&encryptionList.mutex);

    ret = encryptionList.isReport[infoNum];

    /* 使用互斥量进行解锁 */
    pthread_mutex_unlock(&encryptionList.mutex);
    return ret;
}

/**********************************************************************
* @name      : app_set_encryption_list
* @brief     ：将更新的转加密数据写入缓存
* @param[in] ：infoNum   - 任务序号          
               encryption - 任务数据
* @return    ：错误码
**********************************************************************/
int app_set_encryption_list(uint16 infoNum, OOP_ENCRYPTRES_T *encryption)
{
    if(NULL == encryption)
    {
        return ERR_PNULL;
    }

    /* 使用互斥量进行加锁 */
    pthread_mutex_lock(&encryptionList.mutex);

    encryptionList.encryption[infoNum] = *encryption;
    encryptionList.isVaild[infoNum] = TRUE;
    encryptionList.isChanged[infoNum] = TRUE;
    encryptionList.isReport[infoNum] = FALSE;
    encryptionList.encryptionNum ++;
    PRTL_FMT_LOGCC("app_set_encryption_list encryptionNum = %d\n", encryptionList.encryptionNum );

    /* 使用互斥量进行解锁 */
    pthread_mutex_unlock(&encryptionList.mutex);

    return ERR_OK;
}

/**********************************************************************
* @name      : app_set_encryption_invalid
* @brief     ：设置转加密数据无效
* @param[in] : infoNum - 序号
* @param[out]：
* @return    ：错误码
**********************************************************************/
int app_set_encryption_invalid(uint16 infoNum)
{

    /* 使用互斥量进行加锁 */
    pthread_mutex_lock(&encryptionList.mutex);

    memset(&encryptionList.encryption[infoNum], 0, sizeof(OOP_ENCRYPTRES_T));
    encryptionList.isVaild[infoNum] = FALSE;
    encryptionList.isChanged[infoNum] = TRUE;
    if(encryptionList.encryptionNum>0)
        encryptionList.encryptionNum --;

    /* 使用互斥量进行解锁 */
    pthread_mutex_unlock(&encryptionList.mutex);

    return ERR_OK;
}

/**********************************************************************
* @name      : appdb_get_encryption_res_list
* @brief     ：搜表数据列表到缓存
* @param[in] ：int clientid    socket句柄
               uint8 logicId   逻辑地址
               uint16 infoNum  信息点号
               uint32 value    oad值
* @param[out]：
* @return    ：0-成功/错误码
**********************************************************************/
int appdb_get_encryption_res_list(int clientid, uint8 logicId, uint16 infoNum, uint32 value)
{
    int                 ret = 0;
    uint32              readlen = 0;
    uint32              i = 0;
    NOMAL_OAD_T         tOAD = {0};
    OOP_ENCRYPTRES_T    encryption;

    if(0x60200200 != value)
    {
        return -1;
    }

    tOAD.logicId   = logicId;
    tOAD.infoNum   = infoNum;
    tOAD.oad.value = value;

    MEMZERO(&encryption, sizeof(encryption));

    PRTL_FMT_LOGCC("appdb_get_encryption_res_list logicId(%d) infoNum(%d) value(0x%x) running.....\n", logicId, infoNum, value);

    if (infoNum == 0xffff)
    {
        encryptionList.encryptionNum = 0;
        for (i = 1; i <= NO_VIR_MTR_MAX_NUM; i++)
        {
            //判断是否有效
            if (!encryptionList.isVaild[i])
            {
                continue;
            }

            if (app_set_encryption_invalid(i) < ERR_OK)
            {
                PRTL_FMT_LOGCC("app_set_seek_meter_invalid err\n");
            }
        }      
        return 0;
    }

    ret = db_read_nomal(clientid, &tOAD, sizeof(OOP_ENCRYPTRES_T), (uint8 *)&encryption, &readlen);
    if(0 != ret || readlen != sizeof(OOP_ENCRYPTRES_T))
    {
        app_set_encryption_invalid(infoNum);
        PRTL_FMT_DEBUG("db_read_nomal failed. ret(%d), infoNum(%d), readlen(%d), explen(%d)\n", ret, infoNum, readlen, sizeof(OOP_ENCRYPTRES_T));
        return ERR_NORMAL;
    }

    PRTL_FMT_LOGCC("encryption.planID = %d\n", encryption.planID);
    PRTL_FMT_LOGCC("encryption.planType = %d\n", encryption.planType);
    PRTL_FMT_LOGCC("encryption.nMetTotal = %d\n", encryption.nMetTotal);
    
    ret = app_set_encryption_list(infoNum, &encryption);
    if (ret != ERR_OK)
    {
        app_set_encryption_invalid(infoNum);
        PRTL_FMT_DEBUG("appdb_get_encryption_res_list failed. ret(%d), infoNum(%d)\n", ret, infoNum);
        return ERR_NORMAL;
    }

    return 0;
}

/**********************************************************************
* @name      : app_get_encryption_count
* @brief     ：获取转加密结果个数
* @param[in] : 
* @param[out]：
* @return    ：结果个数
**********************************************************************/
uint16 app_get_encryption_count(void)
{
    uint16 ret;
    /* 使用互斥量进行加锁 */
    pthread_mutex_lock(&encryptionList.mutex);

    ret = encryptionList.encryptionNum ;
    PRTL_FMT_LOGCC("app_get_encryption_count = %d\n", ret);

    /* 使用互斥量进行解锁 */
    pthread_mutex_unlock(&encryptionList.mutex);

    return ret;
}

/**********************************************************************
* @name      : app_datainit_encryption
* @brief     ：收到数据初始化事件后清空搜表结果
* @param[in] : 
* @param[out]：
* @return    ：
**********************************************************************/
void app_datainit_encryption(uint8 logicid,uint16 classtag)
{
    PRTL_FMT_DEBUG("app_datainit_encryption logicid = %d classtag = %d\n", logicid,classtag);
    if (logicid == 0 && (classtag == CLASS_DATA_NORMAL || classtag == 0))
    {
        /* 使用互斥量进行加锁 */
        pthread_mutex_lock(&encryptionList.mutex);

        encryptionList.encryptionNum = 0 ;
        memset(encryptionList.isChanged, 0x00, sizeof(BOOLEAN)*(256));
        memset(encryptionList.isVaild, 0x00, sizeof(BOOLEAN)*(256));
        memset(encryptionList.encryption, 0x00, sizeof(OOP_ENCRYPTRES_T)*(256));
        memset(encryptionList.isReport, 0x00, sizeof(BOOLEAN)*(256));

        /* 使用互斥量进行解锁 */
        pthread_mutex_unlock(&encryptionList.mutex);
    }
}

#endif

#if DESC("搜表数据变更" ,1)

/**********************************************************************
* @name      : seek_meter_list_lock_init
* @brief     ：
* @param[out]：
* @return    ：
* @Date      ：2020-1-10
* @Update    :
**********************************************************************/
void seek_meter_list_lock_init(void)
{
    /* 初始化锁 */
    memset(&seekMeterList, 0, sizeof(OOP_SEEK_METER_LIST_T));
    pthread_mutex_init(&seekMeterList.mutex, NULL);
}

/**
*********************************************************************
* @name：      appdb_seek_meter_list_record_init
* @brief：     搜表结果变更初始化(记录数据方式)
* @param[in] ：clientid  数据库句柄
* @param[out]：
* @return    ：
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
void appdb_seek_meter_list_record_init(int clientid)
{
    int                 ret = ERR_NORMAL;
    uint32              i = 0;
    uint32              seekMeterNum = 0;
    OOP_SEARCHMET_RES_T seekMeter;
    READ_RECORD_T       inReadRecord;
    uint32              handle = 0;
    uint32              recNum = 0;
    STEP_READ_RECORD_T  stepReadRecord;
    RECORD_UNIT_T       stepRecordOut;

    memset(&seekMeterList, 0, sizeof(OOP_SEEK_METER_LIST_T));
    memset(&inReadRecord, 0, sizeof(READ_RECORD_T));
    memset(&stepReadRecord, 0, sizeof(STEP_READ_RECORD_T));
    memset(&stepRecordOut, 0, sizeof(RECORD_UNIT_T));
    memset(&seekMeter, 0, sizeof(OOP_SEARCHMET_RES_T));

    PRTL_FMT_DEBUG("Initializing seek meters. please wait......\n");
    inReadRecord.recordOAD.logicId = 0;
    inReadRecord.recordOAD.infoNum = 0xffff;
    inReadRecord.cType = COL_NO_INFO;
    inReadRecord.cStart = 1;
    inReadRecord.cEnd = NO_VIR_MTR_MAX_NUM + 1;
    inReadRecord.sortType = UP_SORT;
    inReadRecord.recordOAD.optional = 0;
    inReadRecord.recordOAD.road.oadMain.value = 0;
    inReadRecord.recordOAD.road.oadCols.nNum = 0x01;
    inReadRecord.recordOAD.road.oadCols.oad[0].value = 0x60020200;

    //不需要附加校验
    stepReadRecord.MAC.nNum = 0;

    ret = db_read_record_start(clientid, &inReadRecord, &handle, &recNum);
    if ((ret != ERR_OK) || (recNum == 0))
    {
        PRTL_FMT_DEBUG("appdb_seek_meter_list_record_init db_read_record_start err! ret = %d, infoNum = %d recNum = %d\n", 
            ret, inReadRecord.recordOAD.infoNum, recNum);
    }
    else
    {
        PRTL_FMT_DEBUG("appdb_seek_meter_list_record_init db_read_record_start ok ret = %d, infoNum = %d recNum = %d\n", 
            ret, inReadRecord.recordOAD.infoNum, recNum);
        for (i = 0; i < recNum; i++)
        {
            ret = db_read_record_step(clientid, handle, &stepReadRecord, &stepRecordOut);
            if (ret != ERR_OK)
            {
                PRTL_FMT_DEBUG("appdb_seek_meter_list_record_init db_read_record_step err = %d!\n", ret);
                continue;
            }

            //检查数据有效性
            if ((stepRecordOut.recordnum != 1) 
            || (stepRecordOut.record[0].road.oadCols.oad[0].value != 0x60020200)
            || (stepRecordOut.record[0].outDatalen != sizeof(OOP_SEARCHMET_RES_T)))
            {
                PRTL_FMT_DEBUG("appdb_seek_meter_list_record_init data err !\n");
                PRTL_FMT_DEBUG("appdb_seek_meter_list_record_init stepRecordOut.recordnum = %d\n", stepRecordOut.recordnum);
                PRTL_FMT_DEBUG("appdb_seek_meter_list_record_init stepRecordOut.record[0].road.oadCols.oad[0] = 0x%08x\n", 
                    stepRecordOut.record[0].road.oadCols.oad[0].value);
                PRTL_FMT_DEBUG("appdb_seek_meter_list_record_init stepRecordOut.record[0].outDatalen = %d\n", 
                    stepRecordOut.record[0].outDatalen);
                PRTL_FMT_DEBUG("appdb_seek_meter_list_record_init sizeof(OOP_SEARCHMET_RES_T) = %d\n", 
                    sizeof(OOP_SEARCHMET_RES_T));
                continue;
            }

            memcpy(&seekMeter, stepRecordOut.record[0].outDataBuf, stepRecordOut.record[0].outDatalen);
            PRTL_FMT_DEBUG("stepRecordOut.infoNum = %d\n", stepRecordOut.infoNum);
            PRTL_FMT_DEBUG("seekMeter.commAdd.len = %d\n", seekMeter.commAdd.len);
            PRTL_BUF_DEBUG(seekMeter.commAdd.add, seekMeter.commAdd.len, "seekMeter.commAdd.add:");
            PRTL_FMT_DEBUG("seekMeter.collAdd.len = %d\n", seekMeter.collAdd.len);
            PRTL_BUF_DEBUG(seekMeter.collAdd.add, seekMeter.collAdd.len, "seekMeter.collAdd.add:");
            PRTL_FMT_DEBUG("seekMeter.serchTime.year = %d\n", seekMeter.serchTime.year);
            PRTL_FMT_DEBUG("seekMeter.serchTime.month = %d\n", seekMeter.serchTime.month);
            PRTL_FMT_DEBUG("seekMeter.serchTime.day = %d\n", seekMeter.serchTime.day);
            PRTL_FMT_DEBUG("seekMeter.serchTime.hour = %d\n", seekMeter.serchTime.hour);
            PRTL_FMT_DEBUG("seekMeter.serchTime.minute = %d\n", seekMeter.serchTime.minute);
            PRTL_FMT_DEBUG("seekMeter.serchTime.second = %d\n", seekMeter.serchTime.second);

            seekMeterNum++;

            //序号从1开始
            seekMeterList.isChanged[stepRecordOut.infoNum] = TRUE;
            seekMeterList.isVaild[stepRecordOut.infoNum] = TRUE;
            seekMeterList.seekMeter[stepRecordOut.infoNum] = seekMeter;
            seekMeterList.isReport[stepRecordOut.infoNum] = TRUE; 
        }
    }
    db_read_record_end(clientid, handle);
    
    PRTL_FMT_DEBUG("Initialize tasks end.\n");

    /* 记录任务数量 */
    seekMeterList.seekMeterNum = seekMeterNum;

    PRTL_FMT_DEBUG("read task num: %d\n", seekMeterList.seekMeterNum);

    return;
}

/**********************************************************************
* @name      : app_get_seek_meter_list
* @brief     ：从缓存中获取搜表结果数据
* @param[in] ：infoNum   - 序号          
* @param[out]: seekMeter - 数据
* @return    : 错误码
**********************************************************************/
int app_get_seek_meter_list(uint16 infoNum, OOP_SEARCHMET_RES_T *seekMeter)
{
    int32 ret = ERR_NORMAL;

    if(NULL == seekMeter)
    {
        return ERR_PNULL;
    }

    /* 使用互斥量进行加锁 */
    pthread_mutex_lock(&seekMeterList.mutex);

    *seekMeter = seekMeterList.seekMeter[infoNum];
    seekMeterList.isChanged[infoNum] = FALSE;

    /* 使用互斥量进行解锁 */
    pthread_mutex_unlock(&seekMeterList.mutex);

    if (seekMeterList.isVaild[infoNum])
    {
        ret = ERR_OK;
    }
    else
    {
        ret = ERR_INVALID;
    }

    return ret;
}
/**********************************************************************
* @name      : app_check_seek_meter_isreport
* @brief     ：检查搜表结果是否已上报过未知电表事件
* @param[in] ：infoNum   - 任务序号          
               
* @return    ：
**********************************************************************/
BOOLEAN app_check_seek_meter_isreport(uint16 infoNum)
{
    BOOLEAN ret = FALSE;
    pthread_mutex_lock(&seekMeterList.mutex);

    ret = seekMeterList.isReport[infoNum];

    /* 使用互斥量进行解锁 */
    pthread_mutex_unlock(&seekMeterList.mutex);
    return ret;
}

/**********************************************************************
* @name      : app_set_seek_meter_isreport
* @brief     ：将搜表结果置为已上报
* @param[in] ：infoNum   - 任务序号          
               
* @return    ：
**********************************************************************/
void app_set_seek_meter_isreport(uint16 infoNum)
{
    pthread_mutex_lock(&seekMeterList.mutex);

    seekMeterList.isReport[infoNum] = TRUE;

    /* 使用互斥量进行解锁 */
    pthread_mutex_unlock(&seekMeterList.mutex);
}

/**********************************************************************
* @name      : app_set_seek_meter_list
* @brief     ：将更新的搜表数据写入缓存
* @param[in] ：infoNum   - 任务序号          
               seekMeter - 任务数据
* @return    ：错误码
**********************************************************************/
int app_set_seek_meter_list(uint16 infoNum, OOP_SEARCHMET_RES_T *seekMeter)
{
    if(NULL == seekMeter)
    {
        return ERR_PNULL;
    }

    /* 使用互斥量进行加锁 */
    pthread_mutex_lock(&seekMeterList.mutex);

    seekMeterList.seekMeter[infoNum] = *seekMeter;
    seekMeterList.isVaild[infoNum] = TRUE;
    seekMeterList.isChanged[infoNum] = TRUE;
    seekMeterList.isReport[infoNum] = FALSE;
    seekMeterList.seekMeterNum ++;
    PRTL_FMT_LOGCC("app_set_seek_meter_list seekMeterNum = %d\n", seekMeterList.seekMeterNum );

    /* 使用互斥量进行解锁 */
    pthread_mutex_unlock(&seekMeterList.mutex);

    return ERR_OK;
}

/**********************************************************************
* @name      : app_set_seek_meter_invalid
* @brief     ：设置搜表数据无效
* @param[in] : infoNum - 序号
* @param[out]：
* @return    ：错误码
**********************************************************************/
int app_set_seek_meter_invalid(uint16 infoNum)
{

    /* 使用互斥量进行加锁 */
    pthread_mutex_lock(&seekMeterList.mutex);

    memset(&seekMeterList.seekMeter[infoNum], 0, sizeof(OOP_SEARCHMET_RES_T));
    seekMeterList.isVaild[infoNum] = FALSE;
    seekMeterList.isChanged[infoNum] = TRUE;
    if(seekMeterList.seekMeterNum>0)
        seekMeterList.seekMeterNum --;

    /* 使用互斥量进行解锁 */
    pthread_mutex_unlock(&seekMeterList.mutex);

    return ERR_OK;
}

#if GET_NEW_METER_EN == 1

/**********************************************************************
* @name      : appdb_update_meter_form_seek
* @brief     ：建搜表结果转换成档案数据
* @param[in] ：infoNum     信息点
               oopMeter    档案
               seekMeter   搜表结果
* @param[out]：
* @return    ：
**********************************************************************/
void appdb_update_meter_form_seek(uint16 infoNum, OOP_METER_T *oopMeter, OOP_SEARCHMET_RES_T seekMeter)
{
    OOP_METER_T meter;
    int32       i = 0;

    MEMZERO(&meter, sizeof(OOP_METER_T));

    meter.nIndex = infoNum;

    // 基本类型
    memcpy(&meter.basic.tsa, &seekMeter.commAdd, sizeof(OOP_TSA_T));
    meter.basic.protocol = seekMeter.proto;

    for (i = 0; i < OOP_MAX_ADDEDINFO; i++)
    {
        if (seekMeter.info[i].oad.value == 0x60060200)
        {
            meter.basic.port.value = seekMeter.info[i].value.oad.value;
        }
        else if (seekMeter.info[i].oad.value == 0x60070200)
        {
            meter.basic.baud = seekMeter.info[i].value.byVal;
        }
    }

    meter.basic.rateNum = 4;
    meter.basic.userType = 1;
    meter.basic.pwrType = 1;
    meter.basic.pwdLen = 6;
    meter.basic.stVolt = 220;
    meter.basic.stAmp = 5;

    // 扩展信息
    memcpy(&meter.extend.add, &seekMeter.collAdd, sizeof(OOP_TSA_T));
    meter.extend.astLen = 6;
    meter.extend.PT = 1;
    meter.extend.CT = 1;


    for(i=0;i<seekMeter.nAdded;i++)
    {
        if(seekMeter.info[i].oad.value == 0x6E010200)
        {
            meter.annex.nNum = 1;
            meter.annex.annexObj[0].oad.value = 0x6E010200;
            meter.annex.annexObj[0].nLen = sizeof(uint16);
            memcpy(meter.annex.annexObj[0].value,&seekMeter.info[i].value.wVal,sizeof(uint16));
            break;
        }
    }


    *oopMeter = meter;

    return;
}

/**********************************************************************
* @name      : get_new_meter_form_seek
* @brief     ：搜表数据列表到缓存，检查是否需要更新档案
* @param[in] ：clientid    socket句柄
               seekMeter   搜表结果
* @param[out]：
* @return    ：
**********************************************************************/
void get_new_meter_form_seek(int clientid, OOP_SEARCHMET_RES_T seekMeter)
{
    int         i = 0;
    int         j = 0;
    int         ret = 0;
    OOP_METER_T oopMeter;
    NOMAL_OAD_T nomalOad = {0};
    OOP_SEARCHPARAM_T searchParam = {0};

    ret = app_get_search_param(&searchParam);
    if (((ret == ERR_OK) && (!searchParam.atuoUpdate)) || (ret != ERR_OK))
    {
        PRTL_FMT_TRACE("no limits to update meters.\n");
        return;
    }

    MEMZERO(&oopMeter, sizeof(OOP_METER_T));
    for (i = 1; i <= NO_VIR_MTR_MAX_NUM; i++)
    {
        ret = app_get_meter_list(i, &oopMeter);
        if (ret != ERR_OK)
        {
            continue;
        }

        if (memcmp(oopMeter.basic.tsa.add, seekMeter.commAdd.add, 6) == 0)
        {
            break;
        }
    }

    //如果在档案里没有找到地址一样的表，添加到档案
    if (i == NO_VIR_MTR_MAX_NUM + 1)
    {
        //找到无效测量点
        for (j = 1; j <= NO_VIR_MTR_MAX_NUM; j++)
        {
            if (is_meter_invalid(j) == 0)
            {
                break;
            }
        }

        if (j == NO_VIR_MTR_MAX_NUM + 1)
        {
            PRTL_FMT_DEBUG("inform is full\n");
            return;
        }

        MEMZERO(&oopMeter, sizeof(OOP_METER_T));
        appdb_update_meter_form_seek(j, &oopMeter, seekMeter);

        PRTL_FMT_TRACE("oopMeter.nIndex = %d\n", oopMeter.nIndex);
        PRTL_BUF_TRACE(oopMeter.basic.tsa.add, oopMeter.basic.tsa.len + 1, "oopMeter.addr:");
        PRTL_FMT_TRACE("oopMeter.basic.baud = %d\n", oopMeter.basic.baud);
        PRTL_FMT_TRACE("oopMeter.basic.protocol = %d\n", oopMeter.basic.protocol);
        PRTL_FMT_TRACE("oopMeter.basic.port = 0x%08x\n", oopMeter.basic.port.value);
        PRTL_BUF_TRACE(oopMeter.extend.add.add, oopMeter.extend.add.len + 1, "extend.addr:");

        nomalOad.logicId = 0x00;
        nomalOad.infoNum = j;
        nomalOad.oad.value = 0x60000200;
        nomalOad.classtag = CLASS_DATA_INIT;
        ret = db_write_nomal(clientid, &nomalOad, (uint8*)&oopMeter, sizeof(OOP_METER_T));
        if (ret != ERR_OK)
        {
            PRTL_FMT_DEBUG("class11_invoke_act_oop_meter db_write_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                ret, nomalOad.logicId, nomalOad.infoNum, nomalOad.oad.value);
            return;
        }

        // 先写入档案全局变量，防止档案更新慢
        ret = app_set_meter_list(nomalOad.infoNum, &oopMeter);
        if (ret != ERR_OK)
        {
            if (app_set_meter_invalid(nomalOad.infoNum) < ERR_OK)
            {
                PRTL_FMT_DEBUG("app_set_meter_invalid err\n");
            }
            PRTL_FMT_DEBUG("app_set_meter_list err\n");
            return;
        }
    }

    return;
}
#endif

/**********************************************************************
* @name      : appdb_get_seek_meter_list
* @brief     ：搜表数据列表到缓存
* @param[in] ：int clientid    socket句柄
               uint8 logicId   逻辑地址
               uint16 infoNum  信息点号
               uint32 value    oad值
* @param[out]：
* @return    ：0-成功/错误码
**********************************************************************/
int appdb_get_seek_meter_list(int clientid, uint8 logicId, uint16 infoNum, uint32 value)
{
    int                 ret = 0;
    uint32              readlen = 0;
    uint32             i = 0;
    NOMAL_OAD_T         tOAD = {0};
    OOP_SEARCHMET_RES_T seekMeter;

    if(0x60020200 != value)
    {
        return -1;
    }

    tOAD.logicId   = logicId;
    tOAD.infoNum   = infoNum;
    tOAD.oad.value = value;

    MEMZERO(&seekMeter, sizeof(seekMeter));

    PRTL_FMT_LOGCC("appdb_get_seek_meter_list logicId(%d) infoNum(%d) value(0x%x) running.....\n", logicId, infoNum, value);

    if (infoNum == 0xffff)
    {
        seekMeterList.seekMeterNum = 0;
        for (i = 1; i <= NO_VIR_MTR_MAX_NUM; i++)
        {
            //判断是否有效
            if (!seekMeterList.isVaild[i])
            {
                continue;
            }

            if (app_set_seek_meter_invalid(i) < ERR_OK)
            {
                PRTL_FMT_LOGCC("app_set_seek_meter_invalid err\n");
            }
        }      
        return 0;
    }

    ret = db_read_nomal(clientid, &tOAD, sizeof(OOP_SEARCHMET_RES_T), (uint8 *)&seekMeter, &readlen);
    if(0 != ret || readlen != sizeof(OOP_SEARCHMET_RES_T))
    {
        app_set_seek_meter_invalid(infoNum);
        PRTL_FMT_DEBUG("db_read_nomal failed. ret(%d), infoNum(%d), readlen(%d), explen(%d)\n", ret, infoNum, readlen, sizeof(OOP_SEARCHMET_RES_T));
        return ERR_NORMAL;
    }

    PRTL_FMT_LOGCC("seekMeter.commAdd.len = %d\n", seekMeter.commAdd.len+1);
    PRTL_BUF_LOGCC(seekMeter.commAdd.add, seekMeter.commAdd.len+1, "seekMeter.commAdd:");
    PRTL_FMT_LOGCC("seekMeter.collAdd.len = %d\n", seekMeter.collAdd.len+1);
    PRTL_BUF_LOGCC(seekMeter.collAdd.add, seekMeter.collAdd.len+1, "seekMeter.collAdd:");
    PRTL_FMT_LOGCC("seekMeter.proto = %d\n", seekMeter.proto);
    PRTL_FMT_LOGCC("seekMeter.phase = %d\n", seekMeter.phase);
    PRTL_FMT_LOGCC("seekMeter.quality = %d\n", seekMeter.quality);
    PRTL_FMT_LOGCC("seekMeter.serchTime.year = %d\n", seekMeter.serchTime.year);
    PRTL_FMT_LOGCC("seekMeter.serchTime.month = %d\n", seekMeter.serchTime.month);
    PRTL_FMT_LOGCC("seekMeter.serchTime.day = %d\n", seekMeter.serchTime.day);
    PRTL_FMT_LOGCC("seekMeter.serchTime.hour = %d\n", seekMeter.serchTime.hour);
    PRTL_FMT_LOGCC("seekMeter.serchTime.minute = %d\n", seekMeter.serchTime.minute);
    PRTL_FMT_LOGCC("seekMeter.serchTime.second = %d\n", seekMeter.serchTime.second);
    PRTL_FMT_LOGCC("seekMeter.nAdded = %d\n", seekMeter.nAdded);
    
    ret = app_set_seek_meter_list(infoNum, &seekMeter);
    if (ret != ERR_OK)
    {
        app_set_seek_meter_invalid(infoNum);
        PRTL_FMT_DEBUG("appdb_get_seek_meter_list failed. ret(%d), infoNum(%d)\n", ret, infoNum);
        return ERR_NORMAL;
    }
    //creat_smlog_event(clientid, logicId, infoNum, value);
    CheckERC_3111(clientid);
    
    #if GET_NEW_METER_EN == 1
    get_new_meter_form_seek(clientid, seekMeter);
    #endif

    return 0;
}

/**********************************************************************
* @name      : app_get_seek_meter_count
* @brief     ：获取搜表结果个数
* @param[in] : 
* @param[out]：
* @return    ：结果个数
**********************************************************************/
uint16 app_get_seek_meter_count(void)
{
    uint16 ret;
    /* 使用互斥量进行加锁 */
    pthread_mutex_lock(&seekMeterList.mutex);

    ret = seekMeterList.seekMeterNum ;
    PRTL_FMT_LOGCC("app_get_seek_meter_count = %d\n", ret);

    /* 使用互斥量进行解锁 */
    pthread_mutex_unlock(&seekMeterList.mutex);

    return ret;
}

/**********************************************************************
* @name      : app_datainit_seek_meter
* @brief     ：收到数据初始化事件后清空搜表结果
* @param[in] : 
* @param[out]：
* @return    ：
**********************************************************************/
void app_datainit_seek_meter(uint8 logicid,uint16 classtag)
{
    PRTL_FMT_DEBUG("app_datainit_seek_meter logicid = %d classtag = %d\n", logicid,classtag);
    if(logicid==0&&(classtag==CLASS_DATA_NORMAL||classtag == 0))
    {
      /* 使用互斥量进行加锁 */
        pthread_mutex_lock(&seekMeterList.mutex);

        seekMeterList.seekMeterNum = 0 ;
        memset(seekMeterList.isChanged,0x00,sizeof(BOOLEAN)*(NO_VIR_MTR_MAX_NUM+1));
        memset(seekMeterList.isVaild,0x00,sizeof(BOOLEAN)*(NO_VIR_MTR_MAX_NUM+1));
        memset(seekMeterList.seekMeter,0x00,sizeof(OOP_SEARCHMET_RES_T)*(NO_VIR_MTR_MAX_NUM+1));
        memset(seekMeterList.isReport,0x00,sizeof(BOOLEAN)*(NO_VIR_MTR_MAX_NUM+1));
        /* 使用互斥量进行解锁 */
        pthread_mutex_unlock(&seekMeterList.mutex);
    }
  


}

#endif

#if DESC("跨台区搜表数据变更" ,1)

/**********************************************************************
* @name      : seek_across_list_lock_init
* @brief     ：
* @param[out]：
* @return    ：
* @Date      ：2020-1-10
* @Update    :
**********************************************************************/
void seek_across_list_lock_init(void)
{
    /* 初始化锁 */
    memset(&seekAcrossList, 0, sizeof(OOP_SEEK_ACROSS_LIST_T));
    pthread_mutex_init(&seekAcrossList.mutex, NULL);
}

/**
*********************************************************************
* @name：      appdb_seek_across_list_record_init
* @brief：     搜表结果变更初始化(记录数据方式)
* @param[in] ：clientid  数据库句柄
* @param[out]：
* @return    ：
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
void appdb_seek_across_list_record_init(int clientid)
{
    int                 ret = ERR_NORMAL;
    uint32              i = 0;
    uint32              seekAcrossNum = 0;
    OOP_ACROSSAREA_RES_T seekAcross;
    READ_RECORD_T       inReadRecord;
    uint32              handle = 0;
    uint32              recNum = 0;
    STEP_READ_RECORD_T  stepReadRecord;
    RECORD_UNIT_T       stepRecordOut;

    memset(&seekAcrossList, 0, sizeof(OOP_SEEK_ACROSS_LIST_T));
    memset(&inReadRecord, 0, sizeof(READ_RECORD_T));
    memset(&stepReadRecord, 0, sizeof(STEP_READ_RECORD_T));
    memset(&stepRecordOut, 0, sizeof(RECORD_UNIT_T));
    memset(&seekAcross, 0, sizeof(OOP_ACROSSAREA_RES_T));

    PRTL_FMT_DEBUG("Initializing seek acrosss. please wait......\n");
    inReadRecord.recordOAD.logicId = 0;
    inReadRecord.recordOAD.infoNum = 0xffff;
    inReadRecord.cType = COL_NO_INFO;
    inReadRecord.cStart = 1;
    inReadRecord.cEnd = NO_VIR_MTR_MAX_NUM + 1;
    inReadRecord.sortType = UP_SORT;
    inReadRecord.recordOAD.optional = 0;
    inReadRecord.recordOAD.road.oadMain.value = 0;
    inReadRecord.recordOAD.road.oadCols.nNum = 0x01;
    inReadRecord.recordOAD.road.oadCols.oad[0].value = 0x60020500;

    //不需要附加校验
    stepReadRecord.MAC.nNum = 0;

    ret = db_read_record_start(clientid, &inReadRecord, &handle, &recNum);
    if ((ret != ERR_OK) || (recNum == 0))
    {
        PRTL_FMT_DEBUG("appdb_seek_across_list_record_init db_read_record_start err! ret = %d, infoNum = %d recNum = %d\n", 
            ret, inReadRecord.recordOAD.infoNum, recNum);
    }
    else
    {
        PRTL_FMT_DEBUG("appdb_seek_across_list_record_init db_read_record_start ok ret = %d, infoNum = %d recNum = %d\n", 
            ret, inReadRecord.recordOAD.infoNum, recNum);
        for (i = 0; i < recNum; i++)
        {
            ret = db_read_record_step(clientid, handle, &stepReadRecord, &stepRecordOut);
            if (ret != ERR_OK)
            {
                PRTL_FMT_DEBUG("appdb_seek_across_list_record_init db_read_record_step err = %d!\n", ret);
                continue;
            }

            //检查数据有效性
            if ((stepRecordOut.recordnum != 1) 
            || (stepRecordOut.record[0].road.oadCols.oad[0].value != 0x60020500)
            || (stepRecordOut.record[0].outDatalen != sizeof(OOP_ACROSSAREA_RES_T)))
            {
                PRTL_FMT_DEBUG("appdb_seek_across_list_record_init data err !\n");
                PRTL_FMT_DEBUG("appdb_seek_across_list_record_init stepRecordOut.recordnum = %d\n", stepRecordOut.recordnum);
                PRTL_FMT_DEBUG("appdb_seek_across_list_record_init stepRecordOut.record[0].road.oadCols.oad[0] = 0x%08x\n", 
                    stepRecordOut.record[0].road.oadCols.oad[0].value);
                PRTL_FMT_DEBUG("appdb_seek_across_list_record_init stepRecordOut.record[0].outDatalen = %d\n", 
                    stepRecordOut.record[0].outDatalen);
                PRTL_FMT_DEBUG("appdb_seek_across_list_record_init sizeof(OOP_ACROSSAREA_RES_T) = %d\n", 
                    sizeof(OOP_ACROSSAREA_RES_T));
                continue;
            }

            memcpy(&seekAcross, stepRecordOut.record[0].outDataBuf, stepRecordOut.record[0].outDatalen);
            PRTL_FMT_DEBUG("stepRecordOut.infoNum = %d\n", stepRecordOut.infoNum);
            PRTL_FMT_DEBUG("seekAcross.commAdd.len = %d\n", seekAcross.commAdd.len);
            PRTL_BUF_DEBUG(seekAcross.commAdd.add, seekAcross.commAdd.len, "seekAcross.commAdd.add:");
            PRTL_FMT_DEBUG("seekAcross.netAdd.len = %d\n", seekAcross.netAdd.len);
            PRTL_BUF_DEBUG(seekAcross.netAdd.add, seekAcross.netAdd.len, "seekAcross.netAdd.add:");
            PRTL_FMT_DEBUG("seekAcross.change.year = %d\n", seekAcross.change.year);
            PRTL_FMT_DEBUG("seekAcross.change.month = %d\n", seekAcross.change.month);
            PRTL_FMT_DEBUG("seekAcross.change.day = %d\n", seekAcross.change.day);
            PRTL_FMT_DEBUG("seekAcross.change.hour = %d\n", seekAcross.change.hour);
            PRTL_FMT_DEBUG("seekAcross.change.minute = %d\n", seekAcross.change.minute);
            PRTL_FMT_DEBUG("seekAcross.change.second = %d\n", seekAcross.change.second);

            seekAcrossNum++;

            //序号从1开始
            seekAcrossList.isChanged[stepRecordOut.infoNum] = TRUE;
            seekAcrossList.isVaild[stepRecordOut.infoNum] = TRUE;
            seekAcrossList.seekAcross[stepRecordOut.infoNum] = seekAcross;
            
        }
    }
    db_read_record_end(clientid, handle);
    
    PRTL_FMT_DEBUG("Initialize tasks end.\n");

    /* 记录任务数量 */
    seekAcrossList.seekAcrossNum = seekAcrossNum;

    PRTL_FMT_DEBUG("read task num: %d\n", seekAcrossList.seekAcrossNum);

    return;
}

/**********************************************************************
* @name      : app_get_seek_across_list
* @brief     ：从缓存中获取搜表结果数据
* @param[in] ：infoNum   - 序号          
* @param[out]: seekAcross - 数据
* @return    : 错误码
**********************************************************************/
int app_get_seek_across_list(uint16 infoNum, OOP_ACROSSAREA_RES_T *seekAcross)
{
    int32 ret = ERR_NORMAL;

    if(NULL == seekAcross)
    {
        return ERR_PNULL;
    }

    /* 使用互斥量进行加锁 */
    pthread_mutex_lock(&seekAcrossList.mutex);

    *seekAcross = seekAcrossList.seekAcross[infoNum];
    seekAcrossList.isChanged[infoNum] = FALSE;

    /* 使用互斥量进行解锁 */
    pthread_mutex_unlock(&seekAcrossList.mutex);

    if (seekAcrossList.isVaild[infoNum])
    {
        ret = ERR_OK;
    }
    else
    {
        ret = ERR_INVALID;
    }

    return ret;
}

/**********************************************************************
* @name      : app_set_seek_across_list
* @brief     ：将更新的搜表数据写入缓存
* @param[in] ：infoNum   - 任务序号          
               seekAcross - 任务数据
* @return    ：错误码
**********************************************************************/
int app_set_seek_across_list(uint16 infoNum, OOP_ACROSSAREA_RES_T *seekAcross)
{
    if(NULL == seekAcross)
    {
        return ERR_PNULL;
    }

    /* 使用互斥量进行加锁 */
    pthread_mutex_lock(&seekAcrossList.mutex);

    seekAcrossList.seekAcross[infoNum] = *seekAcross;
    seekAcrossList.isVaild[infoNum] = TRUE;
    seekAcrossList.isChanged[infoNum] = TRUE;
    seekAcrossList.seekAcrossNum ++;

    /* 使用互斥量进行解锁 */
    pthread_mutex_unlock(&seekAcrossList.mutex);

    return ERR_OK;
}

/**********************************************************************
* @name      : app_set_seek_across_invalid
* @brief     ：设置搜表数据无效
* @param[in] : infoNum - 序号
* @param[out]：
* @return    ：错误码
**********************************************************************/
int app_set_seek_across_invalid(uint16 infoNum)
{

    /* 使用互斥量进行加锁 */
    pthread_mutex_lock(&seekAcrossList.mutex);

    memset(&seekAcrossList.seekAcross[infoNum], 0, sizeof(OOP_ACROSSAREA_RES_T));
    seekAcrossList.isVaild[infoNum] = FALSE;
    seekAcrossList.isChanged[infoNum] = TRUE;
    seekAcrossList.seekAcrossNum --;

    /* 使用互斥量进行解锁 */
    pthread_mutex_unlock(&seekAcrossList.mutex);

    return ERR_OK;
}
extern void CheckERC_3112(int fd,OOP_ACROSSAREA_RES_T info);

/**********************************************************************
* @name      : appdb_get_seek_across_list
* @brief     ：搜表数据列表到缓存
* @param[in] ：int clientid    socket句柄
               uint8 logicId   逻辑地址
               uint16 infoNum  信息点号
               uint32 value    oad值
* @param[out]：
* @return    ：0-成功/错误码
**********************************************************************/
int appdb_get_seek_across_list(int clientid, uint8 logicId, uint16 infoNum, uint32 value)
{
    int                 ret = 0;
    uint32              readlen = 0;
    uint32              i = 0;
    NOMAL_OAD_T         tOAD = {0};
    OOP_ACROSSAREA_RES_T seekAcross;

    if(0x60020500 != value)
    {
        return -1;
    }

    tOAD.logicId   = logicId;
    tOAD.infoNum   = infoNum;
    tOAD.oad.value = value;

    MEMZERO(&seekAcross, sizeof(seekAcross));

    PRTL_FMT_LOGCC("appdb_get_seek_across_list logicId(%d) infoNum(%d) value(0x%x) running.....\n", logicId, infoNum, value);

    if (infoNum == 0xffff)
    {
        seekAcrossList.seekAcrossNum = 0;
        for (i = 1; i <= NO_VIR_MTR_MAX_NUM; i++)
        {
            //判断是否有效
            if (!seekAcrossList.isVaild[i])
            {
                continue;
            }

            if (app_set_seek_across_invalid(i) < ERR_OK)
            {
                PRTL_FMT_LOGCC("app_set_seek_across_invalid err\n");
            }
        }      
        return 0;
    }

    ret = db_read_nomal(clientid, &tOAD, sizeof(OOP_ACROSSAREA_RES_T), (uint8 *)&seekAcross, &readlen);
    if(0 != ret || readlen != sizeof(OOP_ACROSSAREA_RES_T))
    {
        app_set_seek_across_invalid(infoNum);
        PRTL_FMT_DEBUG("db_read_nomal failed. ret(%d), infoNum(%d), readlen(%d), explen(%d)\n", ret, infoNum, readlen, sizeof(OOP_ACROSSAREA_RES_T));
        return ERR_NORMAL;
    }

    PRTL_FMT_LOGCC("seekAcross.commAdd.len = %d\n", seekAcross.commAdd.len);
    PRTL_BUF_LOGCC(seekAcross.commAdd.add, seekAcross.commAdd.len, "seekAcross.commAdd:");
    PRTL_FMT_LOGCC("seekAcross.netAdd.len = %d\n", seekAcross.netAdd.len);
    PRTL_BUF_LOGCC(seekAcross.netAdd.add, seekAcross.netAdd.len, "seekAcross.netAdd:");
    PRTL_FMT_LOGCC("seekAcross.change.year = %d\n", seekAcross.change.year);
    PRTL_FMT_LOGCC("seekAcross.change.month = %d\n", seekAcross.change.month);
    PRTL_FMT_LOGCC("seekAcross.change.day = %d\n", seekAcross.change.day);
    PRTL_FMT_LOGCC("seekAcross.change.hour = %d\n", seekAcross.change.hour);
    PRTL_FMT_LOGCC("seekAcross.change.minute = %d\n", seekAcross.change.minute);
    PRTL_FMT_LOGCC("seekAcross.change.second = %d\n", seekAcross.change.second);
    
    ret = app_set_seek_across_list(infoNum, &seekAcross);
    if (ret != ERR_OK)
    {
        app_set_seek_across_invalid(infoNum);
        PRTL_FMT_DEBUG("appdb_get_seek_across_list failed. ret(%d), infoNum(%d)\n", ret, infoNum);
        return ERR_NORMAL;
    }
    CheckERC_3112(clientid,seekAcross);
    //creat_unmeterlog_event(clientid, logicId, infoNum, value);
    return 0;
}

/**********************************************************************
* @name      : app_get_seek_across_count
* @brief     ：获取跨台区搜表结果个数
* @param[in] ：
* @return    个数
**********************************************************************/
uint16 app_get_seek_across_count(void)
{
    uint16 ret;

    /* 使用互斥量进行加锁 */
    pthread_mutex_lock(&seekAcrossList.mutex);
    ret = seekAcrossList.seekAcrossNum ;

    /* 使用互斥量进行解锁 */
    pthread_mutex_unlock(&seekAcrossList.mutex);

    return ret;
}

#endif

#if DESC("拓扑关系识别" , 1)
#ifdef AREA_ZHEJIANG
/**********************************************************************
* @name      : cbir_list_lock_init
* @brief     ：
* @param[out]：
* @return    ：
* @Date      ：
* @Update    :
**********************************************************************/
void cbir_list_lock_init(void)
{
    /* 初始化锁 */
    memset(&cbirResultList, 0, sizeof(CBIR_RESULT_LIST_T));
    pthread_mutex_init(&cbirResultList.mutex, NULL);
}

/**
*********************************************************************
* @name：      appdb_cbir_list_record_init
* @brief：     拓扑关系识别结果变更初始化(记录数据方式)
* @param[in] ：clientid  数据库句柄
* @param[out]：
* @return    ：
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
void appdb_cbir_list_record_init(int clientid)
{
    int                             ret = ERR_NORMAL;
    uint32                          i = 0;
    uint32                          cbirResultNum = 0;
    Circuit_Breaker_Identify_Result cbirResult;
    READ_RECORD_T                   inReadRecord;
    uint32                          handle = 0;
    uint32                          recNum = 0;
    STEP_READ_RECORD_T              stepReadRecord;
    RECORD_UNIT_T                   stepRecordOut;

    memset(&cbirResultList, 0, sizeof(CBIR_RESULT_LIST_T));
    memset(&inReadRecord, 0, sizeof(READ_RECORD_T));
    memset(&stepReadRecord, 0, sizeof(STEP_READ_RECORD_T));
    memset(&stepRecordOut, 0, sizeof(RECORD_UNIT_T));
    memset(&cbirResult, 0, sizeof(Circuit_Breaker_Identify_Result));

    PRTL_FMT_DEBUG("Initializing cbir result. please wait......\n");
    inReadRecord.recordOAD.logicId = 0;
    inReadRecord.recordOAD.infoNum = 0xffff;
    inReadRecord.cType = COL_NO_INFO;
    inReadRecord.cStart = 1;
    inReadRecord.cEnd = CBIR_MAX_NUM + 1;
    inReadRecord.sortType = UP_SORT;
    inReadRecord.recordOAD.optional = 0;
    inReadRecord.recordOAD.road.oadMain.value = 0;
    inReadRecord.recordOAD.road.oadCols.nNum = 0x01;
    inReadRecord.recordOAD.road.oadCols.oad[0].value = 0x4E080200;

    //不需要附加校验
    stepReadRecord.MAC.nNum = 0;

    ret = db_read_record_start(clientid, &inReadRecord, &handle, &recNum);
    if ((ret != ERR_OK) || (recNum == 0))
    {
        PRTL_FMT_DEBUG("appdb_seek_meter_list_record_init db_read_record_start err! ret = %d, infoNum = %d recNum = %d\n", 
            ret, inReadRecord.recordOAD.infoNum, recNum);
    }
    else
    {
        PRTL_FMT_DEBUG("appdb_seek_meter_list_record_init db_read_record_start ok ret = %d, infoNum = %d recNum = %d\n", 
            ret, inReadRecord.recordOAD.infoNum, recNum);
        for (i = 0; i < recNum; i++)
        {
            ret = db_read_record_step(clientid, handle, &stepReadRecord, &stepRecordOut);
            if (ret != ERR_OK)
            {
                PRTL_FMT_DEBUG("appdb_seek_meter_list_record_init db_read_record_step err = %d!\n", ret);
                continue;
            }

            //检查数据有效性
            if ((stepRecordOut.recordnum != 1) 
            || (stepRecordOut.record[0].road.oadCols.oad[0].value != 0x60020200)
            || (stepRecordOut.record[0].outDatalen != sizeof(OOP_SEARCHMET_RES_T)))
            {
                PRTL_FMT_DEBUG("appdb_seek_meter_list_record_init data err !\n");
                PRTL_FMT_DEBUG("appdb_seek_meter_list_record_init stepRecordOut.recordnum = %d\n", stepRecordOut.recordnum);
                PRTL_FMT_DEBUG("appdb_seek_meter_list_record_init stepRecordOut.record[0].road.oadCols.oad[0] = 0x%08x\n", 
                    stepRecordOut.record[0].road.oadCols.oad[0].value);
                PRTL_FMT_DEBUG("appdb_seek_meter_list_record_init stepRecordOut.record[0].outDatalen = %d\n", 
                    stepRecordOut.record[0].outDatalen);
                PRTL_FMT_DEBUG("appdb_seek_meter_list_record_init sizeof(OOP_SEARCHMET_RES_T) = %d\n", 
                    sizeof(OOP_SEARCHMET_RES_T));
                continue;
            }

            memcpy(&cbirResult, stepRecordOut.record[0].outDataBuf, stepRecordOut.record[0].outDatalen);
            PRTL_FMT_DEBUG("stepRecordOut.infoNum = %d\n", stepRecordOut.infoNum);
            PRTL_FMT_DEBUG("cbirResult.signalphase = %d\n", cbirResult.signalphase);
            PRTL_FMT_DEBUG("cbirResult.serchTime.year = %d\n", cbirResult.Suctime.year);
            PRTL_FMT_DEBUG("cbirResult.serchTime.month = %d\n", cbirResult.Suctime.month);
            PRTL_FMT_DEBUG("cbirResult.serchTime.day = %d\n", cbirResult.Suctime.day);
            PRTL_FMT_DEBUG("cbirResult.serchTime.hour = %d\n", cbirResult.Suctime.hour);
            PRTL_FMT_DEBUG("cbirResult.serchTime.minute = %d\n", cbirResult.Suctime.minute);
            PRTL_FMT_DEBUG("cbirResult.serchTime.second = %d\n", cbirResult.Suctime.second);

            cbirResultNum++;

            //序号从1开始
            cbirResultList.isChanged[stepRecordOut.infoNum] = TRUE;
            cbirResultList.isVaild[stepRecordOut.infoNum] = TRUE;
            cbirResultList.cbirResult[stepRecordOut.infoNum] = cbirResult;
            cbirResultList.isReport[stepRecordOut.infoNum] = TRUE; 
        }
    }
    db_read_record_end(clientid, handle);
    
    PRTL_FMT_DEBUG("Initialize tasks end.\n");

    /* 记录任务数量 */
    cbirResultList.cbirResultNum = cbirResultNum;

    PRTL_FMT_DEBUG("read cbirResult num: %d\n", cbirResultList.cbirResultNum);

    return;
}

/**********************************************************************
* @name      : app_get_cbir_result_list
* @brief     ：从缓存中获取搜表结果数据
* @param[in] ：infoNum    - 序号          
* @param[out]: cbirResult - 数据
* @return    : 错误码
**********************************************************************/
int app_get_cbir_result_list(uint16 infoNum, Circuit_Breaker_Identify_Result *cbirResult)
{
    int32 ret = ERR_NORMAL;

    if (NULL == cbirResult)
    {
        return ERR_PNULL;
    }

    /* 使用互斥量进行加锁 */
    pthread_mutex_lock(&cbirResultList.mutex);

    *cbirResult = cbirResultList.cbirResult[infoNum];
    cbirResultList.isChanged[infoNum] = FALSE;

    /* 使用互斥量进行解锁 */
    pthread_mutex_unlock(&cbirResultList.mutex);

    if (cbirResultList.isVaild[infoNum])
    {
        ret = ERR_OK;
    }
    else
    {
        ret = ERR_INVALID;
    }

    return ret;
}
/**********************************************************************
* @name      : app_check_cbir_isreport
* @brief     ：检查结果是否已上报过
* @param[in] ：infoNum   - 序号          
               
* @return    ：
**********************************************************************/
BOOLEAN app_check_cbir_isreport(uint16 infoNum)
{
    BOOLEAN ret = FALSE;
    pthread_mutex_lock(&cbirResultList.mutex);

    ret = cbirResultList.isReport[infoNum];

    /* 使用互斥量进行解锁 */
    pthread_mutex_unlock(&cbirResultList.mutex);
    return ret;
}

/**********************************************************************
* @name      : app_set_cbir_result_list
* @brief     ：将更新的数据写入缓存
* @param[in] ：infoNum    - 序号          
               cbirResult - 拓扑识别数据
* @return    ：错误码
**********************************************************************/
int app_set_cbir_result_list(uint16 infoNum, Circuit_Breaker_Identify_Result *cbirResult)
{
    if(NULL == cbirResult)
    {
        return ERR_PNULL;
    }

    /* 使用互斥量进行加锁 */
    pthread_mutex_lock(&cbirResultList.mutex);

    cbirResultList.cbirResult[infoNum] = *cbirResult;
    cbirResultList.isVaild[infoNum] = TRUE;
    cbirResultList.isChanged[infoNum] = TRUE;
    cbirResultList.isReport[infoNum] = FALSE;
    cbirResultList.cbirResultNum ++;
    PRTL_FMT_LOGCC("app_set_cbir_result_list cbirResultNum = %d\n", cbirResultList.cbirResultNum );

    /* 使用互斥量进行解锁 */
    pthread_mutex_unlock(&cbirResultList.mutex);

    return ERR_OK;
}

/**********************************************************************
* @name      : app_set_cbir_invalid
* @brief     ：设置拓扑识别数据无效
* @param[in] : infoNum - 序号
* @param[out]：
* @return    ：错误码
**********************************************************************/
int app_set_cbir_invalid(uint16 infoNum)
{

    /* 使用互斥量进行加锁 */
    pthread_mutex_lock(&cbirResultList.mutex);

    memset(&cbirResultList.cbirResult[infoNum], 0, sizeof(Circuit_Breaker_Identify_Result));
    cbirResultList.isVaild[infoNum] = FALSE;
    cbirResultList.isChanged[infoNum] = TRUE;
    if (cbirResultList.cbirResultNum > 0)
        cbirResultList.cbirResultNum --;

    /* 使用互斥量进行解锁 */
    pthread_mutex_unlock(&cbirResultList.mutex);

    return ERR_OK;
}

/**********************************************************************
* @name      : appdb_get_cbir_result_list
* @brief     ：搜表数据列表到缓存
* @param[in] ：int clientid    socket句柄
               uint8 logicId   逻辑地址
               uint16 infoNum  信息点号
               uint32 value    oad值
* @param[out]：
* @return    ：0-成功/错误码
**********************************************************************/
int appdb_get_cbir_result_list(int clientid, uint8 logicId, uint16 infoNum, uint32 value)
{
    int                 ret = 0;
    uint32              readlen = 0;
    uint32             i = 0;
    NOMAL_OAD_T         tOAD = {0};
    Circuit_Breaker_Identify_Result cbirResult;

    if(0x4E080200 != value)
    {
        return -1;
    }

    tOAD.logicId   = logicId;
    tOAD.infoNum   = infoNum;
    tOAD.oad.value = value;

    MEMZERO(&cbirResult, sizeof(cbirResult));

    PRTL_FMT_LOGCC("appdb_get_cbir_result_list logicId(%d) infoNum(%d) value(0x%x) running.....\n", logicId, infoNum, value);

    if (infoNum == 0xffff)
    {
        cbirResultList.cbirResultNum = 0;
        for (i = 1; i <= CBIR_MAX_NUM; i++)
        {
            //判断是否有效
            if (!cbirResultList.isVaild[i])
            {
                continue;
            }

            if (app_set_cbir_invalid(i) < ERR_OK)
            {
                PRTL_FMT_LOGCC("app_set_seek_meter_invalid err\n");
            }
        }      
        return 0;
    }

    ret = db_read_nomal(clientid, &tOAD, sizeof(Circuit_Breaker_Identify_Result), (uint8 *)&cbirResult, &readlen);
    if(0 != ret || readlen != sizeof(Circuit_Breaker_Identify_Result))
    {
        app_set_cbir_invalid(infoNum);
        PRTL_FMT_DEBUG("db_read_nomal failed. ret(%d), infoNum(%d), readlen(%d), explen(%d)\n", ret, infoNum, readlen, sizeof(Circuit_Breaker_Identify_Result));
        return ERR_NORMAL;
    }

    PRTL_FMT_DEBUG("cbirResult.signalphase = %d\n", cbirResult.signalphase);
    PRTL_FMT_DEBUG("cbirResult.serchTime.year = %d\n", cbirResult.Suctime.year);
    PRTL_FMT_DEBUG("cbirResult.serchTime.month = %d\n", cbirResult.Suctime.month);
    PRTL_FMT_DEBUG("cbirResult.serchTime.day = %d\n", cbirResult.Suctime.day);
    PRTL_FMT_DEBUG("cbirResult.serchTime.hour = %d\n", cbirResult.Suctime.hour);
    PRTL_FMT_DEBUG("cbirResult.serchTime.minute = %d\n", cbirResult.Suctime.minute);
    PRTL_FMT_DEBUG("cbirResult.serchTime.second = %d\n", cbirResult.Suctime.second);
    
    ret = app_set_cbir_result_list(infoNum, &cbirResult);
    if (ret != ERR_OK)
    {
        app_set_cbir_invalid(infoNum);
        PRTL_FMT_DEBUG("app_set_cbir_result_list failed. ret(%d), infoNum(%d)\n", ret, infoNum);
        return ERR_NORMAL;
    }

    return 0;
}

/**********************************************************************
* @name      : app_get_cbir_result_count
* @brief     ：获取拓扑识别关系个数
* @param[in] : 
* @param[out]：
* @return    ：结果个数
**********************************************************************/
uint16 app_get_cbir_result_count(void)
{
    uint16 ret;
    /* 使用互斥量进行加锁 */
    pthread_mutex_lock(&cbirResultList.mutex);

    ret = cbirResultList.cbirResultNum ;
    PRTL_FMT_LOGCC("app_get_cbir_result_count = %d\n", ret);

    /* 使用互斥量进行解锁 */
    pthread_mutex_unlock(&cbirResultList.mutex);

    return ret;
}

/**********************************************************************
* @name      : app_datainit_cbir_result
* @brief     ：收到数据初始化事件后清空拓扑识别关系
* @param[in] : 
* @param[out]：
* @return    ：
**********************************************************************/
void app_datainit_cbir_result(uint8 logicid, uint16 classtag)
{
    PRTL_FMT_DEBUG("app_datainit_cbir_result logicid = %d classtag = %d\n", logicid, classtag);
    if (logicid == 0 && (classtag == CLASS_DATA_NORMAL || classtag == 0))
    {
      /* 使用互斥量进行加锁 */
        pthread_mutex_lock(&cbirResultList.mutex);

        cbirResultList.cbirResultNum = 0 ;
        memset(cbirResultList.isChanged, 0x00, sizeof(BOOLEAN) * (CBIR_MAX_NUM+1));
        memset(cbirResultList.isVaild, 0x00, sizeof(BOOLEAN) * (CBIR_MAX_NUM+1));
        memset(cbirResultList.cbirResult, 0x00, sizeof(Circuit_Breaker_Identify_Result)*(CBIR_MAX_NUM + 1));
        memset(cbirResultList.isReport, 0x00, sizeof(BOOLEAN)*(CBIR_MAX_NUM + 1));
        /* 使用互斥量进行解锁 */
        pthread_mutex_unlock(&cbirResultList.mutex);
    }
}

#endif
#endif

#if DESC("精准校时数据变更" ,1)
/**********************************************************************
* @name      : sync_clock_param_lock_init
* @brief     ：
* @param[out]：
* @return    ：
* @Date      ：2020-1-10
* @Update    :
**********************************************************************/
void sync_clock_param_lock_init(void)
{
    /* 初始化锁 */
    memset(&syncClockParam, 0, sizeof(OOP_SYNCCLOCK_TIMING_T));
    pthread_mutex_init(&syncClockParam.mutex, NULL);
}

/**
*********************************************************************
* @brief:       获取校时模式
* @param[in]：   NA
* @param[out]： 
* @return：      
*********************************************************************
*/
int app_timingmode_read(int clientid, uint8 logicId, uint16 infoNum, uint32 value)
{
    int   ret = -1;
    uint8 timingmode;
    uint32 len=0;
    if(value!=0x40000300)
    {
        return ret;
    }
    NOMAL_OAD_T NormalOad;
    NormalOad.logicId = 0;
    NormalOad.oad.value = 0x40000300;
    NormalOad.infoNum = 0;
    ret = db_read_nomal(clientid,&NormalOad, 1, &timingmode, &len);
    if(ret!=0 || len !=1)
    {
        PRTL_FMT_DEBUG("read timing mode failed ret %d \n", ret);
        return -1;
    }
    app_set_timingmode(timingmode);
    return ret;
}
/**
*********************************************************************
* @brief:       获取心跳校时参数
* @param[in]：   NA
* @param[out]： syncclock 校时参数
* @return：      -1 未设置 >=0 获取到
*********************************************************************
*/
int app_syncclock_read(int clientid, uint8 logicId, uint16 infoNum, uint32 value)
{
    int   ret=-1;
    uint32 len=0;
    if(value!=0x40000400)
    {
        return ret;
    }
    NOMAL_OAD_T NormalOad;
    NormalOad.logicId = 0;
    NormalOad.oad.value = 0x40000400;
    NormalOad.infoNum = 0;
    OOP_SYNCCLOCK_T syncclock;
    MEMZERO(&syncclock, sizeof(OOP_SYNCCLOCK_T));
    ret = db_read_nomal(clientid,&NormalOad, sizeof(OOP_SYNCCLOCK_T), (uint8*)&syncclock, &len);
    if(ret!=0 || len !=sizeof(OOP_SYNCCLOCK_T))
    {
        PRTL_FMT_DEBUG("read syncclock failed ret %d \n", ret);
        return -1;
    }
    app_set_syncparam(&syncclock,len);
    return ret;
}
/**
*********************************************************************
* @name：      app_set_timingmode
* @brief：     设置校时模式
* @param[in] timingmode  校时模式
* @param[out]：
* @return    ：
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
void app_set_timingmode(uint8 timingmode)
{

    pthread_mutex_lock(&syncClockParam.mutex);
    syncClockParam.timingModeValid = 1 ;
    syncClockParam.timingMode = timingmode;
    pthread_mutex_unlock(&syncClockParam.mutex);
}
/**
*********************************************************************
* @name：      app_get_timingmode
* @brief：     获取校时模式
* @param[in] ：
* @param[out]：
* @return    ：-1表示未设置 否则返回校时模式
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
int app_get_timingmode(void)
{
    int ret = -1;
    pthread_mutex_lock(&syncClockParam.mutex);
    if(syncClockParam.timingModeValid == 1)
        ret = syncClockParam.timingMode;
    pthread_mutex_unlock(&syncClockParam.mutex);
    return ret;
}
/**
*********************************************************************
* @name：      app_set_syncparam
* @brief：     设置心跳校时参数
* @param[in] ：
* @param[out]：
* @return    ：
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
void app_set_syncparam(OOP_SYNCCLOCK_T *syncclock,uint16  len)
{
    if(len!=sizeof(OOP_SYNCCLOCK_T))
        return;
    pthread_mutex_lock(&syncClockParam.mutex);
    syncClockParam.syncClockValid = 1;
    memcpy(&syncClockParam.syncClock,syncclock,len);
    pthread_mutex_unlock(&syncClockParam.mutex);
}
/**
*********************************************************************
* @name：      app_get_syncparam
* @brief：     获取心跳校时参数
* @param[in] ：
* @param[out]：
* @return    ：-1表示未设置 0 正确
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
int app_get_syncparam(OOP_SYNCCLOCK_T *syncclock)
{
    int ret = -1;
    pthread_mutex_lock(&syncClockParam.mutex);
    if(syncClockParam.syncClockValid == 1)
    {
        memcpy(syncclock,&syncClockParam.syncClock,sizeof(OOP_SYNCCLOCK_T));
        ret = 0;
    }
    pthread_mutex_unlock(&syncClockParam.mutex);
    return ret;
}

/**********************************************************************
* @name      : sync_clock_param_lock_init
* @brief     ：
* @param[out]：
* @return    ：
* @Date      ：2020-1-10
* @Update    :
**********************************************************************/
void sync_cycle_param_lock_init(void)
{
    /* 初始化锁 */
    memset(&syncCycleParam, 0, sizeof(OOP_SYNC_CYCLE_T));
    pthread_mutex_init(&syncCycleParam.mutex, NULL);
}

/**********************************************************************
* @name      : search_param_init
* @brief     ：
* @param[out]：
* @return    ：
* @Date      ：2020-1-10
* @Update    :
**********************************************************************/
int sync_cycle_param_init(int clientid)
{
    int    ret = -1;
    uint32 len = 0;

    PRTL_FMT_DEBUG("Initializing sync_cycle_param_init. please wait......\n");

    NOMAL_OAD_T NormalOad;
    NormalOad.logicId = 0;
    NormalOad.oad.value = 0x40000600;
    NormalOad.infoNum = 0;
    OOP_SYNCCYCLE_T synccycle;

    MEMZERO(&synccycle, sizeof(OOP_SYNCCYCLE_T));
    ret = db_read_nomal(clientid, &NormalOad, sizeof(OOP_SYNCCYCLE_T), (uint8*)&synccycle, &len);
    if (ret != 0 || len != sizeof(OOP_SYNCCYCLE_T))
    {
        PRTL_FMT_DEBUG("read sync_cycle_param failed ret %d \n", ret);
        return -1;
    }

    PRTL_FMT_LOGCC("sync_cycle_param_init : synccycle.norperiod = %d\n", synccycle.norperiod);
    PRTL_FMT_LOGCC("sync_cycle_param_init : synccycle.souperiod = %d\n", synccycle.souperiod);

    app_set_synccycle_param(&synccycle, len);
    return ret;
}

/**
*********************************************************************
* @name：      app_set_synccycle_param
* @brief：     设置时间同步周期
* @param[in] ：
* @param[out]：
* @return    ：
* @author    : 
* @Date      ：
*********************************************************************
*/
void app_set_synccycle_param(OOP_SYNCCYCLE_T *synccycle,uint16  len)
{
    if(len!=sizeof(OOP_SYNCCYCLE_T))
        return;
    pthread_mutex_lock(&syncCycleParam.mutex);
    syncCycleParam.isValid = 1;
    memcpy(&syncCycleParam.synccycle,synccycle,len);
    pthread_mutex_unlock(&syncCycleParam.mutex);
}
/**
*********************************************************************
* @name：      app_get_syncparam
* @brief：     获取心跳校时参数
* @param[in] ：
* @param[out]：
* @return    ：-1表示未设置 0 正确
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
int app_get_synccycle_param(OOP_SYNCCYCLE_T *synccycle)
{
    int ret = -1;
    pthread_mutex_lock(&syncCycleParam.mutex);
    if(syncCycleParam.isValid == 1)
    {
        memcpy(synccycle,&syncCycleParam.synccycle,sizeof(OOP_SYNCCYCLE_T));
        ret = 0;
    }
    pthread_mutex_unlock(&syncCycleParam.mutex);
    return ret;
}
/**
*********************************************************************
* @brief:       获取时间同步周期
* @param[in]：   NA
* @param[out]： 
* @return：      
*********************************************************************
*/
int app_time_sync_cycle_read(int clientid, uint8 logicId, uint16 infoNum, uint32 value)
{
    int   ret = -1;
    OOP_SYNCCYCLE_T synccycle;
    MEMZERO(&synccycle, sizeof(OOP_SYNCCYCLE_T));
    uint32 len = 0;

    PRTL_FMT_LOGCC("app_time_sync_cycle_read logicId(%d) infoNum(%d) value(0x%x) running.....\n", logicId, infoNum, 
value);

    if (value != 0x40000600)
    {
        return ret;
    }

    NOMAL_OAD_T NormalOad;
    NormalOad.logicId = 0;
    NormalOad.oad.value = 0x40000600;
    NormalOad.infoNum = 0;
    ret = db_read_nomal(clientid,&NormalOad, sizeof(OOP_SYNCCYCLE_T), (uint8*)&synccycle, &len);
    if(ret!=0 || len !=sizeof(OOP_SYNCCYCLE_T))
    {
        PRTL_FMT_DEBUG("read 0x40000600 failed ret %d \n", ret);
        return -1;
    }
    app_set_synccycle_param(&synccycle,len);
    return ret;
}


#endif

#if DESC("更新esam密钥下发状态", 1)
/**********************************************************************
* @name      : esam_state_lock_init
* @brief     ：
* @param[out]：
* @return    ：
* @Date      ：2020-1-10
* @Update    :
**********************************************************************/
void esam_state_lock_init(void)
{
    /* 初始化锁 */
    memset(&esamState, 0, sizeof(OOP_ESAM_STATE_T));
    pthread_mutex_init(&esamState.mutex, NULL);
}

/**********************************************************************
* @name      : unset_esam_state
* @brief     ：清除esam下发状态
* @param[in] ：isEsamSet 0 未下发密钥 1 已下发密钥 
* @param[out]：NULL
* @return    ：
**********************************************************************/
void set_esam_state(uint8 isEsamSet)
{
    /* 使用互斥量进行加锁 */
    pthread_mutex_lock(&esamState.mutex);

    esamState.isEsamSet = isEsamSet;

    /* 使用互斥量进行解锁 */
    pthread_mutex_unlock(&esamState.mutex);

    return;
}

/**********************************************************************
* @name      : get_esam_state
* @brief     ：获取esam下发状态
* @param[in] ：
* @param[out]：
* @return    ：state  0 未下发密钥 1 已下发密钥
**********************************************************************/
uint8 get_esam_state(void)
{
    uint8 esamSet = 0;

    /* 使用互斥量进行加锁 */
    pthread_mutex_lock(&esamState.mutex);

    esamSet = esamState.isEsamSet;

    /* 使用互斥量进行解锁 */
    pthread_mutex_unlock(&esamState.mutex);

    return esamSet;
}

/**********************************************************************
* @name      : set_esam_update_state
* @brief     ：清除esam状态更新
* @param[in] ：isEsamUpdate  0 未更新 1 更新
* @param[out]：NULL
* @return    ：
**********************************************************************/
void set_esam_update_state(uint8 isEsamUpdate)
{
    /* 使用互斥量进行加锁 */
    pthread_mutex_lock(&esamState.mutex);

    esamState.isEsamUpdate = isEsamUpdate;

    /* 使用互斥量进行解锁 */
    pthread_mutex_unlock(&esamState.mutex);

    return;
}

/**********************************************************************
* @name      : get_esam_update_state
* @brief     ：获取esam状态更新
* @param[in] ：
* @param[out]：
* @return    ：state 0 未更新 1 更新
**********************************************************************/
uint8 get_esam_update_state(void)
{
    uint8 isEsamUpdate = 0;

    /* 使用互斥量进行加锁 */
    pthread_mutex_lock(&esamState.mutex);

    isEsamUpdate = esamState.isEsamUpdate;

    esamState.isEsamUpdate = 0;

    /* 使用互斥量进行解锁 */
    pthread_mutex_unlock(&esamState.mutex);

    return isEsamUpdate;
}

#endif

#if DESC("搜表参数(0x60020800)变更" ,1)
/**********************************************************************
* @name      : sync_clock_param_lock_init
* @brief     ：
* @param[out]：
* @return    ：
* @Date      ：2020-1-10
* @Update    :
**********************************************************************/
void sync_search_param_lock_init(void)
{
    /* 初始化锁 */
    memset(&oopSearchParam, 0, sizeof(OOP_SEARCHPARAM_G_T));
    pthread_mutex_init(&oopSearchParam.mutex, NULL);
}

/**
*********************************************************************
* @name：      app_set_syncparam
* @brief：     设置搜表参数
* @param[in] ：
* @param[out]：
* @return    ：
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
void app_set_search_param(OOP_SEARCHPARAM_T *searchParm, uint16  len)
{
    if(len != sizeof(OOP_SEARCHPARAM_T))
        return;

    pthread_mutex_lock(&oopSearchParam.mutex);

    oopSearchParam.searchParamValid = 1;
    memcpy(&oopSearchParam.searchParam, searchParm, len);

    pthread_mutex_unlock(&oopSearchParam.mutex);

    return;
}

/**
*********************************************************************
* @name：      app_get_search_param
* @brief：     获取搜表参数
* @param[in] ：
* @param[out]：
* @return    ：-1表示未设置 0 正确
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
int app_get_search_param(OOP_SEARCHPARAM_T *searchParm)
{
    int ret = -1;
    pthread_mutex_lock(&oopSearchParam.mutex);
    if(oopSearchParam.searchParamValid == 1)
    {
        memcpy(searchParm, &oopSearchParam.searchParam, sizeof(OOP_SEARCHPARAM_T));
        ret = 0;
    }
    pthread_mutex_unlock(&oopSearchParam.mutex);
    return ret;
}

/**
*********************************************************************
* @brief:       获取搜表参数
* @param[in]：   NA
* @param[out]： 
* @return：      -1 未设置 >=0 获取到
*********************************************************************
*/
int app_search_param_read(int clientid, uint8 logicId, uint16 infoNum, uint32 value)
{
    int    ret = -1;
    uint32 len = 0;

    if (value != 0x60020800)
    {
        return ret;
    }

    PRTL_FMT_LOGCC("app_search_param_read logicId(%d) infoNum(%d) value(0x%x) running.....\n", logicId, infoNum, value);

    NOMAL_OAD_T NormalOad;
    NormalOad.logicId = 0;
    NormalOad.oad.value = 0x60020800;
    NormalOad.infoNum = 0;
    OOP_SEARCHPARAM_T searchParam;
    MEMZERO(&searchParam, sizeof(OOP_SEARCHPARAM_T));
    ret = db_read_nomal(clientid, &NormalOad, sizeof(OOP_SEARCHPARAM_T), (uint8*)&searchParam, &len);
    if (ret != 0 || len != sizeof(OOP_SEARCHPARAM_T))
    {
        PRTL_FMT_DEBUG("read searchParam failed ret %d \n", ret);
        return -1;
    }

    PRTL_FMT_TRACE("app_search_param_read searchParam.atuoUpdate[%d]\n", searchParam.atuoUpdate);
    PRTL_FMT_TRACE("app_search_param_read searchParam.clearChoice[%d]\n", searchParam.clearChoice);
    PRTL_FMT_TRACE("app_search_param_read searchParam.periodSearch[%d]\n", searchParam.periodSearch);
    PRTL_FMT_TRACE("app_search_param_read searchParam.touchEvent[%d]\n", searchParam.touchEvent);

    app_set_search_param(&searchParam, len);
    return ret;
}

/**********************************************************************
* @name      : search_param_init
* @brief     ：
* @param[out]：
* @return    ：
* @Date      ：2020-1-10
* @Update    :
**********************************************************************/
int search_param_init(int clientid)
{
    int    ret = -1;
    uint32 len = 0;

    PRTL_FMT_DEBUG("Initializing search_param. please wait......\n");

    NOMAL_OAD_T NormalOad;
    NormalOad.logicId = 0;
    NormalOad.oad.value = 0x60020800;
    NormalOad.infoNum = 0;
    OOP_SEARCHPARAM_T searchParam;
    MEMZERO(&searchParam, sizeof(OOP_SEARCHPARAM_T));
    ret = db_read_nomal(clientid, &NormalOad, sizeof(OOP_SEARCHPARAM_T), (uint8*)&searchParam, &len);
    if (ret != 0 || len != sizeof(OOP_SEARCHPARAM_T))
    {
        PRTL_FMT_DEBUG("read searchParam failed ret %d \n", ret);
        return -1;
    }

    PRTL_FMT_TRACE("search_param_init searchParam.atuoUpdate[%d]\n", searchParam.atuoUpdate);
    PRTL_FMT_TRACE("search_param_init searchParam.clearChoice[%d]\n", searchParam.clearChoice);
    PRTL_FMT_TRACE("search_param_init searchParam.periodSearch[%d]\n", searchParam.periodSearch);
    PRTL_FMT_TRACE("search_param_init searchParam.touchEvent[%d]\n", searchParam.touchEvent);

    app_set_search_param(&searchParam, len);
    return ret;
}

#endif

#ifdef AREA_FUJIAN
#if DESC("群组档案信息变更", 0)
/**
*********************************************************************
* @brief：       根据信息点找到对应的索引
* @param[in]：   inform 信息点
* @param[out]： index  索引
* @return：      错误码
*********************************************************************
*/
int find_index_form_inform(uint16 inform, uint16 *index)
{
    uint16 i = 0;
    int32  ret = ERR_OK;

    /* 使用互斥量进行加锁 */
    pthread_mutex_lock(&groupMeterList.mutex);

    for (i = 0; i < GROUP_NUM; i++)
    {
        if (groupMeterList.inform[i] == inform)
        {
            *index = i;
            break;
        }
    }

    if (i == GROUP_NUM)
    {
        PRTL_FMT_DEBUG("没有找到对应的信息点的索引inform[%d]\n", inform);
        ret = ERR_ITEM;
    }

    /* 使用互斥量进行解锁 */
    pthread_mutex_unlock(&groupMeterList.mutex);
    
    return ret;
}

/**
*********************************************************************
* @brief：     判断表地址是否合法
* @param[in]： addr     地址
* @return：    TRUE - 合法
*********************************************************************
*/
BOOL meteraddr_is_legally(uint64 *addr)
{
    uint8 i = 0;
    uint8 add[OOP_TSA_SIZE] = { 0 };

    memcpy(add, addr, sizeof(uint64));

    for (i = 0; i < sizeof(uint64); i++)
    {
        if (add[i] > 0x99)
        {
            return FALSE;
        }
    }
    return TRUE;
}

/**
*********************************************************************
* @name：      group_meter_info_from_ms_choice6
* @brief：     群组档案2个结构体转换(记录数据方式)
* @param[in] ：clientid  数据库句柄
* @param[out]：
* @return    ：
* @author    : 
* @Date      ：2022-3-29
*********************************************************************
*/
int group_meter_info_from_ms_choice3(OOP_GROUPMETER_T *pGroupMeter, OOP_GROUP_METER_T *pGroupMs)
{
    uint32 i = 0;
    
    OOP_MSP_T *ms = &pGroupMs->ms;

    pGroupMeter->groupAddr = pGroupMs->groupAdd;

    if (ms->msa.size > OOP_MAX_MSPARAM)
    {
        PRTL_FMT_DEBUG("群组档案数量超限 ms->msa.size[%d] > %d", ms->msa.size, OOP_MAX_MSPARAM);
        return ERR_ADDR;
    }

    for (i = 0; i < ms->msa.size; i++)
    {
        pGroupMeter->addr[i] = ms->msa.add[i];
    }

    pGroupMeter->num = ms->msa.size;

    return ERR_OK;
}



/**
*********************************************************************
* @name：      group_meter_info_from_ms_choice6
* @brief：     群组档案2个结构体转换(记录数据方式)
* @param[in] ：clientid  数据库句柄
* @param[out]：
* @return    ：
* @author    : 
* @Date      ：2022-3-29
*********************************************************************
*/
int group_meter_info_from_ms_choice6(OOP_GROUPMETER_T *pGroupMeter, OOP_GROUP_METER_T *pGroupMs)
{
    uint32 i = 0;
    uint64 startAddr = 0;
    uint64 endAddr = 0;
    uint32 groupMeterIndex = 0;
    
    OOP_MSP_T *ms = &pGroupMs->ms;
    
    pGroupMeter->groupAddr = pGroupMs->groupAdd;

    for (i = 0; i < ms->msra.size; i++)
    {
        if (ms->msra.region[i].start.flag == 1)
        {
            memcpy_r(&startAddr, &ms->msra.region[i].start.add[0], ms->msra.region[i].start.len);
        }
        else
        {
            memcpy_r(&startAddr, &ms->msra.region[i].start.add[0], ms->msra.region[i].start.len + 1);
        }
        
        if (ms->msra.region[i].end.flag == 1)
        {
            memcpy_r(&endAddr, &ms->msra.region[i].end.add[0], ms->msra.region[i].end.len);
        }
        else
        {
            memcpy_r(&endAddr, &ms->msra.region[i].end.add[0], ms->msra.region[i].end.len + 1);
        }
                
        switch (ms->msra.region[i].nType)
        {
        case 0:
        {
            //前闭后开
            while (startAddr < endAddr)
            {
                //判断表地址是否合法
                if (!meteraddr_is_legally(&startAddr))
                {
                    continue;
                }

                if (groupMeterIndex >= GROUP_METER_NUM - 1)
                {
                    PRTL_FMT_DEBUG("群组档案数量超限 groupMeterIndex[%d] > %d", groupMeterIndex, GROUP_METER_NUM);
                    return ERR_ADDR;
                }
                                
                pGroupMeter->addr[groupMeterIndex] = ms->msra.region[i].start;
                if (ms->msra.region[i].start.flag == 1)
                {
                    memcpy_r(&pGroupMeter->addr[groupMeterIndex].add[0], &startAddr, ms->msra.region[i].start.len);
                }
                else
                {
                    memcpy_r(&pGroupMeter->addr[groupMeterIndex].add[0], &startAddr, ms->msra.region[i].start.len + 1);
                }
                
                groupMeterIndex ++;
                
                startAddr ++;
            }
            break;
        }
        case 1:
        {
            //前开后闭
            while (startAddr <= endAddr)
            {
                startAddr ++;
                
                //判断表地址是否合法
                if (!meteraddr_is_legally(&startAddr))
                {
                    continue;
                }

                if (groupMeterIndex >= GROUP_METER_NUM - 1)
                {
                    PRTL_FMT_DEBUG("群组档案数量超限 groupMeterIndex[%d] > %d", groupMeterIndex, GROUP_METER_NUM);
                    return ERR_ADDR;
                }
                                
                pGroupMeter->addr[groupMeterIndex] = ms->msra.region[i].start;
                if (ms->msra.region[i].start.flag == 1)
                {
                    memcpy_r(&pGroupMeter->addr[groupMeterIndex].add[0], &startAddr, ms->msra.region[i].start.len);
                }
                else
                {
                    memcpy_r(&pGroupMeter->addr[groupMeterIndex].add[0], &startAddr, ms->msra.region[i].start.len + 1);
                }
                groupMeterIndex ++;
            }
            break;
        }
        case 2:
        {
            //前闭后闭
            while (startAddr <= endAddr)
            {
                //判断表地址是否合法
                if (!meteraddr_is_legally(&startAddr))
                {
                    continue;
                }

                if (groupMeterIndex >= GROUP_METER_NUM - 1)
                {
                    PRTL_FMT_DEBUG("群组档案数量超限 groupMeterIndex[%d] > %d", groupMeterIndex, GROUP_METER_NUM);
                    return ERR_ADDR;
                }
                                
                pGroupMeter->addr[groupMeterIndex] = ms->msra.region[i].start;
                if (ms->msra.region[i].start.flag == 1)
                {
                    memcpy_r(&pGroupMeter->addr[groupMeterIndex].add[0], &startAddr, ms->msra.region[i].start.len);
                }
                else
                {
                    memcpy_r(&pGroupMeter->addr[groupMeterIndex].add[0], &startAddr, ms->msra.region[i].start.len + 1);
                }
                groupMeterIndex ++;

                startAddr ++;
            }
            break;
        }
        case 3:
        {
            //前开后开
            while (startAddr < endAddr)
            {
                startAddr ++;
                
                //判断表地址是否合法
                if (!meteraddr_is_legally(&startAddr))
                {
                    continue;
                }

                if (groupMeterIndex >= GROUP_METER_NUM - 1)
                {
                    PRTL_FMT_DEBUG("群组档案数量超限 groupMeterIndex[%d] > %d", groupMeterIndex, GROUP_METER_NUM);
                    return ERR_ADDR;
                }
                                
                pGroupMeter->addr[groupMeterIndex] = ms->msra.region[i].start;
                if (ms->msra.region[i].start.flag == 1)
                {
                    memcpy_r(&pGroupMeter->addr[groupMeterIndex].add[0], &startAddr, ms->msra.region[i].start.len);
                }
                else
                {
                    memcpy_r(&pGroupMeter->addr[groupMeterIndex].add[0], &startAddr, ms->msra.region[i].start.len + 1);
                }
                groupMeterIndex ++;
                
            }
            break;
        }
        default:
            break;
        }
    }

    pGroupMeter->num = groupMeterIndex;

    return ERR_OK;
}


/**
*********************************************************************
* @name：      group_meter_info_from_ms
* @brief：     群组档案2个结构体转换(记录数据方式)
* @param[in] ：clientid  数据库句柄
* @param[out]：
* @return    ：
* @author    : 
* @Date      ：2022-3-29
*********************************************************************
*/
int group_meter_info_from_ms(OOP_GROUPMETER_T *pGroupMeter, OOP_GROUP_METER_T *pGroupMs)
{
    switch (pGroupMs->ms.choice)
    {
    case 3:
    {
        return group_meter_info_from_ms_choice3(pGroupMeter, pGroupMs);
        break;
    }
    case 6:
    {
        return group_meter_info_from_ms_choice6(pGroupMeter, pGroupMs);
        break;
    }
    default:
        break;
    }

    return ERR_NORMAL;
}

/**********************************************************************
* @name      : groupmeter_list_lock_init
* @brief     ：
* @param[out]：
* @return    ：
* @Date      ：2022-3-29
* @Update    :
**********************************************************************/
void groupmeter_list_lock_init(void)
{
    /* 初始化锁 */
    memset(&groupMeterList, 0, sizeof(OOP_GROUPMETER_LIST_T));
    pthread_mutex_init(&groupMeterList.mutex, NULL);
}

/**
*********************************************************************
* @name：      appdb_groupmeter_list_record_init
* @brief：     群组档案变更初始化(记录数据方式)
* @param[in] ：clientid  数据库句柄
* @param[out]：
* @return    ：
* @author    : 
* @Date      ：2022-3-29
*********************************************************************
*/
void appdb_groupmeter_list_record_init(int clientid)
{
    int                   ret = ERR_NORMAL;
    uint32                i = 0;
    uint32                groupMeterNum = 0;

    //注意OOP_GROUP_METER_T和OOP_GROUPMETER_T为不同作用结构体
    OOP_GROUP_METER_T     groupMeter;
    OOP_GROUPMETER_T      groupMeterN;
    READ_RECORD_T         inReadRecord;
    uint32                handle = 0;
    uint32                recNum = 0;
    STEP_READ_RECORD_T    stepReadRecord;
    RECORD_UNIT_T         stepRecordOut;

    memset(&groupMeterList, 0, sizeof(OOP_GROUPMETER_LIST_T));
    memset(&inReadRecord, 0, sizeof(READ_RECORD_T));
    memset(&stepReadRecord, 0, sizeof(STEP_READ_RECORD_T));
    memset(&stepRecordOut, 0, sizeof(RECORD_UNIT_T));
    memset(&groupMeter, 0, sizeof(OOP_GROUP_METER_T));
    memset(&groupMeterN, 0, sizeof(OOP_GROUPMETER_T));

    PRTL_FMT_DEBUG("Initializing groupmeter. please wait......\n");
    inReadRecord.recordOAD.logicId = 0;
    inReadRecord.recordOAD.infoNum = 0xffff;
    inReadRecord.cType = COL_NO_INFO;
    inReadRecord.cStart = 1;
    inReadRecord.cEnd = 10000;
    inReadRecord.sortType = UP_SORT;
    inReadRecord.recordOAD.optional = 0;
    inReadRecord.recordOAD.road.oadMain.value = 0;
    inReadRecord.recordOAD.road.oadCols.nNum = 0x01;
    inReadRecord.recordOAD.road.oadCols.oad[0].value = 0x6e050200;

    //不需要附加校验
    stepReadRecord.MAC.nNum = 0;

    ret = db_read_record_start(clientid, &inReadRecord, &handle, &recNum);
    if ((ret != ERR_OK) || (recNum == 0) || (recNum > GROUP_NUM))
    {
        PRTL_FMT_DEBUG("appdb_groupmeter_list_record_init db_read_record_start err! ret = %d, infoNum = %d recNum = %d\n", 
            ret, inReadRecord.recordOAD.infoNum, recNum);
    }
    else
    {
        PRTL_FMT_DEBUG("appdb_groupmeter_list_record_init db_read_record_start ok ret = %d, infoNum = %d recNum = %d\n", 
            ret, inReadRecord.recordOAD.infoNum, recNum);
        for (i = 0; i < recNum; i++)
        {
            ret = db_read_record_step(clientid, handle, &stepReadRecord, &stepRecordOut);
            if (ret != ERR_OK)
            {
                PRTL_FMT_DEBUG("appdb_groupmeter_list_record_init db_read_record_step err = %d!\n", ret);
                continue;
            }

            //检查数据有效性
            if ((stepRecordOut.recordnum != 1) 
            || (stepRecordOut.record[0].road.oadCols.oad[0].value != 0x6e050200)
            || (stepRecordOut.record[0].outDatalen != sizeof(OOP_GROUP_METER_T)))
            {
                PRTL_FMT_DEBUG("appdb_groupmeter_list_record_init data err !\n");
                PRTL_FMT_DEBUG("appdb_groupmeter_list_record_init stepRecordOut.recordnum = %d\n", stepRecordOut.recordnum);
                PRTL_FMT_DEBUG("appdb_groupmeter_list_record_init stepRecordOut.record[0].road.oadCols.oad[0] = 0x%08x\n", 
                    stepRecordOut.record[0].road.oadCols.oad[0].value);
                PRTL_FMT_DEBUG("appdb_groupmeter_list_record_init stepRecordOut.record[0].outDatalen = %d\n", 
                    stepRecordOut.record[0].outDatalen);
                PRTL_FMT_DEBUG("appdb_groupmeter_list_record_init sizeof(OOP_GROUP_METER_T) = %d\n", 
                    sizeof(OOP_GROUP_METER_T));
                continue;
            }

            memcpy(&groupMeter, stepRecordOut.record[0].outDataBuf, stepRecordOut.record[0].outDatalen);
            PRTL_BUF_DEBUG(groupMeter.groupAdd.add, groupMeter.groupAdd.len + 1, "groupMeter.groupAdd.add");
            PRTL_FMT_DEBUG("groupMeter.ms.choice:%d\n", groupMeter.ms.choice);

            if (group_meter_info_from_ms(&groupMeterN, &groupMeter) != ERR_OK)
            {
                continue;
            }

            //序号从1开始
            groupMeterList.isVaild[groupMeterNum] = TRUE;
            groupMeterList.group[groupMeterNum] = groupMeterN;
            groupMeterList.inform[groupMeterNum] = stepRecordOut.infoNum;
            groupMeterNum ++;
        }
    }
    db_read_record_end(clientid, handle);
    
    PRTL_FMT_DEBUG("Initialize groupMeter end.\n");

    /* 记录任务数量 */
    groupMeterList.num = groupMeterNum;

    PRTL_FMT_DEBUG("read groupMeter num: %d\n", groupMeterList.num);

    return;
}

/**********************************************************************
* @name      : app_get_groupmeter_list
* @brief     ：从缓存中获取子节点信息
* @param[in] ：infoNum   - 序号          
* @param[out]: nodeversion - 数据
* @return    : 错误码
**********************************************************************/
int app_get_groupmeter_list(uint16 index, OOP_GROUPMETER_T *groupMeter)
{
    int32 ret = ERR_NORMAL;

    if(NULL == groupMeter)
    {
        return ERR_PNULL;
    }

    /* 使用互斥量进行加锁 */
    pthread_mutex_lock(&groupMeterList.mutex);

    *groupMeter = groupMeterList.group[index];

    /* 使用互斥量进行解锁 */
    pthread_mutex_unlock(&groupMeterList.mutex);

    if (groupMeterList.isVaild[index])
    {
        ret = ERR_OK;
    }
    else
    {
        ret = ERR_INVALID;
    }

    return ret;
}

/**********************************************************************
* @name      : app_set_groupmeter_list
* @brief     ：将更新的节点信息数据写入缓存
* @param[in] ：infoNum   - 任务序号          
               childNode - 任务数据
* @return    ：错误码
**********************************************************************/
int app_set_groupmeter_list(uint16 infoNum, OOP_GROUP_METER_T *groupMeter)
{
    OOP_GROUPMETER_T groupMeterN;
    uint16 index = 0;
    
    if(NULL == groupMeter)
    {
        return ERR_PNULL;
    }

    //根据信息点找到索引
    if (ERR_OK != find_index_form_inform(infoNum, &index))
    {
        //没有找到，说明是新的数据
        if (groupMeterList.num >= GROUP_NUM)
        {
            PRTL_FMT_DEBUG("群组档案达到最大值 groupMeterList.num[%d]", groupMeterList.num);
            return ERR_OPTION;
        }
        index = groupMeterList.num;
    }

    memset(&groupMeterN, 0, sizeof(groupMeterN));

    if (ERR_OK != group_meter_info_from_ms(&groupMeterN, groupMeter))
    {
        return ERR_OPTION;
    }

    /* 使用互斥量进行加锁 */
    pthread_mutex_lock(&groupMeterList.mutex);

    groupMeterList.group[index] = groupMeterN;
    groupMeterList.isVaild[index] = TRUE;

    if (index == groupMeterList.num)
    {
        groupMeterList.num ++;
    }

    /* 使用互斥量进行解锁 */
    pthread_mutex_unlock(&childNodeList.mutex);

    return ERR_OK;
}

/**********************************************************************
* @name      : app_set_groupmeter_invalid
* @brief     ：设置数据无效
* @param[in] : infoNum - 序号
* @param[out]：
* @return    ：错误码
**********************************************************************/
int app_set_groupmeter_invalid(uint16 index)
{

    /* 使用互斥量进行加锁 */
    pthread_mutex_lock(&groupMeterList.mutex);

    memset(&groupMeterList.group[index], 0, sizeof(OOP_GROUPMETER_T));
    groupMeterList.isVaild[index] = FALSE;

    if (groupMeterList.num > 0)
    {
        groupMeterList.num --;
    }

    /* 使用互斥量进行解锁 */
    pthread_mutex_unlock(&groupMeterList.mutex);

    return ERR_OK;
}

/**********************************************************************
* @name      : appdb_get_groupmeter_list
* @brief     ：群组档案数据列表到缓存
* @param[in] ：int clientid    socket句柄
               uint8 logicId   逻辑地址
               uint16 infoNum  信息点号
               uint32 value    oad值
* @param[out]：
* @return    ：0-成功/错误码
**********************************************************************/
int appdb_get_groupmeter_list(int clientid, uint8 logicId, uint16 infoNum, uint32 value)
{
    int                   ret = 0;
    uint32                readlen = 0;
    uint32                i = 0;
    uint16                index = 0;
    NOMAL_OAD_T           tOAD = {0};
    OOP_GROUP_METER_T     groupMeter;

    if ((0x6E0500 != value) || (infoNum > 10000))
    {
        return -1;
    }

    tOAD.logicId   = logicId;
    tOAD.infoNum   = infoNum;
    tOAD.oad.value = value;

    MEMZERO(&groupMeter, sizeof(groupMeter));

    PRTL_FMT_DEBUG("收到群组档案更新消息 logicId(%d) infoNum(%d) value(0x%08x)\n", logicId, infoNum, value);

    if (infoNum == 0xffff)
    {
        groupMeterList.num = 0;
        for (i = 0; i < GROUP_NUM; i++)
        {
            //判断是否有效
            if (!groupMeterList.isVaild[i])
            {
                continue;
            }

            if (app_set_groupmeter_invalid(i) < ERR_OK)
            {
                PRTL_FMT_LOGCC("app_set_groupmeter_invalid err\n");
            }
        }      
        return 0;
    }

    ret = db_read_nomal(clientid, &tOAD, sizeof(OOP_GROUP_METER_T), (uint8 *)&groupMeter, &readlen);
    if(0 != ret || readlen != sizeof(OOP_GROUP_METER_T))
    {
        if (ERR_OK == find_index_form_inform(infoNum, &index))
        {
            app_set_groupmeter_invalid(index);
        }
        PRTL_FMT_DEBUG("db_read_nomal failed. ret(%d), infoNum(%d), readlen(%d), explen(%d)\n", ret, infoNum, readlen, sizeof(OOP_CHILDNODE_T));
        return ERR_NORMAL;
    }

    PRTL_BUF_DEBUG(&groupMeter.groupAdd.add[0], groupMeter.groupAdd.len + 1, "groupMeter.addr:");
    PRTL_FMT_DEBUG("groupMeter.ms.choice:", groupMeter.ms.choice);
    
    ret = app_set_groupmeter_list(infoNum, &groupMeter);
    if (ret != ERR_OK)
    {
        if (ERR_OK == find_index_form_inform(infoNum, &index))
        {
            app_set_groupmeter_invalid(index);
        }
        PRTL_FMT_LOGCC("app_set_childnode_list failed. ret(%d), infoNum(%d)\n", ret, infoNum);
        return ERR_NORMAL;
    }

    return 0;
}

/**********************************************************************
* @name      : app_get_groupmeter_count
* @brief     ：获取群组档案信息结果个数
* @param[in] ：
* @return    个数
**********************************************************************/
uint16 app_get_groupmeter_count(void)
{
    uint16 ret;

    /* 使用互斥量进行加锁 */
    pthread_mutex_lock(&groupMeterList.mutex);
    
    ret = groupMeterList.num ;

    /* 使用互斥量进行解锁 */
    pthread_mutex_unlock(&groupMeterList.mutex);

    return ret;
}

#endif
#if DESC("ftp升级功能", 1)
/**********************************************************************
* @name      : app_ftp_info_lock_init
* @brief     ：
* @param[out]：
* @return    ：
* @Date      ：2020-1-10
* @Update    :
**********************************************************************/
void app_ftp_info_lock_init(void)
{
    /* 初始化锁 */
    memset(&ftpInfo, 0, sizeof(ftpInfo));
    pthread_mutex_init(&ftpInfo.mutex, NULL);

    return;
}

/**
*********************************************************************
* @name：      app_set_server_info
* @brief：     设置升级服务器信息
* @param[in] timingmode  校时模式
* @param[out]：
* @return    ：
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
void app_set_server_info(OOP_FTPSERVER_T serverInof)
{
    pthread_mutex_lock(&ftpInfo.mutex);
    ftpInfo.serverInfo = serverInof;
    pthread_mutex_unlock(&ftpInfo.mutex);

    return;
}


/**
*********************************************************************
* @brief:       获取升级服务器信息
* @param[in]：   NA
* @param[out]： 
* @return：      
*********************************************************************
*/
int app_server_info_read(int clientid, uint8 logicId, uint16 infoNum, uint32 value)
{
    int   ret = -1;
    OOP_FTPSERVER_T serverInfo;
    uint32 len=0;
    if (value != 0xf0020400)
    {
        return ret;
    }
    NOMAL_OAD_T NormalOad;
    NormalOad.logicId = 0;
    NormalOad.oad.value = 0xf0020400;
    NormalOad.infoNum = 0;
    ret = db_read_nomal(clientid, &NormalOad, sizeof(serverInfo), (uint8 *)&serverInfo, &len);
    if (ret != ERR_OK || len != sizeof(serverInfo))
    {
        PRTL_FMT_DEBUG("app_server_info_read failed ret %d \n", ret);
        return -1;
    }
    app_set_server_info(serverInfo);
    return ret;
}

/**
*********************************************************************
* @name：      app_set_ftp_info
* @brief：     设置ftp升级参数
* @param[in] ：
* @param[out]：
* @return    ：
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
void app_set_ftp_info(OOPM_FTP_T *ftp, uint16  len)
{
    if(len != sizeof(OOPM_FTP_T))
        return;
    
    pthread_mutex_lock(&ftpInfo.mutex);
    ftpInfo.isVaild = 1;
    memcpy(&ftpInfo.ftp, ftp, len);
    pthread_mutex_unlock(&ftpInfo.mutex);
}

/**
*********************************************************************
* @brief:       更新ftp升级参数
* @param[in]：   NA
* @param[out]： syncclock 校时参数
* @return：      -1 未设置 >=0 获取到
*********************************************************************
*/
int app_ftp_info_read(int clientid, uint8 logicId, uint16 infoNum, uint32 value)
{
    int   ret=-1;
    uint32 len=0;
    if (value != 0xf0020700)
    {
        return ret;
    }
    NOMAL_OAD_T NormalOad = {0};
    NormalOad.logicId = 0;
    NormalOad.oad.value = 0xf0020700;
    NormalOad.infoNum = 0;
    OOPM_FTP_T ftp;
    MEMZERO(&ftp, sizeof(ftp));
    ret = db_read_nomal(clientid, &NormalOad, sizeof(ftp), (uint8*)&ftp, &len);
    if(ret != 0 || len != sizeof(ftp))
    {
        PRTL_FMT_DEBUG("app_ftp_info_read failed ret %d \n", ret);
        return -1;
    }
    app_set_ftp_info(&ftp, len);
    return ret;
}

/**
*********************************************************************
* @name：      app_get_server_info
* @brief：     获取升级服务器信息
* @param[in] ：
* @param[out]：
* @return    ：-1表示未设置 
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
int app_get_server_info(OOP_FTPSERVER_T *serverInfo)
{
    int ret = -1;
    pthread_mutex_lock(&ftpInfo.mutex);
    *serverInfo = ftpInfo.serverInfo;
    ret = 0;
    pthread_mutex_unlock(&ftpInfo.mutex);
    return ret;
}

/**
*********************************************************************
* @name：      app_get_ftp_info
* @brief：     获取升级参数
* @param[in] ：
* @param[out]：
* @return    ：-1表示未设置 0 正确
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
int app_get_ftp_info(OOPM_FTP_T *ftp)
{
    int ret = -1;
    pthread_mutex_lock(&ftpInfo.mutex);
    if (ftpInfo.isVaild == 1)
    {
        memcpy(ftp, &ftpInfo.ftp, sizeof(OOPM_FTP_T));
        ret = 0;
    }
    pthread_mutex_unlock(&ftpInfo.mutex);
    return ret;
}

/**
*********************************************************************
* @name：      app_get_ftp_status
* @brief：     是否需要升级
* @param[in] ：
* @param[out]：
* @return    ：
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
BOOLEAN app_get_ftp_status(void)
{
    BOOLEAN ret = -1;
    pthread_mutex_lock(&ftpInfo.mutex);
    ret = ftpInfo.isVaild;
    pthread_mutex_unlock(&ftpInfo.mutex);
    return ret;
}

/**
*********************************************************************
* @name：      app_clr_ftp_status
* @brief：     清空升级状态
* @param[in] ：
* @param[out]：
* @return    ：
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
void app_clr_ftp_status(void)
{
    pthread_mutex_lock(&ftpInfo.mutex);
    ftpInfo.isVaild = 0;
    pthread_mutex_unlock(&ftpInfo.mutex);

    return;
}

#endif
#endif


#if DESC("参数变更初始化" ,1)
/**
*********************************************************************
* @name：      var_update_init
* @brief：     规约解析模板的参数变更初始化
* @param[in] ：clientid  数据库句柄
* @param[out]：
* @return    ：
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
void appdb_var_update_init(int clientid)
{
    sync_cycle_param_init(clientid);
        
    search_param_init(clientid);

    regapp_info_lock_init();

    esam_state_lock_init();

    remain_energy_lock_init();

    guard_set_lock_init();

    remote_set_lock_init();

    appdb_meter_list_record_init(clientid);

    appdb_seek_meter_list_record_init(clientid);

    appdb_encryption_res_list_record_init(clientid);

    appdb_seek_across_list_record_init(clientid);

    appdb_nodeversion_list_record_init(clientid);

    appdb_childnode_list_record_init(clientid);

    appdb_task_list_record_init(clientid);

    appdb_plan_report_list_record_init(clientid);

    appdb_plan_normal_list_record_init(clientid);

    appdb_plan_event_list_record_init(clientid);
    
    appdb_plan_transparent_list_record_init(clientid);
    
    //appdb_percetion_termimal_info_init(clientid);
    #ifdef AREA_ZHEJIANG
    appdb_cbir_list_record_init(clientid);
    #endif

    #ifdef AREA_FUJIAN
    appdb_task_list_record_init_fj(clientid);
    appdb_plan_report_list_record_init_fj(clientid);	
    #endif
    
    app_module_meter_init();

    app_module_meter_update();

    return;
}

#endif
