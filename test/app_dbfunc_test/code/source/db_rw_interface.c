/*
*********************************************************************
* @filename: dbCenter_msg.c
* @brief   : 消息通信 DB操作
* @describe: 
* @history: 
*    auther      date          describe
*    王津剑      2019-12-18    create
*********************************************************************
*/
#include "xlChannel.h"
#include "oopType.h"
#include "comm_log.h"
#include "appmsg_split.h"
#include "db_struct.h"
#include "mqtt_proc.h"
#include "errCode.h"
#include "db_rw_interface.h"
#include "framecomm.h"


#define DB_OPERATE_OUT_TIME      20
#define SMALL_MSG_BUF_LEN        128
//#define MAX_MSG_BUF_LEN          8192

#define DB_INTERFACE_OLD      0


/**********************************************************************
* @name      : send_read_nomal_data
* @brief     ：发送查询普通数据消息
* @param[in] ：DBMSG_SEND_FUN func    发送回调函数
               uint16 label           消息label
               NOMAL_OAD_T *nomalOAD  oad
* @param[out]：uint16 *index          发送消息的index
* @return    ：错误码
* @Create    : 王津剑
* @Date      ：2019-12-27
* @Update    : 2020-4-28
**********************************************************************/
int db_send_read_nomal_data(uint8 thread_id, uint16 label, NOMAL_OAD_T *nomalOAD, uint16 *index)
{
    uint16      bufLen = 0;
    uint8       playBuf[10] = {0};

    playBuf[bufLen++] = nomalOAD->logicId;                                  //逻辑设备号
    appmsg_memcpy_r(playBuf + bufLen, &(nomalOAD->infoNum), sizeof(uint16));//信息点号
    bufLen += 2;
#if DB_INTERFACE_OLD == 0
    playBuf[bufLen++] = 1;                                                  //数量
#endif
    appmsg_memcpy_r(playBuf + bufLen, &(nomalOAD->oad), sizeof(OOP_OAD_U)); //数据项标识
    bufLen += sizeof(OOP_OAD_U);

    return db_send_dbmsg(thread_id, label, DB_IOP_READ_NORMAL_DATA, playBuf, bufLen, index);
}

/**********************************************************************
* @name      : send_write_nomal_data
* @brief     ：发送写普通数据消息
* @param[in] ：DBMSG_SEND_FUN func    发送回调函数
               uint16 label           消息label
               NOMAL_OAD_T *nomalOAD  oad
               uint8 *pData           内容
               uint32 dataLen         长度
* @param[out]：uint16 *index          发送消息的index
* @return    ：错误码
* @Create    : 王津剑
* @Date      ：2019-12-27
* @Update    :
**********************************************************************/
int db_send_write_nomal_data(uint8 thread_id, uint16 label, NOMAL_OAD_T *nomalOAD, uint8 *pData, 
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
        return ERR_MEM;
    }
    memset(playload, 0, msglen);

    playload[i++] = nomalOAD->logicId;                                   //逻辑设备号
    appmsg_memcpy_r(playload + i, &(nomalOAD->infoNum), sizeof(uint16)); //信息点号
    i += 2;
#if DB_INTERFACE_OLD == 0
    playload[i++] = 1;                                                   //普通数据数量
#endif
    
    appmsg_memcpy_r(playload + i, &(nomalOAD->oad), sizeof(OOP_OAD_U));  //数据项标识
    i += sizeof(OOP_OAD_U);
    if(0 == nomalOAD->classtag)
    {
        uint16 tempclass = CLASS_DATA_NORMAL;
        appmsg_memcpy_r(playload + i, &tempclass, sizeof(uint16));//数据类标识 为0，填写成CLASS_DATA_NORMAL
    }
    else
    {
        appmsg_memcpy_r(playload + i, &(nomalOAD->classtag), sizeof(uint16));//数据类标识
    }
    i += 2;

    // 数据缓冲 
    memcpy(playload + i, bufLen, bytes);
    i += bytes;
    memcpy(playload + i, pData, dataLen);
    i += dataLen;
    
    ret = db_send_dbmsg(thread_id, label, DB_IOP_WRITE_NORMAL_DATA, playload, msglen, index);

    free(playload);
    return ret;
}

/**********************************************************************
* @name      : send_write_nomal_datas
* @brief     ：发送写多个普通数据消息
* @param[in] ：DBMSG_SEND_FUN func       发送回调函数
               uint16 label              消息label
               uint8 logic               逻辑设备号
               uint16 info               信息点号
               NOMAL_OAD_DATE_T *inDatas 多条数据
               uint8 inDataNum           数据数量
* @param[out]：uint16 *index             发送消息的index
* @return    ：错误码
* @Create    : 王津剑
* @Date      ：2020-4-28
* @Update    :
**********************************************************************/
int db_send_write_nomal_datas(uint8 thread_id, uint16 label, uint8 logic, uint16 info, NOMAL_OAD_DATE_T *inDatas, uint8 inDataNum, uint16 *index)
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
        return ERR_MEM;
    }
    memset(playload, 0, msglen);

    playload[offset++] = logic;                                   //逻辑设备号
    appmsg_memcpy_r(playload + offset, &(info), sizeof(uint16));  //信息点号
    offset += 2;
    playload[offset++] = inDataNum;                               //普通数据数量

    for(i = 0; i < inDataNum; i++)
    {
        appmsg_memcpy_r(playload + offset, &(inDatas[i].oad), 4); //数据项标识
        offset += 4;

        tempclass = (0 == inDatas[i].classtag) ? CLASS_DATA_NORMAL : inDatas[i].classtag;
        appmsg_memcpy_r(playload + offset, &tempclass, 2); //数据类标识
        offset += 2;

        bytes = appmsg_set_len_offset(inDatas[i].datalen, bufLen);
        memcpy(playload + offset, bufLen, bytes);
        offset += bytes;
        memcpy(playload + offset, inDatas[i].data, inDatas[i].datalen);
        offset += inDatas[i].datalen;
    }
    
    ret = db_send_dbmsg(thread_id, label, DB_IOP_WRITE_NORMAL_DATA, playload, msglen, index);

    free(playload);
    return ret;
}

/**********************************************************************
* @name      : start_record_get_len
* @brief     ：开始读记录型数据长度获取
* @param[in] ：READ_RECORD_T *startRecord 记录型数据刷选条件
* @param[out]：
* @return    ：长度
* @Create    : 王津剑
* @Date      ：2019-12-27
* @Update    :
**********************************************************************/
uint16 db_start_record_get_len(READ_RECORD_T *startRecord)
{

    uint16 len = 0;
    uint8 offset = 0;
    uint8 lenBuf[3] = {0};

    //    逻辑设备号  信息点号   筛选方式    排序方式  主OPTIONAL 
    len = 0x01    + 0x02    + 0x01    + 0x01   + 0x01;

    if(COL_NOT != startRecord->cType)
    {
        //      时间起始点   时间结束点
        len += 0x04   +   0x04;
    }
    //主OAD非0,强制有效
    if(1 == startRecord->recordOAD.optional || 0 != startRecord->recordOAD.road.oadMain.value)
    {
        len += 0x04;   //主数据DI
    }
    
    offset = appmsg_set_len_offset(startRecord->recordOAD.road.oadCols.nNum, lenBuf);
    len += offset;                                                         //次数据DI数量长度
    len += sizeof(uint32) * startRecord->recordOAD.road.oadCols.nNum;      //所有次数据DI内容长度
    return len;
    
}

