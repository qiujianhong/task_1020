/*
*********************************************************************
* @file    apduservice.c
* @brief： apdu应用服务处理
* @author：贺宁
* @date：   2019.12.06
* @copyright 2019 南京新联电子股份有限公司 All rights reserved.
*********************************************************************
*/
#ifndef __OOP_SERVICE_H__
#define __OOP_SERVICE_H__


#include "baseCommon.h"

/** @brief 接口类数量   */
#define OOP_IC_MAX  26

//请求报文到OAD处的偏移
#define OOP_DATA_OFFSET  3
#define OOP_DATA_OFFSET_NEXT  8

//LIST请求报文到OAD处的偏移
#define OOP_DATA_LIST_OFFSET  4

//LIST型数据的最大节点数 按 SEQUENCE OF的个数编码为1字节推算
#define OOP_DATA_LIST_MAX  255

//终端最大发送帧长度
#define LOCAL_RECV_FRAM_MAXLEN             4224

//UDP收发最大长度
#define MAX_UDP_BUFF_LEN                   4224                      


/** @brief 请求类型枚举   */
typedef enum
{
    REQ_GET_NORMAL             = 1,
    REQ_GET_NORMAL_LIST        = 2,        
    REQ_GET_RECORD             = 3,   
    REQ_GET_RECORD_LIST        = 4,   
    REQ_GET_NEXT               = 5,   
    REQ_GET_MD5                = 6,   

    REQ_SET_NORMAL             = 1,
    REQ_SET_NORMAL_LIST        = 2,        
    REQ_STG_NORMAL_LIST        = 3,    

    REQ_ACT_NORMAL             = 1,
    REQ_ACT_NORMAL_LIST        = 2,        
    REQ_ATG_NORMAL_LIST        = 3,
    
}PRTL_REQ_TYPE_E;

/** @brief 响应类型枚举   */
typedef enum
{
    RES_RPT_NORMAL_LIST        = 1,  //上报若干个对象属性响应
    RES_RPT_RECORD_LIST        = 2,  //上报若干个记录型对象属性的响应        
    RES_RPT_TRANS_DATA         = 3,  //上报透明数据的响应
}PRTL_RES_TYPE_E;

typedef struct{
    uint8           appSlicingFlag;        //应用分帧标识
    uint8           isEnd;                 //是否末尾帧
    OOP_OAD_U       serviceOAD;            //记录型数据的主oad(如60120300)
    uint8           seviceapdu[3];         //读取型记录的服务类型
    uint16          seq;                   //应用分帧序号
    uint8           objPre;                //分帧响应对象属性
    uint8           reqType;               //响应类型apdu[1]
    uint16          metsn;                 //60120300记录的测量点信息
    OOP_OCTETSTR6_T MAC;                   //附加校验，起始就表地址
    DateTimeBcd_t   dateTimeTag;           //60120300记录的读数据的时标
    uint8           theLastNTimes;         //60120300记录的剩余未报的上n次
    uint8           ms_cnt;                //60120300单个记录的时间点信息
    uint32          tmStartSeg;            //这条数据内容的时间结束点，用于判断是否取完所有记录
    uint32          recNumSeg;             //记录条数断点（方法9 10）
    uint32          index;                 //序号普通数据使用,用成功时间筛选时，记下到哪个数据点
}__attribute__ ((packed)) BP_INFO_T;

/** @brief 数据结果信息 */
typedef struct 
{
    uint32  resultDataLen;      // 数据结果长度
    uint8   pResultData[LOCAL_RECV_FRAM_MAXLEN];       // 数据结果数据区指针
}RESULT_DATA_T;



#endif /*__OOP_SERVICE_H__*/

