/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved.
* @brief： 交采升级相关
* @date： 2019-10-25
*********************************************************************
*/

#include "ac_sample.h"
#include "ac_sample698.h"
#include "ac_lib.h"
#include "ac_main.h"
#include "ac_commdrv.h"
#include "ac_mqtt.h"
#include "ac_save.h"
#include "ac_update.h"

#if DESC("交采升级相关代码",1)

JC_UPDATE_CFG_T          g_iUpdateCfg;            // 交采升级
uint8                    g_InerSeq = 0;           // 内部帧序列号
uint8                    gJCUpdateFileMark = 0;   // 升级文件标识全局标记. 0x09-交采模块 0x0a-计量芯片 0x0b-拓扑模块
uint8                    gTryCnt = 0;

//版本信息相关变量
uint8                    gVerBuf[8] = { 0 };        // 存在交采版本信息 
uint8                    gVerBufLast[8] = { 0 };    // 存在上版交采版本信息 
uint8                    gDateBuf[8] = { 0 };       // 存在交采版本日期信息
uint8                    gDateBufLast[8] = { 0 };   // 存在上版交采版本日期信息
uint8                    gVerSBuf[8] = { 0 };     // 存在交采小版本信息 
uint8                    gVerSBufLast[8] = { 0 }; // 存在上版交采小版本信息 

/*******************************************************************************
* 函数名称:	ClearCommRecvBuff
* 函数功能:	清除通讯接收数据
* 输入参数:	fd  -  串口操作句柄
* 输出参数:	无
* 返 回 值:	无
*******************************************************************************/
void ClearCommRecvBuff(int32 fd)
{
    int     RecvLen = 0xff;                     //接收数据长度
    uint8   RecvBuf[1024];                      //接收数据缓冲区

    while (RecvLen > 0)
    {
        RecvLen = commdrv_Recv_232(fd, RecvBuf, sizeof(RecvBuf), 5, 5, 0xFF);
    }
}

/**
*********************************************************************
* @brief：     交采打印
* @param[in]： charInfo      - 提示信息
               buf           - 打印内容
               len           - 打印内容长度
* @param[out]：无
* @return：
*********************************************************************
*/
void JcCommFunPrintfBuf(char* charInfo, uint8* buf, int16 len)
{
    uint16 i = 0;

#if JC_UPDATE_BUF_LOG == 1
    uint8 logbuf[1024] = { 0 };
#endif

    ACPRINT("%s\n", charInfo);

    if (len <= 0)
    {
        ACPRINT("len <= 0 \n");
#if JC_UPDATE_BUF_LOG == 1
        writelog(AC_APP_LOG, "%s %s\n", charInfo, "len <= 0");
#endif
        return;
    }

    ACPRINT("len = %d ", len);

    for (i = 0; i < len; i++)
        ACPRINT("%02x ", buf[i]);
    ACPRINT("\n");

#if JC_UPDATE_BUF_LOG == 1
    memset(&logbuf[0], '\0', 2 * len);
    for (i = 0; i < len; i++)
    {
        sprintf(&logbuf[2 * i], "%02X ", buf[i]);
    }
    logbuf[2 * len] = 0x00;//补上'\0'
    writelog(AC_APP_LOG, "%s len = %d %s\n", charInfo, len, logbuf);

#endif
}

/**
*********************************************************************
* @brief：     查找str1种是不是包含str2
* @param[in]：
* @param[out]：无
* @return：
*********************************************************************
*/
BOOL jcIsIncludeName(uint8* str1, uint8* str2, uint16 len)
{
    uint16 i = 0;
    BOOL ret = FALSE;
    for (i = 0; i < (len - strlen((const char*)(str2))); i++)
    {
        if (memcmp(&str1[i], &str2[0], strlen((const char*)(str2))) == 0)
        {
            ret = TRUE;
            break;
        }
    }
    if (i >= len - strlen((const char*)(str2)))
    {
        ret = FALSE;
    }

    return ret;
}

/*******************************************************************************
* 函数名称: IsCloseInnerDelayState
* 函数功能: 是否可以结束任务内部延时状态
* 输入参数: int Condition, 产生累加的条件
            int *Cnt,      计数器，计数器应为静态变量
            int CntMax     计数器的最大值
* 输出参数: 无
* 返 回 值: >0                  成功
*           <0                  失败
*******************************************************************************/
int IsCloseInnerDelayState(int Condition, int* Cnt, int CntMax)
{
    if (Condition)
    {
        UPDATE_FMT_DEBUG("InnerDelayState(meet the condition %d)\n", (*Cnt));
        (*Cnt)++;
    }
    if ((*Cnt) >= CntMax)
    {
        (*Cnt) = 0;
        return 1;
    }
    return 0;
}

/**
*********************************************************************
* @brief：     查找升级文件
* @param[in]： 无
* @param[out]：无
* @return：    0  不存在
*              1  存在
*********************************************************************
*/
BOOL IsExistJcUpDateFile(void)
{
    int        fd = -1;
    int        size = 0;
    uint8      buffer[1024] = { 0 };
    BOOL       ret = FALSE;
    static int WaitTime = 0;
    char       TmpPath[256] = { 0 };
    sprintf(TmpPath, "rm -rf %s", JC_UPDATE_FILE_BACK);
    if ((access(JC_UPDATE_FILE, R_OK) == 0) && (gJCUpdateFileMark == 0x00))    //先判断主节点升级包
    {
        if (IsCloseInnerDelayState((gTimeFlag.BitSect.Second == TRUE), &WaitTime, 1) <= 0)
        {
            return FALSE;
        }

        //如果存在 JC_CTL_UPDATE_FILE_BACK 文件先将其删除
        if (access(JC_UPDATE_FILE_BACK, R_OK) == 0)
        {
            system(TmpPath);
            printf("\n%s\n", TmpPath);
        }

        if (rename(JC_UPDATE_FILE, JC_UPDATE_FILE_BACK) < 0)
        {
            ret = FALSE;
        }
        else
        {
            sleep(1);
            memset(buffer, 0x00, sizeof(buffer));
            fd = open(JC_UPDATE_FILE_BACK, O_RDONLY);
            if (fd <= 0)
            {
                UPDATE_FMT_DEBUG("IsExistJCUpDateFile open file error!\n");
                ret = FALSE;
                return ret;
            }
            else
            {
                UPDATE_FMT_DEBUG("IsExistJCUpDateFile open %s OK!\n", JC_UPDATE_FILE_BACK);
            }


            lseek(fd, -64, SEEK_END);

            size = read(fd, buffer, 64);

            if (size <= 0)
            {
                UPDATE_FMT_DEBUG("IsExistJCUpDateFile read %s error!\n", JC_UPDATE_FILE_BACK);
                ret = FALSE;
            }
            else
            {
                UPDATE_BUF_DEBUG(buffer, size, "升级文件尾部信息");
                if (1)
                {
                    ret = TRUE;
                    g_iUpdateCfg.iUpdateCnt = 0;
                    g_iUpdateCfg.iUpdateState = 0xAA;
                    g_iUpdateCfg.CurJcIndex = 0;
                    g_iUpdateCfg.iUpdateSuccCnt = 0;
                    gJCUpdateFileMark = 0x09;
                    memset(&g_iUpdateCfg, 0x0, sizeof(JC_UPDATE_CFG_T));
                }
                else
                {
                    ret = FALSE;
                    UPDATE_FMT_DEBUG("%s not include %s\n", JC_UPDATE_FILE_BACK, JC_FILE_NAME);
                }
            }

            close(fd);
        }
    }

    return ret;
}

