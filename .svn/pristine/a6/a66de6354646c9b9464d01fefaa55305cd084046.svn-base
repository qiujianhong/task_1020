/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief：参数项定义
* @date： 2019.10.26
*********************************************************************
*/
#ifndef __OAD_LIST_H__
#define __OAD_LIST_H__

#include "../../include/baseCommon.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct _OAD_LIST_T
{
    OOP_OAD_U      oad;       //OAD
    OOP_CLASS_E    class;     //class号
    uint8          OIA1;      //对象标识中A1的值
    uint8          OIA2;      //对象标识中A2的值
    uint32         lenType;   //长度类型，0：nLen是整个buf长度，1：数据单元单元长度，比如档案的配置单元，方案单元，任务单元，事件单元
    OOP_DATATYPE_E dataType;  //数据类型
    int            nLen;      //内容长度
    uint8          group;
}OAD_LIST_T;


/* 电量量类OAD列表 OIA1 = 0x00 */
extern const OAD_LIST_T oadEnergyList[];
extern const uint32 oadEnergyListLen;

/* 最大需量类OAD列表 OIA1 = 0x01 */
extern const OAD_LIST_T oadDemandList[];
extern const uint32 oadDemandListLen;

/* 变量类OAD列表 OIA1 = 0x02 */
extern const OAD_LIST_T oadVariateList[];
extern const uint32 oadVariateListLen;

/* 事件类OAD列表 OIA1 = 0x03 */
extern const OAD_LIST_T oadEvnetList[];
extern const uint32 oadEvnetListLen;

/* 参变量类OAD列表 OIA1 = 0x04 */
extern const OAD_LIST_T oadParaList[];
extern const uint32 oadParaListLen;

/* 冻结类OAD列表 OIA1 = 0x05 */
extern const OAD_LIST_T oadFreezeList[];
extern const uint32 oadFreezeListLen;

/* 采集类OAD列表 OIA1 = 0x06 */
extern const OAD_LIST_T oadCollectList[];
extern const uint32 oadCollectListLen;

/* 集合类OAD列表 OIA1 = 0x07 */
extern const OAD_LIST_T oadGatherList[];
extern const uint32 oadGatherListLen;

/* 控制接口类OAD列表 OIA1 = 0x08 */
extern const OAD_LIST_T oadControlList[];
extern const uint32 oadControlListLen;

/* 文件传输接口类OAD列表 OIA1 = 0x0F OIA2 = 0x00 */
extern const OAD_LIST_T oadFileList[];
extern const uint32 oadFileListLen;

/* ESAM接口类OAD列表 OIA1 = 0x0F OIA2 = 0x01 */
extern const OAD_LIST_T oadEsamList[];
extern const uint32 oadEsamListLen;

/* 输入输出类OAD列表 OIA1 = 0x0F OIA2 = 0x02 */
extern const OAD_LIST_T oadIoList[];
extern const uint32 oadIoListLen;


/* 临时在这里建表，为了读记录数据,只做表名和表结构 最后在具体的class里建表*/
typedef struct _OAD_RECORD_LIST_T
{
    OOP_OAD_U      oad;
    OOP_DATATYPE_E dataType;  //数据类型
    int            nLen;      //内容长度
}OAD_RECORD_LIST;

/* 电量量类OAD列表 OIA1 = 0x00 */
extern  OAD_RECORD_LIST oadEnergyRecordList[];
extern  uint32 oadEnergyRecordListLen;

/* 最大需量类OAD列表 OIA1 = 0x01 */
extern  OAD_RECORD_LIST oadDemandRecordList[];
extern  uint32 oadDemandRecordListLen;

/* 变量类OAD列表 OIA1 = 0x02 */
extern  OAD_RECORD_LIST oadVariateRecordList[];
extern  uint32 oadVariateRecordListLen;

/* 事件类OAD列表 OIA1 = 0x03 */
extern  OAD_RECORD_LIST oadEvnetRecordList[];
extern  uint32 oadEvnetRecordListLen;

/* 参变量类OAD列表 OIA1 = 0x04 */
extern  OAD_RECORD_LIST oadParaRecordList[];
extern  uint32 oadParaRecordListLen;

/* 冻结类OAD列表 OIA1 = 0x05 */
extern  OAD_RECORD_LIST oadFreezeRecordList[];
extern  uint32 oadFreezeRecordListLen;

/* 采集类OAD列表 OIA1 = 0x06 */
extern  OAD_RECORD_LIST oadCollectRecordList[];
extern  uint32 oadCollectRecordListLen;

/* 集合类OAD列表 OIA1 = 0x07 */
extern  OAD_RECORD_LIST oadGatherRecordList[];
extern  uint32 oadGatherRecordListLen;

/* 控制接口类OAD列表 OIA1 = 0x08 */
extern  OAD_RECORD_LIST oadControlRecordList[];
extern  uint32 oadControlRecordListLen;

/* 文件传输接口类OAD列表 OIA1 = 0x0F OIA2 = 0x00 */
extern  OAD_RECORD_LIST oadFileRecordList[];
extern  uint32 oadFileRecordListLen;

/* ESAM接口类OAD列表 OIA1 = 0x0F OIA2 = 0x01 */
extern  OAD_RECORD_LIST oadEsamRecordList[];
extern  uint32 oadEsamRecordListLen;

/* 输入输出类OAD列表 OIA1 = 0x0F OIA2 = 0x02 */
extern  OAD_RECORD_LIST oadIoRecordList[];
extern  uint32 oadIoRecordListLen;


#ifdef __cplusplus
}
#endif
#endif //__OAD_LIST_H__