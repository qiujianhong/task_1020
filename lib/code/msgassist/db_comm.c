/*
*********************************************************************
* @filename: db_comm.c
* @brief   : ��Ϣͨ�� DB����
* @describe: 
* @history: 
*    auther      date          describe
*    ����      2019-12-27    create
*********************************************************************
*/
#include "baseCommon.h"
#include "debug_comm.h"
#include "db_comm.h"
#include "appmsg_split.h"

uint8             g_DBClientNum = 0;
uint8             g_DBClientMAX = 0;
pthread_mutex_t   g_Clientlock;          //�ͻ�����Ϣ��
DBCLIENT_CACHE_T *g_pDBClientInfo = NULL;


/**********************************************************************
* @name      : db_clientinfo_init
* @brief     ��db�ͻ�����Ϣ��ʼ��
* @param[in] ��uint8 clientnum     �ͻ������� 
* @param[out]��
* @return    ��
* @Create    : ����
* @Date      ��2019-12-27
* @Update    :
**********************************************************************/
int db_clientinfo_init(uint8 clientnum)
{
    g_pDBClientInfo = (DBCLIENT_CACHE_T *)malloc(clientnum * sizeof(DBCLIENT_CACHE_T));
    if(NULL == g_pDBClientInfo)
    {
        return DBMSG_ERR_MALLOC;
    }

    memset(g_pDBClientInfo, 0, clientnum * sizeof(DBCLIENT_CACHE_T));
    g_DBClientMAX = clientnum;
    g_DBClientNum = 0;
    pthread_mutex_init(&g_Clientlock, NULL);
    return DBMSG_SUCCESS;
}

/**********************************************************************
* @name      : db_clientinfo_free
* @brief     ��db�ͻ�����Ϣ�ͷ�
* @param[in] ��uint8 clientnum     �ͻ������� 
* @param[out]��
* @return    ��
* @Create    : ����
* @Date      ��2019-12-27
* @Update    :
**********************************************************************/
void db_clientinfo_free()
{
    uint8 i;
    DBCLIENT_CACHE_T *ptInfo = NULL;
    
    if(g_pDBClientInfo)
    {
        pthread_mutex_lock(&g_Clientlock);
        for(i = 0; i < g_DBClientNum; i++)
        {
            ptInfo = &(g_pDBClientInfo[i]);
            pthread_mutex_lock(&(ptInfo->lock));
            pthread_mutex_destroy(&(ptInfo->lock));
            pthread_cond_destroy(&(ptInfo->notify));
        }
        free(g_pDBClientInfo);
        g_pDBClientInfo = NULL;
        g_DBClientMAX = 0;
        g_DBClientNum = 0;
        pthread_mutex_destroy(&g_Clientlock);
    }
}

/**********************************************************************
* @name      : db_get_clientinfo
* @brief     ����ȡһ��client����Ϣ, �����ھͷ���һ��
* @param[in] ��uint8 clientnum     �ͻ������� 
* @param[out]��uint8 *clientid     ��������
* @return    ��DBCLIENT_CACHE_T *  �ͻ�������
* @Create    : ����
* @Date      ��2019-12-27
* @Update    :
**********************************************************************/
DBCLIENT_CACHE_T *db_get_clientinfo(uint32 taskid, uint8 *clientid)
{
    uint8 i;
    pthread_condattr_t  tAttr;

    if(0 == g_DBClientMAX)
    {
        DPRINT("dbAgent not init!\n");
        return NULL;
    }

    pthread_mutex_lock(&g_Clientlock);

    for(i = 0; i < g_DBClientNum; i++)
    {
        if(taskid == g_pDBClientInfo[i].taskid)
        {
            if(clientid)
            {
                *clientid = i;
            }
            pthread_mutex_unlock(&g_Clientlock);
            return &(g_pDBClientInfo[i]);
        }
    }
    if(g_DBClientNum >= g_DBClientMAX)
    {
        DPRINT("dbClient out of maxsize!\n");
        pthread_mutex_unlock(&g_Clientlock);
        return NULL;
    }
    
    pthread_mutex_init(&(g_pDBClientInfo[i].lock), NULL);

    /* pthread_cond_timedwait���ʱ��ģʽ */
    pthread_condattr_setclock(&tAttr, CLOCK_MONOTONIC);
    pthread_cond_init(&(g_pDBClientInfo[i].notify), &tAttr);

    g_pDBClientInfo[i].taskid = taskid;
    g_DBClientNum++;
    
    if(clientid)
    {
        *clientid = i;
    }

    pthread_mutex_unlock(&g_Clientlock);
    return &(g_pDBClientInfo[i]);
}

/**********************************************************************
* @name      : db_get_clientinfo_byid
* @brief     ����ȡһ��client����Ϣ
* @param[in] ��DB_CLIENT clientid     ��������
* @param[out]��
* @return    ��DBCLIENT_CACHE_T *  �ͻ�������
* @Create    : ����
* @Date      ��2019-12-27
* @Update    :
**********************************************************************/
DBCLIENT_CACHE_T *db_get_clientinfo_byid(DB_CLIENT clientid)
{
    if(clientid > g_DBClientNum || clientid <= 0)
    {
        return NULL;
    }
    return &(g_pDBClientInfo[clientid - 1]);
}

/**********************************************************************
* @name      : db_clear_clientinfo
* @brief     ����տͻ��˹ؼ���Ϣ
* @param[in] ��DBCLIENT_CACHE_T *ptInfo  �ͻ�������
* @param[out]��
* @return    ��DBCLIENT_CACHE_T *  �ͻ�������
* @Create    : ����
* @Date      ��2019-12-27
* @Update    :
**********************************************************************/
void db_clear_clientinfo(DBCLIENT_CACHE_T *ptInfo)
{
    if(NULL == ptInfo)
    {
        return;
    }
    pthread_mutex_lock(&(ptInfo->lock));
    ptInfo->ackbuflen = 0;
    ptInfo->bwait = 0;
    ptInfo->msgindex = 0;
    ptInfo->taskindex = 0;
    pthread_mutex_unlock(&(ptInfo->lock));
}

