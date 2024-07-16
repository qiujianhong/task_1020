/*
*********************************************************************
* Copyright(C) 2019 �Ͼ��������ӹɷ����޹�˾
* All rights reserved. 
* @brief���ṩ���ݿ���ʽӿ�
* @date�� 2019.10.23
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
// �ڲ�������غ�����ݶ���
//
#define IS_LITTLE_ENDIAN          1       //����Ϣ��ʱ��index label msgtag�Ƿ���С��ģʽ 0Ϊ��� 1ΪС��

/*��̬���ʼ����ɱ�־*/
static BOOL m_bInited = TRUE;
static char m_AppName[256] = {0};

/* ������Ϣ�ṹ���� */
static XParaInfo g_xparaInfo;

const char *g_pcszRootPath    = "/data/app/";       // "/data/";
const char *g_pcszMovePath    = "/tmp/sdcard/";     // "/mnt/sd/";
const char *g_pcszRamDiskPath = "/ramdisk/";        // "/tmp/ramdisk0/";

/* �������� */
int write_eeprom(uint32 offset, uint32 len, const char *buf);
int read_eeprom(uint32 offset, uint32 len, char *buf, BOOL bRaw);

#define RECV_BUF_LENGTH           (8192)  /*recvfrom���ջ�������С*/
#define OCTET_NO_LENGTH           0       /*octet string seize 6 ��������*/
#define TIME_OUT_RETRY            3
#define RECV_BUF_CLEAR            0       /*������ջ��湦��*/
#define MEMPRINT_BUF_LOG  1


/**********************************************************************
* @name      : set_len_offset
* @brief     ������Ԫ�س���
* @param[in] ��eleLen      �������ݳ���
* @param[out]��indata      ����Buffer
* @return    ������Buffer����
* @Create    : 
* @Date      ��2019-10-30
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
* @brief     ����ȡ����������Ԫ�س���
* @param[in] ��*indata      ����Buffer
* @param[out]��*bytes       ����Buffer����
* @return    ������
* @Create    : 
* @Date      ��2019-10-30
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
// �����ӿڲ���
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
// ҵ�����ݲ����ӿڲ���
//

/**
*********************************************************************
* @brief�������������, ��ͨ�Ķ�ȡ���̻��ȴ�ramdisk��ʼ, ��ȡ�ɹ���ֱ�ӷ��أ����ʧ�ܣ���
          ��nand flash���ļ�ϵͳ���ж�ȡ��Ӧ�����ļ���������Ҫ��ϵͳ���в�����ǰ���ֽ��ʾ�
          ��ȡʧ�ܺ󻹻��eeprom�г��Զ�ȡ������
* @param[in]�� oad     - ������ID
               mtrNum  - �������, TMN_ID�����ն�
               para    - ���ػ�����
               len     - ����������
               inMaxLen- ����buf��󳤶ȣ���ֹoutData��� 
* @param[out]��NULL
* @return��    < 0     - ����
               >=0     - �������
*********************************************************************
*/
int read_xpara(HUDP hUdp, OOP_OAD_U oad, uint32 inMaxLen, char *para, uint32 *len, mtrnum_t mtrNum)
{
    return 0;
}

/**
*********************************************************************
* @brief��     д��������
* @param[in]�� oad     - ������ID
               para    - ������Ż�����
               mtrNum  - ��Ϣ���, TMN_ID�����ն�
               len     - ��������������
* @param[out]��NULL
* @return��    < 0     - ����
               >=0     - �������
*********************************************************************
*/
int write_xpara(HUDP hUdp, OOP_OAD_U oad, const char *para, uint32 len, mtrnum_t mtrNum)
{
    return 0;
}

