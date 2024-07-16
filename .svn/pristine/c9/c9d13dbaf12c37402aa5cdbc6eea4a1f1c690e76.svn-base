/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   高级app 数据中心更新
* @date：    2020-3-5
* @history： 
*********************************************************************
*/

#ifndef __DB_UPDATE_H__
#define __DB_UPDATE_H__


#ifdef __cplusplus
extern "C" {
#endif

extern int g_FilesUpdate;  //数据中心档案变更事件处理

typedef int (*UpdateFunc)(DB_CLIENT, uint8, uint16, uint32);
//入参: dbclient句柄、逻辑设备号、信息点号、主DI、次DI数量、次DI值
typedef int (*UpdateRecordFunc)(DB_CLIENT, uint8, uint16, uint32, uint8, uint32*);
typedef void (*UpdateInitFunc)();
typedef int (*DBInitFunc)(DB_CLIENT, uint8, uint8, uint16);

typedef struct tagDBUpdate
{
    uint16           logicId;
    uint16           infoNum;
    uint32           dataflag;
    uint32           maskInfo;  //掩码, 用于一类的OAD操作
    UpdateFunc       func;
    UpdateRecordFunc rfunc;
    UpdateInitFunc   initc;
}DB_UPDATE_T;

#define DB_DEFAULT_INDEX     0xffff
#define DB_UPDATE_MAX_SLAVE  32
typedef struct tagDBupdateMsg
{
    uint8    bOADData; //数据项标识
    uint8    bOptional;//携带主OAD
    uint8    logicId;
    uint16   infoNum;  //信息点号为0 可能是普通数据

    uint16   classtag; //数据类标识
    uint8    bRecord;  //是否为记录型 CLASS_DATA_EVENT 和 CLASS_DATA_COLLEC
    uint8    slavenum; //次数据数量, 最大DB_UPDATE_MAX_SLAVE

    uint32   dataflag;//主OAD
    uint32   slavedata[DB_UPDATE_MAX_SLAVE];//次OAD

    uint16   index[DB_UPDATE_MAX_SLAVE]; //记录型数据index取第0个
}DB_UPDATE_MSG_T;

typedef struct tagDBCustom
{
    uint16           classTag;
    uint16           bNormal;
    uint32           dataflag;
}DB_CUSTOM_T;

typedef struct tagDBInitEventMsg
{
    uint8    initType;   //0-数据初始化;1-恢复出厂参数
    uint8    logicId;    //逻辑设备号
    uint16   classTag;   //数据类标识 0-表示没有数据标识
}DBINIT_EVENT_MSG_T;

typedef struct tagDBInitEvent
{
    uint8          initType;
    uint8          logicId;
    uint16         classTag;
    DBInitFunc     func;
    UpdateInitFunc initc;
}DBINIT_EVENT_T;

/* 数据中心更新单元 */
typedef struct tagDBUpdateNode
{
    struct tagDBUpdateNode *pNext;
    uint8  type;   /* 0-变更，1-初始化 */
    uint8  res[3];

    union{
        DB_UPDATE_MSG_T    tUpdate;
        DBINIT_EVENT_MSG_T tEvent;
    }msg;
}DBUPDATE_NODE_T;

/* 数据中心更新列表 */
typedef struct tagDBUpdateList
{
    pthread_mutex_t    flock;
    DBUPDATE_NODE_T   *head;
    DBUPDATE_NODE_T   *tail;
}DBUPDATE_LIST_T;

void db_updata_init();
int  db_list_add(uint8 type, void *pMsg);
int  db_list_get(DBUPDATE_NODE_T *pNode);
void *db_update_entry(void *pArg);
int db_update_msg_proc(MESSAGE_INFO_T *pMsg);

#ifdef __cplusplus
}
#endif

#endif //__MQTT_MSG_H__


