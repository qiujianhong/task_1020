/*
***************************************************************************
  Copyright (c) 2006-2007,南京新联电子仪器有限公司
             All rights reserved.

  程序名称: utils.c
  功    能：存储子系统内部实用库
  开 发 人：zhangyu
  开发时间：2006-11-06

  版本历史:

  2006-11-06 :
    v1.0, zhangyu, 新建
  2006-12-28 :
    v1.1, zhangyu, 增加sync_idbInfo等目录/文件同步接口的实现

***************************************************************************
*/

#include "mem_internal.h"

#include "framecomm.h"

/***************************************************************
                      miscillence
 ***************************************************************/
 
/*
  名  称: RcdLock
  功  能: 用fcntl方式实现记录锁
  参  数:
    fd   - 锁定文件的文件描述符
    cmd  - 锁定命令(F_SETLK, F_SETLKW, F_GETLK)
    type - 锁类型(F_RDLCK, F_WRLCK, F_UNLCK)
  返回值:
*/
int RcdLock(int fd, int cmd, int type, off_t offset, int whence, off_t len)
{
  struct flock locker;

  locker.l_type   = type;
  locker.l_start  = offset;
  locker.l_whence = SEEK_SET;
  locker.l_len    = len;

  return fcntl(fd, cmd, &locker);
}

/*
  名  称: isRamDiskValid
  功  能: 检查ramdisk是否可用
  输  入:

  输  出:

  返回值:
    TRUE  - ramdisk准备就绪
    FALSE - ramdisk不能使用
 */
BOOL isRamDiskValid(void)
{
  struct stat sta;

  if( stat(g_pcszRamDiskPath, &sta) < 0)
    return FALSE;

  if( !S_ISDIR(sta.st_mode))
    return FALSE;

  return (RAMDISK_DEVID == sta.st_dev);
}

/*
  名  称: isSDCardValid
  功  能: 检查SDCARD是否可用
  输  入:

  输  出:

  返回值:
    TRUE  - sdcard准备就绪
    FALSE - sdcard不能使用
 */
BOOL isSDCardValid(void)
{
  struct stat sta;

  if( stat(g_pcszMovePath, &sta) < 0)
    return FALSE;

  if( !S_ISDIR(sta.st_mode))
    return FALSE;

  if(SDCARD_DEVID == sta.st_dev)
    return TRUE;

  return FALSE;
}


/*
  名  称: getSDFreeBytes
  功  能:
  输  入:
  输  出:
  返回值:
    0xffffffff - 错误
    >=0 - 返回的SD卡容量(字节数, < 2G)
 */
uint32 getSDFreeBytes(void)
{
  uint32 free, total;
  if( GetDiskUsage(g_pcszMovePath, &free, &total))
    return free;

  return 0xffffffff;
}

/*
  名  称: getDirUsedBytes
  功  能:
  输  入:
  输  出:
  返回值:
    < 0 - 错误
    >=0 -
 */
int32 getDirUsedBytes(const char *path)
{
  return GetFilesSizeRecu(path);
}

int BinarySearch(const int A[], int key, int N)
{
  int Low, Mid, High;
  Low = 0;
  High = N - 1;

  while(Low <= High){
    Mid = (Low + High) / 2;
    if(A[Mid] < key)
      Low = Mid + 1;
    else if(A[Mid] > key)
      High = Mid - 1;
    else
    return Mid;
  }

  return -1;
}

/*
  名  称: parse_dirInfo
  功  能: 从数据文本串中找出日期信息
  输  入:
    kind     - 0 : 目录, 1 : 文件
    fileName - 待解析的文件(目录)名称
  输  出:
    sdRet - 返回从fileName中解析出来的信息
  返回值:
      -1 - 失败
      0  - 成功
 */