/**
*********************************************************************
* @brief：     查找升级文件
* @param[in]： 无
* @param[out]：无
* @return：    0  不存在
*              1  存在
*********************************************************************
*/
BOOL IsExistJcUpDateFile_ic(void)
{
    int        fd = -1;
    int        size = 0;
    uint8      buffer[1024] = { 0 };
    BOOL       ret = FALSE;
    static int WaitTime = 0;
    char       TmpPath[256] = { 0 };
    sprintf(TmpPath, "rm -rf %s", JC_UPDATE_FILE_BACK);
    if ((access(JC_UPDATE_FILE_IC, R_OK) == 0) && (gJCUpdateFileMark == 0))    //先判断是否存在计量芯片升级包，且没有在升级模块
    {
        if (IsCloseInnerDelayState((gTimeFlag.BitSect.Second == TRUE), &WaitTime, 1) <= 0)
        {
            return FALSE;
        }

        //如果存在 JC_CTL_UPDATE_FILE_BACK 文件先将其删除
        if (access(JC_UPDATE_FILE_BACK, R_OK) == 0)
        {
            system(TmpPath);
            printf("\n%s\n", TmpPath);
        }

        if (rename(JC_UPDATE_FILE_IC, JC_UPDATE_FILE_BACK) < 0)
        {
            ret = FALSE;
        }
        else
        {
            sleep(1);
            memset(buffer, 0x00, sizeof(buffer));
            fd = open(JC_UPDATE_FILE_BACK, O_RDONLY);
            if (fd <= 0)
            {
                UPDATE_FMT_DEBUG("IsExistJCUpDateFile open file error!\n");
                ret = FALSE;
                return ret;
            }
            else
            {
                UPDATE_FMT_DEBUG("IsExistJCUpDateFile open %s OK!\n", JC_UPDATE_FILE_BACK);
            }


            lseek(fd, -64, SEEK_END);

            size = read(fd, buffer, 64);

            if (size <= 0)
            {
                UPDATE_FMT_DEBUG("IsExistJCUpDateFile read %s error!\n", JC_UPDATE_FILE_BACK);
                ret = FALSE;
            }
            else
            {
                UPDATE_BUF_DEBUG(buffer, size, "计量升级文件尾部信息");
#ifdef AREA_SHANDONG  //山东地区有升级计量芯片需求，不能判断XLDZ
                if (1)
#else
                if (jcIsIncludeName(buffer, (uint8*)(JC_FILE_NAME), size))
#endif
                {
                    ret = TRUE;
                    gJCUpdateFileMark = 0x0a;
                    memset(&g_iUpdateCfg, 0x0, sizeof(JC_UPDATE_CFG_T));
                }
                else
                {
                    ret = FALSE;
                    UPDATE_FMT_DEBUG("%s not include %s\n", JC_UPDATE_FILE_BACK, JC_FILE_NAME);
                }
            }

            close(fd);
        }
    }

    return ret;
}

/**
*********************************************************************
* @brief：     查找升级文件
* @param[in]： 无
* @param[out]：无
* @return：    0  不存在
*              1  存在
*********************************************************************
*/
BOOL IsExistJcUpDateFile_topo(void)
{
    int        fd = -1;
    int        size = 0;
    uint8      buffer[1024] = { 0 };
    BOOL       ret = FALSE;
    static int WaitTime = 0;
    char       TmpPath[256] = { 0 };
    sprintf(TmpPath, "rm -rf %s", JC_UPDATE_FILE_BACK);
    if ((access(JC_UPDATE_FILE_TOPO, R_OK) == 0) && (gJCUpdateFileMark == 0))    //先判断是否存在拓扑模块升级包，且没有在升级模块
    {
        if (IsCloseInnerDelayState((gTimeFlag.BitSect.Second == TRUE), &WaitTime, 1) <= 0)
        {
            return FALSE;
        }

        //如果存在 JC_CTL_UPDATE_FILE_BACK 文件先将其删除
        if (access(JC_UPDATE_FILE_BACK, R_OK) == 0)
        {
            system(TmpPath);
            printf("\n%s\n", TmpPath);
        }

        if (rename(JC_UPDATE_FILE_TOPO, JC_UPDATE_FILE_BACK) < 0)
        {
            ret = FALSE;
        }
        else
        {
            sleep(1);
            memset(buffer, 0x00, sizeof(buffer));
            fd = open(JC_UPDATE_FILE_BACK, O_RDONLY);
            if (fd <= 0)
            {
                UPDATE_FMT_DEBUG("拓扑升级文件打开失败 file[%s] fd[%d]\n", JC_UPDATE_FILE_BACK, fd);
                ret = FALSE;
                return ret;
            }
            else
            {
                UPDATE_FMT_DEBUG("拓扑升级文件打开成功 file[%s] fd[%d]\n", JC_UPDATE_FILE_BACK, fd);
            }


            //之前用于读尾部信息，现在不再芯片尾部信息，可以检查文件是否可读
            lseek(fd, -64, SEEK_END);
            size = read(fd, buffer, 64);
            if (size <= 0)
            {
                UPDATE_FMT_DEBUG("拓扑升级文件读错误 file[%s]\n", JC_UPDATE_FILE_BACK);
                ret = FALSE;
            }
            else
            {
                ret = TRUE;
                gJCUpdateFileMark = 0x0b;
                memset(&g_iUpdateCfg, 0x0, sizeof(JC_UPDATE_CFG_T));
            }

            close(fd);
        }
    }

    return ret;
}
/**
*********************************************************************
* @brief：     是否在升级中
* @param[in]： 无
* @param[out]：无
* @return：    0  不在升级
*              1  正在升级
*********************************************************************
*/
BOOL isJCUpdateRunning(void)
{
    static BOOL isRun = FALSE;
    if (IsExistJcUpDateFile())
    {
        isRun = TRUE;
    }
    else if (IsExistJcUpDateFile_ic())
    {
        isRun = TRUE;
    }
    else if (IsExistJcUpDateFile_topo())
    {
        isRun = TRUE;
    }
    
    if ((access(JC_UPDATE_FILE_BACK, R_OK) == 0) && (gJCUpdateFileMark != 0))
    {
        isRun = TRUE;
    }
    else
    {
        isRun = FALSE;
    }

    return isRun;
}

