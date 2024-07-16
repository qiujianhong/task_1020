/*
*********************************************************************
* Copyright(C) 2019 �Ͼ��������ӹɷ����޹�˾
* All rights reserved.
* @brief�� �����������
* @date�� 2019-10-25
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

#if DESC("����������ش���",1)

JC_UPDATE_CFG_T          g_iUpdateCfg;            // ��������
uint8                    g_InerSeq = 0;           // �ڲ�֡���к�
uint8                    gJCUpdateFileMark = 0;   // �����ļ���ʶȫ�ֱ��. 0x09-����ģ�� 0x0a-����оƬ 0x0b-����ģ��
uint8                    gTryCnt = 0;

//�汾��Ϣ��ر���
uint8                    gVerBuf[8] = { 0 };        // ���ڽ��ɰ汾��Ϣ 
uint8                    gVerBufLast[8] = { 0 };    // �����ϰ潻�ɰ汾��Ϣ 
uint8                    gDateBuf[8] = { 0 };       // ���ڽ��ɰ汾������Ϣ
uint8                    gDateBufLast[8] = { 0 };   // �����ϰ潻�ɰ汾������Ϣ
uint8                    gVerSBuf[8] = { 0 };     // ���ڽ���С�汾��Ϣ 
uint8                    gVerSBufLast[8] = { 0 }; // �����ϰ潻��С�汾��Ϣ 

/*******************************************************************************
* ��������:	ClearCommRecvBuff
* ��������:	���ͨѶ��������
* �������:	fd  -  ���ڲ������
* �������:	��
* �� �� ֵ:	��
*******************************************************************************/
void ClearCommRecvBuff(int32 fd)
{
    int     RecvLen = 0xff;                     //�������ݳ���
    uint8   RecvBuf[1024];                      //�������ݻ�����

    while (RecvLen > 0)
    {
        RecvLen = commdrv_Recv_232(fd, RecvBuf, sizeof(RecvBuf), 5, 5, 0xFF);
    }
}