/**********************************************************************
* @name      : send_read_record_start
* @brief     ：发送开始读记录型数据
* @param[in] ：DBMSG_SEND_FUN func        发送回调函数
               uint16 label               消息label
               READ_RECORD_T *startRecord 记录型数据刷选条件
* @param[out]：uint16 *index              发送消息的index
* @return    ：错误码
* @Create    : 王津剑
* @Date      ：2019-12-27
* @Update    :
**********************************************************************/
int db_send_read_record_start(uint8 thread_id, uint16 label, READ_RECORD_T *startRecord, uint16 *index)
{
    int    ret = 0;
    uint16 msglen = 0;
    uint16 i = 0;
    uint8  j = 0;
    uint8 *playload = NULL;
    uint8  bufLen[3] = {0};
    uint8  bytes = 0;

    msglen = db_start_record_get_len(startRecord);
    playload = (uint8 *)malloc(msglen);
    if(NULL == playload)
    {
        return ERR_MEM;
    }
    memset(playload, 0, msglen);
    
    playload[i++] = startRecord->recordOAD.logicId;                                 //逻辑设备号
    
    appmsg_memcpy_r(playload + i, &startRecord->recordOAD.infoNum, sizeof(uint16)); //信息点号
    i += sizeof(uint16);

    playload[i++] = startRecord->cType;                                             //筛选方式

    if(COL_NOT != startRecord->cType)
    {
        appmsg_memcpy_r(playload + i, &startRecord->cStart, sizeof(uint32));       //筛选区间开始
        i += sizeof(uint32);

        appmsg_memcpy_r(playload + i, &startRecord->cEnd, sizeof(uint32));        //筛选区间结束
        i += sizeof(uint32);
    }
    playload[i++] = startRecord->sortType;                                        //排序方式

    // 主数据DI 主OAD非0,强制有效 
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

    // 次数据DI 
    bytes = appmsg_set_len_offset(startRecord->recordOAD.road.oadCols.nNum, bufLen);
    memcpy(playload + i, bufLen, bytes);
    i += bytes;
    for(j = 0; j < startRecord->recordOAD.road.oadCols.nNum; j++)
    {
        appmsg_memcpy_r(playload + i, &(startRecord->recordOAD.road.oadCols.oad[j].value), 4);
        i += 4;
    }

    ret = db_send_dbmsg(thread_id, label, DB_IOP_READ_RECORD_START, playload, msglen, index);
    free(playload);
    return ret;

}

/**********************************************************************
* @name      : send_read_record_step
* @brief     ：发送单步读记录型数据
* @param[in] ：DBMSG_SEND_FUN func            发送回调函数
               uint16 label                   消息label
               uint32 handle                  数据中心句柄
               STEP_READ_RECORD_T *stepRecord 数据条件
* @param[out]：uint16 *index                  发送消息的index
* @return    ：错误码
* @Create    : 王津剑
* @Date      ：2019-12-27
* @Update    :
**********************************************************************/
int db_send_read_record_step(uint8 thread_id, uint16 label, uint32 handle, STEP_READ_RECORD_T *stepRecord, uint16 *index)
{
    int    ret = 0;
    uint16 msglen = 0;
    uint16 i = 0;
    uint8 *playload = NULL;
    uint8  bufLen[3] = {0};
    uint8  bytes = 0;

    //        数据库句柄           附加校验OPTIONAL
    msglen = sizeof(uint32) + 1;
    
    if(0 != stepRecord->MAC.nNum)
    {
        bytes = appmsg_set_len_offset(stepRecord->MAC.nNum, bufLen);
        msglen += bytes + stepRecord->MAC.nNum;
    }

    playload = (uint8 *)malloc(msglen);
    if(NULL == playload)
    {
        return ERR_MEM;
    }
    memset(playload, 0, msglen);

    appmsg_memcpy_r(playload, &handle, sizeof(uint32));    //数据库句柄
    i += sizeof(uint32);

    // 附加校验
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
    
    ret = db_send_dbmsg(thread_id, label, DB_IOP_READ_RECORD_STEP, playload, msglen, index);
    free(playload);
    return ret;
    
}

/**********************************************************************
* @name      : send_read_record_end
* @brief     ：发送结束记录型数据读取
* @param[in] ：DBMSG_SEND_FUN func            发送回调函数
               uint16 label                   消息label
               uint32 handle                  数据中心句柄
* @param[out]：uint16 *index                  发送消息的index
* @return    ：错误码
* @Create    : 王津剑
* @Date      ：2019-12-27
* @Update    :
**********************************************************************/
int db_send_read_record_end(uint8 thread_id, uint16 label, uint32 handle, uint16 *index)
{
    uint8  playload[4] = {0};
    
    appmsg_memcpy_r(playload, &handle, sizeof(uint32));
    return db_send_dbmsg(thread_id, label, DB_IOP_READ_RECORD_END, playload, sizeof(uint32), index);
}

/**********************************************************************
* @name      : send_write_record
* @brief     ：发送写记录型数据
* @param[in] ：DBMSG_SEND_FUN func            发送回调函数
               uint16 label                   消息label
               WRITE_RECORD_T *record         记录内容
* @param[out]：uint16 *index                  发送消息的index
* @return    ：错误码
* @Create    : 王津剑
* @Date      ：2019-12-27
* @Update    :
**********************************************************************/
int db_send_write_record(uint8 thread_id, uint16 label, WRITE_RECORD_T *record, uint16 *index)
{

    int    ret = 0;
    uint16 msglen = 0;
    uint16 i = 0;
    uint8 *playload = NULL;
    uint8  bufLen[3] = {0};
    uint8  bytes = 0;

    //        逻辑设备号  采集开始时间  采集结束时间  采集存储时标信息点号数据类标识OPTIONAL 次OAD                            SEQUENCE SEQUENCE    
    msglen = 0x01    +   0x04   +    0x04    +    0x04 +     0x02  + 0x2  +   0x01 +  0x04 + 01 +01 ;
    
    //主OAD非0,强制有效 
    if(1 == record->recordOAD.optional || 0 != record->recordOAD.road.oadMain.value)
    {
        msglen += 0x04;    //主OAD
    }

    //附加校验
    bytes = appmsg_set_len_offset(record->MAC.nNum, bufLen);
    msglen += bytes + record->MAC.nNum;

    //octet-string的长度 
    bytes = appmsg_set_len_offset(record->inDatalen, bufLen);
    msglen += bytes + record->inDatalen;
    
    playload = (uint8 *)malloc(msglen);
    if(NULL == playload)
    {
        return ERR_MEM;
    }
    memset(playload, 0, msglen);
    
    playload[i++] = record->recordOAD.logicId;  //逻辑地址

	appmsg_memcpy_r(playload + i, &(record->colStartTm), sizeof(uint32));        //起始时间
    i += 4;

    appmsg_memcpy_r(playload + i, &(record->colEndTm), sizeof(uint32));          //结束时间
    i += 4;

    appmsg_memcpy_r(playload + i, &(record->colStoreTm), sizeof(uint32));         //存储时间
    i += 4;

    appmsg_memcpy_r(playload + i, &(record->recordOAD.infoNum), sizeof(uint16));  //信息点
    i += 2;

	if(0 == record->recordOAD.classtag)
    {
        uint16 tempclass = CLASS_DATA_NORMAL;
        appmsg_memcpy_r(playload + i, &tempclass, sizeof(uint16));//数据类标识 为0，填写成CLASS_DATA_NORMAL
    }
    else
    {
        appmsg_memcpy_r(playload + i, &(record->recordOAD.classtag), sizeof(uint16));//数据类标识
    }
    i += 2;

    // 主DI 
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

	playload[i++] = 1;
    appmsg_memcpy_r(playload + i, &(record->recordOAD.road.oadCols.oad[0]), sizeof(uint32));//次数据
    i += 4;

	// 数据缓冲 
    playload[i++] = 1;
    bytes = appmsg_set_len_offset(record->inDatalen, bufLen);
    memcpy(playload + i, bufLen, bytes); 
    i += bytes;
    memcpy(playload + i, record->inDataBuf, record->inDatalen);
	i += record->inDatalen;

    // 附加校验 
    bytes = appmsg_set_len_offset(record->MAC.nNum, bufLen);
    memcpy(playload + i, bufLen, bytes);
    i += bytes;
    memcpy(playload + i, record->MAC.value, record->MAC.nNum);
    i += record->MAC.nNum;

    ret = db_send_dbmsg(thread_id, label, DB_IOP_WRITE_RECORD, playload, msglen, index);
    free(playload);
    return ret;
    
}

