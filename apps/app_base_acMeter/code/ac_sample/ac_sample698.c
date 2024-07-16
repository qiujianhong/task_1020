/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief： 功能描述
* @date： 2019-10-25
*********************************************************************
*/

#include "ac_sample698.h"
#include "ac_sample.h"
#include "ac_update.h"
#include "ac_update698.h"
#include "ac_lib.h"
#include "ac_main.h"
#include "ac_commdrv.h"
#include "ac_mqtt.h"
#include "ac_save.h"
#include "commClass.h"
#include "ac_report.h"
#include "ac_parser.h"
#include "ac_oadlist.h"
#include "ac.h"
#include "ac_698msg.h"

//698报文中的PIID
uint8 gAcSampPiid = 0;

CONNECT_RESPONSE_T gConnectRes = { 0 };

BOOL   gIsSupport4K = TRUE;

UART_CONF_T gUartConf = {0};

extern int make_698msg_ac_set(OOP_OAD_U oad, uint8 *inData, uint32 inLen, uint8 *outMsg, uint32 *outLen);

/**********************************************************************
* @name：      ac_spi_reset
* @brief：     交采复位
* @param[in] ：
* @param[out]：
* @return    ：
* @Date      ：2019-12-6
**********************************************************************/
void ac_spi_reset(void)
{
    jc_reset(2000000);
    return;
}

/**********************************************************************
* @name：      ac_spi_reset_proc
* @brief：     交采复位
* @param[in] ：
* @param[out]：
* @return    ：
* @Date      ：2019-12-6
**********************************************************************/
void ac_spi_reset_proc(uint32 *cnt)
{
    uint32 rstCnt = *cnt;

    if (rstCnt > 15)
    {
        rstCnt = 0;
        ac_spi_reset();
        SELFCHK_FMT_DEBUG("warn:the spi state error, reset ac\n");
    }

    *cnt = rstCnt;

    return;
}


/**********************************************************************
* @name：      ac_sample_dev_open
* @brief：     设备打开
* @param[in] ：
* @param[out]：
* @return    ：
* @Date      ：2019-12-6
**********************************************************************/
int ac_sample_dev_open(void)
{
    int fd_spi = -1;
    if (gUartConf.enable)
    {
        RS232_COMM_PARA commPara;

        memset((void *)&commPara, 0, sizeof(RS232_COMM_PARA));
        commPara.dwBaudRate = gUartConf.BaudRate;
        commPara.byByteSize = gUartConf.ByteSize;
        commPara.byParity = gUartConf.Parity;
        commPara.byStopBits = gUartConf.StopBits;

        sprintf((char *)commPara.sDeviceName, gUartConf.dev);

        fd_spi = commdrv_Init_232(&commPara);   /*打开串口*/
    }
    else
    {
        ACSPI_COMM_PARA_T acSpiPara; 

        memset(&acSpiPara, 0, sizeof(acSpiPara));
        acSpiPara.mode = 1;
        acSpiPara.bits = 8;
        acSpiPara.delay = 0;
        acSpiPara.speed = 200*1000;
        fd_spi = ac_spi_init(&acSpiPara);
    }

    return fd_spi;
}

/**********************************************************************
* @name：      ac_sample_dev_close
* @brief：     设备关闭
* @param[in] ：
* @param[out]：
* @return    ：
* @Date      ：2019-12-6
**********************************************************************/
int ac_sample_dev_close(int *fd_spi)
{
    if (gUartConf.enable)
    {
        int fd = *fd_spi;
        if (fd >= 0)
        {
            close(fd);
        }

        *fd_spi = -1;
    }
    else
    {
        ac_spi_close(fd_spi);
    }

    return 0;
}

/**********************************************************************
* @name：      get_head_from_buf
* @brief：     找合法的报文68开头，主要指回码报文
* @param[in] ：
* @param[out]：
* @return    ：
* @Date      ：2019-12-6
**********************************************************************/
uint8 *get_68ptr_from_buf(uint8 *buf, uint32 maxLen, uint32 *offset, uint32 *outLen)
{
    int i;
    uint8 *head;
    uint32 len = 0;
    uint32 off = 0;

    if ((buf == NULL) || (maxLen <= 9))
        return NULL;

    head = buf;
    for (i = 0; i < maxLen - 9; i++)
    {
        if (*head == 0x68)
        {
            memcpy(&len, head + 1, 2);
            //防止数据溢出
            if ((i + len + 2) <= maxLen)
            {
                if (app_check_message((uint8 *)head, len + 2) == 0)
                {
                    *offset = off;
                    *outLen = len + 2;

                    ACSAMP_FMT_TRACE("get_68ptr_from_buf. offset[%d] outlen[%d]\n", *offset, *outLen);
                    return head;
                }     
            }
        }
        off ++; 
        head ++;
    }
    return NULL;
}

/**********************************************************************
* @name：      get_oad_num
* @brief：     获取类型数据的OAD数量
* @param[in] ：type - 数据大类
* @param[out]：
* @return    ：OAD数量
* @Date      ：2019-12-6
**********************************************************************/
uint32 get_oad_num(SAMPLE_TYPE_E type)
{
    uint32 len = 0;
    uint32 i   = 0;

    for (i = 0; i < gOadInfoListNum; i++)
    {
        if (gOadInfoList[i].type == type)
        {
            break;
        }
    }
    if (i == gOadInfoListNum)
    {
        ACSAMP_FMT_DEBUG("get_oad_num err. no find the type[%d]\n", type);
        return 0;
    }

    len = *gOadInfoList[i].pOadInfoNum;

    return len;
}

/**********************************************************************
* @name：      get_oad_num
* @brief：     获取类型数据的OAD数量
* @param[in] ：type - 数据大类
* @param[out]：buf  - 填充OAD
* @return    ：所有OAD的长度
* @Date      ：2019-12-6
**********************************************************************/
uint32 get_oad_value(SAMPLE_TYPE_E type, uint8 *buf, uint32 maxLen)
{
    OAD_INFO_MAP_T *pDataMap = NULL;
    uint32 num = 0;
    uint32 i   = 0;
    uint32 j   = 0;
    uint32 offset = 0;
    uint8  bufTmp[SEND_APDU_LEN] = {0};

    if ((buf == NULL) || (maxLen > SEND_APDU_LEN))
    {
        ACSAMP_FMT_DEBUG("get_oad_value err. buf[%p]. maxLen[%d]\n", buf, maxLen);
        return 0;
    }

    for (i = 0; i < gOadInfoListNum; i++)
    {
        if (gOadInfoList[i].type == type)
        {
            pDataMap = gOadInfoList[i].pOadInfoMap;
            num = *gOadInfoList[i].pOadInfoNum;
            break;
        }
    }
    if ((i == gOadInfoListNum) || (pDataMap == NULL) || (num == 0))
    {
        ACSAMP_FMT_DEBUG("get_oad_value err. i[%d]. pDataMap[%p], type[%d]. num[%d]\n", 
            i, pDataMap, type, num);
        return 0;
    }

    for (j = 0; j < num; j ++)
    {
        //防止缓存溢出
        if ((offset + 4) > maxLen) 
        {
            ACSAMP_FMT_DEBUG("get_oad_value err. i[%d]. offset[%p], maxLen[%d]\n", i, 
                offset, maxLen);
            return 0;
        }
        memcpy_r(&bufTmp[offset], &pDataMap[j].oad.value, sizeof(pDataMap[j].oad.value));
        offset += 4;
    }

    memcpy(&buf[0], bufTmp, offset);

    return offset;
}

