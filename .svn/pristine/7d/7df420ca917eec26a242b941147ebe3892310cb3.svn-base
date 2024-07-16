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
#include "ac.h"
#include "ac_update.h"
#include "ac_update698.h"
#include "ac_698msg.h"
#include "ac_report.h"

#if DESC("698交采升级相关代码", 1)

//交采升级
JC_UPDATE698_CFG_T g_iUpdate698Cfg = {0};

#if UPDATE_SELF_ADAPTION == 1
BOOL gIsUpdate698 = FALSE;
#endif

/*******************************************************************************
* 函数名称: ac_get_frame_ver
* 函数功能: 组织获取版本报文
* 输入参数: 
* 输出参数: sendbuf - 发送报文
* 返 回 值: 大于 0    发送长度
*          小于 0     失败
*******************************************************************************/
int ac_get_frame_ver(uint8* sendbuf, uint32 maxLen, uint8 logic)
{
    OOP_OAD_U oad = { 0 };

    oad.value = 0x43000300;

    return ac_get_msg_package(oad, sendbuf, maxLen, logic);
}

/**********************************************************************
* @name：      ac_get_parser_ver
* @brief：     数据解析
* @param[in] ：recvBuf - 报文输入
*              recvLen - 报文长度
* @param[out]：
* @return    ：
* @Date      ：2019-12-6
**********************************************************************/
int ac_get_parser_ver(uint8* recvBuf, uint32 recvLen)
{
    return ac_read_sample_parser(SAMPLE_OTHER, recvBuf, recvLen);
}

/*******************************************************************************
* 函数名称: ac_get_698ver
* 函数功能: 获取版本号
* 输入参数: fd_spi 操作句柄
* 输出参数: 无
* 返 回 值: >0    成功
*           <0    失败
*******************************************************************************/
int ac_get_698ver(int32 *pfd)
{
    int   ret = -1;
    int32 recvLen = 0;
    int32 sendLen = 0;
    uint8 recvBuf[1024] = { 0 };
    uint8 sendBuf[1024] = { 0 };

    uint8 verbuf[64] = {0};

    int32 fd_spi = *pfd;

    if (fd_spi < 0)
    {
        fd_spi = ac_sample_dev_open();
        if (fd_spi < 0)
        {
            return ERR_NO_HANDLE;
        }
        else
        {
            *pfd = fd_spi;
            UPDATE_FMT_DEBUG("msg: the spi open ok.\n");
        }
    }

    sendLen = ac_get_frame_ver(sendBuf, 1024, 0);
    if (sendLen < 0)    //发送完成
    {
        return ERR_NORMAL;
    }

    if (gUartConf.enable)
    {
        ret = commdrv_Send_232(fd_spi, sendBuf, sendLen);
    }
    else
    {
        ret = ac_spi_send_data(fd_spi, sendBuf, sendLen, recvBuf, 500000);
    }
    
    if (ret < 0)
    {
        UPDATE_FMT_DEBUG("fd_spi = %d send ret = %d\n", fd_spi, ret);
        ac_sample_dev_close(pfd);
        return ERR_IFACE;
    }
    else
    {
        UPDATE_BUF_DEBUG(sendBuf, sendLen, "发送版本帧");
    }

    if (gUartConf.enable)
    {
        recvLen = commdrv_Recv_232(fd_spi, recvBuf, sizeof(recvBuf), 150, 5, 0xFF);
    }
    else
    {
        recvLen = ac_spi_recv_data_timeout(fd_spi, sendBuf, recvBuf, sizeof(recvBuf), 50000, 65);
    }
    
    if (recvLen > 0)
    {
        UPDATE_BUF_DEBUG(recvBuf, recvLen, "接受版本帧");
        ret = ac_get_parser_ver(recvBuf, recvLen);
        if (ret != ERR_OK)
        {
            UPDATE_FMT_DEBUG("版本解帧结果异常[%d]\n", ret);
            return ret;
        }
        else
        {
            memcpy(verbuf, gVerSBuf, MIN(sizeof(verbuf) - 1, sizeof(gVerSBuf)));
            UPDATE_FMT_DEBUG("交采模块版本号VER = V%c%c%c%c\n", gVerBuf[0], gVerBuf[1], gVerBuf[2], gVerBuf[3]);
            UPDATE_FMT_DEBUG("交采模块版本日期DATE = %c%c%c%c%c%c\n", gDateBuf[0], gDateBuf[1], gDateBuf[2], gDateBuf[3], gDateBuf[4], gDateBuf[5]);
            UPDATE_FMT_DEBUG("交采模块小版本号VERS = %s\n", verbuf);
            memcpy(gVerBufLast, gVerBuf, 4);
            memcpy(gDateBufLast, gDateBuf, 6);
            memcpy(gVerSBufLast, gVerSBuf, 8);
        }
    }

    return ERR_OK;
}