/**********************************************************************
* @name      : update_record_get_len
* @brief     ：更新记录数据长度获取
* @param[in] ：UPDATE_RECORD_T *pData   记录内容
* @param[out]：
* @return    ：长度
* @Create    : 王津剑
* @Date      ：2019-12-27
* @Update    :
**********************************************************************/
uint16 db_update_record_get_len(UPDATE_RECORD_T *pData)
{
    uint16 len = 0;
    uint8  bufLen[3] = {0};
    uint8  bytes = 0;

    if(1 == pData->filter.choice || 2 == pData->filter.choice)
    {
        /*    逻辑设备号   OPTIONAL  次OAD      刷选方式  刷选条件   信息点号*/
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

    /* 附加校验 */
    bytes = appmsg_set_len_offset(pData->MAC.nNum, bufLen);
    len += bytes + pData->MAC.nNum;
    
    //octet-string的长度
    bytes = appmsg_set_len_offset(pData->inDatalen, bufLen);
    len += bytes + pData->inDatalen;          
    return len;
}


/**********************************************************************
* @name      : send_write_record
* @brief     ：发送更新记录数据
* @param[in] ：DBMSG_SEND_FUN func            发送回调函数
               uint16 label                   消息label
               UPDATE_RECORD_T *pData         记录内容
* @param[out]：uint16 *index                  发送消息的index
* @return    ：错误码
* @Create    : 王津剑
* @Date      ：2019-12-27
* @Update    :
**********************************************************************/
int db_send_update_record(uint8 thread_id, uint16 label, UPDATE_RECORD_T *pData, uint16 *index)
{
    int    ret = 0;
    uint16 msglen = 0;
    uint16 i = 0;
    uint8 *playload = NULL;
    uint8  bufLen[3] = {0};
    uint8  bytes = 0;

    msglen = db_update_record_get_len(pData);
    if(0 == msglen)
    {
        return ERR_NORMAL;
    }
    playload = (uint8 *)malloc(msglen);
    if(NULL == playload)
    {
        return ERR_MEM;
    }
    memset(playload, 0, msglen);

    playload[i++] = pData->recordOAD.logicId;                                              //逻辑设备号

    if(1 == pData->recordOAD.optional || 0 != pData->recordOAD.road.oadMain.value)
    {
        playload[i++] = 1;
        appmsg_memcpy_r(playload + i, &(pData->recordOAD.road.oadMain.value), sizeof(uint32)); //主数据DI
        i += 4;
    }
    else
    {
        playload[i++] = 0;
    }

    /* 次数据DI */
    appmsg_memcpy_r(playload + i, &(pData->recordOAD.road.oadCols.oad[0]), sizeof(uint32));
    i += 4;

    /* 筛选方式 */
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
        return ERR_NORMAL;
    }

    appmsg_memcpy_r(playload + i, &(pData->recordOAD.infoNum), sizeof(uint16));   //信息点号
    i += 2;

    /* 附加校验 */
    bytes = appmsg_set_len_offset(pData->MAC.nNum, bufLen);
    memcpy(playload + i, bufLen, bytes);
    i += bytes;
    memcpy(playload + i, pData->MAC.value, pData->MAC.nNum);
    i += pData->MAC.nNum;

    /* 数据缓冲 */
    bytes = appmsg_set_len_offset(pData->inDatalen, bufLen);
    memcpy(playload + i, bufLen, bytes);
    i += bytes;
    memcpy(playload + i, pData->inDataBuf, pData->inDatalen);

    ret = db_send_dbmsg(thread_id, label, DB_IOP_UPDATE_RECORD, playload, msglen, index);
    free(playload);
    return ret;
}


/**********************************************************************
* @name      : send_write_record
* @brief     ：发送更新记录数据
* @param[in] ：DBMSG_SEND_FUN func            发送回调函数
               uint16 label                   消息label
               UPDATE_RECORD_T *pData         记录内容
* @param[out]：uint16 *index                  发送消息的index
* @return    ：错误码
* @Create    : 王津剑
* @Date      ：2019-12-27
* @Update    :
**********************************************************************/
int db_send_get_recorddata_num(uint8 thread_id, uint16 label, GET_RECORD_NUM_T *pRecord, uint16 *index)
{
    int    ret = 0;
    uint16 msglen = 0;
    uint16 i = 0;
    uint8 *playload = NULL;

    //        逻辑设备号 信息点号        OPTIONAL 次OAD    筛选方式  
    msglen = 0x01  		+ 0x2    + 0x01 +  0x04  +   0x01;

	//主DI
	if(pRecord->recordOAD.optional == 1 || pRecord->recordOAD.road.oadMain.value != 0)
    {
        msglen += 4;
    }
	//筛选方式
	if(pRecord->filter == GET_INTER_RECORD)
    {
        msglen += 8;
    }
    playload = (uint8 *)malloc(msglen);
    if(NULL == playload)
    {
        return ERR_MEM;
    }
    memset(playload, 0, msglen);

    playload[i++] = pRecord->recordOAD.logicId;                                              //逻辑设备号
    appmsg_memcpy_r(playload + i, &(pRecord->recordOAD.infoNum), sizeof(uint16));   //信息点号
    i += 2;

    if(1 == pRecord->recordOAD.optional || 0 != pRecord->recordOAD.road.oadMain.value)
    {
        playload[i++] = 1;
        appmsg_memcpy_r(playload + i, &(pRecord->recordOAD.road.oadMain.value), sizeof(uint32)); //主数据DI
        i += 4;
    }
    else
    {
        playload[i++] = 0;
    }

    /* 次数据DI */
    appmsg_memcpy_r(playload + i, &(pRecord->recordOAD.road.oadCols.oad[0]), sizeof(uint32));
    i += 4;

    /* 筛选方式 */
    playload[i++] = pRecord->filter;
    if(pRecord->filter == 1)
    {
        appmsg_memcpy_r(playload + i, &(pRecord->gStart), sizeof(uint32));
        i += 4;
		appmsg_memcpy_r(playload + i, &(pRecord->gEnd), sizeof(uint32));
        i += 4;
    }
	
    ret = db_send_dbmsg(thread_id, label, DB_IOP_GET_RECORD_NUM, playload, msglen, index);
    free(playload);
    return ret;
}