/**********************************************************************
* @name：      make_frame_master
* @brief：     组织读该数据类型报文
* @param[in] ：tpye - 数据大类
* @param[out]：
* @return    ：
* @Date      ：2019-12-6
**********************************************************************/
uint32 make_frame_to_apdu_one(SAMPLE_TYPE_E type, uint8 *sendbuf, uint32 maxLen)
{
    uint8 *pApdu = sendbuf;
    uint32 offset = 0;
    uint32 retLen = 0;
    uint32 oadNum = 0;
    
    if ((sendbuf == NULL) || 
        (type <= SAMPLE_NULL) ||
        (type >= SAMPLE_ERR))
    {
        ACSAMP_FMT_DEBUG("make frame err type[%d]. sendbuf[%p]\n", type, sendbuf);
        return 0;
    }

    gAcSampPiid = ACSAMP_ADD_VALUE_CLEAR(gAcSampPiid, 63, 1);
    oadNum = get_oad_num(type);
    if (oadNum == 0)
    {
        ACSAMP_FMT_DEBUG("get_oad_num err type[%d]. oadNum[%d]\n", type, oadNum);
        return 0;
    }

    pApdu[offset ++] = 0x05;

    if (oadNum == 1)
    {
        pApdu[offset ++] = 0x01;
        pApdu[offset ++] = gAcSampPiid;
    }
    else
    {
        pApdu[offset ++] = 0x02;
        pApdu[offset ++] = gAcSampPiid;
        pApdu[offset ++] = get_oad_num(type);
    }

    retLen = get_oad_value(type, &pApdu[offset], maxLen - offset);
    if (retLen > 0)
    {
        //防止缓存溢出
        if (retLen + offset > maxLen)
        {
            ACSAMP_FMT_DEBUG("make frame err retLen[%d]. offset[%d]. maxLen[%d]\n", retLen, 
                offset, maxLen);
            return 0;
        }
    }
    else
    {
        return 0;
    }

    offset += retLen;
    pApdu[offset ++] = 0x00;

    return offset;
} 

/**********************************************************************
* @name：      make_frame_master
* @brief：     组织读该数据类型报文
* @param[in] ：tpye - 数据大类
* @param[out]：
* @return    ：
* @Date      ：2019-12-6
**********************************************************************/
uint32 make_frame_to_apdu_mix(SAMPLE_TYPE_E type1, SAMPLE_TYPE_E type2, uint8 *sendbuf, uint32 maxLen)
{
    uint8 *pApdu = sendbuf;
    uint32 offset = 0;
    uint32 retLen = 0;
    uint32 oadNum = 0;
    
    if ((sendbuf == NULL) || 
        (type1 <= SAMPLE_NULL) ||
        (type1 >= SAMPLE_ERR))
    {
        ACSAMP_FMT_DEBUG("make frame err type1[%d]. sendbuf[%p]\n", type1, sendbuf);
        return 0;
    }

    if ((sendbuf == NULL) || 
        (type2 <= SAMPLE_NULL) ||
        (type2 >= SAMPLE_ERR))
    {
        ACSAMP_FMT_DEBUG("make frame err type2[%d]. sendbuf[%p]\n", type2, sendbuf);
        return 0;
    }

    gAcSampPiid = ACSAMP_ADD_VALUE_CLEAR(gAcSampPiid, 63, 1);
    oadNum = get_oad_num(type1);
    if (oadNum == 0)
    {
        ACSAMP_FMT_DEBUG("get_oad_num err type1[%d]. oadNum[%d]\n", type1, oadNum);
        return 0;
    }

    oadNum += get_oad_num(type2);
    if (oadNum == 0)
    {
        ACSAMP_FMT_DEBUG("get_oad_num err type2[%d]. oadNum[%d]\n", type2, oadNum);
        return 0;
    }

    pApdu[offset ++] = 0x05;

    if (oadNum == 1)
    {
        pApdu[offset ++] = 0x01;
        pApdu[offset ++] = gAcSampPiid;
    }
    else
    {
        pApdu[offset ++] = 0x02;
        pApdu[offset ++] = gAcSampPiid;
        pApdu[offset ++] = oadNum;
    }

    retLen = get_oad_value(type1, &pApdu[offset], maxLen - offset);
    if (retLen > 0)
    {
        //防止缓存溢出
        if (retLen + offset > maxLen)
        {
            ACSAMP_FMT_DEBUG("make frame err retLen[%d]. offset[%d]. maxLen[%d]\n", retLen, 
                offset, maxLen);
            return 0;
        }
    }
    offset += retLen;

    retLen = get_oad_value(type2, &pApdu[offset], maxLen - offset);
    if (retLen > 0)
    {
        //防止缓存溢出
        if (retLen + offset > maxLen)
        {
            ACSAMP_FMT_DEBUG("make frame err retLen[%d]. offset[%d]. maxLen[%d]\n", retLen, 
                offset, maxLen);
            return 0;
        }
    }
    offset += retLen;


    pApdu[offset ++] = 0x00;

    return offset;
} 

/**********************************************************************
* @name：      make_frame_master
* @brief：     组织读该数据类型报文
* @param[in] ：tpye - 数据大类
* @param[out]：
* @return    ：
* @Date      ：2019-12-6
**********************************************************************/
uint32 make_frame_to_apdu(SAMPLE_TYPE_E type, uint8 *sendbuf, uint32 maxLen)
{
    if (gIsSupport4K)
    {
        if (type != SAMPLE_REALTIME)
        {
            return make_frame_to_apdu_mix(type, SAMPLE_REALTIME, sendbuf, maxLen);
        }
        else
        {
            return make_frame_to_apdu_one(type, sendbuf, maxLen);
        }
    }
    else
    {
        return make_frame_to_apdu_one(type, sendbuf, maxLen);
    }
}

/**********************************************************************
* @name：      make_apdu_to_package
* @brief：     组织完整698报文
* @param[in] ：apdu    - apdu
               apduLen - apdu长度
               maxLen  - 698报文最大长度
* @param[out]：outMsg  - 698报文
* @return    ：
* @Date      ：2019-12-6
**********************************************************************/
uint32 make_apdu_to_package(uint8 *apdu, uint32 apduLen, uint8 *outMsg, uint32 maxLen, uint8 logic)
{
    uint32       SendLen = 0;
    FRAME_HEAD_T tFrame = {0};
    uint8        Addr[6] = {0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA};

    memset(&tFrame, 0, sizeof(FRAME_HEAD_T));
    tFrame.dir = 0;
    tFrame.prm = 1;
    tFrame.funCode = 3;
    tFrame.apduLen = apduLen;
    tFrame.pApdu = apdu;

    //填写通配地址
    tFrame.sAddr.len = 6;
    memcpy(tFrame.sAddr.addr, Addr, 6);
    tFrame.sAddr.type = 1;
    tFrame.sAddr.logicAddr = logic;

    tFrame.headlen = APUD_COUNT_FRAME_LEN(tFrame.sAddr.len, tFrame.sAddr.logicAddr);
    tFrame.msglen = tFrame.headlen + tFrame.apduLen + APP_698_CS_REGIN_SIZE;

    SendLen = app_frame_to_package(&tFrame, outMsg, SEND_APDU_LEN - 2, TRUE);
    if ((SendLen > maxLen) || (SendLen == 0))
    {
        ACSAMP_FMT_DEBUG("make_apdu_to_package err SendLen[%d]. maxLen[%d]\n", SendLen, maxLen);
        return 0;
    }

    return SendLen;
}

/**********************************************************************
* @name：      invoke_get_res
* @brief：     解析apdu
* @param[in] ：ptFrame - 698信息
* @param[out]：
* @return    ：错误码
* @Date      ：2019-12-6
**********************************************************************/
int invoke_get_res(uint8 *pInMsg, uint32 inLen)
{
    int ret = -1;
    OAD_INFO_MAP_T *pDataMap = NULL;
    OOP_OAD_U serviceOAD = {0};

    memcpy_r(&serviceOAD, &pInMsg[0], sizeof(OOP_OAD_U));

    if (get_info_map(serviceOAD, &pDataMap) <0)
    {
        ACSAMP_FMT_TRACE("find no pOopParserFun. serviceOAD(0x%08x)\n", serviceOAD.value);
        return -1;
    }

    if (pDataMap->pOopParserFun != NULL)
    {
        ret = pDataMap->pOopParserFun(serviceOAD, &pInMsg[4], inLen - 4, pDataMap->destData, pDataMap->destDataBak);
        if (ret < 0)
        {
            ACSAMP_BUF_DEBUG(&pInMsg[4], inLen - 4, "pOopParserFun err. serviceOAD[0x%08x]. ret[%d]", 
                serviceOAD.value, ret);
            
            return -2;
        }
    }

    return 0;
}

