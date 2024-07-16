/*
*********************************************************************
* @file    class18.c
* @brief： class18处理
* @author：
* @date：   2019.12.06
* @copyright 2019 南京新联电子股份有限公司 All rights reserved.
*********************************************************************
*/
#include "baseCommon.h"
#include "oadInfoList.h"
#include "oopservice.h"
#include "commClass.h"
#include "storage.h"
#include "msg_id.h"
#include "fileOp.h"
#include "md5sum.h"
#include "sm3sum.h"
#include "oopSecurity.h"

#define     TAR_FILE_MAX    (8 * 1024 * 1024)           //文件包的最大大小
#define     FRAMELEN        1024                        //单帧文件包的长度
#define     MASK_INDEX_F001(a)  (a / 8)
#define     MASK_BIT_F001(a)    (a % 8)
#define     FILE_NUM_MAX        128
#ifdef ADV_PU_AMR
#define     UPDATNEW_PATH       "/data/app/puAmr"   
#define     UPDATNEW_PACK       "/data/app/puAmr/updatetmp.tar.gz"  
#define     UPDATNEW_PACK_NEW   "/data/app/puAmr/upgrade.tar.gz"   //文件包文件
#define     UPDATNEW_NAME       "upgrade.tar.gz"
#else
#ifdef ADV_ST_AMR
#define     UPDATNEW_PATH       "/data/app/stAmr"   
#define     UPDATNEW_PACK       "/data/app/stAmr/updatetmp.tar.gz"  
#define     UPDATNEW_PACK_NEW   "/data/app/stAmr/upgrade.tar.gz"   //文件包文件
#define     UPDATNEW_NAME       "upgrade.tar.gz"
#endif
#endif

#define     UPDATNEW_FILE_698   "update_file_698"                                                       //文件传输使用的私有文件，主要用来存放各种文件传输中间信息
#define     UPDATNEW_MASK_698   "update_mask_698"                                                       //用来标识文件传输包各个数据块的接收情况
#define     TRANSFILE_PARAM     "transfile_param"
#define     TRANSFILE_LIST      "transfile_list"
#define     TRANSFILE_PARAM_COUNT   16

typedef struct
{
    char    file_name[256];
    uint32  file_size;              //文件大小
    uint32  file_Pack_size;         //主站下发每一帧大小
    uint8*  pfileVersion;           //文件版本信息
    uint8   checkType;              //校验类型
    uint8   crcValue[16];           //校验值
    uint16  file_Pack_num;          //总块数
    uint16  file_succ_Pack_num;     //成功块数
    uint16  mask_len;               //接收掩码的长度 表示有多少个字节
    uint8   *pPacksMask;            //接收报文的掩码信息
    time_t  update_endtime;         //文件传输结束时间
    uint8   shown_gui;              //显示页面是否弹出
    uint8   update_state;           //终端文件传输状态
    uint8   file_type;
    char    srcfilename[256];     //源文件
    uint32  lastend;                //最后写入的位置
    uint8   lastadd;                //最后补发
}UpdateInfoF001;        //F001文件传输信息

UpdateInfoF001 gUpInfoD_F001;//文件传输信息全局变量

enum TransFilePara_data
{
    SRC_FILE_NAME_LEN=0,     // 1字节,源文件名字节串的长度
    SRC_FILE_FULLNAME,       // 50字节,源文件名(路径+文件名)
    DES_FILE_NAME_LEN,       // 1字节,目标文件名字节串的长度
    DES_FILE_FULLNAME,       // 50字节,目标文件名(路径+文件名)
    TRANS_FILE_LENGTH,       // 4字节,文件大小(字节)

    TRANS_FILE_ATTRIBUTES,   // 1字节,文件属性 bit0:（1:可读,0:不可读）bit1:（1:可写,0:不可写）bit2:（1:可执行,0:不可执行）
    TRANS_FILE_VERSION_LEN,  // 1字节,文件版本字节串的长度
    TRANS_FILE_VERSION,      // 30字节,文件版本
    TRANS_RESULT_1,          // 1字节,传输命令结果
    PROTO_SERVER_IP_ADDR,    // 4字节,网络传输服务器IP地址

    PROTO_SERVER_PORT,       // 2字节,网络传输服务器端口号
    PROTO_USERNAME_LEN,      // 1字节,网络传输用户名字节串的长度
    PROTO_USERNAME,          // 16字节,网络传输用户名
    PROTO_PASSWORD_LEN,      // 1字节,网络传输密码字节串的长度
    PROTO_PASSWORD,          // 16字节,网络传输传输密码

    ORDER_RESULT,            // 1字节,属性3 命令结果

};

const uint16 TransFileParaOffset[TRANSFILE_PARAM_COUNT]=
{
    0,
    1,
    51,
    52,
    102,

    106,
    107,
    108,
    138,
    139,

    143,
    145,
    146,
    162,
    163,

    179,
};

const uint16 TransFileParaLen[TRANSFILE_PARAM_COUNT]=
{
    1,
    50,
    1,
    50,
    4,

    1,
    1,
    30,
    1,
    4,

    2,
    1,
    16,
    1,
    16,

    1
};

// 用于记录终端文件的信息
typedef struct tag_OOP_FILE_LIST
{
    uint32         nNum;
    UpdateInfoF001 file[FILE_NUM_MAX];
}OOP_FILE_LIST_T;

OOP_FILE_LIST_T gFileList;

/**
*********************************************************************
* @name：      ReadUpdateFile
* @brief：     读取传输文件的私有信息
* @param[in] ：filename       文件名称
               offset         偏移
               len            报文长度

* @param[out]：buf            内容
* @return    ：错误码
*********************************************************************
*/
int ReadUpdateFile(const char *filename, uint8 *buf, unsigned offset, unsigned len)
{
    uint8 bCS = 0x33;
    uint8 readbuf[len + 1];
    int bRet = 0;
    int i;

    if(len == 0)
        return -1;
    memset(readbuf, 0xff, len + 1);
    bRet = read_pdata_xram(filename, (char*)readbuf, offset, len + 1);
    if(bRet < 0)
        return -1;
    
    for(i = 0; i < len; i++)
    {
        bCS += readbuf[i];
    }
    if(bCS != readbuf[len])
        return -1;

    memcpy(buf, readbuf, len);
    return len;
}


/**
*********************************************************************
* @name：      WriteUpdateFile
* @brief：     写入传输文件的私有信息
* @param[in] ：filename       文件名称
               offset         偏移
               len            报文长度
               buf            内容
* @param[out]：
* @return    ：错误码
*********************************************************************
*/
int WriteUpdateFile(const char *filename, uint8 *buf, unsigned offset, unsigned len)
{
    uint8 bCS = 0x33;
    uint8 writebuf[len + 1];
    int bRet = 0;
    int i;

    if(len == 0)
        return -1;
    memcpy(writebuf, buf, len);
    for(i = 0; i < len; i++)
        bCS += writebuf[i];
    writebuf[len] = bCS;
    bRet = write_pdata_xram(filename, (char*)writebuf, offset, len + 1);
    if(bRet < 0)
        return -1;
    return len;
}

/**
*********************************************************************
* @name：      ReadFileList
* @brief：     读取传输文件列表信息
* @param[in] ：
* @param[out]：fileList       文件列表信息
* @return    ：错误码
*********************************************************************
*/
int ReadFileList(OOP_FILE_LIST_T *fileList)
{
    int ret = 0;
    OOP_FILE_LIST_T fileListTmp;

    MEMZERO(&fileListTmp, sizeof(OOP_FILE_LIST_T));

    ret = ReadUpdateFile(TRANSFILE_LIST, (uint8 *)&fileListTmp, 0, sizeof(OOP_FILE_LIST_T));
    if (ret != sizeof(OOP_FILE_LIST_T))
    {
        ret = -1;
    }
    else
    {
        *fileList = fileListTmp;
    }

    return ret;
}

/**
*********************************************************************
* @name：      AddFileList
* @brief：     添加文件列表信息
* @param[in] ：
* @param[out]：fileList       文件列表信息
* @return    ：错误码
*********************************************************************
*/
int AddFileList(UpdateInfoF001 file)
{
    int i = 0;
    int ret = 0;
    OOP_FILE_LIST_T fileListTmp;

    MEMZERO(&fileListTmp, sizeof(OOP_FILE_LIST_T));

    ret = ReadUpdateFile(TRANSFILE_LIST, (uint8 *)&fileListTmp, 0, sizeof(OOP_FILE_LIST_T));
    if (ret != sizeof(OOP_FILE_LIST_T))
    {
        PRTL_FMT_DEBUG("the fileList null. ret[%d]\n", ret);
        MEMZERO(&fileListTmp, sizeof(OOP_FILE_LIST_T));
    }

    for (i = 0; i < FILE_NUM_MAX; i++)
    {
        if (strcmp(fileListTmp.file[i].file_name, (const char *)file.file_name) == 0)
        {
            PRTL_FMT_DEBUG("the same file exists. file[%s] and i[%d]\n", file.file_name, i);
            break;
        }
    }

    if (i == FILE_NUM_MAX)
    {
        if (fileListTmp.nNum < FILE_NUM_MAX)
        {
            fileListTmp.file[fileListTmp.nNum] = file;
            fileListTmp.nNum++;
        }
        else
        {
            PRTL_FMT_DEBUG("update file full num[%d]\n", fileListTmp.nNum);
            fileListTmp.nNum = FILE_NUM_MAX;
            fileListTmp.file[fileListTmp.nNum - 1] = file;
        }
    }
    else
    {
        fileListTmp.file[i] = file;
    }
    
    ret = WriteUpdateFile(TRANSFILE_LIST, (uint8 *)&fileListTmp, 0, sizeof(OOP_FILE_LIST_T));
    if (ret < 0)
    {
        return -2;
    }

    return ret;
}

/**
*********************************************************************
* @name：      DelFileList
* @brief：     删除文件列表信息
* @param[in] ：
* @param[out]：fileList       文件列表信息
* @return    ：错误码
*********************************************************************
*/
int DelFileList(UpdateInfoF001 file)
{
    int  i = 0;
    int  ret = 0;
    OOP_FILE_LIST_T fileListTmp;
    char cmd[512] = {0};

    MEMZERO(&fileListTmp, sizeof(OOP_FILE_LIST_T));

    //删除文件传输包软件，终端升级包由升级脚本自己删除
    system("rm -fr "UPDATNEW_PACK);
    if (strcmp(gUpInfoD_F001.file_name, UPDATNEW_NAME) != 0)
    {
        sprintf(cmd, "rm -fr %s/%s", UPDATNEW_PATH, gUpInfoD_F001.file_name);
        system(cmd);
    }   

    ret = ReadUpdateFile(TRANSFILE_LIST, (uint8 *)&fileListTmp, 0, sizeof(OOP_FILE_LIST_T));
    if (ret != sizeof(OOP_FILE_LIST_T))
    {
        return -1;
    }

    for (i = 0; i < FILE_NUM_MAX; i++)
    {
        if (strcmp(fileListTmp.file[i].file_name, (const char *)file.file_name) == 0)
        {
            PRTL_FMT_DEBUG("the same file exists. file[%s] and i[%d]\n", file.file_name, i);
            if (fileListTmp.nNum > 0)
            {
                fileListTmp.nNum --;
                memset(&fileListTmp.file[i], 0, sizeof(UpdateInfoF001));
            }
            break;
        }
    }

    if (i == FILE_NUM_MAX)
    {
        PRTL_FMT_DEBUG("the update file non-exists. file[%s] and i[%d]\n", file.file_name, i);
    }
    
    ret = WriteUpdateFile(TRANSFILE_LIST, (uint8 *)&fileListTmp, 0, sizeof(OOP_FILE_LIST_T));
    if (ret < 0)
    {
        return -2;
    }

    return ret;
}

/**
*********************************************************************
* @name：      UpdateStopF001
* @brief：     结束文件传输
* @param[in] ：
* @param[out]：
* @return    ：错误码
*********************************************************************
*/
int UpdateStopF001()
{
    gUpInfoD_F001.update_state = FALSE;
    if(gUpInfoD_F001.pPacksMask != NULL)
    {
        free(gUpInfoD_F001.pPacksMask);
        gUpInfoD_F001.pPacksMask = NULL;
    }
    memset((void *)&gUpInfoD_F001, 0x0, sizeof(UpdateInfoF001));              //当前文件传输状态
    system("rm -fr "UPDATNEW_PACK);     //删除文件传输包软件
    return 1;
}

/**
*********************************************************************
* @name：      clearUpdateFile
* @brief：     
* @param[in] ：
* @param[out]：
* @return    ：错误码
*********************************************************************
*/
void clearUpdateFile(void)
{
    if(gUpInfoD_F001.pPacksMask != NULL)
    {
        free(gUpInfoD_F001.pPacksMask);
        gUpInfoD_F001.pPacksMask = NULL;
    }
    system("rm -fr "UPDATNEW_PACK);     //删除文件传输包软件
}

/**
*********************************************************************
* @name：      WriteUpdateTar
* @brief：     写入传输文件
* @param[in] ：filename       文件名称
               offset         偏移
               len            报文长度
               buf            内容
* @param[out]：
* @return    ：错误码
*********************************************************************
*/
int WriteUpdateTar(char *buf, int offset, int len)
{
    int bRet = 0;
    int fd = 0;  //注意设置文件的权限

    if(0 == offset)
    {
        char cmd[256] = {0};
        sprintf(cmd, "rm %s", UPDATNEW_PACK);
        system(cmd);
    }

    fd = open(UPDATNEW_PACK, O_WRONLY | O_CREAT, S_IRWXU);  //注意设置文件的权限
    if(fd < 0)
    {
        PRTL_FMT_DEBUG("openning the update file failed!\n");
        bRet = -1;
    }
    else if(lseek(fd, offset, SEEK_SET) < 0)
    {
        printf("lseek offset error!\n");
        bRet = -1;
    }
    else if(write(fd, buf, len) != len)
    {
        bRet = -1;
    }
    else
        bRet = len;
    close(fd);
    return bRet;
}

/**
*********************************************************************
* @name：      WriteUpdateTarREV
* @brief：     写入传输文件
* @param[in] ：filename       文件名称
               offset         偏移
               len            报文长度
               buf            内容
* @param[out]：
* @return    ：错误码
*********************************************************************
*/
int WriteUpdateTarREV(char *buf, int offset, int len)
{
    int bRet = 0;
    int fd = open(UPDATNEW_PACK, O_WRONLY | O_CREAT, S_IRWXU);  //注意设置文件的权限
    char exbuf[len];

    int i;
    for(i = 0; i < len; i++)
        exbuf[len - i - 1] = buf[i];        

    if(fd < 0)
    {
        PRTL_FMT_DEBUG("openning the update file failed!\n");
        bRet = -1;
    }
    else if(lseek(fd, offset, SEEK_SET) < 0)
    {
        printf("lseek offset error!\n");
        bRet = -1;
    }
    else if(write(fd, exbuf, len) != len)
    {
        bRet = -1;
    }
    else
        bRet = len;
    close(fd);
    return bRet;
}

/**
*********************************************************************
* @name：      ReadUpdateTar
* @brief：     读取传输文件
* @param[in] ：filename       文件名称
               offset         偏移
               len            报文长度

* @param[out]：buf            内容
* @return    ：错误码
*********************************************************************
*/
int ReadUpdateTar(const char *filename, char *buf, int offset, int len)
{
    int  bRetlen = 0;
    int  fd = 0;  //注意设置文件的权限
    
    fd = open(filename, O_RDONLY);  //注意设置文件的权限
    if (fd < 0)
    {
        printf("fd error!\n");
        return -111;
    }
    
    if(lseek(fd, offset, SEEK_SET) < 0)
    {
        printf("lseek offset error!\n");
        close(fd);
        return -112;
    }

    bRetlen = read(fd, buf, len);
    if(bRetlen <= 0)
    {
        close(fd);
        return bRetlen;
    }

    close(fd);
    return bRetlen;
}