/**********************************************************************
* @name      : send_write_record
* @brief     ：发送更新记录数据
* @param[in] ：DBMSG_SEND_FUN func            发送回调函数
               uint16 label                   消息label
               UPDATE_RECORD_T *pData         记录内容
* @param[out]：uint16 *index                  发送消息的index
* @return    ：错误码
* @Create    : 王津剑
* @Date      ：2019-12-27
* @Update    :
**********************************************************************/
int db_send_get_recorddata_depth(uint8 thread_id, uint16 label, RECORD_OAD_T *pRecord, uint16 *index)
{
    int    ret = 0;
    uint16 msglen = 0;
    uint16 i = 0;
    uint8 *playload = NULL;
    //        逻辑设备号 OPTIONAL 次OAD  
    msglen = 0x01  		+ 0x01 +  0x04 ;

	//主DI
	if(pRecord->optional == 1 || pRecord->road.oadMain.value != 0)
    {
        msglen += 4;
    }

    playload = (uint8 *)malloc(msglen);
    if(NULL == playload)
    {
        return ERR_MEM;
    }
    memset(playload, 0, msglen);

    playload[i++] = pRecord->logicId;                                              //逻辑设备号


    if(1 == pRecord->optional || 0 != pRecord->road.oadMain.value)
    {
        playload[i++] = 1;
        appmsg_memcpy_r(playload + i, &(pRecord->road.oadMain.value), sizeof(uint32)); //主数据DI
        i += 4;
    }
    else
    {
        playload[i++] = 0;
    }

    /* 次数据DI */
    appmsg_memcpy_r(playload + i, &(pRecord->road.oadCols.oad[0]), sizeof(uint32));
    i += 4;
	
    ret = db_send_dbmsg(thread_id, label, DB_IOP_GET_RECORD_DEPTH, playload, msglen, index);
    free(playload);
    return ret;
}


/**********************************************************************
* @name      : db_read_nomal
* @brief     ：db操作读取普通数据 
* @param[in] ：DB_CLIENT fd            socket句柄
               NOMAL_OAD_T *ptOAD    OAD
               uint32 inMaxLen       最大缓存数量
               uint8 *outData        缓存
* @param[out]：uint32 *outLen        数据长度
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2019-12-24
* @Update    :
**********************************************************************/
int db_read_nomaldata(uint8 thread_id, NOMAL_OAD_T *ptOAD, uint32 inMaxLen, uint8 *outData, uint32 *outLen)
{
    int    ret = ERR_OK;
    uint8  recvbuf[MAX_MSG_BUF_LEN] = {0};
    int    recvlen = 0;       
    uint16            index = 0;
    struct timespec ctime = {0};
    struct timespec stime = {0};
    MESSAGE_INFO_T *pMsgInfo = NULL;

     /* 数据处理 */
    uint8   bytes = 0;
    uint32  len = 0;
    uint32  oad = 0;
    uint16  classtag = 0;
    uint8  *pTemp ;
    
    if(thread_id >= MAX_THREAD || NULL == ptOAD || 0 == inMaxLen || NULL == outData || NULL == outLen)
    {
        return ERR_NORMAL;
    }

    ret = db_send_read_nomal_data(thread_id, 0, ptOAD, &index);
    COMM_FMT_TRACE("read normal data: oad 0x%x, index %u, ret %d\n", ptOAD->oad.value, index, ret);
    if(0 != ret)
    {
        COMM_FMT_DEBUG("send_read_nomal_data err ret %d\n", ret);
        return ret;
    }

    /* 2. 接收等待 */
   clock_gettime(CLOCK_MONOTONIC, &stime);
   for(;;)
   {
       memset(recvbuf, 0, MAX_MSG_BUF_LEN);
       recvlen = db_recv_msg(g_dbMqttMsgCtx[thread_id], recvbuf, MAX_MSG_BUF_LEN);
       if (recvlen > 0)
		{
			ret = appmsg_split(recvbuf, (uint16)recvlen, &pMsgInfo);
			COMM_BUF_TRACE(recvbuf, recvlen, "Recv mqtt msg:\n");
            
			if (0 == ret && NULL != pMsgInfo)
			{
			    
			    COMM_FMT_TRACE("pMsgInfo->IID=%04x, pMsgInfo->IOP=%04x\n",pMsgInfo->IID, pMsgInfo->IOP);
                /* 性能测试 */
				if (MESSAGE_IID_DB != pMsgInfo->IID || DB_IOP_READ_NORMAL_DATA != pMsgInfo->IOP || index != pMsgInfo->index )
				{
					COMM_FMT_DEBUG("MSG ERROR pMsgInfo->IID=%04x, pMsgInfo->IOP=%04x pMsgInfo->index%hu\n",pMsgInfo->IID, pMsgInfo->IOP, pMsgInfo->index);
                    free(pMsgInfo);
                    continue;
				}


                pTemp = pMsgInfo->playload;
				if (0 == pMsgInfo->msglen || 1 != *pTemp)  //数量
                {
                    ret = ERR_NOTEXIST;
                    goto end;
                }
                
                pTemp++;
                appmsg_memcpy_r(&oad, pTemp, sizeof(uint32));
                pTemp += sizeof(uint32);
                if(oad != ptOAD->oad.value)
                {
                    ret = ERR_INVALID;
                    goto end;
                }

                appmsg_memcpy_r(&classtag, pTemp, sizeof(uint16));
                pTemp += sizeof(uint16);

                len = appmsg_get_len_offset(pTemp, &bytes); //计算octet-string的长度
                if(inMaxLen < len)
                {
                    ret = ERR_OVERFLOW;
                    goto end;
                }

                *outLen = len;
                if(len > 0)
                {
                    memcpy(outData, pTemp + bytes, len);
                }
               
end:                
				free(pMsgInfo);
				pMsgInfo = NULL;
                return ret;
			}
		}

       clock_gettime(CLOCK_MONOTONIC, &ctime);
       if(ctime.tv_sec - stime.tv_sec > DB_OPERATE_OUT_TIME)
       {
           return ERR_TIMEOUT;
       }
   }

}