/*******************************************************************************
* 函数名称: ac_get_698ver_bak
* 函数功能: 获取版本号(只为判断底板是否支持698协议)
* 输入参数: fd_spi 操作句柄
* 输出参数: 无
* 返 回 值: >0    成功
*           <0    失败
*******************************************************************************/
int ac_get_698ver_bak(int32 *pfd)
{
    int   ret = -1;
    int32 recvLen = 0;
    int32 sendLen = 0;
    uint8 recvBuf[1024] = { 0 };
    uint8 sendBuf[1024] = { 0 };

    int32 fd_spi = *pfd;

    if (fd_spi < 0)
    {
        fd_spi = ac_sample_dev_open();
        if (fd_spi < 0)
        {
            return ERR_NO_HANDLE;
        }
        else
        {
            *pfd = fd_spi;
            UPDATE_FMT_DEBUG("msg: the spi open ok.\n");
        }
    }

    sendLen = ac_get_frame_ver(sendBuf, 1024, 0);
    if (sendLen < 0)    //发送完成
    {
        return ERR_NORMAL;
    }

    if (gUartConf.enable)
    {
        ret = commdrv_Send_232(fd_spi, sendBuf, sendLen);
    }
    else
    {
        ret = ac_spi_send_data(fd_spi, sendBuf, sendLen, recvBuf, 500000);
    }
    
    if (ret < 0)
    {
        INIT_FMT_DEBUG("fd_spi = %d send ret = %d\n", fd_spi, ret);
        ac_sample_dev_close(pfd);
        return ERR_IFACE;
    }
    else
    {
        INIT_BUF_DEBUG(sendBuf, sendLen, "验证底板是否支持698协议，发送");
    }

    if (gUartConf.enable)
    {
        recvLen = commdrv_Recv_232(fd_spi, recvBuf, sizeof(recvBuf), 150, 5, 0xFF);
    }
    else
    {
        recvLen = ac_spi_recv_data_timeout(fd_spi, sendBuf, recvBuf, sizeof(recvBuf), 50000, 65);
    }
    
    if (recvLen > 0)
    {
        INIT_BUF_DEBUG(recvBuf, recvLen, "验证底板是否支持698协议，接受");
        ret = ac_get_parser_ver(recvBuf, recvLen);
        if (ret != ERR_OK)
        {
            INIT_FMT_DEBUG("解帧结果异常[%d]\n", ret);
            return ret;
        }
    }
    else
    {
        return ERR_TIMEOUT;
    }

    return ERR_OK;
}

/*******************************************************************************
* 函数名称: ac_get_frame_ver
* 函数功能: 组织获取传输状态报文
* 输入参数:
* 输出参数: sendbuf - 发送报文
* 返 回 值: 大于 0    发送长度
*          小于 0     失败
*******************************************************************************/
int ac_get_frame_trans_status(uint8* sendbuf, uint32 maxLen, uint8 logic)
{
    OOP_OAD_U oad = { 0 };

    oad.value = 0xf0010400;

    return ac_get_msg_package(oad, sendbuf, maxLen, logic);
}

/**********************************************************************
* @name：      ac_get_parser_ver
* @brief：     数据解析
* @param[in] ：recvBuf - 报文输入
*              recvLen - 报文长度
* @param[out]：
* @return    ：
* @Date      ：2019-12-6
**********************************************************************/
int ac_get_parser_trans_status(uint8* recvBuf, uint32 recvLen)
{
    return ac_read_sample_parser(SAMPLE_OTHER, recvBuf, recvLen);
}


/*******************************************************************************
* 函数名称: ac_get_trans_status_unit
* 函数功能: 获取升级状态字
* 输入参数: fd_spi 操作句柄
* 输出参数: 无
* 返 回 值: >0    成功
*           <0    失败
*******************************************************************************/
int ac_get_trans_status_unit(int32 fd_spi, uint8 logic)
{
    int   ret = -1;
    int   i = 0;
    int   j = 0;
    int   numByte = 0;
    int   numBit = 0;
    int32 recvLen = 0;
    int32 sendLen = 0;
    uint8 recvBuf[1024] = { 0 };
    uint8 sendBuf[1024] = { 0 };

    if (fd_spi < 0)
    {
        return ERR_NO_HANDLE;
    }

    sendLen = ac_get_frame_trans_status(sendBuf, 1024, logic);
    if (sendLen < 0)    //发送完成
    {
        return ERR_NORMAL;
    }

    if (gUartConf.enable)
    {
        ret = commdrv_Send_232(fd_spi, sendBuf, sendLen);
    }
    else
    {
        ret = ac_spi_send_data(fd_spi, sendBuf, sendLen, recvBuf, 500000);
    }
    
    if (ret <= 0)
    {
        UPDATE_FMT_DEBUG("fd_spi = %d send ret = %d\n", fd_spi, ret);
        return ERR_IFACE;
    }
    else
    {
        UPDATE_BUF_DEBUG(sendBuf, sendLen, "发送传输字");
    }

    if (gUartConf.enable)
    {
        recvLen = commdrv_Recv_232(fd_spi, recvBuf, sizeof(recvBuf), 150, 5, 0xFF);
    }
    else
    {
        recvLen = ac_spi_recv_data_timeout(fd_spi, sendBuf, recvBuf, sizeof(recvBuf), 50000, 65);
    }
    
    if (recvLen > 0)
    {
        UPDATE_BUF_DEBUG(recvBuf, recvLen, "接受传输字");
        ret = ac_get_parser_trans_status(recvBuf, recvLen);
        if (ret != ERR_OK)
        {
            UPDATE_FMT_DEBUG("传输状态字解帧结果异常[%d]\n", ret);
            return ret;
        }
        else
        {
            UPDATE_BUF_DEBUG(g_iUpdate698Cfg.transStatus, g_iUpdate698Cfg.transStatusNum, "传输状态字");

            numByte = g_iUpdate698Cfg.UpdateSeq / 8;
            numBit = g_iUpdate698Cfg.UpdateSeq % 8;

            for (i = 0; i < numByte; i++)
            {
                if (g_iUpdate698Cfg.transStatus[i] != 0xff)
                {
                    return ERR_FORMAT;
                }
            }

            if (numBit != 0)
            {
                for (j = 0; j < numBit; j++)
                {
                    if ((g_iUpdate698Cfg.transStatus[i] & (0x80 >> j)) == 0x00)
                    {
                        return ERR_FORMAT;
                    }
                }
            }
        }
    }

    return ERR_OK;
}

