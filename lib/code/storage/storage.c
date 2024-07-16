/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief：提供数据库访问接口
* @date： 2019.10.23
*********************************************************************
*/
#include "dataCommon.h"
#include "mem_internal.h"
#include "framecomm.h"
#include "errno.h"
#include "storage.h"
//#include "msgUdp.h"
#include "oadlist.h"

//
// 内部操作相关宏和数据定义
//
#define IS_LITTLE_ENDIAN          1       //组消息的时候index label msgtag是否用小端模式 0为大端 1为小端

/*动态库初始化完成标志*/
static BOOL m_bInited = TRUE;
static char m_AppName[256] = {0};

/* 参数信息结构对象 */
static XParaInfo g_xparaInfo;

const char *g_pcszRootPath    = "/data/app/";       // "/data/";
const char *g_pcszMovePath    = "/tmp/sdcard/";     // "/mnt/sd/";
const char *g_pcszRamDiskPath = "/ramdisk/";        // "/tmp/ramdisk0/";

/* 函数声明 */
int write_eeprom(uint32 offset, uint32 len, const char *buf);
int read_eeprom(uint32 offset, uint32 len, char *buf, BOOL bRaw);

#define RECV_BUF_LENGTH           (8192)  /*recvfrom接收缓冲区大小*/
#define OCTET_NO_LENGTH           0       /*octet string seize 6 不带长度*/
#define TIME_OUT_RETRY            3
#define RECV_BUF_CLEAR            0       /*清除接收缓存功能*/
#define MEMPRINT_BUF_LOG  1


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
uint8 set_len_offset(uint16 eleLen, uint8 *indata)
{
    if(eleLen < 0x80)
    {
        indata[0] = eleLen;
        return 1;
    }
    else if(eleLen <= 0xffff)
    {
        indata[0] = 0x82;
        indata[1] = (uint8)((eleLen >> 8) & 0xff);
        indata[2] = (uint8)(eleLen & 0xff);
        return 3;
    }
        
    return 0;
}

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
uint16 get_len_offset(uint8 *indata,uint8 *bytes)
{
    uint16 EleNum;
    uint8 nbytes = 1;
    
    EleNum = indata[0];
    if(EleNum==0x81)
    {
        EleNum = indata[1];
        nbytes = 2;
    }
    else if(EleNum==0x82)
    {
        EleNum = indata[1];
        EleNum <<= 8;
        EleNum += indata[2];
        nbytes = 3;
    }
    else if(EleNum==0x80 || EleNum>0x82)
    {
        return 0;
    }
    
    if(bytes != NULL)
        *bytes = nbytes;
        
    return EleNum;
}

//
// 基本接口操作
//

void *memcpy_r(void *dest, const void *src, int count)
{
    void *ptr = dest;
    
    if(NULL == dest || NULL == src || count <= 0)  
    {   
        return NULL;    
    }
    
    while(count--)
    {
        *(char *)dest++ = *(char *)(src+count); 
    }

    return ptr;
}
//
// 业务数据参数接口操作
//

/**
*********************************************************************
* @brief：读交互类参数, 普通的读取过程会先从ramdisk开始, 读取成功就直接返回；如果失败，再
          从nand flash的文件系统区中读取对应参数文件。对于重要的系统运行参数在前两种介质均
          读取失败后还会从eeprom中尝试读取参数。
* @param[in]： oad     - 参数项ID
               mtrNum  - 测量点号, TMN_ID代表终端
               para    - 返回缓冲区
               len     - 缓冲区长度
               inMaxLen- 传入buf最大长度，防止outData溢出 
* @param[out]：NULL
* @return：    < 0     - 错误
               >=0     - 参数项长度
*********************************************************************
*/
int read_xpara(HUDP hUdp, OOP_OAD_U oad, uint32 inMaxLen, char *para, uint32 *len, mtrnum_t mtrNum)
{
    return 0;
}

/**
*********************************************************************
* @brief：     写参数数据
* @param[in]： oad     - 参数项ID
               para    - 参数存放缓冲区
               mtrNum  - 信息点号, TMN_ID代表终端
               len     - 缓冲区长度限制
* @param[out]：NULL
* @return：    < 0     - 错误
               >=0     - 参数项长度
*********************************************************************
*/
int write_xpara(HUDP hUdp, OOP_OAD_U oad, const char *para, uint32 len, mtrnum_t mtrNum)
{
    return 0;
}

/**
*********************************************************************
* @brief：     读取测量点当日某种类型的数据项
* @param[in]： mtrNum  - 信息点
               oad     - 数据项ID
               buf     - 数据项成功读取后的返回地址缓冲区
               len     - 缓冲区长度
               inMaxLen- 传入buf最大长度，防止outData溢出 
* @param[out]：NULL
* @return：    >= 0    - 成功读取后的数据项字节数
               < 0     - 失败
*********************************************************************
*/
int read_xdata(HUDP hUdp, mtrnum_t mtrNum, OOP_OAD_U oad, uint32 inMaxLen, char *buf, uint32 *len)
{
    return 0;
}

/**
*********************************************************************
* @brief：      测量点当前业务数据写入接口(扩展版本)
* @param[in]：  oad     - 数据项id
                len     - 写入的字节数
                infoNum - 测量点号
                buf     - 返回的数据项内容
                len     - 缓冲区长度限制
* @param[out]： NULL
* @return：     < 0 : 错误
                >=0 : 返回的字节数
*********************************************************************
*/
int write_xdata(HUDP hUdp, mtrnum_t mtrNum, OOP_OAD_U oad, const char *buf, uint32 len)
{
    return 0;
}



