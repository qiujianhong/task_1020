/*
***************************************************************************
  Copyright (c) 2006-2007,南京新联电子仪器有限公司
             All rights reserved.

  程序名称: mem_internal.h
  功    能：存储子系统内部主调用库
  开 发 人：zhangyu
  开发时间：2006-11

  版本历史:

  2006-11-06 :
    v1.0, zhangyu, 新建

***************************************************************************
*/

#ifndef _MEM_INTERNAL_H
#define _MEM_INTERNAL_H

#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/file.h>
#include <sys/mman.h>
#include <time.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <limits.h>
#include <time.h>
#include <string.h>
#include <dirent.h>
#include <stdarg.h>
#include <assert.h>
#include <libgen.h>

#define _DLL_IMPLICIT_   //这个一打开调试开关定要加上,给mem.h作条件编译
//#define _DEBUG_        //输出一些控制台字符串信息, 通过gcc -D_DEBUG_传递
//#define QUICK_LOCATE   //是否采用数据项(参数和业务数据)快速定位方案, 这个不用用户自己添加，
                         //编译时通过gcc -DQUICK_LOCATE传递。

#ifdef _DEBUG_
  #include <errno.h>
  extern int errno;

#define DPRINT(fmt...) \
    ({ if(getenv("DEBUG_MEM") != NULL)  printf("Mem: "fmt);})    
#else
  #define DPRINT(x...)
#endif

#define DPRINT_MEM(fmt...) \
    ({ if(getenv("DEBUG_MEM_NEW") != NULL)  printf("Mem_NEW: "fmt);})    


#include "../../include/dataType.h"
#include "mm_config.h"
#include "fileOp.h"

#ifdef ARM
#include "eeprom.h" /* ARM平台的eeprom驱动接口, PC平台编译时不需要用到 */
#endif

/************************************************************************************
                                    数据结构
 ************************************************************************************/

/*
　文件数据类型定义（补充）:
      和mem.h中的业务数据类型及参数类型定义相对应．都用来在文件头中标识文件保存的数据
  类型，但因为这部分数据类型是存储系统内部使用，故放在此处声明．

  使用对象: "xlFileHeader: fh_typeID"

 */
#define FILE_PRIVATE  0x0100    /* 私有类数据   */
#define FILE_RAMXDATA 0x0101    /* XRam临时数据 */

/* Units of time */
#define HoursPerDay   24
#define MinsPerHour   60
#define SecsPerMin    60
#define MSecsPerSec   1000
#define MinsPerDay    HoursPerDay * MinsPerHour
#define SecsPerDay    MinsPerDay * SecsPerMin
#define MSecsPerDay   SecsPerDay * MSecsPerSec
#define EVENT_MAX_NUM 15

//以下配置内容不用修改

#define MTR485_MAX_NUM                64                            //485测量点的个数(始终为64)
#define NO_VIR_MTR_MAX_NUM      3500                        //不包含总加组的最大测量点个数



//#define MTR_MAX_NUM                    (NO_VIR_MTR_MAX_NUM    + 16)    //包含总加组的最大测量点个数

//系统485计量点和内置交采计量点数量
#define MTRPLS_MAX_NUM              8                                        //脉冲测量点个数
#define AC_MTR_NO                      (NO_VIR_MTR_MAX_NUM + 1)                // 交采测量点号
#define PULSE_MTR_NO_BEGIN              (AC_MTR_NO+1)                              // 脉冲测量点起始号

#define VIR_BEGIN                   (PULSE_MTR_NO_BEGIN + 8)    //总加组起始编号, 只有04规约需要从65开始，普通测量点1~64
#define    REPEATERS_MAX_NUM              64
#define    JOIN_MAX_NUM                    3                              //级联最大数量，集抄部分增加
#define    MTRVIP_MAX_NUM                20                            //重点户个数
#define DataDir_ROUTE               16                        //系统字典定义支持的最大总加组数
#define CtrlGrp_ROUTE                    8                              //总加组数


#define MTR_MAX_NUM                    (VIR_BEGIN + CtrlGrp_ROUTE) //所有测量点的最大数量