/**********************************************************************
* @name      : send_read_nomal_data
* @brief     �����Ͳ�ѯ��ͨ������Ϣ
* @param[in] ��DBMSG_SEND_FUN func    ���ͻص�����
               uint16 label           ��Ϣlabel
               NOMAL_OAD_T *nomalOAD  oad
* @param[out]��uint16 *index          ������Ϣ��index
* @return    ��������
* @Create    : ����
* @Date      ��2019-12-27
* @Update    : 2020-4-28
**********************************************************************/
int send_read_nomal_data(DBMSG_SEND_FUN func, uint16 label, NOMAL_OAD_T *nomalOAD, uint16 *index)
{
    uint16      bufLen = 0;
    uint8       playBuf[10] = {0};

    playBuf[bufLen++] = nomalOAD->logicId;                                  //�߼��豸��
    appmsg_memcpy_r(playBuf + bufLen, &(nomalOAD->infoNum), sizeof(uint16));//��Ϣ���
    bufLen += 2;
    playBuf[bufLen++] = 1;                                                  //����
    appmsg_memcpy_r(playBuf + bufLen, &(nomalOAD->oad), sizeof(OOP_OAD_U)); //�������ʶ
    bufLen += sizeof(OOP_OAD_U);

    return func(label, DB_IOP_READ_NORMAL_DATA, playBuf, bufLen, index);
}

/**********************************************************************
* @name      : send_write_nomal_data
* @brief     ������д��ͨ������Ϣ
* @param[in] ��DBMSG_SEND_FUN func    ���ͻص�����
               uint16 label           ��Ϣlabel
               NOMAL_OAD_T *nomalOAD  oad
               uint8 *pData           ����
               uint32 dataLen         ����
* @param[out]��uint16 *index          ������Ϣ��index
* @return    ��������
* @Create    : ����
* @Date      ��2019-12-27
* @Update    :
**********************************************************************/
int send_write_nomal_data(DBMSG_SEND_FUN func, uint16 label, NOMAL_OAD_T *nomalOAD, uint8 *pData, 
                          uint32 dataLen, uint16 *index)
{
    int    ret = 0;
    uint16 msglen = 0;
    uint16 i = 0;
    uint8 *playload = NULL;
    uint8  bufLen[3] = {0};
    uint8  bytes = 0;

    bytes = appmsg_set_len_offset((uint16)dataLen, bufLen);
    msglen = 1 + 2 + 1 + 4 + 2 + bytes + dataLen;
    playload = (uint8 *)malloc(msglen);
    if(NULL == playload)
    {
        return DBMSG_ERR_MALLOC;
    }
    memset(playload, 0, msglen);

    playload[i++] = nomalOAD->logicId;                                   //�߼��豸��
    appmsg_memcpy_r(playload + i, &(nomalOAD->infoNum), sizeof(uint16)); //��Ϣ���
    i += 2;
    playload[i++] = 1;                                                   //��ͨ��������
    appmsg_memcpy_r(playload + i, &(nomalOAD->oad), sizeof(OOP_OAD_U));  //�������ʶ
    i += sizeof(OOP_OAD_U);
    if(0 == nomalOAD->classtag)
    {
        uint16 tempclass = CLASS_DATA_NORMAL;
        appmsg_memcpy_r(playload + i, &tempclass, sizeof(uint16));//�������ʶ Ϊ0����д��CLASS_DATA_NORMAL
    }
    else
    {
        appmsg_memcpy_r(playload + i, &(nomalOAD->classtag), sizeof(uint16));//�������ʶ
    }
    i += 2;

    /* ���ݻ��� */
    memcpy(playload + i, bufLen, bytes);
    i += bytes;
    memcpy(playload + i, pData, dataLen);
    i += dataLen;
    
    ret = func(label, DB_IOP_WRITE_NORMAL_DATA, playload, msglen, index);

    free(playload);
    return ret;
}

/**********************************************************************
* @name      : send_read_nomal_datas
* @brief     �����Ͳ�ѯ�����ͨ������Ϣ
* @param[in] ��DBMSG_SEND_FUN func     ���ͻص�����
               uint16 label            ��Ϣlabel
               NOMAL_OADS_T *nomalOADs oad�б�
* @param[out]��uint16 *index           ������Ϣ��index
* @return    ��������
* @Create    : ����
* @Date      ��2020-4-28
* @Update    : 
**********************************************************************/
int send_read_nomal_datas(DBMSG_SEND_FUN func, uint16 label, NOMAL_OADS_T *nomalOADs, uint16 *index)
{
    int    ret = 0;
    uint16 msglen = 0;
    uint16 offset = 0;
    uint8 *playload = NULL;
    uint8  i = 0;

    /* ���㳤�� */
    msglen = 1 + 2 + 1 + 4 * nomalOADs->num;
    playload = (uint8 *)malloc(msglen);
    if(NULL == playload)
    {
        return DBMSG_ERR_MALLOC;
    }
    memset(playload, 0, msglen);

    playload[offset++] = nomalOADs->logicId;                                  //�߼��豸��
    appmsg_memcpy_r(playload + offset, &(nomalOADs->infoNum), sizeof(uint16));//��Ϣ���
    offset += 2;
    playload[offset++] = nomalOADs->num;                                      //����
    
    for(i = 0; i < nomalOADs->num; i++)
    {
        appmsg_memcpy_r(playload + offset, &(nomalOADs->oad[i]), sizeof(OOP_OAD_U)); //�������ʶ
        offset += sizeof(OOP_OAD_U);
    }
    ret = func(label, DB_IOP_READ_NORMAL_DATA, playload, msglen, index);

    free(playload);
    return ret;
}

