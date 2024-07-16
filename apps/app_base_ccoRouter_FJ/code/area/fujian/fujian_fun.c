#ifdef AREA_FUJIAN

#include "CcoCommon.h"
#include "CcoLib.h"
#include "CcoPrtl.h"
#include "CcoDebug.h"
#include "area.h"
#include "Mqtt.h"

CCO_AMR_T   g_CcoAmr = {0};

extern uint32  g_CcoMode;

void CcoSimpleAck(INFO_DATA_T *infoMsgData, BOOL bResult)
{
    memset(&infoMsgData->aPayloadData[0], 0x00, infoMsgData->nPayloadLength);
    infoMsgData->MsgRPM = 0;
    infoMsgData->nPayloadLength = 1;
    infoMsgData->aPayloadData[0] = 0x00;

    if(bResult)
    {
        infoMsgData->aPayloadData[0] = 0x01;
    }
    PlcWriteQueue(pAmrSendUp, (uint8 *)infoMsgData, sizeof(MSG_INFO_T) + infoMsgData->nPayloadLength);
}

void CcoSimpleAckUint8(INFO_DATA_T *infoMsgData, uint8 value)
{
    memset(&infoMsgData->aPayloadData[0], 0x00, infoMsgData->nPayloadLength);
    infoMsgData->MsgRPM = 0;
    infoMsgData->nPayloadLength = 1;
    infoMsgData->aPayloadData[0] = value;

    PlcWriteQueue(pAmrSendUp, (uint8 *)infoMsgData, sizeof(MSG_INFO_T) + infoMsgData->nPayloadLength);
}

void CcoSimpleAckPayload(INFO_DATA_T *infoMsgData, uint8 *payload, uint16 len)
{
    memset(&infoMsgData->aPayloadData[0], 0x00, infoMsgData->nPayloadLength);
    infoMsgData->MsgRPM = 0;

    if (len > sizeof(infoMsgData->aPayloadData))
    {
        len = sizeof(infoMsgData->aPayloadData);
    }

    infoMsgData->nPayloadLength = len;
    memcpy(infoMsgData->aPayloadData, payload, len);

    PlcWriteQueue(pAmrSendUp, (uint8 *)infoMsgData, sizeof(MSG_INFO_T) + infoMsgData->nPayloadLength);
}