/*******************************************************************************
* 函数名称: ac_get_trans_status
* 函数功能: 获取升级状态字
* 输入参数: fd_spi 操作句柄
* 输出参数: 无
* 返 回 值: >0    成功
*           <0    失败
*******************************************************************************/
int ac_get_trans_status(int32 fd_spi, uint8 logic)
{
    int i = 0;
    int ret = ERR_NORMAL;

    for (i = 0; i < 2; i++)
    {
        ret = ac_get_trans_status_unit(fd_spi, logic);
        if (ERR_OK == ret)
        {
            break;
        }
    }

    return ret;
}

#if UPDATE_SELF_ADAPTION == 1
/**********************************************************************
* @name：      ac_get_parser_support_update698
* @brief：     数据解析
* @param[in] ：recvBuf - 报文输入
*              recvLen - 报文长度
* @param[out]：
* @return    ：错误码
* @Date      ：2019-12-6
**********************************************************************/
int ac_get_parser_support_update698(uint8* recvBuf, uint32 recvLen)
{
    int    ret = -1;
    FRAME_HEAD_T tFrame = { 0 };
    uint8* ptr = NULL;
    uint32  outLen = 0;
    uint32  offset = 0;
    uint8   recvBufTmp[RECV_BUF_LEN] = { 0 };
    OOP_OAD_U oad = { 0 };

    while ((ptr = get_68ptr_from_buf(recvBuf, recvLen, &offset, &outLen)) != NULL)
    {
        memcpy(recvBufTmp, ptr, outLen);

        recvLen -= (offset + outLen);
        memmove(recvBuf, (ptr + outLen), recvLen);

        /* 合法性检查 */
        ret = app_check_message(recvBufTmp, outLen);
        if (0 != ret)
        {
            ACMSG_BUF_DEBUG(recvBufTmp, outLen, "app_check_message error[%d] msg recv", ret);
            continue;
        }

        /* 解析帧头 */
        memset(&tFrame, 0, sizeof(FRAME_HEAD_T));
        ret = app_get_frame(recvBufTmp, outLen, &tFrame, TRUE);
        if (0 != ret)
        {
            ACMSG_BUF_DEBUG(recvBufTmp, outLen, "app_get_frame error[%d] msg recv", ret);
            continue;
        }

        memcpy_r(&oad.value, &tFrame.pApdu[3], sizeof(oad.value));
        if (oad.value != 0xf0010400)
        {
            ACMSG_BUF_DEBUG(recvBufTmp, outLen, "oad error[%d] msg recv", ret);
            continue;
        }

        if (tFrame.pApdu[7] == 0)
        {
            return ERR_TYPE;
        }
        else if (tFrame.pApdu[7] == 1)
        {
            break;
        }
    }

    if (ptr == NULL)
    {
        return ERR_ITEM;
    }

    return ERR_OK;
}

/*******************************************************************************
* 函数名称: ac_is_support_update698_unit
* 函数功能: 是否支持698升级
* 输入参数: fd_spi 操作句柄
* 输出参数: 无
* 返 回 值: >0    成功
*           <0    失败
*******************************************************************************/
BOOL ac_is_support_update698(int32 fd_spi)
{
    int   ret = -1;
    int32 recvLen = 0;
    int32 sendLen = 0;
    uint8 recvBuf[1024] = { 0 };
    uint8 sendBuf[1024] = { 0 };

    if (fd_spi < 0)
    {
        return FALSE;
    }

    sendLen = ac_get_frame_trans_status(sendBuf, 1024, 0);
    if (sendLen < 0)    //发送完成
    {
        return FALSE;
    }

    if (gUartConf.enable)
    {
        ret = commdrv_Send_232(fd_spi, sendBuf, sendLen);
    }
    else
    {
        ret = ac_spi_send_data(fd_spi, sendBuf, sendLen, recvBuf, 500000);
    }
    
    if (ret <= 0)
    {
        ACSAMP_FMT_DEBUG("fd_spi = %d send ret = %d\n", fd_spi, ret);
        return FALSE;
    }
    else
    {
        ACSAMP_BUF_DEBUG(sendBuf, sendLen, "发送传输字");
    }

    if (gUartConf.enable)
    {
        recvLen = commdrv_Recv_232(fd_spi, recvBuf, sizeof(recvBuf), 150, 5, 0xFF);
    }
    else
    {
        recvLen = ac_spi_recv_data_timeout(fd_spi, sendBuf, recvBuf, sizeof(recvBuf), 50000, 65);
    }
    
    if (recvLen > 0)
    {
        ACSAMP_BUF_DEBUG(recvBuf, recvLen, "接受传输字");
        ret = ac_get_parser_support_update698(recvBuf, recvLen);
        if (ret != ERR_OK)
        {
            ACSAMP_FMT_DEBUG("不支持698升级功能[%d]\n", ret);
            return FALSE;
        }
        else
        {
            ACSAMP_FMT_DEBUG("支持698升级功能\n");
        }
    }

    return TRUE;
}
#endif

