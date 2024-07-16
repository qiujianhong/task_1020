#include "OopAll.h"
#include "hal.h"
#include "esam.h"
#include "framecomm.h"
#include "oopVarUpdate.h"
#include "oadInfoList.h"
#include "security_msg.h"
#include "task_msg.h"
#include "gen_auth_data.h"
#include "commClass.h"

//创建与交采APP交互的句柄和锁
int             g_fUdpAcMeter = 0;
pthread_mutex_t g_fUdpAcMeterLock;
uint8           g_AcPiid = 0;     //上报报文帧序列号

//unix udp 绑定的文件名称(全路径)，目标地址
#define ACMETER_MSG_PATH       "/tmp/dev"
#define ACMETER_APP_DEST       "/tmp/dev/acmeter"

//其他宏
#define ACMETER_FILEPATH_LEN   256
#define ACMETER_BUFF_LEN       4096        //交互缓冲区长度
//在最大值约束下增加值
#define AC_ADD_VALUE_CLEAR(count, mcount, value) ((count) + (value) >= (mcount) ? 0 : (count) + (value))
#define OUT_DATA_MAX           2048
/*
*********************************************************************
* @name      : puAmr_acMeter_fd_init
* @brief     ：创建与交采APP交互的句柄
* @param[in] ：
* @param[out]：
* @return    ：错误码
*********************************************************************
*/
int puAmr_acMeter_fd_init(void)
{
    if (access(ACMETER_APP_DEST, F_OK) != 0)
    {
        MSGAC_FMT_DEBUG("%s is nonexistent\n", ACMETER_APP_DEST);
        return ERR_NODEV;
    }

    if (g_fUdpAcMeter <= 0)
    {
        char filename[ACMETER_FILEPATH_LEN] = {0};
        sprintf(filename, "%s/%s_acMeter", ACMETER_MSG_PATH, ADV_APP_NAME);
        g_fUdpAcMeter = task_pfmsg_init(filename);
        pthread_mutex_init(&g_fUdpAcMeterLock, NULL);
    }

    if (g_fUdpAcMeter <= 0)
    {
        MSGAC_FMT_DEBUG("%d is invalid\n", g_fUdpAcMeter);
        return ERR_NO_HANDLE;
    }

    return ERR_OK;
}


/*
*********************************************************************
* @name      : puAmr_acMeter_revsnd_data
* @brief     ：与交采APP交互的同步接口
* @param[in] ：
* @param[out]：
* @return    ：错误码
*********************************************************************
*/
int puAmr_acMeter_revsnd_unit(uint8 *sndData, uint16 sndLen, uint8 *pOutBuf, uint16 *rcvLen)
{
    int32 i = 0;
    int32 iRet = 0;
    uint8 czBuf[ACMETER_BUFF_LEN] = {0};
    uint8 sndBuf[ACMETER_BUFF_LEN] = {0};
    char  sender[ACMETER_FILEPATH_LEN] = {0};

    if (sndLen > sizeof(sndBuf))
    {
        return ERR_OPTION;
    }

    if (g_fUdpAcMeter <= 0)
    {
        if (0 != puAmr_acMeter_fd_init())
        {
            return ERR_NO_HANDLE;
        } 
    }

    /* 尝试加锁 */
    while (0 != pthread_mutex_trylock(&g_fUdpAcMeterLock))
    {
        if(i++ > 500)
        {
            MSGAC_FMT_DEBUG("puAmr_acMeter_revsnd_data lock faild g_fUdpAcMeterLock[]\n", g_fUdpAcMeterLock);
            return ERR_NORESOURCE;
        }
        usleep(10000);
    }

    i = 0;
    /* 1. 1ms内收到消息, 等待0.1s的时间清空队列 */
    task_msg_settime(g_fUdpAcMeter, 0, 1000);
    iRet = task_pfmsg_recv(&g_fUdpAcMeter, czBuf, ACMETER_BUFF_LEN, sender, ACMETER_FILEPATH_LEN);
    if (iRet > 0)
    {
        while(i < 100)
        {
            i++;
            task_pfmsg_recv(&g_fUdpAcMeter, czBuf, ACMETER_BUFF_LEN, sender, ACMETER_FILEPATH_LEN);
        }
    }

    /* 2. 发送消息 */
    memcpy(sndBuf, sndData, sndLen);
    MSGAC_BUF_TRACE(sndBuf, sndLen, "MSGAC SEND:");
    iRet = task_pfmsg_send(g_fUdpAcMeter, sndBuf, sndLen, ACMETER_APP_DEST);
    if(0 != iRet)
    {
        pthread_mutex_unlock(&g_fUdpAcMeterLock);
        MSGAC_FMT_DEBUG("MSGAC send faild, ret %d\n", iRet);
        return ERR_NORESOURCE;
    }

    /* 3. 接收 5s超时 */
    task_msg_settime(g_fUdpAcMeter, 5, 0);
    iRet = task_pfmsg_recv(&g_fUdpAcMeter, czBuf, ACMETER_BUFF_LEN, sender, ACMETER_FILEPATH_LEN);
    pthread_mutex_unlock(&g_fUdpAcMeterLock);
    if (iRet <= 0)
    {
        MSGAC_FMT_DEBUG("MSGAC recv faild, ret %d\n", iRet);
        return ERR_NORESOURCE;
    }

    MSGAC_BUF_TRACE(czBuf, iRet, "MSGAC RECV:");

    *rcvLen = iRet;
    memcpy(pOutBuf, czBuf, *rcvLen);

    return ERR_OK;
}

