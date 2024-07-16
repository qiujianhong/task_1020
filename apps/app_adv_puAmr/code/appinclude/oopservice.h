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
#include "appcomm.h"
#include "apducomm.h"
#include "oadInfoList.h"

/** @brief 接口类数量   */
#define OOP_IC_MAX  26

//请求报文到OAD处的偏移
#define OOP_DATA_OFFSET  3
#define OOP_DATA_OFFSET_NEXT  8

//LIST请求报文到OAD处的偏移
#define OOP_DATA_LIST_OFFSET  4

//LIST型数据的最大节点数 按 SEQUENCE OF的个数编码为1字节推算
#define OOP_DATA_LIST_MAX  255

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
    RES_RPT_MINUTE_DATA         = 6,  //上报透明数据的响应
}PRTL_RES_TYPE_E;

typedef struct{
    uint8           appSlicingFlag;        //应用分帧标识
    uint8           isEnd;                 //是否末尾帧
    OOP_OAD_U       serviceOAD;            //记录型数据的主oad(如60120300)
    uint8           seviceapdu[3];         //读取型记录的服务类型
    uint16          seq;                   //应用分帧序号
    uint8           objPre;                //分帧响应对象属性 1-普通OAD 2-记录型OAD
    uint8           reqType;               //响应类型apdu[1]
    uint16          metsn;                 //60120300记录的测量点信息
    OOP_OCTETSTR6_T MAC;                   //附加校验，起始就表地址
    DateTimeBcd_t   dateTimeTag;           //60120300记录的读数据的时标
    uint8           theLastNTimes;         //60120300记录的剩余未报的上n次
    #ifdef AREA_FUJIAN
    uint8           node_cnt;              //福建规约使用，存方法52的断点信息
    #endif
    uint8           ms_cnt;                //60120300单个记录的时间点信息
    uint32          tmStartSeg;            //这条数据内容的时间结束点，用于判断是否取完所有记录
    uint32          recNumSeg;             //记录条数断点（方法9 10）
    uint32          index;                 //序号普通数据使用,用成功时间筛选时，记下到哪个数据点
    uint32          oadNo;                 //05 02 读多个OAD时，如果应用分帧，记录下OAD断点(倒序)。
    uint32          oadNum;                //05 02 读多个OAD时，如果应用分帧，记录下OAD数量。
    uint8           channelid;             //通道id
    uint8           piid;
    uint8          *frameaddr;
}__attribute__ ((packed)) BP_INFO_T;

/** @brief 应用分帧断点信息 */
typedef struct 
{
    DB_CLIENT          hUdp;
    uint8              logicAddr;
    uint8              reqType;
    uint8              NoEsamMac;
    uint8              isDateNull;
    APP_CONTEXT_t      connectionInfo;  //应用连接信息
    BP_INFO_T          *bpBreakInfo;    //断点信息

    //山东需求方法10的数据要上报任务周期筛选
    #ifdef AREA_SHANDONG
    OOP_TI_T  runTi;
    BOOL  isreport;//表示是否是上报触发的规约召测
    #endif

    //福建规约需要读上一次接受到主站通信报文时间
    #ifdef AREA_FUJIAN
    OOP_DATETIME_S_T   *pLastRecvTime;   //上次收到报文时间
    #endif
}APDU_INFO_T;

/** @brief 数据结果信息 */
typedef struct 
{
    uint32  resultDataLen;      // 数据结果长度
    uint8   *pResultData;       // 数据结果数据区指针
}RESULT_DATA_T;

typedef OOP_DAR_E (*ClassInvokeGet_t)(OAD_INFO_T *,APDU_INFO_T *, const uint8* ,uint16 ,RESULT_DATA_T*, uint16);
typedef OOP_DAR_E (*ClassInvokeSet_t)(OAD_INFO_T *,APDU_INFO_T *, const uint8* ,uint16 ,RESULT_DATA_T*, uint16);
typedef OOP_DAR_E (*ClassInvokeAct_t)(OAD_INFO_T *,APDU_INFO_T *, const uint8* ,uint32 ,RESULT_DATA_T*, uint16);

/** @brief 接口类操作函数 */
typedef struct
{
    uint8 ClassID;
    ClassInvokeGet_t InvokGet;
    ClassInvokeSet_t InvokSet;
    ClassInvokeAct_t InvokAct;
}PRTL_IC_OPERATE_T;

uint16 analyse_puAmr_acMeter(CHANNEL_CACHE_T *ptChannel, FRAME_HEAD_T *ptFrame, uint8 *pOutMsg, uint16 msgmaxlen, 
                             uint8 *pApduBreak);

uint16 get_request_analyse(CHANNEL_CACHE_T *ptChannel, FRAME_HEAD_T *ptFrame, uint8 *pOutMsg, uint16 msgmaxlen, 
                             uint8 *pApduBreak);

uint16 set_request_analyse(CHANNEL_CACHE_T *ptChannel, FRAME_HEAD_T *ptFrame, uint8 *pOutMsg, uint16 msgmaxlen, 
                             uint8 *pApduBreak);

uint16 action_request_analyse(CHANNEL_CACHE_T *ptChannel, FRAME_HEAD_T *ptFrame, uint8 *pOutMsg, uint16 msgmaxlen, 
                             uint8 *pApduBreak);
uint16 report_response_analyse(CHANNEL_CACHE_T *ptChannel, FRAME_HEAD_T *ptFrame, uint8 *pOutMsg, uint16 msgmaxlen, 
                             uint8 *pBreakFlag);

uint16 invoke_get_req(CHANNEL_CACHE_T *ptChannel, const uint8 *pInMsg, uint16 inLen, uint8 *pOutMsg, uint16 msgmaxlen);

uint16 invoke_set_req(CHANNEL_CACHE_T *ptChannel, const uint8 *pInMsg, uint16 inLen, uint8 *pOutMsg, uint16 msgmaxlen);

uint16 invoke_act_req(CHANNEL_CACHE_T *ptChannel, const uint8 *pInMsg, uint32 inLen, uint8 *pOutMsg, uint16 msgmaxlen);
//将apdu缓存住 测试的时候不是每次都是相同的48个 有两种 所有缓存改成100个
#define MAX_APDU_CACHE_NUM   100
typedef struct
{
    OOP_OAD_U oad;
    time_t apdutime;    //记录下apdu写进来的时间
    uint8 apdu[256];
    uint16 apdulen;
}APDU_CACHE_T;
typedef struct
{
    pthread_rwlock_t lock;
    uint16 cacheNum;
    APDU_CACHE_T cacheList[MAX_APDU_CACHE_NUM];
}ALL_APDU_CACHE_T;

void apdu_cache_init();
uint16 apdu_cache_get(OOP_OAD_U oad,uint8* outbuf);
int apdu_cache_add(OOP_OAD_U oad,uint8* inbuf,uint16 inlen);

#endif /*__OOP_SERVICE_H__*/