/**
*********************************************************************
* @brief��     ��ȡ�����㵱��ĳ�����͵�������
* @param[in]�� mtrNum  - ��Ϣ��
               oad     - ������ID
               buf     - ������ɹ���ȡ��ķ��ص�ַ������
               len     - ����������
               inMaxLen- ����buf��󳤶ȣ���ֹoutData��� 
* @param[out]��NULL
* @return��    >= 0    - �ɹ���ȡ����������ֽ���
               < 0     - ʧ��
*********************************************************************
*/
int read_xdata(HUDP hUdp, mtrnum_t mtrNum, OOP_OAD_U oad, uint32 inMaxLen, char *buf, uint32 *len)
{
    return 0;
}

/**
*********************************************************************
* @brief��      �����㵱ǰҵ������д��ӿ�(��չ�汾)
* @param[in]��  oad     - ������id
                len     - д����ֽ���
                infoNum - �������
                buf     - ���ص�����������
                len     - ��������������
* @param[out]�� NULL
* @return��     < 0 : ����
                >=0 : ���ص��ֽ���
*********************************************************************
*/
int write_xdata(HUDP hUdp, mtrnum_t mtrNum, OOP_OAD_U oad, const char *buf, uint32 len)
{
    return 0;
}



//
// ˽�������ݽӿ�
//
#if (1)
#define WR_TYPE            (O_RDWR)
#define WR_STAT            0    //ͳ��д�����
#if WR_STAT == 1
uint64 bNeedWrite = 0;
uint64 bNotWrite = 0;
#endif
int ReadWrChk(int fd, int offset, const char *pWrBuf, int len, BOOL *pbln_syn)
{
    int    filesize = 0;        //��ȡ�ļ��Ĵ�С
    int tmpFlag = 0;        //�����ʾ

    *pbln_syn = FALSE;

    if(len <= 0)
    {
        return ERR_READWR;
    }

    //�ļ���С������д��
    filesize  = FGetFileSize(fd);
    if( filesize < offset + len)
    {
        *pbln_syn = TRUE;
        return 1;
    }

    char pRdBuf[len];                    //�����ȡ������
    memset(pRdBuf, 0x0, len);

    //�޷���ȡ ���� ��ȡ�����ݷ����仯������д��
    if( read(fd, pRdBuf, len) != len || memcmp(pRdBuf, pWrBuf, len) != 0)
    {
        *pbln_syn = TRUE;
        tmpFlag = 1;
    }
    else
    {
        tmpFlag = 0;
    }
    lseek(fd, offset, SEEK_SET);        //�α��ƻ�ȥ

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

//��ʼ���ļ�ͷ
inline static void InitFileHeader(xlFileHeader *header)
{
    if(NULL == header) return;

    memset(header, 0, sizeof(xlFileHeader));

    header->fh_signature[0] = 'X';
    header->fh_signature[1] = 'L';
    header->fh_version      = 10*MAJOR + MINOR;
    header->fh_projID       = PROJECTID;
    header->fh_len          = sizeof(xlFileHeader);
    //�ļ������в������ƫ����Ϣ
    memset(header->OffBase, 0xFF, sizeof(header->OffBase));
}

//���ò������ļ�״̬
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
        {// �ٵ�һ��
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
��  ��: fillBlankChars
��  ��: ��fdָ����ļ�����д����ַ�
��  ��:
        fd     - ��д�ļ����ļ�������
        offset - ������ļ����ֽڵ��ֽ�ƫ����
        len    - �������
����ֵ:
        (��)
*/
inline static void fillBlankChars(int fd, int offset, int len)
{
    int   iCnt, iFrag, i;
    uint8 buff[32];

    memset(buff, ERASE_CHAR, 32);
    iCnt  = (int)(len / 32);
    iFrag = len % 32;

    writew_lock(fd, offset, len); //Lock

    lseek(fd, offset, SEEK_SET); /* ƫ������λ */
    if( iFrag) write(fd, buff, iFrag);
    for( i = 0; i < iCnt; i++)
        write(fd, buff, 32);

    un_lock(fd, offset, len); //Unlock
}

/**
*********************************************************************
* @brief��     ���ļ�ϵͳ��������ע��һ��˽���������ļ���������һ��˽���������ļ�.
* @param[in]�� filename - ע���˽���������ļ�����
*              appName - ע��app����
* @param[out]��NULL
* @return��    <0       - ����
               =0       - �ɹ�
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

    /* ���dataĿ¼�����ڣ�Ĭ�ϴ���һ�� */
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
        return 0; //�ļ�����,ֱ�ӷ���0(ERR_OK),��ʾ�ɹ�

    InitFileHeader(&fheader);
    fheader.fh_typeID = FILE_PRIVATE;

    pdata_fd = open(pdataPath, O_CREAT|O_TRUNC|O_WRONLY, 0774);
    if( write(pdata_fd, &fheader, sizeof(xlFileHeader_min)) < 0)  //д�ļ�ͷ
        return ERR_MEM;  //����д���ж�Ϊ���̿ռ䲻��
    fsync(pdata_fd);
    close(pdata_fd);

    return ERR_OK;
}


/**
*********************************************************************
* @brief��     ��˽���������ļ�
* @param[in]�� filename  - �ļ���
               offset    - ������������ݵ�ƫ����
               len       - ��ȡ����
               buf       - ���صĻ�����
* @param[out]��NULL
* @return��    <0        - ����
               =0        - �ɹ�
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

    if(read(pdata_fd, &fheader, sizeof(xlFileHeader_min)) < 0)  //���ļ�ͷ
    {
        un_lock(pdata_fd, 0, sizeof(xlFileHeader_min));
        close(pdata_fd);
        return ERR_IO;
    }

    un_lock(pdata_fd, 0, sizeof(xlFileHeader_min));

    if( (size = FGetFileSize(pdata_fd)) < 0)  //�ļ����ȳ���
    {
        close(pdata_fd);
        return ERR_O_RANGE;
    }

    if( (size - sizeof(xlFileHeader_min)) < (offset + len)) //�Ƿ�����ָ��Խ��
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
* @brief��     д˽���������ļ�
* @param[in]�� filename  - �ļ���
               buf       - ���뻺����
               offset    - ������������ݵ�ƫ����
               len       - ���صĻ�����
* @param[out]��NULL
* @return��    <0        - ����
               =0        - �ɹ�
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

        /* ���˽���ļ������ڣ��ʹ���һ�� */
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
    {  /* �����ǰ���ڣ�����ֻд��ʽ��  */
        if( (fdPriv = open(pdataPath, WR_TYPE)) < 0)
        {      
            STORE_FMT_DEBUG("%s open: %s.\n", pdataPath, strerror(errno));
            return ERR_ACCESS;
        }
    }

    /*--- ���ȱʡ�ַ��� ---*/
    filesize = FGetFileSize(fdPriv);
    delta    = offset + sizeof(xlFileHeader_min) - filesize;
    if(delta > 0)
    {
        SetMtrFileState(TRUE);
        filesize = FGetFileSize(fdPriv);
        delta    = offset + sizeof(xlFileHeader_min) - filesize;
        if(delta > 0)
            fillBlankChars(fdPriv, filesize, delta); /* ��дȱʡ�ַ� */
        SetMtrFileState(FALSE);
    }

    /*--- ����д�� ---*/
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

    /* �ֶ��д��,��ֹһ����д��ʱ������ݶ�ʧ. */
    if(filename[0] == '#')        //д��ramdisk
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
        if(filename[0] == '#')        //д��ramdisk
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
* @brief��     ��������ļ��е��������ݣ�ֻ�����ļ�ͷ��
* @param[in]�� filename - ˽���������ļ���(��·��)
* @param[out]��NULL
* @return��    <0       - ����
               =0       - �ɹ�
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

    if( (fd = open(path, O_WRONLY)) < 0) //��˽���������ļ�
        return ERR_ACCESS;

    if( writew_lock(fd, 0, 0) < 0) //����
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
* @brief��     ɾ���������У�filenameָ����˽�����ļ�.filename��
               ��Ҫ���ļ�·��.
* @param[in]�� filename - ˽���������ļ���(��·��)
* @param[out]��NULL
* @return��    <0       - ����
               =0       - �ɹ�
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
* @brief��     eeprom˽����ע�ắ����
* @param[in]�� id   - ��ע���id���
               len  - ע���ֽڳ���
               base - ���ص�eeprom��ʼ��ַ
* @param[out]��NULL
* @return��    <0   - ����
               =0   - �ɹ�
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
    pdatachk_t  chk; /* �����eeprom��ÿ��ע������ʼ��ַ��3�ֽ�ע����Ϣ */
    char buf[128];
    int  cnt, frag, i;
    int  offset;

    sprintf(name, "%s%s/.epdatainfo", g_pcszRootPath, m_AppName);
    sprintf(path, "%s%s/", g_pcszRootPath, m_AppName);

    if( !FileExists(name)) /* epdata��Ϣ�ļ������ڣ�������Ϊ��δ������ע��ʹ�� */
    {
        if( !PathExists(path))
            makePathedDir(path);

        if( (fd = open(name, O_WRONLY | O_CREAT | O_TRUNC, 0660)) < 0)
            return ERR_IO;

        writew_lock(fd, 0, 0); /* �ļ����� */
        //bExist = FALSE;
        header.freeoffset = PDATABASE;
    }
    else
    {
        if( (fd = open(name, O_RDWR)) < 0)
            return ERR_IO;

        //bExist = TRUE;
        memset(&header, 0, sizeof(header));

        writew_lock(fd, 0, 0); /* �ļ����� */
        read(fd, &header, sizeof(header)); /* ��ע���ļ�ͷ */

        if( header.freeoffset < PDATABASE || header.freeoffset >= EEPROM_SIZE_TOTAL)
            header.freeoffset = PDATABASE;
    }

    off = sizeof(header) + id*sizeof(EpdataItemInfo); /* �ҵ���ע�����ƫ���� */
    memset(&itemInfo, 0, sizeof(EpdataItemInfo));
    lseek(fd, off, SEEK_SET);
    read(fd, &itemInfo, sizeof(EpdataItemInfo));

    /* 0xabcdΪ��������Ч�Ա�־�����itemInfo.ValidId = 0xabcd, ������Ϊ���������ַ���Ѿ�
    * ���������, ������Ϊ����������ַ��û�б�ʹ�á�
    */
    if( 0xabcd == itemInfo.ValidId)
    {
        writelog("/tmp/reg.txt", "id:%d, off:%x, len:%d", id, itemInfo.offbase, itemInfo.len);
        *base = itemInfo.offbase;
    }
    else
    {
        /* --��.epdatainfo�ļ����¼�һ��ע����-- */
        itemInfo.ValidId = 0xabcd; /* ��Ч�Ա�־ */
        itemInfo.len     = len & 0xffff;
        itemInfo.offbase = header.freeoffset;

        if( header.freeoffset + sizeof(pdatachk_t) + itemInfo.len > EEPROM_SIZE_TOTAL)
        {
            close(fd);
            return ERR_O_RANGE;
        }

        *base = itemInfo.offbase;

        /* д.pdatainfo�ļ��е�ע������ */
        lseek(fd, off, SEEK_SET);
        write(fd, &itemInfo, sizeof(EpdataItemInfo));

        /* дeeprom��ע����ͷ��Ϣ */
        chk.id    = id & 0xff;
        chk.size  = itemInfo.len;

        DPRINT("chk.id=%x, chk.size=%x\n", chk.id, chk.size);

        if( write_eeprom(header.freeoffset, sizeof(pdatachk_t), (const char *)&chk) < 0)
            return ERR_IFACE;

        /* ��ʼ��eeprom�ڴ�ռ䣬��0 */
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

        /* ˢ���ļ�ͷ  */
        header.freeoffset += sizeof(pdatachk_t) + len; /* eeprom˽��������ռ���ֽ�: ע����Ϣ(3�ֽ�) + len�ֽ�ע�᳤�� */
        lseek(fd, 0, SEEK_SET);
        write(fd, &header, sizeof(EpdataFileHeader));
    }
    fsync(fd);
    close(fd); /* �ļ��رղ����� */

    return 0;
}