/**********************************************************************
* @name：      get_normal_list_res
* @brief：     解析apdu
* @param[in] ：ptFrame - 698信息
* @param[out]：
* @return    ：错误码
* @Date      ：2019-12-6
**********************************************************************/
int get_normal_list_res(FRAME_HEAD_T *ptFrame, SAMPLE_TYPE_E type)
{
    uint8  sequenceNum = 0;
    uint16 dataLen = 0;
    int16  offset = 0;
    OOP_OAD_U oad = {0};

    sequenceNum = ptFrame->pApdu[3]; //序列个数

    while (sequenceNum--)
    {
        dataLen = 5; //偏掉OAD+DATA

        memcpy_r(&oad.value, &ptFrame->pApdu[OOP_DATA_LIST_OFFSET+offset], sizeof(oad.value));

        //判断是否是DATA
        if (ptFrame->pApdu[OOP_DATA_LIST_OFFSET+offset+dataLen-1] == 0)
        {
            ACSAMP_BUF_TRACE(&ptFrame->pApdu[OOP_DATA_LIST_OFFSET + offset], dataLen + 1, "find info from apdu oad[0x%08x] err", 
                oad.value);
            //DAR + 错误码
            offset += dataLen + 1;
        }
        else
        {
            if (!apdu_next_position_get(&ptFrame->pApdu[OOP_DATA_LIST_OFFSET+offset], &dataLen, ptFrame->apduLen, DT_NULL, FALSE))
            {
                return -1;
            }
            
            ACSAMP_BUF_TRACE(&ptFrame->pApdu[OOP_DATA_LIST_OFFSET + offset], dataLen, "find info from apdu oad[0x%08x]", 
                oad.value);
            invoke_get_res(&ptFrame->pApdu[OOP_DATA_LIST_OFFSET + offset], dataLen);
            //APDU的内偏移
            offset += dataLen; 
        }
    }

    return 0;
}

/**********************************************************************
* @name：      get_normal_res
* @brief：     解析apdu
* @param[in] ：ptFrame - 698信息
* @param[out]：
* @return    ：错误码
* @Date      ：2019-12-6
**********************************************************************/
int get_normal_res(FRAME_HEAD_T *ptFrame, SAMPLE_TYPE_E type)
{
    uint16 dataLen = 0;
    int16  offset = 0;
    OOP_OAD_U oad = {0};

    dataLen = 5; //偏掉OAD+DATA

    memcpy_r(&oad.value, &ptFrame->pApdu[OOP_DATA_OFFSET + offset], sizeof(oad.value));

    //判断是否是DATA
    if (ptFrame->pApdu[OOP_DATA_OFFSET+offset+dataLen-1] == 0)
    {
        ACSAMP_BUF_TRACE(&ptFrame->pApdu[OOP_DATA_OFFSET + offset], dataLen + 1, "find info from apdu oad[0x%08x] err", 
            oad.value);
        //DAR + 错误码
        offset += dataLen + 1;
    }
    else
    {
        if (!apdu_next_position_get(&ptFrame->pApdu[OOP_DATA_OFFSET+offset], &dataLen, ptFrame->apduLen, DT_NULL, FALSE))
        {
            return -1;
        }
        memcpy_r(&oad.value, &ptFrame->pApdu[OOP_DATA_OFFSET+offset], sizeof(oad.value));
        ACSAMP_BUF_TRACE(&ptFrame->pApdu[OOP_DATA_OFFSET + offset], dataLen, "find info from apdu oad[0x%08x]", 
            oad.value);

        invoke_get_res(&ptFrame->pApdu[OOP_DATA_OFFSET + offset], dataLen);

        offset += dataLen; //APDU的内偏移
    }

    return 0;
}

/**********************************************************************
* @name：      get_response_analyse
* @brief：     解析apdu
* @param[in] ：ptFrame - 698信息
* @param[out]：
* @return    ：错误码
* @Date      ：2019-12-6
**********************************************************************/
int get_response_analyse(FRAME_HEAD_T *ptFrame, SAMPLE_TYPE_E type)
{
    uint8 reqtype = ptFrame->pApdu[1];

    switch (reqtype)
    {
    case REQ_GET_NORMAL:
        return get_normal_res(ptFrame, type);
        break;

    case REQ_GET_NORMAL_LIST:
        return get_normal_list_res(ptFrame, type);
        break;
    
    default:
        break;
    }

    return 0;
}

/*******************************************************************************
* 函数名称: ac_get_connect_info
* 函数功能: 组织获取应用连接报文
* 输入参数:
* 输出参数: sendbuf - 发送报文
* 返 回 值: 大于 0    发送长度
*          小于 0     失败
*******************************************************************************/
int ac_get_connect_msg(uint8* sendbuf, uint32 maxLen)
{
    CONNECT_REQUEST_T connectReq = { 0 };

    connectReq.version = 0x11;
    memset(connectReq.prtlConf, 0xff, sizeof(connectReq.prtlConf));
    memset(connectReq.funcConf, 0xff, sizeof(connectReq.funcConf));
    connectReq.clientSFML = SEND_BUF_LEN;
    connectReq.clientRFML = RECV_BUF_LEN;
    connectReq.clientWS = 1;
    connectReq.clientPFML = RECV_BUF_LEN;
    connectReq.timeOut = 600;

    return ac_connect_package(&connectReq, sendbuf, maxLen);
}

/**********************************************************************
* @name：      get_connect_analyse
* @brief：     解析apdu
* @param[in] ：ptFrame - 698信息
* @param[out]：
* @return    ：错误码
* @Date      ：2019-12-6
**********************************************************************/
int get_connect_analyse(FRAME_HEAD_T* ptFrame, SAMPLE_TYPE_E type)
{
    uint32 offset = 0;

    if (ptFrame->apduLen < 75)
    {
        ACSAMP_BUF_DEBUG(&ptFrame->pApdu[0], ptFrame->apduLen, "get_connect_analyse len err");
        return -1;
    }

    offset += 34;

    memcpy_r(&gConnectRes.version, &ptFrame->pApdu[offset], sizeof(gConnectRes.version));
    offset += sizeof(gConnectRes.version);

    memcpy(gConnectRes.prtlConf, &ptFrame->pApdu[offset], sizeof(gConnectRes.prtlConf));
    offset += sizeof(gConnectRes.prtlConf);

    memcpy(gConnectRes.funcConf, &ptFrame->pApdu[offset], sizeof(gConnectRes.funcConf));
    offset += sizeof(gConnectRes.funcConf);

    memcpy_r(&gConnectRes.serverSFML, &ptFrame->pApdu[offset], sizeof(gConnectRes.serverSFML));
    offset += sizeof(gConnectRes.serverSFML);

    memcpy_r(&gConnectRes.serverRFML, &ptFrame->pApdu[offset], sizeof(gConnectRes.serverRFML));
    offset += sizeof(gConnectRes.serverRFML);

    memcpy_r(&gConnectRes.serverWS, &ptFrame->pApdu[offset], sizeof(gConnectRes.serverWS));
    offset += sizeof(gConnectRes.serverWS);

    memcpy_r(&gConnectRes.serverPFML, &ptFrame->pApdu[offset], sizeof(gConnectRes.serverPFML));
    offset += sizeof(gConnectRes.serverPFML);

    memcpy_r(&gConnectRes.timeOut, &ptFrame->pApdu[offset], sizeof(gConnectRes.timeOut));
    offset += sizeof(gConnectRes.timeOut);

    gConnectRes.connectRst = ptFrame->pApdu[offset++];

    ACSAMP_FMT_DEBUG("--------------------应用连接信息--------------------\n");
    ACSAMP_FMT_DEBUG("协议版本号:%d\n", gConnectRes.version);
    ACSAMP_BUF_DEBUG(gConnectRes.prtlConf, sizeof(gConnectRes.prtlConf), "协议一致性:");
    ACSAMP_BUF_DEBUG(gConnectRes.funcConf, sizeof(gConnectRes.funcConf), "功能一致性:");
    ACSAMP_FMT_DEBUG("服务器发送帧最大尺寸:%d\n", gConnectRes.serverSFML);
    ACSAMP_FMT_DEBUG("服务器接收帧长度:%d\n", gConnectRes.serverRFML);
    ACSAMP_FMT_DEBUG("服务器接收窗口大小:%d\n", gConnectRes.serverWS);
    ACSAMP_FMT_DEBUG("服务器最大可处理帧尺寸:%d\n", gConnectRes.serverPFML);
    ACSAMP_FMT_DEBUG("应用连接超时时间:%d\n", gConnectRes.timeOut);
    ACSAMP_FMT_DEBUG("---------------------------------------------------\n");

    return 0;
}