/**********************************************************************
* @name      : send_write_nomal_datas
* @brief     ������д�����ͨ������Ϣ
* @param[in] ��DBMSG_SEND_FUN func       ���ͻص�����
               uint16 label              ��Ϣlabel
               uint8 logic               �߼��豸��
               uint16 info               ��Ϣ���
               NOMAL_OAD_DATE_T *inDatas ��������
               uint8 inDataNum           ��������
* @param[out]��uint16 *index             ������Ϣ��index
* @return    ��������
* @Create    : ����
* @Date      ��2020-4-28
* @Update    :
**********************************************************************/
int send_write_nomal_datas(DBMSG_SEND_FUN func, uint16 label, uint8 logic, uint16 info, NOMAL_OAD_DATE_T *inDatas, 
                           uint8 inDataNum, uint16 *index)
{
    int    ret = 0;
    uint16 msglen = 0;
    uint8  i = 0;
    uint16 offset = 0;
    uint8 *playload = NULL;
    uint8  bufLen[3] = {0};
    uint8  bytes = 0;
    uint16 tempclass = CLASS_DATA_NORMAL;

    msglen = 1 + 2 + 1;
    for(i = 0; i < inDataNum; i++)
    {
        bytes = appmsg_set_len_offset(inDatas[i].datalen, bufLen);
        msglen += 4 + 2 + bytes + inDatas[i].datalen;
    }
    
    playload = (uint8 *)malloc(msglen);
    if(NULL == playload)
    {
        return DBMSG_ERR_MALLOC;
    }
    memset(playload, 0, msglen);

    playload[offset++] = logic;                                   //�߼��豸��
    appmsg_memcpy_r(playload + offset, &(info), sizeof(uint16));  //��Ϣ���
    offset += 2;
    playload[offset++] = inDataNum;                               //��ͨ��������

    for(i = 0; i < inDataNum; i++)
    {
        appmsg_memcpy_r(playload + offset, &(inDatas[i].oad), 4); //�������ʶ
        offset += 4;

        tempclass = (0 == inDatas[i].classtag) ? CLASS_DATA_NORMAL : inDatas[i].classtag;
        appmsg_memcpy_r(playload + offset, &tempclass, 2); //�������ʶ
        offset += 2;

        bytes = appmsg_set_len_offset(inDatas[i].datalen, bufLen);
        memcpy(playload + offset, bufLen, bytes);
        offset += bytes;
        memcpy(playload + offset, inDatas[i].data, inDatas[i].datalen);
        offset += inDatas[i].datalen;
    }
    
    ret = func(label, DB_IOP_WRITE_NORMAL_DATA, playload, msglen, index);

    free(playload);
    return ret;
}

/**********************************************************************
* @name      : start_record_get_len
* @brief     ����ʼ����¼�����ݳ��Ȼ�ȡ
* @param[in] ��READ_RECORD_T *startRecord ��¼������ˢѡ����
* @param[out]��
* @return    ������
* @Create    : ����
* @Date      ��2019-12-27
* @Update    :
**********************************************************************/
uint16 start_record_get_len(READ_RECORD_T *startRecord)
{
    uint16 len = 0;
    uint8 offset = 0;
    uint8 lenBuf[3] = {0};

    /*     �߼��豸��  ��Ϣ���   ɸѡ��ʽ    ����ʽ  ��OPTIONAL */
    len = 0x01    + 0x02    + 0x01    + 0x01   + 0x01;

    if(COL_NOT != startRecord->cType)
    {
        /*      ʱ����ʼ��   ʱ�������*/
        len += 0x04   +   0x04;
    }
    //��OAD��0,ǿ����Ч
    if(1 == startRecord->recordOAD.optional || 0 != startRecord->recordOAD.road.oadMain.value)
    {
        len += 0x04;   //������DI
    }

    //���ַ
    if(FALSE != startRecord->bTable)
    {
        len += 1 + startRecord->table.nNum;
    }
    
    offset = appmsg_set_len_offset(startRecord->recordOAD.road.oadCols.nNum, lenBuf);
    len += offset;                                                         //������DI��������
    len += sizeof(uint32) * startRecord->recordOAD.road.oadCols.nNum;      //���д�����DI���ݳ���
    return len;
}

/**********************************************************************
* @name      : send_read_record_start
* @brief     �����Ϳ�ʼ����¼������
* @param[in] ��DBMSG_SEND_FUN func        ���ͻص�����
               uint16 label               ��Ϣlabel
               READ_RECORD_T *startRecord ��¼������ˢѡ����
* @param[out]��uint16 *index              ������Ϣ��index
* @return    ��������
* @Create    : ����
* @Date      ��2019-12-27
* @Update    :
**********************************************************************/
int send_read_record_start(DBMSG_SEND_FUN func, uint16 label, READ_RECORD_T *startRecord, uint16 *index)
{
    int    ret = 0;
    uint16 msglen = 0;
    uint16 i = 0;
    uint8  j = 0;
    uint8 *playload = NULL;
    uint8  bufLen[3] = {0};
    uint8  bytes = 0;

    msglen = start_record_get_len(startRecord);
    playload = (uint8 *)malloc(msglen);
    if(NULL == playload)
    {
        return DBMSG_ERR_MALLOC;
    }
    memset(playload, 0, msglen);
    
    playload[i++] = startRecord->recordOAD.logicId;                                 //�߼��豸��
    
    appmsg_memcpy_r(playload + i, &startRecord->recordOAD.infoNum, sizeof(uint16)); //��Ϣ���
    i += sizeof(uint16);

    if(FALSE != startRecord->bTable)                                                //���ַ
    {
        playload[i++] = startRecord->table.nNum;
        if(startRecord->table.nNum > 0)
        {
            memcpy(playload + i, startRecord->table.value, startRecord->table.nNum);
            i += startRecord->table.nNum;
        }
    }

    playload[i++] = startRecord->cType;                                             //ɸѡ��ʽ

    if(COL_NOT != startRecord->cType)
    {
        appmsg_memcpy_r(playload + i, &startRecord->cStart, sizeof(uint32));       //ɸѡ���俪ʼ
        i += sizeof(uint32);

        appmsg_memcpy_r(playload + i, &startRecord->cEnd, sizeof(uint32));        //ɸѡ�������
        i += sizeof(uint32);
    }
    playload[i++] = startRecord->sortType;                                        //����ʽ

    /* ������DI ��OAD��0,ǿ����Ч */
    if(1 == startRecord->recordOAD.optional || 0 != startRecord->recordOAD.road.oadMain.value)
    {
        playload[i++] = 1;
        appmsg_memcpy_r(playload + i, &startRecord->recordOAD.road.oadMain.value, sizeof(uint32));
        i += sizeof(uint32);
    }
    else
    {
        playload[i++] = 0;
    }

    /* ������DI */
    bytes = appmsg_set_len_offset(startRecord->recordOAD.road.oadCols.nNum, bufLen);
    memcpy(playload + i, bufLen, bytes);
    i += bytes;
    for(j = 0; j < startRecord->recordOAD.road.oadCols.nNum; j++)
    {
        appmsg_memcpy_r(playload + i, &(startRecord->recordOAD.road.oadCols.oad[j].value), 4);
        i += 4;
    }

    if(FALSE == startRecord->bTable)
    {
        ret = func(label, DB_IOP_READ_RECORD_START, playload, msglen, index);
    }
    else
    {
        ret = func(label, DB_IOP_READ_RECORD_START_FJ, playload, msglen, index);
    }
    free(playload);
    return ret;
}