/**
*********************************************************************
* @brief��     ��eeprom˽��������
* @param[in]�� id      - ע���õ�˽����ռ�id
               base    - ע��ռ����ַ
               offset  - ע��ռ����ֽ�ƫ��
               len 
* @param[out]��NULL
* @return��    <0      - ����
               >= 0    - �ɹ���ȡ����������ֽ���
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

    /* ��ע����Ϣ */
    if((ret = read_eeprom(base, sizeof(pdatachk_t), (char *)&chk, FALSE)) < 0) return ret;
    if(chk.id != id) return -ERR_NPERM;

    if(chk.size < (offset + len)) return -ERR_O_RANGE;

    /* ������ */
    retval = read_eeprom(base + PDATACHK_SIZE + offset, len, buf, FALSE);
    return retval;
}

/**
*********************************************************************
* @brief��     ��eeprom˽��������
* @param[in]�� id     - ע���id��
               base   - ע��ռ����ʼ��ַ
               offset - ע��ռ���ƫ����
               buf    - ������Դ������
               len    - д���ֽڳ���
* @param[out]��NULL
* @return��    <0     - ����
               >= 0   - д�볤��
*********************************************************************
*/
int write_pdata_eeprom(unsigned id, uint32 base, uint32 offset, const char *buf, unsigned len)
{
    pdatachk_t chk;

    if( !m_bInited) return -ERR_UNINITED;
    if(!buf) return -ERR_PTR;
    if( (len > REG_MAX_LEN) || (base > EEPROM_SIZE_TOTAL))
        return -ERR_O_RANGE;

    /* �ȶ�ע������Ϣ,�ж�д���ַ�Ƿ�Ϸ� */
    if( read_eeprom(base, sizeof(pdatachk_t), (char *)&chk, FALSE) < 0) return -ERR_IFACE;
    if( chk.id != (uint8)id) return -ERR_NPERM;
    if( chk.size < (offset + len)) return -ERR_O_RANGE;

    DPRINT(">>WritePDataEeprom: 0x%lx, %u\n", base + PDATACHK_SIZE + offset, len);

    /* д���� */
    return write_eeprom( base + PDATACHK_SIZE + offset, len, buf);
}