//
// 私有类数据接口
//
#if (1)
#define WR_TYPE            (O_RDWR)
#define WR_STAT            0    //统计写入次数
#if WR_STAT == 1
uint64 bNeedWrite = 0;
uint64 bNotWrite = 0;
#endif
int ReadWrChk(int fd, int offset, const char *pWrBuf, int len, BOOL *pbln_syn)
{
    int    filesize = 0;        //读取文件的大小
    int tmpFlag = 0;        //检验表示

    *pbln_syn = FALSE;

    if(len <= 0)
    {
        return ERR_READWR;
    }

    //文件过小，可以写入
    filesize  = FGetFileSize(fd);
    if( filesize < offset + len)
    {
        *pbln_syn = TRUE;
        return 1;
    }

    char pRdBuf[len];                    //申请读取缓冲区
    memset(pRdBuf, 0x0, len);

    //无法读取 或者 读取的内容发生变化，可以写入
    if( read(fd, pRdBuf, len) != len || memcmp(pRdBuf, pWrBuf, len) != 0)
    {
        *pbln_syn = TRUE;
        tmpFlag = 1;
    }
    else
    {
        tmpFlag = 0;
    }
    lseek(fd, offset, SEEK_SET);        //游标移回去

    #if WR_STAT == 1
    int i;
    if(tmpFlag == 1)
        bNeedWrite++;
    else
        bNotWrite++;
    printf("pid = %d        offset = %d        tmpFlag = %d        bNeedWrite = %lld        bNotWrite = %lld\n", getpid(), offset, tmpFlag, bNeedWrite, bNotWrite);
    printf("pRdBuf = ");
    for(i = 0; i < len; i++)
        printf(" %02X", pRdBuf[i]);
    printf("\n");
    printf("pWrBuf = ");
    for(i = 0; i < len; i++)
        printf(" %02X", pWrBuf[i]);
    printf("\n");
    #endif

    return tmpFlag;
}


#else
#define WR_TYPE        O_WRONLY
int ReadWrChk(int fd, int offset, const char *pWrBuf, int len, BOOL *pbln_syn)
{
    return 1;
}
#endif

//初始化文件头
inline static void InitFileHeader(xlFileHeader *header)
{
    if(NULL == header) return;

    memset(header, 0, sizeof(xlFileHeader));

    header->fh_signature[0] = 'X';
    header->fh_signature[1] = 'L';
    header->fh_version      = 10*MAJOR + MINOR;
    header->fh_projID       = PROJECTID;
    header->fh_len          = sizeof(xlFileHeader);
    //文件中所有测量点的偏移信息
    memset(header->OffBase, 0xFF, sizeof(header->OffBase));
}

//设置测量点文件状态
inline static void SetMtrFileState(BOOL bSet)
{
    if(bSet == TRUE)
    {
        while(g_xparaInfo.bWriteFile == TRUE)
        {
            usleep(1);
        }
        if(g_xparaInfo.bWriteFile == FALSE
            && g_xparaInfo.bWriteFile == FALSE
            && g_xparaInfo.bWriteFile == FALSE)
        {
            g_xparaInfo.bWriteFile = TRUE;
        }
        else
        {// 再等一次
            while(g_xparaInfo.bWriteFile == TRUE)
            {
                usleep(1);
            }
            g_xparaInfo.bWriteFile = TRUE;
        }
    }
    else
        g_xparaInfo.bWriteFile = FALSE;
}

/*
名  称: fillBlankChars
功  能: 往fd指向的文件中填写填充字符
参  数:
        fd     - 待写文件的文件描述符
        offset - 相对于文件首字节的字节偏移量
        len    - 填充数量
返回值:
        (无)
*/
inline static void fillBlankChars(int fd, int offset, int len)
{
    int   iCnt, iFrag, i;
    uint8 buff[32];

    memset(buff, ERASE_CHAR, 32);
    iCnt  = (int)(len / 32);
    iFrag = len % 32;

    writew_lock(fd, offset, len); //Lock

    lseek(fd, offset, SEEK_SET); /* 偏移量定位 */
    if( iFrag) write(fd, buff, iFrag);
    for( i = 0; i < iCnt; i++)
        write(fd, buff, 32);

    un_lock(fd, offset, len); //Unlock
}

/**
*********************************************************************
* @brief：     在文件系统数据区中注册一个私有类数据文件，即生成一个私有类数据文件.
* @param[in]： filename - 注册的私有类数据文件名称
*              appName - 注册app名称
* @param[out]：NULL
* @return：    <0       - 错误
               =0       - 成功
*********************************************************************
*/
int reg_pdata_xram(const char *appName, const char *filename)
{
    xlFileHeader  fheader;
    char          pdataPath[PATH_MAX_LEN + 1];
    int           pdata_fd;

    if (!m_bInited) return ERR_UNINITED;

    if (appName == NULL)
    {
        return ERR_PTR;
    }

    if (strlen(appName) > 255)
    {
        return ERR_O_RANGE;
    }

    memcpy(m_AppName, appName, strlen(appName) + 1);

    /* 如果data目录不存在，默认创建一个 */
    if(filename[0] == '#')
        sprintf(pdataPath, "%sdata", g_pcszRamDiskPath);
    else if (filename[0] == '/')
    {
        ExtractFilePath(filename, pdataPath);
    }
    else
        sprintf(pdataPath, "%s%s/data", g_pcszRootPath, m_AppName);

    if( !PathExists(pdataPath)) {
        if( !makePathedDir(pdataPath))
            return -1;
    }

    if(filename[0] == '#')
        sprintf(pdataPath, "%sdata/%s", g_pcszRamDiskPath, &filename[1]);
    else if (filename[0] == '/')
        sprintf(pdataPath, "%s", filename);
    else
        sprintf(pdataPath, "%s%s/data/%s", g_pcszRootPath, m_AppName, filename);

    if(FileExists(pdataPath))
        return 0; //文件存在,直接返回0(ERR_OK),表示成功

    InitFileHeader(&fheader);
    fheader.fh_typeID = FILE_PRIVATE;

    pdata_fd = open(pdataPath, O_CREAT|O_TRUNC|O_WRONLY, 0774);
    if( write(pdata_fd, &fheader, sizeof(xlFileHeader_min)) < 0)  //写文件头
        return ERR_MEM;  //不能写，判断为磁盘空间不足
    fsync(pdata_fd);
    close(pdata_fd);

    return ERR_OK;
}