/*******************************************************************************
* 函数名称: ac_get_connect
* 函数功能: 获取应用连接信息
* 输入参数: fd_spi 操作句柄
* 输出参数: 无
* 返 回 值: >0    成功
*           <0    失败
*******************************************************************************/
int ac_get_connect(int32 fd_spi, BOOL* pIsSupport4K)
{
    int   ret = -1;
    int32 recvLen = 0;
    int32 sendLen = 0;
    uint8 recvBuf[1024] = { 0 };
    uint8 sendBuf[1024] = { 0 };

    if (fd_spi < 0)
    {
        return ERR_NO_HANDLE;
    }

    sendLen = ac_get_connect_msg(sendBuf, 1024);
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
        ACSAMP_FMT_DEBUG("fd_spi = %d send ret = %d\n", fd_spi, ret);
        return ERR_IFACE;
    }
    else
    {
        ACSAMP_BUF_TRACE(sendBuf, sendLen, "发送应用连接帧");
    }

    if (gUartConf.enable)
    {
        recvLen = commdrv_Recv_232(fd_spi, recvBuf, sizeof(recvBuf), 300, 8, 0xFF);
    }
    else
    {
        recvLen = ac_spi_recv_data_timeout(fd_spi, sendBuf, recvBuf, sizeof(recvBuf), 50000, 65);
    }
    
    if (recvLen > 0)
    {
        ACSAMP_BUF_TRACE(recvBuf, recvLen, "接受应用连接帧");
        ret = ac_read_sample_parser(SAMPLE_OTHER, recvBuf, recvLen);
        if (ret != ERR_OK)
        {
            ACSAMP_FMT_DEBUG("应用连接解帧结果异常[%d]\n", ret);
            return ret;
        }
        else
        {
            if (gConnectRes.serverSFML >= SEND_BUF_LEN)
            {
                ACSAMP_FMT_DEBUG("支持4K以上报文\n");
                *pIsSupport4K = TRUE;
            }
            else
            {
                ACSAMP_FMT_DEBUG("不支持4K以上报文\n");
                *pIsSupport4K = FALSE;
            }
        }
    }

    return ERR_OK;
}

/**********************************************************************
* @name：      protocol_analyze_apdu
* @brief：     解析apdu
* @param[in] ：ptFrame - 698信息
* @param[out]：
* @return    ：错误码
* @Date      ：2019-12-6
**********************************************************************/
int protocol_analyze_apdu(FRAME_HEAD_T *ptFrame, SAMPLE_TYPE_E type)
{
    uint8 inServerID = 0;

    inServerID = ptFrame->pApdu[0];
    switch (inServerID)
    {
    case SERVERID_GETRESPONSE:
        return get_response_analyse(ptFrame, type);
        break;

    case SERVERID_CONNECTRESPONSE:
        return get_connect_analyse(ptFrame, type);
        break;

    default:
        break;
    }

    return 0;
}

/**********************************************************************
* @name：      ac_read_sample_package
* @brief：     组织数据
* @param[in] ：class - 数据大类
* @param[out]：
* @return    ：
* @Date      ：2019-12-6
**********************************************************************/
int ac_read_sample_package(SAMPLE_TYPE_E type, uint8 *sendbuf, uint32 bufLenMax)
{
    uint8        apdu[SEND_APDU_LEN] = {0};
    uint32       outMsgLen = 0;
    uint32       apduLen = 0;

    if ((type <= SAMPLE_NULL) ||
        (type >= SAMPLE_ERR))
    {
        return -1;
    }

    if (sendbuf == NULL)
    {
        return -2;
    }

    apduLen = make_frame_to_apdu(type, apdu, SEND_APDU_LEN);
    if (apduLen == 0)
    {
        return -3;
    }

    outMsgLen = make_apdu_to_package(apdu, apduLen, sendbuf, bufLenMax, 0);
    if (outMsgLen == 0)
    {
        return -4;
    }

    return outMsgLen;
}

/**********************************************************************
* @name：      ac_other_fun_package
* @brief：     组织数据
* @param[in] ：
* @param[out]：
* @return    ：
* @Date      ：2019-12-6
**********************************************************************/
int ac_other_fun_package(SAMPLE_TYPE_E type, uint8 *sendbuf, uint32 bufLenMax)
{
    if (type == SAMPLE_PARAM_INIT)
    {
        return ac_act_frame_factory_reset(sendbuf, bufLenMax);
    }
    else if (type == SAMPLE_POWER_OFF)
    {
        //检测到电源掉电，通知交采模块
        return ac_act_frame_power_off(sendbuf, bufLenMax);
    }

    return 0;
}

/**********************************************************************
* @name：      ac_read_698_parser
* @brief：     数据解析
* @param[in] ：class - 数据大类
* @param[out]：
* @return    ：
* @Date      ：2019-12-6
**********************************************************************/
int ac_read_698_parser(SAMPLE_TYPE_E type, uint8 *recvBuf, uint32 len)
{
    uint8        recvBufTmp[RECV_BUF_LEN] = {0};
    int          ret = 0;
    uint32       recvLen = len;
    uint32       outLen = 0;
    uint32       offset = 0;
    uint8        *ptr = NULL;
    FRAME_HEAD_T tFrame = {0};

    if ((type <= SAMPLE_NULL) ||
        (type >= SAMPLE_ERR))
    {
        return -1;
    }

    while ((ptr = get_68ptr_from_buf(recvBuf, recvLen, &offset, &outLen)) != NULL)
    {
        memcpy(recvBufTmp, ptr, outLen);

        recvLen -= (offset + outLen);
        memmove(recvBuf, (ptr + outLen), recvLen);

        /* 合法性检查 */
        ret = app_check_message(recvBufTmp, outLen);
        if (0 != ret)
        {
            ACSAMP_BUF_DEBUG(recvBufTmp, outLen, "app_check_message error[%d] msg recv", ret);
            continue;
        }

        /* 解析帧头 */
        memset(&tFrame, 0, sizeof(FRAME_HEAD_T));
        ret = app_get_frame(recvBufTmp, outLen, &tFrame, TRUE);
        if (0 != ret)
        {
            ACSAMP_BUF_DEBUG(recvBufTmp, outLen, "app_get_frame error[%d] msg recv", ret);
            continue;
        }

        ret = protocol_analyze_apdu(&tFrame, type);
        if (0 != ret)
        {
            ACSAMP_FMT_DEBUG("protocol_analyze_apdu error[%d]\n", ret);
            continue;
        }
    }

    return 0;
}