#define MEMFILE_MTR_NUM                (MTRPLS_MAX_NUM + CtrlGrp_ROUTE + 1) // 日、月、曲线冻结的文件中头中测量点的数量
                                                                         // 包括交采、脉冲、总加组

#define    WIDE_COUNT                    16            //大类号数量


/*
 *　数据文件头标识结构体
 */
typedef struct _xlFileHeader {
    char    fh_signature[2];    //文件标志, 始终为 ‘X’, ‘L’
    uint8   fh_version;         //文件头结构版本号*10,  10 -> v1.0
    uint16  fh_len;             //本结构体长度
    uint16  fh_projID;          //方案编号（客户号）
    mtrnum_t fh_mtrID;           //测量点编号（宏TMN_ID指终端）
    uint16  fh_typeID;          //数据类型标识
    uint32  extra0, extra1;     //自定义附加数据
    uint32  TotalLen;                   // 所有测量点长度总和 缩小16倍
    uint32  OffBase[MEMFILE_MTR_NUM];       // 该文件中所有测量点的偏移信息 缩小16倍，即1表示偏移量16
} __attribute__ ((packed)) xlFileHeader;


typedef struct _xlFileHeader_min {
    char    fh_signature[2];    //文件标志, 始终为 ‘X’, ‘L’
    uint8   fh_version;         //文件头结构版本号*10,  10 -> v1.0
    uint16  fh_len;             //本结构体长度
    uint16  fh_projID;          //方案编号（客户号）
    mtrnum_t fh_mtrID;           //测量点编号（宏TMN_ID指终端）
    uint16  fh_typeID;          //数据类型标识
    uint32  extra0, extra1;     //自定义附加数据
} __attribute__ ((packed)) xlFileHeader_min;

typedef struct _SmallDate {
   uint16   year;   // 1999, 2000, 2001, 2003..
   uint8    month;  // 1 ~ 12
   uint8    day;    // 1 ~ 31
} SmallDate;

typedef struct _BinLogHead
{
    uint32 time; 
    uint16 databuf_len;   
    uint16 allsize;
}BinLogHead;

typedef struct{
    //属性2
    uint32 RecNo;                //事件的序号
    uint16 offset;              //事件记录的起始偏移
    uint16 len;                 //事件记录的长度
    char   beginTime[7];            //事件的发生的起始时间
    char   endTime[7];            //事件结束时间
    uint8  rptState[20];
    //属性7
    uint8  evtSrc[17];           //事件发生源
    uint32 ocurNum;              //发生次数
    uint32 ocurTime;             //累计发生时间    
}EventInfo;

#if STD_698PRO_BETA == 1
typedef struct _xlFileHeader_OopEvt {
    char      fh_signature[2];    //文件标志, 始终为 ‘X’, ‘L’
    uint8     fh_maxNum;          //事件最大记录条数
    uint8     fh_curNum;          //事件当前记录条数    
    uint8     oadbuff[80];        //oadbuff 属性3
    uint32    fh_curRecNo;        //最近一次事件记录的序号
    
    EventInfo fh_EventInfo[EVENT_MAX_NUM];   //先只存15条
} __attribute__ ((packed)) xlFileHeader_OopEvt;
#endif

/* 交互类参数信息头结构(终端+测量点参数) */
typedef struct _XParaInfoHeader {
  uint16  RunParaOff, FuncParaOff, MtrParaOffBase;//参数在文件中的偏移量
  uint16  RunParaSize, FuncParaSize, MtrParaSize; //各种参数类型的字节长度    
  mtrnum_t  MtrNum; //测量点数
  uint8   Reserved; //保留
} __attribute__ ((packed)) XParaInfoHeader;

#ifdef EXTEND_PARA
typedef struct _ExtInfoHeader {
  uint16  usTotalSize;//total size of param
  uint8   Reserved; //保留
} __attribute__ ((packed)) ExtInfoHeader;
#endif