int parse_dirInfo(int kind, const char *fileName, SmallDate *sdRet)
{
    unsigned year=0, month=0, day=0;

    if( !sdRet || !fileName) return -1;

    if( 0 == kind) 
    { //目录
        if( fileName[0] != 'D')
            return -1;
    }
    else if( 1 == kind)
    { //文件
        if( fileName[1] != 'F')
            return -1;
    }

    if( 0 == kind)
        sscanf(fileName, "D_%u-%u-%u", &year, &month, &day);
    else if( 1 == kind)
        sscanf(fileName, "F_%u-%u-%u", &year, &month, &day);
    else
        return -1;

    if(year < 3000)
        sdRet->year = year;
    else
        return -1;

    if( month <= 12)
        sdRet->month = month;
    else
        return -1;

    sdRet->day = (day < 32) ? day : 0;
    return 0;
}


int TarLogFile(const char *filepath, int backup_count)
{
    #define        BACKUP_COUNT    10                    //允许保存的压缩包数量

    int            i;
    char        tar_path[PATH_MAX_LEN + 1];            //压缩包路径
    char        tar_cmd[PATH_MAX_LEN + 1];            //压缩包命令
    char        rename_cmd1[PATH_MAX_LEN + 1];        //更换前的文件名
    char        rename_cmd2[PATH_MAX_LEN + 1];        //更换后的文件名

    char        bFilePath[PATH_MAX_LEN];            //文件路径（不含文件名）
    char        bFileName[NAME_MAX];                //文件名，不含扩展名
    char        bFileNameWithPostfix[NAME_MAX];    //文件名，含扩展名
    char        bFileExt[40];                        //文件扩展名
    char        bDirPath[PATH_MAX_LEN];            //文件路径（不含文件名）

    if (backup_count > BACKUP_COUNT)
    {
        backup_count = BACKUP_COUNT;
    }

    if (backup_count < 1)
    {
        return 0;
    }

    memset(tar_cmd, 0x0, sizeof(tar_cmd));
    memset(bFilePath, 0x0, sizeof(bFilePath));
    memset(bFileName, 0x0, sizeof(bFileName));
    memset(bFileNameWithPostfix, 0x0, sizeof(bFileNameWithPostfix));
    memset(bFileExt, 0x0, sizeof(bFileExt));
    memset(bDirPath, 0x0, sizeof(bDirPath));

    ExtractFilePath(filepath, bFilePath);                                //文件路径（不含文件名）
    ExtractFileName(filepath, bFileName);                                //文件名，不含扩展名
    ExtractFileNameWithPostfix(filepath, bFileNameWithPostfix);        //文件名，含扩展名
    ExtractFileExt(filepath, bFileExt);                //文件扩展名
    // 建立以文件名的日志目录
    sprintf(bDirPath, "%s%slog", bFilePath, bFileName);
    //printf("bDirPath = %s\n", bDirPath);
    if(!PathExists(bDirPath))
    {
        makePathedDir(bDirPath);
    }
    sprintf(tar_cmd, "mv -f %s %s", filepath, bDirPath);
    //printf("tar_cmd = %s\n", tar_cmd);
    system(tar_cmd);

    memset(tar_cmd, 0x0, sizeof(tar_cmd));
    //printf("bFilePath = %s\n", bFilePath);
    //printf("bFileName = %s\n", bFileName);
    //printf("bFileExt = %s\n", bFileExt);
    //printf("2 bFileNameWithPostfix = %s\n", bFileNameWithPostfix);

    for(i = 0; i < backup_count; i++)        //确认能够压缩的压缩包文件
    {
        memset(tar_path, 0x0, sizeof(tar_path));
        sprintf(tar_path, "%s/%s_%02d.tar.gz", bDirPath, bFileNameWithPostfix, i);
        if( !FileExists(tar_path))        //文件不存在，则可以压缩
        {
            // 不要包含目录
            memset(tar_path, 0x0, sizeof(tar_path));
            sprintf(tar_path, "%s_%02d.tar.gz", bFileNameWithPostfix, i);
            sprintf(tar_cmd, "cd %s; tar -cvzf %s %s", bDirPath, tar_path, bFileNameWithPostfix);
            //printf("tar_cmd = %s\n", tar_cmd);
            system(tar_cmd);
            //printf("bFileNameWithPostfix = %s\n", bFileNameWithPostfix);
            memset(tar_path, 0x0, sizeof(tar_path));
            sprintf(tar_path, "%s/%s", bDirPath, bFileNameWithPostfix);
            remove(tar_path);
            return 1;
        }
    }

    //执行到这里，表示没有空闲的压缩包，所以需要将最旧的删除，并且将其他的挨个改名
    memset(tar_path, 0x0, sizeof(tar_path));
    sprintf(tar_path, "%s/%s_%02d.tar.gz", bDirPath, bFileNameWithPostfix, 0);
    remove(tar_path);        //删除第一个压缩包

    for(i = 1; i < backup_count; i++) 
    {
        memset(rename_cmd1, 0x0, sizeof(rename_cmd1));
        memset(rename_cmd2, 0x0, sizeof(rename_cmd2));
        sprintf(rename_cmd1, "%s/%s_%02d.tar.gz", bDirPath, bFileNameWithPostfix, i);            //改名前
        sprintf(rename_cmd2, "%s/%s_%02d.tar.gz", bDirPath, bFileNameWithPostfix, i - 1);        //改名后
        rename(rename_cmd1, rename_cmd2);
        //printf("rename_cmd1 = %s rename_cmd2 = %s\n", rename_cmd1, rename_cmd2);
    }

    memset(tar_path, 0x0, sizeof(tar_path));
    sprintf(tar_path, "%s_%02d.tar.gz", bFileNameWithPostfix, backup_count - 1);
    sprintf(tar_cmd, "cd %s; tar -cvzf %s %s", bDirPath, tar_path, bFileNameWithPostfix);
    //printf("tar_cmd = %s\n", tar_cmd);
    system(tar_cmd);        //压缩日志文件
    memset(tar_path, 0x0, sizeof(tar_path));
    sprintf(tar_path, "%s/%s", bDirPath, bFileNameWithPostfix);
    remove(tar_path);    
    return 1;
}


