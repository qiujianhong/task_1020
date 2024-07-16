/*
***************************************************************************
  Copyright (c) 2006-2007,�Ͼ����������������޹�˾
             All rights reserved.

  ��������: mem_internal.h
  ��    �ܣ��洢��ϵͳ�ڲ������ÿ�
  �� �� �ˣ�zhangyu
  ����ʱ�䣺2006-11

  �汾��ʷ:

  2006-11-06 :
    v1.0, zhangyu, �½�

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

#define _DLL_IMPLICIT_   //���һ�򿪵��Կ��ض�Ҫ����,��mem.h����������
//#define _DEBUG_        //���һЩ����̨�ַ�����Ϣ, ͨ��gcc -D_DEBUG_����
//#define QUICK_LOCATE   //�Ƿ����������(������ҵ������)���ٶ�λ����, ��������û��Լ���ӣ�
                         //����ʱͨ��gcc -DQUICK_LOCATE���ݡ�

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
#include "eeprom.h" /* ARMƽ̨��eeprom�����ӿ�, PCƽ̨����ʱ����Ҫ�õ� */
#endif

/************************************************************************************
                                    ���ݽṹ
 ************************************************************************************/

/*
���ļ��������Ͷ��壨���䣩:
      ��mem.h�е�ҵ���������ͼ��������Ͷ������Ӧ�����������ļ�ͷ�б�ʶ�ļ����������
  ���ͣ�����Ϊ�ⲿ�����������Ǵ洢ϵͳ�ڲ�ʹ�ã��ʷ��ڴ˴�������

  ʹ�ö���: "xlFileHeader: fh_typeID"

 */
#define FILE_PRIVATE  0x0100    /* ˽��������   */
#define FILE_RAMXDATA 0x0101    /* XRam��ʱ���� */

/* Units of time */
#define HoursPerDay   24
#define MinsPerHour   60
#define SecsPerMin    60
#define MSecsPerSec   1000
#define MinsPerDay    HoursPerDay * MinsPerHour
#define SecsPerDay    MinsPerDay * SecsPerMin
#define MSecsPerDay   SecsPerDay * MSecsPerSec
#define EVENT_MAX_NUM 15

//�����������ݲ����޸�

#define MTR485_MAX_NUM                64                            //485������ĸ���(ʼ��Ϊ64)
#define NO_VIR_MTR_MAX_NUM      3500                        //�������ܼ���������������



//#define MTR_MAX_NUM                    (NO_VIR_MTR_MAX_NUM    + 16)    //�����ܼ���������������

//ϵͳ485����������ý��ɼ���������
#define MTRPLS_MAX_NUM              8                                        //������������
#define AC_MTR_NO                      (NO_VIR_MTR_MAX_NUM + 1)                // ���ɲ������
#define PULSE_MTR_NO_BEGIN              (AC_MTR_NO+1)                              // �����������ʼ��

#define VIR_BEGIN                   (PULSE_MTR_NO_BEGIN + 8)    //�ܼ�����ʼ���, ֻ��04��Լ��Ҫ��65��ʼ����ͨ������1~64
#define    REPEATERS_MAX_NUM              64
#define    JOIN_MAX_NUM                    3                              //�������������������������
#define    MTRVIP_MAX_NUM                20                            //�ص㻧����
#define DataDir_ROUTE               16                        //ϵͳ�ֵ䶨��֧�ֵ�����ܼ�����
#define CtrlGrp_ROUTE                    8                              //�ܼ�����


#define MTR_MAX_NUM                    (VIR_BEGIN + CtrlGrp_ROUTE) //���в�������������

#define MEMFILE_MTR_NUM                (MTRPLS_MAX_NUM + CtrlGrp_ROUTE + 1) // �ա��¡����߶�����ļ���ͷ�в����������
                                                                         // �������ɡ����塢�ܼ���

#define    WIDE_COUNT                    16            //���������


/*
 *�������ļ�ͷ��ʶ�ṹ��
 */
typedef struct _xlFileHeader {
    char    fh_signature[2];    //�ļ���־, ʼ��Ϊ ��X��, ��L��
    uint8   fh_version;         //�ļ�ͷ�ṹ�汾��*10,  10 -> v1.0
    uint16  fh_len;             //���ṹ�峤��
    uint16  fh_projID;          //������ţ��ͻ��ţ�
    mtrnum_t fh_mtrID;           //�������ţ���TMN_IDָ�նˣ�
    uint16  fh_typeID;          //�������ͱ�ʶ
    uint32  extra0, extra1;     //�Զ��帽������
    uint32  TotalLen;                   // ���в����㳤���ܺ� ��С16��
    uint32  OffBase[MEMFILE_MTR_NUM];       // ���ļ������в������ƫ����Ϣ ��С16������1��ʾƫ����16
} __attribute__ ((packed)) xlFileHeader;