/**********************************************************************
* @name      : send_read_record_step
* @brief     �����͵�������¼������
* @param[in] ��DBMSG_SEND_FUN func            ���ͻص�����
               uint16 label                   ��Ϣlabel
               uint32 handle                  �������ľ��
               STEP_READ_RECORD_T *stepRecord ��������
* @param[out]��uint16 *index                  ������Ϣ��index
* @return    ��������
* @Create    : ����
* @Date      ��2019-12-27
* @Update    :
**********************************************************************/
int send_read_record_step(DBMSG_SEND_FUN func, uint16 label, uint32 handle, STEP_READ_RECORD_T *stepRecord, uint16 *index)
{
    int    ret = 0;
    uint16 msglen = 0;
    uint16 i = 0;
    uint8 *playload = NULL;
    uint8  bufLen[3] = {0};
    uint8  bytes = 0;

    /*        ���ݿ���           ����У��OPTIONAL */
    msglen = sizeof(uint32) + 1;
    
    if(0 != stepRecord->MAC.nNum)
    {
        bytes = appmsg_set_len_offset(stepRecord->MAC.nNum, bufLen);
        msglen += bytes + stepRecord->MAC.nNum;
    }

    playload = (uint8 *)malloc(msglen);
    if(NULL == playload)
    {
        return DBMSG_ERR_MALLOC;
    }
    memset(playload, 0, msglen);

    appmsg_memcpy_r(playload, &handle, sizeof(uint32));    //���ݿ���
    i += sizeof(uint32);

    /* ����У�� */
    if(0 != stepRecord->MAC.nNum)
    {
        playload[i++] = 1;
        memcpy(playload + i, bufLen, bytes);
        i += bytes;
        memcpy(playload + i, stepRecord->MAC.value, stepRecord->MAC.nNum);
        i += stepRecord->MAC.nNum;
    }
    else
    {
        playload[i++] = 0;
    }
    
    ret = func(label, DB_IOP_READ_RECORD_STEP, playload, msglen, index);
    free(playload);
    return ret;
}

/**********************************************************************
* @name      : send_read_record_end
* @brief     �����ͽ�����¼�����ݶ�ȡ
* @param[in] ��DBMSG_SEND_FUN func            ���ͻص�����
               uint16 label                   ��Ϣlabel
               uint32 handle                  �������ľ��
* @param[out]��uint16 *index                  ������Ϣ��index
* @return    ��������
* @Create    : ����
* @Date      ��2019-12-27
* @Update    :
**********************************************************************/
int send_read_record_end(DBMSG_SEND_FUN func, uint16 label, uint32 handle, uint16 *index)
{
    uint8  playload[4] = {0};
    
    appmsg_memcpy_r(playload, &handle, sizeof(uint32));
    return func(label, DB_IOP_READ_RECORD_END, playload, sizeof(uint32), index);
}

/**********************************************************************
* @name      : send_write_record
* @brief     ������д��¼������
* @param[in] ��DBMSG_SEND_FUN func             ���ͻص�����
               uint16 label                   ��Ϣlabel
               WRITE_RECORD_T *record         ��¼����
* @param[out]��uint16 *index                   ������Ϣ��index
* @return    ��������
* @Create    : ����
* @Date      ��2019-12-27
* @Update    : 2022-6-1
**********************************************************************/
int send_write_record(DBMSG_SEND_FUN func, uint16 label, WRITE_RECORD_T *record, uint16 *index)
{
    int    ret = 0;
    uint16 msglen = 0;
    uint16 i = 0;
    uint8 *playload = NULL;
    uint8  bufLen[3] = {0};
    uint8  bytes = 0;

    /*      �߼��豸��, ��ʼʱ��, ����ʱ��, �洢ʱ��, ��Ϣ��, ������, OPTIONAL */
    msglen = 0x01       + 0x04    + 0x04   + 0x4      + 0x2   + 0x2   + 0x1;
    
    /* ��OAD��0,ǿ����Ч */
    if(1 == record->recordOAD.optional || 0 != record->recordOAD.road.oadMain.value)
    {
        msglen += 0x04;    //��OAD
    }

    /* ������ */
    msglen += 1 + 0x04;

    /* ������ */
    bytes = appmsg_set_len_offset(record->inDatalen, bufLen);
    msglen += 1 + bytes + record->inDatalen;

    //����У��
    bytes = appmsg_set_len_offset(record->MAC.nNum, bufLen);
    msglen += bytes + record->MAC.nNum;
    
    playload = (uint8 *)malloc(msglen);
    if(NULL == playload)
    {
        return DBMSG_ERR_MALLOC;
    }
    memset(playload, 0, msglen);
    
    playload[i++] = record->recordOAD.logicId;  //�߼���ַ
    appmsg_memcpy_r(playload + i, &(record->colStartTm), sizeof(uint32));        //��ʼʱ��
    i += 4;
    
    appmsg_memcpy_r(playload + i, &(record->colEndTm), sizeof(uint32));          //����ʱ��
    i += 4;
    
    appmsg_memcpy_r(playload + i, &(record->colStoreTm), sizeof(uint32));         //�洢ʱ��
    i += 4;
    
    appmsg_memcpy_r(playload + i, &(record->recordOAD.infoNum), sizeof(uint16));  //��Ϣ��
    i += 2;

    /* �������ʶ */
    if(0 == record->recordOAD.classtag)
    {
        uint16 tempclass = CLASS_DATA_NORMAL;
        appmsg_memcpy_r(playload + i, &tempclass, sizeof(uint16));//�������ʶ Ϊ0����д��CLASS_DATA_NORMAL
    }
    else
    {
        appmsg_memcpy_r(playload + i, &(record->recordOAD.classtag), sizeof(uint16));//�������ʶ
    }
    i += 2;

    /* ��DI */
    if(1 == record->recordOAD.optional || 0 != record->recordOAD.road.oadMain.value)
    {
        playload[i++] = 1;
        appmsg_memcpy_r(playload + i, &(record->recordOAD.road.oadMain.value), sizeof(uint32));
        i += sizeof(uint32);
    }
    else
    {
        playload[i++] = 0;
    }

    /* ��DI */
    playload[i++] = 1;
    appmsg_memcpy_r(playload + i, &(record->recordOAD.road.oadCols.oad[0]), sizeof(uint32));//������
    i += 4;

    /* ���ݻ��� */
    playload[i++] = 1;
    bytes = appmsg_set_len_offset(record->inDatalen, bufLen);
    memcpy(playload + i,bufLen, bytes); 
    i += bytes;
    memcpy(playload + i, record->inDataBuf, record->inDatalen);
    i += record->inDatalen;
    /* ����У�� */
    bytes = appmsg_set_len_offset(record->MAC.nNum, bufLen);
    memcpy(playload + i, bufLen, bytes);
    i += bytes;
    memcpy(playload + i, record->MAC.value, record->MAC.nNum);
    i += record->MAC.nNum;

    ret = func(label, DB_IOP_WRITE_RECORD, playload, msglen, index);
    free(playload);
    return ret;
}