/*
 * 初始化单值信号量，启动时守护进程调用
 */
void init_sem(int id, int semId)
{
  union semun s;

  if( semId < 0) return;

  s.val = 1;
  semctl(semId, id, SETVAL, s);
}

/*
 *　single-semphore P()
 */
void wait_sem(int id, int semId)
{
  struct sembuf sops;

  if( semId < 0) return;

  sops.sem_num = id;
  sops.sem_op  = -1;
  sops.sem_flg = SEM_UNDO;

  semop(semId, &sops, 1);
}

/*
 * single-semphore V()
 */
void release_sem(int id, int semId)
{
  struct sembuf sops;

  if( semId < 0) return;

  sops.sem_num = id;
  sops.sem_op  = 1;
  sops.sem_flg = SEM_UNDO;

  semop(semId, &sops, 1);
}

void Today(SmallDate *today)
{
  struct tm *ptm;
  time_t t = time(NULL);
  ptm = localtime(&t);
  
  today->year  = 1900 + ptm->tm_year;
  today->month = 1 + ptm->tm_mon;
  today->day   = ptm->tm_mday;  
}

/* IncDay:
 *   将SmallDate类型的对象src数值增加1天, 并将计算返回值保存在ndate变量对象中.
 */
void IncDay(SmallDate *src, SmallDate *ndate)
{
  struct tm *tmp, tmNow;
  time_t time;

  memset(&tmNow, 0, sizeof(tmNow));
  tmNow.tm_year  = src->year  - 1900;
  tmNow.tm_mon   = src->month - 1;
  tmNow.tm_mday  = src->day;

  time = FastDateToSecond(&tmNow);
  time += 86400; /* 60x60x24 = 86400(秒/天)  */

  tmp = localtime(&time);
  ndate->year  = tmp->tm_year + 1900;
  ndate->month = tmp->tm_mon + 1;
  ndate->day   = tmp->tm_mday;
}