/**
*********************************************************************
* @brief��      ���Eeprom��˽����������������
* @param[in]��  base - ע�᷵�صĻ���ַ
                id   - ע�᷵�ص�id
* @param[out]�� NULL
* @return��     <0   - ����
                =0   - �ɹ�
*********************************************************************
*/
int clear_pdata_eeprom(uint32 base, int id)
{
    pdatachk_t chk;
    uint8      buf[64];
    int        cnt, frag, i;

    //���
    if( !m_bInited) return -ERR_UNINITED;

    if( base > EEPROM_SIZE_TOTAL)
        return -ERR_O_RANGE;

    if( read_eeprom(base, sizeof(pdatachk_t), (char *)&chk, FALSE) < 0) return -ERR_IFACE;
    if( chk.id != (uint8)id) return -ERR_NPERM;

    cnt  = (int)(chk.size/64);
    frag = chk.size%64;

    //��ע���eeprom˽���������������ֽ�д��ERASE_CHAR
    memset(buf, ERASE_CHAR, 64);
    write_eeprom( base + PDATACHK_SIZE, frag, (char *)buf);
    for( i = 0; i < cnt; i++)
        write_eeprom( base + PDATACHK_SIZE + frag + i*64, 64, (char *)buf);
    return ERR_OK;
}