typedef struct _XRtMtrInfo
{
    //uint16      RtDataOffBase[MTR_MAX_NUM];   // 不再合并为一个测量点
    uint32      DayPointOffBase[MTR_MAX_NUM];
    uint32      DayCurveOffBase[MTR_MAX_NUM];
    uint32      DayCalcOffBase[MTR_MAX_NUM];
    uint32      DayRecOffBase[MTR_MAX_NUM];
    uint32      MonPointOffBase[MTR_MAX_NUM];
    uint32      MonCalcOffBase[MTR_MAX_NUM];
    uint32      MonRecOffBase[MTR_MAX_NUM];
    uint32      MonCurveOffBase[MTR_MAX_NUM];    
    uint32      DayMinCurveOffBase[MTR_MAX_NUM];

    //uint16      RtDataTotal;                  // 不再合并为一个测量点
    uint32      DayPointTotal;
    uint32      DayCurveTotal;
    uint32      DayCalcTotal;
    uint32      DayRecTotal;
    uint32      MonPointTotal;
    uint32      MonCalcTotal;
    uint32      MonRecTotal;
    uint32      MonCurveTotal;
    uint32         DayMinCurveTotal;

    uint8       MtrType[MTR_MAX_NUM];
    
}__attribute__ ((packed)) XRtMtrInfo;

/*--测量点属性结构体--*/
typedef struct _MtrProperty
{
  BOOL        enabled;    //测量点使能
  mtrnum_t    mtrNum;     //测量点号
  uint8       category;   //测量点类别: MTR_485, MTR_SIM...
  uint8       tacId;      //存储策略号
} MtrProperty;

/* 交互类参数信息结构体.(内存) */
typedef struct _XParaInfo {
  int   ParaTotalSize;  //参数内容总字节数
  int   XParaFileSize;  //参数文件字节数
  mtrnum_t  MtrNum; //测量点总数
  uint16    RunParaSize, FuncParaSize, MtrParaSize; //各种参数占用的字节数  
  int       fdXPara; //参数文件描述符
  
  unsigned char *ParaInfoVect;    // 参数mmap起始地址
  unsigned char *TmnRunParaOff;   // 终端运行参数内存地址
  unsigned char *TmnFuncParaOff;  // 终端功能参数内存地址
  unsigned char *MtrParaOffBase;  // 测量点参数起始地址
  MtrProperty *MtrPorpsVect; /* 测量点基本信息 */

  uint16        bWriteFile;     // 因为文件的操作很难互斥，所以用该状态量来维持
                                // 这个状态量主要互斥文件头的合并测量点相关，所以操作不会很频繁，
                                // 所以串行操作也不会太影响效率
  XRtMtrInfo*   MtrInfoVect;    // 指向文件中各个测量点的偏移量
} XParaInfo;

/* eeprom私有类数据标识结构 */
typedef struct _pdatachk_t {
  uint8  id;
  uint16 size;
}__attribute__ ((packed)) pdatachk_t;

#define PDATACHK_SIZE   3

/* arg for semctl system calls. */
union semun {
  int val;      /* value for SETVAL */
  struct semid_ds *buf;   /* buffer for IPC_STAT & IPC_SET */
  unsigned short *array;    /* array for GETALL & SETALL */
  struct seminfo *__buf;    /* buffer for IPC_INFO */
  void *__pad;
};

/* eeprom私有类数据注册信息文件首部 */
typedef struct _EpdataFileHeader {
  uint32 freeoffset;        /* 空余eeprom地址偏移量*/
  //uint32 freeOffCopy;     /* 空余eeprom地址偏移量（备份）*/
} __attribute__ ((packed)) EpdataFileHeader;

/* eeprom私有类数据注册项  */
typedef struct _EpdataItemInfo {
  uint16 ValidId; /* 有效性标志，0xabcd. 即当ValidId=0xabcd时，认为该数据项有效 */
  uint16 len;     /* 注册的字节长度 */
  uint32 offbase; /* 注册的起始地址 */
} __attribute__ ((packed)) EpdataItemInfo;


/************************************************************************************
                                  内部全局变量
 ************************************************************************************/

/***NandFlash数据存放区绝对路径***/
extern const char *g_pcszRootPath;

/***SD卡数据存放区绝对路径***/
extern const char *g_pcszMovePath;

/***RamDisk挂载点***/
extern const char *g_pcszRamDiskPath;

/***************************************************************
                          存储策略
 ***************************************************************/
