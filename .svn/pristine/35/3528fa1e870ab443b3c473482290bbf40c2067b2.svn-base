/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief：规约解析公共函数
* @date： 2019.12.03
*********************************************************************
*/

#ifndef __COMM_CLASS_H__
#define __COMM_CLASS_H__

#include "framecomm.h"
#include "sys_cfg.h"
#include "oopservice.h"
#include "oadInfoList.h"
#include "db_api.h"

#ifdef __cplusplus
extern "C"{
#endif

#define BCD2BIN(val)    (((val) & 0x0f) + ((val)>>4)*10)
#define BIN2BCD(val)    ((((val)/10)<<4) + (val)%10)
#define BCD_TO_BIN(val) ((val)=BCD2BIN(val))
#define BIN_TO_BCD(val) ((val)=BIN2BCD(val))

#define BUF_LEN_MAX 8192

typedef struct 
{
    uint32              nIndex;         /**< 事件记录序号 */
    OOP_DATETIME_S_T    tmStart;        /**< 事件发生时间 */
    OOP_DATETIME_S_T    tmEnd;          /**< 事件结束时间 */
    OOP_EVTSOURCE_T     source;         /**< 事件发生源      */   
    OOP_REPORTS_T       status;          /**< 事件上报状态 */   
}EVT_BASIC_T;

/*程序内部使用的MS_T集合*/
typedef struct MS{
	// uint8		    prtl;
	// uint8   	    port;
	// uint16		    MtrNo;
    // OOP_OCTETSTR6_T MAC;                //附加校验
	// uint8           Rdret;
	// BOOL            needCBagain;        //补抄标志(方案周期执行补抄标识,首次置默认需要补抄)
    uint16		    MtrNo;
    OOP_METER_T     oopMeter;
	struct MS*      next;
}MS_T;

/*程序内部使用的RSD*/
typedef struct{
	uint8	  selId;				//方法id
	uint8	  LastN;				//上n条记录
	OOP_OAD_U SelOad;				//起始OAD
	uint8     *StartData;			//选择的数据
	uint8     *EndData;			    //选择的数据
	OOP_TI_T  Ti;					//数据间隔
	MS_T	  *Ms;
}RSD_T;

/*程序内部使用的ROAD*/
typedef struct{
	OOP_OAD_U   MainOad;
	uint8       SubOadNum;
	OOP_OAD_U*  SubOad;
}ROAD_T;

/*程序内部使用的CSD*/
typedef struct CSD_{
	uint8       CSDType;
	OOP_OAD_U   Oad;
	ROAD_T      Road;
	struct CSD_ *next;
}CSD_T, *LinkedCSDList;

/*记录数据存储接口依赖数据*/
typedef struct _RELY_INFO_T
{
    MS_T       *ms;              //ms
    OOP_PRSD_T  *oopRsd;         //RSD    
    OOP_RCSD_T  oopRcsd;         //RCSD
    BP_INFO_T   *bpBreakInfo;    //断点信息
    uint8       method;          //记录型数据的读取方式 0-record 1-normal   
    OAD_INFO_T *pOadInfo; 
}RELY_INFO_T;

//一个缓冲区信息结构
typedef struct 
{
    void   *buffer;   //数据缓冲区首地址
    uint32  bufLen;   //缓冲区总大小
    uint32  *offset;  //偏移即实际使用长度    
}BUFFER_INFO_T;

//数据转换结构
typedef struct 
{
    void   *srcBuf;    //数据
    uint32  srcLen;    //长度
    uint32 *sOffset;   //偏移   

    void   *dstBuf;    //数据
    uint32  dstLen;    //长度
    uint32 *dOffset;   //偏移      
}DATA_CONVERT_T;

/* 记录数据存储接口依赖数据 */
typedef struct 
{
    uint8            logicId;        //逻辑地址
    uint16           infoNum;        //信息点    
    OOP_OCTETVAR64_T MAC;            //附加校验    
    BP_INFO_T        *bpInfo;        //断点
    const OAD_INFO_T *pOadInfo;      //主DI的查表信息
    BOOLEAN              bNormal;       //普通方式读  
}RECORD_APPEND_T;

/* 记录数据筛选信息 */
typedef struct 
{
    OOP_OAD_U       oopOad;         //OAD
    OOP_PRSD_T      oopRsd;         //RSD    
    OOP_RCSD_T      oopRcsd;        //RCSD
    RECORD_APPEND_T append;         //附加信息
}RECORD_INFO_T;

typedef OOP_DAR_E (*CLASS_CONV_FUNC)(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert);

/*接口类转换函数集*/
typedef struct {
    uint8            ClassID;
    CLASS_CONV_FUNC  Data2Buf;
    CLASS_CONV_FUNC  Buf2Data;
}IC_COV_T;

//一个数组信息结构
typedef struct 
{
    uint32  arrLen;       //数组结构体的大小
    uint32  maxNum;       //数组最大成员数 
    uint32  unitNum;      //数组当前成员数 
    uint32  unitSize;     //每个成员大小 
    uint32  rsv;          //保留位,用于识别那个元素是有值的
    void   *pArray;       //数组首地址
    CLASS_CONV_FUNC func; //成员转换函数
}ARRAY_INFO_T;

/*OAD信息和OAD*/
typedef struct {
    uint8 choice;          //0-oad 1-oadInfo
    union 
    {
        OOP_OAD_U  oad;
        OAD_INFO_T *pstOdaInfo;
    };
    uint8    subParse;      //解析子结构，用于关联OAD， 1-是，0-否
}IC_OAD_INFO_T;


/**
*********************************************************************
* @name：       class_oadinfo_get
* @brief：      查询ODA列表，得到OAD描述信息
* @param[in] ：OOP_OAD_U oad        查询OAD
* @param[out]：OAD_INFO_T *info     ODA信息
* @return    ：查询结果 TRUE-成功，FALSE-失败
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
int class_oadinfo_get(OOP_OAD_U oad, OAD_INFO_T *pstOdaInfo);


/**********************************************************************
* @name      : get_len_offset
* @brief     ：获取串类型数据元素长度
* @param[in] ：*indata      数据Buffer
* @param[out]：*bytes       数据Buffer长度
* @return    ：长度
* @Create    : 
* @Date      ：2019-10-30
* @Update    :
**********************************************************************/
uint16 get_len_offset(uint8 *indata,uint8 *bytes);


/**
*********************************************************************
* @brief：     从数据中心的数据转698格式报文(class1)
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_class1(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *dsdata);

/**
*********************************************************************
* @brief：     从数据中心的数据转698格式报文(class1)
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_class2(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *dsdata);

/**
*********************************************************************
* @brief：     从数据中心的数据转698格式报文(class3)
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_class3(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *dsdata);

/**
*********************************************************************
* @brief：     从数据中心的数据转698格式报文(class4)
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_class4(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *dsdata);

/**
*********************************************************************
* @brief：     从数据中心的数据转698格式报文(class5)
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_class5(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *dsdata);


/**
*********************************************************************
* @brief：     从数据中心的数据转698格式报文(class6)
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_class6(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *dsdata);

/**
*********************************************************************
* @brief：     从数据中心的数据转698格式报文(class8)
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_class7(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert);

/**
*********************************************************************
* @brief：     从数据中心的数据转698格式报文(class8)
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_class8(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert);
OOP_DAR_E data_to_buf_class19(OAD_INFO_T* pOadInfo, DATA_CONVERT_T* convert);
extern void *memcpy_r(void *dest, const void *src, int count);
extern uint8 set_len_offset(uint16 eleLen, uint8 *indata);

extern OOP_DAR_E class_array_tofmt698(OAD_INFO_T *pOadInfo, ARRAY_INFO_T *pArrayInfo, DATA_CONVERT_T *convert);
extern int32 basic_datetime_s_tofmt698(void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen);
OOP_DAR_E normal_data_to_fmt698(const IC_OAD_INFO_T *pIcInfo, DATA_CONVERT_T *convert);
extern OOP_DAR_E basic_data_to_fmt698(DATA_E eData, DATA_CONVERT_T *convert);
OOP_DAR_E basic_data_from_fmt698(DATA_E eData, DATA_CONVERT_T *convert);
extern OOP_DAR_E basic_db_to_fmt698(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert);
extern OOP_DAR_E basic_data_to_buf(DATA_E eData, void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen);
extern OOP_DAR_E basic_buf_to_data(DATA_E eData, const void *srcBuf, uint32 srcLen, uint32 *pOffset, void *dstData, uint32 dstLen);
int32 data_to_buf_evt_oad(OOP_OAD_U oad, void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen);
OOP_DAR_E oop_null_data_res(const OAD_INFO_T *pOadInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax);

OOP_DAR_E data_to_buf_evt_report(const OAD_INFO_T *pOdaInfo, void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen);
OOP_DAR_E data_to_buf_evt_source(const OAD_INFO_T *pOdaInfo, void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen);
OOP_DAR_E data_to_buf_evt_std(const OAD_INFO_T *pOadInfo, void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen);
OOP_DAR_E basic_db_from_fmt698(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert);

extern void datetime_s_data_to_buf(OOP_DATETIME_S_T tmVal, uint32 *offset, uint8 *apduValue);

extern int combine_sub_data_class3_buff(uint8 *pDesbuffer, uint16 *nDesLen, uint32 OAD);
extern int combine_sub_data_class4_buff(uint8 *pDesbuffer, uint16 *nDesLen, uint32 OAD);

extern OOP_DAR_E class7_invoke_set(OAD_INFO_T *pOadInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax);

extern int combine_sub_data_class8_buff(uint8 *pDesbuffer, uint16 *nDesLen, uint32 OAD);
extern int combine_time_normal_buff(uint8 *pDesbuffer, uint16 *nDesLen);
extern OOP_DAR_E class8_invoke_set_volt_rate(OAD_INFO_T *pOadInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax);

extern OOP_DAR_E class8_invoke_set_time(OAD_INFO_T *pOadInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax);
extern OOP_DAR_E class8_invoke_set_time_zone(OAD_INFO_T *pOadInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax);
extern OOP_DAR_E class8_invoke_set_day_period(OAD_INFO_T *pOadInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax);
extern OOP_DAR_E class8_invoke_set_demand_period(OAD_INFO_T *pOadInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax);
extern OOP_DAR_E class8_invoke_set_slip_time(OAD_INFO_T *pOadInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax);
extern OOP_DAR_E class8_invoke_set_time_region(OAD_INFO_T *pOadInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax);
extern OOP_DAR_E class8_invoke_set_change_time(OAD_INFO_T *pOadInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax);
extern OOP_DAR_E class8_invoke_set_feature(OAD_INFO_T *pOadInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax);
OOP_DAR_E class7_invoke_get(OAD_INFO_T *pOdaInfo, const uint8 *inData, uint16 inLen, RESULT_DATA_T *outData, uint16 outDataMax);
OOP_DAR_E class9_invoke_get(OAD_INFO_T *pOdaInfo, const uint8 *inData, uint16 inLen, RESULT_DATA_T *outData, uint16 outDataMax);

extern OOP_DAR_E frozen_cfg_unit_del(const OAD_INFO_T *pOadInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax);
extern OOP_DAR_E frozen_cfg_batch_add(const OAD_INFO_T *pOadInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax);
int32 evt_all_rcsd_get(OAD_INFO_T *pOadInfo, uint16 *pOffset, uint8 *pOutData, uint16 outDataMax);
int32 frozen_all_rcsd_get(OAD_INFO_T *pOadInfo, DB_CLIENT fd,  uint16 *pOffset, uint8 *pOutData, uint16 outDataMax);
int32 all_rcsd_get(OAD_INFO_T *pOadInfo, DB_CLIENT fd, uint16 *pOffset, uint8 *pOutData, uint16 outDataMax);
OOP_DAR_E apdu_rcsd_fromfmt698(const void *srcBuf, uint32 srcLen, uint32 *pOffset, void *dstData, uint32 dstLen);
OOP_DAR_E apdu_rcsd_tofmt698(void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen);
OOP_DAR_E apdu_rsd_fromfmt698(const void *srcBuf, uint32 srcLen, uint32 *pOffset, void *dstData, uint32 dstLen);
BOOLEAN apdu_next_position_get(const uint8* inData, uint16* offset, uint16 inDataLen, OOP_DATATYPE_E DataType, BOOLEAN bWithType);
BOOL  taskmng_oop_data_extract(const uint8* inData, uint16* offset, uint16 inDataLen, uint8* tembuffer, enum tag_OOP_DATATYPE DataType, BOOL bWithType);
#ifdef __cplusplus
}
#endif

#endif /*__COMM_CLASS_H__*/

