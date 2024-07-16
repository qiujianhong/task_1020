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
#include "oopservice.h"
#include "oopSecurity.h"
#include "oopVarUpdate.h"

#ifdef __cplusplus
extern "C"{
#endif

#define BCD2BIN(val)    (((val) & 0x0f) + ((val)>>4)*10)
#define BIN2BCD(val)    ((((val)/10)<<4) + (val)%10)
#define BCD_TO_BIN(val) ((val)=BCD2BIN(val))
#define BIN_TO_BCD(val) ((val)=BIN2BCD(val))

#define BUF_LEN_MAX 8192

#ifdef AREA_FUJIAN
#define NODE_ID_NUM_MAX      2040    //福建规约最大任务数
#endif

#define METER_READ_MOD       0       //0 从缓存中读 1 直接从数据中心读
#define ACQ_MONITOR_READ_MOD 1       //0 从缓存中读 1 直接从数据中心读
#define NOR_PLAN_READ_MOD    1       //0 从缓存中读 1 直接从数据中心读
#define EVENT_PLAN_READ_MOD  1       //0 从缓存中读 1 直接从数据中心读
#define TRANSPARENT_PLAN_READ_MOD  1 //0 从缓存中读 1 直接从数据中心读
#define REPORT_PLAN_READ_MOD  1      //0 从缓存中读 1 直接从数据中心读

#define TASK_READ_MOD        1       //0 从缓存中读 1 直接从数据中心读
#define SEEK_METER_MOD       0       //0 从缓存中读 1 直接从数据中心读
#define SEEK_ACROSS_MOD      0       //0 从缓存中读 1 直接从数据中心读
#define CHILDNODE_READ_MOD   0       //0 从缓存中读 1 直接从数据中心读
#define NODEVERSION_READ_MOD 0       //0 从缓存中读 1 直接从数据中心读
#define ENCRYPTION_MOD       1       //0 从缓存中读 1 直接从数据中心读

#define DEVINFO_DIR          "/data/devinfo/"   // appid文件目录
#define APP_ID_LEN           24                 //appid长度

typedef enum
{
    PRTL_INIT_DATA  = 0, /**< 数据初始化 */
    PRTL_INIT_PARAM = 1, /**< 参数初始化 */
}PRTL_INIT_TYPE;

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

    #ifdef AREA_FUJIAN
    uint16    transTaskid;                //透明任务方案号
    uint16    nodeidNum;                  //具体任务编号数量
    uint16    nodeid[NODE_ID_NUM_MAX];    //具体任务编号
    uint8     recNum;                     //记录条数
    uint8     isAllmeter;                 //是否要筛选所有表地址
    #endif
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

    #ifdef AREA_FUJIAN
    uint16      nodeidNum;                  //具体任务编号数量
    uint16      nodeid[NODE_ID_NUM_MAX];    //具体任务编号
    uint8	    LastN;                      //上n条记录
    uint8       recNum;                     //m条记录数
    uint8       isAllmeter;                 //是否要筛选所有表地址
    #endif
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

/*记录型档案数据刷选条件*/
typedef struct _READ_METER_T
{
    uint32          startInfoNum;                      //信息点
    uint32          endInfoNum;                        //信息点
    uint8           isValid[NO_VIR_MTR_MAX_NUM + 1];   //适应方法3，0有效，1无效
}READ_METER_T;

/*记录型档案数据刷选条件(普通数据),带OCT类型*/
typedef struct _READ_NORMOAL_OCT_T
{
    uint32           startInfoNum;                      //信息点
    uint32           endInfoNum;                        //信息点
    OOP_OCTETVAR16_T octVal;                            //名称
}READ_NOR_OCT_T;

/*记录型档案数据刷选条件(普通数据)*/
typedef struct _READ_NORMOAL_BP_T
{
    uint32          startInfoNum;                      //信息点
    uint32          endInfoNum;                        //信息点
}READ_NOR_BP_T;

/*功率特征阶段记录*/
typedef struct tag_Power_Characteristic_Record    
{
   uint8 phase;
   uint8 type;
   int32 power;
   uint8 startminute;
   uint8 startsecond;

}OOP_Power_Characteristic_Record_Unit;

typedef struct tag_Power_Characteristic    
{
   uint8 devtype;
   uint8 num;//
   OOP_Power_Characteristic_Record_Unit powerunit[12];  

}OOP_Power_Characteristic_Record;

typedef struct tag_Vlotage    
{
   uint8 num;
   uint16 voltageuint[3];

}OOP_Vlotage_Unit;

typedef struct tag_Energy    
{
   uint8 phase;
   int32 energy;
   int32 activepower;
   int32 reactivepower;
   int32 current;
   int32 currentoddhaimonic;
   int32 currentevenhaimonic;
}OOP_Energy_Unit;

typedef struct tag_Energy_Record    
{
   uint8 num;
   OOP_Energy_Unit energyuint[3];

}OOP_Energy_Record_Unit;

typedef struct tag_Energy_Characteristic_Record    
{
   uint8 devtype;
   OOP_Vlotage_Unit vlotage;
   OOP_Energy_Record_Unit energyuint;

}OOP_Energy_Characteristic_Record_Unit;

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
int class_omdinfo_get(OOP_OAD_U oad, OAD_INFO_T *pstOdaInfo);

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

/**
*********************************************************************
* @name：       apdu_bpinfo_free
* @brief：      释放断点信息
* @param[in] ：pInfo       要释放的对象指针
* @return    ：无
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
void apdu_bpinfo_free(void *pInfo);

/**********************************************************************
* @name      : set_len_offset
* @brief     ：设置元素长度
* @param[in] ：eleLen      输入数据长度
* @param[out]：indata      数据Buffer
* @return    ：数据Buffer长度
* @Create    : 
* @Date      ：2019-10-30
* @Update    :
**********************************************************************/
uint8 set_len_offset(uint16 eleLen, uint8 *indata);

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
* @brief：     从数据中心的数据转698格式报文(通用基本数据)
* @param[in]： ppOadInfo     - 数据表指针
               pOffset       - 数据指针偏移
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
int32 data_to_buf_common(OAD_INFO_T *pOadInfo, uint32 *oadInfoOffset, void *srcData, uint32 srcLen, void *dstBuf, uint32 *dstLen);

/**
*********************************************************************
* @name：       apdu_next_dpos_get
* @brief：      获取报文指定位置处，指定类型数据的长度，也即下一个数据位置
* @param[in] ：inData       输入报文
*              inDataLen   报文长度
*              DataType    需计算长度的数据类型
*              bWithType   1-数据类型由入参携带，0-数据类型从报文获取
* @param[out]：offset       输出的指定类型长度，即从inData的开始的DataType偏移量
* @return    ：查询结果 TRUE-成功，FALSE-失败
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
BOOLEAN apdu_next_dpos_get(const uint8 *inData, uint16 *offset, uint16 inDataLen, OOP_DATATYPE_E DataType, BOOLEAN bWithType);

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
* @brief：     从数据中心的数据转698格式报文(class7)
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

/**
*********************************************************************
* @brief：     从数据中心的数据转698格式报文(采集档案单元)
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
int32 data_to_buf_class8_oop_meter(OAD_INFO_T *pOadInfo, uint32 *oadInfoOffset, void *srcData, uint32 srcLen, void *dstBuf, uint32 *dstLen, uint32 outMaxLen);

int32 buf_to_data_class8_event_scheme(OAD_INFO_T *pOadInfo,  const void *srcBuf, uint32 srcLen, uint32 *pOffset, int hUdp);

/**
*********************************************************************
* @brief：     搜表结果
* @param[in]： ppOadInfo     - 数据表指针
               srcBuf       - 源数据
               srcLen       - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen       - 目的数据长度
               sOffset      - 转换后源数据区的偏移
               dOffset      - 转换后目的数据区的偏移
* @return：     =0           - 成功
              !=0           - 错误码
* @author    : 贺宁
* @Date      ：2019-12-6                 
*********************************************************************
*/
OOP_DAR_E data_to_buf_class8_searchmeter_res_all(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert);

/**
*********************************************************************
* @brief：     搜表结果
* @param[in]： ppOadInfo     - 数据表指针
               srcBuf       - 源数据
               srcLen       - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen       - 目的数据长度
               sOffset      - 转换后源数据区的偏移
               dOffset      - 转换后目的数据区的偏移
* @return：     =0           - 成功
              !=0           - 错误码
* @author    : 贺宁
* @Date      ：2019-12-6                 
*********************************************************************
*/
OOP_DAR_E data_to_buf_class8_searchmeter_res_acrossarea(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert);

/**
*********************************************************************
* @brief：     从数据中心的数据转698格式报文(class9)
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_class9(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert);

/**
*********************************************************************
* @brief：     从数据中心的数据转698格式报文(class10)
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_class10(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert);

/**
*********************************************************************
* @brief：     从数据中心的数据转698格式报文(class11)
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_class11(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert);

/**
*********************************************************************
* @brief：     从数据中心的数据转698格式报文(class12)
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_class12(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert);

/**
*********************************************************************
* @brief：     从数据中心的数据转698格式报文(class13)
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_class13(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert);

/**
*********************************************************************
* @brief：     从数据中心的数据转698格式报文(class14)
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_class14(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert);

/**
*********************************************************************
* @brief：     从数据中心的数据转698格式报文(class15)
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_class15(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert);

/**
*********************************************************************
* @brief：     从数据中心的数据转698格式报文(class16)
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_class16(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert);

/**
*********************************************************************
* @brief：     从数据中心的数据转698格式报文(class17)
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/

OOP_DAR_E data_to_buf_class17(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert);
/**
*********************************************************************
* @brief：     从数据中心的数据转698格式报文(class18)
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/

OOP_DAR_E data_to_buf_class18(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert);
/**
*********************************************************************
* @brief：     从数据中心的数据转698格式报文(class19)
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/

OOP_DAR_E data_to_buf_class19(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert);
/**
*********************************************************************
* @brief：     从数据中心的数据转698格式报文(class20)
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/

OOP_DAR_E data_to_buf_class20(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert);
/**
*********************************************************************
* @brief：     从数据中心的数据转698格式报文(class21)
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/

OOP_DAR_E data_to_buf_class21(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert);
/**
*********************************************************************
* @brief：     从数据中心的数据转698格式报文(class22)
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/

OOP_DAR_E data_to_buf_class22(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *dsdata);
/**
*********************************************************************
* @brief：     从数据中心的数据转698格式报文(class23)
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/

OOP_DAR_E data_to_buf_class23(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *dsdata);
/**
*********************************************************************
* @brief：     从数据中心的数据转698格式报文(class24)
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/

OOP_DAR_E data_to_buf_class24(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *dsdata);
/**
*********************************************************************
* @brief：     从数据中心的数据转698格式报文(class25)
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_class25(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *dsdata);

/**
*********************************************************************
* @brief：     从数据中心的数据转698格式报文(class26)
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_class26(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert);
OOP_DAR_E data_to_buf_oop_meter(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert);
OOP_DAR_E data_to_buf_class8_acq_monitor(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert);
OOP_DAR_E data_to_buf_acq_rule(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert);
OOP_DAR_E data_to_buf_null(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert);
int32 data_to_buf_normal_scheme(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert);
int32 data_to_buf_event_scheme(OAD_INFO_T *pOadInfo,  DATA_CONVERT_T *convert);
OOP_DAR_E data_to_buf_container_info(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert);

int32 buf_to_data_class8_normal_scheme(OAD_INFO_T *pOadInfo, void *srcBuf, uint32 srcLen, uint32 *pOffset, int hUdp);
int32 buf_to_data_class8_oop_meter(OAD_INFO_T *pOadInfo, uint32 *oadInfoOffset, void *srcBuf, uint32 srcLen, uint32 *pOffset, void *dstData, uint32 dstLen);
OOP_DAR_E buf_to_data_oop_meter(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert);
OOP_DAR_E buf_to_data_oop_meter_basic_object(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert);
OOP_DAR_E buf_to_data_oop_meter_extend_object(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert);
OOP_DAR_E buf_to_data_oop_meter_annex_object(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert);

OOP_DAR_E buf_to_data_report_plan(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert);
OOP_DAR_E buf_to_data_acq_rule(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert);
OOP_DAR_E buf_to_data_encryption_plan(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert);
OOP_DAR_E class11_nodetopology_get(const OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax);

OOP_DAR_E buf_to_data_class1(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert);
OOP_DAR_E buf_to_data_class2(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert);
OOP_DAR_E buf_to_data_class3(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert);
OOP_DAR_E buf_to_data_class4(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert);
OOP_DAR_E buf_to_data_class5(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert);
OOP_DAR_E buf_to_data_class6(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert);
OOP_DAR_E buf_to_data_class7(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert);
OOP_DAR_E buf_to_data_class8(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert);
OOP_DAR_E buf_to_data_class9(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert);
OOP_DAR_E buf_to_data_class10(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert);
OOP_DAR_E buf_to_data_class11(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert);
OOP_DAR_E buf_to_data_class12(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert);
OOP_DAR_E buf_to_data_class13(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert);
OOP_DAR_E buf_to_data_class14(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert);
OOP_DAR_E buf_to_data_class15(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert);
OOP_DAR_E buf_to_data_class16(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert);
OOP_DAR_E buf_to_data_class17(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert);
OOP_DAR_E buf_to_data_class18(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert);
OOP_DAR_E buf_to_data_class19(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert);
OOP_DAR_E buf_to_data_class20(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert);
OOP_DAR_E buf_to_data_class21(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert);
OOP_DAR_E buf_to_data_class22(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert);
OOP_DAR_E buf_to_data_class23(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert);
OOP_DAR_E buf_to_data_class24(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert);
OOP_DAR_E buf_to_data_class25(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert);
OOP_DAR_E buf_to_data_class26(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert);
OOP_DAR_E buf_to_data_class8_set(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert);

OOP_DAR_E class1_invoke_get(OAD_INFO_T *pOdaInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 inLen, RESULT_DATA_T *outData, uint16 outDataMax);
OOP_DAR_E class1_invoke_set(OAD_INFO_T *pOdaInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 inLen, RESULT_DATA_T *outData, uint16 outDataMax);
OOP_DAR_E class1_invoke_act(OAD_INFO_T *pOdaInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint32 inLen, RESULT_DATA_T *outData, uint16 outDataMax);

OOP_DAR_E class2_invoke_get(OAD_INFO_T *pOdaInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 inLen, RESULT_DATA_T *outData, uint16 outDataMax);
OOP_DAR_E class2_invoke_set(OAD_INFO_T *pOdaInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 inLen, RESULT_DATA_T *outData, uint16 outDataMax);
OOP_DAR_E class2_invoke_act(OAD_INFO_T *pOdaInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint32 inLen, RESULT_DATA_T *outData, uint16 outDataMax);

OOP_DAR_E class3_invoke_get(OAD_INFO_T *pOdaInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 inLen, RESULT_DATA_T *outData, uint16 outDataMax);
OOP_DAR_E class3_invoke_set(OAD_INFO_T *pOdaInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 inLen, RESULT_DATA_T *outData, uint16 outDataMax);
OOP_DAR_E class3_invoke_act(OAD_INFO_T *pOdaInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint32 inLen, RESULT_DATA_T *outData, uint16 outDataMax);

OOP_DAR_E class4_invoke_get(OAD_INFO_T *pOdaInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 inLen, RESULT_DATA_T *outData, uint16 outDataMax);
OOP_DAR_E class4_invoke_set(OAD_INFO_T *pOdaInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 inLen, RESULT_DATA_T *outData, uint16 outDataMax);
OOP_DAR_E class4_invoke_act(OAD_INFO_T *pOdaInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint32 inLen, RESULT_DATA_T *outData, uint16 outDataMax);

OOP_DAR_E class5_invoke_get(OAD_INFO_T *pOdaInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 inLen, RESULT_DATA_T *outData, uint16 outDataMax);
OOP_DAR_E class5_invoke_set(OAD_INFO_T *pOdaInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 inLen, RESULT_DATA_T *outData, uint16 outDataMax);
OOP_DAR_E class5_invoke_act(OAD_INFO_T *pOdaInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint32 inLen, RESULT_DATA_T *outData, uint16 outDataMax);

OOP_DAR_E class6_invoke_get(OAD_INFO_T *pOdaInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 inLen, RESULT_DATA_T *outData, uint16 outDataMax);
OOP_DAR_E class6_invoke_set(OAD_INFO_T *pOdaInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 inLen, RESULT_DATA_T *outData, uint16 outDataMax);
OOP_DAR_E class6_invoke_act(OAD_INFO_T *pOdaInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint32 inLen, RESULT_DATA_T *outData, uint16 outDataMax);

OOP_DAR_E class7_invoke_get(OAD_INFO_T *pOdaInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 inLen, RESULT_DATA_T *outData, uint16 outDataMax);
OOP_DAR_E class7_invoke_set(OAD_INFO_T *pOdaInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 inLen, RESULT_DATA_T *outData, uint16 outDataMax);
OOP_DAR_E class7_invoke_act(OAD_INFO_T *pOdaInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint32 inLen, RESULT_DATA_T *outData, uint16 outDataMax);

OOP_DAR_E class8_invoke_get(OAD_INFO_T *pOdaInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 inLen, RESULT_DATA_T *outData, uint16 outDataMax);
OOP_DAR_E class8_invoke_set(OAD_INFO_T *pOdaInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 inLen, RESULT_DATA_T *outData, uint16 outDataMax);
OOP_DAR_E class8_invoke_act(OAD_INFO_T *pOdaInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint32 inLen, RESULT_DATA_T *outData, uint16 outDataMax);

OOP_DAR_E class9_invoke_get(OAD_INFO_T *pOdaInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 inLen, RESULT_DATA_T *outData, uint16 outDataMax);
OOP_DAR_E class9_invoke_set(OAD_INFO_T *pOdaInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 inLen, RESULT_DATA_T *outData, uint16 outDataMax);
OOP_DAR_E class9_invoke_act(OAD_INFO_T *pOdaInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint32 inLen, RESULT_DATA_T *outData, uint16 outDataMax);

OOP_DAR_E class10_invoke_get(OAD_INFO_T *pOdaInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 inLen, RESULT_DATA_T *outData, uint16 outDataMax);
OOP_DAR_E class10_invoke_set(OAD_INFO_T *pOdaInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 inLen, RESULT_DATA_T *outData, uint16 outDataMax);
OOP_DAR_E class10_invoke_act(OAD_INFO_T *pOdaInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint32 inLen, RESULT_DATA_T *outData, uint16 outDataMax);

OOP_DAR_E class11_invoke_get(OAD_INFO_T *pOdaInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 inLen, RESULT_DATA_T *outData, uint16 outDataMax);
OOP_DAR_E class11_invoke_set(OAD_INFO_T *pOdaInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 inLen, RESULT_DATA_T *outData, uint16 outDataMax);
OOP_DAR_E class11_invoke_act(OAD_INFO_T *pOdaInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint32 inLen, RESULT_DATA_T *outData, uint16 outDataMax);

OOP_DAR_E class12_invoke_get(OAD_INFO_T *pOdaInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 inLen, RESULT_DATA_T *outData, uint16 outDataMax);
OOP_DAR_E class12_invoke_set(OAD_INFO_T *pOdaInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 inLen, RESULT_DATA_T *outData, uint16 outDataMax);
OOP_DAR_E class12_invoke_act(OAD_INFO_T *pOdaInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint32 inLen, RESULT_DATA_T *outData, uint16 outDataMax);

OOP_DAR_E class13_invoke_get(OAD_INFO_T *pOdaInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 inLen, RESULT_DATA_T *outData, uint16 outDataMax);
OOP_DAR_E class13_invoke_set(OAD_INFO_T *pOdaInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 inLen, RESULT_DATA_T *outData, uint16 outDataMax);
OOP_DAR_E class13_invoke_act(OAD_INFO_T *pOdaInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint32 inLen, RESULT_DATA_T *outData, uint16 outDataMax);

OOP_DAR_E class14_invoke_get(OAD_INFO_T *pOdaInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 inLen, RESULT_DATA_T *outData, uint16 outDataMax);
OOP_DAR_E class14_invoke_set(OAD_INFO_T *pOdaInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 inLen, RESULT_DATA_T *outData, uint16 outDataMax);
OOP_DAR_E class14_invoke_act(OAD_INFO_T *pOdaInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint32 inLen, RESULT_DATA_T *outData, uint16 outDataMax);

OOP_DAR_E class15_invoke_get(OAD_INFO_T *pOdaInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 inLen, RESULT_DATA_T *outData, uint16 outDataMax);
OOP_DAR_E class15_invoke_set(OAD_INFO_T *pOdaInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 inLen, RESULT_DATA_T *outData, uint16 outDataMax);
OOP_DAR_E class15_invoke_act(OAD_INFO_T *pOdaInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint32 inLen, RESULT_DATA_T *outData, uint16 outDataMax);

OOP_DAR_E class16_invoke_get(OAD_INFO_T *pOdaInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 inLen, RESULT_DATA_T *outData, uint16 outDataMax);
OOP_DAR_E class16_invoke_set(OAD_INFO_T *pOdaInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 inLen, RESULT_DATA_T *outData, uint16 outDataMax);
OOP_DAR_E class16_invoke_act(OAD_INFO_T *pOdaInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint32 inLen, RESULT_DATA_T *outData, uint16 outDataMax);

OOP_DAR_E class17_invoke_get(OAD_INFO_T *pOdaInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 inLen, RESULT_DATA_T *outData, uint16 outDataMax);
OOP_DAR_E class17_invoke_set(OAD_INFO_T *pOdaInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 inLen, RESULT_DATA_T *outData, uint16 outDataMax);
OOP_DAR_E class17_invoke_act(OAD_INFO_T *pOdaInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint32 inLen, RESULT_DATA_T *outData, uint16 outDataMax);

OOP_DAR_E class18_invoke_get(OAD_INFO_T *pOdaInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 inLen, RESULT_DATA_T *outData, uint16 outDataMax);
OOP_DAR_E class18_invoke_set(OAD_INFO_T *pOdaInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 inLen, RESULT_DATA_T *outData, uint16 outDataMax);
OOP_DAR_E class18_invoke_act(OAD_INFO_T *pOdaInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint32 inLen, RESULT_DATA_T *outData, uint16 outDataMax);

OOP_DAR_E class19_invoke_get(OAD_INFO_T *pOdaInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 inLen, RESULT_DATA_T *outData, uint16 outDataMax);
OOP_DAR_E class19_invoke_set(OAD_INFO_T *pOdaInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 inLen, RESULT_DATA_T *outData, uint16 outDataMax);
OOP_DAR_E class19_invoke_act(OAD_INFO_T *pOdaInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint32 inLen, RESULT_DATA_T *outData, uint16 outDataMax);

OOP_DAR_E class20_invoke_get(OAD_INFO_T *pOdaInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 inLen, RESULT_DATA_T *outData, uint16 outDataMax);
OOP_DAR_E class20_invoke_set(OAD_INFO_T *pOdaInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 inLen, RESULT_DATA_T *outData, uint16 outDataMax);
OOP_DAR_E class20_invoke_act(OAD_INFO_T *pOdaInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint32 inLen, RESULT_DATA_T *outData, uint16 outDataMax);

OOP_DAR_E class21_invoke_get(OAD_INFO_T *pOdaInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 inLen, RESULT_DATA_T *outData, uint16 outDataMax);
OOP_DAR_E class21_invoke_set(OAD_INFO_T *pOdaInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 inLen, RESULT_DATA_T *outData, uint16 outDataMax);
OOP_DAR_E class21_invoke_act(OAD_INFO_T *pOdaInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint32 inLen, RESULT_DATA_T *outData, uint16 outDataMax);

OOP_DAR_E class22_invoke_get(OAD_INFO_T *pOdaInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 inLen, RESULT_DATA_T *outData, uint16 outDataMax);
OOP_DAR_E class22_invoke_set(OAD_INFO_T *pOdaInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 inLen, RESULT_DATA_T *outData, uint16 outDataMax);
OOP_DAR_E class22_invoke_act(OAD_INFO_T *pOdaInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint32 inLen, RESULT_DATA_T *outData, uint16 outDataMax);

OOP_DAR_E class23_invoke_get(OAD_INFO_T *pOdaInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 inLen, RESULT_DATA_T *outData, uint16 outDataMax);
OOP_DAR_E class23_invoke_set(OAD_INFO_T *pOdaInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 inLen, RESULT_DATA_T *outData, uint16 outDataMax);
OOP_DAR_E class23_invoke_act(OAD_INFO_T *pOdaInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint32 inLen, RESULT_DATA_T *outData, uint16 outDataMax);

OOP_DAR_E class24_invoke_get(OAD_INFO_T *pOdaInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 inLen, RESULT_DATA_T *outData, uint16 outDataMax);
OOP_DAR_E class24_invoke_set(OAD_INFO_T *pOdaInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 inLen, RESULT_DATA_T *outData, uint16 outDataMax);
OOP_DAR_E class24_invoke_act(OAD_INFO_T *pOdaInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint32 inLen, RESULT_DATA_T *outData, uint16 outDataMax);

OOP_DAR_E class25_invoke_get(OAD_INFO_T *pOdaInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 inLen, RESULT_DATA_T *outData, uint16 outDataMax);
OOP_DAR_E class25_invoke_set(OAD_INFO_T *pOdaInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 inLen, RESULT_DATA_T *outData, uint16 outDataMax);
OOP_DAR_E class25_invoke_act(OAD_INFO_T *pOdaInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint32 inLen, RESULT_DATA_T *outData, uint16 outDataMax);

OOP_DAR_E class26_invoke_get(OAD_INFO_T *pOdaInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 inLen, RESULT_DATA_T *outData, uint16 outDataMax);
OOP_DAR_E class26_invoke_set(OAD_INFO_T *pOdaInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 inLen, RESULT_DATA_T *outData, uint16 outDataMax);
OOP_DAR_E class26_invoke_act(OAD_INFO_T *pOdaInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint32 inLen, RESULT_DATA_T *outData, uint16 outDataMax);

/**
*********************************************************************
* @brief：     从698格式报文中获取数据(通用基本数据)
* @param[in]： ppOadInfo     - 数据表指针
               pOffset       - 数据指针偏移
               srcBuf        - 源数据
               srcLen        - 源数据长度
               dstLen        - 目的数据长度
* @param[out]：dstData       - 目的数据
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
int32 buf_to_data_common(OAD_INFO_T *pOadInfo, uint32 *oadInfoOffset, void *srcBuf, uint32 srcLen, uint32 *offset, void *dstData, uint32 dstLen);

/**
*********************************************************************
* @brief：     销毁MS
* @param[in]： pMSHead     ms集合链表头
* @return：    ms集合链表头
*********************************************************************
*/
MS_T* ms_destroy(MS_T* pMSHead);

/**
*********************************************************************
* @brief：     获取RSD
* @param[in]： inData      原始报文数据
* @param[in]： InDataLen   报文长度
* @param[out]：pRSD        筛选方法
* @return：    offset      解完后偏移
*********************************************************************
*/
uint16 rsd_get(DB_CLIENT hUdp, const uint8* inData, int InDataLen, RSD_T* pRSD);

/**
*********************************************************************
* @brief：     获取ROAD下的子OAD
* @param[in]： inData      原始报文数据
* @param[in]： InDataLen   报文长度
* @param[out]：offset      解完后偏移
* @return：    ROAD        pROAD
*********************************************************************
*/
RSD_T* rsd_destroy(RSD_T* pRSD);

/**
*********************************************************************
* @brief：     获取ROAD下的子OAD
* @param[in]： inData      原始报文数据
* @param[in]： InDataLen   报文长度
* @param[out]：offset      解完后偏移
* @return：    ROAD        pROAD
*********************************************************************
*/
CSD_T* rcsd_destroy(CSD_T* pCSDHead);

/**
*********************************************************************
* @brief：     获取RCSD
* @param[in]： inData      原始报文数据
* @param[in]： InDataLen   报文长度
* @param[out]：offset      解完后偏移
* @return：    CSD_T       RCSD
*********************************************************************
*/
CSD_T* rcsd_get(uint8* inData, uint16 inDataLen, uint16* offset, BOOL isWithType);

/**
*********************************************************************
* @brief：     应用分帧情况下判断是否继续抄读该测量点
* @param[in]： bpInfo      断点信息
* @param[in]： metsn       测量点号
* @param[out]：startTime   起始时间
               startTime   结束时间
               oopRecNumPn 起始记录序号
* @return：    0           继续抄读
               1           抄读下一个电表信息
*********************************************************************
*/
uint8 slicing_is_comtinue_ms(BP_INFO_T* bpInfo,uint16 metsn, READ_RECORD_T *inReadRecord, uint32 *oopRecNumPn, uint32 *index);

/**
*********************************************************************
* @brief：     应用分帧情况下判断是否继续抄读
* @param[in]： bpInfo      断点信息
* @param[in]： metsn       测量点号
* @param[out]：
* @return：    0           继续抄读
               1           抄读下一个
*********************************************************************
*/
uint8 slicing_is_comtinue_acq_monitor(BP_INFO_T* bpInfo, uint16 i);

/**
*********************************************************************
* @brief：     APDU检查
* @param[in]： SevID       服务类型
* @param[in]： nChoice     
               inBuff      输入报文
               inLen       输入长度
* @param[out]：
* @return：    -1:数据帧不完整,-2:服务不支持 ,>0解析的帧长度
*********************************************************************
*/
int serial_chkapdu(uint8 sevID, uint8 nChoice, const uint8 *inBuff, uint32 inLen);

OOP_DAR_E normal_data_to_fmt698(const IC_OAD_INFO_T *pIcInfo, DATA_CONVERT_T *convert);
OOP_DAR_E basic_data_to_fmt698(DATA_E eData, DATA_CONVERT_T *convert);
OOP_DAR_E basic_data_from_fmt698(DATA_E eData, DATA_CONVERT_T *convert);
OOP_DAR_E basic_db_to_fmt698(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert);

OOP_DAR_E basic_data_to_buf(DATA_E srcType, void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen);
OOP_DAR_E basic_buf_to_data(DATA_E dstType, const void *srcBuf, uint32 srcLen, uint32 *pOffset, void *dstData, uint32 dstLen);

int32 data_to_buf_evt_oad(OOP_OAD_U oad, void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen);
OOP_DAR_E data_to_buf_class8_encryption_res_all(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert);
OOP_DAR_E data_to_buf_cfg_810C(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert);
OOP_DAR_E data_to_buf_chip_node(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert);
OOP_DAR_E data_to_buf_encryption_plan(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert);
OOP_DAR_E data_to_buf_evt_report(const OAD_INFO_T *pOdaInfo, void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen);
OOP_DAR_E data_to_buf_evt_source(const OAD_INFO_T *pOdaInfo, void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen);
OOP_DAR_E data_to_buf_evt_ethport(const OAD_INFO_T *pOdaInfo, void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen);
OOP_DAR_E data_to_buf_evt_ethconnect(const OAD_INFO_T *pOdaInfo, void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen);
OOP_DAR_E data_to_buf_evt_lteport(const OAD_INFO_T *pOdaInfo, void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen);
OOP_DAR_E data_to_buf_evt_lteconnect(const OAD_INFO_T *pOdaInfo, void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen);
OOP_DAR_E data_to_buf_evt_chgpath(const OAD_INFO_T *pOdaInfo, void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen);
OOP_DAR_E data_to_buf_evt_clrttgpower(const OAD_INFO_T *pOdaInfo, void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen);
OOP_DAR_E data_to_buf_evt_proglist(const OAD_INFO_T *pOdaInfo, void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen);
OOP_DAR_E data_to_buf_class8_time(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert);
OOP_DAR_E data_to_buf_evt_std(const OAD_INFO_T *pOadInfo, void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen);
OOP_DAR_E data_to_buf_modulelog(const OAD_INFO_T *pOadInfo, void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen);
OOP_DAR_E data_to_buf_powerlog(const OAD_INFO_T *pOadInfo, void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen);
OOP_DAR_E data_to_buf_smlog(const OAD_INFO_T *pOadInfo, void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen);
OOP_DAR_E data_to_buf_clockflylog(const OAD_INFO_T *pOadInfo, void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen);
OOP_DAR_E data_to_buf_meterdirtylog(const OAD_INFO_T *pOadInfo, void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen);
OOP_DAR_E data_to_buf_mevent(const OAD_INFO_T *pOadInfo, void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen);
OOP_DAR_E data_to_buf_search_met_res(const OAD_INFO_T *pOadInfo, void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen);
OOP_DAR_E data_to_buf_evt_prog(const OAD_INFO_T *pOadInfo, void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen);
OOP_DAR_E data_to_buf_securitylog(const OAD_INFO_T *pOadInfo, void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen);
OOP_DAR_E data_to_buf_security_session(const OAD_INFO_T *pOadInfo, void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen);
OOP_DAR_E data_to_buf_security_stream(const OAD_INFO_T *pOadInfo, void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen);
OOP_DAR_E data_to_buf_security_prog(const OAD_INFO_T *pOadInfo, void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen);
OOP_DAR_E data_to_buf_security_login(const OAD_INFO_T *pOadInfo, void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen);
OOP_DAR_E data_to_buf_rctrllog(const OAD_INFO_T *pOadInfo, void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen);
OOP_DAR_E data_to_buf_pctrllog(const OAD_INFO_T *pOadInfo, void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen);
OOP_DAR_E data_to_buf_ectrllog(const OAD_INFO_T *pOadInfo, void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen);
OOP_DAR_E data_to_buf_ewarnlog(const OAD_INFO_T *pOadInfo, void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen);
OOP_DAR_E data_to_buf_ct_abnormal(const OAD_INFO_T *pOadInfo, void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen);
OOP_DAR_E data_to_buf_p2pclock(const OAD_INFO_T *pOadInfo, void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen);
OOP_DAR_E data_to_buf_refusenode(const OAD_INFO_T *pOadInfo, void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen);
OOP_DAR_E data_to_buf_acrossarea(const OAD_INFO_T *pOadInfo, void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen);
OOP_DAR_E data_to_buf_sysmonitor(const OAD_INFO_T *pOadInfo, void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen);
OOP_DAR_E data_to_buf_identify(const OAD_INFO_T *pOadInfo, void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen);
OOP_DAR_E data_to_buf_pf_input_exit(const OAD_INFO_T *pOadInfo, void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen);
OOP_DAR_E data_to_buf_report_context(const OAD_INFO_T* pOadInfo, void* dstBuf, uint32 dstLen, uint32* pOffset, void* srcData, uint32 srcLen);
OOP_DAR_E data_to_buf_evt_coload(const OAD_INFO_T *pOadInfo, void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen);
OOP_DAR_E apdu_datetime_s_fromfmt698(const void *srcBuf, uint32 srcLen, uint32 *pOffset, void *dstData, uint32 dstLen);
OOP_DAR_E apdu_rsd_fromfmt698(const void *srcBuf, uint32 srcLen, uint32 *pOffset, void *dstData, uint32 dstLen);
OOP_DAR_E apdu_prsd_tofmt698(void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen);
OOP_DAR_E apdu_rcsd_fromfmt698(const void *srcBuf, uint32 srcLen, uint32 *pOffset, void *dstData, uint32 dstLen);
OOP_DAR_E apdu_rcsd_tofmt698(void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen);
OOP_DAR_E apdu_rcsd_tofmt698_jiangsu(void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen);
OOP_DAR_E apdu_octetstrvar_fromfmt698(const void *srcBuf, uint32 srcLen, uint32 *pOffset, void *dstData, uint32 dstLen);
OOP_DAR_E apdu_visiblestrvar_fromfmt698(const void *srcBuf, uint32 srcLen, uint32 *pOffset, void *dstData, uint32 dstLen);
OOP_DAR_E apdu_region_fromfmt698(const void *srcBuf, uint32 srcLen, uint32 *pOffset, void *dstData, uint32 dstLen);
OOP_DAR_E buf_to_data_apngroup(void* srcData, uint32 srcLen, uint32* pOffset, OOP_APNGROUP_T* ptApnGroup);

OOP_DAR_E evt_rectable_get(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax);
OOP_DAR_E evt_objattr_get(const OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax);
OOP_DAR_E evt_report_flag_get(const OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax);
OOP_DAR_E evt_valid_flag_get(const OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax);
OOP_DAR_E evt_asobjattr_set(const OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax);
OOP_DAR_E evt_report_flag_set(const OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax);
OOP_DAR_E evt_valid_flag_set(const OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax);
OOP_DAR_E evt_reset(const OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax);
OOP_DAR_E evt_objattr_unit_add(const OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax);
OOP_DAR_E evt_objattr_unit_del(const OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax);

int32 vardata_data_to_buf(OOP_VARIANT_T variant, uint8 *apduValue, uint32 dstLen, uint32 *offset);
int32 oop_rcsd_get(CSD_T *pCSD, OOP_RCSD_T *oopRcsd);
int32 basic_octetstrvar2k_fromfmt698(const void *srcBuf, uint32 srcLen, uint32 *pOffset, void *dstData, uint32 dstLen);
int32 basic_mac_fromfmt698(const void *srcBuf, uint32 srcLen, uint32 *pOffset, void *dstData, uint32 dstLen);
OOP_DAR_E basic_db_from_fmt698(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert);

void datetime_s_data_to_buf(OOP_DATETIME_S_T tmVal, uint32 *offset, uint8 *apduValue);
void inner_data_init(uint8 type, DB_CLIENT fd);
OOP_DAR_E record_response_make(OOP_OAD_U oad, OOP_RCSD_T rcsd, DATA_CONVERT_T *convert);
OOP_DAR_E normal_response_make(OOP_OAD_U oad, DATA_CONVERT_T *convert);
OOP_DAR_E digit_to_fmt698(void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen);
OOP_DAR_E digit_from_fmt698(const void *srcBuf, uint32 srcLen, uint32 *pOffset, void *dstData, uint32 dstLen);
OOP_DAR_E record_response_make(OOP_OAD_U oad, OOP_RCSD_T rcsd, DATA_CONVERT_T *convert);
OOP_DAR_E data_to_buf_cognates_attr(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert);
OOP_DAR_E buf_to_data_cognates_attr(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert);
OOP_DAR_E cognates_objattr_set(const OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax);
OOP_DAR_E cognates_objattr_get(const OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax);
OOP_DAR_E stat_rectable_get(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax);

BOOLEAN apdu_next_position_get(const uint8 *inData, uint32 *offset, uint32 inDataLen, OOP_DATATYPE_E DataType, BOOLEAN bWithType);
uint8 slicing_is_comtinue(BP_INFO_T* bpInfo, uint16 i);
uint8 slicing_is_comtinue_oop_meter(BP_INFO_T* bpInfo, uint16 i);
uint8 slicing_is_comtinue_searchmeter_res_all(BP_INFO_T* bpInfo, uint16 i);
uint8 slicing_is_comtinue_oi_rights(BP_INFO_T* bpInfo, uint16 i);
uint8 slicing_is_comtinue_task(BP_INFO_T* bpInfo, uint16 i);
uint8 slicing_is_comtinue_normal_scheme(BP_INFO_T* bpInfo, uint16 i);
uint8 slicing_is_comtinue_event_scheme(BP_INFO_T* bpInfo, uint16 i);
int32 read_nor_param_get(OOP_PRSD_T *pOopRsd, OOP_RCSD_T *pOopRcsd, APDU_INFO_T *pApduInfo, READ_NOR_BP_T *pInReadNor, RELY_INFO_T *pRelyInfo);
OOP_DAR_E data_to_buf_class1_henergys(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert);
int32 buf_to_data_class8_task_config_uint(OAD_INFO_T *pOadInfo, const void *srcBuf, uint32 srcLen, uint32 *pOffset,int hUdp);
MS_T* mtr_list_get(DB_CLIENT hUdp, const uint8 *inData, uint16* offset, BOOL isWithType);
int32 data_to_buf_task_config_uint(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert);
OOP_DAR_E buf_to_data_del_one_meter(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert);
OOP_DAR_E buf_to_data_del_one_meter_by_basic(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert);
OOP_DAR_E buf_to_data_del_one_meter_by_act133(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert);
int32 evt_all_rcsd_get(OAD_INFO_T *pOadInfo, uint16 *pOffset, uint8 *pOutData, uint16 outDataMax);
int32 frozen_all_rcsd_get(OAD_INFO_T *pOadInfo, APDU_INFO_T *apduInfo, uint16 *pOffset, uint8 *pOutData, uint16 outDataMax);
int32 all_rcsd_get(OAD_INFO_T *pOadInfo, APDU_INFO_T *apduInfo, uint16 *pOffset, uint8 *pOutData, uint16 outDataMax);
OOP_DAR_E class_array_tofmt698(OAD_INFO_T *pOadInfo, ARRAY_INFO_T *pArrayInfo, DATA_CONVERT_T *convert);

int frozen_tg_objattr_add(DB_CLIENT fd, OOP_OAD_U tgOAD);
int frozen_pulse_objattr_add(DB_CLIENT fd, OOP_OAD_U pulseOAD);
int frozen_ac_objattr_add(DB_CLIENT fd);
int frozen_tg_objattr_del(DB_CLIENT fd, OOP_OAD_U tgOAD);
int frozen_pulse_objattr_del(DB_CLIENT fd, OOP_OAD_U pulseOAD);
int frozen_ac_objattr_del(DB_CLIENT fd);
int32 data_to_buf_evt_asobjattr(const OAD_INFO_T* pOadInfo, void* dstBuf, uint32 dstLen, uint32* pOffset, void* srcData, uint32 srcLen);
OOP_DAR_E data_to_buf_transparent_scheme(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert);
OOP_DAR_E buf_to_data_class8_transparent_scheme(OAD_INFO_T *pOadInfo, void *srcBuf, uint32 srcLen, uint32 *pOffset, int hUdp);
OOP_DAR_E class11_invoke_act_transparent_scheme(OAD_INFO_T *pOdaInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 inLen, RESULT_DATA_T *outData, uint16 outDataMax);
extern  BOOL transparent_plan_sub_oad_get( ROAD_T* pROAD,uint8 planid);
CSD_T* transparent_rcsd_get(uint8 planid);
int32 read_record_transparent_param_get(RSD_T *pRSD, CSD_T *pCSD, APDU_INFO_T *pApduInfo, READ_RECORD_T *pInReadRecord, RELY_INFO_T *pRelyInfo, uint32 *pReadRecNum);
int32 buf_to_data_class8_update_event_scheme(OAD_INFO_T *pOadInfo,  const void *srcBuf, uint32 srcLen, uint32 *pOffset, int hUdp);
int32 buf_to_data_class8_update_task_config_uint(OAD_INFO_T *pOadInfo, const void *srcBuf, uint32 srcLen, uint32 *pOffset,int hUdp);
OOP_DAR_E buf_to_data_report_context(const OAD_INFO_T* pOadInfo, const void* srcBuf, uint32 srcLen, uint32* pOffset, void* dstData, uint32 dstLen);
OOP_DAR_E apdu_ms_fromfmt698(const void *srcBuf, uint32 srcLen, uint32 *pOffset, void *dstData, uint32 dstLen);
OOP_DAR_E buf_to_data_protocol_list(void *srcData, uint32 srcLen, uint32 *pOffset, void *dstBuf, uint32 *dstLen);
OOP_DAR_E transparent_result_rec_get(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax);
OOP_DAR_E buf_to_data_class8_Add_Meter_Frame(OAD_INFO_T *pOadInfo, void *srcBuf, uint32 srcLen, uint32 *pOffset, int hUdp);
OOP_DAR_E class11_invoke_get_transparent_scheme_nor(OAD_INFO_T *pOdaInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 inLen, RESULT_DATA_T *outData, uint16 outDataMax);
OOP_DAR_E class11_invoke_get_acq_state(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 inLen, RESULT_DATA_T *outData, uint16 outDataMax);
OOP_DAR_E class11_invoke_get_acq_state_normal(OAD_INFO_T *pOdaInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 inLen, RESULT_DATA_T *outData, uint16 outDataMax);
OOP_DAR_E class11_invoke_get_acq_state_record(OAD_INFO_T *pOdaInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax);
OOP_DAR_E buf_to_data_class11_act(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert);
OOP_DAR_E data_to_buf_acq_state(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert);
uint16 cal_excludeLen(APP_CONTEXT_t connectionInfo);
BOOL check_is_all_ch(uint8 *buf ,uint8 ch,uint16 len);
OOP_DAR_E data_to_buf_report_plan(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert);
uint8 slicing_is_comtinue_report_scheme(BP_INFO_T* bpInfo, uint16 i);
OOP_DAR_E buf_to_data_class18_F000_get_update(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint32 inLen, RESULT_DATA_T *outData, uint16 outDataMax);
uint8 set_biglen_offset(uint32 eleLen, uint8 *indata);
uint8 exec_system_cmd(const char* cmd);
OOP_DAR_E oop_null_data_res(const OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax);
OOP_DAR_E oop_null_array_res(const OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax);
OOP_DAR_E data_to_buf_evt_basic(const OAD_INFO_T *pOadInfo, void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen);
OOP_DAR_E apdu_variant_tofmt698(void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen);
void app_setdefault_cfg_area(int sockdbfd);
OOP_DAR_E data_to_buf_acrossarea_met_res(const OAD_INFO_T *pOadInfo, void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen);
OOP_DAR_E apdu_prsd_tofmt698(void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen);
OOP_DAR_E apdu_pcsd_tofmt698(void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen);
int32 evt_default_rcsd_get(OOP_OAD_U *oadEvent, OOP_RCSD_T *pRcsd);

int extreme_tg_objattr_add(DB_CLIENT fd, OOP_OAD_U tgOAD);
int extreme_tg_objattr_del(DB_CLIENT fd, OOP_OAD_U tgOAD);
int extreme_pulse_objattr_add(DB_CLIENT fd, OOP_OAD_U pulseOAD);
int extreme_pulse_objattr_del(DB_CLIENT fd, OOP_OAD_U pulseOAD);

void PRTL_FMT_TIME(char* str, uint32 time);

uint8 GetMtrTypeRegion(const uint8 *inData, uint16* offset, uint8* MtrType);
int32 apdu_tsa_fromfmt698(const void *srcBuf, uint32 srcLen, uint32 *pOffset, void *dstData, uint32 dstLen);
int32 apdu_tsa_tofmt698(void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen);
int32 basic_tsa_fromfmt698(const void *srcBuf, uint32 srcLen, uint32 *pOffset, void *dstData, uint32 dstLen);
int32 basic_tsa_tofmt698(void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen);
void MergeMetSnRegion(uint16* MetSn, uint16* MetNum, uint16 StartSn, uint16 EndSn);
int32 general_param_get(OOP_PRSD_T *pOopRsd, OOP_RCSD_T *pOopRcsd, APDU_INFO_T *pApduInfo, READ_METER_T *pInReadMeter, RELY_INFO_T *pRelyInfo);
OOP_DAR_E class18_F001_read_file(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint32 inLen, RESULT_DATA_T *outData, uint16 outDataMax );
int class19_report_open(DB_CLIENT client);
void updatePrtlInitF001(void);

#ifdef __cplusplus
}
#endif

#endif /*__COMM_CLASS_H__*/