/**
*********************************************************************
* @brief：     是否需要获取版本信息
* @param[in]： 无
* @param[out]：无
* @return：    0  不需要
*              1  需要
*********************************************************************
*/
BOOL isJCNeedGetVer(void)
{
    BOOL isGetVer = FALSE;

    if ((gVerBuf[0] == 0) && (gVerBuf[1] == 0) &&
        (gVerBuf[2] == 0) && (gVerBuf[3] == 0))
    {
        isGetVer = TRUE;
    }
    else
    {
        isGetVer = FALSE;
    }

    //ACPRINT("isJCNeedGetVer isGetVer = %d verBuf:%X %X %X %X\n", isGetVer, verBuf[64], verBuf[65], verBuf[66], verBuf[67]);

    return isGetVer;
}

/**
*********************************************************************
* @brief：     清空版本信息
* @param[in]： 无
* @param[out]：无
* @return：    无
*********************************************************************
*/
void ac_clean_ver(void)
{
    memset(&gVerBuf[0], 0, sizeof(gVerBuf));
    gTryCnt = 0;

    return;
}

/**
*********************************************************************
* @brief：     电表规约组帧
* @param[in]： MeterData           电表规约数据
* @param[out]：PrtlData            组帧后数据
* @return：    帧长度
*********************************************************************
*/
int PackMeterPrtl(uint8* PrtlData, METER_PRTL_FMT_T MeterData)
{
    uint8   CheckSum = 0;                       //校验和
    int     i;
    uint16   CurIndex;                           //索引

#if 0//WHG_FUN == 1
    if (MeterData.PrtlMode == PRTL_WHG)
        return PackWHGMeterPrtl(PrtlData, MeterData);
#endif

    PrtlData[0] = 0x68;
    memcpy(&PrtlData[1], MeterData.MtrAddr, 6);
    PrtlData[7] = 0x68;
    PrtlData[8] = MeterData.CtrlCode;
    PrtlData[9] = MeterData.DataLen;

    for (i = 0; i < MeterData.DataLen; i++)
    {
        PrtlData[10 + i] = MeterData.DataBuff[i] + 0x33;
    }
    CurIndex = MeterData.DataLen + 10;

    for (i = 0; i < CurIndex; i++)
    {
        CheckSum += PrtlData[i];
    }
    PrtlData[CurIndex] = CheckSum;
    PrtlData[CurIndex + 1] = 0x16;

    CurIndex += 2;

    return CurIndex;
}


/**
*********************************************************************
* @brief：     模组模块升级组帧，并且发送到缓冲区
* @param[in]： fileBuf - 文件内容
               Len     - 内容长度
               Uis     - 文件升级信息
* @param[out]：sendbuf - 发送报文
* @return：    >0        发送长度
*              <=0       失败
*********************************************************************
*/
int MakeFrameUpdateJC_one(int32 fdcry, uint8* sendbuf/*, uint8 *fileBuf, uint16 Len, JcUpdateInfoState Uis*/)
{
    int                 PackLen = 220;
    struct stat         statbuf = { 0 };
    int                 PackNum = 0;
    JC_FRAME_TYPE_T     FrameInfo;
    METER_PRTL_FMT_T    MeterPrtlData;
    uint32              DI = 0x04A01101;
    uint16              len = 0;

    if (fdcry < 0)
    {
        UPDATE_FMT_DEBUG("传输后行帧的时候，找不到文件");
        return -9;
    }

    if (stat(JC_UPDATE_FILE_BACK, &statbuf) < 0)
    {
        UPDATE_FMT_DEBUG("传输后行帧的时候，获得不到文件大小");
        return -9;
    }

    //获得所有的包数，也即序列号
    if (statbuf.st_size % PackLen)
        PackNum = statbuf.st_size / PackLen + 1;
    else
        PackNum = statbuf.st_size / PackLen;

    memset(&FrameInfo, 0x0, sizeof(FrameInfo));

    if (g_iUpdateCfg.UpdateSeq >= PackNum)
        return -1;

    if (g_iUpdateCfg.ClearFlag == 1)
        FrameInfo.type = 01;
    else
        FrameInfo.type = 00;

    FrameInfo.kind = 00;
    if (g_iUpdateCfg.UpdateSeq == (PackNum - 1))
        FrameInfo.kind = 01;

    FrameInfo.cmd = 00;
    FrameInfo.seg = PackNum;
    FrameInfo.offset = g_iUpdateCfg.UpdateSeq;

    lseek(fdcry, g_iUpdateCfg.UpdateSeq * PackLen, SEEK_SET);

    if (g_iUpdateCfg.UpdateSeq == (PackNum - 1))
    {
        if (statbuf.st_size % PackLen)
            PackLen = (statbuf.st_size % PackLen);
        read(fdcry, FrameInfo.buf, PackLen);
    }
    else
        read(fdcry, FrameInfo.buf, PackLen);
    FrameInfo.len = PackLen;

    MeterPrtlData.MtrAddr[0] = 0xAA;//地址固定
    MeterPrtlData.MtrAddr[1] = 0xAA;
    MeterPrtlData.MtrAddr[2] = 0xAA;
    MeterPrtlData.MtrAddr[3] = 0xAA;
    MeterPrtlData.MtrAddr[4] = 0xAA;
    MeterPrtlData.MtrAddr[5] = 0xAA;

    memcpy(MeterPrtlData.DataBuff, &DI, 4);
    memcpy(MeterPrtlData.DataBuff + 4, &FrameInfo, 8 + FrameInfo.len);

    MeterPrtlData.CtrlCode = 0x14;
    MeterPrtlData.DataLen = 12 + FrameInfo.len;
    MeterPrtlData.MtrType = METER_PLC;
    MeterPrtlData.PrtlMode = PRTL_07;
    MeterPrtlData.DelayFlag = 0;

    len = PackMeterPrtl(sendbuf, MeterPrtlData);

    // WriteRamXData(MDL_UPDATAFILE_TOTALNUM,(char *)&PackNum);
    // WriteRamXData(MDL_UPDATEFILE_CURINDEX,(char *)&g_iUpdateCfg.UpdateSeq);
    UPDATE_FMT_DEBUG("PackNum = %d\n", PackNum);
    UPDATE_FMT_DEBUG("g_iUpdateCfg.UpdateSeq = %d\n", g_iUpdateCfg.UpdateSeq);

    return len;

}