/**
*********************************************************************
* @brief：     读私有类数据文件
* @param[in]： filename  - 文件名
               offset    - 相对于数据内容的偏移量
               len       - 读取长度
               buf       - 返回的缓冲区
* @param[out]：NULL
* @return：    <0        - 错误
               =0        - 成功
*********************************************************************
*/
int read_pdata_xram(const char *filename, char *buf, unsigned offset, unsigned len)
{
    xlFileHeader  fheader;
    char          pdataPath[PATH_MAX_LEN + 1];
    int           pdata_fd, size;

    if( !m_bInited) return ERR_UNINITED;

    if((filename == NULL) || (buf == NULL)) return ERR_PTR;
    if(len == 0) return ERR_OK;

    DPRINT(">>ReadPDataXram\n");

    if(filename[0] == '#')
        sprintf(pdataPath, "%sdata/%s", g_pcszRamDiskPath, &filename[1]);
    else if (filename[0] == '/')
    {
        sprintf(pdataPath, "%s", filename);
    }
    else
        sprintf(pdataPath, "%s%s/data/%s", g_pcszRootPath, m_AppName, filename);

    if((pdata_fd = open(pdataPath, O_RDONLY)) < 0)
    {
        STORE_FMT_DEBUG("%s open: %s.\n", pdataPath, strerror(errno));
        return ERR_ACCESS;
    }

    if( readw_lock(pdata_fd, 0, sizeof(xlFileHeader_min)) < 0)
    {
        close(pdata_fd);
        return ERR_LOCK;
    }

    if(read(pdata_fd, &fheader, sizeof(xlFileHeader_min)) < 0)  //读文件头
    {
        un_lock(pdata_fd, 0, sizeof(xlFileHeader_min));
        close(pdata_fd);
        return ERR_IO;
    }

    un_lock(pdata_fd, 0, sizeof(xlFileHeader_min));

    if( (size = FGetFileSize(pdata_fd)) < 0)  //文件长度出错
    {
        close(pdata_fd);
        return ERR_O_RANGE;
    }

    if( (size - sizeof(xlFileHeader_min)) < (offset + len)) //是否数据指针越界
    {
        close(pdata_fd);
        return ERR_FILEEXCEED;
    }

    lseek(pdata_fd, sizeof(xlFileHeader_min) + offset, SEEK_SET);

    if( readw_lock(pdata_fd, sizeof(xlFileHeader_min) + offset, len) < 0)
    {
        close(pdata_fd);
        return ERR_LOCK;
    }

    if( read(pdata_fd, buf, len) < 0)
    {
        un_lock(pdata_fd, sizeof(xlFileHeader_min) + offset, len);
        close(pdata_fd);
        return ERR_IO;
    }

    un_lock(pdata_fd, sizeof(xlFileHeader_min) + offset, len);
    close(pdata_fd);
    return ERR_OK;
}