/*-- 存储策略号 --*/
#define   TAC_PULSE_ID    0
#define   TAC_485AC_ID    1
#define   TAC_VIR_ID      2
#define   TAC_PLC_ID      3
#define   TAC_PLCMUI_ID   4//载波多功能

#define   TAC_TMN_ID      0   //终端存储策略号

/*--计量点存储策略方案数--*/
#define JMAP_PROJ_NUM     5
/*--终端存储映射方案数--*/
#define TMAP_PROJ_NUM     1

#ifndef QUICK_LOCATE
  /*-- 普通的定位方式 --*/
  #define PRJTYPE uint16
#else
  /*-- 查找表定位方式 --*/
  #define PRJTYPE uint32

  /*- 下面三个数组存放参数项数组元偏移量 -*/
  extern const uint32 MtrParaOffVect[JLD_PARA_NUM];
  extern const uint32 TmnRunParaOffVect[TMN_RUN_PARA_NUM];
  extern const uint32 TmnFuncParaOffVect[TMN_FUNC_PARA_NUM];
#ifdef EXTEND_PARA
  extern const uint32 TmnExtParaOffVect[TMN_EXT_PARA_NUM];
  extern const uint32 MtrExtParaOffVect[MTR_EXT_PARA_NUM];
  extern const uint32 TmnExtParaAreaOffVect[TMN_EXT_PARA_AREA_NUM];         //各个地区的终端增补参数
  extern const uint32 MtrExtParaAreaOffVect[MTR_EXT_PARA_AREA_NUM];         //各个地区的测量点增补参数
#endif
#if STD_698PRO_BETA== 1
  extern const uint32 Task698CfgParaOffVect[TASK_698_PARA_NUM];
  extern const uint32 NorAcqScheParaOffVect[NOR_ACQ_PARA_NUM];  
  extern const uint32 MtrEvtParaOffVect[MTR_EVT_PARA_NUM];
  extern const uint32 MtrReportParaOffVect[MTR_REPORT_PARA_NUM];
#endif

#endif

/*--测量点存储映射结构--*/
typedef struct _JMapProj
{
    const PRJTYPE *current;     /*- 当前时刻采集数据组 -*/
    const PRJTYPE *point;       /*- 日定点数据组 -*/
    const PRJTYPE *curve;       /*- 计量点日曲线数据组 -*/
    const PRJTYPE *mincurve;    /*- 计量点分钟曲线数据组 -*/
    const PRJTYPE *calc;        /*- 日计算数据组 -*/
    const PRJTYPE *rec;         /*- 日记录数据组-*/
    const PRJTYPE *point_mon;   /*- 月定点数据组 -*/
    const PRJTYPE *calc_mon;    /*- 月计算数据组 -*/
    const PRJTYPE *rec_mon;     /*- 月记录数据组 -*/
}JMapProj;

/*- 终端存储映射结构 -*/
typedef struct _TMapProj
{
    const PRJTYPE *current;     /*- 当前时刻采集数据组 -*/
    const PRJTYPE *curve;       /*- 日曲线数据组 -*/
    const PRJTYPE *mincurve;    /*- 分钟曲线数据组 -*/
    const PRJTYPE *calc;        /*- 日计算数据组 -*/
    const PRJTYPE *rec;         /*- 日记录数据组 -*/
    const PRJTYPE *calc_mon;    /*- 月计算数据组 -*/
    const PRJTYPE *rec_mon;     /*- 月记录数据组 -*/
    //const uint16  *curve_mon;   /*- $月曲线数据组，暂时没有 -*/
}TMapProj;


/*--计量点存储策略数组x3--*/
extern const JMapProj JMapTac[JMAP_PROJ_NUM];
/*--终端存储策略数组x1--*/
extern const TMapProj TMapTac[TMAP_PROJ_NUM];