/**********************************************************************
* @name      : send_write_records
* @brief     ������д��¼������
* @param[in] ��DBMSG_SEND_FUN func             ���ͻص�����
               uint16 label                   ��Ϣlabel
               WRITE_RECORD_T *record         ��¼����
* @param[out]��uint16 *index                   ������Ϣ��index
* @return    ��������
* @Create    : ����
* @Date      ��2022-6-1
* @Update    :
**********************************************************************/
int send_write_records(DBMSG_SEND_FUN func, uint16 label, WRITE_RECORDS_T *record, uint16 *index)
{
    int    ret = 0;
    uint16 msglen = 0;
    uint16 i = 0;
    uint16 j = 0;
    uint8 *playload = NULL;
    uint8  bufLen[3] = {0};
    uint8  bytes = 0;

    /*     �߼��豸�� ��ʼʱ�� ����ʱ�� �洢ʱ�� ��Ϣ��� �������ʶ OPTIONAL */
    msglen = 0x01   + 0x04  +  0x04  +  0x04  +  0x02 +   0x2+       0x01 ;
    
    /* ��OAD��0,ǿ����Ч */
    if(1 == record->recordOAD.optional || 0 != record->recordOAD.road.oadMain.value)
    {
        msglen += 0x04;    //��OAD
    }

    /* ���ݼ������� */
    bytes = appmsg_set_len_offset(record->recordOAD.road.oadCols.nNum, bufLen);
    msglen += bytes;
    //������Ҳ��sequence
    msglen += bytes;
    for(j = 0; j < record->recordOAD.road.oadCols.nNum; j++)
    {
        //octet-string�ĳ��� 
        bytes = appmsg_set_len_offset(record->datas[j].len, bufLen);
        msglen += 4 + bytes + record->datas[j].len;
    }

    //����У��
    bytes = appmsg_set_len_offset(record->MAC.nNum, bufLen);
    msglen += bytes + record->MAC.nNum;
    
    playload = (uint8 *)malloc(msglen);
    if(NULL == playload)
    {
        return DBMSG_ERR_MALLOC;
    }
    memset(playload, 0, msglen);

    i = 0;
    playload[i++] = record->recordOAD.logicId;  //�߼���ַ

    appmsg_memcpy_r(playload + i, &(record->colStartTm), sizeof(uint32));        //��ʼʱ��
    i += 4;

    appmsg_memcpy_r(playload + i, &(record->colEndTm), sizeof(uint32));          //����ʱ��
    i += 4;

    appmsg_memcpy_r(playload + i, &(record->colStoreTm), sizeof(uint32));         //�洢ʱ��
    i += 4;

    appmsg_memcpy_r(playload + i, &(record->recordOAD.infoNum), sizeof(uint16));  //��Ϣ��
    i += 2;

    if(0 == record->recordOAD.classtag)
    {
        uint16 tempclass = CLASS_DATA_NORMAL;
        appmsg_memcpy_r(playload + i, &tempclass, sizeof(uint16));//�������ʶ Ϊ0����д��CLASS_DATA_NORMAL
    }
    else
    {
        appmsg_memcpy_r(playload + i, &(record->recordOAD.classtag), sizeof(uint16));//�������ʶ
    }
    i += 2;

    /* ��DI */
    if(1 == record->recordOAD.optional || 0 != record->recordOAD.road.oadMain.value)
    {
        playload[i++] = 1;
        appmsg_memcpy_r(playload + i, &(record->recordOAD.road.oadMain.value), sizeof(uint32));
        i += sizeof(uint32);
    }
    else
    {
        playload[i++] = 0;
    }

    /* ���� */
    bytes = appmsg_set_len_offset(record->recordOAD.road.oadCols.nNum, bufLen);
    memcpy(playload + i, bufLen, bytes);
    i += bytes;
    
    for(j = 0; j < record->recordOAD.road.oadCols.nNum; j++)
    {
        appmsg_memcpy_r(playload + i, &(record->recordOAD.road.oadCols.oad[j]), sizeof(uint32));//������
        i += 4;
    }
    
    bytes = appmsg_set_len_offset(record->recordOAD.road.oadCols.nNum, bufLen);
    memcpy(playload + i, bufLen, bytes);
    i += bytes;
    for(j = 0; j < record->recordOAD.road.oadCols.nNum; j++)
    {
        /* ���ݻ��� */
        bytes = appmsg_set_len_offset(record->datas[j].len, bufLen);
        memcpy(playload + i, bufLen, bytes);
        i += bytes;
        memcpy(playload + i, record->datas[j].data, record->datas[j].len);
        i += record->datas[j].len;
    }
    /* ����У�� */
    bytes = appmsg_set_len_offset(record->MAC.nNum, bufLen);
    memcpy(playload + i, bufLen, bytes);
    i += bytes;
    memcpy(playload + i, record->MAC.value, record->MAC.nNum);
    i += record->MAC.nNum;

    ret = func(label, DB_IOP_WRITE_RECORD, playload, msglen, index);
    free(playload);
    return ret;
}

/**********************************************************************
* @name      : update_record_get_len
* @brief     �����¼�¼���ݳ��Ȼ�ȡ
* @param[in] ��UPDATE_RECORD_T *pData   ��¼����
* @param[out]��
* @return    ������
* @Create    : ����
* @Date      ��2019-12-27
* @Update    :
**********************************************************************/
uint16 update_record_get_len(UPDATE_RECORD_T *pData)
{
    uint16 len = 0;
    uint8  bufLen[3] = {0};
    uint8  bytes = 0;

    if(1 == pData->filter.choice || 2 == pData->filter.choice)
    {
        /*    �߼��豸��   OPTIONAL  ��OAD      ˢѡ��ʽ  ˢѡ����   ��Ϣ���*/
        len = 0x01    + 0x01  +  0x04 +   0x01 +   0x04   +  0x02;
    }
    else
    {
        return 0;
    }

    if(1 == pData->recordOAD.optional || 0 != pData->recordOAD.road.oadMain.value)
    {
        len += 0x4;
    }

    /* ����У�� */
    bytes = appmsg_set_len_offset(pData->MAC.nNum, bufLen);
    len += bytes + pData->MAC.nNum;
    
    //octet-string�ĳ���
    bytes = appmsg_set_len_offset(pData->inDatalen, bufLen);
    len += bytes + pData->inDatalen;          
    return len;
}

