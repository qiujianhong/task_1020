/*
*********************************************************************
* Copyright(C) 2019 China Electric Power Research Institute
* All rights reserved. 
* @brief：   DB操作基础数据结构
* @date：    2019/12/30
* @history： 
*********************************************************************
*/
#ifndef __DB_STRUCT_H_
#define __DB_STRUCT_H_

#ifdef __cplusplus
    extern "C" {
#endif

#define COMPENSATION_TIME       (8*60*60)    //补偿8小时
#define DAY_TIME               (24*60*60)    //一天的时间

#define TRANSPARENTTIME         (6*60*60)    //透明时间单步读时间

#define DATA_LEN_MAX             8192  //单步读数据的最大长度
#define RECORD_LEN_MAX           4096  //读记录数据时回复最大记录数
#define RECORD_HEAD_LEN_MAX      128   //读记录数据时回复最大记录头长度

/*时标选择*/
typedef enum _CLASS_DATA_E
{
    CLASS_DATA_UNINIT = 1,      //不可初始化参数
    CLASS_DATA_INIT   = 2,      //可初始化参数
    CLASS_DATA_NORMAL = 3,      //普通数据
    CLASS_DATA_EVENT  = 4,      //事件数据
    CLASS_DATA_COLLEC = 5,      //采集数据
}DATA_CLASS_E;

/*普通数据OAD刷选条件*/
typedef struct _NOMAL_OAD_T
{
    uint8         logicId; //逻辑地址
    uint16        infoNum; //信息点  当信息点号为0xFFFF时，表示全部信息点
    OOP_OAD_U     oad;     //普通OAD
    uint16        classtag;//数据类标识, 读数据没有用到  DATA_CLASS_E
}NOMAL_OAD_T;

/* 多OAD支持最大数量 */
#define NORMAL_OADS_MAX   16
/*普通数据多个OAD刷选条件*/
typedef struct _NOMAL_OADS_T
{
    uint8         logicId; //逻辑地址
    uint16        infoNum; //信息点 
    uint8         num;
    OOP_OAD_U     oad[NORMAL_OADS_MAX]; //普通OAD集合
}NOMAL_OADS_T;

/*普通数据*/
typedef struct _NOMAL_OAD_DATA_T
{
    OOP_OAD_U     oad;     //数据项标识OAD
    uint16        classtag;//数据类标识, DATA_CLASS_E
    uint16        datalen;
    uint8         data[DATA_LEN_MAX];
}NOMAL_OAD_DATE_T;
#define JS_OOP_MAX_OADS 45

/** @brief 一组对象属性描述符(参数)  */
typedef struct Object_Attribute_Descriptor_Group_MAX
{
    uint32      nNum;                   /**< 关联对象属性个数           */
    uint32      rsv;
    OOP_OAD_U   oad[JS_OOP_MAX_OADS];      /**< 关联对象属性列表           */
}OOP_OADS_MAX_T;

/** @brief 记录型对象属性描述符(参数)   */
typedef struct Record_Object_Attribute_Descriptor_MAX
{
    OOP_OAD_U       oadMain;            /**< 主OOP_OAD_U           */
    OOP_OADS_MAX_T      oadCols;            /**< 关联对象属性         */
}OOP_ROAD_MAX_T;


/*记录数据OAD刷选条件*/
typedef struct _RECORD_T
{
    uint8         optional;//主OAD选项 1-有效,0-无效 接口可以通过主OAD非0,强制有效 */
    uint8         logicId; //逻辑地址
    uint16        infoNum; //信息点  当信息点号为0xFFFF时，表示全部信息点
    OOP_ROAD_MAX_T    road;    //ROAD
    uint16        classtag;//数据类标识, 读数据没有用到  DATA_CLASS_E
}RECORD_OAD_T;

/*时标选择*/
typedef enum _CHOOSE_TYPE_E
{
    COL_TM_START = 0,      //采集起始时间
    COL_TM_END,            //采集结束时间
    COL_TM_STORE,          //采集存储时间
    COL_NO_STORE,          //数据存储序号
    COL_NO_INFO,           //信息点号
    COL_NOT      = 255,    //不筛选
}CHOOSE_TYPE_E;

/*排序方式*/
typedef enum _SORT_TYPE_E
{
    UP_SORT = 0,               //升序
    DOWN_SORT,                 //降序
    NO_SORT = 255,             //默认不排序
}SORT_TYPE_E;

/*读记录型数据刷选条件*/
typedef struct _READ_RECORD_T
{
    RECORD_OAD_T     recordOAD;   //记录型OAD
    CHOOSE_TYPE_E    cType;       //时标选择
    uint32           cStart;      //时间起始点
    uint32           cEnd;        //时间结束点
    SORT_TYPE_E      sortType;    //排序方式
    OOP_OCTETVAR64_T MAC;         //附加校验
    OOP_TI_T         ti;          //时间间隔

    uint32           bTable;      //是否有表地址
    OOP_OCTETVAR64_T table;       //表地址
}READ_RECORD_T;

/* 返回的698数据格式是否包含下面几项*/
typedef struct _RCSD_CHOOSE_T
{
    uint8           isStartTm;   //698格式中是否需要起始时间，     1 ：需要 0 不需要
    uint8           isEndTm;     //698格式中是否需要结束时间，     1 ：需要 0 不需要
    uint8           isStoreTm;   //698格式中是否需要存储时间，     1 ：需要 0 不需要
    uint8           isTsa;       //698格式中是否需要采集通信地址， 1 ：需要 0 不需要
}RCSD_CHOOSE_T;

/*单步读记录型数据*/
typedef struct _STEP_READ_RECORD_T
{
    //uint32      handle;       //句柄
    OOP_OCTETVAR64_T MAC;   //附加校验  nNUM为0表示没有附加校验
    //uint32          len;      //数据缓冲长度
}STEP_READ_RECORD_T;

/*单步读记录型数据返回的数据单元*/
typedef struct _RECORD_DATA_T
{
    uint32         optional;
    OOP_ROAD_T     road;        //OAD
    uint32         outDatalen;  //数据长度
    uint8          outDataBuf[DATA_LEN_MAX];  //数据内容 
}RECORD_DATA_T;

#define RECORD_DATA_MAX_NUM   64

/*单步读记录型数据返回的数据单元*/
typedef struct _RECORD_UNIT_T
{
    uint32         colStartTm;  //采集起始时间
    uint32         colEndTm;    //采集结束时间
    uint32         colStoreTm;  //采集存储时间
    uint16         infoNum;     //信息点号
    uint16         recordnum;   //数据数量
    RECORD_DATA_T  record[RECORD_DATA_MAX_NUM];//数据
}RECORD_UNIT_T;

/*写记录型数据*/
typedef struct _WRITE_RECORD_T
{
    RECORD_OAD_T     recordOAD; 
    uint32           colStartTm;  //采集起始时间
    uint32           colEndTm;    //采集结束时间
    uint32           colStoreTm;  //采集存储时间
    OOP_OCTETVAR64_T MAC;         //附加校验
    uint32           inDatalen;   //数据长度
    uint8            inDataBuf[DATA_LEN_MAX];  //数据内容 
}WRITE_RECORD_T;


typedef struct _WRITE_RECORD_DATA_T
{
    uint32           len;                 //数据长度
    uint8            data[DATA_LEN_MAX];  //数据内容 
}WRITE_RECORD_DATA_T;

/*写记录型数据*/
typedef struct _WRITE_RECORDS_T
{
    RECORD_OAD_T        recordOAD; 
    uint32              colStartTm;  //采集起始时间
    uint32              colEndTm;    //采集结束时间
    uint32              colStoreTm;  //采集存储时间
    OOP_OCTETVAR64_T    MAC;         //附加校验
    WRITE_RECORD_DATA_T datas[JS_OOP_MAX_OADS]; //与次OAD顺序保持一致
}WRITE_RECORDS_T;


/*更新数据时筛选方式*/
typedef struct
{
    uint8            choice;      //类型标识
    union
    {
        uint32       storeTm;     //采集存储时标
        uint32       storeNo;     //存储序号
    };
}FILTER_E;

/*更新记录型数据*/
typedef struct _UPDATE_RECORD_T
{
    RECORD_OAD_T     recordOAD; 
    FILTER_E         filter;      //刷选方式
    OOP_OCTETVAR64_T MAC;         //附加校验
    uint32           inDatalen;   //数据长度
    uint8            inDataBuf[DATA_LEN_MAX];  //数据内容 
}UPDATE_RECORD_T;

typedef enum _GET_TYPE_E
{
    GET_ALL_RECORD = 0,      //所有记录
    GET_INTER_RECORD,        //存储时间区间
}GET_TYPE_E;

/*获取记录条数*/
typedef struct _GET_RECORD_NUM_T
{
    RECORD_OAD_T    recordOAD; 
    GET_TYPE_E      filter;   //刷选方式
    uint32          gStart;   //开始区间
    uint32          gEnd;     //结束区间
}GET_RECORD_NUM_T;

/*获取记录条数*/
typedef struct _CLEAR_DB_DATA_T
{
    RECORD_OAD_T    recordOAD; 
    uint8           bClassTag;   //数据项标识 TRUE表示没有数据项标识 兼容以前的
    uint8           bFilter;
    uint8           filterType;  //筛选方式，0- 存储时标 255 不筛选
    uint8           res[1];
    uint32          startFilter;
    uint32          endFilter;
}CLEAR_DATA_T;

#ifdef __cplusplus
}
#endif

#endif

