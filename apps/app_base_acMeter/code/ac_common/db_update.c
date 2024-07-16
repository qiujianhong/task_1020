/*
*********************************************************************
* Copyright(C) 2019 �Ͼ��������ӹɷ����޹�˾
* All rights reserved. 
* @brief��   �߼�APP����ͨ��ģ�� �������ĸ���
* @date��    2019-12-31
* @history�� 
*********************************************************************
*/
#include "ac_main.h"
#include "baseCommon.h"
#include "db_api.h"
#include "task_id.h"
#include "task_msg.h"
#include "msg_id.h"
#include <sys/prctl.h>
#include "msg_base.h"
#include "db_update.h"

#define UPDATE_ALL     0xffff
#define UPOAD_NULL     0
#define UPMASK_ALL     0xffffffff
#define UPINIT_ALL     0xff
#define UPCLASS_ALL    0xffff

extern int stat_change_update(int clientid, uint8 logicId, uint16 infoNum, uint32 value);
extern void stat_change_init();
extern int term_addres_update(int clientid, uint8 logicId, uint16 infoNum, uint32 value);

extern uint8 gdelay;
/* 
 * �������ĸ���
 * �߼��豸�ź���Ϣ���Ϊ0xffff, ��ʶͨ��;
 * ɸѡ������OAD, ��¼�����ݸ���ƥ�����DI, ��ͨ���ݸ���ƥ����ǵ�һ����DI
 * ����, ɾѡ���ݺ͸����������벿����ͬ, ��ƥ��
 * ��Ҫ��ע�ĸ������������������
*/
DB_UPDATE_T g_dbUpdateInfo[] = 
{/* �߼��豸��,  ��Ϣ���,   ɸѡ����,   ����      , ��ͨ���ݸ��º���,    ��¼�����ݸ���, ��ʼ������ */

    {UPDATE_ALL, UPDATE_ALL, 0x50000300, UPMASK_ALL, stat_change_update,  NULL,           stat_change_init},
    {UPDATE_ALL, UPDATE_ALL, 0x50010300, UPMASK_ALL, stat_change_update,  NULL,           NULL},
    {UPDATE_ALL, UPDATE_ALL, 0x50020300, UPMASK_ALL, stat_change_update,  NULL,           NULL},
    {UPDATE_ALL, UPDATE_ALL, 0x50030300, UPMASK_ALL, stat_change_update,  NULL,           NULL},
    {UPDATE_ALL, UPDATE_ALL, 0x50040300, UPMASK_ALL, stat_change_update,  NULL,           NULL},
    {UPDATE_ALL, UPDATE_ALL, 0x50050300, UPMASK_ALL, stat_change_update,  NULL,           NULL},
    {UPDATE_ALL, UPDATE_ALL, 0x50060300, UPMASK_ALL, stat_change_update,  NULL,           NULL},
    {UPDATE_ALL, UPDATE_ALL, 0x50070300, UPMASK_ALL, stat_change_update,  NULL,           NULL},
    {UPDATE_ALL, UPDATE_ALL, 0x21000300, UPMASK_ALL, stat_change_update,  NULL,           NULL},
    {UPDATE_ALL, UPDATE_ALL, 0x21010300, UPMASK_ALL, stat_change_update,  NULL,           NULL},
    {UPDATE_ALL, UPDATE_ALL, 0x21020300, UPMASK_ALL, stat_change_update,  NULL,           NULL},
    {UPDATE_ALL, UPDATE_ALL, 0x21030300, UPMASK_ALL, stat_change_update,  NULL,           NULL},
    {UPDATE_ALL, UPDATE_ALL, 0x21040300, UPMASK_ALL, stat_change_update,  NULL,           NULL},
    {UPDATE_ALL, UPDATE_ALL, 0x21100300, UPMASK_ALL, stat_change_update,  NULL,           NULL},
    {UPDATE_ALL, UPDATE_ALL, 0x21110300, UPMASK_ALL, stat_change_update,  NULL,           NULL},
    {UPDATE_ALL, UPDATE_ALL, 0x21120300, UPMASK_ALL, stat_change_update,  NULL,           NULL},
    {UPDATE_ALL, UPDATE_ALL, 0x21130300, UPMASK_ALL, stat_change_update,  NULL,           NULL},
    {UPDATE_ALL, UPDATE_ALL, 0x21140300, UPMASK_ALL, stat_change_update,  NULL,           NULL},
    {UPDATE_ALL, UPDATE_ALL, 0x21200300, UPMASK_ALL, stat_change_update,  NULL,           NULL},
    {UPDATE_ALL, UPDATE_ALL, 0x21210300, UPMASK_ALL, stat_change_update,  NULL,           NULL},
    {UPDATE_ALL, UPDATE_ALL, 0x21220300, UPMASK_ALL, stat_change_update,  NULL,           NULL},
    {UPDATE_ALL, UPDATE_ALL, 0x21230300, UPMASK_ALL, stat_change_update,  NULL,           NULL},
    {UPDATE_ALL, UPDATE_ALL, 0x21240300, UPMASK_ALL, stat_change_update,  NULL,           NULL},
    {UPDATE_ALL, UPDATE_ALL, 0x43000B00, UPMASK_ALL, stat_change_update,  NULL,           NULL},    
    {UPDATE_ALL, UPDATE_ALL, 0xF2000500, UPMASK_ALL, stat_change_update,  NULL,           NULL},    
    {UPDATE_ALL, UPDATE_ALL, 0xF2010500, UPMASK_ALL, stat_change_update,  NULL,           NULL},    
    {UPDATE_ALL, UPDATE_ALL, 0xF2020500, UPMASK_ALL, stat_change_update,  NULL,           NULL},    
    {UPDATE_ALL, UPDATE_ALL, 0xF20E0500, UPMASK_ALL, stat_change_update,  NULL,           NULL},     
    {UPDATE_ALL, UPDATE_ALL, 0x40010200, UPMASK_ALL, term_addres_update,  NULL,           NULL},    
};
    