#ifdef EXTEND_DATA11

#define CS_DEFAULT_VAL          0xfffffffeL                             //默认的校验和的值（即无效数据）

#define CS_FILENAME             "hisdata_cs_file"                       //数据校验存放的文件名
#define CS_DATATYPE_NUN         8                                       //需要校验的数据类型数量
#define HISDATA_CS_LEN          (CS_DATATYPE_NUN * sizeof(uint32))      //每种数据类型校验和的总长度（一般是8种类型，每种类型用4个字节）
#define HISDATA_CS_RTU_POS      0                                       //终端数据校验和的起始位置
#define HISDATA_CS_485_POS      (HISDATA_CS_RTU_POS + HISDATA_CS_LEN)   //485
#define HISDATA_CS_PLS_POS      (HISDATA_CS_485_POS + HISDATA_CS_LEN)   //脉冲
#define HISDATA_CS_VIR_POS      (HISDATA_CS_PLS_POS + HISDATA_CS_LEN)   //总加组
#if MEM_CATA_NEW == 1
#define HISDATA_CS_CLASS_POS    (HISDATA_CS_VIR_POS + HISDATA_CS_LEN)   //大小类号校验和的起始位置
#endif

//计算某种数据项类型的校验和

/*********************************************************************
功能描述：计算某种数据项类型的校验和
输入参数：
-           pPrjtype        数据项存储顺序列表，比如mtr_point_proj、tmn_calc_proj等
-           pIDLenBuf       每个ID的长度
-           id_count        该类型的id数量
-           type            数据类型
输出参数：
-           pDataCSCal      计算出来的校验和
函数返回值：
-            CS_DEFAULT_VAL  无效的校验和
-            > 0             计算出来的校验和
内部处理流程：
-            
********************************************************************/
uint32 CalcTypeDataCs(const PRJTYPE *pPrjtype, const uint16 *pIDLenBuf, uint32 id_count)
{
    uint32 id = 0;
    uint32 tmpCs = 0;

    if(id_count > 0 && !pPrjtype && !pIDLenBuf)
    {
        for(id = 0; id < id_count; id++)
        {
            if( pPrjtype[id + 2] != 0xffffffff)       //参与存储的数据项才计算校验和
            {
                tmpCs += (pIDLenBuf[id] * id * id);
            }
        }
        return tmpCs;
    }

    return CS_DEFAULT_VAL;
}

//读取文件中存放的校验和数据
int ReadTypeDataCs(uint32 bCSPos, uint32 *pDataCSRead)
{
    int bRet = 0;
    int i;
    uint32 DataCSRead[CS_DATATYPE_NUN];     //参与校验的数据类型（从文件中读取到）
    char tmpBuf[HISDATA_CS_LEN];

    memset(tmpBuf, 0xFF, HISDATA_CS_LEN);
    bRet = ReadPDataXram(CS_FILENAME, (char *)DataCSRead, bCSPos, HISDATA_CS_LEN);

    if(bRet < 0 || memcmp(tmpBuf, (char *)DataCSRead, HISDATA_CS_LEN) == 0)     //无效数据
    {
        for(i = 0; i < CS_DATATYPE_NUN; i++)
            pDataCSRead[i] = CS_DEFAULT_VAL;
        return -1;
    }

    memcpy((char *)pDataCSRead, (char *)DataCSRead, HISDATA_CS_LEN);
    return 1;
}