/**********************************************************************
* @name：      ac_read_sample_parser
* @brief：     数据解析
* @param[in] ：class - 数据大类
* @param[out]：
* @return    ：
* @Date      ：2019-12-6
**********************************************************************/
int ac_read_sample_parser(SAMPLE_TYPE_E type, uint8 *recvBuf, uint32 recvLen)
{
    int32  ret = ERR_OK;
    int32  bRet = 0;
    uint32 outLen = 0;
    uint32 offset = 0;
    uint8  *ptr = NULL;
    uint8  *pstart = NULL;  //应答数据帧起始指针(首地址,可能并非0x68,用于多帧解帧)
    int32  validlen = 0;  //一帧完整报文长度
    int32  leftlen = 0;  //报文剩余长度
    int    CheckRetSum = 0;
    uint8  recvBufTemp[RECV_BUF_LEN] = {0};

    ptr = get_68ptr_from_buf(recvBuf, recvLen, &offset, &outLen);
    if (ptr != NULL)
    {
        return ac_read_698_parser(type, recvBuf, recvLen);
    }
    else
    {
        //保留了以前的代码，非同步应答报文处理
        if (recvLen > 9)
        {
            if(recvLen > sizeof(recvBuf))
            {
                recvLen = sizeof(recvBuf);//长度容错
            }

            pstart = (uint8 *)recvBuf;
            leftlen = recvLen;

            while (leftlen >= 0x0d)//报文长度占1字节
            {
                memset(recvBufTemp, 0x00, sizeof(recvBufTemp));
                ptr = (uint8*)commfun_memchr_gb((char *)pstart, 0x68, leftlen);
                if(ptr != NULL)
                {
                    leftlen -= (ptr - pstart);					
                    memmove(recvBufTemp, ptr, leftlen);
                    bRet = STDAC_CheckRspFrame_Single(recvBufTemp, leftlen);
                    if(bRet == 1)
                    {
                        CheckRetSum = 0;
                        validlen = (recvBufTemp[9] + 0x0c);  //报文有效长度
                        ACSAMP_BUF_LOGCC(recvBufTemp, validlen, "L1有效分帧报文: ");
                        Pro_AC_Amr_Response_Single(recvBufTemp);
                        
                        pstart = ptr + validlen;
                        leftlen -= validlen;
                    }
                    else
                    {
                        ACSAMP_FMT_LOGCC("bRet=%d \n", bRet);
                        CheckRetSum++;
                        break;
                    }
                }
                else
                {
                    ACSAMP_FMT_LOGCC("if(ptr == NULL) \n");
                    CheckRetSum++;
                    break;
                }
            }

            if (CheckRetSum >= 1)
            {
                ret = ERR_TIMEOUT;
            }
        }
        else
        {
            ret = ERR_TIMEOUT;
        }
    }

    return ret;
}

/**********************************************************************
* @name：      get_rptptr_from_buf
* @brief：     找合法的上报报文开头，主要指回码报文
* @param[in] ：
* @param[out]：
* @return    ：
* @Date      ：2019-12-6
**********************************************************************/
uint8 *get_rptptr_from_buf(uint8 *buf, uint32 maxLen, uint32 *offset, uint32 *outLen)
{
    int i;
    uint8 *head;
    uint32 len = 0;
    uint32 off = 0;

    if ((buf == NULL) || (maxLen <= 9))
        return NULL;

    head = buf;
    for (i = 0; i < maxLen - 9; i++)
    {
        if (*head == 0x68)
        {
            memcpy(&len, head + 1, 2);
            //防止数据溢出
            if ((i + len + 2) <= maxLen)
            {
                if ((app_check_message((uint8 *)head, len + 2) == 0) && 
                     (app_check_rpt_message((uint8 *)head, len + 2) == 0))
                {
                    *offset = off;
                    *outLen = len + 2;

                    ACSAMP_FMT_TRACE("get_rptptr_from_buf. offset[%d] outlen[%d]\n", *offset, *outLen);
                    return head;
                }     
            }
        }
        off ++; 
        head ++;
    }
    return NULL;
}

/**********************************************************************
* @name：      ac_report_parser
* @brief：     上报数据解析
* @param[in]: pRecv  接收数据缓冲区
              len         接收长度
* @param[out]：
* @return    ：
* @Date      ：2019-12-6
**********************************************************************/
int ac_report_parser(uint8 *recvBuf, uint32 recvLen, FRAME_HEAD_T *pFrame)
{
    int i = 0;
    int ret = 0;
    OOP_OAD_U oad = {0};
    
    int num = gRptDataMapNum;
    OAD_INFO_MAP_T *pDataMap = (OAD_INFO_MAP_T *)gRptDataMap;
    
    if ((recvBuf == NULL) || (pFrame == NULL))
    {
        return ERR_PTR;
    }

    ret = app_get_frame(recvBuf, recvLen, pFrame, FALSE);
    if (ret != 0)
    {
        return ret;
    }

    ret = ERR_NORMAL;
    memcpy_r(&oad.value, &pFrame->pApdu[4], sizeof(oad.value));
    for (i = 0; i < num; i++)
    {
        if (pDataMap[i].oad.value == oad.value)
        {
            ret = pDataMap[i].pOopParserFun(oad, &pFrame->pApdu[0], pFrame->apduLen, pDataMap->destData, pDataMap->destDataBak);
        }
    }
    
    return ret;
}

/*********************************************************************
函数名称：ac_report_parser
功能描述：处理上报报文
输入参数：fd_232      串口文件句柄
          pRecv  接收数据缓冲区
          len         接收长度
输出参数: none
返回值：      长度或者错误码
处理流程：查找报文中是否含有上报报文
********************************************************************/
int32 ac_report_proc(int32 fd, uint8* pSend, uint8* pRecv, int32 len, int32 maxLen)
{
    int ret = 0;
    
    uint8 *ptr = NULL;
    uint32 rptOff = 0;
    uint32 rptLen = 0;

    FRAME_HEAD_T tFrame = {0};
    OOP_OAD_U    oad = {0};
    uint8        isRec = 0;
    uint8        piid = 0;
    uint8        addr[6] = {0};

    uint8        rptbuf[2048] = {0};
    uint8        tmp[RECV_BUF_LEN] = {0};

    uint8        send[1024] = {0};
    uint8        recv[1024] = {0};
    uint8        sendLen = 0;

    if ((pRecv == NULL) || (pSend == NULL))
    {
        return -1;
    }
    
    if ((len <= 0) || (len > maxLen))
    {
        return len;
    }

    memcpy(tmp, pRecv, len);
    
    while ((ptr = get_rptptr_from_buf(pRecv, len, &rptOff, &rptLen)) != NULL)
    {
        ACSAMP_BUF_TRACE(pRecv, len, "fd[%d]上报前的报文");
        memcpy(rptbuf, ptr, rptLen);
        ACSAMP_BUF_DEBUG(rptbuf, rptLen, "fd[%d]上报接收");
        ret = ac_report_parser(rptbuf, rptLen, &tFrame);
        if (0 != ret)
        {
            ACSAMP_FMT_DEBUG("ac_report_parser error[%d]\n", ret);
        }
        else
        {
            //组织上报确认报文
            memcpy_r(&oad.value, &tFrame.pApdu[4], sizeof(oad.value));
            isRec = tFrame.pApdu[1];
            piid = tFrame.pApdu[2];
            memcpy(addr, tFrame.sAddr.addr, 6);
            
            sendLen = ac_rpt_msg_package(addr, isRec, piid, oad, send, 1024);
            if (sendLen > 0)
            {
                ACSAMP_BUF_DEBUG(send, sendLen, "fd[%d]上报确认", fd);
                if (gUartConf.enable)
                {
                    ret = commdrv_Send_232(fd, send, sendLen);
                    if (ret < 0)
                    {
                        ACSAMP_BUF_DEBUG(send, sendLen, "上报确认失败nRet[%d]\n", ret);
                    }
                }
                else
                {
                    ac_spi_send_data(fd, send, sendLen, recv, 500000);
                }
                
                sendLen = 0;
            }
            else
            {
                ACSAMP_FMT_DEBUG("组织上报回码错误 错误码[%d]\n", sendLen);
            }
        }

        //处理过上报报文后，把上报报文剔除
        memcpy(pRecv, tmp, rptOff);

        if (len >= rptOff + rptLen)
        {
            memcpy(&pRecv[rptOff], &tmp[rptOff + rptLen], (len - rptOff - rptLen));
            len = len - rptLen;
        }
        else
        {
            len = rptOff;
        }

        ACSAMP_BUF_TRACE(pRecv, len, "fd[%d]上报后的报文");
        //处理后没有报文，说明仅仅收到上报报文，再尝试收其他报文
        if (len == 0)
        {
            if (gUartConf.enable)
            {
                len = commdrv_Recv_232(fd, pRecv, maxLen, 150, 8, 0xFF);
            }
            else
            {
                len = ac_spi_recv_data_timeout(fd, pSend, pRecv, maxLen, 50000, 480);
            }
        }
    }

    return len;
}