/**
*********************************************************************
* @brief��     ���ɴ�ӡ
* @param[in]�� charInfo      - ��ʾ��Ϣ
               buf           - ��ӡ����
               len           - ��ӡ���ݳ���
* @param[out]����
* @return��
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
    logbuf[2 * len] = 0x00;//����'\0'
    writelog(AC_APP_LOG, "%s len = %d %s\n", charInfo, len, logbuf);

#endif
}

/**
*********************************************************************
* @brief��     ����str1���ǲ��ǰ���str2
* @param[in]��
* @param[out]����
* @return��
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
* ��������: IsCloseInnerDelayState
* ��������: �Ƿ���Խ��������ڲ���ʱ״̬
* �������: int Condition, �����ۼӵ�����
            int *Cnt,      ��������������ӦΪ��̬����
            int CntMax     �����������ֵ
* �������: ��
* �� �� ֵ: >0                  �ɹ�
*           <0                  ʧ��
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
* @brief��     ���������ļ�
* @param[in]�� ��
* @param[out]����
* @return��    0  ������
*              1  ����
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
    if ((access(JC_UPDATE_FILE, R_OK) == 0) && (gJCUpdateFileMark == 0x00))    //���ж����ڵ�������
    {
        if (IsCloseInnerDelayState((gTimeFlag.BitSect.Second == TRUE), &WaitTime, 1) <= 0)
        {
            return FALSE;
        }

        //������� JC_CTL_UPDATE_FILE_BACK �ļ��Ƚ���ɾ��
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
                UPDATE_BUF_DEBUG(buffer, size, "�����ļ�β����Ϣ");
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
* @brief��     ���������ļ�
* @param[in]�� ��
* @param[out]����
* @return��    0  ������
*              1  ����
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
    if ((access(JC_UPDATE_FILE_IC, R_OK) == 0) && (gJCUpdateFileMark == 0))    //���ж��Ƿ���ڼ���оƬ����������û��������ģ��
    {
        if (IsCloseInnerDelayState((gTimeFlag.BitSect.Second == TRUE), &WaitTime, 1) <= 0)
        {
            return FALSE;
        }

        //������� JC_CTL_UPDATE_FILE_BACK �ļ��Ƚ���ɾ��
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
                UPDATE_BUF_DEBUG(buffer, size, "���������ļ�β����Ϣ");
#ifdef AREA_SHANDONG  //ɽ����������������оƬ���󣬲����ж�XLDZ
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
* @brief��     ���������ļ�
* @param[in]�� ��
* @param[out]����
* @return��    0  ������
*              1  ����
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
    if ((access(JC_UPDATE_FILE_TOPO, R_OK) == 0) && (gJCUpdateFileMark == 0))    //���ж��Ƿ��������ģ������������û��������ģ��
    {
        if (IsCloseInnerDelayState((gTimeFlag.BitSect.Second == TRUE), &WaitTime, 1) <= 0)
        {
            return FALSE;
        }

        //������� JC_CTL_UPDATE_FILE_BACK �ļ��Ƚ���ɾ��
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
                UPDATE_FMT_DEBUG("���������ļ���ʧ�� file[%s] fd[%d]\n", JC_UPDATE_FILE_BACK, fd);
                ret = FALSE;
                return ret;
            }
            else
            {
                UPDATE_FMT_DEBUG("���������ļ��򿪳ɹ� file[%s] fd[%d]\n", JC_UPDATE_FILE_BACK, fd);
            }


            //֮ǰ���ڶ�β����Ϣ�����ڲ���оƬβ����Ϣ�����Լ���ļ��Ƿ�ɶ�
            lseek(fd, -64, SEEK_END);
            size = read(fd, buffer, 64);
            if (size <= 0)
            {
                UPDATE_FMT_DEBUG("���������ļ������� file[%s]\n", JC_UPDATE_FILE_BACK);
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
* @brief��     �Ƿ���������
* @param[in]�� ��
* @param[out]����
* @return��    0  ��������
*              1  ��������
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
* @brief��     �Ƿ���Ҫ��ȡ�汾��Ϣ
* @param[in]�� ��
* @param[out]����
* @return��    0  ����Ҫ
*              1  ��Ҫ
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
* @brief��     ��հ汾��Ϣ
* @param[in]�� ��
* @param[out]����
* @return��    ��
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
* @brief��     ����Լ��֡
* @param[in]�� MeterData           ����Լ����
* @param[out]��PrtlData            ��֡������
* @return��    ֡����
*********************************************************************
*/
int PackMeterPrtl(uint8* PrtlData, METER_PRTL_FMT_T MeterData)
{
    uint8   CheckSum = 0;                       //У���
    int     i;
    uint16   CurIndex;                           //����

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
* @brief��     ģ��ģ��������֡�����ҷ��͵�������
* @param[in]�� fileBuf - �ļ�����
               Len     - ���ݳ���
               Uis     - �ļ�������Ϣ
* @param[out]��sendbuf - ���ͱ���
* @return��    >0        ���ͳ���
*              <=0       ʧ��
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
        UPDATE_FMT_DEBUG("�������֡��ʱ���Ҳ����ļ�");
        return -9;
    }

    if (stat(JC_UPDATE_FILE_BACK, &statbuf) < 0)
    {
        UPDATE_FMT_DEBUG("�������֡��ʱ�򣬻�ò����ļ���С");
        return -9;
    }

    //������еİ�����Ҳ�����к�
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

    MeterPrtlData.MtrAddr[0] = 0xAA;//��ַ�̶�
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
* @brief��     �Ƿ����һ֡
* @param[in]�� UpdateSeq - ���к�
* @param[out]��
* @return��    TRUE      - �����һ֡
*              FALSE     - �������һ֡
*********************************************************************
*/
BOOL is_last_updateSeq(uint32 UpdateSeq)
{
    int                 PackLen = 220;
    struct stat         statbuf = { 0 };
    int                 PackNum = 0;

    if (stat(JC_UPDATE_FILE_BACK, &statbuf) < 0)
    {
        UPDATE_FMT_DEBUG("�������֡��ʱ�򣬻�ò����ļ���С");
        return FALSE;
    }

    //������еİ�����Ҳ�����к�
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
* @brief��     ģ��ģ��������֡�����ҷ��͵�������
* @param[in]�� fileBuf - �ļ�����
               Len     - ���ݳ���
               Uis     - �ļ�������Ϣ
* @param[out]��sendbuf - ���ͱ���
* @return��    >0        ���ͳ���
*              <=0       ʧ��
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
        UPDATE_FMT_DEBUG("�������֡��ʱ���Ҳ����ļ�");
        return -9;
    }

    if (stat(JC_UPDATE_FILE_BACK, &statbuf) < 0)
    {
        UPDATE_FMT_DEBUG("�������֡��ʱ�򣬻�ò����ļ���С");
        return -9;
    }

    //������еİ�����Ҳ�����к�
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

    MeterPrtlData.MtrAddr[0] = 0xAA;//��ַ�̶�
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
* @brief��     ģ��ģ��������֡�����ҷ��͵�������
* @param[in]�� fileBuf - �ļ�����
               Len     - ���ݳ���
               Uis     - �ļ�������Ϣ