//"测量点"
//--"485/交采"
extern const PRJTYPE  mtr_current_proj[];     /*- 485/交采表当前采集数据 -*/
extern const PRJTYPE  mtr_curve_proj[];       //-交采表485曲线数据-
extern const PRJTYPE  mtr_mincurve_proj[];       //-交采表485分钟曲线数据-
extern const PRJTYPE  mtr_calc_proj[];        /*- 485/交采表日计算数据- */
extern const PRJTYPE  mtr_point_proj[];       //-交采表/485日定点数据-
extern const PRJTYPE  mtr_rec_proj[];         /*-交采表/485计量点日记录数据项-*/
extern const PRJTYPE  mtr_point_mon_proj[];   /*-交采表/485月定点数据项-*/
extern const PRJTYPE  mtr_calc_mon_proj[];    /*-交采表/485月计算数据项-*/
extern const PRJTYPE  mtr_rec_mon_proj[];     /*-交采表/485计量点月记录数据项-*/

//--"脉冲"
extern const PRJTYPE pls_current_proj[];      /*-脉冲表计量点当前采集数据-*/
extern const PRJTYPE pls_curve_proj[];        /*-脉冲曲线数据-*/
extern const PRJTYPE pls_calc_proj[];         /*-脉冲表日计算数据- */
extern const PRJTYPE pls_point_proj[];        /*-脉冲日定点数据-*/
extern const PRJTYPE pls_point_mon_proj[];    /*-脉冲月定点数据项-*/
extern const PRJTYPE pls_calc_mon_proj[];     /*-脉冲月计算数据项-*/

//--虚拟测量点
extern const PRJTYPE vir_current_proj[];      /*-虚拟测量点当前采集数据-*/
extern const PRJTYPE vir_point_proj[];        /*-虚拟测量点日定点数据-*/
extern const PRJTYPE vir_curve_proj[];        /*-虚拟测量点日曲线数据-*/
extern const PRJTYPE vir_calc_proj[];         /*-虚拟测量点日计算数据-*/
extern const PRJTYPE vir_point_mon_proj[];    /*-虚拟测量点月定点数据项-*/
extern const PRJTYPE vir_calc_mon_proj[];     /*-虚拟测量点月计算数据项-*/

//--载波测量点
extern const PRJTYPE plc_current_proj[];      /*-载波测量点当前采集数据-*/
extern const PRJTYPE plc_point_proj[];        /*-载波测量点日定点数据-*/
extern const PRJTYPE plc_curve_proj[];        /*-载波测量点日曲线数据-*/
extern const PRJTYPE plc_calc_proj[];         /*-载波测量点日计算数据-*/
extern const PRJTYPE plc_point_mon_proj[];    /*-载波测量点月定点数据项-*/
extern const PRJTYPE plc_calc_mon_proj[];     /*-载波测量点月计算数据项-*/

//多功能载波表
extern const PRJTYPE plcmui_current_proj[];   //多功能载波当前时刻数据项
extern const PRJTYPE plcmui_point_proj[];     //多功能载波日定点
extern const PRJTYPE plcmui_curve_proj[];     //多功能载波日曲线
extern const PRJTYPE plcmui_calc_proj[];      //多功能载波日计算
extern const PRJTYPE plcmui_point_mon_proj[]; //多功能载波月定点
extern const PRJTYPE plcmui_calc_mon_proj[];  //多功能载波月计算

//"终端"
extern const PRJTYPE tmn_current_proj[];      /*- 终端当前采集数据 -*/
extern const PRJTYPE tmn_curve_proj[];          /*- 终端日曲线数据-*/
extern const PRJTYPE tmn_calc_proj[];         /*- 终端日计算数据 -*/
extern const PRJTYPE tmn_calc_mon_proj[];     /*- 终端月计算数据 -*/
extern const PRJTYPE tmn_rec_proj[];          /*- 终端日记录数据 -*/
extern const PRJTYPE tmn_rec_mon_proj[];      /*- 终端月记录数据 -*/

//extern const PRJTYPE tmn_curve_proj[];      /*- 终端日曲线数据, 暂时没有 -*/
//extern const PRJTYPE tmn_rec_mon_proj[];    /*- 终端月曲线数据, 暂时没有 -*/

extern const PRJTYPE ramxdata_proj[];  /*- ramdisk临时交互数据 -*/

//#define LOG_MAX_SIZE    512000            /*日志大小 字节数    512K*/

#define     MaxSizeOfSingleFile         1024*1024*2         // 单个OAD数据文件最大长度2M