/**********************************************************************
* @name      : send_write_record
* @brief     �����͸��¼�¼����
* @param[in] ��DBMSG_SEND_FUN func            ���ͻص�����
               uint16 label                   ��Ϣlabel
               UPDATE_RECORD_T *pData         ��¼����
* @param[out]��uint16 *index                  ������Ϣ��index
* @return    ��������
* @Create    : ����
* @Date      ��2019-12-27
* @Update    :
**********************************************************************/
int send_update_record(DBMSG_SEND_FUN func, uint16 label, UPDATE_RECORD_T *pData, uint16 *index)
{
    int    ret = 0;
    uint16 msglen = 0;
    uint16 i = 0;
    uint8 *playload = NULL;
    uint8  bufLen[3] = {0};
    uint8  bytes = 0;

    msglen = update_record_get_len(pData);
    if(0 == msglen)
    {
        return DBMSG_ERR_INPUT;
    }
    playload = (uint8 *)malloc(msglen);
    if(NULL == playload)
    {
        return DBMSG_ERR_MALLOC;
    }
    memset(playload, 0, msglen);

    playload[i++] = pData->recordOAD.logicId;                                              //�߼��豸��

    if(1 == pData->recordOAD.optional || 0 != pData->recordOAD.road.oadMain.value)
    {
        playload[i++] = 1;
        appmsg_memcpy_r(playload + i, &(pData->recordOAD.road.oadMain.value), sizeof(uint32)); //������DI
        i += 4;
    }
    else
    {
        playload[i++] = 0;
    }

    /* ������DI */
    appmsg_memcpy_r(playload + i, &(pData->recordOAD.road.oadCols.oad[0]), sizeof(uint32));
    i += 4;

    /* ɸѡ��ʽ */
    playload[i++] = pData->filter.choice;
    if(pData->filter.choice == 1)
    {
        appmsg_memcpy_r(playload + i, &(pData->filter.storeTm), sizeof(uint32));
        i += 4;
    }
    else if(pData->filter.choice == 2)
    {
        appmsg_memcpy_r(playload + i, &(pData->filter.storeNo), sizeof(uint32));
        i += 4;
    }
    else
    {
        free(playload);
        return DBMSG_ERR_INPUT;
    }

    appmsg_memcpy_r(playload + i, &(pData->recordOAD.infoNum), sizeof(uint16));   //��Ϣ���
    i += 2;

    /* ����У�� */
    bytes = appmsg_set_len_offset(pData->MAC.nNum, bufLen);
    memcpy(playload + i, bufLen, bytes);
    i += bytes;
    memcpy(playload + i, pData->MAC.value, pData->MAC.nNum);
    i += pData->MAC.nNum;

    /* ���ݻ��� */
    bytes = appmsg_set_len_offset(pData->inDatalen, bufLen);
    memcpy(playload + i, bufLen, bytes);
    i += bytes;
    memcpy(playload + i, pData->inDataBuf, pData->inDatalen);

    ret = func(label, DB_IOP_UPDATE_RECORD, playload, msglen, index);
    free(playload);
    return ret;
}

/**********************************************************************
* @name      : send_write_record
* @brief     �����ͻ�ȡ�洢��¼����
* @param[in] ��DBMSG_SEND_FUN func            ���ͻص�����
               uint16 label                   ��Ϣlabel
               GET_RECORD_NUM_T *pRecord      ɾѡ����
* @param[out]��uint16 *index                  ������Ϣ��index
* @return    ��������
* @Create    : ����
* @Date      ��2019-12-27
* @Update    :
**********************************************************************/
int send_get_record_num(DBMSG_SEND_FUN func, uint16 label, GET_RECORD_NUM_T *pRecord, uint16 *index)
{
    int    ret = 0;
    uint16 msglen = 0;
    uint16 i = 0;
    uint8 *playload = NULL;

    /*        �߼��豸�� ��Ϣ��� OPTIONAL  ��OAD      ˢѡ��ʽ */
    msglen = 1   +    2     + 1   +    4    +     1 ;

    if(1 == pRecord->recordOAD.optional || 0 != pRecord->recordOAD.road.oadMain.value)
    {
        msglen += 4;  //��OAD
    }
    
    if(GET_INTER_RECORD == pRecord->filter)
    {
        msglen += 8;  //����
    }

    playload = (uint8 *)malloc(msglen);
    if(NULL == playload)
    {
        return DBMSG_ERR_MALLOC;
    }
    memset(playload, 0, msglen);

    playload[i++] = pRecord->recordOAD.logicId;   //�߼��豸��
    
    appmsg_memcpy_r(playload + i, &(pRecord->recordOAD.infoNum), sizeof(uint16)); //��Ϣ���
    i += 2;

    if(1 == pRecord->recordOAD.optional || 0 != pRecord->recordOAD.road.oadMain.value)
    {
        playload[i++] = 1;
        appmsg_memcpy_r(playload + i, &(pRecord->recordOAD.road.oadMain.value), sizeof(uint32));//������DI
        i += 4;
    }
    else
    {
        playload[i++] = 0;
    }

    /* ������DI */
    appmsg_memcpy_r(playload + i, &(pRecord->recordOAD.road.oadCols.oad[0]), sizeof(uint32));
    i += 4;

    playload[i++] = pRecord->filter;   //ɸѡ��ʽ

    if(GET_INTER_RECORD == pRecord->filter)
    {
        appmsg_memcpy_r(playload + i, &(pRecord->gStart), sizeof(uint32));
        i += 4;
        appmsg_memcpy_r(playload + i, &(pRecord->gEnd), sizeof(uint32));
    }

    ret = func(label, DB_IOP_GET_RECORD_NUM, playload, msglen, index);
    free(playload);
    return ret;
}