/*
*********************************************************************
* @name      : puAmr_acMeter_revsnd
* @brief     ：与交采APP交互的同步接口
* @param[in] ：
* @param[out]：
* @return    ：错误码
*********************************************************************
*/
int puAmr_acMeter_revsnd(uint8 *sndData, uint16 sndLen, uint8 *pOutBuf, uint16 *rcvLen)
{
    uint32 i = 0;
    int8   ret = ERR_NORMAL;

    while (i < 2)
    {
        ret = puAmr_acMeter_revsnd_unit(sndData, sndLen, pOutBuf, rcvLen);
        if (ERR_OK == ret)
        {
            break;
        }
        else
        {
            i++;
            usleep(10000);
        }
    }

    return ret;
}

/*
*********************************************************************
* @name      : puAmr_acMeter_set_time_zone
* @brief     ：设置交采时区
* @param[in] ：
* @param[out]：
* @return    ：错误码
*********************************************************************
*/
OOP_DAR_E puAmr_acMeter_set_time_zone(OOP_OAD_U oad, OOP_TIMEZONE_T data)
{
    int            ret = ERR_OK;
    uint8          sndbuf[OUT_DATA_MAX] = {0};
    uint8          rcvbuf[OUT_DATA_MAX] = {0};
    uint16         rcvLen = 0;
    uint32         offset = 0;
    uint32         sOffset = 0;
    uint32         dOffset = 0;
    OAD_INFO_T     oadInfo;
    DATA_CONVERT_T convert;
    OOP_DAR_E      dar = DATA_SUCCESS;

    if (oad.value != 0x40140200 || oad.value != 0x4015020)
    {
        return DATA_OBJECT_UNDEFINED;
    }

    MEMZERO(&convert, sizeof(convert)); 
    MEMZERO(&oadInfo, sizeof(oadInfo));

    g_AcPiid = AC_ADD_VALUE_CLEAR(g_AcPiid, 63, 1);
    sndbuf[offset++] = 0x06;
    sndbuf[offset++] = 0x01;
    sndbuf[offset++] = g_AcPiid;

    memcpy_r(&sndbuf[offset], &oad, sizeof(OOP_OAD_U));
    offset += sizeof(OOP_OAD_U);

    if (!class_oadinfo_get(oad, &oadInfo))
    {
        return DATA_OBJECT_UNAVAILABLE;
    }

    /* 数据和报文转换 */
    convert.srcBuf = &data;
    convert.srcLen = sizeof(data);
    convert.sOffset = &sOffset;

    convert.dstBuf = &sndbuf[offset];
    convert.dstLen = OUT_DATA_MAX - offset;
    convert.dOffset = &dOffset;

    dar = data_to_buf_class8(&oadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        MSGAC_FMT_DEBUG("data_to_buf_class8 failed. dar(%d), oad(0x%08x)\n", dar, oadInfo.pTab->oad.value);
        return dar;
    }
    offset += dOffset;
    sndbuf[offset++] = 0x00;

    ret = puAmr_acMeter_revsnd(sndbuf, offset, rcvbuf, &rcvLen);
    if (ret != ERR_OK)
    {
        MSGAC_FMT_DEBUG("puAmr_acMeter_revsnd failed. ret(%d), oad(0x%08x)\n", ret, oadInfo.pTab->oad.value);
        return DATA_READ_WRITE_DENIED;
    }

    if (rcvbuf[7] != DATA_SUCCESS)
    {
        return rcvbuf[7];
    }

    return dar;
}