/**
*********************************************************************
* @brief：     写私有类数据文件
* @param[in]： filename  - 文件名
               buf       - 输入缓冲区
               offset    - 相对于数据内容的偏移量
               len       - 返回的缓冲区
* @param[out]：NULL
* @return：    <0        - 错误
               =0        - 成功
*********************************************************************
*/
int write_pdata_xram(const char *filename, const char *buf, unsigned offset, unsigned len)
{
    char          pdataPath[PATH_MAX_LEN + 1];
    int           fdPriv=-1;
    int           delta, i;
    int           filesize;
    xlFileHeader  fheader;
    int           cnt, frag;
    char          *bufOff;
    BOOL bln_syn = FALSE, tmp_syn = FALSE;
    int ret  =0;

    DPRINT(">>WritePDataXram: FileName = \"%s\", offset=0x%x, len=0x%x\n", filename, offset, len);

    if( !m_bInited) return ERR_UNINITED;
    if(!filename || !buf) return ERR_PTR;
    if(len == 0) return ERR_OK;

    if(filename[0] == '#')
        sprintf(pdataPath, "%sdata/%s", g_pcszRamDiskPath, &filename[1]);
    else if (filename[0] == '/')
    {
        sprintf(pdataPath, "%s", filename);
    }
    else
        sprintf(pdataPath, "%s%s/data/%s", g_pcszRootPath, m_AppName, filename);

    if( access(pdataPath, F_OK) < 0)
    {
        STORE_FMT_TRACE("%s access: %s.\n", pdataPath, strerror(errno));

        /* 如果私有文件不存在，就创建一个 */
        InitFileHeader(&fheader);
        fheader.fh_typeID = FILE_PRIVATE;
        fdPriv = open(pdataPath, O_CREAT|O_TRUNC|WR_TYPE, 0664);
        if(fdPriv < 0)
        {
            STORE_FMT_DEBUG("%s open: %s.\n", pdataPath, strerror(errno));
            return ERR_ACCESS;
        }
        
        ret = write(fdPriv, &fheader, sizeof(xlFileHeader_min));
        if(ret == -1 )
        {
            STORE_FMT_DEBUG("%s write: %s.\n", pdataPath, strerror(errno));
            return ERR_IO;
        }
    }
    else
    {  /* 如果以前存在，则以只写方式打开  */
        if( (fdPriv = open(pdataPath, WR_TYPE)) < 0)
        {      
            STORE_FMT_DEBUG("%s open: %s.\n", pdataPath, strerror(errno));
            return ERR_ACCESS;
        }
    }

    /*--- 填充缺省字符流 ---*/
    filesize = FGetFileSize(fdPriv);
    delta    = offset + sizeof(xlFileHeader_min) - filesize;
    if(delta > 0)
    {
        SetMtrFileState(TRUE);
        filesize = FGetFileSize(fdPriv);
        delta    = offset + sizeof(xlFileHeader_min) - filesize;
        if(delta > 0)
            fillBlankChars(fdPriv, filesize, delta); /* 填写缺省字符 */
        SetMtrFileState(FALSE);
    }

    /*--- 数据写入 ---*/
    ret = lseek(fdPriv, offset + sizeof(xlFileHeader_min), SEEK_SET);
    if(ret == -1)
    {
        STORE_FMT_DEBUG("%s lseek: %s.\n", pdataPath, strerror(errno));
    }
    
    if( writew_lock(fdPriv, offset + sizeof(xlFileHeader_min), len) < 0)
    {
        close(fdPriv);
        return ERR_LOCK;
    }

    cnt  = (int)(len / 512);
    frag = (int)(len % 512);
    bufOff = (char *)buf;

    /* 分多次写入,防止一次性写入时造成数据丢失. */
    if(filename[0] == '#')        //写入ramdisk
    {
        if(write(fdPriv, buf, frag) < 0)
        {
            un_lock(fdPriv, offset + sizeof(xlFileHeader_min), len);
            close(fdPriv);
            return ERR_IO;
        }
    }
    else
    {
        if((ReadWrChk(fdPriv, offset + sizeof(xlFileHeader_min), buf, frag, &bln_syn) > 0) && write(fdPriv, buf, frag) < 0)
        {
            un_lock(fdPriv, offset + sizeof(xlFileHeader_min), len);
            close(fdPriv);
            return ERR_IO;
        }
        #if WR_STAT == 1
        if(bln_syn == TRUE)
            printf("111111        pid = %d        offset = %d        filename = %s\n", getpid(), offset, filename);
        #endif
    }

    tmp_syn = bln_syn;
    bufOff += frag;
    for( i = 0; i < cnt; i++)
    {
        if(filename[0] == '#')        //写入ramdisk
        {
            write(fdPriv, bufOff, 512);
        }
        else
        {
            if(ReadWrChk(fdPriv, offset + sizeof(xlFileHeader_min) + frag + 512 * i, bufOff, 512, &bln_syn) > 0)
            {
                tmp_syn = TRUE;
                #if WR_STAT == 1
                printf("7777777        pid = %d        offset = %d\n", getpid(), offset + sizeof(xlFileHeader_min) + frag + 512 * i);
                #endif
                write(fdPriv, bufOff, 512);
            }
        }
        bufOff += 512;
    }

    if(tmp_syn == TRUE)
        fsync(fdPriv);
    un_lock(fdPriv, offset + sizeof(xlFileHeader_min), len);
    close(fdPriv);
    DPRINT("Exit WritePDataXram\n");

    return ERR_OK;
}

/**
*********************************************************************
* @brief：     清除数据文件中的所有数据，只保留文件头。
* @param[in]： filename - 私有类数据文件名(无路径)
* @param[out]：NULL
* @return：    <0       - 错误
               =0       - 成功
*********************************************************************
*/
int clear_pdata_xram(const char *filename)
{
    int fd;
    char path[PATH_MAX_LEN + 1];

    if( !m_bInited) return ERR_UNINITED;

    if(filename[0] == '#')
        sprintf(path, "%sdata/%s", g_pcszRamDiskPath, &filename[1]);
    else if (filename[0] == '/')
    {
        sprintf(path, "%s", filename);
    }
    else
        sprintf(path, "%s%s/data/%s", g_pcszRootPath, m_AppName, filename);

    if( (fd = open(path, O_WRONLY)) < 0) //打开私有类数据文件
        return ERR_ACCESS;

    if( writew_lock(fd, 0, 0) < 0) //加锁
    {
        close(fd);
        return ERR_LOCK;
    }

    ftruncate(fd, sizeof(xlFileHeader_min));
    close(fd);

    return ERR_OK;
}

/**
*********************************************************************
* @brief：     删除数据区中，filename指定的私有类文件.filename不
               需要带文件路径.
* @param[in]： filename - 私有类数据文件名(无路径)
* @param[out]：NULL
* @return：    <0       - 错误
               =0       - 成功
*********************************************************************
*/
int remove_pdata_xram(const char *filename)
{
    char path[PATH_MAX_LEN + 1];
    if( !m_bInited) return ERR_UNINITED;

    sprintf(path, "%s%s/data/%s", g_pcszRootPath, m_AppName, filename);

    if( access(path, F_OK) >= 0)
    remove(path);

    return ERR_OK;
}