/**
*********************************************************************
* @brief：     是否最后一帧
* @param[in]： UpdateSeq - 序列号
* @param[out]：
* @return：    TRUE      - 是最后一帧
*              FALSE     - 不是最后一帧
*********************************************************************
*/
BOOL is_last_updateSeq(uint32 UpdateSeq)
{
    int                 PackLen = 220;
    struct stat         statbuf = { 0 };
    int                 PackNum = 0;

    if (stat(JC_UPDATE_FILE_BACK, &statbuf) < 0)
    {
        UPDATE_FMT_DEBUG("传输后行帧的时候，获得不到文件大小");
        return FALSE;
    }

    //获得所有的包数，也即序列号
    if (statbuf.st_size % PackLen)
        PackNum = statbuf.st_size / PackLen + 1;
    else
        PackNum = statbuf.st_size / PackLen;

    if (UpdateSeq == (PackNum - 1))
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }

    return FALSE;
}

/**
*********************************************************************
* @brief：     模组模块升级组帧，并且发送到缓冲区
* @param[in]： fileBuf - 文件内容
               Len     - 内容长度
               Uis     - 文件升级信息
* @param[out]：sendbuf - 发送报文
* @return：    >0        发送长度
*              <=0       失败
*********************************************************************
*/
int MakeFrameUpdateJC_index(int32 fdcry, uint8* sendbuf, uint8 index)
{
    int                 PackLen = 220;
    struct stat         statbuf = { 0 };
    int                 PackNum = 0;
    JC_FRAME_TYPE_T     FrameInfo;
    METER_PRTL_FMT_T    MeterPrtlData;
    uint32              DI = 0x04A01101;
    uint16              len = 0;
    uint32              UpdateSeq = g_iUpdateCfg.UpdateSeq + index;

    if (fdcry < 0)
    {
        UPDATE_FMT_DEBUG("传输后行帧的时候，找不到文件");
        return -9;
    }

    if (stat(JC_UPDATE_FILE_BACK, &statbuf) < 0)
    {
        UPDATE_FMT_DEBUG("传输后行帧的时候，获得不到文件大小");
        return -9;
    }

    //获得所有的包数，也即序列号
    if (statbuf.st_size % PackLen)
        PackNum = statbuf.st_size / PackLen + 1;
    else
        PackNum = statbuf.st_size / PackLen;

    memset(&FrameInfo, 0x0, sizeof(FrameInfo));

    if (UpdateSeq >= PackNum)
        return -1;

    if (g_iUpdateCfg.ClearFlag == 1)
        FrameInfo.type = 01;
    else
        FrameInfo.type = 00;

    FrameInfo.kind = 00;
    if (UpdateSeq == (PackNum - 1))
        FrameInfo.kind = 01;

    FrameInfo.cmd = 00;
    FrameInfo.seg = PackNum;
    FrameInfo.offset = UpdateSeq;

    lseek(fdcry, UpdateSeq * PackLen, SEEK_SET);

    if (UpdateSeq == (PackNum - 1))
    {
        if (statbuf.st_size % PackLen)
            PackLen = (statbuf.st_size % PackLen);
        read(fdcry, FrameInfo.buf, PackLen);
    }
    else
        read(fdcry, FrameInfo.buf, PackLen);
    FrameInfo.len = PackLen;

    MeterPrtlData.MtrAddr[0] = 0xAA;//地址固定
    MeterPrtlData.MtrAddr[1] = 0xAA;
    MeterPrtlData.MtrAddr[2] = 0xAA;
    MeterPrtlData.MtrAddr[3] = 0xAA;
    MeterPrtlData.MtrAddr[4] = 0xAA;
    MeterPrtlData.MtrAddr[5] = 0xAA;

    memcpy(MeterPrtlData.DataBuff, &DI, 4);
    memcpy(MeterPrtlData.DataBuff + 4, &FrameInfo, 8 + FrameInfo.len);

    MeterPrtlData.CtrlCode = 0x14;
    MeterPrtlData.DataLen = 12 + FrameInfo.len;
    MeterPrtlData.MtrType = METER_PLC;
    MeterPrtlData.PrtlMode = PRTL_07;
    MeterPrtlData.DelayFlag = 0;

    len = PackMeterPrtl(sendbuf, MeterPrtlData);

    UPDATE_FMT_DEBUG("PackNum = %d\n", PackNum);
    UPDATE_FMT_DEBUG("g_iUpdateCfg.UpdateSeq = %d\n", UpdateSeq);

    return len;

}

/**
*********************************************************************
* @brief：     模组模块升级组帧，并且发送到缓冲区
* @param[in]： fileBuf - 文件内容
               Len     - 内容长度
               Uis     - 文件升级信息
* @param[out]：sendbuf - 发送报文
* @return：    >0        发送长度
*              <=0       失败
*********************************************************************
*/
int MakeFrameUpdateJC_mix(int32 fdcry, uint8* sendbuf, uint8 num)
{
    int   offset = 0;
    int   i = 0;
    int   len = 0;

    for (i = 0; i < num; i++)
    {
        len = MakeFrameUpdateJC_index(fdcry, &sendbuf[offset], i);
        if (len < 0)
        {
            break;
        }
        else
        {
            offset += len;
        }

        if (g_iUpdateCfg.ClearFlag == 0)
        {
            break;
        }

        if (offset > SEND_BUF_LEN)
        {
            UPDATE_FMT_DEBUG("MakeFrameUpdateJC_mix len overflow. i[%d]. offset[%d]\n", i, offset);
            return -2;
        }
    }

    if ((offset > 0) && (offset <= SEND_BUF_LEN))
    {
        return offset;
    }
    else
    {
        return -1;
    }
}


/**
*********************************************************************
* @brief：     交采模块升级组帧，并且发送到缓冲区
* @param[in]： fileBuf - 文件内容
               Len     - 内容长度
               Uis     - 文件升级信息
* @param[out]：sendbuf - 发送报文
* @return：    >0        发送长度
*              <=0       失败
*********************************************************************
*/
int MakeFrameUpdateJC(int32 fdcry, uint8* sendbuf/*, uint8 *fileBuf, uint16 Len, JcUpdateInfoState Uis*/)
{
#if AC_HIGH_UPDATE_MOD == 1
    return MakeFrameUpdateJC_mix(fdcry, sendbuf, AC_UPDATE_NUM);
#else
    return MakeFrameUpdateJC_one(fdcry, sendbuf);
#endif
}