//
// ��־����
//

/* 
    * ������filepathָ�����־�ļ�д��һ����־��fmt����
    * ��printf(fmt, ...)�е�fmtһ��֧��ת���ַ��Ϳɱ����.
    * ����0��ʾ�ɹ���������ʾʧ��.
    * ע��: ��Ҫ��fmt���'\n'���У�writelog���Զ�������.
*/

/**
*********************************************************************
* @brief       д��־
* @param[in]   filepath - �ļ�����
               fmt      - д�ļ�����
* @param[out]  
* @return      >= 0     - �ɹ���ȡ����������ֽ���
               < 0      - ʧ�� 
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
        if( (fd = open(filepath, O_WRONLY | O_CREAT | O_TRUNC)) < 0) //�½��ļ�
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
    if( (fd = open(filepath, O_CREAT |O_WRONLY | O_APPEND)) < 0) //�����ݸ��ӵ��ļ�ĩβ
        return -1;
    if( writew_lock(fd, 0, 0) < 0)
    {
      //perror("writew_lock");
        close(fd);
        return -1;
    }
    t = time(NULL);
    ptm = localtime(&t);
    /* д������ */
    sprintf(pathsub, "%4d-%02d-%02d %02d:%02d:%02d ", ptm->tm_year + 1900, ptm->tm_mon + 1, ptm->tm_mday, ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
    write(fd, pathsub, strlen(pathsub));  
    /* д����־�ַ��� */  
    va_start(ap, fmt);  
    vsnprintf(pathsub, sizeof(pathsub), fmt, ap); /* pathsub�ٴλ������ã���������д����ַ��� */
    va_end(ap);
    write(fd, pathsub, strlen(pathsub));

    /* ���� */
    write(fd, "\n", strlen("\n")); 
    un_lock(fd, 0, 0); //Unlock
    
    close(fd);
    return 0;
}