/**
*********************************************************************
* @brief：     eeprom私有区注册函数。
* @param[in]： id   - 待注册的id编号
               len  - 注册字节长度
               base - 返回的eeprom起始地址
* @param[out]：NULL
* @return：    <0   - 错误
               =0   - 成功
*********************************************************************
*/
int reg_pdata_eeprom(unsigned id, unsigned len, uint32 *base)
{
    EpdataFileHeader header;
    EpdataItemInfo   itemInfo;
    char name[256] = {0};
    char path[256] = {0};
    int  fd;
    //BOOL bExist;
    int  off;
    pdatachk_t  chk; /* 存放在eeprom中每个注册项起始地址的3字节注册信息 */
    char buf[128];
    int  cnt, frag, i;
    int  offset;

    sprintf(name, "%s%s/.epdatainfo", g_pcszRootPath, m_AppName);
    sprintf(path, "%s%s/", g_pcszRootPath, m_AppName);

    if( !FileExists(name)) /* epdata信息文件不存在，可以认为尚未有任务注册使用 */
    {
        if( !PathExists(path))
            makePathedDir(path);

        if( (fd = open(name, O_WRONLY | O_CREAT | O_TRUNC, 0660)) < 0)
            return ERR_IO;

        writew_lock(fd, 0, 0); /* 文件加锁 */
        //bExist = FALSE;
        header.freeoffset = PDATABASE;
    }
    else
    {
        if( (fd = open(name, O_RDWR)) < 0)
            return ERR_IO;

        //bExist = TRUE;
        memset(&header, 0, sizeof(header));

        writew_lock(fd, 0, 0); /* 文件加锁 */
        read(fd, &header, sizeof(header)); /* 读注册文件头 */

        if( header.freeoffset < PDATABASE || header.freeoffset >= EEPROM_SIZE_TOTAL)
            header.freeoffset = PDATABASE;
    }

    off = sizeof(header) + id*sizeof(EpdataItemInfo); /* 找到已注册项的偏移量 */
    memset(&itemInfo, 0, sizeof(EpdataItemInfo));
    lseek(fd, off, SEEK_SET);
    read(fd, &itemInfo, sizeof(EpdataItemInfo));

    /* 0xabcd为数据项有效性标志，如果itemInfo.ValidId = 0xabcd, 可以认为该数据项地址中已经
    * 存放了内容, 否则认为这个数据项地址还没有被使用。
    */
    if( 0xabcd == itemInfo.ValidId)
    {
        writelog("/tmp/reg.txt", "id:%d, off:%x, len:%d", id, itemInfo.offbase, itemInfo.len);
        *base = itemInfo.offbase;
    }
    else
    {
        /* --向.epdatainfo文件中新加一个注册项-- */
        itemInfo.ValidId = 0xabcd; /* 有效性标志 */
        itemInfo.len     = len & 0xffff;
        itemInfo.offbase = header.freeoffset;

        if( header.freeoffset + sizeof(pdatachk_t) + itemInfo.len > EEPROM_SIZE_TOTAL)
        {
            close(fd);
            return ERR_O_RANGE;
        }

        *base = itemInfo.offbase;

        /* 写.pdatainfo文件中的注册内容 */
        lseek(fd, off, SEEK_SET);
        write(fd, &itemInfo, sizeof(EpdataItemInfo));

        /* 写eeprom中注册项头信息 */
        chk.id    = id & 0xff;
        chk.size  = itemInfo.len;

        DPRINT("chk.id=%x, chk.size=%x\n", chk.id, chk.size);

        if( write_eeprom(header.freeoffset, sizeof(pdatachk_t), (const char *)&chk) < 0)
            return ERR_IFACE;

        /* 初始化eeprom内存空间，清0 */
        cnt = (int)(len/128);
        frag = len%128;
        memset(buf, 0, 128);
        offset = header.freeoffset + sizeof(pdatachk_t);

        DPRINT("init eeprom: offset=%x, cnt=%d, frag=%d, len=%d\n", offset, cnt, frag, len);

        if( write_eeprom(offset, frag, buf) < 0)
            printf("RegPDataEeprom: write_eeprom error!\n");
        offset += 128;
        for( i = 0; i < cnt; i++) {
            if( write_eeprom(offset, 128, buf) < 0)
                printf("RegPDataEeprom: write_eeprom error!\n");
            offset += 128;
        }

        /* 刷新文件头  */
        header.freeoffset += sizeof(pdatachk_t) + len; /* eeprom私有类数据占用字节: 注册信息(3字节) + len字节注册长度 */
        lseek(fd, 0, SEEK_SET);
        write(fd, &header, sizeof(EpdataFileHeader));
    }
    fsync(fd);
    close(fd); /* 文件关闭并解锁 */

    return 0;
}

/**
*********************************************************************
* @brief：     读eeprom私有类数据
* @param[in]： id      - 注册获得的私有类空间id
               base    - 注册空间基地址
               offset  - 注册空间内字节偏移
               len 
* @param[out]：NULL
* @return：    <0      - 错误
               >= 0    - 成功读取后的数据项字节数
*********************************************************************
*/
int read_pdata_eeprom(unsigned id, uint32 base, uint32 offset, char *buf, unsigned len)
{
    pdatachk_t  chk;
    int         ret;
    int         retval;

    DPRINT("ReadPDataEeprom: id=%d, base=0x%x, offset=%ld, len=%d\n", id, (unsigned int)base, offset, len);

    if( !m_bInited) return -ERR_UNINITED;
    if( (NULL == buf)) return -ERR_PTR;
    if( (len > REG_MAX_LEN) || (base > EEPROM_SIZE_TOTAL)) return -ERR_O_RANGE;

    /* 读注册信息 */
    if((ret = read_eeprom(base, sizeof(pdatachk_t), (char *)&chk, FALSE)) < 0) return ret;
    if(chk.id != id) return -ERR_NPERM;

    if(chk.size < (offset + len)) return -ERR_O_RANGE;

    /* 读数据 */
    retval = read_eeprom(base + PDATACHK_SIZE + offset, len, buf, FALSE);
    return retval;
}