typedef struct _xlFileHeader_min {
    char    fh_signature[2];    //�ļ���־, ʼ��Ϊ ��X��, ��L��
    uint8   fh_version;         //�ļ�ͷ�ṹ�汾��*10,  10 -> v1.0
    uint16  fh_len;             //���ṹ�峤��
    uint16  fh_projID;          //������ţ��ͻ��ţ�
    mtrnum_t fh_mtrID;           //�������ţ���TMN_IDָ�նˣ�
    uint16  fh_typeID;          //�������ͱ�ʶ
    uint32  extra0, extra1;     //�Զ��帽������
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
    //����2
    uint32 RecNo;                //�¼������
    uint16 offset;              //�¼���¼����ʼƫ��
    uint16 len;                 //�¼���¼�ĳ���
    char   beginTime[7];            //�¼��ķ�������ʼʱ��
    char   endTime[7];            //�¼�����ʱ��
    uint8  rptState[20];
    //����7
    uint8  evtSrc[17];           //�¼�����Դ
    uint32 ocurNum;              //��������
    uint32 ocurTime;             //�ۼƷ���ʱ��    
}EventInfo;

#if STD_698PRO_BETA == 1
typedef struct _xlFileHeader_OopEvt {
    char      fh_signature[2];    //�ļ���־, ʼ��Ϊ ��X��, ��L��
    uint8     fh_maxNum;          //�¼�����¼����
    uint8     fh_curNum;          //�¼���ǰ��¼����    
    uint8     oadbuff[80];        //oadbuff ����3
    uint32    fh_curRecNo;        //���һ���¼���¼�����
    
    EventInfo fh_EventInfo[EVENT_MAX_NUM];   //��ֻ��15��
} __attribute__ ((packed)) xlFileHeader_OopEvt;
#endif

/* �����������Ϣͷ�ṹ(�ն�+���������) */
typedef struct _XParaInfoHeader {
  uint16  RunParaOff, FuncParaOff, MtrParaOffBase;//�������ļ��е�ƫ����
  uint16  RunParaSize, FuncParaSize, MtrParaSize; //���ֲ������͵��ֽڳ���    
  mtrnum_t  MtrNum; //��������
  uint8   Reserved; //����
} __attribute__ ((packed)) XParaInfoHeader;

#ifdef EXTEND_PARA
typedef struct _ExtInfoHeader {
  uint16  usTotalSize;//total size of param
  uint8   Reserved; //����
} __attribute__ ((packed)) ExtInfoHeader;
#endif