uint16 g_dbUpdateNum = sizeof(g_dbUpdateInfo)/sizeof(DB_UPDATE_T);

/*
 * �������ĸ��¶��� ���û����DI��ֻ��һ��������DI������
 * �������ͱ�ʶ DATA_CLASS_E
 * ���ݶ���     TRUE-��ͨ����; FALSE-��¼����
 * ɸѡ����
*/
DB_CUSTOM_T g_DBCustom[] = 
{/* ��������,  ���ݶ���,   ɸѡ���� */
    {CLASS_DATA_COLLEC, TRUE, 0xFF100A00},
    {CLASS_DATA_COLLEC, TRUE, 0xFF100B00},
    {CLASS_DATA_COLLEC, TRUE, 0xFF100C00},    
};
    
uint16 g_dbcustomNum = sizeof(g_DBCustom)/sizeof(DB_CUSTOM_T);



/**********************************************************************
* @name      : DB_Default_INFO
* @brief     ����ʼ���¼� �ָ���������
* @param[in] ��int clientid    db�ͻ���
               uint8 initType  ��ʼ������
               uint8 logicId   �߼��豸��
               uint16 classTag �������ͱ��
* @param[out]��
* @return    ��0-�ɹ�/������
* @Create    : 
* @Date      ��2020-03-6
* @Update    :
**********************************************************************/
int DB_INIT_INFO(int clientid, uint8 initType, uint8 logicId, uint16 classTag)
{
    return 0;
}

/**********************************************************************
* @name      : DB_INIT_INIT
* @brief     ����ʼ���¼���ʼ��
* @param[in] ��
* @param[out]��
* @return    ��
* @Create    : 
* @Date      ��2020-05-11
* @Update    :
**********************************************************************/
void DB_INIT_INIT()
{
    return;
}

/**********************************************************************
* @name      : DB_Default_INIT
* @brief     ���ָ��������ó�ʼ��
* @param[in] ��
* @param[out]��
* @return    ��
* @Create    : 
* @Date      ��2020-05-11
* @Update    :
**********************************************************************/
void DB_Default_INIT()
{
    return;
}

/**********************************************************************
* @name      : DB_Default_INFO
* @brief     ����ʼ���¼� �ָ���������
* @param[in] ��int clientid    db�ͻ���
               uint8 initType  ��ʼ������
               uint8 logicId   �߼��豸��
               uint16 classTag �������ͱ��
* @param[out]��
* @return    ��0-�ɹ�/������
* @Create    : 
* @Date      ��2020-03-6
* @Update    :
**********************************************************************/
int DB_Default_INFO(int clientid, uint8 initType, uint8 logicId, uint16 classTag)
{
    return 0;
}