/*
*********************************************************************
* @name      : puAmr_acMeter_set_day_period
* @brief     ：设置交采当前套日时段表
* @param[in] ：
* @param[out]：
* @return    ：错误码
*********************************************************************
*/
OOP_DAR_E puAmr_acMeter_set_day_period(OOP_OAD_U oad, OOP_DAYPERIOD_T data)
{
    int            ret = ERR_OK;
    uint8          sndbuf[OUT_DATA_MAX] = {0};
    uint8          rcvbuf[OUT_DATA_MAX] = {0};
    uint16         rcvLen = 0;
    uint32         offset = 0;
    uint32         sOffset = 0;
    uint32         dOffset = 0;
    OAD_INFO_T     oadInfo;
    DATA_CONVERT_T convert;
    OOP_DAR_E      dar = DATA_SUCCESS;

    if (oad.value != 0x40160200 || oad.value != 0x4017020)
    {
        return DATA_OBJECT_UNDEFINED;
    }

    MEMZERO(&convert, sizeof(convert)); 
    MEMZERO(&oadInfo, sizeof(oadInfo));

    g_AcPiid = AC_ADD_VALUE_CLEAR(g_AcPiid, 63, 1);
    sndbuf[offset++] = 0x06;
    sndbuf[offset++] = 0x01;
    sndbuf[offset++] = g_AcPiid;

    memcpy_r(&sndbuf[offset], &oad, sizeof(OOP_OAD_U));
    offset += sizeof(OOP_OAD_U);

    if (!class_oadinfo_get(oad, &oadInfo))
    {
        return DATA_OBJECT_UNAVAILABLE;
    }

    /* 数据和报文转换 */
    convert.srcBuf = &data;
    convert.srcLen = sizeof(data);
    convert.sOffset = &sOffset;

    convert.dstBuf = &sndbuf[offset];
    convert.dstLen = OUT_DATA_MAX - offset;
    convert.dOffset = &dOffset;

    dar = data_to_buf_class8(&oadInfo, &convert);
    if (dar != DATA_SUCCESS)
    {
        MSGAC_FMT_DEBUG("data_to_buf_class8 failed. dar(%d), oad(0x%08x)\n", dar, oadInfo.pTab->oad.value);
        return dar;
    }
    offset += dOffset;
    sndbuf[offset++] = 0x00;

    ret = puAmr_acMeter_revsnd(sndbuf, offset, rcvbuf, &rcvLen);
    if (ret != ERR_OK)
    {
        MSGAC_FMT_DEBUG("puAmr_acMeter_revsnd failed. ret(%d), oad(0x%08x)\n", ret, oadInfo.pTab->oad.value);
        return DATA_READ_WRITE_DENIED;
    }

    if (rcvbuf[7] != DATA_SUCCESS)
    {
        return rcvbuf[7];
    }

    return dar;
}


/*
*********************************************************************
* @name      : puAmr_acMeter_set_demand_period
* @brief     ：设置交采需量周期
* @param[in] ：
* @param[out]：
* @return    ：错误码
*********************************************************************
*/
OOP_DAR_E puAmr_acMeter_set_demand_period(OOP_OAD_U oad, uint8 data)
{
    int            ret = ERR_OK;
    uint8          sndbuf[OUT_DATA_MAX] = {0};
    uint8          rcvbuf[OUT_DATA_MAX] = {0};
    uint16         rcvLen = 0;
    uint32         offset = 0;
    uint32         sOffset = 0;
    uint32         dOffset = 0;
    OAD_INFO_T     oadInfo;
    DATA_CONVERT_T convert;
    OOP_DAR_E      dar = DATA_SUCCESS;

    if (oad.value != 0x41000200)
    {
        return DATA_OBJECT_UNDEFINED;
    }

    MEMZERO(&convert, sizeof(convert)); 
    MEMZERO(&oadInfo, sizeof(oadInfo));

    g_AcPiid = AC_ADD_VALUE_CLEAR(g_AcPiid, 63, 1);
    sndbuf[offset++] = 0x06;
    sndbuf[offset++] = 0x01;
    sndbuf[offset++] = g_AcPiid;

    memcpy_r(&sndbuf[offset], &oad, sizeof(OOP_OAD_U));
    offset += sizeof(OOP_OAD_U);

    if (!class_oadinfo_get(oad, &oadInfo))
    {
        return DATA_OBJECT_UNAVAILABLE;
    }

    /* 数据和报文转换 */
    convert.srcBuf = &data;
    convert.srcLen = sizeof(data);
    convert.sOffset = &sOffset;

    convert.dstBuf = &sndbuf[offset];
    convert.dstLen = OUT_DATA_MAX - offset;
    convert.dOffset = &dOffset;

    dar = data_to_buf_class8(&oadInfo, &convert);
    if (dar != DATA_SUCCESS)
    {
        MSGAC_FMT_DEBUG("data_to_buf_class8 failed. dar(%d), oad(0x%08x)\n", dar, oadInfo.pTab->oad.value);
        return dar;
    }
    offset += dOffset;
    sndbuf[offset++] = 0x00;

    ret = puAmr_acMeter_revsnd(sndbuf, offset, rcvbuf, &rcvLen);
    if (ret != ERR_OK)
    {
        MSGAC_FMT_DEBUG("puAmr_acMeter_revsnd failed. ret(%d), oad(0x%08x)\n", ret, oadInfo.pTab->oad.value);
        return DATA_READ_WRITE_DENIED;
    }

    if (rcvbuf[7] != DATA_SUCCESS)
    {
        return rcvbuf[7];
    }

    return dar;
}