/*******************************************************************************
* 函数名称: ac_act_frame_trans_start
* 函数功能: 组织启动传输报文
* 输入参数:
* 输出参数: sendbuf - 发送报文
* 返 回 值: 大于 0    发送长度
*          小于 0     失败
*******************************************************************************/
int ac_act_frame_trans_start(int fdcry, uint8* sendbuf, uint32 maxLen, uint8 logic)
{
    OOP_OMD_U   omd = {0};
    struct stat statbuf = {0};
    uint8       inData[256] = {0};
    uint32      inLen = 0;
    uint32      offset = 0;

    omd.value = 0xf0010700;

    if (fdcry < 0)
    {
        UPDATE_FMT_DEBUG("传输后行帧的时候，找不到文件\n");
        return -1;
    }

    if (stat(JC_UPDATE_FILE_BACK, &statbuf) < 0)
    {
        UPDATE_FMT_DEBUG("传输后行帧的时候，获得不到文件大小\n");
        return -2;
    }

    inData[offset++] = DT_STRUCTURE;
    inData[offset++] = 3;
    inData[offset++] = DT_STRUCTURE;
    inData[offset++] = 6;

    //源文件
    inData[offset++] = DT_VISIBLE_STRING;
    inData[offset++] = 0;

    //目的文件
    inData[offset++] = DT_VISIBLE_STRING;
    inData[offset++] = 0;

    //文件大小
    inData[offset++] = DT_DOUBLE_LONG_UNSIGNED;
    memcpy_r(&inData[offset], &statbuf.st_size, 4);
    offset += 4;

    //文件属性
    inData[offset++] = DT_BIT_STRING;
    inData[offset++] = 0x03;
    inData[offset++] = 0xe0;

    //版本
    inData[offset++] = DT_VISIBLE_STRING;
    inData[offset++] = 0;

    //类型
    inData[offset++] = DT_ENUM;
    inData[offset++] = 0;

    //传输块大小，默认1024
    inData[offset++] = DT_LONG_UNSIGNED;
    inData[offset++] = PACKLEN / 256;
    inData[offset++] = PACKLEN % 256;

    inData[offset++] = DT_STRUCTURE;
    inData[offset++] = 2;

    //类型
    inData[offset++] = DT_ENUM;
    inData[offset++] = 0;

    inData[offset++] = DT_OCTET_STRING;
    inData[offset++] = 0;

    inLen = offset;

    return ac_act_msg_package(omd, inData, inLen, sendbuf, maxLen, logic);
}


/**********************************************************************
* @name：      ac_act_parser_trans_start
* @brief：     数据解析
* @param[in] ：recvBuf - 报文输入
*              recvLen - 报文长度
* @param[out]：
* @return    ：错误码
* @Date      ：2019-12-6
**********************************************************************/
int ac_act_parser_trans_start(uint8* recvBuf, uint32 recvLen)
{
    int    ret = -1;
    FRAME_HEAD_T tFrame = {0};
    uint8* ptr = NULL;
    uint32  outLen = 0;
    uint32  offset = 0;
    uint8   recvBufTmp[RECV_BUF_LEN] = { 0 };

    while ((ptr = get_68ptr_from_buf(recvBuf, recvLen, &offset, &outLen)) != NULL)
    {
        memcpy(recvBufTmp, ptr, outLen);

        recvLen -= (offset + outLen);
        memmove(recvBuf, (ptr + outLen), recvLen);

        /* 合法性检查 */
        ret = app_check_message(recvBufTmp, outLen);
        if (0 != ret)
        {
            ACMSG_BUF_DEBUG(recvBufTmp, outLen, "app_check_message error[%d] msg recv", ret);
            continue;
        }

        /* 解析帧头 */
        memset(&tFrame, 0, sizeof(FRAME_HEAD_T));
        ret = app_get_frame(recvBufTmp, outLen, &tFrame, TRUE);
        if (0 != ret)
        {
            ACMSG_BUF_DEBUG(recvBufTmp, outLen, "app_get_frame error[%d] msg recv", ret);
            continue;
        }

        if (tFrame.pApdu[7] != DATA_SUCCESS)
        {
            return tFrame.pApdu[7];
        }
        else
        {
            break;
        }
    }

    if (ptr == NULL)
    {
        return ERR_ITEM;
    }

    return ERR_OK;
}

/*******************************************************************************
* 函数名称: ac_act_frame_trans_run
* 函数功能: 传输报文
* 输入参数:
* 输出参数: sendbuf - 发送报文
* 返 回 值: 大于 0    发送长度
*          小于 0     失败
*******************************************************************************/
int ac_act_frame_trans_run(int fdcry, uint8* sendbuf, uint32 maxLen, uint8 logic)
{
    int         PackLen = PACKLEN;
    int         PackNum = 0;
    OOP_OMD_U   omd = { 0 };
    struct stat statbuf = { 0 };
    uint8       inData[SEND_APDU_LEN] = { 0 };
    uint8       fileDate[PACKLEN] = { 0 };
    uint32      inLen = 0;
    uint32      offset = 0;

    omd.value = 0xf0010800;

    if (fdcry < 0)
    {
        UPDATE_FMT_DEBUG("传输后行帧的时候，找不到文件");
        return -1;
    }

    if (stat(JC_UPDATE_FILE_BACK, &statbuf) < 0)
    {
        UPDATE_FMT_DEBUG("传输后行帧的时候，获得不到文件大小");
        return -2;
    }

    //获得所有的包数，也即序列号
    if (statbuf.st_size % PackLen)
        PackNum = statbuf.st_size / PackLen + 1;
    else
        PackNum = statbuf.st_size / PackLen;

    if (g_iUpdate698Cfg.UpdateSeq >= PackNum)
        return 0;

    lseek(fdcry, g_iUpdate698Cfg.UpdateSeq * PackLen, SEEK_SET);

    if (g_iUpdate698Cfg.UpdateSeq == (PackNum - 1))
    {
        if (statbuf.st_size % PackLen)
            PackLen = (statbuf.st_size % PackLen);
        read(fdcry, fileDate, PackLen);
    }
    else
        read(fdcry, fileDate, PackLen);

    inData[offset++] = DT_STRUCTURE;
    inData[offset++] = 2;

    inData[offset++] = DT_LONG_UNSIGNED;
    inData[offset++] = g_iUpdate698Cfg.UpdateSeq/256;
    inData[offset++] = g_iUpdate698Cfg.UpdateSeq%256;

    inData[offset++] = DT_OCTET_STRING;
    if (PackLen < 0x80)
    {
        inData[offset++] = (uint8)PackLen;
    }
    else
    {
        inData[offset++] = 0x82;
        inData[offset++] = (uint8)(PackLen >> 8);
        inData[offset++] = (uint8)PackLen;
    }

    memcpy(&inData[offset], fileDate, PackLen);
    offset += PackLen;

    inLen = offset;
    
    UPDATE_FMT_DEBUG("progress[%d/%d] logic[%d]\n", g_iUpdate698Cfg.UpdateSeq, PackNum, logic);

    return ac_act_msg_package(omd, inData, inLen, sendbuf, maxLen, logic);
}