/**********************************************************************
* @name      : db_read_nomal
* @brief     ：db操作读取普通数据 
* @param[in] ：DB_CLIENT fd            socket句柄
               NOMAL_OAD_T *ptOAD    OAD
               uint32 inMaxLen       最大缓存数量
               uint8 *outData        缓存
* @param[out]：uint32 *outLen        数据长度
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2019-12-24
* @Update    :
**********************************************************************/
int db_write_nomaldata(uint8 thread_id, NOMAL_OAD_T *ptOAD, uint8 *pData, uint32 datalen)
{
    int    ret = ERR_OK;
    uint8  recvbuf[MAX_MSG_BUF_LEN] = {0};
    int    recvlen = 0;       
    uint16            index = 0;
    struct timespec ctime = {0};
    struct timespec stime = {0};
    MESSAGE_INFO_T *pMsgInfo = NULL;

     /* 数据处理 */
    uint8  *pTemp ;
    
    if(thread_id >= MAX_THREAD || NULL == ptOAD || NULL == ptOAD || 0 == datalen || NULL == pData)
    {
        return ERR_NORMAL;
    }

    ret = db_send_write_nomal_data(thread_id, 0, ptOAD, pData, datalen, &index);
    DPRINT("write normal data: oad 0x%x, index %u, ret %d\n", ptOAD->oad.value, index, ret);
    if(0 != ret)
    {
        printf("send_read_nomal_data err ret %d\n", ret);
        return ret;
    }

    /* 2. 接收等待 */
   clock_gettime(CLOCK_MONOTONIC, &stime);
   for(;;)
   {
       memset(recvbuf, 0, MAX_MSG_BUF_LEN);
       recvlen = db_recv_msg(g_dbMqttMsgCtx[thread_id], recvbuf, MAX_MSG_BUF_LEN);
       if (recvlen > 0)
		{
			ret = appmsg_split(recvbuf, (uint16)recvlen, &pMsgInfo);
			DPRINT_MSG(recvbuf, recvlen, "Recv mqtt msg:\n");
            
			if (0 == ret && NULL != pMsgInfo)
			{
			    
			    DPRINT("pMsgInfo->IID=%04x, pMsgInfo->IOP=%04x\n",pMsgInfo->IID, pMsgInfo->IOP);
                /* 性能测试 */
				if (MESSAGE_IID_DB != pMsgInfo->IID || DB_IOP_WRITE_NORMAL_DATA != pMsgInfo->IOP || index != pMsgInfo->index )
				{
					DPRINT("MSG ERROR pMsgInfo->IID=%04x, pMsgInfo->IOP=%04x pMsgInfo->index%hu\n",pMsgInfo->IID, pMsgInfo->IOP, pMsgInfo->index);
                    free(pMsgInfo);
                    continue;
				}

                pTemp = pMsgInfo->playload;
				if (0 == pMsgInfo->msglen || 1 != *pTemp)  //数量
                {
                    ret = ERR_INVALID;
                }
    
				free(pMsgInfo);
				pMsgInfo = NULL;
                return ret;
			}
		}

       clock_gettime(CLOCK_MONOTONIC, &ctime);
       if(ctime.tv_sec - stime.tv_sec > DB_OPERATE_OUT_TIME)
       {
           return ERR_TIMEOUT;
       }
   }

}

/**********************************************************************
* @name      : db_write_nomal_more
* @brief     ：db操作开始读记录型数据(多条数据)
* @param[in] ：DB_CLIENT fd              DB客户端句柄
               uint8 logic               逻辑设备号
               uint16 info               信息点号
               NOMAL_OAD_DATE_T *inDatas 多条数据
               uint8 inDataNum           数据数量
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2020-4-27
* @Update    :
**********************************************************************/
int db_write_nomaldata_more(uint8 thread_id, uint8 logic, uint16 info, NOMAL_OAD_DATE_T *inDatas, uint8 inDataNum)
{
    int    ret = ERR_OK;
    uint8  recvbuf[MAX_MSG_BUF_LEN] = {0};
    int    recvlen = 0;       
    uint16 index = 0;
    struct timespec ctime = {0};
    struct timespec stime = {0};
    MESSAGE_INFO_T *pMsgInfo = NULL;

     /* 数据处理 */
    uint8  *pTemp ;

    if(thread_id >= MAX_THREAD || NULL == inDatas || 0 == inDataNum)
    {
        return ERR_NORMAL;
    }

    ret = db_send_write_nomal_datas(thread_id, 0, logic, info, inDatas, inDataNum, &index);
    DPRINT("write normal data: oad 0x%x, index %u, ret %d\n", inDatas->oad.value, index, ret);
    if(0 != ret)
    {
        printf("db_send_write_nomal_datas err ret %d\n", ret);
        return ret;
    }

    /* 2. 接收等待 */
   clock_gettime(CLOCK_MONOTONIC, &stime);
   for(;;)
   {
       memset(recvbuf, 0, MAX_MSG_BUF_LEN);
       recvlen = db_recv_msg(g_dbMqttMsgCtx[thread_id], recvbuf, MAX_MSG_BUF_LEN);
       if (recvlen > 0)
		{
			ret = appmsg_split(recvbuf, (uint16)recvlen, &pMsgInfo);
			DPRINT_MSG(recvbuf, recvlen, "Recv mqtt msg:\n");
            
			if (0 == ret && NULL != pMsgInfo)
			{
			    
			    DPRINT("pMsgInfo->IID=%04x, pMsgInfo->IOP=%04x\n",pMsgInfo->IID, pMsgInfo->IOP);
                /* 性能测试 */
				if (MESSAGE_IID_DB != pMsgInfo->IID || DB_IOP_WRITE_NORMAL_DATA != pMsgInfo->IOP || index != pMsgInfo->index )
				{
					DPRINT("MSG ERROR pMsgInfo->IID=%04x, pMsgInfo->IOP=%04x pMsgInfo->index%hu\n",pMsgInfo->IID, pMsgInfo->IOP, pMsgInfo->index);
                    free(pMsgInfo);
                    continue;
				}

                pTemp = pMsgInfo->playload;
				if (0 == pMsgInfo->msglen || 1 != *pTemp)  //数量
                {
                    ret = ERR_INVALID;
                }
             
				free(pMsgInfo);
				pMsgInfo = NULL;
                return ret;
			}
		}

       clock_gettime(CLOCK_MONOTONIC, &ctime);
       if(ctime.tv_sec - stime.tv_sec > DB_OPERATE_OUT_TIME)
       {
           return ERR_TIMEOUT;
       }
   }
}

/**********************************************************************
* @name      : db_read_record_start
* @brief     ：db操作开始读记录型数据
* @param[in] ：DB_CLIENT fd             socket句柄
               READ_RECORD_T *ptRecord  记录条件      主OAD为0表示非复合型数据
* @param[out]：uint32 *handle           数据中心句柄
               uint32 *recNum           记录条数
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2019-12-26
* @Update    :
**********************************************************************/
int db_read_recorddata_start(uint8 thread_id, READ_RECORD_T *ptRecord, uint32 *handle, uint32 *recNum)
{
    int    ret = ERR_OK;
    uint8  recvbuf[MAX_MSG_BUF_LEN] = {0};
    int    recvlen = 0;       
    uint16            index = 0;
    struct timespec ctime = {0};
    struct timespec stime = {0};
    MESSAGE_INFO_T *pMsgInfo = NULL;

     /* 数据处理 */
    uint8  *pTemp ;
    
    if(thread_id >= MAX_THREAD || NULL == ptRecord || NULL == handle || NULL == recNum)
    {
        return ERR_NORMAL;
    }

    ret = db_send_read_record_start(thread_id, 0, ptRecord, &index);
    
    MQTTMSG_FMT_TRACE("read record data start: moad 0x%04x, oad 0x%04x\n", 
        ptRecord->recordOAD.road.oadMain.value, ptRecord->recordOAD.road.oadCols.oad[0].value);
    
    if(0 != ret)
    {
        printf("send_read_record_start err ret %d\n", ret);
        return ret;
    }

    /* 2. 接收等待 */
   clock_gettime(CLOCK_MONOTONIC, &stime);
   for(;;)
   {
       memset(recvbuf, 0, MAX_MSG_BUF_LEN);
       recvlen = db_recv_msg(g_dbMqttMsgCtx[thread_id], recvbuf, MAX_MSG_BUF_LEN);
       if (recvlen > 0)
		{
			ret = appmsg_split(recvbuf, (uint16)recvlen, &pMsgInfo);
			MQTTMSG_BUF_TRACE(recvbuf, recvlen, "Recv mqtt msg:\n");
            
			if (0 == ret && NULL != pMsgInfo)
			{
			    
			    DPRINT("pMsgInfo->IID=%04x, pMsgInfo->IOP=%04x\n",pMsgInfo->IID, pMsgInfo->IOP);
                /* 性能测试 */
				if (MESSAGE_IID_DB != pMsgInfo->IID || DB_IOP_READ_RECORD_START != pMsgInfo->IOP || index != pMsgInfo->index )
				{
					DPRINT("MSG ERROR pMsgInfo->IID=%04x, pMsgInfo->IOP=%04x pMsgInfo->index%hu\n",pMsgInfo->IID, pMsgInfo->IOP, pMsgInfo->index);
                    free(pMsgInfo);
                    continue;
				}

                pTemp = pMsgInfo->playload;
				if (pMsgInfo->msglen != 8)  //
                {
                    ret = ERR_INVALID;
                    goto end;
                }

                appmsg_memcpy_r(handle, pTemp, sizeof(uint32));
                appmsg_memcpy_r(recNum, pTemp + sizeof(uint32), sizeof(uint32));

end:                
				free(pMsgInfo);
				pMsgInfo = NULL;
                return ret;
			}
		}

       clock_gettime(CLOCK_MONOTONIC, &ctime);
       if(ctime.tv_sec - stime.tv_sec > DB_OPERATE_OUT_TIME)
       {
           return ERR_TIMEOUT;
       }
   }

}