/**
*********************************************************************
* @brief：     解析升级报文回码
* @param[in]： recvBuf - 文件内容
               len     - 内容长度
* @param[out]：
* @return：    错误码
*********************************************************************
*/
int parserFrameUpdateJC(int fd_232, uint8* recvBuf, uint32 len)
{
    uint8            recvBufTmp[RECV_BUF_LEN] = { 0 };
    int              ret = ERR_OK;
    uint32           i = 0;
    uint32           k = 0;
    uint32           index = 0;
    uint32           recvLen = len;
    uint32           outLen = 0;
    uint32           offset = 0;
    uint8* ptr = NULL;
    METER_PRTL_FMT_T MeterPrtlData;                            //电表规约格式数据
    uint16           FFBuf = 0xffff;

    if (recvLen < 0x0d)
    {
        return ERR_INVALID;
    }

    while ((ptr = get_07ptr_from_buf(recvBuf, recvLen, &offset, &outLen)) != NULL)
    {
        memcpy(recvBufTmp, ptr, outLen);
        recvLen -= (offset + outLen);
        memmove(recvBuf, (ptr + outLen), recvLen);

        //缓存recvBufTmp 长度outLen
        ret = UnPackMeterData(outLen, recvBufTmp, MeterPrtlData.MtrAddr, &MeterPrtlData.CtrlCode, &MeterPrtlData.DataLen, MeterPrtlData.DataBuff);
        UPDATE_FMT_DEBUG("解帧结果 : %d \n", ret);
        if (ret == CHECK_RESULT_FAILUER)
        {
            return  ERR_ITEM;
        }

        if (MeterPrtlData.DataBuff[0] == 0x01 && MeterPrtlData.DataBuff[1] == 0x11
            && MeterPrtlData.DataBuff[2] == 0xA0 && MeterPrtlData.DataBuff[3] == 0x04)
        {
            uint16    revseq;

            memcpy(&revseq, &MeterPrtlData.DataBuff[4], 2);
            UPDATE_FMT_DEBUG("接收序号 = %d,发送序号 = %d\n", revseq, g_iUpdateCfg.UpdateSeq + i);

            if (revseq != (g_iUpdateCfg.UpdateSeq + i))
            {
                if (memcmp(&FFBuf, &revseq, 2) == 0)
                {
                    UPDATE_FMT_DEBUG("否认回复\n");
                }
                else
                {
                    UPDATE_FMT_DEBUG("帧序号错误\n");
                }

                return ERR_ITEM;
            }

            i++;
        }
        else
        {
            return  ERR_ITEM;
        }

    }

    //没接收完全，再继续收
    index = i;
    for (k = 0; k < (AC_UPDATE_NUM - index); k++)
    {
        if ((g_iUpdateCfg.ClearFlag == 1) && (i < AC_UPDATE_NUM) && (!is_last_updateSeq(g_iUpdateCfg.UpdateSeq + i - 1)))
        {
            recvLen = commdrv_Recv_232(fd_232, recvBuf, SEND_BUF_LEN, 8, 8, 0xFF);
            if (recvLen > 0)
            {
                UPDATE_BUF_DEBUG(recvBuf, recvLen, "升级接受帧ret[%d]", recvLen);
                outLen = 0;
                offset = 0;
                ptr = NULL;
                while ((ptr = get_07ptr_from_buf(recvBuf, recvLen, &offset, &outLen)) != NULL)
                {
                    memcpy(recvBufTmp, ptr, outLen);
                    recvLen -= (offset + outLen);
                    memmove(recvBuf, (ptr + outLen), recvLen);

                    //缓存recvBufTmp 长度outLen
                    ret = UnPackMeterData(outLen, recvBufTmp, MeterPrtlData.MtrAddr, &MeterPrtlData.CtrlCode, &MeterPrtlData.DataLen, MeterPrtlData.DataBuff);
                    UPDATE_FMT_DEBUG("解帧结果 : %d \n", ret);
                    if (ret == CHECK_RESULT_FAILUER)
                    {
                        return  ERR_ITEM;
                    }

                    if (MeterPrtlData.DataBuff[0] == 0x01 && MeterPrtlData.DataBuff[1] == 0x11
                        && MeterPrtlData.DataBuff[2] == 0xA0 && MeterPrtlData.DataBuff[3] == 0x04)
                    {
                        uint16    revseq;

                        memcpy(&revseq, &MeterPrtlData.DataBuff[4], 2);
                        UPDATE_FMT_DEBUG("接收序号 = %d,发送序号 = %d\n", revseq, g_iUpdateCfg.UpdateSeq + i);

                        if (revseq != (g_iUpdateCfg.UpdateSeq + i))
                        {
                            if (memcmp(&FFBuf, &revseq, 2) == 0)
                            {
                                UPDATE_FMT_DEBUG("否认回复\n");
                            }
                            else
                            {
                                UPDATE_FMT_DEBUG("帧序号错误\n");
                            }

                            return ERR_ITEM;
                        }

                        i++;
                    }
                    else
                    {
                        return  ERR_ITEM;
                    }

                }
            }
        }
        else
        {
            break;
        }
    }

    if (g_iUpdateCfg.ClearFlag == 1)
        g_iUpdateCfg.UpdateSeq += i;
    else
        g_iUpdateCfg.ClearFlag = 1;

    return ERR_OK;
}

/*******************************************************************************
* 函数名称: MakeFrameGetVerJC
* 函数功能: 模组模块升级组帧，并且发送到缓冲区
* 输入参数: fileBuf - 文件内容
           Len - 内容长度
           Uis - 文件升级信息
* 输出参数: sendbuf - 发送报文
* 返 回 值: >0                  发送长度
*           <0                  失败
*******************************************************************************/
int MakeFrameGetVerJC(int32 fdcry, uint8* sendbuf/*, uint8 *fileBuf, uint16 Len, JcUpdateInfoState Uis*/)
{
    METER_PRTL_FMT_T    MeterPrtlData;
    uint32              DI = 0x040005FE;
    uint16              len = 0;

    MeterPrtlData.MtrAddr[0] = 0xAA;//地址固定
    MeterPrtlData.MtrAddr[1] = 0xAA;
    MeterPrtlData.MtrAddr[2] = 0xAA;
    MeterPrtlData.MtrAddr[3] = 0xAA;
    MeterPrtlData.MtrAddr[4] = 0xAA;
    MeterPrtlData.MtrAddr[5] = 0xAA;

    memcpy(MeterPrtlData.DataBuff, &DI, 4);

    MeterPrtlData.CtrlCode = 0x11;
    MeterPrtlData.DataLen = 4;
    MeterPrtlData.MtrType = METER_PLC;
    MeterPrtlData.PrtlMode = PRTL_07;
    MeterPrtlData.DelayFlag = 0;

    len = PackMeterPrtl(sendbuf, MeterPrtlData);

    return len;

}