/**********************************************************************
* @name      : send_get_record_depth
* @brief     �����ͻ�ȡ�洢��¼���
* @param[in] ��DBMSG_SEND_FUN func            ���ͻص�����
               uint16 label                   ��Ϣlabel
               OOP_ROAD_T *pRoad              road
* @param[out]��uint16 *index                  ������Ϣ��index
* @return    ��������
* @Create    : ����
* @Date      ��2019-12-27
* @Update    :
**********************************************************************/
int send_get_record_depth(DBMSG_SEND_FUN func, uint16 label, RECORD_OAD_T *pRoad, uint16 *index)
{
    int    ret = 0;
    uint16 msglen = 0;
    uint16 i = 0;
    uint8 *playload = NULL;

    /*       OPTIONAL  ��OAD*/
    msglen =0x01 +0x01 +   0x04;
    
    if(1 == pRoad->optional || 0 != pRoad->road.oadMain.value)
    {
        msglen += 0x04; //��OAD
    }

    playload = (uint8 *)malloc(msglen);
    if(NULL == playload)
    {
        return DBMSG_ERR_MALLOC;
    }
    memset(playload, 0, msglen);
	
	playload[i++] = pRoad->logicId;
    /* ��OAD */
    if(1 == pRoad->optional || 0 != pRoad->road.oadMain.value)
    {
        playload[i++] = 1;
        appmsg_memcpy_r(playload + i, &(pRoad->road.oadMain.value), sizeof(uint32));
        i += 4;
    }
    else
    {
        playload[i++] = 0;
    }

    appmsg_memcpy_r(playload + i, &(pRoad->road.oadCols.oad[0]), sizeof(uint32)); //��OAD

    ret = func(label, DB_IOP_GET_RECORD_DEPTH, playload, msglen, index);
    free(playload);
    return ret;
}

/**********************************************************************
* @name      : send_clear_data
* @brief     ���������ݳ�ʼ��
* @param[in] ��DBMSG_SEND_FUN func            ���ͻص�����
               uint16 label                   ��Ϣlabel
               CLEAR_DATA_T *ptData           �������ѡ��
* @param[out]��uint16 *index                  ������Ϣ��index
* @return    ��������
* @Create    : ����
* @Date      ��2019-12-27
* @Update    : 2020-1-20
**********************************************************************/
int send_clear_data(DBMSG_SEND_FUN func, uint16 label, CLEAR_DATA_T *ptData, uint16 *index)
{
    int    ret = 0;
    uint16 msglen = 0;
    uint16 i = 0;
    uint8 *playload = NULL;

    msglen = 1 + 1;
    if(TRUE == ptData->bClassTag)
    {
        msglen += 2;
    }

    playload = (uint8 *)malloc(msglen);
    if(NULL == playload)
    {
        return DBMSG_ERR_MALLOC;
    }
    memset(playload, 0, msglen);

    playload[i++] = ptData->recordOAD.logicId;
    playload[i++] = ptData->bClassTag;
    if(TRUE == ptData->bClassTag)
    {
        appmsg_memcpy_r(playload + i, &(ptData->recordOAD.classtag), sizeof(uint16));
        i += 2;
    }
    
    ret = func(label, DB_IOP_INIT_DATA, playload, msglen, index);
    free(playload);
    return ret;
}

/**********************************************************************
* @name      : send_clear_data
* @brief     ���������ݳ�ʼ��
* @param[in] ��DBMSG_SEND_FUN func            ���ͻص�����
               uint16 label                   ��Ϣlabel
               RECORD_OAD_T *pRecord          OAD
* @param[out]��uint16 *index                  ������Ϣ��index
* @return    ��������
* @Create    : ����
* @Date      ��2019-12-27
* @Update    :
**********************************************************************/
int send_clear_oad_data(DBMSG_SEND_FUN func, uint16 label, CLEAR_DATA_T *ptData, uint16 *index)
{
    int    ret = 0;
    uint16 msglen = 0;
    uint16 i = 0;
    uint8 *playload = NULL;
	
	//		�߼��豸�� ��Ϣ���
    msglen = 	1 		+ 2;
	
	if(FALSE == ptData->bFilter)
    {
    	msglen += 2;	//��������
        msglen += 1;	//OPTIONAL
        if(TRUE != ptData->bClassTag)
	    {
	        if(1 == ptData->recordOAD.optional || 0 != ptData->recordOAD.road.oadMain.value)
	        {
	            /*         OPTIONAL  ��OAD   ��OAD */
	            msglen += 0x01  + 0x04 + 0x04;
	        }
	        else
	        {
	            /*         OPTIONAL   ��OAD */
	            msglen += 0x01  +  0x04;
	        }
	    }
    }
    else
    {
    	if(1 == ptData->recordOAD.optional || 0 != ptData->recordOAD.road.oadMain.value)
        {
            /*         OPTIONAL  ��OAD   ��OAD */
            msglen += 0x01  + 0x04 + 0x04;
        }
        else
        {
            /*         OPTIONAL   ��OAD */
            msglen += 0x01  +  0x04;
        }
		
        if (0 == ptData->filterType)
        {
            msglen += 9;
        }
        else if (255 == ptData->filterType)
        {
            msglen += 1;
        }
        else
        {
            return DBMSG_ERR_UNKNOW;
        }
    }
	    

    playload = (uint8 *)malloc(msglen);
    if(NULL == playload)
    {
        return DBMSG_ERR_MALLOC;
    }
    memset(playload, 0, msglen);

    playload[i++] = ptData->recordOAD.logicId;                                    //�߼��豸��
    appmsg_memcpy_r(playload + i, &(ptData->recordOAD.infoNum), sizeof(uint16));  //��Ϣ���
    i += 2;
    
	if(FALSE == ptData->bFilter)
    {
    	appmsg_memcpy_r(playload + i, &(ptData->recordOAD.classtag), sizeof(uint16));  //�������ʶ
    	i += 2;
		
		if(TRUE == ptData->bClassTag)
	    {
	        playload[i++] = 0;
	    }
	    else
	    {
	        playload[i++] = 1;
	        
	        if(1 == ptData->recordOAD.optional || 0 != ptData->recordOAD.road.oadMain.value)
	        {
	            playload[i++] = 1;
	            appmsg_memcpy_r(playload + i, &(ptData->recordOAD.road.oadMain.value), sizeof(uint32));
	            i += 4;
	        }
	        else
	        {
	            playload[i++] = 0;
	        }
	        
	        appmsg_memcpy_r(playload + i, &(ptData->recordOAD.road.oadCols.oad[0].value), sizeof(uint32));
	    }
    }
    else
    {
        if(1 == ptData->recordOAD.optional || 0 != ptData->recordOAD.road.oadMain.value)
        {
            playload[i++] = 1;
            appmsg_memcpy_r(playload + i, &(ptData->recordOAD.road.oadMain.value), sizeof(uint32));
            i += 4;
        }
        else
        {
            playload[i++] = 0;
        }
        
        appmsg_memcpy_r(playload + i, &(ptData->recordOAD.road.oadCols.oad[0].value), sizeof(uint32));
        i += 4;

        if (0 == ptData->filterType)
        {
            playload[i++] = 0;
            
            appmsg_memcpy_r(playload + i, &(ptData->startFilter), sizeof(uint32));
            i += 4;
            appmsg_memcpy_r(playload + i, &(ptData->endFilter), sizeof(uint32));
            i += 4;
        }
        else if (255 == ptData->filterType)
        {
            playload[i++] = 255;
        }
    }

    ret = func(label, FALSE == ptData->bFilter ? DB_IOP_CLEAR_ONE_DATA : DB_IOP_CLEAR_ONE_DATA_FJ, playload, msglen, index);
    free(playload);
	    
    return ret;
}