/**********************************************************************
* @name：      ac_act_parser_trans_run
* @brief：     数据解析
* @param[in] ：recvBuf - 报文输入
*              recvLen - 报文长度
* @param[out]：
* @return    ：错误码
* @Date      ：2019-12-6
**********************************************************************/
int ac_act_parser_trans_run(uint8* recvBuf, uint32 recvLen)
{
    int    ret = -1;
    FRAME_HEAD_T tFrame = { 0 };
    uint8* ptr = NULL;
    uint32  outLen = 0;
    uint32  offset = 0;
    uint8   recvBufTmp[RECV_BUF_LEN] = { 0 };

    while ((ptr = get_68ptr_from_buf(recvBuf, recvLen, &offset, &outLen)) != NULL)
    {
        memcpy(recvBufTmp, ptr, outLen);

        recvLen -= (offset + outLen);
        memmove(recvBuf, (ptr + outLen), recvLen);

        /* 合法性检查 */
        ret = app_check_message(recvBufTmp, outLen);
        if (0 != ret)
        {
            ACMSG_BUF_DEBUG(recvBufTmp, outLen, "app_check_message error[%d] msg recv", ret);
            continue;
        }

        /* 解析帧头 */
        memset(&tFrame, 0, sizeof(FRAME_HEAD_T));
        ret = app_get_frame(recvBufTmp, outLen, &tFrame, TRUE);
        if (0 != ret)
        {
            ACMSG_BUF_DEBUG(recvBufTmp, outLen, "app_get_frame error[%d] msg recv", ret);
            continue;
        }

        if (tFrame.pApdu[7] != DATA_SUCCESS)
        {
            return tFrame.pApdu[7];
        }
        else
        {
            break;
        }
    }

    if (ptr == NULL)
    {
        return ERR_ITEM;
    }

    g_iUpdate698Cfg.UpdateSeq++;

    return ERR_OK;
}

/*******************************************************************************
* 函数名称: ac_update_idle_proc
* 函数功能: 交采升级预处理
* 输入参数: fd_spi      设备句柄
*          pfile       升级文件句柄
* 输出参数: pUpdateStat 状态机
*          pUpdateStep 升级状态
* 返 回 值: 无
*******************************************************************************/
void ac_update_idle_proc(int32 fd_spi, int *pfile, JC_UPDATE698_DEAL_T *pUpdateStat, JC_UPDATE_STEP_E *pUpdateStep)
{
    int Soft_fd = *pfile;

    if (fd_spi < 0)
    {
        UPDATE_FMT_DEBUG("WARN: sJcUpdateStat[%d], fd_spi[%d]\n", CMD698_IDLE, fd_spi);
        return;
    }

    if (Soft_fd < 0)
    {
        Soft_fd = open(JC_UPDATE_FILE_BACK, O_RDONLY);
        if (Soft_fd < 0)
        {
            UPDATE_FMT_DEBUG("WARN: sJcUpdateStat[%d], Soft_fd[%d]\n", CMD698_IDLE, Soft_fd);
            return;
        }
        *pfile = Soft_fd;
    }
    else
    {
        UPDATE_FMT_TRACE("WARN: soft_fd existence, Soft_fd[%d]\n", Soft_fd);
        return;
    }

    //升级的帧序列号从0开始下发
    g_iUpdate698Cfg.UpdateSeq = 0;

    //完成一轮后轮次加1
    if (g_iUpdate698Cfg.CurJcIndex > 0)
    {
        g_iUpdate698Cfg.CurJcIndex = 0;
        g_iUpdate698Cfg.iUpdateCnt ++;
    }
    if (g_iUpdate698Cfg.iUpdateCnt > 3)
    {
        UPDATE_FMT_DEBUG("已达到重试轮次，退出\n");
        *pUpdateStat = CMD698_END;
        return;
    }

    if (g_iUpdate698Cfg.iUpdateSuccCnt > 0)
    {
        UPDATE_FMT_DEBUG("JC升级完成，清空版本号，延时，再招版本号\n");
        memset(gVerBuf, 0x00, sizeof(gVerBuf));  //升级完成之后版本号先清零，再召版本号
        *pUpdateStat = CMD698_DELAY;
        return;
    }

    if (g_iUpdate698Cfg.UpdateFlag != 1)
    {
        *pUpdateStep = JC_UPDATE_START;
        UPDATE_FMT_DEBUG("第一次招版本号\n");
        *pUpdateStat = CMD698_VER_SEND;
    }
    else
    {
        ac_update_file_close(pfile);
        UPDATE_FMT_DEBUG("已升级成功，切换下一个状态\n");
        g_iUpdate698Cfg.CurJcIndex ++;
    }

    return;
}