/* 
 * �������ĳ�ʼ���¼�
 * �߼��豸��Ϊ0xff, ��ʶͨ��
 * �������ʶΪ0xffff, ��ʶͨ��
 * ��Ҫ��ע�ĸ������������������
*/
DBINIT_EVENT_T g_dbInitInfo[] = 
{/*  ��ʼ������, �߼��豸��, �������ʶ, �ص�����,           ��ʼ������ */
    {0,          UPINIT_ALL, UPDATE_ALL, DB_INIT_INFO,       DB_INIT_INIT},
    {1,          UPINIT_ALL, UPDATE_ALL, DB_Default_INFO,    DB_Default_INIT},
};

uint16 g_dbInitNum = sizeof(g_dbInitInfo)/sizeof(DBINIT_EVENT_T);

DBUPDATE_LIST_T g_dbUpdateList;

/**********************************************************************
* @name      : db_updata_init
* @brief     �����ݸ��³�ʼ�� ��Ҫ����
* @param[in] ��
* @param[out]��
* @return    ��
* @Create    : ����
* @Date      ��2020-1-10
* @Update    :
**********************************************************************/
void db_updata_init()
{
    uint16 i;

    pthread_mutex_init(&g_dbUpdateList.flock, NULL);
    g_dbUpdateList.head = NULL;
    g_dbUpdateList.tail = NULL;

    for(i = 0; i < g_dbUpdateNum; i++)
    {
        if(g_dbUpdateInfo[i].initc)
        {
            g_dbUpdateInfo[i].initc();
        }
    }

    for(i = 0; i < g_dbInitNum; i++)
    {
        if(g_dbInitInfo[i].initc)
        {
            g_dbInitInfo[i].initc();
        }
    }
}

/**********************************************************************
* @name      : db_list_add
* @brief     �������������ĸ����б�
* @param[in] ��uint8 type    ���� 0,1
               void *pMsg    ��Ϣ DB_UPDATE_MSG_T �� DBINIT_EVENT_MSG_T
* @param[out]��
* @return    ��0-�ɹ�/����
* @Create    : ����
* @Date      ��2020-9-10
* @Update    :
**********************************************************************/
int db_list_add(uint8 type, void *pMsg)
{
    DBUPDATE_NODE_T *pNode;

    if(type > 1 || NULL == pMsg)
    {
        return -1;
    }

    pNode = (DBUPDATE_NODE_T *)malloc(sizeof(DBUPDATE_NODE_T));
    if(NULL == pNode)
    {
        return -2;
    }
    memset(pNode, 0, sizeof(DBUPDATE_NODE_T));

    pNode->type = type;
    if(0 == type)
    {
        memcpy(&(pNode->msg), pMsg, sizeof(DB_UPDATE_MSG_T));
    }
    else if(1 == type)
    {
        memcpy(&(pNode->msg), pMsg, sizeof(DBINIT_EVENT_MSG_T));
    }
    
    /* ��ӵ������� */
    pthread_mutex_lock(&g_dbUpdateList.flock);
    if(NULL == g_dbUpdateList.head)
    {
        g_dbUpdateList.head = pNode;
    }
    if(NULL != g_dbUpdateList.tail)
    {
        g_dbUpdateList.tail->pNext = pNode;
    }
    g_dbUpdateList.tail = pNode;

    pthread_mutex_unlock(&g_dbUpdateList.flock);
    return 0;
}

/**********************************************************************
* @name      : db_list_get
* @brief     ����ȡ�������ĸ����б�
* @param[in] ��
* @param[out]��DBUPDATE_NODE_T *pNode   ��Ϣ�ڵ�
* @return    ��0-�ɹ�/����
* @Create    : ����
* @Date      ��2020-9-10
* @Update    :
**********************************************************************/
int db_list_get(DBUPDATE_NODE_T *pNode)
{
    if(NULL == pNode)
    {
        return -1;
    }

    pthread_mutex_lock(&g_dbUpdateList.flock);
    if(NULL == g_dbUpdateList.head)
    {
        pthread_mutex_unlock(&g_dbUpdateList.flock);
        return -2;
    }

    memcpy(pNode, g_dbUpdateList.head, sizeof(DBUPDATE_NODE_T));

    free(g_dbUpdateList.head);
    g_dbUpdateList.head = pNode->pNext;
    if(NULL == g_dbUpdateList.head)
    {
        g_dbUpdateList.tail = NULL;
    }
    pthread_mutex_unlock(&g_dbUpdateList.flock);
    return 0;
}