* @param[out]��sendbuf - ���ͱ���
* @return��    >0        ���ͳ���
*              <=0       ʧ��
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
* @brief��     ����ģ��������֡�����ҷ��͵�������
* @param[in]�� fileBuf - �ļ�����
               Len     - ���ݳ���
               Uis     - �ļ�������Ϣ
* @param[out]��sendbuf - ���ͱ���
* @return��    >0        ���ͳ���
*              <=0       ʧ��
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
* @brief��     �����������Ļ���
* @param[in]�� recvBuf - �ļ�����
               len     - ���ݳ���
* @param[out]��
* @return��    ������
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
    METER_PRTL_FMT_T MeterPrtlData;                            //����Լ��ʽ����
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

        //����recvBufTmp ����outLen
        ret = UnPackMeterData(outLen, recvBufTmp, MeterPrtlData.MtrAddr, &MeterPrtlData.CtrlCode, &MeterPrtlData.DataLen, MeterPrtlData.DataBuff);
        UPDATE_FMT_DEBUG("��֡��� : %d \n", ret);
        if (ret == CHECK_RESULT_FAILUER)
        {
            return  ERR_ITEM;
        }

        if (MeterPrtlData.DataBuff[0] == 0x01 && MeterPrtlData.DataBuff[1] == 0x11
            && MeterPrtlData.DataBuff[2] == 0xA0 && MeterPrtlData.DataBuff[3] == 0x04)
        {
            uint16    revseq;

            memcpy(&revseq, &MeterPrtlData.DataBuff[4], 2);
            UPDATE_FMT_DEBUG("������� = %d,������� = %d\n", revseq, g_iUpdateCfg.UpdateSeq + i);

            if (revseq != (g_iUpdateCfg.UpdateSeq + i))
            {
                if (memcmp(&FFBuf, &revseq, 2) == 0)
                {
                    UPDATE_FMT_DEBUG("���ϻظ�\n");
                }
                else
                {
                    UPDATE_FMT_DEBUG("֡��Ŵ���\n");
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

    //û������ȫ���ټ�����
    index = i;
    for (k = 0; k < (AC_UPDATE_NUM - index); k++)
    {
        if ((g_iUpdateCfg.ClearFlag == 1) && (i < AC_UPDATE_NUM) && (!is_last_updateSeq(g_iUpdateCfg.UpdateSeq + i - 1)))
        {
            recvLen = commdrv_Recv_232(fd_232, recvBuf, SEND_BUF_LEN, 8, 8, 0xFF);
            if (recvLen > 0)
            {
                UPDATE_BUF_DEBUG(recvBuf, recvLen, "��������֡ret[%d]", recvLen);
                outLen = 0;
                offset = 0;
                ptr = NULL;
                while ((ptr = get_07ptr_from_buf(recvBuf, recvLen, &offset, &outLen)) != NULL)
                {
                    memcpy(recvBufTmp, ptr, outLen);
                    recvLen -= (offset + outLen);
                    memmove(recvBuf, (ptr + outLen), recvLen);

                    //����recvBufTmp ����outLen
                    ret = UnPackMeterData(outLen, recvBufTmp, MeterPrtlData.MtrAddr, &MeterPrtlData.CtrlCode, &MeterPrtlData.DataLen, MeterPrtlData.DataBuff);
                    UPDATE_FMT_DEBUG("��֡��� : %d \n", ret);
                    if (ret == CHECK_RESULT_FAILUER)
                    {
                        return  ERR_ITEM;
                    }

                    if (MeterPrtlData.DataBuff[0] == 0x01 && MeterPrtlData.DataBuff[1] == 0x11
                        && MeterPrtlData.DataBuff[2] == 0xA0 && MeterPrtlData.DataBuff[3] == 0x04)
                    {
                        uint16    revseq;

                        memcpy(&revseq, &MeterPrtlData.DataBuff[4], 2);
                        UPDATE_FMT_DEBUG("������� = %d,������� = %d\n", revseq, g_iUpdateCfg.UpdateSeq + i);

                        if (revseq != (g_iUpdateCfg.UpdateSeq + i))
                        {
                            if (memcmp(&FFBuf, &revseq, 2) == 0)
                            {
                                UPDATE_FMT_DEBUG("���ϻظ�\n");
                            }
                            else
                            {
                                UPDATE_FMT_DEBUG("֡��Ŵ���\n");
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
* ��������: MakeFrameGetVerJC
* ��������: ģ��ģ��������֡�����ҷ��͵�������
* �������: fileBuf - �ļ�����
           Len - ���ݳ���
           Uis - �ļ�������Ϣ
* �������: sendbuf - ���ͱ���
* �� �� ֵ: >0                  ���ͳ���
*           <0                  ʧ��
*******************************************************************************/
int MakeFrameGetVerJC(int32 fdcry, uint8* sendbuf/*, uint8 *fileBuf, uint16 Len, JcUpdateInfoState Uis*/)
{
    METER_PRTL_FMT_T    MeterPrtlData;
    uint32              DI = 0x040005FE;
    uint16              len = 0;

    MeterPrtlData.MtrAddr[0] = 0xAA;//��ַ�̶�
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
* ��������:	UnPackMeterData
* ��������:	����Լ��֡
* �������:	FrameLen            ֡����
*           FrameBuff           ֡����
* �������:	MtrAddr             ����ַ
*           DataLen             �����򳤶�
*           DataBuff            ����������
*           CtrlCode            ������
* �� �� ֵ: CHECK_RESULT_SUCCEED
*           CHECK_RESULT_FAILUER
*           CHECK_RESULT_CONFIRM
*           CHECK_RESULT_DENY
*******************************************************************************/
int8 UnPackMeterData(uint16 FrameLen, uint8* FrameBuff, uint8* MtrAddr
    , uint8* CtrlCode, uint8* DataLen, uint8* DataBuff)
{
    uint16  DataIndex = 0xff;                   //ָ��ƫ��
    uint8   CS = 0;
    uint16   i;//start Common Problem

    //����ƫ�ƣ���λ֡ͷ
    for (i = 0; i < FrameLen; i++)
    {
        if ((FrameBuff[i] == 0x68) && (FrameBuff[i + 7] == 0x68))
        {
            DataIndex = i;
            break;
        }
    }

    //δ��λ��֡ͷ����Ϊʧ��
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

    //ˢ�µ�ַ��
    memcpy(MtrAddr, &FrameBuff[DataIndex + 1], 6);

    //������
    *CtrlCode = FrameBuff[DataIndex + 8];

    //ˢ�����ݳ���
    *DataLen = FrameBuff[DataIndex + 9];

    //�ж�֡β
    if (FrameBuff[(DataIndex + *DataLen + 11)] != 0x16)
    {
        return CHECK_RESULT_FAILUER;
    }

    //�ж�У���
    for (i = 0; i < (*DataLen + 10); i++)
    {
        CS += FrameBuff[DataIndex + i];
    }
    if (CS != FrameBuff[(DataIndex + *DataLen + 10)])
    {
        return CHECK_RESULT_FAILUER;
    }

    //ˢ�����������ݣ�ͬʱ����3�봦��
    for (i = 0; i < *DataLen; i++)
    {
        DataBuff[i] = FrameBuff[DataIndex + 10 + i] - 0x33;
    }

    //�ж��Ƿ��Ƿ���֡
    if (*CtrlCode & 0x40)
    {
        return CHECK_RESULT_DENY;
    }

    //�ж��Ƿ���ȷ��֡
    if (*DataLen == 0)
    {
        return CHECK_RESULT_CONFIRM;
    }

    return CHECK_RESULT_SUCCEED;
}

/**********************************************************************
* @name��      ac_update_file_close
* @brief��     �ر��ļ����
* @param[in] ��
* @param[out]��
* @return    ��
* @Date      ��2019-12-6
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
* ��������: JcUpdateDeal
* ��������: ������������
* �������: isGetVer ��Ҫ�а汾��
* �������: ��
* �� �� ֵ: >0                  �ɹ�
*           <0                  ʧ��
*******************************************************************************/
int JcUpdateDeal(int32 fd_232, BOOL isGetVer)
{
    static MODULE_JC_UPDATE_DEAL_T    gJcUpdateStat = CMD_IDLE;
    struct timespec  StartTime = { 0 };                          //��ʼʱ��
    struct timespec  CurrentTime = { 0 };                        //��ǰʱ��
    static uint32    StartTimeSec = 0;                         //��ʼʱ��(��)
    uint32           CurrentTimeSec = 0;                       //��ǰʱ��(��)
    static int       CommRetryCnt = 0;                         //ͨѶ���Դ���
    static  uint8    OverTimeCnt = 8;                          //��ʱʱ��
    int              RecvResult = 1;                           //���ս��(1:�ɹ� / -1:ʧ��)
    int              ret = -1;
    static int       WaitTime = 0;
    METER_PRTL_FMT_T MeterPrtlData;                            //����Լ��ʽ����
    static int       Soft_fd = -1;                             //�����ļ����
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
            UPDATE_FMT_TRACE("����ʧ��gJcUpdateStat = CMD_IDLE, fd_232 <= 0 fd_232 = %d\n", fd_232);
            return -2;
        }
        else
        {
            UPDATE_FMT_TRACE("gJcUpdateStat = CMD_IDLE, fd_232 > 0 fd_232 = %d\n", fd_232);
        }

        g_iUpdateCfg.AcqAddr[0] = 0xAA;//���ɵ�ַ�̶�
        g_iUpdateCfg.AcqAddr[1] = 0xAA;
        g_iUpdateCfg.AcqAddr[2] = 0xAA;
        g_iUpdateCfg.AcqAddr[3] = 0xAA;
        g_iUpdateCfg.AcqAddr[4] = 0xAA;
        g_iUpdateCfg.AcqAddr[5] = 0xAA;

        if (isGetVer == TRUE)//�а汾��
        {
            //�������л�֮�����ɵ��ٰ汾������
            if (gCallVerAfterBaudRateChanged)
            {
                UPDATE_FMT_DEBUG("JcUpdateDeal() �������л�֮�����ɵ��ٰ汾������... \n");
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

        UPDATE_FMT_DEBUG("��ʼ����JCͨ�ŵ�ַΪ: %02x%02x%02x%02x%02x%02x \n"
            , 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA);

        Soft_fd = open(JC_UPDATE_FILE_BACK, O_RDONLY);

        //������֡���кŴ�0��ʼ�·�
        g_iUpdateCfg.UpdateSeq = 0;

        g_iUpdateCfg.JcNum = 1;//��һ�����ɣ��˶γ�����ֲ�ڲɼ������������Դ����豸����
        //���һ�ֺ��ִμ�1
        if (g_iUpdateCfg.CurJcIndex >= g_iUpdateCfg.JcNum)
        {
            g_iUpdateCfg.CurJcIndex = 0;
            g_iUpdateCfg.iUpdateCnt++;
        }
        if (g_iUpdateCfg.iUpdateCnt > 3)
        {
            UPDATE_FMT_DEBUG("�Ѵﵽ�����ִΣ��˳�\n");
            gJcUpdateStat = CMD_END;
            break;
        }

        if (g_iUpdateCfg.iUpdateSuccCnt >= g_iUpdateCfg.JcNum)
        {
            UPDATE_FMT_DEBUG("JC������ɣ���հ汾�ţ���ʱ�����а汾��\n");
            memset(gVerBuf, 0x00, sizeof(gVerBuf));  //�������֮��汾�������㣬���ٰ汾��
            gJcUpdateStat = CMD_DELAY;
            break;
        }

        if (g_iUpdateCfg.UpdateFlag[g_iUpdateCfg.CurJcIndex] != 1)
        {
            updateStep = JC_UPDATE_START;
            UPDATE_FMT_DEBUG("��һ���а汾��\n");
            gJcUpdateStat = CMD_VER_SEND;
        }
        else
        {
            ac_update_file_close(&Soft_fd);
            UPDATE_FMT_DEBUG("�������ɹ����л���һ��JC\n");
            g_iUpdateCfg.CurJcIndex++;
        }
        break;

    case CMD_SEND:   /*��������*/
        ClearCommRecvBuff(fd_232);
        sendLen = MakeFrameUpdateJC(Soft_fd, sendBuf);
        if (sendLen <= -9)
        {
            UPDATE_FMT_DEBUG("�ļ���������ʧ��\n");
            gJcUpdateStat = CMD_FAIL;
            break;
        }
        else if (sendLen < 0)    //�������
        {
            gJcUpdateStat = CMD_SUC;
            break;
        }

        UPDATE_BUF_DEBUG(sendBuf, sendLen, "��������֡ret[%d]", sendLen);
        ret = commdrv_Send_232(fd_232, sendBuf, sendLen);
        if (ret > 0)
        {
            clock_gettime(CLOCK_MONOTONIC, &StartTime);    //ˢ�·���ʱ��
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

    case CMD_RECV:   /*��������*/
        recvLen = commdrv_Recv_232(fd_232, recvBuf, sizeof(recvBuf), 5, 5, 0xFF);
        if (recvLen > 0)
        {
            UPDATE_BUF_DEBUG(recvBuf, recvLen, "��������֡ret[%d]", recvLen);

#if AC_HIGH_UPDATE_MOD == 0
            ret = UnPackMeterData(recvLen, recvBuf
                , MeterPrtlData.MtrAddr, &MeterPrtlData.CtrlCode, &MeterPrtlData.DataLen, MeterPrtlData.DataBuff);

            UPDATE_FMT_DEBUG("��֡��� : %d \n", ret);
            if (ret == CHECK_RESULT_FAILUER)
            {
                RecvResult = -1;
                break;
            }

            /*if(memcmp(MeterPrtlData.MtrAddr,g_iUpdateCfg.AcqAddr,6) != 0)
            {
                RecvResult = -1;
                UPDATE_FMT_DEBUG("JC��ַ����\n");
                break;
            }*/

            if (MeterPrtlData.DataBuff[0] == 0x01 && MeterPrtlData.DataBuff[1] == 0x11
                && MeterPrtlData.DataBuff[2] == 0xA0 && MeterPrtlData.DataBuff[3] == 0x04)
            {
                uint16    revseq;

                memcpy(&revseq, &MeterPrtlData.DataBuff[4], 2);
                UPDATE_FMT_DEBUG("������� = %d,������� = %d\n", revseq, g_iUpdateCfg.UpdateSeq);

                if (revseq != g_iUpdateCfg.UpdateSeq)
                {
                    if (memcmp(&FFBuf, &revseq, 2) == 0)
                    {
                        UPDATE_FMT_DEBUG("���ϻظ�\n");
                    }
                    else
                    {
                        UPDATE_FMT_DEBUG("֡��Ŵ���\n");
                    }
                    RecvResult = -1;
                    break;
                }

                CommRetryCnt = 0;
                if (g_iUpdateCfg.ClearFlag == 1)
                    g_iUpdateCfg.UpdateSeq++;
                else
                    g_iUpdateCfg.ClearFlag = 1;
                gJcUpdateStat = CMD_SEND; //�л�������״̬
            }
            else
            {
                RecvResult = -1;
            }
#else
            if (parserFrameUpdateJC(fd_232, recvBuf, recvLen) != ERR_OK)
            {
                RecvResult = -1;
                CommRetryCnt++;                  //���Դ���+1
                gJcUpdateStat = CMD_RETRY;      //�л�������״̬
                break;
            }
            else
            {
                RecvResult = 1;
                CommRetryCnt = 0;
                gJcUpdateStat = CMD_SEND; //�л�������״̬
            }
#endif
        }
        else
        {
            clock_gettime(CLOCK_MONOTONIC, &CurrentTime);    //ˢ�·���ʱ��
            CurrentTimeSec = (uint32)CurrentTime.tv_sec;
            if ((CurrentTimeSec - StartTimeSec) > OverTimeCnt)
            {
                RecvResult = -1;
            }
        }

        if (RecvResult == -1)            //����ʧ��
        {
            CommRetryCnt++;                  //���Դ���+1
            gJcUpdateStat = CMD_RETRY;      //�л�������״̬
        }
        break;

    case CMD_RETRY:  /*���ͳ���*/
        if (CommRetryCnt > 3)
        {
            CommRetryCnt = 0;
            gJcUpdateStat = CMD_FAIL;//����ʧ��
            break;
        }

        //�л�������״̬
        gJcUpdateStat = CMD_SEND;
        if (isGetVer == TRUE)
        {
            gJcUpdateStat = CMD_VER_SEND;
        }
        break;

    case CMD_DELAY:  /*��������ʱ*/
        updateStep = JC_UPDATE_END;

        //�������������оƬ������ʱ�ų�
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

    case CMD_SUC:  /*�����ɹ�*/
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
            UPDATE_FMT_DEBUG("�ļ���������ʧ��\n");
            gJcUpdateStat = CMD_FAIL;
            break;
        }
        else if (sendLen < 0)    //�������
        {
            gJcUpdateStat = CMD_SUC;
            break;
        }

        ClearCommRecvBuff(fd_232);
        ret = commdrv_Send_232(fd_232, sendBuf, sendLen);
        UPDATE_BUF_DEBUG(sendBuf, sendLen, "���Ͱ汾֡:");
        UPDATE_FMT_DEBUG("fd_232 = %d commdrv_Send_232 ret = %d\n", fd_232, ret);
        if (ret > 0)
        {
            clock_gettime(CLOCK_MONOTONIC, &StartTime);    //ˢ�·���ʱ��
            StartTimeSec = (uint32)StartTime.tv_sec;
            gJcUpdateStat = CMD_VER_RECV;
        }
        break;

    case CMD_VER_RECV:
        recvLen = commdrv_Recv_232(fd_232, recvBuf, sizeof(recvBuf), 5, 5, 0xFF);
        if (recvLen > 0)
        {
            UPDATE_BUF_DEBUG(recvBuf, recvLen, "���ܰ汾֡:");
            ret = UnPackMeterData(recvLen, recvBuf
                , MeterPrtlData.MtrAddr, &MeterPrtlData.CtrlCode, &MeterPrtlData.DataLen, MeterPrtlData.DataBuff);

            UPDATE_FMT_DEBUG("��֡��� : %d \n", ret);
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

                UPDATE_FMT_DEBUG("����ģ��汾��VER = V%c%c%c%c\n", gVerBuf[0], gVerBuf[1], gVerBuf[2], gVerBuf[3]);

#ifdef AREA_SHANDONG
                if (MeterPrtlData.DataLen == 10)
                {
                    memcpy(&gVerBuf[4], &MeterPrtlData.DataBuff[8], 2);
                    UPDATE_FMT_DEBUG("���ɼ����汾��VER = %c%c\n", gVerBuf[4], gVerBuf[5]);
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
                    gJcUpdateStat = CMD_SEND; //�л�������״̬
                }
                else if (updateStep == JC_UPDATE_END)
                {
                    gJcUpdateStat = CMD_END; //�л�������״̬
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
            clock_gettime(CLOCK_MONOTONIC, &CurrentTime);    //ˢ�·���ʱ��
            CurrentTimeSec = (uint32)CurrentTime.tv_sec;
            if ((CurrentTimeSec - StartTimeSec) > OverTimeCnt)
            {
                RecvResult = -1;
            }
        }

        if (RecvResult == -1)            //����ʧ��
        {
            CommRetryCnt++;                  //���Դ���+1
            gJcUpdateStat = CMD_RETRY;      //�л�������״̬
        }
        break;

    case CMD_END:    /*��������*/
        ac_update_file_close(&Soft_fd);
        sprintf(TmpPath, "rm -rf %s", JC_UPDATE_FILE_BACK);
        system(TmpPath);
        if (g_iUpdateCfg.iUpdateCnt > 3)
        {
            UPDATE_FMT_DEBUG("%s, rm -rf %s", "JC����ʧ��", JC_UPDATE_FILE_BACK);
        }
        else
        {
            UPDATE_FMT_DEBUG("%s, rm -rf %s", "JC�����ɹ�", JC_UPDATE_FILE_BACK);
        }
        memset(&g_iUpdateCfg, 0x0, sizeof(JC_UPDATE_CFG_T));
        gJCUpdateFileMark = 0;
        gJcUpdateStat = CMD_IDLE;
        updateStep = JC_UPDATE_IDLE;

        //�ӽڵ�������ɺ�ˢ�°汾�ţ����³����汾��
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