/**
*********************************************************************
* @brief：     读eeprom私有类数据
* @param[in]： id     - 注册的id号
               base   - 注册空间的起始地址
               offset - 注册空间内偏移量
               buf    - 数据来源缓冲区
               len    - 写入字节长度
* @param[out]：NULL
* @return：    <0     - 错误
               >= 0   - 写入长度
*********************************************************************
*/
int write_pdata_eeprom(unsigned id, uint32 base, uint32 offset, const char *buf, unsigned len)
{
    pdatachk_t chk;

    if( !m_bInited) return -ERR_UNINITED;
    if(!buf) return -ERR_PTR;
    if( (len > REG_MAX_LEN) || (base > EEPROM_SIZE_TOTAL))
        return -ERR_O_RANGE;

    /* 先读注册项信息,判断写入地址是否合法 */
    if( read_eeprom(base, sizeof(pdatachk_t), (char *)&chk, FALSE) < 0) return -ERR_IFACE;
    if( chk.id != (uint8)id) return -ERR_NPERM;
    if( chk.size < (offset + len)) return -ERR_O_RANGE;

    DPRINT(">>WritePDataEeprom: 0x%lx, %u\n", base + PDATACHK_SIZE + offset, len);

    /* 写数据 */
    return write_eeprom( base + PDATACHK_SIZE + offset, len, buf);
}

/**
*********************************************************************
* @brief：      清除Eeprom中私有类数据区的内容
* @param[in]：  base - 注册返回的基地址
                id   - 注册返回的id
* @param[out]： NULL
* @return：     <0   - 错误
                =0   - 成功
*********************************************************************
*/
int clear_pdata_eeprom(uint32 base, int id)
{
    pdatachk_t chk;
    uint8      buf[64];
    int        cnt, frag, i;

    //检查
    if( !m_bInited) return -ERR_UNINITED;

    if( base > EEPROM_SIZE_TOTAL)
        return -ERR_O_RANGE;

    if( read_eeprom(base, sizeof(pdatachk_t), (char *)&chk, FALSE) < 0) return -ERR_IFACE;
    if( chk.id != (uint8)id) return -ERR_NPERM;

    cnt  = (int)(chk.size/64);
    frag = chk.size%64;

    //将注册的eeprom私有类数据区所有字节写成ERASE_CHAR
    memset(buf, ERASE_CHAR, 64);
    write_eeprom( base + PDATACHK_SIZE, frag, (char *)buf);
    for( i = 0; i < cnt; i++)
        write_eeprom( base + PDATACHK_SIZE + frag + i*64, 64, (char *)buf);
    return ERR_OK;
}



//
// 日志调试
//

/* 
    * 用来向filepath指向的日志文件写入一行日志，fmt可以
    * 像printf(fmt, ...)中的fmt一样支持转义字符和可变参数.
    * 返回0表示成功，负数表示失败.
    * 注意: 不要在fmt后加'\n'换行，writelog会自动处理换行.
*/