/*******************************************************************************
* 函数名称: ac_update_idle_proc
* 函数功能: 交采升级预处理
* 输入参数: fd_spi      设备句柄
*          pfile       升级文件句柄
* 输出参数: pUpdateStat 状态机
*          pUpdateStep 升级状态
* 返 回 值: 无
*******************************************************************************/
void ac_update_end_proc(int32 fd_spi, int *pfile, JC_UPDATE698_DEAL_T *pUpdateStat, JC_UPDATE_STEP_E *pUpdateStep)
{
    char TmpPath[128] = {0};

    ac_update_file_close(pfile);
    sprintf(TmpPath, "rm -rf %s", JC_UPDATE_FILE_BACK);
    system(TmpPath);
    sprintf(TmpPath, "rm -rf %s%s*", UPDATE_PATH_WITH_VER, UPDATE_FILE_NAME);
    system(TmpPath);
    if (g_iUpdate698Cfg.iUpdateCnt > 3)
    {
        UPDATE_FMT_DEBUG("%s, rm -rf %s\n", "JC升级失败", JC_UPDATE_FILE_BACK);
    }
    else
    {
        UPDATE_FMT_DEBUG("%s, rm -rf %s\n", "JC升级成功", JC_UPDATE_FILE_BACK);
    }
    memset(&g_iUpdate698Cfg, 0x0, sizeof(JC_UPDATE_CFG_T));
    gJCUpdateFileMark = 0;
    *pUpdateStat = CMD698_IDLE;
    *pUpdateStep = JC_UPDATE_IDLE;
}

/*******************************************************************************
* 函数名称: ac_ver_print
* 函数功能: 版本打印
* 输入参数: 无
* 输出参数: 无
* 返 回 值: 无
*******************************************************************************/
void ac_ver_print(void)
{
    uint8 verbuf[64] = {0};

    memcpy(verbuf, gVerSBufLast, MIN(sizeof(verbuf) - 1, sizeof(gVerSBufLast)));
    UPDATE_FMT_DEBUG("--------------------升级前的交采模块信息--------------------\n");
    UPDATE_FMT_DEBUG("交采模块版本号VER = V%c%c%c%c\n", gVerBufLast[0], gVerBufLast[1], gVerBufLast[2], gVerBufLast[3]);
    UPDATE_FMT_DEBUG("交采模块版本日期DATE = %c%c%c%c%c%c\n", gDateBufLast[0], gDateBufLast[1], gDateBufLast[2], gDateBufLast[3],
        gDateBufLast[4], gDateBufLast[5]);
    UPDATE_FMT_DEBUG("交采模块小版本号VERS = %s\n", gVerSBufLast);
    UPDATE_FMT_DEBUG("------------------------------------------------------------\n");
    
    memset(verbuf, 0, sizeof(verbuf));
    memcpy(verbuf, gVerSBuf, MIN(sizeof(verbuf) - 1, sizeof(gVerSBuf)));
    UPDATE_FMT_DEBUG("--------------------升级后的交采模块信息--------------------\n");
    UPDATE_FMT_DEBUG("交采模块版本号VER = V%c%c%c%c\n", gVerBuf[0], gVerBuf[1], gVerBuf[2], gVerBuf[3]);
    UPDATE_FMT_DEBUG("交采模块版本日期DATE = %c%c%c%c%c%c\n", gDateBuf[0], gDateBuf[1], gDateBuf[2], gDateBuf[3], gDateBuf[4],
        gDateBuf[5]);
    UPDATE_FMT_DEBUG("交采模块小版本号VERS = %s\n", gVerSBuf);
    UPDATE_FMT_DEBUG("------------------------------------------------------------\n");
}