typedef struct _XRtMtrInfo
{
    //uint16      RtDataOffBase[MTR_MAX_NUM];   // ���ٺϲ�Ϊһ��������
    uint32      DayPointOffBase[MTR_MAX_NUM];
    uint32      DayCurveOffBase[MTR_MAX_NUM];
    uint32      DayCalcOffBase[MTR_MAX_NUM];
    uint32      DayRecOffBase[MTR_MAX_NUM];
    uint32      MonPointOffBase[MTR_MAX_NUM];
    uint32      MonCalcOffBase[MTR_MAX_NUM];
    uint32      MonRecOffBase[MTR_MAX_NUM];
    uint32      MonCurveOffBase[MTR_MAX_NUM];    
    uint32      DayMinCurveOffBase[MTR_MAX_NUM];

    //uint16      RtDataTotal;                  // ���ٺϲ�Ϊһ��������
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

/*--���������Խṹ��--*/
typedef struct _MtrProperty
{
  BOOL        enabled;    //������ʹ��
  mtrnum_t    mtrNum;     //�������
  uint8       category;   //���������: MTR_485, MTR_SIM...
  uint8       tacId;      //�洢���Ժ�
} MtrProperty;

/* �����������Ϣ�ṹ��.(�ڴ�) */
typedef struct _XParaInfo {
  int   ParaTotalSize;  //�����������ֽ���
  int   XParaFileSize;  //�����ļ��ֽ���
  mtrnum_t  MtrNum; //����������
  uint16    RunParaSize, FuncParaSize, MtrParaSize; //���ֲ���ռ�õ��ֽ���  
  int       fdXPara; //�����ļ�������
  
  unsigned char *ParaInfoVect;    // ����mmap��ʼ��ַ
  unsigned char *TmnRunParaOff;   // �ն����в����ڴ��ַ
  unsigned char *TmnFuncParaOff;  // �ն˹��ܲ����ڴ��ַ
  unsigned char *MtrParaOffBase;  // �����������ʼ��ַ
  MtrProperty *MtrPorpsVect; /* �����������Ϣ */

  uint16        bWriteFile;     // ��Ϊ�ļ��Ĳ������ѻ��⣬�����ø�״̬����ά��
                                // ���״̬����Ҫ�����ļ�ͷ�ĺϲ���������أ����Բ��������Ƶ����
                                // ���Դ��в���Ҳ����̫Ӱ��Ч��
  XRtMtrInfo*   MtrInfoVect;    // ָ���ļ��и����������ƫ����
} XParaInfo;

/* eeprom˽�������ݱ�ʶ�ṹ */
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

/* eeprom˽��������ע����Ϣ�ļ��ײ� */
typedef struct _EpdataFileHeader {
  uint32 freeoffset;        /* ����eeprom��ַƫ����*/
  //uint32 freeOffCopy;     /* ����eeprom��ַƫ���������ݣ�*/
} __attribute__ ((packed)) EpdataFileHeader;

/* eeprom˽��������ע����  */
typedef struct _EpdataItemInfo {
  uint16 ValidId; /* ��Ч�Ա�־��0xabcd. ����ValidId=0xabcdʱ����Ϊ����������Ч */
  uint16 len;     /* ע����ֽڳ��� */
  uint32 offbase; /* ע�����ʼ��ַ */
} __attribute__ ((packed)) EpdataItemInfo;


/************************************************************************************
                                  �ڲ�ȫ�ֱ���
 ************************************************************************************/

/***NandFlash���ݴ��������·��***/
extern const char *g_pcszRootPath;

/***SD�����ݴ��������·��***/
extern const char *g_pcszMovePath;

/***RamDisk���ص�***/
extern const char *g_pcszRamDiskPath;

/***************************************************************
                          �洢����
 ***************************************************************/
/*-- �洢���Ժ� --*/
#define   TAC_PULSE_ID    0
#define   TAC_485AC_ID    1
#define   TAC_VIR_ID      2
#define   TAC_PLC_ID      3
#define   TAC_PLCMUI_ID   4//�ز��๦��

#define   TAC_TMN_ID      0   //�ն˴洢���Ժ�

/*--������洢���Է�����--*/
#define JMAP_PROJ_NUM     5
/*--�ն˴洢ӳ�䷽����--*/
#define TMAP_PROJ_NUM     1

#ifndef QUICK_LOCATE
  /*-- ��ͨ�Ķ�λ��ʽ --*/
  #define PRJTYPE uint16
#else
  /*-- ���ұ�λ��ʽ --*/
  #define PRJTYPE uint32

  /*- �������������Ų���������Ԫƫ���� -*/
  extern const uint32 MtrParaOffVect[JLD_PARA_NUM];
  extern const uint32 TmnRunParaOffVect[TMN_RUN_PARA_NUM];
  extern const uint32 TmnFuncParaOffVect[TMN_FUNC_PARA_NUM];
#ifdef EXTEND_PARA
  extern const uint32 TmnExtParaOffVect[TMN_EXT_PARA_NUM];
  extern const uint32 MtrExtParaOffVect[MTR_EXT_PARA_NUM];
  extern const uint32 TmnExtParaAreaOffVect[TMN_EXT_PARA_AREA_NUM];         //�����������ն���������
  extern const uint32 MtrExtParaAreaOffVect[MTR_EXT_PARA_AREA_NUM];         //���������Ĳ�������������
#endif
#if STD_698PRO_BETA== 1
  extern const uint32 Task698CfgParaOffVect[TASK_698_PARA_NUM];
  extern const uint32 NorAcqScheParaOffVect[NOR_ACQ_PARA_NUM];  
  extern const uint32 MtrEvtParaOffVect[MTR_EVT_PARA_NUM];
  extern const uint32 MtrReportParaOffVect[MTR_REPORT_PARA_NUM];
#endif

#endif

/*--������洢ӳ��ṹ--*/
typedef struct _JMapProj
{
    const PRJTYPE *current;     /*- ��ǰʱ�̲ɼ������� -*/
    const PRJTYPE *point;       /*- �ն��������� -*/
    const PRJTYPE *curve;       /*- ������������������ -*/
    const PRJTYPE *mincurve;    /*- ������������������� -*/
    const PRJTYPE *calc;        /*- �ռ��������� -*/
    const PRJTYPE *rec;         /*- �ռ�¼������-*/
    const PRJTYPE *point_mon;   /*- �¶��������� -*/
    const PRJTYPE *calc_mon;    /*- �¼��������� -*/
    const PRJTYPE *rec_mon;     /*- �¼�¼������ -*/
}JMapProj;

/*- �ն˴洢ӳ��ṹ -*/
typedef struct _TMapProj
{
    const PRJTYPE *current;     /*- ��ǰʱ�̲ɼ������� -*/
    const PRJTYPE *curve;       /*- ������������ -*/
    const PRJTYPE *mincurve;    /*- �������������� -*/
    const PRJTYPE *calc;        /*- �ռ��������� -*/
    const PRJTYPE *rec;         /*- �ռ�¼������ -*/
    const PRJTYPE *calc_mon;    /*- �¼��������� -*/
    const PRJTYPE *rec_mon;     /*- �¼�¼������ -*/
    //const uint16  *curve_mon;   /*- $�����������飬��ʱû�� -*/
}TMapProj;


/*--������洢��������x3--*/
extern const JMapProj JMapTac[JMAP_PROJ_NUM];
/*--�ն˴洢��������x1--*/
extern const TMapProj TMapTac[TMAP_PROJ_NUM];

//"������"
//--"485/����"
extern const PRJTYPE  mtr_current_proj[];     /*- 485/���ɱ�ǰ�ɼ����� -*/
extern const PRJTYPE  mtr_curve_proj[];       //-���ɱ�485��������-
extern const PRJTYPE  mtr_mincurve_proj[];       //-���ɱ�485������������-
extern const PRJTYPE  mtr_calc_proj[];        /*- 485/���ɱ��ռ�������- */
extern const PRJTYPE  mtr_point_proj[];       //-���ɱ�/485�ն�������-
extern const PRJTYPE  mtr_rec_proj[];         /*-���ɱ�/485�������ռ�¼������-*/
extern const PRJTYPE  mtr_point_mon_proj[];   /*-���ɱ�/485�¶���������-*/
extern const PRJTYPE  mtr_calc_mon_proj[];    /*-���ɱ�/485�¼���������-*/
extern const PRJTYPE  mtr_rec_mon_proj[];     /*-���ɱ�/485�������¼�¼������-*/

//--"����"
extern const PRJTYPE pls_current_proj[];      /*-���������㵱ǰ�ɼ�����-*/
extern const PRJTYPE pls_curve_proj[];        /*-������������-*/
extern const PRJTYPE pls_calc_proj[];         /*-������ռ�������- */
extern const PRJTYPE pls_point_proj[];        /*-�����ն�������-*/
extern const PRJTYPE pls_point_mon_proj[];    /*-�����¶���������-*/
extern const PRJTYPE pls_calc_mon_proj[];     /*-�����¼���������-*/

//--���������
extern const PRJTYPE vir_current_proj[];      /*-��������㵱ǰ�ɼ�����-*/
extern const PRJTYPE vir_point_proj[];        /*-����������ն�������-*/
extern const PRJTYPE vir_curve_proj[];        /*-�������������������-*/
extern const PRJTYPE vir_calc_proj[];         /*-����������ռ�������-*/
extern const PRJTYPE vir_point_mon_proj[];    /*-����������¶���������-*/
extern const PRJTYPE vir_calc_mon_proj[];     /*-����������¼���������-*/

//--�ز�������
extern const PRJTYPE plc_current_proj[];      /*-�ز������㵱ǰ�ɼ�����-*/
extern const PRJTYPE plc_point_proj[];        /*-�ز��������ն�������-*/
extern const PRJTYPE plc_curve_proj[];        /*-�ز�����������������-*/
extern const PRJTYPE plc_calc_proj[];         /*-�ز��������ռ�������-*/
extern const PRJTYPE plc_point_mon_proj[];    /*-�ز��������¶���������-*/
extern const PRJTYPE plc_calc_mon_proj[];     /*-�ز��������¼���������-*/

//�๦���ز���
extern const PRJTYPE plcmui_current_proj[];   //�๦���ز���ǰʱ��������
extern const PRJTYPE plcmui_point_proj[];     //�๦���ز��ն���
extern const PRJTYPE plcmui_curve_proj[];     //�๦���ز�������
extern const PRJTYPE plcmui_calc_proj[];      //�๦���ز��ռ���
extern const PRJTYPE plcmui_point_mon_proj[]; //�๦���ز��¶���
extern const PRJTYPE plcmui_calc_mon_proj[];  //�๦���ز��¼���

//"�ն�"
extern const PRJTYPE tmn_current_proj[];      /*- �ն˵�ǰ�ɼ����� -*/
extern const PRJTYPE tmn_curve_proj[];          /*- �ն�����������-*/
extern const PRJTYPE tmn_calc_proj[];         /*- �ն��ռ������� -*/
extern const PRJTYPE tmn_calc_mon_proj[];     /*- �ն��¼������� -*/
extern const PRJTYPE tmn_rec_proj[];          /*- �ն��ռ�¼���� -*/
extern const PRJTYPE tmn_rec_mon_proj[];      /*- �ն��¼�¼���� -*/

//extern const PRJTYPE tmn_curve_proj[];      /*- �ն�����������, ��ʱû�� -*/
//extern const PRJTYPE tmn_rec_mon_proj[];    /*- �ն�����������, ��ʱû�� -*/

extern const PRJTYPE ramxdata_proj[];  /*- ramdisk��ʱ�������� -*/

//#define LOG_MAX_SIZE    512000            /*��־��С �ֽ���    512K*/

#define     MaxSizeOfSingleFile         1024*1024*2         // ����OAD�����ļ���󳤶�2M

/************************************************************************************
                                    �ڲ���������
 ************************************************************************************/

#define XPARAM_MAP_ID   0

/* �����ڴ���Դ�����߳�ʼ�����ݽ�����־�ź�:
 *   �������߳�ʼ�������ڴ��е�����֮ǰ��Ӧsemphore��ʼֵΪ0, ��ʼ��֮
 * ���Ӧsemphore��ֵ�޸�Ϊ1; ����Ӧ���������ø�IPC��Դʱ,��Ҫ�ȴ���Ӧ
 * �Ĺ����ڴ���Դ��ʼ������.
 */
#define MEM_INIT_ID   1

/*
 * EEPROM�����ڴ�ӳ�����
 */
extern int  g_MemInfoSemId;

extern int  g_EepromShmId;
extern char *g_pEepromMap;
/*
 * �ź�������
 */
void init_sem(int id, int semId);
void wait_sem(int id, int semId);
void release_sem(int id, int semId);

/*
 * EEPROM��д�ӿ�
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

/* ��¼��  */
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

/* ����������Ĭ��ֵд�뺯��������InitDataArea(void)������ */
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
    uint8    type;        //ID���
    uint8    Fn1;        /*-��Ӧ��FN��-*/
    uint8    Fn2;        /*-���ܲ�ֹһ��FN��-*/
    uint8    PlcDefault; //F10��С���û������
    uint16    bDataId;    /*-Դ�ֵ�ID���ֵ��ʽ��-*/
    /*F38��F39����,Ĭ�����*///�����Ϊ0��ʱ��ģ�С���Ĭ�ϵ������� 0x0111--��ʾС���0��1��2Ĭ�ϰ�����Ӧ��������
    uint64    Full_Class[WIDE_COUNT];        //��ʾ��Ӧ��������������������������û�д򿪣���ʹ��С��������˳���������Ҳ���ܳ�����
    uint64    Def_Class[WIDE_COUNT];        //��ʾ��Ӧ���Ĭ�ϳ�����������������˴�С��ŵ�û������F38��39��ʱ��ȡ�ò���
};

#else
struct Id_To_Fn
{    
    uint8    type;        //ID���
    uint16    bDataId;    /*-Դ�ֵ�ID���ֵ��ʽ��-*/    
    uint8    Fn1;        /*-��Ӧ��FN��-*/
    uint8    Fn2;        /*-���ܲ�ֹһ��FN��-*/
    BOOL    PlcDefault; //F10��С���û������
    uint64    Def_Class[WIDE_COUNT];    /*F38��F39����,Ĭ�����*///�����Ϊ0��ʱ��ģ�С���Ĭ�ϵ������� 0x0111--��ʾС���0��1��2Ĭ�ϰ�����Ӧ��������
};
#endif
#endif