/**********************************************************************
* @name      : db_read_record_start
* @brief     ：db操作开始读记录型数据
* @param[in] ：DB_CLIENT fd             socket句柄
               READ_RECORD_T *ptRecord  记录条件      主OAD为0表示非复合型数据
* @param[out]：uint32 *handle           数据中心句柄
               uint32 *recNum           记录条数
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2019-12-26
* @Update    :
**********************************************************************/
int db_read_recorddata_step(uint8 thread_id, uint32 handle, STEP_READ_RECORD_T *ptRecord, RECORD_UNIT_T *stepRecordOut)
{
    int    ret = ERR_OK;
    uint8  recvbuf[MAX_MSG_BUF_LEN] = {0};
    int    recvlen = 0;       
    uint16            index = 0;
    struct timespec ctime = {0};
    struct timespec stime = {0};
    MESSAGE_INFO_T *pMsgInfo = NULL;

    /* 数据处理 */
    uint16  offset = 0;
    uint16  i = 0;
    uint8   bytes = 0;
    RECORD_DATA_T *ptData = NULL;
    uint8  *pTemp ;
    
    if(thread_id >= MAX_THREAD || NULL == ptRecord || NULL == stepRecordOut)
    {
        return ERR_NORMAL;
    }

    ret = db_send_read_record_step(thread_id, 0, handle, ptRecord, &index);

    MQTTMSG_FMT_TRACE("read record data step: handle %u, index %u, ret %d\n", handle, index, ret);
    
    if(0 != ret)
    {
        printf("send_read_record_start err ret %d\n", ret);
        return ret;
    }

    /* 2. 接收等待 */
   clock_gettime(CLOCK_MONOTONIC, &stime);
   for(;;)
   {
       memset(recvbuf, 0, MAX_MSG_BUF_LEN);
       recvlen = db_recv_msg(g_dbMqttMsgCtx[thread_id], recvbuf, MAX_MSG_BUF_LEN);
       if (recvlen > 0)
		{
			ret = appmsg_split(recvbuf, (uint16)recvlen, &pMsgInfo);
			MQTTMSG_BUF_TRACE(recvbuf, recvlen, "Recv mqtt msg:\n");
            
			if (0 == ret && NULL != pMsgInfo)
			{
			    
			    DPRINT("pMsgInfo->IID=%04x, pMsgInfo->IOP=%04x\n",pMsgInfo->IID, pMsgInfo->IOP);
                /* 性能测试 */
				if (MESSAGE_IID_DB != pMsgInfo->IID || DB_IOP_READ_RECORD_STEP != pMsgInfo->IOP || index != pMsgInfo->index )
				{
					DPRINT("MSG ERROR pMsgInfo->IID=%04x, pMsgInfo->IOP=%04x pMsgInfo->index%hu\n",pMsgInfo->IID, pMsgInfo->IOP, pMsgInfo->index);
                    free(pMsgInfo);
                    continue;
				}

                pTemp = pMsgInfo->playload;

                memset(stepRecordOut, 0, sizeof(RECORD_UNIT_T));
                if(1 == pTemp[offset++])
                {
                    appmsg_memcpy_r(&(stepRecordOut->infoNum), pTemp + offset, sizeof(uint16));   //信息点号          
                    offset += sizeof(uint16);
                    
                    stepRecordOut->recordnum = appmsg_get_len_offset(pTemp + offset, &bytes);     //记录数量
                    offset += bytes;

                    for(i = 0; i < stepRecordOut->recordnum; i++)
                    {
                        ptData = &(stepRecordOut->record[i]);

                        ptData->optional = pTemp[offset++];
                        if(1 == ptData->optional)
                        {
                            appmsg_memcpy_r(&(ptData->road.oadMain.value), pTemp + offset, sizeof(uint32));//主数据DI
                            offset += sizeof(uint32);
                        }
                        else if(0 == ptData->optional)
                        {
                            ptData->road.oadMain.value = 0;                                                    //主数据DI
                        }
                        else
                        {
                            memset(stepRecordOut, 0, sizeof(RECORD_UNIT_T));//格式不对，返回数据清0，长度为0
                            ret =  ERR_SETP_REC;
                            goto end;
                        }

                        ptData->road.oadCols.nNum = 0x01;
                        appmsg_memcpy_r(&(ptData->road.oadCols.oad[0].value), pTemp + offset, sizeof(uint32));//次数据DI
                        offset += sizeof(uint32);

                        ptData->outDatalen = appmsg_get_len_offset(pTemp + offset, &bytes); //长度
                        offset += bytes;
                        memcpy(ptData->outDataBuf, pTemp + offset, ptData->outDatalen);     //数据
                        offset += ptData->outDatalen;
                    }

                    if(1 == pTemp[offset])
                    {
                        offset++;
                        appmsg_memcpy_r(&(stepRecordOut->colStartTm), pTemp + offset, sizeof(uint32));//采集起始时间
                        offset += sizeof(uint32);
                        
                        appmsg_memcpy_r(&(stepRecordOut->colEndTm), pTemp + offset, sizeof(uint32));  //采集结束时间
                        offset += sizeof(uint32);
                        
                        appmsg_memcpy_r(&(stepRecordOut->colStoreTm), pTemp + offset, sizeof(uint32));//采集存储时间
                        offset += sizeof(uint32);
                    }
                }
                else
                {
                    ret = ERR_NOTEXIST;
                }

end:                
				free(pMsgInfo);
				pMsgInfo = NULL;
                return ret;
			}
		}

       clock_gettime(CLOCK_MONOTONIC, &ctime);
       if(ctime.tv_sec - stime.tv_sec > DB_OPERATE_OUT_TIME)
       {
           return ERR_TIMEOUT;
       }
   }

}