/**
*********************************************************************
* @name：      Class18_F000Get
* @brief：     class18的ACTION服务
* @param[in] ：pOdaInfo       OAD信息
               pBpInfo        应用分帧断点信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
               outDataMax     输出报文缓冲区最大长度

* @param[out]：outData     输出报文
* @return    ：OOP_DAR_E
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E Class18_F000Get(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)  
{
    return(DATA_READ_WRITE_DENIED);//拒绝
}

/**
*********************************************************************
* @name：      Class18_F001Get
* @brief：     class18的ACTION服务
* @param[in] ：pOdaInfo       OAD信息
               pBpInfo        应用分帧断点信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
               outDataMax     输出报文缓冲区最大长度

* @param[out]：outData     输出报文
* @return    ：OOP_DAR_E
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E Class18_F001Get(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)  
{
    uint16 offset=0;
    UpdateInfoF001 getTmpUpInfoD_F001;
    uint8 byte_offset;
    uint16 reallen=0;

    switch(pOadInfo->pTab->oad.attID)
    {
        case 4:
        {
            PRTL_FMT_DEBUG("Class18_F001Get\n");
            if(ReadUpdateFile(UPDATNEW_FILE_698, (uint8 *)&getTmpUpInfoD_F001,0,sizeof(getTmpUpInfoD_F001)) < 0)//读取文件传输信息
            {
                return DATA_TEMPORARY_FAILURE;
            }
            PRTL_FMT_DEBUG("Class18_F001Get getTmpUpInfoD_F001.mask_len is %d\n",getTmpUpInfoD_F001.mask_len);
            if(getTmpUpInfoD_F001.mask_len == 0)
            {
                return DATA_TYPE_UNMATCHED;
            }
            getTmpUpInfoD_F001.pPacksMask = (uint8*)malloc(getTmpUpInfoD_F001.mask_len);
            ReadUpdateFile(UPDATNEW_MASK_698, getTmpUpInfoD_F001.pPacksMask, 0, getTmpUpInfoD_F001.mask_len);       //接收掩码

            PRTL_BUF_DEBUG(getTmpUpInfoD_F001.pPacksMask, getTmpUpInfoD_F001.mask_len, "文件传输标志位");
            memcpy(outData->pResultData, inData, sizeof(OOP_OAD_U));
            offset += sizeof(OOP_OAD_U);
            outData->pResultData[offset ++] = 0x01;
            outData->pResultData[offset ++] = DT_BIT_STRING;

            byte_offset=set_len_offset(getTmpUpInfoD_F001.file_Pack_num, outData->pResultData+offset);
            PRTL_FMT_DEBUG("byte_offset is %d\n",byte_offset);

            offset=offset+byte_offset;

            if(getTmpUpInfoD_F001.file_Pack_num % 8 == 0)
            {
                reallen = getTmpUpInfoD_F001.file_Pack_num / 8;    //接收掩码的长度
            }
            else
            {
                reallen = getTmpUpInfoD_F001.file_Pack_num / 8 + 1;//接收掩码的长度
            }

            memcpy(&outData->pResultData[offset], getTmpUpInfoD_F001.pPacksMask, reallen);
            offset += reallen;
            outData->resultDataLen = offset;
            PRTL_BUF_DEBUG(outData->pResultData, offset, "APDU");
            free(getTmpUpInfoD_F001.pPacksMask);
            break;
        }
        break;
    }

    return DATA_SUCCESS;
}

/**
*********************************************************************
* @brief：     data_to_buf_ftp_msg
* @param[in]： ppOadInfo     - 数据表指针
               srcBuf       - 源数据
               srcLen       - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen       - 目的数据长度
               sOffset      - 转换后源数据区的偏移
               dOffset      - 转换后目的数据区的偏移
* @return：     =0           - 成功
              !=0           - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_ftp_msg(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{   
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_FILE_T data;

    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    memcpy(&data, &srcBuf[sOffset], sizeof(OOP_FILE_T));
    sOffset += sizeof(data);
    
    dstBuf[dOffset++] = DT_STRUCTURE;
    dstBuf[dOffset++] = 6;
    //源文件
    dstBuf[dOffset++] = DT_VISIBLE_STRING;
    dstBuf[dOffset++] = data.srceFile.nNum;
    memcpy(dstBuf+dOffset,data.srceFile.value,data.srceFile.nNum);
    dOffset=dOffset+data.srceFile.nNum;
    //目标文件
    dstBuf[dOffset++] = DT_VISIBLE_STRING;
    dstBuf[dOffset++] = data.destFile.nNum;
    memcpy(dstBuf+dOffset,data.destFile.value,data.destFile.nNum);
    dOffset=dOffset+data.destFile.nNum;
    //文件大小
    dar =basic_data_to_buf(E_DOUBLE_LONG_UNSIGNED, dstBuf, dstLen, &dOffset, &data.nSize, 4);
    if(dar != DATA_SUCCESS)
    {
        
        return DATA_TYPE_UNMATCHED;
    }  
    //文件属性
    dstBuf[dOffset++] = DT_BIT_STRING;
    dstBuf[dOffset++] = 0x03;
    dstBuf[dOffset++] = data.property.nValue;
    //文件版本
    dstBuf[dOffset++] = DT_VISIBLE_STRING;
    dstBuf[dOffset++] = data.version.nNum;
    memcpy(dstBuf+dOffset,data.version.value,data.version.nNum);
    dOffset=dOffset+data.version.nNum;
    //文件类别
    dar =basic_data_to_buf(E_ENUM, dstBuf, dstLen, &dOffset, &data.nSize, 1);
    if(dar != DATA_SUCCESS)
    {
        
        return DATA_TYPE_UNMATCHED;
    }  
  
            
    //避免每步都判断，放最后检查，但检查前可能已经越界
    if((sOffset > srcLen) || (dOffset > dstLen))
    {
        PRTL_FMT_DEBUG("cross the border. sOffset(%d), srcLen(%d), dOffset(%d), dstLen(%d)\n", sOffset, srcLen, dOffset, dstLen);
        return DATA_SCOPE_OF_ACCESS_VIOLATED;        
    }

    *convert->dOffset = dOffset;
    *convert->sOffset = sOffset;
     
    return dar;
}

/**
*********************************************************************
* @brief：     data_to_buf_ftp_msg
* @param[in]： ppOadInfo    - 数据表指针
               srcBuf       - 源数据
               srcLen       - 源数据长度
* @param[out]：dstBuf       - 目的数据
               dstLen       - 目的数据长度
               sOffset      - 转换后源数据区的偏移
               dOffset      - 转换后目的数据区的偏移
* @return：     =0          - 成功
              !=0           - 错误码
*********************************************************************
*/
OOP_DAR_E buf_to_data_ftp_msg(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{   
    OOP_FTPSERVER_T FTPSERVER_T;
    OOP_DAR_E dar = DATA_SUCCESS;
    uint16 dataLen;

    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_8103_CONFIG_T data;

    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    MEMZERO(&data, sizeof(data));
    

    memset((uint8*)&FTPSERVER_T,0x00,sizeof(OOP_FTPSERVER_T));
    
    if(srcBuf[sOffset++] !=DT_STRUCTURE)
    {
        return DATA_TYPE_UNMATCHED;
    }
    sOffset++;
    if(srcBuf[sOffset++] !=DT_OCTET_STRING)
    {
        return DATA_TYPE_UNMATCHED;
    }
    dataLen = srcBuf[sOffset++];
    if(dataLen ==0x81)
    {
        dataLen = srcBuf[sOffset++];
    }
    else if(dataLen == 0x82)
    {
        dataLen = srcBuf[sOffset++];
        dataLen <<= 8;
        dataLen += srcBuf[sOffset++];
    }
    else if(dataLen>0x82||dataLen==0x80)
    {
        return DATA_TYPE_UNMATCHED;
    }
    //ip
    memcpy(FTPSERVER_T.ip.data,srcBuf+sOffset,dataLen);
    //port
    sOffset=sOffset+dataLen;
    dar =basic_buf_to_data(E_LONG_UNSIGNED,srcBuf, srcLen, &sOffset,&FTPSERVER_T.port,2);
    if(srcBuf[sOffset++]!=DT_VISIBLE_STRING)
    {
        return DATA_TYPE_UNMATCHED;
    }
    FTPSERVER_T.user.nNum=srcBuf[sOffset++];
    memcpy(FTPSERVER_T.user.value,srcBuf+sOffset,FTPSERVER_T.user.nNum);
    sOffset=sOffset+FTPSERVER_T.user.nNum;
    //密码
    
    if(srcBuf[sOffset++]!=DT_VISIBLE_STRING)
    {
        return ERR_OPTION;
    }
    FTPSERVER_T.pwd.nNum=srcBuf[sOffset++];
    memcpy(FTPSERVER_T.pwd.value,srcBuf+sOffset,FTPSERVER_T.pwd.nNum);

    memcpy(&dstBuf[dOffset], &FTPSERVER_T, sizeof(OOP_FTPSERVER_T));
    dOffset += sizeof(OOP_FTPSERVER_T);

    //避免每步都判断，放最后检查，但检查前可能已经越界
    if((sOffset > srcLen) || (dOffset > dstLen))
    {
        PRTL_FMT_DEBUG("cross the border. sOffset(%d), srcLen(%d), dOffset(%d), dstLen(%d)\n", sOffset, srcLen, dOffset, dstLen);
        return DATA_SCOPE_OF_ACCESS_VIOLATED;        
    }

    *convert->dOffset = dOffset;
    *convert->sOffset = sOffset;

    return dar;
}


/**
*********************************************************************
* @brief：     data_to_buf_file_trans_msg
* @param[in]： ppOadInfo    - 数据表指针
               srcBuf       - 源数据
               srcLen       - 源数据长度
* @param[out]：dstBuf       - 目的数据
               dstLen       - 目的数据长度
               sOffset      - 转换后源数据区的偏移
               dOffset      - 转换后目的数据区的偏移
* @return：     =0          - 成功
              !=0           - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_file_trans_msg(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{   
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_FTPSERVER_T data;

    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    memcpy(&data, &srcBuf[sOffset], sizeof(data));
    sOffset += sizeof(data);
    
    dstBuf[dOffset++] = DT_STRUCTURE;
    dstBuf[dOffset++] = 4;
    //IP地址
    dstBuf[dOffset++] = DT_OCTET_STRING;
    dstBuf[dOffset++] = 0x04;
    memcpy(dstBuf+dOffset,data.ip.data,4);
    dOffset=dOffset+4;
    //端口
    dar =basic_data_to_buf(E_LONG_UNSIGNED, dstBuf, dstLen, &dOffset, &data.port, 2);
    if(dar != DATA_SUCCESS)
    {
        //PRTL_FMT_DEBUG("basic_data_to_buf failed. [%d] dar(%d),floatCoe(%d)\n", i, dar, data.item[i].floatCoe);
        return DATA_TYPE_UNMATCHED;
    }    
    //用户名1
    dar =basic_data_to_buf(E_VISIBLE_STRING, dstBuf, dstLen, &dOffset, &data.user, sizeof(OOP_VISIBLEVAR_T)); 
    if(dar != DATA_SUCCESS)
    {
        //PRTL_FMT_DEBUG("basic_data_to_buf failed. [%d] dar(%d),floatCoe(%d)\n", i, dar, data.item[i].floatCoe);
        return DATA_TYPE_UNMATCHED;
    }    
    //密码
    dar =basic_data_to_buf(E_VISIBLE_STRING, dstBuf, dstLen, &dOffset, &data.pwd, sizeof(OOP_VISIBLEVAR_T));
    if(dar != DATA_SUCCESS)
    {
        //PRTL_FMT_DEBUG("basic_data_to_buf failed. [%d] dar(%d),floatCoe(%d)\n", i, dar, data.item[i].floatCoe);
        return DATA_TYPE_UNMATCHED;
    }    
            
    //避免每步都判断，放最后检查，但检查前可能已经越界
    if((sOffset > srcLen) || (dOffset > dstLen))
    {
        //PRTL_FMT_DEBUG("cross the border. sOffset(%d), srcLen(%d), dOffset(%d), dstLen(%d)\n", sOffset, srcLen, dOffset, dstLen);
        return DATA_SCOPE_OF_ACCESS_VIOLATED;        
    }

    *convert->dOffset = dOffset;
    *convert->sOffset = sOffset;
     
    return dar;
}

/**
*********************************************************************
* @brief：     data_to_buf_file_trans_msg
* @param[in]： ppOadInfo    - 数据表指针
               srcBuf       - 源数据
               srcLen       - 源数据长度
* @param[out]：dstBuf       - 目的数据
               dstLen       - 目的数据长度
               sOffset      - 转换后源数据区的偏移
               dOffset      - 转换后目的数据区的偏移
* @return：     =0          - 成功
              !=0           - 错误码
*********************************************************************
*/
OOP_DAR_E buf_to_data_file_trans_msg(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{   
    OOP_FILE_T  file;
    OOP_DAR_E dar = DATA_SUCCESS;

    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    memset((uint8*)&file,0x00,sizeof(OOP_FILE_T));
    
    //结构类型
    if(srcBuf[sOffset++] != DT_STRUCTURE)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "check struct type failed. sOffset(%d)", sOffset);
        return DATA_TYPE_UNMATCHED;
    }
    
    //结构成员数
    if(srcBuf[sOffset++] != 6)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "check struct num failed. sOffset(%d)", sOffset);
        return DATA_TYPE_UNMATCHED;
    }
    
    //源文件
    dar = basic_buf_to_data(E_OOP_VISIBLEVAR_T, srcBuf, srcLen, &sOffset, &file.srceFile, sizeof(file.srceFile));
    if (dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }
    
    //目标文件
    dar = basic_buf_to_data(E_OOP_VISIBLEVAR_T, srcBuf, srcLen, &sOffset, &file.destFile, sizeof(file.destFile));
    if (dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //文件大小
    dar = basic_buf_to_data(E_DOUBLE_LONG_UNSIGNED, srcBuf, srcLen, &sOffset, &file.nSize, sizeof(file.nSize));
    if (dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }  

    //文件属性
    dar = basic_buf_to_data(E_OOP_BITSTR3_T, srcBuf, srcLen, &sOffset, &file.property, sizeof(file.property));
    if (dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    } 

    //文件版本
    dar = basic_buf_to_data(E_OOP_VISIBLEVAR_T, srcBuf, srcLen, &sOffset, &file.version, sizeof(file.version));
    if (dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //文件类别
    dar=basic_buf_to_data(E_ENUM, srcBuf, srcLen, &sOffset, &file.filetype, sizeof(file.filetype));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }  

    memcpy(&dstBuf[dOffset], &file, sizeof(OOP_FILE_T));
    dOffset += sizeof(OOP_FILE_T);

    //避免每步都判断，放最后检查，但检查前可能已经越界
    if ((sOffset > srcLen) || (dOffset > dstLen))
    {
        PRTL_FMT_DEBUG("cross the border. sOffset(%d), srcLen(%d), dOffset(%d), dstLen(%d)\n", sOffset, srcLen, dOffset, dstLen);
        return DATA_SCOPE_OF_ACCESS_VIOLATED;        
    }

    *convert->dOffset = dOffset;
    *convert->sOffset = sOffset;

    return dar;
}

/**
*********************************************************************
* @brief：     data_to_buf_file_trans_msg
* @param[in]： ppOadInfo    - 数据表指针
               srcBuf       - 源数据
               srcLen       - 源数据长度
* @param[out]：dstBuf       - 目的数据
               dstLen       - 目的数据长度
               sOffset      - 转换后源数据区的偏移
               dOffset      - 转换后目的数据区的偏移
* @return：     =0          - 成功
              !=0           - 错误码
*********************************************************************
*/
OOP_DAR_E buf_to_data_filecrc_trans_msg(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{   
    OOP_FILEVC_T  fileCrc;
    OOP_DAR_E dar = DATA_SUCCESS;
    
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    memset((uint8*)&fileCrc, 0x00, sizeof(OOP_FILEVC_T));
    
    //结构类型
    if (srcBuf[sOffset++] != DT_STRUCTURE)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "check struct type failed. sOffset(%d)", sOffset);
        return DATA_TYPE_UNMATCHED;
    }
    
    //结构成员数
    if (srcBuf[sOffset++] != 2)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "check struct num failed. sOffset(%d)", sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //类型
    dar = basic_buf_to_data(E_ENUM, srcBuf, srcLen, &sOffset, &fileCrc.type, sizeof(fileCrc.type));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //内容
    dar = basic_buf_to_data(E_OOP_OCTETVAR16_T, srcBuf, srcLen, &sOffset, &fileCrc.content, sizeof(fileCrc.content));
    if (dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }
    
    memcpy(&dstBuf[dOffset], &fileCrc, sizeof(fileCrc));
    dOffset += sizeof(fileCrc);

    //避免每步都判断，放最后检查，但检查前可能已经越界
    if((sOffset > srcLen) || (dOffset > dstLen))
    {
        //PRTL_FMT_DEBUG("cross the border. sOffset(%d), srcLen(%d), dOffset(%d), dstLen(%d)\n", sOffset, srcLen, dOffset, dstLen);
        return DATA_SCOPE_OF_ACCESS_VIOLATED;
    }

    *convert->dOffset = dOffset;
    *convert->sOffset = sOffset;

    return dar;
}

/**
*********************************************************************
* @brief：     data_to_buf_file_trans_msg
* @param[in]： ppOadInfo    - 数据表指针
               srcBuf       - 源数据
               srcLen       - 源数据长度
* @param[out]：dstBuf       - 目的数据
               dstLen       - 目的数据长度
               sOffset      - 转换后源数据区的偏移
               dOffset      - 转换后目的数据区的偏移
* @return：     =0          - 成功
              !=0           - 错误码
*********************************************************************
*/
OOP_DAR_E buf_to_data_version_msg(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{   
    OOP_COMPATVER_T ver;
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32    num = 0;
    uint32    i = 0;
    
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    memset((uint8*)&ver, 0x00, sizeof(ver));
    
    if (srcBuf[sOffset++] != DT_ARRAY)
    {
        return DATA_TYPE_UNMATCHED;
    }

    num = srcBuf[sOffset++];
    if (num == 0x81)
    {
        num = srcBuf[sOffset++];
    }
    else if (num == 0x82)
    {
        num = srcBuf[sOffset++];
        num <<= 8;
        num += srcBuf[sOffset++];
    }
    else if(num > 0x82 || num == 0x80)
    {
        return DATA_INVALID_BLOCKNO;
    }
    
    if (num > OOP_MAX_COMPATIBLE)
    {
        return DATA_SCOPE_OF_ACCESS_VIOLATED;
    }
    ver.nNum = num;

    for (i = 0; i < num; i++)
    {
        dar=basic_buf_to_data(E_OOP_VISIBLEVAR_T, srcBuf, srcLen, &sOffset, &ver.value[i], sizeof(OOP_VISIBLEVAR_T));
        if(dar != DATA_SUCCESS)
        {
            PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
            return DATA_TYPE_UNMATCHED;
        } 
    }

    memcpy(&dstBuf[dOffset], &ver, sizeof(ver));
    dOffset += sizeof(ver);

    //避免每步都判断，放最后检查，但检查前可能已经越界
    if((sOffset > srcLen) || (dOffset > dstLen))
    {
        //PRTL_FMT_DEBUG("cross the border. sOffset(%d), srcLen(%d), dOffset(%d), dstLen(%d)\n", sOffset, srcLen, dOffset, dstLen);
        return DATA_SCOPE_OF_ACCESS_VIOLATED;        
    }

    *convert->dOffset = dOffset;
    *convert->sOffset = sOffset;

    return dar;
}

/**
*********************************************************************
* @brief：     Class18_F002Get(class18)
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E Class18_F002Get(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)  
{

    int ret = ERR_OK;
    NOMAL_OAD_T normal = {0};

    DATA_CONVERT_T convert;
    uint32 sOffset = 0;
    uint32 offset  = 0;
    uint32 len = 0;
    OOP_DAR_E dar = DATA_SUCCESS;
    
    uint32 srcLen=0;
 
    switch(pOadInfo->pTab->oad.attID)
    {
        case 4:
        {
            srcLen = sizeof(OOP_FTPSERVER_T);
            uint8 buffer[srcLen];
            ret = db_read_nomal(pApduInfo->hUdp, &normal, srcLen, buffer, &len);
            if ((ret != ERR_OK) || (len == 0))
            {
                PRTL_FMT_DEBUG("db_read_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                    ret, normal.logicId, normal.infoNum, normal.oad.value);
                memcpy(outData->pResultData, inData, sizeof(OOP_OAD_U));
                offset += sizeof(OOP_OAD_U);
                outData->pResultData[offset ++] = 0x01;
                outData->pResultData[offset ++] = 0x00;
                outData->resultDataLen += offset;
                return DATA_SUCCESS;
            }
            //转换成报文数据
            convert.srcBuf = buffer;
            convert.srcLen = srcLen;
            convert.sOffset = &sOffset;

            convert.dstBuf = &outData->pResultData[5];
            convert.dstLen = outDataMax;
            convert.dOffset = &outData->resultDataLen;
        }
        break;
        default:
            return DATA_OBJECT_UNAVAILABLE;
        break;
    }
    /* 数据和报文转换 */
    dar=data_to_buf_ftp_msg(pOadInfo, &convert);
     if(dar != DATA_SUCCESS)
    {
        return dar;
    }

    /* 填写报文开头处的OAD和数据结果0x01*/
    memcpy(outData->pResultData, inData, sizeof(OOP_OAD_U));
    offset += sizeof(OOP_OAD_U);
    outData->pResultData[offset ++] = 0x01;
    outData->resultDataLen += offset;

    return dar;
}