//检查终端数据校验和是否不一致
//如果检验和不一致，需要初始化相应数据
int bCheckDataCSRtu()
{
    int i = 0;
    uint32 DataCSCal[CS_DATATYPE_NUN];      //参与校验的数据类型（计算得出）
    uint32 DataCSRead[CS_DATATYPE_NUN];     //参与校验的数据类型（从文件中读取到）
    TMapProj *tproj = (TMapProj *)&TMapTac[0];

    for(i = 0; i < CS_DATATYPE_NUN; i++)
        DataCSCal[i] = CS_DEFAULT_VAL;

    ReadTypeDataCs(HISDATA_CS_RTU_POS, DataCSRead);     //从私有外村中读取之前存放的校验和

    

    DataCSCal[CALC_DATA_DAY - 2] = CalcTypeDataCs(tproj->calc, tmn_calc_len, TMN_CALC_NUM);             //日计算
    DataCSCal[CURVE_DATA_DAY - 2] = CalcTypeDataCs(tproj->curve, tmn_curve_len, TMN_CURVE_NUM);         //日曲线
    DataCSCal[REC_DATA_DAY - 2] = CalcTypeDataCs(tproj->rec, tmn_rec_len, TMN_REC_NUM);                 //日记录
    DataCSCal[CALC_DATA_MON - 2] = CalcTypeDataCs(tproj->calc_mon, tmn_calc_mon_len, TMN_CALC_MON_NUM); //月计算
    DataCSCal[REC_DATA_MON - 2] = CalcTypeDataCs(tproj->rec_mon, tmn_rec_mon_len, TMN_REC_MON_NUM);     //月记录


    for(i = 0; i < CS_DATATYPE_NUN; i++)
    {

        DataCSCal[i] = CS_DEFAULT_VAL;
    }

    return 1;
}

//检查终端、485、脉冲、总加组的数据校验和是否出错
//如果检验和不一致，需要初始化相应数据
int bCheckDataCSMtr()
{
    int i = 0, j = 0;
    JMapProj *pJMapTac[] = {(JMapProj *)&JMapTac[0], (JMapProj *)&JMapTac[1], (JMapProj *)&JMapTac[2]};
    uint32 DataCSCal[3][CS_DATATYPE_NUN];      //参与校验的数据类型（计算得出）
    uint32 DataCSRead[3][CS_DATATYPE_NUN];     //参与校验的数据类型（从文件中读取到）

    for(i = 0; i < 3; i++)
    {
        for(j = 0; j < CS_DATATYPE_NUN; j++)
        {
            DataCSCal[i][j] = CS_DEFAULT_VAL;
        }

        DataCSCal[i][CALC_DATA_DAY - 2] = CalcTypeDataCs(pJMapTac[i]->calc, jl_calc_len, JL_CALC_NUM);                 //日计算
        DataCSCal[i][CALC_DATA_MON - 2] = CalcTypeDataCs(pJMapTac[i]->calc_mon, jl_calc_mon_len, JL_CALC_MON_NUM);     //月计算
        DataCSCal[i][REC_DATA_DAY - 2] = CalcTypeDataCs(pJMapTac[i]->rec, jl_rec_len, JL_REC_NUM);                     //日记录
        DataCSCal[i][REC_DATA_MON - 2] = CalcTypeDataCs(pJMapTac[i]->rec_mon, jl_rec_mon_len, JL_REC_MON_NUM);         //月记录
        DataCSCal[i][POINT_DATA_DAY - 2] = CalcTypeDataCs(pJMapTac[i]->point, jl_point_len, JL_POINT_NUM);             //日定点
        DataCSCal[i][POINT_DATA_MON - 2] = CalcTypeDataCs(pJMapTac[i]->point_mon, jl_point_mon_len, JL_POINT_MON_NUM); //月定点
        DataCSCal[i][CURVE_DATA_DAY - 2] = CalcTypeDataCs(pJMapTac[i]->curve, jl_curve_len, JL_CURVE_NUM,);             //日曲线
        #ifdef MINICURVE
        DataCSCal[i][MINCURVE_DATA - 2] = CalcTypeDataCs(pJMapTac[i]->mincurve, jl_mincurve_len, JL_MINCURVE_NUM);     //分钟曲线
        #endif
    }

    return 1;
}

#endif