/*******************************************************************************
* 函数名称: ac_update698_proc
* 函数功能: 交采升级处理
* 输入参数: fd_spi 设备句柄
* 输出参数: 无
* 返 回 值: 错误码
*******************************************************************************/
int ac_update698_proc(int32 *pfd)
{
    static JC_UPDATE698_DEAL_T    sJcUpdateStat = CMD698_IDLE;
    struct timespec  StartTime = { 0 };                          //开始时间
    struct timespec  CurrentTime = { 0 };                        //当前时间
    static uint32    StartTimeSec = 0;                         //开始时间(秒)
    uint32           CurrentTimeSec = 0;                       //当前时间(秒)
    static int       CommRetryCnt = 0;                         //通讯重试次数
    static  uint8    OverTimeCnt = 8;                          //超时时间
    int              RecvResult = 1;                           //接收结果(1:成功 / -1:失败)
    int              ret = -1;
    static int       WaitTime = 0;
    static int       Soft_fd = -1;                             //升级文件句柄
    int32            fd_spi = *pfd;
    uint8            logic = 0;

    int32 recvLen = 0;
    int32 sendLen = 0;
    uint8 recvBuf[RECV_BUF_LEN] = { 0 };
    uint8 sendBuf[SEND_BUF_LEN] = { 0 };

    static JC_UPDATE_STEP_E sJcUpdateStep = JC_UPDATE_IDLE;

    if (fd_spi < 0)
    {
        fd_spi = ac_sample_dev_open();
        if (fd_spi < 0)
        {
            return -1;
        }
        else
        {
            *pfd = fd_spi;
            UPDATE_FMT_DEBUG("msg: the spi open ok.\n");
        }
    }

    if (gJCUpdateFileMark == 0x0b)
    {
        logic = TOPO_LOGIC_ADDR;
    }

    switch (sJcUpdateStat)
    {
    case CMD698_IDLE:
        ac_update_idle_proc(fd_spi, &Soft_fd, &sJcUpdateStat, &sJcUpdateStep);
        break;

    case CMD698_SEND:   /*升级发送*/
        sJcUpdateStep = JC_UPDATE_RUN;
        ClearCommRecvBuff(fd_spi);
        sendLen = ac_act_frame_trans_run(Soft_fd, sendBuf, SEND_BUF_LEN, logic);
        if (sendLen < 0)
        {
            UPDATE_FMT_DEBUG("文件错误，升级失败\n");
            sJcUpdateStat = CMD698_FAIL;
            break;
        }
        else if (sendLen == 0)    //发送完成
        {
            if (ERR_OK != ac_get_trans_status(fd_spi, logic))
            {
                UPDATE_FMT_DEBUG("文件传输错误，升级失败\n");
                sJcUpdateStat = CMD698_FAIL;
                break;
            }
            sJcUpdateStat = CMD698_SUC;
            break;
        }

        if (gUartConf.enable)
        {
            ret = commdrv_Send_232(fd_spi, sendBuf, sendLen);
        }
        else
        {
            ret = ac_spi_send_data(fd_spi, sendBuf, sendLen, recvBuf, 50000);
        }
        
        UPDATE_BUF_DEBUG(sendBuf, sendLen, "升级发送帧:");
        if (ret > 0)
        {
            clock_gettime(CLOCK_MONOTONIC, &StartTime);    //刷新发送时间
            StartTimeSec = (uint32)StartTime.tv_sec;
            sJcUpdateStat = CMD698_RECV;
        }
        else if (ret < 0)
        {
            sJcUpdateStat = CMD698_FAIL;
            ac_sample_dev_close(pfd);
        }

        break;

    case CMD698_RECV:   /*升级接收*/
        if (gUartConf.enable)
        {
            recvLen = commdrv_Recv_232(fd_spi, recvBuf, sizeof(recvBuf), 150, 5, 0xFF);
        }
        else
        {
            recvLen = ac_spi_recv_data_timeout(fd_spi, sendBuf, recvBuf, sizeof(recvBuf), 25000, 120);
        }
        
        if (recvLen > 0)
        {
            UPDATE_BUF_DEBUG(recvBuf, recvLen, "升级接受帧:");
            ret = ac_act_parser_trans_run(recvBuf, recvLen);
            if (ret != DATA_SUCCESS)
            {
                RecvResult = -1;
                CommRetryCnt++;                  //重试次数+1
                sJcUpdateStat = CMD698_RETRY;      //切换到重试状态
                break;
            }
            else
            {
                sJcUpdateStat = CMD698_SEND; //切换到发送状态
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
            sJcUpdateStat = CMD698_RETRY;      //切换到重试状态
        }

        break;

    case CMD698_RETRY:  /*发送尝试*/
        if (CommRetryCnt > 5)
        {
            CommRetryCnt = 0;
            sJcUpdateStat = CMD698_FAIL;//升级失败
            break;
        }

        //切换到发送状态
        if (sJcUpdateStep == JC_UPDATE_START)
        {
            sJcUpdateStat = CMD698_START_SEND;
        }
        else if (sJcUpdateStep == JC_UPDATE_RUN)
        {
            sJcUpdateStat = CMD698_SEND;
        }
        else if (sJcUpdateStep == JC_UPDATE_END)
        {
            sJcUpdateStat = CMD698_VER_SEND;
            sleep(1);
        }
        break;

    case CMD698_DELAY:  /*升级后延时*/
        sJcUpdateStep = JC_UPDATE_END;
        if (IsCloseInnerDelayState((gTimeFlag.BitSect.Second == TRUE), &WaitTime, 15) > 0)
            sJcUpdateStat = CMD698_VER_SEND;
        break;

    case CMD698_FAIL:
        ac_update_file_close(&Soft_fd);
        g_iUpdate698Cfg.UpdateFlag = 2;
        g_iUpdate698Cfg.CurJcIndex++;
        sJcUpdateStat = CMD698_IDLE;
        sJcUpdateStep = JC_UPDATE_IDLE;
        break;

    case CMD698_SUC:  /*升级成功*/
        ac_update_file_close(&Soft_fd);
        g_iUpdate698Cfg.UpdateFlag = 1;
        g_iUpdate698Cfg.CurJcIndex++;
        g_iUpdate698Cfg.iUpdateSuccCnt++;
        UPDATE_FMT_DEBUG("g_iUpdate698Cfg.iUpdateSuccCnt++\n");
        sJcUpdateStat = CMD698_IDLE;
        sJcUpdateStep = JC_UPDATE_IDLE;
        break;

    case CMD698_START_SEND:
        sendLen = ac_act_frame_trans_start(Soft_fd, sendBuf, SEND_BUF_LEN, logic);
        if (sendLen < 0)
        {
            UPDATE_FMT_DEBUG("文件错误，升级失败\n");
            sJcUpdateStat = CMD698_FAIL;
            break;
        }

        ClearCommRecvBuff(fd_spi);
        if (gUartConf.enable)
        {
            ret = commdrv_Send_232(fd_spi, sendBuf, sendLen);
        }
        else
        {
            ret = ac_spi_send_data(fd_spi, sendBuf, sendLen, recvBuf, 50000);
        }
        
        UPDATE_BUF_DEBUG(sendBuf, sendLen, "发送启动帧:");
        UPDATE_FMT_DEBUG("fd_spi = %d commdrv_Send_232 ret = %d\n", fd_spi, ret);
        if (ret > 0)
        {
            clock_gettime(CLOCK_MONOTONIC, &StartTime);    //刷新发送时间
            StartTimeSec = (uint32)StartTime.tv_sec;
            sJcUpdateStat = CMD698_START_RECV;
        }
        else if (ret < 0)
        {
            sJcUpdateStat = CMD698_FAIL;
            ac_sample_dev_close(pfd);
        }
        break;

    case CMD698_START_RECV:
        if (gUartConf.enable)
        {
            recvLen = commdrv_Recv_232(fd_spi, recvBuf, sizeof(recvBuf), 150, 5, 0xFF);
        }
        else
        {
            recvLen = ac_spi_recv_data_timeout(fd_spi, sendBuf, recvBuf, sizeof(recvBuf), 25000, 120);
        }
        
        if (recvLen > 0)
        {
            UPDATE_BUF_DEBUG(recvBuf, recvLen, "接受启动帧:");
            ret = ac_act_parser_trans_start(recvBuf, recvLen);
            if (ret != DATA_SUCCESS)
            {
                RecvResult = -1;
                CommRetryCnt++;                  //重试次数+1
                sJcUpdateStat = CMD698_RETRY;      //切换到重试状态
                break;
            }
            else
            {
                sJcUpdateStat = CMD698_SEND; //切换到发送状态
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
            sJcUpdateStat = CMD698_RETRY;      //切换到重试状态
        }
        break;

    case CMD698_VER_SEND:
        sendLen = ac_get_frame_ver(sendBuf, SEND_BUF_LEN, logic);
        if (sendLen < 0)
        {
            UPDATE_FMT_DEBUG("异常，升级失败\n");
            sJcUpdateStat = CMD698_FAIL;
            break;
        }

        ClearCommRecvBuff(fd_spi);
        if (gUartConf.enable)
        {
            ret = commdrv_Send_232(fd_spi, sendBuf, sendLen);
        }
        else
        {
            ret = ac_spi_send_data(fd_spi, sendBuf, sendLen, recvBuf, 50000);
        }
        
        UPDATE_BUF_DEBUG(sendBuf, sendLen, "发送版本帧:");
        if (ret > 0)
        {
            clock_gettime(CLOCK_MONOTONIC, &StartTime);    //刷新发送时间
            StartTimeSec = (uint32)StartTime.tv_sec;
            sJcUpdateStat = CMD698_VER_RECV;
        }
        else if (ret < 0)
        {
            sJcUpdateStat = CMD698_FAIL;
            ac_sample_dev_close(pfd);
        }
        break;

    case CMD698_VER_RECV:
        if (gUartConf.enable)
        {
            recvLen = commdrv_Recv_232(fd_spi, recvBuf, sizeof(recvBuf), 150, 5, 0xFF);
        }
        else
        {
            recvLen = ac_spi_recv_data_timeout(fd_spi, sendBuf, recvBuf, sizeof(recvBuf), 25000, 120);
        }
        
        if (recvLen > 0)
        {
            UPDATE_BUF_DEBUG(recvBuf, recvLen, "接受版本帧:");
            ret = ac_get_parser_ver(recvBuf, recvLen);
            if (ret != DATA_SUCCESS)
            {
                RecvResult = -1;
                CommRetryCnt++;                    //重试次数+1
                sJcUpdateStat = CMD698_RETRY;      //切换到重试状态
                break;
            }
            else
            {
                ac_ver_print();

                memcpy(gVerBufLast, gVerBuf, 4);
                memcpy(gDateBufLast, gDateBuf, 6);
                memcpy(gVerSBufLast, gVerSBuf, 8);

                if (sJcUpdateStep == JC_UPDATE_START)
                {
                    sJcUpdateStat = CMD698_START_SEND; //切换到发送状态
                }
                else if (sJcUpdateStep == JC_UPDATE_END)
                {
                    sJcUpdateStat = CMD698_END; //切换到发送状态
                }
                else
                {
                    RecvResult = -1;
                }
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
            sJcUpdateStat = CMD698_RETRY;      //切换到重试状态
        }
        break;

    case CMD698_END:    /*升级结束*/
        ac_update_end_proc(fd_spi, &Soft_fd, &sJcUpdateStat, &sJcUpdateStep);
        sleep(3);

        //重新获取是否支持4K报文
        ac_get_connect(fd_spi, &gIsSupport4K);

        //重新获取是否支持698升级方式
        #if UPDATE_SELF_ADAPTION == 1
        gIsUpdate698 = ac_is_support_update698(fd_spi);
        if ((gIsUpdate698 == FALSE) && (gSampPrtl == 1))
        {
            //升级底板后不支持698升级了，但之前是支持的，说明底板可能升级到了07规约，则重启终端
            UPDATE_FMT_DEBUG("升级底板后不支持698升级了，但之前是支持的，说明底板可能升级到了07规约，则重启终端\n");
            system("sync");
            system("reboot");
            exit(1);
        }
        #endif
        return 1;
        break;

    default:
        break;
    }

    return -1;
}

#endif