/*******************************************************************************
* 函数名称:	UnPackMeterData
* 函数功能:	电表规约解帧
* 输入参数:	FrameLen            帧长度
*           FrameBuff           帧数据
* 输出参数:	MtrAddr             电表地址
*           DataLen             数据域长度
*           DataBuff            数据域内容
*           CtrlCode            控制码
* 返 回 值: CHECK_RESULT_SUCCEED
*           CHECK_RESULT_FAILUER
*           CHECK_RESULT_CONFIRM
*           CHECK_RESULT_DENY
*******************************************************************************/
int8 UnPackMeterData(uint16 FrameLen, uint8* FrameBuff, uint8* MtrAddr
    , uint8* CtrlCode, uint8* DataLen, uint8* DataBuff)
{
    uint16  DataIndex = 0xff;                   //指针偏移
    uint8   CS = 0;
    uint16   i;//start Common Problem

    //滑动偏移，定位帧头
    for (i = 0; i < FrameLen; i++)
    {
        if ((FrameBuff[i] == 0x68) && (FrameBuff[i + 7] == 0x68))
        {
            DataIndex = i;
            break;
        }
    }

    //未定位到帧头，判为失败
    if (DataIndex == 0xff)
    {
#if 0//WHG_FUN == 1
        sint8 ret = UnPackWHGData(FrameLen, FrameBuff, MtrAddr,
            CtrlCode, DataLen, DataBuff);
        if (ret >= 0)
            return ret;
#endif
        return CHECK_RESULT_FAILUER;
    }

    //刷新地址域
    memcpy(MtrAddr, &FrameBuff[DataIndex + 1], 6);

    //控制码
    *CtrlCode = FrameBuff[DataIndex + 8];

    //刷新数据长度
    *DataLen = FrameBuff[DataIndex + 9];

    //判断帧尾
    if (FrameBuff[(DataIndex + *DataLen + 11)] != 0x16)
    {
        return CHECK_RESULT_FAILUER;
    }

    //判断校验和
    for (i = 0; i < (*DataLen + 10); i++)
    {
        CS += FrameBuff[DataIndex + i];
    }
    if (CS != FrameBuff[(DataIndex + *DataLen + 10)])
    {
        return CHECK_RESULT_FAILUER;
    }

    //刷新数据域内容，同时做余3码处理
    for (i = 0; i < *DataLen; i++)
    {
        DataBuff[i] = FrameBuff[DataIndex + 10 + i] - 0x33;
    }

    //判断是否是否认帧
    if (*CtrlCode & 0x40)
    {
        return CHECK_RESULT_DENY;
    }

    //判断是否是确认帧
    if (*DataLen == 0)
    {
        return CHECK_RESULT_CONFIRM;
    }

    return CHECK_RESULT_SUCCEED;
}

/**********************************************************************
* @name：      ac_update_file_close
* @brief：     关闭文件句柄
* @param[in] ：
* @param[out]：
* @return    ：
* @Date      ：2019-12-6
**********************************************************************/
int ac_update_file_close(int* Soft_fd)
{
    int fd = *Soft_fd;
    if (fd >= 0)
    {
        close(fd);
    }

    *Soft_fd = -1;

    return 0;
}