/************************************************************************************
                                    内部公共函数
 ************************************************************************************/

#define XPARAM_MAP_ID   0

/* 共享内存资源创建者初始化数据结束标志信号:
 *   当创建者初始化共享内存中的内容之前对应semphore初始值为0, 初始化之
 * 后对应semphore的值修改为1; 当对应调用者引用该IPC资源时,需要等待对应
 * 的共享内存资源初始化结束.
 */
#define MEM_INIT_ID   1

/*
 * EEPROM共享内存映射变量
 */
extern int  g_MemInfoSemId;

extern int  g_EepromShmId;
extern char *g_pEepromMap;
/*
 * 信号量操作
 */
void init_sem(int id, int semId);
void wait_sem(int id, int semId);
void release_sem(int id, int semId);

/*
 * EEPROM读写接口
 */
int ReadEEPROM(uint32 offset, uint32 len, char *buf, BOOL bRaw);
int WriteEEPROM(uint32 offset, uint32 len, const char *buf);

/*
 * misc
 */
uint32 getSDFreeBytes(void);
int32 getDirUsedBytes(const char *path);

BOOL isRamDiskValid(void);
BOOL isSDCardValid(void);

int parse_dirInfo(int kind, const char *fileName, SmallDate *sdRet);

/* 记录锁  */
int RcdLock(int fd, int cmd, int type, off_t offset, int whence, off_t len);

#define read_lock(fd, offset, len) \
            RcdLock(fd, F_SETLK, F_RDLCK, offset, SEEK_SET, len)

#define readw_lock(fd, offset, len) \
            RcdLock(fd, F_SETLKW, F_RDLCK, offset, SEEK_SET, len)

#define write_lock(fd, offset, len) \
            RcdLock(fd, F_SETLK, F_WRLCK, offset, SEEK_SET, len)

#define writew_lock(fd, offset, len) \
            RcdLock(fd, F_SETLKW, F_WRLCK, offset, SEEK_SET, len)

#define un_lock(fd, offset, len) \
            RcdLock(fd, F_SETLK, F_UNLCK, offset, SEEK_SET, len)

/* 数据区数据默认值写入函数，用在InitDataArea(void)函数中 */
int writedefdata(mtrnum_t msn); 

void Today(SmallDate *today);
void IncDay(SmallDate *src, SmallDate *ndate);
BOOL ChangeNameToDate(char* pName, DateFileStruct *dateFile);
void SortNameDateList(DateFileStruct* pDateList, int nCount);
int ClearMetSnDataSinglePlan(mtrnum_t metSn, const PlanInfo_t planInfo, const uint32 meterSnDataStartOffset, const uint32 sumMetSnDataLen);

int TarLogFile(const char *filepath, int backup_count);

#if JIANGSU_JZQ_FUN == 1

struct Id_To_Fn
{
    uint8    type;        //ID类别
    uint8    Fn1;        /*-对应的FN项-*/
    uint8    Fn2;        /*-可能不止一个FN项-*/
    uint8    PlcDefault; //F10大小类号没有配置
    uint16    bDataId;    /*-源字典ID（字典格式）-*/
    /*F38、F39不配,默认情况*///大类号为0的时候的，小类号默认的数据项 0x0111--表示小类号0、1、2默认包含对应的数据项
    uint64    Full_Class[WIDE_COUNT];        //表示对应类号最多允许抄读的数据项（如果该项没有打开，即使大小类号配置了抄读，程序也不能抄读）
    uint64    Def_Class[WIDE_COUNT];        //表示对应类号默认抄读的数据项，在配置了大小类号但没有配置F38、39的时候，取该参数
};

#else
struct Id_To_Fn
{    
    uint8    type;        //ID类别
    uint16    bDataId;    /*-源字典ID（字典格式）-*/    
    uint8    Fn1;        /*-对应的FN项-*/
    uint8    Fn2;        /*-可能不止一个FN项-*/
    BOOL    PlcDefault; //F10大小类号没有配置
    uint64    Def_Class[WIDE_COUNT];    /*F38、F39不配,默认情况*///大类号为0的时候的，小类号默认的数据项 0x0111--表示小类号0、1、2默认包含对应的数据项
};
#endif
#endif