/**********************************************************************
* @name      : db_write_record
* @brief     ：db操作写记录型数据
* @param[in] ：DB_CLIENT fd             socket句柄
               WRITE_RECORD_T *ptRecord 记录
* @param[out]：uint16 *storeNo          存储序号
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2019-12-26
* @Update    :
**********************************************************************/
int db_read_recorddata_end(uint8 thread_id, uint32 handle)
{
    int    ret = ERR_OK;
    uint8  recvbuf[MAX_MSG_BUF_LEN] = {0};
    int    recvlen = 0;       
    uint16            index = 0;
    struct timespec ctime = {0};
    struct timespec stime = {0};
    MESSAGE_INFO_T *pMsgInfo = NULL;

     /* 数据处理 */
    uint8  *pTemp ;
    
    if(thread_id >= MAX_THREAD )
    {
        return ERR_NORMAL;
    }

    ret = db_send_read_record_end(thread_id, 0, handle, &index);
    
    DPRINT("send_read_record_end: handle %u, index %u, ret %d\n", handle, index, ret);
    
    if(0 != ret)
    {
        printf("send_read_record_end err ret %d\n", ret);
        return ret;
    }

    /* 2. 接收等待 */
   clock_gettime(CLOCK_MONOTONIC, &stime);
   for(;;)
   {
       memset(recvbuf, 0, MAX_MSG_BUF_LEN);
       recvlen = db_recv_msg(g_dbMqttMsgCtx[thread_id], recvbuf, MAX_MSG_BUF_LEN);
       if (recvlen > 0)
		{
			ret = appmsg_split(recvbuf, (uint16)recvlen, &pMsgInfo);
			DPRINT_MSG(recvbuf, recvlen, "Recv mqtt msg:\n");
            
			if (0 == ret && NULL != pMsgInfo)
			{
			    
			    DPRINT("pMsgInfo->IID=%04x, pMsgInfo->IOP=%04x\n",pMsgInfo->IID, pMsgInfo->IOP);
                /* 性能测试 */
				if (MESSAGE_IID_DB != pMsgInfo->IID || DB_IOP_READ_RECORD_END != pMsgInfo->IOP || index != pMsgInfo->index )
				{
					DPRINT("MSG ERROR pMsgInfo->IID=%04x, pMsgInfo->IOP=%04x pMsgInfo->index%hu\n",pMsgInfo->IID, pMsgInfo->IOP, pMsgInfo->index);
                    free(pMsgInfo);
                    continue;
				}

                pTemp = pMsgInfo->playload;
				if (0 == pMsgInfo->msglen || 1 != *pTemp)  //数量
                {
                    ret = ERR_INVALID;
                }
              
				free(pMsgInfo);
				pMsgInfo = NULL;
                return ret;
			}
		}

       clock_gettime(CLOCK_MONOTONIC, &ctime);
       if(ctime.tv_sec - stime.tv_sec > DB_OPERATE_OUT_TIME)
       {
           return ERR_TIMEOUT;
       }
   }
}


/**********************************************************************
* @name      : db_read_record
* @brief     ：db操作读最近的一条记录
* @param[in] ：DB_CLIENT fd             socket句柄
               READ_RECORD_T *ptRecord  记录条件
               uint32 bufLen            最大读取buf长度
* @param[out]：RECORD_UNIT_T *recordOut 数据记录
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2019-12-26
* @Update    :
**********************************************************************/
int db_read_recorddata(uint8 thread_id, READ_RECORD_T *ptRecord, uint32 bufLen, RECORD_UNIT_T *recordOut)
{
    int        ret = ERR_OK;
    READ_RECORD_T      inReadRecordTmp;
    STEP_READ_RECORD_T stepReadRecord;
    uint32 handle;
    uint32 recNum;
    int32  nRet = 0;
    
    if(thread_id >= MAX_THREAD || NULL == ptRecord || NULL == recordOut)
    {
        return ERR_NORMAL;
    }

    if (bufLen > DATA_LEN_MAX)
    {
        return ERR_O_RANGE;
    }

    /* 只读最近一条 */
    inReadRecordTmp = *ptRecord;
    inReadRecordTmp.sortType = 0x01;

    ret = db_read_recorddata_start(thread_id, &inReadRecordTmp, &handle, &recNum);
    if (0 != ret || recNum == 0)
    {
    	MQTTMSG_FMT_DEBUG("db_read_recorddata_start err ret %d\n", ret);
        return ERR_START_REC;
    }
    
    memcpy(&stepReadRecord.MAC, &inReadRecordTmp.MAC, sizeof(OOP_OCTETVAR64_T));
    /* 读最近一条记录 */
    ret = db_read_recorddata_step(thread_id, handle, &stepReadRecord, recordOut);
    if (0 != ret)
    {
    	MQTTMSG_FMT_DEBUG("db_read_recorddata_step err ret %d\n", ret);
        nRet = ERR_SETP_REC;
    }

    /*结束读记录*/
    ret = db_read_recorddata_end(thread_id, handle);
    if (0 != ret)
    {
        nRet = ERR_END_REC;
    }

    return nRet;
}

/**********************************************************************
* @name      : db_write_record
* @brief     ：db操作写记录型数据
* @param[in] ：DB_CLIENT fd             socket句柄
               WRITE_RECORD_T *ptRecord 记录
* @param[out]：uint16 *storeNo          存储序号
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2019-12-26
* @Update    :
**********************************************************************/
int db_write_recorddata(uint8 thread_id, WRITE_RECORD_T *ptRecord, uint32 *storeNo)
{
    int    ret = ERR_OK;
    uint8  recvbuf[MAX_MSG_BUF_LEN] = {0};
    int    recvlen = 0;       
    uint16            index = 0;
    struct timespec ctime = {0};
    struct timespec stime = {0};
    MESSAGE_INFO_T *pMsgInfo = NULL;

     /* 数据处理 */
    uint8  *pTemp ;
    
    if(thread_id >= MAX_THREAD || NULL == ptRecord || NULL == storeNo)
    {
        return ERR_NORMAL;
    }

    ret = db_send_write_record(thread_id, 0, ptRecord, &index);
    
    DPRINT("write record data: moad 0x%04x, oad 0x%04x\n", 
        ptRecord->recordOAD.road.oadMain.value, ptRecord->recordOAD.road.oadCols.oad[0].value);
    
    if(0 != ret)
    {
        printf("send_write_record err ret %d\n", ret);
        return ret;
    }

    /* 2. 接收等待 */
   clock_gettime(CLOCK_MONOTONIC, &stime);
   for(;;)
   {
       memset(recvbuf, 0, MAX_MSG_BUF_LEN);
       recvlen = db_recv_msg(g_dbMqttMsgCtx[thread_id], recvbuf, MAX_MSG_BUF_LEN);
       if (recvlen > 0)
		{
			ret = appmsg_split(recvbuf, (uint16)recvlen, &pMsgInfo);
			DPRINT_MSG(recvbuf, recvlen, "Recv mqtt msg:\n");
            
			if (0 == ret && NULL != pMsgInfo)
			{
			    
			    DPRINT("pMsgInfo->IID=%04x, pMsgInfo->IOP=%04x\n",pMsgInfo->IID, pMsgInfo->IOP);
                /* 性能测试 */
				/* 性能测试 */
				if (MESSAGE_IID_DB != pMsgInfo->IID || DB_IOP_WRITE_RECORD != pMsgInfo->IOP || index != pMsgInfo->index )
				{
					DPRINT("MSG ERROR pMsgInfo->IID=%04x, pMsgInfo->IOP=%04x pMsgInfo->index%hu\n",pMsgInfo->IID, pMsgInfo->IOP, pMsgInfo->index);
                    free(pMsgInfo);
                    continue;
				}

                pTemp = pMsgInfo->playload;
				if(pMsgInfo->msglen != 4)  //数量
                {
                    ret = ERR_INVALID;
                    goto end;
                }

                appmsg_memcpy_r(storeNo, pTemp, sizeof(uint32));

end:                
				free(pMsgInfo);
				pMsgInfo = NULL;
                return ret;
			}
		}

       clock_gettime(CLOCK_MONOTONIC, &ctime);
       if(ctime.tv_sec - stime.tv_sec > DB_OPERATE_OUT_TIME)
       {
           return ERR_TIMEOUT;
       }
   }
}