/**
*********************************************************************
* @brief       写日志
* @param[in]   filepath - 文件名称
               fmt      - 写文件内容
* @param[out]  
* @return      >= 0     - 成功读取后的数据项字节数
               < 0      - 失败 
*********************************************************************
*/
int writelog(const char *filepath, const char *fmt, ...)
{
    int         fd;
    char        path[PATH_MAX_LEN + 1];
    time_t      t;
    struct tm   *ptm;
    va_list     ap;
    char pathsub[8100];    

    if( !FileExists(filepath)) 
    {
        ExtractFilePath(filepath, path);
        if( !PathExists(path)) makePathedDir(path);
        if( (fd = open(filepath, O_WRONLY | O_CREAT | O_TRUNC)) < 0) //新建文件
          return -1;
        fchmod(fd, 0664);
        close(fd);
    }

    if(GetFileSize(filepath)>LOG_MAX_SIZE)
    {
        #if 1
        memset(pathsub,0x0,sizeof(pathsub));
        strcat(pathsub,filepath);
        strcat(pathsub,".sub");
        remove(pathsub);
        rename(filepath,pathsub);
        #else
        //back up *.log 
        char tarname[PATH_MAX_LEN + 1] = {0};
        char cmd[255] = {0};
        memset(pathsub,0x0,sizeof(pathsub));
        strcat(pathsub,filepath);
        strcat(pathsub,".sub");    
        rename(filepath,pathsub);
        //strcat(pathsub,".sub");
        strcat(tarname,pathsub);
        strcat(tarname,".tar.gz");
        sprintf(cmd,"tar zcf %s %s",pathsub,tarname);
        system(cmd);
        remove(pathsub);
        #endif
    }        
    if( (fd = open(filepath, O_CREAT |O_WRONLY | O_APPEND)) < 0) //将内容附加到文件末尾
        return -1;
    if( writew_lock(fd, 0, 0) < 0)
    {
      //perror("writew_lock");
        close(fd);
        return -1;
    }
    t = time(NULL);
    ptm = localtime(&t);
    /* 写入日期 */
    sprintf(pathsub, "%4d-%02d-%02d %02d:%02d:%02d ", ptm->tm_year + 1900, ptm->tm_mon + 1, ptm->tm_mday, ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
    write(fd, pathsub, strlen(pathsub));  
    /* 写入日志字符串 */  
    va_start(ap, fmt);  
    vsnprintf(pathsub, sizeof(pathsub), fmt, ap); /* pathsub再次回收利用，用来保存写入的字符串 */
    va_end(ap);
    write(fd, pathsub, strlen(pathsub));

    /* 换行 */
    write(fd, "\n", strlen("\n")); 
    un_lock(fd, 0, 0); //Unlock
    
    close(fd);
    return 0;
}

/**
*********************************************************************
* @brief       写二进制日志
* @param[in]   filepath    - 文件名称
*              strbuf      - 注释字符串
*              strbuf_len  - 注释字符串长度
*              databuf     - 16进制内容
*              databuf_len - 16进制长度
* @param[out]  
* @return      >= 0        - 成功读取后的数据项字节数
               < 0         - 失败 
*********************************************************************
*/
int writelog_bin(const char *filepath, const char *strbuf ,unsigned int strbuf_len ,const char *databuf,unsigned int databuf_len)
{
    int         fd, i, cnt, frag;
    char        path[PATH_MAX_LEN + 1];
    //char        pathsub[PATH_MAX_LEN + 1];
    BinLogHead    logHead;
    char        *bufOff = NULL;

    //既没有字符串也没有报文要写入，则不再处理
    if(!(strbuf != NULL && strbuf_len > 0) && !(databuf != NULL && databuf_len > 0))
        return -1;

    if( !FileExists(filepath)) 
    {
        ExtractFilePath(filepath, path);
        if( !PathExists(path)) makePathedDir(path);
        if( (fd = open(filepath, O_WRONLY | O_CREAT | O_TRUNC)) < 0) //新建文件
            return -1;
        fchmod(fd, 0664);
    }

    if(GetFileSize(filepath) > (LOG_MAX_SIZE / 10))
    {
        time_t bStart = time(NULL);
        TarLogFile(filepath, 10);
        printf("need time %lds\n", time(NULL) - bStart);
        //memset(pathsub,0x0,sizeof(pathsub));
        //strcat(pathsub,filepath);
        //strcat(pathsub,".sub");
        //remove(pathsub);
        //rename(filepath,pathsub);
    }

    //报文信息头
    memset((void *)&logHead, 0x0, sizeof(logHead));
    logHead.allsize = sizeof(logHead);        //报文信息头
    if(strbuf != NULL && strbuf_len > 0)    //写入字符串
        logHead.allsize += strbuf_len;
    if(databuf != NULL && databuf_len > 0)    //写入报文
    {
        logHead.allsize += databuf_len;
        logHead.databuf_len = databuf_len;
    }
    logHead.time = time(NULL);

    char write_buf[logHead.allsize];        //写入日志文件的缓冲区
    int buf_pos = 0;                    //记录偏移位置
    //报文信息头
    memset(write_buf, 0x0, sizeof(write_buf));
    memcpy(write_buf, (char *)&logHead, sizeof(logHead));
    buf_pos += sizeof(logHead);
    //写入字符串
    if(strbuf != NULL && strbuf_len > 0)
    {
        memcpy(write_buf + buf_pos, strbuf, strbuf_len);
        buf_pos += strbuf_len;
    }
    //写入报文
    if(databuf != NULL && databuf_len > 0)
    {
        memcpy(write_buf + buf_pos, databuf, databuf_len);
        buf_pos += databuf_len;
    }

    cnt  = (int)(buf_pos / 512);
    frag = (int)(buf_pos % 512);
    bufOff = (char *)write_buf;

    fd = open(filepath, O_CREAT|O_WRONLY|O_APPEND, 0664);
    if(fd < 0)
        return -1;

    if( writew_lock(fd, 0, 0) < 0)
    {
        close(fd);
        return -1;
    }  

    /* 分多次写入,防止一次性写入时造成数据丢失. */
    write(fd, bufOff, frag);
    bufOff += frag;
    for( i = 0; i < cnt; i++) 
    {
        write(fd, bufOff, 512);
        bufOff += 512;
    }

    fsync(fd);
    close(fd);
    return 0;
}

/**
*********************************************************************
* @brief       压缩日志文件
* @param[in]   file_path    - 日志文件路径
               backup_count - 支持压缩备份的份数 
* @param[out]  
* @return      错误信息    
*********************************************************************
*/
int tar_log_backup(const char *filepath, int backup_count)
{
    TarLogFile(filepath, backup_count);
    return 1;
}



//
// EEPROM操作
//

/**
*********************************************************************
* @brief：     读EEPROM数据
* @param[in]： buf    - 数据项成功读取后的返回地址缓冲区
               offset - 偏移
               len    - 数据长度
* @param[out]：NULL
* @return：    >= 0   - 成功读取后的数据项字节数
               < 0    - 失败
*********************************************************************
*/
int read_eeprom(uint32 offset, uint32 len, char *buf, BOOL bRaw)
{
    char  path[PATH_MAX_LEN];
    int   fd;

    if( !buf) return ERR_PTR;
    if( (len > REG_MAX_LEN) || ((offset + len) > EEPROM_SIZE_TOTAL) ) return ERR_O_RANGE;
    if( bRaw) goto READ_EPRM;

    /* 从eeprom的ramdisk镜像文件中读取数据 */
    sprintf(path, "%seeprom.dat", g_pcszRamDiskPath);
    if( (fd = open(path, O_RDONLY)) >= 0)
    {
        if( readw_lock(fd, offset, len) < 0) 
        {
            close(fd);
            goto READ_EPRM;
        }

        lseek(fd, offset, SEEK_SET);
        if( read(fd, buf, len) < 0) 
        {
            close(fd);
            goto READ_EPRM;
        }

        close(fd);
        return 0;
    }

READ_EPRM:
    #if (EEPROM_SIM_EN == 0) && defined(ARM)
    if( ReadDataFromEEPROM(0, buf, offset, len) > 0)
        return 0;
    #else
    /* 文件模拟 */
    sprintf(path, "%s%s/eeprom.dat", g_pcszRootPath, m_AppName);
    if( (fd = open(path, O_RDONLY)) >= 0)
    {
        readw_lock(fd, offset, len);
        lseek(fd, offset, SEEK_SET);
        read(fd, buf, len);

        close(fd);
        return 0;
    }
    #endif

    return -1;
}

/**
*********************************************************************
* @brief：     读EEPROM数据
* @param[in]： buf    - 数据项成功读取后的返回地址缓冲区
               offset - 偏移
               len    - 数据长度
* @param[out]：NULL
* @return：    >= 0   - 成功读取后的数据项字节数
               < 0    - 失败
*********************************************************************
*/
int read_from_eeprom(char *buf, uint32 offset, uint32 len)
{
    if( !buf) return ERR_PTR;
    if( (len > REG_MAX_LEN) || ((offset + len) > EEPROM_SIZE_TOTAL) ) return ERR_O_RANGE;

#if (EEPROM_SIM_EN == 0) && defined(ARM)
    if( ReadDataFromEEPROM(0, buf, offset, len) == len)
        return len;
#else
    /* 文件模拟 */
    char  path[PATH_MAX_LEN];
    int   fd;
    sprintf(path, "%s%s/eeprom.dat", g_pcszRootPath, m_AppName);
    if( (fd = open(path, O_RDONLY)) >= 0)
    {
        readw_lock(fd, offset, len);
        lseek(fd, offset, SEEK_SET);
        read(fd, buf, len);
        
        close(fd);
        return len;
    }
#endif
    
    return -1;
}

inline static int write_to_eepromfile(const char *path, uint32 offset, const char *buf, uint32 len)
{
    BOOL  bExist = FALSE;
    int   cnt, frag, i;
    uint8 buff[64];
    int   fd;

    if( FileExists(path))
    bExist = TRUE;
    if( (fd = open(path, O_WRONLY | O_CREAT)) < 0)
    return ERR_ACCESS;

    if( !bExist) 
    {
        /*
        * 如果是第一次创建文件,默认长度设置成EEPROM_SIZE_TOTAL, 
        * 写入默认填充字符.
        */
        lseek(fd, 0, SEEK_SET);
        memset(buff, ERASE_CHAR, 64);
        cnt  = (int)(EEPROM_SIZE_TOTAL / 64);
        frag = EEPROM_SIZE_TOTAL % 64;

        if( writew_lock(fd, 0, 0) < 0)
        {
            close(fd);
            return ERR_LOCK;
        }

        fchmod(fd, 0664);
        write(fd, (int *)buff, frag);

        for( i = 0; i < cnt; i++) 
        {
            write(fd, buff, 64);
        }

        fsync(fd);
        un_lock(fd, 0, 0);
    }

    if( writew_lock(fd, offset, len) < 0)
    {
        close(fd);
        return ERR_LOCK;
    }

    lseek(fd, offset, SEEK_SET);
    write(fd, buf, len);
    close(fd); /* 解锁并关闭文件 */ 

    return 0;
}

/**
*********************************************************************
* @brief：     写EEPROM数据
* @param[in]： buf    - 数据缓存
               offset - 偏移
               len    - 数据长度
* @param[out]：NULL
* @return：    >= 0    - 成功写入后的数据项字节数
               < 0     - 失败
*********************************************************************
*/
int write_eeprom(uint32 offset, uint32 len, const char *buf)
{
    int retval=0;
    char path[PATH_MAX_LEN];

    DPRINT("write_eeprom: offset=0x%x, len=%d\n", offset, len);

    if( !buf) return ERR_PTR;
    if( (len > REG_MAX_LEN) || ((offset + len) > EEPROM_SIZE_TOTAL)) return ERR_O_RANGE;

    sprintf(path, "%seeprom.dat", g_pcszRamDiskPath);
    retval = write_to_eepromfile(path, offset, buf, len);
    if( retval < 0) 
        return retval;

    #if defined(ARM) && (EEPROM_SIM_EN == 0) /* 写入实际的eeprom */ 
    if( WriteDataToEEPROM(0, (char *)buf, offset, len) < 0)
        return -1;
    #else /* undefined ARM or simulate eeprom device */
    /* 将数据写入nandflash中的eeprom模拟文件 */
    sprintf(path, "%s%s/eeprom.dat", g_pcszRootPath, m_AppName);
    retval = write_to_eepromfile(path, offset, buf, len);  
    #endif

    return retval;
}

/**
*********************************************************************
* @brief：     写EEPROM数据
* @param[in]： buf    - 数据缓存
               offset - 偏移
               len    - 数据长度
* @param[out]：NULL
* @return：    >= 0    - 成功写入后的数据项字节数
               < 0     - 失败
*********************************************************************
*/
int write_to_eeprom(const char *buf, uint32 offset, uint32 len)
{
    int retval = 0;
    DPRINT("write_eeprom: offset=0x%x, len=%d\n", offset, len);
    
    if( !buf) return ERR_PTR;
    if( (len > REG_MAX_LEN) || ((offset + len) > EEPROM_SIZE_TOTAL)) return ERR_O_RANGE;

#if defined(ARM) && (EEPROM_SIM_EN == 0) /* 写入实际的eeprom */ 
    if( WriteDataToEEPROM(0, (char *)buf, offset, len) != len)
        return -1;
#else /* undefined ARM or simulate eeprom device */
    /* 将数据写入nandflash中的eeprom模拟文件 */
    char path[PATH_MAX_LEN];
    sprintf(path, "%s%s/eeprom.dat", g_pcszRootPath, m_AppName);
    retval = write_to_eepromfile(path, offset, buf, len);  
#endif
    if(retval < 0)
    {
        return retval;
    }
    return len;
}