/*******************************************************************************
* 函数名称: JcUpdateDeal
* 函数功能: 交采升级处理
* 输入参数: isGetVer 需要招版本号
* 输出参数: 无
* 返 回 值: >0                  成功
*           <0                  失败
*******************************************************************************/
int JcUpdateDeal(int32 fd_232, BOOL isGetVer)
{
    static MODULE_JC_UPDATE_DEAL_T    gJcUpdateStat = CMD_IDLE;
    struct timespec  StartTime = { 0 };                          //开始时间
    struct timespec  CurrentTime = { 0 };                        //当前时间
    static uint32    StartTimeSec = 0;                         //开始时间(秒)
    uint32           CurrentTimeSec = 0;                       //当前时间(秒)
    static int       CommRetryCnt = 0;                         //通讯重试次数
    static  uint8    OverTimeCnt = 8;                          //超时时间
    int              RecvResult = 1;                           //接收结果(1:成功 / -1:失败)
    int              ret = -1;
    static int       WaitTime = 0;
    METER_PRTL_FMT_T MeterPrtlData;                            //电表规约格式数据
    static int       Soft_fd = -1;                             //升级文件句柄
    char             TmpPath[100] = { 0 };
#if AC_HIGH_UPDATE_MOD == 0
    uint16           FFBuf = 0xffff;
#endif
    int32 delay = 0;
    int32 recvLen = 0;
    int32 sendLen = 0;
    uint8 recvBuf[RECV_BUF_LEN] = { 0 };
    uint8 sendBuf[SEND_BUF_LEN] = { 0 };

    static JC_UPDATE_STEP_E updateStep = JC_UPDATE_IDLE;

    switch (gJcUpdateStat)
    {
    case CMD_IDLE:
        if (fd_232 < 0)
        {
            UPDATE_FMT_TRACE("升级失败gJcUpdateStat = CMD_IDLE, fd_232 <= 0 fd_232 = %d\n", fd_232);
            return -2;
        }
        else
        {
            UPDATE_FMT_TRACE("gJcUpdateStat = CMD_IDLE, fd_232 > 0 fd_232 = %d\n", fd_232);
        }

        g_iUpdateCfg.AcqAddr[0] = 0xAA;//交采地址固定
        g_iUpdateCfg.AcqAddr[1] = 0xAA;
        g_iUpdateCfg.AcqAddr[2] = 0xAA;
        g_iUpdateCfg.AcqAddr[3] = 0xAA;
        g_iUpdateCfg.AcqAddr[4] = 0xAA;
        g_iUpdateCfg.AcqAddr[5] = 0xAA;

        if (isGetVer == TRUE)//招版本号
        {
            //波特率切换之后生成的召版本号任务
            if (gCallVerAfterBaudRateChanged)
            {
                UPDATE_FMT_DEBUG("JcUpdateDeal() 波特率切换之后生成的召版本号任务... \n");
                gCallVerAfterBaudRateChanged = 0;
                gTryCnt = 0;
            }

            if (gTryCnt)
            {
                return -3;
            }
            gTryCnt++;
            gJcUpdateStat = CMD_VER_SEND;
            break;
        }

        UPDATE_FMT_DEBUG("开始升级JC通信地址为: %02x%02x%02x%02x%02x%02x \n"
            , 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA);

        Soft_fd = open(JC_UPDATE_FILE_BACK, O_RDONLY);

        //升级的帧序列号从0开始下发
        g_iUpdateCfg.UpdateSeq = 0;

        g_iUpdateCfg.JcNum = 1;//就一个交采，此段程序移植于采集器升级，所以存在设备数量
        //完成一轮后轮次加1
        if (g_iUpdateCfg.CurJcIndex >= g_iUpdateCfg.JcNum)
        {
            g_iUpdateCfg.CurJcIndex = 0;
            g_iUpdateCfg.iUpdateCnt++;
        }
        if (g_iUpdateCfg.iUpdateCnt > 3)
        {
            UPDATE_FMT_DEBUG("已达到重试轮次，退出\n");
            gJcUpdateStat = CMD_END;
            break;
        }

        if (g_iUpdateCfg.iUpdateSuccCnt >= g_iUpdateCfg.JcNum)
        {
            UPDATE_FMT_DEBUG("JC升级完成，清空版本号，延时，再招版本号\n");
            memset(gVerBuf, 0x00, sizeof(gVerBuf));  //升级完成之后版本号先清零，再召版本号
            gJcUpdateStat = CMD_DELAY;
            break;
        }

        if (g_iUpdateCfg.UpdateFlag[g_iUpdateCfg.CurJcIndex] != 1)
        {
            updateStep = JC_UPDATE_START;
            UPDATE_FMT_DEBUG("第一次招版本号\n");
            gJcUpdateStat = CMD_VER_SEND;
        }
        else
        {
            ac_update_file_close(&Soft_fd);
            UPDATE_FMT_DEBUG("已升级成功，切换下一个JC\n");
            g_iUpdateCfg.CurJcIndex++;
        }
        break;

    case CMD_SEND:   /*升级发送*/
        ClearCommRecvBuff(fd_232);
        sendLen = MakeFrameUpdateJC(Soft_fd, sendBuf);
        if (sendLen <= -9)
        {
            UPDATE_FMT_DEBUG("文件错误，升级失败\n");
            gJcUpdateStat = CMD_FAIL;
            break;
        }
        else if (sendLen < 0)    //发送完成
        {
            gJcUpdateStat = CMD_SUC;
            break;
        }

        UPDATE_BUF_DEBUG(sendBuf, sendLen, "升级发送帧ret[%d]", sendLen);
        ret = commdrv_Send_232(fd_232, sendBuf, sendLen);
        if (ret > 0)
        {
            clock_gettime(CLOCK_MONOTONIC, &StartTime);    //刷新发送时间
            StartTimeSec = (uint32)StartTime.tv_sec;
            gJcUpdateStat = CMD_RECV;
        }
        else if (ret < 0)
        {
            gJcUpdateStat = CMD_FAIL;
            close(fd_232);
            fd_232 = -1;
        }

        break;

    case CMD_RECV:   /*升级接收*/
        recvLen = commdrv_Recv_232(fd_232, recvBuf, sizeof(recvBuf), 5, 5, 0xFF);
        if (recvLen > 0)
        {
            UPDATE_BUF_DEBUG(recvBuf, recvLen, "升级接受帧ret[%d]", recvLen);

#if AC_HIGH_UPDATE_MOD == 0
            ret = UnPackMeterData(recvLen, recvBuf
                , MeterPrtlData.MtrAddr, &MeterPrtlData.CtrlCode, &MeterPrtlData.DataLen, MeterPrtlData.DataBuff);

            UPDATE_FMT_DEBUG("解帧结果 : %d \n", ret);
            if (ret == CHECK_RESULT_FAILUER)
            {
                RecvResult = -1;
                break;
            }

            /*if(memcmp(MeterPrtlData.MtrAddr,g_iUpdateCfg.AcqAddr,6) != 0)
            {
                RecvResult = -1;
                UPDATE_FMT_DEBUG("JC地址错误\n");
                break;
            }*/

            if (MeterPrtlData.DataBuff[0] == 0x01 && MeterPrtlData.DataBuff[1] == 0x11
                && MeterPrtlData.DataBuff[2] == 0xA0 && MeterPrtlData.DataBuff[3] == 0x04)
            {
                uint16    revseq;

                memcpy(&revseq, &MeterPrtlData.DataBuff[4], 2);
                UPDATE_FMT_DEBUG("接收序号 = %d,发送序号 = %d\n", revseq, g_iUpdateCfg.UpdateSeq);

                if (revseq != g_iUpdateCfg.UpdateSeq)
                {
                    if (memcmp(&FFBuf, &revseq, 2) == 0)
                    {
                        UPDATE_FMT_DEBUG("否认回复\n");
                    }
                    else
                    {
                        UPDATE_FMT_DEBUG("帧序号错误\n");
                    }
                    RecvResult = -1;
                    break;
                }

                CommRetryCnt = 0;
                if (g_iUpdateCfg.ClearFlag == 1)
                    g_iUpdateCfg.UpdateSeq++;
                else
                    g_iUpdateCfg.ClearFlag = 1;
                gJcUpdateStat = CMD_SEND; //切换到发送状态
            }
            else
            {
                RecvResult = -1;
            }
#else
            if (parserFrameUpdateJC(fd_232, recvBuf, recvLen) != ERR_OK)
            {
                RecvResult = -1;
                CommRetryCnt++;                  //重试次数+1
                gJcUpdateStat = CMD_RETRY;      //切换到重试状态
                break;
            }
            else
            {
                RecvResult = 1;
                CommRetryCnt = 0;
                gJcUpdateStat = CMD_SEND; //切换到发送状态
            }
#endif
        }
        else
        {
            clock_gettime(CLOCK_MONOTONIC, &CurrentTime);    //刷新发送时间
            CurrentTimeSec = (uint32)CurrentTime.tv_sec;
            if ((CurrentTimeSec - StartTimeSec) > OverTimeCnt)
            {
                RecvResult = -1;
            }
        }

        if (RecvResult == -1)            //接收失败
        {
            CommRetryCnt++;                  //重试次数+1
            gJcUpdateStat = CMD_RETRY;      //切换到重试状态
        }
        break;

    case CMD_RETRY:  /*发送尝试*/
        if (CommRetryCnt > 3)
        {
            CommRetryCnt = 0;
            gJcUpdateStat = CMD_FAIL;//升级失败
            break;
        }

        //切换到发送状态
        gJcUpdateStat = CMD_SEND;
        if (isGetVer == TRUE)
        {
            gJcUpdateStat = CMD_VER_SEND;
        }
        break;

    case CMD_DELAY:  /*升级后延时*/
        updateStep = JC_UPDATE_END;

        //如果是升级计量芯片，则延时放长
        if (gJCUpdateFileMark == 0x0a)
        {
            delay = 55;
        }
        else
        {
            delay = 35;
        }

        if (IsCloseInnerDelayState((gTimeFlag.BitSect.Second == TRUE), &WaitTime, delay) > 0)
            gJcUpdateStat = CMD_VER_SEND;
        break;

    case CMD_FAIL:
        ac_update_file_close(&Soft_fd);
        g_iUpdateCfg.UpdateFlag[g_iUpdateCfg.CurJcIndex] = 2;
        g_iUpdateCfg.CurJcIndex++;
        gJcUpdateStat = CMD_IDLE;
        updateStep = JC_UPDATE_IDLE;
        break;

    case CMD_SUC:  /*升级成功*/
        ac_update_file_close(&Soft_fd);
        g_iUpdateCfg.UpdateFlag[g_iUpdateCfg.CurJcIndex] = 1;
        g_iUpdateCfg.CurJcIndex++;
        g_iUpdateCfg.iUpdateSuccCnt++;
        UPDATE_FMT_DEBUG("g_iUpdateCfg.iUpdateSuccCnt++\n");
        gJcUpdateStat = CMD_IDLE;
        updateStep = JC_UPDATE_IDLE;
        break;

    case CMD_VER_SEND:
        sendLen = MakeFrameGetVerJC(Soft_fd, sendBuf);
        if (sendLen <= -9)
        {
            UPDATE_FMT_DEBUG("文件错误，升级失败\n");
            gJcUpdateStat = CMD_FAIL;
            break;
        }
        else if (sendLen < 0)    //发送完成
        {
            gJcUpdateStat = CMD_SUC;
            break;
        }

        ClearCommRecvBuff(fd_232);
        ret = commdrv_Send_232(fd_232, sendBuf, sendLen);
        UPDATE_BUF_DEBUG(sendBuf, sendLen, "发送版本帧:");
        UPDATE_FMT_DEBUG("fd_232 = %d commdrv_Send_232 ret = %d\n", fd_232, ret);
        if (ret > 0)
        {
            clock_gettime(CLOCK_MONOTONIC, &StartTime);    //刷新发送时间
            StartTimeSec = (uint32)StartTime.tv_sec;
            gJcUpdateStat = CMD_VER_RECV;
        }
        break;

    case CMD_VER_RECV:
        recvLen = commdrv_Recv_232(fd_232, recvBuf, sizeof(recvBuf), 5, 5, 0xFF);
        if (recvLen > 0)
        {
            UPDATE_BUF_DEBUG(recvBuf, recvLen, "接受版本帧:");
            ret = UnPackMeterData(recvLen, recvBuf
                , MeterPrtlData.MtrAddr, &MeterPrtlData.CtrlCode, &MeterPrtlData.DataLen, MeterPrtlData.DataBuff);

            UPDATE_FMT_DEBUG("解帧结果 : %d \n", ret);
            if (ret == CHECK_RESULT_FAILUER)
            {
                RecvResult = -1;
                break;
            }

            if (MeterPrtlData.DataBuff[0] == 0xFE && MeterPrtlData.DataBuff[1] == 0x05
                && MeterPrtlData.DataBuff[2] == 0x00 && MeterPrtlData.DataBuff[3] == 0x04)
            {
                CommRetryCnt = 0;

                memcpy(&gVerBuf[0], &MeterPrtlData.DataBuff[4], 4);

                UPDATE_FMT_DEBUG("交采模块版本号VER = V%c%c%c%c\n", gVerBuf[0], gVerBuf[1], gVerBuf[2], gVerBuf[3]);

#ifdef AREA_SHANDONG
                if (MeterPrtlData.DataLen == 10)
                {
                    memcpy(&gVerBuf[4], &MeterPrtlData.DataBuff[8], 2);
                    UPDATE_FMT_DEBUG("交采计量版本号VER = %c%c\n", gVerBuf[4], gVerBuf[5]);
                }
#endif

                if (isGetVer == TRUE)
                {
                    updateStep = JC_UPDATE_IDLE;
                    gJcUpdateStat = CMD_IDLE;
                    break;
                }

                if (updateStep == JC_UPDATE_START)
                {
                    gJcUpdateStat = CMD_SEND; //切换到发送状态
                }
                else if (updateStep == JC_UPDATE_END)
                {
                    gJcUpdateStat = CMD_END; //切换到发送状态
                }
                else
                {
                    RecvResult = -1;
                }
            }
            else
            {
                RecvResult = -1;
            }

        }
        else
        {
            clock_gettime(CLOCK_MONOTONIC, &CurrentTime);    //刷新发送时间
            CurrentTimeSec = (uint32)CurrentTime.tv_sec;
            if ((CurrentTimeSec - StartTimeSec) > OverTimeCnt)
            {
                RecvResult = -1;
            }
        }

        if (RecvResult == -1)            //接收失败
        {
            CommRetryCnt++;                  //重试次数+1
            gJcUpdateStat = CMD_RETRY;      //切换到重试状态
        }
        break;

    case CMD_END:    /*升级结束*/
        ac_update_file_close(&Soft_fd);
        sprintf(TmpPath, "rm -rf %s", JC_UPDATE_FILE_BACK);
        system(TmpPath);
        if (g_iUpdateCfg.iUpdateCnt > 3)
        {
            UPDATE_FMT_DEBUG("%s, rm -rf %s", "JC升级失败", JC_UPDATE_FILE_BACK);
        }
        else
        {
            UPDATE_FMT_DEBUG("%s, rm -rf %s", "JC升级成功", JC_UPDATE_FILE_BACK);
        }
        memset(&g_iUpdateCfg, 0x0, sizeof(JC_UPDATE_CFG_T));
        gJCUpdateFileMark = 0;
        gJcUpdateStat = CMD_IDLE;
        updateStep = JC_UPDATE_IDLE;

        //子节点升级完成后刷新版本号，重新抄读版本号
        //ClearNormalMeterIndex(METER_RTDATA);
        //gRebootAmrThread = 1;

        sleep(3);
        return 1;
        break;

    default:
        break;
    }

    return -1;
}

#endif