/*******************************************************************************
* 函数名称: ac_get_698ver
* 函数功能: 获取版本号
* 输入参数: fd_spi 操作句柄
* 输出参数: 无
* 返 回 值: >0    成功
*           <0    失败
*******************************************************************************/
int ac_pre_proc(int32 *pfd, uint32 *acPreCnt)
{
    int   ret = -1;
    int32 recvLen = 0;
    int32 sendLen = 0;
    uint8 recvBuf[1024] = { 0 };
    uint8 sendBuf[1024] = { 0 };

    uint8  apdu[SEND_APDU_LEN] = {0};
    uint32 apduLen = 0;

    uint32         num = gPreprocListNum;
    uint32         cnt = *acPreCnt;
    AC_PRE_PROC_T *pPre = NULL;
    
    int32 fd_spi = *pfd;

    if (cnt >= num)
    {
        return ERR_PTR;
    }
    else
    {
        pPre = (AC_PRE_PROC_T *)&gPreprocList[cnt];
    }

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
            ACSAMP_FMT_DEBUG("msg: the spi open ok.\n");
        }
    }

    apduLen = pPre->sendLen;
    memcpy(apdu, pPre->sendBuf, apduLen);
    
    sendLen = make_apdu_to_package(apdu, apduLen, sendBuf, 1024, 0);
    if (sendLen == 0)
    {
        return ERR_OPTION;
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
        ACSAMP_FMT_DEBUG("fd_spi = %d send ret = %d\n", fd_spi, ret);
        ac_sample_dev_close(pfd);
        return ERR_IFACE;
    }
    else
    {
        ACSAMP_BUF_DEBUG(sendBuf, sendLen, "发送预处理帧");
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
        ACSAMP_BUF_DEBUG(recvBuf, recvLen, "接受预处理帧");
        if (pPre->apduType == SERVERID_SETREQUEST)
        {
            ret = pPre->pOopParserFun(pPre->oad, recvBuf, recvLen, pPre->destData, pPre->destDataBak);
            if (ret != ERR_OK)
            {
                ACSAMP_FMT_DEBUG("预处理解帧结果异常[%d]\n", ret);
                return ret;
            }
            else
            {
                cnt++;
                *acPreCnt = cnt;
            }
        }
        else if (pPre->apduType == SERVERID_GETREQUEST)
        {
            ret = ac_Get_normal_parser(pPre->oad, recvBuf, recvLen, pPre->destData, pPre->destDataBak);
            if (ret != ERR_OK)
            {
                ACSAMP_FMT_DEBUG("预处理解帧结果异常[%d]\n", ret);
                return ret;
            }
            else
            {
                cnt++;
                *acPreCnt = cnt;
            }
        }

        if (pPre->delay > 0)
        {
            sleep(pPre->delay);
        }
    }
    else
    {
        ACSAMP_FMT_DEBUG("预处理接受超时 cnt[%d]\n", cnt);
    }

    return ERR_OK;
}
/**********************************************************************
* @name：      ac_calc_deal_698
* @brief：     读交采模块数据
* @param[in] ：class - 数据大类
* @param[out]：
* @return    ：
* @Date      ：2019-12-6
**********************************************************************/
int ac_read_sample(int fd, SAMPLE_TYPE_E type)
{
    uint8        recvBufTmp[RECV_BUF_LEN] = {0};
    uint8        recvBuf[RECV_BUF_LEN] = {0};
    uint8        outMsg[SEND_BUF_LEN] = {0};
    uint8        apdu[SEND_APDU_LEN] = {0};
    uint32       outMsgLen = 0;
    uint32       apduLen = 0;
    uint32       recvLen = 0;
    uint32       delay = 0;
    int          ret = 0;
    uint32       outLen = 0;
    uint32       offset = 0;
    uint8        *ptr = NULL;
    FRAME_HEAD_T tFrame = {0};

    if ((type <= SAMPLE_NULL) ||
        (type >= SAMPLE_ERR))
    {
        return -1;
    }

    apduLen = make_frame_to_apdu(type, apdu, SEND_APDU_LEN);
    if (apduLen == 0)
    {
        return -2;
    }

    outMsgLen = make_apdu_to_package(apdu, apduLen, outMsg, SEND_BUF_LEN, 0);
    if (outMsgLen == 0)
    {
        return -3;
    }

    delay = 0;
    do
    {
        delay ++;
        if (gUartConf.enable)
        {
            ret = commdrv_Send_232(fd, outMsg, outMsgLen);
        }
        else
        {
            ret = ac_spi_send_data(fd, outMsg, outMsgLen, recvBuf, 500000);
        }
        
        if (ret <= 0)
        {
            ACSAMP_BUF_DEBUG(outMsg, outMsgLen, "send err ret[%d]", ret);
            return -4;
        }
        else
        {
            ACSAMP_BUF_TRACE(outMsg, outMsgLen, "send");
        }

        if (gUartConf.enable)
        {
            recvLen = commdrv_Recv_232(fd, recvBuf, sizeof(recvBuf), 150, 10, 0xFF);
        }
        else
        {
            recvLen = ac_spi_recv_data_timeout(fd, outMsg, recvBuf, sizeof(recvBuf), 50000, 65);
        }
        
        if (recvLen > 0)
        {
            break;
        }
    }while(delay < COMWAITTIME);
    if (delay >= COMWAITTIME)
    {
        ACSAMP_FMT_DEBUG("recv err recvLen[%d]\n", recvLen);
        return -5;
    }
    ACSAMP_BUF_TRACE(recvBuf, recvLen, "recv");

    while ((ptr = get_68ptr_from_buf(recvBuf, recvLen, &offset, &outLen)) != NULL)
    {
        memcpy(recvBufTmp, ptr, outLen);

        recvLen -= (offset + outLen);
        memmove(recvBuf, (ptr + outLen), recvLen);

        /* 合法性检查 */
        ret = app_check_message(recvBufTmp, outLen);
        if (0 != ret)
        {
            ACSAMP_BUF_DEBUG(recvBufTmp, outLen, "app_check_message error[%d] msg recv", ret);
            continue;
        }

        /* 解析帧头 */
        memset(&tFrame, 0, sizeof(FRAME_HEAD_T));
        ret = app_get_frame(recvBufTmp, outLen, &tFrame, TRUE);
        if (0 != ret)
        {
            ACSAMP_BUF_DEBUG(recvBufTmp, outLen, "app_get_frame error[%d] msg recv", ret);
            continue;
        }
        ACSAMP_BUF_TRACE(tFrame.pApdu, tFrame.apduLen, "apdu recv");

        ret = protocol_analyze_apdu(&tFrame, type);
        if (0 != ret)
        {
            ACSAMP_FMT_DEBUG("protocol_analyze_apdu error[%d]\n", ret);
            continue;
        }
    }

    return 0;
}