/**********************************************************************
* @name      : db_get_record_num
* @brief     ：db操作获取存储记录条数
* @param[in] ：DB_CLIENT fd             socket句柄
               OOP_ROAD_T *pRecord      条件
* @param[out]：uint32 *recNum           数量
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2019-12-26
* @Update    :
**********************************************************************/
int db_get_recorddata_num(uint8 thread_id, GET_RECORD_NUM_T *pRecord, uint32 *recNum)
{
    int   ret = ERR_OK;
    uint8  recvbuf[SMALL_MSG_BUF_LEN] = {0};
    uint16 recvlen = 0;
	uint16            index = 0;
    struct timespec ctime = {0};
    struct timespec stime = {0};
	MESSAGE_INFO_T *pMsgInfo = NULL;
	/* 数据处理 */
    uint8  *pTemp ;

	if(thread_id >= MAX_THREAD || NULL == pRecord || NULL == recNum)
    {
        return ERR_NORMAL;
    }

    ret = db_send_get_recorddata_num(thread_id, 0, pRecord, &index);
    
    
    if(0 != ret)
    {
        COMM_FMT_DEBUG("db_get_recorddata_num err ret %d\n", ret);
        return ret;
    }

    /* 2. 接收等待 */
   clock_gettime(CLOCK_MONOTONIC, &stime);
   for(;;)
   {
       memset(recvbuf, 0, MAX_MSG_BUF_LEN);
       recvlen = db_recv_msg(g_dbMqttMsgCtx[thread_id], recvbuf, MAX_MSG_BUF_LEN);
       if (recvlen > 0)
		{
			ret = appmsg_split(recvbuf, (uint16)recvlen, &pMsgInfo);
			COMM_BUF_TRACE(recvbuf, recvlen, "Recv mqtt msg:\n");
            
			if (0 == ret && NULL != pMsgInfo)
			{
			    
			    COMM_FMT_TRACE("pMsgInfo->IID=%04x, pMsgInfo->IOP=%04x\n",pMsgInfo->IID, pMsgInfo->IOP);
                /* 性能测试 */
				/* 性能测试 */
				if (MESSAGE_IID_DB != pMsgInfo->IID || DB_IOP_GET_RECORD_NUM != pMsgInfo->IOP || index != pMsgInfo->index )
				{
					COMM_FMT_DEBUG("MSG ERROR pMsgInfo->IID=%04x, pMsgInfo->IOP=%04x pMsgInfo->index%hu\n",pMsgInfo->IID, pMsgInfo->IOP, pMsgInfo->index);
                    free(pMsgInfo);
                    continue;
				}

                pTemp = pMsgInfo->playload;
				if(pMsgInfo->msglen != 4)  //数量
                {
                    ret = ERR_INVALID;
                    goto end;
                }

                appmsg_memcpy_r(recNum, pTemp, sizeof(uint32));

end:                
				free(pMsgInfo);
				pMsgInfo = NULL;
                return ret;
			}
		}

       clock_gettime(CLOCK_MONOTONIC, &ctime);
       if(ctime.tv_sec - stime.tv_sec > DB_OPERATE_OUT_TIME)
       {
           return ERR_TIMEOUT;
       }
   }
}

/**********************************************************************
* @name      : db_get_record_num
* @brief     ：db操作获取存储记录条数
* @param[in] ：DB_CLIENT fd             socket句柄
               OOP_ROAD_T *pRecord      条件
* @param[out]：uint32 *recNum           数量
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2019-12-26
* @Update    :
**********************************************************************/
int db_get_recorddata_depth(uint8 thread_id, RECORD_OAD_T *pRecord, uint32 *recNum)
{
    int   ret = ERR_OK;
    uint8  recvbuf[SMALL_MSG_BUF_LEN] = {0};
    uint16 recvlen = 0;
	uint16            index = 0;
    struct timespec ctime = {0};
    struct timespec stime = {0};
	MESSAGE_INFO_T *pMsgInfo = NULL;
	/* 数据处理 */
    uint8  *pTemp ;

	if(thread_id >= MAX_THREAD || NULL == pRecord || NULL == recNum)
    {
        return ERR_NORMAL;
    }

    ret = db_send_get_recorddata_depth(thread_id, 0, pRecord, &index);
    
    
    if(0 != ret)
    {
        MQTTMSG_FMT_DEBUG("db_get_recorddata_num err ret %d\n", ret);
        return ret;
    }

    /* 2. 接收等待 */
   clock_gettime(CLOCK_MONOTONIC, &stime);
   for(;;)
   {
       memset(recvbuf, 0, MAX_MSG_BUF_LEN);
       recvlen = db_recv_msg(g_dbMqttMsgCtx[thread_id], recvbuf, MAX_MSG_BUF_LEN);
       if (recvlen > 0)
		{
			ret = appmsg_split(recvbuf, (uint16)recvlen, &pMsgInfo);
			MQTTMSG_BUF_TRACE(recvbuf, recvlen, "Recv mqtt msg:\n");
            
			if (0 == ret && NULL != pMsgInfo)
			{
			    
			    MQTTMSG_FMT_TRACE("pMsgInfo->IID=%04x, pMsgInfo->IOP=%04x\n",pMsgInfo->IID, pMsgInfo->IOP);
                /* 性能测试 */
				/* 性能测试 */
				if (MESSAGE_IID_DB != pMsgInfo->IID || DB_IOP_GET_RECORD_DEPTH != pMsgInfo->IOP || index != pMsgInfo->index )
				{
					MQTTMSG_FMT_DEBUG("MSG ERROR pMsgInfo->IID=%04x, pMsgInfo->IOP=%04x pMsgInfo->index%hu\n",pMsgInfo->IID, pMsgInfo->IOP, pMsgInfo->index);
                    free(pMsgInfo);
                    continue;
				}

                pTemp = pMsgInfo->playload;
				if(pMsgInfo->msglen != 4)  //数量
                {
                    ret = ERR_INVALID;
                    goto end;
                }

                appmsg_memcpy_r(recNum, pTemp, sizeof(uint32));

end:                
				free(pMsgInfo);
				pMsgInfo = NULL;
                return ret;
			}
		}

       clock_gettime(CLOCK_MONOTONIC, &ctime);
       if(ctime.tv_sec - stime.tv_sec > DB_OPERATE_OUT_TIME)
       {
           return ERR_TIMEOUT;
       }
   }
}