/**********************************************************************
* @name      : db_update_entry
* @brief     ����������������Ϣ����
* @param[in] ��void *pArg
* @param[out]��
* @return    ��
* @Create    : ����
* @Date      ��2019-12-31
* @Update    :
**********************************************************************/
void *db_update_entry(void *pArg)
{
    int              ret = 0;
    int              recvret = 0;
    uint16           i;
    DB_CLIENT        clientid = 0;
    DBUPDATE_NODE_T     tMsgNode;
    DB_UPDATE_MSG_T    *pUpdateMsg = NULL;
    DBINIT_EVENT_MSG_T *pInitMsg = NULL;
    uint16           index = 0;

    THREAD_ARG_T *tArg = (THREAD_ARG_T *)pArg;

    //�߳���
    prctl(PR_SET_NAME, tArg->threadName);

    clientid = db_client_create(DB_AC_AGENT, DB_AC_CLIENT_UPDATE);
    if(clientid <= 0)
    {
        printf("Main update db db_client_create error!\n");
        usleep(100);
        exit(0); //�����˳�
    }

    memset(&tMsgNode, 0, sizeof(DBUPDATE_NODE_T));
    /* ѭ������Ϣ */
    for(;;)
    {
        recvret = db_list_get(&tMsgNode);
        if(0 != recvret)
        {
            usleep(10000);
            continue;
        }

        if(0 == tMsgNode.type)    /* ���ݸ��� */
        {
            pUpdateMsg = &(tMsgNode.msg.tUpdate);
            if(TRUE == pUpdateMsg->bRecord)
            {
                /* ��¼�����ݸ��� */
                index = pUpdateMsg->index[0];
                if(index < g_dbUpdateNum && NULL != g_dbUpdateInfo[index].rfunc)
                {
                    ret = g_dbUpdateInfo[index].rfunc(clientid, pUpdateMsg->logicId, pUpdateMsg->infoNum, 
                                                      pUpdateMsg->dataflag, pUpdateMsg->slavenum, pUpdateMsg->slavedata);
                    YCDB_FMT_TRACE("Update record 0x%08x, di 0x%08x, ret %d, logic %d, infonum %d\n", 
                                   pUpdateMsg->dataflag, g_dbUpdateInfo[index].dataflag, ret, pUpdateMsg->logicId, pUpdateMsg->infoNum);
                }
            }
            else
            {
                /* ��ͨ�����ݸ��� */
                for(i = 0; i < pUpdateMsg->slavenum; i++)
                {
                    index = pUpdateMsg->index[i];
                    if(index < g_dbUpdateNum && NULL != g_dbUpdateInfo[index].func)
                    {
                        ret = g_dbUpdateInfo[index].func(clientid, pUpdateMsg->logicId, pUpdateMsg->infoNum, 
                                                         pUpdateMsg->slavedata[i]);
                        YCDB_FMT_TRACE("Update normal 0x%08x, di 0x%08x, ret %d, logic %d, infonum %d\n", 
                                       pUpdateMsg->slavedata[i], g_dbUpdateInfo[index].dataflag, ret, pUpdateMsg->logicId, pUpdateMsg->infoNum);
                    }
                }
            }
        }
        else if(1 == tMsgNode.type)  /* ��ʼ�� */
        {
            pInitMsg = &(tMsgNode.msg.tEvent);
            for(i = 0; i < g_dbInitNum; i++)
            {
                if(pInitMsg->initType == g_dbInitInfo[i].initType
                   && (pInitMsg->logicId == g_dbInitInfo[i].logicId || UPINIT_ALL == g_dbInitInfo[i].logicId)
                   && (pInitMsg->classTag == g_dbInitInfo[i].classTag || UPCLASS_ALL == g_dbInitInfo[i].classTag))
                {
                    if(g_dbInitInfo[i].func)
                    {
                        ret = g_dbInitInfo[i].func(clientid, pInitMsg->initType, pInitMsg->logicId, 
                                                   pInitMsg->classTag);
                        YCDB_FMT_TRACE("INIT db ligic %d, ret %d, type %d, class %d\n", 
                                        pInitMsg->logicId, ret, pInitMsg->initType, pInitMsg->classTag);
                    }
                    break;
                }
            }

        }

        memset(&tMsgNode, 0, sizeof(DBUPDATE_NODE_T));
        if(gdelay != 0)
        {
            usleep(gdelaytime);
        }
    }
    
    db_client_close(clientid);
    return 0;
}