/**********************************************************************
* @name：       ac_read_proc
* @brief：      交采模块交互处理
* @param[in] ：type - 数据大类
* @param[out]：
* @return    ：
* @Date      ：2019-12-6
**********************************************************************/
int ac_read_proc(int *fd, SAMPLE_TYPE_E type)
{
    int   nRet  = ERR_OK;
    int   fdDev = *fd;
    int32 recvLen = 0;
    int32 sendLen = 0;
    uint8 recvBuf[RECV_BUF_LEN] = {0};
    uint8 sendBuf[SEND_BUF_LEN] = {0};
    static uint32 sendCnt = 0;
    static uint32 recvCnt = 0;
    BOOL  bRelayOTHER = FALSE;
    BOOL  bRelay485 = FALSE;
    BOOL  bRelayUDP = FALSE;
    BOOL  bRelayTime = FALSE;
    uint8 pipe = 0;  //485串口通道
    static READ_AC_STATE state = INIT_AC_STATE;
    uint8 checkTime[5] = {0x40, 0x00, 0x02, 0x00, 0x1c};

    static int sendFlag = 0;
    
    switch (state)
    {
    case INIT_AC_STATE:
        if (fdDev < 0)
        {
            fdDev = ac_sample_dev_open();
            if (fdDev < 0)
            {
                state = INIT_AC_STATE;
                sleep(2);
            }
            else
            {
                *fd = fdDev;
                state = READ_AC_SEND_STATE;
                INIT_FMT_DEBUG("msg: the spi open ok.\n");
            }
        }
        else
        {
            state = READ_AC_SEND_STATE;
        }
        nRet = ERR_DEV_NRDY;
        break;

    case READ_AC_SEND_STATE:
        
        //其他任务，优先处理
        sendLen = ac_other_fun_package(type, sendBuf, SEND_BUF_LEN);
        if (sendLen > 0)
        {
            bRelayOTHER = TRUE;
            ACSAMP_BUF_DEBUG(sendBuf, sendLen, "处理其他转发报文");
            goto send;
        }
        
        sendLen = PlcReadQueue(pTimeRecvDown, sendBuf);
        if (sendLen > 0)
        {
            bRelayTime = TRUE;
            /* 对时校准 */
#if 1
            if(33 == sendLen && 0x68 == sendBuf[0] && 0x06 == sendBuf[14] && 0x01 == sendBuf[15]
                && 0 == memcmp(sendBuf + 17, checkTime, 5))
            {
                struct timespec  curtime = {0};
                uint32 sleepus = 0;
                DateTimeHex_t    dateTime = {0};
                OOP_DATETIME_S_T data = {0};
                OOP_OAD_U        oad = {0};
                uint32           newlen = 0;
                uint8            newbuf[64] = {0};
                
                clock_gettime(CLOCK_REALTIME, &curtime);
                if(curtime.tv_nsec >= 999000000)
                {
                    /* 999ms以上对时到下1s */
                    curtime.tv_sec += 1;
                }
                else if(curtime.tv_nsec >= 1000000)
                {
                    /* 延时到下1s */
                    sleepus = (1000000000 - curtime.tv_nsec)/1000;
                    usleep(sleepus - 500);
                    curtime.tv_sec += 1;
                }
                oad.value = 0x40000200;
                DT_Time2DateTimeHex(curtime.tv_sec, &dateTime);
                memcpy(&data, &dateTime, sizeof(DateTimeHex_t));
                //OOP_DATETIME_S_T的年高底位反转
                memrev((uint8*)&data, 2);  

                make_698msg_ac_set(oad, (uint8 *)&data, sizeof(OOP_DATETIME_S_T), newbuf, &newlen);
                if(newlen > 0)
                {
                    memcpy(sendBuf, newbuf, newlen);
                    sendLen = newlen;
                }

                ACSAMP_BUF_DEBUG(sendBuf, sendLen, "调整UDP转发时间");
                uint16 year = dateTime.year_h;
                year = (uint16)(year << 8) + dateTime.year_l;
                ACSAMP_FMT_DEBUG("dateTime:%d:%d:%d-%d:%d:%d\n", year, dateTime.month, 
                    dateTime.day, dateTime.hour, dateTime.min, dateTime.sec);
            }
            else
#endif
            {
                ACSAMP_BUF_DEBUG(sendBuf, sendLen, "优先处理对时");
            }
        
        }
        else
        {
            //优先判断UDP是否有内容
            sendLen = PlcReadQueue(pUdpRecvDown, sendBuf);
            if (sendLen > 0)
            {
                //优先处理UDP报文
                bRelayUDP = TRUE;
                ACSAMP_BUF_DEBUG(sendBuf, sendLen, "处理UDP转发报文");
            }
            else
            {
                sendLen = PlcReadQueue(pDealRecvDown[pipe], sendBuf);
                if (sendLen > 0)
                {
                    //优先处理485串口报文
                    bRelay485 = TRUE;
                    ACSAMP_BUF_DEBUG(sendBuf, sendLen, "优先处理485转发报文");
                }
                else
                {
                    //ac_read_sample(fdDev, type);
                    sendLen = ac_read_sample_package(type, sendBuf, SEND_BUF_LEN);
                    if (sendLen > 0)
                    {
                        sendCnt ++;
                    }
                    else
                    {
                            break;
                    }
                }
            }
        }
send:
        if (sendLen > 0)
        {
            ACSAMP_BUF_TRACE(sendBuf, sendLen, "fd[%d]发送报文", fdDev);
            if (gUartConf.enable)
            {
                nRet = commdrv_Send_232(fdDev, sendBuf, sendLen);
                if (nRet < 0)
                {
                    ACSAMP_BUF_DEBUG(sendBuf, sendLen, "发送报文失败nRet[%d]\n", nRet);
                    ac_sample_dev_close(fd);
                    state = INIT_AC_STATE;
                    break;
                }
            }
            else
            {
                ac_spi_send_data(fdDev, sendBuf, sendLen, recvBuf, 500000);
                sendFlag = 1;
            }
            
            sendLen = 0;
        }
        state = READ_AC_RECV_STATE;      // 状态不break往下顺序执行

    case READ_AC_RECV_STATE:
        if (gUartConf.enable)
        {
            recvLen = commdrv_Recv_232(fdDev, recvBuf, sizeof(recvBuf), 300, 8, 0xFF);
        }
        else
        {
            if (sendFlag == 1)
            {
                sendFlag = 0;
                if ((bRelayUDP == TRUE) || (bRelay485 == TRUE))
                {
                    recvLen = ac_spi_recv_data_timeout(fdDev, sendBuf, recvBuf, sizeof(recvBuf), 50000, 480);
                }
                else
                {
                    recvLen = ac_spi_recv_data_timeout(fdDev, sendBuf, recvBuf, sizeof(recvBuf), 50000, 480);
                }
            }
        }

        #if 1
        recvLen = ac_report_proc(fdDev, sendBuf, recvBuf, recvLen, sizeof(recvBuf));
        #endif
        if (recvLen <= 0)
        {
            state = READ_AC_SEND_STATE;        // 返回发送
            break;
        }
        else
        {
            ACSAMP_BUF_TRACE(recvBuf, recvLen, "fd[%d]接收报文ret[%d]", fdDev, recvLen);
        }

        //优先处理对时
        if (bRelayTime)
        {
            if(recvLen > sizeof(recvBuf))
            {
                recvLen = sizeof(recvBuf);//长度容错
            }
            ACSAMP_BUF_DEBUG(recvBuf, recvLen, "UDP转发接收报文");
            
            PlcWriteQueue(pTimeSendDown, recvBuf, recvLen);
            bRelayTime = FALSE;
            state = READ_AC_SEND_STATE;        // 返回发送
            break;
        }
        else if (bRelayUDP)//收到UDP相关应答报文
        {
            if(recvLen > sizeof(recvBuf))
            {
                recvLen = sizeof(recvBuf);//长度容错
            }
            ACSAMP_BUF_DEBUG(recvBuf, recvLen, "UDP转发接收报文");
            
            PlcWriteQueue(pUdpSendDown, recvBuf, recvLen);
            bRelayUDP = FALSE;
            state = READ_AC_SEND_STATE;        // 返回发送
            break;
        }
        else if (bRelay485)//收到485相关应答报文
        {
            if(recvLen > sizeof(recvBuf))
            {
                recvLen = sizeof(recvBuf);//长度容错
            }
            ACSAMP_BUF_DEBUG(recvBuf, recvLen, "485转发接收报文");
            
            PlcWriteQueue(pDealSendDown[pipe], recvBuf, recvLen);
            bRelay485 = FALSE;
            state = READ_AC_SEND_STATE;        // 返回发送
            break;
        }
        else if (bRelayOTHER)//收到其他相关应答报文
        {
            if(recvLen > sizeof(recvBuf))
            {
                recvLen = sizeof(recvBuf);//长度容错
            }
            ACSAMP_BUF_DEBUG(recvBuf, recvLen, "其他转发接收报文");
            
            bRelayOTHER = FALSE;
            state = READ_AC_SEND_STATE;        // 返回发送
            break;
        }
        else
        {
            nRet = ac_read_sample_parser(type, recvBuf, recvLen);
            if (ERR_OK == nRet)
            {
                recvCnt ++;
                ACSAMP_FMT_TRACE("成功率统计：接收数[%d]/发送数[%d]\n", recvCnt, sendCnt);
            }

            //统计量计算(非直接采集)
            UpdateStatistics(type);
            if (gIsSupport4K)
            {
                //大包传输时，每帧都带实时数据
                UpdateStatistics(SAMPLE_REALTIME);
            }
        }

        state = READ_AC_SEND_STATE;
        break;

    case CLOSE_AC_STATE:
        ac_sample_dev_close(fd);
        INIT_FMT_DEBUG("warm: the spi err. close spi\n");
        state = INIT_AC_STATE;
        nRet = ERR_DEV_NRDY;
        sleep(3);
        break;

    default:
        break;
    }

    return nRet;
}

