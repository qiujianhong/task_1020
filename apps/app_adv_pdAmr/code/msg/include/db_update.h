/*
*********************************************************************
* Copyright(C) 2019 �Ͼ��������ӹɷ����޹�˾
* All rights reserved. 
* @brief��   �߼�app �������ĸ���
* @date��    2020-3-5
* @history�� 
*********************************************************************
*/

#ifndef __DB_UPDATE_H__
#define __DB_UPDATE_H__


#ifdef __cplusplus
extern "C" {
#endif

extern int g_FilesUpdate;  //�������ĵ�������¼�����

typedef int (*UpdateFunc)(DB_CLIENT, uint8, uint16, uint32);
//���: dbclient������߼��豸�š���Ϣ��š���DI����DI��������DIֵ
typedef int (*UpdateRecordFunc)(DB_CLIENT, uint8, uint16, uint32, uint8, uint32*);
typedef void (*UpdateInitFunc)();
typedef int (*DBInitFunc)(DB_CLIENT, uint8, uint8, uint16);

typedef struct tagDBUpdate
{
    uint16           logicId;
    uint16           infoNum;
    uint32           dataflag;
    uint32           maskInfo;  //����, ����һ���OAD����
    UpdateFunc       func;
    UpdateRecordFunc rfunc;
    UpdateInitFunc   initc;
}DB_UPDATE_T;

#define DB_DEFAULT_INDEX     0xffff
#define DB_UPDATE_MAX_SLAVE  32
typedef struct tagDBupdateMsg
{
    uint8    bOADData; //�������ʶ
    uint8    bOptional;//Я����OAD
    uint8    logicId;
    uint16   infoNum;  //��Ϣ���Ϊ0 ��������ͨ����

    uint16   classtag; //�������ʶ
    uint8    bRecord;  //�Ƿ�Ϊ��¼�� CLASS_DATA_EVENT �� CLASS_DATA_COLLEC
    uint8    slavenum; //����������, ���DB_UPDATE_MAX_SLAVE

    uint32   dataflag;//��OAD
    uint32   slavedata[DB_UPDATE_MAX_SLAVE];//��OAD

    uint16   index[DB_UPDATE_MAX_SLAVE]; //��¼������indexȡ��0��
}DB_UPDATE_MSG_T;

typedef struct tagDBCustom
{
    uint16           classTag;
    uint16           bNormal;
    uint32           dataflag;
}DB_CUSTOM_T;

typedef struct tagDBInitEventMsg
{
    uint8    initType;   //0-���ݳ�ʼ��;1-�ָ���������
    uint8    logicId;    //�߼��豸��
    uint16   classTag;   //�������ʶ 0-��ʾû�����ݱ�ʶ
}DBINIT_EVENT_MSG_T;

typedef struct tagDBInitEvent
{
    uint8          initType;
    uint8          logicId;
    uint16         classTag;
    DBInitFunc     func;
    UpdateInitFunc initc;
}DBINIT_EVENT_T;

/* �������ĸ��µ�Ԫ */
typedef struct tagDBUpdateNode
{
    struct tagDBUpdateNode *pNext;
    uint8  type;   /* 0-�����1-��ʼ�� */
    uint8  res[3];

    union{
        DB_UPDATE_MSG_T    tUpdate;
        DBINIT_EVENT_MSG_T tEvent;
    }msg;
}DBUPDATE_NODE_T;

/* �������ĸ����б� */
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