/**********************************************************************
* @name      : send_recover_default
* @brief     �����ͻָ�����
* @param[in] ��DBMSG_SEND_FUN func            ���ͻص�����
               uint16 label                   ��Ϣlabel
               uint8 logicId                  �߼��豸��
* @param[out]��uint16 *index                  ������Ϣ��index
* @return    ��������
* @Create    : ����
* @Date      ��2019-12-27
* @Update    :
**********************************************************************/
int send_recover_default(DBMSG_SEND_FUN func, uint16 label, uint8 logicId, uint16 *index)
{
    return func(label, DB_IOP_RECOVERY_DEFAULT, &logicId, 1, index);
}

/**********************************************************************
* @name      : db_data_to_base64
* @brief     ����ͨ����תBase64��ʽ
* @param[in] ��uint8 *pData         ����
               uint32 datalen       ���ݳ���
* @param[out]��
* @return    ��base64���
* @Create    : ����
* @Date      ��2020-10-15
* @Update    :
**********************************************************************/
char *db_data_to_base64(uint8 *pData, uint32 datalen)
{
    uint32 base64len = 0;
    uint32 i = 0;
    uint32 j = 0;
    char   *base64 = NULL;
    //����base64�����  
    uint8 base64_table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

    if(NULL == pData || 0 == datalen)
    {
        return NULL;
    }
    //���㾭��base64�������ַ�������

    if(0 == datalen % 3)
    {
        base64len = (datalen / 3) * 4;
    }
    else
    {
        base64len = (datalen / 3 + 1) * 4;
    }
    
    base64 = (char *)malloc(base64len + 1);
    if(NULL == base64)
    {
        return NULL;
    }
    memset(base64, 0, base64len + 1);

    for(i = 0, j = 0; i < datalen && j + 3 < base64len; i += 3, j += 4)
    {
        base64[j] = base64_table[pData[i]>>2];
        if(i + 2 < datalen)
        {
            base64[j+1] = base64_table[((pData[i]&0x3)<<4) | (pData[i+1]>>4)];
            base64[j+2] = base64_table[((pData[i+1]&0xf)<<2) | (pData[i+2]>>6)];
            base64[j+3] = base64_table[pData[i+2]&0x3f];
        }
        else if(i + 1 < datalen)
        {
            base64[j+1] = base64_table[((pData[i]&0x3)<<4) | (pData[i+1]>>4)];
            base64[j+2] = base64_table[(pData[i+1]&0xf)<<2];
            base64[j+3] = '=';
        }
        else
        {
            base64[j+1] = base64_table[(pData[i]&0x3)<<4];
            base64[j+2] = '=';
            base64[j+3] = '=';
        }
    }

    return base64;
}

/**********************************************************************
* @name      : db_base64_to_data
* @brief     ��Base64��ʽר��ͨ����
* @param[in] ��uint8 *pBase         Base64����
               uint32 buflen        �����������С
* @param[out]��uint8 *pBuf          ����������
               uint32 *datalen      ������ʵ�ʳ���
* @return    ��0-�ɹ�/������
* @Create    : ����
* @Date      ��2020-10-15
* @Update    :
**********************************************************************/
int db_base64_to_data(char *pBase, uint8 *pBuf, uint32 buflen, uint32 *datalen)
{
    uint32 dataLen = 0;
    uint32 i = 0;
    uint32 j = 0;
    uint32 num = 0;
    uint8  temp[3] = {0};
    uint8  a0, a1, a2, a3;
    uint32 len = 0;

    //����base64�����ַ��ҵ���Ӧ��ʮ��������  
    uint8 table[]={ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,62, 0, 0, 0,63,
                   52,53,54,55,56,57,58,59,60,61, 0, 0, 0, 0, 0, 0,
                    0, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,
                   15,16,17,18,19,20,21,22,23,24,25, 0, 0, 0, 0, 0,
                    0,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,
                   41,42,43,44,45,46,47,48,49,50,51}; 
    
    if(NULL == pBase || 0 == strlen(pBase) || NULL == pBuf || 0 == buflen || NULL == datalen)
    {
        return DBMSG_ERR_INPUT;
    }

    len = strlen(pBase);
    if(0 != len % 4)
    {
        return DBMSG_ERR_INPUT;
    }

    if('=' == pBase[len-1] && '=' == pBase[len-2])
    {
        dataLen = (len / 4) *3 -2;
    }
    else if('=' == pBase[len-1])
    {
        dataLen = (len / 4) *3 -1;
    }
    else
    {
        dataLen = (len / 4) *3;
    }
    *datalen = dataLen;

    for(i = 0; i < len - 2; i += 4)
    {
        if(pBase[i] > 'z' || pBase[i+1] > 'z' || pBase[i+2] > 'z' || pBase[i+3] > 'z')
        {
            return DBMSG_ERR_INPUT;
        }
        a0 = pBase[i];
        a1 = pBase[i+1];
        a2 = pBase[i+2];
        a3 = pBase[i+3];

        temp[0] = ((table[a0] << 2) | (table[a1] >> 4)); 
        temp[1] = ((table[a1] << 4) | (table[a2] >> 2));
        temp[2] = ((table[a2] << 6) | table[a3]);
        for(j = 0; j < 3; j++)
        {
            if(num >= buflen)
            {
                return 0;
            }
            pBuf[num] = temp[j];
            num++;
        }
    }
    return DBMSG_SUCCESS;
}