char     g_updatastr[1024] = {0};
/**********************************************************************
* @name      : db_update_msg_proc
* @brief     ��db������Ϣ����
* @param[in] ��MESSAGE_INFO_T *pMsg ��Ϣ�ṹ��
* @param[out]��
* @return    ��0-�ɹ�/������(����DB_AGENT_UNKONW_MSG, ��������Ϣ��������)
* @Create    : ����
* @Date      ��2019-12-30
* @Update    :
**********************************************************************/
int db_update_msg_proc(MESSAGE_INFO_T *pMsg)
{
    int                ret = 0;
    DB_UPDATE_MSG_T    tUpdateMsg;
    DBINIT_EVENT_MSG_T tInitMsg = {0};
    uint8             *pTemp = NULL;
    uint16             offset = 0;
    uint16             i, j;
    uint8              bIndexOK = FALSE;

    if(NULL == pMsg)
    {
        return -1;
    }

    /* 1 ���ж���ϢIID�Ƿ񱾺������� */
    if(MESSAGE_IID_DB != pMsg->IID)
    {
        return DB_AGENT_UNKONW_MSG;
    }

    if(DB_IOP_UPDATE_EVENT == pMsg->IOP)
    {
        pTemp = pMsg->playload;
        memset(&tUpdateMsg, 0, sizeof(DB_UPDATE_MSG_T));
        
        tUpdateMsg.logicId = *pTemp;
        pTemp++;
        
        MEMCPY_R(&tUpdateMsg.infoNum, pTemp, 2);
        pTemp += 2;

        MEMCPY_R(&tUpdateMsg.classtag, pTemp, 2);
        pTemp += 2;

        tUpdateMsg.bOADData = *pTemp;
        pTemp++;
        if(0 == tUpdateMsg.bOADData)
        {
            YCDB_FMT_TRACE("DB Update no data, logicId %d, infoNum %d, classtag %d\n", tUpdateMsg.logicId,
                           tUpdateMsg.infoNum, tUpdateMsg.classtag);
            return 0;
        }

        tUpdateMsg.bOptional = *pTemp;
        pTemp++;

        /* �߼��豸7������������ֱ�ӹ��� */
        if (tUpdateMsg.logicId == 7)
        {
            YCDB_FMT_TRACE("DB Update error msg logicId %d\n", tUpdateMsg.logicId);
            return -1;
        }

        if(1 == tUpdateMsg.bOptional)
        {
            tUpdateMsg.dataflag = *(uint32 *)pTemp;
            pTemp += 4;
            tUpdateMsg.dataflag = ntohl(tUpdateMsg.dataflag);
        }
        else if(tUpdateMsg.bOptional > 1)
        {
            YCDB_FMT_TRACE("DB Update error msg %d %d, optional %d\n", tUpdateMsg.logicId, tUpdateMsg.infoNum, tUpdateMsg.bOptional);
            return -1;
        }
        
        tUpdateMsg.slavenum = MIN(*pTemp, DB_UPDATE_MAX_SLAVE);
        pTemp++;
        if(0 == tUpdateMsg.slavenum)
        {
            YCDB_FMT_TRACE("DB Update error msg %d %d master 0x%x, slavenum is 0\n", 
                           tUpdateMsg.logicId, tUpdateMsg.infoNum, tUpdateMsg.dataflag);
            return -1;
        }
        
        offset = sprintf(g_updatastr, "DB Update %d %d class %d, master 0x%x, slave:", tUpdateMsg.logicId, tUpdateMsg.infoNum, 
                         tUpdateMsg.classtag, tUpdateMsg.dataflag);
        for(i = 0; i < tUpdateMsg.slavenum; i++)
        {
            tUpdateMsg.slavedata[i] = *(uint32*)pTemp;
            pTemp += 4;
            tUpdateMsg.slavedata[i] = ntohl(tUpdateMsg.slavedata[i]);
            tUpdateMsg.index[i] = DB_DEFAULT_INDEX;

            offset += sprintf(g_updatastr + offset, " 0x%x", tUpdateMsg.slavedata[i]);
        }
        
        /* �ж��Ƿ��¼���� */
        if(1 == tUpdateMsg.bOptional || CLASS_DATA_EVENT == tUpdateMsg.classtag || CLASS_DATA_COLLEC == tUpdateMsg.classtag)
        {
            tUpdateMsg.bRecord = TRUE;
        }
        else
        {
            tUpdateMsg.bRecord = FALSE;
        }
        /* ���ƻ��ı� */
        if(0 == tUpdateMsg.bOptional && 1 == tUpdateMsg.slavenum)
        {
            for(i = 0; i < g_dbcustomNum; i++)
            {
                if(g_DBCustom[i].classTag == tUpdateMsg.classtag && g_DBCustom[i].dataflag == tUpdateMsg.slavedata[0])
                {
                    tUpdateMsg.bRecord = ((TRUE == g_DBCustom[i].bNormal) ? FALSE : TRUE);
                    break;
                }
            }
        }
        
        YCDB_FMT_TRACE("%s %s\n", TRUE == tUpdateMsg.bRecord ? "Record" : "Normal", g_updatastr);
        memset(g_updatastr, 0, offset);
        if(TRUE == tUpdateMsg.bRecord)
        {
            /* ��¼�ͻ�ȡindex */
            for(i = 0; i < g_dbUpdateNum; i++)
            {
                if(NULL != g_dbUpdateInfo[i].rfunc &&
                   (tUpdateMsg.dataflag & g_dbUpdateInfo[i].maskInfo) == (g_dbUpdateInfo[i].dataflag & g_dbUpdateInfo[i].maskInfo)&&
                   (UPDATE_ALL == g_dbUpdateInfo[i].logicId || tUpdateMsg.logicId == g_dbUpdateInfo[i].logicId) &&
                   (UPDATE_ALL == g_dbUpdateInfo[i].infoNum || tUpdateMsg.infoNum == g_dbUpdateInfo[i].infoNum))
                {
                    tUpdateMsg.index[0] = i;
                    bIndexOK = TRUE;
                    break;
                }
            }
        }
        else
        {
            for(j = 0; j < tUpdateMsg.slavenum; j++)
            {
                for(i = 0; i < g_dbUpdateNum; i++)
                {
                    if(NULL != g_dbUpdateInfo[i].func &&
                       (tUpdateMsg.slavedata[j] & g_dbUpdateInfo[i].maskInfo) == (g_dbUpdateInfo[i].dataflag & g_dbUpdateInfo[i].maskInfo) && 
                       (UPDATE_ALL == g_dbUpdateInfo[i].logicId || tUpdateMsg.logicId == g_dbUpdateInfo[i].logicId) &&
                       (UPDATE_ALL == g_dbUpdateInfo[i].infoNum || tUpdateMsg.infoNum == g_dbUpdateInfo[i].infoNum))
                    {
                        tUpdateMsg.index[j] = i;
                        
                        bIndexOK = TRUE;
                        break;
                    }
                }
            }
        }
        if(TRUE == bIndexOK)
        {
            /* �Լ������Լ� */
            ret = db_list_add(0, &tUpdateMsg);
            if(0 != ret)
            {
                YCMSG_FMT_DEBUG("Update add failed, db 0x%x, logic %d, infonum %d\n", 
                                tUpdateMsg.dataflag, tUpdateMsg.logicId, tUpdateMsg.infoNum);
            }
            else
            {
                YCDB_FMT_TRACE("Update add, db 0x%x, logic %d, infonum %d\n", 
                                tUpdateMsg.dataflag, tUpdateMsg.logicId, tUpdateMsg.infoNum);
            }
        }
    }
    else if(DB_IOP_INIT_EVENT == pMsg->IOP)
    {
        pTemp = pMsg->playload;
        tInitMsg.initType = pTemp[offset++];
        if(0 == tInitMsg.initType)
        {
            tInitMsg.logicId = pTemp[offset++];
            if(1 == pTemp[offset++])
            {
                tInitMsg.classTag = *(uint16 *)(pTemp + offset);
                tInitMsg.classTag = ntohs(tInitMsg.classTag);
            }
        }
        else if(1 == tInitMsg.initType)
        {
            tInitMsg.logicId = pTemp[offset++];
        }
        else
        {
            YCDB_FMT_TRACE("Unknow dbinit msg type %d\n", tInitMsg.initType);
            return ret;
        }
        YCDB_FMT_TRACE("DB Init msg type %d, logic %d, class %d\n", tInitMsg.initType, tInitMsg.logicId, 
                        tInitMsg.classTag);

        /* �Լ������Լ� */
        ret = db_list_add(1, &tInitMsg);
        if(0 != ret)
        {
            YCMSG_FMT_DEBUG("DB init add failed, type %d, logic %d, infonum %d\n",
                            tInitMsg.initType, tInitMsg.logicId, tInitMsg.initType);
        }
    }
    else
    {
        return DB_AGENT_UNKONW_MSG;
    }
    return ret;
}