/*******************************************************************************
* 函数名称: Cco3762ConfirmAck
* 函数功能: 确认帧应答
* 输入参数: 无
* 输出参数: 无
* 返 回 值: 无
*******************************************************************************/
void Cco3762ConfirmAck(uint8 seq, uint16 Fn, uint8 index)
{
    AppData_T appData = {0};
    appData.RInfo[5] = seq; /* 从模块上报帧中获取帧序列号 */

    appData.AFN = 0x50;

    if (Fn == 1 || Fn == 3)
    {
        appData.Fn = Fn;
        appData.DataUnitLen = 0;
    }
    else
    {
        appData.Fn = 2;
        appData.DataUnitLen = 1;
        appData.DataUnitBuff[0] = 0xFF;
    }

    CcoPackFrameData(&appData, index);
}

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
    else if(eleLen <= 0xff)
    {
        indata[0] = 0x81;
        indata[1] = eleLen;
        return 2;
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

/* 初始化消息处理51F1F2F3 */
void CcoInitMsg(INFO_DATA_T *infoMsgData, CCO_AMR_T *amr)
{
    uint32  port = 0;  //端口号
    uint16  offset = 0;  //偏移位
    uint8   type = 0;  /* 1:硬件初始化（复位）2:参数区初始化（恢复至出厂设置）3:数据区初始化（节点侦听信息） */
    AppData_T appData = {0};

    CCOUP_FMT_DEBUG("CcoInitMsg 初始化消息 begin\n");
    //解析消息MSG
    offset = 0;
    port = *(uint32 *)&infoMsgData->aPayloadData[offset];  //端口号
    port = ntohl(port);
    offset += 4;
    type = infoMsgData->aPayloadData[offset];  /* 1:硬件初始化（复位）2:参数区初始化（恢复至出厂设置）3:数据区初始化（节点侦听信息） */

    CCOUP_FMT_DEBUG("CcoInitMsg 初始化消息 端口号port=0x%08X, 操作type=%u (1:硬件初始化,2:参数区初始化,3:数据区初始化)\n", port, type);

    if ((port != PORT_F2090201) && (port != PORT_F2090202))
    {
        CCOUP_FMT_DEBUG("CcoInitMsg 端口号错误!\n");
    }

    if (type == CCO_HW_INIT) /* 硬件初始化（复位） */
    {
        amr->flag.initFlag = CCO_HW_INIT;
        appData.Fn = CCO_HW_INIT;
    }
    else if (type == CCO_PARA_INIT) /* 参数区初始化（恢复至出厂设置） */
    {
        amr->flag.initFlag = CCO_PARA_INIT;
        appData.Fn = CCO_PARA_INIT;
    }
    else if (type == CCO_DATA_INIT) /* 数据区初始化（节点侦听信息） */
    {
        amr->flag.initFlag = CCO_DATA_INIT;
        appData.Fn = CCO_DATA_INIT;
    }
    else //异常
    {
        CCOUP_FMT_DEBUG("CcoInitMsg 初始化消息 操作类型错误!\n");
        //消息组帧发至消息中心
        CcoSimpleAck(infoMsgData, FALSE);
        return;
    }

    //消息组帧发至消息中心
    CcoSimpleAck(infoMsgData, TRUE);

    appData.AFN = 0x51;
    appData.DataUnitLen = 0;
    CcoPackFrameData(&appData, 0);
    return;
}

/* 查询本地模块参数消息处理53F1 */
void CcoQueryParaMsg(INFO_DATA_T *infoMsgData, CCO_AMR_T *amr)
{
    uint32  port = 0;  //端口号
    CCO_QUERY_PARA_ACK *ack = &amr->ack.queryParaAck;
    CCO_QUERY_PARA_ACK ackSend = {0};

    CCOUP_FMT_DEBUG("CcoQueryParaMsg 查询本地模块参数消息 begin\n");
    port = *(uint32 *)&infoMsgData->aPayloadData[0];  //端口号
    port = ntohl(port);

    if ((port != PORT_F2090201) && (port != PORT_F2090202))
    {
        CCOUP_FMT_DEBUG("CcoQueryParaMsg 端口号错误!\n");
    }

    amr->flag.queryParaFlag = 1;

    if (amr->flag.queryParaAckFlag == 1)
    {
        ackSend.taskObjNum = htons(ack->taskObjNum);
        ackSend.meterObjNum = ack->meterObjNum;
    }

    CcoSimpleAckPayload(infoMsgData, (uint8 *)&ackSend, sizeof(ackSend));
    return;
}

/* 查询模块地址消息处理53F2 */
void CcoQueryAddrMsg(INFO_DATA_T *infoMsgData, CCO_AMR_T *amr)
{
    uint32  port = 0;  //端口号

    CCOUP_FMT_DEBUG("CcoQueryAddrMsg 查询模块地址消息 begin\n");
    port = *(uint32 *)&infoMsgData->aPayloadData[0];  //端口号
    port = ntohl(port);

    if ((port != PORT_F2090201) && (port != PORT_F2090202))
    {
        CCOUP_FMT_DEBUG("CcoQueryAddrMsg 端口号错误!\n");
    }

    CcoSimpleAckPayload(infoMsgData, gTerminalAddr[0], CCO_ADDR_LEN);
    return;
}

/* 查询节点信息消息处理53F4 */
void CcoQueryNodeInfoMsg(INFO_DATA_T *infoMsgData, CCO_AMR_T *amr)
{
    uint16  offset = 0;  //偏移位
    CCO_QUERY_NODE_INFO_REQ *req = &amr->req.queryNodeInfoReq;
    AppData_T appData = {0};
    uint16  unitOffset = 0;

    CCOUP_FMT_DEBUG("CcoQueryNodeInfoMsg 查询节点信息消息 begin\n");
    /* 解析消息 */
    offset = 0;
    req->port = *(uint32 *)&infoMsgData->aPayloadData[offset];  //端口号
    req->port = ntohl(req->port);
    offset += 4;
    req->type = infoMsgData->aPayloadData[offset];
    offset += 1;
    memcpy(req->addr, &infoMsgData->aPayloadData[offset], CCO_ADDR_LEN);

    if ((req->port != PORT_F2090201) && (req->port != PORT_F2090202))
    {
        CCOUP_FMT_DEBUG("CcoQueryNodeInfoMsg 端口号错误!\n");
    }

    req->label = infoMsgData->MsgLabel;
    req->index = infoMsgData->MsgIndex;
    amr->flag.queryNodeInfoFlag = 1;

    appData.AFN = 0x53;
    appData.Fn = 4;
    appData.DataUnitBuff[unitOffset] = req->type;
    unitOffset += 1;
    memcpy_r(&appData.DataUnitBuff[unitOffset], req->addr, CCO_ADDR_LEN);
    unitOffset += CCO_ADDR_LEN;
    appData.DataUnitLen = unitOffset;
    CcoPackFrameData(&appData, 0);
    return;
}

/* 查询节点相位信息消息处理53F5 */
void CcoQueryNodePhaseMsg(INFO_DATA_T *infoMsgData, CCO_AMR_T *amr)
{
    uint16  offset = 0;  //偏移位
    CCO_QUERY_NODE_PHASE_REQ *req = &amr->req.queryNodePhaseReq;
    AppData_T appData = {0};
    uint16  unitOffset = 0;

    CCOUP_FMT_DEBUG("CcoQueryNodePhaseMsg 查询节点相位信息消息 begin\n");
    /* 解析消息 */
    offset = 0;
    req->port = *(uint32 *)&infoMsgData->aPayloadData[offset];  //端口号
    req->port = ntohl(req->port);
    offset += 4;
    req->type = infoMsgData->aPayloadData[offset];
    offset += 1;
    memcpy(req->addr, &infoMsgData->aPayloadData[offset], CCO_ADDR_LEN);

    if ((req->port != PORT_F2090201) && (req->port != PORT_F2090202))
    {
        CCOUP_FMT_DEBUG("CcoQueryNodePhaseMsg 端口号错误!\n");
    }

    req->label = infoMsgData->MsgLabel;
    req->index = infoMsgData->MsgIndex;
    amr->flag.queryNodePhaseFlag = 1;

    appData.AFN = 0x53;
    appData.Fn = 5;
    appData.DataUnitBuff[unitOffset] = req->type;
    unitOffset += 1;
    memcpy_r(&appData.DataUnitBuff[unitOffset], req->addr, CCO_ADDR_LEN);
    unitOffset += CCO_ADDR_LEN;
    appData.DataUnitLen = unitOffset;
    CcoPackFrameData(&appData, 0);
    return;
}

/* 查询模块串口参数消息处理53F6 */
void CcoQueryUartParaMsg(INFO_DATA_T *infoMsgData, CCO_AMR_T *amr)
{
    uint32  port = 0;  //端口号
    CCO_QUERY_UART_PARA_ACK *ack = &amr->ack.queryUartParaAck;
    CCO_QUERY_UART_PARA_ACK ackSend = {0};

    CCOUP_FMT_DEBUG("CcoQueryUartParaMsg 查询模块串口参数消息 begin\n");
    port = *(uint32 *)&infoMsgData->aPayloadData[0];  //端口号
    port = ntohl(port);

    if ((port != PORT_F2090201) && (port != PORT_F2090202))
    {
        CCOUP_FMT_DEBUG("CcoQueryUartParaMsg 端口号错误!\n");
    }

    amr->flag.queryUartParaFlag = 1;

    if (amr->flag.queryUartParaAckFlag == 1)
    {
        ackSend = *ack;
    }

    CcoSimpleAckPayload(infoMsgData, (uint8 *)&ackSend, sizeof(ackSend));
    return;
}
uint32 g_CcoModeAck = 0;
/* 查询模块协议模式消息处理53F10 */
void CcoQueryPrtlModeMsg(INFO_DATA_T *infoMsgData, CCO_AMR_T *amr)
{
    uint32  port = 0;  //端口号
    uint8   value = (g_CcoModeAck == CCO_MODE_FUJIAN ? CCO_MODE_FUJIAN : CCO_MODE_GW);
    CCOUP_FMT_DEBUG("CcoQueryPrtlModeMsg 查询模块协议模式消息 begin\n");

    port = *(uint32 *)&infoMsgData->aPayloadData[0];  //端口号
    port = ntohl(port);

    if ((port != PORT_F2090201) && (port != PORT_F2090202))
    {
        CCOUP_FMT_DEBUG("CcoQueryPrtlModeMsg 端口号错误!\n");
    }

    CcoSimpleAckUint8(infoMsgData, value);
    amr->flag.queryPrtlModeFlag = 1;
    return;
}

/* 模块地址设置消息处理55F1 */
void CcoSetAddrMsg(INFO_DATA_T *infoMsgData, CCO_AMR_T *amr)
{
    uint16  offset = 0;  //偏移位
    CCO_SET_ADDR_REQ *req = &amr->req.setAddrReq;
    AppData_T appData = {0};
    int ret = 0;

    CCOUP_FMT_DEBUG("CcoSetAddrMsg 模块地址设置消息 begin\n");
    /* 解析消息 */
    offset = 0;
    req->port = *(uint32 *)&infoMsgData->aPayloadData[offset];  //端口号
    req->port = ntohl(req->port);
    offset += 4;
    memcpy(req->addr, &infoMsgData->aPayloadData[offset], CCO_ADDR_LEN);
    CCOUP_FMT_DEBUG("Old gTerminalAddr:%02x %02x %02x %02x %02x %02x", gTerminalAddr[0][0], gTerminalAddr[0][1],
                    gTerminalAddr[0][2], gTerminalAddr[0][3], gTerminalAddr[0][4], gTerminalAddr[0][5]);
    memcpy(gTerminalAddr[0], req->addr, CCO_ADDR_LEN);
    CCOUP_FMT_DEBUG("New gTerminalAddr:%02x %02x %02x %02x %02x %02x", gTerminalAddr[0][0], gTerminalAddr[0][1],
                    gTerminalAddr[0][2], gTerminalAddr[0][3], gTerminalAddr[0][4], gTerminalAddr[0][5]);

    //将终端地址保存至私有文件
    ret = write_pdata_xram(gModuleManuName[0], (char *)&gTerminalAddr[0][0], 0, sizeof(gTerminalAddr[0]));
    CCOUP_FMT_DEBUG(0, "gTerminalAddr() Ret = write_pdata_xram() Ret:%d...\n", ret);

    if ((req->port != PORT_F2090201) && (req->port != PORT_F2090202))
    {
        CCOUP_FMT_DEBUG("CcoSetAddrMsg 端口号%08X错误!\n", req->port);
    }

    CcoSimpleAck(infoMsgData, TRUE);
    amr->flag.setAddrFlag = 1;

    appData.AFN = 0x55;
    appData.Fn = 1;
    memcpy_r(appData.DataUnitBuff, req->addr, CCO_ADDR_LEN);
    appData.DataUnitLen = CCO_ADDR_LEN;
    CcoPackFrameData(&appData, 0);
    return;
}

/* 从节点上报设置消息处理55F2 */
void CcoSetStaReportMsg(INFO_DATA_T *infoMsgData, CCO_AMR_T *amr)
{
    uint16  offset = 0;  //偏移位
    CCO_SET_STA_REPORT_REQ *req = &amr->req.setStaReportReq;
    AppData_T appData = {0};
    uint16  unitOffset = 0;
    uint32  i = 0;

    CCOUP_FMT_DEBUG("CcoSetStaReportMsg 从节点上报设置消息 begin\n");
    /* 解析消息 */
    offset = 0;
    req->port = *(uint32 *)&infoMsgData->aPayloadData[offset];  //端口号
    req->port = ntohl(req->port);
    offset += 4;
    req->num = infoMsgData->aPayloadData[offset];
    offset += 1;

    for (i = 0; i < req->num && i < CCO_MAX_CONFIG_NUM; i++)
    {
        req->config[i].type = infoMsgData->aPayloadData[offset];
        offset += 1;
        memcpy(req->config[i].addr, &infoMsgData->aPayloadData[offset], CCO_ADDR_LEN);
        offset += CCO_ADDR_LEN;
        req->config[i].flag = infoMsgData->aPayloadData[offset];
        offset += 1;
    }
    
    CcoSimpleAck(infoMsgData, TRUE);
    amr->flag.setStaReportFlag = 1;

    appData.AFN = 0x55;
    appData.Fn = 2;
    appData.DataUnitBuff[unitOffset] = (req->num <= CCO_MAX_CONFIG_NUM ? req->num : CCO_MAX_CONFIG_NUM);
    unitOffset += 1;

    for (i = 0; i < req->num && i < CCO_MAX_CONFIG_NUM; i++)
    {
        appData.DataUnitBuff[unitOffset] = req->config[i].type;
        unitOffset += 1;
        memcpy(&appData.DataUnitBuff[unitOffset], req->config[i].addr, CCO_ADDR_LEN);
        unitOffset += CCO_ADDR_LEN;
        appData.DataUnitBuff[unitOffset] = req->config[i].flag;
        unitOffset += 1;
    }

    appData.DataUnitLen = unitOffset;
    CcoPackFrameData(&appData, 0);
    return;
}

/* 启动获取广播延时消息处理55F4 */
void CcoStartGetBroadDelayMsg(INFO_DATA_T *infoMsgData, CCO_AMR_T *amr)
{
    uint16  offset = 0;  //偏移位
    CCO_START_GET_BROAD_DELAY_REQ *req = &amr->req.startGetBroadDelayReq;
    AppData_T appData = {0};
    uint16  unitOffset = 0;

    CCOUP_FMT_DEBUG("CcoStartGetBroadDelayMsg 启动获取广播延时消息 begin\n");
    /* 解析消息 */
    offset = 0;
    req->port = *(uint32 *)&infoMsgData->aPayloadData[offset];  //端口号
    req->port = ntohl(req->port);
    offset += 4;
    req->type = infoMsgData->aPayloadData[offset];
    offset += 1;
    req->duration = infoMsgData->aPayloadData[offset];

    if ((req->port != PORT_F2090201) && (req->port != PORT_F2090202))
    {
        CCOUP_FMT_DEBUG("CcoStartGetBroadDelayMsg 端口号错误!\n");
    }

    CcoSimpleAck(infoMsgData, TRUE);
    amr->flag.startGetBroadDelayFlag = 1;

    appData.AFN = 0x55;
    appData.Fn = 4;
    appData.DataUnitBuff[unitOffset] = req->type;
    unitOffset += 1;
    appData.DataUnitBuff[unitOffset] = req->duration;
    unitOffset += 1;
    appData.DataUnitLen = unitOffset;
    CcoPackFrameData(&appData, 0);
    return;
}

/* 广播消息处理55F3 */
void CcoBroadcastMsg(INFO_DATA_T *infoMsgData, CCO_AMR_T *amr)
{
    uint16  offset = 0;  //偏移位
    uint8   lenOffset = 0;
    CCO_BROADCAST_REQ *req = &amr->req.broadcastReq;
    AppData_T appData = {0};
    uint16  unitOffset = 0;
    uint8   copyLen = 0;

    CCOUP_FMT_DEBUG("CcoBroadcastMsg 广播消息 begin\n");
    /* 解析消息 */
    offset = 0;
    req->port = *(uint32 *)&infoMsgData->aPayloadData[offset];  //端口号
    req->port = ntohl(req->port);
    offset += 4;
    req->type = infoMsgData->aPayloadData[offset];
    offset += 1;
    req->duration = infoMsgData->aPayloadData[offset];
    offset += 1;

    req->bufLen = 0;

    if (infoMsgData->nPayloadLength > offset)
    {
        req->bufLen = get_len_offset(&infoMsgData->aPayloadData[offset], &lenOffset);
        memcpy_s(req->buf, sizeof(req->buf), &infoMsgData->aPayloadData[offset + lenOffset], req->bufLen);
    }

    if ((req->port != PORT_F2090201) && (req->port != PORT_F2090202))
    {
        CCOUP_FMT_DEBUG("CcoBroadcastMsg 端口号错误!\n");
    }

    CcoSimpleAck(infoMsgData, TRUE);
    amr->flag.broadcastFlag = 1;

    appData.AFN = 0x55;
    appData.Fn = 3;
    appData.DataUnitBuff[unitOffset] = req->type;
    unitOffset += 1;
    appData.DataUnitBuff[unitOffset] = req->duration;
    unitOffset += 1;
    appData.DataUnitBuff[unitOffset] = req->bufLen;
    unitOffset += 1;
    copyLen = (req->bufLen <= 255 ? req->bufLen : 255);
    memcpy(&appData.DataUnitBuff[unitOffset], req->buf, copyLen);
    unitOffset += copyLen;
    appData.DataUnitLen = unitOffset;
    CcoPackFrameData(&appData, 0);
    return;
}

/* 启动从节点主动注册消息处理55F6 */
void CcoStartStaRegMsg(INFO_DATA_T *infoMsgData, CCO_AMR_T *amr)
{
    uint16  offset = 0;  //偏移位
    CCO_START_STA_REG_REQ *req = &amr->req.startStaRegReq;
    AppData_T appData = {0};

    CCOUP_FMT_DEBUG("CcoStartStaRegMsg 启动从节点主动注册消息 begin\n");
    /* 解析消息 */
    offset = 0;
    req->port = *(uint32 *)&infoMsgData->aPayloadData[offset];  //端口号
    req->port = ntohl(req->port);
    offset += 4;
    req->duration = *(uint16 *)&infoMsgData->aPayloadData[offset];

    if ((req->port != PORT_F2090201) && (req->port != PORT_F2090202))
    {
        CCOUP_FMT_DEBUG("CcoStartStaRegMsg 端口号错误!\n");
    }

    CcoSimpleAck(infoMsgData, TRUE);
    amr->flag.startStaRegFlag = 1;

    appData.AFN = 0x55;
    appData.Fn = 6;
    memcpy(appData.DataUnitBuff, &req->duration, 2);
    appData.DataUnitLen = 2;
    CcoPackFrameData(&appData, 0);
    return;
}

/* 结束当前任务消息处理55F7 */
void CcoEndCurrentTaskMsg(INFO_DATA_T *infoMsgData, CCO_AMR_T *amr)
{
    uint32  port = 0;  //端口号
    AppData_T appData = {0};

    CCOUP_FMT_DEBUG("CcoEndCurrentTaskMsg 结束当前任务消息 begin\n");
    port = *(uint32 *)&infoMsgData->aPayloadData[0];  //端口号
    port = ntohl(port);

    if ((port != PORT_F2090201) && (port != PORT_F2090202))
    {
        CCOUP_FMT_DEBUG("CcoEndCurrentTaskMsg 端口号错误!\n");
    }

    CcoSimpleAck(infoMsgData, TRUE);
    amr->flag.endCurrentTaskFlag = 1;

    appData.AFN = 0x55;
    appData.Fn = 7;
    appData.DataUnitLen = 0;
    CcoPackFrameData(&appData, 0);
    return;
}

/* 启动预告任务执行消息处理55F8F18 */
void CcoStartPreviewExecMsg(INFO_DATA_T *infoMsgData, CCO_AMR_T *amr)
{
    uint16  offset = 0;  //偏移位
    CCO_START_PREVIEW_EXEC_REQ *req = &amr->req.startPreviewExecReq;
    AppData_T appData = {0};

    CCOUP_FMT_DEBUG("CcoStartPreviewExecMsg 启动预告任务执行消息 begin\n");
    /* 解析消息 */
    offset = 0;
    req->port = *(uint32 *)&infoMsgData->aPayloadData[offset];  //端口号
    req->port = ntohl(req->port);
    offset += 4;
    req->type = infoMsgData->aPayloadData[offset];

    if ((req->port != PORT_F2090201) && (req->port != PORT_F2090202))
    {
        CCOUP_FMT_DEBUG("CcoStartPreviewExecMsg 端口号错误!\n");
    }

    CcoSimpleAck(infoMsgData, TRUE);
    amr->flag.startPreviewExecFlag = 1;

    appData.AFN = 0x55;
    appData.Fn = (req->type == 0 ? 8 : 18);
    appData.DataUnitLen = 0;
    CcoPackFrameData(&appData, 0);
    return;
}

/* 预告抄读对象消息处理55F9 */
void CcoPreviewMeterObjMsg(INFO_DATA_T *infoMsgData, CCO_AMR_T *amr)
{
    uint16  offset = 0;  //偏移位
    uint8   lenOffset = 0;
    uint16  num = 0;
    uint32  i = 0;
    CCO_PREVIEW_METER_OBJ_REQ *req = &amr->req.previewMeterObjReq;
    AppData_T appData = {0};
    uint16  unitOffset = 0;

    CCOUP_FMT_DEBUG("CcoPreviewMeterObjMsg 预告抄读对象消息 begin\n");
    /* 解析消息 */
    offset = 0;
    req->port = *(uint32 *)&infoMsgData->aPayloadData[offset];  //端口号
    req->port = ntohl(req->port);
    offset += 4;
    memcpy_r((uint8 *)&req->num, &infoMsgData->aPayloadData[offset], 2); /* 预告数量 */
    offset += 2;
    req->modify = infoMsgData->aPayloadData[offset]; /* 延时修改 */
    offset += 1;
    num = get_len_offset(&infoMsgData->aPayloadData[offset], &lenOffset);
    offset += lenOffset;

    if (req->num != num)
    {
        CCOUP_FMT_DEBUG("CcoPreviewMeterObjMsg 预告数量%u与SEQUENCE=%u不匹配!\n", req->num, num);
        //消息组帧发至消息中心
        CcoSimpleAck(infoMsgData, FALSE);
        return;
    }

    if (offset + req->num * sizeof(CCO_AMR_TASK) > infoMsgData->nPayloadLength)
    {
        CCOUP_FMT_DEBUG("CcoPreviewMeterObjMsg 报文预期长度%u与实际长度%u不匹配!\n",
                        offset + req->num * sizeof(CCO_AMR_TASK), infoMsgData->nPayloadLength);
        //消息组帧发至消息中心
        CcoSimpleAck(infoMsgData, FALSE);
        return;
    }

    for (i = 0; i < req->num; i++)
    {
        req->task[i].id = *(uint16 *)&infoMsgData->aPayloadData[offset]; /* 任务序号 */
        offset += 2;
        req->task[i].type = infoMsgData->aPayloadData[offset]; /* 通信对象类型 */
        offset += 1;
        memcpy(req->task[i].addr, &infoMsgData->aPayloadData[offset], CCO_ADDR_LEN);
        offset += CCO_ADDR_LEN;
    }

    CcoSimpleAck(infoMsgData, TRUE);
    amr->flag.previewMeterObjFlag = 1;

    appData.AFN = 0x55;
    appData.Fn = 9;
    memcpy(&appData.DataUnitBuff[unitOffset], &req->num, 2);
    unitOffset += 2;
    appData.DataUnitBuff[unitOffset] = req->modify;
    unitOffset += 1;

    for (i = 0; i < req->num; i++)
    {
        memcpy(&appData.DataUnitBuff[unitOffset], &req->task[i].id, 2);
        unitOffset += 2;
        appData.DataUnitBuff[unitOffset] = req->task[i].type;
        unitOffset += 1;
        memcpy_r(&appData.DataUnitBuff[unitOffset], req->task[i].addr, CCO_ADDR_LEN);
        unitOffset += CCO_ADDR_LEN;
    }

    appData.DataUnitLen = unitOffset;
    CcoPackFrameData(&appData, 0);
    return;
}

/* 设置通信协议模式消息处理55F10 */
void CcoSetPrtlMsg(INFO_DATA_T *infoMsgData, CCO_AMR_T *amr)
{
    uint16  offset = 0;  //偏移位
    CCO_SET_PRTL_REQ *req = &amr->req.setPrtlReq;
    AppData_T appData = {0};
    int     ret = 0;

    CCOUP_FMT_DEBUG("CcoSetPrtlMsg 设置通信协议模式消息 begin\n");
    /* 解析消息 */
    offset = 0;
    req->port = *(uint32 *)&infoMsgData->aPayloadData[offset];  //端口号
    req->port = ntohl(req->port);
    offset += 4;
    req->type = infoMsgData->aPayloadData[offset]; /* 模式字 */

    if ((req->port != PORT_F2090201) && (req->port != PORT_F2090202))
    {
        CCOUP_FMT_DEBUG("CcoSetPrtlMsg 端口号错误!\n");
    }

    CcoSimpleAck(infoMsgData, TRUE);
    amr->flag.setPrtlFlag = 1;

    //if (req->type != g_CcoMode)
    //{
    g_CcoMode = req->type;

    //将协议模式保存至私有文件
  
    ret = write_pdata_xram(gModuleManuName[0], (char *)&g_CcoMode, CCO_MODE_XRAM_OFFSET, sizeof(g_CcoMode));
    CCOUP_FMT_DEBUG(0, "g_CcoMode() Ret = write_pdata_xram() Ret:%d...\n", ret);

    appData.AFN = 0x55;
    appData.Fn = 10;
    appData.DataUnitLen = 1;
    appData.DataUnitBuff[0] = g_CcoMode;
    CcoPackFrameData(&appData, 0);
    /* 模式发生变更，重新初始化 */
    if (req->type == CCO_MODE_FUJIAN)
    {
        amr->flag.initFlag = UINT_MAX;
    }
    //}

    return;
}

/* 查询通信协议模式53F10 */
uint32 CcoQueryPrtl(uint8 index)
{
    AppData_T   sendData;
    AppData_T   recvData;
    int     ret = 0;
    uint32  tmnCcoMode = 0;

    CCOUP_FMT_DEBUG("CcoQueryPrtl 查询通信协议模式流程 begin\n");

    sendData.AFN = 0x53;
    sendData.Fn = 10;
    sendData.DataUnitLen = 0;
    CcoPackFrameData(&sendData, 0);

    CCODOWN_FMT_DEBUG(index, "CcoQueryPrtl 查询通信协议模式\n");
    ret = CcoCommFunDeal(&sendData, &recvData, index);

    if (ret > 0)
    {
        if (recvData.AFN == 0x53 && recvData.Fn == 10)
        {
            if (recvData.DataUnitLen == 1)
            {
                tmnCcoMode = recvData.DataUnitBuff[0];
                g_CcoModeAck = tmnCcoMode;

                if (tmnCcoMode == g_CcoMode)
                {
                    CCODOWN_FMT_DEBUG(index, "CcoQueryPrtl 模块通信协议模式%u与终端参数%u一致!\n", tmnCcoMode, g_CcoMode);
                    return 0;
                }

                CCODOWN_FMT_DEBUG(index, "CcoQueryPrtl 模块通信协议模式%u与终端参数%u不一致!\n", tmnCcoMode, g_CcoMode);
                return 1;
            }

            g_CcoModeAck = 0;
            CCODOWN_FMT_DEBUG(index, "CcoQueryPrtl DataUnitLen=%u错误!\n", recvData.DataUnitLen);
            return 2;
        }

        g_CcoModeAck = 0;
        CCODOWN_FMT_DEBUG(index, "CcoQueryPrtl AFN=0x%02X,Fn=%u错误!\n", recvData.AFN, recvData.Fn);
        return 3;
    }

    g_CcoModeAck = 0;
    CCODOWN_FMT_DEBUG(index, "CcoQueryPrtl ret=%u!\n", ret);
    return 4;
}

/* 设置通信协议模式55F10 */
uint32 CcoSetPrtl(uint8 index)
{
    AppData_T   sendData;
    AppData_T   recvData;
    int ret = 0;

    sendData.AFN = 0x55;
    sendData.Fn = 10;
    sendData.DataUnitBuff[0] = g_CcoMode;
    sendData.DataUnitLen = 1;
    CCODOWN_FMT_DEBUG(index, "CcoSetPrtl 设置通信协议模式\n");
    ret = CcoCommFunDeal(&sendData, &recvData, index);

    if (ret > 0)
    {
        if (recvData.AFN == 0x50 && recvData.Fn == 1)
        {
            CCODOWN_FMT_DEBUG(index, "CcoSetPrtl 模块地址设置成功!\n");
            return 0;
        }

        CCODOWN_FMT_DEBUG(index, "CcoSetPrtl AFN=0x%02X,Fn=%u错误!\n", recvData.AFN, recvData.Fn);
        return 1;
    }

    CCODOWN_FMT_DEBUG(index, "CcoSetPrtl ret=%u!\n", ret);
    return 1;
}

/* 设置模块通信速率消息处理55F11 */
void CcoSetRateMsg(INFO_DATA_T *infoMsgData, CCO_AMR_T *amr)
{
    uint16  offset = 0;  //偏移位
    CCO_SET_RATE_REQ *req = &amr->req.setRateReq;
    AppData_T appData = {0};

    CCOUP_FMT_DEBUG("CcoSetRateMsg 设置模块通信速率消息 begin\n");
    /* 解析消息 */
    offset = 0;
    req->port = *(uint32 *)&infoMsgData->aPayloadData[offset];  //端口号
    offset += 4;
    req->rate = infoMsgData->aPayloadData[offset]; /* 波特率 */

    if ((req->port != PORT_F2090201) && (req->port != PORT_F2090202))
    {
        CCOUP_FMT_DEBUG("CcoSetRateMsg 端口号错误!\n");
    }

    CcoSimpleAck(infoMsgData, TRUE);
    amr->flag.setRateFlag = 1;

    appData.AFN = 0x55;
    appData.Fn = 11;
    appData.DataUnitLen = 1;
    appData.DataUnitBuff[0] = req->rate;
    CcoPackFrameData(&appData, 0);
    return;
}

/* 设置恢复默认速率时长消息处理55F12 */
void CcoSetRestoreDurationMsg(INFO_DATA_T *infoMsgData, CCO_AMR_T *amr)
{
    uint16  offset = 0;  //偏移位
    CCO_SET_RESTORE_DURATION_REQ *req = &amr->req.setRestoreDurationReq;
    AppData_T appData = {0};

    CCOUP_FMT_DEBUG("CcoSetRestoreDurationMsg 设置恢复默认速率时长消息 begin\n");
    /* 解析消息 */
    offset = 0;
    req->port = *(uint32 *)&infoMsgData->aPayloadData[offset];  //端口号
    offset += 4;
    req->duration = *(uint16 *)&infoMsgData->aPayloadData[offset];

    if ((req->port != PORT_F2090201) && (req->port != PORT_F2090202))
    {
        CCOUP_FMT_DEBUG("CcoSetRestoreDurationMsg 端口号错误!\n");
    }

    CcoSimpleAck(infoMsgData, TRUE);
    amr->flag.setRestoreDurationFlag = 1;

    appData.AFN = 0x55;
    appData.Fn = 12;
    appData.DataUnitLen = 2;
    memcpy(&appData.DataUnitBuff[0] ,&req->duration, 2);
    CcoPackFrameData(&appData, 0);
    return;
}

/* 设置最高通信速率消息处理55F13 */
void CcoSetMaxRateMsg(INFO_DATA_T *infoMsgData, CCO_AMR_T *amr)
{
    uint16  offset = 0;  //偏移位
    CCO_SET_MAX_RATE_REQ *req = &amr->req.setMaxRateReq;
    AppData_T appData = {0};

    CCOUP_FMT_DEBUG("CcoSetMaxRateMsg 设置最高通信速率消息 begin\n");
    /* 解析消息 */
    offset = 0;
    req->port = *(uint32 *)&infoMsgData->aPayloadData[offset];  //端口号
    offset += 4;
    req->rate = infoMsgData->aPayloadData[offset]; /* 波特率 */

    if ((req->port != PORT_F2090201) && (req->port != PORT_F2090202))
    {
        CCOUP_FMT_DEBUG("CcoSetMaxRateMsg 端口号错误!\n");
    }

    CcoSimpleAck(infoMsgData, TRUE);
    amr->flag.setMaxRateFlag = 1;

    appData.AFN = 0x55;
    appData.Fn = 13;
    appData.DataUnitLen = 1;
    appData.DataUnitBuff[0] = req->rate;
    CcoPackFrameData(&appData, 0);
    return;
}

/* 数据转发（国网）消息处理02F1 */
void CcoDataFwdMsg(INFO_DATA_T *infoMsgData, CCO_AMR_T *amr)
{
    uint16  offset = 0;  //偏移位
    uint8   lenOffset = 0;
    CCO_DATA_FWD_REQ *req = &amr->req.dataFwdReq;
    AppData_T appData = {0};
    uint16  unitOffset = 0;
    uint8   copyLen = 0;

    CCOUP_FMT_DEBUG("CcoDataFwdMsg 数据转发（国网）消息 begin\n");
    /* 解析消息 */
    offset = 0;
    req->port = *(uint32 *)&infoMsgData->aPayloadData[offset];  //端口号
    req->port = ntohl(req->port);
    offset += 4;
    req->type = infoMsgData->aPayloadData[offset];
    offset += 1;

    if ((req->port != PORT_F2090201) && (req->port != PORT_F2090202))
    {
        CCOUP_FMT_DEBUG("CcoDataFwdMsg 端口号错误!\n");
    }

    if (infoMsgData->nPayloadLength > offset)
    {
        req->index = infoMsgData->MsgIndex;
        req->label = infoMsgData->MsgLabel;
        req->bufLen = get_len_offset(&infoMsgData->aPayloadData[offset], &lenOffset);
        memcpy_s(req->buf, sizeof(req->buf), &infoMsgData->aPayloadData[offset + lenOffset], req->bufLen);

        if (CcoUnPackFrameData(req->buf, req->bufLen, &appData) == 0)
        {
            req->seq3762 = appData.RInfo[5]; /* 记录数据转发携带的3762报文seq */
            //CcoPackFrameData(&appData, 0);
            CcoChangeFrameSeq(&appData, req->buf, req->bufLen, &req->newSeq, 0); /* 记录本地APP实际下发的3762报文seq */
            amr->flag.dataFwdFlag = UINT_MAX; /* 转发的是3762报文 */
            CCOUP_FMT_DEBUG("CcoDataFwdMsg 数据转发的是3762报文,AFN=0x%02X,Fn=%u,seq3762=%u,newSeq=%u\n",
                            appData.AFN, appData.Fn, req->seq3762, req->newSeq);
            return;
        }
        else
        {
            CCOUP_FMT_DEBUG("CcoDataFwdMsg 数据转发的不是3762报文\n");
            memset(&appData, 0, sizeof(appData));
            appData.AFN = 0x02;
            appData.Fn = 1;
            appData.DataUnitBuff[unitOffset] = req->type;
            unitOffset += 1;
            copyLen = (req->bufLen <= 255 ? req->bufLen: 255);
            appData.DataUnitBuff[unitOffset] = copyLen;
            unitOffset += 1;
            memcpy(&appData.DataUnitBuff[unitOffset], req->buf, copyLen);
            unitOffset += copyLen;
            appData.DataUnitLen = unitOffset;
            CcoPackFrameData(&appData, 0);
            amr->flag.dataFwdFlag = 1; /* 转发的是非3762报文 */
            return;
        }
    }
    else
    {
        CCOUP_FMT_DEBUG("CcoDataFwdMsg 报文长度错误nPayloadLength=%u,offset=%u!\n", infoMsgData->nPayloadLength, offset);
    }
   
    return;
}

/* 数据透传消息处理52F1 */
void CcoDataTransMsg(INFO_DATA_T *infoMsgData, CCO_AMR_T *amr)
{
    uint16  offset = 0;  //偏移位
    uint8   lenOffset = 0;
    CCO_DATA_TRANS_REQ *req = &amr->req.dataTransReq;
    AppData_T appData = {0};
    uint16  unitOffset = 0;

    CCOUP_FMT_DEBUG("CcoDataTransMsg 数据透传消息 begin\n");
    /* 解析消息 */
    offset = 0;
    req->port = *(uint32 *)&infoMsgData->aPayloadData[offset];  //端口号
    req->port = ntohl(req->port);
    offset += 4;
    req->type = infoMsgData->aPayloadData[offset];
    offset += 1;
    memcpy(req->addr, &infoMsgData->aPayloadData[offset], CCO_ADDR_LEN);
    offset += CCO_ADDR_LEN;
    req->ctrl = infoMsgData->aPayloadData[offset];
    offset += 1;
    memcpy_r((uint8 *)&req->frameOvertime, &infoMsgData->aPayloadData[offset], 2);
    offset += 2;
    memcpy_r((uint8 *)&req->charOvertime, &infoMsgData->aPayloadData[offset], 2);
    offset += 2;

    if ((req->port != PORT_F2090201) && (req->port != PORT_F2090202))
    {
        CCOUP_FMT_DEBUG("CcoDataTransMsg 端口号%08X错误!\n", req->port);
    }

    if (infoMsgData->nPayloadLength > offset)
    {
        req->index = infoMsgData->MsgIndex;
        req->label = infoMsgData->MsgLabel;
        req->bufLen = get_len_offset(&infoMsgData->aPayloadData[offset], &lenOffset);
        memcpy_s(req->buf, sizeof(req->buf), &infoMsgData->aPayloadData[offset + lenOffset], req->bufLen);
        CCOUP_FMT_DEBUG("CcoDataTransMsg lenOffset=%u,req->bufLen=%u\n", lenOffset, req->bufLen);
        CCOUP_BUF_DEBUG(req->buf, req->bufLen, "@@透传报文@@:");

        if (CcoUnPackFrameData(req->buf, req->bufLen, &appData) == 0)
        {
            req->seq3762 = appData.RInfo[5]; /* 记录数据转发携带的3762报文seq */
            //CcoPackFrameData(&appData, 0);
            CcoChangeFrameSeq(&appData, req->buf, req->bufLen, &req->newSeq, 0); /* 记录本地APP实际下发的3762报文seq */
            amr->flag.dataTransFlag = UINT_MAX; /* 透传的是3762报文 */
            CCOUP_FMT_DEBUG("CcoDataTransMsg 数据透传的是3762报文,AFN=0x%02X,Fn=%u,seq3762=%u,newSeq=%u\n",
                            appData.AFN, appData.Fn, req->seq3762, req->newSeq);
            return;
        }
        else
        {
            CCOUP_FMT_DEBUG("CcoDataTransMsg 数据透传的不是3762报文\n");
            memset(&appData, 0, sizeof(appData));
            appData.AFN = 0x52;
            appData.Fn = 1;
            appData.DataUnitBuff[unitOffset] = req->type;
            unitOffset += 1;
            memcpy_r(&appData.DataUnitBuff[unitOffset], req->addr, CCO_ADDR_LEN);
            unitOffset += CCO_ADDR_LEN;
            appData.DataUnitBuff[unitOffset] = req->ctrl;
            unitOffset += 1;
            appData.DataUnitBuff[unitOffset] = req->frameOvertime;
            unitOffset += 1;
            appData.DataUnitBuff[unitOffset] = req->charOvertime;
            unitOffset += 1;
            req->bufLen = (req->bufLen <= sizeof(appData.DataUnitBuff) - unitOffset ? req->bufLen : sizeof(appData.DataUnitBuff) - unitOffset);
            memcpy(&appData.DataUnitBuff[unitOffset], &req->bufLen, 2);
            unitOffset += 2;
            memcpy(&appData.DataUnitBuff[unitOffset], req->buf, req->bufLen);
            unitOffset += req->bufLen;
            appData.DataUnitLen = unitOffset;
            CcoPackFrameData(&appData, 0);
            amr->flag.dataTransFlag = 1; /* 转发的是非3762报文 */
            return;
        }
    }
    else
    {
        CCOUP_FMT_DEBUG("CcoDataTransMsg 报文长度错误nPayloadLength=%u,offset=%u!\n", infoMsgData->nPayloadLength, offset);
    }

    amr->flag.dataTransFlag = 1;
    return;
}
#if 0
/* 上报从节点信息消息处理56F1 */
void CcoReportStaInfoMsg(INFO_DATA_T *infoMsgData, CCO_AMR_T *amr)
{
    uint8   payLoad[1024] = {0};
    uint16  offset = 0;
    CCO_REPORT_STA_INFO *report = &amr->report.reportStaInfo;
    uint32  i = 0;

    CCOUP_FMT_DEBUG("CcoReportStaInfoMsg 上报从节点信息消息 begin\n");
    memcpy(&payLoad[offset], &report->port, 4);
    offset += 4;

    if (offset + CCO_ADDR_LEN * report->num > sizeof(payLoad))
    {
        CCOUP_FMT_DEBUG("CcoReportStaInfoMsg 上报从节点数量%u过大!\n", report->num);
        return;
    }

    payLoad[offset] = report->num;

    for (i = 0; i < report->num; i++)
    {
        memcpy(&payLoad[offset], &report->addr[i], CCO_ADDR_LEN);
        offset += CCO_ADDR_LEN;
    }

    return;
}

/* 上报事件数据消息处理 */
void CcoReportEventMsg(INFO_DATA_T *infoMsgData, CCO_AMR_T *amr)
{
    uint8   payLoad[1024] = {0};
    uint16  offset = 0;
    CCO_REPORT_EVENT *report = &amr->report.reportEvent;

    CCOUP_FMT_DEBUG("CcoReportEventMsg 上报事件数据消息 begin\n");
    memcpy(&payLoad[offset], &report->port, 4);
    offset += 4;
    payLoad[offset] = report->type;
    offset += 1;
    memcpy(&payLoad[offset], report->addr, CCO_ADDR_LEN);
    offset += CCO_ADDR_LEN;
    offset += set_len_offset(report->bufLen, &payLoad[offset]);

    if (offset + report->bufLen > sizeof(payLoad))
    {
        CCOUP_FMT_DEBUG("CcoReportEventMsg 上报事件数据的数据帧长度%u过长\n", report->bufLen);
        return;
    }

    memcpy(&payLoad[offset], report->buf, report->bufLen);
    return;
}

/* 上报抄读内容请求消息处理 */
void CcoReportMeterReqMsg(INFO_DATA_T *infoMsgData, CCO_AMR_T *amr)
{
    uint8   payLoad[1024] = {0};
    uint16  offset = 0;
    CCO_REPORT_METER_REQ *report = &amr->report.reportMeterReq;

    CCOUP_FMT_DEBUG("CcoReportMeterReqMsg 上报抄读内容请求消息 begin\n");
    memcpy(&payLoad[offset], &report->port, 4);
    offset += 4;
    payLoad[offset] = report->type;
    offset += 1;
    memcpy(&payLoad[offset], report->addr, CCO_ADDR_LEN);
    offset += CCO_ADDR_LEN;
    memcpy(&payLoad[offset], &report->delay, 2);
    offset += 2;
    return;
}
#endif
/* 上报抄读内容响应消息处理56F3F13 */
void CcoReportMeterAck(INFO_DATA_T *infoMsgData, CCO_AMR_T *amr)
{
    uint16  offset = 0;  //偏移位
    uint8   lenOffset = 0;
    CCO_REPORT_METER_ACK *ack = &amr->ack.reportMeterAck;
    AppData_T appData = {0};
    uint16  unitOffset = 0;

    CCOUP_FMT_DEBUG("CcoReportMeterAck 上报抄读内容响应消息 begin\n");
    /* 解析消息 */
    offset = 0;
    ack->port = *(uint32 *)&infoMsgData->aPayloadData[offset];  //端口号
    ack->port = ntohl(ack->port);
    offset += 4;
    memcpy_r((uint8 *)&ack->schemeId, &infoMsgData->aPayloadData[offset], 2);
    offset += 2;
    memcpy_r((uint8 *)&ack->taskId, &infoMsgData->aPayloadData[offset], 2);
    offset += 2;
    ack->type = infoMsgData->aPayloadData[offset];
    offset += 1;
    memcpy(ack->addr, &infoMsgData->aPayloadData[offset], CCO_ADDR_LEN);
    offset += CCO_ADDR_LEN;
    ack->prtl = infoMsgData->aPayloadData[offset];
    offset += 1;
    ack->lengthType = infoMsgData->aPayloadData[offset];
    offset += 1;

    if (infoMsgData->nPayloadLength > offset)
    {
        ack->bufLen = get_len_offset(&infoMsgData->aPayloadData[offset], &lenOffset);
        memcpy_s(ack->buf, sizeof(ack->buf), &infoMsgData->aPayloadData[offset + lenOffset], ack->bufLen);
    }

    if ((ack->port != PORT_F2090201) && (ack->port != PORT_F2090202))
    {
        CCOUP_FMT_DEBUG("CcoReportMeterAck 端口号错误!\n");
    }

    amr->flag.reportMeterAckFlag = 1;

    appData.AFN = 0x56;
    appData.Fn = (ack->lengthType == 0 ? 3 : 13);
    appData.RInfo[5] = infoMsgData->MsgLabel;
    memcpy(&appData.DataUnitBuff[unitOffset], &ack->schemeId, 2);
    unitOffset += 2;
    memcpy(&appData.DataUnitBuff[unitOffset], &ack->taskId, 2);
    unitOffset += 2;
    appData.DataUnitBuff[unitOffset] = ack->type;
    unitOffset += 1;
    memcpy_r(&appData.DataUnitBuff[unitOffset], ack->addr, CCO_ADDR_LEN);
    unitOffset += CCO_ADDR_LEN;

    if (appData.Fn == 3)
    {
        appData.DataUnitBuff[unitOffset] = ack->bufLen;
        unitOffset += 1;
        memcpy(&appData.DataUnitBuff[unitOffset], ack->buf, ack->bufLen);
        unitOffset += ack->bufLen;
    }
    else
    {
        appData.DataUnitBuff[unitOffset] = ack->prtl;
        unitOffset += 1;
        unitOffset += 1; /* 1个字节保留字段 */
        memcpy(&appData.DataUnitBuff[unitOffset], &ack->bufLen, 2);
        unitOffset += 2;
        memcpy(&appData.DataUnitBuff[unitOffset], ack->buf, ack->bufLen);
        unitOffset += ack->bufLen;
    }

    appData.DataUnitLen = unitOffset;
    CcoPackFrameData(&appData, 0);
    return;
}
#if 0
/* 上报预告的通信对象数据消息处理 */
void CcoReportPreviewObjMsg(INFO_DATA_T *infoMsgData, CCO_AMR_T *amr)
{
    uint8   payLoad[1024] = {0};
    uint16  offset = 0;
    CCO_REPORT_PREVIEW_OBJ *report = &amr->report.reportPreviewObj;

    CCOUP_FMT_DEBUG("CcoReportPreviewObjMsg 上报预告的通信对象数据消息 begin\n");
    memcpy(&payLoad[offset], &report->port, 4);
    offset += 4;
    memcpy(&payLoad[offset], &report->schemeId, 2);
    offset += 2;
    memcpy(&payLoad[offset], &report->taskId, 2);
    offset += 2;
    payLoad[offset] = report->type;
    offset += 1;
    memcpy(&payLoad[offset], report->addr, CCO_ADDR_LEN);
    offset += CCO_ADDR_LEN;
    offset += set_len_offset(report->bufLen, &payLoad[offset]);

    if (offset + report->bufLen > sizeof(payLoad))
    {
        CCOUP_FMT_DEBUG("CcoReportPreviewObjMsg 上报预告的通信对象数据的数据帧长度%u过长\n", report->bufLen);
        return;
    }

    memcpy(&payLoad[offset], report->buf, report->bufLen);
    return;
}
#endif
/* 上报预告的通信对象数据响应消息处理56F4F14 */
void CcoReportPreviewObjAckMsg(INFO_DATA_T *infoMsgData, CCO_AMR_T *amr)
{
    uint16  offset = 0;  //偏移位
    CCO_REPORT_PREVIEW_OBJ_ACK *ack = &amr->ack.reportPreviewObjAck;
    uint16 Fn = 0;

    CCOUP_FMT_DEBUG("CcoReportPreviewObjAckMsg 上报预告的通信对象数据响应消息 begin\n");
    /* 解析消息 */
    offset = 0;
    ack->port = *(uint32 *)&infoMsgData->aPayloadData[offset];  //端口号
    ack->port = ntohl(ack->port);
    offset += 4;
    memcpy_r((uint8 *)&ack->schemeId, &infoMsgData->aPayloadData[offset], 2);
    offset += 2;
    memcpy_r((uint8 *)&ack->taskId, &infoMsgData->aPayloadData[offset], 2);
    offset += 2;
    ack->type = infoMsgData->aPayloadData[offset];
    offset += 1;
    memcpy(ack->addr, &infoMsgData->aPayloadData[offset], CCO_ADDR_LEN);
    offset += CCO_ADDR_LEN;
    ack->lengthType = infoMsgData->aPayloadData[offset];
    offset += 1;
    ack->status = infoMsgData->aPayloadData[offset];
    offset += 1;
    ack->errorFlag = infoMsgData->aPayloadData[offset];
    offset += 1;

    if (ack->errorFlag == 1)
    {
        ack->error = infoMsgData->aPayloadData[offset];
        offset += 1;
    }

    if ((ack->port != PORT_F2090201) && (ack->port != PORT_F2090202))
    {
        CCOUP_FMT_DEBUG("CcoReportMeterAck 端口号错误!\n");
    }

    amr->flag.reportPreviewObjAckFlag = 1;

    Fn = ack->status == 1 ? 1 : 3;
    Cco3762ConfirmAck(infoMsgData->MsgLabel, Fn, 0);
    return;
}

/* 上报广播延时消息处理 */
void CcoReportBroadDelayMsg(INFO_DATA_T *infoMsgData, CCO_AMR_T *amr)
{
    uint8   payLoad[1024] = {0};
    uint16  offset = 0;
    CCO_REPORT_BROAD_DELAY *report = &amr->report.reportBroadDelay;

    CCOUP_FMT_DEBUG("CcoReportBroadDelayMsg 上报广播延时消息 begin\n");
    memcpy(&payLoad[offset], &report->port, 4);
    offset += 4;
    memcpy(&payLoad[offset], &report->delay, 2);
    offset += 2;
    return;
}

/* 上报广播延时响应消息处理56F5 */
void CcoReportBroadDelayAckMsg(INFO_DATA_T *infoMsgData, CCO_AMR_T *amr)
{
    uint16  offset = 0;
    uint8   lenOffset = 0;
    CCO_REPORT_BROAD_DELAY_ACK *ack = &amr->ack.reportBroadDelayAck;
    AppData_T appData = {0};
    uint16  unitOffset = 0;
    uint8   copyLen = 0;

    CCOUP_FMT_DEBUG("CcoReportBroadDelayAckMsg 上报广播延时消息 begin\n");
    ack->port = *(uint32 *)&infoMsgData->aPayloadData[offset];  //端口号
    ack->port = ntohl(ack->port);
    offset += 4;

    ack->bufLen = 0;

    if (infoMsgData->nPayloadLength > offset)
    {
        ack->bufLen = get_len_offset(&infoMsgData->aPayloadData[offset], &lenOffset);
        memcpy_s(ack->buf, sizeof(ack->buf), &infoMsgData->aPayloadData[offset + lenOffset], ack->bufLen);
    }

    if ((ack->port != PORT_F2090201) && (ack->port != PORT_F2090202))
    {
        CCOUP_FMT_DEBUG("CcoReportBroadDelayAckMsg 端口号错误!\n");
    }

    amr->flag.reportBroadDelayAckFlag = 1;
    appData.AFN = 0x56;
    appData.Fn = 5;
    appData.RInfo[5] = infoMsgData->MsgLabel;
    copyLen = (ack->bufLen <= 255 ? ack->bufLen : 255);
    appData.DataUnitBuff[unitOffset] = copyLen;
    unitOffset += 1;
    memcpy(&appData.DataUnitBuff[unitOffset], ack->buf, copyLen);
    unitOffset += copyLen;
    appData.DataUnitLen = unitOffset;
    CcoPackFrameData(&appData, 0);
    return;
}

/* 上报广播完成响应消息处理56F6 */
void CcoReportBroadFinishAckMsg(INFO_DATA_T *infoMsgData, CCO_AMR_T *amr)
{
    uint32  port = 0;

    CCOUP_FMT_DEBUG("CcoReportBroadFinishAckMsg 上报广播完成消息 begin\n");
    port = *(uint32 *)&infoMsgData->aPayloadData[0];  //端口号
    port = ntohl(port);

    if ((port != PORT_F2090201) && (port != PORT_F2090202))
    {
        CCOUP_FMT_DEBUG("CcoReportBroadFinishAckMsg 端口号错误!\n");
    }

    amr->flag.reportBroadFinishAckFlag = 1;

    return;
}

uint32 g_CcoReadMode = 0; /* 抄读方式，0:旧模式，数据长度为1字节。1:新模式，数据长度为2字节 */
/* 查询抄读方式消息处理 */
void CcoQueryReadModeMsg(INFO_DATA_T *infoMsgData, CCO_AMR_T *amr)
{
    uint32  port = 0;  //端口号

    CCOUP_FMT_DEBUG("CcoQueryReadModeMsg 查询抄读方式消息 begin\n");
    port = *(uint32 *)&infoMsgData->aPayloadData[0];  //端口号
    port = ntohl(port);

    if ((port != PORT_F2090201) && (port != PORT_F2090202))
    {
        CCOUP_FMT_DEBUG("CcoQueryAddrMsg 端口号错误!\n");
    }

    CcoSimpleAckUint8(infoMsgData, g_CcoReadMode);
    return;
}

/*******************************************************************************
* 函数名称: CalWeekday
* 函数功能: 基姆拉尔森计算星期公式
* 输入参数: int y   年(如:2008)
*           int m   月
*           int d   日
* 输出参数: 无
* 返 回 值:  星期(1-7) 1-6 :星期一~六; 7:星期日
*******************************************************************************/
uint8 CalWeekday(int y, int m, int d)
{
    if (m==1 || m==2)
    {
        m += 12;
        y--;
    }

    uint8 w = (d+1 + 2*m + 3*(m+1)/5 + y + y/4 - y/100 + y/400)%7;

    return (w == 0) ? 7 : w;
}

/*******************************************************************************
* 函数名称: CcoClearCommRecvBuff
* 函数功能: 清除通讯接收数据
* 输入参数: 无
* 输出参数: 无
* 返 回 值:  无
*******************************************************************************/
void CcoClearCommRecvBuff(uint8 nArrIndex)
{
    int     RecvLen = 0xff;                     //接收数据长度
    uint8   RecvBuf[MAX_QUEUE_BUFF_LEN];        //接收数据缓冲区

    uint8   nTtlIndex = 0;  //不同的端口号对应不同的数组下标
    nTtlIndex = nArrIndex;

    while (RecvLen > 0)
    {
        RecvLen = PlcReadQueue(&g_CcoRecvDown[nTtlIndex], RecvBuf);
    }
}

/*******************************************************************************
* 函数名称:         RecvFrameCheck
* 函数功能:         下行发送数据内容与接收到得数据进行判断
* 输入参数:         AppData_T AppData        //下行发送的数据内容
                AppData_T RecvAppData    //上行发送的数据内容
* 输出参数:         NULL
* 返 回 值:          > 0 接收到正确回码
                    < 0 回码内容错误

*******************************************************************************/
extern uint8 gRebootAmrThread[MAX_DOWN_THREAD_NUM];                     //重启抄表线程标志
int CcoRecvFrameCheck(AppData_T *AppData,AppData_T *RecvAppData, uint8 nArrIndex)
{
    static int DenyContinuationTimes[MAX_DOWN_THREAD_NUM] = {0};

    PLCTASK_DETAIL  stPlcTaskDetail;       //载波任务具体信息
    memset((uint8 *)&stPlcTaskDetail, 0x00, sizeof(stPlcTaskDetail));

    uint8 nTtlIndex = 0;  //不同的端口号对应不同的数组下标
    nTtlIndex = nArrIndex;

    //一旦出现不是连续的否认帧，重置否认连续次数
    if(!(RecvAppData->AFN == 0x0 && RecvAppData->Fn == 2))
    {
        DenyContinuationTimes[nTtlIndex] = 0;
    }

    switch(AppData->AFN)//
    {
        case 0x0://确认否认
           return 1;

        //注意:0x01,0x04,0x05,0x11,0x12在1376.2中只存在确认否认帧
        //<prohibition_modify>...</prohibition_modify>两个标签之间的内容不可修改
        case 0x01://初始化
        case 0x04://链路接口检测
        case 0x05://控制命令
        case 0x11://路由设置
            if(RecvAppData->AFN == 0x0 && RecvAppData->Fn == 1)
            {
                CCODOWN_FMT_DEBUG(nTtlIndex, "收到确认帧!  \n");
                return 1;
            }
            
            if(RecvAppData->AFN == 0x0 && RecvAppData->Fn == 2)
            {
                CCODOWN_FMT_DEBUG(nTtlIndex, "收到否认帧!  \n");
                DenyContinuationTimes[nTtlIndex]++;

                if(DenyContinuationTimes[nTtlIndex] > 5)
                {
                    DenyContinuationTimes[nTtlIndex] = 0;
                    CCODOWN_FMT_DEBUG(nTtlIndex, "连续收到否认帧, 超过5次, 重启抄表线程! \n");
                    gRebootAmrThread[nTtlIndex] = 1; //抄表线程重启
                    sleep(3);             //等待线程由主进程的重启
                }

                return 1;
            }
            break;

        case 0x12://路由控制
            if(RecvAppData->AFN == 0x0 && RecvAppData->Fn == 1)
            {
                CCODOWN_FMT_DEBUG(nTtlIndex, "路由控制命令(1:重启 / 2:暂停 / 3:恢复)确认:%d \n", AppData->Fn);
                return 1;
            }

            if(RecvAppData->AFN == 0x0 && RecvAppData->Fn == 2)
            {
                //立即对模块进行重启
                CCODOWN_FMT_DEBUG(nTtlIndex, "路由控制命令(1:重启 / 2:暂停 / 3:恢复)否认:%d \n", AppData->Fn);
                return 1;
            }
            return 1;
            break;

        case 0x02://数据转发
        case 0x03://查询数据
        case 0x10://路由查询
        case 0x13://路由数据转发
        case 0x15://文件传输
            //接收完成AFN=0x03,F10,清除主动查询标志(统一在GetModuleManuID函数中检查)
            /*
            if(RecvAppData.AFN == 0x03 && RecvAppData.Fn == 10)
                CheckModuleSoftReset(0, nTtlIndex);
            */

            if(RecvAppData->AFN == AppData->AFN
                    && RecvAppData->Fn == AppData->Fn)
                return 1;
            break;

        case 0x06://主动上报
            return 1;

        case 0x14://路由数据抄读
            if(RecvAppData->AFN == AppData->AFN)
                return 1;
            break;

        case 0x52:
        case 0x53:
        {
            if (RecvAppData->AFN == AppData->AFN && RecvAppData->Fn == AppData->Fn)
            {
                return 1;
            }

            break;
        }
        case 0x51:
        case 0x55:
        {
            if (RecvAppData->AFN == 0x50)
            {
                return 1;
            }

            break;
        }

        default:
            return 1;
    }

    return -1;

}

/*******************************************************************************
* 函数名称:         CcoCommFunDeal
* 函数功能:         串口收发函数
* 输入参数:         NULL
* 输出参数:         NULL
* 返 回 值:          > 0 接收成功
                    < 0 接收失败

*******************************************************************************/
int CcoCommFunDeal(AppData_T *SendAppData,AppData_T *RecvAppData, uint8 nArrIndex)
{
    //printx("int CommFunDeal(AppData_T SendAppData,AppData_T *RecvAppData, uint32 nPortID)...begin \n");
    
    uint8                       RetryCnt = 0;                       //接收尝试次数
    int                         RecvLen;                            //接收数据长度
    uint8                       RecvBuf[MAX_QUEUE_BUFF_LEN];        //接收数据缓冲区
    static time_t               CommBeginTime[MAX_DOWN_THREAD_NUM];                      //通讯开始时间
    uint16                      SendLen = 0;
    ACTIVE_COMM_STATE_T         CommStat = ACT_COMM_IDLE;
    int                         RecvResult = 1;                     //接收结果(1:成功 / -1:失败)

    uint8   nTtlIndex = 0;  //不同的端口号对应不同的数组下标
    nTtlIndex = nArrIndex;

    while(1)
    {
        switch(CommStat)
        {
        case ACT_COMM_IDLE:
            //printx("串口收发函数[%d]CommFunDeal()...ACT_COMM_IDLE \n", nTtlIndex);
            CcoClearCommRecvBuff(nTtlIndex);                    //清接收数据
            CommBeginTime[nTtlIndex] = time(NULL);
            CommStat = ACT_COMM_SEND;               //切换到发送
            break;

        case ACT_COMM_SEND:
            //printx("串口收发函数[%d]CommFunDeal()...ACT_COMM_SEND \n", nTtlIndex);
            SendLen = CcoPackFrameData(SendAppData, nTtlIndex);
            if(SendLen > 0)
            {
               //发送AFN=0x03,F10,置主动查询标志(统一在GetModuleManuID函数中检查)
               /*
               if(SendAppData.AFN == 0x03 && SendAppData.Fn == 10)
                   CheckModuleSoftReset(1, nTtlIndex);
               */

                CommBeginTime[nTtlIndex] = time(NULL);
                CommStat = ACT_COMM_RECV;           //切换到接收
            }
            break;

        case ACT_COMM_RECV:
            //printx("串口收发函数[%d]CommFunDeal()...ACT_COMM_RECV \n", nTtlIndex);
            while (1)
            {
                RecvLen = PlcReadQueue(&g_CcoRecvDown[nTtlIndex], RecvBuf);

                if (RecvLen > 0)
                {
                    CcoUnPackFrameData(RecvBuf, RecvLen, RecvAppData);

                    /* 发送接收序号判断 */
                    if (SendAppData->RInfo[5] != RecvAppData->RInfo[5])
                    {
                        CCODOWN_FMT_DEBUG(nTtlIndex, "串口收发函数[%d]CcoCommFunDeal()发送序号(%u)接收序号(%u)不匹配\n",
                                          nTtlIndex, SendAppData->RInfo[5], RecvAppData->RInfo[5]);
                        continue;
                    }
                    
                    if (CcoRecvFrameCheck(SendAppData, RecvAppData, nTtlIndex) > 0)
                    {
                        return 1;//接收成功
                    }
                    else
                    {
                        RecvResult = -1;//接收失败
                        break;
                    }

                }
                else
                {
                    RecvResult = -1;//接收失败
                    break;
                }
            }

            if(RecvResult == -1)
            {
                if( abs(time(NULL) - CommBeginTime[nTtlIndex]) > MAX_COMM_DELAY_TIME)
                {
                    RetryCnt++;
                    CommStat = ACT_COMM_RETRY;
                }
            }

            break;

        case ACT_COMM_RETRY:
            //printx("串口收发函数[%d]CommFunDeal()...ACT_COMM_RETRY \n", nTtlIndex);
            CCODOWN_FMT_DEBUG(nTtlIndex, "串口收发函数[%d]CcoCommFunDeal()...ACT_COMM_RETRY \n", nTtlIndex);
            if(RetryCnt > 1)
            {
                CommStat = ACT_COMM_IDLE;
                return 2;
            }

            CommStat = ACT_COMM_SEND;
            break;

        default:
            break;

        }

        usleep(20000);
    }

    return 1;
}

uint8 Bcd2Hex(uint8 BcdNum)
{
    uint8 HexNum = 0;

    HexNum = ((BcdNum >> 4) & 0x0F) * 10 + (BcdNum & 0x0F);

    return (HexNum);
}
#if 0
/* 查询节点信息53F4 */
uint32 CcoQueryNodeInfo(CCO_AMR_T *amr, uint8 index)
{
    AppData_T   sendData;
    AppData_T   recvData;
    uint16  offset = 0;
    int ret = 0;
    //CCO_QUERY_NODE_INFO_ACK_53F4 *ack = NULL;
    CCO_QUERY_NODE_INFO_ACK_03F1 *ack = NULL;
    CCO_QUERY_NODE_INFO_ACK *ackUp = &amr->ack.queryNodeInfoAck;
#if 0
    sendData.AFN = 0x53;
    sendData.Fn = 4;
    offset = 0;
    sendData.DataUnitBuff[offset] = CCO_OBJ_COMM;
    offset += 1;
    //memcpy_r(&sendData.DataUnitBuff[offset], gTerminalAddr[index], CCO_ADDR_LEN);
    memset(&sendData.DataUnitBuff[offset], 0, CCO_ADDR_LEN);
    offset += 6;
#endif
    sendData.AFN = 0x03;
    sendData.Fn = 1;
    offset = 0;
    sendData.DataUnitLen = offset;
    CCODOWN_FMT_DEBUG(index, "CcoQueryNodeInfo 查询节点信息\n");
    ret = CcoCommFunDeal(&sendData, &recvData, index);

    if (ret > 0)
    {
        //if (recvData.AFN == 0x53 && recvData.Fn == 4)
        if (recvData.AFN == 0x03 && recvData.Fn == 1)
        {
            //ack = (CCO_QUERY_NODE_INFO_ACK_53F4 *)&recvData.DataUnitBuff;
            ack = (CCO_QUERY_NODE_INFO_ACK_03F1 *)&recvData.DataUnitBuff;
            memcpy(ackUp->manu, ack->manu, sizeof(ack->manu));
            memcpy(ackUp->chip, ack->chip, sizeof(ack->chip));
            ackUp->date.year = Bcd2Hex(ack->year) + 2000;
            ackUp->date.month = Bcd2Hex(ack->month);
            ackUp->date.mday = Bcd2Hex(ack->day);
            ackUp->date.wday = CalWeekday(ackUp->date.year, ackUp->date.month, ackUp->date.mday);
            ackUp->version = ack->version;
            amr->flag.queryNodeInfoAckFlag = 1;
            CCODOWN_FMT_DEBUG(index, "******CcoQueryNodeInfo manu[%02X%02X],chip[%02X%02X],date[%04u-%02u-%02u],wday[%u],version[%u]\n",
                              ackUp->manu[0], ackUp->manu[1], ackUp->chip[0], ackUp->chip[1], ackUp->date.year, 
                              ackUp->date.month, ackUp->date.mday, ackUp->date.wday, ackUp->version);
            return 0;
        }

        CCODOWN_FMT_DEBUG(index, "CcoQueryNodeInfo AFN=0x%02X,Fn=%u,seq=%u错误!\n", recvData.AFN, recvData.Fn, recvData.RInfo[5]);
        return 1;
    }

    CCODOWN_FMT_DEBUG(index, "CcoQueryNodeInfo ret=%u!\n", ret);
    return 1;
}
#endif
/* 查询节点信息53F4 */
uint32 CcoQueryNodeInfo(CCO_AMR_T *amr, uint8 index)
{
    AppData_T   sendData;
    AppData_T   recvData;
    uint16  offset = 0;
    int ret = 0;
    CCO_QUERY_NODE_INFO_ACK_53F4 *ack = NULL;
    //CCO_QUERY_NODE_INFO_ACK_03F1 *ack = NULL;
    CCO_QUERY_NODE_INFO_ACK *ackUp = &amr->ack.queryNodeInfoAck;

    sendData.AFN = 0x53;
    sendData.Fn = 4;
    offset = 0;
    sendData.DataUnitBuff[offset] = CCO_OBJ_COMM;
    offset += 1;
    //memcpy_r(&sendData.DataUnitBuff[offset], gTerminalAddr[index], CCO_ADDR_LEN);
    memset(&sendData.DataUnitBuff[offset], 0, CCO_ADDR_LEN);
    offset += 6;

#if 0
    sendData.AFN = 0x03;
    sendData.Fn = 1;
    offset = 0;
#endif
    sendData.DataUnitLen = offset;
    CCODOWN_FMT_DEBUG(index, "CcoQueryNodeInfo 查询节点信息\n");
    ret = CcoCommFunDeal(&sendData, &recvData, index);

    if (ret > 0)
    {
        if (recvData.AFN == 0x53 && recvData.Fn == 4)
        //if (recvData.AFN == 0x03 && recvData.Fn == 1)
        {
            ack = (CCO_QUERY_NODE_INFO_ACK_53F4 *)&recvData.DataUnitBuff;
            //ack = (CCO_QUERY_NODE_INFO_ACK_03F1 *)&recvData.DataUnitBuff;
            memcpy(ackUp->manu, ack->manu, sizeof(ack->manu));
            memcpy(ackUp->chip, ack->chip, sizeof(ack->chip));
            ackUp->date.year = Bcd2Hex(ack->year) + 2000;
            ackUp->date.month = Bcd2Hex(ack->month);
            ackUp->date.mday = Bcd2Hex(ack->day);
            ackUp->date.wday = CalWeekday(ackUp->date.year, ackUp->date.month, ackUp->date.mday);
            ackUp->version = ack->version;
            //amr->flag.queryNodeInfoAckFlag = 1;
            CCODOWN_FMT_DEBUG(index, "******CcoQueryNodeInfo manu[%02X%02X],chip[%02X%02X],date[%04u-%02u-%02u],wday[%u],version[%u]\n",
                              ackUp->manu[0], ackUp->manu[1], ackUp->chip[0], ackUp->chip[1], ackUp->date.year, 
                              ackUp->date.month, ackUp->date.mday, ackUp->date.wday, ackUp->version);
            return 0;
        }

        CCODOWN_FMT_DEBUG(index, "CcoQueryNodeInfo AFN=0x%02X,Fn=%u,seq=%u错误!\n", recvData.AFN, recvData.Fn, recvData.RInfo[5]);
        return 1;
    }

    CCODOWN_FMT_DEBUG(index, "CcoQueryNodeInfo ret=%u!\n", ret);
    return 1;
}


uint32 CcoQueryStaNodeInfo(CCO_AMR_T *amr, uint8 index)
{
    AppData_T   sendData;
    AppData_T   recvData;
    uint16  offset = 0;
    int ret = 0;
    CCO_QUERY_NODE_INFO_ACK_53F4 *ack = NULL;
    //CCO_QUERY_NODE_INFO_ACK_03F1 *ack = NULL;
    CCO_QUERY_NODE_INFO_ACK *ackUp = &amr->ack.queryNodeInfoAck;
    //uint8   addr[CCO_ADDR_LEN] = {0x20, 0x40, 0x22, 0x00, 0x07, 0x96};
    uint8   addr[CCO_ADDR_LEN] = {0x20, 0x40, 0x22, 0x00, 0x08, 0x08};

    sendData.AFN = 0x53;
    sendData.Fn = 4;
    offset = 0;
    sendData.DataUnitBuff[offset] = CCO_OBJ_METER;
    offset += 1;
    //memcpy(&sendData.DataUnitBuff[offset], gTerminalAddr[index], CCO_ADDR_LEN);
    //memset(&sendData.DataUnitBuff[offset], 0, CCO_ADDR_LEN);
    memcpy_r(&sendData.DataUnitBuff[offset], addr, CCO_ADDR_LEN);
    offset += 6;
#if 0
    sendData.AFN = 0x03;
    sendData.Fn = 1;
    offset = 0;
#endif
    sendData.DataUnitLen = offset;
    CCODOWN_FMT_DEBUG(index, "CcoQueryStaNodeInfo 查询从节点信息\n");
    ret = CcoCommFunDeal(&sendData, &recvData, index);

    if (ret > 0)
    {
        if (recvData.AFN == 0x53 && recvData.Fn == 4)
        //if (recvData.AFN == 0x03 && recvData.Fn == 1)
        {
            ack = (CCO_QUERY_NODE_INFO_ACK_53F4 *)&recvData.DataUnitBuff;
            //ack = (CCO_QUERY_NODE_INFO_ACK_03F1 *)&recvData.DataUnitBuff;
            memcpy(ackUp->manu, ack->manu, sizeof(ack->manu));
            memcpy(ackUp->chip, ack->chip, sizeof(ack->chip));
            ackUp->date.year = Bcd2Hex(ack->year) + 2000;
            ackUp->date.month = Bcd2Hex(ack->month);
            ackUp->date.mday = Bcd2Hex(ack->day);
            ackUp->date.wday = CalWeekday(ackUp->date.year, ackUp->date.month, ackUp->date.mday);
            ackUp->version = ack->version;
            //amr->flag.queryNodeInfoAckFlag = 1;
            CCODOWN_FMT_DEBUG(index, "******CcoQueryStaNodeInfo manu[%02X%02X],chip[%02X%02X],date[%04u-%02u-%02u],wday[%u],version[%u]\n",
                              ackUp->manu[0], ackUp->manu[1], ackUp->chip[0], ackUp->chip[1], ackUp->date.year, 
                              ackUp->date.month, ackUp->date.mday, ackUp->date.wday, ackUp->version);
            return 0;
        }

        CCODOWN_FMT_DEBUG(index, "CcoQueryStaNodeInfo AFN=0x%02X,Fn=%u,seq=%u错误!\n", recvData.AFN, recvData.Fn, recvData.RInfo[5]);
        return 1;
    }

    CCODOWN_FMT_DEBUG(index, "CcoQueryStaNodeInfo ret=%u!\n", ret);
    return 1;
}

/* 查询模块地址，并比较53F2 */
uint32 CcoQueryAddr(uint8 index)
{
    AppData_T   sendData;
    AppData_T   recvData;
    uint8   addr[CCO_ADDR_LEN] = {0};
    char    addrStr[32] = {0};
    char    tmnAddrStr[32] = {0};
    int ret = 0;

    sendData.AFN = 0x53;
    sendData.Fn = 2;
    sendData.DataUnitLen = 0;
    CCODOWN_FMT_DEBUG(index, "CcoQueryAddr 查询模块地址\n");
    ret = CcoCommFunDeal(&sendData, &recvData, index);

    if (ret > 0)
    {
        if (recvData.AFN == 0x53 && recvData.Fn == 2)
        {
            if (recvData.DataUnitLen == CCO_ADDR_LEN)
            {
                memcpy_r(addr, recvData.DataUnitBuff, CCO_ADDR_LEN);

                sprintf(addrStr, "%02x %02x %02x %02x %02x %02x", addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);
                sprintf(tmnAddrStr, "%02x %02x %02x %02x %02x %02x", gTerminalAddr[index][0], gTerminalAddr[index][1],
                    gTerminalAddr[index][2], gTerminalAddr[index][3], gTerminalAddr[index][4], gTerminalAddr[index][5]);

                if (memcmp(addr, gTerminalAddr[index], CCO_ADDR_LEN) == 0)
                {
                    CCODOWN_FMT_DEBUG(index, "CcoQueryAddr 模块地址%s与终端参数%s一致!\n", addrStr, tmnAddrStr);
                    return 0;
                }

                CCODOWN_FMT_DEBUG(index, "CcoQueryAddr 模块地址%s与终端参数%s不一致!\n", addrStr, tmnAddrStr);
                return 1;
            }
        }

        CCODOWN_FMT_DEBUG(index, "CcoQueryAddr AFN=0x%02X,Fn=%u错误!\n", recvData.AFN, recvData.Fn);
        return 1;
    }

    CCODOWN_FMT_DEBUG(index, "CcoQueryAddr ret=%u!\n", ret);
    return 1;
}

/* 模块地址设置55F1 */
uint32 CcoSetAddr(uint8 index)
{
    AppData_T   sendData;
    AppData_T   recvData;
    int ret = 0;

    sendData.AFN = 0x55;
    sendData.Fn = 1;
    memcpy_r(sendData.DataUnitBuff, gTerminalAddr[index], CCO_ADDR_LEN);
    sendData.DataUnitLen = CCO_ADDR_LEN;
    CCODOWN_FMT_DEBUG(index, "CcoSetAddr 模块地址设置\n");
    ret = CcoCommFunDeal(&sendData, &recvData, index);

    if (ret > 0)
    {
        if (recvData.AFN == 0x50 && recvData.Fn == 1)
        {
            CCODOWN_FMT_DEBUG(index, "CcoSetAddr 模块地址设置成功!\n");
            return 0;
        }

        CCODOWN_FMT_DEBUG(index, "CcoSetAddr AFN=0x%02X,Fn=%u错误!\n", recvData.AFN, recvData.Fn);
        return 1;
    }

    CCODOWN_FMT_DEBUG(index, "CcoSetAddr ret=%u!\n", ret);
    return 1;
}

/* 查询本地模块参数53F1 */
uint32 CcoQueryPara(CCO_AMR_T *amr, uint8 index)
{
    AppData_T   sendData;
    AppData_T   recvData;
    uint16  taskObjNum = 0;     /* 每轮次支持的总预告任务对象数量 */
    uint8   meterObjNum = 0;    /* 每次预告抄读对象数量 */
    uint16  taskObjNum2 = 0;     /* 每轮次支持的总预告任务对象数量 */
    uint8   meterObjNum2 = 0;    /* 每次预告抄读对象数量 */
    int ret = 0;
    CCO_QUERY_PARA_ACK *ack = &amr->ack.queryParaAck;

    sendData.AFN = 0x53;
    sendData.Fn = 1;
    sendData.DataUnitLen = 0;
    CCODOWN_FMT_DEBUG(index, "CcoQueryPara 查询本地模块参数\n");
    ret = CcoCommFunDeal(&sendData, &recvData, index);

    if (ret > 0)
    {
        if (recvData.AFN == 0x53 && recvData.Fn == 1)
        {
            if (recvData.DataUnitLen == 3)
            {
                taskObjNum = *(uint16 *)&recvData.DataUnitBuff[0];
                meterObjNum = recvData.DataUnitBuff[2];
                CCODOWN_FMT_DEBUG(index, "CcoQueryPara第1次查询 taskObjNum=%u,meterObjNum=%u\n", taskObjNum, meterObjNum);
            }
            else
            {
                CCODOWN_FMT_DEBUG(index, "CcoQueryPara第1次查询 数据单元长度%u错误!\n", recvData.DataUnitLen);
                return 1;
            }
        }
        else
        {
            CCODOWN_FMT_DEBUG(index, "CcoQueryPara第1次查询 AFN=0x%02X,Fn=%u错误!\n", recvData.AFN, recvData.Fn);
            return 1;
        }
    }
    else
    {
        CCODOWN_FMT_DEBUG(index, "CcoQueryPara第1次查询 CcoCommFunDeal ret=%d!\n", ret);
        return 1;
    }

    /* 检查两次读取参数是否一致 */
    ret = CcoCommFunDeal(&sendData, &recvData, index);

    if (ret > 0)
    {
        if (recvData.AFN == 0x53 && recvData.Fn == 1)
        {
            if (recvData.DataUnitLen == 3)
            {
                taskObjNum2 = *(uint16 *)&recvData.DataUnitBuff[0];
                meterObjNum2 = recvData.DataUnitBuff[2];
                CCODOWN_FMT_DEBUG(index, "CcoQueryPara第2次查询 taskObjNum=%u,meterObjNum=%u\n", taskObjNum2, meterObjNum2);

                if (taskObjNum == taskObjNum2 && meterObjNum == meterObjNum2)
                {
                    ack->taskObjNum = taskObjNum;
                    ack->meterObjNum = meterObjNum;
                    amr->flag.queryParaAckFlag = 1;
                    CCODOWN_FMT_DEBUG(index, "CcoQueryPara2次查询 参数一致!\n");
                    return 0;
                }
                else
                {
                    CCODOWN_FMT_DEBUG(index, "CcoQueryPara2次查询 参数不一致!\n");
                    return 1;
                }
            }
            else
            {
                CCODOWN_FMT_DEBUG(index, "CcoQueryPara第2次查询 数据单元长度%u错误!\n", recvData.DataUnitLen);
                return 1;
            }
        }
        else
        {
            CCODOWN_FMT_DEBUG(index, "CcoQueryPara第2次查询 AFN=0x%02X,Fn=%u错误!\n", recvData.AFN, recvData.Fn);
            return 1;
        }
    }
    else
    {
        CCODOWN_FMT_DEBUG(index, "CcoQueryPara第2次查询 CcoCommFunDeal ret=%d!\n", ret);
        return 1;
    }
}

/* 查询模块串口参数53F6 */
uint32 CcoQueryUartPara(CCO_AMR_T *amr, uint8 index)
{
    AppData_T   sendData;
    AppData_T   recvData;
    CCO_QUERY_UART_PARA_ACK *ack = NULL;
    CCO_QUERY_UART_PARA_ACK *ackUp = &amr->ack.queryUartParaAck;
    int ret = 0;

    sendData.AFN = 0x53;
    sendData.Fn = 6;
    sendData.DataUnitLen = 0;
    CCODOWN_FMT_DEBUG(index, "CcoQueryUartPara 查询模块串口参数\n");
    ret = CcoCommFunDeal(&sendData, &recvData, index);

    if (ret > 0)
    {
        if (recvData.AFN == 0x53 && recvData.Fn == 6)
        {
            ack = (CCO_QUERY_UART_PARA_ACK *)&recvData.DataUnitBuff[0];
            *ackUp = *ack;
            amr->flag.queryUartParaAckFlag = 1;
            CCODOWN_FMT_DEBUG(index, "CcoQueryUartPara 查询模块串口参数成功!当前速率(%u),最大速率(%u)\n",
                              ackUp->currentRate, ackUp->maxRate);
            return 0;
        }

        CCODOWN_FMT_DEBUG(index, "CcoQueryUartPara AFN=0x%02X,Fn=%u,seq=%u错误!\n", recvData.AFN, recvData.Fn, recvData.RInfo[5]);
        return 1;
    }

    CCODOWN_FMT_DEBUG(index, "CcoQueryUartPara ret=%u!\n", ret);
    return 1;
}

CCO_COMM_RATE_E g_CcoCommRateE = CCO_COMM_RATE_9600;
const uint32 g_CcoCommRate[CCO_COMM_RATE_MAX] = {300, 600, 1200, 2400, 4800, 7200, 9600, 19200, 38400, 57600, 115200};
/* 设置模块通信速率55F11 */
uint32 CcoSetRate(CCO_COMM_RATE_E rate, uint8 index)
{
    AppData_T   sendData;
    AppData_T   recvData;

    int ret = 0;

    sendData.AFN = 0x55;
    sendData.Fn = 11;
    sendData.DataUnitBuff[0] = rate;
    sendData.DataUnitLen = 1;
    CCODOWN_FMT_DEBUG(index, "CcoSetRate 设置模块通信速率\n");
    ret = CcoCommFunDeal(&sendData, &recvData, index);

    if (ret > 0)
    {
        if (recvData.AFN == 0x50 && recvData.Fn == 1)
        {
            CCODOWN_FMT_DEBUG(index, "CcoSetRate 模块通信速率设置成功!\n");
            return 0;
        }

        CCODOWN_FMT_DEBUG(index, "CcoSetRate AFN=0x%02X,Fn=%u错误!\n", recvData.AFN, recvData.Fn);
        return 1;
    }

    CCODOWN_FMT_DEBUG(index, "CcoSetRate ret=%u!\n", ret);
    return 1;
}

/* 速度协商处理函数 */
uint32 CcoRateNegotiateProc(CCO_AMR_T *amr, uint8 index)
{
    uint32  cnt = 0;
    CCO_COMM_RATE_E setRate = CCO_COMM_RATE_115200;
    int ret = 0;

    if (amr->flag.queryUartParaAckFlag == 1 && amr->ack.queryUartParaAck.maxRate < CCO_COMM_RATE_MAX)
    {
        setRate = amr->ack.queryUartParaAck.maxRate;
    }

    while (1)
    {
        while (1)
        {
            /* 通信速率设置【AFN=55H:F11】 */
            if (CcoSetRate(setRate, index) == 0)
            {
                CCODOWN_FMT_DEBUG(index, "CcoRateNegotiateProc 通信速率协商成功%u bps(%u)!\n", g_CcoCommRate[setRate], setRate);

                ret = gDevHalInfo[index].dev->uart_param_set(gDevHalInfo[index].dev, g_CcoCommRate[setRate], 8, 1, PARITY_EVEN);  //TRUE:1, FALSE:0
                CCODOWN_FMT_DEBUG(index, "串口[%d] dev->uart_param_set() ret=%d \n", index, ret);

                if (ret == 0)
                {
                    CCODOWN_FMT_DEBUG(index, "串口[%d]打开成功%u bps CcoRateNegotiateProc Success \n", index, g_CcoCommRate[setRate]);
                    sleep(5);
                    return 0;
                }
                else
                {
                    CCODOWN_FMT_DEBUG(index, "串口[%d]打开失败CcoRateNegotiateProc Error \n", index);
                    sleep(5);
                    break;
                }
            }

            cnt++;

            if (cnt >= CCO_MAX_ATTEMPT_CNT)
            {
                CCODOWN_FMT_DEBUG(index, "CcoRateNegotiateProc 通信速率协商达到最大重试次数%u!\n", CCO_MAX_ATTEMPT_CNT);
                break;
            }
        }

        if (setRate != CCO_COMM_RATE_9600)
        {
            setRate = CCO_COMM_RATE_9600;
        }
        else
        {
            CCODOWN_FMT_DEBUG(index, "CcoRateNegotiateProc 所有通信速率设置均失败!\n");
            break; /* 所有通信速率设置均失败 */
        }
    }

    return 1;
}

/* 模式协商处理函数 */
uint32 CcoModeNegotiateProc(uint8 index)
{
    uint32  cnt = 0;
    uint32  ret = 0;

    while (1)
    {
        /* 查询通信协议模式（AFN=53H:F10） */
        ret = CcoQueryPrtl(index);

        if (ret == 1)
        {
            /* 设置通信协议模式（AFN=55H:F10） */
            CcoSetPrtl(index);

            if (CcoQueryPrtl(index) == 0)
            {
                break;
            }
        }
        else if (ret == 0)
        {
            break;
        }

        cnt++;

        if (cnt >= 1)
        {
            CCODOWN_FMT_DEBUG(index, "CcoModeNegotiateProc 通信协议模式协商达到最大重试次数%u!\n", 1);
            return 1;
        }
    }

    return 0;
}

/* 福建初始化流程处理函数 */
uint32 CcoInitProc(CCO_AMR_T *amr)
{
    uint8   index = 0;
    uint32  cnt = 0;
    CCO_COMM_RATE_E setRate = CCO_COMM_RATE_115200;
    int ret = 0;
    
    PlcResetRouter(index);

    while (1)
    {
        if (CcoModeNegotiateProc(index) == 0)
        {
            CCODOWN_FMT_DEBUG(index, "CcoInitProc 通信协议模式协商成功!\n");
            break;
        }

        ret = gDevHalInfo[index].dev->uart_param_set(gDevHalInfo[index].dev, g_CcoCommRate[setRate], 8, 1, PARITY_EVEN);  //TRUE:1, FALSE:0
        CCODOWN_FMT_DEBUG(index, "串口[%d] dev->uart_param_set() ret=%d \n", index, ret);

        if (ret == 0)
        {
            CCODOWN_FMT_DEBUG(index, "串口[%d]打开成功%u bps CcoInitProc Success \n", index, g_CcoCommRate[setRate]);
            sleep(5);
        }
        else
        {
            CCODOWN_FMT_DEBUG(index, "串口[%d]打开失败CcoInitProc Error \n", index);
            sleep(5);
        }

        if (setRate > CCO_COMM_RATE_300)
        {
            setRate--;
        }
        else
        {
            return 1; /* 所有通信速率均失败 */
        }
    }

    setRate = CCO_COMM_RATE_115200;
    cnt = 0;
    /* 读取通信模块的版本信息【AFN=53H:F4】 */
    //amr->flag.queryNodeInfoAckFlag = 0;
    while (1)
    {
        if (CcoQueryNodeInfo(amr, index) == 0)
        {
            break;
        }

        ret = gDevHalInfo[index].dev->uart_param_set(gDevHalInfo[index].dev, g_CcoCommRate[setRate], 8, 1, PARITY_EVEN);  //TRUE:1, FALSE:0
        CCODOWN_FMT_DEBUG(index, "串口[%d] dev->uart_param_set() ret=%d \n", index, ret);

        if (ret == 0)
        {
            CCODOWN_FMT_DEBUG(index, "串口[%d]打开成功%u bps CcoInitProc Success \n", index, g_CcoCommRate[setRate]);
            sleep(5);
        }
        else
        {
            CCODOWN_FMT_DEBUG(index, "串口[%d]打开失败CcoInitProc Error \n", index);
            sleep(5);
        }

        if (setRate > CCO_COMM_RATE_300)
        {
            setRate--;
        }
        else
        {
            return 1; /* 所有通信速率均失败 */
        }
    }

    while (1)
    {
        /* 查询模块主节点地址（AFN=53H:F2） */
        if (CcoQueryAddr(index) != 0)
        {
            /* 设置模块主节点地址（AFN=55H:F1） */
            CcoSetAddr(index);
        }
        else
        {
            break;
        }

        cnt++;

        if (cnt >= CCO_MAX_ATTEMPT_CNT)
        {
            CCODOWN_FMT_DEBUG(index, "CcoInitProc1达到最大重试次数%u!\n", CCO_MAX_ATTEMPT_CNT);
            return 1;
        }
    }

    cnt = 0;
    amr->flag.queryParaAckFlag = 0;

    while (1)
    {
        /* 查询本地模块参数配置（AFN=53H:F1） */
        if (CcoQueryPara(amr, index) == 0)
        {
            break;
        }

        cnt++;

        if (cnt >= CCO_MAX_ATTEMPT_CNT)
        {
            CCODOWN_FMT_DEBUG(index, "CcoInitProc2达到最大重试次数%u!\n", CCO_MAX_ATTEMPT_CNT);
            return 1;
        }
    }
    /* 读取AFN=53H:F6(本地通信模块当前通信参数) */
    amr->flag.queryUartParaAckFlag = 0;

    if (CcoQueryUartPara(amr, index) == 0)
    {
        /* 通信速率协商【AFN=55H:F11】 */
        //CcoSetRate(g_CcoCommRateE, index);
        if (CcoRateNegotiateProc(amr, index) != 0)
        {
            CCODOWN_FMT_DEBUG(index, "CcoInitProc 通信速率协商失败!\n");
            return 1;
        }

        g_CcoReadMode = 1; /* 新模式 */
        CCODOWN_FMT_DEBUG(index, "CcoInitProc 初始化成功!新模式!\n");
    }
    else
    {
        g_CcoReadMode = 0; /* 旧模式 */
        CCODOWN_FMT_DEBUG(index, "CcoInitProc 初始化成功!旧模式!\n");
    }

    return 0;
}
extern uint8 gPrtlLinkLen; //规约链路长度，默认2字节
/* 376.2快速解码 */
uint32 CcoQuickDecode(uint8 *frameBuf, uint16 frameLen, CCO_QUICK_DECODE_T *data, uint8 index)
{
    //CCODOWN_FMT_DEBUG(0, "UnPackFrameData()...start...FrameLen=%d \n", FrameLen);
    
    uint16  frameDataLen=0;                     //帧数据总长度
    uint16  DataIndex = 0;                      //指针偏移
    int     i = 0;
    //uint8   CheckSum = 0;  //校验和

    //CCODOWN_FMT_DEBUG(0, "UnPackFrameData()... 111 \n");

    if (gPrtlLinkLen == 1)
    {
        //计算帧数据长度, 偏移到信息域R
        frameDataLen= (uint8)frameBuf[1];
        DataIndex = 3;
    }
    else
    {
        //计算帧数据长度, 偏移到信息域R
        memcpy(&frameDataLen, &frameBuf[1], 2);
        DataIndex = 4;
    }

    //CCODOWN_FMT_DEBUG(0, "UnPackFrameData()... 222 \n");

    //判断报文长度是否合法
    if (frameDataLen > frameLen)
    {
        //报文长度大于buff长度, 判断该帧报文不完整
        CCODOWN_FMT_DEBUG(index, "UnPackFrameData()...if(FrameDataLen > FrameLen) 该帧报文不完整! \n");
        return 1;
    }
    
    //CCODOWN_FMT_DEBUG(0, "UnPackFrameData()... 333 \n");

    //解析信息域, 偏移到地址域
    memcpy(data->RInfo, &frameBuf[DataIndex], 6);
    DataIndex += 6;

    //CCODOWN_FMT_DEBUG(0, "UnPackFrameData()... 444 \n");

    //判断通讯模块标识，偏移到应用数据域
    if (data->RInfo[0]& 0x04)  //存在地址域
    {
        //偏移掉A1和A3，从节点上行无A2
        DataIndex += 12;
    }
    
    //CCODOWN_FMT_DEBUG(0, "UnPackFrameData()... 555 \n");

    data->AFN = frameBuf[DataIndex];
    for (i = 0; i < 8; i++)
    {
        if (frameBuf[DataIndex+1] & (0x01 << i))
        {
            data->Fn = frameBuf[DataIndex+2] * 8 + (i + 1);
            break;
        }
    }

    return 0;
}

uint32 CcoUnPackFrameData(uint8 *FrameBuff, uint16 FrameLen, AppData_T *AppData)
{
    //CCODOWN_FMT_DEBUG(0, "UnPackFrameData()...start...FrameLen=%d \n", FrameLen);
    
    uint16  FrameDataLen=0;                     //帧数据总长度
    uint16  DataIndex = 0;                      //指针偏移
    int     i = 0;
    uint8   CheckSum = 0;  //校验和

    //CCODOWN_FMT_DEBUG(0, "UnPackFrameData()... 111 \n");

    if (gPrtlLinkLen == 1)
    {
        //计算帧数据长度, 偏移到信息域R
        FrameDataLen= (uint8)FrameBuff[1];
        DataIndex = 3;
    }
    else
    {
        //计算帧数据长度, 偏移到信息域R
        memcpy(&FrameDataLen, &FrameBuff[1], 2);
        DataIndex = 4;
    }

    //CCODOWN_FMT_DEBUG(0, "UnPackFrameData()... 222 \n");

    //判断报文长度是否合法
    if(FrameDataLen > FrameLen)
    {
        //报文长度大于buff长度, 判断该帧报文不完整
        CCODOWN_FMT_DEBUG(0, "UnPackFrameData()...if(FrameDataLen > FrameLen) 该帧报文不完整! \n");
        return 1;
    }
    
    //CCODOWN_FMT_DEBUG(0, "UnPackFrameData()... 333 \n");

    //解析信息域, 偏移到地址域
    memcpy(AppData->RInfo, &FrameBuff[DataIndex], 6);
    DataIndex += 6;

    //CCODOWN_FMT_DEBUG(0, "UnPackFrameData()... 444 \n");

    //判断通讯模块标识，偏移到应用数据域
    if (AppData->RInfo[0]& 0x04)  //存在地址域
    {
        //偏移掉A1和A3，从节点上行无A2
        DataIndex += 12;
    }
    
    //CCODOWN_FMT_DEBUG(0, "UnPackFrameData()... 555 \n");

    AppData->AFN = FrameBuff[DataIndex];
    for (i = 0; i < 8; i++)
    {
        if (FrameBuff[DataIndex+1] & (0x01 << i))
        {
            AppData->Fn = FrameBuff[DataIndex+2] * 8 + (i + 1);
            break;
        }
    }
    DataIndex += 3;
    
    //CCODOWN_FMT_DEBUG(0, "UnPackFrameData()... 666 \n");

    if(FrameDataLen < (DataIndex + 2))
    {
        CCODOWN_FMT_DEBUG(0, "UnPackFrameData()...if(FrameDataLen < (DataIndex + 2)) 错误! FrameDataLen:%d, DataIndex:%d \n", FrameDataLen, DataIndex);
        return 1;
    }
    AppData->DataUnitLen = FrameDataLen - (DataIndex + 2);

    //CCODOWN_FMT_DEBUG(0, "UnPackFrameData()... 777 \n");
    
    if(AppData->DataUnitLen > sizeof(AppData->DataUnitBuff))
    {
        CCODOWN_FMT_DEBUG(0, "UnPackFrameData()...if(AppData->DataUnitLen > sizeof(AppData->DataUnitBuff)) 错误! AppData->DataUnitLen:%d \n", AppData->DataUnitLen);
        return 1;
    }
    memcpy(AppData->DataUnitBuff, &FrameBuff[DataIndex], AppData->DataUnitLen);
    
    //CCODOWN_FMT_DEBUG(0, "UnPackFrameData()... 888 \n");

    //校验和检查
    if (gPrtlLinkLen == 1)
    {
        for(i=2; i<FrameDataLen-2; i++)
        {
            CheckSum += FrameBuff[i];
        }
    }
    else
    {
        for(i=3; i<FrameDataLen-2; i++)
        {
            CheckSum += FrameBuff[i];
        }
    }
    if(CheckSum != FrameBuff[FrameDataLen-2])
    {
        CCODOWN_FMT_DEBUG(0, "UnPackFrameData()...if(CheckSum != FrameBuff[FrameDataLen-2]) 校验和错误! \n");
        return 1;
    }
    
    //CCODOWN_FMT_DEBUG(0, "UnPackFrameData()... 999 \n");

    //帧尾检查
    if(FrameBuff[FrameDataLen-1] != 0x16)
    {
        CCODOWN_FMT_DEBUG(0, "UnPackFrameData()...if(FrameBuff[FrameDataLen-1] != 0x16) 帧尾错误! \n");
        return 1;
    }
    
    return 0;
}

/*******************************************************************************
* 函数名称: GetCtrlCode
* 函数功能: 获取控制码
* 输入参数: AppData             应用层数据
* 输出参数: DesData             控制域数据
* 返 回 值: 无
*******************************************************************************/
void CcoGetCtrlCode(uint8 *DesData, AppData_T *appData, uint8 nArrIndex)
{
    CTRLCODE_BITS_T CtrlCode;
    memset(&CtrlCode,0x0,sizeof(CtrlCode));

    uint8 nTtlIndex = 0;  //不同的端口号对应不同的数组下标
    nTtlIndex = nArrIndex;

    switch(gLocalCommModuleInfo[nTtlIndex].LocalCommPatternWord.b1.BitSect.CommPattern)
    {
    case 1://窄带
        CtrlCode.COMM_TYPE = 1;
        break;

    case 2://宽带
        CtrlCode.COMM_TYPE = 3;
        break;

    case 3://小无线
        CtrlCode.COMM_TYPE = 10;
        break;

    default:
        CtrlCode.COMM_TYPE = 1;
        break;
    }

    //传输方向位
    CtrlCode.DIR = 0;

    switch (appData->AFN)
    {
    case 0x00:                                  //确认/否认
    case 0x14:                                  //路由数据抄读
        CtrlCode.PRM = 0;
        break;

    case 0x01:                                  //初始化
        CtrlCode.PRM = 1;
        break;

    default:
        CtrlCode.PRM = 1;
        break;
    }

    memcpy(DesData,&CtrlCode,1);

}

/*******************************************************************************
* 函数名称: GetInfoZoneData
* 函数功能: 获取信息域数据
* 输入参数: AppData             应用层数据
* 输出参数: DesData             信息域数据
* 返 回 值: 无
*******************************************************************************/
void CcoGetInfoZoneData(uint8 *DesData, AppData_T *appData, uint8 nArrIndex)
{
    memset(DesData, 0x00, 6);

    uint8 nTtlIndex = 0;  //不同的端口号对应不同的数组下标
    nTtlIndex = nArrIndex;

    //路由标识
    if(gLocalCommModuleInfo[nTtlIndex].LocalCommPatternWord.b1.BitSect.RouterMangerPattern == 0)//带有路由管理方式
        DesData[0] |= 0x01;

    switch (appData->AFN)
    {
        case 0x00://确认/否认
            //从模块上报帧中获取帧序列号
            DesData[5] = appData->RInfo[5];
            DesData[1] = appData->RInfo[1] & 0x0F;//信道标识
            break;

        case 0x01://初始化
        case 0x03://查询数据
        case 0x04://链路接口检测
        case 0x05://控制命令
        case 0x10://路由查询
        case 0x11://路由设置
        case 0x12://路由控制
        case 0x15://文件传输
            DesData[5] = g_InerSeq[nTtlIndex]++;//报文序列号
            break;


        case 0x02://数据转发
        case 0x13://路由数据转发
            DesData[0] |= 0x04;
            DesData[5] = g_InerSeq[nTtlIndex]++;//报文序列号
            DesData[2] = g_MetRetLen+16;
            g_MetRetLen = 60;
            break;

        case 0x14: //请求数据抄读
            if (appData->ModuleID == 1)
            {
                DesData[0] |= 0x04;
            }
            DesData[1] = gChannelID;
            DesData[2] = g_MetRetLen+16;
            g_MetRetLen = 60;
            DesData[5] = appData->RInfo[5];
        break;

        case 0xF1:
            if (appData->ModuleID == 1)
            {
                DesData[0] |= 0x04;
            }
            DesData[5] = g_InerSeq[nTtlIndex]++;//报文序列号
            break;
        case 0x50: /* 确认/否认 */
        {
            DesData[5] = appData->RInfo[5]; /* 从模块上报帧中获取帧序列号 */
            break;
        }
        case 0x51: /* 初始化 */
        case 0x52: /* 数据转发 */
        case 0x53: /* 查询数据 */
        case 0x55: /* 控制命令 */
        {
            DesData[5] = g_InerSeq[nTtlIndex]++;//报文序列号
            break;
        }
        case 0x56: /* 主动上报 */
        {
            DesData[5] = appData->RInfo[5]; /* 从模块上报帧中获取帧序列号 */
            break;
        }

     default:
        break;
    }

    //printf("帧序列号  : %d \n",DesData[5]);
}

/*******************************************************************************
* 函数名称: GetAddrZoneData
* 函数功能: 获取地址域数据
* 输入参数: AppData             应用层数据
* 输出参数: DesData             地址域数据
* 返 回 值: >0                  有地址域
*           <0                  无地址域
*******************************************************************************/
int CcoGetAddrZoneData(uint8 *DesData, AppData_T *appData, uint8 nArrIndex)
{
    switch (appData->AFN)
    {
    case 0x02:                                  //数据转发
    case 0x13:                                  //路由数据转发
        GetAddrZoneA1(DesData, nArrIndex);
        memcpy(&DesData[6], appData->DesAddr, 6);
        return 1;

    case 0x14:                                  //路由请求抄读
        if (appData->Fn == 0x03)
        {
            printx("14 f3没有地址域\n");
            return -1;
        }
        if (appData->ModuleID == 1)
        {
            GetAddrZoneA1(DesData, nArrIndex);
            memcpy(&DesData[6], appData->DesAddr, 6);
            return 1;
        }
        break;

    case 0xF1:
        if (appData->ModuleID == 1)
        {
            GetAddrZoneA1(DesData, nArrIndex);
            memcpy(&DesData[6], appData->DesAddr, 6);
            return 1;
        }
        break;

    default:
        return -1;
    }

    return -1;
}

/*******************************************************************************
* 函数名称: PackFrameData
* 函数功能: 组376.2帧并写到发送缓冲区
* 输入参数: AppData             应用层数据
* 输出参数: 无
* 返 回 值: 数据帧长度
*******************************************************************************/
uint16 CcoPackFrameData(AppData_T *appData, uint8 index)
{
    uint8   FrameBuff[MAX_QUEUE_BUFF_LEN];      //帧数据缓冲区
    uint16  DataIndex = 0;
    int     i;
    uint8   CheckSum = 0;                       //校验和
    uint8   seq = 0;
    //printx("组帧\n");
    //PrintBuf(&AppData,30);
    
    //帧头
    FrameBuff[0] = 0x68;

    if (gPrtlLinkLen == 1)
    {
        //跳过帧长度
        DataIndex = 2;
    }
    else
    {
        //跳过帧长度
        DataIndex = 3;
    }

    //控制码
    CcoGetCtrlCode(&FrameBuff[DataIndex], appData, index);
    DataIndex++;

    //信息域
    CcoGetInfoZoneData(&FrameBuff[DataIndex], appData, index);
    seq = FrameBuff[DataIndex + 5];
    appData->RInfo[5] = seq;
    DataIndex += 6;

    //地址域
    if (CcoGetAddrZoneData(&FrameBuff[DataIndex], appData, index) > 0)
    {
        DataIndex += 12;  //没有中继地址？如何判断？
    }

    //应用数据域
    FrameBuff[DataIndex] = appData->AFN;
    GetDtInfo(&FrameBuff[DataIndex+1], appData->Fn);
    DataIndex += 3;

    if (appData->DataUnitLen > 0)
    {
        memcpy(&FrameBuff[DataIndex], appData->DataUnitBuff, appData->DataUnitLen);
        DataIndex += appData->DataUnitLen;
    }

    //检验和的范围为控制域+用户数据区
    for (i = (gPrtlLinkLen+1); i < DataIndex; i++)
    {
        CheckSum += FrameBuff[i];
    }
    FrameBuff[DataIndex] = CheckSum;
    DataIndex++;

    //帧尾
    FrameBuff[DataIndex] = 0x16;
    DataIndex++;

    if (gPrtlLinkLen == 1)
    {
        //回填长度L
        FrameBuff[1] = (uint8)DataIndex;
    }
    else
    {
        //回填长度L
        memcpy(&FrameBuff[1],&DataIndex,2);
    }

    CCODOWN_FMT_DEBUG(index, "@@@@@@CcoPackFrameData send AFN=0x%02X,Fn=%u,seq=%u\n", appData->AFN, appData->Fn, seq);
    //将数据发送到共享缓冲区
    PlcWriteQueue(pAmrSendDown[index], FrameBuff, DataIndex);

    return DataIndex;
}

/* 修改seq */
void CcoChangeSeq(uint8 *seq, AppData_T *appData, uint8 index)
{
    switch (appData->AFN)
    {
        case 0x01://初始化
        case 0x03://查询数据
        case 0x04://链路接口检测
        case 0x05://控制命令
        case 0x10://路由查询
        case 0x11://路由设置
        case 0x12://路由控制
        case 0x15://文件传输
            *seq = g_InerSeq[index]++;//报文序列号
            break;

        case 0x02://数据转发
        case 0x13://路由数据转发
            *seq = g_InerSeq[index]++;//报文序列号
            break;
        case 0xF1:
            *seq = g_InerSeq[index]++;//报文序列号
            break;
        case 0x51: /* 初始化 */
        case 0x52: /* 数据转发 */
        case 0x53: /* 查询数据 */
        case 0x55: /* 控制命令 */
        {
            *seq = g_InerSeq[index]++;//报文序列号
            break;
        }
         default:
            break;
    }

    //CCODOWN_FMT_DEBUG(index, "帧序列号  : %u\n", *seq);
}

/* 返回重组后报文长度 */
uint16 CcoChangeFrameSeq(AppData_T *appData, uint8 *inBuf, uint16 inBufLen, uint8 *seq, uint8 index)
{
    uint8   FrameBuff[MAX_QUEUE_BUFF_LEN];      //帧数据缓冲区
    uint32  i = 0;
    uint8   CheckSum = 0;                       //校验和
    uint16  checkSumOffset = 0;
    uint16  seqOffset = 0;
    uint16  len = 0;

    memcpy(FrameBuff, inBuf, inBufLen);

    if (gPrtlLinkLen == 1)
    {
        FrameBuff[1] = inBuf[1];
        len = FrameBuff[1];
        seqOffset = 8;
    }
    else
    {
        len = *(uint16 *)&FrameBuff[1];
        seqOffset = 9;
    }

    if (inBufLen != len)
    {
        CCODOWN_FMT_DEBUG(0, "@@@@@@CcoChangeFrameSeq inBufLen=%u不等于len=%u\n", inBufLen, len);
        return 0;
    }

    CcoChangeSeq(&FrameBuff[seqOffset], appData, index);
    *seq = FrameBuff[seqOffset];
    checkSumOffset = len - 2;

    //检验和的范围为控制域+用户数据区
    for (i = (gPrtlLinkLen + 1); i < len -2; i++)
    {
        CheckSum += FrameBuff[i];
    }

    FrameBuff[checkSumOffset] = CheckSum;

    CCODOWN_FMT_DEBUG(0, "@@@@@@CcoChangeFrameSeq send seq=%u\n", *seq);
    //将数据发送到共享缓冲区
    PlcWriteQueue(pAmrSendDown[index], FrameBuff, len);
    return inBufLen;
}

/* 返回重组后报文长度 */
uint16 CcoChangeFrameSeqOnly(uint8 *inBuf, uint16 inBufLen, uint8 *FrameBuff, uint8 seq, uint8 index)
{
    //uint8   FrameBuff[MAX_QUEUE_BUFF_LEN];      //帧数据缓冲区
    
    uint32  i = 0;
    uint8   CheckSum = 0;                       //校验和
    uint16  checkSumOffset = 0;
    uint16  seqOffset = 0;
    uint16  len = 0;

    memcpy(FrameBuff, inBuf, inBufLen);

    if (gPrtlLinkLen == 1)
    {
        FrameBuff[1] = inBuf[1];
        len = FrameBuff[1];
        seqOffset = 8;
    }
    else
    {
        len = *(uint16 *)&FrameBuff[1];
        seqOffset = 9;
    }

    if (inBufLen != len)
    {
        CCODOWN_FMT_DEBUG(0, "@@@@@@CcoPackFrameDataOnly inBufLen=%u不等于len=%u\n", inBufLen, len);
        return 0;
    }

    FrameBuff[seqOffset] = seq;
    checkSumOffset = len - 2;

    //检验和的范围为控制域+用户数据区
    for (i = (gPrtlLinkLen + 1); i < len -2; i++)
    {
        CheckSum += FrameBuff[i];
    }

    FrameBuff[checkSumOffset] = CheckSum;

    CCODOWN_FMT_DEBUG(0, "@@@@@@CcoPackFrameDataOnly send seq=%u\n", seq);
    return inBufLen;
}

/*******************************************************************************
* 函数名称: PackUpMsgData
* 函数功能: 组上行消息
* 输入参数: 
* 输出参数: 无
* 返 回 值: 数据帧长度
*******************************************************************************/
extern char *appName;
uint16 CcoPackUpMsgData(char *DestAddr, uint8 RPM, uint16 IID, uint16 IOP, uint16 index, uint16 label, uint8 *payload, uint16 len)
{
    int    ret = 0;
    MSG_INFO_T *MsgSend = (MSG_INFO_T *)malloc(sizeof(MSG_INFO_T) + len);

    memset(MsgSend, 0x00, sizeof(MSG_INFO_T) + len);
    MsgSend->MsgRPM = RPM;
    MsgSend->MsgPRIORITY = 1;
    MsgSend->MsgIndex = index;
    //*MsgIndex = MsgSend->MsgIndex;
    MsgSend->MsgLabel = label;
    if (MsgSend->MsgRPM == 1)
    {
        MsgSend->SourLen = strlen(appName);
        memcpy(MsgSend->SourAddr, appName, MsgSend->SourLen);
        MsgSend->DestLen = strlen(DestAddr);
        memcpy(MsgSend->DestAddr, DestAddr, MsgSend->DestLen);
        CCOUP_FMT_DEBUG("SourLen %d SourAddr %s DestLen %d DestAddr %s \n", MsgSend->SourLen, MsgSend->SourAddr, MsgSend->DestLen, MsgSend->DestAddr);
    }
    else /* 适应mqtt发送接口源地址目的地址转换 */
    {
        MsgSend->SourLen = strlen(DestAddr);
        memcpy(MsgSend->SourAddr, DestAddr, MsgSend->SourLen);
        MsgSend->DestLen = strlen(appName);
        memcpy(MsgSend->DestAddr, appName, MsgSend->DestLen);
        CCOUP_FMT_DEBUG("--SourLen %d SourAddr %s DestLen %d DestAddr %s \n", MsgSend->DestLen, MsgSend->DestAddr, MsgSend->SourLen, MsgSend->SourAddr);
    }
    MsgSend->IID = IID;
    MsgSend->IOP = IOP;
    MsgSend->MsgLen = len;
    memcpy(MsgSend->MsgData,payload,len);
    CCOUP_FMT_DEBUG("Send IID=0X%04x IOP=0x%04x MsgIndex %d \n", IID, IOP, MsgSend->MsgIndex);

    CCOUP_BUF_DEBUG(payload, len, "@@MQTT发送报文@@:");
    ret = PlcWriteQueue(pAmrSendUp, (uint8 *)MsgSend, sizeof(MSG_INFO_T)+len);

    if (ret == 0)
    {
        CCOUP_FMT_DEBUG("CcoPackUpMsgData g_CcoRecvDown队列满!\n");
    }

    free(MsgSend);
    return sizeof(MSG_INFO_T) + len;
}

uint16 CcoPackUpTaskManagerMsgData(uint16 IOP, uint16 label, uint8 *payload, uint16 len, uint16 *msgIndex)
{
    *msgIndex = GetInfoIndex();
    return CcoPackUpMsgData("M-taskManager", 1, 0x0003, IOP, *msgIndex, label, payload, len);
}

uint16 CcoPackUpDataFwdTrans(uint16 IOP, uint16 label, uint8 *payload, uint16 len, uint16 msgIndex)
{
    return CcoPackUpMsgData("M-taskManager", 0, 0x0003, IOP, msgIndex, label, payload, len);
}

/* 数据转发376.2响应报文上报 */
uint32 CcoDataFwdSeqCheck(CCO_AMR_T *amr, uint8 *buf, uint16 bufLen, uint8 seq, uint8 index)
{
    AppData_T appData = {0};
    uint8   payload[2048] = {0};
    uint16  offset = 0;
    //uint8   frame[2048] = {0};
    uint16  frameLen = 0;
    CCO_DATA_FWD_REQ *req = &amr->req.dataFwdReq;

    if (amr->flag.dataFwdFlag == UINT_MAX && req->newSeq == seq)
    {
        payload[offset] = req->type;
        offset += 1;
        offset += set_len_offset(bufLen, &payload[offset]);

        CCODOWN_FMT_DEBUG(index, "CcoDataFwdSeqCheck bufLen=%u\n", bufLen);

        if ((bufLen <= sizeof(payload) - 5) && (CcoUnPackFrameData(buf, bufLen, &appData) == 0))
        {
             CCODOWN_FMT_DEBUG(index, "CcoDataFwdSeqCheck recv seq=%u\n", appData.RInfo[5]);
            //appData.RInfo[5] = req->seq3762; /* 恢复为数据转发携带的3762报文seq */
            frameLen = CcoChangeFrameSeqOnly(buf, bufLen, &payload[offset], req->seq3762, index);
            CCODOWN_FMT_DEBUG(index, "CcoDataFwdSeqCheck frameLen=%u\n", frameLen);
            CCODOWN_BUF_DEBUG(index, &payload[offset], frameLen, "@@重新组包报文@@:");
            if (frameLen == bufLen)
            {
                offset += frameLen;
                CcoPackUpDataFwdTrans(0x0100, req->label, payload, offset, req->index);
                amr->flag.dataFwdFlag = 0;
                CCODOWN_FMT_DEBUG(index, "CcoDataFwdSeqCheck 2\n");
                return 0;
            }
        }

        CCODOWN_FMT_DEBUG(index, "CcoDataFwdSeqCheck 3\n");

        if (bufLen > sizeof(payload) - 5)
        {
            CCODOWN_FMT_DEBUG(index, "CcoDataFwdSeqCheck bufLen=%u超大!\n", bufLen);
            bufLen = sizeof(payload) - 5;
        }

        memcpy(&payload[offset], buf, bufLen);
        offset += bufLen;
        CcoPackUpDataFwdTrans(0x0100, req->label, payload, offset, req->index);
        amr->flag.dataFwdFlag = 0;
        return 0;
    }

    return 1;
}

/* 数据透传376.2响应报文上报 */
uint32 CcoDataTransSeqCheck(CCO_AMR_T *amr, uint8 *buf, uint16 bufLen, uint8 seq, uint8 index)
{
    AppData_T appData = {0};
    uint8   payload[2048] = {0};
    uint16  offset = 0;
    //uint8   frame[2048] = {0};
    uint16  frameLen = 0;
    CCO_DATA_TRANS_REQ *req = &amr->req.dataTransReq;

    if (amr->flag.dataTransFlag == UINT_MAX && req->newSeq == seq)
    {
        offset += set_len_offset(bufLen, &payload[offset]);

        if ((bufLen <= sizeof(payload) - 4) && (CcoUnPackFrameData(buf, bufLen, &appData) == 0))
        {
            //appData.RInfo[5] = req->seq3762; /* 恢复为数据转发携带的3762报文seq */
            frameLen = CcoChangeFrameSeqOnly(buf, bufLen, &payload[offset], req->seq3762, index);

            if (frameLen == bufLen)
            {
                offset += frameLen;
                CcoPackUpDataFwdTrans(0x0101, req->label, payload, offset, req->index);
                amr->flag.dataTransFlag = 0;
                return 0;
            }
        }

        if (bufLen > sizeof(payload) - 4)
        {
            CCODOWN_FMT_DEBUG(index, "CcoDataTransSeqCheck bufLen=%u超大!\n", bufLen);
            bufLen = sizeof(payload) - 4;
        }

        memcpy(&payload[offset], buf, bufLen);
        offset += bufLen;
        CcoPackUpDataFwdTrans(0x0101, req->label, payload, offset, req->index);
        amr->flag.dataTransFlag = 0;
        return 0;
    }

    return 1;
}

/* 挑选福建376.2规约 */
uint32 CcoPickFrame(CCO_AMR_T *amr, uint8 *buf, uint16 bufLen, uint8 index)
{
    //CCODOWN_FMT_DEBUG(nArrIndex, "InstantReportTaskDeal()...start...buf_len=%d, nArrIndex=%d \n", buf_len, nArrIndex);
    CCO_QUICK_DECODE_T data = {0};
    int ret = 0;
    uint8 payload[1] = {0};
    //AppData_T  SendAppData;
    //uint16 IndexID = 0;  //偏移位
    //INFO_DATA_T SendInfoMsgData;

    if (CcoQuickDecode(buf, bufLen, &data, index) < 0)
    {
        return 1;
    }

    CCODOWN_FMT_DEBUG(index, "@@@@@@CcoPickFrame recv AFN=0x%02X,Fn=%u,seq=%u\n", data.AFN, data.Fn,
                      data.RInfo[5]);

    if (data.RInfo[4] & 0x01)
    {
        CCODOWN_FMT_DEBUG(index, "事件伴随上报\n");
        CcoEventToSmiOS(payload, 0);
    }

    if (CcoDataFwdSeqCheck(amr, buf, bufLen, data.RInfo[5], index) == 0)
    {
        return 0;
    }

    if (CcoDataTransSeqCheck(amr, buf, bufLen, data.RInfo[5], index) == 0)
    {
        return 0;
    }

    if (data.AFN == 0x02 || (data.AFN >= 0x50 && data.AFN <= 0x59)) /* 福建376.2规约 */
    {
        ret = PlcWriteQueue(&g_CcoRecvDown[index], buf, bufLen);

        if (ret == 0)
        {
            CCODOWN_FMT_DEBUG(index, "CcoPickFrame g_CcoRecvDown队列满!\n");
        }

        return 0;
    }

    return 1;
}

uint32 CcoTransAFN02F1ToMqtt(CCO_AMR_T *amr, AppData_T *appData, uint8 index)
{
    uint8   payload[2048] = {0};
    uint16  offset = 0;
    uint16  label = (amr->flag.dataFwdFlag == 1 ? amr->req.dataFwdReq.label : 0);
    uint16  msgIndex = (amr->flag.dataFwdFlag == 1 ? amr->req.dataFwdReq.index : 0);
    CCO_DATA_FWD_ACK_02F1 *p02F1 = (CCO_DATA_FWD_ACK_02F1 *)appData->DataUnitBuff;

    payload[offset] = p02F1->type;
    offset += 1;
    offset += set_len_offset(p02F1->bufLen, &payload[offset]);
    memcpy(&payload[offset], p02F1->buf, p02F1->bufLen);
    offset += p02F1->bufLen;
    
    CCOUP_FMT_DEBUG("CcoTransAFN02F1ToMqtt 数据转发响应 begin\n");
    CcoPackUpDataFwdTrans(0x0100, label, payload, offset, msgIndex);
    return 0;
}

uint32 CcoTransAFN02ToMqtt(CCO_AMR_T *amr, AppData_T *appData, uint8 index)
{
    switch (appData->Fn)
    {
        case 1:
        {
            CcoTransAFN02F1ToMqtt(amr, appData, index);
            break;
        }
        default:
        {
            break;
        }
    }

    return 0;
}

uint32 CcoTransAFN50ToMqtt(AppData_T *appData, uint8 *mqtt, uint16 mqttLen, uint8 index)
{
    return 0;
}

uint32 CcoTransAFN51ToMqtt(AppData_T *appData, uint8 *mqtt, uint16 mqttLen, uint8 index)
{
    return 0;
}

uint32 CcoTransAFN52F1ToMqtt(CCO_AMR_T *amr, AppData_T *appData, uint8 index)
{
    uint8   payload[2048] = {0};
    uint16  offset = 0;
    uint16  label = (amr->flag.dataTransFlag == 1 ? amr->req.dataTransReq.label : 0);
    uint16  msgIndex = (amr->flag.dataTransFlag == 1 ? amr->req.dataTransReq.index : 0);
    CCO_DATA_TRANS_ACK_52F1 *p52F1 = (CCO_DATA_TRANS_ACK_52F1 *)appData->DataUnitBuff;

    offset += set_len_offset(p52F1->bufLen, &payload[offset]);
    memcpy(&payload[offset], p52F1->buf, p52F1->bufLen);
    offset += p52F1->bufLen;
    
    CCOUP_FMT_DEBUG("CcoTransAFN52F1ToMqtt 数据透传响应 begin\n");
    CCOUP_BUF_DEBUG(p52F1->buf, p52F1->bufLen, "@@透传接收报文@@:");
    CcoPackUpDataFwdTrans(0x0101, label, payload, offset, msgIndex);
    return 0;
}

uint32 CcoTransAFN52ToMqtt(CCO_AMR_T *amr, AppData_T *appData, uint8 index)
{
    switch (appData->Fn)
    {
        case 1:
        {
            CcoTransAFN52F1ToMqtt(amr, appData, index);
            break;
        }
        default:
        {
            break;
        }
    }

    return 0;
}

uint32 CcoTransAFN53F4ToMqtt(CCO_AMR_T *amr, AppData_T *appData, uint8 index)
{
    uint8   payload[256] = {0};
    uint16  offset = sizeof(CCO_QUERY_NODE_INFO_ACK);
    uint16  label = (amr->flag.queryNodeInfoFlag == 1 ? amr->req.queryNodeInfoReq.label : 0);
    uint16  msgIndex = (amr->flag.queryNodeInfoFlag == 1 ? amr->req.queryNodeInfoReq.index : 0);
    CCO_QUERY_NODE_INFO_ACK_53F4 *p53F4 = (CCO_QUERY_NODE_INFO_ACK_53F4 *)appData->DataUnitBuff;
    CCO_QUERY_NODE_INFO_ACK *ack = (CCO_QUERY_NODE_INFO_ACK *)payload;

    CCOUP_FMT_DEBUG("CcoTransAFN53F4ToMqtt 查询节点信息响应 begin\n");
    memcpy_r((uint8 *)ack->manu, (uint8 *)p53F4->manu, sizeof(ack->manu));
    memcpy_r((uint8 *)ack->chip, (uint8 *)p53F4->chip, sizeof(ack->chip));
    ack->date.year = htons(Bcd2Hex(p53F4->year) + 2000);
    ack->date.month = Bcd2Hex(p53F4->month);
    ack->date.mday = Bcd2Hex(p53F4->day);
    ack->date.wday = CalWeekday(ntohs(ack->date.year), ack->date.month, ack->date.mday);
    ack->version = htons(p53F4->version);
    CcoPackUpDataFwdTrans(0x0084, label, payload, offset, msgIndex);
    amr->flag.queryNodeInfoFlag = 0;
    return 0;
}

uint32 CcoTransAFN53F5ToMqtt(CCO_AMR_T *amr, AppData_T *appData, uint8 index)
{
    uint16  label = (amr->flag.queryNodePhaseFlag == 1 ? amr->req.queryNodePhaseReq.label : 0);
    uint16  msgIndex = (amr->flag.queryNodePhaseFlag == 1 ? amr->req.queryNodePhaseReq.index : 0);
    CCO_QUERY_NODE_PHASE_ACK_53F5 *p53F5 = (CCO_QUERY_NODE_PHASE_ACK_53F5 *)appData->DataUnitBuff;
    uint16 phase = htons(p53F5->phase);

    CCOUP_FMT_DEBUG("CcoTransAFN53F5ToMqtt 查询节点相位信息响应 begin\n");
    CcoPackUpDataFwdTrans(0x0085, label, (uint8 *)&phase, sizeof(phase), msgIndex);
    amr->flag.queryNodePhaseFlag = 0;
    return 0;
}

uint32 CcoTransAFN53ToMqtt(CCO_AMR_T *amr, AppData_T *appData, uint8 index)
{
    switch (appData->Fn)
    {
        case 4:
        {
            CcoTransAFN53F4ToMqtt(amr, appData, index);
            break;
        }
        case 5:
        {
            CcoTransAFN53F5ToMqtt(amr, appData, index);
            break;
        }
        default:
        {
            break;
        }
    }
    return 0;
}

uint32 CcoTransAFN55ToMqtt(AppData_T *appData, uint8 *mqtt, uint16 mqttLen, uint8 index)
{
    return 0;
}

uint32 CcoTransAFN56F1ToMqtt(AppData_T *appData, uint8 index)
{
    uint8   payload[2048] = {0};
    uint16  offset = 0;
    uint16  msgIndex = 0;
    uint32  port = (index == 0 ? PORT_F2090201 : PORT_F2090202);
    CCO_REPORT_STA_INFO_REQ_56F1 *p56F1 = (CCO_REPORT_STA_INFO_REQ_56F1 *)appData->DataUnitBuff;
    uint32  i = 0;

    CCOUP_FMT_DEBUG("CcoTransAFN56F1ToMqtt 上报从节点信息消息 begin\n");
    Cco3762ConfirmAck(appData->RInfo[5], 1, index);
    port = htonl(port);
    memcpy(&payload[offset], &port, 4);
    offset += 4;
    payload[offset] = p56F1->num;
    offset += 1;

    for (i = 0; i < p56F1->num; i++)
    {
        memcpy_r(&payload[offset], p56F1->addr[i], CCO_ADDR_LEN);
        offset += CCO_ADDR_LEN;
    }

    CcoPackUpTaskManagerMsgData(0x0102, appData->RInfo[5], payload, offset, &msgIndex);
    return 0;
}
/* 获取未注册表计 */
#define CCO_UNREG_METER_PKT_LEN     24  /* 未注册表计报文长度 */
uint32 CcoGetUnregMeter(uint8 *buf, uint8 bufLen, uint8 *addr)
{
    MeterPrtlFmt_T data = {0};
    uint16  offset = 0;
    uint32  di = 0;

    if (UnPackMeterData(bufLen, buf, &data, &offset) == CHECK_RESULT_FAILUER)
    {
        return 1;
    }

    if (data.CtrlCode != 0x81)
    {
        return 1;
    }

    if (data.DataLen != 12)
    {
        return 1;
    }

    di = *(uint32 *)&data.DataBuff[0];

    if (di != 0xEB030101)
    {
        return 1;
    }

    if (data.DataBuff[4] != 0x01)
    {
        return 1;
    }

    memcpy_r(addr, data.MtrAddr,CCO_ADDR_LEN);
    return 0;
}

uint32 CcoTransAFN56F2ToMqtt(AppData_T *appData, uint8 index)
{
    uint8   payload[2048] = {0};
    uint16  offset = 0;
    uint16  msgIndex = 0;
    uint32  port = (index == 0 ? PORT_F2090201 : PORT_F2090202);
    uint8   addr[CCO_ADDR_LEN] = {0};
    CCO_REPORT_EVENT_56F2 *p56F2 = (CCO_REPORT_EVENT_56F2 *)appData->DataUnitBuff;

    CCOUP_FMT_DEBUG("CcoTransAFN56F2ToMqtt 上报事件数据消息 begin\n");
    Cco3762ConfirmAck(appData->RInfo[5], 1, index);
    port = htonl(port);
    memcpy(&payload[offset], &port, 4);
    offset += 4;

    if (CcoGetUnregMeter(p56F2->buf, p56F2->bufLen, addr) != 0)
    {
        payload[offset] = p56F2->type;
        offset += 1;
        memcpy_r(&payload[offset], p56F2->addr, CCO_ADDR_LEN);
        offset += CCO_ADDR_LEN;
        offset += set_len_offset(p56F2->bufLen, &payload[offset]);
        memcpy(&payload[offset], p56F2->buf, p56F2->bufLen);
        offset += p56F2->bufLen;

        //CcoPackUpTaskManagerMsgData(0x0103, appData->RInfo[5], payload, offset, &msgIndex);
        CcoEventToSmiOS(payload, offset);
    }
    else
    {
        payload[offset] = 1;
        offset += 1;
        memcpy(&payload[offset], addr, CCO_ADDR_LEN);
        offset += CCO_ADDR_LEN;
        CCOUP_FMT_DEBUG("CcoTransAFN56F2ToMqtt 修改为上报从节点信息消息 begin\n");
        CcoPackUpTaskManagerMsgData(0x0102, appData->RInfo[5], payload, offset, &msgIndex);
    }

    return 0;
}

uint32 CcoTransAFN56F3F13ToMqtt(AppData_T *appData, uint8 index)
{
    uint8   payload[2048] = {0};
    uint16  offset = 0;
    uint16  msgIndex = 0;
    uint32  port = (index == 0 ? PORT_F2090201 : PORT_F2090202);
    CCO_REPORT_METER_REQ_56F3F13 *p56F3F13 = (CCO_REPORT_METER_REQ_56F3F13 *)appData->DataUnitBuff;

    CCOUP_FMT_DEBUG("CcoTransAFN56F3F13ToMqtt 上报抄读内容请求消息 begin\n");
    port = htonl(port);
    memcpy(&payload[offset], &port, 4);
    offset += 4;
    payload[offset] = p56F3F13->type;
    offset += 1;
    memcpy_r(&payload[offset], p56F3F13->addr, CCO_ADDR_LEN);
    offset += CCO_ADDR_LEN;
    memcpy_r(&payload[offset], (uint8 *)&p56F3F13->delay, 2);
    offset += 2;
    CcoPackUpTaskManagerMsgData(0x0104, appData->RInfo[5], payload, offset, &msgIndex);
    return 0;
}

uint32 CcoTransAFN56F4ToMqtt(AppData_T *appData, uint8 index)
{
    uint8   payload[2048] = {0};
    uint16  offset = 0;
    uint16  msgIndex = 0;
    uint32  port = (index == 0 ? PORT_F2090201 : PORT_F2090202);
    CCO_REPORT_PREVIEW_OBJ_56F4 *p56F4 = (CCO_REPORT_PREVIEW_OBJ_56F4 *)appData->DataUnitBuff;

    CCOUP_FMT_DEBUG("CcoTransAFN56F4ToMqtt 上报预告的通信对象数据消息 begin\n");
    port = htonl(port);
    memcpy(&payload[offset], &port, 4);
    offset += 4;
    memcpy_r(&payload[offset], (uint8 *)&p56F4->schemeId, 2);
    offset += 2;
    memcpy_r(&payload[offset], (uint8 *)&p56F4->taskId, 2);
    offset += 2;
    payload[offset] = p56F4->type;
    offset += 1;
    memcpy_r(&payload[offset], p56F4->addr, CCO_ADDR_LEN);
    offset += CCO_ADDR_LEN;
    offset += set_len_offset(p56F4->bufLen, &payload[offset]);

    if (offset + p56F4->bufLen > sizeof(payload))
    {
        CCOUP_FMT_DEBUG("CcoTransAFN56F4ToMqtt 上报预告的通信对象数据的数据帧长度%u过长\n", p56F4->bufLen);
        return 1;
    }

    memcpy(&payload[offset], p56F4->buf, p56F4->bufLen);
    offset += p56F4->bufLen;
    CcoPackUpTaskManagerMsgData(0x0105, appData->RInfo[5], payload, offset, &msgIndex);
    return 0;
}

uint32 CcoTransAFN56F5ToMqtt(AppData_T *appData, uint8 index)
{
    uint8   payload[16] = {0};
    uint16  offset = 0;
    uint16  msgIndex = 0;
    uint32  port = (index == 0 ? PORT_F2090201 : PORT_F2090202);
    CCO_REPORT_BROAD_DELAY_56F5 *p56F5 = (CCO_REPORT_BROAD_DELAY_56F5 *)appData->DataUnitBuff;

    CCOUP_FMT_DEBUG("CcoTransAFN56F5ToMqtt 上报广播延时消息 begin\n");
    port = htonl(port);
    memcpy(&payload[offset], &port, 4);
    offset += 4;
    memcpy_r(&payload[offset], (uint8 *)&p56F5->delay, 2);
    offset += 2;
    CcoPackUpTaskManagerMsgData(0x0106, appData->RInfo[5], payload, offset, &msgIndex);
    return 0;
}

uint32 CcoTransAFN56F6ToMqtt(AppData_T *appData, uint8 index)
{
    uint8   payload[16] = {0};
    uint16  offset = 0;
    uint16  msgIndex = 0;
    uint32  port = (index == 0 ? PORT_F2090201 : PORT_F2090202);

    CCOUP_FMT_DEBUG("CcoTransAFN56F6ToMqtt 上报广播完成消息 begin\n");
    Cco3762ConfirmAck(appData->RInfo[5], 1, index);
    port = htonl(port);
    memcpy(&payload[offset], &port, 4);
    offset += 4;

    CcoPackUpTaskManagerMsgData(0x0107, appData->RInfo[5], payload, offset, &msgIndex);
    return 0;
}

uint32 CcoTransAFN56F14ToMqtt(AppData_T *appData, uint8 index)
{
    uint8   payload[2048] = {0};
    uint16  offset = 0;
    uint16  msgIndex = 0;
    uint32  port = (index == 0 ? PORT_F2090201 : PORT_F2090202);
    CCO_REPORT_PREVIEW_OBJ_56F14 *p56F14 = (CCO_REPORT_PREVIEW_OBJ_56F14 *)appData->DataUnitBuff;
    uint16  bufLen = 0;

    CCOUP_FMT_DEBUG("CcoTransAFN56F14ToMqtt 上报预告的通信对象数据消息 begin\n");
    port = htonl(port);
    memcpy(&payload[offset], &port, 4);
    offset += 4;
    memcpy_r(&payload[offset], (uint8 *)&p56F14->schemeId, 2);
    offset += 2;
    memcpy_r(&payload[offset], (uint8 *)&p56F14->taskId, 2);
    offset += 2;
    payload[offset] = p56F14->type;
    offset += 1;
    memcpy_r(&payload[offset], p56F14->addr, CCO_ADDR_LEN);
    offset += CCO_ADDR_LEN;
    bufLen = p56F14->bufLen;
    offset += set_len_offset(bufLen, &payload[offset]);

    if (offset + bufLen > sizeof(payload))
    {
        CCOUP_FMT_DEBUG("CcoTransAFN56F14ToMqtt 上报预告的通信对象数据的数据帧长度%u过长\n", bufLen);
        return 1;
    }

    memcpy(&payload[offset], p56F14->buf, bufLen);
    offset += bufLen;
    CcoPackUpTaskManagerMsgData(0x0105, appData->RInfo[5], payload, offset, &msgIndex);
    return 0;
}


uint32 CcoTransAFN56ToMqtt(CCO_AMR_T *amr, AppData_T *appData, uint8 index)
{
    switch (appData->Fn)
    {
        case 1:
        {
            CcoTransAFN56F1ToMqtt(appData, index);
            break;
        }
        case 2:
        {
            CcoTransAFN56F2ToMqtt(appData, index);
            break;
        }
        case 3:
        {
            CcoTransAFN56F3F13ToMqtt(appData, index);
            break;
        }
        case 4:
        {
            CcoTransAFN56F4ToMqtt(appData, index);
            break;
        }
        case 5:
        {
            CcoTransAFN56F5ToMqtt(appData, index);
            break;
        }
        case 6:
        {
            CcoTransAFN56F6ToMqtt(appData, index);
            break;
        }
        case 13:
        {
            CcoTransAFN56F3F13ToMqtt(appData, index);
            break;
        }
        case 14:
        {
            CcoTransAFN56F14ToMqtt(appData, index);
            break;
        }
        default:
        {
            break;
        }
    }

    return 0;
}

/* 本地模块上报的福建376.2规约转换为上报给taskManager的Mqtt消息 */
uint32 CcoTrans3762ToMqtt(CCO_AMR_T *amr, uint8 *buf, uint16 len, uint8 index)
{
    AppData_T appData = {0};

    if (CcoUnPackFrameData(buf, len, &appData) != 0)
    {
        CCODOWN_FMT_DEBUG(index, "CcoTrans3762ToMqtt CcoUnPackFrameData ret!=0\n");
        return 1;
    }

    CCODOWN_FMT_DEBUG(index, "CcoTrans3762ToMqtt recv AFN=0x%02X,Fn=%u,seq=%u\n", appData.AFN, appData.Fn,
                      appData.RInfo[5]);

    switch (appData.AFN)
    {
        case 0x02:
        {
            CcoTransAFN02ToMqtt(amr, &appData, index);
            break;
        }
        case 0x50:
        {
            break;
        }
        case 0x51:
        {
            break;
        }
        case 0x52:
        {
            CcoTransAFN52ToMqtt(amr, &appData, index);
            break;
        }
        case 0x53:
        {
            CcoTransAFN53ToMqtt(amr, &appData, index);
            break;
        }
        case 0x55:
        {
            break;
        }
        case 0x56:
        {
            CcoTransAFN56ToMqtt(amr, &appData, index);
            break;
        }
        default:
        {
            break;
        }
    }

    return 0;
}

/* 本地模块上报的福建376.2规约转换为上报给taskManager的Mqtt消息 */
uint32 CcoTrans3762ToMqttProc(CCO_AMR_T *amr, uint8 index)
{
    uint8   recvBuf[MAX_QUEUE_BUFF_LEN] = {0};        //接收缓冲区(完整报文)
    //uint8   sendBuf[MAX_QUEUE_BUFF_LEN] = {0};        //发送缓冲区
    int32   recvLen = 0;           //接收数据长度(完整报文)
    //int32   sendLen = 0;           //发送数据长度

    recvLen = PlcReadQueue(&g_CcoRecvDown[index], recvBuf);

    if (recvLen > 0)
    {
        CCODOWN_FMT_DEBUG(index, "本地模块[%u]接收 recvLen=%d \n", index, recvLen);
        CCODOWN_BUF_DEBUG(index, recvBuf, recvLen, "@@接收报文@@:");
        CcoTrans3762ToMqtt(amr, recvBuf, recvLen, index);
        memset(recvBuf, 0, recvLen);
    }

    return 0;
}

uint32 CcoFuJianAmr(CCO_AMR_T *amr, uint8 index)
{
    static uint32 cnt = 0;

    if (g_CcoMode == CCO_MODE_FUJIAN && amr->flag.initFlag != 0)
    {
        while (1)
        {
            /* 初始化流程 */
            if (CcoInitProc(amr) == 0)
            {
                amr->flag.initFlag = 0;
                //CcoQueryStaNodeInfo(amr, index);
                break;
            }
            
            sleep(5);
        }
    }

    if (cnt % 5000 == 0)
    {
        //CcoQueryStaNodeInfo(amr, index);
        CcoQueryUartPara(amr, index);
    }

    cnt++;
    CcoTrans3762ToMqttProc(amr, index);
    return 0;
}

/**********************************************************************
* @name      : comm_init_udp
* @brief     ：初始化UDP端口, 指定UDP端口为UDP_PORT
* @param[in] ：char *ifName      接口名称
* @param[out]：
* @return    ：socket
* @Create    : 王津剑
* @Date      ：2019-11-1
* @Update    :
**********************************************************************/
int CcoInitUdp(uint8 index)
{
    int    flags_block;
    int    fd_sock = -1;
    uint16 udp_port;
    int    bind_err;
    struct sockaddr_in sin;
    //struct ifreq interf;

    /*分配socket标识符, UDP协议*/
    if ((fd_sock = socket(AF_INET, SOCK_DGRAM, 0)) <= 0)
    {
        CCODOWN_FMT_DEBUG(index, "CcoInitUdp socket error\n");
        return -1;
    }

    /* socket绑定网口 */
    /*bzero(&interf, sizeof(interf));
    strncpy(interf.ifr_ifrn.ifrn_name, ifName, strlen(ifName));
    if (setsockopt(fd_sock, SOL_SOCKET, SO_BINDTODEVICE, (char *)&interf, sizeof(interf)) < 0) 
    {
        COMM_FMT_TRACE("%s SO_BINDTODEVICE failed\n", ifName);
        return -1;
    }*/
    
    /*将fd_sock 与sin结构中的地址进行绑定*/
    bzero(&sin, sizeof(sin));
    udp_port = CCO_UDP_PORT;
    sin.sin_family      = AF_INET;
    sin.sin_addr.s_addr = htonl(INADDR_ANY);
    sin.sin_port        = htons(udp_port);
    bind_err = bind(fd_sock, (struct sockaddr *)&sin, sizeof(sin));

    /*若UDP_PORT端口已经被使用了, 则重新绑定UDP_PORT+1端口, 直到端口号为5500*/
    while(bind_err < 0)
    {
        CCODOWN_FMT_DEBUG(index, "CcoInitUdp bind error\n");
        sin.sin_port = htons(++udp_port);
        bind_err = bind(fd_sock, (struct sockaddr *)&sin, sizeof(sin));
        if (udp_port > CCO_UDP_PORT_MAX)
        {
            return -1;
        }
    }

    /*绑定端口号成功后, 将fd_sock设置为非阻塞模式*/
    if (fd_sock > 0)
    {
        /* 设置SOCKET文件句柄为非阻塞模式 */
        if((flags_block = fcntl(fd_sock, F_GETFL, 0))<0) 
        {   
            CCODOWN_FMT_DEBUG(index, "CcoInitUdp fcntl ERROR get mode\n");
            close(fd_sock);
            return -1; 
        }               
        if(fcntl(fd_sock, F_SETFL, flags_block | O_NONBLOCK))
        {
            CCODOWN_FMT_DEBUG(index, "CcoInitUdp fcntl ERROR set mode\n"); 
            close(fd_sock);
            return -1;
        }
    }

    return fd_sock;
}

/**********************************************************************
* @name      : comm_eth_udpsend
* @brief     ：eth网口发消息 udp方式
* @param[in] ：int fd                      socket句柄
               uint8 *pBuffer              缓冲区
               uint16 bufLen               缓冲区大小
               COMM_ETH_HANDLE_T *ptHandle 线程句柄
* @param[out]：
* @return    ：发送长度/-1报错需要关闭通道
* @Create    : 王津剑
* @Date      ：2019-11-4
* @Update    :
**********************************************************************/
int CcoEthUdpSend(int fd, uint8 *pBuffer, uint16 bufLen, uint8 index)
{
    //uint8 protocol = -1;         /* 内部协议 */
    int    sendLen = 0;
    struct sockaddr_in address;
    uint32 masterIP = 0xC0A80102;
    uint32 masterPort = 12345;
    
    if (bufLen <= 4)
    {
        CCODOWN_FMT_DEBUG(index, "CcoEthUdpSend msg less then 4\n");
        return 0;
    }

    //protocol = comm_get_protocol(pBuffer);
    /*如果是数据中转传送、心跳帧、登陆帧、退出登陆帧, 将数据发送出去*/
    /*if(PRO_DATA_TRANSMIT != protocol && PRO_LOGIN_FRAME != protocol && PRO_EXIT_LOGIN_FRAME != protocol
        && PRO_HEART_FRAME != protocol)
    {
        return 0;  //其他方式不发送
    }*/

    bzero(&address, sizeof(address));
    address.sin_family      = AF_INET;
    address.sin_addr.s_addr = htonl(masterIP);
    address.sin_port        = htons(masterPort);
    sendLen = sendto(fd, pBuffer+4, bufLen-4, 0, (struct sockaddr *)&address, sizeof(address));
    if(sendLen < 0)
    {
        return -1;
    }
    return sendLen;
}

/**********************************************************************
* @name      : comm_eth_udprecv
* @brief     ：eth网口收消息 udp方式
* @param[in] ：int fd                      socket句柄
               uint16 bufLen               缓冲区大小
               COMM_ETH_HANDLE_T *ptHandle 线程句柄
* @param[out]：uint8 *pBuffer              缓冲区
               uint8 *pOffset              通道偏移
* @return    ：数据长度/-1 网口异常
* @Create    : 王津剑
* @Date      ：2019-12-9
* @Update    :
**********************************************************************/
int CcoEthUdpRecv(int fd, uint8 *pBuffer, uint16 bufLen)
{
    //uint8 i;
    int recvLen = 0;
    int sin_len;
    uint32 ip = 0;
    uint16 port = 0;
    struct sockaddr_in sourceAddr;
    uint32 masterIP = 0xC0A80102;
    uint32 masterPort = 12345;

    /* UDP模式 */
    sin_len = sizeof(struct sockaddr);
    recvLen = recvfrom(fd, pBuffer, bufLen, 0, (struct sockaddr *)&sourceAddr, (socklen_t *)&sin_len);
    if(recvLen > 0)
    {
        ip = ntohl(sourceAddr.sin_addr.s_addr);
        port = ntohs(sourceAddr.sin_port);
        /* UDP模式 检查ip 、port */
        if (masterIP == ip && masterPort == port)
        {
            return recvLen;
        }
    }
    return -1;
}


#endif