/**
*********************************************************************
* @brief       д��������־
* @param[in]   filepath    - �ļ�����
*              strbuf      - ע���ַ���
*              strbuf_len  - ע���ַ�������
*              databuf     - 16��������
*              databuf_len - 16���Ƴ���
* @param[out]  
* @return      >= 0        - �ɹ���ȡ����������ֽ���
               < 0         - ʧ�� 
*********************************************************************
*/
int writelog_bin(const char *filepath, const char *strbuf ,unsigned int strbuf_len ,const char *databuf,unsigned int databuf_len)
{
    int         fd, i, cnt, frag;
    char        path[PATH_MAX_LEN + 1];
    //char        pathsub[PATH_MAX_LEN + 1];
    BinLogHead    logHead;
    char        *bufOff = NULL;

    //��û���ַ���Ҳû�б���Ҫд�룬���ٴ���
    if(!(strbuf != NULL && strbuf_len > 0) && !(databuf != NULL && databuf_len > 0))
        return -1;

    if( !FileExists(filepath)) 
    {
        ExtractFilePath(filepath, path);
        if( !PathExists(path)) makePathedDir(path);
        if( (fd = open(filepath, O_WRONLY | O_CREAT | O_TRUNC)) < 0) //�½��ļ�
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

    //������Ϣͷ
    memset((void *)&logHead, 0x0, sizeof(logHead));
    logHead.allsize = sizeof(logHead);        //������Ϣͷ
    if(strbuf != NULL && strbuf_len > 0)    //д���ַ���
        logHead.allsize += strbuf_len;
    if(databuf != NULL && databuf_len > 0)    //д�뱨��
    {
        logHead.allsize += databuf_len;
        logHead.databuf_len = databuf_len;
    }
    logHead.time = time(NULL);

    char write_buf[logHead.allsize];        //д����־�ļ��Ļ�����
    int buf_pos = 0;                    //��¼ƫ��λ��
    //������Ϣͷ
    memset(write_buf, 0x0, sizeof(write_buf));
    memcpy(write_buf, (char *)&logHead, sizeof(logHead));
    buf_pos += sizeof(logHead);
    //д���ַ���
    if(strbuf != NULL && strbuf_len > 0)
    {
        memcpy(write_buf + buf_pos, strbuf, strbuf_len);
        buf_pos += strbuf_len;
    }
    //д�뱨��
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

    /* �ֶ��д��,��ֹһ����д��ʱ������ݶ�ʧ. */
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
* @brief       ѹ����־�ļ�
* @param[in]   file_path    - ��־�ļ�·��
               backup_count - ֧��ѹ�����ݵķ��� 
* @param[out]  
* @return      ������Ϣ    
*********************************************************************
*/
int tar_log_backup(const char *filepath, int backup_count)
{
    TarLogFile(filepath, backup_count);
    return 1;
}



//
// EEPROM����
//

/**
*********************************************************************
* @brief��     ��EEPROM����
* @param[in]�� buf    - ������ɹ���ȡ��ķ��ص�ַ������
               offset - ƫ��
               len    - ���ݳ���
* @param[out]��NULL
* @return��    >= 0   - �ɹ���ȡ����������ֽ���
               < 0    - ʧ��
*********************************************************************
*/
int read_eeprom(uint32 offset, uint32 len, char *buf, BOOL bRaw)
{
    char  path[PATH_MAX_LEN];
    int   fd;

    if( !buf) return ERR_PTR;
    if( (len > REG_MAX_LEN) || ((offset + len) > EEPROM_SIZE_TOTAL) ) return ERR_O_RANGE;
    if( bRaw) goto READ_EPRM;

    /* ��eeprom��ramdisk�����ļ��ж�ȡ���� */
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
    /* �ļ�ģ�� */
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
* @brief��     ��EEPROM����
* @param[in]�� buf    - ������ɹ���ȡ��ķ��ص�ַ������
               offset - ƫ��
               len    - ���ݳ���
* @param[out]��NULL
* @return��    >= 0   - �ɹ���ȡ����������ֽ���
               < 0    - ʧ��
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
    /* �ļ�ģ�� */
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
        * ����ǵ�һ�δ����ļ�,Ĭ�ϳ������ó�EEPROM_SIZE_TOTAL, 
        * д��Ĭ������ַ�.
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
    close(fd); /* �������ر��ļ� */ 

    return 0;
}

/**
*********************************************************************
* @brief��     дEEPROM����
* @param[in]�� buf    - ���ݻ���
               offset - ƫ��
               len    - ���ݳ���
* @param[out]��NULL
* @return��    >= 0    - �ɹ�д�����������ֽ���
               < 0     - ʧ��
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

    #if defined(ARM) && (EEPROM_SIM_EN == 0) /* д��ʵ�ʵ�eeprom */ 
    if( WriteDataToEEPROM(0, (char *)buf, offset, len) < 0)
        return -1;
    #else /* undefined ARM or simulate eeprom device */
    /* ������д��nandflash�е�eepromģ���ļ� */
    sprintf(path, "%s%s/eeprom.dat", g_pcszRootPath, m_AppName);
    retval = write_to_eepromfile(path, offset, buf, len);  
    #endif

    return retval;
}

/**
*********************************************************************
* @brief��     дEEPROM����
* @param[in]�� buf    - ���ݻ���
               offset - ƫ��
               len    - ���ݳ���
* @param[out]��NULL
* @return��    >= 0    - �ɹ�д�����������ֽ���
               < 0     - ʧ��
*********************************************************************
*/
int write_to_eeprom(const char *buf, uint32 offset, uint32 len)
{
    int retval = 0;
    DPRINT("write_eeprom: offset=0x%x, len=%d\n", offset, len);
    
    if( !buf) return ERR_PTR;
    if( (len > REG_MAX_LEN) || ((offset + len) > EEPROM_SIZE_TOTAL)) return ERR_O_RANGE;

#if defined(ARM) && (EEPROM_SIM_EN == 0) /* д��ʵ�ʵ�eeprom */ 
    if( WriteDataToEEPROM(0, (char *)buf, offset, len) != len)
        return -1;
#else /* undefined ARM or simulate eeprom device */
    /* ������д��nandflash�е�eepromģ���ļ� */
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