/**
*********************************************************************
* @brief：     buf_to_data_class18_F000_get_update
* @param[in]： ppOadInfo    - 数据表指针
               srcBuf       - 源数据
               srcLen       - 源数据长度
* @param[out]：dstBuf       - 目的数据
               dstLen       - 目的数据长度
               sOffset      - 转换后源数据区的偏移
               dOffset      - 转换后目的数据区的偏移
* @return：     =0          - 成功
              !=0           - 错误码
*********************************************************************
*/
OOP_DAR_E buf_to_data_class18_F000_get_update(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint32 inLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    char   filepath[256] = {0};  //文件路径
    char   fileName[64] = {0};
    uint32 startPosition = 0;    //偏移位置
    uint32 readLen = 0;          //读取长度
    uint8 *dataInput = (uint8 *)inData + 4;

    uint32 offset = 0;
    uint16 filepathlen = 0;
    uint8  bytelen = 0;

    uint32 apduHeadlen = 0;
    uint32 apduMaxLen = 0;
    uint8 *databuf = NULL;
    int    ret = 0;
    uint32 datalen = 0;  //实际长度

    /* 1. 解析报文内容 */
    if(DT_STRUCTURE != dataInput[offset++])
    {
        return DATA_TYPE_UNMATCHED;
    }
    offset++;

    // 1.1 源文件
    if(DT_VISIBLE_STRING != dataInput[offset++])
    {
        return DATA_TYPE_UNMATCHED;
    }
    filepathlen = get_len_offset(dataInput + offset, &bytelen);
    offset += bytelen;
    memcpy(filepath, dataInput + offset, filepathlen);
    offset += filepathlen;

    // 1.2 偏移位置
    if(DT_DOUBLE_LONG_UNSIGNED != dataInput[offset++])
    {
        return DATA_TYPE_UNMATCHED;
    }
    memcpy_r(&startPosition, dataInput + offset, 4);
    offset += 4;

    // 1.3 读取长度
    if(DT_DOUBLE_LONG_UNSIGNED != dataInput[offset++])
    {
        return DATA_TYPE_UNMATCHED;
    }
    memcpy_r(&readLen, dataInput + offset, 4);
    offset += 4;

    if('/' == filepath[0] || '.' == filepath[0])
    {
        strcpy(fileName, filepath);
    }
    else
    {
        sprintf(fileName, "%s/%s", UPDATNEW_PATH, UPDATNEW_NAME);
    }
    PRTL_FMT_DEBUG("File get 0xF0007F00: filepath %s, filename %s start %d, len %d\n", filepath, fileName, startPosition, readLen);

    /* 2. 创建缓存内容 */
    //Response, Normal, PIID-ACD, OMD, DAR, OPTIONAL, type, len, 
    apduHeadlen = 1 + 1 + 1 + 4 + 1 + 1 + 1 + 5;
    //apduHeadlen, data, time...
    apduMaxLen = apduHeadlen + readLen + 2; 
    databuf = (uint8*)malloc(apduMaxLen + 2048);
    if(NULL == databuf)
    {
        PRTL_FMT_DEBUG("File get 0xF0007F00: malloc failed\n");
        return DATA_OTHER_REASON;
    }
    memset(databuf, 0, apduMaxLen);

    /* 3. 读取指定长度的内容 */
    ret = ReadUpdateTar((const char *)fileName, (char *)databuf + apduHeadlen, (int)startPosition, (int)readLen);
    PRTL_FMT_DEBUG("File get 0xF0007F00: read update file ret %d\n", ret);
    if(ret <= 0)
    {
        free(databuf);
        return DATA_READ_WRITE_DENIED;
    }
    datalen = (uint32)ret;
    PRTL_BUF_DEBUG(databuf + apduHeadlen, MIN(100, datalen), "read data len %d\n", datalen);
    PRTL_BUF_DEBUG(databuf + apduHeadlen + datalen - MIN(100, datalen), MIN(100, datalen), "last data\n");

    /* 4. 封装报文 */
    if ((datalen < pApduInfo->connectionInfo.sendFrameLen - 15) && (datalen < 2048))
    {
        // 4.1 正常长度 走正常流程
        uint8  *temp = outData->pResultData;
        uint8   bytelen = 0;

        offset = 0;
        memcpy_r(temp, &pOadInfo->pTab->oad.value, 4);
        offset += 4;
        temp[offset++] = 0x00;
        temp[offset++] = 0x01;
        temp[offset++] = DT_OCTET_STRING;

        bytelen = set_len_offset((uint16)datalen, temp + offset);
        offset += bytelen;

        memcpy(temp + offset, databuf + apduHeadlen, datalen);
        offset +=  datalen;
        outData->resultDataLen = offset;
    }
    else
    {
        // 4.2 扩展长度 先计算apdu允许长度
        FRAME_HEAD_T tframe;
        uint32 templen = 0;
        uint8 *pAddr = NULL;
        uint8  byte[5] = {0};
        uint32 newmsglen = 0;
        uint8 *pNewmsg = 0;
        uint32 apduleft = 0;

        memcpy(&tframe, pApduInfo->bpBreakInfo->frameaddr, sizeof(FRAME_HEAD_T));
        tframe.bExtmsg = TRUE;
        tframe.sAddr.len = app_get_localaddr(&pAddr);
        if(0 == tframe.sAddr.len)
        {
            free(databuf);
            return 0;
        }
        memcpy(tframe.sAddr.addr, pAddr, tframe.sAddr.len);
        tframe.prm = 1;
        tframe.dir = 1;

        tframe.headlen = APUD_COUNT_FRAME_LEN(tframe.sAddr.len, tframe.sAddr.logicAddr);

        /* 计算消息长度 */
        bytelen = set_biglen_offset(datalen, byte);
        templen = tframe.headlen + (apduHeadlen + datalen + 2 + bytelen - 5) + APP_698_CS_REGIN_SIZE;

        //这个地方有疑问，姚意的理解是不应该为了发整K而减少发送内容，但为了怕有其他影响，只在安全传输时去掉这个整K处理。
        #if LONG_ENCRYPTION_EN == 1
        if(pApduInfo->connectionInfo.securityServer == TRUE)
        {
            tframe.msglen = templen /*- templen % 1024*/;
        }
        else
        {
            tframe.msglen = templen - templen % 1024;
        }
        #else
        tframe.msglen = templen - templen % 1024;
        #endif
        PRTL_FMT_DEBUG("msglen check templen %d, headlen %d, msglen %d\n",templen, tframe.headlen, tframe.msglen);

        tframe.apduLen = tframe.msglen - tframe.headlen - APP_698_CS_REGIN_SIZE;

        /* 计算新的datalen 1024倍数大小情况下 bytelen大小不变 */
        bytelen = set_biglen_offset(tframe.apduLen - apduHeadlen, byte);
        datalen = tframe.apduLen - (apduHeadlen + 2 + bytelen - 5);

        /* 封装 APDU */
        offset = apduHeadlen;
        databuf[datalen + offset] = 0;
        databuf[datalen + offset + 1] = 0;

        bytelen = set_biglen_offset(datalen, byte);
        offset -= bytelen;
        memcpy(databuf + offset, byte, bytelen); //len

        databuf[--offset] = DT_OCTET_STRING;
        databuf[--offset] = 1;    //操作返回数据 OPTIONAL
        databuf[--offset] = 0x00; //操作执行结果
        offset -= 4;
        memcpy_r(databuf + offset, &pOadInfo->pTab->oad.value, 4); //OMD
        databuf[--offset] = pApduInfo->bpBreakInfo->piid;  //PIID
        databuf[--offset] = 1;  //ActionResponseNormal
        databuf[--offset] = SERVERID_ACTIONRESPONSE;  //ACTION-Response

        tframe.pApdu = databuf + offset;

        /* 是否安全加密 */
        #if LONG_ENCRYPTION_EN == 1
        if(pApduInfo->connectionInfo.securityServer == TRUE)
        {
            tframe.apduLen = GenSecurityApdu_long(tframe.pApdu, tframe.apduLen, pApduInfo->connectionInfo.curSecurityMode, 
                pApduInfo->connectionInfo.securityMode);

            APP698_BUF_DEBUG(tframe.pApdu, MIN(tframe.apduLen, 2048), "长报文开头-加密类型[%d]", 
                pApduInfo->connectionInfo.curSecurityMode);
            APP698_BUF_DEBUG(tframe.pApdu + tframe.apduLen - 2048, 2048, "长报文结尾-加密类型[%d]", 
                pApduInfo->connectionInfo.curSecurityMode);

            //重新计算长度域L 不足千字节的部分在FCS前面补0
            tframe.msglen = tframe.headlen + tframe.apduLen + 2;
            if (tframe.msglen%1024 != 0) 
            {
                memset(tframe.pApdu + tframe.apduLen, 0, 1024 - (tframe.msglen%1024));
                tframe.apduLen += 1024 - (tframe.msglen%1024);
                tframe.msglen = (tframe.msglen/1024 + 1)*1024;
            }
        }
        #endif

        /* 大报文 组装后 走直接发送流程 */
        newmsglen = app_frame_to_bigpackage(&tframe, &pNewmsg, tframe.msglen, &apduleft, FALSE);
        ret = send_bigpackage_master(pApduInfo->bpBreakInfo->channelid, pNewmsg, newmsglen);  
        PRTL_FMT_DEBUG("newmsglen is %d, apduleft %d, sendret %d, offset %d\n",newmsglen, apduleft, ret, offset);
        
        PRTL_BUF_DEBUG(pNewmsg, MIN(newmsglen, 2048), "长报文开头", 
            pApduInfo->connectionInfo.curSecurityMode);
        PRTL_BUF_DEBUG(pNewmsg + newmsglen - 2048, 2048, "长报文结尾", 
            pApduInfo->connectionInfo.curSecurityMode);
        if(0 != ret)
        {
            free(databuf);
            free(pNewmsg);
            memset(outData->pResultData, 0, 4);
            return DATA_OTHER_REASON;
        }
        /* 特殊约定前4个字节是ff, 不返回 */
        memset(outData->pResultData, 0xff, 4);
        outData->resultDataLen = 0;
        free(pNewmsg);
    }

    free(databuf);
    return DATA_SUCCESS;
}

/**
*********************************************************************
* @name：      Class18_F000Set
* @brief：     class19的SET服务
* @param[in] ：pOdaInfo        OAD信息
               pBpInfo        应用分帧断点信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
               outDataMax     输出报文缓冲区最大长度

* @param[out]：outData     输出报文
* @return    ：OOP_DAR_E
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E Class18_F002Set(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 inLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    NOMAL_OAD_T normal = {0};
    uint8 data[BUF_LEN_MAX] = {0};
    DATA_CONVERT_T convert;
    uint32 sOffset = 4;
    uint32 dOffset = 0;
    uint32 offset  = 0;

    MEMZERO(&data, sizeof(data)); 
    MEMZERO(&convert, sizeof(convert)); 

    convert.srcBuf = (uint8 *)inData;
    convert.srcLen = inLen;
    convert.sOffset = &sOffset;

    convert.dstBuf = &data;
    convert.dstLen = sizeof(data);
    convert.dOffset = &dOffset;  

    switch(pOadInfo->pTab->oad.attID)
    {
    case 4:
        {
            OOP_FTPSERVER_T FTPSERVER_T;
            dar =buf_to_data_ftp_msg(pOadInfo, &convert);
            memcpy(&FTPSERVER_T, convert.dstBuf, dOffset);
            normal.oad.value = pOadInfo->pTab->oad.value;
            normal.classtag = CLASS_DATA_INIT;
            ret = db_write_nomal(pApduInfo->hUdp, &normal, (uint8*)&FTPSERVER_T, sizeof(OOP_FTPSERVER_T));
            if(ret != ERR_OK)
            {
                PRTL_FMT_DEBUG("db_write_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                    ret, normal.logicId, normal.infoNum, normal.oad.value);
                return DATA_OTHER_REASON;
            }
            
        }
        break;
    default:
        return DATA_OBJECT_UNAVAILABLE;
        break;
    }
    memcpy(&outData->pResultData[offset], inData, sizeof(OOP_OAD_U));  //OMD
    offset += sizeof(OOP_OAD_U);
    outData->pResultData[offset ++] = 0x00;                            //DAR type
    outData->pResultData[offset ++] = 0x00;                            //DATA:NULL
    outData->resultDataLen = offset;

    return dar;
}

/**
*********************************************************************
* @name：      Class18_F000Set
* @brief：     class19的SET服务
* @param[in] ：pOdaInfo        OAD信息
               pBpInfo        应用分帧断点信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
               outDataMax     输出报文缓冲区最大长度

* @param[out]：outData     输出报文
* @return    ：OOP_DAR_E
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E Class18_CommonGet(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 inLen, RESULT_DATA_T *outData, uint16 outDataMax)
{   
    int ret = ERR_OK;
    NOMAL_OAD_T normal = {0};
    DATA_CONVERT_T convert;
    uint32 sOffset = 0;
    uint32 offset  = 0;
    uint32 len = 0;
    OOP_DAR_E dar = DATA_SUCCESS;
    
    uint32 srcLen=0;
    
    switch(pOadInfo->pTab->oad.attID)
    {
        case 2:
            {
                srcLen = sizeof(OOP_FILE_T);
                uint8 buffer[srcLen];
                ret = db_read_nomal(pApduInfo->hUdp, &normal, srcLen, buffer, &len);
                if ((ret != ERR_OK) || (len == 0))
                {
                    PRTL_FMT_DEBUG("db_read_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                        ret, normal.logicId, normal.infoNum, normal.oad.value);
                    memcpy(outData->pResultData, inData, sizeof(OOP_OAD_U));
                    offset += sizeof(OOP_OAD_U);
                    outData->pResultData[offset ++] = 0x01;
                    outData->pResultData[offset ++] = 0x00;
                    outData->resultDataLen += offset;
                    return DATA_SUCCESS;
                }
                //转换成报文数据
                convert.srcBuf = buffer;
                convert.srcLen = srcLen;
                convert.sOffset = &sOffset;

                convert.dstBuf = &outData->pResultData[5];
                convert.dstLen = outDataMax;
                convert.dOffset = &outData->resultDataLen;
                
            }
            break;
        case 3:
            {

            }
            break;
    }

    /* 数据和报文转换 */
    dar=data_to_buf_ftp_msg(pOadInfo, &convert);
     if(dar != DATA_SUCCESS)
    {
        return dar;
    }

    /* 填写报文开头处的OAD和数据结果0x01*/
    memcpy(outData->pResultData, inData, sizeof(OOP_OAD_U));
    offset += sizeof(OOP_OAD_U);
    outData->pResultData[offset ++] = 0x01;
    outData->resultDataLen += offset;

    return dar;
}