/**********************************************************************
* @name：      ac_calc_deal_698
* @brief：     交采处理线程主控函数
* @param[in] ：
* @param[out]：
* @return    ：
* @Date      ：2019-12-6
**********************************************************************/
void ac_calc_deal_698(uint32 *cntchk)
{   
    uint16 nEnergySecCnt = 60;            //电量采集秒计时器
    uint16 nDemandSecCnt = 296;           //需量采集秒计时器
    uint16 nDemandSecCnt1 = 295;          //需量采集秒计时器
    uint16 nHarmonicSecCnt = 2;           //谐波采集秒计时器
    uint16 nRealtimeSecCnt = 0;           //实时数据采集秒计时器
    uint8  nTaskType = SAMPLE_NULL;       //任务类型

    uint16 nEnergySecCycle = 60;          //电量采集周期
    uint16 nDemandSecCycle = 300;         //需量采集周期
    uint16 nDemandSecCycle1 = 300;        //需量采集周期
    uint16 nHarmonicSecCycle = 5;         //谐波采集周期
    uint16 nRealtimeSecCycle = 1;         //实时数据采集周期

    BOOL   isUpdateAC = FALSE;
    BOOL   isGetVer = FALSE;

    uint32 cnt = 0;
    int32  ret = -1;

    //报文接收失败统计
    uint32 spiRstCnt = 0;

    //预处理报文
    uint32 acPreCnt = 0;
    uint32 acPreFailCnt = 0;

    TimeManageInit();                      //时间管理
    SharedMemMark.Bitsect.FreshMark = 1;   //重新刷新交采共享数据

    //先初始化下设备
    while ((fd_ac = ac_sample_dev_open()) < 0)
    {
        *cntchk = 0;
        cnt ++;
        if (cnt > 10)
        {
            INIT_FMT_DEBUG("ac_sample_dev_open err fd[%d]\n", fd_ac);
            exit(1);
        }
        fd_ac = -1;
        sleep(1); 
    }

    ClearCommRecvBuff(fd_ac);

    //获取版本
    cnt = 0;
    while (ERR_OK != ac_get_698ver(&fd_ac))
    {
        *cntchk = 0;
        cnt ++;
        if (cnt > 5)
        {
            UPDATE_FMT_DEBUG("ac_get_ver err fd[%d]\n", fd_ac);
            break;
        }
        sleep(5);
    }

    ac_get_connect(fd_ac, &gIsSupport4K);

    #if UPDATE_SELF_ADAPTION == 1
    gIsUpdate698 = ac_is_support_update698(fd_ac);
    #endif

    //成功获取版本，进行一次对时
    if (cnt <= 5)
    {
        g_nAcSetDateFlag = 1;
        g_nAcSetTimeFlag = 1;
    }

    while (1)
    {
        *cntchk = 0;
        TimeManageStart();                 //一轮时间管理开始
        nTaskType = SAMPLE_NULL;           //任务类型

        // 过秒处理
        if (gTimeFlag.BitSect.Second)
        {
            nEnergySecCnt ++;
            nDemandSecCnt ++;
            nDemandSecCnt1 ++;
            nHarmonicSecCnt ++;
            nRealtimeSecCnt ++;
            isUpdateAC = isJCUpdateRunning();
            isGetVer = isJCNeedGetVer();     
            #if 0
            ac_spi_reset_proc(&spiRstCnt);
            #endif
        }

        //检测任务类型
        if (nDemandSecCnt >= nDemandSecCycle)
        {
            nDemandSecCnt = 0;
            nTaskType = SAMPLE_DEMAND;  //任务类型 需量
            ACSAMP_FMT_TRACE("TASK:DEMAND,   Cycle[%d]\n", nDemandSecCycle);
        }
        else if (nDemandSecCnt1 >= nDemandSecCycle1)
        {
            nDemandSecCnt1 = 0;
            nTaskType = SAMPLE_DEMAND1;  //任务类型 需量
            ACSAMP_FMT_TRACE("TASK:DEMAND1,  Cycle[%d]\n", nDemandSecCycle1);
        }
        else if (nHarmonicSecCnt >= nHarmonicSecCycle)
        {
            nHarmonicSecCnt = 0;
            nTaskType = SAMPLE_HARMONIC;  //任务类型 谐波
            ACSAMP_FMT_TRACE("TASK:HARMONIC, Cycle[%d]\n", nHarmonicSecCycle);
        }
        else if (nEnergySecCnt >= nEnergySecCycle)
        {
            nEnergySecCnt = 0;
            if (gIsSupport4K)
            {
                nTaskType = SAMPLE_HENERGY;  //任务类型 高精度电量
            }
            else
            {
                nTaskType = SAMPLE_ENERGY;  //任务类型 电量
            }
            ACSAMP_FMT_TRACE("TASK:ENERGY,   Cycle[%d]\n", nEnergySecCycle);
        }
        else if (nRealtimeSecCnt >= nRealtimeSecCycle)
        {
            nRealtimeSecCnt = 0;
            nTaskType = SAMPLE_REALTIME;  //任务类型 实时数据
            ACSAMP_FMT_TRACE("TASK:REALTIME, Cycle[%d]\n", nRealtimeSecCycle);
        }
        else
        {
            nTaskType = SAMPLE_NULL;  //任务类型 空
        }

        if (isUpdateAC == TRUE)
        {
            #if UPDATE_SELF_ADAPTION == 1
            if (!gIsUpdate698)
            {
                ReadACSample(TRUE, FALSE, nTaskType);
            }
            else
            #endif
            {
                ac_update698_proc(&fd_ac);
            } 

        }
        else if (isGetVer == TRUE)
        {
            ret = ac_get_698ver(&fd_ac);
            if (ret != ERR_OK)
            {
                sleep(5);
            }
        }
        else
        {
            //预处理，比如参数初始化后的处理
            if ((g_nAcParamInitFlag == 1) && (gPreprocListNum > 0))
            {
                ret = ac_pre_proc(&fd_ac, &acPreCnt);
                if (ret != ERR_OK)
                {
                    ACSAMP_FMT_DEBUG("预处理失败 索引[%d] 尝试次数[%d]\n", acPreCnt, acPreFailCnt);
                    acPreFailCnt++;
                    if (acPreFailCnt > 3)
                    {
                        acPreFailCnt = 0;
                        g_nAcParamInitFlag = 0;
                        acPreCnt = 0;
                    }
                    continue;
                }
                else
                {
                    acPreFailCnt = 0;
                    if (acPreCnt >= gPreprocListNum)
                    {
                        g_nAcParamInitFlag = 0;
                        acPreCnt = 0;
                        nTaskType = SAMPLE_PARAM_INIT;
                    }
                    else
                    {
                        continue;
                    }
                }
            }
            else if ((g_nAcParamInitFlag == 1) && (gPreprocListNum == 0))
            {
                g_nAcParamInitFlag = 0;
                nTaskType = SAMPLE_PARAM_INIT;
            }
            else if (g_nPowerOffFlag == 1)
            {
                g_nPowerOffFlag = 0;
                nTaskType = SAMPLE_POWER_OFF;
            }

            if (ERR_OK != ac_read_proc(&fd_ac, nTaskType))
            {
                spiRstCnt ++;

                //重试机制，再调度一次
                switch (nTaskType)
                {
                case SAMPLE_DEMAND:
                    nDemandSecCnt = nDemandSecCycle;
                    break;
                case SAMPLE_DEMAND1:
                    nDemandSecCnt1 = nDemandSecCycle1;
                    break;
                case SAMPLE_HARMONIC:
                    nHarmonicSecCnt = nHarmonicSecCycle;
                    break; 
                case SAMPLE_HENERGY:
                case SAMPLE_ENERGY:
                    nEnergySecCnt = nEnergySecCycle;
                    break;    
                case SAMPLE_REALTIME:
                    nRealtimeSecCnt = nRealtimeSecCycle;
                    break;                   
                default:
                    break;
                }
            }
            else
            {
                spiRstCnt = 0;
            }
        }

        usleep(10000);                
        TimeManageEnd();                 // 一轮时间管理结束
    }

    return;
}