/*
*********************************************************************
* @name      : puAmr_acMeter_set_demand_period
* @brief     ：设置交采滑差时间
* @param[in] ：
* @param[out]：
* @return    ：错误码
*********************************************************************
*/
OOP_DAR_E puAmr_acMeter_set_slip_time(OOP_OAD_U oad, uint8 data)
{
    int            ret = ERR_OK;
    uint8          sndbuf[OUT_DATA_MAX] = {0};
    uint8          rcvbuf[OUT_DATA_MAX] = {0};
    uint16         rcvLen = 0;
    uint32         offset = 0;
    uint32         sOffset = 0;
    uint32         dOffset = 0;
    OAD_INFO_T     oadInfo;
    DATA_CONVERT_T convert;
    OOP_DAR_E      dar = DATA_SUCCESS;

    if (oad.value != 0x41010200)
    {
        return DATA_OBJECT_UNDEFINED;
    }

    MEMZERO(&convert, sizeof(convert)); 
    MEMZERO(&oadInfo, sizeof(oadInfo));

    g_AcPiid = AC_ADD_VALUE_CLEAR(g_AcPiid, 63, 1);
    sndbuf[offset++] = 0x06;
    sndbuf[offset++] = 0x01;
    sndbuf[offset++] = g_AcPiid;

    memcpy_r(&sndbuf[offset], &oad, sizeof(OOP_OAD_U));
    offset += sizeof(OOP_OAD_U);

    if (!class_oadinfo_get(oad, &oadInfo))
    {
        return DATA_OBJECT_UNAVAILABLE;
    }

    /* 数据和报文转换 */
    convert.srcBuf = &data;
    convert.srcLen = sizeof(data);
    convert.sOffset = &sOffset;

    convert.dstBuf = &sndbuf[offset];
    convert.dstLen = OUT_DATA_MAX - offset;
    convert.dOffset = &dOffset;

    dar = data_to_buf_class8(&oadInfo, &convert);
    if (dar != DATA_SUCCESS)
    {
        MSGAC_FMT_DEBUG("data_to_buf_class8 failed. dar(%d), oad(0x%08x)\n", dar, oadInfo.pTab->oad.value);
        return dar;
    }
    offset += dOffset;
    sndbuf[offset++] = 0x00;

    ret = puAmr_acMeter_revsnd(sndbuf, offset, rcvbuf, &rcvLen);
    if (ret != ERR_OK)
    {
        MSGAC_FMT_DEBUG("puAmr_acMeter_revsnd failed. ret(%d), oad(0x%08x)\n", ret, oadInfo.pTab->oad.value);
        return DATA_READ_WRITE_DENIED;
    }

    if (rcvbuf[7] != DATA_SUCCESS)
    {
        return rcvbuf[7];
    }

    return dar;
}