/**
*********************************************************************
* @name：      Class18_F000Set
* @brief：     class19的SET服务
* @param[in] ：pOdaInfo        OAD信息
               pBpInfo        应用分帧断点信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
               outDataMax     输出报文缓冲区最大长度

* @param[out]：outData     输出报文
* @return    ：OOP_DAR_E
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E Class18_CommonSet(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    NOMAL_OAD_T normal = {0};
    uint8 data[BUF_LEN_MAX] = {0};
    DATA_CONVERT_T convert;
    uint32 sOffset = 4;
    uint32 dOffset = 0;
    uint32 offset  = 0;

    MEMZERO(&data, sizeof(data)); 
    MEMZERO(&convert, sizeof(convert)); 

    convert.srcBuf = (uint8 *)inData;
    convert.srcLen = InDataLen;
    convert.sOffset = &sOffset;

    convert.dstBuf = &data;
    convert.dstLen = sizeof(data);
    convert.dOffset = &dOffset;
    
    switch(pOadInfo->pTab->oad.attID)
    {
        case 2:
            {
                OOP_FTPSERVER_T FTPSERVER_T;
                dar =buf_to_data_file_trans_msg(pOadInfo, &convert);
                memcpy(&FTPSERVER_T, convert.dstBuf, dOffset);
                normal.oad.value = pOadInfo->pTab->oad.value;
                normal.classtag = CLASS_DATA_INIT;
                ret = db_write_nomal(pApduInfo->hUdp, &normal, (uint8*)&FTPSERVER_T, sizeof(OOP_FTPSERVER_T));
                if(ret != ERR_OK)
                {
//                  PRTL_FMT_DEBUG("db_write_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
//                      ret, normal.logicId, normal.infoNum, normal.oad.value);
                    return DATA_OTHER_REASON;
                }
                
            }
            break;
        default:
            return(DATA_READ_WRITE_DENIED);//拒绝
            break;
    }
    memcpy(&outData->pResultData[offset], inData, sizeof(OOP_OAD_U));  //OMD
    offset += sizeof(OOP_OAD_U);
    outData->pResultData[offset ++] = 0x00;                            //DAR type
    outData->pResultData[offset ++] = 0x00;                            //DATA:NULL
    outData->resultDataLen = offset;

    return dar;
}



/**
*********************************************************************
* @name：      Class18_F000Set
* @brief：     class19的SET服务
* @param[in] ：pOdaInfo        OAD信息
               pBpInfo        应用分帧断点信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
               outDataMax     输出报文缓冲区最大长度

* @param[out]：outData     输出报文
* @return    ：OOP_DAR_E
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E Class18_F000Set(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 inLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    return(DATA_READ_WRITE_DENIED);//拒绝
}

/**
*********************************************************************
* @name：      Class18_F001Set
* @brief：     class19的SET服务
* @param[in] ：pOdaInfo        OAD信息
               pBpInfo        应用分帧断点信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
               outDataMax     输出报文缓冲区最大长度

* @param[out]：outData     输出报文
* @return    ：OOP_DAR_E
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E Class18_F001Set(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 inLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    return(DATA_READ_WRITE_DENIED);//拒绝
}




/**
*********************************************************************
* @name：      Class18_F000Action
* @brief：     class19的ACT服务
* @param[in] ：pOdaInfo        OAD信息
               pBpInfo        应用分帧断点信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
               outDataMax     输出报文缓冲区最大长度

* @param[out]：outData     输出报文
* @return    ：OOP_DAR_E
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E Class18_F000Action(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint32 inLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    switch(pOadInfo->pTab->oad.value)
    {
        case 0xF0007F00://获取文件内容
        {
            return buf_to_data_class18_F000_get_update( pOadInfo, pApduInfo, inData, inLen, outData, outDataMax);
        }
        break;
           
    }
    return DATA_OBJECT_UNAVAILABLE;
}

/**
*********************************************************************
* @brief：     class18_F001_start_update
* @param[in]： ppOadInfo    - 数据表指针
               srcBuf       - 源数据
               srcLen       - 源数据长度
* @param[out]：dstBuf       - 目的数据
               dstLen       - 目的数据长度
               sOffset      - 转换后源数据区的偏移
               dOffset      - 转换后目的数据区的偏移
* @return：     =0          - 成功
              !=0           - 错误码
*********************************************************************
*/
OOP_DAR_E class18_F001_start_update(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOPM_BLOCKTRANS_T blockTrans;
    int               ret = -1;
    UpdateInfoF001    tmpUpInfoD_F001_prtl;//规约下发的文件传输信息
    BOOL              continueUpdate = TRUE;

    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    uint8   offlen = 0;      //数组数量长度

    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    PRTL_FMT_DEBUG("class18_F001_start_update, sOffset[%d]\n", sOffset);
    memset((uint8*)&blockTrans, 0x00, sizeof(OOPM_BLOCKTRANS_T));
    memset(&tmpUpInfoD_F001_prtl, 0x00, sizeof(tmpUpInfoD_F001_prtl));
    
    if(srcBuf[sOffset++] != DT_STRUCTURE)
    {
        return DATA_TYPE_UNMATCHED;
    }
    if(srcBuf[sOffset++] != 3)
    {
        return DATA_TYPE_UNMATCHED;
    }
    if(srcBuf[sOffset++] != DT_STRUCTURE)
    {
        return DATA_TYPE_UNMATCHED;
    }
    if(srcBuf[sOffset++] != 6)
    {
        return DATA_TYPE_UNMATCHED;
    }
    dar = basic_buf_to_data(E_OOP_VISIBLEVAR_T, srcBuf, srcLen, &sOffset, &blockTrans.file.srceFile, sizeof(OOP_VISIBLEVAR_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }
    memcpy(tmpUpInfoD_F001_prtl.srcfilename, blockTrans.file.srceFile.value, blockTrans.file.srceFile.nNum);

    //如果源路径为空，默认一个文件名称
    if (blockTrans.file.srceFile.nNum == 0)
    {
        strcpy(tmpUpInfoD_F001_prtl.srcfilename, "NULL");
    }
    
    dar = basic_buf_to_data(E_OOP_VISIBLEVAR_T, srcBuf, srcLen, &sOffset, &blockTrans.file.destFile, sizeof(OOP_VISIBLEVAR_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }
    
    memcpy(tmpUpInfoD_F001_prtl.file_name, blockTrans.file.destFile.value, blockTrans.file.destFile.nNum);

    //如果目的路径为空，默认一个文件名称，台体远程升级需要
    if (blockTrans.file.destFile.nNum == 0)
    {
        strcpy(tmpUpInfoD_F001_prtl.file_name, UPDATNEW_NAME);
    }

    PRTL_FMT_DEBUG("原文件%s, 目标文件%s\n", tmpUpInfoD_F001_prtl.srcfilename, tmpUpInfoD_F001_prtl.file_name);
    
    dar = basic_buf_to_data(E_DOUBLE_LONG_UNSIGNED, srcBuf, srcLen, &sOffset, &blockTrans.file.nSize, 4);
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    } 
    tmpUpInfoD_F001_prtl.file_size = blockTrans.file.nSize;
    dar = basic_buf_to_data(E_OOP_BITSTR3_T, srcBuf, srcLen, &sOffset, &blockTrans.file.property, sizeof(OOP_BITSTR3_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }
    dar = basic_buf_to_data(E_OOP_VISIBLEVAR_T, srcBuf, srcLen, &sOffset, &blockTrans.file.version, sizeof(OOP_VISIBLEVAR_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }
    dar = basic_buf_to_data(E_ENUM, srcBuf, srcLen, &sOffset, &blockTrans.file.filetype, sizeof(uint8));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }
    tmpUpInfoD_F001_prtl.file_type =  blockTrans.file.filetype;
    if(srcBuf[sOffset] == DT_LONG_UNSIGNED)
    {
        dar = basic_buf_to_data(E_LONG_UNSIGNED, srcBuf, srcLen, &sOffset, &blockTrans.nBlockSize, 2);
        PRTL_FMT_DEBUG("blockTrans.nBlockSize[%d], dar[%d]\n", blockTrans.nBlockSize, dar);
        if(dar != DATA_SUCCESS)
        {
            PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
            return DATA_TYPE_UNMATCHED;
        }       
        tmpUpInfoD_F001_prtl.file_Pack_size = blockTrans.nBlockSize;
    }
    else if(srcBuf[sOffset] == DT_DOUBLE_LONG_UNSIGNED)
    {
        dar = basic_buf_to_data(E_DOUBLE_LONG_UNSIGNED,srcBuf,srcLen,&sOffset,&blockTrans.nBlockSize,4);
        PRTL_FMT_DEBUG("blockTrans.nBlockSize[%d], dar[%d]\n", blockTrans.nBlockSize, dar);
        if(dar != DATA_SUCCESS)
        {
            PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
            return DATA_TYPE_UNMATCHED;
        }  
        tmpUpInfoD_F001_prtl.file_Pack_size = blockTrans.nBlockSize;
    }
    else
    {
        return DATA_TYPE_UNMATCHED;
    }

    if(blockTrans.nBlockSize > TAR_FILE_MAX)        //文件传输包过大（暂定为4M）
    {
        return DATA_READ_WRITE_DENIED;
    }

    //校验
 //   dar = basic_buf_to_data(E_STRUCTURE,srcBuf,srcLen,&sOffset,NULL,0);
  //  PRTL_FMT_DEBUG("blockTrans.nBlockSize[%d], dar[%d]\n", blockTrans.nBlockSize, dar);
  //  if(dar != DATA_SUCCESS)
  //  {
   //     PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
  //      return DATA_TYPE_UNMATCHED;
  //  }
    uint8 option = sOffset;
    if(srcBuf[option] == DT_NULL)
    {
        sOffset++;
    }
    else if(srcBuf[option] == DT_STRUCTURE)
    {
        sOffset++;
        sOffset++;
            //校验类型
        dar =basic_buf_to_data(E_ENUM,srcBuf,srcLen,&sOffset,&blockTrans.crc.type,1);
        if(dar != DATA_SUCCESS)
        {
            PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
            return DATA_TYPE_UNMATCHED;
        }

        //校验值
        if(srcBuf[sOffset++] != DT_OCTET_STRING)
        {
            return DATA_TYPE_UNMATCHED;
        }
        blockTrans.crc.content.nNum = get_len_offset(srcBuf+sOffset,&offlen);
        PRTL_FMT_DEBUG("blockTrans.crc.content.nNum[%d]\n", blockTrans.crc.content.nNum);
        sOffset = sOffset + offlen;
        memcpy(blockTrans.crc.content.value, srcBuf + sOffset, blockTrans.crc.content.nNum);
        sOffset = sOffset+blockTrans.crc.content.nNum;

        memcpy(&dstBuf[dOffset], &blockTrans, sizeof(OOPM_BLOCKTRANS_T));
        dOffset += sizeof(OOPM_BLOCKTRANS_T);
    }else
    {
        return DATA_TYPE_UNMATCHED;
    }

    //避免每步都判断，放最后检查，但检查前可能已经越界
    if((sOffset > srcLen) || (dOffset > dstLen))
    {
        PRTL_FMT_DEBUG("cross the border. sOffset(%d), srcLen(%d), dOffset(%d), dstLen(%d)\n", sOffset, srcLen, dOffset, dstLen);
        return DATA_SCOPE_OF_ACCESS_VIOLATED;        
    }

    *convert->dOffset = dOffset;
    *convert->sOffset = sOffset;

    
    tmpUpInfoD_F001_prtl.checkType = blockTrans.crc.type;                                               //校验类型 
    memcpy(tmpUpInfoD_F001_prtl.crcValue, blockTrans.crc.content.value, blockTrans.crc.content.nNum);//校验值
    if(tmpUpInfoD_F001_prtl.file_size % tmpUpInfoD_F001_prtl.file_Pack_size == 0)                       //文件块数
    {
        tmpUpInfoD_F001_prtl.file_Pack_num = tmpUpInfoD_F001_prtl.file_size / tmpUpInfoD_F001_prtl.file_Pack_size;
    }
    else
    {
        tmpUpInfoD_F001_prtl.file_Pack_num = tmpUpInfoD_F001_prtl.file_size / tmpUpInfoD_F001_prtl.file_Pack_size + 1;
    }

    PRTL_FMT_DEBUG("BtmpUpInfoD_F001.update_state is %d\n",gUpInfoD_F001.update_state);
    if(gUpInfoD_F001.update_state == FALSE)
    {
        continueUpdate = FALSE;
    }
    if(gUpInfoD_F001.file_size != tmpUpInfoD_F001_prtl.file_size)             //文件大小不同，重新文件传输
        continueUpdate = FALSE;
    if(gUpInfoD_F001.file_Pack_size != tmpUpInfoD_F001_prtl.file_Pack_size)   //分包大小不同，重新文件传输
        continueUpdate = FALSE;
    if(gUpInfoD_F001.file_Pack_num != tmpUpInfoD_F001_prtl.file_Pack_num)     //总包数不同，重新文件传输
        continueUpdate = FALSE;
    if(gUpInfoD_F001.checkType != tmpUpInfoD_F001_prtl.checkType)             //校验类型不同，重新文件传输
        continueUpdate = FALSE;
    if(memcmp(gUpInfoD_F001.crcValue, tmpUpInfoD_F001_prtl.crcValue, 16) != 0)//校验值不同，重新文件传输
        continueUpdate = FALSE;
    if (strcmp(gUpInfoD_F001.file_name, tmpUpInfoD_F001_prtl.file_name) != 0) //目的文件名称不同，重新文件传输
        continueUpdate = FALSE;
    if (strcmp(gUpInfoD_F001.srcfilename, tmpUpInfoD_F001_prtl.srcfilename) != 0) //目的文件名称不同，重新文件传输
        continueUpdate = FALSE;

    PRTL_FMT_DEBUG("continueUpdate is %d\n", continueUpdate);
    if(continueUpdate == FALSE)//重新文件传输
    {
        clearUpdateFile();
        gUpInfoD_F001.file_succ_Pack_num = 0;
        gUpInfoD_F001.lastend = 0;
        gUpInfoD_F001.lastadd = 0;
        gUpInfoD_F001.update_state = TRUE;//进入文件传输状态
        memset(gUpInfoD_F001.file_name, 0, 256);
        strcpy(gUpInfoD_F001.file_name, (const char*)tmpUpInfoD_F001_prtl.file_name);
        memset(gUpInfoD_F001.srcfilename, 0, 256);
        strcpy(gUpInfoD_F001.srcfilename, (const char*)tmpUpInfoD_F001_prtl.srcfilename);
        gUpInfoD_F001.file_size = tmpUpInfoD_F001_prtl.file_size;
        gUpInfoD_F001.file_Pack_size = tmpUpInfoD_F001_prtl.file_Pack_size;
        gUpInfoD_F001.file_Pack_num = tmpUpInfoD_F001_prtl.file_Pack_num;
        gUpInfoD_F001.checkType = tmpUpInfoD_F001_prtl.checkType;
        memcpy(gUpInfoD_F001.crcValue, tmpUpInfoD_F001_prtl.crcValue, 16);
        gUpInfoD_F001.file_type = tmpUpInfoD_F001_prtl.file_type;
        /* 防止最后补发 */
        if((gUpInfoD_F001.file_Pack_num + 1) % 8 ==0)
        {
            gUpInfoD_F001.mask_len = (gUpInfoD_F001.file_Pack_num + 1) / 8;    //接收掩码的长度
        }
        else
        {
            gUpInfoD_F001.mask_len = (gUpInfoD_F001.file_Pack_num + 1) / 8 + 1;//接收掩码的长度
        }
        gUpInfoD_F001.pPacksMask = malloc(gUpInfoD_F001.mask_len);       //接收掩码
        memset(gUpInfoD_F001.pPacksMask, 0, gUpInfoD_F001.mask_len);
        gUpInfoD_F001.update_endtime = time(NULL) + 24 * 3600;             //默认文件传输持续时间为24小时

        PRTL_BUF_DEBUG(gUpInfoD_F001.crcValue, 16, "启动文件传输: 源%s, 目标%s, 大小%d, 校验", 
                       gUpInfoD_F001.srcfilename, gUpInfoD_F001.file_name, gUpInfoD_F001.file_size);
        PRTL_FMT_TRACE("gUpInfoD_F001.update_state is %d\n", gUpInfoD_F001.update_state);
        PRTL_FMT_TRACE("gUpInfoD_F001.file_size is %d\n", gUpInfoD_F001.file_size);
        PRTL_FMT_TRACE("gUpInfoD_F001.file_Pack_size is %d\n", gUpInfoD_F001.file_Pack_size);
        PRTL_FMT_TRACE("gUpInfoD_F001.file_Pack_num is %d\n", gUpInfoD_F001.file_Pack_num);
        PRTL_FMT_TRACE("gUpInfoD_F001.checkType is %d\n", gUpInfoD_F001.checkType);
        PRTL_FMT_TRACE("gUpInfoD_F001.mask_len is %d\n", gUpInfoD_F001.mask_len);
        PRTL_FMT_TRACE("gUpInfoD_F001.file_type is %d\n", gUpInfoD_F001.file_type);

        ret = WriteUpdateFile(UPDATNEW_FILE_698, (uint8*)&gUpInfoD_F001, 0, sizeof(gUpInfoD_F001));//写入文件传输信息
        if (ret < 0)
        {
            PRTL_FMT_DEBUG("写入文件传输信息结果错误 ret=%d\n", ret);
            return DATA_READ_WRITE_DENIED;
        }
    }

    return dar;
}

#if DESC("代发", 1)
/**********************************************************************
* @name      : file_proxy_ack_package
* @brief     ：文件代发内容封装
* @param[in] ：OOPM_FILEPROXY_T *pTrans     转发内容
               uint16 buflen               缓存长度
* @param[out]：uint8 *pBuf                 消息内容
* @return    ：消息长度
* @Update    :
**********************************************************************/
uint16 file_proxy_ack_package(OOPM_FILEPROXY_T *pTrans, uint8 *pBuf, uint16 buflen)
{
    uint16 offset = 0;
    uint32 i = 0;
    uint32 len = 0;
    uint32 lenoffset = 0;
    OOP_BITSTR3_T tmp = {0};
    
    if(NULL == pTrans || NULL == pBuf || buflen < 5)
    {
        return 0;
    }
    
    /* 目录类型 */
    pBuf[offset++] = pTrans->type;

    switch (pTrans->type)
    {
    case DT_OAD:
    {
        memcpy_r(&pBuf[offset], &pTrans->oad.value, sizeof(pTrans->oad.value));
        offset += sizeof(pTrans->oad.value);
        break;
    }
    case DT_ROAD:
    {
        memcpy_r(&pBuf[offset], &pTrans->road.oadMain.value, sizeof(pTrans->road.oadMain.value));
        offset += sizeof(pTrans->road.oadMain.value);
        pBuf[offset++] = MIN(OOP_MAX_OADS, pTrans->road.oadCols.nNum);
        for (i = 0; i < MIN(OOP_MAX_OADS, pTrans->road.oadCols.nNum); i++)
        {
            memcpy_r(&pBuf[offset], &pTrans->road.oadCols.oad[i].value, sizeof(pTrans->road.oadCols.oad[i].value));
            offset += sizeof(pTrans->road.oadCols.oad[i].value);
        }
        break;
    }
    case DT_MS:
    {
        if (pTrans->ms.choice == 3)
        {
            len = pTrans->ms.msa.size;
            lenoffset = set_len_offset(len, pBuf + offset);
            if (lenoffset <= 0)
            {
                PRTL_FMT_DEBUG("地址太多 ms.choice[%d] len[%d]\n", pTrans->ms.choice, pTrans->ms.msa.size);
                return DATA_SCOPE_OF_ACCESS_VIOLATED;
            }
            
            offset += lenoffset;
        
            for (i = 0; i < len; i++)
            {
                if (pTrans->ms.msa.add[i].type == 0)
                {
                    pBuf[offset ++] = 0;
                }
                else if (pTrans->ms.msa.add[i].type == 2)
                {
                    pBuf[offset ++] = 1;
                }
                else if (pTrans->ms.msa.add[i].type == 3)
                {
                     pBuf[offset ++] = 2;
                }
                else
                {
                    PRTL_FMT_DEBUG("地址类型错误 不应该为通配地址[%d]\n", pTrans->ms.msa.add[i].type);
                    return DATA_ADDRESS_EXCEPTION;
                }
                //新的变成octet-string 没有默认长度了
                pBuf[offset++] = 6;
                memcpy(&pBuf[offset], pTrans->ms.msa.add[i].add, 6);
                offset += 6;
            }
        }
        else
        {
            PRTL_FMT_DEBUG("ms集合类型不支持 ms.choice[%d] len[%d]\n", pTrans->ms.choice, pTrans->ms.msa.size);
            return 0;
        }
        break;
    }
    default:
        PRTL_FMT_DEBUG("目录类型不支持 type[%d]\n", pTrans->type);
        return 0;
        break;
    }

    /* 文件信息 */
    //源文件
    len = MIN(pTrans->transInfo.file.srceFile.nNum, 64);
    pBuf[offset++] = len;
    memcpy(&pBuf[offset], pTrans->transInfo.file.srceFile.value, len);
    offset += len;

    //目的文件
    len = MIN(pTrans->transInfo.file.destFile.nNum, 64);
    pBuf[offset++] = len;
    memcpy(&pBuf[offset], pTrans->transInfo.file.destFile.value, len);
    offset += len;

    //文件大小
    memcpy_r(&pBuf[offset], &pTrans->transInfo.file.nSize, sizeof(pTrans->transInfo.file.nSize));
    offset += sizeof(pTrans->transInfo.file.nSize);

    //文件属性(暂时先固定为3)
    pBuf[offset++] = 3; 
    memcpy(&tmp, &pTrans->transInfo.file.property, sizeof(pTrans->transInfo.file.property));
    membitrev((uint8 *)&tmp, sizeof(tmp));
    memcpy(&pBuf[offset], (uint8 *)&tmp, sizeof(tmp));
    offset += sizeof(tmp);

    //文件版本
    len = MIN(pTrans->transInfo.file.version.nNum, 64);
    pBuf[offset++] = len;
    memcpy(&pBuf[offset], pTrans->transInfo.file.version.value, len);
    offset += len;

    //文件类别
    pBuf[offset++] = pTrans->transInfo.file.filetype;

    /* 传输块大小 */
    memcpy_r(&pBuf[offset], &pTrans->transInfo.nBlockSize, sizeof(pTrans->transInfo.nBlockSize));
    offset += sizeof(pTrans->transInfo.nBlockSize);

    /* 传输块大小 */
    //校验类型
    pBuf[offset++] = pTrans->transInfo.crc.type;

    //校验值
    len = MIN(pTrans->transInfo.crc.content.nNum, 16);
    pBuf[offset++] = len;
    memcpy(&pBuf[offset], pTrans->transInfo.crc.content.value, len);
    offset += len;

    /* 兼容软件版本号 */
    len =  MIN(pTrans->transInfo.swlist.nNum, OOP_MAX_COMPATIBLE);
    pBuf[offset++] = len;
    for (i = 0; i < len; i++)
    {
        pBuf[offset++] = MIN(pTrans->transInfo.swlist.value[i].nNum, 64);
        memcpy(&pBuf[offset], pTrans->transInfo.swlist.value[i].value, MIN(pTrans->transInfo.swlist.value[i].nNum, 64));
        offset += MIN(pTrans->transInfo.swlist.value[i].nNum, 64);
    }
    
    /* 兼容硬件版本号 */
    len =  MIN(pTrans->transInfo.hwlist.nNum, OOP_MAX_COMPATIBLE);
    pBuf[offset++] = len;
    for (i = 0; i < len; i++)
    {
        pBuf[offset++] = MIN(pTrans->transInfo.hwlist.value[i].nNum, 64);
        memcpy(&pBuf[offset], pTrans->transInfo.hwlist.value[i].value, MIN(pTrans->transInfo.hwlist.value[i].nNum, 64));
        offset += MIN(pTrans->transInfo.hwlist.value[i].nNum, 64);
    }

    /* 下载方的标识 */
    pBuf[offset++] = MIN(pTrans->transInfo.identy.nNum, 64);
    memcpy(&pBuf[offset], pTrans->transInfo.identy.value, MIN(pTrans->transInfo.identy.nNum, 64));
    offset += MIN(pTrans->transInfo.identy.nNum, 64);

    /* 安全传输 */
    pBuf[offset++] = pTrans->bEncode;
    
    return offset;
}

/**
*********************************************************************
* @brief：      buf_to_data_file_proxy_msg
* @param[in]： ppOadInfo    - 数据表指针
               srcBuf       - 源数据
               srcLen       - 源数据长度
* @param[out]：dstBuf       - 目的数据
               dstLen       - 目的数据长度
               sOffset      - 转换后源数据区的偏移
               dOffset      - 转换后目的数据区的偏移
* @return：     =0          - 成功
              !=0           - 错误码
*********************************************************************
*/
OOP_DAR_E buf_to_data_trans_msg(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOPM_STARTTRANS_T data = {0};
    uint8* srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8* dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    uint32            dOffsetTmp = 0;
    DATA_CONVERT_T    converttmp;
    memset(&converttmp, 0, sizeof(converttmp));

    MEMZERO(&data, sizeof(data));

    if (srcBuf[sOffset] == DT_NULL)
    {
        sOffset++;
        *convert->dOffset = dOffset;
        *convert->sOffset = sOffset;

        PRTL_BUF_DEBUG(srcBuf, srcLen, "trans msg is null. sOffset(%d)", sOffset);
        return DATA_SUCCESS;
    }

    //结构类型
    if(srcBuf[sOffset++] != DT_STRUCTURE)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "check struct type failed. sOffset(%d)", sOffset);
        return DATA_TYPE_UNMATCHED;
    }
    
    //结构成员数
    if(srcBuf[sOffset++] != 6)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "check struct num failed. sOffset(%d)", sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    converttmp.srcBuf = (uint8*)srcBuf;
    converttmp.srcLen = srcLen;
    converttmp.sOffset = &sOffset;
    
    converttmp.dstBuf = &data.file;
    converttmp.dstLen = sizeof(data.file);
    converttmp.dOffset = &dOffsetTmp;
    
    dar = buf_to_data_file_trans_msg(pOadInfo, &converttmp);
    if (dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "buf_to_data_file_trans_msg failed. dar(%d), offset(%d)", dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //传输块大小
    dar = basic_buf_to_data(E_LONG_UNSIGNED, srcBuf, srcLen, &sOffset, &data.nBlockSize, sizeof(data.nBlockSize));
    if (dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }
    
    //校验
    dOffsetTmp = 0;
    converttmp.srcBuf = (uint8*)srcBuf;
    converttmp.srcLen = srcLen;
    converttmp.sOffset = &sOffset;
    
    converttmp.dstBuf = &data.crc;
    converttmp.dstLen = sizeof(data.crc);
    converttmp.dOffset = &dOffsetTmp;
    
    dar = buf_to_data_filecrc_trans_msg(pOadInfo, &converttmp);
    if (dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "buf_to_data_filecrc_trans_msg failed. dar(%d), offset(%d)", dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }
    
    //兼容版本
    dOffsetTmp = 0;
    converttmp.srcBuf = (uint8*)srcBuf;
    converttmp.srcLen = srcLen;
    converttmp.sOffset = &sOffset;
    
    converttmp.dstBuf = &data.swlist;
    converttmp.dstLen = sizeof(data.swlist);
    converttmp.dOffset = &dOffsetTmp;
    
    dar = buf_to_data_version_msg(pOadInfo, &converttmp);
    if (dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "buf_to_data_filecrc_trans_msg failed. dar(%d), offset(%d)", dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }
    
    //兼容版本
    dOffsetTmp = 0;
    converttmp.srcBuf = (uint8*)srcBuf;
    converttmp.srcLen = srcLen;
    converttmp.sOffset = &sOffset;
    
    converttmp.dstBuf = &data.hwlist;
    converttmp.dstLen = sizeof(data.hwlist);
    converttmp.dOffset = &dOffsetTmp;
    
    dar = buf_to_data_version_msg(pOadInfo, &converttmp);
    if (dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "buf_to_data_filecrc_trans_msg failed. dar(%d), offset(%d)", dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }
    
    dar = basic_buf_to_data(E_OOP_VISIBLEVAR_T, srcBuf, srcLen, &sOffset, &data.identy, sizeof(OOP_VISIBLEVAR_T));
    if (dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    memcpy(&dstBuf[dOffset], &data, sizeof(data));
    dOffset += sizeof(data);

    //避免每步都判断，放最后检查，但检查前可能已经越界
    if ((sOffset > srcLen) || (dOffset > dstLen))
    {
        PRTL_FMT_DEBUG("cross the border. sOffset(%d), srcLen(%d), dOffset(%d), dstLen(%d)\n", sOffset, srcLen, dOffset, dstLen);
        return DATA_SCOPE_OF_ACCESS_VIOLATED;
    }

    *convert->dOffset = dOffset;
    *convert->sOffset = sOffset;

    return dar;
}

/**
*********************************************************************
* @brief：      buf_to_data_file_proxy_msg
* @param[in]： ppOadInfo    - 数据表指针
               srcBuf       - 源数据
               srcLen       - 源数据长度
* @param[out]：dstBuf       - 目的数据
               dstLen       - 目的数据长度
               sOffset      - 转换后源数据区的偏移
               dOffset      - 转换后目的数据区的偏移
* @return：     =0          - 成功
              !=0           - 错误码
*********************************************************************
*/
OOP_DAR_E buf_to_data_file_proxy_msg(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOPM_FILEPROXY_T data = {0};
    uint8* srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8* dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    uint32            dOffsetTmp = 0;
    DATA_CONVERT_T    converttmp;
    memset(&converttmp, 0, sizeof(converttmp));

    MEMZERO(&data, sizeof(data));

    //结构类型
    if(srcBuf[sOffset++] != DT_STRUCTURE)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "check struct type failed. sOffset(%d)", sOffset);
        return DATA_TYPE_UNMATCHED;
    }
    
    //结构成员数
    if(srcBuf[sOffset++] != 3)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "check struct num failed. sOffset(%d)", sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    data.type = srcBuf[sOffset];

    switch (data.type)
    {
    case DT_OAD:
    {
        dar = basic_buf_to_data(E_OAD, srcBuf, srcLen, &sOffset, &data.oad, sizeof(data.oad));
        if (dar != DATA_SUCCESS)
        {
            PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
            return DATA_TYPE_UNMATCHED;
        } 
        break;
    }
    case DT_ROAD:
    {
        dar = basic_buf_to_data(E_ROAD, srcBuf, srcLen, &sOffset, &data.road, sizeof(data.road));
        if (dar != DATA_SUCCESS)
        {
            PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
            return DATA_TYPE_UNMATCHED;
        } 
        break;
    }
    case DT_TSA:
    {
        dar = basic_buf_to_data(E_TSA, srcBuf, srcLen, &sOffset, &data.tsa, sizeof(data.tsa));
        if (dar != DATA_SUCCESS)
        {
            PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
            return DATA_TYPE_UNMATCHED;
        } 
        break;
    }
    case DT_MS:
    {
        dar = basic_buf_to_data(E_MS, srcBuf, srcLen, &sOffset, &data.ms, sizeof(data.ms));
        if (dar != DATA_SUCCESS)
        {
            PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
            return DATA_TYPE_UNMATCHED;
        } 
        break;
    }
    default:
        PRTL_BUF_DEBUG(srcBuf, srcLen, "check struct num failed. sOffset(%d)", sOffset);
        return DATA_TYPE_UNMATCHED;
        break;
    }

    //文件信息
    converttmp.srcBuf = (uint8*)srcBuf;
    converttmp.srcLen = srcLen;
    converttmp.sOffset = &sOffset;
    
    converttmp.dstBuf = &data.transInfo;
    converttmp.dstLen = sizeof(data.transInfo);
    converttmp.dOffset = &dOffsetTmp;
    
    dar = buf_to_data_trans_msg(pOadInfo, &converttmp);
    if (dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "buf_to_data_file_trans_msg failed. dar(%d), offset(%d)", dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    dar = basic_buf_to_data(E_BOOL, srcBuf, srcLen, &sOffset, &data.bEncode, sizeof(data.bEncode));
    if (dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    memcpy(&dstBuf[dOffset], &data, sizeof(data));
    dOffset += sizeof(data);

    //避免每步都判断，放最后检查，但检查前可能已经越界
    if ((sOffset > srcLen) || (dOffset > dstLen))
    {
        PRTL_FMT_DEBUG("cross the border. sOffset(%d), srcLen(%d), dOffset(%d), dstLen(%d)\n", sOffset, srcLen, dOffset, dstLen);
        return DATA_SCOPE_OF_ACCESS_VIOLATED;
    }

    *convert->dOffset = dOffset;
    *convert->sOffset = sOffset;

    return dar;
}

/**
*********************************************************************
* @name：       class18_F001_excute_update_other
* @brief：      class19的ACT服务
* @param[in] ：pOdaInfo        OAD信息
               pBpInfo        应用分帧断点信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
               outDataMax     输出报文缓冲区最大长度

* @param[out]：outData     输出报文
* @return    ：OOP_DAR_E
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E class18_F001_excute_update_ms(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    uint32    sOffset = *convert->sOffset;
    uint8    *srcBuf = (uint8*)convert->srcBuf;
    uint32    srcLen = convert->srcLen;
    uint32    dOffset = 0;
    
    int       ret;
    OOP_DAR_E dar = DATA_SUCCESS;
    uint8     msgdata[1024] = {0};
    uint32    msglen = 0;
    
    uint8  recvbuf[4] = {0};
    uint16 recLen = 0;

    OOPM_FILEPROXY_T  data;
    DATA_CONVERT_T    converttmp;
    memset(&converttmp, 0, sizeof(converttmp));
    memset(&data, 0, sizeof(data));

    PRTL_BUF_DEBUG(srcBuf, 10, "F0010500带MS集合的命令转发", sOffset);

    converttmp.srcBuf = (uint8*)srcBuf;
    converttmp.srcLen = srcLen;
    converttmp.sOffset = &sOffset;
    
    converttmp.dstBuf = &data;
    converttmp.dstLen = sizeof(data);
    converttmp.dOffset = &dOffset;

    dar = buf_to_data_file_proxy_msg(pOadInfo, &converttmp);
    if (dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "buf_to_data_file_trans_msg failed. dar(%d), offset(%d)", dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    data.transInfo.file.srceFile.nNum = strlen(UPDATNEW_PACK_NEW);
    sprintf(data.transInfo.file.srceFile.value, UPDATNEW_PACK_NEW);

    msglen = file_proxy_ack_package(&data, msgdata, 1024);
    if (msglen == 0)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "file_proxy_ack_package failed. dar(%d), offset(%d)", dar, sOffset);
        return DATA_DATA_BLOCK_UNAVAILABLE;
    }

    PRTL_BUF_DEBUG(msgdata, msglen, "消息发送");
    ret = appmain_send_proxy_to_taskManager(msgdata, msglen, recvbuf, 4, &recLen);
    if ((ret != 0) || (recvbuf[0] != 1))
    {
        PRTL_BUF_DEBUG(recvbuf, recLen, "消息回码失败 ret[%d] file[%s] recv", ret, data.transInfo.file.srceFile.value);

        //不判回码
        //return DATA_SCOPE_OF_ACCESS_VIOLATED;
    }

    return dar;
}

/**
*********************************************************************
* @name：       class18_F001_excute_update_other
* @brief：      class19的ACT服务
* @param[in] ：pOdaInfo        OAD信息
               pBpInfo        应用分帧断点信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
               outDataMax     输出报文缓冲区最大长度

* @param[out]：outData     输出报文
* @return    ：OOP_DAR_E
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E class18_F001_excute_update_other(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    int       ret;
    OOP_DAR_E dar = DATA_OTHER_REASON;
    uint32    sOffset = *convert->sOffset;
    uint8     *srcBuf = (uint8*)convert->srcBuf;
    uint32    srcLen = convert->srcLen;
    char      filepath[100] = {0};
    uint8     fileLen = 0;
    OOP_OAD_U oad;
    uint8     msgdata[100] = {0};
    uint32    msglen=0;
    uint32    offset=0;
    uint32    msgid=0;
    uint16    modulenum=0;
    uint16    label=0;

    PRTL_BUF_DEBUG(srcBuf, 10, "F0010500命令转发:", sOffset);

    if (srcBuf[sOffset++] != DT_STRUCTURE)
    {
        return DATA_TYPE_UNMATCHED;
    }
    
    if (srcBuf[sOffset++] != 3)
    {
        return DATA_OTHER_REASON;
    }

    switch(srcBuf[sOffset++])
    {
        case DT_OAD:
        {
            memcpy_r((uint8*)&oad, &srcBuf[sOffset], 4);
            sOffset = sOffset + 4;
            modulenum = oad.value&0x00FF;
            memcpy_r(msgdata + offset, (uint8*)&modulenum, 4);
            offset = offset + 4;
            sprintf(filepath, "%s", UPDATNEW_PACK_NEW);
            fileLen = strlen(filepath);
            memcpy(msgdata + offset, &fileLen, sizeof(fileLen));
            offset = offset + 1;
            memcpy(msgdata + offset, filepath, strlen(filepath));
            offset = offset + strlen(filepath);
            msglen = offset;

            if (((oad.value & 0xFFFFFF00) == 0xF20D0200)||(oad.nObjID == 0x4040))
            {
                PRTL_BUF_DEBUG(msgdata, msglen, "F0010500发送消息:");
                msgid = MSG_APPAMIN_module_update_cmd;
                ret=appmain_send_msg(msgid, label, msgdata, msglen);
                if (ret == 0)
                {
                    dar = DATA_SUCCESS;
                }
            }
#if (defined AREA_ZHEJIANG)|| (defined AREA_HUNAN)
            else if ((oad.value & 0xFFFFFF00) == 0xF2090200)
            {
                char cmd[1024] = {0};
                sprintf(cmd, "cp %s /data/app/ccoRouter/router_update_file_%d", filepath, modulenum);
                PRTL_FMT_TRACE("%s\n", cmd);
                if (exec_system_cmd(cmd) != 0)
                {
                    PRTL_FMT_DEBUG("命令执行失败, 可能是超时 cmd:%s\n", cmd);
                }
                else
                {
                    dar = DATA_SUCCESS;
                }
            }
#else
            else if (oad.value == 0xF2090201)
            {
                //载波cco升级 设备代发给taskManager 把文件名字改为router
                uint8  recvbuf[4] = {0};
                uint16 recLen = 0;
                char newfilepath[100]={0};
                char cmd[1024]={0};
                sprintf(newfilepath,"/data/app/ccoRouter/router_file");
                sprintf(cmd,"cp %s %s",filepath,newfilepath);
                PRTL_FMT_DEBUG("拷贝文件 %s\n", cmd);
                exec_system_cmd(cmd);
                //组消息发给taskManager
                memset(msgdata,0x00,sizeof(msgdata));
                offset = 0;
                msgdata[offset++] = strlen(newfilepath);
                memcpy(&msgdata[offset],newfilepath,strlen(newfilepath));
                offset+=strlen(newfilepath);
                //只要文件路径 其他都可以为空
                offset+=4;
                offset+=4;
                msgdata[offset++] = 0;
                PRTL_BUF_DEBUG(msgdata, offset, "消息发送至taskManager 文件代发 ");
                ret = appmain_send_proxy_to_taskManager(msgdata, offset, recvbuf, 4, &recLen);
                if ((ret != 0) || (recvbuf[0] != 1))
                {
                    PRTL_BUF_DEBUG(recvbuf, recLen, "消息回码失败 ret[%d] file[%s] recv", ret, newfilepath);
                }
                dar = DATA_SUCCESS;
            }
#endif 

        }
        break;

        case DT_ROAD:
        {
            int        i = 0;
            OOP_ROAD_T road;

            MEMZERO(&road, sizeof(OOP_ROAD_T));

            // 对象属性描述符 OAD
            memcpy_r(&road.oadMain.value, &srcBuf[sOffset], sizeof(OOP_OAD_U));
            sOffset += 4;

            modulenum = road.oadMain.value & 0x00FF;
            sprintf(filepath, "%s", UPDATNEW_PACK_NEW);

            // 关联对象属性描述符  SEQUENCE OF OAD
            road.oadCols.nNum = srcBuf[sOffset];
            sOffset ++;
            for (i = 0; i < road.oadCols.nNum; i++)
            {
                memcpy_r(&road.oadCols.oad[i], &srcBuf[sOffset], sizeof(OOP_OAD_U));
                sOffset += 4;
            }

            // 避免每步都判断，放最后检查，但检查前可能已经越界
            if (sOffset > srcLen)
            {
                PRTL_FMT_DEBUG("cross the border. sOffset(%d), srcLen(%d)\n", sOffset, srcLen);
                return DATA_SCOPE_OF_ACCESS_VIOLATED;        
            }

#if (defined AREA_ZHEJIANG)|| (defined AREA_HUNAN)
            if (road.oadCols.nNum < 1)
            {
                return DATA_OBJECT_UNAVAILABLE;
            }

            if (((road.oadMain.value & 0xFFFFFF00) == 0xF2090200) && (road.oadCols.oad[0].value == 0xF2100200))
            {
                char cmd[1024] = {0};
                sprintf(cmd, "cp %s /data/app/ccoRouter/sub_router_update_file_%d", filepath, modulenum);
                PRTL_FMT_TRACE("%s\n", cmd);
                if (exec_system_cmd(cmd) != 0)
                {
                    PRTL_FMT_DEBUG("命令执行失败, 可能是超时 cmd:%s\n", cmd);
                }
                else
                {
                    dar = DATA_SUCCESS;
                }
            }
#else
            if (road.oadCols.nNum < 1)
            {
                return DATA_OBJECT_UNAVAILABLE;
            }
            
            if (((road.oadMain.value & 0xFFFFFF00) == 0xF2090200) && (road.oadCols.oad[0].value == 0xF2100200))
            {
                //载波sta升级 设备代发给taskManager 把文件名字改为slaver_file
                uint8  recvbuf[4] = {0};
                uint16 recLen = 0;
                char newfilepath[100]={0};
                char cmd[1024]={0};
                sprintf(newfilepath,"/data/app/ccoRouter/slaver_file");
                sprintf(cmd,"cp %s %s",filepath,newfilepath);
                PRTL_FMT_DEBUG("拷贝文件 %s\n", cmd);
                exec_system_cmd(cmd);
                //组消息发给taskManager
                memset(msgdata,0x00,sizeof(msgdata));
                offset = 0;
                msgdata[offset++] = strlen(newfilepath);
                memcpy(&msgdata[offset],newfilepath,strlen(newfilepath));
                offset+=strlen(newfilepath);
                //只要文件路径 其他都可以为空
                offset+=4;
                offset+=4;
                msgdata[offset++] = 0;
                PRTL_BUF_DEBUG(msgdata, offset, "消息发送至taskManager 文件代发 ");
                ret = appmain_send_proxy_to_taskManager(msgdata, offset, recvbuf, 4, &recLen);
                if ((ret != 0) || (recvbuf[0] != 1))
                {
                    PRTL_BUF_DEBUG(recvbuf, recLen, "消息回码失败 ret[%d] file[%s] recv", ret, newfilepath);
                }
                dar = DATA_SUCCESS;

            }
#endif 

        }
        break;

        case DT_MS:
        {
            int       i = 0;
            
            OOP_MSP_T ms;
            uint8  recvbuf[4] = {0};
            uint16 recLen = 0;
            uint16 len = 0;
            struct stat statbuf = {0};
            uint32 fileSize = 0;

            int    PackLen = 200;
            uint8  lenoffset = 0;
     
            MEMZERO(&ms, sizeof(ms));

            dar = apdu_ms_fromfmt698(srcBuf, srcLen, &sOffset, &ms, sizeof(ms));
            if (dar != DATA_SUCCESS)
            {
                PRTL_BUF_DEBUG(srcBuf, srcLen, "apdu_csd_fromfmt698 failed. dar(%d), offset(%d)", dar, offset);
                return DATA_TYPE_UNMATCHED;           
            }

            if (stat(UPDATNEW_PACK_NEW, &statbuf) < 0)
            {
                PRTL_FMT_DEBUG("获得不到文件[%s]大小\n", UPDATNEW_PACK_NEW);
                return DATA_READ_WRITE_DENIED;
            }
            else
            {
                PRTL_FMT_DEBUG("获得到文件[%s]大小[%d]\n", UPDATNEW_PACK_NEW, statbuf.st_size);
                fileSize = statbuf.st_size;
            }

            sprintf(filepath, "%s", UPDATNEW_PACK_NEW);
            len = strlen(filepath) + 1;
            lenoffset = set_len_offset(len, msgdata + offset);
            if (lenoffset <= 0)
            {
                PRTL_FMT_DEBUG("设备file[%s] 路径太长 len[%d]\n", filepath, len);
                return DATA_SCOPE_OF_ACCESS_VIOLATED;
            }
            else
            {
                PRTL_FMT_DEBUG("设备file[%s] 路径长度[%d]\n", filepath, len);
            }
            
            offset += lenoffset;
            
            memcpy(msgdata + offset, filepath, strlen(filepath));
            offset += len;

            memcpy_r(msgdata + offset, &fileSize, sizeof(fileSize));
            offset += sizeof(fileSize);

            memcpy_r(msgdata + offset, &PackLen, sizeof(PackLen));
            offset += sizeof(PackLen);

            if (ms.choice == 3)
            {
                len = ms.msa.size;
                lenoffset = set_len_offset(len, msgdata + offset);
                if (lenoffset <= 0)
                {
                    PRTL_FMT_DEBUG("地址太多 ms.choice[%d] len[%d]\n", ms.choice, ms.msa.size);
                    return DATA_SCOPE_OF_ACCESS_VIOLATED;
                }
                
                offset += lenoffset;

                for (i = 0; i < len; i++)
                {
                    if (ms.msa.add[i].type == 0)
                    {
                        msgdata[offset ++] = 0;
                    }
                    else if (ms.msa.add[i].type == 2)
                    {
                        msgdata[offset ++] = 1;
                    }
                    else if (ms.msa.add[i].type == 3)
                    {
                         msgdata[offset ++] = 2;
                    }
                    else
                    {
                        PRTL_FMT_DEBUG("地址类型错误 不应该为通配地址[%d]\n", ms.msa.add[i].type);
                        return DATA_ADDRESS_EXCEPTION;
                    }
                    
                    memcpy(&msgdata[offset], ms.msa.add[i].add, 6);
                    offset += 6;
                }
            }
            else
            {
                PRTL_FMT_DEBUG("ms集合类型不支持 ms.choice[%d] len[%d]\n", ms.choice, ms.msa.size);
            }
            
            msglen = offset;

            PRTL_BUF_DEBUG(msgdata, msglen, "消息发送");
            ret = appmain_send_proxy_to_taskManager(msgdata, msglen, recvbuf, 4, &recLen);
            if ((ret != 0) || (recvbuf[0] != 1))
            {
                PRTL_BUF_DEBUG(recvbuf, recLen, "消息回码失败 ret[%d] file[%s] recv", ret, filepath);

                //不判回码
                //return DATA_SCOPE_OF_ACCESS_VIOLATED;
            }
        }
        break;
    }
    return dar;
}

/**
*********************************************************************
* @name：       class18_F001_excute_update_other
* @brief：      class19的ACT服务
* @param[in] ：pOdaInfo        OAD信息
               pBpInfo        应用分帧断点信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
               outDataMax     输出报文缓冲区最大长度

* @param[out]：outData     输出报文
* @return    ：OOP_DAR_E
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E class18_F001_excute_update_old(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    int       ret;
    OOP_DAR_E dar = DATA_OTHER_REASON;
    uint32    sOffset = *convert->sOffset;
    uint8     *srcBuf = (uint8*)convert->srcBuf;
    uint32    srcLen = convert->srcLen;
    char      filepath[100] = {0};
    uint8     fileLen = 0;
    OOP_OAD_U oad;
    uint8     msgdata[100] = {0};
    uint32    msglen=0;
    uint32    offset=0;
    uint32    msgid=0;
    uint16    modulenum=0;
    uint16    label=0;

    PRTL_BUF_DEBUG(srcBuf, 10, "F0010500命令转发:", sOffset);

    switch(srcBuf[sOffset++])
    {
        case DT_OAD:
        {
            memcpy_r((uint8*)&oad, &srcBuf[sOffset], 4);
            sOffset = sOffset + 4;
            modulenum = oad.value&0x00FF;
            memcpy_r(msgdata + offset, (uint8*)&modulenum, 4);
            offset = offset + 4;
            sprintf(filepath, "%s", UPDATNEW_PACK_NEW);
            fileLen = strlen(filepath);
            memcpy(msgdata + offset, &fileLen, sizeof(fileLen));
            offset = offset + 1;
            memcpy(msgdata + offset, filepath, strlen(filepath));
            offset = offset + strlen(filepath);
            msglen = offset;

            if (((oad.value & 0xFFFFFF00) == 0xF20D0200)||(oad.nObjID == 0x4040))
            {
                PRTL_BUF_DEBUG(msgdata, msglen, "F0010500发送消息:");
                msgid = MSG_APPAMIN_module_update_cmd;
                ret=appmain_send_msg(msgid, label, msgdata, msglen);
                if (ret == 0)
                {
                    dar = DATA_SUCCESS;
                }
            }
#if (defined AREA_ZHEJIANG)|| (defined AREA_HUNAN)
            else if ((oad.value & 0xFFFFFF00) == 0xF2090200)
            {
                char cmd[1024] = {0};
                sprintf(cmd, "cp %s /data/app/ccoRouter/router_update_file_%d", filepath, modulenum);
                PRTL_FMT_TRACE("%s\n", cmd);
                if (exec_system_cmd(cmd) != 0)
                {
                    PRTL_FMT_DEBUG("命令执行失败, 可能是超时 cmd:%s\n", cmd);
                }
                else
                {
                    dar = DATA_SUCCESS;
                }
            }
#else
            else if (oad.value == 0xF2090201)
            {
                //载波cco升级 设备代发给taskManager 把文件名字改为router
                uint8  recvbuf[4] = {0};
                uint16 recLen = 0;
                char newfilepath[100]={0};
                char cmd[1024]={0};
                sprintf(newfilepath,"/data/app/ccoRouter/router_file");
                sprintf(cmd,"cp %s %s",filepath,newfilepath);
                PRTL_FMT_DEBUG("拷贝文件 %s\n", cmd);
                exec_system_cmd(cmd);
                //组消息发给taskManager
                memset(msgdata,0x00,sizeof(msgdata));
                offset = 0;
                msgdata[offset++] = strlen(newfilepath);
                memcpy(&msgdata[offset],newfilepath,strlen(newfilepath));
                offset+=strlen(newfilepath);
                //只要文件路径 其他都可以为空
                offset+=4;
                offset+=4;
                msgdata[offset++] = 0;
                PRTL_BUF_DEBUG(msgdata, offset, "消息发送至taskManager 文件代发 ");
                ret = appmain_send_proxy_to_taskManager(msgdata, offset, recvbuf, 4, &recLen);
                if ((ret != 0) || (recvbuf[0] != 1))
                {
                    PRTL_BUF_DEBUG(recvbuf, recLen, "消息回码失败 ret[%d] file[%s] recv", ret, newfilepath);
                }
                dar = DATA_SUCCESS;
            }
#endif 

        }
        break;

        case DT_ROAD:
        {
            int        i = 0;
            OOP_ROAD_T road;

            MEMZERO(&road, sizeof(OOP_ROAD_T));

            // 对象属性描述符 OAD
            memcpy_r(&road.oadMain.value, &srcBuf[sOffset], sizeof(OOP_OAD_U));
            sOffset += 4;

            modulenum = road.oadMain.value & 0x00FF;
            sprintf(filepath, "%s", UPDATNEW_PACK_NEW);

            // 关联对象属性描述符  SEQUENCE OF OAD
            road.oadCols.nNum = srcBuf[sOffset];
            sOffset ++;
            for (i = 0; i < road.oadCols.nNum; i++)
            {
                memcpy_r(&road.oadCols.oad[i], &srcBuf[sOffset], sizeof(OOP_OAD_U));
                sOffset += 4;
            }

            // 避免每步都判断，放最后检查，但检查前可能已经越界
            if (sOffset > srcLen)
            {
                PRTL_FMT_DEBUG("cross the border. sOffset(%d), srcLen(%d)\n", sOffset, srcLen);
                return DATA_SCOPE_OF_ACCESS_VIOLATED;        
            }

#if (defined AREA_ZHEJIANG)|| (defined AREA_HUNAN)
            if (road.oadCols.nNum < 1)
            {
                return DATA_OBJECT_UNAVAILABLE;
            }

            if (((road.oadMain.value & 0xFFFFFF00) == 0xF2090200) && (road.oadCols.oad[0].value == 0xF2100200))
            {
                char cmd[1024] = {0};
                sprintf(cmd, "cp %s /data/app/ccoRouter/sub_router_update_file_%d", filepath, modulenum);
                PRTL_FMT_TRACE("%s\n", cmd);
                if (exec_system_cmd(cmd) != 0)
                {
                    PRTL_FMT_DEBUG("命令执行失败, 可能是超时 cmd:%s\n", cmd);
                }
                else
                {
                    dar = DATA_SUCCESS;
                }
            }
#else
            if (road.oadCols.nNum < 1)
            {
                return DATA_OBJECT_UNAVAILABLE;
            }
            
            if (((road.oadMain.value & 0xFFFFFF00) == 0xF2090200) && (road.oadCols.oad[0].value == 0xF2100200))
            {
                //载波sta升级 设备代发给taskManager 把文件名字改为slaver_file
                uint8  recvbuf[4] = {0};
                uint16 recLen = 0;
                char newfilepath[100]={0};
                char cmd[1024]={0};
                sprintf(newfilepath,"/data/app/ccoRouter/slaver_file");
                sprintf(cmd,"cp %s %s",filepath,newfilepath);
                PRTL_FMT_DEBUG("拷贝文件 %s\n", cmd);
                exec_system_cmd(cmd);
                //组消息发给taskManager
                memset(msgdata,0x00,sizeof(msgdata));
                offset = 0;
                msgdata[offset++] = strlen(newfilepath);
                memcpy(&msgdata[offset],newfilepath,strlen(newfilepath));
                offset+=strlen(newfilepath);
                //只要文件路径 其他都可以为空
                offset+=4;
                offset+=4;
                msgdata[offset++] = 0;
                PRTL_BUF_DEBUG(msgdata, offset, "消息发送至taskManager 文件代发 ");
                ret = appmain_send_proxy_to_taskManager(msgdata, offset, recvbuf, 4, &recLen);
                if ((ret != 0) || (recvbuf[0] != 1))
                {
                    PRTL_BUF_DEBUG(recvbuf, recLen, "消息回码失败 ret[%d] file[%s] recv", ret, newfilepath);
                }
                dar = DATA_SUCCESS;

            }
#endif 

        }
        break;

        case DT_MS:
        {
            int       i = 0;
            
            OOP_MSP_T ms;
            uint8  recvbuf[4] = {0};
            uint16 recLen = 0;
            uint16 len = 0;
            struct stat statbuf = {0};
            uint32 fileSize = 0;

            int    PackLen = 200;
            uint8  lenoffset = 0;
     
            MEMZERO(&ms, sizeof(ms));

            dar = apdu_ms_fromfmt698(srcBuf, srcLen, &sOffset, &ms, sizeof(ms));
            if (dar != DATA_SUCCESS)
            {
                PRTL_BUF_DEBUG(srcBuf, srcLen, "apdu_csd_fromfmt698 failed. dar(%d), offset(%d)", dar, offset);
                return DATA_TYPE_UNMATCHED;           
            }

            if (stat(UPDATNEW_PACK_NEW, &statbuf) < 0)
            {
                PRTL_FMT_DEBUG("获得不到文件[%s]大小\n", UPDATNEW_PACK_NEW);
                return DATA_READ_WRITE_DENIED;
            }
            else
            {
                PRTL_FMT_DEBUG("获得到文件[%s]大小[%d]\n", UPDATNEW_PACK_NEW, statbuf.st_size);
                fileSize = statbuf.st_size;
            }

            sprintf(filepath, "%s", UPDATNEW_PACK_NEW);
            len = strlen(filepath) + 1;
            lenoffset = set_len_offset(len, msgdata + offset);
            if (lenoffset <= 0)
            {
                PRTL_FMT_DEBUG("设备file[%s] 路径太长 len[%d]\n", filepath, len);
                return DATA_SCOPE_OF_ACCESS_VIOLATED;
            }
            else
            {
                PRTL_FMT_DEBUG("设备file[%s] 路径长度[%d]\n", filepath, len);
            }
            
            offset += lenoffset;
            
            memcpy(msgdata + offset, filepath, strlen(filepath));
            offset += len;

            memcpy_r(msgdata + offset, &fileSize, sizeof(fileSize));
            offset += sizeof(fileSize);

            memcpy_r(msgdata + offset, &PackLen, sizeof(PackLen));
            offset += sizeof(PackLen);

            if (ms.choice == 3)
            {
                len = ms.msa.size;
                lenoffset = set_len_offset(len, msgdata + offset);
                if (lenoffset <= 0)
                {
                    PRTL_FMT_DEBUG("地址太多 ms.choice[%d] len[%d]\n", ms.choice, ms.msa.size);
                    return DATA_SCOPE_OF_ACCESS_VIOLATED;
                }
                
                offset += lenoffset;

                for (i = 0; i < len; i++)
                {
                    if (ms.msa.add[i].type == 0)
                    {
                        msgdata[offset ++] = 0;
                    }
                    else if (ms.msa.add[i].type == 2)
                    {
                        msgdata[offset ++] = 1;
                    }
                    else if (ms.msa.add[i].type == 3)
                    {
                         msgdata[offset ++] = 2;
                    }
                    else
                    {
                        PRTL_FMT_DEBUG("地址类型错误 不应该为通配地址[%d]\n", ms.msa.add[i].type);
                        return DATA_ADDRESS_EXCEPTION;
                    }
                    
                    memcpy(&msgdata[offset], ms.msa.add[i].add, 6);
                    offset += 6;
                }
            }
            else
            {
                PRTL_FMT_DEBUG("ms集合类型不支持 ms.choice[%d] len[%d]\n", ms.choice, ms.msa.size);
            }
            
            msglen = offset;

            PRTL_BUF_DEBUG(msgdata, msglen, "消息发送");
            ret = appmain_send_proxy_to_taskManager(msgdata, msglen, recvbuf, 4, &recLen);
            if ((ret != 0) || (recvbuf[0] != 1))
            {
                PRTL_BUF_DEBUG(recvbuf, recLen, "消息回码失败 ret[%d] file[%s] recv", ret, filepath);

                //不判回码
                //return DATA_SCOPE_OF_ACCESS_VIOLATED;
            }
        }
        break;
    }
    return dar;
}

/**
*********************************************************************
* @name：       class18_F001_excute_update
* @brief：      class19的ACT服务
* @param[in] ：pOdaInfo        OAD信息
               pBpInfo        应用分帧断点信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
               outDataMax     输出报文缓冲区最大长度

* @param[out]：outData     输出报文
* @return    ：OOP_DAR_E
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E class18_F001_excute_update_new(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    uint32    sOffset = *convert->sOffset;
    uint8    *srcBuf = (uint8*)convert->srcBuf;
    uint8     choice = DT_NULL;
    OOP_OAD_U oad;

    if (srcBuf[sOffset++] != DT_STRUCTURE)
    {
        return DATA_TYPE_UNMATCHED;
    }

    if (srcBuf[sOffset++] != 3)
    {
        return DATA_OTHER_REASON;
    }

    choice = srcBuf[sOffset++];

    if (choice == DT_OAD)
    {
        memcpy_r((uint8*)&oad, &srcBuf[sOffset], 4);
        sOffset = sOffset + 4;
        if (((oad.value & 0xFFFFFF00) == 0xF20D0200)||(oad.nObjID == 0x4040))
        {
            PRTL_FMT_DEBUG("模组代发升级 oad[0x08X]\n", oad.value);
            return class18_F001_excute_update_other(pOadInfo, convert);
        }
    }
    
    return class18_F001_excute_update_ms(pOadInfo, convert);
}

/**
*********************************************************************
* @name：       class18_F001_excute_update
* @brief：      class19的ACT服务
* @param[in] ：pOdaInfo        OAD信息
               pBpInfo        应用分帧断点信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
               outDataMax     输出报文缓冲区最大长度

* @param[out]：outData     输出报文
* @return    ：OOP_DAR_E
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E class18_F001_excute_update(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    uint32    sOffset = *convert->sOffset;
    uint8    *srcBuf = (uint8*)convert->srcBuf;
    uint8     choice = DT_NULL;

    choice = srcBuf[sOffset];

    //兼容老代发协议
    switch (choice)
    {
    case DT_ROAD:
    case DT_OAD:
    case DT_MS:
    case DT_TSA:
        return class18_F001_excute_update_old(pOadInfo, convert);
    default:
        return class18_F001_excute_update_new(pOadInfo, convert);
    }
}

#endif

OOP_DAR_E class18_F001_check_file(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    int       ret;
    OOP_DAR_E dar = DATA_OTHER_REASON;
    uint32    sOffset = *convert->sOffset;
    uint8     *srcBuf = (uint8*)convert->srcBuf;
    uint32    srcLen = convert->srcLen;
	uint8 	  *dstBuf = (uint8 *)convert->dstBuf;
    uint32 	  dOffset = *convert->dOffset; 
	uint32    startAddr = 0;
	uint32    endAddr = 0;
	uint8     msgdata[100] = {0};
	uint8     type = 0;
    OOP_VISIBLEVAR1K_T   filepath;
    uint8  bytesnum = 0;
    uint16 len = 0;

    PRTL_BUF_DEBUG(srcBuf, 10, "sOffset[%d], F0010400命令:", sOffset);

	memset(&filepath, 0, sizeof(filepath));
	
	if(srcBuf[sOffset++]!=DT_STRUCTURE)
    {
        return DATA_TYPE_UNMATCHED;
    }

    if(srcBuf[sOffset++]!=4)
    {
        return DATA_TYPE_UNMATCHED;
    }
    
    //校验文件名
    if(srcBuf[sOffset++]  != DT_VISIBLE_STRING)
    {
        return DATA_TYPE_UNMATCHED;
    }
    GET_LEN_OFFSET(len, srcBuf + sOffset, bytesnum);
    sOffset += bytesnum;
    filepath.nNum = len;
    memcpy(filepath.value, srcBuf + sOffset, len);
    sOffset += len;

    if(srcBuf[sOffset++] != DT_ENUM)
    {
        return DATA_TYPE_UNMATCHED;
    }
    type = srcBuf[sOffset++];

    if(srcBuf[sOffset++] != DT_DOUBLE_LONG_UNSIGNED)
    {
        return DATA_TYPE_UNMATCHED;
    }
    MEMCPY_R(&startAddr, srcBuf + sOffset, 4);
    sOffset += 4;

    if(srcBuf[sOffset++] != DT_DOUBLE_LONG_UNSIGNED)
    {
        return DATA_TYPE_UNMATCHED;
    }
    MEMCPY_R(&endAddr, srcBuf + sOffset, 4);
    sOffset += 4;

    PRTL_FMT_DEBUG("校验文件%s, 方法%d, 范围[%u, %u]\n", filepath.value, type, startAddr, endAddr);

    switch(type)
    {
        case 0:
        {
			//TODO
        }
        break;

        case 1:
        {
            ret = Md5Region(filepath.value, startAddr,endAddr,msgdata);
            if (ret != 1)
            {
                PRTL_FMT_DEBUG("Md5Region error \n", sOffset, srcLen);
                return DATA_SCOPE_OF_ACCESS_VIOLATED;        
            }

			dstBuf[dOffset++] = 16;
			memcpy(&dstBuf[dOffset], msgdata, 16);
			dOffset +=16;

			*convert->dOffset = dOffset;

			dar = DATA_SUCCESS;
        }
        break;
        case 3:
        {
            //校验算法sm3算法
            ret = Sm3Region(filepath.value, startAddr,endAddr,msgdata);
            if (ret != 1)
            {
                PRTL_FMT_DEBUG("sm3Region error \n");
                return DATA_SCOPE_OF_ACCESS_VIOLATED;        
            }

			dstBuf[dOffset++] = 32;
			memcpy(&dstBuf[dOffset], msgdata, 32);
			dOffset +=32;

			*convert->dOffset = dOffset;

			dar = DATA_SUCCESS;
        }
        break;
    }
    return dar;
}

int32 class18_F001_write_file(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    uint16    blocknum = 0;
    uint32    dataLen=0;
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32    sOffset = *convert->sOffset;
    int32     ret = -1;
    uint8     *srcBuf = (uint8*)convert->srcBuf;
    uint32    srcLen = convert->srcLen;
    uint16    file_pack_index;
    char      cmd[1000] = {0};
    uint8     updateStatus;
    uint32    file_write_offset = 0;
    char     *tempdata = NULL;

    if (gUpInfoD_F001.file_Pack_size == 0 || gUpInfoD_F001.file_Pack_num == 0)
    {
        PRTL_FMT_DEBUG("文件传输包 gUpInfoD_F001.file_Pack_size[%d] gUpInfoD_F001.file_Pack_num[%d] err\n", 
            gUpInfoD_F001.file_Pack_size, gUpInfoD_F001.file_Pack_num);
        return DATA_LONG_GET_ABORTED;
    }

    if(srcBuf[sOffset++]!=DT_STRUCTURE)
    {
        return DATA_TYPE_UNMATCHED;
    }
    sOffset++;

    //块序号
    dar = basic_buf_to_data(E_LONG_UNSIGNED, srcBuf, srcLen, &sOffset, &blocknum, 2);
    if(DATA_SUCCESS != dar)
    {
        return dar;
    }

    //块数据
    if(srcBuf[sOffset++] != DT_OCTET_STRING)
    {
        return DATA_TYPE_UNMATCHED;
    }
    dataLen = srcBuf[sOffset++];
    if(dataLen ==0x81)
    {
        dataLen = srcBuf[sOffset++];
    }
    else if(dataLen == 0x82)
    {
        dataLen = srcBuf[sOffset++];
        dataLen <<= 8;
        dataLen += srcBuf[sOffset++];
    }
    else if(dataLen == 0x83)
    {
        dataLen = srcBuf[sOffset++];
        dataLen <<= 8;
        dataLen += srcBuf[sOffset++];
        dataLen <<= 8;
        dataLen += srcBuf[sOffset++];
    }
    else if(dataLen>0x83||dataLen==0x80)
    {
        return DATA_TYPE_UNMATCHED;
    }

    file_pack_index = blocknum;
    if(0 == gUpInfoD_F001.lastadd)
    {
        file_write_offset = file_pack_index * gUpInfoD_F001.file_Pack_size;
        gUpInfoD_F001.lastend = file_write_offset + dataLen;
        if(blocknum + 1 == gUpInfoD_F001.file_Pack_num && gUpInfoD_F001.lastend < gUpInfoD_F001.file_size)
        {
            PRTL_FMT_DEBUG("不能凑成整K帧, 补发最后一帧, 数量加1\n");
            gUpInfoD_F001.file_Pack_num += 1; //需要再加1
            gUpInfoD_F001.lastadd = 1;
        }
    }
    else
    {
        file_write_offset = gUpInfoD_F001.lastend;
        gUpInfoD_F001.lastend = file_write_offset + dataLen;
    }

#if (defined AREA_ZHEJIANG)|| (defined AREA_HUNAN)
    tempdata = malloc(dataLen);
    if(NULL == tempdata)
    {
        return DATA_OTHER_REASON;
    }
    memcpy_r(tempdata, srcBuf + sOffset, dataLen);

    PRTL_BUF_TRACE(tempdata, dataLen, "文件传输帧:");

    PRTL_FMT_DEBUG("文件传输包file_pack_index[%d], writeBlock.data.nNum[%d], file_write_offset[%d], gUpInfoD_F001.file_Pack_size[%d]\n",
        file_pack_index, dataLen, file_write_offset, gUpInfoD_F001.file_Pack_size);
    if (WriteUpdateTarREV(tempdata, file_write_offset, dataLen))//写文件成功
#else
    PRTL_BUF_TRACE(srcBuf + sOffset, MAX(100, dataLen), "文件传输帧(打印100个字节看看):");
    PRTL_FMT_DEBUG("文件传输包file_pack_index[%d], file_write_offset[%d], gUpInfoD_F001.file_Pack_size[%d]\n",
        file_pack_index, file_write_offset, gUpInfoD_F001.file_Pack_size);
    if (WriteUpdateTar((char *)srcBuf + sOffset, file_write_offset, dataLen))
#endif
    {
        int   m_index = 0;                                          //块序号
        uint8 m_bit = 0;                                            //位号

        if(tempdata)
        {
            free(tempdata);
        }

        m_index = MASK_INDEX_F001(file_pack_index);                 //块序号
        m_bit   = 0x01 << (7-MASK_BIT_F001(file_pack_index));       //位号
        if((gUpInfoD_F001.pPacksMask[m_index] & m_bit) == 0)      //如果曾经写过了，就不需要增加成功块数；否则需要写入
        {
            uint16 progress = 0;                                    //文件传输进度
            char   str[128] = {0};

            gUpInfoD_F001.file_succ_Pack_num ++;                   //接收成功的块数
            if(gUpInfoD_F001.file_succ_Pack_num > gUpInfoD_F001.file_Pack_num)
                gUpInfoD_F001.file_succ_Pack_num = gUpInfoD_F001.file_Pack_num;
            gUpInfoD_F001.pPacksMask[m_index] |= m_bit;           //修改对应位
            WriteUpdateFile(UPDATNEW_MASK_698, gUpInfoD_F001.pPacksMask, 0, gUpInfoD_F001.mask_len);        //接收掩码

            // 显示文件传输进度
            progress = gUpInfoD_F001.file_succ_Pack_num * 100 / gUpInfoD_F001.file_Pack_num;
            if ((progress % 2) == 0)
            {
                sprintf(str, "远程文件传输进度%d%%", progress);
                ret = appmain_refresh_status_bar(30, str);
                if (0 != ret)
                {
                    PRTL_FMT_DEBUG("gui err ret[%d]\n", ret);
                }
            }
        }
    }
    else
    {
        if(tempdata)
        {
            free(tempdata);
        }
        PRTL_FMT_DEBUG("0xF001 write block offset is %d, datalen is %d\n", blocknum, dataLen);
        return DATA_READ_WRITE_DENIED;
    }
                
    PRTL_FMT_DEBUG("gUpInfoD_F001.file_succ_Pack_num[%d], gUpInfoD_F001.file_Pack_num[%d]\n", gUpInfoD_F001.file_succ_Pack_num,gUpInfoD_F001.file_Pack_num);
    if(gUpInfoD_F001.file_succ_Pack_num == gUpInfoD_F001.file_Pack_num)     //文件传输包下载完毕
    {
        int i = 0;
        int bFullFlag = 1;
        int lastBlockMaxIndex  = (gUpInfoD_F001.file_Pack_num-1) % 8;//最后一包在字节中的索引
        PRTL_FMT_DEBUG("gUpInfoD_F001.mask_len is %d\n",gUpInfoD_F001.mask_len);
        if (gUpInfoD_F001.mask_len == 1)//如果只有一段 ，第一段就是最后一段，需要判断
        {
            for(i = 0;i <= lastBlockMaxIndex; i++)
            {
                if ((gUpInfoD_F001.pPacksMask[0] & (1<<(7-i))) == 0)
                {
                    bFullFlag = 0;
                    break;
                }
            }
        }
        else
        {
            int maxIndex = gUpInfoD_F001.mask_len - 1;
            for (i = 0; i < maxIndex; i++)
            {
                PRTL_FMT_DEBUG("gUpInfoD_F001.pPacksMask[%d] is %d\n", i ,gUpInfoD_F001.pPacksMask[i]);
                if(gUpInfoD_F001.pPacksMask[i] != 0xff)       //最后一块存在补零的情况，所以单独判断，任何一块没收全，都不允许文件传输
                {
                    bFullFlag = 0;
                    break;
                }
            }
            if (bFullFlag == 1)
            {
                for (i = 0; i <= lastBlockMaxIndex; i++)
                {
                    PRTL_FMT_DEBUG("gUpInfoD_F001.pPacksMask[maxIndex] is %d\n", gUpInfoD_F001.pPacksMask[maxIndex]);
                    if ((gUpInfoD_F001.pPacksMask[maxIndex] & (1<<(7-i))) == 0)
                    {
                        bFullFlag = 0;
                        break;
                    }
                }
            }
        }
        PRTL_FMT_DEBUG("传输成功标志 bFullFlag is %d\n", bFullFlag);
        if (bFullFlag == 1)
        {
            int32 ret = -1;
            PRTL_FMT_DEBUG("传输成功，拷贝文件\n");
            ret = appmain_refresh_status_bar(60, "传输成功");
            if (0 != ret)
            {
                PRTL_FMT_DEBUG("gui err ret[%d]\n", ret);
            }
            if(gUpInfoD_F001.file_name[0] == '/' || gUpInfoD_F001.file_name[0] == '.')
            {
                sprintf(cmd, "cp -f %s %s", UPDATNEW_PACK, gUpInfoD_F001.file_name);
            }
            else
            {
                sprintf(cmd, "cp -f %s %s/%s", UPDATNEW_PACK, UPDATNEW_PATH, gUpInfoD_F001.file_name);
            }
            PRTL_FMT_DEBUG("%s\n", cmd);
            system(cmd);    //将文件传输包从下载文件夹中拷贝到相应目录
            MEMZERO(cmd, sizeof(cmd));

            //同时存一份到默认路径上，方便后面文件代发
            if (strcmp(UPDATNEW_NAME, gUpInfoD_F001.file_name) != 0)
            {
                sprintf(cmd, "cp -f %s %s/%s", UPDATNEW_PACK, UPDATNEW_PATH, UPDATNEW_NAME);
                PRTL_FMT_DEBUG("%s\n", cmd);
                system(cmd);    //将文件传输包从下载文件夹中拷贝到相应目录
                MEMZERO(cmd, sizeof(cmd));
            }
            
            updateStatus = 100; //文件传输完成
            write_pdata_xram(TRANSFILE_PARAM, (char *)&updateStatus, TransFileParaOffset[ORDER_RESULT], 1);
            gUpInfoD_F001.file_succ_Pack_num = 0;
            gUpInfoD_F001.update_state = FALSE;//文件传输完成，退出文件传输状态，初始化的时候会清空文件传输信息
            WriteUpdateFile(UPDATNEW_FILE_698, (uint8*)&gUpInfoD_F001, 0, sizeof(gUpInfoD_F001));//写入文件传输信息

            #if 0
            ret = AddFileList(gUpInfoD_F001);
            if (ret < 0)
            {
                PRTL_FMT_DEBUG("AddFileList err. ret[%d]\n", ret);
                return DATA_OTHER_REASON;
            }
            #endif

            if ((strcmp(gUpInfoD_F001.file_name, UPDATNEW_NAME) == 0) && (gUpInfoD_F001.file_type == 0))
            {
                PRTL_FMT_DEBUG("start run sysUpdate.sh %s. gUpInfoD_F001.file_name[%s]\n", ADV_APP_NAME, gUpInfoD_F001.file_name);
                sprintf(cmd, "/usr/local/extapps/sysUpdate/bin/sysUpdate.sh %s &", ADV_APP_NAME);
                if (exec_system_cmd(cmd) != 0)
                {
                    PRTL_FMT_DEBUG("命令执行失败, 可能是超时 cmd:%s\n", cmd);
                }

                #if 0
                ret = DelFileList(gUpInfoD_F001);
                if (ret < 0)
                {
                    PRTL_FMT_DEBUG("DelFileList err. ret[%d]\n", ret);
                    return DATA_OTHER_REASON;
                }
                #endif
            }
            sprintf(cmd, "rm -f %s", UPDATNEW_PACK);
            PRTL_FMT_DEBUG("%s\n", cmd);
            system(cmd);    //将临时文件删除
        }
    }

    return dar;
}


OOP_DAR_E class18_F001_read_file(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint32 inLen, RESULT_DATA_T *outData, uint16 outDataMax )
{
    uint16    blocknum = 0;
    OOP_DAR_E dar = DATA_SUCCESS;
    int32     ret = -1;
    uint32    offset=0;
    uint32 apduHeadlen = 0;
    uint32 apduMaxLen = 0;
    uint8 *databuf = NULL;
    uint32 datalen = 0;  //实际长度
    uint8  bytelen = 0;
    uint32 startPosition = 0;    //偏移位置
    uint8 *dataInput = (uint8 *)inData + 4;

    if (gUpInfoD_F001.file_Pack_size == 0  )
    {
        PRTL_FMT_DEBUG("文件传输包 gUpInfoD_F001.file_Pack_size[%d] gUpInfoD_F001.file_Pack_num[%d] err\n", 
            gUpInfoD_F001.file_Pack_size, gUpInfoD_F001.file_Pack_num);
        return DATA_LONG_GET_ABORTED;
    }

    if(dataInput[offset++]!=DT_STRUCTURE)
    {
    	PRTL_FMT_DEBUG("类型不匹配11\n");
        return DATA_TYPE_UNMATCHED;
    }
    offset++;

    //块序号
    if(DT_LONG_UNSIGNED != dataInput[offset++])
    {
    	PRTL_FMT_DEBUG("类型不匹配22\n");
        return DATA_TYPE_UNMATCHED;
    }
    memcpy_r((uint8*)&blocknum, (uint8*)&dataInput[offset], 2);
    startPosition=blocknum*gUpInfoD_F001.file_Pack_size;
    apduHeadlen = 1 + 1 + 1 + 4 + 1 + 1 + 1 + 5;
    //apduHeadlen, data, time...
    apduMaxLen = apduHeadlen + gUpInfoD_F001.file_Pack_size + 2; 
    databuf = (uint8*)malloc(apduMaxLen);
    if(NULL == databuf)
    {
        PRTL_FMT_DEBUG("File get 0xF0007F00: malloc failed\n");
        return DATA_OTHER_REASON;
    }
       /* 3. 读取指定长度的内容 */
	PRTL_FMT_DEBUG("UpInfoD_F001.srcfilename %s,gUpInfoD_F001.file_Pack_size is %d\n",gUpInfoD_F001.srcfilename,gUpInfoD_F001.file_Pack_size);
    ret = ReadUpdateTar((const char *)gUpInfoD_F001.srcfilename, (char *)databuf + apduHeadlen, (int)startPosition, (int)gUpInfoD_F001.file_Pack_size);
    PRTL_FMT_DEBUG("File get 0xF0007F00: read update file ret %d\n", ret);
    if(ret <= 0)
    {
        free(databuf);
        return DATA_READ_WRITE_DENIED;
    }

    datalen = (uint32)ret;
    PRTL_BUF_DEBUG(databuf + apduHeadlen, MIN(100, datalen), "read data len %d\n", datalen);
    PRTL_BUF_DEBUG(databuf + apduHeadlen + datalen - MIN(100, datalen), MIN(100, datalen), "last data\n");

    /* 4. 封装报文 */
    if(datalen < pApduInfo->connectionInfo.sendFrameLen - 15)
    {
        // 4.1 正常长度 走正常流程
        uint8  *temp = outData->pResultData;
        uint8   bytelen = 0;

        offset = 0;
        memcpy_r(temp, &pOadInfo->pTab->oad.value, 4);
        offset += 4;
        temp[offset++] = 0x00;
        temp[offset++] = 0x01;
		temp[offset++] = DT_STRUCTURE;
		temp[offset++] = 0x01;
        temp[offset++] = DT_OCTET_STRING;

        bytelen = set_len_offset((uint16)datalen, temp + offset);
        offset += bytelen;

        memcpy(temp + offset, databuf + apduHeadlen, datalen);
        offset +=  datalen;
        outData->resultDataLen = offset;
    }
    else
    {
        // 4.2 扩展长度 先计算apdu允许长度
        FRAME_HEAD_T tframe;
        uint32 templen = 0;
        uint8 *pAddr = NULL;
        uint8  byte[5] = {0};
        uint32 newmsglen = 0;
        uint8 *pNewmsg = 0;
        uint32 apduleft = 0;

        memcpy(&tframe, pApduInfo->bpBreakInfo->frameaddr, sizeof(FRAME_HEAD_T));
        tframe.bExtmsg = TRUE;
        tframe.sAddr.len = app_get_localaddr(&pAddr);
        if(0 == tframe.sAddr.len)
        {
            free(databuf);
            return 0;
        }
        memcpy(tframe.sAddr.addr, pAddr, tframe.sAddr.len);
        tframe.prm = 1;
        tframe.dir = 1;

        tframe.headlen = APUD_COUNT_FRAME_LEN(tframe.sAddr.len, tframe.sAddr.logicAddr);

        /* 计算消息长度 */
        bytelen = set_biglen_offset(datalen, byte);
        templen = tframe.headlen + (apduHeadlen + datalen + 2 + bytelen - 5) + APP_698_CS_REGIN_SIZE;
        tframe.msglen = templen - templen % 1024;
        PRTL_FMT_DEBUG("msglen check templen %d, headlen %d, msglen %d\n",templen, tframe.headlen, tframe.msglen);

        tframe.apduLen = tframe.msglen - tframe.headlen - APP_698_CS_REGIN_SIZE;

        /* 计算新的datalen 1024倍数大小情况下 bytelen大小不变 */
        bytelen = set_biglen_offset(tframe.apduLen - apduHeadlen, byte);
        datalen = tframe.apduLen - (apduHeadlen + 2 + bytelen - 5);

        /* 封装 APDU */
        offset = apduHeadlen;
        databuf[datalen + offset] = 0;
        databuf[datalen + offset + 1] = 0;

        bytelen = set_biglen_offset(datalen, byte);
        offset -= bytelen;
        memcpy(databuf + offset, byte, bytelen); //len

        databuf[--offset] = DT_OCTET_STRING;
		databuf[--offset] = 01;
		databuf[--offset] = DT_STRUCTURE;
        databuf[--offset] = 1;    //操作返回数据 OPTIONAL
        databuf[--offset] = 0x00; //操作执行结果
        offset -= 4;
        memcpy_r(databuf + offset, &pOadInfo->pTab->oad.value, 4); //OMD
        databuf[--offset] = pApduInfo->bpBreakInfo->piid;  //PIID
        databuf[--offset] = 1;  //ActionResponseNormal
        databuf[--offset] = SERVERID_ACTIONRESPONSE;  //ACTION-Response

        tframe.pApdu = databuf + offset;

        /* 大报文 组装后 走直接发送流程 */
        newmsglen = app_frame_to_bigpackage(&tframe, &pNewmsg, tframe.msglen, &apduleft, FALSE);
        ret = send_bigpackage_master(pApduInfo->bpBreakInfo->channelid, pNewmsg, newmsglen);  
        PRTL_FMT_DEBUG("newmsglen is %d, apduleft %d, sendret %d, offset %d\n",newmsglen, apduleft, ret, offset);
        if(0 != ret)
        {
            free(databuf);
            free(pNewmsg);
            memset(outData->pResultData, 0, 4);
            return DATA_OTHER_REASON;
        }
        /* 特殊约定前4个字节是ff, 不返回 */
        memset(outData->pResultData, 0xff, 4);
        outData->resultDataLen = 0;
        free(pNewmsg);
    }

    free(databuf);
 
    return dar;
}

int32 buf_to_data_class18_F001_program_compare(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_SOFTCOMPARE_T SOFTCOMPARE_T;
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;
    
    memset((uint8*)&SOFTCOMPARE_T,0x00,sizeof(OOP_SOFTCOMPARE_T));
    if(srcBuf[sOffset++]!=DT_STRUCTURE)
    {
        return DATA_TYPE_UNMATCHED;
    }
    sOffset++;
    //文件信息
    if(srcBuf[sOffset++]!=DT_STRUCTURE)
    {
        return DATA_TYPE_UNMATCHED;
    }
    sOffset++;
    //CPU编号
    dar=basic_buf_to_data(E_UNSIGNED,srcBuf,srcLen,&sOffset,&SOFTCOMPARE_T.cpu,1);
    if(dar != DATA_SUCCESS)
    {
        //PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }   
    //密钥索引
    dar=basic_buf_to_data(E_UNSIGNED,srcBuf,srcLen,&sOffset,&SOFTCOMPARE_T.idx,1);
    if(dar != DATA_SUCCESS)
    {
        //PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }   
    //因子起始地址
    dar=basic_buf_to_data(E_DOUBLE_LONG_UNSIGNED,srcBuf,srcLen,&sOffset,&SOFTCOMPARE_T.factorAdd,4);
    if(dar != DATA_SUCCESS)
    {
        //PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }   
    //数据起始地址
    dar=basic_buf_to_data(E_DOUBLE_LONG_UNSIGNED,srcBuf,srcLen,&sOffset,&SOFTCOMPARE_T.dataAdd,4);
    if(dar != DATA_SUCCESS)
    {
        //PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }   
    //待加密数据长度
    dar=basic_buf_to_data(E_LONG_UNSIGNED,srcBuf,srcLen,&sOffset,&SOFTCOMPARE_T.dataAdd,4);
    if(dar != DATA_SUCCESS)
    {
        //PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }   

    memcpy(&dstBuf[dOffset], &SOFTCOMPARE_T, sizeof(OOP_SOFTCOMPARE_T));
    dOffset += sizeof(OOP_SOFTCOMPARE_T);

    //避免每步都判断，放最后检查，但检查前可能已经越界
    if((sOffset > srcLen) || (dOffset > dstLen))
    {
        //PRTL_FMT_DEBUG("cross the border. sOffset(%d), srcLen(%d), dOffset(%d), dstLen(%d)\n", sOffset, srcLen, dOffset, dstLen);
        return DATA_SCOPE_OF_ACCESS_VIOLATED;        
    }

    *convert->dOffset = dOffset;
    *convert->sOffset = sOffset;

    return dar;
}

/**
*********************************************************************
* @name：      Class18_F001Action
* @brief：     class19的ACT服务
* @param[in] ：pOdaInfo        OAD信息
               pBpInfo        应用分帧断点信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
               outDataMax     输出报文缓冲区最大长度

* @param[out]：outData     输出报文
* @return    ：OOP_DAR_E
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E Class18_F001Action(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint32 inLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    NOMAL_OAD_T normal;
    uint8 data[BUF_LEN_MAX] = {0};
    DATA_CONVERT_T convert;
    uint32 sOffset = 4;
    uint32 dOffset = 0;
    uint32 offset  = 0;

    MEMZERO(&data, sizeof(data)); 
    MEMZERO(&convert, sizeof(convert)); 
    MEMZERO(&normal, sizeof(normal)); 

    convert.srcBuf = (uint8 *)inData;
    convert.srcLen = inLen;
    convert.sOffset = &sOffset;

    convert.dstBuf = &data;
    convert.dstLen = sizeof(data);
    convert.dOffset = &dOffset;  
    
    switch(pOadInfo->pTab->oad.value)
    {
    	case 0xF0010400://校验
        {
            PRTL_FMT_DEBUG("Class18_F001Action send file to others\n");
            dar = class18_F001_check_file(pOadInfo, &convert);
            if (dar != DATA_SUCCESS)
            {
                PRTL_FMT_DEBUG("excute_update_file_trans err. dar[%d]\n", dar);
                return dar;
            }
			
			memcpy(&outData->pResultData[offset], inData, sizeof(OOP_OAD_U));  //OMD
    		offset += sizeof(OOP_OAD_U);
			outData->pResultData[offset++] = 0x00;
            outData->pResultData[offset++] = 0x01;
            outData->pResultData[offset++] = OCTET_STRING;
			memcpy(&outData->pResultData[offset], data, dOffset);  
            offset += dOffset;
            outData->resultDataLen = offset;
           
            return DATA_SUCCESS;
        }
        case 0xF0010500://代发
        {
            PRTL_FMT_DEBUG("Class18_F001Action send file to others\n");
            dar = class18_F001_excute_update(pOadInfo, &convert);
            if (dar != DATA_SUCCESS)
            {
                PRTL_FMT_DEBUG("excute_update_file_trans err. dar[%d]\n", dar);
                return dar;
            }
            break;
        }
        case 0xF0010700://启动文件传输
        { 
            dar = class18_F001_start_update(pOadInfo, &convert);
            if (dar != DATA_SUCCESS)
            {
                PRTL_FMT_DEBUG("class18_F001_start_update err. dar[%d]\n", dar);
                return dar;
            }
            break;
        }
        case 0xF0010800://写文件
        {
            PRTL_FMT_DEBUG("Class18_F001Action writing file\n");
            dar = class18_F001_write_file(pOadInfo, &convert);
            if (dar != DATA_SUCCESS)
            {
                PRTL_FMT_DEBUG("class18_F001_write_file err. dar[%d]\n", dar);
                return dar;
            }
            break;
        }   
        case 0xF0010900://读文件
        return class18_F001_read_file(pOadInfo, pApduInfo, inData, inLen, outData,outDataMax);
            break;
        #if SICHUAN_FUN == 1
        case 10://软件比对
        {
            char        cpuId = 0;                  //CPU编号
            char        keyIndex = 0;               //密钥索引
            long long   factorStartAddr = 0;        //因子起始地址
            long long   dataStartAddr = 0;          //数据起始地址
            short       enDataLen = 0;              //待加密数据长度
            int         i = 0;
            int         ret = ERR_OK;

            char        FilePath[] = "/mnt/bin/appmain";
            unsigned char    outData5[300]  = {0};
            unsigned char    outData10[300] = {0};
            struct  stat     buf;


            OOP_SOFTCOMPARE_T  SOFTCOMPARE_T;
            MEMZERO(&SOFTCOMPARE_T, sizeof(OOP_SOFTCOMPARE_T));
            dar = buf_to_data_class18_F001_program_compare(pOadInfo, convert);
            if (dar != DATA_SUCCESS)
            {
                
                return dar;
            }
            memcpy(&SOFTCOMPARE_T, convert.dstBuf, dOffset);
            
            cpuId=SOFTCOMPARE_T.cpu;
            keyIndex=SOFTCOMPARE_T.idx;
            factorStartAddr=SOFTCOMPARE_T.factorAdd;
            dataStartAddr=SOFTCOMPARE_T.dataAdd;
            enDataLen=SOFTCOMPARE_T.unencrypt;
            
            if(stat(FilePath, &buf) < 0)
            {
               
                return DATA_OTHER_REASON;
            }

            ret = XORalgorithm(FilePath, factorStartAddr, enDataLen, outData5);
            if(ret == 1)
            {
                //如果获取的比对因子中连续16字节为相同数据（例全00或全FF）时，电能表/终端应返回安全认证异常应答"地址异常"。
                if(JudgeDataEqual(outData5,enDataLen/4) == TRUE)
                    return 27;
                ret = XORalgorithm(FilePath, dataStartAddr, enDataLen, outData10);
                if(ret == 1)
                {
                    offset = 4;
                    outData->pResultData[offset++] = 0x01;
                    outData->pResultData[offset++] = STRUCTURE;
                    outData->pResultData[offset++] = 0x01;
                    outData->pResultData[offset++] = OCTET_STRING;
                    outData->pResultData[offset++] = enDataLen/4;
                    for(i = 0; i < enDataLen/4; i++)
                    {
                        outData->pResultData[offset+i] = outData5[i] ^ outData10[i];
                    }

                    offset += enDataLen/4;
                    outData->resultDataLen = offset;
                   
                    return DATA_SUCCESS;
                }
                else
                {
                  
                    return DATA_OTHER_REASON;
                }
            }
            else
            {
                
                return DATA_OTHER_REASON;
            }
            break;
            }
            #endif
    }
    memcpy(&outData->pResultData[offset], inData, sizeof(OOP_OAD_U));  //OMD
    offset += sizeof(OOP_OAD_U);
    outData->pResultData[offset ++] = 0x00;                            //DAR type
    outData->pResultData[offset ++] = 0x00;                            //DATA:NULL
    outData->resultDataLen = offset;

    return dar;
}

/**
*********************************************************************
* @name：      class19_invoke_get
* @brief：     class19的GET服务
* @param[in] ：pOdaInfo        OAD信息
               pBpInfo        应用分帧断点信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
               outDataMax     输出报文缓冲区最大长度

* @param[out]：outData     输出报文
* @return    ：OOP_DAR_E
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E Class18_F002Action(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 inLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    uint8 prolType = 0;
    uint8 index = 4;

    switch(pOadInfo->pTab->oad.value)
    {
        case 0xF0020700:
            if(inData[index]!=ENUM){
                return(DATA_HARDWARE_FAULT);
            }
            prolType = inData[index+1];
            if(prolType==1)
            {

            }
            break;
        default:
            break;

    }
    return(DATA_SUCCESS);//拒绝
}


/**
*********************************************************************
* @brief：      从数据中心的数据转698格式报文
* @param[in]： ppOadInfo     - 数据表指针
               srcBuf       - 源数据
               srcLen       - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen       - 目的数据长度
               sOffset      - 转换后源数据区的偏移
               dOffset      - 转换后目的数据区的偏移
* @return：     =0           - 成功
              !=0           - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_class18(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    return 0;
}

/**
*********************************************************************
* @brief：      从698格式报文转数据中心的数据
* @param[in]： ppOadInfo     - 数据表指针
               srcBuf       - 源数据
               srcLen       - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen       - 目的数据长度
               sOffset      - 转换后源数据区的偏移
               dOffset      - 转换后目的数据区的偏移
* @return：     =0           - 成功
              !=0           - 错误码
*********************************************************************
*/
OOP_DAR_E buf_to_data_class18(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    return 0;
}


#if DESC("class18对外接口函数",1)

/**
*********************************************************************
* @name：       class18_invoke_set
* @brief：      class18的SET服务
* @param[in] ：pOdaInfo        OAD信息
               pBpInfo        应用分帧断点信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
               outDataMax     输出报文缓冲区最大长度

* @param[out]：outData     输出报文
* @return    ：OOP_DAR_E
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E class18_invoke_set(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 inLen, RESULT_DATA_T *outData, uint16 outDataMax)
{

    if(pOadInfo->pTab->oad.attID==2){//接口公共属性
        return Class18_CommonSet(pOadInfo, pApduInfo, inData, inLen, outData, outDataMax);
    }

    switch(pOadInfo->pTab->oad.nObjID)
    {
        case 0xF000:
            return Class18_F000Set(pOadInfo, pApduInfo, inData, inLen, outData, outDataMax);;
            break;
        case 0xF001:
            return Class18_F001Set(pOadInfo, pApduInfo, inData, inLen, outData, outDataMax);;
            break;
        case 0xF002:
            return Class18_F002Set(pOadInfo, pApduInfo, inData, inLen, outData, outDataMax);;
            break;
        default:
            return(DATA_READ_WRITE_DENIED);//拒绝
    }
    return(DATA_READ_WRITE_DENIED);//拒绝
}


/**
*********************************************************************
* @name：       class18_invoke_get
* @brief：      class18的GET服务
* @param[in] ：pOdaInfo        OAD信息
               pBpInfo        应用分帧断点信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
               outDataMax     输出报文缓冲区最大长度

* @param[out]：outData     输出报文
* @return    ：OOP_DAR_E
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E class18_invoke_get(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 inLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    if(pOadInfo->pTab->oad.attID==2){//接口公共属性
        return Class18_CommonGet(pOadInfo, pApduInfo, inData, inLen, outData, outDataMax);
    }
    switch(pOadInfo->pTab->oad.nObjID)
    {
        case 0xF000:
            return Class18_F000Get(pOadInfo, pApduInfo, inData, inLen, outData, outDataMax);
            break;
        case 0xF001:
            return Class18_F001Get(pOadInfo, pApduInfo, inData, inLen, outData, outDataMax);
            break;
        case 0xF002:
            return Class18_F002Get(pOadInfo, pApduInfo, inData, inLen, outData, outDataMax);
            break;
        default:
            return(DATA_READ_WRITE_DENIED);//拒绝
    }
    return(DATA_READ_WRITE_DENIED);//拒绝
}


/**
*********************************************************************
* @name：       class18_invoke_act
* @brief：      class18的ACTION服务
* @param[in] ：pOdaInfo        OAD信息
               pBpInfo        应用分帧断点信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
               outDataMax     输出报文缓冲区最大长度

* @param[out]：outData     输出报文
* @return    ：OOP_DAR_E
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E class18_invoke_act(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint32 inLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    
    switch (pOadInfo->pTab->oad.nObjID)
    {
        case 0xF000:
            return Class18_F000Action(pOadInfo, pApduInfo, inData, inLen, outData, outDataMax);
            break;
        case 0xF001:
            return Class18_F001Action(pOadInfo, pApduInfo, inData, inLen, outData, outDataMax);
            break;
        case 0xF002:
            return Class18_F002Action(pOadInfo, pApduInfo, inData, inLen, outData, outDataMax);
            break;
        default:
            return(DATA_READ_WRITE_DENIED);//拒绝
    }
    return(DATA_READ_WRITE_DENIED);//拒绝
}
#endif