/*
*********************************************************************
* @name      : puAmr_acMeter_set
* @brief     ：设置交采的通用接口
* @param[in] ：
* @param[out]：
* @return    ：错误码
*********************************************************************
*/
OOP_DAR_E puAmr_acMeter_set(OOP_OAD_U oad, void *data, uint32 dataLen)
{
    int            ret = ERR_OK;
    uint8          sndbuf[OUT_DATA_MAX] = {0};
    uint8          rcvbuf[OUT_DATA_MAX] = {0};
    uint16         rcvLen = 0;
    uint32         offset = 0;
    uint32         sOffset = 0;
    uint32         dOffset = 0;
    OAD_INFO_T     oadInfo;
    DATA_CONVERT_T convert;
    OOP_DAR_E      dar = DATA_SUCCESS;

    if ((data == NULL) || (dataLen == 0))
    {
        return DATA_OTHER_REASON;
    }

    MEMZERO(&convert, sizeof(convert)); 
    MEMZERO(&oadInfo, sizeof(oadInfo));

    g_AcPiid = AC_ADD_VALUE_CLEAR(g_AcPiid, 63, 1);
    sndbuf[offset++] = 0x06;
    sndbuf[offset++] = 0x01;
    sndbuf[offset++] = g_AcPiid;

    memcpy_r(&sndbuf[offset], &oad, sizeof(OOP_OAD_U));
    offset += sizeof(OOP_OAD_U);

    if (!class_oadinfo_get(oad, &oadInfo))
    {
        return DATA_OBJECT_UNAVAILABLE;
    }

    /* 数据和报文转换 */
    convert.srcBuf = data;
    convert.srcLen = dataLen;
    convert.sOffset = &sOffset;

    convert.dstBuf = &sndbuf[offset];
    convert.dstLen = OUT_DATA_MAX - offset;
    convert.dOffset = &dOffset;

    dar = data_to_buf_class8(&oadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        MSGAC_FMT_DEBUG("data_to_buf_class8 failed. dar(%d), oad(0x%08x)\n", dar, oadInfo.pTab->oad.value);
        return dar;
    }
    offset += dOffset;
    sndbuf[offset++] = 0x00;

    ret = puAmr_acMeter_revsnd(sndbuf, offset, rcvbuf, &rcvLen);
    if (ret != ERR_OK)
    {
        MSGAC_FMT_DEBUG("puAmr_acMeter_revsnd failed. ret(%d), oad(0x%08x)\n", ret, oadInfo.pTab->oad.value);
        return DATA_READ_WRITE_DENIED;
    }

    if (rcvbuf[7] != DATA_SUCCESS)
    {
        MSGAC_BUF_DEBUG(rcvbuf, rcvLen, "dar err. apdu:");
        return rcvbuf[7];
    }

    return dar;
}


/*
*********************************************************************
* @name      : puAmr_acMeter_get
* @brief     ：读交采的通用接口
* @param[in] ：
* @param[out]：
* @return    ：错误码
*********************************************************************
*/
OOP_DAR_E puAmr_acMeter_get(OOP_OAD_U oad, void *data, uint32 dataLen)
{
    int            ret = ERR_OK;
    uint8          sndbuf[OUT_DATA_MAX] = {0};
    uint8          rcvbuf[OUT_DATA_MAX] = {0};
    uint16         rcvLen = 0;
    uint32         offset = 0;
    uint32         sOffset = 8;
    uint32         dOffset = 0;
    OAD_INFO_T     oadInfo;
    DATA_CONVERT_T convert;
    OOP_DAR_E      dar = DATA_SUCCESS;

    if ((data == NULL) || (dataLen == 0))
    {
        return DATA_OTHER_REASON;
    }

    MEMZERO(&convert, sizeof(convert)); 
    MEMZERO(&oadInfo, sizeof(oadInfo));

    g_AcPiid = AC_ADD_VALUE_CLEAR(g_AcPiid, 63, 1);
    sndbuf[offset++] = 0x05;
    sndbuf[offset++] = 0x01;
    sndbuf[offset++] = g_AcPiid;

    memcpy_r(&sndbuf[offset], &oad, sizeof(OOP_OAD_U));
    offset += sizeof(OOP_OAD_U);

    sndbuf[offset++] = 0x00;

    ret = puAmr_acMeter_revsnd(sndbuf, offset, rcvbuf, &rcvLen);
    if (ret != ERR_OK)
    {
        MSGAC_FMT_DEBUG("puAmr_acMeter_revsnd failed. ret(%d), oad(0x%08x)\n", ret, oad.value);
        return DATA_READ_WRITE_DENIED;
    }

    if (!class_oadinfo_get(oad, &oadInfo))
    {
        return DATA_OBJECT_UNAVAILABLE;
    }

    /* 数据和报文转换 */
    convert.srcBuf = rcvbuf;
    convert.srcLen = rcvLen;
    convert.sOffset = &sOffset;

    convert.dstBuf = data;
    convert.dstLen = dataLen;
    convert.dOffset = &dOffset;

    dar = buf_to_data_class8_set(&oadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        MSGAC_FMT_DEBUG("data_to_buf_class8 failed. dar(%d), oad(0x%08x)\n", dar, oadInfo.pTab->oad.value);
        return dar;
    }

    return dar;
}

